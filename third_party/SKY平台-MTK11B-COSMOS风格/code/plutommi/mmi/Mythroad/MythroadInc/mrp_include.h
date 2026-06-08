#ifndef __MRP_INCLUDE_H__
#define __MRP_INCLUDE_H__

#ifdef __MMI_DSM_NEW__

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	MR_FALSE = 0,
	MR_TRUE
}MR_BOOL;


#if defined( MMI_ON_HARDWARE_P ) && !defined(__MR_CFG_FEATURE_SLIM__)
#define mr_trace                 mr_printf
#else
#define mr_trace(...)
#endif

#include "MMIDataType.h"//huangsunbo 20120613
#include "mrp_features.h"
#include "mrporting.h"
#include "mmidsmdef.h"

#include "mmidsmapi.h"
#include "mrp_pal.h"
#include "mrp_core.h"
#include "mrp_misc.h"
#include "mrp_fmgr.h"
#include "mrp_media.h"
#include "mrp_phonebook.h"
#include "mrp_util.h"
#include "mrp_apn.h"
#include "mrp_backstage.h"
#include "mrp_extension.h"
#include "mrp_fs.h"
#include "mrp_sms.h"
#include "mrp_localui.h"
#include "mrp_net.h"
#include "mrp_unet.h"
#include "mrp_image.h"
#include "mrp_motion.h"
#include "mrp_setting.h"
#include "mrp_sysinfo.h"
#include "mrp_nes.h"
#include "mrp_application.h"
#include "mrp_main.h"
#include "vapp_mythroad_gprot.h"

#if (MTK_VERSION >= 0x06B)
#define DSM_MTK_PREFIX(x)            gui_##x
#else
#define DSM_MTK_PREFIX(x)            pixtel_UI_##x
#endif


#ifdef __cplusplus
}
#endif
#endif
#endif

