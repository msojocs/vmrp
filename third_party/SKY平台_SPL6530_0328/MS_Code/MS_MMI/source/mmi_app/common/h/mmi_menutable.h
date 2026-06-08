
#ifndef MENUTABLE_H
#define MENUTABLE_H

#include "mmi_common.h"
#include "guimenu.h"
#include "mmi_text.h"
#include "mmi_image.h"
#ifdef WRE_SUPPORT
#include "mmiwre.h"
#endif
#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif
#define TIP_NULL 0
/*-------------------------------------------------------------------------*/
/*                         TYPES/CONSTANTS                                 */
/*-------------------------------------------------------------------------*/
//菜单ID
typedef enum
{
    ID_MAINMENU_COMMUNICATION,
    ID_MAINMENU_SETTINGS,
    ID_MAINMENU_TOOLS,
    ID_MAINMENU_PASTIME,
	ID_MAINMENU_PLAYER,
    ID_COMMUNICATION_DIAL,
    ID_COMMUNICATION_MESSAGE,
    ID_COMMUNICATION_BROWSER,
    ID_COMMUNICATION_PB,

    // 滑动菜单
    ID_MAINMENU_SLIDE_PAGE1,
    ID_MAINMENU_SLIDE_PAGE2,
    ID_MAINMENU_SLIDE_PAGE3,
    ID_MAINMENU_SLIDE_PAGE4,
    ID_MAINMENU_SLIDE_PAGE5,
    ID_MAINMENU_SLIDE_PAGE6,
    ID_MAINMENU_SLIDE_PAGE7,
    ID_MAINMENU_SLIDE_PAGE8,
    ID_MAINMENU_SLIDE_PAGE9,

    ID_MAINMENU_SLIDE_TOOLS_OTHERS,
    ID_MAINMENU_SLIDE_SETTINGS,

#ifdef CMCC_UI_STYLE
   ID_MAINMENU_CMCC_MONERNET,
   ID_MAINMENU_CMCC_SERVICE,
   ID_MAINMENU_APPS,
   ID_BLUETOOTH,
       ID_EXPLORE,
       ID_CUSTMER_SERVICE,
//   ID_MAINMENU_MY_FAVORITE,
#endif 

    ID_MAINMENU_BT,
    ID_MAINMENU_CONNECT_MNG,
    ID_COMMUNICATION_CL,
    ID_COMMUNICATION_STK,
    ID_DOWNLOAD_MANAGER,
    ID_SETTINGS_ENVIONMENT,
    ID_SETTINGS_DISPLAY,
    ID_SETTINGS_PHONE,
    ID_SETTINGS_PHONE_SETTING,
    ID_SETTINGS_CALL,
    ID_SETTINGS_LINK,
    ID_SETTINGS_UPLMN,
    ID_SETTINGS_SECURITY,
    ID_TOOLS_ALARM,
    
#ifdef ASP_SUPPORT
  ID_TOOLS_ASP,
#endif
#ifdef DCD_SUPPORT
    ID_TOOLS_DCD,
#endif
    ID_TOOLS_CALCULATOR,
    ID_TOOLS_CALENDAR,
    ID_TOOLS_PAINT,
	ID_TOOLS_RECORD,
	ID_TOOLS_GPS,
	ID_TOOLS_OTHERS,
    ID_ENTERTAIMENT_PLAYER,
#ifdef  CAMERA_SUPPORT
    ID_ENTERTAIMENT_CAMERA,
    ID_ENTERTAIMENT_DV,
#ifdef MMIDC_F_U_CAMERA
    ID_ENTERTAIMENT_UCAMERA,
#endif
#endif    
#ifdef MMIUZONE_SUPPORT
    ID_ENTERTAIMENT_UZONE,
#endif
    ID_ENTERTAIMENT_FAVORITE,
	ID_ENTERTAIMENT_PIC,
    ID_ENTERTAIMENT_GAME,
    ID_ENTERTAIMENT_GOLDHOUSE,
	ID_ENTERTAIMENT_MP3,
	ID_ENTERTAIMENT_VIDEO,
    ID_ENTERTAIMENT_FM,
    ID_ENTERTAIMENT_ST,
    ID_ENTERTAIMENT_KUR,
    ID_ENTERTAIMENT_MTV,
    ID_ENTERTAIMENT_ATV,
    ID_ENTERTAIMENT_MOBILE_VIDEO,
	
    ID_MESSAGE,
    ID_DC,
    ID_PB,
    ID_ACCESSORY,
    ID_SETTING,
    ID_MULTIMEDIA,
    ID_BROWSER,
    ID_STK,
    ID_ENJOY_WORLD,
    ID_DV,  
    ID_PB_MAIN,
	ID_LENOVO_SPACE,
	ID_MYDOC,
	ID_PLAYER,
	ID_TOOLS,
    ID_GOLD_HOUSE,//@maggie add for ebook

	//MSD begin
	ID_MSD_SETUP,
	ID_MSD_CLOSE,
	ID_MSD_MODIFY_PSW,
	ID_MSD_MODIFY_INFO_TEL,

	//MSD END
	//idle phone number operation
    ID_DIAL_SEND_MESSAGE,
    ID_DIAL_SEND,
	ID_DIAL_SEND_SMS,
    ID_DIAL_SEND_MMS,
    ID_DAIL_CALL_LOG,
    ID_DIAL_ADD_PB_OPT,
    ID_DIAL_OPT,
	ID_DIAL_ADD_PB,
	ID_DIAL_ADD_EXISTNUM,//@zhaohui add
    ID_DIAL_INSERT_P_W, //insert p, w ,n

    //ID_SIM1_DIAL_CALL,
    //ID_SIM2_DIAL_CALL,
    ID_DIAL_CALL,
    ID_DIAL_IPCALL,
    ID_DIAL_CALL_AUDIO,
    ID_DIAL_CALL_VIDEO,

    ID_DIAL_INSERT_P,
    ID_DIAL_INSERT_W,
    ID_DIAL_INSERT_N,

	ID_MESSAGE_WRITE_MMS,
    ID_MESSAGE_SETTING_ID,

	ID_GAME,
    ID_PIC_VIEWER,
	ID_EBOOK,
#ifdef MMI_ENABLE_DCD
	ID_DCD_OPEN,
#endif 
	ID_JAVA_APP,
    ID_WORLDTIME,
	ID_PHONESEARCH,
	ID_COUNTDOWN,
	ID_STOPWATCH,
    ID_DYNAPP,
#ifdef SXH_APPLE_SUPPORT
	ID_SXH_APPLE,
	ID_SXH_TV,
	ID_SXH_TV1,
	ID_SXH_TV2,
	ID_SXH_TV3,
	ID_SXH_UMVCHAT,
	ID_SXH_RADIO,
	ID_SXH_UMVCHATTHIN,
	ID_SXH_UMVCHATDEMO,
	ID_SXH_Games,
	ID_SXH_UMMUSIC,
	ID_SXH_VIDEODL,
	ID_SXH_WEATHER,
    ID_SXH_BOOK,
    ID_SXH_UMPLAYER,
#endif
    ID_MAINMENU_PIM,
#ifdef SEARCH_SUPPORT
    ID_MAINMENU_SEARCH,
#endif
    ID_MAINMENU_OPT_ARRANGE,
    ID_MAINMENU_OPT_HELP,
    ID_MAINMENU_OPT_EXIT,
    ID_APPS_BT,//   NEWMS00173119
#ifdef CMCC_UI_STYLE
    ID_MAINMENU_CMCC,
    ID_MAINMENU_NET,
    ID_NET_FETION,
    ID_NET_MUSIC,
    ID_MAINMENU_APP,
    //ID_APPS_BT,// NEWMS00173119
    ID_APPS_MBBMS,
    //ID_CHINAMOBILE_DREAM,
    //ID_CHINAMOBILE_SERVICE,
    ID_CHINAMOBILE_SECURITIES,
    ID_CHINAMOBILE_MAP,
    ID_JAVA_STOCK,
    ID_FAVORITE_GOLDENHOUSE,
    ID_FAVORITE_RING,
    ID_FAVORITE_PIC,
    ID_FAVORITE_PHOTO,
    ID_FAVORITE_VIDEO,
    ID_SETTINGS_VERSION,
#endif
#ifdef QQ_SUPPORT
    ID_SPREADTRUM_QQ_SUPPORT,
#endif
	ID_TOOLS_UNIT_CONVERSION,
    // new
    ID_MAINMENU_HEALTH_MNG, // 健康管理
    ID_MAINMENU_MAGIC_VOICE, // 魔音
    ID_MAINMENU_FLASH, // Flash 播放

    // QQ	Ucweb	大智慧	大众点评
    ID_MAINMENU_QQ,
    ID_MAINMENU_UCWEB,
    ID_MAINMENU_GURAN,
    ID_MAINMENU_DIANPING,
    
    // 手机地图	开心农场	MSN	携程
    ID_MAINMENU_PHONE_MAP,
    ID_MAINMENU_KAIXIN,
    ID_MAINMENU_MSN,
    ID_MAINMENU_CTRIP,

    // 百度	新浪微博	淘宝网	优酷
    ID_MAINMENU_BAIDU,
    //ID_MAINMENU_SINA_BOLG,
    ID_MAINMENU_TAOBAO,
    ID_MAINMENU_YOUKU,

    // 人人网	起点中文
    ID_MAINMENU_RENREN,
    ID_MAINMENU_QIDIAN,

#ifdef SNS_SUPPORT
   //FaceBook  Twitter  SinaBlog
   ID_MAINMENU_FACEBOOK,
   ID_MAINMENU_TWITTER,
   ID_MAINMENU_SINA_BOLG,
#endif

#ifdef WRE_SUPPORT
    ID_TOOLS_WRE,//移动娱乐商城
#ifdef WRE_CONTRA_SUPPORT  // 魂斗罗
    ID_WRE_CONTRA,
#endif
#ifdef WRE_SUPERMARIO_SUPPORT  // 超级玛丽
    ID_WRE_SUPERMARIO,
#endif
#ifdef WRE_TSLAND_SUPPORT  // 冒险岛
    ID_WRE_TSLAND,
#endif
#endif 
    ID_MAINMENU_SINA,
    ID_MAINMENU_SOSO,
   
#ifdef PUSH_EMAIL_SUPPORT 
    ID_MAINMENU_PUSHMAIL,
#endif /* PUSH_EMAIL_SUPPORT  */
#ifdef MMI_AZAN_SUPPORT
	ID_AZAN,
#endif
#ifdef MMI_FM_TIMER_RECORD_SUPPORT
    ID_ENTERTAIMENT_FM_TIMER_RECORD,
#endif
#ifdef MCARE_V31_SUPPORT
	ID_BRW_APP_SUPPORT,
	ID_TMC_APP_SUPPORT,
#endif

#ifdef WEATHER_SUPPORT 
    //weather
    ID_MAINMENU_WEATHER,
#endif /* WEATHER_SUPPORT  */
#ifdef MMIEMAIL_SUPPORT
    ID_COMMUNICATION_EMAIL,    // end new
#endif

#ifdef MMI_PDA_SUPPORT
    ID_MAINMENU_IDLE_STYLE,
#endif
    ID_MAINMENU_STYLE_SET,

    ID_MAINMENU_LOCK_SET,
	
#ifdef MMI_KING_MOVIE_SUPPORT
	ID_MENU_KING_MOVIE,
#endif

    ID_APP_MANAGER,
    ID_APP_MEMO,

	//new xiyuan
    ID_MAINMENU_PALMREAD,      
	ID_MAINMENU_AMUSEBOX,           
	ID_MAINMENU_MOBILESOHU,      
	ID_MAINMENU_COOLSTATION,     
	ID_MAINMENU_COLORWORLD,    
	ID_MAINMENU_ZHIFUBAO,      
	ID_MAINMENU_TUDOU,          
#ifdef MRAPP_SUPPORT
#ifdef MRAPP_COOKIE_OPEN
    ID_MRAPP_COOKIE,
#endif
#ifdef MRAPP_MRPSTORE_OPEN
    ID_MRAPP_MRPSTORE,
#endif
#ifdef MRAPP_SKYBUDDY_OPEN
    ID_MRAPP_SKYBUDDY,
#endif
#ifdef MRAPP_NETGAME_OPEN
    ID_MRAPP_NETGAME,
#endif
#ifdef MRAPP_CLOCK_OPEN
    ID_MRAPP_CLOCK,
#endif
#ifdef MRAPP_TIANQI_OPEN
    ID_MRAPP_TIANQI,
#endif
#ifdef MRAPP_FETION_OPEN
    ID_MRAPP_FETION,
#endif
#ifdef MRAPP_XLWB_OPEN
    ID_MRAPP_XLWB,
#endif
#ifdef MRAPP_TALKCAT_OPEN
    ID_MRAPP_TALKCAT,
#endif
#ifdef MRAPP_SKIRT_OPEN
    ID_MRAPP_SKIRT,
#endif
#ifdef MRAPP_BIRD_OPEN
    ID_MRAPP_BIRD,
#endif
#ifdef MRAPP_FRUIT_OPEN
	ID_MRAPP_FRUIT,
#endif
#ifdef MRAPP_BYDR_OPEN
	ID_MRAPP_BYDR,
#endif

#ifdef MRAPP_KAIXIN_OPEN
    ID_MRAPP_KAIXIN,
#endif
#if defined(MRAPP_SDKQQ_OPEN) || defined(__MMI_SKYQQ__)
    ID_MRAPP_QQ,
#endif

#ifdef MRAPP_FACEBOOK_OPEN
    ID_MRAPP_FACEBOOK,
#endif
#ifdef MRAPP_YAHOO_OPEN
    ID_MRAPP_YAHOO,
#endif
#ifdef MRAPP_TWITTER_OPEN
    ID_MRAPP_TWITTER,
#endif
#endif

#ifdef QBTHEME_SUPPORT
    ID_MAINMENU_RETURNIDLE,
    ID_MAINMENU_QBTHEMEAPP,
    ID_MAINMENU_QBTHEMEMORE,

    ID_MAINMENU_IDLE_STYLE_GRID,
    ID_MAINMENU_IDLE_ISTYLE,
    ID_MAINMENU_IDLE_QBTHEME,
#endif
    // end new

    //extract
    ID_EXTRACT_CALL_ITEM_ID,
    ID_EXTRACT_SEND_SMS_ITEM_ID,
    ID_EXTRACT_ADDTOPB_ITEM_ID,
    ID_EXTRACT_URL_BROWSE_ITEM_ID,
    ID_EXTRACT_URL_ADDTOBOOKMARK_ITEM_ID,
    ID_EXTRACT_SEND_MMS_ITEM_ID,
    ID_EXTRACT_SEND_EMAIL_ITEM_ID,
    ID_EXTRACT_DIAL_ITEM_ID,
    ID_EXTRACT_VIDEOCALL_ITEM_ID,
    ID_EXTRACT_IPDIAL_ITEM_ID,
    ID_EXTRACT_ANSWER_SMS_ITEM_ID,
    ID_EXTRACT_ANSWER_MMS_ITEM_ID,
    ID_EXTRACT_ANSWER_MAIL_ITEM_ID,
    ID_EXTRACT_ADDTOPB_NEW_ITEM_ID,
    ID_EXTRACT_ADDTOPB_EXIST_ITEM_ID,

#ifdef OPERA_MINI_SUPPORT
    ID_MAINMENU_OPERAMINI,
#endif    
#ifdef BROWSER_SUPPORT_NETFRONT
    ID_MAINMENU_NF,
#endif    
#ifdef BROWSER_SUPPORT_DORADO    
    ID_MAINMENU_DORADO,
#endif
#ifdef IKEYBACKUP_SUPPORT
    ID_MAINMENU_IKEYBACKUP,
#endif
	ID_CLOCK,
//============== MET MEX start ===================
#ifdef MET_MEX_MERIG_SUPPORT
    ID_MAINMENU_MET_MEX_ITEM_ID,
#endif // MET_MEX_MERIG_SUPPORT

#ifdef MET_MEX_QQ_SUPPORT
    ID_MAINMENU_MET_MEX_QQ_ITEM_ID,
#endif// MET_MEX_QQ_SUPPORT

#ifdef MET_MEX_TB_SUPPORT
    ID_MAINMENU_MET_MEX_TB_ITEM_ID,
#endif// MET_MEX_TB_SUPPORT

#ifdef MET_MEX_AQ_SUPPORT
    ID_MAINMENU_MET_MEX_AQ_ITEM_ID,
#endif// MET_MEX_AQ_SUPPORT

#ifdef MET_MEX_ZT_SUPPORT
    ID_MAINMENU_MET_MEX_ZT_ITEM_ID,
#endif// MET_MEX_ZT_SUPPORT
#ifdef MET_MEX_FRUITNINJA_SUPPORT
    ID_MAINMENU_MET_MEX_FRUITNINJA_ITEM_ID,
#endif// MET_MEX_FRUITNINJA_SUPPORT
#ifdef MET_MEX_WEIBO_SUPPORT
    ID_MAINMENU_MET_MEX_WEIBO_ITEM_ID,
#endif
#ifdef MET_MEX_WEATHER_SUPPORT
    ID_MAINMENU_MET_MEX_WEATHER_ITEM_ID,
#endif
#ifdef MET_MEX_WOMIVIDEO_SUPPORT
    ID_MAINMENU_MET_MEX_WOMIVIDEO_ITEM_ID,
#endif
#ifdef MET_MEX_DOUBLE_QQ_SUPPORT 
    ID_MAINMENU_MET_MEX_QQ1_ITEM_ID,
    ID_MAINMENU_MET_MEX_QQ2_ITEM_ID,
#endif
#ifdef MET_MEX_MSN_SUPPORT
    ID_MAINMENU_MET_MEX_MSN_ITEM_ID,
#endif
#ifdef MET_MEX_FETION_SUPPORT
    ID_MAINMENU_MET_MEX_FETION_ITEM_ID,
#endif
#ifdef MET_MEX_AB_SUPPORT
    ID_MAINMENU_MET_MEX_AB_ITEM_ID,
#endif
#ifdef MET_MEX_CQZ_SUPPORT
    ID_MAINMENU_MET_MEX_CQZ_ITEM_ID,
#endif
#ifdef MET_MEX_JSCB_SUPPORT
    ID_MAINMENU_MET_MEX_JSCB_ITEM_ID,
#endif
#ifdef MET_MEX_TOMCAT_SUPPORT
    ID_MAINMENU_MET_MEX_TOMCAT_ITEM_ID,
#endif
//============== MET MEX end ===================
#ifdef GO_MENU_ONE_ICON_CHANGE_STYLE
    ID_GOMENU_CHANGE_STYLE,
#endif
	ID_TOTAL
}MMI_MENU_ID_E;

#define MENU_DEF(_TABLEID, _ITEMPTR, _STYLE, _TITLESTR, _TITLE_ICON, _TITLE_NUMICON, _TITLE_BACKGROUND, \
	_ITEMCOUNT) \
     _TABLEID,

typedef enum
{
	MENU_NULL,
	MENU_DYNA,
	#include "mmi_menutable.def"
	MENUTABLE_MAX
} MenuLabelE;

#undef MENU_DEF

/*-------------------------------------------------------------------------*/
/*                         FUNCTIONS                                       */
/*-------------------------------------------------------------------------*/
extern const GUIMENU_GROUP_T* MMI_GetMenuTable(void);

/*-------------------------------------------------------------------------*/
/*                         VARIABLES                                       */
/*-------------------------------------------------------------------------*/
extern const GUIMENU_GROUP_T menu_table[];

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif
/*-------------------------------------------------------------------------*/

#endif /* MENUTABLE_H */
