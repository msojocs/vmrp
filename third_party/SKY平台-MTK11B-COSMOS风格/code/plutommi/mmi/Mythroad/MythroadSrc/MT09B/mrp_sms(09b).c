
#include "..\..\plutommi\service\smssrv\smsprotsrv.h"
extern void srv_sms_utc_sec_to_scts(U8 scts[], U32 utc_sec);
extern MMI_BOOL dsm_sms_get_list_and_size(U16 **msg_id_list, U16 *msg_list_size, srv_sms_box_enum msg_box_type);
extern srv_sms_msg_node_struct* srv_sms_get_msg_node(U16 msg_id);
static U16 dsm_find_sms(srv_sms_box_enum msg_box_type,U16 index);

#include "mmi_rp_app_vapp_sms_def.h"

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */

#if 0
static void dsm_send_sms_cb_mode2(srv_sms_callback_struct* callback_data)
{
	MMI_BOOL result = callback_data->result;
	dsmSmsOpCount = 0;
	
	if(result == MMI_TRUE)
	{
		mmi_display_popup((UI_string_type) GetString(STR_GLOBAL_SENT),MMI_EVENT_SUCCESS);
	}
	else
	{
		mmi_display_popup((UI_string_type) GetString(STR_GLOBAL_FAILED_TO_SEND),MMI_EVENT_FAILURE);
	}
	
	//DeleteScreenIfPresent(SCR_SENDING_SMS);
	mmi_frm_scrn_close(mr_app_get_group_id(), SCR_SENDING_SMS);
		
	if(result == MMI_TRUE)
	{
		mr_event(MR_SMS_RESULT,MR_SUCCESS,(int32)callback_data->action_data);
	}
	else
	{
		mr_event(MR_SMS_RESULT,MR_FAILED,(int32)callback_data->action_data);
	}
}


static void dsm_send_sms_cb_mode1(srv_sms_callback_struct* callback_data)
{
	MMI_BOOL result = callback_data->result;
	dsmSmsOpCount = 0;

	if(result == MMI_TRUE)
	{
		mr_event(MR_SMS_RESULT,MR_SUCCESS,(int32)callback_data->action_data);
	}
	else
	{
		mr_event(MR_SMS_RESULT,MR_FAILED,(int32)callback_data->action_data);
	}
}
#else
static void dsm_send_sms_cb_mode1_local(mmi_event_struct *param)
{
	MMI_BOOL result = (MMI_BOOL)param->user_data;
	
	dsmSmsOpCount = 0;

	if (result == MMI_TRUE)
	{
		mr_event(MR_SMS_RESULT, MR_SUCCESS, 0);
	}
	else
	{
		mr_event(MR_SMS_RESULT, MR_FAILED, 0);
	}
}


static void dsm_send_sms_cb_mode1(srv_sms_callback_struct* callback_data)
{
	mmi_event_struct evt;
	
	MMI_FRM_INIT_EVENT(&evt, 0);

	if (dsmSmsOpCount == 0)
	{
		/* 这时是通过srv_sms_send_msg同步调上来的，需要做异步处理 */
		MMI_FRM_POST_EVENT(&evt, dsm_send_sms_cb_mode1_local, callback_data->result);
		return ;
	}
	
	evt.user_data = (void*)callback_data->result;
	
	dsm_send_sms_cb_mode1_local(&evt);
}

static void dsm_send_sms_cb_mode2_local(mmi_event_struct *param)
{
	MMI_BOOL result = (MMI_BOOL)param->user_data;
	
	dsmSmsOpCount = 0;
	
	if (result == MMI_TRUE)
	{
		mmi_display_popup((UI_string_type) GetString(STR_GLOBAL_SENT), MMI_EVENT_SUCCESS);
	}
	else
	{
		mmi_display_popup((UI_string_type) GetString(STR_GLOBAL_FAILED_TO_SEND), MMI_EVENT_FAILURE);
	}
	
	//DeleteScreenIfPresent(SCR_SENDING_SMS);
	mmi_frm_scrn_close(mr_app_get_group_id(), SCR_SENDING_SMS);
		
	if (result == MMI_TRUE)
	{
		mr_event(MR_SMS_RESULT, MR_SUCCESS, 0);
	}
	else
	{
		mr_event(MR_SMS_RESULT, MR_FAILED, 0);
	}
}

static void dsm_send_sms_cb_mode2(srv_sms_callback_struct* callback_data)
{
	mmi_event_struct evt;
		
	MMI_FRM_INIT_EVENT(&evt, 0);

	if (dsmSmsOpCount == 0)
	{
		/* 这时是通过srv_sms_send_msg同步调上来的，需要做异步处理 */
		MMI_FRM_POST_EVENT(&evt, dsm_send_sms_cb_mode2_local, callback_data->result);
		return ;
	}
	evt.user_data = (void*)callback_data->result;

	dsm_send_sms_cb_mode2_local(&evt);
}


#endif
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */


static void dsm_entry_send_sms(void)
{
	//需要PAUSE虚拟机.
#if 0
	mr_app_EntryNewScreen(SCR_SENDING_SMS,NULL, dsm_entry_send_sms, MMI_FRM_FULL_SCRN);
	
	ShowCategory8Screen(
		STR_GLOBAL_SENDING,
		0, //IMG_UC_ENTRY_SCRN_CAPTION_ID,
		0,
		0,
		0,
		0, //IMG_SMS_COMMON_NOIMAGE,
		STR_GLOBAL_SENDING,//STR_SENDIING_SMS_BODY,
		IMG_ID_VAPP_SMS_SENT, //IMG_NEW_SMS_SEND,
		NULL);
#else
	DisplayConfirm(0,0,0,0,(UI_string_type)get_string(STR_GLOBAL_SENDING),0,0);
	//mmi_display_popup((UI_string_type)get_string(STR_GLOBAL_SENDING),0);
#endif
	
	ClearAllKeyHandler();
	ClearInputEventHandler(MMI_DEVICE_KEY);
	ClearKeyHandler(KEY_END, KEY_EVENT_UP);
	ClearKeyHandler(KEY_END, KEY_EVENT_DOWN);
	ClearKeyHandler(KEY_END, KEY_LONG_PRESS);
	ClearKeyHandler(KEY_END, KEY_REPEAT);
}


int32 mr_sms_get_sms_count(int32 param)
{
	U16 *msg_id_list=NULL;
	S32 i =0;
	S32 count =0;
	U16 msg_list_size=0;
	S32 simId=0;
	srv_sms_box_enum msg_box_type;
	srv_sms_msg_node_struct *pNode;
	
	if(!srv_sms_is_sms_ready())
		return MR_FAILED;
	
	if(param == MR_SMS_INBOX)
		msg_box_type = SRV_SMS_BOX_INBOX;
	else if(param == MR_SMS_OUTBOX)
		msg_box_type = SRV_SMS_BOX_OUTBOX;
	else if(param == MR_SMS_DRAFTS)
		msg_box_type = SRV_SMS_BOX_DRAFTS;
	else
		return MR_FAILED;

	if(!dsm_sms_get_list_and_size(&msg_id_list, &msg_list_size, msg_box_type))
		return MR_FAILED;

	simId = mr_sim_active_id_dsm2mmi() | MMI_GSM;

	for(i = 0;i<msg_list_size;i++)
	{
		pNode = srv_sms_get_msg_node(msg_id_list[i]);
		if((pNode)
#ifdef __SRV_SMS_MULTI_SIM__		
			&&(pNode->sim_id == simId)
#endif			
		)
		{
			count++;
		}
	}
	return count + MR_PLAT_VALUE_BASE;
}


static void dsm_msg_get_msg_rsp(srv_sms_callback_struct* callback_data)
{
	srv_sms_read_msg_cb_struct *read_msg_cb_data;
	srv_sms_msg_data_struct *msg_data;
	read_msg_cb_data = (srv_sms_read_msg_cb_struct*)callback_data->action_data;
	msg_data = read_msg_cb_data->msg_data;

	/* 支持后台程序短信的读取 */
	if(mr_app_get_state() == DSM_STOP)
	{
		//释放之前申请的内存.
		if(msg_data->content_buff) 
			OslMfree(msg_data->content_buff);
		if (msg_data) 
			OslMfree(msg_data);
			
		return;
	}
	
	if (callback_data->result == MMI_FALSE)
	{
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_MSG_CONTENT,NULL);
	}
	else
	{
	//已经有内存数据不用再分配了
	#if 1
	
		mr_str_convert_endian_with_size(msg_data->content_buff,msg_data->content_size);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_MSG_CONTENT,(int32)msg_data->content_buff);

	#else
		char *tmp = NULL;
		int len = 0;
		len = msg_data->content_size + 2;
		tmp = OslMalloc(len);

		if(tmp == NULL)
		{
			mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_MSG_CONTENT,NULL);
		}
		else
		{
        		memset(tmp,0,len);
        		memcpy(tmp,msg_data->content_buff, msg_data->content_size);
        		mr_str_convert_endian(tmp);
        		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_MSG_CONTENT,(int32)tmp);
        		OslMfree(tmp);
		}
	#endif
	}
	
	//释放之前申请的内存
	if(msg_data->content_buff) 
		OslMfree(msg_data->content_buff);
	if (msg_data) 
		OslMfree(msg_data);
}


static void dsm_msg_get_memory_status_rsp(srv_sms_callback_struct* callback_data)
{
	
	if((mr_app_get_state() == DSM_STOP)||(!callback_data))
	{
		return;
	}
	
	if(callback_data->result == MMI_TRUE)
	{
		srv_sms_memory_status_struct *memorystatus = (srv_sms_memory_status_struct*)callback_data->action_data;

		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE,memorystatus->me_total);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_SIM_SIZE,memorystatus->sim_total);
	}
	else
	{
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE,-1);
		mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_GET_SIM_SIZE,-1);		
	}
}


static void dsm_get_sms_memory_req(void)
{
	srv_sms_sim_enum simId;

	simId = mr_sim_active_id_dsm2mmi() | MMI_GSM;

	srv_sms_get_setting_para(SRV_SMS_GET_MEMORY_STATUS, 
		NULL, 
		simId, 
		dsm_msg_get_memory_status_rsp, 
		NULL);	
}


int32 dsm_get_sms_info(T_DSM_GET_SMS_INFO_REQ *pReq)
{
	srv_sms_box_enum msg_box_type;
	srv_sms_msg_node_struct *pNode;
	U16 msg_id;
	
	if(pReq->type == MR_SMS_INBOX)
		msg_box_type = SRV_SMS_BOX_INBOX;
	else if(pReq->type == MR_SMS_OUTBOX)
		msg_box_type = SRV_SMS_BOX_OUTBOX;
	else
		return MR_FAILED;
	
	msg_id= dsm_find_sms(msg_box_type,pReq->index);

	if(msg_id == 0xffff)
		return MR_FAILED;

	pNode = srv_sms_get_msg_node(msg_id);
	
	memset(&gdsmSMSInfo,0,sizeof(gdsmSMSInfo));

	if(pNode->storage_type == SRV_SMS_STORAGE_ME)
		gdsmSMSInfo.sms_storage = MR_SMS_STORAGE_ME;
	else
		gdsmSMSInfo.sms_storage = MR_SMS_STORAGE_SIM;
	
	strncpy((char*)gdsmSMSInfo.num,(char*)pNode->number,sizeof(gdsmSMSInfo.num)-1);
	
	if(pReq->type == MR_SMS_INBOX)
	{
		MYTIME time_stamp;

		srv_sms_get_msg_timestamp(msg_id, &time_stamp);
		gdsmSMSInfo.t_stamp[0] = time_stamp.nYear-2000;
		gdsmSMSInfo.t_stamp[1] = time_stamp.nMonth;
		gdsmSMSInfo.t_stamp[2] = time_stamp.nDay;
		gdsmSMSInfo.t_stamp[3] = time_stamp.nHour;
		gdsmSMSInfo.t_stamp[4] = time_stamp.nMin;
		gdsmSMSInfo.t_stamp[5] = time_stamp.nSec;
		
		if(pNode->status&SRV_SMS_STATUS_UNREAD)
			gdsmSMSInfo.sms_status = MR_SMS_UNREAD;
		else
			gdsmSMSInfo.sms_status = MR_SMS_READED;
	}
	else
	{
		if(pNode->status&SRV_SMS_STATUS_SENT)
			gdsmSMSInfo.sms_status = MR_SMS_SENT;
		else
			gdsmSMSInfo.sms_status = MR_SMS_UNSENT;
	}

	return MR_SUCCESS;
}


static void dsm_frm_sms_delete_rsp(srv_sms_callback_struct* callback_data)
{
	if(mr_app_get_state() == DSM_STOP)
		return;
		
	if (callback_data->result == MMI_TRUE)
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
	U16 msg_id;
	srv_sms_box_enum msg_box_type;
	T_DSM_DELETE_SMS_REQ *pReq = (T_DSM_DELETE_SMS_REQ*)input;
	if((input == NULL)||(input_len < sizeof(T_DSM_DELETE_SMS_REQ)))
		return MR_FAILED;
	
	if (srv_sms_is_sms_ready() == MMI_FALSE)
		return MR_FAILED;
	if(pReq->type == MR_SMS_INBOX)
		msg_box_type = SRV_SMS_BOX_INBOX;
	else if(pReq->type == MR_SMS_OUTBOX)
		msg_box_type = SRV_SMS_BOX_OUTBOX;
	else
		return MR_FAILED;
	
	msg_id = dsm_find_sms(msg_box_type, pReq->index);

	if(msg_id == 0xffff)
		return MR_FAILED;
	srv_sms_delete_msg(msg_id, dsm_frm_sms_delete_rsp,0);
	return MR_WAITING;
}


int32 mr_sms_read_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	U16 msg_id;
	srv_sms_box_enum msg_box_type;
	srv_sms_msg_data_struct *msg_data;
	T_DSM_GET_SMS_INFO_REQ *pReq = (T_DSM_GET_SMS_INFO_REQ*)input;
	if(pReq == NULL)
		return MR_FAILED;

	if (srv_sms_is_sms_ready() == MMI_FALSE)
		return MR_FAILED;
	if(pReq->type == MR_SMS_INBOX)
		msg_box_type = SRV_SMS_BOX_INBOX;
	else if(pReq->type == MR_SMS_OUTBOX)
		msg_box_type = SRV_SMS_BOX_OUTBOX;
	else
		return MR_FAILED;     

	msg_id = dsm_find_sms(msg_box_type, pReq->index);

	if(msg_id == 0xffff)
		return MR_FAILED;
	 
	//注意内存释放,回调函数里
	msg_data = (srv_sms_msg_data_struct*)OslMalloc(sizeof(srv_sms_msg_data_struct));
	if(!msg_data) return MR_FAILED;

	memset(msg_data, 0, sizeof(srv_sms_msg_data_struct));
	msg_data->para_flag = SRV_SMS_PARA_CONTENT_BUFF;
	msg_data->content_buff_size = (500 + 1) * ENCODING_LENGTH;
	//注意内存释放,在回调函数里
	msg_data->content_buff = OslMalloc(msg_data->content_buff_size);
	if(!msg_data->content_buff)
	{
		OslMfree(msg_data);
		return MR_FAILED;
	}

	srv_sms_read_msg(
		msg_id,
		MMI_TRUE,
		msg_data,
		dsm_msg_get_msg_rsp,
	        (void*)0);
	return MR_WAITING;
}	


static U16 dsm_find_sms(srv_sms_box_enum msg_box_type,U16 index)
{
	S32 count =0;
	S32 i =0;
	U16 *msg_id_list=NULL;
	U16 msg_list_size=0;
	S32 simId=0;
	srv_sms_msg_node_struct *pNode;
	
	if(!dsm_sms_get_list_and_size(&msg_id_list, &msg_list_size, msg_box_type))
		return NULL;

	simId = mr_sim_active_id_dsm2mmi() | MMI_GSM;

	for(i = 0;i<msg_list_size;i++)
	{
		pNode =  srv_sms_get_msg_node(msg_id_list[i]);
		if((pNode)
#ifdef __SRV_SMS_MULTI_SIM__		
			&&(pNode->sim_id == simId)
#endif		
		)
		{
			count++;
			if((count-1)== index)
				return msg_id_list[i];
		}
	}	
	return 0xFFFF;
}


int32 mr_sms_get_memory_size(int32 param)
{
	gui_start_timer(50,dsm_get_sms_memory_req);
	return MR_WAITING;
}


void dsm_get_sc_addr_rsp(srv_sms_callback_struct* callback_data)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	char scTmp[50];
	
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
#if 0	
	if (mmi_msg_check_interrupt() == TRUE)
	{
		return;
	}
#endif	
	if(mr_app_get_state() == DSM_STOP)
	{
		return;
	}
	
	memset(scTmp,0,sizeof(scTmp));
	if (callback_data->result == MMI_TRUE)
	{        
		mmi_ucs2_to_asc((S8 *)scTmp,(S8 *)callback_data->action_data);
	
		if (strlen(scTmp)==0)
		{
			mr_event(MR_SMS_GET_SC,(int32)scTmp,0);
		}
		else
		{
			mr_event(MR_SMS_GET_SC,(int32)scTmp,strlen(scTmp)+1);
		}
	}
	else
	{
		mr_event(MR_SMS_GET_SC,0,0);
	}
}


int32 mr_sms_get_sc_req(int32 param)
{
	srv_sms_sim_enum sms_sim;
	
	sms_sim = mr_sim_active_id_dsm2mmi() | MMI_GSM;
	
	srv_sms_get_setting_para(SRV_SMS_ACTIVE_SC_ADDR, 
		NULL, 
		sms_sim, 
		dsm_get_sc_addr_rsp, 
		NULL);

	return MR_WAITING;
}

void (*g_dsm_get_sc_cb)(int32 result, char *sc_addr);
void dsm_get_sc_addr_rsp_ex(srv_sms_callback_struct* callback_data)
{
	if (g_dsm_get_sc_cb != NULL)
	{
		if (callback_data->result == MMI_TRUE)
		{
			char scTmp[50];

			memset(scTmp, 0, sizeof(scTmp));
			
			mmi_ucs2_to_asc(scTmp, (CHAR *)callback_data->action_data);

			(*g_dsm_get_sc_cb)(1, scTmp);
		}
		else
		{
			(*g_dsm_get_sc_cb)(0, NULL);
		}
	}
}


int32 mr_sms_get_sc_req_ex(void (*cb)(int32 result, char *sc_addr))
{
	srv_sms_sim_enum sms_sim;
	
	sms_sim = mr_sim_active_id_dsm2mmi() | MMI_GSM;
	
	srv_sms_get_setting_para(SRV_SMS_ACTIVE_SC_ADDR, 
		NULL, 
		sms_sim, 
		dsm_get_sc_addr_rsp_ex, 
		NULL);

	g_dsm_get_sc_cb = cb;
	
	return MR_WAITING;
}

static int mr_str_is_ascii(uint8* str, int len)
{
	int i;

	for (i = 0; i < len; i += 2)
	{
		if (str[i + 1] != 0)
			return MR_FALSE;
	}

	return MR_TRUE;
}

#ifdef __SKY_SECURITY_GUARD__
#include "ATS_adaptation_include.h"
#include "sky_sg_features.h"
#endif

int dsmReciveMsg(srv_sms_event_struct* event_data)
{
	// 11A版本接收到的短信数据都是UNICODE编码的
	srv_sms_event_new_sms_struct* event_info;
	srv_sms_new_msg_struct* msg_data;
	int32 len;
	int32 type;
	uint8* content;
	uint8 number[SRV_SMS_MAX_ADDR_LEN + 1];
	int ret;

	event_info = (srv_sms_event_new_sms_struct*)event_data->event_info;
	msg_data = (srv_sms_new_msg_struct*)event_info->msg_data;

	log_sms("new_msg_data->dcs=%d", msg_data->dcs);
	log_sms("new_msg_data->message_len=%d", msg_data->message_len);

	content = OslMalloc(msg_data->message_len + 2);
	if (content == NULL)
		return 0;

	memset(content, 0, msg_data->message_len + 2);
	
	if (mr_str_is_ascii(event_info->content, msg_data->message_len))
	{
		type = MR_ENCODE_ASCII;
		app_ucs2_str_to_asc_str(content, event_info->content);
		len = msg_data->message_len / 2;
	}
	else
	{
		type = MR_ENCODE_UNICODE;
		memcpy(content, event_info->content, msg_data->message_len);
		mr_str_convert_endian(content);
		len = msg_data->message_len;
	}

	memcpy(number, msg_data->number, SRV_SMS_MAX_ADDR_LEN + 1);

	if (mr_sms_execute_incoming_sms(content, len, number, type) != MR_IGNORE)
	{
		srv_sms_delete_msg(event_info->msg_id, NULL, NULL);
		ret = 1;
	}
	else 
	if(mr_smsIndiactionEx(content, len, number, type) != MR_IGNORE)
	{
		srv_sms_delete_msg(event_info->msg_id, NULL, NULL);
		ret = 1;
	}
#ifdef __SKY_SECURITY_GUARD__//huangsunbo todo
	else if (sg_sms_process((void*)content, (ats_uint16)msg_data->message_len, (void*)number, (ats_int32)type, 
		(ats_uint8)msg_data->seg, (ats_uint8)msg_data->total_seg, (ats_int16)msg_data->sim_id))
	{
		srv_sms_delete_msg(event_info->msg_id, NULL, (void*)NULL); 
		ret = 1;
	}
#endif
	else
	{
		ret = 0;
	}

	OslMfree(content);

	return ret;
}


int32 mr_sendSms(char* pNumber, char*pContent, int32 encode)
{
       int numlen = 0;

 	log_sms("mr_sendSms pNumber = %s",pNumber);

	if(pNumber == NULL||pContent == NULL)
		return MR_IGNORE;
	
	if (mr_IsReadySendSms())
	{
		SMS_HANDLE srv_send_handle = NULL;
		srv_sms_setting_struct default_setting;
		S8 *sc_number;
		S8 ucs2_addr[(SRV_SMS_MAX_ADDR_LEN +1) *ENCODING_LENGTH];	
		U8 *pTmp = NULL;
		S32 contentLen =0;
		
		numlen = strlen(pNumber) > MAX_DIGITS_SMS?MAX_DIGITS_SMS:strlen(pNumber);

		if(numlen <=0)
			return MR_FAILED;
		
		srv_send_handle = srv_sms_get_send_handle();
		mmi_asc_n_to_ucs2((S8*)ucs2_addr, pNumber, numlen +1);
		srv_sms_set_address(srv_send_handle, ucs2_addr);
		
		srv_sms_set_sim_id(srv_send_handle, mr_sim_active_id_dsm2mmi() | MMI_GSM);

		if((encode&0x07) == MR_ENCODE_UNICODE)
		{
			srv_sms_set_content_dcs(srv_send_handle,SRV_SMS_DCS_UCS2);
			contentLen = mr_str_wstrlen(pContent)+2;
			//注意内存释放
			pTmp = OslMalloc(contentLen);
			if(pTmp ==NULL)
			{
				srv_sms_free_send_handle(srv_send_handle);
				return MR_FAILED;
			}
			else
			{
				memset(pTmp,0,contentLen);
				memcpy(pTmp,pContent,contentLen);
				mr_str_convert_endian((char *)pTmp);
			}
			
			srv_sms_set_content(srv_send_handle, (S8 *) pTmp,contentLen);
		}
		else
		{
			srv_sms_set_content_dcs(srv_send_handle,SRV_SMS_DCS_7BIT);
			contentLen = (strlen(pContent)+1)*2;
			//注意内存释放
			pTmp = OslMalloc(contentLen);

			if(pTmp ==NULL)
			{
				srv_sms_free_send_handle(srv_send_handle);
				return MR_FAILED;
			}
			else
			{
				memset(pTmp,0,contentLen);
				mmi_asc_to_ucs2((CHAR *)pTmp, (CHAR *)pContent);
				srv_sms_set_content(srv_send_handle, (S8 *) pTmp,contentLen);
			}
		}
		
		srv_sms_set_no_sending_icon(srv_send_handle);
		srv_sms_set_send_type(srv_send_handle,SRV_SMS_FG_SEND);

		if(encode&0x08)
		{
			srv_sms_set_status_report(srv_send_handle, MMI_FALSE);
			srv_sms_send_msg(srv_send_handle, dsm_send_sms_cb_mode1, (void*)0);
		}
		else
		{
			dsm_entry_send_sms();
			srv_sms_send_msg(srv_send_handle, dsm_send_sms_cb_mode2, (void*)0);
		}
	
		OslMfree(pTmp);
		dsmSmsOpCount = 1;
		return MR_SUCCESS;
	}
	else
		return MR_FAILED;
}

void dsm_save_sms_callback(srv_sms_callback_struct* callback_data)
{
         srv_sms_callback_struct* result = callback_data;
         mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_SAVE_SMS, result->result);
}

int32  mr_sms_save_msg(T_MR_SMS_SAVE_ENTRY_REQ *pSmsEntry)
{
	SMS_HANDLE save_handle;
	S32 length;

	save_handle = srv_sms_get_save_handle();	

#if defined(__MMI_DUAL_SIM_MASTER__)
         if (pSmsEntry->flag & MR_SMSSAVE_DISP_SIM_OPT)
         {
  		srv_sms_set_sim_id(save_handle, SRV_SMS_SIM_1);
         }
         else if (pSmsEntry->flag & MR_SMSSAVE_TO_SIM1)
         {
   		srv_sms_set_sim_id(save_handle, SRV_SMS_SIM_1);
         }
         else if (pSmsEntry->flag & MR_SMSSAVE_TO_SIM2)
         {
     		srv_sms_set_sim_id(save_handle, SRV_SMS_SIM_2);
         }
#endif

	if(pSmsEntry->number)
	{
		S8 addr[(SRV_SMS_MAX_ADDR_LEN + 1) *ENCODING_LENGTH];
		U16 addr_len;

		addr_len = mmi_asc_n_to_ucs2((S8*)addr, (S8*)pSmsEntry->number, SRV_SMS_MAX_ADDR_LEN);
		addr[addr_len] = '\0';
		addr[addr_len + 1] = '\0';

		srv_sms_set_address(save_handle, (S8*)addr);
	}	
	if (pSmsEntry->status == 0)
	{
		srv_sms_set_status(save_handle, SRV_SMS_STATUS_UNREAD);
	}
	else
	{
		srv_sms_set_status(save_handle, SRV_SMS_STATUS_READ);
	}
	
	//unicode 大小转换
	mr_str_convert_endian_with_size(pSmsEntry->content,MR_SMS_MAX_CONTENT);
	
	//set countent
	length = mmi_ucs2strlen((const S8*)pSmsEntry->content);
	srv_sms_set_content(save_handle, (S8*)pSmsEntry->content,length * ENCODING_LENGTH);
	srv_sms_set_content_dcs(save_handle, SRV_SMS_DCS_UCS2);
	srv_sms_save_msg(save_handle, dsm_save_sms_callback, NULL);
	
	return MR_SUCCESS;
}

int32  g_dsm_reply_flag = 0;
T_MR_SMS_REPLY_ENTRY_REQ g_dsm_reply_entry = {0};
int32 mr_sms_get_reply_flag(void)
{
	return g_dsm_reply_flag;
}
void mr_sms_set_reply_flag(int32 value)
{
	g_dsm_reply_flag = value;
}
S8*  mr_sms_get_number(void)
{
	return g_dsm_reply_entry.number;
}
int32 mr_sms_get_simid(void)
{
	return g_dsm_reply_entry.simid;
}

int32  mr_sms_reply_msg(T_MR_SMS_REPLY_ENTRY_REQ *pSmsEntry)
{
#if (defined(__MMI_UNIFIED_COMPOSER__) || defined(__MMI_MMS_STANDALONE_COMPOSER__))

	VappUcEntryStruct entry = {0};
	srv_uc_addr_struct uc_addr = {0};
	kal_uint8 number_ucs[MR_PHB_MAX_NUMBER*2] = {0};
	memset(&entry, 0, sizeof(VappUcEntryStruct));
	memset(&uc_addr, 0, sizeof(srv_uc_addr_struct));

	entry.type = SRV_UC_STATE_WRITE_NEW_MSG;
	entry.msg_type = SRV_UC_MSG_TYPE_SMS_PREFER;

	uc_addr.type = SRV_UC_ADDRESS_TYPE_PHONE_NUMBER;
	uc_addr.group = SRV_UC_ADDRESS_GROUP_TYPE_TO;

	memset(number_ucs,0,MR_PHB_MAX_NUMBER*2);
	mmi_asc_to_ucs2(number_ucs,pSmsEntry->number);
	uc_addr.addr = number_ucs;	
	entry.addr_num = 1;
	entry.addr = &uc_addr;
	
	log_sms("mr_sms_reply_msg,%d,%s",pSmsEntry->simid,pSmsEntry->number);
	
	if(pSmsEntry->simid  == 0)
	{
		entry.sim_id = SRV_UC_SIM_ID_GSM_SIM1;
	}
	else
	{
		entry.sim_id = SRV_UC_SIM_ID_GSM_SIM2;
	}
	g_dsm_reply_flag = 1;
	g_dsm_reply_entry.simid = entry.sim_id;
	vappUcAppLauncher(&entry);			
	return MR_SUCCESS;
#endif
	log_sms("mr_sms_reply_msg failed,%s,%d",pSmsEntry->number,pSmsEntry->simid);
	return MR_FAILED;
}
#ifdef __MR_CFG_SET_SMS_STATUS__

void mr_set_sms_status_callback(srv_sms_callback_struct *cb)
{
	if(mr_app_get_state() != DSM_STOP)
	{
	  	if((cb != 0) && (cb->result == MMI_TRUE))
	  	{
			kal_prompt_trace(MOD_MMI,"dsm_set_sms_state_callback true");  		
			mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_SET_STATUS,MR_SUCCESS);

	  	}
	  	else
	  	{
			kal_prompt_trace(MOD_MMI,"dsm_set_sms_state_callback false");  		
			mr_event(MR_SMS_OP_EVENT,MR_SMS_OP_SET_STATUS,MR_FAILED);
	  	}
	}
}

int32 mr_set_sms_status(T_DSM_SET_SMS_STATE_REQ*pReq)
{
	srv_sms_box_enum msg_box_type;
	srv_sms_msg_node_struct *pNode;
	U16 msg_id;
	U16 status = 0;
	
	if(pReq->type == MR_SMS_INBOX)
		msg_box_type = SRV_SMS_BOX_INBOX;
	else if(pReq->type == MR_SMS_OUTBOX)
		msg_box_type = SRV_SMS_BOX_OUTBOX;
	else
		return MR_FAILED;
	
	msg_id= dsm_find_sms(msg_box_type,pReq->index);

	if(msg_id == 0xffff)
		return MR_FAILED;

	pNode = srv_sms_get_msg_node(msg_id);
	
	kal_prompt_trace(MOD_MMI,"dsm_set_sms_state 444,%d,%d,%d",msg_id,pReq->state,pNode->status);
	switch(pReq->state)
	{
		case MR_SMS_UNREAD:
		{
			status = SRV_SMS_STATUS_UNREAD;
			break;
		}
		case MR_SMS_READED:
		{
			status = SRV_SMS_STATUS_READ;
			break;
		}
		case MR_SMS_UNSENT:
		{
			status = SRV_SMS_STATUS_UNSENT;
			break;
		}
		case MR_SMS_SENT:
		{
			status = SRV_SMS_STATUS_SENT;
			break;
		}
	}
	
	srv_sms_change_msg_status(msg_id,status,mr_set_sms_status_callback,NULL);

	return MR_SUCCESS;
}
#endif
