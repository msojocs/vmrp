#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "PixtelDataTypes.h"
#include "MMIDataType.h"
#include "med_utility.h"
//#include "SimDetectionDef.h"
#include "MessagesResourceData.h"
#include "Ems.h"
#include "SmsGuiInterfaceProt.h"
#include "SmsGuiInterfaceType.h"
#include "ProtocolEvents.h"
#include "AlarmFrameworkProt.h"
#include "CommonScreens.h"
#include "Wgui_ems.h"
#include "SettingProfile.h"

#include "MessagesMiscell.h"
#include "SmsGuiInterfaceProt.h"
#include "vapp_uc_gprot.h"
#include "mrp_include.h"

#if (MTK_VERSION >= 0x0924)
//#include "SmsAppProt.h"
#include "SmsAppType.h"
#endif

#if (MTK_VERSION <= 0x0848)
#include "SMSApi.h"
#endif

#if (MTK_VERSION < 0x09B0952)
#include "SMSStruct.h"
#endif


#if (MTK_VERSION >=0x0952)
#ifndef IMG_SEND_SUCCESS_PIC_MSG
#define IMG_SEND_SUCCESS_PIC_MSG  (IMG_GLOBAL_SUCCESS)
#define IMG_SEND_FAIL_PIC_MSG          (IMG_GLOBAL_FAIL)
#endif
#endif


//adaptor
#if (MTK_VERSION >= 0x0824)
#define INPUT_TYPE_PHONE_NUMBER						IMM_INPUT_TYPE_NUMERIC
#define INPUT_TYPE_ALPHANUMERIC_PASSWORD			IMM_INPUT_TYPE_ALPHANUMERIC_PASSWORD
#define INPUT_TYPE_ALPHANUMERIC_SENTENCECASE			IMM_INPUT_TYPE_SENTENCE
#endif

T_DSM_MSG_INFO gdsmSMSInfo = {0};
static int32 dsmDeliveryLock = 0;
U8 newSmsDisplay = 1;

#if(MTK_VERSION >= 0x09B0952)
U8 dsmSmsOpCount = 0;
#endif

#if(MTK_VERSION < 0x09B0952)
static U8 dsmDeliveryRepyStates[2] = {0};
#endif
//缺省不开这个宏
//#define __LOG_SMS__
#ifdef __LOG_SMS__
	#define log_sms                 mr_printf
#else
	#define log_sms(...)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern EMSData * GetEMSDataForEdit (EMSData ** p, U8 force);
extern EMSData *GetEMSDataForView(EMSData **p, U8 force);
U8 mmi_msg_check_interrupt(void);
static MR_BOOL mr_IsReadySendSms(void);


/* 直接INCLUDE对应功能的c文件，大家不要感觉差异。*/
#if(MTK_VERSION < 0x09B0952)
#include ".\mt07a-09a\mrp_sms(07a-09a).c"
#else
#include ".\mt09b\mrp_sms(09b).c"
#endif


#define MR_MAX_PROCESS_SMS_CALLBACK 5
static SMS_CALLBACK sms_process_call[MR_MAX_PROCESS_SMS_CALLBACK];


int32 mr_sms_execute_incoming_sms(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type)
{
	int i;
	int32 ret = MR_IGNORE;
	
	for (i = 0; i < MR_MAX_PROCESS_SMS_CALLBACK; i++)
	{
		if (sms_process_call[i] != NULL)
		{
			ret = sms_process_call[i](pContent, nLen, pNum, type);
			if (ret == MR_SUCCESS)
			{
				break; 
			}
		}
	}

	return ret;
}


int32 mr_sms_register_cb(SMS_CALLBACK cb)
{
	int i;
	
	for (i = 0; i < MR_MAX_PROCESS_SMS_CALLBACK; i++)
	{
		if (sms_process_call[i] == NULL || sms_process_call[i] == cb)
		{
			break;
		}
	}

	if (i < MR_MAX_PROCESS_SMS_CALLBACK)
	{
		sms_process_call[i] = cb;
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_sms_deregister_cb(SMS_CALLBACK cb)
{
	int i;
	
	for (i = 0; i < MR_MAX_PROCESS_SMS_CALLBACK; i++)
	{
		if (sms_process_call[i] == cb)
		{
			sms_process_call[i] = NULL;
			return MR_SUCCESS;
		}
	}

	return MR_FAILED;
}


int32 mr_sms_initialize(void)
{
#if(MTK_VERSION >= 0x09B0952)
	dsmSmsOpCount = 0;
#endif

	srv_sms_set_interrupt_event_handler(EVT_ID_SRV_SMS_NEW_MSG, (SrvSmsEventFunc)dsmReciveMsg, NULL);

	return MR_SUCCESS;
}


int32 mr_sms_terminate(void)
{
	return MR_SUCCESS;
}


int32 mr_sms_send_mms(T_DSM_MMS* pMMS)
{
#ifdef __COSMOS_MMI_PACKAGE__
	VappUcEntryStruct data = { 0 };
	srv_uc_addr_struct uc_addr = {0};
	BOOL result;	
	WCHAR* path;
	WCHAR* tmp;
	
	path = OslMalloc(mr_str_wstrlen((char*)pMMS->file_path) + 2);
	if (path == NULL)
		return MR_FAILED;

	path[0] = 0;
	app_ucs2_strcat((kal_int8*)path, (kal_int8*)pMMS->file_path);

	if(path[0] == 'A'||path[0] == 'a')
	{
		path[0] = MMI_SYSTEM_DRV;
	}
	else if(path[0] == 'B'||path[0] == 'b')
	{
		path[0] = MMI_PUBLIC_DRV;
	}
	else
	{
		path[0] = MMI_CARD_DRV;
	}
	
	tmp = path;
	while(tmp = (WCHAR*)app_ucs2_strchr((kal_int8*)tmp, '/'))
	{
		*tmp = '\\';
		++tmp;
	}

	if (pMMS->addr != NULL)
	{
		uc_addr.addr = (kal_uint8*)pMMS->addr;
		data.addr = &uc_addr;
		data.addr_num = 1;
	}

	data.file_path = (U8*)path;
	data.text_buffer = (U8*)pMMS->content;
	data.text_num = mr_str_wstrlen((char*)pMMS->content);
	data.subject = (U8*)pMMS->subject;

	data.info_type = SRV_UC_INFO_TYPE_MMS;

	result = vappUcAppLauncher(&data);

	OslMfree(path);
	
	if (result)
		return MR_SUCCESS;
	else
		return MR_FAILED;
#else
	return MR_IGNORE;
#endif
}


/****************************************************************************
函数名:bool mr_IsReadySendSms(void)
描  述:判断gsm模块是否初始化完并能发短信
参  数:无
返  回:初始化完成:MR_TRUE
       初始化未完:MR_FALSE
****************************************************************************/
static MR_BOOL mr_IsReadySendSms(void)
{
	MR_BOOL result;
	
#if (MTK_VERSION >= 0x09B0952)
	{
		if(srv_sms_check_send_action_pending(SRV_SMS_FG_SEND)||(!srv_sms_is_sms_ready())||dsmSmsOpCount)
			result = MR_FALSE;
		else
			result = MR_TRUE;
	}
#else

#if (MTK_VERSION>=0x0936)
	 if(mmi_sms_is_allow_send_sms())
	{
		result = MR_TRUE;
	}
	else
#endif
	if(mmi_frm_sms_get_sms_list_size(0x00)==MMI_FRM_SMS_INVALID_INDEX)   //not ready
	{
		result  = MR_FALSE;
	}
	else
	{
		if(mmi_frm_sms_check_action_pending())
			result  = MR_FALSE;
		else
			result  = MR_TRUE;
	}
#endif
	log_sms("mr_IsReadySendSms %d",result);
	return result;
}


int32 mr_sms_send_sms_from_ui(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifndef __COSMOS_MMI_PACKAGE__

	EMSData	* pEMS = NULL;
	S8 * textbufUCS2 = NULL;
	int32 len = 0;
	byte  result = 1;
	T_PLAT_SENDSMS_REQ *req = (T_PLAT_SENDSMS_REQ *)input;
	
	if(input == NULL||input_len < sizeof(T_PLAT_SENDSMS_REQ))
		return MR_FAILED;
	
	if(req->pContent != NULL)
	{
		len = mr_str_wstrlen(req->pContent);
	}

	if(len > 0)
	{
		//注意内存释放
		textbufUCS2 = OslMalloc(len + 2);
	
		if(textbufUCS2 == NULL)
			return MR_FAILED;

		memset(textbufUCS2,0,len+2);

		memcpy(textbufUCS2,req->pContent,len);
		mr_str_convert_endian_with_size((char *)textbufUCS2,len);

	}

#if(MTK_VERSION >=0x0924)

	ReleaseEMSViewBuffer();
	pEMS = GetEMSDataForView(0, 1);
	
	if(pEMS == NULL)
	{
		if(textbufUCS2 != NULL)
			OslMfree(textbufUCS2);
		return MR_FAILED;
	}

	if(len > 0 )
	{
		result = AppendEMSString(INPUT_TYPE_ALPHANUMERIC_SENTENCECASE,pEMS, (U8*)textbufUCS2, SMSAL_UCS2_DCS, NULL); 
		OslMfree(textbufUCS2);
	}
		
	if(result)
	{
		mmi_sms_entry_write_msg(SMS_SEND_CASE_FORWARD);
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
#else
	ReleaseEMSEditBuffer();

	pEMS = GetEMSDataForEdit (0, 1);
	
	if(pEMS == NULL)
	{
		if(textbufUCS2 != NULL)
			OslMfree(textbufUCS2);
		return MR_FAILED;
	}

	if(len > 0 )
	{
		result = AppendEMSString(INPUT_TYPE_ALPHANUMERIC_SENTENCECASE,pEMS, (U8*)textbufUCS2, SMSAL_UCS2_DCS, NULL); 
		OslMfree(textbufUCS2);
	}
		
	if(result)
	{
		mmi_msg_entry_write_msg();
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
#endif	

#else	// __COSMOS_MMI_PACKAGE__

#if (defined(__MMI_UNIFIED_COMPOSER__) || defined(__MMI_MMS_STANDALONE_COMPOSER__))

	VappUcEntryStruct entry;
	srv_uc_addr_struct uc_addr;

	S8* textbufUCS2 = NULL;
	int32 len = 0;
	T_PLAT_SENDSMS_REQ* req = (T_PLAT_SENDSMS_REQ *)input;
	
	if(input == NULL||input_len < sizeof(T_PLAT_SENDSMS_REQ))
		return MR_FAILED;
	
	if(req->pContent != NULL)
	{
		len = mr_str_wstrlen(req->pContent);
	}

	if(len > 0)
	{
		textbufUCS2 = OslMalloc(len + 2);
	
		if(textbufUCS2 == NULL)
			return MR_FAILED;

		memset(textbufUCS2,0,len+2);

		memcpy(textbufUCS2,req->pContent,len);
		mr_str_convert_endian_with_size((char *)textbufUCS2,len);

	}
	
	memset(&entry, 0, sizeof(VappUcEntryStruct));
	memset(&uc_addr, 0, sizeof(srv_uc_addr_struct));

	entry.type = SRV_UC_STATE_WRITE_NEW_MSG;
	entry.msg_type = SRV_UC_MSG_TYPE_SMS_PREFER;
	entry.addr_num = 0;

	uc_addr.type = SRV_UC_ADDRESS_TYPE_PHONE_NUMBER;
	uc_addr.group = SRV_UC_ADDRESS_GROUP_TYPE_TO;

	//地址添加,代码留着
	//uc_addr.addr = (kal_uint8*)L"10086";	
	//entry.addr = &uc_addr;
	
	entry.text_buffer = (U8*)textbufUCS2;
	entry.text_num = len / 2;

	vappUcAppLauncher(&entry);

	if(textbufUCS2 != NULL)
		OslMfree(textbufUCS2);
			
	return MR_SUCCESS;
#endif

	return MR_FAILED;

#endif	// __COSMOS_MMI_PACKAGE__
}

int32 mr_sms_get_sms_status(int32 param)
{
#if (MTK_VERSION >=0x09B0952)	//#if (MTK_VERSION >=09B1012)  eric
	   if(srv_sms_check_send_action_pending(SRV_SMS_FG_SEND)||(!srv_sms_is_sms_ready())||dsmSmsOpCount)
#else
	if((mmi_frm_sms_get_sms_list_size(MMI_FRM_SMS_APP_INBOX) == MMI_FRM_SMS_INVALID_INDEX)
		||mmi_frm_sms_check_action_pending())
#endif
	{
		return MR_SMS_NOT_READY;
	}
	else
	{
		return MR_SMS_READY;
	}
}


U8 dsmIsSmsDisplay(void)
{
	kal_prompt_trace(MOD_MMI,"newSmsDisplay =%d",newSmsDisplay);
	return newSmsDisplay;
}


int32 mr_sms_enable_new_display(int32 param)
{
	if(0 == param)
	{
		newSmsDisplay = 0;
	}
	else
	{
		newSmsDisplay = 1;
	}
	
	return MR_SUCCESS;
}


int32 mr_sms_get_sms_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_DSM_GET_SMS_INFO_REQ *pReq = (T_DSM_GET_SMS_INFO_REQ*)input;
	U8 ret;
	
	if((input == NULL)||(input_len < sizeof(T_DSM_GET_SMS_INFO_REQ)))
		return MR_FAILED;
		
	if (srv_sms_is_sms_ready() == MMI_FALSE)
		return MR_FAILED;

	ret = dsm_get_sms_info(pReq);
	
	if( ret== MR_SUCCESS)
	{
		*output = (uint8 *)&gdsmSMSInfo;
		*output_len = sizeof(gdsmSMSInfo);
		return MR_SUCCESS;
	}
	else
	{
		return ret;
	}
}

int32 mr_sms_save_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if((input == NULL)||(input_len < sizeof(T_MR_SMS_SAVE_ENTRY_REQ)))
	{ 
		return MR_FAILED;
	} 
	 
	return mr_sms_save_msg((T_MR_SMS_SAVE_ENTRY_REQ *)input);
}
int32 mr_sms_reply_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if((input == NULL)||(input_len < sizeof(T_MR_SMS_REPLY_ENTRY_REQ)))
	{
		return MR_FAILED;
	}
	
	return mr_sms_reply_msg((T_MR_SMS_REPLY_ENTRY_REQ *)input);
}

#ifdef __MR_CFG_SET_SMS_STATUS__

int32 mr_sms_set_sms_status(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_DSM_SET_SMS_STATE_REQ *pReq = (T_DSM_SET_SMS_STATE_REQ*)input;
	U8 ret; 
	
	kal_prompt_trace(MOD_MMI,"mr_sms_set_sms_state 111,%d",ret);
	if((input == NULL)||(input_len < sizeof(T_DSM_SET_SMS_STATE_REQ)))
		return MR_FAILED;
		
	if (srv_sms_is_sms_ready() == MMI_FALSE)
		return MR_FAILED;
  
	ret = mr_set_sms_status(pReq);
	 
	return ret;
	
}
#endif
#endif

