#ifdef __MMI_DSM_NEW__

#include "MMI_include.h"
#include "USBDeviceGprot.h"
#include "FileManagerDef.h"
#include "ExtDeviceDefs.h"
#include "IdleAppResDef.h"
#include "SettingProfile.h"
#include "mmi_rp_all_defs.h"
#include "mrp_include.h"


#if(MTK_VERSION<= 0x0812)
#include "FileManagerGprot.h"
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#if defined(  __MR_CFG_ENTRY_QQ__) &&  defined(__MR_CFG_QQ_INROM__)
#ifdef __MMI_SKYQQ__
#include "dsm_skyqq.h"
#else
#include "dsm_qq.h"
#endif
#endif


#if defined(  __MR_CFG_ENTRY_EBOOK__) &&  defined(__MR_CFG_EBOOK_INROM__)
#include "dsm_skyread.h"
#endif

#if defined(  __MR_CFG_ENTRY_ANGRYBIRD__) &&  defined(__MR_CFG_BIRD_INROM__)
#include "dsm_bird.h"
#endif

#if defined(  __MR_CFG_ENTRY_MSN__) &&  defined(__MR_CFG_MSN_INROM__)
#ifdef __MR_CFG_FEATURE_OVERSEA__
#include "dsm_msn_haiwai.h"
#endif
#endif

#if defined(  __MR_CFG_ENTRY_FACEBOOK__) &&  defined(__MR_CFG_FACEBOOK_INROM__)
#ifdef __MR_CFG_FEATURE_OVERSEA__
#include "dsm_facebook.h"
#endif
#endif


#ifdef __MR_CFG_ENTRY_SHOUXIN__
#include "dsm_shouxin.h"
#endif


#if defined(__MR_CFG_FEATURE_OVERSEA__)

#if defined( __DSM_MRPSTORE__)
#include "dsm_mrpstore.h"
#endif

#if defined(__DSM_SKYBUDDY__)
#include "dsm_skybuddy.h"
#endif

#ifdef __DSM_DSMGAME__
#include "dsm_dsmgame.h"
#endif

#ifndef __MR_CFG_FEATURE_SLIM__
#include "dsm_facetalk.h"
#endif

#else

#if defined(__MR_CFG_FEATURE_COOKIE_DOWNLOAD_MOPOZS__) ||defined(__MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__) 
#include "dsm_cookie_mini.h"
#elif !defined( __MR_CFG_ENTRY_APPSTORE__ )
#include "dsm_cookie.h"
#else
#include "dsm_appstore.h"
#endif

#endif/*__MR_CFG_FEATURE_OVERSEA__*/

#ifdef __MR_CFG_FEATURE_ONLINE_MGR__
#include "dsm_htyxgl.h"
#endif

#ifdef __MMI_DSM_COOKIE__
#ifndef __MR_CFG_FEATURE_SLIM__
#include "dsm_netgame.h"
#endif
#endif

#ifdef __MR_CFG_ENTRY_CLOCK__
extern char* ENTRY_DsmClock(void);
#endif


#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
#include "sky_qq_open.h"
char* GetSKYQQEntryMalloc(void);
#endif


#define MR_ENTRY_PARAM_LENGTH_MAX	64
static char g_entry_param[MR_ENTRY_PARAM_LENGTH_MAX];


/*
 * 在移植了挂机助手的时候，如果客户的ROM空间紧张，可以用mr_guajixz替代mr_all_list曲奇的固化在9位置，
 * 在第一次启动应用的时候发现如果曲奇不存在就会下载一次，后续只要曲奇存在就直接通过曲奇启动。
 */
#if defined( __MR_CFG_FEATURE_COOKIE_DOWNLOAD_MOPOZS__ )
	//#include "dsm_guajixz.h"//huangsunbo 20110628
	#define mr_all_list mr_guajixz
#elif defined(__MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__)
	#define mr_all_list  ((char*)0)
#endif


/* 预置应用信息 */
MR_PREINSTALLED_APPINFO_TABLE_BEGIN()
#include "mrp_entry.h"
MR_PREINSTALLED_APPINFO_TABLE_END();


#define __MR_CFG_ENTRY_MYAPP__
#ifdef __MR_CFG_ENTRY_MYAPP__	
void mrapp_license_register(void);
#endif


/*********************************************************************************
 * 本地应用权限注册函数列表
 *********************************************************************************/
/* ---- SKY_MULTIMEDIA start ---- */
#ifdef __DSM_SPLAYER__
void SPlayer_license_register(void);
#endif
/* ---- SKY_MULTIMEDIA end ---- */

MR_LAPP_LICENSE_MANAGER_BEGIN()

/* 将你的应用程序如下方式添加到下面 */
#ifdef __MR_CFG_ENTRY_MYAPP__	
	MR_LAPP_LICENSE_ENTRY("mrapp", mrapp_license_register)
#endif
/* ---- SKY_MULTIMEDIA start ---- */
#ifdef __DSM_SPLAYER__	
	MR_LAPP_LICENSE_ENTRY("splayer", SPlayer_license_register)
#endif
/* ---- SKY_MULTIMEDIA end ---- */

MR_LAPP_LICENSE_MANAGER_END();


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mr_app_find_appinfo(int item, uint32 value)
{
	int tlv;

	for(tlv = 0; tlv < MR_APPINFO_TABLE_SIZE();){
		if(MR_APPINFO_TLV_IS_HEADER(tlv) && value == MR_APPINFO_TLV_VALUE(tlv, item))
			return tlv;
		
		tlv += MR_APPINFO_TLV_LEN(tlv);
	}

	return -1;
} 


static void mr_app_clear_register_apps(void)
{
	int i;

	for(i = 0; i < MR_PREINSTALLED_APP_ENTRY_COUNT; i++){
		mr_registerAPP(0, 0, i);
	}
}


#ifdef __MR_CFG_FEATURE_COOKIE_DOWNLOAD_MOPOZS__
static void mr_app_get_cookie_path(int appinfo, char* cookie_path)
{
	if(MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_ENTRY_DATA) == (uint32)mr_guajixz
		&& MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_ENTRY_LEN) > 1024)
	{			
		mr_screeninfo scrinfo;
		mr_getScreenInfo(&scrinfo);
#ifndef __MR_CFG_FEATURE_OVERSEA__
		sprintf(cookie_path, "%%plugin%d%d/cookie.mrp", scrinfo.width, scrinfo.height);
#else
		sprintf(cookie_path, "%%mrp%dx%d/mopo.mrp", scrinfo.width, scrinfo.height);
#endif
		if(mr_info(&cookie_path[1]) == MR_IS_FILE){
			mr_trace("cookie exist");
#ifdef __MR_CFG_FEATURE_MOPOZS_DOWNLOAD_GBTABLE__
			if (mr_chset_init() == MR_SUCCESS)
#endif
			return;
		}
	}

	//reset
	cookie_path[0] = 0;
}
#endif			


#ifdef __MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__
extern void mr_dlc_start_app(char *param,mr_app_entry_f app_func);
extern void mr_dlc_set_flag(int32 index,int32 flag);

static void mr_app_get_cookie_path( char* cookie_path)
{
	mr_screeninfo scrinfo;
	mr_getScreenInfo(&scrinfo);
#ifndef __MR_CFG_FEATURE_OVERSEA__
	sprintf(cookie_path, "%%plugin%d%d/cookie.mrp", scrinfo.width, scrinfo.height);
#else
	sprintf(cookie_path, "%%mrp%dx%d/mopo.mrp", scrinfo.width, scrinfo.height);
#endif
}
int32 mr_dlc_get_down_screen_id(void)
{
	return SCR_DL_APPLIST; 
}
 int32 mr_dlc_get_card_drv(void)
 {
 	return MMI_CARD_DRV;
 }
  int32 mr_dlc_get_public_drv(void)
 {
 	return MMI_PUBLIC_DRV;
 }

void mr_dlc_ShowCategory(U8* progressInfo,S32 iBufLength,U8* gui_buffer)
{
	ShowCategory74Screen(STR_GLOBAL_PLEASE_WAIT, 0,0, 0,
								STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
								progressInfo, iBufLength,gui_buffer );
}
void mr_dlc_set_fileatt(WCHAR* path)
{
#ifdef __MMI_DSM_HIDE_FOLDER__
		FS_SetAttributes((WCHAR *)path, FS_GetAttributes((WCHAR *)path)|FS_ATTR_HIDDEN);	
#endif
}

#endif			

char g_dsm_backup_entry[64]={0};//huangsunbo 20110622
static void mr_app_menu_start(int appinfo)
{	
	int32 saved_appinfo = appinfo;
	int32 appid = MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_HEADER_APPID);
	char cookie_path[MR_ENTRY_PARAM_LENGTH_MAX] = {0};
#if defined(__MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__)
	mr_app_entry_f dlc_entry_fun = 0;
#endif
	
	if( GetDsmState() != DSM_STOP  
		&&  mr_backstage_get_appid() ==  appid)
	{
		mr_backstage_activate_cur_app();
		return;
	}
	
	if(dsmInit() != MR_SUCCESS){
		return;
	}

	for(appinfo += MR_APPINFO_TLV_LEN(appinfo); appinfo < MR_APPINFO_TABLE_SIZE();)
	{	
		if(MR_APPINFO_TLV_TYPE(appinfo) != MR_APPINFO_TLV_TYPE_ENTRY){
			break;
		}
		
#ifdef __MR_CFG_FEATURE_COOKIE_DOWNLOAD_MOPOZS__
		if(MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_ENTRY_INDEX) == 9)
			mr_app_get_cookie_path(appinfo, cookie_path);
#endif			

		mr_registerAPP((uint8*)MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_ENTRY_DATA)
			, MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_ENTRY_LEN)
			, MR_APPINFO_TLV_VALUE(appinfo, MR_APPINFO_TLV_ENTRY_INDEX));

		appinfo += MR_APPINFO_TLV_LEN(appinfo);
	}

	dsmEntry = NULL;
	switch(MR_APPINFO_TLV_TYPE(saved_appinfo))
	{
		case MR_APPINFO_TLV_TYPE_HEADER_DSMENTRY:
		{
			dsmEntry = (char*)MR_APPINFO_TLV_VALUE(saved_appinfo, MR_APPINFO_TLV_HEADER_ENTRY);
			break;
		}
		
		case MR_APPINFO_TLV_TYPE_HEADER_CALLBACK:
		{
			mr_app_entry_f entry_fun = (mr_app_entry_f)MR_APPINFO_TLV_VALUE(saved_appinfo, MR_APPINFO_TLV_HEADER_ENTRY);
			if(!entry_fun) return;
			dsmEntry = entry_fun();
#if defined(__MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__)
			 dlc_entry_fun = entry_fun;
#endif
			break;
		}
		
		default:
			return;
	}

	if(dsmEntry)
	{
		memset(g_dsm_backup_entry,0,sizeof(g_dsm_backup_entry));
		strcpy(g_dsm_backup_entry,dsmEntry);
#ifdef __MR_CFG_FEATURE_COOKIE_DOWNLOAD_MOPOZS__
		if(cookie_path[0]){
			strcat(cookie_path, ",");
			strcat(cookie_path, dsmEntry);
			strcpy(g_entry_param, cookie_path);
			dsmEntry = g_entry_param;
		}else{
			//mr_backstage_set_online_mgr_running();//huangsunbo 20110412
		}
#elif defined(__MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__)
		if(mr_dlc_check_download())
		{
			mr_app_get_cookie_path(cookie_path);
			strcat(cookie_path, ",");
			strcat(cookie_path, dsmEntry);
			strcpy(g_entry_param, cookie_path);
			dsmEntry = g_entry_param;
		}
		else
		{
			//没有游戏中心需要下载
			mr_dlc_start_app(dsmEntry,dlc_entry_fun);
			return;
		
		}
#endif

		mr_app_entry_dsm_screen();
	}
}


static void mr_app_menu_entry_hdlr(void)
{
	U16 hilite_id;
	int appinfo;

	hilite_id = GetSeqItemId_Ext(GetParentHandler(), GetCurrHiliteID());
	appinfo = mr_app_find_appinfo(MR_APPINFO_TLV_HEADER_MENUID, hilite_id);

	mr_trace("mr_app_menu_entry_hdlr: %d, %d", hilite_id, appinfo);
	
	if(appinfo >= 0){
		mr_app_menu_start(appinfo);
	}
}


static void mr_app_menu_hilite_hdlr()
{
	ChangeLeftSoftkey(STR_GLOBAL_OK, IMG_GLOBAL_OK);
	ChangeRightSoftkey(STR_GLOBAL_BACK,IMG_GLOBAL_BACK);
	SetLeftSoftkeyFunction(mr_app_menu_entry_hdlr,KEY_EVENT_UP);
	SetRightSoftkeyFunction(GoBackHistory,KEY_EVENT_UP);   	
	SetCenterSoftkeyFunction(mr_app_menu_entry_hdlr, KEY_EVENT_UP);
	ChangeCenterSoftkey(NULL, IMG_GLOBAL_COMMON_CSK);
}


void mr_app_menu_initialize(void)
{
#if !defined(__COSMOS_MMI__)
	int tlv;

	for(tlv = 0; tlv < MR_APPINFO_TABLE_SIZE();){
		if(MR_APPINFO_TLV_IS_HEADER(tlv) && MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_HEADER_MENUID)){
			SetHiliteHandler(MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_HEADER_MENUID), mr_app_menu_hilite_hdlr);
		}
		
		tlv += MR_APPINFO_TLV_LEN(tlv);
	}		

	/*如果移植了矢量字体，多媒体，天气助手，需要打开下面的代码*/
#if defined(__HC_CALL__) || defined(__MMI_SKY_FONT__) ||defined(__DSM_MPLIVE_SUPPORT__)
	{
		extern void HighlightDsmGame(void);
		SetHiliteHandler(MENU_DSM_GAME_CENTER, HighlightDsmGame);
	}
#endif
#endif
}


int32 mr_app_start_mrp_by_menuid(uint32 menuid)
{
	int appinfo = mr_app_find_appinfo(MR_APPINFO_TLV_HEADER_MENUID, menuid);
	
	if(appinfo >= 0){
		mr_app_menu_start(appinfo);
		return MR_SUCCESS;
	}

	return MR_FAILED;
}


int32 mr_app_start_mrp_by_appid(uint32 appid)
{
	int appinfo = mr_app_find_appinfo(MR_APPINFO_TLV_HEADER_APPID, appid);
	
	if(appinfo >= 0){
		mr_app_menu_start(appinfo);
		return MR_SUCCESS;
	}

	return MR_FAILED;
}


void mr_app_start_mrp_by_path(const char* path)
{
	if(dsmInit() == MR_SUCCESS)
	{
		mr_fs_set_launch_dir(__MR_CFG_VAR_CUSTOM_DRV__, __MR_CFG_VAR_CUSTOM_PATH__);
		if( mr_info("dsm_gm.mrp") == MR_IS_FILE )//huangsunbo 20110217
		{
			dsmEntry = (char*)path;
			mr_app_entry_dsm_screen();
		}
	}
}

void mr_app_start_mrp_by_cookie(const char* data)
{
	char param[200]={0};
	
	memset(param,0,200);
	#ifndef __MR_CFG_FEATURE_DAM__
	sprintf(param, "*J,%s", data);
	#else
	sprintf(param, "*J,%s,0", data);//huangsunbo 20120503
	#endif

	if(dsmInit() != MR_SUCCESS)
		return;
	
	mr_registerAPP((U8 *)mr_all_list, sizeof(mr_all_list), 9);
	#ifndef __MR_CFG_FEATURE_OVERSEA__
	mr_registerAPP((U8 *)mr_alllist_cfg, sizeof(mr_alllist_cfg), 18);
	#endif
	dsmEntry = param;
	mr_app_entry_dsm_screen();
}


int32 mr_backstage_get_online_mgr(uint8** data, int32* len)
{
#ifdef __MR_CFG_FEATURE_ONLINE_MGR__
	if(!data || !len)
		return MR_FAILED;
	
	*data = (uint8*)mr_htyxgl;
	*len = sizeof(mr_htyxgl);
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __MED_IN_ASM__
void mr_exit_by_asm(void)
{
	if (mr_app_get_state() == DSM_STOP)
	{
		applib_mem_ap_notify_stop_finished(APPLIB_MEM_AP_ID_MYTHROAD, KAL_TRUE);
		return;
	}

	mr_app_exit();
	applib_mem_ap_notify_stop_finished(APPLIB_MEM_AP_ID_MYTHROAD, KAL_FALSE);
}
#endif


void mr_system_intialize(void)
{
	mr_system_environment_setup();
	mr_app_menu_initialize();
	mr_license_mgr_initialize(MR_APPINFO_TABLE_SIZE());

#ifdef __MED_IN_ASM__
	applib_mem_ap_register(APPLIB_MEM_AP_ID_MYTHROAD, STR_DSM_GAME_CENTER, IMG_DSM_LIST_GAME, mr_exit_by_asm);
#endif

#ifdef __SKY_SECURITY_GUARD__
	sg_initialize();
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mr_app_show_factory_info(void)
{
	static char id[400] = {0};
	char tid[200] ={0};
	mr_userinfo info={0};
	
	extern U8 *build_date_time(void);

	mr_app_EntryNewScreen(IDLE_SCREEN_ID_DUMMY, NULL, mr_app_show_factory_info, MMI_FRM_SMALL_SCRN);

	mr_getUserInfo(&info);
	#ifndef __MR_CFG_FEATURE_OVERSEA__
	sprintf(tid, "Factory Id:%s\r\nHandset Id:%s\r\nExt Handset Id:%s\r\ntime:%s", info.manufactory,  info.type, release_verno(),build_date_time());
	#else
	sprintf(tid, "Factory Id:%s\r\nHandset Id:%s\r\ntime:%s", info.manufactory,  info.type, build_date_time());
	#endif
	mmi_asc_to_ucs2((S8 *)id, (S8 *)tid);
	ShowCategory74Screen(0, 0, STR_GLOBAL_OK,IMG_GLOBAL_OK, NULL, NULL, (U8*)id, gui_strlen((UI_string_type)id), NULL);
	SetLeftSoftkeyFunction(GoBackHistory, KEY_EVENT_UP);
} 


void dsmShowId(void)
{
	mr_app_show_factory_info();
}


void dsm_gm(void)
{
	mr_app_start_mrp_by_path("%dsm_gm.mrp");
}


void TEST_GM(void)
{
	dsm_gm();
}


void TEST_SHOW_ID(void)
{
       dsmShowId();
}


int32 dsmInit(void)
{
	int i =0;

	if(dsmGameInit() != MR_SUCCESS){
		return MR_FAILED;
	}
	
	if(GetDsmState() != DSM_STOP)
	{
		mr_trace("dsmInit: %d, %d", mr_backstage_get_appid(), mr_backstage_is_interactive());
		if(mr_backstage_get_appid() != MR_APPID_HTYXGL || mr_backstage_is_interactive())
		{
			dsmShowExitInfo();
			return MR_FAILED;
		}else{
			mr_app_exit();//just exit the current mrp
		}
	}
	/*
	6235,36 NAND Flash的时候，无T卡的时候，工作目录直接切换到手机盘
	*/
#ifdef NAND_SUPPORT
	if( srv_fmgr_drv_is_accessible(SRV_FMGR_CARD_DRV) ==  MMI_TRUE)
	{
		mr_fs_set_launch_dir(__MR_CFG_VAR_CUSTOM_DRV__, __MR_CFG_VAR_CUSTOM_PATH__);
	}
	else
	{
		mr_fs_set_launch_dir(MMI_PUBLIC_DRV, __MR_CFG_VAR_CUSTOM_PATH__);
	}
#else
	mr_fs_set_launch_dir(__MR_CFG_VAR_CUSTOM_DRV__, __MR_CFG_VAR_CUSTOM_PATH__);
#endif
	mr_app_clear_register_apps();


#if (defined(MT6223P)||defined(MT6223)||defined(MT6223P_S00))
	DisplayConfirm(NULL,NULL,NULL,NULL, (UI_string_type)GetString(STR_DSM_WAITING),IMG_GLOBAL_INFO, 0);
#endif

	return MR_SUCCESS;
}


/*请不要改这个函数的名字,移植说明里面提到了该函数*/
int dsmGameInit(void)
{
#ifdef __USB_IN_NORMAL_MODE__
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)
	if (srv_usb_is_in_mass_storage_mode())
#else
	if (mmi_usb_is_in_mass_storage_mode())
#endif
	{
		mmi_frm_nmgr_balloon(MMI_SCENARIO_ID_DEFAULT, MMI_EVENT_INFO_BALLOON, MMI_NMGR_BALLOON_TYPE_INFO, (const WCHAR*)GetString(STR_GLOBAL_NOT_AVAILABLE_IN_MASS_STORAGE_MODE));
		return MR_FAILED;
	}
#endif

#if 0//ndef NAND_SUPPORT//huangsunbo 20120313 去掉T卡判断
#ifndef WIN32
	/*判断是否有t card */
	if(!srv_fmgr_drv_is_accessible(SRV_FMGR_CARD_DRV))
	{
		mmi_frm_nmgr_balloon(MMI_SCENARIO_ID_DEFAULT, MMI_EVENT_INFO_BALLOON, MMI_NMGR_BALLOON_TYPE_INFO, (const WCHAR*)GetString(FMGR_FS_MSDC_NOT_PRESENT_TEXT));
		return MR_FAILED;
	}
#endif	
#endif

/***************************************************
	必须停掉蓝牙立体声
	进入游戏必须提示停止以下设备
	请开启以下注释掉的代码,并添加相应的字符串

****************************************************/

#if 0
#ifdef __MMI_BT_SUPPORT__ 
#if defined(__MMI_BT_HP_SUPPORT__)
	if(mmi_bt_is_hs_connected())
#elif defined(__MMI_BT_SUPPORT__)
	if(mmi_bt_is_hs_connected() || mmi_bt_is_a2dp_connected())
#endif	
	{
		DisplayPopup((U8 *)GetString(STR_DSM_PLZ_STOP_BT), IMG_GLOBAL_INFO, NULL, UI_POPUP_NOTIFYDURATION_TIME, NULL);
		return MR_FAILED;
	}
#endif		

#ifdef __MMI_AUDIO_PLAYER__ 
	if( mmi_audply_is_active() )// 需要客户自己实现 audply is active
	{
		DisplayPopup((U8 *)GetString(STR_DSM_PLZ_STOP_AUDIO), IMG_GLOBAL_INFO, NULL, UI_POPUP_NOTIFYDURATION_TIME, NULL);
		return MR_FAILED;
	}
#endif

#ifdef __MMI_WEBCAM__ 
	if( mmi_webcam_is_active() )// 需要客户实现 webcame is active
	{
		DisplayPopup((U8 *)GetString(STR_DSM_PLZ_STOP_WEBCAMERA), IMG_GLOBAL_INFO, NULL, UI_POPUP_NOTIFYDURATION_TIME, NULL);
		return MR_FAILED;
	}
#endif

#endif
	return MR_SUCCESS;
}


void DsmBook(void)
{
	mr_app_start_mrp_by_appid(MR_APPID_SKYREAD);
}


void DsmGame(void)
{
#ifndef __MR_CFG_FEATURE_OVERSEA__
	mr_app_start_mrp_by_menuid(MENU_DSM_GAME_CENTER);
#else
	mr_app_start_mrp_by_menuid(MENU_DSM_MRPSTORE);
#endif
}

#ifdef __MR_CFG_ENTRY_SKYBUDDY__
void DsmSkybuddy(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_SKYBUDDY);
}
#endif




#ifdef __MR_CFG_ENTRY_NETGAME__
void DsmNetGame(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_NETGAME);
}
#endif

#ifdef __MR_CFG_ENTRY_SHOUXIN__
void DsmShouxin(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_SHOUXIN);
}
#endif


#ifdef __MR_CFG_ENTRY_JS__
void DsmJs(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_JS);
}
#endif

#ifdef __MR_CFG_ENTRY_QQ__
void DsmQQ(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_QQ);
}
#endif


#ifdef __MR_CFG_ENTRY_MSN__
void DsmMsn(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_MSN);
}
#endif


#ifdef __MR_CFG_ENTRY_MPCHAT__
void DsmMpchat(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_MPCHAT);
}
#endif

#ifdef __MR_CFG_ENTRY_BYDR__
void DsmBydr(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_BYDR);
}
#endif

#ifdef __MR_CFG_ENTRY_MPLIVE__
void DsmMplive(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_MPLIVE);
}
#endif

#ifdef __MR_CFG_ENTRY_TOMCAT__
void DsmTomcat(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_TOMCAT);
}
#endif


#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
void DsmBird(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_ANGRYBIRD);
}
#endif

#ifdef __MR_CFG_ENTRY_SKIRT__
void DsmSkirt(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_SKIRT);
}
#endif


#ifdef __MR_CFG_ENTRY_FRUIT__
void DsmFruit(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_FRUIT);
}
#endif

#ifdef __MR_CFG_ENTRY_FETION__
void DsmFetion(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_FETION);
}
#endif

#ifdef __MR_CFG_ENTRY_KAIXIN__
void DsmKaixin(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_KAIXIN);
}
#endif

#ifdef __MR_CFG_ENTRY_SINAWEIBO__
void DsmWeibo(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_SINAWEIBO);
}
#endif

#ifdef __MR_CFG_ENTRY_TIANQI__
void DsmTianq(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_TIANQI);
}
#endif


#ifdef __MR_CFG_ENTRY_NETCLOCK__
void DsmNetClock(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_NETCLOCK);
}
#endif


#ifdef __SKY_SECURITY_GUARD__
void DsmGuard(void)
{
	mr_app_start_mrp_by_menuid(MENU_DSM_SAFE);
}
#endif



#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
void DsmSKY_QQ(void)
{
#ifndef __MMI_NGUAQ__
	mr_app_start_mrp_by_appid(MR_APPID_SKYQQ);
#else
	mr_app_start_mrp_by_appid(MR_APPID_NGUAQ);
#endif
}

char* GetSKYQQEntryMalloc(void)
{
	char param[MR_ENTRY_PARAM_LENGTH_MAX] = {0};
	int32 ret ;

	#ifdef __MMI_NGUAQ__
	/*在分配dsm的内存之前分配QQ所需内存*/
	Sky_qq_TotalMemory_Malloc();  
	#endif

	memset(g_entry_param,0,sizeof(char)*MR_ENTRY_PARAM_LENGTH_MAX);
	if((GetSkyQQRunState() == QQ2008_BACK_RUN )|| (GetSkyQQRunState() == QQ2008_PAUSE))
	{
		sprintf(param, "%s%d%d/%s", "app",UI_device_width,UI_device_height,"skyqq.mrp");
		ret = mr_info(param);
		if(MR_IS_FILE == ret)
		{
				memset(param,0,sizeof(param));
				#ifndef __MMI_NGUAQ__
				sprintf(param, "*J,skyqq,203106");
				#else
				sprintf(param, "*J,nguaq,203114");
				#endif
				memcpy(g_entry_param,param,strlen(param));
		}
		else
		{
			/*
			如果也没有固化版本，那么在以下的处理中就直接退出QQ。
			*/
			#ifdef __MR_CFG_QQ_INROM__
				mr_trace("----DsmSKY_QQ---11111-");
				memset(param,0,sizeof(param));
				//mr_registerAPP((uint8 *) mr_skyqq, sizeof(mr_skyqq), 10);//固化的skyqq用start版本
				#ifndef __MMI_NGUAQ__
				sprintf(param, "*J,skyqq,203106");
				#else
				sprintf(param, "*J,nguaq,203114");
				#endif
				memcpy(g_entry_param,param,strlen(param));
			#else
				//huangsunbo 20100524 如果SKYQQ文件被意外删除，激活QQ的时候，强制退出
				//防止出现上wap情况
				sky_qq_force_exit();
				return NULL;
			#endif
		}
	   }
	   else
	   {
       	memset(param,0,sizeof(param));
		#ifndef __MMI_NGUAQ__
		sprintf(param, "*J,skyqq");
		#else
		sprintf(param, "*J,nguaq");
		#endif
		memcpy(g_entry_param,param,strlen(param));    
	   }
	   
	mr_trace("GetSKYQQEntryMalloc g_entry_param:%s\n",g_entry_param);
	return g_entry_param;
}
#endif/*__MMI_SKYQQ__*/

#ifdef __MMI_SKY_IPHONE__
#ifdef __MR_CFG_FEATURE_DAM__
S32 mr_dsm_start_by_appinfo_iphone(mr_dam_appinfo_t* appinfo,S32 id)
{
	S32 len;
	char param[200]={0};
	if(!checkSkyAPP(id))
		return;
	
	memset(param,0,200);
	sprintf(param, "*J,%s", appinfo->start_data);
	
#ifdef __IPHONE_TRACE__
	kal_prompt_trace(MOD_MMI,"lin-mr_dam_start: %s", param);
#endif

	if(appinfo)
	{
		if(dsmInit() != MR_SUCCESS)
			return MR_FAILED;
		
		mr_registerAPP((U8 *)mr_all_list, sizeof(mr_all_list), 9);
		mr_registerAPP((U8 *)mr_alllist_cfg, sizeof(mr_alllist_cfg), 18);
		
		dsmEntry = param;
		mr_app_entry_dsm_screen();
		return MR_SUCCESS;
	}
	return MR_FAILED;
}
#else
S32 mr_dsm_start_by_appinfo_iphone(void)
{
	return MR_FAILED;
}
#endif
#endif/*__MMI_SKY_IPHONE__*/


#ifdef __PME_SUPPORT__
static char s_mr_app_cust_param[256] = {0};
static int32 s_mr_app_cust_appid = 0;

static void mr_app_firm_main_mrp(void)
{
	mr_registerAPP((uint8*)mr_all_list, sizeof(mr_all_list), 9);
}

static void mr_app_start_mrp_by_param(int32 appid, char* saveName, int32 isDownload)
{
	//static char temp[100] = {0};
	char temp[100] = {0};
	//*J,1000,5,0,3,3,5,0,0,rstone,814039,rstone.mrp
	
	if( GetDsmState() != DSM_STOP  
		&&  mr_backstage_get_appid() ==  appid)
	{
		mr_backstage_activate_cur_app();
		return;
	}
	
	if(dsmInit() != MR_SUCCESS){
		return;
	}
#ifdef __DSM_OVERSEA__
	sprintf(temp, "*J,%d,0,3,3,5,0,0,APP,%d,%s", (isDownload ? 6 : 5), appid, saveName);
#else
	sprintf(temp,"*J,%s,%d",saveName,appid);
#endif
	dsmEntry = temp;
	mr_app_firm_main_mrp();
	mr_app_entry_dsm_screen();
	if(mr_app_get_state() != DSM_STOP && (strstr(dsmEntry, "qq") ||strstr(dsmEntry, "QQ"))){
		mr_backstage_hold_endkey(1);
	}
}

void mr_pme_port_start_app(int32 appid, char* param, char* saveName, int32 isDownload)
{
	s_mr_app_cust_appid = appid;
	memset(s_mr_app_cust_param, 0, sizeof(s_mr_app_cust_param));
	if(param){
		strncpy(s_mr_app_cust_param, param, sizeof(s_mr_app_cust_param) - 1);
	}
	
	if(isDownload){
		mr_app_start_mrp_by_param(appid, saveName, TRUE);
	}else{
		if(mr_app_start_mrp_by_appid(appid) != MR_SUCCESS){//no firm
			mr_app_start_mrp_by_param(appid, saveName, FALSE);
		}
	}
}

int32 mr_app_get_cust_param(uint8* input, int32 input_len, uint8** output, int32* outputLen)
{
	if(s_mr_app_cust_param[0] && *((int32*)input) == s_mr_app_cust_appid && input_len == 4){
		*output = (uint8*)s_mr_app_cust_param;
		*outputLen = strlen(s_mr_app_cust_param);
		return MR_SUCCESS;
	}

	return MR_FAILED;
}

typedef struct
{
         int32 set; // see pme_runmode_en
         int32 hid;// 非零为有效的id
         int32 hcd; // 非零为有效的id
}mr_pme_hids_st;

int32 mr_app_get_pme_hids(uint8* input, int32 inputLen)
{
	int32 ret;
	mr_pme_hids_st* hids = (mr_pme_hids_st*)input;
	int32 hcd;

	ret = pme_get_hid_hcd(&hids->hid, &hids->hcd, &hids->set);
	return ret;
}

int32 mr_app_set_pme(uint8*input,int32 inputlLen)
{
	return pme_set_mode(*input);
}
#else
int32 mr_app_get_cust_param(uint8* input, int32 input_len, uint8** output, int32* outputLen)
{
	return MR_IGNORE;
}

int32 mr_app_get_pme_hids(uint8* input, int32 inputLen)
{
	return MR_IGNORE;
}

int32 mr_app_set_pme(uint8*input,int32 inputlLen)
{
	return MR_IGNORE;
}
#endif/*__PME_SUPPORT__*/


#endif/*__MMI_DSM_NEW__*/

