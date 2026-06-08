#ifdef __MMI_DSM_NEW__
#include "MMI_features.h" 
#include "vapp_mythroad.h"
#include "mmi_rp_vapp_mythroad_def.h"
#include "../xml/vfx_xml_loader.h"
#include "vcp_include.h"

#include "vapp_uc_gprot.h"
extern "C"
{
#include "GlobalResDef.h"   
}
#include "vapp_fmgr_cui_gprot.h"
#include "vcp_global_popup.h"
#include "vapp_camco_setting_mgr.h"
#ifdef __VAPP_CAMCO_EXPRESS__
#include "vapp_camco_express_setting_mgr.h"
#endif
#include "app_str.h"
#include "mrp_include.h"

/***************************************************************************** 
 * Define
 *****************************************************************************/

/***************************************************************************** 
 * Typedef
 *****************************************************************************/

/***************************************************************************** 
 * Global Variable
 *****************************************************************************/

/***************************************************************************** 
 * Function
 *****************************************************************************/

mmi_id vapp_mythroad_launch(void* arg, U32 argSize, mmi_proc_func proc)
{
	VappMythroadApp::proc = proc;
		
    return VfxAppLauncher::launch( 
       VAPP_MYTHROAD,
        VFX_OBJ_CLASS_INFO(VappMythroadApp),
        GRP_ID_ROOT, arg, argSize);
}

void vapp_mythroad_terminate(mmi_id app_id)
{
	VfxAppLauncher::terminate(app_id);
}

void vapp_mythroad_set_cui_proc(mmi_id app_id, mmi_id cui_id, mmi_proc_func proc)
{
	VappMythroadApp::proc = proc;
	VappMythroadApp::cui_id = cui_id;
	//mmi_frm_group_set_caller_proc(cui_id, &VappMythroadApp::cui_proc, (void*)app_id);
}

VFX_IMPLEMENT_CLASS("VappMythroadApp", VappMythroadApp, VfxApp);

mmi_proc_func VappMythroadApp::proc;
mmi_id VappMythroadApp::cui_id;

void VappMythroadApp::onRun(void * args, VfxU32 argSize)
{
    VfxApp::onRun(args, argSize);
}

mmi_ret VappMythroadApp::onProc(mmi_event_struct *evt)
{
	if (VappMythroadApp::proc != NULL && MMI_RET_OK == VappMythroadApp::proc(evt))
		return MMI_RET_OK;
		
	return VfxApp::onProc(evt);
}

mmi_ret VappMythroadApp::cui_proc(mmi_event_struct *evt)
{
	VfxApp* app;
	
	VappMythroadApp::proc(evt);

	app = VfxApp::getObject((int)evt->user_data);

	//vfxPostInvoke0(app, &VfxApp::exit);
	app->exit();

	return MMI_RET_OK;
}

extern "C" mmi_id mr_fmgr_get_cui_id(void);
void VappMythroadApp::onGroupTopActive()
{
	VfxCui *cui = VFX_OBJ_DYNAMIC_CAST(VfxApp::getObject(mr_fmgr_get_cui_id()), VfxCui);

	//kal_prompt_trace(MOD_MMI,"onGroupTopActive,%d,%d,%x",cui,cui->isRunning(),mr_fmgr_get_cui_id());

	//mr_fmgr_get_cui_id() == 0:文件管理已经退出了.
	//cui == 0,是点了UP箭头,插拨USB线,该值!= 0,return 掉文件管理器也不会退出.
	if((mr_fmgr_get_cui_id() == 0)||cui !=0)
	{
		return;
	}
	
	if (isRunning())
	{
			if (proc != NULL)
			{
				vcui_file_selector_result_event_struct event;

				event.evt_id = EVT_ID_VCUI_FILE_SELECTOR_RESULT;
				event.result = -1;
				proc((mmi_event_struct *)&event);
			}
	}
	else
	{
		/* first create, do nothing */
	}
}



#include "vapp_wallpaper_setting_method_gprot.h"

extern "C" int mr_setting_wallpaper_adp(VfxWChar* path)
{
	VappWpSetErrorCode error;

	VFX_ASSERT(path != NULL);
		
	VappWallpaperSettingInterface* WallpaperSetting = VFX_OBJ_GET_INSTANCE(VappWallpaperSettingInterface);

	error = WallpaperSetting->setWallpaperFile(VAPP_WALLPAPER_HOME_SCREEN, VFX_WSTR_STATIC(path));

	if (error == VAPP_WALLPAPER_SETTING_NO_ERROR)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

extern "C" int mr_setting_screensaver_adp(VfxWChar* path)
{
	VappWpSetErrorCode error;

	VFX_ASSERT(path != NULL);
		
	VappWallpaperSettingInterface* WallpaperSetting = VFX_OBJ_GET_INSTANCE(VappWallpaperSettingInterface);

	error = WallpaperSetting->setWallpaperFile(VAPP_WALLPAPER_LOCK_SCREEN, VFX_WSTR_STATIC(path));

	if (error == VAPP_WALLPAPER_SETTING_NO_ERROR)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static void vcp_popup_callback(VfxId button_id, void *userData)
{
	switch (button_id)
	{
	case VCP_CONFIRM_POPUP_BUTTON_USER_1:
		((void (**)(void))userData)[0]();
	case VCP_CONFIRM_POPUP_BUTTON_USER_2:
	case VCP_POPUP_BUTTON_NO_PRESSED:
		((void (**)(void))userData)[1]();
	}
}

extern "C" void  mr_popup_display_question(int32 text_resid, void (*yes)(void), void (*no)(void))
{
	static void (*callback[2])(void);

	callback[0] = yes;
	callback[1] = no;
	
	vcp_global_popup_show_confirm_two_button_id(GRP_ID_ROOT, VCP_POPUP_TYPE_QUESTION, text_resid,
		STR_GLOBAL_YES, STR_GLOBAL_NO, VCP_POPUP_BUTTON_TYPE_NORMAL, VCP_POPUP_BUTTON_TYPE_CANCEL,
		vcp_popup_callback, callback);
}

int32 mr_setting_get_system_image_path(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	static WCHAR mr_camera_path[DSM_MAX_FILE_LEN] = {0};
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144)
	WCHAR* path;
#else
	VfxS8* path;
#endif
	WCHAR* tmp;

#ifdef __MMI_CAMCORDER__
	CamcoSettingMgr* mgr;
	int len;

	VFX_OBJ_CREATE(mgr, CamcoSettingMgr, NULL);
	mgr->setCurrApp(VAPP_CAMCO_APP_CAMERA);
	mgr->getStoragePath(&path);
	mr_camera_path[0] = 0;
	kal_wstrncat((WCHAR*)mr_camera_path, (WCHAR*)path, DSM_MAX_FILE_LEN - 1);
	len = kal_wstrlen(mr_camera_path);
	if (mr_camera_path[len - 1] == '\\') mr_camera_path[len - 1] = 0;

	VFX_OBJ_CLOSE(mgr);
#else
	mr_camera_path[0] = 0;
	kal_wstrncat((WCHAR*)mr_camera_path, (const WCHAR*)"E:\\Image", DSM_MAX_FILE_LEN - 1);	
#endif	

	if(mr_camera_path[0] == MMI_SYSTEM_DRV)
	{
		mr_camera_path[0] = 'A';
	}
	else if(mr_camera_path[0] == MMI_PUBLIC_DRV)
	{
		if(MMI_PUBLIC_DRV == MMI_CARD_DRV)
		{
			mr_camera_path[0] = 'C';
		}
		else
		{
			mr_camera_path[0] = 'B';
		}
	}
	else
	{
		mr_camera_path[0] = 'C';
	}
	
	tmp = mr_camera_path;
	while(tmp = (WCHAR*)app_ucs2_strchr((kal_int8*)tmp, '\\'))
	{
		*tmp = '/';
		++tmp;
	}

	*output = (uint8*)mr_camera_path;

	return MR_SUCCESS;
}
#endif

