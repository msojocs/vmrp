#ifndef __MRP_FEATURES_H__
#define __MRP_FEATURES_H__

#ifdef __MMI_DSM_NEW__



/*

在原来的系统中，系统的宏比较分散，而且命名也没有按照统一的标准。
鉴于此，在新版本中，我们将所有系统的宏都统一到mrp_features.h这个文件中，便于管理和修改。


在我们的软件中，宏基本上分为这样的三类，一是功能相关的开关，二是一些系统的宏变量，三就是入口的开关。
对应的我们将文件分为三个部分，并对各类宏的命名做了规范，在日后的修改和维护中，请务必遵循此约定。

系统宏命名约定:
1. 功能开关以 __MR_CFG_FEATURE_***__ 形式命名
2. 系统变量以 __MR_CFG_VAR_****__形式命名
3. 入口开关以 __MR_CFG_ENTRY_***__形式命名
4. 名字最好以英文单词，不要使用拼音等其他方式

*/


#ifndef __CFG_ON__
#define __CFG_ON__		1
#define __CFG_OFF__		0
#endif


/////////////////////////////////////////////////////////////////////////////////
//1系统功能相关宏开关
/////////////////////////////////////////////////////////////////////////////////

/*
  Description: 打开系统调试模式
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_FEATURE_DEBUG_SWITCH__			(__CFG_ON__)
#if (__MR_CFG_FEATURE_DEBUG_SWITCH__ == __CFG_ON__)
	#define __MR_CFG_FEATURE_DEBUG__
#endif


/*
  Description: 是否为海外移植包
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_FEATURE_OVERSEA_SWITCH__		(__CFG_OFF__)
#if (__MR_CFG_FEATURE_OVERSEA_SWITCH__ == __CFG_ON__)
	#define __MR_CFG_FEATURE_OVERSEA__
#endif


/*
  Description: 是否使用多媒体模块的内存
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#ifdef MMI_ON_HARDWARE_P
#define __MR_CFG_FEATURE_USE_MED_BUFFER_SWITCH__		(__CFG_ON__)
#else
#define __MR_CFG_FEATURE_USE_MED_BUFFER_SWITCH__		(__CFG_OFF__)
#endif
#if (__MR_CFG_FEATURE_USE_MED_BUFFER_SWITCH__ == __CFG_ON__)
	#define DSM_USE_MED_BUFFER 
#endif


/*
  Description: 是否隐藏我们的目录
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_FEATURE_HIDE_FOLDER_SWITCH__	(__CFG_OFF__)
#if (__MR_CFG_FEATURE_HIDE_FOLDER_SWITCH__ == __CFG_ON__)
	#define __MMI_DSM_HIDE_FOLDER__   /*如果想隐藏MYTHROAD 目录请打开这个宏*/
#endif


/*
  Description: 系统WAP浏览器的类型，根据实际项目情况打开
  Option: [__TELECA__, __JATAYU__, __POLLEX__]
*/
#define __TELECA__	0
#define __JATAYU__	1
#define __POLLEX__	2

#if defined(OBIGO_Q03C) || defined(__Q03C__) || defined(__JATAAYU4__)
#define __MR_CFG_FEATURE_WAP_SWITCH__		(__JATAYU__)
#else
#define __MR_CFG_FEATURE_WAP_SWITCH__		(__TELECA__)
#endif

#if (__MR_CFG_FEATURE_WAP_SWITCH__ == __TELECA__)
	#define DSM_TELECA_WAP
#elif (__MR_CFG_FEATURE_WAP_SWITCH__ == __POLLEX__)
	#define DSM_POLLEX_WAP
#elif (__MR_CFG_FEATURE_WAP_SWITCH__ == __JATAYU__)
	#define DSM_JATAYU_WAP
#else
	#error unspoorted wap browser
#endif


/*
  Description: 统一网络宏开关
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_FEATURE_UNET_SWITCH__		(__CFG_ON__)
#if (__MR_CFG_FEATURE_UNET_SWITCH__ == __CFG_ON__)
	#define __DSM_USE_COMMON_NET_MODE__
#endif


/*
  Description: 系统是否支持WIFI
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#ifdef __CUSTOM_WIFI_FEATURES_SWITCH__//__WIFI_SUPPORT__//huangsunbo 20110725
#define __MR_CFG_FEATURE_WIFI_SWITCH__		(__CFG_ON__) 
#endif
#if (__MR_CFG_FEATURE_WIFI_SWITCH__ == __CFG_ON__)
	#define __MMI_DSM_WIFI_SUPPORT__  
#endif


/*
  Description: 系统是否支持DRM
*/
#if defined (__DRM_SUPPORT__)
#define __MR_CFG_FEATURE_DRM__
#endif


/*
  Description: 系统是否没有数字键或者没有OK键，或者是全键盘
  Option: [__CFG_ON__, __CFG_OFF__]
*/
//#define __MR_CFG_FEATURE_NO_NUM_KEY__	/*没有数字键*/	
//#define __MR_CFG_FEATURE_NO_OK_KEY__	/*没有OK键*/	
#ifdef __MMI_REDUCED_KEYPAD__
#define __MR_CFG_FEATURE_NO_NUM_KEY__ /*没有数字键*/ 
#define __MR_CFG_FEATURE_NO_OK_KEY__ /*没有OK键*/ 
#endif

#ifdef __MMI_QWERTY_KEYPAD_SUPPORT__
#define __MR_CFG_FEATURE_QWERTY_KEYPAD__ /*是否全键盘*/
#endif


/*
  Description: 动感芯片支持类型
  Option: [__NONE__, __ADI__, __BOSCH__, __MTK_FRAME__]
*/
#define __NONE__		0
#define __ADI__			1
#define __BOSCH__		2
#define __MTK_FRAME__	3

#define __MR_CFG_FEATURE_MOTION_SWITCH__		(__NONE__)
#if (__MR_CFG_FEATURE_MOTION_SWITCH__ != __NONE__)
	#define __MMI_DSM_MOTION_SUPPORT__
	#if (__MR_CFG_FEATURE_MOTION_SWITCH__ == __ADI__)
		#define __DSM_ADI_MOTION__
	#elif (__MR_CFG_FEATURE_MOTION_SWITCH__ == __BOSCH__)
		#define __DSM_BOSCH_MOTION__
	#elif (__MR_CFG_FEATURE_MOTION_SWITCH__ == __MTK_FRAME__)
		#define __DSM_MOTION_MTK_FRAME__
	#endif
#endif




#define __MOPOZS__		1 //曲奇助手
#define __PLATCODE__	2 //移植层代码

/*
  Description: 是否支持不固化曲奇，通过挂机下载程序来下载曲奇
  Option: [__NONE__, __MOPOZS__, __PLATCODE__]
  __MOPOZS__: 通过曲奇助手下载，ROM 空间在15K左右，但体验较好
  __PLATCODE__: 通过移植层代码下载，ROM空间在5K左右，但体验较差
*/
#ifdef __MR_CFG_FEATURE_SLIM__
#define __MR_CFG_FEATURE_COOKIE_DOWNLOAD_SWITCH__	(__MOPOZS__) 
#else
#define __MR_CFG_FEATURE_COOKIE_DOWNLOAD_SWITCH__	(__NONE__) 
#endif

#ifndef __MR_CFG_FEATURE_OVERSEA__
#if (__MR_CFG_FEATURE_COOKIE_DOWNLOAD_SWITCH__ == __MOPOZS__)
	#define __MR_CFG_FEATURE_COOKIE_DOWNLOAD_MOPOZS__  
#elif (__MR_CFG_FEATURE_COOKIE_DOWNLOAD_SWITCH__ == __PLATCODE__)
	//下载曲奇
	#define    __MR_CFG_FEATURE_COOKIE_DOWNLOAD_PLATCODE__
#endif
#endif


/*
  Description: 是否支持IDLE WIDGET
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_FEATURE_ONLINE_MGR_SWITCH__		(__CFG_OFF__) 
#if (__MR_CFG_FEATURE_ONLINE_MGR_SWITCH__ == __CFG_ON__)
	#define __MR_CFG_FEATURE_ONLINE_MGR__  
#endif


/*
  Description: 是否支持桌面应用管理接口
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_FEATURE_DAM_SWITCH		(__CFG_OFF__) 
#if defined(__MR_CFG_ENTRY_APPSTORE__)//(__MR_CFG_FEATURE_DAM_SWITCH == __CFG_ON__)
	#define __MR_CFG_FEATURE_DAM__
#endif


/*
  Description: 是否支持扩展机型字段
  Option: [__CFG_ON__, __CFG_OFF__]
*/
//#define __MR_CFG_FEATURE_HANDSETID_EX_SWITCH__		(__CFG_ON__) 
//#if (__MR_CFG_FEATURE_HANDSETID_EX_SWITCH__ == __CFG_ON__)
#ifndef __MR_CFG_FEATURE_OVERSEA__
	#define __MR_CFG_FEATURE_HANDSETID_EX__
#endif	
//#endif

/*
  Description: 是否支持曲奇助手下载GB转换表
*/
#if (__MR_CFG_FEATURE_COOKIE_DOWNLOAD_SWITCH__ != __NONE__) && !defined(__MMI_CHSET_GB2312__) && !defined(__MR_CFG_FEATURE_OVERSEA__)
	#define __MR_CFG_FEATURE_MOPOZS_DOWNLOAD_GBTABLE__
#endif


/*
  Description:  this feature added by david.qiu for __MMI_DSM_NEW_JSKY__ auto zoom
  such as, the game is 240*320, but the lcd is 320*480, if you have this feature, you 
  can see good effect at the whole screen with resizer, if the hardware don't support
  hw resizer, please close the macro
*/
#define __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT_SWITCH__ (__CFG_OFF__)
#if (__MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT_SWITCH__ == __CFG_ON__)
	#define __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
#endif

/*
  Description: 支持设短信状态(收发件箱)
*/
#define  __MR_CFG_SET_SMS_STATUS__

/*
  Description: 海外固化CDSM，默认使用DSMGAME,需要使用mrpstore,请FAE自行打开宏__DSM_MRPSTORE__
*/
#if defined( __MR_CFG_FEATURE_OVERSEA__)  
	#ifndef __MR_CFG_FEATURE_SLIM__
	#define __DSM_MRPSTORE__
	#else
	#define __DSM_DSMGAME__
	#endif
	//#define __MRP_CDSM_INROM__
	#if 1//def __MR_CFG_ENTRY_SKYBUDDY__
	#define __DSM_SKYBUDDY__
	#endif
#endif





/////////////////////////////////////////////////////////////////////////////////
//1系统的宏变量
/////////////////////////////////////////////////////////////////////////////////


/*
  Description: MTK 软件版本号，16进制表示，不要忘了前面的0x。
  	由于存在09A0952 和 09B0952 如果按前期的命名规则将导致无法区分这两个版本号。
  	
  	做如下规定:
  	1.09A 之前的全部版本号仍然使用4位表示的版本号比如0x0952 将表示09A0952(因为之前的版本号太多，有些不清楚是属于08A还是08B)
  	2.09B 之后的版本全部使用带主版本号的表示方式，如0x09B0952, 0x09B1012后续的10A同样使用这样的方式如，0x10A1028
*/
#define __MR_CFG_VAR_MTK_VERSION__					(0x11B1224)//0x11B1212,0x11B1144,0x11B1212,0x11B1224
#define MTK_VERSION		__MR_CFG_VAR_MTK_VERSION__                                       

#if (MTK_VERSION >= 0x10A1032)
#define MMI_SYSTEM_DRV SRV_FMGR_SYSTEM_DRV
#define MMI_PUBLIC_DRV SRV_FMGR_PUBLIC_DRV
#define MMI_CARD_DRV SRV_FMGR_CARD_DRV
#endif

/*
  Description: 分配给虚拟机内存值大小，单位为字节,系统最小内存为650K，视不同情况配置

  35 36 等高端机建议内存分配在2M以上，其他情况下根据屏幕分辨率情况及实际手机内存剩余情况下尽量往高配置
*/
#if defined(__ARM9_MMU__) || defined(__ARM11_MMU__)//(defined(MT6235)||defined(MT6235B)) huangsunbo 20110407
	#define __MR_CFG_VAR_VMMEM_SIZE__ (4*1024*1024)
#else
#if defined(__MMI_MAINLCD_240X432__)
	#define __MR_CFG_VAR_VMMEM_SIZE__ (1024*1024)
#elif defined(__MMI_MAINLCD_320X480__) 
	#define __MR_CFG_VAR_VMMEM_SIZE__ (2*1024*1024)
#elif defined(__MMI_MAINLCD_240X400__)  
	#define __MR_CFG_VAR_VMMEM_SIZE__ (1024*1024)
#else
	#define __MR_CFG_VAR_VMMEM_SIZE__ (750*1024)//含240x320常规分辨率，多多益善
#endif
#endif

/*
 * 要支持JSKY必须要5M以上的内存
 */
#ifdef __MMI_DSM_NEW_JSKY__
#undef __MR_CFG_VAR_VMMEM_SIZE__
#define __MR_CFG_VAR_VMMEM_SIZE__ (5*1024*1024)
#endif

/*
  Description: 厂商标识，最多七个小写字符
*/
#define __MR_CFG_VAR_FACTORYID__						"sky"


/*
  Description: 手机标识，最多七个小写字符
*/

#if defined(__MMI_MAINLCD_240X320__)
#define __MR_CFG_VAR_HANDSETID__                                  "m900"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_176X220__)
#define __MR_CFG_VAR_HANDSETID__                                  "qcif"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_128X160__)
#define __MR_CFG_VAR_HANDSETID__                                  "1216"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_240X400__)
#define __MR_CFG_VAR_HANDSETID__                                  "2440"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_320X480__)
#define __MR_CFG_VAR_HANDSETID__                                  "3248"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_480X800__)
#define __MR_CFG_VAR_HANDSETID__                                  "4880"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_320X240__)
#define __MR_CFG_VAR_HANDSETID__                                  "3224"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_220X176__)
#define __MR_CFG_VAR_HANDSETID__                                  "qcifw"				 /*手机标识，最多七个小写字符*/
#elif defined(__MMI_MAINLCD_160X128__)
#define __MR_CFG_VAR_HANDSETID__                                  "qqcw"				 /*手机标识，最多七个小写字符*/
#endif


/*
  Description: 手机扩展机型ID，这个字段会和机型字段连接，连接字符串的长度小于34个
  	比如我们分配的是m900扩展机型是handset1那么返回给应用的将是m900-handset1

  Note: 要支持此功能先启用__MR_CFG_FEATURE_HANDSETID_EX__宏
*/
#define __MR_CFG_VAR_HANDSETID_EX__					"handset1"  // length < 26
#define __MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__	34 // 8 + 26

/*
  Description: 手机平台区分(1~99), 由平台组统一分配
*/
#if (defined(MT6223P) || defined(MT6223) || defined(MT6223P_S00))
	#define __MR_CFG_VAR_PLAT_VER__						(2) 
#elif (defined(MT6226) || defined(MT6226M) || defined(MT6226D))
	#define __MR_CFG_VAR_PLAT_VER__						(4)  
#elif (defined(MT6228))
	#define __MR_CFG_VAR_PLAT_VER__						(5) 
#elif (defined(MT6225))
	#define __MR_CFG_VAR_PLAT_VER__						(3) 
#elif (defined(MT6230))
	#define __MR_CFG_VAR_PLAT_VER__						(6)  
#elif (defined(MT6227) || defined(MT6227D))
	#define __MR_CFG_VAR_PLAT_VER__						(7)
#elif (defined(MT6219))
	#define __MR_CFG_VAR_PLAT_VER__						(1)
#elif(defined(MT6235) || defined(MT6235B))
	#define __MR_CFG_VAR_PLAT_VER__						(8)
#elif(defined(MT6229))
	#define __MR_CFG_VAR_PLAT_VER__						(9)
#elif(defined(MT6253) || defined(MT6253T))
	#define __MR_CFG_VAR_PLAT_VER__						(10)
#elif(defined(MT6238))
	#define __MR_CFG_VAR_PLAT_VER__						(11)
#elif(defined(MT6239))
	#define __MR_CFG_VAR_PLAT_VER__						(12)
#elif(defined(MT6268))
	#define __MR_CFG_VAR_PLAT_VER__						(13)
#elif(defined(MT6236))
	#define __MR_CFG_VAR_PLAT_VER__						(14)
#elif (defined(MT6252) || defined(MT6252H))	
	#define __MR_CFG_VAR_PLAT_VER__						(15)
#elif (defined(MT6276))	
	#define __MR_CFG_VAR_PLAT_VER__						(16)	
#elif (defined(MT6255)) 
	#define __MR_CFG_VAR_PLAT_VER__ 					(17) 
#elif (defined(MT6256)) 
	#define __MR_CFG_VAR_PLAT_VER__ 					(18)
#elif (defined(MT6250) || defined(MT6250D))	
	#define __MR_CFG_VAR_PLAT_VER__ 					(19)
#else
	#error please call sky to add the platform.
#endif


/*
  Description: 由平台组统一分配版本号，有需求请联系平台组
*/
#ifndef __MR_CFG_FEATURE_SLIM__
#define __MR_CFG_VAR_FAE_VER__                                       (204)
#else
/* 如果是瘦接口的话，版本号需要降到190或以下，看具体裁剪的接口*/
#define __MR_CFG_VAR_FAE_VER__                                       (90)
#endif


/*
  Description: 这里的ROOT目录是只存放系统信息的目录，而非指启动目录(不知道当初为啥这么命名)
*/
#define __MR_CFG_VAR_ROOT_DRV__                                       MMI_SYSTEM_DRV
#define __MR_CFG_VAR_ROOT_PATH__                                     "mythroad"


/*
  Description: 定制的启动盘符及根目录，一般情况下不需要做修改
*/
#define __MR_CFG_VAR_CUSTOM_DRV__                                    MMI_CARD_DRV
#define __MR_CFG_VAR_CUSTOM_PATH__                                  "mythroad"


/*
  Description: 字体宽高信息
*/
#define __MR_CFG_VAR_CHWORD_H__							(20) /* 设置字体宽度 */
#define __MR_CFG_VAR_CHWORD_W__							(20) /*设置字体高度  */                          
#define __MR_CFG_VAR_ENWORD_H__							( __MR_CFG_VAR_CHWORD_H__)
#define __MR_CFG_VAR_ENWORD_W__							((__MR_CFG_VAR_CHWORD_W__)>>1)


/*
  Description: 自动消失的对话框显示时间
*/
#define __MR_CFG_VAR_NOTIFY_DURATION__					2000
#define __MR_CFG_VAR_SHORT_NOTIFY_DURATION__			ST_NOTIFYDURATION


/*
huangsunbo 20110321 除了128x160,160x128和128x128三个屏幕以外，其他屏幕都使用新版本曲奇
使用宏__MMI_DSM_COOKIE_NEW__来区分新旧版本曲奇
*/
#ifndef __MR_CFG_FEATURE_OVERSEA__
#define __MMI_DSM_COOKIE__
#if (LCD_WIDTH >= 240)
#define __MMI_DSM_COOKIE_NEW__
#elif (LCD_WIDTH == 176) || (LCD_WIDTH == 220)
#define __MMI_DSM_COOKIE_NEW__
#endif
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////
//1入口相关的宏开关
/////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __MR_CFG_FEATURE_OVERSEA__ 
/*********************************************************************************
 * 国内固化的应用列表信息
 *********************************************************************************/

//#define __MR_CFG_ENTRY_GAMECENTER__	//游戏，收益好，建议打开		
//#define __MR_CFG_ENTRY_NETGAME__		//网游，采用第三方支持，收益好，强烈建议打开
//#define __MR_CFG_ENTRY_QQ__			//手机QQ
//#define __MR_CFG_ENTRY_EBOOK__		//电子书
//#define __MR_CFG_ENTRY_MSN__			//MSN
//#define __MR_CFG_ENTRY_FETION__		//飞信3.0，支持群
//#define __MR_CFG_ENTRY_KAIXIN__		//开心网，官方授权
//#define __MR_CFG_ENTRY_TIANQI__		//天气助手，支持七天预报
//#define __MR_CFG_ENTRY_MPLIVE__		//手机电视 // SKY_MULTIMEDIA
//#define __MR_CFG_ENTRY_MPCHAT__		//视频聊天 // SKY_MULTIMEDIA

/*
  Description: NES 游戏支持宏
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_ENTRY_NES_SWITCH__			(__CFG_OFF__)
#ifdef __MR_CFG_ENTRY_NES__
	//#define __MMI_DSM_NES_APP__ 			/* 红白机模拟器*/
	#ifndef __ARM9_MMU__
	#define __MMI_DSM_MEM_NES_SUPPORT__
	#endif
	//#define __MMI_DSM_NES_APP_TCARD__
	#if !defined(MT6235) && !defined(MT6235B) && defined(MMI_ON_HARDWARE_P) && defined(__MMI_DSM_MEM_NES_SUPPORT__)
		//#define __MMI_DSM_JOYPAD__			/*NES模拟器支持手柄功能开关*/
	#endif
#endif


/*
  Description: 天气预报宏开关
  Option: [__CFG_ON__, __CFG_OFF__]
*/
#define __MR_CFG_ENTRY_WEATHER_SWITCH__	(__CFG_OFF__)
#if (__MR_CFG_ENTRY_WEATHER_SWITCH__ == __CFG_ON__)
	#define __MMI_DSM_WEATHER_APP__			/*天气预报应用*/
	#define __MMI_DSM_IDLE_WEATHER__ 		/*桌面天气功能开关*/
	#define DESKTOP_AUTO_UPDATE_OPEN 		/*天气预报自动更新功能开关*/
#endif


#else 
/*********************************************************************************
 * 海外固化的应用列表信息
 *********************************************************************************/

//#define __MR_CFG_ENTRY_MRPSTORE__		//外单游戏
//#define __MR_CFG_ENTRY_MSN__				//MSN
//#define __MR_CFG_ENTRY_YAHOO__			//yahoo
//#define __MR_CFG_ENTRY_FACEBOOK__		//facebook
//#define __MR_CFG_ENTRY_SKYPE__			//skype
//#define __MR_CFG_ENTRY_TWITTER__			//twitter

//#define __MR_CFG_MSN_INROM__				//MSN 固化开关
//#define __MR_CFG_YAHOO_INROM__			//YAHOO 固化开关
//#define __MR_CFG_FACEBOOK_INROM__		//FACEBOOK 固化开关
//#define __MR_CFG_SKYPE_INROM__			//SKYPE 固化开关
//#define __MR_CFG_TWITTER_INROM__			//TWITTER 固化开关

#endif //__MR_CFG_FEATURE_OVERSEA__

#endif
#endif

