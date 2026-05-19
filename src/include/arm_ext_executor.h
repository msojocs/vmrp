#ifndef __VMRP_ARM_EXT_EXECUTOR_H__
#define __VMRP_ARM_EXT_EXECUTOR_H__

#if !defined(_TYPES_H) && !defined(_M_TYPE__)
#include "types.h"
#endif

typedef struct ArmExtModule ArmExtModule;

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code);
int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len);
void arm_ext_unload(ArmExtModule *m);

#endif
