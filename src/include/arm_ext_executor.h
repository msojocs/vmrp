#ifndef __VMRP_ARM_EXT_EXECUTOR_H__
#define __VMRP_ARM_EXT_EXECUTOR_H__

#if !defined(_TYPES_H) && !defined(_M_TYPE__)
#include "types.h"
#endif

typedef struct ArmExtModule ArmExtModule;

/* ext_ret: ARM 代码 mr_c_function_load() 的返回值 (R0)，供 Lua 层使用 */
int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code, int32 *ext_ret);
int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len);
uint32 arm_ext_helper_addr(ArmExtModule *m);
/* 检查 ext 内部是否加载了嵌套 ext（primary），返回 primary helper 地址，0 表示无 */
uint32 arm_ext_primary_helper(ArmExtModule *m);
/* Host table[25] only records the child helper; consume this when mythroad.c
 * needs to deliver the one foreground code=0 call. Internal cfunction.ext
 * loaders initialize their child themselves and do not set this flag. */
int arm_ext_consume_primary_host_init(ArmExtModule *m);
/* 以 wrapper R9 调用嵌套 ext 的 timer dispatch 目标函数（不经过 unicorn hook） */
int arm_ext_call_dispatch(ArmExtModule *m, int is_stop, uint32 timer_interval);
int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out);
int arm_ext_invoke3(ArmExtModule *m, uint32 func, uint32 arg0, uint32 arg1,
                    uint32 arg2, int32 *ret_out);
uint32 arm_ext_read_timer_queue(ArmExtModule *m);
/* 清除 primary 以外的所有嵌套模块注册，并重置 active/timer 到 primary。
 * 用于 child 流程完整关闭后恢复到 game-only 状态。 */
void arm_ext_reset_child_modules(ArmExtModule *m);
void arm_ext_unload(ArmExtModule *m);
void arm_ext_host_cache_sync(ArmExtModule *m, const void *host_data, uint32 len);

#endif
