#ifndef _DSM_DEFS_H
#define _DSM_DEFS_H

/* DSM 层与宿主共享的接口定义(函数表/事件码/原型)。
 * 本文件不含平台类型与屏幕尺寸宏,由包含方先行提供:
 *   - Mythroad 侧: include/dsm.h(mrporting.h + 固定 240x320)
 *   - 宿主侧: src/include/dsm.h(types.h + skyengine_config 动态尺寸)
 * 原先通过 -DVMRP/-UVMRP 在单个 dsm.h 内分叉,现拆为两个入口头文件,
 * 使 Mythroad 子项目不再依赖外层宏(见 AGENTS.MD"开发要求")。 */

#define VMRP_VER 20210701

enum {
    DSM_INIT = -100,
    MR_START_DSM,
    MR_PAUSEAPP,
    MR_RESUMEAPP,
    MR_TIMER,
    MR_EVENT
};

typedef enum {
    FLAG_USE_UTF8_FS = 1 << 0,
    FLAG_USE_UTF8_EDIT = 1 << 1
} dsm_flag;

typedef int32 (*NETWORK_CB)(int32 result, void *userData);

// 需要平台实现的函数(注意！调整定义的顺序必需相应调整skyengine中funcMap的顺序和偏移量)
typedef struct {
    void (*test)(void);
    void (*log)(char *msg);  // msg末尾不带\n
    void (*exit)(void);
    void (*srand)(uint32 seed);
    int32 (*rand)(void);
    int32 (*mem_get)(char **mem_base, uint32 *mem_len);
    int32 (*mem_free)(char *mem, uint32 mem_len);
    int32 (*timerStart)(uint16 t);
    int32 (*timerStop)(void);
    uint32 (*get_uptime_ms)(void);
    int32 (*getDatetime)(mr_datetime *datetime);
    int32 (*sleep)(uint32 ms);
    int32 (*open)(const char *filename, uint32 mode);
    int32 (*close)(int32 f);
    int32 (*read)(int32 f, void *p, uint32 l);
    int32 (*write)(int32 f, void *p, uint32 l);
    int32 (*seek)(int32 f, int32 pos, int method);
    int32 (*tell)(int32 f);
    int32 (*info)(const char *filename);
    int32 (*remove)(const char *filename);
    int32 (*rename)(const char *oldname, const char *newname);
    int32 (*mkDir)(const char *path);
    int32 (*rmDir)(const char *path);
#ifdef USE_FINDDIR
    int32 (*mrc_findStart)(const char *name, char *buffer, uint32 len);
    int32 (*mrc_findGetNext)(int32 h, char *buffer, uint32 len);
    int32 (*mrc_findStop)(int32 h);
#else
    int32 (*opendir)(const char *name);
    char *(*readdir)(int32 f);
    int32 (*closedir)(int32 f);
#endif
    int32 (*getLen)(const char *filename);
    void (*drawBitmap)(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
    int32 (*getHostByName)(const char *ptr, NETWORK_CB cb, void *userData);
    int32 (*initNetwork)(NETWORK_CB cb, const char *mode, void *userData);
    /* ()→(void):补全 C 原型,调用约定与 ABI 不变(消除 -Wstrict-prototypes) */
    int32 (*mr_closeNetwork)(void);
    int32 (*mr_socket)(int32 type, int32 protocol);
    int32 (*mr_connect)(int32 s, int32 ip, uint16 port, int32 type);
    int32 (*mr_getSocketState)(int32 s);
    int32 (*mr_closeSocket)(int32 s);
    int32 (*mr_recv)(int32 s, char *buf, int len);
    int32 (*mr_send)(int32 s, const char *buf, int len);
    int32 (*mr_recvfrom)(int32 s, char *buf, int len, int32 *ip, uint16 *port);
    int32 (*mr_sendto)(int32 s, const char *buf, int len, int32 ip, uint16 port);
    int32 (*mr_startShake)(int32 ms);
    int32 (*mr_stopShake)(void);
    int32 (*mr_playSound)(int type, const void *data, uint32 dataLen, int32 loop);
    int32 (*mr_stopSound)(int type);
    int32 (*mr_dialogCreate)(const char *title, const char *text, int32 type);
    int32 (*mr_dialogRelease)(int32 dialog);
    int32 (*mr_dialogRefresh)(int32 dialog, const char *title, const char *text, int32 type);
    int32 (*mr_textCreate)(const char *title, const char *text, int32 type);
    int32 (*mr_textRelease)(int32 text);
    int32 (*mr_textRefresh)(int32 handle, const char *title, const char *text);
    int32 (*mr_editCreate)(const char *title, const char *text, int32 type, int32 max_size);
    int32 (*mr_editRelease)(int32 edit);
    const char *(*mr_editGetText)(int32 edit);

#ifdef USE_GET_SCREEN_BUFFER
    uint16 *(*getScreenBuffer)(void);
#endif

    // 变量放在最后
    int32 flags;  // 调整运行时的一些参数，目前只有调整文件系统路径名是否使用UTF8编码这一个功能
    int32 screen_width;
    int32 screen_height;
} DSM_REQUIRE_FUNCS;

typedef struct event_t {
    int32 code;
    int32 p0;
    int32 p1;
} event_t;

typedef struct start_t {
    char *filename;
    char *ext;
    char *entry;
} start_t;

int32 dsm_init(DSM_REQUIRE_FUNCS *inFuncs);
int32 mr_restart_old_app(void);
/* 宿主编码路径 → guest(GBK) 编码；get_filename() 编码规则的逆操作，
 * 详见 dsm.c 中实现处的注释。 */
void dsm_host_path_to_guest(char *buf, uint32 bufsize, const char *host_path);
/* 当前 LCD 旋转(MR_LCD_ROTATE_*: 0=正常,1=90°,2=180°,3=270°)。guest 经
 * mr_plat(101,param) 设置(真机 LCD 驱动状态,由 DSM 层持有);宿主展示层
 * 据此计算旋转后的显示尺寸(skyengine_display_width/height)。 */
int32 dsm_get_lcd_rotation(void);
void dsm_set_lcd_rotation(int32 rotation);

/* 动感芯片(加速度传感器)接口,SKYENGINE 文档《动感芯片接口》与
 * mr_plat(4001~4006):4002 上电/4003 断电/4004 晃动监听/4005 倾斜监听/
 * 4001 停止监听/4006 量程查询。监听值经
 * mr_event(MR_MOTION_EVENT, MR_MOTION_EVENT_SHAKE|TILT, T_MOTION_ACC*) 上送。 */
#define MR_MOTION_EVENT_SHAKE 0
#define MR_MOTION_EVENT_TILT 1
/* 模拟器动感芯片量程:上送分量取值 ±DSM_MOTION_ACC_MAX,
 * plat(4006) 按契约返回 1000+DSM_MOTION_ACC_MAX。 */
#define DSM_MOTION_ACC_MAX 1000
/* 已上电且在监听时返回监听模式(MR_MOTION_EVENT_SHAKE/TILT),否则返回 -1 */
int32 dsm_motion_listening_mode(void);

#endif
