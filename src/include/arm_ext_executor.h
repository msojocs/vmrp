#ifndef __VMRP_ARM_EXT_EXECUTOR_H__
#define __VMRP_ARM_EXT_EXECUTOR_H__

#if !defined(_TYPES_H) && !defined(_M_TYPE__)
#include "types.h"
#endif

typedef struct ArmExtModule ArmExtModule;

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code);
int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len);
uint32 arm_ext_helper_addr(ArmExtModule *m);
int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out);
int arm_ext_invoke3(ArmExtModule *m, uint32 func, uint32 arg0, uint32 arg1,
                    uint32 arg2, int32 *ret_out);
void arm_ext_unload(ArmExtModule *m);

#endif
