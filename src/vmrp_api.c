#include "./include/vmrp_api.h"
#include "./include/vmrp.h"
#include "./include/bridge.h"
#include "./include/memory.h"
#include "./include/native_dsm_funcs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

#define VMRP_API_ASYNC_RUNNER 1
#define API_QUEUE_CAP 64
#define VMRP_API_LOG_ENABLED() (getenv("VMRP_LOG") != NULL)
#define VMRP_API_LOG(...)                          \
    do {                                           \
        if (VMRP_API_LOG_ENABLED()) {              \
            fprintf(stderr, __VA_ARGS__);          \
            fflush(stderr);                        \
        }                                          \
    } while (0)

static uint16_t *screen_buf = NULL;
static uint8_t *screen_rgba_buf = NULL;
static int screen_dirty = 0;
static int pending_timer_ms = 0;
static int api_running = 0;
static int image_processing_mode = VMRP_IMAGE_PROCESSING_NATIVE;

static int edit_active = 0;
static int32_t edit_max_size = 0;
static char *hold_edit_text = NULL;
static char *edit_text_snapshot = NULL;

typedef enum {
    API_CMD_EVENT,
    API_CMD_SET_EDIT_TEXT,
    API_CMD_CANCEL_EDIT
} ApiCommandType;

typedef struct {
    ApiCommandType type;
    int code;
    int p0;
    int p1;
    char *text;
} ApiCommand;

#if VMRP_API_ASYNC_RUNNER
#ifdef _WIN32
static HANDLE api_thread = NULL;
static DWORD api_thread_id = 0;
static CRITICAL_SECTION api_mutex;
static CONDITION_VARIABLE api_cond;
static CRITICAL_SECTION screen_mutex;
static int api_sync_ready = 0;
#define API_THREAD_RET DWORD WINAPI
#define API_THREAD_DONE 0
#else
static pthread_t api_thread;
static pthread_mutex_t api_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t api_cond;
static pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;
static int api_sync_ready = 0;
#define API_THREAD_RET void *
#define API_THREAD_DONE NULL
#endif
static int api_thread_started = 0;
static int api_worker_stop = 0;
static int api_timer_active = 0;
static uint64_t api_timer_due_ms = 0;
static ApiCommand api_queue[API_QUEUE_CAP];
static int api_queue_head = 0;
static int api_queue_tail = 0;
static int api_queue_count = 0;

static uint64_t api_now_ms(void) {
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000u + (uint64_t)ts.tv_nsec / 1000000u;
#endif
}

#ifndef _WIN32
static void api_make_abs_timespec(uint64_t due_ms, struct timespec *out) {
    out->tv_sec = (time_t)(due_ms / 1000u);
    out->tv_nsec = (long)((due_ms % 1000u) * 1000000u);
}
#endif

static void api_ensure_sync(void) {
    if (api_sync_ready) return;
#ifdef _WIN32
    InitializeCriticalSection(&api_mutex);
    InitializeCriticalSection(&screen_mutex);
    InitializeConditionVariable(&api_cond);
#else
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&api_cond, &attr);
    pthread_condattr_destroy(&attr);
#endif
    api_sync_ready = 1;
}

static void api_lock(void) {
#ifdef _WIN32
    EnterCriticalSection(&api_mutex);
#else
    pthread_mutex_lock(&api_mutex);
#endif
}

static void api_unlock(void) {
#ifdef _WIN32
    LeaveCriticalSection(&api_mutex);
#else
    pthread_mutex_unlock(&api_mutex);
#endif
}

static void screen_lock(void) {
#ifdef _WIN32
    EnterCriticalSection(&screen_mutex);
#else
    pthread_mutex_lock(&screen_mutex);
#endif
}

static void screen_unlock(void) {
#ifdef _WIN32
    LeaveCriticalSection(&screen_mutex);
#else
    pthread_mutex_unlock(&screen_mutex);
#endif
}

static void api_signal(void) {
#ifdef _WIN32
    WakeConditionVariable(&api_cond);
#else
    pthread_cond_signal(&api_cond);
#endif
}

static void api_wait_forever(void) {
#ifdef _WIN32
    SleepConditionVariableCS(&api_cond, &api_mutex, INFINITE);
#else
    pthread_cond_wait(&api_cond, &api_mutex);
#endif
}

static void api_wait_until(uint64_t due_ms) {
#ifdef _WIN32
    uint64_t now = api_now_ms();
    uint64_t delta = due_ms > now ? due_ms - now : 0;
    DWORD timeout = delta >= 0xfffffffeULL ? 0xfffffffeUL : (DWORD)delta;
    SleepConditionVariableCS(&api_cond, &api_mutex, timeout);
#else
    struct timespec ts;
    api_make_abs_timespec(due_ms, &ts);
    pthread_cond_timedwait(&api_cond, &api_mutex, &ts);
#endif
}
#endif

static void free_hold_edit_text(void) {
    if (hold_edit_text) {
        my_freeExt(hold_edit_text);
        hold_edit_text = NULL;
    }
}

static void free_edit_text_snapshot(void) {
    free(edit_text_snapshot);
    edit_text_snapshot = NULL;
}

/* --- bridge.h implementations (linked by native_dsm_funcs.c) --- */

void guiDrawBitmapWithStride(uint16_t *bmp, int32_t x, int32_t y,
                             int32_t w, int32_t h,
                             int32_t source_stride,
                             int32_t source_x,
                             int32_t source_y) {
    if (!screen_buf || !bmp || source_stride <= 0 || w <= 0 || h <= 0) return;
    int sw = vmrp_config.screen_width;
    int sh = vmrp_config.screen_height;
    int32_t min_x = x < 0 ? 0 : x;
    int32_t min_y = y < 0 ? 0 : y;
    int32_t max_x = x + w;
    int32_t max_y = y + h;
    if (max_x > sw) max_x = sw;
    if (max_y > sh) max_y = sh;
    if (min_x >= max_x || min_y >= max_y) return;

    size_t row_bytes = (size_t)(max_x - min_x) * sizeof(uint16_t);
#if VMRP_API_ASYNC_RUNNER
    screen_lock();
#endif
    for (int32_t yy = min_y; yy < max_y; yy++) {
        size_t off = (size_t)yy * (size_t)sw + (size_t)min_x;
        int32_t sx = source_x + (min_x - x);
        int32_t sy = source_y + (yy - y);
        if (sx < 0 || sy < 0 || sx + (max_x - min_x) > source_stride) {
            continue;
        }
        memcpy(screen_buf + off,
               bmp + (size_t)sy * (size_t)source_stride + (size_t)sx,
               row_bytes);
    }
    screen_dirty = 1;
#if VMRP_API_ASYNC_RUNNER
    screen_unlock();
#endif
}

void guiDrawBitmap(uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h) {
    /*
     * Default DSM callers submit mr_screenBuf rectangles, so their source
     * coordinates remain absolute screen coordinates.  Local bitmap presents
     * use guiDrawBitmapWithStride() directly.
     */
    guiDrawBitmapWithStride(bmp, x, y, w, h,
                            vmrp_config.screen_width, x, y);
}

int32_t timerStart(uint16_t t) {
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    pending_timer_ms = (int)t;
    api_timer_active = t > 0;
    api_timer_due_ms = api_now_ms() + (uint64_t)t;
    api_signal();
    api_unlock();
    return 0;
#else
    pending_timer_ms = (int)t;
    return 0;
#endif
}

int32_t timerStop(void) {
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    pending_timer_ms = 0;
    api_timer_active = 0;
    api_signal();
    api_unlock();
    return 0;
#else
    pending_timer_ms = 0;
    return 0;
#endif
}

int32_t editCreate(const char *title, const char *text, int32_t type, int32_t max_size) {
    (void)title;
    (void)type;
#if VMRP_API_ASYNC_RUNNER
    api_lock();
#endif
    edit_active = 1;
    edit_max_size = max_size;
    free_hold_edit_text();
    if (text) {
        size_t len = strlen(text);
        hold_edit_text = my_mallocExt((uint32)(len + 1));
        memcpy(hold_edit_text, text, len + 1);
    }
#if VMRP_API_ASYNC_RUNNER
    api_unlock();
#endif
    return 1234;
}

int32 editRelease(int32 edit) {
    (void)edit;
#if VMRP_API_ASYNC_RUNNER
    api_lock();
#endif
    edit_active = 0;
    free_hold_edit_text();
#if VMRP_API_ASYNC_RUNNER
    api_unlock();
#endif
    return 0;
}

char *editGetText(int32 edit) {
    (void)edit;
    return hold_edit_text;
}

static const uint8_t *screen_rgba_native(void) {
    if (!screen_buf || !screen_rgba_buf) return NULL;
#if VMRP_API_ASYNC_RUNNER
    screen_lock();
#endif
    size_t pixel_count = (size_t)vmrp_config.screen_width * (size_t)vmrp_config.screen_height;
    uint8_t *dst = screen_rgba_buf;
    for (size_t i = 0; i < pixel_count; i++) {
        uint16_t c = screen_buf[i];
        uint8_t r5 = (uint8_t)((c >> 11) & 0x1f);
        uint8_t g6 = (uint8_t)((c >> 5) & 0x3f);
        uint8_t b5 = (uint8_t)(c & 0x1f);
        dst[0] = (uint8_t)((r5 << 3) | (r5 >> 2));
        dst[1] = (uint8_t)((g6 << 2) | (g6 >> 4));
        dst[2] = (uint8_t)((b5 << 3) | (b5 >> 2));
        dst[3] = 255;
        dst += 4;
    }
#if VMRP_API_ASYNC_RUNNER
    screen_unlock();
#endif
    return screen_rgba_buf;
}

static const uint8_t *screen_rgba_opencv(void) {
    /* This keeps the OpenCV mode selectable before an OpenCV SDK is linked.
     * A real OpenCV converter can replace this branch without changing Dart. */
    return screen_rgba_native();
}

#if VMRP_API_ASYNC_RUNNER
static int api_queue_command_locked(ApiCommand cmd) {
    if (cmd.type == API_CMD_EVENT && cmd.code == VMRP_MOUSE_MOVE) {
        if (api_queue_count > 0) {
            int idx = (api_queue_tail - 1 + API_QUEUE_CAP) % API_QUEUE_CAP;
            ApiCommand *old = &api_queue[idx];
            if (old->type == API_CMD_EVENT && old->code == VMRP_MOUSE_MOVE) {
                old->p0 = cmd.p0;
                old->p1 = cmd.p1;
                free(cmd.text);
                return 0;
            }
        }
    }
    if (api_queue_count >= API_QUEUE_CAP) {
        if (cmd.type == API_CMD_EVENT && cmd.code == VMRP_MOUSE_MOVE) {
            free(cmd.text);
            return 0;
        }
        return -1;
    }
    api_queue[api_queue_tail] = cmd;
    api_queue_tail = (api_queue_tail + 1) % API_QUEUE_CAP;
    api_queue_count++;
    api_signal();
    return 0;
}

static int api_queue_command(ApiCommand cmd) {
    api_lock();
    int ret = (!api_running || api_worker_stop) ? -1 : api_queue_command_locked(cmd);
    api_unlock();
    if (ret != 0) {
        free(cmd.text);
    }
    return ret;
}

static int api_pop_command_locked(ApiCommand *cmd) {
    if (api_queue_count <= 0) return 0;
    *cmd = api_queue[api_queue_head];
    api_queue_head = (api_queue_head + 1) % API_QUEUE_CAP;
    api_queue_count--;
    return 1;
}

static void api_clear_queue_locked(void) {
    while (api_queue_count > 0) {
        ApiCommand cmd;
        api_pop_command_locked(&cmd);
        free(cmd.text);
    }
}

static void api_apply_edit_text_command(char *text) {
    api_lock();
    if (!edit_active) {
        api_unlock();
        free(text);
        return;
    }

    free_hold_edit_text();
    if (text) {
        const uint8_t *utf8 = (const uint8_t *)text;
        int32_t n = 0;
        while (*utf8 && n < edit_max_size) {
            if (*utf8 < 0x80) utf8 += 1;
            else if ((*utf8 & 0xe0) == 0xc0) utf8 += 2;
            else if ((*utf8 & 0xf0) == 0xe0) utf8 += 3;
            else break;
            n++;
        }
        size_t len = (size_t)(utf8 - (const uint8_t *)text);
        hold_edit_text = my_mallocExt((uint32)(len + 1));
        memcpy(hold_edit_text, text, len);
        hold_edit_text[len] = '\0';
    }
    free(text);
    edit_active = 0;
    api_unlock();
    event(6, 0, 0);
}

static void api_apply_cancel_edit_command(void) {
    api_lock();
    if (!edit_active) {
        api_unlock();
        return;
    }
    edit_active = 0;
    api_unlock();
    event(6, 1, 0);
}

static void api_finish_if_exited(void) {
    if (!vmrp_is_exited()) return;
    api_lock();
    api_running = 0;
    pending_timer_ms = 0;
    api_timer_active = 0;
    api_worker_stop = 1;
    api_clear_queue_locked();
    api_signal();
    api_unlock();
}

static API_THREAD_RET api_worker_main(void *userdata) {
    (void)userdata;
    for (;;) {
        ApiCommand cmd;
        int have_cmd = 0;
        int run_timer = 0;

        api_lock();
        for (;;) {
            if (api_worker_stop || !api_running) {
                api_unlock();
                return API_THREAD_DONE;
            }

            have_cmd = api_pop_command_locked(&cmd);
            if (have_cmd) break;

            if (api_timer_active) {
                uint64_t now = api_now_ms();
                if (api_timer_due_ms <= now) {
                    api_timer_active = 0;
                    pending_timer_ms = 0;
                    run_timer = 1;
                    break;
                }
                api_wait_until(api_timer_due_ms);
            } else {
                api_wait_forever();
            }
        }
        api_unlock();

        if (have_cmd) {
            switch (cmd.type) {
                case API_CMD_EVENT:
                    event(cmd.code, cmd.p0, cmd.p1);
                    break;
                case API_CMD_SET_EDIT_TEXT:
                    api_apply_edit_text_command(cmd.text);
                    cmd.text = NULL;
                    break;
                case API_CMD_CANCEL_EDIT:
                    api_apply_cancel_edit_command();
                    break;
            }
            free(cmd.text);
        } else if (run_timer) {
            timer();
        }

        api_finish_if_exited();
    }
}

static int api_start_worker(void) {
    if (api_thread_started) return 0;
    api_worker_stop = 0;
#ifdef _WIN32
    api_thread = CreateThread(NULL, 0, api_worker_main, NULL, 0, &api_thread_id);
    if (!api_thread) {
        return -1;
    }
#else
    if (pthread_create(&api_thread, NULL, api_worker_main, NULL) != 0) {
        return -1;
    }
#endif
    api_thread_started = 1;
    return 0;
}

static void api_stop_worker(void) {
    if (!api_thread_started) return;
    api_lock();
    api_worker_stop = 1;
    api_timer_active = 0;
    pending_timer_ms = 0;
    api_clear_queue_locked();
    api_signal();
    api_unlock();
#ifdef _WIN32
    WaitForSingleObject(api_thread, INFINITE);
    CloseHandle(api_thread);
    api_thread = NULL;
    api_thread_id = 0;
#else
    pthread_join(api_thread, NULL);
#endif
    api_thread_started = 0;
}
#else
static void api_stop_worker(void) {}
#endif

/* --- Public API --- */

VMRP_EXPORT int vmrp_api_init(int screen_w, int screen_h) {
    api_stop_worker();
#if VMRP_API_ASYNC_RUNNER
    api_ensure_sync();
#endif
    if (screen_w <= 0) screen_w = DEFAULT_SCREEN_WIDTH;
    if (screen_h <= 0) screen_h = DEFAULT_SCREEN_HEIGHT;
    vmrp_config.screen_width = screen_w;
    vmrp_config.screen_height = screen_h;

    free(screen_buf);
    free(screen_rgba_buf);
    size_t pixel_count = (size_t)screen_w * (size_t)screen_h;
    screen_buf = (uint16_t *)calloc(pixel_count, sizeof(uint16_t));
    screen_rgba_buf = (uint8_t *)malloc(pixel_count * 4u);
    if (!screen_buf || !screen_rgba_buf) {
        free(screen_buf);
        free(screen_rgba_buf);
        screen_buf = NULL;
        screen_rgba_buf = NULL;
        return -1;
    }
    memset(screen_rgba_buf, 0, pixel_count * 4u);

    screen_dirty = 0;
    pending_timer_ms = 0;
    image_processing_mode = VMRP_IMAGE_PROCESSING_NATIVE;
    free_edit_text_snapshot();
    edit_active = 0;
    api_running = 0;
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    api_timer_active = 0;
    api_worker_stop = 0;
    api_queue_head = 0;
    api_queue_tail = 0;
    api_queue_count = 0;
    api_unlock();
#endif

    return 0;
}

VMRP_EXPORT int vmrp_api_set_work_dir(const char *work_dir) {
    int n;
    if (!work_dir || !*work_dir) return -1;
    n = snprintf(vmrp_config.work_dir, sizeof(vmrp_config.work_dir), "%s", work_dir);
    if (n < 0 || (size_t)n >= sizeof(vmrp_config.work_dir)) {
        vmrp_config.work_dir[0] = '\0';
        return -1;
    }
    return 0;
}

VMRP_EXPORT int vmrp_api_start(const char *mrp_path, const char *ext, const char *entry) {
    VMRP_API_LOG("[vmrp_api] start('%s','%s','%s')\n", mrp_path ? mrp_path : "(null)",
                 ext ? ext : "(null)", entry ? entry : "(null)");
    if (!mrp_path || !*mrp_path) return -1;
    if (!ext || !*ext) ext = "start.mr";

    VmrpArgs args = vmrp_args_default();
    args.screen_width = vmrp_config.screen_width;
    args.screen_height = vmrp_config.screen_height;
    if (vmrp_config.work_dir[0]) {
        snprintf(args.work_dir, sizeof(args.work_dir), "%s", vmrp_config.work_dir);
    }
    snprintf(args.mrp_path, sizeof(args.mrp_path), "%s", mrp_path);
    snprintf(args.ext_name, sizeof(args.ext_name), "%s", ext);
    if (entry && *entry) snprintf(args.entry, sizeof(args.entry), "%s", entry);

    VMRP_API_LOG("[vmrp_api] startVmrp...\n");
    int ret = startVmrp(&args);
    VMRP_API_LOG("[vmrp_api] startVmrp returned %d\n", ret);
    api_running = (ret == 0 && !vmrp_is_exited()) ? 1 : 0;
#if VMRP_API_ASYNC_RUNNER
    if (api_running && api_start_worker() != 0) {
        api_running = 0;
        stopVmrp();
        return -1;
    }
#endif
    return ret;
}

VMRP_EXPORT void vmrp_api_destroy(void) {
    api_stop_worker();
    stopVmrp();
    free(screen_buf);
    free(screen_rgba_buf);
    screen_buf = NULL;
    screen_rgba_buf = NULL;
    screen_dirty = 0;
    pending_timer_ms = 0;
    api_running = 0;
    free_hold_edit_text();
    free_edit_text_snapshot();
    edit_active = 0;
}

VMRP_EXPORT int vmrp_api_set_dns_map(const char *map) {
    return configureVmrpDnsMap(map) == 0 ? 0 : -1;
}

VMRP_EXPORT int vmrp_api_event(int code, int p0, int p1) {
    if (!api_running || vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
        return -1;
    }
#if VMRP_API_ASYNC_RUNNER
    ApiCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = API_CMD_EVENT;
    cmd.code = code;
    cmd.p0 = p0;
    cmd.p1 = p1;
    return api_queue_command(cmd);
#else
    int ret = event((int32_t)code, (int32_t)p0, (int32_t)p1);
    if (vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
    }
    return ret;
#endif
}

VMRP_EXPORT int vmrp_api_timer(void) {
    if (!api_running || vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
        return -1;
    }
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    pending_timer_ms = 0;
    api_timer_active = 0;
    api_signal();
    api_unlock();
    return 0;
#else
    pending_timer_ms = 0;
    int ret = timer();
    if (vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
    }
    return ret;
#endif
}

VMRP_EXPORT int vmrp_api_get_timer_interval(void) {
    if (!api_running || vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
        return 0;
    }
#if VMRP_API_ASYNC_RUNNER
    return 0;
#else
    return pending_timer_ms;
#endif
}

VMRP_EXPORT int vmrp_api_set_image_processing_mode(int mode) {
    if (mode != VMRP_IMAGE_PROCESSING_NATIVE &&
        mode != VMRP_IMAGE_PROCESSING_OPENCV) {
        return -1;
    }
    image_processing_mode = mode;
    return 0;
}

VMRP_EXPORT int vmrp_api_get_image_processing_mode(void) {
    return image_processing_mode;
}

VMRP_EXPORT int vmrp_api_is_running(void) {
    if (api_running && vmrp_is_exited()) {
#if VMRP_API_ASYNC_RUNNER
        api_stop_worker();
#endif
        api_running = 0;
        pending_timer_ms = 0;
    }
    return api_running;
}

VMRP_EXPORT const uint16_t *vmrp_api_get_screen_buffer(void) {
    return screen_buf;
}

VMRP_EXPORT const uint8_t *vmrp_api_get_screen_rgba_buffer(void) {
    switch (image_processing_mode) {
        case VMRP_IMAGE_PROCESSING_OPENCV:
            return screen_rgba_opencv();
        case VMRP_IMAGE_PROCESSING_NATIVE:
        default:
            return screen_rgba_native();
    }
}

VMRP_EXPORT int vmrp_api_get_screen_dirty(void) {
#if VMRP_API_ASYNC_RUNNER
    screen_lock();
#endif
    int d = screen_dirty;
    screen_dirty = 0;
#if VMRP_API_ASYNC_RUNNER
    screen_unlock();
#endif
    return d;
}

VMRP_EXPORT int vmrp_api_get_screen_width(void) {
    return vmrp_config.screen_width;
}

VMRP_EXPORT int vmrp_api_get_screen_height(void) {
    return vmrp_config.screen_height;
}

VMRP_EXPORT int vmrp_api_audio_sample_rate(void) {
    return native_audio_sample_rate();
}

VMRP_EXPORT int vmrp_api_audio_channels(void) {
    return native_audio_channels();
}

VMRP_EXPORT int vmrp_api_audio_is_active(void) {
    if (!api_running || vmrp_is_exited()) return 0;
    return native_audio_is_active();
}

VMRP_EXPORT int vmrp_api_audio_render_s16le(void *buffer, int frames) {
    if (frames <= 0) {
        return 0;
    }
    if (!api_running || vmrp_is_exited()) {
        if (buffer) {
            memset(buffer, 0, (size_t)frames * (size_t)native_audio_channels() * sizeof(int16_t));
        }
        return 0;
    }
    return native_audio_render_s16le(buffer, frames);
}

VMRP_EXPORT void vmrp_api_audio_stop(void) {
    native_audio_stop();
}

VMRP_EXPORT int vmrp_api_is_edit_active(void) {
    if (!api_running || vmrp_is_exited()) return 0;
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    int active = edit_active;
    api_unlock();
    return active;
#else
    return edit_active;
#endif
}

VMRP_EXPORT const char *vmrp_api_get_edit_text(void) {
    static const char empty_text[] = "";
    if (!api_running || vmrp_is_exited()) return "";
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    const char *source = edit_active && hold_edit_text ? hold_edit_text : "";
    size_t len = strlen(source);
    char *copy = (char *)malloc(len + 1);
    if (copy) {
        memcpy(copy, source, len + 1);
    }
    api_unlock();
#else
    const char *source = edit_active && hold_edit_text ? hold_edit_text : "";
    size_t len = strlen(source);
    char *copy = (char *)malloc(len + 1);
    if (copy) {
        memcpy(copy, source, len + 1);
    }
#endif
    if (!copy) return empty_text;
    free(edit_text_snapshot);
    edit_text_snapshot = copy;
    return edit_text_snapshot;
}

VMRP_EXPORT int vmrp_api_set_edit_text(const char *text) {
    if (!api_running || vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
        return -1;
    }
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    int active = edit_active;
    api_unlock();
    if (!active) return -1;
    ApiCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = API_CMD_SET_EDIT_TEXT;
    if (text) {
        size_t len = strlen(text);
        cmd.text = (char *)malloc(len + 1);
        if (!cmd.text) return -1;
        memcpy(cmd.text, text, len + 1);
    }
    return api_queue_command(cmd);
#else
    if (!edit_active) return -1;
    free_hold_edit_text();
    if (text) {
        const uint8_t *utf8 = (const uint8_t *)text;
        int32_t n = 0;
        while (*utf8 && n < edit_max_size) {
            if (*utf8 < 0x80) utf8 += 1;
            else if ((*utf8 & 0xe0) == 0xc0) utf8 += 2;
            else if ((*utf8 & 0xf0) == 0xe0) utf8 += 3;
            else break;
            n++;
        }
        size_t len = (size_t)(utf8 - (const uint8_t *)text);
        hold_edit_text = my_mallocExt((uint32)(len + 1));
        memcpy(hold_edit_text, text, len);
        hold_edit_text[len] = '\0';
    }
    edit_active = 0;
    /* MR_DIALOG_KEY_OK = 0 */
    return event(6, 0, 0);
#endif
}

VMRP_EXPORT int vmrp_api_cancel_edit(void) {
    if (!api_running || vmrp_is_exited()) {
        api_running = 0;
        pending_timer_ms = 0;
        return -1;
    }
#if VMRP_API_ASYNC_RUNNER
    api_lock();
    int active = edit_active;
    api_unlock();
    if (!active) return -1;
    ApiCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = API_CMD_CANCEL_EDIT;
    return api_queue_command(cmd);
#else
    if (!edit_active) return -1;
    edit_active = 0;
    /* MR_DIALOG_KEY_CANCEL = 1 */
    return event(6, 1, 0);
#endif
}
