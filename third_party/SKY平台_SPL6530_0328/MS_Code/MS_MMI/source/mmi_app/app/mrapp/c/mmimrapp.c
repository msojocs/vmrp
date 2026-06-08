#ifdef MRAPP_SUPPORT
/****************************************************************************
** File Name:      mmimrapp.c                            
** Author:            qgp                                                      
** Date:            2006/05/15
** Copyright:           
** Description:     
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE               NAME             DESCRIPTION                             
** 04/01/2006	qgp		          Create
** 
****************************************************************************/
#ifndef _MMIMRAPP_C_
#define _MMIMRAPP_C_  

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "window_parse.h"
#include "Mn_type.h"
#include "mmisms_app.h"
#include "block_mem.h"
#include "mmipub.h"
#include "mmimrapp_export.h"
#include "mmimrapp_internal.h"
#include "mmimrapp_id.h"
#include "mmienvset_export.h"
#include "mmi_nv.h"
#include "mmiudisk_export.h"

#ifdef __MMI_SKYQQ__
#include "sky_qq_open.h"
#endif

#ifndef __MRAPP_OVERSEA__
#include "mmimrdata_cookie.h"
#include "mmimrdata_config.h"
#else
	#ifdef MRAPP_SKYBUDDY_OPEN
	#include "mmimrdata_skybuddy.h"
	#endif
	#ifdef MRAPP_MRPSTORE_OPEN	 
	#include "mmimrdata_mrpstore.h"
	#endif
#endif
#include "mmifmm_export.h"

#ifdef FRUIT_FIRMED
#include "mmimrdata_fruit.h"
#endif

#ifdef BIRD_FIRMED
#include "mmimrdata_bird.h"
#endif

#ifdef TALKCAT_FIRMED
#include "mmimrdata_talkcat.h"
#endif

#include"mmimrdata_oversea_firmed.h"

#define FIRM_LIST_IND 3//"D", do not change
#define FIRM_NES3_IND 5//"F", do not change
#define FIRM_NES1_IND 6//"G", do not change
#define FIRM_NES2_IND 7//"H", do not change
#define FIRM_KER_IND  8//"I", do not change
#define FIRM_GML_IND  9//"J", do not change
#define FIRM_INDEX_BG 10//"K", do not change

/*
 “1”前台不显示wap
 “2”前台显示wap，
 “3”后台不显示wap，
 “4”后台显示wap
 SPR平台只支持模式“1”
 */
#define STARTUP_CLOCK_UI_MODE 1
char dsmEntry[100] = {0}; 
#ifdef MRAPP_CLOCK_OPEN
char strMrpFile[256];
#endif

mr_screen_angle_t g_screen_angle = {0};


const GUIMENU_GROUP_T mrp_menu_table[] = 
{
    {NULL},
};

LOCAL uint16 mrp_nv_len[] =
{
	sizeof(MRAPP_ENVINFO_T),
	sizeof(int8),
};

#if defined(__MR_CFG_LICENSE_MANAGER__) && !defined(WIN32)
/*********************************************************************************
 * 本地应用权限注册函数列表
 *********************************************************************************/
MR_LAPP_LICENSE_MANAGER_BEGIN()

/* 将你的应用程序参照如下方式添加到下面 */
#ifdef __MR_CFG_ENTRY_MYAPP__	
	MR_LAPP_LICENSE_ENTRY("mrapp", mrapp_license_register)
#endif

MR_LAPP_LICENSE_MANAGER_END();
#endif

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
LOCAL BOOLEAN MMIMRAPP_checkStorage(uint8 grade);
LOCAL void MMIMRAPP_clearAllreg(void);

/*****************************************************************************/
// 	Description : mrp应用的入口
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_AppEntry(MRAPP_APPENTRY_E index)
{
	BOOLEAN recode = FALSE;
	MMIMR_Trace(("MMIMRAPP_AppEntry() index: %d", index));

	MMIMRAPP_setCurApp(index);
	MMIMRAPP_clearAllreg();

	/*modify by zack@20120803 start*/
#ifdef MMI_RES_ORIENT_BOTH
    g_screen_angle.lcd_angle = LCD_ANGLE_0;
    g_screen_angle.win_angle = WIN_SUPPORT_ANGLE_0;
    MMK_SetScreenAngle(LCD_ANGLE_0);
#endif
	/*modify by zack@20120803 end*/

	MMITHEME_DestroyMenuItemCache();

	switch(index)
	{
#ifdef MRAPP_SDKQQ_OPEN
    case MRAPP_SDKQQ:
        if(MMIMRAPP_checkStorage(0))
        {
            mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
            mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);

            recode = MMIMRAPP_MrAppRun((void*)"*J,qq2008", index);		
        }
        break;
#endif

#if defined(__MMI_SKYQQ__) && !defined(__MRAPP_OVERSEA__)
	case MRAPP_SKYQQ:
    	if(MMIMRAPP_checkStorage(0))
    	{
           mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
           mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
       	   
           if(GetSkyQQRunState() != QQ2008_BACK_RUN)
           {
               recode = MMIMRAPP_MrAppRun((void*)"*J,skyqq", index);
           }
           else
           {
               MMIMRAPP_SetBackRun(MRAPP_MAX);
    	       recode = MMIMRAPP_MrAppRun((void*)"*J,skyqq,203106", index);
           }
    	}
    	break;
#endif 

#ifdef MRAPP_EBOOK_OPEN
	case MRAPP_EBOOK:
    	if(MMIMRAPP_checkStorage(0))
    	{
            mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
    		mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);

    		recode = MMIMRAPP_MrAppRun((void*)"*J,skyread", index);
    	}
    	break;
#endif

#ifdef MRAPP_COOKIE_OPEN
	case MRAPP_COOKIE:
    	if(MMIMRAPP_checkStorage(0))
    	{
    		mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
            recode = MMIMRAPP_MrAppRun((void*)"*J,games", index);
    	}
    	break;
#endif

#ifdef MRAPP_MRPSTORE_OPEN
	case MRAPP_MRPSTORE:
    	if(MMIMRAPP_checkStorage(0))
    	{
            mr_registerAPP((uint8 *)mr_m0_mrpstore, sizeof(mr_m0_mrpstore), FIRM_GML_IND);
            recode = MMIMRAPP_MrAppRun((void*)"*J,1,0,3,3,5,0,0", index);
    	}
    	break;
#endif

#ifdef MRAPP_SKYBUDDY_OPEN
	case MRAPP_SKYBUDDY:
    	if(MMIMRAPP_checkStorage(0))
    	{
    	int i=10;
		mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), FIRM_GML_IND);
		mr_registerAPP((uint8 *)mr_m0_edg, sizeof(mr_m0_edg), 8);
		#ifdef FTALK_FIRMED
		mr_registerAPP((uint8 *)mr_ftalk, sizeof(mr_ftalk), i++);
		#endif
		#ifdef YAHOO_FIRMED
		mr_registerAPP((uint8 *)mr_yahoo, sizeof(mr_yahoo), i++);
		#endif
		#ifdef SKYPE_FIRMED
		mr_registerAPP((uint8 *)mr_cskype, sizeof(mr_cskype), i++);
		#endif
		#ifdef TWITTER_FIRMED
		mr_registerAPP((uint8 *)mr_twitter, sizeof(mr_twitter), i++);
		#endif
		#ifdef MSN_FIRMED
		mr_registerAPP((uint8 *)mr_msn_oversea, sizeof(mr_msn_oversea), i++);
		#endif
		#ifdef FACEBOOK_FIRMED
		mr_registerAPP((uint8 *)mr_fbook, sizeof(mr_fbook), i++);
		#endif

		recode = MMIMRAPP_MrAppRun((void*)"*J", index);
    	}
    	break;
#endif

#ifdef MRAPP_DDZ_OPEN
	case MRAPP_DDZ:
		mr_registerAPP((uint8 *)mr_m0_ddz, sizeof(mr_m0_ddz), 7);
		recode = MMIMRAPP_MrAppRun((void*)"*H", index);		
#endif


#ifdef MRAPP_CLOCK_OPEN
	case MRAPP_CLOCK:
    	if(MMIMRAPP_checkStorage(0)) 
    	{
    		mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,clockc", index);
    	}
    	break;
#endif  
 

#ifdef MRAPP_QIPAI_OPEN
	case MRAPP_QIPAI:
		if(MMIMRAPP_checkStorage(0)) 
		{
    		mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,mrpcc", index);
		}
		break;
#endif 

#ifdef MRAPP_TIANQI_OPEN
	case MRAPP_TIANQI:
		if(MMIMRAPP_checkStorage(0)) 
		{
			mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,otatq", index);
		}
		break;
#endif 


#ifdef MRAPP_KAIXIN_OPEN
	case MRAPP_KAIXIN:
		if(MMIMRAPP_checkStorage(0)) 
		{
    		mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);

			recode = MMIMRAPP_MrAppRun((void*)"*J,kx001", index);
		}
		break;
#endif 

#ifdef MRAPP_FETION_OPEN
	case MRAPP_FETION:
		if(MMIMRAPP_checkStorage(0)) 
		{
    		mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);			

			recode = MMIMRAPP_MrAppRun((void*)"*J,otafx3", index);
		}
		break;
#endif 

#ifdef MRAPP_XLWB_OPEN
	case MRAPP_XLWB:
		if(MMIMRAPP_checkStorage(0)) 
		{
    		mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,axlwb", index);
		}
		break;
#endif 

#ifdef MRAPP_FACEBOOK_OPEN 
	case MRAPP_FACEBOOK:
    	if(MMIMRAPP_checkStorage(0))
    	{
    	#ifdef FACEBOOK_FIRMED
			mr_registerAPP((uint8 *)mr_fbook, sizeof(mr_fbook), 10);
		#endif
             #ifdef MRAPP_SKYBUDDY_OPEN
	       mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
	       recode = MMIMRAPP_MrAppRun((void*)"*J,1000,5,0,3,3,5,0,0,Facebook,810002,fbook.mrp", index);
	       #else
    		mr_registerAPP((uint8 *)mr_m0_mrpstore, sizeof(mr_m0_mrpstore), FIRM_GML_IND);
    		recode = MMIMRAPP_MrAppRun((void*)"*J,1000,1,0,3,3,5,0,0,Facebook,810002,fbook.mrp", index);
		#endif
        }
    	break;
#endif


#ifdef MRAPP_YAHOO_OPEN
	case MRAPP_YAHOO:
    	if(MMIMRAPP_checkStorage(0))
    	{
    	#ifdef YAHOO_FIRMED
			mr_registerAPP((uint8 *)mr_yahoo, sizeof(mr_yahoo), 10);
		#endif
             #ifdef MRAPP_SKYBUDDY_OPEN
	       mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
              recode = MMIMRAPP_MrAppRun((void*)"*J,1000,5,0,3,3,5,0,0,Yahoo,810000,yahoo.mrp", index);	
	       #else
    		mr_registerAPP((uint8 *)mr_m0_mrpstore, sizeof(mr_m0_mrpstore), FIRM_GML_IND);
            recode = MMIMRAPP_MrAppRun((void*)"*J,1000,1,0,3,3,5,0,0,Yahoo,810000,yahoo.mrp", index);	
		#endif
    	}
    	break;
#endif

#ifdef MRAPP_TWITTER_OPEN
	case MRAPP_TWITTER:
    	if(MMIMRAPP_checkStorage(0))
    	{
    	#ifdef TWITTER_FIRMED
			mr_registerAPP((uint8 *)mr_twitter, sizeof(mr_twitter), 10);
		#endif
	      #ifdef MRAPP_SKYBUDDY_OPEN
	       mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
	       recode = MMIMRAPP_MrAppRun((void*)"*J,1000,5,0,3,3,5,0,0,Twitter,810006,twitter.mrp", index);	
	      #else
              mr_registerAPP((uint8 *)mr_m0_mrpstore, sizeof(mr_m0_mrpstore), FIRM_GML_IND);
              recode = MMIMRAPP_MrAppRun((void*)"*J,1000,1,0,3,3,5,0,0,Twitter,810006,twitter.mrp", index);	
	      #endif
    	}
    	break;
#endif

#ifdef MRAPP_TALKCAT_OPEN
	case MRAPP_TALKCAT:
	    if(MMIMRAPP_checkStorage(0))
	    {
	    #ifdef __MRAPP_OVERSEA__
		    #ifdef TALKCAT_FIRMED 
			 mr_registerAPP((uint8 *)mr_rglm, sizeof(mr_rglm), 10);
			#endif
			mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
			recode = MMIMRAPP_MrAppRun((void*)"*J,1000,3,0,3,3,5,0,0,TomCat,814459,rglm.mrp", index);
		#else
	        mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
	        mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
	    	recode = MMIMRAPP_MrAppRun((void*)"*J,talkcat", index);
		#endif
	    }
	    break;
#endif

#ifdef MRAPP_SKIRT_OPEN
	case MRAPP_SKIRT:
	    if(MMIMRAPP_checkStorage(0))
	    {
#ifdef __MRAPP_OVERSEA__
	mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
	recode = MMIMRAPP_MrAppRun((void*)"*J,1000,3,0,3,3,5,0,0,SKIRT,819995,rchqz.mrp", index);
#else
	        mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
	        mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
	    	recode = MMIMRAPP_MrAppRun((void*)"*J,skirt", index);
#endif
	    }
	    break;
#endif

#ifdef MRAPP_BIRD_OPEN
	case MRAPP_BIRD:
		if(MMIMRAPP_checkStorage(0))
		{
		#ifdef __MRAPP_OVERSEA__
		    	#ifdef BIRD_FIRMED
			mr_registerAPP((uint8 *)mr_gfkxn_oversea, sizeof(mr_gfkxn_oversea), 10);
			#endif
			mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
			recode = MMIMRAPP_MrAppRun((void*)"*J,1000,3,0,3,3,5,0,0,AngryBird,814041,gfkxn.mrp", index);
		#else
			mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,gfkxn", index);	
		#endif
		}
		break;

#endif

#ifdef MRAPP_FRUIT_OPEN
	case MRAPP_FRUIT:
		if(MMIMRAPP_checkStorage(0))
		{
		#ifdef __MRAPP_OVERSEA__
			#ifdef FRUIT_FIRMED
			mr_registerAPP((uint8 *)mr_fruit, sizeof(mr_fruit), 10);
			#endif
			mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
			recode = MMIMRAPP_MrAppRun((void*)"*J,1000,3,0,3,3,5,0,0,Fruit,814042,rqshg.mrp", index);
			#else
			mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,rzsgqd", index);	
			#endif
		}
		break;

#endif

#ifdef MRAPP_BYDR_OPEN
	case MRAPP_BYDR:
		if(MMIMRAPP_checkStorage(0))
		{
		#ifdef __MRAPP_OVERSEA__
			mr_registerAPP((uint8 *)mr_eim, sizeof(mr_eim), 9);
			recode = MMIMRAPP_MrAppRun((void*)"*J,1000,3,0,3,3,5,0,0,BYDR,819994,rbydr.mrp", index);
		#else
			mr_registerAPP((uint8 *)mr_m0_config, sizeof(mr_m0_config), 18);
			mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), FIRM_GML_IND);
			recode = MMIMRAPP_MrAppRun((void*)"*J,bydr", index);
		#endif
		}
		break;
#endif


    default:
        MMIMR_Trace(("mrapp invalid entry index = %d", index));
        break;
	}

	return recode;
}

/*****************************************************************************/
// 	Description :获取存储器状态
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIMRAPP_checkStorage(uint8 grade)
{
    if ((TRUE != MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_UDISK, MMIFILE_DEVICE_UDISK_LEN))
        && (TRUE != MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN))
#ifdef DUAL_TCARD_SUPPORT
        && (TRUE != MMIAPIFMM_GetDeviceStatus(MMIFILE_DEVICE_SDCARD2, MMIFILE_DEVICE_SDCARD2_LEN))
#endif
        )
	{		
        if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
        {
            MMIPUB_OpenAlertWinByTextId(PNULL,TXT_COMMON_UDISK_USING,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }
        else
        {
			MMIPUB_OpenAlertWinByTextId(NULL,TXT_NO_SD_CARD_ALERT,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,NULL);
        }
		return FALSE;
	}
    else
    {
        return TRUE;
	}
}

/*****************************************************************************/
// 	Description : 保存下载的mrp文件
//	Global resource dependence : 
//  Author: qgp
//  return : 
//0		下载文件是mr 文件，且保存成功
//-1	下载文件是mr文件，但保存失败
// 1	下载文件不是mr 文件
// 2	存储空间不足
/*****************************************************************************/
PUBLIC int32 MMIMRAPP_saveMrApp(uint8 *data,uint32 len)
{
	return MMIMRAPP_savePack(data, len);
}

/*****************************************************************************/
// 	Description : 检查是否是DSM的配置信息。
//	Global resource dependence : 
//  Author: qgp
//  Note : return:
// 0	是DSM配置消息，配置操作成功，需要删除该消息
//-1	是DSM配置消息，配置操作失败，需要删除该消息
// 1	不是DSM配置消息
/*****************************************************************************/
PUBLIC int32 MMIMRAPP_dsmSMSfilter(MN_SMS_ALPHABET_TYPE_E  alphabet_type, MN_SMS_USER_VALID_DATA_T *data, MN_CALLED_NUMBER_T *number)
{
	int32 result, recode = 1;
	
	result = MMIMRAPP_SMSfilter(alphabet_type, data, number);

	if(result == MR_SUCCESS)
		recode = 0;
	else if(result == MR_FAILED)
		recode = -1;
	else
		recode = 1; 

	return recode;
}

/*****************************************************************************/
// 	Description : register mrp module nv len and max item
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_initModule(void)
{
	MMI_RegMenuGroup(MMI_MODULE_MRAPP, mrp_menu_table);
	MMI_RegModuleNv(MMI_MODULE_MRAPP, mrp_nv_len, sizeof(mrp_nv_len) / sizeof(uint16));
}

/*****************************************************************************/
// 	Description : 强制关闭mrp 应用及引擎
//	Global resource dependence : 
//  Author: qgp
//	Note: 当点击触摸屏下方的快捷键来切换菜单时,需要关闭mrp,因
// 为内存共享等原因,否则会产生冲突.
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_closeMrapp(void)
{
	MMIMR_Trace(("MMIMRAPP_closeMrapp(), %d", g_mr_vmBackRun));
#ifdef MR_BINDTO_VIRTUAL_WIN
	if(g_mr_vmBackRun != MRAPP_MAX)
	{
#ifdef __MMI_SKYQQ__
		MAIN_SetIdleQQState(0, GUIWIN_STBDATA_ICON_NONE);
#endif
		MMIMRAPP_exitMrApp(FALSE);
	}
	else
#endif
	if(MMK_IsOpenWin(MMIMRAPP_BASE_WIN_ID))
	{
		MMIMRAPP_setAllReady(FALSE);
		if(MMIMRAPP_closeAllWins())
		{
			return TRUE;
		}
	}

    MMIMRAPP_StopStatusIconTimer();
    MAIN_SetIdleMrappState(FALSE); /*lint !e718 !e18*/
		
	return FALSE;
}

/*****************************************************************************/
// 	Description : 查询mr app是否在运行(用于与mr app有内存、网络等资源
//   冲突的模块调用)
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_mrappIsActived(void)
{
	if(s_mr_allReady
#ifdef MR_BINDTO_VIRTUAL_WIN
		|| (g_mr_vmBackRun != MRAPP_MAX)
#endif
		)
	{
		return TRUE;
	}

	return FALSE;
}

/*****************************************************************************/
// 	Description : 保存当前手机的必要状态.
//	Global resource dependence : none
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC void MMIMRAPP_setPhoneState(e_mrapp_phone_mod mod, int32 state)
{
	if(mod < MRAPP_MD_MAXNUM)
	{
		g_mr_phoneModState[mod] = state;
	}
}

/*****************************************************************************/
// 	Description :  初始化设置
//	Global resource dependence : 
//  Author:qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN  MMIMRAPP_InitSetting(void)
{
#if defined(__MR_CFG_LICENSE_MANAGER__) && !defined(WIN32)
    extern void mr_license_mgr_initialize(int32);
#endif
	/*2009-10-22 111,17787 add. for overseas*/
	MMIMRAPP_initApn();
	/*2009-10-22 111,17787 add end*/

#if defined(__MR_CFG_LICENSE_MANAGER__) && !defined(WIN32)
	mr_license_mgr_initialize(0);
#endif

	return TRUE;
}

/*****************************************************************************/
// 	Description : 通知应用恢复前台运行(如果app当前没有运行，则起app主菜单)
//	Global resource dependence : 
//  Author: qgp
//	Note: param - no use now.
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_activeMrapp2Front(MRAPP_APPENTRY_E app, void *param)
{
	BOOLEAN ret = FALSE;

    MMIMR_Trace(("mrapp MMIMRAPP_activeMrapp2Front app = %d", app));
	if(MMIMRAPP_mrappIsActived())
	{
		ret = MMIMRAPP_MrAppRun(NULL, app);
        /*应用由后台激活到前台时去除状态栏图标*/
        if(TRUE == ret)
        {
            MAIN_SetIdleMrappState(FALSE); /*lint !e718 !e18*/
        }
    }
	else
	{
		ret = MMIMRAPP_AppEntry(app);
	}
	
	return ret;
}

/*****************************************************************************/
// 	Description : 清空所有apps注册
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
LOCAL void MMIMRAPP_clearAllreg(void)
{
	int i;
	for(i = 0; i < 20; i++)
	{
		mr_registerAPP((uint8 *)NULL, 0, i);
	}
}

/*****************************************************************************/
// 	Description : 查询厂商机型信息
//	Global resource dependence : 
//  Author: zack
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_ProcessShow(void)
{
	MMI_STRING_T  s_text = {0};
	BOOLEAN ret = TRUE;
#ifdef __MR_CFG_FEATURE_HANDSETID_EX__
	uint8   temp_buf1[64] = {0};
	uint16  temp_buf2[64] = {0};
	uint8  temp_bufin[34] = {0};
#else
	uint8   temp_buf1[32] = {0};
    uint16  temp_buf2[32] = {0};
#endif
	uint32 temp_time=15000;
#ifdef __MR_CFG_FEATURE_HANDSETID_EX__
	mr_setting_get_handsetID_ex(temp_bufin,34,NULL,NULL,NULL);
	MMIMR_Trace(("MMIMRAPP_ProcessShow() manu:%s handset:%s", MRAPP_MANUFACTORY, temp_bufin));
#else
	MMIMR_Trace(("MMIMRAPP_ProcessShow() manu:%s handset:%s", MRAPP_MANUFACTORY, MRAPP_HANDSET));
#endif
	SCI_MEMSET(temp_buf1, 0, sizeof(temp_buf1));
	SCI_MEMSET(temp_buf2, 0, sizeof(temp_buf2));
#ifdef __MR_CFG_FEATURE_HANDSETID_EX__
	sprintf((char*)temp_buf1, "%s  %s", MRAPP_MANUFACTORY, temp_bufin);
#else
    sprintf((char*)temp_buf1, "%s  %s", MRAPP_MANUFACTORY, MRAPP_HANDSET);
#endif
	s_text.wstr_len = MMIMRAPP_GB2UCS(temp_buf2, temp_buf1, strlen(temp_buf1)); /*lint !e64*/
	s_text.wstr_ptr = temp_buf2;
	
	MMIPUB_OpenAlertWinByTextPtr(&temp_time,&s_text,NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,NULL);	
	return ret;
}

/*****************************************************************************/
// 	Description : 测试入口
//	Global resource dependence : 
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_ProcessEntrStr(
									   uint8* str_ptr, //[IN] input string pointer
									   uint16 str_len  //[IN] input string length
									   )
{ /*lint !e18 !e516*/
	BOOLEAN recode = FALSE;
	MMIMR_Trace(("MMIMRAPP_ProcessEntrStr(), %d, %s", str_len, str_ptr));

	if(strncmp((char*)str_ptr, "*#220807#", str_len) == 0
       && str_len == strlen("*#220807#"))
	{
	    MMIMRAPP_setCurApp(MRAPP_TEST);
		recode = MMIMRAPP_MrAppRun((void*)"%dsm_gm.mrp", MRAPP_TEST);	
	}
	else if(strncmp((char*)str_ptr, "*#777755999#", str_len) == 0
             && str_len == strlen("*#777755999#"))
	{
		MMIMRAPP_ProcessShow();
		recode = TRUE;
	}

	return (recode);
}

/*****************************************************************************/
// 	Description : bl mem Cross 处理回调(可用于block_mem.c中的内存划分那里)
//	Global resource dependence : 
//  Author: qgp
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_BLMemCb(BLOCK_MEM_ID_E block_id, BLOCK_MEM_RELATION_STATUS_E relation_info)
{
    MMIMR_Trace(("mrapp MMIMRAPP_BLMemCb block_id = %d relation_info = %d", block_id, relation_info));
	if(BORROW_FROM_CURRENT_BLOCK == relation_info)
    {
		MMIMRAPP_closeMrapp();
		MMIMR_Trace(("mr close END"));
	}
	 
	return TRUE;
}

/*****************************************************************************/
// 	Description : 通知应用恢复前台运行(如果app当前没有运行，则起app主菜单)
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIMRAPP_activeMrapp(void)
{
    MMIMR_Trace(("mrapp MMIMRAPP_activeMrapp g_mr_vmBackRun = %d", g_mr_vmBackRun));
	return MMIMRAPP_activeMrapp2Front(g_mr_vmBackRun, NULL);
}
	
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: qgp
//	Note: 
/*****************************************************************************/
PUBLIC MRAPP_APPENTRY_E MMIMRAPP_GetCurHungApp(void)
{
	MMIMR_Trace(("MMIMRAPP_GetCurHungApp, %d", g_mr_vmBackRun));
	return (MRAPP_APPENTRY_E)g_mr_vmBackRun;
}

#ifdef MRAPP_CLOCK_OPEN
/*****************************************************************************/
//  Description : netclock timer callback fuction
//  Global resource dependence : 
//  Author: zzh
//  Note:
/*****************************************************************************/
void MMIMRAPP_StartNetClock(void)
{
    int i = 0;
    mr_screeninfo screen;

#ifndef __MR_CFG_VMMEM_CUSTOM__
    if(NOT_USE != BL_GetStatus(BLOCK_MEM_POOL_MRAPP)) /*lint !e64*/
    {
        return;
    }
#endif
   
	for(i = 0; i < 20; i++)
	{
		mr_registerAPP((uint8 *)NULL, 0, i);
	}
    MMIMRAPP_setCurApp(MRAPP_CLOCK);
    MMIMRAPP_setDefFilePath();

    if(MMIMRAPP_checkStorage(0)) 
    {
        mr_getScreenInfo(&screen);
        sprintf(strMrpFile, "app%d%d\\clockc.mrp", screen.width, screen.height);
        MMIMR_Trace(("MRAPP MMIMRAPP_StartNetClock strMrpStr = %s", strMrpFile));
        if(MR_IS_FILE==mr_info(strMrpFile))
        {
            sprintf(strMrpFile, "%%app%d%d\\clockc.mrp,%%%d", screen.width, screen.height, STARTUP_CLOCK_UI_MODE);
            MMIMR_Trace(("MRAPP startup netclock from TCard %s", strMrpFile));
        }
        else
        {
            sprintf(strMrpFile, "*J%%%d", STARTUP_CLOCK_UI_MODE);
            MMIMR_Trace(("MRAPP startup netclock from firm %s", strMrpFile));
        }
        MMIMRAPP_MrAppRun((void*)strMrpFile, MRAPP_CLOCK); 
    }
} 
#endif

#ifdef __MR_CFG_SHORTCUT_SUPPORT__
void mr_scut_start_app(void* param1, void *param2)
{
    int32 appId = *(int32 *)param1;
    uint32 menuId = *(uint32 *)param2;
    BOOLEAN ret = FALSE;
    MR_SHORTCUT_APPINFO_T appInfo = {0};

    MMIMRAPP_setDefFilePath();

    if(MR_SUCCESS == mr_scut_read_data(appId, &appInfo))  
    {
        mr_scut_refresh_icon(menuId, appInfo);
    
        SCI_MEMSET(dsmEntry, 0x00, sizeof(dsmEntry));
        sprintf(dsmEntry, "*J,%s", appInfo.start_data);
#ifndef __MRAPP_OVERSEA__
        mr_registerAPP((uint8 *)mr_m0_cookie, sizeof(mr_m0_cookie), 9);
#endif
        ret = MMIMRAPP_MrAppRun((void*)dsmEntry, appId);
    }

    MMIMR_Trace(("[SKYSHORTCUT] mr_scut_start_app appId = %u dsmEntry = %s ret = %d", appId, dsmEntry, ret));
}
#endif

#endif//#_MMIMRAPP_C_

#endif//#end MRAPP_SUPPORT

