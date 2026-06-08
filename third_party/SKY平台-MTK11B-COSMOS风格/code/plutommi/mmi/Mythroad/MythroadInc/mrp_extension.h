#ifndef __MRP_PAL_EXTENSION_H__
#define __MRP_PAL_EXTENSION_H__


/**
 * 如何增加一个扩展的接口:
 * 	- 在枚举 MR_PLATEX_GROUP_E 的MR_PLATEX_GROUP_MAX之前增加一个GROUP项，如MR_PLATEX_GROUP_MSN。
 *	- 用MR_PLATEX_FUNC_BEGIN、MR_PLATEX_FUNC_END相关宏实现相关函数,比如mr_platEx_msn。
 *	- 在g_platex_func_table 中于GROUP枚举值对应的位置添加函数映射,就是将mr_platEx_msn函数加在MR_PLATEX_GROUP_MSN作为index的表项。
 */


#include "mrporting.h"


/**
 * \brief platEx code相关的宏。
 *	
 *	mr_platEx 的第一个参数定义为int32,也就是一个32位的值，
 *		为了便于管理，我们这里将这个32位的code分成两个部分,高16位表示GROUP,低16位表示功能，
 *		这样code就有了一个2维的空间，每个GROUP都表示一族类似功能的接口，可以在不影响其他GROUP的情况下进行扩展。
 *		由于原本mr_platEx实现的code值都小于0xffff,这里就把原来实现的相关code归到一个系统的GROUP里面(MR_PLATEX_GROUP_SYS)，
 *		也就是说这里的mr_platEx_sys就相当于原来的mr_platEx。
 *
 * \note 这里我们约定在新增加了一个group后，这个group的第一个功能号表示查询，这样在应用层可以通过这个功能号来获取
 *		这个GROUP相关的功能是否实现。MR_SUCCESS表示实现，MR_IGNORE/MR_FAIELD表示不支持。
 */
#define MR_PLATEX_GROUP_MASK				0xffff0000
#define MR_PLATEX_FUNC_MASK				0x0000ffff
#define MR_PLATEX_GROUP(code)				((code & MR_PLATEX_GROUP_MASK) >> 16)	//get group from code
#define MR_PLATEX_FUNC(code)				(code & MR_PLATEX_FUNC_MASK )	//get function from code
#define MR_PLATEX_MAKECODE(group, func)	((group << 16) | func)	//make a code from group and func


/**
 * \brief 用于实现platEx族函数相关的宏。
 *
 * 使用方式如下:
 *
 * \code
 *
 *	MR_PLATEX_FUNC_BEGIN(msn)
 *		MR_PLATEX_FUNC_ENTRY(code1, func1) //这里的code1 值不要带GROUP信息
 *		MR_PLATEX_FUNC_ENTRY(code2, func2)
 *	MR_PLATEX_FUNC_END()
 *
 * \endcode
 *
 * 上面这段代码就相当于产生了一个mr_platEx_msn函数，并将code1,code2分发给函数func1和func2
 *
 * \note MR_PLATEX_FUNC_ENTRY 在使用的时候，code值不需要带GROUP信息，所以每个模块自己定义一个emun的功能枚举就行
 */
#define MR_PLATEX_FUNC_NAME(name)			mr_platEx_##name
#define MR_PLATEX_FUNC_PROTOTYPE(name)		int32 MR_PLATEX_FUNC_NAME(name)(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
#define MR_PLATEX_FUNC_BEGIN(name)			MR_PLATEX_FUNC_PROTOTYPE(name){ switch(MR_PLATEX_FUNC(code)) {
#define MR_PLATEX_FUNC_END()					} return MR_IGNORE;}
#define MR_PLATEX_FUNC_ENTRY(code, func)		case code: return func(input, input_len, output, output_len, cb);
#define MR_PLATEX_FUNC_ENTRY_SUPPORT(code, r)	case code: return r;


/**
 * \brief mr_platEx 族函数原型，用于平台接口扩展
 *
 * \param code 			[in] 函数调用CODE，这个CODE值由平台组统一分配
 * \param input			[in] 输入参数，有可能是数值，也有可能是数据结构，由CODE实现定义
 * \param input_len		[in] 输入参数大小(BYTE)
 * \param output		[out] 输出参数
 * \param output_len	[out] 输出参数的大小(BYTE)
 * \param cb			[out] 平台返回的回调函数，用于以下情况
 *	- NULL ，当函数返回NULL时忽略
 *	- 非NULL, 表示在平台执行功能的时候分配了一些资源，比如ouput相关的内存等，当应用使用完毕后需要调用这个cb来释放相关资源
 *
 * \return 返回函数执行结果
 *	- MR_SUCCESS, 函数执行成功
 *	- MR_FAILED，函数执行失败
 *	- MR_IGNORE，功能未定义
 *	- MR_WAITING，异步操作，可能需要等待后续事件或需主动查询
 */
typedef int32 (*mr_platEx_f)(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief PLATEX GROUP 的枚举值,如果需要增加新的接口，请联系平台组。
 */
typedef enum
{
	/* SYSTEM GROUP , 老的mr_platEx实现的code 现在都属于这个GROUP */
	MR_PLATEX_GROUP_SYS = 0,

	/* DRM 相关接口*/
	MR_PLATEX_GROUP_DRM,

	/* 带CODEC 库的流媒体播放器接口*/
	MR_PLATEX_GROUP_MPLAYER,

	/* 带CODEC 库的互动视频接口*/
	MR_PLATEX_GROUP_MPCHAT,

	/* 短信相关接口*/
	MR_PLATEX_GROUP_DOVE,

	/* MSN 定制预留的接口*/
	MR_PLATEX_GROUP_MSN,

	/* 多国语言接口 */
	MR_PLATEX_GROUP_MULANG,

	/* 启动 J2ME 游戏等接口, 目前只有SYMBIAN上实现*/
	MR_PLATEX_GROUP_J2ME , //7

	/* 本地浏览器相关接口，目前只有SYMBIAN上实现*/
	MR_PLATEX_GROUP_WAP,

	/* 用于扩展系统的设置接口 */
	MR_PLATEX_GROUP_SETTING, 

	/* 其他的内存管理函数 */
	MR_PLATEX_GROUP_MEM,

	/*桌面应用管理器*/
	MR_PLATEX_GROUP_DAM,

#ifdef  __PME_SUPPORT__
	/*海外宽展接口*/
	MR_PLATEX_GROUP_OVS, //0xc0000 - 0xcffff
#endif   //__PME_SUPPORT__ end

	/* ADD GROUP BEFORE MAX*/
	MR_PLATEX_GROUP_MAX
}MR_PLATEX_GROUP_E;


/**
 * \brief PLATEX 函数映射表
 */
extern const mr_platEx_f g_platex_func_table[];


/**
 * \brief PLATEX 函数映射表项数
 */
extern const int32 g_platex_func_table_entries;

#endif

