#ifndef __VMRP__H__
#define __VMRP__H__

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "skyengine_args.h"

typedef struct SkyEngineConfig {
    /* screen_width/height 始终是 --screen 配置的物理面板尺寸。guest 经
     * mr_plat(101,param) 设置的 LCD 旋转状态由 DSM 层持有
     * (dsm_get_lcd_rotation),展示层用 skyengine_display_width/height() 取
     * 旋转后的显示尺寸(奇数旋转为面板转置)。 */
    int screen_width;
    int screen_height;
    int memory_mb; /* 应用可见内存(MB):1/2/4/6/8/16,0 视为默认 */
    /* 非零时覆盖应用可见的年月日；时分秒仍来自宿主时钟。 */
    int device_year;
    int device_month;
    int device_day;
    char work_dir[PATH_MAX];
} SkyEngineConfig;

/* 应用可见内存(字节),memory_mb 为 0 时取默认值 */
static inline uint32_t SKYENGINE_MEMORY_bytes(int memory_mb) {
    int mb = memory_mb > 0 ? memory_mb : DEFAULT_MEMORY_MB;
    return (uint32_t)mb * 1024u * 1024u;
}

extern SkyEngineConfig skyengine_config;

/* 旋转后的显示尺寸(用户视角):真机 plat(101) 旋转后应用绘制转置画布,
 * LCD 控制器旋转扫描,用户横握手机所见即画布原样。模拟器展示层因此不做
 * 像素级旋转,只在奇数旋转(90°/270°)时把窗口/裁剪/行宽按面板转置处理。 */
int skyengine_display_width(void);
int skyengine_display_height(void);

int32_t event(int32_t code, int32_t p1, int32_t p2);
int32_t timer(void);
/* 动感芯片样本注入:x/y/z 为重力加速度分量,取值 ±1000(plat 4006 量程
 * 契约);guest 未经 plat(4004/4005) 开启监听时返回 MR_IGNORE。 */
int32_t skyengine_motion_input(int32_t x, int32_t y, int32_t z);
int configureSkyEngineDnsMap(const char *map);
int startEngine(const SkyEngineArgs *args);
void stopEngine(void);
void skyengine_request_exit(void);
int skyengine_is_exited(void);

#endif
