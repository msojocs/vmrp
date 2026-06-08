#ifdef __MMI_DSM_NEW__

#ifdef WIN32
#include "windows.h"
#endif
#include "MMI_Include.h"
#include "ProtocolEvents.h"
#include "custom_data_account.h"
#include "APP2SOC_STRUCT.H"
#include "soc_api.h"
#include "FileManagerGprot.h"

#include "nwinfosrvgprot.h"
#include "mrp_include.h"

#if(MTK_VERSION <= 0x0812)
#include "Mmi_msg_struct.h"
#endif

#if(MTK_VERSION >=0x0924)
#include "smsapptype.h"
#endif

#if (MTK_VERSION <= 0x0936)
#include "MMSAppProt.h"
#endif

#if (MTK_VERSION > 0x0812)
#include "Browser_api.h"
#endif
#if (MTK_VERSION <= 0x0812)
#include "wapadp.h"
#endif
#include "cbm_api.h"
#include "DtcntSrvIprot.h"
#include "DtcntSrvGprot.h"
#include "vcui_dtcnt_select_gprot.h"

#ifndef WAP_MAX_URL_LENGTH
#define WAP_MAX_URL_LENGTH 255
#endif

extern T_DSM_DUALSIM_SET dsmDualSim;

static int32 dsmNetworHandle = UNET_INVALID_HANDLE;

int32 gDsmUseNVdataAccount = FALSE;
T_DSM_DATA_ACCOUNT gDsmDataAccount ={0};

#ifdef __MMI_DSM_WIFI_SUPPORT__
BOOL isDsmVIAWIFI = FALSE;
static int32 dsmWIFIId = 0;
#endif


kal_uint8 s_dsm_app_id = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int32 mr_net_initialize(void)
{
	{
		extern void unet_ApnAL_Init(uint8 csdAccount, uint8 gprsAccount);
		unet_ApnAL_Init(0, 0);
	}

#ifndef __MR_CFG_FEATURE_SLIM__
	mr_get_gprs_info_load();
#endif

	return MR_SUCCESS;
}


int32 mr_net_terminate(void)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	mr_net_gprs_info_save();
#endif
	return MR_SUCCESS;
}


void mr_net_set_protocol_event_handler(U16 eventID, PsIntFuncPtr funcPtr, MMI_BOOL isMultiHandler)
{
#if (MTK_VERSION > 0x0848)
	#if (MTK_VERSION >= 0x0936)
		mmi_frm_set_protocol_event_handler(eventID, (PsIntFuncPtr)funcPtr, isMultiHandler);
	#else
		mmi_frm_set_protocol_event_handler(eventID, (PsIntFuncPtr)funcPtr, isMultiHandler);
	#endif
#else
	 mmi_frm_set_protocol_event_handler((PsFuncPtr)funcPtr, eventID);
#endif
}


void mr_net_clear_protocol_event_handler(U16 eventID, PsIntFuncPtr funcPtr)
{
#if (MTK_VERSION > 0x0848)
	#if (MTK_VERSION >= 0x0936)
	mmi_frm_clear_protocol_event_handler(eventID, (PsIntFuncPtr)funcPtr);
	#else
	mmi_frm_clear_protocol_event_handler(eventID, (PsIntFuncPtr)funcPtr);
	#endif
#else
	mmi_frm_clear_protocol_event_handler(eventID);
#endif
}


/****************************************************************************
函数名: int32 dsm_getUnetActSimId(void)
描  述:  获取当前sim id
参  数: voids
返  回: sim id(for unet)
****************************************************************************/
static int32 dsm_getUnetActSimId(void)
{
#if 0
	return mr_sim_get_active() == DSM_SLAVE_SIM? UNET_SIM_SLAVE : UNET_SIM_MASTER;
#else
	return (-mr_sim_get_active()) - 1;
#endif
}


int32 mr_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode)
{
	const char* apnMode = mode != NULL ? mode : "cmnet";
	char* plmn;
	int32 plmn_len;

	//jack.ru start  11a1112 MSN WIFI死机问题和MSN掉线问题修改
	//如果有遇到该情况，请打开如下代码试试看。
	#if 0
	if(dsmNetworHandle != UNET_INVALID_HANDLE)
		return MR_SUCCESS;
	#endif
	//jack.ru end
	
	mr_net_get_network_plmn((uint8**)&plmn, &plmn_len);

#ifdef MMI_ON_HARDWARE_P	
	kal_prompt_trace(MOD_MMI,"mr_initNetwork() apn = %s",apnMode);
	kal_prompt_trace(MOD_MMI,"gDsmUseNVdataAccount=%d",gDsmUseNVdataAccount);
#endif

	// 这个提到前边，有些应用可能会没关闭网络就再初始化网络
	if(dsmNetworHandle != UNET_INVALID_HANDLE)
	{
		unet_ApnAL_CloseCntx(&dsmNetworHandle);//temp.maybe del. unet_ApnAl_ActiveCntx()中有保护
	}
	
#ifdef __MMI_DSM_WIFI_SUPPORT__
	if(isDsmVIAWIFI)
	{
		dsmNetworHandle = UNET_SPEC_WIFI_HANDLE;
		return unet_ApnAL_ActiveCntx((char *)"WIFI", NULL, cb, dsm_getUnetActSimId(), &dsmNetworHandle); 
	}
#endif

	if(gDsmUseNVdataAccount)
	{
		return unet_ApnAl_ActiveCntx_Cust(cb, dsm_getUnetActSimId(), &dsmNetworHandle);
	}
	else if(strncmp(plmn, "460", 3) != 0)
	{
		int32 len;
		unsigned char *netId = NULL;
		T_DSM_APN *pApn=NULL;		
		mr_net_get_network_plmn(&netId, &len);

#if !defined(WIN32) && defined(__MR_CFG_FEATURE_OVERSEA__)
		pApn = mr_getAPN(netId);
#endif

		if(pApn == NULL)
		{
			return MR_FAILED;
		}
		else
		{
			t_unet_apn_account account ={0};
			account.authentication_type = pApn->authentication_type;
			memcpy(account.dns ,pApn->dns,4);
			account.passWord = pApn->passWord;
			account.userName = pApn->userName;
			
			return unet_ApnAL_ActiveCntx((char*)pApn->apn, &account, cb, dsm_getUnetActSimId(), &dsmNetworHandle);
		}
	}
	
	return unet_ApnAL_ActiveCntx((char *)apnMode,NULL, cb, dsm_getUnetActSimId(), &dsmNetworHandle);
}


int32 mr_getHostByName(const char *name, MR_GET_HOST_CB cb)
{
	return unet_Soc_GetHostByName(name, cb, dsmNetworHandle);
}


int32 mr_socket(int32 type, int32 protocol)
{
	return unet_Soc_Create(type, protocol, dsmNetworHandle);
 }


int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type)
{
	return unet_Soc_Connect(s, ip, port, type);
}


int32 mr_closeSocket(int32 s)
{
	return unet_Soc_Close(s);
}


int32 mr_network_socket_closeall(void)
{
	{
		extern void unet_Soc_CloseAll(void);
		unet_Soc_CloseAll();
        }

	return MR_SUCCESS;
}


int32 mr_net_socket_breakall(void)
{
	unet_Soc_Break(dsmNetworHandle);

	return MR_SUCCESS;
}


int32 mr_recv (int32 s, char *buf, int len)
{
	return unet_Soc_Recv(s, buf, len);
}


int32 mr_send(int32 s, const char *buf, int len)
{
	return unet_Soc_Send(s, buf, len);
}


int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port)
{
	return unet_Soc_Sendto(s, buf, len,ip,port);
}


int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port)
{
	return unet_Soc_Recvfrom(s, buf, len,ip,port);
}


int32 mr_closeNetwork(void)
{
#ifdef __MMI_DSM_WIFI_SUPPORT__
	isDsmVIAWIFI = FALSE;
#endif	
	
	mr_network_socket_closeall();

	return unet_ApnAL_CloseCntx(&dsmNetworHandle);
}


int32 mr_net_check_connect(int32 param)
{
	return unet_Soc_State(param);
}


int32 mr_net_soc_bind(mr_bind_st *d_bind)
{
	return unet_Soc_Bind(d_bind);	
}

int dsm_data_account_save(const T_DSM_DATA_ACCOUNT* account)
{
	int drv;
	int file;
	WCHAR name[20] = L"C:\\unet_account.dat";
	UINT written = 0;
	
	drv = MMI_SYSTEM_DRV;

	if (drv < 0) return MR_FAILED;

	name[0] = drv;
	
	file = FS_Open(name, FS_READ_WRITE | FS_CREATE_ALWAYS);

	if (file < 0) return MR_FAILED;

	FS_Write(file, (void*)account, sizeof(T_DSM_DATA_ACCOUNT), &written);
	FS_Close(file);

	return MR_SUCCESS;
}


int dsm_data_account_get(T_DSM_DATA_ACCOUNT* account)
{
	int drv;
	int file;
	WCHAR name[20] = L"C:\\unet_account.dat";
	UINT read = 0;
	
	drv = MMI_SYSTEM_DRV;

	if (drv < 0) return MR_FAILED;

	name[0] = drv;
	
	file = FS_Open(name, FS_READ_ONLY);

	if (file < 0) return MR_FAILED;

	FS_Read(file, account, sizeof(T_DSM_DATA_ACCOUNT), &read);
	FS_Close(file);

	return MR_SUCCESS;
}

static void dsmSetDataAccountRsp(void *info)
{
	mr_event(MR_DATA_ACCOUNT_EVENT, MR_DATA_ACCOUNT_OP_SET, (int32)info);
}


int32 mr_net_set_data_account(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 ret;

	ret = dsm_data_account_save((T_DSM_DATA_ACCOUNT*)input);
	gui_start_timer_ex(100, dsmSetDataAccountRsp, (void*)ret);
	return MR_WAITING;
}


static void dsmGetDataAccountRsp(void *para)
{
	int32 ret;

	ret = dsm_data_account_get(&gDsmDataAccount);
	mr_event(MR_DATA_ACCOUNT_EVENT, MR_DATA_ACCOUNT_OP_GET, ret);
}


int32 mr_net_get_data_account_req(int32 param)
{
	gui_start_timer_ex(100, dsmGetDataAccountRsp, 0);
	return MR_WAITING;
}


static mmi_id dsm_dtcnt_cui_id = GRP_ID_INVALID;
static mmi_id mythroad_id = GRP_ID_INVALID;
static mmi_ret dsm_prof_proc(mmi_event_struct* evt)
{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144)
	cui_dtcnt_select_event_any_sim_selected_struct* dtcnt_select_event = (cui_dtcnt_select_event_any_sim_selected_struct*)evt;
#else
	cui_dtcnt_select_event_struct* dtcnt_select_event = (cui_dtcnt_select_event_struct*)evt;
#endif	
	
	switch (evt->evt_id)
	{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144)
		case CUI_DTCNT_SELECT_EVENT_ANY_SIM_ACCOUNT_SELECTED:
#else
		case CUI_DTCNT_SELECT_EVENT_RESULT_OK:
#endif
		{
			cbm_sim_id_enum cbm_sim_id;
			kal_uint8 cbm_app_id;
			kal_bool always_ask;
			kal_uint32 ori_acct_id;
			srv_dtcnt_store_prof_data_struct prof_data;
			srv_dtcnt_prof_gprs_struct prof_gprs;
			T_DSM_DATA_ACCOUNT dsm_account;
			int i;
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144)
			cbm_decode_data_account_id(dtcnt_select_event->accountId, &cbm_sim_id,
                                       &cbm_app_id, &always_ask, &ori_acct_id);
#else
			cbm_decode_data_account_id(dtcnt_select_event->accountId[dtcnt_select_event->selectSim], &cbm_sim_id,
                                       &cbm_app_id, &always_ask, &ori_acct_id);
#endif                                       

			prof_data.prof_data = &prof_gprs;
			prof_data.prof_fields = SRV_DTCNT_PROF_FIELD_ALL;
			prof_data.prof_type = SRV_DTCNT_BEARER_GPRS;
			if (srv_dtcnt_store_qry_prof(ori_acct_id, &prof_data) == SRV_DTCNT_RESULT_SUCCESS)
			{
				memset(&dsm_account, 0, sizeof(T_DSM_DATA_ACCOUNT));
				dsm_account.authentication_type = prof_gprs.prof_common_header.Auth_info.AuthType;
				strcpy((char*)dsm_account.user_name, (char*)prof_gprs.prof_common_header.Auth_info.UserName);
				strcpy ((char*)dsm_account.password, (char*)prof_gprs.prof_common_header.Auth_info.Passwd);
				for ( i = 0; i < 4; i++) {
					dsm_account.dns[i] = prof_gprs.prof_common_header.PrimaryAddr[i];
				}
				strcpy((char*)dsm_account.apn, (char*)prof_gprs.APN);
				dsm_data_account_save(&dsm_account);
			}
#if !(__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144)
			vcui_dtcnt_select_close(dsm_dtcnt_cui_id);
			dsm_dtcnt_cui_id = GRP_ID_INVALID;
			vapp_mythroad_terminate(mythroad_id);
#endif
			return MMI_RET_OK;
		}

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144)
		case CUI_DTCNT_SELECT_EVENT_RESULT_OK:
#endif
		case CUI_DTCNT_SELECT_EVENT_RESULT_CANCEL:
		case CUI_DTCNT_SELECT_EVENT_RESULT_FAILED:
		case CUI_DTCNT_SELECT_EVENT_CLOSE:
		{
			vcui_dtcnt_select_close(dsm_dtcnt_cui_id);
			dsm_dtcnt_cui_id = GRP_ID_INVALID;
			vapp_mythroad_terminate(mythroad_id);
			
			return MMI_RET_OK;
		}
	}

	return MMI_RET_DONT_CARE;
}


void dsmMtkUISetDataAccount(void)
{
	cui_dtcnt_select_run_struct cui_param;
	
	//mmi_frm_group_create(GRP_ID_ROOT, GRP_ID_MYTHROAD, dsm_prof_proc, NULL);
	//mmi_frm_group_enter(GRP_ID_MYTHROAD, MMI_FRM_NODE_SMART_CLOSE_FLAG);

	//dsm_dtcnt_cui_id = vcui_dtcnt_select_create(mmi_frm_group_get_active_id());
	mythroad_id = vapp_mythroad_launch(NULL, 0, dsm_prof_proc);
	dsm_dtcnt_cui_id = vcui_dtcnt_select_create(mythroad_id);
	//mmi_frm_group_set_caller_proc(dsm_dtcnt_cui_id, dsm_prof_proc, NULL);
	//vapp_mythroad_set_cui_proc(mythroad_id, dsm_dtcnt_cui_id, dsm_prof_proc);
	kal_prompt_trace(MOD_MMI, "#####dsm_dtcnt_cui_id=%d", dsm_dtcnt_cui_id);
	if (dsm_dtcnt_cui_id == GRP_ID_INVALID)
	{
		//mmi_frm_group_close(GRP_ID_MYTHROAD);
		return;
	}

	memset(&cui_param, 0, sizeof(cui_dtcnt_select_run_struct));
	cui_param.bearers = DATA_ACCOUNT_BEARER_GPRS;
	cui_param.app_id = 0;
	//cui_param.AppMenuID = SERVICES_WAP_MENU_ID;
	cui_param.option = 0;	// 这个参数必须为0，否则会把所有SIM卡的账号显示出来
	cui_param.type = CUI_DTCNT_SELECT_TYPE_NORMAL;
	cui_param.sim_selection = -mr_sim_get_active();		// - 1;	
	vcui_dtcnt_select_set_param(dsm_dtcnt_cui_id, &cui_param);

	vcui_dtcnt_select_run(dsm_dtcnt_cui_id);
}

static void mr_connectWAP_cb(void* url)
{
#ifdef BROWSER_SUPPORT
	wap_start_browser(WAP_BROWSER_GOTO_URL,(uint8*)url);
#endif	
}

void mr_connectWAP(char* wap)
{
#ifdef BROWSER_SUPPORT
	static int8 url[(WAP_MAX_URL_LENGTH + 1) * 2] = {0};
	
	if(wap == NULL || strlen(wap) == 0 || strlen(wap) > WAP_MAX_URL_LENGTH)
	{
		return;
	}
	
	mmi_asc_to_ucs2((CHAR*)url, wap);
	gui_start_timer_ex(100, mr_connectWAP_cb, url);
#endif	
}


static int32 mr_net_get_network_id_from_plmn(char* plmn)
{
#ifdef MMI_ON_HARDWARE_P
	if (memcmp((char *)plmn,"46002", 5) == 0
		|| memcmp((char *)plmn,"46000", 5) == 0
		|| memcmp((char *)plmn,"46007", 5) == 0)
	{
		return MR_NET_ID_MOBILE;
	} 
	else if(memcmp((char *)plmn, "46001", 5)==0)
	{
		return MR_NET_ID_CN;
	} 
	else if(memcmp((char *)plmn, "46003", 5) == 0)
	{
		return MR_NET_ID_CDMA;
	}
#endif

	return MR_NET_ID_OTHER;
}

int32 mr_getNetworkID(void)
{
	mmi_sim_enum sim;
	srv_nw_info_service_availability_enum nw_info_service;
	int32 dsm_sim;
	char plmn[SRV_MAX_PLMN_LEN + 1];

	sim = mr_sim_active_id_dsm2mmi();

	nw_info_service = srv_nw_info_get_service_availability(sim);

	if (nw_info_service != SRV_NW_INFO_SA_FULL_SERVICE)
	{
		return MR_NET_ID_NONE;
	}

	if (!srv_nw_info_get_nw_plmn(sim, plmn, sizeof(plmn)))
	{
		return MR_NET_ID_NONE;
	}

	return mr_net_get_network_id_from_plmn(plmn);
}


int32 mr_net_get_network_plmn(uint8**output,int32 *output_len)
{
	mmi_sim_enum sim;
	int32 dsm_sim;
	static char plmn[SRV_MAX_PLMN_LEN + 1];

	sim = mr_sim_active_id_dsm2mmi();

	memset(plmn, 0, sizeof(plmn));
	if (srv_nw_info_get_nw_plmn(sim, plmn, sizeof(plmn)))
	{
		*output = (uint8*)plmn;
		*output_len = SRV_MAX_PLMN_LEN;
		
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_net_data_account_set_use_flag(int32 param)
{
	extern int32 gDsmUseNVdataAccount;
	if(param)
	{
		gDsmUseNVdataAccount = 0;
	}
	else
	{
		gDsmUseNVdataAccount = 1; 
	}
	return MR_SUCCESS;
}


int32 mr_net_data_account_show_ui(int32 param)
{
	dsmMtkUISetDataAccount();
	return MR_SUCCESS;
}


int32 mr_net_get_data_account(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(output == NULL||output_len == NULL)
		return MR_FAILED;
	*output = (uint8 *)&gDsmDataAccount;
	*output_len = sizeof(gDsmDataAccount);
	return MR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __MR_CFG_FEATURE_SLIM__

typedef struct {
	char imsi[MAX_IMSI_LEN];
	mr_datetime timestamp;
}mr_net_gprs_info_t;

extern char gIMSI[];
#ifdef __MMI_DUAL_SIM_MASTER__		
static mr_net_gprs_info_t  g_gprs_info[2];
extern char gIMSI_2[];
#else
static mr_net_gprs_info_t  g_gprs_info[1];
#endif

void mr_get_gprs_info_load(void)
{
	if(g_gprs_info[0].timestamp.year == 0 
#ifdef __MMI_DUAL_SIM_MASTER__				
		&& g_gprs_info[1].timestamp.year == 0
#endif		
		)
	{
		U16 path[DSM_MAX_FILE_LEN];
		FS_HANDLE fd;
		UINT readBytes;
		
		kal_wsprintf(path, "%c:\\%s\\gprs.cfg", __MR_CFG_VAR_ROOT_DRV__, __MR_CFG_VAR_ROOT_PATH__);
		fd = FS_Open((WCHAR *)path, FS_READ_ONLY);
		if(fd <= 0) return;
		
		FS_Read(fd, &g_gprs_info, sizeof(g_gprs_info), &readBytes);
		FS_Close(fd);
	}

	mr_trace("mr_get_gprs_info_load: %s, %d, %s, %d", g_gprs_info[0].imsi, g_gprs_info[0].timestamp.year, g_gprs_info[1].imsi, g_gprs_info[1].timestamp.year);
}


void mr_net_gprs_info_save(void)
{
	if(g_gprs_info[0].timestamp.year 
#ifdef __MMI_DUAL_SIM_MASTER__				
		|| g_gprs_info[1].timestamp.year
#endif		
		)
	{
		U16 path[DSM_MAX_FILE_LEN];
		FS_HANDLE fd;
		UINT writeBytes;
		
		kal_wsprintf(path, "%c:\\%s\\gprs.cfg", __MR_CFG_VAR_ROOT_DRV__, __MR_CFG_VAR_ROOT_PATH__);
		fd = FS_Open((WCHAR *)path, FS_READ_WRITE | FS_CREATE);
		FS_Write(fd, &g_gprs_info, sizeof(g_gprs_info), &writeBytes);
		FS_Close(fd);
	}

	mr_trace("mr_net_gprs_info_save: %s, %d, %s, %d", g_gprs_info[0].imsi, g_gprs_info[0].timestamp.year, g_gprs_info[1].imsi, g_gprs_info[1].timestamp.year);
}


int32 mr_net_gprs_info_query(uint8 * input, int32 input_len, uint8 * * output, int32 * output_len, MR_PLAT_EX_CB * cb)
{
	if(!input || input_len != sizeof(mr_datetime))
		return MR_FAILED;

	if(mr_sim_get_active() == DSM_MASTER_SIM)
	{
		mr_trace("mr_net_gprs_info_query0: %s, %d, %d, %d", g_gprs_info[0].imsi, 
			g_gprs_info[0].timestamp.year, g_gprs_info[0].timestamp.month, g_gprs_info[0].timestamp.day);	
		
		if(strncmp(g_gprs_info[0].imsi, gIMSI, MAX_IMSI_LEN) == 0){
			memcpy(input, &g_gprs_info[0].timestamp, sizeof(mr_datetime));
			return MR_SUCCESS;
		}
	}else{
#ifdef __MMI_DUAL_SIM_MASTER__		
		mr_trace("mr_net_gprs_info_query1: %s, %d, %d, %d", g_gprs_info[0].imsi, 
			g_gprs_info[0].timestamp.year, g_gprs_info[0].timestamp.month, g_gprs_info[0].timestamp.day);	

		if(strncmp(g_gprs_info[1].imsi, gIMSI_2, MAX_IMSI_LEN) == 0){
			memcpy(input, &g_gprs_info[1].timestamp, sizeof(mr_datetime));
			return MR_SUCCESS;
		}
#endif
	}
	
	return MR_FAILED;
}


void mr_net_gprs_info_update(void)
{
	if(mr_sim_get_active() == DSM_MASTER_SIM)
	{
		memcpy(g_gprs_info[0].imsi, gIMSI, MAX_IMSI_LEN);
		mr_getDatetime(&g_gprs_info[0].timestamp);
		mr_trace("mr_net_gprs_info_update0: %s, %d, %d, %d", g_gprs_info[0].imsi, 
			g_gprs_info[0].timestamp.year, g_gprs_info[0].timestamp.month, g_gprs_info[0].timestamp.day);	
	}else{
#ifdef __MMI_DUAL_SIM_MASTER__	
		memcpy(g_gprs_info[1].imsi, gIMSI_2, MAX_IMSI_LEN);
		mr_getDatetime(&g_gprs_info[1].timestamp);
		mr_trace("mr_net_gprs_info_update1: %s, %d, %d, %d", g_gprs_info[1].imsi, 
			g_gprs_info[1].timestamp.year, g_gprs_info[1].timestamp.month, g_gprs_info[1].timestamp.day);	
#endif		
	}

#ifndef __MR_CFG_FEATURE_SLIM__
	mr_backstage_update_status_icons();
#endif
}
#endif
//默认使用全语言版本，huangsunbo 20111117
#if 0//def __MR_CFG_FEATURE_SLIM__
#define GET_APN(X)     pApn=(T_DSM_APN*)mr_getAPN##X(netId)

T_DSM_APN *mr_getAPN(unsigned char * netId)
{
#ifndef WIN32
	T_DSM_APN *pApn = NULL;
	if(
	(GET_APN(CHINESE))
	||(GET_APN(AFRICA))
	||(GET_APN(ASIA))
	||(GET_APN(EUROPE))
#if defined( __MMI_LANG_TR_CHINESE__)
	 ||(GET_APN(TR_CHINESE))
#endif

#if defined( __MMI_LANG_SPANISH__)
	 ||(GET_APN(SPANISH))
#endif

#if defined(__MMI_LANG_DANISH__)
	 ||(GET_APN(DANISH))
#endif

#if  defined(__MMI_LANG_POLISH__)
      	 ||(GET_APN(POLISH))
#endif
#if  defined(__MMI_LANG_FRENCH__)
        ||(GET_APN(FRENCH))
#endif

#if  defined(__MMI_LANG_GERMAN__)
      ||(GET_APN(GERMAN))
#endif

#if  defined(__MMI_LANG_ITALIAN__)
       ||(GET_APN(ITALIAN))
#endif

#if  defined(__MMI_LANG_THAI__)
       ||(GET_APN(THAI))
#endif

#if  defined(__MMI_LANG_RUSSIAN__)
      ||(GET_APN(RUSSIAN))
#endif

#if  defined(__MMI_LANG_BULGARIAN__)
       ||(GET_APN(BULGARIAN))
#endif

#if  defined(__MMI_LANG_UKRAINIAN__)
       ||(GET_APN(UKRAINIAN))
#endif

#if  defined(__MMI_LANG_PORTUGUESE__)
       ||(GET_APN(PORTUGUESE))
#endif

#if  defined(__MMI_LANG_TURKISH__)
       ||(GET_APN(TURKISH))
#endif

#if  defined(__MMI_LANG_VIETNAMESE__)
       ||(GET_APN(VIETNAMESE))
#endif

#if  defined(__MMI_LANG_INDONESIAN__)
       ||(GET_APN(INDONESIAN))
#endif

#if  defined(__MMI_LANG_CZECH__)
       ||(GET_APN(CZECH))
#endif

#if  defined(__MMI_LANG_MALAY__)
       ||(GET_APN(MALAY))
#endif

#if  defined(__MMI_LANG_FINNISH__)
      ||(GET_APN(FINNISH))
#endif

#if  defined(__MMI_LANG_HUNGARIAN__)
       ||(GET_APN(HUNGARIAN))
#endif

#if  defined(__MMI_LANG_SLOVAK__)
       ||(GET_APN(SLOVAK))
#endif

#if  defined(__MMI_LANG_DUTCH__)
       ||(GET_APN(DUTCH))
#endif

#if  defined(__MMI_LANG_NORWEGIAN__)
       ||(GET_APN(NORWEGIAN))
#endif

#if  defined(__MMI_LANG_SWEDISH__)
       ||(GET_APN(SWEDISH))
#endif

#if  defined(__MMI_LANG_CROATIAN__)
      ||(GET_APN(CROATIAN))
#endif

#if  defined(__MMI_LANG_ROMANIAN__)
      ||(GET_APN(ROMANIAN))
#endif

#if  defined(__MMI_LANG_SLOVENIAN__)
       ||(GET_APN(SLOVENIAN))
#endif

#if defined(__MMI_LANG_GREEK__)
       ||(GET_APN(GREEK))
#endif

#if  defined (__MMI_LANG_HEBREW__)
       ||(GET_APN(HEBREW))
#endif

#if defined (__MMI_LANG_ARABIC__)
       ||(GET_APN(ARABIC))
#endif

#if  defined (__MMI_LANG_PERSIAN__)
       ||(GET_APN(PERSIAN))
#endif

#if  defined (__MMI_LANG_URDU__)
       ||(GET_APN(URDU))
#endif

#if  defined (__MMI_LANG_HINDI__)
       ||(GET_APN(HINDI))
#endif

#if defined (__MMI_LANG_MARATHI__)
       ||(GET_APN(MARATHI))
#endif

#if  defined (__MMI_LANG_TAMIL__)
       ||(GET_APN(TAMIL))
#endif

#if  defined (__MMI_LANG_BENGALI__)
       ||(GET_APN(BENGALI))
#endif

#if defined (__MMI_LANG_PUNJABI__)
       ||(GET_APN(PUNJABI))
#endif

#if  defined (__MMI_LANG_TELUGU__)
	||(GET_APN(TELUGU))
#endif
	   )
	   	return pApn;
	else
		return NULL;
#else
	return NULL;
#endif
}
#endif

#endif

