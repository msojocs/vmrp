/****************************************************************************
** File Name:      mmimrapp_id.h                                               *
** Author:              qgp                                                   *
** Date:           05/15/2006                                             *
** Copyright:      2003 Spreatrum, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the data struct of        *
**                 system, applicaiton, window and control                 *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 05/2006        qgp		       Create
** 
****************************************************************************/
#ifndef  _MMIMRAPP_ID_H_    
#define  _MMIMRAPP_ID_H_  

 /**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"
#include "mmimrapp_internal.h"

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
   extern   "C"
   {
#endif


//temp. 
//#define	MMI_MODULE_MRAPP  (MMI_MODULE_CONNECTION+1)


typedef enum
{

	MMIMRAPP_WIN_ID_START = (MMI_MODULE_MRAPP << 16),
	MMIMRAPP_BASE_WIN_ID,
	MMIMRAPP_ACCOUNT_WIN_ID,
#ifdef __MR_CFG_CAMERA_SUPPORT__
    MMIMRAPP_CAMERA_WIN_ID,
#endif
	MRAPP_DYN_BASE_WIN_ID,
	MRAPP_DYN_END_WIN_ID = MRAPP_DYN_BASE_WIN_ID + MRAPP_DYN_WIN_ID_MAXNUM,

	MRAPP_CURRENT_WIN_ID,
	MRAPP_WEATHER_WIN_ID,
		
	MRAPP_MAX_WIN_ID
}MMIMRAPP_WINDOW_ID_E;

// 命名规则: 模块名 + 窗口名 + WIN_ID  ,  eg: CC_DIALING_WIN_ID (CC部分呼叫窗口ID)
typedef enum
{
  
	MMIMRAPP_CTRL_ID_START = MRAPP_MAX_WIN_ID,//2009 L change, (MMI_MODULE_MRAPP << 16),
	MRAPP_DYN_BASE_CTRL_ID,
	MRAPP_ACCOUNT_LISTCTRL_ID,
	MRAPP_DYN_END_CTRL_ID = MRAPP_DYN_BASE_CTRL_ID + MRAPP_DYN_WIN_ID_MAXNUM,
     
	MMIMRAPP_MAX_CTRL_ID  
}MMIMRAPP_CTRL_ID_E;

typedef enum
{
	MRAPP_ENV_INFO = (MMI_MODULE_MRAPP << 16),
	MRAPP_ENV_INITED,
	
	MRAPP_MAX_NV_ITEM_NUM
}MMIMRAPP_NV_ITEM_E;


/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
   }
#endif
#endif/*_MMIMRAPP_ID_H_*/

