/*****************************************************************************
** File Name:      mmiidle_statusbar.h                                       *
** Author:                                                                   *
** Date:           11/07/2011                                                *
** Copyright:      2011 Spreadtrum, Incoporated. All Rights Reserved.        *
** Description:    This file is used to describe punctuation                 *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2011        jian.ma              Creat
******************************************************************************/
#ifndef _MMIIDLE_STATUSBAR_H_
#define _MMIIDLE_STATUSBAR_H_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_font.h"
#include "mmi_common.h"
#include "guistring.h"
#include "mn_type.h"
#include "mmi_signal_ext.h"
#include "mmipdp_export.h"
#include "mmiidle_func.h"
#include "guistatusbar_scrollmsg.h"
#include "dualbat_drvapi.h"

#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         TYPE AND STRUCT                                  *
 **--------------------------------------------------------------------------*/
#ifdef WRE_SUPPORT
#define WRE_ICON_MAX 6  //add for wre_background by heng.xiao 20120208
#endif
typedef enum
{
#ifdef PDA_UI_DROPDOWN_WIN
  MMI_WIN_TEXT_DATE,          // MAIN LCD日期
#endif
  MMI_WIN_ICON_SIGNAL,        // MAIN LCD sim1信号量提示
#ifndef MMI_MULTI_SIM_SYS_SINGLE
  MMI_WIN_ICON_SIGNAL_2,        //MAIN LCD sim2信号量提示
#if defined(MMI_MULTI_SIM_SYS_TRI) || defined(MMI_MULTI_SIM_SYS_QUAD)
  MMI_WIN_ICON_SIGNAL_3,        //MAIN LCD sim3信号量提示
#endif
#if defined(MMI_MULTI_SIM_SYS_QUAD)
  MMI_WIN_ICON_SIGNAL_4,        //MAIN LCD sim4信号量提示
#endif
#endif

#ifdef MMI_DUALMODE_ENABLE
  MMI_WIN_ICON_GPRS,          // 3G gprs服务开启
#endif
  MMI_WIN_ICON_CALLING,       // MAIN LCD正在通话中
  MMI_WIN_ICON_MISSED_CALL,   // MAIN LCD未接来电
  MMI_WIN_ICON_MESSAGE,       // MAIN LCD短信提示
#ifdef MMI_IDLE_FDN_ICON_SUPPORT
  MMI_WIN_ICON_FDN,
#endif
#ifdef WIFI_SUPPORT
  MMI_WIN_ICON_WIFI,          // MAIN LCD WIFI
#endif
#if defined(CMMB_SUPPORT) || defined(ATV_SUPPORT)
  MMI_WIN_ICON_CMMB_SIGNAL,   // CMMB Signal
#endif
#ifdef MCARE_V31_SUPPORT
    MMI_WIN_ICON_MCARE_APP,
#endif  
#ifdef QQ_SUPPORT
  MMI_WIN_ICON_SPREADTRUM_QQ, // SPRD_QQ 图标提示
#endif
#ifdef MRAPP_SUPPORT
  MMI_WIN_ICON_MRAPP,         //MAIN LCD冒泡
#ifdef __MMI_SKYQQ__
  MMI_WIN_ICON_SKYQQ,
#endif
#endif
#ifdef JAVA_SUPPORT
  MMI_WIN_JAVA,               // JAVA 背景运行提示
#endif
#ifdef ASP_SUPPORT
  MMI_WIN_ICON_ASP,           // 答录机的图标
#endif
  MMI_WIN_ICON_CALL_FORWARD,  // MAIN LCD呼叫转移
  MMI_WIN_ICON_LINE_INFO,     // MAIN LCD通话线路状态
#ifdef BLUETOOTH_SUPPORT
  MMI_WIN_ICON_BLUE_TOOTH,    // MAIN LCD蓝牙
#endif
  MMI_WIN_ICON_ALARM,         // MAIN LCD闹钟提示
  MMI_WIN_ICON_ENVSET,        // MAIN LCD情景模式:静音,铃声,振动提示
  MMI_WIN_ICON_ROAMING,       // MAIN LCD漫游提示
  MMI_WIN_ICON_EARPHONE,      // MAIN LCD耳机提示
#ifdef WRE_SUPPORT   //add for wre_background by heng.xiao 20120208 
    MMI_WIN_WRE,
    MMI_WIN_WRE_MAX = MMI_WIN_WRE + WRE_ICON_MAX-1,
#endif

  // 这里可能缺存储卡状态表示
#ifdef MET_MEX_QQ_SUPPORT
	MEX_MASTER_QQ_STATUS,		//QQ
	MEX_SLAVE_QQ_STATUS,		
#endif
#ifdef MET_MEX_DOUBLE_QQ_SUPPORT 
  MEX_MASTER_DOUBLE_QQ_STATUS,		//QQ
  MEX_SLAVE_DOUBLE_QQ_STATUS,	 
#endif
#ifdef MET_MEX_MSN_SUPPORT     //MSN
  MEX_MSN_STATUS,
#endif
  // 这里可能缺存储卡状态表示
#ifdef MET_MEX_FETION_SUPPORT     //FETION
	MEX_FETION_STATUS,
#endif

#ifdef MMI_BATTERY_PERCENT_IN_STATUSBAR
    MMI_WIN_ICON_BATTERY_PERCENT,
#endif
  // 下面两个靠右显示
  MMI_WIN_ICON_BATTERY,       // MAIN LCD电池提示
#ifdef MMI_DUAL_BATTERY_SUPPORT
    MMI_WIN_ICON_BATTERY_AUX,
#endif
  MMI_WIN_TEXT_TIME,          // MAIN LCD时间
  // end
  MMI_WIN_ICON_MAX
}MMI_WIN_ICON_E;
/**--------------------------------------------------------------------------*
 **                         EXTERNAL DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

#ifdef JAVA_SUPPORT
/*****************************************************************************/
//  Description : handle java
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIIDLE_JavaCallBack(void);
#endif

#ifdef QQ_SUPPORT
/*****************************************************************************/
//  Description : handle qq
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIIDLE_QqCallBack(void);
#endif

/*****************************************************************************/
//  Description : handle mcareqq
//  Global resource dependence : none
//  Author:wei.ren
//  Note: 
/*****************************************************************************/

#ifdef MCARE_V31_SUPPORT
PUBLIC BOOLEAN MMIMAIN_MCARE_QQCallBack(void);
PUBLIC BOOLEAN MMIMAIN_MCARE_QCCallBack(void);
PUBLIC uint32 Mcare_idle_iconid(void);
#endif//#ifdef MCARE_V31_SUPPORT


/*****************************************************************************/
// Description : 显示图标
// Global resource dependence : 
// Author: Ming.Song
// Note:2007-8-3
/*****************************************************************************/
PUBLIC void MMIDILE_StatusBarInit(void);

/*****************************************************************************/
//  Description : get the state of line
//  Global resource dependence :  
//  Author: kelly.li
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleLineState(
                                  uint16 line_state,
                                  MN_DUAL_SYS_E dual_sys
                                  );

/*****************************************************************************/
//  Description : handle miss call
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMAIN_MissCallCallBack(void);

/*****************************************************************************/
//  Description : handle miss message
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMAIN_MissMessageCallBack(void);

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//  Description : handle miss message
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMAIN_ReadPushCallBack(void);
#endif

/*****************************************************************************/
// Description : 显示信号图标
// Global resource dependence : 
// Author: wancan.you
// Note:
/*****************************************************************************/
PUBLIC void MMIIDLE_SetSimStatusBarInfo(
                                MN_DUAL_SYS_E dual_sys,
                                uint32 item_index,
                                BOOLEAN is_append
                                );

/*****************************************************************************/
//  Description :  设置文本内容
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 文本内容为时间
/*****************************************************************************/
PUBLIC void MAIN_SetStbDispTime(void);

/*****************************************************************************/
//  Description :  获取状态栏timer是否可见
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 文本内容为时间
/*****************************************************************************/
PUBLIC BOOLEAN MAIN_IsStbDispTime(void);

/*****************************************************************************/
//  Description :  设置状态栏是否显示timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 文本内容为时间
/*****************************************************************************/
PUBLIC void MAIN_SetStbTimeVisible(
                                   BOOLEAN is_true
                                   );
/*****************************************************************************/
//  Description :  set status bar time info
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MAIN_SetStbDispTimeinfo(wchar *time_buf,uint16 time_len);

/*****************************************************************************/
// Description : 获得时间字符串
// Global resource dependence : 
// Author: xiaoqing.lu
// Note:
/*****************************************************************************/
PUBLIC void MAIN_StatusbarGetSysTime(
                               wchar *time_wstr, 
                               uint16 wlen
                               );

/*****************************************************************************/
// 	Description : update the state of line
//	Global resource dependence :  
//  Author: kelly.li
//	Note:
/*****************************************************************************/
PUBLIC void MAIN_UpdateLineState(void);

/*****************************************************************************/
//  Description : set msg state
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetMsgState(BOOLEAN is_draw);

/*****************************************************************************/
//  Description : set the call forward state of the idle window
//  Global resource dependence : 
//  Author: kelly.li
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleForwardState(
                                     MN_DUAL_SYS_E dual_sys,
                                     BOOLEAN forward_state
                                     );

/*****************************************************************************/
//  Description : set the Alarm state of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_UpdateIdleEnvsetState(void);

/*****************************************************************************/
// 	Description : set the roaming state of the idle window
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleRoamingState(
                                   MN_DUAL_SYS_E	e_dual_sys,
                                   BOOLEAN is_roaming
                                   );

/*****************************************************************************/
// Description : 显示图标
// Global resource dependence : 
// Author: Ming.Song
// Note:2007-8-3
/*****************************************************************************/
PUBLIC void MMIAPIIDLE_SetGprsInfo(
                                    MN_DUAL_SYS_E dual_sys,
                                    MMIPDP_NOTIFY_EVENT_E pdp_event,
                                    BOOLEAN is_append
                                    );
/*****************************************************************************/
//  Description : set the gprs state of the idle window
//  Global resource dependence : 
//  Author: figo.feng
//  Note: DUAL_MODE下只做关闭图标用
/*****************************************************************************/
PUBLIC void MAIN_SetIdleGprsState(
                                   MN_DUAL_SYS_E    e_dual_sys,
                                   BOOLEAN          gprs_state
                                   );
/*****************************************************************************/
//  Description : set the rx level of the idle window
//    is_direct:TRUE直接显示信号跳变；FALSE平滑显示信号跳变
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleRxLevel(MN_DUAL_SYS_E e_dual_sys, uint8 rx_level, BOOLEAN is_direct);

/*****************************************************************************/
//  Description : get the rx level
//  
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC uint8 MMIMAIN_GetRxLevel(MN_DUAL_SYS_E e_dual_sys);

/*****************************************************************************/
//  Description : set the battery level of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleBatLevel(
                                 uint8 bat_level
                                 );

/*****************************************************************************/
//  Description : handle rxlevel bound timer
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_HandleRxLevelTimer(uint8 timer_id, uint32 param);
/*****************************************************************************/
//  Description : set the charge state
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleChargeState(
                                    BOOLEAN is_charge
                                    );
#ifdef MMI_DUAL_BATTERY_SUPPORT
/*****************************************************************************/
// Description : 
// Global resource dependence : 
// Author: renyi.hu
// Note:
/*****************************************************************************/
PUBLIC void MMIAPIIDLE_SetStbBatteryInfo(BOOLEAN is_main_battery);

/*****************************************************************************/
//  Description : set the battery level of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleDbatLevel(DBAT_STATE_INFO_T* dbat_info);

/*****************************************************************************/
// Description : 
// Global resource dependence : 
// Author: renyi.hu
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN  MMIAPIIDLE_IsChangeChargeState(void);
#endif

#ifdef WIN32
/*****************************************************************************/
//  Description : dummy function of REF
//  Global resource dependence : 
//  Author: bruce.chi
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN CHGMNG_IsChargeConnect(void);

/*****************************************************************************/
//  Description : dummy function of REF
//  Global resource dependence : 
//  Author: bruce.chi
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GPIO_CheckHeadsetStatus(void);
#endif


/*****************************************************************************/
//  Description : 开机时添加滚动信息
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
#ifdef MMI_STATUSBAR_SCROLLMSG_SUPPORT
PUBLIC void MMIMAIN_PowerOnAddStbScrollItem(void);
#endif

#ifdef WIFI_SUPPORT
/*****************************************************************************/
// 	Description : set the wifi icon of the idle window
//	Global resource dependence : 
//  Author: li.li
//	Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleWifiIcon(
                                       MMI_IMAGE_ID_T image_id
                                       );
                                       
/*****************************************************************************/
// 	Description : get the wifi icon of the idle window
//	Global resource dependence : 
//  Author: jixin.xu
//	Note:
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MAIN_GetIdleWifiIcon(void);

#endif

/*****************************************************************************/
//  Description : set the blue tooth state of when power on
//  Global resource dependence : 
//  Author: kelly.li
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleBTStateWhenPowerOn(
                                           BLUETOOTH_STATE_TYPE  bluetooth_state
                                           );

/*****************************************************************************/
//  Description : set the blue tooth state of the idle window
//  Global resource dependence : 
//  Author: kelly.li
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleBlueToothState(
                                       BLUETOOTH_STATE_TYPE  bluetooth_state
                                       );
/*****************************************************************************/
//  Description : set the charge state
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleNetWorkName(void);

/*****************************************************************************/
//  Description : set the charge state
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_UpdateIdleSignalState(void);


/*****************************************************************************/
// 	Description : set state of tv 
//	Global resource dependence :  
//  Author: xiaoqing.lu
//	Note:
/*****************************************************************************/
PUBLIC void MAIN_SetTVSignalState(void);

/*****************************************************************************/
// 	Description : set state of tv 
//	Global resource dependence :  
//  Author: xiaoqing.lu
//	Note:
/*****************************************************************************/
PUBLIC void MAIN_SetTVSignalFalse(void);

/*****************************************************************************/
//  Description : set the battery level of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleBatLevel(
                                 uint8 bat_level
                                 );

/*****************************************************************************/
//  Description : set missed call state
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetCallingState(BOOLEAN is_calling);

/*****************************************************************************/
//  Description : set missed call state
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetMissedCallState(BOOLEAN is_missed_call);

/*****************************************************************************/
//  Description : set the td state of the idle window
//  Global resource dependence : 
//  Author: maggie
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleTdState(void);

/*****************************************************************************/
//  Description : set the unknown state of the idle window
//  Global resource dependence : 
//  Author: maggie
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleUnknownState(void);

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetAllStbAppCallbackValid(
                                           BOOLEAN is_valid
                                           );


/*****************************************************************************/
//  Description : set the Alarm state of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleAlarmState(
                                   BOOLEAN alarm_state
                                   );


/*****************************************************************************/
//  Description : set the java state of the idle window
//  Global resource dependence : 
//  Author: murphy.xie
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_UpdateIdleJavaState(BOOLEAN java_state);

/*****************************************************************************/
//  Description : set the Alarm state of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_UpdateIdleEarPhoneState(void);

/*****************************************************************************/
//  Description : set the vibrator state of the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleShakeState(
                                   BOOLEAN shake_state
                                   );

/*****************************************************************************/
//  Description : if is silent of call ring in the idle window
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleSilentState(
                                    BOOLEAN silent_state
                                    );

#ifdef QQ_SUPPORT
/*****************************************************************************/
// Description : 刷新idle下的QQ icon
// Global resource dependence : 
// Author: fen.xie
// Note:
/*****************************************************************************/
PUBLIC  void MMIAPIIDLE_UpdateQQState(void);
#endif

#ifdef ASP_SUPPORT
/*****************************************************************************/
// Description : 刷新idle下的asp icon
// Global resource dependence : 
// Author: jun.hu
// Note:
/*****************************************************************************/
PUBLIC  void MMIAPIIDLE_UpdateAspState(void);
#endif

/*****************************************************************************/
// Description : 设置状态栏耳机图标的显示 
// Global resource dependence : 
// Author: juan.wu
// Note:
/*****************************************************************************/
PUBLIC void MMIAPIIDLE_SetStbEpDisplay(BOOLEAN is_display);

#ifdef MMI_STATUSBAR_SCROLLMSG_SUPPORT

/*****************************************************************************/
//  Description : 添加滚动信息
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC void MMIMAIN_SetScrollItemNewSMS(void);
                                        
/*****************************************************************************/
//  Description : 添加滚动信息
//  Global resource dependence : none
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMAIN_AppendStbScrollItem(
                                           GUISTBSCROLLMSG_SCROLL_ITEM_E item_type
                                           );
#endif

#ifdef MMI_MULTISIM_COLOR_SUPPORT_STATUSICON

/*****************************************************************************/
//  Description : Set sim color for statusbar
//  Global resource dependence : none
//  Author: sam.hua
//  Note: 
/*****************************************************************************/
PUBLIC void MMIMAIN_UpdateStatusBarBySimColor(MN_DUAL_SYS_E dual_sys,GUI_COLOR_T color);

#endif

/*****************************************************************************/
//  Description : 设置新语音消息图标
//  Global resource dependence :  
//  Author: kelly.li
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetIdleVMFlagState( BOOLEAN vm_status);

#ifdef MMI_IDLE_FDN_ICON_SUPPORT
/*****************************************************************************/
//  Description : set FDN state
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MAIN_SetFDNState(BOOLEAN is_draw);
#endif

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif
