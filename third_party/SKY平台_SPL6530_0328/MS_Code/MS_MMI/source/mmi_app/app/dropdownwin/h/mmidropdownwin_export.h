/*****************************************************************************
** File Name:      mmidropdownwin_export.h                                          *
** Author:                                                                   *
** Date:           05/29/2011                                                *
** Copyright:      2011 Spreadtrum, Incoporated. All Rights Reserved.        *
** Description:    This file is used to describe dropdown win                *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 07/2011        Paul.Huang      Create
******************************************************************************/

#ifndef _MMIDROPDOWNWIN_EXPORT_H_
#define _MMIDROPDOWNWIN_EXPORT_H_

#ifdef PDA_UI_DROPDOWN_WIN
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "mmk_type.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
//#define MMIDROPDOWN_SCROLLKEY_HEIGHT  32//20 //滑动栏的高度
#define MMIDROPDOWN_SLIDE_SPEED  60//50 //滑动时的速度
#define MMIDROPDOWN_INIT_SPEED  8 //判定滑动时的速度
#define MMIDROPDOWN_TPMOVE_REDRAW_TIME      20//30      //为支持TP连续移动而加的timer 
#define MMIDROPDOWN_UPDATE_REDRAW_TIME      1000    //定时刷新的timer
#define MMIDROPDOWN_STATUSBAR_ANIM_FRAME    5    //状态栏转场动画帧数

#define MMIDROPDOWN_SHORTCUT_NUM    4   //shortcut数目

typedef BOOLEAN(*MMIDROPDOWN_NOTIFY_CALLBACK)(void);

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/
typedef enum
{
    SLIDE_DIR_NONE = 0,
    SLIDE_DIR_DOWN,
    SLIDE_DIR_UP,
    SLIDE_DIR_MAX
}SLIDE_DIR_E;

typedef enum
{
    DROPDOWN_STATE_NONE = 0,
    DROPDOWN_STATE_OPEN,
    DROPDOWN_STATE_CLOSE,
    DROPDOWN_STATE_MAX
}DROPDOWN_STATE_E;

typedef enum
{
    DROPDOWN_PAGE_NOTIFY = 0,   //通知
    DROPDOWN_PAGE_RUNNING,      //正在运行
    DROPDOWN_PAGE_MAX
}DROPDOWN_PAGE_E;

typedef struct _MMIDROPDOWN_T
{
    MMI_HANDLE_T    parent_win;
    DROPDOWN_STATE_E    state;
    DROPDOWN_PAGE_E  cur_page;
    BOOLEAN    auto_slide;
    int32    top;
    int32    bottom;
    uint16    height;
    uint16    width;
    GUI_RECT_T    scrollkey_rect;
    // 滑动信息
    uint8    tp_move_timer_id;				// 为支持TP连续移动而加的timer    
    uint8    tp_redraw_timer_id;				// 为支持TP连续移动而加的timer    
    uint8    update_redraw_timer_id;				// 定时刷新的timer    
    MMK_TP_SLIDE_E    slide_state;                            //current slide state
    GUI_POINT_T    slide_pre_point;        // Slide时候使用
    SLIDE_DIR_E    slide_dir;
    BOOLEAN    is_tp_down;          // 是否tpdown
    GUI_LCD_DEV_INFO    dropdown_layer;
    GUI_LCD_DEV_INFO    statusbar_layer;
    GUI_RECT_T    network_rect;
} MMIDROPDOWN_T;

#ifdef WRE_SUPPORT //add for wre_background by heng.xiao
#define WRE_APP_IN_DROPDOWN_WIN
#define WRE_APP_RUN_MAX_NUM			50
#endif

 typedef enum
 {
     /*
#ifdef WIFI_SUPPORT
     DROPDOWN_SHORTCUT_WIFI,
#endif         
#ifdef BLUETOOTH_SUPPORT
     DROPDOWN_SHORTCUT_BT,
#endif
     DROPDOWN_SHORTCUT_FLYMODE,
     DROPDOWN_SHORTCUT_SILENT,
     DROPDOWN_SHORTCUT_BACKLIGHT,  
     */
#ifdef WIFI_SUPPORT
     DROPDOWN_SHORTCUT_WIFI,
#endif
#ifdef BLUETOOTH_SUPPORT
     DROPDOWN_SHORTCUT_BT,
#endif
     DROPDOWN_SHORTCUT_BACKLIGHT,
     DROPDOWN_SHORTCUT_SILENT,

     DROPDOWN_SHORTCUT_SAVEMODE,
     DROPDOWN_SHORTCUT_FLYMODE,
     DROPDOWN_SHORTCUT_SCREENLOCK,     
     DROPDOWN_SHORTCUT_DATACONNECT,

     DROPDOWN_SHORTCUT_MAX
 }MMIDROPDOWN_SHORTCUT_E;

typedef struct
{    
    MMIDROPDOWN_SHORTCUT_E 	item_id;
    MMI_IMAGE_ID_T   				open_image_id;  
    MMI_IMAGE_ID_T			         close_image_id;
    GUI_RECT_T	     				rect;
    GUI_RECT_T	     				icon_rect;
    GUI_RECT_T	     				bar_rect;
    BOOLEAN         					is_open;
    BOOLEAN         					is_wait;
    void(*item_handle_func)(void);
    void(*get_state_func)(void);
}MMIDROPDOWN_SHORTCUT_ITEM_T;

typedef struct
{    
    GUI_RECT_T rect;
    MMI_IMAGE_ID_T bg_image_id;
    MMI_IMAGE_ID_T bar_open_image_id;
    MMI_IMAGE_ID_T bar_close_image_id;
    MMI_IMAGE_ID_T split_line_image_id;
    GUI_RECT_T split_rect[DROPDOWN_SHORTCUT_MAX-1];
    GUI_RECT_T    button_notify_rect;
    GUI_RECT_T    button_split_rect; 
    GUI_RECT_T    button_running_rect; 
    GUI_RECT_T    button_bg_rect;
    MMIDROPDOWN_SHORTCUT_ITEM_T item[DROPDOWN_SHORTCUT_MAX];
}MMIDROPDOWN_SHORTCUT_T;

//LOCAL MMIDROPDOWN_SHORTCUT_ITEM_T s_shortcut_item_t[DROPDOWN_SHORTCUT_MAX] = {0};

//notify
typedef enum
{
    /*
    DROPDOWN_NOTIFY_MISS_CALL= 0,
    DROPDOWN_NOTIFY_NEW_MESSAGE,
    DROPDOWN_NOTIFY_MESSAGE_FULL,
#ifdef MMI_AUDIO_PLAYER_SUPPORT
    DROPDOWN_NOTIFY_PLAYER,
#endif
#ifdef FM_SUPPORT
    DROPDOWN_NOTIFY_FM,
#endif
#ifndef JAVA_SUPPORT_NONE
    DROPDOWN_NOTIFY_JAVA,
#endif
#ifdef QQ_SUPPORT
    DROPDOWN_NOTIFY_QQ,
#endif
    */
    //notify
    DROPDOWN_NOTIFY_MISS_CALL= 0,
    DROPDOWN_NOTIFY_NEW_MESSAGE,
#ifdef MMISMS_CHATMODE_ONLY_SUPPORT
    DROPDOWN_NOTIFY_NEW_PUSH,
#endif
    DROPDOWN_NOTIFY_MESSAGE_FULL,
#if defined(MMI_SMSCB_SUPPORT)
    DROPDOWN_NOTIFY_NEW_SMSCB,
#endif
    DROPDOWN_NOTIFY_STK_IDLE_TEXT,

#ifdef QQ_SUPPORT
    DROPDOWN_NOTIFY_QQ,
#endif
#ifdef ASP_SUPPORT
    DROPDOWN_NOTIFY_NEW_ASP,  //新留言
#endif
    DROPDOWN_NOTIFY_USB_DEBUG, //USB调试
#ifdef MMIEMAIL_SUPPORT
    DROPDOWN_NOTIFY_NEW_EMAIL,
#endif

    //running
#ifdef MMI_AUDIO_PLAYER_SUPPORT
    DROPDOWN_RUNNING_PLAYER,
#endif
#ifdef FM_SUPPORT
    DROPDOWN_RUNNING_FM,
#endif
    DROPDOWN_RUNNING_USB_USING, //USB已连接
    DROPDOWN_RUNNING_CALLING,
#ifdef COUNTEDTIME_SUPPORT
    DROPDOWN_RUNNING_COUNTEDTIME,
#endif
#ifndef JAVA_SUPPORT_NONE
    DROPDOWN_RUNNING_JAVA, //cr00115796 renwei modify
#endif

#ifdef MCARE_V31_SUPPORT
    DROPDOWN_NOTIFY_MCARE_QQ,
    DROPDOWN_NOTIFY_MCARE_QC,
#endif

#ifdef MRAPP_SUPPORT
	DROPDOWN_RUNNING_MRAPP,
#ifdef __MMI_SKYQQ__
	DROPDOWN_RUNNING_SKYQQ,
#endif
#endif

#ifdef BLUETOOTH_SUPPORT
    DROPDOWN_RUNNING_BT_TRANSFERFILE,//蓝牙正在传输文件
#endif
#ifdef WRE_SUPPORT	//WRE_APP_IN_DROPDOWN_WIN	//add for wre_background by heng.xiao
    DROPDOWN_RUNNING_WRE_APP_START,		//david li, 2012-01-06, add wre app start ID into drop-down win list
    DROPDOWN_RUNNING_WRE_APP_END = DROPDOWN_RUNNING_WRE_APP_START + WRE_APP_RUN_MAX_NUM,
#endif
    DROPDOWN_NOTIFY_MAX
}DROPDOWN_NOTIFY_E;

typedef struct
{    
    DROPDOWN_NOTIFY_E 	notify_type;
    BOOLEAN    is_exist;
    uint32    notify_time;
}MMIDROPDOWN_NOTIFY_ITEM_T;

typedef struct
{
    MMI_STRING_T   notify_title;
    MMI_STRING_T   notify_string;
    MMI_STRING_T   notify_time;
    MMI_IMAGE_ID_T    notify_image;
    MMI_IMAGE_ID_T    action_image;
}MMIDROPDOWN_NOTIFY_DETAIL_T;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
 /*****************************************************************************/
//  Description : Init dropdownwin module
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIDROPDOWNWIN_InitModule(void);

/*****************************************************************************/
//  Description : Init dropdown NV
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Init(void);

/*****************************************************************************/
//  Description : Set dropdown win slide state
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_SetSlideState(void);

/*****************************************************************************/
//  Description : Create dropdown win
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 只提供给状态栏下拉创建过程调用
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Create(void);

/*****************************************************************************/
//  Description : Add dropdown notify record
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_AddNotifyRecord(
                       DROPDOWN_NOTIFY_E   notify_type
                       );

/*****************************************************************************/
//  Description : Del dropdown notify record
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_DelNotifyRecord(DROPDOWN_NOTIFY_E  notify_type);

/*****************************************************************************/
//  Description : 更新下拉窗口
//  Global resource dependence : none
//  Author: paul.huang
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_UpdateMainWin(void);

/*****************************************************************************/
//  Description : Get Bar Image
//  Global resource dependence : none
//  Author: Paul.huang
//  Note: 
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MMIDROPDOWNWIN_GetBarImage(void);

/*****************************************************************************/
//  Description : 打开下拉窗口
//  Global resource dependence : none
//  Author: xiaoming.ren
/*****************************************************************************/
//PUBLIC void MMIDROPDOWNWIN_OpenMainWin(void);

/*****************************************************************************/
//  Description : 打开下拉窗口
//  Global resource dependence : none
//  Author: xiaoming.ren
//  Note: 提供给外部调用创建
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Open(void);

/*****************************************************************************/
//  Description : 关闭下拉窗口
//  Global resource dependence : none
//  Author: paul.huang
/*****************************************************************************/
PUBLIC void MMIDROPDOWNWIN_Close(
                                 BOOLEAN is_show_anim
                                 );
/**---------------------------------------------------------------------------*
 **                    Old Interface  Macro Declaration    				   *
 **---------------------------------------------------------------------------*/
/*****************************************************************************/
//  如下宏定义用以实现对外接口更名前后的兼容
/*****************************************************************************/
#ifdef MMI_FUNC_COMPATIBLE_SUPPORT

#define MMIDROPDOWNWIN_InitModule				        MMIAPIDROPDOWNWIN_InitModule

#endif //MMI_FUNC_COMPATIBLE_SUPPORT

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif //PDA_UI_DROPDOWN_WIN

#endif
