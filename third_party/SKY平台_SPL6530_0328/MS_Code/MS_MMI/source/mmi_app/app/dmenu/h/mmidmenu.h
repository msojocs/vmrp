#ifndef __MMIDMENU_H__
#define __MMIDMENU_H__


#if !(defined MRAPP_SUPPORT)
/*厂商ID*/
#define MRAPP_MANUFACTORY 			"sky"    //厂商号,length limited to 7byte

#if(MR_SCREEN_WIDTH == 240 && MR_SCREEN_HEIGHT == 320)
#define MRAPP_HANDSET 					"240320"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 240 && MR_SCREEN_HEIGHT == 400) //qiuyaobo 20121015
#define MRAPP_HANDSET 					"240400"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 320 && MR_SCREEN_HEIGHT == 480)
#define MRAPP_HANDSET 					"320480"   //手机型号,length limited to 7byte
#elif(MR_SCREEN_WIDTH == 176 && MR_SCREEN_HEIGHT == 220)
#define MRAPP_HANDSET 					"176220"   //手机型号,length limited to 7byte
#else
#error "Unknown screen size, please call SKY-MOBI."
#endif
#endif
//#define DSM_DEBUG_MODE                 
//#define   __DSM_MENU_FUNC_TEST__  /*用于功能性测试，正式版本需要关闭本宏*/
#ifndef GPRS_SUPPORT
#define   __DSM_GSM_ONLY__ /*纯GSM手机(无GPRS功能)开启之*/
#endif

//埃及
//#define __MMI_DSM_EGYPT_SUPPORT__
//印度
#ifdef IM_HINDI_SUPPORT
#define __MMI_DSM_INDIA_SUPPORT__
#endif
//巴西
//#define __MMI_DSM_BRAZIL_SUPPORT__
//俄罗斯
#ifdef IM_RUSSIAN_SUPPORT
#define __MMI_DSM_RUSSIA_SUPPORT__
#endif
//哥伦比亚
//#define __MMI_DSM_COLOMBIA_SUPPORT__
//哈萨克斯坦
//#define __MMI_DSM_KAZAKHSTAN_SUPPORT__
#ifdef IM_MALAY_SUPPORT
//马来西亚
#define __MMI_DSM_MAlAYSIA_SUPPORT__
#endif
//秘鲁
//#define __MMI_DSM_PERU_SUPPORT__
//墨西哥
//#define __MMI_DSM_MEXICO_SUPPORT__
//南非
//#define __MMI_DSM_SOUTH_AFRICA_SUPPORT__
//尼日利亚
//#define __MMI_DSM_NIGERIA_SUPPORT__
//泰国
#ifdef IM_THAI_SUPPORT
#define __MMI_DSM_THAILAND_SUPPORT__
#endif
//土耳其
//#define __MMI_DSM_TURKEY_SUPPORT__
//委内瑞拉
//#define __MMI_DSM_VENEZUELA_SUPPORT__
//乌克兰
//#define __MMI_DSM_UKRAINE_SUPPORT__
//印尼
#ifdef IM_INDONESIAN_SUPPORT
#define __MMI_DSM_INDONESIA_SUPPORT__
#endif
//智利
//#define __MMI_DSM_CHILE_SUPPORT__


//肯尼亚
#define __MMI_DSM_KENYA_SUPPORT__

#ifdef IM_SPANISH_SUPPORT
//阿根廷
#define __MMI_DSM_AGENTINA_SUPPORT__
#endif

#ifdef IM_VIETNAMESE_SUPPORT
//越南
#define __MMI_DSM_VIETNAM_SUPPORT__
#endif

#ifdef IM_ENGLISH_SUPPORT
//尼日利亚
#define __MMI_DSM_NIGERIA_SUPPORT__
#endif


#if defined(IM_DUTCH_SUPPORT)||defined(IM_ENGLISH_SUPPORT)
//南非
#define __MMI_DSM_SOUTH_AFRICA_SUPPORT__
#endif

#if defined(IM_RUSSIAN_SUPPORT)
//哈萨克斯坦
#define __MMI_DSM_KAZAKHSTAN_SUPPORT__
#endif

#ifdef IM_ARABIC_SUPPORT
//埃及
#define __MMI_DSM_EGYPT_SUPPORT__
#endif

#ifdef IM_SPANISH_SUPPORT
//墨西哥
#define __MMI_DSM_MEXICO_SUPPORT__
#endif

void MMIDMENU_Entry(int entry);

#endif

