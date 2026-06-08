/****************************************************************************
** File Name:      mmisms_main.c                                           *
** Author:                                                                 *
** Date:           04/11/2004                                              *
** Copyright:      2004 Spreadtrum, Incorporated. All Rights Reserved.     *
** Description:    This file is used to describe the SMS                   *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 04/2004        louis.wei        Create
** 06/2006          Tracy Zhang       Modify for SP7550
** 
****************************************************************************/
#define _MMISMS_MAIN_C_  

/**-------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "mmi_app_sms_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "window_parse.h"
#include "guitext.h"
#include "mmi_text.h"
#include "mmi_image.h"
#include "mmiidle_export.h"
#include "tb_dal.h"
#include "mmipb_export.h"
#if defined(MMI_SMSCB_SUPPORT)
#include "mmismscb_export.h"
#endif
#include "mmistk_export.h"
#include "mmi_atc.h"
#include "mmiphone_export.h"
#include "mmi_textfun.h"
#include "guilistbox.h"
#include "mmi_appmsg.h"
#include "mmiidle_statusbar.h"
#include "mmiset_export.h"
#include "mmikl_export.h"
#include "mmi_default.h"
#include "mmi_nv.h"
//#include "nvitem.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "mmipub.h"
#include "mmisms_text.h"
#include "mmisms_id.h"
#include "mmisms_app.h"
#include "mmismsapp_wintab.h"
#include "mmisms_menutable.h"
//#include "sci_types.h"
#include "mmisms_app.h"
#include "mmisms_order.h"
#include "mmisms_receive.h"
#include "mmisms_save.h"
#include "mmisms_read.h"
#include "mmisms_edit.h"
#include "mmisms_delete.h"
#include "mmisms_set.h"
#include "mmisms_send.h"
#include "mmisms_delete.h"
#include "mmisms_voicemail.h"
#include "mmisms_delete.h"
#include "mmivcard_export.h"
#include "gui_ucs2b_converter.h"
#ifdef MMI_TIMERMSG_SUPPORT
#include "mmisms_timermsg.h"
#include "mmi_event_api.h"
#endif

#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
#ifdef PDA_UI_DROPDOWN_WIN
#include "mmidropdownwin_export.h"
#endif

#ifdef IKEYBACKUP_SUPPORT
#include "mmiikeybackup_api.h"
#endif
#ifdef SXH_APPLE_SUPPORT
#include "OEMOS.h"
#endif

/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/
#define MMISMS_SEND_RECEIVE_FUNC_REQ_TIMES 10

/**-------------------------------------------------------------------------*
**                         GLOBAL DEFINITION                                *
**--------------------------------------------------------------------------*/ 
MMI_APPLICATION_T g_mmisms_app;                // SMS的实体
MMISMS_GLOBAL_T g_mmisms_global;            // SMS的全局变量

#ifdef IKEYBACKUP_SUPPORT   
MMISMS_BACKUP_DATA_T *g_sms_backup_data_ptr = PNULL;//备份全局变量
MMIIKB_BACKUP_ITEM_DATA_REQ_T *g_backup_item_data_req_ptr = PNULL;
MMIIKB_RESTORE_CALLBACK_PARAM_IN_T *g_restore_param_in_ptr = PNULL;
BOOLEAN g_sms_is_backup = FALSE;
MN_SMS_STATUS_E   g_sms_restore_status = MN_SMS_FREE_SPACE;
MMISMS_FOLDER_TYPE_E g_sms_restore_folder_type = MMISMS_FOLDER_NORMAL;
MMISMS_MO_MT_TYPE_E g_mo_mt_type = MMISMS_MO_DRAFT;
uint32 g_sms_save_time = 0;
#endif

MMISMS_SET_VM_ADDR_STATUS_T g_set_voimess_info = {0};   //向sim卡存储语音信箱
BOOLEAN g_comming_sms_java_running = FALSE;//java sms是否在运行

/**--------------------------------------------------------------------------*
**                         STATIC DEFINITION                                *
**--------------------------------------------------------------------------*/
LOCAL uint8 s_sim_sms_num[MMI_DUAL_SYS_MAX] = {0};
LOCAL BOOLEAN s_is_sms_need_re_sort= FALSE;
LOCAL BOOLEAN s_is_sms_refresh[MMI_DUAL_SYS_MAX] = {0};

/**--------------------------------------------------------------------------*
**                         EXTERN DECLARE                                *
**--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//     Description : Handle SIM Plug Notify SMS
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
LOCAL void HandleSIMPlugNotifySMSFunc(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E notify_event, uint32 param);

/*****************************************************************************/
// 	Description : notify callback
//	Global resource dependence :
//  Author: 
//	Note: 
/*****************************************************************************/
LOCAL void HandleSMSNotifyEventCallback(MN_DUAL_SYS_E dual_sys, MMIPHONE_NOTIFY_EVENT_E notify_event, uint32 param);

#ifdef MMI_TIMERMSG_SUPPORT
/*******************************************************************/
//  Interface:		HandleSMSTimeUpdateCallback
//  Description : 	HandleSMSTimeUpdateCallback
//  Global resource dependence : 
//  Author: Cheng.luo
//  Note:
/*******************************************************************/
LOCAL void HandleSMSTimeUpdateCallback(uint32 old_time, uint32 new_time);
#endif

/*****************************************************************************/
//     Description : handle APP_MN_SMS_READY_IND message
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN MMISMS_HandleSmsReadyInd(DPARAM    param);

/*****************************************************************************/
//     Description : handle APP_MN_SIM_NOT_READY_IND message
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN MMISMS_HandleSimNotReadyInd(DPARAM    param);

/*****************************************************************************/
//     Description : the interface to handle SMS protocol message
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePsMsg(
                               PWND        app_ptr,    //IN:
                               uint16    msg_id,        //IN:
                               DPARAM    param        //IN:
                               );

/*****************************************************************************/
//     Description : to get the valid period from NV
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitValidPeriod( void );      

/*****************************************************************************/
//     Description : to handle ps signal of APP_MN_SET_SMS_RECEIVE_FUNC_CNF
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void HandleSetSMSReceiveCnf (
                                   DPARAM param
                                   );
#ifndef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of atuo save
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitAutoSave( void );
#endif
/*****************************************************************************/
//     Description : to init the order type
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitOrderType(void);

/*****************************************************************************/
//     Description : to init the flag of status report
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void InitStatusReportFlag(void);

#ifdef SMS_AUTO_SIGNATURE_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of auto signature
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void InitAutoSignatureFlag(void);
#endif

/*****************************************************************************/
//     Description : to get the reference number form NV
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void GetRefNumFormNV(void);

/*****************************************************************************/
//     Description : to handle mem-full ind
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void HandleMemFullInd(
                            DPARAM param    //IN:
                            );

/*****************************************************************************/
//     Description : to init the preferred connection
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
LOCAL void InitPreferredConnection(void);

/*****************************************************************************/
//     Description : to init the save postion
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
LOCAL void InitSavePos(void);

/*****************************************************************************/
//     Description : 开启APP发起的SMS初始化
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
LOCAL void StartAppInitSmsList(MN_DUAL_SYS_E dual_sys, MNSIM_ERROR_E sim_error_reason);

/*****************************************************************************/
//  Description : Init Folder Type flag
//  Global resource dependence : 
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitFolderTypeFlag(void);

/*****************************************************************************/
//     Description : to init the flag of security pwd
//    Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
LOCAL void InitSecurityPwd(void);

#if defined(MMI_SMS_EXPORT_SUPPORT)
/*****************************************************************************/
//     Description : to init the flag of export method
//    Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
LOCAL void InitExportMethod(void);
#endif

#if defined(MMI_SMS_FONT_SETTING_SUPPORT)
/*****************************************************************************/
//  Description : to init the font size
//  Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitFontSize(void);
#endif

#ifdef MMI_SMS_REPLY_PATH_SUPPORT
/*****************************************************************************/
//  Description : to init the flag of reply path
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitReplyPathFlag(void);
#endif

#ifdef MMI_SMS_MULTI_PARAM_SUPPORT
/*****************************************************************************/
//  Description : to init the flag of sim mode
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitSIMModeFlag(void);
#endif

#ifdef MMI_SMS_CONCATENATED_SETTING_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of concatenated sms
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitContcatenatedSMS(void);
#endif

#ifdef MMI_SMS_ALPHABET_SETTING_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of alphabet management
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitAlphabetManagement(void);
#endif
#ifdef IKEYBACKUP_SUPPORT
/*****************************************************************************/
//  Description : SMS一键备份还原注册函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL BOOLEAN sms_ikeybackup_register(void);

/*****************************************************************************/
//  Description : SMS一键备份函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL void sms_backup_callback(
                               MMIIKB_BACKUP_CALLBACK_PARAM_IN_T *param_in_ptr,
                               MMIIKB_BACKUP_CALLBACK_PARAM_OUT_T *param_out_ptr
                               );

/*****************************************************************************/
//  Description : SMS一键备份还原获取数量函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL void sms_get_total_callback(
                                  MMIIKB_GET_TOTAL_OUT_T *param_out_ptr
                                  );

/*****************************************************************************/
//  Description : SMS一键还原函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL void sms_restore_callback(
                                MMIIKB_RESTORE_CALLBACK_PARAM_IN_T *param_in_ptr,
                                MMIIKB_RESTORE_CALLBACK_PARAM_OUT_T *param_out_ptr
                                );

#endif

/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/

#ifndef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of auto save
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitAutoSave( void )
{
    MMINV_SMS_AUTOSAVE_T flag = FALSE;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;    
    
    MMINV_READ(MMINV_SMS_AUTOSAVE_FLAG, &flag, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        flag = MMISMS_AUTOSAVE_FLAG_DEFAULT;
        MMISMS_SetAutoSave( flag );
    }    
    g_mmisms_global.nv_param.auto_save_setting = flag;        
}
#endif

/*****************************************************************************/
//     Description : to init the order type
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitOrderType(void)
{
    MMISMS_ORDERTYPE_E order_type  = MMISMS_ORDER_BY_TIME;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;    
    
    MMINV_READ(MMINV_SMS_ORDER_TYPE, &order_type, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        order_type = MMISMS_ORDER_BY_TIME;
        MMINV_WRITE(MMINV_SMS_ORDER_TYPE, &order_type);
    }    
    MMISMS_SetOrderType(order_type);
}

/*****************************************************************************/
//     Description : to init the preferred connection
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
LOCAL void InitPreferredConnection(void)
{
    MMISMS_PREFERRED_PATH_E preferred_connection = MMISMS_PREFERRED_PATH_GSM_PREFERRED;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_PREFERRED_CONNECTION, &preferred_connection, nv_value);

    if (MN_RETURN_SUCCESS !=nv_value)
    {
        preferred_connection = MMISMS_PREFERRED_PATH_GSM_PREFERRED;
        MMINV_WRITE(MMINV_SMS_PREFERRED_CONNECTION, &preferred_connection);
    }
    
    g_mmisms_global.nv_param.preferred_connection = preferred_connection;
}

/*****************************************************************************/
//     Description : to init the save postion
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
LOCAL void InitSavePos(void)
{
    MMISMS_SMS_SAVEPOS_TYPE_E pos = MMISMS_SAVEPOS_NONE;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    BOOLEAN is_sim_first = FALSE;
    uint32 i = 0;
    
    MMINV_READ(MMINV_SMS_SAVE_POS, &pos, nv_value);
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        pos = MMISMS_SAVEPOS_PHONE; //修改默认的保存位置为手机
        MMINV_WRITE(MMINV_SMS_SAVE_POS, &pos);
    }

    if(MMISMS_SAVEPOS_SIM == pos)
    {
        is_sim_first = TRUE;
    }
    else
    {
        is_sim_first = FALSE;
    }

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        MNSMS_SetStoragePriorityEx(i, is_sim_first);
    }
}

/*****************************************************************************/
//     Description : to init the flag of status report
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void InitStatusReportFlag(void)
{
    MMINV_SMS_STATUS_REPORT_T flag = FALSE;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_STATUS_REPORT_FLAG, &flag, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        flag = MMISMS_STATUSREPORT_FLAG_DEFAULT;
        MMISMS_SetStatusReportFlag(flag);
    }
    
    g_mmisms_global.nv_param.status_report_flag = flag;        
}

#ifdef SMS_AUTO_SIGNATURE_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of auto signature
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void InitAutoSignatureFlag(void)
{
    BOOLEAN flag = FALSE;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_AUTO_SIGNATURE_FLAG, &flag, nv_value);
    
    if (MN_RETURN_SUCCESS != nv_value)
    {
        flag = MMISMS_AUTO_SIGNATURE_FLAG_DEFAULT;
        MMISMS_SetAutoSignatureFlag(flag);
    }
    
    g_mmisms_global.nv_param.auto_signature_flag = flag;        
}
#endif

/*****************************************************************************/
//     Description : to get the valid period from NV
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitValidPeriod(void)
{
    MMINV_SMS_VALID_PERIOD_T vaild_period = MMISMS_VALID_PERIOD_DEFAULT;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_VALID_PERIOD, &vaild_period, nv_value);
    
    if (MN_RETURN_SUCCESS != nv_value)
    {
        // set default value to NV
        vaild_period = MMISMS_VALID_PERIOD_DEFAULT;
        MMISMS_SetValidPeriod(vaild_period);
    }
    g_mmisms_global.nv_param.vaild_period = vaild_period;    
}

/*****************************************************************************/
//     Description : to init the flag of browser info
//    Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
LOCAL void InitBrowserInfoFlag(void)
{
    BOOLEAN flag = MMISMS_BROWSERINFO_FLAG_DEFAULT;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_BROWSER_INFO, &flag, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        flag = MMISMS_BROWSERINFO_FLAG_DEFAULT;
        MMISMS_SetPushSmsBrowserInfo(flag);
    }
    
    g_mmisms_global.nv_param.browser_info_flag = flag;        
}

/*****************************************************************************/
//     Description : to init the flag of browser info
//    Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
LOCAL void InitIsExtractFlag(void)
{
    BOOLEAN flag = MMISMS_ISEXTRACT_FLAG_DEFAULT;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_IS_EXTRACT, &flag, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        flag = MMISMS_ISEXTRACT_FLAG_DEFAULT;
        MMISMS_SetSMSIsExtract(flag);
    }
    
    g_mmisms_global.nv_param.is_extract_flag = flag;        
}

#if defined(MMI_SMS_FONT_SETTING_SUPPORT)
/*****************************************************************************/
//  Description : to init the font size
//  Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitFontSize(void)
{
    MMISMS_FONT_E font_size  = MMISMS_FONT_NORMAL;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;    
    
    MMINV_READ(MMINV_SMS_FONT_SIZE, &font_size, nv_value);
    
    if (MN_RETURN_SUCCESS != nv_value)
    {
        font_size = MMISMS_FONT_NORMAL;
        MMINV_WRITE(MMINV_SMS_FONT_SIZE, &font_size);
    }    
    MMISMS_SetFontSize(font_size);
}
#endif

#if 1
#ifdef JAVA_SUPPORT
/*****************************************************************************/
//     Description : is MT for java
//    Global resource dependence : 
//  Author: kelly.li
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsMtForJava(
                              MN_SMS_USER_HEAD_T user_head_ptr_t,
                              int32                    *des_port_ptr,
                              int32                    *org_port_ptr
                              )
{
#if 0
    int32 port_test1 = 0;
    int32 port_test2 = 0;
#endif
    uint8 iei_num = 0;
    uint8 i = 0;
    // int32 j = 0;
    int32 index= -1;
    int32 des_port_num = 0,org_port_num = 0;
    BOOLEAN is_for_java = FALSE;
    // int32 *port_num_buff_ptr = PNULL;
    // int32 port_num = 0;
    // int32 ret_value = 0;
    
    iei_num = user_head_ptr_t.iei_num;
    
    for (i = 0; i<iei_num;i++)
    {
        //SCI_TRACE_LOW:"MMISMS MMISMS_IsMtForJava: i value is = %d  .iei=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_570_112_2_18_2_53_47_507,(uint8*)"dd",i,user_head_ptr_t.iei_arr[i].iei);
        
        if((user_head_ptr_t.iei_arr[i].iei == MMISMS_HEAD_IEI_FOR_PORT)
            ||(user_head_ptr_t.iei_arr[i].iei == MMISMS_HEAD_IEL_FOR_PORT))
        {
            index = i;
            break;
        }
        else
        {
            index = -1;
        }
    }
    
    if (-1 == index)
    {
        //SCI_TRACE_LOW:"MMISMS MMISMS_IsMtForJava: There is no java port number!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_586_112_2_18_2_53_47_508,(uint8*)"");
        return FALSE;
    }
    else
    {
        SCI_TRACE_BUF("MMISMS MMISMS_IsMtForJava: user header is %x",user_head_ptr_t.iei_arr[index].iei_data_t, user_head_ptr_t.iei_arr[index].length);
        if (user_head_ptr_t.iei_arr[index].iei == MN_SMS_APP_PORT_8_BIT_ADDR)
        {
            //destination  port  number
            des_port_num = user_head_ptr_t.iei_arr[index].iei_data_t[0];
            org_port_num = user_head_ptr_t.iei_arr[index].iei_data_t[1];            
            
        }
        else if (user_head_ptr_t.iei_arr[index].iei == MN_SMS_APP_PORT_16_BIT_ADDR)
        {
            #if 0
            //destination  port  number
            port_test1 = user_head_ptr_t.iei_arr[index].iei_data_t[0] << 8;
            port_test2 = user_head_ptr_t.iei_arr[index].iei_data_t[1];
            //SCI_TRACE_LOW:"MMISMS MMISMS_IsMtForJava: port_test1 is = %d, tport_test2 is = %d "
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_605_112_2_18_2_53_47_509,(uint8*)"dd",port_test1, port_test2);
            des_port_num = port_test1 | port_test1;
            //            des_port_num = (user_head_ptr_t.iei_arr[index].iei_data_t[0] << 8) | (user_head_ptr_t.iei_arr[index].iei_data_t[1]);
            org_port_num = (user_head_ptr_t.iei_arr[index].iei_data_t[2] << 8) |(user_head_ptr_t.iei_arr[index].iei_data_t[3]);
            #endif
            des_port_num = (user_head_ptr_t.iei_arr[index].iei_data_t[0] << 8) | (user_head_ptr_t.iei_arr[index].iei_data_t[1]);
            org_port_num = (user_head_ptr_t.iei_arr[index].iei_data_t[2] << 8) |(user_head_ptr_t.iei_arr[index].iei_data_t[3]);

        }
        
        *des_port_ptr = des_port_num;
        *org_port_ptr = org_port_num;
        
        //SCI_TRACE_LOW:"MMISMS MMISMS_IsMtForJava: MT Ind the des port num is = %d, the src port is = %d "
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_618_112_2_18_2_53_47_510,(uint8*)"dd",des_port_num,org_port_num);
        
#ifdef JAVA_SUPPORT_IA
	    is_for_java = MMIAPIJAVA_CheckIfJavaPort(des_port_num,PNULL, FALSE);
#endif
        
        //SCI_TRACE_LOW:"MMISMS MMISMS_IsMtForJava: the port result is = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_624_112_2_18_2_53_47_511,(uint8*)"d",is_for_java);

        return is_for_java;
    }    
}
#endif //JAVA_SUPPORT
#endif

/*****************************************************************************/
//     Description : Handle SIM Plug Notify SMS
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
LOCAL void HandleSIMPlugNotifySMSFunc(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E notify_event, uint32 param)
{
    //SCI_TRACE_LOW:"[MMISMS]: HandleSIMPlugNotifySMSFunc dual_sys=%d, notify_event=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_638_112_2_18_2_53_47_512,(uint8*)"dd", dual_sys, notify_event);

    switch (notify_event)
    {
    case MMIPHONE_NOTIFY_SIM_PLUG_IN:
        MMIAPISMS_ExitFromSMS();
        MMISMS_ClearOperInfo();
        g_mmisms_global.is_sim_ready[dual_sys] = FALSE;
        MMISMS_SetIsSMSRefresh(dual_sys, TRUE);

        if (MMISMS_RefreshSMS(dual_sys))
        {
            MMISMS_SetIsNeedReSort(FALSE);
        }
        else
        {
            MMISMS_SetIsNeedReSort(TRUE);
        }
        break;

    case MMIPHONE_NOTIFY_SIM_PLUG_OUT:
        MMIAPISMS_ExitFromSMS();
        MMISMS_ClearOperInfo();
        if (MMISMS_RefreshSMS(dual_sys))
        {
            g_mmisms_global.is_sim_order_ok[dual_sys] = TRUE;
            MMISMS_SetIsNeedReSort(FALSE);

            MMISMS_InitSmsOrder();

            MMISMS_InitSmsList();
        }
        else
        {
            if (g_mmisms_global.is_sim_order_ok[dual_sys])
            {
                MMISMS_SetIsNeedReSort(TRUE);
            }
            else
            {
                g_mmisms_global.is_sim_order_ok[dual_sys] = TRUE;

                if (!MMISMS_GetIsNeedReSort())
                {
                    MMISMS_InitSmsOrder();

                    MMISMS_InitSmsList();
                }
            }
        }        
#ifndef MMI_MULTI_SIM_SYS_SINGLE
        MMK_CloseWin(MMISMS_COM_SELECT_SIM_WIN_ID);
#endif
        break;

    default:
        break;
    }
}

/*****************************************************************************/
// 	Description : notify callback
//	Global resource dependence :
//  Author: 
//	Note: 
/*****************************************************************************/
LOCAL void HandleSMSNotifyEventCallback(MN_DUAL_SYS_E dual_sys, MMIPHONE_NOTIFY_EVENT_E notify_event, uint32 param)
{
    //SCI_TRACE_LOW:"[MMISMS]: HandleSMSNotifyEventCallback dual_sys=%d, notify_event=%d, param=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_704_112_2_18_2_53_47_513,(uint8*)"ddd", dual_sys, notify_event, param);

    //无网恢复有网时
   if(MMIPHONE_NOTIFY_EVENT_CS_AVAIL == notify_event)
    {
#ifdef MMI_TIMERMSG_SUPPORT
        MMIAPISMS_CheckTimerMSG();
#endif
    }
}

#ifdef MMI_TIMERMSG_SUPPORT
/*******************************************************************/
//  Interface:		HandleSMSTimeUpdateCallback
//  Description : 	HandleSMSTimeUpdateCallback
//  Global resource dependence : 
//  Author: Cheng.luo
//  Note:
/*******************************************************************/
LOCAL void HandleSMSTimeUpdateCallback(uint32 old_time, uint32 new_time)
{
    MMISMS_TIMERMSG_CheckTimerMSG(TRUE);
}
#endif

/*****************************************************************************/
//     Description : init the SMS global
//    Global resource dependence : g_mmisms_app, 
//                               g_mmisms_global,
//                               
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_AppInit(void)
{    
    MMIPHONE_SIM_PLUG_NOTIFY_INFO_T sim_plug_notify_info = {0};
    MMIPHONE_NOTIFY_INFO_T notify_info = {0};
    
    MMIAPISMS_SetSavePush(TRUE);//允许保存mms，不让开机无法收到mms forcr 232529
    
    //get the NV param
    InitValidPeriod();
#ifndef MMISMS_CHATMODE_ONLY_SUPPORT    
    InitAutoSave();
#endif
    InitStatusReportFlag();
#ifdef SMS_AUTO_SIGNATURE_SUPPORT    
    /*auto signature*/
    InitAutoSignatureFlag();
#endif
    InitPreferredConnection();
    InitSavePos();
    InitOrderType();
    GetRefNumFormNV();
    MMISMS_InitPresetSms();
    InitBrowserInfoFlag();
    InitIsExtractFlag();
    InitFolderTypeFlag();
    InitSecurityPwd();    
#if defined(MMI_SMS_EXPORT_SUPPORT)
    InitExportMethod();
#endif
#if defined(MMI_SMS_FONT_SETTING_SUPPORT)
    InitFontSize();
#endif
#ifdef MMI_SMS_MULTI_PARAM_SUPPORT
    InitSIMModeFlag();
#endif
#ifdef MMI_SMS_CONCATENATED_SETTING_SUPPORT
    InitContcatenatedSMS();
#endif
#ifdef MMI_SMS_ALPHABET_SETTING_SUPPORT
    InitAlphabetManagement();
#endif

#ifdef TTS_SUPPORT
    MMISMS_InitTTSFlag();
    MMISMS_InitTTSVol();
#endif
#ifdef MMI_TIMERMSG_SUPPORT
    MMISMS_TIMERMSG_Init();
#endif
    //set the status of voice message cnf
    g_set_voimess_info.is_get_set_vm_addr_cnf = FALSE;
    g_set_voimess_info.is_set_vm_addr_success = FALSE;
    
    sim_plug_notify_info.module_id = MMI_MODULE_SMS;
    sim_plug_notify_info.notify_func = HandleSIMPlugNotifySMSFunc;
    MMIAPIPHONE_RegisterSIMPlugNotifyInfo(&sim_plug_notify_info);
    MMK_PBRegisterEvent(MMISMS_HandlePBEventFunc);
    
    notify_info.module_id = MMI_MODULE_SMS;
    notify_info.notify_func = HandleSMSNotifyEventCallback;
    MMIAPIPHONE_RegisterNotifyInfo(&notify_info);

#ifdef IKEYBACKUP_SUPPORT
    sms_ikeybackup_register();
#endif

#ifdef MMI_TIMERMSG_SUPPORT
    MMIFW_SetTimeUpdateListerner(HandleSMSTimeUpdateCallback, TRUE);
#endif
}

/*****************************************************************************/
//     Description : init the SMS for PS
//    Global resource dependence :
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_InitForPS(void)
{
    uint32 i = MN_DUAL_SYS_1;
    
    //init the global variable
    MMISMS_InitGlobalInfo();
    
    //register the callback function for SMS write and read in NV
    /**************************************************************************/
    /*NOTE: If want MMI to store SMS item, the following code needed, and the */
    /*      register function MNSMS_RegReadNVSmsFunc/MNSMS_RegWriteNVSmsFunc  */
    /*      must been run before the function MNSMS_InitNvSms                 */           
    /*****************************START****************************************/
    MNSMS_RegReadNVSmsFunc(MMISMS_NV_ReadSmsItem);
    MNSMS_RegWriteNVSmsFunc(MMIAPISMS_NV_WriteSmsItem);
    MNSMS_RegSetMaxNVSmsNumFunc(MMISMS_NV_SetMaxSmsNum);
    MNSMS_RegDelAllSmsInNvFunc(MMISMS_NV_DelAllSmsItem);
    MNSMS_InitNvSmsEx(MN_DUAL_SYS_1);
    /**************************************************************************/
    /*NOTE: If want MMI to store SMS item, the above code needed, and the     */
    /*      register function MNSMS_RegReadNVSmsFunc/MNSMS_RegWriteNVSmsFunc  */
    /*      must been run before the function MNSMS_InitNvSms                 */           
    /*****************************END******************************************/
    
    //set the ps callback function
    g_mmisms_app.ProcessMsg = HandlePsMsg;         
    
    MMISMS_InitSortStatusAccordingToSetting();

    for(i=MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
    {
        MMISMS_SetMaxSimSmsNum((MN_DUAL_SYS_E)i, 0);
    }
    
    
#ifdef SXH_APPLE_SUPPORT
	OEMTAPI_Init();
#endif
}

/*****************************************************************************/
//     Description : the interface to handle SMS protocol message
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePsMsg(
                               PWND        app_ptr,    //IN:
                               uint16    msg_id,        //IN:
                               DPARAM    param        //IN:
                               )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    if (PNULL == app_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:HandlePsMsg app_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_868_112_2_18_2_53_47_514,(uint8*)"");
        return MMI_RESULT_FALSE;
    }
    //SCI_TRACE_LOW:"MMISMS:HandlePsMsg, msg_id=%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_871_112_2_18_2_53_47_515,(uint8*)"d", msg_id);
    switch ( msg_id )
    {
    case APP_MN_SMS_READY_IND:
        #ifdef JAVA_SUPPORT_SUN
        #ifndef _MSC_VER
        //SCI_TRACE_LOW:"HandlePsMsg APP_MN_SMS_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_877_112_2_18_2_53_47_516,(uint8*)"");
        APPSMS_SmsReady_CallBack();
        #endif
        #endif		
        //SCI_TRACE_LOW:"SMS: receive APP_MN_SMS_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_881_112_2_18_2_53_47_517,(uint8*)"");
        MMISMS_HandleSmsReadyInd(param);
        break;
        
    case APP_MN_SIM_NOT_READY_IND:
        //SCI_TRACE_LOW:"MMISMS: receive APP_MN_SIM_NOT_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_886_112_2_18_2_53_47_518,(uint8*)"");
        MMISMS_HandleSimNotReadyInd(param);   //%discuss
        result = MMI_RESULT_FALSE;       
        break;
        
    case APP_MN_READ_SMS_CNF:
        //SCI_TRACE_LOW:"MMISMS: receive APP_MN_READ_SMS_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_892_112_2_18_2_53_47_519,(uint8*)"");
        MMISMS_HandleReadSmsCnf(param); 
        break;
        
    case APP_MN_UPDATE_SMS_STATE_CNF:
        //SCI_TRACE_LOW:"MMISMS: receive APP_MN_UPDATE_SMS_STATE_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_897_112_2_18_2_53_47_520,(uint8*)"");
        MMISMS_HandleUpdateSMSCnf(param);
        break;
        
    case APP_MN_SEND_SMS_CNF:
        MMISMS_HandleSendCnf(param);
        break;
        
    case APP_MN_SMS_IND:
        MMISMS_HandleSMSMTInd( param );
        break;
        
    case APP_MN_SMS_STATUS_REPORT_IND:
        MMISMS_HandleSMSReportInd(param);
        break;
        
    case APP_MN_EDIT_SMS_CNF :
        MMISMS_HandleEditSMSCnf( param );
        break;
        
    case APP_MN_SMS_MEM_FULL_IND:
        HandleMemFullInd( param );
        break;
        
#ifdef SMS_OVERFLOW_ENABLE
        /* 2007.12.17 */
        /* samuel_for_sms_over_flow */
    case APP_MN_SMS_MEM_OVER_FLOW_IND:
        //SCI_TRACE_LOW:"mmi receive APP_MN_SMS_MEM_OVER_FLOW_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_925_112_2_18_2_53_48_521,(uint8*)"");
        MMIPUB_OpenAlertWarningWin(TXT_SMS_MEMORY_FULL);
        break;
        /* samuel_for_sms_over_flow */    
        /* 2007.12.17 */
#endif
        
    case APP_MN_WRITE_SMS_CNF:
        MMISMS_HandleSaveSMSCnf(param);
        break;

    case APP_MN_WRITE_SMS_PARAM_CNF:
        MMISMS_HandleWriteSMSParamCnf(param);
        break;

    case APP_MN_SET_SMS_RECEIVE_FUNC_CNF:
        if( ( PLMN_NO_SERVICE != MMIAPIPHONE_GetPlmnStatus(MMIAPISET_GetActiveSim())) &&
            ( PLMN_EMERGENCY_ONLY != MMIAPIPHONE_GetPlmnStatus(MMIAPISET_GetActiveSim())))
        {
            HandleSetSMSReceiveCnf( param );
        }
        break;
        
#if defined(MMI_SMSCB_SUPPORT)
        // 提示用户收到了新的普通小区广播
    case APP_MN_SMSCB_MSG_IND:
        MMIAPISMSCB_HandleCBMsgInd(param);
        break;
        
        // 提示用户收到了新的长小区广播
    case APP_MN_SMSCB_MACRO_MSG_IND:
        MMIAPISMSCB_HandleMarcoCBMsgInd(param);
        break;
        
        // 提示普通小区广播服务被拒绝
    case APP_MN_SMSCB_REJ_IND:
        break;
#endif
        
        //提示将语音信箱号码存入sim卡的结果
    case APP_MN_SET_VMW_ADDR_CNF: 
        MMISMS_HandleSetVoiMessAddrCnf( param );
        break;
        
        //提示从sim卡里读取语音信箱号码的结果
    case APP_MN_GET_VMW_ADDR_CNF: 
        MMISMS_HandleGetVoiMessAddrCnf( param );
        break;
        
        //提示从sim读语音信箱新消息flag 
    case APP_MN_GET_VMW_FLAG_CNF:
        MMISMS_HandleGetVoiMessFlagCnf( param );
        break;
        
        //提示有新的语音信息
    case APP_MN_VMW_FALG_IND:
        MMISMS_HandleNewVMFlagInd( param );
        break;

        //移动短信
    case MMISMS_MSG_MOVE_NEXT:
        MMISMS_MoveAllMarkedMsg(MMISMS_GetCurBoxType());
        break;

        //复制短信
    case MMISMS_MSG_COPY_NEXT:    
        MMISMS_CopyAllMarkedMsg(MMISMS_GetCurBoxType());
        break;
		
#ifdef SEARCH_SUPPORT
    case APP_MN_SEARCH_NEXT_SMS_CNF: 
        MMISMS_HandleSearchNextSms();
        break;
#endif

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return ( result );
}
#ifndef    MMISMS_CHATMODE_ONLY_SUPPORT 
/*****************************************************************************/
//     Description : to get the flag of auto_save
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC MMINV_SMS_AUTOSAVE_T MMISMS_GetAutoSave( void )
{
    MMINV_SMS_AUTOSAVE_T auto_save_setting = MMINV_SMS_SETTING_SAVE;

    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;
    
    MMINV_READ( MMINV_SMS_AUTOSAVE_FLAG, &auto_save_setting, return_value );
    if (MN_RETURN_SUCCESS != return_value)
    {
        auto_save_setting = g_mmisms_global.nv_param.auto_save_setting;
    }

    return auto_save_setting;
}
#endif

#ifdef SXH_APPLE_SUPPORT
void * APPLE_GetHandleMsg()
{
	return (void*)HandlePsMsg;
}

#endif
#ifndef MMISMS_CHATMODE_ONLY_SUPPORT 
/*****************************************************************************/
//     Description : to Set the status report setting
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetAutoSave (
                                MMINV_SMS_AUTOSAVE_T    auto_save_setting
                                )
{
    MMINV_WRITE(MMINV_SMS_AUTOSAVE_FLAG, &auto_save_setting);
    
    g_mmisms_global.nv_param.auto_save_setting = auto_save_setting;
}
#endif
/*****************************************************************************/
//     Description : to get the flag of status report
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetStatusReportFlag(void)
{
    BOOLEAN status_report_flag = FALSE;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;
    
    MMINV_READ( MMINV_SMS_STATUS_REPORT_FLAG, &status_report_flag, return_value );
    if (MN_RETURN_SUCCESS != return_value)
    {
        status_report_flag = g_mmisms_global.nv_param.status_report_flag;
    }

    return status_report_flag;
}

#ifdef SMS_AUTO_SIGNATURE_SUPPORT
/*****************************************************************************/
//     Description : allow to add auto signature
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_AllowAutoSignature(void)
{
    BOOLEAN is_show_sig = FALSE;
#ifdef MMI_SMS_CHAT_SUPPORT	
    MMI_CTRL_ID_T active_ctrl_id = 0;
    MMISMS_ORDER_ID_T cur_order_id = PNULL;
    active_ctrl_id = MMK_GetActiveCtrlId(MMISMS_SHOW_CHAT_SMS_WIN_ID);
    cur_order_id = g_mmisms_global.chat_item[active_ctrl_id - MMISMS_SHOW_CHAT_SMS_ITEM_FORM_CTRL_ID].cur_order_id;
#endif

    is_show_sig = MMISMS_GetAutoSignatureFlag() 
        && MMISMS_BOX_SENDSUCC != MMISMS_GetCurBoxType() && 
        MMISMS_BOX_SENDFAIL != MMISMS_GetCurBoxType();

#ifdef MMI_SMS_CHAT_SUPPORT
    if (MMISMS_BOX_CHAT == MMISMS_GetCurBoxType() && 
        (MMISMS_MO_SEND_SUCC == cur_order_id->flag.mo_mt_type || MMISMS_MO_SEND_FAIL == cur_order_id->flag.mo_mt_type)
        )
    {
        is_show_sig = FALSE;
    }
#endif        
    return is_show_sig;
}

/*****************************************************************************/
//     Description : to get the flag of sms auto signature
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetAutoSignatureFlag(void)
{
    BOOLEAN             auto_signature_flag = FALSE;
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    
    MMINV_READ( MMINV_SMS_AUTO_SIGNATURE_FLAG, &auto_signature_flag, return_value );
    if (MN_RETURN_SUCCESS != return_value)
    {
        auto_signature_flag = g_mmisms_global.nv_param.auto_signature_flag;
    }

    return auto_signature_flag;
}

/*****************************************************************************/
//     Description : to get the flag of sms auto signature content
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC uint16 MMISMS_GetAutoSignatureContent(wchar *signature_str_ptr)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint16              signature_str_len = 0;
    
    MMINV_READ( MMINV_SMS_AUTO_SIGNATURE_CONTENT, signature_str_ptr, return_value );
    if (MN_RETURN_SUCCESS != return_value)
    {
        SCI_MEMSET(signature_str_ptr, 0, signature_str_len);
        MMINV_WRITE(MMINV_SMS_AUTO_SIGNATURE_CONTENT, signature_str_ptr);
        signature_str_len = 0;
    }
    else
    {
        signature_str_len = (uint16)MMIAPICOM_Wstrlen(signature_str_ptr);
    }

    return signature_str_len;
}

/*****************************************************************************/
//     Description : to Set the sms auto signature content
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetAutoSignatureContent(
                                       wchar    *signature_str_ptr, //OUT:
                                       uint8    signature_str_len //IN:
                                       )
{
    MMINV_WRITE(MMINV_SMS_AUTO_SIGNATURE_CONTENT, signature_str_ptr);
}

/*****************************************************************************/
//     Description : to Set the sms auto signature flag
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetAutoSignatureFlag(
                                       BOOLEAN    auto_signature_flag
                                       )
{
    MMINV_WRITE(MMINV_SMS_AUTO_SIGNATURE_FLAG, &auto_signature_flag);
    
    g_mmisms_global.nv_param.auto_signature_flag = auto_signature_flag;
}
#endif

/*****************************************************************************/
//     Description : to Set the status report flag
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetStatusReportFlag(
                                       BOOLEAN    status_report_flag
                                       )
{
    MMINV_WRITE(MMINV_SMS_STATUS_REPORT_FLAG, &status_report_flag);
    
    g_mmisms_global.nv_param.status_report_flag = status_report_flag;
}

#ifdef MMI_SMS_REPLY_PATH_SUPPORT
/*****************************************************************************/
//  Description : to init the flag of reply path
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitReplyPathFlag(void)
{
    MMINV_SMS_REPLY_PATH_T flag = FALSE;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    
    MMINV_READ(MMINV_SMS_REPLY_PATH_FLAG, &flag, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        flag = MMISMS_REPLY_PATH_FLAG_DEFAULT;
        MMISMS_SetReplyPathFlag(flag);
    }
}

/*****************************************************************************/
//  Description : to get the flag of reply path
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetReplyPathFlag(void)
{
    BOOLEAN reply_path_flag = FALSE;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;
    
    MMINV_READ( MMINV_SMS_REPLY_PATH_FLAG, &reply_path_flag, return_value );
    if (MN_RETURN_SUCCESS != return_value)
    {
        reply_path_flag = g_mmisms_global.nv_param.reply_path_flag;
    }

    return reply_path_flag;
}

/*****************************************************************************/
//  Description : to Set the status reply path
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetReplyPathFlag(
                                    BOOLEAN    reply_path_flag
                                    )
{
    MMINV_WRITE(MMINV_SMS_REPLY_PATH_FLAG, &reply_path_flag);
    
    g_mmisms_global.nv_param.reply_path_flag = reply_path_flag;
}
#endif

#ifdef MMI_SMS_MULTI_PARAM_SUPPORT
/*****************************************************************************/
//  Description : to init the flag of sim mode
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitSIMModeFlag(void)
{
    uint8 sim_mode[MMI_DUAL_SYS_MAX] = {0};
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    int32 i = 0;
    
    MMINV_READ(MMINV_SMS_SIM_SETMODE, sim_mode, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        for(i = MN_DUAL_SYS_1;i<MMI_DUAL_SYS_MAX;i++)
        {
            sim_mode[i] = 0;        //默认为模式1
        }

        MMINV_WRITE(MMINV_SMS_SIM_SETMODE, sim_mode);
    }
   
    SCI_MEMCPY(g_mmisms_global.nv_param.sim_mode,sim_mode,sizeof(sim_mode));     
}

/*****************************************************************************/
//  Description : to get the sim mode
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC uint8 MMISMS_GetSIMModeFlag(MN_DUAL_SYS_E dual_sys)
{
    uint8 sim_mode[MMI_DUAL_SYS_MAX] = {0};
    MN_RETURN_RESULT_E return_val = MN_RETURN_FAILURE;
    int32 i = 0;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return 0;
    }

    MMINV_READ(MMINV_SMS_SIM_SETMODE, sim_mode, return_val);
    
    if(return_val != MN_RETURN_SUCCESS)
    {
        for(i = MN_DUAL_SYS_1;i<MMI_DUAL_SYS_MAX;i++)
        {
            sim_mode[i] = 0;        //默认为模式1
        }

        MMINV_WRITE(MMINV_SMS_SIM_SETMODE, sim_mode);
    }

    return sim_mode[dual_sys];
}

/*****************************************************************************/
//  Description : to set the sim mode
//  Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetSIMModeFlag(MN_DUAL_SYS_E dual_sys, uint8 mode)
{
    if (dual_sys < MMI_DUAL_SYS_MAX)
    {
        g_mmisms_global.nv_param.sim_mode[dual_sys] = mode;

        MNSMS_SetSmsParamDefaultIdEx(dual_sys, mode + 1);

        MMINV_WRITE(MMINV_SMS_SIM_SETMODE, g_mmisms_global.nv_param.sim_mode);
    }
}
#endif

/*****************************************************************************/
//     Description : to get the send path
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
PUBLIC MN_SMS_PATH_E MMISMS_GetSendPath(void)
{
    MN_SMS_PATH_E send_path = MN_SMS_GSM_PATH;

    //SCI_TRACE_LOW:"MMISMS_GetSendPath preferred_connection=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1309_112_2_18_2_53_48_522,(uint8*)"d", g_mmisms_global.nv_param.preferred_connection);

    switch(g_mmisms_global.nv_param.preferred_connection)
    {
    case MMISMS_PREFERRED_PATH_GSM_PREFERRED:
    case MMISMS_PREFERRED_PATH_GSM_ONLY:
        send_path = MN_SMS_GSM_PATH;
        break;

    case MMISMS_PREFERRED_PATH_GPRS_PREFERRED:
    case MMISMS_PREFERRED_PATH_GPRS_ONLY:
        send_path = MN_SMS_GPRS_PATH;
        break;

    default:
        send_path = MN_SMS_GSM_PATH;
        break;
    }

    return send_path;
}

/*****************************************************************************/
//     Description : to get the preferred connection
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_PREFERRED_PATH_E MMISMS_GetPreferredConnection(void)
{
    MMISMS_PREFERRED_PATH_E preferred_connection = MMISMS_PREFERRED_PATH_GSM_PREFERRED;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;
    
    MMINV_READ( MMINV_SMS_PREFERRED_CONNECTION, &preferred_connection, return_value );
    if (MN_RETURN_SUCCESS != return_value)
    {
        preferred_connection = g_mmisms_global.nv_param.preferred_connection;
    }

    return preferred_connection;    
}

/*****************************************************************************/
//     Description : to set the preferred connection
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetPreferredConnection(MMISMS_PREFERRED_PATH_E preferred_connection)
{
    MMINV_WRITE(MMINV_SMS_PREFERRED_CONNECTION, &preferred_connection);

    g_mmisms_global.nv_param.preferred_connection = preferred_connection;
}

/*****************************************************************************/
//     Description : to get the save position
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_SMS_SAVEPOS_TYPE_E MMISMS_GetSavePos(void)
{
    MMISMS_SMS_SAVEPOS_TYPE_E save_pos = MMISMS_SAVEPOS_PHONE;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;
    BOOLEAN is_sim_first = FALSE;
    MMISMS_SMS_SAVEPOS_TYPE_E mmi_save_pos = MMISMS_SAVEPOS_PHONE; 

    MNSMS_GetStoragePriorityEx(MN_DUAL_SYS_1,&is_sim_first);
    
    //SCI_TRACE_LOW:"MMISMS_GetSavePos is_sim_first is %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1373_112_2_18_2_53_48_523,(uint8*)"d",is_sim_first);

    if(is_sim_first)
    {
        save_pos = MMISMS_SAVEPOS_SIM;
    }
    else
    {
        save_pos = MMISMS_SAVEPOS_PHONE;
    }

    MMINV_READ( MMINV_SMS_SAVE_POS, &mmi_save_pos, return_value );    
    //SCI_TRACE_LOW:"MMISMS_GetSavePos mmi_save_pos is %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1385_112_2_18_2_53_48_524,(uint8*)"d",mmi_save_pos);
    
    if (MN_RETURN_SUCCESS != return_value)
    {
        mmi_save_pos = save_pos;
    }

    if(mmi_save_pos != save_pos)
    {
        MMINV_WRITE(MMINV_SMS_SAVE_POS, &save_pos);
    }
    
    //SCI_TRACE_LOW:"MMISMS_GetSavePos save_pos is %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1397_112_2_18_2_53_48_525,(uint8*)"d",save_pos);
    return save_pos;
}

/*****************************************************************************/
//     Description : to set the save position
//    Global resource dependence : g_mmisms_global
//  Author: liqing.peng
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetSavePos(
                              MMISMS_SMS_SAVEPOS_TYPE_E    pos
                              )
{
    uint32 i = 0;
    BOOLEAN is_sim_first = FALSE;

    MMINV_WRITE(MMINV_SMS_SAVE_POS, &pos);
    if(MMISMS_SAVEPOS_SIM == pos)
    {
        is_sim_first = TRUE;;
    }
    else
    {
        is_sim_first = FALSE;
    }

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        MNSMS_SetStoragePriorityEx(i, is_sim_first);
    }
}

/*****************************************************************************/
//     Description : to handle ps signal of APP_MN_SET_SMS_RECEIVE_FUNC_CNF
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void HandleSetSMSReceiveCnf(
                                  DPARAM    param
                                  )
{
    MnSetSmsReceiveFuncCnf *sig_ptr = (MnSetSmsReceiveFuncCnf *)param;
    LOCAL int32 s_resend_limit = 0;
    LOCAL uint32 s_last_send_time = 0;
    
    if (PNULL == sig_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:HandleSetSMSReceiveCnf sig_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1442_112_2_18_2_53_49_526,(uint8*)"");
        return;
    }
    //SCI_TRACE_LOW:"MMISMS: HandleSetSMSReceiveCnf cause = %d, s_resend_limit=%d, s_last_send_time=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1445_112_2_18_2_53_49_527,(uint8*)"ddd", sig_ptr->cause, s_resend_limit, s_last_send_time);
    if (MN_SMS_OPERATE_SUCCESS != sig_ptr->cause)
    {
        if (s_resend_limit <= MMISMS_SEND_RECEIVE_FUNC_REQ_TIMES)
        {
            //add inteval between two MNSMS_SetSmsReceiveFunc->APP_MN_SET_SMS_RECEIVE_FUNC_CNF(error)->MNSMS_SetSmsReceiveFunc
            SCI_Sleep(500);    
            MNSMS_SetSmsReceiveFuncEx(sig_ptr->dual_sys, TRUE);
            s_resend_limit++;
            s_last_send_time = SCI_GetTickCount();
        }
        else
        {
            s_resend_limit = 0;
        }
    }
    else
    {
        s_resend_limit = 0;
    }
}

/*****************************************************************************/
//     Description : to check the SMS is ready
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsReady(void)
{
    BOOLEAN is_sim_sms_ready = TRUE;
    uint32 i = MN_DUAL_SYS_1;

    for(i=MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
    {
        if(!MMISMS_IsSMSReady(i))
        {
            is_sim_sms_ready = FALSE;
            break;
        }
    }
    
    return is_sim_sms_ready;
}

/*****************************************************************************/
//     Description : to check the SMS is ready
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsSMSReady(MN_DUAL_SYS_E dual_sys)
{
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS: MMISMS_IsSMSReady dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1496_112_2_18_2_53_49_528,(uint8*)"d", dual_sys);
        return FALSE;
    }
    //SCI_TRACE_LOW:" MMISMS: MMISMS_IsSMSReady[%d]=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1499_112_2_18_2_53_49_529,(uint8*)"dd", dual_sys, g_mmisms_global.is_sim_ready[dual_sys]);
    
    return g_mmisms_global.is_sim_ready[dual_sys];
}

/*****************************************************************************/
//     Description : to ready read SMS
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void    MMISMS_ReadyReadSms( void)
{
    SCI_MEMSET(&g_mmisms_global.read_msg, 0, sizeof(MMISMS_READ_MSG_T) );
}

/*****************************************************************************/
//     Description : to get the type of MT message 
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_READ_TYPE_E MMISMS_GetMtType(void)
{
    return (g_mmisms_global.read_msg.read_type);
}

/*****************************************************************************/
//     Description : to get the reference number form NV
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void GetRefNumFormNV(void)
{
    MMINV_SMS_REFERENCE_NUM_T    ref_num = 0;
    MN_RETURN_RESULT_E            nv_value= MN_RETURN_FAILURE;
    
    MMINV_READ(MMINV_SMS_REFERENCE_NUM, &ref_num, nv_value);
    
    if (MN_RETURN_SUCCESS != nv_value)
    {
        ref_num = MMISMS_NV_DEFAULT_REFNUM;
        MMISMS_SetRefNumToNV(ref_num);
    }
    
    //SCI_TRACE_LOW:"MMISMS: GetRefNumFormNV ref_num = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1539_112_2_18_2_53_49_530,(uint8*)"d", ref_num);
    g_mmisms_global.nv_param.ref_num = ref_num;
}

/*****************************************************************************/
//     Description : to check the oper of update is ok
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsUpdateOk( void )
{
    BOOLEAN is_ok = FALSE;
    
    is_ok = ( 0 == g_mmisms_global.update_status_num ) ? TRUE : FALSE;
    
    return (is_ok);
}

/*****************************************************************************/
// Description : to get the origin number form unit info
// Global resource dependence : s_mmisms_linked_order
// Author:liming.deng
// Note:
/*****************************************************************************/
PUBLIC uint8 MMISMS_GetOriginNumBySearchResult(
                                      MN_DUAL_SYS_E     *dual_sys_ptr,
                                      uint8             *num_ptr,        //OUT:
                                      uint8             max_num_len    //IN:                                          
                                      )
{
    uint8 len = 0;
    uint8 i = 0;
    uint16 cur_index = 0;
    // MMISMS_ORDER_ID_T cur_order_id = PNULL;
    MMISMS_LINKED_ORDER_UNIT_T *cur_unit_id = PNULL;
    
    cur_index = GUILIST_GetCurItemIndex(MMISMS_SEARCH_LISTBOX_CTRL_ID);
    cur_unit_id = MMISMS_GetUnitIDByIndex(cur_index);
    
    if(PNULL == cur_unit_id)
    {
        return 0;
    }

    if (cur_unit_id->sms_state.flag.msg_type == MMISMS_TYPE_SMS)
    {
        len = MMIAPICOM_GenDispNumber(
            MMIPB_GetNumberTypeFromUint8(cur_unit_id->sms_state.number.bcd_number.npi_ton),
            MIN(MN_MAX_ADDR_BCD_LEN, cur_unit_id->sms_state.number.bcd_number.number_len),
            cur_unit_id->sms_state.number.bcd_number.number,
            num_ptr,
            max_num_len
            );        
    }
    else
    {
        len = strlen((char *)cur_unit_id->sms_state.number.sender);
        //changed by minghu cr128630
        for (i = 0; i < len; i++)
        {
            if (MMIAPICOM_IsTelephoneNum(*(cur_unit_id->sms_state.number.sender+i)))
            {
                *(num_ptr + i) = *(cur_unit_id->sms_state.number.sender + i);
            }
            else
            {
                len = i;
                break;
            }
        }
        *(num_ptr + len) = 0;
    }
    *dual_sys_ptr = (MN_DUAL_SYS_E)cur_unit_id->sms_state.flag.dual_sys;

    return (len);
}

/*****************************************************************************/
// Description : to get the subject form unit info
// Global resource dependence : 
// Author:plum.peng
// Note:
/*****************************************************************************/
PUBLIC MMISMS_SMSDISPLAY_CONTENT_T* MMISMS_GetSubjectBySearchResult(void)
{
    // uint8 len = 0;
    // uint8 i = 0;
    uint16 cur_index = 0;
    // MMISMS_ORDER_ID_T cur_order_id = PNULL;
    MMISMS_LINKED_ORDER_UNIT_T *cur_unit_id = PNULL;
    
    cur_index = GUILIST_GetCurItemIndex(MMISMS_SEARCH_LISTBOX_CTRL_ID);
    cur_unit_id = MMISMS_GetUnitIDByIndex(cur_index);
    if(PNULL == cur_unit_id)
    {
        return PNULL;
    }

    return &(cur_unit_id->sms_state.display_content);
}

/*****************************************************************************/
//     Description : to get the origin number form MT SMS
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC uint8 MMISMS_GetOriginNum(                        //RETURN: the length of num_ptr
                                 MN_DUAL_SYS_E *dual_sys_ptr,
                                 uint8            *num_ptr,        //OUT:
                                 uint8            max_num_len        //IN:
                                 )
{
    uint8 len = 0;
    MN_CALLED_NUMBER_T *address_ptr = &(g_mmisms_global.read_msg.address);
    
    if (PNULL == dual_sys_ptr || PNULL == num_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_GetOriginNum dual_sys_ptr num_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1650_112_2_18_2_53_49_531,(uint8*)"");
        return 0;
    }
    
    len = MMIAPICOM_GenDispNumber(
        address_ptr->number_type,
        MIN(MN_MAX_ADDR_BCD_LEN, address_ptr->num_len),
        address_ptr->party_num,
        num_ptr,
        max_num_len
        );
    *dual_sys_ptr = g_mmisms_global.read_msg.dual_sys;

    return (len);
}

/*****************************************************************************/
//     Description : to get the origin number form order info
//    Global resource dependence : s_mmisms_order
//  Author:liming.deng
//    Note:
/*****************************************************************************/
PUBLIC uint8 MMISMS_GetOriginNumByPos(                        //RETURN: the length of num_ptr
                                      MN_DUAL_SYS_E     *dual_sys_ptr,
                                      uint8                *num_ptr,        //OUT:
                                      uint8                 max_num_len    //IN:                                          
                                      )
{
    uint8 len = 0;
    uint8 i = 0;
    MMISMS_ORDER_ID_T cur_order_id = PNULL;
    
    g_mmisms_global.operation.cur_order_index = 0;
    cur_order_id = MMISMS_GetCurOperationOrderId();
    
    if (PNULL == dual_sys_ptr || PNULL == num_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_GetOriginNumByPos dual_sys_ptr num_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1685_112_2_18_2_53_49_532,(uint8*)"");
        return 0;
    }
    
    if (cur_order_id->flag.msg_type == MMISMS_TYPE_SMS)
    {
        if(MN_NUM_TYPE_ALPHANUMERIC == MMIPB_GetNumberTypeFromUint8(cur_order_id->number.bcd_number.npi_ton))
        {
            len = 0;
        }
        else
        {
            len = MMIAPICOM_GenDispNumber(
                MMIPB_GetNumberTypeFromUint8(cur_order_id->number.bcd_number.npi_ton),
                MIN(MN_MAX_ADDR_BCD_LEN, cur_order_id->number.bcd_number.number_len),
                cur_order_id->number.bcd_number.number,
                num_ptr,
                max_num_len
                );    
        }
    }
    else
    {
        len = strlen((char *)cur_order_id->number.sender);
        //changed by minghu cr128630
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
    *dual_sys_ptr = (MN_DUAL_SYS_E)cur_order_id->flag.dual_sys;

    return (len);
}

/*****************************************************************************/
//     Description : to set the global operation status
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_SetOperStatus(
                                    MMISMS_OPER_STATUS_E    oper_status
                                    )
{
    //SCI_TRACE_LOW:"MMISMS_SetOperStatus oper_status is %d, g_mmisms_global.operation.oper_status = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1738_112_2_18_2_53_49_533,(uint8*)"dd",oper_status, g_mmisms_global.operation.oper_status);

    g_mmisms_global.operation.oper_status = oper_status;

    return TRUE;
}

/*****************************************************************************/
//  Description : to set the global operation status
//  Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//  Note: 
/*****************************************************************************/
PUBLIC void    MMISMS_SetMoveOperStatus(
                                        MMISMS_OPER_STATUS_E    oper_status
                                        )
{
    //SCI_TRACE_LOW:"MMISMS_SetMoveOperStatus oper_status is %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1753_112_2_18_2_53_49_534,(uint8*)"d",oper_status);
    g_mmisms_global.move_oper_status = oper_status;
}

/*****************************************************************************/
//     Description : to set the global CurOrderId
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: 
/*****************************************************************************/
PUBLIC void    MMISMS_SetCurOrderId(
                                    int index,
                                    MMISMS_ORDER_ID_T order_id
                                    )
{
    if (index >= MMISMS_SPLIT_MAX_NUM)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurOrderId index >= MMISMS_SPLIT_MAX_NUM"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1768_112_2_18_2_53_49_535,(uint8*)"");
        return;
    }
    g_mmisms_global.operation.cur_order_id_arr[index] = order_id;    
}

/*****************************************************************************/
//     Description : to set the g_mmisms_global.operation.cur_order_index
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: 
/*****************************************************************************/
PUBLIC void    MMISMS_SetCurOrderArrIndex(
                                          int index                        
                                          )
{
    if (index >= MMISMS_SPLIT_MAX_NUM)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurOrderId index >= MMISMS_SPLIT_MAX_NUM"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1784_112_2_18_2_53_49_536,(uint8*)"");
        return;
    }
    g_mmisms_global.operation.cur_order_index = index;    
}


/*****************************************************************************/
//     Description : to set the global CurOrderId
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: 
/*****************************************************************************/
PUBLIC void    MMISMS_SetCurType(
                                 MMISMS_BOX_TYPE_E box_type
                                 )
{
    g_mmisms_global.operation.cur_type = box_type;
}

/*****************************************************************************/
//     Description : to get the global operation type
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note: 
/*****************************************************************************/
PUBLIC PUBLIC MMISMS_BOX_TYPE_E  MMISMS_GetCurOperationType(void)
{
    //SCI_TRACE_LOW:"MMISMS_GetCurOperationType cur_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1808_112_2_18_2_53_49_537,(uint8*)"d",g_mmisms_global.operation.cur_type);
    return g_mmisms_global.operation.cur_type;
}

/*****************************************************************************/
//     Description : to handle mem-full ind
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void HandleMemFullInd(
                            DPARAM param    //IN:
                            )
{
    MnMemFullSmsInd *sig_ptr = (MnMemFullSmsInd *)param;
    uint16 unread_count = 0;
    
    if (PNULL == sig_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:HandleMemFullInd sig_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1825_112_2_18_2_53_49_538,(uint8*)"");
        return;
    }        

    unread_count = MMISMS_GetTobeReadSmsCount(sig_ptr->dual_sys);

    //SCI_TRACE_LOW:"SMS: HandleMemFullInd mem_status = %d, unread_count = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1832_112_2_18_2_53_49_539,(uint8*)"dd", sig_ptr->mem_status, unread_count);
    
    // change the icon and etc in idle screen.
    MAIN_SetMsgState(TRUE);
        
#ifdef PDA_UI_DROPDOWN_WIN
    if (MMIAPISMS_IsSMSFull())
    {
        MMIDROPDOWNWIN_AddNotifyRecord(DROPDOWN_NOTIFY_MESSAGE_FULL);
    }    
#endif
    
    MMISMS_ShowMemFullPrompt(sig_ptr->dual_sys,sig_ptr->mem_status);

}

/*****************************************************************************/
//     Description : to display the err code of save
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_DisplayPrompByErr(
                                     MMISMS_OPER_ERR_E        err_code,    //IN:
                                     MMI_TEXT_ID_T            label_id    //IN:
                                     )
{
    //SCI_TRACE_LOW:"MMISMS: MMISMS_DisplayPrompByErr err_code = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1857_112_2_18_2_53_49_540,(uint8*)"d", err_code);
    
    switch (err_code)
    {
    case MMISMS_NO_ERR:
        //open the saving message window anima
        {
            MMI_STRING_T    prompt_str = {0};
            
            MMI_GetLabelTextByLang(label_id, &prompt_str);
            MMIPUB_OpenWaitWin(1,&prompt_str,PNULL,PNULL,MMISMS_SAVEMSG_ANIM_WIN_ID,IMAGE_NULL,
                ANIM_PUBWIN_WAIT,WIN_ONE_LEVEL,MMIPUB_SOFTKEY_NONE,MMISMS_HandleWaitWinMsg);
            // close the option window
            MMK_CloseWin(MMISMS_EDITOPTION_WIN_ID);
        }
        break;

    case MMISMS_NO_ENOUGH_SPACE:
        //打开提示窗口：存储空间不足
        MMISMS_OpenMsgBox(
            TXT_NO_SPACE, 
            IMAGE_PUBWIN_FAIL
            );
        break;
        
    default:
        // open save fail
        MMISMS_OpenMsgBox(
            TXT_ERROR, 
            IMAGE_PUBWIN_FAIL
            );
        MMK_CloseWin(MMISMS_SHOWMSG_WIN_ID);
        break;
    }            
}

/*****************************************************************************/
//     Description : to display the update prompt
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC  void MMISMS_DisplayUpdatePrompt( 
                                        BOOLEAN    is_succ        //IN:
                                        )
{	
    //SCI_TRACE_LOW:"MMISMS: MMISMS_DisplayUpdatePrompt(%d)"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1904_112_2_18_2_53_50_541,(uint8*)"d", is_succ);

    switch (MMISMS_GetCurrentOperStatus()) 
    {
    case MMISMS_SAVE_SENDSUCC_SMS:
        MMISMS_CloseEditWin();
        break;
        
    case MMISMS_UPDATE_SENDFAIL_SMS:
        break;

    case MMISMS_UPDATE_NOSEND_SMS:
        if (MMK_IsOpenWin(MMISMS_SAVEMSG_ANIM_WIN_ID))
        {
            MMISMS_CloseEditWin();
        }
        break;
        
    case MMISMS_SAVE_SENDFAIL_SMS:
        MMISMS_OpenMsgBox( 
            TXT_ERROR, 
            IMAGE_PUBWIN_FAIL  
            );
        MMK_CloseWin(MMISMS_SHOWMSG_WIN_ID);
        break;
        
    default:
        if (is_succ)
        {
            MMK_CloseWin(MMISMS_SHOWMSG_WIN_ID);
            MMISMS_CloseEditWin();
        }
        else
        {
            MMISMS_OpenMsgBox(
                TXT_ERROR, 
                IMAGE_PUBWIN_FAIL  
                );
        }
        break;
    }
    
    MMISMS_ClearOperInfo();
    MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);    
}

/*****************************************************************************/
//     Description : to get the current oper status 
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_OPER_STATUS_E MMISMS_GetCurrentOperStatus( void )
{
    //SCI_TRACE_LOW:"MMISMS_GetCurrentOperStatus: oper_status = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1987_112_2_18_2_53_50_542,(uint8*)"d",g_mmisms_global.operation.oper_status);
    return (g_mmisms_global.operation.oper_status);
}

/*****************************************************************************/
//  Description : to get the current oper status 
//  Global resource dependence : 
//  Author:fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC MMISMS_OPER_STATUS_E MMISMS_GetCurrentMoveOperStatus( void )
{
    //SCI_TRACE_LOW:"MMISMS_GetCurrentMoveOperStatus: oper_status = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_1997_112_2_18_2_53_50_543,(uint8*)"d",g_mmisms_global.move_oper_status);
    return (g_mmisms_global.move_oper_status);
}

/*****************************************************************************/
//     Description : 得到当前的排序方式
//    Global resource dependence : 
//  Author:liming.deng
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_ORDERTYPE_E MMISMS_GetOrderType(void)
{
    return g_mmisms_global.cur_order_type;        
}

/*****************************************************************************/
//     Description : 是否需要重新排序
//    Global resource dependence : 
//  Author:liming.deng
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsNeedSort(void)
{
    return g_mmisms_global.is_need_sort;   
}

/*****************************************************************************/
//     Description : 设置需要重新排序
//    Global resource dependence : 
//  Author:liming.deng
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetNeedSort(BOOLEAN is_sort)
{
    g_mmisms_global.is_need_sort = is_sort;  
}

/*****************************************************************************/
//     Description : 设置当前的排序方式
//    Global resource dependence : 
//  Author:liming.deng
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetOrderType (MMISMS_ORDERTYPE_E  order_type)
{
    if (order_type != MMISMS_GetOrderType()) //仅当排序类型发生改变时，重新排序list
    {
        g_mmisms_global.cur_order_type = order_type;
        MMINV_WRITE(MMINV_SMS_ORDER_TYPE, &order_type);
        
        MMISMS_SetNeedSort(TRUE);
        MMISMS_HandleOrderChangeCallBack();
    }    
}

#if defined(MMI_SMS_FONT_SETTING_SUPPORT)
/*****************************************************************************/
//  Description : 得到当前的字体大小
//  Global resource dependence : 
//  Author:fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC MMISMS_FONT_E MMISMS_GetFontSize(void)
{
    return g_mmisms_global.font_size;        
}

/*****************************************************************************/
//  Description : 设置当前的字体大小
//  Global resource dependence : 
//  Author:fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetFontSize (MMISMS_FONT_E  font_size)
{
    if (font_size != MMISMS_GetFontSize()) //仅当排序类型发生改变时，重新排序list
    {
        g_mmisms_global.font_size = font_size;
        MMINV_WRITE(MMINV_SMS_FONT_SIZE, &font_size);
    }
    MMISMS_ResetWinFont(MMISMS_GetCurBoxType());
}
#endif

/*****************************************************************************/
//     Description : to clear the global variable
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_ClearOperInfo(void)
{
    //SCI_TRACE_LOW:"MMISMS_ClearOperInfo"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2075_112_2_18_2_53_50_544,(uint8*)"");
    g_mmisms_global.operation.oper_status = MMISMS_NO_OPER ;
}

/*****************************************************************************/
//     Description : to clear the global variable
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_ClearSaveInfo(void)
{
    SCI_MEMSET(&g_mmisms_global.save_info, 0, sizeof(MMISMS_SAVE_INFO_T));
}

/*****************************************************************************/
//     Description : Compute Current Order Id
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  
/*****************************************************************************/
PUBLIC MMISMS_ORDER_ID_T MMISMS_GetCurOperationOrderId(void)
{
    return g_mmisms_global.operation.cur_order_id_arr[g_mmisms_global.operation.cur_order_index ];
}

/*****************************************************************************/
//     Description : set Current Order Id
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetCurOperationOrderId(MMISMS_BOX_TYPE_E box_type, uint32 index)
{
    MMISMS_ORDER_ID_T order_id = PNULL;
    //MMISMS_ORDER_ID_T cur_order_id = PNULL;
    //int32 i = 0;
    
    //根据pos_index获得当前的order_id 
#ifdef MRAPP_SUPPORT
   	if(MMIMRAPP_IsReadBySMS()) /*lint !e718 !e18*/
   	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(box_type, (int)index);
	}
	else
	{
    	order_id = MMISMS_GetLinkedArrayPtrIdInList(box_type, index); 
	}
#else
    order_id = MMISMS_GetLinkedArrayPtrIdInList(box_type, index);
#endif
    if (PNULL == order_id)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurOperationOrderId order_id = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2111_112_2_18_2_53_50_545,(uint8*)"");
        return;
    }    
    MMISMS_SetSMSUpdateInfo(order_id);
    
    MMISMS_SetGlobalOperationOrderId(order_id);
}

#ifdef MMI_TIMERMSG_SUPPORT
/*****************************************************************************/
//     Description : set Current Order Id
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetCurOperationOrderIdByTimerMsg(MMINV_SMS_TIMERMSG_T *timer_msg_ptr)
{
    MMISMS_ORDER_ID_T order_id = PNULL;
    //MMISMS_ORDER_ID_T cur_order_id = PNULL;
    //int32 i = 0;
    
    //根据pos_index获得当前的order_id 
    order_id = MMISMS_GetLinkedArrayPtrIdByTimerMsg(timer_msg_ptr); 

    if (PNULL == order_id)
    {
        return;
    }
    
    MMISMS_SetSMSUpdateInfo(order_id);
    
    MMISMS_SetGlobalOperationOrderId(order_id);  
}
#endif

#if 0 /* Reduce Code size */
/*****************************************************************************/
//     Description : set the first to be read sms as Current Order Id
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetFirstUnreadSmsToGlobal(void)
{
    MMISMS_ORDER_ID_T order_id = PNULL;
    //MMISMS_ORDER_ID_T cur_order_id = PNULL;
    //int32 i = 0;
    
    //根据pos_index获得当前的order_id 
    order_id = MMISMS_GetFirstUnreadSmsId();
    
    if (PNULL == order_id)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurOperationOrderId order_id = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2160_112_2_18_2_53_50_546,(uint8*)"");
        return;
    }
    
    MMISMS_SetSMSUpdateInfo(order_id);
    
    MMISMS_SetGlobalOperationOrderId(order_id);   
}
#endif /* Reduce Code size */

/*****************************************************************************/
//     Description : 初始化全局变量g_mmisms_global。
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  将g_mmisms_global清0。
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_InitGlobalInfo(void)
{
    SCI_MEMSET(((void *)&g_mmisms_global), 0, sizeof(g_mmisms_global)); 

    return TRUE;
}

/*****************************************************************************/
//     Description : 根据storage，设置g_mmisms_global. is_sim_sms_ready或者is_nv_sms_ready。
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_SetSmsOrderReady (
                                                                                        MN_DUAL_SYS_E dual_sys,
                                                                                        MN_SMS_STORAGE_E storage,
                                                                                        BOOLEAN is_order_ready
                                                                                        )
{
    //根据storage，设置g_mmisms_global. is_sim_sms_ready或者is_nv_order_ok,表示sim 或者nv 中排序ready.
    if (MN_SMS_STORAGE_SIM == storage)
    {
        g_mmisms_global.is_sim_order_ok[dual_sys] = is_order_ready;
    }
    else
    {
        g_mmisms_global.is_nv_order_ok = is_order_ready;
    }
    return TRUE;
}

/*****************************************************************************/
//     Description : get the SMS num of the read message
//    Global resource dependence :  
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/    
PUBLIC uint8 MMISMS_GetReadSMSNum(void)
{
    return (g_mmisms_global.read_msg.sms_num);
}

#if 0 /* Reduce Code size */
/*****************************************************************************/
//     Description : set the SMS num of the read message
//    Global resource dependence :  
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/    
PUBLIC void MMISMS_SetReadSMSNum(
                                 uint8 sms_num
                                 )
{
    g_mmisms_global.read_msg.sms_num = sms_num;
}
#endif /* Reduce Code size */

/*****************************************************************************/
//     Description : set the SMS num of the read message
//    Global resource dependence :  
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/    
PUBLIC MMISMS_BOX_TYPE_E MMISMS_GetCurBoxType(void)
{
    return g_mmisms_global.operation.cur_type;
}

/*****************************************************************************/
//     Description : to check the order is ok
//    Global resource dependence : g_mmisms_order
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN    MMISMS_IsOrderOk( void )
{
    BOOLEAN ret_value = TRUE;
    uint32 i = MN_DUAL_SYS_1;
    uint32 order_status = 0;

    for(i=MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
    {
        if (g_mmisms_global.is_sim_order_ok[i])
        {
            order_status |= (0x01 << i);
        }
    }

    if (g_mmisms_global.is_nv_order_ok)
    {
        order_status |= (0x01 << MMI_DUAL_SYS_MAX);
    }

    //SCI_TRACE_LOW:"MMISMS_IsOrderOk order_status = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2257_112_2_18_2_53_50_547,(uint8*)"d", order_status);
    
    if(!(g_mmisms_global.is_nv_order_ok))
    {
        return FALSE;
    }

    for(i=MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
    {
         if(!(g_mmisms_global.is_sim_order_ok[i]))
         {
             ret_value = FALSE;
             break;
         }
    }

    return ret_value;
    
}

/*****************************************************************************/
//     Description : set g_mmisms_global.is_mms_ready 
//    Global resource dependence : 
//  Author:liming.deng  
//    Note: 当MMS开机排序完成时设置全局变量g_mmisms_global.is_mms_ready。
/*****************************************************************************/
PUBLIC void MMISMS_SetMMSOrderReady(BOOLEAN is_mms_ready)
{
    g_mmisms_global.is_mms_ready = is_mms_ready;
}

/*****************************************************************************/
//     Description : get g_mmisms_global.is_mms_ready 
//    Global resource dependence : 
//  Author:liming.deng  
//    Note: 得到全局变量g_mmisms_global.is_mms_ready的值
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsMMSOrderReady(void)
{
#ifndef MMS_SUPPORT 
	return TRUE;
#else
    return g_mmisms_global.is_mms_ready;
#endif
}
/*****************************************************************************/
//     Description : 开启APP发起的SMS初始化
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
LOCAL void StartAppInitSmsList(MN_DUAL_SYS_E dual_sys, MNSIM_ERROR_E sim_error_reason)
{
    BOOLEAN is_sim_order_ok = TRUE;
    uint32 i = MN_DUAL_SYS_1;
    
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:StartAppInitSmsList dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2309_112_2_18_2_53_50_548,(uint8*)"");
        return ;
    }    
    MMISMS_SetIsSMSRefresh(dual_sys, FALSE);

    if (!g_mmisms_global.is_reinit[dual_sys])//Refresh初始化的时候，只有等MMI排序完才赋值
    {
        g_mmisms_global.is_sim_order_ok[dual_sys] = TRUE;
    }    
    else
    {
        if (MNSIM_ERROR_REJECT_BY_NET == sim_error_reason
            || MNSIM_ERROR_IMSI_INVALID == sim_error_reason
            || MNSIM_ERROR_NULL == sim_error_reason)
        {
            MMISMS_InitSmsOrder();

            MMISMS_InitSmsList();
        }

        return;
    }

    for(i=MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
    {
        if(!(g_mmisms_global.is_sim_order_ok[i]))
        {
            is_sim_order_ok = FALSE;
            break;
        }
    }
    
    if (is_sim_order_ok && !g_mmisms_global.is_nv_order_ok)//排序NV
    {
        MMISMS_InitSmsOrder();
        MMISMS_InitSmsList();
    }
    else if (MMISMS_IsOrderOk())
    {
        MMISMS_InitOtherMessageList();
    }
}

/*****************************************************************************/
//     Description : handle APP_MN_SMS_READY_IND message
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN MMISMS_HandleSmsReadyInd(DPARAM param)
{
    MN_DUAL_SYS_E dual_sys   = MN_DUAL_SYS_MAX;
    MnSmsReadyIndS *sim_ready_ind = (MnSmsReadyIndS *)param;
    
    dual_sys = sim_ready_ind->dual_sys;
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurSaveDualSys dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2364_112_2_18_2_53_50_549,(uint8*)"");
        return FALSE;
    }

#ifdef MMI_REPLACE_SMS_SUPPORT
    MNSMS_WriteReplaceSmsSupportedEx(dual_sys, TRUE);
#endif

#ifdef MMI_SMS_MULTI_PARAM_SUPPORT
    MNSMS_SetSmsParamDefaultIdEx(dual_sys, g_mmisms_global.nv_param.sim_mode[dual_sys] + 1);
#endif

    //SCI_TRACE_LOW:"APP_MN_SMS_READY_IND (dual_sys=%d)"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2376_112_2_18_2_53_51_550,(uint8*)"d", dual_sys);

    g_mmisms_global.is_sim_ready[dual_sys] = TRUE;

    StartAppInitSmsList(dual_sys, MNSIM_ERROR_NULL);
    
    return TRUE;    
}

/*****************************************************************************/
//     Description : set current operating voice mail dual sys
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_SetCurVoiceMailDualSys(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurVoiceMailDualSys dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2393_112_2_18_2_53_51_551,(uint8*)"");
        return FALSE;
    }
    g_mmisms_global.cur_voice_mail_dual_sys = dual_sys;
    
    return TRUE;
}

/*****************************************************************************/
//     Description : get current operating voice mail dual sys
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMISMS_GetCurVoiceMailDualSys(void)
{
    return g_mmisms_global.cur_voice_mail_dual_sys;
}

/*****************************************************************************/
//     Description : set current operating voice mail dual sys
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_SetCurSendDualSys(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurSendDualSys dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2418_112_2_18_2_53_51_552,(uint8*)"");
        return FALSE;
    }
    g_mmisms_global.cur_send_dual_sys = dual_sys;
    g_mmisms_global.edit_content.dual_sys = dual_sys;
    return TRUE;
}

/*****************************************************************************/
//     Description : get current operating voice mail dual sys
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMISMS_GetCurSendDualSys(void)
{
    return g_mmisms_global.cur_send_dual_sys;
}

/*****************************************************************************/
//     Description : get current read msg dual sys
//    Global resource dependence : 
//  Author: liming.deng
//    Note:
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMISMS_GetCurReadMsgDualSys(void)

{
    return g_mmisms_global.read_msg.dual_sys;
}


/*****************************************************************************/
//     Description : set current operating voice mail dual sys
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_SetCurSaveDualSys(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurSaveDualSys dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2454_112_2_18_2_53_51_553,(uint8*)"");
        return FALSE;
    }
    g_mmisms_global.cur_save_dual_sys = dual_sys;
    return TRUE;
}

/*****************************************************************************/
//     Description : get current operating voice mail dual sys
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMISMS_GetCurSaveDualSys(void)
{
    return g_mmisms_global.cur_save_dual_sys;
}

/*****************************************************************************/
//  Description : init sort status according setting
//  Global resource dependence : none
//  Author: bruce.chi
//  Note: 
/*****************************************************************************/
PUBLIC void MMISMS_InitSortStatusAccordingToSetting(void)
{
    uint32 i = MN_DUAL_SYS_1;

    for(i = MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
    {
        g_mmisms_global.is_sim_order_ok[i] = FALSE;
    }
    
    g_mmisms_global.is_nv_order_ok = FALSE;
    
}

/*****************************************************************************/
//     Description : handle APP_MN_SIM_NOT_READY_IND message
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN MMISMS_HandleSimNotReadyInd(DPARAM param)
{
    MnSimNotReadyIndS sim_not_ready_ind = *(MnSimNotReadyIndS *)param;
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    
    dual_sys = sim_not_ready_ind.dual_sys;
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_HandleSimNotReadyInd dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2499_112_2_18_2_53_51_554,(uint8*)"");
        return FALSE;
    }
    //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleSimNotReadyInd sim_not_ready_ind reason = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2502_112_2_18_2_53_51_555,(uint8*)"d", sim_not_ready_ind.reason);

    g_mmisms_global.is_sim_ready[dual_sys] = FALSE;

    switch(sim_not_ready_ind.reason)
    {
    case MNSIM_ERROR_REJECT_BY_NET: //需要排序 
    case MNSIM_ERROR_IMSI_INVALID:  //需要排序 
    case MNSIM_ERROR_NULL:          //需要排序 
#ifdef MMI_SIM_LOCK_SUPPORT //sim lock 不能排序
    case MNSIM_ERROR_SIM_DATA_INTEGRITY_FAIL:
    case MNSIM_ERROR_SIM_NETWORT_LOCKED:
    case MNSIM_ERROR_SIM_NETWORT_SUBSET_LOCKED:
    case MNSIM_ERROR_SIM_SP_LOCKED:
    case MNSIM_ERROR_SIM_CORPORATE_LOCKED:
    case MNSIM_ERROR_SIM_USER_LOCKED:
#endif    
        break;

    default:
        StartAppInitSmsList(dual_sys, sim_not_ready_ind.reason);
        break;
    }    

    return TRUE;
}

/*****************************************************************************/
//  Description : MMISMS_RefreshSMS
//  Global resource dependence : 
//  Author: wancan.you
//RETURN: 
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_RefreshSMS(MN_DUAL_SYS_E dual_sys)
{
    //SCI_TRACE_LOW:"MMISMS_RefreshSMS: dual_sys=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2536_112_2_18_2_53_51_556,(uint8*)"d", dual_sys);
    
    if (MMIAPISMS_IsOrderOk())
    {
        uint32 i = MN_DUAL_SYS_1;
        
        MMISMS_InitSortStatusAccordingToSetting();
        MMISMS_FreeSmsOrder();

        MMISMS_SetMMSOrderReady(FALSE);

        MAIN_SetMsgState(TRUE);
        
        for(i = MN_DUAL_SYS_1; i<MMI_DUAL_SYS_MAX; i++)
        {
            g_mmisms_global.is_reinit[i] = TRUE;
        }

        return TRUE;
    }
    
    return FALSE;
}

/*****************************************************************************/
//     Description : set is need load saved sms
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetIsNeedLoadSaved(BOOLEAN is_need_load_saved)
{
    //SCI_TRACE_LOW:"MMISMS_SetIsNeedLoadSaved:is_need_load_saved=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2566_112_2_18_2_53_51_557,(uint8*)"d", is_need_load_saved);

    g_mmisms_global.is_need_load_saved = is_need_load_saved;
}

/*****************************************************************************/
//     Description : get is need load saved sms
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetIsNeedLoadSaved(void)
{
    //SCI_TRACE_LOW:"MMISMS_GetCurSaveDualSys:is_need_load_saved=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2577_112_2_18_2_53_51_558,(uint8*)"d", g_mmisms_global.is_need_load_saved);

    return g_mmisms_global.is_need_load_saved;
}

/*****************************************************************************/
//     Description : set is send fail sms
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetIsSendFail(BOOLEAN is_send_fail)
{
    //SCI_TRACE_LOW:"MMISMS_SetIsSendFail:is_send_fail=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2588_112_2_18_2_53_51_559,(uint8*)"d", is_send_fail);

    g_mmisms_global.is_send_fail = is_send_fail;
}

/*****************************************************************************/
//     Description : get is send fail sms
//    Global resource dependence : 
//  Author: bruce.chi
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetIsSendFail(void)
{
    //SCI_TRACE_LOW:"MMISMS_GetIsSendFail:is_send_fail=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2599_112_2_18_2_53_51_560,(uint8*)"d", g_mmisms_global.is_send_fail);

    return g_mmisms_global.is_send_fail;
}

/*****************************************************************************/
//     Description : convet sms content to display content
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_ConvertSmsContentToDisplayContent(
                                                     MMISMS_SMSDISPLAY_CONTENT_T *display_content_ptr,//out
                                                     uint8 *message_content_ptr,
                                                     uint16 content_len,
                                                     MN_SMS_ALPHABET_TYPE_E alphabet
                                                    )
{
    if (PNULL == display_content_ptr || PNULL == message_content_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurOperationOrderId message_content_ptr display_content_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2617_112_2_18_2_53_51_561,(uint8*)"");
        return;
    }
    if (MN_SMS_UCS2_ALPHABET == alphabet)
    {
        display_content_ptr->content_len = MIN(MMIMESSAGE_DIAPLAY_LEN, content_len/sizeof(wchar));

#if defined(__BIG_ENDIAN) || defined(__BigEndian)
        MMI_WSTRNCPY(display_content_ptr->content,
                     MMIMESSAGE_DIAPLAY_LEN,
                     (wchar *)message_content_ptr,
                     content_len/sizeof(wchar),
                     display_content_ptr->content_len);
#else
        GUI_UCS2L2UCS2B((uint8 *)display_content_ptr->content,
                        display_content_ptr->content_len * sizeof(wchar),
                        message_content_ptr,
                        display_content_ptr->content_len * sizeof(wchar));
#endif
    }
    else if (MN_SMS_8_BIT_ALPHBET == alphabet)
    {
        display_content_ptr->content_len = MIN(MMIMESSAGE_DIAPLAY_LEN, content_len);

        MMI_STRNTOWSTR(
            display_content_ptr->content,
            display_content_ptr->content_len,
            (const uint8 *)message_content_ptr,
            display_content_ptr->content_len,
            display_content_ptr->content_len
            );
    }
    else
    {
        display_content_ptr->content_len = MIN(MMIMESSAGE_DIAPLAY_LEN, content_len);

/*
        MMI_STRNTOWSTR(display_content_ptr->content,
                                MMIMESSAGE_DIAPLAY_LEN,
                                (const uint8 *)message_content_ptr,
                                content_len,
                                display_content_ptr->content_len);
*/
        display_content_ptr->content_len = MMIAPICOM_Default2Wchar(
                                                                    message_content_ptr,
                                                                    display_content_ptr->content,
                                                                    display_content_ptr->content_len
                                                                    );
    }
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC uint8 MMISMS_GetMaxSimSmsNum(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_GetMaxSimSmsNum dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2675_112_2_18_2_53_51_562,(uint8*)"");
        return s_sim_sms_num[MN_DUAL_SYS_1];
    }
    return s_sim_sms_num[dual_sys];
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetMaxSimSmsNum(MN_DUAL_SYS_E dual_sys, uint8 sim_sms_num)
{
    if (dual_sys >=  MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurSaveDualSys dual_sys >=  MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2689_112_2_18_2_53_51_563,(uint8*)"");
        return ;
    }
    s_sim_sms_num[dual_sys] = sim_sms_num;
}

/*****************************************************************************/
//  Description : Init Folder Type flag
//  Global resource dependence : 
//  Author: fengming.huang
//  Note:
/*****************************************************************************/
LOCAL void InitFolderTypeFlag(void)
{
    MMINV_SMS_FOLDER_TYPE_T *folder_type_manager = PNULL;

    folder_type_manager = SCI_ALLOCA(sizeof(MMINV_SMS_FOLDER_TYPE_T));

    if(PNULL == folder_type_manager)
    {
        return;
    }

    SCI_MEMSET(folder_type_manager, 0x00, sizeof(MMINV_SMS_FOLDER_TYPE_T));
    MMISMS_NV_ReadFolderTypeManager(folder_type_manager);

	SCI_MEMCPY(
		g_mmisms_global.nv_param.folder_type,
		folder_type_manager,
		sizeof(MMINV_SMS_FOLDER_TYPE_T)
		);

    SCI_FREE(folder_type_manager);
}

/*****************************************************************************/
//     Description : to init the flag of security pwd
//    Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
LOCAL void InitSecurityPwd(void)
{
    MMISMS_SECURITYBOX_PWD_T pwd = {0};
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    MMISMS_SECURITYBOX_PWD_T security_default_pwd ={{0},MMISMS_SECURITY_PWD_DEFAULT_LEN};    
    uint16 default_pwd_len =0;
    
    MMINV_READ(MMINV_SMS_SECURITY_PWD, &pwd, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        default_pwd_len = SCI_STRLEN(MMISMS_GetSecurityDefaultPwd());
        SCI_MEMCPY(security_default_pwd.pwd,MMISMS_GetSecurityDefaultPwd(),default_pwd_len);

        MMINV_WRITE(MMINV_SMS_SECURITY_PWD,&security_default_pwd);
    }
}

#if defined(MMI_SMS_EXPORT_SUPPORT)
/*****************************************************************************/
//     Description : to init the flag of export method
//    Global resource dependence : g_mmisms_global
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
LOCAL void InitExportMethod(void)
{
    MMISMS_SETTING_PATH_TYPE method = 0;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;
    MMISMS_SETTING_PATH_TYPE default_method = MMISMS_SETTING_PATH_DEFAULT;
    
    MMINV_READ(MMINV_SMS_EXPORT_METHOD, &method, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        MMINV_WRITE(MMINV_SMS_EXPORT_METHOD,&default_method);
    }
}
#endif

/*****************************************************************************/
//     Description : to init the factory setting of security pwd
//    Global resource dependence : 
//  Author: fengming.huang
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetSecurityPwd(void)
{
    // MMISMS_SECURITYBOX_PWD_T pwd = {0};
    MMISMS_SECURITYBOX_PWD_T security_default_pwd ={{0},MMISMS_SECURITY_PWD_DEFAULT_LEN};    
    uint16 default_pwd_len = 0;

    default_pwd_len = SCI_STRLEN(MMISMS_GetSecurityDefaultPwd());
    SCI_MEMCPY(security_default_pwd.pwd,MMISMS_GetSecurityDefaultPwd(),default_pwd_len);

    MMINV_WRITE(MMINV_SMS_SECURITY_PWD,&security_default_pwd);

}


/*****************************************************************************/
//     Description : set Current Search Order Id
//    Global resource dependence : g_mmisms_global
//  Author:liming.deng
//    Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetCurSearchOperationOrderId(
                                        MMISMS_BOX_TYPE_E box_type,
                                        MMISMS_ORDER_ID_T order_id)
{
    //MMISMS_ORDER_ID_T cur_order_id = PNULL;
    //int32 i = 0;
    
    //根据pos_index获得当前的order_id 
    if (PNULL == order_id)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetCurSearchOperationOrderId order_id = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2796_112_2_18_2_53_51_564,(uint8*)"");
        return;
    }    

    g_mmisms_global.operation.cur_type = box_type;
    
    MMISMS_SetSMSUpdateInfo(order_id);
    
    MMISMS_SetGlobalOperationOrderId(order_id);   
}

/*****************************************************************************/
//     Description : MMISMS_GetIsNeedReSort
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetIsNeedReSort(void)
{
    //SCI_TRACE_LOW:"MMISMS_GetIsNeedReSort s_is_sms_need_re_sort=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2813_112_2_18_2_53_51_565,(uint8*)"d", s_is_sms_need_re_sort);

    return s_is_sms_need_re_sort;
}

/*****************************************************************************/
//     Description : MMISMS_SetIsNeedReSort
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetIsNeedReSort(BOOLEAN is_need_re_sort)
{
    //SCI_TRACE_LOW:"MMISMS_SetIsNeedReSort is_need_re_sort=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2824_112_2_18_2_53_51_566,(uint8*)"d", is_need_re_sort);

    s_is_sms_need_re_sort = is_need_re_sort;
}

/*****************************************************************************/
//     Description : MMISMS_GetIsSMSRefresh
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetIsSMSRefresh(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return FALSE;
    }

    //SCI_TRACE_LOW:"MMISMS_GetIsSMSRefresh s_is_sms_refresh[%d]=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2840_112_2_18_2_53_51_567,(uint8*)"dd", dual_sys, s_is_sms_refresh[dual_sys]);

    return s_is_sms_refresh[dual_sys];
}

/*****************************************************************************/
//     Description : MMISMS_SetIsSMSRefresh
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetIsSMSRefresh(MN_DUAL_SYS_E dual_sys, BOOLEAN is_sms_refresh)
{
    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    //SCI_TRACE_LOW:"MMISMS_SetIsSMSRefresh is_sms_refresh[%d]=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2856_112_2_18_2_53_51_568,(uint8*)"dd", dual_sys, is_sms_refresh);

    s_is_sms_refresh[dual_sys] = is_sms_refresh;
}

/*****************************************************************************/
//     Description : is sim not initial 
//    Global resource dependence : 
//  Author:louis.wei
//    Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsSimNotInited(void)
{
    uint32 i = 0;
    BOOLEAN is_not_inited = FALSE;

    for(i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if(SIM_NOT_INITED == MMIAPIPHONE_GetSimStatus((MN_DUAL_SYS_E)i))
        {
            is_not_inited = TRUE;
            break;
        }
    }

    return is_not_inited;
}

/*****************************************************************************/
//  Description : is move operation 
//  Global resource dependence : 
//  Author:fengming.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsMoveOperation(MMISMS_OPER_STATUS_E oper_status)
{
    if((MMISMS_MOVE_SMS_TO_SECURITY == oper_status) ||
       (MMISMS_MOVE_SMS_FROM_SECURITY == oper_status) ||
       (MMISMS_MOVE_SMS_TO_ME == oper_status) ||
       (MMISMS_MOVE_SMS_TO_SIM == oper_status))
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************************/
//  Description : is copy operation 
//  Global resource dependence : 
//  Author:fengming.huang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_IsCopyOperation(MMISMS_OPER_STATUS_E oper_status)
{
    if((MMISMS_COPY_SMS_TO_ME == oper_status) ||
       (MMISMS_COPY_SMS_TO_SIM == oper_status))
    {
        return TRUE;
    }

    return FALSE;    
}

/*****************************************************************************/
//  Description : set Current operation order
//  Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//  Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetGlobalOperationOrderId(MMISMS_ORDER_ID_T order_id)
{
    MMISMS_ORDER_ID_T cur_order_id = PNULL;
    int32 i = 0;
    
    g_mmisms_global.operation.cur_order_index = 0;
    SCI_MEMSET(g_mmisms_global.operation.cur_order_id_arr, 0 , 
        sizeof(g_mmisms_global.operation.cur_order_id_arr)); 
    
    if (PNULL != order_id && order_id->flag.is_concatenate_sms) //is long sms
    {
        cur_order_id = order_id;
        while (cur_order_id != PNULL)
        {
            MMISMS_SetCurOrderId(i,cur_order_id); //设置当前order_id到全局变量数组
            cur_order_id  = cur_order_id->next_long_sms_ptr;
            i++;
        }
    }
    else //is normal sms
    {
        MMISMS_SetCurOrderId(0,order_id); //设置当前order_id到全局变量数组 
    }     
}

#ifdef MMI_SMS_CHAT_SUPPORT
/*****************************************************************************/
//  Description : set Current Order Id
//  Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//  Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetChatCurOperationOrderId(uint32 index)
{
    MMISMS_ORDER_ID_T order_id = PNULL;
    MMISMS_ORDER_ID_T cur_order_id = PNULL;
    int32 i = 0;
    
    //根据pos_index获得当前的order_id 
    order_id = MMISMS_GetNumNodeByIndex(index);; 
    if (PNULL == order_id)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetChatCurOperationOrderId order_id = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2958_112_2_18_2_53_52_569,(uint8*)"");
        return;
    }    
    MMISMS_SetSMSUpdateInfo(order_id);
    
    g_mmisms_global.operation.cur_order_index = 0;
    SCI_MEMSET(g_mmisms_global.operation.cur_order_id_arr, 0 , 
        sizeof(g_mmisms_global.operation.cur_order_id_arr)); 
    
    if (order_id->flag.is_concatenate_sms) //is long sms
    {
        cur_order_id = order_id;
        while (cur_order_id != PNULL)
        {
            MMISMS_SetCurOrderId(i,cur_order_id); //设置当前order_id到全局变量数组
            cur_order_id  = cur_order_id->next_long_sms_ptr;
            i++;
        }
    }
    else //is normal sms
    {
        MMISMS_SetCurOrderId(0,order_id); //设置当前order_id到全局变量数组 
    }    
}

#endif

#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
/*****************************************************************************/
//  Description : set Current Order Id
//  Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//  Note:  
/*****************************************************************************/
PUBLIC void MMISMS_SetPushMsgCurOperationOrderId(uint32 index)
{
    MMISMS_ORDER_ID_T order_id = PNULL;
    
    //根据pos_index获得当前的order_id 
    order_id = MMISMS_GetPushMsgLinkedArrayPtrId(index);
    if (PNULL == order_id)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SetChatCurOperationOrderId order_id = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_2998_112_2_18_2_53_52_570,(uint8*)"");
        return;
    }    
    MMISMS_SetSMSUpdateInfo(order_id);
    
    g_mmisms_global.operation.cur_order_index = 0;
    SCI_MEMSET(g_mmisms_global.operation.cur_order_id_arr, 0 , 
        sizeof(g_mmisms_global.operation.cur_order_id_arr)); 
    
    MMISMS_SetCurOrderId(0,order_id); //设置当前order_id到全局变量数组 
}
#endif

#ifdef MMIMMS_SMS_IN_1_SUPPORT
/*****************************************************************************/
//  Description : init update info
//  Global resource dependence : g_mmisms_global
//  Author:rong.gu
//  Note:  
/*****************************************************************************/
PUBLIC void MMISMS_InitUpdateInfo(void)
{
    SCI_MEMSET(((void*)&g_mmisms_global.update_info),0,sizeof(MMISMS_UPDATE_INFO_T));//不更新状态
}
#endif

#ifdef MMI_SMS_CONCATENATED_SETTING_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of concatenated sms
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitContcatenatedSMS(void)
{
    BOOLEAN is_concatenated_sms = TRUE;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;    
    
    MMINV_READ(MMINV_SMS_CONCATENATED_SMS, &is_concatenated_sms, nv_value);
    
    if (MN_RETURN_SUCCESS !=nv_value)
    {
        is_concatenated_sms = MMISMS_IS_CONCATENATED_SMS_DEFAULT;
        MMISMS_SetConcatenatedSMS( is_concatenated_sms );
    }

    g_mmisms_global.nv_param.is_concatenated_sms = is_concatenated_sms;        
}

/*****************************************************************************/
//     Description : to Set the concatenated sms
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetConcatenatedSMS(
                                BOOLEAN is_concatenated_sms
                                )
{
    MMINV_WRITE(MMINV_SMS_CONCATENATED_SMS, &is_concatenated_sms);
    
    g_mmisms_global.nv_param.is_concatenated_sms = is_concatenated_sms;
}

/*****************************************************************************/
//     Description : to Get the concatenated sms
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_GetConcatenatedSMS(void)
{
    return g_mmisms_global.nv_param.is_concatenated_sms;
}
#endif

#ifdef MMI_SMS_ALPHABET_SETTING_SUPPORT
/*****************************************************************************/
//     Description : to init the flag of alphabet management
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void InitAlphabetManagement(void)
{
    MMISMS_ALPHABET_MANAGEMENT_E alpha_management = MMISMS_ALPHABET_MANAGEMENT_DEFAULT;
    MN_RETURN_RESULT_E nv_value = MN_RETURN_SUCCESS;    
    
    MMINV_READ(MMINV_SMS_ALPHABET_MANAGEMENT, &alpha_management, nv_value);
    
    if (MN_RETURN_SUCCESS != nv_value)
    {
        alpha_management = MMISMS_ALPHABET_MANAGEMENT_DEFAULT;
        MMISMS_SetAlphabetManagement(alpha_management);
    }

    g_mmisms_global.nv_param.alpha_management = alpha_management;        
}

/*****************************************************************************/
//     Description : to Set the alphabet management
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetAlphabetManagement(
                                MMISMS_ALPHABET_MANAGEMENT_E alpha_management
                                )
{
    MMINV_WRITE(MMINV_SMS_ALPHABET_MANAGEMENT, &alpha_management);
    
    g_mmisms_global.nv_param.alpha_management = alpha_management;
}

/*****************************************************************************/
//     Description : to Get the alphabet management
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_ALPHABET_MANAGEMENT_E MMISMS_GetAlphabetManagement(void)
{
    //SCI_TRACE_LOW:"MMISMS: MMISMS_GetAlphabetManagement alpha_management=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_3106_112_2_18_2_53_52_571,(uint8*)"d", g_mmisms_global.nv_param.alpha_management);

    return g_mmisms_global.nv_param.alpha_management;
}
#endif

#ifdef IKEYBACKUP_SUPPORT
/*****************************************************************************/
//  Description : SMS一键备份回调函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL void sms_backup_callback(
                               MMIIKB_BACKUP_CALLBACK_PARAM_IN_T *param_in_ptr,
                               MMIIKB_BACKUP_CALLBACK_PARAM_OUT_T *param_out_ptr
                               )
{
    MMIIKB_ERROR_E error_code = MMIIKB_STATUS_INVALID_PARAM;
    MMIIKB_SIG_BACKUP_ITEM_DATA_REQ_T *sig_item_data_req_ptr = PNULL;
    uint16 sig_len = 0;
    
    if (PNULL == param_in_ptr
        || PNULL == param_out_ptr)
    {
        return;
    }

    sig_len = sizeof(MMIIKB_SIG_BACKUP_ITEM_DATA_REQ_T);
    sig_item_data_req_ptr = (MMIIKB_SIG_BACKUP_ITEM_DATA_REQ_T*)SCI_ALLOCA(
        sig_len);
    if (PNULL != sig_item_data_req_ptr)
    {
        SCI_MEMSET(sig_item_data_req_ptr, 0x00, sig_len);

        sig_item_data_req_ptr->SignalCode = MSG_SMS_IKB_BACKUP_ITEM_DATA_REQ;
        sig_item_data_req_ptr->Sender = SCI_IdentifyThread();
        sig_item_data_req_ptr->SignalSize = sig_len;

        //copy data
        sig_item_data_req_ptr->backup_item_data_req.type 
            = param_in_ptr->type;
        sig_item_data_req_ptr->backup_item_data_req.op_type 
            = param_in_ptr->op_type;
        sig_item_data_req_ptr->backup_item_data_req.begin 
            = param_in_ptr->begin;
        sig_item_data_req_ptr->backup_item_data_req.len 
            = param_in_ptr->len;
        sig_item_data_req_ptr->backup_item_data_req.number 
            = param_in_ptr->number;
        sig_item_data_req_ptr->backup_item_data_req.trans_cnt 
            = param_in_ptr->trans_cnt;
        
        IKEYBACKUP_SEND_SIGNAL(sig_item_data_req_ptr, P_APP);

        error_code = MMIIKB_STATUS_SUCCESS;
    }
    else
    {
        error_code =  MMIIKB_STATUS_NO_MEMORY;
    }

    param_out_ptr->error_code = error_code;
    
    return;
}

/*****************************************************************************/
//  Description : SMS一键备份还原获取数量回调函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL void sms_get_total_callback(
                                  MMIIKB_GET_TOTAL_OUT_T *param_out_ptr
                                  )
{
    MMIIKB_ERROR_E error_code = MMIIKB_STATUS_SUCCESS;

    param_out_ptr->total_type = MMIIKB_TOTAL_TYPE_NUMBER;
    param_out_ptr->total = MMISMS_GetSmsNumByStorage(MN_DUAL_SYS_1, MN_SMS_STORAGE_ME);
    
    param_out_ptr->error_code = error_code;

    return;
}

/*****************************************************************************/
//  Description : SMS一键还原回调函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL void sms_restore_callback(
                                MMIIKB_RESTORE_CALLBACK_PARAM_IN_T *param_in_ptr,
                                MMIIKB_RESTORE_CALLBACK_PARAM_OUT_T *param_out_ptr
                                )
{
    MMIIKB_ERROR_E error_code = MMIIKB_STATUS_INVALID_PARAM;
    MMIIKB_SIG_RESTORE_ITEM_DATA_REQ_T *sig_item_data_req_ptr = PNULL;
    uint16 sig_len = 0;
    
    if (PNULL == param_in_ptr
        || PNULL == param_out_ptr)
    {
        return;
    }    
    
    if (MMIIKB_TYPE_SMS != param_in_ptr->type
        || MMIIKB_OPERATE_TYPE_BUF != param_in_ptr->op_type
        || 1 != param_in_ptr->number
        || sizeof(MMISMS_BACKUP_DATA_T) != param_in_ptr->len)
    {
        param_out_ptr->type=param_in_ptr->type;
        param_out_ptr->op_type=param_in_ptr->op_type;
        error_code = MMIIKB_STATUS_INVALID_PARAM;
        param_out_ptr->suc_cnt = 0;
        param_out_ptr->err_cnt = 1;
        param_out_ptr->error_code = error_code;
        return;
    }    

    //需要释放
    sig_len = sizeof(MMIIKB_SIG_RESTORE_ITEM_DATA_REQ_T);
    sig_item_data_req_ptr = (MMIIKB_SIG_RESTORE_ITEM_DATA_REQ_T*)SCI_ALLOCA(sig_len);
    if (PNULL != sig_item_data_req_ptr)
    {
        SCI_MEMSET(sig_item_data_req_ptr, 0x00, sig_len);

        sig_item_data_req_ptr->SignalCode = MSG_SMS_IKB_RESTORE_ITEM_DATA_REQ;
        sig_item_data_req_ptr->Sender = SCI_IdentifyThread();
        sig_item_data_req_ptr->SignalSize = sig_len;

        //copy data
        sig_item_data_req_ptr->restore_item_data_req.type
            = param_in_ptr->type;
        sig_item_data_req_ptr->restore_item_data_req.op_type
            = param_in_ptr->op_type;
        
        sig_item_data_req_ptr->restore_item_data_req.begin
            = param_in_ptr->begin;
        sig_item_data_req_ptr->restore_item_data_req.number
            = param_in_ptr->number;
        sig_item_data_req_ptr->restore_item_data_req.trans_idx
            = param_in_ptr->trans_idx;
        
        if (MMIIKB_OPERATE_TYPE_BUF == param_in_ptr->op_type)
        {
            sig_item_data_req_ptr->restore_item_data_req.len 
                = param_in_ptr->len;

            //需要释放
            sig_item_data_req_ptr->restore_item_data_req.buf_ptr = SCI_ALLOCA(
                param_in_ptr->len);
            if (PNULL == sig_item_data_req_ptr->restore_item_data_req.buf_ptr)
            {
                SCI_FREE(sig_item_data_req_ptr);
                param_out_ptr->error_code =  MMIIKB_STATUS_NO_MEMORY;
                return;
            }
            SCI_MEMSET(sig_item_data_req_ptr->restore_item_data_req.buf_ptr, 
                0x00, param_in_ptr->len);
            SCI_MEMCPY(sig_item_data_req_ptr->restore_item_data_req.buf_ptr, 
                param_in_ptr->buf_ptr, param_in_ptr->len);
        }    
        
        if (PNULL != g_restore_param_in_ptr)
        {
            SCI_FREE(g_restore_param_in_ptr);
        }
        g_restore_param_in_ptr = (MMIIKB_RESTORE_CALLBACK_PARAM_IN_T *)SCI_ALLOCA(sizeof(MMIIKB_RESTORE_CALLBACK_PARAM_IN_T));
        if (PNULL == g_restore_param_in_ptr)
        {
            SCI_FREE(sig_item_data_req_ptr);
            param_out_ptr->error_code =  MMIIKB_STATUS_NO_MEMORY;
            return;
        }
        SCI_MEMSET(g_restore_param_in_ptr, 0x00, sizeof(MMIIKB_RESTORE_CALLBACK_PARAM_IN_T));
        SCI_MEMCPY(g_restore_param_in_ptr, param_in_ptr, sizeof(MMIIKB_RESTORE_CALLBACK_PARAM_IN_T));
        
        IKEYBACKUP_SEND_SIGNAL(sig_item_data_req_ptr, P_APP);

        error_code = MMIIKB_STATUS_SUCCESS;
    }
    else
    {
        error_code =  MMIIKB_STATUS_NO_MEMORY;
    }

    param_out_ptr->error_code = error_code;
    
    return;
}

/*****************************************************************************/
//  Description : SMS一键备份还原注册函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
LOCAL BOOLEAN sms_ikeybackup_register(void)
{
    BOOLEAN is_ret = FALSE;
    MMIIKB_TYPE_PROCESS_PARAM_T ikb_param = {0};
    
    ikb_param.type = MMIIKB_TYPE_SMS;
    ikb_param.op_type = MMIIKB_OPERATE_TYPE_BUF;
    ikb_param.backup_call_back = sms_backup_callback;
    ikb_param.is_backup_asyn = TRUE;
    ikb_param.restore_call_back = sms_restore_callback;
    ikb_param.is_restore_asyn = TRUE;
    ikb_param.get_total_call_back = sms_get_total_callback;
    is_ret = MMIAPIIKB_RegTypeFunc(&ikb_param);
    
    return is_ret;
}

/*****************************************************************************/
//  Description : SMS备份函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
PUBLIC void MMISMS_IKeyBackup(MMIIKB_SIG_BACKUP_ITEM_DATA_REQ_T *backup_data_ptr)
{
    MMIIKB_ERROR_E error_code = MMIIKB_STATUS_INVALID_PARAM;
    MMIIKB_BACKUP_ITEM_DATA_REQ_T *item_data_req_ptr = PNULL;             
    MMIIKB_BACKUP_ITEM_DATA_RES_T item_data_res = {0};
    uint32 sms_total_num = MMISMS_GetSmsNumByStorage(MN_DUAL_SYS_1, MN_SMS_STORAGE_ME);
    MMISMS_ORDER_ID_T backup_record_id = PNULL;
              
    item_data_req_ptr = &(backup_data_ptr->backup_item_data_req);
    //SCI_TRACE_LOW:"MMISMS:MMISMS_IKeyBackup sms_total_num = %d, item_data_req_ptr->number = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_3328_112_2_18_2_53_52_572,(uint8*)"dd", sms_total_num, item_data_req_ptr->number);
    
    if (MMIIKB_TYPE_SMS != item_data_req_ptr->type
        || MMIIKB_OPERATE_TYPE_BUF != item_data_req_ptr->op_type)
    {
        error_code = MMIIKB_STATUS_INVALID_PARAM;
    }
    else
    {
        if (item_data_req_ptr->number > sms_total_num)
        {
            error_code = MMIIKB_STATUS_INVALID_PARAM;
        }
        else
        {
            //由于num是已经备份的数量，需要加1表示当前正在备份的index
            backup_record_id = MMISMS_FindMsgForSmsBackup(item_data_req_ptr->number + 1);
            if (PNULL == backup_record_id)
            {
                error_code = MMIIKB_STATUS_INVALID_PARAM;
            }
            else
            {
                if (PNULL != g_sms_backup_data_ptr)
                {
                    SCI_FREE(g_sms_backup_data_ptr);
                }
                g_sms_backup_data_ptr = (MMISMS_BACKUP_DATA_T *)SCI_ALLOCA(sizeof(MMISMS_BACKUP_DATA_T));
                if (PNULL != g_backup_item_data_req_ptr)
                {
                    SCI_FREE(g_backup_item_data_req_ptr);
                }
                g_backup_item_data_req_ptr = (MMISMS_BACKUP_DATA_T *)SCI_ALLOCA(sizeof(MMISMS_BACKUP_DATA_T));

                if (PNULL == g_sms_backup_data_ptr || PNULL == g_backup_item_data_req_ptr)
                {
                    if (PNULL != g_sms_backup_data_ptr)
                    {
                        SCI_FREE(g_sms_backup_data_ptr);
                    }
                    if (PNULL != g_backup_item_data_req_ptr)
                    {
                        SCI_FREE(g_backup_item_data_req_ptr);
                    }
                    error_code = MMIIKB_STATUS_NO_MEMORY;
                }
                else
                {
                    SCI_MEMSET(g_sms_backup_data_ptr, 0x00, sizeof(MMISMS_BACKUP_DATA_T));
                    
                    SCI_MEMSET(g_backup_item_data_req_ptr, 0x00, sizeof(MMISMS_BACKUP_DATA_T));
                    SCI_MEMCPY(g_backup_item_data_req_ptr, item_data_req_ptr, sizeof(MMIIKB_BACKUP_ITEM_DATA_REQ_T));
                    
                    g_sms_is_backup = TRUE;
                    g_sms_backup_data_ptr->record_id = backup_record_id->record_id;
                    g_sms_backup_data_ptr->mo_mt_type = backup_record_id->flag.mo_mt_type;
                    
                    g_sms_backup_data_ptr->dual_sys = backup_record_id->flag.dual_sys;
                    g_sms_backup_data_ptr->storage = backup_record_id->flag.storage;
                    g_sms_backup_data_ptr->folder_type = backup_record_id->flag.folder_type;
                    
                    g_sms_backup_data_ptr->time = backup_record_id->time;
                    SCI_MEMCPY(&g_sms_backup_data_ptr->number, &backup_record_id->number.bcd_number, sizeof(MMISMS_BCD_NUMBER_T));
                    
                    MNSMS_ReadSmsEx((MN_DUAL_SYS_E)backup_record_id->flag.dual_sys, (MN_SMS_STORAGE_E)backup_record_id->flag.storage, backup_record_id->record_id);
                    error_code = MMIIKB_STATUS_SUCCESS;
                }                
            }
        }
    }
    if (MMIIKB_STATUS_SUCCESS != error_code)
    {
        item_data_res.error_code = error_code;                
        MMIAPIIKB_SendBackupItemDataRes(&item_data_res);
    }
}

/*****************************************************************************/
//  Description : SMS还原函数
//  Global resource dependence : none
//  Author: minghu.mao
//  Note: 
/***************************************************************************/
PUBLIC void MMISMS_IKeyRestore(MMIIKB_SIG_RESTORE_ITEM_DATA_REQ_T *restore_data_ptr)
{
    MMISMS_BACKUP_DATA_T backup_data = {0};
    MMIIKB_RESTORE_ITEM_DATA_REQ_T * restore_req_data_ptr = PNULL;
    MMIIKB_RESTORE_ITEM_DATA_RES_T item_data_res = {0};
    MMIIKB_ERROR_E error_code = MMIIKB_STATUS_INVALID_PARAM;
    APP_SMS_USER_DATA_T ascii_vaild = {0};

    restore_req_data_ptr = &(restore_data_ptr->restore_item_data_req);

    SCI_MEMCPY(&backup_data, restore_req_data_ptr->buf_ptr, sizeof(MMISMS_BACKUP_DATA_T));
    
    if (!MMISMS_IsExistSameSms(&backup_data))
    {//保存sms
        MMISMS_SetOperStatus(MMISMS_RESTORE_SMS);
        MMISMS_ReadyReadSms();
        g_sms_restore_status = backup_data.status;
        g_sms_restore_folder_type = backup_data.folder_type;
        g_mo_mt_type = backup_data.mo_mt_type;
        g_sms_save_time = backup_data.time;
        //set the content to globle for save order info
        if (MN_SMS_MT_TO_BE_READ == g_sms_restore_status
            || MN_SMS_MT_READED == g_sms_restore_status
            || MN_SMS_MT_SR_READED == g_sms_restore_status
            || MN_SMS_MT_SR_TO_BE_READ == g_sms_restore_status
            )
        {
            MNSMS_WriteMtSmsEx(backup_data.dual_sys, backup_data.storage, backup_data.status, &backup_data.sms_t_u.mt_sms_t);
            MMISMS_DecodeUserHeadAndContent(backup_data.dual_sys,
                backup_data.sms_t_u.mt_sms_t.user_head_is_exist,
                &backup_data.sms_t_u.mt_sms_t.dcs,
                &backup_data.sms_t_u.mt_sms_t.user_data_t,
                &ascii_vaild
                );
            // add the readed SMS to global
            MMISMS_SaveReadedSmsToGlobal(
                backup_data.dual_sys,
                MMISMS_READ_MTSMS,
                &ascii_vaild,  
                &(backup_data.sms_t_u.mt_sms_t.origin_addr_t),
                backup_data.sms_t_u.mt_sms_t.dcs.alphabet_type
                );

            g_mmisms_global.read_msg.dual_sys    = backup_data.dual_sys;
        }
        else
        {
            MMISMS_DecodeUserHeadAndContent(backup_data.dual_sys,
                backup_data.sms_t_u.mo_sms_t.user_head_is_exist,
                &backup_data.sms_t_u.mo_sms_t.dcs,
                &backup_data.sms_t_u.mo_sms_t.user_data_t,
                &ascii_vaild
                );
            
            //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUserReadSMSCnf head_len = %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMSAPP_MAIN_3460_112_2_18_2_53_53_573,(uint8*)"d", ascii_vaild.user_data_head_t.length);
            // add the readed SMS to global
            MMISMS_SaveReadedSmsToGlobal(
                backup_data.dual_sys,
                MMISMS_READ_MOSMS,
                &ascii_vaild,  
                &(backup_data.sms_t_u.mo_sms_t.dest_addr_t),
                backup_data.sms_t_u.mo_sms_t.dcs.alphabet_type
                );
            g_mmisms_global.read_msg.dual_sys = backup_data.dual_sys;
            MNSMS_WriteMoSmsEx(backup_data.dual_sys, backup_data.storage, backup_data.status, &backup_data.sms_t_u.mo_sms_t);
        }
    }
    else
    {
        if (PNULL != g_restore_param_in_ptr)
        {
            error_code = MMIIKB_STATUS_SUCCESS;
            item_data_res.op_type = g_restore_param_in_ptr->op_type;
            item_data_res.type = g_restore_param_in_ptr->type;
            item_data_res.error_code = error_code;
            item_data_res.suc_cnt = 1;
            item_data_res.err_cnt = 0;
            if (PNULL != g_restore_param_in_ptr)
            {
                SCI_FREE(g_restore_param_in_ptr);
                g_restore_param_in_ptr = PNULL;
            }
            MMIAPIIKB_SendRestoreItemDataRes(&item_data_res);
        }
        else
        {
            item_data_res.error_code = MMIIKB_STATUS_NO_MEMORY;
            MMIAPIIKB_SendRestoreItemDataRes(&item_data_res);            
        }                
    }    
    //释放sms_restore_callback中分配的空间
    SCI_FREE(restore_req_data_ptr->buf_ptr);
}
#endif

