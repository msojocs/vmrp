/****************************************************************************
** File Name:      mmimrapp.h                                               *
** Author:                                                                  *
** Date:           05/15/2006                                               *
** Copyright:      2003 Spreatrum, Incoporated. All Rights Reserved.        *
** Description:    this file is use to describe the mr app module functions *
*****************************************************************************
**                         Important Edit History                           *
** -------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                              *
** 03/2006        		       Create
** 
****************************************************************************/
#ifndef  _MMIMRAPP_H_    
#define  _MMIMRAPP_H_  

 /**--------------------------------------------------------------------------*
 **                         Include Files                                     *
 **---------------------------------------------------------------------------*/
#include "mmidisplay_data.h"
#include "mmiset_export.h"
#include "mn_type.h"
#include "mmk_type.h"
#include "block_mem_def.h"
#include "guistatusbar.h"
#include "mmipdp_export.h"

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

#define PORT_BASE_NILE//porting base mocor (new rocky, nile,L etc..)

#define EXTPCM //是否开启nes 声音播放
 
 /*应用屏幕size*/
/*modify by zack@20120828 start*/
#if defined(MAINLCD_LOGIC_ANGLE_90) || defined(MAINLCD_LOGIC_ANGLE_270)
#define MR_SCREEN_WIDTH 						(MMI_MAINSCREEN_HEIGHT)
#define MR_SCREEN_HEIGHT 						(MMI_MAINSCREEN_WIDTH)
#else
#define MR_SCREEN_WIDTH 						(MMI_MAINSCREEN_WIDTH)
#define MR_SCREEN_HEIGHT 						(MMI_MAINSCREEN_HEIGHT)
#endif
/*modify by zack@20120828 end*/

/*
  Description: 分配给虚拟机内存值大小，单位为字节,系统最小内存为650K，视不同情况配置

  68等高端机建议内存分配在2M以上，其他情况下根据屏幕分辨率情况及实际手机内存剩余情况下尽量往高配置
*/
#if defined(PLATFORM_SC6800H)
	#define __MRAPP_CFG_VAR_VMMEM_SIZE__ (4*1024*1024)
#else
#if defined(MAINLCD_SIZE_320X480)
	#define __MRAPP_CFG_VAR_VMMEM_SIZE__ (1.5*1024*1024)
#elif defined(MAINLCD_SIZE_240X400)  
	#define __MRAPP_CFG_VAR_VMMEM_SIZE__ (1024*1024)
#else
	#define __MRAPP_CFG_VAR_VMMEM_SIZE__ (800*1024)
#endif
#endif

#define   __MR_CFG_VMMEM_CUSTOM__        //是否支持MRP自定义内存大小

#ifdef __MR_CFG_VMMEM_CUSTOM__
#define __MRAPP_CFG_VAR_VMMEM_MAX_SIZE__ (2.5*1024*1024)
#endif
/*
  Description: SPR 软件版本号，16进制表示，不要忘了前面的0x。
  	移植前请务必确认好客户目标工程的基线版本号，并正确设置以下宏的值
*/
#define __MR_CFG_VAR_SPR_VERSION__		(0x12A1219)//支持12B,12C1234,12C1304
#define SPR_VERSION		                 __MR_CFG_VAR_SPR_VERSION__        


//:TODO: 请务必关注下面几个应用选择的宏，确定不使用的应用请关闭对应的宏，以节约代码空间
 /*应用选择开关*/
#ifdef MRAPP_OVERSEA_SUPPORT
#define __MRAPP_OVERSEA__  //海外版本
#endif
   
/*
  Description: 是否支持扩展机型字段
*/
#ifndef __MRAPP_OVERSEA__
#define __MR_CFG_FEATURE_HANDSETID_EX__		
#endif 

/*
  Description: 是否支持从U盘启动优先
*/
//#define MRAPP_USE_UDISK_FIRST


#ifdef __MRAPP_OVERSEA__
#define MRAPP_MRPSTORE_OPEN	  //MRP Store
#define MRAPP_SKYBUDDY_OPEN   //skybuddy
//#define MRAPP_MSN_OPEN		    //MSN
//#define MRAPP_FACEBOOK_OPEN     //Facebook
//#define MRAPP_TWITTER_OPEN      //Twitter
//#define MRAPP_SKYPE_OPEN        //Skype
//#define MRAPP_YAHOO_OPEN        //Yahoo Message
#define MRAPP_BIRD_OPEN	      //愤怒小鸟
#define MRAPP_TALKCAT_OPEN	  //给力猫
#define MRAPP_FRUIT_OPEN		//切水果
#else
#define MRAPP_NETGAME_OPEN	  //冒泡网游
#define MRAPP_COOKIE_OPEN	  //曲奇
#define MRAPP_FETION_OPEN	  //飞信
#define MRAPP_SDKQQ_OPEN	  //QQ
#define MRAPP_KAIXIN_OPEN	  //开心网
#define MRAPP_CLOCK_OPEN	  //网络时钟
#define MRAPP_XLWB_OPEN       //新浪微博
#define MRAPP_BIRD_OPEN	      //愤怒小鸟
#define MRAPP_TALKCAT_OPEN	  //给力猫
#define MRAPP_SKIRT_OPEN	  //吹裙子
#define MRAPP_FRUIT_OPEN	  //切水果
#define MRAPP_BYDR_OPEN	      //捕鱼达人

//#define __MRAPP_DOWNLOAD__    //曲奇助手
//#define MRAPP_QIPAI_OPEN	    //冒泡棋牌
//#define MRAPP_DDZ_OPEN	    //斗地主
//#define MRAPP_FARM_OPEN       //冒泡农场
//#define MRAPP_TIANQI_OPEN     //天气助手
//#define MRAPP_TOOL_OPEN       //实用工具
//#define MRAPP_SIMUGAME_OPEN   //NES模拟器
 
#endif

/*是否固化应用，否则可选择将对应的mrp预置T卡,具体操作请咨询fae*/

//#define SDKQQ_FIRMED
//#define SKYREAD_FIRMED
//#define FETION_FIRMED
//#define KAIXIN_FIRMED
//#define SKYQQ_FIRMED
#ifdef __MRAPP_OVERSEA__
//#define FACEBOOK_FIRMED
//#define YAHOO_FIRMED
//#define SKYPE_FIRMED
//#define TWITTER_FIRMED
//#define FTALK_FIRMED
//#define MSN_FIRMED
//#define TALKCAT_FIRMED
//#define BIRD_FIRMED
//#define FRUIT_FIRMED
#endif

#ifndef TOUCHPANEL_TYPE_NONE //modify by zack@20100627 for 6600L
#define MR_HANDSET_IS_SUPPORT_TOUCHPANEL//是否支持触摸屏
#endif

#ifdef WIFI_SUPPORT
#ifndef __MRAPP_OVERSEA__
#define __MRAPP_WIFI_SUPPORT__     //是否支持WIFI
#endif
#endif
//#define __MRAPP_MOTION_SUPPORT__   //是否支持传感器
#define __MR_CFG_LICENSE_MANAGER__   //是否支持本地应用绑定
#define __MR_CFG_LOCALAPP_MANAGER__  //是否支持本地应用入口绑定
#define __MR_RECORD_SUPPORT__        //是否支持录音
//#define __MR_CFG_CAMERA_SUPPORT__    //是否支持Camera接口
#ifdef CAP_TP_SUPPORT
#define __MR_CFG_MULTITP_SUPPORT__   //是否支持多点触摸
#endif
#ifdef DYNAMIC_MAINMENU_SUPPORT
//#define __MR_CFG_SHORTCUT_SUPPORT__  //是否支持曲奇桌面化
#endif
//#define __MR_SMS_SET_STATE_SUPPORT__	//是否支持短信状态修改
//#define __MR_SEND_MMS_SUPPORT__			//是否支持彩信发送

/*平台音量设置最大值*/
#define MMI_AUD_VOLUME_MAX 					MMISET_VOL_MAX

/*确保以下的值与情景模式中的对应item的位置值相同(无声音的模式值)*/
#define MSG_RING_TYPE_VIBRATE 				1//信息提示－震动
#define VOLTAGE_WARN_TYPE_NOTRING 			1//低压警告－关
#define FLIP_RING_TYPE_NOTRING 				0//翻盖音－关
#define CALL_RING_TYPE_VIBRATE 				1//来电提示－震动
#define ALARM_RING_TYPE_VIBRATE 			    1//闹钟提示－震动

/*定义字体size，注意:平台的字体size改变的时候，需要修改下
面的定义，必须使各个宏值为有效值*/
#define MRAPP_FONT_SIZE_SMALL 				MMI_DEFAULT_SMALL_FONT
#define MRAPP_FONT_SIZE_MID 				MMI_DEFAULT_NORMAL_FONT
#define MRAPP_FONT_SIZE_BIG 				MMI_DEFAULT_BIG_FONT

#define MRAPP_FONT_UI_SIZE 					MMI_DEFAULT_TEXT_FONT//平台统一UI(如菜单)的字体大小

/*mrp文件存储的根目录*/
#define MMIMRAPP_ROOT_DIR		"\x6d\x00\x79\x00\x74\x00\x68\x00\x72\x00\x6f\x00\x61\x00\x64\x00\x5f\x00\x73\x00\x70\x00\x72\x00\x00\x00"//mythroad_spr


/*nes模拟器的rom文件存放目录的全名,型如:xxx/xxx, 注:前面不要加'/', 应用会自动检查U盘跟外插卡 */
//#define MMIMRAPP_NES_DIR_NAME			"\x00\x67\x00\x61\x00\x6d\x00\x65\x00\x73\x00\x00"//"games"		
#define MMIMRAPP_NES_DIR_NAME			"games"		

/*厂商ID*/
#define MRAPP_MANUFACTORY 			"skysple"    //厂商号,length limited to 7byte

#if(MR_SCREEN_WIDTH == 240 && MR_SCREEN_HEIGHT == 320)
#define MRAPP_HANDSET 					"t240320"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 240 && MR_SCREEN_HEIGHT == 400)
#define MRAPP_HANDSET 					"t240400"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 320 && MR_SCREEN_HEIGHT == 480)
#define MRAPP_HANDSET 					"t320480"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 176 && MR_SCREEN_HEIGHT == 220)
#define MRAPP_HANDSET 					"t176220"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 220 && MR_SCREEN_HEIGHT == 176)
#define MRAPP_HANDSET 					"t220176"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 272 && MR_SCREEN_HEIGHT == 480)
#define MRAPP_HANDSET 					"t272480"   //手机型号,length limited to 7byte
#else
#error "Unknown screen size, please call SKY-MOBI."
#endif

/*
  Description: 手机扩展机型ID，这个字段会和机型字段连接，连接字符串的长度小于34字节
  	比如我们分配的是m900扩展机型是handset1那么返回给应用的将是m900-handset1

  Note: 要支持此功能先启用__MR_CFG_FEATURE_HANDSETID_EX__宏
*/
#define __MR_CFG_VAR_HANDSETID_EX__					"handset1"  // length < 26
#define __MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__	    34 // 8 + 26

//virtual handle 用于进入我们的应用时停掉其他audio声音
#define SKY_VIDEOPLAYER  "Sky videoplayer"


/*应用入口参数*/
typedef enum
 {
	MRAPP_COOKIE    = 800001, //曲奇
	MRAPP_MRPSTORE  = 810010, //MRPStore
	MRAPP_SKYBUDDY  = 200004, //SkyBuddy
	MRAPP_APPLIST   = 9000,   //applist
	MRAPP_NETGAME   = 400101, //冒泡网游
	MRAPP_QIPAI     = 1008,   //冒泡网游
	MRAPP_DDZ       = 390028, //斗地主
	MRAPP_EBOOK     = 2908,  //凯阅
	MRAPP_MPMUSIC   = 2913,  //冒泡音乐盒
	MRAPP_SDKQQ     = 203103,//SDKQQ
	MRAPP_MSN       = 203102, //msn
	MRAPP_CLOCK     = 6008,  //网络时钟
	MRAPP_SKYQQ     = 203106,//SKYQQ
	MRAPP_FETION    = 6005,  //飞信
	MRAPP_KAIXIN    = 6020,  //开心网
	MRAPP_FARM      = 391129, //冒泡农场
	MRAPP_TOOL      = 800000, //冒泡工具
	MRAPP_ENDICT    = 391122, //英汉字典
	MRAPP_CHDICT    = 391014, //新华字典
	MRAPP_XLWB      = 391049, //新浪微博
	MRAPP_TIANQI    = 2919,   //天气助手
	MRAPP_FACEBOOK  = 810002, //Facebook
	MRAPP_SKYPE     = 391022,  //Skype
	MRAPP_YAHOO     = 810000,  //Yahoo message
	MRAPP_TWITTER   = 810006,  //Twitter
#ifdef MRAPP_SIMUGAME_OPEN
	MRAPP_NESGAME   = 9107,
#endif
	MRAPP_TALKCAT	= 2924,	  //给力猫
	MRAPP_BIRD   	= 1016,   //疯狂的小鸟
	MRAPP_SKIRT	= 1020,	  //吹裙子
	MRAPP_FRUIT	= 2937,   //水果忍者
	MRAPP_BYDR	= 2933,	  //捕鱼达人
	MRAPP_SHOUXIN	= 2934,	  //手信
	MRAPP_TEST      = 0xFFFF,

	MRAPP_MAX = 127//MAX ID byte
 }MRAPP_APPENTRY_E;


#ifdef __MR_CFG_LICENSE_MANAGER__
/**
 * \brief 本地应用权限校验入口信息
 */
typedef struct {
	void (*mr_lapp_license_register_f)(void);
}mr_lapp_license_entry_t;


#define MR_LAPP_LICENSE_MANAGER_BEGIN()	const mr_lapp_license_entry_t g_mr_lapp_license_entries[] = {
#define MR_LAPP_LICENSE_MANAGER_END()	{0}}
#define MR_LAPP_LICENSE_ENTRY(appname, reg)	{reg},
#endif

#ifdef __MR_CFG_LOCALAPP_MANAGER__
/**
 * \brief 本地应用入口函数
 */
typedef int32 (*startSkyLapp)(uint8* param);
#endif
 
/**--------------------------------------------------------------------------*
 **                         STRUCT                               *
 **--------------------------------------------------------------------------*/

typedef BOOLEAN (*MMIMRAPP_WAIT2DOPDPACT_FUNC)(uint32 data);

#define MRAPP_MD_MAXNUM 1//NOTE: THIS VALUE PLS EQUAL TO E_MR_MD_MAX
typedef enum
{
	E_MR_MD_GPRS = 0, //GPRS
	E_MR_MD_MAX
}e_mrapp_phone_mod;


/**--------------------------------------------------------------------------*
 **                         FUNCTINS                                         *
 **--------------------------------------------------------------------------*/

/*****************************************************************************/
// 	Description : get and check to proess some ps event msg, as :the pdp activate and deactivate event
//	Global resource dependence : 
//  Author: qgp
//  Note : 
/*****************************************************************************/
MMI_RESULT_E MMIMRAPP_handlePsMsg(MMIPDP_CNF_INFO_T *msg_ptr);

/*****************************************************************************/
// 	Description : 检查注销mr模块使用中的 pdp
//	Global resource dependence : 
//  Author: qgp
//	Note: PARAM - WaitFunc, 等待pdp注销后继续处理的函数
//   	  return- TRUE, pdp注销中，需要等待；FALSE，pdp空闲。
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_waitToDeactiveMrappPdp(MMIMRAPP_WAIT2DOPDPACT_FUNC WaitFunc, uint32 data);

/*****************************************************************************/
// 	Description : bl mem Cross proc cb
//	Global resource dependence : 
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_BLMemCb(BLOCK_MEM_ID_E block_id, BLOCK_MEM_RELATION_STATUS_E relation_info);

PUBLIC void MMIMRAPP_getApnAccountRet(int32 report);

#ifdef MRAPP_CLOCK_OPEN
/*****************************************************************************/
//  Description : netclock timer callback fuction
//  Global resource dependence : 
//  Author: zzh
//  Note:
/*****************************************************************************/
void MMIMRAPP_StartNetClock(void);
#endif

#ifdef __MR_CFG_SHORTCUT_SUPPORT__
void mr_scut_start_app(void* param1, void *param2);
#endif

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
 
#ifdef   __cplusplus
   }
#endif

#endif/*_MMIMRAPP_H_*/
