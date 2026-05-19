#ifndef __VMRP_NATIVE_DSM_FUNCS_H__
#define __VMRP_NATIVE_DSM_FUNCS_H__

#include "../mythroad/include/dsm.h"

DSM_REQUIRE_FUNCS *native_dsm_funcs_get(void);
void native_dsm_funcs_destroy(void);

#endif
