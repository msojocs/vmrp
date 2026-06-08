
#ifndef __PME_DEF_H__
#define __PME_DEF_H__

#include "MMI_features.h"
#include "pme.h"

#if defined( __IS_10A__)
#include "mmi_rp_app_pme_def.h"  //此文件是10A以上版本自动生成的
#elif defined(__IS_11A__) 
#include "mmi_rp_vapp_pme_def.h"  //此文件是10A以上版本自动生成的
#endif

#endif

