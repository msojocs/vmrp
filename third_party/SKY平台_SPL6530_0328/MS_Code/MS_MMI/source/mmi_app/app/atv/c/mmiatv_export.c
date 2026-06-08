/*****************************************************************************
** File Name:      mmiatv_export.c                                           *
** Author:                                                                   *
** Date:           2010-05	                                                 *
** Copyright:      2008 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe atv export                  *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 05/2010        xin.li           Create									 *
*****************************************************************************/

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "mmi_app_atv_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmiatv_export.h"
#ifdef ATV_SUPPORT
#include "mmk_type.h"
#include "mmk_msg.h"
#include "mmk_app.h"
#include "mmi_appmsg.h"
#include "mmi_modu_main.h"
#include "mmi_text.h"
#include "mmi_image.h"
#include "mmipub.h"
#include "mmiudisk_export.h"

#include "mmiatv_main.h"
#include "mmiatv_wintab.h"
#include "mmiatv_menutable.h"
#include "mmiatv_nv.h"
#include "mmiatv_id.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
#include "mmicc_export.h"
#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif
#ifdef FM_SUPPORT
#include "mmifm_export.h"
#endif
/*---------------------------------------------------------------------------*/
/*                          MACRO DEFINITION                                 */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/
/*lint -e552 -e765*/
MMI_APPLICATION_T g_atv_app = {0};
/*lint -e552 -e765*/

/*---------------------------------------------------------------------------*/
/*                         STATIC DEFINITION                                 */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          CONSTANT VARIABLES                               */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          FUNCTION DEFINITION                              */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
// 	Description : ATV app init
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_Init(void)
{
    g_atv_app.ProcessMsg = MMIATV_HandleSysMsg;
    g_atv_app.component_type = CT_APPLICATION;
}

/*****************************************************************************/
// 	Description : ATV module init
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_InitModule(void)
{
    MMIATV_RegMenuWin();
    MMIATV_RegWinIdNameArr();
    MMIATV_RegAtvNv();
}

/*****************************************************************************/
// 	Description : open ATV player
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIATV_OpenATV(void)
{
    //SCI_TRACE_LOW:"[MMIATV]: MMIAPIATV_OpenATV"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIATV_EXPORT_83_112_2_18_2_3_9_33,(uint8*)"");
    if(MMIATV_IsATVOpen())
    {
        //SCI_TRACE_LOW:"[MMIATV]: MMIAPIATV_OpenATV app is opened!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIATV_EXPORT_86_112_2_18_2_3_9_34,(uint8*)"");
        return FALSE;
    }
    
    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    
    if(MMIAPISET_GetFlyMode())
    {
        MMIPUB_OpenAlertWarningWin(TXT_CLOSE_FLY_MODE_FIRST);	
        return FALSE;
    }
	
    //QQ运行后，内存不足，进入DC要提示QQ退出
#ifdef QQ_SUPPORT
    if (QQ_IsRunning())
    {
        MMIPUB_OpenAlertWarningWin( TXT_EXIT_QQ);
        return FALSE;
    }
#endif

        //JAVA 后台运行,内存不足，提示退出
#ifdef JAVA_SUPPORT
    //SCI_TRACE_LOW:"MMIAPIATV_OpenATV java running =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIATV_EXPORT_103_112_2_18_2_3_9_35,(uint8*)"d", MMIAPIJAVA_IsJavaRuning());
    if (MMIAPIJAVA_IsJavaRuning())
    {
        MMIPUB_OpenAlertWarningWin( TXT_EXIT_BACKGROUND_JAVA);
        return FALSE;
    }
#endif

#ifdef MMI_CALL_MINIMIZE_SUPPORT
    //SCI_TRACE_LOW:"MMIAPIATV_OpenATV call running =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIATV_EXPORT_112_112_2_18_2_3_9_36,(uint8*)"d", MMIAPICC_IsInState(CC_IN_CALL_STATE));
    if (MMIAPICC_IsInState(CC_IN_CALL_STATE))
    {
        MMIPUB_OpenAlertWarningWin( TXT_CALL_USING);
        return FALSE;
    }

#endif

#ifdef MRAPP_SUPPORT
    if(MMIMRAPP_HandleEventFromOtherModule(MMI_MODULE_ATV)) /*lint !e718 !e18*/
        return FALSE;
#endif
 
#ifdef FM_SUPPORT
	if(MMIAPIFM_IsFmActive())
	{
		MMIPUB_OpenAlertWarningWin( TXT_EXIT_BACKGROUND_FM);
        return FALSE;
	}
#endif
    //open atv player win
    MMIATV_OpenATVMainWin();

    return TRUE;
}

/*****************************************************************************/
// 	Description : exit ATV
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_Exit(void)
{
    MMIATV_Exit();
}

/*****************************************************************************/
// 	Description : whether atv is open
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIATV_IsATVOpen(void)
{
    return MMIATV_IsATVOpen();
}

/*****************************************************************************/
// 	Description : get atv signal level value 
//	Global resource dependence : none
//  Author: xin.li
//	Note: level 0-5
//        ATV开启时获取TV信号强度，否则返回FALSE
/*****************************************************************************/
PUBLIC BOOLEAN  MMIAPIATV_GetTVSignalLevel(uint8 *signal_level)
{
    if(PNULL == signal_level)
    {
        return FALSE;
    }
    
    if(MMIATV_IsATVOpen())
    {
        *signal_level = MMIATV_GetTVSignalLevel();
        return TRUE;
    }
    else
    {
        //atv已关闭
        return FALSE;
    }
}

/*****************************************************************************/
// 	Description : get signal level image_id for status bar
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MMIAPIATV_GetTVStatusImageID(uint8 signal_level)
{
    return MMIATV_GetTVStatusImageID(signal_level);
}

/*****************************************************************************/
// 	Description : atv restore factory setting
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_RestoreFactorySetting(void)
{
    MMIATV_RestoreFactorySetting();
}


/*****************************************************************************/
// 	Description : clean user data
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_ClearUserData(void)
{
    MMIATV_ClearUserData();
}

/*****************************************************************************/
// 	Description : MMIAPIATV_IsRecording
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIATV_IsRecording(void)
{
	return  MMIATV_IsRecording();
}

/*****************************************************************************/
// 	Description : MMIAPIATV_HandleSensorMSG
//	Global resource dependence : none
//  Author: chunyou.zhu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_HandleSensorMSG(void)
{
	if (MMK_IsFocusWin(MMIATV_MAIN_WIN_ID))
	{
		MMK_SendMsg(MMIATV_MAIN_WIN_ID,MSG_ATV_SENSOR,PNULL);
	}
}

#else
/*****************************************************************************/
// 	Description : ATV app init
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_Init(void)
{
    return;
}

/*****************************************************************************/
// 	Description : ATV module init
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_InitModule(void)
{
    return;
}

/*****************************************************************************/
// 	Description : open ATV player
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIATV_OpenATV(void)
{
    return FALSE;
}

/*****************************************************************************/
// 	Description : exit ATV
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_Exit(void)
{
    return;
}

/*****************************************************************************/
// 	Description : whether atv is open
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIATV_IsATVOpen(void)
{
    return FALSE;
}

/*****************************************************************************/
// 	Description : get atv signal level value 
//	Global resource dependence : none
//  Author: xin.li
//	Note: level 0-5
//        ATV开启时获取TV信号强度，否则返回FALSE
/*****************************************************************************/
PUBLIC BOOLEAN  MMIAPIATV_GetTVSignalLevel(uint8 *signal_level)
{
    return FALSE;
}

/*****************************************************************************/
// 	Description : get signal level image_id for status bar
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MMIAPIATV_GetTVStatusImageID(uint8 signal_level)
{
    return 0;
}

/*****************************************************************************/
// 	Description : atv restore factory setting
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_RestoreFactorySetting(void)
{
    return;
}

/*****************************************************************************/
// 	Description : clean user data
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_ClearUserData(void)
{
	return;
}

/*****************************************************************************/
// 	Description : MMIAPIATV_IsRecording
//	Global resource dependence :
//  Author: xin.li
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIATV_IsRecording(void)
{
	return FALSE;
}

/*****************************************************************************/
// 	Description : MMIAPIATV_GetSensorMSG
//	Global resource dependence : none
//  Author: chunyou.zhu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIATV_HandleSensorMSG(void)
{
}
#endif

