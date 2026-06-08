/*****************************************************************************
** File Name:      mmidropdownwin_main.c                                   *
** Author:                                                                   *
** Date:           05/29/2011                                                *
** Copyright:      2011 Spreadtrum, Incoporated. All Rights Reserved.        *
** Description:    This file is used to describe punctuation                 *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 05/2011      Paul.Huang              Creat
******************************************************************************/

#define _MMI_DROPDOWNWIN_WINTAB_C_

#include "mmi_app_dropdownwin_trc.h"
#ifdef PDA_UI_DROPDOWN_WIN

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmk_app.h"
#include "ui_layer.h"
#include "mmi_position.h"
#include "window_parse.h"
#include "guilistbox.h"
#include "guistatusbar.h"
#include "guires.h"
#include "mmidisplay_data.h"
#include "mmidropdownwin_nv.h"
#include "mmidropdownwin_image.h"
#include "mmidropdownwin_export.h"
#include "mmidropdown_position.h"
#include "mmidropdownwin_id.h"
#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif
#ifdef BLUETOOTH_SUPPORT
#include "mmibt_export.h"
#include "mmibt_wintab.h"   
#endif
#ifdef FM_SUPPORT
//#include "mmifm_internal.h"
#include "mmifm_export.h"
#include "mmifm_text.h"
#include "mmifm_internal.h"
#endif
#include "mmimms_export.h"
#include "mmiset_display.h"
//#include "mmienvset.h"
#include "mmienvset_export.h"
#if (defined MMIWIDGET_SUPPORT) || (defined MMI_GRID_IDLE_SUPPORT) || (defined WEATHER_SUPPORT)
#include "mmiwidget_image.h"
#endif
#include "mmi_text.h"
#include "mmiset_text.h"
#include "mmicc_text.h"
#include "mmicc_image.h"
#include "mmiset_func.h"
#include "mmisms_text.h"
#include "mmisms_export.h"
#include "mmimp3_text.h"
#ifdef JAVA_SUPPORT
#include "java_mmi_text.h"
#endif
#include "mmiidle_export.h" 
#include "mmicl_internal.h"    
#include "Mmisms_app.h"
#include "mmisms_read.h"
#include "mmk_tp.h"
#include "mmimp3_export.h"
#include "mmidropdownwin_text.h"
#include "mmiacc_text.h"
#include "mmicountedtime_export.h"
#include "mmipub.h"
#include "mmiudisk_export.h"
#include "mmisd_export.h"
#include "mmifmm_id.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_id.h"
#endif
#ifdef ATV_SUPPORT
#include "mmiatv_id.h"
#include "mmiatv_export.h"
#endif
#include "mmiota_image.h"
#include "mmiota_text.h"
#ifdef MMS_SUPPORT
#include "mmimms_text.h"
#endif
#ifdef ASP_SUPPORT
#include "mmiasp_cortrol_layer.h"
#include "mmiasp_export.h"
#include "mmiasp_wintab.h"
#endif
#include "usbservice_api.h"
#include "mmikl_export.h"
#include "guictrl_api.h"

#ifdef MMI_PDA_SUPPORT
#include "mmi_timer_export.h"
#endif
#include "mmiidle_func.h"
#include "mmistk_export.h"

#include "mmimp3_id.h"
#ifdef VIDEO_PLAYER_SUPPORT 
#include "mmivp_export.h"
#endif
#include "mmiidle_statusbar.h"
#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#ifdef __MMI_SKYQQ__
#include "sky_qq_open.h"
#endif
#endif
#ifdef MCARE_V31_SUPPORT
#include "McfInterface.h"
#include "McfIdle.h"
#endif
#include "mmicc_image.h"

#ifdef MMI_GPRS_SWITCH_SUPPORT
#include "mmiconnection_export.h"
#endif
#ifdef MMI_VIDEOWALLPAPER_SUPPORT 
#include "mmiset_videowallpaper.h"
#endif

#if defined(MMI_SMSCB_SUPPORT)
#include "mmismscb_export.h"
#endif

#ifdef MMI_POWER_SAVING_MODE_SUPPORT
#include "mmiset_id.h"
#endif

#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
	#include "mmiwre_adaptor.h"
	extern void MMIWRE_MaxmizeAPP(void*);
	#define WRE_APP_TITLE_LEN	40
	#define WRE_APP_STRING_LEN	40
	unsigned short wre_app_title[WRE_APP_RUN_MAX_NUM][WRE_APP_TITLE_LEN+1];
	unsigned short wre_app_string[WRE_APP_RUN_MAX_NUM][WRE_APP_STRING_LEN+1];

 /*****************************************************************************/
 //  Description : to tell whether the specified wre app(appIdx) is running or not
 //  Global resource dependence : none
 //  Author: david.li
 //  Note: 
 /*****************************************************************************/
BOOLEAN IsWreAppRunning(uint32 appId)
{
	return TRUE;
}

 /*****************************************************************************/
 //  Description : get wre app ID by index.
 //  Global resource dependence : none
 //  Author: david.li
 //  Note: 
 /*****************************************************************************/
int32 getRunningWreAppIdByIdx(int32 appIdx)
{
	int32 appId;

	appId = appIdx;
	return appId;
}
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define WIN_CTRL_ID(win_id, ctr_id)   (ctr_id | ((win_id << 8) &0xFF00))
#define MMI_DISPLAY_TIME_LEN    9
#define MMI_COUNT_STR_LEN    10
#define MMI_NOTIFY_TITLE_LEN    50
//#define MMIDROPDOWN_TPMOVE_REDRAW_TIME      30      //为支持TP连续移动而加的timer 

//#define MMIWIDGET_AUTOANSWER_LOG_TIME_LEN           9

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
LOCAL MMIDROPDOWN_T s_dropdown = {0};
//LOCAL MMIDROPDOWN_SHORTCUT_ITEM_T s_shortcut_item_t[DROPDOWN_SHORTCUT_MAX] = {0};
LOCAL MMIDROPDOWN_SHORTCUT_T s_shortcut_data = {0};
LOCAL MMIDROPDOWN_NOTIFY_ITEM_T s_notify_list[DROPDOWN_NOTIFY_MAX] = {0};
//LOCAL uint8    s_noisy_pre_mode = MMIENVSET_STANDARD_MODE;
//LOCAL uint8    s_silent_pre_mode = MMIENVSET_MEETING_MODE;
LOCAL wchar    s_misscall_title[MMI_NOTIFY_TITLE_LEN + 1];
LOCAL wchar    s_newmessage_title[MMI_NOTIFY_TITLE_LEN + 1];
LOCAL wchar    s_mp3_name[MMIMP3_NAME_MAX_LEN + 1];
LOCAL wchar    s_mp3_artist[MMIMP3_COMMON_TAG_LEN +1] ={0}; /*cr127056 renwei add*/
LOCAL wchar    s_fm_name[MMIMP3_NAME_MAX_LEN + 1];
LOCAL wchar    s_misscall_name[PHONE_NUM_MAX_LEN + 1];
LOCAL wchar    s_misscall_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_newmessage_name[MMISMS_PBNAME_MAX_LEN + 1];
LOCAL wchar    s_newmessage_time[MMI_DISPLAY_TIME_LEN + 1];
#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
LOCAL wchar    s_newpush_time[MMI_DISPLAY_TIME_LEN + 1];
#endif
LOCAL wchar    s_messagefull_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_java_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_qq_time[MMI_DISPLAY_TIME_LEN + 1];
//nicklei add
#ifdef MCARE_V31_SUPPORT
LOCAL wchar    s_mcare_qq_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_mcare_qc_time[MMI_DISPLAY_TIME_LEN + 1];
#endif
LOCAL wchar    s_player_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_fm_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_countedtime_value[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_newasp_title[MMI_NOTIFY_TITLE_LEN + 1];
LOCAL wchar    s_newasp_name[MMI_NOTIFY_TITLE_LEN + 1];
LOCAL wchar    s_newasp_time[MMI_DISPLAY_TIME_LEN + 1];
LOCAL wchar    s_calling_name[MMI_NOTIFY_TITLE_LEN + 1];
LOCAL DROPDOWN_PAGE_E s_cur_page = {0};

/*---------------------------------------------------------------------------*/
/*                         TYPE AND CONSTANT                                 */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
//  Description : 开始触笔抬起后的滑动
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StartFling(
                      MMI_WIN_ID_T        win_id
                      );

/*****************************************************************************/
//  Description : 结束触笔抬起后的滑动
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopFling(
                     MMI_WIN_ID_T        win_id
                     );

/*****************************************************************************/
//  Description : Display dropdown bg
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayDropDownScrollBar(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : calc page button rect
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcPageButtonRect(void);

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void HandleListCheck(
                           MMI_WIN_ID_T        win_id, 
                           uint32 user_data,
                           GUILIST_NEED_ITEM_CONTENT_T *need_item_content_ptr
                           );

/*****************************************************************************/
//  Description : fmradio Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopFMRadio(
                         MMI_WIN_ID_T        win_id
                         );

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CheckNeedUpdateTimerItem(
                                       DROPDOWN_NOTIFY_E notify_type                                       
                                       );

/*****************************************************************************/
//  Description : Start TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StartUpdateRedrawTimer(MMI_WIN_ID_T    win_id );

/*****************************************************************************/
//  Description : Stop TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopUpdateRedrawTimer(void);

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutIconRect(
                                      MMI_HANDLE_T  win_id,
                                      uint32 index
                                      );

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutTotalRect(
                                       MMI_HANDLE_T  win_id
                                       );

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutItemRect(
                                      MMI_HANDLE_T  win_id,
                                      uint32 index
                                      );

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutBarRect(
                                     MMI_HANDLE_T  win_id,
                                     uint32 index
                                     );

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutSplitLineRect(
                                     MMI_HANDLE_T  win_id,
                                     uint32 index
                                     );

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsBelongCurPage(
                              DROPDOWN_NOTIFY_E 	notify_type,
                              DROPDOWN_PAGE_E  page
                              );

/*****************************************************************************/
//  Description : 重新加载page内容
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void ReloadPageContent(
                           MMI_WIN_ID_T    win_id
                           );

/*****************************************************************************/
//  Description : Handle dropdown win slide
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void HandlePageButtonPressed(
                                   MMI_WIN_ID_T    win_id,
                                   DROPDOWN_PAGE_E page_index
                                   );

/*****************************************************************************/
//  Description : Display dropdown bg
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayPageButton(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : set dropdown layer pos
//  Global resource dependence : 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
LOCAL void SetDropdownLayerPos(
                               GUI_LCD_DEV_INFO  *dev_info_ptr,
                               int16 x,
                               int16 y
                               );

/*****************************************************************************/
//  Description : Get List Box Rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL GUI_BOTH_RECT_T GetDropDownNotifyListBoxRect(void);

/*****************************************************************************/
//  Description : Get List Box Rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL GUI_BOTH_RECT_T GetDropDownRunningListBoxRect(void);

/*****************************************************************************/
//  Description : Create List Box Ctrl
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateNotifyListBox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T list_id);

/*****************************************************************************/
//  Description : Check notify list
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CheckNotifyList(void);

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void ReadNotifytoListBox(
                               MMI_CTRL_ID_T list_id,
                               BOOLEAN is_replace
                               );

/*****************************************************************************/
//  Description : Get Scroll key rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T  GetScrollKeyRect(void);

/*****************************************************************************/
//  Description : Set Scroll key rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void SetScrollKeyRect(void);

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDropDownWinMsg(
                        MMI_WIN_ID_T        win_id, 
                        MMI_MESSAGE_ID_E    msg_id, 
                        DPARAM              param
                        );

/*****************************************************************************/
//  Description : Init dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void InitDropDownWin(
                        MMI_HANDLE_T win_id
                        );

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void InitDropDownShortCut(MMI_HANDLE_T win_id);

/*****************************************************************************/
//  Description : Handle ShortCut
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void  HandleShortCutFun(
                        MMI_WIN_ID_T        win_id, 
                        GUI_POINT_T        *tp_point_ptr
                        );

#ifdef WIFI_SUPPORT
/*****************************************************************************/
//  Description : ShortCut Wifi
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_OpenWifi(void);

/*****************************************************************************/
//  Description : ShortCut Wifi
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void PowerOnWifiCallback(MMIWIFI_NOTIFY_EVENT_E event_id,MMIWIFI_NOTIFY_PARAM_T* param_ptr);

/*****************************************************************************/
//  Description : ShortCut Wifi
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetWifiState(void);
#endif

/*****************************************************************************/
//  Description : ShortCut BT
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_OpenBt(void);

/*****************************************************************************/
//  Description : ShortCut BT
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetBtState(void);

/*****************************************************************************/
//  Description : ShortCut Flymode
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
//LOCAL void Shortcut_SwitchFlymode(void);

/*****************************************************************************/
//  Description : ShortCut Flymode
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
//LOCAL void Shortcut_SetFlymode(void);

/*****************************************************************************/
//  Description : ShortCut Silent
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_OpenSilent(void);

/*****************************************************************************/
//  Description : ShortCut Silent
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetSilentState(void);

/*****************************************************************************/
//  Description : ShortCut BackLight
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetBacklight(void);

/*****************************************************************************/
//  Description : ShortCut BackLight
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetBackLightState(void);

/*****************************************************************************/
//  Description : Shortcut_SetSaveMode Silent
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetSaveMode(void);
/*****************************************************************************/
//  Description :  Shortcut_SetSaveModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetSaveModeState(void);
/*****************************************************************************/
//  Description : Shortcut_SetSaveMode Silent
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetFlyMode(void);
/*****************************************************************************/
//  Description :  Shortcut_SetSaveModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetFlyModeState(void);
/*****************************************************************************/
//  Description :  Shortcut_SetPhoneLockMode
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetPhoneLockMode(void);
/*****************************************************************************/
//  Description :  Shortcut_SetFlyModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetPhoneLockModeState(void);
/*****************************************************************************/
//  Description : Shortcut_SetDataConnectMode
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetDataConnectMode(void);
/*****************************************************************************/
//  Description : Shortcut_SetDataConnectModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetDataConnectModeState(void);

/*****************************************************************************/
//  Description : Display dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisplayDropDownWin(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : Display dropdown bg
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisplayDropDownBg(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : Display dropdown shortcut
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisplayShortcut(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : Display dropdown network
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisPlayNetWork(
                    MMI_HANDLE_T  win_id,
                    GUI_LCD_DEV_INFO const *lcd_dev_ptr
                    );

/*****************************************************************************/
//  Description : Handle TP move
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownTPMove(
                               MMI_WIN_ID_T        win_id, 
                               GUI_POINT_T        *tp_point_ptr
                               );

/*****************************************************************************/
//  Description : Handle TP down
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownTPDown(
                               MMI_WIN_ID_T        win_id, 
                               GUI_POINT_T        *tp_point_ptr
                               );

/*****************************************************************************/
//  Description : Handle TP up
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownTPUp(
                               MMI_WIN_ID_T        win_id, 
                               GUI_POINT_T        *tp_point_ptr
                               );

/*****************************************************************************/
//  Description : Handle dropdown win slide
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownSlide(MMI_WIN_ID_T    win_id);

/*****************************************************************************/
//  Description : Handle Lcd siwtch
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownLcdSwitch(MMI_WIN_ID_T        win_id);

/*****************************************************************************/
//  Description : Handle lose focus
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownLoseFocus(MMI_WIN_ID_T        win_id);

/*****************************************************************************/
//  Description : Handle get focus
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownGetFocus(MMI_WIN_ID_T        win_id);

/*****************************************************************************/
//  Description : Start TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StartTpRedrawTimer(MMI_WIN_ID_T    win_id );

/*****************************************************************************/
//  Description : Stop TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopTpRedrawTimer(void);

/*****************************************************************************/
//  Description : Start TP move timer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void StartTpMoveTimer(MMI_WIN_ID_T    win_id );

/*****************************************************************************/
//  Description : Stop TP move timer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void StopTpMoveTimer(void);

/*****************************************************************************/
//  Description : Append Layer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void AppendLayer(void);

/*****************************************************************************/
//  Description : Create DropDown Layer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateDropDownLayer(
                        MMI_HANDLE_T win_id,
                        GUI_LCD_DEV_INFO *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : Create Status Bar Layer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateStatusBarLayer(
                        MMI_HANDLE_T win_id,
                        GUI_LCD_DEV_INFO *lcd_dev_ptr
                        );

/*****************************************************************************/
//  Description : counted timer Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CountedTimerNotifyCallBack(void);

/*****************************************************************************/
//  Description : calling Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CallingNotifyCallBack(void);

/*****************************************************************************/
//  Description : usb using Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN UsbUsingNotifyCallBack(void);

/*****************************************************************************/
//  Description : Mp3 Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN Mp3NotifyCallBack(void);

/*****************************************************************************/
//  Description : FM Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN FMNotifyCallBack(void);
#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : MRAPP Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MRAPPNotifyCallBack(void);

#ifdef __MMI_SKYQQ__
/*****************************************************************************/
//  Description : SKYQQ Notify callback
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN SkyQQNotifyCallBack(void);
#endif
#endif

/*****************************************************************************/
//  Description : BT Notify callback
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
#ifdef BLUETOOTH_SUPPORT
LOCAL BOOLEAN BTNotifyCallBack(void);
#endif
//#ifdef ASP_SUPPORT
///*****************************************************************************/
////  Description : new asp Notify callback
////  Global resource dependence : none
////  Author: bin.ji
////  Note: 
///*****************************************************************************/
//LOCAL BOOLEAN NewApNotifyCallBack(void);
//#endif

/*****************************************************************************/
//  Description : Get Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMIDROPDOWN_NOTIFY_CALLBACK GetNotifyCallBack(DROPDOWN_NOTIFY_E    notify_type);

/*****************************************************************************/
//  Description : Get new message notify image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetNewMessageNotifyImage(void);

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//  Description : Get new message notify image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetNewPushNotifyImage(void);
#endif
/*****************************************************************************/
//  Description : Get notify detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN GetNotifyDetail(
                        uint32    index,
                        MMIDROPDOWN_NOTIFY_DETAIL_T *   detail
                        );

/*****************************************************************************/
//  Description : Get notify number
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL uint8 GetNotifyNumber(void);

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetMessageFullNotifyImage(void);

/*****************************************************************************/
//  Description : Get full title
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN GetFullTitle(
                           uint32 record_num,       //in
                           MMI_TEXT_ID_T title_id,  //in
                           MMI_STRING_T *str_ptr   //out
                           );

#ifdef ASP_SUPPORT

/*****************************************************************************/
//  Description : Get new asp detail
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void GetNewAspDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail);
#endif

/*****************************************************************************/
//  Description : Get miss call detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetMissCallDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail);

/*****************************************************************************/
//  Description : Get new message number
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL uint8 GetNewMessageNumber(
                                      uint8                *num_ptr, 
                                      uint8                 max_num_len                                         
                                      );

#if defined(MMI_SMSCB_SUPPORT)
/*****************************************************************************/
//  Description : Get new SMSCB number
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void GetNewSMSCBDetail(MMIDROPDOWN_NOTIFY_DETAIL_T * detail);
#endif

/*****************************************************************************/
//  Description : Get new message time
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetNewMessageTime(wchar    *time_ptr);

/*****************************************************************************/
//  Description : Get new message detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetNewMessageDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail);

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//  Description : Get new Push detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetNewPushMessageDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail);
#endif

/*****************************************************************************/
//  Description : Get STK Idle Text
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void GetSTKIdleTextDetail(MMIDROPDOWN_NOTIFY_DETAIL_T * detail);

/*****************************************************************************/
//  Description : Add notify record
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void AddNotifyRecord(DROPDOWN_NOTIFY_E    notify_type);

/*****************************************************************************/
//  Description : Delete notify record
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DelNotifyRecord(DROPDOWN_NOTIFY_E    notify_type);

/*****************************************************************************/
//  Description : Init notify list
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void InitNotifyList(void);

WINDOW_TABLE( MMIDROPDOWN_WIN_TAB) = 
{
    WIN_FUNC( (uint32)HandleDropDownWinMsg),    
    WIN_ID( MMI_DROPDOWN_WIN_ID),    
    WIN_STATUSBAR,
    WIN_MOVE_STYLE(MOVE_NOT_FULL_SCREEN_WINDOW),
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_CUR),
    END_WIN
};

/*****************************************************************************/
//  Description : Get List Box Rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL GUI_BOTH_RECT_T GetDropDownNotifyListBoxRect(void)
{
    GUI_BOTH_RECT_T both_rect = {0};
    GUI_RECT_T v_rect = MMIDROPDOWN_NOTIFY_LIST_RECT_V;
    GUI_RECT_T h_rect = MMIDROPDOWN_NOTIFY_LIST_RECT_H;

    both_rect.v_rect = v_rect;

    both_rect.h_rect = h_rect;

    return both_rect;
}

/*****************************************************************************/
//  Description : Get List Box Rect
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_BOTH_RECT_T GetDropDownRunningListBoxRect(void)
{
    GUI_BOTH_RECT_T both_rect = {0};
    GUI_RECT_T v_rect = MMIDROPDOWN_RUNNING_LIST_RECT_V;
    GUI_RECT_T h_rect = MMIDROPDOWN_RUNNING_LIST_RECT_H;

    both_rect.v_rect = v_rect;

    both_rect.h_rect = h_rect;

    return both_rect;
}

/*****************************************************************************/
//  Description : Create List Box Ctrl
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateNotifyListBox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T list_id)
{
    MMI_HANDLE_T list_handle = 0;
    GUILIST_TYPE_E type = GUILIST_TEXTLIST_E;
    GUILIST_INIT_DATA_T init_data = {0};
    GUILIST_EMPTY_INFO_T empty_info = {0};
    MMI_STRING_T  string= {0};
    GUI_BOTH_RECT_T list_both_rect = GetDropDownNotifyListBoxRect();

    MMK_DestroyDynaCtrl(list_id);

    init_data.both_rect = list_both_rect;
    init_data.type = type;

    list_handle = GUILIST_CreateListBox(win_id, 0, list_id, &init_data);

    MMK_SetCtrlLcdDevInfo(list_handle, &s_dropdown.dropdown_layer);

    GUILIST_SetListState(list_id, GUILIST_STATE_NEED_SOFTKEY, FALSE);

    MMI_GetLabelTextByLang(TXT_EMPTY_LIST, &string);
    
    empty_info.text_buffer = string;
    
    GUILIST_SetEmptyInfo(list_id, &empty_info);

    GUILIST_SetBgImage(list_id, IMAGE_COMMON_BG/*IMAGE_DROPDOWN_BG*/, FALSE);
    
    MMK_SetAtvCtrl(win_id, list_id);    
}

/*****************************************************************************/
//  Description : Create List Box Ctrl
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateRunningListBox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T list_id)
{
    MMI_HANDLE_T list_handle = 0;
    GUILIST_TYPE_E type = GUILIST_TEXTLIST_E;
    GUILIST_INIT_DATA_T init_data = {0};
    GUILIST_EMPTY_INFO_T empty_info = {0};
    MMI_STRING_T  string= {0};
    GUI_BOTH_RECT_T list_both_rect = GetDropDownRunningListBoxRect();

    MMK_DestroyDynaCtrl(list_id);

    init_data.both_rect = list_both_rect;
    init_data.type = type;

    list_handle = GUILIST_CreateListBox(win_id, 0, list_id, &init_data);

    MMK_SetCtrlLcdDevInfo(list_handle, &s_dropdown.dropdown_layer);

    MMI_GetLabelTextByLang(TXT_EMPTY_LIST, &string);
    
    empty_info.text_buffer = string;
    
    GUILIST_SetEmptyInfo(list_id, &empty_info);

    GUILIST_SetBgImage(list_id, IMAGE_COMMON_BG/*IMAGE_DROPDOWN_BG*/, FALSE);
    
    MMK_SetAtvCtrl(win_id, list_id);    
}

/*****************************************************************************/
//  Description : Check notify list
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CheckNotifyList(void)
{
    uint32 i = 0;
    uint32 j = 0;
    BOOLEAN  valid = TRUE;
    MMIDROPDOWN_NOTIFY_ITEM_T    temp_list[DROPDOWN_NOTIFY_MAX + 1] = {0};
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;

    MMINV_READ(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list,return_value);

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
        valid = TRUE;
        
        if(TRUE ==s_notify_list[i].is_exist)
        {           
            switch(s_notify_list[i].notify_type)
            {
                case DROPDOWN_NOTIFY_MISS_CALL:
                    if(0 == MMIAPICC_GetMissedCallNumber())
                    {
                        valid = FALSE;
                    }
                    break;

                case DROPDOWN_NOTIFY_NEW_MESSAGE:
                    if(IMAGE_NULL == GetNewMessageNotifyImage())
                    {
                        valid = FALSE;
                    }
                    break;

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
                case DROPDOWN_NOTIFY_NEW_PUSH:
                    if(IMAGE_NULL == GetNewPushNotifyImage())
                    {
                        valid = FALSE;
                    }
                    break;
#endif

#if defined(MMI_SMSCB_SUPPORT)
                case DROPDOWN_NOTIFY_NEW_SMSCB:
                    if (0 == MMIAPISMSCB_GetUnreadSMSCBNum())
                    {
                        valid = FALSE;
                    }
                    break;
#endif

                case DROPDOWN_NOTIFY_STK_IDLE_TEXT:
                    if (!MMIIDLE_GetIdleWinInfo()->is_display_stk_idle_text)
                    {
                        valid = FALSE;
                    }
                    break;

                case DROPDOWN_RUNNING_PLAYER:
                    if (!MMIAPIMP3_IsPlayerActive() && !MMK_IsOpenWin(MMIMP3_MAIN_PLAY_WIN_ID))
                    {
                        valid = FALSE;
                    }
                    break;
#ifdef MRAPP_SUPPORT
				case DROPDOWN_RUNNING_MRAPP:
					if (VM_BACK_RUN != MMIMRAPP_GetVMState())
					{
						valid = FALSE;
					}
					break;
#ifdef __MMI_SKYQQ__
				case DROPDOWN_RUNNING_SKYQQ:
					if (GetSkyQQRunState() != QQ2008_BACK_RUN)
					{
						valid = FALSE;
					}
					break;
#endif
#endif

#ifdef COUNTEDTIME_SUPPORT
                case DROPDOWN_RUNNING_COUNTEDTIME:
                    if(!MMIAPICT_IsShowIdleCoutime())
                    {
                        valid = FALSE;
                    }
                    break;
#endif
                case DROPDOWN_RUNNING_CALLING:
                    if(!MMIAPICC_IsInState(CC_IN_CALL_STATE))
                    {
                        valid = FALSE;
                    }
                    break;

                case DROPDOWN_NOTIFY_USB_DEBUG:
                    if(!(MMIAPISD_IsUsbLogOn()||MMIAPIIDLE_IsChargeConnected()||MMIAPIUDISK_UdiskIsRun()))
                    {
                        valid = FALSE;
                    }
                    break;
                    
                case DROPDOWN_RUNNING_USB_USING:
//                    if(!MMIAPIUDISK_UdiskIsRun())
                    if (USB_SERVICE_NULL == USB_GetCurService())
                    {
                        valid = FALSE;
                    }
                    break;
                    
#ifdef ASP_SUPPORT
                case DROPDOWN_NOTIFY_NEW_ASP:
                    if (0 == MMIAPIASP_GetNewRecordNum())
                    {
                        valid = FALSE;
                    }
                    break;
#endif
/*renwei modify cr00117388*/
#ifdef FM_SUPPORT
                case DROPDOWN_RUNNING_FM:
                    if(!MMIAPIFM_IsFmActive())
                    {
                        valid = FALSE;
                    }
                    break;
#endif
#ifdef BLUETOOTH_SUPPORT
                 case DROPDOWN_RUNNING_BT_TRANSFERFILE:
                    if(!MMIAPIBT_IsOpenFileWin())
                    {
                        valid = FALSE;
                    }
                    break;
#endif
/*renwei modify cr00117388*/
//                case DROPDOWN_NOTIFY_MESSAGE_FULL:
//                    if(IMAGE_NULL == GetMessageFullNotifyImage())
//                    {
//                        valid = FALSE;
//                    }
//                    break;

                default:
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
		if (DROPDOWN_RUNNING_WRE_APP_START <= s_notify_list[i].notify_type && s_notify_list[i].notify_type <= DROPDOWN_RUNNING_WRE_APP_END)
		{
			if (!IsWreAppRunning(s_notify_list[i].notify_type -DROPDOWN_RUNNING_WRE_APP_START ))
				valid = FALSE;		//here disable all existing wre app, then to rebuild them
		}
#endif
                    break;
            }
            //SCI_TRACE_LOW:"CheckNotifyList() i = %d, notify_type = %d, valid = %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_1070_112_2_18_2_14_50_0,(uint8*)"ddd", i, s_notify_list[i].notify_type, valid);
            
            if(valid)
            {            
                SCI_MEMCPY(&temp_list[j], &s_notify_list[i], sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));       

                j ++;
            }
        }
    }

    SCI_MEMCPY(&s_notify_list[0], &temp_list[0], DROPDOWN_NOTIFY_MAX*sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));

    MMINV_WRITE(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list);    
}


/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsBelongCurPage(
                              DROPDOWN_NOTIFY_E 	notify_type,
                              DROPDOWN_PAGE_E  page
                              )
{
    DROPDOWN_PAGE_E belong_page = DROPDOWN_PAGE_NOTIFY;

    switch(notify_type)
    {
    case DROPDOWN_NOTIFY_MISS_CALL:
    case DROPDOWN_NOTIFY_NEW_MESSAGE:
#if defined(MMI_SMSCB_SUPPORT)
    case DROPDOWN_NOTIFY_NEW_SMSCB:
#endif
#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
    case DROPDOWN_NOTIFY_NEW_PUSH:
#endif
    case DROPDOWN_NOTIFY_STK_IDLE_TEXT:

#ifdef MMIEMAIL_SUPPORT
    case DROPDOWN_NOTIFY_NEW_EMAIL:
#endif
#ifdef ASP_SUPPORT
    case DROPDOWN_NOTIFY_NEW_ASP:
#endif

    case DROPDOWN_NOTIFY_USB_DEBUG: //USB调试
        belong_page = DROPDOWN_PAGE_NOTIFY;
        break;

#ifdef MMI_AUDIO_PLAYER_SUPPORT
    case DROPDOWN_RUNNING_PLAYER:
#endif
#ifdef MRAPP_SUPPORT
    case DROPDOWN_RUNNING_MRAPP:
#ifdef __MMI_SKYQQ__
    case DROPDOWN_RUNNING_SKYQQ:
#endif
#endif
#ifdef FM_SUPPORT
    case DROPDOWN_RUNNING_FM:
#endif
    case DROPDOWN_RUNNING_CALLING:
#ifdef COUNTEDTIME_SUPPORT
    case DROPDOWN_RUNNING_COUNTEDTIME:
#endif

#ifdef QQ_SUPPORT
    case DROPDOWN_NOTIFY_QQ:
#endif
#ifndef JAVA_SUPPORT_NONE
    case DROPDOWN_RUNNING_JAVA://cr00115796 renwei modify
#endif
#ifdef MCARE_V31_SUPPORT
    case DROPDOWN_NOTIFY_MCARE_QQ:
    case DROPDOWN_NOTIFY_MCARE_QC:
#endif
#ifdef BLUETOOTH_SUPPORT
   case DROPDOWN_RUNNING_BT_TRANSFERFILE:
#endif
    case DROPDOWN_RUNNING_USB_USING:
        belong_page = DROPDOWN_PAGE_RUNNING;
        break;
        
    default:
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
		if (DROPDOWN_RUNNING_WRE_APP_START <= notify_type && notify_type <= DROPDOWN_RUNNING_WRE_APP_END)
		{
			belong_page = DROPDOWN_PAGE_RUNNING;
		}
#endif
        break;
    }
    if (belong_page == page)
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetDefaultNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ANIM_ICON_TEXT_AND_ICON_2TEXT;//GUIITEM_STYLE_TWO_LINE_ICON_TEXT_AND_TWO_TEXT;//GUIITEM_STYLE_TWO_LINE_ICON_TEXT_AND_TEXT;
    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    item_ptr->item_data_ptr->item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[2].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[2].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;
    item_ptr->item_data_ptr->item_content[4].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[4].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[4].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
    item_ptr->item_data_ptr->item_content[5].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[5].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_time.wstr_ptr;
    item_ptr->item_data_ptr->item_content[5].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_time.wstr_len;
}

#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN
//static wchar       g_full_path_wstr[100] = L"d:\\Photos\\IMAGE_DROPDOWN_FM_CLOSE.png";
//static wchar       g_full_path_wstr[100] = L"d:\\Photos\\appstore.jpg";
//static GUIANIM_FILE_INFO_T g_path;
LOCAL void SetWreNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr, //in
                         int type,
                         GUIANIM_FILE_INFO_T* p_path,
                         WREAPP_INFO_T* p_wreApp
                         )
{
    //GUIANIM_FILE_INFO_T path;
    BOOLEAN bRet;
    int appId;
    
    appId = type - DROPDOWN_RUNNING_WRE_APP_START;

    //g_path.full_path_wstr_ptr = g_full_path_wstr;
    //g_path.full_path_wstr_len = MMIAPICOM_Wstrlen(g_full_path_wstr);

    bRet = WRE_GetAppInfo(appId,p_wreApp);
    if (!bRet)
    {
        return;
    }

    //test code
    //if (p_wreApp->iconpath[0] == 0)
    //{
    //    MMIAPICOM_Wstrncat(p_wreApp->iconpath,L"e:\\wre\\hello1.jpg",MMIAPICOM_Wstrlen(L"e:\\wre\\hello1.jpg"));
    //}

    p_path->full_path_wstr_ptr = p_wreApp->iconpath;
    p_path->full_path_wstr_len = MMIAPICOM_Wstrlen(p_wreApp->iconpath);
    
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ICON_TEXT_AND_TWO_TEXT;//GUIITEM_STYLE_TWO_LINE_ICON_TEXT_AND_TEXT;
    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_ANIM_PATH;
    item_ptr->item_data_ptr->item_content[0].item_data.anim_path_ptr = p_path;
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;
    item_ptr->item_data_ptr->item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[2].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[2].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_time.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_time.wstr_len;
}
#endif

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetMisscallNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->item_style     = GUIITEM_STYLE_TWO_LINE_ICON_TEXT_AND_TWO_TEXT;
    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;
    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;
    item_ptr->item_data_ptr->item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[2].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[2].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_time.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_time.wstr_len;
}

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetMessageFullNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->item_style = GUIITEM_STYLE_TWO_LINE_ICON_TEXT_AND_TEXT;
    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;
}

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetMp3NotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->user_data = DROPDOWN_RUNNING_PLAYER;
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ANIM;
    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;

    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;

    item_ptr->item_data_ptr->item_content[2].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[2].item_data.image_id = notify_detail_ptr->action_image;

    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL void SetMRAPPNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
	if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->user_data = DROPDOWN_RUNNING_MRAPP;
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ANIM;
    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;

    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;

    item_ptr->item_data_ptr->item_content[2].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[2].item_data.image_id = notify_detail_ptr->action_image;

    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
}

#ifdef __MMI_SKYQQ__
/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL void SetSkyQQNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
	if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->user_data = DROPDOWN_RUNNING_SKYQQ;
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ANIM;
    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;

    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;

    item_ptr->item_data_ptr->item_content[2].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[2].item_data.image_id = notify_detail_ptr->action_image;

    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
}
#endif
#endif

/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetCCNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    item_ptr->user_data = DROPDOWN_RUNNING_CALLING;
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ANIM;
    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;

    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;

    item_ptr->item_data_ptr->item_content[2].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[2].item_data.image_id = notify_detail_ptr->action_image;

    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
}
/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetBTNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    
#ifdef BLUETOOTH_SUPPORT
    item_ptr->user_data = DROPDOWN_RUNNING_BT_TRANSFERFILE;
#endif
    item_ptr->item_style    = GUIITEM_STYLE_ONE_LINE_TEXT_ICON;

    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;

    item_ptr->item_data_ptr->item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[0].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[0].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;

    item_ptr->item_data_ptr->item_content[1].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[1].item_data.image_id = notify_detail_ptr->action_image;    
}
/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void SetFMRadioNotifyItem(
                         GUILIST_ITEM_T *item_ptr,  //out
                         MMIDROPDOWN_NOTIFY_DETAIL_T *notify_detail_ptr //in
                         )
{
    if ((PNULL == item_ptr) || (PNULL == item_ptr->item_data_ptr) || (PNULL == notify_detail_ptr))
    {
        return;
    }
    /*renwei modify cr00117388*/
#ifdef FM_SUPPORT
    item_ptr->user_data = DROPDOWN_RUNNING_FM;
#endif
/*renwei modify cr00117388*/
    item_ptr->item_style    = GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ANIM;

    item_ptr->item_state |= GUIITEM_STATE_CONTENT_CHECK;

    item_ptr->item_data_ptr->item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[0].item_data.image_id = notify_detail_ptr->notify_image;
    
    item_ptr->item_data_ptr->item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_title.wstr_ptr;
    item_ptr->item_data_ptr->item_content[1].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_title.wstr_len;

    item_ptr->item_data_ptr->item_content[2].item_data_type     = GUIITEM_DATA_IMAGE_ID;
    item_ptr->item_data_ptr->item_content[2].item_data.image_id = notify_detail_ptr->action_image;

    item_ptr->item_data_ptr->item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_ptr = notify_detail_ptr->notify_string.wstr_ptr;
    item_ptr->item_data_ptr->item_content[3].item_data.text_buffer.wstr_len = notify_detail_ptr->notify_string.wstr_len;
}


#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
 /*****************************************************************************/
 //  Description : Add WRE app into dropdown notify record
 //  Global resource dependence : none
 //  Author: david.li
 //  Note: 
 /*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_AddWreNotifyRecord(int32 wre_app_idx)
{
	AddNotifyRecord(DROPDOWN_RUNNING_WRE_APP_START + wre_app_idx);
}

 /*****************************************************************************/
 //  Description : Delete WRE app from dropdown notify record
 //  Global resource dependence : none
 //  Author: david.li
 //  Note: 
 /*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_DelWreNotifyRecord(int32 wre_app_idx)
{
	MMIDROPDOWNWIN_DelNotifyRecord(DROPDOWN_RUNNING_WRE_APP_START + wre_app_idx);
}

#endif


/*****************************************************************************/
//  Description : Read Items to list box
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void ReadNotifytoListBox(
                               MMI_CTRL_ID_T list_id,
                               BOOLEAN is_replace
                               )
{
    GUILIST_ITEM_T item_t = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    uint32 i = 0;
//    uint32 index = 0;
    MMIDROPDOWN_NOTIFY_DETAIL_T    notify_detail = {0};
    uint16 index = 0;

    if (!is_replace)
    {
        GUILIST_SetMaxItem(list_id,DROPDOWN_NOTIFY_MAX, FALSE );   
    }    

    CheckNotifyList();

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
//        index = 0;
        
        SCI_MEMSET(&notify_detail, 0, sizeof(MMIDROPDOWN_NOTIFY_DETAIL_T));
		SCI_MEMSET(&item_t, 0, sizeof(GUILIST_ITEM_T));
        SCI_MEMSET(&item_data, 0, sizeof(GUILIST_ITEM_DATA_T));
		item_t.item_data_ptr = &item_data;  
        
//        if(TRUE == s_notify_list[i].is_exist)
        if ((TRUE == s_notify_list[i].is_exist)
            && (IsBelongCurPage(s_notify_list[i].notify_type, s_dropdown.cur_page)))
        {
            if(GetNotifyDetail(i, &notify_detail))
            {         
                /*
                if(DROPDOWN_NOTIFY_MESSAGE_FULL == s_notify_list[i].notify_type
#ifndef JAVA_SUPPORT_NONE
                  ||DROPDOWN_NOTIFY_JAVA == s_notify_list[i].notify_type
#endif
#ifdef QQ_SUPPORT
                  ||DROPDOWN_NOTIFY_QQ == s_notify_list[i].notify_type
#endif
                  )
                {
                    SetMessageFullNotifyItem(&item_t, &notify_detail);
                }
                else if(DROPDOWN_NOTIFY_MISS_CALL == s_notify_list[i].notify_type
                         ||DROPDOWN_NOTIFY_NEW_MESSAGE == s_notify_list[i].notify_type
                         )
                {
                    SetMisscallNotifyItem(&item_t, &notify_detail);
                }
                else
                */
                if(DROPDOWN_RUNNING_CALLING == s_notify_list[i].notify_type)
                {   
                    SetCCNotifyItem(&item_t, &notify_detail);
                }
#ifdef MMI_AUDIO_PLAYER_SUPPORT
                else if(DROPDOWN_RUNNING_PLAYER == s_notify_list[i].notify_type)
                {   
                    SetMp3NotifyItem(&item_t, &notify_detail);
                }
#endif     
#ifdef MRAPP_SUPPORT
				else if(DROPDOWN_RUNNING_MRAPP == s_notify_list[i].notify_type)
				{
	#if (SPR_VERSION >= 0x12B1226)
		SetMRAPPNotifyItem(&item_t,detail_ptr);
	#else
					SetMRAPPNotifyItem(&item_t, &notify_detail);
	#endif
				}
#ifdef __MMI_SKYQQ__
				else if(DROPDOWN_RUNNING_SKYQQ == s_notify_list[i].notify_type)
				{
	#if (SPR_VERSION >= 0x12B1226)
		SetSkyQQNotifyItem(&item_t,detail_ptr);
	#else
					SetSkyQQNotifyItem(&item_t, &notify_detail);
	#endif			
				}
#endif
#endif  
#ifdef FM_SUPPORT
                else if(DROPDOWN_RUNNING_FM == s_notify_list[i].notify_type)
                {
                    SetFMRadioNotifyItem(&item_t, &notify_detail);
                }
#endif 
#ifdef BLUETOOTH_SUPPORT
                else if(DROPDOWN_RUNNING_BT_TRANSFERFILE == s_notify_list[i].notify_type)
                {
                    SetBTNotifyItem(&item_t, &notify_detail);
                }
#endif
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN
                else if(DROPDOWN_RUNNING_WRE_APP_START <= s_notify_list[i].notify_type && s_notify_list[i].notify_type <= DROPDOWN_RUNNING_WRE_APP_END)
                {
                    int tmp;
                    GUIANIM_FILE_INFO_T path;
                    WREAPP_INFO_T wreApp;

                    tmp = GUILIST_GetTotalItemNum(list_id);
                    SetWreNotifyItem(&item_t,&notify_detail,s_notify_list[i].notify_type,&path,&wreApp);
                    if (!is_replace)
                    {
                        GUILIST_AppendItem(list_id, &item_t);
                        //GUILIST_SetItemContent(list_id,&item_t.item_data_ptr->item_content[0],0,tmp);
                        GUILIST_SetItemContentExt(list_id,&item_t.item_data_ptr->item_content[0],tmp,0,TRUE,FALSE);
                        continue;
                    }                    
                }
#endif
                else
                {
                    SetDefaultNotifyItem(&item_t, &notify_detail);
                }

                if (is_replace)
                {
                    if (CheckNeedUpdateTimerItem(s_notify_list[i].notify_type))
                    {
                        GUILIST_ReplaceItem(list_id, &item_t, index);
                    }
                    index++;
                }
                else
                {
                    GUILIST_AppendItem(list_id, &item_t);
                }
            }
        }
    }    
}

/*****************************************************************************/
//  Description : Get Scroll key rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T  GetScrollKeyRect(void)
{
    return s_dropdown.scrollkey_rect;
}

/*****************************************************************************/
//  Description : Set Scroll key rect
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void SetScrollKeyRect(void)
{
    GUI_RECT_T    scrollkey_rect = {0};

    scrollkey_rect.left = 0;

    scrollkey_rect.right = s_dropdown.width - 1;

    scrollkey_rect.top = s_dropdown.bottom - MMIDROPDOWN_SCROLLKEY_HEIGHT + 1;

    scrollkey_rect.bottom =  s_dropdown.bottom;

    s_dropdown.scrollkey_rect = scrollkey_rect;
}

/*****************************************************************************/
//  Description : set dropdown layer pos
//  Global resource dependence : 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
LOCAL void SetDropdownLayerPos(
                               GUI_LCD_DEV_INFO  *dev_info_ptr,
                               int16 x,
                               int16 y
                               )
{
//    GUI_RECT_T clip_rect = MMITHEME_GetFullScreenRect();

    if (PNULL == dev_info_ptr)
    {
        //SCI_TRACE_LOW:"SetDropdownLayerPos() error input param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_1656_112_2_18_2_14_51_1,(uint8*)"");
        return;
    } 
    if (UILAYER_IsLayerActive(&s_dropdown.dropdown_layer))
    {
        UILAYER_SetLayerPosition(dev_info_ptr, x, y);
    }
//    clip_rect.top = MMI_STATUSBAR_HEIGHT - y;
//    UILAYER_SetClipRect(dev_info_ptr, TRUE, &clip_rect);
}

/*****************************************************************************/
//  Description : Mp3 Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopMp3Player(
                         MMI_WIN_ID_T        win_id
                         )
{
//    MMIAPIMP3_StopApplet(FALSE);
    MMIAPIMP3_StopOperaSDPlug();
    ReloadPageContent(win_id);
    MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
    /*
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);
    uint16 index= 0;
    const GUILIST_ITEM_T *list_item_ptr = PNULL;
    uint16 total_num = 0;    
    uint32 i = 0;
    
    total_num = GUILIST_GetTotalItemNum(ctrl_id);
    MMIAPIMP3_StopAudioPlayer();
    for (i = 0; i < total_num; i++)
    {
        list_item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id, index);
        if (DROPDOWN_RUNNING_PLAYER == list_item_ptr->user_data)
        {
            break;
        }
    }
    if (i < total_num)
    {
        GUILIST_RemoveItem(ctrl_id, i);
        MMK_SendMsg(win_id, MSG_FULL_PAINT, 0);
//        MMK_UpdateScreen();
    }
    */
}

/*****************************************************************************/
//  Description : fmradio Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopFMRadio(
                         MMI_WIN_ID_T        win_id
                         )
{
    MMIAPIFM_Exit();
    ReloadPageContent(win_id);
    MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
    /*
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);
    uint16 index= 0;
    const GUILIST_ITEM_T *list_item_ptr = PNULL;
    uint16 total_num = 0;    
    uint32 i = 0;
    
    total_num = GUILIST_GetTotalItemNum(ctrl_id);
//    MMIAPIMP3_StopAudioPlayer();
    for (i = 0; i < total_num; i++)
    {
        list_item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id, index);
        if (DROPDOWN_RUNNING_FM == list_item_ptr->user_data)
        {
            break;
        }
    }
    MMIAPIFM_Exit();
    if (i < total_num)
    {
        GUILIST_RemoveItem(ctrl_id, i);
        MMK_SendMsg(win_id, MSG_FULL_PAINT, 0);
//        MMK_UpdateScreen();
    }
    */
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : mrapp Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopMRAPP(
                         MMI_WIN_ID_T        win_id
                         )
{
    MMIMRAPP_closeMrapp();
    ReloadPageContent(win_id);
    MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
}

#ifdef __MMI_SKYQQ__
/*****************************************************************************/
//  Description : skyqq Notify callback
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL void StopSkyQQ(
                         MMI_WIN_ID_T        win_id
                         )
{
    sky_qq_force_exit();
    ReloadPageContent(win_id);
    MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
}
#endif
#endif

#ifdef BLUETOOTH_SUPPORT
/*****************************************************************************/
//  Description : bt Notify callback
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void StopBTTransferFile(
                         MMI_WIN_ID_T        win_id
                         )
{
    MMIAPIBT_StopBTTransferFile();
    ReloadPageContent(win_id);
    MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
}
#endif
/*****************************************************************************/
//  Description : Mp3 Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL uint16 ConvertListIndexToNotifyIndex(
                                           uint16 list_index
                                           )
{
    uint16 i = 0;
    uint16 index = 0;

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
        if (s_notify_list[i].is_exist)
        {
            if (IsBelongCurPage(s_notify_list[i].notify_type, s_dropdown.cur_page))
            {
                if (index == list_index)
                {
                    return i;
                }
                index++;
            }
        }
    }
    return DROPDOWN_NOTIFY_MAX;
}

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CheckNeedUpdateTimer(void)
{
    if ((MMIAPICT_IsShowIdleCoutime() || MMIAPICC_IsInState(CC_IN_CALL_STATE))
        && (DROPDOWN_PAGE_RUNNING == s_dropdown.cur_page))
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CheckNeedUpdateTimerItem(
                                       DROPDOWN_NOTIFY_E notify_type                                       
                                       )
{
    if ((DROPDOWN_RUNNING_CALLING == notify_type)
#ifdef COUNTEDTIME_SUPPORT
        ||(DROPDOWN_RUNNING_COUNTEDTIME == notify_type) 
#endif
       )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void HandleListCheck(
                           MMI_WIN_ID_T        win_id, 
                           uint32 user_data,
                           GUILIST_NEED_ITEM_CONTENT_T *need_item_content_ptr
                           )
{
    if (PNULL == need_item_content_ptr)
    {
        return;
    }
    if (DROPDOWN_RUNNING_PLAYER == user_data)
    {
        switch (need_item_content_ptr->item_content_index)
        {
        case 0:
            //open mp3 player
            Mp3NotifyCallBack();
            break;
            
        case 2:
            //stop mp3 player
            StopMp3Player(win_id);
            break;
            
        default:
            break;
        }
    }
    else if (DROPDOWN_RUNNING_CALLING == user_data)
    {
        switch (need_item_content_ptr->item_content_index)
        {
        case 0:
            //open mp3 player
            CallingNotifyCallBack();
            break;
            
        case 2:
            //stop mp3 player
            MMIAPICC_ReleaseCallByRedkey();
            MMK_CloseWin(win_id);
            break;
            
        default:
            break;
        }
    }
    
/*renwei modify cr00117388*/
#ifdef FM_SUPPORT
    else if (DROPDOWN_RUNNING_FM == user_data)
    {
        switch (need_item_content_ptr->item_content_index)
        {
        case 0:
            //open mp3 player
            FMNotifyCallBack();
            break;
            
        case 2:
            //stop mp3 player
            StopFMRadio(win_id);
            break;
            
        default:
            break;
        }
    }
#endif    
/*renwei modify cr00117388*/
#ifdef BLUETOOTH_SUPPORT
    else if(DROPDOWN_RUNNING_BT_TRANSFERFILE == user_data)
    {
        switch (need_item_content_ptr->item_content_index)
        {
        case 0:
            //open bt transfer file win
            BTNotifyCallBack();
            break;
            
        case 1:
            //stop bt transfer file win
            StopBTTransferFile(win_id);            
            break;
            
        default:
            break;
        }       
    }
#endif
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
	else if (DROPDOWN_RUNNING_WRE_APP_START <= user_data && user_data <= DROPDOWN_RUNNING_WRE_APP_END)
	{
		//display corresponding application, this function is mocor-called to wre kernel
		switch (need_item_content_ptr->item_content_index)
		{
			case 0:
				//open wre app
				break;

			case 2:
				//exit wre app
				break;

			default:
				break;
		}
	}
#endif
#ifdef MRAPP_SUPPORT
	else if (DROPDOWN_RUNNING_MRAPP == user_data)
	{
		switch (need_item_content_ptr->item_content_index)
		{
		case 0:
            //open QQ
            MRAPPNotifyCallBack();
            break;
            
        case 2:
            //stop QQ
            StopMRAPP(win_id);
            break;
            
        default:
            break;
        }       
    }
#ifdef __MMI_SKYQQ__
	else if (DROPDOWN_RUNNING_SKYQQ == user_data)
	{
		switch (need_item_content_ptr->item_content_index)
		{
		case 0:
            //open QQ
            SkyQQNotifyCallBack();
            break;
            
        case 2:
            //stop QQ
            StopSkyQQ(win_id);
            break;
            
        default:
            break;
		}	
	}
#endif
#endif
}

/*****************************************************************************/
//  Description : statusbar的转场动画
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StatusBarScrollAnim(
                               MMI_WIN_ID_T        win_id,
                               BOOLEAN is_open
                               )
{
    uint32 i = 0;
    const uint8 alpha_max = 255;
    uint8 start_alpha = 0;
    uint8 end_alpha = 0;
    uint8 alpha = 0;
    int16 y_offset = 0;
    int16 y_interval = 0;   
    int32 alpha_interval = 0;
    int16 start_y_offset = 0;
    int16 end_y_offset = 0;
    MMI_HANDLE_T    statusbar_handle  = MMK_GetCtrlHandleByWin(win_id, MMITHEME_GetStatusBarCtrlId());    
    
//    if (MMIDROPDOWN_STATUSBAR_ANIM_FRAME < 2)
//    {
//        //只有一帧，无转场动画
//        return;
//    }
    AppendLayer();

    if (is_open)
    {
        start_y_offset = s_dropdown.top - MMIDROPDOWN_SCROLLKEY_HEIGHT;
        end_y_offset = s_dropdown.top;
        start_alpha = 0;
        end_alpha = alpha_max;
        y_interval = MMIDROPDOWN_SCROLLKEY_HEIGHT / (MMIDROPDOWN_STATUSBAR_ANIM_FRAME-1);
        alpha_interval = (alpha_max+1)/(MMIDROPDOWN_STATUSBAR_ANIM_FRAME-1);
    }
    else
    {
        start_y_offset = s_dropdown.top;
        end_y_offset = s_dropdown.top - MMIDROPDOWN_SCROLLKEY_HEIGHT;
        start_alpha = alpha_max;
        end_alpha = 0;
        y_interval = -MMIDROPDOWN_SCROLLKEY_HEIGHT / (MMIDROPDOWN_STATUSBAR_ANIM_FRAME-1);
        alpha_interval = -(alpha_max+1)/(MMIDROPDOWN_STATUSBAR_ANIM_FRAME-1);
    }
    y_offset = start_y_offset;
    alpha = start_alpha;
    MMITHEME_SetUpdateDelayCount(0);
    for (i = 0; i < MMIDROPDOWN_STATUSBAR_ANIM_FRAME; i++)
    {
        //display scrollbar
        SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, y_offset);
        DisplayDropDownScrollBar(win_id, &s_dropdown.dropdown_layer);   

        //display statusbar
        MMK_SendMsg(statusbar_handle, MSG_CTL_PAINT, PNULL);
        UILAYER_WeakLayerAlpha(&s_dropdown.statusbar_layer, alpha);
        MMITHEME_UpdateRect();
        //SCI_SLEEP(400);
        if ((MMIDROPDOWN_STATUSBAR_ANIM_FRAME-2) == i)
        {
            //last frame
            y_offset = end_y_offset;
            alpha = end_alpha;
        }
        else
        {
            y_offset += y_interval;
            alpha += alpha_interval;
        }
    }
}

/*****************************************************************************/
//  Description : 开始触笔抬起后的滑动
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StartFling(
                      MMI_WIN_ID_T        win_id
                      )
{
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);

    StartTpMoveTimer(win_id); 
    //隐藏List控件，防止滑动的时候误响应List
    GUIAPICTRL_SetState(ctrl_id,GUICTRL_STATE_INVISIBLE,TRUE);                
}

/*****************************************************************************/
//  Description : 结束触笔抬起后的滑动
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopFling(
                     MMI_WIN_ID_T        win_id
                     )
{
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);

    StopTpMoveTimer(); 
    //隐藏List控件，防止滑动的时候误响应List
    GUIAPICTRL_SetState(ctrl_id,GUICTRL_STATE_INVISIBLE,FALSE);                
}

////////////////here to refresh wre applications list ///////////////////////////
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
void MMIDROPDOWNWIN_RegisterWreNotifyRecords(void)
{
	{
		int32 i, k;
		int32 count = WRE_GetAppCount();


		//SCI_TRACE_LOW:"== Add wre running apps: %d app info into dropdown win \n=="
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_2081_112_2_18_2_14_52_2,(uint8*)"d", count);
        
        //first del
        for (i = 0; i < DROPDOWN_NOTIFY_MAX ; i++)
        {
            if (TRUE == s_notify_list[i].is_exist && (s_notify_list[i].notify_type >= DROPDOWN_RUNNING_WRE_APP_START && s_notify_list[i].notify_type <= DROPDOWN_RUNNING_WRE_APP_END))
            {
                MMIDROPDOWNWIN_DelNotifyRecord(s_notify_list[i].notify_type);  
            }
        }

        if (count <= 0)
        {            
            return;
        }

#if 0
		if (count >= DROPDOWN_NOTIFY_MAX - DROPDOWN_RUNNING_WRE_APP_START)
		{
			count = DROPDOWN_NOTIFY_MAX - DROPDOWN_RUNNING_WRE_APP_START -1;
			//SCI_TRACE_LOW:"too much running wre apps!!!\n"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_2101_112_2_18_2_14_52_3,(uint8*)"");
		}
#else
        if (count >= WRE_APP_RUN_MAX_NUM)
        {
            count = WRE_APP_RUN_MAX_NUM;
            //SCI_TRACE_LOW:"too much running wre apps!!!\n"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_2107_112_2_18_2_14_52_4,(uint8*)"");
		}
#endif

		k = 0;
		for (i = 0; i < DROPDOWN_NOTIFY_MAX ; i++)
		{
			if (FALSE == s_notify_list[i].is_exist)
			{
				MMIDROPDOWNWIN_AddWreNotifyRecord(getRunningWreAppIdByIdx(k));
				k++;
				if (k >= count)
					break;
			}
		}
	}
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
//  Description : Main handle of dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDropDownWinMsg(
                                        MMI_WIN_ID_T        win_id, 
                                        MMI_MESSAGE_ID_E    msg_id, 
                                        DPARAM              param
                                        )
{
    MMI_RESULT_E     result             = MMI_RESULT_TRUE;
    IGUICTRL_T       *stb_ctr_ptr       = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id, MMITHEME_GetStatusBarCtrlId());    
    GUI_POINT_T      point              = {0};
    MMI_CTRL_ID_T    ctrl_id            = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);
    uint16           cur_select_item    = 0;
    static BOOLEAN   s_is_date_on       = FALSE;
    GUI_LCD_DEV_INFO lcd_dev_info       = {0};
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        s_dropdown.parent_win = MMK_GetPrevWinHandle(MMK_ConvertIdToHandle(win_id));
        
        MMK_SetWinSupportAngle(win_id, MMK_GetWinSupportAngle(s_dropdown.parent_win));
        //            //只支持竖屏
        //            MMK_SetWinSupportAngle(win_id, WIN_SUPPORT_ANGLE_0);
        //            MMK_SetWinAngle(win_id, LCD_ANGLE_0, FALSE);
        
#ifdef UI_P3D_SUPPORT1
        MMI_Disable3DEffect(MMI_3D_EFFECT_DROPDOWN);
#endif  
        //当前页
        s_dropdown.cur_page = s_cur_page;
        
        InitDropDownWin(win_id);
        
        InitDropDownShortCut(win_id);
        
        SetScrollKeyRect();
        
        ReloadPageContent(win_id);
        
        if(!s_dropdown.auto_slide)
        {
            StartTpRedrawTimer(win_id);
        }
        UILAYER_SetDirectDraw(TRUE);
        GUIWIN_SetStbDropDownState(win_id,TRUE);            
        GUICTRL_SetLcdDevInfo(stb_ctr_ptr, &s_dropdown.statusbar_layer); 
        //scroll anim
        StatusBarScrollAnim(win_id, TRUE);
        break;
        
    case MSG_CTL_LIST_CHECK_CONTENT:
        {
            uint16 cur_index= 0;
            const GUILIST_ITEM_T *list_item_ptr = PNULL;
            GUILIST_NEED_ITEM_CONTENT_T *need_item_content_ptr = (GUILIST_NEED_ITEM_CONTENT_T*)param;
            
            cur_index = GUILIST_GetCurItemIndex(ctrl_id);
            list_item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id, cur_index);
            HandleListCheck(win_id, list_item_ptr->user_data, need_item_content_ptr);
        }
        break;
        
    case MSG_FULL_PAINT:
        // 这里如果多层不在active，应该画在主层上，否则弹出pubwin会越来越暗
        if (UILAYER_IsLayerActive(&s_dropdown.dropdown_layer))
        {
            lcd_dev_info = s_dropdown.dropdown_layer;
        }
        SetDropdownLayerPos(&lcd_dev_info, 0, MMI_STATUSBAR_HEIGHT);
        DisplayDropDownWin(win_id, &lcd_dev_info);       
        break;
        
    case MSG_NOTIFY_STATUSBAR_PAINT:
        s_is_date_on = GUIWIN_IsStbItemVisible(MMI_WIN_TEXT_DATE);        
        GUIWIN_SetStbItemVisible(MMI_WIN_TEXT_DATE, TRUE);
        break;
        
    case MSG_NOTIFY_STATUSBAR_PAINT_END:
        GUIWIN_SetStbItemVisible(MMI_WIN_TEXT_DATE, s_is_date_on);
        s_is_date_on = FALSE;
        break;
        
    case MSG_LCD_SWITCH:
        HandleDropDownLcdSwitch(win_id);
        ReloadPageContent(win_id);            
        break;
        
    case MSG_LOSE_FOCUS:
        GUIWIN_SetStbItemVisible(MMI_WIN_TEXT_DATE, FALSE);
        StopUpdateRedrawTimer();
        HandleDropDownLoseFocus(win_id);
        break;
        
    case MSG_GET_FOCUS:
        GUIWIN_SetStbItemVisible(MMI_WIN_TEXT_DATE, TRUE);
        HandleDropDownGetFocus(win_id);
        
        if (CheckNeedUpdateTimer())
        {
            StartUpdateRedrawTimer(win_id);
        }
        break;
        
        //        case MSG_TP_PRESS_MOVE:
        //            {               
        //                point.x=MMK_GET_TP_X(param);
        //                point.y=MMK_GET_TP_Y(param);
        //
        //                SCI_TRACE_LOW("HandleDropDownWinMsg() MSG_TP_PRESS_MOVE(%d, %d)", point.x, point.y);
        //                HandleDropDownTPMove(win_id, &point);
        //            }
        //            break;
        
    case MSG_TP_PRESS_DOWN:
        {
            GUI_RECT_T scrollkey_rect = {0};
            
            point.x=MMK_GET_TP_X(param);
            point.y=MMK_GET_TP_Y(param);
            
            //                SCI_TRACE_LOW("HandleDropDownWinMsg() MSG_TP_PRESS_DOWN(%d, %d)", point.x, point.y);
            //point.y有可能大于scrollkey_rect.bottom
            //                if(GUI_PointIsInRect(point, GetScrollKeyRect()))
            
            if ( MMK_TP_SLIDE_FLING != s_dropdown.slide_state )
            {
                scrollkey_rect = GetScrollKeyRect();
                if ((point.x >= scrollkey_rect.left) && (point.x <= scrollkey_rect.right)
                    && (point.y >= scrollkey_rect.top))
                {
                    s_dropdown.state = DROPDOWN_STATE_CLOSE;
                    StartTpRedrawTimer(win_id);
                }
                
                HandleDropDownTPDown(win_id, &point);
            }
        }
        break;
        
    case MSG_TP_PRESS_UP:     
        {
            point.x=MMK_GET_TP_X(param);
            point.y=MMK_GET_TP_Y(param);          
            
            HandleDropDownTPUp(win_id, &point);
            
            if (CheckNeedUpdateTimer())
            {
                StartUpdateRedrawTimer(win_id);
            }
        }
        break;
        
        /*
        case MSG_KEYDOWN_HEADSET_DETECT:
        case MSG_KEYUP_HEADSET_DETECT:
        MMK_SetCurCtrlHighligAction(FALSE);
        if(DROPDOWN_STATE_NONE == s_dropdown.state)
        {
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
        break;
        */
        
    case MSG_TIMER:
        {
            uint8 timer_id = *(uint8*)param;
            
            if ( timer_id == s_dropdown.tp_move_timer_id )
            {                    
                StopTpMoveTimer();
                
                StartTpMoveTimer(win_id);
                
#if 1
                if ( MMK_TP_SLIDE_FLING == s_dropdown.slide_state )
                {
                    if (!s_dropdown.is_tp_down)
                    {
                        HandleDropDownSlide(win_id);
                    }
                }
#endif
            }
            else if ( timer_id == s_dropdown.tp_redraw_timer_id )
            {               
                //                    GUI_POINT_T point = {0};
                MMI_TP_STATUS_E tp_status = MMI_TP_NONE;
                
                StopTpRedrawTimer();
                StartTpRedrawTimer(win_id);
                MMK_GetLogicTPMsg(&tp_status, &point);
                //                    SCI_TRACE_LOW("HandleDropDownWinMsg() tp_redraw_timer_id(%d, %d)", point.x, point.y);
                HandleDropDownTPMove(win_id, &point);
            }
            else if ( timer_id == s_dropdown.update_redraw_timer_id )                
            {                
                StopUpdateRedrawTimer();
                if (CheckNeedUpdateTimer())
                {
                    StartUpdateRedrawTimer(win_id);
                }
                ReadNotifytoListBox(ctrl_id, TRUE);
                
                if(!GUILIST_IsTpDown(ctrl_id))
                {
                    MMK_SendMsg(win_id, MSG_FULL_PAINT, 0);
                }
            }
        }
        break;
        
        //        case MSG_CTL_PIANT_DONE:
    case MSG_END_FULL_PAINT:            
        //SCI_TRACE_LOW:"HandleDropDownWinMsg, MSG_CTL_PIANT_DONE"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_2346_112_2_18_2_14_53_5,(uint8*)"");
        
        SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);
        
        AppendLayer();  
        
        if(s_dropdown.auto_slide)
        {
            StartFling(win_id);
            
            s_dropdown.auto_slide = FALSE;
        }
        break;
        
    case MSG_CTL_OK:
    case MSG_CTL_PENOK:
        {
            MMIDROPDOWN_NOTIFY_CALLBACK    callback = PNULL;
            uint16 notify_index = 0;
            
            cur_select_item = GUILIST_GetCurItemIndex(ctrl_id);
            
            notify_index = ConvertListIndexToNotifyIndex(cur_select_item);
            
            if (notify_index < DROPDOWN_NOTIFY_MAX)
            {
                if(s_notify_list[notify_index].is_exist)
                {                   
                    callback = GetNotifyCallBack(s_notify_list[notify_index].notify_type);
                    
                    if(PNULL != callback)
                    {
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
                        //#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
                        typedef BOOLEAN(*MMIDROPDOWN_NOTIFY_CALLBACK_WRE)(void *app_handle);
                        
                        if (DROPDOWN_RUNNING_WRE_APP_START <= s_notify_list[notify_index].notify_type && s_notify_list[notify_index].notify_type <= DROPDOWN_RUNNING_WRE_APP_END)
                        {
                            uint32  appid;
                            BOOLEAN bRet;
                            WREAPP_INFO_T wreApp;
                            void *app_handle;
                            
                            appid = s_notify_list[notify_index].notify_type - DROPDOWN_RUNNING_WRE_APP_START;
                            bRet = WRE_GetAppInfo(appid,&wreApp);
                            app_handle = wreApp.app_instance;
                            ((MMIDROPDOWN_NOTIFY_CALLBACK_WRE)callback)(app_handle);
                        }
                        else
#endif
                            callback();
                        
                        MMK_CloseWin(win_id);
                    }                    
                }
            }
        }
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMIDROPDOWNWIN_Close(TRUE);
        //            if(DROPDOWN_STATE_NONE == s_dropdown.state)
        //            {
        //                s_dropdown.state = DROPDOWN_STATE_CLOSE;
        //                
        //                s_dropdown.slide_state = MMK_TP_SLIDE_FLING;
        //
        //                s_dropdown.slide_dir = SLIDE_DIR_UP;
        //
        //                StartTpMoveTimer(win_id);
        //            }
        break;
        
    case MSG_DROPDOWNWIN_CLOSE:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CLOSE_WINDOW:
        //当前页
        s_cur_page = s_dropdown.cur_page;
#ifdef UI_P3D_SUPPORT
        MMI_Enable3DEffect(MMI_3D_EFFECT_DROPDOWN);
#endif  
        GUIWIN_SetStbItemVisible(MMI_WIN_TEXT_DATE, FALSE);
        
        StopUpdateRedrawTimer();
        StopFling(win_id);
        
        UILAYER_RELEASELAYER(&s_dropdown.dropdown_layer);   /*lint !e506 !e774*/
        UILAYER_RELEASELAYER(&s_dropdown.statusbar_layer);   /*lint !e506 !e774*/
        //            UILAYER_ReleaseLayer(&s_dropdown.dropdown_layer);
        //            
        //            s_dropdown.dropdown_layer.block_id = UILAYER_NULL_HANDLE;
        //            
        //            UILAYER_ReleaseLayer(&s_dropdown.statusbar_layer);
        //            
        //            s_dropdown.statusbar_layer.block_id = UILAYER_NULL_HANDLE;
        
        GUIWIN_SetStbDropDownState(win_id, FALSE);
        
        GUICTRL_SetLcdDevInfo(stb_ctr_ptr, MMITHEME_GetDefaultLcdDev());
        break;
        
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return result;
}

LOCAL void InitDropDownWin(
                        MMI_HANDLE_T win_id
                        )
{       
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);
    
    CreateDropDownLayer(win_id, &s_dropdown.dropdown_layer);

    CreateStatusBarLayer(win_id, &s_dropdown.statusbar_layer);       

    s_dropdown.width = lcd_width;

    s_dropdown.height = lcd_height -MMI_STATUSBAR_HEIGHT;    
    
    s_dropdown.bottom = MMI_STATUSBAR_HEIGHT + MMIDROPDOWN_SCROLLKEY_HEIGHT;

    s_dropdown.top = s_dropdown.bottom - s_dropdown.height;

    SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);           
}

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutIconRect(
                                      MMI_HANDLE_T  win_id,
                                      uint32 index
                                      )
{
    GUI_RECT_T item_rect = CalcShortcutItemRect(win_id, index);
    GUI_RECT_T icon_rect = {0};
    MMI_IMAGE_ID_T image_id = 0;
    uint16 image_width = 0;
    uint16 image_height = 0;

    image_id = s_shortcut_data.item[index].open_image_id;
    GUIRES_GetImgWidthHeight(&image_width, &image_height, image_id, win_id);
    icon_rect.top = item_rect.top+MMIDROPDOWN_SHORTCUT_ICON_Y_OFFSET;
    icon_rect.bottom = icon_rect.top+image_height-1;
    //左右居中
    icon_rect.left = (item_rect.left+item_rect.right-image_width)/2;
    icon_rect.right = (icon_rect.left+image_width-1);

    return icon_rect;
}

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutTotalRect(
                                       MMI_HANDLE_T  win_id
                                       )
{
    BOOLEAN  is_landscape = MMK_IsWindowLandscape(win_id);
    GUI_RECT_T  rect_v = MMIDROPDOWN_SHORTCUT_RECT_V;
    GUI_RECT_T  rect_h = MMIDROPDOWN_SHORTCUT_RECT_H;
    GUI_RECT_T toal_rect = {0};
    
    if(is_landscape)
    {
        toal_rect = rect_h;
    }
    else
    {
        toal_rect = rect_v;
    }
    return toal_rect;
}

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutItemRect(
                                      MMI_HANDLE_T  win_id,
                                      uint32 index
                                      )
{
    GUI_RECT_T total_rect = CalcShortcutTotalRect(win_id);
    GUI_RECT_T item_rect = {0};
    uint16 item_width = 0;
    
    item_rect.top = total_rect.top;
    item_rect.bottom = total_rect.bottom;
    item_width = (total_rect.right+1-total_rect.left)/MMIDROPDOWN_SHORTCUT_NUM;//for only num display DROPDOWN_SHORTCUT_MAX;
    item_rect.left = total_rect.left + index * item_width;  /*lint !e737*/
    if (index < (DROPDOWN_SHORTCUT_MAX-1))
    {
        item_rect.right = item_rect.left+item_width-1;
    }
    else
    {
        item_rect.right = total_rect.right;
    }
    return item_rect;
}

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutBarRect(
                                     MMI_HANDLE_T  win_id,
                                     uint32 index
                                     )
{
    GUI_RECT_T item_rect = CalcShortcutItemRect(win_id, index);
//    GUI_RECT_T icon_rect = {0};
    GUI_RECT_T bar_rect = {0};
    MMI_IMAGE_ID_T image_id = 0;
    uint16 image_width = 0;
    uint16 image_height = 0;

    image_id = s_shortcut_data.bar_close_image_id;
    GUIRES_GetImgWidthHeight(&image_width, &image_height, image_id, win_id);

    bar_rect.top = item_rect.top+MMIDROPDOWN_SHORTCUT_BAR_Y_OFFSET;
    bar_rect.bottom = bar_rect.top+image_height-1;
    //左右居中
    bar_rect.left = (item_rect.left+item_rect.right-image_width)/2;
    bar_rect.right = (bar_rect.left+image_width-1);
    return bar_rect;
}

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcShortcutSplitLineRect(
                                     MMI_HANDLE_T  win_id,
                                     uint32 index
                                     )
{
    GUI_RECT_T item_rect = CalcShortcutItemRect(win_id, index);
//    GUI_RECT_T icon_rect = {0};
    GUI_RECT_T split_line_rect = {0};
    MMI_IMAGE_ID_T image_id = 0;
    uint16 image_width = 0;
    uint16 image_height = 0;

    image_id = s_shortcut_data.split_line_image_id;
    GUIRES_GetImgWidthHeight(&image_width, &image_height, image_id, win_id);

    split_line_rect.top = item_rect.top;
    split_line_rect.bottom = item_rect.bottom;
    split_line_rect.left = item_rect.right-image_width/2;
    split_line_rect.right = (split_line_rect.left+image_width-1);
    return split_line_rect;
}

/*****************************************************************************/
//  Description : Init dropdown ShortCut
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void InitDropDownShortCut(MMI_HANDLE_T win_id)
{  
    uint32 i = 0;

    s_shortcut_data.bg_image_id = IMAGE_DROPDOWN_SHORTCUT_BG;
    s_shortcut_data.rect = CalcShortcutTotalRect(win_id);
    s_shortcut_data.bar_close_image_id = IMAGE_DROPDOWN_SHORTCUT_BAR_OFF;
    s_shortcut_data.bar_open_image_id = IMAGE_DROPDOWN_SHORTCUT_BAR_ON;
    s_shortcut_data.split_line_image_id = IMAGE_DROPDOWN_SHORTCUT_SPLIT_LINE;
    s_shortcut_data.button_bg_rect = CalcPageButtonRect();
    s_shortcut_data.button_notify_rect = s_shortcut_data.button_split_rect = s_shortcut_data.button_running_rect = s_shortcut_data.button_bg_rect;
    s_shortcut_data.button_notify_rect.right = (s_shortcut_data.button_bg_rect.left + s_shortcut_data.button_bg_rect.right)/2 -MMIDROPDOWN_PAGEBUTTON_SPLIT_WIDTH/2;
    s_shortcut_data.button_split_rect.left = s_shortcut_data.button_notify_rect.right + 1;
    s_shortcut_data.button_split_rect.right = s_shortcut_data.button_split_rect.left + MMIDROPDOWN_PAGEBUTTON_SPLIT_WIDTH;
    s_shortcut_data.button_running_rect.left = s_shortcut_data.button_split_rect.right + 1 ;
    for (i = 0; i < (DROPDOWN_SHORTCUT_MAX-1); i++)
    {
        s_shortcut_data.split_rect[i] = CalcShortcutSplitLineRect(win_id, i);
    }
    //init wifi
#ifdef WIFI_SUPPORT
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].close_image_id = IMAGE_DROPDOWN_SHORTCUT_WIFI_OFF;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].open_image_id = IMAGE_DROPDOWN_SHORTCUT_WIFI_ON;		
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].item_handle_func = Shortcut_OpenWifi;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].get_state_func = Shortcut_SetWifiState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_WIFI);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_WIFI);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_WIFI);
#endif
    //init bt
#ifdef BLUETOOTH_SUPPORT
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].close_image_id = IMAGE_DROPDOWN_SHORTCUT_BT_OFF;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_BT_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].item_handle_func = Shortcut_OpenBt;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].get_state_func = Shortcut_SetBtState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_BT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_BT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_BT);
#endif
/*
    // init flymode
    s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].close_image_id = IMAGE_DROPDOWN_SHORTCUT_FLYMODE_OFF;
    s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_FLYMODE_ON;
    s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].item_handle_func = Shortcut_SwitchFlymode;
    s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].get_state_func = Shortcut_SetFlymode;
    s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].is_wait = FALSE;
    if(is_landscape)
    {
        s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].rect = flymode_rect_h;
    }
    else
    {
         s_shortcut_item_t[DROPDOWN_SHORTCUT_FLYMODE].rect = flymode_rect_v;
    } 
    */
    //init backlight
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].close_image_id = NULL;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_BACKLIGHT_1;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].is_open= TRUE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].item_handle_func = Shortcut_SetBacklight;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].get_state_func = Shortcut_SetBackLightState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_BACKLIGHT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_BACKLIGHT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_BACKLIGHT);
    
    //init silent
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].close_image_id = IMAGE_DROPDOWN_SHORTCUT_SILENT_OFF;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SILENT_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].item_handle_func = Shortcut_OpenSilent;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].get_state_func = Shortcut_SetSilentState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_SILENT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_SILENT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_SILENT);

#ifdef MMI_POWER_SAVING_MODE_SUPPORT
     //init save mode
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].close_image_id = IMAGE_DROPDOWN_SHORTCUT_SAVEMODE_OFF;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SAVEMODE_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].item_handle_func = Shortcut_SetSaveMode;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].get_state_func = Shortcut_SetSaveModeState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_SAVEMODE);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_SAVEMODE);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_SAVEMODE);
#endif

    //init fly mode
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].close_image_id = IMAGE_DROPDOWN_SHORTCUT_FLYMODE_OFF;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_FLYMODE_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].item_handle_func = Shortcut_SetFlyMode;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].get_state_func = Shortcut_SetFlyModeState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_FLYMODE);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_FLYMODE);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_FLYMODE);

    //init screen lock mode
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].close_image_id = IMAGE_DROPDOWN_SHORTCUT_SCREENLOCK_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SCREENLOCK_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].is_open= TRUE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].item_handle_func = Shortcut_SetPhoneLockMode;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].get_state_func = Shortcut_SetPhoneLockModeState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_SCREENLOCK);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_SCREENLOCK);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_SCREENLOCK].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_SCREENLOCK);


    //init data connect mode
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].close_image_id = IMAGE_DROPDOWN_SHORTCUT_DATACONNECT_OFF;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_DATACONNECT_ON;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].item_handle_func = Shortcut_SetDataConnectMode;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].get_state_func = Shortcut_SetDataConnectModeState;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].is_wait = FALSE;
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].rect = CalcShortcutItemRect(win_id, DROPDOWN_SHORTCUT_DATACONNECT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].icon_rect = CalcShortcutIconRect(win_id, DROPDOWN_SHORTCUT_DATACONNECT);
    s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].bar_rect = CalcShortcutBarRect(win_id, DROPDOWN_SHORTCUT_DATACONNECT);

    
}

/*****************************************************************************/
//  Description : Handle ShortCut
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void  HandleShortCutFun(
                        MMI_WIN_ID_T        win_id, 
                        GUI_POINT_T        *tp_point_ptr
                        )
{
    uint32 i = 0;

    for(i = 0; i< DROPDOWN_SHORTCUT_MAX; i++)
    {
        if(GUI_PointIsInRect(*tp_point_ptr, s_shortcut_data.item[i].rect))
        {
            if(PNULL != s_shortcut_data.item[i].item_handle_func)
            {
                s_shortcut_data.item[i].item_handle_func();
            }            
        }
    }

    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL );
}

#ifdef WIFI_SUPPORT
/*****************************************************************************/
//  Description : ShortCut Wifi
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_OpenWifi(void)
{
    MMIWIFI_NOTIFY_INFO_T notify_info = {0};

    if(s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_open)
    {
        MMIAPIWIFI_SetInfo(FALSE);
        MMIAPIWIFI_Off();
    }
    else
    {
        notify_info.module_id = MMI_MODULE_DROPDOWNWIN;
#ifdef WIFI_SUPPORT
        notify_info.notify_func = PowerOnWifiCallback;
#else
        notify_info.notify_func = PNULL;
#endif
        MMIAPIWIFI_RegNotifyInfo(&notify_info);

        if (MMIAPIWIFI_On(TRUE))
//        if (MMIAPIWIFI_On(FALSE))
        {
//            MMIPUB_OpenAlertWarningWin(STXT_OK); 
            s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_wait = TRUE;
        }
    }
}

#ifdef WIFI_SUPPORT
/*****************************************************************************/
//  Description : ShortCut Wifi
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void PowerOnWifiCallback(MMIWIFI_NOTIFY_EVENT_E event_id,MMIWIFI_NOTIFY_PARAM_T* param_ptr)
{     
    if ((MMIWIFI_NOTIFY_EVENT_ON == event_id)
        && WIFISUPP_RESULT_SUCC != param_ptr->result)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_wait = FALSE;
    }

    if(MMK_IsFocusWin(MMI_DROPDOWN_WIN_ID))
    {
        if(DROPDOWN_STATE_NONE == s_dropdown.state)
        {
            MMK_PostMsg(MMI_DROPDOWN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
}
#endif

/*****************************************************************************/
//  Description : ShortCut Wifi
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetWifiState(void)
{
    if (MMIWIFI_STATUS_OFF != MMIAPIWIFI_GetStatus()) 
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_wait = FALSE;
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_WIFI].is_open = FALSE;
    }
}
#endif

#ifdef BLUETOOTH_SUPPORT
/*****************************************************************************/
//  Description : ShortCut BT
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_OpenBt(void)
{
    if(s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].is_open)
    {
        MMIBT_SetOpertor(MMIBT_OPC_NOT_CHANGE, MMIBT_OPC_POWER_ONOFF);
        MMIBT_OpenBluetooth(FALSE, TRUE);
        MMIBT_CloseAllWindow();        
    }
    else
    {
        MMIBT_SetOpertor(MMIBT_OPC_NOT_CHANGE, MMIBT_OPC_POWER_ONOFF);
        MMIBT_OpenBluetooth(TRUE, TRUE);
    }
}

/*****************************************************************************/
//  Description : ShortCut BT
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetBtState(void)
{	
    if(MMIAPIBT_IsBtOn())
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].is_open = TRUE;
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_BT].is_open = FALSE;
    }	
}
#endif

/*****************************************************************************/
//  Description : ShortCut Flymode
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
//LOCAL void Shortcut_SwitchFlymode(void)
//{
//    if(s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].is_open)
//    {
//        MMIAPISET_SetFlyModeWholeFlow(FALSE);
//    }
//    else
//    {
//        MMIAPISET_SetFlyModeWholeFlow(TRUE);
//    }
//}

/*****************************************************************************/
//  Description : ShortCut Flymode
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
//LOCAL void Shortcut_SetFlymode(void)
//{
//    if(MMIAPISET_GetFlyMode())
//    {
//        s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].is_open = TRUE;
//    }
//    else
//    {
//        s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].is_open = FALSE;
//    }
//}

/*****************************************************************************/
//  Description : ShortCut Silent
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_OpenSilent(void)
{
    if(s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].is_open)
    {
        MMIAPIENVSET_ChangeToNoiseMode();
    }
    else
    {
        MMIAPIENVSET_ChangeToSilentMode();
    }

}

/*****************************************************************************/
//  Description : ShortCut Silent
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetSilentState(void)
{    
    uint8           envset_mode     = 0;
    
    envset_mode = MMIAPIENVSET_GetActiveModeId();
#if !defined DPHONE_SUPPORT && defined MMIENVSET_EARPHONE_BLUETOOTH_SUPPORT	
    if (envset_mode == MMIENVSET_EARPHONE_MODE)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SET_EARPHONE;
        return;
    }
    else if (envset_mode == MMIENVSET_BLUETOOTH_MODE)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SET_BT_EARPHONE;
        return;
    }
    else 
#endif
    if((envset_mode== MMIENVSET_MEETING_MODE) || (envset_mode== MMIENVSET_SILENT_MODE))
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SILENT_ON;
        //s_silent_pre_mode = envset_mode;
        return;        
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].is_open = FALSE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SILENT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SILENT_ON;
        //s_noisy_pre_mode = envset_mode;
        return;        
    }
}

/*****************************************************************************/
//  Description : ShortCut BackLight
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetBacklight(void)
{
    uint32    real_contrast     =   0;

    if(s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].open_image_id == IMAGE_DROPDOWN_SHORTCUT_BACKLIGHT_1)
    {
        real_contrast = 4;
    }
    else if(s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].open_image_id == IMAGE_DROPDOWN_SHORTCUT_BACKLIGHT_2)
    {
        real_contrast = 7;
    }
    else
    {
        real_contrast = 0;
    }

    MMIAPISET_SetCurrentContrast(real_contrast);      
    MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, real_contrast);
    MMIAPISET_SetMainLCDContrast();
}

/*****************************************************************************/
//  Description : ShortCut BackLight
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetBackLightState(void)
{
    uint32 backlight = 0;
    backlight = MMIAPISET_GetCurrentContrast();
    if(backlight < 3)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_BACKLIGHT_1;
    }
    else if((3 <= backlight ) && (backlight< 6))
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_BACKLIGHT_2;
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_BACKLIGHT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_BACKLIGHT_3;
    }	
}


/*****************************************************************************/
//  Description : Shortcut_SetSaveMode Silent
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetSaveMode(void)
{
#ifdef MMI_POWER_SAVING_MODE_SUPPORT
    BOOLEAN is_switch = MMIAPISET_GetPowerSavingSwitch();
    MMISET_POWER_SETTING_ITEM_T power_sav_set = MMIAPISET_GetPowerSavingSetting();
    
    is_switch = !is_switch;        
    if(is_switch)//开启省电模式
    {
        MMIAPISET_SetPowerSavingSwitch(TRUE);
#ifdef MMI_VIDEOWALLPAPER_SUPPORT 
        {
            //MMISET_POWER_SETTING_ITEM_T power_sav_setting = MMIAPISET_GetPowerSavingSetting();

            if((power_sav_set.is_fb_anim_wallpaper == TRUE) && MMIAPISETVWP_IsOpen())
            {
                MMIAPISET_ResetDefaultWallpaper();
            }
        }
 #endif            
    }
    else//关闭省电模式
    {
        power_sav_set.is_active = FALSE;
        MMIAPISET_SetPowerSavingSetting(power_sav_set);
        MMIAPISET_SetPowerSavingSwitch(is_switch);
        MMIAPIENVSET_ResetActModeOtherRingSet();
    }
    MMK_SendMsg(MMISET_POWER_SAVE_WIN_ID, MSG_SET_UPDATE_POWER_SAVE_SETTING, PNULL);
#endif
}

/*****************************************************************************/
//  Description :  Shortcut_SetSaveModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetSaveModeState(void)
{
#ifdef MMI_POWER_SAVING_MODE_SUPPORT
    BOOLEAN is_switch = TRUE;
    is_switch = MMIAPISET_GetPowerSavingSwitch();
    if(is_switch)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SAVEMODE_ON;
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].is_open = FALSE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_SAVEMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_SAVEMODE_ON;
    }
#endif
}

/*****************************************************************************/
//  Description : Shortcut_SetFlyMode Silent
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetFlyMode(void)
{
    BOOLEAN    is_fly_mode_on  = MMIAPISET_GetFlyMode();
    uint16     i               = 0;

    if(is_fly_mode_on)
    {
    	BOOLEAN is_ps_deactive = TRUE;
    	
    	for(i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    	{
    		if(!MMIAPIPHONE_GetIsPsDeactiveCnf((MN_DUAL_SYS_E)i))
    		{
    			is_ps_deactive = FALSE;
    			break;
    		}
    	}
    	if (is_ps_deactive)
    	{
    		is_fly_mode_on = !is_fly_mode_on;
    		
    		MMIAPISET_SetFlyMode(is_fly_mode_on);
    		
    		MMIAPISET_SetIsQueryCfu(FALSE,MMI_DUAL_SYS_MAX);
    		
    		// 激活协议栈
    		MMIAPIPHONE_StartUpPsAccordingToSetting(FALSE, TRUE);
    		MMIAPIMMS_RestartAutodownMMS();
    	}
    	else
    	{
    		MMIPUB_OpenAlertWarningWin( TXT_SYS_WAITING_AND_TRY_LATER);
    		
    		//MMK_CloseWin(win_id);
    		return;
    	}
    }
    else
    {
    	uint32 dual_sys =MN_DUAL_SYS_1;
    	BOOLEAN is_ps_ready = TRUE;
    	MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    	
    	for (dual_sys = MN_DUAL_SYS_1; dual_sys < MMI_DUAL_SYS_MAX; dual_sys++)
    	{
    		if ((e_dualsys_setting & (0x01 << dual_sys))
    			&& MMIAPIPHONE_GetSimExistedStatus(dual_sys))
    		{
    			is_ps_ready = MMIAPIPHONE_GetIsPsReady(dual_sys);
    			
    			if (!is_ps_ready)
    			{
    				return;
    			}
    		}
    	}
        
    	if (is_ps_ready)
    	{
    		if (MMISET_GetIsQueryOpenFlyMode())
    		{
    			//MMIPUB_OpenQueryWinByTextId(TXT_FLY_MODE_PS_ALERT,IMAGE_PUBWIN_QUERY,PNULL,MMISET_HandleFlightmodeWinMsg);
    			return;
    		}
    		else
    		{
    			is_fly_mode_on = !is_fly_mode_on;
    			
    			MMIAPISET_SetFlyMode(is_fly_mode_on);
    			
    			MMIAPIPHONE_DeactivePsAccordingToSetting();
    		}
    	}
    	else
    	{
    		MMIPUB_OpenAlertWarningWin( TXT_SYS_WAITING_AND_TRY_LATER);
    		
    		//MMK_CloseWin(win_id);
    		return;
    	}
    }
}

/*****************************************************************************/
//  Description :  Shortcut_SetFlyModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetFlyModeState(void)
{
    BOOLEAN    is_fly_mode_on  = MMIAPISET_GetFlyMode();
    
    if(is_fly_mode_on)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_FLYMODE_ON;
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].is_open = FALSE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_FLYMODE].open_image_id = IMAGE_DROPDOWN_SHORTCUT_FLYMODE_ON;
    }
}


/*****************************************************************************/
//  Description :  Shortcut_SetPhoneLockMode
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetPhoneLockMode(void)
{
    MMIDEFAULT_CloseAllLight();
    MMIDROPDOWNWIN_Close(TRUE);
}


/*****************************************************************************/
//  Description :  Shortcut_SetFlyModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetPhoneLockModeState(void)
{    
    return;
}

/*****************************************************************************/
//  Description : Shortcut_SetDataConnectMode
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetDataConnectMode(void)
{
#ifdef MMI_GPRS_SWITCH_SUPPORT    
    //[----
    //add switch connection SetFun() here 需要wancan 提供接口
    //-----]
    MMIAPICONNECTION_SetGPRSSwitchStatus((MMICONNECTION_SETTING_GPRS_SWITCH_E)0);
    return;
#endif
}

/*****************************************************************************/
//  Description : Shortcut_SetDataConnectModeState
//  Global resource dependence : none
//  Author: CBK
//  Note: 
/*****************************************************************************/
LOCAL void Shortcut_SetDataConnectModeState(void)
{
    BOOLEAN    is_real_contrast  = TRUE;
    
    //[----
    //   需要wancan 提供接口
    // is_real_contrast = GetFun();  
    //-----]
#ifdef MMI_GPRS_SWITCH_SUPPORT    
    is_real_contrast = (BOOLEAN)MMIAPICONNECTION_GetGPRSSwitchStatus();
    if(is_real_contrast)
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].is_open = TRUE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_DATACONNECT_ON;
    }
    else
    {
        s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].is_open = FALSE;
        s_shortcut_data.item[DROPDOWN_SHORTCUT_DATACONNECT].open_image_id = IMAGE_DROPDOWN_SHORTCUT_DATACONNECT_ON;
    }
#endif
}



/*****************************************************************************/
//  Description : Display dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayNotifyTitle(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
//    MMI_STRING_T        count_string = {0};
//    GUISTR_STATE_T      text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE|GUISTR_STATE_ALIGN_BY_DIR;	
////    GUISTR_STATE_T      text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE;	
////    GUISTR_STATE_T      text_state = GUISTR_STATE_INVERSE | GUISTR_STATE_SINGLE_LINE;
//    GUISTR_STYLE_T      str_style = {0};
//    GUI_RECT_T          notify_title_rect = MMIDROPDOWN_NOTIFY_TITLE_RECT_V;
//
//    str_style.angle = ANGLE_0;
//    str_style.align = ALIGN_LVMIDDLE;
//    str_style.font = SONG_FONT_16;
//    str_style.font_color = MMI_BLACK_COLOR;
//
//    //title bg
//    GUIRES_DisplayImg(
//        PNULL,
//        &notify_title_rect,
//        PNULL,
//        win_id,
//        IMAGE_CONTROL_LIST_SPLIT_ITEM_BG,
//        lcd_dev_ptr
//        );
////    LCD_FillRect(lcd_dev_ptr, notify_title_rect, MMI_WHITE_COLOR);
//    MMI_GetLabelTextByLang(TXT_DROPDOWN_NOTIFY, &count_string);
//    GUISTR_DrawTextToLCDInRect(lcd_dev_ptr,&notify_title_rect,&notify_title_rect,&count_string,&str_style,text_state,GUISTR_TEXT_DIR_AUTO);
    GUI_LCD_DEV_INFO lcd_dev_info = *lcd_dev_ptr;
    GUI_RECT_T    notify_title_rect = {0};
    GUI_RECT_T    notify_title_rect_v = MMIDROPDOWN_NOTIFY_TITLE_RECT_V;
    GUI_RECT_T    notify_title_rect_h = MMIDROPDOWN_NOTIFY_TITLE_RECT_H;
    GUISTR_STYLE_T    text_style = {0};
    GUISTR_STATE_T    text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE|GUISTR_STATE_ELLIPSIS;
    MMI_STRING_T    notify_title_str     = {0};

    if(MMK_IsWindowLandscape(win_id))   
    {
        notify_title_rect = notify_title_rect_h;
    }
    else
    {
        notify_title_rect = notify_title_rect_v;
    }

    //根据NETWORK区域大小，重新调整位置
//    notify_title_rect.top = s_dropdown.network_rect.bottom;
//
//    notify_title_rect.bottom = notify_title_rect.top + MMIDROPDOWN_NOTIFY_TITLE_HEIGHT;

    if (UILAYER_IsLayerActive(&lcd_dev_info))
    {        
//#if defined MMI_PDA_SUPPORT
        GUIRES_DisplayImg(
            PNULL,
            &notify_title_rect,
            PNULL,
            win_id,
            IMAGE_CONTROL_LIST_SPLIT_ITEM_BG,
            &lcd_dev_info
            );
//#endif
        text_style.effect 		= FONT_EFFECT_REVERSE_EDGE; 
        text_style.angle 		= ANGLE_0;
        text_style.align 		= ALIGN_LVMIDDLE;//ALIGN_LEFT; //ALIGN_HVMIDDLE;
        text_style.font 		= SONG_FONT_16;
        text_style.font_color 	= MMI_BLACK_COLOR;//MMI_WHITE_COLOR;

        if(0 < GetNotifyNumber())
        {
            MMI_GetLabelTextByLang(STXT_NOTIFICATION/*STXT_NOTIFICATION*/, &notify_title_str);
        }
        else
        {
            MMI_GetLabelTextByLang(STXT_NO_NOTIFICATION/*STXT_NO_NOTIFICATION*/, &notify_title_str);
        }

//        notify_title_rect.left += MMIDROPDOWN_NOTIFY_TITLE_TEXT_GAP;

        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &notify_title_rect,
            &notify_title_rect,
            &notify_title_str,
            &text_style,
            text_state,
            0
            );
    }
}

/*****************************************************************************/
//  Description : Display dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayNotifyPage(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
    uint32 i = 0;
    
    for (i = 0; i < DROPDOWN_SHORTCUT_MAX ; i++)
    {
        if(PNULL != s_shortcut_data.item[i].get_state_func)
        {
            s_shortcut_data.item[i].get_state_func();
        }
    }
    DisplayShortcut(win_id, lcd_dev_ptr);
    DisPlayNetWork(win_id, lcd_dev_ptr);
    DisplayNotifyTitle(win_id, lcd_dev_ptr);
}


/*****************************************************************************/
//  Description : Display dropdown win
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayRunningPage(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
}

/*****************************************************************************/
//  Description : Display dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisplayDropDownWin(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
//    uint32 i = 0;
    
    DisplayDropDownBg(win_id, lcd_dev_ptr);
    DisplayPageButton(win_id, lcd_dev_ptr);
    
    switch (s_dropdown.cur_page)
    {
    case DROPDOWN_PAGE_NOTIFY:
        DisplayNotifyPage(win_id, lcd_dev_ptr);
        break;

    case DROPDOWN_PAGE_RUNNING:
        DisplayRunningPage(win_id, lcd_dev_ptr);
        break;

    default:
        break;
    }
             
}

/*****************************************************************************/
//  Description : Display dropdown bg
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayDropDownScrollBar(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
    GUI_RECT_T    bar_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = *lcd_dev_ptr;
    int16 offset_x = 0;
    int16 offset_y = 0;
    UILAYER_INFO_T layer_info = {0};
    GUI_RECT_T statusbar_rect = {0};
    IGUICTRL_T    *stb_ctr_ptr  = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id, MMITHEME_GetStatusBarCtrlId());    

    GUICTRL_GetRect(stb_ctr_ptr, &statusbar_rect);
    UILAYER_GetLayerPosition(lcd_dev_ptr, &offset_x, &offset_y);
    UILAYER_GetLayerInfo(lcd_dev_ptr, &layer_info);
    bar_rect.left = offset_x;
    bar_rect.right = (offset_x+layer_info.mem_width-1);
    bar_rect.bottom = (offset_y+layer_info.layer_height-1);
    bar_rect.top = bar_rect.bottom - MMIDROPDOWN_SCROLLKEY_HEIGHT + 1;   
    if (UILAYER_IsLayerActive(&lcd_dev_info))
    {
        GUIRES_DisplayImg(
            PNULL,
            &bar_rect,
            PNULL,
            win_id,
            IMAGE_DROPDOWN_BAR,
            &lcd_dev_info
            );
        //清除statusbar rect
        UILAYER_ClearRect(&lcd_dev_info, statusbar_rect);
    }
}

/*****************************************************************************/
//  Description : Display dropdown bg
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisplayDropDownBg(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
    GUI_RECT_T    bg_rect = {0};
    GUI_RECT_T    bar_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = *lcd_dev_ptr;
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);  

    bg_rect.top = 0;//MMI_STATUSBAR_HEIGHT;

    bg_rect.bottom = lcd_height -1;

    bg_rect.left = 0;

    bg_rect.right = lcd_width -1;

    bar_rect = bg_rect;

    bar_rect.top = bg_rect.bottom - MMIDROPDOWN_SCROLLKEY_HEIGHT + 1;
        
    if (UILAYER_IsLayerActive(&lcd_dev_info))
    {
//        LCD_FillRect(&lcd_dev_info, bg_rect, MMI_WHITE_COLOR);
//        LCD_FillRect(&lcd_dev_info, bar_rect, MMI_GRAY_COLOR);
        GUIRES_DisplayImg(
            PNULL,
            &bg_rect,
            PNULL,
            win_id,
            IMAGE_COMMON_BG,//IMAGE_DROPDOWN_BG,
            &lcd_dev_info
            );

        GUIRES_DisplayImg(
            PNULL,
            &bar_rect,
            PNULL,
            win_id,
            IMAGE_DROPDOWN_BAR,
            &lcd_dev_info
            );
    }
}

/*****************************************************************************/
//  Description : calc page button rect
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T CalcPageButtonRect(void)
{
    GUI_RECT_T rect = {0};
    GUI_RECT_T full_rect = {0};

    full_rect = MMITHEME_GetFullScreenRect();
    rect.bottom = full_rect.bottom - MMIDROPDOWN_SCROLLKEY_HEIGHT;
    rect.top = rect.bottom - MMIDROPDOWN_PAGEBUTTON_HEIGHT;
    rect.left = 0;
    rect.right = full_rect.right;
    return rect;
}

/*****************************************************************************/
//  Description : Display dropdown bg
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void DisplayPageButton(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
//    GUI_RECT_T    bg_rect = {0};
//    GUI_RECT_T    bar_rect = {0};
    GUI_RECT_T    button_notify_rect = s_shortcut_data.button_notify_rect;//MMIDROPDOWN_PAGEBUTTON_NOTIFY_RECT_V;
    GUI_RECT_T    button_running_rect = s_shortcut_data.button_running_rect;//MMIDROPDOWN_PAGEBUTTON_RUNNING_RECT_V;
    GUI_RECT_T    button_bg_rect = s_shortcut_data.button_bg_rect;
    GUI_LCD_DEV_INFO lcd_dev_info = *lcd_dev_ptr;
//    uint16    lcd_width = 0;
//    uint16    lcd_height = 0;
    MMI_STRING_T        count_string = {0};
    GUISTR_STATE_T      text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE;	
    GUISTR_STYLE_T      str_style = {0};
//    MMI_IMAGE_ID_T   notify_image = 0;
//    MMI_IMAGE_ID_T   running_image = 0;
    MMI_IMAGE_ID_T   button_hl_image = IMAGE_DROPDOWN_TAB_HIGHLIGHT;
    MMI_IMAGE_ID_T   button_bg_image = IMAGE_DROPDOWN_TAB_DEFAULT;
    MMI_IMAGE_ID_T   button_split_image = IMAGE_DROPDOWN_TAB_SEPARATED;
    GUI_COLOR_T      font_color_hl = 0x2945;
    GUI_COLOR_T      font_color = MMI_WHITE_COLOR;
    GUI_COLOR_T      notify_font_color = 0;
    GUI_COLOR_T      running_font_color = 0;    

    GUI_RECT_T      button_split_rect = s_shortcut_data.button_split_rect;

    //    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);  

//    bg_rect.top = MMI_STATUSBAR_HEIGHT;
//
//    bg_rect.bottom = lcd_height -1;
//
//    bg_rect.left = 0;
//
//    bg_rect.right = lcd_width -1;
//
//    bar_rect = bg_rect;
//
//    bar_rect.top = bg_rect.bottom - MMIDROPDOWN_SCROLLKEY_HEIGHT + 1;    
//    button_bg_rect = CalcPageButtonRect();
//    button_notify_rect = button_running_rect = button_bg_rect;
//    button_notify_rect.right = (button_bg_rect.left + button_bg_rect.right)/2;
//    button_running_rect.left = button_notify_rect.right + 1;
    if (UILAYER_IsLayerActive(&lcd_dev_info))
    {
        str_style.angle = ANGLE_0;
        str_style.align = ALIGN_HVMIDDLE;
        str_style.font = SONG_FONT_16;
        str_style.font_color = MMI_BLACK_COLOR;
        
        //button bg
        GUIRES_DisplayImg(
            PNULL,
            &button_bg_rect,
            PNULL,
            win_id,
            button_bg_image,
            &lcd_dev_info
            );  
        
        //tab split line
        GUIRES_DisplayImg(
            PNULL,
            &button_split_rect,
            PNULL,
            win_id,
            button_split_image,
            &lcd_dev_info
            ); 

        if (DROPDOWN_PAGE_NOTIFY == s_dropdown.cur_page)
        {
            notify_font_color = font_color_hl;
            running_font_color = font_color;
//            notify_image = IMAGE_DROPDOWN_BG_BUTTON_HL;//IMAGE_PUBWIN_BUTTON_HL_PDA;
//            running_image = IMAGE_DROPDOWN_BG_BUTTON;//IMAGE_PUBWIN_BUTTON_PDA;
            GUIRES_DisplayImg(
                PNULL,
                &button_notify_rect,
                PNULL,
                win_id,
                button_hl_image,
                &lcd_dev_info
                );   
        }
        else
        {
            notify_font_color = font_color;
            running_font_color = font_color_hl;
//            notify_image = IMAGE_DROPDOWN_BG_BUTTON;//IMAGE_PUBWIN_BUTTON_PDA;
//            running_image = IMAGE_DROPDOWN_BG_BUTTON_HL;//IMAGE_PUBWIN_BUTTON_HL_PDA;
            GUIRES_DisplayImg(
                PNULL,
                &button_running_rect,
                PNULL,
                win_id,
                button_hl_image,
                &lcd_dev_info
                );  
        }
        MMI_GetLabelTextByLang(STXT_NOTIFICATION, &count_string);
        str_style.font_color = notify_font_color;
        GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&button_notify_rect,&button_notify_rect,&count_string,&str_style,text_state,GUISTR_TEXT_DIR_AUTO);
        
        MMI_GetLabelTextByLang(TXT_DROPDOWN_RUNNING, &count_string);
        str_style.font_color = running_font_color;
        GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&button_running_rect,&button_running_rect,&count_string,&str_style,text_state,GUISTR_TEXT_DIR_AUTO);
    }
}

/*****************************************************************************/
//  Description : Display dropdown shortcut
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DisplayShortcut(
                        MMI_HANDLE_T  win_id,
                        GUI_LCD_DEV_INFO const *lcd_dev_ptr
                        )
{
    GUI_LCD_DEV_INFO lcd_dev_info = *lcd_dev_ptr;
    uint32 i = 0;
    MMI_IMAGE_ID_T   	icon_image_id = 0; 
    MMI_IMAGE_ID_T   	bar_image_id = 0; 
    
    if (UILAYER_IsLayerActive(&lcd_dev_info))
    {        
        //display bg
        GUIRES_DisplayImg(
            PNULL,
            &(s_shortcut_data.rect),
            PNULL,
            win_id,
            s_shortcut_data.bg_image_id,
            &lcd_dev_info
            );
        //display item
        for (i = 0 ; i < DROPDOWN_SHORTCUT_MAX; i++)
        {
            if (s_shortcut_data.item[i].is_open)
            {
                icon_image_id = s_shortcut_data.item[i].open_image_id;
                bar_image_id = s_shortcut_data.bar_open_image_id;
            }	
            else
            {
                icon_image_id = s_shortcut_data.item[i].close_image_id;
                bar_image_id = s_shortcut_data.bar_close_image_id;
            }
            
            //icon
            GUIRES_DisplayImg(
                PNULL,
                &(s_shortcut_data.item[i].icon_rect),
                PNULL,
                win_id,
                icon_image_id,
                &lcd_dev_info
                );
            //bar
            GUIRES_DisplayImg(
                PNULL,
                &(s_shortcut_data.item[i].bar_rect),
                PNULL,
                win_id,
                bar_image_id,
                &lcd_dev_info
                );
            //split line
            if (i < (DROPDOWN_SHORTCUT_MAX-1))
            {
                GUIRES_DisplayImg(
                    PNULL,
                    &(s_shortcut_data.split_rect[i]),
                    PNULL,
                    win_id,
                    s_shortcut_data.split_line_image_id,
                    &lcd_dev_info
                    );            
            }
        }        
    }
}

/*****************************************************************************/
//  Description : get network rect
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL GUI_RECT_T GetNetworkRect(
                                MMI_HANDLE_T  win_id,
                                uint32 sim_index
                                )
{
    GUI_RECT_T 			network_rect = {0};
    GUI_RECT_T 			network_rect_v = MMIDROPDOWN_NETWORK_RECT_V;    //单行的区域
    GUI_RECT_T 			network_rect_h = MMIDROPDOWN_NETWORK_RECT_H;    //单行的区域
    
    if(MMK_IsWindowLandscape(win_id))   
    {
        network_rect = network_rect_h;
#ifndef MMI_MULTI_SIM_SYS_SINGLE              
        if (sim_index & 0x01)
        {
            //奇数
            network_rect.top += MMIDROPDOWN_NETWORK_HEIGHT*(sim_index/2);   /*lint !e737*/
            network_rect.bottom +=MMIDROPDOWN_NETWORK_HEIGHT*(sim_index/2); /*lint !e737*/
            network_rect.left = (network_rect.left+network_rect.right)/2;
        }
        else
        {
            //偶数
            network_rect.top += MMIDROPDOWN_NETWORK_HEIGHT*(sim_index/2);   /*lint !e737*/
            network_rect.bottom +=MMIDROPDOWN_NETWORK_HEIGHT*(sim_index/2); /*lint !e737*/
            network_rect.right = (network_rect.left+network_rect.right)/2+1;
        }
#endif 
    }
    else
    {
        network_rect = network_rect_v;
#ifndef MMI_MULTI_SIM_SYS_SINGLE              
        network_rect.top += MMIDROPDOWN_NETWORK_HEIGHT*sim_index;   /*lint !e737*/
        network_rect.bottom +=MMIDROPDOWN_NETWORK_HEIGHT*sim_index; /*lint !e737*/
#endif    
    }  

    return network_rect;
}

/*****************************************************************************/
//  Description : Display dropdown network
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
#if 1
LOCAL void DisPlayNetWork(
                    MMI_HANDLE_T  win_id,
                    GUI_LCD_DEV_INFO const *lcd_dev_ptr
                    )
{
    GUI_RECT_T 			network_rect = {0};
//    GUI_RECT_T 			network_rect_v = MMIDROPDOWN_NETWORK_RECT_V;   
//    GUI_RECT_T 			network_rect_h = MMIDROPDOWN_NETWORK_RECT_H;  
    MMI_STRING_T            	network_str     = {0};
    GUI_LCD_DEV_INFO     lcd_dev_info    = *lcd_dev_ptr;
    BOOLEAN                 	is_fly_mode_on  = FALSE;
    int                     		i  = 0;
    GUISTR_STYLE_T 			text_style = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE|GUISTR_STATE_ELLIPSIS;	
    wchar 					opn_spn_name[MMIPHONE_MAX_OPER_NAME_LEN] = {0};
    GUISTR_INFO_T	tmp_str_info ={0};
#ifndef MMI_MULTI_SIM_SYS_SINGLE
    wchar *sim_deactivated_str = PNULL;
#endif

//    if(MMK_IsWindowLandscape(win_id))   
//    {
//        network_rect = network_rect_h;
//    }
//    else
//    {
//        network_rect = network_rect_v;
//    }
    
    if(UILAYER_IsLayerActive(&lcd_dev_info))
    {        
        text_style.effect 		= FONT_EFFECT_REVERSE_EDGE; 
        text_style.angle 		= ANGLE_0;
        text_style.align 		= ALIGN_LVMIDDLE;
        text_style.font 		= SONG_FONT_16;
        text_style.font_color 	= MMI_BLACK_COLOR;//MMI_WHITE_COLOR;
        
        // 读取设置模式的值
        is_fly_mode_on = MMIAPISET_GetFlyMode();
        
        for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
        {
            if(is_fly_mode_on)
            {
                MMI_GetLabelTextByLang(TXT_SET_FLY_MODE, &network_str);
            }
            else if (MMIAPIPHONE_GetSimExistedStatus(i))
            {
                if (MMIAPIIDLE_NeedShowIconAccordingToDualSysSetting((MN_DUAL_SYS_E)i))
                {
                    SCI_MEMSET(&network_str, 0x00, sizeof(network_str));
                    network_str.wstr_ptr = (wchar *)opn_spn_name;
                    SCI_MEMSET(network_str.wstr_ptr, 0x00, sizeof(opn_spn_name));
                    
                    MMIAPIPHONE_GetSimAndServiceString((MN_DUAL_SYS_E)i, &network_str);
                }
#ifndef MMI_MULTI_SIM_SYS_SINGLE
                else
                {
                    MMISET_SIM_NAME_T sim_name = {0};
                    MMI_STRING_T sim_deactivated = {0};

                    sim_name = MMIAPISET_GetSimName(i);

                    MMI_GetLabelTextByLang(TXT_SIM_DEACTIVATED, &sim_deactivated);

                    //此处不判断是否需要加入空格，统一都多分配一个空格的存储空间(2bytes)    
                    sim_deactivated_str = (wchar *)SCI_ALLOC_APP(((sim_deactivated.wstr_len + sim_name.wstr_len + 2)*sizeof(wchar)));
                    SCI_MEMSET(sim_deactivated_str, 0x00, ((sim_deactivated.wstr_len + sim_name.wstr_len + 2)*sizeof(wchar)));
                                        
                    MMI_WSTRNCPY(sim_deactivated_str,
                                                (sim_deactivated.wstr_len + sim_name.wstr_len + 1),
                                                sim_name.wstr_arr,
                                                sim_name.wstr_len,
                                                sim_name.wstr_len);
                    
                    // ASCII 码数据，两个字符串中间追加一个空格。
                    if (MMIAPICOM_IsASCIIString(sim_name.wstr_arr,sim_name.wstr_len)
                        && MMIAPICOM_IsASCIIString(sim_deactivated.wstr_ptr ,sim_name.wstr_len))
                    {
                        MMIAPICOM_Wstrcat(sim_deactivated_str, L" "); 
                        MMI_WSTRNCPY(sim_deactivated_str + sim_name.wstr_len + 1,
                                                (sim_deactivated.wstr_len + 1),
                                                sim_deactivated.wstr_ptr,
                                                sim_deactivated.wstr_len,
                                                sim_deactivated.wstr_len);
                        
                        network_str.wstr_ptr = (wchar *)sim_deactivated_str;
                        network_str.wstr_len = sim_deactivated.wstr_len + sim_name.wstr_len + 1 ;
                    }
                    else
                    {
                        MMI_WSTRNCPY(sim_deactivated_str + sim_name.wstr_len ,
                                                (sim_deactivated.wstr_len + 1),
                                                sim_deactivated.wstr_ptr,
                                                sim_deactivated.wstr_len,
                                                sim_deactivated.wstr_len);
                        network_str.wstr_ptr = (wchar *)sim_deactivated_str;
                        network_str.wstr_len = sim_deactivated.wstr_len + sim_name.wstr_len ;
                    }
                }
#endif
            }
            else
            {
                MMI_GetLabelTextByLang(TXT_INSERT_SIM, &network_str);
            }

            GUISTR_GetStringInfo(&text_style, &network_str, text_state,&tmp_str_info);	            

//#ifndef MMI_MULTI_SIM_SYS_SINGLE              
//            if( MN_DUAL_SYS_1  < i ) //每循环遍历到一次，就做一次win_rect的调整.
//            {
//                network_rect.top += MMIDROPDOWN_NETWORK_HEIGHT;
//                network_rect.bottom +=MMIDROPDOWN_NETWORK_HEIGHT;
//            }
//#endif
            network_rect = GetNetworkRect(win_id, i);

            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                &network_rect,
                &network_rect,
                &network_str,
                &text_style,
                text_state,
                0
                );

#ifndef MMI_MULTI_SIM_SYS_SINGLE
            if (PNULL != sim_deactivated_str)
            {
                SCI_FREE(sim_deactivated_str);
                sim_deactivated_str = PNULL;
            }
#endif
        }   
    }    
}
#else
LOCAL void DisPlayNetWork(
                    MMI_HANDLE_T  win_id,
                    GUI_LCD_DEV_INFO const *lcd_dev_ptr
                    )
{
    GUI_RECT_T 			network_rect = {0};
    GUI_RECT_T 			network_rect_v = MMIDROPDOWN_NETWORK_RECT_V;
    GUI_RECT_T 			network_rect_h = MMIDROPDOWN_NETWORK_RECT_H;    
    GUI_RECT_T 			icon_rect = {0};
    GUI_RECT_T 			text_rect = {0};
    MMI_STRING_T            	network_str     = {0};
    GUI_LCD_DEV_INFO     lcd_dev_info    = *lcd_dev_ptr;
    BOOLEAN                 	is_fly_mode_on  = FALSE;
    BOOLEAN                     is_no_sim = TRUE;
    int                     		i  = 0;
    GUISTR_STYLE_T 			text_style = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE|GUISTR_STATE_ELLIPSIS;	
    wchar 					opn_spn_name[MMIPHONE_MAX_OPER_NAME_LEN] = {0};
    GUISTR_INFO_T	tmp_str_info ={0};
#ifndef MMI_MULTI_SIM_SYS_SINGLE
    wchar *sim_deactivated_str = PNULL;
#endif
    MMI_IMAGE_ID_T   sim_image_id = 0;

    if(MMK_IsWindowLandscape(win_id))   
    {
        network_rect = network_rect_h;
    }
    else
    {
        network_rect = network_rect_v;
    }

    s_dropdown.network_rect = network_rect;

    s_dropdown.network_rect.bottom += MMIDROPDOWN_NETWORK_HEIGHT;
    
    if(UILAYER_IsLayerActive(&lcd_dev_info))
    {
        text_style.effect 		= FONT_EFFECT_REVERSE_EDGE; 
        text_style.angle 		= ANGLE_0;
        text_style.align 		= ALIGN_LVMIDDLE;
        text_style.font 		= SONG_FONT_16;
        text_style.font_color 	= MMI_BLACK_COLOR;

        // 读取设置模式的值
        is_fly_mode_on = MMIAPISET_GetFlyMode();
        
        for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
        {
             if (MMIAPIPHONE_GetSimExistedStatus(i))
            {
                is_no_sim = FALSE;                
            }
        }

        GUIRES_DisplayImg(
            PNULL,
            &s_dropdown.network_rect,
            PNULL,
            win_id,
            IMAGE_DROPDOWN_LIST_BG,
            &lcd_dev_info
            );

        if(is_fly_mode_on)
        {            
            MMI_GetLabelTextByLang(TXT_SET_FLY_MODE, &network_str);

            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                &network_rect,
                &network_rect,
                &network_str,
                &text_style,
                text_state,
                0
                );
        }
        else if(is_no_sim)
        {               
            MMI_GetLabelTextByLang(TXT_INSERT_SIM, &network_str);
            
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                &network_rect,
                &network_rect,
                &network_str,
                &text_style,
                text_state,
                0
                );
        }
        else        
        {                
            for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
            {
                 if (MMIAPIPHONE_GetSimExistedStatus(i))
                {                        
                    if(network_rect.bottom > s_dropdown.network_rect.bottom)
                    {
                        s_dropdown.network_rect.bottom = network_rect.bottom;
                    }

                    if (MMIAPIIDLE_NeedShowIconAccordingToDualSysSetting((MN_DUAL_SYS_E)i))
                    {
                        SCI_MEMSET(&network_str, 0x00, sizeof(network_str));
                        network_str.wstr_ptr = (wchar *)opn_spn_name;
                        SCI_MEMSET(network_str.wstr_ptr, 0x00, sizeof(opn_spn_name));
                        
                        MMIAPIPHONE_GetSimAndServiceString((MN_DUAL_SYS_E)i, &network_str);
                    }
#ifndef MMI_MULTI_SIM_SYS_SINGLE
                    else
                    {
                        MMISET_SIM_NAME_T sim_name = {0};
                        MMI_STRING_T sim_deactivated = {0};

                        sim_name = MMIAPISET_GetSimName(i);

                        MMI_GetLabelTextByLang(TXT_SIM_DEACTIVATED, &sim_deactivated);

                        //此处不判断是否需要加入空格，统一都多分配一个空格的存储空间(2bytes)    
                        sim_deactivated_str = (wchar *)SCI_ALLOC_APP(((sim_deactivated.wstr_len + sim_name.wstr_len + 2)*sizeof(wchar)));
                        SCI_MEMSET(sim_deactivated_str, 0x00, ((sim_deactivated.wstr_len + sim_name.wstr_len + 2)*sizeof(wchar)));
                                            
                        MMI_WSTRNCPY(sim_deactivated_str,
                                                    (sim_deactivated.wstr_len + sim_name.wstr_len + 1),
                                                    sim_name.wstr_arr,
                                                    sim_name.wstr_len,
                                                    sim_name.wstr_len);
                        
                        // ASCII 码数据，两个字符串中间追加一个空格。
                        if (MMIAPICOM_IsASCIIString(sim_name.wstr_arr,sim_name.wstr_len)
                            && MMIAPICOM_IsASCIIString(sim_deactivated.wstr_ptr ,sim_name.wstr_len))
                        {
                            MMIAPICOM_Wstrcat(sim_deactivated_str, L" "); 
                            MMI_WSTRNCPY(sim_deactivated_str + sim_name.wstr_len + 1,
                                                    (sim_deactivated.wstr_len + 1),
                                                    sim_deactivated.wstr_ptr,
                                                    sim_deactivated.wstr_len,
                                                    sim_deactivated.wstr_len);
                            
                            network_str.wstr_ptr = (wchar *)sim_deactivated_str;
                            network_str.wstr_len = sim_deactivated.wstr_len + sim_name.wstr_len + 1 ;
                        }
                        else
                        {
                            MMI_WSTRNCPY(sim_deactivated_str + sim_name.wstr_len ,
                                                    (sim_deactivated.wstr_len + 1),
                                                    sim_deactivated.wstr_ptr,
                                                    sim_deactivated.wstr_len,
                                                    sim_deactivated.wstr_len);
                            network_str.wstr_ptr = (wchar *)sim_deactivated_str;
                            network_str.wstr_len = sim_deactivated.wstr_len + sim_name.wstr_len ;
                        }
                    }
#endif

                    GUISTR_GetStringInfo(&text_style, &network_str, text_state,&tmp_str_info);	 

                    GUIRES_DisplayImg(
                        PNULL,
                        &network_rect,
                        PNULL,
                        win_id,
                        IMAGE_DROPDOWN_LIST_BG,
                        &lcd_dev_info
                        );

                    sim_image_id = IMAGE_IDLE_PROMPT_SIM1 + i;

                    icon_rect = network_rect;

                    icon_rect.left = 0;//MMIDROPDOWN_NETWORK_ICON_LEFT;
                    
                    GUIRES_DisplayImg(
                        PNULL,
                        &icon_rect,
                        PNULL,
                        win_id,
                        sim_image_id,
                        &lcd_dev_info
                        );

                    text_rect = network_rect;

                    text_rect.left = 0;//MMIDROPDOWN_NETWORK_TEXT_LEFT;

                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &text_rect,
                        &text_rect,
                        &network_str,
                        &text_style,
                        text_state,
                        0
                        );

#ifndef MMI_MULTI_SIM_SYS_SINGLE           
                    network_rect.top += MMIDROPDOWN_NETWORK_HEIGHT;
                    network_rect.bottom +=MMIDROPDOWN_NETWORK_HEIGHT;
#endif
                }

#ifndef MMI_MULTI_SIM_SYS_SINGLE
                if (PNULL != sim_deactivated_str)
                {
                    SCI_FREE(sim_deactivated_str);
                    sim_deactivated_str = PNULL;
                }
#endif
            }   
        }
    }    
}
#endif

/*****************************************************************************/
//  Description : Handle Lcd siwtch
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownLcdSwitch(MMI_WIN_ID_T        win_id)
{    
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;    
    IGUICTRL_T    *stb_ctr_ptr  = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id, MMITHEME_GetStatusBarCtrlId());     
    MMI_CTRL_ID_T ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);
    IGUICTRL_T    *list_ctrl_ptr = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id,ctrl_id);

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height); 
    
    if(MMK_TP_SLIDE_FLING == s_dropdown.slide_state && SLIDE_DIR_UP == s_dropdown.slide_dir)
    {
        MMK_SendMsg(s_dropdown.parent_win, MSG_FULL_PAINT, PNULL);
        
        MMK_CloseWin(win_id);

        return;
    }

//    MMK_SendMsg(s_dropdown.parent_win, MSG_FULL_PAINT, PNULL);
//
    StopFling(win_id);
    StopTpRedrawTimer();
    StopUpdateRedrawTimer();

    s_dropdown.slide_state = MMK_TP_SLIDE_NONE;
    
    s_dropdown.state = DROPDOWN_STATE_NONE;
    
    UILAYER_RELEASELAYER(&s_dropdown.dropdown_layer);   /*lint !e506 !e774*/
    UILAYER_RELEASELAYER(&s_dropdown.statusbar_layer);   /*lint !e506 !e774*/
//    UILAYER_ReleaseLayer(&s_dropdown.dropdown_layer);   
//
//    s_dropdown.dropdown_layer.block_id = UILAYER_NULL_HANDLE;
//    
//    UILAYER_ReleaseLayer(&s_dropdown.statusbar_layer);
//
//    s_dropdown.statusbar_layer.block_id = UILAYER_NULL_HANDLE;

    InitDropDownWin(win_id);

    InitDropDownShortCut(win_id);

    s_dropdown.top = MMI_STATUSBAR_HEIGHT;

    s_dropdown.bottom = lcd_height;

    SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);  

    SetScrollKeyRect();
    
    GUICTRL_SetLcdDevInfo(list_ctrl_ptr, &s_dropdown.dropdown_layer); 
    
    GUICTRL_SetLcdDevInfo(stb_ctr_ptr, &s_dropdown.statusbar_layer);
    
    AppendLayer();    
    MMK_SendMsg(s_dropdown.parent_win, MSG_FULL_PAINT, PNULL);

	if (CheckNeedUpdateTimer())
	{
		StartUpdateRedrawTimer(win_id);
	}
}

/*****************************************************************************/
//  Description : Handle lose focus
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownLoseFocus(MMI_WIN_ID_T        win_id)
{
    IGUICTRL_T    *stb_ctr_ptr  = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id, MMITHEME_GetStatusBarCtrlId());   
     
//    if(MMK_TP_SLIDE_FLING == s_dropdown.slide_state && SLIDE_DIR_UP == s_dropdown.slide_dir)
    if((MMK_TP_SLIDE_FLING == s_dropdown.slide_state && SLIDE_DIR_UP == s_dropdown.slide_dir) || MMIAPIKL_IsPhoneLocked())
    {
        MMK_CloseWin(win_id);

        return;
    }

    //UILAYER_CopyLcdToMainLayer();

    StopFling(win_id);
    StopUpdateRedrawTimer();
	StopTpRedrawTimer();
    s_dropdown.slide_state = MMK_TP_SLIDE_NONE;
    
    s_dropdown.state = DROPDOWN_STATE_NONE;
    
    GUIWIN_SetStbDropDownState(win_id, FALSE);

    GUICTRL_SetLcdDevInfo(stb_ctr_ptr, MMITHEME_GetDefaultLcdDev());

    UILAYER_RemoveBltLayer(&s_dropdown.dropdown_layer);

    UILAYER_RemoveBltLayer(&s_dropdown.statusbar_layer);   

    UILAYER_RELEASELAYER(&s_dropdown.dropdown_layer);   /*lint !e506 !e774*/
    UILAYER_RELEASELAYER(&s_dropdown.statusbar_layer);   /*lint !e506 !e774*/
//    UILAYER_ReleaseLayer(&s_dropdown.dropdown_layer);
//
//    s_dropdown.dropdown_layer.block_id = UILAYER_NULL_HANDLE;
//
//    UILAYER_ReleaseLayer(&s_dropdown.statusbar_layer);
//
//    s_dropdown.statusbar_layer.block_id = UILAYER_NULL_HANDLE;
}

/*****************************************************************************/
//  Description : Handle get focus
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownGetFocus(MMI_WIN_ID_T        win_id)
{
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;    
    IGUICTRL_T    *stb_ctr_ptr  = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id, MMITHEME_GetStatusBarCtrlId());     
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);
    IGUICTRL_T    *list_ctrl_ptr = (IGUICTRL_T*)MMK_GetCtrlPtrByWin(win_id,ctrl_id);

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);    

//    MMK_SendMsg(s_dropdown.parent_win, MSG_FULL_PAINT, PNULL);

    InitDropDownWin(win_id);

    s_dropdown.top = MMI_STATUSBAR_HEIGHT;

    s_dropdown.bottom = lcd_height;

    SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);  

    SetScrollKeyRect();
    
    ReadNotifytoListBox(ctrl_id, FALSE);

    GUIWIN_SetStbDropDownState(win_id,TRUE);
    
    GUICTRL_SetLcdDevInfo(stb_ctr_ptr, &s_dropdown.statusbar_layer);

    GUICTRL_SetLcdDevInfo(list_ctrl_ptr, &s_dropdown.dropdown_layer); 

    AppendLayer();     
    
    MMK_SendMsg(s_dropdown.parent_win, MSG_FULL_PAINT, PNULL);

	if (CheckNeedUpdateTimer())
	{
		StartUpdateRedrawTimer(win_id);
	}
}

/*****************************************************************************/
//  Description : Handle TP move
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownTPMove(
                               MMI_WIN_ID_T        win_id, 
                               GUI_POINT_T        *tp_point_ptr
                               )
{
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;

    //SCI_TRACE_LOW:"HandleDropDownTPMove"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_3988_112_2_18_2_14_56_6,(uint8*)"");

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);   
        
    if(DROPDOWN_STATE_OPEN == s_dropdown.state || DROPDOWN_STATE_CLOSE == s_dropdown.state)
    {   
        if (abs(tp_point_ptr->y - s_dropdown.slide_pre_point.y) > MMIDROPDOWN_INIT_SPEED)
        {        
            s_dropdown.slide_state = MMK_TP_SLIDE_FLING;

            if(tp_point_ptr->y > s_dropdown.slide_pre_point.y)
            {
                s_dropdown.slide_dir = SLIDE_DIR_DOWN;
            }
            else
            {
                s_dropdown.slide_dir = SLIDE_DIR_UP;
            }
        }
        else
        {
            s_dropdown.slide_state = MMK_TP_SLIDE_NONE;
        }

        s_dropdown.slide_pre_point.x = tp_point_ptr->x;
        
        s_dropdown.slide_pre_point.y = tp_point_ptr->y;   

        if(tp_point_ptr->y > MMI_STATUSBAR_HEIGHT + MMIDROPDOWN_SCROLLKEY_HEIGHT)
        {
            s_dropdown.bottom = tp_point_ptr->y + MMIDROPDOWN_SCROLLKEY_HEIGHT/2;

            if(s_dropdown.bottom > lcd_height)
            {
                s_dropdown.bottom = lcd_height;
            }
        }
        else
        {
            s_dropdown.bottom = MMI_STATUSBAR_HEIGHT + MMIDROPDOWN_SCROLLKEY_HEIGHT;
        }

        s_dropdown.top = s_dropdown.bottom- s_dropdown.height;
        
        SetScrollKeyRect();

        SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);
    }
}

/*****************************************************************************/
//  Description : Handle TP down
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownTPDown(
                               MMI_WIN_ID_T        win_id, 
                               GUI_POINT_T        *tp_point_ptr
                               )
{
    s_dropdown.is_tp_down = TRUE;
    // 记录起始点
    s_dropdown.slide_pre_point.x = tp_point_ptr->x;
    s_dropdown.slide_pre_point.y = tp_point_ptr->y;
//    StartTpRedrawTimer(win_id);
}

/*****************************************************************************/
//  Description : Handle TP up
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownTPUp(
                               MMI_WIN_ID_T        win_id, 
                               GUI_POINT_T        *tp_point_ptr
                               )
{
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;

    //SCI_TRACE_LOW:"HandleDropDownTPUp"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_4066_112_2_18_2_14_56_7,(uint8*)"");

    if (PNULL == tp_point_ptr)
    {
        return;
    }
    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);   

    StopTpMoveTimer();
    StopTpRedrawTimer();
    StopUpdateRedrawTimer();

    s_dropdown.is_tp_down = FALSE;
    
    // compute
    if (s_dropdown.slide_state == MMK_TP_SLIDE_FLING)
    {
        StartFling(win_id);
    }
    else
    {
        if(DROPDOWN_STATE_OPEN == s_dropdown.state)
        {
            if(tp_point_ptr->y > lcd_height/2)
            {
                s_dropdown.slide_state = MMK_TP_SLIDE_FLING;
                
                s_dropdown.slide_dir = SLIDE_DIR_DOWN;
            }
            else
            {
                s_dropdown.slide_state = MMK_TP_SLIDE_FLING;
                
                s_dropdown.slide_dir = SLIDE_DIR_UP;                
            }
            StartFling(win_id);
        }
        else if(DROPDOWN_STATE_CLOSE == s_dropdown.state)
        {
            if(s_dropdown.bottom < lcd_height)
            {
                s_dropdown.slide_state = MMK_TP_SLIDE_FLING;
                
                s_dropdown.slide_dir = SLIDE_DIR_UP;

                StartFling(win_id);
            }
            else
            {
                s_dropdown.state = DROPDOWN_STATE_NONE;
            }
        }
        else
        {
//            GUI_RECT_T    button_notify_rect = MMIDROPDOWN_PAGEBUTTON_NOTIFY_RECT_V;
//            GUI_RECT_T    button_running_rect = MMIDROPDOWN_PAGEBUTTON_RUNNING_RECT_V; 
            if (GUI_PointIsInRect(*tp_point_ptr, s_shortcut_data.button_notify_rect))
            {
                HandlePageButtonPressed(win_id, DROPDOWN_PAGE_NOTIFY);
            }
            else if (GUI_PointIsInRect(*tp_point_ptr, s_shortcut_data.button_running_rect))
            {
                HandlePageButtonPressed(win_id, DROPDOWN_PAGE_RUNNING);
            }
            else
            {
                HandleShortCutFun(win_id, tp_point_ptr);
            }
        }
    }
}

/*****************************************************************************/
//  Description : 重新加载page内容
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void ReloadPageContent(
                           MMI_WIN_ID_T    win_id
                           )
{
    MMI_CTRL_ID_T    list_ctrl_id = WIN_CTRL_ID(win_id,MMIDROPDOWN_LIST_CTRL_ID);

#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
	MMIDROPDOWNWIN_RegisterWreNotifyRecords();
#endif

    switch (s_dropdown.cur_page)
    {
    case DROPDOWN_PAGE_NOTIFY:
        MMK_DestroyDynaCtrl(list_ctrl_id);
        CreateNotifyListBox(win_id, list_ctrl_id);
        ReadNotifytoListBox(list_ctrl_id, FALSE);
        break;

    case DROPDOWN_PAGE_RUNNING:
        MMK_DestroyDynaCtrl(list_ctrl_id);
        CreateRunningListBox(win_id, list_ctrl_id);
        ReadNotifytoListBox(list_ctrl_id, FALSE);
        break;

    default:
        break;
    }
}

/*****************************************************************************/
//  Description : Handle dropdown win slide
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void HandlePageButtonPressed(
                                   MMI_WIN_ID_T    win_id,
                                   DROPDOWN_PAGE_E page_index
                                   )
{
    if (page_index == s_dropdown.cur_page)
    {
        //当前页
        return;
    }
    switch (page_index)
    {
    case DROPDOWN_PAGE_NOTIFY:
        s_dropdown.cur_page = page_index;
        ReloadPageContent(win_id);
        MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
        break;

    case DROPDOWN_PAGE_RUNNING:
        s_dropdown.cur_page = page_index;
        ReloadPageContent(win_id);
        MMK_PostMsg(win_id, MSG_FULL_PAINT, 0, 0);
        break;

    default:
        break;
    }
}

/*****************************************************************************/
//  Description : Handle dropdown win slide
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void HandleDropDownSlide(MMI_WIN_ID_T    win_id)
{
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;
    BOOLEAN   need_close = FALSE;

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);    
    
    if ( MMK_TP_SLIDE_FLING == s_dropdown.slide_state )
    {        
         if (SLIDE_DIR_UP == s_dropdown.slide_dir) // 向上
        {                
            s_dropdown.bottom = s_dropdown.bottom - MMIDROPDOWN_SLIDE_SPEED;

            if(s_dropdown.bottom < MMI_STATUSBAR_HEIGHT + MMIDROPDOWN_SCROLLKEY_HEIGHT)
            {
                s_dropdown.slide_state = MMK_TP_SLIDE_NONE;

                s_dropdown.state = DROPDOWN_STATE_NONE;
                    
                StopTpMoveTimer();

                s_dropdown.bottom = MMI_STATUSBAR_HEIGHT + MMIDROPDOWN_SCROLLKEY_HEIGHT;

                need_close = TRUE;                
            }
            
            s_dropdown.top = s_dropdown.bottom- s_dropdown.height;
            
            SetScrollKeyRect();
            
//            SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);

            if(need_close)
            {             
                StatusBarScrollAnim(win_id, FALSE);

                UILAYER_RemoveBltLayer(&s_dropdown.dropdown_layer);

                MMK_PostMsg(win_id, MSG_DROPDOWNWIN_CLOSE, PNULL, NULL);
            }
            else
            {
                UILAYER_SetLayerPosition(&s_dropdown.dropdown_layer, 0, s_dropdown.top);
            }
        }
        else if(SLIDE_DIR_DOWN == s_dropdown.slide_dir)// 向下
        {      
            s_dropdown.bottom = s_dropdown.bottom + MMIDROPDOWN_SLIDE_SPEED;

            if(s_dropdown.bottom > lcd_height)
            {
                s_dropdown.slide_state = MMK_TP_SLIDE_NONE;
                
                s_dropdown.state = DROPDOWN_STATE_NONE;
                    
                StopFling(win_id);

                s_dropdown.bottom = lcd_height;
            }
            
            s_dropdown.top = s_dropdown.bottom- s_dropdown.height;
            
            SetScrollKeyRect();
            
            SetDropdownLayerPos(&s_dropdown.dropdown_layer, 0, s_dropdown.top);
        }
    }
}

/*****************************************************************************/
//  Description : Start TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StartTpRedrawTimer(MMI_WIN_ID_T    win_id )
{
    if (0 == s_dropdown.tp_redraw_timer_id)
    {
        s_dropdown.tp_redraw_timer_id = MMK_CreateWinTimer( win_id, MMIDROPDOWN_TPMOVE_REDRAW_TIME/*MMITHEME_MENU_REDRAW_TIME*/, FALSE );
    }
}

/*****************************************************************************/
//  Description : Stop TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopTpRedrawTimer(void)
{
    if (0 < s_dropdown.tp_redraw_timer_id)
    {
        MMK_StopTimer(s_dropdown.tp_redraw_timer_id);
        
        s_dropdown.tp_redraw_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : Start TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StartUpdateRedrawTimer(MMI_WIN_ID_T    win_id )
{
    if (0 == s_dropdown.update_redraw_timer_id)
    {
        s_dropdown.update_redraw_timer_id = MMK_CreateWinTimer( win_id, MMIDROPDOWN_UPDATE_REDRAW_TIME/*MMITHEME_MENU_REDRAW_TIME*/, FALSE );
    }
}

/*****************************************************************************/
//  Description : Stop TP move timer
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void StopUpdateRedrawTimer(void)
{
    if (0 < s_dropdown.update_redraw_timer_id)
    {
        MMK_StopTimer(s_dropdown.update_redraw_timer_id);
        
        s_dropdown.update_redraw_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : Start TP move timer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void StartTpMoveTimer(MMI_WIN_ID_T    win_id )
{
    if (0 == s_dropdown.tp_move_timer_id)
    {
        s_dropdown.tp_move_timer_id = MMK_CreateWinTimer( win_id, MMITHEME_MENU_REDRAW_TIME/*MMITHEME_MENU_TPMOVE_TIME*/, FALSE );
    }
}

/*****************************************************************************/
//  Description : Stop TP move timer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void StopTpMoveTimer(void)
{
    if (0 < s_dropdown.tp_move_timer_id)
    {
        MMK_StopTimer(s_dropdown.tp_move_timer_id);
        
        s_dropdown.tp_move_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : Append Layer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void AppendLayer(void)
{
    UILAYER_APPEND_BLT_T    append_layer = {0};
    
    UILAYER_RemoveBltLayer(&s_dropdown.dropdown_layer);
        
    append_layer.lcd_dev_info = s_dropdown.dropdown_layer;

    append_layer.layer_level = UILAYER_LEVEL_NORMAL;

    UILAYER_AppendBltLayer(&append_layer); 

    UILAYER_RemoveBltLayer(&s_dropdown.statusbar_layer);                

    append_layer.lcd_dev_info = s_dropdown.statusbar_layer;

    append_layer.layer_level = UILAYER_LEVEL_NORMAL;

    UILAYER_AppendBltLayer(&append_layer);  
}

/*****************************************************************************/
//  Description : Create DropDown Layer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateDropDownLayer(
                        MMI_HANDLE_T win_id,
                        GUI_LCD_DEV_INFO *lcd_dev_ptr
                        )
{
    UILAYER_CREATE_T    create_info = {0};
    GUI_BOTH_RECT_T    both_rect = MMITHEME_GetWinClientBothRect(win_id);

//    SCI_ASSERT(PNULL != lcd_dev_ptr);
    if (PNULL == lcd_dev_ptr)
    {
        return;
    }
 
    lcd_dev_ptr->lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_ptr->block_id = UILAYER_NULL_HANDLE;
    
    // 创建层
    create_info.lcd_id = GUI_MAIN_LCD_ID;
    create_info.owner_handle = win_id;
    
    if ( MMK_IsWindowLandscape(win_id) )
    {
        create_info.offset_x = 0;
        create_info.offset_y = 0;
        create_info.width = both_rect.h_rect.right - both_rect.h_rect.left + 1;
        create_info.height = both_rect.h_rect.bottom - both_rect.h_rect.top + 1;
    }
    else
    {
        create_info.offset_x = 0;
        create_info.offset_y = 0;
        create_info.width = both_rect.v_rect.right - both_rect.v_rect.left + 1;
        create_info.height = both_rect.v_rect.bottom - both_rect.v_rect.top + 1;
    }    

    create_info.is_bg_layer = FALSE;
    create_info.is_static_layer = FALSE;     
//    create_info.format = UILAYER_MEM_DOUBLE_COPY;//UILAYER_MEM_IMMUTABLE;
    
    UILAYER_CreateLayer(
        &create_info,
        lcd_dev_ptr
        );

    UILAYER_SetLayerColorKey(lcd_dev_ptr, TRUE, UILAYER_TRANSPARENT_COLOR);

    UILAYER_Clear(lcd_dev_ptr);

    SetDropdownLayerPos(lcd_dev_ptr, 0, 0);     

    return;
    
}

/*****************************************************************************/
//  Description : Create Status Bar Layer
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void CreateStatusBarLayer(
                        MMI_HANDLE_T win_id,
                        GUI_LCD_DEV_INFO *lcd_dev_ptr
                        )
{
    UILAYER_CREATE_T     create_info = {0};
    GUI_BOTH_RECT_T    both_rect = MMITHEME_GetStatusBarBothRect();

//    SCI_ASSERT(PNULL != lcd_dev_ptr);
    if (PNULL == lcd_dev_ptr)
    {
        return;
    }
 
    lcd_dev_ptr->lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_ptr->block_id = UILAYER_NULL_HANDLE;
    
    // 创建层
    create_info.lcd_id = GUI_MAIN_LCD_ID;
    create_info.owner_handle = win_id;
    
    if ( MMK_IsWindowLandscape(win_id) )
    {
        create_info.offset_x = 0;
        create_info.offset_y = 0;
        create_info.width = both_rect.h_rect.right - both_rect.h_rect.left + 1;
        create_info.height = both_rect.h_rect.bottom - both_rect.h_rect.top + 1;
    }
    else
    {
        create_info.offset_x = 0;
        create_info.offset_y = 0;
        create_info.width = both_rect.v_rect.right - both_rect.v_rect.left + 1;
        create_info.height = both_rect.v_rect.bottom - both_rect.v_rect.top + 1;
    }    

    create_info.is_bg_layer = FALSE;
    create_info.is_static_layer = FALSE;     
    
    UILAYER_CreateLayer(
        &create_info,
        lcd_dev_ptr
        );

    UILAYER_SetLayerColorKey(lcd_dev_ptr, TRUE, UILAYER_TRANSPARENT_COLOR);

    UILAYER_Clear(lcd_dev_ptr);

    UILAYER_SetLayerPosition(lcd_dev_ptr, 0, 0);     
    
    return;
}

/*****************************************************************************/
//  Description : counted timer Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CountedTimerNotifyCallBack(void)
{
#ifdef MMI_PDA_SUPPORT
	//extern s_timer_cur_item_num;
#endif

    if (MMIAPICT_IsShowIdleCoutime())
    {
#ifdef MMI_PDA_SUPPORT
		//s_timer_cur_item_num = MMI_CT_TIMER;
		MMIAPI_OpenTimerWinByType( MMI_CT_TIMER );
#else
        MMIAPICT_OpenCoutimeRunningWin();
#endif
    }
    else
    {
        MMIAPICT_OpenCoutimeWin();
    }
    return TRUE;
}

/*****************************************************************************/
//  Description : calling Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CallingNotifyCallBack(void)
{
    MMIAPICC_ActiveCCApplet();
    return TRUE;
}

/*****************************************************************************/
//  Description : usb using Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN UsbUsingNotifyCallBack(void)
{
    
    return TRUE;
}

/*****************************************************************************/
//  Description : Mp3 Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN Mp3NotifyCallBack(void)
{
    if(MMK_IsOpenWin(MMIFMM_OPEN_FILE_BYLAYER_WIN_ID))
    {
        //MP3会影响FMM，所以提示不能打开MP3界面
        MMIPUB_OpenAlertWarningWin(TXT_DROPDOWN_FMM_WARNING);
        return FALSE;
    }
#ifdef VIDEO_PLAYER_SUPPORT 
    else if(MMIAPIVP_IsOpenPlayerWin())
    {
        //VP打开时无法恢复播放，所以提示不能打开MP3界面
        MMIPUB_OpenAlertWarningWin(TXT_DROPDOWN_VP_WARNING);
        return FALSE;
    }
#endif
#ifdef ATV_SUPPORT 
    else if(MMIAPIATV_IsATVOpen())
    {
        //ATV打开时无法恢复播放，所以提示不能打开MP3界面
        MMIPUB_OpenAlertWarningWin(TXT_DROPDOWN_ATV_WARNING);
        return FALSE;
    }
#endif
    MMIAPIMP3_OpenMp3Player();

    return TRUE;
}

/*****************************************************************************/
//  Description : FM Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN FMNotifyCallBack(void)
{
    MMIFM_OpenMainWin();

    return TRUE;
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : MRAPP Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MRAPPNotifyCallBack(void)
{
    return MMIMRAPP_activeMrapp();
}

#ifdef __MMI_SKYQQ__
/*****************************************************************************/
//  Description : SKYQQ Notify callback
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN SkyQQNotifyCallBack(void)
{
    return MMIMRAPP_AppEntry(MRAPP_SKYQQ);
}
#endif
#endif

/*****************************************************************************/
//  Description : BT Notify callback
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
#ifdef BLUETOOTH_SUPPORT
LOCAL BOOLEAN BTNotifyCallBack(void)
{ 
    MMIAPIBT_OpenTransferWin();
    return TRUE;
}
#endif
#ifdef ASP_SUPPORT
/*****************************************************************************/
//  Description : new asp Notify callback
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN NewAspNotifyCallBack(void)
{
    BOOLEAN result = FALSE;
    uint16 node_index = 0;
    uint16 new_msg_count = MMIAPIASP_GetNewRecordNum();

    if (0 == new_msg_count)
    {
        return result;
    }
    if (new_msg_count>1 )
    {
        result = MMIAPIASP_EnterAsp();
    }
    else
    {
        if (MMIAPIASP_GetLastestUnreadRecord(PNULL, &node_index))
        {
            result = MMIAPIASP_OpenPlayWinFromExternal(node_index);
        }
    }

    return result;
}
#endif

/*****************************************************************************/
//  Description : Get Notify callback
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMIDROPDOWN_NOTIFY_CALLBACK GetNotifyCallBack(DROPDOWN_NOTIFY_E    notify_type)
{
    MMIDROPDOWN_NOTIFY_CALLBACK    callback = PNULL;
        
    switch(notify_type)
    {
        case DROPDOWN_NOTIFY_MISS_CALL:
            callback = MMIMAIN_MissCallCallBack;
            break;

        case DROPDOWN_NOTIFY_NEW_MESSAGE:
            callback = MMIMAIN_MissMessageCallBack;
            break;

#if defined(MMI_SMSCB_SUPPORT)
        case DROPDOWN_NOTIFY_NEW_SMSCB:
            callback = MMIAPISMSCB_OpenCBList;
            break;
#endif

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
        case DROPDOWN_NOTIFY_NEW_PUSH:
            callback = MMIMAIN_ReadPushCallBack;
            break;
#endif

        case DROPDOWN_NOTIFY_STK_IDLE_TEXT:
            callback = MMIAPIIDLE_CreateStkIdleTextWin;
            break;

        case DROPDOWN_NOTIFY_MESSAGE_FULL:
            callback = MMIMAIN_MissMessageCallBack;
            break;           

        case DROPDOWN_RUNNING_PLAYER:
            callback = Mp3NotifyCallBack;
            break;
#ifdef MRAPP_SUPPORT
		case DROPDOWN_RUNNING_MRAPP:
			callback = MRAPPNotifyCallBack;
            break;	
#ifdef __MMI_SKYQQ__
		case DROPDOWN_RUNNING_SKYQQ:
			callback = SkyQQNotifyCallBack;
            break;	
#endif
#endif
/*renwei modify cr00117388*/
#ifdef FM_SUPPORT
        case DROPDOWN_RUNNING_FM:
            callback = FMNotifyCallBack;
            break;
#endif
/*renwei modify cr00117388*/
#ifndef JAVA_SUPPORT_NONE
        case DROPDOWN_RUNNING_JAVA://cr00115796 renwei modify
            callback = MMIIDLE_JavaCallBack;
            break;
#endif
#ifdef BLUETOOTH_SUPPORT
        case DROPDOWN_RUNNING_BT_TRANSFERFILE:
            callback = BTNotifyCallBack;
            break;
#endif
#ifdef QQ_SUPPORT
        case DROPDOWN_NOTIFY_QQ:
            callback = MMIIDLE_QqCallBack;
            break;
#endif


#ifdef MCARE_V31_SUPPORT
    case DROPDOWN_NOTIFY_MCARE_QQ:
	     callback = MMIMAIN_MCARE_QQCallBack;
	     break;
    case DROPDOWN_NOTIFY_MCARE_QC:
	     callback = MMIMAIN_MCARE_QCCallBack;
	     break;
#endif

#ifdef COUNTEDTIME_SUPPORT
        case DROPDOWN_RUNNING_COUNTEDTIME:
            callback = CountedTimerNotifyCallBack;
            break;
#endif

        case DROPDOWN_RUNNING_USB_USING:
            callback = UsbUsingNotifyCallBack;
            break;

#ifdef ASP_SUPPORT
        case DROPDOWN_NOTIFY_NEW_ASP:
            callback = NewAspNotifyCallBack;
            break;
#endif
        case DROPDOWN_RUNNING_CALLING:
            callback = CallingNotifyCallBack;
            break;

        default:
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
	if (DROPDOWN_RUNNING_WRE_APP_START <= notify_type && notify_type <= DROPDOWN_RUNNING_WRE_APP_END)
	{
 		callback = MMIWRE_MaxmizeAPP;
 	}
#endif
            break;
    }    

    return callback;
}

/*****************************************************************************/
//  Description : Get new message notify image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetNewMessageNotifyImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

#ifndef MMI_MULTI_SIM_SYS_SINGLE
    if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_SMS)) //未读短信
    {
        image_id = MMIAPISMS_GetNotReadSmsImageId();
    }
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_MMS)) //未读彩信
    {
        image_id = MMIAPISMS_GetNotReadMmsImageId();
    }
#ifndef MMISMS_CHATMODE_ONLY_SUPPORT

    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_WAP_PUSH)) //未读push消息
    {
        image_id = MMIAPISMS_GetNotReadWAPPUSHImageId();
    }
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_WAP_OTA)) //未读WAP OTA
    {
        image_id = MMIAPISMS_GetNotReadWAPOTAImageId();
    }
#endif
#else
    if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_1, MMISMS_TYPE_SMS)) //未读短信
    {
        image_id = MMIAPISMS_GetNotReadSmsImageId();
    }
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_1, MMISMS_TYPE_MMS)) //未读彩信
    {
        image_id = MMIAPISMS_GetNotReadMmsImageId();
    }
#ifndef MMISMS_CHATMODE_ONLY_SUPPORT
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_1, MMISMS_TYPE_WAP_PUSH)) //未读push消息
    {
        image_id = MMIAPISMS_GetNotReadWAPPUSHImageId();
    }
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_1, MMISMS_TYPE_WAP_OTA)) //未读WAP OTA
    {
        image_id = MMIAPISMS_GetNotReadWAPOTAImageId();
    }
#endif
#endif
    
    return image_id;
}

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//  Description : Get new message notify image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetNewPushNotifyImage(void)
{
    MMI_IMAGE_ID_T image_id = IMAGE_NULL;

#ifndef MMI_MULTI_SIM_SYS_SINGLE
    if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_WAP_PUSH)) //未读push消息
    {
        image_id = MMIAPISMS_GetNotReadWAPPUSHImageId();
    }
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_WAP_OTA)) //未读WAP OTA
    {
        image_id = MMIAPISMS_GetNotReadWAPOTAImageId();
    }
#else
    if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_1, MMISMS_TYPE_WAP_PUSH)) //未读push消息
    {
        image_id = MMIAPISMS_GetNotReadWAPPUSHImageId();
    }
    else if (0 < MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_1, MMISMS_TYPE_WAP_OTA)) //未读WAP OTA
    {
        image_id = MMIAPISMS_GetNotReadWAPOTAImageId();
    }
#endif
    
    return image_id;
}
#endif

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetMessageFullNotifyImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    if (MMIAPISMS_IsSMSFull()) //短信满
    {
        image_id = MMIAPISMS_GetFullMessageImageId();
    }
    
    return image_id;
}

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetMp3NotifyImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

#ifdef MMI_PDA_SUPPORT
    image_id = IMAGE_DROPDOWN_MP3;//IMAGE_IDLE_TITLE_ICON_FULL_MESSAGE_DUA;
#else
    image_id = IMAGE_SECMENU_ICON_MULTIM_MP3;
#endif

    return image_id;
}
#ifdef BLUETOOTH_SUPPORT
/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: yuantao.xu
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetBTActionImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    image_id = IMAGE_DROPDOWN_MP3_PAUSE;

    return image_id;
}
#endif
/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetMp3ActionImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    image_id = IMAGE_DROPDOWN_FM_OFF;//IMAGE_DROPDOWN_MP3_PAUSE;//IMAGE_IDLE_TITLE_ICON_FULL_MMS_DUA;

    return image_id;
}

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetFMNotifyImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

#ifdef MMI_PDA_SUPPORT
    image_id = IMAGE_DROPDOWN_FM;
#else
    image_id = IMAGE_SECMENU_ICON_MULTIM_FM;
#endif

    return image_id;
}

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetFMActionImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    image_id = IMAGE_DROPDOWN_FM_OFF;

    return image_id;
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Zack.Zhang
//  Note: 
/*****************************************************************************/
LOCAL void GetMRAPPNotifyTitle(MMI_STRING_T *title)
{
    MRAPP_BACKRUN_INFO_T back_info = {0};

    if(title != NULL)
    {
        MMIMRAPP_GetBackRunAPPInfo(&back_info);

        if(back_info.app_id == MRAPP_SDKQQ)
        {
            MMI_GetLabelTextByLang(TXT_MRAPP_QQ, title);
        }
        else if(back_info.app_id == MRAPP_SHOUXIN)
        {
             MMI_GetLabelTextByLang(TXT_MRAPP_SHOUXIN, title);
        }
        else
        {
             MMI_GetLabelTextByLang(TXT_MRAPP_GAMEC, title);
        }
    }
}

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: modify by zack@20120414 for different appid show different icon
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetMRAPPNotifyImage(void)
{
    MMI_IMAGE_ID_T       image_id  = IMAGE_NULL;
    MRAPP_BACKRUN_INFO_T back_info = {0};

    MMIMRAPP_GetBackRunAPPInfo(&back_info);

    if(back_info.app_id == MRAPP_SDKQQ)
    {
        image_id = IMG_MRAPP_MAINMENU_QQ;
    }
    else if(back_info.app_id == MRAPP_SHOUXIN)
    {
        image_id = IMG_MRAPP_MAINMENU_SHOUXIN;
    }
    else
    {
        image_id = IMG_MRAPP_MAINMENU_MAOPAO;
    }

    return image_id;
}

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetMRAPPActionImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    image_id = IMAGE_DROPDOWN_FM_OFF;

    return image_id;
}

#ifdef __MMI_SKYQQ__
/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetSkyQQNotifyImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    image_id = IMG_MRAPP_MAINMENU_QQ;

    return image_id;
}

/*****************************************************************************/
//  Description : Get message full notify image
//  Global resource dependence : none
//  Author: Tommy.yan
//  Note: 
/*****************************************************************************/
LOCAL MMI_IMAGE_ID_T GetSkyQQActionImage(void)
{
    MMI_IMAGE_ID_T          image_id    =   IMAGE_NULL;

    image_id = IMAGE_DROPDOWN_FM_OFF;

    return image_id;
}
#endif
#endif

/*****************************************************************************/
//  Description : Get full title
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN GetFullTitle(
                           uint32 record_num,       //in
                           MMI_TEXT_ID_T title_id,  //in
                           MMI_STRING_T *str_ptr   //out
                           )
{
    MMI_STRING_T    title_str = {0};
    uint8 record_count_str[10] = {0};
    uint32 wstr_len = 0;

    if ((PNULL == str_ptr) || (PNULL == str_ptr->wstr_ptr))
    {
        return FALSE;
    }
    MMI_GetLabelTextByLang(title_id, &title_str);
    if ((title_str.wstr_len+10) >= str_ptr->wstr_len)
    {
        //超过最大长度
        return FALSE;
    }
    sprintf((char *)record_count_str, "%d", record_num);
    MMIAPICOM_StrToWstr(record_count_str, str_ptr->wstr_ptr);
    wstr_len = MMIAPICOM_Wstrlen((const wchar*)str_ptr->wstr_ptr);
    MMIAPICOM_Wstrncpy((str_ptr->wstr_ptr+wstr_len), title_str.wstr_ptr, title_str.wstr_len);
    return TRUE;
}

#ifdef ASP_SUPPORT

/*****************************************************************************/
//  Description : Get new asp detail
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL void GetNewAspDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail)
{
    MMIASP_MSG_INFO_T   record_info = {0};
    uint32 new_record_num = 0;
    uint8    time[MMI_DISPLAY_TIME_LEN + 1 ] = {0};
	MMI_TEXT_ID_T		new_asp_text_id = TXT_MMIASP_NEW_MSG;

    //title
    SCI_MEMSET(s_newasp_title, 0, sizeof(s_newasp_title));
    detail->notify_title.wstr_ptr = s_newasp_title;
    detail->notify_title.wstr_len = ARR_SIZE(s_newasp_title);
    new_record_num = MMIAPIASP_GetNewRecordNum();

	if( 1 < new_record_num)
	{
		new_asp_text_id = TXT_MMIASP_NEW_MULT_MSGS;
	}

    GetFullTitle(new_record_num, new_asp_text_id, &detail->notify_title);
    
    //name
    SCI_MEMSET(s_newasp_name, 0, sizeof(s_newasp_name));
    detail->notify_string.wstr_ptr = s_newasp_name;
    detail->notify_string.wstr_len = ARR_SIZE(s_newasp_name);
    if (MMIAPIASP_GetLastestUnreadRecord(&record_info ,PNULL))
    {
        // get name
        if(MMIAPIASP_GetNameByNumber(record_info.call_num, &detail->notify_string))
        { 
            //SCI_TRACE_LOW:"MMIASP this number has find a name "
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_5029_112_2_18_2_14_58_8,(uint8*)"");
        }
        else
        {
            //MMIAPICOM_StrToWstr(record_info.call_num, name_str.wstr_ptr, record_info.call_num, );
            MMI_STRNTOWSTR((wchar*)detail->notify_string.wstr_ptr, MMIASP_MAX_NAME_LEN, 
                record_info.call_num, MMIPB_MAX_STRING_LEN, 
                strlen((char*)record_info.call_num));
            detail->notify_string.wstr_len = (uint16)MMIAPICOM_Wstrlen(detail->notify_string.wstr_ptr);
        }        
        
        //time
        SCI_MEMSET(s_newasp_time, 0, sizeof(s_newasp_time));
        detail->notify_time.wstr_ptr = s_newasp_time;
        detail->notify_time.wstr_len = ARR_SIZE(s_newasp_time);
        MMIAPISET_FormatTimeStrByTime(record_info.date_info.sys_time.hour, record_info.date_info.sys_time.min,
            time, MMI_DISPLAY_TIME_LEN + 1 );        
        MMI_STRNTOWSTR( s_newasp_time, MMI_DISPLAY_TIME_LEN, time, MMI_DISPLAY_TIME_LEN, strlen((char*)time));
//        MMI_STRNTOWSTR( s_newasp_time, MMI_DISPLAY_TIME_LEN, (uint8 *)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len );
        detail->notify_time.wstr_len = (uint16)strlen((char*)time);
    }

    detail->notify_image = IMAGE_SECMENU_ICON_CALL_RECORD_UNREAD;
}

#endif


/*****************************************************************************/
//  Description : Get miss call detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetMissCallDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail)
{
    BOOLEAN     result = FALSE;
    MMICL_CALL_BCDINFO_T  *call_bcdinfo_ptr = PNULL;
    uint8 	tele_num_len = 0;	
    uint8   	tele_num[PHONE_NUM_MAX_LEN + 2] = {0};
    uint8    time[MMI_DISPLAY_TIME_LEN + 1 ] = {0};
    MMI_TM_T   tm = {0};
    BOOLEAN 	find_result =FALSE;
    MMI_STRING_T   string = {0};    
    wchar    count_wstr[MMI_COUNT_STR_LEN] = {0};
    char       count_str[MMI_COUNT_STR_LEN] = {0};
    uint16    str_len= 0;
    uint32    missed_call_num = 0;   

    call_bcdinfo_ptr = SCI_ALLOCA(sizeof(MMICL_CALL_BCDINFO_T));

    if(PNULL == call_bcdinfo_ptr)
    {
        //SCI_TRACE_LOW:"GetMissCallDetail call info alloc failed"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_5080_112_2_18_2_14_58_9,(uint8*)"");
        return;
    }

    SCI_MEMSET(call_bcdinfo_ptr, 0, sizeof(MMICL_CALL_BCDINFO_T));

    detail->notify_image = IMAGE_CL_MISSED_CALL_ICON;////IMAGE_IDLE_TITLE_ICON_MISSED_CALL;

    missed_call_num = MMIAPICC_GetMissedCallNumber();
    
    if(1 < missed_call_num)
    {        
        SCI_MEMSET(s_misscall_title, 0, sizeof(s_misscall_title));
        
        MMI_GetLabelTextByLang(TXT_COMMON_MISS_CALLS, &string);
        
        MMIAPICOM_Wstrncpy(s_misscall_title,string.wstr_ptr,string.wstr_len);        

        detail->notify_title.wstr_ptr = s_misscall_title;

        detail->notify_title.wstr_len = MMIAPICOM_Wstrlen(s_misscall_title);


        SCI_MEMSET(s_misscall_name, 0, sizeof(s_misscall_name));
        
        detail->notify_string.wstr_ptr = s_misscall_name;        

        sprintf(count_str, "%d ", missed_call_num);
        
        str_len = strlen(count_str);
        
        MMI_STRNTOWSTR(count_wstr, MMI_COUNT_STR_LEN, (const uint8*)count_str, MMI_COUNT_STR_LEN, str_len);
        
        MMIAPICOM_Wstrcat(s_misscall_name, count_wstr);

        MMIAPICOM_Wstrncat(s_misscall_name,string.wstr_ptr,string.wstr_len);
        
        detail->notify_string.wstr_len =  MMIAPICOM_Wstrlen(s_misscall_name);
    }
    else
    {
        MMI_GetLabelTextByLang(TXT_CC_MISS_CALL, &detail->notify_title);

        result = MMICL_ReadLastMissedCall(call_bcdinfo_ptr);
        
        if (result)
        {
            SCI_MEMSET(s_misscall_name, 0, sizeof(s_misscall_name));
            
            detail->notify_string.wstr_ptr = s_misscall_name;        

            tele_num_len = MMIAPICOM_GenDispNumber(call_bcdinfo_ptr->number_type,
                                                                                                    call_bcdinfo_ptr->number_len,
                                                                                                    call_bcdinfo_ptr->number, 
                                                                                                    tele_num,
                                                                                                    PHONE_NUM_MAX_LEN + 2);

            find_result = MMIAPISMS_GetNameByNumberFromPB(tele_num, &detail->notify_string, PHONE_NUM_MAX_LEN);

            if (!find_result)
            {
                detail->notify_string.wstr_len = PHONE_NUM_MAX_LEN;

                MMI_STRNTOWSTR(detail->notify_string.wstr_ptr,
                detail->notify_string.wstr_len,
                (const uint8*)tele_num,
                tele_num_len,
                tele_num_len);

                detail->notify_string.wstr_len =  (uint16)strlen((char*)tele_num);
            }   
        }
    }

    result = MMICL_ReadLastMissedCall(call_bcdinfo_ptr);

    if (result)
    {
        //最后一条对应最近记录
#ifdef CALL_TIMES_SUPPORT
        if (call_bcdinfo_ptr->call_times > 0)
        {
            tm = MMIAPICOM_Second2Tm(call_bcdinfo_ptr->call_start_time[call_bcdinfo_ptr->call_times -1] );
        }       
#else
        tm = MMIAPICOM_Second2Tm(call_bcdinfo_ptr->call_start_time);
#endif

        MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time,MMI_DISPLAY_TIME_LEN + 1 );

        SCI_MEMSET(s_misscall_time, 0, sizeof(s_misscall_time));
        
        detail->notify_time.wstr_ptr = s_misscall_time;
        
        detail->notify_time.wstr_len = (uint16)strlen((char*)time);
        
        MMI_STRNTOWSTR( s_misscall_time, MMI_DISPLAY_TIME_LEN, (uint8 *)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len );
    }
    SCI_FREE(call_bcdinfo_ptr);
    call_bcdinfo_ptr = PNULL;
}


/*****************************************************************************/
//  Description : Get new message number
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetNewMessageDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail)
{
    uint8 number_len = 0;	
    uint8 number[MMISMS_PBNUM_MAX_LEN + 1] = {0};
    BOOLEAN 	find_result =FALSE;
    MMI_STRING_T   string = {0};    
    wchar    count_wstr[MMI_COUNT_STR_LEN] = {0};
    char       count_str[MMI_COUNT_STR_LEN] = {0};
    uint16    str_len= 0;
    uint32    newmessage_num = 0;   
    
    newmessage_num =  MMIAPISMS_GetAllUnreadMsgCount();
    if(0 == newmessage_num)
    {
        return;
    }

    detail->notify_image = GetNewMessageNotifyImage();

    if(1 < newmessage_num)
    {        
        SCI_MEMSET(&s_newmessage_title, 0, sizeof(s_newmessage_title));
        
        MMI_GetLabelTextByLang(TXT_SMS_NEW_MSGS, &string);
        
        MMIAPICOM_Wstrncpy(s_newmessage_title,string.wstr_ptr,string.wstr_len);        

        sprintf(count_str, " (%d)", newmessage_num);
        
        str_len = strlen(count_str);
        
        MMI_STRNTOWSTR(count_wstr, MMI_COUNT_STR_LEN, (const uint8*)count_str, MMI_COUNT_STR_LEN, str_len);
        
        MMIAPICOM_Wstrcat(s_newmessage_title, count_wstr);

        detail->notify_title.wstr_ptr = s_newmessage_title;

        detail->notify_title.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_title);
    }
    else
    {
        MMI_GetLabelTextByLang(TXT_SMS_NEW_MSG, &detail->notify_title);
    }    

    number_len = GetNewMessageNumber(
		number,
		MMISMS_PBNUM_MAX_LEN
		);

    if(number_len > 0)
    {
        SCI_MEMSET(s_newmessage_name, 0, sizeof(s_newmessage_name));
        
        detail->notify_string.wstr_ptr = s_newmessage_name;        
        
        find_result = MMIAPISMS_GetNameByNumberFromPB(number, &detail->notify_string, MMISMS_PBNAME_MAX_LEN);
    	
        if (!find_result)
        {
            detail->notify_string.wstr_len = MMISMS_PBNAME_MAX_LEN;
    		
            MMI_STRNTOWSTR(detail->notify_string.wstr_ptr,
    			detail->notify_string.wstr_len,
    			(const uint8*)number,
    			number_len,
    			number_len);
    		
            detail->notify_string.wstr_len = number_len;
        }	
    }
    else
    {
#ifdef MMS_SUPPORT
        if(MMIAPISMS_GetNotReadWAPPUSHImageId() == detail->notify_image)
        {
            MMI_GetLabelTextByLang(TXT_OTA_PUSH_ARRIVEED, &detail->notify_string);
        }
        else if(MMIAPISMS_GetNotReadWAPOTAImageId() == detail->notify_image)
        {
            MMI_GetLabelTextByLang(TXT_MMS_RECEIVE_CONFIG_PUSH, &detail->notify_string);
        }
#endif
    }
    
    SCI_MEMSET(&s_newmessage_time, 0, sizeof(s_newmessage_time));

    GetNewMessageTime(s_newmessage_time);    

    detail->notify_time.wstr_ptr = s_newmessage_time;
    
    detail->notify_time.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_time);
}

#if defined(MMI_SMSCB_SUPPORT)
/*****************************************************************************/
//  Description : Get new SMSCB number
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void GetNewSMSCBDetail(MMIDROPDOWN_NOTIFY_DETAIL_T * detail)
{
    MMI_STRING_T   string = {0};    
    wchar    count_wstr[MMI_COUNT_STR_LEN] = {0};
    char       count_str[MMI_COUNT_STR_LEN] = {0};
    uint16    str_len= 0;
    uint32    newmessage_num = 0;   
    
    newmessage_num =  MMIAPISMSCB_GetUnreadSMSCBNum();

    if(0 == newmessage_num)
    {
        return;
    }

    detail->notify_image = MMIAPISMS_GetNotReadSmsImageId();

    if(1 < newmessage_num)
    {
        SCI_MEMSET(&s_newmessage_title, 0, sizeof(s_newmessage_title));
        
        MMI_GetLabelTextByLang(TXT_SMS_CB_RECEIVE_NEW, &string);
        
        MMIAPICOM_Wstrncpy(s_newmessage_title,string.wstr_ptr,string.wstr_len);        

        sprintf(count_str, " (%d)", newmessage_num);
        
        str_len = strlen(count_str);
        
        MMI_STRNTOWSTR(count_wstr, MMI_COUNT_STR_LEN, (const uint8*)count_str, MMI_COUNT_STR_LEN, str_len);
        
        MMIAPICOM_Wstrcat(s_newmessage_title, count_wstr);

        detail->notify_title.wstr_ptr = s_newmessage_title;

        detail->notify_title.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_title);
    }
    else
    {
        MMI_GetLabelTextByLang(TXT_SMS_CB_RECEIVE_NEW, &detail->notify_title);
    }

    SCI_MEMSET(s_newmessage_name, 0, sizeof(s_newmessage_name));
    MMIAPISMSCB_GetUnreadSMSCBChannel(s_newmessage_name, MMISMS_PBNAME_MAX_LEN);

    detail->notify_string.wstr_ptr = s_newmessage_name;
    detail->notify_string.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_name);
    
    SCI_MEMSET(s_newmessage_time, 0, sizeof(s_newmessage_time));
    MMIAPISMSCB_GetUnreadSMSCBTime(s_newmessage_time, MMI_DISPLAY_TIME_LEN);

    detail->notify_time.wstr_ptr = s_newmessage_time;
    
    detail->notify_time.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_time);
}
#endif

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//  Description : Get new Push detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetNewPushMessageDetail(MMIDROPDOWN_NOTIFY_DETAIL_T * detail)
{
    MMI_STRING_T string = {0};    
    wchar count_wstr[MMI_COUNT_STR_LEN] = {0};
    char count_str[MMI_COUNT_STR_LEN] = {0};
    uint16 str_len= 0;
    uint32 push_num = 0;
    MMISMS_ORDER_ID_T first_order_id = PNULL;
    uint8 len = 0;
    uint8 time[MMI_DISPLAY_TIME_LEN + 1] = {0};    
    MMI_TM_T tm = {0};

    first_order_id = MMIAPISMS_GetFirstUnreadPushMsgId();

    if(PNULL == first_order_id)
    {
        return;
    }

    detail->notify_image = GetNewPushNotifyImage();

    push_num = MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_WAP_PUSH) + MMIAPISMS_GetTobeReadMsgCount(MN_DUAL_SYS_MAX, MMISMS_TYPE_WAP_OTA);
    if(1 == push_num)
    {
        MMI_GetLabelTextByLang(TXT_SMS_NEW_MSG, &detail->notify_title);        
    }
    else
    {
        SCI_MEMSET(&s_newmessage_title, 0, sizeof(s_newmessage_title));

        MMI_GetLabelTextByLang(TXT_SMS_NEW_MSGS, &string);

        MMIAPICOM_Wstrncpy(s_newmessage_title,string.wstr_ptr,string.wstr_len);        

        sprintf(count_str, " (%d)", push_num);

        str_len = strlen(count_str);

        MMI_STRNTOWSTR(count_wstr, MMI_COUNT_STR_LEN, (const uint8*)count_str, MMI_COUNT_STR_LEN, str_len);

        MMIAPICOM_Wstrcat(s_newmessage_title, count_wstr);

        detail->notify_title.wstr_ptr = s_newmessage_title;

        detail->notify_title.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_title);
    }
    if(first_order_id->flag.msg_type == MMISMS_TYPE_WAP_PUSH)
    {
        MMI_GetLabelTextByLang(TXT_OTA_PUSH_ARRIVEED, &detail->notify_string);
    }
#ifdef MMS_SUPPORT
    else
    {
        MMI_GetLabelTextByLang(TXT_MMS_RECEIVE_CONFIG_PUSH, &detail->notify_string);
    }
#endif

    SCI_MEMSET(&s_newpush_time, 0, sizeof(s_newpush_time));
    tm = MMIAPICOM_Second2Tm(first_order_id->time);
    MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time,MMI_DISPLAY_TIME_LEN + 1 );
    len = (uint16)strlen((char*)time);
    MMI_STRNTOWSTR(s_newpush_time, len, (const uint8*)time, len, len);
    detail->notify_time.wstr_ptr = s_newpush_time;
    
    detail->notify_time.wstr_len = MMIAPICOM_Wstrlen(s_newpush_time);
}
#endif

/*****************************************************************************/
//  Description : Get STK Idle Text
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void GetSTKIdleTextDetail(MMIDROPDOWN_NOTIFY_DETAIL_T * detail)
{
    if (MMIIDLE_GetIdleWinInfo()->is_display_stk_idle_text)
    {
        BOOLEAN is_exist_stk_name = FALSE;
        MMI_STRING_T stk_name = {0};
        IDLE_STK_TEXT_INFO* stk_text_info_ptr = MMIIDLECOM_GetStkTextInfo();
        BOOLEAN is_icon_exist = FALSE;
        BOOLEAN is_self_explanatory = FALSE;

        is_exist_stk_name = MMIAPISTK_GetStkName(stk_text_info_ptr->dual_sys, &stk_name);

        if (!is_exist_stk_name)
        {
            MMI_GetLabelTextByLang(TXT_COMMON_MAIN_STK, &stk_name);
        }

        detail->notify_title.wstr_ptr = stk_name.wstr_ptr;
        detail->notify_title.wstr_len = stk_name.wstr_len;

        MMIAPISTK_GetIdleTextIcon(&is_icon_exist, &is_self_explanatory);

        if (!is_icon_exist || !is_self_explanatory)
        {
            detail->notify_string.wstr_ptr = stk_text_info_ptr->stk_idle_str.wstr_ptr;
            detail->notify_string.wstr_len = stk_text_info_ptr->stk_idle_str.wstr_len;
        }
    }
}

/*****************************************************************************/
//  Description : Get new message number
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL uint8 GetNewMessageNumber(
                                      uint8                *num_ptr, 
                                      uint8                 max_num_len                                         
                                      )
{
    uint8 len = 0;
    uint8 i = 0;
    MMISMS_ORDER_ID_T cur_order_id = PNULL;

    cur_order_id = MMIAPISMS_GetFirstUnreadMsgId();
 
    if (PNULL == cur_order_id ||PNULL == num_ptr )
    {
        return 0;
    }
    
    if (cur_order_id->flag.msg_type == MMISMS_TYPE_SMS)
    {
        MMIPB_BCD_NUMBER_T pb_bcd = MMIAPISMS_SmsBCDToPbBCD(cur_order_id->number.bcd_number);

        len = MMIAPIPB_BCDNumberToString(&pb_bcd, max_num_len, num_ptr);

//         len = MMIAPICOM_GenDispNumber(
//             MMIPB_GetNumberTypeFromUint8(cur_order_id->number.bcd_number.npi_ton),
//             MIN(MN_MAX_ADDR_BCD_LEN, cur_order_id->number.bcd_number.number_len),
//             cur_order_id->number.bcd_number.number,
//             num_ptr,
//             max_num_len
//             );        
    }
    else
    {
        len = strlen((char *)cur_order_id->number.sender);

        for (i = 0; i < len; i++)
        {
            if (MMIAPICOM_IsTelephoneNum(*(cur_order_id->number.sender+i)))
            {
                *(num_ptr + i) = *(cur_order_id->number.sender + i);
            }
            else
            {
                len = i;
                break;
            }
        }
        *(num_ptr + len) = 0;
    }
	
    return (len);
}

/*****************************************************************************/
//  Description : Get new message time
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void GetNewMessageTime(wchar    *time_ptr)
{
    uint8 len = 0;
    uint8 time[MMI_DISPLAY_TIME_LEN + 1] = {0};    
    MMI_TM_T   tm = {0};
    
    MMISMS_ORDER_ID_T   cur_order_id = MMIAPISMS_GetFirstUnreadMsgId();
    
    if (PNULL == cur_order_id || PNULL == time_ptr)
    {
        return;
    }
    
    tm = MMIAPICOM_Second2Tm(cur_order_id->time);
    
    MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time,MMI_DISPLAY_TIME_LEN + 1 );

    len =  (uint16)strlen((char*)time);

    MMI_STRNTOWSTR(time_ptr, len, (const uint8*)time, len, len);
}

/*****************************************************************************/
//  Description : Get new message detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
/*
LOCAL void GetNewMessageDetail(MMIDROPDOWN_NOTIFY_DETAIL_T *   detail)
{
    uint8 number_len = 0;	
    uint8 number[MMISMS_PBNUM_MAX_LEN + 1] = {0};
    BOOLEAN 	find_result =FALSE;

    detail->notify_image = GetNewMessageNotifyImage();
    MMI_GetLabelTextByLang(TXT_SMS_NEW_MSG, &detail->notify_title);

    number_len = GetNewMessageNumber(
		number,
		MMISMS_PBNUM_MAX_LEN
		);

    if(number_len > 0)
    {
        SCI_MEMSET(&s_newmessage_name, 0, sizeof(s_newmessage_name));
        
        detail->notify_string.wstr_ptr = s_newmessage_name;        
        
        find_result = MMIAPISMS_GetNameByNumberFromPB(number, &detail->notify_string, MMISMS_PBNAME_MAX_LEN);
    	
        if (!find_result)
        {
            detail->notify_string.wstr_len = MMISMS_PBNAME_MAX_LEN;
    		
            MMI_STRNTOWSTR(detail->notify_string.wstr_ptr,
    			detail->notify_string.wstr_len,
    			(const uint8*)number,
    			number_len,
    			number_len);
    		
            detail->notify_string.wstr_len = number_len;
        }	
    }

    SCI_MEMSET(&s_newmessage_time, 0, sizeof(s_newmessage_time));

    GetNewMessageTime(s_newmessage_time);    

    detail->notify_time.wstr_ptr = s_newmessage_time;
    
    detail->notify_time.wstr_len = MMIAPICOM_Wstrlen(s_newmessage_time);
}
*/

#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
int16 ucs_strlen(uint16 * strUnicode)
{
	int16 i = 0;
	while (strUnicode[i])
		i++;
	return i;
}

void GetWreAppTitleFromDropdownWin(PWREAPP_INFO_T wreApp, MMI_STRING_T     *str_ptr)
{
	str_ptr->wstr_len = ucs_strlen(wreApp->appname);//(uint16)UnicodeStrLen((char*)str_ptr->wstr_ptr);
	memset((void *)str_ptr->wstr_ptr, 0, (WRE_APP_TITLE_LEN+1)*sizeof(unsigned short));
	memcpy((void*)str_ptr->wstr_ptr, (void*)wreApp->appname, str_ptr->wstr_len*sizeof(unsigned short));
}

void GetWreAppStringFromDropdownWin(PWREAPP_INFO_T wreApp, MMI_STRING_T     *str_ptr)
{
	str_ptr->wstr_len = ucs_strlen(wreApp->appname);//(uint16)UnicodeStrLen((char*)str_ptr->wstr_ptr);
	memset((void *)str_ptr->wstr_ptr, 0, (WRE_APP_STRING_LEN+1)*sizeof(unsigned short));
	memcpy((void*)str_ptr->wstr_ptr, (void*)wreApp->appname, str_ptr->wstr_len*sizeof(unsigned short));
}
#endif

/*****************************************************************************/
//  Description : Get notify detail
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN GetNotifyDetail(
                        uint32    index,
                        MMIDROPDOWN_NOTIFY_DETAIL_T *   detail
                        )
{
    BOOLEAN    result = TRUE;
    MMI_TM_T   tm = {0};
    uint8    time[MMI_DISPLAY_TIME_LEN + 1 ] = {0};
//    MMI_STRING_T    notify_string = {0};
    
//    SCI_ASSERT(PNULL != detail);    
    if (PNULL == detail)
    {
        return FALSE;
    }
    
    switch(s_notify_list[index].notify_type)
    {
        case DROPDOWN_NOTIFY_MISS_CALL:
            GetMissCallDetail(detail);
            break;

        case DROPDOWN_NOTIFY_NEW_MESSAGE:
            GetNewMessageDetail(detail);
            break;

#if defined(MMI_SMSCB_SUPPORT)
        case DROPDOWN_NOTIFY_NEW_SMSCB:
            GetNewSMSCBDetail(detail);;
            break;
#endif

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
        case DROPDOWN_NOTIFY_NEW_PUSH:
            GetNewPushMessageDetail(detail);
            break;
#endif

        case DROPDOWN_NOTIFY_STK_IDLE_TEXT:
            GetSTKIdleTextDetail(detail);
            break;

        case DROPDOWN_NOTIFY_MESSAGE_FULL:
            detail->notify_image = GetMessageFullNotifyImage();
            if (MMIAPISMS_IsSMSFull()) //短信满
            {
                MMI_GetLabelTextByLang(TXT_SMS_MEM_FULL_ALERT_USER, &detail->notify_title);
            }
#ifdef MMS_SUPPORT
            else if (MMIAPIMMS_IsMMSFull()) //彩信满
            {
                MMI_GetLabelTextByLang(TXT_MMS_MEMORYFULL, &detail->notify_title);
            }
#endif
            else
            {
                MMI_GetLabelTextByLang(TXT_DC_NOEMPTYSPACE, &detail->notify_title);
            }
            tm = MMIAPICOM_Second2Tm(s_notify_list[index].notify_time);
            MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time,MMI_DISPLAY_TIME_LEN + 1 );
            SCI_MEMSET(&s_messagefull_time, 0, sizeof(s_messagefull_time));            
            detail->notify_time.wstr_ptr = s_messagefull_time;            
            detail->notify_time.wstr_len = (uint16)strlen((char*)time);            
            MMI_STRNTOWSTR( s_messagefull_time, MMI_DISPLAY_TIME_LEN, (uint8 *)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len );
            break;           

#ifdef MMI_AUDIO_PLAYER_SUPPORT
        case DROPDOWN_RUNNING_PLAYER:
            {
#ifdef MMI_AUDIOPLAYER_ALBUM_SUPPORT
                /*cr127056 renwei add*/
                MMIMP3_ALBUM_INFO_T album_ptr = {0};

                MMIAPIMP3_GetCurMp3AlbumInfo(&album_ptr);

                detail->notify_title.wstr_len = MMIAPICOM_Wstrlen(album_ptr.artist);
                
                if(detail->notify_title.wstr_len > 0)
                {
                    SCI_MEMSET(&s_mp3_artist, 0, sizeof(s_mp3_artist));
                    MMIAPICOM_Wstrcpy(s_mp3_artist, album_ptr.artist);
                    detail->notify_title.wstr_ptr = s_mp3_artist;
                }
                /*cr127056 renwei add*/
#endif                

                detail->notify_string.wstr_ptr = s_mp3_name;
                SCI_MEMSET(&s_mp3_name, 0, sizeof(s_mp3_name));
                MMIAPIMP3_GetCurMp3Name(&detail->notify_string, MMIMP3_NAME_MAX_LEN);
                
                detail->notify_image = GetMp3NotifyImage();
                detail->action_image = GetMp3ActionImage();
                break;
            }
            
#endif

#ifdef FM_SUPPORT
        case DROPDOWN_RUNNING_FM:
            MMI_GetLabelTextByLang(TXT_FM, &detail->notify_title);
            detail->notify_string.wstr_ptr = s_fm_name;
            SCI_MEMSET(&s_fm_name, 0, sizeof(s_fm_name));
            MMIAPIFM_GetCurFreqName(&detail->notify_string,MMIMP3_NAME_MAX_LEN);
            detail->notify_image = GetFMNotifyImage();
            detail->action_image = GetFMActionImage();
            break;
#endif

#ifdef MRAPP_SUPPORT
		case DROPDOWN_RUNNING_MRAPP:
            GetMRAPPNotifyTitle(&detail->notify_title);
			detail->notify_image = GetMRAPPNotifyImage();
			detail->action_image = GetMRAPPActionImage();
            break;	
#ifdef __MMI_SKYQQ__
		case DROPDOWN_RUNNING_SKYQQ:
			MMI_GetLabelTextByLang(TXT_MRAPP_QQ, &detail->notify_title);
			detail->notify_image = GetSkyQQNotifyImage();
			detail->action_image = GetSkyQQActionImage();
            break;		
#endif	
#endif

#ifdef BLUETOOTH_SUPPORT
        case DROPDOWN_RUNNING_BT_TRANSFERFILE:
            MMI_GetLabelTextByLang(TXT_DROPDOWN_BT_TRANSFERRING, &detail->notify_string);
            detail->action_image = GetBTActionImage();
            break;
#endif

#ifdef JAVA_SUPPORT
        case DROPDOWN_RUNNING_JAVA://cr00115796 renwei modify
            detail->notify_image = IMAGE_SECMENU_ICON_PASTIME_JAVA; //cr125321 renwei modify;
            MMI_GetLabelTextByLang(TXT_COMM_JAVAAPP, &detail->notify_title);
            MMI_GetLabelTextByLang(TXT_DROPDOWN_JAVA_RUNNING, &detail->notify_string);
            tm = MMIAPICOM_Second2Tm(s_notify_list[index].notify_time);
            MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time,MMI_DISPLAY_TIME_LEN + 1 );
            SCI_MEMSET(&s_java_time, 0, sizeof(s_java_time));            
            detail->notify_time.wstr_ptr = s_java_time;            
            detail->notify_time.wstr_len = (uint16)strlen((char*)time);            
            MMI_STRNTOWSTR( s_java_time, MMI_DISPLAY_TIME_LEN, (uint8 *)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len );
            break;
#endif

#ifdef QQ_SUPPORT
        case DROPDOWN_NOTIFY_QQ:
            detail->notify_image = MMIAPIQQ_GetIdleImageID();
            MMI_GetLabelTextByLang(TXT_SPREADTRUM_QQ_SUPPORT, &detail->notify_title);
            MMI_GetLabelTextByLang(TXT_DROPDOWN_QQ_RUNNING, &detail->notify_string);
            tm = MMIAPICOM_Second2Tm(s_notify_list[index].notify_time);
            MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time,MMI_DISPLAY_TIME_LEN + 1 );
            SCI_MEMSET(&s_qq_time, 0, sizeof(s_qq_time));            
            detail->notify_time.wstr_ptr = s_qq_time;            
            detail->notify_time.wstr_len = (uint16)strlen((char*)time);            
            MMI_STRNTOWSTR( s_qq_time, MMI_DISPLAY_TIME_LEN, (uint8 *)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len );
            break;
#endif

//nicklei add
#ifdef MCARE_V31_SUPPORT
    case DROPDOWN_NOTIFY_MCARE_QQ:
	        //detail->notify_image = McfIF_GetIdleIconImageID(DROPDOWN_NOTIFY_MCARE_QQ);
	        detail->notify_image = McfIdle_GetIdleIconImageIDQqOnline();			
            MMI_GetLabelTextByLang(TXT_COMM_QQ, &detail->notify_title);
            MMI_GetLabelTextByLang(TXT_MCARE_QQ_IS_RUNNING, &detail->notify_string);
            tm = MMIAPICOM_Second2Tm(s_notify_list[index].notify_time);
            MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time, MMI_DISPLAY_TIME_LEN + 1);
            SCI_MEMSET(&s_mcare_qq_time, 0, sizeof(s_mcare_qq_time));
            detail->notify_time.wstr_ptr = s_mcare_qq_time;
            detail->notify_time.wstr_len = (uint16)strlen((char*)time);
            MMI_STRNTOWSTR(s_mcare_qq_time, MMI_DISPLAY_TIME_LEN, (uint8*)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len);
            break;
    case DROPDOWN_NOTIFY_MCARE_QC:
	        //detail->notify_image = McfIF_GetIdleIconImageID(DROPDOWN_NOTIFY_MCARE_QC);
	        detail->notify_image = McfIdle_GetIdleIconImageIDContact();
            MMI_GetLabelTextByLang(TXT_MCARE_QC_SUPPORT, &detail->notify_title);
            MMI_GetLabelTextByLang(TXT_MCARE_QC_IS_RUNNING, &detail->notify_string);
            tm = MMIAPICOM_Second2Tm(s_notify_list[index].notify_time);
            MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,time, MMI_DISPLAY_TIME_LEN + 1);
            SCI_MEMSET(&s_mcare_qc_time, 0, sizeof(s_mcare_qc_time));
            detail->notify_time.wstr_ptr = s_mcare_qc_time;
            detail->notify_time.wstr_len = (uint16)strlen((char*)time);
            MMI_STRNTOWSTR(s_mcare_qc_time, MMI_DISPLAY_TIME_LEN, (uint8*)time, detail->notify_time.wstr_len, detail->notify_time.wstr_len);
            break;
#endif

#ifdef COUNTEDTIME_SUPPORT
        case DROPDOWN_RUNNING_COUNTEDTIME:
            {
                SCI_TIME_T cur_time = MMIAPICT_GetCurTime();            
                uint8 time_str[9] = {0};
//#if (defined MMIWIDGET_SUPPORT) || (defined MMI_GRID_IDLE_SUPPORT) || (defined WEATHER_SUPPORT)
#ifdef MMI_GRID_COUNTEDTIME
                detail->notify_image = IMAGE_DROPDOWN_TIMER;//
#else 
                detail->notify_image = 0;
#endif
                
                MMI_GetLabelTextByLang(TXT_TOOLS_COUNTEDTIME, &detail->notify_title);
                sprintf((char*)time_str, "%02d:%02d:%02d", cur_time.hour, cur_time.min, cur_time.sec);    
                detail->notify_string.wstr_ptr = s_countedtime_value;
                SCI_MEMSET(&s_countedtime_value, 0, sizeof(s_countedtime_value));
                MMIAPICOM_StrToWstr(time_str, detail->notify_string.wstr_ptr);
                detail->notify_string.wstr_len = MMIAPICOM_Wstrlen(detail->notify_string.wstr_ptr);
            }
            break;
#endif
        case DROPDOWN_RUNNING_CALLING:
            {
                MMI_STRING_T	title_str[4] = {0};
                wchar temp_wchar[CC_DISPLAY_TIME_LEN + 3]={0};
                wchar bracket_l[]=L"(";
                wchar bracket_r[]=L")";

                MMI_GetLabelTextByLang(TXT_DROPDOWN_CALLING_TITLE, &title_str[0]);     

                title_str[1].wstr_ptr = bracket_l;
                title_str[1].wstr_len = MMIAPICOM_Wstrlen(bracket_l);

                title_str[2].wstr_len = sizeof(temp_wchar)/sizeof(wchar);
                title_str[2].wstr_ptr = temp_wchar;

                title_str[3].wstr_ptr = bracket_r;
                title_str[3].wstr_len = MMIAPICOM_Wstrlen(bracket_r);

                MMIAPICC_GetCallTimeStr(&title_str[2]);
                detail->notify_image          = IMAGE_SECMENU_ICON_CALL; //cr125321 renwei add 
                detail->notify_title.wstr_ptr = s_calling_name;
                detail->notify_title.wstr_len = sizeof(s_calling_name);
                detail->action_image = IMAGE_DROPDOWN_CC_HANGUP;
                //TXT_DROPDOWN_CALLING_TITLE+"("+prompt_str+")";
                MMIAPICOM_ConnectSomeString(title_str, 4, &detail->notify_title);

                MMIAPICC_GetCurrentCallString(&detail->notify_string);
            }
            break;

        case DROPDOWN_NOTIFY_USB_DEBUG:
            {
                MMI_TEXT_ID_T text_id = 0;

                MMI_GetLabelTextByLang(TXT_DROPDOWN_USB_DEBUG_TITLE, &detail->notify_title);
                text_id = MMIAPIUDISK_GetCurServiceNameTextID();
                MMI_GetLabelTextByLang(text_id, &detail->notify_string);
				detail->notify_image = IMAGE_DROPDOWN_PLUG_IN_USB;
            }
            break;

        case DROPDOWN_RUNNING_USB_USING:
            MMI_GetLabelTextByLang(TXT_DROPDOWN_USB_CONNECT_TITLE, &detail->notify_title);
            MMI_GetLabelTextByLang(TXT_DROPDOWN_USB_CONNECT_STRING, &detail->notify_string);
            break;            
            
#ifdef ASP_SUPPORT
        case DROPDOWN_NOTIFY_NEW_ASP:
            GetNewAspDetail(detail);
            break;
#endif

        default:
#ifdef WRE_SUPPORT//add for wre_background by heng.xiao
//#ifdef	WRE_APP_IN_DROPDOWN_WIN	//david li, 2012-01-06, add wre app into drop-down win list
		if (DROPDOWN_RUNNING_WRE_APP_START <= s_notify_list[index].notify_type && s_notify_list[index].notify_type <= DROPDOWN_RUNNING_WRE_APP_END)
		{
			BOOLEAN bRet;
			WREAPP_INFO_T wreApp;
			int appId;

			appId = s_notify_list[index].notify_type - DROPDOWN_RUNNING_WRE_APP_START;
			bRet = WRE_GetAppInfo(appId,&wreApp);

			detail->notify_title.wstr_ptr = wre_app_title[appId];
			GetWreAppTitleFromDropdownWin(&wreApp, &detail->notify_title);
			detail->notify_string.wstr_ptr = wre_app_string[appId];
			GetWreAppStringFromDropdownWin(&wreApp, &detail->notify_string);

			//detail->notify_image = GetFMNotifyImage();
			//detail->action_image = GetFMActionImage();
			detail->notify_image = GetMp3NotifyImage();
			detail->action_image = GetMp3ActionImage();
		}
		else
#endif

            result = FALSE;
            break;
    }

    return result;
}

/*****************************************************************************/
//  Description : Get notify number
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL uint8 GetNotifyNumber(void)
{
    uint32    i = 0;
    uint8    num = 0;

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
        if ((TRUE ==s_notify_list[i].is_exist)
            && IsBelongCurPage(s_notify_list[i].notify_type, DROPDOWN_PAGE_NOTIFY))
        {
            num ++;
        }
    }
    
    //SCI_TRACE_LOW:"GetNotifyNumber, num = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_5843_112_2_18_2_15_0_10,(uint8*)"d",num);

    return num;
}


/*****************************************************************************/
//  Description : Add notify record
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void AddNotifyRecord(DROPDOWN_NOTIFY_E    notify_type)
{
    uint32    i = 0;
    uint32    j = 1;
    MMIDROPDOWN_NOTIFY_ITEM_T    temp_list[DROPDOWN_NOTIFY_MAX + 1] = {0};
    MMIDROPDOWN_NOTIFY_ITEM_T    add_record = {0};
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;

    //SCI_TRACE_LOW:"AddNotifyRecord, notify_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_5861_112_2_18_2_15_0_11,(uint8*)"d",notify_type);

    MMINV_READ(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list,return_value);

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
        if(TRUE ==s_notify_list[i].is_exist)
        {
            if(notify_type != s_notify_list[i].notify_type)
            {            
                SCI_MEMCPY(&temp_list[j], &s_notify_list[i], sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));       

                j ++;
            }
        }
    }

    add_record.notify_type = notify_type;

    add_record.is_exist = TRUE;

    add_record.notify_time = MMIAPICOM_GetCurTime();
    
    SCI_MEMCPY(&temp_list[0], &add_record, sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));

    SCI_MEMCPY(&s_notify_list[0], &temp_list[0], DROPDOWN_NOTIFY_MAX*sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));

    MMINV_WRITE(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list);    
}


/*****************************************************************************/
//  Description : Delete notify record
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void DelNotifyRecord(DROPDOWN_NOTIFY_E    notify_type)
{
    uint32    i = 0;
    uint32    j = 0;
    MMIDROPDOWN_NOTIFY_ITEM_T    temp_list[DROPDOWN_NOTIFY_MAX + 1] = {0};
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;

    MMINV_READ(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list,return_value);

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
        if(TRUE ==s_notify_list[i].is_exist)
        {
            if(notify_type != s_notify_list[i].notify_type)
            {               
                SCI_MEMCPY(&temp_list[j], &s_notify_list[i], sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));        

                j ++;
            }
        }
    }

    SCI_MEMCPY(&s_notify_list[0], &temp_list[0], DROPDOWN_NOTIFY_MAX*sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));

    MMINV_WRITE(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list);
}

/*****************************************************************************/
//  Description : Init notify list
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
LOCAL void InitNotifyList(void)
{
    uint32    i = 0;
    uint32    j = 0;
    MMIDROPDOWN_NOTIFY_ITEM_T    temp_list[DROPDOWN_NOTIFY_MAX + 1] = {0};

    for (i = 0; i < DROPDOWN_NOTIFY_MAX; i++)
    {
        if(TRUE ==s_notify_list[i].is_exist)
        {
            if(DROPDOWN_NOTIFY_MISS_CALL == s_notify_list[i].notify_type
               || DROPDOWN_NOTIFY_NEW_MESSAGE == s_notify_list[i].notify_type
#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
               || DROPDOWN_NOTIFY_NEW_PUSH == s_notify_list[i].notify_type
#endif
               || DROPDOWN_NOTIFY_MESSAGE_FULL == s_notify_list[i].notify_type
#ifdef ASP_SUPPORT
               || DROPDOWN_NOTIFY_NEW_ASP == s_notify_list[i].notify_type
#endif
               )
            {               
                SCI_MEMCPY(&temp_list[j], &s_notify_list[i], sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));        

                j ++;
            }
        }
    }

    SCI_MEMCPY(&s_notify_list[0], &temp_list[0], DROPDOWN_NOTIFY_MAX*sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));

    MMINV_WRITE(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list);
}

/*****************************************************************************/
//  Description : Init dropdownwin module
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIDROPDOWNWIN_InitModule(void)
{    
    MMIDROPDOWNWIN_RegWinIdNameArr();
    
    MMIDROPDOWNWIN_RegNv();    
}

/*****************************************************************************/
//  Description : Init dropdown NV
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Init(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;

    MMINV_READ(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list,return_value);

    if (MN_RETURN_SUCCESS != return_value)
    {
        SCI_MEMSET(&s_notify_list, 0, DROPDOWN_NOTIFY_MAX*sizeof(MMIDROPDOWN_NOTIFY_ITEM_T));

        MMINV_WRITE(MMINV_DROPDOWN_NOTIFY_INFO,&s_notify_list);
    }
    else
    {
        InitNotifyList();
    }
}

/*****************************************************************************/
//  Description : Set dropdown win slide state
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_SetSlideState(void)
{
    s_dropdown.slide_state = MMK_TP_SLIDE_FLING;

    s_dropdown.slide_dir = SLIDE_DIR_DOWN;    
}

/*****************************************************************************/
//  Description : Create dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 只提供给状态栏下拉创建过程调用
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Create(void)
{
    //SCI_TRACE_LOW:"MMIDROPDOWNWIN_Create"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_6010_112_2_18_2_15_0_12,(uint8*)"");


    //目前MMI_WIN_TEXT_DATE为0，如果不为0，需要修改下面的0
    GUIWIN_SetStbItemVisible(0/*MMI_WIN_TEXT_DATE*/,TRUE);
#if 0//def MCARE_V31_SUPPORT
    if(McareV31_IsMcareRunning())
    {
        SCI_TRACE_LOW("McareV31_IsMcareRunning return TRUE");
        if(!MMK_IsFocusWin(MAIN_IDLE_WIN_ID) && !MMK_IsFocusWin(MAIN_MAINMENU_WIN_ID))
        {
			SCI_TRACE_LOW("MAIN_IDLE_WIN_ID MMK_IsOpenWin TRUE");  
            return;
        }
    }
#endif

    if(MMK_IsOpenWin(MMI_DROPDOWN_WIN_ID))
    {
        MMK_CloseWin(MMI_DROPDOWN_WIN_ID);
    }    
    
    SCI_MEMSET(&s_dropdown,0,sizeof(s_dropdown));
    s_dropdown.dropdown_layer.block_id = UILAYER_NULL_HANDLE;
    
    s_dropdown.statusbar_layer.block_id = UILAYER_NULL_HANDLE;

    s_dropdown.state = DROPDOWN_STATE_OPEN;
    SCI_MEMSET(&s_shortcut_data, 0, sizeof(s_shortcut_data));

    MMK_CreateWin( (uint32*)MMIDROPDOWN_WIN_TAB, PNULL);
    
    //清除tp状态，使dropdonw win可以处理tp up
    MMK_SetTPDownWin(0);
}


 /*****************************************************************************/
 //  Description : Add dropdown notify record
 //  Global resource dependence : none
 //  Author: Paul.huang
 //  Note: 
 /*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_AddNotifyRecord(DROPDOWN_NOTIFY_E 	notify_type)
// PUBLIC void MMIDROPDOWNWIN_AddNotifyRecord(
//                       DROPDOWN_NOTIFY_E   notify_type,
//                       MMIDROPDOWN_NOTIFY_CALLBACK  callback
//                       )
 {
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(MMI_DROPDOWN_WIN_ID,MMIDROPDOWN_LIST_CTRL_ID);
    
    AddNotifyRecord(notify_type);

    if(MMK_IsFocusWin(MMI_DROPDOWN_WIN_ID))
    {
        if(DROPDOWN_STATE_NONE == s_dropdown.state)
        {
            ReadNotifytoListBox(ctrl_id, FALSE);
            
            MMK_PostMsg(MMI_DROPDOWN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);            
        }
    }
 }

 /*****************************************************************************/
 //  Description : Del dropdown notify record
 //  Global resource dependence : none
 //  Author: Paul.huang
 //  Note: 
 /*****************************************************************************/
 PUBLIC void MMIDROPDOWNWIN_DelNotifyRecord(DROPDOWN_NOTIFY_E  notify_type)
 {
    MMI_CTRL_ID_T    ctrl_id = WIN_CTRL_ID(MMI_DROPDOWN_WIN_ID,MMIDROPDOWN_LIST_CTRL_ID);
    
    DelNotifyRecord(notify_type);

     if(MMK_IsFocusWin(MMI_DROPDOWN_WIN_ID))
     {
         if(DROPDOWN_STATE_NONE == s_dropdown.state)
         {
             ReadNotifytoListBox(ctrl_id, FALSE);
             
             MMK_PostMsg(MMI_DROPDOWN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
         }
     }
 }

/*****************************************************************************/
//  Description : Get Bar Image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MMIDROPDOWNWIN_GetBarImage(void)
{
    return IMAGE_DROPDOWN_BAR;
}

/*****************************************************************************/
//  Description : 更新下拉窗口
//  Global resource dependence : none
//  Author: paul.huang
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_UpdateMainWin(void)
{    
    if (MMK_IsFocusWin(MMI_DROPDOWN_WIN_ID))
    {
        if(DROPDOWN_STATE_NONE == s_dropdown.state)
        {
            ReloadPageContent(MMI_DROPDOWN_WIN_ID);
            
            MMK_PostMsg(MMI_DROPDOWN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
}

/*****************************************************************************/
//  Description : 打开下拉窗口
//  Global resource dependence : none
//  Author: xiaoming.ren
/*****************************************************************************/
//PUBLIC void MMIDROPDOWNWIN_OpenMainWin(void)
//{
//	MMK_PostMsg(VIRTUAL_WIN_ID, MSG_DROPDOWNWIN_OPEN, PNULL, NULL);
//}

/*****************************************************************************/
//  Description : 打开下拉窗口
//  Global resource dependence : none
//  Author: xiaoming.ren
//  Note: 提供给外部调用创建
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Open(void)
{
	MMIDROPDOWNWIN_Create();
	
	MMIDROPDOWNWIN_SetSlideState(); 
	
	s_dropdown.auto_slide = TRUE;
}

/*****************************************************************************/
//  Description : 关闭下拉窗口
//  Global resource dependence : none
//  Author: paul.huang
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Close(
                                 BOOLEAN is_show_anim
                                 )
{
    MMI_WIN_ID_T win_id = MMI_DROPDOWN_WIN_ID;

    //SCI_TRACE_LOW:"MMIDROPDOWNWIN_Close() is_show_anim = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDROPDOWNWIN_MAIN_6158_112_2_18_2_15_0_15,(uint8*)"d", is_show_anim);
    
    if (is_show_anim)
    {
        if(DROPDOWN_STATE_NONE == s_dropdown.state)
        {
            s_dropdown.state = DROPDOWN_STATE_CLOSE;
            
            s_dropdown.slide_state = MMK_TP_SLIDE_FLING;

            s_dropdown.slide_dir = SLIDE_DIR_UP;

            StartFling(win_id);
        }
    }
    else
    {
        if(MMK_IsOpenWin(win_id))
        {
            MMK_CloseWin(win_id);
        }
    }
}


#endif //PDA_UI_DROPDOWN_WIN
