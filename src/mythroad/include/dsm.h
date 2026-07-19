#ifndef _DSM_H
#define _DSM_H

/* Mythroad/DSM 子项目入口头文件:仅依赖本目录内的 mrporting.h,
 * 屏幕尺寸为固定默认值(dsm.c 中仅作 dsmInFuncs->screen_width<=0 的回退)。
 * 宿主侧请包含 src/include/dsm.h(动态尺寸版本),不要包含本文件。 */
#include "mrporting.h"

#define DEFAULT_SCREEN_WIDTH 240
#define DEFAULT_SCREEN_HEIGHT 320
#define SCREEN_WIDTH DEFAULT_SCREEN_WIDTH
#define SCREEN_HEIGHT DEFAULT_SCREEN_HEIGHT

#include "dsm_defs.h"

#endif
