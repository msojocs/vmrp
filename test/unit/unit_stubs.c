/*
 * 单元测试链接桩(P0.2)。
 *
 * test_arm_ext.c 把整个 arm_ext_executor.c 编进测试进程,链接器因此需要
 * mythroad/DSM/bridge 层的全部外部符号。被测函数(纯内存操作)不应触达
 * 这些桥接口,所以除少数无害符号(mr_printf 直通 stdout、全局变量清零)
 * 外,桩一律打印后 abort —— 意外触达即测试大声失败,而不是静默通过。
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../src/include/vmrp.h"

/* vmrp_config 真身定义在 src/vmrp.c(依赖 runtime,不适合链入测试);
 * 这里给出零值实例,被测纯内存函数只读取 screen 尺寸等标量 */
VmrpConfig vmrp_config;

typedef int32_t int32;
typedef uint32_t uint32;
typedef int16_t int16;
typedef uint16_t uint16;
typedef uint8_t uint8;

#define STUB_ABORT()                                             \
    do {                                                         \
        fprintf(stderr, "unit stub %s unexpectedly called\n",    \
                __func__);                                       \
        abort();                                                 \
    } while (0)

/* ---- 无害桩 ---- */
void mr_printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

/* ---- 全局变量(清零即可) ---- */
uint16 *mr_screenBuf;
int32 mr_screen_w;
int32 mr_screen_h;
int32 mr_state;
int32 mr_timer_state;
char *LG_mem_base;
char *LG_mem_end;
uint8 *mr_gzInBuf;
uint8 *mr_gzOutBuf;
unsigned LG_gzinptr;
unsigned LG_gzoutcnt;

/* ---- 一触即 abort 的函数桩 ---- */
int32 mr_rand(void) { STUB_ABORT(); }
void mr_free(void *p, uint32 len) { (void)p; (void)len; STUB_ABORT(); }
int32 mr_mem_get(char **mem_base, uint32 *mem_len) { (void)mem_base; (void)mem_len; STUB_ABORT(); }
int32 mr_timerStart(uint16 t) { (void)t; STUB_ABORT(); }
int32 mr_timerStop(void) { STUB_ABORT(); }
int32 mr_stop_ex(int16 freemem) { (void)freemem; STUB_ABORT(); }
uint32 mr_getTime(void) { STUB_ABORT(); }
int32 mr_getDatetime(void *datetime) { (void)datetime; STUB_ABORT(); }
int32 mr_getUserInfo(void *info) { (void)info; STUB_ABORT(); }
void mr_md5_init(void *pms) { (void)pms; STUB_ABORT(); }
void mr_md5_append(void *pms, const void *data, int nbytes) { (void)pms; (void)data; (void)nbytes; STUB_ABORT(); }
void mr_md5_finish(void *pms, void *digest) { (void)pms; (void)digest; STUB_ABORT(); }
int32 mr_sleep(uint32 ms) { (void)ms; STUB_ABORT(); }
int32 mr_plat(int32 code, int32 param) { (void)code; (void)param; STUB_ABORT(); }
const char *mr_get_pack_filename(void) { STUB_ABORT(); }
const char *mr_get_start_filename(void) { STUB_ABORT(); }
const char *mr_get_old_pack_filename(void) { STUB_ABORT(); }
const char *mr_get_old_start_filename(void) { STUB_ABORT(); }
const char *mr_get_start_fileparameter(void) { STUB_ABORT(); }
int32 mr_platEx(int32 code, uint8 *input, int32 input_len, uint8 **output,
                int32 *output_len, void **cb) {
    (void)code; (void)input; (void)input_len; (void)output; (void)output_len; (void)cb;
    STUB_ABORT();
}
int32 mr_ferrno(void) { STUB_ABORT(); }
int32 mr_open(const char *filename, uint32 mode) { (void)filename; (void)mode; STUB_ABORT(); }
int32 mr_close(int32 f) { (void)f; STUB_ABORT(); }
int32 mr_info(const char *filename) { (void)filename; STUB_ABORT(); }
int32 mr_write(int32 f, void *p, uint32 l) { (void)f; (void)p; (void)l; STUB_ABORT(); }
int32 mr_read(int32 f, void *p, uint32 l) { (void)f; (void)p; (void)l; STUB_ABORT(); }
int32 mr_seek(int32 f, int32 pos, int method) { (void)f; (void)pos; (void)method; STUB_ABORT(); }
int32 mr_getLen(const char *filename) { (void)filename; STUB_ABORT(); }
int32 mr_remove(const char *filename) { (void)filename; STUB_ABORT(); }
int32 mr_rename(const char *oldname, const char *newname) { (void)oldname; (void)newname; STUB_ABORT(); }
int32 mr_mkDir(const char *name) { (void)name; STUB_ABORT(); }
int32 mr_rmDir(const char *name) { (void)name; STUB_ABORT(); }
int32 mr_findStart(const char *name, char *buffer, uint32 len) { (void)name; (void)buffer; (void)len; STUB_ABORT(); }
int32 mr_findGetNext(int32 search_handle, char *buffer, uint32 len) { (void)search_handle; (void)buffer; (void)len; STUB_ABORT(); }
int32 mr_findStop(int32 search_handle) { (void)search_handle; STUB_ABORT(); }
int32 mr_exit(void) { STUB_ABORT(); }
int32 mr_startShake(int32 ms) { (void)ms; STUB_ABORT(); }
int32 mr_stopShake(void) { STUB_ABORT(); }
int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop) { (void)type; (void)data; (void)dataLen; (void)loop; STUB_ABORT(); }
int32 mr_stopSound(int type) { (void)type; STUB_ABORT(); }
void mr_call(char *number) { (void)number; STUB_ABORT(); }
int32 mr_sendSms(char *pNumber, char *pContent, int32 encode) { (void)pNumber; (void)pContent; (void)encode; STUB_ABORT(); }
int32 mr_dialogCreate(const char *title, const char *text, int32 type) { (void)title; (void)text; (void)type; STUB_ABORT(); }
int32 mr_dialogRelease(int32 dialog) { (void)dialog; STUB_ABORT(); }
int32 mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type) { (void)dialog; (void)title; (void)text; (void)type; STUB_ABORT(); }
int32 mr_textCreate(const char *title, const char *text, int32 type) { (void)title; (void)text; (void)type; STUB_ABORT(); }
int32 mr_textRelease(int32 text) { (void)text; STUB_ABORT(); }
int32 mr_textRefresh(int32 handle, const char *title, const char *text) { (void)handle; (void)title; (void)text; STUB_ABORT(); }
int32 mr_editCreate(const char *title, const char *text, int32 type, int32 max_size) { (void)title; (void)text; (void)type; (void)max_size; STUB_ABORT(); }
int32 mr_editRelease(int32 edit) { (void)edit; STUB_ABORT(); }
const char *mr_editGetText(int32 edit) { (void)edit; STUB_ABORT(); }
void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h) { (void)bmp; (void)x; (void)y; (void)w; (void)h; STUB_ABORT(); }
const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height) { (void)ch; (void)fontSize; (void)width; (void)height; STUB_ABORT(); }
int32 mr_getScreenInfo(void *s) { (void)s; STUB_ABORT(); }
void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color) { (void)ch; (void)x; (void)y; (void)color; STUB_ABORT(); }
int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h) { (void)x; (void)y; (void)w; (void)h; STUB_ABORT(); }
uint16 *c2u(const char *cp, int *err, int *size) { (void)cp; (void)err; (void)size; STUB_ABORT(); }
int wstrlen(char *txt) { (void)txt; STUB_ABORT(); }
void _DrawPoint(int16 x, int16 y, uint16 nativecolor) { (void)x; (void)y; (void)nativecolor; STUB_ABORT(); }
void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop,
                 uint16 transcoler, int16 sx, int16 sy, int16 mw) {
    (void)p; (void)x; (void)y; (void)w; (void)h; (void)rop; (void)transcoler; (void)sx; (void)sy; (void)mw;
    STUB_ABORT();
}
void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b) { (void)x; (void)y; (void)w; (void)h; (void)r; (void)g; (void)b; STUB_ABORT(); }
int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font) {
    (void)pcText; (void)x; (void)y; (void)r; (void)g; (void)b; (void)is_unicode; (void)font;
    STUB_ABORT();
}
int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check) {
    (void)p; (void)x; (void)y; (void)w; (void)h; (void)transcoler; (void)color_check;
    STUB_ABORT();
}
void *_mr_readFile(const char *filename, int *filelen, int lookfor) { (void)filename; (void)filelen; (void)lookfor; STUB_ABORT(); }
void *mr_readFile_from_ram(const char *filename, int *filelen, int lookfor, char *ram_file, int ram_file_len) {
    (void)filename; (void)filelen; (void)lookfor; (void)ram_file; (void)ram_file_len;
    STUB_ABORT();
}
void *mr_readFile_from_pack(const char *pack_filename, const char *filename, int *filelen, int lookfor) {
    (void)pack_filename; (void)filename; (void)filelen; (void)lookfor;
    STUB_ABORT();
}
int32 mr_registerAPP(uint8 *p, int32 len, int32 index) { (void)p; (void)len; (void)index; STUB_ABORT(); }
int _mr_TestCom(void *L, int input0, int input1) { (void)L; (void)input0; (void)input1; STUB_ABORT(); }
int _mr_TestCom1(void *L, int input0, char *input1, int32 len) { (void)L; (void)input0; (void)input1; (void)len; STUB_ABORT(); }
int32 mr_socket(int32 type, int32 protocol) { (void)type; (void)protocol; STUB_ABORT(); }
int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type) { (void)s; (void)ip; (void)port; (void)type; STUB_ABORT(); }
int32 mr_closeSocket(int32 s) { (void)s; STUB_ABORT(); }
int32 mr_getSocketState(int32 s) { (void)s; STUB_ABORT(); }
int32 mr_initNetwork(void *cb, const char *mode) { (void)cb; (void)mode; STUB_ABORT(); }
int32 mr_recv(int32 s, char *buf, int len) { (void)s; (void)buf; (void)len; STUB_ABORT(); }
int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port) { (void)s; (void)buf; (void)len; (void)ip; (void)port; STUB_ABORT(); }
int32 mr_send(int32 s, const char *buf, int len) { (void)s; (void)buf; (void)len; STUB_ABORT(); }
int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port) { (void)s; (void)buf; (void)len; (void)ip; (void)port; STUB_ABORT(); }
const char *mr_get_last_written_mrp_path(void) { STUB_ABORT(); }
const uint8 *mr_get_last_written_mrp_data(uint32 *len) { (void)len; STUB_ABORT(); }
void guiDrawBitmapWithStride(uint16_t *bmp, int32_t x, int32_t y, int32_t w,
                             int32_t h, int32_t source_stride,
                             int32_t source_x, int32_t source_y) {
    (void)bmp; (void)x; (void)y; (void)w; (void)h; (void)source_stride; (void)source_x; (void)source_y;
    STUB_ABORT();
}
int32_t bridge_dsm_network_cb(void *uc, uint32_t addr, int32_t p0, uint32_t p1) {
    (void)uc; (void)addr; (void)p0; (void)p1;
    STUB_ABORT();
}
