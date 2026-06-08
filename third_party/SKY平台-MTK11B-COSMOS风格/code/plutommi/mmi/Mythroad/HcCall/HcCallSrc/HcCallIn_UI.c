#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 
/*-------------------------------------------------------------------------------*
~{:<V]K9?-0fH(KySP~}
File: HcCallIn_UI.c
Create: 2009.09.23
--------------------------------------------------------------------------------*/
#include "dsm_datatype.h" 

#include "GlobalMenuItems.h"
#include "GlobalDefs.h"
#include "GlobalConstants.h" //KEY_EVENT_UP
//#include "HistoryGprot.h"  //extern void GoBackHistory()
#include "HcCallDef.h"
#include "HcCallIn_UI.h"
#include "MainMenuDef.h"
#include "wgui_inline_edit.h"
#include "HcFileAdapter.h"
#include "CommonScreens.h"
#include "app_datetime.h"
#include "timerEvents.h"

#include "UCMGProt.h"
#include "PhoneBookTypes.h"
#include "MTPNP_AD_resdef.h"
#include "PhoneBookProt.h"
#include "PhoneBookResDef.h"
#include "Mtpnp_ad_common_def.h"

#include "ems.h"
#include "SmsGuiInterfaceType.h"

#ifdef __MMI_DUAL_SIM_MASTER__
#include "MTPNP_AD_master_if.h"
#endif

#include "SMSApi.h"
#include "MessagesresourceData.h"
#include "SettingProfile.h"
#include "PixtelDatatypes.h"
#include "SimDetectionDef.h"
#include "USBDeviceGprot.h"
#include "FileManagerGProt.h"
#include "UCMResDef.h"


#include "mrp_unet.h"
#include "mrp_sms.h"
#include "mrp_sysinfo.h"

#include "hccall_net.h"
#include "Section_Data.h"

#include "ldzs.h"
#include "mmi_rp_vapp_hccall_def.h"
#include "mrp_features.h"
#include "IdleAppResDef.h"


//////////////////////////////////////////////////////////////////////
//extern functions:
//////////////////////////////////////////////////////////////////////

extern int LocationData_Init_ex(void);
extern int GetLocation_ex( char *area, int size, char *tel );
extern void hccall_set_firmdatabase(const unsigned char* pFirmDatabase, uint32 firmDatabase);
extern int HC_get_driver_char(int type);

//USB
#ifdef __USB_IN_NORMAL_MODE__
extern pBOOL mmi_usb_is_in_mass_storage_mode(void);
#ifndef WIN32
extern void mmi_usb_app_unavailable_popup(U16 stringId);
#endif
#endif

//#ifdef __SKYENGINE_V2__
extern void mr_sim_initialize(void);
//#else
//extern void dsmInitSimInfo(void);
//#endif


//////////////////////////////////////////////////////////////////////
//global variables:
//////////////////////////////////////////////////////////////////////

extern HCCALL_NETSTATE g_hccall_netstate;
extern const U16 gIndexIconsImageList[];


//section
static char   g_HCNumber[HCNUMBER_LEN];
static char   g_HCStrLocation[HCSTRLOCATION_LEN];
static char   g_HCStrGsdLocation[HCSTRLOCATION_LEN];
static char   g_HcStr_Number[HCNUMBER_LEN];

typedef enum
{
    MR_LIST_NO_ICON = 0,
    MR_LIST_NORMAL_ICON = 1,
    MR_LIST_SINGLE_SEL = 2,
    MR_LIST_MUTI_SEL = 3,
    MR_LIST_TYPE_MAX
};

typedef  int32 (*mr_localui_event_handler)(int32 handle, int16 type ,int32 param1,int32 param2);

int32 hccall_menuCreate(const char * title, int16 num,int16 type, mr_localui_event_handler handler)
{
	char *tmp = NULL;
	mmi_id menu = 0;
	int32 length = mr_str_wstrlen(title);
	
    tmp = OslMalloc(length + 2);
    memcpy(tmp, title, length + 2);
	
#ifndef WIN32
	mr_str_convert_endian(tmp);
#endif

	menu = mr_menuCreate_ex(tmp, num, type, handler);

	OslMfree(tmp);

	return menu;
}

int32 hccall_menuSetItem(int32 menu, const char* text, int32 index, int32 select)
{
	char *tmp = NULL;
	int32 length = mr_str_wstrlen(text);
	int32 ret = 0;
	
    tmp = OslMalloc(length + 2);
    memcpy(tmp, text, length + 2);
	
#ifndef WIN32
	mr_str_convert_endian(tmp);
#endif

	ret = mr_menuSetItemEx(menu, tmp, index, select);

	OslMfree(tmp);

	return ret;
}

int32 hccall_textCreate(const char * title, const char * text, int32 type, mr_localui_event_handler handler)
{
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	mmi_id textId = 0;
	int32 length1 = mr_str_wstrlen(title);
	int32 length2 = mr_str_wstrlen(text);
	
    tmp1 = OslMalloc(length1 + 2);
    memcpy(tmp1, title, length1 + 2);
    tmp2 = OslMalloc(length2 + 2);
    memcpy(tmp2, text, length2 + 2);
	
#ifndef WIN32
	mr_str_convert_endian(tmp1);
	mr_str_convert_endian(tmp2);
#endif

	textId = mr_textCreate_ex(tmp1, tmp2, type, handler);

	OslMfree(tmp1);
	OslMfree(tmp2);

	return textId;
}

int32 hccall_editCreate(const char * title, const char * text, int32 type, int32 max_size, mr_localui_event_handler handler)
{
	char *tmp1 = NULL;
	char tmp2[2];
	mmi_id edit = 0;
	int32 length1 = mr_str_wstrlen(title);
	
	tmp1 = OslMalloc(length1 + 2);
	memcpy(tmp1, title, length1 + 2);
	memset(tmp2, 0, 2);
	
#ifndef WIN32
	mr_str_convert_endian(tmp1);
#endif

	edit = mr_editCreate_ex(tmp1, tmp2, type, max_size, handler);

	OslMfree(tmp1);

	return edit;
}

const char* hccall_editGetText(int32 edit)
{
	char *tmp = NULL;

	tmp = (char *)mr_editGetText(edit);
	
#ifndef WIN32
	mr_str_convert_endian(tmp);
#endif

	return tmp;
}

/********************************************************************
Function:		hccall_load_userconfig
Description: 	load the user's config setting
Input:		void
Output:		g_HcUserConfig_p
Return:		1: success; 0:failed
Notice:		
*********************************************************************/
int hccall_load_userconfig(HCCALL_CONFIG *g_HcUserConfig_p)
{
	int32 fhandle = 0;
	uint32 readCount;
	int32 result = 0;
	
	result = HcFile_Open(&fhandle, (int8*)HCUSER_CONFIG_F ,PO_RDONLY, 0);

	if( !result )
	{
		if( fhandle > 0 )
		{
			HcFile_Close(fhandle, 0);
		}
		return result;
	}
	HcFile_Read(fhandle, g_HcUserConfig_p, sizeof(HCCALL_CONFIG), &readCount, 0);
	if(readCount == sizeof(HCCALL_CONFIG))
	{
		result  = 1;
	}
	else
	{
		result  = 0;
	}
	
	HcFile_Close(fhandle, 0);

	return result;
}


/********************************************************************
Function:		hccall_save_userconfig
Description: 	save the user's config setting
Input:		user's config setting
Output:		void
Return:		1: success; 0:failed
Notice:		
*********************************************************************/
int hccall_save_userconfig(HCCALL_CONFIG  *g_HcUserConfig_p)
{
	int32 fhandle = 0;
	uint32 writeCount;
	int32 result = 0;
	
	result = HcFile_Open(&fhandle, (int8*)HCUSER_CONFIG_F ,PO_WRONLY, 0);

	if( !result )
	{
		if( fhandle > 0 )
		{
			HcFile_Close(fhandle, 0);
		}
		return result;
	}
	HcFile_Write(fhandle, g_HcUserConfig_p, sizeof(HCCALL_CONFIG), &writeCount);
	
	if(writeCount == sizeof(HCCALL_CONFIG))
	{
		result  = 1;
	}
	else
	{
		result  = 0;
	}
	
	HcFile_Close(fhandle, 0);    
	return result;
}


/********************************************************************
Function:		HcCall_CheckUsbIsInMassStorageMode
Description: 	check whether the sd card is in mass storage mode
Input:		bPopupTip: wheather show the tip info		
Output:
Return:		TURE:yes, FALSE:no
Notice:
*********************************************************************/
BOOL HcCall_CheckUsbIsInMassStorageMode()
{
#ifdef __USB_IN_NORMAL_MODE__
	if (mmi_usb_is_in_mass_storage_mode())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#else
	return FALSE;
#endif
}


/********************************************************************
Function:		HcCall_CheckSDCard
Description: 	check whether the sd card is exist
Input:		bPopupTip: whether show the tip info		
Output:
Return:		TURE:exist, FALSE:not exist
Notice:
*********************************************************************/
BOOL HcCall_CheckSDCard()
{
#ifdef __USB_IN_NORMAL_MODE__
	if ((FS_NO_ERROR != FS_GetDevStatus(MMI_CARD_DRV,FS_MOUNT_STATE_ENUM)))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#else
	return TRUE;
#endif
}


/********************************************************************
Function:		HcCall_CheckSDCardNoMemory
Description: 	check whether our sd card has enough momery for downloading
Input:		void	
Output:
Return:		TURE:No memory; 
			FALSE:enough momory
Notice:		500K
*********************************************************************/
BOOL HcCall_CheckSDCardNoMemory()
{
	U16 path[20];
	S8 buf[20];
	S32 fs_ret;
	U64 disk_free_space;
	U64 disk_total_space;
	FS_DiskInfo disk_info;

	sprintf(buf, "%c:\\", HC_get_driver_char(HC_DRIVER_CARD));
	mmi_asc_to_ucs2((S8*)path, (S8*)buf);

	fs_ret = FS_GetDiskInfo(path, &disk_info, FS_DI_BASIC_INFO | FS_DI_FREE_SPACE);
	disk_free_space = 0;
	disk_total_space = 0;
	
	if (fs_ret >= 0)
	{
		disk_free_space = (U64)disk_info.FreeClusters * disk_info.SectorsPerCluster * disk_info.BytesPerSector;
		disk_total_space = (U64)disk_info.TotalClusters * disk_info.SectorsPerCluster * disk_info.BytesPerSector;
		if(disk_free_space<512000)
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL hccall_check_simcard(void)
{
	if (mr_getNetworkID() == MR_NET_ID_NONE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/********************************************************************
Function:		Hccall_Update_Database_Cancle
Description: 	cancel the database updating
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void Hccall_Update_Database_Cancle(void)
{
	hccall_net_close();
	GoBackHistory();
}


/********************************************************************
Function:		Hccall_Update_Database_Cancle
Description: 	cancel the database updating
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_autoupdate_flashlastmonth(void)
{
	mr_datetime datetime;

	mr_getDatetime(&datetime);

	g_hccall_confg.lastmonth = datetime.year*12+datetime.month;
	hccall_save_userconfig(&g_hccall_confg);

	FILE_TRACE("HCCALLUI :: hccall_ui_autoupdate_flashlastmonth, time: %d:%d:%d:%d:%d:%d", 
				datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second);

	return;
}

int g_dldpercent = 0;
U8 g_str_buffer[512] = {0};
extern UI_string_type MMI_message_string;
/********************************************************************
Function:		hccall_ui_update_dldprocess
Description: 	updating the downloading process window
Input:		percent: the downloading percent
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_update_dldprocess(int percent)
{
	g_dldpercent = percent;

	if (HcCall_CheckUsbIsInMassStorageMode())
	{
		hccall_net_close();
		
		if ((GetActiveScreenId() == SCR_ID_HCCALL_UPDATE_DOWNLOADING) ||
			(GetActiveScreenId() == SCR_ID_HCCALL_UPDATE_CONNECTING))
		{
			GoBackHistory();
		}
		else
		{
			DeleteScreenIfPresent(SCR_ID_HCCALL_UPDATE_DOWNLOADING);
			DeleteScreenIfPresent(SCR_ID_HCCALL_UPDATE_CONNECTING);
		}	

		return;
	}
	
	if (GetActiveScreenId() == SCR_ID_HCCALL_UPDATE_DOWNLOADING)
	{
		memset(g_str_buffer, 0, sizeof(g_str_buffer));
		kal_wsprintf((kal_wchar*)g_str_buffer, "%w%d%w", (kal_wchar*)GetString(STR_ID_HCCALL_DOWNLOADING), percent, L"%");

		MMI_message_string = (UI_string_type)g_str_buffer;
		cat66_update_progress_string();

		if (g_dldpercent >= 100)
		{
			GoBackHistory();
		}
	}
	else
	{
		if (g_dldpercent >= 100)
		{
			DeleteScreenIfPresent(SCR_ID_HCCALL_UPDATE_DOWNLOADING);
		}
	}
}


/********************************************************************
Function:		hccall_ui_handle_endkey
Description: 	handle the end key event
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_handle_endkey(void)
{
	FILE_TRACE("HCCALLUI :: hccall_ui_handle_endkey");
	hccall_net_close();
	DisplayIdleScreen();
}


/********************************************************************
Function:		hccall_ui_exit_downloading
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static void hccall_ui_exit_downloading(void)
{
	//dsmTurnOffBackLight();
	TurnOffBacklight();
}


/********************************************************************
Function:		Hccall_ui_enter_downloadingwnd
Description: 	the downding process window's entrence
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void Hccall_ui_enter_downloadingwnd(void)
{	
	EntryNewScreen(SCR_ID_HCCALL_UPDATE_DOWNLOADING, hccall_ui_exit_downloading, Hccall_ui_enter_downloadingwnd, NULL);

	//dsmTurnOnBackLight();
	TurnOnBacklight(0);

	memset(g_str_buffer, 0, sizeof(g_str_buffer));
	kal_wsprintf((kal_wchar*)g_str_buffer, "%w%d%w", (kal_wchar*)GetString(STR_ID_HCCALL_DOWNLOADING), g_dldpercent, L"%");
		
	ShowCategory165Screen(0, 0, 
					STR_GLOBAL_BACK, IMG_GLOBAL_BACK, 
					(UI_string_type)g_str_buffer, 
					0,
					NULL);

	SetRightSoftkeyFunction(Hccall_Update_Database_Cancle, KEY_EVENT_UP);
	SetKeyHandler(hccall_ui_handle_endkey, KEY_END, KEY_EVENT_DOWN);
}


/********************************************************************
Function:		hccall_ui_enter_connecting
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void hccall_ui_enter_connecting(void)
{
	EntryNewScreen(SCR_ID_HCCALL_UPDATE_CONNECTING, NULL, hccall_ui_enter_connecting, NULL);
	ShowCategory165Screen(0, 0, 
					STR_GLOBAL_BACK, IMG_GLOBAL_BACK, 
					(UI_string_type)GetString(STR_ID_HCCALL_CHECKUPDATE), 
					0,
					NULL);

	SetRightSoftkeyFunction(Hccall_Update_Database_Cancle ,KEY_EVENT_UP);
	SetKeyHandler(hccall_ui_handle_endkey, KEY_END, KEY_EVENT_DOWN);
}


int g_bcheckingupdate = 0;
extern void hccall_ui_enter_gprs_alert(void);

/********************************************************************
Function:		Hccall_Update_Database
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void hccall_ui_enter_findnewwnd(void)
{
	EntryNewScreen(SCR_ID_HCCALL_UPDATE_NEWVERSION, NULL, hccall_ui_enter_findnewwnd, NULL);

#ifndef __HCCALL_DATABASE_FIRM__
	if (FALSE == HcFile_IsExist((int8*)INDEX_DATA))
	{
		ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK, 
					STR_GLOBAL_BACK, IMG_GLOBAL_BACK, 
					(UI_string_type)GetString(STR_ID_HCCALL_NODATABASE), 
					0,
					NULL);
	}
	else
#endif
	{
		ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK, 
					STR_GLOBAL_BACK, IMG_GLOBAL_BACK, 
					(UI_string_type)GetString(STR_ID_HCCALL_NEWVERSION), 
					0,
					NULL);
	}

	SetLeftSoftkeyFunction(hccall_ui_enter_gprs_alert, KEY_EVENT_UP);
	SetRightSoftkeyFunction(GoBackHistory ,KEY_EVENT_UP);

	SetCenterSoftkeyFunction(hccall_ui_enter_gprs_alert, KEY_EVENT_UP);
	ChangeCenterSoftkey(NULL, IMG_GLOBAL_COMMON_CSK);
	
	hccall_ui_autoupdate_flashlastmonth();

	return;
}

/********************************************************************
Function:		hccall_ui_handle_download_evt
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void hccall_ui_handle_download_evt(int event)
{
	if (g_bcheckingupdate == 1)
	{
		if (event == HC_DLD_FINDNEW)
		{
			hccall_net_close();
			FILE_TRACE("hccall_ui_handle_download_evt HC_DLD_FINDNEW \n");

			if(vapp_home_is_active())// (IDLE_SCREEN_ID == GetActiveScreenId())
			{
				FILE_TRACE("hccall_ui_handle_download_evt SCR_ID_IDLE_MAIN \n");
				hccall_ui_enter_findnewwnd();
			}
		}
		else if (event == HC_DLD_ISNEW)
		{
			FILE_TRACE("hccall_ui_handle_download_evt HC_DLD_ISNEW \n");
			hccall_net_close();
		}
		
		return;
	}
	
	if (event == HC_DLD_FINDNEW)
	{
		Hccall_ui_enter_downloadingwnd();
	}
	else if (event == HC_DLD_ISNEW)
	{
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_ISNEW), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);		
	}
	else if (event == HC_DLD_CONERROR)
	{
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_TIMEOUT), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);		
	}
	else if (event == HC_DLD_NETERROR)
	{
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_NETERROR), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);
		DeleteScreenIfPresent(SCR_ID_HCCALL_UPDATE_DOWNLOADING);
	}
	else if (event == HC_DLD_SERVERERROR)
	{
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_SERVERERROR), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);	
	}

	DeleteScreenIfPresent(SCR_ID_HCCALL_UPDATE_CONNECTING);
}



/********************************************************************
Function:		Hccall_Update_Database
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void hccall_ui_start_download(void)
{
	GoBackHistory();

	FILE_TRACE("HCCALLUI :: hccall_ui_start_download start");

//#ifdef __SKYENGINE_V2__
	mr_sim_initialize();
//#else
//	dsmInitSimInfo();
//#endif

	if ((MR_SUCCESS != mr_setting_check_wifi_support(0))
		|| (2 != g_hccall_confg.networkset))
	{
		if (!hccall_check_simcard())
		{
			FILE_TRACE("HCCALLUI :: have no sim card!");
			DisplayPopup((PU8) GetString(STR_ID_HCCALL_INSERTSIM), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);		
			return;
		}
	}
	
	if (!HcCall_CheckSDCard())
	{
		FILE_TRACE("HCCALLUI :: have no sd card");
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_INSERTSD),IMG_GLOBAL_WARNING, 0, UI_POPUP_NOTIFYDURATION_TIME, WARNING_TONE); 
		return;
	}
	
	if (HcCall_CheckUsbIsInMassStorageMode())
	{
		FILE_TRACE("HCCALLUI :: in usb massstorage mode");
#ifndef WIN32
		mmi_usb_app_unavailable_popup(0); 
#endif
		return;
	}
	
	if(HcCall_CheckSDCardNoMemory())
	{
		FILE_TRACE("HCCALLUI :: sd card have no memory");
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_NOMEMORY), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);		
		return;
	}

	FILE_TRACE("HCCALLUI :: start dial");
	g_dldpercent = 0;
	g_bcheckingupdate = 0;
	hccall_ui_enter_connecting();
	hccall_net_dial(16793108);
}


/********************************************************************
Function:		hccall_ui_isneed_checkupdate
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int hccall_ui_isneed_checkupdate(void)
{
	if (g_hccall_netstate != HCCALL_NET_STATE_IDLE)
	{
		FILE_TRACE("hccall_ui_isneed_checkupdate g_hccall_netstate =%d \n",g_hccall_netstate);
		return 0;
	}

	if (g_hccall_confg.autoupdate == 0)
	{
		FILE_TRACE("g_hccall_confg.autoupdate == 0  \n");
		return 0;
	}
	else
	{
		mr_datetime datetime;
		U32 curmonth = 0;
		U32 gapmonth = 0;

		mr_getDatetime(&datetime);
		curmonth = datetime.year*12+datetime.month;
			
		gapmonth = (curmonth >= g_hccall_confg.lastmonth)?(curmonth - g_hccall_confg.lastmonth):(g_hccall_confg.lastmonth-curmonth);
		FILE_TRACE("HCCALLUI :: hccall_ui_isneed_checkupdate :: gapmonth=%d", gapmonth);
		if (gapmonth >= g_hccall_confg.autoupdate ||g_hccall_confg.lastmonth==0)
		{
			return 1;
		}
	}

	return 0;
}


/********************************************************************
Function:		hccall_ui_check_update
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void hccall_ui_check_update(void)
{
	FILE_TRACE("HCCALLUI :: hccall_ui_check_update!");

//#ifdef __SKYENGINE_V2__
	mr_sim_initialize();
//#else
//	dsmInitSimInfo();
//#endif

	if (hccall_ui_isneed_checkupdate() == 0)
	{
		return;
	}

	if ((MR_SUCCESS != mr_setting_check_wifi_support(0))
		|| (2 != g_hccall_confg.networkset))
	{
		if (!hccall_check_simcard())
		{
			FILE_TRACE("HCCALLUI :: have no sim card!");
			return;
		}
	}

	g_bcheckingupdate = 1;
	hccall_net_dial(16793108);
}


/********************************************************************
Function:		hccall_ui_enter_gprs_alert
Description: 	update
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_gprs_alert(void)
{
	EntryNewScreen(SCR_ID_HCCALL_UPDATE_GPRSALERT, NULL, hccall_ui_enter_gprs_alert, NULL);
	ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK, 
						STR_GLOBAL_BACK, IMG_GLOBAL_BACK, 
						(UI_string_type)GetString(STR_ID_HCCALL_TIP), 
						0,
						NULL);

	DeleteScreenIfPresent(SCR_ID_HCCALL_UPDATE_NEWVERSION);

	SetLeftSoftkeyFunction(hccall_ui_start_download,KEY_EVENT_UP);
	SetRightSoftkeyFunction(GoBackHistory ,KEY_EVENT_UP);	

	SetCenterSoftkeyFunction(hccall_ui_start_download, KEY_EVENT_UP);
	ChangeCenterSoftkey(NULL, IMG_GLOBAL_COMMON_CSK);
}

#ifdef __SUPPORT_PBCALL__

void hccall_ui_enter_phb(void)
{
	vapp_contact_launch(NULL, 0);

	return;
}

/********************************************************************
Function:		HighlightHcCallHistoryMenu
Description: 	call history window
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_callhistory(void)
{
	/*
	mmi_mtpnp_entry_menu(SCR_ID_HCCALL_LOC_SEARCH, 
						NULL, 
						hccall_ui_enter_callhistory,
                         			MENU_MTPNP_DM_CALL_HISTORY, 
                         			MAIN_MENU_CALL_HISTORY_TEXT, 
                         			GetRootTitleIcon(MAIN_MENU_CALL_HISTORY));
	*/
	/*
	mmi_clog_launch();
	*/
	
	vapp_clog_launch(NULL, 0);

	return;
}

#endif


/********************************************************************
Function:		hccall_update_auto_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_update_auto_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_update_auto_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		g_hccall_confg.autoupdate = item;
		hccall_save_userconfig(&g_hccall_confg);
		
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE),
					IMG_GLOBAL_ACTIVATED,
					1,
					1000,
					(U8)SUCCESS_TONE);	
		
		mr_menuRelease(menu);
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}


/********************************************************************
Function:		hccall_ui_enter_autoupdate_setting
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_autoupdate_setting(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_BILL);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 4, MR_LIST_SINGLE_SEL, hccall_update_auto_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_UPDATECLOSE);
	hccall_menuSetItem(menu, text, index, (index == g_hccall_confg.autoupdate ? 1 : 0));
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_UPDATE1);
	hccall_menuSetItem(menu, text, index, (index == g_hccall_confg.autoupdate ? 1 : 0));
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_UPDATE2);
	hccall_menuSetItem(menu, text, index, (index == g_hccall_confg.autoupdate ? 1 : 0));
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_UPDATE3);
	hccall_menuSetItem(menu, text, index, (index == g_hccall_confg.autoupdate ? 1 : 0));

	mr_menuShow(menu);

	return;
}


/********************************************************************
Function:		hccall_update_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_update_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_update_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		if (item == 0)
		{
			hccall_ui_autoupdate_flashlastmonth();
			hccall_ui_enter_gprs_alert();
		}
		else
		{
			hccall_ui_enter_autoupdate_setting();
		}
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}


/********************************************************************
Function:		hccall_ui_enter_update_menu
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_update_menu(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_BILL);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 2, MR_LIST_NORMAL_ICON, hccall_update_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_UPDATEMANUAL);
	hccall_menuSetItem(menu, text, index, 0);
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_UPDATEAUTO);
	hccall_menuSetItem(menu, text, index, 0);

	mr_menuShow(menu);

	return;
}


/********************************************************************
Function:		hccall_text_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_text_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{
	mmi_id textid = (mmi_id) handle;
	
	FILE_TRACE("hccall_search_result_event_handler: %d, %d, %d", type, param1, param2);

	if ((type == MR_DIALOG_EVENT) && (param1 == MR_DIALOG_KEY_CANCEL))
	{
		mr_textRelease(textid);
	}
	
	return MR_SUCCESS;
}


/********************************************************************
Function:		hccall_ui_enter_search_result
Description: 	search a number's eara name
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_search_result()
{
	S8*  buffer;
	static S8 newbuffer[200];
	S8* gsdbuffer;
	memset(newbuffer, 0, sizeof(newbuffer));	

	if(mmi_ucs2strlen((const S8 *)g_HCStrGsdLocation) > 0)
	{
		buffer=(S8* )g_HCStrGsdLocation;
		gsdbuffer=GetString(STR_ID_HCCALL_GUISHUDI);
			
		mmi_ucs2cpy(newbuffer,g_HcStr_Number);
		mmi_ucs2cat(newbuffer, gsdbuffer);
		mmi_ucs2cat(newbuffer, buffer);						
	}
	else
	{
		buffer=(S8* )GetString(STR_ID_HCCALL_LOC_SEARCHERROR);//change
			            
		if(mmi_ucs2strlen((const S8 *)g_HcStr_Number) > 0)
		{
			mmi_ucs2cpy(newbuffer,g_HcStr_Number);
			mmi_ucs2cat(newbuffer, buffer);
		}
		else
		{
			mmi_ucs2cpy(newbuffer,buffer);	
		}
	}
				
	hccall_textCreate(GetString(STR_ID_HCCALL_LOC_SEARCH), newbuffer, MR_DIALOG_CANCEL, hccall_text_event_handler);

	return;
}


/********************************************************************
Function:		hccall_ui_start_search
Description: 	search a number's eara name
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_start_search(char *input)
{
	char str_num[HCNUMBER_LEN];
	char str_phNum[7];
	int pos = 0;
	
	memset(str_num, 0, sizeof(str_num));
	memset(str_phNum, 0, sizeof(str_phNum));
	memset(g_HCStrGsdLocation, 0, sizeof(g_HCStrGsdLocation));
	memset(g_HcStr_Number, 0, sizeof(g_HcStr_Number));
	
	if(mmi_ucs2strlen((const S8 *)input) > 0 )
	{
		mmi_ucs2_to_asc((S8 *)str_num, (S8 *)input);
		mmi_ucs2cpy(g_HcStr_Number, input);

		if(strlen((const char*)str_num) > 0)
		{
			if ((strlen((const char*)str_num) >= 2)
				&& (str_num[0]=='8')&&(str_num[1]=='6'))
			{                                     
				pos=2;                           
			}
			else if ((strlen((const char*)str_num) >= 4)
					&& (str_num[0]=='0') && (str_num[1]=='0') && (str_num[2]=='8') && (str_num[3]=='6'))
			{
				pos=4;
			}
			else
			{                    
				pos=0;              
			}
                
			memcpy(str_phNum,str_num+pos,7);  

			GetLocation_ex( g_HCStrGsdLocation,sizeof(g_HCStrGsdLocation),str_phNum );
		}
        
		hccall_ui_enter_search_result();
	}
	else
	{
		DisplayPopup((PU8) GetString(STR_GLOBAL_EMPTY),
					IMG_GLOBAL_EMPTY,
					1,
					1000,
					(U8)WARNING_TONE);
	}
}

/********************************************************************
Function:		hccall_search_input_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_search_input_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id edit = (mmi_id) handle;
	char* input = NULL;
	
	FILE_TRACE("hccall_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_DIALOG_EVENT)
    {
		if (param1 == MR_DIALOG_KEY_OK)
		{
			input = (char*) hccall_editGetText(edit);
			hccall_ui_start_search(input);
		}
		else if (param1 == MR_DIALOG_KEY_CANCEL)
		{
			mr_editRelease(edit);
		}
    }
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		HighlightHcCallHistoryMenu
Description: 	search
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_search_input(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_LOC_SEARCH_TITLE);
	//char *text = (char*)GetString(STR_ID_HCCALL_LOC_SEARCH_TITLE);
	
	(void) hccall_editCreate(title, NULL, MR_EDIT_NUMERIC, 16, hccall_search_input_event_handler);

	return;
}


//////////////////////////////////////////////////////////////////////
#define HCCALL_MAX_NAMELEN	(30)
#define HCCALL_MAX_PROVNUM	(35)

typedef struct TProvCodeStruct
{
	uint16 m_pos;
	uint8 m_childnum;
	uint8 m_namelen;
	char name[HCCALL_MAX_NAMELEN];
}ProvCode;


int g_provinitsuc = 0;
uint8 g_provnum = 0;
ProvCode g_provarr[HCCALL_MAX_PROVNUM];
char g_hclist_str[HCCALL_MAX_PROVNUM][HCCALL_MAX_NAMELEN];


/********************************************************************
Function:		hccall_get_sectiondatabase_md5
Description: 	
Input:		void
Output:		void
Return:		
Notice:
*********************************************************************/
int32 hccall_get_sectiondatabase_md5(char *pbuffer, int32 bufsize)
{
	int32 ret = 0;
	int32 filehandle = 0;
	uint32 readcount = 0;
	uint16 filemode = 0; 
	
	if (pbuffer == NULL)
	{
		return 0;
	}
	memset(pbuffer, 0, bufsize);

	ret = HcFile_Open(&filehandle, (int8*)SETION_DATAFILE, 0, 0);
	if(ret ==  FALSE)
	{
		HcFile_Close(filehandle, 0);

		if(HcFile_Open(&filehandle, (int8*)mr_section, (uint32)sizeof(mr_section), 1))
		{
			filemode=1;
		}
		else
		{
			return FALSE;
		}
	}

	ret = HcFile_Seek(filehandle, 8, PSEEK_SET, filemode);
	if (ret == FALSE)
	{
		HcFile_Close(filehandle, filemode);
		return 0;
	}
	
	
	ret = HcFile_Read(filehandle, pbuffer, bufsize>32?32:bufsize, &readcount, filemode);
	if (ret == FALSE)
	{
		HcFile_Close(filehandle, filemode);
		return 0;
	}
	HcFile_Close(filehandle, filemode);
	
	return 1;
}


/********************************************************************
Function:		hccall_ui_section_table_init
Description: 	init the section table
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
BOOL hccall_ui_section_table_init(void)
{
	int32 fhandle = 0;
	uint32 readcount = 0;
	int32 result = FALSE;
	int index = 0;
	uint16 filemode = 0;//sd card default

	memset(g_provarr, 0, HCCALL_MAX_PROVNUM*sizeof(ProvCode));
	result = HcFile_Open(&fhandle, (int8*)SETION_DATAFILE ,PO_RDONLY, 0);
	if(result ==  FALSE)
	{
		HcFile_Close(fhandle, 0);

#ifdef __HCCALL_DATABASE_FIRM__
		if(HcFile_Open(&fhandle, (int8*)mr_section, (uint32)sizeof(mr_section), 1))
		{
			filemode=1;
		}
		else
		{
			return FALSE;
		}
#else
		return FALSE;
#endif
	}

	result = HcFile_Seek(fhandle, 48, PSEEK_SET, filemode);
	if (result == FALSE)
	{
		HcFile_Close(fhandle, filemode);
		return FALSE;
	}
	result = HcFile_Read(fhandle, &g_provnum, 1, &readcount, filemode);
	if (result == FALSE)
	{
		HcFile_Close(fhandle, filemode);
		return FALSE;
	}
	FILE_TRACE("HCCALLIN_UI :: hccall_ui_section_table_init--------g_provnum=%d", g_provnum);

	if (g_provnum>HCCALL_MAX_PROVNUM)
	{
		g_provnum = HCCALL_MAX_PROVNUM;
	}

	for (index=0; index<g_provnum; index++)
	{
		memset(&g_provarr[index], 0, sizeof(g_provarr[index]));
		result = HcFile_Read(fhandle, &g_provarr[index].m_pos, 2, &readcount, filemode);
		if (result == FALSE)
		{
			HcFile_Close(fhandle, filemode);
			return FALSE;
		}
		
		result = HcFile_Read(fhandle, &g_provarr[index].m_childnum, 1, &readcount, filemode);
		if (result == FALSE)
		{
			HcFile_Close(fhandle, filemode);
			return FALSE;
		}
		result = HcFile_Read(fhandle, &g_provarr[index].m_namelen, 1, &readcount, filemode);
		if (result == FALSE)
		{
			HcFile_Close(fhandle, filemode);
			return FALSE;
		}
		result = HcFile_Read(fhandle, g_provarr[index].name, g_provarr[index].m_namelen, &readcount, filemode);
		if (result == FALSE)
		{
			HcFile_Close(fhandle, filemode);
			return FALSE;
		}
	}

	HcFile_Close(fhandle, filemode);	
	return TRUE;
}


/********************************************************************
Function:		hccall_ui_get_sectionlist
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
BOOL hccall_ui_get_sectionlist(U8 *list_of_items[],S32 * number_of_items, int ID)
{
	int32 fhandle = 0;
	uint32 readcount = 0;
	int32 result = FALSE;
	int index = 0;
	uint16 filemode = 0;//sd card default

	FILE_TRACE("HCCALLIN_UI :: hccall_ui_get_sectionlist--------ID=%d", ID);

	for (index=0; index<HCCALL_MAX_PROVNUM; index++)
	{
		memset(g_hclist_str[index], 0, HCCALL_MAX_NAMELEN);
	}

	if (ID == 0)
	{
		for (index=0; index<g_provnum; index++)
		{
			memcpy(g_hclist_str[index], g_provarr[index].name, HCCALL_MAX_NAMELEN);		
			list_of_items[index] = (U8 *)g_hclist_str[index];
		}
		*number_of_items = g_provnum;
	}
	else if (ID>0)
	{
		int childtotal = 0;
		int childnum = 0;
		
		result = HcFile_Open(&fhandle, (int8*)SETION_DATAFILE ,PO_RDONLY, 0);
		if(result ==  FALSE)
		{
			HcFile_Close(fhandle, 0);

#ifdef __HCCALL_DATABASE_FIRM__
			if(HcFile_Open(&fhandle, (int8*)mr_section, (uint32)sizeof(mr_section), 1))
			{
				filemode=1;
			}
			else
			{
				return FALSE;
			}
#else
			return FALSE;
#endif
		}
	
		result = HcFile_Seek(fhandle, 48+g_provarr[ID-1].m_pos, PSEEK_SET, filemode);	
		if (result == FALSE)
		{
			HcFile_Close(fhandle, filemode);
			return FALSE;
		}

		childtotal = (g_provarr[ID-1].m_childnum<HCCALL_MAX_PROVNUM)?g_provarr[ID-1].m_childnum : HCCALL_MAX_PROVNUM;
		
		for (index=0; index<childtotal; index++)
		{
			uint8 namelen = 0;
			
			result = HcFile_Read(fhandle, &namelen, 1, &readcount, filemode);	
			if (result == FALSE)
			{
				HcFile_Close(fhandle, filemode);
				return FALSE;
			}
			result = HcFile_Read(fhandle, &g_hclist_str[index], namelen, &readcount, filemode);
			if (result == FALSE)
			{
				HcFile_Close(fhandle, filemode);
				return FALSE;
			}

			list_of_items[index] = (U8 *)g_hclist_str[index];
			childnum++;
		}

		*number_of_items = childnum;

		HcFile_Close(fhandle, filemode);	
	}

	FILE_TRACE("HCCALLIN_UI :: hccall_ui_get_sectionlist--------end");

	return TRUE;
}

/********************************************************************
Function:		hccall_section_province_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_section_province_detail_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	
	FILE_TRACE("hccall_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_ui_enter_section_province_detail
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_section_province_detail(int32 item)
{
	U8 *guiBuffer;
	S32 number_of_items = 0;
	U8 *list_of_items[31];	
	BOOL result = FALSE;
	int32 menu = 0;
	char *title = (char*)GetString(STR_ID_HCCALL_DOMESTIC_SECTION);
	uint32 i = 0;
	
	result = hccall_ui_get_sectionlist(list_of_items, &number_of_items, item + 1);
 	if (result)
	{
 		menu = hccall_menuCreate(title, number_of_items, MR_LIST_NORMAL_ICON, hccall_section_province_detail_event_handler);
		
		for (i=0; i<number_of_items; i++)
		{
			hccall_menuSetItem(menu, list_of_items[i], i, 0);
		}
		
		mr_menuShow(menu);
	}
	else
	{      
		DisplayPopup ((PU8)GetString(STR_ID_HCCALL_GETCODEFAIL), IMG_GLOBAL_WARNING, 1, 2000, (U8)ERROR_TONE);
	}

	return;
}

/********************************************************************
Function:		hccall_section_municipality_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_section_municipality_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	
	FILE_TRACE("hccall_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_ui_enter_section_municipality
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_section_municipality(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_DOMESTIC_SECTION);
	U32 i = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 4, MR_LIST_NORMAL_ICON, hccall_section_municipality_event_handler);

	for(i=0; i<4; i++)
	{
		text = (char*)GetString(STR_ID_HCCALL_FC_BEIJING + i);
		hccall_menuSetItem(menu, text, i, 0);
	}

	mr_menuShow(menu);

	return;
}


/********************************************************************
Function:		hccall_section_province_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_section_province_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_section_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		hccall_ui_enter_section_province_detail(item);
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_ui_enter_section_province
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_section_province(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_DOMESTIC_SECTION);
	uint32 i = 0;
	int32 menu = 0;
	S32 number_of_items=0;
	U8 *list_of_items[HCCALL_MAX_PROVNUM];
	BOOL result = FALSE;
	
	if (FALSE == hccall_ui_section_table_init())
	{
		DisplayPopup ((PU8)GetString(STR_ID_HCCALL_GETCODEFAIL), 
					IMG_GLOBAL_WARNING, 
					1, 
					2000, 
					(U8)ERROR_TONE);
		return;
	}

	result = hccall_ui_get_sectionlist(list_of_items,&number_of_items, 0);
	if (result)
	{
		if (number_of_items > 30)
		{
			number_of_items = 30;
		}

 		menu = hccall_menuCreate(title, number_of_items, MR_LIST_NORMAL_ICON, hccall_section_province_event_handler);
		
		for (i=0; i<number_of_items; i++)
		{
			hccall_menuSetItem(menu, list_of_items[i], i, 0);
		}
		
		mr_menuShow(menu);
	}
	else
	{
		DisplayPopup ((PU8)GetString(STR_ID_HCCALL_GETCODEFAIL), IMG_GLOBAL_WARNING, 1, 2000, (U8)ERROR_TONE);
	}

	return;
}


/********************************************************************
Function:		hccall_section_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_section_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_section_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		if (item == 0)
		{
			hccall_ui_enter_section_municipality();
		}
		else
		{
			hccall_ui_enter_section_province();
		}
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_ui_enter_section
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_section(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_SECTION_NUM);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 2, MR_LIST_NORMAL_ICON, hccall_section_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_MUNICIPALITES);
	hccall_menuSetItem(menu, text, index, 0);
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_PROVINCE);
	hccall_menuSetItem(menu, text, index, 0);

	mr_menuShow(menu);

	return;
}

static int32 hccall_common_usenum_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	
	FILE_TRACE("hccall_common_usenum_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_ui_enter_common_usenum
Description: 	common use number
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_common_usenum(void)
{
	#define TOT_SIZE_OF_NUM_LIST  30
    
	U32 i=0;
	char *title = (char*)GetString(STR_ID_HCCALL_INCOMMON_NUM);
	char *text = NULL;
	int32 menu = 0;

	menu = hccall_menuCreate(title, TOT_SIZE_OF_NUM_LIST, MR_LIST_NORMAL_ICON, hccall_common_usenum_event_handler);
    
	for(i=0; i<TOT_SIZE_OF_NUM_LIST; i++)
	{
		text = (char*)GetString(STR_ID_HCCALL_CC_IML + i);
		hccall_menuSetItem(menu, text, i, 0);
	}

	mr_menuShow(menu);

	return;
}


/********************************************************************
Function:		hccall_help_item_event_handler
Description: 	help item event handler
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_help_item_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{
	mmi_id textid = (mmi_id) handle;
	
	FILE_TRACE("hccall_help_item_event_handler: %d, %d, %d", type, param1, param2);

	if ((type == MR_DIALOG_EVENT) && (param1 == MR_DIALOG_KEY_OK))
	{
		mr_textRelease(textid);
	}
	
	return MR_SUCCESS;
}



/********************************************************************
Function:		hccall_help_event_handler
Description: 	help menu event handler
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_help_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{
	S8*  buffer;
	U16  str_id;
	U16  str_info_id;
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_help_event_handler: %d, %d, %d", type, param1, param2);

    if (type == MR_MENU_SELECT)
	{
		switch(item)
		{
		case 0:
			str_id=STR_ID_HCCALL_MANUAL;
			str_info_id=STR_ID_HCCALL_MANUAL_INFOR;
			break;
			
		case 1:
			str_id=STR_ID_HCCALL_PAY;
			str_info_id=STR_ID_HCCALL_PAY_INFOR;
			break;
			
		case 2:
			str_id=STR_ID_HCCALL_ABOUT;
			str_info_id=STR_ID_HCCALL_ABOUT_INFOR;
			break;
			
		default:
			str_id=STR_ID_HCCALL_ABOUT;
			str_info_id=STR_ID_HCCALL_ABOUT_INFOR;
			break;
		}

		buffer=(S8* )GetString(str_info_id);

		hccall_textCreate(GetString(str_id), buffer, MR_DIALOG_CANCEL, hccall_text_event_handler);
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}



/********************************************************************
Function:		hccall_ui_enter_help
Description: 	help
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_help_menu(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_HELP);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 3, MR_LIST_NORMAL_ICON, hccall_help_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_MANUAL);
	hccall_menuSetItem(menu, text, index, 0);
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_PAY);
	hccall_menuSetItem(menu, text, index, 0);
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_ABOUT);
	hccall_menuSetItem(menu, text, index, 0);

	mr_menuShow(menu);

	return;
}


extern T_DSM_DUALSIM_SET dsmDualSim;

/********************************************************************
Function:		hccall_network_setting_event_handler
Description: 	save network settings
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_network_setting_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_network_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		g_hccall_confg.networkset = item;
		
		hccall_save_userconfig(&g_hccall_confg);
		
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE),
					IMG_GLOBAL_ACTIVATED,
					1,
					1000,
					(U8)SUCCESS_TONE);	
		
		mr_menuRelease(menu);
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_sim_setting_event_handler
Description: 	save sim settings
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_sim_setting_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_sim_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		g_hccall_confg.CurrSimId = dsmDualSim.simId[item];
		
		hccall_save_userconfig(&g_hccall_confg);
		
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE),
					IMG_GLOBAL_ACTIVATED,
					1,
					1000,
					(U8)SUCCESS_TONE);	
		
		mr_menuRelease(menu);
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_function_setting_event_handler
Description: 	save function settings
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_function_setting_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_function_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		g_hccall_confg.functionset = item;
		
		hccall_save_userconfig(&g_hccall_confg);
		
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE),
					IMG_GLOBAL_ACTIVATED,
					1,
					1000,
					(U8)SUCCESS_TONE);	
		
		mr_menuRelease(menu);
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}

/********************************************************************
Function:		hccall_ui_enter_network_setting
Description: 	select cmnet or cmwap
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_network_setting(void)
{
	U8 nNumofItem = 3;
	U16 highlighted_item = g_hccall_confg.networkset;
	char *title = (char*)GetString(STR_ID_HCCALL_NETWORK_SETTING);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;

	if (MR_SUCCESS != mr_setting_check_wifi_support(0))
	{
		nNumofItem = 2;
		
		if (highlighted_item > 1)
		{
			highlighted_item = 0;
		}
 	}
 
	menu = hccall_menuCreate(title, nNumofItem, MR_LIST_SINGLE_SEL, hccall_network_setting_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_NETWORK_SETTING_CMNET);
	hccall_menuSetItem(menu, text, index, (index == highlighted_item ? 1 : 0));
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_NETWORK_SETTING_CMWAP);
	hccall_menuSetItem(menu, text, index, (index == highlighted_item ? 1 : 0));

	if (3 == nNumofItem)
	{
		index ++;
		text = (char*)GetString(STR_ID_HCCALL_NETWORK_SETTING_WIFI);
		hccall_menuSetItem(menu, text, index, (index == highlighted_item ? 1 : 0));
	}

	mr_menuShow(menu);

	return;
}

/********************************************************************
Function:		hccall_ui_enter_sim_setting
Description: 	select the sim card for network
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_sim_setting(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_SIM_SETTING);
	char *text = NULL;
	int32 menu = 0;
	U8 nNumofItem = 0;
	int32 i;
	int32 simId = 0;
	int32 select = 0;
	
	mr_sim_initialize();
	
	nNumofItem = dsmDualSim.totalNum;
	if (0 == nNumofItem)
	{
		FILE_TRACE("HCCALLUI :: have no sim card!");
		DisplayPopup((PU8) GetString(STR_ID_HCCALL_INSERTSIM), IMG_GLOBAL_WARNING,  1,  UI_POPUP_NOTIFYDURATION_TIME,  0);	

		return;
	}
	
	menu = hccall_menuCreate(title, nNumofItem, MR_LIST_SINGLE_SEL, hccall_sim_setting_event_handler);

	for (i=0; i<nNumofItem; i++)
	{
		simId = dsmDualSim.simId[i];

		if (simId == g_hccall_confg.CurrSimId)
		{
			select = 1;
		}
		else
		{
			select = 0;
		}
		
		if (DSM_SLAVE_SIM == simId)
		{
			text = (char*)GetString(STR_ID_HCCALL_SIM_SETTING_CARD2);
		}
#ifdef __MR_CFG_MULTI_SIM_CARD__
#if defined(__RAGENT_SIM3_SUPPORT__)
		else if (DSM_THIRD_SIM == simId)
		{
			text = (char*)GetString(STR_ID_HCCALL_SIM_SETTING_CARD3);
		}
#endif
#if defined(__RAGENT_SIM4_SUPPORT__)
		else if (DSM_FOURTH_SIM == simId)
		{
			text = (char*)GetString(STR_ID_HCCALL_SIM_SETTING_CARD4);
		}
#endif
#endif
		else
		{
			text = (char*)GetString(STR_ID_HCCALL_SIM_SETTING_CARD1);
		}

		hccall_menuSetItem(menu, text, i, select);
	}

	mr_menuShow(menu);

	return;
}

/********************************************************************
Function:		hccall_ui_enter_func_setting
Description: 	open or close the hccall function
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_func_setting(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_FUNCTION_SETTING);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 2, MR_LIST_SINGLE_SEL, hccall_function_setting_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_FUNCTION_SETTING_ON);
	hccall_menuSetItem(menu, text, index, (index == g_hccall_confg.functionset ? 1 : 0));
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_FUNCTION_SETTING_OFF);
	hccall_menuSetItem(menu, text, index, (index == g_hccall_confg.functionset ? 1 : 0));

	mr_menuShow(menu);

	return;
}

/********************************************************************
Function:		hccall_setting_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_setting_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_setting_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
		if (item == 0)
		{
			hccall_ui_enter_network_setting();
		}
		else if (item == 1)
		{
			hccall_ui_enter_sim_setting();
		}
		else
		{
			hccall_ui_enter_func_setting();
		}
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}


/********************************************************************
Function:		hccall_ui_enter_setting
Description: 	enter hccall setting
Input:			void
Output:			void
Return:			void
Notice:
*********************************************************************/
void hccall_ui_enter_setting(void)
{
	char *title = (char*)GetString(STR_ID_HCCALL_SETTING);
	int32 index = 0;
	char *text = NULL;
	int32 menu = 0;
	
	menu = hccall_menuCreate(title, 3, MR_LIST_NORMAL_ICON, hccall_setting_event_handler);

	text = (char*)GetString(STR_ID_HCCALL_NETWORK_SETTING);
	hccall_menuSetItem(menu, text, index, 0);
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_SIM_SETTING);
	hccall_menuSetItem(menu, text, index, 0);
	index ++;

	text = (char*)GetString(STR_ID_HCCALL_FUNCTION_SETTING);
	hccall_menuSetItem(menu, text, index, 0);

	mr_menuShow(menu);

	return;
}


/********************************************************************
Function:		hccall_ui_init_globalvalue
Description: 	init the global values
Input:		void
Output:		void
Return:		void
Notice:		
*********************************************************************/
void hccall_init_globalvalue(void)
{
	int i = 0;
	int ret = 0;
    
	ret = hccall_load_userconfig(&g_hccall_confg);
	if(ret == 0)
	{		
		g_hccall_confg.networkset = 0;
		g_hccall_confg.CurrSimId = DSM_MASTER_SIM;
		g_hccall_confg.functionset = 0;	
		g_hccall_confg.autoupdate = HCCALL_DEFAULT_AUTOUPDATE;
		g_hccall_confg.lastmonth = 0;
        
		hccall_save_userconfig(&g_hccall_confg);        
	}

	FILE_TRACE("hccall_init_globalvalue :: networkset=%d, CurrSimId=%d, functionset=%d, autoupdate=%d, lastmonth=%d", 
				g_hccall_confg.networkset,
				g_hccall_confg.CurrSimId,
				g_hccall_confg.functionset, 
				g_hccall_confg.autoupdate, 
				g_hccall_confg.lastmonth);

	hccall_set_firmdatabase((const unsigned char*)mr_ldzs, (uint32)sizeof(mr_ldzs));
	LocationData_Init_ex();
}


/********************************************************************
Function:		hccall_ui_enter_funny
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
void hccall_ui_enter_funny()
{
	//mr_connectWAP("http://wap.51mrp.com");
	DsmGame();
}


/********************************************************************
Function:		hccall_mainmenu_event_handler
Description: 	
Input:		void
Output:		void
Return:		void
Notice:
*********************************************************************/
static int32 hccall_mainmenu_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{	   
	mmi_id menu = (mmi_id) handle;
	int32 item = param1;
	
	FILE_TRACE("hccall_mainmenu_event_handler: %d, %d, %d", type, param1, param2);
	
    if (type == MR_MENU_SELECT)
	{
#ifdef __SUPPORT_PBCALL__
		if (item == 0)
		{
			hccall_ui_enter_phb();
		}
		else if (item == 1)
		{
			hccall_ui_enter_callhistory();
		}
		else
		{
			item -= 2;
#endif
			if (item == 0)
			{
				hccall_ui_enter_search_input();
			}
			else if (item == 1)
			{
				hccall_ui_enter_section();
			}
			else if (item == 2)
			{
				hccall_ui_enter_common_usenum();
			}
			else if (item == 3)
			{
				hccall_ui_enter_update_menu();
			}
			else if (item == 4)
			{
				hccall_ui_enter_setting();
			}
			else if (item == 5)
			{
				hccall_ui_enter_funny();
			}
			else
			{
				hccall_ui_enter_help_menu();
			}
#ifdef __SUPPORT_PBCALL__
		}
#endif
	}
	else if (type == MR_MENU_RETURN)
	{
		mr_menuRelease(menu);
	}
	
	return MR_SUCCESS;
}


/********************************************************************
Function:		hccall_ui_enter_mainmenu
Description: 	main menu window
Input:		void
Output:		void
Return:		void
Notice:		void
*********************************************************************/
void hccall_ui_enter_mainmenu(void)
{
	U16 nNumofItem = 0; 
	U16 nStrItemList[MAX_SUB_MENUS];
	char *title = (char*)GetString(STR_ID_HCCALL);
	int32 i = 0;
	char *text = NULL;
	int32 menu = 0;
	
	nNumofItem = GetNumOfChild(MENU_ID_HCCALL);
	GetSequenceStringIds(MENU_ID_HCCALL,nStrItemList);
	
	menu = hccall_menuCreate(title, nNumofItem, MR_LIST_NORMAL_ICON, hccall_mainmenu_event_handler);

	for (i=0; i<nNumofItem; i++)
	{
		text = (char*)GetString(nStrItemList[i]);
		hccall_menuSetItem(menu, text, i, 0);
	}

	mr_menuShow(menu);

	return;
}


/********************************************************************
Function:		vapp_hccall_launch
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void vapp_hccall_launch(void)
{
	HcFile_CreateHccallDir();
	
	hccall_ui_enter_mainmenu();

	if (g_hccall_netstate != HCCALL_NET_STATE_IDLE)
	{
		hccall_net_close();
	}

#ifndef __HCCALL_DATABASE_FIRM__
	if (FALSE == HcFile_IsExist((int8*)INDEX_DATA))
	{
		hccall_ui_enter_findnewwnd();
	}
#endif
}


/********************************************************************
Function:		InitHcCallApp
Description: 	the entrence function of the hccall application
Input:		void
Output:		void
Return:		void
Notice:		this function is called when the phone is powered on
*********************************************************************/
void InitHcCallApp(void)
{
	FILE_TRACE("HCCALL :: InitHcCallApp");
	
	HcFile_CreateHccallDir();

	gui_start_timer(1000, hccall_init_globalvalue);
//	gui_start_timer(60*1000, hccall_ui_check_update);
}


#define ELSE else
#define IF_STRCMP(des,des_len) 	if(strncmp((const char*)g_HCNumber+pos, des, des_len) == 0) \
{ \
pos += des_len; \
} \


/********************************************************************
Function:		HcCall_hist_populate_call_list
Description: 	get the city's name from a phone number
Input:		city_number: cellphone or telephone number	
Output:		city_name: the city name
Return:		the length of the city's name
Notice:
*********************************************************************/
int HcCall_hist_populate_call_list(char *city_name, int city_name_size, char *city_number)
{
	int ret_len=0;

	FILE_TRACE("HCCALLUI :: HcCall_hist_populate_call_list-----start");

	if (city_name==NULL || city_number==NULL)
	{
		return 0;
	}

	FILE_TRACE("HCCALLUI :: HcCall_hist_populate_call_list-----functionset=%d, city_number=%s", g_hccall_confg.functionset, city_number);

	memset(city_name, 0, city_name_size);
	if(0 == g_hccall_confg.functionset)
	{
		int pos = 0;
		int len = 0;

       	memset(g_HCNumber, 0, sizeof(g_HCNumber));
		memset(g_HCStrLocation,0,sizeof(g_HCStrLocation));
   		
		if( strlen( ( const char * )city_number) > 10 && strlen( ( const char * )city_number ) < 62 )
		{
			strcpy(g_HCNumber, ( const char * )city_number);
			
			IF_STRCMP("17951",5)
			ELSE IF_STRCMP("12593",5)
			ELSE IF_STRCMP("17969",5)
			ELSE IF_STRCMP("17909",5)
				
			IF_STRCMP("+86",3)
			IF_STRCMP("0086",4)
				
			len = GetLocation_ex( g_HCStrLocation,sizeof(g_HCStrLocation),g_HCNumber + pos );
			FILE_TRACE("HCCALLUI :: HcCall_hist_populate_call_list----len=%d", len);
			if(len > 0 )
			{			
				mmi_ucs2cat((S8 *)city_name , (const S8 *) L"~~");
				mmi_ucs2ncat((S8 *)city_name , (const S8 *)g_HCStrLocation, city_name_size-4);
				ret_len = mmi_ucs2strlen((const S8 *)city_name);
			}
		}
	}

	FILE_TRACE("HCCALLUI :: HcCall_hist_populate_call_list----ret_len=%d", ret_len);
	return ret_len;
}

e_unet_sim_type hccall_get_simId(void)
{
	int32 DsmSimId = dsmDualSim.simId[0];
	uint8 nNumofItem = dsmDualSim.totalNum;
	uint8 i = 0;

	/* For first bootup or sim pulled out, the current sim may not be exists */
	for (i=0; i<nNumofItem; i++)
	{
		if (g_hccall_confg.CurrSimId == dsmDualSim.simId[i])
		{
			DsmSimId = dsmDualSim.simId[i];
			break;
		}
	}
	
	if (DSM_SLAVE_SIM == DsmSimId)
	{
		return UNET_SIM_SLAVE;
	}
#ifdef __MR_CFG_MULTI_SIM_CARD__
#if defined(__RAGENT_SIM3_SUPPORT__)
	else if (DSM_THIRD_SIM == DsmSimId)
	{
		return UNET_SIM_THIRD;
	}
#endif
#if defined(__RAGENT_SIM4_SUPPORT__)
	else if (DSM_FOURTH_SIM == DsmSimId)
	{
		return UNET_SIM_FOURTH;
	}
#endif
#endif
	else
	{
		return UNET_SIM_MASTER;
	}
}

void hccall_start_host_timer(U32 delay, FuncPtr funcPtr)
{
	StartTimer(HCCALL_UI_TIMER, delay, funcPtr);

	return;
}

void hccall_stop_host_timer(void)
{
	StopTimer(HCCALL_UI_TIMER);

	return;
}


#endif // __HC_CALL_NEW__


