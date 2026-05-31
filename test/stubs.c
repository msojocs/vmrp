/*
 * stubs.c — 为 test_vmrp 提供 mythroad.c 所需的外部函数桩
 *
 * 测试只需要 Lua VM 核心，不需要 ARM ext、SDL、文件系统等。
 * 这些桩保证链接通过，实际不会被调用。
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---------- arm_ext_executor stubs ---------- */
typedef struct ArmExtModule ArmExtModule;

int arm_ext_load(ArmExtModule **out, const void *code, uint32_t len,
                 int32_t load_code, int32_t *ext_ret) {
    (void)out; (void)code; (void)len; (void)load_code; (void)ext_ret;
    return -1;
}
int arm_ext_call(ArmExtModule *m, int32_t code, const void *input,
                 uint32_t input_len, void **output, int32_t *output_len) {
    (void)m; (void)code; (void)input; (void)input_len;
    (void)output; (void)output_len;
    return -1;
}
uint32_t arm_ext_helper_addr(ArmExtModule *m) { (void)m; return 0; }
uint32_t arm_ext_primary_helper(ArmExtModule *m) { (void)m; return 0; }
int arm_ext_call_dispatch(ArmExtModule *m, int is_stop, uint32_t ti) {
    (void)m; (void)is_stop; (void)ti; return -1;
}
int arm_ext_invoke0(ArmExtModule *m, uint32_t func, int32_t *ret_out) {
    (void)m; (void)func; (void)ret_out; return -1;
}
int arm_ext_invoke3(ArmExtModule *m, uint32_t func, uint32_t a0,
                    uint32_t a1, uint32_t a2, int32_t *ret_out) {
    (void)m; (void)func; (void)a0; (void)a1; (void)a2; (void)ret_out;
    return -1;
}
uint32_t arm_ext_read_timer_queue(ArmExtModule *m) { (void)m; return 0; }
void arm_ext_unload(ArmExtModule *m) { (void)m; }

/* ---------- dsm / platform stubs ---------- */
/* mythroad 的内存分配器通过 mr_printf → dsmInFuncs->log 打印诊断信息。
 * 提供一个最小的 DSM_REQUIRE_FUNCS 使 mr_printf 不崩溃 */
#include "../src/mythroad/include/dsm.h"

static void stub_log(char *msg) { (void)msg; }
static uint32_t stub_get_uptime_ms(void) { return 0; }
static int32_t stub_timerStart(uint16_t t) { (void)t; return 0; }
static int32_t stub_timerStop(void) { return 0; }
static int32_t stub_sleep(uint32_t ms) { (void)ms; return 0; }
static int32_t stub_getDatetime(void *dt) { (void)dt; return 0; }
static int32_t stub_rand(void) { return 42; }
static void stub_srand(uint32_t s) { (void)s; }
static void stub_exit(void) { exit(1); }
static void stub_drawBitmap(void *b, int16_t x, int16_t y, uint16_t w, uint16_t h) {
    (void)b;(void)x;(void)y;(void)w;(void)h;
}
static int32_t stub_open(const char *f, uint32_t m) { (void)f;(void)m; return 0; }
static int32_t stub_close(int32_t f) { (void)f; return 0; }
static int32_t stub_read(int32_t f, void *p, uint32_t l) { (void)f;(void)p;(void)l; return -1; }
static int32_t stub_write(int32_t f, void *p, uint32_t l) { (void)f;(void)p;(void)l; return -1; }
static int32_t stub_seek(int32_t f, int32_t p, int m) { (void)f;(void)p;(void)m; return -1; }
static int32_t stub_info(const char *f) { (void)f; return -1; }
static int32_t stub_remove(const char *f) { (void)f; return -1; }
static int32_t stub_rename(const char *o, const char *n) { (void)o;(void)n; return -1; }
static int32_t stub_mkDir(const char *n) { (void)n; return 0; }
static int32_t stub_rmDir(const char *n) { (void)n; return 0; }
static int32_t stub_getLen(const char *f) { (void)f; return -1; }
static int32_t stub_opendir(const char *n) { (void)n; return -1; }
static char *stub_readdir(int32_t h) { (void)h; return NULL; }
static int32_t stub_closedir(int32_t h) { (void)h; return 0; }

static int32_t stub_initNetwork(void *cb, const char *m, void *ud) {
    (void)cb;(void)m;(void)ud; return -1;
}
static int32_t stub_getHostByName(const char *n, void *cb, void *ud) {
    (void)n;(void)cb;(void)ud; return -1;
}
static int32_t stub_closeNetwork(void) { return 0; }
static int32_t stub_socket(int32_t t, int32_t p) { (void)t;(void)p; return -1; }
static int32_t stub_connect(int32_t s, int32_t ip, uint16_t port, int32_t t) {
    (void)s;(void)ip;(void)port;(void)t; return -1;
}
static int32_t stub_getSocketState(int32_t s) { (void)s; return -1; }
static int32_t stub_closeSocket(int32_t s) { (void)s; return 0; }
static int32_t stub_recv(int32_t s, char *b, int l) { (void)s;(void)b;(void)l; return -1; }
static int32_t stub_send(int32_t s, const char *b, int l) { (void)s;(void)b;(void)l; return -1; }
static int32_t stub_recvfrom(int32_t s, char *b, int l, int32_t *ip, uint16_t *p2) {
    (void)s;(void)b;(void)l;(void)ip;(void)p2; return -1;
}
static int32_t stub_sendto(int32_t s, const char *b, int l, int32_t ip, uint16_t p2) {
    (void)s;(void)b;(void)l;(void)ip;(void)p2; return -1;
}
static int32_t stub_dialogCreate(const char *t, const char *x, int32_t tp) {
    (void)t;(void)x;(void)tp; return 0;
}
static int32_t stub_dialogRelease(int32_t d) { (void)d; return 0; }
static int32_t stub_dialogRefresh(int32_t d, const char *t, const char *x, int32_t tp) {
    (void)d;(void)t;(void)x;(void)tp; return 0;
}
static int32_t stub_textCreate(const char *t, const char *x, int32_t tp) {
    (void)t;(void)x;(void)tp; return 0;
}
static int32_t stub_textRelease(int32_t t) { (void)t; return 0; }
static int32_t stub_textRefresh(int32_t h, const char *t, const char *x) {
    (void)h;(void)t;(void)x; return 0;
}
static int32_t stub_editCreate(const char *t, const char *x, int32_t tp, int32_t ms) {
    (void)t;(void)x;(void)tp;(void)ms; return 0;
}
static int32_t stub_editRelease(int32_t e) { (void)e; return 0; }
static const char *stub_editGetText(int32_t e) { (void)e; return ""; }
static int32_t stub_mr_startShake(int32_t ms) { (void)ms; return 0; }
static int32_t stub_mr_stopShake(void) { return 0; }
static int32_t stub_mr_playSound(int t, const void *d, uint32_t l, int32_t lp) {
    (void)t;(void)d;(void)l;(void)lp; return 0;
}
static int32_t stub_mr_stopSound(int t) { (void)t; return 0; }

#include <sys/mman.h>
#define FAKE_MEM_SIZE (4 * 1024 * 1024)
/* MRP 代码将 char* 截断为 uint32，必须放在低 32-bit 地址 */
static char *fake_mem_ptr = NULL;
static int32_t stub_mem_get(char **base, uint32_t *len) {
    if (!fake_mem_ptr) {
        fake_mem_ptr = mmap((void*)0x10000000, FAKE_MEM_SIZE,
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        if (fake_mem_ptr == MAP_FAILED) {
            fake_mem_ptr = mmap(NULL, FAKE_MEM_SIZE, PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
        }
        memset(fake_mem_ptr, 0, FAKE_MEM_SIZE);
    }
    *base = fake_mem_ptr;
    *len = FAKE_MEM_SIZE;
    return 0;
}
static int32_t stub_mem_free(char *mem, uint32_t len) {
    (void)mem; (void)len; return 0;
}
static DSM_REQUIRE_FUNCS stub_dsm_funcs;

extern int32_t _mr_mem_init(void);

void test_dsm_init(void) {
    memset(&stub_dsm_funcs, 0, sizeof(stub_dsm_funcs));
    stub_dsm_funcs.log = stub_log;
    stub_dsm_funcs.get_uptime_ms = stub_get_uptime_ms;
    stub_dsm_funcs.timerStart = (void *)stub_timerStart;
    stub_dsm_funcs.timerStop = (void *)stub_timerStop;
    stub_dsm_funcs.sleep = (void *)stub_sleep;
    stub_dsm_funcs.getDatetime = (void *)stub_getDatetime;
    stub_dsm_funcs.rand = (void *)stub_rand;
    stub_dsm_funcs.srand = (void *)stub_srand;
    stub_dsm_funcs.exit = stub_exit;
    stub_dsm_funcs.mem_get = (void *)stub_mem_get;
    stub_dsm_funcs.mem_free = (void *)stub_mem_free;
    stub_dsm_funcs.drawBitmap = (void *)stub_drawBitmap;
    stub_dsm_funcs.open = (void *)stub_open;
    stub_dsm_funcs.close = (void *)stub_close;
    stub_dsm_funcs.read = (void *)stub_read;
    stub_dsm_funcs.write = (void *)stub_write;
    stub_dsm_funcs.seek = (void *)stub_seek;
    stub_dsm_funcs.info = (void *)stub_info;
    stub_dsm_funcs.remove = (void *)stub_remove;
    stub_dsm_funcs.rename = (void *)stub_rename;
    stub_dsm_funcs.mkDir = (void *)stub_mkDir;
    stub_dsm_funcs.rmDir = (void *)stub_rmDir;
    stub_dsm_funcs.getLen = (void *)stub_getLen;
    stub_dsm_funcs.opendir = (void *)stub_opendir;
    stub_dsm_funcs.readdir = (void *)stub_readdir;
    stub_dsm_funcs.closedir = (void *)stub_closedir;
    stub_dsm_funcs.initNetwork = (void *)stub_initNetwork;
    stub_dsm_funcs.getHostByName = (void *)stub_getHostByName;
    stub_dsm_funcs.mr_closeNetwork = (void *)stub_closeNetwork;
    stub_dsm_funcs.mr_socket = (void *)stub_socket;
    stub_dsm_funcs.mr_connect = (void *)stub_connect;
    stub_dsm_funcs.mr_getSocketState = (void *)stub_getSocketState;
    stub_dsm_funcs.mr_closeSocket = (void *)stub_closeSocket;
    stub_dsm_funcs.mr_recv = (void *)stub_recv;
    stub_dsm_funcs.mr_send = (void *)stub_send;
    stub_dsm_funcs.mr_recvfrom = (void *)stub_recvfrom;
    stub_dsm_funcs.mr_sendto = (void *)stub_sendto;
    stub_dsm_funcs.mr_dialogCreate = (void *)stub_dialogCreate;
    stub_dsm_funcs.mr_dialogRelease = (void *)stub_dialogRelease;
    stub_dsm_funcs.mr_dialogRefresh = (void *)stub_dialogRefresh;
    stub_dsm_funcs.mr_textCreate = (void *)stub_textCreate;
    stub_dsm_funcs.mr_textRelease = (void *)stub_textRelease;
    stub_dsm_funcs.mr_textRefresh = (void *)stub_textRefresh;
    stub_dsm_funcs.mr_editCreate = (void *)stub_editCreate;
    stub_dsm_funcs.mr_editRelease = (void *)stub_editRelease;
    stub_dsm_funcs.mr_editGetText = (void *)stub_editGetText;
    stub_dsm_funcs.mr_startShake = (void *)stub_mr_startShake;
    stub_dsm_funcs.mr_stopShake = (void *)stub_mr_stopShake;
    stub_dsm_funcs.mr_playSound = (void *)stub_mr_playSound;
    stub_dsm_funcs.mr_stopSound = (void *)stub_mr_stopSound;
    dsm_init(&stub_dsm_funcs);
    _mr_mem_init();
}

void mr_set_old_pack_filename(const char *n) { (void)n; }
void mr_set_old_start_filename(const char *n) { (void)n; }

/* ---------- GUI stubs ---------- */
uint16_t *mr_screenBuf = NULL;
int32_t mr_screen_w = 240;
int32_t mr_screen_h = 320;

void _DrawPoint(int16_t x, int16_t y, uint16_t c) { (void)x; (void)y; (void)c; }
void _DrawBitmap(uint16_t *p, int16_t x, int16_t y, uint16_t w, uint16_t h,
                 uint16_t rop, uint16_t tc, int16_t sx, int16_t sy, int16_t mw) {
    (void)p;(void)x;(void)y;(void)w;(void)h;(void)rop;(void)tc;(void)sx;(void)sy;(void)mw;
}
void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h,
              uint8_t r, uint8_t g, uint8_t b) {
    (void)x;(void)y;(void)w;(void)h;(void)r;(void)g;(void)b;
}
int32_t _DrawText(char *t, int16_t x, int16_t y, uint8_t r, uint8_t g,
                  uint8_t b, int u, uint16_t f) {
    (void)t;(void)x;(void)y;(void)r;(void)g;(void)b;(void)u;(void)f; return 0;
}
int32_t _DrawTextEx(char *t, int16_t x, int16_t y, void *rect,
                    void *color, int32_t flag, uint16_t font) {
    (void)t;(void)x;(void)y;(void)rect;(void)color;(void)flag;(void)font; return 0;
}
int _BitmapCheck(uint16_t *p, int16_t x, int16_t y, uint16_t w, uint16_t h,
                 uint16_t tc, uint16_t cc) {
    (void)p;(void)x;(void)y;(void)w;(void)h;(void)tc;(void)cc; return 0;
}
int32_t _DispUpEx(int16_t x, int16_t y, uint16_t w, uint16_t h) {
    (void)x;(void)y;(void)w;(void)h; return 0;
}

/* ---------- platform memory stubs ---------- */
int32_t native_mem_get(char **base, uint32_t *len) { return stub_mem_get(base, len); }
int32_t native_mem_free(char *mem, uint32_t len) { return stub_mem_free(mem, len); }

/* ---------- encode stubs ---------- */
int32_t encode_init(void) { return 0; }
char *UTF8StrToGBStr(const void *s, void *l) { (void)s; (void)l; return NULL; }
void *GBStrToUCS2BEStr(const void *s, void *l) { (void)s; (void)l; return NULL; }
char *UCS2BEStrToUTF8Str(const void *s, void *l) { (void)s; (void)l; return NULL; }
char *UCS2BEStrToGBStr(const void *s, void *l) { (void)s; (void)l; return NULL; }
