#ifndef __MRP_MAIN_H__
#define __MRP_MAIN_H__



/**

在原有的移植包中，所有的启动入口都是放在mmidsmmain.c文件中，每个入口都有自己相关的系列函数。
比如DsmGame表示曲奇入口，那么将会有相关的函数菜单的初始化代码及菜单的hilite函数等等。
其实你会发现，曲奇的启动函数和凯阅的启动函数最主要的区别在于mr_registerApp的入口信息不同。
基于这样一个事实，我们将通用的代码提取出来，将唯一不同的入口信息提取出来放在单独的表格中。


这样的代码设计能带来以下好处: 
	1，代码减少，不需要单独的启动函数等等，这样也能保证你写的每个入口的行为的一致性。
	2，添加入口简单，如果你要添加一个启动入口，只有在信息表中添加对应的入口信息就可以，不需要自己在写函数了。


下面就示列如何添加一个新入口:
	1，添加入口的宏开关
	2，添加菜单及相关资源
	3，添加启动信息


一，添加入口的宏开关
	1，在mrp_features.h文件的入口相关宏开关部分，添加一个对应的入口开关，比如__MR_CFG_ENTRY_MSN__.
	

二，添加菜单等相关资源
	1, 添加资源MENU的ID。在mrp_app_menu_def.h里面添加一个菜单ID。
	2，在mmidsmdef.h中定义菜单项要使用的字符串ID，图片ID等。
	3, 在Res_DSM.c的PopulateMythroadRes函数中调用MR_ADD_APPLICATION_MENUITEM宏函数将菜单项添加到指定的主菜单下。
		在这个宏函数中，你就可以指定对应菜单的资源，比如字符串ID，IMAGE ID及路径等信息。这个添加项必须用入口宏开关包括起来。


三，添加启动信息
	1，添加APPID信息。在mrp_main.h文件的MR_APPID_E中添加一个对应MRP的APPID,如MR_APPID_MSN = 203102。
	2，在mrp_main.c中，将固化的.h文件包含进来，注意用入口宏开关来包括起来。
	3，添加入口信息内容。在mrp_main.c中找到MR_PREINSTALLED_APPINFO_TABLE_BEGIN固化应用信息列表。
		请确认好国内还是海外，并用入口宏开关括起来。

		示列代码如下
		MR_APPINFO_BEGIN(MENU_DSM_GAME_CENTER, MR_APPID_COOKIE, "*J,games") //参数分别为，菜单ID, APPID, 启动参数
			MR_APPINFO_ENTRY(mr_all_list,  9) //固化的入口位置及数组
			MR_APPINFO_ENTRY(mr_alllist_cfg, 18) //固化列表的固化位置及数组
		MR_APPINFO_END()


通过上面3个步骤，我们就能添加一个新的入口。当然如果你还没有完全习惯这样的方式，我们还完全支持之前的入口添加方式，
你还可以为每个入口添加单独的启动函数。但我这里强烈建议你不要这样做。

*/



/**
 * \brief 固化应用的APPID
 */
typedef enum {
	MR_APPID_INVALID = 0,
	MR_APPID_COOKIE = 800001,
	MR_APPID_SKYREAD,
	MR_APPID_MRPSTORE,
	MR_APPID_DSMGAME,
	MR_APPID_APPLIST,
	MR_APPID_NET_GAMES = 1007,
	MR_APPID_QQ2007 = 203101,
	MR_APPID_QQ2008 = 203103,
	MR_APPID_SKYQQ = 203106,
	MR_APPID_MSN = 203102,
	MR_APPID_FETION = 6005,
	MR_APPID_KAIXIN = 6020,
	MR_APPID_TIANQI = 2919,
	MR_APPID_MTKQQ2008 = 203104,
	MR_APPID_NETCLOCK = 6008,
	MR_APPID_YAHOO = 810000,
	MR_APPID_FACEBOOK = 810002,
	MR_APPID_HTYXGL = 6022,
	MR_APPID_ATZXCD = 391122,
	MR_APPID_AXHZD = 391014,
	MR_APPID_SKYPE = 391022,
	MR_APPID_TWITTER = 810006,
	MR_APPID_FACETALK = 810017,
#ifdef __MMI_NGUAQ__
	MR_APPID_NGUAQ = 203114,
#endif
#ifndef __MR_CFG_FEATURE_OVERSEA__	
	MR_APPID_TOMCAT = 2924,
#else
	MR_APPID_TOMCAT = 814459,
#endif
#ifndef __MR_CFG_FEATURE_OVERSEA__	
	MR_APPID_FRUIT = 2937,
#else
	MR_APPID_FRUIT = 814042,
#endif
	MR_APPID_JS = 332005,
#ifndef __MR_CFG_FEATURE_OVERSEA__	
	MR_APPID_ANGRYBIRD = 1016,
#else
	MR_APPID_ANGRYBIRD = 814041,
#endif
	MR_APPID_SKIRT = 1020,
	MR_APPID_WEIBO = 391049,
	MR_APPID_MPLIVE = 2917,
	MR_APPID_MPCHAT = 2918,
	MR_APPID_QQBROWSER = 391033,
	#ifdef __HC_CALL_NEW__
	MR_APPID_LDZS = 391105,
	#endif
	#ifdef __MR_CFG_ENTRY_NES__
	MR_APPID_NES = 813105,
	#endif
	MR_APPID_GUARD = 2925,
	#ifdef __MR_CFG_ENTRY_BYDR__
	MR_APPID_BYDR = 2933,
	#endif
	MR_APPID_SKYBUDDY = 200004,
	MR_APPID_SHOUXIN = 2934,

} MR_APPID_E;



/**
 * \brief TLV 的类型
 */
typedef enum{
	MR_APPINFO_TLV_TYPE_HEADER_DSMENTRY = 0x1,	//从启动参数启动
	MR_APPINFO_TLV_TYPE_HEADER_CALLBACK = 0x2,	//从自定义函数启动
	MR_APPINFO_TLV_TYPE_ENTRY = 0x10	//从参数启动时的入口信息
}MR_APPINFO_TLV_TYPE_E;


/**
 * \brief APPINFO TLV 头信息的成员枚举
 */
typedef enum{
	MR_APPINFO_TLV_HEADER_MENUID = 0,
	MR_APPINFO_TLV_HEADER_APPID,
	MR_APPINFO_TLV_HEADER_ENTRY,
	MR_APPINFO_TLV_HEADER_ITEM_COUNT
}MR_APPINFO_TLV_HEADER_ITEM_E;


/**
 * \brief APPINFO TLV 入口信息的成员枚举
 */
typedef enum{
	MR_APPINFO_TLV_ENTRY_DATA = 0,
	MR_APPINFO_TLV_ENTRY_LEN,
	MR_APPINFO_TLV_ENTRY_INDEX,
	MR_APPINFO_TLV_ENTRY_ITEM_COUNT
}MR_APPINFO_TLV_ENTRY_ITEM_E;


/**
 * \brief 构造预置应用信息的相关宏
 */
#define MR_APPINFO_TLV_MAKE_TL(type, len)				(type << 16 | len)
#define MR_PREINSTALLED_APPINFO_TABLE_BEGIN() 		const uint32 g_mr_preinstalled_appinfo_table[] = { 
#define MR_PREINSTALLED_APPINFO_TABLE_END() 			}
#define MR_APPINFO_BEGIN(menuid, appid, entry) 			MR_APPINFO_TLV_MAKE_TL(MR_APPINFO_TLV_TYPE_HEADER_DSMENTRY, MR_APPINFO_TLV_HEADER_ITEM_COUNT), menuid, appid, (uint32)entry,
#define MR_APPINFO_BEGIN_CB(menuid, appid, f) 			MR_APPINFO_TLV_MAKE_TL(MR_APPINFO_TLV_TYPE_HEADER_CALLBACK, MR_APPINFO_TLV_HEADER_ITEM_COUNT), menuid, appid, (uint32)f,
#define MR_APPINFO_BEGIN2								MR_APPINFO_BEGIN_CB
#define MR_APPINFO_ENTRYEX(data, len, index) 			MR_APPINFO_TLV_MAKE_TL(MR_APPINFO_TLV_TYPE_ENTRY, MR_APPINFO_TLV_ENTRY_ITEM_COUNT), (uint32)data, len, index,
#define MR_APPINFO_ENTRY(arr, index)					MR_APPINFO_ENTRYEX(arr, sizeof(arr), index)
#define MR_APPINFO_END()

/**
 * \brief 简化预置应用信息发问的相关宏
 */
#define MR_APPINFO_TABLE_SIZE()				(sizeof(g_mr_preinstalled_appinfo_table)/sizeof(uint32))
#define MR_APPINFO_TLV_TYPE(tlv)				((g_mr_preinstalled_appinfo_table[tlv] & 0xffff0000) >> 16)
#define MR_APPINFO_TLV_LEN(tlv)					((g_mr_preinstalled_appinfo_table[tlv] & 0xffff) + 1)
#define MR_APPINFO_TLV_VALUE(tlv, item)			(g_mr_preinstalled_appinfo_table[tlv + item + 1])
#define MR_APPINFO_TLV_IS_HEADER(tlv)			(g_mr_preinstalled_appinfo_table[tlv] & 0xf0000)


/**
 * \brief 本地应权限校验用入口信息
 */
typedef struct {
	void (*mr_lapp_license_register_f)(void);
}mr_lapp_license_entry_t;


#define MR_LAPP_LICENSE_MANAGER_BEGIN()	const mr_lapp_license_entry_t g_mr_lapp_license_entries[] = {
#define MR_LAPP_LICENSE_MANAGER_END()	{0}}
#define MR_LAPP_LICENSE_ENTRY(appname, reg)	{reg},


/**
 * \brief 注册入口的最大个数
 */
#define MR_PREINSTALLED_APP_ENTRY_COUNT		20


/**
 * \brief 自定义启动函数原型
 *
 * \return 放回dsmEntry的入口参数，这个返回只为非临时变量
 */
typedef char* (*mr_app_entry_f)(void);


/**
 * \brief 系统初始化函数
 *
 * \return void
 */
void mr_system_intialize(void);


/**
 * \brief "*#777755999#" 显示厂商机型信息
 *
 * \return void
 */
void dsmShowId(void);


/**
 * \brief "*#220807#" 启动mythroad目录下的dsm_gm.mrp文件
 *
 * \return void
 */
void dsm_gm(void);


void TEST_GM(void);
void TEST_SHOW_ID(void);


/**
 * \brief VM启动前的初始化公关，比如设置启动路径等
 *
 * \return 初始化成功返回MR_SUCCESS,失败返回MR_FAILED.
 */
int32 dsmInit(void);


/**
 * \brief 判断VM启动的一些前提条件
 *
 * \return 初始化成功返回MR_SUCCESS,失败返回MR_FAILED.
 */
int dsmGameInit(void);


#endif
