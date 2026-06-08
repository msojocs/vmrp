

#ifdef __MMI_DUAL_SIM_MASTER__
extern MMI_BOOL mmi_frm_sms_record_is_slave_sms(U16 box, U16 index);
static U16 dsm_init_sms_show_list_index(U8 box_type, E_MTPNP_AD_FILTER_RULE rule);
#endif



/****************************************************************************
函数名:void mr_send_mode1_message_response(void* number, module_type mod, U16 result)
描  述:短信发送的结构会通过这个函数中来处理
参  数:我们主要关注result 这个值
返  回:无
****************************************************************************/
static void mr_send_mode1_message_response(void* number, module_type mod, U16 result)
{
	if(result == 0)
	{
		mr_event(MR_SMS_RESULT,MR_SUCCESS,(int32)number);
	}
	else
	{
		mr_event(MR_SMS_RESULT,MR_FAILED,(int32)number);
	}
}


/****************************************************************************
函数名:void mr_send_mode2_message_response(void* number, module_type mod, U16 result)
描  述:短信发送的结构会通过这个函数中来处理
参  数:我们主要关注result 这个值
返  回:无
****************************************************************************/
static void mr_send_mode2_message_response(void* number, module_type mod, U16 result)
{
	U16 currentSCRID;

#ifdef MMI_ON_HARDWARE_P
	kal_prompt_trace(MOD_MMI,"mr_send_message_response result=%d",result);
#endif

        switch(result)
	{
		case MMI_FRM_SMS_OK:
#if(MTK_VERSION >=0x0924)
			DisplayPopup ((PU8)GetString(STR_GLOBAL_SENT), IMG_SEND_SUCCESS_PIC_MSG, 1, MESSAGES_POPUP_TIME_OUT, (U8)SUCCESS_TONE);
#else
			DisplayPopup ((PU8)GetString(STR_SMS_SEND_SUCCESS), IMG_SEND_SUCCESS_PIC_MSG, 1, MESSAGES_POPUP_TIME_OUT, (U8)SUCCESS_TONE);
#endif
		break;
		case MMI_FRM_SMS_ABORT:
#if(MTK_VERSION >=0x0924)	
			DisplayPopup ((PU8)GetString (STR_GLOBAL_ABORT), IMG_GLOBAL_ACTIVATED, 1, MESSAGES_POPUP_TIME_OUT, (U8)SUCCESS_TONE);
#else
			DisplayPopup ((PU8)GetString (STR_ABORT_SENDING_SMS), IMG_GLOBAL_ACTIVATED, 1, MESSAGES_POPUP_TIME_OUT, (U8)SUCCESS_TONE);
#endif
		break;
		case MMI_FRM_SMS_NOTREADY:
			DisplayPopup ((PU8)GetString(STR_SMS_MSG_NOT_READY_YET), IMG_SEND_FAIL_PIC_MSG, 1, MESSAGES_POPUP_TIME_OUT, (U8)ERROR_TONE);			
		break;
		case MMI_FRM_SMS_RETURN:
		case MMI_FRM_SMS_END:
		break;
		default:
			DisplayPopup ((PU8)GetString(STR_SMS_SEND_FAILED), IMG_SEND_FAIL_PIC_MSG, 1, MESSAGES_POPUP_TIME_OUT, (U8)ERROR_TONE);
		break;
	}
		
	mmi_frm_sms_delete_screen_history();

       ReleaseEMSEditBuffer();
       GetEMSDataForEdit (0, 1);
	currentSCRID = GetActiveScreenId();

	if((currentSCRID == SCR_ID_MSG_SENDING) || (currentSCRID == SCR_ID_MSG_PROCESSING))
		GoBackHistory();

	AlmEnableSPOF();

	if(result == 0)
	{
		mr_event(MR_SMS_RESULT,MR_SUCCESS,(int32)number);
	}
	else
	{
		mr_event(MR_SMS_RESULT,MR_FAILED,(int32)number);
	}
}


int32 mr_sms_get_sms_count(int32 param)
{
	int count = 0;
	int type;
	
	if(param == MR_SMS_INBOX)
		type = MMI_FRM_SMS_APP_INBOX;
	else if(param == MR_SMS_OUTBOX)
		type = MMI_FRM_SMS_APP_OUTBOX;
	else if(param == MR_SMS_DRAFTS)
		type = MMI_FRM_SMS_APP_DRAFTS;
	else
		return MR_FAILED;

#ifdef __MMI_DUAL_SIM_MASTER__
	if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_MASTER_SIM)
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_MASTER);
		count = dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_MASTER);
	}
	else
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_SLAVE);
		count = dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_SLAVE);
	}

#else
	count = mmi_frm_sms_get_sms_list_size(type);
#endif			
	return count + MR_PLAT_VALUE_BASE;
}


static void dsm_msg_get_memory_status_rsp(void *data, module_type mod, U16 result)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	/* if incoming call ringing or alarm playing, do not show response result in case interrupt current screen */
	kal_prompt_trace(MOD_MMI,"-------------dsm_msg_get_memory_status_rsp");
	
	if (mmi_msg_check_interrupt() == TRUE)
	{
		return;
	}
	
	if(mr_app_get_state() == DSM_STOP)
	{
		return;
	}

	if (result == MMI_FRM_SMS_OK)
	{
		msg_mem_status_struct *memorystatus = (msg_mem_status_struct*) data;
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE,memorystatus->meTotal);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_SIM_SIZE,memorystatus->simTotal);
	}
	else
	{
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE,-1);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_SIM_SIZE,-1);
	}
	
	//AlmEnableExpiryHandler(); /*这句话无意义，而且会造成死机，所以注释掉*/
}


static void dsm_msg_get_msg_rsp(void* data, module_type mod, U16 result)
{
	//U8 type=*(U8*)data;
	EMSData*	pEms;
	char *tmp = NULL;
	int len = 0;
	
	GetEMSDataForView (&pEms, 0);	

	if(mr_app_get_state() != DSM_RUN)
		return;

	len = mr_str_wstrlen((char *)(pEms->textBuffer))+2;

	kal_prompt_trace(MOD_MMI,"---------dsm_msg_get_msg_rsp len=%d",len);

	tmp = med_alloc_ext_mem(len);

	if(tmp == NULL)
	{
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_MSG_CONTENT,NULL);
		return;
	}

	memset(tmp,0,len);
	memcpy(tmp,pEms->textBuffer,len-2);
	
	mr_str_convert_endian(tmp);
	
	mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_MSG_CONTENT,(int32)tmp);

	med_free_ext_mem((void **)&tmp);
}


static int32 dsm_msg_get_msg_req (U8 type, U16 index)
{
	/* disallow re-entering SMS application when there is a pending SMS job running in the background */
	if(mmi_frm_sms_check_action_pending())
	{
		return MR_FAILED;
	}
	
#if (MTK_VERSION >= 0x06B)
	mmi_frm_sms_read_sms(dsm_msg_get_msg_rsp, MOD_MMI, type, index, MMI_TRUE);
#else
	mmi_frm_sms_read_sms(dsm_msg_get_msg_rsp, MOD_MMI, type, index);
#endif
	return MR_SUCCESS;
}


int32 dsm_get_sms_info(T_DSM_GET_SMS_INFO_REQ *pReq)
{
	U8 type;
	U8 ret;
	U8 *addr;
	int index;
	int count=0;

	
	if(pReq->type == MR_SMS_INBOX)
		type = MMI_FRM_SMS_APP_INBOX;
	else if(pReq->type == MR_SMS_OUTBOX)
		type = MMI_FRM_SMS_APP_OUTBOX;
	else
		return MR_FAILED;

	if(mmi_frm_sms_is_in_sim_refresh())
		return MR_FAILED;

#ifdef __MMI_DUAL_SIM_MASTER__
	if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_MASTER_SIM)
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_MASTER);
		count =dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_MASTER);
	}
	else
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_SLAVE);
		count = dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_SLAVE);
	}
	
	if((count <=0)||(pReq->index>= count))
		return MR_FAILED;
	
	index = mmi_frm_sms_show_list_index[pReq->index];
#else
	index = pReq->index;
#endif
	memset(&gdsmSMSInfo,0,sizeof(gdsmSMSInfo));
	
	if(type == MMI_FRM_SMS_APP_INBOX)
	{
		ret = mmi_frm_sms_get_status(type,index);
		
		if(ret == MMI_FRM_SMS_APP_UNREAD)
			gdsmSMSInfo.sms_status = MR_SMS_UNREAD;
		else 
			gdsmSMSInfo.sms_status = MR_SMS_READED;
	}
	else
	{
		if ((mmi_frm_sms_get_status(MMI_FRM_SMS_APP_OUTBOX, (U16) index) & 0x0f) == MMI_FRM_SMS_APP_OUTBOX)
		{
			gdsmSMSInfo.sms_status= MR_SMS_SENT;
		}
		else
		{
			gdsmSMSInfo.sms_status = MR_SMS_UNSENT;
		}
	}

	if(mmi_frm_sms_get_storage(type, index)== SMSAL_ME)
		gdsmSMSInfo.sms_storage = MR_SMS_STORAGE_ME;
	else
		gdsmSMSInfo.sms_storage = MR_SMS_STORAGE_SIM;

	addr = mmi_frm_sms_get_address(type,index);

	strncpy((char*)gdsmSMSInfo.num, (const char*)addr, sizeof(gdsmSMSInfo.num)-1);
	
	if(type == MMI_FRM_SMS_APP_INBOX)
	{
		memcpy(gdsmSMSInfo.t_stamp,mmi_frm_sms_get_timestamp(type, index),6);
	}
	return MR_SUCCESS;
}


static void dsm_frm_sms_delete_rsp(void *data, module_type type, U16 result)
{
	if(mr_app_get_state() == DSM_STOP)
		return;
	
	if(result == MMI_FRM_SMS_OK)
	{
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_DELETE, MR_SUCCESS);
	}
	else
	{
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_DELETE, MR_FAILED);
	}
}

int32 mr_sms_delete_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	U8 type;
	int index;
	int count;

	T_DSM_DELETE_SMS_REQ *pReq = (T_DSM_DELETE_SMS_REQ*)input;
	if((input == NULL)||(input_len < sizeof(T_DSM_DELETE_SMS_REQ)))
		return MR_FAILED;
			
	if(pReq->type == MR_SMS_INBOX)
		type = MMI_FRM_SMS_APP_INBOX;
	else if(pReq->type == MR_SMS_OUTBOX)
		type = MMI_FRM_SMS_APP_OUTBOX;
	else
		return MR_FAILED;
	
#ifdef __MMI_DUAL_SIM_MASTER__
	if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_MASTER_SIM)
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_MASTER);
		count = dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_MASTER);
	}
	else
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_SLAVE);
		count = dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_SLAVE);
	}
	
	if((count <=0)||(pReq->index>= count))
		return MR_FAILED;
	
	index = mmi_frm_sms_show_list_index[pReq->index];
#else
	index = pReq->index;
#endif
	mmi_frm_sms_delete_sms(dsm_frm_sms_delete_rsp, MOD_MMI, type, index);
	return MR_WAITING;
}


int32 mr_sms_read_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	U8 type =0;
	int index =0;
	int count =0;
	T_DSM_GET_SMS_INFO_REQ *pReq = (T_DSM_GET_SMS_INFO_REQ*)input;
	if(pReq == NULL)
		return MR_FAILED;
	
	switch(pReq->type)
	{
		case MR_SMS_INBOX:
			type = MMI_FRM_SMS_APP_INBOX;
			break;
		case MR_SMS_OUTBOX:
			type = MMI_FRM_SMS_APP_OUTBOX;
			break;
		default:
			return MR_FAILED;
	}
		
	if(pReq->index <0 || pReq->index>255)  //pls check the max index for message
	{
		return MR_FAILED;
	}

#ifdef __MMI_DUAL_SIM_MASTER__
	if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_MASTER_SIM)
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_MASTER);
		count =dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_MASTER);
	}
	else
	{
		MTPNP_AD_SMS_Set_Filter_Rule(MTPNP_AD_FILTER_SLAVE);
		count = dsm_init_sms_show_list_index(type, MTPNP_AD_FILTER_SLAVE);
	}
	
	if((count <=0)||(pReq->index>= count))
		return MR_FAILED;
	
	MTPNP_PFAL_SMS_Set_Current_Index(index);
	index = mmi_frm_sms_show_list_index[pReq->index];
#else
	index = pReq->index;
#endif		

	if(MR_FAILED == dsm_msg_get_msg_req(type, index))
	{
		return MR_FAILED;
	}
	return MR_WAITING;
}	




#ifdef __MMI_DUAL_SIM_MASTER__
#include "mtpnp_pfal_master_if.h"
static void dsm_frm_sms_get_memory_status_rsp(void* inMsg)
{
	ST_MTPNP_AD_SMS_GET_MSG_NUM_RSP *msgRsp = (ST_MTPNP_AD_SMS_GET_MSG_NUM_RSP*)inMsg;
	extern void MTPNP_PFAL_Remove_Event_Handler(MTPNP_UINT16 eventID);

       MTPNP_PFAL_Remove_Event_Handler(MTPNP_PFAL_MSG_SMS_GET_MSG_NUM_RSP);

	if(mr_app_get_state() == DSM_STOP)
		return;
	
	if(msgRsp->result==TRUE)
	{
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE,msgRsp->total_me_num);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_SIM_SIZE,msgRsp->total_sim_num);
	}
	else
	{
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE,-1);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_SIM_SIZE,-1);
	}
}
#endif


int32 mr_sms_get_memory_size(int32 param)
{
#ifdef __MMI_DUAL_SIM_MASTER__			
	if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_MASTER_SIM)
	{
		mmi_frm_sms_get_memory_status(dsm_msg_get_memory_status_rsp, MOD_MMI);
	}
	else
	{
		MTPNP_PFAL_Register_Event_Handler(MTPNP_PFAL_MSG_SMS_GET_MSG_NUM_RSP, dsm_frm_sms_get_memory_status_rsp);
		MTPNP_AD_SMS_Get_MSG_Num_Req();
	}

#else				
	mmi_frm_sms_get_memory_status(dsm_msg_get_memory_status_rsp, MOD_MMI);
#endif	

	return MR_WAITING;
}


static void dsm_frm_sms_get_sc_addr_rsp(void *inMsg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	#if(MTK_VERSION >= 0x09B1012)
        mmi_sms_get_profile_params_rsp_struct *msgRsp  = (mmi_sms_get_profile_params_rsp_struct*) inMsg;
  	#else
	MMI_FRM_SMS_GET_PROFILE_PARAMS_RSP_STRUCT *msgRsp = (MMI_FRM_SMS_GET_PROFILE_PARAMS_RSP_STRUCT*) inMsg;
	#endif

	char scTmp[22];
		
	memset(scTmp,0,sizeof(scTmp));
	if(mr_app_get_state() == DSM_STOP)
		return;
		
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	ClearProtocolEventHandler(PRT_MSG_ID_MMI_SMS_GET_PROFILE_PARAMS_RSP);
	
	if (msgRsp->result == TRUE)
	{
		if (msgRsp->sc_addr.length == 0)
		{
			mr_event(MR_SMS_GET_SC,(int32)scTmp,0);
		}
		else
		{
			strncpy(scTmp,(char *)(msgRsp->sc_addr.number),21);
			mr_event(MR_SMS_GET_SC,(int32)scTmp,strlen(scTmp)+1);
		}
	}
	else
	{
		mr_event(MR_SMS_GET_SC,0,0);
	}
}


static void dsm_frm_sms_get_sc_addr_req(void *inMsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    #if(MTK_VERSION >= 0x09B1012)
     mmi_sms_get_profile_params_req_struct  *msgReq;
  
    #else
      MMI_FRM_SMS_GET_PROFILE_PARAMS_REQ_STRUCT *msgReq;
    #endif
    U8 index = *(U8*) inMsg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    #if(MTK_VERSION >= 0x09B1012)
    msgReq = (mmi_sms_get_profile_params_req_struct*) OslConstructDataPtr(sizeof(*msgReq));
    #else
    msgReq = (MMI_FRM_SMS_GET_PROFILE_PARAMS_REQ_STRUCT*) OslConstructDataPtr(sizeof(*msgReq));
    #endif
    msgReq->profile_no = index;
	ClearProtocolEventHandler(PRT_MSG_ID_MMI_SMS_GET_PROFILE_PARAMS_RSP);
    SetProtocolEventHandler(dsm_frm_sms_get_sc_addr_rsp, PRT_MSG_ID_MMI_SMS_GET_PROFILE_PARAMS_RSP);
#if(MTK_VERSION >= 0x09B1012)
	srv_sms_disp_send_msg_req(PRT_MSG_ID_MMI_SMS_GET_PROFILE_PARAMS_REQ, SRV_SMS_SIM_1, (void*) msgReq);
#else
	mmi_frm_sms_send_message(MOD_MMI,MOD_L4C,0, PRT_MSG_ID_MMI_SMS_GET_PROFILE_PARAMS_REQ,(oslParaType*) msgReq,NULL);
#endif
}

#ifdef __MMI_DUAL_SIM_MASTER__
static void dsm_frm_sms_get_card2_sc_addr_rsp(void *info);
#endif

int32 mr_sms_get_sc_req(int32 param)
{
	U8 nvramActiveProfileIndex = 0;
	S16 errorCode = 0;

#ifdef __MMI_DUAL_SIM_MASTER__
	if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_SLAVE_SIM)
	{
		kal_prompt_trace(MOD_MMI,"DSM_SLAVE_SIM");
		MTPNP_AD_Set_Channel(MTPNP_AD_SMS_CHANNEL, MTPNP_AD_CHANNEL_SLAVE);
		ReadValue(NVRAM_SLAVE_ACTIVE_PROFILE_IDX, &nvramActiveProfileIndex, DS_BYTE, &errorCode);
		MTPNP_PFAL_Register_Event_Handler(MTPNP_PFAL_MSG_SMS_GET_PROFILE_PARAMS_RSP, dsm_frm_sms_get_card2_sc_addr_rsp);
		kal_prompt_trace(MOD_MMI,"nvramActiveProfileIndex=%d",nvramActiveProfileIndex);
		MTPNP_AD_SMS_Get_Profile_Params_Req(nvramActiveProfileIndex);
	}
	else
#endif
	{
#ifdef __MMI_DUAL_SIM_MASTER__			
		MTPNP_AD_Set_Channel(MTPNP_AD_SMS_CHANNEL, MTPNP_AD_CHANNEL_MASTER);
#endif
		kal_prompt_trace(MOD_MMI,"DSM_MASTER_SIM");
		ReadValue(NVRAM_SMS_ACTIVE_PROFILE_INDEX, &nvramActiveProfileIndex, DS_BYTE, &errorCode);
		dsm_frm_sms_get_sc_addr_req(&nvramActiveProfileIndex);
	}

	return MR_WAITING;
}



int dsmReciveMsg(void *data)
{
#ifndef WIN32
	int32 dsmLen = 0;
	int32 dsmType = -1;
	mmi_sms_deliver_msg_ind_struct *msgInd = (mmi_sms_deliver_msg_ind_struct *)data;
	
       if(msgInd->dcs == SMSAL_8BIT_DCS||msgInd->dcs == SMSAL_DEFAULT_DCS)
       {
		dsmLen = msgInd->no_msg_data + 1;
		dsmType = MR_ENCODE_ASCII;
       }
	else if(msgInd->dcs == SMSAL_UCS2_DCS)
	{
		dsmLen = msgInd->no_msg_data + 2;
		dsmType = MR_ENCODE_UNICODE;
	}

	if(dsmType != -1)
	{
              uint8 dsmNumber[MAX_CC_ADDR_LEN+1] = {0};
	       uint8 *dsmData = (uint8 *)OslMalloc(dsmLen);

		if(dsmData != NULL)
		{
		       memset(dsmData,0,dsmLen);
			   
		       memcpy(dsmNumber,msgInd->oa_number.number,MAX_CC_ADDR_LEN);
		       memcpy(dsmData,msgInd->msg_data,msgInd->no_msg_data);
			
			if(mr_smsIndiactionEx(dsmData,msgInd->no_msg_data,dsmNumber,dsmType) != MR_IGNORE)
			{
				mmi_frm_sms_delete_sms(NULL, MOD_MMI, MMI_FRM_SMS_APP_AWAITS, msgInd->index);
				OslMfree(dsmData);
				return 1;
				
			}
			OslMfree(dsmData);
			return 0;
		}
       }
#endif	
	return 0;
}


#ifdef __MMI_DUAL_SIM_MASTER__
int dsmReciveMsgSlave(void *data)
{
#ifndef WIN32
	int32 dsmLen = 0;
	int32 dsmType = -1;
	ST_MTPNP_AD_DELIVER_SMS *msgInd = (ST_MTPNP_AD_DELIVER_SMS *)data;
	
       if(msgInd->dcs == MTPNP_CODING_SCHEME_ASCII||msgInd->dcs == MTPNP_CODING_SCHEME_DEFAULT)
       {
		dsmLen = msgInd->no_msg_data + 1;
		dsmType = MR_ENCODE_ASCII;
       }
	else if(msgInd->dcs == MTPNP_CODING_SCHEME_UCS2)
	{
		dsmLen = msgInd->no_msg_data + 2;
		dsmType = MR_ENCODE_UNICODE;
	}

	if(dsmType != -1)
	{
              uint8 dsmNumber[MAX_CC_ADDR_LEN+1] = {0};
	       uint8 *dsmData = (uint8 *)OslMalloc(dsmLen);

		if(dsmData != NULL)
		{
		       memset(dsmData,0,dsmLen);
			   
		       memcpy(dsmNumber,msgInd->oa_number.number,MAX_CC_ADDR_LEN);
		       memcpy(dsmData,msgInd->msg_data,msgInd->no_msg_data);
			
			if(mr_smsIndiactionEx(dsmData,msgInd->no_msg_data,dsmNumber,dsmType) != MR_IGNORE)
			{
				mmi_frm_sms_delete_sms(NULL, MOD_MMI, MMI_FRM_SMS_APP_AWAITS, msgInd->index);
				OslMfree(dsmData);
				return 1;
				
			}
			OslMfree(dsmData);
			return 0;
		}
       }
#endif	
	return 0;
}
#endif


#ifdef __MMI_DUAL_SIM_MASTER__
extern MMI_BOOL mmi_frm_sms_record_is_slave_sms(U16 box, U16 index);
static U16 dsm_init_sms_show_list_index(U8 box_type, E_MTPNP_AD_FILTER_RULE rule)
{
    U16 box_size;
    U16 show_size = 0;
    U16 i = 0;

    box_size = mmi_frm_sms_get_sms_list_size(box_type);
	
    if (box_size == MMI_FRM_SMS_INVALID_INDEX)
    {
       // return MMI_FRM_SMS_INVALID_INDEX;
	return 0;
    }

    memset((S8 *) mmi_frm_sms_show_list_index, MMI_FRM_SMS_INVALID_NUM,
           sizeof(U16) * g_frm_sms_cntx.mmi_frm_sms_msg_box_size);
    switch (rule)
    {
    case MTPNP_AD_FILTER_ALL:
        show_size = box_size;
        break;

    case MTPNP_AD_FILTER_MASTER:
        for (i = 0; i < box_size; i++)
        {
            if (!mmi_frm_sms_record_is_slave_sms(box_type, i))
            {
                mmi_frm_sms_show_list_index[show_size++] = i;
            }
        }
        break;

    case MTPNP_AD_FILTER_SLAVE:
        for (i = 0; i < box_size; i++)
        {
            if (mmi_frm_sms_record_is_slave_sms(box_type, i))
            {
                mmi_frm_sms_show_list_index[show_size++] = i;
            }
        }
        break;
    }
    PRINT_INFORMATION("[LJH]box_type=%d, box_size=%d, rule=%d, show_size=%d", box_type, box_size, rule, show_size);

    return show_size;
}
#endif


#ifdef __MMI_DUAL_SIM_MASTER__

static void dsm_frm_sms_get_card2_sc_addr_rsp(void *info)
{
	ST_MTPNP_AD_SMS_GET_PROFILE_PARAMS_RSP *msgRsp = (ST_MTPNP_AD_SMS_GET_PROFILE_PARAMS_RSP *) info;
	char scTmp[22];
	extern void MTPNP_PFAL_Remove_Event_Handler(MTPNP_UINT16 eventID);

	memset(scTmp,0,sizeof(scTmp));

	
	if(mr_app_get_state() == DSM_STOP)
		return;
		
	MTPNP_PFAL_Remove_Event_Handler(MTPNP_PFAL_MSG_SMS_GET_PROFILE_PARAMS_RSP);

	if (msgRsp->result == TRUE)
	{
	
		kal_prompt_trace(MOD_MMI,"SC=%s",msgRsp->sc_addr.number);
		
		if (msgRsp->sc_addr.length == 0)
		{
			mr_event(MR_SMS_GET_SC,(int32)scTmp,0);
		}
		else
		{
			strncpy(scTmp,(char *)(msgRsp->sc_addr.number),21);
			mr_event(MR_SMS_GET_SC,(int32)scTmp,strlen(scTmp)+1);
		}
	}
	else
	{
		mr_event(MR_SMS_GET_SC,0,0);
	}
}

#endif

#if (MTK_VERSION <= 0x0812)
#include "Mmi_msg_context.h"
extern PendingSaveSendDataStruct PendingSaveSendData;
#endif

int32 mr_sendSms(char* pNumber, char*pContent, int32 encode)
{
       int numlen = 0;

#ifdef MMI_ON_HARDWARE_P
 	kal_prompt_trace(MOD_MMI,"mr_sendSms pNumber = %s",pNumber);
#endif

	if(pNumber == NULL||pContent == NULL)
		return MR_IGNORE;
	
	if (mr_IsReadySendSms())
	{
		S8 * textbufUCS2 = NULL;
		S8 smsPhoneNumber [(MAX_DIGITS_SMS+1)*ENCODING_LENGTH] = {0};
		EMSData	* pEMS = NULL;
		byte  result;
		int len,i;

		if ((encode&0x07) == MR_ENCODE_UNICODE) //unicode
			len = mr_str_wstrlen(pContent);
		else
			len = strlen((char *)pContent);

		ReleaseEMSEditBuffer();
		pEMS = GetEMSDataForEdit (0, 1);

              if(pEMS == NULL)
			  return MR_FAILED;

		numlen = strlen(pNumber) > MAX_DIGITS_SMS?MAX_DIGITS_SMS:strlen(pNumber);

#if(MTK_VERSION < 0x0924)
		g_msg_cntx.sendSaveStatus = SEND_SAVE_NIL;
		memset (g_msg_cntx.smsPhoneNumber, 0, sizeof(g_msg_cntx.smsPhoneNumber)); 
		
		mmi_asc_n_to_ucs2((S8 *) smsPhoneNumber,( S8 *) pNumber,numlen);
		mmi_ucs2cpy(g_msg_cntx.smsPhoneNumber,(S8 *)smsPhoneNumber);
#endif

		PendingSaveSendData.totalSegments=1;
		PendingSaveSendData.mti=SMSAL_MTI_SUBMIT;
		memset(PendingSaveSendData.TPUD, 0,sizeof(PendingSaveSendData.TPUD));

		for (i=1; i<mmi_msg_get_seg_num(); i++)
		{
			PendingSaveSendData.TPUDLen[i]=0;
			PendingSaveSendData.TPUD_p[i]=NULL;
			PendingSaveSendData.l4index[i]=SMS_INVALID_INDEX;
			PendingSaveSendData.TPUD_udhi[i]= FALSE;
		}

#if(MTK_VERSION < 0x0924)
		g_msg_cntx.sendMessageCase = SEND_CASE_SEND_ONLY;
#endif

		if((encode&0x07) == MR_ENCODE_UNICODE)
		{
			textbufUCS2 = (S8 *)OslMalloc(len+2);

			if(textbufUCS2 == NULL)
				return MR_FAILED;
			
			memset((void *)textbufUCS2,0,len+2);
			memcpy((void *)textbufUCS2,(void *)pContent,len);
			mr_str_convert_endian((char *)textbufUCS2);
			result = AppendEMSString (INPUT_TYPE_ALPHANUMERIC_SENTENCECASE,
			pEMS, (U8*)textbufUCS2, SMSAL_UCS2_DCS, NULL);  
		}
		else
		{
			textbufUCS2 = (S8 *)OslMalloc((len+1)*2);

			if(textbufUCS2 == NULL)
				return MR_FAILED;
						
			memset(textbufUCS2,0,(len+1)*2);
			mmi_asc_to_ucs2(textbufUCS2,(S8 *)pContent);
			result = AppendEMSString (INPUT_TYPE_ALPHANUMERIC_SENTENCECASE,
			pEMS, (U8*)textbufUCS2, SMSAL_DEFAULT_DCS, NULL);  
		}
		
		OslMfree(textbufUCS2);

		if(result)
		{
			mmi_frm_sms_send_struct * sendData=OslMalloc(sizeof(mmi_frm_sms_send_struct));

			if(sendData == NULL)
			 	return MR_FAILED;
					 
			memset((S8*)sendData, 0, sizeof(mmi_frm_sms_send_struct));
			memset(sendData->number, 0, MAX_DIGITS_SMS);	

			memcpy(sendData->number, pNumber,numlen);

			//gDsmSendMessage = 0;
			//IsDsmCloseSmsReport = FALSE;
			
#ifdef __MMI_DUAL_SIM_MASTER__
		MTPNP_AD_Free_Channel(MTPNP_AD_SMS_CHANNEL);

		if(dsmDualSim.simId[dsmDualSim.curActive] == DSM_MASTER_SIM)
		{
#if(MTK_VERSION >=0x0924)
			sendData->sendrequire = MMI_FRM_SMS_SEND_BY_SIM1;
#endif
			MTPNP_AD_Set_Channel(MTPNP_AD_SMS_CHANNEL, MTPNP_AD_CHANNEL_MASTER);
		}
		else
		{
#if(MTK_VERSION >=0x0924)
			sendData->sendrequire = MMI_FRM_SMS_SEND_BY_SIM2;
#endif		
			MTPNP_AD_Set_Channel(MTPNP_AD_SMS_CHANNEL, MTPNP_AD_CHANNEL_SLAVE);
		}
#endif
			
			if(encode&0x08)
			{
				sendData->sendcheck = MMI_FRM_SMS_SCR;  
				
				//gDsmSendMessage = 1;
#if (MTK_VERSION>=0x0924)
			sendData->sendrequire |= MMI_FRM_SMS_NOT_DISP_SENDING_ICON;
#endif
				//if(DeliveryRepyStates[0]== 1)
				//{
					//IsDsmCloseSmsReport = TRUE;
					dsmDeliveryRepyStates[0] = 0;
					mmi_frm_sms_set_common_settings(NULL, MOD_MMI, dsmDeliveryRepyStates);
				//}
				mmi_frm_sms_send_sms(mr_send_mode1_message_response, MOD_MMI,  sendData);
				
			}
			else
			{
				mmi_frm_sms_send_sms(mr_send_mode2_message_response, MOD_MMI, sendData);
			}
			
			OslMfree(sendData);

			return MR_SUCCESS;  
		}
		else 
			return MR_FAILED;
	}
	else
		return MR_FAILED;
}

