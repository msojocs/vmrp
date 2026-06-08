#ifdef  __MMI_DSM_NEW__


#include "mrp_extension.h"
#include "mrp_features.h"


/**
 *\brief platEx 相关函数声明。
 *
 * MR_PLATEX_FUNC_PROTOTYPE 宏可以省去你写一堆的参数，但前提是你的函数命名遵循mr_platEx_***这样的规则。
 */
MR_PLATEX_FUNC_PROTOTYPE(sys);
MR_PLATEX_FUNC_PROTOTYPE(drm);
MR_PLATEX_FUNC_PROTOTYPE(streaming_player);
MR_PLATEX_FUNC_PROTOTYPE(mpchat);
MR_PLATEX_FUNC_PROTOTYPE(dove);
MR_PLATEX_FUNC_PROTOTYPE(msn);
MR_PLATEX_FUNC_PROTOTYPE(mulang);
MR_PLATEX_FUNC_PROTOTYPE(jsky);
MR_PLATEX_FUNC_PROTOTYPE(wap);
MR_PLATEX_FUNC_PROTOTYPE(setting);
MR_PLATEX_FUNC_PROTOTYPE(mem);
MR_PLATEX_FUNC_PROTOTYPE(dam);

#ifdef __PME_SUPPORT__ 
MR_PLATEX_FUNC_PROTOTYPE(ovs);
#endif

/**
 *\brief 将platEx相关函数加在其GROUP值对应的位置，如果未实现则设置成NULL。
 */
const mr_platEx_f g_platex_func_table[MR_PLATEX_GROUP_MAX] = {
#ifndef __MR_CFG_FEATURE_SLIM__	

	MR_PLATEX_FUNC_NAME(sys),
#ifndef __MR_CFG_FEATURE_DRM__	
	0,
#else	
	MR_PLATEX_FUNC_NAME(drm),
#endif
// SKY_MULTIMEDIA 
	0, //MR_PLATEX_FUNC_NAME(streaming_player),
	MR_PLATEX_FUNC_NAME(mpchat),
	0,//MR_PLATEX_FUNC_NAME(dove),
	0, //MR_PLATEX_FUNC_NAME(msn),
	0, //MR_PLATEX_FUNC_NAME(mulang),
#ifdef __MMI_DSM_NEW_JSKY__
	MR_PLATEX_FUNC_NAME(jsky),
#else
	0, //MR_PLATEX_FUNC_NAME(jsky),
#endif
	0, //MR_PLATEX_FUNC_NAME(wap),
	MR_PLATEX_FUNC_NAME(setting),
	MR_PLATEX_FUNC_NAME(mem),
#ifndef __MR_CFG_FEATURE_DAM__	
	0,
#else	
	MR_PLATEX_FUNC_NAME(dam),	
#endif	

#else	// __MR_CFG_FEATURE_SLIM__

	MR_PLATEX_FUNC_NAME(sys),
	0, 
	0, 
// SKY_MULTIMEDIA // 手机电视3K版本
	MR_PLATEX_FUNC_NAME(mpchat),

#endif	// __MR_CFG_FEATURE_SLIM__

#ifdef __PME_SUPPORT__
	MR_PLATEX_FUNC_NAME(ovs),//海外自扩展
#endif	//+ __PME_SUPPORT__ end

};


/**
 *\brief g_platex_func_table 中函数的个数，一般情况下不需要做修改。 
 */
const int32 g_platex_func_table_entries = MR_PLATEX_GROUP_MAX;


/**
 * \brief SKYQQ 的移植层接口
 */
#if defined(__MMI_SKYQQ__) && !defined(WIN32) 
extern int32 SDKCallMTK(uint8* input, int32 input_len, uint8** output, int32* output_len);
int32 mr_qq_ext_api(uint8* input,int32 input_len,uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return SDKCallMTK(input, input_len,output,output_len);
	
}
#else
int32 mr_qq_ext_api(uint8* input,int32 input_len,uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return 0 ;
}
#endif




#ifndef __SKY_SECURITY_GUARD__
int32 mr_safe_dealcommand(uint8* input,int32 input_len,uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_IGNORE;
}
#else
extern int32 safe_sdk_mtk(uint8* input,int32 input_len,uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_safe_dealcommand(uint8* input,int32 input_len,uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return safe_sdk_mtk(input, input_len, output, output_len, cb);
}
#endif

// SKY_MULTIMEDIA  // move mr_platEx_mpchat() to mrp_mpchat.c


#endif
