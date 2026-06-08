/****************************************************************************
** File Name:      mmiqq_system.c(Qsystem.c)                                                *
** Author:                                                                 *
** Date:           05/11/2010                                              *
** Copyright:      2003 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe the system  of QQ             *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 05/2010       fen.xie         Create
** 
****************************************************************************/
#include "mmi_app_qq_trc.h"
#ifdef QQ_SUPPORT

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#ifdef WIN32
#include "std_header.h"
#endif
#include "QSystem.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmidisplay_data.h"
#include "window_parse.h"
#include "qq2009.h"
#include "qq_image.h"
#include "mmiaudio.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#include "mmisms_export.h"
#include "mmiidle_export.h"
#include "mmiphone_export.h"
#include "mmiidle_subwintab.h"
#include "mmipub.h"
#include "mmiudisk_export.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "mmimtv_export.h"

#include "mmifm_export.h"//@fen.xie MS00182276
#include "mmi_appmsg.h"
#include "mmicc_export.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
//#include "mmienvset.h"
#include "mmienvset_export.h"
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define MMIQQ_RECV_MSG_VOL  MMISET_VOL_TWO//qq receive msg的ring volume
#define MMIQQ_VIBRATE_TIME  1000//qq 振动的时间
#define QQ_SYSTEM_START_YEAR    (1970)//TENCENT系统的起始年份

 /*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/
/*******************声音播放相关******************/

#if ( defined (WIN32) || defined (__AMNT__) )
#define __align(x)
#endif
/*********声音播放相关数据************************************/
//#define QQ_MSG_MID_SIZE 109

__align(4) LOCAL const uint8 s_qq_msg_midi_arr[] =
{
    0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x00,
    0x00,0x01,0x00,0x78,0x4D,0x54,0x72,0x6B,0x00,0x00,
    0x00,0x57,0x00,0xFF,0x03,0x08,0x75,0x6E,0x74,0x69,
    0x74,0x6C,0x65,0x64,0x00,0xFF,0x01,0x02,0x51,0x51,
    0x00,0xFF,0x58,0x04,0x04,0x02,0x18,0x08,0x00,0xFF,
    0x59,0x02,0x00,0x00,0x00,0xFF,0x51,0x03,0x07,0xA1,
    0x20,0x00,0xC0,0x50,0x00,0xB0,0x07,0x78,0x00,0x90,
    0x5E,0x78,0x0F,0x5E,0x00,0x0F,0x5E,0x78,0x0F,0x5E,
    0x00,0x0F,0x5E,0x78,0x0F,0x5E,0x00,0x0F,0x5E,0x78,
    0x0F,0x5E,0x00,0x0F,0x5E,0x78,0x0F,0x5E,0x00,0x0F,
    0x5E,0x78,0x0F,0x5E,0x00,0x00,0xFF,0x2F,0x00
};


/************************end**************************************************/

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/

LOCAL BOOLEAN      s_is_login_selcet = FALSE; //是否已经完成sim卡(供登录QQ使用)的选择
LOCAL MN_DUAL_SYS_E	s_qq_sim_select = MN_DUAL_SYS_1;//登录QQ使用的sim卡
LOCAL MMIQQ_BUDDY_STAT_E  s_qq_leave_state = MMIQQ_BUDDY_STAT_MAX;//qq当前的状态(离线\在线\隐身...)
#ifdef WIFI_SUPPORT
LOCAL BOOLEAN s_qq_is_use_wifi = FALSE;
#endif
//记录进入QQ前的lcd logic angle,后台时不会reset,只有完全退出QQ再次进入才会reset
LOCAL LCD_ANGLE_E    s_qq_logic_angle = LCD_ANGLE_0;

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DECLARE                                 *
 **--------------------------------------------------------------------------*/

 /**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE) 
/*****************************************************************************/
//  Description :HandleSIMSelectWinMsg
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSIMSelectWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*****************************************************************************/
//  Description : append item to listbox 
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void QQAppendSelectListItem(MMI_CTRL_ID_T ctrl_id);
#endif

/*****************************************************************************/
//  Description :时间戳转换为系统时间
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
LOCAL QTime TimeStamp2Systime(int time_stamp_num);

/*****************************************************************************/
//  Description : select network(sim卡or wifi) for QQ
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MMIQQ_SelectNetworkType(void);

/*****************************************************************************/
//  Description : msd send sms callback function
//  Global resource dependence : 
//  Author:minghu.mao
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN QqSMSSendCnfCallback(BOOLEAN is_succss, DPARAM parm);


//当wifi模块开启时,该窗口为网络选择(sim卡和wifi网络)
#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE) 
WINDOW_TABLE(MMIQQ_SELECT_SIM_WIN_TAB) = 
{
    
//	WIN_PRIO(WIN_ONE_LEVEL),
    WIN_FUNC((uint32)HandleSIMSelectWinMsg),    
    WIN_ID(MMIQQ_WIN_ID_SELECT_SIM),
    WIN_TITLE(TXT_SIM_SEL),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E,  MMIQQ_CTRL_ID_SELECT_SIM),
    END_WIN
};
#endif

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/***FOR TENCENT QQ System BEGIN***/
/*****************************************************************************/
//     Description : QQ paly receive msg call back
//    Global resource dependence : 
//  Author:fen.xie
//    Note: 
/*****************************************************************************/
LOCAL void QQRingCallBack(MMISRVAUD_REPORT_RESULT_E result, DPARAM param)
{
    MMIAPISET_SetVibratorEx(FALSE,MMISET_RING_TYPE_MSG,FALSE,0);
}

/*****************************************************************************/
//  Description :播放声音
//  Global resource dependence : none
//  Author: 
//  Note: audio 类型同MSG_RING_TYPE(短信提示类型)
/*****************************************************************************/
#ifdef WRE_SUPPORT
extern PUBLIC BOOLEAN WRE_IsFocusRunWin(void);
#endif
PUBLIC void QPlayRecvMsg(ISoundPlayer* pSoundPlayer)
{
    MMISET_MSG_RING_TYPE_E ring_type = MMISET_MSG_RING;

    if (!MMIAPISET_IsPermitPlayRing(MMISET_RING_TYPE_MSG)
        || !MMIAUDIO_IsPermitPlayNewMsgRing() 
        || MMIAPICC_IsInState(CC_IN_CALL_STATE)
        || MMIAPIUDISK_UdiskIsRun()
#ifdef JAVA_SUPPORT
        ||MMIAPIJAVA_IsFocusJAVARunWin() 
#endif
#ifdef WRE_SUPPORT
        ||WRE_IsFocusRunWin()
#endif
        || MMIAPIUdisk_IsOpenPcCamera()
        )
    {
        //SCI_TRACE_LOW:"[QQ]QPlayRecvMsg: Can't play msg ring"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIQQ_SYSTEM_185_112_2_18_2_49_11_42,(uint8*)"");
        return;
    }
    
    ring_type = (MMISET_MSG_RING_TYPE_E)MMIAPIENVSET_GetActiveModeOptValue(s_qq_sim_select, MSG_RING_TYPE);

    if (((MMISET_MSG_RING == ring_type) || (MMISET_MSG_VIBRA_AND_RING == ring_type)) && (MMIENVSET_SETTING_SILENT != MMIAPIENVSET_GetActiveModeId()))//允许play ring
    {
        MMIAPISET_PlayRingByPtr(MMISET_RING_TYPE_MSG,
                MMIQQ_RECV_MSG_VOL,
                (uint8 *)s_qq_msg_midi_arr,
                sizeof(s_qq_msg_midi_arr),
                1,
                MMIAUDIO_MIDI,
                QQRingCallBack
                );
    }
                    
}
/*****************************************************************************/
//  Description :播放震动
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void QPlayVibrator(ISound* pSound)
{
    if (MMIAPISET_IsPermitPlayRing(MMISET_RING_TYPE_OTHER))
    {
        if(MMIAPISET_IsVibratTimerActive())
        {
            MMIAPISET_StopVibratTimer();
        }
        MMIAUDIO_Vibrate(MMIQQ_VIBRATE_TIME, FALSE, PNULL);
    }
 
}
/*****************************************************************************/
//  Description :
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void ISOUNDPLAYER_Release(ISoundPlayer* pSoundPlayer)
{
    return;
}

/*****************************************************************************/
//  Description :
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void ISOUND_Release(ISound* pSound)
{
    return;
}

/*****************************************************************************/
//  Description :从QQ模块访问url
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void QGoToUrl(char* url_ptr)
{
#ifdef BROWSER_SUPPORT
    MMIBROWSER_ENTRY_PARAM entry_param = {0};/*lint !e64*/

    entry_param.type = MMIBROWSER_ACCESS_URL;
#ifdef WIFI_SUPPORT
    entry_param.is_use_wifi = s_qq_is_use_wifi;//wifi 优先级高于dual_sys
#endif
    entry_param.dual_sys = s_qq_sim_select;
    entry_param.url_ptr = url_ptr;
    entry_param.user_agent_ptr = PNULL;
    
    //SCI_TRACE_LOW:"[QQ] QGoToUrl is_use_wifi = %d, dual_sys =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIQQ_SYSTEM_255_112_2_18_2_49_11_43,(uint8*)"dd",entry_param.is_use_wifi , entry_param.dual_sys );
    MMIAPIBROWSER_Entry(&entry_param);
#endif	
}

/*****************************************************************************/
//  Description : msd send sms callback function
//  Global resource dependence : 
//  Author:minghu.mao
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN QqSMSSendCnfCallback(BOOLEAN is_succss, DPARAM parm)
{//dm 不需要做任何处理，直接结束流程
    BOOLEAN result = TRUE;
    
    return result;
}

/*****************************************************************************/
//  Description :发送短信
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void QSendSms(char* addr_str_ptr,unsigned short* msg_wstr_ptr)
{
    MMI_STRING_T sms_init_ptr = {0};
    MMISMS_SEND_DATA_T send_data = {0};

    //SCI_TRACE_LOW("[QQ]QSendSms()");

    if ((PNULL == addr_str_ptr) || (PNULL == msg_wstr_ptr))
    {
        return;	
    }
    sms_init_ptr.wstr_len = (uint16)MMIAPICOM_Wstrlen(msg_wstr_ptr);
    sms_init_ptr.wstr_ptr= (wchar*)msg_wstr_ptr;

    send_data.dual_sys = s_qq_sim_select;
    send_data.sms_content_ptr = &sms_init_ptr;
    send_data.dst_port = 0;
    send_data.src_port = 0;
    send_data.dest_addr_ptr = (uint8 *)addr_str_ptr;
    send_data.dest_addr_len = strlen(addr_str_ptr);
    send_data.is_need_packet = TRUE;
    send_data.send_callback = QqSMSSendCnfCallback;

    MMIAPISMS_SendSmsForOther(&send_data);
}

/*****************************************************************************/
//  Description :拨打电话 
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void QMakeCall(char* num_ptr)
{
	
}


/*****************************************************************************/
//  Description :得到当前系统时间的时间戳
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC QU32 QGetSystemTime(void)
{
    QTime time = {0};
    QTime startTime = {0};
    QTime time1 = {0};

    QGetDateTime(&time1);

    //SCI_TRACE_LOW("[QQ]QGetSystemTime");

    startTime.wYear = QQ_SYSTEM_START_YEAR;
    startTime.nMonth = 1;
    startTime.nDay = 1;
    startTime.nHour = 0;
    startTime.nMin = 0;
    startTime.nSec = 0;

    time.wYear =  time1.wYear;
    time.nMonth = time1.nMonth;
    time.nDay = time1.nDay;
    time.nHour = time1.nHour;
    time.nMin = time1.nMin;
    time.nSec = time1.nSec;

    return QTimeInterval(time, startTime);
}

/*****************************************************************************/
//  Description :计算当前日期在其所在年份的总天数(day)
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC QU32 QTimeAllDay(QTime a)   
{     
    QU32   notrun[12] = {31,29,31,30,31,30,31,31,30,31,30,31};   
    QU32   x = 0, i = 0;   

    //SCI_TRACE_LOW("[QQ]QTimeAllDay");

    x = (a.wYear)*365 + a.wYear/4 + a.wYear/400 - a.wYear/100;   
    
    if ((((a.wYear % 4) == 0) && ((a.wYear % 100) != 0)) ||((a.wYear % 400) == 0))   
    {   
        for (i = 0 ;i < a.nMonth; i++)
        {   
            if (i > 0)  
            {   
                x += notrun[i-1];   
            }   
        }   
    } 
    else 
    {   
        for (i = 0; i < a.nMonth; i++)   
        {   
            if (i > 0)
            {   
                x += notrun[i-1];   
            }   
        }   
    }   
    
    x += a.nDay;
    
    return  x;   
}  
/*****************************************************************************/
//  Description :计算时间间隔(s)
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC QU32 QTimeInterval(QTime a, QTime b)   
{   
    QU32   x_days = 0, y_days = 0;   
    QU32   sum = 0;   

    //SCI_TRACE_LOW("[QQ]QTimeInterval");

    x_days = QTimeAllDay(a);   
    y_days = QTimeAllDay(b);   
    
    sum = (x_days-y_days)*24 + (a.nHour -b.nHour) - 32;//hours 差   
    return  (sum*3600 + (a.nMin-b.nMin)*60 + (a.nSec - b.nSec));   
}   

/*****************************************************************************/
//  Description :时间戳转换为系统时间
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
LOCAL QTime TimeStamp2Systime(int time_stamp_num)
{
    QTime time = {0};
    int min1 = 60;// 1 minute is 60 seconds
    int h1 = 60 * min1;
    int d1 = 24 * h1;
    int m28 = 28 * d1;//2419200;
    int m29 = 29 * d1;//2505600;
    int m30 = 30 * d1;//2592000; 
    int m31 = 31 * d1;//2678400;
    int yd365 = 365 * d1;//31536000;
    int yd366 = 366 * d1;//31622400;
    uint16 year =QQ_SYSTEM_START_YEAR, month = 1, day = 1, hour = 0, minute = 0, second = 0;//时间戳以1970.1.1开始计算的
    int c_y = 1, c_m = 1, c_d = 1, c_h = 1, c_min = 1; 
    int time_stamp = time_stamp_num;
    
    //SCI_TRACE_LOW("[QQ]TimeStamp2Systime");

    while (c_y != 0)//calculate year
    {
        if (time_stamp >= yd366)
        {
            if (0 == (year % 4))//闰年
            {
                year++;
                time_stamp -= yd366;
            }
            else 
            {
                year++;
                time_stamp -= yd365;
            }
        }
        else if ((time_stamp <= yd366) && (time_stamp >= yd365))
        {
            year++;
            time_stamp -= yd365;
            c_y = 0;    
        }
        else if (time_stamp <= yd365)
        {
            c_y = 0;        
        }
    }
    
    while (c_m != 0)//calculate month
    {
        // 1月
        if ((time_stamp >= m31) && (1 == month))
        {
            time_stamp -= m31;
            month++;
        }
        else 
        {
            c_m = 0;
        }

        // 2月
        if (0 == (year % 4))//闰年
        {
            if ((time_stamp >= m29) && (2 == month))
            {
                time_stamp -= m29;
                month++;
            }
            else
            {
                c_m = 0;
            }
        }
        else 
        {
            if ((time_stamp >= m28) && (2 == month))
            {
                time_stamp -= m28;
                month++;
            }
            else
            {
                c_m = 0;
            }
        }
        
        if ((time_stamp >= m31) && (3 == month))
        {
            time_stamp -= m31;
            month++;
        }
        else
        {
            c_m = 0;
        }
        
        if ((time_stamp >= m30) && (4 == month))
        {
            time_stamp -= m30;
            month++;
        }
        else
        {
            c_m = 0;
        }
        
        if ((time_stamp >= m31) && (5 == month))
        {
            time_stamp -= m31;
            month++;
        }
        else
        {
            c_m = 0;
        }

        if ((time_stamp >= m30) && (6 == month))
        {
            time_stamp -= m30;
            month++;
        }
        else
        {
            c_m = 0;
        }
        
        if ((time_stamp >= m31) && (7 == month))
        {
            time_stamp -= m31;
            month++;
        }
        else
        {
            c_m = 0;
        }
        
        if ((time_stamp >= m31) && (8 == month))
        {
            time_stamp -= m31;
            month++;
        }
        else
        {
            c_m=0;
        }
        
        if ((time_stamp >= m30) && (9 == month))
        {
            time_stamp -= m30;
            month++;
        }
        else
        {
            c_m=0;
        }
        
        if ((time_stamp >= m31) && (10 == month))
        {
            time_stamp -= m31;
            month++;
        }
        else
        {
            c_m=0;
        }
        
        if ((time_stamp >= m30) && (11 == month))
        {
            time_stamp -= m30;
            month++;
        }
        
        if (month >= 12)
        {
            c_m = 0;
        }
    }
    
    while (c_d != 0)//calcute day
    {
        if (time_stamp >= d1)
        {
            day++;
            time_stamp -= d1;
        }
        else 
        {
            c_d = 0;
        }
    }
    
    while (c_h != 0)//calcute hour
    {
        if (time_stamp > h1)
        {
            hour++;
            time_stamp -= h1;
        }
        else 
        {
            c_h = 0;
        }
    }
        
    while (c_min != 0)//calcute minute
    {
        if (time_stamp > min1)
        {
            minute++;
            time_stamp -= min1;
        }
        else
        {
            c_min = 0;
        }
    }
    
    //剩余的就是second了
    second = time_stamp;
    
    time.wYear = year;
    time.nMonth = month;
    time.nDay = day;
    time.nHour = hour + 8;
    time.nMin = minute;
    time.nSec = second;
    //凌晨的情况，要注意啊 
    time.nHour %= 24;

    return time;
}

/*****************************************************************************/
//  Description :时间戳转换为系统时间
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC QTime QStamp2SysTime(QU32 t)
{
    QTime time = {0};
    
    //SCI_TRACE_LOW("[QQ]QStamp2SysTime");
    
    time = TimeStamp2Systime(t);
    return time;
}

/*****************************************************************************/
//  Description :获得手机系统时间信息
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC void QGetDateTime(QTime* time_ptr)
{
    SCI_DATE_T date = {0};
    SCI_TIME_T time = {0};
    
    //SCI_TRACE_LOW("[QQ]QGetDateTime");

    if (PNULL == time_ptr)
    {
        return;
    }

    TM_GetSysDate(&date);
    TM_GetSysTime(&time);

    time_ptr->wYear = date.year;
    time_ptr->nMonth = date.mon;
    time_ptr->nDay = date.mday;
    time_ptr->nHour = time.hour;
    time_ptr->nMin = time.min; 
    time_ptr->nSec = time.sec;
}

/*****************************************************************************/
//  Description :获得手机系统时间的当天所处的ms值
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC QU32  GETTIMEMS(void)
{
    QTime time = {0};
    //SCI_TRACE_LOW("[QQ]GETTIMEMS");

    QGetDateTime(&time);
    return (QU32)(1000 * (time.nHour*60*60 + time.nMin*60 + time.nSec));
}

/*****************************************************************************/
//  Description :GETRAND
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC void GETRAND(QU8* random_ptr, int size)
{
    int i = 0;
    
    //SCI_TRACE_LOW("[QQ]GETRAND size= %d", size);

    if (random_ptr != PNULL)
    {
        srand(SCI_GetTickCount());
        i = rand() % size;//随机数范围限制
        *random_ptr = (QU8)i;
    }
    
    //SCI_TRACE_LOW("[QQ]GETRAND *random_ptr= %d", *random_ptr);
}

/*****************************************************************************/
//  Description :update the idle status of qq
//  Global resource dependence : none
//  Author: TENCENT
//  Note: 
/*****************************************************************************/
PUBLIC void QUPDATASTATUS(int status)
{
    s_qq_leave_state = status;
    
    //SCI_TRACE_LOW:"[QQ]QUPDATASTATUS =%d= %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIQQ_SYSTEM_714_112_2_18_2_49_12_44,(uint8*)"dd", s_qq_leave_state, status);
    
    MMIAPIIDLE_UpdateQQState();
    GUIWIN_UpdateStb();
    MMISUB_UpdateDisplaySubLcd(); 
}
/***FOR QQ System END***/

/*****************************************************************************/
//  Description :Get the state of QQ when leave it
//  Global resource dependence : s_qq_leave_state
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIQQ_GetLeaveQQState(void)//Get_leavev_qq_state_value(void)
{
    return s_qq_leave_state;
}

/*****************************************************************************/
//  Description: set s_is_login_selcet
//  Global resource dependence :  s_is_login_selcet
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIQQ_SetLoginSelect(BOOLEAN is_login_select)
{
    s_is_login_selcet = is_login_select;
}

/*****************************************************************************/
//  Description :get当前QQ的idle icon
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC  MMI_IMAGE_ID_T MMIAPIQQ_GetIdleImageID(void)
{
     MMI_IMAGE_ID_T image_id = IMAGE_NULL;

     //SCI_TRACE_LOW:"[QQ]MMIAPIQQ_GetIdleImageID = %d"
     SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIQQ_SYSTEM_747_112_2_18_2_49_12_45,(uint8*)"d", s_qq_leave_state);

     switch(s_qq_leave_state)
     {
        case MMIQQ_BUDDY_STAT_ONLINE:
            image_id = (MMI_IMAGE_ID_T)IMAGE_TENXUN_QQ_ONLINE;//IMAGE_TENXUN_QQ_ONLINE;
            break;
            
        case MMIQQ_BUDDY_STAT_AWAY:
            image_id = (MMI_IMAGE_ID_T)IMAGE_TENXUN_QQ_AWAY;
            break;
            
        case MMIQQ_BUDDY_STAT_INVISIBLE:
            image_id = (MMI_IMAGE_ID_T)IMAGE_TENXUN_QQ_INVISIBLE;
            break;
            
        case MMIQQ_BUDDY_STAT_HAVEMSG:
            image_id = (MMI_IMAGE_ID_T)IMAGE_TENXUN_QQ_HAVEMSG;
            break;
            
        case MMIQQ_BUDDY_STAT_OFFLINE:
            image_id = (MMI_IMAGE_ID_T)IMAGE_TENXUN_QQ_OFFLINE;
            break;
            
        default :
          //  image_id = (MMI_IMAGE_ID_T)IMAGE_TENXUN_QQ_OFFLINE;
            break;
     }

   return image_id;

}
/*****************************************************************************/
//  Description :打开QQ的sim卡(or 网络)选择窗口
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIQQ_EntrySelectNetworkWin()
{
#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE)
    MMK_CreateWin((uint32*)MMIQQ_SELECT_SIM_WIN_TAB, PNULL);
#endif
}

#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE) 
/*****************************************************************************/
//  Description :QQ的sim卡(or 网络)选择窗口处理函数
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 当wifi模块开启时,还涉及wifi选择
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSIMSelectWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    uint16              cur_selection = 0;
    MMI_CTRL_ID_T       ctrl_id = MMIQQ_CTRL_ID_SELECT_SIM;
    uint32     item_data = 0;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifndef WIFI_SUPPORT
        GUITITLE_SetTextId(win_id,MMITHEME_GetTitleCtrlId(),TXT_SIM_SEL,FALSE);
#else
        GUITITLE_SetTextId(win_id,MMITHEME_GetTitleCtrlId(),TXT_NETWORK_SELECT,FALSE);
#endif
#if !defined(MMI_MULTI_SIM_SYS_SINGLE) 
        MMIAPIPHONE_SetNeedUpdateNetworkStatusWin(win_id);
#endif
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;

    case MSG_FULL_PAINT:
        {
            cur_selection = GUILIST_GetCurItemIndex(ctrl_id);
            QQAppendSelectListItem(ctrl_id);
            GUILIST_SetCurItemIndex(ctrl_id, cur_selection);
        }
        break;

#if !defined(MMI_MULTI_SIM_SYS_SINGLE) 
    case MSG_GET_FOCUS:
        MMIAPIPHONE_SetNeedUpdateNetworkStatusWin(win_id);
        break;
#endif         
        
    case MSG_APP_OK:
    case MSG_CTL_OK:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        cur_selection = GUILIST_GetCurItemIndex(ctrl_id);
        GUILIST_SetCurItemIndex(ctrl_id, cur_selection);
        
        GUILIST_GetItemData(ctrl_id, cur_selection, &item_data);

        if(item_data < MMI_DUAL_SYS_MAX)
        {
            MMIQQ_SetSimSelect(item_data);
        }
        else
        {
#ifdef WIFI_SUPPORT
            s_qq_is_use_wifi = TRUE;
#endif
        }
        
        MMIQQ_SetLoginSelect(TRUE);  
        s_qq_logic_angle = MMK_GetScreenAngle();
        MMIQQ_EntryQQ();//进入QQ

        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_RED:
        MMK_CloseWin(win_id);//返回上一级
        break;
       
    case MSG_CLOSE_WINDOW:
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return (recode);
}
#endif

/*****************************************************************************/
//  Description :设置当前QQ使用的sim卡(sim1 or sim2)
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIQQ_SetSimSelect(MN_DUAL_SYS_E sim_select)
{
    if (sim_select >= MMI_DUAL_SYS_MAX)/*做个保护*/
    {
        sim_select = MN_DUAL_SYS_1;
    }
    s_qq_sim_select = sim_select;

    //SCI_TRACE_LOW:"[QQ] MMIQQ_SetSimSelect sim_select = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIQQ_SYSTEM_887_112_2_18_2_49_13_46,(uint8*)"d", s_qq_sim_select);
} 

/*****************************************************************************/
//  Description :获得当前QQ使用的sim卡(sim1 or sim2)
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMIQQ_GetSimSelect(void)
{
    return s_qq_sim_select;
}

/*****************************************************************************/
//  Description: QQ入口函数,调用此函数进入QQ 主界面
//  Global resource dependence :  
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIQQ_OpenMainWin(void)
{
    // uint32  sim_ok_num = 0;
    // uint16  sim_ok = MN_DUAL_SYS_1;

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
    else if (MMIAPIMTV_IsMTVRunning()) //CMMB播放中
    {
        MMIPUB_OpenAlertWarningWin(TXT_EXIT_MOBILETV);
        return FALSE;
    }

	
#ifdef MRAPP_SUPPORT
    if(MMIMRAPP_HandleEventFromOtherModule(MMI_MODULE_QQ))
        return FALSE;
#endif

    if(s_is_login_selcet) // 如果选卡OK  直接进入就行了
    {
        MMIQQ_EntryQQ();
        return TRUE;
    }
    else
    {
        if (!MMIQQ_IsGprsStatusReady()) //QQ的gprs状态是否已经准备好
        {
            MMIPUB_OpenAlertWarningWin(TXT_SYS_WAITING_AND_TRY_LATER);
            return FALSE;
        }
        
        if (MMIQQ_SelectNetworkType())
        {
            return TRUE;
        }       
        else
        {
            return FALSE;
        }
    }	
}

/*****************************************************************************/
//  Description: 得到当前QQ的lcd 逻辑角度
//  Global resource dependence :  s_qq_logic_angle
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC LCD_ANGLE_E MMIQQ_GetLogicAngle(void)
{
    return s_qq_logic_angle;
}

/*****************************************************************************/
//  Description :更新当前QQ:若当前QQ登录所使用的sim卡发生变化,则退出QQ
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIQQ_UpdateSimSelect(MN_DUAL_SYS_E sim_select)
{
    if (sim_select == MMIQQ_GetSimSelect())
    {
        ExitQQ();
    }
}

#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE) 
/*****************************************************************************/
//  Description : append item to listbox 
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void QQAppendSelectListItem(MMI_CTRL_ID_T ctrl_id)
{
#if defined(WIFI_SUPPORT)
    MMI_STRING_T sim_str = {0};
    GUILIST_ITEM_T      item_t = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
#endif    

#ifdef WIFI_SUPPORT
    GUILIST_SetMaxItem(ctrl_id, MMI_DUAL_SYS_MAX + 1, FALSE );
#else
    GUILIST_SetMaxItem(ctrl_id, MMI_DUAL_SYS_MAX, FALSE );
#endif

#ifndef MMI_MULTI_SIM_SYS_SINGLE 
    MMIAPISET_AppendMultiSimName(ctrl_id, TXT_COMMON_OK, MMI_DUAL_SYS_MAX, MMISET_APPEND_SYS_OK);
#else//only defined WIFI_SUPPORT
    item_t.item_style = GUIITEM_STYLE_ONE_LINE_RADIO;
    item_t.item_data_ptr = &item_data;
    item_t.user_data = MN_DUAL_SYS_1;

    MMI_GetLabelTextByLang(TXT_GPRS,&sim_str);
    
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[0].item_data.text_buffer.wstr_len = sim_str.wstr_len;
    
    item_data.item_content[0].item_data.text_buffer.wstr_ptr = sim_str.wstr_ptr;

    //set softkey
    item_data.softkey_id[0] = TXT_COMMON_OK;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = STXT_RETURN;
    
    GUILIST_AppendItem(ctrl_id, &item_t);
#endif

#ifdef WIFI_SUPPORT
    SCI_MEMSET(&item_t, 0x00, sizeof(item_t));
    SCI_MEMSET(&item_data, 0x00, sizeof(item_data));

    item_t.item_style = GUIITEM_STYLE_ONE_LINE_ICON_TEXT;
    item_t.item_data_ptr = &item_data;
    item_t.user_data = MMI_DUAL_SYS_MAX;//WIFI
    MMI_GetLabelTextByLang(TXT_WIFI,&sim_str);
    item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
    item_data.item_content[0].item_data.image_id = MAIN_GetIdleWifiIcon();
                              
    item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[1].item_data.text_buffer.wstr_len = sim_str.wstr_len;
    item_data.item_content[1].item_data.text_buffer.wstr_ptr = sim_str.wstr_ptr;

    //set softkey
    item_data.softkey_id[0] = TXT_COMMON_OK;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = STXT_RETURN;
    
    GUILIST_AppendItem(ctrl_id, &item_t);
#endif

    GUILIST_SetOwnSofterKey(ctrl_id, TRUE);
}
#endif

/*****************************************************************************/
//  Description : select network(sim卡or wifi) for QQ
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MMIQQ_SelectNetworkType(void)
{  
#ifndef WIFI_SUPPORT       
    MN_DUAL_SYS_E dual_sys = MMI_DUAL_SYS_MAX;
#endif
    BOOLEAN is_need_sel = FALSE;//记录是否需要进行网络选择
    uint16  sim_state_arr[MMI_DUAL_SYS_MAX] = {0};
    uint32  sim_num = 0;
    uint32  i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        sim_state_arr[i] = MN_DUAL_SYS_MAX; //init
        if (MMIAPIPHONE_IsSimAvailable(i))
        {
            sim_state_arr[i] = i;
            sim_num++;
        }
    }

#ifdef WIFI_SUPPORT
    s_qq_is_use_wifi = FALSE;  //reset
#endif
    
    switch (sim_num)
    {
    case 0:
    {
#ifdef WIFI_SUPPORT
        is_need_sel = FALSE; //only use wifi
        break;
#else                
        MMIAPIPHONE_AlertSimNorOKStatus();
        return FALSE;
#endif                
    }
        
    case 1:
    {
#ifdef WIFI_SUPPORT                        
        is_need_sel = TRUE;
#else
        for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
        {
            if (sim_state_arr[i] != MN_DUAL_SYS_MAX)
            {
                dual_sys = (MN_DUAL_SYS_E)i; //get and set the sim                       
                break;
            }
        }
        MMIQQ_SetSimSelect(dual_sys);
#endif
    }
        break;

    default:
        is_need_sel = TRUE;
        break;
    }

     //进入网络选择(or sim卡选择)
    if (is_need_sel)/*lint !e774*/ 
    {
        MMIQQ_EntrySelectNetworkWin();
        return TRUE;
    }    
    else
    {
#ifdef WIFI_SUPPORT
        s_qq_is_use_wifi = TRUE;
#endif
        //选卡完成
        s_is_login_selcet = TRUE;  
        //进入QQ主界面
        s_qq_logic_angle = MMK_GetScreenAngle();//记录非后台进入QQ主界面的lcd 角度
        MMIQQ_EntryQQ();
        return TRUE;
    }
}


/*****************************************************************************/
//  Description :得到s_qq_is_use_wifi
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIQQ_GetIsUseWifi(void)
{
#ifdef WIFI_SUPPORT
    return s_qq_is_use_wifi;
#else
    return FALSE;
#endif
}

/*****************************************************************************/
//  Description :退出QQ
//  Global resource dependence : none
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIQQ_ExitQQ(void)
{
    ExitQQ();
}

#endif
