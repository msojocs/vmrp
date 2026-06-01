#ifndef __Fix_R9__
#define __Fix_R9__

#include "mr.h"
#include "mr_graphics.h"
#include "mr_gzip.h"
#include "mr_helper.h"
#include "mythroad.h"
#include "type.h"

void fixR9_saveMythroad(void);

#define getR9() NULL
#define setR9(value) ((void)(value))
#define getR10() NULL
#define setR10(value) ((void)(value))
#define setR9R10(r9, r10) ((void)(r9), (void)(r10))

#define asm_mr_malloc mr_malloc
#define asm_mr_free mr_free
#define asm_mr_getDatetime mr_getDatetime
#define asm_mr_sleep mr_sleep
#define asm_DrawRect DrawRect
#define asm_mr_drawBitmap mr_drawBitmap
#define asm_mr_getScreenInfo mr_getScreenInfo
#define asm_mr_smsSetBytes _mr_smsSetBytes
#define asm_mr_smsAddNum _mr_smsAddNum
#define asm_mr_newSIMInd _mr_newSIMInd
#define asm_mr_isMr _mr_isMr
#define asm_mr_realloc mr_realloc
#define asm_mr_rand mr_rand
#define asm_mr_stop_ex mr_stop_ex
#define asm_mr_printf mr_printf
#define asm_mr_mem_get mr_mem_get
#define asm_mr_mem_free mr_mem_free
#define asm_mr_getCharBitmap mr_getCharBitmap
#define asm_mr_timerStart mr_timerStart
#define asm_mr_timerStop mr_timerStop
#define asm_mr_getTime mr_getTime
#define asm_mr_getUserInfo mr_getUserInfo
#define asm_mr_plat mr_plat
#define asm_mr_platEx mr_platEx
#define asm_mr_open mr_open
#define asm_mr_close mr_close
#define asm_mr_read mr_read
#define asm_mr_write mr_write
#define asm_mr_seek mr_seek
#define asm_mr_info mr_info
#define asm_mr_remove mr_remove
#define asm_mr_rename mr_rename
#define asm_mr_mkDir mr_mkDir
#define asm_mr_rmDir mr_rmDir
#define asm_mr_findGetNext mr_findGetNext
#define asm_mr_findStop mr_findStop
#define asm_mr_findStart mr_findStart
#define asm_mr_getLen mr_getLen
#define asm_mr_exit mr_exit
#define asm_mr_startShake mr_startShake
#define asm_mr_stopShake mr_stopShake
#define asm_mr_playSound mr_playSound
#define asm_mr_stopSound mr_stopSound
#define asm_mr_sendSms mr_sendSms
#define asm_mr_call mr_call
#define asm_mr_connectWAP mr_connectWAP
#define asm_mr_dialogCreate mr_dialogCreate
#define asm_mr_dialogRelease mr_dialogRelease
#define asm_mr_dialogRefresh mr_dialogRefresh
#define asm_mr_textCreate mr_textCreate
#define asm_mr_textRelease mr_textRelease
#define asm_mr_textRefresh mr_textRefresh
#define asm_mr_editCreate mr_editCreate
#define asm_mr_editRelease mr_editRelease
#define asm_mr_editGetText mr_editGetText
#define asm_mr_initNetwork mr_initNetwork
#define asm_mr_closeNetwork mr_closeNetwork
#define asm_mr_getHostByName mr_getHostByName
#define asm_mr_socket mr_socket
#define asm_mr_connect mr_connect
#define asm_mr_closeSocket mr_closeSocket
#define asm_mr_recv mr_recv
#define asm_mr_recvfrom mr_recvfrom
#define asm_mr_send mr_send
#define asm_mr_sendto mr_sendto
#define asm_mr_load_sms_cfg _mr_load_sms_cfg
#define asm_mr_save_sms_cfg _mr_save_sms_cfg
#define asm_DispUpEx _DispUpEx
#define asm_DrawPoint _DrawPoint
#define asm_DrawBitmap _DrawBitmap
#define asm_DrawBitmapEx _DrawBitmapEx
#define asm_DrawText _DrawText
#define asm_BitmapCheck _BitmapCheck
#define asm_mr_readFile _mr_readFile
#define asm_mr_registerAPP mr_registerAPP
#define asm_DrawTextEx _DrawTextEx
#define asm_mr_EffSetCon _mr_EffSetCon
#define asm_mr_TestCom _mr_TestCom
#define asm_mr_TestCom1 _mr_TestCom1
#define asm_c2u c2u
#define asm_mr_updcrc mr_updcrc
#define asm_mr_unzip mr_unzip
#define asm_mr_platDrawChar mr_platDrawChar
#define asm_mr_drawRegion mr_drawRegion
#define asm_mr_transbitmapDraw mr_transbitmapDraw

#define asm_mrp_gettop mrp_gettop
#define asm_mrp_settop mrp_settop
#define asm_mrp_pushvalue mrp_pushvalue
#define asm_mrp_remove mrp_remove
#define asm_mrp_insert mrp_insert
#define asm_mrp_replace mrp_replace
#define asm_mrp_isnumber mrp_isnumber
#define asm_mrp_isstring mrp_isstring
#define asm_mrp_iscfunction mrp_iscfunction
#define asm_mrp_isuserdata mrp_isuserdata
#define asm_mrp_type mrp_type
#define asm_mrp_typename mrp_typename
#define asm_mrp_shorttypename mrp_shorttypename
#define asm_mrp_equal mrp_equal
#define asm_mrp_rawequal mrp_rawequal
#define asm_mrp_lessthan mrp_lessthan
#define asm_mrp_tonumber mrp_tonumber
#define asm_mrp_toboolean mrp_toboolean
#define asm_mrp_tostring mrp_tostring
#define asm_mrp_strlen mrp_strlen
#define asm_mrp_tostring_t mrp_tostring_t
#define asm_mrp_strlen_t mrp_strlen_t
#define asm_mrp_tocfunction mrp_tocfunction
#define asm_mrp_touserdata mrp_touserdata
#define asm_mrp_tothread mrp_tothread
#define asm_mrp_topointer mrp_topointer
#define asm_mrp_pushnil mrp_pushnil
#define asm_mrp_pushnumber mrp_pushnumber
#define asm_mrp_pushlstring mrp_pushlstring
#define asm_mrp_pushstring mrp_pushstring
#define asm_mrp_pushvfstring mrp_pushvfstring
#define asm_mrp_pushfstring mrp_pushfstring
#define asm_mrp_pushboolean mrp_pushboolean
#define asm_mrp_pushcclosure mrp_pushcclosure
#define asm_mrp_gettable mrp_gettable
#define asm_mrp_rawget mrp_rawget
#define asm_mrp_rawgeti mrp_rawgeti
#define asm_mrp_newtable mrp_newtable
#define asm_mrp_getmetatable mrp_getmetatable
#define asm_mrp_settable mrp_settable
#define asm_mrp_rawset mrp_rawset
#define asm_mrp_rawseti mrp_rawseti
#define asm_mrp_call mrp_call
#define asm_mrp_pcall mrp_pcall
#define asm_mrp_load mrp_load
#define asm_mrp_getgcthreshold mrp_getgcthreshold
#define asm_mrp_setgcthreshold mrp_setgcthreshold
#define asm_mrp_error mrp_error
#define asm_mrp_checkstack mrp_checkstack
#define asm_mrp_newuserdata mrp_newuserdata
#define asm_mrp_getfenv mrp_getfenv
#define asm_mrp_setfenv mrp_setfenv
#define asm_mrp_setmetatable mrp_setmetatable
#define asm_mrp_cpcall mrp_cpcall
#define asm_mrp_next mrp_next
#define asm_mrp_concat mrp_concat
#define asm_mrp_pushlightuserdata mrp_pushlightuserdata
#define asm_mrp_getgccount mrp_getgccount
#define asm_mrp_dump mrp_dump
#define asm_mrp_yield mrp_yield
#define asm_mrp_resume mrp_resume

extern void *asm_mr_malloc(uint32 len);
extern void asm_mr_free(void *p, uint32 len);
extern int32 asm_mr_getDatetime(mr_datetime *datetime);
extern int32 asm_mr_sleep(uint32 ms);
extern void asm_DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern void asm_mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern int32 asm_DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int32 asm_mr_getScreenInfo(mr_screeninfo *s);
extern int32 asm_mr_smsSetBytes(int32 pos, char *p, int32 len);
extern int32 asm_mr_smsAddNum(int32 index, char *pNum);
extern int32 asm_mr_newSIMInd(int16 type, uint8 *old_IMSI);
extern int asm_mr_isMr(char *input);
extern void *asm_mr_realloc(void *p, uint32 oldlen, uint32 len);
extern int32 asm_mr_rand(void);
extern int32 asm_mr_stop_ex(int16 freemem);
extern void asm_mr_printf(const char *format, ...);
extern int32 asm_mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 asm_mr_mem_free(char *mem, uint32 mem_len);
extern const char *asm_mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);
extern int32 asm_mr_timerStart(uint16 t);
extern int32 asm_mr_timerStop(void);
extern uint32 asm_mr_getTime(void);
extern int32 asm_mr_getUserInfo(mr_userinfo *info);
extern int32 asm_mr_plat(int32 code, int32 param);
extern int32 asm_mr_platEx(int32 code, uint8 *input, int32 input_len, uint8 **output, int32 *output_len, MR_PLAT_EX_CB *cb);
extern int32 asm_mr_open(const char *filename, uint32 mode);
extern int32 asm_mr_close(int32 f);
extern int32 asm_mr_read(int32 f, void *p, uint32 l);
extern int32 asm_mr_write(int32 f, void *p, uint32 l);
extern int32 asm_mr_seek(int32 f, int32 pos, int method);
extern int32 asm_mr_info(const char *filename);
extern int32 asm_mr_remove(const char *filename);
extern int32 asm_mr_rename(const char *oldname, const char *newname);
extern int32 asm_mr_mkDir(const char *name);
extern int32 asm_mr_rmDir(const char *name);
extern int32 asm_mr_findGetNext(int32 search_handle, char *buffer, uint32 len);
extern int32 asm_mr_findStop(int32 search_handle);
extern int32 asm_mr_findStart(const char *name, char *buffer, uint32 len);
extern int32 asm_mr_getLen(const char *filename);
extern int32 asm_mr_exit(void);
extern int32 asm_mr_startShake(int32 ms);
extern int32 asm_mr_stopShake(void);
extern int32 asm_mr_playSound(int type, const void *data, uint32 dataLen, int32 loop);
extern int32 asm_mr_stopSound(int type);
extern int32 asm_mr_sendSms(char *pNumber, char *pContent, int32 encode);
extern void asm_mr_call(char *number);
extern void asm_mr_connectWAP(char *wap);
extern int32 asm_mr_dialogCreate(const char *title, const char *text, int32 type);
extern int32 asm_mr_dialogRelease(int32 dialog);
extern int32 asm_mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type);
extern int32 asm_mr_textCreate(const char *title, const char *text, int32 type);
extern int32 asm_mr_textRelease(int32 text);
extern int32 asm_mr_textRefresh(int32 handle, const char *title, const char *text);
extern int32 asm_mr_editCreate(const char *title, const char *text, int32 type, int32 max_size);
extern int32 asm_mr_editRelease(int32 edit);
extern const char *asm_mr_editGetText(int32 edit);
extern int32 asm_mr_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode);
extern int32 asm_mr_closeNetwork(void);
extern int32 asm_mr_getHostByName(const char* name, MR_GET_HOST_CB cb);
extern int32 asm_mr_socket(int32 type, int32 protocol);
extern int32 asm_mr_connect(int32 s, int32 ip, uint16 port, int32 type);
extern int32 asm_mr_closeSocket(int32 s);
extern int32 asm_mr_recv(int32 s, char *buf, int len);
extern int32 asm_mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);
extern int32 asm_mr_send(int32 s, const char *buf, int len);
extern int32 asm_mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);
extern int32 asm_mr_load_sms_cfg(void);
extern int32 asm_mr_save_sms_cfg(int32 f);
extern int32 asm_DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
extern void asm_DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void asm_DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void asm_DrawBitmapEx(mr_bitmapDrawSt *srcbmp, mr_bitmapDrawSt *dstbmp, uint16 w, uint16 h, mr_transMatrixSt *pTrans, uint16 transcoler);
extern int asm_BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *asm_mr_readFile(const char *filename, int *filelen, int lookfor);
extern int32 asm_mr_registerAPP(uint8 *p, int32 len, int32 index);
extern int32 asm_DrawTextEx(char *pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);
extern int asm_mr_EffSetCon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);
extern int asm_mr_TestCom(mrp_State *L, int input0, int input1);
extern int asm_mr_TestCom1(mrp_State *L, int input0, char *input1, int32 len);
extern uint16 *asm_c2u(const char *cp, int *err, int *size);
extern uint32 asm_mr_updcrc(uint8 *s, unsigned n);
extern int asm_mr_unzip(void);
extern int32 asm_mr_transbitmapDraw(mr_transBitmap *hTransBmp, uint16 *dstBuf, int32 dest_max_w, int32 dest_max_h, int32 sx, int32 sy, int32 width, int32 height, int32 dx, int32 dy);
extern void asm_mr_drawRegion(mr_jgraphics_context_t *gContext, mr_jImageSt *src, int sx, int sy, int w, int h, int transform, int x, int y, int anchor);
extern void asm_mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color);

MRP_API int asm_mrp_gettop(mrp_State *L);
MRP_API void asm_mrp_settop(mrp_State *L, int idx);
MRP_API void asm_mrp_pushvalue(mrp_State *L, int idx);
MRP_API void asm_mrp_remove(mrp_State *L, int idx);
MRP_API void asm_mrp_insert(mrp_State *L, int idx);
MRP_API void asm_mrp_replace(mrp_State *L, int idx);
MRP_API int asm_mrp_isnumber(mrp_State *L, int idx);
MRP_API int asm_mrp_isstring(mrp_State *L, int idx);
MRP_API int asm_mrp_iscfunction(mrp_State *L, int idx);
MRP_API int asm_mrp_isuserdata(mrp_State *L, int idx);
MRP_API int asm_mrp_type(mrp_State *L, int idx);
MRP_API const char *asm_mrp_typename(mrp_State *L, int t);
MRP_API const char *asm_mrp_shorttypename(mrp_State *L, int t);
MRP_API int asm_mrp_equal(mrp_State *L, int index1, int index2);
MRP_API int asm_mrp_rawequal(mrp_State *L, int index1, int index2);
MRP_API int asm_mrp_lessthan(mrp_State *L, int index1, int index2);
MRP_API mrp_Number asm_mrp_tonumber(mrp_State *L, int idx);
MRP_API int asm_mrp_toboolean(mrp_State *L, int idx);
MRP_API const char *asm_mrp_tostring(mrp_State *L, int idx);
MRP_API size_t asm_mrp_strlen(mrp_State *L, int idx);
MRP_API const char *asm_mrp_tostring_t(mrp_State *L, int idx);
MRP_API size_t asm_mrp_strlen_t(mrp_State *L, int idx);
MRP_API mrp_CFunction asm_mrp_tocfunction(mrp_State *L, int idx);
MRP_API void *asm_mrp_touserdata(mrp_State *L, int idx);
MRP_API mrp_State *asm_mrp_tothread(mrp_State *L, int idx);
MRP_API const void *asm_mrp_topointer(mrp_State *L, int idx);
MRP_API void asm_mrp_pushnil(mrp_State *L);
MRP_API void asm_mrp_pushnumber(mrp_State *L, mrp_Number n);
MRP_API void asm_mrp_pushlstring(mrp_State *L, const char *s, size_t len);
MRP_API void asm_mrp_pushstring(mrp_State *L, const char *s);
MRP_API const char *asm_mrp_pushvfstring(mrp_State *L, const char *fmt, va_list argp);
MRP_API const char *asm_mrp_pushfstring(mrp_State *L, const char *fmt, ...);
MRP_API void asm_mrp_pushboolean(mrp_State *L, int b);
MRP_API void asm_mrp_pushcclosure(mrp_State *L, mrp_CFunction fn, int n);
MRP_API void asm_mrp_gettable(mrp_State *L, int idx);
MRP_API void asm_mrp_rawget(mrp_State *L, int idx);
MRP_API void asm_mrp_rawgeti(mrp_State *L, int idx, int n);
MRP_API void asm_mrp_newtable(mrp_State *L);
MRP_API int asm_mrp_getmetatable(mrp_State *L, int objindex);
MRP_API void asm_mrp_settable(mrp_State *L, int idx);
MRP_API void asm_mrp_rawset(mrp_State *L, int idx);
MRP_API void asm_mrp_rawseti(mrp_State *L, int idx, int n);
MRP_API void asm_mrp_call(mrp_State *L, int nargs, int nresults);
MRP_API int asm_mrp_pcall(mrp_State *L, int nargs, int nresults, int errfunc);
MRP_API int asm_mrp_load(mrp_State *L, mrp_Chunkreader reader, void *data, const char *chunkname);
MRP_API int asm_mrp_getgcthreshold(mrp_State *L);
MRP_API void asm_mrp_setgcthreshold(mrp_State *L, int newthreshold);
MRP_API int asm_mrp_error(mrp_State *L);
MRP_API int asm_mrp_checkstack(mrp_State *L, int size);
MRP_API void *asm_mrp_newuserdata(mrp_State *L, size_t size);
MRP_API void asm_mrp_getfenv(mrp_State *L, int idx);
MRP_API int asm_mrp_setfenv(mrp_State *L, int idx);
MRP_API int asm_mrp_setmetatable(mrp_State *L, int objindex);
MRP_API int asm_mrp_cpcall(mrp_State *L, mrp_CFunction func, void *ud);
MRP_API int asm_mrp_next(mrp_State *L, int idx);
MRP_API void asm_mrp_concat(mrp_State *L, int n);
MRP_API void asm_mrp_pushlightuserdata(mrp_State *L, void *p);
MRP_API int asm_mrp_getgccount(mrp_State *L);
MRP_API int asm_mrp_dump(mrp_State *L, mrp_Chunkwriter writer, void *data);
MRP_API int asm_mrp_yield(mrp_State *L, int nresults);
MRP_API int asm_mrp_resume(mrp_State *L, int nargs);

#endif
