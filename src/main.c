#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#include "./include/compat_msvc.h"
#include "./include/dirent_win.h"
#else
#include <dirent.h>
#include <unistd.h>
#endif

#include "./include/bridge.h"
#include "./include/e2e_control.h"
#include "./include/vmrp.h"
#include "./include/memory.h"

#ifdef _MSC_VER
#include <SDL.h>
#elif defined(_WIN32)
// #ifdef __x86_64__
// #include "./windows/SDL2-2.0.10/x86_64-w64-mingw32/include/SDL2/SDL.h"
// #elif __i386__
#include "./windows/SDL2-2.0.10/i686-w64-mingw32/include/SDL2/SDL.h"
// #endif
#else
#include <SDL2/SDL.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define MOUSE_DOWN 2
#define MOUSE_UP 3
#define MOUSE_MOVE 12

// http://wiki.libsdl.org/Tutorials
// http://lazyfoo.net/tutorials/SDL/index.php

static SDL_TimerID timeId = 0;
static SDL_Window *window;
static bool isMouseDown = false;

/* PPM 截屏：收到 SIGUSR1 时将当前 SDL surface 转储为 PPM 文件，
 * 用于在无显示器环境下验证画面是否正常渲染。 */
static SDL_atomic_t guiDrawBitmapCount;

static const char *screen_dump_path(void) {
    const char *path = getenv("VMRP_PPM_PATH");
    return (path && *path) ? path : "/tmp/vmrp_screen.ppm";
}

static int dump_screen_ppm(const char *path) {
    if (!window) return -1;
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface) return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "P6\n%d %d\n255\n", surface->w, surface->h);
    if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0) {
        fclose(fp);
        return -1;
    }
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 px = *((Uint32 *)(((Uint8 *)surface->pixels) + surface->pitch * y) + x);
            Uint8 r, g, b;
            SDL_GetRGB(px, surface->format, &r, &g, &b);
            fputc(r, fp); fputc(g, fp); fputc(b, fp);
        }
    }
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    int ret = ferror(fp) ? -1 : 0;
    fclose(fp);
    // printf("[PPM] dumped to %s (%dx%d)\n", path, surface->w, surface->h);
    return ret;
}

static void sigusr1_handler(int sig) {
    (void)sig;
    dump_screen_ppm(screen_dump_path());
}

static int e2e_dump_screen_ppm_hook(const char *path, void *userdata) {
    (void)userdata;
    return dump_screen_ppm(path);
}

static const char *e2e_screen_dump_path_hook(void *userdata) {
    (void)userdata;
    return screen_dump_path();
}

static int e2e_draw_count_hook(void *userdata) {
    (void)userdata;
    return SDL_AtomicGet(&guiDrawBitmapCount);
}

/* timerCb 在 SDL 定时器线程中触发，直接调用 timer() 会与主线程的 event()
 * 同时访问同一个 Unicorn ARM 引擎，引发竞态崩溃。改为向 SDL 事件队列推送
 * 自定义事件，由主循环统一调度 timer()，保证单线程串行执行。 */
static Uint32 timerEventType = 0;
static Uint32 e2eEventType = (Uint32)-1;
static VmrpE2eControl *e2eControl = NULL;
static bool isEditMode = false;
static int32_t editMaxSize = 0;
static char *holdEditText = NULL;
static uint32_t clickSeq = 0;

static SDL_Keycode isKeyDown = SDLK_UNKNOWN;

void saveEditText(char *str) {
    uint8_t *utf8Str = (uint8_t *)str;
    int32_t n = 0;
    while (*utf8Str && (n < editMaxSize)) {
        if (*utf8Str < 0x80) {  // 1 Byte
            utf8Str += 1;
        } else if ((*utf8Str & 0xe0) == 0xc0) {  // 2 Bytes
            utf8Str += 2;
        } else if ((*utf8Str & 0xf0) == 0xe0) {  // 3 Bytes
            utf8Str += 3;
        } else {
            break;
        }
        n++;
    }
    if (holdEditText != NULL) {
        my_freeExt(holdEditText);
        holdEditText = NULL;
    }
    uint32_t len = (uint32_t)(utf8Str - (uint8_t *)str);
    holdEditText = my_mallocExt(len + 1);
    memcpy(holdEditText, str, len);
    holdEditText[len] = '\0';
}

int32_t editCreate(const char *title, const char *text, int32_t type, int32_t max_size) {
    isEditMode = true;
    editMaxSize = max_size;
    SDL_Log("title: '%s', text: '%s', type: %d, max_size: %d", title, text, type, max_size);
    if (SDL_SetClipboardText(text) == 0) {
        SDL_Log("编辑内容已复制到剪贴板，按ctrl+v输入内容，按ctrl+z取消");
    } else {
        SDL_Log("无法使用剪贴板");
    }
    return 1234;
}

int32 editRelease(int32 edit) {
    isEditMode = false;
    if (holdEditText != NULL) {
        my_freeExt(holdEditText);
        holdEditText = NULL;
    }
    return MR_SUCCESS;
}

char *editGetText(int32 edit) {
    SDL_Log("editGetText(): '%s'", holdEditText);
    return holdEditText;
}

void guiDrawBitmap(uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h) {
    int draw_count = SDL_AtomicAdd(&guiDrawBitmapCount, 1) + 1;
    /* Dump after the bitmap is copied to the SDL surface.  Dumping before the
     * draw captures the previous frame, which makes VMRP_PPM misleading for
     * foreground handoff bugs where the last visible frame matters.  When
     * VMRP_PPM is set, the caller has explicitly requested verification, so
     * keep the configured PPM path equal to the most recent rendered frame. */
    int should_dump_ppm = getenv("VMRP_PPM") || draw_count == 5;
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) != 0) printf("SDL_LockSurface err\n");
    }
    for (int32_t j = 0; j < h; j++) {
        for (int32_t i = 0; i < w; i++) {
            int32_t xx = x + i;
            int32_t yy = y + j;
            if (xx < 0 || yy < 0 || xx >= vmrp_config.screen_width || yy >= vmrp_config.screen_height) {
                continue;
            }
            uint16_t color = *(bmp + (xx + yy * vmrp_config.screen_width));
            Uint32 *p = (Uint32 *)(((Uint8 *)surface->pixels) + surface->pitch * yy) + xx;
            *p = SDL_MapRGB(surface->format, PIXEL565R(color), PIXEL565G(color), PIXEL565B(color));
        }
    }
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    if (SDL_UpdateWindowSurface(window) != 0)
        printf("SDL_UpdateWindowSurface err\n");
    if (should_dump_ppm) {
        dump_screen_ppm(screen_dump_path());
    }
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void setEventEnable(int v) {
    int state = v ? SDL_ENABLE : SDL_DISABLE;
    SDL_EventState(SDL_TEXTINPUT, state);
    SDL_EventState(SDL_KEYDOWN, state);
    SDL_EventState(SDL_KEYUP, state);
    SDL_EventState(SDL_MOUSEMOTION, state);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, state);
    SDL_EventState(SDL_MOUSEBUTTONUP, state);
}
#endif

uint32_t timerCb(uint32_t interval, void *param) {
    SDL_RemoveTimer(timeId);
    timeId = 0;
    /* 不在定时器线程调用 timer()，改为推送事件到主线程 */
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = timerEventType;
    SDL_PushEvent(&ev);
    return 0;
}

int32_t timerStart(uint16_t t) {
    if (!timeId) {
        timeId = SDL_AddTimer(t, timerCb, NULL);
    } else {
        SDL_RemoveTimer(timeId);
        timeId = SDL_AddTimer(t, timerCb, NULL);
    }
    return MR_SUCCESS;
}

int32_t timerStop() {
    if (timeId) {
        SDL_RemoveTimer(timeId);
        timeId = 0;
    }
    return MR_SUCCESS;
}

static void keyEvent(int16 type, SDL_Keycode code) {
    if (code >= SDLK_0 && code <= SDLK_9) {
        int32_t key = MR_KEY_0 + (code - SDLK_0);
        event(type, key, 0);  // 按键 0-9
        return;
    }
    switch (code) {
        case SDLK_KP_0:
            event(type, MR_KEY_0, 0);
            break;
        case SDLK_KP_1:
            event(type, MR_KEY_1, 0);
            break;
        case SDLK_KP_2:
            event(type, MR_KEY_2, 0);
            break;
        case SDLK_KP_3:
            event(type, MR_KEY_3, 0);
            break;
        case SDLK_KP_4:
            event(type, MR_KEY_4, 0);
            break;
        case SDLK_KP_5:
            event(type, MR_KEY_5, 0);
            break;
        case SDLK_KP_6:
            event(type, MR_KEY_6, 0);
            break;
        case SDLK_KP_7:
            event(type, MR_KEY_7, 0);
            break;
        case SDLK_KP_8:
            event(type, MR_KEY_8, 0);
            break;
        case SDLK_KP_9:
            event(type, MR_KEY_9, 0);
            break;
        case SDLK_KP_ENTER:
        case SDLK_RETURN:                   // 回车键
            event(type, MR_KEY_SELECT, 0);  // 确认/选择/ok
            break;
        case SDLK_EQUALS:                 // 等号
            event(type, MR_KEY_SOFTLEFT, 0);
            break;
        case SDLK_MINUS:                  // 减号
            event(type, MR_KEY_SOFTRIGHT, 0);
            break;
        case SDLK_ASTERISK:
            event(type, MR_KEY_STAR, 0);
            break;
        case SDLK_HASH:
            event(type, MR_KEY_POUND, 0);
            break;
        case SDLK_w:
        case SDLK_UP:  // 上
            event(type, MR_KEY_UP, 0);
            break;
        case SDLK_s:
        case SDLK_DOWN:  // 下
            event(type, MR_KEY_DOWN, 0);
            break;
        case SDLK_a:
        case SDLK_LEFT:  // 左
            event(type, MR_KEY_LEFT, 0);
            break;
        case SDLK_d:
        case SDLK_RIGHT:  // 右
            event(type, MR_KEY_RIGHT, 0);
            break;
        case SDLK_q:
        case SDLK_LEFTBRACKET:                // 左中括号
            event(type, MR_KEY_SOFTLEFT, 0);  // 左功能键
            break;
        case SDLK_e:
        case SDLK_RIGHTBRACKET:                // 右中括号
            event(type, MR_KEY_SOFTRIGHT, 0);  // 右功能键
            break;
        case SDLK_TAB:
            event(type, MR_KEY_SEND, 0);  // 接听键
            break;
        case SDLK_ESCAPE:
            event(type, MR_KEY_POWER, 0);  // 挂机键
            break;
        default:
            printf("key:%d\n", code);
            break;
    }
}

static void dispatch_key_down(SDL_Keycode code) {
    if (isKeyDown == SDLK_UNKNOWN) {
        isKeyDown = code;
        keyEvent(MR_KEY_PRESS, code);
    }
}

static void dispatch_key_up(SDL_Keycode code) {
    if (isKeyDown == code) {
        isKeyDown = SDLK_UNKNOWN;
        keyEvent(MR_KEY_RELEASE, code);
    }
}

static void dispatch_mouse_down(int x, int y) {
    uint32_t seq = ++clickSeq;
    printf("[CLICK] #%u down x=%d y=%d\n", seq, x, y);
    isMouseDown = true;
    int32_t ret = event(MR_MOUSE_DOWN, x, y);
    printf("[CLICK] #%u down ret=%d\n", seq, ret);
}

static void dispatch_mouse_up(int x, int y) {
    uint32_t seq = clickSeq;
    printf("[CLICK] #%u up x=%d y=%d\n", seq, x, y);
    isMouseDown = false;
    int32_t ret = event(MR_MOUSE_UP, x, y);
    printf("[CLICK] #%u up ret=%d\n", seq, ret);
}

/*
 * 自动点击注入：通过环境变量 VMRP_AUTO_CLICKS 触发一连串模拟点击，便于在没有
 * 真实交互的情况下复现 UI 路径上的 Bug。格式为 "x1,y1;x2,y2;..."，每个点击之间
 * 间隔 VMRP_AUTO_CLICK_DELAY_MS 毫秒（默认 800ms）。
 *
 * 在专用线程中调用 SDL_PushEvent，让事件像真实输入那样进入主循环。
 */
typedef struct {
    int x;
    int y;
    int delay_ms; /* 本次点击后等待的毫秒数，-1 表示使用全局默认值 */
} AutoClickPoint;

static AutoClickPoint *autoClickList = NULL;
static int autoClickCount = 0;

static int autoClickThread(void *data) {
    (void)data;
    const char *delay_env = getenv("VMRP_AUTO_CLICK_DELAY_MS");
    Uint32 default_delay = delay_env ? (Uint32)atoi(delay_env) : 800;
    if (default_delay == 0) default_delay = 800;

    /* 先等一段时间让应用完成启动 */
    SDL_Delay(default_delay);

    for (int i = 0; i < autoClickCount; ++i) {
        /* 本次点击后的延迟：优先使用自定义值，否则用全局默认值 */
        Uint32 cur_delay = (autoClickList[i].delay_ms >= 0)
                           ? (Uint32)autoClickList[i].delay_ms : default_delay;
        /* 约定：x<0 表示发送一次按键（用 y 不解释）。-1=ESC, -2=否/SOFTRIGHT,
         * -3=是/SOFTLEFT, -4=SELECT/确认 */
        if (autoClickList[i].x < 0) {
            SDL_Keycode kc = SDLK_ESCAPE;
            if (autoClickList[i].x == -2) kc = SDLK_MINUS;      /* 否 */
            else if (autoClickList[i].x == -3) kc = SDLK_EQUALS; /* 是 */
            else if (autoClickList[i].x == -4) kc = SDLK_RETURN; /* 确认 */
            SDL_Event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = SDL_KEYDOWN;
            ev.key.type = SDL_KEYDOWN;
            ev.key.state = SDL_PRESSED;
            ev.key.keysym.sym = kc;
            SDL_PushEvent(&ev);
            SDL_Delay(50);
            memset(&ev, 0, sizeof(ev));
            ev.type = SDL_KEYUP;
            ev.key.type = SDL_KEYUP;
            ev.key.state = SDL_RELEASED;
            ev.key.keysym.sym = kc;
            SDL_PushEvent(&ev);
            SDL_Delay(cur_delay);
            continue;
        }
        SDL_Event ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = SDL_MOUSEBUTTONDOWN;
        ev.button.type = SDL_MOUSEBUTTONDOWN;
        ev.button.button = SDL_BUTTON_LEFT;
        ev.button.state = SDL_PRESSED;
        ev.button.x = autoClickList[i].x;
        ev.button.y = autoClickList[i].y;
        SDL_PushEvent(&ev);

        SDL_Delay(500);

        memset(&ev, 0, sizeof(ev));
        ev.type = SDL_MOUSEBUTTONUP;
        ev.button.type = SDL_MOUSEBUTTONUP;
        ev.button.button = SDL_BUTTON_LEFT;
        ev.button.state = SDL_RELEASED;
        ev.button.x = autoClickList[i].x;
        ev.button.y = autoClickList[i].y;
        SDL_PushEvent(&ev);

        SDL_Delay(cur_delay);
    }
    return 0;
}

static void startAutoClicksIfRequested(void) {
    const char *env = getenv("VMRP_AUTO_CLICKS");
    if (!env || !*env) return;
    /* 解析 "x1,y1[,delay1];x2,y2[,delay2];..." 第三个字段可选，单位 ms */
    int capacity = 8;
    autoClickList = (AutoClickPoint *)malloc(sizeof(AutoClickPoint) * capacity);
    autoClickCount = 0;
    const char *p = env;
    while (*p) {
        int x = 0, y = 0, d = -1;
        int n = sscanf(p, "%d,%d,%d", &x, &y, &d);
        if (n >= 2) {
            if (autoClickCount >= capacity) {
                capacity *= 2;
                autoClickList = (AutoClickPoint *)realloc(autoClickList, sizeof(AutoClickPoint) * capacity);
            }
            autoClickList[autoClickCount].x = x;
            autoClickList[autoClickCount].y = y;
            autoClickList[autoClickCount].delay_ms = (n >= 3) ? d : -1;
            autoClickCount++;
        }
        const char *semi = strchr(p, ';');
        if (!semi) break;
        p = semi + 1;
    }
    if (autoClickCount > 0) {
        printf("[AUTO_CLICK] scheduled %d clicks\n", autoClickCount);
        SDL_CreateThread(autoClickThread, "auto-click", NULL);
    }
}

void loop() {
    SDL_Event ev;
    bool isLoop = true;

    vmrp_e2e_control_start_if_requested(e2eControl);
    startAutoClicksIfRequested();

#if defined(__EMSCRIPTEN__)
#else
    while (isLoop)
#endif
    {
#if defined(__EMSCRIPTEN__)
        while (SDL_PollEvent(&ev))
#else
        while (SDL_WaitEvent(&ev))
#endif
        {
            if (vmrp_is_exited()) {
                isLoop = false;
                break;
            }
            if (ev.type == SDL_QUIT) {
                isLoop = false;
                // emscripten_cancel_main_loop();
                break;
            }
            if (ev.type == e2eEventType) {
                vmrp_e2e_control_execute(e2eControl, &ev);
                continue;
            }
            if (isEditMode) {
                switch (ev.type) {
                    case SDL_KEYDOWN: {
                        if (SDL_GetModState() & KMOD_CTRL) {
                            if (ev.key.keysym.sym == SDLK_z) {  // 取消编辑框输入
                                // MR_DIALOG_KEY_CANCEL=1
                                event(MR_DIALOG_EVENT, 1, 0);
                                SDL_Log("取消输入");
                                continue;
                            } else if (ev.key.keysym.sym == SDLK_v) {  // 编辑框输入
                                char *str = SDL_GetClipboardText();
                                saveEditText(str);
                                SDL_free(str);
                                // MR_DIALOG_KEY_OK=0
                                event(MR_DIALOG_EVENT, 0, 0);
                                continue;
                            }
                        }
                    }
                    case SDL_MOUSEBUTTONDOWN:
                        SDL_Log("ctrl+v输入内容，ctrl+z取消输入");
                }
                continue;
            }
            if (ev.type == timerEventType) {
                /* 由 timerCb 推送到主线程的定时器事件 */
                timer();
            } else switch (ev.type) {
                case SDL_KEYDOWN:
                    dispatch_key_down(ev.key.keysym.sym);
                    break;
                case SDL_KEYUP:
                    dispatch_key_up(ev.key.keysym.sym);
                    break;
                case SDL_MOUSEMOTION:
                    if (isMouseDown) {
                        event(MR_MOUSE_MOVE, ev.motion.x, ev.motion.y);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    dispatch_mouse_down(ev.button.x, ev.button.y);
                    break;
                case SDL_MOUSEBUTTONUP:
                    dispatch_mouse_up(ev.button.x, ev.button.y);
                    break;
            }
            if (vmrp_is_exited()) {
                isLoop = false;
                break;
            }
        }
    }
}

#ifdef _MSC_VER
static void abort_handler(int sig) {
    (void)sig;
    fflush(stdout);
    fprintf(stderr, "[CRASH] SIGABRT received - abort() was called\n");
    fflush(stderr);
    signal(SIGABRT, SIG_DFL);
    raise(SIGABRT);
}
static LONG WINAPI win_exception_filter(EXCEPTION_POINTERS *ep) {
    fprintf(stderr, "[CRASH] Unhandled exception: code=0x%08lX addr=%p\n",
        ep->ExceptionRecord->ExceptionCode,
        ep->ExceptionRecord->ExceptionAddress);
    fflush(stderr);
    return EXCEPTION_CONTINUE_SEARCH;
}
static void invalid_param_handler(const wchar_t *expr, const wchar_t *func,
                                   const wchar_t *file, unsigned int line,
                                   uintptr_t p) {
    (void)p;
    fflush(stdout);
    fprintf(stderr, "[CRASH] Invalid CRT parameter: expr=%ls func=%ls file=%ls line=%u\n",
            expr ? expr : L"(null)",
            func ? func : L"(null)",
            file ? file : L"(null)",
            line);
    fflush(stderr);
    abort();
}
#endif

int main(int argc, char *args[]) {
#ifdef _MSC_VER
    signal(SIGABRT, abort_handler);
    SetUnhandledExceptionFilter(win_exception_filter);
    _set_invalid_parameter_handler(invalid_param_handler);
#endif
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, sigusr1_handler);
#endif
    if (argc > 1 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "--help") == 0)) {
        vmrp_args_print_usage(args[0]);
        return 0;
    }
    VmrpArgs vmrp_args;
    if (vmrp_args_parse(argc, args, &vmrp_args) != MR_SUCCESS) {
        return -1;
    }

#ifdef __x86_64__
    printf("__x86_64__\n");
#elif __i386__
    printf("__i386__\n");
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    timerEventType = SDL_RegisterEvents(1);
    e2eEventType = SDL_RegisterEvents(1);
    if (e2eEventType == (Uint32)-1) {
        printf("SDL_RegisterEvents for E2E failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
#ifndef _WIN32
    /* SDL_Init 会重置 SIGUSR1 为默认处理（终止进程），导致截屏信号反而杀掉
     * 进程。此处在 SDL_Init 之后重新安装 SIGUSR1 截屏处理，确保 PPM 转储可用。 */
    signal(SIGUSR1, sigusr1_handler);
#endif

    /* guiDrawBitmap writes to SDL_GetWindowSurface(); avoiding an OpenGL window
     * lets the E2E pixel tests run under SDL's dummy video driver in CI. */
    window = SDL_CreateWindow("vmrp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vmrp_config.screen_width, vmrp_config.screen_height, 0);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    VmrpE2eHooks e2e_hooks;
    memset(&e2e_hooks, 0, sizeof(e2e_hooks));
    e2e_hooks.dump_screen_ppm = e2e_dump_screen_ppm_hook;
    e2e_hooks.screen_dump_path = e2e_screen_dump_path_hook;
    e2e_hooks.draw_count = e2e_draw_count_hook;
    e2eControl = vmrp_e2e_control_create(e2eEventType, &e2e_hooks);

    if (startVmrp(&vmrp_args) != MR_SUCCESS) {
        vmrp_e2e_control_destroy(e2eControl);
        e2eControl = NULL;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    if (vmrp_is_exited()) {
        stopVmrp();
        vmrp_e2e_control_destroy(e2eControl);
        e2eControl = NULL;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(loop, 0, 1);
#else
    loop();
#endif
    vmrp_e2e_control_destroy(e2eControl);
    e2eControl = NULL;
    stopVmrp();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
