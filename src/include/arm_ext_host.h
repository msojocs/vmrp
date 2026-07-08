#ifndef __VMRP_ARM_EXT_HOST_H__
#define __VMRP_ARM_EXT_HOST_H__

/*
 * ARM EXT 执行器对宿主(mythroad/DSM/bridge 层)的外部符号面(Phase 3 收敛)。
 * 原样收集自 arm_ext_executor.c 顶部 extern 块,供全部拆分单元共用;
 * 将来多实例化(refactor-plan Phase 5)时在此处收口为接口结构体。
 */

#include "arm_ext_executor.h" /* types.h(mr_datetime 等)与公开类型 */
#include "arm_ext_internal.h"

typedef int32 (*MR_INIT_NETWORK_CB_t)(int32 result);
typedef void (*MR_PLAT_EX_CB)(uint8 *output, int32 output_len);

extern void mr_printf(const char *format, ...);
extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern int32 mr_rand(void);
extern void mr_free(void *p, uint32 len);
extern int32 mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 mr_timerStart(uint16 t);
extern int32 mr_timerStop(void);
extern int32 mr_state;
extern int32 mr_timer_state;
extern int32 mr_stop_ex(int16 freemem);
extern uint32 mr_getTime(void);
extern int32 mr_getDatetime(mr_datetime *datetime);
extern int32 mr_getUserInfo(void *info);
extern void mr_md5_init(void *pms);
extern void mr_md5_append(void *pms, const void *data, int nbytes);
extern void mr_md5_finish(void *pms, void *digest);
extern int32 mr_sleep(uint32 ms);
extern int32 mr_plat(int32 code, int32 param);
extern const char *mr_get_pack_filename(void);
extern const char *mr_get_start_filename(void);
extern const char *mr_get_old_pack_filename(void);
extern const char *mr_get_old_start_filename(void);
extern const char *mr_get_start_fileparameter(void);
extern int32 mr_platEx(int32 code, uint8 *input, int32 input_len,
                       uint8 **output, int32 *output_len, MR_PLAT_EX_CB *cb);
extern int32 mr_ferrno(void);
extern int32 mr_open(const char *filename, uint32 mode);
extern int32 mr_close(int32 f);
extern int32 mr_info(const char *filename);
extern int32 mr_write(int32 f, void *p, uint32 l);
extern int32 mr_read(int32 f, void *p, uint32 l);
extern int32 mr_seek(int32 f, int32 pos, int method);
extern int32 mr_getLen(const char *filename);
extern int32 mr_remove(const char *filename);
extern int32 mr_rename(const char *oldname, const char *newname);
extern int32 mr_mkDir(const char *name);
extern int32 mr_rmDir(const char *name);
extern int32 mr_findStart(const char *name, char *buffer, uint32 len);
extern int32 mr_findGetNext(int32 search_handle, char *buffer, uint32 len);
extern int32 mr_findStop(int32 search_handle);
extern int32 mr_exit(void);
extern int32 mr_startShake(int32 ms);
extern int32 mr_stopShake(void);
extern int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop);
extern int32 mr_stopSound(int type);
extern void mr_call(char *number);
extern int32 mr_sendSms(char *pNumber, char *pContent, int32 encode);
extern int32 mr_dialogCreate(const char *title, const char *text, int32 type);
extern int32 mr_dialogRelease(int32 dialog);
extern int32 mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type);
extern int32 mr_textCreate(const char *title, const char *text, int32 type);
extern int32 mr_textRelease(int32 text);
extern int32 mr_textRefresh(int32 handle, const char *title, const char *text);
extern int32 mr_editCreate(const char *title, const char *text, int32 type, int32 max_size);
extern int32 mr_editRelease(int32 edit);
extern const char *mr_editGetText(int32 edit);
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);
extern int32 mr_getScreenInfo(void *s);
extern void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color);
extern int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
extern uint16 *c2u(const char *cp, int *err, int *size);
extern int wstrlen(char *txt);
extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *_mr_readFile(const char *filename, int *filelen, int lookfor);
extern void *mr_readFile_from_ram(const char *filename, int *filelen, int lookfor, char *ram_file, int ram_file_len);
extern void *mr_readFile_from_pack(const char *pack_filename, const char *filename, int *filelen, int lookfor);
extern char *LG_mem_base;
extern char *LG_mem_end;
extern uint8 *mr_gzInBuf;
extern uint8 *mr_gzOutBuf;
extern unsigned LG_gzinptr;
extern unsigned LG_gzoutcnt;
extern int32 mr_registerAPP(uint8 *p, int32 len, int32 index);
extern int _mr_TestCom(void *L, int input0, int input1);
extern int _mr_TestCom1(void *L, int input0, char *input1, int32 len);
extern int32 mr_socket(int32 type, int32 protocol);
extern int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type);
extern int32 mr_closeSocket(int32 s);
extern int32 mr_getSocketState(int32 s);
extern int32 mr_initNetwork(MR_INIT_NETWORK_CB_t cb, const char *mode);
extern int32 mr_recv(int32 s, char *buf, int len);
extern int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);
extern int32 mr_send(int32 s, const char *buf, int len);
extern int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);
extern int32 my_hasOpenSockets(void);
extern int32 my_openSocketCount(void);
extern const char *mr_get_last_written_mrp_path(void);
extern const uint8 *mr_get_last_written_mrp_data(uint32 *len);

#endif
