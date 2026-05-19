#include "./include/runtime.h"

#include <stdio.h>
#include <string.h>

#include "./include/native_dsm_funcs.h"
#include "./mythroad/include/dsm.h"

extern int32 mr_start_dsm(char *filename, char *ext, char *entry);
extern int32 mr_event(int16 type, int32 param1, int32 param2);
extern int32 mr_timer(void);

int vmrp_runtime_init(VmrpRuntime *rt) {
    memset(rt, 0, sizeof(*rt));
    int32 ret = dsm_init(native_dsm_funcs_get());
    if (ret != VMRP_VER) {
        printf("err: native dsm_version got %d expect %d\n", ret, VMRP_VER);
        return MR_FAILED;
    }
    printf("native dsm_init success\n");
    return MR_SUCCESS;
}

int vmrp_runtime_start_dsm(VmrpRuntime *rt, const char *mrp, const char *ext, const char *entry) {
    (void)rt;
    return mr_start_dsm((char *)mrp, (char *)ext, (char *)entry);
}

int vmrp_runtime_event(VmrpRuntime *rt, int32_t code, int32_t p0, int32_t p1) {
    (void)rt;
    return mr_event((int16)code, p0, p1);
}

int vmrp_runtime_timer(VmrpRuntime *rt) {
    (void)rt;
    return mr_timer();
}

void vmrp_runtime_destroy(VmrpRuntime *rt) {
    native_dsm_funcs_destroy();
    memset(rt, 0, sizeof(*rt));
}
