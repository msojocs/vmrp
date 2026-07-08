#ifndef __VMRP_BRIDGE_H__
#define __VMRP_BRIDGE_H__

#include <stdint.h>

#include "utils.h"

extern void guiDrawBitmap(uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h);
extern void guiDrawBitmapWithStride(uint16_t *bmp, int32_t x, int32_t y,
                                    int32_t w, int32_t h,
                                    int32_t source_stride,
                                    int32_t source_x,
                                    int32_t source_y);
extern int32_t timerStart(uint16_t t);
extern int32_t timerStop(void);
extern int32_t editCreate(const char *title, const char *text, int32_t type, int32_t max_size);
extern int32 editRelease(int32 edit);
extern char *editGetText(int32 edit);

int32_t bridge_dsm_network_cb(uc_engine *uc, uint32_t addr, int32_t p0, uint32_t p1);

#endif
