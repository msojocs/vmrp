
#ifdef __MMI_DSM_NEW__

#include "MMI_features.h"
#include "MMIDataType.h"
#include "ProtocolEvents.h"
#include "mmi_msg_struct.h"
#include "Soc_api.h"
#include "custom_config.h"
#if(MTK_VERSION>=0x0924)
#include "mmi_frm_queue_gprot.h"
#include "mmi_frm_mem_gprot.h"
#endif
#if(MTK_VERSION>=0x0852)
#include "QueueGprot.h"
#include "OslMemory.h"
#endif
#include "GlobalConstants.h"
#include "mmi_msg_struct.h"
#if(MTK_VERSION> 0x0852)
#include "mmi_frm_events_gprot.h"
#endif
#include "dtcntsrvgprot.h"
#include "dtcntsrviprot.h"
#include "gui.h"
#include "dataaccountstruct.h"
#include "SimCtrlSrvGprot.h"
#include "mrp_include.h"
#include "DtcntSrv.h"
#include "dtcntsrvintstruct.h"
#include "app_str.h"
#include "NwInfoSrvGprot.h"
#include "cbm_api.h"

#if (MTK_VERSION >= 0x0824)
#define SOCK_STREAM		SOC_SOCK_STREAM	
#define SOCK_DGRAM		SOC_SOCK_DGRAM	
#define PF_INET 			SOC_PF_INET
#endif

#ifdef __MR_CFG_FEATURE_OVERSEA__
#define UNET_CONTEXTS_NUM		1
#else
#define UNET_CONTEXTS_NUM		2
#endif
#define UNET_SOCKETS_NUM		5
#define UNET_ACCOUNT_APN_LEN 31
#define UNET_ACCOUNT_USERNAME_LEN 31
#define UNET_ACCOUNT_PASSWORD_LEN 31
#define UNET_ACCOUNT_DNS_LEN 4

typedef struct _mr_unet_mgr mr_unet_mgr;
typedef struct _mr_unet_context mr_unet_context;
typedef struct _mr_unet_socket mr_unet_socket;

typedef struct
{
	char apn[UNET_ACCOUNT_APN_LEN + 1];
	char username[UNET_ACCOUNT_USERNAME_LEN + 1];
	char password[UNET_ACCOUNT_PASSWORD_LEN + 1];
	uint8 dns[UNET_ACCOUNT_DNS_LEN];
	uint8 auth_type;
} mr_unet_account;

struct _mr_unet_context
{
	uint32 network_id;	// 这个表示MTK encode后的值
	mr_unet_account account;
	e_unet_sim_type sim;
	t_unet_act_cb act_cb;
	t_unet_dsn_cb dns_cb;		// 通知DNS调用状态
	uint32 acc_prof_id;	// 系统数据账户ID
	uint8 app_id;		// 系统底层要求的app id
	int32 state;		// 表示异步初始化时的结果
};

typedef enum
{
	SOCKET_STATE_CREATED,
	SOCKET_STATE_CONNECTING,
	SOCKET_STATE_CONNECTED,
	SOCKET_STATE_CLOSED
} mr_socket_state;

struct _mr_unet_socket
{
	int8 handle;	// native socket handle
	mr_socket_state state;
	mr_unet_context* context;	// socket是那个应用创建的	
};

struct  _mr_unet_mgr
{
	int32 context_num;	// 当前应用个数最大为UNET_MGR_CONTEXTS_NUM
	
	mr_unet_context context_blocks[UNET_CONTEXTS_NUM];
	mr_unet_socket socket_blocks[UNET_SOCKETS_NUM];
};


extern int dsm_data_account_save(const T_DSM_DATA_ACCOUNT* account);
extern int dsm_data_account_get(T_DSM_DATA_ACCOUNT* account);


static void mr_mmi_dtcnt_add_disp_list(U32 acct_id, const U8 *acct_name, const U8 *addr, U8 bearer_type, U32 account_type, U8 conn_type, U8 sim_info, U8 read_only)
{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)

#elif (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)
extern void mmi_dtcnt_add_disp_list(U32 acct_id, U8 bearer_type, mmi_dtcnt_acct_type_enum account_type, U8 conn_type, U8 sim_info, U8 read_only);

	mmi_dtcnt_add_disp_list(acct_id, bearer_type, account_type, conn_type, sim_info, read_only);
#else
extern void mmi_dtcnt_add_disp_list(U32 acct_id, const U8 *acct_name, const U8 *addr, U8 bearer_type, mmi_dtcnt_acct_type_enum account_type, U8 conn_type, U8 sim_info, U8 read_only);

	mmi_dtcnt_add_disp_list(acct_id, acct_name, addr, bearer_type, account_type, conn_type, sim_info, read_only);
#endif
}

static void mr_mmi_dtcnt_update_disp_list(U32 acct_id, const U8 *acct_name, const U8 *addr, U8 bearer_type, U32 account_type, U8 conn_type, U8 sim_info, U8 read_only)
{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)

#elif (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)
extern void mmi_dtcnt_update_disp_list(U32 acct_id, U8 bearer_type, mmi_dtcnt_acct_type_enum account_type, U8 conn_type, U8 sim_info, U8 read_only);

	mmi_dtcnt_update_disp_list(acct_id, bearer_type, account_type, conn_type, sim_info, read_only);
#else
extern void mmi_dtcnt_update_disp_list(U32 acct_id, const U8 *acct_name, const U8 *addr, U8 bearer_type, mmi_dtcnt_acct_type_enum account_type, U8 conn_type, U8 sim_info, U8 read_only);

	mmi_dtcnt_update_disp_list(acct_id, acct_name, addr, bearer_type, account_type, conn_type, sim_info, read_only);
#endif
}

static mr_unet_mgr* mr_unet_get_mgr(void)
{
	static mr_unet_mgr unet_mgr;

	return &unet_mgr;
}

static uint32 mr_unet_mgr_find_acc_prof_id(const WCHAR* account_name)
{
	int i, j;
	srv_dtcnt_result_enum ret;
	srv_dtcnt_store_prof_qry_struct store_prof_qry;
	srv_dtcnt_prof_str_info_qry_struct prof_str_info_qry;
	U16 name[SRV_DTCNT_PROF_MAX_ACC_NAME_LEN + 1] = {0};

	for (j = 0; j < SRV_SIM_CTRL_MAX_SIM_NUM; ++j)
	{
		memset(&store_prof_qry, 0, sizeof(store_prof_qry));
		store_prof_qry.qry_info.filters = SRV_DTCNT_STORE_QRY_TYPE_SIM | SRV_DTCNT_STORE_QRY_TYPE_BEARER_TYPE;
		store_prof_qry.qry_info.sim_qry_info = j + 1;
		store_prof_qry.qry_info.bearer_qry_info = SRV_DTCNT_BEARER_GPRS;

		ret = srv_dtcnt_store_qry_ids(&store_prof_qry);
		if (ret == SRV_DTCNT_RESULT_SUCCESS)
		{
			for (i = 0; i < store_prof_qry.num_ids; ++i)
			{
				prof_str_info_qry.dest = (S8*)name;
				prof_str_info_qry.dest_len = sizeof(name);
				srv_dtcnt_get_account_name(store_prof_qry.ids[i], &prof_str_info_qry, SRV_DTCNT_ACCOUNT_PRIMARY);
				
				if (kal_wstrcmp(account_name, name) == 0)
				{
					return store_prof_qry.ids[i];
				}
			}
		}	
	}

	return -1;	
}

static uint32 mr_unet_mgr_add_acc_prof_id(const WCHAR* account_name)
{
	srv_dtcnt_store_prof_data_struct store_prof_data;
	srv_dtcnt_prof_gprs_struct prof_gprs;
	uint32 acc_prof_id;
	srv_dtcnt_result_enum ret;

	memset(&prof_gprs, 0, sizeof(prof_gprs));
	prof_gprs.APN = "cmwap";	// 这个值在应用使用的时候会修改的
	prof_gprs.prof_common_header.sim_info = SRV_DTCNT_SIM_TYPE_1;
	prof_gprs.prof_common_header.AccountName = (const U8*)account_name;

	prof_gprs.prof_common_header.acct_type = SRV_DTCNT_PROF_TYPE_USER_CONF;
	
	prof_gprs.prof_common_header.px_service = SRV_DTCNT_PROF_PX_SRV_HTTP;
	prof_gprs.prof_common_header.use_proxy = KAL_TRUE;
	prof_gprs.prof_common_header.px_addr[0] = 10;
	prof_gprs.prof_common_header.px_addr[1] = 0;
	prof_gprs.prof_common_header.px_addr[2] = 0;
	prof_gprs.prof_common_header.px_addr[3] = 172;
	prof_gprs.prof_common_header.px_port = 80;
	
	strcpy(prof_gprs.prof_common_header.HomePage,"http://wap.51mrp.com");
	
	store_prof_data.prof_data = &prof_gprs;
	store_prof_data.prof_fields = SRV_DTCNT_PROF_FIELD_ALL;
	store_prof_data.prof_type = SRV_DTCNT_BEARER_GPRS;

	ret = srv_dtcnt_store_add_prof(&store_prof_data, &acc_prof_id);

	if (ret == SRV_DTCNT_RESULT_SUCCESS)
	{
		mr_mmi_dtcnt_add_disp_list(acc_prof_id, prof_gprs.prof_common_header.AccountName, prof_gprs.APN, DATA_ACCOUNT_BEARER_GPRS,
			prof_gprs.prof_common_header.acct_type, 1, prof_gprs.prof_common_header.sim_info, prof_gprs.prof_common_header.read_only);	
	}
	else
	{
		acc_prof_id = -1;
	}	

	return acc_prof_id;
}

static uint32 mr_unet_mgr_find_acc_prof_id_by_apn(const char* apn, e_unet_sim_type sim)
{
	extern srv_dtcnt_store_info_context g_srv_dtcnt_store_ctx;
	int i;

	for (i = 0; i < SRV_DTCNT_PROF_MAX_ACCOUNT_NUM; ++i)
	{
		if (g_srv_dtcnt_store_ctx.acc_list[i].in_use &&
			g_srv_dtcnt_store_ctx.acc_list[i].bearer_type == SRV_DTCNT_BEARER_GPRS &&
			g_srv_dtcnt_store_ctx.acc_list[i].sim_info == (sim + 1) &&
			app_stricmp((char*)g_srv_dtcnt_store_ctx.acc_list[i].dest_name, (char*)apn) == 0)            
		{       
			return g_srv_dtcnt_store_ctx.acc_list[i].acc_id;
		}
	}

	return -1;		
}

static BOOL mr_unet_mgr_update_acc_prof_id(uint32 acc_prof_id, const WCHAR* account_name,
				const char* apn, const t_unet_apn_account* accountInfo, e_unet_sim_type sim )
{
	srv_dtcnt_result_enum ret;
	srv_dtcnt_store_prof_data_struct prof_info;
	srv_dtcnt_prof_gprs_struct prof_gprs;
	int i;

	MMI_ASSERT(apn != NULL);
	
	memset(&prof_gprs, 0, sizeof(prof_gprs));
	prof_gprs.APN = apn;
	prof_gprs.prof_common_header.sim_info = sim + 1;
	prof_gprs.prof_common_header.AccountName = (const U8*)account_name;

	if (accountInfo)
	{
		prof_gprs.prof_common_header.Auth_info.AuthType = accountInfo->authentication_type;
		strcpy((char*)prof_gprs.prof_common_header.Auth_info.UserName, accountInfo->userName);
		strcpy ((char*)prof_gprs.prof_common_header.Auth_info.Passwd, accountInfo->passWord);
		for ( i = 0; i < 4; i++) {
			prof_gprs.prof_common_header.PrimaryAddr[i] = accountInfo->dns[i];
		}		
	}

	prof_gprs.prof_common_header.acct_type = SRV_DTCNT_PROF_TYPE_USER_CONF;
	
	prof_gprs.prof_common_header.px_service = SRV_DTCNT_PROF_PX_SRV_HTTP;
	if (app_stricmp((char*)apn, (char*)"cmwap") == 0)
	{
		prof_gprs.prof_common_header.use_proxy = KAL_TRUE;
		prof_gprs.prof_common_header.px_addr[0] = 10;
		prof_gprs.prof_common_header.px_addr[1] = 0;
		prof_gprs.prof_common_header.px_addr[2] = 0;
		prof_gprs.prof_common_header.px_addr[3] = 172;
		prof_gprs.prof_common_header.px_port = 80;
	}
	
	strcpy(prof_gprs.prof_common_header.HomePage,"http://wap.51mrp.com");	

	prof_info.prof_data = &prof_gprs;
	prof_info.prof_fields = SRV_DTCNT_PROF_FIELD_ALL;
	prof_info.prof_type = SRV_DTCNT_BEARER_GPRS;

	ret = srv_dtcnt_store_update_prof(acc_prof_id, &prof_info);

	if (ret == SRV_DTCNT_RESULT_SUCCESS)
	{
		mr_mmi_dtcnt_update_disp_list(acc_prof_id, prof_gprs.prof_common_header.AccountName, prof_gprs.APN, DATA_ACCOUNT_BEARER_GPRS,
			prof_gprs.prof_common_header.acct_type, 1, prof_gprs.prof_common_header.sim_info, prof_gprs.prof_common_header.read_only);	

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/**
  * 获取分配给应用的数据账号，可以支持两个应用同时使用
  *
  * @param 应用handle
  *
  * @return 账号索引，这个值肯定是有效的
  */
static uint32 mr_unet_mgr_get_acc_prof_id(mr_unet_context* context)
{
	// 1. 搜索系统中存在的数据账户
	// 2. 检查虚拟机平台账号是否已存在
	// 3. 如果存在则返回这个账号ID，如果不存在继续
	// 4. 添加一个虚拟机平台数据账号
	// 5. 如果成功 返回账号ID，如果失败则返回指定的账号ID

	const WCHAR* names[] = { L"KNU1 GPRS", L"KNU2 GPRS" };
	uint32 modify_acc_prof_ids[] = { SRV_DTCNT_PROF_MAX_ACCOUNT_NUM - 1, SRV_DTCNT_PROF_MAX_ACCOUNT_NUM - 2 };
	uint32 acc_prof_id;
	int32 id;
	t_unet_apn_account unet_apn_account;

	id = context - mr_unet_get_mgr()->context_blocks;
	
	MMI_ASSERT(id >=0 && id <= 1);

	acc_prof_id = mr_unet_mgr_find_acc_prof_id_by_apn(context->account.apn, context->sim);

	if (acc_prof_id != -1)
	{
		return acc_prof_id;
	}

	acc_prof_id = mr_unet_mgr_find_acc_prof_id(names[id]);

	if (acc_prof_id == -1)
	{
		acc_prof_id = mr_unet_mgr_add_acc_prof_id(names[id]);
	}

	if (acc_prof_id == -1)
	{
		if (id == 0)
		{
			acc_prof_id = mr_unet_mgr_find_acc_prof_id(names[1]);
		}
		else
		{
			acc_prof_id = mr_unet_mgr_find_acc_prof_id(names[0]);
		}

		if (acc_prof_id == -1)
		{
			acc_prof_id = modify_acc_prof_ids[id];
		}
		else
		{
			if (acc_prof_id == SRV_DTCNT_PROF_MAX_ACCOUNT_NUM - 1)
			{
				acc_prof_id = acc_prof_id - 1;
			}
			else
			{
				acc_prof_id = acc_prof_id + 1;
			}
		}
	}

	unet_apn_account.userName = context->account.username;
	unet_apn_account.passWord = context->account.password;
	memcpy(unet_apn_account.dns, context->account.dns, UNET_ACCOUNT_DNS_LEN);
	unet_apn_account.authentication_type = context->account.auth_type;
	if (!mr_unet_mgr_update_acc_prof_id(acc_prof_id, names[id], context->account.apn, &unet_apn_account, context->sim))
	{
		acc_prof_id = -1;
	}

	return acc_prof_id;
}

static const WCHAR* mr_unet_mgr_get_acc_prof_name(mr_unet_context* context)
{
	const WCHAR* names[] = { L"KNU1 GPRS", L"KNU2 GPRS" };
	int32 id;

	id = context - mr_unet_get_mgr()->context_blocks;	

	MMI_ASSERT(id >=0 && id <= 1);

	return names[id];
}

mr_unet_context* mr_unet_mgr_alloc_context(void)
{
	mr_unet_mgr* mgr;
	mr_unet_context* context;
	int i;

	mgr = mr_unet_get_mgr();

	context = NULL;
	for (i = 0; i  < UNET_CONTEXTS_NUM; ++i)
	{
		if (mgr->context_blocks[i].network_id == 0)
		{
			context = mgr->context_blocks + i;
			break;
		}
	}

	if (context != NULL)
		mgr->context_num++;

	if (mgr->context_num == 1)
	{
		mr_net_set_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND,(PsIntFuncPtr)unet_Soc_Notify,TRUE);
		mr_net_set_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND,(PsIntFuncPtr)unet_Soc_GetHBN_Notify,TRUE);		
	}

	return context;
}

void mr_unet_mgr_free_context(mr_unet_context* context)
{
	mr_unet_mgr* mgr;
	int i;

	mgr = mr_unet_get_mgr();

	// 可能需要先把打开的SOCKET关闭
	for (i = 0; i < UNET_SOCKETS_NUM; ++i)
	{
		if (mgr->socket_blocks[i].handle != -1 && mgr->socket_blocks[i].context == context)
		{
			unet_Soc_Close(i);
		}
	}

	memset(context, 0, sizeof(mr_unet_context));

	if (--mgr->context_num == 0)
	{
		mr_net_clear_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND,(PsIntFuncPtr)unet_Soc_Notify);
		mr_net_clear_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND,(PsIntFuncPtr)unet_Soc_GetHBN_Notify);		
	}
}

mr_unet_socket* mr_unet_mgr_alloc_socket(void)
{
	mr_unet_mgr* mgr;
	int i;

	mgr = mr_unet_get_mgr();

	for (i  = 0; i < UNET_SOCKETS_NUM; ++i)
	{
		if (mgr->socket_blocks[i].handle == -1)
		{
			return (mgr->socket_blocks + i);
		}
	}
	
	return NULL;
}

void mr_unet_mgr_free_socket(mr_unet_socket* socket)
{
	mr_unet_mgr* mgr;

	mgr = mr_unet_get_mgr();

	memset(socket, 0, sizeof(mr_unet_socket));
	socket->handle = -1;
}

// 把socket指针转换为数值的index,这样设计主要是为了和以前的兼容
int mr_unet_mgr_socket2index(mr_unet_socket* socket)
{
	mr_unet_mgr* mgr;

	mgr = mr_unet_get_mgr();

	return socket - mgr->socket_blocks;
}

mr_unet_socket* mr_unet_mgr_index2socket(int index)
{
	mr_unet_mgr* mgr;

	mgr = mr_unet_get_mgr();

	if (index < 0 && index >= UNET_SOCKETS_NUM)
		return NULL;
	
	return mgr->socket_blocks + index;
}

mr_unet_socket* mr_unet_mgr_find_socket_by_handle(int8 handle)
{
	mr_unet_mgr* mgr;
	int i;

	mgr = mr_unet_get_mgr();

	for (i = 0; i < UNET_SOCKETS_NUM; ++i)
	{
		if (mgr->socket_blocks[i].handle == handle)
		{
			return mgr->socket_blocks + i;
		}
	}

	return NULL;
}

mr_unet_context* mr_unet_mgr_find_context_by_request_id(kal_int32 request_id)
{
	mr_unet_mgr* mgr;
	mr_unet_context* context;
	int i;

	mgr = mr_unet_get_mgr();

	for (i = 0; i < UNET_CONTEXTS_NUM; ++i)
	{
		context = mgr->context_blocks + i;

		if (context->network_id != 0 && context == (mr_unet_context*)request_id)
			return context;
	}

	return NULL;
}

void mr_unet_mgr_init(void)
{
	mr_unet_mgr* mgr;
	int i;

	mgr = mr_unet_get_mgr();
	
	memset(mgr, 0, sizeof(mr_unet_mgr));

	for (i = 0; i < UNET_SOCKETS_NUM; ++i)
	{
		mgr->socket_blocks[i].handle = -1;
	}
}

static void mr_init_network_callback(void* user)
{
	mr_unet_context* context;

	mr_trace("mr_init_network_callback");
	
	context = (mr_unet_context*)user;

	if (context->network_id && context->act_cb != NULL)
	{
		context->act_cb(context->state);
	}
}

int32 mr_unet_context_init_network(mr_unet_context* context)
{
	srv_dtcnt_store_prof_data_struct store_prof_data;
	srv_dtcnt_prof_gprs_struct prof_gprs;	
	srv_dtcnt_result_enum ret;
	int i;

	memset(&prof_gprs, 0, sizeof(prof_gprs));
	prof_gprs.APN = context->account.apn;
	prof_gprs.prof_common_header.sim_info = context->sim + 1;
	prof_gprs.prof_common_header.AccountName = (const U8*)mr_unet_mgr_get_acc_prof_name(context);

	prof_gprs.prof_common_header.Auth_info.AuthType = context->account.auth_type;
	strcpy((char*)prof_gprs.prof_common_header.Auth_info.UserName, context->account.username);
	strcpy ((char*)prof_gprs.prof_common_header.Auth_info.Passwd, context->account.password);
	for ( i = 0; i < UNET_ACCOUNT_DNS_LEN; ++i) {
		prof_gprs.prof_common_header.PrimaryAddr[i] = context->account.dns[i];
	}		

	prof_gprs.prof_common_header.acct_type = SRV_DTCNT_PROF_TYPE_USER_CONF;
	
	prof_gprs.prof_common_header.px_service = SRV_DTCNT_PROF_PX_SRV_HTTP;
	if (app_stricmp(context->account.apn, "cmwap") == 0)
	{
		prof_gprs.prof_common_header.use_proxy = KAL_TRUE;
		prof_gprs.prof_common_header.px_addr[0] = 10;
		prof_gprs.prof_common_header.px_addr[1] = 0;
		prof_gprs.prof_common_header.px_addr[2] = 0;
		prof_gprs.prof_common_header.px_addr[3] = 172;
		prof_gprs.prof_common_header.px_port = 80;
	}

	strcpy(prof_gprs.prof_common_header.HomePage,"http://wap.51mrp.com");
	
	store_prof_data.prof_data = &prof_gprs;
	store_prof_data.prof_fields = SRV_DTCNT_PROF_FIELD_ALL;
	store_prof_data.prof_type = SRV_DTCNT_BEARER_GPRS;		

	ret = srv_dtcnt_store_update_prof(context->acc_prof_id, &store_prof_data);

	mr_trace("mr_unet_context_init_network:ret=%d", ret);
	
	if (ret == SRV_DTCNT_RESULT_SUCCESS)
	{
		mr_mmi_dtcnt_update_disp_list(context->acc_prof_id, prof_gprs.prof_common_header.AccountName, prof_gprs.APN, DATA_ACCOUNT_BEARER_GPRS,
			prof_gprs.prof_common_header.acct_type, 1, prof_gprs.prof_common_header.sim_info, prof_gprs.prof_common_header.read_only);	
	
		context->state = MR_SUCCESS;
	}
	else
	{
		context->state = MR_FAILED;
	}

	gui_start_timer_ex(100, mr_init_network_callback, context);

	return MR_WAITING;
}

void unet_ApnAL_Init(uint8 csdAccount, uint8 gprsAccount)
{
	mr_trace("unet_ApnAL_Init");

	mr_unet_mgr_init();
}

static int mr_unet_get_networkid(e_unet_sim_type simId)
{
	char plmn[SRV_MAX_PLMN_LEN + 1];

	srv_nw_info_get_nw_plmn(MMI_SIM1 << simId, plmn, sizeof(plmn));

	if (strncmp(plmn, "46002", 5) == 0
		|| strncmp((char *)plmn, "46000", 5) == 0
		|| strncmp((char *)plmn, "46007", 5) == 0)
	{
		return MR_NET_ID_MOBILE;
	} 
	else if(strncmp(plmn, "46001", 5) ==0)
	{
		return MR_NET_ID_CN;
	} 
	else if(strncmp(plmn, "46003", 5) == 0)
	{
		return MR_NET_ID_CDMA;
	}

	return MR_NET_ID_OTHER;
}

int32 unet_ApnAL_ActiveCntx(char *apn, t_unet_apn_account *accountInfo, t_unet_act_cb cb, e_unet_sim_type simId, int32* appHandle)
{
	mr_unet_context* context;

	mr_trace("unet_ApnAL_ActiveCntx:apn=%s,sim=%d,*app=0x%x", apn, simId, appHandle);
	
	context = mr_unet_mgr_alloc_context();

	if (context == NULL)
		return MR_FAILED;

	// 先处理WIFI联网的情况
	if (*appHandle == UNET_SPEC_WIFI_HANDLE)
	{
		cbm_get_valid_account_id(CBM_BEARER_WIFI, &context->acc_prof_id);
		cbm_register_app_id(&context->app_id);
		cbm_hold_bearer(context->app_id);
		// 无SIM卡时用下边的函数WIFI联网不成功
		//context->network_id = cbm_encode_data_account_id(context->acc_prof_id, CBM_SIM_ID_SIM1, context->app_id, 0);
		context->network_id = cbm_set_app_id(context->acc_prof_id, context->app_id);
		
		*appHandle = (int32)context;	// 以前的版本在WIFI时没有返回这个值，不知道现在这么做会不会有问题
		return MR_SUCCESS;
	}

	// 处理GPRS拨号的情况	
	if (mr_unet_get_networkid(simId) == MR_NET_ID_CN)
	{
		if (app_stricmp(apn, "cmwap")  == 0)
		{
			strncpy(context->account.apn, "uniwap", UNET_ACCOUNT_APN_LEN);
		}
		else
		{
			strncpy(context->account.apn, "uninet", UNET_ACCOUNT_APN_LEN);
		}
	}
	else
	{
		strncpy(context->account.apn, apn, UNET_ACCOUNT_APN_LEN);
	}
	
	if (accountInfo != NULL)
	{
		strncpy(context->account.username, accountInfo->userName, UNET_ACCOUNT_USERNAME_LEN);
		strncpy(context->account.password, accountInfo->passWord, UNET_ACCOUNT_PASSWORD_LEN);
		memcpy(context->account.dns, accountInfo->dns, sizeof(context->account.dns));
		context->account.auth_type = accountInfo->authentication_type;	
	}

	context->sim = simId;
	context->act_cb = cb;
	context->acc_prof_id = mr_unet_mgr_get_acc_prof_id(context);

	if (context->acc_prof_id == -1)
	{
		mr_unet_mgr_free_context(context);
		return MR_FAILED;
	}
	
	cbm_register_app_id(&context->app_id);
	cbm_hold_bearer(context->app_id);
	context->network_id = cbm_encode_data_account_id(context->acc_prof_id, context->sim, context->app_id, 0);

	*appHandle = (int32)context;
	return MR_SUCCESS;	
}

int32 unet_ApnAL_CloseCntx(int32 *appHandle)
{
	mr_unet_context* context;

	mr_trace("unet_ApnAL_CloseCntx:app=0x%x", *appHandle);

	context = (mr_unet_context*)*appHandle;
	*appHandle = UNET_INVALID_HANDLE;

	//防止 有些应用还没初始化就关闭和反复关闭
	if (context == NULL || context->network_id == 0) return MR_SUCCESS;

	// 终止域名解析
	if (context->dns_cb != NULL)
	{
		soc_abort_dns_query(KAL_FALSE, 0, KAL_TRUE, (kal_uint32)context, KAL_FALSE, 0, KAL_FALSE, 0);
	}

	cbm_release_bearer(context->app_id);
	cbm_deregister_app_id(context->app_id);
	
	mr_unet_mgr_free_context(context);
	
	return MR_SUCCESS;
}

int32 unet_ApnAl_ActiveCntx_Cust(t_unet_act_cb cb, e_unet_sim_type simId, int32 *appHandle)
{
	T_DSM_DATA_ACCOUNT dsm_account;
	t_unet_apn_account unet_account;

	if (dsm_data_account_get(&dsm_account) < 0) return MR_FAILED;

	unet_account.userName = (char*)dsm_account.user_name;
	unet_account.passWord = (char*)dsm_account.password;
	unet_account.authentication_type = dsm_account.authentication_type;
	memcpy(unet_account.dns, dsm_account.dns, 4);

	return unet_ApnAL_ActiveCntx((char*)dsm_account.apn, &unet_account, cb, simId, appHandle);
}

int32 unet_Soc_GetHostByName(const char *name, t_unet_dsn_cb cb, int32 appHandle)
{
	mr_unet_context* context;
	int32 ret;
	uint32 addr;
	uint8 addr_len;

	context = (mr_unet_context*)appHandle;

	mr_trace("unet_Soc_GetHostByName:name=%s,cb=0x%x,app=0x%x", name, cb, appHandle);
	
	if (context == NULL) return MR_FAILED;

	addr_len = sizeof(addr);
	ret = soc_gethostbyname(KAL_FALSE, MOD_MMI, (int32)context, name, (kal_uint8*)&addr, &addr_len, 0, context->network_id);

	if (ret == SOC_SUCCESS)
	{
		return soc_ntohl(addr);
	}
	else if (ret == SOC_WOULDBLOCK)
	{
		context->dns_cb = cb;

		return MR_WAITING;
	}
	
 	return MR_FAILED;
}

int32 unet_Soc_Create(int32 type, int32 protocol, int32 appHandle)
{
	mr_unet_socket* socket;
	mr_unet_context* context;
	socket_type_enum soc_type;
	uint8 val;

	mr_trace("unet_Soc_Create:type=%d,protocol=%d,app=%x", type, protocol, appHandle);
	
	socket = mr_unet_mgr_alloc_socket();

	if (socket == NULL)
		return MR_FAILED;

	context = (mr_unet_context*)appHandle;

	soc_type = (type == MR_SOCK_STREAM) ? SOC_SOCK_STREAM : SOC_SOCK_DGRAM;
	
	socket->handle = soc_create(SOC_PF_INET, soc_type, 0, MOD_MMI, context->network_id);

	mr_trace("unet_Soc_Create:socket->handle=%d", socket->handle);

	if (socket->handle < 0)
	{
		mr_unet_mgr_free_socket(socket);
		return MR_FAILED;
	}

	val = KAL_TRUE;
	soc_setsockopt(socket->handle, SOC_NBIO, &val, sizeof(val));

	val = SOC_CONNECT|SOC_READ|SOC_WRITE | SOC_CLOSE;
	soc_setsockopt(socket->handle, SOC_ASYNC, &val, sizeof(val));
	

	socket->state = SOCKET_STATE_CREATED;
	socket->context = context;
	
	return mr_unet_mgr_socket2index(socket);
 }

int32 unet_Soc_Connect(int32 s, int32 ip, uint16 port, int32 type)
{
	mr_unet_socket* socket;
	sockaddr_struct addr;
	kal_uint32 nip;	// network order ip addr
	int8 r;
	int32 ret;
	
	mr_trace("unet_Soc_Connect:s=%d,ip=0x%x,port=%d,type=%d", s, ip, port, type);

	socket = mr_unet_mgr_index2socket(s);

	if (socket == NULL)
		return MR_FAILED;

	nip = soc_htonl(*((kal_uint32*)&ip));
	memcpy(addr.addr, &nip, sizeof(nip));
	addr.addr_len = sizeof(nip);
	addr.port = port;
	
	r = soc_connect(socket->handle, &addr);

	if (r == SOC_SUCCESS)
	{
		socket->state = SOCKET_STATE_CONNECTED;
		ret = MR_SUCCESS;
	}
	else if (r == SOC_WOULDBLOCK)
	{
		if (type == MR_SOCKET_NONBLOCK)
		{
			socket->state = SOCKET_STATE_CONNECTING;
			ret = MR_WAITING;
		}
		else
		{
			socket->state = SOCKET_STATE_CONNECTING;
			ret = MR_SUCCESS;			
		}
	}
	else
	{
		ret = MR_FAILED;
	}

	mr_trace("unet_Soc_Connect:ret=%d",ret);
	
	return ret;
}

int32 unet_Soc_State(int32 s)
{
	mr_unet_socket* socket;
	int32 ret = MR_FAILED;

	mr_trace("unet_Soc_State:s=%d", s);
	
	socket = mr_unet_mgr_index2socket(s);

	if (socket != NULL)
	{
		if (socket->state == SOCKET_STATE_CONNECTED)
		{
			ret = MR_SUCCESS;
		}
		else if (socket->state == SOCKET_STATE_CONNECTING)
		{
			ret = MR_WAITING;
		}
		else
		{
			ret = MR_FAILED;
		}
	}
		
	return ret;
}

int32 unet_Soc_Close(int32 s)
{
	mr_unet_socket* socket;

	mr_trace("unet_Soc_Close:s=%d", s);
	
	socket = mr_unet_mgr_index2socket(s);

	if (socket != NULL && socket->handle != -1)
	{
		soc_close(socket->handle);
		mr_unet_mgr_free_socket(socket);
	}
	
	return MR_SUCCESS;
}

int32 unet_Soc_Recv(int32 s, const char *buf, int len)
{
	mr_unet_socket* socket;
	int32 recv_len;
	int32 ret;

	mr_trace("unet_Soc_Recv:s=%d,buf=%d,len=%d", s, buf, len);

	socket = mr_unet_mgr_index2socket(s);

	if ((socket == NULL) && (socket->handle == -1))
		return MR_FAILED;

	if (socket->state == SOCKET_STATE_CONNECTED)
	{
		recv_len = soc_recv(socket->handle, (void*)buf, len, 0);

		if (recv_len > 0)
		{
			ret = recv_len;
		}
		else if (recv_len == SOC_WOULDBLOCK)
		{
			ret = 0;
		}
		else
		{
			ret = MR_FAILED;
		}
	}
	else if (socket->state == SOCKET_STATE_CREATED || socket->state == SOCKET_STATE_CONNECTING)
	{
		ret = 0;
	}
	else
	{
		ret = MR_FAILED;
	}
	
	return ret;
}

int32 unet_Soc_Send(int32 s, const char *buf, int len)
{
	mr_unet_socket* socket;
	int32 ret;
	int32 send_len;

	mr_trace("unet_Soc_Send:s=%d,buf=%x,len=%d", s, buf, len);

	socket = mr_unet_mgr_index2socket(s);

	if ((socket == NULL) && (socket->handle == -1))
		return MR_FAILED;

	if (socket->state == SOCKET_STATE_CONNECTED)
	{
		send_len = soc_send(socket->handle, buf, len, 0);

		if (send_len >= 0)
		{
			ret = send_len;
		}
		else if (send_len == SOC_WOULDBLOCK)
		{
			ret = 0;
		}
		else
		{	
			ret = MR_FAILED;
		}
	}
	else if (socket->state == SOCKET_STATE_CREATED || socket->state == SOCKET_STATE_CONNECTING)
	{
		ret = 0;
	}
	else
	{
		ret = MR_FAILED;
	}
		
	return ret;
}

int32 unet_Soc_Sendto(int32 s, const char *buf, int len, int32 ip, uint16 port)
{
	mr_unet_socket* socket;
	sockaddr_struct addr;
	kal_uint32 nip;	// network order ip addr
	int32 ret;
	int32 r;

	mr_trace("unet_Soc_Sendto:s=%d,buf=%x,len=%d,ip=0x%x,port=%d", s, buf, len, ip, port);

	socket = mr_unet_mgr_index2socket(s);

	if (socket == NULL)
		return MR_FAILED;
	
	nip = soc_htonl(*((kal_uint32*)&ip));
	memcpy(addr.addr, &nip, sizeof(nip));
	addr.addr_len = sizeof(nip);
	addr.port = port;
	
	r = soc_sendto(socket->handle, buf, len, 0, &addr);

	if (r >= 0)
	{
		ret = r;
	}
	else if (r == SOC_WOULDBLOCK)
	{
		ret = 0;
	}
	else
	{
		ret = MR_FAILED;
	}
	
	return ret;
}

int32 unet_Soc_Recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port)
{
	mr_unet_socket* socket;
	sockaddr_struct addr;
	kal_uint32 nip;	// network order ip addr
	int32 ret;
	int32 r;

	MMI_ASSERT(buf != NULL && ip != NULL && port != NULL);

	mr_trace("unet_Soc_Recvfrom:s=%d,buf=%x,len=%d", s, buf, len);

	socket = mr_unet_mgr_index2socket(s);

	if (socket == NULL)
		return MR_FAILED;
	
	r = soc_recvfrom(socket->handle, buf, len, 0, &addr);

	memcpy(&nip, addr.addr, sizeof(nip));
	*ip = soc_ntohl(nip);
	*port = addr.port;
	
	if (r >= 0)
	{
		ret = r;
	}
	else if (r == SOC_WOULDBLOCK)
	{
		ret = 0;
	}
	else
	{
		ret = MR_FAILED;
	}
	
	return ret;
}

int32 unet_Soc_Bind(mr_bind_st *d_bind)
{
	mr_unet_socket* socket;
	sockaddr_struct addr;
	kal_uint32 nip;	// network order ip addr
	int32 ret;
	int32 r;

	MMI_ASSERT(d_bind != NULL);
		
	mr_trace("unet_Soc_Bind:s=%d,port=%d,ip=0x%x", d_bind->socket, d_bind->port, d_bind->ip);

	socket = mr_unet_mgr_index2socket(d_bind->socket);
	if (socket == NULL)
		return MR_FAILED;

	nip = soc_htonl(d_bind->ip);
	memcpy(addr.addr, &nip, sizeof(nip));
	addr.addr_len = sizeof(nip);
	addr.port = d_bind->port;	

	r = soc_bind(socket->handle, &addr);
	if (r == SOC_SUCCESS)
		ret = MR_SUCCESS;
	else
		ret = MR_FAILED;

	return ret;
}

MMI_BOOL unet_Soc_Notify(void* inMsg)
{
	app_soc_notify_ind_struct* soc_notify = (app_soc_notify_ind_struct *)inMsg;
	mr_unet_socket* socket;

	mr_trace("unet_Soc_Notify:handle=%d,event=%d,result=%d", soc_notify->socket_id, soc_notify->event_type, soc_notify->result);

	socket = mr_unet_mgr_find_socket_by_handle(soc_notify->socket_id);

	if (socket == NULL) return MMI_FALSE;

	switch (soc_notify->event_type)
	{
	case SOC_CONNECT:
		if (soc_notify->result == KAL_TRUE)
			socket->state = SOCKET_STATE_CONNECTED;
		else
			socket->state = SOCKET_STATE_CLOSED;
			
		break;

	case SOC_CLOSE:
		socket->state = SOCKET_STATE_CLOSED;
		break;
	}
	
	return MMI_TRUE;
}

MMI_BOOL unet_Soc_GetHBN_Notify(void* inMsg)
{
	app_soc_get_host_by_name_ind_struct *dns_ind = (app_soc_get_host_by_name_ind_struct *)inMsg;
	mr_unet_context* context;
	int32 ret;
	t_unet_dsn_cb cb;
	
	mr_trace("unet_Soc_GetHBN_Notify:request_id=0x%x,result=%d", dns_ind->request_id, dns_ind->result);

	context = mr_unet_mgr_find_context_by_request_id(dns_ind->request_id);

	if (context == NULL) return MMI_FALSE;
	
	if (context->dns_cb != NULL)
	{
		if (dns_ind->result)
		{
			memcpy(&ret, dns_ind->addr, 4);
			ret = soc_ntohl(ret);
		}
		else
		{
			ret = MR_FAILED;
		}
		// 防止有些应用会在域名解决的回调里再调用域名解析
		cb = context->dns_cb;
		context->dns_cb = NULL;
		cb(ret);
	}
	
	return MMI_TRUE;
}

void unet_Soc_Break(int32 appHandle)
{
	// 这个函数应该不需要了
}

void unet_Soc_CloseAll(void)
{
	// 这个函数应该不需要了
}
#ifdef __MMI_DSM_NEW_JSKY__
int32 unet_Soc_SetOpt(int32 s, uint32 option,void* value,uint8 valueLen)
{
	mr_unet_socket* socket = NULL;
	
	mr_trace("unet_Soc_SetOpt:s=%d,%d,%d,%d", s,option,value,valueLen);
	
	socket = mr_unet_mgr_index2socket(s);
	if(socket != NULL)
	{
		if(0 == soc_setsockopt(socket->handle, option, value, valueLen))
		{
			return MR_SUCCESS;
		}
	}
	return MR_FAILED;
}

int32 unet_Soc_GetOpt(int32 s,uint32 option,void * value,uint8 valueLen)
{
	mr_unet_socket* socket = NULL;
	mr_trace("unet_Soc_GetOpt:s=%d,%d,%d,%d", s,option,value,valueLen);
	
	socket = mr_unet_mgr_index2socket(s);
	if((value != NULL) &&(socket != NULL))
	{
		if(0 == soc_getsockopt(socket->handle, option, value, valueLen))
		{
			return MR_SUCCESS;
		}
	}
	return MR_FAILED;
}
#endif

#endif

