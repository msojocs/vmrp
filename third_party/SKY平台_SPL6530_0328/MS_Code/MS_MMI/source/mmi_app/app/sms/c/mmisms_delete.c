/****************************************************************************
** File Name:      mmisms_delete.c                                          *
** Author:                                                                  *
** Date:           8/23/2007                                                *
** Copyright:      2007 Spreadtrum, Incorporated. All Rights Reserved.      *
** Description:    This file is used to describe deleting SMS.              *
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
#include "mmisms_delete.h"
#include "mmisms_app.h"
#include "mmisms_text.h"
#include "mmisms_id.h"
#include "mmi_text.h"
#include "mmipub.h"
#include "mmi_appmsg.h"
#include "mmismsapp_wintab.h"
#include "mmiudisk_export.h"
#include "mmiota_export.h"
#include "mmimms_export.h"
#include "mmiidle_export.h"
#include "mmialarm_export.h"
#include "mmiota_id.h"
#include "mmisms_save.h"
#include "mmisms_send.h"
#include "mmiidle_statusbar.h"
#ifdef PUSH_EMAIL_3_IN_1_SUPPORT//3 in 1 support
#include "mail_fldr.h"
#include "mail_export.h"
#include "mmisms_order.h"
#endif//PUSH_EMAIL_3_IN_1_SUPPORT
#ifdef MET_MEX_SUPPORT
#include "met_mex_export.h"
#endif
#ifdef PDA_UI_DROPDOWN_WIN
#include "mmidropdownwin_export.h"
#endif

/**--------------------------------------------------------------------------*
**                         EXTERN DECLARE                                   *
**--------------------------------------------------------------------------*/
extern MMISMS_GLOBAL_T g_mmisms_global;      // SMS的全局变量
#ifdef MMI_PDA_SUPPORT
extern MMISMS_DETAIL_MSGLOG_DATA_T *g_sms_match_data_ptr;
extern uint16 g_match_sms_delete_num;
#endif
/**-------------------------------------------------------------------------*
**                         FUNCTION DECLARE                                 *
**--------------------------------------------------------------------------*/

/*****************************************************************************/
//     Description : to handle the signal APP_MN_UPDATE_SMS_STATUS_CNF
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: handle the normal delete operation
/*****************************************************************************/
LOCAL void HandleDeleteCnf( 
                           MN_SMS_CAUSE_E    cause    //IN:
                           );

/*****************************************************************************/
//     Description : to handle the signal APP_MN_UPDATE_SMS_STATUS_CNF
//    Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//    Note: handle the normal move operation
/*****************************************************************************/
LOCAL void HandleMoveDeleteCnf( 
                           MN_SMS_CAUSE_E    cause    //IN:
                           );

/**-------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/
/*****************************************************************************/
//     Description : to handle the signal APP_MN_UPDATE_SMS_STATUS_CNF
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: handle the normal delete operation
/*****************************************************************************/
LOCAL void HandleDeleteCnf( 
                           MN_SMS_CAUSE_E    cause    //IN:
                           )
{
    MN_SMS_RECORD_ID_T record_id = 0;
    
    //SCI_TRACE_LOW:"MMISMS: HandleDeleteCnf cause = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_85_112_2_18_2_52_34_21,(uint8*)"d", cause);
//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT
    if(MMIMEX_DelSmsRsp(0,cause))
    {
        //process started by mex application, should not go on.            
        return;       
    }
#endif // MET_MEX_SUPPORT
//============== MET MEX  end ===================
    
    if (MN_SMS_OPERATE_SUCCESS == cause)
    {

#ifdef MMI_TIMERMSG_SUPPORT
        MMISMS_TIMERMSG_DeleteTimerMsg(g_mmisms_global.operation.cur_order_id_arr[g_mmisms_global.operation.cur_order_index]);
#endif

        // delete the message from order list
        record_id = g_mmisms_global.operation.cur_order_id_arr[g_mmisms_global.operation.cur_order_index]->record_id;
        MMISMS_DelSmsInfoInOrder(MMISMS_GetCurOperationOrderId());
        MMISMS_SetNVFolderTypeFlag(record_id,MMISMS_FOLDER_NORMAL);

        if (MMK_IsOpenWin(MMISMS_DELETE_ANIM_WIN_ID))
        {
            MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
        }
#ifdef MRAPP_SUPPORT
		else if (MMIMRAPP_IsReadBySMS())
		{
			MMK_PostMsg(VIRTUAL_WIN_ID, MMISMS_MSG_DELETE_NEXT, &cause, sizeof(int32));
		}
#endif
        else
        {
            MMISMS_SetDelAllFlag(FALSE);
            //全部删除完毕，将未处理链表中的sms插入到相应的链表中
            MMISMS_InsertUnhandledSmsToOrder();
            MMISMS_ReadMMSPushFromUnhandledList(); //读取mms push 消息并传给mms模块
            MMISMS_HandleOrderChangeCallBack(); //删除完毕刷新list
            MMIAPIALM_CheckAndStartDelayedAlarm();
        }
    }
    else
    {
        MMISMS_ClearOperInfo();

#ifdef MRAPP_SUPPORT
		if (MMIMRAPP_IsReadBySMS())
		{
			MMK_PostMsg(VIRTUAL_WIN_ID, MMISMS_MSG_DELETE_NEXT, &cause, sizeof(int32));
            return;
		}
#endif

        // 提示用户删除失败
        if(MN_SMS_SIM_SAVE_STATUS_FREE == cause)
        {
            MMIPUB_OpenAlertFailWin(TXT_SMS_READ_ERR);
        }
        else
        {
            MMIPUB_OpenAlertFailWin(TXT_ERROR);
        }
        
        //全部删除完毕，将未处理链表中的sms插入到相应的链表中
        MMISMS_InsertUnhandledSmsToOrder();
        MMISMS_ReadMMSPushFromUnhandledList(); //读取mms push 消息并传给mms模块
        MMISMS_SetDelAllFlag(FALSE);
        MMISMS_HandleOrderChangeCallBack(); //删除完毕更新list

        MMIPUB_CloseWaitWin(MMISMS_DELETE_ANIM_WIN_ID);
        MMIAPIALM_CheckAndStartDelayedAlarm();
    }
}

/*****************************************************************************/
//     Description : to delete request to MN
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_DeleteReqToMN(
                            MMISMS_BOX_TYPE_E        type,        //IN:
                            MMISMS_ORDER_ID_T       order_id,    //IN:
                            MMISMS_OPER_STATUS_E    oper_status    //IN:
                            )
{
    BOOLEAN                    ret_val              = FALSE;
    MN_SMS_STORAGE_E        storage              = MN_SMS_NO_STORED;
    ERR_MNSMS_CODE_E        result            = ERR_MNSMS_NONE;
    MMISMS_STATE_T          sms_order_info = {0};
    MN_DUAL_SYS_E           dual_sys          = MN_DUAL_SYS_1;
    //SCI_TRACE_LOW:"MMISMS: DeleteReqToMN(%d, %d, %d)"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_161_112_2_18_2_52_34_22,(uint8*)"ddd", type, order_id, oper_status);
    
    // get the selected SMS's state
    MMISMS_GetSmsInfoFromOrder(order_id,&sms_order_info);
    
    storage = (MN_SMS_STORAGE_E)sms_order_info.flag.storage;
    dual_sys = (MN_DUAL_SYS_E)sms_order_info.flag.dual_sys;
    // to delete the SMS
    //SCI_TRACE_LOW:"MMISMS: DeleteReqToMN record_id = %d, storage = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_170_112_2_18_2_52_34_23,(uint8*)"dd", sms_order_info.record_id, storage);
    result = MNSMS_UpdateSmsStatusEx(dual_sys, storage, sms_order_info.record_id, MN_SMS_FREE_SPACE);
    
    //SCI_TRACE_LOW:"MMISMS: DeleteReqToMN MNSMS_UpdateSmsStatusEx result = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_173_112_2_18_2_52_34_24,(uint8*)"d", result);
    
    if (ERR_MNSMS_NONE == result)
    {
        //SCI_TRACE_LOW:"MMISMS: DeleteReqToMN oper_status = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_177_112_2_18_2_52_34_25,(uint8*)"d",oper_status);
        MMISMS_SetOperStatus(oper_status);
        MMISMS_SetCurOrderId(g_mmisms_global.operation.cur_order_index, order_id);    
        g_mmisms_global.operation.cur_type = type;
        //SCI_TRACE_LOW:"sms: DeleteReqToMN, g_mmisms_global.operation.cur_type = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_181_112_2_18_2_52_34_26,(uint8*)"d", g_mmisms_global.operation.cur_type);
        ret_val = TRUE;
    }
    else
    {
        ret_val = FALSE;
    }
    
    return (ret_val);        
}

/*****************************************************************************/
//     Description : to delete the pointed SMS
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISMS_DeletePointedSMSById( 
                                           MMISMS_ORDER_ID_T    order_pos
                                           )
{
    BOOLEAN ret_val = FALSE;
    MN_SMS_STORAGE_E storage = MN_SMS_NO_STORED;
    MN_SMS_RECORD_ID_T record_id = 0;
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    
    MMISMS_GetPointedSMSMemInfo(order_pos, &dual_sys, &storage, &record_id);
    
    // to delete the SMS
    if (ERR_MNSMS_NONE == MNSMS_UpdateSmsStatusEx(dual_sys, storage, record_id, MN_SMS_FREE_SPACE))
    {
        ret_val = TRUE;
    }
    else
    {
        ret_val = FALSE;
    }
    
    return (ret_val);
}

/*****************************************************************************/
//     Description : to handle the signal APP_MN_UPDATE_SMS_STATUS_CNF
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note: in the operation of delete and update, the system will return the same signal.
/*****************************************************************************/
PUBLIC void MMISMS_HandleUpdateSMSCnf(
                                      DPARAM  param
                                      )
{
    MnUpdateSmsStateCnfS *sig_ptr = (MnUpdateSmsStateCnfS *)param;
    MMISMS_OPER_STATUS_E oper_status = MMISMS_GetCurrentOperStatus();
    MN_SMS_STORAGE_E storage = MN_SMS_NO_STORED;
    MN_SMS_RECORD_ID_T record_id = 0;
    BOOLEAN result = TRUE;
    BOOLEAN is_in_order = TRUE;    // used for checking record in the order
    MMISMS_ORDER_ID_T cur_order_id= PNULL;
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    ERR_MNSMS_CODE_E err_code = ERR_MNSMS_NONE;
    MMISMS_OPER_ERR_E ret_val = MMISMS_NO_SPACE;
    uint8 total_num = 1;
    MMISMS_ORDER_ID_T next_longsms_ptr = PNULL;    
    uint8 fail_id[ MMISMS_DEST_ADDR_MAX_NUM ]= { 0 };
    uint16 send_result = 0;
    uint16 dest_count = 0;

    MMISMS_SEND_T *send_message_ptr = PNULL;
    MMISMS_UPDATE_INFO_T *update_info_ptr = PNULL;
    BOOLEAN is_find_backsend = FALSE;
    BOOLEAN is_mosend = FALSE;
    
    if (PNULL == sig_ptr)
    {
        //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf sig_ptr == PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_251_112_2_18_2_52_34_27,(uint8*)"");
        return;
    }
    //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf record_id = %d, storage = %d,oper_status = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_255_112_2_18_2_52_34_28,(uint8*)"ddd", sig_ptr->record_id, sig_ptr->storage,oper_status);

    send_message_ptr = MMISMS_GetSendInfoFromSendLIst(MMISMS_SENDSTATUS_SENDING);
    if (PNULL != send_message_ptr)
    {
        if (send_message_ptr->backsend_update_data.is_backsend_update)
        {
            if (MN_SMS_NO_STORED != send_message_ptr->backsend_update_data.backsend_updata_storage 
                && sig_ptr->storage == send_message_ptr->backsend_update_data.backsend_updata_storage
                && sig_ptr->record_id == send_message_ptr->backsend_update_data.backsend_record_id)
            {
                oper_status = send_message_ptr->backsend_update_data.backsend_send_oper;
                is_find_backsend = TRUE;
            }
        }
        if (!is_find_backsend && send_message_ptr->mosend_update_data.is_mosend_update)
        {
            if (MN_SMS_NO_STORED != send_message_ptr->mosend_update_data.mosend_updata_storage 
                && sig_ptr->storage == send_message_ptr->mosend_update_data.mosend_updata_storage
                && sig_ptr->record_id == send_message_ptr->mosend_update_data.mosend_record_id)
            {
                oper_status = send_message_ptr->mosend_update_data.mosend_send_oper;
                is_mosend = TRUE;
            }
        }        
    }
    
    // according to the operate status
    switch (oper_status) 
    {
#ifdef MMI_PDA_SUPPORT
    case MMISMS_MATCH_DATA_DELETE:
        {
            if (PNULL == g_sms_match_data_ptr)
            {
                MMISMS_EndDeleteMatchMsg(g_sms_match_data_ptr);
            }
            else
            {
                cur_order_id = g_sms_match_data_ptr->order_id_list[g_match_sms_delete_num];
                MMISMS_DelSmsInfoInOrder(cur_order_id);
                MMISMS_HandleOrderChangeCallBack();
                g_match_sms_delete_num = MMISMS_GetNextSelectedOrderID();
                if (g_match_sms_delete_num >= g_sms_match_data_ptr->matched_count)
                {
                    MMISMS_EndDeleteMatchMsg(g_sms_match_data_ptr);
                }
                else
                {
                    MMISMS_DeleteNextMatchMsgById(g_sms_match_data_ptr->order_id_list[g_match_sms_delete_num]);
                }                
            }
        }
        break;
#endif
    case MMISMS_DELETE_MSG:                // delete SMS operation
    case MMISMS_MOVE_SMS_TO_SECURITY:
    case MMISMS_MOVE_SMS_FROM_SECURITY:
    case MMISMS_MOVE_SMS_TO_ME:
    case MMISMS_MOVE_SMS_TO_SIM:        
        // get the storage and record_id
        is_in_order = MMISMS_GetPointedSMSMemInfo(
            MMISMS_GetCurOperationOrderId(), 
            &dual_sys,
            &storage, 
            &record_id
            );
        if ((is_in_order)
            && (sig_ptr->record_id == record_id) 
            && (sig_ptr->storage == storage)
            && (sig_ptr->dual_sys == dual_sys))
        {
            // the same storage and record_id.
            result = TRUE;
        }
        else
        {
            // search in order list.
            // ignore the different message type
            //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf get record_id = %d, storage = %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_334_112_2_18_2_52_35_29,(uint8*)"dd", record_id, storage);
            result = MMISMS_GetOrderIdByStorage(
                &cur_order_id,
                MMISMS_TYPE_SMS,
                sig_ptr->dual_sys,
                sig_ptr->storage,
                sig_ptr->record_id
                );
        }
        
        if (result)
        {
            if(MMISMS_DELETE_MSG == oper_status)
            {
                HandleDeleteCnf(sig_ptr->cause);
            }
            else if(MMISMS_IsMoveOperation(oper_status))
            {
                HandleMoveDeleteCnf(sig_ptr->cause);
            }
            else
            {
                MMK_CloseParentWin(MMISMS_MSGBOX_MAIN_WIN_ID);
                if (MMK_IsOpenWin(MMISMS_DELETE_ANIM_WIN_ID))
                {
                    //MMK_CloseWin(MMK_GetPrevWinId(MMISMS_DELETE_ANIM_WIN_ID));
                    MMIPUB_CloseWaitWin(MMISMS_DELETE_ANIM_WIN_ID);
                }
            }
        }
        else
        {
            // there's error in the deleting procedure.
            MMK_CloseParentWin(MMISMS_MSGBOX_MAIN_WIN_ID);
            if (MMK_IsOpenWin(MMISMS_DELETE_ANIM_WIN_ID))
            {
                //MMK_CloseWin(MMK_GetPrevWinId(MMISMS_DELETE_ANIM_WIN_ID));
                MMIPUB_CloseWaitWin(MMISMS_DELETE_ANIM_WIN_ID);
            }
        }
        break;
        
    case MMISMS_UPDATE_SENDFAIL_SMS:
    case MMISMS_UPDATE_NOSEND_SMS:    // update the no send SMS   
    case MMISMS_DELETE_SENDFAIL_SMS:
    case MMISMS_UPDATE_BACKSEND_SMS:
        if(g_mmisms_global.save_info.is_change_save_pos)
        {
            cur_order_id = MMISMS_GetLongsmsOrder();
            if(PNULL == cur_order_id)
            {
                g_mmisms_global.save_info.is_change_save_pos = FALSE;
                MMISMS_SetsmsOrder(PNULL);
                MMISMS_ClearSaveInfo();
                MMISMS_DisplayUpdatePrompt(TRUE);
                MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);
                MMISMS_HandleOrderChangeCallBack();
                MMISMS_SetDeliverResendFlag(FALSE);
                //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_422_112_2_18_2_52_35_30,(uint8*)"d", __LINE__);
                MMISMS_ClearOperInfo();
                MMISMS_DeleteSendList();
                return;
            }
            next_longsms_ptr = cur_order_id->next_long_sms_ptr;
            MMISMS_DelSmsInfoInOrder(cur_order_id);            
            if( PNULL != next_longsms_ptr)
            {
                err_code = MNSMS_UpdateSmsStatusEx(
                                            next_longsms_ptr->flag.dual_sys, 
                                            next_longsms_ptr->flag.storage, 
                                            next_longsms_ptr->record_id, 
                                            MN_SMS_FREE_SPACE);
                
                MMISMS_SetsmsOrder(next_longsms_ptr);
            }
            else if(MMISMS_UPDATE_BACKSEND_SMS == oper_status)
            {
                g_mmisms_global.save_info.is_change_save_pos = FALSE;
                MMISMS_SetsmsOrder(PNULL);
                MMISMS_ClearSaveInfo();
                MMISMS_DisplayUpdatePrompt(TRUE);
                MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);
                MMISMS_HandleOrderChangeCallBack();
                MMISMS_SetDeliverResendFlag(FALSE);
                //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_422_112_2_18_2_52_35_30,(uint8*)"d", __LINE__);
                MMISMS_ClearOperInfo();
                MMISMS_DeleteSendList();
            }
            else
            {
                if(MMISMS_DELETE_SENDFAIL_SMS != oper_status)
                {
                    send_message_ptr = MMISMS_GetSendInfoFromSendLIst(MMISMS_SENDSTATUS_SENDING);
                    if(PNULL != send_message_ptr)
                    {
                        MMISMS_SetsmsOrder(PNULL);
                        total_num = MMISMS_SplitLongMsg(&send_message_ptr->send_content.send_message, send_message_ptr->is_head_has_port);
                        ret_val = MMISMS_SaveNextMessage(
                                                    dual_sys,
                                                    send_message_ptr->send_content.dest_info.update_dest_addr,
                                                    total_num,
                                                    1,
                                                    send_message_ptr);
                    }                    
                }
                else
                {
                    // 提示保存成功
                    g_mmisms_global.save_info.is_change_save_pos = FALSE;
                    MMISMS_SetsmsOrder(PNULL);
                    MMISMS_ClearSaveInfo();
                    MMISMS_DisplayUpdatePrompt(TRUE);
                    MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);
                    MMISMS_HandleOrderChangeCallBack();
                    MMISMS_SetDeliverResendFlag(FALSE);
                    //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf %d"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_422_112_2_18_2_52_35_30,(uint8*)"d", __LINE__);
                    MMISMS_ClearOperInfo();
                }                
            }
            return;
        }
        if (PNULL != send_message_ptr)
        {
            if (MMISMS_UPDATE_BACKSEND_SMS == oper_status)
            {
                update_info_ptr = &send_message_ptr->backsend_update_data.backsend_update_info;
            }
            else if (is_mosend)
            {
                update_info_ptr = &send_message_ptr->mosend_update_data.mosend_update_info;
            }
        }
        if (PNULL == update_info_ptr)
        {
            update_info_ptr = &g_mmisms_global.update_info;
        }
        
        // get the storage and record_id
        MMISMS_GetPointedSMSMemInfo(
            update_info_ptr->order_id_arr[update_info_ptr->current_num],
            &dual_sys,
            &storage, 
            &record_id
            );

        if ((sig_ptr->record_id == record_id) && (sig_ptr->storage == storage))
        {
            // update the SMS
            if (MN_SMS_OPERATE_SUCCESS == sig_ptr->cause)
            {
                BOOLEAN is_need_update_chatwin = FALSE;
#ifdef MMI_TIMERMSG_SUPPORT
                MMISMS_TIMERMSG_DeleteTimerMsg(update_info_ptr->order_id_arr[update_info_ptr->current_num]);
#endif
#ifdef MMI_SMS_CHAT_SUPPORT
                if(MMK_IsOpenWin(MMISMS_SHOW_CHAT_SMS_WIN_ID)) //当前条目删除完整了
                {
                    uint32 index = 0;

                    cur_order_id = update_info_ptr->order_id_arr[update_info_ptr->current_num];
                    if(MMISMS_FindChatWinIndex(&index, cur_order_id))
                    {
                        if((update_info_ptr->current_num + 1) < update_info_ptr->update_num)
                        {
                            if (cur_order_id->flag.is_concatenate_sms)
                            {
                                next_longsms_ptr = cur_order_id->next_long_sms_ptr;
                                g_mmisms_global.chat_item[index].cur_order_id = next_longsms_ptr;
                            }
                        }
                        else
                        {
                            is_need_update_chatwin = TRUE;
                            g_mmisms_global.del_index = index;
                        }                        
                    }                    
                    
                }
#endif                
                MMISMS_DelSmsInfoInOrder(update_info_ptr->order_id_arr[update_info_ptr->current_num]);  //根据order_id_arr找到删除的form中的index
                MMISMS_HandleOrderChangeCallBack();
#ifdef MMI_SMS_CHAT_SUPPORT                
                if(is_need_update_chatwin && MMK_IsOpenWin(MMISMS_SHOW_CHAT_SMS_WIN_ID))
                {
                     MMK_SendMsg(MMISMS_SHOW_CHAT_SMS_WIN_ID, 
                        MMISMS_MSG_UPDATELIST, 
                        PNULL);    //通知相应的窗口，更新list内容    
                }
#endif                
               update_info_ptr->current_num++;
                
                if (update_info_ptr->current_num < update_info_ptr->total_num || 
                    (MMISMS_UPDATE_NOSEND_SMS == oper_status && ( update_info_ptr->current_num < MMISMS_SPLIT_MAX_NUM )&& update_info_ptr->order_id_arr[update_info_ptr->current_num] != PNULL))
                {
                    //未删完,要设置storage和record id
                    MMISMS_GetPointedSMSMemInfo(
                        update_info_ptr->order_id_arr[update_info_ptr->current_num],
                        &dual_sys, &storage, &record_id);
                    if (MMISMS_UPDATE_BACKSEND_SMS == oper_status)
                    {
                        if(send_message_ptr != PNULL)
                        {
                            send_message_ptr->backsend_update_data.backsend_record_id = record_id;
                            send_message_ptr->backsend_update_data.backsend_updata_storage = storage;
                        }
                    }
                    else if (PNULL != send_message_ptr && send_message_ptr->mosend_update_data.is_mosend_update)
                    {
                            send_message_ptr->mosend_update_data.mosend_record_id = record_id;
                            send_message_ptr->mosend_update_data.mosend_updata_storage = storage;
                    }                    
                    
                    if (!MMISMS_DeletePointedSMSById(update_info_ptr->order_id_arr[update_info_ptr->current_num]))
                    {
                        MMISMS_DisplayUpdatePrompt(TRUE);
                    }
                }
                else if(MMISMS_UPDATE_NOSEND_SMS == oper_status)
                {
                    if (PNULL != send_message_ptr && send_message_ptr->mosend_update_data.is_mosend_update)
                    {
                        if (MMISMS_CheckNextDestAddr() && MMISMS_MoveDestAddrId())
                        {
                            MMISMS_InitSendSaveResult(send_message_ptr);
                            MMISMS_AppSendSms(sig_ptr->dual_sys, send_message_ptr->send_content.is_need_save, send_message_ptr);
                            MMISMS_UpdateSmsSendindNumber(send_message_ptr);
                            //MMK_SendMsg(MMISMS_SEND_ANIM_WIN_ID,MMISMS_MSG_UPDATE_NUMBER,PNULL);
                        }
                        else
                        {
                            // 提示发送失败/保存成功
                            MMISMS_ClearSaveInfo();
                            send_result = MMISMS_GetResultofSending(fail_id, &dest_count);
                            MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);
                            MMIPUB_CloseWaitWin(MMISMS_SEND_ANIM_WIN_ID);
                            MMIPUB_CloseWaitWin(MMISMS_READSMS_WAIT_WIN_ID);
                            MMISMS_SetDeliverResendFlag(FALSE);
                            MMISMS_ShowSendResult((uint8)send_result, fail_id);
                            MMISMS_DeleteSendList();
                            //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf %d"
                            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_533_112_2_18_2_52_35_31,(uint8*)"d", __LINE__);
                        }
                    }
                    else
                    {
                        MMISMS_DisplayUpdatePrompt(TRUE);
                    }                    
                }
                else if(MMISMS_DELETE_SENDFAIL_SMS == oper_status || MMISMS_UPDATE_BACKSEND_SMS == oper_status)
                {
                    if (MMISMS_UPDATE_BACKSEND_SMS == oper_status 
                        && PNULL != send_message_ptr 
                        && send_message_ptr->mosend_update_data.is_mosend_update
                        && send_message_ptr->mosend_update_data.mosend_update_info.current_num < send_message_ptr->mosend_update_data.mosend_update_info.total_num)
                    {
                        result = TRUE;
                        break;
                    }
                    //if(MMK_IsOpenWin(MMISMS_SEND_ANIM_WIN_ID))
                    {                        
                        // to send the message to next destination
                        if (MMISMS_CheckNextDestAddr() && MMISMS_MoveDestAddrId() && PNULL != send_message_ptr)
                        {
                            MMISMS_InitSendSaveResult(send_message_ptr);
                            MMISMS_AppSendSms(sig_ptr->dual_sys, send_message_ptr->send_content.is_need_save, send_message_ptr);
                            MMISMS_UpdateSmsSendindNumber(send_message_ptr);
                            //MMK_SendMsg(MMISMS_SEND_ANIM_WIN_ID,MMISMS_MSG_UPDATE_NUMBER,PNULL);
                        }
                        else
                        {
                            #ifdef MMI_TIMERMSG_SUPPORT          
							BOOLEAN is_timer_mode = MMISMS_GetIsTimerMode();
                            #endif
                            // 提示发送失败/保存成功
                            MMISMS_ClearSaveInfo();
                            send_result = MMISMS_GetResultofSending(fail_id, &dest_count);
							
                            MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);
                            MMIPUB_CloseWaitWin(MMISMS_SEND_ANIM_WIN_ID);
                            MMIPUB_CloseWaitWin(MMISMS_READSMS_WAIT_WIN_ID);
#ifdef MMI_TIMERMSG_SUPPORT							
                            if(is_timer_mode)
                            {
                               MMISMS_CloseEditWin(); 
                            }
#endif							
                            
                            MMISMS_SetDeliverResendFlag(FALSE);
                            MMISMS_ShowSendResult((uint8)send_result, fail_id);
                            MMISMS_DeleteSendList();
                            //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf %d"
                            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_583_112_2_18_2_52_35_32,(uint8*)"d", __LINE__);
                        }  
                    }
//                     else
//                     {
//                         // 提示保存成功
//                         MMISMS_ClearSaveInfo();
//                         MMISMS_DisplayUpdatePrompt(TRUE);
//                         MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);
//                         MMIPUB_CloseWaitWin(MMISMS_TIMERMSG_WAIT_WIN_ID);
//                         MMISMS_SetDeliverResendFlag(FALSE);
//                         SCI_TRACE_LOW("MMISMS: MMISMS_HandleUpdateSMSCnf %d", __LINE__);
//                         MMISMS_ClearOperInfo();
//                         MMISMS_DeleteSendList();
//                     }
                }
                else
                {
#ifdef MMI_TIMERMSG_SUPPORT
                    BOOLEAN is_timer_msg = MMISMS_GetIsTimerMsg();
#endif
                    //删完
                    g_mmisms_global.is_wait_update = FALSE;

                    send_result = MMISMS_GetResultofSending(fail_id, &dest_count);
                    MMISMS_ClearSaveInfo();
                    MMISMS_ShowSendResult((uint8)send_result, fail_id);
                    MMIPUB_CloseWaitWin(MMISMS_TIMERMSG_WAIT_WIN_ID);
                    MMISMS_DeleteSendList();
                    MMISMS_SetDeliverResendFlag(FALSE);

#ifdef MMI_TIMERMSG_SUPPORT
                    if (!is_timer_msg)
#endif
                    {
                        MMIPUB_CloseWaitWin(MMISMS_READSMS_WAIT_WIN_ID);
#ifdef MMIMMS_SMS_IN_1_SUPPORT 
                        //MMIAPIMMS_CloseEditWin();
#else
                        //MMK_CloseWin(MMISMS_EDIT_SMS_WIN_ID);
#endif
                        MMIPUB_CloseWaitWin(MMISMS_SAVEMSG_ANIM_WIN_ID);                    
                    }
                }
            }
            
            result = TRUE;
        }
        else
        {
#ifdef MMI_TIMERMSG_SUPPORT
            MMISMS_TIMERMSG_CheckTimerMSG(TRUE);
#endif
            result = FALSE;
        }
        break;
        
    case MMISMS_READ_SMS:    // update SMS status from unread to read
        result = FALSE;
        break;

    default:
        result = FALSE;
        break;
    }
    
    if (!result)
    {
        //SCI_TRACE_LOW:"MMISMS: MMISMS_HandleUpdateSMSCnf sig_ptr->record_id = %d, sig_ptr->storage = %d, oper_status = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_652_112_2_18_2_52_35_33,(uint8*)"ddd", sig_ptr->record_id, sig_ptr->storage, oper_status);
        if (g_mmisms_global.update_status_num > 0)
        {
            // from unread to read
            g_mmisms_global.update_status_num--;
        }
    }

    if (MMK_IsOpenWin(MMISMS_MEMFULL_ALERT_USER_WIN_ID))
    {
        if (!MMIAPISMS_IsSMSFull())
        {
            MMK_CloseWin(MMISMS_MEMFULL_ALERT_USER_WIN_ID);
        }
    }

    MAIN_SetMsgState(TRUE);

#ifdef PDA_UI_DROPDOWN_WIN
    if (!MMIAPISMS_IsSMSFull())
    {
        MMIDROPDOWNWIN_DelNotifyRecord(DROPDOWN_NOTIFY_MESSAGE_FULL);
    }    
#endif
}

/*****************************************************************************/
//     Description : DelPushCbf
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL int32 DelPushCbf(BOOLEAN result,void *param)
{
    uint32 record_id = (uint32)(param);
    MMIAPISMS_DeleteMsgInorder(MMISMS_TYPE_WAP_PUSH,record_id);
	if (MMK_IsOpenWin(MMICMSBRW_SHOWPUSHMSG_WIN_ID))
	{
		MMK_CloseWin(MMICMSBRW_SHOWPUSHMSG_WIN_ID);
	}
    MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
    MMISMS_HandleOrderChangeCallBack();
    return 0;    
}

/*****************************************************************************/
//     Description : DelOtaCbf
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
LOCAL int32 DelOtaCbf(BOOLEAN result,void *param)
{
    uint32 record_id = (uint32)(param);
    MMIAPISMS_DeleteMsgInorder(MMISMS_TYPE_WAP_OTA,record_id);
	if (MMK_IsOpenWin(MMIOTA_CONFIG_INFO_DETAIL_WIN_ID))
	{
		MMK_CloseWin(MMIOTA_CONFIG_INFO_DETAIL_WIN_ID);
	}
    MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);    
    return 0;    
}
#ifdef PUSH_EMAIL_3_IN_1_SUPPORT//3 in 1 support
/*****************************************************************************/
//     Description : DelMailCbf
//    Global resource dependence : none
//  Author:sun
//    Note:
/*****************************************************************************/
PUBLIC void Mail_DelMailCbf(BOOLEAN result,int param)
{

    int record_id = param;
    MMIAPISMS_DeleteMsgInorder(MMISMS_TYPE_MAIL,record_id);
    //如果查看邮件窗口打开，则关闭该窗口    
	if (MMK_IsOpenWin(MMIOTA_CONFIG_INFO_DETAIL_WIN_ID))
	{
		MMK_CloseWin(MMIOTA_CONFIG_INFO_DETAIL_WIN_ID);
	}
    MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);    

}
#endif//PUSH_EMAIL_3_IN_1_SUPPORT


/*****************************************************************************/
//     Description : to delete the SMS box called by window
//    Global resource dependence : g_mmisms_global
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC MMISMS_OPER_ERR_E MMISMS_AppDeleteMsgBox(
                                                MMISMS_BOX_TYPE_E    type,    //IN:
                                                uint16* msg_type,  //out
                                                MMISMS_FOLDER_TYPE_E folder_type
                                                )
{
    MMISMS_OPER_ERR_E ret_val = MMISMS_EMPTY_BOX;
    MMISMS_OPER_STATUS_E oper_status = MMISMS_NO_OPER;
    BOOLEAN is_right = FALSE;
    MMISMS_ORDER_ID_T first_pos = PNULL;
    
    g_mmisms_global.operation.cur_order_index = 0;
    SCI_MEMSET(g_mmisms_global.operation.cur_order_id_arr, 0, 
        sizeof(g_mmisms_global.operation.cur_order_id_arr)); 
    
    //SCI_TRACE_LOW:"MMISMS: MMISMS_AppDeleteMsgBox type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_750_112_2_18_2_52_35_34,(uint8*)"d", type);

    if(MMISMS_FOLDER_SECURITY == folder_type)
    {
        oper_status = MMISMS_MOVE_SMS_TO_SECURITY;
    }
    else
    {
        oper_status = MMISMS_DELETE_MSG;
    }

    MMISMS_SetOperStatus(oper_status);
    
    switch (type)        
    {
    case MMISMS_BOX_SENDSUCC://删除已发信箱
        first_pos = MMISMS_FindValidMoSMSForDelAll(type);
        break;
        
    case MMISMS_BOX_SENDFAIL://删除发件箱
        first_pos = MMISMS_FindValidMoSMSForDelAll(type);
        break;
        
    case MMISMS_BOX_NOSEND://删除草稿箱
        first_pos = MMISMS_FindValidMoSMSForDelAll(type);
        break;
        
    case MMISMS_BOX_MT://删除收件箱
        first_pos = MMISMS_FindValidMTSMSForDelAll();
        break;
#if defined (MMI_SMS_SECURITYBOX_SUPPORT)
    case MMISMS_BOX_SECURITY://删除安全信箱
        first_pos = MMISMS_FindValidSecuritySMSForDelAll();
        break;        
#endif
#ifdef MMI_SMS_CHAT_SUPPORT
    case MMISMS_BOX_CHAT:
        first_pos = MMISMS_FindValidChatSMSForDelAll();
        break;
#endif

    default:
        break;
    }
    
    // the index is valid
    if (PNULL != first_pos)
    {
        first_pos->flag.is_marked = FALSE;// 找到后将该标记清除，避免反复找到同一条信息
        *msg_type = first_pos->flag.msg_type;
        switch (first_pos->flag.msg_type)
        {
        case MMISMS_TYPE_SMS:
            is_right = MMISMS_DeleteReqToMN( 
                type, 
                first_pos, 
                oper_status 
                );
            ret_val  = (is_right) ? MMISMS_NO_ERR : MMISMS_DEL_FAIL;
            if (MMISMS_SENDSTATUS_WAITING == first_pos->backsend_status)
            {
                //SCI_TRACE_LOW:"MMISMS: MMISMS_DeleteInfoInSendListById"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_811_112_2_18_2_52_36_35,(uint8*)"");
                MMISMS_DeleteInfoInSendListById(first_pos->record_id, first_pos->flag.storage);
            }
            MMISMS_CLEAR_OPER(ret_val);
            break;
            
        case MMISMS_TYPE_MMS:
            if (MMIAPIUDISK_UdiskIsRun())
            {
                MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
            }
            else
            {
                if(MMIAPIMMS_DeleteMMS(first_pos->record_id,MMISMS_DELETE_ANIM_WIN_ID))
                {
                    MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
                }                
            }
            ret_val = MMISMS_NO_ERR;
            break;
            
        case MMISMS_TYPE_MMS_OTA:
        case MMISMS_TYPE_WAP_OTA:  
            if (MMIAPIUDISK_UdiskIsRun())
            {
                MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
            }
            else
            {
                MMIAPIOTA_DelOTAByID(first_pos->record_id, DelOtaCbf, (void *)first_pos->record_id);
            }
            ret_val = MMISMS_NO_ERR;
            break;
            
        case MMISMS_TYPE_WAP_PUSH:
            if (MMIAPIUDISK_UdiskIsRun())
            {
                MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
            }
            else
            {
                MMIAPIOTA_DelPushByID(first_pos->record_id, DelPushCbf, (void *)first_pos->record_id);
            }
            ret_val = MMISMS_NO_ERR;            
            break;
            
#ifdef PUSH_EMAIL_3_IN_1_SUPPORT//3 in 1 support
        case MMISMS_TYPE_MAIL:
            if (MMIAPIUDISK_UdiskIsRun())
            {
                MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
            }
            else
            {
                if(Mail_DelMailByMsgId(first_pos->record_id) == TRUE)
                {
                    Mail_DelMailCbf(TRUE, first_pos->record_id);
                }
                else
                {
                    MMK_PostMsg(MMISMS_DELETE_ANIM_WIN_ID, MMISMS_MSG_DELETE_NEXT, PNULL,0);
                }
            }
            ret_val = MMISMS_NO_ERR;            
            break;
#endif//PUSH_EMAIL_3_IN_1_SUPPORT
        default: 
            ret_val = MMISMS_DEL_FAIL;            
            break;            
        }        
    }
    else
    {
        //SCI_TRACE_LOW:"MMISMS: MMISMS_AppDeleteMsgBox: The SmsBox is empty!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_884_112_2_18_2_52_36_36,(uint8*)"");
        ret_val = MMISMS_FINISH_OPER;
    }
    
    return (ret_val);
}

/*****************************************************************************/
//     Description : to delete the pointed SMS
//    Global resource dependence : none
//  Author:louis.wei
//    Note:
/*****************************************************************************/
PUBLIC void MMISMS_DeletePointedSMS(
                                    uint16        pos_index //IN: start from 0
                                    )
{
    MMISMS_ORDER_ID_T order_id = PNULL;    
    MMISMS_ORDER_ID_T cur_order_id = PNULL;
    int32 i = 0;
    
    //根据pos_index获得当前的order_id
    order_id = MMISMS_GetLinkedArrayPtrIdInList( g_mmisms_global.operation.cur_type, pos_index);
    if(PNULL == order_id)
    {
        return;
    }
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
    
    //SCI_TRACE_LOW:"MMISMS: MMISMS_DeletePointedSMS pos_index = %d, order_id = %d, type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_929_112_2_18_2_52_36_37,(uint8*)"ddd",pos_index, order_id, g_mmisms_global.operation.cur_type);
    
    //开始删除
    MMISMS_DeleteReqToMN(g_mmisms_global.operation.cur_type, MMISMS_GetCurOperationOrderId(), MMISMS_DELETE_MSG);    
}

/*****************************************************************************/
//     Description : to handle the signal APP_MN_UPDATE_SMS_STATUS_CNF
//    Global resource dependence : g_mmisms_global
//  Author:fengming.huang
//    Note: handle the normal move operation
/*****************************************************************************/
LOCAL void HandleMoveDeleteCnf( 
                           MN_SMS_CAUSE_E    cause    //IN:
                           )
{
    int32 record_id = 0;
    MMISMS_ORDER_ID_T order_id = MMISMS_GetCurOperationOrderId();
    ERR_MNSMS_CODE_E err_code = ERR_MNSMS_NONE;
    uint8 index = g_mmisms_global.operation.cur_order_index;
    
    //SCI_TRACE_LOW:"MMISMS: HandleMoveCnf cause = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_948_112_2_18_2_52_36_38,(uint8*)"d", cause);
    
    if (MN_SMS_OPERATE_SUCCESS == cause)
    {
        //SCI_TRACE_LOW:"MMISMS: HandleMoveCnf move_all_flag = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISMS_DELETE_952_112_2_18_2_52_36_39,(uint8*)"d", MMISMS_GetMoveAllFlag());
        // 删除SIM短信按取消，需将已经删除的短信重新保存
        if (MMISMS_GetMoveSingleFlag())
        {
            //storage = (MN_SMS_STORAGE_E)order_id->flag.storage;
            //MMISMS_MOVESaveSms(order_id);
#ifdef MMI_TIMERMSG_SUPPORT
            MMISMS_TIMERMSG_DeleteTimerMsg(g_mmisms_global.operation.cur_order_id_arr[g_mmisms_global.operation.cur_order_index]);
#endif

            MMISMS_SetCurMsgLocked(FALSE);
            record_id = g_mmisms_global.operation.cur_order_id_arr[g_mmisms_global.operation.cur_order_index]->record_id;

            //order_id->flag.storage = storage; 
            MMISMS_DelSmsInfoInOrder(order_id);
            //order_id->flag.storage = MN_SMS_STORAGE_ME;

            index++;
            if(MMISMS_SPLIT_MAX_NUM <= index)
            {
                order_id = PNULL;
            }
            else
            {
                g_mmisms_global.operation.cur_order_index = index;
                order_id = MMISMS_GetCurOperationOrderId();
            }
#ifdef MMI_SMS_CHAT_SUPPORT
            if(MMK_IsOpenWin(MMISMS_SHOW_CHAT_SMS_WIN_ID) && (PNULL == order_id)) //PNULL 表明当前是最后一条 
            {
               MMK_SendMsg(MMISMS_SHOW_CHAT_SMS_WIN_ID, 
                        MMISMS_MSG_UPDATELIST, 
                        PNULL);    //通知相应的窗口，更新list内容                    
            }
#endif            
            if(PNULL != order_id)
            {
                err_code = MNSMS_UpdateSmsStatusEx(
                                                    order_id->flag.dual_sys, 
                                                    order_id->flag.storage, 
                                                    order_id->record_id, 
                                                    MN_SMS_FREE_SPACE);                
            }
            else
            {
                MMISMS_SetMoveSingleFlag(FALSE);
                
                if(MMISMS_GetMoveAllFlag())
                {
                    MMIAPICOM_SendSignalToMMITask(MMISMS_MSG_MOVE_NEXT);   
                }
                else
                {
                    if(MMK_IsOpenWin(MMISMS_MOVE_ANIM_WIN_ID))
                    {
                        MMK_CloseWin(MMISMS_MOVE_ANIM_WIN_ID);
                    }
                }
            }
        }
        else
        {
            MMISMS_SetMoveAllFlag(FALSE);
            //全部删除完毕，将未处理链表中的sms插入到相应的链表中
            MMISMS_InsertUnhandledSmsToOrder();
            MMISMS_ReadMMSPushFromUnhandledList(); //读取mms push 消息并传给mms模块
            MMISMS_HandleOrderChangeCallBack(); //删除完毕刷新list
        }
    }
    else
    {
        if(MMISMS_GetMoveSingleFlag())
        {
            MMISMS_SetMoveSingleFlag(FALSE);

            if(MMISMS_GetMoveAllFlag())
            {
                MMIAPICOM_SendSignalToMMITask(MMISMS_MSG_MOVE_NEXT);   
            }
            else
            {
                if(MMK_IsOpenWin(MMISMS_MOVE_ANIM_WIN_ID))
                {
                    MMK_CloseWin(MMISMS_MOVE_ANIM_WIN_ID);
                }
            }
        }
        else
        {
            MMISMS_ClearOperInfo();
            // 提示用户删除失败
            if(MN_SMS_SIM_SAVE_STATUS_FREE == cause)
            {
                MMIPUB_OpenAlertFailWin(TXT_SMS_READ_ERR);
            }
            else
            {
                MMIPUB_OpenAlertFailWin(TXT_ERROR);
            }
            
            //全部删除完毕，将未处理链表中的sms插入到相应的链表中
            MMISMS_InsertUnhandledSmsToOrder();
            MMISMS_ReadMMSPushFromUnhandledList(); //读取mms push 消息并传给mms模块
            MMISMS_SetMoveAllFlag(FALSE);
            MMISMS_HandleOrderChangeCallBack(); //删除完毕更新list

            MMIPUB_CloseWaitWin(MMISMS_DELETE_ANIM_WIN_ID);
        }
    }
}

