/****************************************************************************
** File Name:      MMIMAIN.C                                               *
** Author:                                                                 *
** Date:           03/11/2003                                              *
** Copyright:      2003 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe the MMI main              *
*****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 11/2003       Louis.wei         modify
** 12/2003       Tracy Zhang       对任务初始化和对服务的注册进行修改      *
****************************************************************************/
#define MMIMAIN_C

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_app_kernel_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmi_signal.h"
#include "mmi_signal_ext.h"
#include "mmi_osbridge.h"
#include "mmk_msg.h"
#include "mmk_kbd.h"
#include "tb_dal.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmi_theme.h"
#include "mmisd_export.h"
#include "mmiphone_export.h"
#include "mmisms_export.h"
#if defined(MMI_SMSCB_SUPPORT)
#include "mmismscb_export.h"
#endif
#include "mmimp3_export.h"
#include "mmicc_export.h"
#include "mmistk_export.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "mmirecord_export.h"
#include "mmiacc.h"
//#include "mmipb_signal.h"
#include "mmipb_export.h"
#include "mmi_signal.h"
#include "mmi_id.h"
#include "guilcd.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmiset_func.h"
#include "mmi_image.h"
#ifdef WIN32
#include "mmiudisk_simu.h"
#else
#include "usbservice_api.h"
#endif
#include "mmiidle_export.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#endif
#include "mmidc_export.h"
#include "mmiconnection_export.h"
//#include "mmiacc_winmain.h"
#include "mmimms_export.h"
#if defined(MMI_MSD_SUPPORT)
#include "mmimsd_export.h"
#endif
#include "mmi_default.h"
#include "mmi_modu_main.h"
#include "mmi_module.h"
#include "mmiacc.h"
#include "tb_hal.h"
#include "sig_code.h"
//#include "mmiebook.h"
#include "mmiebook_export.h"
#include "mmirecord_export.h"
#include "mmikl_export.h"

#include "mmifm_export.h"
#include "mmi_menutable.h"
#include "mmifilearray_export.h"
#include "mmifmm_export.h"
#include "mmibt_export.h" // baokun add
//#include "prod_tasks_def.h"
#ifdef WIN32
#include "tp_srv.h"
#endif
#ifdef TOUCH_PANEL_SUPPORT
#ifndef WIN32
#include "tp_srv.h"
#else
//#include "tpd_drv_win32_simu.h"
#endif
//#include "tp.h"
#endif
#ifdef ASP_SUPPORT
#include "mmiasp_export.h"
#endif
#include "tp_export.h"
#include "freq_cfg.h"
#include "mmk_tp.h"

#include "mmisd_export.h"
//#include "mmigame.h"
#include "mmigame_export.h"
#include "mmiocr_export.h"
#include "mmiset_nv.h"
#include "mmipb_id.h"   //@maggie add for cr75741
#include "mmivt_export.h"
#include "mmipic_edit_export.h"
#include "mmikur.h"
#include "mmimtv_export.h"

#include "mmi_resource.h"
#include "block_mem.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
#include "mmiudisk_export.h"
#include "mmk_modinfo.h"
//#include "mmk_module_manager.h"
#include "mmiengtd_export.h"
#include "mmivcard_export.h"
#include "mmitv_out.h"
#ifdef MMI_MOCOR_SAMPLE
#include "sample_export.h"
#endif
#include "mmidm_export.h"
#include "mmipicview_export.h"

#include "mmiidle_subwintab.h"
#include "mmi_id.h"
#include "mmiim.h"
#include "mmifdn_export.h"

#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif

#ifdef DL_SUPPORT
#include "mmidl_export.h"
#endif

#ifdef MMI_ENABLE_DCD
#include "mmidcd_export.h"
#include "mmidcd_main.h"
#endif

#include "ui_layer.h"
#include "mmipdp_export.h"
#include "mmi_nv.h"
#ifdef DYNAMIC_MODULE_SUPPORT 
#include "mmidyna_export.h.h"
#endif
#include "mmipim_export.h"
//#include "mmieng.h"
#include "watchdog.h"

#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif

#ifdef MMI_LDN_SUPPORT
#include "mmicl_export.h"
#endif

#include "mmiclipbrd_export.h"

#if (defined MMIWIDGET_SUPPORT) || (defined MMI_GRID_IDLE_SUPPORT)
#include "mmiwidget.h"
#include "mmiwidget_export.h"
#endif

#ifdef MOTION_SENSOR_TYPE
#include "msensor_drv.h"
#endif
#include "mmk_gsensor.h"
#include "mmiatv_export.h"
#ifdef TTS_SUPPORT
#include "mmitts_export.h"
#endif
#include "mmisearch_export.h"


#ifdef PUSH_EMAIL_SUPPORT
#include "mail_export.h"
#include "mmimail_export.h"
#endif /* PUSH_EMAIL_SUPPORT */

#ifdef ATEST_SUPPORT
#include "mmi_atc.h"
#endif //ATEST_SUPPORT

#ifdef MOBILE_VIDEO_SUPPORT
#include "mmimv_export.h"
#endif

#ifdef MMIEMAIL_SUPPORT
#include "mmiemail_export.h"
#endif

#include "mmitheme_block.h"

#include "mmi_autotest.h"
#ifdef CMCC_UI_STYLE
#include "mmics_export.h"
#endif

#ifdef SQLITE_SUPPORT
#include "sqlite3.h"
#define  SQLITE_HEAP_LIMIT   (300*1024)
#endif

#ifdef DRM_SUPPORT
#include "mmidrm_export.h"
#endif
#ifdef WEATHER_SUPPORT   
#include "mmiweather_export.h"
#endif

#ifdef DYNAMIC_MAINMENU_SUPPORT
#include "mmimenu_synchronize.h"
#endif

#ifdef WRE_SUPPORT
#include "mmiwre.h"
#include "mmiwre_export.h"
#endif


#ifdef PDA_UI_DROPDOWN_WIN
#include "mmidropdownwin_export.h"
#endif
#ifdef MMI_KING_MOVIE_SUPPORT
#include "mmikm_export.h"
#endif 
#ifdef LIVE_WALLPAPER_FRAMEWORK_SUPPORT
#include "mmilivewallpaper_export.h"
#endif

#ifdef MCARE_V31_SUPPORT
#include "Mcare_Interface.h"
#endif
#ifdef QBTHEME_SUPPORT
#include "mmiqbtheme_export.h"
#endif
#ifdef HOME_LOCATION_INQUIRE_SUPPORT_TCARD
#include "mmiphonesearch_export.h"
#endif
#ifdef MMI_MEMO_SUPPORT
#include "mmimemo_export.h"
#endif
#ifdef IKEYBACKUP_SUPPORT
#include "mmiikeybackup_api.h"
#endif
#ifdef MMIUZONE_SUPPORT
#include "mmiuzone_export.h"
#endif
#ifdef MMI_PIC_CROP_SUPPORT
#include "mmipic_crop.h"
#endif
#ifdef SNS_SUPPORT
#include "mmisns_srv.h"
#endif
#ifdef BROWSER_SUPPORT_DORADO
#ifdef JS_SUPPORT
#include "mmibrowser_dorado_api.h"
#endif
#endif
#ifdef MET_MEX_SUPPORT
#include "met_mex_export.h"
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
 
#ifndef CHNG_FREQ_REQ_HANDLER_NULL
#define CHNG_FREQ_REQ_HANDLER_NULL 0xFFFFFFFF
#define ARM_CLK_LVL5    0x5 
#endif

#define MMI_MAINSCREEN_WIDTH_RESIZE (((MMI_MAINSCREEN_WIDTH+(MMIBLOCK_MIN_UNIT-1))/MMIBLOCK_MIN_UNIT)*MMIBLOCK_MIN_UNIT)
#define MMI_MAINSCREEN_HEIGHT_RESIZE (((MMI_MAINSCREEN_HEIGHT+(MMIBLOCK_MIN_UNIT-1))/MMIBLOCK_MIN_UNIT)*MMIBLOCK_MIN_UNIT)

#ifdef WIN32
	LOCAL BOOLEAN   s_flip_is_open = TRUE;			//翻盖是否打开的标志
	LOCAL BOOLEAN   s_flip_msg_is_ok = FALSE;		//翻盖消息是否已处理的标志
	LOCAL BOOLEAN   s_earphone_is_insert = FALSE;	//耳机是否插入的标志
	LOCAL BOOLEAN   s_earphone_msg_is_ok = FALSE;	//耳机插拔消息是否已处理的标志 
//because ref_base_win.lib has some questions
#else
    //Daniel.ding modify these for memory config ;
    LOCAL   uint32 s_mmi_chng_freq_req_handler = CHNG_FREQ_REQ_HANDLER_NULL;
#endif

LOCAL BOOLEAN s_is_high_freq_by_quick_key = FALSE;
/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
#ifdef UILAYER_MAINLAYER_ARGB888_TYPE
    #ifdef WIN32
        LOCAL uint32 s_mainlayer_buf[MMI_MAINSCREEN_WIDTH_RESIZE*MMI_MAINSCREEN_HEIGHT_RESIZE+256/sizeof(uint32)] = {0};
    #else
        LOCAL __align(256) uint32 s_mainlayer_buf[MMI_MAINSCREEN_WIDTH_RESIZE*MMI_MAINSCREEN_HEIGHT_RESIZE] = {0};
    #endif
#else
    #ifdef WIN32
        LOCAL GUI_COLOR_T s_mainlayer_buf[MMI_MAINSCREEN_WIDTH_RESIZE*MMI_MAINSCREEN_HEIGHT_RESIZE+256/sizeof(GUI_COLOR_T)] = {0};
    #else
        LOCAL __align(256) GUI_COLOR_T s_mainlayer_buf[MMI_MAINSCREEN_WIDTH_RESIZE*MMI_MAINSCREEN_HEIGHT_RESIZE] = {0};
    #endif
#endif
#ifdef TOUCH_PANEL_SUPPORT
LOCAL GUI_POINT_T s_tp_down_point = {0};    //记录TP_DOWN坐标点
#endif

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DECLARE                                 *
 **--------------------------------------------------------------------------*/
//use for virtual sublcd
LOCAL BOOLEAN s_is_flip = FALSE;//是否已经合盖
/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
//  Description : handle udisk call back function
//  Global resource dependence :                          
//  Author: Jassmine.Meng
//  Note: // @ wancan.you udisk
/*****************************************************************************/
LOCAL void UPMCallBackFunc(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr  // data follow the even
                              );

/*****************************************************************************/
//  Description : handle charge call back function
//  Global resource dependence :                      
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL void ChargeCallBackFunc(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr  // data follow the even
                              );

/*****************************************************************************/
//  Description : handle keypad call back function
//  Global resource dependence :                     
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
LOCAL void KeypadCallBackFunc(
                              BLOCK_ID id,    // Client thread ID
                              uint32   argc,  // Signal code
                              void     *argv  // Key Code
                              );

/*****************************************************************************/
//  Description : initial common menu group
//  Global resource dependence : 
//  Author:wancan.you
//  Modify:
//  Note: 
/*****************************************************************************/
LOCAL void RegComMenuGroup(void);

/*****************************************************************************/
//  Description : initial common module
//  Global resource dependence : 
//  Author:wancan.you
//  Modify:
//  Note: 
/*****************************************************************************/
LOCAL void InitCommonModule(void);

/*****************************************************************************/
//  Description : initial mmi modudle
//  Global resource dependence : 
//  Author:wancan.you
//  Modify:
//  Note: 
/*****************************************************************************/
LOCAL void InitMMIModule(void);

#ifdef WIN32
/*****************************************************************************/
// 	Description : set sub lcd flip flag
//	Global resource dependence : 
//  Author: jesse.yu
//	Note:
/*****************************************************************************/
LOCAL void SetFlipFlag(BOOLEAN flapisOpen);

/*****************************************************************************/
// 	Description : get sub lcd flip flag
//	Global resource dependence : 
//  Author: jesse.yu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN GetFlipFlag(void);

/*****************************************************************************/
// 	Description : set earphone insert flag
//	Global resource dependence : 
//  Author: jesse.yu
//	Note:
/*****************************************************************************/
LOCAL void SetEarphoneFlag(BOOLEAN earphoneIsInsert);

/*****************************************************************************/
// 	Description :  sub lcd flip and handset key on PC simulator 
//	Global resource dependence : 
//  Author: jesse.yu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SpecialKeyHandle(uint16 single_code, uint32 key_code);
#endif

#ifdef MMI_DUAL_BATTERY_SUPPORT
/*****************************************************************************/
//  Description : handle charge call back function
//  Global resource dependence :                     
//  Author: renyi.hu
//  Note: 
/*****************************************************************************/
LOCAL void DualBatteryCallBackFunc(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr  // data follow the even
                              );
#endif

//jesse yu
/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : Set systerm frequency for mmi app
//  Global resource dependence :                     
//  Author: Xiaoqing.Lu
//  Note: 
/*****************************************************************************/
PUBLIC uint32 MMI_SetFreq(PROD_FREQ_INDEX_E freq)
 {
#ifndef WIN32
    if (CHNG_FREQ_REQ_HANDLER_NULL != s_mmi_chng_freq_req_handler)
    {
        CHNG_FREQ_SetArmClk(s_mmi_chng_freq_req_handler,(CHNG_FREQ_INDEX_E)freq);       
    }
#endif
    //SCI_TRACE_LOW:"\"\"  MMI_SetFreq   \"#"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_430_112_2_18_2_0_27_256,(uint8*)"");
    
    return 1;
 }
 
/*****************************************************************************/
//  Description : Get systerm frequency for mmi app
//  Global resource dependence :                     
//  Author: Xiaoqing.Lu
//  Note: 
/*****************************************************************************/ 
PUBLIC uint32 MMI_RestoreFreq(void)
 {
#ifndef WIN32
     if (CHNG_FREQ_REQ_HANDLER_NULL != s_mmi_chng_freq_req_handler)
    {
        CHNG_FREQ_RestoreARMClk(s_mmi_chng_freq_req_handler);
    }       
#endif  
    //SCI_TRACE_LOW:"\"\"  MMI_RestoreFreq   \"#"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_447_112_2_18_2_0_27_257,(uint8*)"");
    return 1;
 }

/*****************************************************************************/
//  Description : apply for mmk module memory call back function
//  Global resource dependence :                     
//  Author: gang.tong
//  Note: 
/*****************************************************************************/
LOCAL void ReleaseMemoryCallBackFunc(uint8* memory_ptr)
{
    MMI_BL_FREE((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_CAF_IMAGE);
}

/*****************************************************************************/
//  Description : apply for mmk module memory call back function
//  Global resource dependence :                     
//  Author: gang.tong
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN ApplyForMemoryCallBackFunc(uint8** memory_pptr, uint32 * size_ptr)
{
    if(NOT_USE == BL_GetStatus((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_CAF_IMAGE))
    {      
        *memory_pptr = MMI_BL_MALLOC((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_CAF_IMAGE);
        *size_ptr = BL_GetSize((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_CAF_IMAGE);
        return TRUE;
    }
    else
    {
        SCI_ASSERT(0); /*assert verified*/
    }
    return FALSE;/*lint !e527 */
}

/*****************************************************************************/
//  Description : handle keypad call back function
//  Global resource dependence :                     
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
LOCAL void KeypadCallBackFunc(
                              BLOCK_ID id,    // Client thread ID
                              uint32   argc,  // Signal code
                              void     *argv  // Key Code
                              )
{
    MmiSignalS* sendSignal  = PNULL;
    uint32      key_code    = (uint32)argv;
    uint16      single_code = (uint16)(argc & 0xFFFF);
	//jesse yu add for Sub-LCD temp start
#ifndef WIN32//use for virtual sublcd	
#ifndef SUBLCD_SIZE_NONE
	//LOCAL BOOLEAN s_is_flip = FALSE;//是否已经合盖
	if(key_code != SCI_VK_HEADSET_DETECT && key_code != SCI_VK_STAR && s_is_flip)
	{
		if(key_code == SCI_VK_CALL)
		{
			key_code = SCI_VK_CAMERA;
			//SCI_TRACE_LOW:"Jesse Yu change call key to camera key"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_501_112_2_18_2_0_28_258,(uint8*)"");
		}
		else if(SCI_VK_UP == key_code)
		{
		    key_code = SCI_VK_SIDE_UP;
            	    //SCI_TRACE_LOW:"Jesse Yu change up key to SIDE_UP key"
            	    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_506_112_2_18_2_0_28_259,(uint8*)"");
		}
		else if(SCI_VK_DOWN == key_code)
		{
		    key_code = SCI_VK_SIDE_DOWN;
                    //SCI_TRACE_LOW:"Jesse Yu change down key to SIDE_down key"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_511_112_2_18_2_0_28_260,(uint8*)"");
		}
	}

	if(key_code == SCI_VK_STAR)
	{
		key_code = SCI_VK_FLIP;
		if (KPD_UP == single_code)
		{
			if(s_is_flip)
			{
				single_code = KPD_UP;
				s_is_flip = FALSE;
				//SCI_TRACE_LOW:"Jesse Yu flip up"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_524_112_2_18_2_0_28_261,(uint8*)"");
			}
			else
			{
				single_code = KPD_DOWN;
				s_is_flip = TRUE;
				//SCI_TRACE_LOW:"Jesse Yu flip down"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_530_112_2_18_2_0_28_262,(uint8*)"");
			}
		}
		else
		{
			//SCI_TRACE_LOW:"Jesse Yu star key down"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_535_112_2_18_2_0_28_263,(uint8*)"");
			return;
		}
	}
#endif
#endif	
	//jesse yu add for Sub-LCD temp end
#ifdef LCD_SLIDE_SUPPORT    
    MMIDEFAULT_ConvertSlideKey(&key_code, &single_code);
#endif    
    // check the key message
    if (MMK_CheckKeySingle(single_code, key_code))
    {
		//jesse yu added for sub lcd start
#ifdef WIN32
		if (SpecialKeyHandle(single_code, key_code))
		{
			return;
		}
#endif
		//jesse yu added for sub lcd end
        // create the signal to send requesting task
        MmiCreateSignal(single_code, sizeof(MmiKeyPress), &sendSignal);

        sendSignal->sig.keyPress.keyCode = key_code;
        sendSignal->send = KPDSVR;
        MmiSendSignal(TASK_FL_ID, sendSignal);
    }
    else
    {
        //SCI_TRACE_LOW:"KeypadCallBackFunc: the key message is too much!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_565_112_2_18_2_0_28_264,(uint8*)"");
    }
}


/*****************************************************************************/
//  Description : handle keypad call back function
//  Global resource dependence :                     
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
LOCAL void TPCallBackFunc(
                              BLOCK_ID id,    // Client thread ID
                              uint32   argc,  // Signal code
                              void     *argv  // Key Code
                              )
{
#ifdef TOUCH_PANEL_SUPPORT
//    typedef struct TPDSVR_SIG_tag
//    {
//        //xSignalHeaderRec signal_head;
//        SIGNAL_VARS
//        int              x_key;
//        int       y_key;
//    } TPDSVR_SIG_T;

    TPDSVR_SIG_T   *tp_ptr  = PNULL;
    MmiSignalS* sendSignal  = PNULL;
    uint16      single_code = (uint16)(argc & 0xFFFF);
    uint32      state = 0;
#ifndef TOUCH_PANEL_HWICON_SUPPORT_NONE
    GUI_POINT_T up_point = {0};
//    GUI_POINT_T point = {0};
    GUI_RECT_T  lcd_rect = {0};
    LCD_INFO_T  lcd_info ={0};
    uint16      hw_icon_num = 0;
    uint32      i;
    uint32      key_code = 0;
    uint16      key_single_code = 0;
    uint16      keyup_single_code = 0;
    MmiSignalS* KeysendSignal  = PNULL;
#endif

    //SCI_TRACE_LOW("TPCallBackFunc, P_TPP = %d", P_TPP);
    
    tp_ptr  = (TPDSVR_SIG_T*)argv;

    if (PNULL == tp_ptr)
    {
        return;
    }

    //和底层消息解耦
    //state = single_code - TP_DOWN + 1;

    if ( TP_DOWN == single_code )
    {
        state = MMI_TP_DOWN;
        
#ifndef TOUCH_PANEL_HWICON_SUPPORT_NONE
        //记录TP_DOWN起始点
        s_tp_down_point.x = tp_ptr->x_key;
        s_tp_down_point.y = tp_ptr->y_key;
#endif
    }
    else if( TP_UP == single_code )
    {
        state = MMI_TP_UP;
        
#ifndef TOUCH_PANEL_HWICON_SUPPORT_NONE
        //记录up点
        up_point.x = tp_ptr->x_key;
        up_point.y = tp_ptr->y_key;
#endif
    }
    else if( TP_MOVE == single_code )
    {
        state = MMI_TP_MOVE;
    }
    else if( TP_GESURE == single_code )
    {
        state = MMI_TP_GESTURE;
    }
    else
    {
        return;
    }

    //check the TP message
#ifdef TOUCH_PANEL_HWICON_SUPPORT_NONE
    if(MMK_CheckTPSignale((uint16)state, tp_ptr->x_key, tp_ptr->y_key ))
    {
        MmiCreateSignal((uint16)state, sizeof(MmiTPPress), &sendSignal);
        
        sendSignal->sig.TpPress.x  = tp_ptr->x_key;
        sendSignal->sig.TpPress.y  = tp_ptr->y_key;
        sendSignal->sig.TpPress.gesture_type = tp_ptr->gesture_type;
        sendSignal->send = P_TPP;
        
        MmiSendSignal(TASK_FL_ID, sendSignal);
    }
    else
    {
        //SCI_TRACE_LOW:"TPCallBackFunc: the TP message is too much!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_666_112_2_18_2_0_28_265,(uint8*)"");
    }

#else

    //为实现TP虚拟物理按键，需要考虑如下情形：
    //1.down point在LCD区域内，up point在LCD区域内， 正常发tp消息；
    //2.down point在LCD区域内，up point在LCD区域外， 需矫正poit到LCD区域内后发tp消息
    //3.down point在LCD区域外，up point在LCD区域外， 转成物理按键消息
    //4.down point在LCD区域外，up point在LCD区域内， 丢弃
    
    if(MMK_IsCoordinatingTp())
    {
        //校准触摸屏时，无需特殊处理
        MmiCreateSignal((uint16)state, sizeof(MmiTPPress), &sendSignal);
        sendSignal->sig.TpPress.x  = tp_ptr->x_key;
        sendSignal->sig.TpPress.y  = tp_ptr->y_key;
        sendSignal->sig.TpPress.gesture_type = tp_ptr->gesture_type;
        sendSignal->send = P_TPP;
        MmiSendSignal(TASK_FL_ID, sendSignal);
        
        return;
    }
    
    //get lcd physical rect
    GUILCD_GetInfo(0, &lcd_info);
    lcd_rect.left   = 0;
    lcd_rect.top    = 0;
    lcd_rect.right  = lcd_info.lcd_width -1;
    lcd_rect.bottom = lcd_info.lcd_height -1;
    
    if(GUI_PointIsInRect(s_tp_down_point, lcd_rect))
    {
        //check the TP message
        if(MMK_CheckTPSignale((uint16)state, tp_ptr->x_key, tp_ptr->y_key ))
        { 
            if(MMI_TP_UP != state)
            {
                MmiCreateSignal((uint16)state, sizeof(MmiTPPress), &sendSignal);
                
                sendSignal->sig.TpPress.x  = tp_ptr->x_key;
                sendSignal->sig.TpPress.y  = tp_ptr->y_key;
                sendSignal->sig.TpPress.gesture_type = tp_ptr->gesture_type;
                sendSignal->send = P_TPP;
                
                MmiSendSignal(TASK_FL_ID, sendSignal);
            }
            else
            { 
                if(GUI_PointIsInRect(up_point, lcd_rect))
                {
                    //down point在lcd_rect内 up point在lcd_rect内: 正常处理
                    MmiCreateSignal((uint16)state, sizeof(MmiTPPress), &sendSignal);
                    
                    sendSignal->sig.TpPress.x  = tp_ptr->x_key;
                    sendSignal->sig.TpPress.y  = tp_ptr->y_key;
                    sendSignal->sig.TpPress.gesture_type = tp_ptr->gesture_type;
                    sendSignal->send = P_TPP;
                    
                    MmiSendSignal(TASK_FL_ID, sendSignal);
                }
                else
                {
                    //down point在lcd_rect内 up point在lcd_rect外: 需要矫正坐标到lcd_rect内
                    if(up_point.x > lcd_rect.right)
                    {
                        up_point.x = lcd_rect.right;
                    }
                    if(up_point.y > lcd_rect.bottom)
                    {
                        up_point.y = lcd_rect.bottom;
                    }
                    
                    MmiCreateSignal((uint16)state, sizeof(MmiTPPress), &sendSignal);
                    
                    sendSignal->sig.TpPress.x  = up_point.x;
                    sendSignal->sig.TpPress.y  = up_point.y;
                    sendSignal->sig.TpPress.gesture_type = tp_ptr->gesture_type;
                    sendSignal->send = P_TPP;
                    
                    MmiSendSignal(TASK_FL_ID, sendSignal);
                }
            }
        }
        else
        {
            //SCI_TRACE_LOW:"TPCallBackFunc: the TP message is too much!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_752_112_2_18_2_0_28_266,(uint8*)"");
        }
    }
    else
    {
        if( !GUI_PointIsInRect(up_point, lcd_rect)
            && MMI_TP_UP == state )
        {
            //down point在lcd_rect外 up point在lcd_rect外: 匹配硬图标rect并映射为指定的物理按键消息
            hw_icon_num = MMK_GetShortcutIconNum();
            for(i = 0; i < hw_icon_num; i++)
            {
                if(MMK_IsPointInShortcutIconRect(up_point, i))
                {
                    key_single_code   = KPD_DOWN;
                    keyup_single_code = KPD_UP;
                    
                    //获取待虚拟的物理键值，mmiidle_hw_icon.c内配置
                    if( MMK_GetShortcutIconVirtualKeycode(i, &key_code)
                        && (0 != key_code))
                    {
                        //虚拟物理按键消息，KEY_DOWN/KEY_UP要成对
                        MmiCreateSignal(key_single_code, sizeof(MmiKeyPress), &KeysendSignal);
                        KeysendSignal->sig.keyPress.keyCode = key_code;
                        KeysendSignal->send = KPDSVR;
                        MmiSendSignal(TASK_FL_ID, KeysendSignal);
                        
                        //key up msg
                        MmiCreateSignal(keyup_single_code, sizeof(MmiKeyPress), &KeysendSignal);
                        KeysendSignal->sig.keyPress.keyCode = key_code;
                        KeysendSignal->send = KPDSVR;
                        MmiSendSignal(TASK_FL_ID, KeysendSignal);
                    }
                }
            }
        }
        else
        {
            //down point在lcd_rect外 up point在lcd_rect内: 无需处理丢弃
            //SCI_TRACE_LOW:"TPCallBackFunc: the TP message is invalid!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_791_112_2_18_2_0_28_267,(uint8*)"");
        }
    }
#endif //TOUCH_PANEL_HWICON_SUPPORT_NONE

#endif
}

/*****************************************************************************/
//  Description : handle gsensor call back function
//  Global resource dependence :                     
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
LOCAL void GsensorCallBackFunc(
                               BLOCK_ID id,    // Client thread ID
                               uint32   argc,  // Signal code
                               void     *argv  // Key Code
                               )
{
#ifdef MOTION_SENSOR_TYPE
    MmiSignalS* sendSignal   = PNULL;
    uint32      gsensor_code = (uint32)argv;

    if( MMK_CheckGsensorSignal( ) )
    {
        MmiCreateSignal( APP_GSENSOR, sizeof(MmiKeyPress), &sendSignal);

        sendSignal->sig.keyPress.keyCode = gsensor_code;
        sendSignal->send = P_APP;
        MmiSendSignal(TASK_FL_ID, sendSignal);

        //SCI_TRACE_LOW:"GsensorCallBackFunc: gsensor_code = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_821_112_2_18_2_0_28_268,(uint8*)"d", gsensor_code);
    }
    else
    {
        //SCI_TRACE_LOW:"GsensorCallBackFunc: the gsensor message is too much!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_825_112_2_18_2_0_28_269,(uint8*)"");
    }
#endif
}

/*****************************************************************************/
//  Description : register the MN signal and callback function  
//  Global resource dependence : 
//                             
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL void APP_RegisterPsService( void )
{
    //Register phone event
    SCI_RegisterMsg( MN_APP_PHONE_SERVICE, 
        (uint8)(EV_MN_APP_NETWORK_STATUS_IND_F & 0xff), /*lint !e778*/
//        (uint8)((MAX_MN_APP_PHONE_EVENTS_NUM - 1) & 0xff), 
        (uint8)((MAX_MN_APP_PHONE_EVENTS_NUM - 1) & 0xff),         
        SCI_NULL);
    
    //Register call event
    SCI_RegisterMsg( MN_APP_CALL_SERVICE, 
        (uint8)(EV_MN_APP_CALL_START_IND_F & 0xff), /*lint !e778*/
        (uint8)((MAX_MN_APP_CALL_EVENTS_NUM - 1) & 0xff), 
        SCI_NULL); 
    
    //Register sms event
    SCI_RegisterMsg( MN_APP_SMS_SERVICE, 
        (uint8)(EV_MN_APP_SMS_READY_IND_F & 0xff), /*lint !e778*/
        (uint8)((MAX_MN_APP_SMS_EVENTS_NUM - 1) & 0xff), 
        SCI_NULL); 
    
    //Register smscb event
    SCI_RegisterMsg( MN_APP_SMSCB_SERVICE, 
        (uint8)(EV_MN_APP_SMSCB_MSG_IND_F & 0xff), /*lint !e778*/
        (uint8)((MAX_MN_APP_SMSCB_EVENTS_NUM - 1) & 0xff), 
        SCI_NULL);
    
    //Register ss event
    SCI_RegisterMsg( MN_APP_SS_SERVICE, 
        (uint8)(EV_MN_APP_BEGIN_REGISTER_SS_F & 0xff), /*lint !e778*/
        (uint8)((MAX_MN_APP_SS_EVENTS_NUM - 1) & 0xff), 
        SCI_NULL); 

    //Register SIM event
    SCI_RegisterMsg( MN_APP_SIM_SERVICE, 
        (uint8)(EV_MN_APP_MSISDN_UPDATE_CNF_F & 0xff), /*lint !e778*/
        (uint8)((MAX_MN_APP_SIM_EVENTS_NUM - 1) & 0xff), 
        SCI_NULL); 

    //Register STK event
    SCI_RegisterMsg( MN_APP_SIMAT_SERVICE,
        (uint8)(EV_MN_APP_SIMAT_DISPLAY_TEXT_IND_F & 0xff), /*lint !e778*/
        (uint8)((SIMAT_EVENT_NUM - 1) & 0xff), 
        SCI_NULL );

    SCI_RegisterMsg( MN_APP_SIMAT_SERVICE,
        (uint8)(EV_MN_APP_REGISTER_SS_CNF_F & 0xff), 
        (uint8)((MAX_MN_APP_SS_EVENTS_NUM - 1) & 0xff), 
        SCI_NULL );

    //add by figo.feng 2004.09.24
    SCI_RegisterMsg(MN_APP_GPRS_SERVICE,
                (uint8)(EV_MN_APP_SET_PDP_CONTEXT_CNF_F & 0xff),/*lint !e778*/
                (uint8)((MAX_MN_APP_GPRS_EVENTS_NUM - 1) & 0xff),
                SCI_NULL); 
    //for http
//    SCI_RegisterMsg(MN_APP_GPRS_SERVICE,
//                (uint8)(EV_MN_APP_SET_PDP_CONTEXT_CNF_F & 0xff),
//                (uint8)((MAX_MN_APP_GPRS_EVENTS_NUM - 1) & 0xff),
//                SCI_NULL);

}


/*****************************************************************************/
//  Description : register the REF signal and callback function  
//  Global resource dependence : 
//                             
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL void APP_RegisterRefService( void )
{
    //Register key event
    SCI_RegisterMsg( KEYPAD_SERVICE, 
        (uint8)(KPD_DOWN & 0xff), 
        (uint8)(KPD_UP & 0xff), 
        KeypadCallBackFunc);

#ifdef TOUCH_PANEL_SUPPORT
    //Register TP event
    SCI_RegisterMsg( TP_SERVICE, 
        (uint8)TP_DOWN, 
        (uint8)(TP_MSG_MAX - 1), 
        TPCallBackFunc);
#endif

#ifdef MOTION_SENSOR_TYPE
    //Register key event
    SCI_RegisterMsg( MSENSOR_SERVICE, 
        (uint8)(MSENSOR_TYPE_ACCELEROMETER & 0xff), 
        (uint8)((MSENSOR_TYPE_MAX - 1) & 0xff), 
        GsensorCallBackFunc);
#endif
    
    //Register TIME and ALARM event
    SCI_RegisterMsg( TIME_SERVICE, 
               (uint8)(MISC_MSG_RTC_MIN & 0xff), 
               (uint8)((MISC_MSG_MAX - 1) & 0xff),      
               SCI_NULL);    
#ifdef MMI_DUAL_BATTERY_SUPPORT
    //Register CHARGE event
    SCI_RegisterMsg(DBAT_SERVICE, 
                (uint8)((DBAT_UPDATE_STATE_IND) & 0xff), 
                (uint8)((DBAT_MSG_MAX_NUM - 1) & 0xff), 
                DualBatteryCallBackFunc);
#else
    //Register CHARGE event
    SCI_RegisterMsg( CHR_SERVICE, 
                (uint8)((CHR_CAP_IND) & 0xff), 
                (uint8)((CHR_MSG_MAX_NUM - 1) & 0xff), 
                ChargeCallBackFunc);
#endif 
//Register UDISK event
#ifndef WIN32 // @wancan.you udisk
    SCI_RegisterMsg( USB_SERVICE, 
                (uint8)((USB_PLUGIN) & 0xff), 
                (uint8)((USB_MSG_MAX - 1) & 0xff), 
                UPMCallBackFunc);
#endif
}

#ifdef MMI_DUAL_BATTERY_SUPPORT
/*****************************************************************************/
//  Description : handle charge call back function
//  Global resource dependence :                     
//  Author: renyi.hu
//  Note: 
/*****************************************************************************/
LOCAL void DualBatterySendMsgToApp(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr,  // data follow the even
                              uint16    signal_code
                              )
{
    uint16                signal_size = 0;  
    MmiSignalS            *signal_ptr = PNULL; 

    signal_size = sizeof(MmiSignalS);

    MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,SCI_IdentifyThread());
    signal_ptr->sig.otherTaskMsg.param_ptr=param_ptr;
    signal_ptr->sig.otherTaskMsg.size_of_param=sizeof(void*);
    // Send the signal to the dedicated task's queue
    MMI_SEND_SIGNAL((MmiSignalS*)signal_ptr, taskid);
}

/*****************************************************************************/
//  Description : handle charge call back function
//  Global resource dependence :                     
//  Author: renyi.hu
//  Note: 
/*****************************************************************************/
LOCAL void DualBatteryCallBackFunc(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr  // data follow the even
                              )
{
    //uint16                signal_size = 0;  
    uint16                signal_code = 0;
    //xSignalHeaderRec      *signal_ptr = PNULL;

    switch (event_id)
    {
    case DBAT_UPDATE_STATE_IND :
        //SCI_TRACE_LOW:"Receive DBAT_UPDATE_STATE_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_996_112_2_18_2_0_29_270,(uint8*)"");
        signal_code = DBAT_MSG_UPDATE_STATE_IND;
        break;
    case DBAT_BAT_PLUGIN_IND :
        //SCI_TRACE_LOW:"Receive DBAT_BAT_PLUGIN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1000_112_2_18_2_0_29_271,(uint8*)"");
        signal_code = DBAT_MSG_BAT_PLUGIN_IND;
        break;
   case DBAT_BAT_PLUGOUT_IND :
        //SCI_TRACE_LOW:"Receive DBAT_BAT_PLUGOUT_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1004_112_2_18_2_0_29_272,(uint8*)"");
        signal_code = DBAT_MSG_BAT_PLUGOUT_IND;
        break;
   case DBAT_SWITCH_BAT_IND :
        //SCI_TRACE_LOW:"Receive DBAT_SWITCH_BAT_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1008_112_2_18_2_0_29_273,(uint8*)"");
        signal_code = DBAT_MSG_SWITCH_BAT_IND;
        break;
   case DBAT_CHARGE_START_IND :
        //SCI_TRACE_LOW:"Receive DBAT_CHARGE_START_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1012_112_2_18_2_0_29_274,(uint8*)"");
        signal_code = DBAT_MSG_CHARGE_START_IND;
        break;
    case DBAT_WARNING_IND :
        //SCI_TRACE_LOW:"Receive DBAT_WARNING_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1016_112_2_18_2_0_29_275,(uint8*)"");
        signal_code = DBAT_MSG_WARNING_IND;
        break;
     case DBAT_SHUTDOWN_IND :
        //SCI_TRACE_LOW:"Receive DBAT_SHUTDOWN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1020_112_2_18_2_0_29_276,(uint8*)"");
        signal_code = DBAT_MSG_SHUTDOWN_IND;
        break;
     case DBAT_CHARGE_FINISH :
        //SCI_TRACE_LOW:"Receive DBAT_CHARGE_FINISH"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1024_112_2_18_2_0_29_277,(uint8*)"");
        signal_code = DBAT_MSG_CHARGE_FINISH;
        break;
     case DBAT_CHARGE_DISCONNECT :
        //SCI_TRACE_LOW:"Receive DBAT_CHARGE_DISCONNECT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1028_112_2_18_2_0_29_278,(uint8*)"");
        signal_code = DBAT_MSG_CHARGE_DISCONNECT;
        break;
     case DBAT_CHARGE_FAULT :
        //SCI_TRACE_LOW:"Receive DBAT_CHARGE_FAULT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1032_112_2_18_2_0_29_279,(uint8*)"");
        signal_code = DBAT_MSG_CHARGE_FAULT;
        break;


    default :
        //SCI_TRACE_LOW:"DualBatteryCallBackFunc:event_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1038_112_2_18_2_0_29_280,(uint8*)"d",event_id);
        return;
    }

    DualBatterySendMsgToApp(taskid,event_id,param_ptr,signal_code);
}
#endif
/*****************************************************************************/
//  Description : handle charge call back function
//  Global resource dependence :                     
//  Author: Jassmine.Meng
//  Note: 
/*****************************************************************************/
LOCAL void ChargeCallBackFunc(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr  // data follow the even
                              )
{
    uint16                signal_size = 0;  
    uint16                signal_code = 0;
    xSignalHeaderRec      *signal_ptr = PNULL;

    switch (event_id)
    {
    case CHR_CAP_IND :
        //SCI_TRACE_LOW:"Receive CHR_CAP_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1062_112_2_18_2_0_29_281,(uint8*)"");
        signal_code = CHR_MSG_CAP_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_CHARGE_START_IND:
        //SCI_TRACE_LOW:"Receive CHR_CHARGE_START_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1073_112_2_18_2_0_29_282,(uint8*)"");
        signal_code = CHR_MSG_CHARGE_START_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_CHARGE_END_IND:
        //SCI_TRACE_LOW:"Receive CHR_CHARGE_END_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1084_112_2_18_2_0_29_283,(uint8*)"");
        signal_code = CHR_MSG_CHARGE_END_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_WARNING_IND:
        //SCI_TRACE_LOW:"Receive CHR_WARNING_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1095_112_2_18_2_0_29_284,(uint8*)"");
        signal_code = CHR_MSG_WARNING_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_SHUTDOWN_IND:
        //SCI_TRACE_LOW:"Receive CHR_SHUTDOWN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1106_112_2_18_2_0_29_285,(uint8*)"");
        signal_code = CHR_MSG_SHUTDOWN_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_CHARGE_FINISH:
        //SCI_TRACE_LOW:"Receive CHR_CHARGE_FINISH"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1117_112_2_18_2_0_29_286,(uint8*)"");
        signal_code = CHR_MSG_CHARGE_FINISH_IND;
        signal_size = sizeof(xSignalHeaderRec);
        
        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_CHARGE_DISCONNECT:
        //SCI_TRACE_LOW:"Receive CHR_CHARGE_DISCONNECT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1128_112_2_18_2_0_29_287,(uint8*)"");
        signal_code = CHR_MSG_CHARGE_DISCONNECT_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    case CHR_CHARGE_FAULT:
        //SCI_TRACE_LOW:"Receive CHR_CHARGE_FAULT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1139_112_2_18_2_0_29_288,(uint8*)"");
        signal_code = CHR_MSG_CHARGE_FAULT_IND;
        signal_size = sizeof(xSignalHeaderRec);

        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,CHR_SVR);

        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
        break;

    default :
        //SCI_TRACE_LOW:"ChargeCallBackFunc:event_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1150_112_2_18_2_0_29_289,(uint8*)"d",event_id);
        break;
    }
}

/*****************************************************************************/
//  Description : init MMK
//                MMI Message
//  Global resource dependence : 
//  Author:Louis wei
//  Modify: Tracy Zhang
//  Note:
/*****************************************************************************/
void MMK_Init(void)
{
    MMK_InitKBD();
    MMK_InitTp();

#ifdef WIN32
    //init lcd
    LCD_Init();
#endif

    MMIDEFAULT_InitDefault(TRUE);

    // 初始化消息队列
    MMK_InitMSGQueue(0);

    // 初始化定时器
    MMK_InitTimer();

    // init the control class and the LCD    
    //MMK_InitControlClass();    
    MMK_InitSystem();

    GUILCD_Init(GUI_MAIN_LCD_ID);

    //初始化全局角度
    MMK_SetScreenAngle( GUILCD_GetInitAngle( GUI_MAIN_LCD_ID ) );
    
#ifndef SUBLCD_SIZE_NONE
    GUILCD_Init(GUI_SUB_LCD_ID);
#endif
    
    //init update
    MMITHEME_InitUpdate();

    //init layer
#ifdef UILAYER_MAINLAYER_ARGB888_TYPE
    UILAYER_InitSystem((GUI_COLOR_T*)s_mainlayer_buf, MMI_MAINSCREEN_WIDTH_RESIZE*MMI_MAINSCREEN_HEIGHT_RESIZE*sizeof(uint32)/*sizeof(s_mainlayer_buf)*/, DATA_TYPE_ARGB888);
#else
    UILAYER_InitSystem(s_mainlayer_buf, MMI_MAINSCREEN_WIDTH_RESIZE*MMI_MAINSCREEN_HEIGHT_RESIZE*sizeof(GUI_COLOR_T)/*sizeof(s_mainlayer_buf)*/, DATA_TYPE_RGB565);
#endif
}

/*****************************************************************************/
//  Description : 初始化MMI中与PS有关的所有应用
//  Global resource dependence : 
//  Author: Tracy Zhang
//  Note:
/*****************************************************************************/
void APP_Init(POWER_RESTART_CONDITION_E   restart_condition)
{
#ifdef MULTI_SIM_SYS_QUAD//MS00226932
    MNPHONE_SetCommonCtrl();
#endif

    //MMITHEME_IniCurThemeFontColor(MMIAPISET_GetCurrentStaticThemeID());

    MMIAPIFMM_InitFileTasks();

    //init sd card
    MMIAPISD_Init();

    // init hidden partion
    MMIAPIUDISK_SysDeviceinit();
    //delete temp filearray files
    MMIAPIFILEARRAY_Init();
#ifdef SQLITE_SUPPORT
	sqlite3_soft_heap_limit(SQLITE_HEAP_LIMIT);
#endif

#ifdef MULTI_THEME_SUPPORT
    MMIAPISET_IniCurrentTheme();
#endif 
#ifdef DYNAMIC_MODULE_SUPPORT
    //caf add    
    MMK_LoadDynamicModInfo();
#endif
    // 初始化开机和IDLE界面的APPLICATION
    MMIAPIPHONE_AppInit();

    //init key lock model
    MMIAPIKL_Init();

    //init ring and setting
    MMIAPISET_AllInit();

    //audio init
    MMISRVMGR_Init();
    MMISRVAUD_Init();
    
    //@zhaohui add,测试用还需进一步完善
    //init vt
#ifdef VT_SUPPORT
    MMIAPIVT_Init();
#endif

    //init the main application
    MMIAPIIDLE_init();

#if defined(MMI_MSD_SUPPORT)
    MMIAPIMSD_Init();
#endif

    //MMIAPIDV_Init();
    MMIAPICC_Init();
    
    MMIAPIDM_Init();
    MMIAPISMS_AppInit();

    //PHONEBOOK
    MMIAPIPB_InitApp();

    // STK
    MMIAPISTK_AppInit();
    // add by allen 2004.08.18 end
    MMIAPIENG_AppInit();

    MMIAPIENGTD_AppInit();

    //alarm, schedule, autopower on/off init
    MMIALM_Init();

    // accessory init for alarm EFS data   -- add by taul
    MMIAPIACC_Init();  

    //初始化mp3
    MMIAPIMP3_Init();  //init mp3 when entering mp3 menu

    //初始化my music
    MMIAPIMP3_InitMyMusic();

    //MMIAPIFMM_InitFolders();
    
    //init pic viewer
    MMIAPIPICVIEW_Init();

#ifdef VIDEO_PLAYER_SUPPORT
    MMIAPIVP_Init();
#endif

    
    //
    MMIAPICONNECTION_Init();

    //init mms module 
    MMIAPIMMS_AppInit();

    MMIAPIFM_Init();   //FM init

#ifdef MMI_LDN_SUPPORT
    MMIAPICL_InitCallInfoRecord();
#endif

    // init java module. @arthur.peng
#ifdef JAVA_SUPPORT
    MMIAPIJAVA_Init();
#endif

#ifdef TOUCHPANEL_TYPE		
    MMIAPITP_COORDINATE_Init(); // 坐标值校正
#endif
    MMIAPIOCR_Init();
    //MMIAPIVCARD_Init();
//macro replace
//#ifdef MMI_STREAMING_ENABLE
#ifdef STREAMING_SUPPORT_NEXTREAMING
    MMIST_Init();   //byte
#endif
#ifdef MMI_ENABLE_DCD
    MMIAPIDCD_AppInit();
#endif

#ifdef CMCC_UI_STYLE
    MMIAPICS_AppInit();
#endif

#ifdef  BLUETOOTH_SUPPORT
    if (restart_condition != RESTART_BY_CHARGE)
    {
        //Blutooth module initialization
        MMIAPIBT_AppInit(); 
    }  
#ifndef WIN32
	else
	{
		//charge start, need not start BT, only init BT chip
		BT_Half_init();
	}
#endif
#endif
    MMIAPIPHONE_InitTDOrGsm();

    MMIAPITVOUT_Init();
    
#ifdef WRE_SUPPORT
    //  WRE_InitDynamicMenu(1,20);
    MMIAPIWRE_Init();
#endif

    //MMIFMM_ClearTempFiles();
#ifdef EBOOK_SUPPORT
    MMIAPIEBOOK_Init();//ebook初始化
#endif
    MMIAPIPIM_InitApp();
    MMIAPIKUR_Init();  //init Kuro
    MMIAPIMTV_Init();
    MMIAPIATV_Init();  //init atv

#ifdef MMI_GPRS_SUPPORT
    MMIAPIPDP_AppInit();
#endif
#ifdef BROWSER_SUPPORT
    MMIAPIBROWSER_InitApp();
#endif

#ifdef WEATHER_SUPPORT   
    MMIAPIWEATHER_InitApp();
#endif

#ifdef DL_SUPPORT
    MMIAPIDL_InitDownload();
#endif

#ifdef FDN_SUPPORT
    MMIAPIFDN_Init();
#endif

#ifdef ENG_SUPPORT
    MMIAPIENG_SetPktOutPut();
#endif

#ifdef TTS_SUPPORT
    MMIAPITTS_Init();
#endif

#ifdef MMIEMAIL_SUPPORT
    MMIAPIEMAIL_InitApp();
#endif
#ifdef PUSH_EMAIL_SUPPORT  
    mail_CoreInit();
    MMIAPIMAIL_Init();
#endif /* PUSH_EMAIL_SUPPORT  */


#ifdef MOBILE_VIDEO_SUPPORT
    MMIAPIMV_Init();
#endif

//init drm network time
#ifdef DRM_SUPPORT
    MMIAPIDRM_Init();
#endif

#ifdef IKEYBACKUP_SUPPORT
    MMIAPIIKB_Init();
#endif

#ifdef DYNAMIC_MAINMENU_SUPPORT
    MMIMENU_DyInit();
#endif
#ifdef PDA_UI_DROPDOWN_WIN
    MMIDROPDOWNWIN_Init();
#endif	

#ifdef MMIUZONE_SUPPORT
    MMIAPIUZONE_Init();
#endif	

#ifdef MMI_CSTAR_UDB_SUPPORT
    MMIAPIIM_Init();
#endif

/* BEGIN: Added by George.Liu 01526, 2010/8/2   PN:MergeToMw1029forCustom */
#if 0
#ifdef WIFI_SUPPORT
    MMIAPIWIFI_Init();
#endif
#endif
/* END:   Added by George.Liu 01526, 2010/8/2   PN:MergeToMw1029forCustom */
//===============Met Mex Start==============
#ifdef MET_MEX_SUPPORT
{
	//spread开机初始化
	MetMex_SpreadStartupInit();
}
#endif
//===============Met Mex End==============

#ifdef MRAPP_SUPPORT
	MMIMRAPP_InitSetting();
#endif

#ifdef QBTHEME_SUPPORT
	MMIAPIQBTHEME_Init();
#endif
#ifdef HOME_LOCATION_INQUIRE_SUPPORT_TCARD            
    MMIAPIPHONESEARCH_RegBrowserCB();
#endif

    //放在初始化最后面
#ifdef CSC_XML_SUPPORT
    MMIAPICUS_ParseCSCXML();
#endif
}

/*****************************************************************************/
//  Description : initial common menu group
//  Global resource dependence : 
//  Author:wancan.you
//  Modify:
//  Note: 
/*****************************************************************************/
LOCAL void RegComMenuGroup(void)
{
    MMI_RegMenuGroup(MMI_MODULE_COMMON, menu_table);
}

/*****************************************************************************/
//  Description : initial common module
//  Global resource dependence : 
//  Author:wancan.you
//  Modify:
//  Note: 
/*****************************************************************************/
LOCAL void InitCommonModule(void)
{
    RegComMenuGroup(); //初始化common menu
    MMI_RegCommonNv();  //初始化 common nv
    MMICOMMON_RegWinIdNameArr();    //初始化win id name数组
    MMIAPIPHONE_CommonMduRegAppletInfo();
}

/*****************************************************************************/
//  Description : initial mmi modudle
//  Global resource dependence : 
//  Author:wancan.you
//  Modify:
//  Note: 所有新增模块的初始化都需要放在这里
/*****************************************************************************/
LOCAL void InitMMIModule(void)
{
    MMI_InitilizeWinIdNameArr();

    
    MMI_RegModuleNvStart();

    InitCommonModule();//初始化common模块相关的操作

#ifdef CLIPBOARD_SUPPORT
    MMIAPICLIPBRD_InitModule();
#endif

	MMIAPIIM_InitModule();

    MMIAPIFM_InitModule(); //Init FM module

    MMIAPIALM_InitModule();
#ifdef GAME_SUPPORT
    MMIAPIGAME_InitModule();
#endif
    MMIAPISET_InitModule();
    MMIAPIMP3_InitModule();
    MMIAPICC_InitModule();
#ifdef VT_SUPPORT
    MMIAPIVT_InitModule();   
#endif
    MMIAPIENG_InitModule(); 
#ifdef MMI_MOCOR_SAMPLE
    MMIAPISAMPLE_HELLO_ModuleInit();
#endif
    MMIAPIENGTD_InitModule(); 
    MMIAPIRECORD_InitModule();
    MMIAPIACC_InitModule();
    MMIAPISMS_InitModule();
#ifdef EBOOK_SUPPORT
    MMIAPIEBOOK_InitModule();   //@maggie add for ebook
#endif
#ifdef MMIEMAIL_SUPPORT
	 MMIAPIEMAIL_InitModule();
#endif

#ifdef  CAMERA_SUPPORT    
    MMIAPIDC_InitModule();
#endif
    
    MMIAPICONNECTION_InitModule();
    MMIAPIPB_InitModule();
    MMIAPIFMM_InitModule();
#ifdef JAVA_SUPPORT
    MMIAPIJAVA_InitModule();
#endif
    MMIAPIPIC_InitModule();
    MMIAPIPICVIEW_InitModule();
    MMIAPIBT_InitModule(); //Init bt module
    MMIAPITP_InitModule();
    
    MMIAPIKUR_InitModule(); 

   #ifdef MMI_ENABLE_DCD
      MMIAPIDCD_InitModule();
   #endif

   #ifdef CMCC_UI_STYLE
         MMIAPICS_InitModule();
   #endif
#ifdef DYNAMIC_MODULE_SUPPORT    
    MMIAPIDYNA_InitModule();
#endif

    MMIAPIPIM_InitModule();

    MMIAPIMTV_InitModule();
    MMIAPIATV_InitModule();

    MMIAPIDM_InitModule();

#ifdef BROWSER_SUPPORT
    MMIAPIBROWSER_InitModule();
#endif

#ifdef WEATHER_SUPPORT   
    MMIAPIWEATHER_InitModule();
#endif
#ifdef SXH_APPLE_SUPPORT
   MMIAPPLE_InitModule();
#endif

#ifdef DL_SUPPORT
    MMIAPIDL_InitModule();
#endif
#ifdef WIFI_SUPPORT
    MMIAPIWIFI_InitModule();
#endif

#ifdef SEARCH_SUPPORT
    MMIAPISEARCH_InitModule();
#endif

#ifdef VIDEO_PLAYER_SUPPORT
    MMIAPIVP_InitModule();
#endif

#ifdef ASP_SUPPORT
    MMIAPIASP_InitModule();
#endif

#ifdef MOBILE_VIDEO_SUPPORT
    MMIAPIMV_InitModule();
#endif

#if (defined MMIWIDGET_SUPPORT) || (defined MMI_GRID_IDLE_SUPPORT)
    MMIAPIWIDGET_InitModule();
#endif

#ifdef LIVE_WALLPAPER_FRAMEWORK_SUPPORT
    MMIAPILIVEWALLPAPER_InitModule();
#endif

#ifdef PUSH_EMAIL_SUPPORT   
    MMIAPIMAIL_InitModule();
#endif /* PUSH_EMAIL_SUPPORT  */

    //init drm nv 
#ifdef DRM_SUPPORT
    MMIAPIDRM_InitModule();
#endif

#ifdef WRE_SUPPORT
   MMIAPIWRE_InitModule();
#endif

#ifdef IKEYBACKUP_SUPPORT
    MMIAPIIKB_InitModule();
#endif

#ifdef PDA_UI_DROPDOWN_WIN
    MMIAPIDROPDOWNWIN_InitModule();
#endif

#ifdef MMI_KING_MOVIE_SUPPORT
    MMIAPIKM_InitModule();
#endif

#ifdef MCARE_V31_SUPPORT
#ifdef MMI_TASK_MANAGER
    McfIF_RegAppletInfo();//lint !e18
#endif
#endif

#ifdef MRAPP_SUPPORT
	MMIMRAPP_initModule();
#endif

#ifdef MMI_PIC_CROP_SUPPORT
    MMIAPIPICCROP_InitModule();
#endif

#ifdef CSC_SUPPORT
    MMIAPICUS_InitModule();
#endif
#ifdef SNS_SUPPORT
    MMISNS_ModuleInit();
#endif 
#ifdef QBTHEME_SUPPORT
	MMIAPIQBTHEME_InitModule();
#endif

#ifdef MMI_MEMO_SUPPORT
    MMIAPIMEMO_InitModule();
#endif

#ifdef MMIUZONE_SUPPORT
    MMIAPIUZONE_InitModule();
#endif

    MMI_RegModuleNvEnd();   
    
}

#if defined WIN32 || defined MMI_CHECK_MEMINFO_SUPPORT
#define MMI_CHECK_MEMINFO    MMI_CheckAllocatedMemInfo(_D_FILE_NAME, _D_FILE_LINE);
#else
#define MMI_CHECK_MEMINFO
#endif
/*****************************************************************************/
//  Description:    
//	Global resource dependence: 
//  Author: 		James.Zhang
//	Note:
/*****************************************************************************/
PUBLIC void MMI_CheckAllocatedMemInfo(
                                      const char* file_name_ptr,
                                      uint32 line
                                      )
{
#ifdef MEMORY_DEBUG_SUPPORT
#include "threadx_os.h"
#define MEM_DEBUG_END_FLAG   0xAA
    // Get the first useful pointer of the memory.
    MEM_HEADER_T    *tmpptr = PNULL;
    extern MEM_HEADER_T dbg_head;

    TX_INTERRUPT_SAVE_AREA
    TX_DISABLE
        
    tmpptr = dbg_head.succ;
    
    // Check if the pointer is valid
    while(tmpptr != &dbg_head)
    {
        uint8 flag = *(uint8 *)((uint8 *)tmpptr + tmpptr->size + sizeof(MEM_HEADER_T) );

        if ( flag != MEM_DEBUG_END_FLAG )
        {
            SCI_PAssert("MMI_CHECK_MEMINFO", file_name_ptr, line, 
                SCI_SaveAssertInfo("Memory memory_ptr 0x%8x has been destroied! It be malloced by %s on %d line ,size is %d",
                tmpptr,
                tmpptr->file_name,
                tmpptr->line,
                tmpptr->size ) );
        }
        
        // Point to the next
        tmpptr = tmpptr->succ;
    }

    TX_RESTORE
#endif  // End of __MEMORY_DEBUG__
}



#define MMI_TICKS_MIN 2

/*****************************************************************************/
//  Description : the function of mmi entry
//  Global resource dependence : 
//                              
//                             
//  Author:Louis.wei
//  Modify: Tracy Zhang
//  Note: 
/*****************************************************************************/
#ifdef WIN32
void APP_Task(
    uint32 argc, 
    void * argv
)
#else
THREAD_ENTRY(P_APP)
#endif
{
	BOOLEAN                     is_assert = FALSE;
    POWER_RESTART_CONDITION_E   restart_condition = RESTART_BY_NONE;
    MmiSignalS                  *receiveSignal ;
    MMI_MESSAGE_PTR_T           mmi_msg;            // 指向消息结构的指针

#ifdef ENG_SUPPORT    
    uint32 ticks1 = 0;
    uint32 ticks2 = 0;
    BOOLEAN is_log_on = FALSE;
#endif

#ifndef WIN32
	SWDG_OBJECT                 *watchdog_ptr = PNULL;  //监测mmi task出现死循环
    //SCI_TRACE_LOW:"APP_Task: begin init, time = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1746_112_2_18_2_0_30_290,(uint8*)"d", SCI_GetTickCount());
#endif

#ifndef WIN32
    if (CHNG_FREQ_REQ_HANDLER_NULL == s_mmi_chng_freq_req_handler)
    {
        s_mmi_chng_freq_req_handler =  CHNG_FREQ_GetReqHandler("mmi");
        SCI_ASSERT (CHNG_FREQ_REQ_HANDLER_NULL != s_mmi_chng_freq_req_handler); /*assert verified*/
    }   
#endif 

    MMI_SetFreq(FREQ_INDEX_APP_BOOT);

    //take resource in NAND to RAM. 
    // Be careful, this should be down before MMI use any kind of resource.
    
    //initial mmi module
    InitMMIModule();
      
    MMISET_GetCurrentLanguageType();
    /*MMI_LoadResource();*/

#ifdef DYNAMIC_MODULE_SUPPORT
    /* init caf module memory manager */
    MMKMNG_MemInit(ApplyForMemoryCallBackFunc, ReleaseMemoryCallBackFunc);
    //MMI_LoadResource();
#endif

    //caf add
    MMK_LoadStaticModInfo();

    //caf add
    MMIRES_ResourceInit();
    MMIRES_LoadAllStaticComResource();

    MMK_Init();

    MMIAPIPHONE_VirtualWinInit();

    //MMI_Enable3DMMI(TRUE);
    
#ifndef WIN32
    restart_condition = POWER_GetRestartCondition();
#else 
    restart_condition = RESTART_BY_POWER_BUTTON;
#endif // WIN32 
   
    //SCI_TRACE_LOW:"APP_Task: restart_condition is %d,time is %dms"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1796_112_2_18_2_0_30_291,(uint8*)"dd",restart_condition,SCI_GetTickCount());
    if (RESTART_BY_ASSERT == restart_condition)
    {
        is_assert = TRUE;
    }
    MMIAPIPHONE_HandleUdiskException(is_assert);
    // 根据系统启动的不同条件对服务进行注册7
    switch (restart_condition)
    {
    case RESTART_BY_POWER_BUTTON:
    case RESTART_BY_SOFTWARE:            
        // 注册REF服务
        APP_RegisterRefService();

        // 注册PS服务
        APP_RegisterPsService();

        //保证MMISMS NV初始化在BASE进行初始化之前
        MMIAPISMS_InitForPS();

        MMISUB_Init();
        MMI_TraceAutoBackLightStatus(MMIDEFAULT_GetBackLightStateEx());

        // 开机界面初始化
        MMIAPIPHONE_NormalInit();

        // MMI所有的APP初始化 
        APP_Init(restart_condition);

        //update backlight
        MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
        
        break;
        
    case RESTART_BY_ALARM:
        // 注册REF服务
        APP_RegisterRefService();

        MMIAPIPHONE_SetStartUpCondition(STARTUP_ALARM);

        // MMI所有的APP初始化 
        APP_Init(restart_condition);
        MMISUB_Init();
        // 闹铃界面初始化
        MMIAPIPHONE_AlarmInit();

        MMI_RestoreFreq();
        break;
        
    case RESTART_BY_CHARGE:
        // 注册REF服务
        APP_RegisterRefService();

        // MMI所有的APP初始化 
        APP_Init(restart_condition);

        MMI_TraceAutoBackLightStatus(MMIDEFAULT_GetBackLightStateEx());
        //update backlight
        MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
		MMISUB_Init();
        // 充电界面初始化
        MMIAPIPHONE_ChargeInit();

        MMI_RestoreFreq();
        break;
        
    case RESTART_BY_ASSERT:
        // 注册REF服务
        APP_RegisterRefService();

        // 注册PS服务
        APP_RegisterPsService();

        //保证MMISMS NV初始化在BASE进行初始化之前
        MMIAPISMS_InitForPS();

        MMIAPIPHONE_StartUpPsAccordingToSetting(TRUE, TRUE);
        
        // MMI所有的APP初始化 
        APP_Init(restart_condition);
        
        //open lcd backlight
        MMIDEFAULT_InitDefault(FALSE);
        MMIDEFAULT_TurnOnLCDBackLight();
        
        MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());

        //assert初始化，直接进入idle界面
        MMIAPIPHONE_AssertInit();
		MMISUB_Init();
        MMI_RestoreFreq();
        break;

    case RESTART_BY_NONE:
    default:
        MMIDEFAULT_TurnOffBackLight();

        MMI_RestoreFreq();

        POWER_PowerOff();       // 关机
        break;
    }

    #ifndef WIN32
    //SCI_TRACE_LOW:"APP_Task: end init, time = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_1900_112_2_18_2_0_31_292,(uint8*)"d", SCI_GetTickCount());
    #endif

    MMIDEFAULT_AllowTurnOffBackLight(TRUE);
    if (RESTART_BY_ASSERT != restart_condition)
    {
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);
    }

    MMI_CheckAutoTestWinId();
    
#ifndef WIN32    
	/* Register app task to soft watchdog monitor */
	watchdog_ptr = SWDG_RegTask("APP", 180000);
	SCI_ASSERT(SCI_NULL != watchdog_ptr);	 /*assert verified*/
#ifdef WRE_SUPPORT
    MMIWRE_SetWatchDogPtr((void *)watchdog_ptr);
#endif
#ifdef BROWSER_SUPPORT_DORADO
#ifdef JS_SUPPORT
    MMIAPIDORADO_SetWatchDogPtr((void *)watchdog_ptr);
#endif
#endif
#endif
    
    while (1)/*lint !e716*/
    {
#ifdef ENG_SUPPORT
        is_log_on = MMIAPIENG_GetIsScreenLogOn();
#endif

        if (MMK_GetMSGQueue(&mmi_msg))
        {
#ifdef ENG_SUPPORT
            if ( is_log_on )
            {
                ticks1 = SCI_GetTickCount();
            }
#endif
            MMI_CHECK_MEMINFO

            MMK_DispatchMSGQueue(mmi_msg);

            MMI_CHECK_MEMINFO

#ifdef ENG_SUPPORT
            if ( is_log_on )
            {
                ticks2 = SCI_GetTickCount() - ticks1;
                
                //screen log
                MMIAPIENG_PrintScreenLog();
            }
            
            if ( is_log_on )
            {
                ticks1 = SCI_GetTickCount();
            }
#endif
            MMITHEME_UpdateRect();

#ifdef ENG_SUPPORT
            if ( is_log_on )
            {
                ticks2 += SCI_GetTickCount() - ticks1;
                
                if ( ticks2 > MMI_TICKS_MIN )
                {
                    MMIAPIENG_SetScreenTag( "APP_INT(ms)", MMIENG_SCREEN_LOG_APP, ticks2 );
                }
            }
#endif
            MMI_ChangeFreqByQuickKey();

            MMK_FreeMSG(mmi_msg);
        }
        else
        {
#ifndef WIN32            
		    /* Set task status suspending*/
		    SWDG_SetStatus(watchdog_ptr, SCI_FALSE);
#endif
            //Receive the system message(include the PS message and the REF message)
            MmiReceiveSignal(P_APP, &receiveSignal); 
#ifndef WIN32
		    /* Fresh task tick */	
		    SWDG_FreshTick(watchdog_ptr);
		    /* Set task status running */
		    SWDG_SetStatus(watchdog_ptr, SCI_TRUE);
#endif
            
#ifdef ENG_SUPPORT
            if ( is_log_on )
            {
                ticks1 = SCI_GetTickCount();
            }
#endif

                MMI_CHECK_MEMINFO

#ifdef ATEST_SUPPORT		
			ATest_DispatchExtSig(&receiveSignal);
#endif //ATEST_SUPPORT

                MMK_DispatchExtSig(&receiveSignal);

                MMI_CHECK_MEMINFO

#ifdef ENG_SUPPORT
            if ( is_log_on )
            {
                ticks2 = SCI_GetTickCount() - ticks1;
                
                //screen log
                MMIAPIENG_PrintScreenLog();
            }
            
            if ( is_log_on )
            {
                ticks1 = SCI_GetTickCount();
            }
#endif
            MMITHEME_UpdateRect();

#ifdef ENG_SUPPORT
            if ( is_log_on )
            {
                ticks2 += SCI_GetTickCount() - ticks1;
                
                if ( ticks2 > MMI_TICKS_MIN )
                {
                    MMIAPIENG_SetScreenTag( "APP_EXT(ms)", MMIENG_SCREEN_LOG_APP, ticks2 );
                }
            }
#endif
            MMI_ChangeFreqByQuickKey();

            MmiDestroySignal(&receiveSignal);
       
        }// end of if
    }  // end while
}  //End of mmi_Task

/*****************************************************************************/
//  Description : handle udisk call back function
//  Global resource dependence :                          
//  Author: Jassmine.Meng
//  Note: // @ wancan.you udisk
/*****************************************************************************/
LOCAL void UPMCallBackFunc(
                              uint32    taskid,     // the client's task id
                              uint32    event_id,   // the event type
                              void      *param_ptr  // data follow the even
                              )
{
    uint16                signal_size = 0;  
    uint16                signal_code = 0;
    xSignalHeaderRec      *signal_ptr = PNULL;

    switch (event_id)
    {
    case USB_PLUGIN :
        //SCI_TRACE_LOW:"Receive UPM_PLUGIN"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2060_112_2_18_2_0_31_293,(uint8*)"");
        signal_code = USB_PLUGIN_IND;
        break;

    case USB_PLUGOUT :
        //SCI_TRACE_LOW:"Receive UPM_PLUGOUT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2065_112_2_18_2_0_31_294,(uint8*)"");
        signal_code = USB_PLUGOUT_IND;
        break;

    case USB_READY:
        //SCI_TRACE_LOW:"Receive UPM_READY"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2070_112_2_18_2_0_31_295,(uint8*)"");
        signal_code = USB_READY_IND;
        break;
    case USB_SERVICE_UDISK_STARTED:
		//udisk started
        signal_code = USB_SERVICE_UDISK_STARTED_IND;
		break;
    case USB_SERVICE_UDISK_STOPPED:
		//udisk stopped
        signal_code = USB_SERVICE_UDISK_STOPPED_IND;
		break;
	case USB_SERVICE_UCOM_STARTED:
        //pc data communicate started
        signal_code = USB_SERVICE_UCOM_STARTED_IND;
		break;
	case USB_SERVICE_UCOM_STOPPED:
		//pc data communicate stopped
		signal_code = USB_SERVICE_UCOM_STOPPED_IND;
		break;
	case USB_SERVICE_UPCC_STARTED:
		//pc camera started
        signal_code = USB_SERVICE_UPCC_STARTED_IND;
		break;
	case USB_SERVICE_UPCC_STOPPED:
		//pc camera stopped
		signal_code = USB_SERVICE_UPCC_STOPPED_IND;
		break;
	case USB_SERVICE_LOG_STARTED:
		//usb log started
		signal_code = USB_SERVICE_LOG_STARTED_IND;
		break;
	case USB_SERVICE_LOG_STOPPED:
		//usb log stopped
		signal_code = USB_SERVICE_LOG_STOPPED_IND;
		break;

    case USB_ERROR:
        //SCI_TRACE_LOW:"Receive UPM_ERROR"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2107_112_2_18_2_0_31_296,(uint8*)"");
        signal_code = USB_ERROR_IND;
        break;

    default :
		signal_code = 0;
        //SCI_TRACE_LOW:"UPMCallBackFunc:event_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2113_112_2_18_2_0_31_297,(uint8*)"d",event_id);
        break;
    }
	
	if(signal_code != 0)
	{
        signal_size = sizeof(xSignalHeaderRec);
		
        MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,UDISK);
		
        // Send the signal to the dedicated task's queue
        MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, taskid);
	}
}

#ifndef WIN32
/*****************************************************************************/
//  Description : get freg handler
//  Global resource dependence :                          
//  Author: ryan.xu
//  Note: 
/*****************************************************************************/
PUBLIC uint32 MMI_GetFreqHandler(void)
{
    return s_mmi_chng_freq_req_handler;

}
#endif

/*****************************************************************************/
//  Description : change freq by quick key
//  Global resource dependence :                          
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
void MMI_ChangeFreqByQuickKey(void)
{
    if(!MMIAPIMP3_IsPlayerActive()
        && !MMIAPIMTV_IsMTVRunning())
    {
        //快速按键的时候提高频率来加快响应,按键变慢则恢复正常值
        if (MMK_IsQuickKey())
        {
            if (!s_is_high_freq_by_quick_key)
            {
                s_is_high_freq_by_quick_key = TRUE;
#ifndef WIN32            
                CHNG_FREQ_SetArmClk(MMI_GetFreqHandler(), FREQ_INDEX_MP3);    
#endif
            }
        }
        else
        {
            if (s_is_high_freq_by_quick_key)
            {
                s_is_high_freq_by_quick_key = FALSE;
#ifndef WIN32
                CHNG_FREQ_RestoreARMClk(MMI_GetFreqHandler());
#endif
            }
        }            
    }
}

//use for virtual sublcd
/*****************************************************************************/
//  Description :get flip status
//  Global resource dependence :                     
//  Author:
//  Note: TURE: mainlcd close; FALSE: mainlcd opened
/*****************************************************************************/
PUBLIC BOOLEAN MMI_IsFlipClosed(void)
 {

	//SCI_TRACE_LOW:"[MMIMAIN] s_is_flip %d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2181_112_2_18_2_0_31_298,(uint8*)"d", s_is_flip);
	return s_is_flip;
 }

#ifdef WIN32
/*****************************************************************************/
// 	Description : set sub lcd flip flag
//	Global resource dependence : 
//  Author: ivan.zhang
//	Note:
/*****************************************************************************/
LOCAL void SetFlipFlag(BOOLEAN flapisOpen)
{
	s_flip_is_open = flapisOpen;
}

/*****************************************************************************/
// 	Description : get sub lcd flip flag
//	Global resource dependence : 
//  Author: ivan.zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN GetFlipFlag(void)
{
	return s_flip_is_open;
}

/*****************************************************************************/
// 	Description : set earphone insert flag
//	Global resource dependence : 
//  Author: ivan.zhang
//	Note:
/*****************************************************************************/
LOCAL void SetEarphoneFlag(BOOLEAN earphoneIsInsert)
{
	s_earphone_is_insert = earphoneIsInsert;
}

/*****************************************************************************/
// 	Description : get earphone insert flag
//	Global resource dependence : 
//  Author: ivan.zhang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMI_GetEarphoneFlag(void)
{
	return s_earphone_is_insert;
}
/*****************************************************************************/
// 	Description :  sub lcd flip and handset key on PC simulator 
//	Global resource dependence : 
//  Author: ivan.zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SpecialKeyHandle(uint16 single_code, uint32 key_code)
{
	switch(key_code) 
	{
	case SCI_VK_FLIP:
	{
		if (TRUE == GetFlipFlag())	//翻盖是打开的，只创建合盖消息
		{
			if (KPD_DOWN == single_code)
			{
				SetFlipFlag(FALSE); //合上翻盖
				s_flip_msg_is_ok = TRUE;
			}
			else
			{
				//SCI_TRACE_LOW:"SpecialKeyHandle: flip is open already!"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2240_112_2_18_2_0_31_299,(uint8*)"");
				return TRUE;
			}
		}
		else	//翻盖是合上的，只创建开盖消息
		{
			if (!s_flip_msg_is_ok)	//翻盖消息还没处理
			{
				if (KPD_UP == single_code)
				{
					SetFlipFlag(TRUE); //打开翻盖
				}
				else
				{
					//SCI_TRACE_LOW:"SpecialKeyHandle: flip is close already!"
					SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2254_112_2_18_2_0_31_300,(uint8*)"");
					return TRUE;
				}
			}
			else
			{
				s_flip_msg_is_ok = FALSE;	//reset此标志
				return TRUE;
			}
		}
	}
	break;
	case SCI_VK_HEADSET_DETECT:
	{
		if (FALSE == MMI_GetEarphoneFlag())	//耳机未插入
		{
			if (KPD_DOWN == single_code)
			{
				SetEarphoneFlag(TRUE); 
				s_earphone_msg_is_ok = TRUE;
				MMISUB_UpdateDisplaySubLcd();
			}
			else
			{
				//SCI_TRACE_LOW:"SpecialKeyHandle: earphone is inserted already!"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2278_112_2_18_2_0_31_301,(uint8*)"");
				return TRUE;
			}
		}
		else
		{
			if (!s_earphone_msg_is_ok)
			{
				if (KPD_UP == single_code)
				{
					SetEarphoneFlag(FALSE);
					MMISUB_UpdateDisplaySubLcd();
				}
				else
				{
					//SCI_TRACE_LOW:"SpecialKeyHandle: earphone isn't inserted!"
					SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMAIN_2293_112_2_18_2_0_31_302,(uint8*)"");
					return TRUE;
				}
			}
			else
			{
				s_earphone_msg_is_ok = FALSE;	//reset此标志
				return TRUE;
			}
		}
	}
	break;
	default:
	break;
	}

	return FALSE;
}
#endif
