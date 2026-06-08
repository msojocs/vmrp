/****************************************************************************
** File Name:      mmimrapp_internal.h                                      *
** Author:         qgp                                                      *
** Date:           05/15/2006                                               *
** Copyright:      2003 Spreatrum, Incoporated. All Rights Reserved.        *
** Description:    this file is use to describe the mr app module internal functions                *
*****************************************************************************
**                         Important Edit History                           *
** -------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                              *
** 03/2006        qgp		       Create
** 
****************************************************************************/
#ifndef  _MMIMRAPP_INTERNAL_H_    
#define  _MMIMRAPP_INTERNAL_H_  

 /**-------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmk_type.h"
#include "mmiaudio.h"
#include "Mmi_theme.h"
#include "mmimrapp.h"
#include "mmifmm_export.h"
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
 
#ifdef __cplusplus
   extern   "C"
   {
#endif

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/

/*trace 开关*/
#define MRAPPLOGOPEN
//#define MR_APP_LOG_FILE  //开启该宏LOG信息将保存在文件中



#ifdef MRAPPLOGOPEN
#define MMIMR_Trace(a)    mr_printf a
#define MMIMR_TraceM(a)   mr_printf a
#define MMIMR_TraceApp(a) SCI_TRACE_LOW a
#define MMIMR_TraceT(a)   //mr_printf a
#define MMIMR_TraceN(a)   mr_printf a
#define MMIMR_TraceB(a)   //mr_printf a
#define MMIMR_TraceF(a)   mr_printf a
#else//关闭TRACE
#define MMIMR_Trace(a)
#define MMIMR_TraceM(a)
#define MMIMR_TraceApp(a)
#define MMIMR_TraceT(a)
#define MMIMR_TraceN(a)
#define MMIMR_TraceB(a)
#define MMIMR_TraceF(a)
#endif

#define M_NEW_VER
#define MR_FS_ASYN //if support asyn ffs ,open it
#define SHARE_BLOCK_MEM
#undef _STANDARD_FS_//do not define
#define MR_USE_PLAT_LCD_BUFFER//use  plat lcd buffer directly
#ifdef WIN32
#undef MRAPP_SUPPORT_CLKADAPT
#else
//#define MRAPP_SUPPORT_CLKADAPT
#endif
#define MR_BINDTO_VIRTUAL_WIN//将timer等消息挂在virtual win上
//for differentiate the different interface of nile and rocky 
#ifdef PORT_BASE_NILE
/*for nile base plat*/
//FFS. adapt len to 1byte union
#define MMIFILE_DEVICE_SYS_NAME_LEN_AD		(MMIFILE_DEVICE_SYS_NAME_LEN)*2
#define MMIFILE_DEVICE_UDISK_LEN_AD			(MMIFILE_DEVICE_UDISK_LEN)*2
#define MMIFILE_DEVICE_SDCARD_LEN_AD		(MMIFILE_DEVICE_SDCARD_LEN)*2
#ifdef DOUBLE_TFCARD
#define MMIFILE_DEVICE_SDCARD_LEN_2_AD		(MMIFILE_DEVICE_SDCARD_LEN_2)*2
#endif

#define MMIFILE_FULL_PATH_MAX_LEN_AD		(MMIFILE_FULL_PATH_MAX_LEN)*2
#define MMIFILE_FILE_NAME_MAX_LEN_AD		(MMIFILE_FILE_NAME_MAX_LEN)*2
#define MMIFILE_DEVICE_NAME_MAX_LEN_AD		(MMIFILE_DEVICE_NAME_MAX_LEN)*2
#define MMIFILE_DIR_NAME_MAX_LEN_AD			(MMIFILE_DIR_NAME_MAX_LEN)*2
//GUI
#define GUILIST_STRING_MAX_NUM_AD		    GUILIST_STRING_MAX_NUM
#else
/*for rocky base plat*/
//FFS. adapt len to 1byte union
#define MMIFILE_DEVICE_SYS_NAME_LEN_AD		(MMIFILE_DEVICE_SYS_NAME_LEN)
#define MMIFILE_DEVICE_UDISK_LEN_AD			(MMIFILE_DEVICE_UDISK_LEN)
#define MMIFILE_DEVICE_SDCARD_LEN_AD		(MMIFILE_DEVICE_SDCARD_LEN)
#ifdef DOUBLE_TFCARD
#define MMIFILE_DEVICE_SDCARD_LEN_2_AD		(MMIFILE_DEVICE_SDCARD_LEN_2)
#endif

#define MMIFILE_FULL_PATH_MAX_LEN_AD		(MMIFILE_FULL_PATH_MAX_LEN)
#define MMIFILE_FILE_NAME_MAX_LEN_AD		(MMIFILE_FILE_NAME_MAX_LEN)
#define MMIFILE_DEVICE_NAME_MAX_LEN_AD		(MMIFILE_DEVICE_NAME_MAX_LEN)
#define MMIFILE_DIR_NAME_MAX_LEN_AD			(MMIFILE_DIR_NAME_MAX_LEN)
//GUI
#define GUILIST_STRING_MAX_NUM_AD		    LISTBOX_STRING_MAX_NUM

#endif

#if (SPR_VERSION >= 0x10A1140)
//UDISK represented by "D"
#define MMIFILE_DEVICE_UDISK                (uint16 *)MMIAPIFMM_GetDevicePath(MMI_DEVICE_UDISK)
#define MMIFILE_DEVICE_UDISK_LEN            MMIAPIFMM_GetDevicePathLen(MMI_DEVICE_UDISK)

//SD card represented by "E"
#define MMIFILE_DEVICE_SDCARD               (uint16 *)MMIAPIFMM_GetDevicePath(MMI_DEVICE_SDCARD)
#define MMIFILE_DEVICE_SDCARD_LEN           MMIAPIFMM_GetDevicePathLen(MMI_DEVICE_SDCARD)

//SD card2 represented by "F" 
#define MMIFILE_DEVICE_SDCARD2               (uint16 *)MMIAPIFMM_GetDevicePath(MMI_DEVICE_SDCARD_1)
#define MMIFILE_DEVICE_SDCARD2_LEN           MMIAPIFMM_GetDevicePathLen(MMI_DEVICE_SDCARD_1)
//hidden partion "C" on nand flash
#define MMIFILE_DEVICE_SYS_NAME             (uint16 *)MMIAPIFMM_GetDevicePath(MMI_DEVICE_SYSTEM)
#define MMIFILE_DEVICE_SYS_NAME_LEN         MMIAPIFMM_GetDevicePathLen(MMI_DEVICE_SYSTEM)

typedef enum
{
    FS_HS,
    FS_UDISK,
    FS_MMC,
    FS_INVALID 
} FILE_DEV_E_T;
#endif

/*UI depend*/
#define MMIMRAPP_DEFAULT_MAX_LEN             500 		//允许的最大输入值
#define MMIMRAPP_EDITPASSWORD_MAX_LEN       100 		//允许的最大密码输入值
#define MMIMRAPP_UCS2_NAME_MAX_LEN          (MMIFILE_FULL_PATH_MAX_LEN_AD + 2)//255				//UCS2存储的文件名的最大长度
#define MMIMRAPP_FULL_PATH_MAX_LEN          (MMIFILE_FULL_PATH_MAX_LEN_AD + 2)
#define MMIMRAPP_FILE_NAME_MAX_LEN          (MMIFILE_FILE_NAME_MAX_LEN_AD + 2)
#define MMIMRAPP_DIR_NAME_MAX_LEN           (MMIFILE_DIR_NAME_MAX_LEN_AD + 2)

#define MRAPP_APPLIST_TITLE               "游戏中心"
#define MMIMRAPP_DIR_FLAG					'/'	 //目录标志
#define MMIMRAPP_DIR_FLAGSTR				"/"	 //目录标志
#define MRAPP_DYN_WIN_ID_MAXNUM             25   //可以取得的最大win id数目
#define MRAPP_MENU_ITEMSTR_MAXLEN           20   //MENU可显示的最大ITEM string长度-BYTE, depend lower ui
#define MRAPP_MENU_ITEM_MAXNUM              40   //MENU可显示的最大ITEM 数目
#define MRAPP_MENU_TREE_MAXDEEP             20   //MENU层次
#define MRAPP_ENV_VALUE_INITED              0x5a //应用设置初始化标志

/*the app base win msgs*/
#define   MMRAPP_MSG_ASYN_FFS_RESULT	0xEE00//asyn ffs operation result msg
#define   MMRAPP_MSG_NETINIT_RESULT		0xEE01//bearer init msg
#define   MMRAPP_MSG_NETREDAIL_IND		0xEE02//bearer redail msg
#define   MMRAPP_MSG_SYS_EXIT			0xEE03//vm exit msg

#define   QQ2008_MSG_NETINIT_RESULT     0xEE04//skyqq

#define MR_LAYER_CLIP_STACK_COUNT        (16)
/*for socket*/
#define MMIMRAPP_GPRSPROTECT_TIME_OUT			60000		//gprs激活连接超时时间60s
#define MRAPP_TCP_CNT_TIMEOUT     15 //连接超时时间15秒
#define MRAPP_SOCKET_MAX_NUM      10 //2007add
#define MRAPP_PDP_RETRY_TIMES     3 //pdp重连次数
#define MRAPP_PDP_RETRY_TIMER_OUT 10000//5000 //pdp重连间隔

#ifndef INVALID_SOCKET
#define  INVALID_SOCKET -1    /* WINsock-ish synonym for SYS_SOCKETNULL */
#endif

#define     ENOBUFS        1
#define     ETIMEDOUT      2
#define     EISCONN        3
#define     EOPNOTSUPP     4
#define     ECONNABORTED   5
#define     EWOULDBLOCK    6
#define     ECONNREFUSED   7
#define     ECONNRESET     8
#define     ENOTCONN       9
#define     EALREADY       10
#define     EINVAL         11
#define     EMSGSIZE       12
#define     EPIPE          13//对端关闭连接
#define     EDESTADDRREQ   14
#define     ESHUTDOWN      15
#define     ENOPROTOOPT    16
#define     EHAVEOOB       17
#define     ENOMEM         18
#define     EADDRNOTAVAIL  19
#define     EADDRINUSE     20
#define     EAFNOSUPPORT   21
#define     EINPROGRESS    22
#define     ELOWER         23    /* lower layer (IP) error */
#define     ENOTSOCK	   24    /* Includes sockets which closed while blocked */
#define     EIEIO		   27 /* bad input/output on Old Macdonald's farm :-) */

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/
#include "mrporting.h"

//declare win type 
typedef enum
{
	MRAPP_WINTYPE_DIALOG = 0,
	MRAPP_WINTYPE_TEXT,
	MRAPP_WINTYPE_EDITOR,
	MRAPP_WINTYPE_MENU,
	MRAPP_WINTYPE_APP,

	MRAPP_WINTYPE_MAX
}MMIMRAPP_DYN_WIN_TYPE_E;

//declare id  type 
typedef enum
{
	MRAPP_ID_WIN,
	MRAPP_ID_CTRL,
	
	MRAPP_ID_INVALID
}MMIMRAPP_ID_TYPE_E;

//declare the operation type to IDs
typedef enum
{
	MRAPP_GET_ID,
	MRAPP_FREE_ID	
}MMIMRAPP_DYN_ID_OP_TYPE_E;

//declare the command type for the app wins
typedef enum
{
	MMIMRAPP_KEY_NORMAL,
	MMIMRAPP_MOUSE_DOWN,
	MMIMRAPP_MOUSE_UP,
	MMIMRAPP_MOUSE_MOVE,
	MMIMRAPP_MENU_SELECT,
	MMIMRAPP_MENU_RETURN,
	MMIMRAPP_DIALOG_SELECT,
	MMIMRAPP_DIALOG_RETURN,
	MMIMRAPP_KEYTYPE_MAX
}MMIMRAPP_KEYTYPE_E;

/*gprs state type*/
typedef enum
{
	MRAPP_GPRS_DEACTIVED = 0,	
	MRAPP_GPRS_ACTIVING,
	MRAPP_GPRS_ACTIVED,
	MRAPP_GPRS_DEACTIVEING
}MMIMRAPP_GPRS_STATE_E; 

/*image type*/
typedef enum
{
	MRP_IMAGE_TYPE_JPG_FILE,
	MRP_IMAGE_TYPE_GIF_FILE,
	MRP_IMAGE_TYPE_PNG_FILE,
	MRP_IMAGE_TYPE_BMP_FILE,
	MRP_IMAGE_TYPE_WBMP_FILE,
	MRP_IMAGE_TYPE_INVALID
}MRAPP_IMG_TYPE;
 
//declare the operation type to menu icon
typedef enum
{
	MRAPP_ICON_ON,
	MRAPP_ICON_OFF	
}MMIMRAPP_MENUICON_OP_TYPE_E;

//2007add
typedef enum
{
	MRAPP_NETSUBEVN_NONE,
	MRAPP_NETSUBEVN_WAITDAIL,
}MMIMRAPP_NETSUBEVN_E;

//2007add
typedef enum
{
	MRAPP_SOCKET_CLOSED,
	MRAPP_SOCKET_OPENED,
	MRAPP_SOCKET_CONNECTING,
	MRAPP_SOCKET_CONNECTED,
	MRAPP_SOCKET_ERROR
}MMIMRAPP_SOCKETSTATE_E;

//2007add
typedef struct
{
	int32 s;
	MMIMRAPP_SOCKETSTATE_E state;
	uint32 ip;
	uint16 port;
}MMIMRAPP_SOCKET_T;


typedef struct
{
	BOOLEAN mrNetActived;
	void *gprsCb;
	MMIMRAPP_GPRS_STATE_E gprsState;
	//2007add
	char apn[40];
	int32 subEvn;
	void *deactWaitFunc;
	uint32 data;

	int32 pdp_retry_times;
	//for gprs data flow
	uint32 gprs_send[MN_DUAL_SYS_MAX - MN_DUAL_SYS_1];
	uint32 gprs_recv[MN_DUAL_SYS_MAX - MN_DUAL_SYS_1];
	BOOLEAN need_count;
	int32 netid;
}MMIMRAPP_NETCONTEXT_T;

/*wuwenjie 2011-12-01 START*/
typedef struct
{
	char hostBuffer[128];
	int32 timerCount;
	MR_GET_HOST_CB cb;
	uint8 getHostTimer;
}MMIMRAPP_HOSTCONTEXT_T;
/*wuwenjie 2011-12-01 END*/

//declare id table struct 
typedef struct 
{
	uint8 *tablePtr;
	uint8 tableSize;
	uint32 baseId;	
}MMIMRAPP_DYN_ID_TABLE_T;

//declare dialog win private data struct 
typedef struct
{
//	MMI_STRING_T title;
	uint8 type;//MR_DIALOG_OK or MR_DIALOG_OK_CANCEL
}MRAPP_DIALOG_DATA;

//declare text win private data struct 
typedef struct
{
	MMI_STRING_T title;
	MMI_STRING_T textInfo;
	uint8 type;//MR_DIALOG_OK or MR_DIALOG_OK_CANCEL
}MRAPP_TEXT_DATA;

//declare editor win private data struct 
typedef struct
{
	MMI_STRING_T title;
	MMI_STRING_T textOut;
	uint16 maxLen;//max unicode input numbers
	uint8 type;//
}MRAPP_EDIT_DATA;

//declare the element struct of dynamic menu win
typedef struct _MRAPP_LIST_ITEM_T
{
	uint16 index;
	int16 key;
	
	struct _MRAPP_LIST_ITEM_T *nextItem;
}MRAPP_LIST_ITEM_T;

//declare dynamic menu win private data struct 
typedef struct
{
	uint8 type;
	uint8 maxItemNum;//item number
	int16 curAddItemIdx;
	MMI_STRING_T title;
	int16 curFocusItemIdx;
	int16 preFocusItemIdx;
	MRAPP_LIST_ITEM_T *listElem;
}MRAPP_MENU_DATA;

//no used now
typedef struct
{
	void *dummy;
}MRAPP_APP_DATA;

//declare user data's struct add to indicated win
typedef struct
{
	MMIMRAPP_DYN_WIN_TYPE_E dynWinType;
	MMI_WIN_ID_T winId;
	MMI_CTRL_ID_T ctrlId;//from we used, one ctrl element enough

	union
	{
		MRAPP_DIALOG_DATA dialog;
		MRAPP_TEXT_DATA text;
		MRAPP_EDIT_DATA edit;
		MRAPP_MENU_DATA menu;
		MRAPP_APP_DATA app;	
	}u;

}MMIMRAPP_ADD_PARAMETERS_T;

//declare dialog data's struct
typedef struct _DIALOG_PARAMETERS_T
{
	MMI_WIN_ID_T winId;
	MRAPP_DIALOG_DATA dialog;
	struct _DIALOG_PARAMETERS_T *next;
}MMIMRAPP_DIALOG_PARAMETERS_T;

typedef struct
{
	uint32	opSize;
 	void		*cb;      //回调函数
	uint32	cb_param;               //回调参数
}MMIMRAPP_ASYN_FS_PARAM_T;

typedef struct
{
 	void		*cb;      //回调函数
	uint32	cb_param;               //回调参数
	int32	result;
}MMIMRAPP_ASYN_FS_RESULT_T;

typedef struct
{
 	void		*cb;      //回调函数
	int32	result;
}MMIMRAPP_NETINIT_RESULT_T;

typedef struct
{
	uint32 audioHdle;
	uint32 curSetPlayPos;
	uint32 curSetPlayTime;
	int32 totalLen;
	int32 totalTime;
	int32 uiSampleRate;
	int8 soundVolume;
	MRAPP_AUDIO_STATUS_T curStatus;
	//MRAPP_AUDIO_OFFSET_T curTimePos;
	MRAPP_AUDIO_OFFSET_T curDataPos;
	
	MMISRVAUD_RING_FMT_E ring_type;
	MRAPP_SRC_TYPE src_type;
	uint8 *name;
	uint32 dataLen;
		
}MRAPP_AUDIO_MNG_T;

typedef struct
{
	MRAPP_AUDIO_STATUS_T curStatus;
	MRAPP_AUDIO_OFFSET_T dataPos;		
}MRAPP_PCM_MNG_T;

typedef struct
{
	uint8 msg[2];//信息提示
	uint8 voltage[2];//低压警告
	uint8 flip[2];//翻盖音
	uint8 call[2];//来电提示
	uint8 alarm[2];//闹钟提示
}MRAPP_ENV_CONFIG_T;

typedef struct
{
	int32 count;
}MRAPP_ENVINFO_T;

typedef struct
{
	T_MR_PHB_ENTRY info;
	int8 result;
	int8 haveBack;
    BOOLEAN needInform;
}MRAPP_PBINFO_T;

typedef struct
{
	uint8 devSel;
	int32 simSel;	
	uint8 dummy;
}MRAPP_DUALSYS_MNG_T;

typedef struct
{
	BOOLEAN setted;
	uint32 curCpuClk;	
	uint32 curCpuClkParam;
	uint32 orgCpuClk;
	uint32 orgCpuClkParam;
}MRAPP_CPUCLK_MNG_T;

typedef struct 
{
	uint8* data;
	uint32 dataLen;
	int8 type;
	int8 loop;
	uint8 Ckcount;
}MRAPP_MRPLAYSOUND_INFO_T;

typedef struct
{
    LCD_ANGLE_E lcd_angle;
    WINDOW_SUPPORT_ANGLE win_angle;
}mr_screen_angle_t;


/**--------------------------------------------------------------------------*
 **                         FUNCTINS                                         *
 **--------------------------------------------------------------------------*/

/*****************************************************************************/
// 	Description : the mr app entity entry
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
BOOLEAN MMIMRAPP_MrAppRun(void* param, uint32 appId);

/*****************************************************************************/
// 	Description :根据动态ID 创建一需求窗体
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
int32 MMIMRAPP_createADynWin(MMIMRAPP_ADD_PARAMETERS_T *addData, MMI_STRING_T *textInfo);

/*****************************************************************************/
// 	Description : stop the timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_timerStop(int32 t);

/*****************************************************************************/
// 	Description : start a timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_timerStart(uint16 t);

/*****************************************************************************/
// 	Description : start a vibrator
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_startShake(int32 t);

/*****************************************************************************/
// 	Description : start a timer for gprs activing timeout protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_gprsProtectTimerStart(uint16 t);

/*****************************************************************************/
// 	Description : stop the protect timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_gprsProtectTimerStop(void);

/*****************************************************************************/
// 	Description : disconnect the gprs launched by mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_CloseMrNet(void);

/*****************************************************************************/
// 	Description : report the rerult to mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_netInitResult(int32 result);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_SMSfilter(MN_SMS_ALPHABET_TYPE_E  alphabet_type, MN_SMS_USER_VALID_DATA_T *data, MN_CALLED_NUMBER_T *number);

/*****************************************************************************/
// 	Description : set root win state to opened or closed
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_setAllReady(BOOLEAN ready);

/*****************************************************************************/
// 	Description : pause mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
int32 MMIMRAPP_pauseApp(void);

/*****************************************************************************/
// 	Description : resume mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
int32 MMIMRAPP_resumeApp(void);

/*****************************************************************************/
// 	Description : set arm clk to changed
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
BOOLEAN MMIMRAPP_CLKSetted(BOOLEAN setted, uint32 curCpuClkParam, uint32 curCpuClk);

/*****************************************************************************/
// 	Description : process ps signal
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
MMI_RESULT_E MMIMRAPP_processPsMsg( PWND app_ptr, uint16 msg_id, DPARAM param);


/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
MMIMRAPP_DIALOG_PARAMETERS_T*  MMIMRAPP_GetPromptWinAddDataPtr(MMI_WIN_ID_T win_id);

/*****************************************************************************/
// 	Description :创建一dialog win
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
int32 MMIMRAPP_createPromptWin(MMIMRAPP_DIALOG_PARAMETERS_T *addData, MMI_STRING_T *textInfo);

/*****************************************************************************/
// 	Description : init local resource
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_stopShake(void);

/*****************************************************************************/
// 	Description :设置默认文件设备与目录
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_setDefFilePath(void);

/*****************************************************************************/
// 	Description :保存mrp
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_savePack(uint8 *p,uint32 l);

/*****************************************************************************/
// 	Description : get according icon of a item
//	Global resource dependence : none
//  Author: qgp
//	Note: item icon 数目依赖平台定义了的数目，当前为20
/*****************************************************************************/
MMI_IMAGE_ID_T MMIMRAPP_getItemIcon(MMIMRAPP_MENUICON_OP_TYPE_E opType, uint16 index);

/*****************************************************************************/
// 	Description : 初始化资源
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_initAllExtraSrc(void);

/*****************************************************************************/
// 	Description : 检查并释放所有使用中的资源
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_freeAllExtraSrc(void);

/*****************************************************************************/
// 	Description : 进入某些应用中可事先选择pause mp3播放。
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
void MMIMRAPP_pauseMp3(void);

/*****************************************************************************/
// 	Description : 应用过程中恢复MP3的函数。
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
void MMIMRAPP_resumeMp3(void);

/*****************************************************************************/
// 	Description :清空socket对列
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_freeAllSnode(void);

/*****************************************************************************/
// 	Description : free mr mmi res
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_freeMmiRes(void);

/*****************************************************************************/
// 	Description : exit vm ,because use mr_stop() in this function, so if trigger by mr app, must use Asyn
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_exitMrApp(BOOLEAN Asyn);

/*****************************************************************************/
// 	Description : re-dail
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_redail(void);

/*****************************************************************************/
// 	Description : 当前启动的应用
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
void MMIMRAPP_setCurApp(MRAPP_APPENTRY_E app);

/*****************************************************************************/
// 	Description : add for mr_playSound() protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_startPlaySoundProtect(void);

/*****************************************************************************/
// 	Description : add for mr_playSound() protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_StopPlaySoundProtect(void);

/*****************************************************************************/
// 	Description : 检查mr_playSound() 启动的playing是否被外部中断。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_playingCheck(void);

/*****************************************************************************/
// 	Description : start a timer for gprs activing timeout protect
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
uint8 MMIMRAPP_gprsRetryTimerStart(uint16 t);

/*****************************************************************************/
// 	Description : stop the protect timer
//	Global resource dependence : none
//  Author: qgp
//	Note:  
/*****************************************************************************/
void MMIMRAPP_gprsRetryTimerStop(void);

/*****************************************************************************/
// 	Description : close mr base win及其上启动的所有win.
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
BOOLEAN MMIMRAPP_closeAllWins(void);


/*****************************************************************************/
// 	Description : 提高cpu clk
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_SetArmClkHigh(void);

/*****************************************************************************/
// 	Description : 恢复clk到设置前状态
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_RestoreARMClk(void);

/*****************************************************************************/
// 	Description : 为了向前版本兼容
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC uint16 MMIMRAPP_GB2UCS(uint16 *ucs2_ptr, const uint8 *gb_ptr, uint16 len);

/*****************************************************************************/
//  Description : handle file browser win message
//  Global resource dependence :
//  Author: zack.zhang
//  Note:
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIMRAPP_HandleFileBrowserWinMsg(
													 MMI_WIN_ID_T          win_id,
													 MMI_MESSAGE_ID_E      msg_id,
													 DPARAM                param
													 );

PUBLIC void MMIMRAPP_setLcdAsynMode(int32 isSet);

//2009-7-7 add begin
PUBLIC uint8 MMIMRAPP_startGetApnAccountTimer(void);
PUBLIC void MMIMRAPP_getApnAccountTimerStop(void);
PUBLIC uint8 MMIMRAPP_startSetApnAccountTimer(void);
PUBLIC void MMIMRAPP_setApnAccountTimerStop(void);
//2009-7-7 add end

//2010-06-24 add begin
PUBLIC uint8 MMIMRAPP_StartGetSCNumberTimer(void);
PUBLIC void MMIMRAPP_GetSCNumberTimerStop(void);
//2010-06-24 add end

/* begin:added by Tommy.yan 20120120 */
PUBLIC uint8 MMIMRAPP_StartGetSMSCapacityTimer(void);
PUBLIC void MMIMRAPP_GetSMSCapacityTimerStop(void);
PUBLIC uint8 MMIMRAPP_StartGetSMSContentTimer(void);
PUBLIC void MMIMRAPP_GetSMSContentTimerStop(void);
PUBLIC uint8 MMIMRAPP_StartDeleteSMSTimer(void);
PUBLIC void MMIMRAPP_DeleteSMSTimerStop(void);
PUBLIC uint8 MMIMRAPP_StartSetSMSStateTimer(void);
PUBLIC void MMIMRAPP_SetSMSStateTimerStop(void);
/* end:added by Tommy.yan 20120120 */

PUBLIC uint8 MMIMRAPP_StartStatusIconTimer(void);
PUBLIC void MMIMRAPP_StopStatusIconTimer(void);
PUBLIC void MMIMRAPP_HandleStatusIconTimer(void);

/*2009-10-22 111,17787 add. for overseas*/
void MMIMRAPP_initApn(void);
void MMIMRAPP_saveApn(void);
/*2009-10-22 111,17787 add end*/

int32 mr_layer_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_layer_set_postion(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_layer_get_base_handle(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_layer_release(int32 param);
int32 mr_layer_set_active(int32 param);
int32 mr_layer_set_blt_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_layer_get_active_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_layer_set_alpha(BOOLEAN enable, int32 param);
int32 mr_layer_set_transparent(BOOLEAN enable, int32 param);

int32 mr_fs_get_filename(char *fullpath, const char *filename);


/**--------------------------------------------------------------------------*
 **                         EXTERNAL DECLARE                                 *
 **--------------------------------------------------------------------------*/
extern uint32  g_mr_curApp;
extern BOOLEAN g_mr_BLmallocingAtMrModule;
extern BOOLEAN s_mr_allReady;
extern BOOLEAN s_mr_VMReady;
extern int32 g_mr_phoneModState[MRAPP_MD_MAXNUM];
#ifdef MR_BINDTO_VIRTUAL_WIN
extern uint32 g_mr_vmBackRun;
#endif
extern BOOLEAN s_mr_inMrSelfWinCreating;
extern BOOLEAN s_mr_PBOfCancelMsg;
extern BOOLEAN s_mr_isWindowOfPB;
extern BOOLEAN s_mr_isReadBySMS;
extern uint8 s_mr_fbhup2idle;
extern mr_screen_angle_t g_screen_angle;



/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/


#ifdef   __cplusplus
   }
#endif

#endif/*_MMIMRAPP_INTERNAL_H_*/

