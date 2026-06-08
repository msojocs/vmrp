/****************************************************************************
** File Name:      mmk_port.c                                              *
** Author:         Great.Tian                                              *
** Date:           14/01/2004                                              *
** Copyright:      2004 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe the port modual.          *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 01/2004        Great.Tian       Create
** 
****************************************************************************/
#define MMI_DEFAULT_C


/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "mmi_app_kernel_trc.h"
#include "mmi_default.h"
#include "mmk_timer.h"
#include "mmk_type.h"
#include "mmk_kbd.h"
#include "mmk_msg.h"
#include "tb_dal.h"
//#include "mmiset.h"
#include "mmiset_export.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "guilcd.h"
#include "mmicc_export.h"
#include "mmimp3_export.h"
#include "mmikur.h"
#include "mmi_id.h"
#include "mmiidle_export.h"
#include "mmiphone_export.h"
#include "mmiidle_subwintab.h"
#include "mmi_appmsg.h"
#include "mmi_theme.h"
#include "mmikl_export.h"
#include "mmi_signal.h"
#include "mmitv_out.h"
#ifndef WIN32
#include "deep_sleep.h"
#include "gpio_prod_api.h"
#include "tp_api.h"
#else
#include "mmiudisk_simu.h"
#endif
#include "mmisd_export.h"
#include "mmiudisk_export.h"

#include "mmifm_export.h"

#include "mmirecord_internal.h"
#include "mmifmm_export.h"
#include "mmikl_export.h"
#ifdef STREAMING_SUPPORT_NEXTREAMING
#include "mmist_internal.h"
#endif

#include "mmiset_display.h"
#include "block_mem.h"
#include "mmialarm_export.h"
#include "mmicountedtime_export.h"

#include "mmitheme_special.h"
#include "mmifm_export.h"
#include "dal_dcamera.h"
#ifdef UI_P3D_SUPPORT
#include "mmi3d_export.h"
#endif
#include "mmidm_export.h"
#ifdef MMI_ENABLE_DCD	//!宏定义开头
#include "mmidcd_id.h"
#endif
#ifdef VT_SUPPORT
#include "mmivt_export.h"
#endif
#ifdef TTS_SUPPORT
#include "mmitts_export.h"
#endif
#ifdef LCD_SLIDE_SUPPORT
#include "mmipub.h"
#endif
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef UI_P3D_SUPPORT
#ifdef LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT
#include "mmiss_screenclose_special.h"
#endif
#endif
#include "mmiatv_export.h"
#include "mmk_gsensor.h"
#include "mmk_tp.h"
//#include "mmienvset.h"
#include "mmienvset_export.h"
#include "guitab.h"
#include "mmicc_id.h"
#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif
#ifdef _ATEST_
#include "atest_export.h"
#endif
#include "mmi_autotest.h"
#include "mmistk_export.h"

#ifdef PRINTSCREEN_SUPPORT
#include "mmiptscr_export.h"
#include "mmipub.h" /*ms00239652 renwei modify add*/ 
#endif

#ifdef TAKESCREENPHOTO_SUPPORT 
#include "mmitakescrphoto_main.h"
#endif
#ifdef WRE_SUPPORT
#include "mmiwre_export.h"
#endif

#include "mmiset_id.h"
#include "mmiset_wintab.h"
#include "guiwin.h"
#include "mmicom_panel.h"
#include "Mmimp3_internal.h"
#include "mmiidle_statusbar.h"
#ifdef SCREENSAVER_SUPPORT
#include "mmiss_export.h"
#endif
#include "mmidc_export.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#endif
#ifdef ASP_SUPPORT
#include "mmiasp_export.h"
#endif

#ifdef TTS_SUPPORT
#include "mmisms_set.h" /*lint !e766*/
#endif
#ifdef JAVA_SUPPORT_MYRIAD
#include "mmijava_export.h"
#endif

#ifdef MMI_TURNON_BACKLIGHT_EFFECT_SUPPORT
#include "special_effect.h"
#endif
#ifdef MET_MEX_SUPPORT
#include "met_mex_export.h"
#endif

/**---------------------------------------------------------------------------*
 **                         Macro Declaration                                 *
 **---------------------------------------------------------------------------*/
#define MMI_KEYRING_PALY_TIME               1    // 按键音播放的次数
#define MMI_BACKLIGHT_TIMER_VALUE           12000
#define MMI_BACKLIGHT_FLIP_DELAY_TIME       5000
#define MMI_BACKLIGHT_HALF_ON_DELAY_TIME    5000
#define MMI_BACKLIGHT_ON_DELAY_TIME_IN_CALL    5000//接通电话后，灭背光(半亮，然后灭屏)
#define MMI_BACKLIGHT_HALF_ON_DELAY_TIME_IN_CALL    2500//接通电话后，灭屏
#define MMI_HEADSET_TIMER_OUT               1000 //耳机插入后1秒以内，不响应button
//@$discuss rocky为MMI_BACK_LIGHT_HALF_ON_DEBGREE       40
#define MMI_BACK_LIGHT_HALF_ON_DEBGREE      9//50 //50%

#define SCR_SIZE    ((MMI_MAINSCREEN_HEIGHT*MMI_MAINSCREEN_WIDTH)*sizeof(uint16))

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/
#ifdef DPHONE_SUPPORT 
typedef struct 
{
  BOOLEAN is_hook;        //is hook up
  BOOLEAN is_handfree;     //is hand free down
  BOOLEAN is_tone_on;      //is tone on
}MMI_HOOKANDHF_STATUS_T;
#endif


typedef enum
{
    MMI_BACKLIGHT_CTRL_NORMAL,          //按键能点亮背光。根据setting的设置能够关闭背光
    MMI_BACKLIGHT_CTRL_ALWAYS_ON,       //长开
    MMI_BACKLIGHT_CTRL_ALWAYS_HALF_ON,  //始终半暗  （注意：ALWAYS_ON 和 HALF_ON不允许互相切换，必须从NORMAL状态切换）
    MMI_BACKLIGHT_CTRL_ALWAYS_OFF,      //长关
    MMI_BACKLIGHT_CTRL_NORMAL_HALF_ON,  //HALF ON状态位置5秒钟
    MMI_BACKLIGHT_CTRL_MAX
}MMI_BACKLIGHT_CONTROL_E;

typedef struct
{
    MMI_HANDLE_T win_handle;            //窗口
    uint32       iDirection;            //切换方向（打开新窗口：0退回原窗口：1）
    WINDOW_SUPPORT_ANGLE  pre_angle;             //previous screen support angle.
    WINDOW_SUPPORT_ANGLE  next_angle;            //next screen angle.
}MMI_TRANS_INFO_T;  //保存场景切换信息

LOCAL uint32 s_mmi_3d_effect_data = 0;
LOCAL int16 s_saved_move_buf_type = SCR_EFFECT_BUF_TYPE_NONE;
LOCAL GUI_RECT_T s_last_popup_menu_rect = {0};
LOCAL BOOLEAN s_is_last_closed_popup_window = FALSE;

LOCAL MMI_WIN_TYPE_E s_last_close_win_type = MMI_WIN_TYPE_NONE;
LOCAL BOOLEAN s_tab_win_switch_item = FALSE;

//LOCAL GUITAB_NEED_ITEM_DATA_T s_tab_data = {0};
//LOCAL uint16 s_key_off_times = 0;
//LOCAL uint16 s_tp_off_times = 0;

#ifdef DPHONE_SUPPORT 
MMI_HOOKANDHF_STATUS_T g_hook_hf_status;
#endif

/**---------------------------------------------------------------------------*
 **                         Constant Variables                                *
 **---------------------------------------------------------------------------*/
//not use
// uint8 mmi_key_press_tone[] =
// {
//     AUD_DTMF_One,    //KPD_NONE =0,                    /* 0x00                             */
//     AUD_DTMF_One,    //MMI_KBD_UP     ,                /* 0x01                             */
//     AUD_DTMF_One,    //MMI_KBD_DOWN   ,                /* 0x02                             */
//     AUD_DTMF_One,    //KPD_LEFT   ,                    /* 0x03                             */
//     AUD_DTMF_One,    //KPD_RIGHT  ,                    /* 0x04                             */
//     AUD_DTMF_One,    //KPD_GREEN   ,                    /* 0x05                             */    
//     AUD_DTMF_One,    //KPD_CAMERA  ,                   /* 0x06                             */
//     
//     AUD_DTMF_One,    //KPD_CLR    ,                    /* 0x07                             */    
//     AUD_DTMF_One,    //KPD_1      ,                    /* 0x08                             */
//     AUD_DTMF_Two,    //KPD_2      ,                    /* 0x09                             */
//     AUD_DTMF_Three,  //KPD_3      ,                    /* 0x0a                             */
//     AUD_DTMF_Four,   //KPD_4      ,                    /* 0x0b                             */
//     AUD_DTMF_Five,   //KPD_5      ,                    /* 0x0c                             */
//     AUD_DTMF_Six,    //KPD_6      ,                    /* 0x0d                             */
//     AUD_DTMF_Seven,  //KPD_7      ,                    /* 0x0e                             */
//     AUD_DTMF_Eight,  //KPD_8      ,                    /* 0x0f                             */
//     AUD_DTMF_Nine,   //KPD_9      ,                    /* 0x10                             */
//     AUD_DTMF_Star,   //KPD_STAR   ,                    /* 0x11                             */
//     AUD_DTMF_Zero,   //KPD_0      ,                    /* 0x12                             */
//     AUD_DTMF_Pond,   //KPD_HASH   ,                    /* 0x13                             */
//     
//     AUD_DTMF_One,    //KPD_SPUP   ,                     /* 0x14                             */
//     AUD_DTMF_One,    //KPD_SPDW   ,                     /* 0x15                             */
//   
//     AUD_DTMF_One,    //KPD_LEFTSOFK,                    /* 0x16*/
//     AUD_DTMF_One,    //KBD_RIGHTSOFK,                   /* 0x17 */
//     AUD_DTMF_One,    //KBD_RED,                         /* 0x18 */
//     AUD_DTMF_One,    //KBD_UPSIDEKEY,  SCI_VK_EXT3      /* 0x19 */
//     AUD_DTMF_One,    //KBD_DOWNSIDEKEY,SCI_VK_EXT4      /* 0x1a */
//     AUD_DTMF_One,    //KBD_WEB                          /* 0x1b */  
//     AUD_DTMF_One,    //                                 /* 0x1c */  
//     AUD_DTMF_One,    //KBD_DELETE                       /* 0x1d */  
//     AUD_DTMF_One,    //KBD_FLIP                         /* 0x1e */  
//     AUD_DTMF_One,    //                                 /* 0x1f */ 
//     AUD_DTMF_One     //KBD_HEADSET                      /* 0x20 */  
// };

LOCAL const uint8 s_keycode_table[] =
{
    0,                      //  KPD_NONE =0,                    /* 0x00                             */
    SCI_VK_UP,              //  KPD_UP     ,                    /* 0x01                             */
    SCI_VK_DOWN,            //  KPD_DOWN   ,                    /* 0x02                             */
    SCI_VK_LEFT,            //  KPD_LEFT   ,                    /* 0x03                             */
    SCI_VK_RIGHT,           //  KPD_RIGHT  ,                    /* 0x04                             */
    SCI_VK_CALL,            //  KPD_GREEN  ,                    /* 0x05                             */
    SCI_VK_CAMERA,          //  KPD_CMERA   ,                   /* 0x06                            */
    0,                      //  KPD_CLR    ,                    /* 0x07                             */
    SCI_VK_1,               //  KPD_1      ,                    /* 0x08                             */
    SCI_VK_2,               //  KPD_2      ,                    /* 0x09                             */
    SCI_VK_3,               //  KPD_3      ,                    /* 0x0a                             */
    SCI_VK_4,               //  KPD_4      ,                    /* 0x0b                             */
    SCI_VK_5,               //  KPD_5      ,                    /* 0x0c                             */
    SCI_VK_6,               //  KPD_6      ,                    /* 0x0d                             */
    SCI_VK_7,               //  KPD_7      ,                    /* 0x0e                             */
    SCI_VK_8,               //  KPD_8      ,                    /* 0x0f                             */
    SCI_VK_9,               //  KPD_9      ,                    /* 0x10                             */
    SCI_VK_STAR,            //  KPD_STAR   ,                    /* 0x11                             */
    SCI_VK_0,               //  KPD_0      ,                    /* 0x12                             */
    SCI_VK_POUND,           //  KPD_HASH   ,                    /* 0x13                             */
    0,                      //  KPD_SPUP   ,                    /* 0x14                             */
    0,                      //  KPD_SPDW   ,                    /* 0x15                             */
#ifdef MMI_PDA_SUPPORT
    0,
#else
    SCI_VK_MENU_SELECT,     //  KPD_LEFTSOFK(ok),               /* 0x16*/
#endif
    SCI_VK_MENU_CANCEL,     //  KBD_RIGHTSOFK(cancel),          /* 0x17 */
#ifdef MMI_PDA_SUPPORT
#ifndef KEYPAD_TYPE_FOUR_KEY_UNIVERSE//wei ren
    SCI_VK_POWER,
#else
    SCI_VK_WEB, //pda的web key转定义成KEY_RED消息
#endif
#else
    SCI_VK_POWER,           //  KBD_RED,                        /* 0x18 */
#endif
    SCI_VK_SIDE_UP,         //KBD_UPSIDEKEY                     /* 0x19 */    
    SCI_VK_SIDE_DOWN,       // KBD_DOWNSIDEKEY                  /* 0x1a*/ 
#ifdef MMI_PDA_SUPPORT
#ifndef KEYPAD_TYPE_FOUR_KEY_UNIVERSE//wei ren
    SCI_VK_WEB,
#else
    0, //pda不产生没有KEY_WEB消息
#endif
#else
    SCI_VK_WEB,             // KBD_WEB                          /* 0x1b*/ 
#endif
    0,                      //SCI_GPIO_SIG1,                    /* 0x1c*/
    0,                      //SCI_GPIO_SIG2,                    /* 0x1d*/
    SCI_VK_FLIP,            //KBD_FLIP                          /* 0x1e*/
    SCI_VK_HEADSET_BUTTOM,                                      /* 0x1f*/
    SCI_VK_HEADSET_DETECT,  //KBD_HEADSET                       /* 0x20*/
//#ifdef MMI_SDCARD    
    SCI_VK_SDCARD_DETECT,   // sdcard_detect                    /*0x21*/
//#endif
    0,              // wap online                       /*0x22*/
    SCI_VK_OK,              // vt call key                      /*0x23*/
//#ifdef SC6800_DVB
            
    SCI_VK_REWIND,          //                                  /*0x24*/
    SCI_VK_FASTFORWARD,     //                                  /*0x25*/
    SCI_VK_PAUSE,           //                                  /*0x26*/

    SCI_VK_AT,							    /*0x27*/	
    SCI_VK_Q, 								/*0x28*/
    SCI_VK_W,								/*0x29*/
    SCI_VK_E,								/*0x2a*/
    SCI_VK_R,								/*0x2b*/
    SCI_VK_T,								/*0x2c*/
    SCI_VK_Y,								/*0x2d*/
    SCI_VK_U,								/*0x2e*/
    SCI_VK_I,								/*0x2f*/
    SCI_VK_O,								/*0x30*/
    SCI_VK_P,								/*0x31*/
    SCI_VK_A,								/*0x32*/
    SCI_VK_S,								/*0x33*/
    SCI_VK_D,								/*0x34*/
    SCI_VK_F,								/*0x35*/
    SCI_VK_G,								/*0x36*/
    SCI_VK_H,								/*0x37*/
    SCI_VK_J,								/*0x38*/
    SCI_VK_K,								/*0x39*/
    SCI_VK_L,								/*0x3a*/
    SCI_VK_DEL,							/*0x3b*/
    SCI_VK_Z,								/*0x3c*/
    SCI_VK_X,								/*0x3d*/
    SCI_VK_C,								/*0x3e*/
    SCI_VK_V,								/*0x3f*/
    SCI_VK_B,								/*0x40*/
    SCI_VK_N,								/*0x41*/
    SCI_VK_M,								/*0x42*/	
    SCI_VK_COMMA,							/*0x43*/
    SCI_VK_PERIOD,							/*0x44*/
    SCI_VK_ENTER,							/*0x45*/
    SCI_VK_FN,								/*0x46*/
    SCI_VK_SHIFT,							/*0x47*/
    SCI_VK_AT_QWERTY,						/*0x48*/	
    SCI_VK_SPACE,							/*0x49*/
    SCI_VK_AND,							/*0x4a*/
    SCI_VK_QUESTION,						/*0x4b*/
    SCI_VK_CTRL,							/*0x4c*/    
//#ifdef MMI_MULTI_KBD
    0,  //#define KEY_PLUS                0x4d  // +
    0,  //#define KEY_LEFT_PARENTHESIS    0x4e  // (
    0,  //#define KEY_RIGHT_PARENTHESIS   0x4f  // )
    0,  //#define KEY_MINUS               0x50  // -
    0,  //#define KEY_DOUBLE_QUOTES       0x51  // "
    0,  //#define KEY_SEMICOLON           0x52  // ;
    0,  //#define KEY_COLON               0x53  // : 
    0,  //#define KEY_SLASH               0x54  // /
    0,  //#define KEY_SHIFT_AND           0x55  // &
    0,  //#define KEY_EXCLAMATION         0x56  // !
//#endif
//#endif

    SCI_VK_SLIDE, //KEY_SLIDE   /*0x57*/
    SCI_VK_TV,					/*0x58*/
    SCI_VK_CALL2,				/*0x59*/
    SCI_VK_CALL3,				/*0x5a*/
	SCI_VK_MP3,					/*0x5b*/
	SCI_VK_NOTES,				/*0x5c*/
	SCI_VK_SMS,					/*0x5d*/
	SCI_VK_CALENDER,			/*0x5e*/
	SCI_VK_IE,					/*0x5f*/
	SCI_VK_HANG,				/*0x60*/
	SCI_VK_VOL_UP,				/*0x61*/
	SCI_VK_VOL_DOWN,			/*0x62*/
	SCI_VK_MO,					/*0x63*/				
//#endif 
#ifdef DPHONE_SUPPORT 
	 SCI_VK_HOOK,			    /*0x64*/
     SCI_VK_HANDFREE,		    /*0x65*/
#else
     0,                         /*0x64*/
     0,                         /*0x65*/
#endif

     0,                         /*0x66*/

#ifdef MMI_PDA_SUPPORT
#ifndef KEYPAD_TYPE_FOUR_KEY_UNIVERSE//wei ren
    0,
#else
     SCI_VK_POWER,              /*0x67*/
#endif
#else
     0,                         /*0x67*/
#endif

#ifdef MMI_PDA_SUPPORT
    SCI_VK_MENU_SELECT,         /*0x68*/
#else
    0,                          /*0x68*/
#endif

#ifdef WIN32    				//win simulator test key
    0x22,
    0x23,
    0x25,
    0x26
#endif
};

#ifdef LCD_SLIDE_SUPPORT
LOCAL MMK_SLIDE_STATUS_E s_slide_status = MMK_SLIDE_OPEN;   //滑盖状态
#endif

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
LOCAL MMI_BACKLIGHT_STATUS_E s_backlight_status;

LOCAL MMI_BACKLIGHT_CONTROL_E s_backlight_ctrol;  // 当前是否允许BackLight, 可以打开但是不允许关闭

LOCAL BOOLEAN   s_is_down_keystatus_backlight_on = FALSE; //是否在背光亮时按下按键

uint8 g_mmi_headset_time_id; // 耳机插入1秒以内，不响应button

LOCAL BOOLEAN s_is_enable_3dmmi = FALSE;    //开启/禁止3D特效
LOCAL BOOLEAN s_is_allow_keylock = FALSE;   // 当前是否允许keylock
LOCAL uint8 * s_msg_data_ptr  = PNULL;

LOCAL MMI_LINK_NODE_T *s_key_off_listhead_ptr = PNULL;  //按键音开关链表头指针
LOCAL MMI_LINK_NODE_T *s_tp_off_listhead_ptr = PNULL;    //触摸音开关链表头指针
static MMI_WIN_ANIM_TYPE_E s_cur_win_anim_type = MMI_WIN_ANIM_NONE;

extern void LCD_SetBackLightBrightness(uint32  brightless);
/**---------------------------------------------------------------------------*
 **                         Functions                                         *
 **---------------------------------------------------------------------------*/

#ifdef LCD_SLIDE_SUPPORT
/*****************************************************************************/
//  Description : 是否滑盖关闭时响应的按键
//  Global resource dependence : 
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsKeyResponseWhenSlideClose( 
                                          uint32 keycode
                                          );
#endif

/*****************************************************************************/
//  Description : Link Process about key_tp ring on or off, module_name_ptr is string of module_name. the message module is "sms"
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
LOCAL MMI_LINK_NODE_T *LinkProcess(MMI_LINK_NODE_T *link_head_ptr, const char *module_name_ptr, BOOLEAN is_enable);

/*****************************************************************************/
//  Description:   find a node with condition
//  Global resource dependence: 
//  Author: yanxian.zhou
//  Note:
/*****************************************************************************/
LOCAL MMI_LINK_NODE_T *MMILINK_AudioSearchNode(MMI_LINK_NODE_T const *node_ptr, char const *condition);

/*****************************************************************************/
// Description : 是否允许启用3D翻转特效
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
//LOCAL BOOLEAN IsPermitDesktop3D(void);

/*****************************************************************************/
//  Description : 分配3d场景切换的内存，开始场景切换时调用
//  Global resource dependence : 
//  Author: bin.ji
//  Note: TRUE success, FALSE fail
/*****************************************************************************/
LOCAL BOOLEAN AllocTrans3DBuf(
                              uint16 **old_buf_pptr,  //out, 保存旧场景的buf
                uint16 **new_buf_pptr       //out, 保存新场景的buf
                              );

/*****************************************************************************/
// Description : 3d block_mem没有被别的模块使用（如果被别的模块使用，就不启动3d特效）
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
LOCAL BOOLEAN Is3DBlockMemUsedByOther(BLOCK_MEM_ID_E block_id);

/*****************************************************************************/
// Description : 是否禁止3D特效的窗口
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsDisable3DWindow(
                                MMI_HANDLE_T win_handle
                                );

/*****************************************************************************/
// Description : 是否允许启用3D特效
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsPermit3D(void);

#ifdef WIN32
/*****************************************************************************/
//  Description : dummy function of prod driver         
//  Global resource dependence : 
//  Author: bruce.chi
//  Note: 
/*****************************************************************************/
LOCAL void TestKey1Func(void);
LOCAL void TestKey2Func(void);
LOCAL void TestKey3Func(void);
LOCAL void TestKey4Func(void);
#endif

/*****************************************************************************/
//  Description : start lcd back light timer according to setting
//  Global resource dependence : 
//  Author: bruce.chi
//  Note:
/*****************************************************************************/    
LOCAL void StartSettingLCDBackLightTimer(void);

/******************************************************************************/
/*! \brief  whether allow lcd backlight half on        
 *  \author Great.Tian
 *  \param[in] 
 */
/******************************************************************************/
LOCAL void SetNormalHalfOnBackLight(BOOLEAN b_half_on);

/*****************************************************************************/
//  Description : handle flip down message
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
LOCAL void HandleFlipDown(
                          void
                          );

/*****************************************************************************/
//  Description : set the status of the lcd sleep          
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
LOCAL void MMIDEFAULT_SetLcdSleep(
                                  BOOLEAN is_sleep //[IN] TRUE: 进入Sleep, FALSE: 退出Sleep
                                  );

/*****************************************************************************/
//  Description : close lcd light
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void CloseAllLight(
                         BOOLEAN    is_lock
                         );

/*****************************************************************************/
// 	Description : is need close back light
//	Global resource dependence : 
//  Author: James.Zhang
//	Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsKbdTpPress( void );

#ifdef UI_P3D_SUPPORT
/*****************************************************************************/
// Description : handle PopMenu Effect
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
LOCAL BOOLEAN HandlePopMenuEffect(
                                  MMI_HANDLE_T win_handle
                                  );

/*****************************************************************************/
// Description : handle tab effect
// Global resource dependence : 
// Author: xiaoqing.lu
// Note:
/*****************************************************************************/
/*LOCAL void HandleTabEffect(
                           MMI_HANDLE_T win_handle
                           );
                           */

#endif // UI_P3D_SUPPORT

LOCAL BOOLEAN DefaultSlideClose(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL BOOLEAN DefaultBackLight(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL void DefaultKeyTone(
    uint32 keystatus,
    uint32 keycode
);

LOCAL void DefaultDtmf(
    uint32 keystatus,
    uint32 keycode
);

LOCAL BOOLEAN DefaultHook(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL BOOLEAN DefaultHandfree(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL BOOLEAN DefaultHeadsetDetect(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL BOOLEAN DefaultPrintScreen(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL void DefaultAutoLockKey(
    uint32 keystatus,
    uint32 keycode
);

LOCAL BOOLEAN DefaultIqMode(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL BOOLEAN DefaultIsRespond(
    uint32 keystatus,
    uint32 keycode
);

#if 0
LOCAL void SetVolume(int16 vol);
LOCAL void AdjustVolume(BOOLEAN is_increase);
LOCAL void SetVolumeCB(MMICOM_PANEL_CALLBACK_T *para_ptr);
LOCAL void OpenVolumePanel(void);
#endif
LOCAL void SetRingVolumeCB(MMICOM_PANEL_CALLBACK_T *para_ptr);
LOCAL void OpenRingVolumePanel(void);

LOCAL BOOLEAN DefaultSideKey(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
);

LOCAL BOOLEAN DefaultTakeScreenPhoto(uint32 keystatus,
                                                                                    uint32 keycode,
                                                                                    BOOLEAN *halt_back_ptr);

#ifdef WIN32
/*****************************************************************************/
//  Description : dummy function of prod driver         
//  Global resource dependence : 
//  Author: bruce.chi
//  Note: 
/*****************************************************************************/
PUBLIC void GPIO_SetLcdBackLight(BOOLEAN b_on);
PUBLIC void GPIO_SetKeyPadBackLight(BOOLEAN b_on);
PUBLIC void LCD_SetBackLightBrightness(uint32  brightless);
#endif // WIN32

#ifdef MMI_TURNON_BACKLIGHT_EFFECT_SUPPORT
/*****************************************************************************/
//  Description : DrawTurnOnBackLightAnim
//  Global resource dependence :
//  Author: juan.wu
//  Note:播放背光点亮渐变动画
/*****************************************************************************/
LOCAL BOOLEAN DrawTurnOnBackLightAnim(void);
#endif

#ifdef LCD_SLIDE_SUPPORT
/*****************************************************************************/
//  Description : 是否滑盖关闭时响应的按键
//  Global resource dependence : 
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsKeyResponseWhenSlideClose( 
                                          uint32 keycode
                                          )
{
#ifdef KEYPAD_TYPE_QWERTY_KEYPAD
    if ((keycode >= KEY_Q)
        && (keycode <= KEY_EXCLAMATION))
#else
    if ((keycode >= KEY_1)
        && (keycode <= KEY_HASH))
#endif
    {
        return FALSE;
    }
    return TRUE;
}
#endif



/*==============================================================================
Description: 一些按键在全部窗口的默认处理
Global resource dependence: NONE
Author: Lianxiang.Zhou(改写一下结构)
Note:
handling the key message by default process which may process
in all window (include to start backlight and ring other key voice
and handle other situation
这个函数要完成两个功能：
1. 需要出来来各种按键以后，背光、键盘灯和按键音的处理。
2. 函数的返回值需要标示出，这个消息是否需要发给后续的窗口处理。
----
分几层逻辑来处理
1 首先是超强大的条件，比如翻盖、滑盖等
2 其次是按不同的gogneng 来处理
3 分两步走，先处理事件，后计算返回值
----
鉴于该函数比较复杂，所有的处理必须写明注释，精简代码。特别是逐步添加的代码。
----
特别注意: 各功能块是不相干的
- 滑盖关闭时的特殊处理
- 一些特殊处理，比如记录全局变量等
- 背光
- 按键音
- dtmf
- hook键 - dphone
- handfree键 - dphone
- handset detect
- print screen
- auto lock key
- 挂电话
- 侧键音量
- 计算返回值(会在上面的各个过程中有计算)

==============================================================================*/
BOOLEAN MMK_DefaultMSGKbd(
    uint32 keystatus,
    uint32 keycode
)
{
    BOOLEAN is_halt = FALSE;
    BOOLEAN halt_back = FALSE;
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();
    BOOLEAN is_respond = FALSE;

    /* 滑盖关闭时候的处理 */
    is_halt = DefaultSlideClose(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

    /*
    不区分功能的预处理，即各种按键消息处理之前的处理
    */

    if (KEY_PRESSED == keystatus)
    {
        MMIAPISTK_CnfSimUserAct();
        s_is_down_keystatus_backlight_on = is_back_light;
    }

    /*
    按功能分类，来处理各个侧键
    */
    /*--背光处理 back light*/
    is_halt = DefaultBackLight(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

    /*--按键音处理 key press tone*/
    DefaultKeyTone(keystatus, keycode);

    /*--dtmf处理*/
    DefaultDtmf(keystatus, keycode);

    /*--处理HOOK键*/
    is_halt = DefaultHook(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

    /*--处理HANDFREE键*/
    is_halt = DefaultHandfree(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

    /*--处理HEADSET_DETECT键*/
    is_halt = DefaultHeadsetDetect(keystatus, keycode, &halt_back);

    if (is_halt) 
    {
//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT
    MMIMEX_IsEarphonePlugIn(SCI_TRUE);                    
#endif
//============== MET MEX  End ===================
        return halt_back;
    }

    /*--处理PRINTSCREEN*/
    is_halt = DefaultPrintScreen(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

    /*--AutoKeylockf处理*/
    DefaultAutoLockKey(keystatus, keycode);


    /*--IQ MODE处理*/
    is_halt = DefaultIqMode(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

    //
    //不区分功能的后处理，即各种按键消息处理之后的处理
    //
    if (KEY_PRESSED == keystatus)
    {
        BOOLEAN is_end_key = (KEY_RED == keycode || KEY_HANG == keycode);
        BOOLEAN is_cc_connect = (MMIAPICC_IsInState(CC_MO_CONNECTING_STATE)
                                 || MMIAPICC_IsInState(CC_MT_CONNECTING_STATE)
                                 || MMIAPICC_IsInState(CC_CALL_CONNECTED_STATE));

#ifdef VT_SUPPORT
        MMIAPIVT_SendUserInput(KEY_PRESSED + keycode);
#endif

        //优先处理红键挂断电话
        if (is_end_key)
        {
            if (is_cc_connect && !MMIAPICC_IsVideoCall() && is_back_light)
            {
#ifdef MET_MEX_SUPPORT
                if(MMIMEX_IsOutCalling())
                {
                    //如果平台的正处于通话状态时不将END键的处理发送给CC模块
                    return TRUE;
                }   
#endif      
                MMIAPICC_SendMsgToApplet(KEY_PRESSED + keycode, PNULL);
                return FALSE;
            }

#ifdef VT_SUPPORT
            if (MMIAPIVT_IsVtCalling())
            {
                MMIAPIVT_ReleaseVTCallByRedkey();
                return FALSE;
            }
#endif
        }
    }

    //
    //按键是否响应 - 计算返回值(可能已经半途返回了)
    //
    is_respond = DefaultIsRespond(keystatus, keycode);

    return is_respond;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - slide
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 滑盖关闭时不响应数字*#键
实际上响应的按键由函数IsKeyResponseWhenSlideClose决定
==============================================================================*/
LOCAL BOOLEAN DefaultSlideClose(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    BOOLEAN is_halt = FALSE;

#ifdef LCD_SLIDE_SUPPORT
    if (MMK_SLIDE_CLOSE == MMIDEFAULT_GetSlideStatus())
    {
        if (!IsKeyResponseWhenSlideClose(keycode))
        {
#ifdef LCD_SLIDE_SIMU
            if (KEY_PRESSED == keystatus)
            {
                MMIDEFAULT_TurnOnBackLight();
                MMIPUB_OpenAlertWinByTextId(PNULL, TXT_SLIDE_IS_CLOSED, TXT_NULL,
                                            IMAGE_PUBWIN_FAIL, PNULL, PNULL,
                                            MMIPUB_SOFTKEY_ONE, PNULL);
            }
#endif
            is_halt = TRUE;
            *halt_back_ptr = FALSE;
        }
    }

#endif

    return is_halt;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Back Light
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 点亮、灭背光
有两类特殊的按键：
1 总是点亮背光的特殊键，是：翻盖、滑动、sd卡检测、耳机检测和耳机插入
2 长按点亮背光的特殊键，是：上下侧键和照相键（我总觉得这个键应该叫另外一个功能无关的名字，哪怕“侧面功能键1”也比照相建好）

分三种情况：
1 按键的时候，如果背光灭的，我们可能需要点亮背光，如果
    1 盖子是翻开的，并且响应按键窗口是顶层窗口(红键除外)
    2 盖子非翻开，则当按动第一类特殊键的时候
2 长按的时候，相应第二类特殊按键
3 放开按键的时候（包括长按放开），如果
    1 是第一类按键，则点亮背光（这一步是不是多余的，因为第一类特殊按键已经处理过了，并且是伪按键）
    2 否则，启动关背光的timer
==============================================================================*/
LOCAL BOOLEAN DefaultBackLight(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    BOOLEAN is_flip = MMIDEFAULT_GetFlipStatus();
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();
    BOOLEAN need_light_up = FALSE;
    BOOLEAN is_halt = FALSE;

    switch (keystatus)
    {
    case KEY_PRESSED:
        
        //back light and whether need handle this key message
        if (is_flip)
        {
            if (MMIAPIKL_IsKLWinOnTop())
            {
#if 0//def PLATFORM_SC6530//仅为测试，使用KEY_CAMREA点亮背光
                if ( KEY_CAMREA == keycode )
                {
                    keycode = KEY_RED;
                }
#endif

#ifdef MMI_PDA_SUPPORT
                if (keycode == KEY_POWER || keycode == KEY_RED) //red is home
#else
                //新增左软键点亮屏幕,按照新的需求进行修改 2011-2-23
                if ((keycode == KEY_OK)||(keycode == KEY_RED)||(keycode == KEY_HANG)||(keycode == KEY_0)||(keycode == KEY_1)||(keycode == KEY_9))
#endif
                {
                    need_light_up = TRUE;

#ifdef MMI_PDA_SUPPORT
                    if ((is_back_light) && (KEY_POWER == keycode))    /*lint !e774*/
#else
                    if ((is_back_light) && (KEY_RED == keycode))
#endif
                    {
                        need_light_up = FALSE;
                    }
#ifndef MMI_PDA_SUPPORT
                    if (keycode == KEY_RED)
#else
                    if ((keycode == KEY_RED  && !is_back_light)||(keycode == KEY_POWER))
#endif
                    {
                        *halt_back_ptr = FALSE;
                        is_halt = TRUE;
                    }
                }
                else
                {
                    if (is_back_light)
                    {
                        need_light_up = TRUE;
                    }
                    else
                    {
                        need_light_up = FALSE;
                    }
                }
            }
            else
            {
                need_light_up = TRUE;
            }
        }
        else if (!is_back_light)
        {
            //背光灭的情况下：响应2个HEADSET_KEY消息和FLIP消息，SLIDE消息,响应所有长按消息。其他消息都不响应。屏保按键响应相关的代码请不要写到这。
            if ((keycode == KEY_HEADSET_DETECT)
                || (keycode == KEY_HEADSET_BUTTIN)
#ifdef FLIP_PHONE_SUPPORT               
                || (keycode == KEY_FLIP)
#endif                
#ifdef LCD_SLIDE_SUPPORT                
                || (keycode == KEY_SLIDE)
#endif
                )
            {
                need_light_up = TRUE;
            }
            else
            {
                need_light_up =  FALSE;
            }
        }
        else
        {
            need_light_up = TRUE;
        }

        if (need_light_up)
        {
            MMIDEFAULT_TurnOnBackLight();
        }

        break;

    case KEY_LONG_PRESSED:

        switch (keycode)
        {
        case KEY_UPSIDEKEY:
        case KEY_DOWNSIDEKEY:
        case KEY_CAMREA:
            if (!is_flip)
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;

        default:
            break;
        }

        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:

        if (
#ifdef FLIP_PHONE_SUPPORT        
            (KEY_FLIP == keycode)
            ||
#endif            
            (KEY_HEADSET_DETECT == keycode)
            || (KEY_SDCARD_DETECT == keycode)
            || (KEY_HEADSET_DETECT == keycode)
#ifdef LCD_SLIDE_SUPPORT            
            || (KEY_SLIDE == keycode)
#endif            
            )
        {
            MMIDEFAULT_TurnOnBackLight();
        }
        else
        {
            MMIDEFAULT_StartLcdBackLightTimer();
        }

        break;

    default:
        break;
    }

    return is_halt;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Key Tone
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
1 忽略的键，即无按键音的：翻盖、滑盖、KEY_HOOK和KEY_HANDFREE
2 松开还要再响一次的键：sd卡检测和耳机检测

分两种情况：
1 按键的时候，在背光亮的情况下才响，并且非第一类特殊键
2 松开按键的时候，如果是第二类键，则响。
==============================================================================*/
LOCAL void DefaultKeyTone(
    uint32 keystatus,
    uint32 keycode
)
{
    /*
    --按键音处理 key press tone
    */
    if (!MMIDEFAULT_IsEnableKeyRing())
    {
        return;
    }

    switch (keystatus)
    {
    case KEY_PRESSED:
        {
            BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();
            BOOLEAN is_stop_msg_ring = MMIAPISET_StopMsgRing();
            BOOLEAN is_omit_key = (keycode >= KEY_MAX_NUM
#ifdef FLIP_PHONE_SUPPORT            
                                   || KEY_FLIP == keycode
#endif                                   
#ifdef LCD_SLIDE_SUPPORT
                                   || KEY_SLIDE == keycode
#endif
#ifdef DPHONE_SUPPORT
                                   || KEY_HOOK == keycode
                                   || KEY_HANDFREE == keycode
#endif
                                  );

            if (is_back_light && !is_omit_key && !is_stop_msg_ring)
            {
                POWER_RESTART_CONDITION_E restart_condition = POWER_GetRestartCondition();

                if (RESTART_BY_CHARGE != restart_condition && RESTART_BY_CHARGE != restart_condition)
                {
                    MMIAPISET_PlayRing(0, FALSE, 0, MMI_KEYRING_PALY_TIME, MMISET_RING_TYPE_KEY, PNULL);
                }
            }
        }
        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:
        {
            BOOLEAN is_respond = (KEY_HEADSET_DETECT == keycode || KEY_SDCARD_DETECT == keycode);

            if (is_respond)
            {
                MMIAPISET_PlayRing(MN_DUAL_SYS_1, FALSE, 0, MMI_KEYRING_PALY_TIME, MMISET_RING_TYPE_KEY, PNULL);
            }
        }

        break;

    default:
        break;
    }

    return;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Dtmf
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
一类特殊键：上下侧键和照相键

在以下情况都会有dtmf
1 按键
2 放开按键（包括长按放开）
3 长按时，非特殊键
==============================================================================*/
LOCAL void DefaultDtmf(
    uint32 keystatus,
    uint32 keycode
)
{
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();

    if (!is_back_light) return;

    MMIAPICC_SendDtmf(keystatus + keycode);

    return;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Hook
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 这部分代码是dphone增加的代码，直接复制过来，没有修改
==============================================================================*/
LOCAL BOOLEAN DefaultHook(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
#ifdef DPHONE_SUPPORT
    if (KEY_HOOK != keycode) return FALSE;

    switch (keystatus)
    {
    case KEY_PRESSED:
        //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd]Hook key is pressed."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1184_112_2_18_1_59_57_133,(uint8*)"");
        MMIDEFAULT_SetHookStates(FALSE);

        if (MMIDEFAULT_GetHandFreeStates())
        {
            /*如果在免提模式下，设置声音设备模式:handfree模式*/
            //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd]Now mode is handfree."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1190_112_2_18_1_59_57_134,(uint8*)"");
            //MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_HANDFREE, MMIAUDIO_GetCurVolume());
            MMISRVAUD_SetRouteNohandle(MMISRVAUD_ROUTE_SPEAKER);
        }
        else
        {
            //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd]Mode is not handfree, so stop all ring."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1196_112_2_18_1_59_57_135,(uint8*)"");
            MMIAPISET_StopAppRing(TRUE);
            MMIDEFAULT_SetToneStates(FALSE);
            //MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_HANDFREE, MMIAUDIO_GetCurVolume());
            MMISRVAUD_SetRouteNohandle(MMISRVAUD_ROUTE_SPEAKER);
        }

        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:
        //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd]Hook key is released."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1207_112_2_18_1_59_57_136,(uint8*)"");

        if (MMIAPIPHONE_GetPinLockHookHandfreeStatus())
        {
            *halt_back_ptr = FALSE;

            return TRUE;
        }

        MMIDEFAULT_SetHookStates(TRUE);
        MMIDEFAULT_SetHandFreeStatesAndLed(FALSE);
        /*设置声音设备模式:hook模式*/
        //MMIAUDIO_SetCurDevMode(AUDIO_DEVICE_MODE_HANDHOLD);
        MMISRVAUD_SetRouteNohandle(MMISRVAUD_ROUTE_HANDHOLD);

        if (MMIIDLE_DIAL_WIN_ID != MMK_GetFocusWinId() && !MMIAPICC_IsInState(CC_IN_CALL_STATE))
        {
            MMIDEFAULT_SetToneStates(TRUE);
            MMIAPISET_PlayRing(0, FALSE, 0, 1, MMISET_RING_TYPE_HOOK_HANDFREE_TONE, PNULL);
        }

        MMIDEFAULT_SetBackLight(TRUE);

        break;

    default:
        break;
    }
#endif

    return FALSE;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Handfree
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 这部分代码是dphone增加的代码，直接复制过来，没有修改
注，该消息自行处理了按键音
==============================================================================*/
LOCAL BOOLEAN DefaultHandfree(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
#ifdef DPHONE_SUPPORT
    if (KEY_HANDFREE != keycode) return FALSE;
    if (KEY_PRESSED != keystatus) return FALSE;

    //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd]HandFree key is pressed"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1257_112_2_18_1_59_57_137,(uint8*)"");

    if (MMIDEFAULT_GetHandFreeStates())
    {
        /*免提状态下，需要关闭免提*/
        MMIDEFAULT_SetHandFreeStatesAndLed(FALSE);

        if (MMIDEFAULT_GetHookStates())
        {
            /*设置声音设备模式:hook模式*/
            //MMIAUDIO_SetCurDevMode(AUDIO_DEVICE_MODE_HANDHOLD);
            MMISRVAUD_SetRouteNohandle(MMISRVAUD_ROUTE_HANDHOLD);
        }
        else
        {
            MMIDEFAULT_SetToneStates(FALSE);
            MMIAPISET_StopAllRing(TRUE);

            /*非通话状态，需要返回到IDLE*/
            if (!MMIAPICC_IsInState(CC_IN_CALL_STATE)) MMK_ReturnIdleWin();
        }
    }
    else
    {
        if (MMIAPIPHONE_GetPinLockHookHandfreeStatus())
        {
            *halt_back_ptr = FALSE;

            return TRUE;
        }

        if (MMK_IsFocusWin(MAIN_IDLE_WIN_ID)
                || MMK_IsFocusWin(MMIIDLE_DIAL_WIN_ID)
                || MMIAPICC_IsInState(CC_IN_CALL_STATE))
        {
            MMIDEFAULT_SetHandFreeStatesAndLed(TRUE);

            if (!MMIDEFAULT_GetHookStates())
            {
                /*设置声音设备模式:handfree模式*/
                //MMIAUDIO_SetCurDevMode(AUDIO_DEVICE_MODE_HANDFREE);
                MMISRVAUD_SetRouteNohandle(MMISRVAUD_ROUTE_SPEAKER);
            }

            if (MMK_IsFocusWin(MAIN_IDLE_WIN_ID))
            {
                MMIDEFAULT_SetToneStates(TRUE);
                /*need add tone when*/
                MMIAPISET_PlayRing(0, FALSE, 0, 1, MMISET_RING_TYPE_HOOK_HANDFREE_TONE, PNULL);
            }
        }
        else
        {
            MMIDEFAULT_SetHandFreeStatesAndLed(TRUE);

            if (!MMIDEFAULT_GetHookStates())
            {
                //MMIAUDIO_SetCurDevMode(AUDIO_DEVICE_MODE_HANDFREE);
                MMISRVAUD_SetRouteNohandle(MMISRVAUD_ROUTE_SPEAKER);
            }

            MMIDEFAULT_SetToneStates(TRUE);
            MMIAPISET_PlayRing(0, FALSE, 0, 1, MMISET_RING_TYPE_HOOK_HANDFREE_TONE, PNULL);
        }
    }
#endif

    return FALSE;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Headset detect
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL BOOLEAN DefaultHeadsetDetect(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    if (KEY_HEADSET_DETECT != keycode) return FALSE;
        
    switch (keystatus)
    {
    case KEY_PRESSED:
        {
            MMIAPIFM_HeadsetInd(MSG_KEYDOWN_HEADSET_DETECT);
#if !defined DPHONE_SUPPORT && defined MMIENVSET_EARPHONE_BLUETOOTH_SUPPORT	
            if (MMIENVSET_BLUETOOTH_MODE != MMIAPIENVSET_GetActiveModeId())
            {
                MMIAPIENVSET_SetEarphoneMode();
            }
#endif
            if (MMIAPICC_IsInState(CC_IN_CALL_STATE)) //需要增加是否是手动处理模式
            {
#ifdef VT_SUPPORT
                if (MMIAPICC_IsVideoCall())
                {
                    MMIAPIVT_SendMsgToApplet(KEY_PRESSED + keycode);
                }
                else
#endif
                {
                    MMIAPICC_HandleHeadsetInCall(KEY_PRESSED + keycode);
                }
            }
        }
        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:
        {
            MMIAPIFM_HeadsetInd(MSG_KEYUP_HEADSET_DETECT);            
#if !defined DPHONE_SUPPORT && defined MMIENVSET_EARPHONE_BLUETOOTH_SUPPORT	
            if (MMIENVSET_EARPHONE_MODE == MMIAPIENVSET_GetActiveModeId())
            {
                MMIAPIENVSET_UnSetEarphoneMode();
            }
#endif
            if (MMIAPICC_IsInState(CC_IN_CALL_STATE)) //需要增加是否是手动处理模式
            {
#ifdef VT_SUPPORT
                if (MMIAPICC_IsVideoCall())
                {
                    MMIAPIVT_SendMsgToApplet(KEY_RELEASED + keycode);
                }
                else
#endif
                {
                    MMIAPICC_HandleHeadsetInCall(KEY_RELEASED + keycode);
                }
            }

        }
        break;

    default:
        break;
    }

    

    MMISRVAUD_ExtDevChg();

    MAIN_UpdateIdleEarPhoneState(); // 耳机插拔需要更新状态栏的状态。

    *halt_back_ptr = TRUE;

    GUIWIN_UpdateStb();

    return TRUE;

    
#if 0
    switch (keystatus)
    {
    case KEY_PRESSED:
        {
        	if(MMIAPIFM_IsFmActive())
            {
                MMIAPIFM_HeadsetInd(MSG_KEYDOWN_HEADSET_DETECT);
            }
            if (MMITHEME_IsTVOpen())
            {
                MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_TVOUT, MMIAUDIO_GetCurVolume());
            }
            else if (MMIAUDIO_IsInVoiceLoop())
            {
                MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_EARPHONE, MMIAUDIO_GetCurVolume());
                //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd](KEY_PRESSED)MMIAUDIO_SetVolume,AUDIO_DEVICE_MODE_EARPHONE, %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1436_112_2_18_1_59_58_138,(uint8*)"d" ,MMIAUDIO_GetCurVolume());
            }
            else if (MMIAPICC_IsInState(CC_IN_CALL_STATE))
            {
#ifdef VT_SUPPORT
                if (MMIAPICC_IsVideoCall())
                {
                    MMIAPIVT_SendMsgToApplet(KEY_PRESSED + keycode);
                }
                else
#endif
                {
                    MMIAPICC_HandleHeadsetInCall(KEY_PRESSED + keycode);
                }
            }
            else if (MMIAPISET_IsPlayingRingType(MMISET_RING_TYPE_CALL)
#ifdef TTS_SUPPORT
                     || MMIAPITTS_IsPlayRing(MMISET_RING_TYPE_CALL)
#endif
                    )
            {

                MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_EARFREE, MMIAUDIO_GetCurVolume());
            }
            else if (MMIAPIALM_IsAlarmActive())
            {
                /*do nothing*/
            }
            else
            {
                MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_EARPHONE, MMIAUDIO_GetCurVolume());
            }

#if !defined DPHONE_SUPPORT && defined MMIENVSET_EARPHONE_BLUETOOTH_SUPPORT	
            if (MMIENVSET_BLUETOOTH_MODE != MMIAPIENVSET_GetActiveModeId())
            {
                MMIAPIENVSET_SetEarphoneMode();
            }
#endif
        }
        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:
        {
        	if(MMIAPIFM_IsFmActive())
            {
                MMIAPIFM_HeadsetInd(MSG_KEYUP_HEADSET_DETECT);
            }
            if (MMITHEME_IsTVOpen())
            {
                MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_TVOUT, MMIAUDIO_GetCurVolume());
            }
            else if (MMIAUDIO_IsInVoiceLoop())
            {
                MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_HANDHOLD, MMIAUDIO_GetCurVolume());
                //SCI_TRACE_LOW:"[MMK_DefaultMSGKbd](KEY_RELEASED)MMIAUDIO_SetVolume, AUDIO_DEVICE_MODE_HANDHOLD ,%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_1493_112_2_18_1_59_58_139,(uint8*)"d",MMIAUDIO_GetCurVolume());
            }
            else if (MMIAPICC_IsInState(CC_IN_CALL_STATE))//是否在通话中
            {
#ifdef VT_SUPPORT
                if (MMIAPICC_IsVideoCall())
                {
                    MMIAPIVT_SendMsgToApplet(KEY_RELEASED + keycode);
                }
                else
#endif
                {
                    MMIAPICC_HandleHeadsetInCall(KEY_RELEASED + keycode);
                }
            }
            else if (MMIAPIBT_GetActiveBTHeadset()
                     && (MMIAPIMP3_IsPlayerPlaying() || MMIAPIKUR_IsPlayerPlaying()))
            {
                if (MMIAPIMP3_IsThroughBt())
                {
                    MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_EARPHONE, MMIAUDIO_GetCurVolume());
                }
                else
                {
                    MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_HANDFREE, MMIAUDIO_GetCurVolume());
                }
            }
            else
            {
                if (MMIAPISET_IsThroughReceiver())
                {
                    MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_HANDHOLD, MMIAUDIO_GetCurVolume());
                }
                else
                {
                    MMIAUDIO_SetVolume(AUDIO_DEVICE_MODE_HANDFREE, MMIAUDIO_GetCurVolume());
                }
            }

#if !defined DPHONE_SUPPORT && defined MMIENVSET_EARPHONE_BLUETOOTH_SUPPORT	
            if (MMIENVSET_EARPHONE_MODE == MMIAPIENVSET_GetActiveModeId())

            {
                MMIAPIENVSET_UnSetEarphoneMode();
            }
#endif
        }
        break;

    default:
        break;
    }

    //MAIN_UpdateIdleEarPhoneState(); // 耳机插拔需要更新状态栏的状态。

    *halt_back_ptr = TRUE;

    return TRUE;
#endif
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Print Screen
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL BOOLEAN DefaultPrintScreen(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    BOOLEAN is_halt = FALSE;

#ifdef PRINTSCREEN_SUPPORT
#if defined KEYPAD_TYPE_FOUR_KEY_UNIVERSE //没有call键，但是有camera键
    uint32 const print_key = KEY_CAMREA;
#else
    uint32 const print_key = KEY_GREEN;
#endif
    BOOLEAN run_mode = MMIAPIPTSCR_GetRunMode();
    BOOLEAN has_exception = TRUE;
    MMI_TEXT_ID_T ex_alert_text = 0;
    MMI_TEXT_ID_T print_alert_text = 0;
    MMI_IMAGE_ID_T print_alert_img = 0;
    BOOLEAN is_includelayer = (MMIPTSCR_RUNMODE_PRESS_LCDANDLAYER == MMIAPIPTSCR_GetRunMode());
    MMIPTSCR_ERROR_STATUS_E sdcard_status = MMIAPIPTSCR_PreCheck();

    if (print_key != keycode) return FALSE;
    if (KEY_PRESSED != keystatus) return FALSE;

    if (MMIPTSCR_RUNMODE_PRESS_ONLYLCD != run_mode
            && MMIPTSCR_RUNMODE_PRESS_LCDANDLAYER != run_mode) return FALSE;

    *halt_back_ptr = TRUE;

    if (MMIPTSCR_SDCARD_NO_EXIST == sdcard_status)
    {
        ex_alert_text = TXT_SD_NO_EXIST;
    }
    else if (MMIPTSCR_SDCARD_NO_SPACE == sdcard_status)
    {
        ex_alert_text = TXT_DC_NOEMPTYSPACE;
    }
    else
    {
        has_exception = FALSE;
    }

    if (has_exception)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL, ex_alert_text, TXT_NULL,
                                    IMAGE_PUBWIN_WARNING, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);

        return TRUE;
    }

    if (MMIAPIPTSCR_Print(is_includelayer))
    {
        print_alert_text = TXT_PRINTSCREEN_COMPLETE;
        print_alert_img = IMAGE_PUBWIN_SUCCESS;
    }
    else
    {
        print_alert_text = TXT_MMI_SAVEFAILED;
        print_alert_img = IMAGE_PUBWIN_WARNING;
    }

    MMIPUB_OpenAlertWinByTextId(PNULL, print_alert_text, TXT_NULL,
                                print_alert_img, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);

    is_halt = TRUE;
#endif

    return is_halt;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - TakeScreenPhoto
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL BOOLEAN DefaultTakeScreenPhoto(uint32 keystatus,
                                                                                    uint32 keycode,
                                                                                    BOOLEAN *halt_back_ptr)
{
    BOOLEAN is_halt = FALSE;
#ifdef TAKESCREENPHOTO_SUPPORT
    uint32 const print_key = KEY_CAMREA;
    BOOLEAN has_exception = TRUE;
    MMI_TEXT_ID_T ex_alert_text = 0;
    MMI_TEXT_ID_T print_alert_text = 0;
    MMI_IMAGE_ID_T print_alert_img = 0;
    MMITSCRPHOTO_ERROR_STATUS_E sdcard_status = MMITSCRPHOTO_PreCheck();
    uint32  dis_time        = MMI_1SECONDS;
    MMI_WIN_ID_T win_id=MMK_GetFocusWinId();

    if(win_id == MMIKL_CLOCK_DISP_WIN_ID) return FALSE;
    
    if (print_key != keycode) return FALSE;

    if(win_id == MAIN_IDLE_WIN_ID)
    {
        if (KEY_RELEASED!= keystatus) return FALSE;
    }
    else
    {
        if (KEY_PRESSED!= keystatus) return FALSE;            
    }
    
    *halt_back_ptr = FALSE;

    if (MMITSCRPHOTO_SDCARD_NO_EXIST == sdcard_status)
    {
        ex_alert_text = TXT_SD_NO_EXIST;
    }
    else if (MMITSCRPHOTO_SDCARD_NO_SPACE == sdcard_status)
    {
        ex_alert_text = TXT_DC_NOEMPTYSPACE;
    }
    else if(MMITSCRPHOTO_SDCARD_USING== sdcard_status)
    {
         ex_alert_text = TXT_COMMON_UDISK_USING;
    }
    else
    {
        has_exception = FALSE;
    }

    if (has_exception)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL, ex_alert_text, TXT_NULL,
                                    IMAGE_PUBWIN_WARNING, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);

        return TRUE;
    }

   
    if (MMITSCRPHOTO_TakePhoto())
    {
        print_alert_text = TXT_PRINTSCREEN_COMPLETE;
        print_alert_img = IMAGE_PUBWIN_SUCCESS;
    }
    else
    {
        print_alert_text = TXT_MMI_SAVEFAILED;
        print_alert_img = IMAGE_PUBWIN_WARNING;
    }

    MMIPUB_DisplayTipsIDEx(PNULL, print_alert_text, MMIPUB_TIPS_DIS_MIDDLE, &dis_time);


  //  MMIPUB_OpenAlertWinByTextId(PNULL, print_alert_text, TXT_NULL,
  //                              print_alert_img, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);

    is_halt = TRUE;
#endif
    return is_halt;
}

/*==============================================================================
Description: MMK_DefaultMSGKbd - Auto Lock Key
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
分三种情况
1 按键，亮屏则停止定时器
2 长按，无条件停止定时器
3 松开（包括长按松开），启动定时器
==============================================================================*/
LOCAL void DefaultAutoLockKey(
    uint32 keystatus,
    uint32 keycode
)
{
    switch (keystatus)
    {
    case KEY_PRESSED:

        if (MMIDEFAULT_IsBacklightOn())
        {
            MMIDEFAULT_StopAutoKeylockTimer();
        }

        break;

    case KEY_LONG_PRESSED:
        MMIDEFAULT_StopAutoKeylockTimer();
        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:
        MMIDEFAULT_StartAutoKeylockTimer();
        break;

    default:
        break;
    }

    return;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - iq data
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL BOOLEAN DefaultIqMode(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    BOOLEAN is_flip = MMIDEFAULT_GetFlipStatus();
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();
    BOOLEAN is_key_lock = MMIAPIKL_IsKLWinOnTop();
#ifdef WIN32
    uint32 const iq_key = KEY_GREEN;
#else
    uint32 const iq_key = KEY_UPSIDEKEY;
#endif
    BOOLEAN is_deal_iq_data = (MMIAPIENG_GetIQModeStatus() && iq_key == keycode);

        
    if (!is_flip) return FALSE;
    if (!is_back_light) return FALSE;
    if (is_key_lock) return FALSE;
    if (!is_deal_iq_data) return FALSE;

    switch (keystatus)
    {
    case KEY_PRESSED:
    case KEY_LONG_PRESSED:
        *halt_back_ptr = FALSE;
        break;

    case KEY_RELEASED:
    case KEY_LONG_RELEASED:
        *halt_back_ptr = FALSE;
        MMIAPIENG_OpenIQDataWin();
        break;

    default:
        break;
    }
    

    return TRUE;
}


/*==============================================================================
Description: MMK_DefaultMSGKbd - Is Respond?
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 按键是否响应
==============================================================================*/
LOCAL BOOLEAN DefaultIsRespond(
    uint32 keystatus,
    uint32 keycode
)
{
    BOOLEAN is_flip = MMIDEFAULT_GetFlipStatus();
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();

#ifdef FLIP_PHONE_SUPPORT
    /*FLIP键特殊处理*/
    if (keycode == KEY_FLIP) return TRUE;
#endif

    /*Slide键特殊处理*/
#ifdef LCD_SLIDE_SUPPORT
    if (keycode == KEY_SLIDE) return TRUE;
#endif

    /*SD卡插拔*/
    if (KEY_SDCARD_DETECT == keycode) return TRUE;

    if (is_flip)
    {
        //开盖背光灭，只点亮背光
        if (is_back_light)
        {
            if (MMIAPIKL_IsKLWinOnTop())
            {
                return TRUE;
            }
#if defined(MMI_PDA_SUPPORT)//@tao.xue 通话过程中黑屏不锁屏，在KEY_RED点亮屏幕时，返回FALSE，不继续处理KEY_RED消息。
            else if(MMIAPICC_IsInState(CC_IN_CALL_STATE)&&(KEY_RED == keycode))
            {
                return s_is_down_keystatus_backlight_on;
            }
#endif
            else
            {
                return s_is_down_keystatus_backlight_on;
            }
        }
        else
        {
            //黑屏情况下需要响应的按键
            BOOLEAN is_dark_key = (KEY_UPSIDEKEY == keycode
                                || KEY_DOWNSIDEKEY == keycode);

            if (is_dark_key) return TRUE;

            //当按键不放背光熄灭后，松开按键，需要发送相应的up,long消息处理给窗口处理
            switch (keystatus)
            {
            case KEY_PRESSED:
            case KEY_REPEATED:
                return FALSE;

            case KEY_LONG_PRESSED:
            case KEY_RELEASED:
            case KEY_LONG_RELEASED:
                return s_is_down_keystatus_backlight_on;

            default:
                SCI_PASSERT(FALSE,("MMK_DefaultMSGKbd:keystatus %d is error!",keystatus)); /*assert verified*/
                break;
            }
        }
    }
    else    //合盖情况下
    {
        //背光打开的情况下，所有按键都响应。
        if (is_back_light) return TRUE;

        //背光灭的情况下：响应HEADSET_KEY的插入和拔出消息,响应所有长按消息。其他消息都不响应。屏保按键响应相关的代码请不要写到这。
        if ( keystatus == KEY_PRESSED
            || keystatus == KEY_RELEASED
            || keystatus == KEY_LONG_RELEASED )
        {
            return (keycode == KEY_HEADSET_DETECT || keycode == KEY_HEADSET_BUTTIN);
        }
        else if (keystatus == KEY_LONG_PRESSED)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}


/*==============================================================================
Description: 按键的后处理操作
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 既一个按键在被所有的模块放弃之后，系统作一些默认处理
能调用到此函数的按键，已经经过了前处理的所有判断了
特别的，目前只处理侧键
==============================================================================*/
BOOLEAN MMK_DefaultMSGKbdLater(
    uint32 keystatus,
    uint32 keycode
)
{
    BOOLEAN is_halt = FALSE;
    BOOLEAN halt_back = FALSE;

    /*--侧键处理*/
    is_halt = DefaultSideKey(keystatus, keycode, &halt_back);

    if (is_halt) return halt_back;

#ifdef GUI_DUMP_CTRL_TREE
    // 通过camera打印窗口树
    if (KEY_PRESSED == keystatus && KEY_CAMREA == keycode)
    {
        MMITHEME_DumpCurrentWindow();
    }
#endif

    /*--处理TAKESCREENPHOTO*/
   is_halt = DefaultTakeScreenPhoto(keystatus, keycode, &halt_back);

   if (is_halt) return halt_back;
    //临时实现一键SEARCH功能，客户需要可打开此处
    //(dal_keypad.h内目前无SEARCH物理键值，使用CALENDER替代)
#if 0
    if( KEY_PRESSED == keystatus
        && KEY_CALENDER == keycode
#ifndef TOUCH_PANEL_HWICON_SUPPORT_NONE
        && !MMIAPIIDLE_IsProhibitHandleHWIcon()
#endif
        )
    {
        MMIAPISEARCH_Entry();
    }
#endif

    return TRUE;
}

#if 0
/*==============================================================================
Description: 设置音量
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: audio模块无法提供一个简单的函数完成，不得不自己写
目前，mp3的蓝牙播出有特殊设置
==============================================================================*/
LOCAL void SetVolume(int16 vol)
{
    AUDIO_DEVICE_MODE_TYPE_E  mode = AUDIO_DEVICE_MODE_HANDFREE;
    BOOLEAN is_bt_play = MMIAPIBT_GetActiveBTHeadset();
    uint8 cur_vol = 0;

    if (vol > 0) cur_vol = vol;

    MMIAPISET_SetMultimVolume(cur_vol);
    
	if(!MMIAPIFM_IsPreviewFM())//alarm fm预览时不可以切换设备
	{
		if (MMIAPIFM_IsFmActive())
		{
		    mode = MMIAPIFM_GetFMCurrentDeviceMode();
		}
		else
		{
		    MMIAPISET_GetDeviceMode(&mode);
		}
	    if(is_bt_play)
	    {
	        mode = AUDIO_DEVICE_MODE_EARPHONE;
	    }
    }
    if (cur_vol == 0)
    {
        //MMIAUDIO_SetCurDevMode(mode);
        //MMIAUDIO_SetMute(TRUE);
        MMISRVAUD_SetMuteNohandle(TRUE);
    }
    else
    {
        //MMIAUDIO_SetMute(FALSE);
        //MMIAUDIO_SetVolume(mode, (uint32)cur_vol);
        MMISRVAUD_SetMuteNohandle(FALSE);        
        MMISRVAUD_SetVolumeNoHandle(cur_vol);
    }

    return;
}

/*==============================================================================
Description: 调节音量
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: 
==============================================================================*/
LOCAL void AdjustVolume(BOOLEAN is_increase)
{
    if (MMISRVAUD_IsAudPlaying(MMIAPIMP3_GetBGPlayerHandle()))
    {
        MMIAPIMP3_SetMp3Volume(is_increase);
    }
#ifdef TTS_SUPPORT
    else if(MMIAPITTS_GetPlayStatus())
    {
        uint8 cur_vol = MMISMS_GetTTSVol();

        cur_vol += (is_increase ? 1 : -1);
        
        if(cur_vol <= MMISET_VOL_MAX)
        {
            MMISMS_SetTTSVol(cur_vol);
            //MMIAPISET_InitAudioDevice(cur_vol);
            MMIAPITTS_SetTTSVolume(cur_vol);
        }
    }
#endif
    else
    {
        int16 cur_vol = (int16)MMIAPISET_GetMultimVolume();

        cur_vol += (is_increase ? 1 : -1);

        SetVolume(cur_vol);
    }
    
    return;
}

/*==============================================================================
Description: 调节音量面板的回调函数
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL void SetVolumeCB(MMICOM_PANEL_CALLBACK_T *para_ptr) 
{
    if(PNULL == para_ptr) return;

    if (MMISRVAUD_IsAudPlaying(MMIAPIMP3_GetBGPlayerHandle()))
    {
        MMIAPISET_SetMultimVolume(para_ptr->cur_value);
        MMISRVAUD_SetVolume(MMIAPIMP3_GetBGPlayerHandle(),para_ptr->cur_value);
    }
#ifdef TTS_SUPPORT
    else if(MMIAPITTS_GetPlayStatus())
    {
        uint8 cur_vol = para_ptr->cur_value;

        if(cur_vol <= MMISET_VOL_MAX)
        {
            MMISMS_SetTTSVol(cur_vol);
            //MMIAPISET_InitAudioDevice(cur_vol);
            MMIAPITTS_SetTTSVolume(cur_vol);
        }
    }
#endif
    else
    {
        SetVolume(para_ptr->cur_value);
    }

    return;
}

/*==============================================================================
Description: 弹出调节音量的面板
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/

LOCAL void OpenVolumePanel(void)
{
    MMICOM_PANEL_INFO_T panel_info = {0};

    if (MMIAPICOM_IsPanelOpened()) return;

    panel_info.x = -1;
    panel_info.y = -1;
    
#ifdef TTS_SUPPORT
    if(MMIAPITTS_GetPlayStatus())
    {
        panel_info.cur_value = MMISMS_GetTTSVol();
    }
    else
#endif
    {
        panel_info.cur_value = MMIAPISET_GetMultimVolume();
    }

    panel_info.last_value = MMIAPISET_GetLastMultimVolume();
    panel_info.min_value = MMISET_VOL_ZERO;   
    panel_info.max_value = MMISET_VOL_MAX;
    panel_info.set_callback = SetVolumeCB;
    
    MMIAPICOM_OpenPanelChildWin(MMK_GetFocusWinId(), &panel_info);

    return;
}
#endif

/*==============================================================================
Description: 调节音量面板的回调函数
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL void SetVolumeCB(MMICOM_PANEL_CALLBACK_T *para_ptr) 
{
    MMISRV_HANDLE_T handle = 0;
    if(PNULL == para_ptr) return;
    {
        if((handle = MMISRVAUD_GetHandleByVolumeType(MMISRVAUD_VOLUME_TYPE_MED)) != 0)
        {
            MMISRVAUD_SetVolume(handle, para_ptr->cur_value);
        }
        MMIAPISET_SetMultimVolume(para_ptr->cur_value);        
    }
    return;
}

/*==============================================================================
Description: 弹出调节音量的面板
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/

LOCAL void OpenVolumePanel(void)
{
    MMICOM_PANEL_INFO_T panel_info = {0};

    if (MMIAPICOM_IsPanelOpened()) return;

    panel_info.x = -1;
    panel_info.y = -1;
    
    panel_info.cur_value = MMIAPISET_GetMultimVolume();

    panel_info.last_value = MMIAPISET_GetLastMultimVolume();
    panel_info.min_value = MMISET_VOL_ZERO;
    panel_info.max_value = MMISET_VOL_MAX;
    panel_info.set_callback = SetVolumeCB;
    
    MMIAPICOM_OpenPanelChildWin(MMK_GetFocusWinId(), &panel_info);

    return;
}

/*****************************************************************************/
//  Description : Adjust multimedia volume and save it.
//  Global resource dependence : 
//  Author: bruce.chi
//  Note: 关闭背光, 不管现在的背光状态. 此函数仅在关机前调用.
/*****************************************************************************/    
LOCAL void AdjustMedVolume(MMISRV_HANDLE_T handle, BOOLEAN is_increase)
{
    uint8 vol = 0, vol_bak = 0;
    vol = MMIAPISET_GetMultimVolume();
    vol_bak = vol;
    if(is_increase)
    {
        if(vol < MMISET_VOL_MAX)
        {
            vol++;
        }
    }
    else
    {
        if(vol > 0)
        {
            vol--;
        }
    }
    if(vol != vol_bak)
    {
        MMISRVAUD_SetVolume(handle, vol);
        MMIAPISET_SetMultimVolume(vol);
    }
}
/*==============================================================================
Description: 调节来电铃声音量面板的回调函数
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL void SetRingVolumeCB(MMICOM_PANEL_CALLBACK_T *para_ptr) 
{
    if(PNULL == para_ptr) return;

    MMIENVSET_SetCurModeOptValue(CALL_RING_VOL, para_ptr->cur_value, MN_DUAL_SYS_MAX);

    return;
}

/*==============================================================================
Description: 弹出来电铃声调节音量的面板
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note:
==============================================================================*/
LOCAL void OpenRingVolumePanel(void)
{
    MMICOM_PANEL_INFO_T panel_info = {0};

    if (MMIAPICOM_IsPanelOpened()) return;

    panel_info.x = -1;
    panel_info.y = -1;
    panel_info.cur_value = MMIAPIENVSET_GetCurModeOptValue(MN_DUAL_SYS_MAX, CALL_RING_VOL);
    panel_info.last_value = MMIAPIENVSET_GetCurModeOptValue(MN_DUAL_SYS_MAX, CALL_RING_VOL);
    panel_info.min_value = MMISET_VOL_ZERO;
    panel_info.max_value = MMISET_VOL_MAX;
    panel_info.set_callback = SetRingVolumeCB;
    
    MMIAPICOM_OpenPanelChildWin(MMK_GetFocusWinId(), &panel_info);

    return;
}



/*==============================================================================
Description: MMK_DefaultMSGKbd - Side Key
Global resource dependence: NONE
Author: Lianxiang.Zhou
Note: up/down side key
需求原文
5. [锁屏界面]背景播放情况下侧键可进行音量调节，不弹出媒体音量面板；点击即增加或减少音量且立刻生效,侧键可亮屏
6. [黑屏锁屏情况下]背景播放情况下侧键可进行音量调节，不弹出媒体音量面板，侧键不亮屏
7. [黑屏]背景播放情况下侧键可进行音量调节, 弹出媒体音量面板，点击即增加或减少音量且立刻生效，屏被点亮 
8. 无媒体背景播放侧键打开音量窗口进行手机铃声音量调节,黑屏情况下亮屏且打开音量窗口调节音量(锁屏&黑屏锁屏情况下不响应)
9. 来电，侧键静音，音量窗口不弹出
10. 通话中，侧键调节通话音量(黑屏情况下，侧键调音，屏不被点亮，不弹音量窗口；PDA项目，物理电源键亮屏)
==============================================================================*/
#if 1
LOCAL BOOLEAN DefaultSideKey(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    MMISRV_HANDLE_T handle = 0;
    BOOLEAN is_halt = FALSE;
    BOOLEAN is_flip = MMIDEFAULT_GetFlipStatus();
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();
    BOOLEAN is_key_lock = MMIAPIKL_IsPhoneLocked();
    BOOLEAN is_up_key = FALSE;
    BOOLEAN is_incoming_call_ring = MMIAPICC_IsMtCallPlayingRing();
    BOOLEAN is_cc_connect = (MMIAPICC_IsInState(CC_MO_CONNECTING_STATE)
                             || MMIAPICC_IsInState(CC_MT_CONNECTING_STATE)
                             || MMIAPICC_IsInState(CC_CALL_CONNECTED_STATE));

    /* 侧键处理 */
    if (KEY_UPSIDEKEY == keycode)
    {
        is_up_key = TRUE;
    }
    else if (KEY_DOWNSIDEKEY != keycode)
    {
        return FALSE;
    }
        
    /* 翻盖的需求目前未作，暂不处理 */
    if (!is_flip) return FALSE;

    *halt_back_ptr = FALSE;

    if (is_incoming_call_ring && KEY_PRESSED == keystatus)
    {
        MMIAPICC_StopMtCallPlayingRing();

        return TRUE;
    }

    if (!is_back_light && !is_key_lock)
    {
        MMIDEFAULT_TurnOnBackLight();
    }

    switch (keystatus)
    {
    case KEY_PRESSED:
    case KEY_REPEATED:

        if (is_cc_connect)
        {
            is_halt = MMIAPICC_AdjustCallVolume(is_up_key);
        }
        else if((handle = MMISRVAUD_GetHandleByVolumeType(MMISRVAUD_VOLUME_TYPE_MED)) != 0)
        {
            AdjustMedVolume(handle, is_up_key);
            if(!is_key_lock)
            {
                OpenVolumePanel();
            }
            is_halt = TRUE;
        }
        else if((handle = MMISRVAUD_GetHandleByVolumeType(MMISRVAUD_VOLUME_TYPE_SELF)) != 0)
        {
            MMISRVAUD_NotifyVolumeSelfApp(is_up_key);
            is_halt = TRUE;
        }
        else
        {
            if (!MMIAPICOM_IsPanelOpened())
            {
                OpenRingVolumePanel();
                is_halt = TRUE;
            }
        }
        break;

    default:
        break;
    }
    
    return is_halt;
}


#else
LOCAL BOOLEAN DefaultSideKey(
    uint32 keystatus,
    uint32 keycode,
    BOOLEAN *halt_back_ptr
)
{
    BOOLEAN is_halt = FALSE;
    BOOLEAN is_flip = MMIDEFAULT_GetFlipStatus();
    BOOLEAN is_back_light = MMIDEFAULT_IsBacklightOn();
    BOOLEAN is_key_lock = MMIAPIKL_IsPhoneLocked();
    BOOLEAN is_up_key = FALSE;
    BOOLEAN is_incoming_call_ring = MMIAPICC_IsMtCallPlayingRing();
    BOOLEAN is_cc_connect = (MMIAPICC_IsInState(CC_MO_CONNECTING_STATE)
                             || MMIAPICC_IsInState(CC_MT_CONNECTING_STATE)
                             || MMIAPICC_IsInState(CC_CALL_CONNECTED_STATE));

    /* 侧键处理 */
    if (KEY_UPSIDEKEY == keycode)
    {
        is_up_key = TRUE;
    }
    else if (KEY_DOWNSIDEKEY != keycode)
    {
        return FALSE;
    }
        
    /* 翻盖的需求目前未作，暂不处理 */
    if (!is_flip) return FALSE;

    *halt_back_ptr = FALSE;

    if (is_incoming_call_ring && KEY_PRESSED == keystatus)
    {
        MMIAPICC_StopMtCallPlayingRing();

        return TRUE;
    }

    /*
    1 黑屏锁屏
    2 锁屏非黑屏
    3 黑屏非锁屏
    4 非黑屏非锁屏，即正常状态
    */
    if (is_key_lock && !is_back_light)
    {
        switch (keystatus)
        {
        case KEY_PRESSED:
        case KEY_REPEATED:

            if(MMISRVAUD_GetVolumeNoHandle() != MMISRVAUD_VOLUME_LEVEL_NONE)
            {
                AdjustVolume(is_up_key);
                is_halt = TRUE;
            }

            break;

        default:
            break;
        }
    }
    else if (is_key_lock)
    {
        switch (keystatus)
        {
        case KEY_PRESSED:
        case KEY_REPEATED:

            if(MMISRVAUD_GetVolumeNoHandle() != MMISRVAUD_VOLUME_LEVEL_NONE)
            {
                AdjustVolume(is_up_key);
                is_halt = TRUE;
            }

            break;

        default:
            break;
        }
    }
    else if (!is_back_light)
    {
        //这个分支基本是不会走到的，因为之前已经将屏幕点亮了
        //当然，如果没有点亮，此处会点亮，之后的处理，将会进入亮屏的处理
        switch (keystatus)
        {
        case KEY_PRESSED:
        case KEY_REPEATED:
            MMIDEFAULT_TurnOnBackLight();

            if(MMISRVAUD_GetVolumeNoHandle() != MMISRVAUD_VOLUME_LEVEL_NONE)
            {
                if (!MMIAPICOM_IsPanelOpened())
                {
                    AdjustVolume(is_up_key);
                    OpenVolumePanel();
                    is_halt = TRUE;
                }
            }
            else if (is_cc_connect)
            {
                is_halt = MMIAPICC_AdjustCallVolume(is_up_key);
            }

            break;

        default:
            break;
        }
    }
    else
    {
        switch (keystatus)
        {
        case KEY_PRESSED:
        case KEY_REPEATED:

            if (is_cc_connect)
            {
                is_halt = MMIAPICC_AdjustCallVolume(is_up_key);
            }
            else if(MMISRVAUD_GetVolumeNoHandle() != MMISRVAUD_VOLUME_LEVEL_NONE)
            {
                if (!MMIAPICOM_IsPanelOpened())
                {
                    AdjustVolume(is_up_key);
                    OpenVolumePanel();
                    is_halt = TRUE;
                }
            }
            else
            {
                if (!MMIAPICOM_IsPanelOpened())
                {
                    OpenRingVolumePanel();
                    is_halt = TRUE;
                }
            }

            break;

        default:
            break;
        }
    }
    

    return is_halt;
}
#endif

/*****************************************************************************/
//  Description : handle flip down message
//  Global resource dependence : 
//  Author: Great.Tian
//  Note:
/*****************************************************************************/
LOCAL void HandleFlipDown(
                          void
                          )
{

    if (MMIAPICC_IsInState(CC_IN_CALL_STATE))
    {
#ifndef WIN32
        if(!GPIO_CheckHeadsetStatus())
#endif
        {
            //不能马上退回IdleWin
            MMIAPICC_ReleaseAllCallExt(CC_RELEASE_ALL_PROMPT);
        }
    }
    else
    {
        MMK_ReturnIdleWin(); // Idle下的Timer应该挂起，可以发送一个消息给Idle
    }
}

/*****************************************************************************/
//  Description : Handle red key process
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMK_HandleRedKeyProcess(void)
{
    if (MMIAPICC_IsInState(CC_IN_CALL_STATE)&&MMIAPICC_IsCallByMMI())
    {
        BOOLEAN is_connecting = TRUE;

        //不能马上退回IdleWin
#if !(defined(KEYPAD_TYPE_FOUR_KEY_UNIVERSE) && defined(MMI_CALL_MINIMIZE_SUPPORT))
        MMIAPICC_ReleaseAllCallExt(CC_RELEASE_ALL_PROMPT);
#endif
        is_connecting = MMIAPICC_IsInState(CC_MO_CONNECTING_STATE) || MMIAPICC_IsInState(CC_MT_CONNECTING_STATE);

        if (!is_connecting)
        {
            MMK_ReturnIdleWin();
        }
    }
    //去电窗口正在打开的时候，不做任何操作
    else if (!MMIAPICC_IsInState(CC_IN_CALL_STATE))
    {
        MMK_ReturnIdleWin();
    }
}

/*****************************************************************************/
//  Description : Handle headset key process
//  Global resource dependence : 
//  Author: tao.xue
//  Note: 
/*****************************************************************************/
LOCAL void MMK_HandleHeadSetKeyProcess(MMI_MESSAGE_ID_E  msg_id)
{
    //The different between Red_key and Headset_button is
    //Red_key aways hang up a call regardless it's call state
    //Headset_button hang up call by it's call_state:CC_CALLING_STATE,CC_CALL_STATE,CC_HOLD_STATE
    //but it also connect call when there is a call in this state:CC_INCOMING_CALL_STATE,CC_WAITING_STATE
    if (MMIAPICC_IsInState(CC_IN_CALL_STATE))
    {
        if (MMK_IsOpenWin(MMICC_ANIMATION_WIN_ID))
        {
            MMK_PostMsg(MMICC_ANIMATION_WIN_ID,msg_id,PNULL,0);
        }
        else if (MMK_IsOpenWin(MMICC_STATUS_WIN_ID))
        {
            MMK_PostMsg(MMICC_STATUS_WIN_ID,msg_id,PNULL,0);
        }
        else if (GPIO_CheckHeadsetStatus() && MMI_NONE_TIMER_ID == g_mmi_headset_time_id)
        {
            //SCI_TRACE_LOW:"MMK_HandleHeadSetKeyProcess: warning! There is no call window exists when in call state!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2535_112_2_18_2_0_0_140,(uint8*)"");
            MMIAPICC_ReleaseCallByRedkey();
        }
    }
}

/*****************************************************************************/
// 	Description : 是否含statusbar
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_IsWinIncludeStatusbar(
                                         MMI_HANDLE_T win_handle
                                         )
{
    BOOLEAN is_include = FALSE;

    // 判断条件需与MMK_CreateDefaultControl保持一致
#if defined(MENU_CRYSTALCUBE_SUPPORT) || defined (MENU_CYCLONE_SUPPORT) 
    if ((MAIN_MAINMENU_WIN_ID == MMK_GetWinId(MMK_ConvertIdToHandle(win_handle)))
		&& ((GUIMENU_STYLE_CYCLONE == MMIAPISET_GetMainmenuStyle())||(GUIMENU_STYLE_CRYSTAL_CUBE== MMIAPISET_GetMainmenuStyle())))
	{
		//no status bar
	}
    else
#endif
    {
        if (MMK_IsIncludeStatusBar(win_handle)
            || (MMK_GetWinDisplayStyleState(win_handle, WS_HAS_STATUSBAR)
                && !MMK_GetWinDisplayStyleState(win_handle, WS_HAS_TRANSPARENT)
                && !MMK_IsChildWin(win_handle)))
        {
            is_include = TRUE;
        }
    }
    return is_include;
}

/*****************************************************************************/
// 	Description : 是否含softkey
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_IsWinIncludeSoftkey(
                                       MMI_HANDLE_T win_handle
                                       )
{
    BOOLEAN is_include = FALSE;

    // 判断条件需与MMK_CreateDefaultControl保持一致
#if defined(MENU_CRYSTALCUBE_SUPPORT) || defined (MENU_CYCLONE_SUPPORT) 
    if ((MAIN_MAINMENU_WIN_ID == MMK_GetWinId(MMK_ConvertIdToHandle(win_handle)))
		&& ((GUIMENU_STYLE_CYCLONE == MMIAPISET_GetMainmenuStyle())||(GUIMENU_STYLE_CRYSTAL_CUBE== MMIAPISET_GetMainmenuStyle())))
	{
		//no status bar
	}
    else
#endif
    {
        if ((0 != MMK_GetWinSoftkeyCtrlId(win_handle)
                && GUIWIN_GetSoftkeyVisible(win_handle))
            || MMK_GetWinDisplayStyleState(win_handle, WS_HAS_SOFTKEY))
        {
            is_include = TRUE;
        }
    }
    return is_include;
}

/*****************************************************************************/
// 	Description : 是否含title
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_IsWinIncludeTitle(
                                     MMI_HANDLE_T win_handle
                                     )
{
    BOOLEAN is_include = FALSE;

    // 判断条件需与MMK_CreateDefaultControl保持一致
#if defined(MENU_CRYSTALCUBE_SUPPORT) || defined (MENU_CYCLONE_SUPPORT) 
    if ((MAIN_MAINMENU_WIN_ID == MMK_GetWinId(MMK_ConvertIdToHandle(win_handle)))
		&& ((GUIMENU_STYLE_CYCLONE == MMIAPISET_GetMainmenuStyle())||(GUIMENU_STYLE_CRYSTAL_CUBE== MMIAPISET_GetMainmenuStyle())))
	{
		//no TITLE bar
	}
    else
#endif
    {
        if (0 != MMK_GetWinTitleCtrlId(win_handle)
        || MMK_GetWinDisplayStyleState(win_handle, WS_HAS_TITLE))
        {
            is_include = TRUE;
        }
    }
    return is_include;
}

/*****************************************************************************/
// 	Description : create default control
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_CreateDefaultControl(
                                   MMI_HANDLE_T win_handle,
                                   MMK_DEFAULT_CTRL_PARAM_T *param_ptr
                                   )
{
    BOOLEAN result = TRUE;

    if (PNULL == param_ptr)
    {
        result = FALSE;
    }

#if defined(MENU_CRYSTALCUBE_SUPPORT) || defined (MENU_CYCLONE_SUPPORT) 
    if ((MAIN_MAINMENU_WIN_ID == MMK_GetWinId(MMK_ConvertIdToHandle(win_handle)))
		&& ((GUIMENU_STYLE_CYCLONE == MMIAPISET_GetMainmenuStyle())||(GUIMENU_STYLE_CRYSTAL_CUBE== MMIAPISET_GetMainmenuStyle())))
	{
		//no status bar
	}
    else
#endif
    {
		// 判断条件需与MMK_IsWinIncludeStatusbar保持一致
		if (result
			&& MMK_GetWinDisplayStyleState(win_handle, WS_HAS_STATUSBAR)
			&& !MMK_GetWinDisplayStyleState(win_handle, WS_HAS_TRANSPARENT)
			&& !MMK_IsChildWin(win_handle))
		{
			if (!MMK_IsIncludeStatusBar(win_handle))
			{
				result = GUIWIN_CreateStbDyn(win_handle);
			}
		}
    }

    return result;
}

/*****************************************************************************/
//  Description : HandleHandsetTimer           
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void HandleHandsetTimer(
                              uint8 timer_id, 
                              uint32 param
                              )
{
    //SCI_TRACE_LOW:"VirWin_HandleMsg: received headset timer"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2681_112_2_18_2_0_0_141,(uint8*)"");
    MMK_StopTimer(g_mmi_headset_time_id);
    g_mmi_headset_time_id = 0;
}

/*****************************************************************************/
//  Description : handle the public message after window           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMK_HandlePublicKey(
                                MMI_MESSAGE_ID_E  msg_id,  //the message id
                                DPARAM            param_ptr  // param
                                )
{
#ifdef WIN32 
    static LCD_DIRECT_E s_eDirect = LCD_DIRECT_NORMAL;
#endif
    switch(msg_id)
    {
#ifdef MMI_PDA_SUPPORT
#ifdef KEYPAD_TYPE_FOUR_KEY_UNIVERSE
    case MSG_KEYLONG_POWER:
        if (MMIAPIENG_RedkeyIsEnableInIdle())
        {
            if (STARTUP_NORMAL == MMIAPIPHONE_GetStartUpCondition())
            {
                //pda长按power, 弹出关机选择窗口, to do...
                // 这里暂时用关机替代
                if (s_is_down_keystatus_backlight_on)
                {
					MMIAPISET_LongpowerCreatmenu();
                    //MMIAPIPHONE_PowerOff(); //开机
                }
            }
            else
            {
                MMIAPIPHONE_PowerReset(); //开机
            }
        }
        break;

    case MSG_KEYUP_POWER:
        //pda按power, 一键灭屏, to do...
        if (s_is_down_keystatus_backlight_on)
        {
#ifdef LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT        
#ifdef MRAPP_SUPPORT
			if(MMIMRAPP_GetVMState() == VM_RUN)
			{
				MMIDEFAULT_CloseAllLight();
			}
			else
			{
				MMISEPC_CloseAllLight();
			}
#else
            MMISEPC_CloseAllLight();
#endif
#else
            MMIDEFAULT_CloseAllLight();
#endif

        }
        break;
#endif
#endif

    case MSG_KEYLONG_RED:
        {
#ifdef MMI_PDA_SUPPORT
#ifdef MMI_TASK_MANAGER
            MMI_OpenAppletManager();
#endif
#else
            MMI_WIN_ID_T     focus_win_id = MMK_GetFocusWinId();
            MMI_WIN_ID_T     prev_win_id = MMK_GetPrevWinId(focus_win_id);

            if ((PHONE_STARTUP_CHARGE_WIN_ID != focus_win_id) &&
                (PHONE_STARTUP_CHARGE_OK_WIN_ID != focus_win_id) &&
                (PHONE_STARTUP_ALARM_WIN_ID != focus_win_id) &&
                (MMIAPIALM_GetAlarmPromptWinId() != focus_win_id) &&
                (!((MMIPUB_QUERY_WIN_ID == focus_win_id) && (PHONE_STARTUP_ALARM_WIN_ID == prev_win_id))))//是否开机询问界面需要特殊处理long red
            {
                //关机
                if (MMIAPIENG_RedkeyIsEnableInIdle())
                {
                    if (STARTUP_NORMAL == MMIAPIPHONE_GetStartUpCondition())
                    {
                        MMIDEFAULT_TurnOnBackLight();
                        MMIAPIPHONE_PowerOff(); //关机
                    }
                    else
                    {
                        MMIAPIPHONE_PowerReset(); //开机
                    }
                }
                else
                {
                    //SCI_TRACE_LOW:"MMK_DefaultMSGKbd: can not power off because be forbidden by engineer mode"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2796_112_2_18_2_0_1_142,(uint8*)"");
                }
            }
#endif
        }
        break;

#ifdef LCD_SLIDE_SUPPORT        
    case MSG_KEYDOWN_SLIDE:     //关闭滑盖
    case MSG_KEYUP_SLIDE:       //打开滑盖
        MMIDEFAULT_HandleSlideKey(msg_id);
        break;
#endif

    //避免关机和锁屏冲突, 改为响应keyup
#ifdef MMI_PDA_SUPPORT
    case MSG_KEYUP_RED:
        if(MMIAPIKL_IsKLWinOnTop())
        {
             //pda按power, 一键灭屏, to do...
            if (s_is_down_keystatus_backlight_on)
            {
#ifdef LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT        
                MMISEPC_CloseAllLight();
#else
                MMIDEFAULT_CloseAllLight();
#endif
            }
        }
        else     
        {
            if (s_is_down_keystatus_backlight_on)
            {
                MMK_HandleRedKeyProcess();
            }
        }
        
        break;
#else   
    case MSG_APP_RED:      
        MMK_HandleRedKeyProcess();
        break;
#endif

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone. 
    case MSG_KEYDOWN_FLIP:
        //SCI_TRACE_LOW:"MMK_HandlePublicKey: MSG_KEYDOWN_FLIP!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2842_112_2_18_2_0_1_143,(uint8*)"");
        MMIDEFAULT_HandleFlipKey(FALSE);
        HandleFlipDown();
        break;

    case MSG_KEYUP_FLIP:
        //SCI_TRACE_LOW:"MMK_HandlePublicKey: MSG_KEYUP_FLIP!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2848_112_2_18_2_0_1_144,(uint8*)"");
        MMIDEFAULT_HandleFlipKey(TRUE);
        break;
#endif
        
    case MSG_KEYDOWN_HEADSET_BUTTON:
        //SCI_TRACE_LOW:"MMK_HandlePublicKey: MSG_KEYDOWN_HEADSET_BUTTON!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2854_112_2_18_2_0_1_145,(uint8*)"");
        MMK_HandleHeadSetKeyProcess(msg_id);
        break;
        
    case MSG_KEYDOWN_HEADSET_DETECT:
        //SCI_TRACE_LOW:"MMK_HandlePublicKey, msg_id = MSG_KEYDOWN_HEADSET_DETECT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2859_112_2_18_2_0_1_146,(uint8*)"");

        if (0 < g_mmi_headset_time_id)
        {
            MMK_StopTimer(g_mmi_headset_time_id);
            g_mmi_headset_time_id = 0;
        }
        g_mmi_headset_time_id = MMK_CreateTimerCallback(MMI_HEADSET_TIMER_OUT,  HandleHandsetTimer, NULL, FALSE);
        
        // 设置LCD和KeyBad的背景灯
        MMIDEFAULT_TurnOnBackLight();

        //刷新小屏的显示，耳机状态
        MMISUB_UpdateDisplaySubLcd();
        MMIDEFAULT_TurnOnBackLight();      
        break;

    case MSG_KEYUP_HEADSET_DETECT:    
        //SCI_TRACE_LOW:"MMK_HandlePublicKey, msg_id = MSG_KEYUP_HEADSET_DETECT"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2877_112_2_18_2_0_1_147,(uint8*)"");
        
        // 设置LCD和KeyBad的背景灯
        MMIDEFAULT_TurnOnBackLight();

        //刷新小屏的显示，耳机状态
        MMISUB_UpdateDisplaySubLcd();
        break;
#ifndef SUBLCD_SIZE_NONE
	case MSG_KEYDOWN_UPSIDE:
	case MSG_KEYDOWN_DOWNSIDE:
		//合盖情况下，停止播放铃声和震动，如来电，短信，闹钟等
		if (!MMIAPIMP3_IsPlayerPlaying() && !MMIAPIKUR_IsPlayerPlaying() && (FM_PLAYING != MMIAPIFM_GetStatus()))
		{
			if(MMIDEFAULT_GetFlipStatus())
			{
				MMIAPISET_StopAllVibrator();
				MMIAPISET_StopAllRing(TRUE);
				//SCI_TRACE_LOW:"MMK_HandlePublicKey:MSG_KEYDOWN_DOWNSIDE stop ring"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2895_112_2_18_2_0_1_148,(uint8*)"");
			}
		}
		else
		{
			//SCI_TRACE_LOW:"MMK_HandlePublicKey: can not power off because be forbidden by engineer mode"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2900_112_2_18_2_0_1_149,(uint8*)"");
		}
		break;   
#endif        
    case MSG_KEYLONG_HASH:
        MMIAPITVOUT_Close(); //close tv out
        break;

#ifdef DPHONE_SUPPORT 
	case MSG_KEYUP_HOOK:
		//---摘机处理
        if(!MMIAPICC_IsInState(CC_IN_CALL_STATE))
        {
		      MMIAPIIDLE_OpenDialWin(msg_id, FALSE); //---打开拨号窗口
    	}
		break;
	case MSG_KEYDOWN_HOOK:
		//---挂机处理
		//SCI_TRACE_LOW:"---WD:MMK_HandlePublicKey(),hook key is pressed"
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2918_112_2_18_2_0_1_150,(uint8*)"");
       if (!MMIDEFAULT_GetHandFreeStates())
       {
			if (TRUE == MMIAPICC_IsInState(CC_IN_CALL_STATE))
			{
				//---不能马上退回IdleWin
				if (MMK_IsOpenWin( MMICC_STATUS_WIN_ID ))
				{
		            MMK_CloseMiddleWin(MMK_GetFocusWinId(), MMICC_STATUS_WIN_ID);
				}
				//SCI_TRACE_LOW:"TRUE == CC_IsInCall ,CC_RELEASE_ALL_NORMAL"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2928_112_2_18_2_0_1_151,(uint8*)"");
				MMIAPICC_ReleaseAllCallExt(CC_RELEASE_ALL_PROMPT);
			}
			else
			{
			    //SCI_TRACE_LOW:"---WD:hook key is pressed,return to idle windows"
			    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2933_112_2_18_2_0_1_152,(uint8*)"");
				MMK_ReturnIdleWin();
			}
       	}
		break;
		
	case MSG_KEYDOWN_HANDSFREE:
		//SCI_TRACE_LOW:"---WD:MMK_HandlePublicKey(),HANDSFREE key is pressed"
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2940_112_2_18_2_0_1_153,(uint8*)"");
		if ((TRUE ==MMIAPICC_IsInState(CC_IN_CALL_STATE)) && MMIDEFAULT_GetHookStates() == FALSE)
		{
			//---不能马上退回IdleWin
			//SCI_TRACE_LOW:"---WD:handfree key is pressed,Release all call"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2944_112_2_18_2_0_1_154,(uint8*)"");
			MMIAPICC_ReleaseAllCallExt(CC_RELEASE_ALL_PROMPT);
		}
		else if (MMIDEFAULT_GetHandFreeStates() && !MMIAPICC_IsInState(CC_IN_CALL_STATE)) //added by taiping.lai 2011/01/10 非idle也可以进入拨号状态
		{  
		     //---在免提状态下,并且当前窗口是idle窗口,就直接打开拨号窗口。
		    //SCI_TRACE_LOW:"---WD:MMK_HandlePublicKey(),open dial window"
		    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_2950_112_2_18_2_0_1_155,(uint8*)"");
			MMIAPIIDLE_OpenDialWin(msg_id, FALSE);
		} 
        break;
#endif

#ifdef WIN32
    case 0xfa60:
        {
            static uint32 s_angle = 0;
            s_angle = (s_angle+1) % 4;

            MMK_SetScreenAngle( s_angle );
        }
        break;
#ifdef SUBLCD_SIZE_NONE    
    case 0xfa19:
        {
            extern void MMIPUB_Test( void );

            MMIPUB_Test();
//            MMISOFTKEY_Test();
        }
        break;
#endif
    case 0xfb19:
        {
//            MmiSignalS* sendSignal  = PNULL;
//            static uint32 s_type = 0;
//
//            s_type = (s_type+1)%MMI_GESTURE_MAX;
//
//            if ( 0 == s_type )
//            {
//                s_type++;
//            }
//
//            MmiCreateSignal( MMI_TP_GESTURE, sizeof(MmiTPPress), &sendSignal);
//            
//            sendSignal->sig.TpPress.x  = 0;
//            sendSignal->sig.TpPress.y  = 0;
//            sendSignal->sig.TpPress.gesture_type = s_type;
//            sendSignal->send = P_TPP;
//            
//            MmiSendSignal(TASK_FL_ID, sendSignal);
            MMIPHONE_ChargeHandlePsAndRefMsg(PNULL, CHR_MSG_CAP_IND, PNULL);
        }
        break;

    case 0xfa00: // 模拟器上模拟闹钟
        MMIALMSVC_StartService();
        break;
        
    case 0X82:
        TestKey1Func();
        break;

    case 0X83:
        TestKey2Func();
        break;

    case 0X84:
        TestKey3Func();
        break;

    case 0X85:
        TestKey4Func();
        break;

    case 0xfa5a://0xfa57: // TST3 key
        if(s_eDirect == LCD_DIRECT_NORMAL)
		{
			s_eDirect = LCD_DIRECT_ROT_270;
		}
		else
		{
			s_eDirect = LCD_DIRECT_NORMAL;
		}
		LCD_SetDirection(0,s_eDirect);
        MMK_PostMsg(MMK_GetFirstAppletHandle(),MSG_UPDATE_SCREEN,PNULL,0);
        break;
#endif
        
    default:
        //SCI_TRACE_LOW:"MMK_HandlePublicKey: can't handle msg! the msg_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3034_112_2_18_2_0_1_156,(uint8*)"d", msg_id);
        break;
    }
}

/**********************2*******************************************************/
//  Description : init the default modual           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_InitDefault(
                                   BOOLEAN is_backlight_on
                                   )
{
    if(is_backlight_on)
    {
        s_backlight_status  = MMI_BACKLIGHT_ON; //lcd is light up in product driver
    }
    else
    {
        s_backlight_status = MMI_BACKLIGHT_OFF; //关机闹钟响，lcd背光默认是关闭的
    }

    s_backlight_ctrol     = MMI_BACKLIGHT_CTRL_ALWAYS_ON;
    g_mmi_headset_time_id = 0;
    s_is_allow_keylock    = FALSE;
}

/*****************************************************************************/
//  Description : set the back light of the mmi moudal, 由应用模块调用，不进入Sleep
//                这个函数用来打开关闭LCD和KEYBAD的背景灯，但是要判断当前的状态。
//                当前状态主要包括Setting的设置，应用的设置(比如DC环境下)        
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetBackLight(
                                    BOOLEAN is_light
                                    )
{
    if (is_light)
    {
        MMIDEFAULT_TurnOnBackLight();
    }
    else
    {
        //do not supply this function
    }
}


/******************************************************************************/
/*! \brief  turn on lcd back light and keyboard light
 *  \author Bruce.Chi  
 *  \param[in]  void
 */
/******************************************************************************/
PUBLIC void MMIDEFAULT_TurnOnBackLight(void)
{
    
    MMISET_KEYBOARD_LIGHT_SET_T     keyboard_light_info = {0};
    SCI_TIME_T                      sys_time = {0};   
    
    MMIDEFAULT_TurnOnLCDBackLight();

    MMIAPISET_GetKeyBoardLightTime(&keyboard_light_info);
    //get current system time
    TM_GetSysTime(&sys_time); 
    if (MMISET_KEYBOARD_LIGHT_CLOSE == keyboard_light_info.keyboard_type)
    {
        MMIDEFAULT_SetKeybadBackLight(FALSE);
    }
    else if (MMISET_KEYBOARD_LIGHT_AUTO == keyboard_light_info.keyboard_type 
            &&(sys_time.hour >= MMISET_OPEN_KEYBOARD_LIGHT_MIN_HOUR && sys_time.hour < MMISET_OPEN_KEYBOARD_LIGHT_MAX_HOUR))
    {
        MMIDEFAULT_SetKeybadBackLight(FALSE);
    }
    else
    {
        MMIDEFAULT_SetKeybadBackLight(TRUE);
    }
}

/******************************************************************************/
/*! \brief  turn on lcd back light
 *  \author Bruce.Chi  
 *  \param[in]  void
 */
/******************************************************************************/
PUBLIC void MMIDEFAULT_TurnOnLCDBackLight(void)
{
    GUI_RECT_T  full_screen_rect = MMITHEME_GetFullScreenRect();
    
    //SCI_TRACE_LOW:"MMIDEFAULT_TurnOnLCDBackLight: s_backlight_ctrol = %d, s_backlight_status = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3120_112_2_18_2_0_1_157,(uint8*)"dd", s_backlight_ctrol, s_backlight_status);

    switch(s_backlight_ctrol)
    {
    case MMI_BACKLIGHT_CTRL_NORMAL:         //按键能点亮背光。根据setting的设置能够关闭背光
        switch(s_backlight_status)
        {
        case MMI_BACKLIGHT_ON:
            //重新设置一下timer
            StartSettingLCDBackLightTimer();
            break;

        case MMI_BACKLIGHT_HALF_ON:
            s_backlight_status = MMI_BACKLIGHT_ON;
            GPIO_SetLcdBackLight(TRUE);
            MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
            MMI_TraceAutoBackLightStatus(s_backlight_status);
            //重新设置一下timer
            StartSettingLCDBackLightTimer();            
            
            break;

        case MMI_BACKLIGHT_OFF:
            s_backlight_status = MMI_BACKLIGHT_ON;
        
            //唤醒LCD
            MMIDEFAULT_SetLcdSleep(FALSE);
            
            // 当点亮屏幕的时候需要Invalidate整个屏幕
            MMITHEME_InitMainLcdStortUpdateRect();
            MMITHEME_StoreUpdateRect(MMITHEME_GetDefaultLcdDev(),full_screen_rect);

            //send system backlight turn on message to  control and window
            MMK_DispMsgToWin(MSG_BACKLIGHT_TURN_ON, PNULL);
            MMITHEME_UpdateRect();
#ifdef MMI_TURNON_BACKLIGHT_EFFECT_SUPPORT//点亮屏幕动画 
            if(!DrawTurnOnBackLightAnim())
#endif
            {           
                //点亮背光
                GPIO_SetLcdBackLight(TRUE);
            }
            MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
            MMI_TraceAutoBackLightStatus(s_backlight_status);
            //退出deep sleep
            MMIDEFAULT_SetDeepSleep(FALSE);            
            // @Great.Tian, handle
            MMIAPIIDLE_HandleLCDBacklight(TRUE);
            
            //开启屏幕背光timer
            StartSettingLCDBackLightTimer();
            
            break;

        default:
            break;
        }
        break;

    case MMI_BACKLIGHT_CTRL_ALWAYS_ON:      //长开
        switch(s_backlight_status)
        {
        case MMI_BACKLIGHT_ON:
            //按键不处理。长开对应的操作应该在设置长开的函数中实现。
            break;

        case MMI_BACKLIGHT_HALF_ON:
            //按键不处理。长开对应的操作应该在设置长开的函数中实现。
            break;

        case MMI_BACKLIGHT_OFF:
            //按键不处理。（正常情况下不应该出现这个CASE）长开对应的操作应该在设置长开的函数中实现。
            s_backlight_status = MMI_BACKLIGHT_ON;
        
            //唤醒LCD
            MMIDEFAULT_SetLcdSleep(FALSE);
            
            // 当点亮屏幕的时候需要Invalidate整个屏幕
            MMITHEME_InitMainLcdStortUpdateRect();
            MMITHEME_StoreUpdateRect(MMITHEME_GetDefaultLcdDev(),full_screen_rect);

            //send system backlight turn on message to  control and window
            MMK_DispMsgToWin(MSG_BACKLIGHT_TURN_ON, PNULL);

            MMITHEME_UpdateRect();
            
            //点亮背光
            GPIO_SetLcdBackLight(TRUE);
            MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
            MMI_TraceAutoBackLightStatus(s_backlight_status);
            //退出deep sleep
            MMIDEFAULT_SetDeepSleep(FALSE);
            
            // @Great.Tian, handle
            MMIAPIIDLE_HandleLCDBacklight(TRUE);
            
            //开启屏幕背光timer
            StartSettingLCDBackLightTimer();
          
            break;

        default:
            break;
        }
        break;

    case MMI_BACKLIGHT_CTRL_ALWAYS_HALF_ON: //始终半暗  （注意：ALWAYS_ON 和 HALF_ON不允许互相切换，必须从NORMAL状态切换）
        switch(s_backlight_status)
        {
        case MMI_BACKLIGHT_ON:
            //按键不处理。（正常情况下不应该出现这个CASE）始终半暗对应的操作应该在设置始终半暗的函数中实现。
            break;

        case MMI_BACKLIGHT_HALF_ON:
            //按键不处理。（正常情况下不应该出现这个CASE）始终半暗对应的操作应该在设置始终半暗的函数中实现。
            break;

        case MMI_BACKLIGHT_OFF:
            //按键不处理。（正常情况下不应该出现这个CASE）始终半暗对应的操作应该在设置始终半暗的函数中实现。
            break;

        default:
            break;
        }
        break;

    case MMI_BACKLIGHT_CTRL_ALWAYS_OFF:     //长关
        switch(s_backlight_status)
        {
        case MMI_BACKLIGHT_ON:
            break;

        case MMI_BACKLIGHT_HALF_ON:
            break;

        case MMI_BACKLIGHT_OFF:
            break;

        default:
            break;
        }
        break;

    case MMI_BACKLIGHT_CTRL_NORMAL_HALF_ON: //HALF ON状态位置5秒钟
        switch(s_backlight_status)
        {
        case MMI_BACKLIGHT_ON:
            //按键不处理（正常情况下不应该出现这个CASE）
            break;

        case MMI_BACKLIGHT_HALF_ON:
            if (MMIAPIKL_IsPhoneLocked())
            {
                //重新设置一下timer
                MMK_StartWinTimer(
                    VIRTUAL_WIN_ID,
                    MMI_LCD_LIGHT_TIMER_ID,
                    MMI_BACKLIGHT_HALF_ON_DELAY_TIME,
                    FALSE);
            }
            else
            {
                // 如果在NORMAL半暗的情况下，用户turn on back light，则转变成NORMAL状态。
                SetNormalHalfOnBackLight(FALSE);

                //打开背光
                s_backlight_status = MMI_BACKLIGHT_ON;
                GPIO_SetLcdBackLight(TRUE);
                MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
                MMI_TraceAutoBackLightStatus(s_backlight_status);
                //重新设置一下timer
                StartSettingLCDBackLightTimer();            
            }
            break;

        case MMI_BACKLIGHT_OFF:
            //这个由按键控制，主要用于键盘锁状态
            s_backlight_status = MMI_BACKLIGHT_HALF_ON;     

            //唤醒LCD
            MMIDEFAULT_SetLcdSleep(FALSE);
            
            //send system backlight turn on message to  control and window
            MMK_DispMsgToWin(MSG_BACKLIGHT_TURN_ON, PNULL);
            
            // 当点亮屏幕的时候需要Invalidate整个屏幕
            MMITHEME_UpdateRect();
            
            //点亮背光
            GPIO_SetLcdBackLight(TRUE);
            LCD_SetBackLightBrightness(MMI_BACK_LIGHT_HALF_ON_DEBGREE);
//          MMIAPISET_UpdateLCDContrast(GUI_MAIN_LCD_ID, MMIAPISET_GetCurrentContrast());
            MMI_TraceAutoBackLightStatus(s_backlight_status);
            
            //退出deep sleep
            MMIDEFAULT_SetDeepSleep(FALSE);
            
            // @Great.Tian, handle
            MMIAPIIDLE_HandleLCDBacklight(TRUE);
            
            //start half on timer
            MMK_StartWinTimer(
                VIRTUAL_WIN_ID,
                MMI_LCD_LIGHT_TIMER_ID,
                MMI_BACKLIGHT_HALF_ON_DELAY_TIME,
                FALSE);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }  

}

/******************************************************************************/
/*! \brief  whether allow turn off backlight           
 *  \author Great.Tian
 *  \param[in] allow_light whether allow light.
 */
/******************************************************************************/
PUBLIC void MMIDEFAULT_AllowTurnOffBackLight(BOOLEAN allow_turn_off)
{
    //SCI_TRACE_LOW:"MMIDEFAULT_AllowBackLight: s_backlight_ctrol=%d, allow_light = %d!"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3345_112_2_18_2_0_2_158,(uint8*)"dd", s_backlight_ctrol, allow_turn_off);
#ifdef VT_SUPPORT
    if (MMIAPIVT_IsVtCalling() && allow_turn_off)
    {
        //SCI_TRACE_LOW:"MMIDEFAULT_AllowBackLight vt calling "
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3349_112_2_18_2_0_2_159,(uint8*)"");
        return;
    }
#endif
#ifdef WRE_SUPPORT
    if(MMIWRE_Actived() && allow_turn_off)
    {
        return;
    }
#endif
    if (s_backlight_ctrol != MMI_BACKLIGHT_CTRL_ALWAYS_HALF_ON)         //ALWAYS_ON 和 HALF_ON不允许互相切换，必须从NORMAL状态切换
    {
        if (allow_turn_off)
        {
            s_backlight_ctrol = MMI_BACKLIGHT_CTRL_NORMAL;
        }
        else
        {
            s_backlight_ctrol = MMI_BACKLIGHT_CTRL_ALWAYS_ON;
        }
        //    s_allow_turnoff_backlight = allow_turn_off;
        
        if (allow_turn_off)
        {
            MMIDEFAULT_StartLcdBackLightTimer();
        }
    }
    MMIDEFAULT_AllowOpenKeylocWin(allow_turn_off);
}

/******************************************************************************/
/*! \brief  whether allow lcd backlight half on        
 *  \author Great.Tian
 *  \param[in] 
 */
/******************************************************************************/
PUBLIC void MMIDEFAULT_SetAlwaysHalfOnBackLight(BOOLEAN b_half_on)
{
    //SCI_TRACE_LOW:"MMIDEFAULT_AllowHalfOnBackLight:s_backlight_ctrol=%d, b_half_on = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3379_112_2_18_2_0_2_160,(uint8*)"dd", s_backlight_ctrol, b_half_on);
    if (s_backlight_ctrol != MMI_BACKLIGHT_CTRL_ALWAYS_ON)
    {
        //ALWAYS_ON 和 HALF_ON不允许互相切换，必须从NORMAL状态切换
        if (b_half_on)
        {
            s_backlight_ctrol = MMI_BACKLIGHT_CTRL_ALWAYS_HALF_ON;
        }
        else
        {
            s_backlight_ctrol = MMI_BACKLIGHT_CTRL_NORMAL;
        }
        
        if (b_half_on)
        {
            s_backlight_status = MMI_BACKLIGHT_HALF_ON;
            LCD_SetBackLightBrightness(MMI_BACK_LIGHT_HALF_ON_DEBGREE); 
            MMI_TraceAutoBackLightStatus(s_backlight_status);
        }
        else
        {
            //MMIDEFAULT_SetLcdBackLightBySleep(TRUE);
            MMIDEFAULT_TurnOnBackLight();
        }
    }
}

/******************************************************************************/
/*! \brief  whether allow lcd backlight half on        
 *  \author Great.Tian
 *  \param[in] 
 */
/******************************************************************************/
LOCAL void SetNormalHalfOnBackLight(BOOLEAN b_half_on)
{
    //SCI_TRACE_LOW:"MMIDEFAULT_AllowHalfOnBackLight:s_backlight_ctrol=%d, b_half_on = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3412_112_2_18_2_0_2_161,(uint8*)"dd", s_backlight_ctrol, b_half_on);
    if (s_backlight_ctrol != MMI_BACKLIGHT_CTRL_ALWAYS_ON
        && s_backlight_ctrol != MMI_BACKLIGHT_CTRL_ALWAYS_HALF_ON
        )
    {
        //ALWAYS_ON 和 HALF_ON时，不允许设置为NORMAL_HALF_ON，必须从NORMAL状态切换
        if (b_half_on)
        {
            s_backlight_ctrol = MMI_BACKLIGHT_CTRL_NORMAL_HALF_ON;
        }
        else
        {
            s_backlight_ctrol = MMI_BACKLIGHT_CTRL_NORMAL;
        }
    }
}


/*****************************************************************************/
//  Description : whether allow turn off backlight  (是否允许关闭背光)         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsAllowBackLightTurnOff(void)
{
    BOOLEAN result = FALSE;

    switch(s_backlight_ctrol)
    {
    case MMI_BACKLIGHT_CTRL_NORMAL:         //按键能点亮背光。根据setting的设置能够关闭背光
    case MMI_BACKLIGHT_CTRL_NORMAL_HALF_ON:
        result = TRUE;
        break;
        
    case MMI_BACKLIGHT_CTRL_ALWAYS_ON:      //长开
    case MMI_BACKLIGHT_CTRL_ALWAYS_HALF_ON: //始终半暗
        result = FALSE;
        break;

    case MMI_BACKLIGHT_CTRL_ALWAYS_OFF:     //长关
        result = TRUE;
        break;

    default:
        result = FALSE;
        SCI_PASSERT(0,(("s_backlight_ctrol = %d"),s_backlight_ctrol)); /*assert verified*/
        break;        
    }

    return result;
}

/*****************************************************************************/
//  Description : start the lcd backlight timer           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StartLcdBackLightTimer(
                                              void
                                              )
{
    MMISET_BACKLIGHT_SET_T backlight_info = {0};
    POWER_RESTART_CONDITION_E   restart_condition = POWER_GetRestartCondition();//@zhaohui,cr102060

    SCI_TRACE_LOW("[mmi_default]: MMIDEFAULT_StartLcdBackLightTimer");//@zhaohui,cr107584

    if(MMIDEFAULT_GetFlipStatus())
    {
        //@zhaohui,cr102060,start
        if ((RESTART_BY_ALARM == restart_condition )
            ||(RESTART_BY_CHARGE == restart_condition ))
        {
            switch(s_backlight_status)
            {
                case MMI_BACKLIGHT_ON:
                    MMK_StartWinTimer(
                        VIRTUAL_WIN_ID,
                        MMI_LCD_LIGHT_TIMER_ID,
                        MMI_BACKLIGHT_TIMER_VALUE,
                        FALSE);
                    break;

                case MMI_BACKLIGHT_OFF:
                    //do nothing
                    break;

                case MMI_BACKLIGHT_HALF_ON:
                    MMK_StartWinTimer(
                        VIRTUAL_WIN_ID,
                        MMI_LCD_LIGHT_TIMER_ID,
                        MMI_BACKLIGHT_HALF_ON_DELAY_TIME,
                        FALSE);
                    break;
                default:
                    break;
            }  
        }
        else
        //@zhaohui,cr102060,end
        {    
            //SCI_TRACE_LOW:"MMIDEFAULT_StartLcdBackLightTimer backlight_type is %d,s_backlight_ctrol is %d,s_backlight_status is %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3510_112_2_18_2_0_2_162,(uint8*)"ddd",backlight_info.backlight_type,s_backlight_ctrol,s_backlight_status);

            MMIAPISET_GetBackLightInfo(&backlight_info);

            if (MMISET_BACKLIGHT_OPEN == backlight_info.backlight_type)
            {
                if (MMIDEFAULT_IsAllowBackLightTurnOff())
                {
                    switch(s_backlight_status)
                    {
                    case MMI_BACKLIGHT_ON:
#if defined(MMI_PDA_SUPPORT)//@tao.xue
                        if (MMIAPICC_IsNeedTurnOffBLQuicklyInCall())
                        {
                            MMK_StartWinTimer(
                                VIRTUAL_WIN_ID,
                                MMI_LCD_LIGHT_TIMER_ID,
                                MMI_BACKLIGHT_ON_DELAY_TIME_IN_CALL,
                                FALSE);
                        }
                        else
#endif
                        {
                            MMK_StartWinTimer(
                                VIRTUAL_WIN_ID,
                                MMI_LCD_LIGHT_TIMER_ID,
                                backlight_info.backlight_time,
                                FALSE);
                        }
                        break;

                    case MMI_BACKLIGHT_OFF:
                        //do nothing
                        break;

                    case MMI_BACKLIGHT_HALF_ON:
#if defined(MMI_PDA_SUPPORT)//@tao.xue
                        if (MMIAPICC_IsNeedTurnOffBLQuicklyInCall())
                        {
                            MMK_StartWinTimer(
                                VIRTUAL_WIN_ID,
                                MMI_LCD_LIGHT_TIMER_ID,
                                MMI_BACKLIGHT_HALF_ON_DELAY_TIME_IN_CALL,
                                FALSE);
                        }
                        else
#endif
                        {
                            MMK_StartWinTimer(
                                VIRTUAL_WIN_ID,
                                MMI_LCD_LIGHT_TIMER_ID,
                                MMI_BACKLIGHT_HALF_ON_DELAY_TIME,
                                FALSE);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
    else
    {
        MMK_StartWinTimer(
            VIRTUAL_WIN_ID,
            MMI_LCD_LIGHT_TIMER_ID,
            MMI_BACKLIGHT_FLIP_DELAY_TIME,
            FALSE);
    }
}

/*****************************************************************************/
//  Description : start the backlight timer           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StopLcdBackLightTimer(
                                             void
                                             )
{
    MMK_StopTimer(MMI_LCD_LIGHT_TIMER_ID);
}

/*****************************************************************************/
//  Description : set the backlight of the lcd, 直接设置当前的背光，不判断任何状态         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetLcdBackLight(
                                       BOOLEAN is_light
                                       )
{
    //do not use this function
}

/*****************************************************************************/
//  Description : set the backlight of the keybad，直接设置当前的背光，不判断任何状态      
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetKeybadBackLight(
                                          BOOLEAN is_light
                                          )
{
    // KeyPad一旦点亮, 就要启动Timer准备关掉
    //SCI_TRACE_LOW:"[default]: MMIDEFAULT_SetKeybadBackLight(%d) "
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3613_112_2_18_2_0_2_163,(uint8*)"d", is_light);
    if (is_light)
    {
        if (MMIDEFAULT_GetFlipStatus())
        {
            GPIO_SetKeyPadBackLight(TRUE);
            MMIDEFAULT_StartKeybadBackLightTimer();
        }
    }
    else
    {
        GPIO_SetKeyPadBackLight(FALSE);
    }
}

/*****************************************************************************/
//  Description : start the Keybad backlight timer           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StartKeybadBackLightTimer(
                                             void
                                             )
{
    MMISET_KEYBOARD_LIGHT_SET_T     keyboard_light_info = {0};

    MMIAPISET_GetKeyBoardLightTime(&keyboard_light_info);
    if (0 < keyboard_light_info.keyboard_light_time)
    {
        MMK_StartWinTimer(
            VIRTUAL_WIN_ID,
            MMI_KEYBAD_LIGHT_TIMER_ID,
            keyboard_light_info.keyboard_light_time,
            FALSE);
    }
}

/*****************************************************************************/
//  Description : stop the Keybad backlight timer           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StopKeybadBackLightTimer(
                                                void
                                                )
{
    MMK_StopTimer(MMI_KEYBAD_LIGHT_TIMER_ID);
}

/*****************************************************************************/
//  Description : set the status of the lcd sleep          
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
LOCAL void MMIDEFAULT_SetLcdSleep(
                                  BOOLEAN is_sleep //[IN] TRUE: 进入Sleep, FALSE: 退出Sleep
                                  )
{
    if( MMK_IsFocusWin(PHONE_STARTUP_CHARGE_WIN_ID) )
    {
        
        //SCI_TRACE_LOW:"MMIDEFAULT_SetLcdSleep: Focus win!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3671_112_2_18_2_0_2_164,(uint8*)"");
        if(is_sleep)
        {
            MMIAPIPHONE_StopChargeTimer();
        }
        else
        {
            MMK_PostMsg(PHONE_STARTUP_CHARGE_WIN_ID,MSG_GET_FOCUS,PNULL,0);
            MMIAPIPHONE_StartChargeTimer();
        }

    }       
#ifndef WIN32
	#ifdef TOUCHPANEL_TYPE
    if (is_sleep)
    {
        TP_Disable();
    }
    else
    {
        TP_Enable();
    }
	#endif	
#endif
#ifdef MONOCHROME_LCD_SUPPORT //@CR224767 2011.02.23 MONOCHROME LCD disable sleep
    if (is_sleep)
    {
        return;
    }
#endif 
    LCD_EnterSleep(LCD_ID_0, is_sleep);
//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT 
#ifndef SUBLCD_SIZE_NONE
    //在MEX平台运行应用的时候关闭子屏可以根据客户的要求开关本修改
     if(MMIMEX_IsSysRunning())
   {
         //关闭子屏
         LCD_EnterSleep(GUI_SUB_LCD_ID, SCI_TRUE);
         return;
   }           
#endif //SUBLCD_SIZE_NONE
#endif // MET_MEX_SUPPORT
//============== MET MEX  end ===================
#ifndef SUBLCD_SIZE_NONE
    LCD_EnterSleep(GUI_SUB_LCD_ID, is_sleep);
#endif
}

/*****************************************************************************/
//  Description : stop lcd sleep timer         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StopLcdSleepTimer(
                                         void
                                         )
{
    //bruce,backlight temp, I think this is now use now
}

/*****************************************************************************/
//  Description : stop lcd sleep timer         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsLcdSleepTimer(
                                          uint8 timer_id //[IN] 当前的Timer id
                                          )
{
    return (FALSE);
}

/*****************************************************************************/
//  Description : get the current flip status         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_GetFlipStatus(
                                        void
                                        )
{
    //JIUYANG2 only has one LCD, so it is alway TRUE
    //return TRUE;   by lisa
#ifdef WIN32
    return TRUE;
#elif defined SUBLCD_SIZE_NONE
    return TRUE;

#else
    return (!MMI_IsFlipClosed());/*lint !e718*/
#endif
}

/*****************************************************************************/
//  Description : set the deep sleep status         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetDeepSleep(
                                    BOOLEAN is_deepsleep
                                    )
{
#ifndef WIN32	
    LOCAL BOOLEAN   s_is_enter_sleep = FALSE;
#endif

//#ifdef MMI_RES_ORIENT_BOTH
    //deepsleep前需要关闭gsensor
    if ( is_deepsleep )
    {
        MMK_CloseGsensor( MAIN_IDLE_WIN_ID );
    }
    else
    {
        //MMK_OpenGsensor( MAIN_IDLE_WIN_ID );
        MMISET_UpdateCommonGSensor();
    }
//#endif

#ifdef WIFI_SUPPORT
    MMIAPIWIFI_HandleDeepSleep(is_deepsleep);
#endif

#ifndef WIN32
    s_is_enter_sleep = is_deepsleep;
#endif

#ifdef WRE_SUPPORT
	{
		MMIWRE_SetDeepSleepStatus(is_deepsleep);
	}
#endif

}

#ifdef LCD_SLIDE_SUPPORT

/*****************************************************************************/
//  Description : handle the slide key         
//  Global resource dependence : 
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_HandleSlideKey(
                                      MMI_MESSAGE_ID_E  msg_id //[IN] TRUE: flip up; FALSE: flip down
                                      )
{
    //SCI_TRACE_LOW:"MMIDEFAULT_HandleSlideKey: msg_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3813_112_2_18_2_0_3_165,(uint8*)"d", msg_id);

    switch (msg_id)
    {
    case MSG_KEYUP_SLIDE:   //打开滑盖
        MMIAPISET_PlayRing(0, FALSE,0, 1, MMISET_RING_TYPE_SLIDE_ON, PNULL);
        MMIKL_UnlockPhone();
        break;

    case MSG_KEYDOWN_SLIDE:     //关闭滑盖
        MMIAPISET_PlayRing(0, FALSE,0, 1, MMISET_RING_TYPE_SLIDE_OFF, PNULL);
        //@fen.xie modify for 需求文档
        if (!MMIAPICC_IsInState(CC_IN_CALL_STATE))//normal
        {
            if (MMISET_STOP_OPERATION_TYPE == MMIAPISET_GetSlideCloseType())    /*停止操作*/
            {
                MMK_ReturnIdleWin();    /*回退到IdleWin*/
            }
            
            if (MMISET_KEYLOCK_SWITCH_ON_TYPE == MMIAPISET_GetKeylockSwitchType())     /*键盘锁开启*/
            {
                MMIAPIKL_LockPhone();
            }
            else/*合盖锁键盘未开启时，需触发合盖后自动锁键盘的操作*/
            {
                MMIDEFAULT_StartAutoKeylockTimer();
            }
        }
        else//in call state
        {
            if (MMISET_STOP_OPERATION_TYPE == MMIAPISET_GetSlideCloseType())    /*停止操作*/
            {
                MMIAPICC_ReleaseAllCallExt(CC_RELEASE_ALL_PROMPT);/*不能马上退回IdleWin，先挂断所有电话*/
                MMIKL_SetIsLaterHandleSlideKey(TRUE);/*表示需要在call结束之后再处理slide键:是否ReturnIdleWin,是否LockPhone*/
            }
            /*说明:call state下若SlideCloseType为"继续操作"则do nothing；因为需求定义call时,不可锁键盘*/
        }        
        break;

    default:
        //SCI_TRACE_LOW:"MMIDEFAULT_HandleSlideKey invalid msg = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3853_112_2_18_2_0_3_166,(uint8*)"d", msg_id);
        break;
    }
}
#endif

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone. 
/*****************************************************************************/
//  Description : handle the flip key         
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_HandleFlipKey(
                                     BOOLEAN is_flipup //[IN] TRUE: flip up; FALSE: flip down
                                     )
{
    //SCI_TRACE_LOW:"MMIDEFAULT_HandleFlipKey: is_flipup = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3868_112_2_18_2_0_3_167,(uint8*)"d", is_flipup);

    if (is_flipup)
    {
        MMIAPISET_PlayRing(0, FALSE,0, 1, MMISET_RING_TYPE_FLIP, PNULL);
    }
    else
    {
        MMIAPISET_PlayRing(0, FALSE,0, 1, MMISET_RING_TYPE_FLIP_DOWN, PNULL);
    }
}
#endif

/*****************************************************************************/
// 	Description : is kbd tp press
//	Global resource dependence : 
//  Author: James.Zhang
//	Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsKbdTpPress( void )
{
    int32               cur_key_num = 0;
    MMI_MESSAGE_ID_E	multi_key_msg[MMI_MAX_MULTI_KEYS] = {0};
    MMI_TP_STATUS_E     tp_status = MMI_TP_NONE;
    GUI_POINT_T         point = {0};
    BOOLEAN             result = FALSE;
	
	MMK_GetMultiTPMsg(&tp_status, &point);

	cur_key_num = MMK_GetMultiKeyMsg(multi_key_msg, KEY_NONE);

    //如果有tp或kbd按下
    if ( MMI_TP_NONE != tp_status
        || cur_key_num > 0 )
    {
        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
//  Description : to stop backlight or handle other operation when the backlight
//                timer received   
//  Global resource dependence : 
//  Author: louis.wei
//  Note: 
/*****************************************************************************/
BOOLEAN MMIDEFAULT_HandleBackLightOver( void )
{
    POWER_RESTART_CONDITION_E   start_condition = POWER_GetRestartCondition();//@zhaohui,cr102060
    MMISET_BACKLIGHT_SET_T      backlight_info = {0};

    // to stop the timer of backlight
    //SCI_TRACE_LOW:"mmi_default: MMIDEFAULT_HandleBackLightOver()"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_3918_112_2_18_2_0_3_168,(uint8*)"");
    MMIDEFAULT_StopLcdBackLightTimer();
    if ( MMIDEFAULT_IsAllowBackLightTurnOff()
        //如果有tp或kbd按下, 不能关闭背光
        && !IsKbdTpPress()
#if defined(MMI_PDA_SUPPORT)//@tao.xue
            //when there is an incoming call , turn off backlight is forbidden
         && !MMIAPICC_IsInState(CC_MT_CONNECTING_STATE)
#endif
        )
    {
        MMIAPISET_GetBackLightInfo(&backlight_info);

        if (!MMIDEFAULT_GetFlipStatus() 
        || (MMISET_BACKLIGHT_OPEN_FOREVER != backlight_info.backlight_type)
        || (RESTART_BY_ALARM == start_condition )//@zhaohui,cr102060
        ||(RESTART_BY_CHARGE == start_condition ))
        {
            switch(s_backlight_status)
            {
            case MMI_BACKLIGHT_ON:
#ifdef MMI_KEY_LOCK_BACKLIGHT_IN_1_SUPPORT
#ifdef LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT
                MMISEPC_CloseAllLight(); 
#else
                CloseAllLight(FALSE);
#endif 
#else
                s_backlight_status = MMI_BACKLIGHT_HALF_ON;
                LCD_SetBackLightBrightness(MMI_BACK_LIGHT_HALF_ON_DEBGREE);
                MMI_TraceAutoBackLightStatus(s_backlight_status);
                
                //          MMIDEFAULT_StopLcdSleepTimer(); 
                MMIDEFAULT_StartLcdBackLightTimer();
#endif                
                break;
                
            case MMI_BACKLIGHT_OFF:
                //do nothing
                break;
                
            case MMI_BACKLIGHT_HALF_ON:
#ifdef LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT
                MMISEPC_CloseAllLight(); 
#else
                CloseAllLight(FALSE);
#endif 
                break;
                
            default:
                break;
            }
        }
    }
    
    return TRUE;
}

/******************************************************************************/
/*! \brief  Get the Table size.
 *  \author Jassmine.Meng 
 *  \return the size of table
 */
/******************************************************************************/
uint8 MMIDEFAULT_GetKeyTableSize(void)
{
    return (ARR_SIZE(s_keycode_table));
}

/*****************************************************************************/
//      Description : 判断lcd是否点亮        
//      Global resource dependence : 
//  Author: Phoebe.Chen
//      Note: 
/*****************************************************************************/
BOOLEAN MMIDEFAULT_IsBacklightOn(void)
{
#ifdef MONOCHROME_LCD_SUPPORT 
    return TRUE;
#else
    switch(s_backlight_status)
    {
    case MMI_BACKLIGHT_ON:
    case MMI_BACKLIGHT_HALF_ON:
        return TRUE;
        //break;

    case MMI_BACKLIGHT_OFF:
        return FALSE;
        //break;

    default:
        return FALSE;
        //break;
    }
#endif
}

/*****************************************************************************/
//  Description : convert (system)signal_keycode to (mmk)keycode
//                
//  Global resource dependence : 
//  Author: jibin
//  Note:
/*****************************************************************************/    
uint8 MMIDEFAULT_ConvertKeyCode( 
                                uint32 signal_keycode 
                                ) 
{ 
    uint8           keytable_size   = MMIDEFAULT_GetKeyTableSize(); 
    uint8           key_code        = KEY_NONE; 
    uint8           i               = 0; 

    //音量调节键直接适配为侧边键, 不需要修改mmi代码, 否则需要增加msg case
    if ( SCI_VK_VOL_UP == signal_keycode )
    {
        signal_keycode = SCI_VK_SIDE_UP;
    }
    else if ( SCI_VK_VOL_DOWN == signal_keycode )
    {
        signal_keycode = SCI_VK_SIDE_DOWN;
    }
    //alt直接适配为fn键, 不需要修改mmi代码, 否则需要增加msg case
    else if ( SCI_VK_ALT == signal_keycode )
    {
        signal_keycode = SCI_VK_FN;
    }

    for( i = 0; i< keytable_size; i++) 
    { 
        if((uint8)signal_keycode == s_keycode_table[i] ) 
        { 
            key_code = i; 
            break; 
        } 
    } 

#ifdef KEYPAD_TYPE_TYPICAL_KEYPAD
    key_code = MMIDEFAULT_ConvertQwertyKeyToTypicalKey( key_code );
#endif

    return key_code; 
} 

/*****************************************************************************/
//  Description : turn off back light directly
//  Global resource dependence : 
//  Author: bruce.chi
//  Note: 关闭背光, 不管现在的背光状态. 此函数仅在关机前调用.
/*****************************************************************************/    
PUBLIC void MMIDEFAULT_TurnOffBackLight(void)
{
    MMI_WIN_ID_T    focus_win_id = 0;
    
    s_backlight_status = MMI_BACKLIGHT_OFF;
    
    //关闭背光
    GPIO_SetLcdBackLight(FALSE);
    MMI_TraceAutoBackLightStatus(s_backlight_status);
    
    // 如果要将背光关掉则必须停掉打开背光的Timer，否则容易导致蓝瓶
    //          MMIDEFAULT_StopLcdSleepTimer();     
    
    // 关闭LCD
    MMIDEFAULT_SetLcdSleep(TRUE);
   
    // @Great.Tian, handle
    MMIAPIIDLE_HandleLCDBacklight(FALSE);
    
    
    focus_win_id = MMK_GetFocusWinId();
    if(0 != focus_win_id 
        && MMIAPICOM_GetInvalidWinId() != focus_win_id)
    {
        //SCI_TRACE_LOW:"MMIDEFAULT_TurnOffBackLight, focus_win_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4084_112_2_18_2_0_3_169,(uint8*)"d", focus_win_id);
        //send system backlight turn off message to  control and window
        MMK_SendMsg(focus_win_id, MSG_BACKLIGHT_TURN_OFF, PNULL);
    }
}

/*****************************************************************************/
//  Description : start lcd back light timer according to setting
//  Global resource dependence : 
//  Author: bruce.chi
//  Note:
/*****************************************************************************/    
LOCAL void StartSettingLCDBackLightTimer(void)
{
    //@zhaohui,cr102060,start 
    POWER_RESTART_CONDITION_E   restart_condition = POWER_GetRestartCondition();

    if ((RESTART_BY_ALARM == restart_condition)
        ||(RESTART_BY_CHARGE == restart_condition))
    {
        MMK_StartWinTimer(
            VIRTUAL_WIN_ID,
            MMI_LCD_LIGHT_TIMER_ID,
            MMI_BACKLIGHT_TIMER_VALUE,
            FALSE);
    }
    else
    //@zhaohui,cr102060,end
    {
        MMISET_BACKLIGHT_SET_T backlight_info = {0};
    
        MMIAPISET_GetBackLightInfo(&backlight_info);
        if (MMISET_BACKLIGHT_OPEN == backlight_info.backlight_type)
        {
            MMK_StartWinTimer(
                VIRTUAL_WIN_ID,
                MMI_LCD_LIGHT_TIMER_ID,
                backlight_info.backlight_time,
                FALSE);
        }
    }
}
/*****************************************************************************/
//  Description : start lcd back light timer according to setting
//  Global resource dependence : 
//  Author: xiangjie
//  Note:
/*****************************************************************************/    
PUBLIC void MMIDEFAULT_StartSettingLCDBackLightTimer(void)
{
    StartSettingLCDBackLightTimer();
}
/*****************************************************************************/
//  Description : handling the key message by default process which may process
//                in all window (include to start backlight and ring other key voice 
//                and handle other situation
//                
//  Global resource dependence : 
//  Author:Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_DefaultMSGKtp(
                                 MMI_MESSAGE_ID_E   msg_id
                                 )
{
    BOOLEAN is_handle_tp_press = TRUE;

    switch(msg_id)
    {
    case MSG_TP_PRESS_DOWN:
        if (!MMIDEFAULT_IsBacklightOn())
        {
            //SCI_TRACE_LOW:"MMK: backlight is off, abandon tp down msg!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4150_112_2_18_2_0_3_170,(uint8*)"");
            is_handle_tp_press = FALSE;
        }
        else
        {
#if defined(MMI_DIALPANEL_DTMF_SUPPORT)
#ifdef TOUCH_PANEL_SUPPORT
            MMI_TP_STATUS_E tp_status = MMI_TP_NONE;
            GUI_POINT_T tp_point = {0};
            BOOLEAN is_press_in_dial = FALSE;

            MMK_GetLogicTPMsg(&tp_status, &tp_point);
            is_press_in_dial = MMIAPIIDLE_IsPressInDialPanel(tp_point);
#endif
#endif

            // 设置LCD的背景灯
            // MMIDEFAULT_SetBackLight(TRUE);
            MMIDEFAULT_TurnOnLCDBackLight();
            
            //MMIDEFAULT_StopLcdBackLightTimer();
            //  MMIDEFAULT_StopKeybadBackLightTimer();
            if(MMIDEFAULT_IsEnableTpRing()
#if defined(MMI_DIALPANEL_DTMF_SUPPORT)
#ifdef TOUCH_PANEL_SUPPORT
            && !is_press_in_dial
#endif
#endif
            )
            {
                MMIAPISET_PlayRing(0, FALSE,0, 1, MMISET_RING_TYPE_TKEY, PNULL);
            }
        }
        MMIDEFAULT_StopAutoKeylockTimer();
        
        break;

    case MSG_TP_PRESS_UP:
#ifdef TOUCHPANEL_TYPE_MULTITP        
    case MSG_TP_GESTURE:
#endif
        if (!MMIDEFAULT_IsBacklightOn())
        {
            //SCI_TRACE_LOW:"MMK: backlight is off, abandon tp up msg; but turn on th backlight!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4176_112_2_18_2_0_3_171,(uint8*)"");
#if !defined(MMI_PDA_SUPPORT)//@tao.xue
            if(MMIAPICC_IsInState(CC_IN_CALL_STATE)) //cr127538
            {                
                MMIDEFAULT_TurnOnLCDBackLight();
            }
#endif
            is_handle_tp_press = FALSE;
        }
        else
        {
            // 设置LCD的背景灯
            // MMIDEFAULT_SetBackLight(TRUE);
            //MMIDEFAULT_TurnOnLCDBackLight();
            
            MMIDEFAULT_StartLcdBackLightTimer();
            // MMIDEFAULT_StartKeybadBackLightTimer();
        }
        MMIDEFAULT_StartAutoKeylockTimer();
        break;

    case MSG_TP_PRESS_MOVE:
        if (!MMIDEFAULT_IsBacklightOn())
        {
            //SCI_TRACE_LOW:"MMK: backlight is off, abandon tp move msg!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4200_112_2_18_2_0_3_172,(uint8*)"");
            is_handle_tp_press = FALSE;
        }
        break;

    default:
        //SCI_TRACE_LOW:"MMK_DefaultMSGKtp msg_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4206_112_2_18_2_0_3_173,(uint8*)"d",msg_id);
        //SCI_PASSERT(0,(("msg_id = %d"),msg_id));
        break;
    }

    return is_handle_tp_press;
}

/*****************************************************************************/
//  Description : get the back light state of the mmi moudal           
//  Global resource dependence : 
//  Author: Great.Tian
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_GetBackLightState(
                                            void
                                            )
{
    BOOLEAN     result = FALSE;

    result = MMIDEFAULT_IsBacklightOn();

    return (result);
}

#ifdef WIN32
/*****************************************************************************/
//  Description : dummy function of prod driver         
//  Global resource dependence : 
//  Author: bruce.chi
//  Note: 
/*****************************************************************************/
/*
PUBLIC void GPIO_SetLcdBackLight(BOOLEAN b_on)
{
    b_on = b_on;
}
*/

PUBLIC void LCD_SetBackLightBrightness(uint32  brightless)
{
    brightless = brightless;
}

extern void GPIO_SetVibrator (BOOLEAN b_on);
/*
{
    b_on = b_on;
}
*/

void GPIO_SetKeyPadBackLight(BOOLEAN b_on)
{
    b_on = b_on;
}

LOCAL void TestKey1Func(void)
{
    APP_MN_SIM_NOT_READY_IND_T *signal_ptr = PNULL;

    signal_ptr = SCI_ALLOC_APP(sizeof(APP_MN_SIM_NOT_READY_IND_T));

    signal_ptr->SignalCode = (uint16)APP_MN_SIM_NOT_READY_IND;
    signal_ptr->SignalSize = sizeof(APP_MN_SIM_NOT_READY_IND_T);
    signal_ptr->Sender     = P_APP + 1;
#if defined(MMI_MULTI_SIM_SYS_DUAL) && !defined(WIN32)    
    signal_ptr->dual_sys   = MN_DUAL_SYS_2;
#endif
    signal_ptr->Pre        = SCI_NULL;
    signal_ptr->Suc        = SCI_NULL;


    // Send the signal to the dedicated task's queue
    MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, P_APP);
}

LOCAL void TestKey2Func(void)
{
    APP_MN_CALL_READY_IND_T *signal_ptr = PNULL;
    APP_MN_SIM_READY_IND_T  *signal_ptr2 = PNULL;
    uint8                   imsi[] = {"543267"};
    APP_MN_SMS_READY_IND_T  *signal_ptr3 = PNULL;

    //sim ready
    signal_ptr2 = SCI_ALLOC_APP(sizeof(APP_MN_SIM_READY_IND_T));

    signal_ptr2->SignalCode = (uint16)APP_MN_SIM_READY_IND;
    signal_ptr2->SignalSize = sizeof(APP_MN_SIM_READY_IND_T);
    signal_ptr2->Sender     = P_APP + 1;
#if defined(MMI_MULTI_SIM_SYS_DUAL) && !defined(WIN32)    
    signal_ptr2->dual_sys   = MN_DUAL_SYS_2;
#endif
    signal_ptr2->Pre        = SCI_NULL;
    signal_ptr2->Suc        = SCI_NULL;
    signal_ptr2->imsi.imsi_len = strlen((const char*)imsi);
    SCI_MEMCPY(signal_ptr2->imsi.imsi_val, imsi, strlen((const char*)imsi));

    MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr2, P_APP);

    //call ready
    signal_ptr = SCI_ALLOC_APP(sizeof(APP_MN_CALL_READY_IND_T));

    signal_ptr->SignalCode = (uint16)APP_MN_CALL_READY_IND;
    signal_ptr->SignalSize = sizeof(APP_MN_CALL_READY_IND_T);
    signal_ptr->Sender     = P_APP + 1;
#if defined(MMI_MULTI_SIM_SYS_DUAL) && !defined(WIN32)        
    signal_ptr->dual_sys   = MN_DUAL_SYS_2;
#endif
    signal_ptr->Pre        = SCI_NULL;
    signal_ptr->Suc        = SCI_NULL;


    // Send the signal to the dedicated task's queue
    MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, P_APP);
    
    //sms ready
    signal_ptr3 = SCI_ALLOC_APP(sizeof(APP_MN_SMS_READY_IND_T));

    signal_ptr3->SignalCode = (uint16)APP_MN_SMS_READY_IND;
    signal_ptr3->SignalSize = sizeof(APP_MN_SMS_READY_IND_T);
    signal_ptr3->Sender     = P_APP + 1;
#if defined(MMI_MULTI_SIM_SYS_DUAL) && !defined(WIN32)    
    signal_ptr3->dual_sys   = MN_DUAL_SYS_2;
#endif
    signal_ptr3->Pre        = SCI_NULL;
    signal_ptr3->Suc        = SCI_NULL;


    // Send the signal to the dedicated task's queue
    MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr3, P_APP);
}

LOCAL void TestKey3Func(void)
{
    uint16          signal_size;  
    uint16          signal_code;
    MmiSignalS      *signal_ptr = PNULL;
    MmiKeyPress*    keypress_ptr = PNULL;
    
    signal_code = KPD_DOWN;
    signal_size = sizeof(xSignalHeaderRec);

    MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,SCI_IdentifyThread());
    // Send the signal to the dedicated task's queue
    keypress_ptr = (MmiKeyPress*)&signal_ptr->sig;
    keypress_ptr->keyCode = SCI_VK_POWER;
    signal_ptr->send = KPDSVR;
    MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, P_APP);
}

LOCAL void TestKey4Func(void)
{
 uint16                signal_size;  
    uint16                signal_code;
    xSignalHeaderRec      *signal_ptr = PNULL;

    signal_code = USB_PLUGIN_IND;
    signal_size = sizeof(xSignalHeaderRec);

    MMI_CREATE_SIGNAL(signal_ptr,signal_code,signal_size,SCI_IdentifyThread());
    // Send the signal to the dedicated task's queue
    signal_ptr->Sender = P_APP + 1;
    MMI_SEND_SIGNAL((xSignalHeaderRec*)signal_ptr, P_APP);
}

#endif

/*****************************************************************************/
//  Description : handle other mmi task's signal 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMK_HandleMmiSig(
                             uint16     sig_code,
                             DPARAM     sig_data_ptr
                             )
{
    //SCI_TRACE_LOW:"MMK_HandleMmiSig: receive 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4377_112_2_18_2_0_4_174,(uint8*)"d",sig_code);
    
    switch(sig_code)
    {
    case APP_COPY_END_IND:
        //SCI_TRACE_LOW:"MMK_HandleMmiSig: receive APP_COPY_END_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4382_112_2_18_2_0_4_175,(uint8*)"");
        MMIAPIFMM_CopyCallBack(((MmiAppCopyMsgS*)sig_data_ptr)->data);
        break;

    case APP_MOVE_END_IND:
    case APP_MOVE_ONE_FILE_IND:
        MMIAPIFMM_MoveCallBack(((MmiAppCopyMsgS*)sig_data_ptr)->data);
        break;

    case APP_MOVE_FILE_EXIST:
        MMIAPIFMM_MoveFileExist();
        //MMIAPIFMM_MoveCallBack(((MmiAppCopyMsgS*)sig_data_ptr)->data);
        break;
        
	case APP_COPY_EXIST_IND:
         MMIAPIFMM_CopyFileExistInform();
		break;
    case APP_COPY_DIR_END_IND:
    case APP_COPY_ONE_FILE_END_IND:
    case APP_CUT_DIR_END_IND:
        MMIAPIFMM_Callback(((MmiAppCopyMsgS*)sig_data_ptr)->data);
        break;
	case APP_COPY_ONE_DIR_END_IND:
		//SCI_TRACE_LOW("MMK_HandleMmiSig: receive APP_COPY_ONE_DIR_END_IND");
		MMIAPIFMM_CopyNext();
		break;
    case APP_FIND_FILES_END_IND:
        MMIAPIFMM_SearchFileCallBack();
        break;
        
    case APP_DEL_DIR_END_IND:
        MMIAPIFMM_DelCallBack(((MmiAppCopyMsgS*)sig_data_ptr)->data);
        break;
     case APP_DM_MESSAGE_IND:
        MMIAPIDM_HandleDmMessage(sig_data_ptr);
        break;

    case APP_OTHER_TASK_TO_APP:
        {
            MmiOtherTaskMsgS* data_ptr = (MmiOtherTaskMsgS*)sig_data_ptr;
            
            SCI_ASSERT( PNULL != data_ptr ); /*assert verified*/

            if(PNULL != s_msg_data_ptr)
            {
                SCI_FREE(s_msg_data_ptr);
                s_msg_data_ptr = PNULL;
            }

            if(0 < data_ptr->size_of_param)
            {
                s_msg_data_ptr = SCI_ALLOC_APP(data_ptr->size_of_param);
                SCI_ASSERT(PNULL != data_ptr->param_ptr); /*assert verified*/
                SCI_MEMCPY(s_msg_data_ptr, data_ptr->param_ptr, data_ptr->size_of_param);
            }
            
            MMK_PostMsg( data_ptr->handle, data_ptr->msg_id, data_ptr->param_ptr, data_ptr->size_of_param );
            
            //释放额外的内存
            if( PNULL != data_ptr->param_ptr )
            {
                SCI_FREE( data_ptr->param_ptr );
            }
        }
        break;
#ifdef _ATEST_
#ifdef WIN32
    case APP_ATEST_IND:  // ATEST MODULE - added by Mingxuan.Zhu
        {            
            //SCI_TRACE_LOW:"ATEST_TRACE====Enter ATEST MODULE!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4451_112_2_18_2_0_4_176,(uint8*)"");
            MMIAPI_ATESTMAINHandleAtestMsg(sig_data_ptr);        
        }
        break;
#endif//WIN32
#endif//_ATEST_
    default:
        //SCI_TRACE_LOW:"MMK_HandleMmiSig: receive unknown signal! sig_code = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4458_112_2_18_2_0_4_177,(uint8*)"d", sig_code);
        break;
    }
}

/*****************************************************************************/
// Description : Make a judgement that if a Win is POPUP Win.
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsPopupWin(
                                     MMI_HANDLE_T win_handle
                                     )
{
    GUIMENU_CTRL_T *menu_ptr = PNULL;
    MMI_HANDLE_T handle =MMK_GetCtrlHandleByGuid(win_handle, SPRD_GUI_MENU_ID);

    menu_ptr = (GUIMENU_CTRL_T *)MMK_GetCtrlPtr(handle);

    if(menu_ptr != PNULL)
    {
        return GUIMENU_IsPopMenu(menu_ptr);
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
// Description : 判断窗口是否全屏的list窗口
// Global resource dependence : 
// Author: xiaoqing.lu
// Note: 暂时的使用接口，因为这样判断并不严谨，如一个窗口包含
// 多个list时或者list是半屏窗口时，不能这么判断
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsFullListWin(
                                        MMI_HANDLE_T win_handle
                                        )
{
    MMI_HANDLE_T    handle1 =MMK_GetCtrlHandleByGuid(win_handle, SPRD_GUI_LIST_ID);
    MMI_HANDLE_T    handle2 =MMK_GetParentWinHandle(win_handle);

    handle2 = MMK_GetCtrlHandleByGuid(handle2, SPRD_GUI_LIST_ID);

    if(0 != handle1 || 0 != handle2)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
// Description : 判断是否主菜单
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsMainMenuWin(
                                        MMI_HANDLE_T win_handle
                                        )
{
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T *menu_ptr = PNULL;
    MMI_HANDLE_T handle =MMK_GetCtrlHandleByGuid(win_handle, SPRD_GUI_MENU_ID);

    menu_ptr = (GUIMENU_CTRL_T *)MMK_GetCtrlPtr(handle);

    if(menu_ptr != PNULL)
    {
        switch(menu_ptr->cur_style)
        {
        case GUIMENU_STYLE_ICON:
        case GUIMENU_STYLE_OPTION_PAGE:    
        case GUIMENU_ISTYLE:            
        case GUIMENU_STYLE_TITLE_ICON:
#ifdef CUBE_MAIN_MENU_SUPPORT
        case GUIMENU_STYLE_CUBE:
#endif

#ifdef MENU_CRYSTALCUBE_SUPPORT        
        case GUIMENU_STYLE_CRYSTAL_CUBE:
#endif
#ifdef MENU_CYCLONE_SUPPORT        
        case GUIMENU_STYLE_CYCLONE:
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
		case GUIMENU_STYLE_GO:
#else
#ifdef SLIDE_MAIN_MENU_SUPPORT             
        case GUIMENU_STYLE_SLIDE_PAGE: 
#endif
#ifdef TRAPEZOID_MAIN_MENU_SUPPORT
        case GUIMENU_STYLE_TRAPEZOID:
#endif        
#ifdef BINARY_STAR_MAIN_MENU_SUPPORT
        case GUIMENU_STYLE_BINARY_STAR:
#endif
#ifdef SHUTTER_MAIN_MENU_SUPPORT
        case GUIMENU_STYLE_SHUTTER: 
#endif        
#ifdef CHARIOT_MAIN_MENU_SUPPORT
        case GUIMENU_STYLE_CHARIOT: 
#endif 
        case GUIMENU_STYLE_SPHERE:
        case GUIMENU_STYLE_CYLINDER:
#endif
            result = TRUE;
            break;
            
        default:
            result = FALSE;
            break;
        }
    }
    else
    {
        result = FALSE;
    }

    return result;
}

/*****************************************************************************/
// Description : 判断是否二级菜单
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsSecMenuWin(
                                       MMI_HANDLE_T win_handle
                                       )
{
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T *menu_ptr = PNULL;
    MMI_HANDLE_T handle =MMK_GetCtrlHandleByGuid(win_handle, SPRD_GUI_MENU_ID);

    menu_ptr = (GUIMENU_CTRL_T *)MMK_GetCtrlPtr(handle);

    if(menu_ptr != PNULL)
    {
        switch(menu_ptr->cur_style)
        {
        case GUIMENU_STYLE_SECOND:
        case GUIMENU_STYLE_THIRD:            
            result = TRUE;
            break;

        default:
            result = FALSE;
            break;
        }
    }
    else
    {
        result = FALSE;
    }

    return result;
}

#ifdef UI_P3D_SUPPORT

#ifdef EFFECT_LCD_ROTATE_SUPPORT
/*****************************************************************************/
// Description : Do the prepare for LCD rotate.
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
PUBLIC void MMIDEFAULT_HandlePreLCDRotate(
                                  void
                                  )
{
    if(MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
    {
        MMIDEFAULT_SaveOldMoveBuf(SCR_EFFECT_BUF_TYPE_LCD_ROTATE);
    }
}
#endif
/*****************************************************************************/
// Description : handle PopMenu Effect
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
LOCAL BOOLEAN HandlePopMenuEffect(
                                  MMI_HANDLE_T win_handle
                                  )
{
    BOOLEAN             result          = FALSE;
    GUI_RECT_T          rect            = MMITHEME_GetFullScreenRect();
    uint16              lcd_width       = 0;
    uint16              lcd_height      = 0;
    uint16              *old_buf_ptr    = PNULL;
    uint16              *new_buf_ptr    = PNULL;                   
    MMI_HANDLE_T        handle          = MMK_GetCtrlHandleByGuid(win_handle, SPRD_GUI_MENU_ID);
    GUIMENU_CTRL_T      *menu_ptr       = (GUIMENU_CTRL_T *)MMK_GetCtrlPtr(handle);
    MMITHEME_POPMENU_T  theme_ptr       = {0};
    GUI_COLOR_T         *lcdbuf_ptr     = GUILCD_GetMainLcdBufPtr();
    MMI_HANDLE_T        focus_handle    = MMK_GetFocusWinHandle();

    if(MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
    {
        if(MMIDEFAULT_IsPopupWin(win_handle))
        {
            if(menu_ptr->is_first_disp && MMITHEME_Is3DOpen())
            {
                if(0 == menu_ptr->menu_level && MMK_GetWinMoveStyle(win_handle) != MOVE_FORBIDDEN)
                {
                    if(AllocTrans3DBuf(&old_buf_ptr, &new_buf_ptr))
                    {
                        GUI_RECT_T menu_rect = menu_ptr->rect;
                        GUILCD_GetLogicWidthHeight(LCD_ID_0,&lcd_width,&lcd_height);
                        MMITHEME_GetPopMenuTheme(&theme_ptr);
                        LCD_FillDestBuffArgbRect( old_buf_ptr, lcd_width, lcd_width, lcd_height, rect, theme_ptr.mask_color, theme_ptr.mask_alpha );
                        MMITHEME_PopupMenuEffect(&menu_rect, 0, TRUE);
                    }
                }
                menu_ptr->is_first_disp = FALSE;
            }
            result = TRUE;
        }
        else if ( MMIDEFAULT_IsPopupWin( focus_handle ) )
        {
            result = TRUE;
        }
        else if(s_is_last_closed_popup_window)
        {
            MMI_HANDLE_T idle_win_handle = MMK_ConvertIdToHandle( MAIN_IDLE_WIN_ID );
            if (win_handle != idle_win_handle
                && !MMK_IsChildWin(win_handle) /* && MMK_GetParentWinHandle(win_handle) != idle_win_handle) */
                && MMITHEME_Is3DOpen()
                && s_last_popup_menu_rect.top != s_last_popup_menu_rect.bottom
                && s_last_popup_menu_rect.left != s_last_popup_menu_rect.right
                && MMI_WIN_ANIM_NONE != s_cur_win_anim_type
                )
            {            
                UILAYER_InvalidateRect( LCD_ID_0, &rect );
                GUILCD_GetLogicWidthHeight(LCD_ID_0,&lcd_width,&lcd_height);
                MMITHEME_GetPopMenuTheme(&theme_ptr);
                LCD_FillDestBuffArgbRect( lcdbuf_ptr, lcd_width, lcd_width, lcd_height, rect, theme_ptr.mask_color, theme_ptr.mask_alpha);

                MMIDEFAULT_SaveOldMoveBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);    
                MMITHEME_PopupMenuEffect(&s_last_popup_menu_rect, 0,  FALSE);
            }

            s_is_last_closed_popup_window = FALSE;

            result = TRUE;
        }
    }

    return result;
}

/*****************************************************************************/
// Description : handle tab effect
// Global resource dependence : 
// Author: xiaoqing.lu
// Note:
/*****************************************************************************/
/*LOCAL void HandleTabEffect(
                           MMI_HANDLE_T win_handle
                           )
{
    uint16 *buf_ptr     = PNULL;
    uint16 *old_buf_ptr = PNULL;
    uint16 *new_buf_ptr = PNULL;
    
    if (MMK_IsChildWin(win_handle))
    {
        uint16      lcd_width       = 0;
        uint16      lcd_height      = 0;
        uint16      *temp_dst_ptr   = PNULL;
        uint16      *temp_src_ptr   = PNULL;
        uint16      *dst_ptr        = PNULL;
        GUI_RECT_T  win_rect        = {0};
        GUI_RECT_T  full_rect       = {0};
        GUI_RECT_T  top_rect        = {0};
        GUI_RECT_T  bottom_rect     = {0};
        
        s_tab_win_switch_item = TRUE;
        
        MMK_GetWinRect(win_handle, &win_rect); // 获取区域
        
        win_rect.bottom -= MMI_FULLSCREEN_SOFTKEY_BAR_HEIGHT;
        
        // 取得全屏区域
        full_rect = MMITHEME_GetFullScreenRect();
        
        // 计算子窗口的上下的区域
        bottom_rect = top_rect = full_rect;
        top_rect.bottom = win_rect.top - 1;
        bottom_rect.top = win_rect.bottom + 1;
        
        // 保存到old buffer
        MMI_GetTrans3DBuf(&old_buf_ptr, &new_buf_ptr);
        
        buf_ptr = GUILCD_GetMainLcdBufPtr();
        GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID, &lcd_width, &lcd_height);
        
        
        UILAYER_InvalidateRect( GUI_MAIN_LCD_ID,  &full_rect);
        
        if(PNULL != old_buf_ptr && PNULL != new_buf_ptr)
        {
            SCI_MEMCPY(old_buf_ptr, buf_ptr, SCR_SIZE); // 保存buffer
            SCI_MEMCPY(buf_ptr, new_buf_ptr, SCR_SIZE); // 保存buffer
            
            dst_ptr = buf_ptr + lcd_width*(top_rect.bottom + 1);
            temp_dst_ptr = new_buf_ptr + lcd_width*(top_rect.bottom + 1);
            temp_src_ptr = old_buf_ptr + lcd_width*(top_rect.bottom + 1);
            
            MMITHEME_PlayHorSlideAnimWithOld(
                GUI_MAIN_LCD_ID, 
                dst_ptr, 
                temp_src_ptr, 
                temp_dst_ptr, 
                lcd_width, 
                win_rect.bottom - win_rect.top + 1, 
                s_tab_data.is_to_left
                );
        }
        else
        {
            return;
        }
        
        MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
    }

    return;
}*/

/*****************************************************************************/
// Description : handle window pre-moving type by msg
// Global resource dependence : 
// Author: Yintang.Ren
// Note:
/*****************************************************************************/
PUBLIC void MMK_HandleWinPreMoveMsg(
                                    MMI_HANDLE_T     win_handle,
                                    MMI_MESSAGE_ID_E msg,
                                    DPARAM           param
                                    )

{
#ifndef MMI_PDA_SUPPORT
    switch(msg)
    {
    case MSG_NOTIFY_TAB_SWITCH:
        if (MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP)
            && s_tab_win_switch_item) // 使用s_tab_win_switch_item暂时屏蔽
        {
            uint16 lcd_width = 0;
            uint16 lcd_height = 0;
            GUITAB_NEED_ITEM_DATA_T *tab_data_ptr = PNULL;
            
            if (PNULL != param)
            {
                tab_data_ptr = (GUITAB_NEED_ITEM_DATA_T*)param;

                if (tab_data_ptr->is_child)
                {
                    GUI_RECT_T win_rect = {0};

                    //s_tab_data= *tab_data_ptr;

                    MMK_GetWinRect(win_handle, &win_rect); // 获取区域
                    
                    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID, &lcd_width, &lcd_height);
                    
                    win_rect.bottom -= MMI_FULLSCREEN_SOFTKEY_BAR_HEIGHT;
                    
                    MMIDEFAULT_SaveMoveToNewBuf(); // 保存到buffer

                    s_tab_win_switch_item = TRUE;
                }
            }
        }
        break;

    case MSG_CLOSE_WINDOW:
        if (MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
        {
            if (MMK_IsFocusWin(win_handle))
            {
                if(MMIDEFAULT_IsPopupWin(win_handle) && s_saved_move_buf_type != SCR_EFFECT_BUF_TYPE_LCD_ROTATE)
                {
                    MMIDEFAULT_SaveMoveToNewBuf();
                    if(MMK_GetWinMoveStyle(win_handle) != MOVE_FORBIDDEN)
                    {
                        MMI_HANDLE_T handle = MMK_GetCtrlHandleByGuid(win_handle, SPRD_GUI_MENU_ID);
                        
                        GUIMENU_CTRL_T *menu_ptr = (GUIMENU_CTRL_T *)MMK_GetCtrlPtr(handle);

                        s_last_popup_menu_rect = menu_ptr->rect;
                    }
                    else
                    {
                        s_last_popup_menu_rect.left = 0;
                        s_last_popup_menu_rect.right = 0;
                        s_last_popup_menu_rect.top = 0;
                        s_last_popup_menu_rect.bottom = 0;                        
                    }
                    s_is_last_closed_popup_window = TRUE;
                }
                else
                {
                    s_is_last_closed_popup_window = FALSE;
                }
                
                if (MMIDEFAULT_IsMainMenuWin(win_handle))
                {
                    s_last_close_win_type = MMI_WIN_TYPE_MAINMENU;
                }
                else
                {
                    s_last_close_win_type = MMI_WIN_TYPE_NONE;
                }
            }
        }       
        break;
    default:
        break;
    }
#endif
}

/*****************************************************************************/
// Description : handle window move type by msg
// Global resource dependence : 
// Author: ryan.xu
// Note:
/*****************************************************************************/
PUBLIC void MMK_HandleWinMoveMsg(
                                 MMI_HANDLE_T     win_handle,
                                 MMI_MESSAGE_ID_E msg,
                                 DPARAM           param
                                 )
{
    MMI_HANDLE_T   base_handle = 0;
    static MMI_TRANS_INFO_T trans_info = {0};
    static BOOLEAN s_close_fullpaint_match = FALSE;
    static BOOLEAN s_last_closed_window_is_3d_enable = FALSE;
    static BOOLEAN s_is_open_paint_order = FALSE;

    if (MSG_TIMER != msg)
    {
        //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg msg = 0x%x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4894_112_2_18_2_0_5_178,(uint8*)"d", msg);
    }

    win_handle = MMK_ConvertIdToHandle( win_handle );
    
    switch(msg)
    {
    case MSG_LOSE_FOCUS:  // 打开下一个窗口

        #if defined(MENU_CRYSTALCUBE_SUPPORT) || defined(MENU_CYCLONE_SUPPORT)
        /*某些极端情况下，收到此消息时窗口已经关闭，此时不应继续处理LOSE_FOCUS，此处修改应该不用宏区分，加宏只是为了标记*/
        if (!MMK_IsOpenWin(win_handle)) break;
        #endif
        
        s_cur_win_anim_type = MMITHEME_Get3DScreenAnimType(win_handle, MMI_WIN_DISP_STATE_LOSE_FOCUS);

        if (MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
        {
            //if (MMK_ConvertIdToHandle(MAIN_IDLE_WIN_ID) == win_handle)
            if (IsDisable3DWindow(win_handle))
            {
                //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_LOSE_FOCUS: idlewin disable!"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4915_112_2_18_2_0_5_179,(uint8*)"");
            }
            else if (MMK_IsChildWin(win_handle))
            {
                //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_LOSE_FOCUS: childwin disable!"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4919_112_2_18_2_0_5_180,(uint8*)"");
            }
            else if ( MMK_GetWinDisplayStyleState( MMK_GetFocusWinHandle(), WS_HAS_TRANSPARENT ) )
            {
                SCI_TRACE_LOW("MMK_HandleWinMoveMsg MSG_LOSE_FOCUS: pubwin disable!");
            }
            else
            {
                MMIDEFAULT_SaveOldMoveBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
                if (!IsDisable3DWindow(win_handle))  //某些窗口禁止场景翻转特效
                {
                    if (MMITHEME_Is3DOpen())
                    {
                        MMK_GetWinAngle(win_handle,&trans_info.pre_angle);
                        trans_info.win_handle = win_handle;
                        trans_info.iDirection = 0;
                        s_is_open_paint_order = FALSE;
                        s_close_fullpaint_match = FALSE;
                    }
                    else
                    {
                        //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_LOSE_FOCUS: roll close!"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4941_112_2_18_2_0_5_181,(uint8*)"");
                    }
                }
                else
                {
                    /* 对一般的窗口而言，不应该进行特效；设成一个非正常值*/
                    trans_info.pre_angle = WIN_SUPPORT_ANGLE_INIT;
                    //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_LOSE_FOCUS: IsDisable3DWindow!"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4948_112_2_18_2_0_5_182,(uint8*)"");
                }
            }
        }
        else
        {
            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_LOSE_FOCUS: not Permit3D!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_4954_112_2_18_2_0_5_183,(uint8*)"");
        }
        break;

    case MSG_CLOSE_WINDOW: // 关闭当前窗口
        s_cur_win_anim_type = MMITHEME_Get3DScreenAnimType(win_handle, MMI_WIN_DISP_STATE_CLOSE);

        if (MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
        {
            //if (MAIN_IDLE_WIN_ID == MMK_GetPrevWinId(win_handle))
            if (IsDisable3DWindow(MMK_GetPrevWinId(win_handle)))
            {
                MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
            }
            else if (MMK_IsFocusWin(win_handle))
            {
                if(s_is_last_closed_popup_window)
                {
                    s_last_closed_window_is_3d_enable = FALSE;
                    trans_info.iDirection = 1;
                }
                else if (!IsDisable3DWindow(win_handle))  //某些窗口禁止场景翻转特效
                {
                    if (MMITHEME_Is3DOpen())
                    {
                        MMIDEFAULT_SaveOldMoveBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);

                        base_handle = MMK_GetParentWinHandle(win_handle);

                        if (0 != base_handle)
                        {
                            MMK_GetWinAngle(win_handle,&trans_info.pre_angle);
                            trans_info.win_handle = base_handle;
                        }
                        else
                        {
                            MMK_GetWinAngle(win_handle,&trans_info.pre_angle);
                            trans_info.win_handle = win_handle;
                        }

                        MMK_GetWinAngle(win_handle,&trans_info.pre_angle);
                        trans_info.iDirection = 1;
                        s_last_closed_window_is_3d_enable = TRUE;
                    }
                    else
                    {
                        s_last_closed_window_is_3d_enable = FALSE;
                        //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_CLOSE_WINDOW: roll close!"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5001_112_2_18_2_0_5_184,(uint8*)"");
                    }
                }
                else
                {
                    s_last_closed_window_is_3d_enable = FALSE;
                    //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_CLOSE_WINDOW: IsDisable3DWindow!"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5007_112_2_18_2_0_5_185,(uint8*)"");
                }
                s_close_fullpaint_match = TRUE;
            }
            else
            {
                //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_CLOSE_WINDOW: not focus win!"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5013_112_2_18_2_0_5_186,(uint8*)"");
            }
        }
        else
        {
            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_CLOSE_WINDOW: not Permit3D!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5018_112_2_18_2_0_5_187,(uint8*)"");
        }
        break;

    case MSG_OPEN_WINDOW:
        s_close_fullpaint_match = FALSE;
        s_is_open_paint_order   = TRUE;
        s_cur_win_anim_type = MMITHEME_Get3DScreenAnimType(win_handle, MMI_WIN_DISP_STATE_LOSE_FOCUS);
        break;

    case MSG_FULL_PAINT:
        if(HandlePopMenuEffect(win_handle))
        {
            break;
        }

/*        if (s_tab_win_switch_item)
        {
            HandleTabEffect(win_handle);
            s_tab_win_switch_item = FALSE;
            break;
        }
*/
        if( ((s_saved_move_buf_type != SCR_EFFECT_BUF_TYPE_WIN_SWITCH))
            || MMK_IsQuickKey() )
        {
            MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
            break;
        }

        if (MMI_IsPermit3D((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
        {
            base_handle = MMK_GetParentWinHandle(win_handle);

            if (0 != base_handle)
            {
                MMK_GetWinAngle(base_handle,&trans_info.next_angle);
            }
            else
            {
                MMK_GetWinAngle(win_handle,&trans_info.next_angle);
            }
            
            if (trans_info.pre_angle != trans_info.next_angle)
            {
                MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
                break;
            }
            
            if (!IsDisable3DWindow(MMK_GetFocusWinHandle())) //focus_win_id: 像popmenu这样的窗口会给prev窗口发fullpaint消息
            {
                /* if (!(MMK_IsChildWin(win_handle)&&MMK_IsChildWin(trans_info.win_handle)))*/  //tab页间切换禁止场景翻转特效
                {
                    if ( ((s_close_fullpaint_match && s_last_closed_window_is_3d_enable) || (!s_close_fullpaint_match && s_is_open_paint_order)))
                    {
                        if (MMK_IsChildWin(win_handle))
                        {
                            //延迟，让后面的父窗口的MSG_FULL_PAINT去处理
                            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: delay to parent win!"
                            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5075_112_2_18_2_0_5_188,(uint8*)"");
                            break;
                        }
#ifdef MENU_CRYSTALCUBE_SUPPORT
                        else if (MMITHEME_Is3DOpen()
                            && (MMK_ConvertIdToHandle( MAIN_MAINMENU_WIN_ID ) == trans_info.win_handle
                            || MMK_ConvertIdToHandle( MAIN_MAINMENU_WIN_ID ) == win_handle)
                            && GUIMENU_STYLE_CRYSTAL_CUBE == MMIAPISET_GetMainmenuStyle())
                        {
                            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: no need to roll!"
                            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5084_112_2_18_2_0_5_189,(uint8*)"");
                        }
#endif
#ifdef MENU_CYCLONE_SUPPORT
                        else if (MMITHEME_Is3DOpen()
                            && (MMK_ConvertIdToHandle( MAIN_MAINMENU_WIN_ID ) == trans_info.win_handle
                            || MMK_ConvertIdToHandle( MAIN_MAINMENU_WIN_ID ) == win_handle)
                            && GUIMENU_STYLE_CYCLONE == MMIAPISET_GetMainmenuStyle())
                        {
                            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: no need to roll!"
                            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5093_112_2_18_2_0_5_190,(uint8*)"");
                        }
#endif           
#ifdef MENU_CARDS_SUPPORT
                        else if (MMITHEME_Is3DOpen()
                            && (MMK_ConvertIdToHandle( MAIN_MAINMENU_WIN_ID ) == trans_info.win_handle
                            || MMK_ConvertIdToHandle( MAIN_MAINMENU_WIN_ID ) == win_handle)
                            && GUIMENU_STYLE_CARDS == MMIAPISET_GetMainmenuStyle())
                        {
                            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: no need to roll!"
                            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5102_112_2_18_2_0_5_191,(uint8*)"");
                        }
#endif                           
                        else
                        {
                            if ((MMITHEME_Is3DOpen())
                                || ( MMK_ConvertIdToHandle( MAIN_IDLE_WIN_ID ) == trans_info.win_handle 
                                || MMK_ConvertIdToHandle( MAIN_IDLE_WIN_ID ) == win_handle)
                                )
                            {
                                MMI_WIN_ANIM_PARAM_T anim_param = {0};
                                
                                //test code
                                anim_param.lcd_id = LCD_ID_0;
                                
#ifdef EFFECT_ZOOM_SUPPORT
                                if (MMISET_3D_STYLE_SCREEN_DEFAULT == MMISET_Get3DStyleScreen())
                                {
                                    if (MMI_WIN_TYPE_MAINMENU == s_last_close_win_type)
                                    {
                                        s_cur_win_anim_type = MMI_WIN_ANIM_ZOOM_INOUT;
                                    }
                                }
#endif

                                anim_param.type = s_cur_win_anim_type;
                                anim_param.is_back = trans_info.iDirection ? TRUE : FALSE;
                                MMITHEME_PlayWinAnim( &anim_param );
                            }
                            else
                            {
                                //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: no need to roll!"
                                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5133_112_2_18_2_0_5_192,(uint8*)"");
                            }
                        }
                    }
                    s_last_close_win_type = MMI_WIN_TYPE_NONE;
                }
                /*else
                {
                    //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: Is Child Win Switch!"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5141_112_2_18_2_0_5_193,(uint8*)"");
                }*/
            }

            s_close_fullpaint_match = FALSE;

            s_is_open_paint_order   = FALSE;
        }
        else
        {
            //SCI_TRACE_LOW:"MMK_HandleWinMoveMsg MSG_FULL_PAINT: not Permit3D!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5151_112_2_18_2_0_5_194,(uint8*)"");
        }
        
        MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
        break;

    case MSG_TIMER:
        break;
                
    default:
        break;
    }
}
#endif

/*****************************************************************************/
// Description : 是否禁止3D特效的窗口
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsDisable3DWindow(
                                MMI_HANDLE_T win_handle
                                )
{    
    //删除此代码，提高效率，给开机窗口加状态
//     if ( MMK_ConvertIdToHandle( PHONE_STARTUP_NORMAL_WIN_ID ) == win_handle )   //开机窗口PHONE_STARTUP_NORMAL_WIN_ID
//     {
//         return TRUE;
//     }
    
    if ((NULL == win_handle)  //窗口已经释放了
        || (MOVE_NOT_FULL_SCREEN_WINDOW == MMK_GetWinMoveStyle(win_handle))
        || (MOVE_FORBIDDEN == MMK_GetWinMoveStyle(win_handle)))
    {
        return TRUE;
    }    
    
    return FALSE;
}

/*****************************************************************************/
// Description : 3D特效是否开启
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMI_Is3DMMIEnable(void)
{
    return TRUE; //s_is_enable_3dmmi;
}

#ifdef UI_P3D_SUPPORT
/*****************************************************************************/
// Description : disable 3d effect module. 
// Global resource dependence : 
// Author: .
// Note:
/*****************************************************************************/
PUBLIC void MMI_Disable3DEffect(MMI_3D_EFFECT_MOD_E mod_val)
{
    s_mmi_3d_effect_data = s_mmi_3d_effect_data | mod_val;

    //try to free 3d buffer.
    MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);
}

/*****************************************************************************/
// Description : enable 3d effect module.
// Global resource dependence : 
// Author: .
// Note:
/*****************************************************************************/
PUBLIC void MMI_Enable3DEffect(MMI_3D_EFFECT_MOD_E mod_val)
{
    s_mmi_3d_effect_data = s_mmi_3d_effect_data & (~mod_val);
}
#endif

/*****************************************************************************/
// Description : 3d block_mem没有被别的模块使用（如果被别的模块使用，就不启动3d特效）
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
LOCAL BOOLEAN Is3DBlockMemUsedByOther(BLOCK_MEM_ID_E block_id)
{
    BOOLEAN result = FALSE;

    if (ALLOC_BY_OTHERS == BL_GetStatus((BLOCK_MEM_ID_E)block_id))
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return result;
}

/*****************************************************************************/
// Description : 是否允许启用3D翻转特效
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMI_IsPermit3D(BLOCK_MEM_ID_E block_id)
{
    BOOLEAN is_3dblockmem_usedbyother = Is3DBlockMemUsedByOther((BLOCK_MEM_ID_E)block_id);

    s_mmi_3d_effect_data &= (uint32)MMI_3D_EFFECT_MOD;
    if (0 != s_mmi_3d_effect_data )
    {
        return FALSE;
    }

    if (!is_3dblockmem_usedbyother && IsPermit3D())
    {
        return TRUE;
    }
    //SCI_TRACE_LOW:"IsPermitDesktop3D is_3dblockmem_usedbyother = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5259_112_2_18_2_0_6_195,(uint8*)"d", is_3dblockmem_usedbyother);
    return FALSE;
}

/*****************************************************************************/
// Description : 是否允许启用3D特效
// Global resource dependence : 
// Author: bin.ji
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsPermit3D(void)
{
    BOOLEAN is_3dmmi_enable = MMI_Is3DMMIEnable();
    BOOLEAN is_play_mp3bg = MMISRVAUD_IsAudPlaying(MMIAPIMP3_GetBGPlayerHandle());
    BOOLEAN is_play_kurobg = MMIAPIKUR_IsPlayerActive();
    BOOLEAN is_IQ_mode = MMIAPIENG_GetIQModeStatus();
    BOOLEAN is_normal_eq = TRUE;

#ifdef P3D_AUDIO_EQ_CONFLICT
    MMISRVAUD_AUD_CFG_T cfg = {0};

    if(MMISRVAUD_IsActive())
    {
        MMISRVAUD_GetAudCfg(&cfg);
        if(cfg.eq_mode != MMISRVAUD_EQ_REGULAR)
        {
            is_normal_eq = FALSE;
        }
    }
#endif

    if (is_normal_eq                &&
        !is_IQ_mode)/*lint !e774*/
    {
        return TRUE;
    }
    //SCI_TRACE_LOW:"MMI_IsPermit3D is_3dmmi_enable = %d, is_play_mp3bg = %d, is_play_kurobg = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5294_112_2_18_2_0_6_196,(uint8*)"ddd",is_3dmmi_enable, is_play_mp3bg, is_play_kurobg);
    return FALSE;    
}

/*****************************************************************************/
//  Description : 分配3d场景切换的内存，开始场景切换时调用
//  Global resource dependence : 
//  Author: bin.ji
//  Note: TRUE success, FALSE fail
/*****************************************************************************/
LOCAL BOOLEAN AllocTrans3DBuf(
                              uint16 **old_buf_pptr,  //out, 保存旧场景的buf
                uint16 **new_buf_pptr       //out, 保存新场景的buf
                              )
{
    BOOLEAN             result = FALSE;
    uint32              block_size = 0;
    BLOCK_MEM_STATUS_E  status = NOT_USE;
    
    SCI_ASSERT(old_buf_pptr); /*assert verified*/
    SCI_ASSERT(new_buf_pptr); /*assert verified*/
    
    status = BL_GetStatus((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP);

    switch (status)
    {
    case NOT_USE:
        *old_buf_pptr = (uint16*)MMI_BL_MALLOC((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP);        
        if(PNULL != *old_buf_pptr)
        {
            /* make buff 256 bytes align */
            *old_buf_pptr = (uint16*)((((uint32)(*old_buf_pptr) + 255)/256)*256);
            block_size = BL_GetSize((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP);
#ifdef UI_P3D_SUPPORT
            SCI_MEMSET(*old_buf_pptr, MMI3D_KEY_COLOR_MEMSET, SCR_SIZE * 2);
#endif
            *new_buf_pptr = *old_buf_pptr + SCR_SIZE/2;
            /* make buff 256 bytes align */
            *new_buf_pptr = (uint16*)((((uint32)(*new_buf_pptr) + 255)/256)*256);
            result = TRUE;
        }
        else
        {
            *new_buf_pptr = PNULL;
            result = FALSE;
        }
        break;

    case ALLOC_BY_SELF: //被3d自己使用（已分配空间还未释放）
        MMI_GetTrans3DBuf(old_buf_pptr, new_buf_pptr);
        result = TRUE;
        break;

    case ALLOC_BY_OTHERS:   //被别人使用，分配失败
        result = FALSE;
        break;

    default:
        SCI_PASSERT(FALSE, ("BL_GetStatus = %d", status)); /*assert verified*/
        break;
    }

    return result;
}

/*****************************************************************************/
//  Description : 释放3d场景切换的内存，场景切换结束时调用
//  Global resource dependence : 
//  Author: Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC void MMIDEFAULT_FreeSavedMove3DBuf(SCR_EFFECT_BUF_TYPE_E buf_type)
{
    if(SCR_EFFECT_BUF_TYPE_NONE != s_saved_move_buf_type && buf_type == s_saved_move_buf_type)
    {
        MMI_BL_FREE((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP);
        s_saved_move_buf_type = SCR_EFFECT_BUF_TYPE_NONE;
    }
}

/*****************************************************************************/
//  Description : 保存旧的场景内存
//  Global resource dependence : 
//  Author: Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SaveOldMoveBuf(SCR_EFFECT_BUF_TYPE_E buf_type)
{
    uint16 *buf_ptr     = 0;
    uint16 buf_width    = 0;
    LCD_ID_E lcd_id = LCD_ID_0;
    uint16 *old_buf_ptr = PNULL;
    uint16 *new_buf_ptr = PNULL;

    if(SCR_EFFECT_BUF_TYPE_NONE != s_saved_move_buf_type && SCR_EFFECT_BUF_TYPE_WIN_SWITCH != s_saved_move_buf_type)
    {
        return;
    }
    
    GUILCD_GetLogicWidthHeight(lcd_id,&buf_width,PNULL);
    buf_ptr = GUILCD_GetMainLcdBufPtr();
    
    if (AllocTrans3DBuf(&old_buf_ptr, &new_buf_ptr))
    {
        SCI_MEMCPY(old_buf_ptr, buf_ptr, SCR_SIZE);
        s_saved_move_buf_type = buf_type;
    }
    else
    {
        //SCI_TRACE_LOW:"SaveOldMoveBuf AllocTrans3DBuf fail"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5397_112_2_18_2_0_6_197,(uint8*)"");
    }
}

/*****************************************************************************/
//  Description : 保存旧的场景内存到New Buff
//  Global resource dependence : 
//  Author: Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SaveMoveToNewBuf(void)
{
    uint16 *old_buf_ptr = PNULL;
    uint16 *new_buf_ptr = PNULL;
    
    uint16 *buf_ptr = GUILCD_GetMainLcdBufPtr();
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    
    UILAYER_InvalidateRect(LCD_ID_0, &rect);

    if (AllocTrans3DBuf(&old_buf_ptr, &new_buf_ptr))
    {
        if (new_buf_ptr)
        {
            SCI_MEMCPY(new_buf_ptr, buf_ptr, SCR_SIZE);
        }
    }
}

/*****************************************************************************/
//  Description : 保存旧的场景内存
//  Global resource dependence : 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
PUBLIC void MMI_GetTrans3DBuf(
                              uint16 **old_buf_pptr,
                              uint16 **new_buf_pptr
                              )
{
    SCI_ASSERT(old_buf_pptr); /*assert verified*/
    SCI_ASSERT(new_buf_pptr); /*assert verified*/
    
    if (ALLOC_BY_SELF == BL_GetStatus((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP))
    {
        *old_buf_pptr = (uint16*)BL_GetBlockPtr((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP);
        
        /* make buff 256 bytes align */
        *old_buf_pptr = (uint16*)((((uint32)(*old_buf_pptr) + 255)/256)*256);

        *new_buf_pptr = *old_buf_pptr + SCR_SIZE/2;
        /* make buff 256 bytes align */
        *new_buf_pptr = (uint16*)((((uint32)(*new_buf_pptr) + 255)/256)*256);
    }
    else    
    {
        //未分配或者已被别人使用
        *old_buf_pptr = PNULL;
        *new_buf_pptr = PNULL;
    }
}

/*****************************************************************************/
//  Description : 灭屏
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void MMIDEFAULT_CloseAllLight(void)
{
    if(
        MMK_IsOpenWin(PHONE_STARTUP_NORMAL_WIN_ID) 
        ||MMK_IsOpenWin(PHONE_POWER_OFF_WIN_ID)
#ifdef JAVA_SUPPORT_MYRIAD
        ||MMIAPIJAVA_IsWaitingWinOpen()
#endif
        )
    {
        return;
    }
    //close key
    MMIDEFAULT_StopKeybadBackLightTimer();
    MMIDEFAULT_SetKeybadBackLight(FALSE);

    //close auto lock timer
    MMIDEFAULT_StopAutoKeylockTimer();

    //close lcd light
    CloseAllLight(TRUE);
}

#ifdef LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT 
/*****************************************************************************/
//  Description : 特效灭屏
//  Global resource dependence : 
//  Author: yaye.jiang
//  Note:
/*****************************************************************************/
PUBLIC void MMISEPC_CloseAllLight(void)
{ 
	//亮屏操作下，才进入下一步的操作
    if (MMIDEFAULT_GetBackLightState() 
        && !MMK_IsOpenWin(PHONE_STARTUP_NORMAL_WIN_ID) 
        && !MMK_IsOpenWin(PHONE_POWER_OFF_WIN_ID)
#ifdef JAVA_SUPPORT_MYRIAD
        && !MMIAPIJAVA_IsWaitingWinOpen()
#endif
        )
    {
		MMI_CreatScreenCloseWin();
    }
}
#endif

/*****************************************************************************/
//  Description : close lcd light
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void CloseAllLight(
                         BOOLEAN    is_lock
                         )
{
    POWER_RESTART_CONDITION_E   start_condition = POWER_GetRestartCondition();

    #ifdef MMI_KEY_LOCK_BACKLIGHT_IN_1_SUPPORT
    MMIDEFAULT_HandleAutoKeylockOver();
    #endif

    //stop lcd back light timer
    MMIDEFAULT_StopLcdBackLightTimer();
#ifdef SCREENSAVER_SUPPORT
    //close screen saver
    MMIAPIIDLESS_CloseScreenSaver();
#endif
    //关闭背光
    GPIO_SetLcdBackLight(FALSE);
    s_backlight_status = MMI_BACKLIGHT_OFF; 
    MMI_TraceAutoBackLightStatus(s_backlight_status);   
     
    
    //enter auto key lock
    if (is_lock &&
        !MMIAPIKL_IsPhoneLocked() && 
        (!MMIAPICC_IsInState(CC_IN_CALL_STATE)) &&
        ((RESTART_BY_ALARM != start_condition) && (RESTART_BY_CHARGE != start_condition))
#ifndef MMI_PDA_SUPPORT
        &&(!MMIAPIALM_IsAliveWinOpened())//闹铃，倒计时提示界面均不允许锁键
        &&(!MMIAPICT_IsArriveWinOpened())//
#endif
#ifdef WRE_SUPPORT
        &&(!MMIWRE_Actived())//WRE运行时不允许锁键
#endif
#ifdef MRAPP_SUPPORT
        &&(MMIMRAPP_GetVMState() != VM_RUN)
#endif
#ifdef BROWSER_SUPPORT_DORADO 
        &&(!MMIAPIBROWSER_IsRunning())
#endif
        )
    {
        MMIAPIKL_LockPhone();
        MMK_PostMsg(VIRTUAL_WIN_ID, MSG_SLEEP_LCD, PNULL, 0);
    }
    else
    {
       // s_backlight_status = MMI_BACKLIGHT_OFF; 

        // 关闭LCD
        MMIDEFAULT_SetLcdSleep(TRUE);
    }
                    
    //进入deep sleep
    MMIDEFAULT_SetDeepSleep(TRUE);
    
    // @Great.Tian, handle
    MMIAPIIDLE_HandleLCDBacklight(FALSE);
    
    if (s_backlight_ctrol == MMI_BACKLIGHT_CTRL_NORMAL_HALF_ON)
    {
        SetNormalHalfOnBackLight(FALSE);
    }
    
    //send system backlight turn off message to  control and window
    MMK_SendMsg(MMK_GetFocusWinId(), MSG_BACKLIGHT_TURN_OFF, PNULL);
}

/*****************************************************************************/
//  Description : stop the auto keylock timer           
//  Global resource dependence : 
//  Author: apple.zhang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StopAutoKeylockTimer(void)
{
    MMK_StopTimer(MMI_AUTO_KEYLOCK_TIMER_ID);
}

/*****************************************************************************/
//  Description : to stop auto keylock or handle other operation when the keylock
//                timer received   
//  Global resource dependence : 
//  Author: apple.zhang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_HandleAutoKeylockOver( void )
{
    POWER_RESTART_CONDITION_E   start_condition = POWER_GetRestartCondition();
    MMISET_KEYLOCK_TYPE_E auto_keylock_timer = {0};   

    auto_keylock_timer = MMIAPISET_GetAutoBoardKey();  
    
    MMIDEFAULT_StopAutoKeylockTimer();
    //enter auto key lock
    if (s_is_allow_keylock 
        && (MMISET_KEYLOCK_CLOSE != auto_keylock_timer) 
        && (!MMIAPIKL_IsPhoneLocked())&&(RESTART_BY_ALARM != start_condition) 
        && (RESTART_BY_CHARGE != start_condition)
        //如果有tp或kbd按下, 不能锁键盘
        && !IsKbdTpPress()
#ifdef LCD_SLIDE_SUPPORT
        //滑盖关闭时才自动锁键
        && (MMK_SLIDE_CLOSE == MMIDEFAULT_GetSlideStatus())
#endif
#ifdef MRAPP_SUPPORT
        &&(MMIMRAPP_GetVMState() != VM_RUN)
#endif
#ifdef BROWSER_SUPPORT_DORADO 
        &&(!MMIAPIBROWSER_IsRunning())
#endif
        )
    {
        MMIAPIKL_LockPhone();       
    }               
    
    return TRUE;
}
/*****************************************************************************/
//  Description : start the auto keylock timer           
//  Global resource dependence : 
//  Author: apple.zhang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_StartAutoKeylockTimer(void)
{
#ifndef MMI_KEY_LOCK_BACKLIGHT_IN_1_SUPPORT
    MMISET_KEYLOCK_TYPE_E auto_keylock_timer = {0};   
    auto_keylock_timer = MMIAPISET_GetAutoBoardKey();  
    if (s_is_allow_keylock &&(MMISET_KEYLOCK_CLOSE != auto_keylock_timer) && !MMIAPIKL_IsPhoneLocked())
    {
        MMK_StartWinTimer(
        VIRTUAL_WIN_ID,
        MMI_AUTO_KEYLOCK_TIMER_ID,
        auto_keylock_timer,
        FALSE);
    }
#endif
}

/******************************************************************************/
/*! \brief  whether allow open key lock        
 *  \author apple.zhang
 *  \param[in] allow_keylock whether allow keylock.
 */
/******************************************************************************/
PUBLIC void MMIDEFAULT_AllowOpenKeylocWin(BOOLEAN allow_keylock)
{   
    /* Bug fix: cr00198061. If in call state, we don't allow to lock keypad anywhere */
    if(allow_keylock)
    {
        if(MMIAPICC_IsInState(CC_IN_CALL_STATE))
        {
            return;
        }
    }
    
    s_is_allow_keylock = allow_keylock;    
    if (allow_keylock)
    {        
        MMIDEFAULT_StartAutoKeylockTimer();
    }
    else
    {        
        MMIDEFAULT_StopAutoKeylockTimer();

    }     
    
}

typedef struct  
{
    uint8 qwerty_key;
    uint8 typical_key;
}QWERTY_TYPICAL_KEY_CONVERT_T;

LOCAL const QWERTY_TYPICAL_KEY_CONVERT_T s_qwerty_to_typical_key_convert[] = 
{
#ifdef QWERTY_KEYPAD_BLACKBERRY
    { KEY_W, KEY_1 },
    { KEY_E, KEY_2 },
    { KEY_R, KEY_3 },
    { KEY_S, KEY_4 },
    { KEY_D, KEY_5 },
    { KEY_F, KEY_6 },
    { KEY_Z, KEY_7 },
    { KEY_X, KEY_8 },
    { KEY_C, KEY_9 },
    { KEY_A, KEY_STAR },
    { KEY_Q, KEY_HASH },
//#elif defined QWERTY_KEYPAD_NOKIA
#else
    { KEY_M, KEY_0 },
    { KEY_R, KEY_1 },
    { KEY_T, KEY_2 },
    { KEY_Y, KEY_3 },
    { KEY_F, KEY_4 },
    { KEY_G, KEY_5 },
    { KEY_H, KEY_6 },
    { KEY_V, KEY_7 },
    { KEY_B, KEY_8 },
    { KEY_N, KEY_9 },
    { KEY_U, KEY_STAR },
    { KEY_J, KEY_HASH }
#endif //QWERTY_KEYPAD_BLACKBERRY
};

/*****************************************************************************/
//  Description : convert qwerty key to typical key
//  Global resource dependence : 
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
PUBLIC uint8 MMIDEFAULT_ConvertQwertyKeyToTypicalKey( uint8 src_key )
{
    uint8 dst_key = src_key;

    uint32 i = 0;
    uint32 table_size = ARR_SIZE( s_qwerty_to_typical_key_convert );

    for ( ; i < table_size; i++ )
    {
        if ( src_key == s_qwerty_to_typical_key_convert[i].qwerty_key )
        {
            dst_key = s_qwerty_to_typical_key_convert[i].typical_key;
            break;
        }
    }

    return dst_key;
}

#ifdef KEYPAD_TYPE_QWERTY_KEYPAD
LOCAL const QWERTY_TYPICAL_KEY_CONVERT_T s_typical_to_qwerty_key_convert[] = 
{
#ifdef QWERTY_KEYPAD_BLACKBERRY
    { KEY_W, KEY_1 },
    { KEY_E, KEY_2 },
    { KEY_R, KEY_3 },
    { KEY_S, KEY_4 },
    { KEY_D, KEY_5 },
    { KEY_F, KEY_6 },
    { KEY_Z, KEY_7 },
    { KEY_X, KEY_8 },
    { KEY_C, KEY_9 },
    { KEY_A, KEY_STAR },
    { KEY_Q, KEY_HASH },
    { KEY_O, KEY_PLUS },
    { KEY_I, KEY_MINUS },
    { KEY_L, KEY_DOUBLE_QUOTES },
    { KEY_T, KEY_LEFT_PARENTHESIS },
    { KEY_Y, KEY_RIGHT_PARENTHESIS },
    { KEY_J, KEY_SEMICOLON },
    { KEY_H, KEY_COLON },
    { KEY_P, KEY_AT_QWERTY },
    { KEY_G, KEY_SLASH},
    { KEY_K, KEY_SHIFT_AND},    
    { KEY_V, KEY_QUESTION}, 
    { KEY_B, KEY_EXCLAMATION}, 
    { KEY_N, KEY_COMMA}, 
    { KEY_M, KEY_PERIOD} 
//#elif defined QWERTY_KEYPAD_NOKIA
#else
    { KEY_M, KEY_0 },
    { KEY_R, KEY_1 },
    { KEY_T, KEY_2 },
    { KEY_Y, KEY_3 },
    { KEY_F, KEY_4 },
    { KEY_G, KEY_5 },
    { KEY_H, KEY_6 },
    { KEY_V, KEY_7 },
    { KEY_B, KEY_8 },
    { KEY_N, KEY_9 },
    { KEY_U, KEY_STAR },
    { KEY_J, KEY_HASH },
    { KEY_I, KEY_PLUS },
    { KEY_K, KEY_MINUS },
    { KEY_L, KEY_DOUBLE_QUOTES },
    { KEY_O, KEY_LEFT_PARENTHESIS },
    { KEY_P, KEY_RIGHT_PARENTHESIS },
    { KEY_COMMA, KEY_SEMICOLON },
    { KEY_PERIOD, KEY_COLON },
    { KEY_SLASH, KEY_AT_QWERTY },
    { KEY_SHIFT_AND,KEY_AND   },
    { KEY_EXCLAMATION, KEY_QUESTION }
#endif //QWERTY_KEYPAD_BLACKBERRY
};

/*****************************************************************************/
//  Description : convert typical key to qwerty key
//  Global resource dependence : 
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
PUBLIC uint8 MMIDEFAULT_ConvertTypicalKeyToQwertyKey( uint8 src_key )

{
    uint8 dst_key = src_key;
    
    uint32 i = 0;
    uint32 table_size = ARR_SIZE( s_typical_to_qwerty_key_convert );
    
    for ( ; i < table_size; i++ )
    {
        if ( src_key == s_typical_to_qwerty_key_convert[i].typical_key )
        {
            dst_key = s_typical_to_qwerty_key_convert[i].qwerty_key;
            break;
        }
    }

    return dst_key;
}

/*****************************************************************************/
//  Description : is shift key
//  Global resource dependence : 
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsShiftKey( 
                                     uint8 key
                                     )
{
    return (BOOLEAN)( KEY_SHIFT == key );
}

/*****************************************************************************/
//  Description : get shift key
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
PUBLIC uint8 MMIDEFAULT_GetShiftKey( uint8 src_key )
{
    uint8 dst_key = src_key;
    
    uint32 i = 0;
    uint32 table_size = ARR_SIZE( s_typical_to_qwerty_key_convert );
    
    for ( ; i < table_size; i++ )
    {
        if ( src_key == s_typical_to_qwerty_key_convert[i].qwerty_key )
        {
            dst_key = s_typical_to_qwerty_key_convert[i].typical_key;
            break;
        }
    }

    return dst_key;
}

#endif

#ifdef LCD_SLIDE_SUPPORT
/*****************************************************************************/
//  Description : convert slide key           
//  Global resource dependence : 
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_ConvertSlideKey(
                                       uint32 *key_code_ptr,
                                       uint16 *single_code_ptr
                                       )
{
    if ((PNULL == key_code_ptr)
        || (PNULL == single_code_ptr))
    {
        //SCI_TRACE_LOW:"MMIDEFAULT_ConvertSlideKey input param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5832_112_2_18_2_0_7_198,(uint8*)"");
        return;
    }
#ifdef LCD_SLIDE_SIMU
    //KEY_STAR模拟滑盖消息
    if (SCI_VK_STAR == *key_code_ptr)
    {
        if (KPD_DOWN == *single_code_ptr)
        {
            *key_code_ptr = SCI_VK_SLIDE;
            if (MMK_SLIDE_OPEN == s_slide_status)
            {
                s_slide_status = MMK_SLIDE_CLOSE;
                *single_code_ptr = KPD_DOWN;
            }
            else
            {
                s_slide_status = MMK_SLIDE_OPEN;
                *single_code_ptr = KPD_UP;
            }
        }
        else
        {
            return;
        }
    }
#else
    if (SCI_VK_SLIDE == *key_code_ptr)
    {
        if (KPD_DOWN == *single_code_ptr)
        {
            s_slide_status = MMK_SLIDE_CLOSE;
        }
        else
        {
            s_slide_status = MMK_SLIDE_OPEN;
        }
    }
#endif
}

/*****************************************************************************/
//  Description : get the slide status         
//  Global resource dependence : 
//  Author: bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC MMK_SLIDE_STATUS_E MMIDEFAULT_GetSlideStatus(void)
{
    return s_slide_status;
}

#endif

/*****************************************************************************/
//  Description : cancel current window effect          
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_CancelCurWin3DEffect(void)
{
    s_cur_win_anim_type = MMI_WIN_ANIM_NONE; 
}


/*****************************************************************************/
//  Description : set current window effect          
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetCurWin3DEffect(MMI_WIN_ANIM_TYPE_E type)
{
    s_cur_win_anim_type = type; 
}


#ifdef DPHONE_SUPPORT 
/*****************************************************************************/
// 	Description : set the tone on       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetToneStates(
										BOOLEAN is_on//TRUE: on, FALSE:off
									 )
{
	g_hook_hf_status.is_tone_on=is_on;
	//SCI_TRACE_LOW:"MMIDEFAULT_SetToneStates is_tone_on=%d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5914_112_2_18_2_0_7_199,(uint8*)"d",g_hook_hf_status.is_tone_on);
#ifndef WIN32
	l1_set_dsp_sleep_flag(!is_on);
#endif
}

/*****************************************************************************/
// 	Description : set the handfree states       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetHandFreeStatesAndLed(
										        BOOLEAN state //TRUE:handfree used,FALSE:handfree unused
										       )
{
	g_hook_hf_status.is_handfree = state;
	//SCI_TRACE_LOW:"MMIDEFAULT_SetHandFreeStatesAndLed: is_handfree=%d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5929_112_2_18_2_0_7_200,(uint8*)"d",g_hook_hf_status.is_handfree);
    #ifndef WIN32
    GPIO_EnableHandFreeILed(state);
	#endif
}

/*****************************************************************************/
// 	Description : init the hook hand free and tone status       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_InitHookAndHF(void)
{
	g_hook_hf_status.is_handfree=FALSE;
	g_hook_hf_status.is_hook=FALSE;
	g_hook_hf_status.is_tone_on=FALSE;
}


/*****************************************************************************/
// 	Description :     
//	Global resource dependence : 
//  Author: Will.Shao
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsToneOn(void)
{
	return	g_hook_hf_status.is_tone_on;
}


/*****************************************************************************/
// 	Description : set the hook states       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetHookStates(
									 BOOLEAN state //TRUE:hook used,FALSE:hook unused
									 )
{
	g_hook_hf_status.is_hook=state;
	//SCI_TRACE_LOW:"MMIDEFAULT_SetHookStates: is_hook=%d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5966_112_2_18_2_0_7_201,(uint8*)"d",g_hook_hf_status.is_hook);
}


/*****************************************************************************/
// 	Description : set the handfree states       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_SetHandFreeStates(
										 BOOLEAN state //TRUE:handfree used,FALSE:handfree unused
										 )
{
	g_hook_hf_status.is_handfree=state;
	//SCI_TRACE_LOW:"MMIDEFAULT_SetHandFreeStates: is_handfree=%d, %s, %d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5979_112_2_18_2_0_7_202,(uint8*)"dsd",g_hook_hf_status.is_handfree);
}

/*****************************************************************************/
// 	Description : get the handfree states       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_GetHandFreeStates(void)
{
	//SCI_TRACE_LOW:"MMIDEFAULT_GetHandFreeStates: _is_handfree=%d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5988_112_2_18_2_0_7_203,(uint8*)"d",g_hook_hf_status.is_handfree);
	return (g_hook_hf_status.is_handfree);
}

/*****************************************************************************/
// 	Description : get the hook states       
//	Global resource dependence : 
//  Author: alex.ye
//	Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_GetHookStates(void)
{
	//SCI_TRACE_LOW:"MMIDEFAULT_GetHookStates: is_hook=%d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_5998_112_2_18_2_0_7_204,(uint8*)"d",g_hook_hf_status.is_hook);
	return (g_hook_hf_status.is_hook);
}


#endif


/*****************************************************************************/
//  Description : get the back light state of the mmi moudal           
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC MMI_BACKLIGHT_STATUS_E MMIDEFAULT_GetBackLightStateEx(void)
{
      //SCI_TRACE_LOW:"MMIDEFAULT_GetBackLightState: %d"
      SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_6012_112_2_18_2_0_7_205,(uint8*)"d",s_backlight_status);
     return s_backlight_status;
}

/*****************************************************************************/
//  Description : Link Process about key_tp ring on or off.
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
LOCAL MMI_LINK_NODE_T *LinkProcess(MMI_LINK_NODE_T *link_head_ptr, const char *module_name_ptr, BOOLEAN is_enable)
{
    
    MMI_LINK_NODE_T *result_search_ptr = PNULL;
    MMI_LINK_NODE_T *add_node_ptr = PNULL;
    MMI_LINK_NODE_T *link_head_ret_ptr = link_head_ptr;
    char *off_key_style_ptr = PNULL;
    
    if(PNULL == module_name_ptr)
    {
        return link_head_ret_ptr;
    }
    
    if(is_enable)
    {   
        if(PNULL != link_head_ret_ptr)
        {
            result_search_ptr = MMILINK_AudioSearchNode(link_head_ret_ptr, module_name_ptr);
            if(PNULL != result_search_ptr)
            {
                SCI_FREE(((MMI_LINK_NODE_T*)result_search_ptr)->data);/*lint !e64*/
                link_head_ret_ptr = MMILINK_DestroyNode(result_search_ptr, link_head_ret_ptr, PNULL);
            }
        }
    }
    else
    {
        if(PNULL == link_head_ret_ptr)
        {
            off_key_style_ptr = (MMI_LINK_NODE_T *)SCI_ALLOC_APPZ(strlen(module_name_ptr)+1);/*lint !e64*/
            strcpy(off_key_style_ptr, module_name_ptr);
            link_head_ret_ptr = MMILINK_CreateHead((uint32)off_key_style_ptr);
        }    
        else
        {
            result_search_ptr = MMILINK_AudioSearchNode(link_head_ret_ptr, module_name_ptr); 
            if(PNULL == result_search_ptr)
            { 
                off_key_style_ptr = (MMI_LINK_NODE_T *)SCI_ALLOC_APPZ(strlen(module_name_ptr) + 1);/*lint !e64*/
                strcpy(off_key_style_ptr, module_name_ptr);
                add_node_ptr = MMILINK_CreateNode((uint32)off_key_style_ptr);
                MMILINK_AddNodeAfterBaseNode(link_head_ret_ptr, add_node_ptr);
            }
        }
    }
    
    return link_head_ret_ptr;
}

/*****************************************************************************/
//  Description : Recording the time of key ring turn off.
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_EnableKeyRing(char *module_name_ptr, BOOLEAN is_enable)
{  
    s_key_off_listhead_ptr = LinkProcess(s_key_off_listhead_ptr, module_name_ptr, is_enable);
}

/*****************************************************************************/
//  Description : Recording the time of tp ring turn off.
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDEFAULT_EnableTpRing(const char *module_name_ptr, BOOLEAN is_enable)
{
    s_tp_off_listhead_ptr = LinkProcess(s_tp_off_listhead_ptr, module_name_ptr, is_enable);
}

/*****************************************************************************/
//  Description : Recording the time of tp and key ring turn off.
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
//PUBLIC void MMIDEFAULT_EnableKeyTpRing(BOOLEAN is_enable)
PUBLIC void MMIDEFAULT_EnableKeyTpRing(char *modelname_ptr, BOOLEAN is_enable)
{
    MMIDEFAULT_EnableKeyRing(modelname_ptr, is_enable);
    MMIDEFAULT_EnableTpRing(modelname_ptr, is_enable);
}

/*****************************************************************************/
//  Description : If enable TP down ring.
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsEnableTpRing(void)
{
    return ((PNULL == s_tp_off_listhead_ptr) ? TRUE : FALSE);
}

/*****************************************************************************/
//  Description : If enable key down ring.
//  Global resource dependence : 
//  Author:yanxian.zhou
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsEnableKeyRing(void)
{
    return ((PNULL == s_key_off_listhead_ptr) ? TRUE : FALSE);
}

/*****************************************************************************/
//  Description:   find a node with condition
//	Global resource dependence: 
//  Author: yanxian.zhou
//	Note:
/*****************************************************************************/
LOCAL MMI_LINK_NODE_T *MMILINK_AudioSearchNode(MMI_LINK_NODE_T const *node_ptr, char const*condition)
{
    BOOLEAN is_found = FALSE;
    MMI_LINK_NODE_T *move_node_ptr = (MMI_LINK_NODE_T*)node_ptr;
    MMI_LINK_NODE_T *next_node_ptr = PNULL;
    if(PNULL != move_node_ptr)
    {
        do 
        {
            next_node_ptr = move_node_ptr->next_ptr;                
            is_found = (BOOLEAN)(strcmp(condition, ((MMI_LINK_NODE_T*)move_node_ptr)->data) == 0);  /*lint !e64*/              
            if(is_found)
            {
                break;
            }
            
            if(PNULL == next_node_ptr->next_ptr)
            {
                break;
            }        
        }while(PNULL != move_node_ptr);
    }         
    if(is_found)
    {
        /* found */
        return move_node_ptr;        
    }
    else
    {
        /* not found */
        return PNULL;
    }
}

/*****************************************************************************/
//  Description : judge whether can play new message alert ring
//  Global resource dependence :
//  Author: bruce.chi
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIDEFAULT_IsPermitPlayNewMsgRing(void)
{

    if( MMIAPIRECORD_IsOpened()
        //|| MMIAPIKUR_IsPlayerFocus()
        || MMIAPIDC_IsOpened()//因为isp不能和audio共用，会有冲突。所以在dc的整个过程都不能开audio。
#ifdef VIDEO_PLAYER_SUPPORT
        || MMIAPIVP_IsLiving()
#endif
#ifdef MOBILE_VIDEO_SUPPORT
        || MMIAPIMV_IsLiving()
#endif
#ifdef VIDEO_PLAYER_SUPPORT        
        //|| MMIAPIVP_IsOpenPlayerWin()
#endif
#ifdef MOBILE_VIDEO_SUPPORT
        //|| MMIAPIMV_IsOpenMobileVideo()
#endif
        || MMIAPIUDISK_IsInPCCamera()
#ifdef GPS_SUPPORT
        //|| MMIAPIGPS_IsGpsOpened( )     // GPS
#endif
#ifdef TTS_SUPPORT
        //|| MMIAPITTS_GetPlayStatus()
#endif
#ifdef ASP_SUPPORT
	    || MMIAPIASP_lsPlayingMusic()
#endif
#ifdef FM_SUPPORT
        ||MMIAPIFM_IsOpened()
#endif
#ifdef VT_SUPPORT
        || MMIAPIVT_IsVtCalling()
#endif
		|| MMIAPIATV_IsRecording()
//#ifdef MMI_BLUETOOTH
        //|| MMIAPIBT_IsOpenFileWin()  //蓝牙在接收或传送文件时，会使信息铃声断续
//#endif
        //|| MMIAPIMMS_IsAudioInUse()
        //|| MMIAPIMMS_IsVideoInUse()
        //|| MMIAPIENG_IsFocusInLoopBack()
        //|| MMIAPIMTV_IsMTVOpen()
        //|| MMIAPIATV_IsATVOpen()
        //|| MMIAUDIO_IsInVoiceLoop()
        )
    {   
        return FALSE;
    }
    else
    {   
        return TRUE;
    }
}
#ifdef MMI_TURNON_BACKLIGHT_EFFECT_SUPPORT
/*****************************************************************************/
//  Description : DrawTurnOnBackLightAnim
//  Global resource dependence :
//  Author: juan.wu
//  Note:播放背光点亮渐变动画
/*****************************************************************************/
LOCAL BOOLEAN DrawTurnOnBackLightAnim(void)
{
    uint16    lcd_width = 0;
    uint16    lcd_height = 0;
    uint8     i = 0;
    UILAYER_PRINTSCREEN_INFO_T  screen_info = {0};
	SE_HANDLE handle =0;
	SE_CURTAIN_INIT_PARAM_T init_param = {0};
	SE_CURTAIN_RENDER_IN_T render_in = {0};
	SE_CURTAIN_RENDER_OUT_T render_out = {0};
    GUI_LCD_DEV_INFO        lcd_dev_info = {0, 0};
    MMI_WIN_ID_T        win_id = MMIKL_CLOCK_DISP_WIN_ID;
    GUI_RECT_T update_rect = MMITHEME_GetFullScreenRect();
    UILAYER_COPYBUF_PARAM_T  copy_param = {0};
    UILAYER_COPYBUF_T       copybuf_info = {0};
    UILAYER_CREATE_T        create_info = {0};
    //UILAYER_APPEND_BLT_T    append_layer = {0};
    UILAYER_RESULT_E        create_layer_result = 0;
    UILAYER_APPEND_BLT_T old_array[UILAYER_TOTAL_BLT_COUNT] = {0};
    UILAYER_APPEND_BLT_T cur_array[1] = {0};
    uint32 old_count = UILAYER_GetBltLayer(old_array,UILAYER_TOTAL_BLT_COUNT);//获得旧的叠加序列

    if(!MMK_IsOpenWin(win_id))
    {
        return FALSE;
    }
    if(!UILAYER_GetBltInfoForPrintScreen(&screen_info))
    {
        //SCI_TRACE_LOW:"DrawTurnOnBackLightAnim UILAYER_GetBltInfoForPrintScreen ERROR!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_6243_112_2_18_2_0_8_206,(uint8*)"");
        return FALSE;
    }
    GUILCD_GetLogicWidthHeight(lcd_dev_info.lcd_id, &lcd_width, &lcd_height);
    create_info.lcd_id = GUI_MAIN_LCD_ID;
    create_info.owner_handle = win_id;
    create_info.offset_x = 0;
    create_info.offset_y = 0;
    create_info.width = lcd_width;
    create_info.height = lcd_height;
    create_info.is_bg_layer = TRUE;
    create_info.is_static_layer = FALSE; 
    create_info.format = UILAYER_MEM_DOUBLE_COPY;
    create_layer_result = UILAYER_CreateLayer(&create_info, &lcd_dev_info);
    if(UILAYER_RESULT_SUCCESS != create_layer_result)
    {
        //SCI_TRACE_LOW:"DrawTurnOnBackLightAnim CreateLayer FAILED result = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_6259_112_2_18_2_0_8_207,(uint8*)"d",create_layer_result);
        return FALSE;
    }
    cur_array[0].lcd_dev_info = lcd_dev_info;//新的叠加序列
    UILAYER_SetBltLayer( cur_array, 1 );//将新的叠加序列替换
     
	init_param.act_mode = SE_CURTAIN_MODE_ONE;
	init_param.center_pt_x = lcd_width/2;
	init_param.center_pt_y = lcd_height/2;
	init_param.data_type = IMGREF_FORMAT_RGB565;		
	init_param.disp_rect.x = 0;
	init_param.disp_rect.y = 0;
	init_param.disp_rect.h = lcd_height;
	init_param.disp_rect.w = lcd_width;
#ifdef MAINLCD_SIZE_320X480//屏越大，速度越慢，针对大屏，减少帧数
    init_param.frame_num = 8;//8帧
#else
	init_param.frame_num = 12;//12帧
#endif
	init_param.graph_type = SE_GRAPH_CIRCLE;
	init_param.img_height = lcd_height;
	init_param.img_width = lcd_width;
	
    handle = SE_Create(SE_ID_CURTAIN,&init_param);
    
    if(0 == handle || init_param.frame_num <= 1)
    {
        //SCI_TRACE_LOW:"DrawTurnOnBackLightAnim 0 == handle"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_6286_112_2_18_2_0_8_208,(uint8*)"");
        UILAYER_RELEASELAYER(&lcd_dev_info);/*lint !e506 !e774*/
        return FALSE;
    }
    render_in.src_chn.chn0.ptr = SCI_ALLOCA(lcd_height * lcd_width * 2);
    if(PNULL == render_in.src_chn.chn0.ptr)
    {
        //SCI_TRACE_LOW:"DrawTurnOnBackLightAnim PNULL == render_in.src_chn.chn0.ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_6293_112_2_18_2_0_8_209,(uint8*)"");
        SE_Destory(handle);
        UILAYER_RELEASELAYER(&lcd_dev_info);/*lint !e506 !e774*/
        return FALSE;
    }
    SCI_MEMSET(render_in.src_chn.chn0.ptr,0,lcd_height * lcd_width * 2);
    SCI_MEMCPY(render_in.src_chn.chn0.ptr,screen_info.lcd_layer_blend.layer_info.buf_addr,lcd_height * lcd_width * 2);/*lint !e64*/
	render_in.src_chn.chn0.size = lcd_height * lcd_width * 2;
	
	render_in.dst_chn.chn0.ptr = SCI_ALLOCA(lcd_height * lcd_width * 2);
    if(PNULL == render_in.dst_chn.chn0.ptr)
    {
        SCI_FREE(render_in.src_chn.chn0.ptr);
        UILAYER_RELEASELAYER(&lcd_dev_info);/*lint !e506 !e774*/
        SE_Destory(handle);
        //SCI_TRACE_LOW:"DrawTurnOnBackLightAnim PNULL == render_in.dst_chn.chn0.ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMI_DEFAULT_6308_112_2_18_2_0_8_210,(uint8*)"");
        return FALSE;
    }
    SCI_MEMSET(render_in.dst_chn.chn0.ptr,0,lcd_height * lcd_width * 2);
	render_in.dst_chn.chn0.size = lcd_height * lcd_width * 2;
	
	render_in.img_height = lcd_height;
	render_in.img_width = lcd_width;

	for(i = 0 ; i < init_param.frame_num; i++)
	{
	    UILAYER_SwitchDoubleMem( &lcd_dev_info, TRUE );
	    
	    render_in.index = i;

        SE_Render( handle, &render_in, &render_out);

        copy_param.rect = update_rect;
        copybuf_info.buf_ptr = render_in.dst_chn.chn0.ptr;
        copybuf_info.data_type = DATA_TYPE_RGB565;
        copybuf_info.width = lcd_width;
        copybuf_info.height = lcd_height;
        UILAYER_CopyBufferToLayer(&lcd_dev_info, &copy_param, &copybuf_info);

        if ( 1 == i )//第一帧数据刷出来以后再点亮屏幕
        {
            GPIO_SetLcdBackLight(TRUE);
        }        
        if ( i < init_param.frame_num - 1 )
        {
            UILAYER_SetDirectDraw(TRUE);
        }

        GUILCD_Invalidate( GUI_MAIN_LCD_ID, 0 );
        SCI_SLEEP(1);
	}
    UILAYER_RELEASELAYER(&lcd_dev_info);/*lint !e506 !e774*/
    SCI_FREE(render_in.dst_chn.chn0.ptr);
    SCI_FREE(render_in.src_chn.chn0.ptr);
    SE_Destory(handle);
    UILAYER_SetBltLayer( old_array, old_count );//将旧的叠加序列设回
    return TRUE;
}
#endif
