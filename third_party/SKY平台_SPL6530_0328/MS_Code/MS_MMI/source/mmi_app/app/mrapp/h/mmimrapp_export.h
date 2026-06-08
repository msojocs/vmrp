/****************************************************************************
** File Name:      mmimrapp_export.h                                               *
** Author:                                                                  *
** Date:           03/08/2012                                               *
** Copyright:      2003 Spreatrum, Incoporated. All Rights Reserved.        *
** Description:    this file is for the Third Part *
*****************************************************************************
**                         Important Edit History                           *
** -------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                              *
** 03/2012        		       Create
** 
****************************************************************************/
#ifndef  _MMIMRAPP_EXPORT_H_    
#define  _MMIMRAPP_EXPORT_H_  

 /**--------------------------------------------------------------------------*
 **                         Include Files                                     *
 **---------------------------------------------------------------------------*/
#include "mmidisplay_data.h"
#include "mmiset_export.h"
#include "mn_type.h"
#include "mmk_type.h"
//#include "mmisms_export.h"
#include "block_mem_def.h"
#include "guistatusbar.h"
#include "mmipdp_export.h"
#include "Mmimrapp.h"
#include "Mrporting.h"
#include "Mrapp_mmi_image.h"
#include "Mrapp_mmi_text.h"

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
typedef struct 
{
    int32 app_id;
    int32 icon_state;
}MRAPP_BACKRUN_INFO_T;

typedef enum
{
	MRAPP_STATE_ONLINE,
	MRAPP_STATE_OFFLINE,
	MRAPP_STATE_AWAY,
	MRAPP_STATE_INVISIBLE,
	MRAPP_STATE_MAX
}MMIMRAPP_BACKRUN_STATE_E;

/**--------------------------------------------------------------------------*
 **                         FUNCTINS                                         *
 **--------------------------------------------------------------------------*/


/*****************************************************************************/
// 	Description : the mr app (games) entity entry
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_AppEntry(MRAPP_APPENTRY_E index);

/*****************************************************************************/
// 	Description : 关闭mrp 应用及引擎
//	Global resource dependence : 
//  Author: qgp
//	Note: 当点击触摸屏下方的快捷键来切换菜单时,需要关闭mrp,因
// 为内存共享等原因,否则会产生冲突.
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_closeMrapp(void);

/*****************************************************************************/
// 	Description : save mr app content entry
//	Global resource dependence : 
//  Author: qgp
//	Note:
//return : 
//0		下载文件是mr 文件，且保存成功
//-1	下载文件是mr文件，但保存失败
// 1	下载文件不是mr 文件

/*****************************************************************************/
PUBLIC int32 MMIMRAPP_saveMrApp(uint8 *data,uint32 len);

/*****************************************************************************/
// 	Description : 检查是否是DSM的配置信息。
//	Global resource dependence : 
//  Author: qgp
//  Note : return:
// 0是DSM配置消息，配置操作成功，需要删除该消息
//-1是DSM配置消息，配置操作失败，需要删除该消息
// 1不是DSM配置消息
/*****************************************************************************/
PUBLIC int32 MMIMRAPP_dsmSMSfilter(MN_SMS_ALPHABET_TYPE_E  alphabet_type, MN_SMS_USER_VALID_DATA_T *data, MN_CALLED_NUMBER_T *number);

/*****************************************************************************/
// 	Description : 查询mr app是否在运行(用于与mr app有内存、网络等资源
//   冲突的模块调用)
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_mrappIsActived(void);

/*****************************************************************************/
// 	Description : 保存当前手机的必要状态.
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_setPhoneState(e_mrapp_phone_mod mod, int32 state);

/*****************************************************************************/
// 	Description : register mrp module nv len and max item
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_initModule(void);

/*****************************************************************************/
// 	Description :  初始化设置
//	Global resource dependence : 
//  Author:qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN  MMIMRAPP_InitSetting(void);

/*****************************************************************************/
// 	Description : 通知应用恢复前台运行(如果app当前没有运行，则起app主菜单)
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_activeMrapp(void);

/*****************************************************************************/
// 	Description : VM后台运行时启动WAP、Camera等模块之前先调用该接口，由该接口
//                根据传入的模块ID来决定强制退出后台应用还是给出提示
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_HandleEventFromOtherModule(int32 module_id);

/*****************************************************************************/
// 	Description : 获取虚拟机运行状态
//	Global resource dependence : none
//  Author: zack
//	Note:  
/*****************************************************************************/
PUBLIC int32 MMIMRAPP_GetVMState(void);

/*****************************************************************************/
// 	Description : 获取当前mr中的sim设置
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMIMRAPP_GetActiveSim(void);

/*****************************************************************************/
// 	Description : 获取当前后台运行应用的相关信息
//	Global resource dependence : none
//  Author: zack
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_GetBackRunAPPInfo(MRAPP_BACKRUN_INFO_T *info);

/*****************************************************************************/
// 	Description : 测试入口
//	Global resource dependence : 
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_ProcessEntrStr(
									   uint8* str_ptr, //[IN] input string pointer
									   uint16 str_len  //[IN] input string length
									   );

/*****************************************************************************/
//	Description : 应用消息处理函数澹(只关心自己应用的消息)
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIMRAPP_handleMrMsg(MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*****************************************************************************/
//	Description : 设置收到短信是否提示
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC int32 MMIMRAPP_GetNewSmsAction(void);

/*****************************************************************************/
//	Description : 判断窗口是否由SKY的应用创建
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_IsWindowCreatedBySky(void);

/*****************************************************************************/
//	Description : 判断VM是否处于pause状态
//	Global resource dependence : none
//	Author: zack
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_IsVMPaused(void);

/*****************************************************************************/
//	Description : 判断短信读取操作是否由SKY的应用进行
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_IsReadBySMS(void);
/*****************************************************************************/
//	Description : 设置短信读取操作是否由SKY的应用进行
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_SetIsReadBySMS(BOOLEAN IsReadBySMS);
/*****************************************************************************/
//	Description : 判断电话本界面是否由SKY的应用打开
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_IsWindowOfPB(void);
/*****************************************************************************/
//	Description : 设置电话本界面是否由SKY的应用打开
//	Global resource dependence : none
//	Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_SetIsWindowOfPB(BOOLEAN isWindowOfPB);

/*****************************************************************************/
// 	Description :  获取保存信息的order id
//	Global resource dependence : 
//  Author: Tommy.yan
//   note:
/*****************************************************************************/
//PUBLIC MMISMS_STATE_T * MMIMRAPP_GetSaveSMSOrderId(void);
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
 
#ifdef   __cplusplus
   }
#endif

#endif/*_MMIMRAPP_H_*/

