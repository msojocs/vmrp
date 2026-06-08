/****************************************************************************
** File Name:      mmisms_receive.c                                         *
** Author:                                                                  *
** Date:           8/23/2007                                                *
** Copyright:      2007 Spreadtrum, Incorporated. All Rights Reserved.      *
** Description:    This file is used to describe receiving SMS.             *
*****************************************************************************
**                         Important Edit History                           *
** -------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                              *
** 08/2007        liming.deng      Create                                   *
**                                                                          *
****************************************************************************/


/**-------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "mmi_app_sms_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmisms_receive.h"
#include "window_parse.h"
#include "guitext.h"
#include "mmi_text.h"
#include "mmi_image.h"
#include "mmiidle_export.h"
#include "tb_dal.h"
#include "mmipb_export.h"

#include "mmistk_export.h"
#include "mmi_atc.h"
#include "mmiphone_export.h"
#include "mmi_textfun.h"
#include "guilistbox.h"
#include "mmimms_export.h"
#include "mmi_appmsg.h"
//#include "mmiset.h"
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
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
//#include "sci_types.h"
#include "mmisms_order.h"
#include "mmisms_read.h"
#include "mmivcard_export.h"
#include "mmiudisk_export.h"
#include "mmidm_export.h"
#include "mmisms_save.h"
#include "mmisms_set.h"
#include "gui_ucs2b_converter.h"
#include "mmisms_send.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#endif

#include "mmimtv_export.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
//#include "mmiset.h"
#include "mmiset_export.h"

#ifdef _ATEST_
#include "atest_ota.h"
#endif//_ATEST_

#ifdef MOBILE_VIDEO_SUPPORT
#include "mv_sms_api.h"
#endif
#ifdef MMI_VCALENDAR_SUPPORT
#include "Mmivcalendar.h"
#endif

#ifdef SCREENSAVER_SUPPORT
#include "mmiss_export.h"  
#endif
#ifdef MET_MEX_SUPPORT
#include "met_mex_export.h"
#include "met_mex_app_export.h"
#endif

#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif

#define MMISMS_STATUS_REPORT_LEN    30

/**--------------------------------------------------------------------------*
**                         STATIC DEFINITION                                *
**--------------------------------------------------------------------------*/
LOCAL VCARD_LONG_SMS_T *s_long_vcardsms_ptr = PNULL;//记录Vcard
#ifdef MMI_VCALENDAR_SUPPORT
LOCAL VCARD_LONG_SMS_T *s_long_vcalsms_ptr = PNULL;
#endif

#ifdef MMI_STATUSBAR_SCROLLMSG_SUPPORT
typedef struct
{
    MN_DUAL_SYS_E dual_sys;
    MN_SMS_STORAGE_E storage;
    BOOLEAN is_concatenate_sms;
    uint8 longsms_max_num;
    uint8 longsms_seq_num;
    uint16 head_ref;
    MMISMS_READ_TYPE_E msg_type;
    MMISMS_READ_MSG_T read_msg;
    uint32 record_id;
}MMI_STATUSBAR_SCROLLMSG_T;

MMI_STATUSBAR_SCROLLMSG_T s_sms_statusbar_scrollmsg = {0};
#endif

/**--------------------------------------------------------------------------*
**                         EXTERN DECLARE                                *
**--------------------------------------------------------------------------*/
extern MMISMS_GLOBAL_T  		   g_mmisms_global; // SMS的全局变量
#ifdef IKEYBACKUP_SUPPORT
extern BOOLEAN g_sms_is_backup;
#endif

#ifdef SXH_APPLE_SUPPORT
// extern BOOLEAN Apple_Get_ps_mode(void);
#include "apple_export.h"
#endif
/*****************************************************************************/
//     Description : to insert new message to order list
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN InsertNewMtToOrder(                                                //RETURN:
                              MN_DUAL_SYS_E                dual_sys,            //IN:
                              MN_SMS_STORAGE_E            storage,            //IN:
                              MN_SMS_RECORD_ID_T            record_id,            //IN:
                              MN_SMS_STATUS_E                status,                //IN:
                              APP_SMS_USER_DATA_T         *user_data_ptr,     //IN:
                              MN_SMS_MT_SMS_T *mt_sms_ptr,//IN:
                              BOOLEAN                     is_mms_push,        //IN:
                              BOOLEAN                    is_unhandle_order
                              );

/*****************************************************************************/
//     Description : operate MT ind as mms
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN OperateMtIndAsMmsPush(                                    //RETURN: TRUE, indicate the operation is finished
                                    MN_DUAL_SYS_E       dual_sys,
                                    APP_SMS_USER_DATA_T    *user_data_ptr,    //IN:
                                    MN_SMS_STORAGE_E    storage,        //IN:
                                    MN_SMS_RECORD_ID_T    record_id,        //IN:
                                    MN_SMS_MT_SMS_T *mt_sms_ptr//IN:
                                    );

/*****************************************************************************/
// Description : receive Vcard over sms
// Global resource dependence :s_long_vcardsms_ptr
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL void MMISMS_RecvVcardOrVcalSms(
                                     MN_DUAL_SYS_E         dual_sys, 
                                     APP_SMS_USER_DATA_T   *sms_ptr,
                                     MN_SMS_TIME_STAMP_T   *time_stamp_ptr,
                                     MMIVC_SMS_SEND_TYPE_E send_type
                                     );
#if defined MMI_VCARD_SUPPORT
/*****************************************************************************/
// Description : save received Vcard data over sms
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL void MMISMS_SaveVcardSms(
                               uint8   *name_ptr,
                               uint8   *buf_ptr,
                               uint16   buf_len
                               );
#endif
/*****************************************************************************/
// Description : 获得vCard文件名称(名称由SMS+接收时间字串组成)
// Global resource dependence : 
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL void GetVcardFileNameOverSms(
                                   MN_SMS_TIME_STAMP_T  *sc_time_ptr,
                                   uint8                *name_ptr
                                   );

/*****************************************************************************/
// Description : Get SM receive time stamp
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL uint32 GetSmsReceiveTimeStamp(void);

/*****************************************************************************/
//     Description : load SMS content to window
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void LoadSms2Window(
                          MN_SMS_ALPHABET_TYPE_E        alphabet_type,        //IN:
                          BOOLEAN                        is_only_display,    //IN: TRUE, indicate the SMS class is 0
                          uint16                        msg_len,            //IN:
                          uint8                         *msg_data_ptr,        //IN:
                          MN_SMS_TIME_STAMP_T            *sc_time_ptr,        //IN:
                          MN_CALLED_NUMBER_T            *orginal_addr_ptr,    //IN:
                          MMISMS_R8_LANGUAGE_E language
                          );

/*****************************************************************************/
//     Description : to show the content of message
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void ShowMsgContent(
                          MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                          uint16                    msg_len,            //IN:
                          uint8                     *msg_data_ptr,        //IN:
                          MN_SMS_TIME_STAMP_T        *sc_time_ptr,        //IN:
                          MN_CALLED_NUMBER_T        *orginal_addr_ptr,    //IN:
                          MMISMS_R8_LANGUAGE_E    language,
#ifdef MMI_SMS_CHAT_SUPPORT                          
                          BOOLEAN                    is_chat_mode,
                          BOOLEAN                    is_need_time,
#endif                          
                          MMI_STRING_T                *string_ptr            //OUT:
                          );

/*****************************************************************************/
//     Description : Get transmitter disply string
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void GetTransmitterDispString(
                                    MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                                    MN_CALLED_NUMBER_T        *orginal_addr_ptr,    //IN:
                                    MMI_STRING_T            *string_ptr            //OUT:
                                    );

/*****************************************************************************/
// Description : save received Vcard data over sms
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL VCARD_LONG_SMS_T* InitLongVcardsmsStruct(void);

/*****************************************************************************/
//     Description : operate MT ind as SMS
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note: except class 0
/*****************************************************************************/
LOCAL BOOLEAN OperateMtIndAsSms(                                        //RETURN: TRUE, indicate the operation is finished
                                MN_DUAL_SYS_E           dual_sys,
                                APP_SMS_USER_DATA_T        *user_data_ptr,    //IN:
                                MN_SMS_STORAGE_E        storage,        //IN:
                                MN_SMS_RECORD_ID_T        record_id,        //IN:
                                MN_SMS_MT_SMS_T *mt_sms_ptr//IN:
                                );

/*****************************************************************************/
//  Description : add sms content item to richtext
//  Global resource dependence : 
//  Author: Kun Yu
//  Note:
/*****************************************************************************/
LOCAL void AddSMSContentItemtoRichText( 
                          MMI_CTRL_ID_T ctrl_id,
                          BOOLEAN is_only_display,    //IN: TRUE, indicate the SMS class is 0
                          MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                          uint16                    msg_len,            //IN:
                          uint8                     *msg_data_ptr,        //IN:
                          MN_SMS_TIME_STAMP_T        *sc_time_ptr,        //IN:
                          MN_CALLED_NUMBER_T        *orginal_addr_ptr    //IN:
                          
                          );
#ifdef MMI_VCALENDAR_SUPPORT
/*****************************************************************************/
// Description : save received Vcalendar data over sms
// Global resource dependence :
// Author:renyi.hu
// Note: 
/*****************************************************************************/
LOCAL void MMISMS_SaveVcalendarSms(
                               uint8   *name_ptr,
                               uint8   *buf_ptr,
                               uint16   buf_len
                               );
#endif
/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/

#ifdef JAVA_SUPPORT_IA
/*****************************************************************************/
// 	Description : to add the java data to global, and inform jvm
//	Global resource dependence : g_mmisms_javadata
//  Author: kfyy.ye
//      @Params:    (in)java_data_ptr - pointer to the java data to set
//      @Return:    None
//	Note:
/*****************************************************************************/
LOCAL void AddJavaSMSData(MMISMS_JAVA_DATA_T* java_data_ptr)
{
	MMISMS_JAVA_GLOBAL_T*   	java_global_ptr = MMIAPIJAVA_GetSmsJavaData();
	MMISMS_JAVA_DATA_T*     	tmp_data_ptr    = PNULL;
	SMS_CALLBACK              	javasms_func    = PNULL;
	SMS_Message					javamessage = {0};

    if (PNULL == java_data_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:AddJavaSMSData java_data_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_299_112_2_18_2_53_2_178,(uint8*)"");
        return;
    }
    
    //SCI_TRACE_LOW:"MMISMS:AddJavaSMSData java_data_ptr = is_long =%d,total_num=%d,cur_num=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_303_112_2_18_2_53_2_179,(uint8*)"ddd",java_data_ptr->is_long,java_data_ptr->total_num,java_data_ptr->cur_num);


    // insert the data to global
    if (!java_data_ptr->is_long || java_data_ptr->cur_num == 1)
    {
        // new data, need to insert to TAIL
        if (java_global_ptr->java_data_root == PNULL)
        {
            java_global_ptr->java_data_root = java_data_ptr;
        }
        else
        {
            tmp_data_ptr = java_global_ptr->java_data_root;
            while (tmp_data_ptr->next_ptr != PNULL)
            {
                tmp_data_ptr = tmp_data_ptr->next_ptr;
            }
            tmp_data_ptr->next_ptr = java_data_ptr;

            // update new data
            java_data_ptr->previous_ptr = tmp_data_ptr;
        }
    }

    // inform java virtual machine to process the new mt sms
    if (!java_data_ptr->is_long || java_data_ptr->cur_num == java_data_ptr->total_num)
    {
        MMIAPIJAVA_CheckIfJavaPort(java_data_ptr->dst_port, &javasms_func, TRUE);
        if (PNULL == javasms_func)
        {
            //SCI_TRACE_LOW:"MMISMS:AddJavaSMSData javasms_func = PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_334_112_2_18_2_53_2_180,(uint8*)"");
            return;
        }
        javamessage.binaryData = java_data_ptr->binaryData;
        javamessage.binaryDataLength = java_data_ptr->binaryDataLength;
        javamessage.dstPort = java_data_ptr->dst_port;
        javamessage.isTimeStampAvailable = java_data_ptr->isTimeStampAvailable;
        javamessage.srcPort = java_data_ptr->src_port;
        javamessage.textData = java_data_ptr->textData;
        javamessage.textDataLength = java_data_ptr->textDataLength;
        javamessage.timeStampHigh = java_data_ptr->timeStampHigh;
        javamessage.timeStampLow = java_data_ptr->timeStampLow;
        javamessage.type = java_data_ptr->type;
        javamessage.url = (char *)java_data_ptr->addr_buf;
        javamessage.urlLength =java_data_ptr->addr_len;
        javasms_func(JAVA_NEW_MT_IND, (int)&javamessage);
       
        // data is processed by jvm, remove it from global
        if (java_data_ptr->previous_ptr != PNULL)
        {
            java_data_ptr->previous_ptr->next_ptr = java_data_ptr->next_ptr;
        }
        else
        {
            java_global_ptr->java_data_root = java_data_ptr->next_ptr;
            
        }
        if (java_data_ptr->next_ptr != PNULL)
        {
            java_data_ptr->next_ptr->previous_ptr = java_data_ptr->previous_ptr;
        }

        // and delete the data
        if (PNULL != java_data_ptr->textData)
        {
            SCI_FREE(java_data_ptr->textData);
        }
        if (PNULL != java_data_ptr->binaryData)
        {
            SCI_FREE(java_data_ptr->binaryData);
        }
        SCI_FREE(java_data_ptr);

    }
}
/*****************************************************************************/
//     Description : operate MT ind as for java
//    Global resource dependence : 
//  Author: kelly.li
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN OperateMtIndForJava(                                    //RETURN: 
                                                                    MN_DUAL_SYS_E dual_sys,
                                                                    APP_SMS_USER_DATA_T    *user_data_ptr,    //IN:
                                                                    MN_SMS_STORAGE_E    storage,        //IN:
                                                                    MN_SMS_RECORD_ID_T    record_id,        //IN:
                                                                    MN_SMS_MT_SMS_T        sms_t
                                                                    )
{
    BOOLEAN is_finished = TRUE;
    BOOLEAN is_new_java = FALSE;
//    CPL_SmsMessage new_java_info = {0};
    #ifdef JAVA_SUPPORT_IA
    MMISMS_JAVA_GLOBAL_T*   java_global_data_ptr = MMIAPIJAVA_GetSmsJavaData();
    MMISMS_JAVA_DATA_T      *java_data_ptr   = PNULL;    
    uint8 	                limit_bytes = 0;
	uint8	                split_bytes = 0;
    char *                  data_ptr = PNULL;
    #endif
    uint8 *valid_date_ptr = PNULL;
    uint8 iei_num = 0;
    int32 i = 0, index = -1;
    int32 des_port = 0,org_port = 0;
    MN_SMS_USER_HEAD_T user_head_ptr_t = {0};
     int32 seq_id = 0, seq_indx = 0,seq_len = 0;
	 //int32 is_cbs = 0;
    // uint8 addr[ MMISMS_MAX_USERNUMBER_LEN + 2 ] = {0};
    // uint16 num_len = 0;
    
    if (PNULL == user_data_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: user_data_ptr = PNULL!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_413_112_2_18_2_53_2_181,(uint8*)"");
        return FALSE;
    }

    MNSMS_DecodeUserDataHeadEx(
        dual_sys,
        &user_data_ptr->user_data_head_t,
        &user_head_ptr_t
        );

    // 判断是否为java sms
    is_new_java = MMISMS_IsMtForJava(user_head_ptr_t,&des_port,&org_port);
    
    //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: is_new_java_sms = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_426_112_2_18_2_53_2_182,(uint8*)"d",is_new_java);
    
    if (is_new_java)
    {    
        valid_date_ptr = SCI_ALLOCAZ(user_data_ptr->user_valid_data_t.length);
        if (PNULL == valid_date_ptr)
        {
            //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: valid_date_ptr alloc fail!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_433_112_2_18_2_53_2_183,(uint8*)"");
            return FALSE;
        }
        //SCI_MEMSET(valid_date_ptr, 0, user_data_ptr->user_valid_data_t.length);
        
        SCI_MEMCPY(valid_date_ptr,user_data_ptr->user_valid_data_t.user_valid_data_arr,user_data_ptr->user_valid_data_t.length);
        
        iei_num = user_head_ptr_t.iei_num;
        for (i = 0; i <iei_num; i++)
        {
            //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: iei_arr[%d].iei=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_443_112_2_18_2_53_2_184,(uint8*)"dd",i,user_head_ptr_t.iei_arr[i].iei);
            if((user_head_ptr_t.iei_arr[i].iei == MN_SMS_CONCAT_8_BIT_REF_NUM)
                ||(user_head_ptr_t.iei_arr[i].iei == MN_SMS_CONCAT_16_BIT_REF_NUM))
            {
                index = i;
                break;
            }
            else
            {
                index = -1;
            }
        }
        
        if (index != -1)    //该sms 为长短信
        {
            if (user_head_ptr_t.iei_arr[index].iei == MN_SMS_CONCAT_8_BIT_REF_NUM)
            {
                seq_id = user_head_ptr_t.iei_arr[index].iei_data_t[0];
                seq_len = user_head_ptr_t.iei_arr[index].iei_data_t[1];
                seq_indx = user_head_ptr_t.iei_arr[index].iei_data_t[2];    
            }
            else if (user_head_ptr_t.iei_arr[index].iei == MN_SMS_CONCAT_16_BIT_REF_NUM)
            {
                seq_id = user_head_ptr_t.iei_arr[index].iei_data_t[0];
                seq_id += user_head_ptr_t.iei_arr[index].iei_data_t[1];
                seq_len = user_head_ptr_t.iei_arr[index].iei_data_t[2];
                seq_indx = user_head_ptr_t.iei_arr[index].iei_data_t[3];
            }
        }
        else
        {
            //不是长短信
            seq_len = 1;
            seq_id  = record_id;
            seq_indx = 1;
        }
        
        //set CPL_SmsMessage info
#ifdef JAVA_SUPPORT_IA        
        
        java_data_ptr = java_global_data_ptr->java_data_root;
        while ((PNULL != java_data_ptr)&&(java_data_ptr->dst_port != des_port)&&(java_data_ptr->src_port != org_port)&&(seq_id != java_data_ptr->ref_num))
        {
            java_data_ptr = java_data_ptr->next_ptr;
        }            
       
        MMISMS_GetLenByAlpha(sms_t.dcs.alphabet_type, &split_bytes, &limit_bytes, TRUE);  
        if (PNULL == java_data_ptr)
        {
            java_data_ptr = (MMISMS_JAVA_DATA_T*)SCI_ALLOCAZ(sizeof(MMISMS_JAVA_DATA_T));
            if (PNULL == java_data_ptr)
            {
                SCI_FREE(valid_date_ptr);
                //SCI_TRACE_LOW:"MMISMS:OperateMtIndForJava java_data_ptr = PNULL"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_496_112_2_18_2_53_2_185,(uint8*)"");
                return FALSE;
            }

            //SCI_MEMSET(java_data_ptr, 0, sizeof(MMISMS_JAVA_DATA_T));       

            java_data_ptr->addr_len = MMIAPICOM_GenDispNumber(    
                                sms_t.origin_addr_t.number_type, 
                                MIN(MN_MAX_ADDR_BCD_LEN, sms_t.origin_addr_t.num_len),
                                sms_t.origin_addr_t.party_num,
                                java_data_ptr->addr_buf,
                                MMISMS_JAVA_ADDR_MAX_LEN
                            );
            java_data_ptr->dst_port = des_port;
            java_data_ptr->is_cbs = FALSE;
            if (1 == seq_len)
            {
                java_data_ptr->is_long = FALSE;
            }
            else
            {
                java_data_ptr->is_long = TRUE;
            }

            MMISMS_CorrectTimeStamp(&sms_t.time_stamp_t, MMISMS_MT_TYPE);

            java_data_ptr->isTimeStampAvailable = TRUE;
            java_data_ptr->src_port = org_port;
            java_data_ptr->timeStampHigh = 0;
            java_data_ptr->timeStampLow = (int32)MMIAPICOM_Tm2Second(
                                            sms_t.time_stamp_t.second, 
                                            sms_t.time_stamp_t.minute, 
                                            sms_t.time_stamp_t.hour, 
                                            sms_t.time_stamp_t.day, 
                                            sms_t.time_stamp_t.month, 
                                            sms_t.time_stamp_t.year + MMISMS_MT_OFFSERT_YEAR
                                        );
            java_data_ptr->total_num = seq_len;       

            if (java_data_ptr->is_long)
            {
            
                data_ptr = SCI_ALLOCAZ(split_bytes*seq_len+1);
                if(PNULL == data_ptr)
                {
                    SCI_FREE(valid_date_ptr);
                    return FALSE;
                }
                
                //SCI_MEMSET(data_ptr,0,(split_bytes*seq_len+1)); 
                SCI_MEMCPY(data_ptr,user_data_ptr->user_valid_data_t.user_valid_data_arr,user_data_ptr->user_valid_data_t.length);
                java_data_ptr->ref_num   = seq_id;   
                java_data_ptr->cur_num  = 0;  
            
            }  
            else
            {
                data_ptr = SCI_ALLOCAZ(user_data_ptr->user_valid_data_t.length+1);//CR227692 maryxiao
                if(PNULL == data_ptr)
                {
                    SCI_FREE(valid_date_ptr);
                    return FALSE;
                }
                //SCI_MEMSET(data_ptr,0,user_data_ptr->user_valid_data_t.length+1);//CR227692 maryxiao
                SCI_MEMCPY(data_ptr,user_data_ptr->user_valid_data_t.user_valid_data_arr,user_data_ptr->user_valid_data_t.length);

            }
            
            //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: sms_t.dcs.alphabet_type = %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_564_112_2_18_2_53_2_186,(uint8*)"d",sms_t.dcs.alphabet_type);

            if (MN_SMS_UCS2_ALPHABET == sms_t.dcs.alphabet_type)
            {
                java_data_ptr->type = MN_SMS_UCS2_ALPHABET;
                java_data_ptr->textData = data_ptr;
                java_data_ptr->textDataLength = user_data_ptr->user_valid_data_t.length;//maryxiao
                //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: MN_SMS_UCS2_ALPHABET user_data_ptr->user_valid_data_t.length  %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_571_112_2_18_2_53_2_187,(uint8*)"d",user_data_ptr->user_valid_data_t.length);
            }
            else
            {
                java_data_ptr->type = MN_SMS_8_BIT_ALPHBET;
                java_data_ptr->binaryData = data_ptr;
                java_data_ptr->binaryDataLength = user_data_ptr->user_valid_data_t.length;//maryxiao
                //SCI_TRACE_LOW:"MMISMS: OperateMtIndForJava: user_data_ptr->user_valid_data_t.length = %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_578_112_2_18_2_53_2_188,(uint8*)"d",user_data_ptr->user_valid_data_t.length);
            }
        }
        else
        {

            if (MN_SMS_UCS2_ALPHABET == java_data_ptr->type)
            {
                data_ptr = java_data_ptr->textData;
                java_data_ptr->textDataLength += user_data_ptr->user_valid_data_t.length;
                //SCI_TRACE_LOW:"second:MMISMS: OperateMtIndForJava: MN_SMS_UCS2_ALPHABET user_data_ptr->user_valid_data_t.length  %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_588_112_2_18_2_53_2_189,(uint8*)"d",user_data_ptr->user_valid_data_t.length);
            }
            else
            {
                data_ptr = java_data_ptr->binaryData;
                java_data_ptr->binaryDataLength += user_data_ptr->user_valid_data_t.length;
                //SCI_TRACE_LOW:"second:MMISMS: OperateMtIndForJava: user_data_ptr->user_valid_data_t.length = %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_594_112_2_18_2_53_2_190,(uint8*)"d",user_data_ptr->user_valid_data_t.length);
            }
            
            //SCI_TRACE_LOW:"seq_indx,split_bytesseq_len =%d,%d,%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_597_112_2_18_2_53_2_191,(uint8*)"ddd",seq_len,seq_indx,split_bytes,user_data_ptr->user_valid_data_t.length);
            SCI_MEMCPY(data_ptr + (seq_indx-1)*split_bytes,user_data_ptr->user_valid_data_t.user_valid_data_arr,user_data_ptr->user_valid_data_t.length);

        }

        if (java_data_ptr->is_long)
        {        
            java_data_ptr->cur_num ++;
            //SCI_TRACE_LOW:"seq_indx,split_bytesseq_len =%d,%d,%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_605_112_2_18_2_53_2_192,(uint8*)"ddd",seq_len,seq_indx,split_bytes,user_data_ptr->user_valid_data_t.length);
        }

        // add the data to global
		AddJavaSMSData(java_data_ptr);      
        //SCI_FREE(data_ptr);  无需在这个地方释放，在函数 AddJavaSMSData会进行释放的

        //SCI_FREE(java_data_ptr);     
#endif
        
        is_finished = TRUE;

        MNSMS_UpdateSmsStatusEx(dual_sys,storage, record_id, MN_SMS_FREE_SPACE);
        
        SCI_FREE(valid_date_ptr);
    }
    else
    {
        is_finished = FALSE;
    }

    MNSMS_FreeUserDataHeadSpaceEx(dual_sys, &user_head_ptr_t);
    
    return (is_finished);
}
#endif

/*****************************************************************************/
// Description : save received Vcard data over sms
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL VCARD_LONG_SMS_T* InitLongVcardsmsStruct(void)
{
    uint32 i = 0;
    VCARD_LONG_SMS_T *long_vcardsms_ptr = PNULL;
    
    //SCI_TRACE_LOW:"SMS: InitLongVcardsmsStruct()"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_641_112_2_18_2_53_2_193,(uint8*)"");

    long_vcardsms_ptr = (VCARD_LONG_SMS_T *)SCI_ALLOCAZ(sizeof(VCARD_LONG_SMS_T));

    if (PNULL != long_vcardsms_ptr)
    {
        //SCI_MEMSET(long_vcardsms_ptr, 0, sizeof(VCARD_LONG_SMS_T));

        for(i = 0;i < MMIVC_MAX_LONG_SMS_NUM; i++)
        {
            long_vcardsms_ptr->vcard_sms[i].ref_num = MMISMS_INVAILD_INDEX;
            long_vcardsms_ptr->vcard_sms[i].is_valid = FALSE;
        }
    }

    return long_vcardsms_ptr;
}

/*****************************************************************************/
//     Description : operate MT ind as SMS
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note: except class 0
/*****************************************************************************/
LOCAL BOOLEAN OperateMtIndAsSms(                                        //RETURN: TRUE, indicate the operation is finished
                                MN_DUAL_SYS_E           dual_sys,
                                APP_SMS_USER_DATA_T        *user_data_ptr,    //IN:
                                MN_SMS_STORAGE_E        storage,        //IN:
                                MN_SMS_RECORD_ID_T        record_id,        //IN:
                                MN_SMS_MT_SMS_T *mt_sms_ptr//IN:
                                )
{
    MN_SMS_USER_DATA_HEAD_T *read_head_ptr = PNULL; 
    MN_SMS_USER_DATA_HEAD_T *current_head_ptr = PNULL; 
    BOOLEAN is_finished = TRUE;
    MN_SMS_STATUS_E status = MN_SMS_MT_TO_BE_READ;  
    BOOLEAN is_replace = TRUE;
    MMISMS_SEND_T           *send_info_ptr = PNULL;
    BOOLEAN is_find_oper = FALSE;
    MN_SMS_STORAGE_E        sending_storage = MN_SMS_NO_STORED;
    MN_SMS_RECORD_ID_T        sending_record_id = 0;
#ifdef MMI_SMS_CHAT_SUPPORT
    BOOLEAN is_num_compare = FALSE;
    MMIPB_BCD_NUMBER_T mt_num = {0};
#endif

    if (PNULL == user_data_ptr || PNULL == mt_sms_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:OperateMtIndAsSms user_data_ptr mt_sms_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_687_112_2_18_2_53_3_194,(uint8*)"");

        return FALSE;
    }

#ifdef MMI_SMS_CHAT_SUPPORT
    mt_num.number_len = mt_sms_ptr->origin_addr_t.num_len;
    mt_num.npi_ton = MMIAPICOM_ChangeTypePlanToUint8(mt_sms_ptr->origin_addr_t.number_type,MN_NUM_PLAN_UNKNOW);
    SCI_MEMCPY(
               mt_num.number,
               mt_sms_ptr->origin_addr_t.party_num,
               mt_num.number_len
               ); 

    if(0 == MMIAPIPB_BcdSmartCmp(&mt_num,(MMIPB_BCD_NUMBER_T *)&g_mmisms_global.chat_number))
    {
        is_num_compare = TRUE;
    }

    if( !MMK_IsFocusWin(MMISMS_READSMS_WAIT_WIN_ID)
        && MMK_IsOpenWin(MMISMS_SHOW_CHAT_SMS_WIN_ID)
        && is_num_compare
      )
    {               
        // to modify the status from to_be_read to READ
        MMISMS_SetOperStatus(MMISMS_READ_CHAT_NUM_SMS);

        //MNSMS_ReadSmsEx(dual_sys, storage, record_id);
        MNSMS_UpdateSmsStatusEx(dual_sys, storage, record_id, MN_SMS_MT_READED);
        g_mmisms_global.update_status_num++;

        g_mmisms_global.is_need_update_order = TRUE;

        status = MN_SMS_MT_READED;
    }
#endif
   
    //SCI_TRACE_LOW:"MMISMS:OperateMtIndAsSms storage = %d, record_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_724_112_2_18_2_53_3_195,(uint8*)"dd", storage, record_id);

    send_info_ptr = MMISMS_GetSendInfoFromSendLIst(MMISMS_SENDSTATUS_SAVING);
    if (PNULL != send_info_ptr)
    {
        if (MN_SMS_NO_STORED != send_info_ptr->cur_send_data.cur_send_storage 
            && storage == send_info_ptr->cur_send_data.cur_send_storage
            && record_id == send_info_ptr->cur_send_data.cur_send_record_id)
        {
            is_find_oper = TRUE;
            MMISMS_GetStorageAndRecordidForSendSms(send_info_ptr, &sending_storage, &sending_record_id);
            if (MN_SMS_NO_STORED != storage)
            {
                send_info_ptr->cur_send_data.cur_send_record_id = sending_record_id;
                send_info_ptr->cur_send_data.cur_send_storage = sending_storage;
            }
        }
    }
    send_info_ptr = MMISMS_GetSendInfoFromSendLIst(MMISMS_SENDSTATUS_SENDING);
    if (PNULL != send_info_ptr && !is_find_oper)
    {
        MMISMS_UpdateCurSendRecordId(send_info_ptr, storage, record_id, FALSE);
    }

    //SCI_TRACE_LOW:" MMISMS:OperateMtIndAsSms storage = %d, record_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_748_112_2_18_2_53_3_196,(uint8*)"dd", storage, record_id );
    if (MMISMS_IsOrderOk())  //排序完成
    {
        //当前状态为删除所有收件箱短消息，则将此时收到的短消息放入到未处理链表中，
        //全部删除完毕时将未处理链表中的短消息放入相应的链表中
        if (MMISMS_GetDelAllFlag()
            || MMISMS_GetMoveAllFlag()
#ifdef MMI_SMS_CHAT_SUPPORT 
            || ((MMISMS_READ_CHAT_NUM_SMS == MMISMS_GetCurrentOperStatus())
                && MMK_IsFocusWin(MMISMS_READSMS_WAIT_WIN_ID)
                && MMK_IsOpenWin(MMISMS_SHOW_CHAT_SMS_WIN_ID)
               )
#endif
#ifdef IKEYBACKUP_SUPPORT
            || g_sms_is_backup
#endif
#ifdef MMI_PDA_SUPPORT
            ||(MMISMS_GetCurrentOperStatus() ==MMISMS_MATCH_DATA_DELETE)
#endif
            )
        {
            is_replace = InsertNewMtToOrder(
                dual_sys,
                storage, 
                record_id,
                MN_SMS_MT_TO_BE_READ,
                user_data_ptr,
                mt_sms_ptr,
                FALSE,
                TRUE
                );           
        }
        else
        {
            // insert this new message to order list
            is_replace = InsertNewMtToOrder(
                dual_sys,
                storage, 
                record_id,
                MN_SMS_MT_TO_BE_READ,
                user_data_ptr,
                mt_sms_ptr,
                FALSE,
                FALSE
                );
        }       

        current_head_ptr = &(user_data_ptr->user_data_head_t);
        read_head_ptr     = &(g_mmisms_global.read_msg.head_data);
        
        //SCI_TRACE_LOW:"MMISMS: OperateMtIndAsSms head length = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_798_112_2_18_2_53_3_197,(uint8*)"d", read_head_ptr->length);
        //SCI_TRACE_LOW:"MMISMS: OperateMtIndAsSms ref = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_799_112_2_18_2_53_3_198,(uint8*)"d", MMISMS_GetRefNumFromUserHead(read_head_ptr));
        
        if (((MMISMS_NO_OPER == MMISMS_GetCurrentOperStatus())
             && (MMISMS_SHOWMSG_WIN_ID == MMK_GetFocusWinId())
             && (MMISMS_READ_MTSMS == MMISMS_GetMtType())
             && (read_head_ptr->length > 0)
             && (current_head_ptr->length > 0)
             && (MMISMS_GetRefNumFromUserHead(current_head_ptr) == MMISMS_GetRefNumFromUserHead(read_head_ptr))
             && !MMISMS_IsR8Language(g_mmisms_global.read_msg.language))
             ||((MMISMS_NO_OPER == MMISMS_GetCurrentOperStatus()) 
             && (MMISMS_SHOWMSG_WIN_ID == MMK_GetFocusWinId()) 
             && (MMISMS_READ_MTSMS == MMISMS_GetMtType())  
             && (read_head_ptr->length > MMISMS_R8_MAX_NORMAL_HEAD_LEN) 
             && (current_head_ptr->length > MMISMS_R8_MAX_NORMAL_HEAD_LEN)
             && (MMISMS_GetRefNumFromUserHead(current_head_ptr) == MMISMS_GetRefNumFromUserHead(read_head_ptr)
             && MMISMS_IsR8Language(g_mmisms_global.read_msg.language))
             )) 
        {
            //SCI_TRACE_LOW:"MMISMS: OperateMtIndAsSms need not update window"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_817_112_2_18_2_53_3_199,(uint8*)"");
        }
        else
        {
#ifdef MMI_SMS_CHAT_SUPPORT        
            if( !MMK_IsFocusWin(MMISMS_READSMS_WAIT_WIN_ID)
                && MMK_IsOpenWin(MMISMS_SHOW_CHAT_SMS_WIN_ID)
                && is_num_compare
              )
            {
			    SCI_MEMSET(&g_mmisms_global.read_msg,0x0,sizeof(MMISMS_READ_MSG_T));

                g_mmisms_global.read_msg.mo_mt_type = MMISMS_MT_HAVE_READ;

            	MMISMS_ChatSetMTGlobalReadMsg(
            								  dual_sys,
            								  user_data_ptr,    //IN:
            								  storage,        //IN:
            								  record_id,        //IN:
            								  mt_sms_ptr->dcs.alphabet_type,
            								  &mt_sms_ptr->time_stamp_t,
            								  &mt_sms_ptr->origin_addr_t
            								 ); 

    
                MMIDEFAULT_TurnOnBackLight();
                // save the flag
                MMISMS_SetNewSMSRingFlag(TRUE);
                MMISMS_SetSimIdOfNewMsg(dual_sys);
                MMISMS_PlayNewSMSRing(dual_sys);

                MMK_SendMsg(
                            MMISMS_SHOW_CHAT_SMS_WIN_ID,
                            MSG_SMS_FINISH_READ,
                            (ADD_DATA)g_mmisms_global.chat_oper_order_id
                            ); 
            }
            else
#endif                
            {
                MMIDEFAULT_TurnOnBackLight();
                MMISMS_ShowNewMsgPrompt(dual_sys, TRUE);
            }
        }
    }
    else //排序未完成
    {
        is_replace = InsertNewMtToOrder(
            dual_sys,
            storage, 
            record_id,
            MN_SMS_MT_TO_BE_READ,
            user_data_ptr,
            mt_sms_ptr,
            FALSE,
            TRUE
            );
    }

    if (is_replace)
    {
        MMISMS_HandleOrderChangeCallBack();

        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SMS_UPDATED,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
    }
    
    return (is_finished);
}

#ifdef SXH_APPLE_SUPPORT
//	extern uint32 g_apple_ps_mode;
void AppleOperateMtIndAsSms(										//RETURN: TRUE, indicate the operation is finished
                                                        MN_DUAL_SYS_E			 dual_sys,
                                                        APP_SMS_USER_DATA_T 	   *user_data_ptr,	  //IN:
                                                        MN_SMS_STORAGE_E		storage,		//IN:
                                                        MN_SMS_RECORD_ID_T		  record_id,		//IN:
                                                        MN_SMS_MT_SMS_T *mt_sms_ptr//IN:
                                                        )
{
    if (PNULL == user_data_ptr || PNULL == mt_sms_ptr)
    {
        return;
    }

    //SCI_TRACE_LOW:" MMISMS:OperateMtIndAsSms storage = %d, record_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_902_112_2_18_2_53_3_200,(uint8*)"dd",storage, record_id );

    if (MMISMS_IsOrderOk())  //排序完成
    {
        //当前状态为删除所有收件箱短消息，则将此时收到的短消息放入到未处理链表中，
        //全部删除完毕时将未处理链表中的短消息放入相应的链表中
        if (MMISMS_GetDelAllFlag()) 
        {
            InsertNewMtToOrder(
                dual_sys,
                storage, 
                record_id,
                MN_SMS_MT_TO_BE_READ,
                user_data_ptr,
                mt_sms_ptr,
                FALSE,
                TRUE
                );	   
        }
        else
        {
            // insert this new message to order list
            InsertNewMtToOrder(
                dual_sys,
                storage, 
                record_id,
                MN_SMS_MT_TO_BE_READ,
                user_data_ptr,
                mt_sms_ptr,
                FALSE,
                FALSE
                );
        }
    }
    else //排序未完成
    {
        InsertNewMtToOrder(
            dual_sys,
            storage, 
            record_id,
            MN_SMS_MT_TO_BE_READ,
            user_data_ptr,
            mt_sms_ptr,
            FALSE,
            TRUE
            );
    }
}
#endif
/*****************************************************************************/
//     Description : to insert new message to order list
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN InsertNewMtToOrder(                                                //RETURN:
                              MN_DUAL_SYS_E                dual_sys,            //IN:
                              MN_SMS_STORAGE_E            storage,            //IN:
                              MN_SMS_RECORD_ID_T            record_id,            //IN:
                              MN_SMS_STATUS_E                status,                //IN:
                              APP_SMS_USER_DATA_T         *user_data_ptr,     //IN:
                              MN_SMS_MT_SMS_T *mt_sms_ptr,//IN:
                              BOOLEAN                     is_mms_push,        //IN:
                              BOOLEAN                    is_unhandle_order
                              )
{
    uint8 is_ucs2 = FALSE;
    uint32 sc_time = 0;
    MMISMS_STATE_T insert_order_info = {0};
    uint8 mmisms_status = 0;    //inserted SMS status
    uint16 num_len = 0;
    MN_SMS_USER_DATA_HEAD_T *data_head_ptr =  &(user_data_ptr->user_data_head_t);
    uint8 content_len = 0;
    uint8 content [MMIMESSAGE_DIAPLAY_LEN * sizeof(wchar) + 1] = {0};
    MN_SMS_ALPHABET_TYPE_E alpha = MN_SMS_DEFAULT_ALPHABET;
    MN_SMS_TIME_STAMP_T *sc_time_ptr = PNULL;
    MN_CALLED_NUMBER_T *orginal_addr_ptr = PNULL;
    MN_SMS_PID_E pid_e = MN_SMS_PID_DEFAULT_TYPE;
    BOOLEAN is_replace = FALSE;
    MN_SMS_USER_HEAD_T    user_head_ptr_t = {0};
    MMISMS_R8_LANGUAGE_E language = MMISMS_R8_LANGUAGE_NONE;
    uint16 content_r8 [MMIMESSAGE_DIAPLAY_LEN * sizeof(wchar) + 1] = {0};
    BOOLEAN is_lock_shift = FALSE;
    BOOLEAN is_single_shift = FALSE;
    
    if (PNULL == mt_sms_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:InsertNewMtToOrder mt_sms_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_987_112_2_18_2_53_3_201,(uint8*)"");
        return FALSE;
    }

    alpha = mt_sms_ptr->dcs.alphabet_type;
    sc_time_ptr = &mt_sms_ptr->time_stamp_t;
    orginal_addr_ptr = &mt_sms_ptr->origin_addr_t;
    pid_e = mt_sms_ptr->pid_e;

    //SCI_TRACE_LOW:"MMISMS: InsertNewMtToOrder(%d,%d, %d, %d), pid_e=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_996_112_2_18_2_53_3_202,(uint8*)"ddddd", dual_sys, storage, record_id, status, pid_e);

    is_ucs2 = (MN_SMS_UCS2_ALPHABET == alpha) ? TRUE : FALSE;

    MMISMS_CorrectTimeStamp(sc_time_ptr, MMISMS_MT_TYPE);
    
    // translate the time to second
    sc_time = MMIAPICOM_Tm2Second( 
        sc_time_ptr->second,
        sc_time_ptr->minute,
        sc_time_ptr->hour,
        sc_time_ptr->day,
        sc_time_ptr->month,
        (sc_time_ptr->year) + MMISMS_MT_OFFSERT_YEAR
        );
    
    // get display content
    //if (PNULL != orginal_addr_ptr)
    {
        num_len = MIN(orginal_addr_ptr->num_len, MMISMS_BCD_NUMBER_MAX_LEN);
        
        insert_order_info.number.bcd_number.npi_ton =  MMIAPICOM_ChangeTypePlanToUint8(
            (MN_NUMBER_TYPE_E)orginal_addr_ptr->number_type,
            (MN_NUMBER_PLAN_E)orginal_addr_ptr->number_plan
            );
/*        
        if (MN_NUM_TYPE_ALPHANUMERIC == orginal_addr_ptr->number_type)
        {
            num_len = MMIAPICOM_Default2Ascii(
                orginal_addr_ptr->party_num, 
                insert_order_info.number.bcd_number.number, 
                num_len
                );
        }

        else
*/
        {
            SCI_MEMCPY(
                insert_order_info.number.bcd_number.number,
                orginal_addr_ptr->party_num,
                num_len
                );
        }             
        insert_order_info.number.bcd_number.number_len = num_len;
    }
    
    if (MN_SMS_MT_SR_TO_BE_READ != status) //not status report
    {
        content_len = MIN(MMIMESSAGE_DIAPLAY_LEN * sizeof(wchar), user_data_ptr->user_valid_data_t.length);    
        //SCI_TRACE_LOW:"MMISMSR8:InsertNewMtToOrder content_len is %d,alpha is %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1045_112_2_18_2_53_3_203,(uint8*)"dd",content_len,alpha);

        //add for display sms content
        if (MN_SMS_DEFAULT_ALPHABET == alpha)
        {
            MMISMS_DecodeUserDataHead(&(user_data_ptr->user_data_head_t), &user_head_ptr_t);
            language = MMISMS_JudgeR8Language(&user_head_ptr_t);
            MMISMS_DecodeR8Header(&user_head_ptr_t,&is_lock_shift,&is_single_shift);

            //SCI_TRACE_LOW:"MMISMSR8:InsertNewMtToOrder language is %d,is_lock_shift is %d,is_single_shift is %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1055_112_2_18_2_53_3_204,(uint8*)"ddd",language,is_lock_shift,is_single_shift);

            if(MMISMS_IsR8Language(language))
            {
                content_len = MMISMS_Default2NationalR8(
                        user_data_ptr->user_valid_data_t.user_valid_data_arr,
                        content_r8,
                        content_len,
                        language,
                        is_lock_shift,
                        is_single_shift
                        );

                //SCI_TRACE_LOW:"MMISMSR8:InsertNewMtToOrder content_len is %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1068_112_2_18_2_53_3_205,(uint8*)"d",content_len);
            }
            else
            {
/*
                content_len = MMIAPICOM_Default2Ascii(
                    user_data_ptr->user_valid_data_t.user_valid_data_arr,
                    content,
                    content_len
                    );
*/
                SCI_MEMCPY(
                    content,
                    user_data_ptr->user_valid_data_t.user_valid_data_arr,
                    content_len
                    ); 

            }
            MMISMS_FreeUserDataHeadSpace(&user_head_ptr_t);

        }
        else
        {
            SCI_MEMCPY(
                content,
                user_data_ptr->user_valid_data_t.user_valid_data_arr,
                content_len
                );
        }         
    }
    
    if(!MMISMS_IsR8Language(language))
    {
        MMISMS_ConvertSmsContentToDisplayContent(&insert_order_info.display_content,
                                                                            content,
                                                                            content_len,
                                                                            alpha);
    }
    else
    {
        insert_order_info.display_content.content_len = MIN(MMIMESSAGE_DIAPLAY_LEN, content_len);

        MMI_WSTRNCPY(insert_order_info.display_content.content,
                                MMIMESSAGE_DIAPLAY_LEN,
                                content_r8,
                                content_len,
                                insert_order_info.display_content.content_len);      
    }
    
    //SCI_TRACE_LOW:"MMISMS: InsertNewMtToOrder head_len = %d, ref_num = 0x%X, seq_num = 0x%X"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1119_112_2_18_2_53_3_206,(uint8*)"ddd", data_head_ptr->length,data_head_ptr->user_header_arr[MMISMS_HEAD_REF_NUM_POS],data_head_ptr->user_header_arr[MMISMS_HEAD_8BIT_SEQ_NUM_POS]);
    
    // get the related sf_status
    if (is_mms_push)
    {
        mmisms_status = MMISMS_MT_NOT_DOWNLOAD;
        insert_order_info.flag.msg_type = MMISMS_TYPE_WAP_PUSH;
    }
    else
    {
        mmisms_status = MMISMS_TranslateMNStatus2SFStatus(status, MMISMS_MT_TYPE, TRUE);
        insert_order_info.flag.msg_type = MMISMS_TYPE_SMS;
    }
    
    insert_order_info.flag.storage = storage;
    insert_order_info.flag.mo_mt_type = mmisms_status;
    insert_order_info.flag.is_ucs2 = is_ucs2;
    insert_order_info.record_id = record_id;
    insert_order_info.time = sc_time;
    insert_order_info.flag.dual_sys = dual_sys;
    if (g_mmisms_global.is_need_update_order)
    {
        insert_order_info.flag.is_need_update = TRUE;
    }

    if (MN_SMS_MT_SR_TO_BE_READ == status) //not status report
    {
        insert_order_info.flag.is_concatenate_sms = FALSE;
    }
    else if ((PNULL != data_head_ptr) &&
             (((data_head_ptr->length != 0) && !MMISMS_IsR8Language(language)) ||
             ((data_head_ptr->length > MMISMS_R8_MAX_NORMAL_HEAD_LEN) && MMISMS_IsR8Language(language))))/*lint !e774*/ 
    {
#ifdef MMI_SMS_CONCATENATED_SETTING_SUPPORT
        if (MMISMS_GetConcatenatedSMS())
#endif
        {
            insert_order_info.flag.is_concatenate_sms = TRUE;
        }

        if(MN_SMS_CONCAT_8_BIT_REF_NUM == data_head_ptr->user_header_arr[MMISMS_HEAD_IEI_POS])
        {
            insert_order_info.longsms_max_num = data_head_ptr->user_header_arr[MMISMS_HEAD_8BIT_MAX_NUM_POS];
            insert_order_info.longsms_seq_num = data_head_ptr->user_header_arr[MMISMS_HEAD_8BIT_SEQ_NUM_POS];
            insert_order_info.head_ref = data_head_ptr->user_header_arr[MMISMS_HEAD_REF_NUM_POS];
        }
        else if(MN_SMS_CONCAT_16_BIT_REF_NUM == data_head_ptr->user_header_arr[MMISMS_HEAD_IEI_POS])
        {
            insert_order_info.longsms_max_num = data_head_ptr->user_header_arr[MMISMS_HEAD_16BIT_MAX_NUM_POS];
            insert_order_info.longsms_seq_num = data_head_ptr->user_header_arr[MMISMS_HEAD_16BIT_SEQ_NUM_POS];
            insert_order_info.head_ref = (data_head_ptr->user_header_arr[MMISMS_HEAD_REF_NUM_POS] << 8) 
                                                    | (data_head_ptr->user_header_arr[MMISMS_HEAD_REF_NUM_POS+1]) ;
        }
    }

#ifdef MMI_STATUSBAR_SCROLLMSG_SUPPORT
    if (!is_unhandle_order)
    {
        BOOLEAN is_same_sms = FALSE;
        BOOLEAN is_need_swap = FALSE;
        uint16 cur_index = 0;
        int32 i = 0;
        int32 start_pos = 0;
        BOOLEAN is_need_assemble_prompt =  TRUE;
        s_sms_statusbar_scrollmsg.is_concatenate_sms = insert_order_info.flag.is_concatenate_sms;
        s_sms_statusbar_scrollmsg.msg_type = MMISMS_READ_MTSMS;

        if (s_sms_statusbar_scrollmsg.is_concatenate_sms)
        {
            if (s_sms_statusbar_scrollmsg.dual_sys == dual_sys
                && s_sms_statusbar_scrollmsg.longsms_max_num == insert_order_info.longsms_max_num
                && s_sms_statusbar_scrollmsg.head_ref == insert_order_info.head_ref
                && ((s_sms_statusbar_scrollmsg.longsms_seq_num - 1)/MMISMS_SPLIT_MAX_NUM == (insert_order_info.longsms_seq_num - 1)/MMISMS_SPLIT_MAX_NUM)
                )
            {
                cur_index = (insert_order_info.longsms_seq_num - 1)%MMISMS_SPLIT_MAX_NUM;

                if (s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index] > 0
                    && s_sms_statusbar_scrollmsg.read_msg.read_content.length >= s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index]
                    && s_sms_statusbar_scrollmsg.read_msg.read_content.sms_num > 0
                    && s_sms_statusbar_scrollmsg.read_msg.sms_num > 0)
                {
                    for (i = 0; i < cur_index; i++)
                    {
                        start_pos += s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[i];
                    }

                    if ((cur_index == s_sms_statusbar_scrollmsg.read_msg.sms_num) && (cur_index + 1) == s_sms_statusbar_scrollmsg.longsms_max_num )
                    {
                        is_need_swap = FALSE;
                    }
                    else
                    {
                        is_need_swap = TRUE;
                    }

                    if (s_sms_statusbar_scrollmsg.read_msg.read_content.length - start_pos >= 0)
                    {
                        SCI_MEMCPY(s_sms_statusbar_scrollmsg.read_msg.read_content.data + start_pos,
                                            s_sms_statusbar_scrollmsg.read_msg.read_content.data + start_pos + s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index],
                                            s_sms_statusbar_scrollmsg.read_msg.read_content.length - start_pos - s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index]);

                        SCI_MEMSET(s_sms_statusbar_scrollmsg.read_msg.read_content.data + (s_sms_statusbar_scrollmsg.read_msg.read_content.length - s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index]),
                                            0x00,
                                            s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index]);
                        s_sms_statusbar_scrollmsg.read_msg.read_content.length -=  s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index];
                        s_sms_statusbar_scrollmsg.read_msg.read_content.sms_num --;
                    }

                    if (MMISMS_SPLIT_MAX_NUM - 1 == cur_index)
                    {
                        s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index] = 0;
                        s_sms_statusbar_scrollmsg.read_msg.read_content.alphabet_ori[cur_index] = MN_SMS_DEFAULT_ALPHABET;
                    }
                    else
                    {
                        for (i = cur_index; i < MMISMS_SPLIT_MAX_NUM - 1; i++)
                        {
                            s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[i] = s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[i + 1];
                            s_sms_statusbar_scrollmsg.read_msg.read_content.alphabet_ori[i] = s_sms_statusbar_scrollmsg.read_msg.read_content.alphabet_ori[i + 1];
                        }
                    }
                }

                is_same_sms = TRUE;
            }
        }

        s_sms_statusbar_scrollmsg.dual_sys = dual_sys;
        s_sms_statusbar_scrollmsg.storage = storage;
        s_sms_statusbar_scrollmsg.record_id = insert_order_info.record_id;
        s_sms_statusbar_scrollmsg.longsms_seq_num = insert_order_info.longsms_seq_num;
        s_sms_statusbar_scrollmsg.longsms_max_num = insert_order_info.longsms_max_num;
        s_sms_statusbar_scrollmsg.head_ref = insert_order_info.head_ref;

        if (!is_same_sms)
        {
            SCI_MEMSET(&s_sms_statusbar_scrollmsg.read_msg, 0x00, sizeof(s_sms_statusbar_scrollmsg.read_msg));
        }

        for(i = 0; i < MMISMS_SPLIT_MAX_NUM; i++)
        {
            if(s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[i] != 0)
            {
                is_need_assemble_prompt = FALSE;
                break;
            }
        }
        
        MMISMS_AssembleSmsToGlobal(s_sms_statusbar_scrollmsg.dual_sys,
                                                        s_sms_statusbar_scrollmsg.msg_type,
                                                        user_data_ptr,
                                                        orginal_addr_ptr,
                                                        alpha,
                                                        &s_sms_statusbar_scrollmsg.read_msg,
                                                        is_need_assemble_prompt,
                                                        is_need_assemble_prompt
                                                        );

        if (is_need_swap
            && s_sms_statusbar_scrollmsg.read_msg.read_content.sms_num > 0
            && s_sms_statusbar_scrollmsg.read_msg.read_content.length >= s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index])
        {
            MMISMS_CONTENT_T read_content = {0};

            SCI_MEMCPY(&read_content, &s_sms_statusbar_scrollmsg.read_msg.read_content, sizeof(s_sms_statusbar_scrollmsg.read_msg.read_content));
            start_pos = 0;
            for (i = 0; i < cur_index; i++)
            {
                start_pos += s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[i];
            }

            SCI_MEMCPY(s_sms_statusbar_scrollmsg.read_msg.read_content.data + start_pos,
                                read_content.data + (s_sms_statusbar_scrollmsg.read_msg.read_content.length - read_content.split_sms_length[read_content.sms_num - 1]),
                                read_content.split_sms_length[read_content.sms_num - 1]);

            s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[cur_index] = s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[read_content.sms_num - 1];
            s_sms_statusbar_scrollmsg.read_msg.read_content.alphabet_ori[cur_index] = s_sms_statusbar_scrollmsg.read_msg.read_content.alphabet_ori[read_content.sms_num - 1];

            SCI_MEMCPY(s_sms_statusbar_scrollmsg.read_msg.read_content.data + start_pos + read_content.split_sms_length[read_content.sms_num - 1],
                                read_content.data + start_pos,
                                (s_sms_statusbar_scrollmsg.read_msg.read_content.length - read_content.split_sms_length[read_content.sms_num - 1] - start_pos));

            for (i = cur_index + 1; i < MIN(s_sms_statusbar_scrollmsg.longsms_max_num,MMISMS_SPLIT_MAX_NUM); i++)
            {
                s_sms_statusbar_scrollmsg.read_msg.read_content.split_sms_length[i] = read_content.split_sms_length[i - 1];
                s_sms_statusbar_scrollmsg.read_msg.read_content.alphabet_ori[i] = read_content.alphabet_ori[i - 1];
            }
        }
    }
#endif

    is_replace = MMISMS_ReplaceSms(pid_e, &insert_order_info);

    if (!is_replace)
    {
        if (is_unhandle_order)
        {
            MMISMS_InsertSmsToUnhandleOrder(&insert_order_info);
        }
        else
        {
            MMISMS_InsertSmsToOrder(&insert_order_info);
            MMISMS_SetNeedSort(TRUE);
#ifdef SXH_APPLE_SUPPORT
	if(Apple_Get_ps_mode() != 2)
	{    
		MMISMS_HandleOrderChangeCallBack();
	}
#else
	MMISMS_HandleOrderChangeCallBack();
#endif
        }
    }

#ifdef _ATEST_
#ifdef WIN32
    ATESTOTA_ProcessPressKeySerialByReceiveOta();  // added by Mingxuan.Zhu
#endif//WIN32
#endif//_ATEST_

    return is_replace;
}

/*****************************************************************************/
//     Description : operate MT ind as mms
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL BOOLEAN OperateMtIndAsMmsPush(                                    //RETURN: TRUE, indicate the operation is finished
                                    MN_DUAL_SYS_E       dual_sys,
                                    APP_SMS_USER_DATA_T    *user_data_ptr,    //IN:
                                    MN_SMS_STORAGE_E    storage,        //IN:
                                    MN_SMS_RECORD_ID_T    record_id,        //IN:
                                    MN_SMS_MT_SMS_T *mt_sms_ptr//IN:
                                    )
{
    BOOLEAN is_finished = TRUE;
    BOOLEAN is_new_mms    = FALSE;
    ERR_MNSMS_CODE_E err_code = ERR_MNSMS_NONE;
    APP_MMS_USER_DATA_T mms_user_data_t = {0};
    uint8 tele_num[MMISMS_PBNAME_MAX_LEN + 1] = {0};
    uint16 num_len = 0;
    uint8 sc_tele_num[MMISMS_PBNAME_MAX_LEN + 1] = {0};
    uint16 sc_num_len = 0;
    MN_CALLED_NUMBER_T *org_addr_ptr = PNULL;
    MN_CALLED_NUMBER_T *sc_addr_ptr = PNULL;
    
    if (PNULL == user_data_ptr || PNULL == mt_sms_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:OperateMtIndAsMmsPush user_data_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1368_112_2_18_2_53_4_207,(uint8*)"");

        return FALSE;
    }

    org_addr_ptr = &mt_sms_ptr->origin_addr_t;
    sc_addr_ptr = &mt_sms_ptr->sc_addr_t;

    // 判断是否为mms
    err_code = MNSMS_IsSupportMMSEx(dual_sys,
        user_data_ptr->user_data_head_t.length,
        user_data_ptr->user_data_head_t.user_header_arr,
        &is_new_mms
        );
    //SCI_TRACE_LOW:"MMISMS: OperateMtIndAsMmsPush err_code=%d, is_new_mms=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1382_112_2_18_2_53_4_208,(uint8*)"dd", err_code, is_new_mms);
    
    if ( ( ERR_MNSMS_NONE == err_code) && (is_new_mms) )
    {    
        if (MMISMS_GetDelAllFlag()
            || MMISMS_GetMoveAllFlag()
            || !MMISMS_IsMMSOrderReady()
            || MMIAPIUDISK_UdiskIsRun())
        {
            //插入到未处理连表中 //later
            InsertNewMtToOrder(
                dual_sys,
                storage, 
                record_id,
                MN_SMS_MT_TO_BE_READ,
                user_data_ptr,
                mt_sms_ptr,
                TRUE,
                TRUE
                );             

            is_finished = TRUE;
        }
        else
        {
            mms_user_data_t.user_data_head_t.length = MIN(user_data_ptr->user_data_head_t.length, 40);
            SCI_MEMCPY(
                mms_user_data_t.user_data_head_t.user_header_arr,
                user_data_ptr->user_data_head_t.user_header_arr,
                mms_user_data_t.user_data_head_t.length
                );
            
            //SCI_TRACE_LOW:"MMISMS: OperateMtIndAsMmsPush mms head length = %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1414_112_2_18_2_53_4_209,(uint8*)"d", mms_user_data_t.user_data_head_t.length);
            
            mms_user_data_t.user_valid_data_t = user_data_ptr->user_valid_data_t;
            
            if (org_addr_ptr->num_len > 0)//get number
            {
                num_len = MMIAPICOM_GenNetDispNumber((MN_NUMBER_TYPE_E)(org_addr_ptr->number_type),
                                                                    (uint8)MIN((MMISMS_PBNUM_MAX_LEN >> 1),
                                                                    org_addr_ptr->num_len),
                                                                    org_addr_ptr->party_num,
                                                                    tele_num,
                                                                    (uint8)(MMISMS_PBNUM_MAX_LEN + 2)
                                                                    );
            }

            if (sc_addr_ptr->num_len > 0)//get number
            {
                sc_num_len = MMIAPICOM_GenNetDispNumber((MN_NUMBER_TYPE_E)(sc_addr_ptr->number_type),
                                                                    (uint8)MIN((MMISMS_PBNUM_MAX_LEN >> 1),
                                                                    sc_addr_ptr->num_len),
                                                                    sc_addr_ptr->party_num,
                                                                    sc_tele_num,
                                                                    (uint8)(MMISMS_PBNUM_MAX_LEN + 2)
                                                                    );
            }

            //call the mms interface
            MMIAPIMMS_DecodePush(dual_sys, &mms_user_data_t,tele_num,num_len,sc_tele_num,sc_num_len);
            
            // call MN API to update SMS status.
            if(MN_SMS_NO_STORED != storage)
            {
                MNSMS_UpdateSmsStatusEx(dual_sys,storage, record_id, MN_SMS_FREE_SPACE);
            }
            is_finished = TRUE;
        }
    }
    else
    {
        is_finished = FALSE;
    }
    
    return (is_finished);
}

#ifdef MMI_STATUSBAR_SCROLLMSG_SUPPORT
/*****************************************************************************/
//  Description : get read msg content
//  Global resource dependence : 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
PUBLIC void MMISMS_GetReadMsgContent(
                                    MMISMS_READMSG_CONTENT_T *read_content_ptr      //OUT
                                    )
{
    MMI_STRING_T num_name = {0};
//    uint16         nIndex = 0;
    uint16         convert_msg_len = 0;
    uint8           time_len = 0;
//    wchar          time_string[GUIRICHTEXT_TEXT_MAX_LEN+1] = {0};
    char           time[GUIRICHTEXT_TEXT_MAX_LEN+1] = {0};
    uint8          date[MMISET_DATE_STR_LEN] = {0};
    uint8          time_str[MMISET_TIME_STR_12HOURS_LEN + 1] = {0};
//    GUIRICHTEXT_ITEM_T  item_data = {0};/*lint !e64*/
    wchar    *string = PNULL;
//    wchar content_str[GUIRICHTEXT_TEXT_MAX_LEN+1] = {0};
//    MMI_CTRL_ID_T ctrl_id = MMISMS_SHOW_MSG_RICHTEXT_CTRL_ID;
    uint16 content_len = 0;    
    uint16 year = 0;
    //param
    MN_SMS_ALPHABET_TYPE_E    alphabet = 0;            //IN:
    uint16                    msg_len = 0;            //IN:
    uint8                     *msg_data_ptr = PNULL;        //IN:
    MN_SMS_TIME_STAMP_T        *sc_time_ptr = PNULL;        //IN:
    MN_CALLED_NUMBER_T        *orginal_addr_ptr = PNULL;    //IN:
    MMISMS_READ_MSG_T        *read_msg_ptr = &s_sms_statusbar_scrollmsg.read_msg;
    wchar *content_r8_ptr = PNULL;

    if (PNULL == read_content_ptr)
    {
        return;
    }

    content_r8_ptr = SCI_ALLOCAZ( ( MMISMS_R8_MESSAGE_LEN + 1 )*sizeof(wchar) );
    
    if(PNULL == content_r8_ptr)
    {
        return;
    }

    //param
    alphabet =    read_msg_ptr->read_content.alphabet;
    msg_len =    read_msg_ptr->read_content.length;
    msg_data_ptr =    read_msg_ptr->read_content.data;
    sc_time_ptr = &(read_msg_ptr->time_stamp);
    orginal_addr_ptr =    &(read_msg_ptr->address);

    //SCI_MEMSET(content_r8_ptr,0x0,(sizeof(wchar) * ( MMISMS_R8_MESSAGE_LEN + 1 )));

    //Add item sender name or No.
    num_name.wstr_ptr = SCI_ALLOCAZ((MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
    if(PNULL == num_name.wstr_ptr)
    {
        SCI_FREE(content_r8_ptr);
        content_r8_ptr = PNULL;

        return;
    }
    //SCI_MEMSET((uint8 *)num_name.wstr_ptr, 0, (MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
    GetTransmitterDispString(
        alphabet,
        orginal_addr_ptr,
        &num_name
        );
    
    if (PNULL != read_content_ptr->name_string.wstr_ptr)
    {
        read_content_ptr->name_string.wstr_len = MMIAPICOM_CopyStr2Buf(read_content_ptr->name_string.wstr_ptr, 
            &num_name, read_content_ptr->name_string.wstr_len);
    }

    if(MMISMS_READ_MTSMS == read_msg_ptr->read_type)
    {
        year = sc_time_ptr->year + MMISMS_MT_OFFSERT_YEAR;
    }

    else
    {
        year = sc_time_ptr->year + MMISMS_OFFSERT_YEAR;
    }
    MMIAPISET_FormatDateStrByDateStyle(year,sc_time_ptr->month, sc_time_ptr->day,
        '/',date,MMISET_DATE_STR_LEN);
    MMIAPISET_FormatTimeStrByTime(sc_time_ptr->hour, sc_time_ptr->minute,time_str,MMISET_TIME_STR_12HOURS_LEN + 1);
    //Add item SMS time
    sprintf(time, "%s %s",date,time_str);

    time_len = strlen( time );

    if (PNULL != read_content_ptr->time_string.wstr_ptr)
    {
        MMI_STRNTOWSTR( 
                read_content_ptr->time_string.wstr_ptr,
                read_content_ptr->time_string.wstr_len,
                (const uint8 *)time,
                read_content_ptr->time_string.wstr_len,
                read_content_ptr->time_string.wstr_len
                );
    }

    //Add item SMS content
    convert_msg_len = msg_len;

    if(MMISMS_IsR8Language(read_msg_ptr->language))
    {
        content_len = MMISMS_Default2NationalR8(
                msg_data_ptr,
                content_r8_ptr,
                convert_msg_len,
                read_msg_ptr->language,
                read_msg_ptr->is_lock_shift,
                read_msg_ptr->is_single_shift
                );
    }

    string = SCI_ALLOCAZ( ( MMISMS_R8_MESSAGE_LEN + 1 )*sizeof(wchar) );
    if(PNULL == string)
    {
        SCI_FREE(content_r8_ptr);
        content_r8_ptr = PNULL;

        SCI_FREE(num_name.wstr_ptr);
        num_name.wstr_ptr = PNULL;
        return;
    }

    //SCI_MEMSET(string,0,( ( MMISMS_R8_MESSAGE_LEN + 1 )*sizeof(wchar)));

    //SCI_TRACE_LOW:"MMISMS_R8:AddSMSContentItemtoRichText,language is %d,alphabet is %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1591_112_2_18_2_53_4_210,(uint8*)"dd",read_msg_ptr->language,alphabet);
    
    if(MMISMS_IsR8Language(read_msg_ptr->language) &&  (alphabet != MN_SMS_UCS2_ALPHABET))
    {
        //SCI_TRACE_LOW:"MMISMS_R8:AddSMSContentItemtoRichText,convert_msg_len is %d,content_len is %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1596_112_2_18_2_53_4_211,(uint8*)"dd",convert_msg_len,content_len);

        MMI_WSTRNCPY(
                string,
                MMISMS_R8_MESSAGE_LEN,
                (const wchar *)content_r8_ptr,
                content_len,
                content_len);
    }
    else
    {
        uint32 i = 0;
        uint16 offset = 0;
        uint16 data_offset = 0;

        for (i = 0; i < MIN(read_msg_ptr->read_content.sms_num, MMISMS_SPLIT_MAX_NUM); i++)
        {
            if (0 != read_msg_ptr->read_content.split_sms_length[i])
            {
                if (MN_SMS_UCS2_ALPHABET == read_msg_ptr->read_content.alphabet_ori[i])
                {
#if defined(__BIG_ENDIAN) || defined(__BigEndian)
                    MMI_MEMCPY( (uint8 *)(string + offset),
                                            read_msg_ptr->read_content.split_sms_length[i],
                                            msg_data_ptr + data_offset,
                                            read_msg_ptr->read_content.split_sms_length[i],
                                            read_msg_ptr->read_content.split_sms_length[i]
                                            );
#else
                    GUI_UCS2L2UCS2B((uint8 *)(string + offset),
                                                    read_msg_ptr->read_content.split_sms_length[i],
                                                    msg_data_ptr + data_offset,
                                                    read_msg_ptr->read_content.split_sms_length[i]);
#endif

                    offset += read_msg_ptr->read_content.split_sms_length[i]/sizeof(wchar);
                    data_offset += read_msg_ptr->read_content.split_sms_length[i];
                }
                else if (MN_SMS_UCS2_ALPHABET == read_msg_ptr->read_content.alphabet_ori[i])
                {
                    uint16 default_len = 0;

                    if(offset + read_msg_ptr->read_content.split_sms_length[i] <= convert_msg_len) 
                    {
                        default_len = read_msg_ptr->read_content.split_sms_length[i];

                        MMI_STRNTOWSTR(string + offset,
                                                        default_len,
                                                        msg_data_ptr + data_offset,
                                                        default_len,
                                                        default_len
                                                        );
                    }

                    offset += default_len;
                    data_offset += read_msg_ptr->read_content.split_sms_length[i];
                }
                else
                {
                    uint16 default_len = 0;

                    if(offset + read_msg_ptr->read_content.split_sms_length[i] <= convert_msg_len) 
                    {
                        default_len = MMIAPICOM_Default2Wchar(
                                                              msg_data_ptr + data_offset,
                                                              (string + offset),
                                                              read_msg_ptr->read_content.split_sms_length[i]
                                                              );
                    }

                    offset += default_len;
                    data_offset += read_msg_ptr->read_content.split_sms_length[i];
                }
            }
        }
    }

    if (PNULL != read_content_ptr->content_string.wstr_ptr)
    {
        MMI_WSTRNCPY(
            read_content_ptr->content_string.wstr_ptr,
            read_content_ptr->content_string.wstr_len,
            (const wchar *)string,
            MMISMS_R8_MESSAGE_LEN,
            MMISMS_R8_MESSAGE_LEN);
        read_content_ptr->content_string.wstr_len = MMIAPICOM_Wstrlen(read_content_ptr->content_string.wstr_ptr);
    }

    SCI_FREE(num_name.wstr_ptr);
    num_name.wstr_ptr = PNULL;

    SCI_FREE(string);
    string = PNULL; 
    
    SCI_FREE(content_r8_ptr);
    content_r8_ptr = PNULL;
}
#endif

/*****************************************************************************/
//  Description : add sms content item to richtext
//  Global resource dependence : 
//  Author: Kun Yu
//  Note:
/*****************************************************************************/
LOCAL void AddSMSContentItemtoRichText( 
                          MMI_CTRL_ID_T ctrl_id,
                          BOOLEAN                        is_only_display,    //IN: TRUE, indicate the SMS class is 0
                          MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                          uint16                    msg_len,            //IN:
                          uint8                     *msg_data_ptr,        //IN:
                          MN_SMS_TIME_STAMP_T        *sc_time_ptr,        //IN:
                          MN_CALLED_NUMBER_T        *orginal_addr_ptr    //IN:
                          )
{
    MMI_STRING_T num_name = {0};
    // uint32         string_len = 0;
    uint16         nIndex = 0;
    uint16         convert_msg_len = 0;
    uint8           time_len = 0;
    wchar          time_string[GUIRICHTEXT_TEXT_MAX_LEN+1] = {0};
    char           time[GUIRICHTEXT_TEXT_MAX_LEN+1] = {0};
    uint8          date[MMISET_DATE_STR_LEN] = {0};
    uint8          time_str[MMISET_TIME_STR_12HOURS_LEN + 1] = {0};
    GUIRICHTEXT_ITEM_T  item_data = {0};/*lint !e64*/
    wchar    *string = PNULL;
    wchar content_str[GUIRICHTEXT_TEXT_MAX_LEN+1] = {0};
    uint16 content_len = 0;    
    uint16 year = 0;

    wchar *content_r8_ptr = PNULL;
    content_r8_ptr = SCI_ALLOCAZ( ( MMISMS_R8_MESSAGE_LEN + 1 )*sizeof(wchar) );

    if(PNULL == content_r8_ptr)
    {
        return;
    }

    //SCI_MEMSET(content_r8_ptr,0x0,(sizeof(wchar) * ( MMISMS_R8_MESSAGE_LEN + 1 )));

    GUIRICHTEXT_DeleteAllItems(ctrl_id);	
#if defined(MMI_SMS_FONT_SETTING_SUPPORT)
    GUIRICHTEXT_SetFont(ctrl_id, MMISMS_GetWinFont());
#endif

    //Add item sender name or No.
    num_name.wstr_ptr = SCI_ALLOCAZ((MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
    if(PNULL == num_name.wstr_ptr)
    {
        SCI_FREE(content_r8_ptr);
        content_r8_ptr = PNULL;

        return;
    }
    //SCI_MEMSET((uint8 *)num_name.wstr_ptr, 0, (MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
    GetTransmitterDispString(
        alphabet,
        orginal_addr_ptr,
        &num_name
        );
    
    item_data.img_type = GUIRICHTEXT_IMAGE_NONE;
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
#ifdef DPHONE_SUPPORT 
    item_data.tag_type = GUIRICHTEXT_TAG_NONE;
#else
    if (MMISMS_GetSMSIsExtract() && 
       ((MMISMS_GetCurBoxType() == MMISMS_BOX_MT) ||
       (MMISMS_BOX_MT == g_mmisms_global.security_box_type)) ||
       is_only_display)
    {
        item_data.tag_type = GUIRICHTEXT_TAG_PHONENUM;
    }
    else
    {
        item_data.tag_type = GUIRICHTEXT_TAG_NONE;
    }
#endif
    SCI_MEMSET( content_str, 0, ((GUIRICHTEXT_TEXT_MAX_LEN+1)*sizeof(wchar)));
    item_data.text_data.buf.len = MMIAPICOM_CopyStr2Buf(content_str, &num_name,GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = content_str;
    GUIRICHTEXT_AddItem (
        ctrl_id,
        &item_data,
        &nIndex
        );

    if(MMISMS_READ_MTSMS == g_mmisms_global.read_msg.read_type
        || is_only_display)
    {
        year = sc_time_ptr->year + MMISMS_MT_OFFSERT_YEAR;
    }

    else
    {
        year = sc_time_ptr->year + MMISMS_OFFSERT_YEAR;
    }
    MMIAPISET_FormatDateStrByDateStyle(year,sc_time_ptr->month, sc_time_ptr->day,
        '/',date,MMISET_DATE_STR_LEN);
    MMIAPISET_FormatTimeStrByTime(sc_time_ptr->hour, sc_time_ptr->minute,time_str,MMISET_TIME_STR_12HOURS_LEN + 1);
    //Add item SMS time
    sprintf(time, "%s %s",date,time_str);

    time_len = strlen( time );

    MMI_STRNTOWSTR( 
            time_string,
            time_len,
            (const uint8 *)time,
            time_len,
            time_len
            );	
    item_data.text_data.buf.str_ptr = time_string;
    item_data.text_data.buf.len = time_len;
    item_data.tag_type = GUIRICHTEXT_TAG_NONE;
    GUIRICHTEXT_AddItem (
        ctrl_id,
        &item_data,
        &nIndex
        );

    //Add item SMS content
    convert_msg_len = msg_len;

    if (is_only_display)
    {
        if (MMISMS_IsR8Language(g_mmisms_global.class0_msg.language))
        {
            content_len = MMISMS_Default2NationalR8(
                    msg_data_ptr,
                    content_r8_ptr,
                    convert_msg_len,
                    g_mmisms_global.class0_msg.language,
                    g_mmisms_global.class0_msg.is_lock_shift,
                    g_mmisms_global.class0_msg.is_single_shift
                    );
        }
    }
    else if (MMISMS_IsR8Language(g_mmisms_global.read_msg.language))
    {
        content_len = MMISMS_Default2NationalR8(
                msg_data_ptr,
                content_r8_ptr,
                convert_msg_len,
                g_mmisms_global.read_msg.language,
                g_mmisms_global.read_msg.is_lock_shift,
                g_mmisms_global.read_msg.is_single_shift
                );
    }

    string = SCI_ALLOCAZ( ( MMISMS_R8_MESSAGE_LEN + 1 )*sizeof(wchar) );
    if(PNULL == string)
    {
        SCI_FREE(content_r8_ptr);

        SCI_FREE(num_name.wstr_ptr);
        return;
    }

    //SCI_TRACE_LOW:"MMISMS_R8:AddSMSContentItemtoRichText,language is %d,alphabet is %d,&&=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1854_112_2_18_2_53_5_212,(uint8*)"ddd",g_mmisms_global.read_msg.language,alphabet,is_only_display);

    if (is_only_display)
    {
        if(MMISMS_IsR8Language(g_mmisms_global.class0_msg.language)
          && (alphabet != MN_SMS_UCS2_ALPHABET)
          && (alphabet != MN_SMS_8_BIT_ALPHBET))
        {
            //SCI_TRACE_LOW:"MMISMS_R8:AddSMSContentItemtoRichText,convert_msg_len is %d,content_len is %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1863_112_2_18_2_53_5_213,(uint8*)"dd",convert_msg_len,content_len);

            MMI_WSTRNCPY(
                    string,
                    MMISMS_R8_MESSAGE_LEN,
                    (const wchar *)content_r8_ptr,
                    content_len,
                    content_len);
        }
        else if (MN_SMS_UCS2_ALPHABET == alphabet)
        {
#if defined(__BIG_ENDIAN) || defined(__BigEndian)
            MMI_MEMCPY( 
                (uint8 *)string,
                convert_msg_len*sizeof(wchar),
                msg_data_ptr,
                convert_msg_len*sizeof(wchar),
                convert_msg_len*sizeof(wchar)
                );        // indicate the SMS content need convert
#else
            GUI_UCS2L2UCS2B((uint8 *)string,
                                            convert_msg_len * sizeof(wchar),
                                            msg_data_ptr,
                                            convert_msg_len * sizeof(wchar));
#endif
        }
        //MN_SMS_8_BIT_ALPHBET
        else if (MN_SMS_8_BIT_ALPHBET == alphabet)
        {
            MMI_STRNTOWSTR(
                string,
                convert_msg_len,
                msg_data_ptr,
                convert_msg_len,
                convert_msg_len
                );
        }
        //MN_SMS_DEFAULT_ALPHABET
        else
        {
            content_len = MMIAPICOM_Default2Wchar(
                                                  msg_data_ptr,
                                                  string,
                                                  convert_msg_len
                                                  );
        }
    }
    else if(MMISMS_IsR8Language(g_mmisms_global.read_msg.language)
      && (alphabet != MN_SMS_UCS2_ALPHABET)
      && (alphabet != MN_SMS_8_BIT_ALPHBET))
    {
        //SCI_TRACE_LOW:"MMISMS_R8:AddSMSContentItemtoRichText,convert_msg_len is %d,content_len is %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_1915_112_2_18_2_53_5_214,(uint8*)"dd",convert_msg_len,content_len);

        MMI_WSTRNCPY(
                string,
                MMISMS_R8_MESSAGE_LEN,
                (const wchar *)content_r8_ptr,
                content_len,
                content_len);
    }
    else
    {
        uint32 i = 0;
        uint16 offset = 0;
        uint16 data_offset = 0;

        for (i = 0; i < MIN(g_mmisms_global.read_msg.read_content.sms_num, MMISMS_SPLIT_MAX_NUM); i++)
        {
            if (0 != g_mmisms_global.read_msg.read_content.split_sms_length[i])
            {
                if (MN_SMS_UCS2_ALPHABET == g_mmisms_global.read_msg.read_content.alphabet_ori[i])
                {
#if defined(__BIG_ENDIAN) || defined(__BigEndian)
                    MMI_MEMCPY( (uint8 *)(string + offset),
                                            g_mmisms_global.read_msg.read_content.split_sms_length[i],
                                            msg_data_ptr + data_offset,
                                            g_mmisms_global.read_msg.read_content.split_sms_length[i],
                                            g_mmisms_global.read_msg.read_content.split_sms_length[i]
                                            );
#else
                    GUI_UCS2L2UCS2B((uint8 *)(string + offset),
                                                    g_mmisms_global.read_msg.read_content.split_sms_length[i],
                                                    msg_data_ptr + data_offset,
                                                    g_mmisms_global.read_msg.read_content.split_sms_length[i]);
#endif

                    offset += g_mmisms_global.read_msg.read_content.split_sms_length[i]/sizeof(wchar);
                    data_offset += g_mmisms_global.read_msg.read_content.split_sms_length[i];
                }
                else
                {
                    uint16 default_len = 0;

                    //MN_SMS_8_BIT_ALPHBET
                    if (alphabet == MN_SMS_8_BIT_ALPHBET)
                    {
                        default_len = g_mmisms_global.read_msg.read_content.split_sms_length[i];

                        MMI_STRNTOWSTR(
                            (string + offset),
                            default_len,
                            msg_data_ptr + data_offset,
                            default_len,
                            default_len
                            );
                    }
                    //MN_SMS_DEFAULT_ALPHABET
                    else
                    {
                        default_len = MMIAPICOM_Default2Wchar(
                                                msg_data_ptr + data_offset,
                                                (string + offset),
                                                g_mmisms_global.read_msg.read_content.split_sms_length[i]
                                                );
                    }


                    offset += default_len;
                    if((msg_data_ptr[(i + 1)* MMISMS_DEFAULT_CHAR_LEN - 1] == 0x1b) && (msg_data_ptr[(i + 1)* MMISMS_DEFAULT_CHAR_LEN ] != 0x00) && (msg_data_ptr[(i + 1)* MMISMS_DEFAULT_CHAR_LEN ] < 0x80))
                    {
                        data_offset += g_mmisms_global.read_msg.read_content.split_sms_length[i] + 1;
                    }
                    else
                    {
                        data_offset += g_mmisms_global.read_msg.read_content.split_sms_length[i];
                    }
                }
            }
        }
    }
    item_data.text_data.buf.str_ptr = string;
    item_data.text_data.buf.len = MMIAPICOM_Wstrlen(string);
#ifdef DPHONE_SUPPORT 
	item_data.tag_type = GUIRICHTEXT_TAG_NONE;
#else 
    if(MMISMS_GetSMSIsExtract() && 
       ((MMISMS_GetCurBoxType() == MMISMS_BOX_MT) ||
       (MMISMS_BOX_MT == g_mmisms_global.security_box_type) ||
       is_only_display))
    {
        item_data.tag_type = GUIRICHTEXT_TAG_PARSE_PHONENUM;/*lint !e655*/

#ifdef STREAMING_HS_SUPPORT
        item_data.tag_type = item_data.tag_type | GUIRICHTEXT_TAG_PARSE_RTSP;/*lint !e655*/
#endif // STREAMING_HS_SUPPORT

#ifdef MMS_SUPPORT
        item_data.tag_type = item_data.tag_type | GUIRICHTEXT_TAG_PARSE_EMAIL;/*lint !e655*/
#endif

#ifdef BROWSER_SUPPORT
        item_data.tag_type = item_data.tag_type | GUIRICHTEXT_TAG_PARSE_URL;/*lint !e655*/
#endif
    }

    else
    {
        item_data.tag_type = GUIRICHTEXT_TAG_NONE;
    }
#endif    
    GUIRICHTEXT_AddItem (
        ctrl_id,
        &item_data,
        &nIndex
        );

    SCI_FREE(num_name.wstr_ptr);
    num_name.wstr_ptr = PNULL;

    SCI_FREE(string);
    string = PNULL; 
    
    SCI_FREE(content_r8_ptr);
    content_r8_ptr = PNULL;
}

/*****************************************************************************/
//     Description : load SMS content to window
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
//	Kun Yu  Replace text control with Richtext to get URL and number.
/*****************************************************************************/
LOCAL void LoadSms2Window(
                          MN_SMS_ALPHABET_TYPE_E        alphabet_type,        //IN:
                          BOOLEAN                        is_only_display,    //IN: TRUE, indicate the SMS class is 0
                          uint16                        msg_len,            //IN:
                          uint8                         *msg_data_ptr,        //IN:
                          MN_SMS_TIME_STAMP_T            *sc_time_ptr,        //IN:
                          MN_CALLED_NUMBER_T            *orginal_addr_ptr,    //IN:
                          MMISMS_R8_LANGUAGE_E    language
                         )
{
    MMI_CTRL_ID_T ctrl_id = MMISMS_SHOW_MSG_RICHTEXT_CTRL_ID;
    
    if (PNULL == msg_data_ptr || PNULL == orginal_addr_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:LoadSms2Window msg_data_ptr orginal_addr_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2059_112_2_18_2_53_5_215,(uint8*)"");
        return;
    }
            
    // open the different window.
    if (is_only_display)
    {
        MMK_CloseWin( MMISMS_SHOWCLASS0_WIN_ID );
        MMK_CreateWin( (uint32*)MMISMS_SHOWCLASS0MSG_WIN_TAB, (ADD_DATA)MMISMS_GetSimIdOfNewMsg() );
        ctrl_id = MMISMS_SHOWCLASS0_TEXTBOX_CTRL_ID;
    }
    else
    {
        if (!MMK_IsOpenWin(MMISMS_SHOWMSG_WIN_ID))
        {
            MMK_CreateWin((uint32 *)MMISMS_SHOWMSG_WIN_TAB, PNULL);
        }
        else
        {
            if(MMK_IsFocusWin(MMISMS_SHOWMSG_WIN_ID))
            {
                MMK_PostMsg(MMISMS_SHOWMSG_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
            }
        }
			
        ctrl_id = MMISMS_SHOW_MSG_RICHTEXT_CTRL_ID;
    }

    AddSMSContentItemtoRichText(
        ctrl_id,
        is_only_display,
        alphabet_type,
        msg_len,
        msg_data_ptr,
        sc_time_ptr,
        orginal_addr_ptr
        );

#ifdef DPHONE_SUPPORT 
    GUIRICHTEXT_SetItemBorderSpace(ctrl_id, 0);
    GUIRICHTEXT_SetLineSpace(ctrl_id, MMISMS_DETAIL_LINE_SPACE);
    GUIRICHTEXT_SetFocusMode(ctrl_id, GUIRICHTEXT_FOCUS_SHIFT_NONE);
#else
    GUIRICHTEXT_SetFocusMode(ctrl_id, GUIRICHTEXT_FOCUS_SHIFT_BASIC);
#endif
}

/*****************************************************************************/
//     Description : to show the content of message
//    Global resource dependence : 
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL void ShowMsgContent(
                          MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                          uint16                    msg_len,            //IN:
                          uint8                     *msg_data_ptr,        //IN:
                          MN_SMS_TIME_STAMP_T        *sc_time_ptr,        //IN:
                          MN_CALLED_NUMBER_T        *orginal_addr_ptr,    //IN:
                          MMISMS_R8_LANGUAGE_E    language,
#ifdef MMI_SMS_CHAT_SUPPORT                          
                          BOOLEAN                    is_chat_mode,
                          BOOLEAN                    is_need_time,
#endif                          
                          MMI_STRING_T                *string_ptr            //OUT:
                          )
{
    MMI_STRING_T num_name = {0};
    uint16 copy_offset = 0;
    uint16 convert_msg_len = 0;
    uint8 time_len = 0;
    wchar time[40] = {0};
    wchar cr_ch = MMIAPICOM_GetCRCh();
    uint8 cr_len = 1;
    wchar *content_r8_ptr = PNULL;
	uint16 content_len = 0;

    content_r8_ptr = SCI_ALLOCAZ( ( MMISMS_R8_MESSAGE_LEN + 1 )*sizeof(wchar) );

    if(PNULL == content_r8_ptr)
    {
        return;
    }
    
    //SCI_MEMSET(content_r8_ptr,0x0,(sizeof(wchar) * ( MMISMS_R8_MESSAGE_LEN + 1 )));
    
    if (PNULL == msg_data_ptr || PNULL == orginal_addr_ptr)
    {
        SCI_FREE(content_r8_ptr);
        content_r8_ptr = PNULL;  
        //SCI_TRACE_LOW:"MMISMS:ShowMsgContent msg_data_ptr orginal_addr_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2145_112_2_18_2_53_6_216,(uint8*)"");
        return;
    }
    
    // change original address to display string
    // the address will be the name stored in phonebook
    // currently, if the address string is ucs2, then the whole string will be ucs2.
    // if you just display phone number, please modify the following function.
    // and modify the priority of the whole string. 

#ifdef MMI_SMS_CHAT_SUPPORT
    if(!is_chat_mode)
#endif        
    {
        num_name.wstr_ptr = SCI_ALLOC_APP((MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
        SCI_MEMSET((uint8 *)num_name.wstr_ptr, 0, (MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
        GetTransmitterDispString(
            alphabet,
            orginal_addr_ptr,
            &num_name
            );        
    }

#ifdef MMI_SMS_CHAT_SUPPORT 
    if(is_need_time)
#endif
    {
        time_len = MMISMS_GetTimeDispString(sc_time_ptr, time);
    }
    
    // the length of SMS content will be too large to allocate memory, so we just calculate the length.
    if (alphabet != MN_SMS_UCS2_ALPHABET)
    {
        convert_msg_len = msg_len;
    }
    else
    {
        convert_msg_len = msg_len/sizeof(wchar);
    }
    
    // in first line, display address
    // in second line, display time
    // and then display the SMS content.
    
    // in order to allocate memory for string, calculate the string length.

    if (MMISMS_IsR8Language(g_mmisms_global.read_msg.language) && (alphabet != MN_SMS_UCS2_ALPHABET))
    {
        content_len = MMISMS_Default2NationalR8(
                msg_data_ptr,
                content_r8_ptr,
                convert_msg_len,
                g_mmisms_global.read_msg.language,
                g_mmisms_global.read_msg.is_lock_shift,
                g_mmisms_global.read_msg.is_single_shift
                );
        //SCI_TRACE_LOW:"MMISMS_R8:ShowMsgContent convert_msg_len is %d,content_len is %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2202_112_2_18_2_53_6_217,(uint8*)"dd",convert_msg_len,content_len);
        convert_msg_len = content_len;        
    }

#ifdef MMI_SMS_CHAT_SUPPORT
    if(is_chat_mode)
    {
        if (0 < time_len)
        {
            string_ptr->wstr_len = time_len + cr_len + convert_msg_len;
        }
        else
        {
            string_ptr->wstr_len = convert_msg_len;
        }        
    }
    else
#endif 
    {
        if (0 < time_len)
        {
            string_ptr->wstr_len = num_name.wstr_len + cr_len + time_len + cr_len + convert_msg_len;
        }
        else
        {
            string_ptr->wstr_len = num_name.wstr_len + cr_len + convert_msg_len;
        }
    }
    
    
    // allocate memory
    string_ptr->wstr_ptr = SCI_ALLOC_APP((string_ptr->wstr_len+1)*sizeof(wchar));
    SCI_MEMSET((uint8 *)string_ptr->wstr_ptr, 0x00, ((string_ptr->wstr_len+1)*sizeof(wchar)));
    
    copy_offset = 0;

#ifdef MMI_SMS_CHAT_SUPPORT
    if(!is_chat_mode)
#endif        
    {
        // address
        MMI_WSTRNCPY(
            &(string_ptr->wstr_ptr[copy_offset]),
            num_name.wstr_len,
            num_name.wstr_ptr,
            num_name.wstr_len,
            num_name.wstr_len
            );
        
        copy_offset += num_name.wstr_len;
        // CR
        MMI_WSTRNCPY(
            &(string_ptr->wstr_ptr[copy_offset]),
            cr_len,
            &cr_ch,
            cr_len,
            cr_len
            );
        
        copy_offset += cr_len;
    }
    
    if (0 < time_len)
    {
        // time
        MMI_WSTRNCPY(
            &(string_ptr->wstr_ptr[copy_offset]),
            time_len,
            time,
            time_len,
            time_len
            );
        
        copy_offset += time_len;
        // CR
        MMI_WSTRNCPY(
            &(string_ptr->wstr_ptr[copy_offset]),
            cr_len,
            &cr_ch,
            cr_len,
            cr_len
            );
        copy_offset += cr_len;
    }
    
    // message content

    //SCI_TRACE_LOW:"MMISMS_R8:ShowMsgContent language is %d,alphabet is %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2291_112_2_18_2_53_6_218,(uint8*)"dd",g_mmisms_global.class0_msg.language,alphabet);
    
    if(MMISMS_IsR8Language(g_mmisms_global.read_msg.language) &&  (alphabet != MN_SMS_UCS2_ALPHABET))
    {
        MMI_WSTRNCPY(
                &(string_ptr->wstr_ptr[copy_offset]),
                MMISMS_MAX_MESSAGE_LEN,
                (const wchar *)content_r8_ptr,
                content_len,
                content_len);
    }
    else
    {
        if (convert_msg_len != msg_len)
        {
#if defined(__BIG_ENDIAN) || defined(__BigEndian)
            MMI_MEMCPY( (uint8 *)&(string_ptr->wstr_ptr[copy_offset]),
                                    convert_msg_len*sizeof(wchar),
                                    msg_data_ptr,
                                    convert_msg_len*sizeof(wchar),
                                    convert_msg_len*sizeof(wchar)
                                    );
#else
        GUI_UCS2L2UCS2B((uint8 *)&(string_ptr->wstr_ptr[copy_offset]),
                                        convert_msg_len * sizeof(wchar),
                                        msg_data_ptr,
                                        convert_msg_len * sizeof(wchar));
#endif
        }
        else
        {
            MMIAPICOM_Default2Wchar(
                msg_data_ptr,
                &(string_ptr->wstr_ptr[copy_offset]),
                convert_msg_len
                );
        }
    }

    
    //SCI_TRACE_LOW:"MMISMS: ShowMsgContent length = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2332_112_2_18_2_53_6_219,(uint8*)"d", string_ptr->wstr_len);

    if (PNULL != num_name.wstr_ptr)
    {
        SCI_FREE(num_name.wstr_ptr);
        num_name.wstr_ptr = PNULL;
    }

    SCI_FREE(content_r8_ptr);
    content_r8_ptr = PNULL;        
    return;
}

/*****************************************************************************/
//     Description : Get time display string
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC uint8 MMISMS_GetTimeDispString(                                        //RETURN: the length of time string
                              MN_SMS_TIME_STAMP_T    *sc_time_ptr,    //IN:
                              wchar                    *time_ptr        //OUT:
                              )
{
    uint8 time_tmp[20] = {0};
    uint8 time_len = 0;
    
    if (PNULL == time_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_GetTimeDispString time_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2359_112_2_18_2_53_6_220,(uint8*)"");
        return 0;
    }
    if (PNULL != sc_time_ptr)
    {
        time_len = MMIAPICOM_TranslateTime(
            sc_time_ptr,
            time_tmp
            );
        
        MMI_STRNTOWSTR(
            time_ptr,
            time_len,
            (const uint8 *)time_tmp,
            time_len,
            time_len
            );
    }
    
    return (time_len);
}

/*****************************************************************************/
//     Description : Get transmitter display string
//    Global resource dependence : 
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
LOCAL void GetTransmitterDispString(
                                    MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                                    MN_CALLED_NUMBER_T        *orginal_addr_ptr,    //IN:
                                    MMI_STRING_T            *string_ptr            //OUT:
                                    )
{
    uint8 tele_num[MMISMS_PBNAME_MAX_LEN + 2] = {0};
    uint16 num_len = 0;
    wchar pb_name[MMISMS_PBNAME_MAX_LEN + 2] = {0};
    MMI_STRING_T num_name = {0};
    BOOLEAN find_result = FALSE;
    
    if (PNULL == orginal_addr_ptr || PNULL == string_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:GetTransmitterDispString orginal_addr_ptr string_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2399_112_2_18_2_53_6_221,(uint8*)"");
        return;
    }
    
    num_name.wstr_len  = 0;
    num_name.wstr_ptr = pb_name;
    
    if (orginal_addr_ptr->num_len > 0)
    {
        //indicate the phonebook info of the address is not saved.
        //SCI_TRACE_LOW:"MMISMS: GetTransmitterDispString number_type = %d, number_plan = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2410_112_2_18_2_53_6_222,(uint8*)"dd",orginal_addr_ptr->number_type, orginal_addr_ptr->number_plan);
        
        num_len = MMIAPICOM_GenNetDispNumber((MN_NUMBER_TYPE_E)(orginal_addr_ptr->number_type),
                                                                    (uint8)MIN((MMISMS_PBNUM_MAX_LEN >> 1),
                                                                    orginal_addr_ptr->num_len),
                                                                    orginal_addr_ptr->party_num,
                                                                    tele_num,
                                                                    (uint8)(MMISMS_PBNUM_MAX_LEN + 2)
                                                                    );

        find_result = MMISMS_GetNameByNumberFromPB(tele_num,&num_name, MMISMS_PBNAME_MAX_LEN);    
        
        if ((!find_result) 
            || (0 == num_name.wstr_len))
        {
            num_name.wstr_len = strlen((char *)tele_num);

            num_name.wstr_len = MIN(MMISMS_PBNAME_MAX_LEN, num_name.wstr_len);

            MMI_STRNTOWSTR(num_name.wstr_ptr,
                                        num_name.wstr_len,
                                        (const uint8 *)tele_num,
                                        num_name.wstr_len,
                                        num_name.wstr_len);

        }
    }
    else
    {
        num_name.wstr_len  = 0;
    }
    
    string_ptr->wstr_len  = num_name.wstr_len;

    MMI_WSTRNCPY(
        string_ptr->wstr_ptr,
        MMISMS_PBNAME_MAX_LEN,
        num_name.wstr_ptr,
        MMISMS_PBNAME_MAX_LEN,
        string_ptr->wstr_len
        );
}

/*****************************************************************************/
//     Description : to handle the signal APP_MN_SMS_IND
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_HandleSMSMTInd( 
                                  DPARAM    param    //IN:
                                  )
{    
    MnSmsMtIndS *sig_ptr = (MnSmsMtIndS *)param;
    APP_SMS_USER_DATA_T sms_user_data_t = {0};  
#if defined(MMI_BLACKLIST_SUPPORT)
    MMIPB_BCD_NUMBER_T orig_num = {0};
#endif
    MN_SMS_USER_HEAD_T    user_head_ptr_t = {0};
#ifdef MRAPP_SUPPORT
    int32 result = 0;
#endif
    
    if (PNULL == sig_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_HandleSMSMTInd sig_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2473_112_2_18_2_53_6_223,(uint8*)"");
        return;
    }
        
    SCI_MEMSET( &sms_user_data_t, 0x00, sizeof( APP_SMS_USER_DATA_T ) );
    
    //SCI_TRACE_LOW:"MMISMS: HandleSMSMTInd user_head_is_exist = %d, record_id = %d, storage = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2480_112_2_18_2_53_6_224,(uint8*)"ddd", sig_ptr->sms_t.user_head_is_exist, sig_ptr->record_id, sig_ptr->storage);

#if defined(MMI_BLACKLIST_SUPPORT)
    //if num in blacklist,block and delete the message
#ifdef MET_MEX_SUPPORT
	if (1 == MMIMex_Phs_HookResult(MEX_MSG_ID_SMS_DELIVER_MSG_IND, param, 0, NULL, MEX_HOOK_MSG_FROM_MMITASK))
	{
        BOOLEAN is_saved_in_sim = sig_ptr->storage != MN_SMS_STORAGE_ME;
        mex_log(1, "hook SMS MT");

        //删除短信
        MMISMS_DelSMSByAT(
            sig_ptr->dual_sys,
            TRUE,
            is_saved_in_sim,
            sig_ptr->record_id);
        //mex application registered to hook this message

        return;
	}
#ifdef MET_MEX_AQ_SUPPORT
        if (1==MMIMex_Aq_SmsWall(sig_ptr->dual_sys,&sig_ptr->sms_t.origin_addr_t))
        {
            //删除短信
            MMISMS_DelSMSByAT(
                sig_ptr->dual_sys,
                TRUE,
                (sig_ptr->storage != MN_SMS_STORAGE_ME),
                sig_ptr->record_id); 
			//mexsystem already  do the next
            return;
        }
#endif
        //判断是否正在运行全屏应用
        if(MMIMEX_IsSysRunning())
        {
            //显示新短信提示
            MMIMEX_SmsReceivePromt();
        }
#endif

    orig_num.number_len = sig_ptr->sms_t.origin_addr_t.num_len;
    orig_num.npi_ton = MMIAPICOM_ChangeTypePlanToUint8(sig_ptr->sms_t.origin_addr_t.number_type, MN_NUM_PLAN_UNKNOW );
    SCI_MEMCPY(&orig_num.number,&sig_ptr->sms_t.origin_addr_t.party_num,orig_num.number_len);  

    if(MMIAPISET_IsSMSBlacklist(&orig_num))
    {
        MNSMS_UpdateSmsStatusEx(sig_ptr->dual_sys,sig_ptr->storage, sig_ptr->record_id, MN_SMS_FREE_SPACE);
        return;
    }
#endif
    
    // need call MN API to decode user head and data.
    MMISMS_DecodeUserHeadAndContent(
        sig_ptr->dual_sys,
        sig_ptr->sms_t.user_head_is_exist,
        &(sig_ptr->sms_t.dcs),
        &(sig_ptr->sms_t.user_data_t),
        &sms_user_data_t
        );

#ifdef MRAPP_SUPPORT
	result = MMIMRAPP_dsmSMSfilter(sig_ptr->sms_t.dcs.alphabet_type, &sms_user_data_t.user_valid_data_t, &sig_ptr->sms_t.origin_addr_t); /*lint !e718 !e18*/
	if(result == 0 || result == -1)
	{
#ifdef _USE_OLD_L4_API
		MNSMS_UpdateSmsStatus(sig_ptr->storage, sig_ptr->record_id, MN_SMS_FREE_SPACE );
#else
		MNSMS_UpdateSmsStatusEx(sig_ptr->dual_sys, sig_ptr->storage, sig_ptr->record_id, MN_SMS_FREE_SPACE );
#endif	
		return;//is mrp configure files
	}
#endif

#ifdef MOBILE_VIDEO_SUPPORT
    CM_IsMvSms(&sms_user_data_t);
#endif

    #ifdef JAVA_SUPPORT_SUN
    #ifndef _MSC_VER
	/*检查是不是JAVA短消息*/
    if(APPSMS_IsAppSms_CallBack(sig_ptr->dual_sys,&sms_user_data_t)){
        APPSMS_RecvSms_CallBack(&(sig_ptr->sms_t));
        if(MN_SMS_NO_STORED != sig_ptr->storage)
        {
            MNSMS_UpdateSmsStatusEx(sig_ptr->dual_sys,sig_ptr->storage, sig_ptr->record_id, MN_SMS_FREE_SPACE);
        }
        return;
    }
    #endif
    #endif

#ifdef WRE_SUPPORT
	{
            extern BOOLEAN  WRESMS_RecvSms_CallBack(MN_DUAL_SYS_E dual_sys,uint32 alphabet,MN_CALLED_NUMBER_T * origin_addr,APP_SMS_USER_DATA_T *sms_user_data_t);

            if(TRUE == WRESMS_RecvSms_CallBack(sig_ptr->dual_sys,sig_ptr->sms_t.dcs.alphabet_type,&sig_ptr->sms_t.origin_addr_t,&sms_user_data_t))
            {
                MNSMS_UpdateSmsStatusEx(sig_ptr->dual_sys,sig_ptr->storage, sig_ptr->record_id, MN_SMS_FREE_SPACE);
                return;
            }
	}
#endif	

    if( (sig_ptr->sms_t.dcs.class_is_present) &&
        (MN_SMS_CLASS_0 == sig_ptr->sms_t.dcs.sms_class) )
    {
        //提取消息的有关信息
        //调用窗口立即显示有关内容
/*
        if ((MN_SMS_DEFAULT_ALPHABET == sig_ptr->sms_t.dcs.alphabet_type) ||
            (MN_SMS_8_BIT_ALPHBET == sig_ptr->sms_t.dcs.alphabet_type))
        {
            content_len = MMIAPICOM_Default2Ascii(
                sms_user_data_t.user_valid_data_t.user_valid_data_arr,
                ascii_vaild,
                sms_user_data_t.user_valid_data_t.length
                );
            sms_user_data_t.user_valid_data_t.length = content_len;

            SCI_MEMCPY(
                    sms_user_data_t.user_valid_data_t.user_valid_data_arr,
                    ascii_vaild,
                    sms_user_data_t.user_valid_data_t.length
                    );
        }
*/
        MMISMS_SetSimIdOfNewMsg(sig_ptr->dual_sys);
        MMISMS_SetNewSMSRingFlag(TRUE);
        MMIDEFAULT_TurnOnBackLight();

        SCI_MEMSET(&g_mmisms_global.class0_msg,0x0,sizeof(MMISMS_READ_MSG_T));
        g_mmisms_global.class0_msg.read_type = MMISMS_READ_MTSMS;
        g_mmisms_global.class0_msg.dual_sys = sig_ptr->dual_sys;
        SCI_MEMCPY(&g_mmisms_global.class0_msg.time_stamp,&sig_ptr->sms_t.time_stamp_t,sizeof(MN_SMS_TIME_STAMP_T));
        SCI_MEMCPY(&g_mmisms_global.class0_msg.address,&sig_ptr->sms_t.origin_addr_t,sizeof(MN_CALLED_NUMBER_T));
        g_mmisms_global.class0_msg.read_content.alphabet = sig_ptr->sms_t.dcs.alphabet_type;
        g_mmisms_global.class0_msg.read_content.length = sms_user_data_t.user_valid_data_t.length;
        SCI_MEMCPY(&g_mmisms_global.class0_msg.read_content.data,
                   sms_user_data_t.user_valid_data_t.user_valid_data_arr,
                   sms_user_data_t.user_valid_data_t.length);

        MMISMS_DecodeUserDataHead(&(sms_user_data_t.user_data_head_t), &user_head_ptr_t);
        g_mmisms_global.class0_msg.language = MMISMS_JudgeR8Language(&user_head_ptr_t);
        MMISMS_DecodeR8Header(
                &user_head_ptr_t,
                &g_mmisms_global.class0_msg.is_lock_shift,
                &g_mmisms_global.class0_msg.is_single_shift);

        MMISMS_FreeUserDataHeadSpace(&user_head_ptr_t);
            
        LoadSms2Window(
            sig_ptr->sms_t.dcs.alphabet_type,
            TRUE,
            sms_user_data_t.user_valid_data_t.length,
            sms_user_data_t.user_valid_data_t.user_valid_data_arr,
            &(sig_ptr->sms_t.time_stamp_t),            
            &(sig_ptr->sms_t.origin_addr_t),
            MMISMS_R8_LANGUAGE_NONE
            );
    }
    else
    {
        BOOLEAN is_finished = FALSE;
        
        // the SMS class is not 0, then must followed the operate step
        // 1 operate as java
        // 2 operate as mms
        // 3 operate as SMS
        is_finished = MMIAPIDM_HandleSmsMtInd(
                                                sig_ptr->sms_t.origin_addr_t.party_num,
                                                sig_ptr->sms_t.origin_addr_t.num_len,
                                                sms_user_data_t
                                                ); 
        if (is_finished)
        {
            if(MN_SMS_NO_STORED != sig_ptr->storage)
            {
                MNSMS_UpdateSmsStatusEx(sig_ptr->dual_sys,sig_ptr->storage, sig_ptr->record_id, MN_SMS_FREE_SPACE);
            }
        }

#ifdef JAVA_SUPPORT_IA
        if (!is_finished)
        {
            is_finished = OperateMtIndForJava(
                sig_ptr->dual_sys,
                &sms_user_data_t,
                sig_ptr->storage,
                sig_ptr->record_id,
                sig_ptr->sms_t
                );
        }
#endif     
        if (!is_finished)

        {
            MMIVC_SMS_SEND_TYPE_E send_type = MMIVC_SMS_SEND_NORMAL;
            send_type = MMISMS_OperateMtIndForVcardOrVcal(
                sig_ptr->dual_sys,
                &sms_user_data_t,
                sig_ptr->storage,
                sig_ptr->record_id,
                sig_ptr->sms_t
                );
            if(MMIVC_SMS_SEND_NORMAL != send_type)
            {
                is_finished = TRUE;
            }
        }
        
        if (!is_finished)
        {
            is_finished = OperateMtIndAsMmsPush(
                sig_ptr->dual_sys,
                &sms_user_data_t,
                sig_ptr->storage,
                sig_ptr->record_id,
                &sig_ptr->sms_t
                );
        }
        
        if (!is_finished)
        {
            is_finished = OperateMtIndAsSms(
                sig_ptr->dual_sys,
                &sms_user_data_t,
                sig_ptr->storage,
                sig_ptr->record_id,
                &sig_ptr->sms_t
                );
        }
    }
    //Add for CR 103433
#ifndef MMI_MULTI_SIM_SYS_SINGLE
    MMIAPISET_BgSetIgStatusParam(MMIAPISET_GetAnotherSim((sig_ptr->dual_sys)));    
#endif
}

/*****************************************************************************/
//     Description : to handle the signal APP_MN_SMS_STATUS_REPORT_IND
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_HandleSMSReportInd( 
                                      DPARAM    param    //IN:
                                      )
{
    MnSmsReportIndS *sig_ptr = (MnSmsReportIndS*)param;    
    // APP_SMS_USER_DATA_T sms_user_data_t = {0};  
    MMI_STRING_T            status_str = {0};
    MMI_STRING_T            status_str1 = {0};
    MMISMS_OPER_ERR_E result = MMISMS_MAX_ERR;
    MMI_WIN_ID_T alert_win_id = MMISMS_STATUE_REPORT_WIN_ID;
    MMI_WIN_PRIORITY_E win_priority = WIN_LOWEST_LEVEL;
    uint32 time_out = 0;
    MMI_TEXT_ID_T text_id = TXT_NULL;
    
    if (PNULL == sig_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_HandleSMSReportInd sig_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2736_112_2_18_2_53_7_225,(uint8*)"");
        return;
    }
    if (MMISMS_IsOrderOk())
    {
        MMISMS_SetSimIdOfNewMsg(sig_ptr->dual_sys);
        MMISMS_SetNewSMSRingFlag(TRUE);
        MMIDEFAULT_TurnOnBackLight();
        MMISMS_PlayNewSMSRing(sig_ptr->dual_sys);

        // status
        status_str.wstr_ptr = SCI_ALLOC_APP((MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar));
        SCI_MEMSET((uint8 *)status_str.wstr_ptr, 0, ((MMISMS_PBNAME_MAX_LEN + 1)*sizeof(wchar)));
        GetTransmitterDispString(
            MN_SMS_UCS2_ALPHABET,
            &(sig_ptr->status_report.dest_addr_t),
            &status_str
            );

        SCI_TRACE_LOW("MMISMS_HandleSMSReportInd tp_status=%d", sig_ptr->status_report.tp_status);

        if (MN_SMS_RECEIVED_BY_SME == sig_ptr->status_report.tp_status
            || MN_SMS_NOT_CONFIRM_RECEIVED == sig_ptr->status_report.tp_status
            || MN_SMS_SM_REPLACED_BY_SC == sig_ptr->status_report.tp_status)
        {
            text_id = TXT_SMS_SR_SUCC;
        }
        else
        {
            text_id = TXT_COMM_SEND_FAILED;
        }

        MMI_GetLabelTextByLang(text_id, &status_str1);

        if(MMK_IsOpenWin(alert_win_id))
        {
            MMK_CloseWin(alert_win_id);
        }

        if(MMISMS_IsForbitNewMsgWin(FALSE))
        {
            win_priority = WIN_LOWEST_LEVEL;
        }
        else
        {
            win_priority = WIN_ONE_LEVEL;
        }

        MMIPUB_OpenAlertWinByTextPtr(&time_out,&status_str,&status_str1,IMAGE_PUBWIN_NEWMSG,&alert_win_id,&win_priority,MMIPUB_SOFTKEY_ONE,PNULL);

        //Set Pubwin Title
        {
            MMI_STRING_T title_str = {0};

            MMI_GetLabelTextByLang(TXT_SMS_STATUSREPORT, &title_str);

            MMIPUB_SetWinTitleText(alert_win_id, &title_str, FALSE);
        }

        //MMISMS_SetOperStatus(update_oper);
        result = MMISMS_UpdateStatusReportMsgReq(
                                sig_ptr->dual_sys,
                                MN_SMS_FREE_SPACE,
                                sig_ptr->storage,
                                sig_ptr->record_id);

        if(PNULL != status_str.wstr_ptr)
        {
            SCI_FREE(status_str.wstr_ptr);
            status_str.wstr_ptr = PNULL;
        }

        //SCI_TRACE_LOW:"MMISMS:MMISMS_HandleSMSReportInd dual_sys is %d,storage is %d,record_id is %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2798_112_2_18_2_53_7_226,(uint8*)"ddd",sig_ptr->dual_sys,sig_ptr->storage,sig_ptr->record_id);
    }
    
#ifndef MMI_MULTI_SIM_SYS_SINGLE
    //Add for CR 103433
    MMIAPISET_BgSetIgStatusParam(MMIAPISET_GetAnotherSim((sig_ptr->dual_sys)));   
#endif
}

/*****************************************************************************/
//     Description : Set readed SMS to window
//    Global resource dependence : g_mmisms_global
//  Author: Tracy Zhang
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_SetReadedSms2Window(
                                       MMISMS_BOX_TYPE_E    box_type        //IN:
                                       )
{
    MMISMS_READ_MSG_T    *read_msg_ptr    = &(g_mmisms_global.read_msg);
    MN_SMS_TIME_STAMP_T    *time_stamp_ptr = PNULL;
    
    if ((MMISMS_BOX_MT == box_type) ||
        //Add by huangfengming for cr150400
        (MMISMS_BOX_SENDSUCC == box_type) 
#if defined (MMI_SMS_SECURITYBOX_SUPPORT)
        ||(MMISMS_BOX_SECURITY == box_type)
#endif
        )
    {
        time_stamp_ptr = &(read_msg_ptr->time_stamp);
    }
    
    LoadSms2Window( 
        read_msg_ptr->read_content.alphabet,
        FALSE,                                            // the SMS class is not 0
        read_msg_ptr->read_content.length,
        read_msg_ptr->read_content.data,
        time_stamp_ptr,
        &(read_msg_ptr->address),
        read_msg_ptr->language
        );
}

/*****************************************************************************/
// Description :
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
PUBLIC MMIVC_SMS_SEND_TYPE_E MMISMS_IsMtForVcard( 
                                                 MN_DUAL_SYS_E         dual_sys,                                  
                                                 APP_SMS_USER_DATA_T   *user_data_ptr)
{
    uint8 iei_num = 0;
    int32 i = 0;
    int32 index = -1;
    int32 des_port_num = 0;
    MMIVC_SMS_SEND_TYPE_E send_type = MMIVC_SMS_SEND_NORMAL;
    MN_SMS_USER_HEAD_T user_head_ptr_t = {0};
    
    //SCI_TRACE_LOW:"SMS: MMISMS_IsMtForVcard()"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2855_112_2_18_2_53_7_227,(uint8*)"");
    MNSMS_DecodeUserDataHeadEx(
        dual_sys,
        &user_data_ptr->user_data_head_t,
        &user_head_ptr_t);
    // Check whether Vcard sms
    iei_num = user_head_ptr_t.iei_num;
    index = -1;
    for (i = 0;i < iei_num;i++)
    {
        if( MN_SMS_APP_PORT_16_BIT_ADDR == user_head_ptr_t.iei_arr[i].iei)
        {
            index = i;
            des_port_num = (user_head_ptr_t.iei_arr[i].iei_data_t[0] << 8) | (user_head_ptr_t.iei_arr[i].iei_data_t[1]);
            break;
        }
    }
    //SCI_TRACE_LOW:"SMS: MMISMS_IsMtForVcard: index = %d  port = %d "
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2872_112_2_18_2_53_7_228,(uint8*)"dd", index, des_port_num);
    if (-1 != index && MMISMS_VCARD_PORT_NUM == des_port_num )
    {
        send_type =  MMIVC_SMS_SEND_VCARD;
    }
#ifdef MMI_VCALENDAR_SUPPORT
    else if (-1 != index && MMISMS_VCAL_PORT_NUM == des_port_num )
    {
        send_type =  MMIVC_SMS_SEND_VCALENDAR;
    }
#endif
    MNSMS_FreeUserDataHeadSpaceEx(dual_sys, &user_head_ptr_t);

    return send_type;
}

/*****************************************************************************/
// Description : operate MT ind as for Vcard
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
PUBLIC MMIVC_SMS_SEND_TYPE_E MMISMS_OperateMtIndForVcardOrVcal(
                                                               MN_DUAL_SYS_E         dual_sys,                                  
                                                               APP_SMS_USER_DATA_T   *user_data_ptr,
                                                               MN_SMS_STORAGE_E       storage,
                                                               MN_SMS_RECORD_ID_T     record_id,
                                                               MN_SMS_MT_SMS_T        sms_t)
{
    BOOLEAN is_finished = TRUE;
    MMIVC_SMS_SEND_TYPE_E send_type = MMIVC_SMS_SEND_NORMAL;
#ifdef WIN32
    //    APP_SMS_USER_DATA_T test_data = {{ 0x05,
    //                                       0x05,0x04,0x23,0xf4,0x00,0x00
    //                                     },
    //                                     { 0x5f,
    //                                       0x42,0x45,0x47,0x49,0x4e,0x3a,0x56,0x43,0x41,0x52,
    //                                       0x44,0x0d,0x0a,0x56,0x45,0x52,0x53,0x49,0x4f,0x4e,
    //                                       0x3a,0x32,0x2e,0x31,0x0d,0x0a,0x4e,0x3b,0x43,0x48,
    //                                       0x41,0x52,0x53,0x45,0x54,0x3d,0x55,0x54,0x46,0x2d,
    //                                       0x38,0x3a,0xe9,0x83,0x91,0xe5,0x9b,0xbd,0xe5,0xb8,
    //                                       0x85,0x0d,0x0a,0x54,0x45,0x4c,0x3b,0x50,0x52,0x45,
    //                                       0x46,0x3b,0x56,0x4f,0x49,0x43,0x45,0x3a,0x2b,0x38,
    //                                       0x36,0x31,0x33,0x37,0x38,0x34,0x31,0x30,0x37,0x31,
    //                                       0x35,0x36,0x0d,0x0a,0x45,0x4e,0x44,0x3a,0x56,0x43,
    //                                       0x41,0x52,0x44,0x0d,0x0a
    //                                     }
    //                                    };  
    //    user_data_ptr = &test_data;
#endif
    
    if (PNULL == user_data_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_OperateMtIndForVcardOrVcal user_data_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2923_112_2_18_2_53_7_229,(uint8*)"");
        return send_type;
    }
    // 判断是否为Vcard sms
    send_type = MMISMS_IsMtForVcard(dual_sys, user_data_ptr);
    //SCI_TRACE_LOW:"SMS: OperateMtIndForVcard: send_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2928_112_2_18_2_53_7_230,(uint8*)"d",send_type);
    if (MMIVC_SMS_SEND_NORMAL != send_type)
    {    
        MMISMS_RecvVcardOrVcalSms(dual_sys, user_data_ptr, &sms_t.time_stamp_t,send_type); 

        // call MN API to update SMS status.
        if(MN_SMS_NO_STORED != storage)
        {
            MNSMS_UpdateSmsStatusEx(dual_sys,storage, record_id, MN_SMS_FREE_SPACE);
        }
        is_finished = TRUE;
    }
    else
    {
        is_finished = FALSE;
    }
    //SCI_TRACE_LOW:"SMS: OperateMtIndForVcard: is_finished = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2944_112_2_18_2_53_7_231,(uint8*)"d",is_finished);
    return send_type;
}

/*****************************************************************************/
// Description : receive Vcard over sms
// Global resource dependence :s_long_vcardsms_ptr
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL void MMISMS_RecvVcardOrVcalSms(
                                     MN_DUAL_SYS_E         dual_sys, 
                                     APP_SMS_USER_DATA_T   *sms_ptr,
                                     MN_SMS_TIME_STAMP_T   *time_stamp_ptr,
                                     MMIVC_SMS_SEND_TYPE_E send_type
                                     )
{
    MN_SMS_USER_HEAD_T user_head_ptr_t = {0};
    VCARD_LONG_SMS_DATA_T vcard_data = {0};
    uint8 receive_num = 1;
    uint8 iei_num = 0;
    uint8 sms_index[MMIVC_MAX_LONG_SMS_NUM]={0};
    uint8 data_length = 0;
    uint32 i = 0;
    uint8 j = 1;
    int32 index = -1;
    uint16 ref_num = 0;
    uint8 max_num = 0;
    uint8 seq_num = 0;
    uint32 current_time = 0;
    int32 empty_pos = -1;
    uint8 name_arr[VCARD_NAME_MAX_LEN +1] = {0};
    VCARD_LONG_SMS_T *send_type_ptr =PNULL;
    
    if (PNULL == sms_ptr || PNULL == time_stamp_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:OperateMtIndAsSms sms_ptr time_stamp_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2978_112_2_18_2_53_7_232,(uint8*)"");
        return;
    }
    //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_2981_112_2_18_2_53_7_233,(uint8*)"");
    MNSMS_DecodeUserDataHeadEx(
        dual_sys, 
        &sms_ptr->user_data_head_t,
        &user_head_ptr_t);
    index   = -1;
    iei_num = user_head_ptr_t.iei_num;
    for (i = 0;i < iei_num;i++)
    {
        if (MN_SMS_CONCAT_8_BIT_REF_NUM  == user_head_ptr_t.iei_arr[i].iei ||
            MN_SMS_CONCAT_16_BIT_REF_NUM == user_head_ptr_t.iei_arr[i].iei )
        {
            index = i;
            break;
        }
    }

    if(MMIVC_SMS_SEND_VCARD == send_type)
    {
        send_type_ptr = s_long_vcardsms_ptr;
    }
#ifdef MMI_VCALENDAR_SUPPORT
    else if(MMIVC_SMS_SEND_VCALENDAR == send_type)
    {
        send_type_ptr = s_long_vcalsms_ptr;
    }
#endif
    //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms  index=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3008_112_2_18_2_53_7_234,(uint8*)"d", index);
    //是长短信
    if (index != -1)
    {
        if (PNULL == send_type_ptr)//指针为空
        {
            send_type_ptr = InitLongVcardsmsStruct();//申请内存

            if (PNULL == send_type_ptr) //申请不到
            {
                goto RECV_RETURN;
            }

            if(MMIVC_SMS_SEND_VCARD == send_type)
            {
               s_long_vcardsms_ptr= send_type_ptr ;
            }
#ifdef MMI_VCALENDAR_SUPPORT
            else if(MMIVC_SMS_SEND_VCALENDAR == send_type)
            {
                s_long_vcalsms_ptr=send_type_ptr ;
            }
#endif
        }

        //获取长短信相关信息
        data_length = sms_ptr->user_valid_data_t.length;
        if (MN_SMS_CONCAT_8_BIT_REF_NUM == user_head_ptr_t.iei_arr[index].iei)
        {
            ref_num = user_head_ptr_t.iei_arr[index].iei_data_t[0];
            max_num = user_head_ptr_t.iei_arr[index].iei_data_t[1];
            seq_num = user_head_ptr_t.iei_arr[index].iei_data_t[2];    
        }
        else if (MN_SMS_CONCAT_16_BIT_REF_NUM == user_head_ptr_t.iei_arr[index].iei)
        {
            ref_num  = user_head_ptr_t.iei_arr[index].iei_data_t[0];
            ref_num  = ref_num << 8;
            ref_num += user_head_ptr_t.iei_arr[index].iei_data_t[1];
            max_num  = user_head_ptr_t.iei_arr[index].iei_data_t[2];
            seq_num  = user_head_ptr_t.iei_arr[index].iei_data_t[3];
        }
        //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms ref_num=%d,max_num=%d,seq_num=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3050_112_2_18_2_53_7_235,(uint8*)"ddd",ref_num, max_num, seq_num);
        //重复判断
        for(i = 0;i < MMIVC_MAX_LONG_SMS_NUM;i++)
        {
            //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms ref_num=%d,max_num=%d,seq_num=%d,vcard_sms[i].ref_num=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3055_112_2_18_2_53_7_236,(uint8*)"dddd",ref_num, max_num, seq_num,send_type_ptr->vcard_sms[i].ref_num);
            if (ref_num == send_type_ptr->vcard_sms[i].ref_num)
            {
                if (seq_num != send_type_ptr->vcard_sms[i].seq_num)
                {
                    sms_index[receive_num] = i;
                    receive_num++;
                }
                else
                {
                    //已经保存有同样的短信
                    //SCI_TRACE_LOW:"MMISMS_RecvVcardOrVcalSms the same seq sms"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3066_112_2_18_2_53_7_237,(uint8*)"");
                    goto RECV_RETURN;
                }
            }
        }
        //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms receive_num=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3071_112_2_18_2_53_7_238,(uint8*)"d", receive_num);
        //长短信未到齐
        if (receive_num < max_num)
        {
            //搜索空白位置，保存此条信息
            for(i = 0;i < MMIVC_MAX_LONG_SMS_NUM;i++)
            {
                if (send_type_ptr->vcard_sms[i].recv_time == 0)
                {
                    empty_pos = i;
                    break;
                }
            }
            //清理超时短信:如果五分钟内不能来齐，则丢弃
            current_time = GetSmsReceiveTimeStamp();
            //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms i=%d,current_time=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3086_112_2_18_2_53_7_239,(uint8*)"dd", i,current_time);
            if(i == MMIVC_MAX_LONG_SMS_NUM)
            {
                for(i = 0;i < MMIVC_MAX_LONG_SMS_NUM;i++)
                {
                    if (((send_type_ptr->vcard_sms[i].recv_time + 5*MMISMS_MINUTE_TIME < current_time)
                        ||(send_type_ptr->vcard_sms[i].recv_time > current_time))
                        &&(ref_num != send_type_ptr->vcard_sms[i].ref_num))
                    {
                        SCI_MEMSET(&(send_type_ptr->vcard_sms[i]),0,sizeof(VCARD_SMS_T));
                        send_type_ptr->vcard_sms[i].ref_num = MMISMS_INVAILD_INDEX;
                        send_type_ptr->vcard_sms[sms_index[i]].is_valid = FALSE;
                        empty_pos = i;
                        //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms i=%d"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3099_112_2_18_2_53_7_240,(uint8*)"d", i);
                        break;
                    }
                }
            }
            //将收到的信息保存到全局变量
            //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms empty_pos=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3105_112_2_18_2_53_7_241,(uint8*)"d", empty_pos);
            if ((empty_pos < MMIVC_MAX_LONG_SMS_NUM) && (empty_pos != -1))
            {
                //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms write global var,seq_num=%d,ref_num=%d,max_num=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3108_112_2_18_2_53_7_242,(uint8*)"ddd",seq_num,ref_num,max_num);
                send_type_ptr->vcard_sms[empty_pos].recv_time = current_time;
                send_type_ptr->vcard_sms[empty_pos].seq_num   = seq_num;
                send_type_ptr->vcard_sms[empty_pos].ref_num   = ref_num;
                send_type_ptr->vcard_sms[empty_pos].max_num   = max_num;
                send_type_ptr->vcard_sms[empty_pos].length    = data_length;
                send_type_ptr->vcard_sms[empty_pos].is_valid    = TRUE;
                SCI_MEMCPY(send_type_ptr->vcard_sms[empty_pos].data_arr,
                    sms_ptr->user_valid_data_t.user_valid_data_arr,
                    data_length);
            }
            goto RECV_RETURN;
        }
        else
        {
            //长短信已到齐，合并长短信
            //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms write vcf file"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3124_112_2_18_2_53_8_243,(uint8*)"");
            j = 1;
            while(j <= receive_num)
            {
                for(i = 1;i < receive_num;i++)
                {
                    if (send_type_ptr->vcard_sms[sms_index[i]].seq_num == j)
                    {
                        SCI_MEMCPY(vcard_data.data+vcard_data.length,
                                            send_type_ptr->vcard_sms[sms_index[i]].data_arr,
                                            send_type_ptr->vcard_sms[sms_index[i]].length);
                        vcard_data.length += send_type_ptr->vcard_sms[sms_index[i]].length;
                        SCI_MEMSET(&(send_type_ptr->vcard_sms[sms_index[i]]),
                                            0, sizeof(VCARD_SMS_T));

                        send_type_ptr->vcard_sms[sms_index[i]].ref_num = MMISMS_INVAILD_INDEX;
                        send_type_ptr->vcard_sms[sms_index[i]].is_valid = FALSE;
                        break;
                    }
                    else if (seq_num == j)
                    {
                        SCI_MEMCPY(vcard_data.data+vcard_data.length,
                            sms_ptr->user_valid_data_t.user_valid_data_arr,
                            sms_ptr->user_valid_data_t.length);
                        vcard_data.length += sms_ptr->user_valid_data_t.length;
                        break;
                    }    
                }
                j++;
            }            

            //检查是否还有未组装的Vcard
            for(i = 0;i < MMIVC_MAX_LONG_SMS_NUM; i++)
            {
                if (send_type_ptr->vcard_sms[i].is_valid)
                {
                    break;
                }
            }

            if (MMIVC_MAX_LONG_SMS_NUM == i)//没有未组装的Vcard
            {
                SCI_FREE(send_type_ptr);
                send_type_ptr = PNULL;
                if(MMIVC_SMS_SEND_VCARD == send_type)
                {
                   s_long_vcardsms_ptr= PNULL ;
                }
#ifdef MMI_VCALENDAR_SUPPORT
                else if(MMIVC_SMS_SEND_VCALENDAR == send_type)
                {
                    s_long_vcalsms_ptr=PNULL ;
                }
#endif
                //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms free memory"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3178_112_2_18_2_53_8_244,(uint8*)"");
            }
            else
            {
                //SCI_TRACE_LOW:"SMS: MMISMS_RecvVcardOrVcalSms exist vcard sms"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3182_112_2_18_2_53_8_245,(uint8*)"");
            }
        }
    }
    else
    {   //不是长短信
        SCI_MEMCPY(vcard_data.data,
            sms_ptr->user_valid_data_t.user_valid_data_arr,
            sms_ptr->user_valid_data_t.length);
        vcard_data.length = sms_ptr->user_valid_data_t.length;
    }

    GetVcardFileNameOverSms(time_stamp_ptr, name_arr);

#if defined MMI_VCARD_SUPPORT
    if(MMIVC_SMS_SEND_VCARD == send_type)
    {
        MMISMS_SaveVcardSms(name_arr, vcard_data.data, vcard_data.length);
    }
    else
#endif
#ifdef MMI_VCALENDAR_SUPPORT
    if(MMIVC_SMS_SEND_VCALENDAR == send_type)
    {
        //SCI_TRACE_LOW:"SMS: MMISMS_SaveVcalendarSms send_type=%d, vcard_data.length=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3206_112_2_18_2_53_8_246,(uint8*)"dd",send_type,vcard_data.length);
        MMISMS_SaveVcalendarSms(name_arr, vcard_data.data, vcard_data.length);
    }
#endif
    MMISMS_SetNewSMSRingFlag(TRUE); 
    MMIDEFAULT_TurnOnBackLight();
    MMISMS_PlayNewSMSRing(dual_sys);
    //如果屏保被点亮中，则关闭屏保窗口
    if (MMK_IsOpenWin(MMIMAIN_SHOWTIME_WIN_ID))
    {
        MMK_CloseWin(MMIMAIN_SHOWTIME_WIN_ID);
    }    
    
RECV_RETURN:
    MNSMS_FreeUserDataHeadSpaceEx(dual_sys, &user_head_ptr_t);
    return;
}

/*****************************************************************************/
// Description : 获得vCard文件名称(名称由SMS+接收时间字串组成)
// Global resource dependence : 
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL void GetVcardFileNameOverSms(MN_SMS_TIME_STAMP_T  *sc_time_ptr, uint8 *name_ptr)
{
    if (PNULL == sc_time_ptr || PNULL == name_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:GetVcardFileNameOverSms sc_time_ptr name_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3232_112_2_18_2_53_8_247,(uint8*)"");
        return;
    }
    sprintf((char*)name_ptr, "SMS_%02d%02d%02d%02d%02d.vcf",
        sc_time_ptr->year, sc_time_ptr->month, sc_time_ptr->day,
        sc_time_ptr->hour, sc_time_ptr->minute);

    return;
}
#if defined MMI_VCARD_SUPPORT
/*****************************************************************************/
// Description : save received Vcard data over sms
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL void MMISMS_SaveVcardSms(
                               uint8   *name_ptr,
                               uint8   *buf_ptr,
                               uint16   buf_len
                               )
{

    VCARD_ERROR_E  save_ret = VCARD_FILE_ERROR;
    wchar          *name_wstr_ptr = PNULL;
   
    if (PNULL == name_ptr || PNULL == buf_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SaveVcardSms name_ptr buf_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3255_112_2_18_2_53_8_248,(uint8*)"");
        return;
    }
    
    if (buf_len > 0)
    {
        name_wstr_ptr = SCI_ALLOCA(sizeof(wchar) *(MMIFILE_FILE_NAME_MAX_LEN +1));
        if(name_wstr_ptr != PNULL)
        {
            SCI_MEMSET(name_wstr_ptr, 0x00, sizeof(wchar) *(MMIFILE_FILE_NAME_MAX_LEN +1));
            MMI_STRNTOWSTR(name_wstr_ptr, MMIFILE_FILE_NAME_MAX_LEN, name_ptr, MMIFILE_FILE_NAME_MAX_LEN, strlen((char*)name_ptr));
            save_ret = MMIAPIVCard_SaveVCardByData(name_wstr_ptr, buf_ptr, buf_len);
        }

        if (VCARD_NO_ERROR == save_ret)
        {
            MMIAPISMS_ShowNewVcardOrVcalendarPrompt(TRUE,MMIVC_SMS_SEND_VCARD);
            MMK_SendMsg(MMISMS_NEW_VCARD_MSG_WIN_ID,
                MSG_SMS_VCARD_FILE_NAME,
                name_wstr_ptr);
            
        }
        else
        {
            //MMIPUB_OpenAlertWarningWin(TXT_MMS_TEXT_UNSUPORTED_MESSAGE);
            MMIAPIPB_OpenVcardContactWin(buf_ptr, buf_len);                
        }  
        if(name_wstr_ptr != PNULL)
        {
            SCI_FREE(name_wstr_ptr);
        }
    }

    return;
}
#endif
#ifdef MMI_VCALENDAR_SUPPORT
/*****************************************************************************/
// Description : save received Vcalendar data over sms
// Global resource dependence :
// Author:renyi.hu
// Note: 
/*****************************************************************************/
LOCAL void MMISMS_SaveVcalendarSms(
                               uint8   *name_ptr,
                               uint8   *buf_ptr,
                               uint16   buf_len
                               )
{

    if (PNULL == name_ptr || PNULL == buf_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS:MMISMS_SaveVcardSms name_ptr buf_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3280_112_2_18_2_53_8_249,(uint8*)"");
        return;
    }
    if (buf_len > 0)
    {
        MMIVCAL_SaveVCalendarByData(VIRTUAL_WIN_ID, MSG_VCALENDAR_WRITE_SMS_CNF, buf_ptr, buf_len);
    }

    return;
}
#endif

#if defined(MMI_VCARD_SUPPORT) || defined(MMI_VCALENDAR_SUPPORT)
/*****************************************************************************/
// Description : to show the new Vcard prompt
//               (参考函数:MMISMS_ShowNewMsgPrompt)
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
PUBLIC void MMISMS_ShowNewVcardOrVcalendarPrompt(BOOLEAN is_play_ring,MMIVC_SMS_SEND_TYPE_E type)
{
    MMI_STRING_T prompt_str = {0};
    uint32 time_out = 0;
    MMI_WIN_PRIORITY_E win_priority = WIN_LOWEST_LEVEL;
    MMI_WIN_ID_T alert_win_id = MMISMS_NEW_VCARD_MSG_WIN_ID;
    MMIPUB_HANDLE_FUNC  handle_func = PNULL;
    //SCI_TRACE_LOW:"MMISMS_ShowNewVcardOrVcalendarPrompt type=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3305_112_2_18_2_53_8_250,(uint8*)"d",type);
#ifdef MMI_VCALENDAR_SUPPORT
    if(MMIVC_SMS_SEND_VCALENDAR == type)
    {
        alert_win_id =MMISMS_NEW_VCALENDAR_MSG_WIN_ID;
        MMI_GetLabelTextByLang(TXT_SMS_RECEIVE_VCALENDAR_MSG, &prompt_str);
        handle_func=(MMIPUB_HANDLE_FUNC)MMISMS_HandleNewVcalendarMsgWin;
    }
    else// if(MMIVC_SMS_SEND_VCARD == type)
    {
        alert_win_id =MMISMS_NEW_VCARD_MSG_WIN_ID;
        MMI_GetLabelTextByLang(TXT_SMS_RECEIVE_VCARD_MSG, &prompt_str);
        handle_func=(MMIPUB_HANDLE_FUNC)MMISMS_HandleNewVcardMsgWin;
    }
#else
    alert_win_id =MMISMS_NEW_VCARD_MSG_WIN_ID;
    MMI_GetLabelTextByLang(TXT_SMS_RECEIVE_VCARD_MSG, &prompt_str);
    handle_func=(MMIPUB_HANDLE_FUNC)MMISMS_HandleNewVcardMsgWin;
#endif   
    //SCI_TRACE_LOW:"SMS: MMISMS_ShowNewVcardOrVcalendarPrompt"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_RECEIVE_3324_112_2_18_2_53_8_251,(uint8*)"");
    MMISMS_SetNewSMSRingFlag(is_play_ring);
    if (MMK_IsFocusWin(alert_win_id))
    {
        // play the ring
        //has played ring when new msg received MMK_SendMsg(alert_win_id, MSG_SMS_PLAY_RING, PNULL); 
    }
    else
    {
        if (MMK_IsOpenWin(alert_win_id))
        {
            MMK_WinGrabFocus(alert_win_id);
        }
        else
        {
#ifdef SCREENSAVER_SUPPORT
            if (MMK_IsFocusWin(MMIIDLE_SCREENSAVER_WIN_ID))
            {
                MMIAPIIDLESS_CloseScreenSaver();
            }
#endif
            if (MMIAPIKL_IsPhoneLocked())        
            {
                win_priority = WIN_ONE_LEVEL;
                if(MMIVC_SMS_SEND_VCARD==type)
                {
                    MMIAPIKL_SetNewVcardStatus();//设置新Vcard状态
                }
#ifdef MMI_VCALENDAR_SUPPORT
                else if(MMIVC_SMS_SEND_VCALENDAR==type)
                {
                    MMIAPIKL_SetNewVcalendarStatus();
                }
#endif
            }
            else
            {
                win_priority = WIN_LOWEST_LEVEL;
            }
            MMIPUB_OpenAlertWinByTextPtr(&time_out,
                &prompt_str,
                PNULL,
                IMAGE_PUBWIN_NEWMSG,
                &alert_win_id,
                &win_priority,
                MMIPUB_SOFTKEY_CUSTOMER,
                handle_func);
        }
    }
}
#endif

/*****************************************************************************/
// Description : Get SM receive time stamp
// Global resource dependence :
// Author:Bill.Ji
// Note: 
/*****************************************************************************/
LOCAL uint32 GetSmsReceiveTimeStamp(void)
{
    return (TM_GetTotalSeconds() + (365*23+366*7)*24*3600);
    /*1970年1月1日零点到1999年12月31日23点59分59秒以来经过的秒数*/
}

#ifdef MMI_STATUSBAR_SCROLLMSG_SUPPORT
/*****************************************************************************/
//  Description : get scroll msg record id
//  Global resource dependence : 
//  Author:
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISMS_GetScrollmsgRecordId(void)
{
    return s_sms_statusbar_scrollmsg.record_id;
}
#endif

#ifdef MMI_SMS_CHAT_SUPPORT
/*****************************************************************************/
//  Description : to show the content of message
//  Global resource dependence : 
//  Author:fengming.huang
//  Note:
/*****************************************************************************/
PUBLIC void MMISMS_ShowMsgContent(
                                  MN_SMS_ALPHABET_TYPE_E    alphabet,            //IN:
                                  uint16                    msg_len,            //IN:
                                  uint8                     *msg_data_ptr,        //IN:
                                  MN_SMS_TIME_STAMP_T        *sc_time_ptr,        //IN:
                                  MN_CALLED_NUMBER_T        *orginal_addr_ptr,    //IN:
                                  MMISMS_R8_LANGUAGE_E    language,
                                  BOOLEAN                    is_chat_mode,
                                  BOOLEAN                    is_need_time,
                                  MMI_STRING_T                *string_ptr            //OUT:
                                  )
{
    ShowMsgContent(
                   alphabet,
                   msg_len,
                   msg_data_ptr,
                   sc_time_ptr,            
                   orginal_addr_ptr,
                   language,
                   is_chat_mode,
                   is_need_time,
                   string_ptr
                  );    
}
#endif
