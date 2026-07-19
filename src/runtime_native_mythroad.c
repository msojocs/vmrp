#include "./include/runtime.h"

#include <stdio.h>
#include <string.h>

#include "./include/native_dsm_funcs.h"
#include "./include/native_text_widget.h"
#include "./include/dsm.h"

extern int32 mr_start_dsm(char *filename, char *ext, char *entry);
extern int32 mr_event(int16 type, int32 param1, int32 param2);
extern int32 mr_timer(void);
extern int32 mr_motion_input(int32 x, int32 y, int32 z);

int skyengine_runtime_init(VmrpRuntime *rt) {
    memset(rt, 0, sizeof(*rt));
    int32 ret = dsm_init(native_dsm_funcs_get());
    if (ret != VMRP_VER) {
        printf("err: native dsm_version got %d expect %d\n", ret, VMRP_VER);
        return MR_FAILED;
    }
    printf("native dsm_init success\n");
    return MR_SUCCESS;
}

int skyengine_runtime_start_dsm(VmrpRuntime *rt, const char *mrp, const char *ext, const char *entry) {
    (void)rt;
    return mr_start_dsm((char *)mrp, (char *)ext, (char *)entry);
}

int skyengine_runtime_event(VmrpRuntime *rt, int32_t code, int32_t p0, int32_t p1) {
    (void)rt;
    /* 平台文本框(mr_textCreate)显示期间按真机语义接管输入:软键翻译为
     * MR_DIALOG_EVENT 投递给应用,其余输入事件被平台窗口消费。这里是
     * 全部前端入口(SDL/wasm/flutter)进入 guest 的唯一漏斗,保证行为一致。
     * MR_DIALOG_EVENT=6/MR_SUCCESS 见 include/types.h。 */
    int32_t dialog_param = 0;
    switch (native_text_widget_filter_event(code, p0, &dialog_param)) {
        case 1: /* 平台窗口消费(含滚动),不投递给应用 */
            return MR_SUCCESS;
        case 2: /* 软键命中文本框按钮 → 对话框事件 */
            return mr_event((int16)MR_DIALOG_EVENT, dialog_param, 0);
        default:
            break;
    }
    return mr_event((int16)code, p0, p1);
}

int skyengine_runtime_timer(VmrpRuntime *rt) {
    (void)rt;
    return mr_timer();
}

/* 动感芯片样本注入(语义见 mythroad.c mr_motion_input 注释) */
int skyengine_runtime_motion(VmrpRuntime *rt, int32_t x, int32_t y, int32_t z) {
    (void)rt;
    return mr_motion_input(x, y, z);
}

void skyengine_runtime_destroy(VmrpRuntime *rt) {
    native_dsm_funcs_destroy();
    memset(rt, 0, sizeof(*rt));
}
