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

#define E2E_SOCKET_PATH_LIMIT 108
#define E2E_DEFAULT_HOLD_MS 500
#define E2E_DEFAULT_KEY_SYNC_TIMEOUT_MS 30000

/*
 * SCREEN 必须回主线程读取 SDL surface。KEY 由控制线程注入，但每个 DOWN/UP
 * 都等待主线程确认，并按 timer generation 建立 guest 调度边界；两条路径都
 * 不能只把 SDL_PushEvent 成功当成 guest 已经完成处理。
 */
typedef struct {
    char path[1024];
    char response[1200];
    int draw_count;
    /* MOTION 命令复用同一主线程回投通道:is_motion 置位时忽略 path/
     * draw_count,主线程调用 hooks.motion_input(mx,my,mz) 注入动感样本。 */
    int is_motion;
    int32_t mx;
    int32_t my;
    int32_t mz;
    int done;
    SDL_mutex *mutex;
    SDL_cond *cond;
} E2eScreenRequest;

struct E2eControl {
    Uint32 event_type;
    VmrpE2eHooks hooks;
    SDL_Thread *thread;
    SDL_atomic_t stop_requested;
    int listen_fd;
    char socket_path[E2E_SOCKET_PATH_LIMIT];
    /* 控制线程一次只处理一个客户端；这组状态将当前按键事件与主线程确认配对。 */
    SDL_mutex *key_mutex;
    SDL_cond *key_cond;
    int key_event_waiting;
    int key_event_consumed;
    int key_event_delivered;
    Uint32 key_event_type;
    SDL_Keycode key_event_code;
    uint32_t key_event_token;
    uint32_t key_event_timer_arm_generation;
    uint32_t key_event_timer_pending_generation;
    /* 默认短按由主线程在首个后续 guest timer 后闭环 release。 */
    int short_key_active;
    int short_key_down_consumed;
    int short_key_down_delivered;
    int short_key_up_ready;
    int short_key_up_consumed;
    int short_key_up_delivered;
    SDL_Keycode short_key_code;
    uint32_t short_key_token;
    uint32_t short_key_release_arm_generation;
    uint32_t short_key_release_pending_generation;
    uint32_t next_key_token;
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

static int e2e_draw_count(E2eControl *control) {
    return control->hooks.draw_count ? control->hooks.draw_count(control->hooks.userdata) : 0;
}

static uint32_t e2e_timer_arm_generation(E2eControl *control) {
    return control->hooks.timer_arm_generation
        ? control->hooks.timer_arm_generation(control->hooks.userdata) : 0;
}

static uint32_t e2e_timer_dispatched_generation(E2eControl *control) {
    return control->hooks.timer_dispatched_generation
        ? control->hooks.timer_dispatched_generation(control->hooks.userdata) : 0;
}

static uint32_t e2e_timer_pending_generation(E2eControl *control) {
    return control->hooks.timer_pending_generation
        ? control->hooks.timer_pending_generation(control->hooks.userdata) : 0;
}

static int e2e_timer_dispatch_in_progress(E2eControl *control) {
    return control->hooks.timer_dispatch_in_progress
        ? control->hooks.timer_dispatch_in_progress(control->hooks.userdata) : 0;
}

static int e2e_runtime_exited(E2eControl *control) {
    return control->hooks.runtime_exited
        ? control->hooks.runtime_exited(control->hooks.userdata) : 0;
}

static const char *e2e_screen_dump_path(E2eControl *control) {
    if (control->hooks.screen_dump_path) {
        return control->hooks.screen_dump_path(control->hooks.userdata);
    }
    return "/tmp/skyengine_screen.ppm";
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

static uint32_t e2e_next_key_token(E2eControl *control) {
    control->next_key_token++;
    if (!control->next_key_token) control->next_key_token++;
    return control->next_key_token;
}

static int e2e_push_key(int type, SDL_Keycode key, Uint8 state,
                        uint32_t token) {
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.key.type = type;
    /* SDL_PushEvent 会用当前 tick 覆盖 timestamp。windowID 标记合成事件，
     * 本项目不解释的 scancode 保存 token，主线程处理后可原样回执。 */
    ev.key.windowID = E2E_KEY_WINDOW_ID;
    ev.key.keysym.scancode = (SDL_Scancode)token;
    ev.key.state = state;
    ev.key.keysym.sym = key;
    return SDL_PushEvent(&ev);
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

static int e2e_click_or_paste_hold_ms(void) {
    const char *env = getenv("SKYENGINE_E2E_HOLD_MS");
    int ms = env ? atoi(env) : 0;
    /* 点击和平台粘贴沿用已验证的 500ms 默认值。 */
    if (ms <= 0) ms = E2E_DEFAULT_HOLD_MS;
    return ms;
}

static int e2e_configured_key_hold_ms(const char *command_value) {
    if (command_value && *command_value) {
        int ms = atoi(command_value);
        /* 命令显式传 0 表示逻辑短按，不再回落到环境变量。 */
        return ms > 0 ? ms : 0;
    }
    const char *env = getenv("SKYENGINE_E2E_KEY_HOLD_MS");
    if (env && *env) {
        int ms = atoi(env);
        if (ms > 0) return ms;
    }
    /* 0 表示由 guest timer generation 定义离散短按，不引入固定墙钟时长。 */
    return 0;
}

/*
 * 输入（点击/按键）不能在主线程上同步地"按下+松开"。主循环依赖 SDL_WaitEvent
 * 阻塞，定时器（timerCb 推送的 timerEventType）只有在两次事件之间空闲运行时
 * 才会被处理；而真实应用是由定时器循环驱动的状态机，需要在 down 与 up 之间
 * 观察到"按下"状态才会识别为一次输入。若 down/up 背靠背执行（中间没有定时器
 * tick），应用往往不会产生事件。
 *
 * 因此控制线程只负责注入，按键由 generation 边界分隔，点击沿用物理 hold；
 * SDL_PushEvent 是线程安全的，实际的 event(MR_MOUSE_*)/event(MR_KEY_*) 仍然
 * 发生在主线程，编辑模式下的抑制也由主循环统一处理。
 */
static void e2e_inject_click(int x, int y, int fd) {
    e2e_push_mouse_button(SDL_MOUSEBUTTONDOWN, x, y, SDL_PRESSED);
    SDL_Delay((Uint32)e2e_click_or_paste_hold_ms());
    e2e_push_mouse_button(SDL_MOUSEBUTTONUP, x, y, SDL_RELEASED);
    char resp[64];
    snprintf(resp, sizeof(resp), "OK click %d %d", x, y);
    e2e_write_line(fd, resp);
}

static int e2e_push_key_and_wait(E2eControl *control, Uint32 type,
                                 SDL_Keycode key, Uint8 state,
                                 uint32_t *timer_arm_generation,
                                 uint32_t *timer_pending_generation) {
    SDL_LockMutex(control->key_mutex);
    if (SDL_AtomicGet(&control->stop_requested)) {
        SDL_UnlockMutex(control->key_mutex);
        return 0;
    }
    control->key_event_type = type;
    control->key_event_code = key;
    control->key_event_token = e2e_next_key_token(control);
    control->key_event_waiting = 1;
    control->key_event_consumed = 0;
    control->key_event_delivered = 0;
    if (e2e_push_key((int)type, key, state,
                     control->key_event_token) != 1) {
        control->key_event_waiting = 0;
        SDL_UnlockMutex(control->key_mutex);
        return 0;
    }
    while (!control->key_event_consumed &&
           !e2e_runtime_exited(control) &&
           !SDL_AtomicGet(&control->stop_requested)) {
        SDL_CondWait(control->key_cond, control->key_mutex);
    }
    int delivered = control->key_event_consumed && control->key_event_delivered;
    if (delivered) {
        *timer_arm_generation = control->key_event_timer_arm_generation;
        *timer_pending_generation = control->key_event_timer_pending_generation;
    }
    control->key_event_waiting = 0;
    SDL_UnlockMutex(control->key_mutex);
    return delivered;
}

/*
 * 默认短按在 KEYDOWN 入队前登记。主线程不必等待控制线程醒来：它在首个
 * 后续 timer dispatch 结束时取得 short_key_code 并同步执行 KEYUP。
 */
static int e2e_inject_short_key(E2eControl *control, SDL_Keycode key,
                                uint32_t *release_arm_generation,
                                uint32_t *release_pending_generation) {
    SDL_LockMutex(control->key_mutex);
    if (SDL_AtomicGet(&control->stop_requested)) {
        SDL_UnlockMutex(control->key_mutex);
        return 0;
    }
    control->short_key_active = 1;
    control->short_key_down_consumed = 0;
    control->short_key_down_delivered = 0;
    control->short_key_up_ready = 0;
    control->short_key_up_consumed = 0;
    control->short_key_up_delivered = 0;
    control->short_key_code = key;
    control->short_key_token = e2e_next_key_token(control);
    if (e2e_push_key(SDL_KEYDOWN, key, SDL_PRESSED,
                     control->short_key_token) != 1) {
        control->short_key_active = 0;
        SDL_UnlockMutex(control->key_mutex);
        return 0;
    }

    while (!control->short_key_down_consumed &&
           !SDL_AtomicGet(&control->stop_requested)) {
        SDL_CondWait(control->key_cond, control->key_mutex);
    }
    if (!control->short_key_down_consumed ||
        !control->short_key_down_delivered) {
        control->short_key_active = 0;
        SDL_UnlockMutex(control->key_mutex);
        return 0;
    }
    if (e2e_runtime_exited(control)) {
        control->short_key_active = 0;
        SDL_UnlockMutex(control->key_mutex);
        return 2;
    }

    while (!control->short_key_up_consumed &&
           !e2e_runtime_exited(control) &&
           !SDL_AtomicGet(&control->stop_requested)) {
        SDL_CondWait(control->key_cond, control->key_mutex);
    }

    if (e2e_runtime_exited(control)) {
        control->short_key_active = 0;
        SDL_UnlockMutex(control->key_mutex);
        return 2;
    }

    int delivered = control->short_key_up_consumed &&
                    control->short_key_up_delivered;
    if (delivered) {
        *release_arm_generation = control->short_key_release_arm_generation;
        *release_pending_generation = control->short_key_release_pending_generation;
    }
    control->short_key_active = 0;
    SDL_UnlockMutex(control->key_mutex);
    return delivered;
}

static int e2e_generation_after(uint32_t lhs, uint32_t rhs) {
    return (int32_t)(lhs - rhs) > 0;
}

static int e2e_wait_timer_boundary(E2eControl *control,
                                   uint32_t arm_generation,
                                   uint32_t pending_generation,
                                   int timeout_ms) {
    if (!pending_generation || e2e_runtime_exited(control)) return 1;
    Uint32 started_at = SDL_GetTicks();
    while (!SDL_AtomicGet(&control->stop_requested) &&
           !e2e_runtime_exited(control)) {
        /*
         * 必须严格晚于按键回执时的 arm generation。等于快照的 timer 可能早在
         * 按键前就已排队；只有后续重新 arm 的一代完整返回，才能证明 guest
         * 输入状态机跨过了一个属于该按键之后的完整周期。
         */
        if (e2e_generation_after(e2e_timer_dispatched_generation(control),
                                 arm_generation)) {
            return 1;
        }
        /*
         * timer() 内部可能短暂 stop 后再 start。仅当两次采样都处于完整 dispatch
         * 之外且 pending 仍为 0，才把它认作真实停止或 arm/push 失败。
         */
        if (!e2e_timer_dispatch_in_progress(control) &&
            !e2e_timer_pending_generation(control) &&
            !e2e_timer_dispatch_in_progress(control) &&
            !e2e_timer_pending_generation(control)) {
            return 1;
        }
        /* 超时是显式错误终态；按键已 release，不会以成功继续后续步骤。 */
        if ((int)(SDL_GetTicks() - started_at) >= timeout_ms) return 0;
        SDL_Delay(1);
    }
    return e2e_runtime_exited(control);
}

/*
 * 未显式配置 hold 时，主线程在 KEYDOWN 后第一个完整 timer dispatch 的尾部
 * 同步执行 KEYUP，确保短按只跨一个 guest tick。命令参数或环境变量指定的
 * hold 仍表示物理长按时长；两条路径都保留 release 后的 generation 边界。
 */
static void e2e_inject_key(E2eControl *control, SDL_Keycode key,
                           const char *hold_value, int sync_timeout_ms, int fd) {
    uint32_t timer_arm_generation = 0;
    uint32_t timer_pending_generation = 0;
    int hold_ms = e2e_configured_key_hold_ms(hold_value);
    int runtime_exited = 0;
    if (hold_ms > 0) {
        if (!e2e_push_key_and_wait(control, SDL_KEYDOWN, key, SDL_PRESSED,
                                   &timer_arm_generation,
                                   &timer_pending_generation)) {
            if (e2e_runtime_exited(control)) goto respond;
            e2e_write_line(fd, "ERR key_down_not_consumed");
            return;
        }
        runtime_exited = e2e_runtime_exited(control);
        if (!runtime_exited) {
            SDL_Delay((Uint32)hold_ms);
            runtime_exited = e2e_runtime_exited(control);
        }
        if (!runtime_exited &&
            !e2e_push_key_and_wait(control, SDL_KEYUP, key, SDL_RELEASED,
                                   &timer_arm_generation,
                                   &timer_pending_generation)) {
            if (e2e_runtime_exited(control)) goto respond;
            e2e_write_line(fd, "ERR key_up_not_consumed");
            return;
        }
    } else {
        int short_result = e2e_inject_short_key(
            control, key, &timer_arm_generation,
            &timer_pending_generation);
        if (!short_result) {
            e2e_write_line(fd, "ERR short_key_not_consumed");
            return;
        }
        runtime_exited = short_result == 2 || e2e_runtime_exited(control);
    }

    runtime_exited = runtime_exited || e2e_runtime_exited(control);
    if (!runtime_exited &&
        !e2e_wait_timer_boundary(control, timer_arm_generation,
                                 timer_pending_generation,
                                 sync_timeout_ms)) {
        e2e_write_line(fd, "ERR key_up_sync_timeout");
        return;
    }
respond:
    runtime_exited = e2e_runtime_exited(control);
    /*
     * 响应只表示上述输入契约完成；画面变化仍由独立 WAIT_DRAW 验证，不能把
     * timer generation 当成绘制成功。
     */
    char resp[64];
    snprintf(resp, sizeof(resp), runtime_exited
             ? "OK key %d exited" : "OK key %d", (int)key);
    e2e_write_line(fd, resp);
}

static int e2e_set_clipboard(const char *text) {
    if (SDL_SetClipboardText(text ? text : "") != 0) {
        return 0;
    }
    return 1;
}

static void e2e_inject_paste_shortcut(int fd) {
    /* Ctrl+V is a platform-edit action, not a Mythroad key.  Carry the Ctrl
     * modifier in the SDL event and mirror it into SDL's mod state so the main
     * edit-mode branch observes the same condition as a real keyboard paste. */
    SDL_Keymod old_mod = SDL_GetModState();
    SDL_SetModState((SDL_Keymod)(old_mod | KMOD_CTRL));
    e2e_push_key_with_mod(SDL_KEYDOWN, SDLK_v, SDL_PRESSED, KMOD_CTRL);
    SDL_Delay((Uint32)e2e_click_or_paste_hold_ms());
    e2e_push_key_with_mod(SDL_KEYUP, SDLK_v, SDL_RELEASED, KMOD_CTRL);
    SDL_Delay(1);
    SDL_SetModState(old_mod);
    e2e_write_line(fd, "OK paste");
}

static void e2e_inject_paste(const char *text, int fd) {
    if (!e2e_set_clipboard(text)) {
        e2e_write_line(fd, "ERR clipboard");
        return;
    }
    e2e_inject_paste_shortcut(fd);
}

static void e2e_handle_wait_draw(E2eControl *control, int draw_count, int timeout_ms, int fd) {
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
 * （e2e_control_execute），用条件变量等待完成后再回写响应。这是唯一仍需
 * mutex/cond 往返的命令。 */
static void e2e_handle_screen(E2eControl *control, const char *path,
                              int draw_count, int fd) {
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
    req.draw_count = draw_count;

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

/* MOTION 与 SCREEN 一样必须回主线程执行(guest 事件入口非线程安全),
 * 复用同一自定义事件回投+条件变量等待通道。 */
static void e2e_handle_motion(E2eControl *control,
                              int32_t x, int32_t y, int32_t z, int fd) {
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
    req.is_motion = 1;
    req.mx = x;
    req.my = y;
    req.mz = z;

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

static void e2e_handle_client(E2eControl *control, int fd) {
    char line[2048];
    if (e2e_read_line(fd, line, sizeof(line)) <= 0) {
        e2e_write_line(fd, "ERR empty_command");
        return;
    }

    char op[32] = {0};
    char a[1024] = {0};
    char b[64] = {0};
    char c[64] = {0};
    if (sscanf(line, "%31s %1023s %63s %63s", op, a, b, c) < 1) {
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
        int sync_timeout_ms = c[0] ? atoi(c) : E2E_DEFAULT_KEY_SYNC_TIMEOUT_MS;
        if (sync_timeout_ms <= 0) sync_timeout_ms = E2E_DEFAULT_KEY_SYNC_TIMEOUT_MS;
        /* 第二参数是物理 hold，第三参数只限制 release 后同步等待并在超时时报错。 */
        e2e_inject_key(control, key, b, sync_timeout_ms, fd);
    } else if (strcasecmp(op, "SET_CLIPBOARD") == 0) {
        const char *text = line + strlen(op);
        while (*text == ' ' || *text == '\t') text++;
        e2e_write_line(fd, e2e_set_clipboard(text)
                               ? "OK clipboard" : "ERR clipboard");
    } else if (strcasecmp(op, "PASTE_SHORTCUT") == 0) {
        e2e_inject_paste_shortcut(fd);
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
    } else if (strcasecmp(op, "MOTION") == 0) {
        int x, y, z;
        if (sscanf(line, "%*31s %d %d %d", &x, &y, &z) != 3) {
            e2e_write_line(fd, "ERR usage");
            return;
        }
        e2e_handle_motion(control, (int32_t)x, (int32_t)y, (int32_t)z, fd);
    } else if (strcasecmp(op, "SCREEN") == 0) {
        e2e_handle_screen(control, a[0] ? a : e2e_screen_dump_path(control),
                          0, fd);
    } else if (strcasecmp(op, "SCREEN_DRAW") == 0) {
        int draw_count = atoi(a);
        if (draw_count <= 0 || !b[0]) {
            e2e_write_line(fd, "ERR usage");
            return;
        }
        e2e_handle_screen(control, b, draw_count, fd);
    } else {
        e2e_write_line(fd, "ERR usage");
    }
}

static int e2e_control_thread(void *data) {
    E2eControl *control = (E2eControl *)data;
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

int e2e_control_available(void) {
    return 1;
}

E2eControl *e2e_control_create(uint32_t event_type, const VmrpE2eHooks *hooks) {
    E2eControl *control = (E2eControl *)calloc(1, sizeof(E2eControl));
    if (!control) return NULL;
    control->key_mutex = SDL_CreateMutex();
    control->key_cond = SDL_CreateCond();
    if (!control->key_mutex || !control->key_cond) {
        if (control->key_cond) SDL_DestroyCond(control->key_cond);
        if (control->key_mutex) SDL_DestroyMutex(control->key_mutex);
        free(control);
        return NULL;
    }
    control->event_type = event_type;
    control->listen_fd = -1;
    if (hooks) control->hooks = *hooks;
    return control;
}

void e2e_control_start_if_requested(E2eControl *control) {
    if (!control) return;
    if (control->thread || control->listen_fd >= 0) return;
    const char *path = getenv("SKYENGINE_E2E_SOCKET");
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

/* 在主线程上执行 SCREEN/MOTION：SCREEN 读取 SDL surface，MOTION 调用
 * guest 事件入口，两者都必须在 VM 所在的主循环线程执行。 */
void e2e_control_execute(E2eControl *control, SDL_Event *event) {
    if (!control || !event) return;
    E2eScreenRequest *req = (E2eScreenRequest *)event->user.data2;
    if (!req) return;

    if (req->is_motion) {
        if (control->hooks.motion_input) {
            int ret = control->hooks.motion_input(req->mx, req->my, req->mz,
                                                  control->hooks.userdata);
            /* MR_IGNORE(1) 表示 guest 未开启动感监听,回报给测试脚本区分 */
            e2e_finish_screen(req, ret == 0 ? "OK motion delivered"
                                            : "OK motion ignored");
        } else {
            e2e_finish_screen(req, "ERR motion_unsupported");
        }
        return;
    }

    if (req->draw_count > 0) {
        if (control->hooks.dump_draw_frame_ppm &&
            control->hooks.dump_draw_frame_ppm(
                req->draw_count, req->path, control->hooks.userdata) == 0) {
            e2e_finish_screen(req, "OK screen_draw %d %s",
                              req->draw_count, req->path);
        } else {
            e2e_finish_screen(req, "ERR screen_draw_failed %d %s",
                              req->draw_count, req->path);
        }
    } else if (control->hooks.dump_screen_ppm &&
        control->hooks.dump_screen_ppm(req->path, control->hooks.userdata) == 0) {
        e2e_finish_screen(req, "OK screen %s", req->path);
    } else {
        e2e_finish_screen(req, "ERR screen_failed %s", req->path);
    }
}

void e2e_control_key_event_completed(E2eControl *control,
                                          uint32_t event_type,
                                          int32_t keycode,
                                          uint32_t window_id,
                                          uint32_t token,
                                          int delivered) {
    if (!control || window_id != E2E_KEY_WINDOW_ID || !token) return;
    SDL_LockMutex(control->key_mutex);
    if (control->short_key_active &&
        control->short_key_code == (SDL_Keycode)keycode &&
        control->short_key_token == token) {
        if (event_type == SDL_KEYDOWN && !control->short_key_down_consumed) {
            control->short_key_down_consumed = 1;
            control->short_key_down_delivered = delivered;
            SDL_CondSignal(control->key_cond);
        } else if (event_type == SDL_KEYUP &&
                   control->short_key_up_ready &&
                   !control->short_key_up_consumed) {
            control->short_key_release_arm_generation =
                e2e_timer_arm_generation(control);
            control->short_key_release_pending_generation =
                e2e_timer_pending_generation(control);
            control->short_key_up_consumed = 1;
            control->short_key_up_delivered = delivered;
            SDL_CondSignal(control->key_cond);
        }
    }
    if (control->key_event_waiting &&
        control->key_event_type == (Uint32)event_type &&
        control->key_event_code == (SDL_Keycode)keycode &&
        control->key_event_token == token) {
        /* main.c 在 guest event() 返回后调用，所以这里采样的是事件完成点。 */
        control->key_event_timer_arm_generation = e2e_timer_arm_generation(control);
        control->key_event_timer_pending_generation =
            e2e_timer_pending_generation(control);
        control->key_event_consumed = 1;
        control->key_event_delivered = delivered;
        SDL_CondSignal(control->key_cond);
    }
    SDL_UnlockMutex(control->key_mutex);
}

int e2e_control_take_key_up(E2eControl *control,
                                 int after_timer,
                                 int32_t *keycode,
                                 uint32_t *token) {
    if (!control || !keycode || !token) return 0;
    SDL_LockMutex(control->key_mutex);
    int ready = control->short_key_active &&
                control->short_key_down_consumed &&
                control->short_key_down_delivered &&
                !control->short_key_up_ready &&
                !e2e_runtime_exited(control);
    if (ready && !after_timer) {
        /* KEYDOWN 回调已完整返回；只有稳定地没有 guest timer 时才立即 release。 */
        ready = !e2e_timer_dispatch_in_progress(control) &&
                !e2e_timer_pending_generation(control);
    }
    if (ready) {
        /* main.c 会在当前主线程边界内同步 release，不再向 SDL 队列追加事件。 */
        control->short_key_up_ready = 1;
        *keycode = (int32_t)control->short_key_code;
        *token = control->short_key_token;
    }
    SDL_UnlockMutex(control->key_mutex);
    return ready;
}

void e2e_control_destroy(E2eControl *control) {
    if (!control) return;
    /* KEYDOWN 可能直接退出 guest；先唤醒等待 KEYUP 确认的控制线程再 join。 */
    SDL_AtomicSet(&control->stop_requested, 1);
    SDL_LockMutex(control->key_mutex);
    SDL_CondBroadcast(control->key_cond);
    SDL_UnlockMutex(control->key_mutex);
    if (control->listen_fd >= 0) {
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
    SDL_DestroyCond(control->key_cond);
    SDL_DestroyMutex(control->key_mutex);
    free(control);
}

#else

struct E2eControl {
    int unused;
};

int e2e_control_available(void) {
    return 0;
}

E2eControl *e2e_control_create(uint32_t event_type, const VmrpE2eHooks *hooks) {
    (void)event_type;
    (void)hooks;
    return NULL;
}

void e2e_control_start_if_requested(E2eControl *control) {
    (void)control;
}

void e2e_control_execute(E2eControl *control, SDL_Event *event) {
    (void)control;
    (void)event;
}

void e2e_control_key_event_completed(E2eControl *control,
                                          uint32_t event_type,
                                          int32_t keycode,
                                          uint32_t window_id,
                                          uint32_t token,
                                          int delivered) {
    (void)control;
    (void)event_type;
    (void)keycode;
    (void)window_id;
    (void)token;
    (void)delivered;
}

int e2e_control_take_key_up(E2eControl *control,
                                 int after_timer,
                                 int32_t *keycode,
                                 uint32_t *token) {
    (void)control;
    (void)after_timer;
    (void)keycode;
    (void)token;
    return 0;
}

void e2e_control_destroy(E2eControl *control) {
    (void)control;
}

#endif
