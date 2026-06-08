#include "mmi_app_mrapp_trc.h"
#ifdef MRAPP_SUPPORT
/****************************************************************************
** File Name:      mmimrapp.c                            
** Author:         qgp                                                      
** Date:           2006/05/15
** Copyright:          
** Description:    
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE               NAME             DESCRIPTION                             
** 04/01/2006	      qgp		          Create
** 04/24/2008         qgp      102060021 - 依照mocor平台gprs接口变化进行修改:
1/增加gprs流量记录，累加到系统流量上。
2/增加对gprs成功失败消息的细分处理。
** 05/13/2008    102060022 - 完善qq后台挂机处理(idle status bar显示)
** 04/18/2009    	-适配L
****************************************************************************/
#ifndef _MMIMRAPP_FUNC_C_
#define _MMIMRAPP_FUNC_C_  

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "window_parse.h"
#include "std_header.h"
#include "mmk_app.h"
#include "mmk_timer.h"
#include "guimsgbox.h"
#include "guicommon.h"
#include "guitext.h"
#include "guilistbox.h"
#include "guilabel.h"
#include "guimenu.h"
#include "guiedit.h"
#include "guilistbox.h"
#include "guilcd.h"
#include "mmidisplay_data.h"
#include "mmisrvaud_api.h"
#include "mmiset_export.h"
#include "mmipub.h"
#include "ffs.h"
#include "ffs_error.h"
#include "mn_type.h"
#include "Mn_api.h"
#include "mmisms_app.h"
#include "mmisms_export.h"
#include "mmiidle_export.h"
#include "mmidc_export.h"
#include "block_mem.h"
#include "sci_log.h"
#include "arm_reg.h"
#include "tb_hal.h"
#include "aud_gen.h"
#include "mmi_default.h"
#include "Mmiphone_export.h"
#include "Mmipb_export.h"
#include "mmimp3_export.h"
#include "mmimrapp_internal.h"
#include "mmimrapp_id.h"
#include "mmimrapp_export.h"
#ifdef MRAPP_SIMUGAME_OPEN
//#include "mmienvset.h"
#include "audio_extpcm.h"
#endif
#include "freq_cfg.h"
#include "mmicc_internal.h"
#include "mmicc_text.h"
#include "sig_code.h"
#include "Mmi_filemgr.h"
#include "mmi_nv.h"
#include "mmi_id.h"
#include "mmisms_send.h"
#include "mmisms_edit.h"
#include "stdarg.h"
#include "Mmisd_export.h"
#include "mmi_common.h"
#include "Mmienvset_internal.h"
#include "Guiimg.h"
#include "Guianim_internal.h"
#include "dal_lcd.h"
#include "mmiudisk_export.h"
#include "version.h"
#ifdef __MR_RECORD_SUPPORT__
#include "mmirecord_export.h"
#endif
#include "mmiset_id.h"
#ifndef WIN32
#include "iram_mgr.h"
#endif
#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif
#include "mmifmm_export.h"

#ifndef WIN32
typedef unsigned short		unshort;
typedef unsigned short		u_short;
#include "app_tcp_if.h"
#else
#include "sys\stat.h"
#include "stdio.h"
#endif
#include "mmiconnection_export.h"
#include "mmiset_id.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef __MR_RECORD_SUPPORT__
#include "mmirecord_internal.h"
#endif
#include "wav_adp.h"
#include "apm_express.h"

#include "mmimms_edit.h"
#include "mmimms_internal.h"
#include "pe_process.h"
#include "mmidc_setting.h"
#ifdef __MR_CFG_MULTITP_SUPPORT__
#include "mmk_type.h"
#include "mmk_tp.h"
#endif
#ifdef __MR_CFG_SHORTCUT_SUPPORT__
#include "guianim_internal.h"
#endif

#include "socket_api.h"
#include "mrporting.h"
#include "mmi_wallpaper_export.h"
#include "img_dec_interface.h"

#ifdef __MMI_SKYQQ__
#include "sky_qq_open.h"
#endif
/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/
#define MRP_PLAT_VER      (116)//平台版本
#define MRP_SUB_VER	   (9)   //子平台版本9为6530
#define MRP_PORT_VER	   (14)  //MR移植版本-最大9999
#define MRP_ASSERT(_EXP) //SCI_ASSERT(_EXP)

#define MRAPP_IRAM_ADDR 0x40000000
#define MRAPP_IRAM_LEN  0x000037FF

#define FMGR_FILTER_INIT(a)      (memset((&a), 0, sizeof((a))))
#define FMGR_FILTER_SET(a,b)     ((a) |= (b))
#define FMGR_FILTER_CLEAR(a,b)   ((a) &= (~(b)))

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/

typedef struct
{
	FILE_DEV_E_T device_type;
	uint8 *dir_ptr;
	uint16 dir_len;
	BOOLEAN mp3PreState;
	uint8 alain;
	
	uint8 dir_buf[MMIMRAPP_DIR_NAME_MAX_LEN];
}MRAPP_GLOBAL_T;

//2009-7-7 add begin
typedef struct
{
    uint8 authentication_type;
    uint8 user_name[32];
    uint8 password[32];
    uint8 dns[4];
    uint8 apn[100];
}T_DSM_DATA_ACCOUNT;

typedef struct
{
	char *NetId;
	char *apn;
	char *userName;
	char *passWord;
	unsigned char dns[4];
	unsigned char authentication_type;
}T_DSM_APN;
//2009-7-7 add end

/**--------------------------------------------------------------------------*
**                         STATIC DEFINITION                                *
**--------------------------------------------------------------------------*/

#ifndef SHARE_BLOCK_MEM
#define MR_APP_MEM_SIZE 1000*1024
LOCAL uint32 s_mr_run_mem[MR_APP_MEM_SIZE/sizeof(uint32)];//vm 内部内存空间,size视应用而定
#endif

LOCAL MMIMRAPP_NETCONTEXT_T s_mr_netContext = {FALSE, NULL, 0, {0}, 0, NULL};
LOCAL MMIMRAPP_SOCKET_T s_mr_sockets[MRAPP_SOCKET_MAX_NUM] = {0};

/*wuwenjie 2011-12-01 START*/
LOCAL MMIMRAPP_HOSTCONTEXT_T s_mr_hostContext ={0};
/*wuwenjie 2011-12-01 END*/

LOCAL uint8 full_path[MMIMRAPP_UCS2_NAME_MAX_LEN] = {0};
LOCAL uint16 ucs2_name_arr[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};

LOCAL uint32 tempTransBuf[MMIMRAPP_UCS2_NAME_MAX_LEN/4+1];
LOCAL uint32 s_mr_file_filter_type  = 0; //存放文件浏览时的过滤器类型

LOCAL uint16 s_mr_connect_timeout = MRAPP_TCP_CNT_TIMEOUT;//unit: s
LOCAL MRAPP_AUDIO_MNG_T s_mr_audio_mng = {INVALID_HANDLE, 0, IDLE, NULL, NULL, 0, 0, NULL, 0};
LOCAL uint8 s_mr_audio_filename[MMIMRAPP_UCS2_NAME_MAX_LEN*2] = {0};
LOCAL MRAPP_GIF_HEADER *s_mr_gifHeader = NULL;
MRAPP_PBINFO_T* g_mr_pbinfo = NULL;
uint32 g_mr_curApp = MRAPP_MAX;
LOCAL uint8 s_mr_mp3_resume_locked = FALSE;
LOCAL uint8 s_mr_need_resume_audio = FALSE;
#ifdef __MR_RECORD_SUPPORT__
LOCAL int32 s_mr_record_state = MMIRECORD_STATE_MAX;
#endif
//add by Tommy.yan 20120214
LOCAL MMISRV_HANDLE_T s_mr_record_handle = INVALID_HANDLE;
LOCAL uint32 s_mr_play_times = 1;

//added by Tommy.yan 20120406
LOCAL MMISRV_HANDLE_T s_mr_mix_audio_handle = INVALID_HANDLE;

LOCAL mr_audio_play_info_t g_audio_info = {0};

LOCAL mr_mma_ctrl_t g_mma_ctrl_info = {0};
#ifdef __MR_CFG_LOCALAPP_MANAGER__
LOCAL startSkyLapp s_skyLappEntry = NULL;
#endif
#ifdef MRAPPLOGOPEN
LOCAL char s_printfbuf[MAX_STR_SIZE-2];
#endif
LOCAL uint32 s_mrapp_chng_freq_req_handler = CHNG_FREQ_REQ_HANDLER_NULL;
LOCAL char s_mr_wallpaperBufNeedRestore = FALSE;
LOCAL uint8 s_mr_lcdasyn_state = FALSE;
LOCAL uint8 s_mr_new_sms_action = TRUE;
LOCAL uint8 s_mr_need_restore_backlight = FALSE;
#ifdef __MRAPP_WIFI_SUPPORT__
LOCAL uint8 s_mr_wifi_mode = FALSE;
#endif

//add by Tommy.yan 20111102
LOCAL MRAPP_BACKRUN_INFO_T     s_mr_backrun_info = {MRAPP_MAX, MRAPP_STATE_MAX};
LOCAL MRAPP_MRPLAYSOUND_INFO_T s_mr_mrPlaySound_info;//2008-1-2 add for mr_playSound() protect

/* begin:added by Tommy.yan 20120120 */
LOCAL T_DSM_MSG_INFO s_mr_msgInfo = {0};
LOCAL MMISMS_SMSDISPLAY_CONTENT_T s_mr_sms_content = {0};
LOCAL MMISMS_ORDER_ID_T s_mr_sms_order_id = {0};
LOCAL uint16 s_mr_sms_NVNum = 0;
LOCAL uint16 s_mr_sms_SIMNum = 0;
LOCAL ERR_MNSMS_CODE_E s_mr_sms_err_code = 0;
LOCAL MMISMS_STATE_T s_mr_save_order_id = {0};
/*end:added by Tommy.yan 20120120 */

MRAPP_GLOBAL_T g_mr_global = {0, (uint8*)MMIMRAPP_ROOT_DIR, sizeof(MMIMRAPP_ROOT_DIR)-3};
MRAPP_DUALSYS_MNG_T g_mr_dualsys_mng = {0xff};

mr_common_rsp_t  g_mr_common_rsp = {0};
mr_layer_info_t  g_mr_layer_info = {0};
mr_layer_active_info_t g_mr_active_layer = {0};

int32 g_mr_phoneModState[MRAPP_MD_MAXNUM] = {0xff};

//2009-7-7 add begin
static T_DSM_DATA_ACCOUNT gDsmDataAccount ={0};
static int32 gDsmUseNVdataAccount = FALSE;
uint8 g_mr_curUserApnAccuntId = 0;
//2009-7-7 add end

static int32 g_mr_font_type = MRAPP_FONT_SIZE_MID;
static uint8 g_mr_build_time[24] = {0};

//2010-10-14 add begin
#ifndef WIN32
LOCAL ILOCK_HANDLE s_mr_iramlock_handle = IRAM_INVALID_HANDLE;

// iram area which is used by mrapp
const IRAM_AREA s_mr_iram_area[] = 
	{
		{MRAPP_IRAM_ADDR, MRAPP_IRAM_LEN, (void *)SCI_NULL, IRAM_ATTRIBUTE_NORMAL}
	};
#endif
//2010-10-14 add end

#ifdef MRAPP_SIMUGAME_OPEN
//#define MRAPP_NES_USE_IRAM //6800H不使用IRAM
#ifdef MRAPP_NES_USE_IRAM
#ifdef PLATFORM_SC6600L//L  12K IRAM
#define NES_IRAM_CODE_START	0x40000000
#define NES_IRAM_CODE_LEN	0x3400
#else// 42.75k iram
#define NES_IRAM_CODE_START	0x40004D00
#define NES_IRAM_CODE_LEN	0xAB00
#endif
LOCAL ILOCK_HANDLE s_mrapp_ilock_handle = IRAM_INVALID_HANDLE;
const IRAM_AREA s_mrapp_iram_area[] = 
{
	{NES_IRAM_CODE_START, 	NES_IRAM_CODE_LEN, 	(void *)SCI_NULL, 	IRAM_ATTRIBUTE_NORMAL}
};
#endif


/*游戏模拟器出厂默认按键设置，可以修改此处来自定义此默认设置*/
#define NES_DEFAULT_KEYSET {15, 16, 17, 18, 10, 11, 1, 2, 4, 5}//the index value according to mrKeyMap[]

#define ENVLOCK_NONE 0X00
#define ENVLOCK_ENV  0X01
#define ENVLOCK_IRAM 0X02

LOCAL uint8 s_mr_envLock_state = ENVLOCK_NONE;//2008-6-11 add, fix abnormal exit
MRAPP_ENV_CONFIG_T s_mr_env_preVal;
LOCAL MRAPP_PCM_MNG_T s_mr_pcm_mng = {IDLE, {0}};

LOCAL const uint8 mrKeyMap[] = 
{/*增加修改注意: 请顺序放到最后面去，不要插队*/
	SCI_VK_0,
	SCI_VK_1,
	SCI_VK_2,
	SCI_VK_3,
	SCI_VK_4,
	SCI_VK_5,
	SCI_VK_6,
	SCI_VK_7,
	SCI_VK_8,
	SCI_VK_9,
	SCI_VK_STAR,//10
	SCI_VK_POUND,
	SCI_VK_CALL,
	SCI_VK_POWER,
	SCI_VK_CANCEL,
	SCI_VK_UP,//15
	SCI_VK_DOWN,
	SCI_VK_LEFT,
	SCI_VK_RIGHT,
	SCI_VK_MENU_SELECT,
	SCI_VK_MENU_CANCEL,//20
	SCI_VK_SIDE_UP,
	SCI_VK_SIDE_DOWN,
	SCI_VK_OK,
	SCI_VK_WEB,
	SCI_VK_CAMERA,//25
	SCI_VK_AT,
};
#endif


/**--------------------------------------------------------------------------*
**                         EXTERNAL DECLARE                                 *
**--------------------------------------------------------------------------*/
extern uint8 g_mr_filebrowser_path[256];
extern mr_screen_angle_t g_screen_angle;
extern void MMIMRAPP_SelectPBContactCallback(MMIPB_HANDLE_T handle);
extern ERR_LCD_E LCD_SetBrushMode (LCD_ID_E lcd_id, LCD_BRUSH_MODE_E mode);
//2009-7-7 add begin
#ifdef __MRAPP_OVERSEA__
extern T_DSM_APN *mr_getAPN(unsigned char * netId);
#endif
//2009-7-7 add end

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
// 	Description :获取对应设备的名字
//	Global resource dependence : 
//  Author:	qgp
//	Note:
/*****************************************************************************/
LOCAL const uint8* MMIMRAPP_getCurDevName(uint16 *length);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: qgp
//	Note:
///*****************************************************************************/
LOCAL uint32 MMIMRAPP_getUcstrLen(uint8 *wstr);

/*****************************************************************************/
// 	Description :将BCD码转换成十进制的字符串
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_bcd2dec(uint8 *bcd, uint8* dec, uint16 convertLen);

/*****************************************************************************/
// 	Description :audio 命令接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_AudioCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

#ifdef MRAPP_SIMUGAME_OPEN
/*****************************************************************************/
// 	Description :pcm 命令接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_PcmCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
#endif

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_mutiCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

/* begin:added by Tommy.yan 20120120 */

/*****************************************************************************/
// 	Description :  信息发送结果
//	Global resource dependence : 
//  Author: qgp
//   note:
/*****************************************************************************/
LOCAL BOOLEAN MMIMRAPP_SMSCnf(BOOLEAN is_succss, DPARAM parm);

/*****************************************************************************/
// 	Description :获取短信模块状态
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_status(int32 param);

/*****************************************************************************/
// 	Description :获取短信信箱中短信的条数
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_num(int32 param);

/*****************************************************************************/
// 	Description :获取手机和SIM卡中短信的存储容量
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_capacity(int32 param);

/*****************************************************************************/
// 	Description :获取短信信息(时间戳、号码、状态)
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_info(uint8* input, int32 input_len, uint8** output, int32* output_len);

/*****************************************************************************/
// 	Description :获取短信内容
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_content(uint8* input, int32 input_len, uint8** output, int32* output_len);

/*****************************************************************************/
// 	Description :删除一条短信
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_delete(uint8* input, int32 input_len, uint8** output, int32* output_len);

#ifdef __MR_SMS_SET_STATE_SUPPORT__
/*****************************************************************************/
// 	Description :修改发件箱/收件箱短信状态
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_set_state(uint8* input, int32 input_len, uint8** output, int32* output_len);
#endif

/*****************************************************************************/
// 	Description :保存一条(已读/未读)短信到收件箱中
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_save_sms(uint8* input, int32 input_len, uint8** output, int32* output_len);

/*****************************************************************************/
// 	Description :调用本地UI回复短信
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_replay_by_UI(uint8* input, int32 input_len, uint8** output, int32* output_len);

/*****************************************************************************/
// 	Description :调用本地UI发送短信
//	Global resource dependence : none
//  Author: Zack.Zhang
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_send_sms_from_ui(uint8* input, int32 input_len, uint8** output, int32* output_len);
/* end:added by Tommy.yan 20120120 */

/*****************************************************************************/
// 	Description :image命令接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_imageCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

/*****************************************************************************/
// 	Description :U获取jpeg宽高
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL uint32 MMIMRAPP_get_jpeg_width_height(uint8 * pbuf, uint32 size);

/*****************************************************************************/
// 	Description :释放内存
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
//LOCAL void MMIMRAPP_AudioPlayCb(MMIAUDIO_RESULT result, DPARAM param);

/*****************************************************************************/
// 	Description :获取图片类型
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL MRAPP_IMG_TYPE MIMRAPP_get_image_type_from_buf(unsigned char *pbuf, int32 len);

/*****************************************************************************/
// 	Description :获取图片信息
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_get_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

/*****************************************************************************/
// 	Description :图片解码
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_decode(uint8 * input, int32 input_len, uint8 * * output, int32 * output_len, MR_PLAT_EX_CB * cb);

/*****************************************************************************/
// 	Description :图片绘制
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_draw(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

/*****************************************************************************/
// 	Description :释放
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_gifFree(MRAPP_GIF_HEADER ** header);

/*****************************************************************************/
// 	Description : 将"\" 转换成"/"
//	Global resource dependence : 
//  Author:	qgp
//	Note:
/*****************************************************************************/
static void MMIMRAPP_ConvertPath2Loc(char *path, uint16 len);

/*****************************************************************************/
// 	Description : 将'/' 转换成'\'
//	Global resource dependence : 
//  Author:	qgp
//	Note:
/*****************************************************************************/
static void MMIMRAPP_ConvertPath2Pc(char *str, uint16 len);

/*****************************************************************************/
// 	Description :创建一个LIST
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_listCreate(const char * title, int16 num, uint8 type);

/*****************************************************************************/
// 	Description :设置item
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_menuSetItem(int32 menu, const char *text, int32 index, int32 isSelected);

/*****************************************************************************/
// 	Description :mr_playSound 回调
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MRRingCallBack(MMISRVAUD_REPORT_RESULT_E result, DPARAM param);

/*****************************************************************************/
// 	Description :获取一个空闲的SOCKET node
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_getAfreeSnode(void);

/*****************************************************************************/
// 	Description :通过socket id 获取对应的对列index
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_findTheSnode(int32 s);

/*****************************************************************************/
// 	Description :建立连接
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_Sconnect(int32 s, int32 ip, uint16 port);

/*****************************************************************************/
//	Description : combine full pathname from device, file type, filename
//	Global resource dependence : 
//	Author:liqing.peng
//	Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIFILE_CombineFullPath_fix(
										  const uint8	   *device_ptr, 	   //in
										  uint16		   device_len,		   //in
										  const uint8	   *dir_ptr,		   //in
										  uint16		   dir_len, 		   //in
										  const uint8	   *file_name_ptr,	   //in, can be NULL
										  uint16		   file_name_len,	   //in
										  uint8 		   *full_path_ptr,	   //out
										  uint16		   *full_path_len_ptr  //out
										  );

/*****************************************************************************/
// 	Description : 获取VM中当前使用的sim卡的NetworkID
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_GetNetworkID(uint8**output, int32 *output_len);

/* begin:added by Tommy.yan 20120214 */
/***************************************************************************/
//  Description : get current record handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_GetRecordHandle(void);

/*****************************************************************************/
// 	Description :set record handle
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_SetRecordHandle(MMISRV_HANDLE_T handle);

/***************************************************************************/
//  Description : get current record handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_GetAudioHandle(void);

/*****************************************************************************/
// 	Description :set record handle
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_SetMixAudioHandle(MMISRV_HANDLE_T handle);

/***************************************************************************/
//  Description : get current record handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_GetMixAudioHandle(void);

/*****************************************************************************/
// 	Description :set record handle
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_SetAudioHandle(MMISRV_HANDLE_T handle);

/***************************************************************************/
//  Description : create audio handle by buf
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_CreateAudioFileHandle(
													MMISRVAUD_RING_FMT_E ring_type,
													const wchar *name_ptr,
													MMISRVMGR_NOTIFY_FUNC call_back
													);

/***************************************************************************/
//  Description : create audio handle by file
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_CreateAudioBufHandle(
													MMISRVAUD_RING_FMT_E ring_type,
                     								const uint8* pucData, 
                     								uint32 uiDataLength,
                     								MMISRVMGR_NOTIFY_FUNC call_back
                     								);
/* end:added by Tommy.yan 20120214 */

/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/

/*****************************************************************************/
// 	Description : 	将bmp指向的Bitmap图片绘制于屏幕中。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void mr_drawBitmap(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h)
{
    GUI_RECT_T     rect = {0};
    rect.left = x;
    rect.top  = y;
    rect.right = x + w - 1;
    rect.bottom= y + h - 1;

    //MMIMR_Trace(("mrapp. mr_drawBitmap() x: %d, y: %d, w: %d, h: %d", x, y, w, h));

    if(rect.right <= rect.left || rect.bottom <= rect.top)
    {
        return;
    }
   
    GUILCD_InvalidateRect(GUI_MAIN_LCD_ID, rect, GUIREF_GetUpdateBlockSet(GUI_BLOCK_MAIN)); /*lint !e718 !e18*/ 
}

/*****************************************************************************/
// 	Description : 	将字符ch以矢量的方式绘制在指定位置
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
void mr_platDrawCharReal(uint16 ch, int32 x, int32 y, uint16 color)
{
    GUI_LCD_DEV_INFO *lcd_dev_ptr = MMITHEME_GetDefaultLcdDev();
    UILAYER_INFO_T   layer_info = {0};
    MMI_STRING_T     text_str  = {0};
    GUI_COLOR_T      *lcdbuf_ptr = PNULL;
    GUISTR_BUF_T     dest_buf  = {0};
    GUI_RECT_T       dest_rect = {0};
    GUISTR_STYLE_T   str_style = {0};
    GUISTR_STATE_T   str_state = 0;
    uint8            width  = 0;
    uint8            height = 0;
#ifndef UI_MULTILAYER_SUPPORT
    LCD_INFO_T    lcd_info = {0};
#endif

    //MMIMR_Trace(("mrapp mr_platDrawCharReal ch:0x%04x x:%d y:%d color:%d font:%d", ch, x, y, color, g_mr_font_type));

	width  = GUI_GetFontWidth(g_mr_font_type, ch);
	height = GUI_GetFontHeight(g_mr_font_type, ch);	

#ifndef UI_MULTILAYER_SUPPORT
    GUILCD_GetInfo(GUI_MAIN_LCD_ID, &lcd_info);
    if(x < 0 || y < 0 || x+width > lcd_info.lcd_width || y+height > lcd_info.lcd_height)
    {
        return;
    }
#else
    UILAYER_GetLayerInfo(lcd_dev_ptr, &layer_info);
    
    if(x < 0 || y < 0 || x+width > layer_info.layer_width || y+height > layer_info.layer_height)
    {
        return;
    }
#endif

    text_str.wstr_ptr = &ch;
    text_str.wstr_len = 1;

#ifndef UI_MULTILAYER_SUPPORT
    dest_buf.buffer = lcd_info.lcdbuff_ptr;
    dest_buf.width  = lcd_info.lcd_width;
    dest_buf.height = lcd_info.lcd_height;
    dest_buf.type   = GUISTR_BUF_RGB565;
    dest_buf.mem_width = lcd_info.lcd_width;

    dest_rect.left = x;
    dest_rect.top  = y;
    dest_rect.right = lcd_info.lcd_width;
    dest_rect.bottom= lcd_info.lcd_height;
#else
#if (SPR_VERSION >= 0x10A1140)
	dest_buf.buffer = (GUI_COLOR_T*)UILAYER_GetLayerBufferPtr(lcd_dev_ptr);
#else
    dest_buf.buffer = (void*)layer_info.layer_buf_ptr;
#endif
    dest_buf.width  = layer_info.layer_width;
    dest_buf.height = layer_info.layer_height;
    dest_buf.type   = layer_info.data_type;
    dest_buf.mem_width = layer_info.layer_width;

    dest_rect.left = x;
    dest_rect.top  = y;
    dest_rect.right = layer_info.layer_width;
    dest_rect.bottom= layer_info.layer_height;
#endif
    str_style.font = g_mr_font_type;
    str_style.font_color = color;
   
    GUISTR_DrawTextInRect(&dest_buf, &dest_rect, &dest_rect, &text_str, &str_style, str_state, 0);
}

/*****************************************************************************/
// 	Description : 	获取unicode码ch指向的字体的点阵信息，并告之该字
//	体的宽和高，获取到的点阵信息每一个bit表示字体的一个象素，
//	字体每行的象素必须按字节对其，也就是说如果一个字体宽为12，
//	则需要用两个字节来表示该信息，第二个字节的后四个bit为0，
//	从第三个字节开始才表示下一行的点阵数据。
//	Global resource dependence : none
//  Author: qgp
//	Note:6800H平台统一为调用移植层接口显示文本，不再返回点阵信息
/*****************************************************************************/
const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height)
{
	GUI_FONT_T fontType;
	uint8 *ret = NULL;
	uint8 textPtr[3];
    GUISTR_STYLE_T   str_style = {0};
    GUISTR_STATE_T   str_state = 0;
    MMI_STRING_T     text_str  = {0};
    GUISTR_INFO_T    str_info  = {0};
	
	if(width != NULL && height != NULL)
	{
		if(ch < 128)
		{
			textPtr[0] = 0x00;
			textPtr[1] = (uint8)ch;
		}
		else
		{
#ifndef WIN32
			textPtr[0] = *((char*)&ch);
			textPtr[1] = *(((char*)&ch)+1);
#else
			textPtr[0] = *(((char*)&ch)+1);
			textPtr[1] = *((char*)&ch);
#endif
		}
		textPtr[2] = 0x00;
		
		switch(fontSize)
		{
		default:
		case MR_FONT_BIG:
			fontType = MRAPP_FONT_SIZE_BIG;
			break;
		case MR_FONT_MEDIUM:
			fontType = MRAPP_FONT_SIZE_MID;
			break;
		case MR_FONT_SMALL:
			fontType = MRAPP_FONT_SIZE_SMALL;
			break;
		}
		g_mr_font_type = fontType;
        str_style.font = fontType;
        text_str.wstr_ptr = &ch;
        text_str.wstr_len = 1;
        GUISTR_GetStringInfo(&str_style, &text_str, str_state, &str_info);
        *width = str_info.width;
        *height= str_info.height;
    }

    //MMIMR_Trace(("ch: 0x%04x, w: %d, h: %d, fontType: %d", ch, *width, *height, fontType));      

    return (char*)(-1);
}

/*****************************************************************************/
// 	Description : 	获取屏幕宽、高、象素深度等信息。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_getScreenInfo(mr_screeninfo * screeninfo)
{
	MMIMR_Trace(("mrapp. mr_getScreenInfo() in: 0x%x", screeninfo));
	
	if(screeninfo != NULL)
	{
		screeninfo->width = MR_SCREEN_WIDTH;
		screeninfo->height = MR_SCREEN_HEIGHT;
		screeninfo->bit = 16;
        MMIMR_Trace(("mrapp mr_getScreenInfo  width: %d, height: %d, bit: %d", screeninfo->width, screeninfo->height, screeninfo->bit));
		return MR_SUCCESS;
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	启动手机震动。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_startShake(int32 ms)
{
	MMIMR_Trace(("mrapp. mr_startShake(), ms: %d", ms));
	
	if(ms != 0 && MMIMRAPP_startShake(ms) != 0)
	{
		return MR_SUCCESS;
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	结束手机震动
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_stopShake(void)
{
	MMIMRAPP_stopShake();
	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description : 	播放音频。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_playSound(int type, const void* data, uint32 dataLen, int32 loop)
{
	uint8				audio_vol = 0;
	MMISRVAUD_RING_FMT_E   audio_type = MMISRVAUD_RING_FMT_MAX;
	uint32 play_times = 1;
	MMIMR_Trace(("mrapp. mr_playSound() type: %d, datalen: %d, loop: %d", type, dataLen, loop));
	
	if(data == NULL || dataLen == 0)
		return MR_FAILED;
		
	switch(type)
	{
	case MR_SOUND_MIDI:
		audio_type = MMISRVAUD_RING_FMT_MIDI;
		break;

	case MR_SOUND_WAV:
		audio_type = MMISRVAUD_RING_FMT_WAVE;
		break;

	case MR_SOUND_MP3:
		audio_type = MMISRVAUD_RING_FMT_MP3;
		break;
	case MR_SOUND_AMR:
		audio_type = MMISRVAUD_RING_FMT_AMR;
		break;
	case MR_SOUND_M4A:
		audio_type = MMISRVAUD_RING_FMT_M4A;
		break;
	case MR_SOUND_PCM:
	default:
		return MR_FAILED;
	}
	
	if(loop)
	{
		play_times = AUD_PLAY_FOREVER;
	}
	
	audio_vol = s_mr_audio_mng.soundVolume;
	//if (MMIAPISET_IsPermitPlayRing(MMISET_RING_TYPE_OTHER))
	{
		MMIAPISET_PlayRingByPtr(MMISET_RING_TYPE_OTHER,
			audio_vol,
			(uint8*)data,
			dataLen,
			play_times,
			audio_type,
			MRRingCallBack
			);
		//2008-1-2 add
		if(loop)//ignore others. 
		{
			MMIMRAPP_startPlaySoundProtect();
			s_mr_mrPlaySound_info.data = (uint8*)data;
			s_mr_mrPlaySound_info.dataLen = dataLen;
			s_mr_mrPlaySound_info.loop = loop;
			s_mr_mrPlaySound_info.type = type;
			s_mr_mrPlaySound_info.Ckcount = 0;
		}
	}		
	
	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description : 	停止播放音频。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_stopSound (int type)
{
	MMIMR_Trace(("mrapp. mr_stopSound() type: %d", type));
	MMIMRAPP_StopPlaySoundProtect();//2008-1-2 add
	MMIAPISET_StopRing(MMISET_RING_TYPE_OTHER);//停止播放
	MMIMRAPP_resumeMp3();
	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description : 	启动定时器。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_timerStart(uint16 t)
{
	static uint8 calcNum = 0;
	MMIMR_TraceT(("mrapp. mr_timerStart() t: %d", t));
	
	if(calcNum ++ > 50){// 100ms情况下,100*50=5s
		calcNum = 0;
		SCI_Sleep(30);//temp. test SCM queue full bug
	}
	
	if(t != 0 && MMIMRAPP_timerStart(t) > 0)
	{
		return MR_SUCCESS;
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	停止定时器。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_timerStop(void)
{
	MMIMRAPP_timerStop(0);	
	return MR_SUCCESS;
}

char* mr_str_trim(char *src, char *trim_string)
{
	char *head_str;
	char *tail_str;

	if (src == NULL || src[0] == 0 || trim_string == NULL || trim_string[0] == 0)
	{
		return src;
	}
	
	head_str = src;
	while (*head_str != 0 && strchr(trim_string, *head_str) != NULL)
	{
		head_str++;
	}
	tail_str = head_str + strlen(head_str) - 1;
	while (tail_str >= head_str && strchr(trim_string, *tail_str) != NULL)
	{
		*tail_str = 0;
		tail_str--;
	}

	return head_str;
}

uint8 mr_fs_get_work_drv(void)
{
	uint8 dsmWorkDrv = 'E';

    switch(g_mr_global.device_type)
	{
	case FS_HS:
        dsmWorkDrv = 'C';
		break;
	case FS_UDISK:
        dsmWorkDrv = 'D';
		break;
	case FS_MMC:
#ifdef DUAL_TCARD_SUPPORT
        if(FALSE== MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN)
            &&TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
            dsmWorkDrv = 'F';
        else
#endif
        dsmWorkDrv = 'E';
		break;
#ifdef DOUBLE_TFCARD
	case FS_MMC_2:
        dsmWorkDrv = 'F';
		break;
#endif
	default:
		MRP_ASSERT(0);
		break;
	}
	
	return dsmWorkDrv;
}

char *mr_fs_get_work_path(void)
{
    char   path_name[MMIFMM_PATHNAME_LEN] = {0};

    SCI_MEMSET(path_name, 0x00, sizeof(path_name));
    MMIAPICOM_WstrToStr(g_mr_global.dir_ptr, path_name);

	return path_name;
}

int32 mr_fs_check_path_valid(char *path, int32 path_len)
{
    int32  ret  = 0;
    uint16 attr = 0;
    char   path_name[MMIFMM_PATHNAME_LEN] = {0};
    wchar  path_ucs2[MMIFMM_PATHNAME_LEN] = {0};

    if(path == NULL)
        return FALSE;

    if(path_len > 3)
    {
        strncpy(path_name, (const char *)path, sizeof(path_name) - 1);
        path_name[0] += 2;
#ifdef DUAL_TCARD_SUPPORT
        if(path[0] == 'c' || path[0] == 'C')
        {
            if(FALSE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN)
               && TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
            {
                path_name[0] += 1;
            }
        }
#endif
        MMIMRAPP_ConvertPath2Pc(path_name, path_len);

        if(path_name[path_len-1] == '/' || path_name[path_len-1] == '\\')
        {
            path_name[path_len-1] == '\0';
            path_len--;
        }

        MMIMRAPP_GB2UCS(path_ucs2, path_name, path_len);

        ret = SFS_GetAttr(path_ucs2, &attr);
        if(ret != SFS_ERROR_NONE)
        {
            return FALSE;
        }

        if(attr&SFS_ATTR_DIR)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void mr_fs_create_vm_dir(void)
{
    wchar  path_ucs2[MMIFMM_PATHNAME_LEN] = {0};
    char   filename[MMIFMM_PATHNAME_LEN]  = {0};
	int32  ret  = 0;
    uint16 attr = 0;
    char   path_name[MMIFMM_PATHNAME_LEN] = {0};

    SCI_MEMSET(path_name, 0x00, sizeof(path_name));
    MMIAPICOM_WstrToStr(g_mr_global.dir_ptr, path_name);

	
	sprintf(filename, "%c:\\%s", mr_fs_get_work_drv(), path_name/*mr_fs_get_work_path()*/);

    MMIMR_TraceF(("mr_fs_create_vm_dir()filename=%s ",filename));
	
	MMIAPICOM_StrToWstr(filename, path_ucs2);

	ret = SFS_GetAttr(path_ucs2, &attr);
	if ((ret != SFS_ERROR_NONE) || (!(attr&SFS_ATTR_DIR)))
	{
		SFS_CreateDirectory(path_ucs2);
	}
}

/*****************************************************************************/
// 	Description : 	取得文件长度。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_getLen(const char* filename)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	uint32 file_size_ptr;
	SFS_DATE_T modify_date;
	SFS_TIME_T modify_time;
	const uint8 *devName;
	uint16 devNameLen;
    SFS_HANDLE  fHandle = 0;
    SFS_ERROR_E fRet = 0;
    char   tmp_filename[MMIFMM_PATHNAME_LEN+1] = {0};
    char   *filename_ex = NULL;
	
	if(filename == NULL)
	{
		MMIMR_Trace(("mrapp. mr_getLen() no name!!"));
		return MR_FAILED;
	}
	MMIMR_TraceF(("mrapp. mr_getLen() file: %s", filename));
	
	devName = MMIMRAPP_getCurDevName(&devNameLen);
	if(devName != NULL)
	{
		memset(full_path, 0, sizeof(full_path));
		memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

        memset(tmp_filename, 0x00, sizeof(tmp_filename));
        strncpy(tmp_filename, filename, sizeof(tmp_filename) - 1);
        filename_ex = mr_str_trim(tmp_filename, " \\/\r\n");

        MMIMR_TraceF(("mrapp. mr_getLen() filename_ex=%s", filename_ex));

        if(filename_ex[0] == '/' || filename_ex[0] == '\\')
        {
            ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename_ex[1], strlen((char*)&filename_ex[1]));
        }
        else
        {
        	ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename_ex, strlen((char*)filename_ex));
        }
		if(	MMIFILE_CombineFullPath_fix(
			(const uint8*)devName, devNameLen,
			(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
			(const uint8*)ucs2_name_arr, ucs2_name_len,
			(uint8*)full_path,
			&full_path_len
			))
		{
            fHandle = SFS_CreateFile(full_path, SFS_MODE_OPEN_EXISTING|SFS_MODE_READ, NULL, NULL);
    		if(fHandle != SFS_INVALID_HANDLE)
            {
                fRet = SFS_GetFileSize(fHandle, &file_size_ptr);
                SFS_CloseFile(fHandle);
                if(SFS_ERROR_NONE == fRet)
                {
                    return (int32)file_size_ptr;
                }
            }
		}
	}
	
	MMIMR_Trace(("mrapp. mr_getLen() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	打开文件
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
MR_FILE_HANDLE mr_open(const char* filename,  uint32 mode)
{
	MMIFILE_HANDLE fHandle = FFS_INVALID_HANDLE;
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	uint32 opMode = SFS_MODE_OPEN_EXISTING;
	const uint8 *devName;
	uint16 devNameLen;
    uint16 path_name_len = MMIFMM_PATHNAME_LEN;
    wchar  path_name[MMIFMM_PATHNAME_LEN+1] = {0};
    char   tmp_filename[MMIFMM_PATHNAME_LEN+1] = {0};
    char   *filename_ex = NULL;
	
	if(filename == NULL)
	{
		MMIMR_TraceF(("mrapp. mr_open() no name in !!"));
		return NULL;
	}
	
	MMIMR_TraceF(("mrapp. mr_open() file: %s, mode: 0x%x", filename, mode));

	devName = MMIMRAPP_getCurDevName(&devNameLen);
	if(devName != NULL)
	{
		if(mode & MR_FILE_SHARE)
        {
            if(mode & MR_FILE_RDWR)
            {
                opMode |= SFS_MODE_SHARE_READ | SFS_MODE_SHARE_WRITE;
            }
            else if(mode & MR_FILE_RDONLY)
            {
                opMode |= SFS_MODE_SHARE_READ;
            }
            else if(mode & MR_FILE_WRONLY)
            {
            	opMode |= SFS_MODE_SHARE_WRITE;
            }
        }
		else if(mode & MR_FILE_RDONLY)
		{
			opMode |= SFS_MODE_READ;
		}
		else if(mode & MR_FILE_WRONLY)
		{
			opMode |= SFS_MODE_WRITE;
		}
		else if(mode & MR_FILE_RDWR)
		{
			opMode |= SFS_MODE_READ | SFS_MODE_WRITE;
		}
		
		if(mode & MR_FILE_CREATE)
		{
			opMode &= ~SFS_MODE_OPEN_EXISTING;
			opMode |= SFS_MODE_OPEN_ALWAYS;
		}
		memset(full_path, 0, sizeof(full_path));
		memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

        memset(tmp_filename, 0x00, sizeof(tmp_filename));
        strncpy(tmp_filename, filename, sizeof(tmp_filename) - 1);
        filename_ex = mr_str_trim(tmp_filename, " \\/\r\n");

        MMIMR_TraceF(("mrapp. mr_open() filename_ex=%s", filename_ex));

        if(filename_ex[0] == '/' || filename_ex[0] == '\\')
        {
    		ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename_ex[1], strlen(&filename_ex[1]));
        }
        else
        {
    		ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename_ex, strlen(filename_ex));
        }
		if(	MMIFILE_CombineFullPath_fix(
			(const uint8*)devName, devNameLen,
			(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
			(const uint8*)ucs2_name_arr, ucs2_name_len,
			(uint8*)full_path,
			&full_path_len
			))
		{
			MMIMR_TraceF(("dir_len: %d, %d, %d", g_mr_global.dir_len, ucs2_name_len, full_path_len));

            /*modify by zack@20120410*/
            //不应该支持在不存在的目录下创建文件，即不应该支持目录的自动创建
            MMIMRAPP_ConvertPath2Pc((char*)full_path, full_path_len);
            MMIAPIFMM_SplitFullPathExt((const uint16 *)full_path, full_path_len, path_name, &path_name_len, NULL, NULL, NULL, NULL);
			if(path_name[path_name_len-1] == '\\' || path_name[path_name_len-1] == '/')
			{
				path_name[path_name_len-1] = '\0';
				path_name_len--;
			}

            if(MMIAPIFMM_IsFolderExist((const wchar *)path_name, path_name_len))
            {
        		fHandle = (MR_FILE_HANDLE)MMIFILE_CreateFile((const uint16 *)full_path, opMode, NULL, NULL);
            }
		}
	}
	
	MMIMR_Trace(("mrapp. mr_open() fHandle: 0x%x", fHandle));
	return (MR_FILE_HANDLE)fHandle;
}

/*****************************************************************************/
// 	Description : 	关闭文件
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_close(MR_FILE_HANDLE f)
{
	MMIMR_TraceF(("mrapp. mr_close() fhandle: 0x%x", f));
	
	if(f != (MR_FILE_HANDLE)FFS_INVALID_HANDLE)
	{
		if(MMIFILE_CloseFile((MMIFILE_HANDLE)f) == SFS_ERROR_NONE)
		{
			MMIMR_TraceF(("mrapp. mr_close() success!"));
			return MR_SUCCESS;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_close() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	异步读文件
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_asyn_read(MR_FILE_HANDLE f, mr_asyn_fs_param *param)
{
	MMIMR_Trace(("mrapp. mr_asyn_read() MR_FAILED"));
	return MR_FAILED;
}


/*****************************************************************************/
// 	Description : 	异步写文件
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_asyn_write(MR_FILE_HANDLE f, mr_asyn_fs_param *param)
{
	MMIMR_Trace(("mrapp. mr_asyn_write() MR_FAILED"));
	return MR_FAILED;
}


/*****************************************************************************/
// 	Description : 	(同步)读取文件的内容到指定的缓冲。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_read(MR_FILE_HANDLE f,void *p,uint32 l)
{
	uint32 transmit;
	MMIMR_TraceF(("mrapp. mr_read() fhandle: 0x%x, len: %d, p: 0x%x", f, l, p));
	
	/*保持平台接口行为统一，读取0B返回成功*/
	if(f != (MR_FILE_HANDLE)FFS_INVALID_HANDLE && p != NULL && l >= 0)
	{
		if(MMIFILE_ReadFile(f, p, l, &transmit, NULL) == SFS_ERROR_NONE)
		{
			return (int32)transmit;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_read() MR_FAILED "));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	(同步)写文件。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_write(MR_FILE_HANDLE f,void *p,uint32 l)
{
	uint32 transmit;
	MMIMR_TraceF(("mrapp. mr_write() fhandle: 0x%x, len: %d, p: 0x%x", f, l, p));
	
	/*保持平台接口行为统一，写入0B返回成功*/
	if(f != (MR_FILE_HANDLE)FFS_INVALID_HANDLE && p != NULL && l >= 0)
	{
		if(MMIFILE_WriteFile(f, p, l, &transmit, NULL) == SFS_ERROR_NONE)
			return (int32)transmit;
	}
	
	MMIMR_Trace(("mrapp. mr_write() MR_FAILED "));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	取得文件类型信息。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_info(const char* filename)
{
	SFS_HANDLE		sfs_handle = 0;
	SFS_FIND_DATA_T find_data = {0};
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
	
	if(filename != NULL)
	{
		MMIMR_Trace(("mrapp. mr_info() file: %s", filename));

		devName = MMIMRAPP_getCurDevName(&devNameLen);
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

        if(filename[0] == '/' || filename[0] == '\\')
        {			
			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename[1], strlen(&filename[1]));
        }
        else
        {
            ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename, strlen(filename));
        }

			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
				sfs_handle = SFS_FindFirstFile((const uint16 *)full_path, &find_data);				
				MMIMR_Trace(("find_data.attr: 0x%x", find_data.attr));
				if(SFS_INVALID_HANDLE != sfs_handle)
				{
					SFS_FindClose(sfs_handle);
					if(SFS_ATTR_DIR & find_data.attr)
					{
						return MR_IS_DIR;
					}
					else
					{
						return MR_IS_FILE;
					}
				}
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_info() MR_IS_INVALID"));	
	return MR_IS_INVALID;	
}

/*****************************************************************************/
// 	Description : 	设置文件指针
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_seek(MR_FILE_HANDLE f, int32 pos, int method)
{
	uint8 origin;
	uint32 ret;
	MMIMR_TraceF(("mrapp. mr_seek() fhandle: 0x%x, pos: %d, method: 0x%x", f, pos, method));
	
	if(f != (MR_FILE_HANDLE)FFS_INVALID_HANDLE)
	{
		switch(method)
		{
		case MR_SEEK_SET:
			origin = SFS_SEEK_BEGIN;
			break;
		case MR_SEEK_CUR:
			origin = SFS_SEEK_CUR;
			break;
		case MR_SEEK_END:
			origin = SFS_SEEK_END;
			break;
		default:
			MMIMR_TraceM(("mr_seek para error !!"));
			return MR_FAILED;
		}
		ret = MMIFILE_SetFilePointer((MMIFILE_HANDLE)f, pos, origin);
		MMIMR_TraceF(("mrapp. mr_seek() ret: %d", ret));
		if(ret == SFS_ERROR_NONE)//comment. care the returne value define !
		{
			return MR_SUCCESS;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_seek() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	 删除文件
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_remove(const char* filename)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
	
	if(filename != NULL)
	{
		MMIMR_TraceF(("mrapp. mr_remove() file: %s", filename));

		devName = MMIMRAPP_getCurDevName(&devNameLen);
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

            if(filename[0] == '/' || filename[0] == '\\')
            {			
    			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename[1], strlen(&filename[1]));
            }
            else
            {
                ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename, strlen(filename));
            }

			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
				
			{
				if(MMIFILE_DeleteFile((const uint16*)full_path, NULL) == SFS_ERROR_NONE)
					return MR_SUCCESS;
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_remove() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	 判断两个文件是否在同一目录下
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
BOOLEAN mr_fs_diff_path(const char* oldname, const char* newname)
{
    int32 oldname_len = 0;
    int32 newname_len = 0;
    int32 comp_result = 0;
    uint16 oldpath_len = MMIMRAPP_FILE_NAME_MAX_LEN;
    uint16 newpath_len = MMIMRAPP_FILE_NAME_MAX_LEN;
    uint8 oldname_ex[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
    uint8 newname_ex[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
    uint8 fullpathname_old[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
    uint8 fullpathname_new[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
    uint8 pathname_old[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
    uint8 pathname_new[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};

    if(oldname == NULL || newname == NULL)
    {
        return FALSE;
    }

    if(newname[0] == 0 || (newname[0] == ' ' && strlen(newname) == 1))
    {
        return FALSE;
    }

    SCI_MEMSET(oldname_ex, 0, sizeof(oldname_ex));
    SCI_MEMSET(newname_ex, 0, sizeof(newname_ex));
    SCI_MEMCPY(oldname_ex, oldname, strlen(oldname)); /*lint !e666*/
    SCI_MEMCPY(newname_ex, newname, strlen(newname)); /*lint !e666*/

    MMIMRAPP_ConvertPath2Pc(oldname_ex, strlen(oldname_ex));
    MMIMRAPP_ConvertPath2Pc(newname_ex, strlen(newname_ex));

    oldname_len = mr_fs_get_filename(fullpathname_old, oldname_ex);
    newname_len = mr_fs_get_filename(fullpathname_new, newname_ex);

    MMIAPIFMM_SplitFullPathExt(fullpathname_old, oldname_len, pathname_old, &oldpath_len, NULL, NULL, NULL, NULL);
    MMIAPIFMM_SplitFullPathExt(fullpathname_new, newname_len, pathname_new, &newpath_len, NULL, NULL, NULL, NULL);

    comp_result = MMIAPICOM_CompareTwoWstrExt(pathname_old, oldpath_len*2, pathname_new, newpath_len*2, FALSE);

    MMIMR_TraceF(("%d %d, %d, %d, %d", comp_result, oldname_len, newname_len, oldpath_len, newpath_len));
    
    if(0 != comp_result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
// 	Description : 	 不同路径下文件重命名
//	Global resource dependence : none
//  Author: zack
//	Note: 为了屏蔽平台间差异性，间接实现不同路径下文件的重命名，效率低请慎用
/*****************************************************************************/
int32 mr_rename_ex(const char* oldname, const char* newname)
{
    int32 oldHandle = 0;
    int32 newHandle = 0;
    int32 oldName_len = 0;
    int32 newName_len = 0;
    int32 fResult   = 0;
    uint32 fileSize = 0;
    uint32 fileLeft = 0;
    uint32 fileRead = 0;
    uint32 fileWrite= 0;
    uint32 fileBufLen = 5*1024;
    uint8  *fileBuf = NULL;
    uint16 pathdir_len = MMIMRAPP_FILE_NAME_MAX_LEN;
	wchar fullpathname_old[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
	wchar fullpathname_new[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};
    wchar pathdir_new[MMIMRAPP_FILE_NAME_MAX_LEN] = {0};

    if(oldname == NULL || newname == NULL)
    {
        return MR_FAILED;
    }
        
    oldName_len = mr_fs_get_filename(fullpathname_old, oldname);
    newName_len = mr_fs_get_filename(fullpathname_new, newname);

    MMIMRAPP_ConvertPath2Pc(fullpathname_old, oldName_len);
    MMIMRAPP_ConvertPath2Pc(fullpathname_new, newName_len);

    MMIAPIFMM_SplitFullPathExt(fullpathname_new, newName_len, pathdir_new, &pathdir_len, NULL, NULL, NULL, NULL);
    
	if(pathdir_new[pathdir_len-1] == '\\' || pathdir_new[pathdir_len-1] == '/')
	{
		pathdir_new[pathdir_len-1] = '\0';
		pathdir_len--;
	}
	
    if(!MMIAPIFMM_IsFolderExist(pathdir_new, pathdir_len))
    {
        return MR_FAILED;
    }

    oldHandle = SFS_CreateFile(fullpathname_old, SFS_MODE_OPEN_EXISTING|SFS_MODE_READ, NULL, NULL);
    if(oldHandle == 0)
    {
        return MR_FAILED;
    }

    fResult = SFS_GetFileSize(oldHandle, &fileSize);
    if(fResult == SFS_ERROR_NONE)
    {
        newHandle = SFS_CreateFile(fullpathname_new, SFS_MODE_CREATE_NEW|SFS_MODE_WRITE, NULL, NULL);
        if(newHandle == 0)
        {
            SFS_CloseFile(oldHandle);
            return MR_FAILED;
        }

        fileBuf = (uint8*)SCI_ALLOCA(fileBufLen);

        if(fileBuf == NULL)
        {
            SFS_CloseFile(oldHandle);
            SFS_CloseFile(newHandle);
            return MR_FAILED;
        }

        while(fileLeft < fileSize)
        {
            SCI_MEMSET(fileBuf, 0x00, fileBufLen);
            SFS_ReadFile(oldHandle, (void *)fileBuf, fileBufLen, &fileRead, NULL);
            SFS_WriteFile(newHandle, (void *)fileBuf, fileRead, &fileWrite, NULL);
            if(fileRead != fileWrite)
            {
                SFS_CloseFile(oldHandle);
                SFS_CloseFile(newHandle);
                SFS_DeleteFile(fullpathname_new, NULL);
            }
            fileLeft += fileWrite;
        }

        if(fileBuf != NULL)
        {
            SCI_FREE(fileBuf);
            fileBuf = NULL;
        }

        SFS_CloseFile(oldHandle);
        SFS_CloseFile(newHandle);
        SFS_DeleteFile(fullpathname_old, NULL);
        MMIMR_TraceF(("mrapp. mr_rename_ex() suceess"));
        return MR_SUCCESS;
    }
    else
    {
        SFS_CloseFile(oldHandle);
        return MR_FAILED;
    }
}

/*****************************************************************************/
// 	Description : 	 文件重命名
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_rename(const char* oldname, const char* newname)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
    char oldname_ex[MMIMRAPP_FILE_NAME_MAX_LEN*2] = {0};
    char newname_ex[MMIMRAPP_FILE_NAME_MAX_LEN*2] = {0};
	
	if(oldname != NULL && newname != NULL)
	{
		MMIMR_TraceF(("mrapp. mr_rename() oldname: %s, newname: %s", oldname, newname));

        /*modify by zack@20120411*/
        //统一各平台接口行为, 该接口不支持文件夹重命名
        if(mr_info(oldname) != MR_IS_FILE)
        {
            return MR_FAILED;
        }

        //处理不同路径下的文件重命名
        if(mr_fs_diff_path(oldname, newname))
        {
            return mr_rename_ex(oldname, newname);
        }

        SCI_MEMSET(oldname_ex,0,sizeof(oldname_ex));
        SCI_MEMSET(newname_ex,0,sizeof(newname_ex));
        SCI_MEMCPY(oldname_ex, oldname, strlen(oldname)); /*lint !e666*/
        SCI_MEMCPY(newname_ex, newname, strlen(newname)); /*lint !e666*/

        MMIMRAPP_ConvertPath2Loc(oldname_ex, strlen(oldname_ex));
        MMIMRAPP_ConvertPath2Loc(newname_ex, strlen(newname_ex));


		devName = MMIMRAPP_getCurDevName(&devNameLen);
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

            if(oldname_ex[0] == '/' || oldname_ex[0] == '\\')
            {	
    			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&oldname_ex[1], strlen(&oldname_ex[1]));
            }
            else
            {
    			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)oldname_ex, strlen(oldname_ex));
            }
			
			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
				memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

                if(newname_ex[0] == '/' || newname_ex[0] == '\\')
                {	
    				ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&newname_ex[1], strlen((char*)&newname_ex[1]));
                }
                else
                {
                    ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)newname_ex, strlen((char*)newname_ex));
                }
				if(MMIFILE_RenameFile((wchar*)full_path, full_path_len/2, (wchar*)ucs2_name_arr, ucs2_name_len/2))
					return MR_SUCCESS;
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_rename() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	创建目录。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_mkDir(const char* name)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
    uint16 path_name_len = MMIFMM_PATHNAME_LEN;
    wchar  path_name[MMIFMM_PATHNAME_LEN+1] = {0};
    char   tmp_filename[MMIFMM_PATHNAME_LEN+1] = {0};
    char   *filename_ex = NULL;
	
	if(name != NULL)
	{
		MMIMR_TraceF(("mrapp. mr_mkDir() name: %s", name));

        /*modify by zack@20120411*/
        //系统保留目录不应该被创建成功
        if((strncmp(name, ".", 1) == 0) || (strncmp(name, "..", 2) == 0) || (strncmp(name, " ", 2) == 0))
        {
            return MR_FAILED;
        }
            
		devName = MMIMRAPP_getCurDevName(&devNameLen);
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

            memset(tmp_filename, 0x00, sizeof(tmp_filename));
            strncpy(tmp_filename, name, sizeof(tmp_filename) - 1);
            filename_ex = mr_str_trim(tmp_filename, " \\/\r\n");

            MMIMR_TraceF(("mrapp. mr_mkDir() filename_ex=%s", filename_ex));

            if(filename_ex[0] == '/' || filename_ex[0] == '\\')
            {				
    			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename_ex[1], strlen((char*)&filename_ex[1]));
            }
            else
            {
                ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename_ex, strlen((char*)filename_ex));
            }
			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
	            /*modify by zack@20120410*/
                //不应该支持在不存在的目录下创建文件夹，即不应该支持目录的自动创建
                MMIMRAPP_ConvertPath2Pc((char*)full_path, full_path_len);
                MMIAPIFMM_SplitFullPathExt((const uint16 *)full_path, full_path_len, path_name, &path_name_len, NULL, NULL, NULL, NULL);
    			if(path_name[path_name_len-1] == '\\' || path_name[path_name_len-1] == '/')
    			{
    				path_name[path_name_len-1] = '\0';
    				path_name_len--;
    			}
                MMIMR_TraceF(("dir_len: %d, %d, %d, %d", g_mr_global.dir_len, ucs2_name_len, full_path_len, path_name_len));

                if(MMIAPIFMM_IsFolderExist((const wchar *)path_name, path_name_len))
                {
    				if(MMIFILE_CreateDirectory((const uint16*)full_path) == SFS_ERROR_NONE)
    					return MR_SUCCESS;
                }
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_mkDir() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	删除目录
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_rmDir(const char* name)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
	
	if(name != NULL)
	{
		MMIMR_TraceF(("mrapp. mr_rmDir() name: %s", name));
		devName = MMIMRAPP_getCurDevName(&devNameLen);
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

            if(name[0] == '/' || name[0] == '\\')
            {				
    			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&name[1], strlen((char*)&name[1]));
            }
            else
            {
    			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)name, strlen((char*)name));
            }
			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
				if(MMIFILE_DeleteDirectory((const uint16*)full_path) == SFS_ERROR_NONE)
					return MR_SUCCESS;
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_rmDir() MR_FAILED"));
	return MR_FAILED;
}


/*****************************************************************************/
// 	Description : 	准备搜索name指定的目录，当dsm调用该函数后，
//系统初始化一个目录搜索，并返回第一个目录搜索的结果，
//每当dsm调用一次mr_findGetNext函数，系统返回下一个该目录下
//的文件或一级子目录名。该函数只返回查找句柄。当name为
//空字符串""时（注意name指向空串，但name不是NULL），搜索
//DSM引擎的文件根目录。如：若手机以"/dsmdata/"作为DSM引擎
//的文件根目录，当name为空字符串时，搜索目录"/dsmdata"。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
MR_SEARCH_HANDLE mr_findStart(const char* name, char* buffer, uint32 len)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;	
	MMIFILE_FIND_DATA_T  find_data = {0};
	MMIFILE_HANDLE		file_handle = PNULL;
	uint32 ucsNameLen, gbNameLen;
	const uint8 *devName;
	uint16 devNameLen;
	
	if(name != NULL && buffer != NULL && len != 0)
	{
		MMIMR_Trace(("mrapp. mr_findStart() name: %s, len: %d", name, len));
	
		devName = MMIMRAPP_getCurDevName(&devNameLen);
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));
			if(name[0] == 0 || (name[0] == ' ' && strlen(name) == 1))
			{
				ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)"*", strlen("*"));
			}
			else
			{
                if(name[0] == '/' || name[0] == '\\')
                {
    				strcpy((char*)full_path, &name[1]);
                }
                else
                {
                    strcpy((char*)full_path, name);
                }
				
				/*add by zack@20120508 start*/
					gbNameLen = strlen(full_path);
					if(full_path[gbNameLen-1] == '/' || full_path[gbNameLen-1] == '\\')
					{
						 full_path[gbNameLen-1] = '\0';
					}
                /*add by zack@20120508 end*/
				strcat((char*)full_path, "\\*");
				ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)full_path, strlen((char*)full_path));			
			}
			if( MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
				file_handle = MMIFILE_FindFirstFile((const uint16 *)full_path, full_path_len/2, &find_data);
				
				if(SFS_INVALID_HANDLE != file_handle)
				{
					ucsNameLen = MMIMRAPP_getUcstrLen((uint8 *)find_data.name);
					if(ucsNameLen >= MMIFILE_FILE_NAME_MAX_LEN_AD)//for rocky file name length limit of filemgr
					{
						if(MMIFILE_FILE_NAME_MAX_LEN_AD%2 == 0) /*lint !e506 !e774 !e831*/
							ucsNameLen = MMIFILE_FILE_NAME_MAX_LEN_AD - 2;
						else//奇数-1
							ucsNameLen = MMIFILE_FILE_NAME_MAX_LEN_AD - 1;
					}
					gbNameLen = GUI_WstrToGB((uint8*)ucs2_name_arr, (uint8 *)find_data.name, ucsNameLen/2); /*lint !e64*/
					MMIMR_Trace(("ucsNameLen: %d, gbNameLen: %d, len: %d", ucsNameLen, gbNameLen, len));
					if(gbNameLen >= len)
					{
						gbNameLen = len - 1;
					}
					SCI_MEMCPY(buffer, ucs2_name_arr, gbNameLen);
					buffer[gbNameLen] = '\0';
					MMIMR_Trace(("buffer: %s", buffer));
					return file_handle;
				}
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_findStart() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	取得一个目录搜索的结果。将结果放置于buffer中
// 。当目录中的结果都遍历过后，返回MR_FAILED。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_findGetNext(MR_SEARCH_HANDLE search_handle, char* buffer, uint32 len)
{
	MMIFILE_FIND_DATA_T  find_data = {0};
	uint32 ucsNameLen, gbNameLen;
	MMIFILE_ERROR_E ret;
	MMIMR_Trace(("mrapp. mr_findGetNext() fhandle: 0x%x, len: %d", search_handle, len));
	if(search_handle != (MR_SEARCH_HANDLE)FFS_INVALID_HANDLE && buffer != NULL && len != 0)
	{
		ret = MMIFILE_FindNextFile((MMIFILE_HANDLE)search_handle, &find_data);
		MMIMR_Trace(("mrapp. mr_findGetNext() ret: %d", ret));
		if(ret == FFS_ERROR_NONE) /*lint !e58*/
		{
			ucsNameLen = MMIMRAPP_getUcstrLen((uint8 *)find_data.name);
			if(ucsNameLen >= MMIFILE_FILE_NAME_MAX_LEN_AD)//for rocky file name length limit of filemgr
			{
				if(MMIFILE_FILE_NAME_MAX_LEN_AD%2 == 0) /*lint !e506 !e774 !e831*/
					ucsNameLen = MMIFILE_FILE_NAME_MAX_LEN_AD - 2;
				else//奇数-1
					ucsNameLen = MMIFILE_FILE_NAME_MAX_LEN_AD - 1;
			}
			gbNameLen = GUI_WstrToGB((uint8*)full_path, (uint8 *)find_data.name, ucsNameLen/2); /*lint !e64*/
			MMIMR_Trace(("ucsNameLen: %d, gbNameLen: %d, len: %d", ucsNameLen, gbNameLen, len));
			if(gbNameLen >= len)
			{
				gbNameLen = len - 1;
			}
			SCI_MEMCPY(buffer, full_path, gbNameLen);
			buffer[gbNameLen] = '\0';
			MMIMR_Trace(("buffer: %s", buffer));
			return MR_SUCCESS;
		}	
	}
	
	MMIMR_Trace(("mrapp. mr_findGetNext() MR_FAILED"));
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	目录搜索结束。中止一个mr_findStart开启的目录搜索
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_findStop(MR_SEARCH_HANDLE search_handle)
{
	MMIMR_TraceF(("mrapp. mr_findStop() fhandle: 0x%x", search_handle));
	
	if(search_handle != (MR_SEARCH_HANDLE)FFS_INVALID_HANDLE)
	{
		if(MMIFILE_FindClose((MMIFILE_HANDLE)search_handle) == SFS_ERROR_NONE)
			return MR_SUCCESS;
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 	该函数用于调试使用，返回的是最后一次操作文
// 	件失败的错误信息.
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_ferrno(void)
{
	return 0;
}

/****************************************************************************
函数名:int32 mr_fs_get_filename(char *fullpath, const char *filename) 
描  述:获取文件全路径
参  数:相对路径
返  回:全路径长度
****************************************************************************/
int32 mr_fs_get_filename(char *fullpath, const char *filename) 
{
	const uint8 *devName;
	uint16 devNameLen;
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;

	devName = MMIMRAPP_getCurDevName(&devNameLen);

    if(filename[0] == '/' || filename[0] == '\\')
    {
    	ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename[1], strlen(&filename[1]));
    }
    else
    {
        ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename, strlen(filename));
    }

	MMIFILE_CombineFullPath_fix(
		(const uint8*)devName, devNameLen,
		(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
		(const uint8*)ucs2_name_arr, ucs2_name_len,
		(uint8*)fullpath,
		&full_path_len
		);

	return full_path_len;
}

/*****************************************************************************/
// 	Description :打印调试信息
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void mr_printf(const char *format,...)
{
    char *ptr;
    va_list params;
    int32 fHandle = 0;
    int32 ret = 0;
    int32 writen = 0;

#ifdef MRAPPLOGOPEN
#ifdef MR_APP_LOG_FILE
    {
    	memset(s_printfbuf, 0, sizeof(s_printfbuf));
    	strcpy(s_printfbuf, "\n* MR log: ");
    	ptr = s_printfbuf + sizeof("\n* MR log: ") - 1;
    	va_start(params, format);
#ifndef WIN32
    	vsnprintf(ptr, sizeof(s_printfbuf) - sizeof("\n* MR log: ") -1, format, params); 
#else
    	//vsprintf(ptr, format, params); 
    	_vsnprintf(ptr, sizeof(s_printfbuf) - sizeof("\n* MR log: ") -1, format, params); 
#endif
    	fHandle = SFS_CreateFile(L"E:\\mythroad_spr\\spr.txt", SFS_MODE_OPEN_ALWAYS|SFS_MODE_WRITE, PNULL, PNULL);
    	if(fHandle != 0)
        {
            SFS_SetFilePointer(fHandle, 0, SFS_SEEK_END);
            SFS_WriteFile(fHandle, (void*)s_printfbuf, sizeof(s_printfbuf), &writen, PNULL);
            SFS_CloseFile(fHandle);
        }   
    	va_end(params);	
    }
#else
    {
    	memset(s_printfbuf, 0, sizeof(s_printfbuf));
    	strcpy(s_printfbuf, "* MR log: ");
    	ptr = s_printfbuf + sizeof("* MR log: ") - 1;
    	va_start(params, format);
#ifndef WIN32
    	vsnprintf(ptr, sizeof(s_printfbuf) - sizeof("* MR log: ") -1, format, params); 
#else
    	//vsprintf(ptr, format, params); 
    	_vsnprintf(ptr, sizeof(s_printfbuf) - sizeof("* MR log: ") -1, format, params); 
#endif
    	MMIMR_TraceApp(((char*)s_printfbuf));
    	
    	va_end(params);	
    }
#endif
#endif
}

/*****************************************************************************/
// 	Description : DSM 内存获取函数。
//	Global resource dependence : none
//  Author: qgp
//	Note:请务必确保内存分配无误。修改时请同步mr_mem_free()中的修改
/*****************************************************************************/
int32 mr_mem_get (char** mem_base, uint32* mem_len)
{
#ifdef __MR_CFG_VMMEM_CUSTOM__
    uint8 *buf_ptr_1 = PNULL;
	int32 mem_size = __MRAPP_CFG_VAR_VMMEM_SIZE__;

	MMIMR_Trace(("mr_mem_get: mem_len=%d mem_size=%d", *mem_len, mem_size));
	if(*mem_len > 0 && *mem_len <= __MRAPP_CFG_VAR_VMMEM_MAX_SIZE__) 
    {
		mem_size = *mem_len;
	}
    else if(*mem_len > __MRAPP_CFG_VAR_VMMEM_MAX_SIZE__)
    {
        mem_size = __MRAPP_CFG_VAR_VMMEM_MAX_SIZE__;
    }
#ifdef UI_MULTILAYER_SUPPORT
	UILAYER_SetMainLayerMemFormat(UILAYER_MEM_NONE);
#endif	
	buf_ptr_1 = SCI_ALLOCA(mem_size);
#ifdef UI_MULTILAYER_SUPPORT
	UILAYER_SetMainLayerMemFormat(UILAYER_MEM_DOUBLE_COPY);
#endif	
	if(buf_ptr_1 != NULL)
    {
        *mem_base = (char*)buf_ptr_1;
		*mem_len = mem_size;
        memset(*mem_base, 0, *mem_len);
		MMIMR_Trace(("mrapp. mr_mem_get(). Start addr: 0x%x, size: 0x%x, %d", *mem_base, *mem_len, g_mr_curApp));
		
		/*保证内存不要少于600k底线(240*320屏幕)*/
		SCI_ASSERT(*mem_len >= 600*1024);
		return MR_SUCCESS;
	}	
	
	MMIMR_Trace(("mrapp. mr_mem_get(). failed! "));
	*mem_base = NULL;
	*mem_len = 0;
	return MR_FAILED;
#else
#ifdef SHARE_BLOCK_MEM
	uint8 *buf_ptr_1 = PNULL;
	BLOCK_MEM_ID_E mem_id; 
    
	mem_id = BLOCK_MEM_POOL_MRAPP; /*lint !e64*/

    MMIMR_Trace(("mrapp. mr_mem_get(). mem_id: %d, size: 0x%x, left: 0x%08x, mem_len: %d", mem_id, BL_GetSize(mem_id), SCI_GetSystemSpaceMaxAvalidSpace(), *mem_len));
#ifdef UI_MULTILAYER_SUPPORT
	UILAYER_SetMainLayerMemFormat(UILAYER_MEM_NONE);
#endif
	buf_ptr_1 = BL_Malloc(mem_id);
#ifdef UI_MULTILAYER_SUPPORT
	UILAYER_SetMainLayerMemFormat(UILAYER_MEM_DOUBLE_COPY);
#endif    
	if(buf_ptr_1 != NULL)
    {
		*mem_base = (char*)buf_ptr_1;
		*mem_len = BL_GetSize(mem_id);
		MMIMR_Trace(("mrapp. mr_mem_get(). Start addr: 0x%x, size: 0x%x, left: 0x%08x, %d", *mem_base, *mem_len, SCI_GetSystemSpaceMaxAvalidSpace(), g_mr_curApp));
		
		/*保证内存不要少于600k底线(240*320屏幕)*/
		SCI_ASSERT(*mem_len >= 600*1024);
			return MR_SUCCESS;
    }	

	MMIMR_Trace(("mrapp. mr_mem_get(). failed! "));
	*mem_base = NULL;
	*mem_len = 0;
	return MR_FAILED;
#else//使用直接划分的mem buf
	*mem_base = (char*)s_mr_run_mem;
	*mem_len = (uint32)MR_APP_MEM_SIZE;// 4byte对齐
	MMIMR_Trace(("mrapp. mr_mem_get(). Start addr: 0x%x, size: 0x%x", *mem_base, *mem_len));
	return MR_SUCCESS;
#endif	
#endif	
}

/*****************************************************************************/
// 	Description : 释放由mr_mem_get申请的内存
//	Global resource dependence : none
//  Author: qgp
//	Note:若为静态空间s_mr_mem[]，这里不不必做任何事情
/*****************************************************************************/
int32 mr_mem_free (char* mem, uint32 mem_len)
{
	MMIMR_Trace(("mrapp. mr_mem_free(), len: %d", mem_len));
#ifdef __MR_CFG_VMMEM_CUSTOM__
	if(mem != NULL)
	{
	    SCI_FREE(mem);
	}
#else
#ifdef SHARE_BLOCK_MEM
	BL_Free(BLOCK_MEM_POOL_MRAPP); /*lint !e64*/
#else
	//do nothing
#endif
#endif
	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :当用户选择退出应用时，DSM菜单调用该函数，通知
//			系统释放资源。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_exit(void)
{
	MMIMR_Trace(("mrapp. mr_exit()"));
	
	MMIMRAPP_setAllReady(FALSE);
#ifdef MR_BINDTO_VIRTUAL_WIN
	if(g_mr_vmBackRun != MRAPP_MAX)
	{
		MMIMR_Trace(("mrapp. exit bk, %d", g_mr_vmBackRun));
		MMK_PostMsg(VIRTUAL_WIN_ID, MMRAPP_MSG_SYS_EXIT, NULL, 0);
		MMI_TriggerMMITask();
	}
	else
#endif
		if(MMK_IsOpenWin(MMIMRAPP_BASE_WIN_ID))
		{
			MMK_PostMsg(MMIMRAPP_BASE_WIN_ID, MMRAPP_MSG_SYS_EXIT, NULL, 0);
			MMI_TriggerMMITask();
		}
		
		MMIMR_Trace(("mrapp. mr_exit() END"));
		return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :同步Cache
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 mr_cacheSync(void* addr, int32 len)
{
    MMIMR_Trace(("mrapp. mr_cacheSync() start"));
#ifndef WIN32
    MMU_IcacheSyncAll();
#endif
    MMIMR_Trace(("mrapp. mr_cacheSync() end"));

    return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :获取系统时间，单位毫秒。系统时间可以以DSM引
//擎启动之前的任意时刻为起始时间，返回从起始时间到目
//前经过的毫秒数。例如这个函数可以实现为返回系统启动
//后经过的毫秒数。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
uint32 mr_getTime(void)
{
	MMIMR_TraceT(("mrapp. mr_getTime()"));
	return SCI_GetTickCount();	
}

/*****************************************************************************/
// 	Description :获取系统日期时间。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_getDatetime(mr_datetime* datetime)
{
	SCI_DATE_T  date;
	SCI_TIME_T  time;
	MMIMR_Trace(("mrapp. mr_getDatetime()"));
	
	if(datetime != NULL)
	{
		if(TM_GetSysDate(&date) == ERR_TM_NONE
			&& TM_GetSysTime(&time) == ERR_TM_NONE
			)
		{
			datetime->year = date.year;
			datetime->month = date.mon;
			datetime->day = date.mday;
			datetime->hour = time.hour;
			datetime->minute = time.min;
			datetime->second = time.sec;
			
			return MR_SUCCESS;
		}
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description :让当前任务睡眠ms毫秒。方便调试的接口，可实现为空。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_sleep(uint32 ms)
{
	MMIMR_Trace(("mrapp. mr_sleep() ms: %d", ms));
	
	SCI_Sleep(ms);
	
	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :发送一条短消息。
//	Global resource dependence : none
//  Author: qgp
//	Note:
// MR_SUCCESS	发送成功
// MR_FAILED	发送失败
// MR_IGNORE	未准备好
/*****************************************************************************/
int32 mr_sendSms(char* pNumber, char*pContent, int32 flags)
{
	int32 resValue = MR_FAILED;
	BOOLEAN result = FALSE;
	BOOLEAN repFlag = FALSE;
	MMISMS_SEND_DATA_T send_data_ptr = {0};
	uint8 numLen = 0;
	MMI_STRING_T msg = {0};
	char *temp = NULL;
	int32 lenth = 0;
	int32 i =0;
	MMIMR_Trace(("mrapp. mr_sendSms() pNumber: %s, len: %d, ucs2?: 0x%x", pNumber, strlen(pNumber), flags)); /*lint !e831*/
	
	if(pNumber == NULL  /*lint !e831*/
		|| pContent == NULL /*lint !e831*/
		|| (numLen = strlen(pNumber)) == 0) /*lint !e774*/
	{
		return MR_IGNORE;
	}
	
	if((int)(flags&0x00000007) == MR_ENCODE_ASCII)
	{
		msg.wstr_len = (uint16)strlen(pContent);
		msg.wstr_ptr = (wchar*)SCI_ALLOCA((msg.wstr_len + 1) * sizeof(wchar)); 
		memset(msg.wstr_ptr, 0, (msg.wstr_len + 1) * sizeof(wchar));
		MMI_STRNTOWSTR((wchar*)msg.wstr_ptr, msg.wstr_len, (uint8*)pContent, msg.wstr_len, msg.wstr_len); /*lint !e64*/
	}
	else
	{
#ifndef WIN32
		lenth = mr_str_wstrlen(pContent) + 2;
		temp = (char*)SCI_ALLOCA(lenth);
		memset(temp, 0, lenth);
		SCI_MEMCPY(temp, pContent, lenth);
		mr_str_convert_endian((char*)temp);
#endif	
		msg.wstr_len = (uint16)MMIMRAPP_getUcstrLen((uint8*)temp)/2;
		msg.wstr_ptr = (wchar*)SCI_ALLOCA((msg.wstr_len + 1) * sizeof(wchar));
		memset(msg.wstr_ptr, 0, (msg.wstr_len + 1) * sizeof(wchar));
		MMIAPICOM_Wstrncpy(msg.wstr_ptr, (const wchar*)temp, msg.wstr_len);
	}
	
	if((int)(flags&0x00000008))
	{
		repFlag = MMISMS_GetStatusReportFlag();
		if(repFlag)
		{
			MMISMS_SetStatusReportFlag(FALSE);
		}
	}

	if((int)(flags&0x00000010))
	{
		MMISMS_ClearOperInfo();
	}

	MMIMR_Trace(("mrapp. mr_sendSms() msg.wstr_len: %d", msg.wstr_len));
	if(0 != msg.wstr_len)
	{
		numLen = MIN(numLen, MMISMS_NUMBER_MAX_LEN);
		send_data_ptr.is_need_packet = TRUE;
		send_data_ptr.dual_sys = MMIMRAPP_GetActiveSim();
		send_data_ptr.src_port = 0;
		send_data_ptr.dst_port = 0;
		send_data_ptr.sms_content_ptr = &msg;
		send_data_ptr.dest_addr_ptr = (const uint8*)pNumber;
		send_data_ptr.dest_addr_len = numLen;
		send_data_ptr.send_callback = MMIMRAPP_SMSCnf;

		result = MMIAPISMS_SendSmsForOther(&send_data_ptr);
		
		if(result)
		{
			resValue= MR_SUCCESS;
		}

		MMIMR_Trace(("mrapp. mr_sendSms() resValue: %d", result));
	}

	if(NULL != temp)
	{
		SCI_FREE(temp);
	}

	if(NULL != msg.wstr_ptr)
	{
		SCI_FREE(msg.wstr_ptr);
	}
	
	return resValue;
}

/*****************************************************************************/
// 	Description :拨打电话。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void mr_call(char * pNumber)
{
	uint8 len;
	MN_DUAL_SYS_E dual_sys = MMIMRAPP_GetActiveSim();
	
	if(pNumber != NULL)
    {
		len = strlen(pNumber);		
		if(len > CC_MAX_TELE_NUM_LEN)
        {//length limited
			MMIPUB_OpenAlertWinByTextId(PNULL,TXT_CC_INVALID_NUM,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
		}
        else
        {
			MMIMR_Trace(("mrapp. mr_call() pNumber: %s", pNumber));
#if (SPR_VERSION >= 0x10A1140)
			MMIAPICC_MakeCall(dual_sys, (uint8*)pNumber, len, PNULL, PNULL, MMIAPICC_GetCallSimType(dual_sys), CC_CALL_NORMAL_CALL, PNULL);
#else
			MMIAPICC_MakeCall(dual_sys, (uint8*)pNumber, len, PNULL, MMIAPICC_GetCallSimType(dual_sys), CC_CALL_NORMAL_CALL, PNULL);
#endif
		}
	}
	return;
}

/*****************************************************************************/
// 	Description :连接WAP。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void mr_connectWAP(char* url)
{
#ifdef BROWSER_SUPPORT
    MMIBROWSER_ENTRY_PARAM entry_param = {0};

    if(NULL != url)
    {
        entry_param.type = MMIBROWSER_ACCESS_URL;
        entry_param.dual_sys = MMIMRAPP_GetActiveSim();
        entry_param.url_ptr = url;
        entry_param.user_agent_ptr = PNULL;
        
        MMIBROWSER_Entry(&entry_param);
    }
#endif
}

/*****************************************************************************/
// 	Description :取得网络ID。
//	Global resource dependence : none
//  Author: qgp
//	Note:飞行模式下返回MR_NET_ID_NONE
/*****************************************************************************/
int32 mr_getNetworkID(void)
{
	int32 resValue = MR_NET_ID_NONE;
	MN_IMSI_T imsi;
	char temp[10];	
	MMIMR_Trace(("mrapp. mr_getNetworkID()"));
	
	imsi = MNSIM_GetImsiEx(MMIMRAPP_GetActiveSim());
	MMIMRAPP_bcd2dec(imsi.imsi_val, (uint8*)temp, 3);
	temp[6] = '\0';
	MMIMR_Trace(("temp: %s", temp));	

#ifdef __MRAPP_OVERSEA__
	if (MMIAPISET_GetFlyMode())
	{
		resValue= MR_NET_ID_NONE;
	}
    else if(MMIPHONE_IsSimOk(MMIMRAPP_GetActiveSim()))
    {
        resValue= MR_NET_ID_OTHER;
    }
#else
	if (MMIAPISET_GetFlyMode())
	{
		resValue= MR_NET_ID_NONE; //modify by zack@20100628
	}
	else if(!strcmp(&temp[1], "46000") || !strcmp(&temp[1], "46002") || !strcmp(&temp[1], "46007"))//china-mobile
	{
		resValue = MR_NET_ID_MOBILE;
	}
	else if(!strcmp(&temp[1], "46001") || !strcmp(&temp[1], "46006"))//china-union
	{
		resValue = MR_NET_ID_CN;
	}
	else if(!strcmp(&temp[1], "46003") || !strcmp(&temp[1], "46005"))//cdma
	{
		resValue= MR_NET_ID_CDMA;
	}
	/*2009-10-22 111,17787 add. for overseas app*/
	else if(MMIPHONE_IsSimOk(MMIMRAPP_GetActiveSim()))//other sim
	{
		resValue= MR_NET_ID_OTHER;
	}
	/*2009-10-22 111,17787 add end*/
#endif

	MMIMR_Trace(("mrapp. mr_getNetworkID() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :返回手机相关信息。mr_userinfo说明如下：
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_getUserInfo(mr_userinfo* info)
{
	int32 resValue = MR_FAILED;
	MN_IMSI_T imsi;
	MN_IMEI_T imei;
	BOOLEAN ret;
	MMIMR_Trace(("mrapp. mr_getUserInfo()"));
	
	if(info != NULL)
	{
		memset(info, 0, sizeof(mr_userinfo));
		imsi = MNSIM_GetImsiEx(MMIMRAPP_GetActiveSim());
		MMIMRAPP_bcd2dec(imsi.imsi_val, (uint8*)info->IMSI, 8);
		SCI_MEMCPY(info->IMSI, &info->IMSI[1], 15);
		info->IMSI[15] = 0x00;
		ret = MNNV_GetIMEIEx(MMIMRAPP_GetActiveSim(), imei);
		if(!ret)
        {
			ret = MNNV_GetIMEIEx(MN_DUAL_SYS_1, imei);
#ifdef MULTI_SIM_SYS_DUAL
			if(!ret)
            {
				ret = MNNV_GetIMEIEx(MN_DUAL_SYS_2, imei);
			}
#endif
		}
		
		if(ret)
		{
			MMIMRAPP_bcd2dec(imei, (uint8*)info->IMEI, 8);
			SCI_MEMCPY(info->IMEI, &info->IMEI[1], 15);
			info->IMEI[15] = 0x00;
		}
		
		MMIMR_Trace(("IMSI: %s", info->IMSI));
		MMIMR_Trace(("IMEI: %s", info->IMEI));
		SCI_MEMCPY(info->manufactory, MRAPP_MANUFACTORY, sizeof(info->manufactory)-1);
		SCI_MEMCPY(info->type, MRAPP_HANDSET, sizeof(info->type)-1);
		MMIMR_Trace(("manufactory: %s, HANDSET: %s", info->manufactory, info->type));
		info->ver = (MRP_PLAT_VER%1000)*1000000 + (MRP_SUB_VER%100)*10000 + (MRP_PORT_VER%10000);
		MMIMR_Trace(("Ver: %d", info->ver));
		resValue = MR_SUCCESS;
	}
	
	MMIMR_Trace(("mrapp. mr_getUserInfo() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :激活GPRS连接
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode)
{
	int32 resValue = MR_FAILED;
	char *apn = (char*)(mode != NULL ? mode : "cmnet"); /*lint !e831*/
//2009-7-7 add begin
	uint8 *user_ptr = ""; /*lint !e64*/
	uint8 *passwd_ptr = ""; /*lint !e64*/
    uint32 dns = 0;
//2009-7-7 add end
    MMIPDP_ACTIVE_INFO_T app_info = {0};
	MMIMR_Trace(("mrapp. mr_initNetwork() state: %d, apn: %s useNvData: %d", s_mr_netContext.gprsState, apn, gDsmUseNVdataAccount));
	
#ifndef WIN32
#ifdef __MRAPP_WIFI_SUPPORT__
	if(s_mr_wifi_mode)
	{
		return MR_SUCCESS;
	}
#endif

	if(cb == NULL)
		return resValue;
	
	s_mr_netContext.mrNetActived = TRUE;
	s_mr_netContext.deactWaitFunc = NULL;
	s_mr_netContext.gprsCb = (void*)cb; /*lint !e611*/
	if(s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVED)
	{
		resValue = MR_SUCCESS;
	}
	else if(s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVEING)
	{
#ifdef __MRAPP_OVERSEA__
//2009-7-7 add begin
		if(gDsmUseNVdataAccount)
        {
			apn = (char*)gDsmDataAccount.apn;
			user_ptr = gDsmDataAccount.user_name;
			passwd_ptr = gDsmDataAccount.password;
#ifndef WIN32
            dns = (gDsmDataAccount.dns[0]<<24)|(gDsmDataAccount.dns[1]<<16)|(gDsmDataAccount.dns[2]<<8)|(gDsmDataAccount.dns[3]);
            sci_setsparedns(dns, 0);
#endif
    	}
        else if(mr_getNetworkID() == MR_NET_ID_OTHER)
        {//2009-11-10 111,17787 fix 
    		MN_IMSI_T imsi = {0};
            uint8  temp[10];	
    		T_DSM_APN *apnAccount = NULL;
    		imsi = MNSIM_GetImsiEx(MMIMRAPP_GetActiveSim());
	        MMIMRAPP_bcd2dec(imsi.imsi_val, (uint8*)temp, 4);
	        temp[7] = '\0';
    		apnAccount = mr_getAPN(&temp[1]);
    		if(apnAccount == NULL)
            {
                temp[6] = '\0';
        		apnAccount = mr_getAPN(&temp[1]);
                if(!apnAccount)
    			{
        			s_mr_netContext.gprsCb = NULL;
        			return MR_FAILED;
                }
                MMIMR_Trace(("apn2 plmn:%s apn:%s", &temp[1], apnAccount->apn));
    		}
            MMIMR_Trace(("apn1 plmn:%s apn:%s", &temp[1], apnAccount->apn));
    		apn = apnAccount->apn;
    		user_ptr = (uint8*)apnAccount->userName;
    		passwd_ptr = (uint8*)apnAccount->passWord;			
#ifndef WIN32
            dns = (apnAccount->dns[0]<<24)|(apnAccount->dns[1]<<16)|(apnAccount->dns[2]<<8)|(apnAccount->dns[3]);
            sci_setsparedns(dns, 0);
#endif			
		}
//2009-7-7 add end
#endif
		s_mr_netContext.subEvn = MRAPP_NETSUBEVN_WAITDAIL;
		s_mr_netContext.pdp_retry_times = 0;
	}
	else if(s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVED)
	{
		int32 ret;
		MMIMR_Trace(("cur sim: %d", MMIMRAPP_GetActiveSim()));

#ifdef __MRAPP_OVERSEA__
//2009-7-7 add begin
		if(gDsmUseNVdataAccount)
        {
			apn = (char*)gDsmDataAccount.apn;
			user_ptr = gDsmDataAccount.user_name;
			passwd_ptr = gDsmDataAccount.password;
#ifndef WIN32
            dns = (gDsmDataAccount.dns[0]<<24)|(gDsmDataAccount.dns[1]<<16)|(gDsmDataAccount.dns[2]<<8)|(gDsmDataAccount.dns[3]);
            sci_setsparedns(dns, 0);
#endif
    	}
        else if(mr_getNetworkID() == MR_NET_ID_OTHER)
        {//2009-11-10 111,17787 fix 
    		MN_IMSI_T imsi = {0};
            uint8  temp[10];	
    		T_DSM_APN *apnAccount = NULL;
    		imsi = MNSIM_GetImsiEx(MMIMRAPP_GetActiveSim());
	         MMIMRAPP_bcd2dec(imsi.imsi_val, (uint8*)temp, 4);
	        temp[7] = '\0';
    		apnAccount = mr_getAPN(&temp[1]);
    		if(apnAccount == NULL)
            {
                temp[6] = '\0';
        		apnAccount = mr_getAPN(&temp[1]);
                if(!apnAccount)
    			{
        			s_mr_netContext.gprsCb = NULL;
        			return MR_FAILED;
                }
                MMIMR_Trace(("apn2 plmn:%s apn:%s", &temp[1], apnAccount->apn));
    		}
            MMIMR_Trace(("apn1 plmn:%s apn:%s", &temp[1], apnAccount->apn));
    		apn = apnAccount->apn;
    		user_ptr = (uint8*)apnAccount->userName;
    		passwd_ptr = (uint8*)apnAccount->passWord;			
#ifndef WIN32
            dns = (apnAccount->dns[0]<<24)|(apnAccount->dns[1]<<16)|(apnAccount->dns[2]<<8)|(apnAccount->dns[3]);
            sci_setsparedns(dns, 0);
#endif			
		}
//2009-7-7 add end
#endif

        app_info.app_handler = MMI_MODULE_MRAPP;
        app_info.apn_ptr = (char*)apn;
        app_info.user_name_ptr = (char*)user_ptr;
        app_info.psw_ptr = (char*)passwd_ptr;
        app_info.dual_sys = MMIMRAPP_GetActiveSim();
        app_info.priority = 3;
        app_info.ps_service_rat = MN_UNSPECIFIED;
        app_info.handle_msg_callback = MMIMRAPP_handlePsMsg; /*lint !e64*/
        MNGPRS_ReadStorageExtReqQosEx(MMIMRAPP_GetActiveSim(),MN_GPRS_STORAGE_ALL,IM_E,&app_info.qos_info);

        ret = MMIAPIPDP_Active(&app_info);
	
		MMIMR_Trace(("mrapp apn:%s ret: %d", apn, ret));
		if(ret == TRUE)
		{
			s_mr_netContext.gprsState = MRAPP_GPRS_ACTIVING;
		}
		s_mr_netContext.pdp_retry_times = 0;
	}
	
	if(s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVING
		|| (s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVEING && s_mr_netContext.subEvn == MRAPP_NETSUBEVN_WAITDAIL))
	{
		memcpy(s_mr_netContext.apn, apn, sizeof(s_mr_netContext.apn)-1); /*lint !e670 !e831*/
		s_mr_netContext.apn[sizeof(s_mr_netContext.apn)-1] = '\0';
		MMIMRAPP_gprsProtectTimerStart(MMIMRAPP_GPRSPROTECT_TIME_OUT);
		resValue = MR_WAITING;
	}
#else
	resValue = MR_SUCCESS;
#endif
	
	MMIMR_Trace(("mrapp. mr_initNetwork() resValue: %d", resValue));
	return resValue;
}


/*****************************************************************************/
// 	Description :关闭拨号连接
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_closeNetwork(void)
{
	MMIMRAPP_CloseMrNet();
	return MR_SUCCESS;
}

/*wuwenjie 2011-12-01 START*/
/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: wuwenjie
//	Note:
/*****************************************************************************/
uint8 mr_getHostTimerId(void)
{
	return s_mr_hostContext.getHostTimer;
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: wuwenjie
//	Note:
/*****************************************************************************/
void mr_gethostTimerDeal(void)
{
	int ret = -1;
	int32 ip = 0;
	uint32 ipExt = 0 ;
	if(s_mr_hostContext.getHostTimer >0)
	MMK_StopTimer(s_mr_hostContext.getHostTimer);
	s_mr_hostContext.getHostTimer = 0;

	if(SCI_STRLEN(s_mr_hostContext.hostBuffer)&&s_mr_hostContext.cb)
	{
		s_mr_hostContext.timerCount ++;

#ifdef __MRAPP_WIFI_SUPPORT__
	    	if(s_mr_wifi_mode)
	    	{
	            //hostent = sci_gethostbyname_ext((char*)name, MMIAPIWIFI_GetNetId());
	            ret = sci_parse_host_ext((char *)s_mr_hostContext.hostBuffer,&ipExt,0,MMIAPIWIFI_GetNetId());
	        }
	        else
	        {
	    		//hostent = sci_gethostbyname((char*)name);
	    		ret = sci_parse_host_ext((char *)s_mr_hostContext.hostBuffer,&ipExt,0,s_mr_netContext.netid);
	        }
#else
	    	//hostent = sci_gethostbyname((char*)name);
    		ret = sci_parse_host_ext((char *)s_mr_hostContext.hostBuffer,&ipExt,0,s_mr_netContext.netid);
#endif
		MMIMR_Trace(("mrapp. mr_gethostTimerDeal ret: %d", ret));

		if(ret == 0)
		{
			ip = ipExt;
			ip = ntohl(ip); /*lint !e737*/
			s_mr_hostContext.cb(ip);
			s_mr_hostContext.cb = NULL;
		}
		else if((ret == 1)&&s_mr_hostContext.timerCount <30)
		{
			s_mr_hostContext.getHostTimer = MMK_CreateWinTimer(VIRTUAL_WIN_ID,1500 ,FALSE);
		}
		else
		{
			s_mr_hostContext.cb(MR_FAILED);
			s_mr_hostContext.cb = NULL;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_gethostTimerDeal ip: %d", ip));	
}

/*wuwenjie 2011-12-01 END*/
/*****************************************************************************/
// 	Description :通过主机名获得该主机的IP地址值，如果一个主机
//的IP地址为218.18.95.203，则值为218<<24 + 18<<16 + 95<<8 + 203 = 0xda125fcb。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_getHostByName(const char *name, MR_GET_HOST_CB cb)
{
	int32 resValue = MR_FAILED;
	struct sci_hostent * hostent = NULL;
	int32 hostAddr = 0;
	int32 ip = 0;
	uint32 ipExt = 0;
	
#ifndef WIN32
#ifdef __MRAPP_WIFI_SUPPORT__
	 if(s_mr_wifi_mode)
	 {
	   if(name== NULL)
		 return resValue;
	 }
	 else
#endif
	   if(name == NULL || s_mr_netContext.gprsState != MRAPP_GPRS_ACTIVED)
		return resValue;
	{
		MMIMR_Trace(("mrapp. mr_getHostByName() in name: %s", name));
		
#ifdef __MRAPP_WIFI_SUPPORT__
    	if(s_mr_wifi_mode)
    	{
            //hostent = sci_gethostbyname_ext((char*)name, MMIAPIWIFI_GetNetId());
            resValue = sci_parse_host_ext((char *)name,&ipExt,0,MMIAPIWIFI_GetNetId());
        }
        else
        {
    		//hostent = sci_gethostbyname((char*)name);
    		resValue = sci_parse_host_ext((char *)name,&ipExt,0,s_mr_netContext.netid);
        }
#else
    	//hostent = sci_gethostbyname((char*)name);
    	resValue = sci_parse_host_ext((char *)name,&ipExt,0,s_mr_netContext.netid);
#endif
	MMIMR_Trace(("mrapp. mr_getHostByName() resValue: %d", resValue));

	if(resValue ==0)
	{
		ip = ipExt;
		ip = ntohl(ip); /*lint !e737*/
		return ip;
	}
	else if(resValue ==1&&cb)
	{
		if(s_mr_hostContext.getHostTimer >0)
			MMK_StopTimer(s_mr_hostContext.getHostTimer);
		
		SCI_MEMSET(s_mr_hostContext.hostBuffer,0,sizeof(s_mr_hostContext.hostBuffer));
		strncpy(s_mr_hostContext.hostBuffer,name,sizeof(s_mr_hostContext.hostBuffer)-1);
		s_mr_hostContext.cb = cb;
		s_mr_hostContext.timerCount = 0;
		s_mr_hostContext.getHostTimer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, 1500,FALSE);
		return MR_WAITING;
	}
	else
	{
		return MR_FAILED;
	}
#if 0
		if(hostent != NULL)
		{
			SCI_MEMCPY(&hostAddr, hostent->h_addr_list[0], 4);
			resValue = hostAddr;
		}	
#endif

	}
#endif
	
	MMIMR_Trace(("mrapp. mr_getHostByName() out :0x%08x", resValue)); /*lint !e527*/	
	return resValue;
}

/*****************************************************************************/
// 	Description :创建一个socket。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_socket(int32 type, int32 protocol)
{
	int socket = INVALID_SOCKET;
	int32 e = 0;
	int32 sNode;
	MMIMR_Trace(("mrapp. mr_socket() type: %d", type));
	
	sNode = MMIMRAPP_getAfreeSnode();
	if(sNode >= 0)
	{
#ifndef WIN32
#ifdef __MRAPP_WIFI_SUPPORT__
	if(s_mr_wifi_mode)
	{
    	socket = sci_sock_socket(AF_INET, SOCK_STREAM, 0, MMIAPIWIFI_GetNetId());
	}
    else
    {
		socket = sci_sock_socket(AF_INET, SOCK_STREAM, 0, s_mr_netContext.netid);
    }
#else
    socket = sci_sock_socket(AF_INET, SOCK_STREAM, 0, s_mr_netContext.netid);
#endif

		MMIMR_Trace(("mrapp. socket:%d", socket));
		
		if(socket != INVALID_SOCKET)
		{
			sci_sock_setsockopt(socket, SO_NBIO, NULL);
			s_mr_sockets[sNode].s = socket;
			s_mr_sockets[sNode].state = MRAPP_SOCKET_OPENED;
		}
		else
		{ 
			e = sci_sock_errno(socket);
			MMIMR_Trace(("mrapp. errorno: %d", e)); 
			return MR_FAILED;
		}
#endif
	}
	MMIMR_Trace(("mrapp. mr_socket() snode: %d", sNode));
	return sNode;
}


/*****************************************************************************/
// 	Description :建立TCP连接。如果失败，可以调用jnet_errno()获取错误码。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_connect(int32 sNode, int32 ip, uint16 port, int32 type)
{
	int32 ret = 0;
	int32 error = 0;
	int32 socket = -1;
#ifndef WIN32
	MMIMR_Trace(("mrapp. mr_connect() sNode: %d, ip: 0x%x, port: %d, type: %d", sNode, ip, port, type));

#ifdef __MRAPP_WIFI_SUPPORT__
	 if(s_mr_wifi_mode)
	 {
	   if(sNode < 0)
		 return MR_FAILED;
	 }
	 else
#endif
	 {
	   if(sNode < 0 || s_mr_netContext.gprsState != MRAPP_GPRS_ACTIVED)
		return MR_FAILED;
	 }

	{
		if(sNode >= 0)
		{
			socket = s_mr_sockets[sNode].s;
			MMIMR_TraceN(("mrapp. socket:%d", socket));
			
			ret = MMIMRAPP_Sconnect(socket, ip, port);
			MMIMR_TraceN(("mrapp. ret:%d", ret));
			s_mr_sockets[sNode].ip = ip;
			s_mr_sockets[sNode].port = port;
			if(ret < 0)
			{
				error = sci_sock_errno(socket);
				MMIMR_TraceN(("error0:%d", error));
				if(EINPROGRESS == error)
				{
					if(MR_SOCKET_NONBLOCK == type)
					{
						s_mr_sockets[sNode].state = MRAPP_SOCKET_CONNECTING;
						return MR_WAITING;
					}
					else
					{
						s_mr_sockets[sNode].state = MRAPP_SOCKET_CONNECTING;
						return MR_SUCCESS;//allways show apps a SUCCESS result.
					}
				}
			}
			else
			{
				s_mr_sockets[sNode].state = MRAPP_SOCKET_CONNECTED;
				return MR_SUCCESS;
			}
		}
	}
	error = sci_sock_errno((long)socket);
#endif
	
	MMIMR_Trace(("mrapp. mr_connect() failed: %d", error));
	return MR_FAILED;
}


/*****************************************************************************/
// 	Description :关闭一个socket连接。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_closeSocket(int32 sNode)
{
	int32 resValue = MR_FAILED;
	int ret = 0;
	int32 socket;
	MMIMR_Trace(("mrapp. mr_closeSocket() sNode: %d", sNode));
	
#ifndef WIN32
	if(sNode >= 0)
	{
		socket = s_mr_sockets[sNode].s;
		MMIMR_Trace(("mrapp. mr_closeSocket() socket: %d", socket));
		
		if(socket != -1)
		{
			ret = sci_sock_socketclose((long)socket);
			MMIMR_Trace(("mrapp. mr_closeSocket() ret:%d ", ret));
			if(ret < 0)
			{
				MMIMR_Trace(("mrapp. error:%d", sci_sock_errno((long)socket)));
				s_mr_sockets[sNode].state = MRAPP_SOCKET_ERROR;
			}
			else
			{
				s_mr_sockets[sNode].state = MRAPP_SOCKET_CLOSED;
				s_mr_sockets[sNode].s = INVALID_SOCKET;
				resValue = MR_SUCCESS;
			}
		}
	}
#endif
	
	MMIMR_Trace(("mrapp. mr_closeSocket() resValue: %d", resValue));
	return resValue;
}


/*****************************************************************************/
// 	Description :从网络接收数据。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_recv (int32 sNode, char *buf, int len)
{
	int32 resValue = MR_FAILED;
	int ret, e;
	int32 socket;
	MMIMR_TraceN(("mrapp. mr_recv()  sNode: %d, len:%d", sNode, len));
	
#ifndef WIN32
	if(sNode >= 0 && buf != PNULL && len != 0)
	{
		socket = s_mr_sockets[sNode].s;
		if(socket != -1)
		{
			if(s_mr_sockets[sNode].state == MRAPP_SOCKET_CONNECTED)
			{
				ret = sci_sock_recv((long)socket, buf, len, 0);
				MMIMR_TraceM(("mrapp. recv ret:%d", ret));
				if(ret > 0)
				{
					MMIMR_TraceN(("mrapp. recved some things !!:%d", ret));
					resValue = ret;
				}
				else if(ret == 0)//对端已关闭连接
				{
					resValue = MR_FAILED;
				}
				else
				{
					e = sci_sock_errno(socket);
					MMIMR_TraceM(("mrapp. errorno:%d", e));
					
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
			else if(s_mr_sockets[sNode].state == MRAPP_SOCKET_CONNECTING)
			{
				ret = MMIMRAPP_Sconnect(socket, s_mr_sockets[sNode].ip, s_mr_sockets[sNode].port);
				MMIMR_TraceN(("ret:%d", ret));
				if(ret < 0)
				{
					e = sci_sock_errno(socket);
					MMIMR_TraceM(("mrapp. errorno:%d", e));
					if(EINPROGRESS == e)
					{
						resValue = 0;
					}
				}
				else
				{
					s_mr_sockets[sNode].state = MRAPP_SOCKET_CONNECTED;
					resValue = 0;
				}
			}
		}
	}
#endif
	
	MMIMR_TraceN(("mrapp. mr_recv() resValue: %d", resValue));
	return resValue;
}


/*****************************************************************************/
// 	Description :发送数据。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_send(int32 sNode, const char *buf, int len)
{
	int32 resValue = MR_FAILED;
	int32 e = 0;
	int ret = 0;
	int32 socket;
	MMIMR_TraceN(("mrapp. mr_send() sNode: %d, len: %d", sNode, len));
	
#ifndef WIN32
#ifdef __MRAPP_WIFI_SUPPORT__
	if(sNode >= 0 && buf != PNULL && len != 0)
#else
	if(s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVED && sNode >= 0 && buf != PNULL && len != 0)
#endif
	{
		socket = s_mr_sockets[sNode].s;
		if(socket != -1)
		{
			if(s_mr_sockets[sNode].state == MRAPP_SOCKET_CONNECTED)
			{
				ret = sci_sock_send((long)socket, (char*)buf, len, 0);
				
				MMIMR_TraceM(("mrapp. send ret: %d", ret));
				if(ret < 0)
				{
					e = sci_sock_errno((int32)socket);
					MMIMR_TraceM(("mrapp. error0: %d", e)); 
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
			else if(s_mr_sockets[sNode].state == MRAPP_SOCKET_CONNECTING)
			{
				ret = MMIMRAPP_Sconnect(socket, s_mr_sockets[sNode].ip, s_mr_sockets[sNode].port);
				MMIMR_TraceN(("ret:%d", ret));
				if(ret < 0)
				{
					int error;
					error = sci_sock_errno(socket);
					MMIMR_TraceN(("error0:%d", error));
					if(EINPROGRESS == error)
					{
						resValue = 0;
					}
				}
				else
				{
					s_mr_sockets[sNode].state = MRAPP_SOCKET_CONNECTED;
					resValue = 0;
				}
			}
		}
	}
#endif
	
	MMIMR_TraceN(("mrapp. mr_send() resValue: %d", resValue));
	return resValue;
}


/*****************************************************************************/
// 	Description :从指定地址接收数据
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
int32 mr_recvfrom(int32 sNode, char *buf, int len, int32 *ip, uint16 *port)
{
	int32 resValue = MR_FAILED;
	int32 socket;
#ifndef WIN32
	int ret, e;
	struct sci_sockaddr to;
	MMIMR_Trace(("mr_recvfrom()"));
	
	if(sNode >= 0 && buf != PNULL && ip != PNULL && port != NULL)
	{
		socket = s_mr_sockets[sNode].s;
		to.family = AF_INET;
		
		ret = sci_sock_recvfrom(socket, (char*)buf, len, 0, &to);
		MMIMR_Trace(("mrapp. recv len:%d", ret));
		if(ret > 0)
		{
			MMIMR_Trace(("mrapp. recved some things !!:%d", ret));
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
			e = sci_sock_errno(socket);
			MMIMR_Trace(("mrapp. errorno:%d", e));
			
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

/*****************************************************************************/
// 	Description :向指定地址发送数据
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
int32 mr_sendto(int32 sNode, const char *buf, int len, int32 ip, uint16 port)
{
	int32 resValue = MR_FAILED;
	int32 socket;
#ifndef WIN32
	int ret, e;
	struct sci_sockaddr to;
	MMIMR_Trace(("mrapp.mr_sendto()"));
	
	if(sNode >= 0 && buf != PNULL && len != 0)
	{
		socket = s_mr_sockets[sNode].s;
		to.family = AF_INET;
		to.port = htons(port);
		to.ip_addr = (unsigned long)htonl(ip); /*lint !e737*/
		SCI_MEMSET((void*)to.sa_data, 0, 8*sizeof(char));
		ret = sci_sock_sendto(socket, (char*)buf, len, 0, &to);
		if(ret < 0)
		{
			e = sci_sock_errno((int32)socket);
			MMIMR_Trace(("mrapp. error0: %d", e));	
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

/*****************************************************************************/
// 	Description :创建一个菜单，并返回菜单句柄，title是菜单的标题。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_menuCreate(const char * title, int16 num)
{
	return MMIMRAPP_listCreate(title, num, MR_LIST_NORMAL_ICON);
}

/*****************************************************************************/
// 	Description :在菜单里添加一个菜单项，参数index是上层对该
//菜单项的标示，当用户选中某个菜单项时，把index号通过
//mr_event函数发送到DSM中。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_menuSetItem(int32 menu, const char *text, int32 index)
{
	return MMIMRAPP_menuSetItem(menu, text, index, 0);
}

/*****************************************************************************/
// 	Description :显示菜单。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_menuShow(int32 menu)
{
	int32 resValue = MR_FAILED;
	MMIMRAPP_ADD_PARAMETERS_T* addData;
	MMIMR_Trace(("mrapp. mr_menuShow() menu: %d", menu));
	
	addData = (MMIMRAPP_ADD_PARAMETERS_T*)MMK_GetWinAddDataPtr(menu);
	if(addData != NULL)
	{
		MMK_SetAtvCtrl(addData->winId, addData->ctrlId);
		resValue = MR_SUCCESS;
	}
	MMIMR_Trace(("mrapp. mr_menuShow() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :刷新菜单显示。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_menuRefresh(int32 menu)
{
	int32 resValue = MR_FAILED;
	MMIMRAPP_ADD_PARAMETERS_T* addData;
	MMIMR_Trace(("mrapp. mr_menuRefresh() menu: %d", menu));

	addData = (MMIMRAPP_ADD_PARAMETERS_T*)MMK_GetWinAddDataPtr(menu);
	if(addData != NULL)
	{
        if(MMK_IsFocusWin(addData->winId))
        {   
    		MMK_SetAtvCtrl(addData->winId, addData->ctrlId);
    		if(MMK_SendMsg(addData->ctrlId, MSG_CTL_PAINT, PNULL))
    		{
    			resValue = MR_SUCCESS;
    		}
    	}
	}

	MMIMR_Trace(("mrapp. mr_menuRefresh() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :释放菜单。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_menuRelease(int32 menu)
{
	int32 resValue = MR_FAILED;
	MMIMR_Trace(("mrapp. mr_menuRelease() menu: %d", menu));
	
	if(MMK_IsOpenWin((MMI_WIN_ID_T)menu))
	{
		if(MMK_CloseWin((MMI_WIN_ID_T)menu))
		{
			resValue = MR_SUCCESS;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_menuRelease() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :创建一个对话框，并返回对话框句柄。
//	Global resource dependence : none
//  Author: qgp
//	Note:  -忽略-title
/*****************************************************************************/
int32 mr_dialogCreate(const char * title, const char * text, int32 type)
{
	int32 resValue = MR_FAILED;
	MMI_STRING_T textInfo;
	MMIMRAPP_DIALOG_PARAMETERS_T *dialogNode = NULL;
	char *temp_title = NULL;
	char *temp_text = NULL;
	int32 len_title = 0;
	int32 len_text = 0;
	MMIMR_Trace(("mrapp. mr_dialogCreate() type: %d", type));

#ifndef WIN32
	if(title != NULL)
	{
		len_title = mr_str_wstrlen(title) + 2;
		temp_title = (char*)SCI_ALLOCA(len_title);
    	if(PNULL == temp_title)
    	{
        	return resValue;
    	}
    	SCI_MEMCPY(temp_title, title, len_title);
    	mr_str_convert_endian((char*)temp_title);
	}

	if(text != NULL)
	{
		len_text = mr_str_wstrlen(text) + 2;
		temp_text = (char*)SCI_ALLOCA(len_text);
    	if(PNULL == temp_text)
    	{
       		SCI_FREE(temp_title);
       		return resValue;
    	}
    	SCI_MEMCPY(temp_text, text, len_text);
    	mr_str_convert_endian((char*)temp_text);
	}
#endif
	
	dialogNode = (MMIMRAPP_DIALOG_PARAMETERS_T *)SCI_ALLOC(sizeof(MMIMRAPP_DIALOG_PARAMETERS_T));
	if(dialogNode != NULL)
	{		
		dialogNode->next = NULL;
		dialogNode->dialog.type = type;
		textInfo.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_text)/2;
		textInfo.wstr_ptr = (uint8*)temp_text; /*lint !e64*/
		
		resValue = MMIMRAPP_createPromptWin(dialogNode, &textInfo);
		if(resValue < 0)
		{
			SCI_FREE(dialogNode);
			resValue = MR_FAILED;
		}
	}	

	if((temp_title != NULL) && (temp_text != NULL))
	{
		SCI_FREE(temp_title);
		SCI_FREE(temp_text);
	}
	MMIMR_Trace(("mrapp. mr_dialogCreate() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description : 刷新对话框
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_dialogRefresh(int32 dialog, const char * title, const char * text, int32 type)
{
	int32 resValue = MR_FAILED;
	MMI_STRING_T   	textInfo;
	MMIMRAPP_DIALOG_PARAMETERS_T *addData = (MMIMRAPP_DIALOG_PARAMETERS_T*)MMIMRAPP_GetPromptWinAddDataPtr((MMI_WIN_ID_T)dialog);	
	char *temp_title = NULL;
	char *temp_text = NULL;
	int32 len_title = 0;
	int32 len_text = 0;
	MMIMR_Trace(("mrapp. mr_dialogRefresh() dialog: %d", dialog));

#ifndef WIN32
	if(title != NULL)
	{
		len_title = mr_str_wstrlen(title) + 2;
		temp_title = (char*)SCI_ALLOCA(len_title);
		if(PNULL == temp_title)
		{
		    return resValue;
		}
		SCI_MEMCPY(temp_title, title, len_title);
		mr_str_convert_endian((char*)temp_title);
	}

	if(text != NULL)
	{
		len_text = mr_str_wstrlen(text) + 2;
		temp_text = (char*)SCI_ALLOCA(len_text);
    	if(PNULL == temp_text)
    	{
        	SCI_FREE(temp_title);
        	return resValue;
    	}
    	SCI_MEMCPY(temp_text, text, len_text);
    	mr_str_convert_endian((char*)temp_text);
	}
#endif
	
	if(addData != NULL)
	{
		if(MMK_IsOpenWin(addData->winId))
		{
			textInfo.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_text)/2;
			textInfo.wstr_ptr = (uint8*)temp_text; /*lint !e64*/
			
			if(type != -1)
			{
				addData->dialog.type = type;
			}
			//MMIPUB_SetAlertWinTextByPtrEx(addData->winId, &textInfo, TRUE);
			MMIPUB_SetAlertWinTextByPtr(addData->winId, &textInfo, NULL, TRUE);
			MMK_SendMsg(addData->winId, MSG_FULL_PAINT, PNULL);//刷新全部内容
			resValue = MR_SUCCESS;
		}
	}

	if((temp_title != NULL) && (temp_text != NULL))
	{
		SCI_FREE(temp_title);
		SCI_FREE(temp_text);
	}
	MMIMR_Trace(("mrapp. mr_dialogRefresh() resValue: %d", resValue));
	return resValue;
}


/*****************************************************************************/
// 	Description :释放对话框。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_dialogRelease(int32 dialog)
{
	int32 resValue = MR_FAILED;
	MMIMR_Trace(("mrapp. mr_dialogRelease() dialog: %d", dialog));
	
	if(MMK_IsOpenWin((MMI_WIN_ID_T)dialog))
	{
		if(MMK_CloseWin((MMI_WIN_ID_T)dialog))
		{
			resValue = MR_SUCCESS;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_dialogRelease() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :创建一个文本框，并返回文本框句柄。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_textCreate(const char * title, const char * text, int32 type)
{
	int32 resValue = MR_FAILED;
	MMIMRAPP_ADD_PARAMETERS_T *addData = NULL;
	MRAPP_TEXT_DATA *textPtr;
	MMI_STRING_T textInfo = {0};
	char *temp_title = NULL;
	char *temp_text = NULL;
	int32 len_title = 0;
	int32 len_text = 0;
	MMIMR_Trace(("mrapp. mr_textCreate() type: %d", type));

#ifndef WIN32
	if(title != NULL)
	{
		len_title = mr_str_wstrlen(title) + 2;
		temp_title = (char*)SCI_ALLOCA(len_title);
		if(PNULL == temp_title)
		{
		    return resValue;
		}
		SCI_MEMCPY(temp_title, title, len_title);
		mr_str_convert_endian((char*)temp_title);
	}

	if(text != NULL)
	{
		len_text = mr_str_wstrlen(text) + 2;
		temp_text = (char*)SCI_ALLOCA(len_text);
		if(PNULL == temp_text)
	    {
	        SCI_FREE(temp_title);
	        return resValue;
	    }
		SCI_MEMCPY(temp_text, text, len_text);
		mr_str_convert_endian((char*)temp_text);
	}
#endif

	addData = (MMIMRAPP_ADD_PARAMETERS_T *)SCI_ALLOC(sizeof(MMIMRAPP_ADD_PARAMETERS_T));
	
	if(addData != NULL)
	{
		addData->dynWinType = MRAPP_WINTYPE_TEXT;
		textPtr = &addData->u.text;
		textPtr->type = type;
		textPtr->title.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_title)/2;
		if(textPtr->title.wstr_len != 0)
		{
			textPtr->title.wstr_ptr = (wchar*)SCI_ALLOC(textPtr->title.wstr_len*2);
			if(textPtr->title.wstr_ptr != NULL)
			{
				SCI_MEMCPY(textPtr->title.wstr_ptr, temp_title, textPtr->title.wstr_len*2);
			}
			else
			{
				textPtr->title.wstr_len = 0;
			}
		}
		else
		{
			textPtr->title.wstr_ptr = NULL;
		}

        textPtr->textInfo.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_text)/2;
		if(textPtr->textInfo.wstr_len != 0)
		{
			textPtr->textInfo.wstr_ptr = (wchar*)SCI_ALLOC(textPtr->textInfo.wstr_len*2);
			if(textPtr->textInfo.wstr_ptr != NULL)
			{
				SCI_MEMCPY(textPtr->textInfo.wstr_ptr, temp_text, textPtr->textInfo.wstr_len*2);
			}
			else
			{
				textPtr->textInfo.wstr_len = 0;
			}
		}
		else
		{
			textPtr->textInfo.wstr_ptr = NULL;
		}
		
		resValue = MMIMRAPP_createADynWin(addData, &textInfo);
		if(resValue < 0)
		{
			if(resValue == -2)
			{
				if(textPtr->title.wstr_ptr != NULL)
				{
					SCI_FREE(textPtr->title.wstr_ptr);
                    textPtr->title.wstr_len = 0;
				}

				if(textPtr->textInfo.wstr_ptr != NULL)
				{
					SCI_FREE(textPtr->textInfo.wstr_ptr);
                    textPtr->textInfo.wstr_len = 0;
				}
				
				SCI_FREE(addData);
			}
			resValue = MR_FAILED;
		}
	}

	if((temp_title != NULL) && (temp_text != NULL))
	{
		SCI_FREE(temp_title);
		SCI_FREE(temp_text);
	}
	MMIMR_Trace(("mrapp. mr_textCreate() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description : 刷新文本框
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_textRefresh(int32 handle, const char * title, const char * text)
{
	int32 resValue = MR_FAILED;
	MMI_STRING_T   	textInfo, *titlePtr;
	MMIMRAPP_ADD_PARAMETERS_T* addData = (MMIMRAPP_ADD_PARAMETERS_T*)MMK_GetWinAddDataPtr(handle);
	MRAPP_TEXT_DATA *textPtr;
	uint16 len;
	char *temp_title = NULL;
	char *temp_text = NULL;
	int32 len_title = 0;
	int32 len_text = 0;
	MMIMR_Trace(("mrapp. mr_textRefresh() win: %d", handle));

#ifndef WIN32
	if(title != NULL)
	{
		len_title = mr_str_wstrlen(title) + 2;
		temp_title = (char*)SCI_ALLOCA(len_title);
		if(PNULL == temp_title)
	    {
	        return resValue;
	    }
		SCI_MEMCPY(temp_title, title, len_title);
		mr_str_convert_endian((char*)temp_title);
	}

	if(text != NULL)
	{
		len_text = mr_str_wstrlen(text) + 2;
		temp_text = (char*)SCI_ALLOCA(len_text);
		if(PNULL == temp_text)
	    {
	        SCI_FREE(temp_title);
	        return resValue;
	    }
		SCI_MEMCPY(temp_text, text, len_text);
		mr_str_convert_endian((char*)temp_text);
	}
#endif

	if(addData != NULL)
	{
		textPtr = &addData->u.text;
		titlePtr = &addData->u.text.title;
		len = MMIMRAPP_getUcstrLen((uint8 *)temp_title)/2;
		if(len > titlePtr->wstr_len)
		{
			SCI_FREE(titlePtr->wstr_ptr);
			titlePtr->wstr_ptr = (wchar*)SCI_ALLOC(len*2);
		}
		titlePtr->wstr_len = len;
		SCI_MEMCPY(titlePtr->wstr_ptr, temp_title, len*2);
		
		textInfo.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_text)/2;
		textInfo.wstr_ptr = (wchar*)temp_text;
		GUITEXT_SetString(addData->ctrlId, textInfo.wstr_ptr, textInfo.wstr_len, TRUE);
		
		if(MMK_SendMsg(addData->winId, MSG_FULL_PAINT, PNULL) == TRUE)
		{
			resValue = MR_SUCCESS;
		}
		
	}

	if((temp_title != NULL) && (temp_text != NULL))
	{
		SCI_FREE(temp_title);
		SCI_FREE(temp_text);
	}
	MMIMR_Trace(("mrapp. mr_textRefresh() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :释放文本框。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_textRelease(int32 text)
{	
	int32 resValue = MR_FAILED;
	MMIMR_Trace(("mrapp. mr_textRelease() text: %d", text));
	
	if(MMK_IsOpenWin((MMI_WIN_ID_T)text))
	{
		if(MMK_CloseWin((MMI_WIN_ID_T)text))
		{
			resValue = MR_SUCCESS;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_textRelease() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :创建一个编辑框，并返回编辑框句柄。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_editCreate(const char * title, const char * text, int32 type, int32 max_size)
{
	int32 resValue = MR_FAILED;
	MMIMRAPP_ADD_PARAMETERS_T *addData = NULL;
	MRAPP_EDIT_DATA *editPtr;
	MMI_STRING_T textInfo;
	char *temp_title = NULL;
	char *temp_text = NULL;
	int32 len_title = 0;
	int32 len_text = 0;
	MMIMR_Trace(("mrapp. mr_editCreate() type: %d, max_size: %d", type, max_size));

#ifndef WIN32
	if(title != NULL)
	{
		len_title = mr_str_wstrlen(title) + 2;
		temp_title = (char*)SCI_ALLOCA(len_title);
		if(PNULL == temp_title)
    	{
        	return resValue;
    	}
		SCI_MEMCPY(temp_title, title, len_title);
		mr_str_convert_endian((char*)temp_title);
	}

	if(text != NULL)
	{
		len_text = mr_str_wstrlen(text) + 2;
		temp_text = (char*)SCI_ALLOCA(len_text);
		if(PNULL == temp_text)
    	{
        	SCI_FREE(temp_title);
        	return resValue;
    	}
		SCI_MEMCPY(temp_text, text, len_text);
		mr_str_convert_endian((char*)temp_text);
	}
#endif
	
	addData = (MMIMRAPP_ADD_PARAMETERS_T *)SCI_ALLOC(sizeof(MMIMRAPP_ADD_PARAMETERS_T));
	if(addData != NULL)
	{
		addData->dynWinType = MRAPP_WINTYPE_EDITOR;
		editPtr = &addData->u.edit;
		textInfo.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_text)/2;
		textInfo.wstr_ptr = (wchar*)temp_text;
		editPtr->textOut.wstr_ptr = NULL;
		editPtr->textOut.wstr_len = 0;
		editPtr->title.wstr_len = MMIMRAPP_getUcstrLen((uint8 *)temp_title)/2;
		if(editPtr->title.wstr_len != 0)
		{
			editPtr->title.wstr_ptr = (wchar*)SCI_ALLOC(editPtr->title.wstr_len*2);
			if(editPtr->title.wstr_ptr != NULL)
			{
				SCI_MEMCPY(editPtr->title.wstr_ptr, temp_title, editPtr->title.wstr_len*2);
			}
			else
			{
				editPtr->title.wstr_len = 0;
			}
		}
		else
		{
			editPtr->title.wstr_ptr = NULL;
		}
		editPtr->type = type;
		editPtr->maxLen = (uint16)max_size;		
		resValue = MMIMRAPP_createADynWin(addData, &textInfo);
		if(resValue < 0)
		{
			if(resValue == -2)
			{
				if(editPtr->title.wstr_ptr != NULL)
				{
					SCI_FREE(editPtr->title.wstr_ptr);
				}
				
				SCI_FREE(addData);
			}
			resValue = MR_FAILED;
		}
	}

	if((temp_title != NULL) && (temp_text != NULL))
	{
		SCI_FREE(temp_title);
		SCI_FREE(temp_text);
	}
	MMIMR_Trace(("mrapp. mr_editCreate() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :获取编辑框内容，unicode编码。调用该函数后，DSM
//平台会在平台内部建立一个内容的备份，调用者不需保证在
//编辑框释放后内容仍然有效。该函数会在编辑框释放之前调用。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
const char* mr_editGetText(int32 edit)
{
	MMIMRAPP_ADD_PARAMETERS_T* addData = (MMIMRAPP_ADD_PARAMETERS_T*)MMK_GetWinAddDataPtr(edit);
	MMI_STRING_T string_info;
	MRAPP_EDIT_DATA *editPtr; /*lint !e831*/
	uint8* ret = NULL;
	int32 lenth = 0;

	MMIMR_Trace(("mrapp. mr_editGetText() edit: %d", edit));
	
	if(addData != NULL)
	{
		editPtr = &addData->u.edit;
		if(editPtr->type == MR_EDIT_NUMERIC)
		{
			GUIEDIT_GetPhoneNumString(addData->ctrlId, &string_info);
		}
		else
		{
			GUIEDIT_GetString(addData->ctrlId, &string_info);
		}
		
		if(editPtr->textOut.wstr_ptr != NULL)
		{
			SCI_FREE(editPtr->textOut.wstr_ptr);
		}

		editPtr->textOut.wstr_len = string_info.wstr_len;
		editPtr->textOut.wstr_ptr = SCI_ALLOC((string_info.wstr_len+1)*2);
		if(editPtr->textOut.wstr_ptr != NULL)
		{
			SCI_MEMCPY(editPtr->textOut.wstr_ptr, string_info.wstr_ptr, string_info.wstr_len*2);
			editPtr->textOut.wstr_ptr[string_info.wstr_len] = 0x0000;
		}	
#ifndef WIN32
		if(string_info.wstr_ptr != NULL)
		{
			mr_str_convert_endian((char*) editPtr->textOut.wstr_ptr);
		}
#endif
		ret = (uint8*)editPtr->textOut.wstr_ptr;
	}
	MMIMR_Trace(("mrapp. mr_editGetText() ucPtr: 0x%x", editPtr->textOut.wstr_ptr)); /*lint !e613 !e644*/
	return (const char*)ret;
}


/*****************************************************************************/
// 	Description :释放编辑框。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_editRelease(int32 edit)
{
	int32 resValue = MR_FAILED;
	MMIMR_Trace(("mrapp. mr_editRelease() edit: %d", edit));
	
	if(MMK_IsOpenWin((MMI_WIN_ID_T)edit))
	{
		if(MMK_CloseWin((MMI_WIN_ID_T)edit))
		{
			resValue = MR_SUCCESS;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_editRelease() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :创建一个可扩展窗体，并返回可扩展窗体句柄。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_winCreate(void)
{
	int32 resValue = MR_FAILED;
	MMIMRAPP_ADD_PARAMETERS_T *addData = NULL;
	MMIMR_Trace(("mrapp. mr_winCreate()"));
	
	addData = (MMIMRAPP_ADD_PARAMETERS_T *)SCI_ALLOC(sizeof(MMIMRAPP_ADD_PARAMETERS_T));
	if(addData != NULL)
	{
		addData->dynWinType = MRAPP_WINTYPE_APP;		
		resValue = MMIMRAPP_createADynWin(addData, NULL); /*lint !e831*/
		if(resValue < 0)
		{
			if(resValue == -2)
			{
				SCI_FREE(addData);
			}
			resValue = MR_FAILED;
		}
	}
	
	MMIMR_Trace(("mrapp. mr_winCreate() resValue: %d", resValue));
	return resValue;
}


/*****************************************************************************/
// 	Description :释放可扩展窗体。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_winRelease(int32 win)
{
	int32 resValue = MR_FAILED;
	MMIMR_Trace(("mrapp. mr_winRelease() win: %d", win));
	
	if(MMK_IsOpenWin((MMI_WIN_ID_T)win))
	{	
		if(MMK_CloseWin((MMI_WIN_ID_T)win))
		{
			resValue = MR_SUCCESS;
		}
	}
	
	return resValue;
} 

/* begin:added by Tommy.yan 20120117 */
/*****************************************************************************/
// 	Description :获取字符串长度。
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
int32 mr_str_wstrlen(const char * str)
{
	int32 lenth = 0;
	unsigned char *tmp = (unsigned char*)str;

	if(str == NULL)
	{
		return 0;
	}
	
	while(tmp[0] | tmp[1])
	{
		lenth += 2;
		tmp += 2;
	}
	return lenth;
}

/*****************************************************************************/
// 	Description :大小端转换。
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
char* mr_str_convert_endian(char * str)
{
	char temp;
	int32 i=0;
	int32 len = 0;

	if(str == NULL)
	{
		return NULL;
	}
	
	len = mr_str_wstrlen(str);
	for(i = 0; i < len; i += 2)
	{
		temp = str[i];
		str[i] = str[i+1];
		str[i+1] = temp;
	}
	return str;
}

/*****************************************************************************/
// 	Description :大小端转换(防止传入的字符串没有结束符，导致转换异常)。
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
char* mr_str_convert_endian_with_size(char * str, int32 size)
{
	char temp;
	int32 i = 0;
	int len = 0;

	if(str == NULL)
	{
		return NULL;
	}

	len = mr_str_wstrlen(str);
	len = (len > size) ? size : len;

	for(i = 0; i < len; i += 2)
	{
		temp = str[i];
		str[i] = str[i+1];
		str[i+1] = temp;
	}
	return str;
}
/* end:added by Tommy.yan 20120117 */

/*****************************************************************************/
// 	Description :调整cpu频率
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_adaptCpuClk(int32 param)
{
#ifdef MRAPP_SUPPORT_CLKADAPT
	MMIMR_Trace(("cur: %d", HAL_GetMCUClk()));
	switch(param)
	{  
	case 0:
	default:
		MMIMRAPP_RestoreARMClk();
		MMIMRAPP_CLKSetted(FALSE, FREQ_INDEX_APP_BOOT, HAL_GetMCUClk());
		
		break;
	case 1:
	case 2:
	case 3:
		MMIMRAPP_SetArmClkHigh();
		MMIMRAPP_CLKSetted(TRUE, FREQ_INDEX_APP_HIGH, HAL_GetMCUClk());
		break;
	}
#endif
	return MR_SUCCESS;
}

#ifdef MRAPP_SIMUGAME_OPEN
/*****************************************************************************/
// 	Description :nes模拟器启动、退出通知
//	Global resource dependence : none
//  Author: qgp
//	Note:nes启动需要申请iram及保护其他可能产生冲突的平台资源
/*****************************************************************************/
LOCAL int32 MMIMRAPP_nesActionInd(MRAPP_NES_ACTION action)
{
	int32 resValue = MR_SUCCESS;
	
#ifndef WIN32
	if(action == NES_LOCK_IRAM)//启动	rom
	{
		MMIMRAPP_pauseMp3();
		MMIAPISET_StopAllRing(TRUE);
		MMIAPISET_StopRing(MMISET_RING_TYPE_KEY);  //停止按键音
#ifdef MRAPP_NES_USE_IRAM
		*(volatile uint32 *)0x8b00002c |= 0x01;//2009-11-10 111,17787 L add
		s_mrapp_ilock_handle = IRAM_LockArea((const IRAM_AREA *)&(s_mrapp_iram_area), 
												sizeof(s_mrapp_iram_area)/sizeof(IRAM_AREA), 
												IRAM_WAIT_FOR_EVER);
	
		if(IRAM_INVALID_HANDLE != s_mrapp_ilock_handle)//申请IRAM成功
#endif
		{
			MMIDEFAULT_AllowTurnOffBackLight(FALSE); 
			MMIDEFAULT_SetBackLight(TRUE);
			s_mr_envLock_state |= ENVLOCK_IRAM;
		}
#ifdef MRAPP_NES_USE_IRAM
		else
		{
			MMIMRAPP_resumeMp3();
			resValue = MR_FAILED;//申请资源失败，通知NES
		}
#endif
	}
	else if(action == NES_UNLOCK_IRAM)
	{
#ifdef MRAPP_NES_USE_IRAM
		//释放IRAM
		if(IRAM_INVALID_HANDLE != s_mrapp_ilock_handle)
		{
			IRAM_UnlockArea (s_mrapp_ilock_handle);
			s_mrapp_ilock_handle = IRAM_INVALID_HANDLE;
			*(volatile uint32 *)0x8b00002c &= ~(0x01);//2009-11-10 111,17787 L add
		}
#endif
		MMIDEFAULT_AllowTurnOffBackLight(TRUE);
		MMIDEFAULT_StartLcdBackLightTimer();
		MMIMRAPP_resumeMp3();
		s_mr_envLock_state &= (~ENVLOCK_IRAM);
	}
	else if(action == NES_LOCK_OTHERS)
	{
		/*将短消息等提示暂设为震动*/
		s_mr_env_preVal.msg[0] = MMIAPIENVSET_GetActiveModeOptValue(MN_DUAL_SYS_1, MSG_RING_TYPE);
		s_mr_env_preVal.msg[1] = MMIAPIENVSET_GetActiveModeOptValue(MN_DUAL_SYS_2, MSG_RING_TYPE);
		s_mr_env_preVal.voltage[0] = MMIAPIENVSET_GetActiveModeOptValue(MN_DUAL_SYS_1, VOLTAGE_WARN);
		s_mr_env_preVal.voltage[1] = MMIAPIENVSET_GetActiveModeOptValue(MN_DUAL_SYS_2, VOLTAGE_WARN);
		s_mr_env_preVal.flip[0] = MMIAPIENVSET_GetActiveModeOptValue(MN_DUAL_SYS_1, FLIP_RING_TYPE);
		s_mr_env_preVal.flip[1] = MMIAPIENVSET_GetActiveModeOptValue(MN_DUAL_SYS_2, FLIP_RING_TYPE);
		MMIENVSET_SetActiveModeOptValue(MSG_RING_TYPE, (uint8)MSG_RING_TYPE_VIBRATE, MN_DUAL_SYS_1);
		MMIENVSET_SetActiveModeOptValue(MSG_RING_TYPE, (uint8)MSG_RING_TYPE_VIBRATE, MN_DUAL_SYS_2);
		MMIENVSET_SetActiveModeOptValue(VOLTAGE_WARN, (uint8)VOLTAGE_WARN_TYPE_NOTRING, MN_DUAL_SYS_1);
		MMIENVSET_SetActiveModeOptValue(VOLTAGE_WARN, (uint8)VOLTAGE_WARN_TYPE_NOTRING, MN_DUAL_SYS_2);
		MMIENVSET_SetActiveModeOptValue(FLIP_RING_TYPE, (uint8)FLIP_RING_TYPE_NOTRING, MN_DUAL_SYS_1);
		MMIENVSET_SetActiveModeOptValue(FLIP_RING_TYPE, (uint8)FLIP_RING_TYPE_NOTRING, MN_DUAL_SYS_2);
		s_mr_envLock_state |= ENVLOCK_ENV;
	}
	else if(action == NES_UNLOCK_OTHERS)
	{
		/*恢复短消息等原始提示模式*/
		MMIENVSET_SetActiveModeOptValue(MSG_RING_TYPE, (uint8)s_mr_env_preVal.msg[0], MN_DUAL_SYS_1);
		MMIENVSET_SetActiveModeOptValue(MSG_RING_TYPE, (uint8)s_mr_env_preVal.msg[1], MN_DUAL_SYS_2);
		MMIENVSET_SetActiveModeOptValue(VOLTAGE_WARN, (uint8)s_mr_env_preVal.voltage[0], MN_DUAL_SYS_1);
		MMIENVSET_SetActiveModeOptValue(VOLTAGE_WARN, (uint8)s_mr_env_preVal.voltage[1], MN_DUAL_SYS_2);
		MMIENVSET_SetActiveModeOptValue(FLIP_RING_TYPE, (uint8)s_mr_env_preVal.flip[0], MN_DUAL_SYS_1);
		MMIENVSET_SetActiveModeOptValue(FLIP_RING_TYPE, (uint8)s_mr_env_preVal.flip[1], MN_DUAL_SYS_2);
		s_mr_wallpaperBufNeedRestore = TRUE;
		s_mr_envLock_state &= (~ENVLOCK_ENV);
	}
#endif
	MMIMR_Trace(("mrapp MMIMRAPP_nesActionInd action = %d, resValue = %d", action, resValue));
	return resValue;
}
#endif

/*****************************************************************************/
// 	Description :非阻塞连接时，应用需要调用此函来查询连接状态
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_connectStateQuery(int32 param)
{
#ifndef WIN32
	int32 ret = 0, error = 0;
	sci_fd_set writefds;
	int32 socket;
	MMIMR_Trace(("param: %d", param));
	
	if(param >= 0)
	{
		socket = s_mr_sockets[param].s;
		SCI_FD_ZERO (&writefds);
		SCI_FD_SET((long)socket, &writefds);
		
		ret = sci_sock_select(NULL, &writefds, NULL, 0); 
		
		MMIMR_Trace(("sci_sock_select() ret: %d ", ret));
		if(ret > 0)
		{
			if(SCI_FD_ISSET(socket, &writefds))
			{
				MMIMR_Trace(("writefds true "));
                s_mr_sockets[param].state = MRAPP_SOCKET_CONNECTED;
				return MR_SUCCESS;
			}
		}
		else if(ret == 0)
		{
			return MR_WAITING;
		}
		else
		{
			error = sci_sock_errno((long)socket);
			MMIMR_Trace(("error: %d", error));
			return MR_FAILED;
		}
	}
#endif
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description :设置声音播放音量
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_setSoundVolume(int32 param)
{
	uint32 vol = param;
	
	//设置音量
	if(vol > MRAPP_AUDIO_VOL_MAX)
		vol = MRAPP_AUDIO_VOL_MAX;
	else if(vol < 1)
		vol = 0;
	
	vol = vol*MMI_AUD_VOLUME_MAX/MRAPP_AUDIO_VOL_MAX;
	
	MMIMR_Trace(("audio.vol: %d", vol));
#ifndef WIN32
	if (0 == vol)
	{
		MMISRVAUD_SetMuteNohandle(TRUE);
	}
	else
	{
		MMISRVAUD_SetMuteNohandle(FALSE);
		MMISRVAUD_SetVolumeNoHandle(vol);
	}
#endif
	s_mr_audio_mng.soundVolume = vol;
	return MR_SUCCESS;
}

//2009-7-7 add begin
#ifdef __MRAPP_OVERSEA__
LOCAL int32 MMIMRAPP_setApnAccount(T_DSM_DATA_ACCOUNT *param)
{
	if(param)
    {
		memset(&gDsmDataAccount, param, sizeof(gDsmDataAccount)); /*lint !e64*/
        MMIMRAPP_startSetApnAccountTimer();
		return MR_WAITING;
	}
	return MR_FAILED;
}

PUBLIC void MMIMRAPP_setApnAccountRet(void)
{
	if(s_mr_allReady )
	{
        MMIMR_Trace(("mrapp MMIMRAPP_getApnAccountRet"));
		mr_event(MR_DATA_ACCOUNT_EVENT, MR_DATA_ACCOUNT_OP_SET, MR_SUCCESS);
	}
}

LOCAL int32 MMIMRAPP_setApnByUser(void)
{
    return MMIMRAPP_OpenSelectDataAccountWin(); /*lint !e718 !e746*/
}

LOCAL int32 MMIMRAPP_getApnAccount(void)
{	
	MMIMRAPP_startGetApnAccountTimer();
	return MR_WAITING;//must asyn return.
}

PUBLIC void MMIMRAPP_getApnAccountRet(int32 report)
{
	int32 i   = 0;
	int32 ret = MR_SUCCESS;
	MMICONNECTION_SETTING_T *connection_setting_info = NULL;
	
#if(SPR_VERSION >= 0x10A1140)
	MN_DUAL_SYS_E simsel;
#endif
	connection_setting_info = MMIMRAPP_GetConnetInfo(); /*lint !e718 !e18*/

#if (SPR_VERSION >= 0x10A1140)
    simsel = MMIMRAPP_GetActiveSim();

	if(g_mr_curUserApnAccuntId < MMICONNECTION_MAX_SETTING_NUM)
	{
		memset(&gDsmDataAccount,0,sizeof(gDsmDataAccount));
		memcpy(gDsmDataAccount.apn, connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].apn, connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].apn_len);
		memcpy(gDsmDataAccount.user_name, connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].username, connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].username_len);
		memcpy(gDsmDataAccount.password, connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].password, connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].password_len);
		gDsmDataAccount.authentication_type = connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].auth_type;
		
		for(i =0; i<4; i++)
		{
			gDsmDataAccount.dns[i] = connection_setting_info->setting_detail[simsel][g_mr_curUserApnAccuntId].dns[i];
		}

		ret = MR_SUCCESS;
	}
	else
	{
		ret = MR_FAILED;
	}
#else
	if(g_mr_curUserApnAccuntId < MMICONNECTION_MAX_SETTING_NUM)
	{
		memset(&gDsmDataAccount,0,sizeof(gDsmDataAccount));
		memcpy(gDsmDataAccount.apn, connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].apn, connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].apn_len);
		memcpy(gDsmDataAccount.user_name, connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].username, connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].username_len);
		memcpy(gDsmDataAccount.password, connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].password, connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].password_len);
		gDsmDataAccount.authentication_type = connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].auth_type;
		
		for(i =0; i<4; i++)
		{
			gDsmDataAccount.dns[i] = connection_setting_info->setting_detail[g_mr_curUserApnAccuntId].dns[i];
		}

		ret = MR_SUCCESS;
	}
	else
	{
		ret = MR_FAILED;
	}
#endif
	MMIMR_Trace(("mrapp MMIMRAPP_getApnAccountRet g_mr_curUserApnAccuntId:%d apn:%s ret:%d report:%d",
		          g_mr_curUserApnAccuntId, gDsmDataAccount.apn, ret, report));

	if(s_mr_allReady && report)
	{
		mr_event(MR_DATA_ACCOUNT_EVENT, MR_DATA_ACCOUNT_OP_GET, ret);
	}
}
#endif
//2009-7-7 add end

//2010-06-24 add begin
LOCAL int32 MMIMRAPP_SetNewSmsAction(uint8 param)
{
    MMIMR_Trace(("mrapp MMIMRAPP_SetNewSmsAction param = %d", param));

	if(0 == param || 1 == param)
    {
        s_mr_new_sms_action = param;
		return MR_SUCCESS;
	}
	return MR_FAILED;
}

PUBLIC int32 MMIMRAPP_GetNewSmsAction(void)
{ /*lint !e18*/
    MMIMR_Trace(("mrapp MMIMRAPP_GetNewSmsAction s_mr_new_sms_action = %d", s_mr_new_sms_action));
	return s_mr_new_sms_action;
}

LOCAL int32 MMIMRAPP_GetSCNumber(void)
{	
	MMIMRAPP_StartGetSCNumberTimer();
	return MR_WAITING;//must asyn return.
}

PUBLIC void MMIMRAPP_GetSCNumberRet(void)
{
    BOOLEAN ret = FALSE;
	uint8 sc_num_len = 0;
    uint8 sc_num[40 + 2] = {0};

    if(!s_mr_allReady)
        return;

    ret = MMISMS_AppGetSCAddr(MMIMRAPP_GetActiveSim(), sc_num, &sc_num_len, 40 + 2); /*lint !e718 !e18*/

    MMIMR_Trace(("mrapp MMIMRAPP_GetSCNumberRet ret = %d, sc_num = %s sc_num_len = %d", ret, sc_num, sc_num_len));
    
    if(ret && sc_num_len > 0)
    {
		mr_event(MR_SMS_GET_SC, (int32)sc_num, strlen(sc_num)+1); /*lint !e64*/
    }
    else
    {
		mr_event(MR_SMS_GET_SC, 0, 0);
    }
}
//2010-06-24 add end

//2010-07-01 add begin
LOCAL int32 MMIMRAPP_GetTCardStatus(void)
{	
#if (SPR_VERSION >= 0x10A1140)
#ifdef DUAL_TCARD_SUPPORT
    if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN)
      || TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
#else
    if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#endif
#else
    if(MMIAPISD_GetStatus())
#endif
	{
		if(MMIAPIUDISK_UdiskIsRun())
		{
			return MR_MSDC_NOT_USEFULL;
		}
		else
		{
			return MR_MSDC_OK;
		}
	}
	else
	{
		return MR_MSDC_NOT_EXIST;
	}
}
//2010-07-01 add end

//2010-09-15 add begin
LOCAL int32 MMIMRAPP_CheckWifiSupport(void)
{	
#ifdef __MRAPP_WIFI_SUPPORT__
    if(MMIWIFI_STATUS_CONNECTED == MMIAPIWIFI_GetStatus())
    {
        return MR_SUCCESS;
    }
    else
    {
       return MR_FAILED;
    }
#else
    return MR_FAILED;
#endif
}

LOCAL int32 MMIMRAPP_SetWifiSwitch(int32 param)
{	
#ifdef __MRAPP_WIFI_SUPPORT__
	if(param)
		s_mr_wifi_mode = TRUE;
	else		
		s_mr_wifi_mode = FALSE;
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}

PUBLIC uint8 MMIMRAPP_GetWifiMode(void)
{
#ifdef __MRAPP_WIFI_SUPPORT__
    return s_mr_wifi_mode;
#else
    return 0;
#endif
}
//2010-09-15 add end

//2010-09-26 add begin
LOCAL int32 MMIMRAPP_GetFilePos(int32 fHandle)
{	
	int32 pos = 0;
	int32 ret = 0;

    ret = SFS_GetFilePointer(fHandle, MMIFILE_SEEK_BEGIN, &pos);
    MMIMR_Trace(("mrapp MMIMRAPP_GetFilePos handle = 0x%08x pos = %d ret = %d", fHandle, pos, ret));

    if(ret == SFS_NO_ERROR)
		return (pos+MR_PLAT_VALUE_BASE);
	else
		return MR_FAILED;
}

LOCAL int32 MMIMRAPP_GetCurScene(void)
{	
    int32 mode = 0;
    int32 cur_mode = MMIAPIENVSET_GetActiveModeId();

    MMIMR_Trace(("mrapp MMIMRAPP_GetCurScene cur_mode = %d", cur_mode));

    switch(cur_mode)
    {
    case MMIENVSET_STANDARD_MODE:
        mode = MR_SCENE_NORMAL;
        break;
        
    case MMIENVSET_SILENT_MODE:
        mode = MR_SCENE_MUTE;
        break;

    case MMIENVSET_MEETING_MODE:
        mode = MR_SCENE_MEETING;
        break;

    case MMIENVSET_INSIDE_MODE:
        mode = MR_SCENE_INDOOR;
        break;

    case MMIENVSET_NOISY_MODE:
        mode = MR_SCENE_OUTDOOR;
        break;

    default:
        mode = MR_SCENE_NORMAL;
        break;
    }

    return mode;
}

int32 MMIMRAPP_GetBuildTime(uint8** output, int32* output_len)
{
    int32 i = 0;
    int32 len  = 0;
    int32 year = 0;
    int32 mon  = 0;
    int32 day  = 0;
    int32 hour = 0;
    int32 min  = 0;
    int32 ret  = MR_FAILED;
    uint8 *build_time = NULL;
    uint8 date[12] = {0};
    uint8 time[12] = {0};

#ifndef WIN32    
    build_time = (uint8*)VERSION_GetInfo((uint32)BUILD_TIME);
#endif
    MMIMR_Trace(("mrapp build_time: %s", build_time));

    if(build_time != NULL)
    {
        while(*build_time != 0x20)
        {
            date[i++] = *build_time++;
        }
        date[i] = '\0';
        build_time++;
        i = 0;
        while(*build_time != '\0')
        {
            time[i++] = *build_time++;
        }
        time[i] = '\0';
        
        if(date[0] >= '0' && date[0] <= '9')
        {
            mon = date[0]-'0';
        }
        
        if(date[1] >= '0' && date[1] <= '9')
        {
            mon = mon*10+date[1]-'0';
            day = date[3]-'0';
            
            if(date[4] >= '0' && date[4] <= '9')
            {
                day = day*10+date[4]-'0';

                year = (date[6]-'0')*1000+(date[7]-'0')*100+(date[8]-'0')*10+(date[9]-'0');
            }
            else
            {
                year = (date[5]-'0')*1000+(date[6]-'0')*100+(date[7]-'0')*10+(date[8]-'0');
            }
        }
        else
        {
            day = date[2]-'0';
            if(date[3] >= '0' && date[3] <= '9')
            {
                day = day*10+date[3]-'0';
                year = (date[5]-'0')*1000+(date[6]-'0')*100+(date[7]-'0')*10+(date[8]-'0');
            }
            else
            {
                year = (date[4]-'0')*1000+(date[5]-'0')*100+(date[6]-'0')*10+(date[7]-'0');
            }
        }

        if(time[0] >= '0' && time[0] <= '9')
        {
            hour = time[0]-'0';
        }

        if(time[1] >= '0' && time[1] <= '9')
        {
            hour = hour*10+time[1]-'0';

            if(time[3] >= '0' && time[3] <= '9')
            {
                min = time[3]-'0';
                if(time[4] >= '0' && time[4] <= '9')
                {
                    min = min*10+time[4]-'0';
                }
            }
        }
        else
        {
            if(time[2] >= '0' && time[2] <= '9')
            {
                min = time[2]-'0';
                if(time[3] >= '0' && time[3] <= '9')
                {
                    min = min*10+time[3]-'0';
                }
            }
        }

        sprintf((char*)g_mr_build_time, "%04d/%02d/%02d %02d:%02d", year, mon, day, hour, min);
		*output = g_mr_build_time;
        *output_len = strlen((char*)g_mr_build_time);
        ret = MR_SUCCESS;
    }

    MMIMR_Trace(("mrapp output: %s output_len:%d", *output, *output_len));

    return ret;
}

int32 MMIMRAPP_GetFileCreateTime(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
    uint32 create_time = 0;
    uint32 file_size   = 0;
    BOOLEAN  result = FALSE;
    SFS_HANDLE          sfs_handle = 0;
    SFS_FIND_DATA_T     find_data = {0};
    MMI_TM_T tm = {0};
    
    if(input == NULL || input_len == NULL || output == NULL || output_len == NULL)
    {
        return MR_FAILED;
    }

    devName = MMIMRAPP_getCurDevName(&devNameLen);

	memset(full_path, 0, sizeof(full_path));
	memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));
		
	ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)input, (uint16)strlen(input)); /*lint !e64*/
	if(	MMIFILE_CombineFullPath_fix(
			(const uint8*)devName, devNameLen,
			(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
			(const uint8*)ucs2_name_arr, ucs2_name_len,
			(uint8*)full_path,
			&full_path_len
			))
    {
        sfs_handle = MMIFILE_FindFirstFile(full_path, full_path_len, &find_data); /*lint !e64*/

        if(SFS_INVALID_HANDLE != sfs_handle)
        {
            mr_datetime *temp = (mr_datetime*)output;
            temp->year = find_data.create_Date.year;
            temp->month = find_data.create_Date.mon;
            temp->day   = find_data.create_Date.mday;
            temp->hour  = find_data.create_time.hour;
            temp->minute= find_data.create_time.min;
            temp->second= find_data.create_time.sec;

            MMIMR_Trace(("mrapp MMIMRAPP_GetFileCreateTime %d-%d-%d %d:%d:%d", temp->year, temp->month, temp->day, temp->hour, temp->minute, temp->second));
            SFS_FindClose(sfs_handle);
			*output_len = sizeof(mr_datetime);
            return MR_SUCCESS;
        }        
    }   

    return MR_FAILED;
}

int32 MMIMRAPP_SetAllowTurnOffBacklight(BOOLEAN allow)
{
    MMIMR_Trace(("mrapp MMIMRAPP_SetAllowTurnOffBacklight allow: %d need_restore: %d", allow, s_mr_need_restore_backlight));
    
    if(allow)
    {
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        if(s_mr_need_restore_backlight == TRUE)
        {
            s_mr_need_restore_backlight = FALSE;
        }
    }
    else
    {
        //是否需要自动解锁，不需要则将下面if语句注掉
		//if(MMK_IsOpenWin(MMIKL_CLOCK_DISP_WIN_ID))
		//{
		//	MMK_CloseWin(MMIKL_CLOCK_DISP_WIN_ID);
		//}
		MMIDEFAULT_SetBackLight(TRUE);
		MMIDEFAULT_AllowTurnOffBackLight(FALSE); 

        if(s_mr_need_restore_backlight == FALSE)
        {
            s_mr_need_restore_backlight = TRUE;
        }
    }
    return MR_SUCCESS;
}
//2010-09-26 add end

#ifdef __MR_RECORD_SUPPORT__
/*****************************************************************************/
// 	Description :处理录音过程中的异常事件
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
void MMIMRAPP_HandleRecordResult(MMISRVAUD_REPORT_RESULT_E  result, DPARAM param)
{
	MMIMR_Trace(("mrapp MMIMRAPP_HandleRecordResult state: %d result: %d", s_mr_record_state, result));

    if(s_mr_record_state == MMIRECORD_STATE_RECORDING)
    {
        if(MMISRVAUD_REPORT_RESULT_SUCESS == result)
        {
			if(!MMIFILE_IsEnoughSpace(MMIFMM_STORE_TYPE_FIXED, g_mr_global.device_type, MMIRECORD_MIN_NEED_SPACE, PNULL)) /*lint !e64*/
            {
                mr_event(MR_ERROR_EVENT, DSM_ERROR_NO_SPACE, 0);
            }
        }
        else
        {
            mr_event(MR_ERROR_EVENT, DSM_ERROR_UNKNOW, 0);
        }
        s_mr_record_state == MMIRECORD_STATE_RECORD_STOPED;
    }
}

/*****************************************************************************/
// 	Description :录音回调函数
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
BOOLEAN MMIMRAPP_HandleRecordCallBack(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = NULL;
	MMIMR_Trace(("mrapp MMIMRAPP_HandleRecordResult state: %d handle: %d", s_mr_record_state, handle));

	if(param != NULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		MMIMR_Trace(("mrapp MMIMRAPP_HandleRecordResult report_ptr: 0x%x", report_ptr));

		if(report_ptr != NULL)
		{
			MMIMR_Trace(("mrapp MMIMRAPP_HandleRecordResult report_ptr->report=%d", report_ptr->report));

			switch(report_ptr->report)
            {
            case MMISRVAUD_REPORT_END:
                MMISRVAUD_Stop(handle);
                MMISRVMGR_Free(handle);
                MMIMRAPP_SetRecordHandle(0);
                
                MMIMRAPP_HandleRecordResult(report_ptr->data1, NULL);
                break;

            default:
                break;
            }
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*****************************************************************************/
// 	Description :开始录音
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_RecordStart(MR_T_VOICE_REC_REQ *req)
{
    int32 resValue = FALSE;
    uint8  *devName = NULL;
	uint16 devNameLen = 0;
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
    int32  file_handle = 0;
    uint16 file_path[SFS_MAX_PATH] = {0};
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
	MMISRVMGR_SERVICE_REQ_T req_data = {0};
	MMISRVAUD_RET_E play_res = MMISRVAUD_RET_MAX;
    MMISRVAUD_TYPE_T audio_srv = {0};

    MMIMR_Trace(("mrapp MMIMRAPP_RecordStart src: %s srcLen: %d srcType: %d format: %d",
        req->src, req->src_len, req->src_type, req->format));

    switch(req->format)
    {
    case MR_SOUND_WAV:
        audio_srv.info.record_file.fmt = MMISRVAUD_RECORD_FMT_ADPCM;
        break;

    case MR_SOUND_AMR:
        audio_srv.info.record_file.fmt = MMISRVAUD_RECORD_FMT_AMR;
        break;

    case MR_SOUND_PCM:
        audio_srv.info.record_file.fmt = MMISRVAUD_RECORD_FMT_PCM;
        break;

    default:
        return MR_IGNORE;
    }

    if(req->src_type != SRC_NAME || req->src == NULL)
    {
        return MR_FAILED;
    }

    mr_fs_get_filename(file_path, req->src);

    file_handle = SFS_CreateFile(file_path, SFS_MODE_SHARE_WRITE|SFS_MODE_CREATE_NEW, NULL, NULL);
    MMIMR_Trace(("mrapp MMIMRAPP_RecordStart file_handle: %d", file_handle));
    if(file_handle == SFS_INVALID_HANDLE)
    {
        return MR_FAILED;
    }

	req_data.notify = MMIMRAPP_HandleRecordCallBack;
    req_data.pri = MMISRVAUD_PRI_NORMAL;
	
    audio_srv.duation = 0;
    audio_srv.eq_mode = 0;
    audio_srv.is_mixing_enable = FALSE;
    audio_srv.play_times = 1;
    audio_srv.all_support_route = MMISRVAUD_ROUTE_NONE;
	
	audio_srv.info.record_file.type = MMISRVAUD_TYPE_RECORD_FILE;
	audio_srv.info.record_file.file_handle = file_handle;
	audio_srv.info.record_file.frame_len = MMISRVAUD_RECORD_FRAME_LEN_DEFAULT;    
    audio_srv.info.record_file.source = MMISRVAUD_RECORD_SOURCE_NORMAL;
	audio_srv.info.record_file.name = file_path;

	audio_handle = MMIMRAPP_GetRecordHandle();
	if(audio_handle > INVALID_HANDLE)
	{
		MMISRVAUD_Stop(audio_handle);
        MMISRVMGR_Free(audio_handle);
		audio_handle = INVALID_HANDLE;
        MMIMRAPP_SetRecordHandle(INVALID_HANDLE);
	}
	audio_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req_data, &audio_srv);
    MMIMR_Trace(("mrapp MMIMRAPP_RecordStart audio_handle: %d", audio_handle));
    if(audio_handle > INVALID_HANDLE)
    {
		MMIMRAPP_SetRecordHandle(audio_handle);
        play_res = MMISRVAUD_Play(audio_handle, 0);
		MMIMR_Trace(("mrapp MMIMRAPP_RecordStart play_res: %d", play_res));
		
        s_mr_record_state = MMIRECORD_STATE_RECORDING;
        resValue = MR_SUCCESS;
    }
    else
    {
		s_mr_record_state = MMIRECORD_STATE_START;
        resValue = MR_FAILED;
    }
	MMIMR_Trace(("mrapp MMIMRAPP_RecordStart resValue: %d", resValue));

	return resValue;
}

/*****************************************************************************/
// 	Description :暂停录音
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_RecordPause(void)
{
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
    BOOLEAN result = FALSE;

	audio_handle = MMIMRAPP_GetRecordHandle();
	MMIMR_Trace(("mrapp MMIMRAPP_RecordPause audio_handle: %d", audio_handle));

	if(audio_handle > INVALID_HANDLE)
	{
    	result = MMISRVAUD_Pause(audio_handle);

    	MMIMR_Trace(("mrapp MMIMRAPP_RecordPause result: %d", result));

    	if(result)
    	{
       		s_mr_record_state = MMIRECORD_STATE_RECORD_PAUSED;
       		return MR_SUCCESS;
    	}
    	else
    	{
        	return MR_FAILED;
    	}
	}
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description :继续录音
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_RecordResume(void)
{
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
    BOOLEAN result = FALSE;

	audio_handle = MMIMRAPP_GetRecordHandle();
	MMIMR_Trace(("mrapp MMIMRAPP_RecordPause audio_handle: %d", audio_handle));

	if(audio_handle > INVALID_HANDLE)
	{
    	result = MMISRVAUD_Resume(audio_handle);

    	MMIMR_Trace(("mrapp MMIMRAPP_RecordResume result: %d", result));

    	if(result)
    	{
        	s_mr_record_state = MMIRECORD_STATE_RECORDING;
        	return MR_SUCCESS;
    	}
    	else
    	{
        	return MR_FAILED;
    	}
	}
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description :停止录音
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_RecordStop(void)
{
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
    BOOLEAN stop_result  = FALSE;
    BOOLEAN close_result = FALSE;

	audio_handle = MMIMRAPP_GetRecordHandle();
	MMIMR_Trace(("mrapp MMIMRAPP_RecordStop audio_handle: %d", audio_handle));

	if(audio_handle > INVALID_HANDLE)
	{
    	MMIRECORD_Destructor();
		s_mr_record_state = MMIRECORD_STATE_STOPING;
    	stop_result = MMISRVAUD_Stop(audio_handle);
    	close_result = MMISRVMGR_Free(audio_handle);
		audio_handle = INVALID_HANDLE;
		MMIMRAPP_SetRecordHandle(INVALID_HANDLE);
    	MMIMR_Trace(("mrapp MMIMRAPP_RecordStop stop_result: %d close_result: %d", stop_result, close_result));

    	if(stop_result && close_result)
    	{
			s_mr_record_state = MMIRECORD_STATE_RECORD_STOPED;
        	return MR_SUCCESS;
    	}
    	else
    	{
        	return MR_FAILED;
    	}
	}
	return MR_FAILED;
}
#endif

int32 mr_lcd_set_rotation(int32 param)
{
    switch(param)
    {
    case MR_LCD_ROTATE_NORMAL:
        g_screen_angle.lcd_angle = LCD_ANGLE_0;
        g_screen_angle.win_angle = WIN_SUPPORT_ANGLE_0;
        break;
    case MR_LCD_ROTATE_90:
        g_screen_angle.lcd_angle = LCD_ANGLE_270;
        g_screen_angle.win_angle = WIN_SUPPORT_ANGLE_270;
        break;
    case MR_LCD_ROTATE_180:
        g_screen_angle.lcd_angle = LCD_ANGLE_180;
        g_screen_angle.win_angle = WIN_SUPPORT_ANGLE_180;
        break;
    case MR_LCD_ROTATE_270:
        g_screen_angle.lcd_angle = LCD_ANGLE_90;
        g_screen_angle.win_angle = WIN_SUPPORT_ANGLE_90;
        break;
    case MR_LCD_MIRROR:
    case MR_LCD_MIRROR_ROTATE_90:
    case MR_LCD_MIRROR_ROTATE_180:
    case MR_LCD_MIRROR_ROTATE_270:
    default:
        return MR_IGNORE;
    }

    MMK_SetWinSupportAngle(MMIMRAPP_BASE_WIN_ID, g_screen_angle.win_angle);
    //MMK_SetScreenAngle(g_screen_angle.cur_angle);
    GUILCD_SetLogicAngle(GUI_MAIN_LCD_ID, g_screen_angle.lcd_angle);

    return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :平台扩展接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_plat(int32 code, int32 param)
{
    MMIMR_Trace(("mrapp mr_plat code:%d param:%d", code, param));
	switch(code)
	{
//2009-7-7 add begin
#ifdef __MRAPP_OVERSEA__
	case MR_SET_DATA_ACCOUNT_USE_FLAG:
		if(param)
        {
			gDsmUseNVdataAccount = 0;
		}
        else
        {
			gDsmUseNVdataAccount = 1; 
		}
		return MR_SUCCESS;
        
	case MR_SET_DATA_ACCOUNT_MTK_UI:
		MMIMRAPP_setApnByUser();
		return MR_SUCCESS;
        
	case MR_GET_DATA_ACCOUNT_REQ:
		return MMIMRAPP_getApnAccount();
#endif
//2009-7-7 add end

	case PLAT_ADAPT_CPUCLK:
		return MMIMRAPP_adaptCpuClk(param);
        
#ifdef MRAPP_SIMUGAME_OPEN
	case PLAT_GET_KEY_NUM:
		return MR_PLAT_VALUE_BASE + sizeof(mrKeyMap);
	case PLAT_GET_KEY_STATE:
		if(param < sizeof(mrKeyMap))
		{
			uint32 state = KPDSVR_GetKeyState(mrKeyMap[param]);
			//MMIMR_Trace(("key[%d] state:%d", param, state));
			return MR_PLAT_VALUE_BASE + state;
		}
		break;
	case PLAT_NES_ACTION_IND:
		return MMIMRAPP_nesActionInd(param);
#endif

	case PLAT_ENVINFO_WRITE:
		{
			MRAPP_ENVINFO_T envinfo;
			envinfo.count = param;
			MMINV_WRITE(MRAPP_ENV_INFO, &envinfo);
			return MR_SUCCESS;
		}
    
	case PLAT_ENVINFO_READ:
		{
			MRAPP_ENVINFO_T envinfo;
			int8 inited;
			MN_RETURN_RESULT_E result;
			
			MMINV_READ(MRAPP_ENV_INITED, &inited, result);
			MMIMR_Trace(("result: %d, inited: %d", result, inited));
			if(result != MN_RETURN_SUCCESS)
			{
				inited = 0;
			}
			
			if(inited == MRAPP_ENV_VALUE_INITED)
			{
				MMINV_READ(MRAPP_ENV_INFO, &envinfo, result);
				MMIMR_Trace(("result: %d, count: %d", result, envinfo.count));
				if(result == MN_RETURN_SUCCESS)
				{
					return MR_PLAT_VALUE_BASE + envinfo.count;
				}
				else
				{
					return MR_FAILED;
				}
			}
			else
			{
				inited = MRAPP_ENV_VALUE_INITED;
				MMINV_WRITE(MRAPP_ENV_INITED, &inited);
				return MR_WAITING;
			}
		}
			
	case PLAT_SET_KEYSOUND://按键音开启关闭控制: 0 关闭， 1 开启
		if(param == 0)
		{
			MMIAPISET_StopRing(MMISET_RING_TYPE_KEY);  //停止按键音//for Z998 by zlm 20071114
			MMIDEFAULT_EnableKeyRing(SKY_VIDEOPLAYER, 0);
//			MMIDEFAULT_EnableKeyRing(FALSE);
		}
		else
		{
			MMIDEFAULT_EnableKeyRing(SKY_VIDEOPLAYER, 1);
//			MMIDEFAULT_EnableKeyRing(TRUE);
		}
		return MR_SUCCESS;
        
#ifdef MRAPP_SIMUGAME_OPEN
	case PLAT_SET_NESWAPSAVEDEV:
		MMIMR_Trace(("dev: %c", param));
		g_mr_dualsys_mng.devSel = (uint8)param;
		return MR_SUCCESS;	
#endif		

	case PLAT_SET_LCD_REFRESH_MODE:// 2009-4-28 add
		MMIMRAPP_setLcdAsynMode(param);
		return MR_SUCCESS;

    case PLAT_SET_TP_SOUND://触屏音开启关闭控制: 0 关闭， 1 开启
		if(param == 0)
		{
			MMIAPISET_StopRing(MMISET_RING_TYPE_TKEY);
            MMIDEFAULT_EnableTpRing(SKY_VIDEOPLAYER, 0);
//	        MMIDEFAULT_EnableTpRing(FALSE);
		}
		else
		{
            MMIDEFAULT_EnableTpRing(SKY_VIDEOPLAYER, 1);
//			MMIDEFAULT_EnableTpRing(TRUE);
		}
		return MR_SUCCESS;
        
	case PLAT_CONNECT_RESULT:
		return MMIMRAPP_connectStateQuery(param);
        
	case PLAT_SET_CONNECT_TIMEOUT:
		s_mr_connect_timeout = param/1000;
		return MR_SUCCESS;

	case PLAT_APP_PICSTOP_IND:			
		MMIMR_Trace(("mrapp PLAT_APP_PICSTOP_IND g_mr_vmBackRun = %d", g_mr_vmBackRun));
        MMIMRAPP_StopStatusIconTimer();
        MAIN_SetIdleMrappState(FALSE); /*lint !e718 !e18*/
		return MR_SUCCESS;
        
	case PLAT_SET_ACTIVE_SIM:
		{
            int32 simId = (-param)-1;
            BOOLEAN ret = FALSE;
            if(simId >= MN_DUAL_SYS_1 && simId < MN_DUAL_SYS_MAX)
            {
    			ret = MMIPHONE_IsSimOk(simId);
    			MMIMR_Trace(("ret: %d, simId: %d", ret, simId));
    			if(ret)
    			{
    				g_mr_dualsys_mng.simSel = (int32)simId;
    				return MR_SUCCESS;
    			}
            }
			break;
		}

    case PLAT_SMS_RECIVE_ACTION:
        return MMIMRAPP_SetNewSmsAction(param);

	/* begin:added by Tommy.yan 20120120 */
	case PLAT_GET_SMS_STATUS:
		return mr_sms_get_status(param);
		
	case PLAT_GET_SMS_NUM:
		return mr_sms_get_num(param);
		
	case PLAT_GET_SMS_CAPACITY:
		return mr_sms_get_capacity(param);
	/* end:added by Tommy.yan 20120120 */
	
    case PLAT_GET_BACKLIGHT_STATE:
        if(MMIDEFAULT_IsAllowBackLightTurnOff())
        {
            return MR_BACKLIGHT_OFF;
        }
        else
        {
            return MR_BACKLIGHT_ON;
        }

    case PLAT_BACKSTAGE_SUPPORT:
		return MR_IGNORE;
        //return MR_SUCCESS;

    case PLAT_GET_SC_REQ:
        return MMIMRAPP_GetSCNumber();
        
	case PLAT_APP_ACTIVE2FRONT_REQ:
		MMIMR_Trace(("DSMQQ PLAT_APP_ACTIVE2FRONT_REQ g_mr_vmBackRun = %d", g_mr_vmBackRun));
		switch(g_mr_vmBackRun)
		{
		case MRAPP_MAX:
			break;				
		default:
			if(MMIMRAPP_MrAppRun(NULL, g_mr_vmBackRun))
			{
				mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_APPRESUME, 0);
				return MR_SUCCESS;
			}
			break;
		}
		return MR_FAILED;
        
	case PLAT_GET_TC_INFO:
#ifdef MR_HANDSET_IS_SUPPORT_TOUCHPANEL
#ifdef MMI_PDA_SUPPORT
        return MR_SETTING_SCREEN_ONLY_TOUCH;
#else
		return MR_SETTING_SCREEN_TOUCH;
#endif
#else
		return MR_SETTING_SCREEN_NORMAL;
#endif

	case PLAT_GET_LANG_INFO:
	/*2009-10-22 111,17787 fix. for overseas*/
		{
			MMISET_LANGUAGE_TYPE_E language_type;
			MMIAPISET_GetLanguageType(&language_type);
			switch(language_type)
            {
			default:
		    case MMISET_LANGUAGE_ENGLISH:     
				return MR_ENGLISH;
		    case MMISET_LANGUAGE_SIMP_CHINESE:   
				return MR_CHINESE;
		    case MMISET_LANGUAGE_TRAD_CHINESE:   
				return MR_TCHINESE;
		    case MMISET_LANGUAGE_ARABIC:
				return MR_ARABIC;
		    case MMISET_LANGUAGE_FRENCH:
				return MR_FRENCH;
		    case MMISET_LANGUAGE_HINDI:
				return MR_HINDI;
		    case MMISET_LANGUAGE_HUNGARIAN:
				return MR_HUNGARIAN;
		    case MMISET_LANGUAGE_INDONESIAN:
				return MR_INDONESIAN;
		    case MMISET_LANGUAGE_MALAY:
				return MR_MALAY;
		    case MMISET_LANGUAGE_PORTUGUESE:
				return MR_PORTUGUESE;
		    case MMISET_LANGUAGE_RUSSIAN:
				return MR_RUSSIAN;
		    case MMISET_LANGUAGE_SPANISH:
				return MR_SPANISH;
		    case MMISET_LANGUAGE_TAGALOG:
				return MR_ENGLISH;//temp.
		    case MMISET_LANGUAGE_THAI:
				return MR_THAI;
		    case MMISET_LANGUAGE_VIETNAMESE://only include text of above 15 langugages
		    	return MR_VIETNAMESE;
		    case MMISET_LANGUAGE_URDU:
				return MR_URDU;
		    case MMISET_LANGUAGE_ITALIAN:
				return MR_ITALIAN;
		    case MMISET_LANGUAGE_PERSIAN:
				return MR_PERSIAN;
		    case MMISET_LANGUAGE_TURKISH:
				return MR_TURKISH;
		    case MMISET_LANGUAGE_GERMAN:
				return MR_GERMAN;
		    case MMISET_LANGUAGE_GREEK:
				return MR_GREEK;
				//break;
    		}
		}
	/*2009-10-22 111,17787 fix end*/
	case PLAT_GET_RAND_NUM:
		srand(SCI_GetTickCount());
		return MR_PLAT_VALUE_BASE + rand()%param;

    case PLAT_GET_CUR_SCENE:
        return MMIMRAPP_GetCurScene();
    
	case PLAT_FORBID_HUP2IDLE:
		s_mr_fbhup2idle = param;
		return MR_SUCCESS;

    case PLAT_GET_CELLINFO_START:
        return MR_SUCCESS;

    case PLAT_GET_CELLINFO_END:
        return MR_SUCCESS;

    case PLAT_TCARD_STATUS:
        return MMIMRAPP_GetTCardStatus();
        
	case PLAT_GET_FILE_POS:
        return MMIMRAPP_GetFilePos(param);
    
	case PLAT_GET_SOUND_VOLUME:
		return MRAPP_AUDIO_VOL_MAX + MR_PLAT_VALUE_BASE;
    
	case PLAT_SET_SOUND_VOLUME:
		return MMIMRAPP_setSoundVolume(param);

    case PLAT_CHECK_WIFI:
        return MMIMRAPP_CheckWifiSupport();

    case PLAT_SET_WIFI_SWITCH:
        return MMIMRAPP_SetWifiSwitch(param);
		
#ifdef __MR_RECORD_SUPPORT__
    case MR_VOICE_RECORD_PAUSE:
        return MMIMRAPP_RecordPause();

    case MR_VOICE_RECORD_RESUME:
        return MMIMRAPP_RecordResume();
        
    case MR_VOICE_RECORD_STOP:
        return MMIMRAPP_RecordStop();
#endif

#ifdef __MRAPP_MOTION_SUPPORT__
    case MR_MOTION_STOP_LISTEN:				
        return mr_motion_stop_listen(param);
    case MR_MOTION_PWRON:
        return mr_motion_power_on(param);
    case MR_MOTION_PWROFF:
        return mr_motion_power_off(param);
    case MR_MOTION_LISTEN_SHAKE:
        return mr_motion_listen_shake(param);
    case MR_MOTION_LISTEN_TILT:
        return mr_motion_listen_tilt(param);
    case MR_MOTION_GET_MAX_ACC:	
        return mr_motion_get_max_acc(param);
#endif

#ifdef UI_MULTILAYER_SUPPORT
    case PLAT_RELEASE_LAYER:
        return mr_layer_release(param);

    case PLAT_SET_ACTIVE_LAYER:
        return mr_layer_set_active(param);

    case PLAT_SET_LAYER_ALPHA:
        return mr_layer_set_alpha(TRUE, param);

    case PLAT_SET_LAYER_NO_ALPHA:
        return mr_layer_set_alpha(FALSE, param);
        
    case PLAT_SET_LAYER_TRANSPARENT:
        return mr_layer_set_transparent(TRUE, param);
        
    case PLAT_SET_LAYER_NO_TRANSPARENT:
        return mr_layer_set_transparent(FALSE, param);
#endif

#ifdef __MR_CFG_CAMERA_SUPPORT__
    case PLAT_CAMERA_INIT:
        return mr_camera_init(param);
        
    case PLAT_CAMERA_PAUSE:
        return mr_camera_pause(param);
        
    case PLAT_CAMERA_RESUME:
        return mr_camera_resume(param);
            
    case PLAT_CAMERA_CLOSE:
        return mr_camera_destroy(param);

    case PLAT_CAMERA_ZOOM_MAX:
        return mr_camera_get_zoom_max(param);
        
    case PLAT_CAMERA_ZOOM_IN:
        return mr_camera_zoom_in(param);
        
    case PLAT_CAMERA_ZOOM_OUT:
        return mr_camera_zoom_out(param);
#endif 

    case PLAT_SET_LCD_ROTATE:
        return mr_lcd_set_rotation(param);

	case PLAT_VM_PAUSE_MODE:
	default:
		return MR_IGNORE;
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description :增强的平台扩展接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 resValue = MR_IGNORE;

    MMIMR_Trace(("mrapp mr_platEx code:%d input:0x%08x input_len:%d output:0x%08x output_len:%d", code, input, input_len, output, output_len));

	if(code < 2000 || code >= 4000)
	{
		resValue = MMIMRAPP_mutiCmd(code, input, input_len, output, output_len, cb);
	}
	else if(code >= 2000 && code < 3000)//audio
	{
#ifdef MRAPP_SIMUGAME_OPEN
		if(code%10 == ACI_AUDIO_PCM && code/10 >= 201 && code/10 <= 220)//pcm
		{
			resValue = MMIMRAPP_PcmCmd(code, input, input_len, output, output_len, cb);
		}
		else
#endif
		{
			resValue = MMIMRAPP_AudioCmd(code, input, input_len, output, output_len, cb);
		}
	}
	else if(code >= 3000 && code < 4000)//Image
	{
		resValue = MMIMRAPP_imageCmd(code, input, input_len, output, output_len, cb);
	}
	
	return resValue;
}

/**---------------------------------------------------------------------------*
**                       function bodies  -- local
**---------------------------------------------------------------------------*/


/*****************************************************************************/
// 	Description : 将'\' 转换成'/'
//	Global resource dependence : 
//  Author:	qgp
//	Note:
/*****************************************************************************/
static void MMIMRAPP_ConvertPath2Loc(char *str, uint16 len)
{
	int i;
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == 0x5c)// "\" to "/"
			str[i] = 0x2f;
	}
}

/*****************************************************************************/
// 	Description : 将'/' 转换成'\'
//	Global resource dependence : 
//  Author:	qgp
//	Note:
/*****************************************************************************/
static void MMIMRAPP_ConvertPath2Pc(char *str, uint16 len)
{
	int i;
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == 0x2f)// '/' to '\'
			str[i] = 0x5c;
	}
}


/*****************************************************************************/
// 	Description :获取对应设备的名字
//	Global resource dependence : 
//  Author:	qgp
//	Note:
/*****************************************************************************/
LOCAL const uint8* MMIMRAPP_getCurDevName(uint16 *length)
{
	const uint8* ptr = NULL;
    MMIMR_Trace(("mrapp MMIMRAPP_getCurDevName device_type:%d", g_mr_global.device_type));
	switch(g_mr_global.device_type)
	{
	case FS_HS:
		*length = MMIFILE_DEVICE_SYS_NAME_LEN_AD;
		ptr = (const uint8*)MMIFILE_DEVICE_SYS_NAME;
		break;
	case FS_UDISK:
		*length = MMIFILE_DEVICE_UDISK_LEN_AD;
		ptr = (const uint8*)MMIFILE_DEVICE_UDISK;
		break;
	case FS_MMC:
        if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
        {
    		*length = MMIFILE_DEVICE_SDCARD_LEN_AD;
    		ptr = (const uint8*)MMIFILE_DEVICE_SDCARD;
        }
#ifndef DUAL_TCARD_SUPPORT
        else if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
            *length = MMIFILE_DEVICE_SDCARD2_LEN;
    		ptr = (const uint8*)MMIFILE_DEVICE_SDCARD2;
        }
#endif
		break;
#ifdef DOUBLE_TFCARD
	case FS_MMC_2:
		*length = MMIFILE_DEVICE_SDCARD_LEN_2_AD;
		ptr = (const uint8*)MMIFILE_DEVICE_SDCARD_2;
		break;
#endif
	default:
		MRP_ASSERT(0);
		break;
	}
	
	return ptr;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: qgp
//	Note:
///*****************************************************************************/
LOCAL uint32 MMIMRAPP_getUcstrLen(uint8 *wstr)
{
	uint32 wlen = 0;
	uint8 *ptr = (uint8 *)wstr;
	
	if(ptr != NULL)
	{
		while((*ptr<<8) + *(ptr+1) != 0)
		{
			wlen += 2;
			ptr += 2;
		}
	}
	
	return wlen;
}

/*****************************************************************************/
// 	Description :BCD码转换成十进制的字符串
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_bcd2dec(uint8 *bcd, uint8* dec, uint16 convertLen)
{
	uint8 i, j;
	MMIMR_Trace(("mrapp. MMIMRAPP_bcd2dec() convertLen: %d", convertLen));
	
	if(bcd == NULL || dec == NULL)
		return;
	
	for(i = 0, j = 0; i < convertLen; i++)
	{
		dec[j++] = (bcd[i] & 0x0F) + '0';
		dec[j++] = ((bcd[i] >> 4) & 0x0F) + '0';
	}
}

/*****************************************************************************/
// 	Description :process APP_MN_ACTIVATE_PDP_CONTEXT_CNF
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_ProcessGprsActiveCnf(MMIPDP_RESULT_E result)
{
	int32 ret = MR_FAILED;
	
	MMIMR_Trace(("mrapp MMIMRAPP_ProcessGprsActiveCnf result: %d", result));
    
	if(result == MMIPDP_RESULT_SUCC)
    {
		s_mr_netContext.gprsState = MRAPP_GPRS_ACTIVED;
		ret = MR_SUCCESS;
	}
/*modify by zack@20120316 start */
//底层已经做了重试操作，这里没必要再重拨
#if 0
    else if(s_mr_netContext.pdp_retry_times < MRAPP_PDP_RETRY_TIMES)
    {
		s_mr_netContext.pdp_retry_times++;
		MMIMRAPP_gprsRetryTimerStart(MRAPP_PDP_RETRY_TIMER_OUT);
		ret = MR_WAITING;
    }
    else
    {
		MMIMRAPP_gprsRetryTimerStop();
		s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
		s_mr_netContext.mrNetActived = FALSE;
	}
#endif
/*modify by zack@20120316 end */
	return ret;
}

/*****************************************************************************/
// 	Description : process ps signal
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
MMI_RESULT_E MMIMRAPP_handlePsMsg(MMIPDP_CNF_INFO_T *msg_ptr)
{
	uint8 recode = MMI_RESULT_TRUE;
	int32 resValue = MR_FAILED;
	MMIMRAPP_NETINIT_RESULT_T opResult;
    MMIPDP_CNF_INFO_T *signal_ptr = (MMIPDP_CNF_INFO_T*)msg_ptr;

	MMIMR_Trace(("mrapp. MMIMRAPP_handlePsMsg() msg_id = %d state = %d", signal_ptr->msg_id, s_mr_netContext.mrNetActived));
	
	if(!signal_ptr)
    {
		MMIMR_Trace(("mrapp. not mr event"));
		return MMI_RESULT_FALSE;
	}
	
	MMIMRAPP_gprsProtectTimerStop();

	switch( signal_ptr->msg_id )
	{
	case MMIPDP_ACTIVE_CNF:
        s_mr_netContext.netid = signal_ptr->nsapi;
		resValue = MMIMRAPP_ProcessGprsActiveCnf(signal_ptr->result);
    	if(resValue != MR_WAITING)
    	{
    		opResult.cb = s_mr_netContext.gprsCb;
    		opResult.result = resValue;
    		MMK_PostMsg(
#ifdef MR_BINDTO_VIRTUAL_WIN
    			VIRTUAL_WIN_ID,
#else
    			MMIMRAPP_BASE_WIN_ID,
#endif
    			MMRAPP_MSG_NETINIT_RESULT, 
    			&opResult, sizeof(MMIMRAPP_NETINIT_RESULT_T));
    		
    		MMI_TriggerMMITask();
    	}
		break;
		
	case MMIPDP_DEACTIVE_CNF:
		if(s_mr_netContext.subEvn == MRAPP_NETSUBEVN_WAITDAIL)
		{
			s_mr_netContext.subEvn = MRAPP_NETSUBEVN_NONE;
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
            s_mr_netContext.netid = 0;
			
			if(s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVEING)
            {
				s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
				s_mr_netContext.mrNetActived = FALSE;
			}
		}
		break;
    
	case MMIPDP_DEACTIVE_IND:
        s_mr_netContext.need_count = FALSE;
		s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
		s_mr_netContext.mrNetActived = FALSE;
		break;
		
	default:
		recode = MMI_RESULT_FALSE;
		break;
	}
	
	MMIMR_Trace(("mrapp. MMIMRAPP_processPsMsg() RETURN"));
	return recode;
}

/*****************************************************************************/
// 	Description : re-dail
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_redail(void)
{
	int32 ret = 0;
	uint8 *user_ptr = ""; /*lint !e64*/
	uint8 *passwd_ptr = ""; /*lint !e64*/
    MMIPDP_ACTIVE_INFO_T app_info = {0};
	
	MMIMR_Trace(("mrapp MMIMRAPP_redail ret: %d, APN: %s", ret, s_mr_netContext.apn));

    app_info.app_handler = MMI_MODULE_MRAPP;
    app_info.apn_ptr = (char*)s_mr_netContext.apn;
    app_info.user_name_ptr = (char*)user_ptr;
    app_info.psw_ptr = (char*)passwd_ptr;
    app_info.dual_sys = MMIMRAPP_GetActiveSim();
    app_info.priority = 3;
    app_info.ps_service_rat = MN_UNSPECIFIED;
    app_info.handle_msg_callback = MMIMRAPP_handlePsMsg; /*lint !e64*/
    MNGPRS_ReadStorageExtReqQosEx(MMIMRAPP_GetActiveSim(),MN_GPRS_STORAGE_ALL,IM_E,&app_info.qos_info);

    ret = MMIAPIPDP_Active(&app_info);

    if(ret == TRUE)
	{
		s_mr_netContext.gprsState = MRAPP_GPRS_ACTIVING;
		MMIMRAPP_gprsProtectTimerStart(MMIMRAPP_GPRSPROTECT_TIME_OUT);
	}
	else
	{
		s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
		s_mr_netContext.mrNetActived = FALSE;
		if(s_mr_netContext.gprsCb != NULL)
		{
			MR_INIT_NETWORK_CB callBack;
			callBack = (MR_INIT_NETWORK_CB)s_mr_netContext.gprsCb; /*lint !e611*/
			callBack(MR_FAILED);
			s_mr_netContext.gprsCb = NULL;
		}
	}
}

/*****************************************************************************/
// 	Description : disconnect the gprs launched by mr app
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_CloseMrNet(void)
{
	ERR_MNGPRS_CODE_E err;
	MMIMR_Trace(("mrapp. MMIMRAPP_CloseMrNet() need? :%d, state; %d", s_mr_netContext.mrNetActived,s_mr_netContext.gprsState));
	
#ifndef WIN32
	if(s_mr_netContext.mrNetActived == TRUE)
	{
		MMIMRAPP_gprsProtectTimerStop();//2007-10-8 add
		MMIMRAPP_gprsRetryTimerStop();
		//2007-11-9dis for sys err. close it ignore all !! if(s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVED || s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVING)//acting can ir directly ?
		{
			err = MMIAPIPDP_Deactive(MMI_MODULE_MRAPP); /*lint !e64*/
			MMIMR_Trace(("mrapp MMIAPIPDP_Deactive err: %d", err));
            s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
            s_mr_netContext.mrNetActived = FALSE;
/*modify by zack@20120316 start*/
//没有拨号成功的情况下底层会返回FALSE，此时也应该将状态复位
#if 0
			if(err == TRUE)
            {
				s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVED;
				s_mr_netContext.mrNetActived = FALSE;
			}
            else if(s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVED
				|| s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVING)
			{
				s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVEING;
			}
#endif
/*modify by zack@20120316 end*/
			s_mr_netContext.gprsCb = NULL;

			if(s_mr_hostContext.cb != NULL)
			{
				s_mr_hostContext.cb = NULL;
				s_mr_hostContext.timerCount = 0;
				SCI_MEMSET(s_mr_hostContext.hostBuffer, 0x00, sizeof(s_mr_hostContext.hostBuffer));
				MMK_StopTimer(s_mr_hostContext.getHostTimer);
				s_mr_hostContext.getHostTimer = 0;
			}
		}
	}
#endif
	
}

/*****************************************************************************/
// 	Description : report the rerult to mr app (the caller is run in mmi task)
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_netInitResult(int32 result)
{
	MR_INIT_NETWORK_CB callBack;
	MMIMR_Trace(("mrapp. MMIMRAPP_netInitResult() result :%d", result));
	
	/*2007add, do the wait proccess*/
	if(s_mr_netContext.deactWaitFunc != NULL && result == MR_FAILED)
	{
		MMIMRAPP_WAIT2DOPDPACT_FUNC waitCb;
		waitCb = (MMIMRAPP_WAIT2DOPDPACT_FUNC)s_mr_netContext.deactWaitFunc; /*lint !e611*/
		s_mr_netContext.deactWaitFunc = NULL;
		waitCb(s_mr_netContext.data);
	}
	/*2009-10-22 111,17787 add. for overseas*/
	if(gDsmUseNVdataAccount && result == MR_SUCCESS)
    {
		MMIMRAPP_saveApn();
	}
	/*2009-10-22 111,17787 add end*/	

	MMIMRAPP_gprsRetryTimerStop();
    
    MMIMR_Trace(("mrapp. MMIMRAPP_netInitResult() mrNetActived:%d gprsState:%d", s_mr_netContext.mrNetActived, s_mr_netContext.gprsState));
    if(s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVED || s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVEING)
    {
        return;
    }
    
	if(s_mr_netContext.gprsCb != NULL)
	{
		callBack = (MR_INIT_NETWORK_CB)s_mr_netContext.gprsCb; /*lint !e611*/
		MMIMR_Trace(("mrapp. MMIMRAPP_netInitResult() callBack :0x%08x", callBack));
		callBack(result);
		MMIMR_Trace(("mrapp. MMIMRAPP_netInitResult() callBack end"));
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_SMSfilter(MN_SMS_ALPHABET_TYPE_E  alphabet_type, MN_SMS_USER_VALID_DATA_T *data, MN_CALLED_NUMBER_T *number)
{
	uint8 num[MMISMS_PBNUM_MAX_LEN + 2];
	uint8 bcd_limit_len = MMISMS_PBNUM_MAX_LEN >> 1; 
	uint8 copy_len = 0;
	uint8 user_data_arr[MN_SMS_MAX_USER_VALID_DATA_LENGTH];
	uint8 *content = NULL;
	int32 result = MR_IGNORE;
	int32 type;
	
	if(data == NULL || number == NULL || number->num_len == 0)
		return MR_IGNORE;
	
	copy_len = MIN( bcd_limit_len , number->num_len);
	SCI_MEMSET(num, '\0', sizeof(num));
	MMIAPICOM_GenDispNumber( number->number_type,
		copy_len,
		number->party_num,
		num,
		MMISMS_PBNUM_MAX_LEN + 2);
	
	MMIMR_Trace(("alphabet_type: %d", alphabet_type));
	if(alphabet_type == MN_SMS_DEFAULT_ALPHABET)
	{
		SCI_MEMSET(user_data_arr, '\0', sizeof(user_data_arr));
		MMIAPICOM_Default2Ascii(data->user_valid_data_arr,
			user_data_arr,
			data->length);
		content = user_data_arr;
		type = MR_ENCODE_ASCII;
	}
	else
	{
		content = data->user_valid_data_arr;
		if(alphabet_type == MN_SMS_8_BIT_ALPHBET)
		{
			type = MR_ENCODE_ASCII;
		}
		else
		{
			type = MR_ENCODE_UNICODE;
		}
	}
	
	MMIMR_Trace(("number: %s, clen: %d, type: %d", num, data->length, type));
#ifndef WIN32
	result = mr_smsIndiactionEx(content, data->length, num, type);
#endif
	MMIMR_Trace(("res: %d", result));
	return result;
}

/*****************************************************************************/
// 	Description :获取
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_AudioGetSoundInfo(int32 cmd)
{
	int32 pos = -1;
	
	if(cmd == ACI_TOTALTIME)
	{
		pos = s_mr_audio_mng.totalTime;
	}
	else
	{
		if(s_mr_audio_mng.curStatus == PLAYING)
		{
			MMISRVAUD_PLAY_INFO_T ptPlayInfo = {0};

			MMIMR_Trace(("mrapp MMIMRAPP_AudioGetSoundInfo curAudioHdle: %d", s_mr_audio_mng.audioHdle));
			if(s_mr_audio_mng.audioHdle > INVALID_HANDLE)
			{
				MMISRVAUD_GetPlayingInfo(s_mr_audio_mng.audioHdle, &ptPlayInfo);
				MMIMR_Trace((".uiTotalTime: %d, CurrentTime: %d", ptPlayInfo.total_time, ptPlayInfo.cur_time));
				MMIMR_TraceM((".uiTotalDataLength: %d, Offset: %d", ptPlayInfo.total_data_length, ptPlayInfo.cur_data_offset));
				pos = ptPlayInfo.cur_data_offset;											
			}
		}
		else if(s_mr_audio_mng.curStatus == PAUSE
			|| s_mr_audio_mng.curStatus == LOADED)
		{
			pos = s_mr_audio_mng.curSetPlayPos;
		}
		
		if(pos != -1 && cmd == ACI_PLAYTIME)
		{
			pos = (pos*((s_mr_audio_mng.totalTime<<18)/s_mr_audio_mng.totalLen))>>18;
		}
	}
	
	MMIMR_Trace(("MMIMRAPP_Audio, pos: %d", pos));
	return pos;
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_AudioPlayEnd(void)
{
	s_mr_audio_mng.totalLen = -1;
	s_mr_audio_mng.totalTime = -1;
	s_mr_audio_mng.uiSampleRate = -1;
	s_mr_audio_mng.curSetPlayTime = 0;
}

/*****************************************************************************/
//  Description : Handle audio callback message.
//  Global resource dependence : none
//  Author: zack.zhang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIMRAPP_HandleAudioPlayCBMsg(
                                                MmiAudioCallbackMsgS *callback_info_ptr
                                                )
{
    BOOLEAN         result = FALSE;
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
    HAUDIO          haudiohandle = callback_info_ptr->hAudio;
    AUDIO_RESULT_E  audio_result = (AUDIO_RESULT_E)callback_info_ptr->affix_info;
    
    MMIMR_Trace(("MMIMRAPP_HandleAudioPlayCBMsg haudiohandle = %u audio_result = %d msg_id = %d cb = 0x%08x loop = %d", haudiohandle, 
													audio_result, callback_info_ptr->notify_info, g_audio_info.cb, g_audio_info.loop));

    switch (callback_info_ptr->notify_info)
    {
    case AUDIO_PLAYEND_IND:  
		//播放结束或者播放失败后释放handle
		audio_handle = MMIMRAPP_GetAudioHandle();
			
		if(audio_handle > INVALID_HANDLE)
		{
		    if(g_audio_info.loop == 1) //解决不能循环播放的问题
            {
                break;
            }      
			MMISRVAUD_Stop(audio_handle);
			MMISRVMGR_Free(audio_handle);
			audio_handle = INVALID_HANDLE;
			MMIMRAPP_SetAudioHandle(INVALID_HANDLE);

    		//播放结束后应修改audio的状态
    		if(s_mr_audio_mng.curStatus == PLAYING)
            {
                s_mr_audio_mng.curStatus = LOADED;
            }
            
            if(AUDIO_NO_ERROR == audio_result)
            {            
                if(g_audio_info.cb && (g_audio_info.loop == 0))
                {
                    g_audio_info.cb(ACI_PLAY_COMPLETE);
                    MMIMR_Trace(("MMIMRAPP_HandleAudioPlayCBMsg CallBack End"));
                }
                else
                {
                    mr_event(MR_AUDIO_EVENT, ACI_PLAY_COMPLETE, 0);
                }
            }
            else
            {
                if(g_audio_info.cb)
                {
                    g_audio_info.cb(ACI_PLAY_ERROR);
                }
                else
                {
                    mr_event(MR_AUDIO_EVENT, ACI_PLAY_ERROR, 0);
                }
            }
		}
        break;

    default:
        break;
    }
}

LOCAL BOOLEAN MMIMRAPP_AudioPlayCallback(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;

    if(handle == MMIMRAPP_GetAudioHandle() && PNULL != param)
    {
        MMIMR_Trace(("MMIMRAPP_AudioPlayCallback event = %d", param->event));
        
        switch(param->event)
        {
        case MMISRVMGR_NOTIFY_SUSPEND:
            break;
            
        case MMISRVMGR_NOTIFY_RESUME:
            MMISRVAUD_SetVolume(handle, s_mr_audio_mng.soundVolume);
            break;
            
        case MMISRVMGR_NOTIFY_EXT:
            break;
            
        default:
            break;
        }
    }
    return TRUE;

}

void mr_media_mma_init(void)
{
    int32 i = 0;

    g_mma_ctrl_info.total_channel = 0;
    g_mma_ctrl_info.play_channel  = 0;
    g_mma_ctrl_info.hCodecHandle = 0;
    g_mma_ctrl_info.hExpHandle   = 0;

    for(i=0; i<MR_MMA_CHANNEL_MAX; i++)
    {
        g_mma_ctrl_info.mmaInfo[i].cur   = 0;
        g_mma_ctrl_info.mmaInfo[i].len   = 0;
        g_mma_ctrl_info.mmaInfo[i].loop  = 0;
        g_mma_ctrl_info.mmaInfo[i].handle= -1;
        g_mma_ctrl_info.mmaInfo[i].state = MR_MMA_INIT;
        if(g_mma_ctrl_info.mmaInfo[i].dataBuf != NULL)
        {
            SCI_FREE(g_mma_ctrl_info.mmaInfo[i].dataBuf);
            g_mma_ctrl_info.mmaInfo[i].dataBuf = NULL;
        }
    }
}

int32 mr_media_mma_get_handle(void)
{
    int32 i = 0;
    int32 audioIndex = -1;

    for(i=0; i<MR_MMA_CHANNEL_MAX; i++)
    {
        if(g_mma_ctrl_info.mmaInfo[i].handle == -1)
        {
            audioIndex = i;
            break;
        }
    }

    MMIMR_Trace(("[SKYAUDIO] mr_media_mma_get_handle audioHandle=%d", audioIndex));

    return audioIndex;
}

/*****************************************************************************/
//! \fn LOCAL void  SKYAUDIOMIX_Process(
//!    AUDIO_OBJECT_T *audio_obj_ptr,
//!    int16* psSrcLeftData,
//!    int16* psSrcRightData,
//!    uint32 uiSrcCount,
//!    int16* psDestLeftData,
//!    int16* psDestRightData,
//!    uint32* puiDestCount
//!    )
//! \param audio_obj_ptr audio object.
//! \param psSrcLeftData input left channel data.
//! \param psSrcRightData input right channel data.
//! \param psSrcRightData length of input data.
//! \param psDestLeftData output left channel data.
//! \param psDestRightData output right channel data.
//! \param puiDestCount length of output data.
//! \return void no return
//! \brief  Description:  This function is to mix pcm data. 
//! \author Author:  zack.zhang
//! \note   Note:Nothing.
/*****************************************************************************/
LOCAL void  SKYAUDIOMIX_Process(
                                    AUDIO_OBJECT_T *audio_obj_ptr,
                                    int16* psSrcLeftData,
                                    int16* psSrcRightData,
                                    uint32 uiSrcCount,
                                    int16* psDestLeftData,
                                    int16* psDestRightData,
                                    uint32* puiDestCount
                                    )
{
    int32 i = 1;
    int32 j = 0;
    int32 cur = 0;

    //MMIMR_Trace(("[SKYAUDIO] SKYAUDIOMIX_Process cur=%d len=%d uiSrcCount=%d play_channel=%d", g_mma_ctrl_info.mmaInfo[i].cur, g_mma_ctrl_info.mmaInfo[i].len, uiSrcCount, g_mma_ctrl_info.play_channel));

    if(g_mma_ctrl_info.play_channel == 1)
    {
    	SCI_MEMCPY((int16*)psDestLeftData, (int16*)psSrcLeftData, uiSrcCount*2);
    	SCI_MEMCPY((int16*)psDestRightData, (int16*)psSrcRightData, uiSrcCount*2);
    }
    else
    {
    	SCI_MEMCPY((int16*)psDestLeftData, (int16*)psSrcLeftData, uiSrcCount*2);
    	SCI_MEMCPY((int16*)psDestRightData, (int16*)psSrcRightData, uiSrcCount*2);
    	for (i = 1; i < MR_MMA_CHANNEL_MAX; i++)
        {
            //MMIMR_Trace(("[SKYAUDIO] SKYAUDIOMIX_Process i=%d cur=%d state=%d", i, g_mma_ctrl_info.mmaInfo[i].cur, g_mma_ctrl_info.mmaInfo[i].state));
            if(g_mma_ctrl_info.mmaInfo[i].state != MR_MMA_PLAY)
                continue;
            
            cur = g_mma_ctrl_info.mmaInfo[i].cur;
          
            for (j = 0; j < uiSrcCount; j++)
            {
                if(cur < g_mma_ctrl_info.mmaInfo[i].len/2)
                {
            		psDestLeftData[j] = psDestLeftData[j] +g_mma_ctrl_info.mmaInfo[i].dataBuf[cur];
                    psDestRightData[j]= psDestRightData[j]+g_mma_ctrl_info.mmaInfo[i].dataBuf[cur];
                    if(psDestLeftData[j] > 32767)
                    {
                        psDestLeftData[j] = 32767;
                    }
                    if(psDestLeftData[j] < -32768)
                    {
                        psDestLeftData[j] = -32768;
                    }
                    cur++;
                    g_mma_ctrl_info.mmaInfo[i].cur++;
                }
                else
                {
                    cur = 0;
                    g_mma_ctrl_info.mmaInfo[i].cur = 0;
                    if(g_mma_ctrl_info.mmaInfo[i].loop == 0)
                    {
                        g_mma_ctrl_info.play_channel--;
                        g_mma_ctrl_info.mmaInfo[i].state = MR_MMA_STOP;
                        break;
                    }
                }
            }
        }
    }
    *puiDestCount = uiSrcCount;
}


PUBLIC HAUDIOEXP SKYAUDIOMIX_RegExpPlugger( void )
{
#ifndef WIN32
	AUDIO_EXPRESS_T ptAudioExp = {0};
	HAUDIOEXP hAudioMix;
    
	ptAudioExp.pusExpName = ANSI2UINT16("AUDIOMIX");
	ptAudioExp.pusExpDescribe = SCI_NULL;
	ptAudioExp.process =  SKYAUDIOMIX_Process;
	ptAudioExp.set_para =  SCI_NULL;
	ptAudioExp.init_para = SCI_NULL;
	ptAudioExp.deinit_para = SCI_NULL;
	hAudioMix = AUDIO_PM_RegExpressPlugger(&ptAudioExp);
    MMIMR_Trace(("[SKYAUDIO] SKYAUDIOMIX_RegExpPlugger hAudioMix=%d", hAudioMix));

	return hAudioMix;
#endif
}

int32 mr_media_mma_open(int32 code ,uint8 *input,int32 input_len)
{ 
 	int32 ret = 0; 
    uint32 result = 0;
    uint32 audioIndex = -1;
    uint32 fHandle = 0;
    int32 len = 0;
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
    MMISRVAUD_RING_FMT_E audioType = MMISRVAUD_RING_FMT_MIDI;
    mr_bg_play_info_t *req = (mr_bg_play_info_t*)input;

	if((req == NULL)
	||(input_len < sizeof(mr_bg_play_info_t))
	||(req->data == NULL)
	||(req->len  == 0)
	)
    {   
		return MR_FAILED;
    }

    MMIMR_Trace(("[SKYAUDIO] mr_media_mma_open type=%d data=0x%08x len=%d loop=%d total_channel=%d", code%10, req->data, req->len, req->loop, g_mma_ctrl_info.total_channel));

    if(g_mma_ctrl_info.total_channel == 0)
    {
        mr_media_mma_init();
    }

    g_mma_ctrl_info.total_channel++;

    audioIndex = mr_media_mma_get_handle();

    if(audioIndex == -1)
    {
        g_mma_ctrl_info.total_channel--;
        return MR_FAILED;
    }

    if(g_mma_ctrl_info.total_channel == 1)
    {
        switch(code%10)
        {
        case ACI_AUDIO_MIDI:
            audioType = MMISRVAUD_RING_FMT_MIDI;
            g_mma_ctrl_info.hCodecHandle = hMIDICodec;
            break;
        case ACI_AUDIO_WAV:
            audioType = MMISRVAUD_RING_FMT_WAVE;
            g_mma_ctrl_info.hCodecHandle = hWAVCodec;
            break;
        case ACI_AUDIO_MP3:
            audioType = MMISRVAUD_RING_FMT_MP3;
            g_mma_ctrl_info.hCodecHandle = hMP3Codec;
            break;
        case ACI_AUDIO_AAC:
            audioType = MMISRVAUD_RING_FMT_AAC;
            g_mma_ctrl_info.hCodecHandle = hAACCodec;
            break;
        case ACI_AUDIO_AMR:
            audioType = MMISRVAUD_RING_FMT_AMR;
            g_mma_ctrl_info.hCodecHandle = hAACCodec;
            break;
        default:
            audioType = MMISRVAUD_RING_FMT_WAVE;
            g_mma_ctrl_info.hCodecHandle = hWAVCodec;
            break;
        }
        g_mma_ctrl_info.hExpHandle = SKYAUDIOMIX_RegExpPlugger();    
        result = AUDIO_PM_AddCodecExpress(g_mma_ctrl_info.hCodecHandle, g_mma_ctrl_info.hExpHandle);
        MMIMR_Trace(("[SKYAUDIO] AUDIO_PM_AddCodecExpress result=%d", result));

		//创建handle之前需要将播放次数初始化一次
		s_mr_play_times = 1;
			
		if(req->loop == 1)
        {
            s_mr_play_times = 0xFFFFFFFF;
        }
		
        audio_handle = MMIMRAPP_CreateAudioBufHandle(audioType, req->data, req->len, MMIMRAPP_AudioPlayCallback);
        MMIMR_Trace(("[SKYAUDIO] MMIAUDIO_CreateAudioBufHandle handle = %d", audio_handle));

		if(audio_handle > INVALID_HANDLE)
		{
			MMIMRAPP_SetMixAudioHandle(audio_handle);
		}
    }
    else
    {
        g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf = (int16*)SCI_ALLOCA(req->len);
        if(g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf == NULL)
        {
            SCI_MEMSET(g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf, 0x00, req->len);
            g_mma_ctrl_info.total_channel--;
            return MR_FAILED;
        }
        SCI_MEMCPY((uint8*)g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf, req->data, req->len);
        g_mma_ctrl_info.mmaInfo[audioIndex].len = req->len;
    }

	g_mma_ctrl_info.mmaInfo[audioIndex].loop = req->loop;
    g_mma_ctrl_info.mmaInfo[audioIndex].handle = audioIndex;
    g_mma_ctrl_info.mmaInfo[audioIndex].state  = LOADED;
        
	return (audioIndex+MR_PLAT_VALUE_BASE);
}


int32 mr_media_mma_play(int32 code ,uint8 *input,int32 input_len)
{
	uint32 audioIndex = 0;
    MMISRVAUD_RET_E  ret = MMISRVAUD_RET_MAX;
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;

    if(input != NULL)
    {
        audioIndex = *(int32*)input - MR_PLAT_VALUE_BASE;

        MMIMR_Trace(("[SKYAUDIO]: mr_media_mma_play audioHandle=%d total_channel=%d play_channel=%d", audioIndex, g_mma_ctrl_info.total_channel, g_mma_ctrl_info.play_channel));

        if(audioIndex < 0)
        {
            return MR_FAILED;
        }

        if(g_mma_ctrl_info.total_channel <= 0)
        {
            return MR_FAILED;
        }

        if(g_mma_ctrl_info.mmaInfo[audioIndex].state == MR_MMA_PLAY)
        {
            return MR_SUCCESS;
        }

        g_mma_ctrl_info.play_channel++;
        g_mma_ctrl_info.mmaInfo[audioIndex].state = MR_MMA_PLAY;
        if(g_mma_ctrl_info.total_channel == 1)
        {
			MMIAPISET_SetCurRingType(MMISET_RING_TYPE_OTHER);
			audio_handle = MMIMRAPP_GetMixAudioHandle();
			MMIMR_Trace(("[SKYAUDIO]: MMIAUDIO_AudioPlay audio_handle = %d", audio_handle));

			if(audio_handle > INVALID_HANDLE)
			{
				ret = MMISRVAUD_Play(audio_handle, 0);
				MMISRVAUD_SetVolume(audio_handle, s_mr_audio_mng.soundVolume);
            	MMIMR_Trace(("[SKYAUDIO]: MMIAUDIO_AudioPlay ret=%d volume=%d", ret, s_mr_audio_mng.soundVolume));
			}
        }

    	return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

int32 mr_media_mma_stop(int32 code ,uint8 *input,int32 input_len)
{
    uint32 audioIndex = 0;
	MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;

    if(input != NULL)
    {
        audioIndex = *(int32*)input - MR_PLAT_VALUE_BASE;

        MMIMR_Trace(("[SKYAUDIO]: mr_media_mma_stop audioHandle=%d total_channel=%d play_channel=%d", audioIndex, g_mma_ctrl_info.total_channel, g_mma_ctrl_info.play_channel));

        if(audioIndex < 0)
        {
            return MR_FAILED;
        }

        if(g_mma_ctrl_info.total_channel <= 0)
        {
            return MR_FAILED;
        }

        if(g_mma_ctrl_info.mmaInfo[audioIndex].state == MR_MMA_PLAY)
        {
        	g_mma_ctrl_info.play_channel--;
        	g_mma_ctrl_info.mmaInfo[audioIndex].cur   = 0;
        	g_mma_ctrl_info.mmaInfo[audioIndex].state = MR_MMA_STOP;
    	}
        
        if(g_mma_ctrl_info.total_channel == 1)
        {
			audio_handle = MMIMRAPP_GetMixAudioHandle();

			if(audio_handle > INVALID_HANDLE)
			{
			 	MMISRVAUD_Stop(audio_handle);
			 	MMISRVMGR_Free(audio_handle);
			}
        }

    	return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

int32 mr_media_mma_close(int32 code ,uint8 *input,int32 input_len)
{
    int32  ret = 0;
    uint32 audioIndex = 0;
    AUDIO_RESULT_E result = 0;

    if(input != NULL)
    {
        audioIndex = *(int32*)input - MR_PLAT_VALUE_BASE;

        MMIMR_Trace(("[SKYAUDIO]: mr_media_mma_close audioIndex=%d total_channel=%d", audioIndex, g_mma_ctrl_info.total_channel));

        if(audioIndex < 0)
        {
            return MR_FAILED;
        }

        if(g_mma_ctrl_info.total_channel <= 0)
        {
            return MR_FAILED;
        }

        g_mma_ctrl_info.total_channel--;
        g_mma_ctrl_info.mmaInfo[audioIndex].state  = MR_MMA_CLOSE;
        g_mma_ctrl_info.mmaInfo[audioIndex].handle = -1;
        g_mma_ctrl_info.mmaInfo[audioIndex].len = 0;
        g_mma_ctrl_info.mmaInfo[audioIndex].loop= 0;
        if(g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf != NULL)
        {
            SCI_FREE(g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf);
            g_mma_ctrl_info.mmaInfo[audioIndex].dataBuf = NULL;
        }

        if(g_mma_ctrl_info.total_channel == 0)
        {
            result = AUDIO_PM_UnRegExpressPlugger(g_mma_ctrl_info.hExpHandle);
            MMIMR_Trace(("[SKYAUDIO] AUDIO_PM_UnRegExpressPlugger result=%d", result));
            
            result = AUDIO_PM_DeleteCodecExpress(g_mma_ctrl_info.hCodecHandle, g_mma_ctrl_info.hExpHandle);
            MMIMR_Trace(("[SKYAUDIO] AUDIO_PM_DeleteCodecExpress result=%d", result));
        }

    	return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }

}

/*****************************************************************************/
// 	Description :audio 命令接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_AudioCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 resValue = MR_FAILED;
	int32 cmd;
	int type;
	MMIMR_TraceM(("MMIMRAPP_AudioCmd() :%d", code));

    switch(code)
    {
#ifdef __MR_CFG_CAMERA_SUPPORT__
    case PLATEX_CAMERA_PREVIEW:
        return mr_camera_start_preview(input, input_len, output, output_len, cb);
    case PLATEX_CAMERA_CAPTURE:
        return mr_camera_capture(input, input_len, output, output_len, cb);
    case PLATEX_CAMERA_GETDATA:
        return mr_camera_get_preview_data(input, input_len, output, output_len, cb);
#endif

#ifdef __MR_RECORD_SUPPORT__
    case MR_VOICE_RECORD_START:
        {
        if(input_len < sizeof(MR_T_VOICE_REC_REQ)) /*lint !e574 !e737*/
            return MR_FAILED;
        return MMIMRAPP_RecordStart((MR_T_VOICE_REC_REQ*)input);
        }
#endif

	default:
		break;
    }

	cmd = (code/10)%200;
	type = code%10;
	switch(type)
	{
	case ACI_AUDIO_MIDI:
		type = MMISRVAUD_RING_FMT_MIDI;
		break;
	case ACI_AUDIO_WAV:
		type = MMISRVAUD_RING_FMT_WAVE;
		break;
	case ACI_AUDIO_MP3:
		type = MMISRVAUD_RING_FMT_MP3;
		break;       
	case ACI_AUDIO_AMR:
		type = MMISRVAUD_RING_FMT_AMR;
		break;
	case ACI_AUDIO_AAC:
		type = MMISRVAUD_RING_FMT_AAC;
		break;
	case ACI_AUDIO_M4A:
		type = MMISRVAUD_RING_FMT_M4A;
		break;
	default:
		MMIMR_Trace(("type err !!! : %d", type));
		//return MR_IGNORE;
		break;
	}
	
	MMIMR_Trace(("s_mr_audio_mng.curStatus: %d, type :%d", s_mr_audio_mng.curStatus, type));	
	switch(cmd)
	{
	case ACI_OPEN://设备初始化
		if(s_mr_audio_mng.curStatus > LOADED && s_mr_audio_mng.curStatus <= PAUSE)
		{
			MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
			audio_handle = MMIMRAPP_GetAudioHandle();
			
			if(audio_handle > INVALID_HANDLE)
			{
				MMISRVAUD_Stop(audio_handle);
				MMISRVMGR_Free(audio_handle);
				audio_handle = INVALID_HANDLE;
				MMIMRAPP_SetAudioHandle(INVALID_HANDLE);
			}
		}

		MMIMR_Trace(("s_mr_audio_mng.soundVolume: %d", s_mr_audio_mng.soundVolume));
		s_mr_audio_mng.curStatus = INITED;
		MMIMRAPP_AudioPlayEnd();
		resValue = MR_SUCCESS;
		break;
        
	case ACI_LOAD_FILE://加载一个文件
		if(s_mr_audio_mng.curStatus >= INITED && s_mr_audio_mng.curStatus <= PAUSE)
		{
			int len = strlen((char*)input);
			uint16 full_path_len = 0;
			uint16 ucs2_name_len = 0;
			const uint8 *devName;
			uint16 devNameLen;
			
			MMIMR_Trace(("MMIMRAPP_AudioCmd input_file: %s", (char*)input));
			if(len < MMIMRAPP_FILE_NAME_MAX_LEN)
			{
				devName = MMIMRAPP_getCurDevName(&devNameLen);
				if(devName != NULL)
				{
					MMIMRAPP_ConvertPath2Pc((char *)input, strlen((char*)input));
					memset(full_path, 0, sizeof(full_path));
					memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));
					ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)input, strlen((char*)input));
					if(MMIFILE_CombineFullPath_fix(
						(const uint8*)devName, devNameLen,
						(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
						(const uint8*)ucs2_name_arr, ucs2_name_len,
						(uint8*)full_path,
						&full_path_len
						)
						)
					{
						memset(s_mr_audio_filename, 0, sizeof(s_mr_audio_filename));
						s_mr_audio_mng.name = (uint8*)s_mr_audio_filename;
						SCI_MEMCPY(s_mr_audio_mng.name, full_path, full_path_len);
						s_mr_audio_mng.ring_type = type;
						s_mr_audio_mng.src_type = SRC_NAME;
						if(s_mr_audio_mng.curStatus != PAUSE)
						{
							s_mr_audio_mng.curStatus = LOADED;
						}
						resValue = MR_SUCCESS;
					}
				}
			}
		}
		break;
        
	case ACI_LOAD_BUF://加载缓冲数据
		if(s_mr_audio_mng.curStatus >= INITED && s_mr_audio_mng.curStatus <= PAUSE && s_mr_audio_mng.curStatus != PAUSE)
		{	
			MRAPP_AUDIO_BUF_T *buf = (MRAPP_AUDIO_BUF_T*)input;
			MMIMR_Trace(("buf addr: 0x%x, input_len: %d ", buf->buf, buf->buf_len));
			
			s_mr_audio_mng.ring_type = type;
			s_mr_audio_mng.src_type = SRC_STREAM;
			s_mr_audio_mng.name = (uint8*)buf->buf;
			s_mr_audio_mng.dataLen = buf->buf_len;
			if(s_mr_audio_mng.curStatus != PAUSE)
			{
				s_mr_audio_mng.curStatus = LOADED;
			}
			resValue = MR_SUCCESS;
		}
		break;
        
	case ACI_PLAY://从当前的位置开始播放
		{
			MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
			MMISRVAUD_PLAY_INFO_T ptPlayInfo;
			MMISRVAUD_RET_E play_res = MMISRVAUD_RET_MAX;
			MMIMR_Trace(("src_type: %d, ring_type: %d", s_mr_audio_mng.src_type, s_mr_audio_mng.ring_type));

			//每次播放的时候将播放次数初始化一次
			s_mr_play_times = 1;
			
			if(s_mr_audio_mng.curStatus == LOADED || s_mr_audio_mng.curStatus == PAUSE)
			{
                mr_audio_play_info_t *playReq = NULL;
                if(input_len == sizeof(mr_audio_play_info_t) && input != NULL)
                {
                    playReq = (mr_audio_play_info_t *)input;
                    g_audio_info.cb = playReq->cb;
                    g_audio_info.loop = playReq->loop;
                    g_audio_info.block = playReq->block;
                    if(playReq->loop)
                    {
                        s_mr_play_times = 0xFFFFFFFF;
                    }                        
                }
			
				if(s_mr_audio_mng.src_type == SRC_STREAM)
				{						
					audio_handle = MMIMRAPP_CreateAudioBufHandle(s_mr_audio_mng.ring_type, s_mr_audio_mng.name, s_mr_audio_mng.dataLen, MMIMRAPP_AudioPlayCallback);
				}
				else
				{
					audio_handle = MMIMRAPP_CreateAudioFileHandle(s_mr_audio_mng.ring_type, (const wchar *)s_mr_audio_mng.name, MMIMRAPP_AudioPlayCallback);
				}
				
				MMIMR_Trace(("MMIMRAPP_AudioCmd create audio_handle: %d", audio_handle));
				if(audio_handle > INVALID_HANDLE)
				{
					MMIMRAPP_SetAudioHandle(audio_handle);
					MMIMR_TraceM(("MRAPP_AudioCmd play started pos: %d", s_mr_audio_mng.curSetPlayPos));
					play_res = MMISRVAUD_Play(audio_handle, s_mr_audio_mng.curSetPlayPos);
					MMIMR_Trace(("MRAPP_AudioCmd play_res: %d", play_res));

					if(MMISRVAUD_RET_OK == play_res)
					{
						MMISRVAUD_GetPlayingInfo(audio_handle, &ptPlayInfo);
						
						s_mr_audio_mng.curStatus = PLAYING;
						s_mr_audio_mng.totalLen = ptPlayInfo.total_data_length;
						s_mr_audio_mng.totalTime = ptPlayInfo.total_time;
						
						MMIMR_Trace(("MRAPP_AudioCmd audioHdle: %d", s_mr_audio_mng.audioHdle));
						MMIMR_Trace(("MRAPP_AudioCmd uiTotalTime: %d, CurrentTime: %d", ptPlayInfo.total_time, ptPlayInfo.cur_time));
						MMIMR_TraceM(("MRAPP_AudioCmd uiTotalDataLength: %d, Offset: %d", ptPlayInfo.total_data_length, ptPlayInfo.cur_data_offset));
						
						resValue = MR_SUCCESS;
					}
				}
				else
				{
					MMISRVMGR_Free(audio_handle);
					audio_handle = INVALID_HANDLE;
					MMIMRAPP_SetAudioHandle(INVALID_HANDLE);
				}
			}
		}	
		break;
        
	case ACI_PAUSE://暂停播放
#if 1
		if(s_mr_audio_mng.curStatus == PLAYING
			&& s_mr_audio_mng.audioHdle > INVALID_HANDLE)
		{
			MMISRVAUD_Pause(s_mr_audio_mng.audioHdle);
			s_mr_audio_mng.curStatus = PAUSE;
            resValue = MR_SUCCESS;
		}
		else
		{
			resValue = MR_FAILED;
		}
#else
		if(s_mr_audio_mng.curStatus == PLAYING)
		{
			BOOLEAN isPlaying;
			HAUDIO curAudioHdle = INVALID_HANDLE;
			
			isPlaying= MMIAUDIO_GetIsPlaying();
			MMIMR_Trace(("isPlaying: %d", isPlaying));
			if(isPlaying)
			{
				if(MRAPP_AUDIO_PLAYERTYPE == MMIAUDIO_GetAudioHtype())
				{
					curAudioHdle = MMIAUDIO_GetAudioHandle(MRAPP_AUDIO_PLAYERTYPE);
					MMIMR_Trace(("audioHdle: %d", curAudioHdle));
					if(s_mr_audio_mng.audioHdle == curAudioHdle)
					{
						AUDIO_PLAYINFO_T ptPlayInfo;
						MMIAUDIO_AudioGetPlayingFileInfo(MRAPP_AUDIO_PLAYERTYPE, &ptPlayInfo);				
						MMIAUDIO_AudioStop(MRAPP_AUDIO_PLAYERTYPE);
						MMIAUDIO_CloseAudioHandle(MRAPP_AUDIO_PLAYERTYPE);
						s_mr_audio_mng.curStatus = PAUSE;
						s_mr_audio_mng.curSetPlayPos = ptPlayInfo.uiCurrentDataOffset;
						MMIMR_TraceM(("pause Pos: %d", s_mr_audio_mng.curSetPlayPos));
						s_mr_audio_mng.audioHdle = INVALID_HANDLE;	
						resValue = MR_SUCCESS;
					}						
				}		
			}
			MMIAUDIO_EnableKeyRing(MMIAUDIO_KEY_ALL, MMIBGPLAY_MODULE_NES, TRUE);
		}
#endif
		break;

	case ACI_RESUME://继续播放
#if 1
		if(s_mr_audio_mng.curStatus == PAUSE
			&& s_mr_audio_mng.audioHdle > INVALID_HANDLE)
		{
			MMISRVAUD_Resume(s_mr_audio_mng.audioHdle);
			s_mr_audio_mng.curStatus = PLAYING;
            resValue = MR_SUCCESS;
		}
		else
		{
			resValue = MR_FAILED;
		}
#else
		if(s_mr_audio_mng.curStatus == PAUSE)
		{
			resValue = MMIMRAPP_AudioCmd(2043, NULL, 0, NULL, NULL, NULL);
			resValue = MR_SUCCESS;
		}
#endif
		break;

	case ACI_STOP://停止播放
		{
			BOOLEAN stop_res = FALSE;
			BOOLEAN close_res = FALSE;
			MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
			audio_handle = MMIMRAPP_GetAudioHandle();
			MMIMR_Trace(("audioHdle: %d", audio_handle));
			MMIMR_Trace(("audioStatus: %d", s_mr_audio_mng.curStatus));
		
			if(s_mr_audio_mng.curStatus >= PLAYING && s_mr_audio_mng.curStatus <= PAUSE)
			{
				if(audio_handle > INVALID_HANDLE)
				{
					stop_res = MMISRVAUD_Stop(audio_handle);
					close_res= MMISRVMGR_Free(audio_handle);
					audio_handle = INVALID_HANDLE;
					MMIMRAPP_SetAudioHandle(INVALID_HANDLE);
					MMIMR_Trace(("audio_stop stop_res: %d, close_res : %d", audio_handle));
					
					if(stop_res && close_res)
					{
						s_mr_audio_mng.curSetPlayPos = 0;
						s_mr_audio_mng.curStatus = LOADED;
						MMIMRAPP_AudioPlayEnd();
						resValue = MR_SUCCESS;
					}
				}
			}
		}
		break;
        
	case ACI_CLOSE://关闭设备
		if(s_mr_audio_mng.curStatus >= INITED && s_mr_audio_mng.curStatus <= PAUSE)
		{
			if(s_mr_audio_mng.curStatus >= PLAYING && s_mr_audio_mng.curStatus <= PAUSE)
			{
				MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
				audio_handle = MMIMRAPP_GetAudioHandle();
				
				if(audio_handle > INVALID_HANDLE)
				{
					MMISRVAUD_Stop(audio_handle);
					MMISRVMGR_Free(audio_handle);
					audio_handle = INVALID_HANDLE;
					MMIMRAPP_SetAudioHandle(INVALID_HANDLE);
				}
			}
			s_mr_audio_mng.curStatus = IDLE;
			MMIMRAPP_AudioPlayEnd();
			resValue = MR_SUCCESS;
		}
		break;
        
	case ACI_STATUS://获取当前设备的状态
		MMIMR_Trace(("audio get_status : %d", s_mr_audio_mng.curStatus));
		
		resValue = MR_PLAT_VALUE_BASE + s_mr_audio_mng.curStatus;
		break;
        
	case ACI_POSITION_TIME://设置播放位置，已经播放了多少时间
	case ACI_POSITION_LEN://设置播放位置	
		MMIMR_Trace(("total len:%d total time: %d", s_mr_audio_mng.totalLen, s_mr_audio_mng.totalTime));
		
		if(input != NULL && s_mr_audio_mng.curStatus >= LOADED && s_mr_audio_mng.curStatus <= SUSPENDED)
		{
			BOOLEAN stop_res = FALSE;
			BOOLEAN close_res = FALSE;
			MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;	
			uint32 len = ((MRAPP_AUDIO_OFFSET_T*)input)->pos;
			audio_handle = MMIMRAPP_GetAudioHandle();
			if(s_mr_audio_mng.curStatus != LOADED && s_mr_audio_mng.curStatus != SUSPENDED)
			{
				MMIMR_Trace(("current status: %d", s_mr_audio_mng.curStatus));	
				if(audio_handle > INVALID_HANDLE)
				{
					stop_res = MMISRVAUD_Stop(audio_handle);
					close_res = MMISRVMGR_Free(audio_handle);
					audio_handle = INVALID_HANDLE;
					MMIMRAPP_SetAudioHandle(INVALID_HANDLE);
				}
			}

			if(stop_res && close_res)
			{
				if(cmd == ACI_POSITION_TIME)
				{
    				if(s_mr_audio_mng.totalTime <= 0)
                	{
                    	return MR_FAILED;
                	}         
					len = (len*((s_mr_audio_mng.totalLen<<8)/s_mr_audio_mng.totalTime))>>8;
				}
			
				s_mr_audio_mng.curSetPlayPos = len;
				s_mr_audio_mng.curStatus = LOADED;
				MMIMR_Trace(("uiOffset: %d, input: %d", s_mr_audio_mng.curSetPlayPos, ((MRAPP_AUDIO_OFFSET_T*)input)->pos));
				resValue = MR_SUCCESS;
			}
		}
		break;
        
	case ACI_TOTALTIME://获取整首歌的播放时间			
		{
			BOOLEAN res = FALSE;
			MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
            MMISRVAUD_PLAY_INFO_T   audio_info = {0};
            MRAPP_AUDIO_OFFSET_T *total_info = (MRAPP_AUDIO_OFFSET_T*)tempTransBuf;
            SCI_MEMSET(total_info, 0x00, sizeof(MRAPP_AUDIO_OFFSET_T));
			if(output != NULL && output_len != NULL)
            {
                if(s_mr_audio_mng.curStatus == LOADED)
                {
    				if(s_mr_audio_mng.src_type == SRC_STREAM)
    				{						
    					audio_handle = MMIMRAPP_CreateAudioBufHandle(s_mr_audio_mng.ring_type, s_mr_audio_mng.name, s_mr_audio_mng.dataLen, MMIMRAPP_AudioPlayCallback);
    				}
    				else
    				{
    					audio_handle = MMIMRAPP_CreateAudioFileHandle(s_mr_audio_mng.ring_type, (const wchar *)s_mr_audio_mng.name, MMIMRAPP_AudioPlayCallback);
    				}
                    
                    if(audio_handle > INVALID_HANDLE)
                    {
						MMIMRAPP_setSoundVolume(s_mr_audio_mng.soundVolume);
						MMISRVAUD_Play(audio_handle, 0);
            			res= MMISRVAUD_GetPlayingInfo(audio_handle, &audio_info);

						if(res)
						{
							MMISRVAUD_Stop(audio_handle);
                        	MMISRVMGR_Free(audio_handle);
							audio_handle= INVALID_HANDLE;
            				MMIMR_Trace(("uiTotalDataLength: %d, uiTotalTime: %d", audio_info.total_data_length, audio_info.total_time));
        					total_info->pos = audio_info.total_time;
        					*output = (uint8*)total_info;
        					*output_len = sizeof(MRAPP_AUDIO_OFFSET_T);
        					resValue = MR_SUCCESS;
						}
                    }
                }
                else if(s_mr_audio_mng.curStatus >= PLAYING)
                {
                    total_info->pos = s_mr_audio_mng.totalTime;
      				*output = (uint8*)total_info;
    				*output_len = sizeof(MRAPP_AUDIO_OFFSET_T);
                    MMIMR_Trace(("uiTotalDataLength: %d, uiTotalTime: %d", s_mr_audio_mng.totalLen, s_mr_audio_mng.totalTime));
    				resValue = MR_SUCCESS;
                }
			}
		}
        break;
        
	case ACI_PLAYTIME://获取当前的播放进度	
    	if(output != NULL && output_len != NULL)
        {   
    		if(s_mr_audio_mng.curStatus >= PLAYING)
    		{
				MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;	
                MRAPP_AUDIO_OFFSET_T *cur_info = (MRAPP_AUDIO_OFFSET_T *)tempTransBuf;
                MMISRVAUD_PLAY_INFO_T ptPlayInfo = {0};
                SCI_MEMSET(cur_info, 0x00, sizeof(MRAPP_AUDIO_OFFSET_T));
				
				audio_handle = MMIMRAPP_GetAudioHandle();
				MMIMR_Trace(("mmimrapp audio audio_handle %d", audio_handle));
				 
				if(audio_handle > INVALID_HANDLE)
				{
                	MMISRVAUD_GetPlayingInfo(s_mr_audio_mng.audioHdle, &ptPlayInfo);
                	MMIMR_Trace(("cur time: %d offset: %d", ptPlayInfo.cur_time, ptPlayInfo.cur_data_offset));
                	cur_info->pos = ptPlayInfo.cur_time;
                	*output = (uint8*)cur_info;
                	resValue = MR_SUCCESS;
				}
    		}	
            else
            {
                MRAPP_AUDIO_OFFSET_T *cur_info = (MRAPP_AUDIO_OFFSET_T *)tempTransBuf;
                SCI_MEMSET(cur_info, 0x00, sizeof(MRAPP_AUDIO_OFFSET_T));
                cur_info->pos = 0;
                *output = (uint8*)cur_info;
                resValue = MR_SUCCESS;
            }
        }
        break;

       case ACI_PLAYTIME_MS://获取当前的播放进度(ms)
        	if(output != NULL && output_len != NULL)
            {   
        		if(s_mr_audio_mng.curStatus >= PLAYING)
        		{
					MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
                    MRAPP_AUDIO_OFFSET_T *cur_info = (MRAPP_AUDIO_OFFSET_T *)tempTransBuf;
                    MMISRVAUD_PLAY_INFO_T ptPlayInfo = {0};
                    BOOLEAN result = AUDIO_NO_ERROR;
                    SCI_MEMSET(cur_info, 0x00, sizeof(MRAPP_AUDIO_OFFSET_T));

					audio_handle = MMIMRAPP_GetAudioHandle();
					MMIMR_Trace(("mmimrapp audio audio_handle %d", audio_handle));

					if(audio_handle > INVALID_HANDLE)
					{
                    	MMISRVAUD_GetPlayingInfo(s_mr_audio_mng.audioHdle, &ptPlayInfo);
                    	MMIMR_Trace(("cur time: %d offset: %d", ptPlayInfo.cur_time, ptPlayInfo.cur_data_offset));
                    	cur_info->pos = ptPlayInfo.cur_time*1000;
                    	*output = (uint8*)cur_info;
                    	resValue = MR_SUCCESS;
					}
        		}
                else
                {
                    MRAPP_AUDIO_OFFSET_T *cur_info = (MRAPP_AUDIO_OFFSET_T *)tempTransBuf;
                    SCI_MEMSET(cur_info, 0x00, sizeof(MRAPP_AUDIO_OFFSET_T));
                    cur_info->pos = 0;
                    *output = (uint8*)cur_info;
                    resValue = MR_SUCCESS;
                }
            }
            break;

    case ACI_SETTIME_MS:
        {
			BOOLEAN res = FALSE;
			MMISRV_HANDLE_T audio_handle = INVALID_HANDLE;
            uint32 len = ((MRAPP_AUDIO_OFFSET_T*)input)->pos;

			audio_handle = MMIMRAPP_GetAudioHandle();
			MMIMR_Trace(("audio audio_handle %d", audio_handle));

			if(audio_handle > INVALID_HANDLE)
			{
				res = MMISRVAUD_SeekByTime(s_mr_audio_mng.audioHdle, len);
            	MMIMR_Trace(("result: %d", res));
			
            	if(res)
            	{
                	resValue = MR_SUCCESS;
            	}
            	else
            	{
                	resValue = MR_FAILED;
            	}
			}
        }
        break;
		
	case ACI_PLAYLEN://获取当前的播放进度
		{
			int32 len = -1;			
			len = MMIMRAPP_AudioGetSoundInfo(cmd);
			
			MMIMR_Trace(("pos: %d", len));
			if(len != -1 && output != NULL && output_len != NULL)
			{
				s_mr_audio_mng.curDataPos.pos = len;
				*output = (uint8*)&s_mr_audio_mng.curDataPos;
				*output_len = sizeof(MRAPP_AUDIO_OFFSET_T);
				resValue = MR_SUCCESS;
			}
		}
		break;

    case ACI_MULTIPATH_OPEN:
        return mr_media_mma_open(code, input, input_len);
        
    case ACI_MULTIPATH_PLAY:
        return mr_media_mma_play(code, input, input_len);
        
    case ACI_MULTIPATH_STOP:
        return mr_media_mma_stop(code, input, input_len);
        
    case ACI_MULTIPATH_CLOSE:
        return mr_media_mma_close(code, input, input_len);

	default:
		resValue = MR_IGNORE;//命令不支持
		break;
	}
	
	MMIMR_TraceM(("MMIMRAPP_AudioCmd().resValue: %d", resValue));
	return resValue;
}

#ifdef MRAPP_SIMUGAME_OPEN
/*add by zack@20120711 start*/
void MMIMRAPP_SetAudioDevice(void)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    AUDIO_DEVICE_MODE_TYPE_E cur_dev_mode = 0;
    AUDIO_DEVICE_MODE_TYPE_E set_dev_mode = AUDIO_DEVICE_MODE_HANDHOLD;

    if(MMISRVAUD_IsHeadSetPlugIn())
    {
        set_dev_mode = AUDIO_DEVICE_MODE_EARPHONE;
    }
    else
    {
        set_dev_mode = AUDIO_DEVICE_MODE_HANDFREE;
    }
    
    if(AUDIO_GetDevMode(&cur_dev_mode) == AUDIO_NO_ERROR) 
    {
        if(set_dev_mode != cur_dev_mode)
        {
            result = AUDIO_SetDevMode(set_dev_mode);
        }
    }
}
/*add by zack@20120711 end*/
/*****************************************************************************/
// 	Description :pcm 命令接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_PcmCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 cmd, resValue = MR_SUCCESS;
	MMIMR_TraceM(("MMIMRAPP_PcmCmd() :%d, curStatus: %d", code,s_mr_pcm_mng.curStatus));
	
#ifndef WIN32
#ifdef EXTPCM	
	cmd = (code/10)%200;
	switch(cmd)
	{
	case ACI_OPEN://设备初始化
		if(s_mr_pcm_mng.curStatus == IDLE)
		{
			ExtPCM_SoundInit(15*1024, 8000);
			ExtPCM_SoundOpen(15*1024, 8000);
			s_mr_pcm_mng.curStatus = PLAYING;
		}	
		break;
	case ACI_LOAD_BUF://加载缓冲数据
		if(s_mr_pcm_mng.curStatus == PLAYING && input != NULL)
		{	
			MRAPP_AUDIO_BUF_T *buf = (MRAPP_AUDIO_BUF_T*)input;
			MMIMR_Trace(("buf addr: 0x%x, input_len: %d ", buf->buf, buf->buf_len));
			MMIMRAPP_SetAudioDevice(); //add by zack@20120711
			ExtPCM_SoundOutput(buf->buf_len/2, (int16 *)buf->buf);
			s_mr_pcm_mng.curStatus = PLAYING;
		}
		else
		{
			resValue = MR_FAILED;
		}				
		break;
	case ACI_PLAY://从当前的位置开始播放
		//do nothing
		break;
	case ACI_STOP://停止播放
	case ACI_CLOSE://关闭设备
		if(s_mr_pcm_mng.curStatus == PLAYING)
		{
			ExtPCM_SoundClose();
			s_mr_pcm_mng.curStatus = IDLE;
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
	case ACI_STATUS://获取当前设备的状态				
		resValue = MR_PLAT_VALUE_BASE + s_mr_pcm_mng.curStatus;			
		break;
	case ACI_PLAYLEN://获取还剩多少数据没有播放
		if(s_mr_pcm_mng.curStatus == PLAYING && output != NULL && output_len != NULL)
		{
			s_mr_pcm_mng.dataPos.pos = 2*ExtPCM_SoundGetPCMNum();//get left bytes
			MMIMR_Trace(("left: %d", s_mr_pcm_mng.dataPos.pos));
			*output = (uint8*)&s_mr_pcm_mng.dataPos;
			*output_len = sizeof(MRAPP_AUDIO_OFFSET_T);
			*cb = (MR_PLAT_EX_CB)NULL;		
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
	default:
		resValue = MR_IGNORE;//命令不支持
		break;
	}	
#endif
#endif
	
	MMIMR_TraceM(("MMIMRAPP_PcmCmd(). code: %d, resValue: %d", code, resValue));
	return resValue;
}
#endif

/*****************************************************************************/
// 	Description : mp3作为来电铃声或者闹钟铃声
//	Global resource dependence : none
//  Author: qugp
//	Note://对应当前系统做法:只认U盘中的MP3目录下铃声可以设置
/*****************************************************************************/
LOCAL int32 MMIMRAPP_SetMp3AsRing(MR_RING_SET_T *info)
{
	int32 resValue = MR_FAILED;
	SFS_DATE_T modify_date;
	SFS_TIME_T modify_time;
	
	return resValue;
}

int32 MMMRAPP_FileMgrInit(void)
{
    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        return MR_FAILED;
    }
    FMGR_FILTER_INIT(s_mr_file_filter_type);
    return MR_SUCCESS;
}

int32 MMMRAPP_FileMgrSetFilter(int32 type)
{
    int32  ret = MR_SUCCESS;
    uint32 plat_file_type = 0;
    MMIMR_Trace(("mrapp MMMRAPP_FileMgrSetFilter type:%d", type));

    switch(type)
    {
	case MR_FMGR_TYPE_ALL:
        plat_file_type = MMIFMM_FILE_ALL;
		break;
	case MR_FMGR_TYPE_FOLDER:
        plat_file_type = MMIFMM_FILE_ALL;
		break;
	case MR_FMGR_TYPE_BMP:
        plat_file_type = MMIFMM_PICTURE_BMP;
		break;
	case MR_FMGR_TYPE_JPG:
        plat_file_type = MMIFMM_PICTURE_JPG;
		break;
	case MR_FMGR_TYPE_JPEG:
        plat_file_type = MMIFMM_PICTURE_JPG;
		break;
	case MR_FMGR_TYPE_GIF:
        plat_file_type = MMIFMM_PICTURE_GIF;
		break;
	case MR_FMGR_TYPE_PNG:
        plat_file_type = MMIFMM_PICTURE_PNG;
		break;
	case MR_FMGR_TYPE_WBMP:
         plat_file_type = MMIFMM_PICTURE_WBMP;
		break;
	case MR_FMGR_TYPE_MID:
        plat_file_type = MMIFMM_MUSIC_MID;
		break;
	case MR_FMGR_TYPE_MIDI:
        plat_file_type = MMIFMM_MUSIC_MIDI;
		break;
	case MR_FMGR_TYPE_WAV:
        plat_file_type = MMIFMM_MUSIC_WAV;
		break;
	case MR_FMGR_TYPE_AMR:
        plat_file_type = MMIFMM_MUSIC_AMR;
		break;
	case MR_FMGR_TYPE_AAC:
        plat_file_type = MMIFMM_MUSIC_ACC;
		break;
	case MR_FMGR_TYPE_WMA:
        plat_file_type = MMIFMM_MUSIC_WMA;
		break;
	case MR_FMGR_TYPE_3GP:
        plat_file_type = MMIFMM_MOVIE_3GP;
		break;
	case MR_FMGR_TYPE_MP4:
        plat_file_type = MMIFMM_MOVIE_MP4;
		break;
	case MR_FMGR_TYPE_AVI:	
        plat_file_type = MMIFMM_MOVIE_AVI;
		break;
	case MR_FMGR_TYPE_JAD:
        plat_file_type = MMIFMM_JAVA_JAD;
		break;
	case MR_FMGR_TYPE_JAR:
        plat_file_type = MMIFMM_JAVA_JAR;
		break;
	case MR_FMGR_TYPE_MRP:
        plat_file_type = MMIFMM_MRAPP_MRP;
		break;
#ifdef MRAPP_SIMUGAME_OPEN
	case MR_FMGR_TYPE_NES:
        plat_file_type = MMIFMM_MRAPP_NES;
		break;
#endif
	case MR_FMGR_TYPE_DAF:
	case MR_FMGR_TYPE_VM:
	case MR_FMGR_TYPE_AWB:
	case MR_FMGR_TYPE_AIF:
	case MR_FMGR_TYPE_AIFF:
	case MR_FMGR_TYPE_AIFC:
	case MR_FMGR_TYPE_AU:
	case MR_FMGR_TYPE_SND:
	case MR_FMGR_TYPE_M4A:
	case MR_FMGR_TYPE_MMF:
	case MR_FMGR_TYPE_WBM:
	case MR_FMGR_TYPE_IMY:
	case MR_FMGR_TYPE_VCF:
	case MR_FMGR_TYPE_VCS:
	case MR_FMGR_TYPE_THEME:
#ifndef MRAPP_SIMUGAME_OPEN
	case MR_FMGR_TYPE_NES:
#endif
	case MR_FMGR_TYPE_ZIP:	
	case MR_FMGR_TYPE_ZPK:
	case MR_FMGR_TYPE_EMS:
	case MR_FMGR_TYPE_ANM:
	case MR_FMGR_TYPE_FOLDER_DOT:
	case MR_FMGR_TYPE_UNKNOW:
        return MR_IGNORE;
	default:
		break;
	}

    FMGR_FILTER_SET(s_mr_file_filter_type, plat_file_type);

    return ret;
}

int32 MMMRAPP_FileMgrCleanFilter(int32 type)
{
    int32  ret = MR_SUCCESS;
    uint32 plat_file_type = 0;
    MMIMR_Trace(("mrapp MMMRAPP_FileMgrCleanFilter type:%d", type));

    switch(type)
    {
	case MR_FMGR_TYPE_ALL:
        plat_file_type = MMIFMM_FILE_ALL;
		break;
	case MR_FMGR_TYPE_BMP:
        plat_file_type = MMIFMM_PICTURE_BMP;
		break;
	case MR_FMGR_TYPE_JPG:
        plat_file_type = MMIFMM_PICTURE_JPG;
		break;
	case MR_FMGR_TYPE_JPEG:
        plat_file_type = MMIFMM_PICTURE_JPG;
		break;
	case MR_FMGR_TYPE_GIF:
        plat_file_type = MMIFMM_PICTURE_GIF;
		break;
	case MR_FMGR_TYPE_PNG:
        plat_file_type = MMIFMM_PICTURE_PNG;
		break;
	case MR_FMGR_TYPE_WBMP:
         plat_file_type = MMIFMM_PICTURE_WBMP;
		break;
	case MR_FMGR_TYPE_MID:
        plat_file_type = MMIFMM_MUSIC_MID;
		break;
	case MR_FMGR_TYPE_MIDI:
        plat_file_type = MMIFMM_MUSIC_MIDI;
		break;
	case MR_FMGR_TYPE_WAV:
        plat_file_type = MMIFMM_MUSIC_WAV;
		break;
	case MR_FMGR_TYPE_AMR:
        plat_file_type = MMIFMM_MUSIC_AMR;
		break;
	case MR_FMGR_TYPE_AAC:
        plat_file_type = MMIFMM_MUSIC_ACC;
		break;
	case MR_FMGR_TYPE_WMA:
        plat_file_type = MMIFMM_MUSIC_WMA;
		break;
	case MR_FMGR_TYPE_3GP:
        plat_file_type = MMIFMM_MOVIE_3GP;
		break;
	case MR_FMGR_TYPE_MP4:
        plat_file_type = MMIFMM_MOVIE_MP4;
		break;
	case MR_FMGR_TYPE_AVI:	
        plat_file_type = MMIFMM_MOVIE_AVI;
		break;
	case MR_FMGR_TYPE_JAD:
        plat_file_type = MMIFMM_JAVA_JAD;
		break;
	case MR_FMGR_TYPE_JAR:
        plat_file_type = MMIFMM_JAVA_JAR;
		break;
	case MR_FMGR_TYPE_MRP:
        plat_file_type = MMIFMM_MRAPP_MRP;
		break;
#ifdef MRAPP_SIMUGAME_OPEN
	case MR_FMGR_TYPE_NES:
        plat_file_type = MMIFMM_MRAPP_NES;
		break;
#endif
	case MR_FMGR_TYPE_FOLDER:
      	return MR_SUCCESS;
	case MR_FMGR_TYPE_DAF:
	case MR_FMGR_TYPE_VM:
	case MR_FMGR_TYPE_AWB:
	case MR_FMGR_TYPE_AIF:
	case MR_FMGR_TYPE_AIFF:
	case MR_FMGR_TYPE_AIFC:
	case MR_FMGR_TYPE_AU:
	case MR_FMGR_TYPE_SND:
	case MR_FMGR_TYPE_M4A:
	case MR_FMGR_TYPE_MMF:
	case MR_FMGR_TYPE_WBM:
	case MR_FMGR_TYPE_IMY:
	case MR_FMGR_TYPE_VCF:
	case MR_FMGR_TYPE_VCS:
	case MR_FMGR_TYPE_THEME:
#ifndef MRAPP_SIMUGAME_OPEN
	case MR_FMGR_TYPE_NES:
#endif
	case MR_FMGR_TYPE_ZIP:	
	case MR_FMGR_TYPE_ZPK:
	case MR_FMGR_TYPE_EMS:
	case MR_FMGR_TYPE_ANM:
	case MR_FMGR_TYPE_FOLDER_DOT:
	case MR_FMGR_TYPE_UNKNOW:
        return MR_IGNORE;
	default:
		break;
	}

    FMGR_FILTER_CLEAR(s_mr_file_filter_type, plat_file_type);

    return ret;
}

int32 MMMRAPP_FileMgrBrowser(MR_T_FMGR_REQ *input)
{
    int32 i = 0;
    int32 ret = 0;
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName = NULL;
	uint16 devNameLen = 0;
    MMIFMM_FILTER_T filter = {0};
    uint16  path_ucs2[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16  find_path[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16  find_path_len = MMIFILE_FULL_PATH_MAX_LEN;

    if(NULL == input)
    {
        return MR_FAILED;
    }

    switch(input->list_path[0])
    {
    case 'a':
    case 'A':
    	input->list_path[0] = 'C';
        break;
    case 'b':
    case 'B':
    	input->list_path[0] = 'D';
        break;
    case 'c':
    case 'C':
#ifdef DUAL_TCARD_SUPPORT
        if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
        {
        	input->list_path[0] = 'E';
        }
        else if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
        	input->list_path[0] = 'F';
        }
#else
    	input->list_path[0] = 'E';
#endif
        break;
	default:
		break;
    }
    MMIMR_Trace(("mrapp MMMRAPP_FileMgrBrowser type:%d path:%s", input->dft_list, input->list_path));
    
    switch(input->dft_list)
    {
    case MR_DSPL_IMAGE:
#if (SPR_VERSION >= 0x10A1140)
		if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#else
		if(MMIAPISD_GetStatus())
#endif
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN,
               MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE),
               PNULL, 0,
               find_path, &find_path_len);
        }
#ifdef DUAL_TCARD_SUPPORT
        else if (TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN,
               MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE),
               PNULL, 0,
               find_path, &find_path_len);
        }
#endif
        else if (MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN,
               MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE),
               PNULL, 0,
               find_path, &find_path_len);
        }
        break;
        
    case MR_DSPL_AUDIO:
#if (SPR_VERSION >= 0x10A1140)
		if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#else
		if(MMIAPISD_GetStatus())
#endif
		{
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN,
               MMIMULTIM_DIR_MUSIC, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC),
               PNULL, 0,
               find_path, &find_path_len);
        }
#ifdef DUAL_TCARD_SUPPORT
        else if (TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN,
               MMIMULTIM_DIR_MUSIC, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC),
               PNULL, 0,
               find_path, &find_path_len);
        }
#endif
        else if (MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN,
               MMIMULTIM_DIR_MUSIC, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC),
               PNULL, 0,
               find_path, &find_path_len);
        }
        break;
        
    case MR_DSPL_VIDEO:
#if (SPR_VERSION >= 0x10A1140)
		if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#else
		if(MMIAPISD_GetStatus())
#endif
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN,
               MMIMULTIM_DIR_MOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE),
               PNULL, 0,
               find_path, &find_path_len);
        }
#ifdef DUAL_TCARD_SUPPORT
        else if (TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN,
               MMIMULTIM_DIR_MOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE),
               PNULL, 0,
               find_path, &find_path_len);
        }
#endif
        else if (MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN,
               MMIMULTIM_DIR_MOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE),
               PNULL, 0,
               find_path, &find_path_len);
        }
        break;
        
    case MR_DSPL_NES:
        ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)path_ucs2, (const uint8 *)MMIMRAPP_NES_DIR_NAME, strlen(MMIMRAPP_NES_DIR_NAME));
#if (SPR_VERSION >= 0x10A1140)
		if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#else
		if(MMIAPISD_GetStatus())
#endif
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN,
               path_ucs2, ucs2_name_len/2,
               PNULL, 0,
               find_path, &find_path_len);
        }
#ifdef DUAL_TCARD_SUPPORT
        else if (TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN,
               path_ucs2, ucs2_name_len/2,
               PNULL, 0,
               find_path, &find_path_len);
        }
#endif
        else if (MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
        {
            MMIFILE_CombineFullPath(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN,
               path_ucs2, ucs2_name_len/2,
               PNULL, 0,
               find_path, &find_path_len);
        }
        break;
        
    case MR_DSPL_NONE:
    default:
        MMIMRAPP_ConvertPath2Pc(input->list_path, strlen(input->list_path));
    	find_path_len = MMIMRAPP_GB2UCS((uint16*)find_path, (const uint8 *)input->list_path, strlen(input->list_path));
        find_path_len = find_path_len/2;
        break;
    }

	if(s_mr_file_filter_type == MMIFMM_FILE_ALL)
	{
		SCI_MEMCPY(filter.filter_str, "*.*", strlen("*.*")); /*lint !e666*/
	}
	else
	{
		MMIFMM_GetFilerInfo(&filter, s_mr_file_filter_type);
	}
    MMIMR_Trace(("filter:%s find_path_len: %d", filter.filter_str, find_path_len));
	
	if(find_path_len>3)
	{
		ret = MMIFMM_OpenFileWinByLayer(find_path, find_path_len, &filter, FUNC_FIND_FILE_AND_ALLFOLER, FALSE, MMIMRAPP_HandleFileBrowserWinMsg, NULL, NULL, FALSE);
	}
	else
	{
		ret = MMIFMM_OpenFileWinByLayer(NULL, 0, &filter, FUNC_FIND_FILE_AND_ALLFOLER, FALSE, MMIMRAPP_HandleFileBrowserWinMsg, NULL, NULL, FALSE);
	}
	MMIMR_Trace(("fileMgrBroswer openWin ret : %d", ret));
	
    if(FMM_OPENWIN_SUCCESS != ret)
    {
        return MR_FAILED;
    }
    else
    {
        return MR_SUCCESS;
    }
}

int32 MMMRAPP_FileMgrGetPath(uint8 **output, int32 *output_len)
{
    if(output == NULL || output_len == NULL)
    {
        return MR_FAILED;
    }
    else
    {
        switch(g_mr_filebrowser_path[0])
        {
        case 'c':
        case 'C':
            g_mr_filebrowser_path[0] = 'A';
            break;

        case 'd':
        case 'D':
            g_mr_filebrowser_path[0] = 'B';
            break;

        case 'e':
        case 'E':
#ifdef DUAL_TCARD_SUPPORT
        case 'f':
        case 'F':
#endif
            g_mr_filebrowser_path[0] = 'C';
            break;
		default:
		break;
        }
        MMIMRAPP_ConvertPath2Loc((char*)g_mr_filebrowser_path, strlen((char*)g_mr_filebrowser_path));
        *output = g_mr_filebrowser_path;
        *output_len = strlen((char*)g_mr_filebrowser_path);
        return MR_SUCCESS;
    }
}

int32 MMMRAPP_FileMgrExit(void)
{
    s_mr_file_filter_type = 0;
    SCI_MEMSET(g_mr_filebrowser_path, 0x00, sizeof(g_mr_filebrowser_path));

    return MR_SUCCESS;
}

void dsmGB2UCS2(char *src, char *dest)
{
    uint16 len = 0;
    
    if(NULL != src && NULL != dest)
    {
       len = GUI_GB2UCS((uint16*)dest, (uint8*)src, strlen(src));
       //MMIMR_Trace(("mrapp dsmGB2UCS2 src:0x%02x 0x%02x 0x%02x 0x%02x dest: 0x%02x 0x%02x len:%d", src[0], src[1], src[2], src[3], dest[0], dest[1], len));
    }
}

/**
  * 设置墙纸和屏保
  *
  * @param[in] input 墙纸或屏保文件路径
  *
  * @return MR_SUCCESS 成功，MR_FAILED 失败，MR_IGNORE 不支持
  */
// platex code: 1009
int32 mr_setting_set_wallpaper(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    BOOLEAN result = TRUE;
    uint16 fullpathname[SFS_MAX_PATH] = {0};  
    int32  fullPathLen = 0;
	T_SET_IDLE_PAPER_REQ *req = (T_SET_IDLE_PAPER_REQ*)input;
	
	if(req == NULL || req->filename == NULL)
		return MR_FAILED;

    fullPathLen = mr_fs_get_filename(fullpathname, req->filename);

    if(req->setType == MR_IDLE_BG_PAPER)
    {
        result = MMIAPIMULTIM_SetPictureToWallpaper(FALSE, fullpathname, fullPathLen);
    }
    else
    {
        return MR_IGNORE;
    }
    MMIMR_Trace(("mrapp mr_setting_set_wallpaper type=%d filename=%s result=%d", req->setType, req->filename, result));

    if(result == TRUE)
    {
    	return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

/**
  * 获取系统默认的相册路径
  *
  * @param[out] output 指向路径首地址，UNICODE编码
  *
  * @return MR_SUCCESS 成功，MR_FAILED 失败，MR_IGNORE 不支持
  */
// platex code: 0x9000A
int32 mr_setting_get_system_image_path(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	uint16 ucs2_name_len = SFS_MAX_PATH;
	uint16 devName = 'C';
	uint16 devNameLen = 1;
#if (SPR_VERSION >= 0x10A1140)
    MMIFILE_DEVICE_E dev = MMI_DEVICE_SDCARD;
#else
    DC_DEV_E dev = DC_DEV_UDISK;
#endif
	wchar  *save_path = NULL;

    MMIMR_Trace(("[SKYSETTING] mr_setting_get_system_image_path output=0x%08x", output));

	if(output == NULL)
	{
		return MR_FAILED;
	}

#ifdef CAMERA_SUPPORT
    MMIDC_AllocSettingMemory();
#if (SPR_VERSION >= 0x10A1140)
    MMIDC_Setting_GetNVDefaultValue();
#else
    MMIDC_Setting_InitDefaultValue();
#endif
	dev = MMIDC_GetPhotoStorageDevice();
    save_path = MMIDC_GetPhotoSavePath();
    MMIDC_FreeSettingMemory();
#endif

#if (SPR_VERSION >= 0x10A1140)
    switch(dev)
    {
    case MMI_DEVICE_SYSTEM:
        devName = 'A';
        break;
    case MMI_DEVICE_UDISK:
        devName = 'B';
        break;
    case MMI_DEVICE_SDCARD:
    default:
        devName = 'C';
        break;
    }
#else
    switch(dev)
    {
    case DC_DEV_UDISK:
        devName = 'B';
        break;
    case DC_DEV_SD:
    default:
        devName = 'C';
        break;
    }
#endif

	memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));

	if(save_path != NULL)
	{
		ucs2_name_len = MMIAPICOM_Wstrlen(save_path);
		MMIAPICOM_Wstrncpy(ucs2_name_arr, save_path, ucs2_name_len);
		ucs2_name_arr[0] = devName;
	}
	else
	{ 
		if(!MMIFILE_CombineFullPath(
			(const uint16*)&devName, devNameLen,
			(const uint16*)MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE),
			NULL, 0,
			(uint16*)ucs2_name_arr,
			&ucs2_name_len
			))
		{
			return MR_FAILED;
		}
	}
	
	MMIAPICOM_WstrTraceOut(ucs2_name_arr, ucs2_name_len);
	MMIMRAPP_ConvertPath2Loc((uint8*)ucs2_name_arr, ucs2_name_len*2);
	*output = (uint8*)ucs2_name_arr;
	
	return MR_SUCCESS;
}

/**
  * 把RGB565的数据保存为指定格式的图片
  *
  * @param input 指向mr_image_encode_info
  *
  * @return MR_SUCCESS 成功，MR_FAILED 失败，MR_IGNORE 不支持
  */
// platex code: 0x9000B  
int32 mr_setting_image_encode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    int32 result = FALSE;
	uint16 path_buf[SFS_MAX_PATH] = {0};
    uint32 path_len = 0;
	uint8 *outbuf_ptr = NULL;
	uint8 *probuf_ptr = NULL;
    int32 outbuf_size = 0;
    int32 probuf_size = 0;
	PEPROCESS_INPUT_T       input_param  = {0};
    PEPROCESS_OUTPUT_T      output_param = {0};
    PEPROCESS_PARAMETER_T   process_param= {0};
    PEPROCESS_IMAGE_FORMAT_E out_type = PEPROCESS_IMAGE_FORMAT_JPEG;
    PEPROCESS_RESULT_E ref_return = PEPROCESS_OK;
	mr_image_encode_info* info = (mr_image_encode_info* )input;

    if(info == NULL || info->buf == NULL || info->path == NULL || info->w <= 0 || info->h <= 0)
    {
        return MR_FAILED;
    }

    MMIMR_Trace(("[SKYSETTING] mr_setting_image_encode type=%d w=%d h=%d", info->type, info->w, info->h));
    MMIAPICOM_WstrTraceOut(info->path, MMIAPICOM_Wstrlen(info->path));
	
	path_len = MMIAPICOM_Wstrlen(info->path);
	SCI_MEMSET(path_buf, 0x00, sizeof(path_buf));
	MMIAPICOM_Wstrncpy(path_buf, info->path, path_len);
	path_buf[0] += 2;
	
    outbuf_size = info->w*info->h*2;
    probuf_size = info->w*info->h*3+100*1024;

    outbuf_ptr = (uint8*)SCI_ALLOCA(outbuf_size);
	MMIMR_Trace(("[SKYSETTING] mr_setting_image_encode outbuf_ptr=0x%08x outbuf_size=%d", outbuf_ptr, outbuf_size));
    if(outbuf_ptr == NULL)
    {
        return MR_FAILED;
    }
    
    probuf_ptr = (uint8*)SCI_ALLOCA(probuf_size);
	MMIMR_Trace(("[SKYSETTING] mr_setting_image_encode probuf_ptr=0x%08x probuf_size=%d", probuf_ptr, probuf_size));
    if(probuf_ptr == NULL)
    {
		if(outbuf_ptr != NULL)
		{
			SCI_FREE(outbuf_ptr);
			outbuf_ptr = NULL;
		}
        return MR_FAILED;
    }

	SCI_MEMSET(outbuf_ptr, 0x00, outbuf_size);
	SCI_MEMSET(probuf_ptr, 0x00, probuf_size);

    if(info->type == IMG_BMP)
    {
        out_type = PEPROCESS_IMAGE_FORMAT_BMP;
    }
    else if(info->type == IMG_JPG)
    {
        out_type = PEPROCESS_IMAGE_FORMAT_JPEG;
    }

    //set input param   
    input_param.src_ptr     = info->buf;
    input_param.out_buf_ptr = outbuf_ptr;
    input_param.out_buf_size= outbuf_size;
    input_param.input_width = info->w;
    input_param.input_height= info->h;
    input_param.target_width  = info->w;
    input_param.target_height = info->h;
    input_param.in_type  = PEPROCESS_IMAGE_FORMAT_RGB_565;
    input_param.out_type = PEPROCESS_IMAGE_FORMAT_JPEG;
    
    //set process_param_ptr param
    process_param.process_buf_ptr  = probuf_ptr;
    process_param.process_buf_size = probuf_size;
    process_param.type = PEPROCESS_ENCODE_JPEG;
    
    //param
    process_param.process_param.encode_to_jpeg.quality_level = PEPROCESS_COMPRESS_LEVEL_LOW;
    
    //image encode
#ifdef IMG_EDITER_SUPPORT
    ref_return = PEPROCESS_ImageProcess(&input_param, &output_param, &process_param);
#else
	ref_return = PEPROCESS_NO_SUPPORT;
#endif
    if(PEPROCESS_OK == ref_return)
    {
		if(MMIFILE_WriteFilesDataSyn(path_buf, path_len, input_param.out_buf_ptr, output_param.compress_out_size))
		{
			result = TRUE;
		}
    }

	if(outbuf_ptr)
	{
		SCI_FREE(outbuf_ptr);
		outbuf_ptr = NULL;
	}

	if(probuf_ptr)
	{
		SCI_FREE(probuf_ptr);
		probuf_ptr = NULL;
	}

    if(result == TRUE)
    {
        return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

/**
  * 获取触摸屏类型
  *
  *
  * @return MR_SCREEN_TYPE_RESISTOR  电阻屏  
  *         MR_SCREEN_TYPE_CAPACITOR 电容屏，MR_FAILED 失败，MR_IGNORE 不支持
  *
  */
// platex code: 0x9000C  
int32 mr_setting_get_touchscreen_type(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef TOUCHPANEL_TYPE
#ifdef CAP_TP_SUPPORT
    return MR_SCREEN_TYPE_CAPACITOR;
#else
    return MR_SCREEN_TYPE_RESISTOR;
#endif
#else
    return MR_IGNORE;
#endif
}

/**
  * 获取是否支持Camera接口
  *
  *
  * @return MR_SUCCESS 成功，MR_FAILED 失败，MR_IGNORE 不支持
  */
// platex code: 0x9000D  
int32 mr_setting_get_camera_support(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef __MR_CFG_CAMERA_SUPPORT__
    return MR_SUCCESS;
#else
    return MR_IGNORE;
#endif
}

#ifdef __MR_SEND_MMS_SUPPORT__
/**
 * \brief 发送彩信
 *
 * \param pMMS	[in] 彩信内容 对方号码 主题 图片
 * \return 成功返回MR_SUCCESS, 失败返回MR_FIALED
 */
int32 mr_sms_send_mms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB* cb)
{
    T_DSM_MMS *pMMS = (T_DSM_MMS*)input;
    MMIMMS_ERROR_E ret = MMIMMS_NO_ERROR;
    MMIMMS_EDIT_DOCUMENT_T *editdoc_ptr = NULL;
    MMIFILE_FILE_INFO_T    file_info = {0};
    MMI_STRING_T addr = {0};
    MMI_STRING_T subject = {0};
    MMI_STRING_T content = {0};
    BOOLEAN success = TRUE;
    uint16 path[SFS_MAX_PATH] = {0};
    uint16 num[MMIPB_NUMBER_MAX_STRING_LEN] = {0};

    ret = MMIMMS_EnterEditCheck();

    MMIMR_Trace(("[SKYSMS] mr_sms_send_mms ret=%d pMMS=0x%08x input_len=%d", ret, pMMS, input_len));

    if(ret != MMIMMS_NO_ERROR || pMMS == NULL || input_len != sizeof(T_DSM_MMS))
    {
        return MR_FAILED;
    }

    editdoc_ptr = MMIMMS_CreateEditDocument();
    if(editdoc_ptr)
    {
        MMS_CreateNewSlide(editdoc_ptr->editbody_ptr);

        if(pMMS->addr != NULL)
        {
            addr.wstr_len = GUI_GB2UCS(num, pMMS->addr, strlen(pMMS->addr));
            addr.wstr_ptr = num;
            MMIMMS_SetMMSTo(&addr, editdoc_ptr);
#if (SPR_VERSION >= 0x10A1140)
            MMIMMS_SetMMSNameListByTo(editdoc_ptr);
#endif
        }

        if(pMMS->subject != NULL)
        {
            MMIAPICOM_WstrTraceOut(pMMS->subject, MMIAPICOM_Wstrlen(pMMS->subject));
            subject.wstr_ptr = pMMS->subject;
            subject.wstr_len = MMIAPICOM_Wstrlen(pMMS->subject);
            MMIMMS_SetSubject(&subject, editdoc_ptr);
        }

        if(pMMS->content != NULL)
        {
            MMIAPICOM_WstrTraceOut(pMMS->content, MMIAPICOM_Wstrlen(pMMS->content));
            content.wstr_ptr = pMMS->content;
            content.wstr_len = MMIAPICOM_Wstrlen(pMMS->content);
            MMIMMS_AddText(editdoc_ptr, &content);
        }

        if(pMMS->file_path != NULL)
        {
            MMIAPICOM_WstrTraceOut(pMMS->file_path, MMIAPICOM_Wstrlen(pMMS->file_path));
            MMIAPICOM_Wstrncpy(path, pMMS->file_path, MMIAPICOM_Wstrlen(pMMS->file_path));
            path[0] += 2;
            MMIMRAPP_ConvertPath2Pc((uint8*)path, MMIAPICOM_Wstrlen(path)*2);
            MMIAPIFMM_GetFileInfoFormFullPath(path, MMIAPICOM_Wstrlen(path), &file_info);

            if(!MMIMMS_AddFile(editdoc_ptr, &file_info, MMIFMM_FILE_TYPE_PICTURE, FALSE, 0))
            {
                success = FALSE;
            }
        }

        if(success == TRUE)
        {
            MMIMMS_MMSEditInit(editdoc_ptr, MMIMMS_RICHTEXT_TO);
            MMIMMS_CreateEditWin(); 
            return MR_SUCCESS;
        }
        else
        {
            MMIMMS_DestroyEditDocument(editdoc_ptr);//正常发送结束之后也需要Destroy?
            return MR_FAILED;
        }
    }

    return MR_FAILED;
}
#endif

int32 mr_fs_get_free_space(uint8 *input, int32 input_len, uint8** output, int32 *output_len, MR_PLAT_EX_CB *cb)
{
    uint32 kb_size = 1 << 10;
    uint32 mb_size = 1 << 20;
    uint32 gb_size = 1 << 30;
    uint32 used_low  = 0; 
    uint32 used_high = 0;
    uint32 free_low  = 0;
    uint32 free_high = 0;
    uint64 total_space = 0;
    uint64 free_space  = 0;
    uint32 total_space_low  = 0;
    uint32 total_space_high = 0;
    int32 resValue = MR_SUCCESS;
    T_MR_FREE_SAPCE *spaceInfo = (T_MR_FREE_SAPCE*)tempTransBuf;
	
	if(input == NULL || spaceInfo == NULL)
		return MR_FAILED;
	
	MMIMR_Trace(("input: %s", input));
    
	switch((char)input[0])
	{
	case 'A':case 'a':
#ifdef PORT_BASE_NILE
		if(MMIFILE_GetDeviceStatus((uint8 *)MMIFILE_DEVICE_SYS_NAME, MMIFILE_DEVICE_SYS_NAME_LEN))
		{
			MMIFILE_GetDeviceUsedSpace((uint8 *)MMIFILE_DEVICE_SYS_NAME, MMIFILE_DEVICE_SYS_NAME_LEN, &used_high, &used_low);
			MMIFILE_GetDeviceFreeSpace((uint8 *)MMIFILE_DEVICE_SYS_NAME, MMIFILE_DEVICE_SYS_NAME_LEN, &free_high, &free_low);
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
#endif

	case 'B':case 'b':
		if(MMIFILE_GetDeviceStatus((uint8*)MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
		{
			MMIFILE_GetDeviceUsedSpace((uint8 *)MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN, &used_high, &used_low);
			MMIFILE_GetDeviceFreeSpace((uint8 *)MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN, &free_high, &free_low);
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
        
	case 'C':case 'c':
#if (SPR_VERSION >= 0x10A1140)
		if(TRUE == MMIFILE_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#else
		if(MMIAPISD_GetStatus())
#endif
		{
			MMIFILE_GetDeviceUsedSpace((uint8 *)MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN, &used_high, &used_low);
			MMIFILE_GetDeviceFreeSpace((uint8 *)MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN, &free_high, &free_low);
		}
#ifdef DUAL_TCARD_SUPPORT
        else if(TRUE == MMIFILE_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
        {
            MMIFILE_GetDeviceUsedSpace((uint8 *)MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN, &used_high, &used_low);
            MMIFILE_GetDeviceFreeSpace((uint8 *)MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN, &free_high, &free_low);
        }
#endif
		else
		{
			resValue = MR_FAILED;
		}
		break;
        
#ifdef DOUBLE_TFCARD
	case 'D':case 'd':
		if(MMIAPISD2_GetStatus())
		{
			MMIFILE_GetDeviceUsedSpace((uint8 *)MMIFILE_DEVICE_SDCARD_2, MMIFILE_DEVICE_SDCARD_LEN_2, &used_high, &used_low);
			MMIFILE_GetDeviceFreeSpace((uint8 *)MMIFILE_DEVICE_SDCARD_2, MMIFILE_DEVICE_SDCARD_LEN_2, &free_high, &free_low);
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
#endif

	default:
		resValue = MR_FAILED;
		break;
	}	

	memset(spaceInfo, 0, sizeof(T_MR_FREE_SAPCE));
	//MMIMR_Trace(("free_high: %u, free_low: %u, used_high: %u, used_low: %u", free_high, free_low, used_high, used_low));		
	if(resValue == MR_SUCCESS)
    {
		/*free space*/
		if(free_high || (uint32)free_low > (uint32)0x7fffffff)
        {
            free_space = (uint64)((free_high<<22) + (free_low>>10))*1024;
		}
        else
        {
            free_space = free_low;
		}
		
		/*total space*/
		total_space_low  = used_low  + free_low;  
		total_space_high = used_high + free_high;
        
		if ((total_space_low < used_low) || (total_space_low < free_low))
        {
			total_space_high += 1;
		}
		//MMIMR_Trace(("total_space_low: %u, total_space_high: %u", total_space_low, total_space_high));		
		if(total_space_high || (uint32)total_space_low > (uint32)0x7fffffff)
        {
			total_space = (uint64)((total_space_high<<22) + (total_space_low>>10))*1024;
		}
        else
        {
			total_space = total_space_low;
		}
        MMIMR_Trace(("total_space: %llu, free_space: %llu", total_space, free_space));		
#if 0   //单位太大会导致精度不精确
        if((total_space/10) > (uint64)gb_size)
    	{
    		spaceInfo->tunit = gb_size;
    	}
    	else if((total_space/10) > mb_size)
    	{
    		spaceInfo->tunit = mb_size;
    	}
        else
#endif
    	if((total_space/10) > kb_size)
    	{
    		spaceInfo->tunit = kb_size;
    	}
    	else 
    	{
    		spaceInfo->tunit = 1;
    	}
    	
    	spaceInfo->total = total_space/spaceInfo->tunit;
#if 0   //单位太大会导致精度不精确
    	if((free_space/10) >(uint64)gb_size)
    	{
    		spaceInfo->unit = gb_size;
    	}
    	else if((free_space/10) > mb_size)
    	{
    		spaceInfo->unit = mb_size;
    	}
    	else
#endif
        if((free_space/10) > kb_size)
    	{
    		spaceInfo->unit = kb_size;
    	}
    	else 
    	{
    		spaceInfo->unit = 1;
    	}
    	
    	spaceInfo->account = free_space/spaceInfo->unit;

		MMIMR_Trace(("free: %d, total: %d, unit: %d, tunit: %d", spaceInfo->account, spaceInfo->total, spaceInfo->unit, spaceInfo->tunit));
            
		*output = (uint8*)spaceInfo;
		*output_len = sizeof(T_MR_FREE_SAPCE);    	
    	return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

int32 mr_setting_wifi_exist(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
//是否存在WIFI功能
#ifdef __MRAPP_WIFI_SUPPORT__
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}


#ifdef __MR_CFG_FEATURE_HANDSETID_EX__
int mr_get_local_version(char* temp)
{
#ifdef WIN32
	return 0;
#else
	extern char* const s_version_info[];
	int iIndex = 18;
	while( *(s_version_info[0]+iIndex) !='\0'
		&& *(s_version_info[0]+iIndex) == ' ' )
	{
		iIndex++;
	}
       SCI_MEMCPY(temp,s_version_info[0]+iIndex,strlen(s_version_info[0])-iIndex);
	return strlen(temp);
	
#endif
}

int32 mr_setting_get_handsetID_ex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_userinfo usrinfo;
	char temp[256]={0};
	int templen=0;
	char tempex[__MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__+1]={0};
	if(!input || input_len < __MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__)
	return MR_FAILED;
	mr_getUserInfo(&usrinfo);
	templen=mr_get_local_version(temp);
	if(templen==0)
	{
	_snprintf( (char*)input, input_len, "%s!%s", usrinfo.type, __MR_CFG_VAR_HANDSETID_EX__);	
	return MR_SUCCESS;
	}
	
	if(templen<=(__MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__-10))
	{
		SCI_MEMCPY(tempex,temp,templen);
	}
	else
	{
		SCI_MEMCPY(tempex,temp,(__MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__-10));
	}

	_snprintf( (char*)input, input_len, "%s!%s", usrinfo.type, tempex);	
        MMIMR_Trace(("mrapp mr_setting_get_handsetID_ex type=%s", input));
    
     return MR_SUCCESS;
}
#endif


/* begin:added by Tommy.yan 20120120 */

#ifdef __MR_CFG_LOCALAPP_MANAGER__
void mr_registerLappEntry(void* entry)
{
    s_skyLappEntry = (startSkyLapp)entry;
}

int32 mr_start_plat_application(uint8* input)
{
    if(s_skyLappEntry)
    {
        return s_skyLappEntry(input);
    }

    return MR_FAILED;
}
#endif

/*****************************************************************************/
// 	Description :  获取保存信息的order id
//	Global resource dependence : 
//  Author: Tommy.yan
//   note:
/*****************************************************************************/
PUBLIC MMISMS_STATE_T * MMIMRAPP_GetSaveSMSOrderId(void)
{
	return &s_mr_save_order_id;
}

/*****************************************************************************/
// 	Description :  信息发送结果
//	Global resource dependence : 
//  Author: Tommy.yan
//   note:
/*****************************************************************************/
LOCAL BOOLEAN MMIMRAPP_SMSCnf(BOOLEAN is_succss, DPARAM parm)
{
	BOOLEAN resValue = TRUE;
	
	MMIMR_Trace(("mrapp MMIMRAPP_SMSCnf is_succss: %d", is_succss));
	if(TRUE == is_succss)
	{
	 	mr_event(MR_SMS_RESULT, MR_SUCCESS, 0);//短信发送成功
	}
	else
	{
		mr_event(MR_SMS_RESULT, MR_FAILED, 0);//短信发送失败
	}

	MMIMR_Trace(("MMIMRAPP_SMSCnf resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetSMSCapacityCallBack(void)
{
	MMIMR_Trace(("MRAPP[SMS] MMIMRAPP_GetSMSCapacityCallBack NVMaxNum: %d", s_mr_sms_NVNum));
	mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_GET_ME_SIZE, s_mr_sms_NVNum);

	MMIMR_Trace(("MRAPP[SMS] MMIMRAPP_GetSMSCapacityCallBack SIMMaxNum: %d", s_mr_sms_SIMNum));
	mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_GET_SIM_SIZE, s_mr_sms_SIMNum);
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetSMSContentCallBack()
{ /*lint !e18*/	

	MMI_STRING_T string_ptr = {0};
	wchar content_ptr[500] = {0};
	string_ptr.wstr_ptr = content_ptr;

	MMIMRAPP_SetIsReadBySMS(FALSE);
	
	MMISMS_GetSMSContent(&string_ptr);
	MMIMR_Trace(("MRAPP[SMS] MMIMRAPP_GetSMSContentCallBack string_ptr.wstr_ptr: %d",  MMIAPICOM_Wstrlen(string_ptr.wstr_ptr)));
	MMIAPICOM_WstrTraceOut(string_ptr.wstr_ptr, MMIAPICOM_Wstrlen(string_ptr.wstr_ptr));
	MMIAPICOM_Wstrcpy(s_mr_sms_content.content, string_ptr.wstr_ptr);

	if(NULL != s_mr_sms_content.content) /*lint !e506 !e774 !e831*/
	{
#ifndef WIN32
		mr_str_convert_endian((char*)s_mr_sms_content.content);
#endif
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_GET_MSG_CONTENT, s_mr_sms_content.content); /*lint !e64*/
	}
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:针对长短信做相应的处理
/*****************************************************************************/
PUBLIC void MMIMRAPP_DeleteSMSCallBack(void *param)
{
	BOOLEAN result = FALSE;
	MN_SMS_CAUSE_E    cause = *(int32*)param;
    MMISMS_ORDER_ID_T order_id = NULL;
    MMISMS_BOX_TYPE_E type = MMISMS_BOX_NONE;
        
	MMIMR_Trace(("MRAPP[SMS] MMIMRAPP_DeleteSMSCallBack order_id: 0x%08x cause=%d", s_mr_sms_order_id, cause));

	if(cause != MN_SMS_OPERATE_SUCCESS)
	{
		MMIMRAPP_SetIsReadBySMS(FALSE);
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_DELETE, MR_FAILED);
		return;
	}

    if(s_mr_sms_order_id != NULL)
    {
        order_id = s_mr_sms_order_id;
        s_mr_sms_order_id = order_id->next_long_sms_ptr;
        if(order_id->flag.mo_mt_type == MMISMS_MT_HAVE_READ || order_id->flag.mo_mt_type == MMISMS_MT_TO_BE_READ)
        {
            type = MMISMS_BOX_MT;
        }
        else if(order_id->flag.mo_mt_type == MMISMS_MO_SEND_SUCC)
        {
            type = MMISMS_BOX_SENDSUCC;
        }
        
        result = MMISMS_DeleteReqToMN(type, order_id, MMISMS_DELETE_MSG);
        if(FALSE == result)
        {
            MMIMRAPP_SetIsReadBySMS(FALSE);
            mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_DELETE, MR_FAILED);
        }
    }
    else
    {
        MMIMRAPP_SetIsReadBySMS(FALSE);
    	if(cause == MN_SMS_OPERATE_SUCCESS)
    	{
    		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_DELETE, MR_SUCCESS);
    	}
    	else
    	{
    		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_DELETE, MR_FAILED);
    	}
    }
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_SetSMSStateCallBack(void)
{
	MMIMR_Trace(("MRAPP[SMS] MMIMRAPP_SetSMSStateCallBack err_code: %d", s_mr_sms_err_code));
	if(s_mr_sms_err_code == ERR_MNSMS_NONE)
	{
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_SET_STATUS, MR_SUCCESS);
	}
	else
	{
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_SET_STATUS, MR_FAILED);
	}
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_SaveSMSToMtCallBack(MN_SMS_CAUSE_E result)
{
	MMIMR_Trace(("MRAPP[SMS] MMIMRAPP_SaveSMSToMtCallBack result: %d", result));
	if(result == MN_SMS_OPERATE_SUCCESS)
	{
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_SAVE_SMS, TRUE);
	}
	else
	{
		mr_event(MR_SMS_OP_EVENT, MR_SMS_OP_SAVE_SMS, FALSE);
	}	
}
/* begin:added by Tommy.yan 20120120 */

#ifdef __MR_CFG_MULTITP_SUPPORT__
#define MOUSE_MOVE_SPACE (6)
static MR_TOUCH_PANEL_COORD_LIST_T g_last_touch_info = {0};

static uint16 mr_touch_panel_down_func(uint16 x_diff, uint16 y_diff, uint16 count)
{
	if (x_diff > MOUSE_MOVE_SPACE || y_diff > MOUSE_MOVE_SPACE)
	{
		return MR_MOUSE_MOVE;
	}
	else
	{
		return MR_MOUSE_DOWN | 0x0100;
	}
}

static uint16 mr_touch_panel_up_func(uint16 x_diff, uint16 y_diff, uint16 count)
{
	return MR_MOUSE_UP;
}

static uint16 mr_touch_panel_move_func(uint16 x_diff, uint16 y_diff, uint16 count)
{
	if (x_diff > MOUSE_MOVE_SPACE || y_diff > MOUSE_MOVE_SPACE)
	{
		return MR_MOUSE_MOVE;
	}
	else
	{
		return MR_MOUSE_DOWN | 0x0100;
	}
}

typedef uint16 (* MR_TOUCH_PANEL_FUNC)(uint16 x_diff, uint16 y_diff, uint16 count);

static const MR_TOUCH_PANEL_FUNC g_mr_touch_panel_func[] =  
{
	mr_touch_panel_down_func,    /*Down */
	mr_touch_panel_up_func,      /*Up*/
	mr_touch_panel_move_func,    /*Move*/
};

static float mr_touch_panel_inv_sqrt(float x)
{
	/* 0x5f375a86 这是一个非常神奇的数字*/
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating VALUE 
	i = 0x5f375a86 - (i>>1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
	return x;
}

static int16 mr_touch_panel_get_diff(int16 x, int16 y)
{
	int xx,yy,tmp;
	
	if (x == 0)
	{
		return y;
	}
	else if (y == 0)
	{
		return x;
	}
	else
	{
		xx = x*x; yy = y*y; tmp = xx + yy;
		return (int16)(1/mr_touch_panel_inv_sqrt(tmp));
	}
}

static int16 mr_touch_panel_find_nearest(int16 diff[MAX_TOUCH_POINTS][MAX_TOUCH_POINTS], int16 *row, int16 *col)
{
#define VERY_LARGE 0x7FFF
	uint16 i, j;
	int16 min_diff = VERY_LARGE;
	int16 k = -1, l = -1;

	for (i = 0; i < *row; i++)
	{
		for (j = 0; j < *col; j++)
		{
			if (diff[i][j] >= 0 && min_diff > diff[i][j])
			{
				min_diff = diff[i][j];
				k = i;
				l = j;
			}
		}
	}

	if (k != -1 && l != -1)
	{
		for (i = 0; i < *row; i++)
		{
			if (diff[i][l] >= 0)
			{
				diff[i][l] = -1;
			}
		}
		for (i = 0; i < *col; i++)
		{
			diff[k][i] = -2;
		}
		*row = k;
		*col = l;
		return 1;
	}
	else
	{
		for (i = 0; i < *row; i++)
		{
			if (diff[i][0] > -2)
			{
				for (j = 0; j < *col; j++)
				{
					diff[i][j] = -2;
				}
				*row = i;
				*col = -1;
				return 1;
			}
			
		}
		return 0;
	}
}


static uint16 mr_touch_panel_state_machine(uint16 x_diff, uint16 y_diff, uint16 pre_event, uint16 count)
{
	int index;
	
	if (pre_event == MR_MOUSE_DOWN)
	{
		index = 0;
	}
	else if (pre_event == MR_MOUSE_UP)
	{
		index = 1;
	}
	else
	{
		index = 2;
	}
	
	return g_mr_touch_panel_func[index](x_diff, y_diff, count);
}

void mr_touch_panel_convert_event(MR_TOUCH_PANEL_COORD_LIST_T *current_event, MR_TOUCH_PANEL_COORD_LIST_T *last_event)
{
	int16 i, j;
	uint32 event;
	int16 x_diff;
	int16 y_diff;
	uint32 touch_count = 0;
	uint32 ori_touch_count = 0;
	MR_TOUCH_PANEL_COORD_LIST_T tmp_event;
	int16 diff[MAX_TOUCH_POINTS][MAX_TOUCH_POINTS];
	uint8 dir;

	if (current_event->touch_count == 0) //all touch up
	{
		for (i = 0; i < last_event->touch_count; i++)
		{
			if (last_event->points[i].event != MR_MOUSE_UP)
			{
				current_event->points[i].x = last_event->points[i].x;
				current_event->points[i].y = last_event->points[i].y;
				current_event->points[i].z = last_event->points[i].z;
				current_event->points[i].event = MR_MOUSE_UP;
				touch_count++;
			}
		}

		current_event->touch_count = touch_count;

		return ;
	}

	for (i = 0; i < last_event->touch_count; i++)
	{
		if (last_event->points[i].event != MR_MOUSE_UP)
		{
			ori_touch_count++;
		}
	}

	if (ori_touch_count == 0)
	{
		/* no need convert */
		return ;
	}

	if (ori_touch_count != last_event->touch_count)
	{
		ori_touch_count = 0;
		for (i = 0; i < last_event->touch_count; i++)
		{
			if (last_event->points[i].event != MR_MOUSE_UP)
			{
				last_event->points[ori_touch_count].x = last_event->points[i].x;
				last_event->points[ori_touch_count].y = last_event->points[i].y;
				last_event->points[ori_touch_count].z = last_event->points[i].z;
				last_event->points[ori_touch_count].event = last_event->points[i].event;
				ori_touch_count++;
			}
		}
		last_event->touch_count = ori_touch_count;
	}

	if (current_event->touch_count < last_event->touch_count)
	{
		dir = 1;
	}
	else
	{
		dir = 0;
	}

	/* calc the diff of all the down point */
	for (i = 0; i < current_event->touch_count; i++)
	{
		for (j = 0; j < last_event->touch_count; j++)
		{
			if (current_event->points[i].x > last_event->points[j].x)   
				x_diff = current_event->points[i].x - last_event->points[j].x;
			else
				x_diff = last_event->points[j].x - current_event->points[i].x;
			if (current_event->points[i].y > last_event->points[j].y)
				y_diff = current_event->points[i].y - last_event->points[j].y;
			else
				y_diff = last_event->points[j].y - current_event->points[i].y;
			if (dir)
			{
				diff[j][i] = mr_touch_panel_get_diff(x_diff, y_diff);
			}
			else
			{
				diff[i][j] = mr_touch_panel_get_diff(x_diff, y_diff);
			}
		}
	}

	if (dir)
	{
		j = current_event->touch_count;
		i = last_event->touch_count;
	}
	else
	{
		i = current_event->touch_count;
		j = last_event->touch_count;
	}

	touch_count = 0;
	while (mr_touch_panel_find_nearest(diff, &i, &j))
	{
		if (i != -1 && j != -1)
		{
			if (dir)
			{
				if (current_event->points[j].x > last_event->points[i].x)   
					x_diff = current_event->points[j].x - last_event->points[i].x;
				else
					x_diff = last_event->points[i].x - current_event->points[j].x;
				if (current_event->points[j].y > last_event->points[i].y)
					y_diff = current_event->points[j].y - last_event->points[i].y;
				else
					y_diff = last_event->points[i].y - current_event->points[j].y;

				event = mr_touch_panel_state_machine(x_diff, y_diff, last_event->points[i].event, i);

				tmp_event.points[touch_count].x = current_event->points[j].x;
				tmp_event.points[touch_count].y = current_event->points[j].y;
				tmp_event.points[touch_count].z = current_event->points[j].z;
				tmp_event.points[touch_count].event = event;
			}
			else
			{
				if (current_event->points[i].x > last_event->points[j].x)   
					x_diff = current_event->points[i].x - last_event->points[j].x;
				else
					x_diff = last_event->points[j].x - current_event->points[i].x;
				if (current_event->points[i].y > last_event->points[j].y)
					y_diff = current_event->points[i].y - last_event->points[j].y;
				else
					y_diff = last_event->points[j].y - current_event->points[i].y;

				event = mr_touch_panel_state_machine(x_diff, y_diff, last_event->points[j].event, j);

				tmp_event.points[touch_count].x = current_event->points[i].x;
				tmp_event.points[touch_count].y = current_event->points[i].y;
				tmp_event.points[touch_count].z = current_event->points[i].z;
				tmp_event.points[touch_count].event = event;
			}
			
			touch_count++;
		}
		else if (i != -1)
		{
			if (dir)
			{
				tmp_event.points[touch_count].x = last_event->points[i].x;
				tmp_event.points[touch_count].y = last_event->points[i].y;
				tmp_event.points[touch_count].z = last_event->points[i].z;
				tmp_event.points[touch_count].event = MR_MOUSE_UP;
			}
			else
			{
				tmp_event.points[touch_count].x = current_event->points[i].x;
				tmp_event.points[touch_count].y = current_event->points[i].y;
				tmp_event.points[touch_count].z = current_event->points[i].z;
				tmp_event.points[touch_count].event = MR_MOUSE_DOWN;
			}
			touch_count++;
		}
		
		if (dir)
		{
			j = current_event->touch_count;
			i = last_event->touch_count;
		}
		else
		{
			i = current_event->touch_count;
			j = last_event->touch_count;
		}
	}
	
	tmp_event.touch_count = touch_count;

	memcpy(current_event, &tmp_event, sizeof(tmp_event));
}

/**
 * \brief 获取当前触摸屏坐标
 *
 * \param input		[out] #MR_TOUCH_PANEL_COORD_T 类型的结构
 * \param input_len	[in] 结构MR_TOUCH_PANEL_COORD_T的长度
 * \param output		[out] #MR_TOUCH_PANEL_COORD_LIST_T 类型的结构(可以为空)
 * \param output_len	[out] 最后一次touch的状态0: UP 1: DOWN (可以为空)
 * \retval
 * #MR_SUCCESS  : 成功
 * \retval
 * #MR_FAILED : 失败
 * \retval
 * #MR_IGNORE : 不支持
 * \par 举例说明
 * \code
 * //mrp code
 * MR_TOUCH_PANEL_COORD_T coord = {0};
 * MR_TOUCH_PANEL_COORD_LIST_T coordlist = {0};
 * int32 re;
 * int32 touch_state;
 * re = mrc_platEx(4200, (uint8 *)&coord, sizeof(MR_TOUCH_PANEL_COORD_T), (uint8 **)&coordlist, &touch_state, NULL);
 * \endcode
 */
int32 mr_touch_panel_get_position(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    int32 i = 0;
	int16 touch_count = 0;
	uint8 have_touch_count = 0;
    GUI_POINT_T     tp_point  = {0};
    MMI_TP_POINTS_T tp_points = {0};
    MMI_TP_STATUS_E tp_status = MMI_TP_NONE;
    
	MR_TOUCH_PANEL_COORD_T *pCoord = (MR_TOUCH_PANEL_COORD_T *)input;
    MR_TOUCH_PANEL_COORD_LIST_T *list = (MR_TOUCH_PANEL_COORD_LIST_T *)output;
	
	if (!pCoord || input_len != sizeof(MR_TOUCH_PANEL_COORD_T))
		return MR_FAILED;

    MMK_GetLogicTPMsg(&tp_status, &tp_point);

    pCoord->x = tp_point.x;
    pCoord->y = tp_point.y;
    
//#ifdef TOUCHPANEL_TYPE_MULTITP
    MMK_GetTpPoints(&tp_points);

	if (output != NULL)
	{
        list->touch_count = tp_points.num;
        for(i=0; i<tp_points.num; i++)
        {
            list->points[i].x = tp_points.points[i].x;
            list->points[i].y = tp_points.points[i].y;
            list->points[i].z = 0;
            list->points[i].event = MR_MOUSE_DOWN;
        }
        //MMIMR_Trace(("SKY total tp_status:%d points:%d", tp_status, list->touch_count));
		mr_touch_panel_convert_event(list, &g_last_touch_info);
		memcpy(&g_last_touch_info, list, sizeof(MR_TOUCH_PANEL_COORD_LIST_T));
		for (i = 0; i < g_last_touch_info.touch_count; i++)
		{
			if ((g_last_touch_info.points[i].event & 0xff00) != 0)
			{
				g_last_touch_info.points[i].event &= 0x00ff;
			}
			else
			{
				have_touch_count = 1;
			}
		}
		for (i = 0; i < list->touch_count; i++)
		{
			if (((list->points[i].event & 0xff00) == 0) || (have_touch_count > 0 && output_len != NULL && *output_len != 0))
			{
				//MMIMR_Trace(("point[%d] = [%d, %d, %d]", touch_count, list->points[i].x, list->points[i].y, list->points[i].event));
				memcpy(&list->points[touch_count], &g_last_touch_info.points[i], sizeof(MR_TOUCH_PANEL_COORD_EX_T));
				touch_count++;
			}
		}
		list->touch_count = touch_count;

    }
    else
    {
        memset(&g_last_touch_info, 0, sizeof(g_last_touch_info));
    }

    if(output_len != NULL)
    {
        switch(tp_status)
        {
        default:
        case MMI_TP_DOWN:
            *output_len = MR_MOUSE_DOWN;
            break;

        case MMI_TP_UP:
            *output_len = MR_MOUSE_UP;
            break;
        }        
    }
//#endif

    return MR_SUCCESS;
}
#endif

#ifdef __MR_CFG_SHORTCUT_SUPPORT__
uint32 mr_skybmp_open(const uint16* path, uint8 *isSkyBmp)
{
	uint32 f;
	char type[5] = {0};
	uint32 read;
	
	f = SFS_CreateFile(path, SFS_MODE_OPEN_EXISTING|SFS_MODE_READ, NULL, NULL);

	if (f == 0)
		return 0;

	SFS_ReadFile(f, type, sizeof(type) - 1, &read, NULL);

    MMIMR_Trace(("[SKYSHORTCUT] mr_skybmp_open type = %s", type));

	if(strcmp(type, "SKBM") == 0)
	{
        *isSkyBmp = 1;
	}
    else
    {
        *isSkyBmp = 0;
    }

	return f;
}

void mr_skybmp_get_size(uint32 f, int16* w, int16* h)
{
	uint32 read = 0;
	
	SFS_SetFilePointer(f, 4, SFS_SEEK_BEGIN);	// 跳过4个字节的头

	SFS_ReadFile(f, w, 2, &read, NULL);
	SFS_ReadFile(f, h, 2, &read, NULL);
#ifdef WIN32
    mr_str_convert_endian_with_size(w, 2);
    mr_str_convert_endian_with_size(h, 2);
#endif
    MMIMR_Trace(("[SKYSHORTCUT] mr_skybmp_get_size w = %d h = %d", *w, *h));
}

uint32 mr_skybmp_get_data(uint32 f, void* data, int32 len)
{
    int16 w = 0;
    int16 h = 0;
    int32 ret = 0;
	uint32 read = 0;
    int32 outbuf_size = 0;
    int32 probuf_size = 0;
	uint8 *outbuf_ptr = NULL;
	uint8 *probuf_ptr = NULL;
	PEPROCESS_INPUT_T       input_param  = {0};
    PEPROCESS_OUTPUT_T      output_param = {0};
    PEPROCESS_PARAMETER_T   process_param= {0};
    PEPROCESS_RESULT_E ref_return = PEPROCESS_OK;

    mr_skybmp_get_size(f, &w, &h);

	ret = SFS_SetFilePointer(f, 8, SFS_SEEK_BEGIN);
    if(ret != SFS_ERROR_NONE)
    {
        return 0;
    }
    
	ret = SFS_ReadFile(f, data, len, &read, NULL);
    if(ret != SFS_ERROR_NONE)
    {
        return 0;
    }

    outbuf_size = w*h*2;
    probuf_size = w*h*3+100*1024;

    outbuf_ptr = (uint8*)SCI_ALLOCA(outbuf_size);
    if(outbuf_ptr == NULL)
    {
        return 0;
    }
    
    probuf_ptr = (uint8*)SCI_ALLOCA(probuf_size);
    if(probuf_ptr == NULL)
    {
		if(outbuf_ptr != NULL)
		{
			SCI_FREE(outbuf_ptr);
			outbuf_ptr = NULL;
		}
        return 0;
    }

	SCI_MEMSET(outbuf_ptr, 0x00, outbuf_size);
	SCI_MEMSET(probuf_ptr, 0x00, probuf_size);

    //set input param   
    input_param.src_ptr     = data;
    input_param.out_buf_ptr = outbuf_ptr;
    input_param.out_buf_size= outbuf_size;
    input_param.input_width = w;
    input_param.input_height= h;
    input_param.target_width  = w;
    input_param.target_height = h;
    input_param.in_type  = PEPROCESS_IMAGE_FORMAT_RGB_565;
    input_param.out_type = PEPROCESS_IMAGE_FORMAT_JPEG;
    
    //set process_param_ptr param
    process_param.process_buf_ptr  = probuf_ptr;
    process_param.process_buf_size = probuf_size;
    process_param.type = PEPROCESS_ENCODE_JPEG;
    
    //image encode
#ifdef IMG_EDITER_SUPPORT
    ref_return = PEPROCESS_ImageProcess(&input_param, &output_param, &process_param);
#else
	ref_return = PEPROCESS_NO_SUPPORT;
#endif
    if(PEPROCESS_OK == ref_return)
    {
        read = MIN(read, output_param.compress_out_size);
        SCI_MEMSET((uint8*)data, 0x00, len);
        SCI_MEMCPY((uint8*)data, input_param.out_buf_ptr, read);
    }
    else
    {
        read = 0;
		SCI_MEMSET((uint8*)data, 0x00, len);
    }

	if(outbuf_ptr)
	{
		SCI_FREE(outbuf_ptr);
		outbuf_ptr = NULL;
	}

	if(probuf_ptr)
	{
		SCI_FREE(probuf_ptr);
		probuf_ptr = NULL;
	}

    MMIMR_Trace(("[SKYSHORTCUT] mr_skybmp_get_data len = %d read = %d ref_return = %d", len, read, ref_return));

	return read;
}

void mr_skybmp_close(uint32 f)
{
	SFS_CloseFile(f);
}

int32 mr_scut_save_data(int32 appId, MR_SHORTCUT_APPINFO_T *appInfo)
{
    int32 fInfo = 0;
    int32 fHandle = 0;
    uint8 fName[SFS_MAX_PATH] = {0};

    if(appInfo != NULL && appId != 0)
    {
        if(mr_info("dam") != MR_IS_DIR)
        {
            mr_mkDir("dam");
        }
        sprintf(fName, "dam\\%d.dat", appId);
        fInfo = mr_info(fName);
        if(fInfo == MR_IS_FILE)
        {
            mr_remove(fName);
        }
        
        fHandle = mr_open(fName, MR_FILE_WRONLY|MR_FILE_CREATE);
        if(fHandle > 0)
        {
            mr_write(fHandle, (void *)appInfo, sizeof(MR_SHORTCUT_APPINFO_T));
        }
        mr_close(fHandle);

        return MR_SUCCESS;
    }

    return MR_FAILED;
}

int32 mr_scut_read_data(int32 appId, MR_SHORTCUT_APPINFO_T *appInfo)
{
    int32 fInfo = 0;
    int32 fHandle = 0;
    uint8 fName[SFS_MAX_PATH] = {0};

    if(appInfo != NULL && appId != 0)
    {
        sprintf(fName, "dam\\%d.dat", appId);
        fInfo = mr_info(fName);

        if(fInfo == MR_IS_FILE)
        {
            fHandle = mr_open(fName, MR_FILE_RDONLY);
            if(fHandle > 0)
            {
                mr_read(fHandle, (void *)appInfo, sizeof(MR_SHORTCUT_APPINFO_T));
                mr_close(fHandle);
                return MR_SUCCESS;
            }
        }
    }

    return MR_FAILED;
}

void mr_scut_remove_data_file(int32 appId)
{
    int32 fInfo = 0;
    uint8 fName[SFS_MAX_PATH] = {0};

    if(appId != 0)
    {
        sprintf(fName, "dam\\%d.dat", appId);
        fInfo = mr_info(fName);

        if(fInfo == MR_IS_FILE)
        {
            mr_remove(fName);
        }
    }
}

int32 mr_scut_refresh_icon(uint32 menuId, MR_SHORTCUT_APPINFO_T appInfo)
{
    int32 has_icon = 0;
    int32 fState = 0;
    BOOLEAN ret = FALSE;
    BOOLEAN is_found = FALSE;
    int16 imgW = 0;
    int16 imgH = 0;
    uint8 isSkyBmp = 0;
    uint32 fHandle = 0;
    uint32 dataLen = 0;
    uint32 imageSize = 0;
    uint8 *iconbuf = NULL;
    uint16 fullpath[SFS_MAX_PATH] = {0};  
	GUIANIM_IMG_INFO_T img_info = {0};
    GUIMENU_MAINMENU_DY_ITEM_T menu_item = {0};


    fState = mr_info(appInfo.image_path);

    if(fState != MR_IS_FILE)
    {
        return MR_FAILED;
    }

    is_found = MMIMENU_DySlideHasIcon(menuId, &has_icon);

    if(is_found && !has_icon)
    {
        ret = MMIMENU_DySlideGetItemInfo(menuId, &menu_item);
		if(ret == TRUE)
        {
            mr_fs_get_filename(fullpath, appInfo.image_path);
            fHandle = mr_skybmp_open(fullpath, &isSkyBmp);
            if(fHandle > 0)
            {
                if(isSkyBmp == 1)
                {
                    mr_skybmp_get_size(fHandle, &imgW, &imgH);
                    dataLen = imgW*imgH*2;
                    iconbuf = (uint8*)SCI_ALLOCA(dataLen);
                    if(iconbuf != NULL)
                    {
                        imageSize = mr_skybmp_get_data(fHandle, iconbuf, dataLen);
                    }
                }
                else
                {
                    SFS_GetFileSize(fHandle, &dataLen);
                    iconbuf = (uint8*)SCI_ALLOCA(dataLen);
                    if(iconbuf != NULL)
                    {
                        SFS_ReadFile(fHandle, (void*)iconbuf, dataLen, &imageSize, NULL);
        				GUIANIM_GetInfo(NULL, fullpath, PNULL, &img_info);
        				imgW = img_info.img_width;
        				imgH = img_info.img_height;
                    }
                }
                mr_skybmp_close(fHandle);
            	menu_item.ori_data_ptr->dynamic_menu_info.has_icon = TRUE;
            	menu_item.ori_data_ptr->dynamic_menu_info.icon_datasize = imageSize;
                menu_item.ori_data_ptr->dynamic_menu_info.icon_ptr = (uint8 *)iconbuf;
                menu_item.ori_data_ptr->dynamic_menu_info.icon_height = imgH;
                menu_item.ori_data_ptr->dynamic_menu_info.icon_width  = imgW;
                menu_item.ori_data_ptr->dynamic_menu_info.select_icon_id= IMAGE_MAINMENU_ICON_SELECT;
                ret = MMIMENU_DySlideSaveItemInfo(menuId, &menu_item);

                if(iconbuf != NULL)
                {
                    SCI_FREE(iconbuf);
                }
            }
        }      
    }    
    
    MMIMR_Trace(("[SKYSHORTCUT] mr_scut_refresh_icon menuId = %u ret = %d", menuId, ret));
    return ret;
}

void mr_scut_delete_app(void* param1,void *param2)
{
    int32 appId = (*(int32 *)param1);
	uint32 menuId = (*(uint32 *)param2);
    MR_SHORTCUT_APPINFO_T appInfo = {0};
    
    MMIMR_Trace(("[SKYSHORTCUT] mr_scut_delete_app appId = %u menuId = %d", appId, menuId));

    if(MR_SUCCESS == mr_scut_read_data(appId, &appInfo))  
    {
        if(MR_IS_FILE == mr_info(appInfo.filepath))
        {
            mr_remove(appInfo.filepath);
            mr_scut_remove_data_file(appId);
        }
    }
}

int32 mr_scut_support(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef DYNAMIC_MAINMENU_SUPPORT
    return MR_SUCCESS;
#else
    return MR_IGNORE;
#endif
}

int32 mr_scut_install_app(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    int16 imgW = 0;
    int16 imgH = 0;
    uint8 isSkyBmp = 0;
    uint32 fHandle = 0;
    uint32 dataLen = 0;
    uint32 imageSize = 0;
    uint8 *iconbuf = NULL;
    BOOLEAN ret = FALSE;
    uint16 fullpath[SFS_MAX_PATH] = {0};  
	GUIANIM_IMG_INFO_T img_info = {0};
    GUIMENU_FILE_DEVICE_E store_idx = GUIMENU_DEVICE_UDISK;
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr = PNULL;
	MR_SHORTCUT_APPINFO_T* appinfo = (MR_SHORTCUT_APPINFO_T* )input;

	if(!input || input_len != sizeof(MR_SHORTCUT_APPINFO_T)
		|| appinfo->filepath[0] == 0)
	{
		return MR_FAILED;
	}

    MMIMR_Trace(("[SKYSHORTCUT] mr_scut_install_app appId = %u", appinfo->appid));
    MMIMR_Trace(("[SKYSHORTCUT] total_size = %d cur_size = %d", appinfo->total_size, appinfo->cur_size));
    MMIMR_Trace(("[SKYSHORTCUT] provider = %d startParam = %s", appinfo->provider, appinfo->start_data));
    MMIMR_Trace(("[SKYSHORTCUT] filePath = %s", appinfo->filepath));
    MMIMR_Trace(("[SKYSHORTCUT] imagePath = %s", appinfo->image_path));
    MMIMR_Trace(("[SKYSHORTCUT] title_en = %s", appinfo->title_en));
    MMIMR_Trace(("[SKYSHORTCUT] title_mt_len = %d", MMIAPICOM_Wstrlen(appinfo->title_mt)));

    dynamic_item_ptr = SCI_ALLOCA(sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T));
    if (dynamic_item_ptr == PNULL)
    {
        MMIMR_Trace(("[SKYSHORTCUT]mr_scut_install_app: alloc dynamic_item_ptr return PNULL"));    
        return MR_FAILED;
    }
    SCI_MEMSET(dynamic_item_ptr, 0, sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T));
#ifndef WIN32   
    mr_fs_get_filename(fullpath, appinfo->image_path);
    fHandle = mr_skybmp_open(fullpath, &isSkyBmp);
    MMIMR_Trace(("[SKYSHORTCUT] mr_skybmp_open: fHandle = 0x%08x isSkyBmp = %d", fHandle, isSkyBmp));    
    if(fHandle > 0)
    {
        if(isSkyBmp == 1)
        {
            mr_skybmp_get_size(fHandle, &imgW, &imgH);
            dataLen = imgW*imgH*2;
            iconbuf = (uint8*)SCI_ALLOCA(dataLen);
            if(iconbuf != NULL)
            {
                imageSize = mr_skybmp_get_data(fHandle, iconbuf, dataLen);
            }
        }
        else
        {
            SFS_GetFileSize(fHandle, &dataLen);
            iconbuf = (uint8*)SCI_ALLOCA(dataLen);
            if(iconbuf != NULL)
            {
                SFS_ReadFile(fHandle, (void*)iconbuf, dataLen, &imageSize, NULL);
				GUIANIM_GetInfo(NULL, fullpath, PNULL, &img_info);
				imgW = img_info.img_width;
				imgH = img_info.img_height;
            }
        }
        mr_skybmp_close(fHandle);
    	dynamic_item_ptr->dynamic_menu_info.has_icon = TRUE;
    	dynamic_item_ptr->dynamic_menu_info.icon_datasize = imageSize;
        dynamic_item_ptr->dynamic_menu_info.icon_ptr = (uint8 *)iconbuf;
        dynamic_item_ptr->dynamic_menu_info.icon_height = imgH;
        dynamic_item_ptr->dynamic_menu_info.icon_width  = imgW;
        dynamic_item_ptr->dynamic_menu_info.select_icon_id= IMAGE_MAINMENU_ICON_SELECT;
    }
    else
#endif
    {
        dynamic_item_ptr->dynamic_menu_info.has_icon = FALSE;
    }

	if(appinfo->title_mt != NULL)
	{
		mr_str_convert_endian(appinfo->title_mt);
	}
    MMIAPICOM_Wstrcpy(dynamic_item_ptr->dynamic_menu_info.text, appinfo->title_mt);
    dynamic_item_ptr->dynamic_menu_info.text_len = MIN(MMIAPICOM_Wstrlen(appinfo->title_mt), GUIMENU_DYMAINMENU_STR_MAX_NUM);

    dynamic_item_ptr->dynamic_menu_info.link_function_ptr  = mr_scut_start_app;
    dynamic_item_ptr->dynamic_menu_info.delete_function_ptr= mr_scut_delete_app;
    dynamic_item_ptr->dynamic_menu_info.param1ptr = &appinfo->appid;
    dynamic_item_ptr->dynamic_menu_info.param2ptr = &dynamic_item_ptr->menu_id;
    dynamic_item_ptr->dynamic_menu_info.parm1_size = 4;
    dynamic_item_ptr->dynamic_menu_info.parm2_size = 4;
    
    ret = MMIMENU_DyAppendItem(dynamic_item_ptr, store_idx);

    MMIMR_Trace(("[SKYSHORTCUT] mr_scut_install_app: menu_id = %d ret = %d", dynamic_item_ptr->menu_id, ret));

    if(dynamic_item_ptr != NULL)
    {
        SCI_FREE(dynamic_item_ptr);
    }

    if(iconbuf != NULL)
    {
        SCI_FREE(iconbuf);
    }

    if(ret == TRUE)
    {
        mr_scut_save_data(appinfo->appid, appinfo);
        return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

int32 mr_scut_update_progress(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	MR_SHORTCUT_PROGRESS_T* progress = (MR_SHORTCUT_PROGRESS_T*)input;

	return MR_IGNORE;
}

int32 mr_scut_remove_app(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    int32 nAppNameLen = 0;
    BOOLEAN   ret = TRUE;
    GUIMENU_MAINMENU_DYNAMIC_INFO_T dynamic_item_info = {0};
	MR_SHORTCUT_REMOVE_T* pRemoveInfo = (MR_SHORTCUT_REMOVE_T*)input;
    MR_SHORTCUT_APPINFO_T appInfo = {0};
    
	if(!input || input_len != sizeof(MR_SHORTCUT_REMOVE_T))
		return MR_FAILED;

    if(MR_SUCCESS != mr_scut_read_data(pRemoveInfo->appid, &appInfo))
    {
        return MR_FAILED;
    }

    nAppNameLen = MMIAPICOM_Wstrlen(appInfo.title_mt);
    nAppNameLen = nAppNameLen>GUIMENU_DYMAINMENU_STR_MAX_NUM?GUIMENU_DYMAINMENU_STR_MAX_NUM:nAppNameLen;
    MMIAPICOM_Wstrncpy(dynamic_item_info.dynamic_menu_info.text, appInfo.title_mt, nAppNameLen);
    dynamic_item_info.dynamic_menu_info.link_function_ptr = mr_scut_start_app;

    ret = MMIMENU_DyDeleteItem(&dynamic_item_info);
    MMIMR_Trace(("[SKYSHORTCUT] mr_scut_remove_app: appid = %d provider = %d ret = %d", pRemoveInfo->appid, pRemoveInfo->provider, ret));

    if(ret == TRUE)
    {
        mr_scut_remove_data_file(pRemoveInfo->appid);
    	return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

void test_dam_install(void)
{
	MR_SHORTCUT_APPINFO_T appinfo = {0};

	memset(&appinfo, 0, sizeof(appinfo));

	appinfo.appid = 10000;
	strcpy(appinfo.title_mt, L"测试");
	strcpy(appinfo.title_en, "test");
	strcpy(appinfo.image_path, "test.jpg");
	strcpy(appinfo.filepath, "test.mrp");
    strcpy(appinfo.start_data, "test,10000");

	MMIMRAPP_setDefFilePath();
	mr_scut_install_app((uint8*)&appinfo, sizeof(appinfo), NULL, NULL, NULL);
}

void test_dam_remove(void)
{
	MR_SHORTCUT_REMOVE_T appinfo;

	memset(&appinfo, 0, sizeof(appinfo));

	appinfo.appid = 10000;

	mr_scut_remove_app((uint8*)&appinfo, sizeof(appinfo), NULL, NULL, NULL);
}
#endif

int32 mr_setting_get_hw_version(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
   const char *hw_ver = NULL;

	if(output == NULL||output_len == NULL)
		return MR_FAILED;

#if defined(PLATFORM_SC6530)
    hw_ver = "SC6530";
#elif defined(PLATFORM_SC6600L)
    hw_ver = "SC6600L";
#elif defined(PLATFORM_SC6600R)
    hw_ver = "SC6600R";
#elif defined(PLATFORM_SC6800H)
    hw_ver = "SC800H";
#else
    return MR_IGNORE;
#endif

	*output =(uint8*) hw_ver;
	*output_len = strlen((char*)*output)+1;
    
	MMIMR_Trace(("mr_setting_get_hw_version hw_ver=%s len=%d", *output, *output_len));
	return MR_SUCCESS;
}
/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_mutiCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 resValue = MR_SUCCESS;
	MMIMR_Trace(("MMIMRAPP_mutiCmd() code:%d", code));
	
	switch(code)
	{	
		//2009-7-7 add begin
#ifdef __MRAPP_OVERSEA__
		case MR_GET_DATA_ACCOUNT:
		{
			if(output == NULL||output_len == NULL)
				return MR_FAILED;
			*output = (uint8 *)&gDsmDataAccount;
			*output_len = sizeof(gDsmDataAccount);
			return MR_SUCCESS;
		}
    
		case MR_SET_DATA_ACCOUNT:
		{
			if(input == NULL||input_len != (int32)sizeof(T_DSM_DATA_ACCOUNT))
				return MR_FAILED;
			return MMIMRAPP_setApnAccount((T_DSM_DATA_ACCOUNT*)input);
		}
#endif
		//2009-7-7 add end
		
	case PLATEX_CREATE_LIST:
		{
			MMIMR_LISTBOXSELECT_T *list = (MMIMR_LISTBOXSELECT_T*)input;
			return MMIMRAPP_listCreate((const char *)list->title, list->itemNum, list->listType);
		}
    
	case PLATEX_SET_LISTITEM:
		{
			MMIMR_LISTBOXITEM_T *item = (MMIMR_LISTBOXITEM_T*)input;
			return MMIMRAPP_menuSetItem(item->menu, (const char*)item->text, item->index, item->selected);
		}
    
    /*modify by zack@20100812 start*/
	case PLATEX_GET_MP3_DIR:
        {
            SCI_MEMSET(ucs2_name_arr, 0x00, sizeof(ucs2_name_arr));
            *output_len = GUI_WstrToGB((uint8*) ucs2_name_arr, (uint16*)MMIMULTIM_DIR_MUSIC, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC));
            //SCI_TRACE_LOW:"MP3: %d 0x%02x 0x%02x 0x%02x 0x%02x"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_FUNC_6212_112_2_18_3_12_45_0,(uint8*)"ddddd", *output_len, ucs2_name_arr[0], ucs2_name_arr[1], ucs2_name_arr[2], ucs2_name_arr[3]);
			*output = (uint8*)ucs2_name_arr;
			break;
        }
        
    case PLATEX_GET_PIC_DIR:
        {
            SCI_MEMSET(ucs2_name_arr, 0x00, sizeof(ucs2_name_arr));
            *output_len = GUI_WstrToGB((uint8*) ucs2_name_arr, (uint16*)MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE));
            //SCI_TRACE_LOW:"PIC: %d 0x%02x 0x%02x 0x%02x 0x%02x"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_FUNC_6221_112_2_18_3_12_45_1,(uint8*)"ddddd", *output_len , ucs2_name_arr[0], ucs2_name_arr[1], ucs2_name_arr[2], ucs2_name_arr[3]);
    		*output = (uint8*)ucs2_name_arr;
    		break;
        }
    /*modify by zack@20100812 end*/        
    
	case PLATEX_APP_BACKRUN_IND:
        {
            mr_backstage_st *ptr = (mr_backstage_st*)input;
    		MMIMR_Trace(("mrapp PLATEX_APP_BACKRUN_IND g_mr_vmBackRun = %d, g_mr_curApp = %d, param = %d", g_mr_vmBackRun, g_mr_curApp, ptr->appid));
    		if(g_mr_vmBackRun == MRAPP_MAX && ptr != NULL && ptr->appid > 0)
    		{
    			g_mr_vmBackRun = ptr->appid;
    		    MMIMRAPP_closeAllWins();
    			/*如果mp3是在启动mr 应用时pause的，恢复*/
    			s_mr_mp3_resume_locked = FALSE;
				MMIMRAPP_resumeMp3();

                /* 为后台应用添加默认图标 */
                s_mr_backrun_info.app_id = ptr->appid;
                s_mr_backrun_info.icon_state = MRAPP_STATE_ONLINE;
                MAIN_SetIdleMrappState(TRUE); /*lint !e718 !e18*/
    		}
    		else
    		{
    			resValue = MR_FAILED;
    		}
    		break;
       }
        
	case PLATEX_APP_PICIND_REQ:
		MMIMR_Trace(("DSMQQ PLATEX_APP_PICIND_REQ g_mr_vmBackRun = %d, g_mr_curApp = %d", g_mr_vmBackRun, g_mr_curApp));
		/*modify by zack@20110726 start*/
		if(g_mr_vmBackRun != MRAPP_MAX || input != NULL) /*lint !e831*/
		{
			mr_pic_req *pic = (mr_pic_req*)input;
            s_mr_backrun_info.app_id = pic->appid;
            if(pic->time == -1) //离线
            {
                s_mr_backrun_info.icon_state = MRAPP_STATE_OFFLINE;
            }
            else if(pic->time == 1 || pic->time == 1001)  //离开
            {
                s_mr_backrun_info.icon_state = MRAPP_STATE_AWAY;
            }
            else if(pic->time == 2 || pic->time == 1002)  //隐身
            {
                s_mr_backrun_info.icon_state = MRAPP_STATE_INVISIBLE;
            }
            else//在线
            {
                s_mr_backrun_info.icon_state = MRAPP_STATE_ONLINE;
            }
            MAIN_SetIdleMrappState(TRUE); /*lint !e718 !e18*/
            if(pic->time >= 1000) //有消息
            {
                MMIMRAPP_StartStatusIconTimer();
            }
		}
		else
		{
			resValue = MR_FAILED;
		}
		/*modify by zack@20110726 start*/
		break;
        
#ifdef MR_USE_PLAT_LCD_BUFFER
	case PLATEX_GET_LCD_INFO://获取LCD buffer指针与大小
		{
/*modify by zack@20120725 start*/
#ifndef UI_MULTILAYER_SUPPORT
			BOOLEAN    ret = FALSE;
			LCD_INFO_T lcd_info = {0};
            ret = GUILCD_GetInfo(GUI_MAIN_LCD_ID, &lcd_info);  
			if(ret == TRUE)
			{
				*output = (uint8 *)lcd_info.lcdbuff_ptr;
				*output_len = (int32)(lcd_info.lcd_width*lcd_info.lcd_height*lcd_info.bits_per_pixel/8);
			}
			else
			{
				resValue = MR_FAILED;
			}
#else
            GUI_LCD_DEV_INFO *lcd_dev_ptr = MMITHEME_GetDefaultLcdDev();
            UILAYER_INFO_T   layer_info = {0};
            UILAYER_GetLayerInfo(lcd_dev_ptr, &layer_info);
#if (SPR_VERSION >= 0x10A1140)
			{
				GUI_COLOR_T *lcd_buffer_ptr = (GUI_COLOR_T*)UILAYER_GetLayerBufferPtr(lcd_dev_ptr);
				MMIMR_Trace(("mrapp layer_buf_ptr = 0x%08x width = %d height = %d mem_width = %d", lcd_buffer_ptr, layer_info.layer_width, layer_info.layer_height, layer_info.mem_width));
				if(output != NULL && output_len != NULL && lcd_buffer_ptr != NULL)
				{
					*output = (uint8 *)lcd_buffer_ptr;
					*output_len = (int32)(layer_info.layer_width*layer_info.layer_height*2);
				}
				else
				{
					resValue = MR_FAILED;
				}
			}
#else
            MMIMR_Trace(("mrapp layer_buf_ptr = 0x%08x width = %d height = %d mem_width = %d", layer_info.layer_buf_ptr, layer_info.layer_width, layer_info.layer_height, layer_info.mem_width));
			if(output != NULL && output_len != NULL && layer_info.layer_buf_ptr != NULL)
			{
				*output = (uint8 *)layer_info.layer_buf_ptr;
				*output_len = (int32)(layer_info.layer_width*layer_info.layer_height*2);
			}
			else
			{
				resValue = MR_FAILED;
			}
#endif
#endif
/*modify by zack@20120725 end*/
		}
		break;
	case PLATEX_FREE_LCD_BUF:
		return MR_SUCCESS;
#endif

	case PLATEX_ALLOC_MEM:
		{
            uint8 *buf = NULL;
            if(output == NULL||output_len == NULL||input_len == 0)
            {
                resValue = MR_FAILED;
            }
            else
            {
                buf = (uint8*)SCI_ALLOCA(input_len);
                if(buf != NULL)
                {
                    SCI_MEMSET(buf, 0x00, input_len);
                    *output = (uint8 *)buf;
                    *output_len =sizeof(int32);
                    resValue = MR_SUCCESS;
                }
                else
                {
                    resValue = MR_FAILED;
                }
            }
            MMIMR_Trace(("mrapp PLATEX_ALLOC_MEM len = %d, buf = 0x%08x, resValue = %d", input_len, buf, resValue));
		}
		break;
        
	case PLATEX_FREE_MEM:
        {
            if(input != NULL)
            {
                SCI_FREE(input);
                input = NULL;
                resValue = MR_SUCCESS;
            }
            else
            {
                resValue = MR_FAILED;
            }
            MMIMR_Trace(("mrapp PLATEX_FREE_MEM resValue = %d", resValue));
        }
		break;

    case PLATEX_GET_BUILD_DT:
        {
			if(output == NULL || output_len == NULL)
            {         
				return MR_FAILED;
            }
            else
            {
                return MMIMRAPP_GetBuildTime(output, output_len);
            }
        }
       

#ifndef WIN32
	case PLATEX_SOCKET_BIND://socket－bind
		{
			MRAPP_BIND_T *info = (MRAPP_BIND_T*)input;
			struct sci_sockaddr  addr;
			int ret;
			
			addr.family = AF_INET;
			addr.port = htons(info->port);
			addr.ip_addr = (uint32)sci_gethostaddress();
			SCI_MEMSET((void*)addr.sa_data, 0, 8*sizeof(char));
			
			ret = sci_sock_bind((long)info->socket, &addr, sizeof(struct sci_sockaddr));
			if(ret != 0)
			{
				resValue = MR_FAILED;
			}
		}
		break;
#endif

#ifdef __MR_SEND_MMS_SUPPORT__
    case PLATEX_SEND_MMS:
        return mr_sms_send_mms(input, input_len, output, output_len, cb);
#endif

	case PLATEX_SET_FILE_DEV://存储路径选择
		if(input != NULL && input_len > 0)
		{
			uint8 *ptr = input;
			uint16 len;
			const uint8 *device_ptr;
			uint16 device_len;
			FILE_DEV_E_T device_type;
			char* tempPtr = (char*)tempTransBuf; /*lint !e831*/
			
			MMIMR_Trace(("input: %s input_len:%d output:0x%x output_len:0x%x cb:0x%x", input, input_len, output, output_len, cb));
			switch((char)ptr[0])
			{
			case 'A':case 'a':
				device_type = FS_HS;
				device_ptr = (const uint8 *)MMIFILE_DEVICE_SYS_NAME;
				device_len = MMIFILE_DEVICE_SYS_NAME_LEN;
				break;
			case 'B':case 'b':
				device_type = FS_UDISK;
				device_ptr = (const uint8 *)MMIFILE_DEVICE_UDISK;
				device_len = MMIFILE_DEVICE_UDISK_LEN;
				break;
			case 'C':case 'c':
#ifdef DUAL_TCARD_SUPPORT
                if(TRUE == MMIFILE_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
                {
    				device_type = FS_MMC;
    				device_ptr = (const uint8 *)MMIFILE_DEVICE_SDCARD;
    				device_len = MMIFILE_DEVICE_SDCARD_LEN;
                }
                else if(TRUE == MMIFILE_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
                {
                    device_type = FS_MMC;
                    device_ptr = (const uint8 *)MMIFILE_DEVICE_SDCARD2;
                    device_len = MMIFILE_DEVICE_SDCARD2_LEN;
                }
                else
#endif
                {
    				device_type = FS_MMC;
    				device_ptr = (const uint8 *)MMIFILE_DEVICE_SDCARD;
    				device_len = MMIFILE_DEVICE_SDCARD_LEN;
                }
				break;
#ifdef DOUBLE_TFCARD
			case 'D':case 'd':
				device_type = FS_MMC_2;
				device_ptr = (const uint8 *)MMIFILE_DEVICE_SDCARD_2;
				device_len = MMIFILE_DEVICE_SDCARD_LEN_2;
				break;
#endif
			case 'Y':case 'y':
				if(output != NULL && output_len != NULL)
				{
    				MMIMR_Trace(("mrapp device type %d", g_mr_global.device_type));
					SCI_MEMSET(tempPtr, 0, sizeof(tempTransBuf));
					if(g_mr_global.device_type == FS_MMC)
						strcpy((char*)tempPtr, "C:\\");
					else if(g_mr_global.device_type == FS_UDISK)
						strcpy((char*)tempPtr, "B:\\");
					else
						strcpy((char*)tempPtr, "A:\\");
					
					if(g_mr_global.dir_len != 0)
					{
						GUI_WstrToGB((uint8*)tempPtr+3, g_mr_global.dir_ptr, g_mr_global.dir_len/2); /*lint !e64*/
						strcat((char*)tempPtr, "\\");
					}
					len = strlen((char*)tempPtr);
					MMIMRAPP_ConvertPath2Loc((char*)tempPtr, len);
					
					MMIMR_TraceF(("after tempPtr: %s", (char*)tempPtr));
					*output = (uint8*)tempPtr;
					*output_len = len;
                    if(cb != NULL)
                    {
    					*cb = NULL;
                    }
					return MR_SUCCESS;
				}
				else
				{
					return MR_FAILED;
				}
			case 'X':case 'x':
			case 'Z':case 'z':
				MMIMRAPP_setDefFilePath();
				return MR_SUCCESS;
			default:
				return MR_FAILED;
			}	
			
			if(!MMIAPIFMM_GetDeviceStatus((uint8*)device_ptr, device_len)) /*lint !e64*/
				return MR_FAILED;
			MMIMR_TraceF(("mrapp get device status OK"));
            if(!mr_fs_check_path_valid(input, input_len))
            {
                MMIMR_TraceF(("mrapp mr_fs_check_path_invalid "));
                return MR_FAILED;
            }
			g_mr_global.device_type = device_type;
			memset(g_mr_global.dir_buf, 0, sizeof(g_mr_global.dir_buf));
			g_mr_global.dir_ptr = (uint8*)g_mr_global.dir_buf;
			g_mr_global.dir_len = 0;
			if(input_len > 3 && ptr[1] == ':' && (ptr[2] == '\\' || ptr[2] == '/'))//"C:\App\"-->"/App"
			{
				len = input_len - 3;
				if(len >= MMIFILE_DIR_NAME_MAX_LEN_AD/2)//simple deal
					return MR_FAILED;
				
				SCI_MEMCPY((char*)tempPtr, &ptr[3], len+1);//add '\0'
				MMIMRAPP_ConvertPath2Loc((char*)tempPtr, len);
				if(tempPtr[len-1] == '/')  /*lint !e676*/
					tempPtr[len-1] = '\0'; /*lint !e676*/
				g_mr_global.dir_len = MMIMRAPP_GB2UCS((uint16*)g_mr_global.dir_ptr, (const uint8 *)tempPtr, strlen((char*)tempPtr));
			}
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
		
	case PLATEX_UCS_TO_GB:
		{
			char *temp = NULL;
			int32 lenth = 0;
			uint16 len;
			uint8 *out = (uint8*)*output;
#ifndef WIN32
			if(input != NULL)
			{
				len = mr_str_wstrlen((char*)input) + 2;
				temp = (char*)SCI_ALLOCA(len);
				SCI_MEMCPY(temp, input, len);
				mr_str_convert_endian((char*)temp);
			}
#endif
			len = GUI_WstrToGB(out, (const uint8*)temp, MMIMRAPP_getUcstrLen(input)/2); /*lint !e64*/
			out[len] = 0x00;
			MMIMR_Trace(("len: %d", len));
			SCI_FREE(temp);
		}
		break;

	case PLATEX_SET_RINGS:
		resValue = MMIMRAPP_SetMp3AsRing((MR_RING_SET_T*)input);
		break;

#ifdef MRAPP_SIMUGAME_OPEN
	case PLATEX_GET_NES_KEYSCONFIG://获取平台默认按键设置for游戏模拟器
		{
			mr_nes_keys_st keys = NES_DEFAULT_KEYSET;
			mr_nes_keys_st *defaultKeys = (mr_nes_keys_st*)tempTransBuf;
			memcpy(defaultKeys, &keys, sizeof(mr_nes_keys_st));
			*output = (uint8*)defaultKeys;
			*output_len = sizeof(mr_nes_keys_st);
		}
		break;
		
	case PLATEX_GET_NES_DIR_NAME:
		*output = (uint8*)MMIMRAPP_NES_DIR_NAME;
		*output_len = sizeof(MMIMRAPP_NES_DIR_NAME)-1;
		break;
#endif

    case PLATEX_GET_NES_TITLE_NAME:
        {
            if(output == NULL || output_len == NULL)
                return MR_FAILED;
        	*output = (uint8 *)MRAPP_APPLIST_TITLE;
        	*output_len = strlen(MRAPP_APPLIST_TITLE);
        }
        break;

	case PLATEX_LCD_SLEEP_OFF:
        return MMIMRAPP_SetAllowTurnOffBacklight(FALSE);

	case PLATEX_LCD_SLEEP_ON:
        return MMIMRAPP_SetAllowTurnOffBacklight(TRUE);
        
	case PLATEX_GET_CELL_INFO:
		{
			char temp[4] = {0};
			MN_PHONE_CURRENT_PLMN_INFO_T info = MNPHONE_GetCurrentPLMNInfoEx(MMIMRAPP_GetActiveSim());
			MRAPP_CELL_INFO_T *cells = (MRAPP_CELL_INFO_T*)tempTransBuf;
            MMIMR_Trace(("mrapp PLATEX_GET_CELL_INFO plmn_status=%d, plmn_is_roaming=%d, mcc=%d, mnc=%d, lac=%d, cell_id=%d, mnc_digit_num=%d", \
                info.plmn_status, info.plmn_is_roaming, info.mcc, info.mnc, info.lac, info.cell_id, info.mnc_digit_num));
			if(output == NULL||output_len == NULL)
            {         
				return MR_FAILED;
            }
			memset(cells, 0, sizeof(MRAPP_CELL_INFO_T));
			cells->lac = info.lac;
			cells->cell_id = info.cell_id;		
			sprintf((char*)temp, "%03d", info.mnc);
			memcpy(cells->mnc, temp, 2);
			memcpy(cells->mnc3, &temp[2], 1);
			sprintf((char*)temp, "%03d", info.mcc);
			memcpy(cells->mcc, temp, 3);
			*output = (uint8*)cells;
			*output_len = sizeof(MRAPP_CELL_INFO_T);
			return MR_SUCCESS;
		}

	case PLATEX_GET_NETWORK_ID:
		{
            if(output == NULL||output_len == NULL)
            {         
                return MR_FAILED;
            }
            else
            {
                return MMIMRAPP_GetNetworkID(output, output_len);
            }
		}

    case PLATEX_GET_FILE_CRTTIME:
        return 	MMIMRAPP_GetFileCreateTime(input, input_len, output, output_len);
                
	case PLATEX_GET_DISK_SPACE:		
        return mr_fs_get_free_space(input, input_len, output, output_len, cb);
        
	case PLATEX_GET_DISK_NAME://unicode
		{
			MMISET_LANGUAGE_TYPE_E language_type;
			MMIAPISET_GetLanguageType(&language_type);
			
			resValue = MR_FAILED;
			switch((char)input[0])
			{
			case 'A':case 'a':	
				if(MMIAPIFMM_GetDeviceStatus((uint8 *)MMIFILE_DEVICE_SYS_NAME, MMIFILE_DEVICE_SYS_NAME_LEN)) /*lint !e64*/
				{
					if(language_type == MMISET_LANGUAGE_SIMP_CHINESE)
					{
						*output = (uint8*)"\x7c\xfb\x7e\xdf\x76\xd8\x00\x00";//系统盘
						*output_len = sizeof("\x7c\xfb\x7e\xdf\x76\xd8\x00\x00")-1; 
					}
					else
					{
						*output = (uint8*)"\x00\x53\x00\x79\x00\x73\x00\x64\x00\x69\x00\x73\x00\x6b\x00\x00";//Sysdisk
						*output_len = sizeof("\x00\x53\x00\x79\x00\x73\x00\x64\x00\x69\x00\x73\x00\x6b\x00\x00")-1; 
					}					
					resValue = MR_SUCCESS;
				}
				break;
			case 'B':case 'b':
				if(MMIAPIFMM_GetDeviceStatus((uint8*)MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN)) /*lint !e64*/
				{
					if(language_type == MMISET_LANGUAGE_SIMP_CHINESE)
					{
						*output = (uint8*)"\x62\x4b\x67\x3a\x51\x85\x5b\x58\x00\x00";//手机内存
						*output_len = sizeof("\x62\x4b\x67\x3a\x51\x85\x5b\x58\x00\x00")-1; 
					}
					else
					{
						*output = (uint8*)"\x00\x55\x00\x64\x00\x69\x00\x73\x00\x6b\x00\x00";//Udisk
						*output_len = sizeof("\x00\x55\x00\x64\x00\x69\x00\x73\x00\x6b\x00\x00")-1; 
					}					
					resValue = MR_SUCCESS;
				}
				break;
			case 'C':case 'c':	
#if (SPR_VERSION >= 0x10A1140)
#ifdef DUAL_TCARD_SUPPORT
                if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN)
                   || TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
#else
				if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#endif
#else
				if(MMIAPISD_GetStatus())
#endif
				{
#ifdef DOUBLE_TFCARD
					if(language_type == MMISET_LANGUAGE_SIMP_CHINESE)
					{
						*output = (uint8*)"\x5b\x58\x50\xa8\x53\x61\x00\x2d\x00\x31\x00\x00";//存储卡-1
						*output_len = sizeof("\x5b\x58\x50\xa8\x53\x61\x00\x2d\x00\x31\x00\x00")-1; 
					}
					else
					{
						*output = (uint8*)"\x00\x54\x00\x2d\x00\x63\x00\x61\x00\x72\x00\x64\x00\x31\x00\x00";//T-card1
						*output_len = sizeof("\x00\x54\x00\x2d\x00\x63\x00\x61\x00\x72\x00\x64\x00\x31\x00\x00")-1; 
					}					
#else
					if(language_type == MMISET_LANGUAGE_SIMP_CHINESE)
					{
						*output = (uint8*)"\x5b\x58\x50\xa8\x53\x61\x00\x00";//存储卡
						*output_len = sizeof("\x5b\x58\x50\xa8\x53\x61\x00\x00")-1; 
					}
					else
					{
						*output = (uint8*)"\x00\x54\x00\x2d\x00\x63\x00\x61\x00\x72\x00\x64\x00\x00";//T-card
						*output_len = sizeof("\x00\x54\x00\x2d\x00\x63\x00\x61\x00\x72\x00\x64\x00\x00")-1; 
					}					
#endif
					resValue = MR_SUCCESS;
				}
				break;
#ifdef DOUBLE_TFCARD
			case 'D':case 'd':	
				if(MMIAPISD2_GetStatus())
				{
					if(language_type == MMISET_LANGUAGE_SIMP_CHINESE)
					{
						*output = (uint8*)"\x5b\x58\x50\xa8\x53\x61\x00\x2d\x00\x32\x00\x00";//存储卡-2
						*output_len = sizeof("\x5b\x58\x50\xa8\x53\x61\x00\x2d\x00\x32\x00\x00")-1; 
					}
					else
					{
						*output = (uint8*)"\x00\x54\x00\x2d\x00\x63\x00\x61\x00\x72\x00\x64\x00\x32\x00\x00";//T-card2
						*output_len = sizeof("\x00\x54\x00\x2d\x00\x63\x00\x61\x00\x72\x00\x64\x00\x32\x00\x00")-1; 
					}
					resValue = MR_SUCCESS;
				}
				break;
#endif
			default:
				break;
			}
		}
		break;
        
	case PLATEX_GET_DUALSIM_INFO:
		{
            int32 i = 0;
			MR_DUALSIM_SET_T *simInfo = (MR_DUALSIM_SET_T*)tempTransBuf;
			memset(simInfo, 0, sizeof(MR_DUALSIM_SET_T));

            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
            {
                if (MMIPHONE_IsSimOk(i))
    			{
                    simInfo->simId[simInfo->totalNum] = -(i+1);
                    if(i == 0)
    				{
                        simInfo->simNamePtr[simInfo->totalNum] = (char*)"\x00\x53\x00\x49\x00\x4d\x00\x31\x00\x00";
    					simInfo->simNameLen[simInfo->totalNum] = sizeof("\x00\x53\x00\x49\x00\x4d\x00\x31\x00\x00")-1;
                    }
                    else if(i == 1)
                    {
                        simInfo->simNamePtr[simInfo->totalNum] = (char*)"\x00\x53\x00\x49\x00\x4d\x00\x32\x00\x00";
                        simInfo->simNameLen[simInfo->totalNum] = sizeof("\x00\x53\x00\x49\x00\x4d\x00\x32\x00\x00")-1;
                    }
                    else if(i == 2)
                    {
                        simInfo->simNamePtr[simInfo->totalNum] = (char*)"\x00\x53\x00\x49\x00\x4d\x00\x33\x00\x00";
                        simInfo->simNameLen[simInfo->totalNum] = sizeof("\x00\x53\x00\x49\x00\x4d\x00\x33\x00\x00")-1;
                    }   
                    else if(i == 3)
                    {
                        simInfo->simNamePtr[simInfo->totalNum] = (char*)"\x00\x53\x00\x49\x00\x4d\x00\x34\x00\x00";
                        simInfo->simNameLen[simInfo->totalNum] = sizeof("\x00\x53\x00\x49\x00\x4d\x00\x34\x00\x00")-1;
                    }  
					simInfo->totalNum++;
				}
                else
                {
                    if(g_mr_dualsys_mng.simSel == i)
    				{
                        g_mr_dualsys_mng.simSel = (i+1)%MMI_DUAL_SYS_MAX;
                    }
                }
			}
				
			resValue = MR_SUCCESS;
            *output = (uint8*)simInfo;
            *output_len = sizeof(MR_DUALSIM_SET_T); 
			
            for(i = 0; i < sizeof(simInfo->simId)/sizeof(int32); i++) /*lint !e574 !e737*/
			{
				if(i== g_mr_dualsys_mng.simSel)
				{
					simInfo->curActive =i;
					break;
				}
			}

            MMIMR_Trace(("simInfo total:%d, simSel:%d, curActive:%d simId:%d %d %d %d",
                          simInfo->totalNum, g_mr_dualsys_mng.simSel, simInfo->curActive, 
                          simInfo->simId[0], simInfo->simId[1], simInfo->simId[2], simInfo->simId[3]));
		}
		break;

#ifdef UI_MULTILAYER_SUPPORT
    case PLATEX_SET_BLT_LAYER:
        return mr_layer_set_blt_layer(input, input_len, output, output_len, cb);
    case PLATEX_CREATE_LAYER:
        return mr_layer_create(input, input_len, output, output_len, cb);
    case PLATEX_GET_BASE_LAYER:
        return mr_layer_get_base_handle(input, input_len, output, output_len, cb);
    case PLATEX_SET_LAYER_POS:
        return mr_layer_set_postion(input, input_len, output, output_len, cb);
    case PLATEX_SET_PNGBG_LAYER:
        return MR_IGNORE;
    case PLATEX_GET_ACTICE_LAYER:
        return mr_layer_get_active_layer(input, input_len, output, output_len, cb);
    case PLATEX_LAYER_APPEND:
        return MR_IGNORE;
    case PLATEX_LOCK_INVALIDATE:
        return MR_IGNORE;
    case PLATEX_UNLOCK_INVALIDATE:
        return MR_IGNORE;
#endif

    case PLATEX_BROWSE_FMGR_FILTER_INIT:
        return MMMRAPP_FileMgrInit();

    case PLATEX_BROWSE_FMGR_FILTER_SET:
        return MMMRAPP_FileMgrSetFilter(*(int32*)input);

    case PLATEX_BROWSE_FMGR_FILTER_CLEAR:
        return MMMRAPP_FileMgrCleanFilter(*(int32*)input);

    case PLATEX_BROWSE_FMGR:
        return MMMRAPP_FileMgrBrowser((MR_T_FMGR_REQ*)input);

    case PLATEX_BROWSE_FMGR_GET_PATH:
        return MMMRAPP_FileMgrGetPath(output, output_len);

    case PLATEX_BROWSE_FMGR_EXIT:
        return MMMRAPP_FileMgrExit();

	case PLATEX_CREATE_PB_OBJ:
		resValue = MR_FAILED;
		if(g_mr_pbinfo == NULL)
		{
			g_mr_pbinfo = (MRAPP_PBINFO_T*)SCI_ALLOCA(sizeof(MRAPP_PBINFO_T));
			if(g_mr_pbinfo != NULL)
			{
				SCI_MEMSET(g_mr_pbinfo,0,sizeof(MRAPP_PBINFO_T));
				//s_mr_inMrSelfWinCreating = TRUE;
				s_mr_PBOfCancelMsg = TRUE;
				s_mr_isWindowOfPB = TRUE;
				if(MMIAPIPB_OpenMainWin(MMIPB_ALL_CONTACT, 
										MMIPB_LIST_RADIO, 
										MMIPB_LIST_FIELD_WHOLE, 
										1, NULL, MMIMRAPP_SelectPBContactCallback))
				{
					resValue = MR_SUCCESS;
				}
				else
				{
					SCI_FREE(g_mr_pbinfo);
					g_mr_pbinfo = NULL;
				}		

				s_mr_isWindowOfPB = FALSE;
				//s_mr_inMrSelfWinCreating = FALSE;
			}
		}			
		break;

	case PLATEX_GET_A_PB_INFO:
		if(g_mr_pbinfo != NULL && g_mr_pbinfo->result != MR_FAILED)
		{
#ifndef WIN32
			/* 大小端转换 */
			mr_str_convert_endian_with_size(g_mr_pbinfo->info.name, MR_PHB_MAX_NAME);
			mr_str_convert_endian_with_size(g_mr_pbinfo->info.number, MR_PHB_MAX_NUMBER);
			mr_str_convert_endian_with_size(g_mr_pbinfo->info.homeNumber, MR_PHB_MAX_NUMBER);
			mr_str_convert_endian_with_size(g_mr_pbinfo->info.officeNumber, MR_PHB_MAX_NUMBER);
			mr_str_convert_endian_with_size(g_mr_pbinfo->info.faxNumber, MR_PHB_MAX_NUMBER);
			mr_str_convert_endian_with_size(g_mr_pbinfo->info.emailAddress, MR_PHB_MAX_NUMBER);
#endif
			
			*output = (uint8*)&g_mr_pbinfo->info;
			*output_len = sizeof(T_MR_PHB_ENTRY);
		}
		else
		{
			resValue = MR_FAILED;
		}
		break;
        
	case PLATEX_CLOSE_PB_OBJ:
		if(g_mr_pbinfo != NULL)
		{
			SCI_FREE(g_mr_pbinfo);
			g_mr_pbinfo = NULL;
		}
		break;
        
	case PLATEX_SET_TIMES:
		{
			MRAPP_MYTIME *time = (MRAPP_MYTIME*)input;
			SCI_TIME_T  sys_time;
			SCI_DATE_T  sys_date;
			sys_date.year = time->nYear;
			sys_date.mon = time->nMonth;
			sys_date.mday = time->nDay;
			sys_time.sec = time->nSec;
			sys_time.min = time->nMin;
			sys_time.hour = time->nHour;
            if(MMK_IsOpenWin(MMISET_SET_DATE_WIN_ID))
            {
                MMK_CloseWin(MMISET_SET_DATE_WIN_ID);
            }
            if(MMK_IsOpenWin(MMISET_SET_TIME_WIN_ID))
            {
                MMK_CloseWin(MMISET_SET_TIME_WIN_ID);
            }
			TM_SetSysDate(sys_date);
			TM_SetSysTime(sys_time);
			MMIAPISET_UpdateAlarmSet();
			MMIAPISET_SetNeedResetDateTime(FALSE);
            MAIN_SetStbDispTime(); /*lint !e718 !e18*/
		}
		break;

    case PLATEX_LOCK_IRAM:
        {
#ifndef WIN32
            if(output == NULL || output_len == NULL)
            {
                return MR_FAILED;
            }

    		MMIAPISET_StopAllRing(TRUE);
    		MMIAPISET_StopRing(MMISET_RING_TYPE_KEY);  //停止按键音
    		MMIAPISET_StopRing(MMISET_RING_TYPE_TKEY);  //停止触屏音
    		
    		s_mr_iramlock_handle = IRAM_LockArea((const IRAM_AREA *)&(s_mr_iram_area), 
    												sizeof(s_mr_iram_area)/sizeof(IRAM_AREA), 
    												IRAM_NO_WAIT);
    		
    		if(IRAM_INVALID_HANDLE != s_mr_iramlock_handle)//申请IRAM成功
    		{
 //               MMIDEFAULT_EnableKeyRing(TRUE);
                *(volatile uint32 *)0x8b00002c |= 0x01;
                *output = (uint8*)MRAPP_IRAM_ADDR;
                *output_len = MRAPP_IRAM_LEN;
    		}
    		else
    		{
    			resValue = MR_FAILED;//申请资源失败
    		}
            
            MMIMR_Trace(("mrapp PLATEX_LOCK_IRAM handle: %d iram_addr: 0x%08x size: %d", s_mr_iramlock_handle, *output, *output_len));
#endif
        }
		break;
        
    case PLATEX_UNLOCK_IRAM:
        {
#ifndef WIN32
            MMIMR_Trace(("mrapp PLATEX_UNLOCK_IRAM handle: %d input: 0x%08x input_len: %d", s_mr_iramlock_handle, input, input_len));
            if(input == NULL || input_len == 0)
            {
                return MR_FAILED;
            }

    		//释放IRAM
    		if(IRAM_INVALID_HANDLE != s_mr_iramlock_handle)
    		{
    			IRAM_UnlockArea (s_mr_iramlock_handle);
    			s_mr_iramlock_handle = IRAM_INVALID_HANDLE;
    			*(volatile uint32 *)0x8b00002c &= ~(0x01);
    		}
#endif
        }
		break;

	/* begin:added by Tommy.yan 20120201 */
	case PLATEX_GET_PB_TOTAL_NUM:
	    return MMIMRAPP_PHBGetTotalNum(input, input_len, output, output_len); /*lint !e718 !e746*/

	//case PLATEX_REPLY_SMS_REQ:
	    //return MMIMRAPP_ReplySmsByUI(input, input_len, output, output_len);

	//case PLATEX_SAVE_SMS_INBOX:
	    //return MMIMRAPP_SaveSmsInbox(input, input_len, output, output_len);

	case PLATEX_PHB_SEARCH_ENTRY:
		return MMIMRAPP_PHBSearchEntry(input, input_len, output, output_len); /*lint !e718 !e746*/
		
	case PLATEX_PHB_SET_ENTRY:
	    return MMIMRAPP_PHBSetEntry(input, input_len, output, output_len); /*lint !e718 !e746*/

	case PLATEX_PHB_GET_ENTRY:
		return MMIMRAPP_PHBGetEntry(input, input_len, output, output_len); /*lint !e718 !e746*/

	case PLATEX_PHB_COPY_ENTRY:
		 break;

	case PLATEX_PHB_SET_OWNER_ENTRY:
		 break;

	case PLATEX_PHB_GET_OWNER_ENTRY:
		 break;

	case PLATEX_PHB_DELETE_ENTRY_ALL:
		 break;

	case PLATEX_PHB_GET_COUNT:
		return MMIMRAPP_PHBGetCount(input, input_len, output, output_len); /*lint !e718 !e746*/

	case PLATEX_PHB_GET_STATUS:
		return MMIMRAPP_PHBGetStatus(); /*lint !e718 !e746*/
		
	case PLATEX_GET_SMS_INFO:
		return mr_sms_get_info(input, input_len, output, output_len);

	case PLATEX_GET_SMS_CONTENT:
		return mr_sms_get_content(input, input_len, output, output_len);

	case PLATEX_DELETE_SMS:
		return mr_sms_delete(input, input_len, output, output_len);

#ifdef __MR_SMS_SET_STATE_SUPPORT__
	case PLATEX_SET_SMS_STATE:
		return mr_sms_set_state(input, input_len, output, output_len);
#endif

	case PLATEX_SAVE_SMS:
		return mr_sms_save_sms(input, input_len, output, output_len);

	case PLATEX_REPLY_SMS_REQ:
		return mr_sms_replay_by_UI(input, input_len, output, output_len);

    case PLATEX_SEND_SMS_LOCALUI:
        return mr_sms_send_sms_from_ui(input, input_len, output, output_len);
	/* end:added by Tommy.yan 20120201 */

   case PLATEX_CHECK_WIFI_SUPPORT:
        return mr_setting_wifi_exist(input, input_len, output, output_len, cb);
	
#ifdef __MR_CFG_FEATURE_HANDSETID_EX__
    case PLATEX_GET_HEADSET_EX:
        return mr_setting_get_handsetID_ex(input, input_len, output, output_len, cb);
#endif
		
    case PLATEX_SET_WALLPAPER:
        return mr_setting_set_wallpaper(input, input_len, output, output_len, cb);
    case PLATEX_GET_ABLUM_PATH:
        return mr_setting_get_system_image_path(input, input_len, output, output_len, cb);

    case PLATEX_IMAGE_ENCODE:
        return mr_setting_image_encode(input, input_len, output, output_len, cb);

    case PLATEX_GET_TOUCHSCREEN_TYPE:
        return mr_setting_get_touchscreen_type(input, input_len, output, output_len, cb);

    case PLATEX_GET_CAMERA_SUPPORT:
        return mr_setting_get_camera_support(input, input_len, output, output_len, cb);
		
#ifdef __MR_CFG_LOCALAPP_MANAGER__
    case PLATEX_START_LOCALAPP:
        return mr_start_plat_application(input);
#endif

#ifdef __MR_CFG_MULTITP_SUPPORT__
    case PLATEX_GET_TP_POINTS:
        return mr_touch_panel_get_position(input, input_len, output, output_len, cb);
#endif
#ifdef __MR_CFG_SHORTCUT_SUPPORT__
    case PLATEX_SHORTCUT_SUPPORT:
        return mr_scut_support(input, input_len, output, output_len, cb);
    case PLATEX_SHORTCUT_INSTALL:
        return mr_scut_install_app(input, input_len, output, output_len, cb);
    case PLATEX_SHORTCUT_REMOVE:
        return mr_scut_remove_app(input, input_len, output, output_len, cb);
    case PLATEX_SHORTCUT_PROGESS:
        return mr_scut_update_progress(input, input_len, output, output_len, cb);
#endif

#ifdef __MMI_SKYQQ__
	case PLATEX_SKY_QQ:
		MMIMR_Trace(("PLATEX_SKY_QQ input_len = %d", input_len));
		return SDKCallSPR(input, input_len, output, output_len);
		break;
#endif

    case PLATEX_GET_HW_VER:
        return mr_setting_get_hw_version(input, input_len, output, output_len, cb);

#ifndef WIN32
    case PLATEX_DOVE_SET_SMS_STRATEGY:
        return mr_dove_set_sms_strategy(input, input_len, output, output_len, cb); 
#endif
	default:
		resValue = MR_IGNORE;
		break;
	}
	
	return resValue;
}

/*****************************************************************************/
// 	Description :获取短信模块状态
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_status(int32 param)
{
	if(!MMISMS_IsOrderOk())
	{
		return MR_SMS_NOT_READY;
	}
	else
	{
		return MR_SMS_READY;
	}
}

/*****************************************************************************/
// 	Description :获取短信信箱中短信的条数
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_num(int32 param)
{
	int32 num = 0;
	if(param == MR_SMS_INBOX)
	{
		num = MMIMRAPP_GetSMSCountByBoxType(MMISMS_BOX_MT);
		return num + MR_PLAT_VALUE_BASE;
	}
	else if(param == MR_SMS_OUTBOX)
	{
		num = MMIMRAPP_GetSMSCountByBoxType(MMISMS_BOX_SENDSUCC);
		return num + MR_PLAT_VALUE_BASE;
	}
	else if(param == MR_SMS_DRAFTS)
	{
		num = MMIMRAPP_GetSMSCountByBoxType(MMISMS_BOX_NOSEND);
		return num + MR_PLAT_VALUE_BASE;
	}
	return num;
}

/*****************************************************************************/
// 	Description :获取手机和SIM卡中短信的存储容量
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_capacity(int32 param)
{
	uint16 maxNum = 0;
	uint16 usedNum = 0;
	uint16 smsUsedNum = 0;
	MN_DUAL_SYS_E activeSim = MMIMRAPP_GetActiveSim();

	MMISMS_GetSMSUsedAndMax(activeSim, TRUE, &maxNum, &usedNum, &smsUsedNum);
	s_mr_sms_NVNum = maxNum;

	if(activeSim >= MN_DUAL_SYS_1 && activeSim < MN_DUAL_SYS_MAX)
	{
		MMISMS_GetSMSUsedAndMax(activeSim, FALSE, &maxNum, &usedNum, &smsUsedNum);
		s_mr_sms_SIMNum = maxNum;
	}

	MMIMRAPP_StartGetSMSCapacityTimer();
	return MR_WAITING;
}

/*****************************************************************************/
// 	Description :获取短信信息(时间戳、号码、状态)
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_info(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	int32 resValue = MR_FAILED; 
	T_DSM_GET_SMS_INFO_REQ *req = NULL;
	MMISMS_ORDER_ID_T order_id = NULL;
	MMI_TM_T tm ={0};

	if(input == NULL || input_len == 0)
	{
		return MR_FAILED;
	}

	req = (T_DSM_GET_SMS_INFO_REQ *)input;
	
	if(req->type == MR_SMS_INBOX)
	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_MT ,req->index);
	}
	else if(req->type == MR_SMS_OUTBOX)
	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_SENDSUCC,req->index);
	}
	
	if(order_id != NULL)
	{
		MMIMR_Trace(("mr_platEx GET_SMS_INFO order_id = %d",order_id));
    	MMIMR_Trace(("mr_platEx GET_SMS_INFO mo_mt_type = %d",order_id->flag.mo_mt_type));
    	MMIMR_Trace(("mr_platEx GET_SMS_INFO storage = %d",order_id->flag.storage));
    
		if(req->type == MR_SMS_INBOX)
		{
			if(MMISMS_MT_TO_BE_READ == order_id->flag.mo_mt_type)
			{
				s_mr_msgInfo.sms_status = MR_SMS_UNREAD;
			}
			else
			{
				s_mr_msgInfo.sms_status = MR_SMS_READED;
			}
		}
		else
		{
			if(MMISMS_MO_SEND_SUCC == order_id->flag.mo_mt_type)
			{
				s_mr_msgInfo.sms_status = MR_SMS_SENT;
			}
			else if(MMISMS_MO_DRAFT == order_id->flag.mo_mt_type)
			{
				s_mr_msgInfo.sms_status = MR_SMS_UNSENT ;
			}
		}

		if(MN_SMS_STORAGE_ME == order_id->flag.storage)
		{
			s_mr_msgInfo.sms_storage = MR_SMS_STORAGE_ME;
		}
		else
		{
			s_mr_msgInfo.sms_storage = MR_SMS_STORAGE_SIM;
		}
		
		MMIAPICOM_GenDispNumber( 
				MMIPB_GetNumberTypeFromUint8(order_id->number.bcd_number.npi_ton), 
				(uint8)MIN(order_id->number.bcd_number.number_len, MR_SMS_MAX_NUMBER), 
				order_id->number.bcd_number.number,  
				(uint8*)s_mr_msgInfo.num,
				(MR_SMS_MAX_NUMBER * 2 + 2)
    			);

		tm = MMIAPICOM_Second2Tm(order_id->time);
		s_mr_msgInfo.t_stamp[0] = tm.tm_year - 2000;
		s_mr_msgInfo.t_stamp[1] = tm.tm_mon;
		s_mr_msgInfo.t_stamp[2] = tm.tm_mday;
		s_mr_msgInfo.t_stamp[3] = tm.tm_hour;
		s_mr_msgInfo.t_stamp[4] = tm.tm_min;
		s_mr_msgInfo.t_stamp[5] = tm.tm_sec;
		*output = (uint8*)&s_mr_msgInfo;
		*output_len = sizeof(T_DSM_MSG_INFO);
		
		resValue = MR_SUCCESS;
	}
	else
	{
		resValue = MR_FAILED;
	}

	return resValue;
}

/*****************************************************************************/
// 	Description :获取短信内容
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_get_content(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	T_DSM_GET_SMS_INFO_REQ *req = NULL;
	MMISMS_ORDER_ID_T order_id = NULL;

	if(input == NULL || input_len == 0)
	{
		return MR_FAILED;
	}

	req = (T_DSM_GET_SMS_INFO_REQ *)input;
	MMIMRAPP_SetIsReadBySMS(TRUE);
	
	if(req->type == MR_SMS_INBOX)
	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_MT ,req->index);
		MMISMS_SetCurOperationOrderId(MMISMS_BOX_MT, req->index);
		MMISMS_SetCurType(MMISMS_BOX_MT);
	}
	else if(req->type == MR_SMS_OUTBOX)
	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_SENDSUCC,req->index);
		MMISMS_SetCurOperationOrderId(MMISMS_BOX_SENDSUCC, req->index);
		MMISMS_SetCurType(MMISMS_BOX_SENDSUCC);
	}
	MMIMR_Trace(("mr_platEx:MMIMRAPP_GetSMSContent order_id = %d",order_id));
	
	if(order_id == NULL)
	{
		return MR_FAILED;
	}

	//设置当前操作状态
	MMISMS_SetOperStatus(MMISMS_READ_SMS);
    MMISMS_ReadyReadSms();
    //开始读取
    MMISMS_AppReadSms(order_id); /*lint !e718 !e18*/
	MMIMRAPP_StartGetSMSContentTimer();
		
	return MR_WAITING;
}

/*****************************************************************************/
// 	Description :删除一条短信
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:针对长短信做相应的处理
/*****************************************************************************/
LOCAL int32 mr_sms_delete(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
    BOOLEAN result = FALSE;
    MMISMS_BOX_TYPE_E box_type = 0;
	T_DSM_GET_SMS_INFO_REQ *req = NULL;
    MMISMS_ORDER_ID_T order_id  = NULL;
    
	if(input == NULL || input_len == 0)
	{
		return MR_FAILED;
	}
	
	req = (T_DSM_GET_SMS_INFO_REQ *)input;
    s_mr_sms_order_id = NULL;

	if(req->type == MR_SMS_INBOX)
	{
    	box_type = MMISMS_BOX_MT;
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_MT, req->index);
	}
	else if(req->type == MR_SMS_OUTBOX)
	{
    	box_type = MMISMS_BOX_SENDSUCC;
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_SENDSUCC, req->index);
	}
	MMIMR_Trace(("mr_platEx:MMIMRAPP_DeleteSMS order_id=0x%08x", order_id));

	if(order_id == NULL)
	{
		return MR_FAILED;
	}

    if(order_id->next_long_sms_ptr != NULL)
    {
        s_mr_sms_order_id = order_id->next_long_sms_ptr;
    }

    MMIMRAPP_SetIsReadBySMS(TRUE);
    result = MMISMS_DeleteReqToMN(box_type, order_id, MMISMS_DELETE_MSG);
	
	//MMIMRAPP_StartDeleteSMSTimer();
	if(TRUE == result)
    {   
    	return MR_WAITING;
    }
    else
    {
        MMIMRAPP_SetIsReadBySMS(FALSE);
        return MR_FAILED;
    }
}

#ifdef __MR_SMS_SET_STATE_SUPPORT__
/*****************************************************************************/
// 	Description :修改收件箱/发件箱短信状态
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_set_state(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	MN_SMS_STATUS_E update_status = MN_SMS_FREE_SPACE;
	MN_SMS_STORAGE_E  storage = MN_SMS_NO_STORED;
	T_DSM_SET_SMS_STATE_REQ *req = NULL;
	MN_DUAL_SYS_E dual_sys = 0;
	MN_SMS_RECORD_ID_T record_id = 0;
	MMISMS_ORDER_ID_T order_id = NULL;
	MMISMS_STATE_T new_order_id = {0};
	MMISMS_MO_MT_TYPE_E sms_new_status = 0;

	if(input == NULL || input_len == 0)
	{
		return MR_FAILED;
	}

	req = (T_DSM_SET_SMS_STATE_REQ*)input;
	
	if(req->type == MR_SMS_INBOX)
	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_MT ,req->index);
	}
	else if(req->type == MR_SMS_OUTBOX)
	{
		order_id = MMIMRAPP_GetSMSLinkedArrayPtrIdInList(MMISMS_BOX_SENDSUCC,req->index);
	}
	MMIMR_Trace(("mr_platEx:mr_sms_set_state order_id = %d",order_id));

	if(order_id == NULL)
	{
		return MR_FAILED;
	}

	storage = (MN_SMS_STORAGE_E)order_id->flag.storage;
	dual_sys = (MN_DUAL_SYS_E)order_id->flag.dual_sys;
	record_id = (MN_SMS_RECORD_ID_T)order_id->record_id;

	switch(req->state)
	{
		case MR_SMS_UNREAD:
			update_status = MN_SMS_MT_TO_BE_READ;
			sms_new_status = MMISMS_MT_TO_BE_READ;
			break;

		case MR_SMS_READED:
			update_status = MN_SMS_MT_READED;
			sms_new_status = MMISMS_MT_HAVE_READ;
			break;

		case MR_SMS_UNSENT:
			update_status = MN_SMS_MO_TO_BE_SEND;
			sms_new_status = MMISMS_MO_DRAFT;
			break;

		case MR_SMS_SENT:
			update_status = MN_SMS_MO_SR_RECEIVED_AND_STORED;
			sms_new_status = MMISMS_MO_SEND_SUCC;
			break;

		default:
			break;
	}

	s_mr_sms_err_code = MNSMS_UpdateSmsStatusEx(dual_sys, storage, record_id, update_status);

	//重设修改后短信的状态

    MMISMS_GetSmsInfoFromOrder(order_id, &new_order_id);
	new_order_id.flag.mo_mt_type = sms_new_status;
	MMISMS_UpdateSmsInfoInOrder(order_id, &new_order_id);
	MAIN_SetMsgState(TRUE); /*lint !e718 !e18*/
	
	MMIMRAPP_StartSetSMSStateTimer();
	return MR_WAITING;
}
#endif

/*****************************************************************************/
// 	Description :保存一条(已读/未读)短信到收件箱中
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_save_sms(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	MR_SMS_SAVE_ENTRY_REQ_T *pSmsInput = NULL;
	MN_SMS_STATUS_E sms_status = MN_SMS_FREE_SPACE;
	MN_SMS_MT_SMS_T sms_ptr = {0};
	MMI_PARTY_NUMBER_T party_num = {0};
	APP_SMS_USER_DATA_T user_data_ptr = {0};
	SCI_DATE_T date = {0};
	SCI_TIME_T time = {0};
	BOOLEAN result = FALSE;
	
	if(input == NULL || input_len == 0)
	{
		return MR_FAILED;
	}

	pSmsInput = (MR_SMS_SAVE_ENTRY_REQ_T *)input;

	SCI_MEMSET(&s_mr_save_order_id, 0x00, sizeof(MMISMS_STATE_T));
	SCI_MEMSET(&sms_ptr, 0x00, sizeof(MN_SMS_MT_SMS_T));

#if defined(MMI_MULTI_SIM_SYS_DUAL) || defined(MMI_MULTI_SIM_SYS_TRI) || defined(MMI_MULTI_SIM_SYS_QUAD)
	if(pSmsInput->flag & MR_SMSSAVE_DISP_SIM_OPT)
	{
		s_mr_save_order_id.flag.dual_sys = MN_DUAL_SYS_1;
	}
	else if(pSmsInput->flag & MR_SMSSAVE_TO_SIM1)
	{
		s_mr_save_order_id.flag.dual_sys = MN_DUAL_SYS_1;
	}
	else if(pSmsInput->flag & MR_SMSSAVE_TO_SIM2)
	{
		s_mr_save_order_id.flag.dual_sys = MN_DUAL_SYS_2;
	}
#else
	if(pSmsInput->flag & MR_SMSSAVE_DISP_SIM_OPT
		|| pSmsInput->flag & MR_SMSSAVE_TO_SIM1)
	{
		s_mr_save_order_id.flag.dual_sys = MN_DUAL_SYS_1;
	}
#endif

	if(strlen(pSmsInput->number) > 0)
	{
		MMIAPICOM_GenPartyNumber((uint8*)pSmsInput->number, (int16)strlen(pSmsInput->number), &party_num);
		s_mr_save_order_id.number.bcd_number.number_len = (uint8)party_num.num_len;
		s_mr_save_order_id.number.bcd_number.npi_ton = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, 0);
		MMI_MEMCPY(s_mr_save_order_id.number.bcd_number.number, MMIPB_BCD_NUMBER_MAX_LEN, party_num.bcd_num, party_num.num_len, party_num.num_len);
	}

	if(pSmsInput->status == 0)
	{
		s_mr_save_order_id.flag.mo_mt_type = MMISMS_MT_TO_BE_READ;
		sms_status = MN_SMS_MT_TO_BE_READ;
	}
	else
	{
		s_mr_save_order_id.flag.mo_mt_type = MMISMS_MT_HAVE_READ;
		sms_status = MN_SMS_MT_READED;
	}

	s_mr_save_order_id.flag.msg_type   = MMISMS_TYPE_SMS;
	s_mr_save_order_id.flag.storage    = MN_SMS_STORAGE_ME;

	//存储的短信时间
	TM_GetSysDate(&date);
	TM_GetSysTime(&time);
	sms_ptr.time_stamp_t.year = date.year-MMISMS_MT_OFFSERT_YEAR; //modify by zack@20120618
	sms_ptr.time_stamp_t.month= date.mon;
	sms_ptr.time_stamp_t.day  = date.mday;
	sms_ptr.time_stamp_t.hour = time.hour;
	sms_ptr.time_stamp_t.minute = time.min;
	sms_ptr.time_stamp_t.second = time.sec;
  
    s_mr_save_order_id.time= MMIAPICOM_Tm2Second(
        					time.sec,
        					time.min,
        					time.hour,
        					date.mday,
        					date.mon,
        					date.year
        					);

	//存储的短信内容
	sms_ptr.dcs.alphabet_type = MN_SMS_UCS2_ALPHABET;
	sms_ptr.user_data_t.length = MIN(MMIAPICOM_Wstrlen(pSmsInput->content)*2, MN_SMS_USER_DATA_LENGTH);
	sms_ptr.user_data_t.user_data_arr[0] = sms_ptr.user_data_t.length;
	SCI_MEMCPY(sms_ptr.user_data_t.user_data_arr+1, pSmsInput->content, sms_ptr.user_data_t.length);
	sms_ptr.user_data_t.length++;

	MMIMR_Trace(("mr_sms_save_sms length = %d", sms_ptr.user_data_t.length));
	MMIAPICOM_WstrTraceOut(sms_ptr.user_data_t.user_data_arr, sms_ptr.user_data_t.length/2);

	//存储的短信号码
	sms_ptr.origin_addr_t.num_len 	  = party_num.num_len;
	sms_ptr.origin_addr_t.number_type = party_num.number_type;
	sms_ptr.origin_addr_t.number_plan = MN_NUM_PLAN_ISDN_TELE;
	SCI_MEMCPY(sms_ptr.origin_addr_t.party_num, party_num.bcd_num, party_num.num_len);

	//unicode 大小转换
	mr_str_convert_endian_with_size((char *)pSmsInput->content, MR_SMS_MAX_CONTENT);

	//短信主题显示内容
	s_mr_save_order_id.flag.is_ucs2 = TRUE;
	s_mr_save_order_id.display_content.content_len = MIN(MMIAPICOM_Wstrlen(pSmsInput->content)*2, MMIMESSAGE_DIAPLAY_LEN);
	SCI_MEMCPY(s_mr_save_order_id.display_content.content, pSmsInput->content, s_mr_save_order_id.display_content.content_len);
	
	//设置当前操作状态
	MMISMS_SetOperStatus(MMISMS_MRAPP_SAVE_SMS);
	
	//将短信内容写入
	MNSMS_WriteMtSmsEx(s_mr_save_order_id.flag.dual_sys, MN_SMS_STORAGE_ME, sms_status, &sms_ptr);

	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :调用本地UI回复短信
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_replay_by_UI(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	uint8 num_len = 0;
	uint8 number[MMISMS_NUMBER_MAX_LEN] = {0};
	T_MR_SMS_REPLY_ENTRY_REQ *req = NULL;
	
	if(input == NULL || input_len == 0)
	{
	    return MR_FAILED;
	}

	req = (T_MR_SMS_REPLY_ENTRY_REQ *)input;
	num_len = MIN(strlen(req->number), MMISMS_NUMBER_MAX_LEN);
	SCI_MEMSET(number, 0x00, sizeof(number));
	SCI_MEMCPY(number, req->number, num_len);
	MMISMS_WriteNewMessage(req->simid, NULL, FALSE, number, num_len);
	
	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :调用本地UI发送短信
//	Global resource dependence : none
//  Author: Zack.Zhang
//	Note:
/*****************************************************************************/
LOCAL int32 mr_sms_send_sms_from_ui(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
    wchar *text = NULL;
	uint8 num_len = 0;
	uint8 number[MMISMS_NUMBER_MAX_LEN] = {0};
    MMI_STRING_T content = {0};
	MR_SMS_SEND_REQ_T* req = (MR_SMS_SEND_REQ_T *)input;
    
	if(input == NULL || input_len != sizeof(MR_SMS_SEND_REQ_T))
	{
	    return MR_FAILED;
	}

	if (req->content != NULL)
	{
		content.wstr_len = mr_str_wstrlen(req->content);
	}

	if (content.wstr_len > 0)
	{
		text = SCI_ALLOCA(content.wstr_len*2 + 2);
		if (text == NULL)
		{
			return MR_FAILED;
		}

		memset(text, 0x00, content.wstr_len*2+2);
		memcpy(text, req->content, content.wstr_len*2);
		mr_str_convert_endian_with_size((char *)text, content.wstr_len*2);
        content.wstr_ptr = text;

        if(req->addr != NULL)
        {
        	num_len = MIN(strlen(req->addr), MMISMS_NUMBER_MAX_LEN);
        	SCI_MEMSET(number, 0x00, sizeof(number));
        	SCI_MEMCPY(number, req->addr, num_len);
            MMISMS_WriteNewMessage(MMIMRAPP_GetActiveSim(), &content, FALSE, number, num_len);
        }
        else
        {
        	MMISMS_WriteNewMessage(MMIMRAPP_GetActiveSim(), &content, FALSE, NULL, 0);
        }

        if(text != NULL)
        {
            SCI_FREE(text);
            text = NULL;
        }
	}
    else
    {
        if(req->addr != NULL)
        {
        	num_len = MIN(strlen(req->addr), MMISMS_NUMBER_MAX_LEN);
        	SCI_MEMSET(number, 0x00, sizeof(number));
        	SCI_MEMCPY(number, req->addr, num_len);
            MMISMS_WriteNewMessage(MMIMRAPP_GetActiveSim(), NULL, FALSE, number, num_len);
        }
        else
        {
            MMISMS_WriteNewMessage(MMIMRAPP_GetActiveSim(), NULL, FALSE, NULL, 0);
        }
    }

	return MR_SUCCESS;
}

/*****************************************************************************/
// 	Description :
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
/*LOCAL void MMIMRAPP_AudioPlayCb(MMIAUDIO_RESULT result, DPARAM param)
{
	MMIMR_Trace(("mrapp. MMIMRAPP_AudioPlayCb()"));
	s_mr_audio_mng.curStatus = LOADED;
	s_mr_audio_mng.curSetPlayPos = 0;
	s_mr_audio_mng.audioHdle = INVALID_HANDLE;
}*/

/*****************************************************************************/
// 	Description :图片类型转换
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_type_translate(GUIANIM_TYPE_E type)
{
	int32 image_type;
	if(type == GUIANIM_TYPE_IMAGE_BMP)
	{
		image_type = MR_IMAGE_IMG_TYPE_BMP;
	}
	else if(type == GUIANIM_TYPE_IMAGE_JPG)
	{
		image_type = MR_IMAGE_IMG_TYPE_JPG;
	}
	else if(type == GUIANIM_TYPE_IMAGE_PNG)
	{
		image_type = MR_IMAGE_IMG_TYPE_PNG;
	}
	else if(type == GUIANIM_TYPE_IMAGE_GIF)
	{
		image_type = MR_IMAGE_IMG_TYPE_GIF;
	}
	else
	{
		image_type = MR_IMAGE_IMG_TYPE_NONE;
	}

	return image_type;
}

/*****************************************************************************/
// 	Description :image命令接口
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_imageCmd(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 resValue = MR_FAILED;
	MMIMR_Trace(("MMIMRAPP_ImageCmd() :%d", code));
	
	switch(code)
	{
	case MR_IMG_GET_INFO://取得一张图片的信息
		return mr_image_get_info(input, input_len, output, output_len, cb);
               
	case MR_IMG_DEC_JPG://解码一张图片，将解完的数据放到dest所指的buf中，并通知vm解码结果
		return mr_image_decode(input, input_len, output, output_len, cb);
        
	case MR_IMG_GET_STAT://取得图像解码状态(异步解码)
		break;

	case MR_IMG_DEC_GIF://GIF DECODE
		break;
        
	case MR_IMG_FREE_GIF://FREE GIF		
		break;

	case MR_IMG_DRAW_JPG://DRAW PIC
		return mr_image_draw(input, input_len, output, output_len, cb);

	default:
		resValue = MR_IGNORE;//命令不支持
		break;
	}

	return resValue;
}

/*****************************************************************************/
// 	Description :获取图片信息
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_get_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	MRAPP_IMAGE_ORIGIN_T* origin = (MRAPP_IMAGE_ORIGIN_T*)input;
	int32 resValue = MR_FAILED;
	MRAPP_IMG_TYPE imgType;
	uint8* data = NULL;
	int32 dataLen;
			
	MMIMR_Trace(("mr_image_get_info origin:%x", origin));
	if(origin != NULL)
	{
		MMIMR_Trace(("mr_image_get_info src_type: %d, src: %d, len: %d", origin->src_type, origin->src, origin->len));
		if(origin->src_type == SRC_STREAM)
		{
			data = (uint8*)origin->src;
			dataLen = origin->len;
				
			if(data != NULL && dataLen > 0)
			{
				MRAPP_IMAGE_INFO_T *rspInfo = (MRAPP_IMAGE_INFO_T*)tempTransBuf;
				IMG_DEC_SRC_T    dec_src = {0};
    			IMG_DEC_INFO_T  dec_info = {0};
				
				dec_src.src_ptr 	  = (unsigned char*)data;
				dec_src.src_file_size = (uint32)dataLen;
				
				if(IMG_DEC_RET_SUCCESS == IMG_DEC_GetInfo(&dec_src, &dec_info))
				{
					if(IMG_DEC_TYPE_JPEG == dec_info.img_type)
					{
						rspInfo->width    = dec_info.img_detail_info.jpeg_info.img_width;
						rspInfo->height   = dec_info.img_detail_info.jpeg_info.img_heigth;
						rspInfo->img_type = MR_IMAGE_IMG_TYPE_JPG;
					}
					else if(IMG_DEC_TYPE_GIF == dec_info.img_type)
					{
						rspInfo->width    = dec_info.img_detail_info.gif_info.img_width;
						rspInfo->height   = dec_info.img_detail_info.gif_info.img_height;
						rspInfo->img_type = MR_IMAGE_IMG_TYPE_GIF;
					}
					else if(IMG_DEC_TYPE_PNG == dec_info.img_type)
					{
						rspInfo->width    = dec_info.img_detail_info.png_info.img_width;
						rspInfo->height   = dec_info.img_detail_info.png_info.img_height;
						rspInfo->img_type = MR_IMAGE_IMG_TYPE_PNG;
					}
					else
					{
						resValue = MR_FAILED;
					}

					MMIMR_Trace(("mr_image_get_info h : %d, w: %d, type: %d", rspInfo->height, rspInfo->width, rspInfo->img_type));
							
					*output = (uint8*)rspInfo;
					*output_len = sizeof(MRAPP_IMAGE_INFO_T);
					resValue = MR_SUCCESS;
				}
				else
				{
					resValue = MR_FAILED;//不支持
				}
			}
		}

		else if(origin->src_type == SRC_NAME)
		{
			MRAPP_IMAGE_INFO_T *rspInfo = (MRAPP_IMAGE_INFO_T*)tempTransBuf;
			char* fileName = NULL;
			uint32 fileSize = 0;
			int32 fullNameLen = 0;
			GUIANIM_IMG_INFO_T  img_info = {0};
			fileName = (char*)origin->src;

			if(fileName != NULL)
			{
				memset(full_path, 0, sizeof(full_path));
				fileSize = mr_getLen(fileName);
				fullNameLen = mr_fs_get_filename((char*)full_path, fileName);
						
				if(fullNameLen > 0)
				{
					GUIANIM_GetInfo(NULL, full_path, NULL, &img_info);
					rspInfo->width    = img_info.img_width;
					rspInfo->height   = img_info.img_height;
					rspInfo->img_type = mr_image_type_translate(img_info.type);
					
					MMIMR_Trace(("mr_image_get_info h : %d, w: %d, type: %d", rspInfo->height, rspInfo->width, rspInfo->img_type));
							
					*output = (uint8*)rspInfo;
					*output_len = sizeof(MRAPP_IMAGE_INFO_T);
					resValue = MR_SUCCESS;
				}
			}
			else
			{
				resValue = MR_FAILED;
			}
		}
	}

	return resValue;
}

/*****************************************************************************/
// 	Description :图片解码
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_decode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	MRAPP_IMAGE_DECODE_T *image = (MRAPP_IMAGE_DECODE_T*)input;
	int32 result = MR_FAILED;
	GUIIMG_SRC_T		img_src = {0};     
	GUIIMG_DISPLAY_T	img_display = {0};  
	GUIIMG_DEC_OUT_T	img_output = {0};  
	
	MMIMR_Trace(("mrapp. jpg decode before!type: %d",image->src_type));
	MMIMR_Trace(("mrapp.decode in!w: %d, h: %d",image->width, image->height));
	
	if(image == NULL || image->src == NULL || image->dest == NULL) /*lint !e831*/
	{
		return MR_FAILED;
	}
	
	if(image != NULL) /*lint !e774 !e831*/
	{
		if(image->src_type == SRC_STREAM)
		{
			uint8* data = NULL;
			int32 dataLen = 0;
			data = (uint8*)image->src;
			dataLen = image->len;
			
			if(data != NULL && dataLen > 0)
			{
				img_src.is_file = FALSE;
				img_src.src_buf_ptr = data;
				img_src.src_data_size = dataLen;
			}
		}
		
		else if(image->src_type == SRC_NAME)
		{
			char* fileName = NULL;
			int32 fullNameLen = 0;
			fileName = (char*)image->src;

			if(fileName != NULL)
			{
				memset(full_path, 0, sizeof(full_path));
				fullNameLen = mr_fs_get_filename((char*)full_path, fileName);
				
				img_src.is_file = TRUE;
				img_src.full_path_ptr = (wchar*)full_path;
			}
		}
	}
		
	img_display.dest_width = image->width;
	img_display.dest_height = image->height;
	img_output.decode_data_ptr = (uint8*)image->dest;
	img_output.decode_data_size = image->width*image->height*2;

	if(GUIIMG_Decode(&img_src, &img_display, &img_output))
	{
		MMIMR_Trace(("mrapp.decode by name ok!w: %d, h: %d,w: %d, h: %d", img_output.actual_width, img_output.actual_height, 
																		image->width, image->height));
		image->width = img_output.actual_width;
		image->height = img_output.actual_height;
		result = MR_SUCCESS;
	}
	else
	{
		result = MR_FAILED;
	}

	MMIMR_Trace(("mrapp.decode by name ok!result: %d", result));
	return result;
}

/*****************************************************************************/
// 	Description :图片绘制
//	Global resource dependence : none
//  Author: Tommy.yan
//	Note:
/*****************************************************************************/
LOCAL int32 mr_image_draw(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_DRAW_DIRECT_REQ *image = (T_DRAW_DIRECT_REQ*)input;
	int32 result = MR_FAILED;
	int32 decodeBufSize = 0;
	GUI_LCD_DEV_INFO dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	GUI_RECT_T  rect = {0};
	GUI_POINT_T point = {0};
	GUIIMG_BITMAP_T bitmap_ptr = {0};
	GUIIMG_SRC_T		img_src = {0};     
	GUIIMG_DISPLAY_T	img_display = {0};  
	GUIIMG_DEC_OUT_T	img_output = {0};

	if(image == NULL || image->src == NULL)
	{
		return MR_FAILED;
	}

	decodeBufSize = image->h*image->w*4;
	bitmap_ptr.bit_ptr = (void*)SCI_ALLOCA(decodeBufSize);	

	if(bitmap_ptr.bit_ptr == NULL)
	{
		return MR_FAILED;
	}

	if(image->src_type == SRC_STREAM)
	{
		uint8* data = NULL;
		int32 dataLen = 0;
		data = (uint8*)image->src;
		dataLen = image->src_len;
			
		if(data != NULL && dataLen > 0)
		{
			img_src.is_file = FALSE;
			img_src.src_buf_ptr = data;
			img_src.src_data_size = dataLen;
		}
	}
	else if(image->src_type == SRC_NAME)
	{
		char* fileName = NULL;
		int32 fullNameLen = 0;
		fileName = (char*)image->src;
			
		if(fileName != NULL)
		{
			memset((void*)full_path, 0, sizeof(full_path));
			fullNameLen = mr_fs_get_filename((char*)full_path, fileName);
				
			img_src.is_file = TRUE;
			img_src.full_path_ptr = (wchar*)full_path;
		}
	}

	memset(bitmap_ptr.bit_ptr, 0, decodeBufSize);
	
	img_display.dest_width = image->w;
	img_display.dest_height = image->h;
	img_output.decode_data_ptr = bitmap_ptr.bit_ptr;
	img_output.decode_data_size = decodeBufSize;

	point.x = image->ox;
	point.y = image->oy;
    rect.left = image->ox;
    rect.top  = image->oy;
    rect.right = image->ox + image->w - 1;
    rect.bottom= image->oy + image->h - 1;

	MMIMR_Trace(("mrapp.decode in!w: %d, h: %d", image->w, image->h));
	if(GUIIMG_Decode(&img_src, &img_display, &img_output))
	{
		MMIMR_Trace(("mrapp.decode by name ok!w: %d, h: %d,w: %d, h: %d", img_output.actual_width, img_output.actual_height, 
																		image->w, image->h));
		bitmap_ptr.img_height = img_output.actual_height;
		bitmap_ptr.img_width = img_output.actual_width;
		if(GUIIMG_DisplayBmp(FALSE, &rect, &point, (void*)&bitmap_ptr, &dev_info))
		{
			MMIMR_Trace(("mrapp.Display ok!w: %d, h: %d", bitmap_ptr.img_width, bitmap_ptr.img_height));
			result = MR_SUCCESS;
		}
		else
		{
			result = MR_FAILED;
		}
	}
	else
	{
		MMIMR_Trace(("mrapp.decode failed!w: %d, h: %d, ptr: 0x%x, size: %d", img_output.actual_width, img_output.actual_height, 
			          img_output.decode_data_ptr, img_output.decode_data_size));
		result = MR_FAILED;
	}
	SCI_FREE(bitmap_ptr.bit_ptr);

	return result;
}

/*****************************************************************************/
// 	Description :释放
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_gifFree(MRAPP_GIF_HEADER ** header)
{
	int32 resValue = MR_FAILED;
	MRAPP_GIF_HEADER *hdr;
	if(header != NULL)
	{
		hdr = *header;
		if(hdr != NULL)
		{
			MRAPP_GIF_FRAME_INFO_T *framePtr, *temp;
			framePtr = hdr->first;
			while(framePtr != NULL)
			{
				temp = framePtr;
				framePtr = framePtr->next;
				MMIMR_Trace(("temp->pdata: 0x%x", temp->pdata));
				if(temp->pdata != NULL)
				{
					SCI_FREE(temp->pdata);
				}
				SCI_FREE(temp);
			}
			MMIMR_Trace(("hdr: 0x%x", hdr));
			SCI_FREE(hdr);
			*header = NULL;
			resValue = MR_SUCCESS;
		}
	}
	
	return resValue;
}

/*****************************************************************************/
// 	Description :U获取jpeg宽高
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL uint32 MMIMRAPP_get_jpeg_width_height(uint8 * pbuf, uint32 size)
{	
	uint32 i = 0, width = 0, height = 0,len = 0;;
	MMIMR_Trace(("MMIMRAPP_get_jpeg_width_height() :%d", size));
	
	for(i = 2; i < size;)
	{
		if ((pbuf[i]==0xff) &&(pbuf[i+1]==0xC0))
		{
			width = (((uint16)pbuf[i+7]) << 8)|(((uint16)pbuf[i+8]));
			height = (((uint16)pbuf[i+5]) << 8)|(((uint16)pbuf[i+6]));
			MMIMR_Trace(("jpeg_width: %d", width));
			MMIMR_Trace(("jpeg_height: %d", height));
			return (height << 16) |width;
		}
		else
		{
			i+=2;
			len = ((pbuf[i])<<8)|(pbuf[i+1]);
			i += len;
		}
	}
	return 0;
}

/*****************************************************************************/
// 	Description :获取图片类型
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL MRAPP_IMG_TYPE MIMRAPP_get_image_type_from_buf(unsigned char *pbuf, int32 len)
{
	if(pbuf == NULL)
		return MRP_IMAGE_TYPE_INVALID;
	   
	if((len>=3)&&( pbuf[0] == 0xff && pbuf[1] == 0xd8 && pbuf[2] == 0xff))
		return MRP_IMAGE_TYPE_JPG_FILE;
	else if((len>=3)&&( pbuf[0] == 'G' && pbuf[1] == 'I' && pbuf[2] == 'F'))
		return MRP_IMAGE_TYPE_GIF_FILE;
	else if((len>=8)&&(memcmp(pbuf,"\x89PNG\x0d\x0a\x1a\x0a",8) == 0))
		return MRP_IMAGE_TYPE_PNG_FILE;
	else if((len>=2)&&( pbuf[0] == 'B' && pbuf[1] == 'M'))
		return MRP_IMAGE_TYPE_BMP_FILE;
	else if((len>=2)&&( pbuf[0] == 0 && pbuf[1] == 0))
		return MRP_IMAGE_TYPE_WBMP_FILE;
	else
		return MRP_IMAGE_TYPE_INVALID;
}

/*****************************************************************************/
// 	Description :创建一个LIST
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_listCreate(const char * title, int16 num, uint8 type)
{
	int32 resValue = MR_FAILED;
	MMIMRAPP_ADD_PARAMETERS_T *addData = NULL;
	MRAPP_MENU_DATA *menuPtr;	
	char *temp = NULL;
	int32 lenth  = 0;
	MMIMR_Trace(("mrapp. mr_menuCreate() num: %d", num));

	if(title != NULL)
	{
		lenth = mr_str_wstrlen(title) + 2;
		temp = (char*)SCI_ALLOCA(lenth);
		SCI_MEMCPY(temp, title, lenth);
		mr_str_convert_endian((char*)temp);
	}
	
	addData = (MMIMRAPP_ADD_PARAMETERS_T *)SCI_ALLOC(sizeof(MMIMRAPP_ADD_PARAMETERS_T));
	if(addData != NULL)
	{		
		addData->dynWinType = MRAPP_WINTYPE_MENU;
		menuPtr = &addData->u.menu;
		menuPtr->type = type;
		menuPtr->title.wstr_len = (uint16)MMIMRAPP_getUcstrLen((uint8 *)temp)/2;
		menuPtr->maxItemNum = MIN(num, 40);
		menuPtr->curAddItemIdx = -1;
		menuPtr->curFocusItemIdx = 0;
		menuPtr->preFocusItemIdx = 0;
		menuPtr->listElem = NULL;
		MMIMR_Trace(("menu.title len: %d", menuPtr->title.wstr_len));
		if(menuPtr->title.wstr_len != 0)
		{
			menuPtr->title.wstr_ptr = (wchar*)SCI_ALLOC(menuPtr->title.wstr_len*2+2);
			if(menuPtr->title.wstr_ptr != NULL)
			{
				SCI_MEMCPY(menuPtr->title.wstr_ptr, temp, menuPtr->title.wstr_len*2+2);
			}
			else
			{
				menuPtr->title.wstr_len = 0;
			}
		}
		else
		{
			menuPtr->title.wstr_ptr = NULL;
		}
		
		resValue = MMIMRAPP_createADynWin(addData, NULL);
		if(resValue < 0)
		{
			if(resValue == -2)
			{
				if(menuPtr->title.wstr_ptr != NULL)
				{
					SCI_FREE(menuPtr->title.wstr_ptr);
				}
				SCI_FREE(addData);
			}
			resValue = MR_FAILED;
		}	
	}

	if(temp != NULL)
	{
		SCI_FREE(temp);
	}
	MMIMR_Trace(("mrapp. mr_menuCreate() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :设置item
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_menuSetItem(int32 menu, const char *text, int32 index, int32 isSelected)
{
	int32 resValue = MR_FAILED;
	GUIMENU_DYNA_ITEM_T node_item = {0};
	MMI_STRING_T item_text;
	MRAPP_MENU_DATA *menuPtr;
	MRAPP_LIST_ITEM_T *addElem, **continueSite = NULL;
	MMIMRAPP_ADD_PARAMETERS_T* addData;
	uint16 len;
	int32 replaceIndex = -1;	
	char *temp = NULL;
	int32 lenth = 0;
	MMIMR_Trace(("mrapp. mr_menuSetItem() menu: %d, index: %d", menu, index));
	
	addData = (MMIMRAPP_ADD_PARAMETERS_T*)MMK_GetWinAddDataPtr(menu);

	if(text != NULL)
	{
		lenth = mr_str_wstrlen(text) + 2;
		temp = (char*)SCI_ALLOCA(lenth);
		SCI_MEMCPY(temp, text, lenth);
		mr_str_convert_endian((char*)temp);
	}

	if(addData != NULL)
	{
		len = MMIMRAPP_getUcstrLen((uint8 *)temp);
		len = (len <= (GUILIST_STRING_MAX_NUM_AD - 2)) ? len : (GUILIST_STRING_MAX_NUM_AD - 2);
		if(len != 0)
		{
			menuPtr = &addData->u.menu;			
			//txt info
			item_text.wstr_len = len/2;
			item_text.wstr_ptr = (wchar*)temp;
			
			//local info
			continueSite = &menuPtr->listElem;
			while(*continueSite != NULL)
			{
				if((*continueSite)->key == index)
				{
					replaceIndex = (*continueSite)->index;
					continueSite = &(*continueSite)->nextItem;
					break;
				}
				continueSite = &(*continueSite)->nextItem;
			}
			
			node_item.item_text_ptr = &item_text;
			if(menuPtr->type == MR_LIST_NORMAL_ICON)
            {
				if(replaceIndex == -1)
                {
					node_item.select_icon_id = MMIMRAPP_getItemIcon(MRAPP_ICON_OFF, menuPtr->curAddItemIdx + 1);
				}
                else
                {
					node_item.select_icon_id = MMIMRAPP_getItemIcon(MRAPP_ICON_OFF, replaceIndex);
				}
			}
            else if(menuPtr->type == MR_LIST_SINGLE_SEL)
            {
				if(isSelected == 1)
                {
					node_item.select_icon_id = IMAGE_RADIO_SELECTED_ICON;
				}
                else
                {
					node_item.select_icon_id = IMAGE_RADIO_UNSELECTED_ICON;
				}
			}
            else if(menuPtr->type == MR_LIST_MUTI_SEL)
            {
				if(isSelected == 1)
                {
					node_item.select_icon_id = IMAGE_CHECK_SELECTED_ICON;
				}
                else
                {
					node_item.select_icon_id = IMAGE_CHECK_UNSELECTED_ICON;
				}
			}
			
			if(replaceIndex == -1)
            {//add a new item
				MMIMR_Trace(("mrapp. curAddItemIdx: %d, index: %d", menuPtr->curAddItemIdx, index));
				if(menuPtr->curAddItemIdx+1 < menuPtr->maxItemNum)
                {
					GUIMENU_InsertNode(menuPtr->curAddItemIdx+1, menuPtr->curAddItemIdx+2, 0, &node_item, addData->ctrlId);
					menuPtr->curAddItemIdx ++;
					addElem = (MRAPP_LIST_ITEM_T*)SCI_ALLOC(sizeof(MRAPP_LIST_ITEM_T));
					SCI_MEMSET(addElem, 0, sizeof(MRAPP_LIST_ITEM_T));
					addElem->index = menuPtr->curAddItemIdx;
					addElem->key = index;
					addElem->nextItem = NULL;
					*continueSite = addElem;
					if (isSelected == 1)
					{
						GUIMENU_SetFirstAndSelectedItem(0, menuPtr->curAddItemIdx, addData->ctrlId);
					}
				}
			}
            else
            {	//replace a item
				MMIMR_Trace(("mrapp. replaceIndex: %d, index: %d", replaceIndex, index));
				GUIMENU_ReplaceNodeEx(replaceIndex, replaceIndex+1, 0, &node_item, addData->ctrlId);
			}
			
			resValue = MR_SUCCESS;
		}
	}

	if(temp != NULL)
	{
		SCI_FREE(temp);
	}
	MMIMR_Trace(("mrapp. mr_menuSetItem() resValue: %d", resValue));
	return resValue;
}

/*****************************************************************************/
// 	Description :mr_playSound 回调
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MRRingCallBack(MMISRVAUD_REPORT_RESULT_E result, DPARAM param)
{
	MMIMR_TraceM(("mrapp. MRRingCallBack() result: %d", result));
	MMIMRAPP_StopPlaySoundProtect();//2008-1-2 add
	MMIMRAPP_resumeMp3();
    MMK_PostMsg(VIRTUAL_WIN_ID,MSG_MP3_CONTINUE,PNULL,0);
}

/*****************************************************************************/
// 	Description :保存mrp
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_savePack(uint8 *p,uint32 l)
{
	int32 ret = MR_IGNORE;
	MMIFILE_HANDLE f;
	const uint8 *devName;
	uint16 devNameLen;		
	uint32 space_low = 0, space_high = 0;
	uint16 full_path_len = 0, ucs2_name_len = 0, dirLen = 0;
	uint16 dir[MMIFILE_DIR_NAME_MAX_LEN] = {0};
	MMIMR_Trace(("MMIMRAPP_savePack() len: %d", l));
	
	if(p != NULL && l != 0)
	{
		if((*((char*)p + 69) !=0x7e) || (*((char*)p + 70) != 0x7e) || (*((char*)p + 71) != 0x7e))
		{
			ret = mr_save_mrp((void*)p, l);
		}
#ifdef MRAPP_SIMUGAME_OPEN
		else if((*(char*)p == 'M') && (*((char*)p + 1) == 'R') && (*((char*)p + 2) == 'P') && (*((char*)p + 3) == 'G'))//.nes pack
		{
			MMIMR_Trace(("##NES ZPK !!!!"));
			ret = MR_FAILED;
			switch(g_mr_dualsys_mng.devSel)
			{
			default:
			case 'b':
			case 'B':
				devName = (const uint8 *)MMIFILE_DEVICE_UDISK;
				devNameLen = MMIFILE_DEVICE_UDISK_LEN;
				break;
			case 'c':
			case 'C':
#ifdef DUAL_TCARD_SUPPORT
                if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
                {
#endif
        			devName = (const uint8 *)MMIFILE_DEVICE_SDCARD;
        			devNameLen = MMIFILE_DEVICE_SDCARD_LEN;
#ifdef DUAL_TCARD_SUPPORT
                }
                else if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
                {
             	    devName = (const uint8 *)MMIFILE_DEVICE_SDCARD2;
    				devNameLen = MMIFILE_DEVICE_SDCARD2_LEN;
                }
#endif
				break;
#ifdef DOUBLE_TFCARD				
			case 'd':
			case 'D':
				devName = (const uint8 *)MMIFILE_DEVICE_SDCARD_2;
				devNameLen = MMIFILE_DEVICE_SDCARD_LEN_2;
				break;
#endif
			}
			
			if(MMIAPIFMM_GetDeviceStatus((uint8*)devName, devNameLen))
			{
				MMIFILE_GetDeviceFreeSpace((uint8 *)devName, devNameLen, &space_high, &space_low);
			}
			if(space_low == 0 || space_low == 0xFFFFFFFF)
			{
				return 2;
			}
			
			SCI_MEMSET(tempTransBuf, 0, sizeof(tempTransBuf));
			SCI_MEMCPY(tempTransBuf , (char*)p + 28, 24);
			SCI_MEMSET(ucs2_name_arr, 0, sizeof(ucs2_name_arr));
			SCI_MEMSET(full_path, 0, sizeof(full_path));
			ucs2_name_len = MMIMRAPP_GB2UCS(ucs2_name_arr, (const uint8 *)tempTransBuf, strlen((char *)(tempTransBuf)));
			dirLen = MMIMRAPP_GB2UCS(dir, (const uint8 *)MMIMRAPP_NES_DIR_NAME, strlen(MMIMRAPP_NES_DIR_NAME));
#ifdef PORT_BASE_NILE
			devNameLen = devNameLen*2;
#endif
			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)dir, dirLen,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
				MMIFILE_DeleteFile((const uint16*)full_path, NULL);
				f = MMIFILE_CreateFile((const uint16 *)full_path, SFS_MODE_OPEN_ALWAYS|SFS_MODE_WRITE, NULL, NULL);
				if(f != FFS_INVALID_HANDLE)
				{
					if(mr_write(f, (void *)p, l) == (int32)l)
					{
						ret = MR_SUCCESS;
					}
					mr_close(f);
				}
			}
		}
#endif	
	}
	
	return ret;
}

#ifdef MRAPP_USE_UDISK_FIRST
/*****************************************************************************/
// 	Description :设置默认文件设备与目录
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
extern char  gappparam[64];
/*****************************************************************************/
// 	Description : 	取得文件类型信息。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 mr_info_udisk(const char* filename)
{
	SFS_HANDLE		sfs_handle = 0;
	SFS_FIND_DATA_T find_data = {0};
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	const uint8 *devName;
	uint16 devNameLen;
	
	if(filename != NULL)
	{
		MMIMR_Trace(("mrapp. mr_info() file: %s", filename));

		//devName = MMIMRAPP_getCurDevName(&devNameLen);
		devName=MMIFILE_DEVICE_UDISK;
		devNameLen=MMIFILE_DEVICE_UDISK_LEN_AD;
		if(devName != NULL)
		{
			memset(full_path, 0, sizeof(full_path));
			memset(ucs2_name_arr, 0, sizeof(ucs2_name_arr));
			
#ifdef SKY_MTSHOW_SUPPORT
        if(filename[0] == '/')
        {			
			ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)&filename[1], strlen(&filename[1]));
        }
        else
        {
            ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename, strlen(filename));
        }
#else
        ucs2_name_len = MMIMRAPP_GB2UCS((uint16*)ucs2_name_arr, (const uint8 *)filename, strlen(filename));
#endif
			if(	MMIFILE_CombineFullPath_fix(
				(const uint8*)devName, devNameLen,
				(const uint8*)g_mr_global.dir_ptr, g_mr_global.dir_len,
				(const uint8*)ucs2_name_arr, ucs2_name_len,
				(uint8*)full_path,
				&full_path_len
				))
			{
				sfs_handle = SFS_FindFirstFile((const uint16 *)full_path, &find_data);				
				MMIMR_Trace(("find_data.attr: 0x%x", find_data.attr));
				if(SFS_INVALID_HANDLE != sfs_handle)
				{
					SFS_FindClose(sfs_handle);
					if(SFS_ATTR_DIR & find_data.attr)
					{
						return MR_IS_DIR;
					}
					else
					{
						return MR_IS_FILE;
					}
				}
			}
		}
	}
	
	MMIMR_Trace(("mrapp. mr_info() MR_IS_INVALID"));	
	return MR_IS_INVALID;	
}
int mr_is_find_comma_site()
{
 int i=0;
	  
#ifdef  MRAPP_OVERSEA_SUPPORT	
          for(i=strlen(gappparam)-5;i>0;i--)
          {
                if(gappparam[i]==',')
			return i;
          }
          return -1;
#else
          for(i=3;i<strlen(gappparam);i++)
          {
             if(gappparam[i]==',')
			return i;
          }
          return -1;
#endif
}

BOOL mr_is_runapp_in_udisk(void)
{
    char tempfile[64]={0};
    char filepath[256]={0};
    mr_screeninfo screen={0};
	int commasite=-1;
	commasite=mr_is_find_comma_site();
    #ifdef  MRAPP_OVERSEA_SUPPORT
    MMIMR_Trace(("mr_is_runapp_in_udisk commasite=%d",commasite));
    if(commasite==-1)
		return FALSE;
    SCI_MEMCPY(tempfile,gappparam+commasite+1,strlen(gappparam)-commasite-5);
    MMIMR_Trace(("mr_is_runapp_in_udisk tempfile=%s",tempfile));

    mr_getScreenInfo(&screen);
    sprintf(filepath, "mrp%dX%d\\%s.mrp", screen.width, screen.height,tempfile);
    MMIMR_Trace(("mr_is_runapp_in_udisk filepath=%s",filepath));
    if(mr_info_udisk(filepath)==MR_IS_FILE)
		return TRUE;
    return FALSE;
    #else
    if(!strcmp(gappparam,"*J,mtshow"))
	return FALSE;
	if(commasite==-1)
	{
      SCI_MEMCPY(tempfile,gappparam+3,strlen(gappparam)-3);
	}
	else
	{
	  SCI_MEMCPY(tempfile,gappparam+3,commasite-3);
	}
    mr_getScreenInfo(&screen);
    sprintf(filepath, "app%d%d\\%s.mrp", screen.width, screen.height,tempfile);
    if(mr_info_udisk(filepath)==MR_IS_FILE)
		return TRUE;
    return FALSE;
    #endif
    
}

BOOL mr_is_enough_space_in_udisk(void)
{
	uint32 space_low2 = 0, space_high2 = 0;
	int32 account=0;  //剩余空间的大小
	int32 unit=0;      //剩余大小的单位
   if(MMIFILE_GetDeviceStatus((uint8*)MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
	 {
		MMIFILE_GetDeviceFreeSpace((uint8 *)MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN,&space_high2,&space_low2);
      
		if(space_high2 || (uint32)space_low2 > (uint32)0x7fffffff)
         {
					account = (space_high2<<22) + (space_low2>>10);//kb
					unit = 1024;
		 }
         else
         {
					account = space_low2;
					unit = 1;
		 }
		 if(account*unit>=512000)
		 	return TRUE;
		 return FALSE;
      }
   
	return FALSE;			
}
#endif

/*****************************************************************************/
// 	Description :设置默认文件设备与目录
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_setDefFilePath(void)
{
    if(TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN)
#ifdef DUAL_TCARD_SUPPORT
        || TRUE == MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN)
#endif
        )
    {
        
#ifdef MRAPP_USE_UDISK_FIRST
		if(mr_is_runapp_in_udisk()&&mr_is_enough_space_in_udisk())
		{
				  g_mr_global.device_type = FS_UDISK;
		}
		else
#endif
        {
        		 g_mr_global.device_type = FS_MMC;
		}
    }
    else
    {
        g_mr_global.device_type = FS_UDISK;
    }
	
	if(g_mr_global.device_type == FS_MMC){//t-card
		g_mr_dualsys_mng.devSel = 'c';		
	}else{//u-disk
		g_mr_dualsys_mng.devSel = 'b';		
	}
	
	g_mr_global.dir_ptr = (uint8*)MMIMRAPP_ROOT_DIR;
	g_mr_global.dir_len = MMIMRAPP_getUcstrLen((uint8*)MMIMRAPP_ROOT_DIR);
	MMIMR_Trace(("MMIMRAPP_setDefFilePath(), %d, %d", g_mr_global.device_type, g_mr_global.dir_len));

    mr_fs_create_vm_dir();
}

/*****************************************************************************/
// 	Description : 初始化资源
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_initAllExtraSrc(void)
{
	/*audio*/
	SCI_MEMSET(&s_mr_audio_mng, 0, sizeof(MRAPP_AUDIO_MNG_T));
	s_mr_audio_mng.audioHdle = INVALID_HANDLE;
	s_mr_audio_mng.soundVolume = MMI_AUD_VOLUME_MAX;
	s_mr_audio_mng.curStatus = LOADED;
	s_mr_audio_mng.curSetPlayPos = 0;
	
	/*dual sim*/
	g_mr_dualsys_mng.simSel = MMIAPISET_GetActiveSim();

#ifdef __MRAPP_WIFI_SUPPORT__
#ifdef __MMI_SKYQQ__
    if(GetSkyQQRunState() == QQ2008_STOP)
    {
        s_mr_wifi_mode = FALSE;
    }
#else
    s_mr_wifi_mode = FALSE;
#endif
#endif
}

/*****************************************************************************/
// 	Description : 检查并释放所有未释放的资源
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_freeAllExtraSrc(void)
{		
	if(s_mr_mp3_resume_locked == TRUE)
	{
		s_mr_mp3_resume_locked = FALSE;
	}
	
	/*audio*/
	if(s_mr_audio_mng.curStatus >= PLAYING && s_mr_audio_mng.curStatus <= PAUSE)
	{
		MMISRVAUD_Stop(s_mr_audio_mng.audioHdle);
		MMISRVMGR_Free(s_mr_audio_mng.audioHdle);
		s_mr_audio_mng.audioHdle = INVALID_HANDLE;
		s_mr_audio_mng.curSetPlayPos = 0;
		s_mr_audio_mng.curStatus = LOADED;
	}
	
#ifndef WIN32
#ifdef MRAPP_SIMUGAME_OPEN	
#ifdef EXTPCM	
	if(s_mr_pcm_mng.curStatus == PLAYING)
	{
		ExtPCM_SoundClose();
		s_mr_pcm_mng.curStatus = IDLE;
	}
#endif
	
	if(s_mr_envLock_state & ENVLOCK_IRAM){
		MMIMRAPP_nesActionInd(NES_UNLOCK_IRAM);
	}
	if(s_mr_envLock_state & ENVLOCK_ENV){
		MMIMRAPP_nesActionInd(NES_UNLOCK_OTHERS);
	}
#endif
#endif
	
	MMIMRAPP_setLcdAsynMode(0);
	mr_stopSound(0);
	if(s_mr_wallpaperBufNeedRestore){
		s_mr_wallpaperBufNeedRestore = FALSE;
		MMIIDLE_SetWallpaper(TRUE); /*lint !e718 !e18*/
	}
	
#ifdef __MRAPP_WIFI_SUPPORT__
    if(s_mr_wifi_mode == TRUE)
    {
#ifdef __MMI_SKYQQ__
        if(GetSkyQQRunState() == QQ2008_STOP)
        {
            s_mr_wifi_mode = FALSE;
        }
#else
        s_mr_wifi_mode = FALSE;
#endif
    }
#endif
	
	/*image*/
	MMIMRAPP_gifFree(&s_mr_gifHeader);
	
    if(s_mr_need_restore_backlight)
    {
        MMIMRAPP_SetAllowTurnOffBacklight(TRUE);
    }

    /*退出VM时恢复状态*/
    if(s_mr_new_sms_action == FALSE)
    {
        s_mr_new_sms_action = TRUE;
    }
	
	MMIMR_Trace(("g_mr_curApp :%d", g_mr_curApp));	
	switch(g_mr_curApp)
	{

		default:
		break;
	}	
}

/*****************************************************************************/
// 	Description : 当前启动的应用
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
void MMIMRAPP_setCurApp(MRAPP_APPENTRY_E app)
{
	g_mr_curApp = (uint32)app;
}

/*****************************************************************************/
// 	Description : 获取当前启动的应用
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
uint32 MMIMRAPP_GetCurApp(void)
{
	return g_mr_curApp;
}

/*****************************************************************************/
// 	Description : 进入某些应用中可事先选择pause mp3等后台播放。
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
void MMIMRAPP_pauseMp3(void)
{
	MMISRVAUD_ReqVirtualHandle(SKY_VIDEOPLAYER, MMISRVAUD_PRI_NORMAL);
    //MMIAPISET_StopAllRing(FALSE);
	s_mr_mp3_resume_locked = TRUE;
}

/*****************************************************************************/
// 	Description : 应用过程中恢复MP3等后台播放的函数。
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
void MMIMRAPP_resumeMp3(void)
{
	if(!s_mr_mp3_resume_locked)
	{
		MMISRVAUD_FreeVirtualHandle(SKY_VIDEOPLAYER);
	}
}

/*****************************************************************************/
// 	Description :获取一个空闲的SOCKET node
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_getAfreeSnode(void)
{
	int i;
	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		MMIMR_Trace(("s_mr_sockets[%d].s :%d", i, s_mr_sockets[i].s));	
		if(s_mr_sockets[i].s == INVALID_SOCKET)
			return i;
	}
	
	return -1;
}

/*****************************************************************************/
// 	Description :清空socket对列
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_freeAllSnode(void)
{
	int i;
	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		s_mr_sockets[i].s = INVALID_SOCKET;
		s_mr_sockets[i].state = MRAPP_SOCKET_CLOSED;
	}
}

/*****************************************************************************/
// 	Description :通过socket id 获取对应的对列index
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL int32 MMIMRAPP_findTheSnode(int32 s)
{
	int i;
	for(i = 0; i < MRAPP_SOCKET_MAX_NUM; i++)
	{
		MMIMR_Trace(("s_mr_sockets[%d].s :%d, s: %d", i, s_mr_sockets[i].s, s));	
		if(s_mr_sockets[i].s == s)
			return i;
	}
	
	return -1;
}

/*****************************************************************************/
// 	Description :建立连接
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_Sconnect(int32 s, int32 ip, uint16 port)
{
#ifndef WIN32
	struct sci_sockaddr sockAddr;
	int32 ret = 0;
	
	MMIMR_TraceN(("MMIMRAPP_Sconnect(), socket: %d, ip: 0x%x, port: %d", s, ip, port));
	sockAddr.family = AF_INET;
	sockAddr.port = htons(port);
	sockAddr.ip_addr = (unsigned long)htonl(ip); /*lint !e737*/
	SCI_MEMSET((void*)sockAddr.sa_data, 0, 8*sizeof(char));
	ret = sci_sock_connect((long)s, &sockAddr, sizeof(struct sci_sockaddr));	
	
	return ret;
#else
	return 0;
#endif
}

/*****************************************************************************/
// 	Description : GET MP3 锁状态
//	Global resource dependence : 
//  Author: limin
//	Note: 
/*****************************************************************************/
uint8 MMIMRAPP_GetMp3Lock(void)
{
	return s_mr_mp3_resume_locked;
}

/*****************************************************************************/
//	Description : combine full pathname from device, file type, filename
//	Global resource dependence : 
//	Author:liqing.peng
//	Note: len -byte
/*****************************************************************************/
LOCAL BOOLEAN MMIFILE_CombineFullPath_fix(
										  const uint8	   *device_ptr, 	   //in
										  uint16		   device_len,		   //in
										  const uint8	   *dir_ptr,		   //in
										  uint16		   dir_len, 		   //in
										  const uint8	   *file_name_ptr,	   //in, can be NULL
										  uint16		   file_name_len,	   //in
										  uint8 		   *full_path_ptr,	   //out
										  uint16		   *full_path_len_ptr  //out
										  )
{
	uint16		*wchar_ptr = NULL;
	uint32		j = 0;
	
	MMIMR_Trace(("mrapp, CombineFullPath: %d, %d, %d, %d, %d, %d", device_ptr, device_len, dir_ptr, dir_len, file_name_ptr, file_name_len));
	
    if((NULL == device_ptr)||(0 >= device_len || MMIFILE_DEVICE_NAME_MAX_LEN_AD < device_len))
    {
        return FALSE;
    }
	if(dir_len > MMIFILE_DIR_NAME_MAX_LEN_AD)
		return FALSE;

    /*modify by zack@20120411*/
    //应该支持传""的mr_info操作
	if(NULL == file_name_ptr || file_name_len > MMIFILE_FILE_NAME_MAX_LEN_AD)
	{
		return FALSE;
	}
	
    if(NULL == full_path_ptr || NULL == full_path_len_ptr)
    {
        return FALSE;
    }	
	
	//copy device to full path
	MMI_MEMCPY(full_path_ptr, device_len,
		device_ptr, device_len, device_len);
	
	j = device_len;
	
	wchar_ptr = (uint16 *)(full_path_ptr+j);
	*wchar_ptr = MMIFILE_COLON;
	wchar_ptr++;
	*wchar_ptr = MMIFILE_SLASH;
	
	j = j+4;
	
	//copy dir to full path
	if(dir_len > 0)
	{
		MMI_MEMCPY(full_path_ptr+j, dir_len,
			dir_ptr, dir_len, dir_len);
		
		j = j+dir_len;
	}
	
	if(NULL != file_name_ptr && file_name_len > 0)
	{
		wchar_ptr = (uint16 *)(full_path_ptr+j);
		if(dir_len > 0)
		{
			*wchar_ptr = MMIFILE_SLASH;
			j = j+2;
		}
		
		//copy file name to full path
		MMI_MEMCPY(full_path_ptr+j, file_name_len,
			file_name_ptr, file_name_len, file_name_len);
		
		j = j+file_name_len;
	}
	
	*full_path_len_ptr = j;
	
	return TRUE;
}

/*****************************************************************************/
// 	Description : 获取当前mr中的sim设置
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
MN_DUAL_SYS_E MMIMRAPP_GetActiveSim(void)
{
	MMIMR_Trace(("mrapp, curSim: %d, total Sim Num: %d", g_mr_dualsys_mng.simSel, MN_DUAL_SYS_MAX));
	return (MN_DUAL_SYS_E)g_mr_dualsys_mng.simSel;
}

/*****************************************************************************/
// 	Description : 检查注销mr模块使用中的 pdp
//	Global resource dependence : 
//  Author: qgp
//	Note: PARAM - WaitFunc, 等待pdp注销后继续处理的函数
//   		return- TRUE, pdp注销中，需要等待；FALSE，pdp空闲。
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_waitToDeactiveMrappPdp(MMIMRAPP_WAIT2DOPDPACT_FUNC WaitFunc, uint32 data)
{
	if(s_mr_netContext.mrNetActived)
	{
		if(s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVED || s_mr_netContext.gprsState == MRAPP_GPRS_ACTIVING)//acting can ir directly ?
		{
			MNGPRS_ResetAndDeactivePdpContextEx(MMIMRAPP_GetActiveSim());
			s_mr_netContext.gprsState = MRAPP_GPRS_DEACTIVEING;
		}
		
		if(s_mr_netContext.gprsState == MRAPP_GPRS_DEACTIVEING)
		{
			s_mr_netContext.deactWaitFunc = (void*)WaitFunc; /*lint !e611*/
			s_mr_netContext.data = (uint32)data;
			return TRUE;
		}
		s_mr_netContext.mrNetActived = FALSE;//2008-4-24 add
	}
	
	return FALSE;
}


/*****************************************************************************/
// 	Description : 检查mr_playSound() 启动的playing是否被外部中断。
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
void MMIMRAPP_playingCheck(void)
{	
	MMIMR_Trace(("MMIMRAPP_playingCheck(), count: %d", s_mr_mrPlaySound_info.Ckcount));
	
	/*if(s_mr_mrPlaySound_info.Ckcount++ > 0)//delay 1 count
	{
		s_mr_mrPlaySound_info.Ckcount = 0;
		mr_playSound((int)s_mr_mrPlaySound_info.type, s_mr_mrPlaySound_info.data, s_mr_mrPlaySound_info.dataLen, (int32)s_mr_mrPlaySound_info.loop);
	}*/
}

/*****************************************************************************/
// 	Description : 提高cpu clk
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_SetArmClkHigh(void)
{
#ifndef WIN32 
	if (CHNG_FREQ_REQ_HANDLER_NULL == s_mrapp_chng_freq_req_handler){
		s_mrapp_chng_freq_req_handler = CHNG_FREQ_GetReqHandler("mrapp");
		SCI_ASSERT(CHNG_FREQ_REQ_HANDLER_NULL != s_mrapp_chng_freq_req_handler);
	}								  	
	CHNG_FREQ_SetArmClk(s_mrapp_chng_freq_req_handler, FREQ_INDEX_DC_HIGH);
#endif
}

/*****************************************************************************/
// 	Description : 恢复clk到设置前状态
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_RestoreARMClk(void)
{
#ifndef WIN32 
	CHNG_FREQ_RestoreARMClk(s_mrapp_chng_freq_req_handler);
#endif
}

/*****************************************************************************/
// 	Description : 为了向前版本兼容
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC uint16 MMIMRAPP_GB2UCS(uint16 *ucs2_ptr, const uint8 *gb_ptr, uint16 len)
{
    return GUI_GB2UCS( ucs2_ptr, gb_ptr, len )*2;
}

/*****************************************************************************/
// 	Description : 设置lcd 异步刷新模式
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_setLcdAsynMode(int32 isSet)
{
	if(!s_mr_lcdasyn_state && isSet)
    {
		LCD_SetBrushMode(MAIN_LCD_ID, LCD_BRUSH_MODE_ASYNC); /*lint !e628 !e718 !e746*/
		s_mr_lcdasyn_state = TRUE;
	}
    else if(s_mr_lcdasyn_state && !isSet)
    {
		LCD_SetBrushMode(MAIN_LCD_ID, LCD_BRUSH_MODE_SYNC); /*lint !e628 !e718 !e746*/	
		s_mr_lcdasyn_state = FALSE;
	}
}

/*2009-10-22 111,17787 add. for overseas*/
/*****************************************************************************/
// 	Description : 初始化apn资料(主要为海外配置)
//	Global resource dependence : none
//  Author: qgp
//	Note: modify by zack@20120927 for dual T-Card
/*****************************************************************************/
void MMIMRAPP_initApn(void)
{
#ifndef WIN32
	MMIFILE_HANDLE f;
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	uint32 bytes_read;
	const uint8 *devName;
	uint16 devNameLen;
    
    devName = MMIMRAPP_getCurDevName(&devNameLen);
	memset(full_path, 0, sizeof(full_path));
	if(	MMIFILE_CombineFullPath_fix(
							(const uint8*)devName, devNameLen,
							(const uint8*)L"mythroad_spr", 2*strlen("mythroad_spr"),
							(const uint8*)L"useApnAct.dat", 2*strlen("useApnAct.dat"),
							(uint8*)full_path,
							&full_path_len
							))
	{
		f = (MR_FILE_HANDLE)MMIFILE_CreateFile((const uint16 *)full_path, SFS_MODE_OPEN_EXISTING | SFS_MODE_READ, NULL, NULL);
		if(f != FFS_INVALID_HANDLE){
			MMIFILE_ReadFile(f, (void *)&gDsmDataAccount, (uint32)sizeof(T_DSM_DATA_ACCOUNT), &bytes_read, NULL);
			mr_close(f);
		}
	}
#endif
}

/*****************************************************************************/
// 	Description : 保存当前有效apn资料
//	Global resource dependence : none
//  Author: qgp
//	Note: modify by zack@20120927 for dual T-Card
/*****************************************************************************/
void MMIMRAPP_saveApn(void)
{
#ifndef WIN32
	MMIFILE_HANDLE f;
	uint16 full_path_len = 0;
	uint16 ucs2_name_len = 0;
	uint32 bytes_write;
	const uint8 *devName;
	uint16 devNameLen;

    devName = MMIMRAPP_getCurDevName(&devNameLen);
	memset(full_path, 0, sizeof(full_path));
	if(	MMIFILE_CombineFullPath_fix(
							(const uint8*)devName, devNameLen,
							(const uint8*)L"mythroad_spr", 2*strlen("mythroad_spr"),
							(const uint8*)L"useApnAct.dat", 2*strlen("useApnAct.dat"),
							(uint8*)full_path,
							&full_path_len
							))
	{
		f = (MR_FILE_HANDLE)MMIFILE_CreateFile((const uint16 *)full_path, SFS_MODE_CREATE_ALWAYS | SFS_MODE_WRITE, NULL, NULL);
		if(f != FFS_INVALID_HANDLE){
			MMIFILE_WriteFile(f, (void *)&gDsmDataAccount, (uint32)sizeof(T_DSM_DATA_ACCOUNT), &bytes_write, NULL);
			mr_close(f);
		}
	}
#endif
}
/*2009-10-22 111,17787 add end*/

/*2010-02-24 add start*/
void MMIMRAPP_SetBackRun(MRAPP_APPENTRY_E app)
{
    g_mr_vmBackRun = app;
}
/*2010-02-24 add end*/

/* 2010-07-07 add start*/
void MMIMRAPP_PauseAudioPlayer(void)
{
    /*AudioService会根据优先级自动激活处于Paused状态的音频*/
#if 0
    if(s_mr_audio_mng.curStatus == PLAYING)
    {
        s_mr_need_resume_audio = TRUE;
		mr_platEx(2053, NULL, 0, NULL, NULL, NULL);
    }
#endif
}

void MMIMRAPP_ResumeAudioPlayer(void)
{
    /*AudioService会根据优先级自动激活处于Paused状态的音频*/
#if 0
    if(s_mr_need_resume_audio == TRUE)
    {
        MMIMRAPP_setSoundVolume(s_mr_audio_mng.soundVolume);
        mr_platEx(2063, NULL, 0, NULL, NULL, NULL);
        s_mr_need_resume_audio = FALSE;
    }
#endif
}
/* 2010-07-07 add end*/

/*****************************************************************************/
// 	Description : 获取VM中当前使用的sim卡的NetworkID
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
int32 MMIMRAPP_GetNetworkID(uint8** output, int32* output_len)
{
	MN_IMSI_T imsi;
	char *temp = (char*)tempTransBuf;
	MMIMR_Trace(("mrapp. MMIMRAPP_GetNetworkID()"));
	
    SCI_MEMSET(tempTransBuf, 0x00, sizeof(tempTransBuf));
	imsi = MNSIM_GetImsiEx(MMIMRAPP_GetActiveSim());
	MMIMRAPP_bcd2dec(imsi.imsi_val, (uint8*)temp, 3);
	temp[6] = '\0';
    
	*output = (uint8*)&temp[1];
    *output_len = strlen(&temp[1]);

	MMIMR_Trace(("mrapp. MMIMRAPP_GetNetworkID() output: %s output_len: %d", *output, *output_len));

    return MR_SUCCESS;
}

/* begin:added by Tommy.yan 20120214 */
/***************************************************************************/
//  Description : get current record handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_GetRecordHandle(void)
{
	return s_mr_record_handle;
}

/*****************************************************************************/
// 	Description :set record handle
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_SetRecordHandle(MMISRV_HANDLE_T handle)
{
	s_mr_record_handle = handle;
}

/***************************************************************************/
//  Description : get current record handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_GetAudioHandle(void)
{
	return s_mr_audio_mng.audioHdle;
}

/*****************************************************************************/
// 	Description :set record handle
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_SetAudioHandle(MMISRV_HANDLE_T handle)
{
	s_mr_audio_mng.audioHdle = handle;
}

/***************************************************************************/
//  Description : get current record handle
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_GetMixAudioHandle(void)
{
	return s_mr_mix_audio_handle;
}

/*****************************************************************************/
// 	Description :set record handle
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
LOCAL void MMIMRAPP_SetMixAudioHandle(MMISRV_HANDLE_T handle)
{
	s_mr_mix_audio_handle = handle;
}

/***************************************************************************/
//  Description : create audio handle by buf
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_CreateAudioFileHandle(
													MMISRVAUD_RING_FMT_E ring_type,
													const wchar *name_ptr,
													MMISRVMGR_NOTIFY_FUNC call_back
													)
{
	MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    MMISRV_HANDLE_T handle = INVALID_HANDLE;

	req.ind_data = 0;
    req.is_async = FALSE;
    req.is_auto_resume_off = FALSE;
    req.notify = call_back;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.duation = 0;
    audio_srv.eq_mode = 0;
    audio_srv.is_mixing_enable = FALSE;
    audio_srv.play_times = s_mr_play_times;
	audio_srv.volume = s_mr_audio_mng.soundVolume;
	audio_srv.all_support_route = MMISRVAUD_ROUTE_AUTO;
    audio_srv.info.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
	audio_srv.info.ring_file.fmt = ring_type;
    audio_srv.info.ring_file.name = name_ptr;
    audio_srv.info.ring_file.name_len = MMIAPICOM_Wstrlen(name_ptr);
	
    handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
	MMIMR_Trace(("mrapp MMIMRAPP_CreateAudioFileHandle handle: %d", handle));

	return handle;
}

/***************************************************************************/
//  Description : create audio handle by file
//  Global resource dependence : none
//  Author: yaye.jiang
//  Note: 
/*****************************************************************************/
LOCAL MMISRV_HANDLE_T MMIMRAPP_CreateAudioBufHandle(
													MMISRVAUD_RING_FMT_E ring_type,
                     								const uint8* pucData, 
                     								uint32 uiDataLength,
                     								MMISRVMGR_NOTIFY_FUNC call_back
                     								)
{
	MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    MMISRV_HANDLE_T handle = INVALID_HANDLE;
    
    req.ind_data = 0;
    req.is_async = FALSE;
    req.is_auto_resume_off = FALSE;
    req.notify = call_back;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.duation = 0;
    audio_srv.eq_mode = 0;
    audio_srv.is_mixing_enable = FALSE;
    audio_srv.play_times = s_mr_play_times;
	audio_srv.volume = s_mr_audio_mng.soundVolume;
	audio_srv.all_support_route = MMISRVAUD_ROUTE_AUTO;
    audio_srv.info.ring_buf.type = MMISRVAUD_TYPE_RING_BUF;
    audio_srv.info.ring_buf.fmt = ring_type;
    audio_srv.info.ring_buf.data = pucData;
    audio_srv.info.ring_buf.data_len = uiDataLength;
	
    handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
	MMIMR_Trace(("mrapp MMIMRAPP_CreateAudioBufHandle handle: %d", handle));

	return handle;
}
/* end:added by Tommy.yan 20120214 */

/* begin:add by Tommy.yan 20111102 */
/*****************************************************************************/
// 	Description : 获取当前后台运行应用的相关信息
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetBackRunAPPInfo(MRAPP_BACKRUN_INFO_T *info)
{/*lint !e18*/
    if(info != NULL)
    {
        info->app_id     = s_mr_backrun_info.app_id;
        info->icon_state = s_mr_backrun_info.icon_state;
    }
}

#ifdef UI_MULTILAYER_SUPPORT
int32 mr_layer_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    UILAYER_RESULT_E result = 0;
    UILAYER_CREATE_T layer = {0};
	mr_layer_create_info_t *pReq = (mr_layer_create_info_t *)input;
            
	if(pReq == NULL||output == NULL||output_len == NULL||input_len < sizeof(mr_layer_create_info_t)||pReq->buffer== NULL||((pReq->size) < (pReq->w*pReq->h*2)))
    {   
		return MR_FAILED;
    }

    layer.lcd_id = GUI_MAIN_LCD_ID;
    layer.owner_handle = MMIMRAPP_BASE_WIN_ID;
    layer.offset_x = pReq->x;
    layer.offset_y = pReq->y;
    layer.width = pReq->w;
    layer.height = pReq->h;
    layer.is_bg_layer = TRUE;
    layer.is_static_layer = TRUE; 
    
    result = mr_layer_create_ex(&layer, &g_mr_common_rsp.p1, pReq->buffer, pReq->size);
    //MMIMR_Trace(("SKYENGINE [layer] mr_layer_create result:%d handle:%d", result, g_mr_common_rsp.p1));

    if(result == UILAYER_RESULT_SUCCESS)
    {
        *output = (uint8*)&g_mr_common_rsp;
        *output_len = sizeof(UILAYER_HANDLE_T);
        return MR_SUCCESS;
    }
    else
    {
        return MR_FAILED;
    }
}

int32 mr_layer_release(int32 param)
{
    GUI_LCD_DEV_INFO dev_info = {0};

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = param;

    if(UILAYER_IsEmptyLayerHandle(&dev_info))
    {
        return MR_FAILED;
    }
    //MMIMR_Trace(("SKYENGINE [layer] mr_layer_release actHandle:%d handle:%d", g_mr_active_layer.block_id, param));

    if(g_mr_active_layer.block_id == param)
    {
        SCI_MEMSET(&g_mr_active_layer, 0x00, sizeof(mr_layer_active_info_t))
        g_mr_active_layer.block_id = UILAYER_NULL_HANDLE;
    }

    mr_layer_release_ex(&dev_info);

    return MR_SUCCESS;
}

int32 mr_layer_set_postion(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    GUI_LCD_DEV_INFO dev_info = {0};
    mr_layer_position_info_t *pReq = (mr_layer_position_info_t *)input;

    if(input == NULL||input_len < sizeof(mr_layer_position_info_t))
    {
		return MR_FAILED;
    }

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = g_mr_active_layer.block_id;

    //MMIMR_Trace(("SKYENGINE [layer] mr_layer_set_postion actHandle:%d x:%d y:%d", g_mr_active_layer.block_id, pReq->x, pReq->y));
    if(UILAYER_IsEmptyLayerHandle(&dev_info))
    {
        return MR_FAILED;
    }
    
    UILAYER_SetLayerPosition(&dev_info, pReq->x, pReq->y);
    return MR_SUCCESS;
}

int32 mr_layer_get_base_handle(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    GUI_LCD_DEV_INFO *dev_info_ptr = MMITHEME_GetDefaultLcdDev();

	if(output == NULL || output_len == NULL || dev_info_ptr == NULL)
    {   
		return MR_FAILED;
    }

    g_mr_common_rsp.p1 = dev_info_ptr->block_id;
	*output = (uint8*)&g_mr_common_rsp;
	*output_len = sizeof(UILAYER_HANDLE_T);
    
    //MMIMR_Trace(("SKYENGINE [layer] mr_layer_get_base_handle baseHandle:%d", g_mr_common_rsp.p1));

	return MR_SUCCESS;
}

int32 mr_layer_set_active(int32 param)
{
    UILAYER_HANDLE_T oldHandle = NULL;
    GUI_LCD_DEV_INFO dev_info = {0};
    GUI_LCD_DEV_INFO *default_dev_info_ptr = MMITHEME_GetDefaultLcdDev();

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = param;

    if(UILAYER_IsEmptyLayerHandle(&dev_info))
    {
        return MR_FAILED;
    }

    if(g_mr_active_layer.block_id == UILAYER_NULL_HANDLE)
    {
        oldHandle = default_dev_info_ptr->block_id;
    }
    else
    {
        oldHandle = g_mr_active_layer.block_id;
    }
    g_mr_active_layer.block_id = param;
    //MMIMR_Trace(("MR_JSKY mr_layer_set_active handle=%d old_act_handle = %d", param, oldHandle));    
    return MR_PLAT_VALUE_BASE+oldHandle;
}

int32 mr_layer_get_active_handle(void)
{
    GUI_LCD_DEV_INFO dev_info = {0};
    GUI_LCD_DEV_INFO *default_dev_info_ptr = MMITHEME_GetDefaultLcdDev();

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = g_mr_active_layer.block_id;

    if(UILAYER_IsEmptyLayerHandle(&dev_info))
    {
        return default_dev_info_ptr->block_id;
    }
    else
    {
        return g_mr_active_layer.block_id;
    }
}

int32 mr_layer_set_blt_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    GUI_LCD_DEV_INFO dev_info = {0};
    UILAYER_APPEND_BLT_T blt_layer = {0};
    mr_layer_blt_info_t *pReq = (mr_layer_blt_info_t *)input;

	if(input == NULL || input_len < sizeof(mr_layer_blt_info_t))
    {   
		return MR_FAILED;
    }

    //MMIMR_Trace(("mr_layer_set_blt_layer layer1 = %d layer2 = %d layer3 = %d layer4 = %d", pReq->layer1, pReq->layer2, pReq->layer3, pReq->layer4));

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = pReq->layer1;
    if(pReq->layer1 != NULL && !UILAYER_IsEmptyLayerHandle(&dev_info) && !UILAYER_IsMainLayerHandle(&dev_info))
    {
        blt_layer.layer_level = UILAYER_LEVEL_NORMAL;
        blt_layer.lcd_dev_info.lcd_id   = dev_info.lcd_id;
        blt_layer.lcd_dev_info.block_id = dev_info.block_id;
        UILAYER_AppendBltLayer(&blt_layer);
    }

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = pReq->layer2;
    if(pReq->layer2 != NULL && !UILAYER_IsEmptyLayerHandle(&dev_info) && !UILAYER_IsMainLayerHandle(&dev_info))
    {
        blt_layer.layer_level = UILAYER_LEVEL_NORMAL;
        blt_layer.lcd_dev_info.lcd_id   = dev_info.lcd_id;
        blt_layer.lcd_dev_info.block_id = dev_info.block_id;
        UILAYER_AppendBltLayer(&blt_layer);
    }

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = pReq->layer3;
    if(pReq->layer3 != NULL && !UILAYER_IsEmptyLayerHandle(&dev_info) && !UILAYER_IsMainLayerHandle(&dev_info))
    {
        blt_layer.layer_level = UILAYER_LEVEL_NORMAL;
        blt_layer.lcd_dev_info.lcd_id   = dev_info.lcd_id;
        blt_layer.lcd_dev_info.block_id = dev_info.block_id;
        UILAYER_AppendBltLayer(&blt_layer);
    }

    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = pReq->layer4;
    if(pReq->layer4 != NULL && !UILAYER_IsEmptyLayerHandle(&dev_info) && !UILAYER_IsMainLayerHandle(&dev_info))
    {
        blt_layer.layer_level = UILAYER_LEVEL_NORMAL;
        blt_layer.lcd_dev_info.lcd_id   = dev_info.lcd_id;
        blt_layer.lcd_dev_info.block_id = dev_info.block_id;
        UILAYER_AppendBltLayer(&blt_layer);
    }

    return MR_SUCCESS;
}

int32 mr_layer_get_active_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    UILAYER_INFO_T   layer_info = {0};
    GUI_LCD_DEV_INFO dev_info   = {0};
    GUI_LCD_DEV_INFO *default_dev_info_ptr = MMITHEME_GetDefaultLcdDev();
            
	if(output == NULL || output_len == NULL)
		return MR_FAILED;
        
    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = g_mr_active_layer.block_id;

    if(g_mr_active_layer.block_id != UILAYER_NULL_HANDLE && !UILAYER_IsEmptyLayerHandle(&dev_info))
    {
		g_mr_layer_info.handle = g_mr_active_layer.block_id;
        UILAYER_GetLayerInfo(&dev_info, &layer_info);
    }
    else
    {
		g_mr_layer_info.handle = default_dev_info_ptr->block_id;
        UILAYER_GetLayerInfo(default_dev_info_ptr, &layer_info);
    }

	g_mr_layer_info.w = layer_info.layer_width;
	g_mr_layer_info.h = layer_info.layer_height;
#if (SPR_VERSION >= 0x10A1140)
	g_mr_layer_info.buffer = (GUI_COLOR_T*)UILAYER_GetLayerBufferPtrEx(&dev_info);
#else
    g_mr_layer_info.buffer = (void*)layer_info.layer_buf_ptr;
#endif
    //SCI_TRACE_MID("MR_JSKY mr_layer_get_active_layer handle=%d buf_ptr=0x%08x w=%d h=%d", g_mr_layer_info.handle, g_mr_layer_info.buffer, g_mr_layer_info.w, g_mr_layer_info.h);    
	*output = (uint8*)&g_mr_layer_info;
	*output_len = sizeof(g_mr_layer_info);
	return MR_SUCCESS;
}

int32 mr_layer_set_alpha(BOOLEAN enable, int32 param)
{
    GUI_LCD_DEV_INFO dev_info = {0};
    
    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = g_mr_active_layer.block_id;
    
    if(UILAYER_IsEmptyLayerHandle(&dev_info))
    {
        return MR_FAILED;
    }

    if(enable)
    {
        if(param > 255)
        {
            param = 255;
        }
        param = 255-param;
    }
    else
    {
        param = 255;
    }
    //SCI_TRACE_MID("MR_JSKY mr_layer_set_alpha enable=%d handle=%d alpha=%d", enable, g_mr_active_layer.block_id, param);    
    UILAYER_SetLayerAlpha(&dev_info, param);
    return MR_SUCCESS;
}

int32 mr_layer_set_transparent(BOOLEAN enable, int32 param)
{
    GUI_LCD_DEV_INFO dev_info = {0};
    
    dev_info.lcd_id   = GUI_MAIN_LCD_ID;
    dev_info.block_id = g_mr_active_layer.block_id;
    
    if(UILAYER_IsEmptyLayerHandle(&dev_info))
    {
        return MR_FAILED;
    }
    //SCI_TRACE_MID("MR_JSKY mr_layer_set_transparent enable=%d handle=%d transprent=%d", enable, g_mr_active_layer.block_id, param);    
    UILAYER_SetLayerColorKey(&dev_info, enable, MMI_BLUE_COLOR);

    return MR_SUCCESS;
}

int32 mr_jsky_push_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    GUI_RECT_T *rect = NULL;

    if(g_mr_active_layer.clips_top >= MR_LAYER_CLIP_STACK_COUNT)
    {
        return MR_FAILED;
    }

    rect = &g_mr_active_layer.clips[g_mr_active_layer.clips_top];

    rect->left = g_mr_active_layer.clip.left; 
    rect->top  = g_mr_active_layer.clip.top;
    rect->right  = g_mr_active_layer.clip.right;
    rect->bottom = g_mr_active_layer.clip.bottom;

    g_mr_active_layer.clips_top++;
    //SCI_TRACE_MID("MR_JSKY mr_jsky_push_clip total=%d rect[%d %d %d %d]", g_mr_active_layer.clips_top, rect->left, rect->top, rect->right, rect->bottom);    
	return MR_SUCCESS;
}

int32 mr_jsky_pop_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
    GUI_RECT_T *rect = NULL;

    if(g_mr_active_layer.clips_top <= 0)
    {
        return MR_FAILED;
    }

    rect = &g_mr_active_layer.clips[--(g_mr_active_layer.clips_top)];

    g_mr_active_layer.clip.left = rect->left; 
    g_mr_active_layer.clip.top  = rect->top;
    g_mr_active_layer.clip.right  = rect->right;
    g_mr_active_layer.clip.bottom = rect->bottom;
    //SCI_TRACE_MID("MR_JSKY mr_jsky_pop_clip total=%d rect[%d %d %d %d]", g_mr_active_layer.clips_top, rect->left, rect->top, rect->right, rect->bottom);    
	return MR_SUCCESS;
}

int32 mr_jsky_set_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	skyclip *clip = (skyclip*)input;

	if(clip != NULL)
	{
    	//SCI_TRACE_MID("MR_JSKY mr_jsky_set_clip l:%d t:%d r:%d b:%d", clip->x1, clip->y1, clip->x2, clip->y2);
		g_mr_active_layer.clip.left = clip->x1;
		g_mr_active_layer.clip.top  = clip->y1;
		g_mr_active_layer.clip.right  = clip->x2;
		g_mr_active_layer.clip.bottom = clip->y2;

		return MR_SUCCESS;
	}
	else 
	{
		return MR_FAILED;
	}
}

int32 mr_jsky_get_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	skyclip *clip = (skyclip*)output;

	if((output != NULL) && (output_len != NULL))
	{
		clip->x1 = g_mr_active_layer.clip.left;
		clip->y1 = g_mr_active_layer.clip.top;
		clip->x2 = g_mr_active_layer.clip.right;
		clip->y2 = g_mr_active_layer.clip.bottom;
        
    	//MMIMR_Trace(("MR_JSKY mr_jsky_get_clip l:%d t:%d r:%d b:%d", clip->x1, clip->y1, clip->x2, clip->y2));
    	
		*output_len = sizeof(skyclip);
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}
#endif

PUBLIC int32 MMIMRAPP_GetVMState(void)
{/*lint !e18*/
    int32 vm_state = 0;
    
    if(s_mr_allReady)
    {
        if(MMIMRAPP_IsVMPaused()) /*lint !e718 !e746*/
        {
            vm_state = VM_PAUSE;
        }
        else
        {
            if(MMIMRAPP_GetCurHungApp() != MRAPP_MAX) /*lint !e718 !e746*/
            {
                vm_state = VM_BACK_RUN;
            }
            else
            {
                vm_state = VM_RUN;
            }
        }
    }
    else
    {
        vm_state = VM_STOP;
    }

    MMIMR_Trace(("mrapp MMIMRAPP_GetVMState vm_state = %d", vm_state));
    return vm_state;
}

int32 GetDsmState(void)
{
	return MMIMRAPP_GetVMState();
}
/* end:add by Tommy.yan 20111102 */

#endif//#_MMIMRAPP_FUNC_C_
#endif//#end MRAPP_SUPPORT

