/*****************************************************************************
** File Name:      mainmenu_win.c                                            *
** Author:                                                                   *
** Date:           12/2003                                                   *
** Copyright:      2003 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe call log                    *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 05/2009       Jassmine           Creat
******************************************************************************/

#define MAINMENU_WIN_C

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
//#include "std_header.h"
#include "mmi_app_menu_trc.h"
#include "window_parse.h"
#include "guitext.h"
#include "mmiidle_export.h"
#include "mmiidle_statusbar.h"
#include "mmk_tp.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmiset_mainmenu_export.h"
#include "mmistk_export.h"
#include "mmisms_export.h"
#include "mmicl_export.h"
#include "mmidc_export.h"
#include "mmimenu_export.h"
#include "mmipicview_export.h"
#include "mmialarm_export.h"
//#include "mmigame.h"
#include "mmigame_export.h"
//#include "mmiebook.h"
#include "mmiebook_export.h"
#include "mmicalendar_export.h"
#include "mmirecord_export.h"
//#include "mmiset_wintab.h"
#include "mmienvset_export.h"
#include "mmiconnection_export.h"
#include "mmifm_export.h"
#include "mmikur.h"
#include "mmimtv_export.h"
#include "mmiatv_export.h"
#ifdef MMI_PDA_SUPPORT
#include "mmi_timer_export.h"
#else
#include "mmicountedtime_export.h"
#include "mmistopwatch_export.h"
#endif
#include "mmiphonesearch_export.h"
#include "mmigame_menutable.h"
//#include "mmigame_link_internal.h"
#include "mmigame_link_export.h"
//#include "mmigame_boxman_internal.h"
#include "mmigame_boxman_export.h"
//#include "mmidyna_export.h"
#include "mmimenu_data.h"
#include "mmi_appmsg.h"
#include "mmi_position.h"
#include "mmidisplay_data.h"
#include "mmitheme_special_func.h"
#include "mmicalc_export.h"
#include "mmiwclk_export.h"
#include "mmiacc_id.h"
#include "mmipub.h"
#include "mmisd_export.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "mmiudisk_export.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef DL_SUPPORT
#include "mmidl_export.h"
#endif
#include "mmiphone_export.h"
#ifdef MMI_ENABLE_DCD
#include "mmidcd_export.h"
#endif
#ifdef CMCC_UI_STYLE
#include "mmics_export.h"
#endif
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
#include "mmipim_export.h"
#include "mmiunitconversion_export.h"

#ifdef QQ_SUPPORT
#include "mmiqq_export.h"
#endif
#ifdef ASP_SUPPORT
#include "mmiasp_export.h"
#endif
#ifdef SEARCH_SUPPORT
#include "mmisearch_export.h"
#endif

#include "mmiconnection_menutable.h"

#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif

#ifdef PUSH_EMAIL_SUPPORT   //Added by yuanl  2010.5.10
#include "mmimail_export.h"
#endif /* PUSH_EMAIL_SUPPORT  */

#ifdef MOBILE_VIDEO_SUPPORT
#include "mmimv_export.h"
#endif

#ifdef  CMCC_UI_STYLE 
//ONLY FOR uplmn SETTING
#include "mmiengtd_win.h"   
#endif

#include "guistatusbar.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#endif

#ifdef MMI_AZAN_SUPPORT
#include "mmiazan_export.h"
#endif
#ifdef MMIEMAIL_SUPPORT
#include "mmiemail_export.h"
#endif
#if defined(DYNAMIC_MAINMENU_SUPPORT)
#include "mmimenu_synchronize.h"
#endif
#ifdef WEATHER_SUPPORT
#include "mmiweather_export.h"
#endif
#ifdef MCARE_V31_SUPPORT
#include "McfInterface.h"
#include "Mcare_Interface.h"
#endif
#ifdef MMI_VIDEOWALLPAPER_SUPPORT
#include "mmiset_videowallpaper.h"
#endif

#ifdef MMI_TASK_MANAGER
#include "mmi_applet_manager.h"
#endif

#ifdef WRE_SUPPORT
#include "mmiwre_export.h"
#include "mmiwre.h"
#endif

#ifdef MMI_MEMO_SUPPORT
#include "mmimemo_export.h"
#endif

#ifdef MMI_KING_MOVIE_SUPPORT
#include "mmikm_export.h"
#endif
#ifdef SXH_APPLE_SUPPORT	
#include "Apple_export.h"
#endif

#if (defined MMI_GRID_IDLE_SUPPORT) || (defined QBTHEME_SUPPORT)
#include "Mmiwidget_export.h"
#endif

//#ifdef MMI_PDA_SUPPORT
#include "Mmiset_display.h"
//#endif
#include "guictrl_api.h"
//#include "mmiset_lock_effect.h"  //xiyuan edit for  pclint
#include "mmiset_lock_effect_export.h"
#if (defined QBTHEME_SUPPORT)
#include "mmiqbtheme_export.h"
#include "mmiqbtheme_idleexport.h"
#endif

#ifdef IKEYBACKUP_SUPPORT
#include "mmiikeybackup_api.h"
#endif
#include "mmi_menutable.h"
#ifdef MET_MEX_SUPPORT
#include "met_mex_export.h"
#endif

#ifdef MMI_CALL_MINIMIZE_SUPPORT  
#include "mmicc_export.h"
#endif
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define MSG_MAINMENU_UPDATE (MSG_USER_START + 0x01)//异步打开应用消息
#define DYNAMIC_MAINMENU_DEFAULT_LIST_INDEX 0xffff
/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/
typedef enum
{
    BROWSER_URL_TYPE_DIANPING,   //大众点评网
    BROWSER_URL_TYPE_KAIXIN,     //开心网
    BROWSER_URL_TYPE_CTRIP,      //携程网  //
    BROWSER_URL_TYPE_BAIDU,      //百度         
    BROWSER_URL_TYPE_SINA_BOLG,  //新浪微博
    BROWSER_URL_TYPE_TAOBAO,     //淘宝
    BROWSER_URL_TYPE_YOUKU,      //优酷    //
    BROWSER_URL_TYPE_RENREN,     //人人网
    BROWSER_URL_TYPE_QIDIAN,     //起点中文网
    BROWSER_URL_TYPE_SINA,       //新浪网  //
    BROWSER_URL_TYPE_SOSO,       //SOSO搜搜网
    BROWSER_URL_TYPE_MOBILESOHU, //手机搜狐网
	BROWSER_URL_TYPE_COOLSTATION,//酷站导航
	BROWSER_URL_TYPE_AMUSEBOX,   //娱乐百宝箱 
    BROWSER_URL_TYPE_COLORWORLD, //彩世界
	BROWSER_URL_TYPE_TUDOU,      //土豆网
	BROWSER_URL_TYPE_PALMREAD,   //掌阅书城
	BROWSER_URL_TYPE_ZHIFUBAO,   //支付宝
    BROWSER_URL_TYPE_MAX
}MAINMENU_BROWSER_URL_TYPE_E;

LOCAL const char* const s_mainmenu_browser_url[BROWSER_URL_TYPE_MAX] =
{
    "http://wap.dianping.com",
    "http://wap.kaixin001.com",
    "http://m.ctrip.com",
    "http://m.baidu.com/?from=1214a",//the url is from baidu
    "http://3g.sina.com.cn/3g/pro/index.php?sa=t254d1293v446&wm=9709",//sina wei bo
    "http://wap.taobao.com",
    "http://3g.youku.com",
    "http://wap.renren.com/",
    "http://wap.qidian.com/r/U7nYRr",
    "http://3g.sina.com.cn/?wm=9815",//the url is from sina
    "http://fwd.wap.soso.com:8080/forward.jsp?bid=421&cd_f=70373",//the url is from soso
	"http://wap.sohu.com/?v=3&fr=zhanxun_zm1_0923",
	"http://wap.51wre.com",
	"http://wap.sohu.com/ent/?fr=zx_ylzm_0923",
	"http://zx.m.caishijie.com/wap/jsp/index.jsp?sid=12074000000",
	"http://m.tudou.com/index.do?cp=1074",
	"http://kx.z3.cn/yx/zqqd.php?c=zx2001",
	"http://wap.alipay.com"
};

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
// Description : main menu handle window message
// Global resource dependence : 
// Author:jassmine.meng
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainMenuWinMsg(
                                        MMI_WIN_ID_T         win_id, 
                                        MMI_MESSAGE_ID_E     msg_id, 
                                        DPARAM               param
                                        );

/*****************************************************************************/
// Description : icon main second menu handle window message
// Global resource dependence : 
// Author:jassmine.meng
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainSecondMenuWinMsg(
                                              MMI_WIN_ID_T         win_id, 
                                              MMI_MESSAGE_ID_E     msg_id, 
                                              DPARAM               param
                                              );

/*****************************************************************************/
// Description : 处理二级菜单静态菜单消息
// Global resource dependence : 
// Author:jin.wang
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSecMenuStaticMsg(
    MMI_WIN_ID_T         win_id, 
    MMI_MESSAGE_ID_E     msg_id, 
    MMI_MENU_ID_T        menu_id
);

/*****************************************************************************/
// Description : 二级菜单list控件处理函数
// Global resource dependence : 
// Author:jin.wang
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainSecondMenuListWinMsg(
                                              MMI_WIN_ID_T         win_id, 
                                              MMI_MESSAGE_ID_E     msg_id, 
                                              DPARAM               param
                                              );
#ifdef MMITHEME_MAINMENU_ARRANGE

/*****************************************************************************/
// Description : to handle the SMS menu option window message
// Global resource dependence : 
// Author:liming.deng
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleMainMenuOptWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );

/*****************************************************************************/
// Description : to handle the SMS menu option window message
// Global resource dependence : 
// Author:liming.deng
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleMainMenuMoveOptWinMsg(
                                                MMI_WIN_ID_T        win_id, 
                                                MMI_MESSAGE_ID_E    msg_id, 
                                                DPARAM                param
                                                );

/*****************************************************************************/
// FUNCTION:     HandleMainMenuHelpWinMsg
// Description :   handle help win msg
// Global resource dependence : 
// Author: xiaoqing.lu
// Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainMenuHelpWinMsg( 
                                            MMI_WIN_ID_T win_id, 
                                            MMI_MESSAGE_ID_E msg_id, 
                                            DPARAM param
                                            );
#endif
                                            
/*****************************************************************************/
// FUNCTION:     HandleBrowerUrlMenu
// Description :   处理主菜单中通过browser 访问url的菜单项
// Global resource dependence : 
// Author: fen.xie
// Note:   
/*****************************************************************************/
LOCAL void HandleBrowerUrlMenu(MAINMENU_BROWSER_URL_TYPE_E browser_url_type);

/*****************************************************************************/
// Description : 创建list
// Global resource dependence : 
// Author:jinwang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN SecMenuList_Create(MMI_MENU_GROUP_ID_T group_id);

/*****************************************************************************/
// Description : 给list控件传入资源
// Global resource dependence : 
// Author:jinwang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN SecMenuList_SetItem(
    MMI_CTRL_ID_T ctrl_id,         //in:control id
    uint16 item_index,     //in:item索引
    GUIMENU_GROUP_T *sc_menu_group_ptr
);
/*****************************************************************************/
// Description : 获取list控件被选中index
// Global resource dependence : 
// Author:jinwang
// Note:返回list控件被选中的list index
/*****************************************************************************/
LOCAL uint32 GetSecondMenuListIndex(MMI_MESSAGE_ID_E msg_id);

#if defined(DYNAMIC_MAINMENU_SUPPORT)
/*****************************************************************************/
// Description : 给list控件传入图片信息
// Global resource dependence : 
// Author:jinwang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN SecMenuList_SetDyPic(
    uint16 item_index,  //控件索引
    uint16 item_content_index,//内容索引
    MMI_CTRL_ID_T ctrl_id,//in:control id
    GUIMENU_GROUP_T *sc_menu_group_ptr,
    MMI_WIN_ID_T win_id
);
#endif

/**--------------------------------------------------------------------------*
 **                         CONSTANT VARIABLES                               *
 **--------------------------------------------------------------------------*/
#ifdef OPTION_PAGE_MAIN_MENU_SUPPORT
//option page main menu window
WINDOW_TABLE(MAINMENU_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU, MAINMENUWIN_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif
//icon main menu window 
WINDOW_TABLE(MAINMENU_ICON_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_WIN_ID),
	WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_INIT),
#if defined MAINLCD_SIZE_128X64 
    
#else        
    WIN_STATUSBAR,
    //WIN_HIDE_STATUS,
#endif 
    CREATE_MENU_CTRL(MENU_MAINMENU_ICON, MAINMENUWIN_MENU_CTRL_ID), 
    //WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),

    END_WIN
};

//others main menu window
WINDOW_TABLE(MAINMENU_OTHERS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_OTHERS_WIN_ID),
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(MENU_OTHERS, MAINMENUWIN_SECOND_MENU_CTRL_ID),
	WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

//others main menu window
WINDOW_TABLE(MAINMENU_SLIDE_OTHERS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_OTHERS_WIN_ID),
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(SLIDE_MENU_OTHERS, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

// icon main menu window 
WINDOW_TABLE(MAINMENU_TITLE_ICON_WIN_TAB) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC((uint32)HandleMainMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_TITLE_ICON, MAINMENUWIN_MENU_CTRL_ID),
// WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

#ifdef QBTHEME_SUPPORT
// 千变主题menu窗口
WINDOW_TABLE(MAINMENU_QBTHEME_PAGE_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    WIN_MOVE_STYLE(MOVE_FORBIDDEN),
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_0),
    CREATE_MENU_CTRL(MENU_MAINMENU_QBTHEME, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif

// icon main menu window with bar
// WINDOW_TABLE(MAINMENU_ISTYLE_WIN_TAB) =
// {
//     WIN_FUNC((uint32)HandleMainMenuWinMsg),
//     WIN_ID(MAIN_MAINMENU_WIN_ID),
//     WIN_STATUSBAR,
//     CREATE_MENU_CTRL(MENU_MAINMENU_ISTYLE, MAINMENUWIN_MENU_CTRL_ID),
//     WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
//     END_WIN
// };

#ifdef CUBE_MAIN_MENU_SUPPORT
// cube main menu window 
WINDOW_TABLE(MAINMENU_CUBE_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_MOVE_STYLE(MOVE_FORBIDDEN),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_CUBE, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
// GO main menu window 
WINDOW_TABLE(MAINMENU_GO_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    //WIN_STATUSBAR,
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_INIT),
    CREATE_MENU_CTRL(MENU_MAINMENU_GO, MAINMENUWIN_MENU_CTRL_ID),
   // WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#else
#ifdef SLIDE_MAIN_MENU_SUPPORT 
// icon main menu window 
WINDOW_TABLE(MAINMENU_SLIDE_PAGE_WIN_TAB) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC((uint32)HandleMainMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_SLIDE_PAGE, MAINMENUWIN_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif
#ifdef TRAPEZOID_MAIN_MENU_SUPPORT
// trapezoid main menu window 
WINDOW_TABLE(MAINMENU_TRAPEZOID_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_TRAPEZOID, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif

#ifdef BINARY_STAR_MAIN_MENU_SUPPORT

// binary star main menu window 
WINDOW_TABLE(MAINMENU_BINARY_STAR_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_BINARY_STAR, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif
#ifdef SHUTTER_MAIN_MENU_SUPPORT
// shutter main menu window 
WINDOW_TABLE(MAINMENU_SHUTTER_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_SHUTTER, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif

#ifdef CHARIOT_MAIN_MENU_SUPPORT
// chariot main menu window 
WINDOW_TABLE(MAINMENU_CHARIOT_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_CHARIOT, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif


#ifdef SPHERE_MAIN_MENU_SUPPORT
// sphere main menu window 
WINDOW_TABLE(MAINMENU_SPHERE_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_SPHERE, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif

#ifdef CYLINDER_MAIN_MENU_SUPPORT
// cylinder main menu window 
WINDOW_TABLE(MAINMENU_CYLINDER_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    WIN_STATUSBAR,
    CREATE_MENU_CTRL(MENU_MAINMENU_CYLINDER, MAINMENUWIN_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif
#endif
#ifdef MENU_CRYSTALCUBE_SUPPORT
WINDOW_TABLE(MAINMENU_CRYSTALCUBE_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    CREATE_MENU_CTRL(MENU_MAINMENU_CRYSTALCUBE, MAINMENUWIN_MENU_CTRL_ID),
    //WIN_TITLE(TXT_NULL),
    END_WIN
};
#endif

#ifdef MENU_CYCLONE_SUPPORT
WINDOW_TABLE(MAINMENU_CYCLONE_WIN_TAB) =
{
    WIN_FUNC((uint32)HandleMainMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_WIN_ID),
    CREATE_MENU_CTRL(MENU_MAINMENU_CYCLONE, MAINMENUWIN_MENU_CTRL_ID),
    END_WIN
};
#endif

//icon player menu window 
WINDOW_TABLE(MAINMENU_ICON_PLAYER_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_PLAYER_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(MENU_ICON_PLAYER, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

//icon tools main menu window 
WINDOW_TABLE(MAINMENU_ICON_TOOLS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_TOOLS_WIN_ID),
#ifdef MAINLCD_SIZE_128X64    

#else
    WIN_STATUSBAR,    
    WIN_TITLE(TXT_NULL),
#endif    
    CREATE_MENU_CTRL(MENU_ICON_TOOLS, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    WIN_TIPS,
    END_WIN
};

#ifdef CMCC_UI_STYLE
/*移动服务*/
WINDOW_TABLE(MAINMENU_ICON_SERVICE_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_CMCC_SERVICE_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(MENU_ICON_SERVICE, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    WIN_TIPS,
    END_WIN
};

/*移动梦网*/
WINDOW_TABLE(MAINMENU_ICON_MONERNET_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_CMCC_MONERNET_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
    //CREATE_MENU_CTRL(MENU_ICON_MONERNET, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    WIN_TIPS,
    END_WIN
};

/*我的收藏*/
WINDOW_TABLE(MAINMENU_ICON_MYFAVORITE_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_MY_FAVORITE_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(MENU_ICON_MY_FAVORITE, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    WIN_TIPS,
    END_WIN
};

/* 应用*/
WINDOW_TABLE(MAINMENU_ICON_APPS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),    
    WIN_ID(MAIN_MAINMENU_APP_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(MENU_ICON_APPS, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    WIN_TIPS,
    END_WIN
};

#endif //CMCC_UI_STYLE
//icon pastime main menu window 
WINDOW_TABLE(MAINMENU_ICON_PASTIME_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuListWinMsg),    
    WIN_ID(MAIN_MAINMENU_PASTIME_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_MAINMENU_ICON_PASTIME),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MAINMENUWIN_SECOND_LIST_CTRL_ID),
    //CREATE_MENU_CTRL(MENU_ICON_PASTIME, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

//icon settings main menu window 
WINDOW_TABLE(MAINMENU_ICON_SETTINGS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_SETTINGS_WIN_ID),
#ifdef MAINLCD_SIZE_128X64    

#else
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
#endif    
    CREATE_MENU_CTRL(MENU_SETTINGS_ICON, MAINMENUWIN_SECOND_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

//icon settings main menu window 
WINDOW_TABLE(MAINMENU_SLIDE_ICON_SETTINGS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleMainSecondMenuWinMsg),
    WIN_ID(MAIN_MAINMENU_SETTINGS_WIN_ID),
    WIN_STATUSBAR,
    WIN_TITLE(TXT_NULL),
    CREATE_MENU_CTRL(SLIDE_MENU_SETTINGS_ICON, MAINMENUWIN_SECOND_MENU_CTRL_ID),
 WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

#ifdef MMITHEME_MAINMENU_ARRANGE
WINDOW_TABLE( MAINMENU_OPTMENU_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)HandleMainMenuOptWinMsg),
    WIN_ID(MAIN_MAINMENU_OPT_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    CREATE_POPMENU_CTRL(MENU_MAINMENU_OPT,MAIN_MAINMENU_OPT_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MAINMENU_MOVEOPTMENU_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)HandleMainMenuMoveOptWinMsg),
    WIN_ID(MAIN_MAINMENU_MOVEOPT_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    CREATE_POPMENU_CTRL(MENU_MAINMENU_MOVE_OPT,MAIN_MAINMENU_MOVEOPT_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MAINMENU_HELP_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleMainMenuHelpWinMsg ),    
    WIN_ID( MAIN_MAINMENU_HELP_WIN_ID ),
    WIN_TITLE(TXT_HELP),
    CREATE_TEXT_CTRL(MAIN_MAINMENU_HELP_CTRL_ID),
    WIN_SOFTKEY(TXT_NULL,TXT_NULL,STXT_RETURN),
    END_WIN
}; 
#endif

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/*****************************************************************************/
// Description : creat main menu
// Global resource dependence : 
// Author:jassmine.meng
// Note: 
/*****************************************************************************/
PUBLIC void MMIAPIMENU_CreatMainMenu(void)
{
    GUIMENU_STYLE_E    menu_style = GUIMENU_STYLE_NULL;

#ifdef QBTHEME_SUPPORT
    MMISET_IDLE_STYLE_E idle_style = MMISET_IDLE_STYLE_MAX; 

    idle_style = MMIAPISET_GetIdleStyle();

    if (MMISET_IDLE_QBTHEME == idle_style)
    {
		MMI_HANDLE_T qbwin_handle = NULL;
        qbwin_handle = MMK_CreateWin((uint32*)MAINMENU_QBTHEME_PAGE_WIN_TAB, PNULL);
		GUIWIN_SetStbState(qbwin_handle, GUISTATUSBAR_STATE_USE_LAYER, TRUE);
        return;
    }
#endif

    menu_style = MMIAPISET_GetMainmenuStyle();
    
    switch (menu_style)
    {
    case GUIMENU_STYLE_ICON:
        //set special effect
        MMITHEME_SetSpecialEffectDirty(MMITHEME_SPECIAL_MAINMENU);    
        
        //creat
        MMK_CreateWin((uint32*)MAINMENU_ICON_WIN_TAB, PNULL);
        break;  
        
#ifdef OPTION_PAGE_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_OPTION_PAGE:
        MMK_CreateWin((uint32*)MAINMENU_WIN_TAB, PNULL);
        
        //主菜单进入时，设置选中当中的菜单项
        GUIMENU_SetFirstAndSelectedItem(0,
            MMIMENU_MAIN_OPTION_PAGE_DEFAULT_SEL,
            MAINMENUWIN_MENU_CTRL_ID);
        break;
#endif
        
    case GUIMENU_STYLE_TITLE_ICON:
        MMK_CreateWin((uint32*)MAINMENU_TITLE_ICON_WIN_TAB, PNULL);
        break;
        
#ifdef CUBE_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_CUBE:
        MMK_CreateWin((uint32*)MAINMENU_CUBE_WIN_TAB, PNULL);
        break;
#endif

#ifdef MENU_CRYSTALCUBE_SUPPORT
    case GUIMENU_STYLE_CRYSTAL_CUBE:
        GUILCD_SetLogicAngle( GUI_MAIN_LCD_ID, LCD_ANGLE_0 );
        MMK_CreateWin((uint32*)MAINMENU_CRYSTALCUBE_WIN_TAB, PNULL);
        break;
#endif

#ifdef MENU_CYCLONE_SUPPORT
    case GUIMENU_STYLE_CYCLONE:
         GUILCD_SetLogicAngle( GUI_MAIN_LCD_ID, LCD_ANGLE_0 );//jinx
        MMK_CreateWin((uint32*)MAINMENU_CYCLONE_WIN_TAB, PNULL);
        break;
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
    case GUIMENU_STYLE_GO:
        MMK_CreateWin((uint32*)MAINMENU_GO_WIN_TAB, PNULL);
        break;
#else
#ifdef SLIDE_MAIN_MENU_SUPPORT 
    case GUIMENU_STYLE_SLIDE_PAGE:
        MMK_CreateWin((uint32*)MAINMENU_SLIDE_PAGE_WIN_TAB, PNULL);
        break;
#endif
#ifdef TRAPEZOID_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_TRAPEZOID:
        MMK_CreateWin((uint32*)MAINMENU_TRAPEZOID_WIN_TAB, PNULL);
        break;
#endif

#ifdef SPHERE_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_SPHERE:
        MMK_CreateWin((uint32*)MAINMENU_SPHERE_WIN_TAB, PNULL);
        break;
#endif

#ifdef CYLINDER_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_CYLINDER:
        MMK_CreateWin((uint32*)MAINMENU_CYLINDER_WIN_TAB, PNULL);
        break;
#endif

#ifdef BINARY_STAR_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_BINARY_STAR:
         MMK_CreateWin((uint32*)MAINMENU_BINARY_STAR_WIN_TAB, PNULL);
        break;
#endif
#ifdef SHUTTER_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_SHUTTER:
         MMK_CreateWin((uint32*)MAINMENU_SHUTTER_WIN_TAB, PNULL);
        break;
#endif        

#ifdef CHARIOT_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_CHARIOT:
         MMK_CreateWin((uint32*)MAINMENU_CHARIOT_WIN_TAB, PNULL);
        break;
#endif  
#endif
    default:
        MMIAPISET_SetMainmenuStyle(GUIMENU_STYLE_ICON);
        //set special effect
        MMITHEME_SetSpecialEffectDirty(MMITHEME_SPECIAL_MAINMENU);    
        //creat
        MMK_CreateWin((uint32*)MAINMENU_ICON_WIN_TAB, PNULL);
        break;
    }
}

/*****************************************************************************/
// Description : close main menu
// Global resource dependence : 
// Author:
// Note: 
/*****************************************************************************/
PUBLIC void MMIAPIMENU_UpdateMainMenu(void)
{
 if (MMK_IsFocusWin(MAIN_MAINMENU_WIN_ID))
 {
  MMK_CloseWin(MAIN_MAINMENU_WIN_ID);
  MMIAPIMENU_CreatMainMenu();
 }
}

#if (defined MMI_GRID_IDLE_SUPPORT) || (defined QBTHEME_SUPPORT)
/*****************************************************************************/
// 	Description : handle main menu id, called by idle.
//	Global resource dependence : 
//  Author: hongbo.lan
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIMENU_HandleIdleMenuItem(MMI_MENU_GROUP_ID_T  group_id, MMI_MENU_ID_T menu_id)
{
    MMIMAINMENU_ITEM_INFO_T menu_item = {0};

    menu_item.group_id = group_id;
    menu_item.menu_id  = menu_id;

    HandleMainMenuWinMsg(VIRTUAL_WIN_ID, MSG_GRID_PEN_MENU_ITEM, (DPARAM)&menu_item); 
}  
#endif


/*****************************************************************************/
// 	Description : handle main menu win msg
//	Global resource dependence : 
//  Author: James.Zhang
//	Note: 
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIAPIMENU_HandleMainMenuWinMsg(
                                                    MMI_WIN_ID_T         win_id, 
                                                    MMI_MESSAGE_ID_E     msg_id, 
                                                    DPARAM               param
                                                    )
{
    return HandleMainMenuWinMsg( win_id, msg_id, param );
}  

/*****************************************************************************/
// Description : main menu handle window message
// Global resource dependence : 
// Author:jassmine.meng
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainMenuWinMsg(
                                        MMI_WIN_ID_T         win_id, 
                                        MMI_MESSAGE_ID_E     msg_id, 
                                        DPARAM               param
                                        )
{
    BOOLEAN                             is_menu_gray = TRUE;
    GUI_BG_T                            softkey_bg = {0};
    MMI_RESULT_E                        result = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T                       ctrl_id = MAINMENUWIN_MENU_CTRL_ID;
    MMI_MENU_ID_T                  menu_id = 0;
    MMI_MENU_GROUP_ID_T                 group_id = 0;
    uint16 stk_sim_id = 0;
    uint32 stk_sim_num = 0;
    MMI_STRING_T stk_str = {0};
    BOOLEAN is_exist_stk_name = FALSE;
#ifndef PDA_VIDEOPLAYER_WALLPAPER_SUPPORT_LOW_MEMORY
#ifdef MMI_VIDEOWALLPAPER_SUPPORT  
    MMISET_IDLE_STYLE_E     idle_style  = MMISET_IDLE_STYLE_COMMON;
    GUIMENU_STYLE_E     mainmenu_style  =   GUIMENU_STYLE_OPTION_PAGE;
#endif
#endif
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMITHEME_InitMenuItemCache();
#ifdef LOW_MEMORY_SUPPORT
    //    MMK_SetWinMoveStyle(win_id, MOVE_FORBIDDEN);        
#endif        
        //set status bar
        MAIN_SetAllStbAppCallbackValid(TRUE);
        MMK_SetAtvCtrl(win_id, ctrl_id);
        GUIWIN_SetStbBgImageId(win_id, IMAGE_IDLE_STATUS_BAR);
        GUIWIN_SetStbState(win_id, GUISTATUSBAR_STATE_EVENT, TRUE);
		GUIWIN_SetStbState(win_id, GUISTATUSBAR_STATE_USE_LAYER, TRUE);
		GUIWIN_SetStbBgIsDesplay(win_id,TRUE);

        //set softkey bg
        softkey_bg.bg_type = GUI_BG_IMG;
        softkey_bg.img_id  = IMAGE_THEME_BG;
        GUIWIN_SetSoftkeyCommonBg(win_id,&softkey_bg);
        break;
    case MSG_FULL_PAINT:
//        RemoveBgLayer();
        if (MMIAPISET_GetMultiSysSettingNum(PNULL, 0) > 1)
        {
            is_menu_gray = FALSE;
        }
        else
        {
            is_menu_gray = TRUE;
        }
        MMIAPICOM_EnableGrayed(win_id, MENU_SLIDE_TOOLS, ID_CONNECTION_GPRS, is_menu_gray);

        stk_sim_num = MMIAPISTK_GetStkCardNum(&stk_sim_id, 1);
        if (stk_sim_num > 0)
        {
            is_menu_gray = FALSE;
        }
        else
        {
            is_menu_gray = TRUE;
        }
        if (1 == stk_sim_num)
        {
            is_exist_stk_name = MMIAPISTK_GetStkName((MN_DUAL_SYS_E)stk_sim_id, &stk_str);
        }

        if (!is_exist_stk_name)
        {
            MMI_GetLabelTextByLang(TXT_COMMON_MAIN_STK, &stk_str);
        }

        //STK Menu
        {
#ifdef MAINLCD_SIZE_320X480
            GUIMENU_SetStaticItem(MAINMENUWIN_MENU_CTRL_ID,
                                                    MENU_SLIDE_TOOLS,
                                                    ID_COMMUNICATION_STK,
                                                    &stk_str,
                                                    IMAGE_NULL
                                                    );
#else
            GUIMENU_SetStaticItem(MAINMENUWIN_MENU_CTRL_ID,
                                                    MENU_SLIDE_COMMUNICATION,
                                                    ID_COMMUNICATION_STK,
                                                    &stk_str,
                                                    IMAGE_NULL
                                                    );
#endif
            GUIMENU_SetStaticItem(MAINMENUWIN_MENU_CTRL_ID,
                                                    MENU_COMMUNICATION,
                                                    ID_COMMUNICATION_STK,
                                                    &stk_str,
                                                    IMAGE_NULL
                                                    );

#ifdef MAINLCD_SIZE_320X480
            MMIAPICOM_EnableGrayed(win_id, MENU_SLIDE_TOOLS, ID_COMMUNICATION_STK, is_menu_gray);
#else
            MMIAPICOM_EnableGrayed(win_id, MENU_SLIDE_COMMUNICATION, ID_COMMUNICATION_STK, is_menu_gray);
#endif
            MMIAPICOM_EnableGrayed(win_id, MENU_COMMUNICATION, ID_COMMUNICATION_STK, is_menu_gray);

            GUIMENU_GetId(ctrl_id, &group_id, &menu_id);

            GUIMENU_SetStaticItem(MAINMENUWIN_MENU_CTRL_ID,
                                                    group_id,
                                                    ID_COMMUNICATION_STK,
                                                    &stk_str,
                                                    IMAGE_NULL
                                                    );
            
            MMIAPICOM_EnableGrayed(win_id, group_id, ID_COMMUNICATION_STK, is_menu_gray);
        }
        break;

    case MSG_GET_FOCUS:
#ifdef MRAPP_SUPPORT
        GUIWIN_UpdateStb();
#endif
        MMITHEME_InitMenuItemCache();
        MAIN_SetAllStbAppCallbackValid(TRUE);
        break;

#if (defined MMI_GRID_IDLE_SUPPORT) || (defined QBTHEME_SUPPORT)
    case MSG_NOTIFY_GRID_SHORTCUT2IDLE:
    case MSG_NOTIFY_QBTHEME_SHORTCUT2IDLE:
        {
            MMIMAINMENU_ITEM_INFO_T item_info = {0};
            GUI_POINT_T  current_point = {0};
            LCD_ANGLE_E  current_angle = LCD_ANGLE_0;
            LCD_ANGLE_E  idlewin_angle = LCD_ANGLE_0;
            int16 temp = 0;
            uint16  lcd_width = 0; 
            uint16  lcd_height= 0;                    

            current_point = *(GUI_POINT_T*)param;
            MMK_GetWinAngle(MAIN_IDLE_WIN_ID,&idlewin_angle);
            MMK_GetWinAngle(MAIN_MAINMENU_WIN_ID,&current_angle);
            GUILCD_GetWidthHeightByAngle(MAIN_LCD_ID,idlewin_angle,&lcd_width,&lcd_height);

            switch ((uint16)current_angle - (uint16)idlewin_angle)/* lint !e656 */
            {
            case LCD_ANGLE_90:
                temp  = current_point.x;
                current_point.x = (int16)(lcd_width - current_point.y);
                current_point.y =  (int16)temp;
                break;
                
            case LCD_ANGLE_180:
                current_point.x = (int16)(lcd_width - current_point.x);
                current_point.y = (int16)(lcd_height - current_point.y);
                break;
                
            case LCD_ANGLE_270:
                temp  = current_point.y;
                current_point.y = (int16)(lcd_height - current_point.x);
                current_point.x = (int16)(temp);
                break;
                
            default:
                break;
            }

            GUIMENU_GetId(ctrl_id, &item_info.group_id, &item_info.menu_id);
            
#if defined MMI_GRID_IDLE_SUPPORT
            if(MMISET_IDLE_STYLE_GRID == MMIAPISET_GetIdleStyle())
            {
                MMIAPIGRID_AddMenu2Idle(item_info, current_point);
            }
#endif

#if defined QBTHEME_SUPPORT
            if(MMISET_IDLE_QBTHEME == MMIAPISET_GetIdleStyle())
            {
                MMIAPIQBIDLE_AddMenu2Idle(item_info, current_point);
            }
#endif

            //清除tp状态，使ilde可以处理tp up
            MMK_SetTPDownWin((MMI_HANDLE_T)0);

            //MMK_ReturnIdleWin();
            MMK_CloseWin(win_id);
            result = MMI_RESULT_TRUE;
        }
        break;
#endif        
      

    case MSG_NOTIFY_MENU_SET_STATUS:
		//fixed by hongjun.jia for cr152406
		if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
		{
			MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
		}
		else
		{
			if (*(GUIMENU_MOVE_STATE_T*)param == GUIMENU_STATE_MOVE_SET)
			{
#ifdef MMI_VIDEOWALLPAPER_SUPPORT
				MMIAPISETVWP_Pause();
#endif
				GUIMENU_SetMenuMoveState(MAINMENUWIN_MENU_CTRL_ID, TRUE);
			}
			else if (*(GUIMENU_MOVE_STATE_T*)param == GUIMENU_STATE_MOVE_CANCEL)
			{
				GUIMENU_CancelMoveIcon(MAINMENUWIN_MENU_CTRL_ID);
			}
			else
			{
				result = MMI_RESULT_FALSE;
			}
		}
        break;

    case MSG_LOSE_FOCUS:
        MMITHEME_DestroyMenuItemCache();
        MAIN_SetAllStbAppCallbackValid(FALSE);
        break;
        
    case MSG_CTL_OK:
#ifdef MMITHEME_MAINMENU_ARRANGE
        if (GUIMENU_IsMoveState(MAINMENUWIN_MENU_CTRL_ID))
        {
            //SCI_TRACE_LOW:"Create MAINMENU_MOVEOPTMENU_WIN_TAB!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MAINMENU_WIN_1130_112_2_18_2_32_50_0,(uint8*)"");
            MMK_CreateWin((uint32*)MAINMENU_MOVEOPTMENU_WIN_TAB, PNULL);
        }
        else
        {
            //SCI_TRACE_LOW:"Create MAINMENU_OPTMENU_WIN_TAB!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MAINMENU_WIN_1135_112_2_18_2_32_50_1,(uint8*)"");
            MMK_CreateWin((uint32*)MAINMENU_OPTMENU_WIN_TAB, PNULL);
        }
        break;
#endif
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
#if (defined MMI_GRID_IDLE_SUPPORT) || (defined QBTHEME_SUPPORT)
    case MSG_GRID_PEN_MENU_ITEM:
        if (MSG_GRID_PEN_MENU_ITEM == msg_id)
        {
            group_id = ((MMIMAINMENU_ITEM_INFO_T*)param)->group_id;
            menu_id  = ((MMIMAINMENU_ITEM_INFO_T*)param)->menu_id;
        }
        else
#endif
        {
            GUIMENU_GetId(ctrl_id, &group_id, &menu_id);
        }

#if defined(DYNAMIC_MAINMENU_SUPPORT)
        if (DYNAMIC_MAINMENU_DEFAULT_GROUP_ID == group_id)
        {
            GUIMENU_MAINMENU_DY_ITEM_T item_info = {0};
            BOOLEAN is_info_get = FALSE;
            
            is_info_get = MMIMENU_DySlideGetRunInfo(menu_id, &item_info);
            
            if (is_info_get
                && PNULL != item_info.ori_data_ptr
                && MMIMENU_DyIsRunInfoValid(&(item_info.ori_data_ptr->dynamic_menu_info)))
            {
                (item_info.ori_data_ptr->dynamic_menu_info.link_function_ptr)(
                    item_info.ori_data_ptr->dynamic_menu_info.param1ptr, 
                    item_info.ori_data_ptr->dynamic_menu_info.param2ptr
                    );
                
                MMIMENU_DyFreeParmInfo(&item_info);
            }

            break;
        }
#endif

        switch (menu_id)
        {
        case ID_COMMUNICATION_DIAL:
            MMIAPIIDLE_EnterDialWin();
            break;
            
        case ID_COMMUNICATION_MESSAGE:
            MMIAPISMS_OpenMessageMainMenuWin();
            break;
            
        case ID_COMMUNICATION_BROWSER:
#ifdef BROWSER_SUPPORT
        {
            MMIBROWSER_ENTRY_PARAM entry_param = {0};
            
            entry_param.type = MMIBROWSER_ACCESS_MAINMENU;
            entry_param.dual_sys = MN_DUAL_SYS_MAX;
            
            MMIAPIBROWSER_Entry(&entry_param);
        }
#endif
            break;

#ifdef OPERA_MINI_SUPPORT
        case ID_MAINMENU_OPERAMINI:
        {
            MMIBROWSER_ENTRY_PARAM entry_param = {0};
            
            entry_param.type = MMIBROWSER_ACCESS_MAINMENU;
            entry_param.dual_sys = MN_DUAL_SYS_MAX;
            entry_param.is_browser_type_customized = TRUE;
            entry_param.browser_type = MMIBROWSER_TYPE_OPERA_MINI;
            
            MMIAPIBROWSER_Entry(&entry_param);
        }
        break;
#endif    
#ifdef BROWSER_SUPPORT_NETFRONT
        case ID_MAINMENU_NF:
        {
            MMIBROWSER_ENTRY_PARAM entry_param = {0};
            
            entry_param.type = MMIBROWSER_ACCESS_MAINMENU;
            entry_param.dual_sys = MN_DUAL_SYS_MAX;
            entry_param.is_browser_type_customized = TRUE;
            entry_param.browser_type = MMIBROWSER_TYPE_NF;
            
            MMIAPIBROWSER_Entry(&entry_param);
        }
        break;
#endif    
#ifdef BROWSER_SUPPORT_DORADO    
        case ID_MAINMENU_DORADO:
        {
            MMIBROWSER_ENTRY_PARAM entry_param = {0};
            
            entry_param.type = MMIBROWSER_ACCESS_MAINMENU;
            entry_param.dual_sys = MN_DUAL_SYS_MAX;
            entry_param.is_browser_type_customized = TRUE;
            entry_param.browser_type = MMIBROWSER_TYPE_DORADO;
            
            MMIAPIBROWSER_Entry(&entry_param);
        }
        break;
#endif    

#ifdef MMIEMAIL_SUPPORT
        case ID_COMMUNICATION_EMAIL:
           MMIAPIEMAIL_Entry();
            break;
#endif

        case ID_COMMUNICATION_CL:	
            MMIAPICL_OpenMainMenuWindow();
            break;
            
        case ID_COMMUNICATION_PB:
            MMIAPIPB_OpenListAllGroupWin();
            break;
            
        case ID_COMMUNICATION_STK:     
            {
                uint16 sim_sys = MN_DUAL_SYS_1;
                uint32 sim_num = 0;
				
				if (0 == MMIAPISTK_GetStkCardNum( PNULL, 0 ))
				{
					break;
				}
				
                if (MMIAPISET_GetFlyMode())
                {
                    MMIAPIPHONE_AlertSimNorOKStatus();
                    break;
                }

                sim_num = MMIAPISTK_GetStkCardNum(&sim_sys, 1);
				
                if (sim_num == 0)
                {
                    MMIAPIPHONE_AlertSimNorOKStatus();
                }
                else if (sim_num == 1)
                {
                    MMIAPISTK_EnterToplevelMenu((MN_DUAL_SYS_E)sim_sys);
                }
#ifndef MMI_MULTI_SIM_SYS_SINGLE
                else
                {
                    MMIAPISTK_SelectSIMFunc();//Sam.hua	use SelectSimFunc 
                }
#endif
            }
            break;
            
#ifdef  CAMERA_SUPPORT
        case ID_ENTERTAIMENT_CAMERA:
            MMIAPIDC_OpenPhotoWin(); 
            break;
            
        case ID_ENTERTAIMENT_DV:
            MMIAPIDC_OpenVideoWin(); 
            break;
            
#if defined(MMIDC_F_U_CAMERA)
        case ID_ENTERTAIMENT_UCAMERA:
            MMIAPIDC_OpenUCameraPhotoWin();
            break;
#endif            
#endif
#ifdef SNS_SUPPORT
#ifdef WRE_FACEBOOK_SUPPORT
       case ID_MAINMENU_FACEBOOK:
	   MMIWRE_StartAppEx(WRE_FIX_ID_FACEBOOK, NULL);
       break;	   	
#endif

#ifdef WRE_TWITTER_SUPPORT
       case ID_MAINMENU_TWITTER:
	  MMIWRE_StartAppEx(WRE_FIX_ID_TWITTER, NULL);
       break;	   	
#endif

#ifdef WRE_WEIBO_SUPPORT
       case ID_MAINMENU_SINA_BOLG:
	  MMIWRE_StartAppEx(WRE_FIX_ID_SINA, NULL);	
       break;	   	
#endif
#endif

			
#ifdef MMI_KING_MOVIE_SUPPORT	// 电影王
		case ID_MENU_KING_MOVIE:
			MMIAPIKM_MainEntry();
			break;
#endif

#ifdef OPTION_PAGE_MAIN_MENU_SUPPORT
        case ID_ENTERTAIMENT_PLAYER:
            MMIAPIMP3_OpenPlayerMenuWin();
            break;
#endif
            
        case ID_PIC_VIEWER:
            MMIAPIPICVIEW_OpenPicViewer();
            break;
            
#ifdef GAME_SUPPORT
        case ID_ENTERTAIMENT_GAME:
            MMIAPIGAME_OpenMainMenuWindow();
            break;
#endif
            
        case ID_EBOOK:
#ifdef EBOOK_SUPPORT
            if ( MMIAPISET_IsOpenPrivacyProtect(MMISET_PROTECT_EBOOK) )
            {
                MMIAPISET_ValidatePrivacyAppEntry(MMISET_PROTECT_EBOOK,MMIAPIEBOOK_ValidatePrivacyAppEntry);
            }
            else
            {
                MMIAPIEBOOK_ManagerWinMsg(TRUE);
            }
#endif 
            break;
            
#ifdef JAVA_SUPPORT            
        case ID_ENTERTAIMENT_GOLDHOUSE:
            MMIAPIJAVA_OpenMainMenu();
            break;
#endif

#ifdef GPS_SUPPORT
        case ID_CONNECTION_GPS:
            MMIAPIGPS_OpenMainWin();
            break;
#endif
            
        case ID_TOOLS_ALARM:
            MMIAPIALM_OpenMainWin();
            break;
   
#ifdef ASP_SUPPORT
        case ID_TOOLS_ASP:
            MMIAPIASP_EnterAsp();
            break;
#endif
            
#ifdef  DCD_SUPPORT
        case ID_TOOLS_DCD:
            MMIAPIDCD_OpenMainWin();
            break;
#endif //DCD_SUPPORT

        case ID_TOOLS_CALENDAR:
            MMIAPISET_ValidatePrivacyAppEntry(MMISET_PROTECT_CALENDAR, MMIAPICALEND_OpenMainWin);
            break;
            
        case ID_TOOLS_RECORD:
            MMIAPIRECORD_OpenMainWin();
            break;
            
        case ID_MAINMENU_PIM:
            MMIAPIPIM_Main(0);
            break;
            
#ifdef SEARCH_SUPPORT
        case ID_MAINMENU_SEARCH:
            MMIAPISEARCH_Entry();
            break;
#endif
            
        case ID_TOOLS_CALCULATOR:
            MMIAPICALC_OpenMainWin();
            break;
            
        case ID_TOOLS_UNIT_CONVERSION:
            MMIAPIUNITCONV_OpenMainWin();
            break;
            
        case ID_ENTERTAIMENT_FAVORITE:
#ifdef CMCC_UI_STYLE    
            MMK_CreateWin((uint32 *)MAINMENU_ICON_MYFAVORITE_WIN_TAB,PNULL);
#else
            MMIAPIFMM_OpenExplorer();
#endif
            break;
            
        case ID_TOOLS_OTHERS:
            MMK_CreateWin((uint32 *)MAINMENU_OTHERS_WIN_TAB,PNULL);
            break;
            
        case ID_MAINMENU_SLIDE_TOOLS_OTHERS:
            MMK_CreateWin((uint32 *)MAINMENU_SLIDE_OTHERS_WIN_TAB,PNULL);
            break;

        case ID_MAINMENU_SLIDE_SETTINGS:
            MMK_CreateWin((uint32 *)MAINMENU_SLIDE_ICON_SETTINGS_WIN_TAB,PNULL);
            break;
            
        case ID_SETTINGS_CALL:
            MMIAPISET_OpenCallSettingsWindow();
            break;
            
        case ID_SETTINGS_PHONE:
            MMIAPISET_EnterPhoneSettingWin();
            break;
            
        case ID_SETTINGS_DISPLAY:
            MMIAPISET_EnterDisplaySettingWin();
            break;

        case ID_MAINMENU_STYLE_SET:
            MMIAPISET_CreateMainmenuStyleWin();
            break;
            
#ifdef GO_MENU_ONE_ICON_CHANGE_STYLE
        case ID_GOMENU_CHANGE_STYLE:
            MMIAPIMENU_ChangeGOMenuStyle(win_id);
            break;
#endif
#ifdef MMI_KEY_LOCK_EFFECT_SUPPORT
        case ID_MAINMENU_LOCK_SET:
            MMIAPISET_CreateLockStyleWin();
            break;
#endif // #ifdef MMI_KEY_LOCK_EFFECT_SUPPORT

        case ID_SETTINGS_SECURITY:
            MMIAPISET_EnterSecuritySettingWin();
            break;
            
        case ID_SETTINGS_ENVIONMENT:
            MMIENVSET_OpenMainMenuWindow();
            break;
            
        case ID_SETTINGS_LINK:
            MMIAPICONNECTION_OpenMenuWin();
            break;
            
        case ID_MAINMENU_PLAYER:
            MMK_CreateWin((uint32 *)MAINMENU_ICON_PLAYER_WIN_TAB,PNULL);
            break;
            
#ifdef CMMB_SUPPORT
        case ID_ENTERTAIMENT_MTV:
            MMIAPIMTV_OpenMTVPlayer();
            break;
#endif
#ifdef ATV_SUPPORT
        case ID_ENTERTAIMENT_ATV:
            MMIAPIATV_OpenATV();
            break;
#endif

#ifdef FM_SUPPORT
        case ID_ENTERTAIMENT_FM:
            MMIAPIFM_OpenMainWin();
            break;
#ifdef MMI_FM_TIMER_RECORD_SUPPORT
        case ID_ENTERTAIMENT_FM_TIMER_RECORD:
            MMIAPIFM_CreateFMTimerRecordInfoWin();    
            break; 
#endif           
#endif
            
#ifdef CMCC_UI_STYLE
        case ID_MAINMENU_CMCC_MONERNET:     
            MMIAPISET_EnterMonternetWin();
            break;
            
        case ID_MAINMENU_CMCC_SERVICE:
            MMK_CreateWin((uint32 *)MAINMENU_ICON_SERVICE_WIN_TAB,PNULL);     
            
            break;
        case ID_CUSTMER_SERVICE:
            MMIAPICS_OpenMainWin();
            break;
        case ID_MAINMENU_APPS:   
            MMK_CreateWin((uint32 *)MAINMENU_ICON_APPS_WIN_TAB,PNULL);
            break;     
        case ID_NET_FETION:
            MMIAPISET_EnterFetionWin();
            break;
            
        case ID_NET_MUSIC:
            MMIAPISET_EnterMusicWin();
            break;
        case ID_ENTERTAIMENT_PIC:
#ifdef CMCC_UI_STYLE
            MMIAPIPICVIEW_OpenPicViewerExt(MMIPICVIEW_DOWNLOAD_PIC);
#else
            MMIAPIPICVIEW_OpenPicViewer();
#endif
            break;
        case ID_APPS_BT:
            MMIAPIBT_OpenMainMenuWin(); 
            break;
#ifdef JAVA_SUPPORT
        case ID_JAVA_APP:
            MMIAPIJAVA_OpenMainMenu();
            break;
#endif

#ifdef CMMB_SUPPORT
        case ID_APPS_MBBMS:
            MMIAPIMTV_OpenMTVPlayer();
            break;
#endif
        case ID_CHINAMOBILE_SECURITIES:
            MMIAPISET_EnterStockWin();
            break;
        case ID_CHINAMOBILE_MAP:
            MMIAPISET_EnterMapWin();
            break;
            
        case ID_SETTINGS_VERSION:
            MMIAPIENG_CreateShowVersionWin();
            break;
#endif //CMCC_UI_STYLE
        case ID_MAINMENU_TOOLS:
            MMK_CreateWin((uint32 *)MAINMENU_ICON_TOOLS_WIN_TAB,PNULL);
            break;
            
        case ID_MAINMENU_PASTIME:
            MMK_CreateWin((uint32 *)MAINMENU_ICON_PASTIME_WIN_TAB,PNULL);
            break;
            
        case ID_MAINMENU_SETTINGS:
            MMK_CreateWin((uint32 *)MAINMENU_ICON_SETTINGS_WIN_TAB,PNULL);
            break;
            
#ifdef QQ_SUPPORT
        case ID_SPREADTRUM_QQ_SUPPORT:
            MMIAPIQQ_OpenMainWin();
            break;
#endif
        case ID_ENTERTAIMENT_MP3:
            MMIAPIMP3_OpenMp3Player();
            break;

#ifdef MMIUZONE_SUPPORT
        case ID_ENTERTAIMENT_UZONE:
            MMIAPIUZONE_Entry();
            break;
#endif
            
        case ID_ENTERTAIMENT_VIDEO:
#ifdef VIDEO_PLAYER_SUPPORT
            MMIAPIVP_MainEntry();
#endif
            break;      

        case ID_ENTERTAIMENT_MOBILE_VIDEO:
#ifdef MOBILE_VIDEO_SUPPORT
            MMIAPIMV_Entry();
#endif
            break;


        case ID_MAINMENU_BT:
            MMIAPIBT_OpenMainMenuWin();   
            break;
            
#ifdef MMI_PDA_SUPPORT
        case ID_COUNTDOWN:
            MMIAPI_OpenTimerWin(ID_COUNTDOWN);   
            break;
            
#else   //MMI_PDA_SUPPORT
        case ID_COUNTDOWN:
            if (MMIAPICT_IsShowIdleCoutime())
            {
                MMIAPICT_OpenCoutimeRunningWin();
            }
            else
            {
                MMIAPICT_OpenCoutimeWin();
            }
            break;
#ifdef STOPWATCH_SUPPORT           
        case ID_STOPWATCH:  //yaye.jiang 2011.7.21
            MMIAPIST_OpenStopWatchWin();
            break;  
#endif
#endif//MMI_PDA_SUPPORT        
        case ID_CLOCK:
            MMIAPI_OpenTimerWin(ID_CLOCK);   
        break;
#ifdef HOME_LOCATION_INQUIRE_SUPPORT
        case ID_PHONESEARCH:
            MMIAPIPHONESEARCH_OpenWnd();
            break;
#endif

#ifdef WIFI_SUPPORT
        case ID_CONNECTION_WIFI:
            MMIAPIWIFI_OpenMainWin();
            break;
#endif

#ifdef MMI_GPRS_SUPPORT
        case ID_CONNECTION_NET_USERID:  
            MMIAPICONNECTION_OpenSettingListWin();
            break;

        case ID_CONNECTION_GPRS:
            MMIAPICONNECTION_OpenGprsWin();
            break;
#endif

        case ID_WORLDTIME:
            MMIAPIWCLK_OpenMainWin();
            break;
            
#ifdef PUSH_EMAIL_SUPPORT   //Added by yuanl  2010.5.7
        case ID_MAINMENU_PUSHMAIL:
            MMIAPIMAIL_Enter(MAIL_ENTER_MAINMENU, PNULL);
            break;
#endif /* PUSH_EMAIL_SUPPORT  */

#ifdef MMI_GPRS_SUPPORT

        case ID_MAINMENU_DIANPING:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_DIANPING);
            break;
            
        case ID_MAINMENU_KAIXIN:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_KAIXIN);
            break;
            
        case ID_MAINMENU_CTRIP:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_CTRIP);
            break;
            
        case ID_MAINMENU_BAIDU:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_BAIDU);
            break;
            
//         case ID_MAINMENU_SINA_BOLG:
//             HandleBrowerUrlMenu(BROWSER_URL_TYPE_SINA_BOLG);
//             break;
            
        case ID_MAINMENU_TAOBAO:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_TAOBAO);
            break;
            
        case ID_MAINMENU_YOUKU:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_YOUKU);
            break;
            
        case ID_MAINMENU_RENREN:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_RENREN);
            break;
            
        case ID_MAINMENU_QIDIAN:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_QIDIAN);
            break;
#ifdef SXH_APPLE_SUPPORT
    	case ID_SXH_APPLE:
		{
			APPLE_StartApple();
		}
		break;
        case ID_SXH_BOOK:
		    {
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_BOOK);
				}
				else
				{
					extern void APPLE_StartBook(void);
					APPLE_StartBook();
				}
			}
		break;
		case ID_SXH_UMVCHAT:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_UMVCHAT);	
				}
				else
				{
					extern void APPLE_StartUMvchat(void);
					APPLE_StartUMvchat();
				}
			}
		break;
		case ID_SXH_TV1:
			{
				if(OEMOS_GetLangId() == 86)
				{
					SMDMenuTv1();
				}
				else
				{
					extern void APPLE_StartTv1(void);
					APPLE_StartTv1();
				}
			}
		break;
		case ID_SXH_TV2:
			{
				if(OEMOS_GetLangId() == 86)
				{
					SMDMenuTv2();	
				}
				else
				{
					extern void APPLE_StartTv2(void);
					APPLE_StartTv2();
				}
			}
		break;
		case ID_SXH_TV3:
			{
				if(OEMOS_GetLangId() == 86)
				{
					SMDMenuTv3();	
				}
				else
				{
					extern void APPLE_StartTv3(void);
					APPLE_StartTv3();
				}
			}
		break;
		case ID_SXH_TV:		// switch (menu_id)下
			{	
				if(OEMOS_GetLangId() == 86)
				{	

					APPLE_StartSMDApp(SXH_TV);	
				}
				else
				{
					void APPLE_StartTv1(void);	
					APPLE_StartTv1();
				}
				
			}
		break;
		case ID_SXH_RADIO:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_RADIO);	
				}
				else
				{
					extern void APPLE_StartRadio(void);
					APPLE_StartRadio();
				}
			}
	break;
		case ID_SXH_Games:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_ONLINEGAME);	
				}
				else
				{
					extern void APPLE_StartOnlineGame(void);
					APPLE_StartOnlineGame();
				}
			}
			break;
		case ID_SXH_UMMUSIC:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_MUSIC);	
				}
				else
				{
					extern void APPLE_StartMusic(void);
					APPLE_StartMusic();
				}
			}
			break;
		case ID_SXH_VIDEODL:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_VIDELDL);	
				}
				else
				{
					extern void APPLE_StartVideoDL(void);
					APPLE_StartVideoDL();
				}
			}
			break;
		case ID_SXH_UMPLAYER:
			{
				extern void APPLE_StartUMPlayer(void);
				APPLE_StartUMPlayer();
			}
			break;	
		case ID_SXH_WEATHER:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_WEATHER);	
				}
				else
				{
					extern void APPLE_StartWeather(void);
					APPLE_StartWeather();
				}
			}
			break;
			
#ifdef SXH_APPLE_3GVEDIO_SUPPORT	
	case ID_SXH_UMVCHATTHIN:
		{
			APPLE_StartUMvchatthin();
		}
	break;
#endif // SXH_APPLE_3GVEDIO_SUPPORT	
#ifdef SXH_APPLE_UMVCHAT_DEMO
       case ID_SXH_UMVCHATDEMO:
	   	{
			APPLE_StartUMvchatDemo();
       	}
	   break;
#endif
#endif
			
        case ID_MAINMENU_SINA:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_SINA);
            break;
            
        case ID_MAINMENU_SOSO:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_SOSO);
            break;

//new xiyuan
		case ID_MAINMENU_PALMREAD:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_PALMREAD);
            break;
			
		case ID_MAINMENU_AMUSEBOX:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_AMUSEBOX);
            break;
			
		case ID_MAINMENU_MOBILESOHU:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_MOBILESOHU);
            break;
			
		case ID_MAINMENU_COOLSTATION:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_COOLSTATION);
            break;
			
		case ID_MAINMENU_COLORWORLD:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_COLORWORLD);
            break;
			
		case ID_MAINMENU_TUDOU:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_TUDOU);
            break;
			
		case ID_MAINMENU_ZHIFUBAO:
            HandleBrowerUrlMenu(BROWSER_URL_TYPE_ZHIFUBAO);
            break;
//new end
#endif

#ifdef QQ_SUPPORT
        case ID_MAINMENU_QQ:
            MMIAPIQQ_OpenMainWin();
            break;
#endif

//============== MET MEX Start ===================
#ifdef MET_MEX_MERIG_SUPPORT
        case ID_MAINMENU_MET_MEX_ITEM_ID:
            {
                MetMexOpenWin();
            }
            break;
#endif // MET_MEX_MERIG_SUPPORT


#ifdef MET_MEX_QQ_SUPPORT
        case ID_MAINMENU_MET_MEX_QQ_ITEM_ID:
            {
                MetMexQQOpenWin();
            }
            break;
#endif // MET_MEX_QQ_SUPPORT
#ifdef MET_MEX_ZT_SUPPORT
		case ID_MAINMENU_MET_MEX_ZT_ITEM_ID:
			{
				MetMexZTOpenWin();
			}
			break;
#endif // MET_MEX_ZT_SUPPORT
#ifdef MET_MEX_FRUITNINJA_SUPPORT
		case ID_MAINMENU_MET_MEX_FRUITNINJA_ITEM_ID:
			{
				MetMexFruitNinjaOpenWin();
			}
			break;
#endif // MET_MEX_FRUITNINJA_SUPPORT
#ifdef MET_MEX_WEIBO_SUPPORT
        case ID_MAINMENU_MET_MEX_WEIBO_ITEM_ID:
            {
                MetMexWeiboOpenWin();
            }
			break;
#endif // MET_MEX_WEIBO_SUPPORT
#ifdef MET_MEX_WEATHER_SUPPORT
        case ID_MAINMENU_MET_MEX_WEATHER_ITEM_ID:
            {
                MetMexWeatherOpenWin();
            }
			break;
#endif  // MET_MEX_WEATHER_SUPPORT
#ifdef MET_MEX_WOMIVIDEO_SUPPORT
        case ID_MAINMENU_MET_MEX_WOMIVIDEO_ITEM_ID:
            {
                MetMexWomiVideoOpenWin();
            }
			break;
#endif  //MET_MEX_WOMIVIDEO_SUPPORT
#ifdef MET_MEX_TB_SUPPORT
		case ID_MAINMENU_MET_MEX_TB_ITEM_ID:
			{
                MetMexTBOpenWin();
			}
			break;
#endif // MET_MEX_TB_SUPPORT


#ifdef MET_MEX_AQ_SUPPORT
		case ID_MAINMENU_MET_MEX_AQ_ITEM_ID:
			{
                MetMexAQOpenWin();
			}
			break;
#endif // MET_MEX_AQ_SUPPORT
#ifdef MET_MEX_DOUBLE_QQ_SUPPORT
		case ID_MAINMENU_MET_MEX_QQ1_ITEM_ID:
			{
				MetMexQQ1OpenWin();
			}
			break;
		case ID_MAINMENU_MET_MEX_QQ2_ITEM_ID:
			{
				MetMexQQ2OpenWin();
			}
			break;
#endif // MET_MEX_DOUBLE_QQ_SUPPORT

#ifdef MET_MEX_MSN_SUPPORT
		case ID_MAINMENU_MET_MEX_MSN_ITEM_ID:
			{
				MetMexMSNOpenWin();
			}
			break;
#endif 
#ifdef MET_MEX_FETION_SUPPORT
		case ID_MAINMENU_MET_MEX_FETION_ITEM_ID:
			{
				MetMexFETIONOpenWin();
			}
			break;
#endif
#ifdef MET_MEX_AB_SUPPORT
		case ID_MAINMENU_MET_MEX_AB_ITEM_ID:
			{
				MetMexABOpenWin();
			}
			break;
#endif // MET_MEX_AQ_SUPPORT
#ifdef MET_MEX_CQZ_SUPPORT
    case ID_MAINMENU_MET_MEX_CQZ_ITEM_ID:
		{
			MetMexCQZOpenWin();
		}
	break;
#endif

#ifdef MET_MEX_JSCB_SUPPORT
    case ID_MAINMENU_MET_MEX_JSCB_ITEM_ID:
		{
			MetMexJSCBOpenWin();
		}
	break;
#endif
#ifdef MET_MEX_TOMCAT_SUPPORT
		case ID_MAINMENU_MET_MEX_TOMCAT_ITEM_ID:
			{
				MetMexTomCatOpenWin();
			}
			break;
#endif // MET_MEX_AQ_SUPPORT
//============== MET MEX end ===================


#ifdef WEATHER_SUPPORT
        case ID_MAINMENU_WEATHER:
            MMIAPIWEATHER_OpenMainWin();
            break;
#endif

#ifdef IKEYBACKUP_SUPPORT
        case ID_MAINMENU_IKEYBACKUP:
            MMIAPIIKB_OpenMainWin();
            break;
#endif


            
#ifdef MV_SUPPORT
        case ID_MAINMENU_MAGIC_VOICE:
            MMIAPISET_OpenMagicVoiceWin();
            break;
#endif
#ifdef MMI_AZAN_SUPPORT
        case ID_AZAN:
            MMIAPIAZAN_OpenMuSiLinPrayWin();
            break;
#endif  
#ifdef WRE_SUPPORT
		case ID_TOOLS_WRE:
            {
		  MMIWRE_Entry();
            }	
            break;
#ifdef WRE_CONTRA_SUPPORT  // 魂斗罗
	case ID_WRE_CONTRA:
            {
		  MMIWRE_StartAppEx(WRE_FIX_ID_CONTRA, NULL);
            }	
            break;
#endif
#ifdef WRE_SUPERMARIO_SUPPORT  // 超级玛丽
	case ID_WRE_SUPERMARIO:
            {
		  MMIWRE_StartAppEx(WRE_FIX_ID_SUPERMARIO, NULL);
            }	
            break;
#endif
#ifdef WRE_TSLAND_SUPPORT  // 冒险岛
	case ID_WRE_TSLAND:
            {
		  MMIWRE_StartAppEx(WRE_FIX_ID_TSLAND, NULL);
            }	
            break;
#endif
#endif
	                    
#ifdef MCARE_V31_SUPPORT
        case ID_TMC_APP_SUPPORT:
            MCareV31_Main_Entry();
            break;
        case ID_BRW_APP_SUPPORT:
            MCareV31_QQBrowser_Shortcut();
            break;
#endif

#ifdef MMI_TASK_MANAGER
        case ID_APP_MANAGER:
            MMI_OpenAppletManager();
            break;
#endif

#ifdef MMI_MEMO_SUPPORT
        case ID_APP_MEMO:
            MMIMEMO_OpenMain();
            break;
#endif

#ifdef MRAPP_SUPPORT
#ifdef MRAPP_COOKIE_OPEN
    case ID_MRAPP_COOKIE:
        MMIMRAPP_AppEntry(MRAPP_COOKIE);
        break;
#endif

#ifdef MRAPP_MRPSTORE_OPEN
    case ID_MRAPP_MRPSTORE:
        MMIMRAPP_AppEntry(MRAPP_MRPSTORE);
        break;
#endif

#ifdef MRAPP_SKYBUDDY_OPEN
		case ID_MRAPP_SKYBUDDY:
            MMIMRAPP_AppEntry(MRAPP_SKYBUDDY);
            break;
#endif

#ifdef __MMI_SKYQQ__
        case ID_MRAPP_QQ:
            MMIMRAPP_AppEntry(MRAPP_SKYQQ);
            break;
#else
#ifdef MRAPP_SDKQQ_OPEN
    case ID_MRAPP_QQ:
        MMIMRAPP_AppEntry(MRAPP_SDKQQ);
        break;
#endif
#endif
			


#ifdef MRAPP_NETGAME_OPEN
   case ID_MRAPP_NETGAME:
        MMIMRAPP_AppEntry(MRAPP_NETGAME);
        break;
#endif



#ifdef MRAPP_TIANQI_OPEN
    case ID_MRAPP_TIANQI:
        MMIMRAPP_AppEntry(MRAPP_TIANQI);
        break;
#endif

#ifdef MRAPP_FETION_OPEN
    case ID_MRAPP_FETION:
        MMIMRAPP_AppEntry(MRAPP_FETION);
        break;
#endif

#ifdef MRAPP_XLWB_OPEN
    case ID_MRAPP_XLWB:
        MMIMRAPP_AppEntry(MRAPP_XLWB);
        break;
#endif

#ifdef MRAPP_KAIXIN_OPEN
   case ID_MRAPP_KAIXIN:
        MMIMRAPP_AppEntry(MRAPP_KAIXIN);
        break;
#endif

#ifdef MRAPP_FARM_OPEN
   case ID_MRAPP_FARM:
        MMIMRAPP_AppEntry(MRAPP_FARM);
        break;
#endif

#ifdef MRAPP_CLOCK_OPEN
    case ID_MRAPP_CLOCK:
        MMIMRAPP_AppEntry(MRAPP_CLOCK);
        break;
#endif

#ifdef MRAPP_FACEBOOK_OPEN
    case ID_MRAPP_FACEBOOK:
        MMIMRAPP_AppEntry(MRAPP_FACEBOOK);
        break;
#endif

#ifdef MRAPP_YAHOO_OPEN
    case ID_MRAPP_YAHOO:
        MMIMRAPP_AppEntry(MRAPP_YAHOO);
        break;
#endif


#ifdef MRAPP_TWITTER_OPEN
		case ID_MRAPP_TWITTER:
			MMIMRAPP_AppEntry(MRAPP_TWITTER);
			break;
#endif


#ifdef MRAPP_TALKCAT_OPEN
  		 case ID_MRAPP_TALKCAT:
        	MMIMRAPP_AppEntry(MRAPP_TALKCAT);
        	break;
#endif

#ifdef MRAPP_SKIRT_OPEN
   		case ID_MRAPP_SKIRT:
        	MMIMRAPP_AppEntry(MRAPP_SKIRT);
        	break;
#endif

#ifdef MRAPP_BIRD_OPEN
   		case ID_MRAPP_BIRD:
        	MMIMRAPP_AppEntry(MRAPP_BIRD);
        	break;
#endif

#ifdef MRAPP_FRUIT_OPEN
   		case ID_MRAPP_FRUIT:
        	MMIMRAPP_AppEntry(MRAPP_FRUIT);
        	break;
#endif

#ifdef MRAPP_BYDR_OPEN
   		case ID_MRAPP_BYDR:
        	MMIMRAPP_AppEntry(MRAPP_BYDR);
        	break;
#endif

#endif
                  
#ifdef DMENU_SUPPORT
   	case ID_DMENU:
        MMIDMENU_Entry(0);
        break;
#endif


#ifdef MMI_PDA_SUPPORT
#if defined(MMI_ANDROID_SUPPORT) && defined(MMI_ISTYLE_SUPPORT)
        case ID_MAINMENU_IDLE_STYLE:
            
#ifndef QBTHEME_SUPPORT
            if ( MMISET_IDLE_ISTYLE == MMIAPISET_GetIdleStyle() )
            {
                MMISET_SetIdleStyle( MMISET_IDLE_STYLE_GRID );
            }
            else
            {
                GUI_BOTH_RECT_T both_rect = {0};
                
                MMISET_SetIdleStyle( MMISET_IDLE_ISTYLE );
                
                both_rect  = MMITHEME_GetFullScreenBothRect();
                GUIAPICTRL_SetRect(MMIIDLE_WALLPAPER_ANIM_CTRL_ID, &both_rect.v_rect);                 
            }
            MMK_ReturnIdleWin();
            
            MMIPUB_OpenAlertWaitingWin(TXT_SWITCHING_SYSTEM);
#else
            MMIQBTHEMEMENU_SetIldeStyle();
#endif//QBTHEME_SUPPORT

            break;
#endif

#ifdef QBTHEME_SUPPORT
        case ID_MAINMENU_RETURNIDLE:
            MMK_ReturnIdleWin();
        break;

        case ID_MAINMENU_QBTHEMEAPP:
            MMIAPIQBTHEME_OpenMainWin();
        break;

        case ID_MAINMENU_QBTHEMEMORE:
            MMIAPIQBTHEME_OpenDownLoadUrl();
        break;        
#endif
#endif
        default:
            break;
        }

        break;
        
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
#ifdef CMMB_SUPPORT
        case MSG_KEYDOWN_TV:
      MMIAPIMTV_OpenMTVPlayer();
   break;
#endif
  
    default:
        result = MMI_RESULT_FALSE;
        break;
    }

#ifndef PDA_VIDEOPLAYER_WALLPAPER_SUPPORT_LOW_MEMORY
#ifdef MMI_VIDEOWALLPAPER_SUPPORT 
    idle_style = MMIAPISET_GetIdleStyle();
    mainmenu_style = MMIAPISET_GetMainmenuStyle();
    if(MMISET_IDLE_ISTYLE != idle_style)
    {

        switch (mainmenu_style)
        {

        case GUIMENU_STYLE_CUBE:
        case GUIMENU_STYLE_CRYSTAL_CUBE:
        case GUIMENU_STYLE_CYCLONE:
            //与视屏墙纸冲突
            break;
        default:
            result = MMIAPISETVWP_HandleVideoWallPaper(win_id, msg_id, param);
            break;
        }
    }
#endif
#endif

    return (result);
}

/*****************************************************************************/
// Description : icon main second menu handle window message
// Global resource dependence : 
// Author:jassmine.meng
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainSecondMenuWinMsg(
                                              MMI_WIN_ID_T         win_id, 
                                              MMI_MESSAGE_ID_E     msg_id, 
                                              DPARAM               param
                                              )
{
    MMI_RESULT_E                        result = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T                       ctrl_id = MAINMENUWIN_SECOND_MENU_CTRL_ID;
    MMI_MENU_ID_T                     menu_id = 0;
    MMI_MENU_GROUP_ID_T                 group_id = 0;
    uint16 stk_sim_id = 0;
    uint32 stk_sim_num = 0;
    MMI_STRING_T stk_str = {0};
    BOOLEAN is_exist_stk_name = FALSE;
    BOOLEAN is_menu_visible = TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;

    case MSG_FULL_PAINT:
        stk_sim_num = MMIAPISTK_GetStkCardNum(&stk_sim_id, 1);

        if (stk_sim_num > 0)
        {
            is_menu_visible = TRUE;
        }
        else
        {
            is_menu_visible = FALSE;
        }

        if (1 == stk_sim_num)
        {
            is_exist_stk_name = MMIAPISTK_GetStkName((MN_DUAL_SYS_E)stk_sim_id, &stk_str);
        }

        if (!is_exist_stk_name)
        {
            MMI_GetLabelTextByLang(TXT_COMMON_MAIN_STK, &stk_str);
        }

#ifdef CMCC_UI_STYLE
        GUIMENU_SetStaticItem(MAINMENUWIN_SECOND_MENU_CTRL_ID,
                                                MENU_ICON_SERVICE,
                                                ID_COMMUNICATION_STK,
                                                &stk_str,
                                                IMAGE_NULL
                                                );
#else
        GUIMENU_SetStaticItem(MAINMENUWIN_SECOND_MENU_CTRL_ID,
                                                MENU_ICON_TOOLS,
                                                ID_COMMUNICATION_STK,
                                                &stk_str,
                                                IMAGE_NULL
                                                );
#endif

#ifdef CMCC_UI_STYLE
        GUIMENU_SetItemVisible(MAINMENUWIN_SECOND_MENU_CTRL_ID, MENU_ICON_SERVICE,ID_COMMUNICATION_STK,is_menu_visible);
#else
        GUIMENU_SetItemVisible(MAINMENUWIN_SECOND_MENU_CTRL_ID,MENU_ICON_TOOLS,ID_COMMUNICATION_STK,is_menu_visible);
#endif
        break;

    case MSG_CTL_OK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
        GUIMENU_GetId(ctrl_id, &group_id, &menu_id);
        HandleSecMenuStaticMsg(win_id, msg_id, menu_id);
        break;

    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        result = MMI_RESULT_FALSE;
        break;
    }

    return (result);
}
/*****************************************************************************/
// Description : 处理二级菜单静态菜单消息
// Global resource dependence : 
// Author:jin.wang
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSecMenuStaticMsg(
    MMI_WIN_ID_T         win_id, 
    MMI_MESSAGE_ID_E     msg_id, 
    MMI_MENU_ID_T        menu_id
)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    switch(menu_id)
    {
#ifdef  CAMERA_SUPPORT
    case ID_ENTERTAIMENT_CAMERA:
        MMIAPIDC_OpenPhotoWin();
        break;
    case ID_ENTERTAIMENT_DV:
        MMIAPIDC_OpenVideoWin(); 
        break;
#if defined(MMIDC_F_U_CAMERA)
        case ID_ENTERTAIMENT_UCAMERA:
            MMIAPIDC_OpenUCameraPhotoWin();
            break;
#endif            
#endif

#ifdef MMI_KING_MOVIE_SUPPORT	// 电影王
	case ID_MENU_KING_MOVIE:
		MMIAPIKM_MainEntry();
		break;
#endif

    case ID_ENTERTAIMENT_MP3:
        MMIAPIMP3_OpenMp3Player();
        break;

#ifdef MMIUZONE_SUPPORT
    case ID_ENTERTAIMENT_UZONE:
        MMIAPIUZONE_Entry();
        break;
#endif

    case ID_ENTERTAIMENT_VIDEO:
#ifdef VIDEO_PLAYER_SUPPORT
        MMIAPIVP_MainEntry();
#endif
        break;

    case ID_ENTERTAIMENT_MOBILE_VIDEO:
#ifdef MOBILE_VIDEO_SUPPORT
        MMIAPIMV_Entry();
#endif
        break;

#ifdef SEARCH_SUPPORT
    case ID_MAINMENU_SEARCH:
         MMIAPISEARCH_Entry();
         break;
#endif

#ifdef FM_SUPPORT
    case ID_ENTERTAIMENT_FM:
        MMIAPIFM_OpenMainWin();
        break;
#ifdef MMI_FM_TIMER_RECORD_SUPPORT
    case ID_ENTERTAIMENT_FM_TIMER_RECORD:
        MMIAPIFM_CreateFMTimerRecordInfoWin();    
        break; 
#endif
#endif

#ifdef STREAMING_SUPPORT_NEXTREAMING
    case ID_ENTERTAIMENT_ST:
        MMIAPIST_SetFromNet(FALSE);
  MMIAPIST_OpenPlayWin();
        break;
#endif

#ifdef KURO_SUPPORT
    case ID_ENTERTAIMENT_KUR:
        MMIAPIKUR_OpenKurPlayer();
        break;
#endif

#ifdef GPS_SUPPORT
    case ID_CONNECTION_GPS:
        MMIAPIGPS_OpenMainWin();
        break;
#endif

#ifdef CMMB_SUPPORT
    case ID_ENTERTAIMENT_MTV:
        MMIAPIMTV_OpenMTVPlayer();
        break;
#endif

#ifdef ATV_SUPPORT
    case ID_ENTERTAIMENT_ATV:
        MMIAPIATV_OpenATV();
        break;
#endif
        
    case ID_WORLDTIME:
        MMIAPIWCLK_OpenMainWin();
        break;

#ifdef MMI_PDA_SUPPORT
    case ID_COUNTDOWN:
        MMIAPI_OpenTimerWin(ID_COUNTDOWN);   
        break;
#else
    case ID_COUNTDOWN:
       if (MMIAPICT_IsShowIdleCoutime())
       {
            MMIAPICT_OpenCoutimeRunningWin();
       }
       else       
       {
            MMIAPICT_OpenCoutimeWin();
       }
       break;

#ifdef STOPWATCH_SUPPORT        
    case ID_STOPWATCH:
        MMIAPIST_OpenStopWatchWin();
        break;
#endif            
#endif//  MMI_PDA_SUPPORT 

#ifdef HOME_LOCATION_INQUIRE_SUPPORT
    case ID_PHONESEARCH:
        MMIAPIPHONESEARCH_OpenWnd();
        break;
#endif

#ifdef DYNAMIC_MODULE_SUPPORT
    case ID_DYNAPP:
        MMIAPIDYNA_OpenMainListWindow();
        break;
#endif

    case ID_COMMUNICATION_CL:
        MMIAPICL_OpenMainMenuWindow();
        break;
  
#ifdef ASP_SUPPORT
    case ID_TOOLS_ASP:
     MMIAPIASP_EnterAsp();
    break;
#endif
#ifdef MMIEMAIL_SUPPORT
    case ID_COMMUNICATION_EMAIL:
        MMIAPIEMAIL_Entry();
        break;
#endif
    case ID_TOOLS_ALARM:
        MMIAPIALM_OpenMainWin();
        break;

    case ID_CLOCK:
        MMIAPI_OpenTimerWin(ID_CLOCK);   
    break;
    
    case ID_TOOLS_CALENDAR:
        MMIAPISET_ValidatePrivacyAppEntry(MMISET_PROTECT_CALENDAR, MMIAPICALEND_OpenMainWin);
        break;

    case ID_TOOLS_CALCULATOR:
        MMIAPICALC_OpenMainWin();
        break;
case ID_TOOLS_UNIT_CONVERSION:
        MMIAPIUNITCONV_OpenMainWin();
        break;
        

   case ID_MAINMENU_PIM:
     MMIAPIPIM_Main(0);
    break;
    
    case ID_TOOLS_RECORD:
        MMIAPIRECORD_OpenMainWin();
        break;

    case ID_COMMUNICATION_STK:
        {
            uint16 sim_sys = MN_DUAL_SYS_1;
            uint32 sim_num = 0;

            if (MMIAPISET_GetFlyMode())
            {
                MMIAPIPHONE_AlertSimNorOKStatus();
                break;
            }

            sim_num = MMIAPISTK_GetStkCardNum(&sim_sys, 1);
            if (sim_num == 0)
            {
                MMIAPIPHONE_AlertSimNorOKStatus();
            }
            else if (sim_num == 1)
            {
                MMIAPISTK_EnterToplevelMenu((MN_DUAL_SYS_E)sim_sys);
            }
#ifndef MMI_MULTI_SIM_SYS_SINGLE
            else
            {
                MMIAPISTK_SelectSIMFunc();
            }
#endif
        }
        break;

    case ID_DOWNLOAD_MANAGER:
#ifdef DL_SUPPORT
        MMIAPIDL_StartDownload(MMIDL_OPEN_DOWNLOAD_MANAGER, PNULL);
#endif
        break;

    case ID_PIC_VIEWER:
        MMIAPIPICVIEW_OpenPicViewer();
        break;
#ifdef JAVA_SUPPORT
    case ID_ENTERTAIMENT_GOLDHOUSE:
        MMIAPIJAVA_OpenMainMenu();
        break;
#endif
    case ID_EBOOK:
#ifdef EBOOK_SUPPORT
        if ( MMIAPISET_IsOpenPrivacyProtect(MMISET_PROTECT_EBOOK) )
        {
            MMIAPISET_ValidatePrivacyAppEntry(MMISET_PROTECT_EBOOK,MMIAPIEBOOK_ValidatePrivacyAppEntry);
        }
        else
        {
            MMIAPIEBOOK_ManagerWinMsg(TRUE);
        }
#endif
        break;
#ifdef MMI_ENABLE_DCD            
   case ID_DCD_OPEN:     
        {
            MMIAPIDCD_OpenMainWin();
        }   
     break;
#endif    

#ifdef SXH_APPLE_SUPPORT
    	case ID_SXH_APPLE:
		{
			extern void APPLE_StartApple(void);
			APPLE_StartApple();
		}
		break;
        case ID_SXH_BOOK:
		    {
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_BOOK);
				}
				else
				{
					extern void APPLE_StartBook(void);
					APPLE_StartBook();
				}
			}
		break;
		case ID_SXH_UMVCHAT:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_UMVCHAT);	
				}
				else
				{
					extern void APPLE_StartUMvchat(void);
					APPLE_StartUMvchat();
				}
			}
		break;
		case ID_SXH_TV1:
			{
				if(OEMOS_GetLangId() == 86)
				{
					SMDMenuTv1();
				}
				else
				{
					extern void APPLE_StartTv1(void);
					APPLE_StartTv1();
				}
			}
		break;
		case ID_SXH_TV2:
			{
				if(OEMOS_GetLangId() == 86)
				{
					SMDMenuTv2();	
				}
				else
				{
					extern void APPLE_StartTv2(void);
					APPLE_StartTv2();
				}
			}
		break;
		case ID_SXH_TV3:
			{
				if(OEMOS_GetLangId() == 86)
				{
					SMDMenuTv3();	
				}
				else
				{
					extern void APPLE_StartTv3(void);
					APPLE_StartTv3();
				}
			}
		break;
		case ID_SXH_TV:		// switch (menu_id)下
			{
#ifdef APPLE_LOG    
			OEMOS_dbgprintf("ID_SXH_TV is ok OEMOS_GetLangId is %d" ,OEMOS_GetLangId());
#endif		

				if(OEMOS_GetLangId() == 86)
				{	

					APPLE_StartSMDApp(SXH_TV);	
				}
				else
				{
					//MMK_CreateWin((uint32*)MAINMENU_ICON_APPLETV_WIN_TAB,NULL);
					APPLE_StartTv1();
				}
				
			}
		break;
		case ID_SXH_RADIO:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_RADIO);	
				}
				else
				{
					extern void APPLE_StartRadio(void);
					APPLE_StartRadio();
				}
			}
	break;
		case ID_SXH_Games:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_ONLINEGAME);	
				}
				else
				{
					extern void APPLE_StartOnlineGame(void);
					APPLE_StartOnlineGame();
				}
			}
			break;
		case ID_SXH_UMMUSIC:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_MUSIC);	
				}
				else
				{
					extern void APPLE_StartMusic(void);
					APPLE_StartMusic();
				}
			}
			break;
		case ID_SXH_VIDEODL:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_VIDELDL);	
				}
				else
				{
					extern void APPLE_StartVideoDL(void);
					APPLE_StartVideoDL();
				}
			}
			break;
		case ID_SXH_UMPLAYER:
			{
				extern void APPLE_StartUMPlayer(void);
				APPLE_StartUMPlayer();
			}
			break;	
		case ID_SXH_WEATHER:
			{
				if(OEMOS_GetLangId() == 86)
				{
					APPLE_StartSMDApp(SXH_WEATHER);	
				}
				else
				{
					extern void APPLE_StartWeather(void);
					APPLE_StartWeather();
				}
			}
			break;
			
#ifdef SXH_APPLE_3GVEDIO_SUPPORT	
	case ID_SXH_UMVCHATTHIN:
		{
			APPLE_StartUMvchatthin();
		}
	break;
#endif // SXH_APPLE_3GVEDIO_SUPPORT	
#ifdef SXH_APPLE_UMVCHAT_DEMO
       case ID_SXH_UMVCHATDEMO:
	   	{
			APPLE_StartUMvchatDemo();
       	}
	   break;
#endif
#endif
#ifdef CMCC_UI_STYLE
    case ID_EXPLORE:
  MMIAPIFMM_OpenExplorer();
  break;

    case ID_COMMUNICATION_BROWSER:
#ifdef BROWSER_SUPPORT
        {
            MMIBROWSER_ENTRY_PARAM entry_param = {0};

            entry_param.type = MMIBROWSER_ACCESS_MAINMENU;
            entry_param.dual_sys = MN_DUAL_SYS_MAX;
            entry_param.url_ptr = PNULL;
            entry_param.user_agent_ptr = PNULL;
            
            MMIAPIBROWSER_Entry(&entry_param);
        }
#endif
        break;

    case ID_BLUETOOTH:
  MMIAPIBT_OpenMainMenuWin();   
  break;

     case ID_CUSTMER_SERVICE:
  MMIAPICS_OpenMainWin();    
  break;

     case ID_NET_MUSIC:
   MMIAPISET_EnterMusicWin();
   break;
     case ID_NET_FETION:
   MMIAPISET_EnterFetionWin();
   break;
     case  ID_CHINAMOBILE_MAP:
   MMIAPISET_EnterMapWin();
   break;

#ifdef GAME_SUPPORT
    case ID_ENTERTAIMENT_GAME:
        MMIAPIGAME_OpenMainMenuWindow();
        break;
#endif

     case ID_JAVA_STOCK:
               MMIAPISET_EnterStockWin();    
   break;
#endif 
#ifdef GAME_LINK_SUPPORT
    case IDGAME_MENU_LINK_ITEM_ID:
        MMIAPIGMLINK_OpenGameWin();
        break;
#endif

#ifdef GAME_BOXMAN_SUPPORT
    case IDGAME_MENU_BOXMAN_ITEM_ID:
        MMIAPIGMBOXMAN_OpenGameWin();
        break;
#endif

    case ID_SETTINGS_CALL:
        MMIAPISET_OpenCallSettingsWindow();
        break;

    case ID_SETTINGS_PHONE:
        MMIAPISET_EnterPhoneSettingWin();
        break;

    case ID_SETTINGS_DISPLAY:
        MMIAPISET_EnterDisplaySettingWin();
        break;

    case ID_SETTINGS_SECURITY:
        MMIAPISET_EnterSecuritySettingWin();
        break;

    case ID_SETTINGS_ENVIONMENT:
        MMIENVSET_OpenMainMenuWindow();
        break;
        
    case ID_SETTINGS_LINK:
        MMIAPICONNECTION_OpenMenuWin();
        break;
        
#ifdef  CMCC_UI_STYLE 
    case ID_SETTINGS_UPLMN:
         MMIENGTD_OpenUPLMNSettingWin();
         break;
#endif

#ifdef CMCC_UI_STYLE
#ifdef JAVA_SUPPORT
    case ID_FAVORITE_GOLDENHOUSE:
        MMIAPIJAVA_OpenMainMenu();
        break;
#endif
    case ID_FAVORITE_RING:
        MMIAPIFMM_OpenMusicFileList();
        break;
    case ID_FAVORITE_PIC:
        MMIAPIPICVIEW_OpenPicViewerExt(MMIPICVIEW_DOWNLOAD_PIC);
        break;
    case ID_FAVORITE_PHOTO:
        MMIAPIPICVIEW_OpenPicViewer();
        break;
     case ID_FAVORITE_VIDEO://xuefangjiang add 20101102
        MMIAPIFMM_OpenMovieFileList();
        break;
     case ID_SETTINGS_VERSION:
        MMIAPIENG_CreateShowVersionWin();
        break;
#endif

#ifdef QQ_SUPPORT
    case ID_SPREADTRUM_QQ_SUPPORT:
        MMIAPIQQ_OpenMainWin();
        break;
#endif

#ifdef PUSH_EMAIL_SUPPORT   //Added by yuanl  2010.5.7
    case ID_MAINMENU_PUSHMAIL:
        MMIAPIMAIL_Enter(MAIL_ENTER_MAINMENU, PNULL);
        break;
#endif /* PUSH_EMAIL_SUPPORT  */
 /*added by zack@20090915 for mrapp start*/
#ifdef MRAPP_SUPPORT
#ifdef MRAPP_COOKIE_OPEN
    case ID_MRAPP_COOKIE:
        MMIMRAPP_AppEntry(MRAPP_COOKIE);
        break;
#endif

#ifdef MRAPP_MRPSTORE_OPEN
    case ID_MRAPP_MRPSTORE:
        MMIMRAPP_AppEntry(MRAPP_MRPSTORE);
        break;
#endif

#ifdef MRAPP_SKYBUDDY_OPEN
		case ID_MRAPP_SKYBUDDY:
            MMIMRAPP_AppEntry(MRAPP_SKYBUDDY);
            break;
#endif

#ifdef __MMI_SKYQQ__
        case ID_MRAPP_QQ:
            MMIMRAPP_AppEntry(MRAPP_SKYQQ);
            break;
#else
#ifdef MRAPP_SDKQQ_OPEN
		case ID_MRAPP_QQ:
			MMIMRAPP_AppEntry(MRAPP_SDKQQ);
			break;
#endif
#endif

#ifdef MRAPP_MSN_OPEN
    case ID_MRAPP_MSN:
        MMIMRAPP_AppEntry(MRAPP_MSN);
        break;
#endif

#ifdef MRAPP_EBOOK_OPEN
	case ID_MRAPP_BOOKS:
		MMIMRAPP_AppEntry(MRAPP_EBOOK);
		break;		
#endif

#ifdef MRAPP_NETGAME_OPEN
   case ID_MRAPP_NETGAME:
        MMIMRAPP_AppEntry(MRAPP_NETGAME);
        break;
#endif

#ifdef MRAPP_TOOL_OPEN
    case ID_MRAPP_TOOL:
        MMIMRAPP_AppEntry(MRAPP_TOOL);
        break;
#endif

#ifdef MRAPP_TIANQI_OPEN
    case ID_MRAPP_TIANQI:
        MMIMRAPP_AppEntry(MRAPP_TIANQI);
        break;
#endif

#ifdef MRAPP_FETION_OPEN
    case ID_MRAPP_FETION:
        MMIMRAPP_AppEntry(MRAPP_FETION);
        break;
#endif

#ifdef MRAPP_XLWB_OPEN
    case ID_MRAPP_XLWB:
        MMIMRAPP_AppEntry(MRAPP_XLWB);
        break;
#endif

#ifdef MRAPP_KAIXIN_OPEN
   case ID_MRAPP_KAIXIN:
        MMIMRAPP_AppEntry(MRAPP_KAIXIN);
        break;
#endif

#ifdef MRAPP_FARM_OPEN
   case ID_MRAPP_FARM:
        MMIMRAPP_AppEntry(MRAPP_FARM);
        break;
#endif

#ifdef MRAPP_CLOCK_OPEN
    case ID_MRAPP_CLOCK:
        MMIMRAPP_AppEntry(MRAPP_CLOCK);
        break;
#endif

#ifdef MRAPP_FACEBOOK_OPEN
    case ID_MRAPP_FACEBOOK:
        MMIMRAPP_AppEntry(MRAPP_FACEBOOK);
        break;
#endif

#ifdef MRAPP_YAHOO_OPEN
    case ID_MRAPP_YAHOO:
        MMIMRAPP_AppEntry(MRAPP_YAHOO);
        break;
#endif


#ifdef MRAPP_TWITTER_OPEN
   case ID_MRAPP_TWITTER:
        MMIMRAPP_AppEntry(MRAPP_TWITTER);
        break;
#endif
 

#ifdef MRAPP_TALKCAT_OPEN
  	case ID_MRAPP_TALKCAT:
        MMIMRAPP_AppEntry(MRAPP_TALKCAT);
        break;
#endif

#ifdef MRAPP_SKIRT_OPEN
   	case ID_MRAPP_SKIRT:
        MMIMRAPP_AppEntry(MRAPP_SKIRT);
        break;
#endif

#ifdef MRAPP_BIRD_OPEN
   	case ID_MRAPP_BIRD:
        MMIMRAPP_AppEntry(MRAPP_BIRD);
        break;
#endif

#ifdef MRAPP_FRUIT_OPEN
   	case ID_MRAPP_FRUIT:
        MMIMRAPP_AppEntry(MRAPP_FRUIT);
        break;
#endif

#ifdef MRAPP_BYDR_OPEN
   	case ID_MRAPP_BYDR:
        MMIMRAPP_AppEntry(MRAPP_BYDR);
        break;
#endif

#endif
#ifdef DMENU_SUPPORT
   	case ID_DMENU:
        MMIDMENU_Entry(0);
        break;
#endif
	/*added by zack@20090915 for mrapp end*/
#ifdef MMI_AZAN_SUPPORT
     case ID_AZAN:
      MMIAPIAZAN_OpenMuSiLinPrayWin();
      break;
#endif

#ifdef MCARE_V31_SUPPORT
        case ID_TMC_APP_SUPPORT:
            MCareV31_Main_Entry();
            break;
        case ID_BRW_APP_SUPPORT:
            MCareV31_QQBrowser_Shortcut();
            break;
#endif

#ifdef WEATHER_SUPPORT
    case ID_MAINMENU_WEATHER:
        MMIAPIWEATHER_OpenMainWin();
        break;
#endif

#ifdef IKEYBACKUP_SUPPORT
    case ID_MAINMENU_IKEYBACKUP:
        MMIAPIIKB_OpenMainWin();
        break;
#endif

#ifdef WRE_SUPPORT
	case ID_TOOLS_WRE:
		{
			MMIWRE_Entry();
		}	
		break;
#ifdef WRE_CONTRA_SUPPORT  // 魂斗罗
	case ID_WRE_CONTRA:
            {
		  MMIWRE_StartAppEx(WRE_FIX_ID_CONTRA, NULL);
            }	
            break;
#endif
#ifdef WRE_SUPERMARIO_SUPPORT  // 超级玛丽
	case ID_WRE_SUPERMARIO:
            {
		  MMIWRE_StartAppEx(WRE_FIX_ID_SUPERMARIO, NULL);
            }	
            break;
#endif
#ifdef WRE_TSLAND_SUPPORT  // 冒险岛
	case ID_WRE_TSLAND:
            {
		  MMIWRE_StartAppEx(WRE_FIX_ID_TSLAND, NULL);
            }	
            break;
#endif
#endif
//NEWMS00173119
#ifdef BLUETOOTH_SUPPORT
    case ID_APPS_BT:
        MMIAPIBT_OpenMainMenuWin();
        break;
#endif

    default:
        result = MMI_RESULT_FALSE;
        break;
    }  
    return result;
}
/*****************************************************************************/
// Description : 二级菜单list控件处理函数
// Global resource dependence : 
// Author:jin.wang
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainSecondMenuListWinMsg(
                                              MMI_WIN_ID_T         win_id, 
                                              MMI_MESSAGE_ID_E     msg_id, 
                                              DPARAM               param
                                              )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    GUIMENU_GROUP_T *sc_menu_group_ptr = PNULL; //current menu group info
    MMI_MENU_GROUP_ID_T group_id = MENU_ICON_PASTIME;
    uint16 list_index = 0;
    GUILIST_NEED_ITEM_CONTENT_T * need_item_content_ptr = PNULL;
    uint16 static_item_cnt = 0;
    
    //SCI_TRACE_LOW:"HandleMainSecondMenuListWinMsg, msg_id = 0x%02x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MAINMENU_WIN_3134_112_2_18_2_32_54_2,(uint8*)"d", msg_id);
    
    switch (msg_id)
    {    
    case MSG_OPEN_WINDOW:    

#if defined(DYNAMIC_MAINMENU_SUPPORT)
        MMIMENU_DyGetAllItemData();
#endif

        // 添加list菜单项
        if (SecMenuList_Create(group_id))
        {        
            MMK_SetAtvCtrl(win_id,MAINMENUWIN_SECOND_LIST_CTRL_ID);
            GUIWIN_SetSoftkeyTextId(win_id, TXT_COMMON_OK, TXT_NULL, (MMI_TEXT_ID_T)STXT_RETURN, FALSE);      
        }
        else
        {
            MMK_CloseWin(win_id);
        }          
        break;
        
    case MSG_CTL_MIDSK:  
    case MSG_KEYDOWN_WEB:
    case MSG_CTL_PENOK:
    case MSG_APP_OK:        
    case MSG_CTL_OK:
        sc_menu_group_ptr = MMI_GetMenuGroup(group_id);

#if defined(DYNAMIC_MAINMENU_SUPPORT)
        if (PNULL != sc_menu_group_ptr)
        {
            static_item_cnt = sc_menu_group_ptr->item_count;
        }
#endif
        
        list_index = GUILIST_GetCurItemIndex(MAINMENUWIN_SECOND_LIST_CTRL_ID); 
        
#if defined(DYNAMIC_MAINMENU_SUPPORT)

        if (list_index >= static_item_cnt)
        {
            GUIMENU_MAINMENU_DY_ITEM_T item_info = {0};
            BOOLEAN is_get_info = FALSE;
            
            list_index -= static_item_cnt;

            is_get_info = MMIMENU_DyListGetRunInfo(list_index, &item_info);
            
            if (is_get_info
                && PNULL != item_info.ori_data_ptr
                && MMIMENU_DyIsRunInfoValid(&(item_info.ori_data_ptr->dynamic_menu_info)))
            {
                (item_info.ori_data_ptr->dynamic_menu_info.link_function_ptr)(
                    item_info.ori_data_ptr->dynamic_menu_info.param1ptr, 
                    item_info.ori_data_ptr->dynamic_menu_info.param2ptr);

                MMIMENU_DyFreeParmInfo(&item_info);
            }

            break;
        }
#endif
        if (PNULL != sc_menu_group_ptr)
        {
            HandleSecMenuStaticMsg(win_id,
                                   msg_id,
                                   sc_menu_group_ptr->item_ptr[list_index].menu_id);
        }
        
        break;
        
    case MSG_KEYDOWN_0:
    case MSG_KEYDOWN_1:
    case MSG_KEYDOWN_2:
    case MSG_KEYDOWN_3:
    case MSG_KEYDOWN_4:
    case MSG_KEYDOWN_5:
    case MSG_KEYDOWN_6:
    case MSG_KEYDOWN_7:
    case MSG_KEYDOWN_8:
    case MSG_KEYDOWN_9:
        list_index = GetSecondMenuListIndex(msg_id);
        
        if (list_index != (uint16)DYNAMIC_MAINMENU_DEFAULT_LIST_INDEX)
        {
            GUILIST_SetCurItemIndex(MAINMENUWIN_SECOND_LIST_CTRL_ID, list_index);
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            MMK_PostMsg(win_id, MSG_MAINMENU_UPDATE, &list_index, sizeof(uint16));
        }
        break;


    case MSG_MAINMENU_UPDATE:
        sc_menu_group_ptr = MMI_GetMenuGroup(group_id);
        
#if defined(DYNAMIC_MAINMENU_SUPPORT)
        if (PNULL != sc_menu_group_ptr)
        {
            static_item_cnt = sc_menu_group_ptr->item_count;
        }
#endif
        
        list_index = *(uint16*)param;
        
#if defined(DYNAMIC_MAINMENU_SUPPORT)
        if (list_index >= static_item_cnt)
        {
            GUIMENU_MAINMENU_DY_ITEM_T item_info = {0};
            BOOLEAN is_get_info = FALSE;
            
            list_index -= static_item_cnt;
            is_get_info = MMIMENU_DyListGetRunInfo(list_index, &item_info);
            
            if (is_get_info
                && PNULL != item_info.ori_data_ptr
                && MMIMENU_DyIsRunInfoValid(&(item_info.ori_data_ptr->dynamic_menu_info)))
            {
                (item_info.ori_data_ptr->dynamic_menu_info.link_function_ptr)(
                    item_info.ori_data_ptr->dynamic_menu_info.param1ptr, 
                    item_info.ori_data_ptr->dynamic_menu_info.param2ptr);
                
                MMIMENU_DyFreeParmInfo(&item_info);
            }

            break;
        }
#endif
        if (PNULL != sc_menu_group_ptr)
        {
            HandleSecMenuStaticMsg(win_id,
                                   msg_id,
                                   sc_menu_group_ptr->item_ptr[list_index].menu_id);
        }

        break;
          
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:     
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_LIST_NEED_ITEM_DATA:
        sc_menu_group_ptr = MMI_GetMenuGroup(group_id);

        //add text into list item
        need_item_content_ptr = (GUILIST_NEED_ITEM_CONTENT_T*)param;
        
        if (PNULL != need_item_content_ptr)
        {
            SecMenuList_SetItem(MAINMENUWIN_SECOND_LIST_CTRL_ID,
                need_item_content_ptr->item_index, 
                sc_menu_group_ptr);
        }
        
        break;

#if defined(DYNAMIC_MAINMENU_SUPPORT)
    case MSG_CTL_LIST_NEED_ITEM_CONTENT:
        sc_menu_group_ptr = MMI_GetMenuGroup(group_id);
        
        need_item_content_ptr = (GUILIST_NEED_ITEM_CONTENT_T*)param;
        
        if (PNULL != need_item_content_ptr)
        {
            SecMenuList_SetDyPic(need_item_content_ptr->item_index,
                need_item_content_ptr->item_content_index,
                MAINMENUWIN_SECOND_LIST_CTRL_ID,
                sc_menu_group_ptr,
                win_id);
        }
        break;
#endif
        
    case MSG_GET_FOCUS:

#if defined(DYNAMIC_MAINMENU_SUPPORT)
        MMIMENU_DyGetAllItemData();
#endif

        list_index = GUILIST_GetCurItemIndex(MAINMENUWIN_SECOND_LIST_CTRL_ID); 

        // 添加list菜单项数
        if (SecMenuList_Create(group_id))
        {        
            MMK_SetAtvCtrl(win_id,MAINMENUWIN_SECOND_LIST_CTRL_ID);
            GUIWIN_SetSoftkeyTextId(win_id, TXT_COMMON_OK, TXT_NULL, (MMI_TEXT_ID_T)STXT_RETURN, FALSE);      
            GUILIST_SetCurItemIndex(MAINMENUWIN_SECOND_LIST_CTRL_ID, list_index);
        }

        break;
        
    case MSG_CLOSE_WINDOW:
#if defined(DYNAMIC_MAINMENU_SUPPORT)
        MMIMENU_DyFreeData();
#endif 
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return (result);
}
#ifdef MMITHEME_MAINMENU_ARRANGE

/*****************************************************************************/
// Description : HandleMainMenuOptWinMsg
// Global resource dependence : 
// Author:xiaoqing.lu
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleMainMenuOptWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_MENU_GROUP_ID_T group_id = 0;
    MMI_MENU_ID_T menu_id = 0;
    GUIMENU_MOVE_STATE_T move_state = GUIMENU_STATE_MOVE_NONE;

    //SCI_TRACE_LOW:"HandleMainMenuOptWinMsg msg_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MAINMENU_WIN_3356_112_2_18_2_32_54_3,(uint8*)"d", msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id, MAIN_MAINMENU_OPT_CTRL_ID);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        GUIMENU_GetId(MAIN_MAINMENU_OPT_CTRL_ID,&group_id,&menu_id);
        switch (menu_id)
        {
        case ID_MAINMENU_OPT_ARRANGE:
            move_state = GUIMENU_STATE_MOVE_SET;
            MMK_PostMsg(MAIN_MAINMENU_WIN_ID, MSG_NOTIFY_MENU_SET_STATUS, &move_state, sizeof(GUIMENU_MOVE_STATE_T));
            break;

        case ID_MAINMENU_OPT_HELP:
            {
                MMI_STRING_T    string = {0};
                MMI_CTRL_ID_T   ctrl_id = MAIN_MAINMENU_HELP_CTRL_ID;

                MMK_CreateWin((uint32*)MAINMENU_HELP_WIN_TAB, PNULL);
                MMI_GetLabelTextByLang(TXT_MAINMENU_ARRANGE_HELP, &string);
                GUITEXT_SetString(ctrl_id,string.wstr_ptr,string.wstr_len,FALSE);
            }
            break;

        default:
            // 不可能有其他的菜单选项，除非又新加了，那得再加case
            SCI_PASSERT(FALSE,("HandleMainMenuOptWinMsg menu_id = %d", menu_id));   /*assert to do*/
        }
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

/*****************************************************************************/
// Description : HandleMainMenuMoveOptWinMsg
// Global resource dependence : 
// Author:xiaoqing.lu
// Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainMenuMoveOptWinMsg(
                                               MMI_WIN_ID_T        win_id, 
                                               MMI_MESSAGE_ID_E    msg_id, 
                                               DPARAM                param
                                               )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_MENU_GROUP_ID_T group_id = 0;
    MMI_MENU_ID_T menu_id = 0;
    GUIMENU_MOVE_STATE_T move_state = GUIMENU_STATE_MOVE_NONE;

    //SCI_TRACE_LOW:"HandleMainMenuMoveOptWinMsg msg_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MAINMENU_WIN_3422_112_2_18_2_32_55_4,(uint8*)"d", msg_id);
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id, MAIN_MAINMENU_MOVEOPT_CTRL_ID);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        GUIMENU_GetId(MAIN_MAINMENU_MOVEOPT_CTRL_ID,&group_id,&menu_id);
        switch (menu_id)
        {
        case ID_MAINMENU_OPT_HELP:
            {
                MMI_STRING_T    string = {0};
                MMI_CTRL_ID_T   ctrl_id = MAIN_MAINMENU_HELP_CTRL_ID;

                MMK_CreateWin((uint32*)MAINMENU_HELP_WIN_TAB, PNULL);
                MMI_GetLabelTextByLang(TXT_MAINMENU_HELP, &string);
                GUITEXT_SetString(ctrl_id,string.wstr_ptr,string.wstr_len,FALSE);
            }
            break;

        case ID_MAINMENU_OPT_EXIT:
            move_state = GUIMENU_STATE_MOVE_CANCEL;
            MMK_PostMsg(MAIN_MAINMENU_WIN_ID, MSG_NOTIFY_MENU_SET_STATUS, &move_state, sizeof(GUIMENU_MOVE_STATE_T));
            // GUIMENU_CancelMoveIcon(MAIN_MAINMENU_MOVEOPT_CTRL_ID);
            break;

        default:
            // 不可能有其他的菜单选项，除非又新加了，那得再加case
            SCI_PASSERT(FALSE,("HandleMainMenuMoveOptWinMsg menu_id = %d", menu_id)); /*assert to do*/
        }
        MMK_CloseWin(win_id);

        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

/*****************************************************************************/
// FUNCTION:     HandleMainMenuHelpWinMsg
// Description :   handle help win msg
// Global resource dependence : 
// Author:xiaoqing.lu
// Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMainMenuHelpWinMsg( 
                                            MMI_WIN_ID_T win_id, 
                                            MMI_MESSAGE_ID_E msg_id, 
                                            DPARAM param
                                            )
{
    MMI_RESULT_E    recode = MMI_RESULT_TRUE;
    // MMI_STRING_T    string = {0};
    MMI_CTRL_ID_T   ctrl_id = MAIN_MAINMENU_HELP_CTRL_ID;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
// MMI_GetLabelTextByLang(TXT_MAINMENU_HELP, &string);
// GUITEXT_SetString(ctrl_id,string.wstr_ptr,string.wstr_len,FALSE);
// 
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;
  
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}
#endif

/*****************************************************************************/
// FUNCTION:     HandleBrowerUrlMenu
// Description :   处理主菜单中通过browser 访问url的菜单项
// Global resource dependence : 
// Author: fen.xie
// Note:   
/*****************************************************************************/
LOCAL void HandleBrowerUrlMenu(MAINMENU_BROWSER_URL_TYPE_E browser_url_type)
{
#ifdef BROWSER_SUPPORT
    MMIBROWSER_ENTRY_PARAM entry_param = {0};

    if (browser_url_type >= BROWSER_URL_TYPE_MAX)
    {
        return;
    }

    entry_param.type = MMIBROWSER_ACCESS_URL;
    entry_param.dual_sys = MN_DUAL_SYS_MAX;
    entry_param.user_agent_ptr = PNULL;
    
    entry_param.url_ptr = (char *)s_mainmenu_browser_url[browser_url_type];
    MMIAPIBROWSER_Entry(&entry_param);
#endif
}


/*****************************************************************************/
// Description : 创建list
// Global resource dependence : 
// Author:jinwang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN SecMenuList_Create(MMI_MENU_GROUP_ID_T group_id)
{
    uint16 item_cnt = 0;
    uint16 list_index = 0;
    GUIMENU_GROUP_T *sc_menu_group_ptr = PNULL; //current menu group info
    uint32 dy_menu_count = 0;

    //读取静态菜单数据个数
    sc_menu_group_ptr = MMI_GetMenuGroup(group_id);
    
    if (PNULL != sc_menu_group_ptr)
    {
        item_cnt = sc_menu_group_ptr->item_count;
    }

#if defined(DYNAMIC_MAINMENU_SUPPORT)
    dy_menu_count = MMIMENU_DyGetItemCount();

    if (dy_menu_count > 0)//有动态菜单数据
    {
        item_cnt += (uint16)dy_menu_count;
    }
#endif

    if (0 == item_cnt)
    {
        return FALSE;
    }

    GUILIST_SetSyncDecodeAnim(MAINMENUWIN_SECOND_LIST_CTRL_ID, TRUE); 

    GUILIST_SetMaxItem(MAINMENUWIN_SECOND_LIST_CTRL_ID, item_cnt, TRUE);
    
#ifdef JAVA_SUPPORT
    GUILIST_SetDefaultIcon(MAINMENUWIN_SECOND_LIST_CTRL_ID, IMAGE_SECMENU_ICON_DYNAMIC_DEFAULT, IMAGE_SECMENU_ICON_DYNAMIC_DEFAULT);
#endif

    for (list_index = 0; list_index < item_cnt; list_index++)
    {
        GUILIST_ITEM_T listbox_item  = {0};
        GUILIST_ITEM_DATA_T item_data  = {0};

        //图标 + 文本文字
        listbox_item.item_style = GUIITEM_STYLE_ONE_LINE_ICON_TEXT; 
         
        listbox_item.item_data_ptr = &item_data;
        GUILIST_AppendItem (MAINMENUWIN_SECOND_LIST_CTRL_ID, &listbox_item); 
    }

    // Update listbox
    GUILIST_SetCurItemIndex(MAINMENUWIN_SECOND_LIST_CTRL_ID, 0);
    GUILIST_SetSelectedItem(MAINMENUWIN_SECOND_LIST_CTRL_ID, 0, TRUE);
    
    return TRUE;
}

/*****************************************************************************/
// Description : 给list控件传入资源
// Global resource dependence : 
// Author:jinwang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN SecMenuList_SetItem(
    MMI_CTRL_ID_T ctrl_id,         //in:control id
    uint16 item_index,     //in:item索引
    GUIMENU_GROUP_T *sc_menu_group_ptr
)
{
    BOOLEAN result = FALSE;
    GUILIST_ITEM_DATA_T item_data = {0};
    uint16 static_item_cnt = 0;
    BOOLEAN has_get_item = FALSE;
    
    if (PNULL != sc_menu_group_ptr)
    {
        static_item_cnt = sc_menu_group_ptr->item_count;
    }
    
#if defined(DYNAMIC_MAINMENU_SUPPORT)
    //动态菜单
    if (item_index >= static_item_cnt)
    {
        uint16 dy_item_index = 0;
        GUIMENU_MAINMENU_DY_ITEM_T item_info ={0};
        BOOLEAN is_get_info = FALSE;
        
        //计算出动态菜单的index
        dy_item_index = item_index - static_item_cnt;

        is_get_info = MMIMENU_DyListGetDispInfoEx(dy_item_index, &item_info);

        if (is_get_info && PNULL != item_info.ori_data_ptr)
        {
            //图片
            if (item_info.ori_data_ptr->dynamic_menu_info.has_icon)
            {
                item_data.item_content[0].item_data_type = GUIITEM_DATA_ANIM_DATA;
            }
            else
            {
                item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[0].item_data.image_id = IMAGE_SECMENU_ICON_DYNAMIC_DEFAULT;
            }
            
            //文本
            item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[1].item_data.text_buffer.wstr_ptr 
                = item_info.ori_data_ptr->dynamic_menu_info.text;
            item_data.item_content[1].item_data.text_buffer.wstr_len
                = (uint16)MIN(GUIMENU_DYMAINMENU_STR_MAX_NUM, 
            item_info.ori_data_ptr->dynamic_menu_info.text_len);

            has_get_item = TRUE;
        }
    }
    else
#endif
    if (static_item_cnt > 0 && PNULL != sc_menu_group_ptr)
    {
        //静态菜单
        //图片
        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = sc_menu_group_ptr->item_ptr[item_index].select_icon_id;
        //文本
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_ID;
        item_data.item_content[1].item_data.text_id
            = sc_menu_group_ptr->item_ptr[item_index].text_str_id;

        has_get_item = TRUE;
    }
    
    // 设置字符信息
    if (has_get_item)
    {
        result = GUILIST_SetItemData(ctrl_id,&item_data,item_index);
    }
    
    return result;
}

/*****************************************************************************/
// Description : 获取list控件被选中index
// Global resource dependence : 
// Author:jinwang
// Note:返回list控件被选中的list index
/*****************************************************************************/
LOCAL uint32 GetSecondMenuListIndex(MMI_MESSAGE_ID_E msg_id)
{
    uint32 list_index = 0;
    uint16 list_num = 0;
    
    list_num = GUILIST_GetTotalItemNum(MAINMENUWIN_SECOND_LIST_CTRL_ID);//list控件item总个数
    list_index = (uint32)(msg_id - MSG_APP_1);
    
    if (list_index >= list_num)
    {
        list_index = DYNAMIC_MAINMENU_DEFAULT_LIST_INDEX;
    }
    
    return list_index;
}

#if defined(DYNAMIC_MAINMENU_SUPPORT)
/*****************************************************************************/
// Description : 给list控件传入动态菜单图片信息
// Global resource dependence : 
// Author:jinwang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN SecMenuList_SetDyPic(
    uint16 item_index,  //控件索引
    uint16 item_content_index,//内容索引
    MMI_CTRL_ID_T ctrl_id,
    GUIMENU_GROUP_T *sc_menu_group_ptr,
    MMI_WIN_ID_T win_id
)
{
    uint16 static_item_cnt = 0;
    GUILIST_ITEM_DATA_T item_data = {0}; 
    GUIANIM_DATA_INFO_T anim_data = {0};
    BOOLEAN result = FALSE;
    uint16 dy_item_index = 0;
    GUIMENU_MAINMENU_DY_ITEM_T item_info ={0};
    BOOLEAN is_get_info = FALSE;

    if (PNULL != sc_menu_group_ptr)
    {
        static_item_cnt = sc_menu_group_ptr->item_count;
    }

    if (item_index < static_item_cnt)
    {
        return FALSE;
    }
    
    //计算出动态菜单的index
    dy_item_index = item_index - static_item_cnt;
    
    is_get_info = MMIMENU_DyListGetDispInfo(dy_item_index, &item_info);
           
    if (is_get_info
        && item_info.ori_data_ptr->dynamic_menu_info.has_icon
        && PNULL != item_info.ori_data_ptr
        && PNULL != item_info.ori_data_ptr->dynamic_menu_info.icon_ptr)
    {

        //图片数据
        item_data.item_content[0].item_data_type = GUIITEM_DATA_ANIM_DATA;
        item_data.item_content[0].item_data.anim_data_ptr = &anim_data;
        
        anim_data.is_bitmap = FALSE;
        anim_data.is_save_data = TRUE;
        anim_data.data_ptr = item_info.ori_data_ptr->dynamic_menu_info.icon_ptr;
        anim_data.data_size = item_info.ori_data_ptr->dynamic_menu_info.icon_datasize;
        
        GUILIST_SetItemContent(ctrl_id, 
                               &item_data.item_content[item_content_index], 
                               item_index, 
                               item_content_index);

        SCI_FREE(item_info.ori_data_ptr->dynamic_menu_info.icon_ptr);

        result = TRUE;
    }   

    return result;
}
#endif

#ifdef FLASH_SUPPORT
PUBLIC void MMIAPIMENU_EnterEntertainment(void)
{
    MMK_CreateWin((uint32 *)MAINMENU_ICON_PASTIME_WIN_TAB,PNULL);
}
PUBLIC void MMIAPIMENU_EnterTools(void)
{
    MMK_CreateWin((uint32 *)MAINMENU_ICON_TOOLS_WIN_TAB,PNULL);
}
PUBLIC void MMIAPIMENU_EnterSetting(void)
{
    MMK_CreateWin((uint32 *)MAINMENU_ICON_SETTINGS_WIN_TAB,PNULL);
}
PUBLIC void MMIAPIMENU_EnterPlayer(void)
{
    MMK_CreateWin((uint32 *)MAINMENU_ICON_PLAYER_WIN_TAB,PNULL);
}
#endif

