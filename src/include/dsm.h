#ifndef _DSM_HOST_H
#define _DSM_HOST_H

/* 宿主侧 DSM 接口入口:屏幕尺寸取运行时配置(skyengine_config)。
 * Mythroad 子项目源码请包含 src/mythroad/include/dsm.h(固定尺寸版本)。
 * 原先两侧共用一个 dsm.h 靠 -DVMRP/-UVMRP 分叉,现拆分为两个入口,
 * 公共接口定义在 src/mythroad/include/dsm_defs.h。 */
#include "./types.h"
#include "./skyengine.h"

#define SCREEN_WIDTH (skyengine_config.screen_width)
#define SCREEN_HEIGHT (skyengine_config.screen_height)

#include "../mythroad/include/dsm_defs.h"

#endif
