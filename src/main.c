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
/*
 * E2E 输入同步把每次 timerStart 作为独立 generation。pending 标识仍可完成的
 * 那一代 timer，dispatched 只在主线程完整执行 timer() 后发布；三者共同避免
 * 把按键前已经排队的 timer 事件误认为按键后的 guest 调度边界。
 */
static SDL_atomic_t timerArmGeneration;
static SDL_atomic_t timerDispatchedGeneration;
static SDL_atomic_t timerPendingGeneration;
static SDL_atomic_t timerDispatchInProgress;
static SDL_atomic_t runtimeExited;

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

#define E2E_DRAW_FRAME_RING_CAP 64

typedef struct {
    int draw_count;
    int width;
    int height;
    size_t rgb_len;
    uint8_t *rgb;
} E2eDrawFrame;

static E2eDrawFrame e2eDrawFrames[E2E_DRAW_FRAME_RING_CAP];
static SDL_mutex *e2eDrawFrameMutex = NULL;

static int e2e_draw_frame_capture_enabled(void) {
    static int cached = -1;
    if (cached < 0) {
        const char *socket = getenv("VMRP_E2E_SOCKET");
        cached = (socket && *socket) ? 1 : 0;
    }
    return cached;
}

static int e2e_ensure_draw_frame_mutex(void) {
    if (e2eDrawFrameMutex) return 1;
    e2eDrawFrameMutex = SDL_CreateMutex();
    return e2eDrawFrameMutex != NULL;
}

static int write_ppm_rgb(const char *path, int width, int height,
                         const uint8_t *rgb, size_t rgb_len) {
    if (!path || !rgb || width <= 0 || height <= 0) return -1;
    size_t expected = (size_t)width * (size_t)height * 3u;
    if (rgb_len < expected) return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    fwrite(rgb, 1, expected, fp);
    int ret = ferror(fp) ? -1 : 0;
    fclose(fp);
    return ret;
}

static void e2e_record_draw_frame(int draw_count, SDL_Surface *surface) {
    if (!e2e_draw_frame_capture_enabled() || draw_count <= 0 || !surface)
        return;
    if (!e2e_ensure_draw_frame_mutex()) return;

    int width = surface->w;
    int height = surface->h;
    if (width <= 0 || height <= 0) return;
    size_t rgb_len = (size_t)width * (size_t)height * 3u;
    uint8_t *rgb = (uint8_t *)malloc(rgb_len);
    if (!rgb) return;

    if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0) {
        free(rgb);
        return;
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 px = *((Uint32 *)(((Uint8 *)surface->pixels) +
                                      surface->pitch * y) + x);
            Uint8 r, g, b;
            size_t out = ((size_t)y * (size_t)width + (size_t)x) * 3u;
            SDL_GetRGB(px, surface->format, &r, &g, &b);
            rgb[out] = r;
            rgb[out + 1] = g;
            rgb[out + 2] = b;
        }
    }
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

    SDL_LockMutex(e2eDrawFrameMutex);
    E2eDrawFrame *slot =
        &e2eDrawFrames[(uint32_t)draw_count % E2E_DRAW_FRAME_RING_CAP];
    free(slot->rgb);
    slot->rgb = rgb;
    slot->rgb_len = rgb_len;
    slot->width = width;
    slot->height = height;
    slot->draw_count = draw_count;
    SDL_UnlockMutex(e2eDrawFrameMutex);
}

static int dump_e2e_draw_frame_ppm(int draw_count, const char *path) {
    if (!path || draw_count <= 0 || !e2eDrawFrameMutex) return -1;
    uint8_t *rgb = NULL;
    size_t rgb_len = 0;
    int width = 0;
    int height = 0;

    SDL_LockMutex(e2eDrawFrameMutex);
    E2eDrawFrame *slot =
        &e2eDrawFrames[(uint32_t)draw_count % E2E_DRAW_FRAME_RING_CAP];
    if (slot->draw_count == draw_count && slot->rgb && slot->rgb_len) {
        width = slot->width;
        height = slot->height;
        rgb_len = slot->rgb_len;
        rgb = (uint8_t *)malloc(rgb_len);
        if (rgb) memcpy(rgb, slot->rgb, rgb_len);
    }
    SDL_UnlockMutex(e2eDrawFrameMutex);

    if (!rgb) return -1;
    int ret = write_ppm_rgb(path, width, height, rgb, rgb_len);
    free(rgb);
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

static int e2e_dump_draw_frame_ppm_hook(int draw_count, const char *path,
                                        void *userdata) {
    (void)userdata;
    return dump_e2e_draw_frame_ppm(draw_count, path);
}

static const char *e2e_screen_dump_path_hook(void *userdata) {
    (void)userdata;
    return screen_dump_path();
}

static int e2e_draw_count_hook(void *userdata) {
    (void)userdata;
    return SDL_AtomicGet(&guiDrawBitmapCount);
}

static uint32_t e2e_timer_arm_generation_hook(void *userdata) {
    (void)userdata;
    return (uint32_t)SDL_AtomicGet(&timerArmGeneration);
}

static uint32_t e2e_timer_dispatched_generation_hook(void *userdata) {
    (void)userdata;
    return (uint32_t)SDL_AtomicGet(&timerDispatchedGeneration);
}

static uint32_t e2e_timer_pending_generation_hook(void *userdata) {
    (void)userdata;
    return (uint32_t)SDL_AtomicGet(&timerPendingGeneration);
}

static int e2e_timer_dispatch_in_progress_hook(void *userdata) {
    (void)userdata;
    return SDL_AtomicGet(&timerDispatchInProgress);
}

static int e2e_runtime_exited_hook(void *userdata) {
    (void)userdata;
    return SDL_AtomicGet(&runtimeExited);
}

static void e2e_publish_runtime_exit(void) {
    if (vmrp_is_exited()) SDL_AtomicSet(&runtimeExited, 1);
}

static void e2e_publish_timer_dispatch(uint32_t generation) {
    int current = SDL_AtomicGet(&timerDispatchedGeneration);
    /* Removed timers can already have queued callbacks; never let a stale event
     * move the published completion generation backwards. */
    while ((int32_t)(generation - (uint32_t)current) > 0 &&
           !SDL_AtomicCAS(&timerDispatchedGeneration, current, (int)generation)) {
        current = SDL_AtomicGet(&timerDispatchedGeneration);
    }
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
    /* Opening an editor must not replace text that the user copied before
     * entering it; Ctrl+V reads the existing platform clipboard below. */
    SDL_Log("按ctrl+v输入内容，按ctrl+z取消");
    return 1234;
}

int32 editRelease(int32 edit) {
    (void)edit; /* 单编辑框实现,句柄未使用;签名由 bridge ABI 固定 */
    isEditMode = false;
    if (holdEditText != NULL) {
        my_freeExt(holdEditText);
        holdEditText = NULL;
    }
    return MR_SUCCESS;
}

char *editGetText(int32 edit) {
    (void)edit; /* 同 editRelease:单编辑框实现,句柄未使用 */
    SDL_Log("editGetText(): '%s'", holdEditText);
    return holdEditText;
}

void guiDrawBitmapWithStride(uint16_t *bmp, int32_t x, int32_t y,
                             int32_t w, int32_t h,
                             int32_t source_stride,
                             int32_t source_x,
                             int32_t source_y) {
    if (!bmp || source_stride <= 0 || w <= 0 || h <= 0) return;
    int draw_count = SDL_AtomicAdd(&guiDrawBitmapCount, 1) + 1;
    /* Dump after the bitmap is copied to the SDL surface.  Dumping before the
     * draw captures the previous frame, which makes VMRP_PPM misleading for
     * foreground handoff bugs where the last visible frame matters.  When
     * VMRP_PPM is set, the caller has explicitly requested verification, so
     * keep the configured PPM path equal to the most recent rendered frame. */
    int should_dump_ppm = getenv("VMRP_PPM") || draw_count == 5;
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface) return;
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
            int32_t sx = source_x + i;
            int32_t sy = source_y + j;
            if (sx < 0 || sy < 0 || sx >= source_stride) continue;
            uint16_t color = *(bmp + ((size_t)sy * (size_t)source_stride + (size_t)sx));
            Uint32 *p = (Uint32 *)(((Uint8 *)surface->pixels) + surface->pitch * yy) + xx;
            *p = SDL_MapRGB(surface->format, PIXEL565R(color), PIXEL565G(color), PIXEL565B(color));
        }
    }
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    if (SDL_UpdateWindowSurface(window) != 0)
        printf("SDL_UpdateWindowSurface err\n");
    e2e_record_draw_frame(draw_count, surface);
    if (should_dump_ppm) {
        dump_screen_ppm(screen_dump_path());
    }
}

void guiDrawBitmap(uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h) {
    /*
     * Mythroad _DispUpEx submits rectangles from mr_screenBuf, whose source
     * coordinates are absolute screen coordinates.  Keep this legacy full-screen
     * stride path and let ARM EXT local bitmap presents opt into
     * guiDrawBitmapWithStride().
     */
    guiDrawBitmapWithStride(bmp, x, y, w, h,
                            vmrp_config.screen_width, x, y);
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
    (void)interval; /* 签名由 SDL_AddTimer 回调 ABI 固定 */
    uint32_t generation = (uint32_t)(uintptr_t)param;
    /* 回调返回 0 已使 SDL timer 成为 one-shot；不在 timer 线程改写 timeId，
     * 否则一个刚被替换的旧 callback 可能清掉主线程保存的新 timer identity。 */
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = timerEventType;
    ev.user.data1 = (void *)(uintptr_t)generation;
    if (SDL_PushEvent(&ev) != 1) {
        /* 只清除本代；较新的 timerStart 不能被较旧 callback 的失败覆盖。 */
        SDL_AtomicCAS(&timerPendingGeneration, (int)generation, 0);
    }
    return 0;
}

int32_t timerStart(uint16_t t) {
    uint32_t generation = (uint32_t)SDL_AtomicAdd(&timerArmGeneration, 1) + 1;
    /* 先发布身份，callback 即使立即运行也能针对同一 generation 清理失败状态。 */
    SDL_AtomicSet(&timerPendingGeneration, (int)generation);
    if (!timeId) {
        timeId = SDL_AddTimer(t, timerCb, (void *)(uintptr_t)generation);
    } else {
        SDL_RemoveTimer(timeId);
        timeId = SDL_AddTimer(t, timerCb, (void *)(uintptr_t)generation);
    }
    /* generation 即 timer 的身份；0 专门表示已停止或 arm 失败。 */
    if (!timeId) SDL_AtomicCAS(&timerPendingGeneration, (int)generation, 0);
    return MR_SUCCESS;
}

int32_t timerStop(void) {
    if (timeId) {
        SDL_RemoveTimer(timeId);
        timeId = 0;
    }
    SDL_AtomicSet(&timerPendingGeneration, 0);
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

static int dispatch_key_down(SDL_Keycode code) {
    if (isKeyDown == SDLK_UNKNOWN) {
        isKeyDown = code;
        keyEvent(MR_KEY_PRESS, code);
        return 1;
    }
    return 0;
}

static int dispatch_key_up(SDL_Keycode code) {
    if (isKeyDown == code) {
        isKeyDown = SDLK_UNKNOWN;
        keyEvent(MR_KEY_RELEASE, code);
        return 1;
    }
    return 0;
}

static void complete_e2e_key_event(const SDL_KeyboardEvent *key, int delivered) {
    e2e_publish_runtime_exit();
    vmrp_e2e_control_key_event_completed(
        e2eControl, key->type, key->keysym.sym,
        key->windowID, (uint32_t)key->keysym.scancode, delivered);
}

static void dispatch_e2e_key_up(int after_timer) {
    int32_t raw_code;
    uint32_t token;
    if (!vmrp_e2e_control_take_key_up(
            e2eControl, after_timer, &raw_code, &token)) return;

    SDL_Keycode code = (SDL_Keycode)raw_code;
    int delivered;
    if (isEditMode) {
        /* 与 edit-mode SDL_KEYUP 分支一致：编辑器拥有输入，只清宿主按键锁。 */
        delivered = isKeyDown == code;
        if (delivered) isKeyDown = SDLK_UNKNOWN;
    } else {
        delivered = dispatch_key_up(code);
    }
    e2e_publish_runtime_exit();
    vmrp_e2e_control_key_event_completed(
        e2eControl, SDL_KEYUP, code,
        VMRP_E2E_KEY_WINDOW_ID, token, delivered);
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

void loop(void) {
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
            if (ev.type == timerEventType) {
                uint32_t generation = (uint32_t)(uintptr_t)ev.user.data1;
                /* The SDL timer is one-shot and timer() rearms the guest's
                 * next tick.  Process it even while the platform editor owns
                 * keyboard input; dropping it there stops the guest scheduler
                 * permanently after a normal pause before Ctrl+V. */
                /* guest timer() 可能在一次调用内部先 stop 再 start；对控制线程
                 * 标记整个调用，避免把中途 pending=0 当成真正停止。 */
                SDL_AtomicSet(&timerDispatchInProgress, 1);
                timer();
                /*
                 * timer() 内可能已 arm 下一代。先发布本代完成，再仅在 pending
                 * 仍指向本代时清零，控制线程便不会看到 dispatch 中途的假停止。
                 * SDL 事件按入队顺序处理，generation 因而单调递增。
                */
                e2e_publish_timer_dispatch(generation);
                SDL_AtomicCAS(&timerPendingGeneration, (int)generation, 0);
                e2e_publish_runtime_exit();
                /* runtimeExited 必须先于 in-progress 清除发布，避免控制线程在
                 * 正常退出窗口中继续注入一个永远不会被主循环确认的按键。 */
                SDL_AtomicSet(&timerDispatchInProgress, 0);
                /* 默认 E2E 短按在这一拍结束时立即 release，不依赖控制线程调度。 */
                dispatch_e2e_key_up(1);
                if (vmrp_is_exited()) {
                    isLoop = false;
                    break;
                }
                continue;
            }
            if (isEditMode) {
                switch (ev.type) {
                    case SDL_KEYUP: {
                        /* A key can open the editor from its KEYDOWN handler.
                         * Its matching KEYUP then arrives while edit mode owns
                         * input, so consume it without sending a Mythroad key
                         * release but clear the host key latch.  Otherwise the
                         * next physical keydown is rejected as a duplicate. */
                        int delivered = isKeyDown == ev.key.keysym.sym;
                        if (delivered) {
                            isKeyDown = SDLK_UNKNOWN;
                        }
                        /* 编辑模式也完成了 release；按 token 通知对应的 E2E 命令。 */
                        complete_e2e_key_event(&ev.key, delivered);
                        continue;
                    }
                    case SDL_KEYDOWN: {
                        SDL_Keymod key_mod = (SDL_Keymod)(ev.key.keysym.mod | SDL_GetModState());
                        /* SDL_KEYDOWN carries the modifier state observed with
                         * the key event; use it so injected and physical Ctrl+V
                         * follow the same edit commit path. */
                        if (key_mod & KMOD_CTRL) {
                            if (ev.key.keysym.sym == SDLK_z) {  // 取消编辑框输入
                                // MR_DIALOG_KEY_CANCEL=1
                                event(MR_DIALOG_EVENT, 1, 0);
                                SDL_Log("取消输入");
                                complete_e2e_key_event(&ev.key, 1);
                                dispatch_e2e_key_up(0);
                                continue;
                            } else if (ev.key.keysym.sym == SDLK_v) {  // 编辑框输入
                                char *str = SDL_GetClipboardText();
                                saveEditText(str);
                                SDL_free(str);
                                // MR_DIALOG_KEY_OK=0
                                event(MR_DIALOG_EVENT, 0, 0);
                                complete_e2e_key_event(&ev.key, 1);
                                dispatch_e2e_key_up(0);
                                continue;
                            }
                        }
                    }
                    /* 非 Ctrl+V/Z 的按键与鼠标点击一样,只提示编辑操作方式 */
                    /* fall through */
                    case SDL_MOUSEBUTTONDOWN:
                        SDL_Log("ctrl+v输入内容，ctrl+z取消输入");
                }
                if (ev.type == SDL_KEYDOWN) {
                    /* 非编辑快捷键也已由 edit-mode 分支完整消费。 */
                    complete_e2e_key_event(&ev.key, 1);
                    dispatch_e2e_key_up(0);
                }
                continue;
            }
            switch (ev.type) {
                case SDL_KEYDOWN:
                    complete_e2e_key_event(
                        &ev.key, dispatch_key_down(ev.key.keysym.sym));
                    /* A guest with no pending timer still needs a deterministic
                     * short-key release at this same main-thread boundary. */
                    dispatch_e2e_key_up(0);
                    break;
                case SDL_KEYUP: {
                    int delivered = dispatch_key_up(ev.key.keysym.sym);
                    /* dispatch 返回表示 guest release 回调已完成，可等待下一 timer epoch。 */
                    complete_e2e_key_event(&ev.key, delivered);
                    break;
                }
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

    /* SDL 窗口必须按 --screen/环境变量解析出的分辨率创建。此前窗口用
     * vmrp_config 的编译期默认值(240x320),而 --screen 要到后面的
     * startVmrp() 才写入 vmrp_config,导致任何非默认分辨率都只显示
     * 240x320 窗口(gtcm --screen 480x320 只能看到左上裁切)。这里提前
     * 同步一次;startVmrp() 内部的赋值保持不变(共享库入口依赖它)。 */
    vmrp_config.screen_width = vmrp_args.screen_width;
    vmrp_config.screen_height = vmrp_args.screen_height;

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
    SDL_AtomicSet(&timerArmGeneration, 0);
    SDL_AtomicSet(&timerDispatchedGeneration, 0);
    SDL_AtomicSet(&timerPendingGeneration, 0);
    SDL_AtomicSet(&timerDispatchInProgress, 0);
    SDL_AtomicSet(&runtimeExited, 0);
    e2e_hooks.dump_screen_ppm = e2e_dump_screen_ppm_hook;
    e2e_hooks.dump_draw_frame_ppm = e2e_dump_draw_frame_ppm_hook;
    e2e_hooks.screen_dump_path = e2e_screen_dump_path_hook;
    e2e_hooks.draw_count = e2e_draw_count_hook;
    e2e_hooks.timer_arm_generation = e2e_timer_arm_generation_hook;
    e2e_hooks.timer_dispatched_generation = e2e_timer_dispatched_generation_hook;
    e2e_hooks.timer_pending_generation = e2e_timer_pending_generation_hook;
    e2e_hooks.timer_dispatch_in_progress = e2e_timer_dispatch_in_progress_hook;
    e2e_hooks.runtime_exited = e2e_runtime_exited_hook;
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
