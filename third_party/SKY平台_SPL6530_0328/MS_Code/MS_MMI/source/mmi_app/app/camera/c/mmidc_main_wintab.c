/*****************************************************************************
** File Name:      mmidc_preview_wintab.c                                    *
** Author:                                                                   *
** Date:           2006-5	                                                 *
** Copyright:      2003 Spreadtrum, Incorporated. All Rights Reserved.         *
** Description:    This file is used to describe dc preview window table     *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 05/2006        gang.tong        Create									 *
*****************************************************************************/

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "mmi_app_camera_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmieng_uitestwin.h"
#ifdef CAMERA_SUPPORT
#include "cafapplet_gen.h"
#include "mmidisplay_data.h"
#include "window_parse.h"
#include "mmi_common.h"
#include "mmiidle_subwintab.h"
#include "mmiphone_export.h"
#include "mmi_default.h"
#include "mmk_timer.h"
#include "mmi_appmsg.h"
#include "guicommon.h"
#include "guilcd.h"
#include "guiblock.h"
#include "mmidc_camera_id.h"
#include "mmipub.h"
#include "block_mem.h"
#include "mmidc_gui.h"
#include "mmidc_flow.h"
#include "mmidc_camera_text.h"
#include "mmi_common.h"
#include "guilistbox.h"
#include "mmi_module.h"
//#include "mmi_filetask.h"
#include "guitext.h"
#include "mmiudisk_export.h"
#include "mmidc_display.h"
#include "mmidc_option.h"
#include "mmidc_export.h"
#include "mmisd_export.h"
//#include "mmieng_uitestwin.h"
#include "guirichtext.h"
#include "mmidc_save.h"
#include "mmidc_setting.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "guires.h"
#include "ui_layer.h"
#include "guiref_scale.h"
#include "mmitheme_update.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#include "java_mmi_text.h"
#endif
#ifdef QQ_SUPPORT
#include "qq2009.h"
#include "qq_text.h"
#endif
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#endif

#include "mmipicview_export.h"
//other module
#include "mmieng_uitestwin.h"
#include "mmicc_export.h"
#include "mmiset_id.h"

//local module
#ifdef MMI_CAMERA_F_PHOTO_DATE
#include "mmidc_photodate.h"
#endif
#if defined(MMIDC_DESKTOP_ICON_BAR)    
#include "mmidc_guiiconbar.h"
#endif

#ifdef MMIDC_DESKTOP_PDA
#include "mmidc_guipda.h"
#endif
#ifdef MET_MEX_SUPPORT
#include "met_mex_export.h"
#endif
#ifdef MMIDC_F_WORK_MODE
#include "mmidc_workmode.h"
#endif

#include "mmialarm_export.h"
#ifdef MMIDC_F_EFFECT_360
#include "mmidc_effect360.h"
#endif

#if defined(MMIDC_F_U_CAMERA)
#include "mmidc_ucamera_wintab.h"
#include "mmidc_imageprocess.h"
#endif


#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif

#if (defined MCARE_V31_SUPPORT)
#include "McfInterface.h"
#endif

#include "mmidc_main.h"
//#include "mmiset.h"
#include "mmiset_export.h"
/*---------------------------------------------------------------------------*/
/*                          MACRO DEFINITION                                 */
/*---------------------------------------------------------------------------*/
#define MIN_FILE_SIZE                  (1024*2)
#define WIN_ID_DEF(win_id, win_id_name)          win_id_name,

const uint8 mmidc_id_name_arr[][MMI_WIN_ID_NAME_MAX_LENGTH] =
{
#include "mmidc_camera_id.def"    
};
#undef WIN_ID_DEF




typedef void (*DCKEYFUNCTION)(void);

/*---------------------------------------------------------------------------*/
/*                         STATIC DEFINITION                                 */
/*---------------------------------------------------------------------------*/
LOCAL uint8				s_timer_delay = 0; // used for delay shoot
LOCAL uint8				s_3seconds_timer = 0;    //used for close OSD in 3 seconds if user doesn't do any operations
LOCAL uint8				s_timer_count_down = 0;
LOCAL uint8				s_updown_tip_timer = 0;
LOCAL uint8				s_record_timer = 0;
LOCAL uint8				s_text_scroll_timer = 0;
LOCAL uint32			s_hit_timer = 0;
LOCAL uint8				s_switch_osd_key_timer_id = 0;             //key timer
LOCAL DIRECTION_KEY		s_last_key_down = CANCEL_KEY;  

#ifdef WIN32
LOCAL uint32			s_record_total_time = 0;
#endif

LOCAL BOOLEAN       s_is_calcuate_space = FALSE;
LOCAL uint8         s_is_chip_test_timer = 0;
LOCAL BOOLEAN       s_is_chip_test = FALSE;

LOCAL BOOLEAN       s_is_pressed_save = FALSE;
LOCAL BOOLEAN       s_is_dv_pause = FALSE;
LOCAL BOOLEAN       s_is_capture_success_msg = FALSE;
LOCAL BOOLEAN        s_is_back_to_preview = FALSE;

LOCAL MMIDC_CHIPTEST_STEP_E s_chiptest_step = MMIDC_CHIPTEST_STEP_START;
LOCAL VIDEO_SIZE_E          s_chiptest_last_video_size = VIDEO_SIZE_128X96;
LOCAL AUTO_SAVE_E           s_chiptest_last_is_autosave = AUTO_SAVE_OFF;

LOCAL MMISRV_HANDLE_T   s_dc_audio_handle = 0;

LOCAL BOOLEAN s_need_full_paint = FALSE;

LOCAL MMIDCSRV_INFO_T s_mmidcsrv_info = {0};
/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleCameraWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E	msg_id, DPARAM param);

/*****************************************************************************/
// 	Description : handle photo and video send window msg
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSendWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*****************************************************************************/
// 	Description : handle the help window msg
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleHelpWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void KeyFunction(DC_KEY_E key);

/*****************************************************************************/
// 	Description : close hit timer
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void CloseHitTimer(void);

/*****************************************************************************/
// 	Description : close all times 
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void CloseDCAllTimers(void);

/*****************************************************************************/
// 	Description : init working parameter for get focus
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL int32 InitWorkParameterForGetFocus(void);

/*****************************************************************************/
// 	Description : get chip test step
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL MMIDC_CHIPTEST_STEP_E GetChipTestStep(void);

/*****************************************************************************/
// 	Description : set chip test step
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void SetChipTestStep(MMIDC_CHIPTEST_STEP_E step);

/*****************************************************************************/
// 	Description : reset chip test timer
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void ResetChipTestTimer(int count_s);

/*****************************************************************************/
// 	Description : close chip test timer
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void CloseChipTestTimer(void);

/*****************************************************************************/
// 	Description : set chip test step
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void ProcessChipTestStep(void);

/*****************************************************************************/
// 	Description : get chip test last video size
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void GetChipTestLastVideoSize(void);

/*****************************************************************************/
// 	Description : restore chip test last video size
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void RestoreChipTestLastVideoSize(void);

/*****************************************************************************/
// 	Description : get chip test last auto save
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void GetChipTestLastAutoSave(void);

/*****************************************************************************/
// 	Description : restore chip test last auto save
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void RestoreChipTestLastAutoSave(void);

/*****************************************************************************/
//Description :  handle shortcut key
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void HandleShortcutKey(MMI_MESSAGE_ID_E msg_id);

#if defined(MMIDC_DESKTOP_ICON_BAR)
/*****************************************************************************/
//Description :  handle tp for icon bar
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleTPForIconBar(GUI_POINT_T point);

#if defined(MMIDC_F_DV_ZOOM)
/*****************************************************************************/
//Description :  handle tp for recording zoom button
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleTPForRecordingZoomButton(GUI_POINT_T point);
#endif
#endif

/*****************************************************************************/
// 	Description : init working parameter 
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL int32 InitWorkParameter(void);

/*****************************************************************************/
//Description : init working environment 
//Global resource dependence : none
//Author: ryan.xu
//Note:
/*****************************************************************************/
LOCAL int32 InitWorkEnvironment(void);

/*****************************************************************************/
//Description : init working environment 
//Global resource dependence : none
//Author: ryan.xu
//Note:
/*****************************************************************************/
LOCAL int32 RestoreWorkEnvironment(void);

/*****************************************************************************/
//Description : handle the message of dc wait window
//Global resource dependence : none
//Author: robert.wang
//Note: for release wallpaper buffer before open dc
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDCWaitWinMsg(
                                      MMI_WIN_ID_T win_id, 
                                      MMI_MESSAGE_ID_E msg_id, 
                                      DPARAM param	
                                      );
                                      
/*****************************************************************************/
//Description : close dc applet
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
LOCAL void CloseDCApplet(void);

/*****************************************************************************/
//Description : start dc applet
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
LOCAL BOOLEAN StartDCApplet(CAMERA_MODE_E mode
                                );
                                
/*****************************************************************************/
//Description : register dc module applet info
//Global resource dependence : 
//Author:robert.wang
//Note:
/*****************************************************************************/
LOCAL void RegDCAppletInfo(void);

/*****************************************************************************/
//Description : the process message function of the camera
//Global resource dependence : 
//Author:robert.wang
//Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E  DCApplet_HandleEvent(    
                                                IAPPLET_T*          iapplet_ptr, //in
                                                MMI_MESSAGE_ID_E    msg_id, //in
                                                DPARAM              param //in
                                                );
                                                
/*****************************************************************************/
//  Description : process stop record
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL void ProcessStopRecord(DV_END_TYPE_E end_type);


#if defined(MMIDC_DESKTOP_PDA)

/*****************************************************************************/
//Description : delete image
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void DeleteImage(void);
#endif

/*---------------------------------------------------------------------------*/
/*                          CONSTANT VARIABLES                               */
/*---------------------------------------------------------------------------*/
WINDOW_TABLE(MMIDC_DC_TAB) = 
{
    //CLEAR_LCD,
        //WIN_PRIO( WIN_ONE_LEVEL ),
        WIN_FUNC((uint32)HandleCameraWinMsg), 
        WIN_ID(MMIDC_MAIN_WIN_ID),
        WIN_STYLE(WS_DISABLE_COMMON_BG),
        WIN_MOVE_STYLE(MOVE_FORBIDDEN),
        WIN_HIDE_STATUS,
        END_WIN
};

WINDOW_TABLE(MMIDC_DC_WAIT_TAB) = 
{
    WIN_FUNC((uint32)HandleDCWaitWinMsg), 
    WIN_ID(MMIDC_MAIN_WAIT_WIN_ID),
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    WIN_HIDE_STATUS,
    END_WIN
};


WINDOW_TABLE(MMIDC_SEND_TAB) = 
{
    //CLEAR_LCD,
        //WIN_PRIO( WIN_ONE_LEVEL ),
        WIN_FUNC((uint32)HandleSendWinMsg),
        WIN_ID(MMIDC_SEND_WIN_ID),
        WIN_TITLE(TXT_SEND),
#if defined(MMS_SUPPORT) || defined(BLUETOOTH_SUPPORT) || defined(SNS_SUPPORT)       
        CREATE_MENU_CTRL(MMIDC_SEND_OPT, MMIDC_SEND_MENU_CTRL_ID),
        WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
//        WIN_STYLE(WS_DISABLE_COMMON_BG),
#endif        
        END_WIN
};

WINDOW_TABLE(MMIDC_HELP_TAB) = 
{
    WIN_FUNC((uint32)HandleHelpWinMsg),    
    WIN_ID(MMIDC_HELP_WIN_ID),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    WIN_TITLE(TXT_HELP),
	CREATE_TEXT_CTRL(MMIDC_HELP_TEXT_CTRL_ID),
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    END_WIN
};

//applet的数组
LOCAL CAF_STATIC_APPLET_T s_mmidc_applet_array[] = 
{
    {
        SPRD_CAMERA_APPLET_ID, 
        sizeof(MMIDC_APPLET_T), 
        DCApplet_HandleEvent , 
        IMAGE_SECMENU_ICON_MULTIM_CAMERA, 
        STXT_MAIN_CAMERA
     }
};

//applet的信息, 被mmi_applet_table.c extern
LOCAL CAF_STATIC_APPLET_INFO_T g_mmidc_applet_info = 
{
    s_mmidc_applet_array,
    ARR_SIZE(s_mmidc_applet_array)
};

/*---------------------------------------------------------------------------*/
/*                          FUNCTION DEFINITION                              */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : HandleAlertWin
//  Global resource dependence :
//  Author: ryan.xu
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleAlertWin(MMI_WIN_ID_T          win_id,
                                  MMI_MESSAGE_ID_E      msg_id,
                                  DPARAM                param
                                  )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    switch (msg_id)
    {        
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : open alert window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_OpenAlertWin(									 
                               MMIPUB_SOFTKEY_STYLE_E  softkey_type,   //softkey type
                               MMI_TEXT_ID_T           text_id,        //text id
                               MMI_IMAGE_ID_T          image_id,       //image ptr
                               uint32                  time_out)       //time out
{
    MMIPUB_OpenAlertWinByTextIdEx(SPRD_CAMERA_APPLET_ID,\
                                &time_out,\
                                text_id,\
                                TXT_NULL,\
                                image_id,\
                                PNULL,\
                                PNULL,\
                                softkey_type,\
                                HandleAlertWin,\
                                PNULL \
                                );
}

/*****************************************************************************/
// 	Description : handle the help window msg
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleHelpWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E	msg_id, DPARAM param)
{
    MMI_RESULT_E        recode		= MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO	lcd_dev_info = {0};
    MMI_STRING_T        text_str = {0};
    GUI_POINT_T         dis_point = {0};
    MMIDC_DESKTOP_STYLE_E     desktop = 0;

    lcd_dev_info.lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id = GUI_BLOCK_MAIN;
    
    //SCI_TRACE_LOW:"[MMI DC]: HandleHelpWinMsg, msg_id = %x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_476_112_2_18_2_9_16_378,(uint8*)"d", msg_id);
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id, MMIDC_HELP_TEXT_CTRL_ID);

        desktop = MMIDC_GetDesktopStyle();
#ifdef MMIDC_DESKTOP_ICON_BAR
        if (DESKTOP_STYLE_ICON == desktop)
        {
#if defined (MMIDC_F_U_CAMERA)
            if(MMIDC_IsUCameraEnable())
            {
                MMI_GetLabelTextByLang(TXT_DC_UCAMERA_HELP_CONTENT, &text_str);
            }
            else
#endif
            {
                MMI_GetLabelTextByLang(TXT_DC_SHORTCUT_HELP_CONTENT, &text_str);
            }
        }
        else
#endif
        {
            MMI_GetLabelTextByLang(TXT_DC_HELP_CONTENT, &text_str);
        }
        
        GUITEXT_SetString(MMIDC_HELP_TEXT_CTRL_ID, text_str.wstr_ptr, text_str.wstr_len, FALSE);
        break;
        
    case MSG_FULL_PAINT:
        dis_point.x = 0;
        dis_point.y = MMI_TITLE_HEIGHT;
        GUIRES_DisplayImg(&dis_point,
            PNULL,
            PNULL,
            win_id,
            IMAGE_COMMON_BG,
            &lcd_dev_info);
        break;
        
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode; 
}

/*****************************************************************************/
// 	Description : set lcd infor for rotate dc
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_OpenHelpWin(void)
{
    MMIDC_CreateWinByApplet((uint32*)MMIDC_HELP_TAB, (ADD_DATA)PNULL);
}

/*****************************************************************************/
// 	Description : set lcd infor for rotate dc
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_SetLcdForRotate(BOOLEAN set)
{
    LCD_ANGLE_E  platform_angle = MMIDC_GetPlatformAngle();
    LCD_ANGLE_E  current_angle = GUILCD_GetLogicAngle((LCD_ID_E)GUI_MAIN_LCD_ID);

    //SCI_TRACE_LOW:"[MMIDC] MMIDC_SetLcdForRotate set = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_547_112_2_18_2_9_16_379,(uint8*)"d", set);
    //SCI_TRACE_LOW:"[MMIDC] MMIDC_SetLcdForRotate platform_angle = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_548_112_2_18_2_9_16_380,(uint8*)"d", platform_angle);
    //SCI_TRACE_LOW:"[MMIDC] MMIDC_SetLcdForRotate current_angle = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_549_112_2_18_2_9_16_381,(uint8*)"d", current_angle);

    MMK_SetWinSupportAngle(MMIDC_MAIN_WIN_ID, WIN_SUPPORT_ANGLE_ALL);
    if(set)
    {
        if(LCD_ANGLE_90 != current_angle)
        {
            MMK_SetWinAngle( MMIDC_MAIN_WIN_ID, LCD_ANGLE_90, FALSE );
        }
    }
    else
    {
        if (current_angle != platform_angle)
        {
            MMK_SetWinAngle( MMIDC_MAIN_WIN_ID, platform_angle, FALSE );
        }
    }
    
    MMIDC_ResetLogicalScreen();
    MMK_SetWinSupportAngle(MMIDC_MAIN_WIN_ID, WIN_SUPPORT_ANGLE_CUR);

    if(s_need_full_paint && (MMIDC_MAIN_WIN_ID == MMK_GetFocusWinId()))
    {
        MMK_SendMsg(MMIDC_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
        s_need_full_paint = FALSE;
    }

}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleOSDKey(DIRECTION_KEY key)
{
    BOOLEAN   ret = FALSE;
    
    //SCI_TRACE_LOW:"[MMIDC]: HandleOSDKey key = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_580_112_2_18_2_9_16_382,(uint8*)"d", key);
#if !defined(MMIDC_DESKTOP_PDA)
    
    if(MMIDC_GetIconHandle()->IconIsOpen())
    {
        if(MMIDC_HandleAdjustKey(key))
        {
            MMIDC_SetShowState(MMIDC_SHOW_ICON);
            ret = TRUE;
        }
        else if(MMIDC_GetIconHandle()->IconHandleKeyDown(key))
        {
            MMIDC_SetShowState(MMIDC_SHOW_ICON);
            ret = TRUE;
        }
        else if(MMIDC_GetMenuHandle()->MenuIsOpen())
        {
            if(MMIDC_GetMenuHandle()->MenuHandleKeyDown(key))
            {
                MMIDC_SetShowState(MMIDC_SHOW_ICON);
                ret = TRUE;
            }
        }
    }
    else if(MMIDC_GetMenuHandle()->MenuIsOpen())
    {
        if(MMIDC_GetMenuHandle()->MenuHandleKeyDown(key))
        {
            if(MMIDC_GetIconHandle()->IconIsOpen())
            {
                MMIDC_SetShowState(MMIDC_SHOW_ICON);
            }
            else
            {
                MMIDC_SetShowState(MMIDC_SHOW_MENU);
            }
            ret = TRUE;
        }
    }

/*
    if (OK_KEY == key)
    {
        MMIDC_ClearMenuInPreview();
    }
*/
#endif

    return ret;
}

/*****************************************************************************/
// 	Description : start delay shoot timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void StartShootDelayTimer(uint32 time)
{
    if(0 == s_timer_delay)
    {
        s_timer_delay = MMK_CreateTimer(time, TRUE);			
    }
    if(0 == s_timer_count_down)
    {
        s_timer_count_down = MMIDC_GetSelfShootDelaySeconds();
    }
}

/*****************************************************************************/
// 	Description : close delay shoot timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void CloseShootDelayTimer(void)
{
    if(0 != s_timer_delay)
    {
        MMK_StopTimer(s_timer_delay);
        s_timer_delay = 0;
    }
}

/*****************************************************************************/
// 	Description : start osd menu display timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_StartTipDisplayTimer(void)
{
    if(0 == s_3seconds_timer)
    {
        s_3seconds_timer = MMK_CreateTimer(MMI_3SECONDS, TRUE);			
    }
    else
    {
        MMK_StopTimer(s_3seconds_timer);
        s_3seconds_timer = MMK_CreateTimer(MMI_3SECONDS, TRUE);			
    }
}

/*****************************************************************************/
// 	Description : close osd menu display timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void CloseTipDisplayTimer(void)
{
    if(0 != s_3seconds_timer)
    {
        MMK_StopTimer(s_3seconds_timer);
        s_3seconds_timer = 0;
    }
}

/*****************************************************************************/
// 	Description : close up down tip timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void CloseUpDownTipTimer(void)
{
    if(0 != s_updown_tip_timer)
    {
        MMK_StopTimer(s_updown_tip_timer);
        s_updown_tip_timer = 0;
    }
}

/*****************************************************************************/
// 	Description : start record timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void StartRecordTimer(void)
{
    if(0 == s_record_timer)
    {
        s_record_timer = MMK_CreateTimer(MMI_1SECONDS, TRUE);
    }
    else
    {
        MMK_StopTimer(s_record_timer);
        s_record_timer = MMK_CreateTimer(MMI_1SECONDS, TRUE);
    }
}

/*****************************************************************************/
// 	Description : close record timer
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void CloseRecordTimer(void)
{
    if(0 != s_record_timer)
    {
        MMK_StopTimer(s_record_timer);
        s_record_timer = 0;
    }
}

/*****************************************************************************/
//  Description : start switch osd key timer
//  Global resource dependence : 
//  Author: ryan.xu
//  Note:
/*****************************************************************************/
LOCAL void StartSwitchOSDMenuItemsKeyTimer(DIRECTION_KEY key)
{
    if (0 == s_switch_osd_key_timer_id)
    {
        s_switch_osd_key_timer_id = MMK_CreateTimer(300, FALSE);
        
    }
    else
    {
        //SCI_TRACE_LOW:"StartSwitchOSDMenuItemsKeyTimer: the key timer has started!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_745_112_2_18_2_9_16_383,(uint8*)"");
    }
    s_last_key_down = key;
}

/*****************************************************************************/
//  Description : stop switch osd key timer
//  Global resource dependence : 
//  Author: ryan.xu
//  Note:
/*****************************************************************************/
LOCAL void StopSwitchOSDMenuItemsKeyTimer(void)
{
    if (0 < s_switch_osd_key_timer_id)
    {
        MMK_StopTimer(s_switch_osd_key_timer_id);
        s_switch_osd_key_timer_id = 0;
    }
    else
    {
        //SCI_TRACE_LOW:"StopSwitchOSDMenuItemsKeyTimer: the key timer has stop!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_763_112_2_18_2_9_16_384,(uint8*)"");
    }
}

/*****************************************************************************/
//  Description : start text scroll timer
//  Global resource dependence : 
//  Author: ryan.xu
//  Note:
/*****************************************************************************/
PUBLIC void MMIDC_StartTextScrollTimer(void)
{
    if (0 == s_text_scroll_timer)
    {
        s_text_scroll_timer = MMK_CreateTimer(500, FALSE);
    }
    else
    {
        //SCI_TRACE_LOW:"MMIDC_StartTextScrollTimer: the key timer has started!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_779_112_2_18_2_9_16_385,(uint8*)"");
    }
}

/*****************************************************************************/
//  Description : stop text scroll timer
//  Global resource dependence : 
//  Author: ryan.xu
//  Note:
/*****************************************************************************/
PUBLIC void MMIDC_StopTextScrollTimer(void)
{
    if (0 < s_text_scroll_timer)
    {
        MMK_StopTimer(s_text_scroll_timer);
        s_text_scroll_timer = 0;
    }
    else
    {
        //SCI_TRACE_LOW:"MMIDC_StopTextScrollTimer: the key timer has stop!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_796_112_2_18_2_9_16_386,(uint8*)"");
    }
}

/*****************************************************************************/
// 	Description : Chec kDC Running Environment
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CheckDCRunningEnvironment(void)
{
#ifdef MET_MEX_SUPPORT
	if ( MMIMEX_ConflictPromt() )
	{
        // mex kernel is running and may result in mem conflict
		return FALSE;
	}
#endif
    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    else if (MMIAPIENG_GetIQModeStatus())
    {
        MMIPUB_OpenAlertWarningWin(TXT_IQ_DATA_PROCESSING);
        return FALSE;
    }

    if(s_is_calcuate_space)
    {
        MMIDC_OpenAlertWin(MMIPUB_SOFTKEY_ONE, TXT_ERROR, IMAGE_PUBWIN_SUCCESS, MMI_3SECONDS);
        return FALSE;
    }

    //QQ运行后，内存不足，进入DC要提示QQ退出
#ifdef QQ_SUPPORT
    if (QQ_IsRunning())
    {
        MMIPUB_OpenAlertWarningWin( TXT_EXIT_QQ);
        return FALSE;
    }
#endif

    //JAVA 后台运行,内存不足，提示退出
#ifdef JAVA_SUPPORT
    //SCI_TRACE_LOW:"[MMIDC] MMIAPIDC_OpenPhotoWin java running =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_841_112_2_18_2_9_17_387,(uint8*)"d", MMIAPIJAVA_IsJavaRuning());
    if (MMIAPIJAVA_IsJavaRuning())
    {
        MMIPUB_OpenAlertWarningWin( TXT_EXIT_BACKGROUND_JAVA);
        return FALSE;
    }
#endif

    if (MMIAPICC_IsInState(CC_IN_CALL_STATE))
    {
        MMIPUB_OpenAlertWarningWin(TXT_CALL_USING);
        return FALSE;
    }
#ifdef MRAPP_SUPPORT
    if(MMIMRAPP_HandleEventFromOtherModule(MMI_MODULE_CAMERA)) /*lint !e718 !e18*/
        return FALSE;
#endif
#ifdef MCARE_V31_SUPPORT
	if(MCareV31_Exit_Confirm())
	{
		return FALSE;	///具体的return值不同，和添加出的函数相关
	}
#endif
    return TRUE;
}

/*****************************************************************************/
//Description : init running parameter
//Global resource dependence : none
//Author: 
//Note:
/*****************************************************************************/
LOCAL void InitRunningParameter(void)
{
    MMIDC_NewSaveData();
    MMIAPIDC_AllocSettingMemory();
#if !defined(MMIDC_DESKTOP_PDA)
    MMIAPIDC_InitOSDMenuIcon();
#endif    
    MMIDC_Setting_GetNVDefaultValue();
}

/*****************************************************************************/
//Description : free running parameter
//Global resource dependence : none
//Author: 
//Note:
/*****************************************************************************/
LOCAL void FreeRunningParameter(void)
{
#if !defined(MMIDC_DESKTOP_PDA)
    MMIAPIDC_FreeGUI();
#endif    

    MMIDC_CloseAllFileHandle();
    MMIDC_DeleteFileHandleMutexPtr();

#ifdef MMIDC_F_U_CAMERA
    MMIDC_DeleteImgProcMutexPtr();
    MMIDC_DeleteUCameraStatusSemaphorePtr();
#endif
    
    MMIAPIDC_FreeSettingMemory();
    MMIDC_DeleteSaveData();
}

/*****************************************************************************/
// 	Description : open preview window
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIDC_OpenPhotoWin(void)
{
    BOOLEAN  ret = FALSE;
    MMIDCSRV_WORK_PARAM_T work_param = {0};
    
    //SCI_TRACE_LOW:"[MMIDC] MMIAPIDC_OpenPhotoWin"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_897_112_2_18_2_9_17_388,(uint8*)"");

    work_param.work_mode = MMIDCSRV_MODE_DC;
    
    work_param.dc.mem_cfg = MMIDCSRV_MEM_DC_NORMAL;
    work_param.dc.disp_mode = DCAMERA_DISP_MODE_MAX;
    
    work_param.dv.mem_cfg = MMIDCSRV_MEM_DV_NORMAL;
    work_param.dv.max_video_size = WORK_VIDEO_SIZE_MAX;
    work_param.dv.disp_mode = DRECORDER_DISP_MODE_MAX;
    
    ret = MMIAPIDCSRV_OpenMainWin(&work_param);
    
    SCI_TRACE_LOW("[MMIDC] MMIAPIDC_OpenPhotoWin ret=%d",ret);
    
    return ret;
}

#if defined(MMIDC_F_U_CAMERA)
/*****************************************************************************/
//  Description : open preview window
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIDC_OpenUCameraPhotoWin(void)
{
    BOOLEAN  ret = FALSE;
    MMIDCSRV_WORK_PARAM_T work_param = {0};


    //SCI_TRACE_LOW:"[MMIDC] MMIAPIDC_OpenUCameraPhotoWin"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_924_112_2_18_2_9_17_389,(uint8*)"");


    work_param.work_mode = MMIDCSRV_MODE_DC;
    
    work_param.dc.mem_cfg = MMIDCSRV_MEM_DC_UCAMERA;
    work_param.dc.disp_mode = DCAMERA_DISP_MODE_MAX;    

    work_param.dv.mem_cfg = MMIDCSRV_MEM_DV_NORMAL;
    work_param.dv.max_video_size = WORK_VIDEO_SIZE_MAX;
    work_param.dv.disp_mode = DRECORDER_DISP_MODE_MAX;
    
    ret = MMIAPIDCSRV_OpenMainWin(&work_param);
    
    SCI_TRACE_LOW("[MMIDC] MMIAPIDC_OpenUCameraPhotoWin ret=%d",ret);
   
    return ret;
}
#endif

/*****************************************************************************/
// 	Description : open preview window
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIDC_OpenForChipTest(void)
{
    BOOLEAN ret = FALSE;
    
    ret = MMIAPIDC_OpenPhotoWin();

    if (!ret)
    {
        return ret;
    }
  
    s_is_chip_test = TRUE;	
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : open preview window
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIDC_OpenVideoWin(void)
{	
    BOOLEAN  ret = FALSE;
    MMIDCSRV_WORK_PARAM_T work_param = {0};
    

    //SCI_TRACE_LOW:"[MMIDC] MMIAPIDC_OpenVideoWin"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_968_112_2_18_2_9_17_390,(uint8*)"");
    
    work_param.work_mode = MMIDCSRV_MODE_DV;
    
    work_param.dc.mem_cfg = MMIDCSRV_MEM_DC_NORMAL;
    work_param.dc.disp_mode = DCAMERA_DISP_MODE_MAX;
    
    work_param.dv.mem_cfg = MMIDCSRV_MEM_DV_NORMAL;
    work_param.dv.max_video_size = WORK_VIDEO_SIZE_MAX;
    work_param.dv.disp_mode = DRECORDER_DISP_MODE_MAX;
    
    ret = MMIAPIDCSRV_OpenMainWin(&work_param);
    SCI_TRACE_LOW("[MMIDC] MMIAPIDC_OpenVideoWin ret=%d", ret);

    return ret;
}

/*****************************************************************************/
// 	Description : record video
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void Record(void)
{
    if(MMIDC_FlowFunction(DC_FORWARD))
    {
        s_is_dv_pause = FALSE;
        #ifdef WIN32
        s_record_total_time = 0;
        #endif
        StartRecordTimer();
    }
}

/*****************************************************************************/
// 	Description : capture photo
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void Capture(void)
{
    s_is_pressed_save = FALSE;
    s_is_back_to_preview = FALSE;
    s_is_capture_success_msg = FALSE;

    MMIDC_FlowFunction(DC_FORWARD);
    if (MMIAPIPICVIEW_IsPicFolderWinOpen())
    {
        MMIAPIPICVIEW_ReloadFile();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LeftDCPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN  is_desktop_open = FALSE;
#endif    
    
    if(!HandleOSDKey(LEFT_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_LEFT);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        {
            MMIDC_OpenOSDIcons();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LeftSave(void)
{
    if(MMIDC_GetMultiShootEnum() != MULTI_SHOOT_DISABLE)
    {
        MMIDC_MoveCurReviewPhotoID(MOVE_LEFT);
        
        MMIDC_ReviewPhotos();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LeftDVPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if(!HandleOSDKey(LEFT_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_LEFT);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        {
            MMIDC_OpenOSDIcons();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RightDCPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif

    if(!HandleOSDKey(RIGHT_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_RIGHT);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        {
            MMIDC_OpenOSDIcons();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RightSave(void)
{
    if(MMIDC_GetMultiShootEnum() != MULTI_SHOOT_DISABLE)
    {
        MMIDC_MoveCurReviewPhotoID(MOVE_RIGHT);
        
        MMIDC_ReviewPhotos();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RightDVPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if(!HandleOSDKey(RIGHT_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_RIGHT);
        }
        else
#endif        
        {
            MMIDC_OpenOSDIcons();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void UpDCPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if(!HandleOSDKey(UP_KEY))
    {  
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_UP);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        {
            if(MMIDC_IncreaseZoomValue())
            {
            #if !defined(MMIDC_DESKTOP_PDA)  
                MMIDC_DisplayZoomTip(MMIDC_GetPhotoZoomValue() + 1);
            #endif

#ifdef MMIDC_DESKTOP_PDA
                MMIDC_UpdateZoomSeekBar();
#endif
            }     
        }    
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void UpSave(void)
{
}

/*****************************************************************************/
//  Description : up dv zoom
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL void UpDVZoom(void)
{
 #if defined(MMIDC_F_DV_ZOOM)
    BOOLEAN ret = FALSE;
    ret = MMIDC_IncreaseDVZoomValue();

#ifdef MMIDC_DESKTOP_PDA
    if(ret)
    {
        MMIDC_UpdateZoomSeekBar();
    }
#endif
 
#endif
}

/*****************************************************************************/
//  Description : down dv zoom
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL void DownDVZoom(void)
{
 #if defined(MMIDC_F_DV_ZOOM)
    BOOLEAN ret = FALSE;
    ret = MMIDC_DecreaseDVZoomValue();

#ifdef MMIDC_DESKTOP_PDA
    if(ret)
    {
        MMIDC_UpdateZoomSeekBar();
    }
#endif

#endif
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void UpDVPreivew(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if (!HandleOSDKey(UP_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_UP);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)
     
        {
            UpDVZoom();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void UpDVReview(void)
{
    HandleOSDKey(UP_KEY);
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void DownDCPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if(!HandleOSDKey(DOWN_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_DOWN);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        {
            if(MMIDC_DecreaseZoomValue())
            {
            #if !defined(MMIDC_DESKTOP_PDA)  
                MMIDC_DisplayZoomTip(MMIDC_GetPhotoZoomValue() + 1);
            #endif

#ifdef MMIDC_DESKTOP_PDA
                MMIDC_UpdateZoomSeekBar();
#endif
            } 
        }     
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void DownSave(void)
{
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void DownDVPreview(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if (!HandleOSDKey(DOWN_KEY))
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            HandleShortcutKey(MSG_APP_DOWN);
        }
        else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)         
        {
            DownDVZoom();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void DownDVReview(void)
{
    HandleOSDKey(DOWN_KEY);
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void OKDCPreview(void)
{
    if(!HandleOSDKey(OK_KEY))
    {
        MMIDC_CloseAllOSDMenu();

        if(MMIDC_GetSelfShootDelayTime() == SELF_SHOOT_DISABLE)
        {
            Capture();
        }
        else
        {
            MMIDC_PlayCountVoice(1);
            MMIDC_SetCurrentMode(DC_CAPTURE_CONT_DOWN_MODE);
            StartShootDelayTimer(MMIDC_DELAY_TIME);
            
            #if defined(MMIDC_DESKTOP_PDA)
            {
                int32 count_down_seconds = MMIDC_GetCountDownSeconds();
                
                MMIDC_DisplayDelaySecondsTip((uint8)count_down_seconds);
                MMIDC_CompleteDisplayOSD();
            }
            #endif
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LSDCPreview(void)
{
#if !defined(MMIDC_DESKTOP_PDA)
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if(!MMIDC_GetIconHandle()->IconIsOpen()        
       && !MMIDC_GetMenuHandle()->MenuIsOpen()
      )
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            MMIDC_CloseGUIIconDesktop();
        }
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        MMIDC_OpenPhotoOption();
    }
    else
#endif    
    {
        OKDCPreview();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LSSave(void)
{
    if(!HandleOSDKey(OK_KEY))
    {
        MMIDC_OpenPhotoReviewOption();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LSDVPreview(void)
{
#if !defined(MMIDC_DESKTOP_PDA)
#if defined(MMIDC_DESKTOP_ICON_BAR)
    BOOLEAN is_desktop_open = FALSE;
#endif    
    
    if(!MMIDC_GetIconHandle()->IconIsOpen() 
       && !MMIDC_GetMenuHandle()->MenuIsOpen()
       )
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        is_desktop_open = MMIDC_IsIconDesktopOpen();
        if (is_desktop_open)
        {
            MMIDC_CloseGUIIconDesktop();
        }
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)

        MMIDC_OpenVideoOption();
    }
    else if(!HandleOSDKey(OK_KEY))
#endif    
    {
        Record();
    }
}

/*****************************************************************************/
//  Description : process stop record
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL void ProcessStopRecord(DV_END_TYPE_E end_type)
{
    CloseRecordTimer();

    MMIDC_SetTransparentColor(MMI_BLACK_COLOR);

#if !defined(MMIDC_DESKTOP_PDA) || defined(MMIDC_F_PDA_BIT_16)
    if (LCD_ANGLE_0 == MMIDC_GetPlatformAngle())
    {
        MMIDC_SetLcdForRotate(FALSE);
        MMIDC_SetScreenMode(SCREEN_MODE_VERTICAL);
    }
#endif    

#if defined(WIN32)
#if defined(MMIDC_DESKTOP_PDA)
{
    MMI_WIN_ID_T win_id = MMIDC_MAIN_WIN_ID;
    MMI_IMAGE_ID_T image_id = IMAGE_THEME_BG;
    uint32 img_data_size = 0;
    uint16        dest_width = 0;
    uint16        dest_height = 0;
    GUI_POINT_T   src_point = {0};
    GUI_COLOR_T   dest_buf[480*320] = {0};
    
    uint8 *src_ptr = PNULL;

    dest_width = MMIDC_GetPreviewWidth();
    dest_height = MMIDC_GetPreviewHeight();
    
    src_ptr =(uint8 *)MMI_GetLabelImage(image_id,win_id,&img_data_size);
    GUIRES_CopyImgBuf(src_ptr,
        dest_width,
        dest_height,
        dest_width * dest_height * 2,
        img_data_size,
        &src_point,
        PNULL,
        (GUI_COLOR_T *)dest_buf);

    #if !defined(MMIDC_F_PDA_BIT_16)
    MMIDC_GetA888LastFrameData(dest_buf, dest_width, dest_height);
    #endif
        
}
#endif
#endif

 #if defined(MMIDC_DESKTOP_PDA)
    if(MMIDC_GET_FOCUS == MMIDC_GetMMIFocusStatus())
    {
        DC_FLOW_MODE_E mode = 0;

        MMIDC_ClearOSDBlock();
        MMIDC_DisplayLastFrame();
        mode = MMIDC_GetCurrentMode();
        MMIDC_DisplaySoftKey(mode);
#if defined (MMIDC_F_WORK_MODE)
        if(!MMIDC_IsWorkMode())
#endif
        {
            MMIDC_DisplayVideoReview();
        }
        MMIDC_CompleteDisplayOSD();
    }
 #endif

}

/*****************************************************************************/
//  Description : handle key function
//  Global resource dependence : none
//  Author: ryan.xu
//  Note:
/*****************************************************************************/
LOCAL void StopRecord(void)
{
    CloseRecordTimer();

    MMIDC_FlowFunction(DC_FORWARD);
}
/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void LSDVReview(void)
{
    if(!HandleOSDKey(OK_KEY))
    {
#if defined(MMIDC_F_WORK_MODE)
        if(MMIDC_IsWorkMode())
        {
            MMIDC_PostStateMsg();
        }
        else
#endif
        {
            MMIDC_OpenVideoReviewOption();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RSDCPreview(void)
{
    BOOLEAN res = FALSE;
#if defined(MMIDC_DESKTOP_ICON_BAR)    
    MMIDC_DESKTOP_STYLE_E  desktop = 0;
    BOOLEAN is_desktop_open = FALSE;

    desktop = MMIDC_GetDesktopStyle();
    is_desktop_open = MMIDC_IsIconDesktopOpen();

    if ((DESKTOP_STYLE_ICON == desktop)
        && (!is_desktop_open)
       )
    {
        MMIDC_CloseAllOSDMenu();        
        MMIDC_SetIconDesktopVisibility(TRUE);
    }
    else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)
    {
#if !defined(MMIDC_DESKTOP_PDA)
        res = MMIDC_GetMenuHandle()->MenuHandleKeyDown(CANCEL_KEY);
        res = MMIDC_GetIconHandle()->IconHandleKeyDown(CANCEL_KEY) || res;
        res = MMIDC_HandleAdjustKey(CANCEL_KEY) || res;
#endif    
        if(!res) /*lint !e774*/
        {
            MMIDC_PostStateMsg();
        }
        else
        {
           MMIDC_FlowStart();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RSCountDown(void)
{
#if defined(MMIDC_DESKTOP_ICON_BAR)
    MMIDC_DESKTOP_STYLE_E  desktop = 0;
    BOOLEAN is_desktop_open = FALSE;

    desktop = MMIDC_GetDesktopStyle();
    is_desktop_open = MMIDC_IsIconDesktopOpen();

    if ((DESKTOP_STYLE_ICON == desktop)
        && (!is_desktop_open)
       )
    {
        MMIDC_SetIconDesktopVisibility(TRUE);
    }
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)

    MMIAPISET_StopAllRing(FALSE);
    CloseShootDelayTimer();
    s_timer_count_down = 0;
    MMIDC_SetCurrentMode(DC_PREVIEW_MODE);
    MMIDC_ClearOSDBlock();

#if defined(MMIDC_DESKTOP_PDA)
    MMIDC_ShowAllOSD();
#endif
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RSDCReview(void)
{
#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        MMIDC_PostStateMsg();
    }
    else
#endif        
    {
        MMIDC_DeleteAllSavedImageFile();

        MMIDC_ClearOSDBlock();
        MMIDC_FlowFunction(DC_BACKWARD);

        MMIDC_PostCreateFileHandleMsg();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RSSave(void)
{
    if(HandleOSDKey(CANCEL_KEY))
    {
        if(MMIDC_GetMultiShootEnum() != MULTI_SHOOT_DISABLE && !MMIDC_IsReviewFullScreen())
        {
            MMIDC_DisplayCurrentFileName();
        }
    }
    else
    {
        if(MMIDC_IsReviewFullScreen())
        {
            MMIDC_FlowFunction(DC_BACKWARD);
        }
        else
        {
            MMIDC_ClearOSDBlock();
            MMIDC_FlowFunction(DC_BACKWARD);
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RSDVPreview(void)
{
    BOOLEAN res = FALSE;
    
#if defined(MMIDC_DESKTOP_ICON_BAR)    
    MMIDC_DESKTOP_STYLE_E  desktop = 0;
    BOOLEAN is_desktop_open = FALSE;

    desktop == MMIDC_GetDesktopStyle();
    is_desktop_open = MMIDC_IsIconDesktopOpen();
    
    if ((DESKTOP_STYLE_ICON == desktop)
        && (!is_desktop_open)
        ) /*lint !e774*/
    {
        MMIDC_CloseAllOSDMenu();
        MMIDC_OpenIconDesktop();
    }

    else
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)    

    {
#if !defined(MMIDC_DESKTOP_PDA)
        res = MMIDC_GetMenuHandle()->MenuHandleKeyDown(CANCEL_KEY);
        res = MMIDC_GetIconHandle()->IconHandleKeyDown(CANCEL_KEY) || res;
        res = MMIDC_HandleAdjustKey(CANCEL_KEY) || res;
#endif        
        if(!res) /*lint !e774*/
        {
            MMIDC_PostStateMsg();
        }
        else
        {
            MMIDC_FlowStart();
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void CancelRecord(void)
{
    CloseRecordTimer();
    MMIDC_FlowFunction(DC_BACKWARD);
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void RSDVReview(void)
{
    if(HandleOSDKey(CANCEL_KEY))
    {
       ;
    }
    else
    {
#if defined(MMIDC_F_WORK_MODE)
        if (MMIDC_IsWorkMode())
        {
            MMIDC_DeleteNoSavedFile();
            MMIDC_SetWorkError(MMIDC_WORK_ERROR_CANCEL_BY_USER);
            MMIDC_PostStateMsg();
        }
        else
#endif
        {
            MMIDC_SetTransparentColor(MMIDC_TRANSPARENT_COLOR);
            MMIDC_ClearMainBlock(MMI_BLACK_COLOR);
            if(MMIDC_GetDefaultScreenMode() == SCREEN_MODE_HORIIZONTAL)
            {
                if(SCREEN_MODE_VERTICAL == MMIDC_GetScreenMode())
                {
                    s_need_full_paint = TRUE;
                    MMIDC_SetScreenMode(SCREEN_MODE_HORIIZONTAL);
                }
            }       
            MMIDC_FlowFunction(DC_BACKWARD);
        }
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void OKDVPreview(void)
{
    if(!HandleOSDKey(OK_KEY))
    {
        MMIDC_CloseAllOSDMenu();
        
        Record();
    }
}

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void OKDVReview(void)
{
    if(!HandleOSDKey(OK_KEY))
    {
#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        MMIDC_DeleteNoSavedFile();
        MMIDC_SetWorkError(MMIDC_WORK_ERROR);
        MMIDC_SetTransparentColor(MMIDC_TRANSPARENT_COLOR);
        MMIDC_ClearMainBlock(MMI_BLACK_COLOR);
        if(MMIDC_GetDefaultScreenMode() == SCREEN_MODE_HORIIZONTAL)
        {
            if(SCREEN_MODE_VERTICAL == MMIDC_GetScreenMode())
            {
                s_need_full_paint = TRUE;
                MMIDC_SetScreenMode(SCREEN_MODE_HORIIZONTAL);
            }
        }       
        MMIDC_FlowFunction(DC_BACKWARD);
        return;
    }
#endif
#ifdef VIDEO_PLAYER_SUPPORT
        MMIDC_ReviewVideo();
#endif
    }
}

/*****************************************************************************/
// 	Description : display waitting photo
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void DisplayWaitPhoto(void)
{
    GUI_LCD_DEV_INFO dev_info = MMIDC_GetOSDBlockInfo();
    GUI_POINT_T      dis_point = {0};
    
    int16  screen_width = MMIDC_GetPreviewWidth();
    int16  screen_height = MMIDC_GetPreviewHeight();
    uint16 img_width = 0;
    uint16 img_height = 0;
    
    //get resources width or height
    GUIRES_GetImgWidthHeight(&img_width, &img_height, IMG_DC_WAIT, MMIDC_MAIN_WIN_ID);

    dis_point.x = (screen_width - img_width) / 2;
    dis_point.y = (screen_height - img_height) / 2;
    
    dev_info.lcd_id = MMIDC_GetLcdID();
    GUIRES_DisplayImg(&dis_point, PNULL, PNULL, MMIDC_MAIN_WIN_ID, IMG_DC_WAIT, &dev_info);    
    
    MMIDC_CompleteDisplayOSD();
}
/*****************************************************************************/
// 	Description : save the captured photo
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void Save(void)
{
#if defined(MMIDC_F_U_CAMERA)
    if (MMIDC_IsUCameraEnable() && !MMIDC_IsPhotoData())
    {
        RSSave();
    }
    else
#endif

    {
        s_is_pressed_save = TRUE;

        if (MMIDC_CanPressSave())
        {
            DisplayWaitPhoto();
        }
        
        if(s_is_capture_success_msg)
        {
            s_is_capture_success_msg = FALSE;
            MMIDC_FlowFunction(DC_FORWARD);        
#if defined(MMIDC_F_WORK_MODE)
            if (MMIDC_IsWorkMode())
            {
                MMIDC_PostStateMsg();
                return;
            }
#endif    
        }
    }
}

/*****************************************************************************/
// 	Description : back to preview 
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void BackToPreview(void)
{
    s_is_back_to_preview = TRUE;

    DisplayWaitPhoto();
}
/*****************************************************************************/
// 	Description : dv pause or resume
//	Global resource dependence : none
//  Author: robert.wang 
//	Note:
/*****************************************************************************/
LOCAL void PauseOrResume(void)
{
    //SCI_TRACE_LOW:"[MMIDC] PauseOrResume s_is_dv_pause = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_1886_112_2_18_2_9_19_391,(uint8*)"d", s_is_dv_pause);
    
    if (s_is_dv_pause)
    {
        s_is_dv_pause = FALSE;
        StartRecordTimer();
    }
    else
    {
        s_is_dv_pause = TRUE;
        CloseRecordTimer();
    }
    
    MMIDC_FlowFunction(DV_PAUSERESUME);    
}


/*****************************************************************************/
//Description : DC review ok key
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void OKDCReview(void)
{
#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        MMIDC_DeleteAllSavedImageFile();

        MMIDC_ClearOSDBlock();
        MMIDC_FlowFunction(DC_BACKWARD);
    }
    else
#endif
#if 0 //defined(MMIDC_F_EFFECT_360)
//    MMIDC_OpenEffect360Win();
#else
    {
      Save();
    }
#endif  
}


const DCKEYFUNCTION s_key_function[DC_KEY_MAX][DC_FLOW_MODE_MAX] = 
{
    /*DC_PREVIEW     CONT_DOWN     DC_CAPTURE  DC_REVIEW    DC_SAVE                 DV_PREVIEW          DV_RECORD        DV_PAUSE         DV_REVIEW */
    {LeftDCPreview,  PNULL,        PNULL,      PNULL,       LeftSave,               LeftDVPreview,      PNULL,           PNULL,           PNULL},
    {RightDCPreview, PNULL,        PNULL,      PNULL,       RightSave,              RightDVPreview,     PNULL,           PNULL,           PNULL},
    {UpDCPreview,    PNULL,        PNULL,      PNULL,       UpSave,                 UpDVPreivew,        UpDVZoom,        UpDVZoom,        UpDVReview},
    {DownDCPreview,  PNULL,        PNULL,      PNULL,       DownSave,               DownDVPreview,      DownDVZoom,      DownDVZoom,      DownDVReview},
    {LSDCPreview,    PNULL,        PNULL,      Save,        LSSave,                 LSDVPreview,        StopRecord,      StopRecord,      LSDVReview},
    {RSDCPreview,    RSCountDown,  PNULL,      RSDCReview,  RSSave,                 RSDVPreview,        CancelRecord,    CancelRecord,    RSDVReview},
    {OKDCPreview,    PNULL,        PNULL,      OKDCReview,        MMIDC_OpenSendPhotoWin, OKDVPreview,        PauseOrResume,   PauseOrResume,   OKDVReview},
};

/*****************************************************************************/
// 	Description : handle key function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void KeyFunction(DC_KEY_E key)
{    
    DC_FLOW_MODE_E  mode = MMIDC_GetCurrentMode();
    
    if(key >= DC_KEY_MAX)
    {    
        //MMIDC_ASSERT_WARNING:"[MMIDC]KeyFunction key >= DC_KEY_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_1951_112_2_18_2_9_19_392,(uint8*)"");
        return;
    }
    
    if(PNULL != s_key_function[key][MMIDC_GetCurrentMode()])
    {
        //SCI_TRACE_LOW:"[MMIDC] KeyFunction key=%d,cur_mode=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_1957_112_2_18_2_9_19_393,(uint8*)"dd", key, mode);
        MMIDC_CleanAllOSDRect();

        s_key_function[key][mode]();
        
        MMIDC_ShowAllOSD();
    }
}

/*****************************************************************************/
// 	Description : delete window handle function
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIDC_HandleDeletePubWinBG(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
#if !defined(MMIDC_DESKTOP_PDA)  
    GUI_POINT_T         dis_point = {0};
#endif
    //SCI_TRACE_LOW:"[MMIDC]: MMIDC_HandleDeletePubWinBG"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_1975_112_2_18_2_9_19_394,(uint8*)"");
    switch (msg_id)
    {
    case MSG_FULL_PAINT:
#if !defined(MMIDC_DESKTOP_PDA)     
        GUIRES_DisplayImg(&dis_point,
            PNULL,
            PNULL,
            win_id,
            IMAGE_COMMON_BG,
            MMITHEME_GetDefaultLcdDev());
#endif
        break;
    default:
        break;
    }
    return MMIPUB_HandleQueryWinMsg(win_id, msg_id, param);
}

/*****************************************************************************/
//Description :  handle tp for multi photo
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleTPForMultiPhoto(GUI_POINT_T point)
{
    BOOLEAN ret = FALSE;
    GUI_RECT_T file_name_left_arrow = {0};
    GUI_RECT_T file_name_right_arrow ={0};
    GUI_RECT_T multi_photo_rect ={0};


    if ((DC_SAVE_MODE == MMIDC_GetCurrentMode())
        || (DC_REVIEW_MODE == MMIDC_GetCurrentMode()) 
        )
    {
         ret = TRUE;
        if(1 < MMIDC_GetCapturedPhotosNumber())
        {
            multi_photo_rect = MMIDC_GetMultiPhotoRect();
            file_name_left_arrow.left = multi_photo_rect.left;
            file_name_left_arrow.right = file_name_left_arrow.left + MMIDC_ITEM_TEXT_HEIGHT - 1;
            file_name_left_arrow.top = multi_photo_rect.top;
            file_name_left_arrow.bottom = file_name_left_arrow.top + MMIDC_ITEM_TEXT_HEIGHT - 1;

            file_name_right_arrow.right= multi_photo_rect.right;
            file_name_right_arrow.left = file_name_right_arrow.right - MMIDC_ITEM_TEXT_HEIGHT + 1;
            file_name_right_arrow.top = multi_photo_rect.top;
            file_name_right_arrow.bottom = file_name_right_arrow.top + MMIDC_ITEM_TEXT_HEIGHT - 1;

            if(GUI_PointIsInRect(point, file_name_left_arrow))
            {
                KeyFunction(DC_LEFT_KEY);
            }
            else if(GUI_PointIsInRect(point, file_name_right_arrow))
            {
               KeyFunction(DC_RIGHT_KEY);
            }
        }
    }


    //SCI_TRACE_LOW:"[MMIDC] HandleTPForMultiPhoto ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2036_112_2_18_2_9_19_395,(uint8*)"d", ret);
    return ret;
}

/*****************************************************************************/
//Description :  handle tp for softkey
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleTPForSoftkey(GUI_POINT_T point)
{
    BOOLEAN ret = TRUE;
    GUI_RECT_T box_right = {0};
    GUI_RECT_T box_left = {0};
    GUI_RECT_T box_middle = {0};

  
    box_left = MMIDC_GetLeftSoftkey();
    box_middle = MMIDC_GetMiddleSoftkey();
    box_right = MMIDC_GetRightSoftkey();

    if(GUI_PointIsInRect(point, box_right))
    {
        //SCI_TRACE_LOW:"[MMIDC]: GUI_PointIsInRect rs_rect"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2058_112_2_18_2_9_19_396,(uint8*)"");
        KeyFunction(DC_RIGHT_SOFT_KEY);
        MMIDC_StopTextScrollTimer();
    }
    else if(GUI_PointIsInRect(point, box_middle) && !MMIDC_IsMiddleSoftKeyNULL())
    {
        //SCI_TRACE_LOW:"[MMIDC]: GUI_PointIsInRect mid_rect"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2064_112_2_18_2_9_19_397,(uint8*)"");
        KeyFunction(DC_OK_KEY);
    }
    else if(GUI_PointIsInRect(point, box_left))
    {
        //SCI_TRACE_LOW:"[MMIDC]: GUI_PointIsInRect ls_rect"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2069_112_2_18_2_9_19_398,(uint8*)"");
        KeyFunction(DC_LEFT_SOFT_KEY);
    }
    else
    {
        ret = FALSE;
    }

    //SCI_TRACE_LOW:"[MMIDC] HandleTPForSoftkey ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2077_112_2_18_2_9_19_399,(uint8*)"d", ret);
    return ret;
}
/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void HandleDCWinTpPressDown(GUI_POINT_T	point)
{
#if !defined(MMIDC_DESKTOP_PDA)
    BOOLEAN res = FALSE;
    MMIDC_DESKTOP_STYLE_E desktop = 0;
    
    //SCI_TRACE_LOW:"[MMIDC]: HandleDCWinTpPressDown %d, %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2090_112_2_18_2_9_19_400,(uint8*)"dd", point.x, point.y);

   //here is sequence to handle for tp 
   do
   {
        //first, softkey
        res = HandleTPForSoftkey(point);
        if (res)
        {
            break;
        }
        
        //second, multi photo
        res = HandleTPForMultiPhoto(point);
        if (res)
        {
            break;
        }

#if defined(MMIDC_DESKTOP_ICON_BAR)        
        //third, icon bar
        res = HandleTPForIconBar(point);
        if (res)
        {
            break;
        }
		
#if defined(MMIDC_F_DV_ZOOM)        
        res = HandleTPForRecordingZoomButton(point);
        if(res)
        {
            break;
        }
#endif

        if (!MMIDC_IsIconDesktopOpen())
#endif        
        {
            //other tp
            MMIDC_CleanAllOSDRect();
            
            res = MMIDC_GetIconHandle()->IconHandleTpDown(point.x, point.y);
            res = MMIDC_GetMenuHandle()->MenuHandleTpDown(point.x, point.y) || res;
            res = MMIDC_HandleAdjustTP(point.x, point.y) || res;
            
            MMIDC_SetShowState(MMIDC_SHOW_ALLMENU);

            desktop = MMIDC_GetDesktopStyle();
            if(!res 
               && (DESKTOP_STYLE_ICON != desktop)
               )
            {
                //when touch blank region,
                //open or close OSD menu
                if(MMIDC_GetCurrentMode() == DC_PREVIEW_MODE || MMIDC_GetCurrentMode() == DV_PREVIEW_MODE)
                {
                    if(!MMIDC_GetIconHandle()->IconIsOpen() &&
                        !MMIDC_GetMenuHandle()->MenuIsOpen())
                    {
                        KeyFunction(DC_LEFT_KEY);
                    }
                    else if(MMIDC_GetIconHandle()->IconIsOpen())
                    {
                        KeyFunction(DC_RIGHT_SOFT_KEY);
                    }
                }
            }
            MMIDC_ShowAllOSD();
        }
    }while(FALSE);
#endif    
}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_HandleLoseFocus(void)
{
    MMIDC_CloseAllOSDMenu();
    
    MMIDC_ResetScrollNumber();

    CloseDCAllTimers();

    #if defined(MMIDC_DESKTOP_PDA)
    MMIDC_PDAForLoseFocus();
    #endif

    MMIDC_FlowFunction(DC_INTERRUPT);

    if(MMIDC_GetCameraMode() == CAMERA_MODE_DC)
    {
        MMIDC_SetCurrentMode(DC_PREVIEW_MODE);
    }
    else if(MMIDC_GetCameraMode() == CAMERA_MODE_DV 
           && MMIDC_GetCurrentMode() != DV_REVIEW_MODE
           )
    {
        MMIDC_SetCurrentMode(DV_PREVIEW_MODE);
    }

#if !defined(MMI_RES_ORIENT_BOTH)
    switch(MMIDC_GetPlatformAngle())
    {
    case LCD_ANGLE_0:
        MMIDC_SetLcdForRotate(FALSE);

        if (SCREEN_MODE_HORIIZONTAL == MMIDC_GetDefaultScreenMode())
        {
            MMIDC_SetScreenMode(SCREEN_MODE_VERTICAL);
        }
        break;
    case LCD_ANGLE_90:
        MMIDC_SetLcdForRotate(TRUE);
        break;
    default:
        break;
    }
#endif

}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_HandleGetFocus(void)
{
    DC_FLOW_MODE_E mode = DC_PREVIEW_MODE;
#if defined(MMIDC_DESKTOP_ICON_BAR)    
    MMIDC_DESKTOP_STYLE_E desktop = 0;
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)    
    
    MMIDC_SetTransparentColor(MMIDC_TRANSPARENT_COLOR);
    MMIDC_ClearMainBlock(MMI_BLACK_COLOR);
    
    MMIDC_ClearOSDBlock();


    {
#if defined(MMIDC_DESKTOP_ICON_BAR)    
        desktop = MMIDC_GetDesktopStyle();
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)        
        
        mode = MMIDC_GetCurrentMode();
        //SCI_TRACE_LOW:"[MMIDC]: MMIDC_HandleGetFocus mode = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2236_112_2_18_2_9_20_401,(uint8*)"d", mode);
        switch(mode)
        {
        case DC_PREVIEW_MODE:
            if(MMIDC_GetDefaultScreenMode() == SCREEN_MODE_HORIIZONTAL 
                && MMIDC_GetScreenMode() == SCREEN_MODE_VERTICAL 
                && MMIDC_GetPhotoSize() != MMIDC_GePhotoSizetVerLcd()
              )
            {
                MMIDC_SetScreenMode(SCREEN_MODE_HORIIZONTAL);
                MMIDC_SetLcdForRotate(TRUE);
            }
#if defined(MMIDC_DESKTOP_ICON_BAR)
            if (DESKTOP_STYLE_ICON == desktop)
            {
                MMIDC_OpenIconDesktop();
            }
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)
            MMIDC_FlowStart();
            break;
        case DV_PREVIEW_MODE:
            if(MMIDC_GetDefaultScreenMode() == SCREEN_MODE_HORIIZONTAL 
                && MMIDC_GetScreenMode() == SCREEN_MODE_VERTICAL
              )
            {
                MMIDC_SetScreenMode(SCREEN_MODE_HORIIZONTAL);
                MMIDC_SetLcdForRotate(TRUE);
            }
#if defined(MMIDC_DESKTOP_ICON_BAR)            
            if (DESKTOP_STYLE_ICON == desktop)
            {
                MMIDC_OpenIconDesktop();
            }
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)            
            MMIDC_FlowStart();
            break;
        case DV_REVIEW_MODE:
#if !defined(MMIDC_DESKTOP_PDA)
            if (LCD_ANGLE_0 == MMIDC_GetPlatformAngle())
            {
                MMIDC_SetLcdForRotate(FALSE);
                MMIDC_SetScreenMode(SCREEN_MODE_VERTICAL);
            }
#endif            
            if(MIN_FILE_SIZE  >= MMIDC_GetFileSize(MMIDC_GetVideoFullFileName()))
            {
                //reset preview screen mode
                if(MMIDC_GetDefaultScreenMode() == SCREEN_MODE_HORIIZONTAL 
                    && MMIDC_GetScreenMode() == SCREEN_MODE_VERTICAL
                  )
                {
					s_need_full_paint = TRUE;
                    MMIDC_SetScreenMode(SCREEN_MODE_HORIIZONTAL);
                    MMIDC_SetLcdForRotate(TRUE);
                }
                
                MMIDC_FlowFunction(DC_BACKWARD);
            }
            else
            {            
                MMIDC_FlowFunction(DC_CONTINUE);
                MMIDC_SetTransparentColor(MMI_BLACK_COLOR);
            }
            break;
        default:        
            MMIDC_FlowFunction(DC_CONTINUE);
            break;
        }
    }


    {
        GUI_RECT_T lcd_rect = {0};
        GUI_LCD_DEV_INFO dev_info = MMIDC_GetOSDBlockInfo();
        
        lcd_rect = MMIDC_GetFullScreenRect(dev_info.lcd_id);
        MMITHEME_StoreUpdateRect(&dev_info, lcd_rect);   
    }      

    
    MMIDC_ShowAllOSD();
}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void HandleWinTimer(uint8 timer_id)
{
    if(s_3seconds_timer == timer_id)
    {
        //SCI_TRACE_LOW:"[MMIDC]: MSG_TIMER s_3seconds_timer"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2315_112_2_18_2_9_20_402,(uint8*)"");
#if !defined(MMIDC_DESKTOP_PDA)        
        if(MMIDC_GetCurrentMode() == DC_PREVIEW_MODE 
          )
        {
            MMIDC_DisplayZoomTip(INVALID_DATA);
            CloseTipDisplayTimer();
            MMIDC_CompleteDisplayOSD();
        }
#endif        
    }
    else if(s_timer_delay == timer_id)
    {
        static uint8 short_voice = 0;

        //SCI_TRACE_LOW:"[MMIDC]: MSG_TIMER s_timer_delay"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2330_112_2_18_2_9_20_403,(uint8*)"");
        if(1 == s_timer_count_down && 3 == short_voice)
        {
            MMIAPISET_StopAllRing(FALSE);
            MMIDC_SetCurrentMode(DC_PREVIEW_MODE);
            Capture();
            CloseShootDelayTimer();
            s_timer_count_down = 0;
        }
        else
        {
            if(s_timer_count_down > 2)
            {
                short_voice = 0;
                s_timer_count_down --;
                MMIDC_DisplayDelaySecondsTip(s_timer_count_down);
                CloseShootDelayTimer();
                
                if(s_timer_count_down > 2)
                {
                    StartShootDelayTimer(MMIDC_DELAY_TIME);
                }
                else
                {
                    StartShootDelayTimer(500);
                }
                
                if(MMIDC_GetCurrentMode() == DC_CAPTURE_CONT_DOWN_MODE)
                {
                    MMIDC_PlayCountVoice(1);
                }
            }
            else
            {
                short_voice ++;
                if(2 == short_voice)
                {
                    s_timer_count_down = 1;
                    MMIDC_DisplayDelaySecondsTip(s_timer_count_down);
                }
                CloseShootDelayTimer();
                StartShootDelayTimer(500);
                if(MMIDC_GetCurrentMode() == DC_CAPTURE_CONT_DOWN_MODE)
                {
                    MMIDC_PlayCountVoice(1);
                }
            }
            
            MMIDC_CompleteDisplayOSD();
        }
    }
    else if(s_updown_tip_timer == timer_id)
    {
        //SCI_TRACE_LOW:"[MMIDC]: MSG_TIMER s_updown_tip_timer"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2383_112_2_18_2_9_20_404,(uint8*)"");
#if !defined(MMIDC_DESKTOP_PDA)        
        if(!MMIDC_GetIconHandle()->IconIsOpen())
        {
            if((MMIDC_GetCurrentMode() == DC_PREVIEW_MODE 
                || MMIDC_GetCurrentMode() == DV_PREVIEW_MODE)
              )
            {
                MMIDC_DisplaySettingTip();
            }
        }
        CloseUpDownTipTimer();
#endif        
    }
    else if (s_switch_osd_key_timer_id == timer_id)
    {
#if defined(MMIDC_DESKTOP_ICON_BAR)
        BOOLEAN is_shortcut_key = FALSE;
        MMI_MESSAGE_ID_E msg_id = 0;
#endif         
        
        //SCI_TRACE_LOW:"[MMIDC]: MSG_TIMER s_switch_osd_key_timer_id last_key=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2404_112_2_18_2_9_20_405,(uint8*)"d", s_last_key_down);
        StopSwitchOSDMenuItemsKeyTimer();
        StartSwitchOSDMenuItemsKeyTimer(s_last_key_down);

#if defined(MMIDC_DESKTOP_ICON_BAR)        
        if (MMIDC_IsIconDesktopOpen())
        {
            is_shortcut_key = TRUE;
            
            switch(s_last_key_down)
            {
            case LEFT_KEY:
                msg_id = MSG_APP_LEFT;
                break;
            case RIGHT_KEY:
                msg_id = MSG_APP_RIGHT;
                break;
            case UP_KEY:
                msg_id = MSG_APP_UP;
                break;
            case DOWN_KEY:
                msg_id = MSG_APP_DOWN;
                break;      
            default:
                is_shortcut_key = FALSE;
                break;
            }
            
            if (is_shortcut_key)
            {
                HandleShortcutKey(msg_id);
            }
        }
        else
#endif        
        {
            if(LEFT_KEY == s_last_key_down)
            {
                if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
                {
                    KeyFunction(DC_LEFT_KEY);
                }
            }
            else if(RIGHT_KEY == s_last_key_down)
            {
                if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
                {
                    KeyFunction(DC_RIGHT_KEY);
                }
            }
            else if(UP_KEY == s_last_key_down)
            {
                if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
                {
                    KeyFunction(DC_UP_KEY);
                }
            }
            else if(DOWN_KEY == s_last_key_down)
            {
                if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
                {
                    KeyFunction(DC_DOWN_KEY);
                }
            }
         }   
    }
    else if(s_record_timer == timer_id)
    {
        //SCI_TRACE_LOW:"[MMIDC]: MSG_TIMER s_record_timer"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2472_112_2_18_2_9_20_406,(uint8*)"");
        #ifdef WIN32
        s_record_total_time ++;
        #endif
        MMIDC_DisplayVideoRecordTip(DV_RECORD_MODE);
#if defined(MMIDC_DESKTOP_ICON_BAR) && defined(MMIDC_F_DV_ZOOM)
    MMIDC_DisplayZoomButton();
#endif
        MMIDC_CompleteDisplayOSD();    
    }
    else if(s_text_scroll_timer == timer_id)
    {
        GUI_RECT_T temp = {0};
        
        //SCI_TRACE_LOW:"[MMIDC]: MSG_TIMER s_text_scroll_timer"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2486_112_2_18_2_9_20_407,(uint8*)"");
        MMIDC_StopTextScrollTimer();
        MMIDC_DisplayString(temp, PNULL, 0,FALSE);
        MMIDC_CompleteDisplayOSD();
    }
    else if(MMIDC_GetCallbackTimer() == timer_id)
    {
        MMIDC_PostStateMsg();
        MMIDC_ErrorTipForExit(TXT_USSD_REQUEST_UNCOMPLETED);
    }
    else if(s_is_chip_test_timer == timer_id)
    {
        ProcessChipTestStep();
    }
    else if(s_hit_timer == timer_id)
    {
        CloseHitTimer();
        
        //SCI_TRACE_LOW:"[MMIDC] s_hit_timer exec"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2504_112_2_18_2_9_20_408,(uint8*)"");
        
        MMIDC_PostStateMsg();
    }
#ifdef MMIDC_DESKTOP_PDA
    else
    {
        MMIDC_DesktopProcessMsg(MSG_TIMER, (void*)&timer_id);
    }
#endif
}

/*****************************************************************************/
//Description : init working environment 
//Global resource dependence : none
//Author: ryan.xu
//Note:
/*****************************************************************************/
LOCAL int32 InitWorkEnvironment(void)
{
    MMIAPISET_StopAllRing(FALSE);
    MMIDEFAULT_TurnOnBackLight(); 
    MMIDEFAULT_AllowTurnOffBackLight(FALSE);	

    //MMI_Enable3DMMI(FALSE);

    MMIDC_SetTransparentColor(MMIDC_TRANSPARENT_COLOR);

    MMIDC_ClearMainBlock(MMI_BLACK_COLOR);

    MMIDC_InitSettingParamCtrl();

    UILAYER_SetMainLayerMemFormat(UILAYER_MEM_NONE);
    
    return 0;
}

/*****************************************************************************/
//Description : init working environment 
//Global resource dependence : none
//Author: ryan.xu
//Note:
/*****************************************************************************/
LOCAL int32 RestoreWorkEnvironment(void)
{
//    MMIAPISET_StopAllRing(TRUE);  //masked for cr250848;
    MMIDEFAULT_AllowTurnOffBackLight(TRUE);

    //MMI_Enable3DMMI(TRUE);

#ifdef SUBLCD_SIZE
    MMISUB_IsPermitUpdate(TRUE);
#endif


    MMIDC_ClearMainBlock(MMI_BLACK_COLOR);
    //NO brush lcd
    MMITHEME_InitMainLcdStortUpdateRect(); 

    UILAYER_SetMainLayerMemFormat(UILAYER_MEM_DOUBLE_COPY);

    return 0;
}

/*****************************************************************************/
// 	Description : init working environment for lose focus
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL int32 RestoreWorkEnvironmentForLoseFocus(void)
{
    MMIDEFAULT_AllowTurnOffBackLight(TRUE);

    //MMI_Enable3DMMI(TRUE);

#ifdef SUBLCD_SIZE
    MMISUB_IsPermitUpdate(TRUE);
#endif

   MMIDC_DestroyMainOSDLayer();
    MMIDC_ClearMainBlock(MMI_BLACK_COLOR);
    //NO brush lcd
    MMITHEME_InitMainLcdStortUpdateRect();

    MMIDC_DeleteAllSavedImageFile();

    UILAYER_SetMainLayerMemFormat(UILAYER_MEM_DOUBLE_COPY);
    
    return 0;
}

/*****************************************************************************/
// 	Description : init working parameter 
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL int32 InitWorkParameter(void)
{
    InitWorkParameterForGetFocus();   

    MMIDC_InitFlowParameter();

    s_is_pressed_save = FALSE;
    s_is_capture_success_msg = FALSE;
    s_is_back_to_preview = FALSE;
    s_need_full_paint = FALSE;

    MMIDC_SetMMIBrushStatus(MMIDC_BRUSH_START);
    MMIDC_SetMMIFocusStatus(MMIDC_GET_FOCUS);

    MMIDC_CreateFileHandleMutexPtr();    

#ifdef MMIDC_F_U_CAMERA
	MMIDC_CreateImgProcMutexPtr();
    MMIDC_CreateUCameraStatusSemaphorePtr();
#endif

    return 0;
}

/*****************************************************************************/
// 	Description : init working parameter for get focus
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL int32 InitWorkParameterForGetFocus(void)
{
    s_timer_count_down=0;
    MMIDC_ResetScrollNumber(); 
    MMIDC_InitAdjustData();

    return 0;
}

/*****************************************************************************/
//Description : release dc module
//Global resource dependence : none
//Author: ryan.xu
//Note:
/*****************************************************************************/
LOCAL void ReleaseDCModule(void)
{
    MMIDC_SetDCModuleRunningFlag(FALSE);

    //step 1
    CloseDCAllTimers();

#ifdef MMIDC_F_U_CAMERA
    MMIDC_CloseUCamera();
    MMIDC_SetImageProcessType(IMG_PROC_NORMAL);
#endif

    //step 2
    MMIDC_FlowFunction(DC_EXIT);
    
    MMIDC_ResetScrollNumber();

    //step 3


    //step 4
    MMIDC_SaveSettings();

    //step 5
#if defined(MMIDC_F_WORK_MODE)
    MMIDC_ReleaseWorkMode();
#endif

    //删除临时文件
    MMIDC_DeleteAllSavedImageFile();

    MMIDC_FreeMemoryForReview();
    
    FreeRunningParameter();

    MMIAPIDC_SetChangeFolderState(FALSE);

    MMIDC_FreeLastFrame();
#ifdef MMI_CAMERA_F_PHOTO_DATE
    MMIDC_ReleasePhotoDateData();
#endif
#ifdef MMIDC_DESKTOP_PDA
    MMIDC_DestroyDesktopPDA();
#endif
    MMIDC_DestroyMainOSDLayer();
    RestoreWorkEnvironment();        

    MMIAPIALM_CheckAndStartDelayedAlarm();
  
    if(s_is_chip_test)
    {
        CloseChipTestTimer();
        s_is_chip_test = FALSE;
    }

    {
        MMISRV_HANDLE_T    audio_handle = MMIDC_GetAudioHandle();
        if(audio_handle > 0)
        {
            MMISRVAUD_Stop(audio_handle);
            MMISRVMGR_Free(audio_handle);
            MMIDC_SetAudioHandle(0);
        }
    }
}

/*****************************************************************************/
//Description : start dc flowing
//Global resource dependence : none
//Author: robert.wang
//Note: 
/*****************************************************************************/
LOCAL BOOLEAN StartDCActivity(CAMERA_MODE_E mode)
{
    BOOLEAN  ret = FALSE;
    MMISRV_HANDLE_T audio_handle = MMIDC_GetAudioHandle();

#if defined(MMIDC_DESKTOP_PDA) && !defined(MMIDC_F_PREVIEW_ANIM)    
    MMI_WIN_ID_T wait_win_id = MMIDC_MAIN_WAIT_WIN_ID;
    MMI_WIN_ID_T win_id = MMIDC_MAIN_WIN_ID;
    SCREEN_MODE_E screen_mode = 0;
    LCD_ANGLE_E angle = 0;
#endif

    MMIDC_SetDCModuleRunningFlag(TRUE);
    
    InitRunningParameter();
    
    InitWorkEnvironment();
    InitWorkParameter();

    MMIDC_SetCameraMode(mode);
    //MMIDC_GetPreviewImgData(mode);

    //yaye.jiang 2011.11.26 add for audio 
    if(audio_handle > 0)
    {
        MMISRVAUD_Stop(audio_handle);
        MMISRVMGR_Free(audio_handle);
        MMIDC_SetAudioHandle(0);
    }

    audio_handle = MMIDC_RequestHandle();
    if(audio_handle > 0)
    {
        MMIDC_SetAudioHandle(audio_handle);
    }

    //SCI_TRACE_LOW:"[MMIDC] StartDCActivity audio_handle=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2720_112_2_18_2_9_20_409,(uint8*)"d",audio_handle);
	
    if (0 == audio_handle)
    {
        CloseDCApplet();
	 MMIPUB_OpenAlertWarningWin(TXT_SMS_BUSY);
	 return FALSE;
    }
    
#ifdef MMIDC_DESKTOP_PDA    
    MMIDC_StartPreviewAnim(PREVIEW_ANIM_STEP_IMG);
#if !defined(MMIDC_F_PREVIEW_ANIM)
    screen_mode = MMIDC_GetScreenMode();

    win_id = MMK_GetFocusWinId();   

    if (wait_win_id == win_id || MMISET_VALIDATE_PRIVACY_PWD_WIN_ID == win_id)
    {
        if(SCREEN_MODE_VERTICAL == screen_mode)
        {
            angle = LCD_ANGLE_0;
        }
        else
        {
            angle = LCD_ANGLE_90;
        }
        MMK_SetWinAngle(win_id, angle, FALSE);
        MMITHEME_SetUpdateDelayCount(0);
    }
#endif
#endif    
    if (CAMERA_MODE_DC == mode)
    {
        ret = MMIDC_OpenDC();
    }
    else
    {
        ret = MMIDC_OpenDV();
    }
    
    if (!ret )
    {
        CloseDCApplet();   
        MMIPUB_OpenAlertWarningWin(TXT_DC_OPEN_FAIL);	
    }
    else
    {        
        MMIDC_Setting_InitDefaultValue();

        MMIDC_CreateWinByApplet((uint32*)MMIDC_DC_TAB, (ADD_DATA)PNULL);
    }
    
    return ret;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author:tao.xue
//	Note: 
/*****************************************************************************/
LOCAL void DC_ValidatePrivacyAppEntry(BOOLEAN is_permitted, DPARAM param_ptr)
{
    CAMERA_MODE_E mode = CAMERA_MODE_DC;
    if (is_permitted)
    {
        if (PNULL != param_ptr)
        {
            mode = *(CAMERA_MODE_E*)param_ptr;
        }
        StartDCActivity(mode);
    }
    else
    {
        CloseDCApplet();
    }
}

/*****************************************************************************/
//Description : handle the message of dc wait window
//Global resource dependence : none
//Author: robert.wang
//Note: for release wallpaper buffer before open dc
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDCWaitWinMsg(
                                      MMI_WIN_ID_T win_id, 
                                      MMI_MESSAGE_ID_E msg_id, 
                                      DPARAM param	
                                      )
{
    MMI_RESULT_E recode	= MMI_RESULT_TRUE;
    CAMERA_MODE_E mode = CAMERA_MODE_DC;

    //SCI_TRACE_LOW:"[MMIDC]: HandleDCWaitWinMsg, msg_id = 0x%.2x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2809_112_2_18_2_9_21_410,(uint8*)"d", msg_id);
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //SCI_TRACE_LOW:"[MMIDC] HandleDCWaitWinMsg start_time=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2815_112_2_18_2_9_21_411,(uint8*)"d",SCI_GetTickCount());

        mode = (CAMERA_MODE_E)(uint32)MMK_GetWinAddDataPtr(win_id);/*lint !e64*/

        //SCI_TRACE_LOW:"[MMIDC] HandleDCWaitWinMsg mode=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2820_112_2_18_2_9_21_412,(uint8*)"d",mode);
                        
        MMIAPISET_ValidatePrivacy(MMISET_PROTECT_CAMER, (MMISET_PRIVACY_CALLBACK_PFUNC)DC_ValidatePrivacyAppEntry, (void *)&mode, sizeof(mode));
        MMK_CloseWin(win_id);
        break;

    default:
        recode	= MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

/*****************************************************************************/
//  Description : process record end
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN ProcessRecordEnd(void *param)
{
    BOOLEAN ret = TRUE;
    BOOLEAN is_normal = TRUE;
    DV_END_TYPE_E end_type = 0;
    BOOLEAN is_show = FALSE;

    end_type = *((DV_END_TYPE_E*)param);

    switch(end_type)
    {
    case DV_END_ERROR:
    case DV_END_BY_APP_CANCEL:
        break;
    default:
        MMIDC_FlowFunction(DC_INTERRUPT);
        break;
    }  

    switch(end_type)
    {
    case DV_END_ERROR:
        is_normal = FALSE;
        ret = FALSE;
        break;
    case DV_END_ROOM_INSUF_ERROR:
        MMIDC_ShowAppointedErrorMsg(SFS_ERROR_NO_SPACE);
        break;
    case DV_END_BY_APP_CANCEL:
        is_normal = FALSE;
        is_show = FALSE;
        ret = TRUE;
        break;
    case DV_END_LIMIT_SIZE:
    case DV_END_BY_APP:
    default:
        is_show = TRUE;
        break;
    }

    if (is_show)
    {
        ProcessStopRecord(end_type);
    }

    if (is_normal)
    {
#if defined(MMIDC_F_WORK_MODE)    
        if (MMIDC_IsWorkMode())
        {
            MMIDC_SaveWorkFile();
        }
#endif        
    }

    //SCI_TRACE_LOW:"[MMIDC] ProcessRecordEnd is_normal= %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2893_112_2_18_2_9_21_413,(uint8*)"d", is_normal);

    return ret;
}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleCameraWinMsg(
                                      MMI_WIN_ID_T		win_id, 
                                      MMI_MESSAGE_ID_E	msg_id, 
                                      DPARAM				param	
                                      )
{
    MMI_RESULT_E recode	= MMI_RESULT_TRUE;
#ifdef SUBLCD_SIZE
    BOOLEAN  flip_status = FALSE;
#endif    
    MMIDC_ERROR_TYPE_E err_ret = MMIDC_ERR_DELETE_FAIL;    
    
    //SCI_TRACE_LOW:"[MMIDC]: HandleCameraWinMsg, msg_id = 0x%.2x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2914_112_2_18_2_9_21_414,(uint8*)"d", msg_id);
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_OPEN_WINDOW"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2919_112_2_18_2_9_21_415,(uint8*)"");

        if(MMIDC_GetScreenMode() == SCREEN_MODE_HORIIZONTAL)
        {
            MMIDC_SetLcdForRotate(TRUE);
        }
        else if(MMIDC_GetScreenMode() == SCREEN_MODE_VERTICAL)
        {
            MMIDC_SetLcdForRotate(FALSE);
        }
        
#ifdef MMIDC_F_U_CAMERA
        if(CAMERA_MODE_DC == MMIDC_GetCameraMode() 
            && MMIDC_GetUCameraModuleFlag() 
           )
        {
            MMIDC_U_CAMERA_ERR_E err = UCAMERA_ERR_MAX;
            
            err = MMIDC_OpenUCamera();
            if (UCAMERA_ERR_NONE == err)
            {
                MMIDC_SetImageProcessType(IMG_PROC_LOMO);
            }
            else if(UCAMERA_ERR_NO_SD_CARD == err)
            {
                MMIDC_OpenUCameraAlertWin(TXT_PLEASE_INSERT_SD);//TXT_NO_SD_CARD_ALERT
                break;
            }
            else
            {
                MMIDC_OpenUCameraAlertWin(TXT_DC_OPEN_FAIL);
                break;
            }
        }
#endif


        //mmidcsrv
        MMIDCSRV_CheckParam();

        //create osd layer and dev info
        MMIDC_CreateMainOSDLayer();
#ifdef MMIDC_DESKTOP_PDA
        //pda parameter must be later at osd layer is created
        MMIDC_CreateDesktopPDA();
#endif
        
        MMITHEME_SetUpdateDelayCount(0);

#ifdef SUBLCD_SIZE
        flip_status = MMIDEFAULT_GetFlipStatus();
        //SCI_TRACE_LOW:"[MMIDC] HandleCameraWinMsg flip_status = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_2966_112_2_18_2_9_21_416,(uint8*)"d",flip_status);
        if (flip_status)
        {
            MMIDC_SetLcdID(MAIN_LCD_ID);
        }
        else
        {
           MMIDC_SetLcdID(SUB_LCD_ID);
        }
#else
        MMIDC_SetLcdID(MAIN_LCD_ID);
#endif

#if defined(MMIDC_F_WORK_MODE)
        if (MMIDC_IsWorkMode())
        {
            MMIDC_CheckAndSetWorkParam();
        }
#endif        

#ifdef MMIDC_F_U_CAMERA
        if(CAMERA_MODE_DC == MMIDC_GetCameraMode() 
            && MMIDC_GetUCameraModuleFlag() 
           )
        {
            MMIDC_OpenUCameraWin();
        }
        else
#endif
        {
            MMIDC_FlowStart();
        }


        MMIDC_PostCreateFileHandleMsg();
        
        if(s_is_chip_test)
        {
            GetChipTestLastAutoSave();
            MMIDC_SetAutoSave(AUTO_SAVE_ON);
        
            if (CAMERA_MODE_DC == MMIDC_GetCameraMode())
            {
                SetChipTestStep(MMIDC_CHIPTEST_STEP_SNAPSHOT);
            }
            else
            {
                SetChipTestStep(MMIDC_CHIPTEST_STEP_RECORD);
            }
            ResetChipTestTimer(5);
        }
        break;
        
    case MSG_LOSE_FOCUS:
        //SCI_TRACE_LOW:"[MMIDC]: HandleCameraWinMsg, MSG_LOSE_FOCUS"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3017_112_2_18_2_9_21_417,(uint8*)"");
        MMIDC_SetMMIFocusStatus(MMIDC_LOSE_FOCUS);
        
#ifdef MMIDC_F_U_CAMERA
       if(CAMERA_MODE_DC == MMIDC_GetCameraMode() 
            && MMIDC_IsUCameraEnable()
          )
        {
            MMIDC_CloseUCamera();
        }
#endif

        MMIDC_HandleLoseFocus();
        
        RestoreWorkEnvironmentForLoseFocus();
        
        MMK_SetWinSupportAngle(MMIDC_MAIN_WIN_ID, WIN_SUPPORT_ANGLE_ALL);
        MMK_SetWinAngle( MMIDC_MAIN_WIN_ID, MMK_GetScreenAngle(), FALSE);
        break;
        
    case MSG_GET_FOCUS:
        //SCI_TRACE_LOW:"[MMIDC]: HandleCameraWinMsg, MSG_GET_FOCUS"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3029_112_2_18_2_9_21_418,(uint8*)"");
        
        MMIDC_SetMMIFocusStatus(MMIDC_GET_FOCUS);

        if (s_is_chip_test)
        {
            break;
        }
        
        if(MMK_IsOpenWin(win_id))
        {
            InitWorkEnvironment();  
            InitWorkParameterForGetFocus();
            MMITHEME_SetUpdateDelayCount(0);

#if !defined(MMIDC_DESKTOP_PDA) || defined(MMIDC_F_PDA_BIT_16)
            if ((LCD_ANGLE_0 == MMIDC_GetPlatformAngle())
                && (DV_REVIEW_MODE == MMIDC_GetCurrentMode()))
            {
				s_need_full_paint = TRUE;
                MMIDC_SetScreenMode(SCREEN_MODE_VERTICAL);
            }
#endif    
            if(MMIDC_GetScreenMode() == SCREEN_MODE_HORIIZONTAL)
            {
                MMIDC_SetLcdForRotate(TRUE);
            }
            else if(MMIDC_GetScreenMode() == SCREEN_MODE_VERTICAL)
            {
                MMIDC_SetLcdForRotate(FALSE);
            }

#ifdef MMIDC_F_U_CAMERA
        if(CAMERA_MODE_DC == MMIDC_GetCameraMode() 
            && MMIDC_GetUCameraModuleFlag() 
           )
        {
            MMIDC_U_CAMERA_ERR_E err = UCAMERA_ERR_MAX;

            if (!MMIDC_IsUCameraEnable())
            {
                err = MMIDC_OpenUCamera();
                if (UCAMERA_ERR_NONE != err)
                {
                    MMIDC_OpenUCameraAlertWin(TXT_DC_OPEN_FAIL);
                    break;
                }
            }
            MMIDC_OpenUCameraFeature(MMIDC_GetImageProcessType());
        }
#endif

            
            MMK_SendMsg(MMIDC_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
            MMIDC_ReCreateMainOSDLayer();
            
#ifdef SUBLCD_SIZE       
            //DC在小屏时，不允许刷新小屏， CR175065
            flip_status = MMIDEFAULT_GetFlipStatus();
            if (!flip_status)
            {
    	        MMISUB_IsPermitUpdate(FALSE);
            }
#endif				
            
           MMIDC_HandleGetFocus();
        }
        break;
        
    case MSG_DC_CAPTURE_SUCCESS_MSG:	/** capture success **/
        //SCI_TRACE_LOW:"[MMIDC]: MSG_DC_CAPTURE_SUCCESS_MSG"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3069_112_2_18_2_9_21_419,(uint8*)"");
        if(MMK_IsFocusWin(win_id))
        {
            {
                s_is_capture_success_msg = TRUE; 

#ifdef SUBLCD_SIZE
                if(SUB_LCD_ID == MMIDC_GetLcdID())
                {
                    MMIDC_SetCurrentMode(DC_REVIEW_MODE);
                    Save();
                    MMIDC_ClearSUBLCD();
                    RSSave();//返回预览状态
                    MMIDC_CompleteDisplayOSD();
                }
                else 
#endif
                {
                    //for auto save or save key
                    if(MMIDC_GetIsAutoSave() == AUTO_SAVE_ON 
                        || s_is_pressed_save
                    #if defined(MMIDC_F_U_CAMERA)
                        || (MMIDC_IsUCameraEnable() 
                            && !MMIDC_IsPhotoData()
                            )
                    #endif
                        )
                    {
                        MMIDC_SetCurrentMode(DC_REVIEW_MODE);
                        Save();
                        s_is_pressed_save = FALSE;
                    }
                    else
                    {   
                        MMIDC_FlowFunction(DC_FORWARD);
                    }
                }
            }
        }
        break;

    case MSG_DC_PLAY_SHUTTER_VOICE:
        MMIAPISET_PlayRing((MN_DUAL_SYS_E)0, FALSE, 0, 1, MMISET_RING_TYPE_DC, MMIDC_ShutterViceCallback);
        break;

    case MSG_DC_REVIEW_SUCCESS_MSG:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_DC_REVIEW_SUCCESS_MSG"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3115_112_2_18_2_9_21_420,(uint8*)"");
        break;
        
    case MSG_DC_CAPTURE_FAILE_MSG:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_DC_CAPTURE_FAILE_MSG"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3119_112_2_18_2_9_21_421,(uint8*)"");

        MMIDC_PostCreateFileHandleMsg();

         if (s_is_back_to_preview)
        {
            //for cancel key
            s_is_back_to_preview = FALSE;
            RSDCReview();            
            break;
        }
        if (PNULL != param)
        {
            MMIFILE_ERROR_E  file_ret = 0;
            MMIFILE_DEVICE_E  file_dev = 0; 

            file_ret = *((MMIFILE_ERROR_E*)param);
            file_dev = MMIDC_GetPhotoStorageDevice();
            MMIDC_ShowErrorMsg(file_ret, file_dev);
        }
        else
        {
            MMIDC_OpenAlertWin(MMIPUB_SOFTKEY_ONE, TXT_DC_CAPTURE_FAILED, IMAGE_PUBWIN_FAIL, MMI_2SECONDS);  
        }
        break;
        
    case MSG_RECORD_END:
        {
            if (!ProcessRecordEnd(param))
            {
                MMK_CloseWin(win_id);
            }
        }
        break;
        
     case MSG_TIMER:
        if(MMK_IsFocusWin(win_id) || s_is_chip_test)
        {
            HandleWinTimer(*((uint8*)param));
        }
        break;      
    case MSG_FULL_PAINT:
        if(!MMK_IsFocusWin(win_id))
        {
            MMIDC_ClearMainBlock(MMI_BLACK_COLOR);
        }
        break;
    case MSG_PROMPTWIN_OK:
        if(MMIDC_GetCameraMode() == CAMERA_MODE_DC)
        {
            MMIDC_DeletePhoto();
            MMIPUB_CloseQuerytWinEx(SPRD_CAMERA_APPLET_ID, PNULL);
        }
        else
        {
            if(!MMIDC_DeleteNoSavedFile())
            {
                err_ret = MMIDC_ERR_DELETE_FAIL;
		        MMIDC_ShowMsg(err_ret);
            }    
            MMIPUB_CloseQuerytWinEx(SPRD_CAMERA_APPLET_ID, PNULL);
            MMIDC_SetCurrentMode(DV_PREVIEW_MODE);
        }
        break;
        
    case MSG_PROMPTWIN_CANCEL: 
        MMIPUB_CloseQuerytWinEx(SPRD_CAMERA_APPLET_ID, PNULL);
        break;
#if !defined(MMIDC_DESKTOP_PDA)     
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_WEB"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3187_112_2_18_2_9_21_422,(uint8*)"");
        if (s_is_chip_test)
        {
            break;
        }
        if(MMIDC_IsCapturing())
        {
            Save();
        }
        else
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_OK_KEY);
            }
        }    
        break;            
    case MSG_APP_CAMERA:
        if (MMIAPIDC_IsRecording())
        {
            KeyFunction(DC_LEFT_SOFT_KEY);
            break;
        }

        if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
        {
            if(MMIDC_GetCurrentMode() == DC_PREVIEW_MODE || MMIDC_GetCurrentMode() == DV_PREVIEW_MODE)
            {
                MMIDC_CloseAllOSDMenu();
                KeyFunction(DC_OK_KEY);
            }
        }
        break;
 
    case MSG_APP_OK:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_OK"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3222_112_2_18_2_9_21_423,(uint8*)"");
        if (s_is_chip_test)
        {
            break;
        }
        
        //in sub lcd, not permit to open osd menu in preview state
        if (SUB_LCD_ID == MMIDC_GetLcdID()
            && MMIAPIDC_IsPreviewing()
           )
        {
            break;
        }
        
        if(MMIDC_IsCapturing())
        {
            Save();
        }
        else
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_LEFT_SOFT_KEY);
            }
        }
        break;		
    case MSG_APP_CANCEL:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_CANCEL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3249_112_2_18_2_9_22_424,(uint8*)"");
        if(MMIDC_IsCapturing())
        {
            BackToPreview();
        }
        else
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_RIGHT_SOFT_KEY);
    			//在关闭osd菜单的时候需要将这个定时器停掉，否则再次打开菜单就无法创建该定时器了
    			MMIDC_StopTextScrollTimer();//hisense2 add to modify CR 149684
            }
        }
        break;
        
    case MSG_APP_LEFT:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_LEFT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3266_112_2_18_2_9_22_425,(uint8*)"");
        if (s_is_chip_test)
        {
            break;
        }
        
        //in sub lcd, not permit to open osd menu in preview state
        if (SUB_LCD_ID == MMIDC_GetLcdID()
            && MMIAPIDC_IsPreviewing()
           )
        {
            break;
        }
        
        if (MAIN_LCD_ID == MMIDC_GetLcdID())
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_LEFT_KEY);
                StartSwitchOSDMenuItemsKeyTimer(LEFT_KEY);
            }
        }
        break;
        
    case MSG_APP_RIGHT:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_RIGHT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3291_112_2_18_2_9_22_426,(uint8*)"");
        if (s_is_chip_test)
        {
            break;
        }
        
        //in sub lcd, not permit to open osd menu in preview state
        if (SUB_LCD_ID == MMIDC_GetLcdID()
            && MMIAPIDC_IsPreviewing()
           )
        {
            break;
        } 
        
        if (MAIN_LCD_ID == MMIDC_GetLcdID())
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_RIGHT_KEY);
                StartSwitchOSDMenuItemsKeyTimer(RIGHT_KEY);
            }
        }
        break;
        
    case MSG_KEYUP_LEFT:
    case MSG_KEYUP_RIGHT:	
    case MSG_KEYPRESSUP_LEFT:
    case MSG_KEYPRESSUP_RIGHT:
    case MSG_KEYUP_UP:
    case MSG_KEYUP_UPSIDE:    
    case MSG_KEYUP_DOWN:
    case MSG_KEYUP_DOWNSIDE:    
    case MSG_KEYPRESSUP_UP:
    case MSG_KEYPRESSUP_DOWN:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_KEYUP"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3325_112_2_18_2_9_22_427,(uint8*)"");
        StopSwitchOSDMenuItemsKeyTimer();
        break;
        
    case MSG_APP_UP:
    case MSG_APP_UPSIDE:
    case MSG_KEYREPEAT_UPSIDE:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_UP"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3332_112_2_18_2_9_22_428,(uint8*)"");
        if (s_is_chip_test)
        {
            break;
        }
        
        //in sub lcd, not permit to zoom in preview state
        if (SUB_LCD_ID == MMIDC_GetLcdID()
            && MMIAPIDC_IsPreviewing()
           )
        {
            break;
        }
        
        if (MAIN_LCD_ID == MMIDC_GetLcdID())
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_UP_KEY);
                StartSwitchOSDMenuItemsKeyTimer(UP_KEY);
            }
        } 
        break;       
    case MSG_APP_DOWNSIDE:
    case MSG_KEYREPEAT_DOWNSIDE:
    case MSG_APP_DOWN:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_APP_DOWN"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3358_112_2_18_2_9_22_429,(uint8*)"");
        if (s_is_chip_test)
        {
            break;
        }
        
        //in sub lcd, not permit to zoom in preview state
        if (SUB_LCD_ID == MMIDC_GetLcdID()
            && MMIAPIDC_IsPreviewing()
           )
        {
            break;
        }
        
        if (MAIN_LCD_ID == MMIDC_GetLcdID())
        {
            if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
            {
                KeyFunction(DC_DOWN_KEY); 
                StartSwitchOSDMenuItemsKeyTimer(DOWN_KEY);
            }
        }
        break;
    case MSG_TP_PRESS_UP:
        if (s_is_chip_test)
        {
            break;
        }
        
        if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
        {
            GUI_POINT_T	point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            HandleDCWinTpPressDown(point);
        }
        break;
#endif  //#if !defined(MMIDC_DESKTOP_PDA)         
    case MSG_DC_MMI_STATE:
        MMIAPIDC_Exit();
        break;
    case MSG_DC_MMI_CMD:
        MMIDC_ProcessMsgCmd(*(MMIDC_CMD_E*)param);
        break;
    case MSG_CLOSE_WINDOW:
        //SCI_TRACE_LOW:"[MMIDC]: MSG_CLOSE_WINDOW"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3403_112_2_18_2_9_22_430,(uint8*)"");
        CloseDCApplet();        
        break;

#ifdef SUBLCD_SIZE
#ifdef FLIP_PHONE_SUPPORT
    case MSG_KEYDOWN_FLIP:
        //SCI_TRACE_LOW:"[MMIDC] HandleCameraWinMsg: MSG_KEYDOWN_FLIP!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3410_112_2_18_2_9_22_431,(uint8*)"");
		if (MMK_IsFocusWin(MMIDC_MAIN_WIN_ID) 
		    && DC_PREVIEW_MODE == MMIDC_GetCurrentMode()
			&& MMIAPIDC_IsPreviewing()
		   )
		{
			MMIDC_HandleLoseFocus();
			MMIDC_SetLcdID(SUB_LCD_ID);
			MMIDC_HandleGetFocus();
			MMISUB_IsPermitUpdate(FALSE);
		}
		else
		{
			recode = MMI_RESULT_FALSE;
		}
		break;

    case MSG_KEYUP_FLIP:
		{
	        //SCI_TRACE_LOW:"[MMIDC] HandleCameraWinMsg: MSG_KEYUP_FLIP! %d %d"
	        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3429_112_2_18_2_9_22_432,(uint8*)"dd",(MMK_IsFocusWin(MMIDC_MAIN_WIN_ID)),(MMIDC_GetCurrentMode()));
			MMIDC_SetLcdID(MAIN_LCD_ID);
			if (MMK_IsFocusWin(MMIDC_MAIN_WIN_ID) 
			    && DC_PREVIEW_MODE == MMIDC_GetCurrentMode()
				&& MMIAPIDC_IsPreviewing()
				)
			{
				//SCI_TRACE_LOW:"[MMIDC] Go on preview!"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3436_112_2_18_2_9_22_433,(uint8*)"");
				MMIDC_HandleLoseFocus();
				MMIDC_HandleGetFocus();
	            MMIDC_ShowAllOSD();
			}
			else if (MMK_IsFocusWin(MMIDC_MAIN_WIN_ID) 
			         && DC_REVIEW_MODE == MMIDC_GetCurrentMode()
				     ||DC_SAVE_MODE == MMIDC_GetCurrentMode()
				     )
			{
				//SCI_TRACE_LOW:"[MMIDC] Display picture!"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3446_112_2_18_2_9_22_434,(uint8*)"");
				
				MMIDC_ClearOSDBlock();

			    MMIDC_ReviewPhotos();
			}
			MMISUB_IsPermitUpdate(TRUE);
			recode = MMI_RESULT_FALSE;
	    }
        break;
#endif        
#endif 

#if 0
//    case MSG_GSENSOR:
        //mmk angle will be convert
//        SCI_TRACE_LOW("[MMIDC] HandleCameraWinMsg: MSG_GSENSOR");
//        break;
    case MSG_LCD_SWITCH:
        //mmk angle is converted 
        //SCI_TRACE_LOW:"[MMIDC] HandleCameraWinMsg: MSG_LCD_SWITCH"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3466_112_2_18_2_9_22_435,(uint8*)"");
        if (MMK_IsFocusWin(MMIDC_MAIN_WIN_ID))
        {
            if (MMIAPIDC_IsPreviewing()
               || MMIDC_GetSettingItemHandle()->SettingItemIsOpen()
               )
            {
                MMIDC_HandleLoseFocus();
                MMIDC_ResetLogicalScreen();
                MMIDC_HandleGetFocus();
            }
        }
        break;
#endif

#if defined(MMIDC_DESKTOP_ICON_BAR)
    case MSG_APP_1:
    case MSG_APP_2:
    case MSG_APP_3:
    case MSG_APP_4:
    case MSG_APP_5:
    case MSG_APP_6:
    case MSG_APP_7:
    case MSG_APP_8:
    case MSG_APP_9:
    case MSG_APP_0:
    case MSG_APP_STAR:
    case MSG_APP_HASH:
        //SCI_TRACE_LOW:"[MMIDC]: HandleCameraWinMsg msg_id = 0x%.2x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3494_112_2_18_2_9_22_436,(uint8*)"d", msg_id);
        if (MMIDC_IsIconDesktopOpen()
            && !MMIDC_IsReviewing() 
            && !MMIDC_IsCapturing() 
            && !MMIDC_IsSettingParamCtrl()
            )
        {
            HandleShortcutKey(msg_id);
        }
        break;
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)   

#ifdef MMIDC_DESKTOP_PDA
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
    case MSG_APP_CAMERA:

    case MSG_CTL_PENLONGOK:
    case MSG_CTL_PENOK:
        
    case MSG_APP_OK:
    case MSG_CTL_OK:

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:   

    case MSG_APP_RIGHT:
    case MSG_KEYREPEAT_RIGHT:
    case MSG_APP_LEFT:
    case MSG_KEYREPEAT_LEFT:
    case MSG_APP_UP:
    case MSG_KEYREPEAT_UP:
    case MSG_APP_DOWN:
    case MSG_KEYREPEAT_DOWN:
    case MSG_DC_RETURN_KEY:
        
    case MSG_APP_MENU:
        
    case MSG_TP_PRESS_UP:
    case MSG_TP_PRESS_DOWN:
    case MSG_TP_PRESS_MOVE:

    case MSG_CTL_BUTTON_DOWN:
//    case MSG_CTL_BUTTON_UP:    
    
    case MSG_APP_UPSIDE:
    case MSG_KEYREPEAT_UPSIDE:
    case MSG_APP_DOWNSIDE:
    case MSG_KEYREPEAT_DOWNSIDE:    
        if (s_is_chip_test)
        {
            break;
        }

        MMIDC_DesktopProcessMsg(msg_id, param);
        break;
#endif         
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }


    //camera exit when do nothing o dc preview state and in one minute.
    if (DC_PREVIEW_MODE == MMIDC_GetCurrentMode()
        || (DV_PREVIEW_MODE == MMIDC_GetCurrentMode())
        )
    {
        if (((msg_id >= MSG_KEYDOWN_UP) && (msg_id <= MSG_KEYDOWN_AT))
            || ((msg_id >=MSG_TP_PRESS_DOWN) && (msg_id <= MSG_TP_PRESS_DRAG))
           )
        {
            MMIDC_ResetHitTimer();
        }
    }
    else
    {
        CloseHitTimer();
    }
    
    return recode;
}

/*****************************************************************************/
// 	Description : get camera is open
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIDC_IsOpened(void)
{
    return (MMK_IsOpenWin(MMIDC_MAIN_WIN_ID) || MMIENG_IsDCEngMode());
}

#ifdef WRE_SUPPORT	
extern int32 wre_in_camera_scan_mode(void);
extern void wre_camera_exit_callback(void);
#endif
/*****************************************************************************/
// 	Description : close camera
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIDC_Exit(void)
{
    //SCI_TRACE_LOW:"[MMIDC]: MMIAPIDC_Exit"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3593_112_2_18_2_9_22_437,(uint8*)"");
    
    if(MMK_IsOpenWin(MMIDC_HELP_WIN_ID))
    {
        MMK_CloseWin(MMIDC_HELP_WIN_ID);
    }
    if(MMK_IsOpenWin(MMIDC_SEND_WIN_ID))
    {
        MMK_CloseWin(MMIDC_SEND_WIN_ID);
    }
    if(MMK_IsOpenWin(MMIDC_PHOTO_REVIEW_OPTION_WIN_ID))
    {
        MMK_CloseWin(MMIDC_PHOTO_REVIEW_OPTION_WIN_ID);
    }
    if(MMK_IsOpenWin(MMIDC_STORAGE_OPT_WIN_ID))
    {
        MMK_CloseWin(MMIDC_STORAGE_OPT_WIN_ID);
    }

    //last to close main win
    if(MMK_IsOpenWin(MMIDC_MAIN_WIN_ID))
    {
        MMK_CloseWin(MMIDC_MAIN_WIN_ID);
    }
    
    MMIPUB_CloseQuerytWinEx(SPRD_CAMERA_APPLET_ID, PNULL);
    MMIPUB_CloseAlertWinEx(SPRD_CAMERA_APPLET_ID);

    #ifdef WRE_SUPPORT	
		if(wre_in_camera_scan_mode()){
			wre_camera_exit_callback();
		}
    #endif		
}

/*****************************************************************************/
// 	Description : interrupt camera
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIDC_Interrupt(void)
{
    //SCI_TRACE_LOW:"[MMIDC]: MMIAPIDC_Interrupt"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3634_112_2_18_2_9_22_438,(uint8*)"");
    if(MMK_IsFocusWin(MMIDC_MAIN_WIN_ID))
    {
        MMK_SendMsg(MMIDC_MAIN_WIN_ID, MSG_LOSE_FOCUS, 0);
    }
}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC uint32 MMIDC_GetRecordSecond(void)
{
#ifdef WIN32
    uint32 sleep_time = 0;

    sleep_time = MMI_1SECONDS - MMIDC_RECORD_COUNT_TIME;
    SCI_Sleep(sleep_time);
    
    return s_record_total_time;
#else
    DRECORDER_RECORD_INFO_T record_info;
    
    DRECORDER_GetRecordInfo(&record_info);

    return (record_info.media_time / 1000);
#endif    
}

/*****************************************************************************/
// 	Description : init camera module
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIDC_InitModule(void)
{
    MMIDC_RegMenuWin();
    MMI_RegWinIdNameArr(MMI_MODULE_CAMERA, (const uint8**)mmidc_id_name_arr);
    MMIDC_RegSettingNv();

    RegDCAppletInfo();

    //启动手机时，删除相机的临时文件
    MMIDC_DeleteAllDiskImageFile();
}


/*****************************************************************************/
// 	Description : handle photo and video send window msg
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSendWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E            result      = MMI_RESULT_TRUE;
    MMI_MENU_ID_T	        menu_id     = 0;
    MMI_MENU_GROUP_ID_T     group_id    = 0;
    
    //SCI_TRACE_LOW:"mmidc_wintab.c,  HandleSendWinMsg(), msg_id = %x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3690_112_2_18_2_9_22_439,(uint8*)"d", msg_id);
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(MMIDC_SEND_WIN_ID, MMIDC_SEND_MENU_CTRL_ID);
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_COMMON_OK, NULL, STXT_RETURN, FALSE);


        #ifdef SNS_SUPPORT
        if (CAMERA_MODE_DV == MMIDC_GetCameraMode())
        {
            MMI_CTRL_ID_T ctrl_id = MMIDC_SEND_MENU_CTRL_ID;
            MMI_MENU_GROUP_ID_T group_opt_id    = MMIDC_SEND_OPT;

            
            #ifdef WRE_WEIBO_SUPPORT
            menu_id = ID_CAMERA_SHARE_TO_SINA;
            GUIMENU_SetItemVisible(ctrl_id, group_opt_id, menu_id, FALSE);
            #endif

            #ifdef WRE_FACEBOOK_SUPPORT
            menu_id = ID_CAMERA_SHARE_TO_FACEBOOK;
            GUIMENU_SetItemVisible(ctrl_id, group_opt_id, menu_id, FALSE);
            #endif

            #ifdef WRE_TWITTER_SUPPORT
            menu_id = ID_CAMERA_SHARE_TO_TIWTTER;
            GUIMENU_SetItemVisible(ctrl_id, group_opt_id, menu_id, FALSE);
            #endif
        }
        #endif
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_WEB:
        GUIMENU_GetId(MMIDC_SEND_MENU_CTRL_ID, (MMI_MENU_GROUP_ID_T *)(&group_id),(MMI_MENU_ID_T *) (&menu_id));
        switch ( menu_id )
        {
#if defined(MMS_SUPPORT)        
        case ID_CAMERA_SEND_BY_MMS:
            MMIDC_SendByMMS();
            break;
#endif
#if defined(BLUETOOTH_SUPPORT)        
        case ID_CAMERA_SEND_BY_BT:
            MMIDC_SendByBt();
            break;
#endif            
#ifdef SNS_SUPPORT
        case ID_CAMERA_SHARE_TO_SINA:
        case ID_CAMERA_SHARE_TO_FACEBOOK:
        case ID_CAMERA_SHARE_TO_TIWTTER:
             MMIDC_SharetoSNS(menu_id);
             break;
        case MSG_SNS_OPERA_RESULT:
            HandleSNSRelease();    
            break;     
#endif
        default:
            break;
        }
        break;
        
        case MSG_CTL_CANCEL:
            MMK_CloseWin(win_id);
            break;
            
        default:
            result = MMI_RESULT_FALSE;
            break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : open send photo window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_OpenSendPhotoWin(void)
{
/*
#ifndef BLUETOOTH_SUPPORT
        MMIDC_HandleLoseFocus();
        MMIDC_SendByMMS();
#else
    MMIDC_CreateWinByApplet((uint32*)MMIDC_SEND_TAB, (ADD_DATA)PNULL);
#endif
*/
    MMIDC_CreateWinByApplet((uint32*)MMIDC_SEND_TAB, (ADD_DATA)PNULL);
}

/*****************************************************************************/
// 	Description : dc engineer mode
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_CloseStoreWin(void)
{

}

/*****************************************************************************/
// 	Description : post dc state msg  for error
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_PostStateMsg(void)
{
     MMK_PostMsg(MMIDC_MAIN_WIN_ID, MSG_DC_MMI_STATE, (DPARAM)PNULL, 0);
}

/*****************************************************************************/
// 	Description : clean all osd in preview status
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_CloseAllOSDMenu(void)
{
#if defined(MMIDC_DESKTOP_PDA)
    MMIDC_ClearOSDBlock();
#else
    MMIDC_CloseAdjust(); 

    MMIDC_GetIconHandle()->IconClose();
    MMIDC_GetMenuHandle()->MenuClose();
    
#if defined(MMIDC_DESKTOP_ICON_BAR)    
    MMIDC_CloseGUIIconDesktop();
#endif

#endif
}

/*****************************************************************************/
// 	Description : clear all osd in preview status 
//                and display current mode softkey
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC void MMIDC_ClearMenuInPreview(void)
{
#if !defined(MMIDC_DESKTOP_PDA)
    DC_FLOW_MODE_E  flow_mode = MMIDC_GetCurrentMode();
    
    if (MMIDC_GetIconHandle()->IconIsOpen()
       && ((DC_PREVIEW_MODE == flow_mode) || (DV_PREVIEW_MODE == flow_mode))
       && (!MMIDC_IsFocusMenuGray())
       )
    {
        MMIDC_CloseAllOSDMenu();
    }
#endif    
}

/*****************************************************************************/
//Description : start hit timer
//Global resource dependence : none
//Author: robert.wang
//Note: camera exit while do nothing in 1 minute 
/*****************************************************************************/
PUBLIC void MMIDC_ResetHitTimer(void)
{
    if (0 == s_hit_timer)
    {
        s_hit_timer = MMK_CreateTimer(60*MMI_1SECONDS, TRUE);
    }
    else
    {
        MMK_StopTimer(s_hit_timer);
        s_hit_timer = MMK_CreateTimer(60*MMI_1SECONDS, TRUE);
    }
}

/*****************************************************************************/
// 	Description : close hit timer
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void CloseHitTimer(void)
{
    if (0 != s_hit_timer)
    {
        MMK_StopTimer(s_hit_timer);
        s_hit_timer = 0;
    }
}

/*****************************************************************************/
// 	Description : handle the message of dc window
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL void CloseDCAllTimers(void)
{
    MMIDC_CloseCallbackTimer();

    CloseShootDelayTimer();
    CloseTipDisplayTimer();
    CloseUpDownTipTimer();
    StopSwitchOSDMenuItemsKeyTimer();
    CloseRecordTimer();
    MMIDC_StopTextScrollTimer();
    CloseHitTimer();
#ifdef MMIDC_DESKTOP_PDA
    MMIDC_CloseAllPDATimer();
#endif
}

/*****************************************************************************/
// 	Description : get count down seconds
//	Global resource dependence : none
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC int32 MMIDC_GetCountDownSeconds(void)
{
    return s_timer_count_down;
}

/*****************************************************************************/
// 	Description : get chip test step
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL MMIDC_CHIPTEST_STEP_E GetChipTestStep(void)
{
    return s_chiptest_step;
}

/*****************************************************************************/
// 	Description : set chip test step
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void SetChipTestStep(MMIDC_CHIPTEST_STEP_E step)
{
    s_chiptest_step = step;
}

/*****************************************************************************/
// 	Description : set chip test step
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void ProcessChipTestStep(void)
{
    MMIDC_CHIPTEST_STEP_E step = MMIDC_CHIPTEST_STEP_START;
    VIDEO_SIZE_E  video_size = 0;
    DCAMERA_IMG_SENSOR_TYPE_E sensor_type = 0;
    int16 i = 0;

    step = GetChipTestStep();

    //SCI_TRACE_LOW:"[MMIDC] ProcessChipTestStep step=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_3902_112_2_18_2_9_23_440,(uint8*)"d", step);
    
    switch(step)
    {
    case MMIDC_CHIPTEST_STEP_START:
    case MMIDC_CHIPTEST_STEP_SNAPSHOT:
        MMIDC_CloseAllOSDMenu();
        KeyFunction(DC_OK_KEY);
#ifdef VIDEO_PLAYER_SUPPORT
        SetChipTestStep(MMIDC_CHIPTEST_STEP_RECORD);
#else
        SetChipTestStep(MMIDC_CHIPTEST_STEP_END);
#endif
        ResetChipTestTimer(5);
        break;
    case MMIDC_CHIPTEST_STEP_RECORD:
        for (i = 0; i < 3; i++)
        {
            MMIDC_PlayCountVoice(1);
            SCI_Sleep(1000);
        }
    
        MMIDC_CloseAllOSDMenu();
        MMIDC_FlowFunction(DC_EXIT);
        MMIDC_SetCameraMode(CAMERA_MODE_DV);  
        GetChipTestLastVideoSize();
        sensor_type = MMIAPIDC_GetDCModulSensorType();
        if (DCAMERA_IMG_SENSOR_TYPE_4P3 == sensor_type)
        {
            video_size = VIDEO_SIZE_128X96;
        }
        else
        {
            video_size = VIDEO_SIZE_96X128;
        }
        MMIDC_SetVideoSize(video_size);
        MMIDC_FlowStart();

        Record();
        SetChipTestStep(MMIDC_CHIPTEST_STEP_PLAY);
        ResetChipTestTimer(5);
        break;
    case MMIDC_CHIPTEST_STEP_PLAY:
        StopRecord();
        RestoreChipTestLastVideoSize();

        MMIDC_ReviewVideo();
        SetChipTestStep(MMIDC_CHIPTEST_STEP_END);
        ResetChipTestTimer(5);
        break;
    case MMIDC_CHIPTEST_STEP_END:
#ifdef VIDEO_PLAYER_SUPPORT
        MMIAPIVP_ExitVideoPlayer();
#endif        
        RestoreChipTestLastAutoSave();

        //exit dc chip test
        MMIDC_PostStateMsg();
        break;
    default:
        break;
    }
    
}

/*****************************************************************************/
// 	Description : reset chip test timer
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void ResetChipTestTimer(int count_s)
{
    if(s_is_chip_test_timer > 0)
    {
        MMK_StopTimer(s_is_chip_test_timer);
    }
    s_is_chip_test_timer = MMK_CreateTimer(count_s * MMI_1SECONDS, TRUE);		
}

/*****************************************************************************/
// 	Description : close chip test timer
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void CloseChipTestTimer(void)
{
    if (0 != s_is_chip_test_timer)
    {
        MMK_StopTimer(s_is_chip_test_timer);
    }
    s_is_chip_test_timer = 0;
}

/*****************************************************************************/
// 	Description : get chip test last video size
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void GetChipTestLastVideoSize(void)
{
    s_chiptest_last_video_size = MMIDC_GetVideoSize();
}

/*****************************************************************************/
// 	Description : restore chip test last video size
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void RestoreChipTestLastVideoSize(void)
{
    //SCI_TRACE_LOW:"[MMIDC] RestoreChipTestLastVideoSize video_size =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4008_112_2_18_2_9_23_441,(uint8*)"d", s_chiptest_last_video_size);
    
    MMIDC_SetVideoSize(s_chiptest_last_video_size);
}

/*****************************************************************************/
// 	Description : get chip test last auto save
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void GetChipTestLastAutoSave(void)
{
    s_chiptest_last_is_autosave = MMIDC_GetIsAutoSave();
}
/*****************************************************************************/
// 	Description : restore chip test last auto save
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void RestoreChipTestLastAutoSave(void)
{
    //SCI_TRACE_LOW:"[MMIDC] RestoreChipTestLastAutoSave autosave=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4027_112_2_18_2_9_23_442,(uint8*)"d", s_chiptest_last_is_autosave);
    MMIDC_SetAutoSave(s_chiptest_last_is_autosave);
}

/*****************************************************************************/
//Description :  handle shortcut key
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void HandleShortcutKey(MMI_MESSAGE_ID_E msg_id)
{
#ifdef MMIDC_DESKTOP_ICON_BAR

    BOOLEAN is_need_update = TRUE;
    CAMERA_MODE_E  mode = 0;

    mode = MMIDC_GetCameraMode();

    //SCI_TRACE_LOW:"[MMIDC] HandleShortcutKey msg_id = %d, mode=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4044_112_2_18_2_9_23_443,(uint8*)"dd", msg_id, mode);

    switch(msg_id)
    {
    case MSG_APP_LEFT:
        MMIDC_OpenMoreOption();
        break;
    case MSG_APP_RIGHT:
        MMIDC_OpenMoreOption();
        break;
    case MSG_APP_HASH: 
#if defined (MMIDC_F_U_CAMERA)
        if(!MMIDC_IsUCameraEnable())
#endif
        {
            MMIDC_ShortcutForBrightnessToLow();
        }
        break;
    case MSG_APP_STAR:    
#if defined (MMIDC_F_U_CAMERA)
        if(!MMIDC_IsUCameraEnable())
#endif
        {
            MMIDC_ShortcutForBrightnessToHigh();
        }
        break;
     case MSG_APP_UP: 
        if (CAMERA_MODE_DC == mode)
        {
            MMIDC_ShortcutForZoomToHigh();
        }
        else
        {
#if defined(MMIDC_F_DV_ZOOM)         
            UpDVZoom();
#else
            MMIDC_ShortcutForContrastToHigh();
#endif
        }
        break;
    case MSG_APP_DOWN:
        if (CAMERA_MODE_DC == mode)
        {
            MMIDC_ShortcutForZoomToLow();
        }
        else
        {
#if defined(MMIDC_F_DV_ZOOM)         
            DownDVZoom();
#else
            MMIDC_ShortcutForContrastToLow();
#endif
        }
        break;
    case MSG_APP_1:
        MMIDC_ShortcutForSwitchSensor();
        break;
    case MSG_APP_C:
        MMIDC_ShortcutForCapture(); //for capture 
        break;
    case MSG_APP_R:
        MMIDC_ShortcutForRecorder(); //for recorder
        break;
    case MSG_APP_2:
#if defined (MMIDC_F_U_CAMERA)
        if(!MMIDC_IsUCameraEnable())
#endif        
        {
#if defined (MMIDC_F_WORK_MODE)
            if(!MMIDC_IsWorkMode())
#endif
            {
                MMIDC_ShortcutForCameraMode();
            }
        }
        break;
    case MSG_APP_3:
#if defined (MMIDC_F_U_CAMERA)
        if(!MMIDC_IsUCameraEnable())
#endif        
        {
            MMIDC_ShortcutForSize();
        }
        break;
    case MSG_APP_4:
        if (CAMERA_MODE_DC == mode)
        {
#if defined (MMIDC_F_U_CAMERA)
            if(!MMIDC_IsUCameraEnable())
#endif                
            {
                MMIDC_ShortcutForEnvironment();
            }
        }
        else
        {
            MMIDC_ShortcutForEffect();
        }
        break;
    case MSG_APP_5:
        if (CAMERA_MODE_DC == mode)
        {
#if defined (MMIDC_F_U_CAMERA)
            if(!MMIDC_IsUCameraEnable())
#endif                
            {
                MMIDC_ShortcutForWhiteBalance();
            }
       }
        break;
    case MSG_APP_6:
        if (CAMERA_MODE_DC == mode)
        {
            MMIDC_ShortcutForDelayTime();
        }
        break;
    case MSG_APP_7:
        MMIDC_ShortcutForStore();
        break;     
    case MSG_APP_8:
        if (CAMERA_MODE_DC == mode)
        {
#if defined (MMIDC_F_U_CAMERA)
            if(!MMIDC_IsUCameraEnable())
#endif                
            {
#if defined (MMIDC_F_WORK_MODE)
                if(!MMIDC_IsWorkMode())
#endif
                {
                    MMIDC_ShortcutForMultiShot();
                }
            }
        }
        break;
#if defined (MMIDC_F_U_CAMERA)  
    case MSG_APP_9:
        if(CAMERA_MODE_DC == mode)
        {
#if defined (MMIDC_F_WORK_MODE)
            if(MMIDC_IsWorkMode())
#endif
            {
                MMIDC_OpenUCameraWin();
            }
        }
        break;
#endif        
    default:
        is_need_update = FALSE;
        break;
    }

    if (is_need_update)
    {
        MMIDC_ClearAllRect();
        
        MMIDC_ShowAllOSD();
    }
#endif    
}

#if defined(MMIDC_DESKTOP_ICON_BAR)
/*****************************************************************************/
//Description :  handle tp for icon bar
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleTPForIconBar(GUI_POINT_T point)
{
    BOOLEAN is_desktop_open = FALSE;

    is_desktop_open = MMIDC_IsIconDesktopOpen();

    //SCI_TRACE_LOW:"[MMIDC] HandleTPForIconBar is_desktop_open=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4217_112_2_18_2_9_23_444,(uint8*)"d", is_desktop_open);
    if (is_desktop_open)
    {
        BOOLEAN  ret = FALSE;

        int16       rect_index = 0;
        GUI_RECT_T  disp_rect = {0};
        MMIDC_ICON_DISP_T disp_list[ICON_RECT_MAX] = {0};
        MMI_MESSAGE_ID_E msg_id = 0;
        
        int16 i = 0;
        int16 total_num = 0;

        total_num = MMIDC_GetIconDesktopRectList(ARR_SIZE(disp_list), disp_list);
        for (i = 0; i < total_num; i++)
        {
            disp_rect = disp_list[i].rect;
            if(GUI_PointIsInRect(point, disp_rect))
            {
                ret = TRUE;
                rect_index = disp_list[i].rect_index;
                
                //SCI_TRACE_LOW:"[MMIDC] HandleTPForIconBar rect_index=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4239_112_2_18_2_9_23_445,(uint8*)"d", rect_index);
                
                switch(rect_index)
                {
                case ICON_RECT_LEFT:
                    msg_id = MSG_APP_STAR; //adjust to high  by touch
                    break;
                case ICON_RECT_RIGHT:
                    msg_id =MSG_APP_HASH; //adjust to low by touch
                    break;
                case ICON_RECT_UP:
                    msg_id = MSG_APP_UP;
                    break;
                case ICON_RECT_DOWN:
                    msg_id = MSG_APP_DOWN;
                    break;
                case ICON_RECT_CAPTURE:
                    msg_id = MSG_APP_C; //for capture
                    break;
                case ICON_RECT_RECORDER:
                    msg_id = MSG_APP_R; //for recorder
                    break;
                case ICON_RECT_SENSOR:
                    msg_id = MSG_APP_1;
                    break;
                case ICON_RECT_SIZE:
                    msg_id = MSG_APP_3;
                    break;
                case ICON_RECT_NIGHT:
                    msg_id = MSG_APP_4;
                    break;
                case ICON_RECT_WB:
                    msg_id = MSG_APP_5;
                    break;
                case ICON_RECT_DELAY:
                    msg_id = MSG_APP_6;
                    break;
                case ICON_RECT_STORE:
                    msg_id = MSG_APP_7;
                    break;
                case ICON_RECT_MULTI:
                    msg_id = MSG_APP_8;
                    break;
                default:
                    ret = FALSE;
                    break;
                }

                //if finded rect to handle, then break
                //other continue
                if (ret)
                {
                    break;//break for 
                }
            }
        }

        if (ret)
        {
            HandleShortcutKey(msg_id);
        }

        //SCI_TRACE_LOW:"[MMIDC] HandleTPForIconBar msg_id=%d, ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4301_112_2_18_2_9_24_446,(uint8*)"dd", msg_id, ret);
    
        return ret;    
    }
    else
    {
        return FALSE;
    }
}

#if defined(MMIDC_F_DV_ZOOM)
/*****************************************************************************/
//Description :  handle tp for recording zoom button
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleTPForRecordingZoomButton(GUI_POINT_T point)
{
    BOOLEAN  ret = FALSE;

    int16       rect_index = 0;
    GUI_RECT_T  disp_rect = {0};
    MMIDC_ICON_DISP_T disp_list[ICON_RECT_MAX] = {0};
    MMI_MESSAGE_ID_E msg_id = 0;
    
    int16 i = 0;
    int16 total_num = 0;

    total_num = MMIDC_GetIconDesktopRectList(ARR_SIZE(disp_list), disp_list);
    for (i = 0; i < total_num; i++)
    {
        disp_rect = disp_list[i].rect;
        if(GUI_PointIsInRect(point, disp_rect))
        {
            ret = TRUE;
            rect_index = disp_list[i].rect_index;
            
            //SCI_TRACE_LOW:"[MMIDC] HandleTPForIconBar rect_index=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4337_112_2_18_2_9_24_447,(uint8*)"d", rect_index);
            
            switch(rect_index)
            {
            case ICON_RECT_LEFT:
                msg_id = MSG_APP_STAR; //adjust to high  by touch
                break;
            case ICON_RECT_RIGHT:
                msg_id =MSG_APP_HASH; //adjust to low by touch
                break;
            case ICON_RECT_UP:
                msg_id = MSG_APP_UP;
                break;
            case ICON_RECT_DOWN:
                msg_id = MSG_APP_DOWN;
                break;
            case ICON_RECT_CAPTURE:
                msg_id = MSG_APP_C; //for capture
                break;
            case ICON_RECT_RECORDER:
                msg_id = MSG_APP_R; //for recorder
                break;
            case ICON_RECT_SENSOR:
                msg_id = MSG_APP_1;
                break;
            case ICON_RECT_SIZE:
                msg_id = MSG_APP_3;
                break;
            case ICON_RECT_NIGHT:
                msg_id = MSG_APP_4;
                break;
            case ICON_RECT_WB:
                msg_id = MSG_APP_5;
                break;
            case ICON_RECT_DELAY:
                msg_id = MSG_APP_6;
                break;
            case ICON_RECT_STORE:
                msg_id = MSG_APP_7;
                break;
            case ICON_RECT_MULTI:
                msg_id = MSG_APP_8;
                break;
            default:
                ret = FALSE;
                break;
            }

            //if finded rect to handle, then break
            //other continue
            if (ret)
            {
                break;//break for 
            }
        }
    }

    if (ret)
    {
        HandleShortcutKey(msg_id);
    }
    return ret;
}
#endif
#endif //#if defined(MMIDC_DESKTOP_ICON_BAR)

#if defined(MMIDC_DESKTOP_PDA)

/*****************************************************************************/
//Description : delete image
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void DeleteImage(void)
{
#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
    
            MMIDC_DeleteNoSavedFile();
  //          MMIDC_SetCurrentMode(DV_PREVIEW_MODE);
    }
    else
#endif    
    {
        MMIPUB_OpenQueryWinByTextIdEx(SPRD_CAMERA_APPLET_ID,\
                                         TXT_DELETE,\
                                         IMAGE_PUBWIN_QUERY,\
                                         PNULL,\
                                         MMIDC_HandleDeletePubWinBG, \
                                         PNULL
                                         );
    }

    /*MMIDC_ERROR_TYPE_E err_ret = MMIDC_ERR_DELETE_FAIL;

    if(MMIDC_GetCameraMode() == CAMERA_MODE_DC)
    {
        MMIDC_DeletePhoto();
    }
    else
    {
        if(MMIDC_DeleteNoSavedFile())
        {
            err_ret = MMIDC_ERR_DELETE_SUCCESS;
        }
        else
        {
            err_ret = MMIDC_ERR_DELETE_FAIL;
        }
        MMIDC_ShowMsg(err_ret);
    }*/
}

/*****************************************************************************/
//Description : left dc
//Global resource dependence : none
//Author: kunliu
//Note:
/*****************************************************************************/
LOCAL void LSDCSaveAction(void)
{
    BOOLEAN isIstyle = MMITHEME_IsIstyle();

#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        DeleteImage();
    }
    else
#endif    
    {
        if(isIstyle)
        {
            LSSave();
        }
        else
        {
            MMIDC_OpenSendPhotoWin();
        }
    }    
}

/*****************************************************************************/
//Description : rs dc
//Global resource dependence : none
//Author: kunliu
//Note:
/*****************************************************************************/
LOCAL void RSDCSaveAction(void)
{
    BOOLEAN isIstyle = MMITHEME_IsIstyle();

#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        RSSave();
    }
    else
#endif    
    {
        if(isIstyle)
        {
            RSSave();
        }
        else
        {
            DeleteImage();
        }
    }    
}
/*****************************************************************************/
//Description : rs dv
//Global resource dependence : none
//Author: kunliu
//Note:
/*****************************************************************************/
LOCAL void RSDVReviewAction(void)
{
    BOOLEAN isIstyle = MMITHEME_IsIstyle();

#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        DeleteImage();
        MMIDC_SetWorkError(MMIDC_WORK_ERROR_CANCEL_BY_USER);
        MMIDC_PostStateMsg();
    }
    else
#endif    
    {
        if(isIstyle)
        {
            RSDVReview();
        }
        else
        {
            DeleteImage();
        }
    }
}
/*****************************************************************************/
//Description : left soft key dv review action
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void LSDVReviewAction(void)
{
#if defined(MMIDC_F_WORK_MODE)
    if (MMIDC_IsWorkMode())
    {
        MMIDC_PostStateMsg();
    }
    else
#endif    
    {
        MMIDC_OpenSendPhotoWin();
    }

}

/*****************************************************************************/
//Description : dc review by left key
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void LeftDCReview(void)
{
#if 0 //defined(MMIDC_F_EFFECT_360)
    if(MMIDC_GetMultiShootEnum() != MULTI_SHOOT_DISABLE)
    {
        MMIDC_MoveCurReviewPhotoID(MOVE_LEFT);
    }
    MMIDC_ReviewPhotos();
#endif 
}

/*****************************************************************************/
//Description : dc review by right key
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
LOCAL void RightDCReview(void)
{
#if 0 //defined(MMIDC_F_EFFECT_360)
    if(MMIDC_GetMultiShootEnum() != MULTI_SHOOT_DISABLE)
    {
        MMIDC_MoveCurReviewPhotoID(MOVE_LEFT);
    }
    MMIDC_ReviewPhotos();
#endif 
}

const DCKEYFUNCTION s_pda_key_function[PDA_KEY_MAX][DC_FLOW_MODE_MAX] = 
{
    /*DC_PREVIEW     CONT_DOWN     DC_CAPTURE  DC_REVIEW      DC_SAVE                 DV_PREVIEW          DV_RECORD        DV_PAUSE         DV_REVIEW */
    {LeftDCPreview,  PNULL,        PNULL,      LeftDCReview,  LeftSave,               LeftDVPreview,      PNULL,           PNULL,           PNULL},
    {RightDCPreview, PNULL,        PNULL,      RightDCReview, RightSave,              RightDVPreview,     PNULL,           PNULL,           PNULL},
    {UpDCPreview,    PNULL,        PNULL,      PNULL,         UpSave,                 UpDVPreivew,        UpDVZoom,        UpDVZoom,        UpDVReview},
    {DownDCPreview,  PNULL,        PNULL,      PNULL,         DownSave,               DownDVPreview,      DownDVZoom,      DownDVZoom,      DownDVReview},
    {LSDCPreview,    PNULL,        PNULL,      Save,          LSDCSaveAction,         LSDVPreview,        StopRecord,      StopRecord,      LSDVReviewAction},
    {RSDCPreview,    RSCountDown,  PNULL,      RSDCReview,    RSDCSaveAction,         PNULL,              CancelRecord,    CancelRecord,    RSDVReviewAction},
    {OKDCPreview,    PNULL,        PNULL,      OKDCReview,    PNULL,                  OKDVPreview,        PauseOrResume,   PauseOrResume,   OKDVReview},

    {RSDCPreview,    RSCountDown,  PNULL,      RSDCReview,    RSSave,                 RSDVPreview,        CancelRecord,    CancelRecord,    RSDVReview},
    {PNULL,          PNULL,        PNULL,      PNULL,         LSSave,                 PNULL,              PNULL,           PNULL,           PNULL}
};

/*****************************************************************************/
//Description : pda cmd
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
PUBLIC void MMIDC_PDACmd(DC_PDA_KEY_E key)
{
    DC_FLOW_MODE_E  mode = MMIDC_GetCurrentMode();
    
    
    //SCI_TRACE_LOW:"[MMIDC] MMIDC_PDACmd key=%d,cur_mode=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4604_112_2_18_2_9_24_448,(uint8*)"dd", key, mode);
    
    if(key >= PDA_KEY_MAX)
    {    
        //SCI_TRACE_LOW:"[MMIDC]MMIDC_PDACmd key >= PDA_KEY_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4608_112_2_18_2_9_24_449,(uint8*)"");
        return;
    }

    if(!MMIDC_IsReviewing() && !MMIDC_IsCapturing() && !MMIDC_IsSettingParamCtrl())
    {
        if(PNULL != s_pda_key_function[key][mode])
        {
            s_pda_key_function[key][mode]();
        }
    }
    
}

/*****************************************************************************/
//Description : back to preview by cancel key
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
PUBLIC void MMIDC_BackToPreview(void)
{
    BackToPreview();
}

/*****************************************************************************/
//Description : save photo 
//Global resource dependence : none
//Author: robert.wang
//Note:
/*****************************************************************************/
PUBLIC void MMIDC_Saving(void)
{
    Save();
}
#endif

/*****************************************************************************/
//Description : register dc module applet info
//Global resource dependence : 
//Author:robert.wang
//Note:
/*****************************************************************************/
LOCAL void RegDCAppletInfo(void)
{
    MMI_RegAppletInfo(MMI_MODULE_CAMERA, &g_mmidc_applet_info);
}

/*****************************************************************************/
//Description : the process message function of the camera
//Global resource dependence : 
//Author:robert.wang
//Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E  DCApplet_HandleEvent(    
                                            IAPPLET_T*          iapplet_ptr, //in
                                            MMI_MESSAGE_ID_E    msg_id, //in
                                            DPARAM              param //in
                                            )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)iapplet_ptr;

    //SCI_TRACE_LOW:"[MMIDC] DCApplet_HandleEvent msg_id = 0x%04x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4663_112_2_18_2_9_24_450,(uint8*)"d",msg_id);
    
    switch (msg_id)
    {
    case MSG_APPLET_START:
        {
            MMI_WINDOW_TABLE_CREATE_T win_table = {0};

            win_table.applet_handle = applet_ptr->caf_applet.app_handle;
            win_table.win_table_ptr = MMIDC_DC_WAIT_TAB;
            win_table.add_data_ptr = param;
            
            MMK_CreateWinTable( &win_table );

            //SCI_TRACE_LOW:"[MMIDC] DCApplet_HandleEvent MSG_APPLET_START"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4677_112_2_18_2_9_24_451,(uint8*)"");
        }
        break;
        
    case MSG_APPLET_STOP:
        //SCI_TRACE_LOW:"[MMIDC] DCApplet_HandleEvent MSG_APPLET_STOP"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4682_112_2_18_2_9_24_452,(uint8*)"");

        if (MMIDC_GetDCModuleRunningFlag())
        {
            ReleaseDCModule();
        }
        
#ifdef UI_P3D_SUPPORT
        MMI_Enable3DEffect(MMI_3D_EFFECT_CAMERA);
#endif        
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return result;
}

/*****************************************************************************/
//Description : close dc applet
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
LOCAL void CloseDCApplet(void)
{
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr( SPRD_CAMERA_APPLET_ID );
    
    if( PNULL != applet_ptr )
    {
        MMK_CloseApplet(applet_ptr->caf_applet.app_handle);
    }

    //SCI_TRACE_LOW:"[MMIDC] CloseDCApplet applet_ptr=0x%lX"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4716_112_2_18_2_9_24_453,(uint8*)"d",applet_ptr);
}

/*****************************************************************************/
//Description : start dc applet
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
LOCAL BOOLEAN StartDCApplet(CAMERA_MODE_E mode
                                )
{
    BOOLEAN ret = FALSE;
    uint32 applet_id = SPRD_CAMERA_APPLET_ID;
    MMI_HANDLE_T applet_handle = 0;
    
    MMI_APPLET_START_T start ={0};
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr(applet_id);
    
    if (PNULL == applet_ptr)
    {
        start.guid = applet_id;
        
        start.param_ptr = (void*)mode;
        start.size_of_param = sizeof(mode);
        
        //free idle buffer
        MMIPUB_OpenTempWaitWin();
        
        applet_handle = MMK_StartApplet( &start );
        if (PNULL != applet_handle)
        {
            ret = TRUE;
        }
    }
    else
    {
        ret = TRUE;
        
        MMK_GrabWinFocusByApplet(applet_id);
    }

    //SCI_TRACE_LOW:"[MMIDC] StartDCApplet ret = %d,applet_ptr=0x%lX"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4755_112_2_18_2_9_25_454,(uint8*)"dd",ret, applet_ptr);

    return ret;
}

/*****************************************************************************/
//Description : create win by applet
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
PUBLIC void MMIDC_CreateWinByApplet(uint32* win_table_ptr, //in
                                             ADD_DATA add_data_ptr //in
                                             )
{
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr( SPRD_CAMERA_APPLET_ID );
    MMI_WINDOW_TABLE_CREATE_T win_table = {0};


    //SCI_TRACE_LOW:"[MMIDC] MMIDC_CreateWinByApplet applet_ptr=0x%lX"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4773_112_2_18_2_9_25_455,(uint8*)"d",applet_ptr);

    if (PNULL != applet_ptr)
    {
        win_table.applet_handle = applet_ptr->caf_applet.app_handle;
        win_table.win_table_ptr = win_table_ptr;
        win_table.add_data_ptr = add_data_ptr;
        
        MMK_CreateWinTable( &win_table );
    }
}

/*****************************************************************************/
//Description : get first preview flag
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDC_GetFirstPreviewFlag(void)
{
    BOOLEAN ret = FALSE;
    
#if defined(MMIDC_F_PREVIEW_ANIM)    
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr( SPRD_CAMERA_APPLET_ID );

    if (PNULL != applet_ptr)
    {
        ret = applet_ptr->is_first_preview;
    }
#endif

    //SCI_TRACE_LOW:"[MMIDC] MMIDC_GetFirstPreview ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4803_112_2_18_2_9_25_456,(uint8*)"d",ret);

    return ret;
}

/*****************************************************************************/
//Description : set first preview flag
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
PUBLIC void MMIDC_SetFirstPreviewFlag(BOOLEAN is_first_preview)
{
#if defined(MMIDC_F_PREVIEW_ANIM)
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr( SPRD_CAMERA_APPLET_ID );

    if (PNULL != applet_ptr)
    {
        applet_ptr->is_first_preview = is_first_preview;
    }

    //SCI_TRACE_LOW:"[MMIDC] MMIDC_SetFirstPreview is_first_preview=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4823_112_2_18_2_9_25_457,(uint8*)"d",is_first_preview);
#endif         
}

/*****************************************************************************/
//Description : get dc running flag
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDC_GetDCModuleRunningFlag(void)
{
    BOOLEAN ret = FALSE;
    
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr( SPRD_CAMERA_APPLET_ID );

    if (PNULL != applet_ptr)
    {
        ret = applet_ptr->is_running;
    }

    //SCI_TRACE_LOW:"[MMIDC] MMIDC_GetDCModuleRunningFlag ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4843_112_2_18_2_9_25_458,(uint8*)"d",ret);

    return ret;
}

/*****************************************************************************/
//Description : set dc running flag
//Global resource dependence :none
//Author:robert.wang
//Note: 
/*****************************************************************************/
PUBLIC void MMIDC_SetDCModuleRunningFlag(BOOLEAN is_running)
{
    MMIDC_APPLET_T* applet_ptr = (MMIDC_APPLET_T*)MMK_GetAppletPtr( SPRD_CAMERA_APPLET_ID );

    if (PNULL != applet_ptr)
    {
        applet_ptr->is_running = is_running;
    }

    //SCI_TRACE_LOW:"[MMIDC] MMIDC_SetDCModuleRunningFlag is_running=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4862_112_2_18_2_9_25_459,(uint8*)"d",is_running);
}

/*****************************************************************************/
//  Description : API of create audio file handle
//  Global resource dependence : none
//  Author: 
//  Note: SUCCESS - return the audio handle created
//        FAILURE - return SCI_NULL
/*****************************************************************************/
PUBLIC MMISRV_HANDLE_T MMIDC_RequestHandle(void)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T        audio_srv = {0};

    req.pri = MMISRVAUD_PRI_NORMAL;
    audio_srv.volume = MMISET_DC_DEFAULT_VOL;
    audio_srv.info.type = MMISRVAUD_TYPE_VIRTUAL;
    
    if(audio_srv.volume > MMISET_VOL_MAX)
    {
        audio_srv.volume = MMISET_VOL_MAX;
    }
    
    return MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
}

/*****************************************************************************/
//  Description : Get current audio handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
PUBLIC MMISRV_HANDLE_T MMIDC_GetAudioHandle(void)
{
    return s_dc_audio_handle;
}

/*****************************************************************************/
//  Description : Set current audio handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
PUBLIC MMISRV_HANDLE_T MMIDC_SetAudioHandle(MMISRV_HANDLE_T handle)
{
    return s_dc_audio_handle = handle;
}
/*****************************************************************************/
//Description : get capture status by user
//Global resource dependence :none
//Author:robert.wang
//Note: 
/****************************************************************************/
PUBLIC BOOLEAN MMIDC_GetCaptureStatusByUser(void)
{
    return s_is_back_to_preview;
}

/*****************************************************************************/
//Description : process msg cmd
//Global resource dependence :none
//Author:robert.wang
//Note: 
/****************************************************************************/
PUBLIC int32 MMIDC_ProcessMsgCmd(MMIDC_CMD_E cmd)
{
    int32 ret = 0;

    //SCI_TRACE_LOW:"[MMIDC] MMIDC_ProcessMsgCmd cmd=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIDC_MAIN_WINTAB_4921_112_2_18_2_9_25_460,(uint8*)"d", cmd);

    switch(cmd)
    {
    case MMIDC_CMD_DISP_PREVIEW_FRAME:
        MMIDC_DisplayPreviewFrame();
        break;
    #if defined(MMIDC_F_U_CAMERA)
    case MMIDC_CMD_IMAGE_PROCESS_DONE:
        if (MMIDC_IsUCameraEnable())
        {
            MMIDC_UCameraASyncCallbackToSavePhotoFile();
        }
        break;
    #endif
    case MMIDC_CMD_CREATE_FILE_HANDLE:
        MMIDC_CreateAllFileHandle();
        break;
    default:
        break;
    }

    return ret;
}

/*****************************************************************************/
//  Description : post create file handle msg
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
PUBLIC void MMIDC_PostCreateFileHandleMsg(void)
{
    MMIDC_CMD_E cmd = 0;


    MMIDC_CloseAllFileHandle();
    
    cmd = MMIDC_CMD_CREATE_FILE_HANDLE;    
    MMK_PostMsg(MMIDC_MAIN_WIN_ID, MSG_DC_MMI_CMD, &cmd, sizeof(cmd));

    SCI_TRACE_LOW("[MMIDC] MMIDC_PostCreateFileHandleMsg");
}

/*****************************************************************************/
// 	Description : init mmidcsrv info
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL void MMIDCSRV_InitInfo(void)
{
    SCI_MEMSET(&s_mmidcsrv_info, 0 ,sizeof(s_mmidcsrv_info));
}

/*****************************************************************************/
// 	Description : get mmidcsrv info ptr
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
LOCAL MMIDCSRV_INFO_T *GetMMIDCSRVInfoPtr(void)
{
    return &s_mmidcsrv_info;
}

/*****************************************************************************/
// 	Description : get mmidcsrv dc mem cfg
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC MMIDCSRV_MEM_CFG_E MMIDCSRV_GetDCMemCfg(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    MMIDCSRV_MEM_CFG_E mem_cfg = 0;

    
    if (PNULL != info_ptr)
    {
        mem_cfg = info_ptr->work_param.dc.mem_cfg;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_GetDCMemCfg mem_cfg=%d",
                mem_cfg);
                
    return mem_cfg;
}

/*****************************************************************************/
// 	Description : get mmidcsrv dv mem cfg
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC MMIDCSRV_MEM_CFG_E MMIDCSRV_GetDVMemCfg(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    MMIDCSRV_MEM_CFG_E mem_cfg = 0;

    
    if (PNULL != info_ptr)
    {
        mem_cfg = info_ptr->work_param.dv.mem_cfg;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_GetDVMemCfg mem_cfg=%d",
                mem_cfg);
    
    return mem_cfg;
}

/*****************************************************************************/
// 	Description : get mmidcsrv work mode
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC MMIDCSRV_MODE_E MMIDCSRV_GetWorkMode(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    MMIDCSRV_MODE_E work_mode = 0;

    
    if (PNULL != info_ptr)
    {
        work_mode = info_ptr->work_param.work_mode;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_GetWorkMode work_mode=%d",
                work_mode);
    
    return work_mode;
}

/*****************************************************************************/
//  Description : get work video size width and height
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL void GetMMIDCSRVVideoSizeWidthAndHeight(MMIDC_WORK_VIDEO_SIZE_E video_size, //in
                                                            MMIDC_SIZE_ATTRIBUTE_T *size_ptr //out
                                                            )
{
    MMIDC_SIZE_ATTRIBUTE_T size_array[] =
    {
        {128, 96},//WORK_VIDEO_128_96,/*128x96 or 96x128*/
        {160, 128},//WORK_VIDEO_160_128,/*160x128 or 128x160*/
        {176, 144},//WORK_VIDEO_176_144,
        {320, 240}//WORK_VIDEO_320X240,/*320x240 or 240x320*/
    };
    int32 total_num = ARR_SIZE(size_array);

    
    if ((PNULL != size_ptr)
        && (video_size < WORK_VIDEO_SIZE_MAX)
        && (total_num <= WORK_VIDEO_SIZE_MAX) /*lint !e774*/
       )
    {
        SCI_MEMCPY(size_ptr, &size_array[video_size], sizeof(*size_ptr));
    }
}

/*****************************************************************************/
//  Description : convert work video size to dv video size
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
LOCAL VIDEO_SIZE_E ConvertMMIDCSRVVideoSizeToDCVideoSize(MMIDC_WORK_VIDEO_SIZE_E work_video_size)
{
    VIDEO_SIZE_E size_array[VIDEO_SIZE_MAX] = {0};
    int32 total_item = 0; 
    int32 i = 0;

    
    MMIDC_SIZE_ATTRIBUTE_T video_size_val = {0};
    MMIDC_SIZE_ATTRIBUTE_T work_size_val = {0};
    VIDEO_SIZE_E video_size = VIDEO_SIZE_MAX;
    VIDEO_SIZE_E support_size = 0;
    MMIDC_WORK_VIDEO_SIZE_E work_size = 0;

    
    do
    {
        work_size = work_video_size;
        
        //check work screen size
        if (work_size < WORK_VIDEO_SIZE_MIN)
        {
            work_size = WORK_VIDEO_SIZE_MIN;
        }

        //check work max size
        if (WORK_VIDEO_SIZE_MAX < work_size)
        {
            work_size = WORK_VIDEO_SIZE_MAX;
        }

        total_item = MMIDC_GetSupportVideoSizeArray(ARR_SIZE(size_array),size_array);

        //check other work size
        GetMMIDCSRVVideoSizeWidthAndHeight(work_size, &work_size_val);
        for (i = total_item - 1; i >= 0; i--)
        {
            support_size = size_array[i];
            if (support_size < VIDEO_SIZE_MAX)
            {
                //find work photo size val to equal dc photo size val
                //if true, get dc photo size by work photo size
                MMIDC_GetVideoSizeWidthAndHeight(support_size, &video_size_val);
                if ((work_size_val.size_width == video_size_val.size_width)
                    && (work_size_val.size_height == video_size_val.size_height)
                   )
                {
                    video_size = support_size;
                    break;
                }

                if ((work_size_val.size_height == video_size_val.size_width)
                    && (work_size_val.size_width == video_size_val.size_height)
                   )
                {
                    video_size = support_size;
                    break;
                }
            }
        }

    }while(0);
    

    SCI_TRACE_LOW("[MMIDC] ConvertMMIDCSRVVideoSizeToDCVideoSize video_size=%d",\
                  video_size);
    
    return video_size;
}

/*****************************************************************************/
//  Description : check video size
//  Global resource dependence : none
//  Author: robert.wang
//  Note:
/*****************************************************************************/
PUBLIC VIDEO_SIZE_E MMIDCSRV_CheckVideoSize(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();

    MMIDC_WORK_VIDEO_SIZE_E work_video_size = 0;
    VIDEO_SIZE_E video_size = 0;


    if (PNULL != info_ptr)
    {
        work_video_size = info_ptr->work_param.dv.max_video_size;        
        video_size = ConvertMMIDCSRVVideoSizeToDCVideoSize(work_video_size);

        info_ptr->dv_info.max_video_size = video_size;
    }
    
    SCI_TRACE_LOW("[MMIDC] MMIDCSRV_CheckVideoSize video_size=%d",\
                 video_size);
    
    return video_size;
}

/*****************************************************************************/
// 	Description : mmidcsrv open dc main window
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIDCSRV_OpenMainWin(MMIDCSRV_WORK_PARAM_T *work_param_ptr /*in*/)
{
    BOOLEAN ret = FALSE;
    CAMERA_MODE_E mode = 0;
    MMIDCSRV_MODE_E work_mode = 0;

    if(MMIAPIDC_IsOpened())
    {
        MMIAPIDC_Exit();
    }
    
    ret = CheckDCRunningEnvironment();
    if (!ret)
    {
        return ret;
    }
    
#ifdef UI_P3D_SUPPORT
    MMI_Disable3DEffect(MMI_3D_EFFECT_CAMERA);
#endif   

    UILAYER_SetMainLayerMemFormat(UILAYER_MEM_NONE);

    MMIDCSRV_InitInfo();
    s_mmidcsrv_info.work_param = *work_param_ptr;


#if defined(MMIDC_F_U_CAMERA)
    {
        BOOLEAN is_ucamera = FALSE;

        if (MMIDCSRV_MEM_DC_UCAMERA == MMIDCSRV_GetDCMemCfg())
        {
            is_ucamera = TRUE;
        }
        
        MMIDC_SetUCameraModuleFlag(is_ucamera);
    }
#endif

    
    work_mode = MMIDCSRV_GetWorkMode();
    switch(work_mode)
    {
    case MMIDCSRV_MODE_DC:
        mode = CAMERA_MODE_DC;
        break;
    case MMIDCSRV_MODE_DV:
        mode = CAMERA_MODE_DV;
        break;
    default:
        ret = FALSE;
        break;
    }   

    if (ret)
    {
        ret = StartDCApplet(mode);
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIAPIDCSRV_OpenMainWin ret=%d",
                 ret);
    
    return ret;
}

/*****************************************************************************/
// 	Description : get mmidcsrv max video size
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC VIDEO_SIZE_E MMIDCSRV_GetMaxVideoSizee(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    VIDEO_SIZE_E video_size = 0;

    
    if (PNULL != info_ptr)
    {
        video_size = info_ptr->dv_info.max_video_size;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_GetMaxVideoSizee video_size=%d",
                video_size);
    
    return video_size;
}

/*****************************************************************************/
// 	Description : check mmidcsrv dc display mode
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC DCAMERA_DISP_MODE_E MMIDCSRV_CheckDCDisplayMode(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    DCAMERA_DISP_MODE_E disp_mode = 0;

    
    if (PNULL != info_ptr)
    {
        disp_mode = info_ptr->work_param.dc.disp_mode;
        if (DCAMERA_DISP_MODE_MAX <= disp_mode)
        {
            disp_mode = MMIDC_GetPhotoDisplayMode();
        }
#if defined(MMIDC_F_WORK_MODE)
        if (MMIDC_IsWorkMode())
        {
            if((MMIDC_GetPhotoSize() == MMIDC_GePhotoSizetVerLcd())
                || (MMIDC_GetPhotoSize() == MMIDC_GePhotoSizetHorLcd()))
            {

                if((PHOTO_SIZE_320X240 == MMIDC_GetPhotoSize()) 
                  && (LCD_ANGLE_0 == MMIAPIDC_GetCameraSensorAngle()))
                {
                    //nothing
                }
                else
                {
                    disp_mode = DCAMERA_DISP_FULLSCREEN;
                }
            }
        }
#endif
        info_ptr->dc_info.disp_mode = disp_mode;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_CheckDCDisplayMode disp_mode=%d",
                disp_mode);
    
    return disp_mode;
}

/*****************************************************************************/
// 	Description : get mmidcsrv dc display mode
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC DCAMERA_DISP_MODE_E MMIDCSRV_GetDCDisplayMode(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    DCAMERA_DISP_MODE_E disp_mode = 0;

    
    if (PNULL != info_ptr)
    {
        disp_mode = info_ptr->dc_info.disp_mode;
        
    #if defined(MMIDC_F_U_CAMERA)
        if (MMIDC_GetUCameraModuleFlag())
        {
            disp_mode = DCAMERA_DISP_FULLVIEW;
        }
    #endif
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_GetDCDisplayMode disp_mode=%d",
                disp_mode);
    
    return disp_mode;
}

/*****************************************************************************/
// 	Description : check mmidcsrv dv display mode
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC DRECORDER_DISP_MODE_E MMIDCSRV_CheckDVDisplayMode(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    DRECORDER_DISP_MODE_E disp_mode = 0;

    
    if (PNULL != info_ptr)
    {
        disp_mode = info_ptr->work_param.dv.disp_mode;
        if (DCAMERA_DISP_MODE_MAX <= disp_mode)
        {
            disp_mode = (DRECORDER_DISP_MODE_E)MMIDC_GetDefaultVisualMode();
        }
        info_ptr->dv_info.disp_mode = disp_mode;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_CheckDVDisplayMode disp_mode=%d",
                disp_mode);
    
    return disp_mode;
}

/*****************************************************************************/
// 	Description : get mmidcsrv dv display mode
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC DRECORDER_DISP_MODE_E MMIDCSRV_GetDVDisplayMode(void)
{
    MMIDCSRV_INFO_T *info_ptr = GetMMIDCSRVInfoPtr();
    DRECORDER_DISP_MODE_E disp_mode = 0;

    
    if (PNULL != info_ptr)
    {
        disp_mode = info_ptr->dv_info.disp_mode;
    }

    SCI_TRACE_LOW("[MMIDCSRV] MMIDCSRV_GetDVDisplayMode disp_mode=%d",
                disp_mode);
    
    return disp_mode;
}

/*****************************************************************************/
// 	Description : check mmidcsrv param
//	Global resource dependence : none
//  Author: robert.wang
//	Note:
/*****************************************************************************/
PUBLIC void MMIDCSRV_CheckParam(void)
{
    MMIDCSRV_CheckDCDisplayMode();
    MMIDCSRV_CheckDVDisplayMode();
}
#endif  //#ifdef  CAMERA_SUPPORT

