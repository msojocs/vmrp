#include "./include/e2e_control.h"

#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <SDL2/SDL.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <strings.h>

#define VMRP_E2E_SOCKET_PATH_LIMIT 108

/*
 * SCREEN 是唯一需要回到主线程执行的命令——它读取 SDL window surface，必须与
 * event()/timer() 串行（见 main.c 中关于 Unicorn 引擎竞态的说明）。其余命令
 * （CLICK/KEY/WAIT_DRAW/DRAW_COUNT/QUIT）只做线程安全的 SDL_PushEvent 或对绘制
 * 计数的原子读取，可直接在控制线程上完成，无需 mutex/cond/事件往返。
 */
typedef struct {
    char path[1024];
    char response[1200];
    int done;
    SDL_mutex *mutex;
    SDL_cond *cond;
} E2eScreenRequest;

struct VmrpE2eControl {
    Uint32 event_type;
    VmrpE2eHooks hooks;
    SDL_Thread *thread;
    SDL_atomic_t stop_requested;
    int listen_fd;
    char socket_path[VMRP_E2E_SOCKET_PATH_LIMIT];
};

static void e2e_finish_screen(E2eScreenRequest *req, const char *fmt, ...) {
    char response[sizeof(req->response)];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(response, sizeof(response), fmt, ap);
    va_end(ap);

    SDL_LockMutex(req->mutex);
    snprintf(req->response, sizeof(req->response), "%s", response);
    req->done = 1;
    SDL_CondSignal(req->cond);
    SDL_UnlockMutex(req->mutex);
}

static int e2e_draw_count(VmrpE2eControl *control) {
    return control->hooks.draw_count ? control->hooks.draw_count(control->hooks.userdata) : 0;
}

static const char *e2e_screen_dump_path(VmrpE2eControl *control) {
    if (control->hooks.screen_dump_path) {
        return control->hooks.screen_dump_path(control->hooks.userdata);
    }
    return "/tmp/vmrp_screen.ppm";
}

static SDL_Keycode e2e_parse_key(const char *name) {
    if (strcasecmp(name, "ENTER") == 0 || strcasecmp(name, "SELECT") == 0) return SDLK_RETURN;
    if (strcasecmp(name, "ESC") == 0 || strcasecmp(name, "ESCAPE") == 0 || strcasecmp(name, "POWER") == 0) return SDLK_ESCAPE;
    if (strcasecmp(name, "SOFTLEFT") == 0 || strcasecmp(name, "LEFT_SOFT") == 0) return SDLK_EQUALS;
    if (strcasecmp(name, "SOFTRIGHT") == 0 || strcasecmp(name, "RIGHT_SOFT") == 0) return SDLK_MINUS;
    if (strcasecmp(name, "UP") == 0) return SDLK_UP;
    if (strcasecmp(name, "DOWN") == 0) return SDLK_DOWN;
    if (strcasecmp(name, "LEFT") == 0) return SDLK_LEFT;
    if (strcasecmp(name, "RIGHT") == 0) return SDLK_RIGHT;
    if (strcasecmp(name, "SEND") == 0) return SDLK_TAB;
    if (strcasecmp(name, "STAR") == 0 || strcmp(name, "*") == 0) return SDLK_ASTERISK;
    if (strcasecmp(name, "POUND") == 0 || strcasecmp(name, "HASH") == 0 || strcmp(name, "#") == 0) return SDLK_HASH;
    if (strlen(name) == 1) {
        if (name[0] >= '0' && name[0] <= '9') return SDLK_0 + (name[0] - '0');
        if (name[0] >= 'a' && name[0] <= 'z') return SDLK_a + (name[0] - 'a');
        if (name[0] >= 'A' && name[0] <= 'Z') return SDLK_a + (name[0] - 'A');
    }
    return SDLK_UNKNOWN;
}

static void e2e_push_mouse_button(int type, int x, int y, Uint8 state) {
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.button.type = type;
    ev.button.button = SDL_BUTTON_LEFT;
    ev.button.state = state;
    ev.button.x = x;
    ev.button.y = y;
    SDL_PushEvent(&ev);
}

static void e2e_push_key(int type, SDL_Keycode key, Uint8 state) {
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.key.type = type;
    ev.key.state = state;
    ev.key.keysym.sym = key;
    SDL_PushEvent(&ev);
}

static void e2e_push_key_with_mod(int type, SDL_Keycode key, Uint8 state, SDL_Keymod mod) {
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.key.type = type;
    ev.key.state = state;
    ev.key.keysym.sym = key;
    ev.key.keysym.mod = mod;
    SDL_PushEvent(&ev);
}

static int e2e_read_line(int fd, char *buf, size_t size) {
    size_t pos = 0;
    while (pos + 1 < size) {
        char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n == 0) break;
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (c == '\n') break;
        if (c != '\r') buf[pos++] = c;
    }
    buf[pos] = '\0';
    return (int)pos;
}

static void e2e_write_line(int fd, const char *line) {
    size_t len = strlen(line);
    while (len > 0) {
        ssize_t n = send(fd, line, len, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return;
        }
        line += n;
        len -= (size_t)n;
    }
    send(fd, "\n", 1, 0);
}

static int e2e_input_hold_ms(void) {
    const char *env = getenv("VMRP_E2E_HOLD_MS");
    int ms = env ? atoi(env) : 0;
    /* 默认按住 500ms，与 autoClickThread 一致：这是已验证可用的间隔。 */
    if (ms <= 0) ms = 500;
    return ms;
}

/*
 * 输入（点击/按键）不能在主线程上同步地"按下+松开"。主循环依赖 SDL_WaitEvent
 * 阻塞，定时器（timerCb 推送的 timerEventType）只有在两次事件之间空闲运行时
 * 才会被处理；而真实应用是由定时器循环驱动的状态机，需要在 down 与 up 之间
 * 观察到"按下"状态才会识别为一次输入。若 down/up 背靠背执行（中间没有定时器
 * tick），应用往往不会产生事件。
 *
 * 因此在控制线程上注入 down/延迟/up（与 autoClickThread 的做法一致）：
 * SDL_PushEvent 是线程安全的，主循环会在间隔期间继续处理定时器并重绘，
 * 实际的 event(MR_MOUSE_*)/event(MR_KEY_*) 仍然发生在主线程。编辑模式下的
 * 抑制也由主循环统一处理。
 */
static void e2e_inject_click(int x, int y, int fd) {
    e2e_push_mouse_button(SDL_MOUSEBUTTONDOWN, x, y, SDL_PRESSED);
    SDL_Delay((Uint32)e2e_input_hold_ms());
    e2e_push_mouse_button(SDL_MOUSEBUTTONUP, x, y, SDL_RELEASED);
    char resp[64];
    snprintf(resp, sizeof(resp), "OK click %d %d", x, y);
    e2e_write_line(fd, resp);
}

/*
 * hold_ms<=0 时使用全局默认（VMRP_E2E_HOLD_MS 或 500ms）。
 * Mythroad 应用自身以定时器轮询按键状态实现长按/重复：按住时长直接决定
 * 应用语义（短按=单步/激活，长按=按键重复或弹出长按菜单）。全局调大
 * VMRP_E2E_HOLD_MS（如为剪贴板粘贴稳定性设 1500ms）会让所有按键都变成
 * 长按，导致方向键连滚、确认键弹菜单。因此 KEY 命令支持按次覆盖时长。
 */
static void e2e_inject_key(SDL_Keycode key, int hold_ms, int fd) {
    if (hold_ms <= 0) hold_ms = e2e_input_hold_ms();
    e2e_push_key(SDL_KEYDOWN, key, SDL_PRESSED);
    SDL_Delay((Uint32)hold_ms);
    e2e_push_key(SDL_KEYUP, key, SDL_RELEASED);
    char resp[64];
    snprintf(resp, sizeof(resp), "OK key %d", (int)key);
    e2e_write_line(fd, resp);
}

static void e2e_inject_paste(const char *text, int fd) {
    if (SDL_SetClipboardText(text ? text : "") != 0) {
        e2e_write_line(fd, "ERR clipboard");
        return;
    }
    /* Ctrl+V is a platform-edit action, not a Mythroad key.  Carry the Ctrl
     * modifier in the SDL event and mirror it into SDL's mod state so the main
     * edit-mode branch observes the same condition as a real keyboard paste. */
    SDL_Keymod old_mod = SDL_GetModState();
    SDL_SetModState((SDL_Keymod)(old_mod | KMOD_CTRL));
    e2e_push_key_with_mod(SDL_KEYDOWN, SDLK_v, SDL_PRESSED, KMOD_CTRL);
    SDL_Delay((Uint32)e2e_input_hold_ms());
    e2e_push_key_with_mod(SDL_KEYUP, SDLK_v, SDL_RELEASED, KMOD_CTRL);
    SDL_Delay(1);
    SDL_SetModState(old_mod);
    e2e_write_line(fd, "OK paste");
}

static void e2e_handle_wait_draw(VmrpE2eControl *control, int draw_count, int timeout_ms, int fd) {
    Uint32 start = SDL_GetTicks();
    char resp[128];
    while (e2e_draw_count(control) <= draw_count) {
        if ((int)(SDL_GetTicks() - start) >= timeout_ms) {
            snprintf(resp, sizeof(resp), "ERR wait_draw_timeout current=%d target>%d",
                     e2e_draw_count(control), draw_count);
            e2e_write_line(fd, resp);
            return;
        }
        SDL_Delay(5);
    }
    snprintf(resp, sizeof(resp), "OK draw_count %d", e2e_draw_count(control));
    e2e_write_line(fd, resp);
}

/* SCREEN 必须在主线程读取 surface：通过自定义事件把请求投递给主循环
 * （vmrp_e2e_control_execute），用条件变量等待完成后再回写响应。这是唯一仍需
 * mutex/cond 往返的命令。 */
static void e2e_handle_screen(VmrpE2eControl *control, const char *path, int fd) {
    E2eScreenRequest req;
    memset(&req, 0, sizeof(req));
    req.mutex = SDL_CreateMutex();
    req.cond = SDL_CreateCond();
    if (!req.mutex || !req.cond) {
        if (req.cond) SDL_DestroyCond(req.cond);
        if (req.mutex) SDL_DestroyMutex(req.mutex);
        e2e_write_line(fd, "ERR sync_init_failed");
        return;
    }
    snprintf(req.path, sizeof(req.path), "%s", path);

    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = control->event_type;
    ev.user.data1 = control;
    ev.user.data2 = &req;

    SDL_LockMutex(req.mutex);
    if (SDL_PushEvent(&ev) != 1) {
        SDL_UnlockMutex(req.mutex);
        e2e_write_line(fd, "ERR push_event_failed");
    } else {
        while (!req.done) SDL_CondWait(req.cond, req.mutex);
        SDL_UnlockMutex(req.mutex);
        e2e_write_line(fd, req.response);
    }
    SDL_DestroyCond(req.cond);
    SDL_DestroyMutex(req.mutex);
}

static void e2e_handle_client(VmrpE2eControl *control, int fd) {
    char line[2048];
    if (e2e_read_line(fd, line, sizeof(line)) <= 0) {
        e2e_write_line(fd, "ERR empty_command");
        return;
    }

    char op[32] = {0};
    char a[1024] = {0};
    char b[64] = {0};
    if (sscanf(line, "%31s %1023s %63s", op, a, b) < 1) {
        e2e_write_line(fd, "ERR bad_command");
        return;
    }

    if (strcasecmp(op, "CLICK") == 0) {
        int x, y;
        if (sscanf(line, "%*31s %d %d", &x, &y) != 2) {
            e2e_write_line(fd, "ERR usage");
            return;
        }
        e2e_inject_click(x, y, fd);
    } else if (strcasecmp(op, "KEY") == 0) {
        SDL_Keycode key = a[0] ? e2e_parse_key(a) : SDLK_UNKNOWN;
        if (key == SDLK_UNKNOWN) {
            e2e_write_line(fd, "ERR usage");
            return;
        }
        /* 可选第二参数 = 本次按住毫秒数（覆盖 VMRP_E2E_HOLD_MS） */
        e2e_inject_key(key, b[0] ? atoi(b) : 0, fd);
    } else if (strcasecmp(op, "PASTE") == 0) {
        const char *text = line + strlen(op);
        while (*text == ' ' || *text == '\t') text++;
        e2e_inject_paste(text, fd);
    } else if (strcasecmp(op, "WAIT_DRAW") == 0) {
        int draw_count, timeout_ms;
        if (b[0]) {
            draw_count = atoi(a);
            timeout_ms = atoi(b);
        } else {
            draw_count = e2e_draw_count(control);
            timeout_ms = a[0] ? atoi(a) : 2000;
        }
        if (draw_count < 0) draw_count = 0;
        if (timeout_ms <= 0) timeout_ms = 2000;
        e2e_handle_wait_draw(control, draw_count, timeout_ms, fd);
    } else if (strcasecmp(op, "DRAW_COUNT") == 0) {
        char resp[64];
        snprintf(resp, sizeof(resp), "OK draw_count %d", e2e_draw_count(control));
        e2e_write_line(fd, resp);
    } else if (strcasecmp(op, "QUIT") == 0) {
        SDL_Event ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = SDL_QUIT;
        SDL_PushEvent(&ev);
        e2e_write_line(fd, "OK quit");
    } else if (strcasecmp(op, "SCREEN") == 0) {
        e2e_handle_screen(control, a[0] ? a : e2e_screen_dump_path(control), fd);
    } else {
        e2e_write_line(fd, "ERR usage");
    }
}

static int e2e_control_thread(void *data) {
    VmrpE2eControl *control = (VmrpE2eControl *)data;
    while (!SDL_AtomicGet(&control->stop_requested)) {
        int fd = accept(control->listen_fd, NULL, NULL);
        if (fd < 0) {
            if (errno == EINTR) continue;
            if (SDL_AtomicGet(&control->stop_requested)) break;
            SDL_Delay(10);
            continue;
        }
        e2e_handle_client(control, fd);
        close(fd);
    }
    return 0;
}

int vmrp_e2e_control_available(void) {
    return 1;
}

VmrpE2eControl *vmrp_e2e_control_create(uint32_t event_type, const VmrpE2eHooks *hooks) {
    VmrpE2eControl *control = (VmrpE2eControl *)calloc(1, sizeof(VmrpE2eControl));
    if (!control) return NULL;
    control->event_type = event_type;
    control->listen_fd = -1;
    if (hooks) control->hooks = *hooks;
    return control;
}

void vmrp_e2e_control_start_if_requested(VmrpE2eControl *control) {
    if (!control) return;
    if (control->thread || control->listen_fd >= 0) return;
    const char *path = getenv("VMRP_E2E_SOCKET");
    if (!path || !*path) return;

    if (strlen(path) >= sizeof(control->socket_path)) {
        fprintf(stderr, "[E2E] socket path too long: %s\n", path);
        return;
    }
    snprintf(control->socket_path, sizeof(control->socket_path), "%s", path);

    control->listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (control->listen_fd < 0) {
        perror("[E2E] socket");
        return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", control->socket_path);
    unlink(control->socket_path);
    if (bind(control->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("[E2E] bind");
        close(control->listen_fd);
        control->listen_fd = -1;
        return;
    }
    if (listen(control->listen_fd, 8) != 0) {
        perror("[E2E] listen");
        close(control->listen_fd);
        unlink(control->socket_path);
        control->listen_fd = -1;
        return;
    }

    SDL_AtomicSet(&control->stop_requested, 0);
    control->thread = SDL_CreateThread(e2e_control_thread, "e2e-control", control);
    if (!control->thread) {
        fprintf(stderr, "[E2E] SDL_CreateThread failed: %s\n", SDL_GetError());
        close(control->listen_fd);
        unlink(control->socket_path);
        control->listen_fd = -1;
        return;
    }
    printf("[E2E] listening on %s\n", control->socket_path);
    fflush(stdout);
}

/* 在主线程上执行 SCREEN：唯一需要回到主循环的命令（读取 SDL surface）。 */
void vmrp_e2e_control_execute(VmrpE2eControl *control, SDL_Event *event) {
    if (!control || !event) return;
    E2eScreenRequest *req = (E2eScreenRequest *)event->user.data2;
    if (!req) return;

    if (control->hooks.dump_screen_ppm &&
        control->hooks.dump_screen_ppm(req->path, control->hooks.userdata) == 0) {
        e2e_finish_screen(req, "OK screen %s", req->path);
    } else {
        e2e_finish_screen(req, "ERR screen_failed %s", req->path);
    }
}

void vmrp_e2e_control_destroy(VmrpE2eControl *control) {
    if (!control) return;
    if (control->listen_fd >= 0) {
        SDL_AtomicSet(&control->stop_requested, 1);
        shutdown(control->listen_fd, SHUT_RDWR);
        close(control->listen_fd);
        control->listen_fd = -1;
    }
    if (control->thread) {
        SDL_WaitThread(control->thread, NULL);
        control->thread = NULL;
    }
    if (control->socket_path[0]) {
        unlink(control->socket_path);
        control->socket_path[0] = '\0';
    }
    free(control);
}

#else

struct VmrpE2eControl {
    int unused;
};

int vmrp_e2e_control_available(void) {
    return 0;
}

VmrpE2eControl *vmrp_e2e_control_create(uint32_t event_type, const VmrpE2eHooks *hooks) {
    (void)event_type;
    (void)hooks;
    return NULL;
}

void vmrp_e2e_control_start_if_requested(VmrpE2eControl *control) {
    (void)control;
}

void vmrp_e2e_control_execute(VmrpE2eControl *control, SDL_Event *event) {
    (void)control;
    (void)event;
}

void vmrp_e2e_control_destroy(VmrpE2eControl *control) {
    (void)control;
}

#endif
