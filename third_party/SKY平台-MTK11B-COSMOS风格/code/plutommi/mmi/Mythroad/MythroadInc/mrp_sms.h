#ifndef __MRP_PAL_SMS_H__
#define __MRP_PAL_SMS_H__



typedef enum
{
         MR_SMSSAVE_DISP_SAVINGSCREEN = 0x0010,
         MR_SMSSAVE_DISP_RESULTSCREEN = 0x0020,
         MR_SMSSAVE_DISP_SIM_OPT = 0x0200,
         
         MR_SMSSAVE_TO_SIM1 = 0x0400,
         MR_SMSSAVE_TO_SIM2 = 0x0800,
         MR_SMSSAVE_TO_SIM3 = 0x0800,
         MR_SMSSAVE_TO_SIM4 = 0x0800,

         MR_SMSSAVE_ENUM_END
}T_MR_SMS_SAVE_FLAG_E;

//wsl add for new
#define MR_SMS_MAX_CONTENT          (160)

typedef struct
{
         S8 number[MR_PHB_MAX_NUMBER];
         S8 content[MR_SMS_MAX_CONTENT];
         int32 status;
         int32 flag;
}T_MR_SMS_SAVE_ENTRY_REQ;

typedef enum
{
         MR_SMSREPLY_SIM1 = 0,
         MR_SMSREPLY_SIM2 ,
         MR_SMSREPLY_SIM3 ,
         MR_SMSREPLY_SIM4 
}T_MR_SMS_SIMID_E;

typedef struct
{
         S8 number[MR_PHB_MAX_NUMBER];
         int32 simid;
}T_MR_SMS_REPLY_ENTRY_REQ;

typedef struct
{
	uint8 totalNum;             //sim numbers, max to 4
	uint8 curActive;            //index of simId[] witch actived
	uint16 dummy;              //no use, just for align
	int32 simId[4];              //mapping to plat sim socket id
	char* simNamePtr[4];   //sim卡名字,unicode
	uint16 simNameLen[4]; //名字长度
}T_DSM_DUALSIM_SET;


typedef struct
{
	int8	t_stamp [6];  /*t_stamp[0]年t_stamp[1]月t_stamp[2]日t_stamp[3]时t_stamp[4]分t_stamp[5]秒*/
	int8 num[21];		/*发送方号码*/  
	int8 sms_status; /*Read or Unread ?*/
	int8 sms_storage;
}T_DSM_MSG_INFO;


typedef struct
{
	char *pNum;
	char *pContent;
}T_PLAT_SENDSMS_REQ;


typedef struct
{
      char *addr;	// UNICODE
      char *content;	// UNICODE
      uint16* subject;	// UNICODE
      uint16* file_path;	// UNICODE
}T_DSM_MMS;


extern T_DSM_DUALSIM_SET dsmDualSim;


/*  these interfaces are provided for local pay */
typedef int32 (*SMS_CALLBACK)(uint8 *, int32, uint8 *, int32);

/**
 * \brief register the incoming sms callback
 *
 * \return 
 */
int32 mr_sms_register_cb(SMS_CALLBACK cb);

/**
 * \brief deregister the incoming sms callback
 *
 * \return 
 */
int32 mr_sms_deregister_cb(SMS_CALLBACK cb);


/**
 * \brief 短信模块初始化
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_initialize(void);


/**
 * \brief 短信模块释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_terminate(void);


/**
 * \brief 获取短信信息，将会更改T_DSM_MSG_INFO类型的全局变量，并将这个变量返回给MRP
 *
 * \param pReq	[in] 请求参数
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 dsm_get_sms_info(T_DSM_GET_SMS_INFO_REQ *pReq);


/**
 * \brief 删除短信
 *
 * \param pReq	[in] 请求参数
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_delete_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 读取短信内容
 *
 * \param pReq	[in] 请求参数
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_read_sms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 发送彩信
 *
 * \param pMMS	[in] 彩信内容及对方号码
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_send_mms(T_DSM_MMS *pMMS);


/**
 * \brief 调用发送短信界面,不会直接发送，用户可以继续编辑短信内容及发送地址
 *
 * \param req	[in] 短信内容及地址
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_send_sms_from_ui(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 获取短信条数
 *
 * \param param	[in] 短信位置，取值MR_SMS_INBOX，MR_SMS_OUTBOX, MR_SMS_DRAFTS,
 * \return 短信条数
 */
int32 mr_sms_get_sms_count(int32 param);


/**
 * \brief 获得SIM的短信存储空间的大小
 *	异步操作，通过事件MR_SMS_OP_EVENT,MR_SMS_OP_GET_ME_SIZE返回给MRP
 * 
 */
int32 mr_sms_get_memory_size(int32 param);


/**
 * \brief 获取短信中心号码,通过MR_SMS_GET_SC事件通知应用程序
 *
 */
int32 mr_sms_get_sc_req(int32 param);


/**
 * \brief 获取短信中心号码,通过cb函数通知应用程序
 *
 */
int32 mr_sms_get_sc_req_ex(void (*cb)(int32 result, char *sc_addr));


/**
 * \brief 获取短信状态，用来判断短信模块是否准备好了
 *
 * \return  
 *	- MR_SMS_NOT_READY 未准备好
 * 	- MR_SMS_READY 准备好
 */
int32 mr_sms_get_sms_status(int32 param);


/**
 * \brief 收到短信通知
 */
#if 0
 #if(MTK_VERSION >=0x09B0952)
int dsmReciveMsg(srv_sms_event_struct* event_data);
#else
int dsmReciveMsg(void *data);
#endif
#endif


/**
 * \brief 副卡收到短信通知, 在09B0952版本之后未使用
 */
int dsmReciveMsgSlave(void *data);


/**
 * \brief 是否显示短息到达信息，主要供平台调用获取应用层的设置
 */
U8 dsmIsSmsDisplay(void);


/** 
 * \brief 发送短信到指定的号码
 *
 * \param pNumber 	[in]要发送的目的号码
 * \param pContent	[in]要发送的内容
 * \param encode	[in]短信内容的编码
 * \return 
 *	 - MR_SUCCESS:发送成功(提交到网络)
 *	 - MR_FAILED :发送失败
 * 	 - MR_IGNORE :gsm模块未准备好
 */
int32 mr_sendSms(char* pNumber, char*pContent, int32 encode);


/**
 * \brief 获取短信信息
 */
int32 mr_sms_get_sms_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_sms_enable_new_display(int32 param);

#endif
