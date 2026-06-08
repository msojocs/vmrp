/*****************************************************************************
** File Name:      mmibrowser_wintable.c                                     *
** Author:          li.li                                                    *
** Date:           23/06/2009                                                *
** Copyright:      2007 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe the browser window table    *
*****************************************************************************/

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_app_browser_trc.h"
#include "window_parse.h"
//#include "sci_types.h" 
#include "mmi_module.h"
#include "mmi_position.h"
#include "guilabel.h"
#include "mmidisplay_data.h"
#include "brw_control_api.h"
#ifdef JS_SUPPORT
#include "brw_core_api.h"
#endif
#include "mmibrowser_wintable.h"
#include "mmibrowser_id.h"
//#include "mmibrowser_menutable.h"
#include "guibutton.h"
#include "browser_image.h"
#include "browser_text.h"
#include "mmipub.h"
#include "mmi_appmsg.h"
#include "guirichtext.h"
#include "guilistbox.h"
#include "mmibrowser_setting.h"
#include "mmifilearray_export.h"
#include "mmifmm_export.h"
#include "mmiconnection_export.h"
#include "cfl_wchar.h"
#include "guires.h"
#include "mmi_default.h"
#include "mmiidle_export.h"
#include "mmicom_time.h"
#include "mmk_timer.h"
#include "cfl_char.h"
#include "guiedit.h"
#include "guiim_base.h"
#include "browser_anim.h"
#include "guiform.h"
#include "guiownerdraw.h"
#include "guidropdownlist.h"
#include "cafcontrol.h"
#include "guictrl_api.h"
#include "mmistk_export.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
//#include "mmiebook_file.h"//MS00187401
#include "mmisd_export.h"
#include "mmibrowser_menutable.h"
#include "mmi_resource.h"
#ifdef DRM_SUPPORT
#include "mmidrm.h"
#endif
#include "mmiacc_text.h"
#include "mmialarm_image.h"
#include "mmialarm_text.h"
#include "mmibrowser_bookmark.h"
#include "mmifmm_image.h"
#include "mime_type.h"
#include "mmibrowser_func.h"
#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif
#include "mmk_tp.h"
#include "guisetlist.h"
#include "browser_manager_text.h"
#include "guitab.h"
#include "guiiconlist.h"
#include "guianim.h"
#include "browser_image.h"
#ifdef WIFI_SUPPORT
#include "mmiidle_statusbar.h"
#endif
#ifdef SNS_SUPPORT
#include "mmisns_srv.h"
#endif
#if defined(DYNAMIC_MAINMENU_SUPPORT)
#include "mmimenu_synchronize.h"
#endif
#ifdef MMI_SLIDE_SUPPORT
#include "mmi_slide.h"
#endif
#if defined(CSC_XML_SUPPORT)
#include "mmicus_export.h"
#endif

#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif

#include "Mmi_text.h"
#include "img_dec_interface.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                *
 **--------------------------------------------------------------------------*/
 //special UA define begin
#ifdef MRAPP_SUPPORT
#define MMIBROWSER_DEFAULT_DORADO_UA "Dorado WAP-Browser/1.0.0/powerplay/2"
#else
#define MMIBROWSER_DEFAULT_DORADO_UA "Dorado WAP-Browser/1.0.0"
#endif
#define MMIBROWSER_UA_SPECIAL                   "SonyEricssonK700c/R2AE SEMC-Browser/4.0.3 Profile/MIDP-2.0 Configuration/CLDC-1.1"
//#define MMIBROWSER_CMCC                         "MOCOR/1.0 DORADO/1.0 Release/10.1.2009 Profile/MIDP-1.0 Configuration/CLDC-1.0"
#define MMIBROWSER_CMCC     "HS-N51/1.0 Release/10.25.2009 Browser/NF3.5 Profile/MIDP-2.0 Config/CLDC-1.1"//使用HS的UA
#define MMIBROWSER_OVERSEA_UA   "MAUI WAP Browser"
#define MMIBROWSER_OVERSEA_UA_2 "MicromaxQ50/Q03C MAUI-browser/MIDP-2.0Configuration/CLDC-1.1"//MS00243828
//special UA define end

#define MMIBROWSER_MAX_USER_NAME_LEN            20
#define MMIBROWSER_MAX_PWD_LEN                  20

#define MMIBROWSER_PROGRESS_BAR_COLOR           0xFCC0 

#define MMIBROSER_KEY_REPEAT_INTERVAL           100   //100 ms
#define MMIBROSER_KEY_REPEAT_DELAY              500   //500 ms

#define MMIBROWSER_URL_INPUT_FORM_CTRL_HEIGHT   (38)
#ifdef MMI_PDA_SUPPORT
#define MMIBROWSER_POP_MENU_ITEM_DIMEN          3
#else
#define MMIBROWSER_POP_MENU_ITEM_DIMEN          2
#endif /* MMI_PDA_SUPPORT */
#define MMIBROWSER_DOWNLOAD_EXT_LEN             1024
#define MMIBROWSER_PHONE_NUM_LEN                40
#define MMIBROWSER_PROGRESS_BUF_LEN             50
#define MMIBROWSER_SMS_CONTENT_LEN                160

#define MMIBROWSER_ADDR_LABEL_FONT  SONG_FONT_16

#ifndef MMI_PDA_SUPPORT
#define SLIDE_MIN_SPACE 4
#define SLIDE_TIME     100
#define MMIBROWSER_FLING_MIN_VELOCITY    FLING_MIN_VELOCITY
#else
#define SLIDE_MIN_SPACE 3
#define SLIDE_TIME     30
#define MMIBROWSER_FLING_MIN_VELOCITY    100
#endif

#define MMIBROWSER_START_PAGE_ITEM_INFO_LEN     (20)
#define MMIBROWSER_STARTPAGE_FORM1_HEIGHT       (40)
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ARRAY)     (sizeof(ARRAY) / sizeof(ARRAY)[0])
#endif

#ifdef SNS_SUPPORT
LOCAL IMMISns *pMe  = PNULL;
#endif

#ifdef BROWSER_START_PAGE_THEME1//slide style
LOCAL MMI_SLIDE_HANDLE s_startpage_win_slide_handle = MMI_SLIDE_HANDLE_INVALID;
#endif 

#define MMIBROWSER_START_INDICATOR_DISTANCE     (1)

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
#ifndef WIN32
#define MMIBROWSER_FIX_HTML_URL "http://wap.51wre.com"
#else
#define MMIBROWSER_FIX_HTML_URL "http://172.16.14.173:8082"
#endif
#endif
/**--------------------------------------------------------------------------*
**                         TYPE AND CONSTANT                                *
**--------------------------------------------------------------------------*/
typedef enum
{
    ID_POPUP_MENU_SUB_DISCONNECT,
    ID_POPUP_MENU_SUB_CLEARCACHE,
    ID_POPUP_MENU_SUB_CLEARCOOKIE,
    ID_POPUP_MENU_SUB_SETTING,
    ID_POPUP_MENU_SUB_HELP,
    ID_POPUP_MENU_OPEN,
    ID_POPUP_MENU_CURR_PAGE,
    ID_POPUP_MENU_SEND_URL,
    ID_POPUP_MENU_BOOKMARK_LIST,
    ID_POPUP_MENU_HISTORY,
    ID_POPUP_MENU_HAS_SAVE_FILE,
    ID_POPUP_MENU_TOOL,
    ID_POPUP_MENU_EXIT,
    ID_POPUP_MENU_INPUTADD,
    ID_POPUP_MENU_OPEN_HOMEPAGE,
    ID_POPUP_MENU_STOP,
    ID_POPUP_MENU_FORWARD,
    ID_POPUP_MENU_BACKWARD,
    ID_POPUP_MENU_REFRESH,
    ID_POPUP_MENU_SET_MAIN,
    ID_POPUP_MENU_SAVE_BOOKMARK,
    ID_POPUP_MENU_SAVE_INTERNET_FILE,
    ID_POPUP_MENU_SUB_FILEINFO,
    ID_POPUP_MENU_SEARCH,
    ID_POPUP_MENU_SUB_CLEARKEY,
    ID_POPUP_MENU_CLEAR,
    ID_POPUP_MENU_CLEAR_SUB_ALL,
    ID_POPUP_MENU_NAV,//navigation
    ID_POPUP_MENU_SAVE,
    ID_POPUP_MENU_CLEAR_INPUT_HISTORY,
    ID_POPUP_MENU_SAVE_IMG,
#ifdef SNS_SUPPORT
    ID_POPUP_MENU_SHARE_TO_SNS,
    ID_POPUP_MENU_SHARE_TO_SINA,
    ID_POPUP_MENU_SHARE_TO_FACEBOOK,
    ID_POPUP_MENU_SHARE_TO_TIWTTER,
#endif
    ID_POPUP_MENU_ADD_TO_MAINMENU,
    ID_POPUP_MENU_MAX
}MMIBROWSER_OPTMENU_NODE_E;

typedef struct
{
    uint16	mcc;
    uint16	mnc;
    char*	ua_ptr;
}MCC_MNC_UA_T;

#ifdef BROWSER_TOOLBAR
typedef struct
{
    MMI_CTRL_ID_T               button_ctrl_id;
    MMI_IMAGE_ID_T              fg_image_id;
    MMI_IMAGE_ID_T              pressed_fg_image_id;
    MMI_IMAGE_ID_T              grayed_fg_image_id;
    GUIBUTTON_CALLBACK_FUNC     button_proc;
}MMIBROWSER_BUTTON_T;
#endif

typedef struct
{
    //GUI_BOTH_RECT_T status_bar;
    GUI_BOTH_RECT_T addr_bar;
    GUI_BOTH_RECT_T web_page;
    GUI_BOTH_RECT_T scroll;
#ifdef BROWSER_TOOLBAR
    GUI_BOTH_RECT_T toolbar;
#endif    
    GUI_BOTH_RECT_T addr_label;
    GUI_BOTH_RECT_T connect_anim;
    //GUI_BOTH_RECT_T soktkey;
}MMIBROWSER_SHOW_T;

//rong.gu cc select info
typedef struct _MMIBROWSER_CC_INFO_T_
{
    uint8 * phone_num_ptr;
    uint32 cc_option;
} MMIBROWSER_CC_INFO_T;

typedef enum
{
    MMIBROWSER_CC_OPTION_VOICE = 0x0001,
    MMIBROWSER_CC_OPTION_IP = 0x0001 << 1,
    MMIBROWSER_CC_OPTION_VIDEO = 0x0001 << 2,

    MMIBROWSER_CC_OPTION_MAX
}MMIBROWSER_CC_OPTION_E;
typedef enum
{
    PREPARE_DL_ERR_NONE,
    PREPARE_DL_ERR_FULL_NO_STORE,//选择的存储位置空间满
    PREPARE_DL_ERR_EXSIT,//要保持的文件名已存在
    PREPARE_DL_ERR_OTHER,//其他错误
    PREPARE_DL_ERR_MAX
}MMIBROWSER_PREPARE_DL_ERR_E;

typedef void (*SETTING_ITEM_FUNC)(void);

typedef struct
{
    GUI_POINT_T                 start_point;        //slide start point
    GUI_POINT_T                 prev_point;         //slide previous point
    MMK_TP_SLIDE_E              slide_state;        //slide state
    MMK_TP_SLIDE_E              tpdown_slide_state; //tp down slide state
    uint8                       slide_timer_id;     //slide timer id
    float                       fling_velocity;     //fling velocity
    BOOLEAN                     is_tp_down;         //is tp down
}MMIBROWSER_TP_MOVE_T;

typedef enum
{
    MMIBROWSER_SAVE_HANDLE_FOCUS,
    MMIBROWSER_SAVE_HANDLE_TP,
    MMIBROWSER_SAVE_HANDLE_MAX
}MMIBROWSER_SAVE_HANDLE_E;

#ifdef MMI_PDA_SUPPORT
typedef struct
{
    MMI_CTRL_ID_T   button_ctrl_id;
    MMI_TEXT_ID_T   text_id;
    GUIBUTTON_CALLBACK_FUNC     callback_func;
    MMI_IMAGE_ID_T          release_img_id;
    //MMI_IMAGE_ID_T          grayed_img_id;
    MMI_IMAGE_ID_T          pressed_img_id;
    MMI_CTRL_ID_T   label_ctrl_id;
    MMI_CTRL_ID_T   parent_form_ctrl_id;
} MMIBROWSER_BUTTON_INFO_T;
#endif

#ifdef BROWSER_OPERA_START_PAGE
typedef struct
{
    BOOLEAN is_add_new;
    uint16  favorite_index;
}MMIBROWSER_FAVORITE_EDIT_INFO_T;
#endif

#ifdef MMIDORADO_MY_NAV_SUPPORT
typedef struct
{
    BOOLEAN is_add_new;
    uint16  index;
}MMIBROWSER_MY_NAV_EDIT_INFO_T;
#endif

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
const uint32 s_menu_brw_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
    {ID_POPUP_MENU_FORWARD, TXT_COMMON_COMMON_FORWARD, IMAGE_BROWSER_OPTION_FORWARD},
    {ID_POPUP_MENU_BACKWARD, TXT_COMMON_COMMON_BACK, IMAGE_BROWSER_OPTION_BACK}, 
    {ID_POPUP_MENU_SAVE_BOOKMARK,TXT_BROWSER_ADD_BOOKMARK, IMAGE_COMMON_OPTMENU_ADD},
    {ID_POPUP_MENU_BOOKMARK_LIST,TXT_BOOKMARK,IMAGE_BROWSER_OPTION_BOOKMARK},
    {ID_POPUP_MENU_HISTORY,TXT_COMMON_HISTORY, IMAGE_BROWSER_OPTION_HISTORY},
    {ID_POPUP_MENU_OPEN_HOMEPAGE,TXT_COMMON_HOMEPAGE, 0},
#ifdef BROWSER_SEARCH_SUPPORT
    {ID_POPUP_MENU_SEARCH,STXT_SEARCH, 0},
#endif
    {ID_POPUP_MENU_HAS_SAVE_FILE,TXT_BROWSER_HAS_SAVE_FILE, 0},
    {ID_POPUP_MENU_ADD_TO_MAINMENU,TXT_BROWSER_ADD_TO_MAINMENU, 0},
    {ID_POPUP_MENU_SAVE,STXT_SAVE, 0},
#ifdef SNS_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_SNS, TXT_SHARE_TO_SNS, 0},
#endif    
    {ID_POPUP_MENU_SUB_FILEINFO,TXT_WWW_PAGE_PROPERTY, 0},
    {ID_POPUP_MENU_SEND_URL,TXT_WWW_SEND_LINK, 0},
    {ID_POPUP_MENU_SUB_SETTING, STXT_MAIN_SETTING, 0},
    {ID_POPUP_MENU_EXIT,STXT_EXIT, 0}
#else
    {ID_POPUP_MENU_OPEN,TXT_COMM_OPEN},
    {ID_POPUP_MENU_NAV,TXT_WWW_PAGE_PILOT},
    {ID_POPUP_MENU_ADD_TO_MAINMENU,TXT_BROWSER_ADD_TO_MAINMENU},
    {ID_POPUP_MENU_SAVE,STXT_SAVE},
#ifdef SNS_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_SNS, TXT_SHARE_TO_SNS},
#endif 
    //{ID_POPUP_MENU_CLEAR,TXT_BROWSER_CLEAR},
    {ID_POPUP_MENU_SEND_URL,TXT_WWW_SEND_LINK},
    {ID_POPUP_MENU_SUB_FILEINFO,TXT_WWW_PAGE_PROPERTY},
    {ID_POPUP_MENU_SUB_SETTING, STXT_MAIN_SETTING},
    {ID_POPUP_MENU_EXIT,STXT_EXIT}
#endif /* MMI_PDA_SUPPORT */
};
const uint32 s_menu_brw_open_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
    {ID_POPUP_MENU_OPEN_HOMEPAGE,TXT_COMMON_HOMEPAGE, IMAGE_NULL},
    {ID_POPUP_MENU_INPUTADD,TXT_INPUTURL, IMAGE_NULL},
#ifdef BROWSER_SEARCH_SUPPORT
    {ID_POPUP_MENU_SEARCH,STXT_SEARCH, IMAGE_NULL},
#endif
    {ID_POPUP_MENU_BOOKMARK_LIST,TXT_BOOKMARK, IMAGE_NULL},
    {ID_POPUP_MENU_HISTORY,TXT_COMMON_HISTORY, IMAGE_NULL},
    {ID_POPUP_MENU_HAS_SAVE_FILE,TXT_BROWSER_HAS_SAVE_FILE, IMAGE_NULL},
#else
    {ID_POPUP_MENU_OPEN_HOMEPAGE,TXT_COMMON_HOMEPAGE},
    {ID_POPUP_MENU_INPUTADD,TXT_INPUTURL},
#ifdef BROWSER_SEARCH_SUPPORT
    {ID_POPUP_MENU_SEARCH,STXT_SEARCH},
#endif
    {ID_POPUP_MENU_BOOKMARK_LIST,TXT_BOOKMARK},
    {ID_POPUP_MENU_HISTORY,TXT_COMMON_HISTORY},
    {ID_POPUP_MENU_HAS_SAVE_FILE,TXT_BROWSER_HAS_SAVE_FILE},
#endif
};

const uint32 s_menu_brw_nav_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
    {ID_POPUP_MENU_REFRESH, TXT_REFRESH, IMAGE_NULL},
    {ID_POPUP_MENU_STOP, STXT_STOP, IMAGE_NULL},
    {ID_POPUP_MENU_FORWARD, TXT_COMMON_COMMON_FORWARD, IMAGE_NULL},
    {ID_POPUP_MENU_BACKWARD, TXT_COMMON_COMMON_BACK, IMAGE_NULL}, 
#else
    {ID_POPUP_MENU_REFRESH, TXT_REFRESH},
    {ID_POPUP_MENU_STOP, STXT_STOP},
    {ID_POPUP_MENU_FORWARD, TXT_COMMON_COMMON_FORWARD},
    {ID_POPUP_MENU_BACKWARD, TXT_COMMON_COMMON_BACK}, 
#endif
};

const uint32 s_menu_brw_save_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
    {ID_POPUP_MENU_SET_MAIN,TXT_BROWSER_PDA_SAVE_AS_HOMEPAGE, IMAGE_NULL},
    {ID_POPUP_MENU_SAVE_INTERNET_FILE,TXT_BROWSER_SNAPSHOT_SAVE_FILE, IMAGE_NULL},
    {ID_POPUP_MENU_SAVE_IMG, TXT_COMMON_PIC, IMAGE_NULL},
#else
    {ID_POPUP_MENU_SET_MAIN,TXT_WWW_AS_HOMEPAGE},
    {ID_POPUP_MENU_SAVE_BOOKMARK,TXT_WWW_SAVE_BOOKMARK},
    {ID_POPUP_MENU_SAVE_INTERNET_FILE,TXT_WEBPAGE},
    {ID_POPUP_MENU_SAVE_IMG,TXT_COMMON_PIC},
#endif /* MMI_PDA_SUPPORT */
};

#ifdef SNS_SUPPORT
#if (defined(WRE_WEIBO_SUPPORT)) || (defined(WRE_FACEBOOK_SUPPORT)) || (defined(WRE_TWITTER_SUPPORT))
const uint32 s_menu_brw_share_sns_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
#ifdef WRE_WEIBO_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_SINA,TXT_SHARE_TO_SINA,IMAGE_NULL},
#endif
#ifdef WRE_FACEBOOK_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_FACEBOOK,TXT_SHARE_TO_FACEBOOK,IMAGE_NULL},
#endif
#ifdef WRE_TWITTER_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_TIWTTER,TXT_SHARE_TO_TWITTER,IMAGE_NULL},
#endif   
#else
#ifdef WRE_WEIBO_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_SINA,TXT_SHARE_TO_SINA},
#endif    
#ifdef WRE_FACEBOOK_SUPPORT
    {ID_POPUP_MENU_SHARE_TO_FACEBOOK,TXT_SHARE_TO_FACEBOOK},
#endif    
#ifdef WRE_TWITTER_SUPPORT    
    {ID_POPUP_MENU_SHARE_TO_TIWTTER,TXT_SHARE_TO_TWITTER},
#endif    
#endif /* MMI_PDA_SUPPORT */
};
#endif
#endif
const uint32 s_menu_brw_startpage_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
#ifndef BROWSER_OPERA_START_PAGE
    {ID_POPUP_MENU_OPEN_HOMEPAGE,TXT_COMMON_HOMEPAGE, IMAGE_BROWSER_OPTION_HOME},
    {ID_POPUP_MENU_SUB_SETTING, STXT_MAIN_SETTING, IMAGE_COMMON_OPTMENU_SET},
    //{ID_POPUP_MENU_CLEAR,TXT_BROWSER_PDA_CLEAR, IMAGE_COMMON_OPTMENU_REMOVE_ALL},
    {ID_POPUP_MENU_EXIT,STXT_EXIT, IMAGE_BROWSER_OPTION_EXIT}
#else
    {ID_POPUP_MENU_OPEN_HOMEPAGE,TXT_COMMON_HOMEPAGE, IMAGE_BROWSER_OPTION_HOME},
    {ID_POPUP_MENU_BOOKMARK_LIST,TXT_BOOKMARK, IMAGE_BROWSER_OPTION_BOOKMARK},
    {ID_POPUP_MENU_HISTORY,TXT_COMMON_HISTORY, IMAGE_BROWSER_OPTION_HISTORY},
    {ID_POPUP_MENU_HAS_SAVE_FILE,TXT_BROWSER_HAS_SAVE_FILE, IMAGE_BROWSER_OPTION_PAGE},
    //{ID_POPUP_MENU_CLEAR,TXT_BROWSER_PDA_CLEAR, IMAGE_COMMON_OPTMENU_REMOVE_ALL},
    {ID_POPUP_MENU_SUB_SETTING, STXT_MAIN_SETTING, IMAGE_COMMON_OPTMENU_SET},
#endif
#else
    {ID_POPUP_MENU_OPEN_HOMEPAGE,TXT_COMMON_HOMEPAGE},
    //{ID_POPUP_MENU_CLEAR,TXT_BROWSER_CLEAR},
    {ID_POPUP_MENU_SUB_SETTING, STXT_MAIN_SETTING},
    {ID_POPUP_MENU_EXIT,STXT_EXIT}
#endif /* MMI_PDA_SUPPORT */
};

#if 0
const uint32 s_menu_brw_clear_popupmenu_table[][MMIBROWSER_POP_MENU_ITEM_DIMEN] = 
{
#ifdef MMI_PDA_SUPPORT
    {ID_POPUP_MENU_CLEAR_INPUT_HISTORY, TXT_BROWSER_INPUT_HISTORY, IMAGE_NULL},
#ifdef BROWSER_SEARCH_SUPPORT
    {ID_POPUP_MENU_SUB_CLEARKEY, TXT_BROWSER_KEY, IMAGE_NULL},
#endif
    {ID_POPUP_MENU_SUB_CLEARCACHE, TXT_BROWSER_CACHE, IMAGE_NULL},
    {ID_POPUP_MENU_SUB_CLEARCOOKIE, TXT_BROWSER_COOKIE, IMAGE_NULL},
#else
    {ID_POPUP_MENU_CLEAR_INPUT_HISTORY, TXT_BROWSER_INPUT_HISTORY},
#ifdef BROWSER_SEARCH_SUPPORT
    {ID_POPUP_MENU_SUB_CLEARKEY, TXT_BROWSER_KEY},
#endif
    {ID_POPUP_MENU_SUB_CLEARCACHE, TXT_BROWSER_CACHE},
    {ID_POPUP_MENU_SUB_CLEARCOOKIE, TXT_BROWSER_COOKIE},
    {ID_POPUP_MENU_CLEAR_SUB_ALL, TXT_COMMON_ALL},
#endif
};
#endif

static const MCC_MNC_UA_T s_mcc_mnc_ua_table[] = 
{  //mcc   mnc       UA
    //{460,    0,  MMIBROWSER_CMCC        },    //PLMN_CMCC: China mobile
    {286,    2,  MMIBROWSER_OVERSEA_UA},//MS00219047
    {334,    3,  MMIBROWSER_OVERSEA_UA},//MS00222409
    {334,   20,  MMIBROWSER_OVERSEA_UA},//MS00219047
    {404,    4,  MMIBROWSER_OVERSEA_UA},   //MS00241449
    {404,    7,  MMIBROWSER_UA_SPECIAL},   //IDEA   India: Hyderabad
    {404,   10,  MMIBROWSER_OVERSEA_UA},   //MS00245237
    {404,   11,  MMIBROWSER_OVERSEA_UA_2},//MS00243828
    {404,   12,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   14,  MMIBROWSER_UA_SPECIAL},   //Vodafone  India:Chandigarh
    {404,   19,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   22,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   24,  MMIBROWSER_OVERSEA_UA},//MS00246294    
    {404,   25,  MMIBROWSER_OVERSEA_UA},   //NEWMS00164781
    {404,   44,  MMIBROWSER_UA_SPECIAL},   //IDEA  India:Bangalore 
    {404,   56,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   76,  MMIBROWSER_OVERSEA_UA}, //MS00236707
    {404,   78,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   82,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   87,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {404,   89,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,    5,  MMIBROWSER_OVERSEA_UA},//MS00242026    
    {405,    6,  MMIBROWSER_OVERSEA_UA},//MS00246294 
    {405,   52,  MMIBROWSER_OVERSEA_UA},   //NEWMS00164781
    {405,   70,  MMIBROWSER_OVERSEA_UA},   //NEWMS00164781
    {405,   83,  MMIBROWSER_OVERSEA_UA},   //NEWMS00115523
    {405,  799,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,  800,  MMIBROWSER_OVERSEA_UA},//MS00242026
    {405,  827,  MMIBROWSER_OVERSEA_UA},//MS00249682
    {405,  840,  MMIBROWSER_UA_SPECIAL},   //India: Videocon
    {405,  845,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,  846,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,  848,  MMIBROWSER_OVERSEA_UA},   //NEWMS00119010
    {405,  849,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,  850,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,  852,  MMIBROWSER_UA_SPECIAL},   //IDEA   India: Chennai
    {405,  853,  MMIBROWSER_UA_SPECIAL},   //IDEA
    {405,  876,  MMIBROWSER_OVERSEA_UA},   //NEWMS00164781
    {452,    1,  MMIBROWSER_OVERSEA_UA}, //MS00238830
    {452,    2,  MMIBROWSER_OVERSEA_UA}, //MS00238830
    {452,    5,  MMIBROWSER_OVERSEA_UA}, //MS00238830
    {510,    8,  MMIBROWSER_UA_SPECIAL},//NEWMS00188207
    {510,   11,  MMIBROWSER_OVERSEA_UA},
    {510,   89,  MMIBROWSER_UA_SPECIAL},//NEWMS00188207 (//MS00249718 MMIBROWSER_OVERSEA_UA is valid)
    {610,    2,  MMIBROWSER_OVERSEA_UA},   //BAMAKO: orange
    {621,   50,  MMIBROWSER_OVERSEA_UA},//MS00219047
    {639,    7,  MMIBROWSER_OVERSEA_UA},    //Kenya: orange
    {648,    4,  MMIBROWSER_OVERSEA_UA}, //NEWMS00113296
};

LOCAL MMIBROWSER_SHOW_T s_brw_show_info = 
{
    //{{0}, {0}},// status_bar;
    {{0}, {0}},// addr_bar;
    {{0}, {0}},// web_page;
    {{0}, {0}},// scroll;
#ifdef BROWSER_TOOLBAR
    {{0}, {0}},// toolbar;
#endif    
    {{0}, {0}},// addr_label;    
    {{0}, {0}}//connect_anim
};

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
PUBLIC MMIBROWSER_FIX_DISPLAY_T  s_fix_display_info = {0};
#endif

#ifdef MMI_PDA_SUPPORT
/*****************************************************************************/
//  Description : the callback of button21
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button21CallBack(void);

/*****************************************************************************/
//  Description : the callback of button22
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button22CallBack(void);

#if 0
/*****************************************************************************/
//  Description : the callback of button23
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button23CallBack(void);
#endif

/*****************************************************************************/
//  Description : the callback of button24
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button24CallBack(void);

LOCAL const MMIBROWSER_BUTTON_INFO_T s_start_page_button_info[] = 
{
    {MMIBROWSER_START_PAGE_BUTTON1_CTRL_ID, STXT_EXIT, Button21CallBack,IMAGE_BROWSER_OPTION_I_EXIT
        , IMAGE_BROWSER_OPTION_I_EXIT_FOCUS
        ,MMIBROWSER_START_PAGE_LABEL1_CTRL_ID, MMIBROWSER_START_PAGE_FORM21_CTRL_ID
        },
    {MMIBROWSER_START_PAGE_BUTTON2_CTRL_ID, STXT_MAIN_SETTING, Button22CallBack,IMAGE_BROWSER_OPTION_I_SET
        , IMAGE_BROWSER_OPTION_I_SET_FOCUS
        ,MMIBROWSER_START_PAGE_LABEL2_CTRL_ID, MMIBROWSER_START_PAGE_FORM22_CTRL_ID
        },
#if 0        
    {MMIBROWSER_START_PAGE_BUTTON3_CTRL_ID, TXT_BROWSER_PDA_CLEAR, Button23CallBack,IMAGE_BROWSER_OPTION_I_DEL
        , IMAGE_BROWSER_OPTION_I_DEL_FOCUS
        ,MMIBROWSER_START_PAGE_LABEL3_CTRL_ID, MMIBROWSER_START_PAGE_FORM23_CTRL_ID
        },
#endif        
    {MMIBROWSER_START_PAGE_BUTTON4_CTRL_ID, TXT_COMMON_HOMEPAGE, Button24CallBack,IMAGE_BROWSER_OPTION_I_HOME
        , IMAGE_BROWSER_OPTION_I_HOME_FOCUS
        ,MMIBROWSER_START_PAGE_LABEL4_CTRL_ID, MMIBROWSER_START_PAGE_FORM24_CTRL_ID
        }
};
#endif /* MMI_PDA_SUPPORT */


LOCAL MMIBROWSER_TP_MOVE_T s_browser_tp_info = {0};

#ifdef BROWSER_OPERA_START_PAGE
/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton11CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton12CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton13CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton21CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton22CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton23CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton31CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton32CallBack(void);

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton33CallBack(void);

LOCAL MMI_CTRL_ID_T   s_browser_favorite_button_ctrl[MMIBRW_FAVORIT_MAX_ITEM] =
{
    MMIBROWSER_FAVORITE_BUTTON11_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON21_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON31_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON12_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON22_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON32_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON13_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON23_CTRL_ID,
    MMIBROWSER_FAVORITE_BUTTON33_CTRL_ID
};

LOCAL MMI_CTRL_ID_T   s_browser_favorite_label_ctrl[MMIBRW_FAVORIT_MAX_ITEM] =
{
    MMIBROWSER_FAVORITE_LABEL11_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL21_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL31_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL12_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL22_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL32_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL13_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL23_CTRL_ID,
    MMIBROWSER_FAVORITE_LABEL33_CTRL_ID
};
LOCAL MMI_CTRL_ID_T   s_browser_favorite_child_form_ctrl[MMIBRW_FAVORIT_MAX_ITEM] =
{
    MMIBROWSER_FAVORITE_CHILD_FORM11_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM21_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM31_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM12_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM22_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM32_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM13_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM23_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM33_CTRL_ID
};

LOCAL GUIBUTTON_CALLBACK_FUNC   s_browser_favorite_button_cb[MMIBRW_FAVORIT_MAX_ITEM] =
{
    FavoriteButton11CallBack,
    FavoriteButton21CallBack,
    FavoriteButton31CallBack,
    FavoriteButton12CallBack,
    FavoriteButton22CallBack,
    FavoriteButton32CallBack,
    FavoriteButton13CallBack,
    FavoriteButton23CallBack,
    FavoriteButton33CallBack
};

LOCAL MMI_CTRL_ID_T   s_browser_favorite_form_ctrl[3] =
{
    MMIBROWSER_FAVORITE_CHILD_FORM1_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM2_CTRL_ID,
    MMIBROWSER_FAVORITE_CHILD_FORM3_CTRL_ID,
};

#endif

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
**                         EXTERNAL DECLARE                                 *
**--------------------------------------------------------------------------*/
#ifdef MMIDORADO_FIX_NAV_SUPPORT
extern MMIBRW_FIX_NAV_ITEM_T  g_fix_nav_default_info[];
#endif
/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : handle main window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMainWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

/*****************************************************************************/
//  Description : handle edit url window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleEditUrlWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

/*****************************************************************************/
//  Description : handle auth window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleAuthWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

/*****************************************************************************/
//  Description : handle file detail window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFileDetailWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           );

#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE) 
/*****************************************************************************/
//  Description : sim select callback
//  Global resource dependence : 
//  Author: li.li
//  Note: sam.hua @Dec27
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwSIMSelectCallback(uint32 dual_sys, BOOLEAN isReturnOK, DPARAM param);

#endif

/*****************************************************************************/
//  Description : handle input username window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleInputUserNameWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

#if 0
/*****************************************************************************/
//  Description : handle browser cc window message
//  Global resource dependence : 
//  Author: rong.gu
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleCCSelectWinMsg(
                                   MMI_WIN_ID_T win_id, 
                                   MMI_MESSAGE_ID_E msg_id,
                                   DPARAM param
                                   );
#endif

/*****************************************************************************/
// 	Description : ssl cert query window message handle
//	Global resource dependence : 
//  Author:
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwSSLCertQueryCallback(
                                        MMI_WIN_ID_T win_id, 	
                                        MMI_MESSAGE_ID_E msg_id, 
                                        DPARAM param
                                        );

/*****************************************************************************/
//  Description : handle control message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIBROWSER_HandleCtrlMsg(BRW_EXPORT_MSG_E message_id,void* param,uint16 size_of_param);


/*****************************************************************************/
//  Description : the callback of bookmark button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BookmarkButtonCallBack(void);

#ifdef BROWSER_TOOLBAR 
/*****************************************************************************/
//  Description : change the button image
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwChangeRefreshButtonImg(BOOLEAN is_loading, BOOLEAN is_need_update);

/*****************************************************************************/
//  Description : the callback of more button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E MoreButtonCallBack(void);

/*****************************************************************************/
//  Description : the callback of stop button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E StopButtonCallBack(void);


/*****************************************************************************/
//  Description : the callback of forward button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E ForwardButtonCallBack(void);

/*****************************************************************************/
//  Description : display toolbar bg
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayToolBarImg(MMI_WIN_ID_T win_id, BOOLEAN is_need_update);
                    
/*****************************************************************************/
// 	Description : change the backward button
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL void BrowserChangeButtonStatus(BOOLEAN is_update);

/*****************************************************************************/
//  Description : clear toolbar rect
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearToolBar(GUI_COLOR_T back_color);

/*****************************************************************************/
//  Description : the callback of forward button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HomeButtonCallBack(void);

#endif

/*****************************************************************************/
//  Description : the callback of back button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BackButtonCallBack(void);

/*****************************************************************************/
//  Description : check if touch the address bar
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwIsTouchAddressBar(GUI_POINT_T point);

/*****************************************************************************/
//  Description : set the auth window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetAuthWindow(MMI_CTRL_ID_T ctrl_id);

/*****************************************************************************/
//  Description : set the detail window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetFileDetailWindow(MMI_CTRL_ID_T ctrl_id);

/*****************************************************************************/
//  Description : check if touch the web page
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwIsTouchInWebPage(GUI_POINT_T point);

/*****************************************************************************/
//  Description : show error in the window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwShowErrMsg(BRW_ERROR_E error);

/*****************************************************************************/
//  Description : get the dafualt file name in download(if null by random, and combined the suffix by mime_type)
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwDlGetDefaultFileName(wchar *dest_file_name_ptr, //[IN/OUT]
        uint16 *dest_file_name_len_ptr,//[IN/OUT]
        wchar *res_name_ptr,
        BRW_MIME_TYPE_E mime_type
        );

/*****************************************************************************/
//  Description : handle popmenu window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandlePopMenuWinMsg(
                                        MMI_WIN_ID_T win_id, 
                                        MMI_MESSAGE_ID_E msg_id, 
                                        DPARAM param
                                        );

/*****************************************************************************/
//  Description : handle input password window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleInputPWDWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM param
                                   );

/*****************************************************************************/
//  Description : handle save media file window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleSaveMediaFileWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           );

/*****************************************************************************/
//  Description :sms export setting location win handle fun
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  BrwHandleSaveMediaFileOnlyStorageWinMsg(
                                                 MMI_WIN_ID_T win_id, 
                                                 MMI_MESSAGE_ID_E msg_id, 
                                                 DPARAM param
                                                 );

/*****************************************************************************/
//  Description : handle input file name window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleInputFileNameWinMsg(
                                          MMI_WIN_ID_T win_id,
                                          MMI_MESSAGE_ID_E msg_id,
                                          DPARAM param
                                          );

/*****************************************************************************/
//  Description : handle download media file window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleDownladMediaFileWinMsg(
                                       MMI_WIN_ID_T win_id, 
                                       MMI_MESSAGE_ID_E msg_id, 
                                       DPARAM param
                                       );

/*****************************************************************************/
//  Description : create scroll bar
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL uint32 BrwScrollBarCreate(uint32 win_handle, GUI_BOTH_RECT_T *rect_ptr , MMI_IMAGE_ID_T   bg_id);

LOCAL BOOLEAN CreatePopWinMenu(void);

#if 0
/*****************************************************************************/
//  Description : handle input file name window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL wchar* BrwGetFileNameExceptExt(wchar* file_name_ptr);
#endif

/*****************************************************************************/
//  Description : handle main window open win message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwHandleMainOpenWinMsg(MMI_WIN_ID_T	win_id);


/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwPositonInfo(MMI_WIN_ID_T	win_id);

/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwHorPositonInfo(MMI_WIN_ID_T win_id);

/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwVerPositonInfo(MMI_WIN_ID_T win_id);

/*****************************************************************************/
//  Description : display progress.MMIBROWSER_DOWNLOAD_PROGRESS_CTRL_ID的自绘制函数
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void MMIBROWSER_DisplayDownloadProgress(GUIOWNDRAW_INFO_T *owner_draw_ptr);
/*****************************************************************************/
//  Description : clear the window of browser(address bar and web_page---need update when on web)
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearAddrBar(GUI_COLOR_T back_color);
/*****************************************************************************/
//  Description : clear the window of browser(address bar and web_page---need update when on web)
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearWebpage(GUI_COLOR_T back_color);

/*****************************************************************************/
//  Description : display address bar bg
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayAdressBarImg(MMI_WIN_ID_T win_id);

/*****************************************************************************/
//  Description : display progress
//  Global resource dependence : 
//  Author: fenxie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayAddrLabelBg(MMI_WIN_ID_T win_id);

/*****************************************************************************/
//  Description : handle MSG_TP_PRESS_MOVE\MSG_TP_PRESS_DOWN\MSG_TP_PRESS_UP for browser main window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void MMIBROWSER_HandleTpPress(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param,
                                   BRW_INSTANCE brw_instance
                                   );

#if 0
/*****************************************************************************/
//  Description : handle MMIBROWSER_OpenCCSelectWin window
//  Global resource dependence : 
//  Author: rong.gu
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIBROWSER_OpenCCSelectWin(MMIBROWSER_CC_INFO_T *in_cc_info);
#endif
                                   
/*****************************************************************************/
//  Description : 准备开始下载:得到文件全路径...
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMIBROWSER_PREPARE_DL_ERR_E BrwPrepareToDL(void);

/*****************************************************************************/
//  Description : set control info for download media file window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetDownloadMediaFileWindow(void);

/*****************************************************************************/
//  Description : set downloading process label
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetProgressForDL(void);
                                   
#ifdef OMADL_SUPPORT
/*****************************************************************************/
//  Description : handle display DD property and query download
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleOMADLQueryWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );
#endif

/*****************************************************************************/
//  Description : set bookmark edit window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void SetDownloadFileNamePathWindow(MMI_WIN_ID_T win_id);

/*****************************************************************************/
// Description : Export Open file Callback
// Global resource dependence : none
// Author: dave.ruan
// Note:
/*****************************************************************************/
LOCAL void BrwOpenSetMediaFileNameBack(BOOLEAN is_success, FILEARRAY file_array);

/*****************************************************************************/
// 	Description :  Open label file Callback
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL void BrwOpenLabelCallBack(BOOLEAN is_success, FILEARRAY file_array, MMI_HANDLE_T ctrl_handle);
                                             
/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void AppendInputHistoryList(MMI_CTRL_ID_T ctrl_id, MMI_STRING_T *text_str);

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void AppendInputHistoryListData(MMI_HANDLE_T ctrl_handle, MMI_STRING_T *text_str, uint16 index);
											 
#ifdef BROWSER_START_PAGE_DEFAULT
/*****************************************************************************/
//  Description : handle Start page of the browser
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleStartPageWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );
#endif

/*****************************************************************************/
//  Description : handle popmenu (start page win) window message 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleStartPagePopMenuWinMsg(MMI_WIN_ID_T win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM param);

#if 0
/*****************************************************************************/
//  Description : Get Real device type
//  Global resource dependence : none
//  Author: dave.ruan
//  Note:
/*****************************************************************************/
LOCAL MMIFILE_DEVICE_E MMIBROWSER_GetRealDeviceTypeByText(MMI_STRING_T *path_value_str_ptr);
#endif

/*****************************************************************************/
// 	Description : to handle exit webpage query win
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleQueryExitWebWin(
                    MMI_WIN_ID_T win_id, 
                    MMI_MESSAGE_ID_E msg_id, 
                    DPARAM param
                    );

/*****************************************************************************/
// 	Description : to handle exit browser query win
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleQueryExitBrowserWin(
                    MMI_WIN_ID_T win_id, 
                    MMI_MESSAGE_ID_E msg_id, 
                    DPARAM param
                    );
                    
/*****************************************************************************/
//  Description : handle slide timer
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void HandleSlideTimer(MMI_WIN_ID_T win_id, BRW_INSTANCE brw_instance);

/*****************************************************************************/
//  Description : stop tp slide timer
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void BrowserStopTpSlideTimer(MMI_WIN_ID_T	win_id);

#ifdef MMI_PDA_SUPPORT
#if 0
/*****************************************************************************/
//  Description :
//  Global resource dependence : 
//  Author: Jiaoyou.wu
//  Note:
/*****************************************************************************/
LOCAL void BrwOpenPropertyWin(void);
#endif

/*****************************************************************************/
// Description :
// Global resource dependence : none
// Author:Jiaoyou.wu
// Note:    
/*****************************************************************************/
LOCAL MMI_RESULT_E  BrwHandleAlertWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

#endif /* MMI_PDA_SUPPORT */

#ifdef BROWSER_START_PAGE_DEFAULT
#ifndef BROWSER_BOOKMARK_HISTORY_TAB
/*****************************************************************************/
//  Description : UpdateStartPageListInfo
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void UpdateStartPageListInfo(void);
#endif
#endif

/*****************************************************************************/
//  Description : handle save wait window message for save local resource in one web page
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleSaveLocalWaitWin(
                                       MMI_WIN_ID_T win_id, 
                                       MMI_MESSAGE_ID_E msg_id, 
                                       DPARAM param
                                       );

/*****************************************************************************/
//  Description : save callback func for local resouce(img...) in web page
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void EnterFileNmForLocalCbf(uint32 handle,const uint16* file_name_ptr,uint16 file_name_len, BOOLEAN result);

#ifdef BROWSER_OPERA_START_PAGE
/*****************************************************************************/
//  Description : handle opera Start page of the browser
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleOperaStartPageWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );
                                   
/*****************************************************************************/
//  Description : handle favorite link edit window message
//  Global resource dependence : 
//  Author: lfen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFavoriteEditWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM param
                                   );
/*****************************************************************************/
//  Description :
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFavoritePopUpWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*****************************************************************************/
//  Description : handle file detail window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFavoriteDetailWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           );
#endif


#ifdef BROWSER_BOOKMARK_HISTORY_TAB
/*****************************************************************************/
// 	Description : to handle tab parent window message
//	Global resource dependence : 
//  Author:
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleBkAndHistoryWindow(
                    MMI_WIN_ID_T win_id, 
                    MMI_MESSAGE_ID_E msg_id, 
                    DPARAM param
                    );
#endif

#ifdef MMIDORADO_MY_NAV_SUPPORT
/*****************************************************************************/
//  Description : handle my navigation edit window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMyNavEditWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

/*****************************************************************************/
//  Description : handle my navigation popup menu window message
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMyNavPopUpWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

/*****************************************************************************/
//  Description : handle my navigation detail window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMyNavDetailWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );
/*****************************************************************************/
//  Description : OPEN one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void OpenOneMyNavUrl(MMI_CTRL_ID_T ctrl_id);
/*****************************************************************************/
//  Description : remove one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN RemoveOneMyNav(MMI_CTRL_ID_T ctrl_id);

/*****************************************************************************/
//  Description : append the my navigation info to the window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void AppendMyNavInfo(MMI_CTRL_ID_T ctrl_id);

/*****************************************************************************/
//  Description : append the my navigation info to the window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void AppendOneMyNavInfo(MMI_CTRL_ID_T ctrl_id, 
            uint16 my_nav_index, 
            uint16 icon_index,
            BOOLEAN is_append_text,
            BOOLEAN is_append_ico
            );

#ifndef MMI_PDA_SUPPORT
/*****************************************************************************/
//  Description : open Boutique And my Navigation window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenBoutiqueAndNavWin(void);

/*****************************************************************************/
//  Description : handle boutique and navigation window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleBoutiqueAndNavWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           );
#endif
#endif

#ifdef BROWSER_START_PAGE_THEME1
/*****************************************************************************/
//  Description : handle Start page of the browser
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleStartPageTheme1WinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

#endif

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
/*****************************************************************************/
//  Description : handle Fix Html window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFixHtmlWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );
#if 0
/*****************************************************************************/
//  Description : handle wait window message for update the Fix Html html page
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleUpdateFixHtmlWaitWin(
                                       MMI_WIN_ID_T win_id, 
                                       MMI_MESSAGE_ID_E msg_id, 
                                       DPARAM param
                                       );
#endif
/*****************************************************************************/
//  Description : handle control message:异步asynchronous
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleFixedInstanceControlMsg(MMI_MESSAGE_ID_E msg_id,void* param);

/*****************************************************************************/
//  Description : open Top Categorise window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenFixHtmlWin(void);
#endif

/*****************************************************************************/
//  Description : handle add to mainmenu edit window message
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleAdd2MainEditWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   );

/**--------------------------------------------------------------------------*
 **                         WINDOW DEFINITION                              *
 **--------------------------------------------------------------------------*/
#ifdef BROWSER_TOOLBAR
LOCAL const MMIBROWSER_BUTTON_T s_button_ctrl_id[] = 
{
    {MMIBROWSER_TOOLBAR_1_BUTTON_CTRL_ID, IMAGE_BROWSER_BACKWARD, IMAGE_BROWSER_PRESSED_BACKWARD, IMAGE_BROWSER_BACKWARD, BackButtonCallBack},

    {MMIBROWSER_TOOLBAR_2_BUTTON_CTRL_ID, IMAGE_BROWSER_FORWARD, IMAGE_BROWSER_PRESSED_FORWARD, IMAGE_BROWSER_FORWARD_GRAY, ForwardButtonCallBack},
    
    {MMIBROWSER_TOOLBAR_3_BUTTON_CTRL_ID, IMAGE_BROWSER_MORE, IMAGE_BROWSER_PRESSED_MORE, IMAGE_BROWSER_MORE, MoreButtonCallBack},

    {MMIBROWSER_TOOLBAR_4_BUTTON_CTRL_ID, IMAGE_BROWSER_REFRESH, IMAGE_BROWSER_PRESSED_REFRESH, IMAGE_BROWSER_REFRESH, StopButtonCallBack},

    {MMIBROWSER_TOOLBAR_5_BUTTON_CTRL_ID, IMAGE_BROWSER_HOME, IMAGE_BROWSER_PRESSED_HOME, IMAGE_BROWSER_HOME_GRAY, HomeButtonCallBack},
};
#endif

//the browser main window
WINDOW_TABLE(MMIBROWSER_MAIN_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleMainWinMsg),
    WIN_ID(MMIBROWSER_MAIN_WIN_ID),
    WIN_STATUSBAR,
#ifndef BROWSER_TOOLBAR
        WIN_SOFTKEY(STXT_OPTION, TXT_NULL, TXT_COMMON_CLOSE),
#endif
    //address bar
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT,MMIBROWSER_LABEL_ADDRESS_CTRL_ID),

    CREATE_ANIM_CTRL(MMIBROWSER_ANIMATE_CONNECTING_CTRL_ID,MMIBROWSER_MAIN_WIN_ID),
    
#ifdef BROWSER_TOOLBAR
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_SBS,MMIBROWSER_BUTTON_FORM_CTRL_ID),

    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_BACKWARD,MMIBROWSER_TOOLBAR_1_BUTTON_CTRL_ID, MMIBROWSER_BUTTON_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FORWARD, MMIBROWSER_TOOLBAR_2_BUTTON_CTRL_ID, MMIBROWSER_BUTTON_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_MORE,    MMIBROWSER_TOOLBAR_3_BUTTON_CTRL_ID, MMIBROWSER_BUTTON_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_REFRESH, MMIBROWSER_TOOLBAR_4_BUTTON_CTRL_ID, MMIBROWSER_BUTTON_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_HOME,    MMIBROWSER_TOOLBAR_5_BUTTON_CTRL_ID, MMIBROWSER_BUTTON_FORM_CTRL_ID),
#endif
    END_WIN
}; 

//the edit url window
WINDOW_TABLE(MMIBROWSER_INPUT_URL_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleEditUrlWinMsg),    
    WIN_ID(MMIBROWSER_ADDRESS_WIN_ID),
    WIN_STATUSBAR,
#ifndef MMI_PDA_SUPPORT
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#endif    
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMIBROWSER_URL_INPUT_FORM_CTRL_ID),
        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_SBS, MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID, MMIBROWSER_URL_INPUT_FORM_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBROWSER_MAX_URL_LEN, 
                                            MMIBROWSER_URL_INPUT_CHILD_CTRL_ID, MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID),
#ifdef TOUCH_PANEL_SUPPORT
            CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_LINK,MMIBROWSER_URL_INPUT_LINK_CHILD_CTRL_ID, MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID),
#endif
        CHILD_LIST_CTRL(TRUE, GUILIST_TEXTLIST_E, MMIBROWSER_URL_INPUT_LIST_CHILD_CTRL_ID, MMIBROWSER_URL_INPUT_FORM_CTRL_ID),

#ifdef MMI_PDA_SUPPORT
        CHILD_SOFTKEY_CTRL(TXT_NULL, STXT_RETURN, TXT_NULL, MMICOMMON_SOFTKEY_CTRL_ID,MMIBROWSER_URL_INPUT_FORM_CTRL_ID),
#endif

    END_WIN
};


//the auth window
WINDOW_TABLE(MMIBROWSER_AUTH_WIN_TAB) = 
{
    WIN_TITLE(TXT_BROWSER_NEEDAUTH),
    WIN_FUNC((uint32)BrwHandleAuthWinMsg),    
    WIN_ID(MMIBROWSER_AUTH_WIN_ID),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_BROWSER_CHANGE, STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */

    CREATE_RICHTEXT_CTRL(MMIBROWSER_RICHTEXT_AUTH_CTRL_ID),
    
    END_WIN
};

//the file detail window
WINDOW_TABLE(BRW_FILE_DETAIL_WIN_TAB) = 
{
    WIN_TITLE(TXT_WWW_PAGE_PROPERTY),
    WIN_FUNC((uint32)BrwHandleFileDetailWinMsg),    
    WIN_ID(MMIBROWSER_FILE_DETAIL_WIN_ID),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    CREATE_RICHTEXT_CTRL(MMIBROWSER_FILE_DETAIL_CTRL_ID),
    
    END_WIN
};
#if defined(WIFI_SUPPORT) || !defined (MMI_MULTI_SIM_SYS_SINGLE)
/*  //Sam.hua	use SelectSimFunc 
//the select sim window
WINDOW_TABLE( MMIBROWSER_SELECTSIM_WIN_TAB ) = 
{
    WIN_TITLE(TXT_BROWSER_SIM_SELECT),
    WIN_FUNC((uint32)BrwHandleSIMSelectWinMsg),    
    WIN_ID(MMIBROWSER_SELECTSIM_WIN_ID),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E,MMIBROWSER_SELECT_SIM_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
*/
#endif

WINDOW_TABLE(MMIBROWSER_INPUT_FILE_NAME_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleInputFileNameWinMsg ),    
    WIN_ID(MMIBROWSER_INPUT_FILE_NAME_WIN_ID),
    WIN_TITLE(TXT_COMMON_NAME),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_EDIT_TEXT_CTRL(MMIBROWSER_DL_FILE_NAME_MAX_LEN,MMIBROWSER_INPUT_FILE_NAME_CTRL_ID),
    END_WIN
};

WINDOW_TABLE( MMIBROWSER_MORE_POPMENU_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)BrwHandlePopMenuWinMsg),
    WIN_ID(MMIBROWSER_MAIN_POPMENU_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
    END_WIN
};

WINDOW_TABLE(MMIBROWSER_INPUT_USERNAME_WIN_TAB) = 
{
    WIN_TITLE(TXT_COMMON_USERNAME),
    WIN_FUNC((uint32)BrwHandleInputUserNameWinMsg),    
    WIN_ID(MMIBROWSER_USERNAME_WIN_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_EDIT_TEXT_CTRL(MMIBROWSER_MAX_USER_NAME_LEN,MMIBROWSER_EDIT_USERNAME_CTRL_ID),
    
    END_WIN
};

WINDOW_TABLE(MMIBROWSER_INPUT_PWD_WIN_TAB) = 
{
    WIN_TITLE(TXT_COMMON_PASSWORD),
    WIN_FUNC((uint32)BrwHandleInputPWDWinMsg),    
    WIN_ID(MMIBROWSER_PWD_WIN_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_EDIT_TEXT_CTRL(MMIBROWSER_MAX_PWD_LEN,MMIBROWSER_EDIT_PWD_CTRL_ID),
    END_WIN
};

WINDOW_TABLE( MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_TAB ) = 
{ 
    WIN_FUNC( (uint32)BrwHandleDownladMediaFileWinMsg ), 
    WIN_ID( MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID ),
    WIN_TITLE(TXT_BROWSER_DOWNLOAD),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER,MMIBROWSER_DOWNLOAD_FORM_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMIBROWSER_DOWNLOAD_LABEL1_CTRL_ID, MMIBROWSER_DOWNLOAD_FORM_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMIBROWSER_DOWNLOAD_LABEL2_CTRL_ID, MMIBROWSER_DOWNLOAD_FORM_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMIBROWSER_DOWNLOAD_LABEL3_CTRL_ID, MMIBROWSER_DOWNLOAD_FORM_CTRL_ID),
        CHILD_OWNDRAW_CTRL(FALSE, MMIBROWSER_DOWNLOAD_PROGRESS_CTRL_ID, MMIBROWSER_DOWNLOAD_FORM_CTRL_ID, MMIBROWSER_DisplayDownloadProgress),/*lint !e64*/
    
    WIN_STYLE(WS_DISABLE_HWICON | WS_DISABLE_RETURN_WIN),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_CANCEL),
    END_WIN
};

#if 0
//the window for CC select menu
WINDOW_TABLE(MMIBROWSER_CC_SELECT_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleCCSelectWinMsg),
    WIN_TITLE(TXT_NULL),
    WIN_ID(MMIBROWSER_CC_SELECT_WIN_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};
#endif

//save media file download path
WINDOW_TABLE( BRW_SAVE_MEDIAFILE_WIN_TAB ) = 
{
    WIN_FUNC((uint32)BrwHandleSaveMediaFileWinMsg),    
    WIN_ID(MMIBROWSER_SAVE_MEDIAFILE_WIN_ID),
    WIN_TITLE(TXT_COMM_SAVE_MEDIA_FILE),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_BROWSER_CHANGE, STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */
    
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER,MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID),

    CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,FALSE,MMIBROWSER_SAVE_MEDIAFILE_LABEL1_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,TRUE,MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID),
        //CHILD_EDIT_TEXT_CTRL(TRUE,MMIBROWSER_MAX_URL_LEN,MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID),
    
    CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,FALSE,MMIBROWSER_SAVE_MEDIAFILE_LABEL2_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,TRUE,MMIBROWSER_SAVE_MEDIAFILE_PATH_LABEL_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID),
    END_WIN
};

WINDOW_TABLE(BRW_SAVE_MEDIAFILE_ONLY_STORAGE_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleSaveMediaFileOnlyStorageWinMsg),    
    WIN_ID(MMIBROWSER_SAVE_MEDIAFILE_WIN_ID),
    WIN_TITLE(TXT_COMM_SAVE_MEDIA_FILE),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_BROWSER_CHANGE, STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */
    
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER,MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID),

    CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,FALSE,MMIBROWSER_SAVE_MEDIAFILE_LABEL1_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,TRUE,MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID),
    
    CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,FALSE,MMIBROWSER_SAVE_MEDIAFILE_LABEL2_CTRL_ID,MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID),


#ifdef MMI_PDA_SUPPORT
        CHILD_SETLIST_CTRL(TRUE,MMIBROWSER_SAVE_MEDIAFILE_PATH_DROPDOWNLIST_CTRL_ID, MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID),
#else
        CHILD_DROPDOWNLIST_CTRL(TRUE, MMIBROWSER_SAVE_MEDIAFILE_PATH_DROPDOWNLIST_CTRL_ID, MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID),
#endif /* MMI_PDA_SUPPORT */
        END_WIN
};

#ifdef OMADL_SUPPORT
//the window for DD property and query download
WINDOW_TABLE(MMIBROWSER_OMADL_QUERY_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleOMADLQueryWinMsg),
    WIN_TITLE(TXT_DL_OMA_DL_QUERY),
    WIN_ID(MMIBROWSER_OMADL_QUERY_WIN_ID),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
#endif /* MMI_PDA_SUPPORT */
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_RICHTEXT_CTRL(MMIBROWSER_OMADL_QUERY_RICHTEXT_CTRL_ID),
    END_WIN
};
#endif

#ifdef BROWSER_START_PAGE_DEFAULT
WINDOW_TABLE(MMIBROWSER_START_PAGE_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleStartPageWinMsg),
    WIN_ID(MMIBROWSER_START_PAGE_WIN_ID),
    WIN_STATUSBAR,
    WIN_SOFTKEY(STXT_OPTION, TXT_NULL, STXT_EXIT),

    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM_CTRL_ID),
        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_SBS, MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_FORM_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, TRUE, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
#ifdef BROWSER_SEARCH_SUPPORT
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, TRUE, MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
#endif
        CHILD_LIST_CTRL(TRUE, GUILIST_TEXTLIST_E, MMIBROWSER_START_PAGE_LIST_CTRL_ID, MMIBROWSER_START_PAGE_FORM_CTRL_ID),
    END_WIN
}; 

#elif defined(BROWSER_START_PAGE_THEME1)

WINDOW_TABLE(MMIBROWSER_START_PAGE_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleStartPageTheme1WinMsg),
    WIN_ID(MMIBROWSER_START_PAGE_WIN_ID),
    WIN_STATUSBAR,
    WIN_SOFTKEY(STXT_OPTION, TXT_NULL, STXT_EXIT),
    
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_SBS, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
        CHILD_BUTTON_CTRL(TRUE, IMAGE_BROWSER_BOOKMARK_LIST, MMIBROWSER_START_PAGE_BUTTON5_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, TRUE, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
#ifdef BROWSER_SEARCH_SUPPORT
        CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, TRUE, MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
#endif
    CREATE_ICONLIST_CTRL(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID),
  
    //softkey
#if MMI_PDA_SUPPORT    
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_SBS, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM21_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
            CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_OPTION_I_EXIT, MMIBROWSER_START_PAGE_BUTTON1_CTRL_ID, MMIBROWSER_START_PAGE_FORM21_CTRL_ID),
            CHILD_LABEL_CTRL(ALIGN_HMIDDLE , FALSE, MMIBROWSER_START_PAGE_LABEL1_CTRL_ID, MMIBROWSER_START_PAGE_FORM21_CTRL_ID),

        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM22_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
            CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_OPTION_I_SET, MMIBROWSER_START_PAGE_BUTTON2_CTRL_ID, MMIBROWSER_START_PAGE_FORM22_CTRL_ID),
            CHILD_LABEL_CTRL(ALIGN_HMIDDLE , FALSE, MMIBROWSER_START_PAGE_LABEL2_CTRL_ID, MMIBROWSER_START_PAGE_FORM22_CTRL_ID),

#if 0
        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM23_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
            CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_OPTION_I_DEL, MMIBROWSER_START_PAGE_BUTTON3_CTRL_ID, MMIBROWSER_START_PAGE_FORM23_CTRL_ID),
            CHILD_LABEL_CTRL(ALIGN_HMIDDLE , FALSE, MMIBROWSER_START_PAGE_LABEL3_CTRL_ID, MMIBROWSER_START_PAGE_FORM23_CTRL_ID),
#endif

        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM24_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
            CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_OPTION_I_HOME, MMIBROWSER_START_PAGE_BUTTON4_CTRL_ID, MMIBROWSER_START_PAGE_FORM24_CTRL_ID),
            CHILD_LABEL_CTRL(ALIGN_HMIDDLE , FALSE, MMIBROWSER_START_PAGE_LABEL4_CTRL_ID, MMIBROWSER_START_PAGE_FORM24_CTRL_ID),
#endif

    END_WIN
}; 

#elif defined(BROWSER_OPERA_START_PAGE)//opera

WINDOW_TABLE(MMIBROWSER_START_PAGE_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleOperaStartPageWinMsg),
    WIN_SUPPORT_ANGLE( WIN_SUPPORT_ANGLE_INIT),
    WIN_ID(MMIBROWSER_START_PAGE_WIN_ID),
    WIN_STATUSBAR,
    WIN_SOFTKEY(STXT_OPTION, TXT_NULL, STXT_EXIT),

    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM_CTRL_ID),
        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_SBS, MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_FORM_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
#ifdef BROWSER_SEARCH_SUPPORT
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID),
#endif

        CHILD_OWNDRAW_CTRL(FALSE, MMIBROWSER_START_PAGE_FORM3_CTRL_ID, MMIBROWSER_START_PAGE_FORM_CTRL_ID, PNULL),

        CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_START_PAGE_FORM2_CTRL_ID, MMIBROWSER_START_PAGE_FORM_CTRL_ID),
            CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_SBS, MMIBROWSER_FAVORITE_CHILD_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM11_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM1_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON11_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM11_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL11_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM11_CTRL_ID),

                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM12_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM1_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON12_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM12_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL12_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM12_CTRL_ID),

                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM13_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM1_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON13_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM13_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL13_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM13_CTRL_ID),

            CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_SBS, MMIBROWSER_FAVORITE_CHILD_FORM2_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM21_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM2_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON21_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM21_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL21_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM21_CTRL_ID),

                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM22_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM2_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON22_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM22_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL22_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM22_CTRL_ID),

                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM23_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM2_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON23_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM23_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL23_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM23_CTRL_ID),
                
            CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_SBS, MMIBROWSER_FAVORITE_CHILD_FORM3_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID),
                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM31_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM3_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON31_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM31_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL31_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM31_CTRL_ID),

                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM32_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM3_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON32_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM32_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL32_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM32_CTRL_ID),

                 CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMIBROWSER_FAVORITE_CHILD_FORM33_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM3_CTRL_ID),
                    CHILD_BUTTON_CTRL(FALSE, IMAGE_BROWSER_FAVORITE_ADD_ICON,MMIBROWSER_FAVORITE_BUTTON33_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM33_CTRL_ID),
                    CHILD_LABEL_CTRL(ALIGN_HMIDDLE, FALSE, MMIBROWSER_FAVORITE_LABEL33_CTRL_ID, MMIBROWSER_FAVORITE_CHILD_FORM33_CTRL_ID),


    END_WIN
}; 

WINDOW_TABLE(MMIBROWSER_FAVORITE_EDIT_WIN_TAB) = 
{
    WIN_TITLE(TXT_EDIT),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
#endif /* MMI_PDA_SUPPORT */

    WIN_FUNC((uint32)BrwHandleFavoriteEditWinMsg),    
    WIN_ID(MMIBROWSER_FAVORITE_EDIT_WIN_ID),
    
#ifdef MMI_PDA_SUPPORT
    WIN_SOFTKEY(STXT_SAVE, TXT_NULL, STXT_RETURN),
#else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */

    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER,MMIBROWSER_FAVORITE_EDIT_CTRL_ID),
        CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_FAVORITE_EDIT_FORM1_CTRL_ID,MMIBROWSER_FAVORITE_EDIT_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_FAVORITE_EDIT_LABEL1_CTRL_ID, MMIBROWSER_FAVORITE_EDIT_FORM1_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH,MMIBROWSER_FAVORITE_EDIT_TITLE_CTRL_ID, MMIBROWSER_FAVORITE_EDIT_FORM1_CTRL_ID),

        CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_FAVORITE_EDIT_FORM2_CTRL_ID,MMIBROWSER_FAVORITE_EDIT_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_FAVORITE_EDIT_LABEL2_CTRL_ID, MMIBROWSER_FAVORITE_EDIT_FORM2_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBROWSER_MAX_URL_LEN,MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID, MMIBROWSER_FAVORITE_EDIT_FORM2_CTRL_ID),

    END_WIN
};

WINDOW_TABLE(MMIBROWSER_FAVORITE_POPUP_WIN_TAB) = 
{
    WIN_FUNC((uint32) BrwHandleFavoritePopUpWinMsg), 
    WIN_SUPPORT_ANGLE( WIN_SUPPORT_ANGLE_INIT),
    WIN_ID(MMIBROWSER_FAVORITE_POPUP_WIN_ID),
    END_WIN
};

//the file detail window
WINDOW_TABLE(MMIBROWSER_FAVORITE_DETAIL_WIN_TAB) = 
{
    WIN_TITLE(STXT_DETAIL),
    WIN_FUNC((uint32)BrwHandleFavoriteDetailWinMsg),    
    WIN_ID(MMIBROWSER_FAVORITE_DETAIL_WIN_ID),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    CREATE_RICHTEXT_CTRL(MMIBROWSER_FAVORITE_DETAIL_CTRL_ID),
    
    END_WIN
};

#endif//BROWSER_OPERA_START_PAGE

#ifdef MMIDORADO_MY_NAV_SUPPORT
WINDOW_TABLE(MMIBROWSER_MY_NAV_EDIT_WIN_TAB) = 
{
    WIN_TITLE(TXT_EDIT),
    WIN_FUNC((uint32)BrwHandleMyNavEditWinMsg),    
    WIN_ID(MMIBROWSER_MY_NAV_EDIT_WIN_ID),
    
#ifndef MMI_PDA_SUPPORT
    WIN_SOFTKEY(STXT_SAVE, TXT_NULL, STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */

    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER,MMIBROWSER_MY_NAV_EDIT_CTRL_ID),
        CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_MY_NAV_EDIT_FORM1_CTRL_ID,MMIBROWSER_MY_NAV_EDIT_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_MY_NAV_EDIT_LABEL1_CTRL_ID, MMIBROWSER_MY_NAV_EDIT_FORM1_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH,MMIBROWSER_MY_NAV_EDIT_TITLE_CTRL_ID, MMIBROWSER_MY_NAV_EDIT_FORM1_CTRL_ID),

        CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_MY_NAV_EDIT_FORM2_CTRL_ID,MMIBROWSER_MY_NAV_EDIT_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_MY_NAV_EDIT_LABEL2_CTRL_ID, MMIBROWSER_MY_NAV_EDIT_FORM2_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBRW_MY_NAV_MAX_URL_LENGTH,MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID, MMIBROWSER_MY_NAV_EDIT_FORM2_CTRL_ID),
#ifdef MMI_PDA_SUPPORT
        CHILD_SOFTKEY_CTRL(STXT_SAVE, TXT_NULL, STXT_RETURN,MMICOMMON_SOFTKEY_CTRL_ID,MMIBROWSER_MY_NAV_EDIT_CTRL_ID),
#endif

    END_WIN
};

WINDOW_TABLE(MMIBROWSER_MY_NAV_POPUP_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleMyNavPopUpWinMsg), 
    WIN_ID(MMIBROWSER_MY_NAV_POPUP_WIN_ID),
#ifndef MMI_PDA_SUPPORT
    WIN_STYLE( WS_HAS_TRANSPARENT),
    CREATE_POPMENU_CTRL(MENU_BRW_MY_NAV_OPT, MMIBROWSER_MY_NAV_POPUP_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
#else
    WIN_SUPPORT_ANGLE(WIN_SUPPORT_ANGLE_INIT),
#endif

    END_WIN
};

//the file detail window
WINDOW_TABLE(MMIBROWSER_MY_NAV_DETAIL_WIN_TAB) = 
{
    WIN_TITLE(STXT_DETAIL),
    WIN_FUNC((uint32)BrwHandleMyNavDetailWinMsg),    
    WIN_ID(MMIBROWSER_MY_NAV_DETAIL_WIN_ID),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    CREATE_RICHTEXT_CTRL(MMIBROWSER_MY_NAV_DETAIL_CTRL_ID),
    
    END_WIN
};
#ifndef MMI_PDA_SUPPORT
WINDOW_TABLE(MMIBROWSER_BOUTIQUE_AND_NAV_WIN_TAB) = 
{
    WIN_FUNC((uint32)BrwHandleBoutiqueAndNavWinMsg),
    WIN_ID(MMIBROWSER_BOUTIQUE_AND_NAV_WIN_ID),
    WIN_STATUSBAR,
    CREATE_ICONLIST_CTRL(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID),
    WIN_SOFTKEY(STXT_OPTION, TXT_COMM_OPEN, STXT_EXIT),
    END_WIN
}; 

#endif

#endif

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
//the browser main window
WINDOW_TABLE(MMIBROWSER_FIX_HTML_WIN_TAB) = 
{
    WIN_TITLE(TXT_BROWSER_RMFL),
    WIN_FUNC((uint32)BrwHandleFixHtmlWinMsg),
    WIN_ID(MMIBROWSER_FIX_HTML_WIN_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMIBROWSER_FIX_HTML_LABEL_CTRL_ID),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    WIN_STATUSBAR,    
    END_WIN
}; 
#endif

#ifdef BROWSER_BOOKMARK_HISTORY_TAB
WINDOW_TABLE( MMIBROWSER_BK_AND_HISTORY_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)HandleBkAndHistoryWindow),    
    WIN_ID( MMIBROWSER_BK_AND_HISTORY_WIN_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#ifdef MMI_GUI_STYLE_TYPICAL
    CREATE_TAB_CTRL(GUI_TAB_ITEM_LEFT, GUITAB_STATE_NULL, MMIBROWSER_BK_AND_HISTORY_TAB_CTRL_ID),
#else
    CREATE_TAB_CTRL(GUI_TAB_ITEM_LEFT, GUITAB_STATE_NONEED_SCROLL|GUITAB_STATE_SINGLE_LINE, MMIBROWSER_BK_AND_HISTORY_TAB_CTRL_ID),
#endif
    END_WIN
};
#endif


WINDOW_TABLE( MMIBROWSER_STARTPAGE_POPMENU_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)BrwHandleStartPagePopMenuWinMsg),
    WIN_ID(MMIBROWSER_STARTPAGE_POPMENU_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
    END_WIN
};

WINDOW_TABLE(MMIBROWSER_ADD2MAIN_EDIT_WIN_TAB) = 
{
    WIN_TITLE(TXT_EDIT),
    WIN_FUNC((uint32)BrwHandleAdd2MainEditWinMsg),    
    WIN_ID(MMIBROWSER_ADD2MAIN_EDIT_WIN_ID),
#ifndef MMI_PDA_SUPPORT
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER,MMIBROWSER_ADD2MAIN_EDIT_CTRL_ID),
        CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_ADD2MAIN_EDIT_FORM1_CTRL_ID,MMIBROWSER_ADD2MAIN_EDIT_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_ADD2MAIN_EDIT_LABEL1_CTRL_ID, MMIBROWSER_ADD2MAIN_EDIT_FORM1_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH,MMIBROWSER_ADD2MAIN_EDIT_TITLE_CTRL_ID, MMIBROWSER_ADD2MAIN_EDIT_FORM1_CTRL_ID),

        CHILD_FORM_CTRL(TRUE,GUIFORM_LAYOUT_ORDER,MMIBROWSER_ADD2MAIN_EDIT_FORM2_CTRL_ID,MMIBROWSER_ADD2MAIN_EDIT_CTRL_ID),
            CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMIBROWSER_ADD2MAIN_EDIT_LABEL2_CTRL_ID, MMIBROWSER_ADD2MAIN_EDIT_FORM2_CTRL_ID),
            CHILD_EDIT_TEXT_CTRL(TRUE, MMIBROWSER_MAX_URL_LEN,MMIBROWSER_ADD2MAIN_EDIT_URL_CTRL_ID, MMIBROWSER_ADD2MAIN_EDIT_FORM2_CTRL_ID),
#ifdef MMI_PDA_SUPPORT
    CHILD_SOFTKEY_CTRL(STXT_SAVE, TXT_NULL, STXT_RETURN,MMICOMMON_SOFTKEY_CTRL_ID,MMIBROWSER_ADD2MAIN_EDIT_CTRL_ID),
#endif
 
    END_WIN
};


/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
#ifdef OMADL_SUPPORT
/*****************************************************************************/
//  Description : Set DD property info
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void BrwSetDDPropertyInfo(MMI_WIN_ID_T  win_id)
{
    BRW_MSG_OMADL_CONFIRM_REQ_T  *req_ptr = PNULL;
    GUIRICHTEXT_ITEM_T          item_data = {0};
    uint16  index = 0;
    MMI_CTRL_ID_T ctrl_id = MMIBROWSER_OMADL_QUERY_RICHTEXT_CTRL_ID;
    char temp_buf[MMIBROWSER_PROGRESS_BUF_LEN +1] = {0};
    uint16 temp_uint16_buf[MMIBROWSER_PROGRESS_BUF_LEN +1] = {0};
    uint32  temp_len = 0;

    req_ptr = (BRW_MSG_OMADL_CONFIRM_REQ_T *)MMK_GetWinAddDataPtr(win_id);

    if (PNULL == req_ptr)
    {
        //SCI_TRACE_LOW:"[BRW]W BrwSetDDPropertyInfo PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2035_112_2_18_2_5_58_116,(uint8*)"");
        return;
    }
    
    //name
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_DL_DD_MO_NAME;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_data.buf.len = MMIAPICOM_Wstrlen(req_ptr->name_arr);
    item_data.text_data.buf.len = MIN(item_data.text_data.buf.len, GUIRICHTEXT_TEXT_MAX_LEN);
    if (item_data.text_data.buf.len > 0)
    {
        item_data.text_type = GUIRICHTEXT_TEXT_BUF;
        item_data.text_data.buf.str_ptr = req_ptr->name_arr;
    }
    else
    {
        item_data.text_type = GUIRICHTEXT_TEXT_RES;
        item_data.text_data.res.id = TXT_UNKNOWN;//unknown
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //type
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_TYPE;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    temp_len = SCI_STRLEN(req_ptr->type);
    
    if ((BRW_MIME_OMA_DRM_MESSAGE == req_ptr->mime_type) || (BRW_MIME_OMA_DRM_CONTENT == req_ptr->mime_type))
    {
        item_data.text_type = GUIRICHTEXT_TEXT_RES;
        item_data.text_data.res.id = TXT_FILE;//DRM type
    }
    else if ((BRW_MIME_UNKOWN == req_ptr->mime_type) || (0 == temp_len))
    {
        item_data.text_type = GUIRICHTEXT_TEXT_RES;
        item_data.text_data.res.id = TXT_UNKNOWN;//unknown
    }
    else
    {
        SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
        SCI_MEMSET(temp_uint16_buf, 0x00, sizeof(temp_uint16_buf));

        MMI_STRNTOWSTR(temp_uint16_buf, MMIBROWSER_PROGRESS_BUF_LEN, 
                (const uint8*)(req_ptr->type), temp_len, 
                MIN(temp_len, MMIBROWSER_PROGRESS_BUF_LEN));
        
        item_data.text_data.buf.len = MMIAPICOM_Wstrlen(temp_uint16_buf);
        item_data.text_data.buf.len = MIN(item_data.text_data.buf.len, GUIRICHTEXT_TEXT_MAX_LEN);
        if (item_data.text_data.buf.len > 0)
        {
            item_data.text_type = GUIRICHTEXT_TEXT_BUF;
            item_data.text_data.buf.str_ptr = temp_uint16_buf;
        }
        else
        {
            item_data.text_type = GUIRICHTEXT_TEXT_RES;
            item_data.text_data.res.id = TXT_UNKNOWN;//unknown
        }
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //vendor
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));    
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_DL_DD_MO_VENDOR;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_data.buf.len = MMIAPICOM_Wstrlen(req_ptr->vendor_arr);
    item_data.text_data.buf.len = MIN(item_data.text_data.buf.len, GUIRICHTEXT_TEXT_MAX_LEN);
    if (item_data.text_data.buf.len > 0)
    {
        item_data.text_type = GUIRICHTEXT_TEXT_BUF;
        item_data.text_data.buf.str_ptr = req_ptr->vendor_arr;
    }
    else
    {
        item_data.text_type = GUIRICHTEXT_TEXT_RES;
        item_data.text_data.res.id = TXT_UNKNOWN;//unknown
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //size    
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_DETAIL_SIZE;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    if (req_ptr->size/MMIBROWSER_ONE_K > 0)
    {
        CFL_Snprintf(temp_buf,sizeof(temp_buf),"%.1f%s", (float)(req_ptr->size)/MMIBROWSER_ONE_K, "KB");
    }
    else
    {
        CFL_Snprintf(temp_buf,sizeof(temp_buf),"%d%s", req_ptr->size,"B");
    }    
    SCI_MEMSET(temp_uint16_buf, 0x00, sizeof(temp_uint16_buf));
    CFLWCHAR_StrNCpyWithAsciiStr(temp_uint16_buf, temp_buf, (MMIBROWSER_PROGRESS_BUF_LEN+1));
    item_data.text_data.buf.len = MMIAPICOM_Wstrlen(temp_uint16_buf);
    item_data.text_data.buf.str_ptr = temp_uint16_buf;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //description
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));    
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_DL_DD_MO_DESCRIPTION;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_data.buf.len = MMIAPICOM_Wstrlen(req_ptr->description_arr);
    item_data.text_data.buf.len = MIN(item_data.text_data.buf.len, GUIRICHTEXT_TEXT_MAX_LEN);
    if (item_data.text_data.buf.len > 0)
    {
        item_data.text_type = GUIRICHTEXT_TEXT_BUF;
        item_data.text_data.buf.str_ptr = req_ptr->description_arr;
    }
    else
    {
        item_data.text_type = GUIRICHTEXT_TEXT_RES;
        item_data.text_data.res.id = TXT_UNKNOWN;//unknown
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
   
}

/*****************************************************************************/
//  Description : handle display DD property and query download
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleOMADLQueryWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    recode = MMI_RESULT_TRUE;
    LOCAL BOOLEAN   s_is_need_to_cancle_dl = FALSE;//use in MSG_CLOSE_WINDOW
    BRW_MSG_OMADL_CONFIRM_REQ_T  *req_ptr = PNULL;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        BrwSetDDPropertyInfo(win_id);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_OMADL_QUERY_RICHTEXT_CTRL_ID);
        s_is_need_to_cancle_dl = TRUE;//INIT
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
#ifdef MMI_PDA_SUPPORT
        {
            MMI_CTRL_ID_T       id = 0;

            if (PNULL != param)
            {
                id = ((MMI_NOTIFY_T*) param)->src_id;

                if (id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                    break;
                }
                else if (id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                    break;
                }
            }    
        }
#endif /* MMI_PDA_SUPPORT */    
    {
        BRW_MSG_ENTER_FILENM_REQ_T  enter_filenm_req_ptr = {0};
        uint32  req_name_size = 0;
        req_ptr = (BRW_MSG_OMADL_CONFIRM_REQ_T *)MMK_GetWinAddDataPtr(win_id);

        if (req_ptr != PNULL)
        {
            s_is_need_to_cancle_dl = FALSE;
            //打开文件名编辑窗口
            enter_filenm_req_ptr.handle = req_ptr->handle;
            req_name_size = sizeof(req_ptr->name_arr)/sizeof(uint16);
            req_name_size= MIN(req_name_size, BRW_MAX_URL_LEN);
            
            BrwGetDlFileNameByMimeType(enter_filenm_req_ptr.res_name_arr, 
                BRW_MAX_URL_LEN, 
                req_ptr->mime_type, 
                req_ptr->name_arr);//MS00249210

            enter_filenm_req_ptr.mime_type = req_ptr->mime_type;
            enter_filenm_req_ptr.content_len = req_ptr->size;
            enter_filenm_req_ptr.enter_filenm_cbf = req_ptr->dl_confirm_cbf;
            enter_filenm_req_ptr.dl_type = BRW_DL_TYPE_OMA_CONTENT;
            
#ifdef DRM_SUPPORT    
            enter_filenm_req_ptr.drm_mo_mime_type = req_ptr->drm_mo_mime_type;
            enter_filenm_req_ptr.expired_time = req_ptr->expired_time;
#endif

#ifdef OMADL_SUPPORT
            enter_filenm_req_ptr.next_url_ptr = (uint8 *)req_ptr->next_url;
#endif

            MMIBROWSER_SetLoading(TRUE);
            MMIBROWSER_SetBeginDownload(TRUE);
            MMIBROWSER_UpdateFileNameReq(&enter_filenm_req_ptr);
            MMIBROWSER_UpdateReqHandle(enter_filenm_req_ptr.handle);
            MMIBROWSER_SetDownloadMedia(TRUE);
            MMIBROWSER_SetTotalDataLen(enter_filenm_req_ptr.content_len);
            MMIBROWSER_OpenSaveMediaFileWin(enter_filenm_req_ptr.mime_type, PNULL);
        }
        else
        {
            //SCI_TRACE_LOW:"[BRW]W HandleOMADLQueryWinMsg OK PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2256_112_2_18_2_5_59_117,(uint8*)"");
        }
        MMK_CloseWin(win_id);
    }
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        //stop download
        req_ptr = (BRW_MSG_OMADL_CONFIRM_REQ_T *)MMK_GetWinAddDataPtr(win_id);
        if ((req_ptr != PNULL) && (req_ptr->dl_confirm_cbf != PNULL))
        {
            req_ptr->dl_confirm_cbf(req_ptr->handle, PNULL, 0, FALSE);
        }
        else
        {
            BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle());
            //SCI_TRACE_LOW:"[BRW]W HandleOMADLQueryWinMsg cancel PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2273_112_2_18_2_5_59_118,(uint8*)"");
        }
        MMIBROWSER_SetToStopDL();
        s_is_need_to_cancle_dl = FALSE;//has canceled
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        if (s_is_need_to_cancle_dl)//need to cancel download
        {
            BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle());
            MMIBROWSER_SetToStopDL();            
        }
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}
#endif

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_DISPLAY_READY_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlDisplayReadyInd(void *param)
{
    BOOLEAN ret = TRUE;
    uint32  cur_media_num = 0;
    uint32  total_media_num = 0;
    wchar  *title_wstr_ptr = PNULL;
    uint16  title_len = 0;
    BOOLEAN is_update_button = TRUE;
    BOOLEAN is_update = FALSE;

    //if has get all media file, set loading false
    cur_media_num = MMIBROWSER_GetCurrMediaNum();
    total_media_num = MMIBROWSER_GetTotalMediaNum();
    if (cur_media_num >= total_media_num)//complete
    {
        MMIBROWSER_SetLoading(FALSE);
        MMIBROWSER_SetParamToStop();
    }
    
    //MS00201868 get page title and update the label ctrl
    title_wstr_ptr = (wchar *)SCI_ALLOCA((MMIBROWSER_MAX_TITLE_LEN+1) *sizeof(wchar));
    if (title_wstr_ptr != PNULL)
    {
        SCI_MEMSET(title_wstr_ptr, 0x00, (MMIBROWSER_MAX_TITLE_LEN+1) *sizeof(wchar));
        BRW_GetPageTitle(MMIBROWSER_GetInstance(), title_wstr_ptr, &title_len, MMIBROWSER_MAX_TITLE_LEN);
        //SCI_TRACE_LOW:"[BRW]HandleControlDisplayReadyInd :title_len = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2324_112_2_18_2_5_59_119,(uint8*)"d", title_len);
        if (title_len > 0)
        {
            MMIBROWSER_UpdateAddrLable(title_wstr_ptr, FALSE);
        }
        SCI_FREE(title_wstr_ptr);
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]W display page title error, alloc PNULL!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2333_112_2_18_2_5_59_120,(uint8*)"");
    }
    
    if (MMIBROWSER_IsMainActive())
    {
        is_update = TRUE;
        if (!BRW_DisplayPage(MMIBROWSER_GetInstance()))
        {
            //SCI_TRACE_LOW:"[BRW]W BRW_MSG_TYPE_DISPLAY_READY_IND:BRW_DisplayPage Error!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2340_112_2_18_2_5_59_121,(uint8*)"");
            MMIBROWSER_SetNeedDisplay(TRUE);
        }
        
        //若页面没有media, 表示web page is complete
        if (0 == MMIBROWSER_GetTotalMediaNum())
        {
            MMIBROWSER_SetToStop();
            is_update_button = FALSE;//the button has been updated in MMIBROWSER_SetToStop
        }
    }
    else
    {
        //SCI_TRACE_LOW:"MMIBROWSER need display TRUE"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2352_112_2_18_2_5_59_122,(uint8*)"");
        MMIBROWSER_SetNeedDisplay(TRUE);
        is_update = FALSE;
    }

    if (is_update_button)
    {
#ifdef BROWSER_TOOLBAR
        BrowserChangeButtonStatus(is_update);
        BrwChangeRefreshButtonImg(TRUE, is_update);
#else
        MMI_TEXT_ID_T   rightsoft_id = TXT_NULL;

        rightsoft_id = (BRW_NavHisIsBackwardEnable())? TXT_COMMON_COMMON_BACK: TXT_COMMON_CLOSE;
        GUIWIN_SetSoftkeyTextId(MMIBROWSER_MAIN_WIN_ID, STXT_OPTION, TXT_NULL, rightsoft_id, is_update);
#endif
    }
    return ret;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_AUTH_REQ
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlAuthReq(void *param)
{
    BRW_MSG_AUTH_REQ_T  *req_ptr = PNULL;
    
    if (PNULL != param)
    {
        req_ptr = (BRW_MSG_AUTH_REQ_T*)param;
        MMIBROWSER_UpdateAuthReq(req_ptr);
        MMK_CreateWin((uint32*)MMIBROWSER_AUTH_WIN_TAB,PNULL);
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]W HandleControlAuthReq:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2375_112_2_18_2_5_59_123,(uint8*)"");
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_CHANGE_URL_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlChangeUrlInd(void *param)
{
    BRW_MSG_CHANGE_URL_IND_T    *ind_ptr = PNULL;

    if (PNULL != param)
    {
        ind_ptr = (BRW_MSG_CHANGE_URL_IND_T*)param;
        
        MMIBROWSER_SetParamToStop();
        
        MMIBROWSER_UpdateUrl((char*)(ind_ptr->url_arr));
        MMIBROWSER_SetToAccess((const char*)(ind_ptr->url_arr), TRUE);
    }

    return TRUE;
}
/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_NEW_PAGE_REQ_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlNewPageInd(void *param)
{
    BRW_MSG_NEW_PAGE_REQ_IND_T    *ind_ptr = PNULL;

    if (PNULL != param)
    {
        ind_ptr = (BRW_MSG_NEW_PAGE_REQ_IND_T*)param;
        
        MMIBROWSER_SetParamToStop();

        MMIBROWSER_UpdateUrl((char*)(ind_ptr->url_arr));
        MMIBROWSER_SetToAccess((const char*)(ind_ptr->url_arr), TRUE);
    }
    
    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_ERROR_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlErrorInd(void *param)
{
    BRW_MSG_ERROR_IND_T *err_ind=PNULL;
    MMI_WIN_ID_T   sll_query_win_id = MMIBROWSER_SSL_QUERY_WIN_ID;	

    if (PNULL != param)
    {
        err_ind=(BRW_MSG_ERROR_IND_T*)param;
        
        MMIBROWSER_CloseQuerytWin(&sll_query_win_id);//@fen.xie 
        BrwShowErrMsg(err_ind->error);
    }

    if (MMIBROWSER_GetLoading())//set to stop(normally, error 之前control层已经stop了req)
    {
        MMIBROWSER_SetLoading(FALSE);
        MMIBROWSER_SetParamToStop();
        MMIBROWSER_SetToStop();
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_ENTER_FILENM_REQ
//  Global resource dependence : 
//  Author: fen.xie
//  Note: if error, need to stop the download process
/*****************************************************************************/
LOCAL BOOLEAN HandleControlEnterFileNMInd(void *param)
{
    BRW_MSG_ENTER_FILENM_REQ_T  *req_ptr = PNULL;
    BOOLEAN  result = FALSE;
    MMIFILE_DEVICE_E file_type = MMI_DEVICE_NUM;

    if(PNULL != param)
    {
        req_ptr = (BRW_MSG_ENTER_FILENM_REQ_T*)param;
        
        file_type = BrowserDlGetDeviceName(PNULL, PNULL, req_ptr->mime_type);//get the first valid device for mime type
        if (file_type < MMI_DEVICE_NUM)//nornal, begin to download
        {
            MMIBROWSER_UpdateFileNameReq(req_ptr);
            MMIBROWSER_UpdateReqHandle(req_ptr->handle);
            MMIBROWSER_SetDownloadMedia(TRUE);
            MMIBROWSER_SetTotalDataLen(req_ptr->content_len);
            MMIBROWSER_OpenSaveMediaFileWin(req_ptr->mime_type, PNULL);
            result = TRUE;
        }
        else//without storage
        {
            //MS00247559:need to stop the download process
            BRW_StopDL(MMIBROWSER_GetInstance(), req_ptr->handle);
            MMIBROWSER_SetToStopDL();
            
            MMIPUB_OpenAlertWarningWin(TXT_SD_NO_EXIST);
        }
    }
    else
    {
        //param error, can't to stop the download process(need to assert......)
        //SCI_TRACE_LOW:"[BRW]W HandleControlEnterFileNMInd:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2485_112_2_18_2_5_59_124,(uint8*)"");
    }

    MMIBROWSER_DelInvalidDlFile();//MS00233946 clean the prevent download fail file

    return result;
}

#ifdef SSL_SUPPORT
/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_SSL_CERT_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlSslCertInd(void *param)
{
    BRW_MSG_SSL_CERT_IND_T  *ind_ptr = PNULL;

    if(PNULL != param)
    {
        MMIBROWSER_NV_SETTING_T *nv_setting_ptr = PNULL;
        MMI_WIN_ID_T   sll_query_win_id = MMIBROWSER_SSL_QUERY_WIN_ID;//@fen.xie	
     
        ind_ptr = (BRW_MSG_SSL_CERT_IND_T*)param;
        
        nv_setting_ptr = MMIBROWSER_GetNVSetting();
        if (nv_setting_ptr->is_ssl_warning_enable)
        {
            MMIBROWSER_UpdateSSLCert(ind_ptr);
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_SSL_CERT,IMAGE_PUBWIN_QUERY,&sll_query_win_id,BrwSSLCertQueryCallback);
        }
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]W HandleControlSslCertInd:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2518_112_2_18_2_5_59_125,(uint8*)"");
    }

    return TRUE;
}
#endif

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_PROGRESS_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlProgressInd(void *param)
{
    BRW_MSG_PROGRESS_IND_T  *prg_ptr = PNULL;
    uint32 receive_data_len = 0;
    uint32 total_data_len = 0;

    if (PNULL == param)
    {
        //SCI_TRACE_LOW:"[BRW]W BHandleControlProgressInd:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2537_112_2_18_2_5_59_126,(uint8*)"");
        return FALSE;
    }
    
    prg_ptr = (BRW_MSG_PROGRESS_IND_T*)param;

    //SCI_TRACE_LOW:"HandleControlProgressInd:prg_ptr->total_data_len =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2543_112_2_18_2_5_59_127,(uint8*)"d", prg_ptr->total_data_len);
    if (prg_ptr->is_download_req && !MMIBROWSER_GetDownloadMedia())
    {                
        return TRUE;//download has been cancel, discard the ind
    }
    
    MMIBROWSER_UpdateRecvDataLen(prg_ptr->received_data_len);
    if ((0 == MMIBROWSER_GetTotalDataLen()) && (0 != prg_ptr->total_data_len))
    {
        MMIBROWSER_SetTotalDataLen(prg_ptr->total_data_len);
    }
    receive_data_len = MMIBROWSER_GetRecvDataLen();
    total_data_len = MMIBROWSER_GetTotalDataLen();
    
    //SCI_TRACE_LOW:"BHandleControlProgressInd:receive_data_len = %d,total_data_len =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2557_112_2_18_2_5_59_128,(uint8*)"dd", receive_data_len, total_data_len);
     //cr150690 if receive data len > total data len, set receive data len == total data len
    if ((0 != total_data_len) && (receive_data_len > total_data_len))
    {
        receive_data_len = total_data_len;
    }

    //if download media file, open download media file windown and draw prodress
    if (prg_ptr->is_download_req)
    {                
        if (!MMK_IsOpenWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID))
        {
            if (MMIBROWSER_GetBeginDownload())
            {
                MMK_CreateWin((uint32 *)MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_TAB, PNULL);
            }
        }
        else
        {
            BrwSetProgressForDL();
            if (MMK_IsFocusWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID))//MS00189093
            {
                MMK_SendMsg(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID, MSG_FULL_PAINT,  PNULL);
            }
        }
    }
    else//display the progress in address bar
    {
        if (MMIBROWSER_IsMainActive())
        {
            if (0 == total_data_len)
            {
                MMIBROWSER_DisplayAdressBar(receive_data_len,(receive_data_len + MMIBROWSER_DOWNLOAD_EXT_LEN)*2);
            }
            else
            {
                MMIBROWSER_DisplayAdressBar(receive_data_len,total_data_len*2);
            }
        }
    }

    return TRUE;
}


/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_DOWNLOAD_OK_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlDownloadOkInd(void *param)
{
    MMIBROWSER_MIME_TYPE_E  mime_type = MMIBROWSER_GetMimeType();
#ifdef MMI_GUI_STYLE_TYPICAL
    MMI_WIN_ID_T    alert_win_id = MMIBROWSER_ALERT_WIN_ID;
#endif /* MMI_GUI_STYLE_TYPICAL */
#ifdef DRM_SUPPORT
    MMIDRM_DL_DATA_T    drm_dl_data = {0};
#endif
#ifdef OMADL_SUPPORT
    uint8   url_arr[MMIBROWSER_MAX_URL_LEN + 1] = {0};
#endif

    {
        /*MMIBROWSER_GetFileNameReq的数据在MMIBROWSER_SetParaForStopDL中会重置,因此
            限制file_name_req_ptr的范围,避免外部使用已经reset的数据*/    
        BRW_MSG_ENTER_FILENM_REQ_T  *file_name_req_ptr = PNULL;

        file_name_req_ptr = MMIBROWSER_GetFileNameReq();

#ifdef DRM_SUPPORT
        if ((MMIBROWSER_MIME_TYPE_DM == mime_type) || (MMIBROWSER_MIME_TYPE_DCF == mime_type))
        {
            drm_dl_data.drm_mo_type = BrwConvertMimeType(file_name_req_ptr->drm_mo_mime_type);
            if ((file_name_req_ptr != PNULL) && (file_name_req_ptr->expired_time > 0))
            {
                drm_dl_data.is_include_sd_timer_out = TRUE;
                drm_dl_data.timer_out = file_name_req_ptr->expired_time;
            }
        }
#endif

#ifdef OMADL_SUPPORT
        if ((file_name_req_ptr != PNULL) && (file_name_req_ptr->next_url_ptr != PNULL))
        {
            uint16  size = SCI_STRLEN((char *)file_name_req_ptr->next_url_ptr);
            SCI_MEMCPY(url_arr, file_name_req_ptr->next_url_ptr, size);
        }
#endif
    }

    MMIBROWSER_SetParaForStopDL();        
    SCI_TRACE_LOW("[BRW]Dorado download:end successful = %d",  SCI_GetTickCount());

    if (MMIBROWSER_MIME_TYPE_NONE != mime_type)
    {
        MMIBROWSER_MIME_CALLBACK_PARAM_T mime_param = {0};
        uint32 len = 0;
        
        mime_param.mime_type = mime_type;
        mime_param.url_ptr = MMIBROWSER_GetUrl();
        SCI_MEMSET(mime_param.file_name, 0x00, sizeof(mime_param.file_name));
        
        len = MMIAPICOM_Wstrlen(MMIBROWSER_GetFileName());
        MMI_WSTRNCPY(mime_param.file_name, MMIFILE_FULL_PATH_MAX_LEN, 
                MMIBROWSER_GetFileName(), len, 
                MIN(len, MMIFILE_FULL_PATH_MAX_LEN));
        mime_param.download_result = TRUE;

#ifdef DRM_SUPPORT
        //MMIBROWSER_MIME_TYPE_DR,MMIBROWSER_MIME_TYPE_DRC 待扩展
        if ((MMIBROWSER_MIME_TYPE_DM == mime_type) || (MMIBROWSER_MIME_TYPE_DCF == mime_type))
        {       
            mime_param.user_data = &drm_dl_data;
            MMIBROWSER_MIMEDownlaodCallBack(&mime_param);
            MMIPUB_CloseWaitWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID);
       }
        else
#endif        
        {
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertWinByTextId(PNULL,TXT_DOWNLOAD_SUCCEED,PNULL,IMAGE_PUBWIN_SUCCESS,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
#endif /* MMI_GUI_STYLE_TYPICAL */
            MMIPUB_CloseWaitWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID);
            MMIBROWSER_MIMEDownlaodCallBack(&mime_param);
        }
        
    }
    else
    {
#ifdef MMI_GUI_STYLE_TYPICAL
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_DOWNLOAD_SUCCEED,PNULL,IMAGE_PUBWIN_SUCCESS,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
#endif /* MMI_GUI_STYLE_TYPICAL */
        MMIPUB_CloseWaitWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID);
    }
    MMIBROWSER_SetToStop();//MS00241440

#ifdef OMADL_SUPPORT
    if (SCI_STRLEN((char *)url_arr) > 0)
    {
        MMIBROWSER_EnterNewUrl(url_arr, FALSE);
    }
#endif

    MMIBROWSER_UpdateFileName(PNULL);//MS00233946 clean the data,it means the download is complete

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_DOWNLOAD_ERR_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlDownloadErrInd(void *param)
{
    BRW_MSG_DOWNLOAD_ERROR_IND_T *err_ind=(BRW_MSG_DOWNLOAD_ERROR_IND_T*)param;
    MMI_TEXT_ID_T   alert_text_id = TXT_DOWNLOAD_FAILED;
#ifdef OMADL_SUPPORT
    uint8   url_arr[MMIBROWSER_MAX_URL_LEN + 1] = {0};
    BRW_MSG_ENTER_FILENM_REQ_T  *file_name_req_ptr = MMIBROWSER_GetFileNameReq();
#endif

#ifdef OMADL_SUPPORT
    if ((file_name_req_ptr != PNULL) && (file_name_req_ptr->next_url_ptr != PNULL))
    {
        uint16  size = SCI_STRLEN((char *)file_name_req_ptr->next_url_ptr);
        SCI_MEMCPY(url_arr, file_name_req_ptr->next_url_ptr, size);
    }
#endif
    
    if ((PNULL != err_ind) && (BRW_ERR_GPRS_LINK_DISCONNECT == err_ind->error))
    {
        alert_text_id = TXT_BROWSER_GPRS_DISCONNECT;
    }
    
    MMIBROWSER_SetToStopDL();
    MMIPUB_OpenAlertWarningWin(alert_text_id);
    MMK_CloseWin(MMIBROWSER_SAVE_MEDIAFILE_WIN_ID);
    MMIPUB_CloseWaitWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID);
#ifdef OMADL_SUPPORT
    MMK_CloseWin(MMIBROWSER_OMADL_QUERY_WIN_ID);
#endif    

#ifdef OMADL_SUPPORT
    if (SCI_STRLEN((char *)url_arr) > 0)
    {
        MMIBROWSER_EnterNewUrl(url_arr, FALSE);
    }
#endif

    SCI_TRACE_LOW("[BRW]Dorado download:end fail = %d",  SCI_GetTickCount());

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_PAGE_GET_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlPageGetInd(void *param)
{
    uint32 media_num = 0;

    if (PNULL != param)
    {
        media_num = *(uint32*)param;
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]W HandleControlPageGetInd:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2756_112_2_18_2_6_0_129,(uint8*)"");
        media_num = 0;
    }

    MMIBROWSER_SetTotalMediaNum(media_num);
    MMIBROWSER_SetCurrMediaNum(0);
    MMIBROWSER_UpdateCurrMediaNum(0);
    MMIBROWSER_ClearRecvDataLen();
    MMIBROWSER_SetTotalDataLen(0);
    MMIBROWSER_UpdateReqHandle(0);
    
    if (0 == media_num)//if without media file, stop download
    {
        MMIBROWSER_SetLoading(FALSE);
        MMIBROWSER_SetParamToStop();
    }
    SCI_TRACE_LOW("[BRW]HandleControlPageGetInd total_media_num=%d",media_num);//MS00211311

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_MEDIA_GET_ONE_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlMediaGetOneInd(void *param)
{
    uint32  cur_media_num = 0;
    uint32  total_media_num = 0;

    MMIBROWSER_UpdateCurrMediaNum(1);
    cur_media_num = MMIBROWSER_GetCurrMediaNum();
    total_media_num = MMIBROWSER_GetTotalMediaNum();

    //MS00211311:增加media get的容错处理
    if (cur_media_num < total_media_num)//processing
    {
        //目前的process表示方案是:前半段表示页面下载速度,后半段表示media的下载进度
        MMIBROWSER_DisplayAdressBar((cur_media_num + total_media_num), 2*total_media_num);
    }
    else if (cur_media_num == total_media_num)//complete
    {
        MMIBROWSER_SetLoading(FALSE);
        MMIBROWSER_SetParamToStop();
        MMIBROWSER_SetToStop();
    }
    else//error
    {
        //SCI_TRACE_LOW:"[BRW]W HandleControlMediaGetOneInd cur=%d,total=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2803_112_2_18_2_6_0_130,(uint8*)"dd",cur_media_num,total_media_num);
    }

    //BRW_RepaintPage in the BRW_MSG_TYPE_REPAINT_READY_IND msg
    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_PAGE_STOP_IND
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlPageStopInd(void *param)
{
    BRW_REQ_STATE_E req_state = Brw_GetPageReqState(MMIBROWSER_GetInstance()) ;

    MMIBROWSER_SetParamToStop();//reset param
    
    //SCI_TRACE_LOW:"[BRW]HandleControlPageStopInd req_state =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2820_112_2_18_2_6_0_131,(uint8*)"d", req_state);
    //已经开始新请求了,无需再刷新stop的界面
    switch (req_state)
    {
    case BRW_STATE_PAGE_REQUESTING:
    case BRW_STATE_MEDIA_REQUESTING:
    case BRW_STATE_DOWNLOADING:
        break;
        
    default:
        MMIBROWSER_SetLoading(FALSE);
        MMIBROWSER_SetToStop();//update display
        break;
    }
    
    return TRUE;
}

#ifdef OMADL_SUPPORT
/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_OMA_DL_CONFIRM_REQ
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlOMADLConfirmReq(void *param)
{
    BRW_MSG_OMADL_CONFIRM_REQ_T  *temp_req_ptr = (BRW_MSG_OMADL_CONFIRM_REQ_T *)param;
    BRW_MSG_OMADL_CONFIRM_REQ_T  *req_ptr = PNULL;
#ifdef MMI_GUI_STYLE_TYPICAL
    MMI_WIN_ID_T    alert_win_id = MMIBROWSER_ALERT_WIN_ID;
#endif /* MMI_GUI_STYLE_TYPICAL */

    BOOLEAN result = FALSE;
    
    if ((PNULL != temp_req_ptr) && (temp_req_ptr->dl_confirm_cbf != PNULL))
    {
        req_ptr = SCI_ALLOCA(sizeof(BRW_MSG_OMADL_CONFIRM_REQ_T));//need to free in MMIBROWSER_OMADL_QUERY_WIN_TAB add data
        if (PNULL != req_ptr)//normal and goto the next step
        {
            MMIBROWSER_SetParaForStopDL();//reset the pre-download of DD
            SCI_MEMCPY(req_ptr, temp_req_ptr, sizeof(BRW_MSG_OMADL_CONFIRM_REQ_T));
            MMIBROWSER_UpdateReqHandle(req_ptr->handle);
            MMK_CreateWin((uint32*)MMIBROWSER_OMADL_QUERY_WIN_TAB, (ADD_DATA)req_ptr);
            result = TRUE;
        }
        else
        {
            //error
        }
    }

    if (!result)//alert the DD is download successfull, but there is error and stop to download the MO file
    {
        BRW_StopDL(MMIBROWSER_GetInstance(), MMIBROWSER_GetReqHandle());
        MMIBROWSER_SetToStopDL();    
#ifdef MMI_GUI_STYLE_TYPICAL
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_DOWNLOAD_SUCCEED,PNULL,IMAGE_PUBWIN_SUCCESS,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
#endif /* MMI_GUI_STYLE_TYPICAL */
        //SCI_TRACE_LOW:"[BRW]W HandleControlOMADLConfirmReq:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2872_112_2_18_2_6_0_132,(uint8*)"");
    }

    MMIPUB_CloseWaitWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID);

    return TRUE;
}
#endif

#if 0
/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_MAKECALL_REQ
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlMCReq(void *param)
{
    BRW_MSG_MAKE_CALL_REQ_T *call_ptr = PNULL;
    
    if (PNULL != param)
    {
        uint32 len = 0;
        
        call_ptr = (BRW_MSG_MAKE_CALL_REQ_T*)param;
        
        len = strlen((char*)(call_ptr->phone_num_arr));
        if (len > MMIBROWSER_PHONE_NUM_LEN)
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_PHONE_NUM_TOO_LONG);
        }
        else
        {
            if(0 == len)
            {
                MMIAPIIDLE_EnterDialWin();
            }
            else
            {       
                MMIBROWSER_CC_INFO_T cc_info = {0};
                //SCI_TRACE_LOW:"[BRW]HandleControlMCReq"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2910_112_2_18_2_6_0_133,(uint8*)"");
                cc_info.cc_option = MMIBROWSER_CC_OPTION_VOICE | MMIBROWSER_CC_OPTION_IP | MMIBROWSER_CC_OPTION_VIDEO;/*lint !e655*/
                cc_info.phone_num_ptr = call_ptr->phone_num_arr;
                MMIBROWSER_OpenCCSelectWin(&cc_info);
            }
        }
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_SENDDTMF_REQ
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlSendDTMFReq(void *param)
{
    BRW_MSG_SEND_DTMF_REQ_T *dtmf_ptr = PNULL;

    if (PNULL != param)
    {
        MMIBROWSER_CC_INFO_T cc_info = {0};
        
        dtmf_ptr = (BRW_MSG_SEND_DTMF_REQ_T*)param;
        //SCI_TRACE_LOW:"[BRW]HandleControlSendDTMFReq"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_2934_112_2_18_2_6_0_134,(uint8*)"");
        cc_info.cc_option = MMIBROWSER_CC_OPTION_VOICE | MMIBROWSER_CC_OPTION_IP;/*lint !e655*/
        cc_info.phone_num_ptr = dtmf_ptr->dtmf_arr;
        MMIBROWSER_OpenCCSelectWin(&cc_info);
    }
    
    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_ADD_PBITEM_REQ
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlAddPBItemReq(void *param)
{
    BRW_MSG_ADD_PB_REQ_T    *pb_req_ptr = PNULL;
	
    if (PNULL != param)
    {
        MMIPB_BCD_NUMBER_T  pb_bcd = {0};
        MMI_PARTY_NUMBER_T  party_num = {MN_NUM_TYPE_UNKNOW, 0, 0};
        uint32  len = 0;
        
        pb_req_ptr = (BRW_MSG_ADD_PB_REQ_T*)param;
        len = strlen((char*)(pb_req_ptr->phone_num_arr));
        
        if (!MMIAPICOM_IsValidNumberString((char *)pb_req_ptr->phone_num_arr, len))//MS00223055
        {
            //提示无效号码
            MMIPUB_OpenAlertWarningWin(TXT_CC_INVALID_NUM);
        }
        else if (len > MMIBROWSER_PHONE_NUM_LEN)
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_PHONE_NUM_TOO_LONG);
        }
        else
        {
            MMIAPICOM_GenPartyNumber(pb_req_ptr->phone_num_arr, strlen((char*)(pb_req_ptr->phone_num_arr)), &party_num);

            pb_bcd.npi_ton  = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, MN_NUM_PLAN_UNKNOW);
            pb_bcd.number_len = (uint8)MIN(party_num.num_len, MMIPB_BCD_NUMBER_MAX_LEN);
            SCI_MEMCPY( 
                    pb_bcd.number,  
                    party_num.bcd_num, 
                    pb_bcd.number_len 
                    );
            MMIAPIPB_AddContactWin(&pb_bcd,PB_GROUP_ALL,MMIPB_ADD_TYPE_NUMBER);
        }
    }			           

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_SENDSMS_REQ or 
//                                       BRW_MSG_TYPE_SENDSMSTO_REQ
//  Global resource dependence : 
//  Author: jixin.xu
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlSendSmsReq(void *param)
{
    BRW_MSG_SEND_SMS_REQ_T *sms_ptr = PNULL;

    if (PNULL != param)
    {
        uint32 num_len            = 0;
        uint32 content_len        = 0;
        uint8 *num_str_ptr        = PNULL;
        uint8 *content_str_ptr    = PNULL;
        MMI_STRING_T content_wstr = {0};

        sms_ptr = (BRW_MSG_SEND_SMS_REQ_T*)param;
        num_str_ptr = sms_ptr->phone_num_arr;
        content_str_ptr = sms_ptr->sms_content_arr;
        num_len = strlen((char*)num_str_ptr);
        content_len = strlen((char*)content_str_ptr);
        if (!MMIAPICOM_IsValidNumberString((char *)num_str_ptr, num_len))
        {
            MMIPUB_OpenAlertWarningWin(TXT_CC_INVALID_NUM);
        }
        else if (num_len > MMIBROWSER_PHONE_NUM_LEN)
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_PHONE_NUM_TOO_LONG);
        }
        else if (content_len > MMIBROWSER_SMS_CONTENT_LEN)
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_SMS_CONTENT_TOO_LONG);
        }
        else
        {
            content_wstr.wstr_len = content_len;
            content_wstr.wstr_ptr = SCI_ALLOCA((content_len +1 )* sizeof(wchar));
            if (PNULL != content_wstr.wstr_ptr)
            {
                SCI_MEMSET(content_wstr.wstr_ptr, 0, (content_len +1 )* sizeof(wchar));
                GUI_UTF8ToWstr(content_wstr.wstr_ptr,content_len,content_str_ptr,content_len);
                //SCI_TRACE_LOW:"[BRW]HandleControlSendSMSReq"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3029_112_2_18_2_6_1_135,(uint8*)"");
                MMIAPISMS_WriteNewMessage( MN_DUAL_SYS_MAX, &content_wstr, num_str_ptr, num_len );
                SCI_FREE(content_wstr.wstr_ptr);
             }
            else
            {
                //SCI_TRACE_LOW:"[BRW]HandleControlSendSmsReq content_wstr->wstr_ptr ALLOC Fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3035_112_2_18_2_6_1_136,(uint8*)"");
            }
    	}
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]HandleControlSendSmsReq:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3041_112_2_18_2_6_1_137,(uint8*)"");
    }
    
    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_SENDMMS_REQ or 
//                                       BRW_MSG_TYPE_SENDMMSTO_REQ
//  Global resource dependence : 
//  Author: jixin.xu
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlSendMmsReq(void *param)
{
#ifdef MMS_SUPPORT
    BRW_MSG_MAKE_CALL_REQ_T *mms_ptr = PNULL;
    MMI_STRING_T string_to = {0};

    if (PNULL != param)
    {
		uint32 len = 0;
		uint8 *str_ptr = PNULL;

        mms_ptr = (BRW_MSG_MAKE_CALL_REQ_T*)param;
		str_ptr = mms_ptr->phone_num_arr;
		len = strlen((char*)str_ptr);
        if (!MMIAPICOM_IsValidNumberString((char *)str_ptr, len))
        {
            MMIPUB_OpenAlertWarningWin(TXT_CC_INVALID_NUM);
        }

        else if (len > MMIBROWSER_PHONE_NUM_LEN)
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_PHONE_NUM_TOO_LONG);
        }
        else
        {
            //SCI_TRACE_LOW:"[BRW]HandleControlSendMMSReq"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3077_112_2_18_2_6_1_138,(uint8*)"");
            MMIAPIMMS_SetMMSEditEnterReason(MMIMMS_EDIT_FROM_PB_SEND);
            string_to.wstr_ptr= (wchar*)SCI_ALLOC_APP(strlen((char *)str_ptr) * sizeof(wchar));
            MMI_STRNTOWSTR(string_to.wstr_ptr, strlen((char *)str_ptr), (uint8 *)str_ptr, strlen((char *)str_ptr), strlen((char *)str_ptr));
            string_to.wstr_len = strlen((char *)str_ptr);
            MMIAPIMMS_AnswerMMS(MN_DUAL_SYS_MAX,PNULL,&string_to);
            SCI_FREE(string_to.wstr_ptr);
    	}
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]HandleControlSendMmsReq:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3088_112_2_18_2_6_1_139,(uint8*)"");
    }

#else
    MMIPUB_OpenAlertWarningWin(TXT_BROWSER_MMS_NOT_SUPPORT);
#endif

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message:BRW_MSG_TYPE_SENDSMAILTO_REQ
//  Global resource dependence : 
//  Author: jixin.xu
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleControlSendMailToReq(void *param)
{
#ifdef MMIEMAIL_SUPPORT
    BRW_MSG_SEND_MAILTO_REQ_T *email_name_ptr = PNULL;

    if (PNULL != param)
    {
        uint32 len = 0;
        uint8 *temp_str_ptr = PNULL;
        MMI_STRING_T email_send_to = {0};
        
        email_name_ptr = (BRW_MSG_SEND_MAILTO_REQ_T*)param;
		temp_str_ptr = email_name_ptr->email_name_arr;
        if (0 == (len = strlen((char *)temp_str_ptr)))
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMMON_MAIL_ADDR_INVALID);
        }
        else if (len > 0)
        {
            email_send_to.wstr_ptr = SCI_ALLOCA((len+1) * sizeof(wchar));
            if (PNULL != email_send_to.wstr_ptr)
            {
                SCI_MEMSET(email_send_to.wstr_ptr, 0, (len+1) * sizeof(wchar));
                GUI_UTF8ToWstr(email_send_to.wstr_ptr,len,temp_str_ptr,len);
                //SCI_TRACE_LOW:"[BRW]HandleControlSendMAILTOReq"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3126_112_2_18_2_6_1_140,(uint8*)"");
                email_send_to.wstr_len = len;
                
                MMIAPIEMAIL_EditEmail(&email_send_to);

                SCI_FREE(email_send_to.wstr_ptr);
            }
            else
            {
                //SCI_TRACE_LOW:"[BRW]HandleControlSendMailToReq temp_ptr ALLOC Fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3135_112_2_18_2_6_1_141,(uint8*)"");
            }
        }      
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]HandleControlSendMailToReq:wrong param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3141_112_2_18_2_6_1_142,(uint8*)"");
    }
#else
    MMIPUB_OpenAlertWarningWin(TXT_BROWSER_EMAIL_NOT_SUPPORT);
#endif
    return TRUE;
}

/*****************************************************************************/
//  Description : handle browser cc window message
//  Global resource dependence : 
//  Author: rong.gu
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleCCSelectWinMsg(
                                   MMI_WIN_ID_T win_id, 
                                   MMI_MESSAGE_ID_E msg_id,
                                   DPARAM param
                                   )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMIBROWSER_CC_INFO_T *cc_info = PNULL;

    cc_info = (MMIBROWSER_CC_INFO_T *)MMK_GetWinAddDataPtr(win_id);
    if (PNULL == cc_info)
    {
        return recode;
    }

    switch( msg_id )
    {       
    case MSG_OPEN_WINDOW:
        {
            MMI_HANDLE_T ctrl_handle = PNULL;
            GUIMENU_DYNA_ITEM_T node_item = {0};
            uint16 node_index = 0;
            MMI_STRING_T string = {0};
            //GUI_RECT_T rect = {0, MMI_CLIENT_RECT_TOP, MMI_MAINSCREEN_RIGHT_MAX_PIXEL, MMI_CLIENT_RECT_BOTTOM};
            GUI_BOTH_RECT_T both_rect = MMITHEME_GetWinClientBothRect(win_id);

            GUIMENU_CreatDynamic(&both_rect, win_id, MMIBROWSER_MENU_CC_SELECT_CTRL_ID, GUIMENU_STYLE_THIRD);
            ctrl_handle = MMK_GetCtrlHandleByWin(win_id, MMIBROWSER_MENU_CC_SELECT_CTRL_ID);

            SCI_MEMSET(&string, 0x00, sizeof(MMI_STRING_T));
            MMIRES_GetText(TXT_CALL, ctrl_handle, &string);
            GUIMENU_SetMenuTitle(&string, ctrl_handle);

            if ((cc_info->cc_option & MMIBROWSER_CC_OPTION_VOICE) != 0)
            {

                SCI_MEMSET(&string, 0x00, sizeof(MMI_STRING_T));
                MMIRES_GetText(TXT_CALL_AUDIO, ctrl_handle, &string);
                node_item.item_text_ptr = &string;
                node_item.select_icon_id = 0;
                GUIMENU_InsertNode(node_index++, ID_BROWSER_CC_VOICE, 0, &node_item, ctrl_handle);
            }

#ifdef VT_SUPPORT
            if ((cc_info->cc_option & MMIBROWSER_CC_OPTION_VIDEO) != 0)
            {
                SCI_MEMSET(&string, 0x00, sizeof(MMI_STRING_T));
                MMIRES_GetText(TXT_CALL_VIDEO, ctrl_handle, &string);
                node_item.item_text_ptr = &string;
                node_item.select_icon_id = 0;
                GUIMENU_InsertNode(node_index++, ID_BROWSER_CC_VIDEO, 0, &node_item, ctrl_handle);
            }
#endif

            if ((cc_info->cc_option & MMIBROWSER_CC_OPTION_IP) != 0)
            {
                SCI_MEMSET(&string, 0x00, sizeof(MMI_STRING_T));
                MMIRES_GetText(TXT_CALL_IPCALL, ctrl_handle, &string);
                node_item.item_text_ptr = &string;
                node_item.select_icon_id = 0;
                GUIMENU_InsertNode(node_index++, ID_BROWSER_CC_IP, 0, &node_item, ctrl_handle);
            }

            MMK_SetActiveCtrl(ctrl_handle, FALSE);
        }
        break;

    case MSG_CTL_OK:
    case MSG_CTL_PENOK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        {
            uint32 call_type = CC_CALL_NORMAL_CALL;
            uint32 menu_id = 0;
            MMI_HANDLE_T ctrl_handle = PNULL;
            uint8  phone_len = 0;

            ctrl_handle = MMK_GetCtrlHandleByWin(win_id, MMIBROWSER_MENU_CC_SELECT_CTRL_ID);
            menu_id = GUIMENU_GetCurNodeId(ctrl_handle);

            switch(menu_id) {
            case ID_BROWSER_CC_IP:
                call_type = CC_CALL_IP_CALL;
                break;
            case ID_BROWSER_CC_VIDEO:
                call_type = CC_CALL_VIDEO_CALL;
                break;

            //case ID_BROWSER_CC_VOICE:
            default:
                call_type = CC_CALL_NORMAL_CALL;
                break;
            }
            
            if (PNULL != cc_info->phone_num_ptr)
            {
                phone_len = strlen((char *)cc_info->phone_num_ptr);
            }

            if (MMIAPICOM_IsValidNumberString((char *)cc_info->phone_num_ptr, phone_len))//MS00223055
            {
                MMIAPICC_MakeCall(MN_DUAL_SYS_MAX, cc_info->phone_num_ptr, phone_len, PNULL, PNULL, CC_CALL_SIM_MAX, call_type, PNULL);
            }
            else
            {
                //提示无效号码
                 MMIPUB_OpenAlertWarningWin(TXT_CC_INVALID_NUM);
            }

            MMK_CloseWin(win_id);
        }

        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        if (PNULL != cc_info->phone_num_ptr)
        {
            SCI_FREE(cc_info->phone_num_ptr);
        }
        SCI_FREE(cc_info);
       break;

    default:
        break;
    }

    return recode;

}
#endif

/*****************************************************************************/
//  Description : handle download media file window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleDownladMediaFileWinMsg(
                                       MMI_WIN_ID_T win_id, 
                                       MMI_MESSAGE_ID_E msg_id, 
                                       DPARAM param
                                       )
{
    MMI_RESULT_E    result   = MMI_RESULT_TRUE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        BrwSetDownloadMediaFileWindow();
        GUIFORM_IsSlide(MMIBROWSER_DOWNLOAD_FORM_CTRL_ID, FALSE);//NEWMS00115263
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);
        break;
                
    case MSG_GET_FOCUS:
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);//MS00189926
        break;
    
    case MSG_APP_RED:
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        //stop download
        if (BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle()))
        {
            //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleDownladMediaFileWinMsg stop dl SUCC"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3320_112_2_18_2_6_1_143,(uint8*)"");
        }
        else
        {
            //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleDownladMediaFileWinMsg stop dl FAIL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3324_112_2_18_2_6_1_144,(uint8*)"");
        }
        MMIBROWSER_SetToStopDL();
        MMK_CloseWin(MMIBROWSER_SAVE_MEDIAFILE_WIN_ID);//@fen.xie 确保save mediafile window要close
        MMK_CloseWin(win_id);
  
        if (MSG_APP_RED == msg_id)
        {
            result = MMI_RESULT_FALSE;//MS00210727:return to idle
        }
        break;

    case MSG_CLOSE_WINDOW:
        MMIDEFAULT_AllowOpenKeylocWin(TRUE);//MS00189926
        break;            

    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
        {
            MMI_CTRL_ID_T       id = 0;

            if (PNULL != param)
            {
		        id = ((MMI_NOTIFY_T*) param)->src_id;

                if (MSG_CTL_MIDSK == msg_id)
                {
                    id = MMK_GetActiveCtrlId(win_id);
                }

                if (id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                    break;
                }
            }    
        }
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : set full lcd color
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_BrwSetFullLcd(GUI_COLOR_T back_color)
{
    GUI_BOTH_RECT_T          win_rect = MMITHEME_GetFullScreenBothRect();
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

    lcd_dev_info.lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id = GUI_BLOCK_MAIN;   
    
    if (!MMITHEME_IsMainScreenLandscape())
    {
        GUI_FillRect(&lcd_dev_info, win_rect.v_rect, back_color);
    }
    else
    {
        GUI_FillRect(&lcd_dev_info, win_rect.h_rect, back_color);
    }
}
/*****************************************************************************/
//  Description : display progress.MMIBROWSER_DOWNLOAD_PROGRESS_CTRL_ID的自绘制函数
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void MMIBROWSER_DisplayDownloadProgress(GUIOWNDRAW_INFO_T *owner_draw_ptr)
{
    uint32 pos = MMIBROWSER_GetRecvDataLen();
    uint32 totalsize = MMIBROWSER_GetTotalDataLen();
    uint16  progress_bar_height = 0;
    uint16  progress_bar_width = 0;
    GUI_RECT_T          progress_rect = {0};
    GUI_RECT_T          bg_rect = {0};
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    int16   cur_prg_len = 0;

    GUIRES_GetImgWidthHeight(&progress_bar_width, &progress_bar_height, IMAGE_BROWSER_PROGRESS_BAR, owner_draw_ptr->win_handle);
    
    bg_rect.left = owner_draw_ptr->display_rect.left + MMIBROWSER_DL_PROGRESS_MARGIN ;
    bg_rect.top = (owner_draw_ptr->display_rect.top+ owner_draw_ptr->display_rect.bottom - progress_bar_height)/2;
    bg_rect.right = owner_draw_ptr->display_rect.right - MMIBROWSER_DL_PROGRESS_MARGIN;
    bg_rect.bottom = bg_rect.top + progress_bar_height - 1;
    
    if (!GUI_IntersectRect(&bg_rect, bg_rect, owner_draw_ptr->display_rect))
    {
        //SCI_TRACE_LOW:"[BRW]MMIBROWSER_DisplayDownloadProgress GUI_IntersectRect FALSE"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3415_112_2_18_2_6_2_145,(uint8*)"");
        return;
    }

    progress_bar_width = (bg_rect.right  - bg_rect.left + 1);
    
    //if total size is unknown
    if (0 == totalsize)
    {
        totalsize = pos + MMIBROWSER_DOWNLOAD_EXT_LEN;
    }
    else if (pos > totalsize)
    {
        pos = totalsize;
    }
    
    GUIRES_DisplayImg(PNULL,
            &bg_rect,
            PNULL,
            owner_draw_ptr->win_handle,
            IMAGE_BROWSER_PROGRESS_BG,
            &lcd_dev_info);
            
    if (0 != pos)//display progress bar
    {
        SCI_MEMCPY(&progress_rect, &bg_rect, sizeof(GUI_RECT_T));
        if (pos != totalsize)
        {
            cur_prg_len = (progress_bar_width * ((uint64)pos))/totalsize;//MS00222366
            progress_rect.right = bg_rect.left + cur_prg_len;
            if (progress_rect.right > bg_rect.right)
            {
                progress_rect.right = bg_rect.right;
            }
        }
            
        GUIRES_DisplayImg(PNULL,
                &progress_rect,
                PNULL,
                owner_draw_ptr->win_handle,
                IMAGE_BROWSER_PROGRESS_BAR,
                &lcd_dev_info);
    }
}
/*****************************************************************************/
//  Description : display address bar bg
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayAdressBarImg(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO	                lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_RECT_T  addr_rect = {0};
    
    if (!MMITHEME_IsMainScreenLandscape())
    {
        SCI_MEMCPY(&addr_rect, &s_brw_show_info.addr_bar.v_rect, sizeof(GUI_RECT_T));
    }
    else
    {
        SCI_MEMCPY(&addr_rect, &s_brw_show_info.addr_bar.h_rect, sizeof(GUI_RECT_T));
    }
    
    GUIRES_DisplayImg(PNULL,
            &addr_rect,
            PNULL,
            win_id,
            IMAGE_BROWSER_TOOLBAR,
            &lcd_dev_info);
}
#ifdef BROWSER_TOOLBAR
/*****************************************************************************/
//  Description : display toolbar bg
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayToolBarImg(MMI_WIN_ID_T win_id, BOOLEAN is_need_update)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_RECT_T  toolbar_rect = {0};
       
    if (!MMITHEME_IsMainScreenLandscape())
    {
        SCI_MEMCPY(&toolbar_rect, &s_brw_show_info.toolbar.v_rect, sizeof(GUI_RECT_T));
    }
    else
    {
        SCI_MEMCPY(&toolbar_rect, &s_brw_show_info.toolbar.h_rect, sizeof(GUI_RECT_T));
    }

    MMIBROWSER_ClearToolBar(MMI_WHITE_COLOR);//for stop/refresh button
    GUIRES_DisplayImg(PNULL,
            &toolbar_rect,
            PNULL,
            win_id,
            IMAGE_BROWSER_TOOLBAR,
            &lcd_dev_info);
}

/*****************************************************************************/
//  Description : clear toolbar rect
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearToolBar(GUI_COLOR_T back_color)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

    if (!MMITHEME_IsMainScreenLandscape())
    {
        GUI_FillRect(&lcd_dev_info, s_brw_show_info.toolbar.v_rect, back_color);
    }    
    else
    {
        GUI_FillRect(&lcd_dev_info, s_brw_show_info.toolbar.h_rect, back_color);
    }
}

#endif    

/*****************************************************************************/
//  Description : display progress
//  Global resource dependence : 
//  Author: fenxie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayAddrLabelBg(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_RECT_T  bg_rect = {0};
       
    if (!MMITHEME_IsMainScreenLandscape())
    {
        SCI_MEMCPY(&bg_rect, &s_brw_show_info.addr_label.v_rect, sizeof(GUI_RECT_T));
    }
    else
    {
        SCI_MEMCPY(&bg_rect, &s_brw_show_info.addr_label.h_rect, sizeof(GUI_RECT_T));
    }

    //使label 内的字串与边框有一点距离,美观
    bg_rect.left--;
    bg_rect.right++;
            
    GUIRES_DisplayImg(PNULL,
            &bg_rect,
            PNULL,
            win_id,
            IMAGE_BROWSER_LABEL_BG,
            &lcd_dev_info);
}


/*****************************************************************************/
//  Description : display address bar(address bar bg, url label bg, process, label ctrl, animal ctrl)
//  Global resource dependence : 
//  Author: fen.xie
//  Note:  address bar的前半办法表示的是页面下载进度;后半部分表示的是子媒体下载进度
/*****************************************************************************/
PUBLIC void MMIBROWSER_DisplayAdressBar(uint32 pos, uint32 totalsize)
{
    MMI_WIN_ID_T    win_id = MMIBROWSER_MAIN_WIN_ID;
    if (MMIBROWSER_IsMainActive())
    {
        MMIBROWSER_ClearAddrBar(MMI_WHITE_COLOR);
        
        DisplayAdressBarImg(win_id);//address bar bg(PNG)
        DisplayAddrLabelBg(win_id);//address label bg(PNG)

        MMIBROWSER_DisplayProcess(pos, totalsize, win_id);//process img(PNG)

        MMK_SendMsg(MMIBROWSER_LABEL_ADDRESS_CTRL_ID, MSG_CTL_PAINT, PNULL);//display url label
        GUIANIM_UpdateDisplay(MMIBROWSER_ANIMATE_CONNECTING_CTRL_ID);//display animal control
    }
}

/*****************************************************************************/
//  Description : display progress(loading webpage)
//  Global resource dependence : 
//  Author:  fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_DisplayProcess(uint32 pos, uint32 totalsize, MMI_WIN_ID_T win_id)
{
    GUI_RECT_T          rect = {0};
//     GUI_POINT_T         dis_point = {0};
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    int16   cur_prg_len = 0;

    if(!(MMIBROWSER_IsMainActive()) || (0 == pos))
    {
        return;
    }
    
    //if total size is unknown
    if (0 == totalsize)
    {
        totalsize = pos+MMIBROWSER_DOWNLOAD_EXT_LEN;
    }
    else if (pos > totalsize)
    {
        pos = totalsize;
    }
    
    //display scroll bar
    if (!MMITHEME_IsMainScreenLandscape())
    {
        rect.left = s_brw_show_info.addr_label.v_rect.left;
        rect.top = s_brw_show_info.addr_label.v_rect.top;
        cur_prg_len =  (uint16)(s_brw_show_info.addr_label.v_rect.right - s_brw_show_info.addr_label.v_rect.left + 1) * ((uint64)pos)/ totalsize;
        rect.right = s_brw_show_info.addr_label.v_rect.left + cur_prg_len;
        rect.bottom = s_brw_show_info.addr_label.v_rect.bottom;
        //防止滚动条长度超过进度条
        if (s_brw_show_info.addr_label.v_rect.right < rect.right)
        {
            rect.right = s_brw_show_info.addr_label.v_rect.right;
        }
   }
    else
    {
        rect.left = s_brw_show_info.addr_label.h_rect.left;
        rect.top = s_brw_show_info.addr_label.h_rect.top;
        cur_prg_len =  (uint16)(s_brw_show_info.addr_label.h_rect.right - s_brw_show_info.addr_label.h_rect.left + 1) * ((uint64)pos) / totalsize;
        rect.right = s_brw_show_info.addr_label.h_rect.left + cur_prg_len;
        rect.bottom = s_brw_show_info.addr_label.h_rect.bottom;;
        //防止滚动条长度超过进度条
        if (s_brw_show_info.addr_label.h_rect.right < rect.right)
        {
            rect.right = s_brw_show_info.addr_label.h_rect.right;
        }
    }
    
    if (rect.right > rect.left)
    {
        GUIRES_DisplayImg(PNULL,
            &rect,
            PNULL,
            win_id,
            IMAGE_BROWSER_LOADING_PROCESS,
            &lcd_dev_info);
    }

}

/*****************************************************************************/
//  Description : open the file detail window of browser
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenFileDetailWin(void)
{
//#ifdef MMI_PDA_SUPPORT//NEWMS00122487
//       BrwOpenPropertyWin();
//#else
    MMK_CreateWin((uint32*)BRW_FILE_DETAIL_WIN_TAB,PNULL);
//#endif /* MMI_PDA_SUPPORT */
}

/*****************************************************************************/
//  Description : open the sim select window of browser
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenSimSelectWin(void)
{
#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE)
{
    MMI_WIN_ID_T	selectSimWinID = MMIBROWSER_COM_SELECTSIM_WIN_ID;
    MMIPHONE_SELECT_SIM_DATA_T  caller_data={0};

    caller_data.append_type = (uint32)MMISET_APPEND_SYS_OK;
    caller_data.callback_func =BrwSIMSelectCallback ;
    caller_data.select_type= MMIPHONE_SELECTION_SIM_WLAN;
    caller_data.publist_id = MMIBROWSER_COM_SELECT_SIM_CTRL_ID;
    caller_data.win_id_ptr = &selectSimWinID;
    caller_data.user_data  = PNULL;
    caller_data.extra_data = PNULL;

    MMIAPIPHONE_OpenSelectSimWin(&caller_data);
}

#endif    
}


/*****************************************************************************/
//  Description : clear the window of browser(address bar and web_page---need update when on web)
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearClientLcd(GUI_COLOR_T back_color)
{
    MMIBROWSER_ClearAddrBar(back_color);
    MMIBROWSER_ClearWebpage(back_color);
}

/*****************************************************************************/
//  Description : clear the window of browser(address bar and web_page---need update when on web)
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearWebpage(GUI_COLOR_T back_color)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T web_rect = {0};
    uint16  scroll_width = 0;

#if !defined(MMI_PDA_SUPPORT)
    GUIRES_GetImgWidthHeight(&scroll_width, PNULL, IMAGE_BROWSER_SCROLL_VER_GROOVE, MMIBROWSER_MAIN_WIN_ID);
#endif

    if (!MMITHEME_IsMainScreenLandscape())
    {
        web_rect = s_brw_show_info.web_page.v_rect;
    }    
    else
    {
        web_rect = s_brw_show_info.web_page.h_rect;
    }

    web_rect.right += scroll_width;

    GUI_FillRect(&lcd_dev_info, web_rect, back_color);
}

/*****************************************************************************/
//  Description : clear the window of browser(address bar and web_page---need update when on web)
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ClearAddrBar(GUI_COLOR_T back_color)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

    if (!MMITHEME_IsMainScreenLandscape())
    {
        GUI_FillRect(&lcd_dev_info, s_brw_show_info.addr_bar.v_rect, back_color);
    }    
    else
    {
        GUI_FillRect(&lcd_dev_info, s_brw_show_info.addr_bar.h_rect, back_color);
    }
}

/*****************************************************************************/
//  Description : set timer to check repeating key
//  Global resource dependence : 
//  Author: jiqun.li
//  Note: 
/*****************************************************************************/
LOCAL void MMIBROWSER_StartKeyRepeatTimer(MMI_WIN_ID_T win_id, uint32 timeout)
{
    uint8 key_timer_id = MMI_NONE_TIMER_ID;

    if(MMI_NONE_TIMER_ID != MMIBROWSER_GetKeyTimerID()) {
        return;
    }
    //SCI_TRACE_LOW:"\n MMIBROWSER start key timer\n"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3745_112_2_18_2_6_2_146,(uint8*)"");
    key_timer_id = MMK_CreateWinTimer((MMI_HANDLE_T)win_id, timeout, FALSE);
    MMIBROWSER_SetKeyTimerID(key_timer_id);
}

/*****************************************************************************/
//  Description : stop key repeating timer
//  Global resource dependence : 
//  Author: jiqun.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIBROWSER_StopKeyRepeatTimer(void)
{
    if(MMI_NONE_TIMER_ID == MMIBROWSER_GetKeyTimerID()) {
        return FALSE;
    }
    //SCI_TRACE_LOW:"\n MMIBROWSER stop key timer\n"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3759_112_2_18_2_6_2_147,(uint8*)"");
    MMK_StopTimer(MMIBROWSER_GetKeyTimerID());
    return TRUE;
}

/*****************************************************************************/
//  Description : handle main window open win message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwHandleMainOpenWinMsg(MMI_WIN_ID_T	win_id)
{
    MMI_CTRL_ID_T   address_label_ctrl_id=MMIBROWSER_LABEL_ADDRESS_CTRL_ID;
    BRW_PROFILE_T   profile={0};
    BRW_INSTANCE    brw_instance = 0;
    char            *url_ptr = PNULL;
    uint32          scroll_handle = 0;
#ifdef BROWSER_TOOLBAR
    uint32           i = 0;
    GUI_BG_T        bg ={0};
#endif /* BROWSER_TOOLBAR */
#if !defined(MMI_PDA_SUPPORT)
    MMI_IMAGE_ID_T  scroll_bg_id = IMAGE_BROWSER_SCROLL_VER_GROOVE;
#else
    MMI_IMAGE_ID_T  scroll_bg_id = IMAGE_NULL;
#endif

    //GUI_BG_T    gray_bg = {0};
    
    InitBrwPositonInfo(win_id);

    //MMI_Enable3DMMI(FALSE);
    /*MS00192609:目前dorado浏览器支持背景播放,若以后有些项目或平台有限制,可增加开关重新打开MMIAUDIO_PauseBgPlay*/
    //MMIAUDIO_PauseBgPlay(MMIBGPLAY_MODULE_WAP_MAINMENU);
    MMIBROWSER_SetIntanceInit(FALSE);
#ifdef BROWSER_TOOLBAR
    bg.bg_type = GUI_BG_IMG;

    for (i = 0; i < ARRAY_SIZE(s_button_ctrl_id); i++)
    {
        if (PNULL != s_button_ctrl_id[i].button_proc)
        {
            GUIBUTTON_SetCallBackFunc(s_button_ctrl_id[i].button_ctrl_id, s_button_ctrl_id[i].button_proc);
        }

        GUIBUTTON_SetRunSheen(s_button_ctrl_id[i].button_ctrl_id, FALSE);

        bg.img_id = IMAGE_BROWSER_TOOLBAR_BUTTON_BG;
        GUIBUTTON_SetBg(s_button_ctrl_id[i].button_ctrl_id, &bg);

        bg.img_id = IMAGE_BROWSER_TOOLBAR_PRESSED_BUTTON_BG;
        GUIBUTTON_SetPressedBg(s_button_ctrl_id[i].button_ctrl_id, &bg);

        bg.img_id = s_button_ctrl_id[i].fg_image_id;
        GUIBUTTON_SetFg(s_button_ctrl_id[i].button_ctrl_id, &bg);

        bg.img_id = s_button_ctrl_id[i].pressed_fg_image_id;
        GUIBUTTON_SetPressedFg(s_button_ctrl_id[i].button_ctrl_id, &bg);

        bg.img_id = s_button_ctrl_id[i].grayed_fg_image_id;
        GUIBUTTON_SetGrayedFg(s_button_ctrl_id[i].button_ctrl_id, FALSE, &bg, FALSE);
    }
#endif
    
    //scroll
    {
        
        scroll_handle = BrwScrollBarCreate(win_id, &s_brw_show_info.scroll, scroll_bg_id);
        MMIBROWSER_SetScrollHandle(scroll_handle);
    }

    //label
    {
        GUIAPICTRL_SetBothRect(address_label_ctrl_id, &s_brw_show_info.addr_label);
        IGUICTRL_SetState(MMK_GetCtrlPtr(address_label_ctrl_id), GUICTRL_STATE_DISABLE_ACTIVE, TRUE);//MS00187890
        GUILABEL_SetBackgroundColor(address_label_ctrl_id,MMI_TRANSPARENCE_COLOR);
        GUILABEL_SetFont(address_label_ctrl_id,MMIBROWSER_ADDR_LABEL_FONT,MMI_BLACK_COLOR);
    }
    //anim
    {
        GUIAPICTRL_SetBothRect(MMIBROWSER_ANIMATE_CONNECTING_CTRL_ID, &s_brw_show_info.connect_anim);
    }

#ifdef BROWSER_TOOLBAR
    //button form
    {
#ifdef MAINLCD_SIZE_320X480
        uint16  magin_x = 0;
        uint16  magin_y = 6;
#elif defined MAINLCD_SIZE_240X320
        uint16  magin_x = 0;
        uint16  magin_y = 5;
#elif defined MAINLCD_SIZE_240X400
        uint16  magin_x = 0;
        uint16  magin_y = 5;
#else
        uint16  magin_x = 0;
        uint16  magin_y = 0;
#endif
        uint16  img_width = 0;
        uint16  img_height = 0;
        uint16			ver_space = 0;
        uint16			hor_space = 0;
        GUI_BG_T		form_bg = {0};
        GUI_BOTH_RECT_T both_rect = {0};
        GUI_BOTH_RECT_T client_rect = {0};
        
        client_rect = MMITHEME_GetWinClientBothRect(win_id);

        GUIRES_GetImgWidthHeight(&img_width, &img_height, s_button_ctrl_id[0].fg_image_id, win_id);

        both_rect.v_rect.left = client_rect.v_rect.left + magin_x;
        both_rect.v_rect.right = client_rect.v_rect.right - magin_x;
        both_rect.v_rect.top = s_brw_show_info.toolbar.v_rect.top + magin_y;
        both_rect.v_rect.bottom = MIN(s_brw_show_info.toolbar.v_rect.bottom, (both_rect.v_rect.top + img_height));

        both_rect.h_rect.left = client_rect.h_rect.left + magin_x;
        both_rect.h_rect.right = client_rect.h_rect.right - magin_x;
        both_rect.h_rect.top = s_brw_show_info.toolbar.h_rect.top + magin_y;
        both_rect.h_rect.bottom = MIN(s_brw_show_info.toolbar.h_rect.bottom, (both_rect.h_rect.top + img_height));
        GUIAPICTRL_SetBothRect(MMIBROWSER_BUTTON_FORM_CTRL_ID, &both_rect);
        GUIFORM_SetSpace(MMIBROWSER_BUTTON_FORM_CTRL_ID, &hor_space, &ver_space);
        GUIFORM_SetMargin(MMIBROWSER_BUTTON_FORM_CTRL_ID, 0);
        form_bg.bg_type = GUI_BG_NONE;
        GUIFORM_SetBg( MMIBROWSER_BUTTON_FORM_CTRL_ID, &form_bg);
    }
#endif

    profile = MMIBROWSER_SetProfile();
    
    //init brw instance
    brw_instance = BRW_InitInstance(&profile, FALSE);
    if (0 == brw_instance)
    {
        MMIPUB_OpenAlertFailWin(TXT_BROWSER_INIT_FAIL);
        MMK_CloseWin(win_id);
        //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_InitInstance error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3869_112_2_18_2_6_2_148,(uint8*)"");
        return;
    }
    MMIBROWSER_SetInstance(brw_instance);
    MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_NONE, FALSE);
    //MMIBROWSER_SetIsNeedClear(TRUE);
    
    MMIBROWSER_SetIntanceInit(TRUE);

    switch (MMIBROWSER_GetWebEntryType())
    {
    case MMIBROWSER_WEB_ENTRY_URL:
        {
            wchar   *ico_file_name_ptr = MMIBROWSER_GetIcoFilename();
            BOOLEAN is_access_fail = FALSE;
            
            url_ptr = MMIBROWSER_GetUrl();
            
            if ((PNULL != url_ptr) && (0 != url_ptr[0]))
            {
                if (PNULL == ico_file_name_ptr)
                {
                    if (0 != BRW_AccessPage(MMIBROWSER_GetInstance(),(uint8*)url_ptr))
                    {
                    }
                    else
                    {
                        is_access_fail = TRUE;
                    }
                }
                else//need to load the ico of the url
                {
                    if (0 != BRW_AccessPageEx(brw_instance,(uint8*)url_ptr,ico_file_name_ptr))
                    {
                    }
                    else
                    {
                        is_access_fail = TRUE;
                    }
                }

                if (!is_access_fail)
                {
                    MMIBROWSER_SetToAccess(url_ptr, FALSE);
                }
                else
                {
                    //虽然access错误,但是仍需要update address label
                    wchar   wurl_arr[MMIBROWSER_MAX_URL_LEN + 1] = {0};
                    MMI_STRING_T    temp_str = {0};

                    CFLWCHAR_StrNCpyWithAsciiStr(wurl_arr, url_ptr, MMIBROWSER_MAX_URL_LEN + 1);
                    temp_str.wstr_ptr = wurl_arr;
                    temp_str.wstr_len = (uint16)CFLWCHAR_StrLen(wurl_arr);

                    GUILABEL_SetText(MMIBROWSER_LABEL_ADDRESS_CTRL_ID, &temp_str, FALSE);
                
                    //SCI_TRACE_LOW:"BrwHandleMainOpenWinMsg: BRW_AccessPage error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3926_112_2_18_2_6_3_149,(uint8*)"");
                } 
            }
            else
            {
                MMIBROWSER_AlertUrlInvalid();
            }
        }
        break;
        
    case MMIBROWSER_WEB_ENTRY_SNAPSHOT:
        MMK_PostMsg(MMIBROWSER_MAIN_WIN_ID, MSG_BRW_ENTER_SNAPSHOT_FILENAME, 
                    MMIBROWSER_GetSnapshotFilename(), 
                    sizeof(wchar)* (MMIAPICOM_Wstrlen(MMIBROWSER_GetSnapshotFilename()) + 1));
        break;
        
    default:
        break;
    }
    
    MMIBROWSER_FreeIcoFilename();
}
/*****************************************************************************/
//  Description : handle main window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMainWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )

{
    MMI_RESULT_E                        ret=MMI_RESULT_TRUE;
    BRW_INSTANCE                        brw_instance = 0;
    uint8                               timer_id = 0;
    uint32 pos = 0;
    uint32 totalsize = 0;
    /*s_init_hor_info由于目前API未实现同时得到一个图片横屏和竖屏的size,故使用该变量记录第一次转
        屏的情况 并计算此时图片size得到横屏尺寸*/
    LOCAL   uint16 s_init_hor_info = 0;
#ifdef TOUCHPANEL_TYPE_MULTITP
    LOCAL BOOLEAN s_is_need_update_font_to_nv= FALSE;//whether the font has been modify by zoom,and need to update to nv data
#endif    

    //SCI_TRACE_LOW:"BrwHandleMainWinMsg:msg_id = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_3971_112_2_18_2_6_3_150,(uint8*)"d", msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_PDA_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);//MS00189926
        BrwHandleMainOpenWinMsg(win_id);
        s_init_hor_info = 0;

        //set form not support slide
        GUIFORM_IsSlide(MMIBROWSER_BUTTON_FORM_CTRL_ID,FALSE);
#ifdef TOUCHPANEL_TYPE_MULTITP
        s_is_need_update_font_to_nv= FALSE;//whether the font has been modify by zoom,and need to update to nv data
#endif    
        break;

    case MSG_LCD_SWITCH:
    {
        BRW_PROFILE_T   profile={0};
        LCD_ANGLE_E cur_lcd_angle = LCD_ANGLE_0;
      
        //InitBrwPositonInfo(win_id);
        if (0 == s_init_hor_info)
        {
            cur_lcd_angle = GUILCD_GetLogicAngle(MAIN_LCD_ID);
            if ((LCD_ANGLE_0 == cur_lcd_angle) || (LCD_ANGLE_180 == cur_lcd_angle))
            {
                InitBrwVerPositonInfo(win_id);
            }
            else
            {
                InitBrwHorPositonInfo(win_id);
            }
            GUIAPICTRL_SetBothRect(MMIBROWSER_PRGBOX_CTRL_ID, &s_brw_show_info.scroll);
            GUIAPICTRL_SetBothRect(MMIBROWSER_ANIMATE_CONNECTING_CTRL_ID, &s_brw_show_info.connect_anim);
            GUIAPICTRL_SetBothRect(MMIBROWSER_LABEL_ADDRESS_CTRL_ID, &s_brw_show_info.addr_label);

            s_init_hor_info++;
        }

        profile = MMIBROWSER_SetProfile();
        BRW_SetParam(MMIBROWSER_GetInstance(), &profile, BRW_SCREEN_PROFILE);
        //MMIBROWSER_SetIsNeedClear(TRUE);
    }
        break; 
        
    case MSG_FULL_PAINT:
        //if (MMIBROWSER_GetIsNeedClear())//when open, lose focus, lcd switch
        {
            MMIBROWSER_ClearClientLcd(MMI_WHITE_COLOR);
            
#ifdef BROWSER_TOOLBAR
            BrowserChangeButtonStatus(FALSE);
#endif
            if (MMIBROWSER_GetLoading())
            {
                MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_ANIM, FALSE);
#ifdef BROWSER_TOOLBAR
                BrwChangeRefreshButtonImg(TRUE, FALSE);
#else
                GUIWIN_SetSoftkeyTextId(win_id, STXT_OPTION, TXT_NULL, STXT_STOP, FALSE);
#endif
            }            
            else
            {            
                MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_STOP, FALSE);
#ifdef BROWSER_TOOLBAR
                BrwChangeRefreshButtonImg(FALSE, FALSE);
#else
                if (BRW_NavHisIsBackwardEnable())
                {
                    GUIWIN_SetSoftkeyTextId(win_id, STXT_OPTION, TXT_NULL, TXT_COMMON_COMMON_BACK, FALSE);
                }
                else
                {
                    GUIWIN_SetSoftkeyTextId(win_id, STXT_OPTION, TXT_NULL, TXT_COMMON_CLOSE, FALSE);
                }
#endif
            }
        }
        
        //cr150614 if the brw main win lose focus, when it get focus, need display the progress on address bar
        if (0 != MMIBROWSER_GetRecvDataLen())//页面下载时:进度条的前半部分表示页面下载的进度
        {
            totalsize = MMIBROWSER_GetTotalDataLen();
            pos = MMIBROWSER_GetRecvDataLen();
          
            if (0 == totalsize)//protect
            {
                totalsize = (pos + MMIBROWSER_DOWNLOAD_EXT_LEN)*2;
            }
            else
            {
                if (pos > totalsize)//protect
                {
                    pos = totalsize;
                }
                totalsize *= 2;
            }
        }
        else if (0 != MMIBROWSER_GetTotalMediaNum())//页面下载已完毕,下载media时
        {
            if (MMIBROWSER_GetCurrMediaNum() >= MMIBROWSER_GetTotalMediaNum())//complete
            {
            }
            else
            {
                //进度条的后半部分表示下载media的进度
                totalsize = MMIBROWSER_GetTotalMediaNum();
                pos = MMIBROWSER_GetCurrMediaNum() + totalsize;
                totalsize *= 2;
                pos = (pos < totalsize)? pos: totalsize;//protect
            }
        }
        else// do nothing
        {
        }

        //[BEGIN]prepare for display address bar 
        MMIBROWSER_ClearAddrBar(MMI_WHITE_COLOR);
        DisplayAdressBarImg(win_id);
        DisplayAddrLabelBg(win_id);
        MMIBROWSER_DisplayProcess(pos, totalsize, win_id);//此时只需要显示进度条区域
        //[END]prepare for display address bar 

#ifdef BROWSER_TOOLBAR
        DisplayToolBarImg(win_id, TRUE);
#endif

        //SCI_TRACE_LOW:"MMIBROWSER DISPLAY = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4105_112_2_18_2_6_3_151,(uint8*)"d",MMIBROWSER_GetNeedDisplay());
        
        brw_instance = MMIBROWSER_GetInstance();

        if (MMIBROWSER_GetNeedDisplay())//主要用于当需要显示页面,但browser当时却不处于focus时作为一个刷新也页面内容的记录
        {
            if ((0 != brw_instance) && BRW_DisplayPage(brw_instance))
            {
                MMIBROWSER_SetNeedDisplay(FALSE);
            }
            else
            {
                //SCI_TRACE_LOW:"[BRW]W BrwHandleMainWinMsg BRW_DisplayPage error instance=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4117_112_2_18_2_6_3_152,(uint8*)"d", brw_instance);
            }
        }
        else //if (MMIBROWSER_GetIsNeedClear())
        {
            if ((0 != brw_instance) && BRW_RepaintPage(brw_instance))
            {
            }
            else
            {
                //SCI_TRACE_LOW:"[BRW]W BrwHandleMainWinMsg BRW_RepaintPage error instance=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4127_112_2_18_2_6_3_153,(uint8*)"d", brw_instance);
            } 
            //MMIBROWSER_SetIsNeedClear(FALSE);
        }
        /*else
        {
            
        }*/
        break;

    case MSG_LOSE_FOCUS:
        //MMIBROWSER_SetIsNeedClear(TRUE);
#ifdef TOUCHPANEL_TYPE_MULTITP
        if (s_is_need_update_font_to_nv)//whether the font has been modify by zoom,and need to update to nv data
        {
            MMIBROWSER_UpdateNVSetting();
            s_is_need_update_font_to_nv = FALSE;
        }
#endif    
        break;

    case MSG_GET_FOCUS:
        //MMIBROWSER_SetIsNeedClear(TRUE);
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);//MS00189926
        break;

    case MSG_CLOSE_WINDOW: 
        MMIBROWSER_StopKeyRepeatTimer();
        MMIBROWSER_SetKeyTimerID(MMI_NONE_TIMER_ID);
        MMIBROWSER_SetKeyMSG(0);
        
        MMIBROWSER_ExitWeb();
#ifdef BROWSER_START_PAGE_THEME1
        if (MMK_IsOpenWin(MMIBROWSER_START_PAGE_WIN_ID))
        {
            MMIBROWSER_SetSlide(TRUE);
            MMISLIDE_SwitchToWin(&s_startpage_win_slide_handle, MMIBROWSER_START_PAGE_WIN_ID);
            MMIBROWSER_SetIsHistoryUpdate(TRUE);
        }
#endif
        break;

    case MSG_KEYPRESSUP_LEFT:   //fall through
    case MSG_KEYPRESSUP_DOWN:   //fall through
    case MSG_KEYPRESSUP_RIGHT:  //fall through
    case MSG_KEYPRESSUP_UP:     //fall through
    case MSG_KEYUP_LEFT:        //fall through
    case MSG_KEYUP_DOWN:        //fall through
    case MSG_KEYUP_RIGHT:       //fall through
    case MSG_KEYUP_UP:
        MMIBROWSER_StopKeyRepeatTimer();
        MMIBROWSER_SetKeyTimerID(MMI_NONE_TIMER_ID);
        MMIBROWSER_SetKeyMSG(0);
        break;

    case MSG_APP_UP:
        brw_instance = MMIBROWSER_GetInstance();
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_UP))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 1"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4180_112_2_18_2_6_3_154,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;

    case MSG_APP_DOWN:
        brw_instance = MMIBROWSER_GetInstance();
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_DOWN))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 2"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4193_112_2_18_2_6_3_155,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;

    case MSG_APP_LEFT:
        brw_instance = MMIBROWSER_GetInstance();
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_LEFT))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 3"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4206_112_2_18_2_6_3_156,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;

    case MSG_APP_RIGHT:
        brw_instance = MMIBROWSER_GetInstance();
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_RIGHT))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 4"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4219_112_2_18_2_6_3_157,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;      

    case MSG_CTL_MIDSK://MS00188753
    case MSG_APP_WEB:
        brw_instance = MMIBROWSER_GetInstance();
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_OK))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 7"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4233_112_2_18_2_6_3_158,(uint8*)"");
            }
        }
        break;
        
    case MSG_BRW_ENTER_URL://输入了新的url
        {
//             uint32 req_id=0;
            char    *url_ptr = PNULL;
            wchar   wurl_arr[MMIBROWSER_MAX_URL_LEN + 1] = {0};

            url_ptr = (char*)param;
            brw_instance = MMIBROWSER_GetInstance();
            if ((PNULL != url_ptr) && (0 != url_ptr[0]) && (brw_instance != BRW_INVALID_INSTANCE))
            {
                //enter new url时,无论是否成功都应刷新label 的url
                MMIBROWSER_UpdateUrl(url_ptr);
                CFLWCHAR_StrNCpyWithAsciiStr(wurl_arr, url_ptr, MMIBROWSER_MAX_URL_LEN + 1);
                MMIBROWSER_UpdateAddrLable(wurl_arr, FALSE);
                if(MMIBROWSER_GetKernelInit())
                {
                    if (0 == BRW_AccessPage(brw_instance,(uint8*)url_ptr))
                    {
                        MMIBROWSER_SetToStop();
                        //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_AccessPage error"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4257_112_2_18_2_6_3_159,(uint8*)"");
                    }
                    else
                    {
                        MMIBROWSER_SetToAccess(url_ptr, TRUE);//full paint时再刷新，此时只设置
                    }
                }   
            }
        }
        break;  
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL://MS00188753
//#ifndef BROWSER_TOOLBAR//MS00250425
        //the func of right softkey:first->stop; second->backward;third->quit
        if (MMIBROWSER_GetLoading())//stop loading
        {
            if (BRW_StopPageReq(MMIBROWSER_GetInstance()))
            {
                MMIBROWSER_SetLoading(FALSE);
                MMIBROWSER_SetParamToStop();
            }
            else
            {
                //SCI_TRACE_LOW:"BrwHandleMainWinMsg:MSG_CTL_CANCEL BRW_StopPageReq error"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4281_112_2_18_2_6_3_160,(uint8*)"");
            }
        }
        else
//#endif        
        {
            //the func of right softkey is same to backward button
            BackButtonCallBack();//backward
        }
        break;
        
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
#if 0//NEWMS00125869:目前平台都响应menu键，统一       
#ifdef MMI_PDA_SUPPORT
        if (MMITHEME_IsIstyle())
        {
            break;//istyle without pop menu        
        }
#endif
#endif

        MMK_CreateWin((uint32*)MMIBROWSER_MORE_POPMENU_WIN_TAB,PNULL);
        GUIMENU_CreatDynamic(PNULL,MMIBROWSER_MAIN_POPMENU_WIN_ID,MMIBROWSER_MENU_CTRL_ID,GUIMENU_STYLE_POPUP);
        break;

    case MSG_TIMER:
        {
            timer_id = *(uint8*)param;
            brw_instance = MMIBROWSER_GetInstance();

            if(timer_id == MMIBROWSER_GetKeyTimerID()) 
            {
                /*key timer*/
                MMIBROWSER_StopKeyRepeatTimer();
                MMIBROWSER_SetKeyTimerID(MMI_NONE_TIMER_ID);
                if (MMK_IsFocusWin(win_id) && brw_instance!=BRW_INVALID_INSTANCE) 
                {
                    if(MSG_APP_LEFT==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_LEFT_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    } 
                    else if (MSG_APP_RIGHT==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_RIGHT_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    } 
                    else if (MSG_APP_UP==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_UP_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    } 
                    else if (MSG_APP_DOWN==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_DOWN_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    }
                }
            } 
            else if (s_browser_tp_info.slide_timer_id == timer_id)
            {
                HandleSlideTimer(win_id,brw_instance);
            }
            else 
            {
                /*layout timer*/
                if(MMK_IsFocusWin(win_id) && !MMIBROWSER_GetLoading() && (brw_instance!=BRW_INVALID_INSTANCE)) 
                {
                    if (!BRW_ProcLayoutTimer(brw_instance,timer_id)) 
                    {
                        //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcLayoutTimer error"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4353_112_2_18_2_6_3_161,(uint8*)"");
                    }                    
                } 
                else 
                {
                    BRW_StopLayoutTimerById(brw_instance, timer_id);
                }
            }
        }
        break;
        
    case MSG_CTL_PRGBOX_MOVEUP_ONE:
        brw_instance = MMIBROWSER_GetInstance();
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_UPLINE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 1"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4370_112_2_18_2_6_3_162,(uint8*)"");
            }
        }
        
        break;

    case MSG_CTL_PRGBOX_MOVEDOWN_ONE:
        brw_instance = MMIBROWSER_GetInstance();
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_DOWNLINE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 2"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4382_112_2_18_2_6_3_163,(uint8*)"");
            }
        }
        break;

    case MSG_CTL_PRGBOX_MOVEUP_PAGE:
        brw_instance = MMIBROWSER_GetInstance();
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_UPPAGE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 3"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4393_112_2_18_2_6_4_164,(uint8*)"");
            }
        }
        break;

    case MSG_CTL_PRGBOX_MOVEDOWN_PAGE:
        brw_instance = MMIBROWSER_GetInstance();
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_DOWNPAGE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 4"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4404_112_2_18_2_6_4_165,(uint8*)"");
            }
        }
        break;
    case MSG_CTL_PRGBOX_MOVE:
        brw_instance = MMIBROWSER_GetInstance();
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            uint32 top_index = *(uint32*)param;
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_MOVE_VERTICAL, top_index) )
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 4"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4415_112_2_18_2_6_4_166,(uint8*)"");
            }
        }
        break;

    case MSG_BRW_EXIT_QUERY://MS00191558:询问是否退出浏览器
 #if 0//useless
       MMIBROWSER_OpenQueryWinByTextId(TXT_EXIT_QUERY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
#endif       
        break;
      
    case MSG_TP_PRESS_MOVE:        
    case MSG_TP_PRESS_DOWN:        
    case MSG_TP_PRESS_UP:
        MMIBROWSER_HandleTpPress(win_id, msg_id, param, MMIBROWSER_GetInstance());
        break;
        
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_LONG:
    {
        GUI_POINT_T point = {0};

        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);

        if (BRW_IsTpPointImg(MMIBROWSER_GetInstance(), point.x, point.y))
        {
            BRW_MSG_ENTER_FILENM_REQ_T  req_ptr = {0};
            MMIFILE_DEVICE_E    file_type = MMI_DEVICE_NUM;
            
            BRW_GetTpPointInfo(MMIBROWSER_GetInstance(), req_ptr.res_name_arr, BRW_MAX_URL_LEN, 
                &req_ptr.mime_type, PNULL, &req_ptr.content_len,point.x, point.y);
                
            file_type = BrowserDlGetDeviceName(PNULL, PNULL, req_ptr.mime_type);//get the first valid device for mime type
            if (file_type < MMI_DEVICE_NUM)//nornal, begin to download
            {
                req_ptr.dl_type = BRW_DL_TYPE_LOCAL_CONTENT;
                req_ptr.handle = MMIBROWSER_SAVE_HANDLE_TP;
                req_ptr.enter_filenm_cbf = EnterFileNmForLocalCbf;
                MMIBROWSER_UpdateFileNameReq(&req_ptr);
                MMIBROWSER_OpenSaveMediaFileWin(req_ptr.mime_type, PNULL);
            }
            else
            {
                MMIPUB_OpenAlertWarningWin(TXT_SD_NO_EXIST);
            }
        }
    }
    break;
#endif    

#ifdef TOUCHPANEL_TYPE_MULTITP
    case MSG_TP_GESTURE:
        {//stop the slide state,reset the param
            BrowserStopTpSlideTimer(win_id);//stop slide timer
            MMK_ResetVelocityItem();//reset velocity item
            SCI_MEMSET(&s_browser_tp_info, 0x00, sizeof(s_browser_tp_info));
        }

        if (MMK_IsFocusWin(win_id))//NEWMS00123456 
        {
            MMI_GESTURE_TYPE_E gesture_type = MMK_GET_TP_GESTURE(param);
            if ((MMI_GESTURE_ZOOM_IN == gesture_type) || (MMI_GESTURE_ZOOM_OUT == gesture_type))
            {
                //SCI_TRACE_LOW:"[BRW]:MMIBROWSER_HandleZoom gesture_type =%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4479_112_2_18_2_6_4_167,(uint8*)"d", gesture_type);
                s_is_need_update_font_to_nv = MMIBROWSER_HandleZoom(gesture_type);
                if (s_is_need_update_font_to_nv)//means need to zoom
                {
                    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
                }
           }
        }
        break;
#endif

    case MSG_BRW_ENTER_SNAPSHOT_FILENAME://输入了新的snapshot's file name
        {
            wchar   *filename_ptr = PNULL;

            filename_ptr = (wchar*)param;
            brw_instance = MMIBROWSER_GetInstance();
            MMIBROWSER_OpenSnapshot(filename_ptr, brw_instance, FALSE);
        }
        break;  

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }
    return ret;
}

/*****************************************************************************/
//  Description : get default UA profile(first according to different network)
//      (临时方案,UA未认证! 根据场测结果匹配UA)
//  Global resource dependence : 
//  Author: Sally.He
//  Note:  modify by fen.xie--Add return vaule
/*****************************************************************************/
PUBLIC BOOLEAN MMIBROWSER_GetDefaultUserAgent(uint8** user_agent_pptr)
{
    MN_PLMN_T select_plmn = {0, 0, 2};
    uint32 i = 0;
    uint32 total_num = 0;
    BOOLEAN ua_is_set = FALSE;
    BOOLEAN result = FALSE;

    if (PNULL == user_agent_pptr)
    {
        return FALSE;
    }

    if (MMIAPIPHONE_IsPSAvail(MMIBROWSER_GetSim()))/*PS OK后再根据plmn status遍历到对应的UA*/
    {
        MMIAPIPHONE_GetSimHPLMN(&select_plmn, MMIBROWSER_GetSim());//MS00239059
        //SCI_TRACE_LOW:"[BRW]MMIBROWSER_GetDefaultUserAgent mcc:%d, mnc:%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4528_112_2_18_2_6_4_168,(uint8*)"dd", select_plmn.mcc, select_plmn.mnc);
        
        total_num = sizeof(s_mcc_mnc_ua_table) / sizeof(MCC_MNC_UA_T);
        for(i = 0; i < total_num; i++)
        {
            if((select_plmn.mcc == s_mcc_mnc_ua_table[i].mcc) &&
               (select_plmn.mnc == s_mcc_mnc_ua_table[i].mnc))
            {
                 *user_agent_pptr = (uint8*)s_mcc_mnc_ua_table[i].ua_ptr;
                 ua_is_set = TRUE;
                 break;
            }
        }
        result = TRUE;
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]MMIBROWSER_GetDefaultUserAgent PS is invalid"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4545_112_2_18_2_6_4_169,(uint8*)"");
    }
    
    if(!ua_is_set)
    {
        //*user_agent_pptr = (uint8*)MMIAPICOM_GetUserAgent();
        *user_agent_pptr = (uint8*)MMIBROWSER_DEFAULT_DORADO_UA;
    }
    
    return result;
}
/*****************************************************************************/
//  Description : set profile
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC BRW_PROFILE_T MMIBROWSER_SetProfile(void)
{
    BRW_PROFILE_T                       profile={0};
    MMIBROWSER_NV_SETTING_T*            nv_setting_ptr = PNULL;
#if 0    
    MMICONNECTION_LINKSETTING_DETAIL_T* setting_item_ptr = PNULL;
    MMISET_LANGUAGE_TYPE_E              language_type = 0;
    uint8   index = 0;
    char    *user_agent_ptr = PNULL;
    wchar   *device_name_ptr=PNULL;
    char    *ua_profile_ptr = PNULL;
    uint16  device_name_len = 0;
    
#ifdef WLAN_SUPPORT
    MMIWLAN_PROXY_SETTING_DETAIL_T detail_struct = {0};
#endif
#endif

    nv_setting_ptr = MMIBROWSER_GetNVSetting();
    if (PNULL == nv_setting_ptr)
    {
        //SCI_TRACE_LOW:"\n MMIBROWSER MMIBROWSER_SetProfile MMIBROWSER_GetNVSetting error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4581_112_2_18_2_6_4_170,(uint8*)"");
        return profile;
    }
#if 0    
    profile.is_bgsound_download=nv_setting_ptr->is_sound_played;
    MMIBROWSER_GetBrwSysDevName(&device_name_ptr, &device_name_len);
    if (PNULL == device_name_ptr)
    {
        profile.is_cache_enable = FALSE;
        profile.is_cookie_enable = FALSE;
    }
    else
    {
        profile.is_cache_enable=nv_setting_ptr->is_cache_enable;
        profile.is_cookie_enable=nv_setting_ptr->is_cookie_enable;
    }
    profile.is_image_download=nv_setting_ptr->is_image_loaded;
    profile.is_gwt_enable = nv_setting_ptr->is_google_change_enable;

    profile.dual_sys = MMIBROWSER_GetSim();
    
    profile.is_use_wifi = MMIBROWSER_GetIsUseWifi();
    if(profile.is_use_wifi)
    {
        profile.proxy_opt.is_use_proxy = FALSE;
        profile.protocol = BRW_PROTOCOL_HTTP;
#ifdef WLAN_SUPPORT
        MMIAPIWIFI_GetProxySettingDetail(&detail_struct);
        profile.proxy_opt.is_use_proxy = detail_struct.is_use_proxy;
        if(profile.proxy_opt.is_use_proxy)
        {
            profile.proxy_opt.proxy_addr_ptr = detail_struct.gateway;
            profile.proxy_opt.proxy_port = detail_struct.port;
            profile.psw_ptr = (char*)detail_struct.password;
            profile.username_ptr = (char*)detail_struct.username;
        }
#endif
    }
    else
    {
        index = MMIDORADO_GetNetSettingIndex(MMIBROWSER_GetSim());
        
        setting_item_ptr = MMIAPICONNECTION_GetLinkSettingItemByIndex(MMIBROWSER_GetSim(), index);
        if (PNULL == setting_item_ptr)
        {
            //SCI_TRACE_LOW:"\n MMIBROWSER MMIBROWSER_SetProfile MMIAPICONNECTION_GetLinkSettingItemByIndex error"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_4626_112_2_18_2_6_4_171,(uint8*)"");
            return profile;
        }
        profile.auth_type = setting_item_ptr->auth_type;
        profile.proxy_opt.proxy_addr_ptr = setting_item_ptr->gateway;
        profile.proxy_opt.proxy_port = setting_item_ptr->port;
        
        if (0 != strncmp(MMIBROWSER_INVALID_GATEWAY,(const char*)(setting_item_ptr->gateway),strlen(MMIBROWSER_INVALID_GATEWAY))&&
            0!=strlen((const char*)(setting_item_ptr->gateway)))
        {
            profile.proxy_opt.is_use_proxy = TRUE;
        }
        else
        {
            profile.proxy_opt.is_use_proxy = FALSE;
        }
        profile.psw_ptr = (char*)(setting_item_ptr->password);
        profile.username_ptr = (char*)(setting_item_ptr->username);
        profile.apn_ptr = (char*)(setting_item_ptr->apn);
        
        if (0 == setting_item_ptr->access_type)
        {
            profile.protocol = BRW_PROTOCOL_WAP;
        }
        else
        {
            profile.protocol = BRW_PROTOCOL_HTTP;
        }
    }
    
    MMIAPISET_GetLanguageType(&language_type);
    if (MMISET_LANGUAGE_ENGLISH == language_type)
    {
        profile.lang = BRW_LANG_EN;
    }
    else
    {
        profile.lang = BRW_LANG_CH;
    }
    
    profile.browser_pdp_priority = 1;
#endif
    profile.msg_proc_func = MMIBROWSER_HandleCtrlMsg;

    profile.scroll_bar_handler = MMIBROWSER_GetScrollHandle();
    if (s_brw_show_info.scroll.v_rect.right > s_brw_show_info.scroll.v_rect.left)
    {
        profile.scroll_bar_width = (s_brw_show_info.scroll.v_rect.right - s_brw_show_info.scroll.v_rect.left + 1);
    }
    
    //get lcd width and height
    MMIBROWSER_GetScreenProfile(&profile.screen_opt);
    profile.win_handle=MMIBROWSER_MAIN_WIN_ID;  

#if 0   
    ua_profile_ptr = MMIBROWSER_GetUaProfile();
    if (PNULL != ua_profile_ptr)
    {
        profile.ua_prof_ptr = (uint8*)ua_profile_ptr;
    }
    else
    {
        profile.ua_prof_ptr = (uint8*)MMIAPICOM_GetUserAgentProfile();//(uint8*)MMIBROWSER_DEFAULT_UAPROFILE;
    }
    
    user_agent_ptr = MMIBROWSER_GetUserAgent();
    if (PNULL != user_agent_ptr)
    {
        profile.user_agent_ptr = (uint8*)user_agent_ptr;
    }
    else
    {
#ifdef CUSTOM_DEFINE_UA//客户自定义UA
        profile.user_agent_ptr = (char *)MMIAPICOM_GetUserAgent();        
#else
        /*get UA in CONTROL,when start wap request. MMIBROWSER_GetDefaultUserAgent()
            Set different User_Agent according to different network. */
#endif
    }
    
#endif    
    switch (nv_setting_ptr->font)
    {
    case MMIBROWSER_FONT_MINI:
        profile.font_type = MMIBROWSER_SMALL_FONT;
        break;
    case MMIBROWSER_FONT_MIDDLE:
        profile.font_type = MMIBROWSER_NORMAL_FONT;
        break;
    case MMIBROWSER_FONT_BIG:
        profile.font_type = MMIBROWSER_BIG_FONT;
        break;
        
    default:
        profile.font_type = MMIBROWSER_NORMAL_FONT;
        break;
    }
    return profile;
}

/*****************************************************************************/
//  Description : set profile for BRW_SCREEN_PROFILE
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_GetScreenProfile(BRW_SCREEN_OPTION_T *screen_opt_ptr)
{
    uint16      lcd_height = 0;
    uint16      lcd_width = 0;

    if (PNULL == screen_opt_ptr)
    {
        return;
    }

    //get lcd width and height
    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID, &lcd_width, &lcd_height);

    screen_opt_ptr->width = lcd_width;
    screen_opt_ptr->left_top_x = 0;
    if (!MMITHEME_IsMainScreenLandscape())
    {
        screen_opt_ptr->height = s_brw_show_info.web_page.v_rect.bottom - s_brw_show_info.web_page.v_rect.top + 1;
        screen_opt_ptr->left_top_y = s_brw_show_info.web_page.v_rect.top;
   }
    else
    {
        screen_opt_ptr->height = s_brw_show_info.web_page.h_rect.bottom - s_brw_show_info.web_page.h_rect.top + 1;
        screen_opt_ptr->left_top_y = s_brw_show_info.web_page.h_rect.top;//MMI_STATUSBAR_HEIGHT+MMIBROWSER_ADDRESS_BAR_HIGHT;
   }

}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void AppendInputHistoryList(MMI_CTRL_ID_T ctrl_id, MMI_STRING_T *text_str)
{
    uint32 i = 0;
    GUILIST_ITEM_T item_t = {0};
    uint8 *url_ptr = PNULL;
    uint16 url_len = 0;
    uint16 match_num = 0;

    if (PNULL != text_str && PNULL != text_str->wstr_ptr && 0 != text_str->wstr_len)
    {
        url_len= text_str->wstr_len;
        url_ptr = SCI_ALLOCA((url_len + 1));

        if (PNULL != url_ptr)
        {
            SCI_MEMSET(url_ptr, 0x00, ((url_len + 1)));

            MMI_WSTRNTOSTR(url_ptr, url_len, text_str->wstr_ptr, url_len, url_len);
        }
    }

    match_num = BRW_GetMatchInputHisNum(url_ptr, url_len);

    GUILIST_RemoveAllItems(ctrl_id);
    
    if (0 != match_num)
    {
        GUIFORM_SetChildDisplay(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, ctrl_id, GUIFORM_CHILD_DISP_NORMAL);
        GUILIST_SetMaxItem(ctrl_id, match_num, TRUE);
        item_t.item_style = GUIITEM_STYLE_ONE_LINE_TEXT;
        for (i = 0; i < match_num; i++)
        {            
            GUILIST_AppendItem(ctrl_id, &item_t);
        }
    }
    else
    { 
        GUIFORM_SetChildDisplay(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, ctrl_id, GUIFORM_CHILD_DISP_HIDE);
    }

    if (PNULL != url_ptr)
    {
        SCI_FREE(url_ptr);
    }
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void AppendInputHistoryListData(MMI_HANDLE_T ctrl_handle, MMI_STRING_T *text_str, uint16 index)
{
    GUILIST_ITEM_DATA_T item_data = {0};
    uint8 *url_ptr = PNULL;
    uint16 url_len = 0;
    uint16 match_num = 0;

    if (PNULL != text_str && PNULL != text_str->wstr_ptr && 0 != text_str->wstr_len)
    {
        url_len= text_str->wstr_len;
        url_ptr = SCI_ALLOCA((url_len + 1));

        if (PNULL != url_ptr)
        {
            SCI_MEMSET(url_ptr, 0x00, ((url_len + 1)));

            MMI_WSTRNTOSTR(url_ptr, url_len, text_str->wstr_ptr, url_len, url_len);
        }
    }

    match_num = BRW_GetMatchInputHisNum(url_ptr, url_len);

    if (index < match_num)
    {
        uint8 * his_url_ptr = PNULL;
        uint16 his_url_len  = 0;
        wchar *url_wstr_ptr = PNULL;

        his_url_ptr = BRW_GetMatchInputHisUrlByIndex(url_ptr, url_len, index);

        if (PNULL != his_url_ptr)
        {
            his_url_len = strlen((char *)his_url_ptr);
        
            url_wstr_ptr = SCI_ALLOCA((his_url_len + 1)*sizeof(wchar));

            if (PNULL != url_wstr_ptr)
            {
                SCI_MEMSET(url_wstr_ptr, 0x00, ((his_url_len + 1)*sizeof(wchar)));

                MMI_STRNTOWSTR(url_wstr_ptr, his_url_len, his_url_ptr, his_url_len, his_url_len);
            }
        }
    
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[0].item_data.text_buffer.wstr_len = his_url_len;
        item_data.item_content[0].item_data.text_buffer.wstr_ptr = url_wstr_ptr;

        //set softkey
        item_data.softkey_id[0] = TXT_COMMON_OK;
        item_data.softkey_id[1] = TXT_NULL;
        item_data.softkey_id[2] = STXT_RETURN;
        
        GUILIST_SetItemData(ctrl_handle, &item_data, index);
        
        if (PNULL != url_wstr_ptr)
        {        
            SCI_FREE(url_wstr_ptr);
        }
    }

    if (PNULL != url_ptr)
    {
        SCI_FREE(url_ptr);
    }
}
/*****************************************************************************/
//  Description : the callback of link button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E LinkButtonCallBack(void)
{
    MMI_STRING_T text_str = {0};
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    uint8 url_arr[MMIBROWSER_MAX_URL_LEN+1] = {0};
    MMI_CTRL_ID_T   edit_ctrl_id=MMIBROWSER_URL_INPUT_CHILD_CTRL_ID;
    uint16          url_len=0;    

    GUIEDIT_GetString(edit_ctrl_id,&text_str);

    if ((text_str.wstr_ptr != PNULL) && (text_str.wstr_len != 0))
    {
        url_len = MIN(MMIBROWSER_MAX_URL_LEN, text_str.wstr_len);
    
        MMI_WSTRNTOSTR(url_arr, MMIBROWSER_MAX_URL_LEN, 
                text_str.wstr_ptr, text_str.wstr_len, url_len);

        BrwInputHisAddItem(url_arr, url_len);
    }
    
    if (Brw_IsIncludeInvalidUrl(url_arr))
    {
        //alert the url is invalid
        MMIBROWSER_AlertUrlInvalid();
    }
    else//enter new url
    {
        MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, url_arr, PNULL);
        MMK_CloseWin(MMIBROWSER_ADDRESS_WIN_ID);
    }

    return result;
}

/*****************************************************************************/
//  Description : set the list date to edit in edit url window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetListDataToEdit(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T   edit_ctrl_id=MMIBROWSER_URL_INPUT_CHILD_CTRL_ID;
    MMI_CTRL_ID_T   list_ctrl_id=MMIBROWSER_URL_INPUT_LIST_CHILD_CTRL_ID;
    uint16 cur_index = 0;
    const GUILIST_ITEM_T *list_item_ptr = PNULL;
    MMI_STRING_T text_str = {0};
    
    cur_index = GUILIST_GetCurItemIndex(list_ctrl_id);
    list_item_ptr = GUILIST_GetItemPtrByIndex(list_ctrl_id, cur_index);

    if (PNULL != list_item_ptr)
    {
        text_str.wstr_ptr = list_item_ptr->item_data_ptr->item_content[0].item_data.text_buffer.wstr_ptr;
        text_str.wstr_len = list_item_ptr->item_data_ptr->item_content[0].item_data.text_buffer.wstr_len;
    
        GUIEDIT_SetString(edit_ctrl_id,text_str.wstr_ptr, text_str.wstr_len);
        //update the button
#ifdef TOUCH_PANEL_SUPPORT
        if ((text_str.wstr_ptr != PNULL) && (text_str.wstr_len > 0))
        {
            GUIBUTTON_SetGrayed(MMIBROWSER_URL_INPUT_LINK_CHILD_CTRL_ID,FALSE,PNULL,FALSE);
        }
        else
        {
            GUIBUTTON_SetGrayed(MMIBROWSER_URL_INPUT_LINK_CHILD_CTRL_ID,TRUE,PNULL,FALSE);
        }
#endif
        AppendInputHistoryList(list_ctrl_id, &text_str);

        MMK_SetAtvCtrl(win_id, edit_ctrl_id);
    }
}
/*****************************************************************************/
//  Description : init the reference infomation of edit url window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwEditUrlWin(MMI_WIN_ID_T win_id)
{
    uint8   *url_ptr=PNULL;
    wchar           wchar_url_arr[MMIBROWSER_MAX_URL_LEN+1]={0};
    uint16          url_len=0;    
    MMI_CTRL_ID_T   edit_ctrl_id=MMIBROWSER_URL_INPUT_CHILD_CTRL_ID;
    MMI_CTRL_ID_T   list_ctrl_id=MMIBROWSER_URL_INPUT_LIST_CHILD_CTRL_ID;
#ifdef TOUCH_PANEL_SUPPORT
    MMI_CTRL_ID_T   link_ctrl_id=MMIBROWSER_URL_INPUT_LINK_CHILD_CTRL_ID;
    GUIFORM_CHILD_WIDTH_T child_width  = {0};
#endif
    GUI_BG_T bg = {0};
    GUIFORM_CHILD_HEIGHT_T child_height  = {0};
    MMI_STRING_T text_str = {0};
    wchar forbid_char_value[]= {0x0d, 0x0a};
    
    GUIFORM_IsSlide(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, FALSE);//MS00246907

#ifdef TOUCH_PANEL_SUPPORT
    GUIEDIT_PermitBorder(edit_ctrl_id, FALSE);
    child_width.type = GUIFORM_CHLID_WIDTH_FIXED;
    GUIRES_GetImgWidthHeight(&child_width.add_data,PNULL,IMAGE_BROWSER_LINK,win_id);
    GUIFORM_SetChildWidth(MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID, link_ctrl_id, &child_width);
#endif

    GUIFORM_SetType(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, GUIFORM_TYPE_TP);
    GUIFORM_SetMargin(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, 0);
    GUIFORM_PermitChildBg(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, FALSE);

    GUIEDIT_SetAlign(edit_ctrl_id, ALIGN_LVMIDDLE);

    GUILIST_PermitBorder(list_ctrl_id, FALSE);

    child_height.type = GUIFORM_CHLID_HEIGHT_FIXED;
    child_height.add_data = MMIBROWSER_URL_INPUT_FORM_CTRL_HEIGHT;
    GUIFORM_SetChildHeight(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID, &child_height);

    SCI_MEMSET(&bg, 0x00, sizeof(bg));
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_SEARCH_BAR_BG;
    GUIFORM_SetBg( MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID, &bg);

    child_height.type     = GUIFORM_CHLID_HEIGHT_PERCENT;
    child_height.add_data = 95;
    GUIFORM_SetChildHeight(MMIBROWSER_URL_INPUT_CHILD_FORM_CTRL_ID, edit_ctrl_id, &child_height);

    bg.bg_type = GUI_BG_IMG;
#ifdef MMI_PDA_SUPPORT
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
#else
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG_FOCUSED;
#endif /* MMI_PDA_SUPPORT */

    GUIEDIT_SetBg(edit_ctrl_id, &bg);

    child_height.type = GUIFORM_CHLID_HEIGHT_LEFT;
    GUIFORM_SetChildHeight(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, list_ctrl_id, &child_height);

    GUIEDIT_SetStyle(edit_ctrl_id, GUIEDIT_STYLE_SINGLE);
    //set im
    GUIEDIT_SetImTag(edit_ctrl_id, GUIIM_TAG_URL);
    GUIEDIT_SetIm(edit_ctrl_id,GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL, GUIIM_TYPE_ABC); /*lint !e655*/ //MS00220112
    GUIEDIT_SetSoftkey(edit_ctrl_id,0,1,TXT_NULL,TXT_COMMON_OK,PNULL);

    GUIEDIT_ConfigDisableChar(edit_ctrl_id, forbid_char_value, sizeof(forbid_char_value)/sizeof(wchar));

#ifdef MMI_PDA_SUPPORT
    GUILIST_PermitBorder(list_ctrl_id, FALSE);
#endif /* MMI_PDA_SUPPORT */

    {
        url_ptr=(uint8*)MMK_GetWinAddDataPtr(win_id);
        if (PNULL != url_ptr)
        {
            //url_len=(uint16)strlen((char*)url_ptr);
            CFLWCHAR_StrNCpyWithAsciiStr(wchar_url_arr,(const char*)url_ptr,MMIBROWSER_MAX_URL_LEN+1);
            url_len = (uint16)CFLWCHAR_StrLen(wchar_url_arr);
        }
        else
        {
            CFLWCHAR_StrNCpyWithAsciiStr(wchar_url_arr,MMIBROWSER_HTTP_STRING,MMIBROWSER_MAX_URL_LEN+1);
            url_len=(uint16)strlen(MMIBROWSER_HTTP_STRING);
        }
        GUIEDIT_SetString(edit_ctrl_id,wchar_url_arr,(uint16)url_len);

        text_str.wstr_ptr = wchar_url_arr;
        text_str.wstr_len = url_len;

        //set button
#ifdef TOUCH_PANEL_SUPPORT
        bg.bg_type = GUI_BG_IMG;
        bg.img_id = IMAGE_BROWSER_LINK_GRAY;
        if ((text_str.wstr_ptr != PNULL) && (text_str.wstr_len > 0))
        {
            GUIBUTTON_SetGrayed(link_ctrl_id,FALSE,&bg,FALSE);
        }
        else
        {
            GUIBUTTON_SetGrayed(link_ctrl_id,TRUE,&bg,FALSE);
        }
        GUIBUTTON_SetCallBackFunc(link_ctrl_id, LinkButtonCallBack);
#endif

        AppendInputHistoryList(list_ctrl_id, &text_str);
    }

}

/*****************************************************************************/
//  Description : handle edit url window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleEditUrlWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    ret=MMI_RESULT_TRUE;    
    MMI_CTRL_ID_T   edit_ctrl_id=MMIBROWSER_URL_INPUT_CHILD_CTRL_ID;
    MMI_CTRL_ID_T   list_ctrl_id=MMIBROWSER_URL_INPUT_LIST_CHILD_CTRL_ID;
    MMI_STRING_T text_str = {0};
    MMI_CTRL_ID_T acitve_ctrl_id = 0;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        InitBrwEditUrlWin(win_id);

#ifdef MMI_PDA_SUPPORT
        if (!MMITHEME_IsIstyle())
        {
            GUIFORM_SetChildDisplay(MMIBROWSER_URL_INPUT_FORM_CTRL_ID, MMICOMMON_SOFTKEY_CTRL_ID, GUIFORM_CHILD_DISP_HIDE);
        }
#endif
        MMK_SetAtvCtrl(win_id,edit_ctrl_id);
        break;

    case MSG_NOTIFY_EDITBOX_UPDATE_STRNUM:
        GUIEDIT_GetString(edit_ctrl_id, &text_str);
        if ((text_str.wstr_ptr != PNULL) && (text_str.wstr_len > 0))
        {
            GUIBUTTON_SetGrayed(MMIBROWSER_URL_INPUT_LINK_CHILD_CTRL_ID,FALSE,PNULL,FALSE);
        }
        else
        {
            GUIBUTTON_SetGrayed(MMIBROWSER_URL_INPUT_LINK_CHILD_CTRL_ID,TRUE,PNULL,FALSE);
        }
        
        AppendInputHistoryList(list_ctrl_id, &text_str);

        if (MMK_IsFocusWin(win_id))
        {
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
        break;

    case MSG_CTL_LIST_NEED_ITEM_DATA:
        {
            GUILIST_NEED_ITEM_DATA_T *need_item_data_ptr = (GUILIST_NEED_ITEM_DATA_T*)param;
            uint16  item_index = 0;
        
            if( PNULL == need_item_data_ptr )
            {
                //SCI_TRACE_LOW:"[BRW] BrwHandleEditUrlWinMsg PNULL == need_item_data_ptr"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5125_112_2_18_2_6_5_172,(uint8*)"");
                break;
            }
            
            GUIEDIT_GetString(edit_ctrl_id, &text_str);
            item_index = need_item_data_ptr->item_index;

            AppendInputHistoryListData(need_item_data_ptr->ctrl_handle, &text_str, item_index);
        }
        break;

    case MSG_NOTIFY_FORM_SWITCH_ACTIVE:
        {
            MMI_CTRL_ID_T ctrl_id = 0;
            GUI_BG_T bg = {0};

            ctrl_id = MMK_GetActiveCtrlId(win_id);

            bg.bg_type = GUI_BG_IMG;
            bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;

            if (ctrl_id == edit_ctrl_id)
            {
                bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG_FOCUSED;
                GUIEDIT_SetBg(edit_ctrl_id, &bg);

                GUILIST_SetCurItemIndex(list_ctrl_id, 0);
            }
            else
            {
                bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
                GUIEDIT_SetBg(edit_ctrl_id, &bg);
            }

            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
        break;

    case MSG_CTL_MIDSK:
    case MSG_APP_OK:
    case MSG_CTL_OK:
    case MSG_APP_WEB:
        acitve_ctrl_id = MMK_GetActiveCtrlId(win_id);

        if (list_ctrl_id == acitve_ctrl_id)//selected
        {
            BrwSetListDataToEdit(win_id);
        }
        else//other: link
        {
            LinkButtonCallBack();
        }
        break;

    case MSG_CTL_PENOK:
        {
            MMI_CTRL_ID_T src_id = ((MMI_NOTIFY_T*)param)->src_id;

            if (list_ctrl_id == src_id)
            {
                BrwSetListDataToEdit(win_id);
            }
#ifdef MMI_PDA_SUPPORT
            else if (MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID == src_id)
            {
                MMK_CloseWin(win_id);
            }
#endif
        }
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMK_CloseWin(win_id);
        break;

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}


/*****************************************************************************/
//  Description : the callback of bookmark button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BookmarkButtonCallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MMIBROWSER_OpenBookmarkWin();
    return result;
}

#ifdef BROWSER_TOOLBAR 
/*****************************************************************************/
//  Description : the callback of more button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E MoreButtonCallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MMK_CreateWin((uint32*)MMIBROWSER_MORE_POPMENU_WIN_TAB,PNULL);
    GUIMENU_CreatDynamic(PNULL,MMIBROWSER_MAIN_POPMENU_WIN_ID,MMIBROWSER_MENU_CTRL_ID,GUIMENU_STYLE_POPUP);
	return result;
}

/*****************************************************************************/
//  Description : the callback of stop button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E StopButtonCallBack(void)
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    uint8   *url_ptr = PNULL;
    
    MMIBROWSER_GetCurPageUrlInfo(&url_ptr, PNULL);
    if (!MMIBROWSER_GetLoading())
    {
        if (0 == BRW_AccessPage(MMIBROWSER_GetInstance(), url_ptr))
        {
            //SCI_TRACE_LOW:"StopButtonCallBack BRW_AccessPage error"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5221_112_2_18_2_6_5_173,(uint8*)"");
        }
        else
        {
            MMIBROWSER_SetToAccess((char *)url_ptr, TRUE);
        }

    }
    else
    {
        if (BRW_StopPageReq(MMIBROWSER_GetInstance()))
        {
            MMIBROWSER_SetLoading(FALSE);
            MMIBROWSER_SetParamToStop();
        }
        else
        {
            //SCI_TRACE_LOW:"StopButtonCallBack() BRW_StopPageReq error"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5238_112_2_18_2_6_5_174,(uint8*)"");
        }
    }

    MMIBROWSER_FreeCurPageUrlInfo(&url_ptr, PNULL);
    
    return result;
}

/*****************************************************************************/
//  Description : change the button image
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwChangeRefreshButtonImg(BOOLEAN is_loading, BOOLEAN is_need_update)
{
    uint32              i = 0;
    MMI_CTRL_ID_T       ctlr_id = MMIBROWSER_TOOLBAR_4_BUTTON_CTRL_ID;
    GUI_BG_T            fg = {0};
    GUI_BG_T            pressed_fg = {0};

    fg.bg_type = GUI_BG_IMG;
    pressed_fg.bg_type = GUI_BG_IMG;

    if (is_loading)
    {
        fg.img_id = IMAGE_BROWSER_STOP;
        pressed_fg.img_id = IMAGE_BROWSER_PRESSED_STOP;
    }
    else
    {
        fg.img_id = IMAGE_BROWSER_REFRESH;
        pressed_fg.img_id = IMAGE_BROWSER_PRESSED_REFRESH;
    }

    GUIBUTTON_SetFg(ctlr_id, &fg);

    GUIBUTTON_SetPressedFg(ctlr_id, &pressed_fg);

    if (is_need_update && MMIBROWSER_IsMainActive())
    {
        /*因提供的button图片是png的,而stop 和refresh在一个位置显示.故需要这种方式以避免重叠;
            若图片被修改为bmp,则只需刷新对于的button即可*/
        DisplayToolBarImg(MMIBROWSER_MAIN_WIN_ID, TRUE);

        for (i = 0;i < ARRAY_SIZE(s_button_ctrl_id); i++)
        {
            MMK_SendMsg(s_button_ctrl_id[i].button_ctrl_id, MSG_CTL_PAINT, PNULL);
        }
    }
}

/*****************************************************************************/
//  Description : the callback of forward button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E ForwardButtonCallBack(void)
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    
    if (BRW_NavHisIsForwardEnable())
    {
        if (!BRW_ProcNavEvent(MMIBROWSER_GetInstance(),BRW_NAV_EVENT_FORWARD))
        {
            result = MMI_RESULT_FALSE;
        }
        else
        {
            MMIBROWSER_SetLoading(TRUE);
            BrwChangeRefreshButtonImg(TRUE, TRUE);

            result = MMI_RESULT_TRUE;
        }
        
    }
	return result;
}

/*****************************************************************************/
//  Description : the callback of forward button
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HomeButtonCallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CloseWin(MMIBROWSER_MAIN_WIN_ID);

    return result;
}
#endif

/*****************************************************************************/
//  Description : the callback of back button
//  Global resource dependence : 
//  Author: li.li
//  Note: fenxie-MS00241839
/*****************************************************************************/
LOCAL MMI_RESULT_E BackButtonCallBack(void)
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    MMI_WIN_ID_T    query_exit_web_win_id = MMIBROWSER_QUERY_EXIT_WEB_WIN_ID;

    if (BRW_NavHisIsBackwardEnable())//backward
    {
        if (!BRW_ProcNavEvent(MMIBROWSER_GetInstance(),BRW_NAV_EVENT_BACKWARD))
        {
            result = MMI_RESULT_FALSE;
        }
        else
        {
            MMIBROWSER_SetLoading(TRUE);
#ifdef BROWSER_TOOLBAR
            BrwChangeRefreshButtonImg(TRUE, TRUE);
#endif
            result = MMI_RESULT_TRUE;
        }
    }
    else//退出browser instance
    {
        MMIBROWSER_OpenQueryWinByTextId(TXT_WWW_CLOSE_WEBPAGE/*TXT_EXIT_QUERY*/,
            IMAGE_PUBWIN_QUERY,
            &query_exit_web_win_id,
            HandleQueryExitWebWin);
    }

    return result;
}

/*****************************************************************************/
//  Description : handle control message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIBROWSER_HandleCtrlMsg(BRW_EXPORT_MSG_E message_id,void* param,uint16 size_of_param)
{
    //SCI_TRACE_LOW:"brw mmi get callback msg, id:%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5357_112_2_18_2_6_6_175,(uint8*)"d",message_id);
    return MMK_PostMsg(VIRTUAL_WIN_ID,message_id+MSG_BRW_START_ID,param,size_of_param);    
}

#if 0
/*****************************************************************************/
//  Description : Create CC select win
//  Global resource dependence : 
//  Author: rong.gu
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MMIBROWSER_OpenCCSelectWin(MMIBROWSER_CC_INFO_T *in_cc_info)
{
    MMIBROWSER_CC_INFO_T *cc_info = NULL;
    uint16 num_len = 0;
    BOOLEAN result = FALSE;

    do
    {
        if (PNULL == in_cc_info || PNULL == in_cc_info->phone_num_ptr)
        {
            //SCI_TRACE_LOW:"[BRW]MMIBROWSER_OpenCCSelectWin error1"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5376_112_2_18_2_6_6_176,(uint8*)"");
            break;
        }
        
        if (!MMIAPICOM_IsValidNumberString((char *)in_cc_info->phone_num_ptr, strlen((char *)in_cc_info->phone_num_ptr)))//MS00223055
        {
            //SCI_TRACE_LOW:"[BRW]MMIBROWSER_OpenCCSelectWin invalid number"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5382_112_2_18_2_6_6_177,(uint8*)"");
            break;
        }
        
        cc_info = SCI_ALLOCA(sizeof(MMIBROWSER_CC_INFO_T));
        if (PNULL == cc_info)
        {
            break;
        }
        SCI_MEMSET(cc_info, 0x00, sizeof(MMIBROWSER_CC_INFO_T));
        cc_info->cc_option = in_cc_info->cc_option;

        num_len = strlen((char *)in_cc_info->phone_num_ptr);
        cc_info->phone_num_ptr = SCI_ALLOCA(num_len + 1);
        if (PNULL == cc_info->phone_num_ptr)
        {
            SCI_FREE(cc_info);
            break;
        }

        SCI_MEMSET(cc_info->phone_num_ptr, 0x00, num_len + 1);
        SCI_MEMCPY(cc_info->phone_num_ptr, in_cc_info->phone_num_ptr, num_len);

        //SCI_TRACE_LOW:"BRW mmi cc enter selectwin  %s"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5405_112_2_18_2_6_6_178,(uint8*)"s",(char*)cc_info->phone_num_ptr);
        MMK_CreateWin((uint32*)MMIBROWSER_CC_SELECT_WIN_TAB, (ADD_DATA)cc_info);
        result = TRUE;
    }
    while(0);
    
    if (!result)//MS00223055
    {
        //提示无效号码
         MMIPUB_OpenAlertWarningWin(TXT_CC_INVALID_NUM);
    }
    
    return result;
}
#endif

/*****************************************************************************/
//  Description : handle control message:异步asynchronous
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDORADO_HandleBrwControlMsg(MMI_MESSAGE_ID_E msg_id,void* param)
{
    BOOLEAN ret = TRUE;
    BRW_EXPORT_MSG_E    brw_msg_id = (msg_id-MSG_BRW_START_ID);

    //SCI_TRACE_LOW:"[BRW]MMIDORADO_HandleBrwControlMsg:brw_message_id:%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5430_112_2_18_2_6_6_179,(uint8*)"d", brw_msg_id);

#ifdef MMIDORADO_BGSOUND_SUPPORT
    if (MSG_BRW_STOP_BGSOUND == msg_id)
    {
        //SCI_TRACE_LOW:"[BRW]MMIDORADO_HandleBrwControlMsg:MSG_BRW_STOP_BGSOUND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5435_112_2_18_2_6_6_180,(uint8*)"");
        Brw_StopAndFreeBGsound(BRW_INVALID_INSTANCE);
        return ret;
    }
#endif/*MMIDORADO_BGSOUND_SUPPORT*/

    switch(brw_msg_id)
    {
    case BRW_MSG_INIT_CNF://do nothing
        break;
        
    case BRW_MSG_TYPE_DISPLAY_READY_IND:
        ret = HandleControlDisplayReadyInd(param);
        break;
    
    case BRW_MSG_TYPE_REPAINT_READY_IND:
        if (MMIBROWSER_IsMainActive())
        {
            if (!BRW_RepaintPage(MMIBROWSER_GetInstance()))
            {
                //SCI_TRACE_LOW:"[BRW]W MMIDORADO_HandleBrwControlMsg BRW_RepaintPage ERROR"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5455_112_2_18_2_6_6_181,(uint8*)"");
            }

        }
        break;
        
    case BRW_MSG_TYPE_AUTH_REQ:
        ret = HandleControlAuthReq(param);
        break;
        
    case BRW_MSG_TYPE_CHANGE_URL_IND:
        ret = HandleControlChangeUrlInd(param);
        break;

    case BRW_MSG_TYPE_NEW_PAGE_REQ_IND:
        ret = HandleControlNewPageInd(param);
        break;

    case BRW_MSG_TYPE_ERROR_IND:
        ret = HandleControlErrorInd(param);
        break;

    case BRW_MSG_TYPE_ENTER_FILENM_REQ:
        ret = HandleControlEnterFileNMInd(param);
        break;
        
#ifdef SSL_SUPPORT
    case BRW_MSG_TYPE_SSL_CERT_IND:
        ret = HandleControlSslCertInd(param);        
        break;
#endif

    case BRW_MSG_TYPE_PROGRESS_IND:
        ret = HandleControlProgressInd(param);
        break;

    case BRW_MSG_TYPE_DOWNLOAD_OK_IND:
        ret =HandleControlDownloadOkInd(param);
        break;
        
    case BRW_MSG_TYPE_DOWNLOAD_ERR_IND:
        ret = HandleControlDownloadErrInd(param);
        break;
        
#if 0        
    case BRW_MSG_TYPE_MAKECALL_REQ:
        ret = HandleControlMCReq(param);
        break;
        
    case BRW_MSG_TYPE_SENDDTMF_REQ:
        ret = HandleControlSendDTMFReq(param);
        break;
        
    case BRW_MSG_TYPE_ADD_PBITEM_REQ:
        ret = HandleControlAddPBItemReq(param);
        break;
#endif

    case BRW_MSG_TYPE_PAGE_GET_IND:
        ret = HandleControlPageGetInd(param);
        break;

    case BRW_MSG_TYPE_MEDIA_GET_ONE_IND:
        ret = HandleControlMediaGetOneInd(param);
        break;

    case BRW_MSG_TYPE_PAGE_STOP_IND:
        ret = HandleControlPageStopInd(param);
        break;

#ifdef OMADL_SUPPORT
    case BRW_MSG_TYPE_OMA_DL_CONFIRM_REQ:
        ret = HandleControlOMADLConfirmReq(param);
        break;
#endif

#if 0        
    case BRW_MSG_TYPE_OPEN_RTSP_IND:
#ifdef MMI_STREAMING_PLAYER_SUPPORT
    {
        uint8 *url_ptr = (uint8*)param;
        MMIAPIVP_PlayVideoFromVideoPlayer(VP_ENTRY_FROM_NET,PNULL, 0, url_ptr);        
    }
#endif    
        break;
    case BRW_MSG_TYPE_SENDSMS_REQ:
	case BRW_MSG_TYPE_SENDSMSTO_REQ:
        ret = HandleControlSendSmsReq(param);
        break;
	case BRW_MSG_TYPE_SENDMMS_REQ:
	case BRW_MSG_TYPE_SENDMMSTO_REQ:
        ret = HandleControlSendMmsReq(param);
        break;
	case BRW_MSG_TYPE_SENDMAILTO_REQ:
        ret = HandleControlSendMailToReq(param);
        break;
#endif

#ifdef JS_SUPPORT
    case BRW_MSG_TYPE_LAYOUTCTRL_DESTROY_REQ:
        BrwDestroyLayoutCtrl(param);
        break;
#endif
        
    default:
        break;
    }
    
    return ret;
}

#ifdef SSL_SUPPORT
/*****************************************************************************/
// 	Description : ssl cert query window message handle
//	Global resource dependence : 
//  Author:
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwSSLCertQueryCallback(
                                        MMI_WIN_ID_T win_id, 	
                                        MMI_MESSAGE_ID_E msg_id, 
                                        DPARAM param
                                        )
{
    MMI_RESULT_E            result = MMI_RESULT_TRUE;
    BRW_MSG_SSL_CERT_IND_T* ssl_cert_ptr = PNULL;

    switch (msg_id)
    {
    case MSG_APP_OK:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_WEB:
        ssl_cert_ptr = MMIBROWSER_GetSSLCert();
        ssl_cert_ptr->cert_cbf(ssl_cert_ptr->handle,TRUE);
        MMK_CloseWin(win_id);
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        ssl_cert_ptr = MMIBROWSER_GetSSLCert();
        ssl_cert_ptr->cert_cbf(ssl_cert_ptr->handle,FALSE);
        MMK_CloseWin(win_id);
        break;
        
    default:
        result = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param );
        break;
    }
    
    return result;
}
#endif
/*****************************************************************************/
//  Description : show error in the window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwShowErrMsg(BRW_ERROR_E error)
{
    MMI_TEXT_ID_T text_id=TXT_NULL;
    MMI_WIN_ID_T   win_id = MMIBROWSER_ERR_ALERT_WIN_ID;	

    //SCI_TRACE_LOW:"[BRW]BrwShowErrMsg error=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5616_112_2_18_2_6_6_182,(uint8*)"d", error);
    
    if(!MMK_IsOpenWin(MMIBROWSER_MAIN_WIN_ID))
    {
        return;
    }
    switch(error)
    {
    case BRW_ERR_CONNECT_FAILED:
        text_id=TXT_COMM_SERVER_ERROR;
        break;
    case BRW_ERR_CONNECT_TIMEOUT:
        text_id=TXT_COMM_TIMEOUT;
        break;
    case BRW_ERR_GPRS_LINK_FAILED:
        if (!MMIBROWSER_GetIsUseWifi())//MS00219813:wifi激活失败,不再提示"拨号失败"
        {
            text_id=TXT_DIAL_FAILED;
        }
        break;
    case BRW_ERR_SERVER_RESET:
        text_id=TXT_COMM_SERVER_ERROR;
        break;
    case BRW_ERR_NO_MEM:
        /*MS00210645:when no memory, clean up ram but still alert "no momery";
            因为内存不足的原因很多,不仅是clean up ram就可以的*/
        BRW_RamCleanUp();
        text_id = TXT_COMMON_NO_MEMORY;
#if 0
        if(BRW_RamCleanUp())
	{
		text_id= TXT_BROWSER_RETRY;//显示如“内部错误请重试”
	}
	else
	{
		text_id=TXT_COMMON_NO_MEMORY;
	}
#endif	
        break;

    case BRW_ERR_GPRS_LINK_DISCONNECT:
        text_id=TXT_BROWSER_GPRS_DISCONNECT;
        break;
    case BRW_ERR_HTTP_RTN:
        text_id = TXT_BROWSER_HTTP_ERROR;
        break;
    case BRW_ERR_PAGE_ERR:
        text_id = TXT_BROWSER_PAGE_ERROR;
        break;
    case BRW_ERR_PAGE_TOO_BIG:
        text_id=TXT_BROWSER_FILE_TOO_LARGE;
        break;
    case BRW_ERR_PAGE_UNSUPPORT:
        text_id=TXT_COMM_TYPE_NOT_SUPPORT;
        break;
    case BRW_ERR_GWT_URL_FAIL:
        text_id = TXT_BROWSER_GOOGLE_CHANGE_ERROR;
        break;
    case BRW_ERR_URL_INVALID:
        text_id = TXT_COMMON_URL_INVALID;
        break;
    case BRW_ERR_GPRS_LINK_FDN_ONLY:
        text_id=TXT_CC_FDN_ONLY;
        break;
              
    case BRW_ERR_UNKNOWN:
        text_id = TXT_BROWSER_HTTP_ERROR;//Unknown error(文本相同,共用ID)
        break;
    //////////////////////////////////////////////////////////////////////////
    // added by feng.xiao
#ifdef DATA_ROAMING_SUPPORT
    case BRW_ERR_DATA_ROAMING_DISABLED:
        text_id = TXT_DATA_ROAMING_DISABLED;
        break;
#endif
    default:
        break;
    }
    if(TXT_NULL != text_id)
    {
         MMIPUB_OpenAlertWinByTextId(PNULL,
            text_id,
            TXT_NULL,
            IMAGE_PUBWIN_WARNING,
            &win_id,
            PNULL,
            MMIPUB_SOFTKEY_ONE,
            PNULL
            );
    }
    //MMIBROWSER_SetLoading(FALSE);
}
/*****************************************************************************/
//  Description : check if touch the address bar
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwIsTouchAddressBar(GUI_POINT_T point)
{

    if (!MMITHEME_IsMainScreenLandscape())
    {
        return GUI_PointIsInRect(point, s_brw_show_info.addr_label.v_rect);
    }
    else
    {
        return GUI_PointIsInRect(point, s_brw_show_info.addr_label.h_rect);
    }
}

/*****************************************************************************/
//  Description : check if touch the web page
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwIsTouchInWebPage(GUI_POINT_T point)
{
    if (!MMITHEME_IsMainScreenLandscape())
    {
        return GUI_PointIsInRect(point, s_brw_show_info.web_page.v_rect);
    }
    else
    {
         return GUI_PointIsInRect(point, s_brw_show_info.web_page.h_rect);
    }
}

/*****************************************************************************/
//  Description : entry the input url window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_InputUrl(uint8* original_url_ptr)
{
    MMK_CreateWin((uint32*)MMIBROWSER_INPUT_URL_WIN_TAB,(ADD_DATA)original_url_ptr);
}

/*****************************************************************************/
//  Description : entry the edit file name window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
//PUBLIC void MMIBROWSER_InputFileName(void)
//{
//    MMK_CreateWin((uint32*)MMIBROWSER_ENTER_FILE_NAME_WIN_TAB,PNULL);
//}
/*****************************************************************************/
//  Description : handle auth window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleAuthWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    result=MMI_RESULT_TRUE;
    uint16  index = 0;
    BRW_MSG_AUTH_REQ_T  *param_ptr=PNULL;
    char    user_name[MMIBROWSER_MAX_USER_NAME_LEN + 1] = {0};
    MMI_CTRL_ID_T   ctrl_id = MMIBROWSER_RICHTEXT_AUTH_CTRL_ID;

    switch(msg_id) 
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id,ctrl_id);
        GUIRICHTEXT_SetFocusMode(ctrl_id,GUIRICHTEXT_FOCUS_SHIFT_ADVANCED);
        BrwSetAuthWindow(ctrl_id);
        GUIRICHTEXT_SetFocusItem(ctrl_id,2);
        break;
        
    case MSG_CTL_OK:
    case MSG_APP_OK:
        //获得聚焦的item索引
        param_ptr = MMIBROWSER_GetAuthReq();
        if (MMIBROWSER_GetUserNameSetting())
        {
            if ((PNULL != param_ptr )&& (PNULL != param_ptr->auth_cbf))
            {
                MMIAPICOM_WstrToStr(MMIBROWSER_GetUserName(), (uint8*)user_name);
                param_ptr->auth_cbf(param_ptr->handle,(uint8*)user_name,(uint8*)(MMIBROWSER_GetPassword()),TRUE);
            }
            MMIBROWSER_FreePassword();
            MMIBROWSER_FreeUserName();
            MMIBROWSER_FreeAuthReq();
            MMK_CloseWin(win_id);
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_INPUT_NAME_ALERT);
        }
        
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
#ifdef MMI_PDA_SUPPORT
        {
            MMI_CTRL_ID_T       id = 0;

            if (PNULL != param)
            {
		        id = ((MMI_NOTIFY_T*) param)->src_id;

                if (id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                    break;
                }
                else if (id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                    break;
                }
            }    
        }
#endif /* MMI_PDA_SUPPORT */
        GUIRICHTEXT_GetFocusItemIndex( ctrl_id, &index );
        
        if (2==index)
        {
            MMK_CreatePubEditWin((uint32*)MMIBROWSER_INPUT_USERNAME_WIN_TAB,PNULL);
        }
        else if (4==index)
        {
            MMK_CreatePubEditWin((uint32*)MMIBROWSER_INPUT_PWD_WIN_TAB,PNULL);
        }
        else
        {
            
        }

        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        //need to stop download
        param_ptr = MMIBROWSER_GetAuthReq();
        if ((PNULL != param_ptr )&& (PNULL != param_ptr->auth_cbf))
        {
            param_ptr->auth_cbf(param_ptr->handle, PNULL, PNULL, FALSE);
        }
        else
        {
            BRW_StopPageReq(MMIBROWSER_GetInstance());
        }
        MMIBROWSER_SetLoading(FALSE);
        MMIBROWSER_SetParamToStop();
        MMIBROWSER_SetToStopDL();

        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        MMIBROWSER_FreePassword();
        MMIBROWSER_FreeUserName();
        MMIBROWSER_FreeAuthReq();
        break;
        
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : set the auth window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetAuthWindow(MMI_CTRL_ID_T ctrl_id)
{
    GUIRICHTEXT_ITEM_T  item_data = {0};
    uint16              index = 0;

    //title
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_COMMON_INPUT_USERNAME_PASSWORD;    
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_BROWSER_USERNAME;    
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    item_data.text_frame_type = GUIRICHTEXT_FRAME_NORMAL;
    item_data.tag_type = GUIRICHTEXT_TAG_NORMAL;
    item_data.text_data.buf.len = 1;
    item_data.text_data.buf.str_ptr = L" ";
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_COMN_PASSWORD_INPUT;    
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    item_data.text_frame_type = GUIRICHTEXT_FRAME_NORMAL;
    item_data.tag_type = GUIRICHTEXT_TAG_NORMAL;
    item_data.text_data.buf.len = 1;
    item_data.text_data.buf.str_ptr = L" ";
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
}

/*****************************************************************************/
//  Description : update the display url in address bar
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_UpdateAddrLable(wchar* url_ptr, BOOLEAN is_update)
{
    if (PNULL == url_ptr)
    {
        //SCI_TRACE_LOW:"BRW:MMIBROWSER_UpdateAddrLable url_ptr is PNULL!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_5928_112_2_18_2_6_7_183,(uint8*)"");
        return;
    }
    
    if(MMK_IsOpenWin(MMIBROWSER_MAIN_WIN_ID))
    {
        MMI_CTRL_ID_T   address_label_ctrl_id=MMIBROWSER_LABEL_ADDRESS_CTRL_ID;
        MMI_STRING_T    text_str={0};
        uint16          url_len=(uint16)MMIAPICOM_Wstrlen(url_ptr);
        
        //if(url_len>0)
        {
            text_str.wstr_ptr=url_ptr;
            text_str.wstr_len=url_len;
            if (is_update && MMIBROWSER_IsMainActive())
            {
                GUILABEL_SetText(address_label_ctrl_id,&text_str,TRUE);
            }
            else
            {
                GUILABEL_SetText(address_label_ctrl_id,&text_str,FALSE);
            }
        }
        
    }
}

/*****************************************************************************/
//  Description : handle file detail window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFileDetailWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    switch(msg_id) 
    {
        case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id,MMIBROWSER_FILE_DETAIL_CTRL_ID);
        BrwSetFileDetailWindow(MMIBROWSER_FILE_DETAIL_CTRL_ID);
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : set the detail window
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetFileDetailWindow(MMI_CTRL_ID_T ctrl_id)
{
    GUIRICHTEXT_ITEM_T  item_data = {0};
    uint16              index = 0;
    wchar *wurl_ptr = PNULL;
    uint16              file_title[GUIRICHTEXT_TEXT_MAX_LEN + 1] = {0};
    uint16              tile_len = 0;

    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_COMM_TITLE;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    if (BRW_GetPageTitle(MMIBROWSER_GetInstance(),file_title,&tile_len,GUIRICHTEXT_TEXT_MAX_LEN))
    {
        item_data.text_type = GUIRICHTEXT_TEXT_BUF;
        item_data.text_data.buf.len = tile_len;
        item_data.text_data.buf.str_ptr = file_title;
    }
    else
    {
        item_data.text_type = GUIRICHTEXT_TEXT_RES;
        item_data.text_data.res.id = TXT_NO_TITLE;
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_INPUT_URL_ADRESS;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    MMIBROWSER_GetCurPageUrlInfo(PNULL, &wurl_ptr);
    item_data.text_data.buf.len = MMIAPICOM_Wstrlen(wurl_ptr);
    item_data.text_data.buf.str_ptr = wurl_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    MMIBROWSER_FreeCurPageUrlInfo(PNULL, &wurl_ptr);
}


#if defined(WIFI_SUPPORT) || !defined(MMI_MULTI_SIM_SYS_SINGLE) 
/*****************************************************************************/
//  Description : append item to listbox 
//  Global resource dependence : none
//  Author: fen.xie
//  Note:
/*****************************************************************************/
//Sam.hua@Dec 20	This api is no use now
/*
LOCAL BOOLEAN BrwAppendSelectListItem(MMI_CTRL_ID_T ctrl_id)
{
#if defined(WIFI_SUPPORT)
    MMI_STRING_T sim_str = {0};
    GUILIST_ITEM_T      item_t = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
#endif    

#ifdef WIFI_SUPPORT
    GUILIST_SetMaxItem(ctrl_id, MMI_DUAL_SYS_MAX + 1, FALSE );
#else
    GUILIST_SetMaxItem(ctrl_id, MMI_DUAL_SYS_MAX, FALSE );
#endif

#ifndef MMI_MULTI_SIM_SYS_SINGLE 
    MMIAPISET_AppendMultiSimName(ctrl_id, TXT_COMMON_OK, MMI_DUAL_SYS_MAX, MMISET_APPEND_SYS_OK);
#else//only defined WIFI_SUPPORT
    item_t.item_style = GUIITEM_STYLE_ONE_LINE_RADIO;
    item_t.item_data_ptr = &item_data;
    item_t.user_data = MN_DUAL_SYS_1;

    MMI_GetLabelTextByLang(TXT_GPRS,&sim_str);
    
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[0].item_data.text_buffer.wstr_len = sim_str.wstr_len;
    
    item_data.item_content[0].item_data.text_buffer.wstr_ptr = sim_str.wstr_ptr;

    //set softkey
    item_data.softkey_id[0] = TXT_COMMON_OK;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = STXT_RETURN;
    
    GUILIST_AppendItem(ctrl_id, &item_t);
#endif

#ifdef WIFI_SUPPORT
    SCI_MEMSET(&item_t, 0x00, sizeof(item_t));
    SCI_MEMSET(&item_data, 0x00, sizeof(item_data));

    item_t.item_style = GUIITEM_STYLE_ONE_LINE_ICON_TEXT;
    item_t.item_data_ptr = &item_data;
    item_t.user_data = MMI_DUAL_SYS_MAX;//WIFI
    #ifdef WLAN_SUPPORT
        MMI_GetLabelTextByLang(TXT_WIFI,&sim_str);
    #else
        MMI_GetLabelTextByLang(TXT_WIFI,&sim_str);
    #endif
    item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
    item_data.item_content[0].item_data.image_id = MAIN_GetIdleWifiIcon();
                                 

    item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[1].item_data.text_buffer.wstr_len = sim_str.wstr_len;
    item_data.item_content[1].item_data.text_buffer.wstr_ptr = sim_str.wstr_ptr;

    //set softkey
    item_data.softkey_id[0] = TXT_COMMON_OK;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = STXT_RETURN;
    
    GUILIST_AppendItem(ctrl_id, &item_t);
#endif

    GUILIST_SetOwnSofterKey(ctrl_id, TRUE);

    return TRUE;
}
*/
/*****************************************************************************/
//  Description : handle sim select window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwSIMSelectCallback(uint32 dual_sys, BOOLEAN isReturnOK, DPARAM param)
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;

    if (isReturnOK)
    {
        if( (MN_DUAL_SYS_E)dual_sys < MMI_DUAL_SYS_MAX )
        {
            MMIBROWSER_SetIsUseWifi(FALSE);
            MMIBROWSER_SetSim((MN_DUAL_SYS_E)dual_sys);
        }    
        else//WIFI
        {
#ifdef WIFI_SUPPORT    
        if (MMI_DUAL_SYS_MAX == dual_sys)
        {
            MMIBROWSER_SetIsUseWifi(TRUE);
        }    
#else
        //SCI_TRACE_LOW:"[BRW]BrwSIMSelectCallback sys_state[%d] = MMI_DUAL_SYS_MAX"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6127_112_2_18_2_6_7_184,(uint8*)"d", dual_sys);
#endif
        }

#ifdef MRAPP_SUPPORT
		if(MMIMRAPP_GetVMState() == VM_BACK_RUN)
		{
			if(MMIBROWSER_GetSim() != MMIMRAPP_GetActiveSim())
			{
				if(MMIMRAPP_HandleEventFromOtherModule(MMI_MODULE_MMIBROWSER))
				{
					MMIBROWSER_ResetStartInfo();
				}
			}
		}
#endif

        MMIBROWSER_Enter();
    }
    else
    {
        //select fail, need to destory something when MMIAPIBROWSER_Entry()
        //if (!MMIDORADO_IsRunning())
        {
            MMIBROWSER_ResetStartInfo();
        }
    }

    return result;
}
#endif

#if 0
/*****************************************************************************/
//  Description : handle input file name window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL wchar* BrwGetFileNameExceptExt(wchar* file_name_ptr)
{
    wchar* wch_ptr = PNULL;
    if (PNULL != file_name_ptr)
    {
        wch_ptr = file_name_ptr;
        while (*wch_ptr != 0)
        {
            if (*wch_ptr == '.')
            {
                break;
            }
            wch_ptr ++;
        }
    }
    return wch_ptr;
}
#endif

/*****************************************************************************/
//  Description : handle input file name window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleInputFileNameWinMsg(
                                          MMI_WIN_ID_T          win_id,
                                          MMI_MESSAGE_ID_E      msg_id,
                                          DPARAM                 param
                                          )
{
    BRW_MSG_ENTER_FILENM_REQ_T* file_name_req_ptr = PNULL;
    MMI_CTRL_ID_T               ctrl_id=MMIBROWSER_INPUT_FILE_NAME_CTRL_ID;
    MMI_RESULT_E	            ret = MMI_RESULT_TRUE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        file_name_req_ptr = MMIBROWSER_GetFileNameReq();
        if (PNULL != file_name_req_ptr && 0 != MMIAPICOM_Wstrlen(file_name_req_ptr->res_name_arr))
        {
            wchar  *file_name_ptr = PNULL;          
            uint16  file_name_len = 0;      
            wchar  suffix_name[MMIBROWSER_DL_SUFFIX_MAX_LEN + 1] = {0};          
            uint16  suffix_name_len = MMIBROWSER_DL_SUFFIX_MAX_LEN;   

            uint32  res_name_len = MMIAPICOM_Wstrlen(file_name_req_ptr->res_name_arr);
            
            file_name_ptr = SCI_ALLOC_APP(sizeof(wchar)*res_name_len);//if retrun PNULL,then assert!
            SCI_MEMSET(file_name_ptr, 0x00, (sizeof(wchar)*res_name_len));
            
            MMIAPIFMM_SplitFileName(file_name_req_ptr->res_name_arr, res_name_len, 
                file_name_ptr, &file_name_len, 
                suffix_name, &suffix_name_len);
            GUIEDIT_SetString(ctrl_id, file_name_ptr, file_name_len);

            SCI_FREE(file_name_ptr);
        }
        
        //set left softkey
        GUIEDIT_SetSoftkey(ctrl_id,0,1,TXT_NULL,TXT_COMMON_OK,PNULL);//MS00246150

        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;

    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_SK_SKB_ICON_PRESSED:
#ifdef MMI_PDA_SUPPORT
        {
            MMI_CTRL_ID_T   id = 0;

            if (PNULL != param)
            {
		        id = ((MMI_NOTIFY_T*) param)->src_id;

                if (id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                    break;
                }
                else if (id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                    break;
                }
            }
        }
#endif /* MMI_PDA_SUPPORT */
    {
        //GUIRICHTEXT_ITEM_T          item_data = {0};
        MMI_STRING_T 	            text_str = {0};
        wchar  suffix_name[MMIBROWSER_DL_SUFFIX_MAX_LEN + 1] = {0};          
        uint16  suffix_name_len = MMIBROWSER_DL_SUFFIX_MAX_LEN;      
        wchar   dot_wstr = MMIFILE_DOT;
        wchar   res_name_arr[BRW_MAX_URL_LEN+1] = {0}; 
        wchar full_name[BRW_MAX_URL_LEN + 1] = {0};  
        
        file_name_req_ptr = MMIBROWSER_GetFileNameReq();
        GUIEDIT_GetString(ctrl_id,&text_str);
        
        if (0 == text_str.wstr_len || PNULL == text_str.wstr_ptr || !MMIAPICOM_FileNameIsValid(&text_str))//MS00192708
        {
            MMIPUB_OpenAlertWarningWin(TXT_DC_NAMEINVALID);
            break;
        }

        //cr150684 add ext name when display
        MMIAPIFMM_SplitFileName(file_name_req_ptr->res_name_arr, MMIAPICOM_Wstrlen(file_name_req_ptr->res_name_arr), 
            PNULL, PNULL,
            suffix_name, &suffix_name_len
            );

        if (suffix_name_len > 0)
        {
            //cat the new file name--确保文件的后缀名不被截取(若名字过长,可以被截取)
            MMIAPICOM_Wstrncpy(res_name_arr, text_str.wstr_ptr, MIN((BRW_MAX_URL_LEN - suffix_name_len -1), text_str.wstr_len));
            //cat the suffix
            MMIAPICOM_Wstrncat(res_name_arr, &dot_wstr, 1);
            MMIAPICOM_Wstrncat(res_name_arr, suffix_name, suffix_name_len);
            
            /*reset text str with suffix*/
            MMIAPICOM_Wstrncpy(full_name, text_str.wstr_ptr, MIN((BRW_MAX_URL_LEN - suffix_name_len -1), text_str.wstr_len));
            MMIAPICOM_Wstrncat(full_name, &dot_wstr, 1);
            MMIAPICOM_Wstrncat(full_name, suffix_name, suffix_name_len);
            text_str.wstr_ptr = full_name;
            text_str.wstr_len = MMIAPICOM_Wstrlen(full_name);
        }
        else
        {
            MMIAPICOM_Wstrncpy(res_name_arr, text_str.wstr_ptr, MIN(BRW_MAX_URL_LEN, text_str.wstr_len));
            //SCI_TRACE_LOW:"BrwHandleInputFileNameWinMsg suffix name len ==0, ERROR"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6301_112_2_18_2_6_7_185,(uint8*)"");
        }

        MMIBROWSER_UpdateEnterFileName(res_name_arr);
        GUILABEL_SetText(MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID, &text_str, FALSE);
        MMK_CloseWin(win_id);
    }
        break;
        
    default:
        ret = MMI_RESULT_FALSE;
        break;
   
    }
    return ret;
}
#ifdef SNS_SUPPORT
/*****************************************************************************/
//  Description : handle wheterh install app
//  Global resource dependence : 
//  Author: renwei
//  Note: 
/*****************************************************************************/
PUBLIC MMI_RESULT_E  HandleBROWSERWhetherInstallWinMsg(
                                             MMI_WIN_ID_T		win_id, 	
                                             MMI_MESSAGE_ID_E	msg_id,
                                             DPARAM				param
                                             )
{
    int            widget_id       = 0;
    MMI_RESULT_E   recode          = MMI_RESULT_TRUE;
    MMIPUB_INFO_T *pubwin_info_ptr = PNULL;
    
    pubwin_info_ptr = MMIPUB_GetWinAddDataPtr(win_id);

    if(PNULL == pubwin_info_ptr)
    {
        return MMI_RESULT_FALSE;
    }
    
    widget_id = pubwin_info_ptr->user_data;
    
    switch(msg_id)
    {
        case MSG_APP_WEB:
        case MSG_APP_OK:
        {
            IMMISNS_CreateObject(&pMe);

            if(pMe)
            {
                IMMISNS_AppInstall(pMe, widget_id);
                
                IMMISNS_Release(pMe);
                pMe         = PNULL;
            }
            break;
        }
        case MSG_APP_CANCEL:
            break;
        default:
            recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
            break;
    }
    
    return recode;
}
#endif
/*****************************************************************************/
//  Description : get the dafualt file name in download(if null by random, and combined the suffix by mime_type)
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwDlGetDefaultFileName(wchar *dest_file_name_ptr, //[IN/OUT]
        uint16 *dest_file_name_len_ptr,//[IN/OUT]
        wchar *res_name_ptr,
        BRW_MIME_TYPE_E mime_type
        )
{
    BOOLEAN is_exit_ext_name=FALSE;
    wchar   suffix_arr[MMIBROWSER_DL_SUFFIX_MAX_LEN + 1] = {0};
    uint16  suffix_arr_len = MMIBROWSER_DL_SUFFIX_MAX_LEN;
    uint16  res_name_len = 0;

    if ((PNULL == dest_file_name_ptr) || (PNULL == dest_file_name_len_ptr))
    {
        return  FALSE;
    }
    
    if (PNULL != res_name_ptr)
    {
        res_name_len= MMIAPICOM_Wstrlen(res_name_ptr);
        MMIAPIFMM_SplitFileName(res_name_ptr, res_name_len, PNULL, PNULL, suffix_arr, &suffix_arr_len);
        is_exit_ext_name = (suffix_arr_len > 0)? TRUE: FALSE;
    }

    //SCI_TRACE_LOW:"[BRW]BrwDlGetDefaultFileName is_exit_ext_name = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6394_112_2_18_2_6_8_186,(uint8*)"d", is_exit_ext_name);

    if ((PNULL == res_name_ptr) //get the file name by random
        || (!is_exit_ext_name)//combined the suffix by mime_type
        )
    {
        BrwGetDlFileNameByMimeType(dest_file_name_ptr, *dest_file_name_len_ptr, mime_type, res_name_ptr);
    }
    else
    {
        MMI_WSTRNCPY(dest_file_name_ptr, *dest_file_name_len_ptr, 
                res_name_ptr, res_name_len, 
                MIN(*dest_file_name_len_ptr, res_name_len));

        //do nothing;the file name is correct
    }
    
    return TRUE;
}


/*****************************************************************************/
//  Description : handle more popmenu window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandlePopMenuWinMsg(MMI_WIN_ID_T win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM param)
{
    MMI_MENU_ID_T                       menu_id = 0;
    MMI_RESULT_E                        result = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T                       ctrl_id = MMIBROWSER_MENU_CTRL_ID;
    MMI_STRING_T                        url_str = {0};
    char    *url_ptr = PNULL;
    
    //SCI_TRACE_LOW:"BrwHandlePopMenuWinMsg:msg_id = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6429_112_2_18_2_6_8_187,(uint8*)"d", msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        CreatePopWinMenu();
        MMK_SetAtvCtrl(win_id, ctrl_id);						
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        menu_id = GUIMENU_GetCurNodeId(ctrl_id);/*lint !e718*/
        //SCI_TRACE_LOW:"BrwHandlePopMenuWinMsg:menu_id = 0x%x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6443_112_2_18_2_6_8_188,(uint8*)"d", menu_id);
        switch(menu_id) 
        {
        case ID_POPUP_MENU_INPUTADD:
            url_ptr = MMIBROWSER_GetUrl();
            MMIBROWSER_InputUrl((uint8*)url_ptr);
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_OPEN_HOMEPAGE:
            MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, MMIBROWSER_GetHomepageUrl(), PNULL);
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_SUB_SETTING:
            MMIBROWSER_OpenSettingWin();
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_SET_MAIN:
            MMIBROWSER_GetCurPageUrlInfo(PNULL, &url_str.wstr_ptr);
            url_str.wstr_len = MMIAPICOM_Wstrlen(url_str.wstr_ptr);
            if (MMIBROWSER_SetHomepageUrl(&url_str))
            {
#ifdef MMI_GUI_STYLE_TYPICAL
                MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_SET_HOMEPAGE_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            }
//            else
//            {
//                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_SET_HOMEPAGE_FAIL);
//            }
            MMIBROWSER_FreeCurPageUrlInfo(PNULL, &url_str.wstr_ptr);
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_SAVE_BOOKMARK:
            MMIBROWSER_FreeBookmarkEdit();
            MMIBRW_ChangeBookmarkWin(MMIBRW_BM_CHANGE_ADD_CUR_PAGE);
            MMK_CloseWin(win_id);
            break;
        
        case ID_POPUP_MENU_SAVE_INTERNET_FILE:
            {
                MMIFILE_DEVICE_E fmm_dev = MMIAPIFMM_GetValidateDevice(MMI_DEVICE_SDCARD);
                if (BRW_IsPageBlank(MMIBROWSER_GetInstance()))
                {
                    MMIPUB_OpenAlertWarningWin(TXT_BROWSER_SNAPSHOT_SAVE_FILE_BLANK);
                }
                else if(fmm_dev < MMI_DEVICE_NUM)
                {
                    MMIBROWSER_OpenSnapshotSaveFileWin();
                }
                else
                {
                    MMIPUB_OpenAlertWarningWin(TXT_SD_NO_EXIST);
                }
                MMK_CloseWin(win_id);
            }
            break;
            
        case ID_POPUP_MENU_HISTORY:
            MMIBROWSER_OpenHisListWin();
#ifdef BROWSER_START_PAGE_THEME1
            MMIBROWSER_SetSlide(FALSE);
#endif
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_HAS_SAVE_FILE:
            MMIBROWSER_OpenSnapshotListWin();
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_SUB_FILEINFO:
            MMIBROWSER_OpenFileDetailWin();
            MMK_CloseWin(win_id);
            break;

#if 0
        case ID_POPUP_MENU_SUB_CLEARCACHE:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_CACHE,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;

        case ID_POPUP_MENU_SUB_CLEARCOOKIE:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_COOKIE,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
            
        case ID_POPUP_MENU_CLEAR_INPUT_HISTORY:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_INPUT,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
#endif

#if 0//MS00214337          
        case ID_POPUP_MENU_SUB_DISCONNECT:
            MMIBROWSER_SetLoading(FALSE);
            MMIBROWSER_SetParamToStop();
            BRW_DisconnetNetwork(MMIBROWSER_GetInstance());
            MMK_CloseWin(win_id);
            break;
#endif            
            
        case ID_POPUP_MENU_STOP:
            if (MMIBROWSER_GetLoading())
            {
                if (BRW_StopPageReq(MMIBROWSER_GetInstance()))
                {
                    MMIBROWSER_SetLoading(FALSE);
                    MMIBROWSER_SetParamToStop();
                }
                else
                {
                    //SCI_TRACE_LOW:"ID_POPUP_MENU_STOP BRW_StopPageReq error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6553_112_2_18_2_6_8_189,(uint8*)"");
                }
            }
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_REFRESH:
            MMIBROWSER_GetCurPageUrlInfo((uint8**)&url_ptr, PNULL);
            if (BRW_AccessPage(MMIBROWSER_GetInstance(), (uint8*)url_ptr))
            {
                MMIBROWSER_SetToAccess(url_ptr, TRUE);
            }
            MMIBROWSER_FreeCurPageUrlInfo((uint8**)&url_ptr, PNULL);
            MMK_CloseWin(win_id);
            break;

        case ID_POPUP_MENU_FORWARD:
            if (BRW_NavHisIsForwardEnable())
            {
                if (!BRW_ProcNavEvent(MMIBROWSER_GetInstance(),BRW_NAV_EVENT_FORWARD))
                {
                    //SCI_TRACE_LOW:"MMIBROWSER forward error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6574_112_2_18_2_6_8_190,(uint8*)"");
                }
            }
            MMK_CloseWin(win_id);
            break;

        case ID_POPUP_MENU_BACKWARD:
            if (BRW_NavHisIsBackwardEnable())
            {
                if (!BRW_ProcNavEvent(MMIBROWSER_GetInstance(),BRW_NAV_EVENT_BACKWARD))
                {
                    //SCI_TRACE_LOW:"MMIBROWSER backward error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6585_112_2_18_2_6_8_191,(uint8*)"");
                }
            }
            MMK_CloseWin(win_id);
            break;

        case ID_POPUP_MENU_BOOKMARK_LIST:
            MMIBROWSER_OpenBookmarkWin();
#ifdef BROWSER_START_PAGE_THEME1
            MMIBROWSER_SetSlide(FALSE);
#endif
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_SEND_URL:
            {
                uint16  title[MMIBRW_BOOKMARK_MAX_TITLE_LENGTH + 1] = {0};
                uint16  title_len = 0;

                if (BRW_GetPageTitle(MMIBROWSER_GetInstance(),title,&title_len,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH))
                {
                    title_len = MMIAPICOM_Wstrlen(title);//protect
                }
                else
                {
                    title_len = 0;//protect to reset
                }
                            
                MMIBROWSER_GetCurPageUrlInfo(PNULL, &url_str.wstr_ptr);
                url_str.wstr_len = MMIAPICOM_Wstrlen(url_str.wstr_ptr);

                if(MMIBROWSER_SendUrlContentBySms(title, title_len, url_str.wstr_ptr, url_str.wstr_len))
                {
                    //successful
                }
                else
                {
                    MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
                }

                MMIBROWSER_FreeCurPageUrlInfo(PNULL, &url_str.wstr_ptr);
                MMK_CloseWin(win_id);
          }
          break;
            
#ifdef BROWSER_SEARCH_SUPPORT
       case ID_POPUP_MENU_SEARCH:
            MMIBROWSER_OpenSearchWin();
            MMK_CloseWin(win_id);
            break;
#if 0            
        case ID_POPUP_MENU_SUB_CLEARKEY:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_KEY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
#endif            
#endif/*BROWSER_SEARCH_SUPPORT*/

#if 0
        case ID_POPUP_MENU_CLEAR_SUB_ALL:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_ALL,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
#endif

        case ID_POPUP_MENU_EXIT:
            MMIBROWSER_QueryExit();
            break;

        case ID_POPUP_MENU_SAVE_IMG:
        {
            if (BRW_IsFocusHotLinkImg(MMIBROWSER_GetInstance()))
            {
                BRW_MSG_ENTER_FILENM_REQ_T  req_ptr = {0};
                MMIFILE_DEVICE_E    file_type = MMI_DEVICE_NUM;
                
                BRW_GetFocusLinkInfo(MMIBROWSER_GetInstance(), req_ptr.res_name_arr, BRW_MAX_URL_LEN, 
                    &req_ptr.mime_type, PNULL, &req_ptr.content_len);
                    
                file_type = BrowserDlGetDeviceName(PNULL, PNULL, req_ptr.mime_type);//get the first valid device for mime type
                if (file_type < MMI_DEVICE_NUM)//nornal, begin to download
                {
                    req_ptr.dl_type = BRW_DL_TYPE_LOCAL_CONTENT;
                    req_ptr.handle = MMIBROWSER_SAVE_HANDLE_FOCUS;
                    req_ptr.enter_filenm_cbf = EnterFileNmForLocalCbf;
                    MMIBROWSER_UpdateFileNameReq(&req_ptr);
                    MMIBROWSER_OpenSaveMediaFileWin(req_ptr.mime_type, PNULL);
                    result = TRUE;
                }
                else
                {
                    MMIPUB_OpenAlertWarningWin(TXT_SD_NO_EXIST);
                }
            }
            MMK_CloseWin(win_id);
        }
            break;
            
#ifdef SNS_SUPPORT
        case ID_POPUP_MENU_SHARE_TO_SINA:
        case ID_POPUP_MENU_SHARE_TO_FACEBOOK:
        case ID_POPUP_MENU_SHARE_TO_TIWTTER:
        {
            int share_id = 0;
            
            if(ID_POPUP_MENU_SHARE_TO_SINA == menu_id)
            {
                share_id = MMI_SNS_SINA;
            }
            else if(ID_POPUP_MENU_SHARE_TO_FACEBOOK == menu_id)
            {
                share_id = MMI_SNS_FACEBOOK;
            }
            else
            {
                share_id = MMI_SNS_TIWTTER;
            }
            
            IMMISNS_CreateObject(&pMe);
            
            if(pMe)
            {
                if(IMMISNS_IsFixAppInstalled(pMe, share_id) < 0)
                {
                    IMMISNS_Release(pMe);
                    pMe = PNULL;
                    
                    MMIPUB_OpenAlertWinByTextIdEx(MMK_GetFirstAppletHandle(),PNULL,TXT_WHETHER_INSTALL,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_OKCANCEL,HandleBROWSERWhetherInstallWinMsg, share_id);
                }
                else
                {
                    MMIBROWSER_GetCurPageUrlInfo(PNULL, &url_str.wstr_ptr);                   

                    if (PNULL != url_str.wstr_ptr)
                    {
                        uint32        time_num    = 1000;

                        url_str.wstr_len = MMIAPICOM_Wstrlen(url_str.wstr_ptr);
                        IMMISNS_ShareToSns(pMe, share_id, SNS_MODULE_BROWSER , (unsigned long int)&url_str);
                        MMIBROWSER_FreeCurPageUrlInfo(PNULL, &url_str.wstr_ptr);
                        MMIPUB_OpenAlertWinByTextId(&time_num, TXT_COMMON_WAITING,TXT_NULL,IMAGE_PUBWIN_WAIT,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
                    }
                }
            }
        }
        break;
#endif

#ifdef DYNAMIC_MAINMENU_SUPPORT
        case ID_POPUP_MENU_ADD_TO_MAINMENU:
        {
            uint16              title[MMIBRW_BOOKMARK_MAX_TITLE_LENGTH + 1] = {0};
            uint16              title_len = 0;

            MMK_CreateWin((uint32*)MMIBROWSER_ADD2MAIN_EDIT_WIN_TAB,PNULL);

            MMIBROWSER_GetCurPageUrlInfo(PNULL, &url_str.wstr_ptr);
            url_str.wstr_len = MMIAPICOM_Wstrlen(url_str.wstr_ptr);
            GUIEDIT_SetString(MMIBROWSER_ADD2MAIN_EDIT_URL_CTRL_ID, url_str.wstr_ptr, url_str.wstr_len);
            MMIBROWSER_FreeCurPageUrlInfo(PNULL, &url_str.wstr_ptr);

            if (BRW_GetPageTitle(MMIBROWSER_GetInstance(),title,&title_len,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH))
            {
                GUIEDIT_SetString(MMIBROWSER_ADD2MAIN_EDIT_TITLE_CTRL_ID, title, title_len);
            }     
            
            MMK_CloseWin(win_id);
        }
            break;
#endif

        default:
            break;
        }
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
#if 0
    case MSG_PROMPTWIN_OK:
        MMIBROWSER_CloseQuerytWin(PNULL);
        menu_id = GUIMENU_GetCurNodeId(ctrl_id);/*lint !e718*/
        switch(menu_id)
        {
        case ID_POPUP_MENU_SUB_CLEARCACHE:
            if (!BRW_ClearCache(MMIBROWSER_GetInstance()))
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_CACHE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCache fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6776_112_2_18_2_6_8_192,(uint8*)"");
            }
            else
            {
#ifdef MMI_GUI_STYLE_TYPICAL
                MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_CACHE_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            }
            break;
            
        case ID_POPUP_MENU_SUB_CLEARCOOKIE:
            if (!BRW_ClearCookie(MMIBROWSER_GetInstance()))
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_COOKIE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCookie fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6788_112_2_18_2_6_8_193,(uint8*)"");
            }
            else
            {
#ifdef MMI_GUI_STYLE_TYPICAL
                MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_COOKIE_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            }
            break;

        case ID_POPUP_MENU_CLEAR_INPUT_HISTORY:
            BrwDeleteInputHis();
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_INPUT_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;
            
 #ifdef BROWSER_SEARCH_SUPPORT
       case ID_POPUP_MENU_SUB_CLEARKEY:
            MMIBROWSER_CleanKeyInfo();
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_KEY_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;
#endif

        case ID_POPUP_MENU_CLEAR_SUB_ALL:
            BrwDeleteInputHis();
            
#ifdef BROWSER_SEARCH_SUPPORT
            MMIBROWSER_CleanKeyInfo();
#endif            
            if (!BRW_ClearCache(MMIBROWSER_GetInstance()))
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_CACHE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCache fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6817_112_2_18_2_6_8_194,(uint8*)"");
                break;
            }
            if (!BRW_ClearCookie(MMIBROWSER_GetInstance()))
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_COOKIE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCookie fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_6823_112_2_18_2_6_8_195,(uint8*)"");
                break;
            }
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_ALL_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;

        default:
            break;
            
        }
        MMK_CloseWin(win_id);
        break;
        
    case MSG_PROMPTWIN_CANCEL:
        MMIBROWSER_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;   
#endif

#ifdef SNS_SUPPORT
    case MSG_SNS_OPERA_RESULT:
        {
            if(pMe)
            {
                IMMISNS_Release(pMe);
                pMe = PNULL;
            }
        }
        break;
#endif    
    
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return result;
    
}

/*****************************************************************************/
//  Description : handle input username window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleInputUserNameWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM param
                                   )
{
    MMI_RESULT_E        ret=MMI_RESULT_TRUE;    
//     wchar               wchar_username_arr[MMIBROWSER_MAX_USER_NAME_LEN+2]={0};
//     uint16              username_len=0;
    MMI_CTRL_ID_T       ctrl_id=MMIBROWSER_EDIT_USERNAME_CTRL_ID;
    BRW_MSG_AUTH_REQ_T* param_ptr=PNULL;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;

    case MSG_CLOSE_WINDOW:        
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
        {
            MMI_STRING_T 	text_str = {0};
            GUIRICHTEXT_ITEM_T  item_data = {0};
            
            param_ptr=MMIBROWSER_GetAuthReq();
            GUIEDIT_GetString(ctrl_id,&text_str);
            if(PNULL==text_str.wstr_ptr || 0==text_str.wstr_len)
            {
                //invalid user name
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_USERNAME_INVALID);
                break;
            }
            
            MMIBROWSER_UpdateUserName(text_str.wstr_ptr);
            
            item_data.img_type = GUIRICHTEXT_IMAGE_NONE;
            item_data.text_type = GUIRICHTEXT_TEXT_BUF;
            item_data.text_frame_type = GUIRICHTEXT_FRAME_NORMAL;
            item_data.tag_type = GUIRICHTEXT_TAG_NORMAL;
            item_data.text_data.buf.len = MIN(text_str.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
            item_data.text_data.buf.str_ptr = text_str.wstr_ptr;
            GUIRICHTEXT_ModifyItem(MMIBROWSER_RICHTEXT_AUTH_CTRL_ID,2,&item_data);
            GUIRICHTEXT_Update(MMIBROWSER_RICHTEXT_AUTH_CTRL_ID);
            MMK_CloseWin(win_id);
        }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMK_CloseWin(win_id);
        break;

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}

/*****************************************************************************/
//  Description : handle input password window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleInputPWDWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E        ret=MMI_RESULT_TRUE;    
//     wchar               wchar_pwd_arr[MMIBROWSER_MAX_PWD_LEN+2]={0};
//     uint16              pwd_len=0;
    MMI_CTRL_ID_T       ctrl_id=MMIBROWSER_EDIT_PWD_CTRL_ID;
    BRW_MSG_AUTH_REQ_T* param_ptr=PNULL;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //set im
        GUIEDIT_SetIm(ctrl_id,GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL,GUIIM_TYPE_ENGLISH);/*lint !e655*/ 
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;

    case MSG_CLOSE_WINDOW:
        param_ptr=MMIBROWSER_GetAuthReq();
        break;

    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
        {
            MMI_STRING_T        text_str = {0};
            uint8               pwd_arr[MMIBROWSER_MAX_PWD_LEN+1]={0};
            GUIRICHTEXT_ITEM_T  item_data = {0};
            wchar               password_display[MMIBROWSER_MAX_PWD_LEN + 1] = {0};
            
            GUIEDIT_GetString(ctrl_id,&text_str);
            if(PNULL==text_str.wstr_ptr || 0==text_str.wstr_len)
            {
                break;
            }
            
            MMIAPICOM_WstrToStr(text_str.wstr_ptr, pwd_arr);
            MMIBROWSER_UpdatePassword((char*)pwd_arr);
            item_data.img_type = GUIRICHTEXT_IMAGE_NONE;
            item_data.text_type = GUIRICHTEXT_TEXT_BUF;
            item_data.text_frame_type = GUIRICHTEXT_FRAME_NORMAL;
            item_data.tag_type = GUIRICHTEXT_TAG_NORMAL;
            item_data.text_data.buf.len = MIN(text_str.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
            CFLWCHAR_StrNCpyWithAsciiStr(password_display,"********************",MMIBROWSER_MAX_PWD_LEN + 1);
            item_data.text_data.buf.str_ptr = password_display;
            GUIRICHTEXT_ModifyItem(MMIBROWSER_RICHTEXT_AUTH_CTRL_ID,4,&item_data);
            GUIRICHTEXT_Update(MMIBROWSER_RICHTEXT_AUTH_CTRL_ID);
            MMK_CloseWin(win_id);
        }
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMK_CloseWin(win_id);
        break;
        
    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}


/*****************************************************************************/
//  Description : add two line list item
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_AppendTwoLineTextListItem(
                                                     MMI_CTRL_ID_T       ctrl_id,
                                                     MMI_TEXT_ID_T       left_softkey_id,
                                                     MMI_STRING_T        string_1,
                                                     MMI_STRING_T        string_2,
                                                     MMI_IMAGE_ID_T      image,
                                                     uint16              pos,
                                                     BOOLEAN             is_update
                                                     )
{
    GUILIST_ITEM_T      item_t = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T        dst_temp = {0};
    MMI_TEXT_ID_T       r_softkey_id = STXT_RETURN;

#ifdef BROWSER_START_PAGE_THEME1
    if (MMIBROWSER_HISTORY_LIST_CTRL_ID == ctrl_id)
    {
        r_softkey_id = STXT_EXIT;
    }
#endif
    
#ifdef MMI_PDA_SUPPORT
    item_t.item_style = GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ICON;


    item_t.item_data_ptr = &item_data;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = TXT_COMM_OPEN;
    item_data.softkey_id[2] = r_softkey_id;

    item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
    item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[3].item_data_type = GUIITEM_DATA_TEXT_BUFFER;

    item_data.item_content[0].item_data.image_id = image;

    if(string_1.wstr_ptr!=PNULL && string_1.wstr_len!=0)
    {
        item_data.item_content[1].item_data.text_buffer.wstr_len = string_1.wstr_len;
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = string_1.wstr_ptr;
    }
    else
    {
        MMI_GetLabelTextByLang(TXT_NO_TITLE, &dst_temp);
        item_data.item_content[1].item_data.text_buffer.wstr_len = dst_temp.wstr_len;
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = dst_temp.wstr_ptr;
    }   
    
    item_data.item_content[3].item_data.text_buffer.wstr_len = string_2.wstr_len;
    item_data.item_content[3].item_data.text_buffer.wstr_ptr = string_2.wstr_ptr;

    if( is_update )
    {
        GUILIST_ReplaceItem( ctrl_id, &item_t, pos );
    }
    else
    {
        GUILIST_AppendItem( ctrl_id, &item_t );
    }
#else
    item_t.item_style    = GUIITEM_STYLE_ONE_TWO_LINE_ICON_TEXT_AND_TEXT;

    item_t.item_data_ptr = &item_data;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = TXT_COMM_OPEN;
    item_data.softkey_id[2] = r_softkey_id;
    
    item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
    item_data.item_content[0].item_data.image_id = image;


    item_data.item_content[1].item_data_type = item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    
    if(string_1.wstr_ptr!=PNULL && string_1.wstr_len!=0)
    {
        item_data.item_content[1].item_data.text_buffer.wstr_len = string_1.wstr_len;
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = string_1.wstr_ptr;
    }
    else
    {
        MMI_GetLabelTextByLang(TXT_NO_TITLE, &dst_temp);
        item_data.item_content[1].item_data.text_buffer.wstr_len = dst_temp.wstr_len;
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = dst_temp.wstr_ptr;
    }   
    
    item_data.item_content[2].item_data.text_buffer.wstr_len = string_2.wstr_len;
    item_data.item_content[2].item_data.text_buffer.wstr_ptr = string_2.wstr_ptr;

    if( is_update )
    {
        GUILIST_ReplaceItem( ctrl_id, &item_t, pos );
    }
    else
    {
        GUILIST_AppendItem( ctrl_id, &item_t );
    }
#endif /* MMI_PDA_SUPPORT */

}

/*****************************************************************************/
//  Description : create scroll bar
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL uint32 BrwScrollBarCreate(uint32 win_handle, GUI_BOTH_RECT_T *rect_ptr , MMI_IMAGE_ID_T   bg_id)
{
    MMI_CONTROL_CREATE_T    create      = {0};
    GUIPRGBOX_INIT_DATA_T   init_data   = {0};
    GUIPRGBOX_CTRL_T*       prgbar_ptr = NULL;

    if(PNULL==rect_ptr)
    {
        //SCI_TRACE_LOW:"BrwScrollBarCreate ::PNULL==rect_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_7120_112_2_18_2_6_9_196,(uint8*)"");
        return 0;
    }

    SCI_MEMCPY(&init_data.both_rect, rect_ptr, sizeof(GUI_BOTH_RECT_T));
    init_data.lcd_id        = GUI_MAIN_LCD_ID;
    init_data.style         = GUIPRGBOX_STYLE_VERTICAL_WINDOWS;
    create.ctrl_id              = MMIBROWSER_PRGBOX_CTRL_ID;
    create.guid                 = SPRD_GUI_PRGBOX_ID;
    create.parent_win_handle    = (MMI_HANDLE_T)win_handle;
    create.init_data_ptr        = &init_data;

    prgbar_ptr = (GUIPRGBOX_CTRL_T*)MMK_CreateControl( &create );
    if(!prgbar_ptr) 
    {
        return 0;
    }
    GUIPRGBOX_SetParamByPtr(prgbar_ptr, 0, 0);
    GUIPRGBOX_SetPosByPtr(TRUE, 0, 0,prgbar_ptr);
#ifndef MMI_PDA_SUPPORT
    GUIPRGBOX_SetBgColor(create.ctrl_id, MMI_WHITE_COLOR);
    GUIPRGBOX_SetScrollBarImg(create.ctrl_id, bg_id);
#endif

    return (uint32)prgbar_ptr;
}

/*****************************************************************************/
//  Description : check if browser is inited
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDORADO_IsRunning(void)
{
    if (MMK_IsOpenWin(MMIBROWSER_MAIN_WIN_ID) || MMK_IsOpenWin(MMIBROWSER_START_PAGE_WIN_ID))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : check if main browser is the focused win
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIBROWSER_IsMainActive(void)
{
    return MMK_IsFocusWin(MMIBROWSER_MAIN_WIN_ID);
}

/*****************************************************************************/
//  Description : check if browser is the focused win
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIDORADO_IsActive(void)
{
    MMI_WIN_ID_T win_id = MMI_BRW_BEGIN_WIN_ID;

    for (win_id = MMI_BRW_BEGIN_WIN_ID + 1; win_id <  MMIBROWSER_WINDOW_ID_NUMBER; win_id++)
    {
        if (MMK_IsFocusWin(win_id))
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

/*****************************************************************************/
//  Description : create pop menu
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CreatePopWinMenu(void)
{
    MMI_CTRL_ID_T       ctrl_id = MMIBROWSER_MENU_CTRL_ID;
    MMI_STRING_T        kstring = {0};
    uint16              nodex_index =0;
    uint32              i = 0;
    uint32              item_num = 0;  
    GUIMENU_DYNA_ITEM_T node_item = {0};
    uint32  node_id = 0;

    item_num = sizeof(s_menu_brw_popupmenu_table)/sizeof(*s_menu_brw_popupmenu_table);
    nodex_index = 0;
    for(i = 0; i < item_num; i++)
    {
        node_id = s_menu_brw_popupmenu_table[i][0];
        MMI_GetLabelTextByLang(s_menu_brw_popupmenu_table[i][1], &kstring);
        node_item.item_text_ptr = &kstring;
#ifdef MMI_PDA_SUPPORT
        node_item.select_icon_id = s_menu_brw_popupmenu_table[i][2];
#endif /* MMI_PDA_SUPPORT */
        GUIMENU_InsertNode(nodex_index, node_id, 0, &node_item, ctrl_id);

        if ((ID_POPUP_MENU_ADD_TO_MAINMENU == node_id) 
            && (MMIBROWSER_GetLoading() || BRW_IsPageBlank(MMIBROWSER_GetInstance())))
        {
            GUIMENU_SetNodeGrayed(TRUE, nodex_index, 0, ctrl_id);
        }
#ifdef MMI_PDA_SUPPORT//NEWMS00163973
        if (((ID_POPUP_MENU_FORWARD == node_id) && !BRW_NavHisIsForwardEnable())
            || ((ID_POPUP_MENU_BACKWARD == node_id) && !BRW_NavHisIsBackwardEnable())
            )
        {
            GUIMENU_SetNodeGrayed(TRUE, nodex_index, 0, ctrl_id);
        }
#endif

        nodex_index ++;
    }

#if 0
    item_num = sizeof(s_menu_brw_clear_popupmenu_table)/sizeof(*s_menu_brw_clear_popupmenu_table);
    nodex_index = 0;
    for(i = 0; i < item_num; i++)
    {
        node_id = s_menu_brw_clear_popupmenu_table[i][0];
        MMI_GetLabelTextByLang(s_menu_brw_clear_popupmenu_table[i][1], &kstring);
        node_item.item_text_ptr = &kstring;
        GUIMENU_InsertNode(nodex_index, node_id, ID_POPUP_MENU_CLEAR, &node_item, ctrl_id);
        
        //disable
        if ((ID_POPUP_MENU_SUB_CLEARCACHE == node_id) || (ID_POPUP_MENU_SUB_CLEARCOOKIE == node_id))
        {
            if (MMIAPIFMM_GetFirstValidDevice() >= MMI_DEVICE_NUM)
            {
                GUIMENU_SetNodeGrayed(TRUE, nodex_index, ID_POPUP_MENU_CLEAR, ctrl_id);
            }
        }

        nodex_index ++;
    }
#endif    
    
    item_num = sizeof(s_menu_brw_open_popupmenu_table)/sizeof(*s_menu_brw_open_popupmenu_table);
    nodex_index = 0;
    for(i = 0; i < item_num; i++)
    {
        node_id = s_menu_brw_open_popupmenu_table[i][0];
        MMI_GetLabelTextByLang(s_menu_brw_open_popupmenu_table[i][1], &kstring);
        node_item.item_text_ptr = &kstring;
        GUIMENU_InsertNode(nodex_index, node_id, ID_POPUP_MENU_OPEN, &node_item, ctrl_id);
        nodex_index ++;
    }
    
    item_num = sizeof(s_menu_brw_nav_popupmenu_table)/sizeof(*s_menu_brw_nav_popupmenu_table);
    nodex_index = 0;
    for(i = 0; i < item_num; i++)
    {
        node_id = s_menu_brw_nav_popupmenu_table[i][0];
        MMI_GetLabelTextByLang(s_menu_brw_nav_popupmenu_table[i][1], &kstring);
        node_item.item_text_ptr = &kstring;
        GUIMENU_InsertNode(nodex_index, node_id, ID_POPUP_MENU_NAV, &node_item, ctrl_id);
        if (((ID_POPUP_MENU_FORWARD == node_id) && !BRW_NavHisIsForwardEnable())
            || ((ID_POPUP_MENU_BACKWARD == node_id) && !BRW_NavHisIsBackwardEnable())
            || ((ID_POPUP_MENU_STOP == node_id) && !MMIBROWSER_GetLoading())
            || ((ID_POPUP_MENU_REFRESH == node_id) && MMIBROWSER_GetLoading())
            )
        {
            GUIMENU_SetNodeGrayed(TRUE, nodex_index, ID_POPUP_MENU_NAV, ctrl_id);
        }

        nodex_index ++;
    }
    
    item_num = sizeof(s_menu_brw_save_popupmenu_table)/sizeof(*s_menu_brw_save_popupmenu_table);
    nodex_index = 0;
    for(i = 0; i < item_num; i++)
    {
        node_id = s_menu_brw_save_popupmenu_table[i][0];
        MMI_GetLabelTextByLang(s_menu_brw_save_popupmenu_table[i][1], &kstring);
        node_item.item_text_ptr = &kstring;
        GUIMENU_InsertNode(nodex_index, node_id, ID_POPUP_MENU_SAVE, &node_item, ctrl_id);
        if (((ID_POPUP_MENU_SAVE_INTERNET_FILE == node_id) && MMIBROWSER_GetLoading())
            || ((ID_POPUP_MENU_SAVE_IMG == node_id) && !BRW_IsFocusHotLinkImg(MMIBROWSER_GetInstance()))
            )
        {
            GUIMENU_SetNodeGrayed(TRUE, nodex_index, ID_POPUP_MENU_SAVE, ctrl_id);
        }
        nodex_index ++;
    }
    
#ifdef SNS_SUPPORT
#if (defined(WRE_WEIBO_SUPPORT)) || (defined(WRE_FACEBOOK_SUPPORT)) || (defined(WRE_TWITTER_SUPPORT))
    item_num = sizeof(s_menu_brw_share_sns_popupmenu_table)/sizeof(*s_menu_brw_share_sns_popupmenu_table);
    nodex_index = 0;
    for(i = 0; i < item_num; i++)
    {
        node_id = s_menu_brw_share_sns_popupmenu_table[i][0];
        MMI_GetLabelTextByLang(s_menu_brw_share_sns_popupmenu_table[i][1], &kstring);
        node_item.item_text_ptr = &kstring;
        GUIMENU_InsertNode(nodex_index, node_id, ID_POPUP_MENU_SHARE_TO_SNS, &node_item, ctrl_id);

        nodex_index ++;
    }
#endif    
#endif

#ifdef MMI_PDA_SUPPORT
    {
        GUIMENU_BUTTON_INFO_T button_info = {0};

        button_info.is_static   = FALSE;
        button_info.button_style = GUIMENU_BUTTON_STYLE_CANCEL;
        button_info.node_id = ID_POPUP_MENU_SAVE;
        GUIMENU_SetButtonStyle(MMIBROWSER_MENU_CTRL_ID, &button_info);
    }
#endif /* MMI_PDA_SUPPORT */

    return TRUE;
} 

/*****************************************************************************/
//  Description : close browser 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
PUBLIC MMIBROWSER_ERROR_E MMIDORADO_Close(void)
{
    MMIBROWSER_Exit();
    return MMIBROWSER_SUCC;
}

/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwPositonInfo(MMI_WIN_ID_T win_id)
{    
    GUI_BOTH_RECT_T client_rect = {0};
//     uint16 font_height = GUIFONT_GetHeight(MMIBROWSER_ADDR_LABEL_FONT);
    uint16  img_width = 0;
    uint16  img_height = 0;
    uint16  img2_width = 0;
    uint16  img2_height = 0;
    uint16  scroll_width = 0;
    //uint16  scroll_height = 0;
    //uint16  button_width = 0;
    uint16  button_height = 0;
    uint16  addr_bar_height = 0;
    uint16  connect_anim_width = 0;
    uint16  connect_anim_height = 0;

    client_rect = MMITHEME_GetWinClientBothRect(win_id);//without title, statusbar, softkey

    //get scroll image info    
#if !defined(MMI_PDA_SUPPORT)
    GUIRES_GetImgWidthHeight(&scroll_width, PNULL, IMAGE_BROWSER_SCROLL_VER_GROOVE, win_id);
#endif

    //get toolbar image info    
#ifdef BROWSER_TOOLBAR
    GUIRES_GetImgWidthHeight(PNULL, &button_height, IMAGE_BROWSER_TOOLBAR, win_id);
#endif
    
    //get address bar image info    
    GUIRES_GetImgWidthHeight(PNULL, &addr_bar_height, IMAGE_BROWSER_TOOLBAR, win_id);

    addr_bar_height = (0 == addr_bar_height)? 1: addr_bar_height;//for protect
    button_height = (0 == button_height)? 1: button_height;//for protect
    scroll_width = (0 == scroll_width)? 1: scroll_width;//for protect
  
    s_brw_show_info.addr_bar.v_rect.left = 0;
    s_brw_show_info.addr_bar.v_rect.top = client_rect.v_rect.top;
    s_brw_show_info.addr_bar.v_rect.right = client_rect.v_rect.right;
    s_brw_show_info.addr_bar.v_rect.bottom = s_brw_show_info.addr_bar.v_rect.top + addr_bar_height - 1;
    s_brw_show_info.addr_bar.h_rect.left = 0;
    s_brw_show_info.addr_bar.h_rect.top = client_rect.h_rect.top;
    s_brw_show_info.addr_bar.h_rect.right = client_rect.h_rect.right;
    s_brw_show_info.addr_bar.h_rect.bottom = s_brw_show_info.addr_bar.h_rect.top + addr_bar_height - 1;

#ifdef BROWSER_TOOLBAR
    s_brw_show_info.toolbar.v_rect.left = 0;
    s_brw_show_info.toolbar.v_rect.bottom = client_rect.v_rect.bottom;
    s_brw_show_info.toolbar.v_rect.right = client_rect.v_rect.right;
    s_brw_show_info.toolbar.v_rect.top = s_brw_show_info.toolbar.v_rect.bottom - button_height + 1;
    s_brw_show_info.toolbar.h_rect.left = 0;
    s_brw_show_info.toolbar.h_rect.bottom = client_rect.h_rect.bottom;
    s_brw_show_info.toolbar.h_rect.right = client_rect.h_rect.right;
    s_brw_show_info.toolbar.h_rect.top = s_brw_show_info.toolbar.h_rect.bottom - button_height + 1;
#endif
    
    s_brw_show_info.web_page.v_rect.left = 0;
    s_brw_show_info.web_page.v_rect.top = s_brw_show_info.addr_bar.v_rect.bottom + 1;
    s_brw_show_info.web_page.v_rect.right = client_rect.v_rect.right - scroll_width + 1;
#ifdef BROWSER_TOOLBAR
    s_brw_show_info.web_page.v_rect.bottom = s_brw_show_info.toolbar.v_rect.top - 1;
#else
    s_brw_show_info.web_page.v_rect.bottom = client_rect.v_rect.bottom;
#endif
    
    s_brw_show_info.web_page.h_rect.left = 0;
    s_brw_show_info.web_page.h_rect.top = s_brw_show_info.addr_bar.h_rect.bottom + 1;
    s_brw_show_info.web_page.h_rect.right = client_rect.h_rect.right - scroll_width + 1;
#ifdef BROWSER_TOOLBAR
    s_brw_show_info.web_page.h_rect.bottom = s_brw_show_info.toolbar.h_rect.top - 1;
#else
    s_brw_show_info.web_page.h_rect.bottom = client_rect.h_rect.bottom;
#endif
    
    s_brw_show_info.scroll.v_rect.right = client_rect.v_rect.right ;
    s_brw_show_info.scroll.v_rect.left = s_brw_show_info.scroll.v_rect.right - scroll_width + 1;
    s_brw_show_info.scroll.v_rect.top = s_brw_show_info.web_page.v_rect.top;
    s_brw_show_info.scroll.v_rect.bottom = s_brw_show_info.web_page.v_rect.bottom;
    s_brw_show_info.scroll.h_rect.right = client_rect.h_rect.right ;
    s_brw_show_info.scroll.h_rect.left = s_brw_show_info.scroll.h_rect.right - scroll_width + 1;
    s_brw_show_info.scroll.h_rect.top = s_brw_show_info.web_page.h_rect.top;
    s_brw_show_info.scroll.h_rect.bottom = s_brw_show_info.web_page.h_rect.bottom;

    //取connection animal的最大尺寸
    GUIRES_GetAnimWidthHeight(&img_width, &img_height, PNULL, IMAGE_BROWSER_CONNECTION_ANIM, win_id);
    GUIRES_GetAnimWidthHeight(&img2_width, &img2_height, PNULL, IMAGE_BROWSER_SSL_CONNECTION_ANIM, win_id);
    connect_anim_width = MAX(img_width, img2_width);
    connect_anim_height = MAX(img_height, img2_height);

    connect_anim_width = (0 == connect_anim_width)? 1: connect_anim_width;//for protect
    connect_anim_height = (0 == connect_anim_height)? 1: connect_anim_height;//for protect

    s_brw_show_info.connect_anim.v_rect.left = s_brw_show_info.addr_bar.v_rect.left + MMIBROWSER_ANIMA_RELATIVE_LEFT;
    s_brw_show_info.connect_anim.v_rect.top =(s_brw_show_info.addr_bar.v_rect.top + s_brw_show_info.addr_bar.v_rect.bottom - connect_anim_height)/2;
    s_brw_show_info.connect_anim.v_rect.right = s_brw_show_info.connect_anim.v_rect.left + connect_anim_width -1;
    s_brw_show_info.connect_anim.v_rect.bottom = s_brw_show_info.connect_anim.v_rect.top + connect_anim_height - 1;

    s_brw_show_info.connect_anim.h_rect.left = s_brw_show_info.addr_bar.h_rect.left + MMIBROWSER_ANIMA_RELATIVE_LEFT;
    s_brw_show_info.connect_anim.h_rect.top =(s_brw_show_info.addr_bar.h_rect.top + s_brw_show_info.addr_bar.h_rect.bottom - connect_anim_height)/2;
    s_brw_show_info.connect_anim.h_rect.right = s_brw_show_info.connect_anim.h_rect.left + connect_anim_width -1;
    s_brw_show_info.connect_anim.h_rect.bottom = s_brw_show_info.connect_anim.h_rect.top + connect_anim_height - 1;

    GUIRES_GetImgWidthHeight(PNULL, &img_height, IMAGE_BROWSER_LABEL_BG, win_id);
    s_brw_show_info.addr_label.v_rect.left = s_brw_show_info.connect_anim.v_rect.right + MMIBROWSER_LABEL_RELATIVE_LEFT;
    s_brw_show_info.addr_label.v_rect.top = (s_brw_show_info.addr_bar.v_rect.top + s_brw_show_info.addr_bar.v_rect.bottom - img_height)/2;
    s_brw_show_info.addr_label.v_rect.right = s_brw_show_info.addr_bar.v_rect.right - MMIBROWSER_LABEL_RELATIVE_RIGHT;
    s_brw_show_info.addr_label.v_rect.bottom = s_brw_show_info.addr_label.v_rect.top + img_height -1;

    s_brw_show_info.addr_label.h_rect.left = s_brw_show_info.connect_anim.h_rect.right + MMIBROWSER_LABEL_RELATIVE_LEFT;
    s_brw_show_info.addr_label.h_rect.top = (s_brw_show_info.addr_bar.h_rect.top + s_brw_show_info.addr_bar.h_rect.bottom - img_height)/2;
    s_brw_show_info.addr_label.h_rect.right = s_brw_show_info.addr_bar.h_rect.right - MMIBROWSER_LABEL_RELATIVE_RIGHT;
    s_brw_show_info.addr_label.h_rect.bottom = s_brw_show_info.addr_label.h_rect.top + img_height -1;

}

/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwHorPositonInfo(MMI_WIN_ID_T win_id)
{    
    GUI_BOTH_RECT_T client_rect = {0};
//     uint16 font_height = GUIFONT_GetHeight(MMIBROWSER_ADDR_LABEL_FONT);
    uint16  img_width = 0;
    uint16  img_height = 0;
    uint16  img2_width = 0;
    uint16  img2_height = 0;
    uint16  scroll_width = 0;
    //uint16  scroll_height = 0;
   // uint16  button_width = 0;
    uint16  button_height = 0;
    uint16  addr_bar_height = 0;
    uint16  connect_anim_width = 0;
    uint16  connect_anim_height = 0;

    client_rect = MMITHEME_GetWinClientBothRect(win_id);//without title, statusbar, softkey

    //get scroll image info    
#if !defined(MMI_PDA_SUPPORT)
    GUIRES_GetImgWidthHeight(&scroll_width, PNULL, IMAGE_BROWSER_SCROLL_VER_GROOVE, win_id);
#endif

    //get toolbar image info    
#ifdef BROWSER_TOOLBAR
    GUIRES_GetImgWidthHeight(PNULL, &button_height, IMAGE_BROWSER_TOOLBAR, win_id);
#endif
    
    //get address bar image info    
    GUIRES_GetImgWidthHeight(PNULL, &addr_bar_height, IMAGE_BROWSER_TOOLBAR, win_id);

    addr_bar_height = (0 == addr_bar_height)? 1: addr_bar_height;//for protect
    button_height = (0 == button_height)? 1: button_height;//for protect
    scroll_width = (0 == scroll_width)? 1: scroll_width;//for protect
  
    s_brw_show_info.addr_bar.h_rect.left = 0;
    s_brw_show_info.addr_bar.h_rect.top = client_rect.h_rect.top;
    s_brw_show_info.addr_bar.h_rect.right = client_rect.h_rect.right;
    s_brw_show_info.addr_bar.h_rect.bottom = s_brw_show_info.addr_bar.h_rect.top + addr_bar_height - 1;

#ifdef BROWSER_TOOLBAR
    s_brw_show_info.toolbar.h_rect.left = 0;
    s_brw_show_info.toolbar.h_rect.bottom = client_rect.h_rect.bottom;
    s_brw_show_info.toolbar.h_rect.right = client_rect.h_rect.right;
    s_brw_show_info.toolbar.h_rect.top = s_brw_show_info.toolbar.h_rect.bottom - button_height + 1;
#endif
    
    s_brw_show_info.web_page.h_rect.left = 0;
    s_brw_show_info.web_page.h_rect.top = s_brw_show_info.addr_bar.h_rect.bottom + 1;
    s_brw_show_info.web_page.h_rect.right = client_rect.h_rect.right - scroll_width + 1;
#ifdef BROWSER_TOOLBAR
    s_brw_show_info.web_page.h_rect.bottom = s_brw_show_info.toolbar.h_rect.top - 1;
#else
    s_brw_show_info.web_page.h_rect.bottom = client_rect.h_rect.bottom;
#endif

    //scroll
    s_brw_show_info.scroll.h_rect.right = client_rect.h_rect.right ;
    s_brw_show_info.scroll.h_rect.left = s_brw_show_info.scroll.h_rect.right - scroll_width + 1;
    s_brw_show_info.scroll.h_rect.top = s_brw_show_info.web_page.h_rect.top;
    s_brw_show_info.scroll.h_rect.bottom = s_brw_show_info.web_page.h_rect.bottom;
    //GUIAPICTRL_SetBothRect(MMIBROWSER_PRGBOX_CTRL_ID, &s_brw_show_info.scroll);

    //取connection animal的最大尺寸
    GUIRES_GetAnimWidthHeight(&img_width, &img_height, PNULL, IMAGE_BROWSER_CONNECTION_ANIM, win_id);
    GUIRES_GetAnimWidthHeight(&img2_width, &img2_height, PNULL, IMAGE_BROWSER_SSL_CONNECTION_ANIM, win_id);
    connect_anim_width = MAX(img_width, img2_width);
    connect_anim_height = MAX(img_height, img2_height);

    connect_anim_width = (0 == connect_anim_width)? 1: connect_anim_width;//for protect
    connect_anim_height = (0 == connect_anim_height)? 1: connect_anim_height;//for protect

    s_brw_show_info.connect_anim.h_rect.left = s_brw_show_info.addr_bar.h_rect.left + MMIBROWSER_ANIMA_RELATIVE_LEFT;
    s_brw_show_info.connect_anim.h_rect.top =(s_brw_show_info.addr_bar.h_rect.top + s_brw_show_info.addr_bar.h_rect.bottom - connect_anim_height)/2;
    s_brw_show_info.connect_anim.h_rect.right = s_brw_show_info.connect_anim.h_rect.left + connect_anim_width -1;
    s_brw_show_info.connect_anim.h_rect.bottom = s_brw_show_info.connect_anim.h_rect.top + connect_anim_height - 1;
    //GUIAPICTRL_SetBothRect(MMIBROWSER_ANIMATE_CONNECTING_CTRL_ID, &s_brw_show_info.connect_anim);

    //addr label
    GUIRES_GetImgWidthHeight(PNULL, &img_height, IMAGE_BROWSER_STARTPAGE_LABEL_BG, win_id);
    s_brw_show_info.addr_label.h_rect.left = s_brw_show_info.connect_anim.h_rect.right + MMIBROWSER_LABEL_RELATIVE_LEFT;
    s_brw_show_info.addr_label.h_rect.top = (s_brw_show_info.addr_bar.h_rect.top + s_brw_show_info.addr_bar.h_rect.bottom - img_height)/2;
    s_brw_show_info.addr_label.h_rect.right = s_brw_show_info.addr_bar.h_rect.right - MMIBROWSER_LABEL_RELATIVE_RIGHT;
    s_brw_show_info.addr_label.h_rect.bottom = s_brw_show_info.addr_label.h_rect.top + img_height -1;
    //GUIAPICTRL_SetBothRect(MMIBROWSER_LABEL_ADDRESS_CTRL_ID, &s_brw_show_info.addr_label);


}

/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwVerPositonInfo(MMI_WIN_ID_T win_id)
{    
    GUI_BOTH_RECT_T client_rect = {0};
//     uint16 font_height = GUIFONT_GetHeight(MMIBROWSER_ADDR_LABEL_FONT);
    uint16  img_width = 0;
    uint16  img_height = 0;
    uint16  img2_width = 0;
    uint16  img2_height = 0;
    uint16  scroll_width = 0;
    //uint16  scroll_height = 0;
   // uint16  button_width = 0;
    uint16  button_height = 0;
    uint16  addr_bar_height = 0;
    uint16  connect_anim_width = 0;
    uint16  connect_anim_height = 0;

    client_rect = MMITHEME_GetWinClientBothRect(win_id);//without title, statusbar, softkey

    //get scroll image info    
#if !defined(MMI_PDA_SUPPORT)
    GUIRES_GetImgWidthHeight(&scroll_width, PNULL, IMAGE_BROWSER_SCROLL_VER_GROOVE, win_id);
#endif

    //get toolbar image info    
#ifdef BROWSER_TOOLBAR
    GUIRES_GetImgWidthHeight(PNULL, &button_height, IMAGE_BROWSER_TOOLBAR, win_id);
#endif
    
    //get address bar image info    
    GUIRES_GetImgWidthHeight(PNULL, &addr_bar_height, IMAGE_BROWSER_TOOLBAR, win_id);

    addr_bar_height = (0 == addr_bar_height)? 1: addr_bar_height;//for protect
    button_height = (0 == button_height)? 1: button_height;//for protect
    scroll_width = (0 == scroll_width)? 1: scroll_width;//for protect
  
    s_brw_show_info.addr_bar.v_rect.left = 0;
    s_brw_show_info.addr_bar.v_rect.top = client_rect.v_rect.top;
    s_brw_show_info.addr_bar.v_rect.right = client_rect.v_rect.right;
    s_brw_show_info.addr_bar.v_rect.bottom = s_brw_show_info.addr_bar.v_rect.top + addr_bar_height - 1;

#ifdef BROWSER_TOOLBAR
    s_brw_show_info.toolbar.v_rect.left = 0;
    s_brw_show_info.toolbar.v_rect.bottom = client_rect.v_rect.bottom;
    s_brw_show_info.toolbar.v_rect.right = client_rect.v_rect.right;
    s_brw_show_info.toolbar.v_rect.top = s_brw_show_info.toolbar.v_rect.bottom - button_height + 1;
#endif
    
    s_brw_show_info.web_page.v_rect.left = 0;
    s_brw_show_info.web_page.v_rect.top = s_brw_show_info.addr_bar.v_rect.bottom + 1;
    s_brw_show_info.web_page.v_rect.right = client_rect.v_rect.right - scroll_width + 1;
#ifdef BROWSER_TOOLBAR
    s_brw_show_info.web_page.v_rect.bottom = s_brw_show_info.toolbar.v_rect.top - 1;
#else
    s_brw_show_info.web_page.v_rect.bottom = client_rect.v_rect.bottom;
#endif

    //scroll
    s_brw_show_info.scroll.v_rect.right = client_rect.v_rect.right ;
    s_brw_show_info.scroll.v_rect.left = s_brw_show_info.scroll.v_rect.right - scroll_width + 1;
    s_brw_show_info.scroll.v_rect.top = s_brw_show_info.web_page.v_rect.top;
    s_brw_show_info.scroll.v_rect.bottom = s_brw_show_info.web_page.v_rect.bottom;
    //GUIAPICTRL_SetBothRect(MMIBROWSER_PRGBOX_CTRL_ID, &s_brw_show_info.scroll);

    //取connection animal的最大尺寸
    GUIRES_GetAnimWidthHeight(&img_width, &img_height, PNULL, IMAGE_BROWSER_CONNECTION_ANIM, win_id);
    GUIRES_GetAnimWidthHeight(&img2_width, &img2_height, PNULL, IMAGE_BROWSER_SSL_CONNECTION_ANIM, win_id);
    connect_anim_width = MAX(img_width, img2_width);
    connect_anim_height = MAX(img_height, img2_height);

    connect_anim_width = (0 == connect_anim_width)? 1: connect_anim_width;//for protect
    connect_anim_height = (0 == connect_anim_height)? 1: connect_anim_height;//for protect

    s_brw_show_info.connect_anim.v_rect.left = s_brw_show_info.addr_bar.v_rect.left + MMIBROWSER_ANIMA_RELATIVE_LEFT;
    s_brw_show_info.connect_anim.v_rect.top =(s_brw_show_info.addr_bar.v_rect.top + s_brw_show_info.addr_bar.v_rect.bottom - connect_anim_height)/2;
    s_brw_show_info.connect_anim.v_rect.right = s_brw_show_info.connect_anim.v_rect.left + connect_anim_width -1;
    s_brw_show_info.connect_anim.v_rect.bottom = s_brw_show_info.connect_anim.v_rect.top + connect_anim_height - 1;
    //GUIAPICTRL_SetBothRect(MMIBROWSER_ANIMATE_CONNECTING_CTRL_ID, &s_brw_show_info.connect_anim);

    //addr label
    GUIRES_GetImgWidthHeight(PNULL, &img_height, IMAGE_BROWSER_STARTPAGE_LABEL_BG, win_id);
    s_brw_show_info.addr_label.v_rect.left = s_brw_show_info.connect_anim.v_rect.right + MMIBROWSER_LABEL_RELATIVE_LEFT;
    s_brw_show_info.addr_label.v_rect.top = (s_brw_show_info.addr_bar.v_rect.top + s_brw_show_info.addr_bar.v_rect.bottom - img_height)/2;
    s_brw_show_info.addr_label.v_rect.right = s_brw_show_info.addr_bar.v_rect.right - MMIBROWSER_LABEL_RELATIVE_RIGHT;
    s_brw_show_info.addr_label.v_rect.bottom = s_brw_show_info.addr_label.v_rect.top + img_height -1;
    //GUIAPICTRL_SetBothRect(MMIBROWSER_LABEL_ADDRESS_CTRL_ID, &s_brw_show_info.addr_label);
}

/*****************************************************************************/
//  Description : for stop loading
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_StopLoading(void)
{ 
    if (MMIBROWSER_GetLoading())
    {
        if (BRW_StopPageReq(MMIBROWSER_GetInstance()))
        {
            MMIBROWSER_SetLoading(FALSE);
            MMIBROWSER_SetParamToStop();
        }
        else
        {
            //SCI_TRACE_LOW:"MMIBROWSER_StopLoading: BRW_StopPageReq error"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_7662_112_2_18_2_6_10_197,(uint8*)"");
        }
    }
}



/*****************************************************************************/
//  Description : Tidy all windows except Mainmenu window
//  Global resource dependence : 
//  Author:fen.xie
//  Note: MS00192555
/*****************************************************************************/
PUBLIC void MMIBROWSER_TidyExistedWin(void)
{
    MMI_WIN_ID_T win_id = MMI_BRW_BEGIN_WIN_ID;

    for (win_id = MMI_BRW_BEGIN_WIN_ID + 1; win_id <  MMIBROWSER_WINDOW_ID_NUMBER; win_id++)
    {
        if ((MMIBROWSER_MAIN_WIN_ID != win_id) 
            && (MMIBROWSER_START_PAGE_WIN_ID != win_id)
#ifdef BROWSER_START_PAGE_THEME1//the windows in slide array will be close in MMIBROWSER_START_PAGE_WIN_ID
            && (MMIBROWSER_BK_AND_HISTORY_WIN_ID != win_id)
            && (MMIBROWSER_FIX_HTML_WIN_ID != win_id)
            && (MMIBROWSER_HISTORYLIST_WIN_ID != win_id)
            && (MMIBROWSER_BOOKMARK_WIN_ID != win_id)
#endif
            )//main window don't need to close
        {
            MMK_CloseWin(win_id);
        }
    }
}


/*****************************************************************************/
//  Description : 设置access web的相关参数,UI显示
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_SetToAccess(
                    const char *url_ptr, //[IN]:access url
                    BOOLEAN is_update//[IN]:是否需要刷新当前的address bar
                    )
{
#ifndef BROWSER_TOOLBAR
    MMI_HANDLE_T    win_handle = MMIBROWSER_MAIN_WIN_ID;
#endif
    MMI_STRING_T    temp_str = {0};
    wchar   wurl_arr[MMIBROWSER_MAX_URL_LEN + 1] = {0};

    is_update = (MMIBROWSER_IsMainActive())? is_update: FALSE;
    
    //参数设置
    MMIBROWSER_SetIsSnapshot(FALSE);
    MMIBROWSER_SetLoading(TRUE);

    if (url_ptr != PNULL)
    { 
        CFLWCHAR_StrNCpyWithAsciiStr(wurl_arr, url_ptr, MMIBROWSER_MAX_URL_LEN + 1);
        temp_str.wstr_ptr = wurl_arr;
        temp_str.wstr_len = (uint16)CFLWCHAR_StrLen(wurl_arr);

        GUILABEL_SetText(MMIBROWSER_LABEL_ADDRESS_CTRL_ID, &temp_str, FALSE);
    }
    
    MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_ANIM, FALSE);
    
#ifdef BROWSER_TOOLBAR
    BrwChangeRefreshButtonImg(TRUE, is_update);
#else
    GUIWIN_SetSoftkeyTextId(win_handle, STXT_OPTION, TXT_NULL, STXT_STOP, is_update);
#endif

    if (is_update)//刷新
    {
        MMIBROWSER_DisplayAdressBar(0,0);
    }
}

/*****************************************************************************/
//  Description : 当stop web 的UI显示
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_SetToStop(void)
{
#ifndef BROWSER_TOOLBAR  
	MMI_TEXT_ID_T   rightsoft_id = TXT_NULL;
#endif
    BOOLEAN is_update = FALSE;
    uint8    *url_ptr = PNULL;
    wchar    *wurl_ptr = PNULL;
    BOOLEAN is_snapshot = FALSE;
    
    BRW_GetPageUrl(MMIBROWSER_GetInstance(), PNULL, 0, &is_snapshot, PNULL);    
    if (!is_snapshot)//new page is open, free the snapshot data
    {
        MMIBROWSER_FreeSnapshotFilename();
        MMIBROWSER_FreeSnapshotUrl();
    }

    //update the edit url
    MMIBROWSER_GetCurPageUrlInfo(&url_ptr ,PNULL);
    MMIBROWSER_UpdateUrl((char *)url_ptr);
    MMIBROWSER_FreeCurPageUrlInfo(&url_ptr ,PNULL);
    //update the address label
    MMIBROWSER_GetCurAddrLabelInfo(&wurl_ptr);
    MMIBROWSER_UpdateAddrLable(wurl_ptr, FALSE);
    MMIBROWSER_FreeCurAddrLabelInfo(&wurl_ptr);

    //for display
    is_update = (MMIBROWSER_IsMainActive())? TRUE: FALSE;

#ifdef BROWSER_TOOLBAR
    BrowserChangeButtonStatus(is_update);
    BrwChangeRefreshButtonImg(FALSE, is_update);
#else
    rightsoft_id = (BRW_NavHisIsBackwardEnable())? TXT_COMMON_COMMON_BACK: TXT_COMMON_CLOSE;
    GUIWIN_SetSoftkeyTextId(MMIBROWSER_MAIN_WIN_ID, STXT_OPTION, TXT_NULL, rightsoft_id, is_update);
#endif
    MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_STOP, FALSE);
    MMIBROWSER_DisplayAdressBar(0,0);
}

/*****************************************************************************/
//  Description : 当stop时,reset some params
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_SetParamToStop(void)
{
    MMIBROWSER_SetTotalMediaNum(0);
    MMIBROWSER_SetCurrMediaNum(0);
    MMIBROWSER_ClearRecvDataLen();
    MMIBROWSER_SetTotalDataLen(0);
}

/****begin:封装mmi pub query window的API的原因,是为了browser的重入时能及时关闭browser模块的所有窗口;
因此browser模块内使用query pub接口,必须调用以下几个;未封装MMIPUB_OpenQuery的所有api,需要可以再扩展****/
/*****************************************************************************/
//  Description : 打开browser的query window by text ptr
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenQueryWinByTextPtr(
                                         MMI_STRING_T               *text1_ptr,          //text 1,no default TXT_NULL
                                         MMI_STRING_T               *text2_ptr,          //text 1,no default TXT_NULL
                                         MMI_IMAGE_ID_T             image_id,           //alert image id
                                         MMI_WIN_ID_T               *win_id_ptr,  //win id must be in browser module,default is MMIBROWSER_QUERY_WIN_ID
                                         MMIPUB_HANDLE_FUNC         handle_func         //handle function
                                         )
{
    MMI_WIN_ID_T    query_win_id = MMIBROWSER_QUERY_WIN_ID;
    
    if (win_id_ptr != PNULL)
    {
        MMIPUB_OpenQueryWinByTextPtr(text1_ptr, text2_ptr, image_id, win_id_ptr,handle_func);
    }
    else
    {
        MMIPUB_OpenQueryWinByTextPtr(text1_ptr, text2_ptr, image_id, &query_win_id,handle_func);
    }
}

/*****************************************************************************/
//  Description :  打开browser的query window by text id
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenQueryWinByTextId(
                                        MMI_TEXT_ID_T              text_id,           //text 1,no default TXT_NULL
                                        MMI_IMAGE_ID_T             image_id,           //alert image id
                                        MMI_WIN_ID_T               *win_id_ptr,  //win id must be in browser module,default is MMIBROWSER_QUERY_WIN_ID
                                        MMIPUB_HANDLE_FUNC         handle_func         //handle function
                                        )
{
    MMI_WIN_ID_T    query_win_id = MMIBROWSER_QUERY_WIN_ID;
    if (win_id_ptr != PNULL)
    {
        MMIPUB_OpenQueryWinByTextId(text_id,image_id,win_id_ptr,handle_func);
    }
    else
    {
        MMIPUB_OpenQueryWinByTextId(text_id,image_id,&query_win_id,handle_func);
    }
}

/*****************************************************************************/
//  Description : close browser query windows
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_CloseQuerytWin(
                                         MMI_WIN_ID_T               *win_id_ptr  //win id must be in browser module,default is MMIBROWSER_QUERY_WIN_ID
                                         )
{
    MMI_WIN_ID_T    query_win_id = MMIBROWSER_QUERY_WIN_ID;
    if (win_id_ptr != PNULL)
    {
        MMIPUB_CloseQuerytWin(win_id_ptr);
    }
    else
    {
        MMIPUB_CloseQuerytWin(&query_win_id);
    }
}
/****end:封装mmi pub query window的API的原因,是为了browser的重入时能及时关闭browser模块的所有窗口;
因此browser模块内使用query pub接口,必须调用以上几个;未封装MMIPUB_OpenQuery的所有api,需要可以再扩展****/

/*****************************************************************************/
//  Description : start tp slide timer
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void BrowserStartTpSlideTimer(MMI_WIN_ID_T win_id)
{
    //include progress bar
    if (0 == s_browser_tp_info.slide_timer_id)
    {
        s_browser_tp_info.slide_timer_id = MMK_CreateWinTimer(win_id,
                                                SLIDE_TIME,//100
                                                FALSE);
    }
}

/*****************************************************************************/
//  Description : stop tp slide timer
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void BrowserStopTpSlideTimer(MMI_WIN_ID_T	win_id)
{
    if (0 < s_browser_tp_info.slide_timer_id)
    {
        MMK_StopTimer(s_browser_tp_info.slide_timer_id);
        s_browser_tp_info.slide_timer_id = 0;
    }
}
/*****************************************************************************/
//  Description : handle slide
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void HandleSlide(
                           GUI_POINT_T      tp_point,
                           MMI_WIN_ID_T	win_id,
                           BRW_INSTANCE brw_instance
                           )
{
    BOOLEAN     is_reset = FALSE;
    int32       scroll_step = 0;
    int32       brw_scroll_step = 0;

    switch (s_browser_tp_info.slide_state)
    {
    case MMK_TP_SLIDE_TP_SCROLL:
        scroll_step = s_browser_tp_info.prev_point.y - tp_point.y;
        BRW_ProcTpScrollEvent(brw_instance, scroll_step);
        UILAYER_SetDirectDraw(TRUE);
        break;

    case MMK_TP_SLIDE_FLING:
        //get fling offset
        scroll_step = MMK_GetFlingOffset(0,s_browser_tp_info.fling_velocity,(float)FLING_TIME,&s_browser_tp_info.fling_velocity);
        brw_scroll_step= -scroll_step;
        
        if (0 == s_browser_tp_info.fling_velocity)//stop flig
        {
            //SCI_TRACE_LOW:"[BRW]HandleSlide 1"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_7919_112_2_18_2_6_11_198,(uint8*)"");
            //stop slide timer
            BrowserStopTpSlideTimer(win_id);

            //reset slide state
            s_browser_tp_info.slide_state = MMK_TP_SLIDE_NONE;

            is_reset = TRUE;
        }
        else
        {
            //SCI_TRACE_LOW:"[BRW]HandleSlide 2"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_7930_112_2_18_2_6_11_199,(uint8*)"");

            if (!BRW_ProcTpScrollEvent(brw_instance, brw_scroll_step))
            {
                //up to top or down to bottom,end fling
                //stop slide timer
                BrowserStopTpSlideTimer(win_id);

                //reset slide state
                s_browser_tp_info.slide_state = MMK_TP_SLIDE_NONE;

                is_reset = TRUE;
            }
            else
            {
                //continue to fling
                UILAYER_SetDirectDraw(TRUE);
            }
        }
        break;

    default:
        break;
    }

}

/*****************************************************************************/
//  Description : handle slide timer
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
LOCAL void HandleSlideTimer(MMI_WIN_ID_T	win_id, BRW_INSTANCE brw_instance)
{
    GUI_POINT_T         tp_point = {0};
    MMI_TP_STATUS_E     tp_status = MMI_TP_NONE;

    //stop slide timer
    BrowserStopTpSlideTimer(win_id);

    if (MMK_IsFocusWin(win_id))
    {
        //start tp slide timer
        BrowserStartTpSlideTimer(win_id);
        MMK_GetLogicTPMsg(&tp_status,&tp_point);
        if (MMK_TP_SLIDE_NONE != s_browser_tp_info.slide_state)
        {
            //handle slide
            if ((!s_browser_tp_info.is_tp_down) || 
                (abs(s_browser_tp_info.prev_point.y - tp_point.y) > 3))
            {
                HandleSlide(tp_point, win_id,brw_instance);
                s_browser_tp_info.prev_point = tp_point;
            }
        }
    }
}

/*****************************************************************************/
//  Description : handle MSG_TP_PRESS_MOVE\MSG_TP_PRESS_DOWN\MSG_TP_PRESS_UP for browser main window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void MMIBROWSER_HandleTpPress(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param,
                                   BRW_INSTANCE brw_instance
                                   )
{
    char    *url_ptr = PNULL;
    GUI_POINT_T point = {0};
    BOOLEAN is_need_reset_slide = FALSE;
    BOOLEAN is_fixed_instance = FALSE;
    BOOLEAN is_point_in_webpage = FALSE;

    point.x = MMK_GET_TP_X(param);
    point.y = MMK_GET_TP_Y(param);

    is_fixed_instance = (MMIBROWSER_GetFixedInstance() == brw_instance)? TRUE: FALSE;
    if (!is_fixed_instance)
    {
        is_point_in_webpage = BrwIsTouchInWebPage(point);
    }
    else
    {
        is_point_in_webpage = GUI_PointIsInRect(point, MMITHEME_GetClientRectEx(win_id));
    }

    switch(msg_id)
    {
    case MSG_TP_PRESS_DOWN:
        if (!is_fixed_instance && BrwIsTouchAddressBar(point))//touch web page
        {
            //touched addressbar, entry edit url window
            url_ptr = MMIBROWSER_GetUrl();
            MMIBROWSER_InputUrl((uint8*)url_ptr);
            is_need_reset_slide = TRUE;
        }
        else
        {            
            if (is_point_in_webpage && (brw_instance != BRW_INVALID_INSTANCE))
            {
                if (!BRW_ProcPenDownEvent(brw_instance,(BRW_PEN_CLICK_INFO_T*)&point))
                {
                    //SCI_TRACE_LOW:"MMIBROWSER MMIBROWSER_HandleTpPress BRW_ProcPenDownEvent error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8033_112_2_18_2_6_11_200,(uint8*)"");
                }
                //is tp down
                s_browser_tp_info.is_tp_down = TRUE;
                //set start and previous point
                s_browser_tp_info.start_point = point;
                s_browser_tp_info.prev_point  = point;

                //set tp down slide state
                s_browser_tp_info.tpdown_slide_state = s_browser_tp_info.slide_state;

                //add velocity item
                MMK_AddVelocityItem(point.x,point.y);

                 if (MMK_TP_SLIDE_FLING != s_browser_tp_info.slide_state)
                {
                    //start tp slide timer
                    BrowserStartTpSlideTimer(win_id);
                }
            }
            else
            {
                is_need_reset_slide = TRUE;
            }
        }

        if (is_need_reset_slide)
        {
            //stop slide timer
            BrowserStopTpSlideTimer(win_id);
            //reset velocity item
            MMK_ResetVelocityItem();
            s_browser_tp_info.fling_velocity = 0;
            //reset slide state
            s_browser_tp_info.slide_state = MMK_TP_SLIDE_NONE;
            SCI_MEMSET(&s_browser_tp_info, 0x00, sizeof(s_browser_tp_info));
        }
        break;
       
    case MSG_TP_PRESS_UP:
    {
        GUI_POINT_T     tp_point = {0};
        
         //is tp down
        s_browser_tp_info.is_tp_down = FALSE;
        //stop slide timer
        BrowserStopTpSlideTimer(win_id);

        if (!MMK_GET_TP_SLIDE(param)) //not slide
        {
            //set tp press point
            tp_point.x = MMK_GET_TP_X(param);
            tp_point.y = MMK_GET_TP_Y(param);

            if ((MMK_TP_SLIDE_NONE == s_browser_tp_info.slide_state) && is_point_in_webpage && (brw_instance!=BRW_INVALID_INSTANCE))
            {
                if (!BRW_ProcPenUpEvent(brw_instance,(BRW_PEN_CLICK_INFO_T*)&point))//whether click the focus
                {
                    //SCI_TRACE_LOW:"[BRW]MMIBROWSER_HandleTpPress BRW_ProcPenUpEvent error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8091_112_2_18_2_6_11_201,(uint8*)"");
                }
            }

            if (MMK_TP_SLIDE_NONE != s_browser_tp_info.slide_state)
            {
                //text is slide
                ((MMI_MULTI_KEY_TP_MSG_PARAM_T *)(param))->is_slide = TRUE;

                //add velocity item
                MMK_AddVelocityItem(tp_point.x,tp_point.y);

                //calculate current velocity
                MMK_ComputeCurrentVelocity(PNULL,&s_browser_tp_info.fling_velocity, FLING_0_VELOCITY, (float)FLING_MAX_VELOCITY);
                
                //SCI_TRACE_LOW:"[BRW]MMIBROWSER_HandleTpPress fling_velocity =%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8106_112_2_18_2_6_11_202,(uint8*)"d", abs((int32)s_browser_tp_info.fling_velocity));

                //fling velocity
                if (MMIBROWSER_FLING_MIN_VELOCITY > abs((int32)s_browser_tp_info.fling_velocity))
                {
                    //reset velocity item
                    MMK_ResetVelocityItem();
                    s_browser_tp_info.fling_velocity = 0;

                    //reset slide state
                    s_browser_tp_info.slide_state = MMK_TP_SLIDE_NONE;
                }
                else
                {
                    //enter fling state
                    s_browser_tp_info.slide_state = MMK_TP_SLIDE_FLING;

                    //start tp slide timer
                    BrowserStartTpSlideTimer(win_id);
                }
            
            }
        }
    }
        break;

    case MSG_TP_PRESS_MOVE:
        if (s_browser_tp_info.is_tp_down //means for tp slide state,begin with the MSG_TP_PRESS_DOWN
            && is_point_in_webpage)//touch web page
        {
            if (abs(point.y - s_browser_tp_info.start_point.y) > SLIDE_MIN_SPACE)
            {
                //set slide state
                if ((MMK_TP_SLIDE_NONE == s_browser_tp_info.tpdown_slide_state) ||
                    (MMK_IsWayChanged(s_browser_tp_info.fling_velocity, &s_browser_tp_info.start_point, &point, FALSE)))
                {
                    s_browser_tp_info.slide_state = MMK_TP_SLIDE_TP_SCROLL;
                }
            }

            //add velocity item
            MMK_AddVelocityItem(point.x,point.y);
        }
        break;
        
    default:
        break;
    }
}
#if 0
/*****************************************************************************/
//  Description : handle MSG_TP_PRESS_MOVE\MSG_TP_PRESS_DOWN\MSG_TP_PRESS_UP for browser main window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void MMIBROWSER_HandleTpPress(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    BRW_INSTANCE    brw_instance = 0;
    char    *url_ptr = PNULL;
    GUI_POINT_T point = {0};
    LOCAL   BOOLEAN s_is_handle_move = FALSE;//当MSG_TP_PRESS_DOWN时的point在webpage范围内时
    LOCAL   BOOLEAN s_is_moving = FALSE;//当前是否正处于moving的处理过程中
    LOCAL int16 s_move_x = 0;
    LOCAL int16 s_move_y = 0;
    int16 by_x = 0;
    int16 by_y = 0;
    
    point.x = MMK_GET_TP_X(param);
    point.y = MMK_GET_TP_Y(param);
    brw_instance = MMIBROWSER_GetInstance();

    //SCI_TRACE_LOW:"MMIBROWSER_HandleTpPress s_is_handle_move=%d, s_is_moving=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8180_112_2_18_2_6_11_203,(uint8*)"dd", s_is_handle_move, s_is_moving);
    //SCI_TRACE_LOW:"MMIBROWSER_HandleTpPress s_move_x=%d, s_move_y=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8181_112_2_18_2_6_11_204,(uint8*)"dd", s_move_x, s_move_y);

    if ((MSG_TP_PRESS_DOWN == msg_id) && BrwIsTouchAddressBar(point))
    {
        //touched addressbar, entry edit url window
        url_ptr = MMIBROWSER_GetUrl();
        MMIBROWSER_InputUrl((uint8*)url_ptr);
    }
    else
    {
        switch(msg_id)
        {
        case MSG_TP_PRESS_DOWN:
            //touch web page
            if (BrwIsTouchInWebPage(point) && (brw_instance!=BRW_INVALID_INSTANCE))
            {
                if (!BRW_ProcPenDownEvent(brw_instance,(BRW_PEN_CLICK_INFO_T*)&point))
                {
                    //SCI_TRACE_LOW:"MMIBROWSER MMIBROWSER_HandleTpPress BRW_ProcPenDownEvent error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8199_112_2_18_2_6_11_205,(uint8*)"");
                }
                //init move
                s_is_handle_move = TRUE;
                s_move_x = point.x;
                s_move_y = point.y;
            }
            else//reset move
            {
                s_is_handle_move = FALSE;
                s_move_x = 0;
                s_move_y = 0;                
            }
            s_is_moving = FALSE;
            break;
           
        case MSG_TP_PRESS_UP:
            if (!s_is_moving && BrwIsTouchInWebPage(point) && (brw_instance!=BRW_INVALID_INSTANCE))
            {
                if (!BRW_ProcPenUpEvent(brw_instance,(BRW_PEN_CLICK_INFO_T*)&point))
                {
                    //SCI_TRACE_LOW:"MMIBROWSER MMIBROWSER_HandleTpPress BRW_ProcPenUpEvent error"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8220_112_2_18_2_6_11_206,(uint8*)"");
                }
            }
            //reset is_move
            s_is_handle_move = FALSE;
            s_is_moving = FALSE;
            s_move_x = 0;
            s_move_y = 0;                
            break;

        case MSG_TP_PRESS_MOVE:
            if (s_is_handle_move && BrwIsTouchInWebPage(point) && (brw_instance!=BRW_INVALID_INSTANCE))//touch web page
            {
                by_x = s_move_x - point.x;
                by_y = s_move_y - point.y;
                if (abs(by_y) > 5)
                {
                    s_is_moving = TRUE;
                    BRW_ProcTpScrollEvent(brw_instance, by_y);
                    //s_move_x = point.x;
                    //s_move_y = point.y;
                }
            }
            break;
            
        default:
            break;
        }
    }
}
#endif
/*****************************************************************************/
//  Description : stop loading and enter a new valid url to browser;if url is invalid,alert ......
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIBROWSER_EnterNewUrl(uint8* url_ptr, //[IN]new url buf
                                BOOLEAN is_need_check_url//[IN]TRUE:需判断url有效性
                                )
{
     
    if (is_need_check_url && Brw_IsIncludeInvalidUrl(url_ptr))
    {
        //alert the url is invalid
        MMIBROWSER_AlertUrlInvalid();
        return FALSE;
    }
    else
    {
        if (PNULL != url_ptr)
        {
            MMIBROWSER_StopLoading();
            MMK_PostMsg(MMIBROWSER_MAIN_WIN_ID, MSG_BRW_ENTER_URL, url_ptr, strlen((char *)url_ptr)+1);
        }
        else
        {
            //SCI_TRACE_LOW:"[BRW]W MMIBROWSER_EnterNewUrl url_ptr PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8275_112_2_18_2_6_11_207,(uint8*)"");
        }
        return TRUE;
    }
    
}

/*****************************************************************************/
//  Description : 停止下载后设置相应的参数
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_SetParaForStopDL(void)
{
    MMIBROWSER_UpdateReqHandle(0);
    MMIBROWSER_SetDownloadMedia(FALSE);
    MMIBROWSER_SetBeginDownload(FALSE);
    MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_NONE);
    MMIBROWSER_ClearRecvDataLen();
    MMIBROWSER_SetTotalDataLen(0);
    MMIBROWSER_UpdateFileNameReq(PNULL);
    MMIBROWSER_SetLoading(FALSE);
}

/*****************************************************************************/
//  Description : 停止下载文件
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_SetToStopDL(void)
{
    MMIBROWSER_SetParaForStopDL();
    MMIBROWSER_SetToStop();//MS00241440
}

/*****************************************************************************/
//  Description : 准备开始下载:得到文件全路径...
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMIBROWSER_PREPARE_DL_ERR_E BrwPrepareToDL(void)
{
    BRW_MSG_ENTER_FILENM_REQ_T  *file_name_req_ptr = PNULL;
    wchar   full_path_name[MMIFILE_FULL_PATH_MAX_LEN + 1] = {0};
    uint16  full_path_name_len = MMIFILE_FULL_PATH_MAX_LEN;
    wchar   file_name[MMIBROWSER_DL_FILE_FULL_PATH_MAX_LEN + 1] = {0};
    uint32  data_len = 0;
    MMI_CTRL_ID_T path_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_PATH_LABEL_CTRL_ID; 
    MMIFILE_DEVICE_E device_type = MMI_DEVICE_NUM;
    BRW_MIME_TYPE_E mime_type = BRW_MIME_UNKOWN;
    MMI_CTRL_ID_T dropdown_path_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_PATH_DROPDOWNLIST_CTRL_ID;
    BOOLEAN is_only_storage = FALSE;
    uint32  dev_space = 0;//NEWMS00167822

    SCI_TRACE_LOW("[BRW]Dorado download:begin = %d",  SCI_GetTickCount());

    file_name_req_ptr = MMIBROWSER_GetFileNameReq();
    if (PNULL == file_name_req_ptr)
    {
        //SCI_TRACE_LOW:"[BRW] BrwPrepareToDL error file_name_req_ptr = PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8329_112_2_18_2_6_12_208,(uint8*)"");
        return PREPARE_DL_ERR_OTHER;
    }
    
    mime_type = file_name_req_ptr->mime_type;

    is_only_storage = MMIBROWSER_IsOnlyChooseStorage(mime_type);

    if (is_only_storage)//get the select device
    {
        uint16 index = 0;
        uint16  i = 0;
    
#ifdef MMI_PDA_SUPPORT
        index = GUISETLIST_GetCurIndex(dropdown_path_ctrl_id);
#else
        if (GUIDROPDOWNLIST_GetTotalItemNum(dropdown_path_ctrl_id) > 1)
        {
            index = GUIDROPDOWNLIST_GetCurItemIndex(dropdown_path_ctrl_id);
        }
#endif /* MMI_PDA_SUPPORT */

        if(index < MMI_DEVICE_NUM)
        {
            for(device_type=MMI_DEVICE_UDISK; device_type<MMI_DEVICE_NUM; device_type++)
            {
                if(MMIAPIFMM_GetDeviceTypeStatus(device_type))
                {
                    if (i == index)
                    {
                        MMIBROWSER_SetSaveDeviceType(device_type);
                        break;
                    }
                    i++;
                }
            }
        }
    }

    data_len = file_name_req_ptr->content_len;//MMIBROWSER_GetTotalDataLen();
    
    device_type = MMIBROWSER_GetSaveDeviceType();
    dev_space = MMI_GetFreeSpace(device_type);

    if ((0 == dev_space) || (dev_space < (data_len >> 10)))//NEWMS00167822
    {
        return PREPARE_DL_ERR_FULL_NO_STORE;
    }
    else if (device_type >= MMI_DEVICE_NUM)
    {
        return PREPARE_DL_ERR_OTHER;
    }
       
    //generate file name
    if ((0 != MMIAPICOM_Wstrlen(file_name_req_ptr->res_name_arr) )
        && (PNULL != file_name_req_ptr->enter_filenm_cbf)
        && MMIAPICOM_GetValidFileName(FALSE,
			file_name_req_ptr->res_name_arr,MMIAPICOM_Wstrlen(file_name_req_ptr->res_name_arr),
			file_name,MMIBROWSER_DL_FILE_FULL_PATH_MAX_LEN))//MS00194888
    {    
        //set only storage for some special media file
        if (is_only_storage)
        {
            wchar   *dir_ptr=PNULL;
            uint16  dir_len=0;

            BrowserDlGetDefaultMediaDir((uint16 **)&dir_ptr, &dir_len, mime_type);
            MMIAPIFMM_CombineFullPath(
                    MMIAPIFMM_GetDevicePath(device_type), MMIAPIFMM_GetDevicePathLen(device_type),
                    dir_ptr, dir_len,
                    file_name, MMIAPICOM_Wstrlen(file_name), 
                    full_path_name,&full_path_name_len
                    );
        }
        else
        {
            wchar   dir_name[MMIFILE_DIR_NAME_MAX_LEN + 1] = {0};
            uint16  dir_name_len = MMIFILE_DIR_NAME_MAX_LEN;
            MMI_STRING_T path_display_str = {0};
            
            GUILABEL_GetText(path_ctrl_id, &path_display_str);
            MMIBROWSER_SplitPrePath(path_display_str.wstr_ptr, path_display_str.wstr_len, 
                PNULL, PNULL,
                dir_name, &dir_name_len
                );

            MMIAPIFMM_CombineFullPath(
                    MMIAPIFMM_GetDevicePath(device_type), MMIAPIFMM_GetDevicePathLen(device_type),
                    dir_name, dir_name_len,
                    file_name, MMIAPICOM_Wstrlen(file_name), 
                    full_path_name,&full_path_name_len
                    );
        }

        //check if the file is exist
        if (MMIAPIFMM_IsFileExist(full_path_name, full_path_name_len))
        {
            return PREPARE_DL_ERR_EXSIT;//fenxie
        }
        else if (BRW_DL_TYPE_LOCAL_CONTENT == file_name_req_ptr->dl_type)
        {
            file_name_req_ptr->enter_filenm_cbf(file_name_req_ptr->handle, full_path_name, MMIAPICOM_Wstrlen(full_path_name), TRUE);
        }
        else
        {
            switch (mime_type)
            {
            case BRW_MIME_JAD:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_JAD);
                break;
                
            case BRW_MIME_JAR:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_JAR);
                break;
                
            case BRW_MIME_SDP:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_SDP);
                break;

#ifdef DRM_SUPPORT    
            case BRW_MIME_OMA_DRM_MESSAGE:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_DM);
                break;
                
            case BRW_MIME_OMA_DRM_CONTENT:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_DCF);
                break;
                
            case BRW_MIME_OMA_DRM_RIGHTS_XML:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_DR);
               break;
               
            case BRW_MIME_OMA_DRM_RIGHTS_WBXML:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_DRC);
                break;
#endif
#ifdef QBTHEME_SUPPORT
            case BRW_MIME_QBT:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_QBT);
                break;
#endif
#ifdef HOME_LOCATION_INQUIRE_SUPPORT_TCARD
            case BRW_MIME_PNL:
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_PNL);
                break;
#endif
#ifdef DATA_ACCOUNT_IN_TCARD_SUPPORT
            case BRW_MIME_NTAC:  //network account
                MMIBROWSER_SetMimeType(MMIBROWSER_MIME_TYPE_NTAC);
                break;
#endif
            default:
                break;
            }

#ifdef OMADL_SUPPORT//the file is managed by OMADL in DAPS
            if (BRW_DL_TYPE_OMA_DD != file_name_req_ptr->dl_type) 
#endif    
            {
                MMIBROWSER_UpdateFileName(full_path_name);//MS00233946 save the file name
            }

            file_name_req_ptr->enter_filenm_cbf(file_name_req_ptr->handle, full_path_name, MMIAPICOM_Wstrlen(full_path_name), TRUE);
            
        }
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW] BrwPrepareToDL generate file name fail"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8512_112_2_18_2_6_12_209,(uint8*)"");
        return PREPARE_DL_ERR_OTHER;
    }
    
    return PREPARE_DL_ERR_NONE;
}

/*****************************************************************************/
//  Description : set control info for download media file window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetDownloadMediaFileWindow(void)
{
        MMI_STRING_T loading_str = {0};
        MMI_STRING_T name_str = {0};
//         MMI_STRING_T progress_str = {0};
        BRW_MSG_ENTER_FILENM_REQ_T* name_req_ptr = PNULL;

        MMI_GetLabelTextByLang(TXT_COMMON_DOWNLOADING_WAIT, &loading_str); 
        name_req_ptr = MMIBROWSER_GetFileNameReq();
        name_str.wstr_len = MMIAPICOM_Wstrlen(name_req_ptr->res_name_arr);
        name_str.wstr_ptr = name_req_ptr->res_name_arr;
        GUILABEL_SetText(MMIBROWSER_DOWNLOAD_LABEL1_CTRL_ID, &loading_str, FALSE);
        GUILABEL_SetText(MMIBROWSER_DOWNLOAD_LABEL2_CTRL_ID, &name_str, FALSE);
        BrwSetProgressForDL();
}
/*****************************************************************************/
//  Description : set downloading process label
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void BrwSetProgressForDL(void)
{
    MMI_STRING_T progress_str = {0};
    MMI_STRING_T unknown_str = {0};
    char  progress_temp[MMIBROWSER_PROGRESS_BUF_LEN] = {0};
    wchar progress_temp_wchar[MMIBROWSER_PROGRESS_BUF_LEN] = {0};
    uint32 receive_data_len = 0;
    uint32 total_data_len = 0;

    receive_data_len = MMIBROWSER_GetRecvDataLen();
    total_data_len = MMIBROWSER_GetTotalDataLen();
    if ((0 != total_data_len) && (receive_data_len > total_data_len))
    {
        receive_data_len = total_data_len;
    }
    if (receive_data_len/MMIBROWSER_ONE_K > 0)
    {
        CFL_Snprintf(progress_temp,sizeof(progress_temp),"%.1f%s",(float)(receive_data_len)/MMIBROWSER_ONE_K,"KB/");
    }
    else
    {
        CFL_Snprintf(progress_temp,sizeof(progress_temp),"%d%s",receive_data_len,"B/");
    }
    
    if (0 == total_data_len)
    {
        CFLWCHAR_StrNCpyWithAsciiStr(progress_temp_wchar,(const char*)progress_temp,MMIBROWSER_PROGRESS_BUF_LEN);
        MMI_GetLabelTextByLang(TXT_BROWSER_FILE_SIZE_UNKNOWN, &unknown_str);
        MMIAPICOM_Wstrncpy(progress_temp_wchar+MMIAPICOM_Wstrlen(progress_temp_wchar),unknown_str.wstr_ptr,unknown_str.wstr_len);
    }
    else
    {
        if (total_data_len/MMIBROWSER_ONE_K > 0)
        {
            CFL_Snprintf(progress_temp+strlen(progress_temp),sizeof(progress_temp)-strlen(progress_temp),"%.1f%s",(float)(total_data_len)/MMIBROWSER_ONE_K,"KB");
        }
        else
        {
            CFL_Snprintf(progress_temp+strlen(progress_temp),sizeof(progress_temp)-strlen(progress_temp),"%d%s",total_data_len,"B");
        }
        
        CFLWCHAR_StrNCpyWithAsciiStr(progress_temp_wchar,(const char*)progress_temp,MMIBROWSER_PROGRESS_BUF_LEN);
    }
    progress_str.wstr_len = MMIAPICOM_Wstrlen(progress_temp_wchar);
    progress_str.wstr_ptr = progress_temp_wchar;

    GUILABEL_SetText(MMIBROWSER_DOWNLOAD_LABEL3_CTRL_ID, &progress_str, FALSE);
}

/*****************************************************************************/
//  Description : 提示URL无效
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
PUBLIC void MMIBROWSER_AlertUrlInvalid(void)
{
    MMIPUB_OpenAlertWarningWin(TXT_COMMON_URL_INVALID);
}


/*****************************************************************************/
//  Description : 删除未下载完毕(下载失败或用户取消)的目标文件
//  Global resource dependence : 
//  Author: fen.xie
//  Note:MS00233946
/*****************************************************************************/
PUBLIC void MMIBROWSER_DelInvalidDlFile(void)
{
    wchar   *file_name_ptr =PNULL;
    uint32  file_name_len = 0;
    BOOLEAN result = FALSE;

    //SCI_TRACE_LOW:"[BRW] MMIBROWSER_DelInvalidDlFile"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8611_112_2_18_2_6_12_210,(uint8*)"");
    
    file_name_ptr = MMIBROWSER_GetFileName();

    if ((file_name_ptr != PNULL) && ((file_name_len = MMIAPICOM_Wstrlen(file_name_ptr)) > 0))
    {
        result = MMIAPIFMM_DeleteFileSyn(file_name_ptr, file_name_len);
        //SCI_TRACE_LOW:"[BRW] MMIBROWSER_DelInvalidDlFile result = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8618_112_2_18_2_6_12_211,(uint8*)"d", result);
        MMIBROWSER_UpdateFileName(PNULL);//clean the data,it means the download is complete
    }
}

/*****************************************************************************/
//  Description : stop download
//  Global resource dependence : 
//  Author: fen.xie
//  Note:
/*****************************************************************************/
PUBLIC void MMIBROWSER_StopDownload(void)
{
    if (MMIBROWSER_GetDownloadMedia())//downloading
    {
        if (BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle()))
        {
            //SCI_TRACE_LOW:"[BRW]MMIBROWSER_StopDownload SUCC"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8633_112_2_18_2_6_12_212,(uint8*)"");
        }
        else
        {
            //SCI_TRACE_LOW:"[BRW]MMIBROWSER_StopDownload FAIL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8637_112_2_18_2_6_12_213,(uint8*)"");
        }
        MMIBROWSER_SetParaForStopDL();
        MMK_CloseWin(MMIBROWSER_SAVE_MEDIAFILE_WIN_ID);
        MMIPUB_CloseWaitWin(MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_ID);
    }
}

/*****************************************************************************/
//  Description :sms export setting location win handle fun
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  BrwHandleSaveMediaFileWinMsg(
                                                 MMI_WIN_ID_T win_id, 
                                                 MMI_MESSAGE_ID_E msg_id, 
                                                 DPARAM param
                                                 )
{
    MMI_RESULT_E err_code                = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        SetDownloadFileNamePathWindow(win_id);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID);
        break;
        
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
#ifdef MMI_PDA_SUPPORT
        {
            MMI_CTRL_ID_T       id = 0;

            if (PNULL != param)
            {
		        id = ((MMI_NOTIFY_T*) param)->src_id;

                if (id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                    break;
                }
                else if (id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                    break;
                }
            }    
        }
#endif /* MMI_PDA_SUPPORT */
    {
        MMI_STRING_T path_str = {0};
        wchar path[MMIFILE_FULL_PATH_MAX_LEN + 1] = {0};
        MMI_CTRL_ID_T active_ctrl_id = 0;
        MMI_STRING_T path_display_str = {0};
        
        active_ctrl_id = MMK_GetActiveCtrlId(win_id);
        
        switch (active_ctrl_id)
        {
        case MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID:
            MMK_CreatePubEditWin((uint32*)MMIBROWSER_INPUT_FILE_NAME_WIN_TAB,PNULL);
            break;
            
        case MMIBROWSER_SAVE_MEDIAFILE_PATH_LABEL_CTRL_ID:
            GUILABEL_GetText(MMIBROWSER_SAVE_MEDIAFILE_PATH_LABEL_CTRL_ID, &path_display_str);
            path_str.wstr_ptr = path;
            path_str.wstr_len = MMIFILE_FULL_PATH_MAX_LEN;
            MMIBROWSER_DisplayToFmmPath(&path_display_str, &path_str);
            MMIBROWSER_AppointSaveMediaFilePath(path_str.wstr_ptr, path_str.wstr_len, BrwOpenSetMediaFileNameBack);
            break;
            
        default:
            break;
        }
    }
        break;
        
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
        MMIBROWSER_PREPARE_DL_ERR_E dl_error_e = PREPARE_DL_ERR_NONE;
        BRW_MSG_ENTER_FILENM_REQ_T  *file_name_req_ptr = PNULL;
        BRW_DOWNLOADING_TYPE_E dl_type = BRW_DL_TYPE_MAX;

        file_name_req_ptr = MMIBROWSER_GetFileNameReq();
        if (PNULL != file_name_req_ptr)
        {
            dl_type = file_name_req_ptr->dl_type;
        }

        //begin to download
        dl_error_e = BrwPrepareToDL();
        if (dl_error_e != PREPARE_DL_ERR_NONE)
        {
            SCI_TRACE_LOW("[BRW]Dorado download:end fail = %d",  SCI_GetTickCount());
            SCI_TRACE_LOW("[BRW]Dorado download:end fail_reseason  = %d",  dl_error_e);
        }
        switch (dl_error_e)
        {
        case PREPARE_DL_ERR_NONE:
            if (BRW_DL_TYPE_LOCAL_CONTENT != dl_type)
            {
                //open download media file window and draw progress
                MMK_CreateWin((uint32 *)MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_TAB, PNULL);
                MMIBROWSER_SetBeginDownload(TRUE); 
            }
            else
            {
                //do nothing(handle in callback)
            }
            MMK_CloseWin(win_id);
            break;
                    
        case PREPARE_DL_ERR_FULL_NO_STORE:
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_FULL_NO_STORE);
            break;
            
        case PREPARE_DL_ERR_EXSIT:
            MMIPUB_OpenAlertWarningWin(TXT_FILE_EXISTED);//MS00191252
            break;
            
        case PREPARE_DL_ERR_OTHER:
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
            if (BRW_DL_TYPE_LOCAL_CONTENT != dl_type)
            {
                BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle());
                MMIBROWSER_SetToStopDL();
            }
            else
            {
                MMIBROWSER_UpdateFileNameReq(PNULL);
            }
            MMK_CloseWin(win_id);
            break;
            
        default:
            break;
        }
    }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
    {
        BRW_MSG_ENTER_FILENM_REQ_T  *file_name_req_ptr = PNULL;
        BRW_DOWNLOADING_TYPE_E dl_type = BRW_DL_TYPE_MAX;

        file_name_req_ptr = MMIBROWSER_GetFileNameReq();
        if (PNULL != file_name_req_ptr)
        {
            dl_type = file_name_req_ptr->dl_type;
        }

        if (BRW_DL_TYPE_LOCAL_CONTENT != dl_type)
        {
            //stop download
            if (BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle()))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleDownladMediaFileWinMsg stop dl SUCC"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8792_112_2_18_2_6_13_214,(uint8*)"");
            }
            else
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleDownladMediaFileWinMsg stop dl FAIL"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8796_112_2_18_2_6_13_215,(uint8*)"");
            }
            MMIBROWSER_SetToStopDL();
        }
        else
        {
            MMIBROWSER_UpdateFileNameReq(PNULL);
        }
        MMK_CloseWin(win_id);
    }
        break;
    default :
        err_code = MMI_RESULT_FALSE;
        break;
    } 
    return err_code;
}

/*****************************************************************************/
//  Description :sms export setting location win handle fun
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  BrwHandleSaveMediaFileOnlyStorageWinMsg(
                                                 MMI_WIN_ID_T win_id, 
                                                 MMI_MESSAGE_ID_E msg_id, 
                                                 DPARAM param
                                                 )
{
    MMI_RESULT_E err_code                = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        SetDownloadFileNamePathWindow(win_id);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID);
        break;

    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
#ifdef MMI_PDA_SUPPORT
        {
            MMI_CTRL_ID_T       id = 0;

            if (PNULL != param)
            {
		        id = ((MMI_NOTIFY_T*) param)->src_id;

                if (id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                    break;
                }
                else if (id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                    break;
                }
            }    
        }
#endif /* MMI_PDA_SUPPORT */
        MMK_CreatePubEditWin((uint32*)MMIBROWSER_INPUT_FILE_NAME_WIN_TAB,PNULL);
        break;
        
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
        MMIBROWSER_PREPARE_DL_ERR_E dl_error_e = PREPARE_DL_ERR_NONE;
        
        //begin to download
        dl_error_e = BrwPrepareToDL();
        if (dl_error_e != PREPARE_DL_ERR_NONE)
        {
            SCI_TRACE_LOW("[BRW]Dorado download:end fail = %d",  SCI_GetTickCount());
            SCI_TRACE_LOW("[BRW]Dorado download:end fail_reseason = %d",  dl_error_e);
        }
        switch (dl_error_e)
        {
        case PREPARE_DL_ERR_NONE:
            //open download media file window and draw progress
            MMK_CreateWin((uint32 *)MMIBROWSER_DOWNLOAD_MEDIA_FILE_WIN_TAB, PNULL);
            MMIBROWSER_SetBeginDownload(TRUE); 
            MMK_CloseWin(win_id);
            break;
                    
        case PREPARE_DL_ERR_FULL_NO_STORE:
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_FULL_NO_STORE);
            break;
            
        case PREPARE_DL_ERR_EXSIT:
            MMIPUB_OpenAlertWarningWin(TXT_FILE_EXISTED);//MS00191252
            break;
            
        case PREPARE_DL_ERR_OTHER:
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
            BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle());
            MMIBROWSER_SetToStopDL();
            MMK_CloseWin(win_id);
            break;
            
        default:
            break;
        }
    }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        //stop download
        if (BRW_StopDL(MMIBROWSER_GetInstance(),MMIBROWSER_GetReqHandle()))
        {
            //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleDownladMediaFileWinMsg stop dl SUCC"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8902_112_2_18_2_6_13_216,(uint8*)"");
        }
        else
        {
            //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleDownladMediaFileWinMsg stop dl FAIL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_8906_112_2_18_2_6_13_217,(uint8*)"");
        }
        MMIBROWSER_SetToStopDL();
        MMK_CloseWin(win_id);
        break;

#ifndef MMI_PDA_SUPPORT
    case MSG_NOTIFY_FORM_SWITCH_ACTIVE:
    {
        MMI_CTRL_ID_T active_ctrl_id = 0;
        
        active_ctrl_id = MMK_GetActiveCtrlId(win_id);
        
        switch (active_ctrl_id)
        {
        case MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID:
            GUIWIN_SetSoftkeyTextId(win_id, TXT_COMMON_OK, TXT_BROWSER_CHANGE, STXT_RETURN, TRUE);
            break;
            
        case MMIBROWSER_SAVE_MEDIAFILE_PATH_DROPDOWNLIST_CTRL_ID:
            GUIWIN_SetSoftkeyTextId(win_id, TXT_COMMON_OK, TXT_NULL, STXT_RETURN, TRUE);
            break;
            
        default:
            break;
        }
    }        
        break;
#endif /* MMI_PDA_SUPPORT */

    default :
        err_code = MMI_RESULT_FALSE;
        break;
    } 
    return err_code;
}

#if 0
/*****************************************************************************/
//  Description : Get Real device type
//  Global resource dependence : none
//  Author: dave.ruan
//  Note:
/*****************************************************************************/
LOCAL MMIFILE_DEVICE_E MMIBROWSER_GetRealDeviceTypeByText(MMI_STRING_T *path_value_str_ptr)
{
    MMI_STRING_T path_str = {0};
    MMIFILE_DEVICE_E dev = MMI_DEVICE_UDISK;
    MMIFILE_DEVICE_E device_type =MMI_DEVICE_NUM;
    
    if (PNULL == path_value_str_ptr || PNULL == path_value_str_ptr->wstr_ptr)
    {
        return  MMI_DEVICE_NUM;
    }
     
    for(; dev<MMI_DEVICE_NUM; dev++)
    {
        MMI_GetLabelTextByLang(MMIAPIFMM_GetDeviceName(dev), &path_str);
        if(0 == MMIAPICOM_Wstrncmp(path_value_str_ptr->wstr_ptr, path_str.wstr_ptr, path_str.wstr_len))
        {
            device_type = dev;
            break;
        }
    }

    return device_type;
}
#endif

/*****************************************************************************/
//  Description : set download prepare window message
//  Global resource dependence : 
//  Author: dave.ruan
//  Note:  fen.xie modify the get filename and path func
/*****************************************************************************/
LOCAL void SetDownloadFileNamePathWindow(MMI_WIN_ID_T win_id)
{
    BRW_MSG_ENTER_FILENM_REQ_T  *file_name_req_ptr = PNULL;
    MMI_STRING_T str1 = {0};
    MMI_STRING_T str2 = {0};
    MMI_CTRL_ID_T file_name_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_LABEL1_CTRL_ID;
    MMI_CTRL_ID_T file_name_value_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_EDITBOX_CTRL_ID;
    MMI_CTRL_ID_T path_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_LABEL2_CTRL_ID;
    MMI_CTRL_ID_T path_value_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_PATH_LABEL_CTRL_ID;
    wchar file_name[MMIBROWSER_DL_FILE_FULL_PATH_MAX_LEN+ 1] = {0};
    uint16 file_name_len = MMIBROWSER_DL_FILE_FULL_PATH_MAX_LEN;
    wchar fmm_path[MMIFILE_FULL_PATH_MAX_LEN + 1] = {0};
    uint16 fmm_path_len = MMIFILE_FULL_PATH_MAX_LEN;
    wchar display_path[MMIFILE_FULL_PATH_MAX_LEN + 1] = {0};
    uint16 display_path_len = MMIFILE_FULL_PATH_MAX_LEN;
    MMIFILE_DEVICE_E file_type = MMI_DEVICE_NUM;
    MMI_CTRL_ID_T dropdown_path_ctrl_id = MMIBROWSER_SAVE_MEDIAFILE_PATH_DROPDOWNLIST_CTRL_ID;
    BOOLEAN is_only_storage = FALSE;
    BRW_MIME_TYPE_E mime_type = BRW_MIME_UNKOWN;

    GUIFORM_SetStyle(MMIBROWSER_SAVE_MEDIAFILE_FORM1_CTRL_ID,GUIFORM_STYLE_UNIT);
    GUIFORM_SetStyle(MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID,GUIFORM_STYLE_UNIT);

    GUILABEL_SetTextById(file_name_ctrl_id, TXT_FILENAME, FALSE);
    GUILABEL_SetTextById(path_ctrl_id, STXT_STORE_MEDIUM, FALSE);

#ifdef MMI_PDA_SUPPORT
    GUIFORM_SetType(MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID, GUIFORM_TYPE_TP);
#endif /* MMI_PDA_SUPPORT */

    // file name edit control
    file_name_req_ptr = MMIBROWSER_GetFileNameReq();
    if (file_name_req_ptr != PNULL) 
    {
        mime_type = file_name_req_ptr->mime_type;
        //set file name label
        //get the default file name(protect)
        BrwDlGetDefaultFileName(file_name, &file_name_len, file_name_req_ptr->res_name_arr, mime_type);
        MMIBROWSER_UpdateEnterFileName(file_name);//update the file_name_req->res_name_arr
        str1.wstr_ptr = file_name;
        str1.wstr_len = MMIAPICOM_Wstrlen(file_name);
        GUILABEL_SetText(file_name_value_ctrl_id, &str1, FALSE);
#ifdef HOME_LOCATION_INQUIRE_SUPPORT_TCARD
        if (BRW_MIME_PNL == mime_type)
        {
            GUIFORM_SetChildDisplay(MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID, MMIBROWSER_SAVE_MEDIAFILE_LABEL1_CTRL_ID, GUIFORM_CHILD_DISP_GRAY);
        }
#endif
        //path or storage
        is_only_storage = MMIBROWSER_IsOnlyChooseStorage(mime_type);
        
        //set only storage for some special media file
        if (is_only_storage)
        {
            MMI_STRING_T  dropdownlist[MMI_DEVICE_NUM]  = {0};
            uint16  index = 0;
            uint16  cur_index = 0;
#ifdef MMI_PDA_SUPPORT
            GUISETLIST_ITEM_INFO_T  item_info = {0};
#endif /* MMI_PDA_SUPPORT */
            
#ifdef MMI_PDA_SUPPORT
            GUISETLIST_SetFontAlign(dropdown_path_ctrl_id, ALIGN_LVMIDDLE);
#endif /* MMI_PDA_SUPPORT */
            
            file_type = BrowserDlGetDeviceName(PNULL, PNULL, mime_type);//get the first valid device for mime type
            MMIBROWSER_SetSaveDeviceType(file_type);//init
            if (file_type < MMI_DEVICE_NUM)
            {
                MMIFILE_DEVICE_E dev = MMI_DEVICE_UDISK;

                for(; dev < MMI_DEVICE_NUM; dev++)//get the valid devices
                {
                    if(MMIAPIFMM_GetDeviceTypeStatus(dev))
                    {
                        MMI_GetLabelTextByLang(MMIAPIFMM_GetDeviceName(dev), &dropdownlist[index]);
#ifdef MMI_PDA_SUPPORT
                        SCI_MEMSET(&item_info, 0x00, sizeof(GUISETLIST_ITEM_INFO_T));

                        item_info.item_state = GUISETLIST_ITEM_STATE_TEXT;
                        item_info.str_info.wstr_ptr = dropdownlist[index].wstr_ptr;
                        item_info.str_info.wstr_len = dropdownlist[index].wstr_len;
                        GUISETLIST_AddItem(dropdown_path_ctrl_id, &item_info);
#endif /* MMI_PDA_SUPPORT */

                        if (dev == file_type)
                        {
                            cur_index = index;
                        }

                        index++;
                   }
                }
#ifdef MMI_PDA_SUPPORT
                GUISETLIST_SetCurIndex(dropdown_path_ctrl_id, cur_index);
                GUISETLIST_SetTitleTextId(dropdown_path_ctrl_id, STXT_STORE_MEDIUM);
#else
                GUIDROPDOWNLIST_AppendItemArray(dropdown_path_ctrl_id, dropdownlist, index);
                GUIDROPDOWNLIST_SetCurItemIndex(dropdown_path_ctrl_id, cur_index);
#endif /* MMI_PDA_SUPPORT */
            }
            else//don't find storage,hide it temp
            {
                GUIFORM_SetChildDisplay(MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID, MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID, GUIFORM_CHILD_DISP_HIDE);
            }
        }
        else
        {
            //set path label for normal media file
            file_type = MMIBROWSER_GetDefaultMediaFilePath(fmm_path,&fmm_path_len, mime_type);
            MMIBROWSER_SetSaveDeviceType(file_type);//init
            if (file_type < MMI_DEVICE_NUM)
            {
                str1.wstr_ptr = (wchar *)fmm_path;
                str1.wstr_len = fmm_path_len;

                str2.wstr_ptr = display_path;
                str2.wstr_len = display_path_len;
                MMIBROWSER_FmmPathToDisplay(&str1, &str2);
                GUILABEL_SetText(path_value_ctrl_id, &str2, FALSE);
                GUILABEL_SetIcon(path_value_ctrl_id, IMAGE_FMM_FOLDER_ICON);
            }
            else//don't find storage,hide it temp
            {
                GUIFORM_SetChildDisplay(MMIBROWSER_SAVE_MEDIAFILE_CTRL_ID, MMIBROWSER_SAVE_MEDIAFILE_FORM2_CTRL_ID, GUIFORM_CHILD_DISP_HIDE);
            }
        }
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]W SetDownloadFileNamePathWindow file_name_req_ptr PNULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_9098_112_2_18_2_6_14_218,(uint8*)"");
    }
}

/*****************************************************************************/
// Description : Export Open file Callback
// Global resource dependence : none
// Author: dave.ruan
// Note:
/*****************************************************************************/
LOCAL void BrwOpenSetMediaFileNameBack(BOOLEAN is_success, FILEARRAY file_array)
{
    BrwOpenLabelCallBack(is_success, file_array, MMIBROWSER_SAVE_MEDIAFILE_PATH_LABEL_CTRL_ID);
}

/*****************************************************************************/
// 	Description : Export Open file Callback
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL void BrwOpenLabelCallBack(BOOLEAN is_success, FILEARRAY file_array, MMI_HANDLE_T ctrl_handle)
{
    uint32 file_num = 0;
    FILEARRAY_DATA_T file_array_data = {0};
    MMI_STRING_T path_str = {0};
    MMI_STRING_T dsp_str = {0};
    wchar path[MMIFILE_FULL_PATH_MAX_LEN + 1] = {0};
    wchar dsp_path[MMIFILE_FULL_PATH_MAX_LEN + 1] = {0};
    MMIFILE_DEVICE_E file_type = MMI_DEVICE_NUM;

    //SCI_TRACE_LOW:"[BRW]BrwOpenLabelCallBack is_success = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_9125_112_2_18_2_6_14_219,(uint8*)"d", is_success);
    
    if (is_success)
    {
        file_num = MMIAPIFILEARRAY_GetArraySize(file_array);
        
        //SCI_TRACE_LOW:"[BRW]BrwOpenLabelCallBack, file_num = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_9131_112_2_18_2_6_14_220,(uint8*)"d", file_num);
        
        if ((file_num > 0) && MMIAPIFILEARRAY_Read(file_array, 0, &file_array_data))
        {
            //read user selected dir
            path_str.wstr_len = MIN(file_array_data.name_len, MMIFILE_FULL_PATH_MAX_LEN);
            path_str.wstr_ptr = path;
            MMI_WSTRNCPY(path, MMIFILE_FULL_PATH_MAX_LEN,
                file_array_data.filename, file_array_data.name_len, 
                path_str.wstr_len);
            
            if (PNULL != ctrl_handle)
            {
                dsp_str.wstr_ptr = dsp_path;
                dsp_str.wstr_len = MMIFILE_FULL_PATH_MAX_LEN;
                file_type = MMIBROWSER_FmmPathToDisplay(&path_str,  &dsp_str);
                MMIBROWSER_SetSaveDeviceType(file_type);//set
                //SCI_TRACE_LOW:"[BRW]BrwOpenLabelCallBack, file_type = %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_9148_112_2_18_2_6_14_221,(uint8*)"d", file_type);
                GUILABEL_SetText(ctrl_handle, &dsp_str, FALSE);
            }
        }
    }
    
    //释放列表
    if (PNULL != file_array)
    {
        MMIAPIFILEARRAY_Destroy(&file_array);
        file_array = PNULL;
    }
}

/*****************************************************************************/
//  Description : the callback of button21
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button21CallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMIBROWSER_QueryExit();
    return result;
}

/*****************************************************************************/
//  Description : the callback of button22
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button22CallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMIBROWSER_OpenSettingWin();

    return result;
}

#if 0
/*****************************************************************************/
//  Description : the callback of button23
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button23CallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMK_CreateWin((uint32*)MMIBROWSER_STARTPAGE_POPMENU_WIN_TAB,(ADD_DATA)TRUE);
    GUIMENU_CreatDynamic(PNULL,MMIBROWSER_STARTPAGE_POPMENU_WIN_ID,MMIBROWSER_MENU_CTRL_ID,GUIMENU_STYLE_POPUP_CHECK);

    return result;
}
#endif

/*****************************************************************************/
//  Description : the callback of button24
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E Button24CallBack(void)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, MMIBROWSER_GetHomepageUrl(), PNULL);

    return result;
}

#ifdef BROWSER_START_PAGE_DEFAULT
/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwStartPageWin(MMI_WIN_ID_T win_id)
{
#ifdef BROWSER_SEARCH_SUPPORT
    GUIFORM_CHILD_WIDTH_T   from_width = {0};
#endif    
    //uint16  img_height = 0;
    MMI_STRING_T                item_text_1 = {0};
#ifndef BROWSER_BOOKMARK_HISTORY_TAB
    MMI_STRING_T                item_text_2 = {0};
    char                        temp_item[MMIBROWSER_START_PAGE_ITEM_INFO_LEN + 1] = {0};
    wchar                       item_info[MMIBROWSER_START_PAGE_ITEM_INFO_LEN + 1] = {0};
    uint16                      num_len = 0;
#endif
    GUIFORM_CHILD_HEIGHT_T      label_height = {0};
    GUIFORM_CHILD_HEIGHT_T      child_height = {0};
    GUI_BG_T                    bg = {0};
    MMI_CTRL_ID_T               list_ctrl = MMIBROWSER_START_PAGE_LIST_CTRL_ID;

    SCI_MEMSET(&bg, 0x00, sizeof(bg));
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_SEARCH_BAR_BG;
    GUIFORM_SetBg( MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &bg);

    label_height.type     = GUIFORM_CHLID_HEIGHT_FIXED;
    label_height.add_data = MMIBROWSER_STARTPAGE_FORM1_HEIGHT;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &label_height);

    child_height.type = GUIFORM_CHLID_HEIGHT_LEFT;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM_CTRL_ID, list_ctrl, &child_height);
    GUIFORM_IsSlide(MMIBROWSER_START_PAGE_FORM_CTRL_ID, FALSE);
    GUIFORM_PermitChildBg(MMIBROWSER_START_PAGE_FORM_CTRL_ID, FALSE);
    GUIFORM_PermitChildFont(MMIBROWSER_START_PAGE_FORM_CTRL_ID, FALSE);
    
#ifdef BROWSER_SEARCH_SUPPORT
    from_width.type = GUIFORM_CHLID_WIDTH_PERCENT;
    from_width.add_data = 65;
    GUIFORM_SetChildWidth(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &from_width);

    label_height.type     = GUIFORM_CHLID_HEIGHT_PERCENT;
    label_height.add_data = 95;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &label_height);
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, &label_height);

    {
        uint16 hor_space = 10;
        GUIFORM_SetSpace(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &hor_space, PNULL);
    }
#endif

    GUILABEL_SetIcon(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, IMAGE_BROWSER_ADDRESS_ICON);
    GUILABEL_SetTextById(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, TXT_WWW_INPUT_URL, FALSE);
    GUILABEL_SetFont(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID,MMI_DEFAULT_NORMAL_FONT, MMI_BLACK_COLOR);

    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG_FOCUSED;
    GUILABEL_SetBg(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &bg);

#ifdef BROWSER_SEARCH_SUPPORT
    GUILABEL_SetIcon(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, IMAGE_BROWSER_SEARCH_ICON);
    GUILABEL_SetTextById(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, STXT_SEARCH, FALSE);
    GUILABEL_SetFont(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID,MMI_DEFAULT_NORMAL_FONT, MMI_BLACK_COLOR);
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
    GUILABEL_SetBg(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, &bg);
#endif

    GUIFORM_SetType(MMIBROWSER_START_PAGE_FORM_CTRL_ID, GUIFORM_TYPE_TP);
    // GUISETLIST_SetFontAlign(list_ctrl, ALIGN_LVMIDDLE);

    GUILIST_SetMaxItem(list_ctrl, 10, FALSE);
    GUILIST_RemoveAllItems(list_ctrl);
    GUILIST_SetSlideState(list_ctrl, TRUE);

#ifdef MMIDORADO_MY_NAV_SUPPORT
    MMIRES_GetText(TXT_BROWSER_JP_AND_DH, PNULL, &item_text_1);
    MMIBROWSER_AppendOneTextOneImageItem(list_ctrl, &item_text_1, IMAGE_NULL, (uint32)MMIBROWSER_OpenBoutiqueAndNavWin);
#endif

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
    MMIRES_GetText(TXT_BROWSER_RMFL, PNULL, &item_text_1);
    MMIBROWSER_AppendOneTextOneImageItem(list_ctrl, &item_text_1, IMAGE_NULL, (uint32)MMIBROWSER_OpenFixHtmlWin);
#endif

#ifdef BROWSER_BOOKMARK_HISTORY_TAB
    MMIRES_GetText(TXT_BROWSER_BOOKMARK_AND_HISTORY, PNULL, &item_text_1);
    MMIBROWSER_AppendOneTextOneImageItem(list_ctrl, &item_text_1, IMAGE_NULL, (uint32)MMIBROWSER_OpenBookmarkWin);
#else
    //bookmark
    MMIRES_GetText(TXT_BOOKMARK, PNULL, &item_text_1);
    SCI_MEMSET(item_info, 0, sizeof(item_info));
    num_len = (uint16)sprintf(temp_item, "(%d/%d)", MMIBRW_BookmarkGetCount(), MMIBRW_BOOKMARK_MAX_ITEM);
    MMI_STRNTOWSTR(item_info, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, (uint8 *)temp_item, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, num_len);
    item_text_2.wstr_ptr = item_info;
    item_text_2.wstr_len = MMIAPICOM_Wstrlen(item_info);

    MMIBROWSER_AppendOneTextOneImageItem(list_ctrl, &item_text_1, IMAGE_NULL, (uint32)MMIBROWSER_OpenBookmarkWin);

    //History
    MMIRES_GetText(TXT_COMMON_HISTORY, PNULL, &item_text_1);
    SCI_MEMSET(item_info, 0, sizeof(item_info));
    num_len = (uint16)sprintf(temp_item, "(%d/%d)", BRW_VisitHisGetTotal(), BRW_MAX_VISIT_HIS_ITEM_NUM);
    MMI_STRNTOWSTR(item_info, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, (uint8 *)temp_item, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, num_len);
    item_text_2.wstr_ptr = item_info;
    item_text_2.wstr_len = MMIAPICOM_Wstrlen(item_info);

    MMIBROWSER_AppendOneTextOneImageItem(list_ctrl, &item_text_1, IMAGE_NULL, (uint32)MMIBROWSER_OpenHisListWin);
#endif

    //Page Memo
    MMIRES_GetText(TXT_BROWSER_HAS_SAVE_FILE, PNULL, &item_text_1);
    MMIBROWSER_AppendOneTextOneImageItem(list_ctrl, &item_text_1, IMAGE_NULL, (uint32)MMIBROWSER_OpenSnapshotListWin);
    GUIFORM_SetMargin(MMIBROWSER_START_PAGE_FORM_CTRL_ID, 0);
}

/*****************************************************************************/
//  Description : handle Start page of the browser
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleStartPageWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    ret=MMI_RESULT_TRUE;    
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifndef TOUCH_PANEL_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        InitBrwStartPageWin(win_id);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID);
        break;

#ifndef BROWSER_BOOKMARK_HISTORY_TAB
    case MSG_GET_FOCUS:
        UpdateStartPageListInfo();
        break;
#endif

#ifdef WIN32
    case MSG_APP_1:
        MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, "http://172.16.14.173:8082/", PNULL);
        break;
#endif

    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
        {
            MMI_CTRL_ID_T       ctrl_id = 0;

            if (PNULL != param)
            {
                if (MSG_CTL_PENOK == msg_id)
                {
		            ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;
                }
                else
                {
                    ctrl_id = MMK_GetActiveCtrlId(win_id);
                }    

                if (ctrl_id == MMIBROWSER_START_PAGE_LABEL11_CTRL_ID)
                {
                    MMIBROWSER_InputUrl(PNULL);
                }
                else if (ctrl_id == MMIBROWSER_START_PAGE_LABEL12_CTRL_ID)
                {
#ifdef BROWSER_SEARCH_SUPPORT
                    MMIBROWSER_OpenSearchWin();
#endif
                }
                else if (ctrl_id == MMIBROWSER_START_PAGE_LIST_CTRL_ID)
                {
                    uint32              index = 0;
                    SETTING_ITEM_FUNC   item_func = PNULL;

                    index = GUILIST_GetCurItemIndex(ctrl_id);

                    GUILIST_GetItemData(ctrl_id, index, (uint32*)(&item_func));

                    if (PNULL != item_func)
                    {
                        (*item_func)();
                    }
                }
            }
        }
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
#ifdef MMI_PDA_SUPPORT
        if (MMITHEME_IsIstyle())
        {
            break;//istyle without pop menu        
        }
#endif
        MMK_CreateWin((uint32*)MMIBROWSER_STARTPAGE_POPMENU_WIN_TAB,PNULL);
        GUIMENU_CreatDynamic(PNULL,MMIBROWSER_STARTPAGE_POPMENU_WIN_ID,MMIBROWSER_MENU_CTRL_ID,GUIMENU_STYLE_POPUP);
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMIBROWSER_QueryExit();
        break;

#ifndef MMI_PDA_SUPPORT
    case MSG_NOTIFY_FORM_SWITCH_ACTIVE:
    {
        uint32          i= 0;
        MMI_CTRL_ID_T   active_ctrl_id = MMK_GetActiveCtrlId(win_id);
        GUI_BG_T        bg = {0};

        bg.bg_type = GUI_BG_IMG;
        bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;

        for (i = MMIBROWSER_START_PAGE_LABEL11_CTRL_ID; i <= MMIBROWSER_START_PAGE_LABEL12_CTRL_ID; i++)
        {
            if (i != active_ctrl_id)
            {
                bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
            }
            else
            {
                bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG_FOCUSED;
            }

            GUILABEL_SetBg(i, &bg);
        }

        if (MMK_IsFocusWin(win_id))//update
        {
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
    }
        break;
#endif
        
    case MSG_CLOSE_WINDOW:
        MMIBROWSER_Exit();
        break;

#ifdef MMI_PDA_SUPPORT
    case MSG_TP_PRESS_DOWN:
        if (MMITHEME_IsIstyle())
        {
            GUI_POINT_T point = {0};
            uint32  i = 0;
            
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);

            for (i = 0; i < ARRAY_SIZE(s_start_page_button_info); i++)
            {
                if (MMK_PosIsCtrl(s_start_page_button_info[i].parent_form_ctrl_id, point))
                {
                    GUI_BG_T    bg = {0};
                    
                    bg.bg_type = GUI_BG_IMG;
                    bg.img_id = s_start_page_button_info[i].pressed_img_id;
                    GUIBUTTON_SetBg(s_start_page_button_info[i].button_ctrl_id, &bg);
                    GUIBUTTON_Update(s_start_page_button_info[i].button_ctrl_id);
                    //reset
                    bg.img_id = s_start_page_button_info[i].release_img_id;
                    GUIBUTTON_SetBg(s_start_page_button_info[i].button_ctrl_id, &bg);
                    break;
                }
            }  
        }
        break;

    case MSG_TP_PRESS_UP:
        if (MMITHEME_IsIstyle())
        {
            uint32  i = 0;
            GUI_POINT_T point = {0};
            GUI_BG_T    bg = {0};

            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);

            for (i = 0; i < ARRAY_SIZE(s_start_page_button_info); i++)
            {
                if (MMK_PosIsCtrl(s_start_page_button_info[i].button_ctrl_id, point))
                {
                    if (PNULL != s_start_page_button_info[i].callback_func)
                    {
                        s_start_page_button_info[i].callback_func();
                    }
                    break;
                }
            }

            bg.bg_type = GUI_BG_IMG;
            for (i = 0; i < ARRAY_SIZE(s_start_page_button_info); i++)
            {
                bg.img_id = s_start_page_button_info[i].release_img_id;
                GUIBUTTON_SetBg(s_start_page_button_info[i].button_ctrl_id, &bg);
                GUIBUTTON_Update(s_start_page_button_info[i].button_ctrl_id);
            }
        }
        break;
#endif

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
    case MSG_BRW_MSG_INIT_CNF:
    case MSG_BRW_MSG_EXIT_CNF:
    case MSG_BRW_MSG_TYPE_ENTER_FILENM_REQ:
    case MSG_BRW_MSG_TYPE_DL_CONFIRM_REQ:     
    case MSG_BRW_MSG_TYPE_AUTH_REQ:           
    case MSG_BRW_MSG_TYPE_CHANGE_URL_IND:     
    case MSG_BRW_MSG_TYPE_NEW_PAGE_REQ_IND:   
    case MSG_BRW_MSG_TYPE_UPDATE_CONTENT_IND: 
    case MSG_BRW_MSG_TYPE_PROGRESS_IND:       
    case MSG_BRW_MSG_TYPE_ERROR_IND:            
    case MSG_BRW_MSG_TYPE_DOWNLOAD_OK_IND:     
    case MSG_BRW_MSG_TYPE_DOWNLOAD_ERR_IND:   
    case MSG_BRW_MSG_TYPE_PAGE_GET_IND:
    case MSG_BRW_MSG_TYPE_MEDIA_GET_ONE_IND:
    case MSG_BRW_MSG_TYPE_DISPLAY_READY_IND:  
    case MSG_BRW_MSG_TYPE_SSL_CERT_IND:       
    case MSG_BRW_MSG_TYPE_REPAINT_READY_IND:
    case MSG_BRW_MSG_TYPE_PAGE_STOP_IND:
    case MSG_BRW_MSG_TYPE_OMA_DL_CONFIRM_REQ:
    case MSG_BRW_MSG_TYPE_LAYOUTCTRL_DESTROY_REQ:
    case MSG_BRW_MSG_TYPE_PDP_ACTIVE_CNF:        
        HandleFixedInstanceControlMsg(msg_id,param);
        break;
#endif        

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}
#endif/*BROWSER_START_PAGE_DEFAULT*/

/*****************************************************************************/
//  Description : open the start page window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenStartPageWin(void)
{
#ifdef BROWSER_START_PAGE_THEME1
    {        
        MMIBROWSER_OpenBookmarkWin();

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
        MMK_CreateWin((uint32*)MMIBROWSER_FIX_HTML_WIN_TAB,PNULL);
#endif
        MMK_CreateWin((uint32*)MMIBROWSER_START_PAGE_WIN_TAB,PNULL);

        MMIBROWSER_SetSlide(TRUE);
    }
#else
    MMK_CreateWin((uint32*)MMIBROWSER_START_PAGE_WIN_TAB,PNULL);
#endif
}

#ifdef BROWSER_START_PAGE_THEME1
/*****************************************************************************/
//  Description : Set Side
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_SetSlide(BOOLEAN is_slide)
{
#ifdef MMIDORADO_FIX_HMTL_SUPPORT
    MMI_WIN_ID_T slide_win_arr[] = {MMIBROWSER_BK_AND_HISTORY_WIN_ID, MMIBROWSER_START_PAGE_WIN_ID, MMIBROWSER_FIX_HTML_WIN_ID};
#else
    MMI_WIN_ID_T slide_win_arr[] = {MMIBROWSER_BK_AND_HISTORY_WIN_ID, MMIBROWSER_START_PAGE_WIN_ID};
#endif
    MMI_SLIDE_INIT_T slide_init = {0};

    if (MMK_IsOpenWin(MMIBROWSER_START_PAGE_WIN_ID))
    {
        if (is_slide)
        {
            if (MMI_SLIDE_HANDLE_INVALID == s_startpage_win_slide_handle)
            {
                slide_init.default_win = MMIBROWSER_START_PAGE_WIN_ID;
                slide_init.is_has_symbol = TRUE;
                slide_init.parent_win_tab_ptr = PNULL;
                slide_init.slide_both_rect_ptr = PNULL;
                slide_init.slide_rect_type = SLIDE_RECT_TYPE_CLIENTRECT;
                slide_init.type = MMI_SLIDE_ALL_NO_CHILD_WIN;
                slide_init.win_id_arr_ptr = slide_win_arr;
                slide_init.win_id_arr_size = ARR_SIZE(slide_win_arr);
                
                if (MMISLIDE_Creat(&slide_init, &s_startpage_win_slide_handle))
                {
                    GUI_POINT_T     indicator_point = {0};
                    SLIDE_SYM_DIS_INFO_T sym_dis_info = 
                    {
                        SLIDE_SYM_WORK_AUTO_HIDE,
                        SLIDE_SYMBOL_TYPE_CUSTOM_IMG,
                        {IMAGE_BROWSER_SLIDE_OFF, IMAGE_BROWSER_SLIDE_ON, IMAGE_BROWSER_SLIDE_BG, IMAGE_BROWSER_SLIDE_BGL, IMAGE_BROWSER_SLIDE_BGR},
                        PNULL,
                        PNULL,
                    };

#ifdef MMI_PDA_SUPPORT
                    if (MMITHEME_IsIstyle())
                    {
                        indicator_point.y = MMITHEME_GetTabHeight();
                        indicator_point.y += MMIBROWSER_START_INDICATOR_DISTANCE;
                    }
#endif /* MMI_PDA_SUPPORT */
                    MMISLIDE_SetSymbolIsOnTop(&s_startpage_win_slide_handle, FALSE);
                    MMISLIDE_SetSymbolPosition(&s_startpage_win_slide_handle, &indicator_point);
                    MMISLIDE_SetSymbolDisplayInfo(&s_startpage_win_slide_handle, &sym_dis_info);
                    MMISLIDE_Active(&s_startpage_win_slide_handle);
                }
            }
        }
        else if (MMI_SLIDE_HANDLE_INVALID != s_startpage_win_slide_handle)
        {
            MMISLIDE_ReleaseEx(&s_startpage_win_slide_handle, FALSE);
        }
    }
}

/*****************************************************************************/
//  Description : release the Side in browser
//  Global resource dependence : s_startpage_win_slide_handle
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_ReleaseSlide(void)
{
    MMISLIDE_Release(&s_startpage_win_slide_handle);
}
#endif

/*****************************************************************************/
//  Description : open the web window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenWebWin(void)
{
    MMK_CreateWin((uint32*)MMIBROWSER_MAIN_WIN_TAB,PNULL);
}

/*****************************************************************************/
//  Description : create pop menu for start page
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CreateStartPagePopWinMenu(BOOLEAN is_clear_menu)
{
    MMI_CTRL_ID_T   ctrl_id = MMIBROWSER_MENU_CTRL_ID;
    MMI_STRING_T    kstring = {0};
    uint16  nodex_index =0;
    uint32  i = 0;
    uint32  item_num = 0;  
    GUIMENU_DYNA_ITEM_T node_item = {0};
    uint32  node_id = 0;

#if 0
    if (is_clear_menu)
    {
        item_num = sizeof(s_menu_brw_clear_popupmenu_table)/sizeof(*s_menu_brw_clear_popupmenu_table);
        nodex_index = 0;
        for(i = 0; i < item_num; i++)
        {
            node_id = s_menu_brw_clear_popupmenu_table[i][0];
            MMI_GetLabelTextByLang(s_menu_brw_clear_popupmenu_table[i][1], &kstring);
            node_item.item_text_ptr = &kstring;
            GUIMENU_InsertNode(nodex_index, node_id, 0, &node_item, ctrl_id);
            //disable
            if ((ID_POPUP_MENU_SUB_CLEARCACHE == node_id) || (ID_POPUP_MENU_SUB_CLEARCOOKIE == node_id))
            {
                if (MMIAPIFMM_GetFirstValidDevice() >= MMI_DEVICE_NUM)
                {
                    GUIMENU_SetNodeGrayed(TRUE, nodex_index, 0, ctrl_id);
                }
            }

            nodex_index ++;
        }
#ifdef MMI_PDA_SUPPORT
        {
            GUIMENU_BUTTON_INFO_T button_info = {0};

            button_info.is_static   = FALSE;
            button_info.button_style = GUIMENU_BUTTON_STYLE_OK_CANCEL;
            button_info.node_id = 0;
            GUIMENU_SetButtonStyle(ctrl_id, &button_info);
        }
#endif /* MMI_PDA_SUPPORT */
    }
    else
#endif        
    {
        item_num = sizeof(s_menu_brw_startpage_popupmenu_table)/sizeof(*s_menu_brw_startpage_popupmenu_table);
        nodex_index = 0;
        for(i = 0; i < item_num; i++)
        {
            node_id = s_menu_brw_startpage_popupmenu_table[i][0];
            MMI_GetLabelTextByLang(s_menu_brw_startpage_popupmenu_table[i][1], &kstring);
            node_item.item_text_ptr = &kstring;
#ifdef MMI_PDA_SUPPORT
            node_item.select_icon_id = s_menu_brw_startpage_popupmenu_table[i][2];
#endif /* MMI_PDA_SUPPORT */
            GUIMENU_InsertNode(nodex_index, node_id, 0, &node_item, ctrl_id);

#if 0
//#ifdef MMI_PDA_SUPPORT
            if (ID_POPUP_MENU_CLEAR == node_id)
            {
                GUIMENU_SetSubMenuStyle(ctrl_id, node_id, GUIMENU_STYLE_POPUP_CHECK);
            }
#endif /* MMI_PDA_SUPPORT */

            nodex_index ++;
        }
        
#if 0
        item_num = sizeof(s_menu_brw_clear_popupmenu_table)/sizeof(*s_menu_brw_clear_popupmenu_table);
        nodex_index = 0;
        for(i = 0; i < item_num; i++)
        {
            node_id = s_menu_brw_clear_popupmenu_table[i][0];
            MMI_GetLabelTextByLang(s_menu_brw_clear_popupmenu_table[i][1], &kstring);
            node_item.item_text_ptr = &kstring;
            GUIMENU_InsertNode(nodex_index, node_id, ID_POPUP_MENU_CLEAR, &node_item, ctrl_id);
            //disable
            if ((ID_POPUP_MENU_SUB_CLEARCACHE == node_id) || (ID_POPUP_MENU_SUB_CLEARCOOKIE == node_id))
            {
                if (MMIAPIFMM_GetFirstValidDevice() >= MMI_DEVICE_NUM)
                {
                    GUIMENU_SetNodeGrayed(TRUE, nodex_index, ID_POPUP_MENU_CLEAR, ctrl_id);
                }
            }

            nodex_index ++;
        }
#ifdef MMI_PDA_SUPPORT
        {
            GUIMENU_BUTTON_INFO_T button_info = {0};

            button_info.is_static   = FALSE;
            button_info.button_style = GUIMENU_BUTTON_STYLE_OK_CANCEL;
            button_info.node_id = ID_POPUP_MENU_CLEAR;
            GUIMENU_SetButtonStyle(ctrl_id, &button_info);
        }
#endif /* MMI_PDA_SUPPORT */
#endif
    }

    return TRUE;
} 

/*****************************************************************************/
//  Description : handle popmenu (start page win) window message 
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleStartPagePopMenuWinMsg(MMI_WIN_ID_T win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM param)
{
    MMI_MENU_ID_T                       menu_id = 0;
    MMI_RESULT_E                        result = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T                       ctrl_id = MMIBROWSER_MENU_CTRL_ID;
    uint32 is_clear_menu = (uint32)MMK_GetWinAddDataPtr(win_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        CreateStartPagePopWinMenu((BOOLEAN)is_clear_menu);
        MMK_SetAtvCtrl(win_id, ctrl_id);						
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        menu_id = GUIMENU_GetCurNodeId(ctrl_id);/*lint !e718*/
        //SCI_TRACE_LOW:"BrwHandleStartPagePopMenuWinMsg:menu_id = 0x%x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_9912_112_2_18_2_6_16_222,(uint8*)"d", menu_id);
        switch(menu_id) 
        {
        case ID_POPUP_MENU_OPEN_HOMEPAGE:
            MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, MMIBROWSER_GetHomepageUrl(), PNULL);
            MMK_CloseWin(win_id);
            break;
            
        case ID_POPUP_MENU_SUB_SETTING:
            MMIBROWSER_OpenSettingWin();
            MMK_CloseWin(win_id);
            break;

#if 0
#ifdef MMI_PDA_SUPPORT
        case ID_POPUP_MENU_SUB_CLEARCACHE:
        case ID_POPUP_MENU_SUB_CLEARCOOKIE:
        case ID_POPUP_MENU_CLEAR_INPUT_HISTORY:
        case ID_POPUP_MENU_SUB_CLEARKEY:
            {
                uint16                  checked_item_count = 0;
                uint32                  i = 0;
                GUIMENU_CHECK_INFO_T    check_info_ptr[sizeof(s_menu_brw_clear_popupmenu_table)/sizeof(s_menu_brw_clear_popupmenu_table[0])] = {0};

                checked_item_count = GUIMENU_GetCheckInfo(ctrl_id, PNULL, 0);

                if (checked_item_count > 0)
                {
                    GUIMENU_GetCheckInfo(ctrl_id, check_info_ptr, checked_item_count);
                    for (i = 0; i < checked_item_count; i++)
                    {
                        if (check_info_ptr[i].node_id == ID_POPUP_MENU_SUB_CLEARCACHE)
                        {
                            BRW_ClearAllCache();
                        }
                        else if (check_info_ptr[i].node_id == ID_POPUP_MENU_SUB_CLEARCOOKIE)
                        {
                            BRW_ClearAllCookie();
                        }
                        else if (check_info_ptr[i].node_id == ID_POPUP_MENU_CLEAR_INPUT_HISTORY)
                        {
                            BrwDeleteInputHis();
                        }
#ifdef BROWSER_SEARCH_SUPPORT
                        else if (check_info_ptr[i].node_id == ID_POPUP_MENU_SUB_CLEARKEY)
                        {
                            MMIBROWSER_CleanKeyInfo();
                        }
#endif/*BROWSER_SEARCH_SUPPORT*/
                    }
#ifdef MMI_GUI_STYLE_TYPICAL
                    MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_PDA_CLEAR_SUCCESS);
#endif /* MMI_GUI_STYLE_TYPICAL */
                }

                MMK_CloseWin(win_id);
            }
            break;
#else
        case ID_POPUP_MENU_SUB_CLEARCACHE:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_CACHE,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;

        case ID_POPUP_MENU_SUB_CLEARCOOKIE:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_COOKIE,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
            
        case ID_POPUP_MENU_CLEAR_INPUT_HISTORY:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_INPUT,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;

#ifdef BROWSER_SEARCH_SUPPORT
        case ID_POPUP_MENU_SUB_CLEARKEY:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_KEY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
#endif/*BROWSER_SEARCH_SUPPORT*/

#endif /* MMI_PDA_SUPPORT */
#endif

        case ID_POPUP_MENU_EXIT:
            MMIBROWSER_QueryExit();
            break;
            
#if 0
        case ID_POPUP_MENU_CLEAR_SUB_ALL:
            MMIBROWSER_OpenQueryWinByTextId(TXT_BROWSER_CLEAR_ALL,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            break;
#endif            

#ifdef BROWSER_OPERA_START_PAGE
        case ID_POPUP_MENU_BOOKMARK_LIST:
            MMIBROWSER_OpenBookmarkWin();
            MMK_CloseWin(win_id);
            break;

        case ID_POPUP_MENU_HISTORY:
            MMIBROWSER_OpenHisListWin();
            MMK_CloseWin(win_id);
            break;
        case ID_POPUP_MENU_HAS_SAVE_FILE:
            MMIBROWSER_OpenSnapshotListWin();
            MMK_CloseWin(win_id);
            break;
#endif

        default:
            break;
        }
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
#if 0
    case MSG_PROMPTWIN_OK:
        MMIBROWSER_CloseQuerytWin(PNULL);
        menu_id = GUIMENU_GetCurNodeId(ctrl_id);/*lint !e718*/
        switch(menu_id)
        {
        case ID_POPUP_MENU_SUB_CLEARCACHE:
            if (!BRW_ClearAllCache())
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_CACHE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCache fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_10037_112_2_18_2_6_16_223,(uint8*)"");
            }
            else
            {
#ifdef MMI_GUI_STYLE_TYPICAL
                MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_CACHE_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            }
            
            break;
        case ID_POPUP_MENU_SUB_CLEARCOOKIE:
            if (!BRW_ClearAllCookie())
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_COOKIE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCookie fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_10049_112_2_18_2_6_16_224,(uint8*)"");
            }
            else
            {
#ifdef MMI_GUI_STYLE_TYPICAL
                MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_COOKIE_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            }
            break;
            
        case ID_POPUP_MENU_CLEAR_INPUT_HISTORY:
            BrwDeleteInputHis();
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_INPUT_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;

 #ifdef BROWSER_SEARCH_SUPPORT
       case ID_POPUP_MENU_SUB_CLEARKEY:
            MMIBROWSER_CleanKeyInfo();
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_KEY_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;
#endif

        case ID_POPUP_MENU_CLEAR_SUB_ALL:
            BrwDeleteInputHis();
#ifdef BROWSER_SEARCH_SUPPORT
            MMIBROWSER_CleanKeyInfo();
#endif            
            if (!BRW_ClearAllCache())
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_CACHE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCache fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_10077_112_2_18_2_6_16_225,(uint8*)"");
                break;
            }
            if (!BRW_ClearAllCookie())
            {
                MMIPUB_OpenAlertWarningWin(TXT_BROWSER_CLEAR_COOKIE_FAIL);
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandlePopMenuWinMsg BRW_ClearCookie fail"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_10083_112_2_18_2_6_16_226,(uint8*)"");
                break;
            }
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_BROWSER_CLEAR_ALL_SUCC);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;
            
        default:
            break;
            
        }
        MMK_CloseWin(win_id);
        break;

    case MSG_PROMPTWIN_CANCEL:
        MMIBROWSER_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;        
#endif

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return result;
    
}

/*****************************************************************************/
// 	Description : to handle exit webpage query win
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleQueryExitWebWin(
                    MMI_WIN_ID_T win_id, 
                    MMI_MESSAGE_ID_E msg_id, 
                    DPARAM param
                    )
{
    MMI_RESULT_E        result = MMI_RESULT_TRUE;

    switch (msg_id)
    {
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
        MMK_CloseWin(win_id);  
        MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_STOP, TRUE);//for the animal display normal
        MMK_CloseWin(MMIBROWSER_MAIN_WIN_ID);//only exit the web window
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin( win_id );
        break;

    default:
        result = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param );
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : to handle exit browser query win
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleQueryExitBrowserWin(
                    MMI_WIN_ID_T win_id, 
                    MMI_MESSAGE_ID_E msg_id, 
                    DPARAM param
                    )
{
    MMI_RESULT_E        result = MMI_RESULT_TRUE;

    switch (msg_id)
    {
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
        MMK_CloseWin(win_id);  
        MMIBROWSER_SetAnimateConnectCtrlState(MMIBROWSER_ANIMATE_STATE_STOP, TRUE);//for the animal display normal
        MMIBROWSER_Exit();//close the browser
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin( win_id );
        break;

    default:
        result = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param );
        break;
    }
    
    return (result);
}

#ifdef BROWSER_TOOLBAR
/*****************************************************************************/
// 	Description : change the backward button
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL void BrowserChangeButtonStatus(BOOLEAN is_update)
{
    GUI_BG_T        fg = {0};
    GUI_BG_T        pressed_fg = {0};
    MMI_CTRL_ID_T   button_1 = MMIBROWSER_TOOLBAR_1_BUTTON_CTRL_ID;
    MMI_CTRL_ID_T   button_2 = MMIBROWSER_TOOLBAR_2_BUTTON_CTRL_ID;
    MMI_CTRL_ID_T   button_5 = MMIBROWSER_TOOLBAR_5_BUTTON_CTRL_ID;
    BOOLEAN         can_backward = TRUE;
    BOOLEAN         can_forward = TRUE;
    
    fg.bg_type = GUI_BG_IMG;
    pressed_fg.bg_type = GUI_BG_IMG;

    can_backward = BRW_NavHisIsBackwardEnable();
    can_forward = BRW_NavHisIsForwardEnable();

    fg.img_id = can_backward ? IMAGE_BROWSER_BACKWARD : IMAGE_BROWSER_EXIT;
    pressed_fg.img_id = can_backward ? IMAGE_BROWSER_PRESSED_BACKWARD : IMAGE_BROWSER_PRESSED_EXIT;

    GUIBUTTON_SetFg(button_1, &fg);
    
    GUIBUTTON_SetPressedFg(button_1, &pressed_fg);

    if (is_update)
    {
        GUIBUTTON_Update(button_1);
    }

    GUIBUTTON_SetGrayed(button_2, (BOOLEAN)(!can_forward), PNULL, is_update);

    GUIBUTTON_SetGrayed(button_5, (BOOLEAN)(!MMK_IsOpenWin(MMIBROWSER_START_PAGE_WIN_ID)), PNULL, is_update);
}
#endif

/*****************************************************************************/
//  Description : open the save media file window for enter file name and path to load
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenSaveMediaFileWin(BRW_MIME_TYPE_E mime_type,ADD_DATA add_data_ptr)
{
    BOOLEAN is_only_storage = FALSE;

    is_only_storage = MMIBROWSER_IsOnlyChooseStorage(mime_type);

    //set only storage for some special media file
    if (is_only_storage)
    {
        MMK_CreateWin((uint32*)BRW_SAVE_MEDIAFILE_ONLY_STORAGE_WIN_TAB, add_data_ptr);
    }
    else
    {
        MMK_CreateWin((uint32*)BRW_SAVE_MEDIAFILE_WIN_TAB, add_data_ptr);
    }
}

#ifdef MMI_PDA_SUPPORT
#if 0
/*****************************************************************************/
//  Description :
//  Global resource dependence : 
//  Author: Jiaoyou.wu
//  Note:
/*****************************************************************************/
LOCAL void BrwOpenPropertyWin(void)
{
    uint16              temp_url[MMIBROWSER_MAX_URL_LEN + 1] = {0};
    char*               url_ptr = PNULL;
    uint16              file_title[GUIRICHTEXT_TEXT_MAX_LEN + 1] = {0};
    uint16              tile_len = 0;
    uint16              url_len = 0;
    uint32              alert_time_period = 0;
    wchar               property[MMIBROWSER_MAX_URL_LEN + GUIRICHTEXT_TEXT_MAX_LEN + MMIBROWSER_PROPERTY_EXT_LEN] = {0};
    MMI_STRING_T        str = {0};
    wchar               *property_ptr = PNULL;

    property_ptr = property;

    /* Title: */
    MMIRES_GetText(TXT_COMM_TITLE, PNULL, &str);
    MMIAPICOM_Wstrncpy(property_ptr, str.wstr_ptr, str.wstr_len);
    property_ptr += str.wstr_len;

    property_ptr[0] = '\n';
    property_ptr += 1;

    /* Title */
    if (BRW_GetPageTitle(MMIBROWSER_GetInstance(),file_title,&tile_len,GUIRICHTEXT_TEXT_MAX_LEN))
    {
        MMIAPICOM_Wstrncpy(property_ptr, file_title, tile_len);
        property_ptr += tile_len;
    }
    else
    {
        MMIRES_GetText(TXT_NO_TITLE, PNULL, &str);
        MMIAPICOM_Wstrncpy(property_ptr, str.wstr_ptr, str.wstr_len);
        property_ptr += str.wstr_len;
    }

    property_ptr[0] = '\n';
    property_ptr += 1;

    /* URL: */
    MMIRES_GetText(TXT_INPUT_URL_ADRESS, PNULL, &str);
    MMIAPICOM_Wstrncpy(property_ptr, str.wstr_ptr, str.wstr_len);
    property_ptr += str.wstr_len;

    property_ptr[0] = '\n';
    property_ptr += 1;

    /* URL */

    url_ptr = MMIBROWSER_GetUrl();
    CFLWCHAR_StrNCpyWithAsciiStr(temp_url,url_ptr,MMIBROWSER_MAX_URL_LEN + 1);
    url_len = MMIAPICOM_Wstrlen(temp_url);
    url_len = MIN(url_len, MMIBROWSER_MAX_URL_LEN);

    if (url_len != 0)
    {
        MMIAPICOM_Wstrncpy(property_ptr, temp_url, url_len);
        property_ptr += tile_len;
    }

    str.wstr_ptr = property;
    str.wstr_len = MMIAPICOM_Wstrlen(property);

    MMIPUB_OpenAlertWinByTextPtr(&alert_time_period, &str, PNULL, IMAGE_PUBWIN_SUCCESS, PNULL, PNULL, MMIPUB_SOFTKEY_CUSTOMER, BrwHandleAlertWinMsg);
}
#endif

/*****************************************************************************/
// Description :
// Global resource dependence : none
// Author:Jiaoyou.wu
// Note:    
/*****************************************************************************/
LOCAL MMI_RESULT_E  BrwHandleAlertWinMsg(
                                        MMI_WIN_ID_T     win_id, 
                                        MMI_MESSAGE_ID_E msg_id, 
                                        DPARAM           param 
                                        )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;

    //SCI_TRACE_LOW:"BrwHandleAlertWinMsg msg_id = %x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_10313_112_2_18_2_6_17_227,(uint8*)"d", msg_id);
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            MMI_STRING_T    title_str = {0};

            MMIPUB_SetWinSoftkey(win_id, TXT_NULL, TXT_COMMON_OK, FALSE);

            MMIRES_GetText(TXT_WWW_PAGE_PROPERTY, PNULL, &title_str);
            MMIPUB_SetWinTitleText(win_id, &title_str, FALSE);
        }
        break;

    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
    default:
        recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
        break;
    }

    return (recode);
}
#endif /* MMI_PDA_SUPPORT */

#ifdef BROWSER_START_PAGE_DEFAULT
#ifndef BROWSER_BOOKMARK_HISTORY_TAB
/*****************************************************************************/
//  Description : UpdateStartPageListInfo
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void UpdateStartPageListInfo(void)
{
    char                        temp_item[MMIBROWSER_START_PAGE_ITEM_INFO_LEN + 1] = {0};
    wchar                       item_info[MMIBROWSER_START_PAGE_ITEM_INFO_LEN + 1] = {0};
    uint16                      num_len = 0;
    GUIITEM_CONTENT_T   item_content = {0};
    GUILIST_ITEM_T  *replace_item_ptr = PNULL;

    //get bookmark info
    SCI_MEMSET(item_info, 0, sizeof(item_info));
    num_len = (uint16)sprintf(temp_item, "(%d/%d)", MMIBRW_BookmarkGetCount(), MMIBRW_BOOKMARK_MAX_ITEM);
    MMI_STRNTOWSTR(item_info, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, (uint8 *)temp_item, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, num_len);
    item_content.item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_content.item_data.text_buffer.wstr_ptr = item_info;
    item_content.item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen(item_info);
    //replase
    replace_item_ptr = GUILIST_GetItemPtrByIndex(MMIBROWSER_START_PAGE_LIST_CTRL_ID, 0);/*lint !e605*/
    SCI_MEMCPY(&(replace_item_ptr->item_data_ptr->item_content[2]), &item_content, sizeof(GUIITEM_CONTENT_T));
    GUILIST_ReplaceItem(MMIBROWSER_START_PAGE_LIST_CTRL_ID, replace_item_ptr, 0);

     //get history info
    SCI_MEMSET(item_info, 0, sizeof(item_info));
    SCI_MEMSET(temp_item, 0, sizeof(temp_item));
    num_len = (uint16)sprintf(temp_item, "(%d/%d)", BRW_VisitHisGetTotal(), BRW_MAX_VISIT_HIS_ITEM_NUM);
    MMI_STRNTOWSTR(item_info, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, (uint8 *)temp_item, MMIBROWSER_START_PAGE_ITEM_INFO_LEN, num_len);
    item_content.item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_content.item_data.text_buffer.wstr_ptr = item_info;
    item_content.item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen(item_info);
    //replase
    replace_item_ptr = GUILIST_GetItemPtrByIndex(MMIBROWSER_START_PAGE_LIST_CTRL_ID, 1);/*lint !e605*/
    SCI_MEMCPY(&(replace_item_ptr->item_data_ptr->item_content[2]), &item_content, sizeof(GUIITEM_CONTENT_T));
    GUILIST_ReplaceItem(MMIBROWSER_START_PAGE_LIST_CTRL_ID, replace_item_ptr, 1);
}
#endif
#endif

/*****************************************************************************/
//  Description : handle save wait window message for save local resource in one web page
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleSaveLocalWaitWin(
                                       MMI_WIN_ID_T win_id, 
                                       MMI_MESSAGE_ID_E msg_id, 
                                       DPARAM param
                                       )
{
    MMI_RESULT_E    result   = MMI_RESULT_TRUE;  
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, PNULL);
        break;
    
    case MSG_GET_FOCUS:
        MMIDEFAULT_AllowOpenKeylocWin(FALSE);
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, PNULL);
        break;

    case MSG_APP_RED:
        break;      

    case MSG_CLOSE_WINDOW:
        MMIBROWSER_UpdateFileNameReq(PNULL);
        MMIDEFAULT_AllowOpenKeylocWin(TRUE);
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, PNULL);
        break;

    case MSG_BRW_LOCAL_SAVE:
    {
#ifdef MMI_GUI_STYLE_TYPICAL
        MMI_WIN_ID_T    alert_win_id = MMIBROWSER_ALERT_WIN_ID;
#endif /* MMI_GUI_STYLE_TYPICAL */

        MMIBROWSER_UpdateFileNameReq(PNULL);
#ifdef MMI_GUI_STYLE_TYPICAL
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_DOWNLOAD_SUCCEED,PNULL,IMAGE_PUBWIN_SUCCESS,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
#endif /* MMI_GUI_STYLE_TYPICAL */
        MMK_CloseWin(win_id);
    }
        break;
        
    default:
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, PNULL);
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : save callback func for local resouce(img...) in web page
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void EnterFileNmForLocalCbf(uint32 handle,const uint16* file_name_ptr,uint16 file_name_len, BOOLEAN result)
{                                                               
    MMI_WIN_ID_T    waiting_win_id = MMIBROWSER_SAVE_WAIT_WIN_ID;
    MMI_STRING_T wait_str = {0};
    uint8 *buf_ptr = PNULL;
    uint32 to_write_size = 0;
    BOOLEAN write_result = FALSE;

    if (result && (file_name_ptr != PNULL) && (file_name_len > 0))
    {
        if (MMIBROWSER_SAVE_HANDLE_FOCUS == handle)
        {
            BRW_GetFocusLinkInfo(MMIBROWSER_GetInstance(), PNULL, 0, PNULL, &buf_ptr, &to_write_size);
        }
        else
        {
            BRW_GetTpPointInfo(MMIBROWSER_GetInstance(), PNULL, 0, PNULL, &buf_ptr, &to_write_size, 0, 0);
        }

        if ((buf_ptr != PNULL) && (to_write_size > 0))
        {
            MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_str); 
            MMIPUB_OpenWaitWin(1,&wait_str,PNULL,PNULL,waiting_win_id,IMAGE_NULL,
                ANIM_PUBWIN_WAIT,WIN_ONE_LEVEL,MMIPUB_SOFTKEY_NONE,(MMIPUB_HANDLE_FUNC)BrwHandleSaveLocalWaitWin);
                
            write_result = MMIAPIFMM_WriteFilesDataAsyn(file_name_ptr, file_name_len, buf_ptr, to_write_size, waiting_win_id, MSG_BRW_LOCAL_SAVE);
                
            if (!write_result)
            {
                MMK_CloseWin(waiting_win_id);
            }
            else
            {
                //handle in waiting_win_id
            }
        }
        else//ERROR
        {
        }
    }
    else//ERROR
    {
    }

    if (!write_result)
    {
        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
        MMIBROWSER_UpdateFileNameReq(PNULL);
    }
    
}

#ifdef BROWSER_OPERA_START_PAGE
/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL int32 GetFavoriteIndexFromCtrlId(MMI_CTRL_ID_T ctrl_id)
{
    int32   i = 0;
    
    for (i = 0; i <MMIBRW_FAVORIT_MAX_ITEM; i++)
    {    
        if (ctrl_id == s_browser_favorite_button_ctrl[i])
        {
            return i;
        }
    }
    
    return -1;
}

/*****************************************************************************/
//  Description : the callback of favorite button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButtonCallBack(MMI_CTRL_ID_T ctrl_id)
{
    int32   favorite_index = 0;

    favorite_index = GetFavoriteIndexFromCtrlId(ctrl_id);
    if ((favorite_index >= 0) && (favorite_index < MMIBRW_FAVORIT_MAX_ITEM ))
    {
        MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
        
        MMIBROWSER_GetFavoriteInfo(&nv_info);
        
        if (0 != nv_info.item_info[favorite_index].img_id)
        {
            if (Brw_IsIncludeInvalidUrl((const uint8*)(nv_info.item_info[favorite_index].url_arr)))
            {
                //alert the url is invalid
                MMIBROWSER_AlertUrlInvalid();
            }
            else//enter new url
            {
                MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, (uint8*)(nv_info.item_info[favorite_index].url_arr), PNULL);
            }
            return MMI_RESULT_TRUE;//end
        }
        else//add
        {
            MMIBROWSER_FAVORITE_EDIT_INFO_T *edit_info_ptr = PNULL;

            edit_info_ptr = SCI_ALLOCA(sizeof(MMIBROWSER_FAVORITE_EDIT_INFO_T));
            
            if (edit_info_ptr != PNULL)
            {
                SCI_MEMSET(edit_info_ptr, 0x00, sizeof(MMIBROWSER_FAVORITE_EDIT_INFO_T));
                edit_info_ptr->is_add_new = TRUE;
                edit_info_ptr->favorite_index = favorite_index;
                MMK_CreateWin((uint32 *)MMIBROWSER_FAVORITE_EDIT_WIN_TAB, (ADD_DATA)edit_info_ptr);
                return MMI_RESULT_TRUE;//end
            }
        }
    }

    //handle error
    MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);

    return MMI_RESULT_TRUE;//end
}

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton11CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON11_CTRL_ID);
}
/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton12CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON12_CTRL_ID);
}/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton13CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON13_CTRL_ID);
}

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton21CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON21_CTRL_ID);
}
/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton22CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON22_CTRL_ID);
}/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton23CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON23_CTRL_ID);
}
/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton31CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON31_CTRL_ID);
}
/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton32CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON32_CTRL_ID);
}

/*****************************************************************************/
//  Description : the callback of button
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E FavoriteButton33CallBack(void)
{
    return FavoriteButtonCallBack(MMIBROWSER_FAVORITE_BUTTON33_CTRL_ID);
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void BrwFavoriteResetButtonInfo(void)
{
    uint32  i = 0;
    MMI_CTRL_ID_T   button_ctrl_id  = 0;
    MMI_CTRL_ID_T   label_ctrl_id  = 0;
    MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
    MMI_STRING_T text ={0};
    GUI_BG_T    bg = {0};

    MMIBROWSER_GetFavoriteInfo(&nv_info);

    bg.bg_type = GUI_BG_IMG;
    for (i = 0; i < sizeof(s_browser_favorite_button_ctrl)/sizeof(s_browser_favorite_button_ctrl[0]); i++)
    {
        button_ctrl_id = s_browser_favorite_button_ctrl[i];
        label_ctrl_id = s_browser_favorite_label_ctrl[i];
        if (nv_info.item_info[i].img_id != 0)//img_id is the flag
        {
            bg.img_id = nv_info.item_info[i].img_id;
            
            text.wstr_ptr = nv_info.item_info[i].title_warr;
            text.wstr_len = MMIAPICOM_Wstrlen(nv_info.item_info[i].title_warr);
        }
        else
        {
            bg.img_id = IMAGE_BROWSER_FAVORITE_ADD_ICON;
            text.wstr_ptr = L" ";
            text.wstr_len = 1;
        }
        GUIBUTTON_SetBg(button_ctrl_id, &bg);
        GUIBUTTON_SetPressedBg(button_ctrl_id, &bg);
        GUILABEL_SetText(label_ctrl_id, &text, FALSE);

    }
    

}
/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwOperaStartPageWin(MMI_WIN_ID_T win_id)
{
#ifdef BROWSER_SEARCH_SUPPORT
    GUIFORM_CHILD_WIDTH_T   from_width = {0};
#endif    
    GUIFORM_CHILD_HEIGHT_T  label_height = {0};
    GUI_BG_T    bg = {0};
    uint16  hor_space = 0;
    uint16  ver_space = 0;
    GUI_BORDER_T    border = {0};

    //parent
    GUIFORM_IsSlide(MMIBROWSER_START_PAGE_FORM_CTRL_ID, FALSE);
    GUIFORM_PermitChildBg(MMIBROWSER_START_PAGE_FORM_CTRL_ID, FALSE);
    GUIFORM_SetMargin(MMIBROWSER_START_PAGE_FORM_CTRL_ID, 0);
    #if 0
    GUIFORM_SetActiveChildTheme(MMIBROWSER_START_PAGE_FORM_CTRL_ID, PNULL, &border, PNULL);
    border.color = MMI_BLUE_COLOR;
    border.type = GUI_BORDER_ROUNDED;
    border.width = 2;
    GUIFORM_SetChildTheme(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, PNULL, &border, PNULL);
    #endif
    //form1
    SCI_MEMSET(&bg, 0x00, sizeof(bg));
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_SEARCH_BAR_BG;
    GUIFORM_SetBg( MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &bg);

    label_height.type     = GUIFORM_CHLID_HEIGHT_FIXED;
    label_height.add_data = MMIBROWSER_STARTPAGE_FORM1_HEIGHT;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM_CTRL_ID, MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &label_height);

#ifdef BROWSER_SEARCH_SUPPORT
    from_width.type = GUIFORM_CHLID_WIDTH_PERCENT;
    from_width.add_data = 70;
    GUIFORM_SetChildWidth(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &from_width);

    label_height.type     = GUIFORM_CHLID_HEIGHT_PERCENT;
    label_height.add_data = 95;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &label_height);
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, &label_height);

    hor_space = 10;
    GUIFORM_SetSpace(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &hor_space, PNULL);
#endif

    GUILABEL_SetIcon(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, IMAGE_BROWSER_ADDRESS_ICON);
    GUILABEL_SetTextById(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, TXT_WWW_INPUT_URL, FALSE);
    GUILABEL_SetFont(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID,MMI_DEFAULT_NORMAL_FONT, MMI_BLACK_COLOR);

    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
    GUILABEL_SetBg(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &bg);

#ifdef BROWSER_SEARCH_SUPPORT
    GUILABEL_SetIcon(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, IMAGE_BROWSER_SEARCH_ICON);
    GUILABEL_SetTextById(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, STXT_SEARCH, FALSE);
    GUILABEL_SetFont(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID,MMI_DEFAULT_NORMAL_FONT, MMI_BLACK_COLOR);
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
    GUILABEL_SetBg(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, &bg);
#endif

    //for some blank space
    label_height.type     = GUIFORM_CHLID_HEIGHT_FIXED;
    label_height.add_data = 14;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM_CTRL_ID, MMIBROWSER_START_PAGE_FORM3_CTRL_ID, &label_height);

    //form2
    label_height.type     = GUIFORM_CHLID_HEIGHT_LEFT;
    label_height.add_data = 0;
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM_CTRL_ID, MMIBROWSER_START_PAGE_FORM2_CTRL_ID, &label_height);
    GUIFORM_SetMargin(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, 10);
    {
        uint32  i = 0;
        uint32  j = 0;
        MMI_CTRL_ID_T   button_ctrl_id  = 0;
        MMI_CTRL_ID_T   label_ctrl_id  = 0;
        MMI_CTRL_ID_T   parent1_form_ctrl_id  = 0;
        MMI_CTRL_ID_T   parent11_form_ctrl_id  = 0;
        MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
        MMI_STRING_T text ={0};
        uint32  favorite_index = 0;
        
        hor_space = 0;
        ver_space = 0;
        border.color = MMI_DARK_GRAY_COLOR;
        border.type = GUI_BORDER_SROUNDED;
        border.width = 1;
        MMIBROWSER_GetFavoriteInfo(&nv_info);

        for (i = 0; i < 3; i++)
        {
            parent1_form_ctrl_id = s_browser_favorite_form_ctrl[i];
            
            hor_space = 8;
            GUIFORM_SetChildSpace(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, parent1_form_ctrl_id, &hor_space, &ver_space);
            GUIFORM_SetMargin(parent1_form_ctrl_id, 0);
            GUIFORM_PermitChildBg(parent1_form_ctrl_id, FALSE);
            GUIFORM_PermitChildFont(parent1_form_ctrl_id, FALSE);
            //GUIFORM_PermitChildBorder(parent1_form_ctrl_id, FALSE);
            
            for (j = 0; j < 3; j++)
            {
                favorite_index = 3*j + i;
                button_ctrl_id  = s_browser_favorite_button_ctrl[favorite_index];
                label_ctrl_id  = s_browser_favorite_label_ctrl[favorite_index];
                parent11_form_ctrl_id  = s_browser_favorite_child_form_ctrl[favorite_index];
                
                label_height.type     = GUIFORM_CHLID_HEIGHT_FIXED;
                label_height.add_data = 120;
                GUIFORM_SetChildHeight(parent1_form_ctrl_id, parent11_form_ctrl_id, &label_height);

                GUIFORM_SetChildSpace(parent1_form_ctrl_id, parent11_form_ctrl_id, &hor_space, &ver_space);
                GUIFORM_SetMargin(parent11_form_ctrl_id, 0);
                GUIFORM_PermitChildBg(parent11_form_ctrl_id, FALSE);
                GUIFORM_SetStyle(parent11_form_ctrl_id,GUIFORM_STYLE_UNIT);
          
                label_height.type     = GUIFORM_CHLID_HEIGHT_FIXED;
                label_height.add_data = 90;
                GUIFORM_SetChildHeight(parent11_form_ctrl_id, button_ctrl_id, &label_height);

                if (nv_info.item_info[favorite_index].img_id != 0)//img_id is the flag
                {
                    bg.bg_type = GUI_BG_IMG;
                    bg.img_id = nv_info.item_info[favorite_index].img_id;
                    GUIBUTTON_SetBg(button_ctrl_id, &bg);
                    GUIBUTTON_SetPressedBg(button_ctrl_id, &bg);
                    
                    text.wstr_ptr = nv_info.item_info[favorite_index].title_warr;
                    text.wstr_len = MMIAPICOM_Wstrlen(nv_info.item_info[favorite_index].title_warr);
                    GUILABEL_SetText(label_ctrl_id, &text, FALSE);
                }
                GUILABEL_SetFont(label_ctrl_id, SONG_FONT_14, MMI_BLACK_COLOR);
                GUIBUTTON_SetCallBackFunc(button_ctrl_id, s_browser_favorite_button_cb[favorite_index]);
                GUIBUTTON_SetRunSheen(button_ctrl_id, TRUE);
                GUIBUTTON_PermitBorder(button_ctrl_id, TRUE);
                IGUICTRL_SetBorder(MMK_GetCtrlPtr(button_ctrl_id),  &border);

            }
        }
        

    }

}


/*****************************************************************************/
//  Description : handle opera Start page of the browser
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleOperaStartPageWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    ret=MMI_RESULT_TRUE;    
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_PDA_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        InitBrwOperaStartPageWin(win_id);
        MMIBROWSER_SetIsResetFavoriteInfo(FALSE);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_START_PAGE_FORM1_CTRL_ID);
        break;

#ifdef WIN32
    case MSG_APP_1:
        MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, "http://172.16.14.173:8082/", PNULL);
#endif

    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    {
        MMI_CTRL_ID_T       ctrl_id = 0;

        if (PNULL != param)
        {
            ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

            if (ctrl_id == MMIBROWSER_START_PAGE_LABEL11_CTRL_ID)
            {
                MMIBROWSER_InputUrl(PNULL);
            }
            else if (ctrl_id == MMIBROWSER_START_PAGE_LABEL12_CTRL_ID)
            {
#ifdef BROWSER_SEARCH_SUPPORT
                MMIBROWSER_OpenSearchWin();
#endif
            }
        }
    }
        break;

    case MSG_TP_PRESS_LONG:
    {
        GUI_POINT_T point = {0};
        uint32  i = 0;
        
        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);
        
        //open the current favorite popmenu
        for (i = 0; i < sizeof(s_browser_favorite_button_ctrl)/sizeof(s_browser_favorite_button_ctrl[0]); i++)
        {
            if (MMK_PosIsCtrl(s_browser_favorite_button_ctrl[i], point))
            {
                int32   favorite_index = 0;

                favorite_index = GetFavoriteIndexFromCtrlId(s_browser_favorite_button_ctrl[i]);
                if ((favorite_index >= 0) && (favorite_index < MMIBRW_FAVORIT_MAX_ITEM ))
                {
                    MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
                    
                    MMIBROWSER_GetFavoriteInfo(&nv_info);
                    
                    if (0 != nv_info.item_info[favorite_index].img_id)
                    {
                        MMK_CreateWin((uint32 *)MMIBROWSER_FAVORITE_POPUP_WIN_TAB, (ADD_DATA)favorite_index);
                    }
                    else
                    {
                        //do nothing
                    }
                }
                break;
            }                
        }  
    }        
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
        MMK_CreateWin((uint32*)MMIBROWSER_STARTPAGE_POPMENU_WIN_TAB,PNULL);
        GUIMENU_CreatDynamic(PNULL,MMIBROWSER_STARTPAGE_POPMENU_WIN_ID,MMIBROWSER_MENU_CTRL_ID,GUIMENU_STYLE_POPUP);
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMIBROWSER_QueryExit();
        break;

    case MSG_GET_FOCUS:
        if (MMIBROWSER_GetIsResetFavoriteInfo())
        {
            BrwFavoriteResetButtonInfo();
            MMIBROWSER_SetIsResetFavoriteInfo(FALSE);
        }
        break;
        
    case MSG_CLOSE_WINDOW:
        MMIBROWSER_Exit();
        break;
    
    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}
/*****************************************************************************/
//  Description : handle bookmark edit window message
//  Global resource dependence : 
//  Author: li.li
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFavoriteEditWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    MMIBROWSER_FAVORITE_EDIT_INFO_T *edit_info_ptr = PNULL;

    switch(msg_id) 
    {
    case MSG_OPEN_WINDOW:
    {
        MMI_TEXT_ID_T   title_text_id = TXT_NULL;
        
        edit_info_ptr = (MMIBROWSER_FAVORITE_EDIT_INFO_T*)MMK_GetWinAddDataPtr(win_id);
        
#ifdef MMI_PDA_SUPPORT
        GUIFORM_SetType(MMIBROWSER_FAVORITE_EDIT_CTRL_ID, GUIFORM_TYPE_TP);
        GUIEDIT_SetAlign(MMIBROWSER_FAVORITE_EDIT_TITLE_CTRL_ID, ALIGN_LVMIDDLE);
        GUIEDIT_SetAlign(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID, ALIGN_LVMIDDLE);
#endif /* MMI_PDA_SUPPORT */

        if ((PNULL == edit_info_ptr) || edit_info_ptr->is_add_new)
        {
            title_text_id = STXT_ADD;
            GUIEDIT_SetTextMaxLen(MMIBROWSER_FAVORITE_EDIT_TITLE_CTRL_ID,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH);
            GUIEDIT_SetTextMaxLen(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID,MMIBRW_BOOKMARK_MAX_URL_LENGTH,MMIBRW_BOOKMARK_MAX_URL_LENGTH);
        }
        else
        {
            title_text_id = TXT_EDIT;
            //max len has set outside
        }
        
        GUIWIN_SetTitleTextId(win_id, title_text_id, FALSE);

        GUIFORM_SetStyle(MMIBROWSER_FAVORITE_EDIT_FORM1_CTRL_ID,GUIFORM_STYLE_UNIT);
        GUIFORM_SetStyle(MMIBROWSER_FAVORITE_EDIT_FORM2_CTRL_ID,GUIFORM_STYLE_UNIT);
            
        GUILABEL_SetTextById(MMIBROWSER_FAVORITE_EDIT_LABEL1_CTRL_ID, TXT_BROWSER_FILE_TITLE, FALSE);
        GUILABEL_SetTextById(MMIBROWSER_FAVORITE_EDIT_LABEL2_CTRL_ID, TXT_URL_ADRESS, FALSE);
        GUIEDIT_SetIm(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID,(uint32)GUIIM_TYPE_ENGLISH | (uint32)GUIIM_TYPE_DIGITAL | (uint32)GUIIM_TYPE_ABC,GUIIM_TYPE_ABC);//MS00220112
        GUIEDIT_SetImTag(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID, GUIIM_TAG_URL);        

        MMK_SetAtvCtrl(win_id,MMIBROWSER_FAVORITE_EDIT_CTRL_ID);
    }
        break;
        
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        {
            MMI_CTRL_ID_T       ctrl_id = 0;

            if (PNULL != param)
            {
		        ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

                if (ctrl_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                }
                else if (ctrl_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                }
                else if (ctrl_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_MIDSK, PNULL, 0);
                }
            }
        }
        break;
#endif /* MMI_PDA_SUPPORT */

    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
#ifndef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK: 
#endif /* MMI_PDA_SUPPORT */
    case MSG_CTL_OK:
    case MSG_APP_OK:
        edit_info_ptr = (MMIBROWSER_FAVORITE_EDIT_INFO_T*)MMK_GetWinAddDataPtr(win_id);

        if (edit_info_ptr != PNULL)
        {
            MMI_STRING_T url_text_str = {0};
            MMI_STRING_T title_text_str = {0};
            
            GUIEDIT_GetString(MMIBROWSER_FAVORITE_EDIT_TITLE_CTRL_ID, &title_text_str);
            GUIEDIT_GetString(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID, &url_text_str);

            if ((PNULL == title_text_str.wstr_ptr) || (0 == title_text_str.wstr_len))
            {
                MMIPUB_OpenAlertWarningWin(TXT_COMM_NO_TITLE);
            }
            else if ((PNULL == url_text_str.wstr_ptr) || (0 == url_text_str.wstr_len))
            {
                MMIPUB_OpenAlertWarningWin(TXT_COMM_URL_NULL);
            }
            else
            {
                uint8    url_arr[MMIBRW_BOOKMARK_MAX_URL_LENGTH+1] ={0};   
                
                MMI_WSTRNTOSTR(url_arr, MMIBRW_BOOKMARK_MAX_URL_LENGTH,
                            url_text_str.wstr_ptr, url_text_str.wstr_len, 
                            MIN(MMIBRW_BOOKMARK_MAX_URL_LENGTH, url_text_str.wstr_len));
                            
                if (Brw_IsIncludeInvalidUrl(url_arr))
                {
                    MMIBROWSER_AlertUrlInvalid();
                }
                else
                {
                    MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
                    uint16  edit_index = 0;

                    MMIBROWSER_GetFavoriteInfo(&nv_info);

                    edit_index = edit_info_ptr->favorite_index;
                    if ((edit_index >= MMIBRW_FAVORIT_MAX_ITEM ))//protect
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
                        MMK_CloseWin(win_id);
                        break;
                    }
                    
                    if (edit_info_ptr->is_add_new)
                    {
                        nv_info.item_count++;
                        //SCI_TRACE_LOW:"nv_info.item_count=%d"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_11085_112_2_18_2_6_19_228,(uint8*)"d", nv_info.item_count);
                    }
                    else
                    {
#ifdef BROWSER_OPERA_FAVORITE_WITH_DEFAULT//the default can't be deleted                    
                        if (nv_info.item_info[edit_index].is_default)//protect
                        {
                            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
                            MMK_CloseWin(win_id);
                            break;
                        }
#endif
                    }
                    
                    SCI_MEMSET(&(nv_info.item_info[edit_index]), 0x00, sizeof(MMIBRW_FAVORITE_ITEM_T));
                    nv_info.item_info[edit_index].img_id = IMAGE_BROWSER_DEFAULT_FAVORITE_ICON;
#ifdef BROWSER_OPERA_FAVORITE_WITH_DEFAULT              
                    nv_info.item_info[edit_index].is_default = FALSE;
#endif                    
                    MMI_WSTRNCPY(nv_info.item_info[edit_index].title_warr, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH, 
                        title_text_str.wstr_ptr, title_text_str.wstr_len,
                        MIN(title_text_str.wstr_len, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH));
                    SCI_MEMCPY(nv_info.item_info[edit_index].url_arr, url_arr, MIN(url_text_str.wstr_len, MMIBRW_BOOKMARK_MAX_URL_LENGTH));
    
                    MMIBROWSER_SetFavoriteInfo(&nv_info);
                    {
                        GUI_BG_T bg = {0};
                        MMI_CTRL_ID_T   button_ctrl_id = 0;
                        MMI_STRING_T text ={0};

                        button_ctrl_id = s_browser_favorite_button_ctrl[edit_index];
                        bg.bg_type = GUI_BG_IMG;
                        bg.img_id = nv_info.item_info[edit_index].img_id;
                        GUIBUTTON_SetBg(button_ctrl_id, &bg);
                        
                        text.wstr_ptr = nv_info.item_info[edit_index].title_warr;
                        text.wstr_len = MMIAPICOM_Wstrlen(nv_info.item_info[edit_index].title_warr);
                        GUILABEL_SetText(s_browser_favorite_label_ctrl[edit_index], &text, FALSE);
                    }
                }
                
#ifdef MMI_GUI_STYLE_TYPICAL
                MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
#endif /* MMI_GUI_STYLE_TYPICAL */
                MMK_CloseWin(win_id);
            }
                
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
            MMK_CloseWin(win_id);
        }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        MMK_FreeWinAddData(win_id);
        break;
    
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

/****************************************************************************/
//  Description :
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/****************************************************************************/
LOCAL void  BrwCreateFavoritePopUpMenu(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, uint16  favorite_index)
{
    uint32                      num = 0;
    MMI_STRING_T                title_str = {0};
    MMI_TEXT_ID_T               text_index_arr[] = {TXT_DELETE, TXT_EDIT, TXT_COMM_DETAIL};
    uint32                      node_index_arr[] = {ID_POPUP_MENU_FAVORITE_DELETE, ID_POPUP_MENU_FAVORITE_EDIT, ID_POPUP_MENU_FAVORITE_DETAIL};
    MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};

    MMIBROWSER_GetFavoriteInfo(&nv_info);

    if ((favorite_index >= MMIBRW_FAVORIT_MAX_ITEM ))//protect
    {
        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
        MMK_CloseWin(win_id);
        return;
    }

    title_str.wstr_ptr = nv_info.item_info[favorite_index].title_warr;
    title_str.wstr_len = MMIAPICOM_Wstrlen(title_str.wstr_ptr);

#ifdef BROWSER_OPERA_FAVORITE_WITH_DEFAULT              
    if (nv_info.item_info[favorite_index].is_default)
    {
        num = sizeof(node_index_arr) / sizeof(node_index_arr[0]) - 2;
        MMIBROWSER_CreatePopupAutoMenu(win_id, ctrl_id, &title_str, num, (text_index_arr+2), (node_index_arr+2));
    }
    else
#endif    
    {
        num = sizeof(node_index_arr) / sizeof(node_index_arr[0]);
        MMIBROWSER_CreatePopupAutoMenu(win_id, ctrl_id, &title_str, num, text_index_arr, node_index_arr);
    }
}

/*****************************************************************************/
//  Description :
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFavoritePopUpWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    uint16  favorite_index = (uint32)MMK_GetWinAddDataPtr(win_id);
    MMI_WIN_ID_T query_win_id = MMIBROWSER_QUERY_WIN_ID;

    switch (msg_id) 
    {
    case MSG_OPEN_WINDOW:
        BrwCreateFavoritePopUpMenu(win_id, MMIBROWSER_FAVORITE_POPUP_MENU_CTRL_ID, favorite_index);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_FAVORITE_POPUP_MENU_CTRL_ID);
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        {
            MMI_MENU_ID_T               menu_id = 0;
            menu_id = GUIMENU_GetCurNodeId(MMIBROWSER_FAVORITE_POPUP_MENU_CTRL_ID);
            switch (menu_id)
            {
            case ID_POPUP_MENU_FAVORITE_DELETE:
                MMIBROWSER_OpenQueryWinByTextId(TXT_DELETEQUERY,IMAGE_PUBWIN_QUERY,&query_win_id,PNULL);
                break;
                
            case ID_POPUP_MENU_FAVORITE_EDIT:
            {
                MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
                
                MMIBROWSER_GetFavoriteInfo(&nv_info);
                   
                if ((favorite_index < MMIBRW_FAVORIT_MAX_ITEM ) && (0 != nv_info.item_info[favorite_index].img_id))
                {
                    MMIBROWSER_FAVORITE_EDIT_INFO_T *edit_info_ptr = PNULL;

                    edit_info_ptr = SCI_ALLOCA(sizeof(MMIBROWSER_FAVORITE_EDIT_INFO_T));//free in edit window
                    
                    if (edit_info_ptr != PNULL)
                    {
                        wchar    url_warr[MMIBRW_BOOKMARK_MAX_URL_LENGTH+1] ={0};   
                        uint16  url_len = 0;
                        
                        SCI_MEMSET(edit_info_ptr, 0x00, sizeof(MMIBROWSER_FAVORITE_EDIT_INFO_T));
                        edit_info_ptr->is_add_new = FALSE;
                        edit_info_ptr->favorite_index = favorite_index;
                        MMK_CreateWin((uint32 *)MMIBROWSER_FAVORITE_EDIT_WIN_TAB, (ADD_DATA)edit_info_ptr);
                        //set title
                        GUIEDIT_SetTextMaxLen(MMIBROWSER_FAVORITE_EDIT_TITLE_CTRL_ID,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH);
                        GUIEDIT_SetString(MMIBROWSER_FAVORITE_EDIT_TITLE_CTRL_ID, 
                            nv_info.item_info[favorite_index].title_warr, MMIAPICOM_Wstrlen(nv_info.item_info[favorite_index].title_warr));
                        //set url
                        GUIEDIT_SetTextMaxLen(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID,MMIBRW_BOOKMARK_MAX_URL_LENGTH,MMIBRW_BOOKMARK_MAX_URL_LENGTH);
                        url_len = SCI_STRLEN(nv_info.item_info[favorite_index].url_arr);
                        MMI_STRNTOWSTR(url_warr, MMIBRW_BOOKMARK_MAX_URL_LENGTH,
                                (const uint8*)(nv_info.item_info[favorite_index].url_arr), url_len,
                                MIN(MMIBRW_BOOKMARK_MAX_URL_LENGTH, url_len));
                        GUIEDIT_SetString(MMIBROWSER_FAVORITE_EDIT_URL_CTRL_ID, url_warr, MMIAPICOM_Wstrlen(url_warr));
                        
                        MMK_CloseWin(win_id);
                        break;
                    }
                    else//error
                    {
                    }
                }
                else//error
                {
                }
                
                //handle error
                MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
                MMK_CloseWin(win_id);
            }
                break;
                
            case ID_POPUP_MENU_FAVORITE_DETAIL:
                MMK_CreateWin((uint32 *)MMIBROWSER_FAVORITE_DETAIL_WIN_TAB, (ADD_DATA)favorite_index);
                MMK_CloseWin(win_id);
                break;

            default:
                break;
            }
            break;
        }

    case MSG_PROMPTWIN_OK:
    {
        MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
        
        MMIBROWSER_GetFavoriteInfo(&nv_info);
           
        if ((favorite_index < MMIBRW_FAVORIT_MAX_ITEM ) && (0 != nv_info.item_info[favorite_index].img_id))
        {
#ifdef BROWSER_OPERA_FAVORITE_WITH_DEFAULT              
            if (!nv_info.item_info[favorite_index].is_default)//the default can't be deleted
#endif            
            {
                GUI_BG_T bg = {0};
                MMI_CTRL_ID_T   button_ctrl_id = 0;
                MMI_STRING_T text ={0};
                
                //delete
                nv_info.item_count--;
                SCI_MEMSET(&(nv_info.item_info[favorite_index]), 0x00, sizeof(MMIBRW_FAVORITE_ITEM_T));
                MMIBROWSER_SetFavoriteInfo(&nv_info);
                //update the control info
                button_ctrl_id = s_browser_favorite_button_ctrl[favorite_index];
                bg.bg_type = GUI_BG_IMG;
                bg.img_id = IMAGE_BROWSER_FAVORITE_ADD_ICON;
                GUIBUTTON_SetBg(button_ctrl_id, &bg);
                GUIBUTTON_SetPressedBg(button_ctrl_id, &bg);
                text.wstr_ptr = L" ";
                text.wstr_len = 1;
                GUILABEL_SetText(s_browser_favorite_label_ctrl[favorite_index], &text, FALSE);

                MMIBROWSER_CloseQuerytWin(&query_win_id);
                MMK_CloseWin(win_id);
                break;
            }
        }
        else//error
        {
        }
        
        //handle error
        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
                
        MMIBROWSER_CloseQuerytWin(&query_win_id);
        MMK_CloseWin(win_id);
    }
        break;

    case MSG_PROMPTWIN_CANCEL:
        MMIBROWSER_CloseQuerytWin(&query_win_id);
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
//  Description : set the detail window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwSetFavoriteDetailWindow(MMI_CTRL_ID_T ctrl_id, uint16 favorite_index)
{
    GUIRICHTEXT_ITEM_T  item_data = {0};
    uint16              index = 0;
    MMIBRW_FAVORITE_CONTEXT_T   nv_info = {0};
    wchar    url_warr[MMIBRW_BOOKMARK_MAX_URL_LENGTH+1] ={0};   
    uint16  len = 0;
    
    MMIBROWSER_GetFavoriteInfo(&nv_info);
    if ((favorite_index >= MMIBRW_FAVORIT_MAX_ITEM ) || (0 == nv_info.item_info[favorite_index].img_id))
    {
        return FALSE;  
    }

    /* Title */
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_COMM_TITLE;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    /* Title info*/
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    len = MMIAPICOM_Wstrlen(nv_info.item_info[favorite_index].title_warr);
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    if (len > 0)
    {
        item_data.text_data.buf.len = len;
        item_data.text_data.buf.str_ptr = nv_info.item_info[favorite_index].title_warr;
    }
    else
    {
        item_data.text_data.buf.len = 1;
        item_data.text_data.buf.str_ptr = L" ";
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    /* Url*/
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_INPUT_URL_ADRESS;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    /* Url info*/
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    
    len = SCI_STRLEN(nv_info.item_info[favorite_index].url_arr);
    MMI_STRNTOWSTR(url_warr, MMIBRW_BOOKMARK_MAX_URL_LENGTH,
            (const uint8*)(nv_info.item_info[favorite_index].url_arr), len,
            MIN(MMIBRW_BOOKMARK_MAX_URL_LENGTH, len));
    if (len > 0)
    {
        item_data.text_type = GUIRICHTEXT_TEXT_BUF;
        item_data.text_data.buf.len = len;
        item_data.text_data.buf.str_ptr = url_warr;
    }
    else
    {
        item_data.text_type = GUIRICHTEXT_TEXT_BUF;
        item_data.text_data.buf.len = 1;
        item_data.text_data.buf.str_ptr = L" ";
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    return TRUE;  
}

/*****************************************************************************/
//  Description : handle file detail window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFavoriteDetailWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    switch(msg_id) 
    {
    case MSG_OPEN_WINDOW:
    {
        uint16  favorite_index = (uint32)MMK_GetWinAddDataPtr(win_id);
        
        MMK_SetAtvCtrl(win_id,MMIBROWSER_FAVORITE_DETAIL_CTRL_ID);
        if (BrwSetFavoriteDetailWindow(MMIBROWSER_FAVORITE_DETAIL_CTRL_ID, favorite_index))
        {
        }
        else
        {
            MMK_CloseWin(win_id);
        }
    }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

#endif

#ifdef BROWSER_BOOKMARK_HISTORY_TAB
/*****************************************************************************/
// 	Description : add all tabs and all child windows to parent window.
//	Global resource dependence : 
//  Author: fen.xie
//	Note: 
/*****************************************************************************/
LOCAL void  AddTabsAndChildWindows(MMIBROWSER_TAB1_TYPE_E tab_child_type)
{
    MMI_CTRL_ID_T   tab_ctrl_id = MMIBROWSER_BK_AND_HISTORY_TAB_CTRL_ID;
    MMI_STRING_T    str_text = {0};

    if (tab_child_type >= MMIBROWSER_TAB1_MAX)
    {
        //SCI_TRACE_LOW:"[BRW]AddTabsAndChildWindows:tab_child_type=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_11472_112_2_18_2_6_19_229,(uint8*)"d", tab_child_type);
        tab_child_type = MMIBROWSER_TAB1_FIRST;//the first
    }
    
    GUITAB_SetMaxItemNum(tab_ctrl_id, MMIBROWSER_TAB1_MAX);

    MMI_GetLabelTextByLang(TXT_BOOKMARK, &str_text);
#ifdef MAINLCD_SIZE_320X480
    GUITAB_AppendSimpleItem(tab_ctrl_id, &str_text, IMAGE_BROWSER_TAB_BOOKMARK_ICON_SELECTED, IMAGE_BROWSER_TAB_BOOKMARK_ICON_UNSELECTED);
#else
    GUITAB_AppendSimpleItem(tab_ctrl_id, &str_text, IMAGE_NULL, IMAGE_NULL);
#endif
    GUITAB_AddChildWindow(tab_ctrl_id, MMIBROWSER_BOOKMARK_WIN_ID, MMIBROWSER_TAB1_BOOKMARK);

    MMI_GetLabelTextByLang(TXT_COMMON_HISTORY, &str_text);
#ifdef MAINLCD_SIZE_320X480
    GUITAB_AppendSimpleItem(tab_ctrl_id, &str_text, IMAGE_BROWSER_TAB_HISTORY_ICON_SELECTED, IMAGE_BROWSER_TAB_HISTORY_ICON_UNSELECTED);
#else
    GUITAB_AppendSimpleItem(tab_ctrl_id, &str_text, IMAGE_NULL, IMAGE_NULL);
#endif
    GUITAB_AddChildWindow(tab_ctrl_id, MMIBROWSER_HISTORYLIST_WIN_ID, MMIBROWSER_TAB1_HISTORY);

    GUITAB_SetCurSel(tab_ctrl_id, tab_child_type);

    if (MMIBROWSER_TAB1_HISTORY == tab_child_type)
    {
        GUIWIN_SetTitleButtonTextId(MMIBROWSER_HISTORYLIST_WIN_ID, GUITITLE_BUTTON_RIGHT, TXT_DELALL, FALSE);
    }
    else
    {
        GUIWIN_SetTitleButtonTextId(MMIBROWSER_BOOKMARK_WIN_ID, GUITITLE_BUTTON_RIGHT, STXT_OPTION, FALSE);
    }
}

/*****************************************************************************/
// 	Description : to handle tab parent window message
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleBkAndHistoryWindow(
                    MMI_WIN_ID_T win_id, 
                    MMI_MESSAGE_ID_E msg_id, 
                    DPARAM param
                    )
{
    MMI_RESULT_E    recode = MMI_RESULT_TRUE;
    MMIBROWSER_TAB1_TYPE_E tab_child_type = (uint32)MMK_GetWinAddDataPtr(win_id);
    uint32  cur_select = 0;
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id, MMIBROWSER_BK_AND_HISTORY_TAB_CTRL_ID);
        AddTabsAndChildWindows(tab_child_type);
        break;
        
    case MSG_CTL_CANCEL:
        MMK_CloseParentWin(win_id);
        break;

    case MSG_NOTIFY_TAB_SWITCH:
        cur_select = GUITAB_GetCurSel(MMIBROWSER_BK_AND_HISTORY_TAB_CTRL_ID);
        if (0 == cur_select)
        {
			GUIWIN_SetTitleButtonTextId(MMIBROWSER_BOOKMARK_WIN_ID, GUITITLE_BUTTON_RIGHT, STXT_OPTION, FALSE);
        }
        else
        {
			GUIWIN_SetTitleButtonTextId(MMIBROWSER_HISTORYLIST_WIN_ID, GUITITLE_BUTTON_RIGHT, TXT_DELALL, FALSE);
        }
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

/*****************************************************************************/
// 	Description : MMIBROWSER_OpenBkHistoryWindow
//	Global resource dependence : 
//  Author:fen.xie
//	Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenBkHistoryWindow(MMIBROWSER_TAB1_TYPE_E tab_child_type, ADD_DATA add_data_ptr)
{
    ADD_DATA    bookmark_add_data_ptr = PNULL;
    ADD_DATA    history_add_data_ptr = PNULL;
	MMI_WIN_ID_T    focus_sub_win_id = 0;
    
    if (MMIBROWSER_TAB1_HISTORY == tab_child_type)
    {
        history_add_data_ptr = add_data_ptr;
        focus_sub_win_id = MMIBROWSER_HISTORYLIST_WIN_ID;
    }
    else
    {
        bookmark_add_data_ptr = add_data_ptr;
        focus_sub_win_id = MMIBROWSER_BOOKMARK_WIN_ID;
    }

    if (!MMK_IsOpenWin(MMIBROWSER_BK_AND_HISTORY_WIN_ID))
    {
        MMK_CloseParentWin(MMIBROWSER_BK_AND_HISTORY_WIN_ID);
        MMK_CloseMiddleWin(MMIBROWSER_BOOKMARK_START_WIN_ID, MMIBROWSER_BOOKMARK_END_WIN_ID);
        MMK_CreateParentWin((uint32 *)MMIBROWSER_BK_AND_HISTORY_WIN_TAB, (ADD_DATA)tab_child_type);	
        MMIBROWSER_CreateChildBookmarkWin(MMIBROWSER_BK_AND_HISTORY_WIN_ID, bookmark_add_data_ptr);
        MMIBROWSER_CreateChildHistoryWin(MMIBROWSER_BK_AND_HISTORY_WIN_ID, history_add_data_ptr);
        MMK_SetChildWinFocus(MMIBROWSER_BK_AND_HISTORY_WIN_ID, focus_sub_win_id);
    }
    else
    {
        GUITAB_SetCurSel(MMIBROWSER_BK_AND_HISTORY_TAB_CTRL_ID, tab_child_type);
#ifdef BROWSER_START_PAGE_THEME1
        MMIBROWSER_SetIsHistoryUpdate(TRUE);
        //MMIBROWSER_UpdateBkListToRoot();

        MMIBROWSER_SetSlide(TRUE);
        MMK_SetChildWinFocus(MMIBROWSER_BK_AND_HISTORY_WIN_ID, focus_sub_win_id);
        MMISLIDE_SwitchToWin(&s_startpage_win_slide_handle, MMIBROWSER_BK_AND_HISTORY_WIN_ID);
        MMK_PostMsg(focus_sub_win_id, MSG_FULL_PAINT, PNULL, 0);
#else
        MMK_WinGrabFocus(MMIBROWSER_BK_AND_HISTORY_WIN_ID);
#endif    
    }
}
#endif

#ifdef BROWSER_START_PAGE_THEME1
/*****************************************************************************/
//  Description : init the reference infomation of brower position
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBrwNewStartPageWin(MMI_WIN_ID_T win_id)
{
#ifdef BROWSER_SEARCH_SUPPORT
    GUIFORM_CHILD_WIDTH_T   from_width = {0};
#endif    
    GUIFORM_CHILD_HEIGHT_T      label_height = {0};
    GUI_BG_T                    bg = {0};
    GUI_BOTH_RECT_T both_rect = {0};
    GUI_BOTH_RECT_T client_rect = MMITHEME_GetWinClientBothRect(win_id);
    GUI_FONT_ALL_T font_all = {0};
    uint16  i=0;
    uint16 hor_space = 10;
#ifdef MMI_PDA_SUPPORT
    GUI_BOTH_RECT_T form2_rect = {0};
#endif

    GUIFORM_PermitChildBg(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, FALSE);
    GUIFORM_IsSlide(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, FALSE);
    GUIFORM_SetType(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, GUIFORM_TYPE_TP);    
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_SEARCH_BAR_BG;
    GUIFORM_SetBg( MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &bg);

    both_rect = client_rect;
    both_rect.h_rect.bottom = both_rect.h_rect.top + MMIBROWSER_STARTPAGE_FORM1_HEIGHT - 1;
    both_rect.v_rect.bottom = both_rect.v_rect.top + MMIBROWSER_STARTPAGE_FORM1_HEIGHT - 1;
    GUIAPICTRL_SetBothRect(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &both_rect);
    
    from_width.type = GUIFORM_CHLID_WIDTH_PERCENT;
    from_width.add_data = 11;
    GUIFORM_SetChildWidth(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_BUTTON5_CTRL_ID, &from_width);
    GUIBUTTON_SetCallBackFunc(MMIBROWSER_START_PAGE_BUTTON5_CTRL_ID, BookmarkButtonCallBack);
    label_height.type     = GUIFORM_CHLID_HEIGHT_PERCENT;
    label_height.add_data = 95;
#ifdef BROWSER_SEARCH_SUPPORT
    from_width.type = GUIFORM_CHLID_WIDTH_PERCENT;
    from_width.add_data = 60;
    GUIFORM_SetChildWidth(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &from_width);
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, &label_height);
#endif
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &label_height);
    GUIFORM_SetChildHeight(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, MMIBROWSER_START_PAGE_BUTTON5_CTRL_ID, &label_height);

    GUIFORM_SetSpace(MMIBROWSER_START_PAGE_FORM1_CTRL_ID, &hor_space, PNULL);

    GUILABEL_SetIcon(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, IMAGE_BROWSER_ADDRESS_ICON);
    GUILABEL_SetTextById(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, TXT_WWW_INPUT_URL, FALSE);
    GUILABEL_SetFont(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID,MMI_DEFAULT_SMALL_FONT, MMI_BLACK_COLOR);

    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
    GUILABEL_SetBg(MMIBROWSER_START_PAGE_LABEL11_CTRL_ID, &bg);

#ifdef BROWSER_SEARCH_SUPPORT
    GUILABEL_SetIcon(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, IMAGE_BROWSER_SEARCH_ICON);
    GUILABEL_SetTextById(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, STXT_SEARCH, FALSE);
    GUILABEL_SetFont(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID,MMI_DEFAULT_SMALL_FONT, MMI_BLACK_COLOR);
    bg.bg_type = GUI_BG_IMG;
    bg.img_id = IMAGE_BROWSER_STARTPAGE_LABEL_BG;
    GUILABEL_SetBg(MMIBROWSER_START_PAGE_LABEL12_CTRL_ID, &bg);
#endif
    GUIBUTTON_SetCallBackFunc(MMIBROWSER_START_PAGE_BUTTON5_CTRL_ID, BookmarkButtonCallBack);

#ifdef MMI_PDA_SUPPORT
    if (MMITHEME_IsIstyle())
    {
        MMI_TAB_STYLE_T style_t = {0};
        MMI_CTRL_ID_T   button_ctrl_id  = 0;
        MMI_CTRL_ID_T   label_ctrl_id  = 0;
        MMI_CTRL_ID_T   parent_form_ctrl_id  = 0;
        uint16  img_height = 0;
        uint16  ver_space = 0;

        MMITHEME_GetTabStyle(&style_t);

        GUIRES_GetImgWidthHeight(PNULL, &img_height, style_t.bg_image_id, win_id);
        form2_rect = client_rect;
        form2_rect.h_rect.top = form2_rect.h_rect.bottom - img_height + 1;
        form2_rect.v_rect.top = form2_rect.v_rect.bottom - img_height + 1;
        GUIAPICTRL_SetBothRect(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, &form2_rect);
        
        GUIFORM_SetSpace(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, &hor_space, &ver_space);
        GUIFORM_SetMargin(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, 0);
        GUIFORM_PermitChildBg(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, FALSE);
        GUIFORM_PermitChildFont(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, FALSE);
        GUIFORM_IsSlide(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, FALSE);
        bg.bg_type = GUI_BG_IMG;
        bg.img_id = style_t.bg_image_id;
        GUIFORM_SetBg( MMIBROWSER_START_PAGE_FORM2_CTRL_ID, &bg);

        bg.bg_type = GUI_BG_IMG;
        for (i = 0; i < sizeof(s_start_page_button_info)/sizeof(s_start_page_button_info[0]); i++)
        {
            button_ctrl_id = s_start_page_button_info[i].button_ctrl_id;
            label_ctrl_id  = s_start_page_button_info[i].label_ctrl_id;
            parent_form_ctrl_id  = s_start_page_button_info[i].parent_form_ctrl_id;
       
            GUIBUTTON_SetCallBackFunc(button_ctrl_id, s_start_page_button_info[i].callback_func);
            
            bg.img_id = s_start_page_button_info[i].release_img_id;
            GUIBUTTON_SetBg(button_ctrl_id, &bg);
            
            bg.img_id = s_start_page_button_info[i].pressed_img_id;
            GUIBUTTON_SetPressedBg(button_ctrl_id, &bg);
            GUIBUTTON_SetRunSheen(button_ctrl_id, FALSE);
            
            GUILABEL_SetTextById(label_ctrl_id, s_start_page_button_info[i].text_id,  FALSE);       
            GUILABEL_SetFont(label_ctrl_id, style_t.font, style_t.font_color);

            GUIFORM_SetStyle(parent_form_ctrl_id,GUIFORM_STYLE_UNIT);
            GUIFORM_SetChildSpace(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, parent_form_ctrl_id, &hor_space, &hor_space);
            GUIFORM_SetMargin(parent_form_ctrl_id, 0);
        }
    }
    else
    {
        GUIFORM_SetVisible(MMIBROWSER_START_PAGE_FORM2_CTRL_ID, FALSE, FALSE);
    }
#endif

#if defined(MMIDORADO_MY_NAV_SUPPORT) && defined(MMIDORADO_FIX_NAV_SUPPORT)
    {
        GUIICONLIST_MARGINSPACE_INFO_T    margin_space = {6,6,6,6};
        GUIICONLIST_DELIMITER_INFO_T group_info = {0};
        MMI_STRING_T        str_info = {0};
        uint16  img_height = 0;
        uint16  img_width = 0;

#ifdef MMIDORADO_MY_NAV_SUPPORT
        GUIRES_GetImgWidthHeight(&img_width, &img_height, IMAGE_BROWSER_FAVORITE_ADD_ICON, win_id);
#else
        GUIRES_GetImgWidthHeight(&img_width, &img_height, g_fix_nav_default_info[0].img_id, win_id);
#endif
        both_rect.h_rect.top = both_rect.h_rect.bottom;
        both_rect.v_rect.top = both_rect.v_rect.bottom;
#ifdef MMI_PDA_SUPPORT
        if (MMITHEME_IsIstyle())
        {
            both_rect.h_rect.bottom = form2_rect.h_rect.top - 1;
            both_rect.v_rect.bottom = form2_rect.v_rect.top - 1;
        }
        else
#endif        
        {
            both_rect.h_rect.bottom = client_rect.h_rect.bottom;
            both_rect.v_rect.bottom = client_rect.v_rect.bottom;
        }

        GUIAPICTRL_SetBothRect(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &both_rect);

        //set iconlist
        GUIICONLIST_SetTotalIcon(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM));
        GUIICONLIST_SetCurIconIndex(0,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
        GUIICONLIST_SetStyle(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, GUIICONLIST_STYLE_ICON_UIDT);
        GUIICONLIST_SetIconWidthHeight(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, img_width, img_height);
        font_all.color = MMI_BLACK_COLOR;
        font_all.font = MMI_DEFAULT_SMALL_FONT;
        GUIICONLIST_SetIconListTextInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, font_all);   
        bg.bg_type = GUI_BG_COLOR;
        bg.color = MMI_WHITE_COLOR;
        GUIICONLIST_SetBg(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &bg); 
        GUIICONLIST_SetIconItemSpace(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, margin_space);
        {
            GUI_BORDER_T   select_border = {0};
            GUIICONLIST_SetItemBorderStyle(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, FALSE, &select_border);
        }
        
#ifdef MMIDORADO_FIX_NAV_SUPPORT
        for (i = 0; i< MMIBRW_FIX_NAV_MAX_ITEM; i++)
        {
            uint16                      icon_index = 0;
            GUIICONLIST_DISP_T          icon_list = {0};
            GUIANIM_DATA_INFO_T         data_info = {0};
            
#ifndef MMI_PDA_SUPPORT
            icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
            icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
            icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
            //append icon text item
            icon_index = i;
            MMI_GetLabelTextByLang(g_fix_nav_default_info[icon_index].text_id,&icon_list.name_str);//title
            data_info.img_id = g_fix_nav_default_info[icon_index].img_id;

            GUIICONLIST_AppendText(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&icon_list);
            GUIICONLIST_AppendIcon(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&data_info,PNULL);
        }    
#endif    
        
#ifdef MMIDORADO_MY_NAV_SUPPORT
        AppendMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
#endif
            
        group_info.start_item_index = 0;
        MMI_GetLabelTextByLang(TXT_BROWSER_RMJP, &str_info);
        group_info.delimiter_str_info = str_info;
        GUIICONLIST_CreateGroup(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &group_info);

        group_info.start_item_index = MMIBRW_FIX_NAV_MAX_ITEM;
        MMI_GetLabelTextByLang(TXT_BROWSER_WDDH, &str_info);
        group_info.delimiter_str_info = str_info;
        GUIICONLIST_CreateGroup(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &group_info);

        bg.bg_type = GUI_BG_COLOR;
        bg.color = MMI_CYAN_COLOR;
        GUIICONLIST_SetDelimiterBg(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &bg);
        font_all.color = MMI_WHITE_COLOR;
        font_all.font = MMI_DEFAULT_SMALL_FONT;
        GUIICONLIST_SetDelimiterStrInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &font_all, 0);
        GUIICONLIST_SetDelimiterHeight(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, (MMI_DEFAULT_SMALL_FONT_SIZE + 2));
    }
#endif
}

/*****************************************************************************/
//  Description : handle Start page of the browser
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleStartPageTheme1WinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    ret=MMI_RESULT_TRUE;    

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_PDA_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        InitBrwNewStartPageWin(win_id);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_START_PAGE_LABEL11_CTRL_ID);
#ifdef MMIDORADO_MY_NAV_SUPPORT
        MMIBROWSER_SetIsIcoUpdate(FALSE);
#endif
        break;
        
#ifdef WIN32
    case MSG_APP_1:
        MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, "http://172.16.14.173:8082/", PNULL);
        break;
#endif

    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    {
        MMI_CTRL_ID_T       active_ctrl_id = 0;

#ifdef MMI_PDA_SUPPORT
        if (PNULL != param)
        {
	        active_ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;
        }
#else
        active_ctrl_id = MMK_GetActiveCtrlId(win_id);
#endif
        switch (active_ctrl_id)
        {
        case MMIBROWSER_START_PAGE_LABEL11_CTRL_ID:
            MMIBROWSER_InputUrl(PNULL);
            break;
            
        case MMIBROWSER_START_PAGE_LABEL12_CTRL_ID:
#ifdef BROWSER_SEARCH_SUPPORT
            MMIBROWSER_OpenSearchWin();
#endif
            break;

#if defined(MMIDORADO_MY_NAV_SUPPORT) && defined(MMIDORADO_FIX_NAV_SUPPORT)
        case MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID:
        {
            uint16  cur_index = 0;
            cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            
#if defined(MMIDORADO_FIX_NAV_SUPPORT)
            if (cur_index < MMIBRW_FIX_NAV_MAX_ITEM)
            {
                MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, (uint8*)g_fix_nav_default_info[cur_index].url_ptr, PNULL);
            }
            else
#endif            
#if defined(MMIDORADO_MY_NAV_SUPPORT)
            if (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM))
            {
                OpenOneMyNavUrl(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            }
            else
#endif                
            {
                //SCI_TRACE_LOW:"[BRW]BrwHandleStartPageTheme1WinMsg:err cur_index = %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_11942_112_2_18_2_6_20_230,(uint8*)"d", cur_index);
            }
        }            
            break;
#endif            

        default:
            break;
        }
    }
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
#ifdef MMI_PDA_SUPPORT
        if (MMITHEME_IsIstyle())
        {
            break;//istyle without pop menu        
        }
#endif
        MMK_CreateWin((uint32*)MMIBROWSER_STARTPAGE_POPMENU_WIN_TAB,PNULL);
        GUIMENU_CreatDynamic(PNULL,MMIBROWSER_STARTPAGE_POPMENU_WIN_ID,MMIBROWSER_MENU_CTRL_ID,GUIMENU_STYLE_POPUP);
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMIBROWSER_QueryExit();
        break;

#ifndef MMI_PDA_SUPPORT
    case MSG_NOTIFY_FORM_SWITCH_ACTIVE:
    {
        uint32  i= 0;
        MMI_CTRL_ID_T   active_ctrl_id = MMK_GetActiveCtrlId(win_id);
        for (i = MMIBROWSER_START_PAGE_LABEL11_CTRL_ID; i <= MMIBROWSER_START_PAGE_LABEL12_CTRL_ID; i++)
        {
            if (i != active_ctrl_id)
            {
                GUILABEL_SetBackgroundColor(i, MMI_WHITE_COLOR);
            }
            else
            {
                GUILABEL_SetBackgroundColor(active_ctrl_id, MMITHEME_GetCurThemeFontColor(MMI_THEME_FORM_BG));
            }

        }
        
        if (MMK_IsFocusWin(win_id))//update
        {
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
    }
        break;
#endif

#ifdef MMIDORADO_MY_NAV_SUPPORT
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_LIST_LONGOK:
    case MSG_NOTIFY_ICONLIST_TPLONG:
        //长按List弹出context menu菜单
        if(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID == MMK_GetActiveCtrlId(win_id))
        {
            uint16  cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            
            if ((cur_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
            {
                MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
                
                cur_index -= MMIBRW_FIX_NAV_MAX_ITEM;
                MMIBROWSER_GetMyNavInfo(&nv_info);
                
                if (MMIBRW_MY_NAV_ITEM_TYPE_NONE != nv_info.item_info[cur_index].item_type)
                {
                    MMK_CreateWin((uint32*)MMIBROWSER_MY_NAV_POPUP_WIN_TAB,PNULL);
                }
                else
                {
                    //do nothing
                }
            }
        }
        break;
#endif
#endif

#ifdef MMI_PDA_SUPPORT
    case MSG_TP_PRESS_DOWN:
        if (MMITHEME_IsIstyle())
        {
            GUI_POINT_T point = {0};
            uint32  i = 0;
            
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);

            for (i = 0; i < sizeof(s_start_page_button_info)/sizeof(s_start_page_button_info[0]); i++)
            {
                if (MMK_PosIsCtrl(s_start_page_button_info[i].parent_form_ctrl_id, point))
                {
                    GUI_BG_T    bg = {0};
                    
                    bg.bg_type = GUI_BG_IMG;
                    bg.img_id = s_start_page_button_info[i].pressed_img_id;
                    GUIBUTTON_SetBg(s_start_page_button_info[i].button_ctrl_id, &bg);
                    GUIBUTTON_Update(s_start_page_button_info[i].button_ctrl_id);
                    //reset
                    bg.img_id = s_start_page_button_info[i].release_img_id;
                    GUIBUTTON_SetBg(s_start_page_button_info[i].button_ctrl_id, &bg);
                    break;
                }
            }  
        }
        break;

    case MSG_TP_PRESS_UP:
        if (MMITHEME_IsIstyle())
        {
            uint32  i = 0;
            GUI_POINT_T point = {0};
            GUI_BG_T    bg = {0};

            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);

            for (i = 0; i < ARRAY_SIZE(s_start_page_button_info); i++)
            {
                if (MMK_PosIsCtrl(s_start_page_button_info[i].button_ctrl_id, point))
                {
                    if (PNULL != s_start_page_button_info[i].callback_func)
                    {
                        s_start_page_button_info[i].callback_func();
                    }
                    break;
                }
            }

            bg.bg_type = GUI_BG_IMG;
            for (i = 0; i < ARRAY_SIZE(s_start_page_button_info); i++)
            {
                bg.img_id = s_start_page_button_info[i].release_img_id;
                GUIBUTTON_SetBg(s_start_page_button_info[i].button_ctrl_id, &bg);
                GUIBUTTON_Update(s_start_page_button_info[i].button_ctrl_id);
            }
        }
        break;
#endif

    case MSG_CLOSE_WINDOW:
#ifdef BROWSER_START_PAGE_THEME1
        MMIBROWSER_SetSlide(FALSE);
#endif
        MMIBROWSER_Exit();
        break;

#ifdef MMIDORADO_MY_NAV_SUPPORT
    case MSG_GET_FOCUS:
        if (MMIBROWSER_GetIsIcoUpdate())
        {
            AppendMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            MMIBROWSER_SetIsIcoUpdate(FALSE);
        }
        break;
#endif

    case MSG_CTL_ICONLIST_APPEND_TEXT://useful when slide or switch lcd
    {
        uint16  icon_index = *((uint16 *)param);

#ifdef MMIDORADO_FIX_NAV_SUPPORT
        if (icon_index < MMIBRW_FIX_NAV_MAX_ITEM)
        {
            GUIICONLIST_DISP_T          icon_list = {0};
            
#ifndef MMI_PDA_SUPPORT
            icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
            icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
            icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
            //append icon text item
            MMI_GetLabelTextByLang(g_fix_nav_default_info[icon_index].text_id,&icon_list.name_str);//title
            GUIICONLIST_AppendText(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&icon_list);
        }    
#endif    
#ifdef MMIDORADO_MY_NAV_SUPPORT
        if ((icon_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (icon_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
        {
            AppendOneMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,
                (icon_index - MMIBRW_FIX_NAV_MAX_ITEM), icon_index, TRUE, FALSE);
        }
#endif
    }
        break;
        
    case MSG_CTL_ICONLIST_APPEND_ICON://useful when slide or switch lcd
    {
        uint16  icon_index = *((uint16 *)param);

#ifdef MMIDORADO_FIX_NAV_SUPPORT
        if (icon_index < MMIBRW_FIX_NAV_MAX_ITEM)
        {
            GUIANIM_DATA_INFO_T         data_info = {0};
            data_info.img_id = g_fix_nav_default_info[icon_index].img_id;
            GUIICONLIST_AppendIcon(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&data_info,PNULL);
        }    
#endif    
#ifdef MMIDORADO_MY_NAV_SUPPORT
        if ((icon_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (icon_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
        {
            AppendOneMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,
                (icon_index - MMIBRW_FIX_NAV_MAX_ITEM), icon_index, FALSE, TRUE);
        }
#endif
    }
        break;
                        
#ifdef MMIDORADO_FIX_HMTL_SUPPORT
    case MSG_BRW_MSG_INIT_CNF:
    case MSG_BRW_MSG_EXIT_CNF:
    case MSG_BRW_MSG_TYPE_ENTER_FILENM_REQ:
    case MSG_BRW_MSG_TYPE_DL_CONFIRM_REQ:     
    case MSG_BRW_MSG_TYPE_AUTH_REQ:           
    case MSG_BRW_MSG_TYPE_CHANGE_URL_IND:     
    case MSG_BRW_MSG_TYPE_NEW_PAGE_REQ_IND:   
    case MSG_BRW_MSG_TYPE_UPDATE_CONTENT_IND: 
    case MSG_BRW_MSG_TYPE_PROGRESS_IND:       
    case MSG_BRW_MSG_TYPE_ERROR_IND:            
    case MSG_BRW_MSG_TYPE_DOWNLOAD_OK_IND:     
    case MSG_BRW_MSG_TYPE_DOWNLOAD_ERR_IND:   
    case MSG_BRW_MSG_TYPE_PAGE_GET_IND:
    case MSG_BRW_MSG_TYPE_MEDIA_GET_ONE_IND:
    case MSG_BRW_MSG_TYPE_DISPLAY_READY_IND:  
    case MSG_BRW_MSG_TYPE_SSL_CERT_IND:       
    case MSG_BRW_MSG_TYPE_REPAINT_READY_IND:
    case MSG_BRW_MSG_TYPE_PAGE_STOP_IND:
    case MSG_BRW_MSG_TYPE_OMA_DL_CONFIRM_REQ:
    case MSG_BRW_MSG_TYPE_LAYOUTCTRL_DESTROY_REQ:
    case MSG_BRW_MSG_TYPE_PDP_ACTIVE_CNF:        
        HandleFixedInstanceControlMsg(msg_id,param);
        break;
#endif

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}

#endif

#ifdef MMIDORADO_MY_NAV_SUPPORT
//my navigation:我的导航begin
/*****************************************************************************/
//  Description : remove one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN RemoveOneMyNav(MMI_CTRL_ID_T ctrl_id)
{                
    MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
    uint16  cur_index = GUIICONLIST_GetCurIconIndex(ctrl_id);
    GUIICONLIST_DISP_T  icon_list = {0};
    GUIANIM_DATA_INFO_T data_info = {0};
    uint16  my_nav_index = 0;

    MMIBROWSER_GetMyNavInfo(&nv_info);
    
    if ((cur_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
    {
        my_nav_index = cur_index - MMIBRW_FIX_NAV_MAX_ITEM;
    }
    else
    {
        return FALSE;//error
    }
       
    if (MMIBRW_MY_NAV_ITEM_TYPE_NONE != nv_info.item_info[my_nav_index].item_type)
    {
#ifdef BROWSER_OPERA_WDDHE_WITH_DEFAULT              
        if (MMIBRW_MY_NAV_ITEM_TYPE_FIXED != nv_info.item_info[my_nav_index].item_type)//fixed can't be deleted
#endif            
        {                
            //delete(save to NV)
            //nv_info.item_count--;
            SCI_MEMSET(&(nv_info.item_info[my_nav_index]), 0x00, sizeof(MMIBRW_MY_NAV_ITEM_T));
            MMIBROWSER_SetMyNavInfo(&nv_info);

            MMIBROWSER_DeleteIco(my_nav_index);
            
            //update the control info
#ifndef MMI_PDA_SUPPORT
            icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
            icon_list.softkey_id[MIDDLE_BUTTON] = STXT_ADD;
            icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
            icon_list.name_str.wstr_len = 1;
            icon_list.name_str.wstr_ptr = L" ";
            GUIICONLIST_AppendText(cur_index, ctrl_id, &icon_list);
            data_info.img_id = IMAGE_BROWSER_FAVORITE_ADD_ICON;//default
            GUIICONLIST_AppendIcon(cur_index,ctrl_id,&data_info,PNULL);//icon
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************/
//  Description : OPEN one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void OpenOneMyNavUrl(MMI_CTRL_ID_T ctrl_id)
{
    uint16  cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
    uint16  my_nav_index = 0;

    if ((cur_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
    {
        MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
        
        my_nav_index = cur_index - MMIBRW_FIX_NAV_MAX_ITEM;

        MMIBROWSER_GetMyNavInfo(&nv_info);
        
        if (MMIBRW_MY_NAV_ITEM_TYPE_NONE != nv_info.item_info[my_nav_index].item_type)
        {
            wchar   file_name[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
            uint16  file_name_len = MMIFILE_FULL_PATH_MAX_LEN;
            
            MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, (uint8*)nv_info.item_info[my_nav_index].url_arr, PNULL);
            if (MMIBROWSER_GetIcoFullFileNameByIdex(my_nav_index, file_name, &file_name_len))
            {
                MMIBROWSER_UpdateIcoFilename(file_name);
            }
        }
        else
        {
            MMIBROWSER_MY_NAV_EDIT_INFO_T *nav_edit_info_ptr = PNULL;

            nav_edit_info_ptr = SCI_ALLOCA(sizeof(MMIBROWSER_MY_NAV_EDIT_INFO_T));
            
            if (nav_edit_info_ptr != PNULL)
            {
                nav_edit_info_ptr->is_add_new = TRUE;
                nav_edit_info_ptr->index = my_nav_index;
                MMK_CreateWin((uint32 *)MMIBROWSER_MY_NAV_EDIT_WIN_TAB, (ADD_DATA)nav_edit_info_ptr);
            }
            else
            {
                MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
            }
        }
    }
    else
    {
        //SCI_TRACE_LOW:"[BRW]OpenOneMyNavUrl:err cur_index = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_12196_112_2_18_2_6_21_231,(uint8*)"d", cur_index);
    }
}

/*****************************************************************************/
//  Description : append the my navigation info to the window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void AppendMyNavInfo(MMI_CTRL_ID_T ctrl_id)
{
    MMIBRW_MY_NAV_CONTEXT_T *nv_info_ptr = PNULL;
    uint16  icon_index = 0;
    GUIICONLIST_DISP_T  icon_list = {0};
    GUIANIM_DATA_INFO_T data_info = {0};
    MMIBRW_MY_NAV_ITEM_T    *my_nav_item_ptr = PNULL;
    uint16  my_nav_index = 0;

    nv_info_ptr = SCI_ALLOC_APPZ(sizeof(MMIBRW_MY_NAV_CONTEXT_T));
    if (nv_info_ptr != PNULL)
    {
        MMIBROWSER_GetMyNavInfo(nv_info_ptr);
        
        //append icon text item
        for (my_nav_index = 0; my_nav_index < MMIBRW_MY_NAV_MAX_ITEM; my_nav_index++)
        {
#ifndef MMI_PDA_SUPPORT
            icon_list.softkey_id[LEFT_BUTTON] = STXT_OPTION;
            icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
            icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
            icon_index = my_nav_index + MMIBRW_FIX_NAV_MAX_ITEM;
            my_nav_item_ptr = &(nv_info_ptr->item_info[my_nav_index]);
            if (MMIBRW_MY_NAV_ITEM_TYPE_FIXED == my_nav_item_ptr->item_type)
            {
                if (my_nav_item_ptr->img_id != 0)
                {
                    data_info.img_id = my_nav_item_ptr->img_id;
                }
                else
                {
                    data_info.img_id = MMIBRW_MY_NAV_DEFAULT_IMAGE_ID;//default
                }
                GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);//icon

                MMI_GetLabelTextByLang(my_nav_item_ptr->title_info_u.title_text_id,&icon_list.name_str);//title
                GUIICONLIST_AppendText(icon_index,ctrl_id,&icon_list);//text
            }
            else if (MMIBRW_MY_NAV_ITEM_TYPE_CUSTOMIZED == my_nav_item_ptr->item_type)
            {
                BOOLEAN is_use_default_img = TRUE;
                wchar   file_name[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
                uint16  file_name_len = MMIFILE_FULL_PATH_MAX_LEN;
                
                if (MMIBROWSER_GetIcoFullFileNameByIdex(my_nav_index, file_name, &file_name_len))
                {
                    if (0 != file_name_len)//valid file name
                    {
                        GUIANIM_FILE_INFO_T   file_info = {0};

                        if (MMIFILE_IsFileExist(file_name, file_name_len))//valid file
                        {
                            file_info.full_path_wstr_len = file_name_len;
                            file_info.full_path_wstr_ptr = file_name;
                            GUIICONLIST_AppendIcon(icon_index,ctrl_id,PNULL,&file_info);//icon
                            is_use_default_img = FALSE;
                        }
                    }
                }
 
                if (is_use_default_img)
                {
                    data_info.img_id = MMIBRW_MY_NAV_DEFAULT_IMAGE_ID;//default
                    GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);//icon
                }
                                    
                icon_list.name_str.wstr_len = MMIAPICOM_Wstrlen(my_nav_item_ptr->title_info_u.title_warr);
                icon_list.name_str.wstr_ptr = my_nav_item_ptr->title_info_u.title_warr;
                GUIICONLIST_AppendText(icon_index,ctrl_id,&icon_list);//text
            }
            else
            {
#ifndef MMI_PDA_SUPPORT
                icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
                icon_list.softkey_id[MIDDLE_BUTTON] = STXT_ADD;
#endif
                icon_list.name_str.wstr_len = 0;
                icon_list.name_str.wstr_ptr = PNULL;
             
                GUIICONLIST_AppendText(icon_index,ctrl_id,&icon_list);//text

                data_info.img_id = IMAGE_BROWSER_FAVORITE_ADD_ICON;
                GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);
            }
        }
        
        SCI_FREE(nv_info_ptr);
    }
}

/*****************************************************************************/
//  Description : append the my navigation info to the window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void AppendOneMyNavInfo(MMI_CTRL_ID_T ctrl_id, 
            uint16 my_nav_index, 
            uint16 icon_index,
            BOOLEAN is_append_text,
            BOOLEAN is_append_ico
            )
{
    MMIBRW_MY_NAV_CONTEXT_T *nv_info_ptr = PNULL;
    GUIICONLIST_DISP_T  icon_list = {0};
    GUIANIM_DATA_INFO_T data_info = {0};
    MMIBRW_MY_NAV_ITEM_T    *my_nav_item_ptr = PNULL;

    nv_info_ptr = SCI_ALLOC_APPZ(sizeof(MMIBRW_MY_NAV_CONTEXT_T));
    if (nv_info_ptr != PNULL)
    {
        MMIBROWSER_GetMyNavInfo(nv_info_ptr);//notice:read NV!
        my_nav_item_ptr = &(nv_info_ptr->item_info[my_nav_index]);

        //append icon text item
        if (is_append_text)
        {
#ifndef MMI_PDA_SUPPORT
            icon_list.softkey_id[LEFT_BUTTON] = STXT_OPTION;
            icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
            icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
        }

        if (MMIBRW_MY_NAV_ITEM_TYPE_FIXED == my_nav_item_ptr->item_type)
        {
            if (is_append_text)
            {
                MMI_GetLabelTextByLang(my_nav_item_ptr->title_info_u.title_text_id,&icon_list.name_str);//title
                GUIICONLIST_AppendText(icon_index,ctrl_id,&icon_list);//text
            }

            if (is_append_ico)
            {
                if (my_nav_item_ptr->img_id != 0)
                {
                    data_info.img_id = my_nav_item_ptr->img_id;
                }
                else
                {
                    data_info.img_id = MMIBRW_MY_NAV_DEFAULT_IMAGE_ID;//default
                }
                GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);//icon
            }
        }
        else if (MMIBRW_MY_NAV_ITEM_TYPE_CUSTOMIZED == my_nav_item_ptr->item_type)
        {
            if (is_append_text)
            {
                icon_list.name_str.wstr_len = MMIAPICOM_Wstrlen(my_nav_item_ptr->title_info_u.title_warr);
                icon_list.name_str.wstr_ptr = my_nav_item_ptr->title_info_u.title_warr;
                GUIICONLIST_AppendText(icon_index,ctrl_id,&icon_list);//text
            }

            if (is_append_ico)
            {
                BOOLEAN is_use_default_img = TRUE;
                wchar   file_name[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
                uint16  file_name_len = MMIFILE_FULL_PATH_MAX_LEN;
                
                if (MMIBROWSER_GetIcoFullFileNameByIdex(my_nav_index, file_name, &file_name_len))
                {
                    if (0 != file_name_len)//valid file name
                    {
                        GUIANIM_FILE_INFO_T   file_info = {0};

                        if (MMIFILE_IsFileExist(file_name, file_name_len))//valid file
                        {
                            file_info.full_path_wstr_len = file_name_len;
                            file_info.full_path_wstr_ptr = file_name;
                            GUIICONLIST_AppendIcon(icon_index,ctrl_id,PNULL,&file_info);//icon
                            is_use_default_img = FALSE;
                        }
                    }
                }

                if (is_use_default_img)
                {
                    data_info.img_id = MMIBRW_MY_NAV_DEFAULT_IMAGE_ID;//default
                    GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);//icon
                }
            }            
        }
        else
        {
            if (is_append_text)
            {
#ifndef MMI_PDA_SUPPORT
                icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
                icon_list.softkey_id[MIDDLE_BUTTON] = STXT_ADD;
#endif
                icon_list.name_str.wstr_len = 0;
                icon_list.name_str.wstr_ptr = PNULL;
             
                GUIICONLIST_AppendText(icon_index,ctrl_id,&icon_list);//text
            }
            
            if (is_append_ico)
            {
                data_info.img_id = IMAGE_BROWSER_FAVORITE_ADD_ICON;
                GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);
            }
        }
        
        SCI_FREE(nv_info_ptr);
    }
}

/*****************************************************************************/
//  Description : add/edit one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN AddOneMyNav(
                //MMI_CTRL_ID_T     ctrl_id, //in
                uint16  my_nav_index,
                MMI_STRING_T    title_text_str,
                MMI_STRING_T    url_text_str
                )
{
    uint8    url_arr[MMIBRW_MY_NAV_MAX_URL_LENGTH+1] ={0};   
    MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
        
    MMI_WSTRNTOSTR(url_arr, MMIBRW_MY_NAV_MAX_URL_LENGTH,
                url_text_str.wstr_ptr, url_text_str.wstr_len, 
                MIN(MMIBRW_MY_NAV_MAX_URL_LENGTH, url_text_str.wstr_len));
                
    if (Brw_IsIncludeInvalidUrl(url_arr))
    {
        MMIBROWSER_AlertUrlInvalid();
        return FALSE;
    }
        
    if (my_nav_index >= MMIBRW_MY_NAV_MAX_ITEM )
    {
        return FALSE;
    }
    
    MMIBROWSER_GetMyNavInfo(&nv_info);
    
    SCI_MEMSET(&(nv_info.item_info[my_nav_index]), 0x00, sizeof(MMIBRW_MY_NAV_ITEM_T));
    nv_info.item_info[my_nav_index].item_type = MMIBRW_MY_NAV_ITEM_TYPE_CUSTOMIZED;
    MMI_WSTRNCPY(nv_info.item_info[my_nav_index].title_info_u.title_warr, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH, 
        title_text_str.wstr_ptr, title_text_str.wstr_len,
        MIN(title_text_str.wstr_len, MMIBRW_BOOKMARK_MAX_TITLE_LENGTH));
    SCI_MEMCPY(nv_info.item_info[my_nav_index].url_arr, url_arr, MIN(url_text_str.wstr_len, MMIBRW_MY_NAV_MAX_URL_LENGTH));

    MMIBROWSER_SetMyNavInfo(&nv_info);

    MMIBROWSER_DeleteIco(my_nav_index);

    return TRUE;
}
/*****************************************************************************/
//  Description : add one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN GetFirstInvalidMyNav(
                MMIBRW_MY_NAV_CONTEXT_T   my_nav_info,
                uint16  *first_invalid_nav_index_ptr
                )
{
    uint32  i = 0;
    
    if (PNULL == first_invalid_nav_index_ptr)
    {
        return FALSE;
    }
    *first_invalid_nav_index_ptr = MMIBRW_MY_NAV_MAX_ITEM;

    for (i = 0; i < MMIBRW_MY_NAV_MAX_ITEM; i++)
    {
        if (MMIBRW_MY_NAV_ITEM_TYPE_NONE == my_nav_info.item_info[i].item_type)
        {
            *first_invalid_nav_index_ptr = i;
            break;
        }
    }

    return ((i < MMIBRW_MY_NAV_MAX_ITEM)? TRUE: FALSE);
}


/*****************************************************************************/
//  Description : open the edit window to add one my navigation item
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIBROWSER_Add2MyNav(
                MMI_STRING_T    title_text_str,
                MMI_STRING_T    url_text_str
                )
{
    MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
    uint16  my_nav_index = 0;
    BOOLEAN result = FALSE;
    MMIBROWSER_MY_NAV_EDIT_INFO_T *nav_edit_info_ptr = PNULL;

    MMIBROWSER_GetMyNavInfo(&nv_info);

    if (!GetFirstInvalidMyNav(nv_info, &my_nav_index))
    {
        if (my_nav_index >= MMIBRW_MY_NAV_MAX_ITEM)
        {
            MMIPUB_OpenAlertWarningWin(TXT_BROWSER_NAV_FULL);
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
        }
        return FALSE;
    }
       
    nav_edit_info_ptr = SCI_ALLOC_APPZ(sizeof(MMIBROWSER_MY_NAV_EDIT_INFO_T));
    if (nav_edit_info_ptr != PNULL)
    {
        nav_edit_info_ptr->is_add_new = TRUE;
        nav_edit_info_ptr->index = my_nav_index;
        MMK_CreateWin((uint32 *)MMIBROWSER_MY_NAV_EDIT_WIN_TAB, (ADD_DATA)nav_edit_info_ptr);
        GUIEDIT_SetString(MMIBROWSER_MY_NAV_EDIT_TITLE_CTRL_ID, title_text_str.wstr_ptr, title_text_str.wstr_len);
        GUIEDIT_SetString(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID, url_text_str.wstr_ptr, url_text_str.wstr_len);      
        result = TRUE;
    }
    else
    {
        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
    }

    return result;
}

/*****************************************************************************/
//  Description : handle my navigation edit window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMyNavEditWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    MMIBROWSER_MY_NAV_EDIT_INFO_T *edit_info_ptr= PNULL;

    switch(msg_id) 
    {
    case MSG_OPEN_WINDOW:
    {
        MMI_TEXT_ID_T   title_text_id = TXT_NULL;
                
        //GUIEDIT_SetTextMaxLen(MMIBROWSER_MY_NAV_EDIT_TITLE_CTRL_ID,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH,MMIBRW_BOOKMARK_MAX_TITLE_LENGTH);
        //GUIEDIT_SetTextMaxLen(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID,MMIBRW_MY_NAV_MAX_URL_LENGTH,MMIBRW_MY_NAV_MAX_URL_LENGTH);

        edit_info_ptr = (MMIBROWSER_MY_NAV_EDIT_INFO_T*)MMK_GetWinAddDataPtr(win_id);
        if ((PNULL == edit_info_ptr) || edit_info_ptr->is_add_new)
        {
            title_text_id = STXT_ADD;
        }
        else
        {
            title_text_id = TXT_EDIT;
        }
        GUIWIN_SetTitleTextId(win_id, title_text_id, FALSE);

#ifdef MMI_PDA_SUPPORT
        GUIFORM_SetType(MMIBROWSER_MY_NAV_EDIT_CTRL_ID, GUIFORM_TYPE_TP);
        GUIEDIT_SetAlign(MMIBROWSER_MY_NAV_EDIT_TITLE_CTRL_ID, ALIGN_LVMIDDLE);
        GUIEDIT_SetAlign(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID, ALIGN_LVMIDDLE);
#endif /* MMI_PDA_SUPPORT */
        GUIFORM_SetStyle(MMIBROWSER_MY_NAV_EDIT_FORM1_CTRL_ID,GUIFORM_STYLE_UNIT);
        GUIFORM_SetStyle(MMIBROWSER_MY_NAV_EDIT_FORM2_CTRL_ID,GUIFORM_STYLE_UNIT);
            
        GUILABEL_SetTextById(MMIBROWSER_MY_NAV_EDIT_LABEL1_CTRL_ID, TXT_BROWSER_FILE_TITLE, FALSE);
        GUILABEL_SetTextById(MMIBROWSER_MY_NAV_EDIT_LABEL2_CTRL_ID, TXT_URL_ADRESS, FALSE);
        GUIEDIT_SetIm(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID,(uint32)GUIIM_TYPE_ENGLISH | (uint32)GUIIM_TYPE_DIGITAL | (uint32)GUIIM_TYPE_ABC,GUIIM_TYPE_ABC);//MS00220112
        GUIEDIT_SetImTag(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID, GUIIM_TAG_URL);        

        MMK_SetAtvCtrl(win_id,MMIBROWSER_MY_NAV_EDIT_CTRL_ID);
    }
        break;
        
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        {
            MMI_CTRL_ID_T src_ctrl_id = 0;

            if (PNULL != param)
            {
                src_ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

                if (src_ctrl_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                }
                else if (src_ctrl_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_MIDSK, PNULL, 0);
                }
                else if (src_ctrl_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                }
            }
        }
        break;
#endif /* MMI_PDA_SUPPORT */

    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
#ifndef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK: 
#endif /* MMI_PDA_SUPPORT */
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
        MMI_STRING_T url_text_str = {0};
        MMI_STRING_T title_text_str = {0};

        edit_info_ptr = (MMIBROWSER_MY_NAV_EDIT_INFO_T*)MMK_GetWinAddDataPtr(win_id);
        if ((PNULL == edit_info_ptr) || (edit_info_ptr->index >= MMIBRW_MY_NAV_MAX_ITEM))
        {
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
            MMK_CloseWin(win_id);
            break;
        }        
        
        GUIEDIT_GetString(MMIBROWSER_MY_NAV_EDIT_TITLE_CTRL_ID, &title_text_str);
        GUIEDIT_GetString(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID, &url_text_str);

        if ((PNULL == title_text_str.wstr_ptr) || (0 == title_text_str.wstr_len))
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMM_NO_TITLE);
        }
        else if ((PNULL == url_text_str.wstr_ptr) || (0 == url_text_str.wstr_len))
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMM_URL_NULL);
        }
        else
        {
            uint16  my_nav_index = edit_info_ptr->index;

            if (AddOneMyNav(my_nav_index, title_text_str, url_text_str))
            {
                if (MMK_IsOpenWin(MMIBROWSER_START_PAGE_WIN_ID))
                {
                    uint16  edit_index = 0;
                    GUIICONLIST_DISP_T  icon_list = {0};
                    GUIANIM_DATA_INFO_T data_info = {0};
                    
#ifndef MMI_PDA_SUPPORT
                    icon_list.softkey_id[LEFT_BUTTON] = STXT_OPTION;
                    icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
                    icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
                    edit_index = my_nav_index + MMIBRW_FIX_NAV_MAX_ITEM;
                
                    icon_list.name_str = title_text_str;
                    GUIICONLIST_AppendText(edit_index, MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &icon_list);
                    
                    data_info.img_id = MMIBRW_MY_NAV_DEFAULT_IMAGE_ID;//use the default image
                    GUIICONLIST_AppendIcon(edit_index, MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &data_info, PNULL);
                }
                
#ifdef MMI_GUI_STYLE_TYPICAL
                //update the control info
                MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
#endif /* MMI_GUI_STYLE_TYPICAL */
            }
            else
            {
                MMIPUB_OpenAlertFailWin(TXT_ERROR);
            }
            MMK_CloseWin(win_id);
        }
    }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CLOSE_WINDOW:
        MMK_FreeWinAddData(win_id);
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

#ifdef MMI_PDA_SUPPORT
/****************************************************************************/
//  Description :
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/****************************************************************************/
LOCAL void  BrwCreateMyNavPopUpMenu(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, uint16  cur_index)
{
    uint32                      num = 0;
    MMI_STRING_T                title_str = {0};
    MMI_TEXT_ID_T               text_index_arr[] = {TXT_DELETE, TXT_EDIT, TXT_COMM_DETAIL};
    uint32                      node_index_arr[] = {ID_POPUP_MENU_MY_NAV_DELETE, ID_POPUP_MENU_MY_NAV_EDIT, ID_POPUP_MENU_MY_NAV_DETAIL};
    MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};

    MMIBROWSER_GetMyNavInfo(&nv_info);

    if ((cur_index >= MMIBRW_MY_NAV_MAX_ITEM ))//protect
    {
        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
        MMK_CloseWin(win_id);
        return;
    }

    if (MMIBRW_MY_NAV_ITEM_TYPE_FIXED == nv_info.item_info[cur_index].item_type)
    {
        MMI_GetLabelTextByLang(nv_info.item_info[cur_index].title_info_u.title_text_id, &title_str);
    }
    else
    {
        title_str.wstr_ptr = nv_info.item_info[cur_index].title_info_u.title_warr;
        title_str.wstr_len = MMIAPICOM_Wstrlen(title_str.wstr_ptr);
    }
    
    num = sizeof(node_index_arr) / sizeof(node_index_arr[0]);
    MMIBROWSER_CreatePopupAutoMenu(win_id, ctrl_id, &title_str, num, text_index_arr, node_index_arr);
}
#endif

/*****************************************************************************/
//  Description : handle my navigation popup menu window message
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMyNavPopUpWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    uint16  cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
    MMI_WIN_ID_T query_win_id = MMIBROWSER_QUERY_WIN_ID;
    uint16  my_nav_index = 0;

    if ((cur_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
    {
        my_nav_index = cur_index - MMIBRW_FIX_NAV_MAX_ITEM;
    }
    
    switch (msg_id) 
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_PDA_SUPPORT
        BrwCreateMyNavPopUpMenu(win_id, MMIBROWSER_MY_NAV_POPUP_MENU_CTRL_ID, my_nav_index);
#endif
        MMK_SetAtvCtrl(win_id, MMIBROWSER_MY_NAV_POPUP_MENU_CTRL_ID);
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        {
            MMI_MENU_ID_T               menu_id = 0;
#ifdef MMI_PDA_SUPPORT
            menu_id = GUIMENU_GetCurNodeId(MMIBROWSER_MY_NAV_POPUP_MENU_CTRL_ID);
#else 
            MMI_MENU_GROUP_ID_T         group_id = 0;

            GUIMENU_GetId(MMIBROWSER_MY_NAV_POPUP_MENU_CTRL_ID,&group_id,&menu_id);
#endif

           switch (menu_id)
            {
            case ID_POPUP_MENU_MY_NAV_DELETE:
                MMIBROWSER_OpenQueryWinByTextId(TXT_DELETEQUERY,IMAGE_PUBWIN_QUERY,&query_win_id,PNULL);
                break;
                
            case ID_POPUP_MENU_MY_NAV_EDIT:
            {
                MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
                BOOLEAN result = FALSE;

                MMIBROWSER_GetMyNavInfo(&nv_info);
                   
                if (my_nav_index < MMIBRW_MY_NAV_MAX_ITEM )
                {                    
                    MMIBROWSER_MY_NAV_EDIT_INFO_T *nav_edit_info_ptr = PNULL;

                    nav_edit_info_ptr = SCI_ALLOCA(sizeof(MMIBROWSER_MY_NAV_EDIT_INFO_T));
                    if (nav_edit_info_ptr != PNULL)
                    {
                        wchar   url_warr[MMIBRW_MY_NAV_MAX_URL_LENGTH+1] ={0};   
                        uint16  url_len = 0;
                        MMI_STRING_T    text_str = {0};
                        
                        nav_edit_info_ptr->is_add_new = FALSE;
                        nav_edit_info_ptr->index = my_nav_index;
                        MMK_CreateWin((uint32 *)MMIBROWSER_MY_NAV_EDIT_WIN_TAB, (ADD_DATA)nav_edit_info_ptr);
                        
                        //set title
                        if (MMIBRW_MY_NAV_ITEM_TYPE_FIXED == nv_info.item_info[my_nav_index].item_type)
                        {
                            MMI_GetLabelTextByLang(nv_info.item_info[my_nav_index].title_info_u.title_text_id, &text_str);
                        }
                        else
                        {
                            text_str.wstr_ptr = nv_info.item_info[my_nav_index].title_info_u.title_warr;
                            text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
                        }    
                        GUIEDIT_SetString(MMIBROWSER_MY_NAV_EDIT_TITLE_CTRL_ID, text_str.wstr_ptr, text_str.wstr_len);
                        
                        //set url
                        url_len = SCI_STRLEN(nv_info.item_info[my_nav_index].url_arr);
                        MMI_STRNTOWSTR(url_warr, MMIBRW_MY_NAV_MAX_URL_LENGTH,
                                (const uint8*)(nv_info.item_info[my_nav_index].url_arr), url_len,
                                MIN(MMIBRW_MY_NAV_MAX_URL_LENGTH, url_len));
                        GUIEDIT_SetString(MMIBROWSER_MY_NAV_EDIT_URL_CTRL_ID, url_warr, MMIAPICOM_Wstrlen(url_warr));                    
                        result = TRUE;
                    }
                }
                
                if (!result)//error
                {
                    MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
                }
                
                MMK_CloseWin(win_id);
            }
                break;
                
            case ID_POPUP_MENU_MY_NAV_DETAIL:
                MMK_CreateWin((uint32 *)MMIBROWSER_MY_NAV_DETAIL_WIN_TAB, PNULL);
                MMK_CloseWin(win_id);
                break;

            default:
                break;
            }
            break;
        }

    case MSG_PROMPTWIN_OK:
    {
        if (RemoveOneMyNav(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID))
        {
        }
        else//error
        {
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
        }
        
        MMIBROWSER_CloseQuerytWin(&query_win_id);
        MMK_CloseWin(win_id);
    }
        break;

    case MSG_PROMPTWIN_CANCEL:
        MMIBROWSER_CloseQuerytWin(&query_win_id);
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
//  Description : set the detail window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN BrwSetMyNavDetailWindow(MMI_CTRL_ID_T ctrl_id, uint16 cur_index)
{
    GUIRICHTEXT_ITEM_T  item_data = {0};
    uint16              index = 0;
    MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
    wchar    url_warr[MMIBRW_MY_NAV_MAX_URL_LENGTH+1] ={0};   
    uint16  len = 0;
    MMI_STRING_T    text_str = {0};
    
    MMIBROWSER_GetMyNavInfo(&nv_info);
    if (cur_index >= MMIBRW_MY_NAV_MAX_ITEM)
    {
        //SCI_TRACE_LOW:"[BRW]BrwSetMyNavDetailWindow:cur_index=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_12787_112_2_18_2_6_22_232,(uint8*)"d", cur_index);
        return FALSE;  
    }

    /* Title */
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_COMM_TITLE;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    /* Title info*/
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    if (MMIBRW_MY_NAV_ITEM_TYPE_FIXED == nv_info.item_info[cur_index].item_type)
    {
        MMI_GetLabelTextByLang(nv_info.item_info[cur_index].title_info_u.title_text_id, &text_str);
    }
    else
    {
        text_str.wstr_ptr = nv_info.item_info[cur_index].title_info_u.title_warr;
        text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
    }    
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    if (text_str.wstr_len  > 0)
    {
        item_data.text_data.buf.len = text_str.wstr_len;
        item_data.text_data.buf.str_ptr = text_str.wstr_ptr;
    }
    else
    {
        item_data.text_data.buf.len = 1;
        item_data.text_data.buf.str_ptr = L" ";
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    /* Url*/
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    item_data.text_type = GUIRICHTEXT_TEXT_RES;
    item_data.text_data.res.id = TXT_INPUT_URL_ADRESS;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    /* Url info*/
    SCI_MEMSET(&item_data, 0x00, sizeof(GUIRICHTEXT_ITEM_T));
    
    len = SCI_STRLEN(nv_info.item_info[cur_index].url_arr);
    MMI_STRNTOWSTR(url_warr, MMIBRW_MY_NAV_MAX_URL_LENGTH,
            (const uint8*)(nv_info.item_info[cur_index].url_arr), len,
            MIN(MMIBRW_MY_NAV_MAX_URL_LENGTH, len));
    
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;
    if (len > 0)
    {
        item_data.text_data.buf.len = len;
        item_data.text_data.buf.str_ptr = url_warr;
    }
    else
    {
        item_data.text_data.buf.len = 1;
        item_data.text_data.buf.str_ptr = L" ";
    }
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    return TRUE;  
}

/*****************************************************************************/
//  Description : handle file detail window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleMyNavDetailWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    switch(msg_id) 
    {
    case MSG_OPEN_WINDOW:
    {        
        uint16  cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
        uint16  my_nav_index = 0;

        MMK_SetAtvCtrl(win_id,MMIBROWSER_MY_NAV_DETAIL_CTRL_ID);
        if ((cur_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
        {
            my_nav_index = cur_index - MMIBRW_FIX_NAV_MAX_ITEM;
            if (BrwSetMyNavDetailWindow(MMIBROWSER_MY_NAV_DETAIL_CTRL_ID, my_nav_index))
            {
                break;
            }
        }

        MMK_CloseWin(win_id);//error
    }
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}
#ifndef MMI_PDA_SUPPORT
/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void InitBoutiqueAndNavWin(MMI_WIN_ID_T win_id)
{
    GUI_BG_T    bg = {0};
    GUI_FONT_ALL_T font_all = {0};
    uint16  i=0;
    GUIICONLIST_MARGINSPACE_INFO_T    margin_space = {6,6,6,6};

#if defined(MMIDORADO_MY_NAV_SUPPORT) && defined(MMIDORADO_FIX_NAV_SUPPORT)     
    //set iconlist1:
    GUIICONLIST_SetTotalIcon(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM));
    GUIICONLIST_SetCurIconIndex(0,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
    GUIICONLIST_SetStyle(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, GUIICONLIST_STYLE_ICON_UIDT);
    GUIICONLIST_SetIconWidthHeight(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, 50, 50);
    font_all.color = MMI_BLACK_COLOR;
    font_all.font = MMI_DEFAULT_SMALL_FONT;
    GUIICONLIST_SetIconListTextInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, font_all);   
    GUIICONLIST_SetItemIconZoom(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, TRUE);
    bg.bg_type = GUI_BG_COLOR;
    bg.color = MMI_WHITE_COLOR;
    GUIICONLIST_SetBg(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &bg); 
    GUIICONLIST_SetIconItemSpace(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, margin_space);
    {
        GUI_BORDER_T   select_border = {0};
        GUIICONLIST_SetItemBorderStyle(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, FALSE, &select_border);
    }

#ifdef MMIDORADO_FIX_NAV_SUPPORT
    for (i = 0; i< MMIBRW_FIX_NAV_MAX_ITEM; i++)
    {
        uint16                      icon_index = 0;
        GUIICONLIST_DISP_T          icon_list = {0};
        GUIANIM_DATA_INFO_T         data_info = {0};

#ifndef MMI_PDA_SUPPORT
        icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
        icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
        icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
        //append icon text item
        icon_index = i;
        MMI_GetLabelTextByLang(g_fix_nav_default_info[icon_index].text_id,&icon_list.name_str);//title
        data_info.img_id = g_fix_nav_default_info[icon_index].img_id;

        GUIICONLIST_AppendText(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&icon_list);
        GUIICONLIST_AppendIcon(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&data_info,PNULL);
    }    
#endif    
    
#ifdef MMIDORADO_MY_NAV_SUPPORT
    AppendMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
#endif

{
    GUIICONLIST_DELIMITER_INFO_T group_info = {0};
    MMI_STRING_T        str_info = {0};
        
    group_info.start_item_index = 0;
    MMI_GetLabelTextByLang(TXT_BROWSER_RMJP, &str_info);
    group_info.delimiter_str_info = str_info;
    GUIICONLIST_CreateGroup(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &group_info);

    group_info.start_item_index = MMIBRW_FIX_NAV_MAX_ITEM;
    MMI_GetLabelTextByLang(TXT_BROWSER_WDDH, &str_info);
    group_info.delimiter_str_info = str_info;
    GUIICONLIST_CreateGroup(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &group_info);

    bg.bg_type = GUI_BG_COLOR;
    bg.color = MMI_CYAN_COLOR;
    GUIICONLIST_SetDelimiterBg(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &bg);
    font_all.color = MMI_WHITE_COLOR;
    font_all.font = MMI_DEFAULT_SMALL_FONT;
    GUIICONLIST_SetDelimiterStrInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, &font_all, 0);
    GUIICONLIST_SetDelimiterHeight(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID, (MMI_DEFAULT_SMALL_FONT_SIZE + 2));
}
#endif
    
}
/*****************************************************************************/
//  Description : handle boutique and navigation window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleBoutiqueAndNavWinMsg(
                                           MMI_WIN_ID_T	win_id, 
                                           MMI_MESSAGE_ID_E	msg_id, 
                                           DPARAM param
                                           )
{
    MMI_RESULT_E    ret=MMI_RESULT_TRUE;    

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_PDA_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        InitBoutiqueAndNavWin(win_id);
        MMK_SetAtvCtrl(win_id, MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
#ifdef MMIDORADO_MY_NAV_SUPPORT
        MMIBROWSER_SetIsIcoUpdate(FALSE);
#endif
        break;


#ifdef MMIDORADO_MY_NAV_SUPPORT
    case MSG_GET_FOCUS:
        if (MMIBROWSER_GetIsIcoUpdate())
        {
    		AppendMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            MMIBROWSER_SetIsIcoUpdate(FALSE);
        }
        break;
#endif

    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    {
        MMI_CTRL_ID_T       active_ctrl_id = 0;

#ifdef MMI_PDA_SUPPORT
        if (PNULL != param)
        {
	        active_ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;
        }
#else
        active_ctrl_id = MMK_GetActiveCtrlId(win_id);
#endif
        switch (active_ctrl_id)
        {
#if defined(MMIDORADO_MY_NAV_SUPPORT) && defined(MMIDORADO_FIX_NAV_SUPPORT)
        case MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID:
        {
            uint16  cur_index = 0;
            cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            
#if defined(MMIDORADO_FIX_NAV_SUPPORT)
            if (cur_index < MMIBRW_FIX_NAV_MAX_ITEM)
            {
                MMIBROWSER_StartWeb(MMIBROWSER_WEB_ENTRY_URL, (uint8*)g_fix_nav_default_info[cur_index].url_ptr, PNULL);
            }
            else
#endif
#if defined(MMIDORADO_MY_NAV_SUPPORT)
            if (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM))
            {
                OpenOneMyNavUrl(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
            }
            else
#endif                
            {
                //SCI_TRACE_LOW:"[BRW]BrwHandleStartPageTheme1WinMsg:err cur_index = %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13095_112_2_18_2_6_23_233,(uint8*)"d", cur_index);
            }
        }            
            break;
#endif            

        default:
            break;
        }
    }
        break;

#if defined(MMIDORADO_MY_NAV_SUPPORT)
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
    {
        uint16  cur_index = 0;
        
        cur_index = GUIICONLIST_GetCurIconIndex(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID);
        
        if ((cur_index >= MMIBRW_FIX_NAV_MAX_ITEM) && (cur_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
        {
            MMIBRW_MY_NAV_CONTEXT_T   nv_info = {0};
            uint16  my_nav_index = 0;
            
            my_nav_index = cur_index - MMIBRW_FIX_NAV_MAX_ITEM;
            MMIBROWSER_GetMyNavInfo(&nv_info);
            if (MMIBRW_MY_NAV_ITEM_TYPE_NONE != nv_info.item_info[my_nav_index].item_type)
            {
                MMK_CreateWin((uint32 *)MMIBROWSER_MY_NAV_POPUP_WIN_TAB, PNULL);
            }
        }
        
    }       
        break;
#endif

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
        MMK_CloseWin(win_id);
        break;
        
#if 0
    case MSG_CLOSE_WINDOW:
        MMIBROWSER_Exit();
        break;
#endif

    case MSG_CTL_ICONLIST_APPEND_TEXT://useful when slide or switch lcd
    {
        uint16  icon_index = *((uint16 *)param);

#ifdef MMIDORADO_FIX_NAV_SUPPORT
        if (icon_index < MMIBRW_FIX_NAV_MAX_ITEM)
        {
            GUIICONLIST_DISP_T          icon_list = {0};
            
#ifndef MMI_PDA_SUPPORT
            icon_list.softkey_id[LEFT_BUTTON] = TXT_NULL;
            icon_list.softkey_id[MIDDLE_BUTTON] = TXT_COMM_OPEN;
            icon_list.softkey_id[RIGHT_BUTTON] = STXT_RETURN;
#endif
            //append icon text item
            MMI_GetLabelTextByLang(g_fix_nav_default_info[icon_index].text_id,&icon_list.name_str);//title
            GUIICONLIST_AppendText(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&icon_list);
        }    
#endif    
#ifdef MMIDORADO_MY_NAV_SUPPORT
        if ((icon_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (icon_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
        {
            AppendOneMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,
                (icon_index - MMIBRW_FIX_NAV_MAX_ITEM), icon_index, TRUE, FALSE);
        }
#endif
    }
        break;
        
    case MSG_CTL_ICONLIST_APPEND_ICON://useful when slide or switch lcd
    {
        uint16  icon_index = *((uint16 *)param);

#ifdef MMIDORADO_FIX_NAV_SUPPORT
        if (icon_index < MMIBRW_FIX_NAV_MAX_ITEM)
        {
            GUIANIM_DATA_INFO_T         data_info = {0};
            data_info.img_id = g_fix_nav_default_info[icon_index].img_id;
            GUIICONLIST_AppendIcon(icon_index,MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,&data_info,PNULL);
        }    
#endif    
#ifdef MMIDORADO_MY_NAV_SUPPORT
        if ((icon_index >= MMIBRW_FIX_NAV_MAX_ITEM ) && (icon_index < (MMIBRW_FIX_NAV_MAX_ITEM + MMIBRW_MY_NAV_MAX_ITEM)))
        {
            AppendOneMyNavInfo(MMIBROWSER_START_PAGE_ICONLIST1_CTRL_ID,
                (icon_index - MMIBRW_FIX_NAV_MAX_ITEM), icon_index, FALSE, TRUE);
        }
#endif
    }
        break;

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }

    return ret;
}

/*****************************************************************************/
//  Description : open Boutique And Nav window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenBoutiqueAndNavWin(void)
{
    MMK_CreateWin((uint32*)MMIBROWSER_BOUTIQUE_AND_NAV_WIN_TAB,PNULL);
}
#endif

//my navigation:我的导航end
#endif

/*****************************************************************************/
//  Description : set public profile
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BRW_PUBLIC_PROFILE_T MMIBROWSER_SetPublicProfile(void)
{
    BRW_PUBLIC_PROFILE_T    profile={0};
    MMIBROWSER_NV_SETTING_T *nv_setting_ptr = PNULL;
    MMICONNECTION_LINKSETTING_DETAIL_T  *setting_item_ptr = PNULL;
    MMISET_LANGUAGE_TYPE_E  language_type = 0;
    char    *user_agent_ptr = PNULL;
    wchar   *device_name_ptr=PNULL;
    char    *ua_profile_ptr = PNULL;
    uint16  device_name_len = 0;
#ifdef WIFI_SUPPORT
    MMIWLAN_PROXY_SETTING_DETAIL_T detail_struct = {0};
#endif
    MN_DUAL_SYS_E   active_sim = MMIBROWSER_GetSim();

    nv_setting_ptr = MMIBROWSER_GetNVSetting();
    if (PNULL == nv_setting_ptr)
    {
        //SCI_TRACE_LOW:"MMIBROWSER_SetPublicProfile MMIBROWSER_GetNVSetting error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13187_112_2_18_2_6_23_234,(uint8*)"");
        return profile;
    }
#ifdef MMIDORADO_BGSOUND_SUPPORT
    profile.is_bgsound_download = nv_setting_ptr->is_sound_played;
#else
    profile.is_bgsound_download = FALSE;
#endif
    MMIBROWSER_GetBrwSysDevName(&device_name_ptr, &device_name_len);
    if (PNULL == device_name_ptr)
    {
        profile.is_cache_enable = FALSE;
        profile.is_cookie_enable = FALSE;
    }
    else
    {
        profile.is_cache_enable=nv_setting_ptr->is_cache_enable;
        profile.is_cookie_enable=nv_setting_ptr->is_cookie_enable;
    }
    profile.is_image_download=nv_setting_ptr->is_image_loaded;
    profile.is_gwt_enable = nv_setting_ptr->is_google_change_enable;

    profile.dual_sys = active_sim;
    
    profile.is_use_wifi = MMIBROWSER_GetIsUseWifi();
    if(profile.is_use_wifi)
    {
        profile.proxy_opt.is_use_proxy = FALSE;
        profile.protocol = BRW_PROTOCOL_HTTP;
#ifdef WIFI_SUPPORT
        MMIAPIWIFI_GetProxySettingDetail(&detail_struct);
        profile.proxy_opt.is_use_proxy = detail_struct.is_use_proxy;
        if(profile.proxy_opt.is_use_proxy)
        {
            profile.proxy_opt.proxy_addr_ptr = detail_struct.gateway;
            profile.proxy_opt.proxy_port = detail_struct.port;
            profile.psw_ptr = (char*)detail_struct.password;
            profile.username_ptr = (char*)detail_struct.username;
        }
#endif
    }
    else
    {
        uint8 connection_index = 0;
        MMIDORADO_CUSTOMIZED_CONNECTION_T   customized_connection = MMIDORADO_GetConnectionSettings();
        BOOLEAN is_valid_customized_connection = FALSE;

        if (IS_VALID_CUSTOMIZED_CONNECTION(customized_connection, active_sim))
        {
            is_valid_customized_connection = TRUE;
            //SCI_TRACE_LOW:"[BRW]MMIBROWSER_SetProfile:use customized_connection"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13237_112_2_18_2_6_23_235,(uint8*)"");
        }      
        
        if (!is_valid_customized_connection)
        {
            connection_index = MMIDORADO_GetNetSettingIndex(active_sim);
        }
        else
        {
            connection_index = customized_connection.connection_index;
        }
        //SCI_TRACE_LOW:"[BRW]MMIBROWSER_SetProfile:index=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13248_112_2_18_2_6_23_236,(uint8*)"d", connection_index);
               
        setting_item_ptr = MMIAPICONNECTION_GetLinkSettingItemByIndex(active_sim, connection_index);
        if (PNULL == setting_item_ptr)
        {
            //SCI_TRACE_LOW:"[BRW]W MMIBROWSER_SetPublicProfile:GetLinkSettingItemByIndex error"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13253_112_2_18_2_6_23_237,(uint8*)"");
            return profile;
        }
        profile.auth_type = setting_item_ptr->auth_type;
        profile.proxy_opt.proxy_addr_ptr = setting_item_ptr->gateway;
        profile.proxy_opt.proxy_port = setting_item_ptr->port;
        
        if (0 != strncmp(MMIBROWSER_INVALID_GATEWAY,(const char*)(setting_item_ptr->gateway),strlen(MMIBROWSER_INVALID_GATEWAY))&&
            0!=strlen((const char*)(setting_item_ptr->gateway)))
        {
            profile.proxy_opt.is_use_proxy = TRUE;
        }
        else
        {
            profile.proxy_opt.is_use_proxy = FALSE;
        }
        profile.psw_ptr = (char*)(setting_item_ptr->password);
        profile.username_ptr = (char*)(setting_item_ptr->username);
        profile.apn_ptr = (char*)(setting_item_ptr->apn);
        
        if (0 == setting_item_ptr->access_type)
        {
            profile.protocol = BRW_PROTOCOL_WAP;
        }
        else
        {
            profile.protocol = BRW_PROTOCOL_HTTP;
        }
    }
    
    MMIAPISET_GetLanguageType(&language_type);
    if (MMISET_LANGUAGE_ENGLISH == language_type)
    {
        profile.lang = BRW_LANG_EN;
    }
    else
    {
        profile.lang = BRW_LANG_CH;
    }
    
    profile.browser_pdp_priority = 1;
    //profile.msg_proc_func = MMIBROWSER_HandleCtrlMsg;
   
    ua_profile_ptr = MMIBROWSER_GetUaProfile();
    if (PNULL != ua_profile_ptr)
    {
        profile.ua_prof_ptr = (uint8*)ua_profile_ptr;
    }
    else
    {
        profile.ua_prof_ptr = (uint8*)MMIAPICOM_GetUserAgentProfile();//(uint8*)MMIBROWSER_DEFAULT_UAPROFILE;
    }
    
    user_agent_ptr = MMIBROWSER_GetUserAgent();
    if (PNULL != user_agent_ptr)
    {
        profile.user_agent_ptr = (uint8*)user_agent_ptr;
    }
    else
    {
#if defined(CSC_XML_SUPPORT)
        user_agent_ptr = (char *)MMIAPICUS_GetDefaultUAFromCSC();

        if (PNULL != user_agent_ptr)
        {
            profile.user_agent_ptr = (uint8*)user_agent_ptr;
        }
#if defined(CUSTOM_DEFINE_UA)//客户自定义UA
        else
        {
            profile.user_agent_ptr = (char *)MMIAPICOM_GetUserAgent();        
        }
#endif

#elif defined(CUSTOM_DEFINE_UA)//客户自定义UA
        profile.user_agent_ptr = (char *)MMIAPICOM_GetUserAgent();        
#else
        /*get UA in CONTROL,when start wap request. MMIBROWSER_GetDefaultUserAgent()
            Set different User_Agent according to different network. */
#endif
    }
    
/*    switch (nv_setting_ptr->font)
    {
    case MMIBROWSER_FONT_MINI:
        profile.font_type = MMIBROWSER_SMALL_FONT;
        break;
    case MMIBROWSER_FONT_MIDDLE:
        profile.font_type = MMIBROWSER_NORMAL_FONT;
        break;
    case MMIBROWSER_FONT_BIG:
        profile.font_type = MMIBROWSER_BIG_FONT;
        break;
        
    default:
        profile.font_type = MMIBROWSER_NORMAL_FONT;
        break;
    }
*/    
    return profile;
}

#ifdef MMIDORADO_FIX_HMTL_SUPPORT
/*****************************************************************************/
//  Description : DisplayFixHtmlPage
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void DisplayFixHtmlPage(BRW_INSTANCE instance)
{
    if (MMK_IsFocusWin(MMIBROWSER_FIX_HTML_WIN_ID))
    {
        GUILABEL_SetVisible(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  FALSE, TRUE);
        BRW_DisplayPage(instance);
        s_fix_display_info.is_need_display= FALSE;
    }
    else
    {
        GUILABEL_SetVisible(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  FALSE, FALSE);
        s_fix_display_info.is_need_display= TRUE;
    }
}

/*****************************************************************************/
//  Description : get profile for BRW_SCREEN_PROFILE for fix html
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_GetFixHtmlScreenProfile(BRW_SCREEN_OPTION_T *screen_opt_ptr, MMI_WIN_ID_T win_id)
{
    GUI_RECT_T client_rect = {0};
    
    if (PNULL == screen_opt_ptr)
    {
        return;
    }
    
    client_rect = MMITHEME_GetClientRectEx(win_id);
    screen_opt_ptr->width = client_rect.right - client_rect.left + 1;
    screen_opt_ptr->left_top_x = client_rect.left;
    screen_opt_ptr->height = client_rect.bottom - client_rect.top + 1;
    screen_opt_ptr->left_top_y = client_rect.top;
}

/*****************************************************************************/
//  Description : handle control message of fixed instance:异步asynchronous
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN HandleFixedInstanceControlMsg(MMI_MESSAGE_ID_E msg_id,void* param)
{
    //BOOLEAN ret = TRUE;
    MMI_MESSAGE_ID_E    message_id = (msg_id - MSG_BRW_START_ID);
    BRW_INSTANCE    instance = MMIBROWSER_GetFixedInstance();
    
    //SCI_TRACE_LOW:"[BRW] HandleFixedInstanceControlMsg msg_id:%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13495_112_2_18_2_6_23_238,(uint8*)"d", message_id);
    
    switch(message_id)
    {
    case BRW_MSG_TYPE_PDP_ACTIVE_CNF:
        if (MMIBROWSER_FIX_DISPLAY_TYPE_MAX == s_fix_display_info.display_type)
        {
            if (0 == BRW_AccessPage(instance,(uint8*)MMIBROWSER_FIX_HTML_URL))
            {
                SCI_TRACE_LOW("[BRW] HandleFixedInstanceControlMsg :BRW_AccessPage fail");
                s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT;
                if (MMK_IsOpenWin(MMIBROWSER_FIX_HTML_WIN_ID))
                {
                    MMIBROWSER_OpenSnapshot(PNULL, instance, TRUE);
                    s_fix_display_info.is_content_valid = TRUE;
                    DisplayFixHtmlPage(instance);
                }
                else
                {
                    s_fix_display_info.is_content_valid = FALSE;
                }            
            }
            else
            {
                s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_PAGE;
                s_fix_display_info.is_content_valid = FALSE;
            }
        }
        else
        {
            //has in status,don't handle it
        }
        break;

    case BRW_MSG_TYPE_ERROR_IND:
    {
        BRW_MSG_ERROR_IND_T *err_ind=PNULL;

        if (PNULL != param)
        {
            err_ind=(BRW_MSG_ERROR_IND_T*)param;
                        
            switch(err_ind->error)
            {
            case BRW_ERR_GPRS_LINK_FAILED:
            //case BRW_ERR_GPRS_LINK_DISCONNECT:
                if (MMIBROWSER_FIX_DISPLAY_TYPE_MAX == s_fix_display_info.display_type)
                {
                    if (0 == BRW_AccessPage(instance,(uint8*)MMIBROWSER_FIX_HTML_URL))
                    {
                        SCI_TRACE_LOW("[BRW] HandleFixedInstanceControlMsg :BRW_AccessPage fail");
                        s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT;
                        if (MMK_IsOpenWin(MMIBROWSER_FIX_HTML_WIN_ID))
                        {
                            MMIBROWSER_OpenSnapshot(PNULL, instance, TRUE);
                            s_fix_display_info.is_content_valid = TRUE;
                            DisplayFixHtmlPage(instance);
                        }
                        else
                        {
                            s_fix_display_info.is_content_valid = FALSE;
                        }
                    }
                    else
                    {
                        s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_PAGE;
                        s_fix_display_info.is_content_valid = FALSE;
                    }
                }
                else if (MMIBROWSER_FIX_DISPLAY_TYPE_PAGE == s_fix_display_info.display_type)
                {
                    BRW_StopPageReq(instance);
                    s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT;
                    if (MMK_IsOpenWin(MMIBROWSER_FIX_HTML_WIN_ID))
                    {
                        MMIBROWSER_OpenSnapshot(PNULL, instance, TRUE);
                        s_fix_display_info.is_content_valid = TRUE;
                        DisplayFixHtmlPage(instance);
                    }
                    else
                    {
                        s_fix_display_info.is_content_valid = FALSE;
                    }            
                }
                else
                {
                    //do nothing
                }
                break;
            
            case BRW_ERR_NO_ERROR:
                break;
                
            default:
                if (MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT != s_fix_display_info.display_type)
                {
                    BRW_StopPageReq(instance);
                    s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT;
                    if (MMK_IsOpenWin(MMIBROWSER_FIX_HTML_WIN_ID))
                    {
                        MMIBROWSER_OpenSnapshot(PNULL, instance, TRUE);
                        s_fix_display_info.is_content_valid = TRUE;
                        DisplayFixHtmlPage(instance);
                    }
                    else
                    {
                        s_fix_display_info.is_content_valid = FALSE;
                    }            
                }
                break;
            }
        }
    }
        break;

    case BRW_MSG_TYPE_DISPLAY_READY_IND:
        s_fix_display_info.is_content_valid = TRUE;
        DisplayFixHtmlPage(instance);
        break;   
        
    case BRW_MSG_TYPE_REPAINT_READY_IND:
        if (MMK_IsFocusWin(MMIBROWSER_FIX_HTML_WIN_ID))
        {
            BRW_RepaintPage(instance);
        }
        break;
        
    case BRW_MSG_TYPE_PAGE_STOP_IND:
        if (MMIBROWSER_FIX_DISPLAY_TYPE_PAGE == s_fix_display_info.display_type)
        {
            s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT;
            if (MMK_IsOpenWin(MMIBROWSER_FIX_HTML_WIN_ID))
            {
                MMIBROWSER_OpenSnapshot(PNULL, instance, TRUE);
                s_fix_display_info.is_content_valid = TRUE;
                DisplayFixHtmlPage(instance);
            }
            else
            {
                s_fix_display_info.is_content_valid = FALSE;
            }            
        }
        break;

    default:
        break;
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : handle control message of fixed instance
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIBROWSER_HandleFixedCtrlMsg(BRW_EXPORT_MSG_E message_id,void* param,uint16 size_of_param)
{
    SCI_TRACE_LOW("[BRW]MMIBROWSER_HandleFixedCtrlMsg:message_id = %d", message_id);

    return MMK_PostMsg(MMIBROWSER_START_PAGE_WIN_ID, (message_id + MSG_BRW_START_ID), param, size_of_param);    
}

/*****************************************************************************/
//  Description : set profile for fix instance
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC BRW_PROFILE_T MMIBROWSER_SetFixHtmlProfile(MMI_WIN_ID_T	win_id)
{
    BRW_PROFILE_T                       profile={0};

    profile.scroll_bar_handler = s_fix_display_info.scroll_handle;    
    MMIBROWSER_GetFixHtmlScreenProfile(&profile.screen_opt, win_id);
       
    profile.win_handle = win_id;  
    profile.font_type = MMIBROWSER_NORMAL_FONT;
    
    profile.msg_proc_func = MMIBROWSER_HandleFixedCtrlMsg;
    
    return profile;
}

/*****************************************************************************/
//  Description : handle Fix Html window message
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleFixHtmlWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E                        ret=MMI_RESULT_TRUE;
    BRW_INSTANCE    brw_instance = MMIBROWSER_GetFixedInstance();
    uint8   timer_id = 0;
    //SCI_TRACE_LOW:"BrwHandleFixHtmlWinMsg:msg_id = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13604_112_2_18_2_6_24_240,(uint8*)"d", msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_PDA_SUPPORT        
        GUIWIN_SetTitleButtonTextId(win_id, GUITITLE_BUTTON_LEFT, STXT_EXIT, FALSE);
#endif   
        if (brw_instance != BRW_INVALID_INSTANCE)
        {
            BRW_PROFILE_T   profile={0};
                        
            //scroll
            {
                GUI_BOTH_RECT_T client_both_rect = {0};
                GUI_BOTH_RECT_T scroll_both_rect = {0};
                uint32  scroll_handle = 0;
               
                client_both_rect = MMITHEME_GetWinClientBothRect(win_id);
                scroll_both_rect.v_rect.right = client_both_rect.v_rect.right;
                scroll_both_rect.v_rect.left = scroll_both_rect.v_rect.right - 4;
                scroll_both_rect.v_rect.top = client_both_rect.v_rect.top;
                scroll_both_rect.v_rect.bottom = client_both_rect.v_rect.bottom;
                scroll_both_rect.h_rect.right = client_both_rect.h_rect.right;
                scroll_both_rect.h_rect.left = scroll_both_rect.h_rect.right - 4;
                scroll_both_rect.h_rect.top = client_both_rect.h_rect.top;
                scroll_both_rect.h_rect.bottom = client_both_rect.h_rect.bottom;

#if !defined(MMI_PDA_SUPPORT)
                scroll_handle = BrwScrollBarCreate(win_id, &scroll_both_rect, IMAGE_BROWSER_SCROLL_VER_GROOVE);
#else
                scroll_handle = BrwScrollBarCreate(win_id, &scroll_both_rect, IMAGE_NULL);
#endif
                s_fix_display_info.scroll_handle = scroll_handle;
            }

            profile = MMIBROWSER_SetFixHtmlProfile(win_id);
            BRW_SetParam(brw_instance, &profile, BRW_SCREEN_PROFILE);
            GUILABEL_SetVisible(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  FALSE, FALSE);
#ifndef BROWSER_START_PAGE_THEME1
            if ((MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT == s_fix_display_info.display_type) 
                && !s_fix_display_info.is_content_valid)
            {
                MMIBROWSER_OpenSnapshot(PNULL, brw_instance, TRUE);
                s_fix_display_info.is_content_valid = TRUE;
            }
            else  if (MMIBROWSER_FIX_DISPLAY_TYPE_MAX == s_fix_display_info.display_type)
            {
                if (0 == BRW_AccessPage(brw_instance,(uint8*)MMIBROWSER_FIX_HTML_URL))
                {
                    SCI_TRACE_LOW("[BRW] HandleFixedInstanceControlMsg :BRW_AccessPage fail");
                    s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT;
                    MMIBROWSER_OpenSnapshot(PNULL, brw_instance, TRUE);
                    s_fix_display_info.is_content_valid = TRUE;
                }
                else
                {
                    s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_PAGE;
                    s_fix_display_info.is_content_valid = FALSE;
                }
            }                
#endif
       }
        break;

    case MSG_LCD_SWITCH:
    {
        BRW_PROFILE_T   profile={0};

        profile = MMIBROWSER_SetFixHtmlProfile(win_id);
        BRW_SetParam(MMIBROWSER_GetFixedInstance(), &profile, BRW_SCREEN_PROFILE);
    }
        break; 
        
    case MSG_FULL_PAINT:
    {
        GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
        GUI_RECT_T client_rect = {0};
        BOOLEAN is_display_success = FALSE;
                    
        client_rect = MMITHEME_GetClientRectEx(win_id);
        GUI_FillRect(&lcd_dev_info, client_rect, MMI_WHITE_COLOR);

        if ((MMIBROWSER_FIX_DISPLAY_TYPE_MAX == s_fix_display_info.display_type) 
            || (!s_fix_display_info.is_content_valid))
        {
            //please waiting
            GUILABEL_SetTextById(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  TXT_COMMON_WAITING, FALSE);
            GUILABEL_SetVisible(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  TRUE, TRUE);
        }
        else
        {
            GUILABEL_SetVisible(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  FALSE, TRUE);
            if (s_fix_display_info.is_need_display)
            {
                is_display_success = BRW_DisplayPage(brw_instance);
                s_fix_display_info.is_need_display = FALSE;
            }
            else
            {
                is_display_success = BRW_RepaintPage(brw_instance);
            }

            if (!is_display_success) //error
            {
                SCI_TRACE_LOW("[BRW]W BrwHandleFixHtmlWinMsg:DisplayPage error instance=%d", brw_instance);
                GUILABEL_SetTextById(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  TXT_BROWSER_HTTP_ERROR, FALSE);
                GUILABEL_SetVisible(MMIBROWSER_FIX_HTML_LABEL_CTRL_ID,  TRUE, TRUE);
            }
        }
    }
        break;
         
    case MSG_CLOSE_WINDOW: 
        MMIBROWSER_StopKeyRepeatTimer();
        MMIBROWSER_SetKeyTimerID(MMI_NONE_TIMER_ID);
        MMIBROWSER_SetKeyMSG(0);
        
        //BrwSetSnapshotOpenLayoutHandler(brw_instance, 0);
        BRW_SnapshotEndOfflinePage();
        BRW_FreeBrwInfoStructForFixed(brw_instance);
        
        s_fix_display_info.is_content_valid = FALSE;
        s_fix_display_info.is_need_display = TRUE;
        s_fix_display_info.scroll_handle = 0;
#ifndef BROWSER_START_PAGE_THEME1
        if (MMIBROWSER_FIX_DISPLAY_TYPE_SNAPSHOT != s_fix_display_info.display_type) 
        {
            s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_MAX;
        }
#endif                
        break;

    case MSG_KEYPRESSUP_LEFT:   //fall through
    case MSG_KEYPRESSUP_DOWN:   //fall through
    case MSG_KEYPRESSUP_RIGHT:  //fall through
    case MSG_KEYPRESSUP_UP:     //fall through
    case MSG_KEYUP_LEFT:        //fall through
    case MSG_KEYUP_DOWN:        //fall through
    case MSG_KEYUP_RIGHT:       //fall through
    case MSG_KEYUP_UP:
        MMIBROWSER_StopKeyRepeatTimer();
        MMIBROWSER_SetKeyTimerID(MMI_NONE_TIMER_ID);
        MMIBROWSER_SetKeyMSG(0);
        break;

    case MSG_APP_UP:
        if(brw_instance != BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_UP))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 1"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13738_112_2_18_2_6_24_243,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;

    case MSG_APP_DOWN:
        if(brw_instance != BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_DOWN))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 2"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13750_112_2_18_2_6_24_244,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;

    case MSG_APP_LEFT:
        if(brw_instance != BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_LEFT))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 3"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13762_112_2_18_2_6_24_245,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;

    case MSG_APP_RIGHT:
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_RIGHT))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 4"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13774_112_2_18_2_6_24_246,(uint8*)"");
            }
            MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_DELAY);
            MMIBROWSER_SetKeyMSG(msg_id);
        }
        break;      

    case MSG_CTL_MIDSK://MS00188753
    case MSG_APP_WEB:
        if(brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_OK))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcKeyEvent error 7"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13787_112_2_18_2_6_24_247,(uint8*)"");
            }
        }
        break;
        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL: 
#ifdef MMI_PDA_SUPPORT
        MMIBROWSER_QueryExit();
#else
        MMK_CloseWin(win_id);
#endif
        break;

    case MSG_TIMER:
        {
            timer_id = *(uint8*)param;
            if(timer_id == MMIBROWSER_GetKeyTimerID()) 
            {
                /*key timer*/
                MMIBROWSER_StopKeyRepeatTimer();
                MMIBROWSER_SetKeyTimerID(MMI_NONE_TIMER_ID);
                if (MMK_IsFocusWin(win_id) && brw_instance!=BRW_INVALID_INSTANCE) 
                {
                    if(MSG_APP_LEFT==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_LEFT_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    } 
                    else if (MSG_APP_RIGHT==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_RIGHT_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    } 
                    else if (MSG_APP_UP==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_UP_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    } 
                    else if (MSG_APP_DOWN==MMIBROWSER_GetKeyMSG()) 
                    {
                        BRW_ProcKeyEvent(brw_instance,BRW_KEY_EVENT_DOWN_REPEAT);
                        MMIBROWSER_StartKeyRepeatTimer(win_id, MMIBROSER_KEY_REPEAT_INTERVAL);
                    }
                }
            } 
            else if (s_browser_tp_info.slide_timer_id == timer_id)
            {
                HandleSlideTimer(win_id,brw_instance);
            }
            else 
            {
                /*layout timer*/
                if(MMK_IsFocusWin(win_id) && !MMIBROWSER_GetLoading() && (brw_instance != BRW_INVALID_INSTANCE)) 
                {
                    if (!BRW_ProcLayoutTimer(brw_instance,timer_id)) 
                    {
                        //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcLayoutTimer error"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13844_112_2_18_2_6_24_248,(uint8*)"");
                    }                    
                } 
                else 
                {
                    BRW_StopLayoutTimerById(brw_instance, timer_id);
                }
            }
        }
        break;
        
    case MSG_CTL_PRGBOX_MOVEUP_ONE:
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_UPLINE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 1"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13860_112_2_18_2_6_24_249,(uint8*)"");
            }
        }
        
        break;

    case MSG_CTL_PRGBOX_MOVEDOWN_ONE:
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_DOWNLINE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 2"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13871_112_2_18_2_6_24_250,(uint8*)"");
            }
        }
        break;

    case MSG_CTL_PRGBOX_MOVEUP_PAGE:
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_UPPAGE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 3"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13881_112_2_18_2_6_24_251,(uint8*)"");
            }
        }
        break;

    case MSG_CTL_PRGBOX_MOVEDOWN_PAGE:
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_DOWNPAGE,0))
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 4"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13891_112_2_18_2_6_24_252,(uint8*)"");
            }
        }
        break;
    case MSG_CTL_PRGBOX_MOVE:
        if (brw_instance!=BRW_INVALID_INSTANCE)
        {
            uint32 top_index = *(uint32*)param;
            if (!BRW_ProcScrollEvent(brw_instance,BRW_SCROLL_EVENT_MOVE_VERTICAL, top_index) )
            {
                //SCI_TRACE_LOW:"\n MMIBROWSER BrwHandleMainWinMsg BRW_ProcScrollEvent error 4"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13901_112_2_18_2_6_24_253,(uint8*)"");
            }
        }
        break;
      
    case MSG_TP_PRESS_MOVE:        
    case MSG_TP_PRESS_DOWN:        
    case MSG_TP_PRESS_UP:
        MMIBROWSER_HandleTpPress(win_id, msg_id, param, brw_instance);
        break;

    default:
        ret = MMI_RESULT_FALSE;
        break;
    }
    return ret;
}

/*****************************************************************************/
//  Description : open Fix Html window
//  Global resource dependence : 
//  Author: fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_OpenFixHtmlWin(void)
{
    MMK_CreateWin((uint32*)MMIBROWSER_FIX_HTML_WIN_TAB,PNULL);
}

/*****************************************************************************/
//  Description : MMIBROWSER_InitFixInfo
//  Global resource dependence : s_fix_display_info
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_InitFixInfo(void)
{
    s_fix_display_info.display_type = MMIBROWSER_FIX_DISPLAY_TYPE_MAX;
    s_fix_display_info.is_content_valid = FALSE;
    s_fix_display_info.is_need_display= TRUE;
    s_fix_display_info.scroll_handle = 0;
}
#endif

/*****************************************************************************/
//  Description : handle the link func of the added to mainmenu's url
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL void Add2MainLinkFunc(void* param1,void *param2)
{
    uint8   *url_ptr = (uint8 *)param1;

    if (url_ptr != PNULL)
    {
        MMIBROWSER_ENTRY_PARAM entry_param = {0};
        
        entry_param.type = MMIBROWSER_ACCESS_URL;
        entry_param.dual_sys = MN_DUAL_SYS_MAX;
        entry_param.url_ptr = (char *)url_ptr;
        MMIAPIBROWSER_Entry(&entry_param);
    }
    else
    {
        MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
    }
}

/*****************************************************************************/
//  Description : handle the delete func of the added to mainmenu's url
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN Add2MainDelFunc(void* param1, void* param2)
{
    //SCI_TRACE_LOW:"[WRE] Add2MainDelFunc  begin:"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_13982_112_2_18_2_6_25_254,(uint8*)"");
    return TRUE;
}

/*****************************************************************************/
//  Description : handle add to mainmenu edit window message
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E BrwHandleAdd2MainEditWinMsg(
                                   MMI_WIN_ID_T	win_id, 
                                   MMI_MESSAGE_ID_E	msg_id, 
                                   DPARAM		param
                                   )
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;

    switch(msg_id) 
    {
    case MSG_OPEN_WINDOW:
    {
#ifdef MMI_PDA_SUPPORT
        GUIFORM_SetType(MMIBROWSER_ADD2MAIN_EDIT_CTRL_ID, GUIFORM_TYPE_TP);
        GUIEDIT_SetAlign(MMIBROWSER_ADD2MAIN_EDIT_TITLE_CTRL_ID, ALIGN_LVMIDDLE);
        GUIEDIT_SetAlign(MMIBROWSER_ADD2MAIN_EDIT_URL_CTRL_ID, ALIGN_LVMIDDLE);
#endif /* MMI_PDA_SUPPORT */
        GUIFORM_SetStyle(MMIBROWSER_ADD2MAIN_EDIT_FORM1_CTRL_ID,GUIFORM_STYLE_UNIT);
        GUIFORM_SetStyle(MMIBROWSER_ADD2MAIN_EDIT_FORM2_CTRL_ID,GUIFORM_STYLE_UNIT);
        
        GUILABEL_SetTextById(MMIBROWSER_ADD2MAIN_EDIT_LABEL1_CTRL_ID, TXT_BROWSER_FILE_TITLE, FALSE);
        GUILABEL_SetTextById(MMIBROWSER_ADD2MAIN_EDIT_LABEL2_CTRL_ID, TXT_URL_ADRESS, FALSE);
        GUIEDIT_SetIm(MMIBROWSER_ADD2MAIN_EDIT_URL_CTRL_ID,(uint32)GUIIM_TYPE_ENGLISH | (uint32)GUIIM_TYPE_DIGITAL | (uint32)GUIIM_TYPE_ABC,GUIIM_TYPE_ABC);//MS00220112
        GUIEDIT_SetImTag(MMIBROWSER_ADD2MAIN_EDIT_URL_CTRL_ID, GUIIM_TAG_URL);

        GUIWIN_SetTitleTextId(win_id, TXT_EDIT, FALSE);
        MMK_SetAtvCtrl(win_id,MMIBROWSER_ADD2MAIN_EDIT_CTRL_ID);
    }
        break;
        
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        {
            MMI_CTRL_ID_T src_ctrl_id = 0;

            if (PNULL != param)
            {
                src_ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

                if (src_ctrl_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_OK, PNULL, 0);
                }
                else if (src_ctrl_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_MIDSK, PNULL, 0);
                }
                else if (src_ctrl_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
                {
                    MMK_PostMsg(win_id, MSG_CTL_CANCEL, PNULL, 0);
                }
            }
        }
        break;
#endif /* MMI_PDA_SUPPORT */

    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
#ifndef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK: 
#endif /* MMI_PDA_SUPPORT */
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
        MMI_STRING_T url_text_str = {0};
        MMI_STRING_T title_text_str = {0};
        GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr = PNULL;
        GUIMENU_FILE_DEVICE_E store_idx = GUIMENU_DEVICE_SDCARD_1;
        MMIMENU_SYN_VALUE_E ret = MMIMENU_SYN_VALUE_SUC;
        uint8   *url_ptr = PNULL;
        uint8 *buf_ptr = PNULL;
        uint32 to_read_size = 0;
        uint16  web_ico_name[MMIFILE_FULL_PATH_MAX_LEN+ 1] = {0};
        uint16  web_ico_name_len = MMIFILE_FULL_PATH_MAX_LEN;
#ifdef NANDBOOT_SUPPORT
        MMIFILE_DEVICE_E    valid_device = MMIAPIFMM_GetFirstValidDevice();
#else
        MMIFILE_DEVICE_E    valid_device = MMIAPIFMM_GetFirstValidSD();
#endif

        if (valid_device < MMI_DEVICE_NUM)
        {
            store_idx = (GUIMENU_FILE_DEVICE_E)(valid_device);
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_PLEASE_INSERT_SD);
            MMK_CloseWin(win_id);
            break;
        }

        GUIEDIT_GetString(MMIBROWSER_ADD2MAIN_EDIT_TITLE_CTRL_ID, &title_text_str);
        GUIEDIT_GetString(MMIBROWSER_ADD2MAIN_EDIT_URL_CTRL_ID, &url_text_str);
        if ((PNULL == title_text_str.wstr_ptr) || (0 == title_text_str.wstr_len))
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMM_NO_TITLE);
            break;
        }
        if ((PNULL == url_text_str.wstr_ptr) || (0 == url_text_str.wstr_len))
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMM_URL_NULL);
            break;
        }
               
        dynamic_item_ptr = SCI_ALLOCA(sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T));
        if (PNULL == dynamic_item_ptr)
        {
            //SCI_TRACE_LOW:"[BRW]ID_POPUP_MENU_ADD_TO_MAINMENU: alloc dynamic_item_ptr return PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_14078_112_2_18_2_6_25_255,(uint8*)"");
            MMIPUB_OpenAlertWarningWin(TXT_UNKNOWN_ERROR);
            MMK_CloseWin(win_id);
            break;
        }
        
        SCI_MEMSET(dynamic_item_ptr,0,sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T));

        BrwGetWebIcoFullName(web_ico_name, &web_ico_name_len);
        web_ico_name_len = MMIAPICOM_Wstrlen(web_ico_name);
        if (MMIAPIFMM_GetFileInfo(web_ico_name, web_ico_name_len, &to_read_size, PNULL, PNULL)
            && (to_read_size > 0))
        {
            buf_ptr = SCI_ALLOCAZ(to_read_size + 1);
            if (buf_ptr != PNULL)
            {
                IMG_DEC_SRC_T   img_dec_src = {0};
                IMG_DEC_INFO_T  img_info_param ={0};

                result = MMIAPIFMM_ReadFilesDataSyn(web_ico_name, web_ico_name_len, buf_ptr, to_read_size);
                if (result)
                {
                    img_dec_src.src_file_size = to_read_size;
                    img_dec_src.src_ptr = buf_ptr;
                    
                    if (IMG_DEC_RET_SUCCESS == IMG_DEC_GetInfo(&img_dec_src, &img_info_param))
                    {
                        dynamic_item_ptr->dynamic_menu_info.icon_datasize = to_read_size;
                        dynamic_item_ptr->dynamic_menu_info.icon_ptr = (uint8 *)buf_ptr;
                        dynamic_item_ptr->dynamic_menu_info.icon_height = img_info_param.img_detail_info.ico_info.img_height[0];
                        dynamic_item_ptr->dynamic_menu_info.icon_width = img_info_param.img_detail_info.ico_info.img_width[0];
                        
                        dynamic_item_ptr->dynamic_menu_info.has_icon = TRUE;
                    }

                }
            }
        }

        dynamic_item_ptr->dynamic_menu_info.text_len = MIN(title_text_str.wstr_len, GUIMENU_DYMAINMENU_STR_MAX_NUM);
        MMI_WSTRNCPY(dynamic_item_ptr->dynamic_menu_info.text, dynamic_item_ptr->dynamic_menu_info.text_len, 
            title_text_str.wstr_ptr, dynamic_item_ptr->dynamic_menu_info.text_len, 
            dynamic_item_ptr->dynamic_menu_info.text_len);

        url_ptr = SCI_ALLOCA(url_text_str.wstr_len + 1);//free it in Add2MainDelFunc
        if (url_ptr != PNULL)
        {
            BOOLEAN flag = TRUE;
            
            SCI_MEMSET(url_ptr,0,(url_text_str.wstr_len + 1));
            MMI_WSTRNTOSTR(url_ptr, url_text_str.wstr_len, url_text_str.wstr_ptr, url_text_str.wstr_len, url_text_str.wstr_len);

            dynamic_item_ptr->dynamic_menu_info.link_function_ptr = Add2MainLinkFunc;
            dynamic_item_ptr->dynamic_menu_info.delete_function_ptr = Add2MainDelFunc;
            dynamic_item_ptr->dynamic_menu_info.param1ptr = url_ptr;
            dynamic_item_ptr->dynamic_menu_info.parm1_size = (url_text_str.wstr_len + 1);
            //temp
            dynamic_item_ptr->dynamic_menu_info.param2ptr = &flag;
            dynamic_item_ptr->dynamic_menu_info.parm2_size = sizeof(flag);

            ret = MMIMENU_DyAppendItem(dynamic_item_ptr, store_idx);
        }
        else
        {
            //SCI_TRACE_LOW:"[BRW]ID_POPUP_MENU_ADD_TO_MAINMENU: alloc url_ptr return PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_14132_112_2_18_2_6_25_256,(uint8*)"");
        }
        //SCI_TRACE_LOW:"[BRW] ID_POPUP_MENU_ADD_TO_MAINMENU: ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIBROWSER_WINTABLE_14134_112_2_18_2_6_25_257,(uint8*)"d",ret);
        
        if (url_ptr != PNULL)
        {            
            SCI_FREE(url_ptr);
        }
        if (buf_ptr != PNULL)
        {            
            SCI_FREE(buf_ptr);
        }        
        SCI_FREE(dynamic_item_ptr);
        switch (ret)
        {
        case MMIMENU_SYN_VALUE_SUC:
#ifdef MMI_GUI_STYLE_TYPICAL
            MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
#endif /* MMI_GUI_STYLE_TYPICAL */
            break;

        case MMIMENU_SYN_VALUE_NOSPACE:
            MMIPUB_OpenAlertWarningWin(TXT_NO_SPACE);
            break;
            
        case MMIMENU_SYN_VALUE_EXIST:
            MMIPUB_OpenAlertWarningWin(TXT_CMSBRW_LISTEXIT);
            break;
            
        case MMIMENU_SYN_VALUE_INVALID:
        case MMIMENU_SYN_VALUE_ERROR:
            MMIPUB_OpenAlertWarningWin(TXT_MMI_SAVEFAILED);
            break;
            
        default:
            break;
        }
        MMK_CloseWin(win_id);
    }
    break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        break;
    
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : query for exit the browser
//  Global resource dependence : 
//  Author:fen.xie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIBROWSER_QueryExit(void)
{
    MMI_WIN_ID_T    query_exit_win_id = MMIBROWSER_QUERY_EXIT_WIN_ID;
    MMIBROWSER_OpenQueryWinByTextId(TXT_EXIT_QUERY,
        IMAGE_PUBWIN_QUERY,
        &query_exit_win_id,
        HandleQueryExitBrowserWin);
}


/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif
