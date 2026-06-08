#ifdef __MMI_DSM_NEW__


#include "MMI_Include.h"
#include "PhonebookResDef.h"


#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
#include "sky_qq_open.h"
#endif
#include "SmsSrvGprot.h"
#include "ModeSwitchSrvGprot.h"
#include "mrp_include.h"
/* 解决在VM在运行的时候不锁屏*/
#include "vapp_screen_lock_gprot.h"
#include "GpioSrvGprot.h"

#define MR_APP_COCURRENT_COUNT 4


typedef struct {
	uint32 nRunStyle;
	uint32 nExitReason;
	T_DSM_STATE nAppState;
	U16 nPreKeyEvt;
	U16 nPreMouseEvt;
	int32 nPreKeyEvtParam;
	int32 nPreMouseEvtParam;
}mr_app_ctrl_t;


static mr_app_ctrl_t s_app_ctrl = {
	0, 0, 
	DSM_STOP, 
	MR_KEY_RELEASE,
	MR_MOUSE_UP,
	0, 0
};

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_SCREEN_ROTATE__)
U8 current_screen_rotation = MMI_FRM_SCREEN_ROTATE_0;
#endif

/* 52_xsgrz_patch begin */
#ifdef __MMI_TOUCH_SCREEN__
static kal_uint32 current_sampling_period = 0;
static kal_uint32 current_move_offset = 0;
#endif
/* 52_xsgrz_patch end */

char * dsmEntry = NULL;
////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
//这个变量用来区分调用退出函数的是因为按挂机键退出还是按正常流程退出
U8 IS_SKYQQ_END_KEY = 0;
#endif

const uint16 g_root_menu_id = IDLE_SCREEN_MENU_ID;

static void mr_app_reset_events(void);
static BOOL mr_app_check_unpause_scrid(void);
void dsmShowExitInfo(void);
/* 解决在VM在运行的时候不锁屏*/
mmi_ret mr_app_events_handler(mmi_event_struct *evt);



////////////////////////////////////////////////////////////////////////////////////////////////

static mr_app_event_handler_f s_app_event_handler_pool[MR_APP_COCURRENT_COUNT] = {0};


int32 mr_app_register_notify_event_hdlr(mr_app_event_handler_f handler)
{
#ifndef __MR_CFG_FEATURE_SLIM__

	int i;

	if( !handler){
		return MR_FAILED;
	}

	for(i = 0; i < MR_APP_COCURRENT_COUNT; i++){
		if(!s_app_event_handler_pool[i])
			break;
	}

	if(i >= MR_APP_COCURRENT_COUNT){
		return MR_FAILED;
	}

	s_app_event_handler_pool[i] = handler;
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}


int32 mr_app_clear_notify_event_hdlr(mr_app_event_handler_f handler)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	int i;

	if(!handler){
		return MR_FAILED;
	}

	for(i = 0; i < MR_APP_COCURRENT_COUNT; i++){
		if(s_app_event_handler_pool[i] == handler)
		{
			s_app_event_handler_pool[i] = 0;
			break;
		}
	}

	return MR_FAILED;
#else
	return MR_FAILED;
#endif
}


int32 mr_app_send_notify_event(int32 msg, uint32 param1, uint32 param2)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	int i;
	
	if(msg < 0 || msg >= MR_APPMSG_COUNT){
		return MR_FAILED;
	}

	for(i = 0; i < MR_APP_COCURRENT_COUNT; i++)
	{
		if(s_app_event_handler_pool[i])
			s_app_event_handler_pool[i] (msg, param1, param2);
	}

	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}

extern BOOL isDsmVIAWIFI;
////////////////////////////////////////////////////////////////////////////////////////////////
int32 mr_app_initialize(void)
{
	mr_localui_initialize();
	
	s_app_ctrl.nPreKeyEvt = MR_KEY_RELEASE;
	s_app_ctrl.nPreMouseEvt = MR_MOUSE_UP;
	s_app_ctrl.nExitReason = MR_EXIT_NORMAL;
	
#ifdef __MMI_DSM_WIFI_SUPPORT__
	isDsmVIAWIFI = FALSE;
#endif

	return MR_SUCCESS;
}


int32 mr_app_terminate(void)
{
      int i = 0;
	  
#ifdef __MMI_DSM_WEATHER_APP__	  
      extern int weather_start;
#endif

#ifdef __MMI_DSM_WIFI_SUPPORT__
	isDsmVIAWIFI = FALSE;
#endif

	gDsmUseNVdataAccount = FALSE;
	mr_trace("---------mr_app_terminate: %d", mr_app_get_state());

      if(mr_app_get_state() != DSM_STOP 
	  	&& mr_app_get_state() != DSM_BACK_RUN)
      {
	     	mr_stop();
		mr_stopSound(MR_SOUND_MIDI);
		mr_closeNetwork();
		mr_timerStop();
		
		mr_localui_free_all_screen();		
	  	dsmFreeScrIdAll();
		mr_media_apm_release_buffer();
		mr_fs_switch_to_root_dir();
		mr_media_close_device();
		mr_localui_terminate();
		
		//huangsunbo 20100318 解决按END键强制退出应用提示完成问题
		#if (MTK_VERSION < 0x0852)
			mr_plat(1216,0);
		#endif
	
		mr_net_terminate();
		mr_image_terminate();
		mr_media_audio_reset();
		mr_app_set_state(DSM_STOP);

#ifdef __MMI_DSM_WEATHER_APP__		
		weather_start = 0;
#endif

		mr_signal_terminate(0);
		mdi_audio_resume_background_play();
		mr_backstage_terminate();
      	} 

#ifdef __MR_CFG_FEATURE_MOPOZS_DOWNLOAD_GBTABLE__
	mr_chset_unload_map();
#endif

	return MR_SUCCESS;
}


T_DSM_STATE mr_app_get_state(void)
{
	return s_app_ctrl.nAppState;
}


void mr_app_set_state(T_DSM_STATE state)
{
	s_app_ctrl.nAppState = state;
}


void mr_app_set_run_style(uint32 attr)
{
	s_app_ctrl.nRunStyle = attr;
}


void mr_app_set_exit_reason(uint32 reason)
{
	s_app_ctrl.nExitReason = reason;
}


uint32 mr_app_get_exit_reason(void)
{
	return s_app_ctrl.nExitReason;
}


int32  mr_app_pause(void)
{
	mr_trace("mr_app_pause: %d", mr_app_get_state());
	
	if(mr_app_get_state() != DSM_RUN){
		return MR_FAILED;
	}

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_pause();
#endif
	mr_media_audio_reset();
	mr_app_reset_events();
	mr_app_set_state(DSM_PAUSE);
	mr_pauseApp();
	//mdi_audio_stop_string();
	#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
	SKYQQShowIdle(QQ2008_HIDE_ICON);
	#endif
	mr_app_send_notify_event(MR_APPMSG_STATE_CHANGED, DSM_RUN, 0);
	return MR_SUCCESS;
}


int32  mr_app_resume(void)
{
	mr_trace("mr_app_resume: %d", mr_app_get_state());

	if(mr_app_get_state() != DSM_PAUSE){
		return MR_FAILED;
	}

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_resume();
#endif
	//mdi_audio_suspend_background_play(); /*2009-04-14 lxm*/
	mr_app_set_state(DSM_RUN);
	mr_media_audio_restore_play();
	mr_resumeApp();

	mr_app_send_notify_event(MR_APPMSG_STATE_CHANGED, DSM_PAUSE, 0);
	return MR_SUCCESS;
}


int32 mr_app_run(char *entry, uint32 param)
{
	int32 ret = MR_FAILED;
	char *entry_param =NULL;

	mr_trace("mr_app_run: %s", entry);
	mr_imei_initialize();
	mr_fs_create_vm_dir();

#ifdef __MR_CFG_ENTRY_NES__
	mr_nes_create_root_dir();
#endif	

	if(s_app_ctrl.nRunStyle & MR_RUN_BACK){
		mr_app_set_state(DSM_BACK_RUN);
	}else{
		mr_app_set_state(DSM_RUN);
	}
	
	if(*entry == '%')
	{
		char* entry_param = strchr(entry, ',');		 
		if(entry_param)
		{
			char path[64] = {0};
			ASSERT((entry_param - entry) < 64);
			memcpy(path, entry, entry_param - entry);
			entry_param++;
			mr_trace("%s -- %s", path, entry_param);
			#ifndef WIN32
			ret = mr_start_dsm_ex(path,entry_param);
			#endif
		}
		else
		{
			ret = mr_start_dsm(entry);
		}
	}	
	else
	{
		ret = mr_start_dsm(entry);
	}
	
	if (ret == MR_FAILED)
	{
	       mr_app_set_state(DSM_STOP);
		mr_app_force_exit_dsm_screen();
	}

	return MR_SUCCESS;
}


void mr_app_exit(void)
{
	if(mr_app_get_state() != DSM_STOP){
		mr_app_set_exit_reason(MR_EXIT_FORCE);
		mr_app_set_state(DSM_RUN); //将状态设置成DSM_RUN,主要是为了能强制退出
		mr_app_force_exit_dsm_screen();
	}
}

#ifdef WIN32
mr_appInfoSt_st mrc_appInfo_st;	// 模拟器编译问题
#endif
static void mr_app_enter_idlescreen(void)
{	
	//按挂机键时能使SDK QQ挂后台.
	if((mr_app_get_state() == DSM_RUN ||mr_app_get_state() == DSM_PAUSE) 
		&& (strstr(dsmEntry, "qq") ||strstr(dsmEntry, "QQ") 
		//以下代码只有精简虚拟机才能使用.
		||(mrc_appInfo_st.id == MR_APPID_QQ2007)
		||(mrc_appInfo_st.id == MR_APPID_QQ2008) ||(mrc_appInfo_st.id == 203109)	// 不知道为啥套壳QQ的appid为203109
		)) 
		//mrc_appInfo_st.id == MR_APPID_QQ
	{
		if(mr_app_get_state() == DSM_PAUSE)
		{
			mr_app_resume();
		}
		mr_app_send_event_wrapper(MR_KEY_PRESS,MR_KEY_POWER,0);
	}

	mr_app_terminate();
	mr_backstage_on_enter_idlescreen();
	mr_app_send_notify_event(MR_APPMSG_ENTRY_IDLESCREEN, 0, 0);

#ifdef __PME_SUPPORT__
#ifdef __DSM_OVERSEA__
	pme_idle_notify(1);
#else
	pme_idle_notify(0);
#endif
#endif

#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
	if(GetSkyQQRunState()!=0)
	{
		QQ2008MINIShowIdle(0);
		QQmin=1;
	}
	else
	{
		SKYQQShowIdle(QQ2008_HIDE_ICON);
	}
#endif/*__MMI_SKYQQ__*/

#ifdef __SKY_SECURITY_GUARD__
	sg_show_status_icon();
#endif

}


static void mr_app_exit_idlescreen(void)
{
	mr_backstage_on_exit_idlescreen();
	mr_app_send_notify_event(MR_APPMSG_EXIT_IDLESCREEN, 0, 0);
}


int32 mr_app_system_event_hdlr(int32 msg, uint32 param1, uint32 param2)
{
	mr_trace("mr_app_system_event_hdlr: %d, %d, %x", msg, param1, param2);
	
	switch(msg)
	{
	case MR_SYSMSG_SYSTEM_INIT:
		mr_system_intialize();
		break;
		
	case MR_SYSMSG_ENTRY_IDLESCREEN:
		mr_app_enter_idlescreen();
		break;

	case MR_SYSMSG_EXIT_IDLESCREEN:
		mr_app_exit_idlescreen();
		break;
		
	case MR_SYSMSG_LAUNCH_CAMERA:
	case MR_SYSMSG_LAUNCH_VDOPLY:
	case MR_SYSMSG_LAUNCH_3RD:
#ifdef DSM_USE_MED_BUFFER
		if(GetDsmState() != DSM_STOP)
		{	/*可以启动一个确认框，确认后可以调用cb来启动*/
			dsmShowExitInfo();
			return MR_SUCCESS;
		}
#endif
		break;

	case MR_SYSMSG_SUSPEND_TIMER:
		if(GetDsmState() != DSM_STOP)  
			return MR_SUCCESS;

#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
		if(GetSkyQQRunState() != QQ2008_STOP)  
			return MR_SUCCESS;;
#endif
		break;

	case MR_SYSMSG_SMS_INDICATION:
		if(!dsmIsSmsDisplay())
			return MR_SUCCESS;
		break;

	case MR_SYSMSG_ENTER_USB:
	case MR_SYSMSG_FORCE_EXIT:
#if defined(__MMI_SKYQQ__) && !defined(WIN32) 			
		if(GetSkyQQRunState())
		{
			extern void sky_qq_force_exit();
			sky_qq_force_exit();
		}
		else
		{
			mr_app_exit();
		}
#else
		mr_app_exit();
#endif
		break;
		
#ifndef __MR_CFG_FEATURE_SLIM__
	case MR_SYSMSG_MMI_ROTATION:
      		mr_backstage_on_rotation();
		break;
#endif		
	}

	return MR_IGNORE;
}


void mr_system_environment_setup(void)
{
	static BOOL gDsmInited = FALSE;

	mr_trace("mr_system_environment_setup ");  
	if(gDsmInited == FALSE)
	{
		gDsmInited = TRUE;

#ifdef __MR_CFG_ENTRY_NES__		
		mr_nes_initialize();
#endif
		
		mr_fs_create_root_dir();
#ifdef __MMI_DSM_HIDE_FOLDER__
		mr_fs_set_launch_dir(__MR_CFG_VAR_CUSTOM_DRV__, __MR_CFG_VAR_CUSTOM_PATH__);
		mr_fs_create_vm_dir();		
#endif
		mr_imei_initialize();
		
#ifdef __MMI_DSM_WEATHER_APP__		
		dsmInitWeatherData();
#endif

		mr_net_initialize();
		mr_sms_initialize();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////

void mr_app_force_exit_dsm_screen(void)
{
	mr_trace("mr_app_force_exit_dsm_screen: %d", mr_app_get_state());
		
	mr_app_terminate();

#if 0
	if(DSM_SCREEN_BASE == GetActiveScreenId())
	{
		GoBackHistory();
	}
	else if(IsScreenPresent(DSM_SCREEN_BASE))
	{
		DeleteUptoScrID(DSM_SCREEN_BASE);
		DeleteNHistory(1);
		GoBackHistory();
	}
#endif

	mmi_frm_group_close(mr_app_get_group_id());
}


static mmi_ret mr_app_on_dsm_screen_delete(mmi_event_struct* param)
{
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	MMI_BOOL is_enable_zoom;
#endif
	mr_trace("mr_app_on_dsm_screen_delete");
	/* 解决在VM在运行的时候不锁屏*/
	mmi_frm_cb_dereg_event(EVT_ID_VAPP_SCREEN_LOCK_PRE_LOCK_EVT_ROUTING, mr_app_events_handler, NULL);
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	is_enable_zoom = (MMI_BOOL)mr_layer_zoom_is_enabled();
	mr_layer_zoom_disable();
#endif
/* __MMI_DSM_NEW_JSKY__ begin support screen rotate */
#if defined(__MMI_SCREEN_ROTATE__)
	current_screen_rotation = MMI_FRM_SCREEN_ROTATE_0;
#endif
/* __MMI_DSM_NEW_JSKY__ end support screen rotate */
/* 52_xsgrz_patch begin */
#ifdef __MMI_TOUCH_SCREEN__
	current_sampling_period = 0;
	current_move_offset = 0;
#endif
/* 52_xsgrz_patch end */
	mr_plat(1011, 1);  /*在这个base 窗口删除的时候就要能显示收到短信*/
	mr_backlight_turnoff();
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	if (param->evt_id == EVT_ID_SCRN_GOBACK_IN_END_KEY || param->evt_id == EVT_ID_SCRN_DELETE_REQ_IN_END_KEY)
	{
		if (is_enable_zoom)
		{
			mr_app_enter_idlescreen();
		}
	}
#endif
	return MMI_RET_OK;
}


#ifdef __MR_CFG_FEATURE_QWERTY_KEYPAD__	
static MMI_BOOL mr_qwerty_key_handler(S16 mmi_key_code, S16 mmi_key_type, U16 ucs2_value, U32 special_key_flag)
{

	U16 keytype = 0;
	U16 keycode = 0;
      // mr_trace(("mr_app_dsm_screen_key_hdlr"));
	   
	kal_prompt_trace(MOD_MMI,"mr_qwerty_key_handler");
	kal_prompt_trace(MOD_MMI,"mmi_key_code=%d---mmi_key_type=%d",mmi_key_code,mmi_key_type);
	kal_prompt_trace(MOD_MMI,"ucs2_value=%d---special_key_flag=%d",ucs2_value,special_key_flag);

	keycode = mmi_key_code;
	keytype = mmi_key_type;
	
#ifdef __MMI_SKYQQ__
	IS_SKYQQ_END_KEY = 0;
#endif

     //mr_trace(("keytype=%d",keytype));
	if(keytype == KEY_EVENT_DOWN){
		keytype = MR_KEY_PRESS;
	}else if(keytype == KEY_EVENT_UP){
		keytype = MR_KEY_RELEASE;
	}else{
		return ;
	}
	
#ifdef __MMI_SKYQQ__
	if(keycode == KEY_END)
	{
		IS_SKYQQ_END_KEY = 1;
		if(GetSkyQQRunState()==QQ2008_RUN)
			mr_app_send_event_wrapper(keytype,MR_KEY_POWER,0);
		else
			DisplayIdleScreen();
		return ;
	}
#endif

       //mr_trace(("keycode"));
	keycode = mr_app_translate_keycode(keycode);
	//mr_trace(("keycode=%d",keycode));
	if(keycode != MR_KEY_NONE){
		mr_app_send_event_wrapper(keytype, keycode, 0);
	}
}
#endif

void mr_app_setup_dsm_screen_key_hdlr(void)
{
	U16 dsmKeyGrpoup[] = {DSM_DEAL_KEYS};
#ifdef __MR_CFG_FEATURE_QWERTY_KEYPAD__	
	U16 dsmQwertyKeys[] = {DSM_DEAL_QWERTY_KEYS};
#endif
	
	SetGroupKeyHandler(mr_app_dsm_screen_key_hdlr, dsmKeyGrpoup, sizeof(dsmKeyGrpoup)/sizeof(dsmKeyGrpoup[0]), KEY_EVENT_DOWN);
	SetGroupKeyHandler(mr_app_dsm_screen_key_hdlr, dsmKeyGrpoup, sizeof(dsmKeyGrpoup)/sizeof(dsmKeyGrpoup[0]), KEY_EVENT_UP);

#ifdef __MR_CFG_FEATURE_QWERTY_KEYPAD__	
	mmi_frm_set_translate_key_handler(MMI_FRM_TRANS_KEY_APP, mr_qwerty_key_handler);
#endif
	mr_backstage_setup_endkey_hdlr();
}


void mr_app_setup_dsm_screen_pen_hdlr(void)
{
#ifdef __MMI_TOUCH_SCREEN__
	mmi_pen_register_down_handler(mr_app_dsm_screen_mouse_down_hdlr);
	mmi_pen_register_up_handler(mr_app_dsm_screen_mouse_up_hdlr);
	mmi_pen_register_move_handler(mr_app_dsm_screen_mouse_move_hdlr);
#endif
}


void mr_app_setup_dsm_screen_event_hdlr(void)
{
	mr_app_setup_dsm_screen_key_hdlr();
	mr_app_setup_dsm_screen_pen_hdlr();
}

static MMI_ID mythrod_group_id = GRP_ID_INVALID;

MMI_ID mr_app_get_group_id(void)
{
	return mythrod_group_id;
}

mmi_ret dsm_fmgr_proc(mmi_event_struct* evt);
static mmi_ret mr_app_group_proc(mmi_event_struct* param)
{
	mr_trace("mr_app_group_proc:%d", param->evt_id);

	//dsm_fmgr_proc(param);

	if (param->evt_id == EVT_ID_GROUP_ACTIVE)
	{
		//mr_app_resume();
	}
	else if (param->evt_id == EVT_ID_GROUP_INACTIVE)
	{
		//mr_app_pause();
	}
	else if (param->evt_id == EVT_ID_GROUP_GOBACK_IN_END_KEY || param->evt_id == EVT_ID_GROUP_DELETE_REQ_IN_END_KEY)
	{
		mr_app_enter_idlescreen();
	}
	
	return MMI_RET_OK;
}

MMI_ID mr_app_get_group(void)
{
	if (!mmi_frm_group_is_present(mythrod_group_id))
	{
		mythrod_group_id = mmi_frm_group_create(GRP_ID_ROOT, GRP_ID_AUTO_GEN, mr_app_group_proc, NULL);
		mmi_frm_group_enter(mythrod_group_id, MMI_FRM_NODE_SMART_CLOSE_FLAG);
	}

	return mythrod_group_id;
}

void mr_app_EntryNewScreen(U16 newscrnID, FuncPtr newExitHandler, FuncPtr newEntryHandler, mmi_frm_scrn_type_enum flag)
{
	if (!mmi_frm_group_is_present(mythrod_group_id))
	{
		mythrod_group_id = mmi_frm_group_create(GRP_ID_ROOT, GRP_ID_AUTO_GEN, mr_app_group_proc, NULL);
		mmi_frm_group_enter(mythrod_group_id, MMI_FRM_NODE_SMART_CLOSE_FLAG);
	}

	mmi_frm_scrn_enter(mythrod_group_id, newscrnID, newExitHandler, newEntryHandler, flag);
}

void mr_app_show_dsm_base_screen(void)
{
	internalCovered = MR_TRUE;

	mr_app_EntryNewScreen(DSM_SCREEN_BASE, mr_app_exit_dsm_screen, mr_app_entry_dsm_screen, MMI_FRM_FULL_SCRN);

	mdi_audio_suspend_background_play();
	//SetDelScrnIDCallbackHandler(DSM_SCREEN_BASE, mr_app_on_dsm_screen_delete);
	mmi_frm_scrn_set_leave_proc(mr_app_get_group_id(), DSM_SCREEN_BASE, mr_app_on_dsm_screen_delete);
	mr_app_setup_dsm_screen_event_hdlr();
	entry_full_screen();
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_SCREEN_ROTATE__)
	if (current_screen_rotation != MMI_FRM_SCREEN_ROTATE_0)
	{
		mmi_frm_screen_rotate((mmi_frm_screen_rotate_enum)current_screen_rotation);
	}
#endif
	/* 解决在VM在运行的时候不锁屏*/
	mmi_frm_cb_reg_event(EVT_ID_VAPP_SCREEN_LOCK_PRE_LOCK_EVT_ROUTING, mr_app_events_handler, NULL);
/* 52_xsgrz_patch begin */
#ifdef __MMI_TOUCH_SCREEN__
	if (current_sampling_period != 0)
	{
		mmi_pen_config_sampling_period(current_sampling_period, MMI_PEN_SAMPLING_PERIOD_2);
	}
	if (current_move_offset != 0)
	{
		mmi_pen_config_move_offset(
	        current_move_offset,
	        MMI_PEN_SKIP_STROKE_MOVE_OFFSET,
	        MMI_PEN_SKIP_LONGTAP_OFFSET,
	        MMI_PEN_SKIP_STROKE_LONGTAP_OFFSET);
	}
#endif
/* 52_xsgrz_patch end */
	internalCovered = MR_FALSE;
}


void mr_app_entry_dsm_screen(void)
{
	extern S32 mmi_is_redrawing_bk_screens();
	mr_trace("mr_app_entry_dsm_screen: %x", s_app_ctrl.nRunStyle);
	
	if(!(s_app_ctrl.nRunStyle & MR_RUN_BACK)){
		mr_app_show_dsm_base_screen();
	}
	
	if (s_app_ctrl.nAppState == DSM_STOP)
	{
		mr_sim_initialize();
		mr_app_initialize();
		mr_media_init_device();
		mr_app_run(dsmEntry, 0);
	}else if(s_app_ctrl.nAppState == DSM_PAUSE){
		mr_app_resume();
		/*
		 * 下边的代码是处理虚拟机界面来小窗口再来全屏窗口，在全屏窗口退出时
		 * 会调用虚拟机界面的入口函数，在退出小窗口时会再调用一次虚拟机界面
		 * 的入口函数,所以需要在第一次RESUME虚拟机把背景刷新后要PAUSE掉
		 */
		if (mmi_is_redrawing_bk_screens())
		{
			mr_app_pause();
		}
	}else{
	/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
	/* normal when local ui(text, win, edit, menu) exit will enter here */
	/* resize the base layer data to screen */
	#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
		mr_layer_zoom_resize(0, 0, mr_layer_zoom_get_width(), mr_layer_zoom_get_height());
	#endif
	}
}


static void mr_app_reset_events(void)
{
	if(s_app_ctrl.nPreKeyEvt == MR_KEY_PRESS){
		mr_app_send_event_wrapper(MR_KEY_RELEASE, s_app_ctrl.nPreKeyEvtParam, s_app_ctrl.nPreMouseEvtParam);
	}
	
	if(s_app_ctrl.nPreMouseEvt == MR_MOUSE_DOWN){
		mr_app_send_event_wrapper(MR_MOUSE_UP, s_app_ctrl.nPreKeyEvtParam, s_app_ctrl.nPreMouseEvtParam);
	}
}


 int32 mr_app_send_event_wrapper(int16 ktype ,int32 param1,int32 param2)
{	   
	s_app_ctrl.nPreKeyEvtParam = param1;
	s_app_ctrl.nPreMouseEvtParam = param2;

	mr_trace("mr_app_send_event_wrapper: %d, %d, %d", ktype, param1, param2);
	
	if(mr_app_get_state() == DSM_STOP){
		return MR_IGNORE;
	}
	
       if ((ktype ==  MR_MENU_SELECT)
	   	|| (ktype == MR_MENU_RETURN)
	   	|| (ktype == MR_DIALOG_EVENT)
	   	|| (ktype == MR_LOCALUI_EVENT))
	{
              s_app_ctrl.nPreKeyEvt = MR_KEY_RELEASE;
	       s_app_ctrl.nPreMouseEvt = MR_MOUSE_UP;
              return mr_event(ktype,param1,param2);
       }
	else
	{
	       if((ktype == MR_KEY_RELEASE) ||(ktype == MR_KEY_PRESS))
	       {
		       if(s_app_ctrl.nPreKeyEvt != ktype)
		       {
		             s_app_ctrl.nPreKeyEvt = ktype;
		             return mr_event(ktype, param1, param2);
		       }
	       }
		else if((ktype == MR_MOUSE_DOWN)||(ktype == MR_MOUSE_UP))
		{
		       if(s_app_ctrl.nPreMouseEvt!= ktype)
		       {
		             s_app_ctrl.nPreMouseEvt = ktype;
		             return mr_event(ktype, param1, param2);
		       }
		}
		else if(ktype == MR_MOUSE_MOVE)
		{
			return mr_event(ktype,param1,param2);
		}
		else
	       {
	             return MR_IGNORE;
		}
	}
	return MR_IGNORE;
}


/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
int32 mr_app_rotate_keycode(U16 keycode)
{
#if defined(__MMI_SCREEN_ROTATE__)
	U16 r_90_table[] = {MR_KEY_RIGHT, MR_KEY_LEFT, MR_KEY_UP, MR_KEY_DOWN};
	U16 r_180_table[] = {MR_KEY_DOWN, MR_KEY_UP, MR_KEY_RIGHT, MR_KEY_LEFT};
	U16 r_270_table[] = {MR_KEY_LEFT, MR_KEY_RIGHT, MR_KEY_DOWN, MR_KEY_UP};
	
	switch (current_screen_rotation)
	{
		case MMI_FRM_SCREEN_ROTATE_90:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_90:
			return r_90_table[keycode - MR_KEY_UP];

		case MMI_FRM_SCREEN_ROTATE_180:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_180:
			return r_180_table[keycode - MR_KEY_UP];

		case MMI_FRM_SCREEN_ROTATE_270:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_270:
			return r_270_table[keycode - MR_KEY_UP];

		default:
			return keycode;
	}
#else
	return keycode;
#endif
}
#endif


int32 mr_app_translate_keycode(U16 keycode)
{
	switch(keycode)
	{
	case KEY_HOME:
	case KEY_END: 			return MR_KEY_POWER;
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9: 			return (keycode - KEY_0 + MR_KEY_0);
	case KEY_STAR:			return MR_KEY_STAR;
 	case KEY_POUND:			return MR_KEY_POUND;

#ifdef __SENDKEY2_SUPPORT__
	case KEY_SEND2:			return MR_KEY_SOFTLEFT;
	case KEY_LSK: 			return MR_KEY_SOFTLEFT;
#else
	case KEY_LSK: 			return MR_KEY_SOFTLEFT;
#endif
    case KEY_BACK:
	case KEY_RSK:			return MR_KEY_SOFTRIGHT;
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	case KEY_UP_ARROW: 		return mr_app_rotate_keycode(MR_KEY_UP);
	case KEY_DOWN_ARROW:	return mr_app_rotate_keycode(MR_KEY_DOWN);
	case KEY_LEFT_ARROW: 	return mr_app_rotate_keycode(MR_KEY_LEFT);
 	case KEY_RIGHT_ARROW:	return mr_app_rotate_keycode(MR_KEY_RIGHT);
#else
	case KEY_UP_ARROW: 		return MR_KEY_UP;
	case KEY_DOWN_ARROW:	return MR_KEY_DOWN;
	case KEY_LEFT_ARROW: 	return MR_KEY_LEFT;
 	case  KEY_RIGHT_ARROW:	return MR_KEY_RIGHT;
#endif

#ifdef __SENDKEY2_SUPPORT__
	case KEY_SEND1:
#endif
	case KEY_SEND:			return MR_KEY_SEND;
	case KEY_ENTER:			return MR_KEY_SELECT;
	case KEY_VOL_UP: 		return MR_KEY_VOLUME_UP;
	case KEY_VOL_DOWN:		return MR_KEY_VOLUME_DOWN;
	case KEY_CAMERA:		return MR_KEY_CAPTURE;

#ifdef __MR_CFG_FEATURE_QWERTY_KEYPAD__
	case KEY_A:
	case KEY_B:
	case KEY_C:
	case KEY_D:
	case KEY_E:
	case KEY_F:
	case KEY_G:
	case KEY_H:
	case KEY_I:
	case KEY_J:
	case KEY_K:
	case KEY_L:
	case KEY_M:
	case KEY_N:
	case KEY_O:
	case KEY_P:
	case KEY_Q:
	case KEY_R:
	case KEY_S:
	case KEY_T:
	case KEY_U:
	case KEY_V:
	case KEY_W:
	case KEY_X:
	case KEY_Y:
	case KEY_Z:				return (keycode - KEY_A + MR_KEY_QWERTY_A);
	case KEY_CLEAR:			return MR_KEY_QWERTY_DEL;
	case KEY_OK:			return MR_KEY_SELECT;
	case KEY_SPACE:			return MR_KEY_QWERTY_SPACE;
	case KEY_SYMBOL:		return MR_KEY_QWERTY_SYMBOL;
#if 0
	MR_KEY_QWERTY_SYMBOL_1, /*;  ,*/
	MR_KEY_QWERTY_SYMBOL_2, /*:  .*/
	MR_KEY_QWERTY_SYMBOL_3, /*/ @*/
	MR_KEY_QWERTY_SYMBOL_4, /*& '*/
	MR_KEY_QWERTY_SYMBOL_5, /*! ?*/
#endif
	case KEY_SHIFT:			return MR_KEY_QWERTY_SHIFT;
	case KEY_CTRL:			return MR_KEY_QWERTY_CTRL;
	case KEY_ALT:			return MR_KEY_QWERTY_ALT;
	case KEY_QUESTION:		return MR_KEY_QWERTY_QUESTION;
	//case KEY_CURRENCY:		return MR_KEY_QWERTY_CURRENCY;
	case KEY_FN:				return MR_KEY_QWERTY_FN;		
#endif

	default:
		return MR_KEY_NONE;
	}
}


void mr_app_dsm_screen_key_hdlr(void)
{
	U16 keytype = 0;
	U16 keycode = 0;

	GetkeyInfo(&keycode,&keytype);
	
#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
	IS_SKYQQ_END_KEY = 0;
#endif

	if(keytype == KEY_EVENT_DOWN){
		keytype = MR_KEY_PRESS;
	}else if(keytype == KEY_EVENT_UP){
		keytype = MR_KEY_RELEASE;
	}else{
		return ;
	}
	
#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
	if(keycode == KEY_HOME)
	{
		IS_SKYQQ_END_KEY = 1;
		if(GetSkyQQRunState()==QQ2008_RUN)
			mr_app_send_event_wrapper(keytype,MR_KEY_POWER,0);
		else
			DisplayIdleScreen();
		return;
	}
#endif

	keycode = mr_app_translate_keycode(keycode);
	if(keycode != MR_KEY_NONE){
		mr_app_send_event_wrapper(keytype, keycode, 0);
	}
}


/****************************************************************************
函数名:BOOL mr_app_check_unpause_scrid(void)
描  述:检查新的串口创建的时候，VM是否要暂停
参  数:无
返  回:TRUE:需要PAUSE
****************************************************************************/
static BOOL mr_app_check_unpause_scrid(void)
{
	int32 scrId = mmi_frm_scrn_get_active_id();
	
	if(		
#if (MTK_VERSION < 0x09B0952)
		( (scrId >= FILE_MANAGER_BASE) && (scrId <= FILE_MANAGER_BASE_MAX) && (IsScreenPresent(MENU_DSM_FMGR_APP)))
#else
		((scrId >= GRP_ID_AUTO_GEN_BASE) && (scrId <= GRP_ID_AUTO_GEN_MAX))
		//|| (scrId == APP_DSM)
#endif
		|| (scrId == MENU_DSM_FMGR_APP))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


void mr_app_exit_dsm_screen(void)
{
     extern pBOOL IsBackHistory;

	mr_trace("mr_app_exit_dsm_screen");

/* 52_xsgrz_patch begin */	
#ifdef __MMI_TOUCH_SCREEN__
	if (current_sampling_period != 0)
	{
		mmi_pen_config_sampling_period(MMI_PEN_SAMPLING_PERIOD_1, MMI_PEN_SAMPLING_PERIOD_2);
	}
	if (current_move_offset != 0)
	{
		mmi_pen_config_move_offset(
	        MMI_PEN_SKIP_MOVE_OFFSET,
	        MMI_PEN_SKIP_STROKE_MOVE_OFFSET,
	        MMI_PEN_SKIP_LONGTAP_OFFSET,
	        MMI_PEN_SKIP_STROKE_LONGTAP_OFFSET);
	}
#endif
/* 52_xsgrz_patch end */
     if (internalCovered == MR_FALSE)
     {
		if(( mr_app_get_state() == DSM_RUN )
			&& (mmi_frm_is_in_backward_scenario() != MMI_TRUE)
			&& ( mr_app_check_unpause_scrid()) )
		{
			mr_app_pause();
		}
     }
     else
     {
		internalCovered = MR_FALSE;
     }
}


#ifdef __MMI_TOUCH_SCREEN__
void mr_app_dsm_screen_mouse_move_hdlr(mmi_pen_point_struct pos)
{
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_convert_touch_position(&pos.x, &pos.y);
#endif
	mr_app_send_event_wrapper(MR_MOUSE_MOVE, pos.x, pos.y);
}


void mr_app_dsm_screen_mouse_up_hdlr(mmi_pen_point_struct pos)
{
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_convert_touch_position(&pos.x, &pos.y);
#endif
	mr_app_send_event_wrapper(MR_MOUSE_UP, pos.x, pos.y);
}


void mr_app_dsm_screen_mouse_down_hdlr(mmi_pen_point_struct pos)
{
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_convert_touch_position(&pos.x, &pos.y);
#endif
	mr_app_send_event_wrapper(MR_MOUSE_DOWN, pos.x, pos.y);
}
#endif


int32 mr_app_goto_dsm_screen(int32 param)
{
	if(mmi_frm_scrn_get_active_id() == DSM_SCREEN_BASE)
	{
		return MR_SUCCESS;
	}

	//GoBackToHistory(DSM_SCREEN_BASE);
	mmi_frm_scrn_multiple_close(mr_app_get_group_id(), mmi_frm_scrn_get_active_id(), MMI_TRUE, 0, DSM_SCREEN_BASE, MMI_FALSE);
	
	mr_localui_free_all_screen();		
	dsmFreeScrIdAll();

	return MR_SUCCESS;
}


/* 52_xsgrz_patch begin */
int32 mr_app_config_sampling_period(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef __MMI_TOUCH_SCREEN__
	if (input == NULL)
	{
		return MR_FAILED;
	}

	current_sampling_period = (kal_uint32)*input;
	
	if (current_sampling_period == 0)
	{
		mmi_pen_config_sampling_period(MMI_PEN_SAMPLING_PERIOD_1, MMI_PEN_SAMPLING_PERIOD_2);
	}
	else
	{
		mmi_pen_config_sampling_period(current_sampling_period, MMI_PEN_SAMPLING_PERIOD_2);
	}

	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_app_config_move_offset(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef __MMI_TOUCH_SCREEN__
	if (input == NULL)
	{
		return MR_FAILED;
	}

	current_move_offset = (kal_uint32)*input;
	
	if (current_move_offset == 0)
	{
		mmi_pen_config_move_offset(
	        MMI_PEN_SKIP_MOVE_OFFSET,
	        MMI_PEN_SKIP_STROKE_MOVE_OFFSET,
	        MMI_PEN_SKIP_LONGTAP_OFFSET,
	        MMI_PEN_SKIP_STROKE_LONGTAP_OFFSET);
	}
	else
	{
		mmi_pen_config_move_offset(
	        current_move_offset,
	        MMI_PEN_SKIP_STROKE_MOVE_OFFSET,
	        MMI_PEN_SKIP_LONGTAP_OFFSET,
	        MMI_PEN_SKIP_STROKE_LONGTAP_OFFSET);
	}

	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}
/* 52_xsgrz_patch end */


////////////////////////////////////////////////////////////////////////////////////////////////

void EntryDsmScreen()
{
	mr_app_entry_dsm_screen();
}


int GetDsmState(void)
{
	return (int)mr_app_get_state();
}


void dsmForceExitEx(void)
{
#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
	if(GetSkyQQRunState())
	{
			extern void sky_qq_force_exit();
			sky_qq_force_exit();
	}
	else
	{
		mr_app_exit();
	}
#else
	mr_app_exit();
#endif
	GoBackHistory();
}

static void dsm_exit_yes(void)
{
	if(mr_app_get_state() != DSM_STOP){
		mr_app_set_exit_reason(MR_EXIT_FORCE);
		mr_app_set_state(DSM_RUN);
		mr_app_terminate();
	}
}

static void dsm_exit_no(void)
{

}

void dsmShowExitInfo(void)
{
	void  mr_popup_display_question(int32 text_resid, void (*yes)(void), void (*no)(void));

	mr_popup_display_question(STR_DSM_EXITBACKAPP, dsm_exit_yes, dsm_exit_no);
}


// 接收一些系统事件，如来短消息，U盘模式等
mmi_ret mr_app_events_handler(mmi_event_struct *evt)
{
	switch (evt->evt_id)
	{
	/* 解决在VM在运行的时候不锁屏*/
	case EVT_ID_VAPP_SCREEN_LOCK_PRE_LOCK_EVT_ROUTING:
		if (mr_app_get_state() == DSM_RUN)
		{
			return MMI_RET_ERR;
		}
		break;

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1212)	
	case EVT_ID_VAPP_LAUNCHER_ACTIVE:
#else	
	case EVT_ID_VAPP_HOME_ACTIVE:
#endif	
		mr_app_system_event_hdlr(MR_SYSMSG_ENTRY_IDLESCREEN, 0, 0);
		break;

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1212)	
	case EVT_ID_VAPP_LAUNCHER_INACTIVE:
#else	
	case EVT_ID_VAPP_HOME_INACTIVE:
#endif
		mr_app_system_event_hdlr(MR_SYSMSG_EXIT_IDLESCREEN, 0, 0);
		break;	

	case EVT_ID_SRV_SMS_NEW_MSG:
		/*
			由于这个函数调用点比较靠后，改为下边的函数可以优先执行
			srv_sms_set_interrupt_event_handler(EVT_ID_SRV_SMS_NEW_MSG, (SrvSmsEventFunc)dsmReciveMsg, NULL);
		 */
		//dsmReciveMsg((srv_sms_event_struct*)evt);
		break;

	case EVT_ID_USB_ENTER_MS_MODE:
		mr_app_system_event_hdlr(MR_SYSMSG_ENTER_USB, 0, 0);
#ifdef __MR_CFG_FEATURE_DAM__
		srv_mrpfactory_refresh_app();
#endif
		break;		

	case EVT_ID_SRV_MODE_SWITCH_FINISH_NOTIFY:
		mr_app_system_event_hdlr(MR_SYSMSG_FORCE_EXIT, 0, 0);
		break;
	}
	
	return MMI_RET_OK;
}

#endif

