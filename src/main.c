#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "./include/bridge.h"
#include "./include/vmrp.h"
#include "./include/memory.h"

#ifdef _WIN32
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
/* timerCb 在 SDL 定时器线程中触发，直接调用 timer() 会与主线程的 event()
 * 同时访问同一个 Unicorn ARM 引擎，引发竞态崩溃。改为向 SDL 事件队列推送
 * 自定义事件，由主循环统一调度 timer()，保证单线程串行执行。 */
static Uint32 timerEventType = 0;
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
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) != 0) printf("SDL_LockSurface err\n");
    }
    for (int32_t j = 0; j < h; j++) {
        for (int32_t i = 0; i < w; i++) {
            int32_t xx = x + i;
            int32_t yy = y + j;
            if (xx < 0 || yy < 0 || xx >= SCREEN_WIDTH || yy >= SCREEN_HEIGHT) {
                continue;
            }
            uint16_t color = *(bmp + (xx + yy * SCREEN_WIDTH));
            Uint32 *p = (Uint32 *)(((Uint8 *)surface->pixels) + surface->pitch * yy) + xx;
            *p = SDL_MapRGB(surface->format, PIXEL565R(color), PIXEL565G(color), PIXEL565B(color));
        }
    }
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    if (SDL_UpdateWindowSurface(window) != 0)
        printf("SDL_UpdateWindowSurface err\n");
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
} AutoClickPoint;

static AutoClickPoint *autoClickList = NULL;
static int autoClickCount = 0;

static int autoClickThread(void *data) {
    (void)data;
    const char *delay_env = getenv("VMRP_AUTO_CLICK_DELAY_MS");
    Uint32 delay_ms = delay_env ? (Uint32)atoi(delay_env) : 800;
    if (delay_ms == 0) delay_ms = 800;

    /* 先等一段时间让应用完成启动 */
    SDL_Delay(delay_ms);

    for (int i = 0; i < autoClickCount; ++i) {
        /* 约定：x = -1 表示发送一次 ESC（MR_KEY_POWER），y 携带 SDL_Keycode 用
         * 于自定义按键；这里只用到 ESC。其它情况按普通鼠标点击注入。 */
        if (autoClickList[i].x == -1) {
            SDL_Event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = SDL_KEYDOWN;
            ev.key.type = SDL_KEYDOWN;
            ev.key.state = SDL_PRESSED;
            ev.key.keysym.sym = SDLK_ESCAPE;
            SDL_PushEvent(&ev);
            SDL_Delay(50);
            memset(&ev, 0, sizeof(ev));
            ev.type = SDL_KEYUP;
            ev.key.type = SDL_KEYUP;
            ev.key.state = SDL_RELEASED;
            ev.key.keysym.sym = SDLK_ESCAPE;
            SDL_PushEvent(&ev);
            SDL_Delay(delay_ms);
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

        SDL_Delay(50); /* down 与 up 之间的间隔 */

        memset(&ev, 0, sizeof(ev));
        ev.type = SDL_MOUSEBUTTONUP;
        ev.button.type = SDL_MOUSEBUTTONUP;
        ev.button.button = SDL_BUTTON_LEFT;
        ev.button.state = SDL_RELEASED;
        ev.button.x = autoClickList[i].x;
        ev.button.y = autoClickList[i].y;
        SDL_PushEvent(&ev);

        SDL_Delay(delay_ms);
    }
    return 0;
}

static void startAutoClicksIfRequested(void) {
    const char *env = getenv("VMRP_AUTO_CLICKS");
    if (!env || !*env) return;
    /* 解析 "x1,y1;x2,y2;..." */
    int capacity = 8;
    autoClickList = (AutoClickPoint *)malloc(sizeof(AutoClickPoint) * capacity);
    autoClickCount = 0;
    const char *p = env;
    while (*p) {
        int x = 0, y = 0;
        if (sscanf(p, "%d,%d", &x, &y) == 2) {
            if (autoClickCount >= capacity) {
                capacity *= 2;
                autoClickList = (AutoClickPoint *)realloc(autoClickList, sizeof(AutoClickPoint) * capacity);
            }
            autoClickList[autoClickCount].x = x;
            autoClickList[autoClickCount].y = y;
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
            if (ev.type == SDL_QUIT) {
                isLoop = false;
                // emscripten_cancel_main_loop();
                break;
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
                    if (isKeyDown == SDLK_UNKNOWN) {
                        isKeyDown = ev.key.keysym.sym;
                        keyEvent(MR_KEY_PRESS, ev.key.keysym.sym);
                    }
                    break;
                case SDL_KEYUP:
                    if (isKeyDown == ev.key.keysym.sym) {
                        isKeyDown = SDLK_UNKNOWN;
                        keyEvent(MR_KEY_RELEASE, ev.key.keysym.sym);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (isMouseDown) {
                        event(MR_MOUSE_MOVE, ev.motion.x, ev.motion.y);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    uint32_t seq = ++clickSeq;
                    printf("[CLICK] #%u down x=%d y=%d\n", seq, ev.button.x, ev.button.y);
                    isMouseDown = true;
                    int32_t ret = event(MR_MOUSE_DOWN, ev.button.x, ev.button.y);
                    printf("[CLICK] #%u down ret=%d\n", seq, ret);
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    uint32_t seq = clickSeq;
                    printf("[CLICK] #%u up x=%d y=%d\n", seq, ev.button.x, ev.button.y);
                    isMouseDown = false;
                    int32_t ret = event(MR_MOUSE_UP, ev.button.x, ev.button.y);
                    printf("[CLICK] #%u up ret=%d\n", seq, ret);
                    break;
                }
            }
        }
    }
}

int main(int argc, char *args[]) {
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    if (argc > 1 && (strcmp(args[1], "-h") == 0 || strcmp(args[1], "--help") == 0)) {
        printVmrpUsage(args[0]);
        return 0;
    }
    if (prepareVmrpArgs(argc, args) != MR_SUCCESS) {
        return -1;
    }

#ifdef __x86_64__
    printf("__x86_64__\n");
#elif __i386__
    printf("__i386__\n");
#endif

    printf("CODE_ADDRESS:0x%X, CODE_SIZE:0x%X\n", CODE_ADDRESS, CODE_SIZE);
    printf("STACK_ADDRESS:0x%X, STACK_SIZE:0x%X\n", STACK_ADDRESS, STACK_SIZE);
    printf("MEMORY_MANAGER_ADDRESS:0x%X, MEMORY_MANAGER_SIZE:0x%X\n", MEMORY_MANAGER_ADDRESS, MEMORY_MANAGER_SIZE);
    printf("START_ADDRESS:0x%X, END_ADDRESS:0x%X\n", START_ADDRESS, END_ADDRESS);
    printf("TOTAL_MEMORY:0x%X(%d)\n", TOTAL_MEMORY, TOTAL_MEMORY);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    timerEventType = SDL_RegisterEvents(1);

    window = SDL_CreateWindow("vmrp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    if (startVmrp(argc, args) != MR_SUCCESS) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(loop, 0, 1);
#else
    loop();
#endif
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
