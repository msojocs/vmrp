#include "./include/runtime.h"

#include <stdio.h>
#include <string.h>

#include "./include/bridge.h"
#include "./include/fileLib.h"
#include "./include/memory.h"
#include "./include/vmrp.h"

extern uc_engine *initVmrp(void);
extern int32_t loadCode(uc_engine *uc);

int vmrp_runtime_init(VmrpRuntime *rt) {
    memset(rt, 0, sizeof(*rt));
    rt->uc = initVmrp();
    if (rt->uc == NULL) {
        printf("initVmrp() fail.\n");
        return MR_FAILED;
    }
    if (loadCode(rt->uc) == MR_FAILED) {
        printf("loadCode fail.\n");
        return MR_FAILED;
    }
    bridge_ext_init(rt->uc);
    if (bridge_dsm_init(rt->uc) == MR_SUCCESS) {
        printf("bridge_dsm_init success\n");
        return MR_SUCCESS;
    }
    return MR_FAILED;
}

int vmrp_runtime_start_dsm(VmrpRuntime *rt, const char *mrp, const char *ext, const char *entry) {
    return bridge_dsm_mr_start_dsm(rt->uc, (char *)mrp, (char *)ext, (char *)entry);
}

int vmrp_runtime_event(VmrpRuntime *rt, int32_t code, int32_t p0, int32_t p1) {
    if (rt->uc) return bridge_dsm_mr_event(rt->uc, code, p0, p1);
    return MR_FAILED;
}

int vmrp_runtime_timer(VmrpRuntime *rt) {
    if (rt->uc) return bridge_dsm_mr_timer(rt->uc);
    return MR_FAILED;
}

void vmrp_runtime_destroy(VmrpRuntime *rt) {
    if (rt->uc) {
        freeVmrp(rt->uc);
    }
    memset(rt, 0, sizeof(*rt));
}
