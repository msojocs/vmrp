
/*
 * 定义mythroad中需要使用的VENUS APP下的些功能
 */

#ifndef __VAPP_MYTHROAD_GPROT_H__
#define __VAPP_MYTHROAD_GPROT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "MMIDataType.h"
#include "mmi_res_range_def.h"


typedef enum
{
    EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND = (RESOURCE_BASE_VAPP_MYTHROAD + 1),  /* app goto back ground */

#ifdef __PME_SUPPORT__
    EVT_ID_VAPP_MYTHROAD_PME_MSG_EMAIL,
    EVT_ID_VAPP_MYTHROAD_PME_MSG_IM,
    EVT_ID_VAPP_MYTHROAD_PME_MSG_SYSTEM,
#endif

    EVT_ID_VAPP_MYTHROAD_TOTAL
} vapp_mythroad_evt_enum;

/* event structure for EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND */
typedef struct
{
    MMI_EVT_PARAM_HEADER
    MMI_BOOL is_goto_backgroud;
    S32 app_id;
    S32 img_id;
    S32 str_id;
} vapp_mythroad_goto_background_evt_struct;


typedef enum
{
	MYTHROAD_CUID_FMGR = 0,
	MYTHROAD_CUID_DTCNT,
	MYTHROAD_CUID_PHONEBOOK,

	/* add more cuid before this */
	MYTHROAD_CUID_MAX
} mythroad_cuid_enum;

typedef struct
{
	mythroad_cuid_enum cuid_enum;
	mmi_id *p_cuid_id;
	void *user_data;
} cui_info_struct;

mmi_id vapp_mythroad_launch(void* arg, U32 argSize, mmi_proc_func proc);
void vapp_mythroad_terminate(mmi_id app_id);
void vapp_mythraod_goto_background(MMI_BOOL is_goto_background, S32 app_id, S32 img_id, S32 str_id);
#ifdef __PME_SUPPORT__
void vapp_mythraod_goto_pme_msg_email(MMI_BOOL is_show,S32 img_id, S32 str_id);
void vapp_mythraod_goto_pme_msg_im(MMI_BOOL is_show,S32 img_id, S32 str_id);
void vapp_mythraod_goto_pme_msg_system(MMI_BOOL is_show,S32 img_id, S32 str_id);
#endif

#ifdef __cplusplus
}
#endif

#endif
