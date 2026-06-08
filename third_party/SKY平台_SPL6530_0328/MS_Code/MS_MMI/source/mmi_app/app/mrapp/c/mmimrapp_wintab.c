#include "mmi_app_mrapp_trc.h"
#ifdef MRAPP_SUPPORT
/****************************************************************************
** File Name:      mmimrapp_wintab.c                            
** Author:              qgp                                                    
** Date:            2006/05/15
** Copyright:      2004 Spreatrum, Incoporated. All Rights Reserved.       
** Description:    This file is used to create windows of mr app
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE               NAME             DESCRIPTION                             
** 04/01/2006     	qgp	         Create
** 
****************************************************************************/
#ifndef _MMIMRAPP_WINTAB_C_
#define _MMIMRAPP_WINTAB_C_

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/

#include "window_parse.h"
#include "Mmi_text.h"
#include "mmk_app.h"
#include "mmk_type.h"
#include "mmk_timer.h"
#include "guimsgbox.h"
#include "guicommon.h"
#include "guitext.h"
#include "guilistbox.h"
#include "guilabel.h"
#include "guiedit.h"
#include "guilistbox.h"
#include "guilcd.h"
#include "mmidisplay_data.h"
#include "tb_dal.h"
#include "mmiacc_menutable.h"
#include "mmipub.h"
#include "mmialarm_internal.h"
#include "mmialarm_export.h"
#include "mmi_textfun.h"
#include "mmiphone_export.h"
#include "mmiset_export.h"
#include "tb_hal.h"
#include "mmi_image.h"
#include "Mmimrapp_export.h"
#include "mmimrapp_internal.h"
#include "mmimrapp_id.h"
#include "window_parse.h"
#ifdef M_NEW_VER
#ifdef PORT_BASE_NILE
#include "freq_cfg.h"
#else
#include "sc6600m_freq.h"
#endif
#else
#include "chip.h"
#endif
#include "mmi_appmsg.h"
#include "mmipb_common.h"
#include "mmipb_interface.h"
#include "Mmipb_export.h"
#include "mmi_common.h"
//#include "im_id.h"
#include "mmistk_app.h"
#include "mmifilearray_export.h"
#include "mmiconnection_export.h"
#include "mmiconnection_text.h"

#ifdef __MMI_SKYQQ__
#include "sky_qq_open.h"
#endif

#ifdef __MR_CFG_CAMERA_SUPPORT__
#include "dal_dcamera.h"
#include "dcamera_misc.h"
#include "dcamera_common.h"
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                       LOCAL DATA DECLARATION
 **---------------------------------------------------------------------------*/
 
LOCAL uint8		s_mrTimer = 0;
LOCAL uint8		s_mr_vibra_timer = 0;
LOCAL uint8		s_mr_gprsProtectTimer = 0;
LOCAL uint8		s_mr_prompt_timer = 0;
LOCAL uint8		s_mr_mrPlaySound_timer = 0;//2008-1-2 add for mr_playSound() protect broken off by other player,as sms .
LOCAL uint8		s_mr_gprsRetryTimer = 0;//2008-2-25 add fro mocor
LOCAL uint8		s_mr_AccessUrl_timer = 0;
//2009-7-7 add begin
LOCAL uint8		s_mr_getApnAccount_timer = 0;
LOCAL uint8		s_mr_setApnAccount_timer = 0;
//2009-7-7 add end

/* begin:added by Tommy.yan 20120120 */
LOCAL uint8     s_mr_deleteSMS_timer = 0;
LOCAL uint8		s_mr_getSMSContent_timer = 0;
LOCAL uint8		s_mr_getSMSCapacity_timer = 0;
LOCAL uint8     s_mr_setSMSState_timer = 0;
/* end:added by Tommy.yan 20120120 */

LOCAL uint8     s_mr_getSCNumber_timer = 0;
LOCAL uint8     s_mr_statusIcon_timer  = 0;
LOCAL char*	    s_mr_AccessUrl_url = 0;
LOCAL uint32	s_mr_gsensor_flag  = 0;
LOCAL BOOLEAN	s_mr_vmPaused = FALSE;
LOCAL BOOLEAN	s_mr_keyDown = FALSE;
LOCAL MRAPP_CPUCLK_MNG_T s_mr_cpuClk_mng;

LOCAL MMIMRAPP_DIALOG_PARAMETERS_T *s_mr_dialogWinList = NULL;

LOCAL uint8 s_mr_winTab[MRAPP_DYN_WIN_ID_MAXNUM]  = {0};
LOCAL uint8 s_mr_ctrlTab[MRAPP_DYN_WIN_ID_MAXNUM] = {0};


LOCAL const int32 s_KeyTable[] = {
MR_KEY_NONE,    //#define KEY_NONE				0x00				/*!< press none */
MR_KEY_UP,      //#define KEY_UP				0x01				/*!< press [up] */
MR_KEY_DOWN,    //#define KEY_DOWN				0x02				/*!< press [down] */
MR_KEY_LEFT,    //#define KEY_LEFT				0x03				/*!< press [left] */
MR_KEY_RIGHT,   //#define KEY_RIGHT				0x04				/*!< press [right] */
MR_KEY_SEND,    //#define KEY_GREEN				0x05				/*!< press [Green] */
MR_KEY_CAPTURE,//#define KEY_CAMREA				0x06				/*!< press [CAMERA] */
MR_KEY_NONE,    //0x0007 NO define
MR_KEY_1,       //#define KEY_1					0x08				/*!< press [1] */
MR_KEY_2,       //#define KEY_2					0x09				/*!< press [2] */
MR_KEY_3,       //#define KEY_3					0x0a				/*!< press [3] */
MR_KEY_4,       //#define KEY_4					0x0b				/*!< press [4] */
MR_KEY_5,       //#define KEY_5					0x0c				/*!< press [5] */
MR_KEY_6,       //#define KEY_6					0x0d				/*!< press [6] */
MR_KEY_7,       //#define KEY_7					0x0e				/*!< press [7] */
MR_KEY_8,       //#define KEY_8					0x0f				/*!< press [8] */
MR_KEY_9,       //#define KEY_9					0x10				/*!< press [9] */
MR_KEY_STAR,    //#define KEY_STAR				0x11				/*!< press [*] */
MR_KEY_0,       //#define KEY_0					0x12				/*!< press [0] */
MR_KEY_POUND,   //#define KEY_HASH				0x13				/*!< press [#] */
MR_KEY_NONE,    //#define KEY_SPUP              0x14
MR_KEY_NONE,    //#define KEY_SPDW              0x15
MR_KEY_SOFTLEFT,//#define KEY_OK				0x16				/*!< press left softkey(OK) */
MR_KEY_SOFTRIGHT,//#define KEY_CANCEL			0x17				/*!< press right softkey (Cancel) */
MR_KEY_POWER,     //#define KEY_RED				0x18				/*!< press red key */
MR_KEY_VOLUME_UP,//#define KEY_UPSIDEKEY		0x19				/*!< press up side key */
MR_KEY_VOLUME_DOWN,//#define KEY_DOWNSIDEKEY	0x1a				/*!< press down side key */
MR_KEY_SELECT,      //#define KEY_WEB			0x1b				/*!< press web key */

MR_KEY_NONE,  //#define KEY_GPIO_SIG1        	0x1c
MR_KEY_NONE,  //#define KEY_GPIO_SIG2        	0x1d
MR_KEY_NONE,  //#define KEY_FLIP				0x1e				/*!< flip gpio  */
MR_KEY_NONE,  //#define KEY_HEADSET_BUTTIN		0x1f				/*!< handset gpio buttion */
MR_KEY_NONE,  //#define KEY_HEADSET_DETECT		0x20				/*!< handset gpio detect */
MR_KEY_NONE,  //#define KEY_SDCARD_DETECT		0x21				/*!< SD card detct */
MR_KEY_NONE,  //#define KEY_VIDEO_TEL    	    0x22
MR_KEY_NONE,  //#define KEY_PLAYANDSTOP    	    0x23
MR_KEY_NONE,  //#define KEY_FORWARD    		    0x24
MR_KEY_NONE,  //#define KEY_BACKWARD    		0x25
MR_KEY_NONE,  //#define KEY_MIDDLE              0x26

MR_KEY_NONE,     //#define KEY_AT       0x27
MR_KEY_QWERTY_Q, //#define KEY_Q        0x28
MR_KEY_QWERTY_W, //#define KEY_W		0x29  
MR_KEY_QWERTY_E, //#define KEY_E		0x2a  
MR_KEY_QWERTY_R, //#define KEY_R		0x2b  
MR_KEY_QWERTY_T, //#define KEY_T		0x2c  
MR_KEY_QWERTY_Y, //#define KEY_Y		0x2d  
MR_KEY_QWERTY_U, //#define KEY_U		0x2e  
MR_KEY_QWERTY_I, //#define KEY_I		0x2f  
MR_KEY_QWERTY_O, //#define KEY_O		0x30  
MR_KEY_QWERTY_P, //#define KEY_P		0x31  
MR_KEY_QWERTY_A, //#define KEY_A		0x32  
MR_KEY_QWERTY_S, //#define KEY_S		0x33  
MR_KEY_QWERTY_D, //#define KEY_D		0x34  
MR_KEY_QWERTY_F, //#define KEY_F		0x35  
MR_KEY_QWERTY_G, //#define KEY_G		0x36  
MR_KEY_QWERTY_H, //#define KEY_H		0x37  
MR_KEY_QWERTY_J, //#define KEY_J		0x38  
MR_KEY_QWERTY_K, //#define KEY_K		0x39  
MR_KEY_QWERTY_L, //#define KEY_L		0x3a  
MR_KEY_QWERTY_DEL, //#define KEY_DEL		0x3b  
MR_KEY_QWERTY_Z, //#define KEY_Z		0x3c  
MR_KEY_QWERTY_X, //#define KEY_X		0x3d  
MR_KEY_QWERTY_C, //#define KEY_C		0x3e  
MR_KEY_QWERTY_V, //#define KEY_V		0x3f  
MR_KEY_QWERTY_B, //#define KEY_B		0x40  
MR_KEY_QWERTY_N, //#define KEY_N		0x41  
MR_KEY_QWERTY_M, //#define KEY_M		0x42  
MR_KEY_NONE,     //#define KEY_COMMA	0x43
MR_KEY_NONE,     //#define KEY_PERIOD	0x44
MR_KEY_QWERTY_ENTER, //#define KEY_ENTER	 0x45
MR_KEY_QWERTY_FN,    //#define KEY_FN		 0x46
MR_KEY_QWERTY_SHIFT, //#define KEY_SHIFT	 0x47
MR_KEY_NONE,         //#define KEY_AT_QWERTY 0x48
MR_KEY_QWERTY_SPACE, //#define KEY_SPACE	 0x49
MR_KEY_NONE,         //#define KEY_AND		 0x4a
MR_KEY_NONE,         //#define KEY_QUESTION	 0x4b
MR_KEY_QWERTY_CTRL,  //#define KEY_CTRL	     0x4c

MR_KEY_NONE, //#define KEY_PLUS                0x4d  // +
MR_KEY_NONE, //#define KEY_LEFT_PARENTHESIS    0x4e  // (
MR_KEY_NONE, //#define KEY_RIGHT_PARENTHESIS   0x4f  // )
MR_KEY_NONE, //#define KEY_MINUS               0x50  // -
MR_KEY_NONE, //#define KEY_DOUBLE_QUOTES       0x51  // "
MR_KEY_NONE, //#define KEY_SEMICOLON           0x52  // ;
MR_KEY_NONE, //#define KEY_COLON               0x53  // : 
MR_KEY_NONE, //#define KEY_SLASH               0x54  // /
MR_KEY_NONE, //#define KEY_SHIFT_AND           0x55  // &
MR_KEY_NONE, //#define KEY_EXCLAMATION         0x56  // !

MR_KEY_NONE, //#define KEY_SLIDE               0x57  
MR_KEY_NONE, //#define KEY_TV			       0x58  
MR_KEY_NONE, //#define KEY_CALL2			   0x59  
MR_KEY_NONE, //#define KEY_CALL3               0x5a  
MR_KEY_NONE, //#define KEY_MP3                 0x5b  
MR_KEY_NONE, //#define KEY_NOTES               0x5c  
MR_KEY_NONE, //#define KEY_SMS                 0x5d  
MR_KEY_NONE, //#define KEY_CALENDER            0x5e  
MR_KEY_NONE, //#define KEY_IE                  0x5f  
MR_KEY_NONE, //KEY_HANG                        0x60  

MR_KEY_VOLUME_UP,   //#define KEY_VOL_UP       0x61  
MR_KEY_VOLUME_DOWN, //#define KEY_VOL_DOWN     0x62  
MR_KEY_NONE, //#define KEY_MO                  0x63  
MR_KEY_NONE, //#define KEY_HOOK                0x64  
MR_KEY_NONE, //#define KEY_HANDFREE            0x65  
MR_KEY_NONE, //#define KEY_CALL4               0x66  
MR_KEY_NONE, //#define KEY_POWER               0x67  
MR_KEY_SOFTLEFT, //#define KEY_MENU            0x68  

//end at//#define    MSG_KEYLONG              		0x0030              /*!< LAST_KEYUP_MSG_ID + 0x01 */
};


/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/
BOOLEAN s_mr_inMrSelfWinCreating = FALSE;
BOOLEAN s_mr_PBOfCancelMsg = FALSE;
BOOLEAN s_mr_isWindowOfPB = FALSE;
BOOLEAN s_mr_isReadBySMS = FALSE;
uint8 s_mr_fbhup2idle = 0;
BOOLEAN	s_mr_allReady = FALSE;//mmi res ok
BOOLEAN	s_mr_VMReady = FALSE;//vm res ok
#ifdef MR_BINDTO_VIRTUAL_WIN
uint32 g_mr_vmBackRun = MRAPP_MAX;
#endif
uint8 g_mr_filebrowser_path[256] = {0};

LOCAL uint8     s_mr_phb_operate_timer = 0;
LOCAL uint8     s_mr_phb_search_timer=0;
PUBLIC void MMIMRAPP_StopPHBOperateTimer(void);
void MMIMRAPP_PHBSearchCallBack();

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DECLARE                                 *
 **--------------------------------------------------------------------------*/
extern mr_screen_angle_t g_screen_angle;
extern MMI_APPLICATION_T g_main_app;
extern MRAPP_PBINFO_T* g_mr_pbinfo;
extern uint8 g_mr_curUserApnAccuntId; /*lint !e752*/

extern uint32 MMI_SetFreq(PROD_FREQ_INDEX_E freq); /*lint !e762*/
extern uint32 MMI_RestoreFreq(void); /*lint !e762*/

void MMIMRAPP_AccessUrl(void);

/*wuwenjie 2011-12-01 START*/
extern uint8 mr_getHostTimerId(void);
extern void mr_gethostTimerDeal(void);
/*wuwenjie 2011-12-01 END*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/


/*****************************************************************************/
// 	Description : init local resource
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL  void MMIMRAPP_initLocalRes(void);

/*****************************************************************************/
// 	Description : get pb info back
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL void MMIMRAPP_pbInfoReturn(MMIPB_HANDLE_T handle);


/*****************************************************************************/
// 	Description : process the key event
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E ProcessMrAppKeyMsg(
									  MMIMRAPP_KEYTYPE_E kType,
									  void *info
									  );

/*****************************************************************************/
// 	Description :动态窗口消息处理函数
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL MMI_RESULT_E  MMIMRAPP_HandleDynWinMsg(
											 MMIMRAPP_WINDOW_ID_E win_id, 
											 MMI_MESSAGE_ID_E msg_id, 
											 DPARAM param
											 );

/*****************************************************************************/
// 	Description : 释放窗体相关的用户数据
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL void MMIMRAPP_freeDynWinData(MMIMRAPP_ADD_PARAMETERS_T* addData);

/*****************************************************************************/
// 	Description : 获取一空闲的 ID,或释放一id
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL int32 MMIMRAPP_OpDynId(MMIMRAPP_DYN_WIN_TYPE_E winType, int32 opId, MMIMRAPP_ID_TYPE_E idType);

/*****************************************************************************/
// 	Description : 初始化窗体及控件ID
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL void MMIMRAPP_initDynIdTable(void);

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL MMI_RESULT_E  MMIMRAPP_HandlePromptWinMsg(
												MMI_WIN_ID_T win_id, 
												MMI_MESSAGE_ID_E msg_id, 
												DPARAM param
												);

/*****************************************************************************/
// 	Description : the process message function of the mr app root win
//	Global resource dependence : 
//  Author:        qgp
//	Note:   
/*****************************************************************************/
LOCAL void EditSelectPBContactCallback(
							MMIPB_HANDLE_T handle
							);

/*****************************************************************************/
// 	Description : the process message function of the mr app root win
//	Global resource dependence : 
//  Author:        qgp
//	Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMrAppBaseWinMsg( 
										 MMI_WIN_ID_T win_id, 
										 MMI_MESSAGE_ID_E msg_id, 
										 DPARAM param
										 );

/*****************************************************************************/
// 	Description : the message handler function of select data account win
//	Global resource dependence : 
//  Author: zack
//	Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMrappAccountWinMsg( 
											MMI_WIN_ID_T win_id, 
											MMI_MESSAGE_ID_E msg_id, 
											DPARAM param
											);

#ifdef __MR_CFG_CAMERA_SUPPORT__
/*****************************************************************************/
// 	Description : the message handler function of camera win
//	Global resource dependence : 
//  Author: zack
//	Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMrCameraWinMsg( 
											MMI_WIN_ID_T win_id, 
											MMI_MESSAGE_ID_E msg_id, 
											DPARAM param
											);
#endif

/**---------------------------------------------------------------------------*
 **                         Constant Variables                                *
 **---------------------------------------------------------------------------*/

//this window is the base win (a manager win) for mr app entity
WINDOW_TABLE( MMIMRAPP_BASE_WIN_TAB ) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ), 
    WIN_MOVE_STYLE(MOVE_FORBIDDEN),
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_CUR),
    WIN_FUNC((uint32) HandleMrAppBaseWinMsg),    
    WIN_ID(MMIMRAPP_BASE_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

//this window is select data account win for user
WINDOW_TABLE( MMIMRAPP_ACCOUNT_WIN_TAB ) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ), 
    WIN_TITLE(TXT_COMMON_WWW_ACCOUNT),
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_CUR),
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MRAPP_ACCOUNT_LISTCTRL_ID),
    WIN_FUNC((uint32) HandleMrappAccountWinMsg), 
    WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
    WIN_ID(MMIMRAPP_ACCOUNT_WIN_ID),
    END_WIN
};

#ifdef __MR_CFG_CAMERA_SUPPORT__
WINDOW_TABLE( MMIMRAPP_CAMERA_WIN_TAB ) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ), 
    WIN_MOVE_STYLE(MOVE_FORBIDDEN),
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_CUR),
    WIN_FUNC((uint32) HandleMrCameraWinMsg),    
    WIN_ID(MMIMRAPP_CAMERA_WIN_ID),
    END_WIN
};
#endif

/**---------------------------------------------------------------------------*
 **                       function bodies 
 **---------------------------------------------------------------------------*/
 
/*****************************************************************************/
//	   Description : to handle the message of prompt alert window
//	  Global resource dependence : 
//	Author:qgp
//	  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E	MMIMRAPP_HandleQueryWin1Msg(
												MMI_WIN_ID_T     win_id, 
												MMI_MESSAGE_ID_E msg_id, 
												DPARAM param
												)
{
	BOOLEAN recode = MMI_RESULT_TRUE;

	switch(msg_id)
	{
	case MSG_APP_OK:
    case MSG_APP_WEB:
#ifdef __MMI_SKYQQ__
        sky_qq_force_exit();
#endif
		MMK_CloseWin(win_id);
		MMIMRAPP_exitMrApp(FALSE);
        MMIMRAPP_StopStatusIconTimer();
        MAIN_SetIdleMrappState(FALSE); /*lint !e718 !e18*/
#ifdef __MMI_SKYQQ__
		MAIN_SetIdleQQState(0, GUIWIN_STBDATA_ICON_NONE);
#endif
		break;
		
	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		MMK_CloseWin(win_id);
		break;
		
	case MSG_TIMER://pass timer
		break;
		
	default:
		recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
		break;
	}
	
	return recode;
}

/*****************************************************************************/
// 	Description : the mr app entity entry
//	Global resource dependence : 
//  Author: qgp
//	Note: param: prototype is "%xxx.mrp" or NULL
/*****************************************************************************/
#ifdef MRAPP_USE_UDISK_FIRST
char  gappparam[64];
#endif
BOOLEAN MMIMRAPP_MrAppRun(void* param, uint32 appId)
{	
	BOOLEAN ret = FALSE;
	MMIMR_Trace(("enter MMIMRAPP_MrAppRun entryParam=%s, appId=%d, g_mr_curApp=%d, g_mr_vmBackRun=%d", (char*)param, appId, g_mr_curApp, g_mr_vmBackRun));
#ifdef MRAPP_USE_UDISK_FIRST
	SCI_MEMSET(gappparam,0,sizeof(gappparam));
	SCI_MEMCPY(gappparam,(char*)param,strlen((char*)param))
#endif

	if(MMK_IsOpenWin(MMIMRAPP_BASE_WIN_ID))
	{
		if(!MMK_IsFocusWin(MMIMRAPP_BASE_WIN_ID))
		{
			MMK_WinGrabFocus(MMIMRAPP_BASE_WIN_ID);//获得FOCUS
		}
		MMIMR_Trace(("MMIMRAPP_MrAppRun MMIMRAPP_BASE_WIN_ID is opend!"));
	}
	else
	{
#ifdef MR_BINDTO_VIRTUAL_WIN
		if(g_mr_vmBackRun != MRAPP_MAX)
		{
			if(appId == g_mr_vmBackRun)
			{
				if(MMK_CreateWin((uint32*)MMIMRAPP_BASE_WIN_TAB, (ADD_DATA)param))
				{
					//MMIMRAPP_pauseMp3();
					mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_APPRESUME, 0);//send back2front event to app
					ret = TRUE;
				}
			}
			else
			{
				MMI_STRING_T string = {0};
				//提示:请先退出后台应用
				string.wstr_len = sizeof("\xF7\x8B\x48\x51\x00\x90\xFA\x51\x0E\x54\xF0\x53\x94\x5E\x28\x75\x00\x00")/2-1;
				string.wstr_ptr = (uint8*)"\xF7\x8B\x48\x51\x00\x90\xFA\x51\x0E\x54\xF0\x53\x94\x5E\x28\x75\x00\x00"; /*lint !e64*/
				MMIPUB_OpenAlertWinByTextPtr(NULL,&string,NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_OKCANCEL,MMIMRAPP_HandleQueryWin1Msg);
			}
		}
		else
		{
			MMIMRAPP_setCurApp(appId);
			MMIMRAPP_pauseMp3();
			ret = MMK_CreateWin((uint32*)MMIMRAPP_BASE_WIN_TAB, (ADD_DATA)param);
		}
#else
		MMIMRAPP_setCurApp(appId);
		MMIMRAPP_pauseMp3();
		ret = MMK_CreateWin((uint32*)MMIMRAPP_BASE_WIN_TAB, (ADD_DATA)param);
#endif
	}

	MMIMR_Trace(("exit MMIMRAPP_MrAppRun entryParam=%s, appId=%d, g_mr_curApp=%d, g_mr_vmBackRun=%d", (char*)param, appId, g_mr_curApp, g_mr_vmBackRun));
	return ret;	
}

BOOLEAN MMIMRAPP_StartApp(uint8* param)
{
	int32 i = 0;
    BOOLEAN ret = 0;
    uint8 entry[64] = {0};

	//SCI_TRACE_LOW:"mrapp MMIMRAPP_StartApp param = %s"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_WINTAB_479_112_2_18_3_12_53_7,(uint8*)"s", param);

    if(*param == '%')
    {
        while((*param != ',') && (*param != '\0'))
        {
			entry[i] = *param;
            param++;
			i++;
        }
		entry[i] = '\0';

		if (*param != '\0')
		{
			param++;
		}
		else
		{
			param = NULL;
		}
        
		//SCI_TRACE_LOW:"mrapp MMIMRAPP_StartApp entry = %s param = %s"
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_WINTAB_500_112_2_18_3_12_53_8,(uint8*)"ss", entry, param);

		ret = mr_start_dsm_ex((char*)entry, (char*)param); /*lint !e718 !e746*/
    }
    else
    {
        ret = mr_start_dsm((char*)param); 
    }

    return ret;
}

/*****************************************************************************/
// 	Description : the process message function of the mr app root win
//	Global resource dependence : 
//  Author:        qgp
//	Note:   
/*****************************************************************************/
PUBLIC void MMIMRAPP_SelectPBContactCallback(
										MMIPB_HANDLE_T handle
										)
{
	MMIMR_Trace(("enter MMIMRAPP_SelectPBContactCallback() handle = 0x%x", handle));
	MMK_SendMsg(MMIMRAPP_BASE_WIN_ID, MMI_SELECT_CONTACT_CNF, (DPARAM)handle);
}

void MMIMRAPP_ClearScreen(void)
{
	uint16       str_width  = 0;
	uint16       str_height = 0;
	uint16       lcd_width  = 0;
	uint16       lcd_height = 0;
    GUI_RECT_T   rect  = {0};
	GUI_POINT_T  point = {0};
	MMI_STRING_T text  = {0};
   	GUISTR_STYLE_T text_style = {0};
	GUISTR_STATE_T text_state = GUISTR_STATE_ANGLE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	
	MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &text);
	GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID, &lcd_width, &lcd_height);

    rect.top  = 0;
	rect.left = 0;
	rect.right  = lcd_width;
	rect.bottom = lcd_height;
	LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

	str_width  = GUI_GetStringWidth(MMI_DEFAULT_TEXT_FONT, text.wstr_ptr, text.wstr_len);
	str_height = GUI_GetStringHeight(MMI_DEFAULT_TEXT_FONT, text.wstr_ptr, text.wstr_len);

	point.x = (lcd_width-str_width)/2;
	point.y = (lcd_height-str_height)/2;

    text_style.effect = FONT_EFFECT_REVERSE_EDGE; 
    text_style.angle = MMK_GetScreenAngle();
    text_style.align = ALIGN_LEFT;
    text_style.font  = MMI_DEFAULT_TEXT_FONT;
    text_style.font_color = MMI_WHITE_COLOR;
    
	GUISTR_DrawTextToLCDSingleLine(&lcd_dev_info, &point, &text, &text_style, text_state); 
}

/*****************************************************************************/
// 	Description : the process message function of the mr app root win
//	Global resource dependence : 
//   Author:        qgp
//	Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMrAppBaseWinMsg( 
										 MMI_WIN_ID_T win_id, 
										 MMI_MESSAGE_ID_E msg_id, 
										 DPARAM param)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	MMIMR_Trace(("enter HandleMrAppBaseWinMsg() msg_id = 0x%x, param = 0x%x", msg_id, param));
    
	switch(msg_id)
	{	    
	case MSG_OPEN_WINDOW:
		{
			uint32 timout = 1000;
			s_mr_keyDown = FALSE;
			s_mr_vmPaused = FALSE;
            s_mr_gsensor_flag = MMISET_GetIsSensorOpenFlag();
            MMISET_SetIsSensorOpenFlag(0);
#ifdef MR_BINDTO_VIRTUAL_WIN
			if(g_mr_vmBackRun != MRAPP_MAX)
			{
				g_mr_vmBackRun = MRAPP_MAX;
				break;
			}
#endif	
			MMIMRAPP_initLocalRes();
			MMIMRAPP_initDynIdTable();
			MMIMRAPP_initAllExtraSrc();
			MMIMRAPP_freeAllSnode();
			s_mr_allReady = TRUE;

#ifdef MRAPP_SUPPORT_CLKADAPT
			MMIMR_Trace(("#######HAL_GetMCUClk: %d", HAL_GetMCUClk()));
			s_mr_cpuClk_mng.orgCpuClk = HAL_GetMCUClk();
			MMIMRAPP_CLKSetted(TRUE, FREQ_INDEX_APP_BASE, s_mr_cpuClk_mng.orgCpuClk);
			MMIMRAPP_SetArmClkHigh();
#endif
            //MMIPUB_OpenAlertWinByTextId(&timout, TXT_COMMON_WAITING,
            //    TXT_NULL, IMAGE_PUBWIN_WAIT, PNULL, PNULL, MMIPUB_SOFTKEY_NONE, PNULL);
			s_mr_prompt_timer = MMK_CreateWinTimer(MMIMRAPP_BASE_WIN_ID, 50, FALSE);//delay to start dsm app, for the wait win to show 
			break;
		}
	case MSG_CLOSE_WINDOW:
		MMIMR_Trace(("#MSG_CLOSE_WINDOW"));
#ifdef __MMI_SKYQQ__
        if(!MMK_IsFocusWin(win_id) && GetSkyQQRunState() == QQ2008_RUN)
        {
            uint32 message_id = MSG_KEYDOWN_RED;
    	    recode = ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&message_id);//add for skyqq backrun
        }    
#endif
#ifdef MR_BINDTO_VIRTUAL_WIN
		if(g_mr_vmBackRun != MRAPP_MAX)
		{
			break;
		}
#endif

		if(s_mr_VMReady)
		{
			s_mr_VMReady = FALSE;
			mr_stop();
		}	

		MMIMRAPP_freeMmiRes();
#ifdef __MMI_SKYQQ__
        if(GetSkyQQRunState() == QQ2008_STOP)
        {
            QQ2008MemoryFree();
        }
#endif
		break;
			
	case MSG_TIMER:	
		if(s_mr_prompt_timer == *(uint8*)param)
		{

			char* addData = (char*)MMK_GetWinAddDataPtr(win_id);
			int32 ret = MR_SUCCESS;
			
			MMK_StopTimer(s_mr_prompt_timer);
			s_mr_prompt_timer = 0;	
			s_mr_VMReady = TRUE;
            MMIMR_Trace(("DSM entry = %s", addData));
#ifndef WIN32
            ret = (int32)MMIMRAPP_StartApp((const char*)addData); /*lint !e64*/
#endif

#ifdef MRAPP_SUPPORT_CLKADAPT
			if(!s_mr_cpuClk_mng.setted)
				MMIMRAPP_RestoreARMClk();
#endif
			if(ret != MR_SUCCESS)
			{
				s_mr_VMReady = FALSE;
				MMK_CloseWin(MMIMRAPP_BASE_WIN_ID);
			}

			MMIMR_Trace(("mrapp. DSM start param = %s, ret = %d, vm_ready = %d", addData, ret, s_mr_VMReady));
		}	
		break;
		
	case MSG_LOSE_FOCUS:
		MMIMR_Trace(("ROOT. MSG_LOSE_FOCUS"));
		MMIMRAPP_pauseApp();
		break;

	case MSG_GET_FOCUS:
		MMIMR_Trace(("ROOT. MSG_GET_FOCUS"));
		MMIMRAPP_resumeApp();
        /*add by zack@20110921 start*/
        if(g_mr_pbinfo != NULL)
        {
			if(g_mr_pbinfo->needInform == TRUE)
			{
            	g_mr_pbinfo->needInform = FALSE;
            	mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_OK, 0);
			}

			if(s_mr_PBOfCancelMsg)
			{
				s_mr_PBOfCancelMsg = FALSE;
            	mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_CANCEL, 0);
			}
        }
        /*add by zack@20110921 end*/
		break;

	case MMRAPP_MSG_NETINIT_RESULT:
		if(param != NULL)
		{
			MMIMRAPP_netInitResult(((MMIMRAPP_NETINIT_RESULT_T*)param)->result);
			MMIMR_Trace(("mrapp. net init result return."));
		}
		break;
        
	case MMRAPP_MSG_NETREDAIL_IND:
		MMIMRAPP_redail();
		break;

	case MMRAPP_MSG_SYS_EXIT:	
		MMIMRAPP_closeAllWins();	
		break;

/* begin:modified by Tommy.yan 20120222 */
	case MMI_SELECT_CONTACT_CNF: 
		MMIMRAPP_pbInfoReturn((MMIPB_HANDLE_T)param);
		break;
/* end:modified by Tommy.yan 20120222 */

#ifdef MR_HANDSET_IS_SUPPORT_TOUCHPANEL
	case MSG_TP_PRESS_DOWN:
		recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_DOWN, (void*)param);
		break;
	case MSG_TP_PRESS_UP:
		recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_UP, (void*)param);
		break;
//2010-2-24 add move & update MRP_PORT_VER to 5
	case MSG_TP_PRESS_MOVE:
		recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_MOVE, (void*)param);
		break;
//2010-2-24 add end
#endif

#ifdef __MMI_SKYQQ__
    case MSG_KEYDOWN_FLIP:
        {
            uint32 message_id = MSG_KEYDOWN_RED;
    	    ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&message_id);//add for skyqq backrun
        }
        recode = MMI_RESULT_FALSE;
        break;
#endif	
		
	case MSG_KEYPRESSUP_RED:
	case MSG_KEYDOWN_RED:
	case MSG_KEYUP_RED://2007add
    	MMIMR_Trace(("mrapp HandleMrAppBaseWinMsg() msg_id = 0x%x, s_mr_fbhup2idle = %d", msg_id, s_mr_fbhup2idle));
		if(s_mr_fbhup2idle != 1)	
        {      
#ifdef __MMI_SKYQQ__
            if(GetSkyQQRunState() == QQ2008_RUN)
            {
                ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
            }
#endif
			recode = MMI_RESULT_FALSE;
        }
        else
        {
            recode = ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
        }
		break;

	default:
		if(s_mr_VMReady)
		{
			recode = ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
		}
		break;
	}

	MMIMR_Trace(("exit HandleMrAppBaseWinMsg() msg_id = 0x%x, recode = %d", msg_id, recode));
	return recode;
}

/*****************************************************************************/
// 	Description : 初始化窗体及控件ID
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL void MMIMRAPP_initDynIdTable(void)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_initDynIdTable()"));
	SCI_MEMSET(s_mr_winTab, 0, sizeof(s_mr_winTab));
	SCI_MEMSET(s_mr_ctrlTab, 0, sizeof(s_mr_ctrlTab));
}

/*****************************************************************************/
// 	Description : 获取一空闲的 ID,或释放一id
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL int32 MMIMRAPP_OpDynId(MMIMRAPP_DYN_WIN_TYPE_E winType, int32 opId, MMIMRAPP_ID_TYPE_E idType)
{
	int32 id = -1;
	int32 i = 0;
	uint8 *tablePtr = NULL;
	int32 baseId = 0;
	uint8 tableSize = 0;

	MMIMR_Trace(("mrapp. MMIMRAPP_OpDynId() optype:%d, idType: %d", opId, idType));

	if(opId == 0)//this indi that: not free ,not get too
	{
		return id;
	}
	
	if(winType < MRAPP_WINTYPE_MAX && winType >= 0) /*lint !e568*/
	{
		if(idType == MRAPP_ID_WIN)
		{
			tablePtr = (uint8*)s_mr_winTab;
			tableSize = sizeof(s_mr_winTab);
			baseId = MRAPP_DYN_BASE_WIN_ID;
		}
		else if(idType == MRAPP_ID_CTRL)
		{
			tablePtr = (uint8*)s_mr_ctrlTab;
			tableSize = sizeof(s_mr_ctrlTab);
			baseId = MRAPP_DYN_BASE_CTRL_ID;
		}
	}

	if(tablePtr != NULL)
	{
		if(opId == -1)//get a new idle win id
		{
			for(i = 0; i < tableSize; i++)
			{
				if(tablePtr[i] == 0)
				{
					id = baseId + i;
					tablePtr[i] = 1;
					if((idType != MRAPP_ID_WIN) || !MMK_IsOpenWin(id))
					{
						break;
					}
					else
					{
						id = -1;
					}
				}
			}
		}
		else//free the spec id to idle
		{
			i = opId - baseId;
			if(i >= 0 && i < tableSize)
			{
				tablePtr[i] = 0;
			}
		}
	}
	
	MMIMR_Trace(("mrapp. get id :%d", id));

	return id;
}

/*****************************************************************************/
// 	Description : 释放窗体相关的用户数据
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL void MMIMRAPP_freeDynWinData(MMIMRAPP_ADD_PARAMETERS_T* addData)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_freeDynWinData()"));
	
	if(addData != NULL)
	{
		uint8 *titlePtr = NULL;
		uint8 *textPtr  = NULL;
		MRAPP_MENU_DATA *menuPtr = NULL;
		MRAPP_LIST_ITEM_T *delElem, *elemPtr;
		
		/*free dynamic id*/
		MMIMRAPP_OpDynId(addData->dynWinType, addData->winId, MRAPP_ID_WIN);
		if(addData->dynWinType != MRAPP_WINTYPE_APP)
		{
			MMIMRAPP_OpDynId(addData->dynWinType, addData->ctrlId, MRAPP_ID_CTRL);
		}
		
		/*free user additional data*/
		switch(addData->dynWinType)
		{
		case MRAPP_WINTYPE_DIALOG:
			break;
			
		case MRAPP_WINTYPE_TEXT:
			titlePtr = (uint8*)addData->u.text.title.wstr_ptr; 
            textPtr  = (uint8*)addData->u.text.textInfo.wstr_ptr;//add by zack@20110916
			break;
			
		case MRAPP_WINTYPE_EDITOR:
			titlePtr = (uint8*)addData->u.edit.title.wstr_ptr;
			if(addData->u.edit.textOut.wstr_ptr!= NULL)
			{
				SCI_FREE(addData->u.edit.textOut.wstr_ptr);
			}
			break;
			
		case MRAPP_WINTYPE_MENU:
			menuPtr = &addData->u.menu;
			titlePtr = (uint8*)menuPtr->title.wstr_ptr;
			
			if(menuPtr->listElem != NULL)
			{
				delElem = menuPtr->listElem->nextItem;
				while(delElem != NULL)
				{
					elemPtr = delElem->nextItem;
					SCI_FREE(delElem);
					delElem = elemPtr;
				}
			
				SCI_FREE(menuPtr->listElem);
			}
			break;
			
		case MRAPP_WINTYPE_APP:
			break;
			
		default:
			MMIMR_Trace(("error para: %d", addData->dynWinType));
			break;
		}
		
		if(titlePtr != NULL)
		{
			SCI_FREE(titlePtr);
		}
        /*add by zack@20110916 start*/
    	if(textPtr != NULL)
		{
			SCI_FREE(textPtr);
		}
        /*add by zack@20110916 end*/
		SCI_FREE(addData);
	}
}

/*****************************************************************************/
//  Description : creat softkey control
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_CreatSoftkeyCtrl(
									 MMI_HANDLE_T win_handle,     //win id
									 MMI_TEXT_ID_T leftsoft_id,
									 MMI_TEXT_ID_T midsoft_id,
									 MMI_TEXT_ID_T rightsoft_id
									 )
{
    GUISOFTKEY_INIT_DATA_T	softkey_data = {0};
    MMI_CONTROL_CREATE_T softkey_create = {0};
    MMI_CTRL_ID_T	softkey_ctrl_id = 0;

    softkey_data.leftsoft_id = leftsoft_id;
    softkey_data.midsoft_id	= midsoft_id;
    softkey_data.rightsoft_id = rightsoft_id;

    softkey_ctrl_id = MMITHEME_GetSoftkeyCtrlId();
    softkey_create.ctrl_id = softkey_ctrl_id;
    softkey_create.guid    = SPRD_GUI_SOFTKEY_ID;
    softkey_create.parent_win_handle = win_handle;
    softkey_create.init_data_ptr = &softkey_data;

#ifdef MMI_PDA_SUPPORT
	if(MMISET_IDLE_ISTYLE != MMIAPISET_GetIdleStyle())
	{
		MMK_SetWinDisplayStyleState(win_handle, WS_HAS_BUTTON_SOFTKEY, TRUE);
	}
#endif
    MMK_CreateControl( &softkey_create );
    MMK_SetWinSoftkeyCtrlId(win_handle, softkey_ctrl_id);
}

/*****************************************************************************/
// 	Description :根据动态ID 创建一需求窗体
//	Global resource dependence : none
//  Author: qgp
//	Note:  the return value: -2 the caller need to free the addData; -1 the caller need not
/*****************************************************************************/
int32 MMIMRAPP_createADynWin(MMIMRAPP_ADD_PARAMETERS_T *addData, MMI_STRING_T *textInfo)
{
	int32		winId = -1;
	int32		ctrlId = -1;
	MMIMRAPP_DYN_WIN_TYPE_E winType;
	BOOLEAN		result = FALSE;
	MMI_WINDOW_CREATE_T win_create = {0};
	MMI_CONTROL_CREATE_T create = {0};
	MMI_HANDLE_T win_handle;
	GUI_BORDER_T    edit_border = {0}; //add by zack@20120809
	MMIMR_Trace(("mrapp. MMIMRAPP_createADynWin()"));
	
	if(addData == NULL)
	{
		return -2;
	}
	
	addData->winId = 0;
	addData->ctrlId = 0;
	winType = addData->dynWinType;
	winId = MMIMRAPP_OpDynId(winType, -1, MRAPP_ID_WIN);//get id
	if(winId == -1)
	{
		return -2;
	}	
	addData->winId = winId;
	/*modify by zack@20120809 start*/
#ifdef MMI_RES_ORIENT_BOTH
	MMK_SetScreenAngle(LCD_ANGLE_0);
#endif
    /*modify by zack@20120809 end*/
	win_create.applet_handle = MMK_GetFirstAppletHandle();
	win_create.win_id = addData->winId;
	win_create.func = MMIMRAPP_HandleDynWinMsg;
	win_create.win_priority = WIN_ONE_LEVEL;
	win_create.add_data_ptr = (ADD_DATA)addData;
	win_create.window_move_style = MOVE_FORBIDDEN;
#ifdef PDA_UI_ANDROID_SOFTKEY
    if(winType == MRAPP_WINTYPE_TEXT || winType == MRAPP_WINTYPE_EDITOR || winType == MRAPP_WINTYPE_DIALOG)
    {
        win_create.window_style = WS_HAS_SOFTKEY;
    }
#endif
	s_mr_inMrSelfWinCreating = TRUE;
	win_handle = MMK_CreateWindow(&win_create);
    MMK_SetWinSupportAngle(win_handle, WIN_SUPPORT_ANGLE_CUR);
	s_mr_inMrSelfWinCreating = FALSE;
	
	MMIMR_Trace(("mrapp. MMK_CreateWindow() result: 0x%x", win_handle));	
	if(win_handle != 0)
	{
		result = FALSE;
		if(winType != MRAPP_WINTYPE_APP)
		{
			ctrlId = MMIMRAPP_OpDynId(winType, -1, MRAPP_ID_CTRL);//get id
			if(ctrlId == -1)
			{
				MMK_CloseWin(addData->winId);
				return -1;
			}
			addData->ctrlId = ctrlId;
		}
		
		switch(winType)
		{
		case MRAPP_WINTYPE_TEXT:
			{
				GUITEXT_INIT_DATA_T init_data = {0};
				MMI_TEXT_ID_T 	left_sk_id = TXT_NULL;
				MMI_TEXT_ID_T 	right_sk_id = TXT_NULL;
				if(addData->u.text.type == MR_DIALOG_OK)
				{
					left_sk_id = TXT_COMMON_OK;
				}
				else if(addData->u.text.type == MR_DIALOG_OK_CANCEL)
				{
					left_sk_id = TXT_COMMON_OK;
					right_sk_id = STXT_CANCEL;
				}
				else
				{
					right_sk_id = STXT_RETURN;
				}
                GUIWIN_CreateTitleDyna(addData->winId, TXT_NULL);
                MMIMRAPP_CreatSoftkeyCtrl(addData->winId, left_sk_id, TXT_NULL, right_sk_id);
                init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);
				create.ctrl_id = addData->ctrlId;
				create.guid    = SPRD_GUI_TEXTBOX_ID;
				create.parent_win_handle = addData->winId;
				create.init_data_ptr = (void*)&init_data; /*lint !e733*/
				MMK_CreateControl(&create); 
				result = TRUE;
			}
			break;
		case MRAPP_WINTYPE_EDITOR:
			{
				MRAPP_EDIT_DATA *editPtr = &addData->u.edit;
				GUIEDIT_INIT_DATA_T init_data = {0};
                BOOLEAN is_disp_im_icon = TRUE;
                BOOLEAN is_disp_num_info = TRUE;
                //init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);
                //init_data.both_rect.v_rect.top = init_data.both_rect.v_rect.top + MMI_TITLE_THEME_HEIGHT;
                //init_data.both_rect.h_rect.top = init_data.both_rect.h_rect.top + MMI_TITLE_THEME_HEIGHT_H;
				editPtr->maxLen = (editPtr->maxLen > MMIMRAPP_DEFAULT_MAX_LEN) ? MMIMRAPP_DEFAULT_MAX_LEN : editPtr->maxLen;//temp.
				switch(editPtr->type)
				{
				default:
				case MR_EDIT_ANY:
                    GUIWIN_CreateTitleDyna(addData->winId, TXT_NULL);
                    MMIMRAPP_CreatSoftkeyCtrl(addData->winId, STXT_OK, TXT_NULL, STXT_RETURN);

                    init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);
						init_data.type = GUIEDIT_TYPE_TEXT;
						create.ctrl_id = addData->ctrlId;
						create.guid = SPRD_GUI_EDITBOX_ID;
						create.parent_win_handle = addData->winId; 
						create.init_data_ptr = (void*)&init_data; /*lint !e733*/
						MMK_CreateControl( &create );
                    GUIEDIT_SetIm(addData->ctrlId,GUIIM_TYPE_SET_ALL,GUIIM_TYPE_SMART);
                    GUIEDIT_SetStyle(addData->ctrlId, GUIEDIT_STYLE_MULTI);//多行
                    GUIEDIT_SetTextMaxLen(addData->ctrlId,editPtr->maxLen,editPtr->maxLen);
                    GUIEDIT_SetString(addData->ctrlId, textInfo->wstr_ptr, textInfo->wstr_len);    
                    GUIEDIT_SetDispImIcon(addData->ctrlId,&is_disp_im_icon,&is_disp_num_info);

                    //init_data.add_data.text_add.cur_inputmethod = IM_SMART_MODE;
					break;
				case MR_EDIT_NUMERIC:
                    GUIWIN_CreateTitleDyna(addData->winId, TXT_NULL);
                    MMIMRAPP_CreatSoftkeyCtrl(addData->winId, STXT_OK, TXT_NULL, STXT_RETURN);

                    init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);

                    init_data.type = GUIEDIT_TYPE_PHONENUM;
                    init_data.str_max_len = editPtr->maxLen;
                    create.ctrl_id = addData->ctrlId;
                    create.guid = SPRD_GUI_EDITBOX_ID;
                    create.parent_win_handle = addData->winId; 
                    create.init_data_ptr = &init_data;
                    MMK_CreateControl( &create );
                    GUIEDIT_SetString(addData->ctrlId, textInfo->wstr_ptr, textInfo->wstr_len);
                    GUIEDIT_SetIm(addData->ctrlId,GUIIM_TYPE_DIGITAL,GUIIM_TYPE_DIGITAL);
					break;
				case MR_EDIT_PASSWORD:
                    GUIWIN_CreateTitleDyna(addData->winId, TXT_NULL);
                    MMIMRAPP_CreatSoftkeyCtrl(addData->winId, STXT_OK, TXT_NULL, STXT_RETURN);

                    init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);

                    init_data.type = GUIEDIT_TYPE_PASSWORD;
                    create.ctrl_id = addData->ctrlId;
                    create.guid = SPRD_GUI_EDITBOX_ID;
                    create.parent_win_handle = addData->winId; 
                    create.init_data_ptr = &init_data;
                    MMK_CreateControl( &create );

                    GUIEDIT_SetIm(addData->ctrlId, GUIIM_TYPE_ABC|GUIIM_TYPE_DIGITAL, GUIIM_TYPE_ABC);
                    GUIEDIT_SetMaxLen(addData->ctrlId, editPtr->maxLen);
                    GUIEDIT_SetAlign(addData->ctrlId, ALIGN_LEFT);//靠左上起
                    GUIEDIT_SetDispImIcon(addData->ctrlId, &is_disp_im_icon, PNULL);//显示输入法的icon
                    GUIEDIT_SetString(addData->ctrlId, textInfo->wstr_ptr, textInfo->wstr_len);
					/* add by zack@20120809 start */
					edit_border.type  = GUI_BORDER_NONE;
					GUIEDIT_SetBorder(addData->ctrlId, &edit_border);
                    /* add by zack@20120809 end */
					break;
                case MR_EDIT_ALPHA:
                    GUIWIN_CreateTitleDyna(addData->winId, TXT_NULL);
                    MMIMRAPP_CreatSoftkeyCtrl(addData->winId, STXT_OK, TXT_NULL, STXT_RETURN);

                    init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);
					init_data.type = GUIEDIT_TYPE_TEXT;
					create.ctrl_id = addData->ctrlId;
					create.guid = SPRD_GUI_EDITBOX_ID;
					create.parent_win_handle = addData->winId; 
					create.init_data_ptr = (void*)&init_data; /*lint !e733*/
					MMK_CreateControl( &create );
                    GUIEDIT_SetIm(addData->ctrlId,GUIIM_TYPE_DIGITAL|GUIIM_TYPE_ABC|GUIIM_TYPE_ENGLISH,GUIIM_TYPE_ENGLISH);
                    GUIEDIT_SetStyle(addData->ctrlId, GUIEDIT_STYLE_MULTI);//多行
                    GUIEDIT_SetTextMaxLen(addData->ctrlId,editPtr->maxLen,editPtr->maxLen);
                    GUIEDIT_SetString(addData->ctrlId, textInfo->wstr_ptr, textInfo->wstr_len);    
                    GUIEDIT_SetDispImIcon(addData->ctrlId,&is_disp_im_icon,&is_disp_num_info);

                    //init_data.add_data.text_add.cur_inputmethod = IM_SMART_MODE;
                    break;
				}
				result = TRUE;
			}
			break;
		case MRAPP_WINTYPE_MENU:
			{
				GUIMENU_INIT_DATA_T  init_data = {0};
                init_data.both_rect = MMITHEME_GetWinClientBothRect(addData->winId);
                init_data.both_rect.v_rect.top = init_data.both_rect.v_rect.top + MMI_TITLE_THEME_HEIGHT;
                init_data.both_rect.h_rect.top = init_data.both_rect.h_rect.top + MMI_TITLE_THEME_HEIGHT_H;
#ifdef MMI_PDA_SUPPORT
				init_data.both_rect.v_rect.bottom = init_data.both_rect.v_rect.bottom + (int16)MMITHEME_GetWinSoftkeyHeight(addData->winId);
				init_data.both_rect.h_rect.bottom = init_data.both_rect.h_rect.bottom + (int16)MMITHEME_GetWinSoftkeyHeight(addData->winId);
#endif
				init_data.is_static = FALSE;
				init_data.node_max_num = GUIMENU_DYNA_NODE_MAX_NUM;
				init_data.menu_style = GUIMENU_STYLE_THIRD;
				create.ctrl_id = addData->ctrlId;
				create.guid    = SPRD_GUI_MENU_ID;
				create.parent_win_handle = addData->winId;
				create.init_data_ptr = (void*)&init_data; /*lint !e733*/
				MMK_CreateControl(&create);
#ifndef MMI_PDA_SUPPORT
				MMIMRAPP_CreatSoftkeyCtrl(addData->winId, TXT_NULL, TXT_NULL, TXT_NULL);
				GUIMENU_SetDynamicMenuSoftkey(addData->ctrlId, TXT_COMMON_OK, TXT_NULL, STXT_RETURN);
#endif
				
				GUIWIN_CreateTitleDyna(addData->winId, TXT_NULL);
				if(addData->u.menu.title.wstr_ptr!= NULL)
				{
					GUIMENU_SetMenuTitle(&addData->u.menu.title, addData->ctrlId);
				}
				
				result = TRUE;
			}
			break;
		case MRAPP_WINTYPE_APP:
			result = TRUE;
			break;
		default:
			break;
		}
		
		if(result != TRUE)
		{			
			MMK_CloseWin(addData->winId);
			winId = -1;
		}
	}
	else 
	{
		MMIMRAPP_OpDynId(winType, addData->winId, MRAPP_ID_WIN);//free id
		winId = -2;
	}
		
	return winId;
}


/*****************************************************************************/
// 	Description :动态窗口消息处理函数
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL MMI_RESULT_E  MMIMRAPP_HandleDynWinMsg(
											 MMIMRAPP_WINDOW_ID_E win_id, 
											 MMI_MESSAGE_ID_E msg_id, 
											 DPARAM param
											 )
{
	BOOLEAN recode = MMI_RESULT_TRUE;
	MMIMRAPP_ADD_PARAMETERS_T* addData = (MMIMRAPP_ADD_PARAMETERS_T*)MMK_GetWinAddDataPtr(win_id);

	MMIMR_Trace(("mrapp. MMIMRAPP_HandleDynWinMsg win: %d, msg_id :0x%x, addData:0x%x", win_id, msg_id, addData));

	if(addData == NULL)
	{	
		return MMI_RESULT_FALSE;
	}
	
	MMIMR_Trace(("mrapp. dynWinType: %d", addData->dynWinType));
	if(msg_id == MSG_LOSE_FOCUS)
	{
		MMIMRAPP_pauseApp();
	}
	else if(msg_id == MSG_GET_FOCUS)
	{
		MMIMRAPP_resumeApp();
	}
/* begin:modified by Tommy.yan 2012022 */
	else if(msg_id == MMI_SELECT_CONTACT_CNF)
	{
		MMIMRAPP_pbInfoReturn((MMIPB_HANDLE_T)param);
	}
/* end:modified by Tommy.yan 2012022 */
	else
	{
		switch(addData->dynWinType)
		{
		case MRAPP_WINTYPE_TEXT:
			{
				MRAPP_TEXT_DATA *textPtr = &addData->u.text;
				switch(msg_id)
				{
				case MSG_OPEN_WINDOW:
					GUITEXT_SetString(addData->ctrlId, textPtr->textInfo.wstr_ptr, textPtr->textInfo.wstr_len, FALSE);
					
#ifdef MMI_PDA_SUPPORT
					if(MR_DIALOG_OK_CANCEL == textPtr->type)
					{
						GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
            			GUIWIN_SetTitleButtonTextId(win_id, GUITITLE_BUTTON_RIGHT, TXT_COMMON_OK, FALSE);
            			GUIWIN_SetTitleButtonHandleMsgInfo(win_id, GUITITLE_BUTTON_RIGHT, MSG_APP_OK, 0);
					}
					else if(MR_DIALOG_OK == textPtr->type)
					{
						GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_LEFT, TITLE_BUTTON_INVISIBLE, FALSE);
						GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
            			GUIWIN_SetTitleButtonTextId(win_id, GUITITLE_BUTTON_RIGHT, TXT_COMMON_OK, FALSE);
            			GUIWIN_SetTitleButtonHandleMsgInfo(win_id, GUITITLE_BUTTON_RIGHT, MSG_APP_OK, 0);
					}
#endif
					break;
                    
				case MSG_FULL_PAINT:
					{
						if(textPtr->title.wstr_ptr != NULL)
						{
							GUIWIN_SetTitleText(win_id, textPtr->title.wstr_ptr, textPtr->title.wstr_len, TRUE);				
						}
						MMK_SetAtvCtrl( win_id, addData->ctrlId);
					}
					break;
					
				case MSG_CLOSE_WINDOW:
					MMIMRAPP_freeDynWinData(addData);
					break;
#ifdef MMI_PDA_SUPPORT
     			case MSG_CTL_PENOK:
        			if (PNULL != param)
        			{
            			uint src_id = ((MMI_NOTIFY_T*)param)->src_id;
            			switch(src_id)
            			{
            			case MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID:
                			MMK_SendMsg(win_id, MSG_APP_OK, NULL);
                			break;
            			case MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID:
                			break;
            			case MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID:
                 			MMK_SendMsg(win_id, MSG_APP_CANCEL, NULL);
                			break;
            			default:
                			break;
            			}
        			}
        			break;
#else
    			case MSG_CTL_PENOK:
#endif
				case MSG_CTL_MIDSK:
				case MSG_APP_OK:
				case MSG_CTL_OK:
				case MSG_APP_WEB:
#ifdef MMI_PDA_SUPPORT
				case MSG_APP_MENU:
#endif
					if(textPtr->type == MR_DIALOG_OK || textPtr->type == MR_DIALOG_OK_CANCEL)
						ProcessMrAppKeyMsg(MMIMRAPP_DIALOG_SELECT, NULL);
					break;
					
				case MSG_CTL_CANCEL:
				case MSG_APP_CANCEL:
					if(textPtr->type == MR_DIALOG_CANCEL || textPtr->type == MR_DIALOG_OK_CANCEL)
						ProcessMrAppKeyMsg(MMIMRAPP_DIALOG_RETURN, NULL);
					break;
					
				case MSG_KEYPRESSUP_RED:
				case MSG_KEYDOWN_RED:
					if(s_mr_fbhup2idle != 1)	
						recode = MMI_RESULT_FALSE;
					break;			
                    
				default:
					recode = MMI_RESULT_FALSE;
					break;
				}	
			}
			break;
		
		case MRAPP_WINTYPE_EDITOR:
			{
				MRAPP_EDIT_DATA *editPtr = &addData->u.edit;
				
				switch(msg_id)
				{
				case MSG_OPEN_WINDOW:
					MMK_SetAtvCtrl(win_id, addData->ctrlId);
					if(editPtr->title.wstr_ptr!= NULL)
					{
						GUIWIN_SetTitleText(addData->winId, (uint8 *)editPtr->title.wstr_ptr, (uint16)editPtr->title.wstr_len, FALSE); /*lint !e64*/
					}

#ifdef MMI_PDA_SUPPORT
					GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
            		GUIWIN_SetTitleButtonTextId(win_id, GUITITLE_BUTTON_RIGHT, TXT_COMMON_OK, FALSE);
            		GUIWIN_SetTitleButtonHandleMsgInfo(win_id, GUITITLE_BUTTON_RIGHT, MSG_APP_OK, 0);
//					GUIWIN_SetTitleButtonImage(win_id, GUITITLE_BUTTON_RIGHT, IMAGE_ISTYLE_TITLE_OK, IMAGE_ISTYLE_TITLE_OK_DOWN, FALSE);
#endif

					break;

				case MSG_FULL_PAINT:
					MMIMR_Trace(("mrapp.edit. MSG_FULL_PAINT"));
					break;
					
				case MSG_CLOSE_WINDOW:
					MMIMRAPP_freeDynWinData(addData);
					break;

#ifdef MMI_PDA_SUPPORT
     			case MSG_CTL_PENOK:
        			if (PNULL != param)
        			{
            			uint src_id = ((MMI_NOTIFY_T*)param)->src_id;
            			switch(src_id)
            			{
            			case MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID:
                			MMK_SendMsg(win_id, MSG_APP_OK, NULL);
                			break;
            			case MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID:
                			break;
            			case MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID:
                 			MMK_SendMsg(win_id, MSG_APP_CANCEL, NULL);
                			break;
            			default:
                			break;
            			}
        			}
        			break;
#else
    			case MSG_CTL_PENOK:
#endif				
				case MSG_CTL_OK:
                case MSG_APP_WEB:
                case MSG_APP_OK:
#ifdef MMI_PDA_SUPPORT
				case MSG_APP_MENU:
#endif
					ProcessMrAppKeyMsg(MMIMRAPP_DIALOG_SELECT, NULL);
					break;
					
				case MSG_CTL_CANCEL:
                case MSG_APP_CANCEL:
					ProcessMrAppKeyMsg(MMIMRAPP_DIALOG_RETURN, NULL);
					break;
					
				default:
					recode = MMI_RESULT_FALSE;
					break;
				}	
			}
			break;
			
		case MRAPP_WINTYPE_MENU:
			{
				MRAPP_MENU_DATA *menuPtr = &addData->u.menu;
				MRAPP_LIST_ITEM_T *findElem;
				uint16 curIndex;
				int32 key;
				BOOLEAN found = FALSE;
				
				curIndex = GUIMENU_GetCurNodeId(addData->ctrlId) - 1;
				menuPtr->curFocusItemIdx = curIndex;
				
				switch(msg_id)
				{
				case MSG_OPEN_WINDOW:
					MMIMR_Trace(("mrapp.menu. MSG_OPEN_WINDOW"));
					MMK_SetAtvCtrl( win_id, addData->ctrlId);
					break;

				case MSG_CLOSE_WINDOW:
					MMIMRAPP_freeDynWinData(addData);
					break;
					
				case MSG_CTL_MIDSK:
				case MSG_CTL_OK:
				case MSG_CTL_PENOK:
                case MSG_APP_OK:
                case MSG_APP_WEB:
					findElem = menuPtr->listElem;
					while(findElem != NULL)
					{
						if(findElem->index == curIndex)
						{
							key = findElem->key;
							found = TRUE;
							break;
						}
						findElem = findElem->nextItem;
					}
					
					if(found)
					{
						menuPtr->preFocusItemIdx = curIndex;
						ProcessMrAppKeyMsg(MMIMRAPP_MENU_SELECT, (void*)&key);
					}
					break;
					
				case MSG_CTL_CANCEL:
                case MSG_APP_CANCEL:
					ProcessMrAppKeyMsg(MMIMRAPP_MENU_RETURN, NULL);
					break;

				case MSG_CTL_LIST_MOVEUP:
				case MSG_CTL_LIST_MOVEDOWN:
				case MSG_CTL_LIST_MOVEBOTTOM:
				case MSG_CTL_LIST_MOVETOP:
					MMIMR_Trace(("*****************MSG_CTL_LIST_XXX, msg_id: %d", msg_id));

					if(msg_id == MSG_CTL_LIST_MOVEUP)
					{
						if(curIndex == 0)
						{
							menuPtr->curFocusItemIdx = menuPtr->maxItemNum - 1;
						}
						else
						{
							menuPtr->curFocusItemIdx = curIndex - 1;
						}
					}
					else if(msg_id == MSG_CTL_LIST_MOVEDOWN)
					{
						if(curIndex == menuPtr->maxItemNum - 1)
						{
							menuPtr->curFocusItemIdx = 0;
						}
						else
						{
							menuPtr->curFocusItemIdx = curIndex + 1;
						}
					}
					else if(msg_id == MSG_CTL_LIST_MOVEBOTTOM)
					{
						menuPtr->curFocusItemIdx = 0;
					}
					else
					{
						menuPtr->curFocusItemIdx = menuPtr->maxItemNum - 1;
					}
					
					MMIMR_Trace(("curIndex: %d, maxItemNum: %d", curIndex, menuPtr->maxItemNum));
					menuPtr->preFocusItemIdx = menuPtr->curFocusItemIdx;
					break;	

				default:
					recode = MMI_RESULT_FALSE;
					break;
				}	
			}
			break;
			
		case MRAPP_WINTYPE_APP:
			{
				switch(msg_id)
				{
				case MSG_OPEN_WINDOW:
				case MSG_FULL_PAINT:
					break;
				case MSG_CLOSE_WINDOW:
					MMIMRAPP_freeDynWinData(addData);
					break;
#ifdef MR_HANDSET_IS_SUPPORT_TOUCHPANEL
				case MSG_TP_PRESS_DOWN:
					recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_DOWN, (void*)param);
					break;
				case MSG_TP_PRESS_UP:
					recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_UP, (void*)param);
					break;
//2010-2-24 add move & update MRP_PORT_VER to 5
				case MSG_TP_PRESS_MOVE:
					recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_MOVE, (void*)param);
					break;
//2010-2-24 add end
#endif
				case MSG_KEYPRESSUP_RED:
				case MSG_KEYDOWN_RED:
					if(s_mr_fbhup2idle != 1)	
						recode = MMI_RESULT_FALSE;
					break;
				default:
					recode = ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
					break;
				}	
			}
			break;
            
		default:
			MMIMR_Trace(("error para: %d",addData->dynWinType));
			recode = MMI_RESULT_FALSE;
			break;
		}
	}
	
	MMIMR_Trace(("MMIMRAPP_HandleDynWinMsg() win:%d, msg_id: 0x%x, recode :%d", win_id, msg_id, recode));
	return recode;	
}

/*****************************************************************************/
// 	Description : get according icon of a item
//	Global resource dependence : none
//  Author: qgp
//	Note: item icon 数目依赖平台定义了的数目，当前为20
/*****************************************************************************/
MMI_IMAGE_ID_T MMIMRAPP_getItemIcon(MMIMRAPP_MENUICON_OP_TYPE_E opType, uint16 index)
{
	MMI_IMAGE_ID_T imageId;
	uint16 fixIdx;
	MMIMR_Trace(("MMIMRAPP_getItemIcon index: %d",index));

	/*item icon 数目依赖平台定义了的数目，当前为20*/
	fixIdx = index%20;

	switch(fixIdx)
	{
	case 0:
		imageId = IMAGE_NUMBER_1;
		break;
	case 1:
		imageId = IMAGE_NUMBER_2;
		break;
	case 2:
		imageId = IMAGE_NUMBER_3;
		break;
	case 3:
		imageId = IMAGE_NUMBER_4;
		break;
	case 4:
		imageId = IMAGE_NUMBER_5;
		break;
    case 5:
		imageId = IMAGE_NUMBER_6;
		break;
	case 6:
		imageId = IMAGE_NUMBER_7;
    	break;
	case 7:
		imageId = IMAGE_NUMBER_8;
		break;
	case 8:
		imageId = IMAGE_NUMBER_9;
		break;
	case 9:
		imageId = IMAGE_NUMBER_10;
		break;
	case 10:
		imageId = IMAGE_NUMBER_11;
		break;
	case 11:
		imageId = IMAGE_NUMBER_12;
		break;
	case 12:
		imageId = IMAGE_NUMBER_13;
		break;
	case 13:
		imageId = IMAGE_NUMBER_14;
		break;
	case 14:
		imageId = IMAGE_NUMBER_15;
		break;
	case 15:
		imageId = IMAGE_NUMBER_16;
		break;
	case 16:
		imageId = IMAGE_NUMBER_17;
		break;
	case 17:
		imageId = IMAGE_NUMBER_18;
		break;
	case 18:
		imageId = IMAGE_NUMBER_19;
		break;
	case 19:
		imageId = IMAGE_NUMBER_20;
		break;
	default:
		imageId = IMAGE_NUMBER_1;
		break;
	}

	MMIMR_Trace(("MMIMRAPP_getItemIcon imageId: %d",imageId));
	return imageId;
}

/*****************************************************************************/
// 	Description : process the key event
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E ProcessMrAppKeyMsg(
									  MMIMRAPP_KEYTYPE_E kType,
									  void *info
									  )
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	MMI_MESSAGE_ID_E  msg_id;
	int16 subType = 0;
	int32 key = -1;
	int32 mrResult = MR_FAILED;
	MMIMR_Trace(("mrapp. ProcessMrAppKeyMsg() kType:%d, p?: %d", kType, s_mr_vmPaused));

	switch(kType)
	{
	case MMIMRAPP_KEY_NORMAL:
		msg_id = *(MMI_MESSAGE_ID_E*)info;
        MMIMR_Trace(("mrapp. keyid:0x%04x, KeyTableLen: %d", msg_id, ARR_SIZE(s_KeyTable)));
	
		if(msg_id == MSG_KEYDOWN_SDCARD_DETECT)
        {
			mr_event(MR_TCARD_EVENT, MR_TCARD_PLUGIN, 0);
		}
        else if(msg_id == MSG_KEYUP_SDCARD_DETECT)
        {
			mr_event(MR_TCARD_EVENT, MR_TCARD_PLUGOUT, 0);
		}
		/* add by zack@20110907 start */
        else if(msg_id == MSG_KEYDOWN_HEADSET_DETECT || msg_id == MSG_KEYUP_HEADSET_DETECT)
        {
            recode = MMI_RESULT_FALSE;
        }
        /* add by zack@20110907 end */
        else
        {
            if((msg_id >= MSG_KEYPRESSUP) && (msg_id - MSG_KEYPRESSUP < ARR_SIZE(s_KeyTable)))
			{
				msg_id = msg_id - MSG_KEYPRESSUP;
				/*add by zack@20120902 start*/
#ifdef KEYPAD_TYPE_QWERTY_KEYPAD
        msg_id = MMIDEFAULT_ConvertQwertyKeyToTypicalKey(msg_id);
#endif
                /*add by zack@20120902 end*/
				if(s_mr_keyDown == TRUE)
				{
					key = s_KeyTable[msg_id];
					subType = MR_KEY_RELEASE;
					s_mr_keyDown = FALSE;
				}
			}
			else if((msg_id >= MSG_KEYDOWN) && (msg_id - MSG_KEYDOWN < ARR_SIZE(s_KeyTable)))
			{
				msg_id = msg_id - MSG_KEYDOWN;
				/*add by zack@20120902 start*/
#ifdef KEYPAD_TYPE_QWERTY_KEYPAD
        msg_id = MMIDEFAULT_ConvertQwertyKeyToTypicalKey(msg_id);
#endif
                /*add by zack@20120902 end*/
				if(s_mr_keyDown == FALSE)
				{
					key = s_KeyTable[msg_id];
					subType = MR_KEY_PRESS;
					s_mr_keyDown = TRUE;
				}
			}
            else if((msg_id >= MSG_KEYUP) && (msg_id - MSG_KEYUP < ARR_SIZE(s_KeyTable)))
			{
				msg_id = msg_id - MSG_KEYUP;
				/*add by zack@20120902 start*/
#ifdef KEYPAD_TYPE_QWERTY_KEYPAD
        msg_id = MMIDEFAULT_ConvertQwertyKeyToTypicalKey(msg_id);
#endif
                /*add by zack@20120902 end*/
				if(s_mr_keyDown == TRUE)
				{
					key = s_KeyTable[msg_id];
					subType = MR_KEY_RELEASE;
					s_mr_keyDown = FALSE;
				}
			}
			
			if(key != -1)
			{
				mrResult = mr_event(subType, key, 0);
			}
		}
		break;
		
#ifdef MR_HANDSET_IS_SUPPORT_TOUCHPANEL
	case MMIMRAPP_MOUSE_DOWN:
		if(info != NULL)
		{
			if(s_mr_keyDown == FALSE)
			{
				GUI_POINT_T TpPoint = {0};
				s_mr_keyDown = TRUE;
				TpPoint.x = MMK_GET_TP_X(info);
				TpPoint.y = MMK_GET_TP_Y(info);
				MMIMR_Trace(("down x: %d, y: %d", TpPoint.x, TpPoint.y));
				mrResult = mr_event(MR_MOUSE_DOWN, (int32)TpPoint.x, (int32)TpPoint.y);
			}
		}
		break;
		
	case MMIMRAPP_MOUSE_UP:
		if(info != NULL)
		{
			if(s_mr_keyDown == TRUE)
			{
				GUI_POINT_T TpPoint = {0};
				s_mr_keyDown = FALSE;
				TpPoint.x = MMK_GET_TP_X(info);
				TpPoint.y = MMK_GET_TP_Y(info);
				MMIMR_Trace(("up x: %d, y: %d", TpPoint.x, TpPoint.y));
				mrResult = mr_event(MR_MOUSE_UP, (int32)TpPoint.x, (int32)TpPoint.y);
			}
		}
		break;

//2010-2-24 add move & update MRP_PORT_VER to 5
	case MMIMRAPP_MOUSE_MOVE:
		if(info != NULL)
		{
			if(s_mr_keyDown == TRUE)
			{
				GUI_POINT_T TpPoint = {0};
				TpPoint.x = MMK_GET_TP_X(info);
				TpPoint.y = MMK_GET_TP_Y(info);
				MMIMR_Trace(("move x: %d, y: %d", TpPoint.x, TpPoint.y));
				mrResult = mr_event(MR_MOUSE_MOVE, (int32)TpPoint.x, (int32)TpPoint.y);
			}
		}
        break;
//2010-2-24 add end
#endif		
		
	case MMIMRAPP_MENU_SELECT:	
		s_mr_keyDown = FALSE;
		mrResult = mr_event(MR_MENU_SELECT, *(int32*)info, 0);
		break;
		
	case MMIMRAPP_MENU_RETURN:	
		s_mr_keyDown = FALSE;
		mrResult = mr_event(MR_MENU_RETURN, 0, 0);
		break;
		
	case MMIMRAPP_DIALOG_SELECT:
		s_mr_keyDown = FALSE;
		mrResult = mr_event(MR_DIALOG_EVENT, MR_DIALOG_KEY_OK, 0);
		break;
		
	case MMIMRAPP_DIALOG_RETURN:
		s_mr_keyDown = FALSE;
		mrResult = mr_event(MR_DIALOG_EVENT, MR_DIALOG_KEY_CANCEL, 0);
		break;

	default:
		break;
	}

	MMIMR_Trace(("_mrResult_ : %d, key: 0x%x", mrResult, key));
		
	if(mrResult != MR_SUCCESS || key == MR_KEY_NONE)
	{
		recode = MMI_RESULT_FALSE;
	}
	
	return recode;
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
MMIMRAPP_DIALOG_PARAMETERS_T*  MMIMRAPP_GetPromptWinAddDataPtr(MMI_WIN_ID_T win_id)
{
	MMIMRAPP_DIALOG_PARAMETERS_T *cur, *find=NULL;
	
	cur = s_mr_dialogWinList;
	while(cur != NULL)
	{
		if(cur->winId == win_id)
		{
			find = cur;
			break;
		}
		else
		{
			cur = cur->next;	
		}
	}

	return find;
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL MMI_RESULT_E  MMIMRAPP_HandlePromptWinMsg(
												 MMI_WIN_ID_T win_id, 
												 MMI_MESSAGE_ID_E msg_id, 
												 DPARAM param
												 )
{
	BOOLEAN recode = MMI_RESULT_TRUE;
	MMIMRAPP_DIALOG_PARAMETERS_T *addData = (MMIMRAPP_DIALOG_PARAMETERS_T*)MMIMRAPP_GetPromptWinAddDataPtr(win_id);
	MRAPP_DIALOG_DATA *dialog;

	MMIMR_Trace(("mrapp. MMIMRAPP_HandlePromptWinMsg msg_id :0x%x, addData:0x%x", msg_id, addData));

	if(addData == NULL)
	{	
		return MMI_RESULT_FALSE;
	}
		
	dialog = (MRAPP_DIALOG_DATA *)&addData->dialog;
	switch(msg_id)
	{
	case MSG_FULL_PAINT:
		{
			MMI_TEXT_ID_T 	left_sk_id = TXT_NULL;
			MMI_TEXT_ID_T 	right_sk_id = TXT_NULL;

			if(dialog->type == MR_DIALOG_OK)
			{
				left_sk_id = TXT_COMMON_OK;
			}
			else if(dialog->type == MR_DIALOG_OK_CANCEL)
			{
				left_sk_id = TXT_COMMON_OK;
				right_sk_id = STXT_CANCEL;
			}
			else if(dialog->type != MR_DIALOG_NOKEY)
			{
				right_sk_id = STXT_RETURN;
			}
			
			s_mr_inMrSelfWinCreating = TRUE;
			MMIPUB_SetWinSoftkey(win_id, left_sk_id, right_sk_id, FALSE);
			/* modify by zzh@20100112 start */
			/* 解决版本升级时频繁更新下载进度重启的问题。
			   AlertWin处理MSG_FULL_PAINT消息时会重新创建自动关闭AlertWin的定时器，
			   移植层创建的AlertWin不需要自动关闭故FULL_PAINT时只刷新窗口内容即可。*/
			//MMIPUB_DisplayPubWin(win_id);
			recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
			/* modify by zzh@20100112 end */
			s_mr_inMrSelfWinCreating = FALSE;
		}
		break;

	case MSG_CLOSE_WINDOW:
		{
    		MMIMRAPP_DIALOG_PARAMETERS_T *cur, *pre;
    				
    		MMIMRAPP_OpDynId(MRAPP_WINTYPE_DIALOG, addData->winId, MRAPP_ID_WIN);//free id
    		/*delete the node from list*/
    		if(addData == s_mr_dialogWinList)
    		{
    			s_mr_dialogWinList = addData->next;
    		}
    		else
    		{
    			cur = s_mr_dialogWinList->next;
    			pre = s_mr_dialogWinList;
    			while(cur != NULL)
    			{
    				if(cur == addData)
    				{
    					pre->next = addData->next;
    					break;
    				}
    				else
    				{
    					pre = cur;
    					cur = cur->next;	
    				}
    			}
    		}
    		SCI_FREE(addData);
    		MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
		}
		break;
		
	case MSG_GET_FOCUS:
		//刷新背景窗口
		MMK_SendMsg(MMK_GetPrevWinId(win_id), MSG_FULL_PAINT, PNULL);
		MMITHEME_SetUpdateDelayCount(1);
		MMIMRAPP_resumeApp();
		break;

	//new ver del. case MSG_TIMER://pass timer
	//	break;
		
	case MSG_LOSE_FOCUS:
		MMIMRAPP_pauseApp();
		break;	
		
	case MSG_APP_OK:
    case MSG_CTL_OK:
	case MSG_CTL_PENOK:
	case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
#ifdef MMI_PDA_SUPPORT
	case MSG_APP_MENU:
#endif
		if(dialog->type == MR_DIALOG_OK || dialog->type == MR_DIALOG_OK_CANCEL)
			ProcessMrAppKeyMsg(MMIMRAPP_DIALOG_SELECT, NULL);
		break;
		
	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		if(dialog->type == MR_DIALOG_CANCEL || dialog->type == MR_DIALOG_OK_CANCEL)
			ProcessMrAppKeyMsg(MMIMRAPP_DIALOG_RETURN, NULL);
		break;
        
	case MSG_KEYPRESSUP_RED:
	case MSG_KEYDOWN_RED:
		if(s_mr_fbhup2idle != 1)	
			recode = MMI_RESULT_FALSE;
		break;
        
	default:
        //modify by zack@20100628 本地UI对话框普通按键事件不需要传给VM
#ifdef MRAPP_SIMUGAME_OPEN
        if(MMIMRAPP_GetCurApp() == MRAPP_NESGAME && dialog->type == MR_DIALOG_CANCEL)
        {
    		ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
        }
#endif
        //modify by zack@20110907 NES模拟器自定义按键时需要将所有按键事件传给MRP
		recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
		break;
	}		

	MMIMR_Trace(("MMIMRAPP_HandlePromptWinMsg() recode :%d, event: 0x%x", recode, msg_id));
	return recode;	
}

/*****************************************************************************/
//  Description : handle file browser win message
//  Global resource dependence :
//  Author: zack.zhang
//  Note:modify by zack@20120415
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIMRAPP_HandleFileBrowserWinMsg(
        													 MMI_WIN_ID_T          win_id,
        													 MMI_MESSAGE_ID_E      msg_id,
        													 DPARAM                param
        													 )
{
	int32 len = 0;
    int32 ret = 0;
    MMI_RESULT_E       result = MMI_RESULT_TRUE;
    FILEARRAY_DATA_T   file_data = {0};
	static int32 isSelecet = 0;

	MMIMR_Trace(("MMIMRAPP_HandleFileBrowserWinMsg win_id = %d, msg_id = 0x%04x isSelecet=%d s_mr_vmPaused=%d",win_id, msg_id, isSelecet, s_mr_vmPaused));
    switch(msg_id)
    {
	case MSG_OPEN_WINDOW:
		isSelecet = 0;
		result = MMI_RESULT_FALSE;
		break;

    case MSG_APP_OK:
    case MSG_CTL_OK:
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
		isSelecet = 1;
        MMIFMM_GetOpenFileWinCurFile(&file_data); /*lint !e718 !e18*/
        MMIMR_Trace(("MMIMRAPP_HandleFileBrowserWinMsg len = %d",file_data.name_len));
		/*modify by zack@20120816 start*/
        if (file_data.type == FILEARRAY_TYPE_FOLDER)
		{
			ret = MMI_RESULT_FALSE;
		}
        else
		/*modify by zack@20120816 end*/
        if(file_data.name_len > 0)
        {
            SCI_MEMSET(g_mr_filebrowser_path, 0x00, sizeof(g_mr_filebrowser_path));
            len = GUI_WstrToGB(g_mr_filebrowser_path, file_data.filename, file_data.name_len);
			MMIMR_Trace(("len = %d path = %s", len, g_mr_filebrowser_path));
            //MMK_CloseWin(win_id);
			MMK_SendMsg(win_id, MSG_FMM_CLOSE_LAYER_WIN_BY_CLIENT, NULL);
            ret = mr_event(MR_LOCALUI_EVENT, MR_DIALOG_KEY_OK, 0);
        }
		else
		{
			//MMK_CloseWin(win_id);
			MMK_SendMsg(win_id, MSG_FMM_CLOSE_LAYER_WIN_BY_CLIENT, NULL);
            ret = mr_event(MR_LOCALUI_EVENT, MR_DIALOG_KEY_CANCEL, 0);
		}
        break;

	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		//MMK_CloseWin(win_id);
		MMK_SendMsg(win_id, MSG_FMM_CLOSE_LAYER_WIN_BY_CLIENT, NULL);
        ret = mr_event(MR_LOCALUI_EVENT, MR_DIALOG_KEY_CANCEL, 0);
        break;

	case MSG_CLOSE_WINDOW:
		if(isSelecet == 0)
		{
			if(s_mr_vmPaused)
            {
                MMIMRAPP_resumeApp();
            }         
			ret = mr_event(MR_LOCALUI_EVENT, MR_DIALOG_KEY_CANCEL, 0);
		}
		result = MMI_RESULT_FALSE;
		break;
        
    default:
		result = MMI_RESULT_FALSE;
        //result = MMIFMM_HandleOpenFileByLayerWin(win_id, msg_id, param); /*lint !e718 !e18*/
        break;
    }

	MMIMR_Trace(("MMIMRAPP_HandleFileBrowserWinMsg Exit"));
    return result;
}


/*****************************************************************************/
// 	Description :创建一dialog win
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
int32 MMIMRAPP_createPromptWin(MMIMRAPP_DIALOG_PARAMETERS_T *addData, MMI_STRING_T *textInfo)
{
	int32		winId = -1;
	BOOLEAN		result = FALSE;
	MMIMRAPP_DIALOG_PARAMETERS_T *cur, *pre;
	MMI_IMAGE_ID_T imgId;
	uint32 time_period = 0xffffff;
	
	MMIMR_Trace(("mrapp. MMIMRAPP_createPromptWin()"));
	
	winId = MMIMRAPP_OpDynId(MRAPP_WINTYPE_DIALOG, -1, MRAPP_ID_WIN);//get id
	if(winId == -1)
	{
		return MR_FAILED;
	}	
	addData->winId = winId;

	if(addData->dialog.type == MR_DIALOG_OK_CANCEL)
	{
		imgId = IMAGE_PUBWIN_QUERY;
	}
	else
	{
		imgId = IMAGE_PUBWIN_WARNING;
	}
	s_mr_inMrSelfWinCreating = TRUE;
	MMIPUB_OpenAlertWinByTextPtr(&time_period, textInfo, NULL, imgId, &addData->winId, NULL, MMIPUB_SOFTKEY_CUSTOMER, (MMIPUB_HANDLE_FUNC)MMIMRAPP_HandlePromptWinMsg);		
	s_mr_inMrSelfWinCreating = FALSE;//move to it handle as some latest sw the pop is asyn created
	MMIMR_Trace(("MMIPUB_OpenAlertWinByTextPtr(), result: %d, id: %d", result, addData->winId));
    //MMIPUB_SetAlertWinTextByPtrEx(addData->winId, textInfo, TRUE);
	cur = s_mr_dialogWinList;
	pre = s_mr_dialogWinList;
	while(cur != NULL)
	{
		pre = cur;
		cur = cur->next;
	}
	
	if(pre == NULL)
	{
		s_mr_dialogWinList = addData;
	}
	else
	{
		pre->next = addData;
	}

	return addData->winId;
}

/*****************************************************************************/
// 	Description : init local resource
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL  void MMIMRAPP_initLocalRes(void)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_initLocalRes()"));
	MMIMRAPP_setDefFilePath();
	MMIMRAPP_timerStop(0);	
	MMIMRAPP_stopShake();
	MMIMRAPP_gprsProtectTimerStop();
}


/* begin:modified by Tommy.yan 20120222 */
/*****************************************************************************/
// 	Description : get pb info back
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
LOCAL void MMIMRAPP_pbInfoReturn(MMIPB_HANDLE_T handle)
{
	MMIPB_BCD_NUMBER_T	bcd_number = {0};
	uint8	tele_len = 0;
	uint8	temp_num[MMIPB_BCD_NUMBER_MAX_LEN*2 + 2]= {0};
	wchar   pb_name_str[MR_PHB_MAX_NAME + 1] = {0};
	wchar	pb_email_str[MR_PHB_MAX_NUMBER + 1] = {0};
	MMI_STRING_T content = {0};
	uint16 num_index = 0;
	MMIPB_CONTACT_T contact_ptr = {0};
	
	if(g_mr_pbinfo != NULL)
	{
		SCI_MEMSET(g_mr_pbinfo, 0, sizeof(MRAPP_PBINFO_T));
		g_mr_pbinfo->result = MR_SUCCESS;
		
		if(MMIAPIPB_GetSelectCount(handle) > 0)
		{
			/*get number*/
			if (MMIAPIPB_GetSelectNumber(handle, &bcd_number, MMIPB_BCD_NUMBER_MAX_LEN, 0))
			{
				tele_len = MMIAPICOM_GenDispNumber( 
						MMIPB_GetNumberTypeFromUint8(bcd_number.npi_ton), 
						(uint8)MIN(bcd_number.number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
						bcd_number.number,  
						temp_num,
						(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
					);
				MMI_STRNTOWSTR(g_mr_pbinfo->info.number, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/
			}
				
			/*get name*/
			content.wstr_ptr= pb_name_str;
			MMIAPIPB_GetSelectName(handle, &content, MR_PHB_MAX_NAME, 0);
			MMIAPICOM_Wstrcpy((uint16*)g_mr_pbinfo->info.name, content.wstr_ptr);
				
			/*get email*/
			content.wstr_ptr= pb_email_str;
			MMIAPIPB_GetSelectEmail(handle, &content, MR_PHB_MAX_NUMBER, 0);
			MMIAPICOM_Wstrcpy((uint16*)g_mr_pbinfo->info.emailAddress, content.wstr_ptr);
		}
	}
	else
	{
		g_mr_pbinfo->result = MR_FAILED;
	}

	g_mr_pbinfo->haveBack = 1;
    /*modify by zack@20110921 start*/
    /*
     *把调用mr_event放到BaseWin获得焦点的地方，否则动态横竖屏切换时会有花屏现象
     */
	//mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_OK, 0);
	g_mr_pbinfo->needInform = TRUE;
	s_mr_PBOfCancelMsg = FALSE;

    /*modify by zack@20110921 end*/
}
/* end:modified by Tommy.yan 2012022 */

/*****************************************************************************/
// 	Description : stop the vibrator timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_stopShake(void)
{
	if(s_mr_vibra_timer > 0)
	{
		MMK_StopTimer(s_mr_vibra_timer);
		s_mr_vibra_timer = 0;
		MMIAPISET_StopVibrator(MMISET_RING_TYPE_OTHER);
	}
}

/*****************************************************************************/
// 	Description : start a timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_timerStart(uint16 t)
{
	MMIMR_TraceT(("mrapp. MMIMRAPP_timerStart() t: %d", t));

	if(!s_mr_allReady)
	{
		MMIMR_TraceT(("mrapp.timer not ready !!!"));
		return 0;
	}
	if (s_mrTimer != 0)
	{
		MMK_StopTimer(s_mrTimer);
	}
	s_mrTimer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, t, FALSE);

	MMIMR_TraceT(("mrapp. return s_mrTimer: %d", s_mrTimer));
	return s_mrTimer;
}

/*****************************************************************************/
// 	Description : start a vibrator
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_startShake(int32 t)
{
    MMIMR_Trace(("mrapp. MMIMRAPP_startShake() t: %d", t));

#ifdef __MMI_SKYQQ__
	if(!s_mr_allReady && GetSkyQQRunState() != QQ2008_BACK_RUN)
#else
    if(!s_mr_allReady)
#endif
	{
		return 0;
	}
	
	MMIMRAPP_stopShake();
	s_mr_vibra_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, t, FALSE);
	if(s_mr_vibra_timer != 0)
	{
		MMIAPIENVSET_StartVibrator(MMISET_RING_TYPE_OTHER);
	}

	return s_mr_vibra_timer;
}

/*****************************************************************************/
// 	Description : stop the timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_timerStop(int32 t)
{
	MMIMR_TraceT(("mrapp. MMIMRAPP_timerStop() s_mrTimer: %d", s_mrTimer));

	if (s_mrTimer > 0)
	{
		MMK_StopTimer(s_mrTimer);
		s_mrTimer = 0;
	}
}

/*****************************************************************************/
// 	Description : start a timer for gprs activing timeout protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_gprsProtectTimerStart(uint16 t)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_gprsProtectTimerStart() t: %d", t));
	
	if(!s_mr_allReady)
	{
		return 0;
	}
	
	if (s_mr_gprsProtectTimer > 0)
	{
		MMK_StopTimer(s_mr_gprsProtectTimer);
	}

	s_mr_gprsProtectTimer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, t, FALSE);

	MMIMR_Trace(("mrapp. s_mr_gprsProtectTimer: %d", s_mr_gprsProtectTimer));
	return s_mr_gprsProtectTimer;
}

/*****************************************************************************/
// 	Description : stop the protect timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_gprsProtectTimerStop(void)
{
	if (s_mr_gprsProtectTimer > 0)
	{
		MMK_StopTimer(s_mr_gprsProtectTimer);
		s_mr_gprsProtectTimer = 0;
	}
}

/*****************************************************************************/
// 	Description : start a timer for gprs activing timeout protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_gprsRetryTimerStart(uint16 t)
{	
	if(!s_mr_allReady)
    {
		return 0;
	}
	
	if (s_mr_gprsRetryTimer > 0)
    {
		MMK_StopTimer(s_mr_gprsRetryTimer);
	}

	s_mr_gprsRetryTimer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, t, FALSE);
	MMIMR_Trace(("mrapp. MMIMRAPP_gprsRetryTimerStart: %d", s_mr_gprsRetryTimer));
	return s_mr_gprsRetryTimer;
}

/*****************************************************************************/
// 	Description : stop the protect timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_gprsRetryTimerStop(void)
{
	if (s_mr_gprsRetryTimer > 0)
    {
		MMK_StopTimer(s_mr_gprsRetryTimer);
		s_mr_gprsRetryTimer = 0;
	}
}

/*****************************************************************************/
// 	Description : set all the resource state
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_setAllReady(BOOLEAN ready)
{
	s_mr_allReady = ready;
}

/*****************************************************************************/
// 	Description : pause mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:  
//comment. 分为一下情况:
// 1。被mr app自己开的窗体覆盖:则不调用mr_pauseApp(),应该由app自己管理暂停那事件。
// 2。被其他窗体(如call)覆盖:则调用mr_pauseApp(),
/*****************************************************************************/
int32 MMIMRAPP_pauseApp(void)
{
	int32 res;
	MMIMR_Trace(("mrapp. MMIMRAPP_pauseApp() : %d, %d, %d, %d", s_mr_vmPaused, s_mrTimer, s_mr_inMrSelfWinCreating, s_mr_VMReady));

	if(s_mr_VMReady)
	{
		if(s_mr_keyDown == TRUE)
		{
			s_mr_keyDown = FALSE;//current mr win have been covered, so reset the key event flag
		}

		if(!s_mr_inMrSelfWinCreating && !s_mr_vmPaused)
		{
            MMIMRAPP_PauseAudioPlayer(); /*lint !e718 !e18*/ //add for mpmusic 2010-07-07  
			res = mr_pauseApp();
			MMIMR_Trace(("paused! res: %d, %d", res, s_mrTimer));
			s_mr_vmPaused = TRUE;
			MMIMRAPP_StopPlaySoundProtect();//2008-3-14 add, fix sound error after get a call .
			return res;
		}
		else
		{
			return MR_SUCCESS;
		}
	}

	return MR_IGNORE;
}

/*****************************************************************************/
// 	Description : resume mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
int32 MMIMRAPP_resumeApp(void)
{
	int32 res;
	
	MMIMR_Trace(("mrapp. MMIMRAPP_resumeApp() :%d, %d, %d", s_mr_vmPaused, s_mrTimer, s_mr_VMReady));	
    
	if(s_mr_VMReady)
	{
		if(s_mr_vmPaused)
		{
#ifdef MRAPP_SUPPORT_CLKADAPT
			MMIMR_Trace(("mrapp. pre clk:%d, cur:%d", s_mr_cpuClk_mng.curCpuClk, HAL_GetMCUClk()));
			if(s_mr_cpuClk_mng.curCpuClk != HAL_GetMCUClk())
			{
				MMIMRAPP_SetArmClkHigh();
			}
#endif
			res = mr_resumeApp();
            MMIMRAPP_ResumeAudioPlayer(); /*lint !e718 !e18*/ //add for mpmusic 2010-07-07  
			MMIMR_Trace(("resume! res: %d, %d", res, s_mrTimer));
			s_mr_vmPaused = FALSE;
			return res;
		}
		else
		{
			if(g_mr_pbinfo !=NULL && g_mr_pbinfo->haveBack == 0)
			{
				g_mr_pbinfo->haveBack = 1;
				mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_CANCEL, 0);
			}
			return MR_SUCCESS;
		}
	}

	return MR_IGNORE;
}

/*****************************************************************************/
// 	Description : set arm clk to changed
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
BOOLEAN MMIMRAPP_CLKSetted(BOOLEAN setted, uint32 curCpuClkParam, uint32 curCpuClk)
{
	BOOLEAN old = s_mr_cpuClk_mng.setted;
	
	s_mr_cpuClk_mng.setted = setted;
	s_mr_cpuClk_mng.curCpuClkParam = curCpuClkParam;
	s_mr_cpuClk_mng.curCpuClk = curCpuClk;

	return old;
}

#ifdef MR_BINDTO_VIRTUAL_WIN
/*****************************************************************************/
//	Description : 
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_ifMrTimer(DPARAM param)
{
	BOOLEAN ret = TRUE;
	MMIMR_TraceT(("MMIMRAPP_ifMrTimer() param:%d", param));
	
	if(s_mrTimer == *(uint8*)param)
	{
		MMIMRAPP_timerStop(0);
		mr_timer();
		
#ifdef MRAPP_TIMERWATCH_DOG	
		if(s_mr_protectTimer.mrTimerStarted == TRUE && !MMK_IsTimerActive(s_mrTimer))
		{
			MMIMR_Trace(("s_mrTimer evn lost!!"));
			MMK_StartWinTimer(VIRTUAL_WIN_ID, s_mrTimer, s_mr_protectTimer.timerOut, FALSE);
		}
#endif
	}

	/*wuwenjie 2011-12-01 START*/
	else if(mr_getHostTimerId()== *(uint8*)param)
	{
		mr_gethostTimerDeal();
	}
	/*wuwenjie 2011-12-01 END*/
	else if(s_mr_mrPlaySound_timer == *(uint8*)param)
	{
		MMIMRAPP_playingCheck();
	}
	else if(s_mr_AccessUrl_timer == *(uint8*)param)
	{
		MMIMRAPP_AccessUrl();
	}
	else if(s_mr_vibra_timer == *(uint8*)param)
	{
		MMIMRAPP_stopShake();
	}
	else if(s_mr_gprsProtectTimer == *(uint8*)param)
	{
		MMIMRAPP_gprsProtectTimerStop();
		MMIMRAPP_netInitResult(MR_FAILED);
	}
	else if(s_mr_gprsRetryTimer == *(uint8*)param)
	{
		MMIMRAPP_gprsRetryTimerStop();
		MMIMRAPP_redail();
	}
//2009-7-7 add begin
#ifdef __MRAPP_OVERSEA__
	else if (s_mr_getApnAccount_timer == *(uint8*)param)
    {
		MMIMRAPP_getApnAccountTimerStop();
		MMIMRAPP_getApnAccountRet(TRUE);
	}
    else if(s_mr_setApnAccount_timer == *(uint8*)param)
    {
        MMIMRAPP_setApnAccountTimerStop();
        MMIMRAPP_setApnAccountRet(); /*lint !e718 !e18*/
    } 
#endif
//2009-7-7 add end
	else if (s_mr_getSCNumber_timer == *(uint8*)param)
    {
		MMIMRAPP_GetSCNumberTimerStop();
		MMIMRAPP_GetSCNumberRet(); /*lint !e718 !e18*/
	}
	/* begin:added by Tommy.yan 20120120 */
	else if (s_mr_getSMSCapacity_timer == *(uint8*)param)
	{
		MMIMRAPP_GetSMSCapacityTimerStop();
		MMIMRAPP_GetSMSCapacityCallBack(); /*lint !e718 !e18*/
	}

	else if (s_mr_getSMSContent_timer == *(uint8*)param)
	{
		MMIMRAPP_GetSMSContentTimerStop();
		MMIMRAPP_GetSMSContentCallBack(); /*lint !e718 !e18*/
	}

	else if (s_mr_deleteSMS_timer == *(uint8*)param)
	{
		MMIMRAPP_DeleteSMSTimerStop();
		MMIMRAPP_DeleteSMSCallBack(); /*lint !e718 !e18*/
	}

	else if(s_mr_setSMSState_timer == * (uint8*)param)
	{
		MMIMRAPP_SetSMSStateTimerStop();
		MMIMRAPP_SetSMSStateCallBack(); /*lint !e718 !e18*/
	}

	else if (s_mr_phb_operate_timer == *(uint8*)param)
	{
		MMIMRAPP_StopPHBOperateTimer();
		//MMIMRAPP_PHBCallBack(); /*lint !e718 !e746*/
	}
	else if(s_mr_phb_search_timer == *(uint8*)param)
	{
		MMIMRAPP_PHBSearchCallBack(); /*lint !e746*/
	}
	/* end:added by Tommy.yan 20120120 */	
    else if (s_mr_statusIcon_timer == *(uint8*)param)
    {
		MMIMRAPP_HandleStatusIconTimer();
	}
#ifdef __MRAPP_MOTION_SUPPORT__
    else if(mr_motion_handle_timer(*(uint8*)param))
    {
        MMIMR_Trace(("[Motion] timer "));
    }
#endif
#ifdef __MMI_SKYQQ__
	else if (QQ2008_Timer(param))
	{
		SCI_TRACE_LOW("VirWin_HandleMsg: received qq2008 timer");
	}
#endif
	else 
	{
		ret = FALSE;
	}

	return ret;
}
#endif

/*****************************************************************************/
//	Description : 
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIMRAPP_handleMrMsg(MMI_MESSAGE_ID_E msg_id, DPARAM param)
{ /*lint !e18*/
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMIMR_Trace(("mrapp. MMIMRAPP_handleMrMsg msg_id=%d", msg_id));
#ifdef MR_BINDTO_VIRTUAL_WIN
	switch(msg_id)
	{
		case MSG_TIMER:
			MMIMRAPP_ifMrTimer(param);
			break;
		case MMRAPP_MSG_NETINIT_RESULT:
			MMIMR_Trace(("mrapp. MMRAPP_MSG_NETINIT_RESULT"));
			if(param != NULL)
			{
				MMIMRAPP_netInitResult(((MMIMRAPP_NETINIT_RESULT_T*)param)->result);
				MMIMR_Trace(("mrapp. net init result return."));
			}
			break;
		case MMRAPP_MSG_NETREDAIL_IND:
			MMIMR_Trace(("mrapp. MMRAPP_MSG_NETREDAIL_IND."));
			MMIMRAPP_redail();
			break;
		case MMRAPP_MSG_SYS_EXIT:	
			MMIMR_Trace(("mrapp. MMRAPP_MSG_SYS_EXIT."));
			MMIMRAPP_exitMrApp(FALSE);
			break;

#ifdef __MMI_SKYQQ__
        case QQ2008_MSG_NETINIT_RESULT:
            MMIMR_Trace(("QQ2008. QQ2008_MSG_NETINIT_RESULT"));
			if(param != NULL)
			{
				QQ2008_netInitResult(((MMIMRAPP_NETINIT_RESULT_T*)param)->result);
				MMIMR_Trace(("QQ2008. net init result return."));
			}
            break;
#endif


        case APP_AUDIO_END_IND:
            MMIMRAPP_HandleAudioPlayCBMsg((MmiAudioCallbackMsgS *)param);
            break;

        case MMISMS_MSG_DELETE_NEXT:
            MMIMRAPP_DeleteSMSCallBack(param);
            break;

             /*热拔T卡的情况下提示用户并强制退出VM*/
        case MSG_HANDLE_SD_ERR:
            if(MMIMRAPP_mrappIsActived())
            {
                MMIPUB_OpenAlertWarningWin(TXT_COMMON_SD_FAIL);
                MMK_ReturnIdleWin();
            }
            break;
        
		default:
			result = MMI_RESULT_FALSE;
			break;
	}
	
#endif
	return result;
}

/*****************************************************************************/
// 	Description : free mr mmi res
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_freeMmiRes(void)
{
	MMIMR_Trace(("MMIMRAPP_freeMmiRes()"));
	MMIMRAPP_initLocalRes();
	MMIMRAPP_CloseMrNet();
	MMIMRAPP_freeAllExtraSrc();
#ifdef MR_BINDTO_VIRTUAL_WIN
	g_mr_vmBackRun = MRAPP_MAX;
#endif

#ifdef MRAPP_SUPPORT_CLKADAPT
	MMIMR_Trace(("#######HAL_GetMCUClk1: %d", HAL_GetMCUClk()));
	if(MMIMRAPP_CLKSetted(FALSE, 0, 0))
	{
		MMIMRAPP_RestoreARMClk();
	}
	MMIMR_Trace(("#######HAL_GetMCUClk2: %d", HAL_GetMCUClk()));
#endif
    MMISET_SetIsSensorOpenFlag(s_mr_gsensor_flag);
	s_mr_allReady = FALSE;
	
	MMIMR_Trace(("MMIMRAPP_freeMmiRes() out"));
}	

/*****************************************************************************/
// 	Description : exit vm ,because use mr_stop() in this function, so if trigger by mr app, must use Asyn
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_exitMrApp(BOOLEAN Asyn)
{
	MMIMR_Trace(("MMIMRAPP_exitMrApp(), %d", Asyn));
	MMIMRAPP_setAllReady(FALSE);
	if(Asyn)
	{
		MMK_PostMsg(VIRTUAL_WIN_ID, MMRAPP_MSG_SYS_EXIT, NULL,	0);
		MMI_TriggerMMITask();
	}
	else
	{
		if(s_mr_VMReady)
		{
			s_mr_VMReady = FALSE;
			mr_stop();
			MMIMRAPP_freeMmiRes();
		}
	}
	MMIMR_Trace(("MMIMRAPP_exitMrApp() out"));
}

/*****************************************************************************/
// 	Description : add for mr_playSound() protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_startPlaySoundProtect(void)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_startPlaySoundProtect()"));

	if(!s_mr_allReady)
	{
		return 0;
	}
	
	MMIMRAPP_StopPlaySoundProtect();
	s_mr_mrPlaySound_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 1000, TRUE);
	return s_mr_mrPlaySound_timer;
}

/*****************************************************************************/
// 	Description : add for mr_playSound() protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_StopPlaySoundProtect(void)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_StopPlaySoundProtect() tr: %d", s_mr_mrPlaySound_timer));

	if (s_mr_mrPlaySound_timer > 0)
	{
		MMK_StopTimer(s_mr_mrPlaySound_timer);
		s_mr_mrPlaySound_timer = 0;
	}
}

/*****************************************************************************/
// 	Description : add for start wap broswer in mr apps, when app's mem is shared with wap
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_delayAccessUrl(char* url)
{
	int32 len;

	MMIMR_Trace(("mrapp. MMIMRAPP_delayAccessUrl()"));
	if(!url || (len = strlen(url)) == 0)
    {
		return;
	}
	
	if(s_mr_AccessUrl_url)
    {
		SCI_FREE(s_mr_AccessUrl_url);
	}
	
	s_mr_AccessUrl_url = (char*)SCI_ALLOC(len+2);
	if(!s_mr_AccessUrl_url)
    {
		return;
	}
	
	memset(s_mr_AccessUrl_url, 0, len+2);
	memcpy(s_mr_AccessUrl_url, url, len);

	if(s_mr_AccessUrl_timer > 0)
    {
		MMK_StopTimer(s_mr_AccessUrl_timer);
	}
	
	s_mr_AccessUrl_timer = MMK_CreateWinTimer(	VIRTUAL_WIN_ID,	20,	FALSE);

	MMIMR_Trace(("mrapp. s_mr_AccessUrl_timer: %d", s_mr_AccessUrl_timer));
}

void MMIMRAPP_AccessUrl(void)
{
#if 0
	MMIMR_Trace(("mrapp. MMIMRAPP_AccessUrl(), %d, %d", s_mr_AccessUrl_timer, s_mr_AccessUrl_url));
	if(s_mr_AccessUrl_timer && s_mr_AccessUrl_url)
    {
		MMK_StopTimer(s_mr_AccessUrl_timer);
		s_mr_AccessUrl_timer = 0;
		MMIMRAPP_closeMrapp();//exit mr app to free mem of shared
		MMICMSBRW_AccessUrl((uint8*)s_mr_AccessUrl_url);
		SCI_FREE(s_mr_AccessUrl_url);
		s_mr_AccessUrl_url = 0;
	}
#endif
}


/*****************************************************************************/
// 	Description : close mr base win及其上启动的所有win.
//	Global resource dependence : none
//  Author: qgp
//	Note: 2008-6-11 add
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_closeAllWins(void)
{
	int32 i;
	
	/*mr local wins*/
	for(i = MMIMRAPP_BASE_WIN_ID; i < MRAPP_MAX_WIN_ID; i++)
	{
		if(MMK_IsOpenWin(i))
		{
			MMK_CloseWin(i);
		}
	}
	
	return TRUE;
}

//2009-7-7 add begin
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_startGetApnAccountTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_getApnAccountTimerStop();
	s_mr_getApnAccount_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 50, TRUE);
	return s_mr_getApnAccount_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_getApnAccountTimerStop(void)
{
	if (s_mr_getApnAccount_timer > 0)
	{
		MMK_StopTimer(s_mr_getApnAccount_timer);
		s_mr_getApnAccount_timer = 0;
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_startSetApnAccountTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_setApnAccountTimerStop();
	s_mr_setApnAccount_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 50, FALSE);
	return s_mr_setApnAccount_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
void MMIMRAPP_setApnAccountTimerStop(void)
{
	if (s_mr_setApnAccount_timer > 0)
	{
		MMK_StopTimer(s_mr_setApnAccount_timer);
		s_mr_setApnAccount_timer = 0;
	}
}
//2009-7-7 add end

/* begin:added by Tommy.yan 20120120 */
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartGetSMSCapacityTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_GetSMSCapacityTimerStop();
	s_mr_getSMSCapacity_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 50, FALSE);
	return s_mr_getSMSCapacity_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetSMSCapacityTimerStop(void)
{
	if (s_mr_getSMSCapacity_timer > 0)
	{
		MMK_StopTimer(s_mr_getSMSCapacity_timer);
		s_mr_getSMSCapacity_timer = 0;
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartGetSMSContentTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_GetSMSContentTimerStop();
	s_mr_getSMSContent_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 500, FALSE);
	return s_mr_getSMSContent_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetSMSContentTimerStop(void)
{
	if (s_mr_getSMSContent_timer > 0)
	{
		MMK_StopTimer(s_mr_getSMSContent_timer);
		s_mr_getSMSContent_timer = 0;
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartDeleteSMSTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_DeleteSMSTimerStop();
	s_mr_deleteSMS_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 50, FALSE);
	return s_mr_deleteSMS_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_DeleteSMSTimerStop(void)
{
	if (s_mr_deleteSMS_timer > 0)
	{
		MMK_StopTimer(s_mr_deleteSMS_timer);
		s_mr_deleteSMS_timer = 0;
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartSetSMSStateTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_SetSMSStateTimerStop();
	s_mr_setSMSState_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 50, FALSE);
	return s_mr_setSMSState_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_SetSMSStateTimerStop(void)
{
	if (s_mr_setSMSState_timer > 0)
	{
		MMK_StopTimer(s_mr_setSMSState_timer);
		s_mr_setSMSState_timer = 0;
	}
}
/* end:added by Tommy.yan 20120120 */

//2010-06-24 add begin
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartGetSCNumberTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}

	MMIMRAPP_GetSCNumberTimerStop();
	s_mr_getSCNumber_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 50, FALSE);
	return s_mr_getSCNumber_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetSCNumberTimerStop(void)
{
	if (s_mr_getSCNumber_timer > 0)
	{
		MMK_StopTimer(s_mr_getSCNumber_timer);
		s_mr_getSCNumber_timer = 0;
	}
}
//2010-06-24 add end

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartStatusIconTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}
    
	MMIMRAPP_StopStatusIconTimer();
	s_mr_statusIcon_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 1000, TRUE);
	return s_mr_statusIcon_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_StopStatusIconTimer(void)
{
	if (s_mr_statusIcon_timer > 0)
	{
		MMK_StopTimer(s_mr_statusIcon_timer);
		s_mr_statusIcon_timer = 0;
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_HandleStatusIconTimer(void)
{
	static int flag = 0;

	MMIMR_Trace(("mrapp MMIMRAPP_HandleStatusIconTimer flag = %d", flag));

	MAIN_SetIdleMrappState(flag); /*lint !e718 !e18*/
	flag = !flag;
	GUIWIN_UpdateStb();
}

/*****************************************************************************/
// 	Description : VM后台运行时启动WAP、Camera等模块之前先调用该接口，由该接口
//                根据传入的模块ID来决定强制退出后台应用还是给出提示
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_HandleEventFromOtherModule(int32 module_id)
{ /*lint !e18*/
    BOOLEAN isExit = FALSE;
    MMI_STRING_T string = {0};

    MMIMR_Trace(("mrapp MMIMRAPP_HandleEventFromOtherModule module_id = %d", module_id));

    if(!MMIMRAPP_mrappIsActived())
    {
        return FALSE;
    }

    switch(module_id)
    {
    case MMI_MODULE_CAMERA:
        break;

#ifdef BROWSER_SUPPORT_DORADO
    case MMI_MODULE_MMIBROWSER:
        break;
#endif

#ifdef ATV_SUPPORT
    case MMI_MODULE_ATV:
        break;
#endif

#ifdef QQ_SUPPORT
    case MMI_MODULE_QQ:
        break;
#endif

    default:
        isExit = TRUE;
        break;
    }

    if(isExit == TRUE)
    {
		MMIMRAPP_exitMrApp(FALSE);
        MMIMRAPP_StopStatusIconTimer();
        MAIN_SetIdleMrappState(FALSE); /*lint !e718 !e18*/
        return FALSE;
    }
    else
    {
		//提示:请先退出后台应用
		string.wstr_len = sizeof("\xF7\x8B\x48\x51\x00\x90\xFA\x51\x0E\x54\xF0\x53\x94\x5E\x28\x75\x00\x00")/2-1;
		string.wstr_ptr = (uint8*)"\xF7\x8B\x48\x51\x00\x90\xFA\x51\x0E\x54\xF0\x53\x94\x5E\x28\x75\x00\x00"; /*lint !e64*/
		MMIPUB_OpenAlertWinByTextPtr(NULL,&string,NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_OKCANCEL,MMIMRAPP_HandleQueryWin1Msg);
        return TRUE;
    }
}

int32 MMIMRAPP_OpenSelectDataAccountWin(void)
{/*lint !e18*/
    return MMK_CreateWin((uint32 *)MMIMRAPP_ACCOUNT_WIN_TAB, PNULL);
}

/*****************************************************************************/
//  Description :append one line icon text list item
//  Global resource dependence : none
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
LOCAL void AppendOneLineTextListItem(
                                        MMI_CTRL_ID_T       ctrl_id,
                                        GUIITEM_STYLE_E    item_style,
                                        MMI_TEXT_ID_T       left_softkey_id,
                                        MMI_IMAGE_ID_T      icon_id,
                                        const wchar*        wstr_ptr,
                                        uint16              wstr_len,
                                        uint16              pos,
                                        BOOLEAN             is_update
                                        )
{
    GUILIST_ITEM_T      item_t = {0};/*lint !e64*/
    GUILIST_ITEM_DATA_T item_data = {0};/*lint !e64*/
    uint16                  i         = 0;
    
    item_t.item_style    = item_style;
    item_t.item_data_ptr = &item_data;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = STXT_RETURN;
    
    if( 0 != icon_id )
    {
        item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = icon_id;
        i++;
    }
    
    item_data.item_content[i].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[i].item_data.text_buffer.wstr_len = wstr_len;
    
    item_data.item_content[i].item_data.text_buffer.wstr_ptr = wstr_ptr;
    
    if( is_update )
    {
        GUILIST_ReplaceItem( ctrl_id, &item_t, pos );
    }
    else
    {
        GUILIST_AppendItem( ctrl_id, &item_t );
    }
}


/*****************************************************************************/
// 	Description : handle message of setting list window of browser
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMrappAccountWinMsg(
											MMI_WIN_ID_T	    win_id,
											MMI_MESSAGE_ID_E	msg_id,
											DPARAM           param
											)
{
#ifdef __MRAPP_OVERSEA__
	uint8			i = 0;
	uint32          icon_id = 0;
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T   ctrl_id = MRAPP_ACCOUNT_LISTCTRL_ID;
	MMICONNECTION_SETTING_T *connectin_setting_info = NULL;
#if (SPR_VERSION >= 0x10A1140)
    MN_DUAL_SYS_E   simsel = MMIMRAPP_GetActiveSim(); /*lint !e718*/
#endif

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //动态添加LIST ITEM
        GUILIST_SetMaxItem( ctrl_id, MMICONNECTION_MAX_SETTING_NUM, FALSE);//max item 20

		//获取GPRS连接信息
		connectin_setting_info = MMIMRAPP_GetConnetInfo(); /*lint !e718 !e18*/
#if (SPR_VERSION >= 0x10A1140)
        for(i = 0; i < connectin_setting_info->settinglink_num[simsel]; i++)                              //显示LIST
        {
			if(i == g_mr_curUserApnAccuntId)
			{
				icon_id = IMAGE_RADIO_SELECTED_ICON;
			}
			else
			{
				icon_id = IMAGE_RADIO_UNSELECTED_ICON;
			}
            AppendOneLineTextListItem(ctrl_id,
                GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                TXT_COMMON_OK,
                icon_id,
                connectin_setting_info->setting_detail[simsel][i].name,
                connectin_setting_info->setting_detail[simsel][i].name_len,
                i,
                FALSE
                );
        } 
		if(g_mr_curUserApnAccuntId >= connectin_setting_info->settinglink_num[simsel])
		{
			GUILIST_SetCurItemIndex(ctrl_id, 0);
		}
		else
		{
			GUILIST_SetCurItemIndex(ctrl_id, g_mr_curUserApnAccuntId);
		}
#else
        for(i = 0; i < connectin_setting_info->settinglink_num; i++)                              //显示LIST
        {
			if(i == g_mr_curUserApnAccuntId)
			{
				icon_id = IMAGE_RADIO_SELECTED_ICON;
			}
			else
			{
				icon_id = IMAGE_RADIO_UNSELECTED_ICON;
			}
            AppendOneLineTextListItem(ctrl_id,
                GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                TXT_COMMON_OK,
                icon_id,
                connectin_setting_info->setting_detail[i].name,
                connectin_setting_info->setting_detail[i].name_len,
                i,
                FALSE
                );
        } 
		if(g_mr_curUserApnAccuntId >= connectin_setting_info->settinglink_num)
		{
			GUILIST_SetCurItemIndex(ctrl_id, 0);
		}
		else
		{
			GUILIST_SetCurItemIndex(ctrl_id, g_mr_curUserApnAccuntId);
		}
#endif
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    case MSG_APP_WEB:
        {
            i = GUILIST_GetCurItemIndex(ctrl_id);
			g_mr_curUserApnAccuntId = i;
			MMK_CloseWin(win_id);
			MMIMRAPP_getApnAccountRet(FALSE);
        }
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        MMIMR_Trace(("mrapp HandleSettingListWinMsg() msg_id = %d", msg_id));
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
#endif
}/*lint !e533*/

PUBLIC BOOLEAN MMIMRAPP_IsWindowOfPB(void)
{/*lint !e18*/
	return s_mr_isWindowOfPB;
}

PUBLIC BOOLEAN MMIMRAPP_IsReadBySMS(void)
{/*lint !e18*/
	return s_mr_isReadBySMS;
}

PUBLIC BOOLEAN MMIMRAPP_IsVMPaused(void)
{ /*lint !e18*/
    if(s_mr_vmPaused)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

PUBLIC BOOLEAN MMIMRAPP_IsWindowCreatedBySky(void)
{ /*lint !e18*/
    return s_mr_inMrSelfWinCreating;
}

PUBLIC BOOLEAN MMIMRAPP_SetIsReadBySMS(BOOLEAN IsReadBySMS)
{/*lint !e18*/
	s_mr_isReadBySMS = IsReadBySMS;
	return TRUE;
}

/* begin:added by Tommy.yan 20120201 */
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
void MMIMRAPP_PHBSearchCallBack()
{
	s_mr_phb_search_timer=0;
	mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_OK, 0);
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC uint8 MMIMRAPP_StartPHBOperateTimer(void)
{
	if(!s_mr_allReady)
	{
		return 0;
	}
    
	MMIMRAPP_StopPHBOperateTimer();
	s_mr_phb_operate_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 1000, FALSE);
	return s_mr_phb_operate_timer;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC void MMIMRAPP_StopPHBOperateTimer(void)
{
	if (s_mr_phb_operate_timer > 0)
	{
		MMK_StopTimer(s_mr_phb_operate_timer);
		s_mr_phb_operate_timer = 0;
	}
}
/* end:added by Tommy.yan 20120201 */

#ifdef __MR_CFG_CAMERA_SUPPORT__
int32 mr_camera_create(void)
{
#ifndef WIN32
    int32 ret = 0;

    ret = DCAMERA_Open(DCAMERA_OP_NORMAL);
    MMIMR_Trace(("[SKYCAMERA] DCAMERA_Open ret = %d", ret));
    if(ret == DCAMERA_OP_SUCCESS)
    {
        s_mr_inMrSelfWinCreating = TRUE;
        MMK_CreateWin((uint32 *)MMIMRAPP_CAMERA_WIN_TAB, PNULL);
        s_mr_inMrSelfWinCreating = FALSE;

        return MR_SUCCESS;
    }
    else
    {
        DCAMERA_Close();
        return MR_FAILED;
    }
#endif
}

/*****************************************************************************/
// 	Description : the message handler function of camera win
//	Global resource dependence : 
//  Author: zack
//	Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMrCameraWinMsg( 
											MMI_WIN_ID_T win_id, 
											MMI_MESSAGE_ID_E msg_id, 
											DPARAM param
											)
{
    GUI_COLOR_T  color = MMI_BLACK_COLOR;
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO dev_info_ptr = {0};
#if (SPR_VERSION < 0x10A1140)
    GUI_RECT_T full_rect = MMITHEME_GetFullScreenRect();
#endif
	MMIMR_Trace(("enter HandleMrCameraWinMsg() msg_id = 0x%x, param = 0x%x", msg_id, param));
    
	switch(msg_id)
	{	    
	case MSG_OPEN_WINDOW:
#ifdef WIN32
        GUIBLOCK_SetType(0, color, GUIBLOCK_TYPE_OSD, &dev_info_ptr);
#else
        GUIBLOCK_SetType(255, color, GUIBLOCK_TYPE_COMMON, &dev_info_ptr);
#endif
#if (SPR_VERSION < 0x10A1140)
        GUIBLOCK_ClearRect(&dev_info_ptr);
        GUIBLOCK_SetRect(full_rect, &dev_info_ptr);
        GUIBLOCK_ClearByColor(MMI_BLACK_COLOR,  &dev_info_ptr);		
        GUIBLOCK_Enable(&dev_info_ptr);  
#endif
		break;
        
	case MSG_CLOSE_WINDOW:
        mr_camera_destroy(0);
		break;
		
	case MSG_LOSE_FOCUS:
        mr_camera_pause(0);
		MMIMRAPP_pauseApp();
		break;

	case MSG_GET_FOCUS:
		MMIMRAPP_resumeApp();
        mr_camera_resume(0);
		break;

    case MSG_TIMER:
        mr_camera_handle_timer(*(uint8*)param);
        break;
        
#ifdef MR_HANDSET_IS_SUPPORT_TOUCHPANEL
	case MSG_TP_PRESS_DOWN:
		recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_DOWN, (void*)param);
		break;
	case MSG_TP_PRESS_UP:
		recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_UP, (void*)param);
		break;
	case MSG_TP_PRESS_MOVE:
		recode = ProcessMrAppKeyMsg(MMIMRAPP_MOUSE_MOVE, (void*)param);
		break;
#endif
		
	case MSG_KEYPRESSUP_RED:
	case MSG_KEYDOWN_RED:
	case MSG_KEYUP_RED://2007add
    	MMIMR_Trace(("mrapp HandleMrCameraWinMsg() msg_id = 0x%x, s_mr_fbhup2idle = %d", msg_id, s_mr_fbhup2idle));
		if(s_mr_fbhup2idle != 1)	
        {      
			recode = MMI_RESULT_FALSE;
        }
        else
        {
            recode = ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
        }
		break;

	default:
		if(s_mr_VMReady)
		{
			recode = ProcessMrAppKeyMsg(MMIMRAPP_KEY_NORMAL, (void*)&msg_id);
		}
		break;
	}

	MMIMR_Trace(("exit HandleMrCameraWinMsg() msg_id = 0x%x, recode = %d", msg_id, recode));
	return recode;
}
#endif

#endif//#_MMIMRAPP_WINTAB_C_
#endif//#end MRAPP_SUPPORT

