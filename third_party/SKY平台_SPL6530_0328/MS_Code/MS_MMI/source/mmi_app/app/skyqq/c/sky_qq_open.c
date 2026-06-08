#ifdef __MMI_SKYQQ__

//#include "sky_qq_postting.h"
#include "sky_qq_open.h"
#include "mrporting.h"
#include "mmi_font.h"
#include "sfs.h"
#include "os_api.h"
#include "mmimrapp.h"
#include "mmicc_export.h"
#include "mmimrapp_internal.h"
#include "app_tcp_if.h"
#include "Sig_code.h"
#include "mmifm_export.h"
#include "mmisms_id.h"
#include "mmipub.h"
#include "mmi_image.h"
#include "mmipdp_export.h"

extern void* QQ2008MEMptr;

#define INPUT_TYPE_PHONE_NUMBER						IMM_INPUT_TYPE_NUMERIC
#define INPUT_TYPE_ALPHANUMERIC_PASSWORD			IMM_INPUT_TYPE_ALPHANUMERIC_PASSWORD
#define INPUT_TYPE_ALPHANUMERIC_SENTENCECASE	    IMM_INPUT_TYPE_SENTENCE



//QQ所用字体大小
GUI_FONT_T g_qq_font_type = SONG_FONT_20;

static uint8 s_qq2008_timer = 0;   //qq2008 timer

LOCAL MMIMRAPP_NETCONTEXT_T s_qq_netContext = {FALSE, NULL, 0, {0}, 0, NULL};
LOCAL MMIMRAPP_SOCKET_T s_qq_sockets[MRAPP_SOCKET_MAX_NUM] = {0};

/*add by zack@20110110 start*/
extern MRAPP_DUALSYS_MNG_T g_mr_dualsys_mng;
/*add by zack@20110110 end*/

int32 QQ2008_handlePsMsg(MMIPDP_CNF_INFO_T *msg_ptr);

#ifdef WIN32
void* QQ2008MEMptr=NULL;

void sky_qq_force_exit(void)
{
}

int GetSkyQQRunState(void)
{
}

void SetSkyQQRunState(int state)
{
}

int QQ2008GetQQStatus(void)
{
}

void message_getMsCount(void)
{
}

void SDKCallSPR(void)
{
}

void QQFlicker(void)
{
}

void QQ_CloseEditor(void)
{
}

int sky_qq_timer_handle(uint8 timer_id)
{
    return FALSE;
}

void sky_qq_timerTimeout(void)
{
}
#endif

//数字键定义开关，如果有数字键就返回1，否则返回0。

int32 IsHaveNumKey(void)
{
	return 1;
}

//是否有中间键，五方向键的定义开关，如果是五方向键就返回1，否则返回0；
int32 IsHaveMidleKey(void)
{
	return 1;
}

//是否需要显示wifi这种联网方式的菜单；返回1就是显示菜单，否则就是不显示菜单。
int32 IsWIFIModle(void)
{
#ifdef __MRAPP_WIFI_SUPPORT__
    return 1;
#else
	return 0;
#endif
}

//好友上线后是否播放上线声音
//返回1 代表好友上线时需要播放声音，返回0就是不播放
int32 GetOnlineSoundPlayFlag(void)
{
	return 1;
}

//是否有振动的提示开关，返回1表示需要屏蔽振动菜单,否则就不需要屏蔽(即支持振动) 
//这里要反过来定义，是因为之前的版本没有这个开关，返回的默认是0；
int32 IsShieldVibrator(void)
{
	return 0;
}

/*********************************************************************
函数名:int32 GetMallocMemSize(void)
描  述:返回需要申请内存的大小
参  数:无
返  回:返回需要申请内存的大小
*********************************************************************/
int32 GetMallocMemSize(void)
{
    return (int32)QQ2008_MEM_LEN;
}

/********************************************************************
函数名:int32 GetAutoConnectTimers(void)
描  述:返回重连次数的设置
参  数:无
返  回:返回重连次数的设置
********************************************************************/
int32 GetAutoConnectTimers(void)
{
    return (int32)QQ2008_AUTOLOGIN_TIMERS;
}

void QQ2008FontSizeSet(uint16 font)
{
	switch(font)
	{
	case MR_FONT_BIG:
		g_qq_font_type = SONG_FONT_20;
		break;
	case MR_FONT_MEDIUM:
		g_qq_font_type = SONG_FONT_20;
		break;
	case MR_FONT_SMALL:
		g_qq_font_type = SONG_FONT_16;
		break;
	default:
		g_qq_font_type = SONG_FONT_20;
		break;
	}	
}

uint16 QQ2008_get_string_height(void)
{
    return GUIFONT_GetHeight(g_qq_font_type);  
}

uint16 QQ2008_get_string_width(const uint16* text)
{
    uint16 text_len = 0;

    text_len = MMIAPICOM_Wstrlen(text);
    return GUI_GetStringWidth(g_qq_font_type, text, text_len);
}


//判断T卡是否存在。
static int8 TFalshDecide;

int8 GetPathFree(void)
{
#ifdef DUAL_TCARD_SUPPORT 
    if(!MMIAPISD_GetStatus(MMI_DEVICE_SDCARD) && !MMIAPISD_GetStatus(MMI_DEVICE_SDCARD_1) )
#else
	if(!MMIAPISD_GetStatus(MMI_DEVICE_SDCARD))
#endif
	{
		TFalshDecide = FALSE;
	}
    else
    {
    	TFalshDecide = TRUE;
    }
	sky_qq_log(("QQ2008 GetPathFree() TFalshDecide = %d", TFalshDecide));
    return TFalshDecide;
}

int32 GetPathFreeFlag(void)
{
	sky_qq_log(("QQ2008 GetPathFreeFlag() TFalshDecide = %d", TFalshDecide));
	return TFalshDecide;
}

#define QQ2008_ROOT_PATH   "mythroad_spr"
#define MMI_CARD_DRV       'E'
#define MMI_CARD2_DRV      'F'
#define MMI_PUBLIC_DRV     'E'

/****************************************************************************
函数名:void dsmConvertPath(U8 * path)
描  述:路径分割符转换，将vm的分割符"/"转成本地的分割符"\"
参  数:path :路径名 GB编码
返  回:无
****************************************************************************/
void dsmConvertPath(uint8 * path)
{
	int len = 0 ,i = 0;
	
	len = strlen((char *)path);
	
	for(i=0; i<len; i++)
	{
		if(path[i] == 0x2f)
		{
			path[i] = 0x5c;
		}
	}
}

void QQ2008CreateRootDir(void)
{
	int ret;
    uint16 attr = 0;
	char temp[130] = {0};
	uint16 fileBuf[65] ={0};

#ifdef PURE_MEMORY
	sprintf(temp,"%c:\\%s",MMI_CARD_DRV,QQ2008_ROOT_PATH);
#else
	if (GetPathFreeFlag())
	{   
#ifdef DUAL_TCARD_SUPPORT
        if(MMIAPISD_GetStatus(MMI_DEVICE_SDCARD))
        {   
            sprintf(temp,"%c:\\%s",MMI_CARD_DRV,QQ2008_ROOT_PATH);
        }
        else
        {
            sprintf(temp,"%c:\\%s",MMI_CARD2_DRV,QQ2008_ROOT_PATH);
        }
#else
		sprintf(temp,"%c:\\%s",MMI_CARD_DRV,QQ2008_ROOT_PATH);
#endif
	}
	else
	{   
		sprintf(temp,"%c:\\%s",MMI_PUBLIC_DRV,QQ2008_ROOT_PATH);
	}
#endif
    sky_qq_log(("QQ2008CreateRootDir path = %s", temp));
    dsmConvertPath((uint8 *)temp);
	MMIMRAPP_GB2UCS(fileBuf,(char*)temp,strlen(temp));

	ret = SFS_GetAttr((uint16 *)fileBuf, &attr);
    sky_qq_log(("QQ2008CreateRootDir ret = %d, attr = %d", ret, attr));
	if(ret < 0||(!(attr&SFS_ATTR_DIR)))
	{	
		ret = SFS_CreateDirectory((uint16 *)fileBuf);
        sky_qq_log(("QQ2008CreateRootDir ret = %d", ret));
	}
}

void QQ2008MINIShowIdle(int8 flag)
{
	QQFlicker(flag);
}

uint8 QQmin=0;
void MINQQ(void)
{
	if(GetSkyQQRunState() == QQ2008_RUN)
	{
		SetSkyQQRunState(QQ2008_BACK_RUN);
        QQ_CloseEditor();
		QQ2008MINIShowIdle(0);
		QQmin = 1;
	}
    else if(GetSkyQQRunState() == QQ2008_PAUSE)
	{
		SetSkyQQRunState(QQ2008_BACK_RUN);

		//下面这个函数必须添加，否则会造成内存的泄露，因为平台已经退出
		QQ_CloseEditor();
		QQ2008MINIShowIdle(0);
		QQmin = 1;
	}
}

void SetSkyQQBackRunState(void)
{
    sky_qq_log(("QQ2008 SetSkyQQBackRunState"));
    MMIMRAPP_SetBackRun(MRAPP_SKYQQ);
}

void SKYQQShowIdle(int status)
{
    QQ2008_STATE_E state = 0;
#ifndef WIN32
    if(GetSkyQQRunState() != QQ2008_BACK_RUN)
    {
        return;
    }
#endif
    state = QQ2008GetQQStatus();
    sky_qq_log(("SKYQQShowIdle state = %d, status = %d", state, status));

    switch(status)
    {
    case QQ2008_SHOW_ICON:
        MAIN_SetIdleQQState(state, GUIWIN_STBDATA_ICON_NORMAL);
        break;

    case QQ2008_BLINK_ICON:
        MAIN_SetIdleQQState(state, GUIWIN_STBDATA_ICON_ANIM);
        break;

    case QQ2008_HIDE_ICON:
        MAIN_SetIdleQQState(state, GUIWIN_STBDATA_ICON_NONE);
        break;
		
    default:
    	break;
    }
}

//本函数用来唤醒QQ
void ActiveQQ2008Screen(void)
{    
    SKYQQShowIdle(QQ2008_HIDE_ICON);
	MMIMRAPP_AppEntry(MRAPP_SKYQQ);
    
    QQmin = 0;
}

int32 QQ2008DrivingActiveIsInCallDecide(void)
{
	return MMIAPICC_IsInState(CC_CALL_CONNECTED_STATE);
}

void* QQ2008Mem_malloc(int32 size)
{
	void* ptr = NULL;
	sky_qq_log(("QQ2008Mem_malloce enter mem_left = %ld, size = %d", SCI_GetSystemSpaceMaxAvalidSpace(), size));
	ptr = (void* )SCI_ALLOCA(size); 
    sky_qq_log(("QQ2008Mem_malloc exit mem_left = %ld, size = %d", SCI_GetSystemSpaceMaxAvalidSpace(), size));
	return ptr;
}

void  QQ2008Mem_free(void * ptr)
{
    sky_qq_log(("QQ2008Mem_free enter mem_left = %ld", SCI_GetSystemSpaceMaxAvalidSpace()));
	if (ptr)
	{
		SCI_FREE(ptr);
		ptr = NULL;
	}
    sky_qq_log(("QQ2008Mem_free exit mem_left = %ld", SCI_GetSystemSpaceMaxAvalidSpace()));
}

void * QQ2008MemoryMalloc(void)
{
 	if (!QQ2008MEMptr)
    {  
		QQ2008MEMptr = QQ2008Mem_malloc(GetMallocMemSize());
    }
}

void QQ2008MemoryFree(void)
{
 	if (QQ2008MEMptr)
    {  
        QQ2008Mem_free((void*)QQ2008MEMptr);
        QQ2008MEMptr = NULL;
    }
}

int32 QQ2008PlaySoundEx(uint8 *audio_data,uint32 len,int32 loop)
{
    if(MMIAPIMP3_IsPlayerPlaying()||(FM_PLAYING == MMIAPIFM_GetStatus()))
    {
        return FALSE;
    }
    
    if (audio_data != NULL)
    {
        return mr_playSound(0, (void*)audio_data, len, loop);
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////////
//network
typedef void (*timerCbFunc)(void);

int8 s_qqSimSel = -1;
int32 s_qq2008_unet_handle = 0;//UNET_INVALID_HANDLE;

int32 QQ2008_getfreeSnode(void)
{
	int i;
	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		sky_qq_log(("QQ2008_getfreeSnode s_qq_sockets[%d].s : %d", i, s_qq_sockets[i].s));	
		if(s_qq_sockets[i].s == INVALID_SOCKET)
			return i;
	}
	
	return -1;
}

int32 QQ2008_findtheSnode(int32 s)
{
	int i;
	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		sky_qq_log(("QQ2008_findtheSnode s_qq_sockets[%d].s = %d, s = %d", i, s_qq_sockets[i].s, s));	
		if(s_qq_sockets[i].s == s)
			return i;
	}
	
	return -1;
}

void QQ2008_bcd2dec(uint8 *bcd, uint8* dec, uint16 convertLen)
{
	uint8 i, j;

	sky_qq_log(("QQ2008_bcd2dec() convertLen: %d", convertLen));
	
	if(bcd == NULL || dec == NULL)
		return;
	
	for(i = 0, j = 0; i < convertLen; i++)
	{
		dec[j++] = (bcd[i] & 0x0F) + '0';
		dec[j++] = ((bcd[i] >> 4) & 0x0F) + '0';
	}
}

void QQ2008_freeAllSnode(void)
{
	int i;
	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		s_qq_sockets[i].s = INVALID_SOCKET;
		s_qq_sockets[i].state = MRAPP_SOCKET_CLOSED;
	}
}

/*add by zack@20110110 start*/
MN_DUAL_SYS_E QQ2008_GetActiveSim(void)
{
	sky_qq_log(("QQ2008 QQ2008_GetActiveSim: %d", g_mr_dualsys_mng.simSel));
	return (MN_DUAL_SYS_E)g_mr_dualsys_mng.simSel;
}
/*add by zack@20110110 end*/

int32 QQ2008_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode)
{	
	int32 resValue = MR_FAILED;
	char *apn = (char*)(mode != NULL ? mode : "cmnet");
	uint8 *user_ptr = "";
	uint8 *passwd_ptr = "";
    MMIPDP_ACTIVE_INFO_T app_info = {0};

	sky_qq_log(("QQ2008_initNetwork() state: %d, apn: %s", s_qq_netContext.gprsState, apn));
	
	QQ2008_freeAllSnode();

#ifdef __MRAPP_WIFI_SUPPORT__
    if(MMIMRAPP_GetWifiMode())
    {
        s_qq_netContext.gprsState = MRAPP_GPRS_ACTIVED;
        return MR_SUCCESS;
    }
#endif
	if(cb == NULL)
		return resValue;
	
	s_qq_netContext.mrNetActived = TRUE;
	s_qq_netContext.deactWaitFunc = NULL;
	s_qq_netContext.gprsCb = (void*)cb;
	if(s_qq_netContext.gprsState == MRAPP_GPRS_ACTIVED)
	{
		resValue = MR_SUCCESS;
	}
	else if(s_qq_netContext.gprsState == MRAPP_GPRS_DEACTIVEING)
	{
		s_qq_netContext.subEvn = MRAPP_NETSUBEVN_WAITDAIL;
		s_qq_netContext.pdp_retry_times = 0;
	}
	else if(s_qq_netContext.gprsState == MRAPP_GPRS_DEACTIVED)
	{
		int32 ret;
		sky_qq_log(("QQ2008_initNetwork() cur sim: %d", QQ2008_GetActiveSim()));
        app_info.app_handler = MMI_MODULE_SKYQQ;
        app_info.apn_ptr = (char*)apn;
        app_info.user_name_ptr = (char*)user_ptr;
        app_info.psw_ptr = (char*)passwd_ptr;
        app_info.dual_sys = QQ2008_GetActiveSim();
        app_info.priority = 3;
        //app_info.ps_service_rat = MN_UNSPECIFIED;
        app_info.handle_msg_callback = QQ2008_handlePsMsg;
        MNGPRS_ReadStorageExtReqQosEx(QQ2008_GetActiveSim(),MN_GPRS_STORAGE_ALL,IM_E,&app_info.qos_info);

        ret = MMIAPIPDP_Active(&app_info);
        
		sky_qq_log(("QQ2008 apn:%s ret: %d", apn, ret));
		
		if(ret == TRUE)
		{
			s_qq_netContext.gprsState = MRAPP_GPRS_ACTIVING;
		}
		s_qq_netContext.pdp_retry_times = 0;
	}
	
	if(s_qq_netContext.gprsState == MRAPP_GPRS_ACTIVING
	   || (s_qq_netContext.gprsState == MRAPP_GPRS_DEACTIVEING && s_qq_netContext.subEvn == MRAPP_NETSUBEVN_WAITDAIL))
	{
		memcpy(s_qq_netContext.apn, apn, sizeof(s_qq_netContext.apn)-1);
		s_qq_netContext.apn[sizeof(s_qq_netContext.apn)-1] = '\0';
		MMIMRAPP_gprsProtectTimerStart(MMIMRAPP_GPRSPROTECT_TIME_OUT);
		resValue = MR_WAITING;
	}
	
	sky_qq_log(("QQ2008_initNetwork() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description : re-dail
//	Global resource dependence : none
//  Author: zzh
//	Note:
/*****************************************************************************/
PUBLIC void QQ2008_redail(void)
{
	int32 ret;
	uint8 *user_ptr = "";
	uint8 *passwd_ptr = "";
    MMIPDP_ACTIVE_INFO_T app_info = {0};
	
	sky_qq_log(("QQ2008 QQ2008_redail ret: %d, APN: %s", ret, s_qq_netContext.apn));

    app_info.app_handler = MMI_MODULE_SKYQQ;
    app_info.apn_ptr = (uint8*)s_qq_netContext.apn;
    app_info.user_name_ptr = (char*)user_ptr;
    app_info.psw_ptr = (char*)passwd_ptr;
    app_info.dual_sys = QQ2008_GetActiveSim();
    app_info.priority = 3;
    //app_info.ps_service_rat = MN_UNSPECIFIED;
    app_info.handle_msg_callback = QQ2008_handlePsMsg;
    MNGPRS_ReadStorageExtReqQosEx(QQ2008_GetActiveSim(),MN_GPRS_STORAGE_ALL,IM_E,&app_info.qos_info);

    ret = MMIAPIPDP_Active(&app_info);

    if(ret == TRUE)
	{
		s_qq_netContext.gprsState = MRAPP_GPRS_ACTIVING;
		MMIMRAPP_gprsProtectTimerStart(MMIMRAPP_GPRSPROTECT_TIME_OUT);
	}
	else
	{
		s_qq_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
		s_qq_netContext.mrNetActived = FALSE;//2007add
		if(s_qq_netContext.gprsCb != NULL)
		{
			MR_INIT_NETWORK_CB callBack = (MR_INIT_NETWORK_CB)s_qq_netContext.gprsCb;
			callBack(MR_FAILED);
			s_qq_netContext.gprsCb = NULL;
		}
	}
	
}

/****************************************************************************
函数名:int32 mr_getHostByName(const char *name, MR_GET_HOST_CB cb)
描     述:请求进行域名解析
参     数:name:
返     回:无
****************************************************************************/
int32 QQ2008_getHostByName(const char *name, MR_GET_HOST_CB cb)
{
	int32 hostAddr = 0;
	int32 resValue = MR_FAILED;
	struct sci_hostent * hostent = NULL;
	
#ifndef WIN32
	if(name != NULL && s_qq_netContext.gprsState == MRAPP_GPRS_ACTIVED)//2007add
	{
		sky_qq_log(("QQ2008_getHostByName() in name: %s", name));
		
		hostent = sci_gethostbyname((char*)name);
		if(hostent != NULL)
		{
			SCI_MEMCPY(&hostAddr, hostent->h_addr_list[0], 4);
			resValue = hostAddr;
		}
	}
#endif
	
	sky_qq_log(("QQ2008_getHostByName() resValue :%d", resValue));
	return resValue;
}

/****************************************************************************
函数名:int32 mr_socket(int32 type, int32 protocol)
描     述:创建一个socket
参     数:
返     回:创建成功的socket的句柄
****************************************************************************/
int32 QQ2008_socket(int32 type, int32 protocol)
{
	int32 resValue = MR_FAILED;
	int socket = 0;
	int32 e = 0;
	int32 snode;

	sky_qq_log(("QQ2008_socket() type: %d protocol: %d", type, protocol));
	
	snode = QQ2008_getfreeSnode();
	if(snode != -1)
	{
#ifndef WIN32
#ifdef __MRAPP_WIFI_SUPPORT__
    	if(MMIMRAPP_GetWifiMode())
    	{
        	socket = sci_sock_socket(AF_INET, SOCK_STREAM, 0, MMIAPIWIFI_GetNetId());
    	}
        else
        {
    		socket = sci_sock_socket(AF_INET, SOCK_STREAM, 0, s_qq_netContext.netid);
        }
#else
		socket = sci_sock_socket(AF_INET, SOCK_STREAM, 0, 0);
#endif
		sky_qq_log(("QQ2008_socket() socket:%d", socket));
		
		if(socket >= 0)
		{
			sci_sock_setsockopt(socket, SO_NBIO, NULL);
			resValue = socket;
			s_qq_sockets[snode].s = socket;
			s_qq_sockets[snode].state = MRAPP_SOCKET_OPENED;
		}
		else
		{ 
			e = sci_sock_errno(socket);
			sky_qq_log(("QQ2008_socket() errno: %d", e)); 
		}
#endif
	}
	sky_qq_log(("QQ2008_socket() resValue: %d", resValue));
	return resValue;
}

int32 QQ2008_SocConnect(int32 s, int32 ip, uint16 port)
{
#ifndef WIN32
	int32 ret = 0;
	struct sci_sockaddr sockAddr;
	
	sky_qq_log(("QQ2008_SocConnect(), s: %d, ip: 0x%x, port: %d", s, ip, port));
	sockAddr.family = AF_INET;
	sockAddr.port = htons(port);
	sockAddr.ip_addr = (unsigned long)htonl(ip);
	SCI_MEMSET((void*)sockAddr.sa_data, 0, 8*sizeof(char));
	ret = sci_sock_connect((long)s, &sockAddr, sizeof(struct sci_sockaddr));	
	
	return ret;
#else
	return 0;
#endif
}

int32 QQ2008_connect(int32 s, int32 ip, uint16 port, int32 type)
{
	int32 ret = 0;
	int32 error = 0;

#ifndef WIN32
	sky_qq_log(("QQ2008_connect() s: %d, ip: 0x%x, port: %d, type: %d, state: %d", s, ip, port, type, s_qq_netContext.gprsState));
	
	if(s != INVALID_SOCKET && s_qq_netContext.gprsState == MRAPP_GPRS_ACTIVED)//2007add
	{
		int32 sNode;
		
		sNode = QQ2008_findtheSnode(s);
		if(sNode != -1)
		{
		    SCI_Sleep(1000);
			ret = QQ2008_SocConnect(s, ip, port);
			sky_qq_log(("QQ2008_connect() ret:%d", ret));
			s_qq_sockets[sNode].ip = ip;
			s_qq_sockets[sNode].port = port;
			if(ret < 0)
			{
				error = sci_sock_errno(s);
				sky_qq_log(("QQ2008_connect() errno1:%d", error));
				if(EINPROGRESS == error)
				{
					s_qq_sockets[sNode].state = MRAPP_SOCKET_CONNECTING;
					return MR_SUCCESS;//allways show apps a SUCCESS result.
				}
			}
			else
			{
				s_qq_sockets[sNode].state = MRAPP_SOCKET_CONNECTED;
				return MR_SUCCESS;
			}
		}
	}
	error = sci_sock_errno((long)s);
#endif
	
	sky_qq_log(("QQ2008_connect() errno2: %d", error));
	return MR_FAILED;
}

int32 QQ2008_closeSocket(int32 s)
{
	int32 resValue = MR_FAILED;
	int ret;
	int32 sNode;
	sky_qq_log(("QQ2008_closeSocket() s: %d", s));

#ifndef WIN32
	if(s != INVALID_SOCKET)
	{
		sNode = QQ2008_findtheSnode(s);
		if(sNode != -1)
		{
			ret = sci_sock_socketclose((long)s);
			sky_qq_log(("QQ2008_closeSocket() ret:%d ", ret));
			if(ret < 0)
			{
				sky_qq_log(("QQ2008_closeSocket() error:%d", sci_sock_errno((long)s)));
				s_qq_sockets[sNode].state = MRAPP_SOCKET_ERROR;
			}
			else
			{
				s_qq_sockets[sNode].state = MRAPP_SOCKET_CLOSED;
				s_qq_sockets[sNode].s = INVALID_SOCKET;
				resValue = MR_SUCCESS;
			}
		}
	}
#endif
	
	sky_qq_log(("QQ2008_closeSocket() resValue: %d", resValue));
	return resValue;
}

int32 QQ2008_CloseAllSocket(void)
{
	int i;

	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		sky_qq_log(("QQ2008_CloseAllSocket s_qq_sockets[%d].s = %d, state = %d", i, s_qq_sockets[i].s, s_qq_sockets[i].state));	
		if(s_qq_sockets[i].s != INVALID_SOCKET)
		{
			QQ2008_closeSocket(s_qq_sockets[i].s);
		}
	}
	
	return 0;
}

int32 QQ2008_recv (int32 s, char *buf, int len)
{
	int32 resValue = MR_FAILED;
	int ret, e;
	int32 sNode;
	sky_qq_log(("QQ2008_recv() s: %d, len:%d", s, len));

#ifndef WIN32
	if(s != INVALID_SOCKET && buf != PNULL && len != 0)
	{
		sNode = QQ2008_findtheSnode(s);
		if(sNode != -1)
		{
			if(s_qq_sockets[sNode].state == MRAPP_SOCKET_CONNECTED)
			{
				ret = sci_sock_recv((long)s, buf, len, 0);
				sky_qq_log(("QQ2008_recv() ret:%d, len:%d", ret, len));
				if(ret > 0)
				{
					resValue = ret;
				}
				else if(ret == 0)//对端已关闭连接
				{
					resValue = MR_FAILED;
				}
				else
				{
					e = sci_sock_errno(s);
					sky_qq_log(("QQ2008_recv() errno:%d", e));
					
					if(e == ECONNRESET || e == EPIPE)//对端已关闭连接或服务器死机后重启
					{
						resValue = MR_FAILED;
					}
					else
					{
						resValue = 0;
					}
				}
			}
			else if(s_qq_sockets[sNode].state == MRAPP_SOCKET_CONNECTING)
			{
				ret = QQ2008_SocConnect(s, s_qq_sockets[sNode].ip, s_qq_sockets[sNode].port);
				sky_qq_log(("QQ2008_recv socconnect ret:%d", ret));
				if(ret < 0)
				{
					e = sci_sock_errno(s);
					sky_qq_log(("QQ2008_recv() socconnect errno:%d", e));
					if(EINPROGRESS == e)
					{
						resValue = 0;
					}
				}
				else
				{
					s_qq_sockets[sNode].state = MRAPP_SOCKET_CONNECTED;
					resValue = 0;
				}
			}
		}
	}
#endif
	
	sky_qq_log(("QQ2008_recv() resValue: %d", resValue));
	return resValue;
}

int32 QQ2008_send(int32 s, const char *buf, int len)
{
	int32 resValue = MR_FAILED;
	int32 e = 0;
	int ret = 0;
	int32 sNode;

	sky_qq_log(("QQ2008_send() sock: %d, len: %d", s, len));
	
#ifndef WIN32
	if(s_qq_netContext.gprsState == MRAPP_GPRS_ACTIVED && s != INVALID_SOCKET && buf != PNULL && len != 0)
	{
		sNode = QQ2008_findtheSnode(s);
		if(sNode != -1)
		{
			if(s_qq_sockets[sNode].state == MRAPP_SOCKET_CONNECTED)
			{
				ret = sci_sock_send((long)s, (char*)buf, len, 0);
				
				sky_qq_log(("QQ2008_send() ret: %d", ret));
				if(ret < 0)
				{
					e = sci_sock_errno((int32)s);
					sky_qq_log(("QQ2008_send() errno: %d", e)); 
					if(e == EWOULDBLOCK)
					{
						return 0;
					}
					else
					{
						resValue = MR_FAILED;
					}
				}
				else
				{
					resValue = ret;
				}
			}
			else if(s_qq_sockets[sNode].state == MRAPP_SOCKET_CONNECTING)
			{
				ret = QQ2008_SocConnect(s, s_qq_sockets[sNode].ip, s_qq_sockets[sNode].port);
				sky_qq_log(("QQ2008_send() socconnect ret: %d", ret));
				if(ret < 0)
				{
					int error;
					error = sci_sock_errno(s);
					sky_qq_log(("QQ2008_send() socconnect errno:%d", error));
					if(EINPROGRESS == error)
					{
						resValue = 0;
					}
				}
				else
				{
					s_qq_sockets[sNode].state = MRAPP_SOCKET_CONNECTED;
					resValue = 0;
				}
			}
		}
	}
#endif
	
	sky_qq_log(("QQ2008_send() resValue: %d", resValue));
	return resValue;
}

int32 QQ2008_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port)
{
	int32 resValue = MR_FAILED;

#ifndef WIN32
	int ret, e;
	struct sci_sockaddr to;

	sky_qq_log(("QQ2008_recvfrom()"));

	if(s != INVALID_SOCKET && buf != PNULL && ip != PNULL && port != NULL)
	{
		to.family = AF_INET;
		
		ret = sci_sock_recvfrom(s, (char*)buf, len, 0, &to);
		sky_qq_log(("QQ2008_recvfrom() ret:%d, len:%d", ret, len));
		if(ret > 0)
		{
			resValue = ret;
			*port = ntohs(to.port);
			*ip = ntohl(to.ip_addr);
		}
		else if(ret == 0)//对端已关闭连接
		{
			resValue = MR_FAILED;//comment. 
		}
		else
		{
			e = sci_sock_errno(s);
			sky_qq_log(("QQ2008_recvfrom() errno:%d", e));
			
			if(e == ECONNRESET || e == EPIPE)//对端已关闭连接或服务器死机后重启
			{
				resValue = MR_FAILED;
			}
			else
			{
				resValue = 0;
			}
		}
	}
#endif	
	return resValue;
}

int32 QQ2008_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port)
{
	int32 resValue = MR_FAILED;

#ifndef WIN32
	int ret, e;
	struct sci_sockaddr to;

	sky_qq_log(("QQ2008_sendto()"));
	
	if(s != INVALID_SOCKET && buf != PNULL && len != 0)
	{
		to.family = AF_INET;
		to.port = htons(port);
		to.ip_addr = (unsigned long)htonl(ip);
		SCI_MEMSET((void*)to.sa_data, 0, 8*sizeof(char));
		ret = sci_sock_sendto(s, (char*)buf, len, 0, &to);
		if(ret < 0)
		{
			e = sci_sock_errno((int32)s);
			sky_qq_log(("QQ2008_sendto() errno: %d", e));	
			if(e == EWOULDBLOCK)//comment. EWOULDBLOCK will ok ??
			{
				return 0;
			}
			else
			{
				resValue = MR_FAILED;
			}
		}
		else
		{
			resValue = ret;
		}
	}
#endif	
	return resValue;
}

int32 QQ2008_closeNetwork(void)
{
	int32 ret;
	ERR_MNGPRS_CODE_E err;

	QQ2008_CloseAllSocket();
		
	sky_qq_log(("QQ2008_closeNetwork() isActive: %d, state: %d", s_qq_netContext.mrNetActived, s_qq_netContext.gprsState));

#ifdef __MRAPP_WIFI_SUPPORT__
    if(MMIMRAPP_GetWifiMode())
    {
        s_qq_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
        return MR_SUCCESS;
    }
#endif
#ifndef WIN32
	if(s_qq_netContext.mrNetActived == TRUE)
	{
		MMIMRAPP_gprsProtectTimerStop();
		MMIMRAPP_gprsRetryTimerStop();
		err = MMIAPIPDP_Deactive(MMI_MODULE_SKYQQ);
		sky_qq_log(("QQ2008 MMIAPIPDP_Deactive err: %d", err));

		s_qq_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
		s_qq_netContext.mrNetActived = FALSE;

		s_qq_netContext.gprsCb = NULL;
	}
#endif

	return ret;
}

int32 QQ2008_checkConnect(int32 soc)
{
	int16 state = 0;

	return sci_sock_getsockstate(soc, &state);
}

int32 QQ2008_ProcessGprsActiveCnf(MMIPDP_RESULT_E result)
{
	int32 ret = MR_FAILED;

	sky_qq_log(("QQ2008 QQ2008_ProcessGprsActiveCnf result: %d", result));

	if(result == MMIPDP_RESULT_SUCC)
    {
		s_qq_netContext.gprsState = MRAPP_GPRS_ACTIVED;
		ret = MR_SUCCESS;
	}

	return ret;
}


int32 QQ2008_handlePsMsg(MMIPDP_CNF_INFO_T *msg_ptr)
{
	uint8 recode = MMI_RESULT_TRUE;
	int32 resValue = MR_FAILED;
	MMIMRAPP_NETINIT_RESULT_T opResult;
    MMIPDP_CNF_INFO_T *signal_ptr = (MMIPDP_CNF_INFO_T*)msg_ptr;

	sky_qq_log(("QQ2008. QQ2008_handlePsMsg() msg_id = %d state = %d", signal_ptr->msg_id, s_qq_netContext.mrNetActived));
	
	if(!signal_ptr)
    {
		sky_qq_log(("QQ2008. not skyqq event"));
		return MMI_RESULT_FALSE;
	}
	
	MMIMRAPP_gprsProtectTimerStop();

	switch( signal_ptr->msg_id )
	{
	case MMIPDP_ACTIVE_CNF:
#ifdef __MRAPP_WIFI_SUPPORT__
        s_qq_netContext.netid = signal_ptr->nsapi;
#endif
		resValue = QQ2008_ProcessGprsActiveCnf(signal_ptr->result);
    	if(resValue != MR_WAITING)
    	{
    		opResult.cb = s_qq_netContext.gprsCb;
    		opResult.result = resValue;
    		MMK_PostMsg(
#ifdef MR_BINDTO_VIRTUAL_WIN
    			VIRTUAL_WIN_ID,
#else
    			MMIMRAPP_BASE_WIN_ID,
#endif
    			QQ2008_MSG_NETINIT_RESULT, 
    			&opResult, sizeof(MMIMRAPP_NETINIT_RESULT_T));
    		
    		MMI_TriggerMMITask();
    	}
		break;
		
	case MMIPDP_DEACTIVE_CNF:
		if(s_qq_netContext.subEvn == MRAPP_NETSUBEVN_WAITDAIL)
		{
			s_qq_netContext.subEvn = MRAPP_NETSUBEVN_NONE;
			resValue = MR_WAITING;
			MMK_PostMsg(
#ifdef MR_BINDTO_VIRTUAL_WIN
				VIRTUAL_WIN_ID,
#else
				MMIMRAPP_BASE_WIN_ID,
#endif				
				MMRAPP_MSG_NETREDAIL_IND, 
				PNULL, 0);
		}
		else
		{	
    		if(s_qq_netContext.gprsState == MRAPP_GPRS_DEACTIVEING)
            {      
    			s_qq_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
    			s_qq_netContext.mrNetActived = FALSE;
            }
		}
		break;
    
	case MMIPDP_DEACTIVE_IND:
        s_qq_netContext.need_count = FALSE;
		s_qq_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
		s_qq_netContext.mrNetActived = FALSE;
		break;
		
	default:
		recode = MMI_RESULT_FALSE;
		break;
	}
	
	sky_qq_log(("QQ2008_processPsMsg() RETURN"));
	return recode;
}

void QQ2008_netInitResult(int32 result)
{
	MR_INIT_NETWORK_CB callBack;
	sky_qq_log(("QQ2008_netInitResult() result :%d", result));

	/*2007add, do the wait proccess*/
	if(s_qq_netContext.deactWaitFunc != NULL && result == MR_FAILED)
	{
		MMIMRAPP_WAIT2DOPDPACT_FUNC waitCb = (MMIMRAPP_WAIT2DOPDPACT_FUNC)s_qq_netContext.deactWaitFunc;
		s_qq_netContext.deactWaitFunc = NULL;
		waitCb(s_qq_netContext.data);
	}	

	MMIMRAPP_gprsRetryTimerStop();
	if(s_qq_netContext.gprsCb != NULL)
	{
		callBack = (MR_INIT_NETWORK_CB)s_qq_netContext.gprsCb;
		callBack(result);
		sky_qq_log(("QQ2008_netInitResult() callback = 0x%x", callBack));
	}
}

//timer
int32 SKYQQ_timerStart(uint16 t, timerCbFunc cb)
{
	if (s_qq2008_timer == 0)
	{
		s_qq2008_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, t, FALSE);
	}

	return MR_SUCCESS;
}

int32 SKYQQ_timerStop(void)
{
	MMK_StopTimer(s_qq2008_timer);
	s_qq2008_timer = 0;

	return MR_SUCCESS;
}

BOOLEAN QQ2008_Timer(DPARAM param)
{
	//sky_qq_log(("QQ2008_Timer() s_qq2008_timer = %d, param = %d", s_qq2008_timer, *(uint8*)param));
	
	if(s_qq2008_timer == *(uint8*)param)
	{
		SKYQQ_timerStop();
        sky_qq_timerTimeout();

		return TRUE;
	}
    else if(sky_qq_timer_handle(*(uint8*)param))
    {
        return TRUE;
    }

	return FALSE;
}

void SKYQQ_ReturnBack(void)
{
	if(!MMIMRAPP_mrappIsActived())
    {
		MMIMRAPP_AppEntry(MRAPP_SKYQQ);
    }
}

/*****************************************************************************/
//	   Description : to handle the message of prompt alert window
//	  Global resource dependence : 
//	Author:zzh
//	  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E	MMIMRAPP_HandleQueryWin2Msg(
												MMI_WIN_ID_T     win_id, 
												MMI_MESSAGE_ID_E msg_id, 
												DPARAM param
												)
{
	BOOLEAN recode = MMI_RESULT_TRUE;

	switch(msg_id)
	{
	case MSG_APP_WEB:
	case MSG_APP_OK:
		MMK_CloseWin(win_id);
        sky_qq_force_exit();
		MMIMRAPP_exitMrApp(FALSE);
		MAIN_SetIdleQQState(0, GUIWIN_STBDATA_ICON_NONE);
		break;
		
	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		MMK_CloseWin(win_id);
		break;
		
	case MSG_TIMER://pass timer
		break;
		
	default:
		recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
		break;
	}
	
	return recode;
}

/*****************************************************************************/
//Description : 查询SKYQQ是否在后台运行,如果在运行则提示用户是否要退出当前应用
//	Global resource dependence : 
//  Author: zzh
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_CheckBackRunApp()
{
    if (GetSkyQQRunState() == QQ2008_BACK_RUN)
	{
		MMI_STRING_T string;
		MMISET_LANGUAGE_TYPE_E language_type;
        sky_qq_log(("skyqq is run now!"));
		
		//提示:QQ正在运行，请先退出后再启动当前选择的应用。				
		MMIAPISET_GetLanguageType(&language_type);
		if(language_type == MMISET_LANGUAGE_SIMP_CHINESE)
		{
    		string.wstr_len = sizeof("\xF7\x8B\x48\x51\x00\x90\xFA\x51\x0E\x54\xF0\x53\x94\x5E\x28\x75\x00\x00")/2-1;
    		string.wstr_ptr = (uint8*)"\xF7\x8B\x48\x51\x00\x90\xFA\x51\x0E\x54\xF0\x53\x94\x5E\x28\x75\x00\x00"; /*lint !e64*/
		}
		else
		{
			string.wstr_len = sizeof("Pls press \"OK\" to exit QQ first.")-1;
			string.wstr_ptr = (uint8*)L"Pls press \"OK\" to exit QQ first.";
		}
		
		MMIPUB_OpenAlertWinByTextPtr(NULL,&string,NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_OKCANCEL,MMIMRAPP_HandleQueryWin2Msg);	

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static int32 msgFlag = 0;
void setRecvMsgFlag(int32 flag)
{
    msgFlag= flag;
}

int32 getRecvMsgFlag(void)
{
    return msgFlag;
}

#endif//__MMI_SKYQQ__

