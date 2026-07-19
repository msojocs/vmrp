#ifndef __VMRP_RUNTIME_H__
#define __VMRP_RUNTIME_H__

#include <stdint.h>
#include <stddef.h>

#include "utils.h"

typedef struct VmrpRuntime {
    uc_engine *uc;
    void *impl;
} VmrpRuntime;

int skyengine_runtime_init(VmrpRuntime *rt);
int skyengine_runtime_start_dsm(VmrpRuntime *rt, const char *mrp, const char *ext, const char *entry);
int skyengine_runtime_event(VmrpRuntime *rt, int32_t code, int32_t p0, int32_t p1);
int skyengine_runtime_timer(VmrpRuntime *rt);
/* 动感芯片样本注入,x/y/z 取值 ±1000(plat 4006 量程契约) */
int skyengine_runtime_motion(VmrpRuntime *rt, int32_t x, int32_t y, int32_t z);
void skyengine_runtime_destroy(VmrpRuntime *rt);

#endif
