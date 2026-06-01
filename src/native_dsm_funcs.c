#include "./include/native_dsm_funcs.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/mman.h>
#endif

#include "./include/bridge.h"
#include "./include/fileLib.h"
#include "./include/network.h"
#include "./include/utils.h"
#include "./include/vmrp.h"

#define NATIVE_DSM_MEM_SIZE (4 * 1024 * 1024)

static void *native_mem_base;
static uint32 native_mem_len;
static uint64_t native_uptime_base;
static char readdir_buf[128];

static void native_test(void) {}

static void native_log(char *msg) {
    puts(msg ? msg : "(null)");
}

static void native_exit(void) {
    puts("mythroad exit.");
    exit(0);
}

static int32 native_mem_get(char **mem_base, uint32 *mem_len) {
    if (native_mem_base == NULL) {
#ifdef __linux__
#ifdef __x86_64__
        native_mem_base = mmap(NULL, NATIVE_DSM_MEM_SIZE, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
        if (native_mem_base == MAP_FAILED) native_mem_base = NULL;
#endif
#endif
        if (native_mem_base == NULL) {
            native_mem_base = malloc(NATIVE_DSM_MEM_SIZE);
        }
        if (native_mem_base == NULL) {
            return MR_FAILED;
        }
        native_mem_len = NATIVE_DSM_MEM_SIZE;
    }
    *mem_base = (char *)native_mem_base;
    *mem_len = native_mem_len;
    printf("native_mem_get base=%p len=%u(%u kb)\n", native_mem_base, native_mem_len, native_mem_len / 1024);
    return MR_SUCCESS;
}

static int32 native_mem_free(char *mem, uint32 mem_len) {
    (void)mem;
    (void)mem_len;
    return MR_SUCCESS;
}

static uint32 native_get_uptime_ms(void) {
    return (uint32)((uint64_t)get_uptime_ms() - native_uptime_base);
}

static int32 native_sleep(uint32 ms) {
    usleep(ms * 1000);
    return MR_SUCCESS;
}

static char *native_readdir(int32 f) {
    char *r = my_readdir(f);
    if (r == NULL) return NULL;
    strncpy(readdir_buf, r, sizeof(readdir_buf) - 1);
    readdir_buf[sizeof(readdir_buf) - 1] = '\0';
    return readdir_buf;
}

static int32 native_initNetwork(NETWORK_CB cb, const char *mode, void *userData) {
    return my_initNetwork(NULL, NULL, mode, NULL);
}

static int32 native_getHostByName(const char *ptr, NETWORK_CB cb, void *userData) {
    (void)ptr;
    (void)cb;
    (void)userData;
    return MR_FAILED;
}

static int32 native_playSound(int type, const void *data, uint32 dataLen, int32 loop) {
    (void)type;
    (void)data;
    (void)dataLen;
    (void)loop;
    return MR_SUCCESS;
}

static int32 native_stopSound(int type) {
    (void)type;
    return MR_SUCCESS;
}

static int32 native_startShake(int32 ms) {
    (void)ms;
    return MR_SUCCESS;
}

static int32 native_stopShake(void) {
    return MR_SUCCESS;
}

static int32 native_dialogCreate(const char *title, const char *text, int32 type) {
    (void)title;
    (void)text;
    (void)type;
    return MR_FAILED;
}

static int32 native_dialogRelease(int32 dialog) {
    (void)dialog;
    return MR_FAILED;
}

static int32 native_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type) {
    (void)dialog;
    (void)title;
    (void)text;
    (void)type;
    return MR_FAILED;
}

static int32 native_textCreate(const char *title, const char *text, int32 type) {
    (void)title;
    (void)text;
    (void)type;
    return MR_FAILED;
}

static int32 native_textRelease(int32 text) {
    (void)text;
    return MR_FAILED;
}

static int32 native_textRefresh(int32 handle, const char *title, const char *text) {
    (void)handle;
    (void)title;
    (void)text;
    return MR_FAILED;
}

#if defined(__EMSCRIPTEN__)
#define NATIVE_DSM_FLAGS FLAG_USE_UTF8_FS
#elif defined(_WIN32)
#define NATIVE_DSM_FLAGS FLAG_USE_UTF8_EDIT
#else
#define NATIVE_DSM_FLAGS (FLAG_USE_UTF8_FS | FLAG_USE_UTF8_EDIT)
#endif

static DSM_REQUIRE_FUNCS native_funcs = {
    .test = native_test,
    .log = native_log,
    .exit = native_exit,
    .srand = srand,
    .rand = rand,
    .mem_get = native_mem_get,
    .mem_free = native_mem_free,
    .timerStart = timerStart,
    .timerStop = timerStop,
    .get_uptime_ms = native_get_uptime_ms,
    .getDatetime = getDatetime,
    .sleep = native_sleep,
    .open = my_open,
    .close = my_close,
    .read = my_read,
    .write = my_write,
    .seek = my_seek,
    .info = my_info,
    .remove = my_remove,
    .rename = my_rename,
    .mkDir = my_mkDir,
    .rmDir = my_rmDir,
    .opendir = my_opendir,
    .readdir = native_readdir,
    .closedir = my_closedir,
    .getLen = my_getLen,
    .drawBitmap = guiDrawBitmap,
    .getHostByName = native_getHostByName,
    .initNetwork = native_initNetwork,
    .mr_closeNetwork = my_closeNetwork,
    .mr_socket = my_socket,
    .mr_connect = my_connect,
    .mr_getSocketState = my_getSocketState,
    .mr_closeSocket = my_closeSocket,
    .mr_recv = my_recv,
    .mr_send = my_send,
    .mr_recvfrom = my_recvfrom,
    .mr_sendto = my_sendto,
    .mr_startShake = native_startShake,
    .mr_stopShake = native_stopShake,
    .mr_playSound = native_playSound,
    .mr_stopSound = native_stopSound,
    .mr_dialogCreate = native_dialogCreate,
    .mr_dialogRelease = native_dialogRelease,
    .mr_dialogRefresh = native_dialogRefresh,
    .mr_textCreate = native_textCreate,
    .mr_textRelease = native_textRelease,
    .mr_textRefresh = native_textRefresh,
    .mr_editCreate = editCreate,
    .mr_editRelease = editRelease,
    .mr_editGetText = editGetText,
    .flags = NATIVE_DSM_FLAGS,
    .screen_width = 0,
    .screen_height = 0,
};

DSM_REQUIRE_FUNCS *native_dsm_funcs_get(void) {
    native_uptime_base = (uint64_t)get_uptime_ms();
    native_funcs.screen_width = vmrp_config.screen_width;
    native_funcs.screen_height = vmrp_config.screen_height;
    return &native_funcs;
}

void native_dsm_funcs_destroy(void) {
#ifdef __linux__
#ifdef __x86_64__
    if (native_mem_base != NULL) {
        uintptr_t p = (uintptr_t)native_mem_base;
        if (p < 0x80000000ULL) {
            munmap(native_mem_base, native_mem_len);
            native_mem_base = NULL;
            native_mem_len = 0;
            return;
        }
    }
#endif
#endif
    free(native_mem_base);
    native_mem_base = NULL;
    native_mem_len = 0;
}

int32_t bridge_dsm_network_cb(uc_engine *uc, uint32_t addr, int32_t p0, uint32_t p1) {
    (void)uc;
    int32 (*cb)(int32, void *) = (int32 (*)(int32, void *))(uintptr_t)addr;
    return cb ? cb(p0, (void *)(uintptr_t)p1) : MR_FAILED;
}
