
#include "MMI_features.h"
#include "CustResDef.h"
#include "mrp_features.h"

#define DSM_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM"                       

#ifdef __MMI_MAINLCD_240X320__
#define DSM_WEATHER_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\Weather\\\\240"   
#elif defined(__MMI_MAINLCD_176X220__)
#define DSM_WEATHER_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\Weather\\\\176" 
#else
#define DSM_WEATHER_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\Weather\\\\240" 
#endif
//默认情况下这里都设置成了小图标，大图标会占用比较大的ROM空间，请根据实际情况使用
#ifdef __MR_CFG_FEATURE_SLIM__  
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\16"
#else
#ifndef __COSMOS_MMI__
#if (defined(__MMI_MAINLCD_128X160__)||defined(__MMI_MAINLCD_176X220__))
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\16"
#else
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\24"
#endif
#else
#ifndef __MMI_MAINLCD_480X800__
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\48"
#else
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\72"
#endif
#endif
#endif

<?xml version="1.0" encoding="UTF-8"?>
<APP id="VAPP_MYTHROAD" name="STR_APP_MYTHROAD" type="venus">

    <MEMORY heap="1024*300" cui="max(VCUI_FMGR, VCUI_DTCNT)" />
    		
    <!----- Include Area ------------------------------------------------------>
    <INCLUDE file="GlobalResDef.h"/>

    <!----- String Resource Area ---------------------------------------------->
#ifdef __SKY_SECURITY_GUARD__    
    <STRING id="STR_ID_SKY_SG_MCI_INDICATION"/>
    <STRING id="STR_ID_SKY_SG"/>
    <STRING id="STR_ID_SKY_SG_ASR_PREFIX"/>
    <STRING id="STR_ID_SKY_SG_ASR_MESSAGE_1"/>
#endif    
    <STRING id="STR_DSM_EXITBACKAPP"/>
    <STRING id="STR_DSM_WAITING"/>
    <STRING id="STR_DSM_PLZ_STOP_BT"/>
    
    <STRING id="STR_DSM_GAME_CENTER"/>
    <STRING id="STR_DSM_DSMMAIN"/>
    <STRING id="STR_DSM_DSMGAME"/>
    <STRING id="STR_DSM_MRPSTORE"/>
    <STRING id="STR_DSM_SETTING"/>
    <STRING id="STR_DSM_STOCK"/>
    <STRING id="STR_DSM_BOOK"/>         
    <STRING id="STR_DSM_DSM_MAIN"/>
    <STRING id="STR_DSM_QQGAME"/>
    <STRING id="STR_DSM_ONLINE_GAME"/>
    <STRING id="STR_DSM_CALLMASTER"/>
    <STRING id="STR_DSM_NCLOCK"/>
    <STRING id="STR_DSM_KAIXIN"/>
    <STRING id="STR_DSM_TIANQI"/>
    <STRING id="STR_DSM_FETION"/>
    <STRING id="STR_DSM_MSN"/>         
    <STRING id="STR_DSM_QQ"/>
    <STRING id="STR_DSM_YAHOO"/>
    <STRING id="STR_DSM_SKYPE"/>
    <STRING id="STR_DSM_FACEBOOK"/>
    <STRING id="STR_DSM_EDICT"/>
    <STRING id="STR_DSM_CDICT"/>
    <STRING id="STR_DSM_MPLAYER"/>
    <STRING id="STR_DSM_THEMER"/> 
    <STRING id="STR_DSM_NES"/>   
    <STRING id="STR_DSM_HXSG"/>  
    <STRING id="STR_DSM_NETGAME"/> 
    <STRING id="STR_DSM_TWITTER"/>
    <STRING id="STR_ID_SPLAYER_MORE"/>
    <STRING id="STR_ID_SPLAYER_MORE2"/>
    <STRING id="STR_ID_SPLAYER_MORE_KEY"/>
    <STRING id="STR_DSM_MPLIVE"/>
    <STRING id="STR_DSM_MPCHAT"/>

    <STRING id="STR_DSM_NEWMSG"/>
    <STRING id="STR_DSM_TOMCAT"/>
    <STRING id="STR_DSM_FRUIT"/>
    <STRING id="STR_DSM_JS"/>
    <STRING id="STR_DSM_BIRD"/>
    <STRING id="STR_DSM_SKIRT"/>
    <STRING id="STR_DSM_SINAWEIBO"/>
    <STRING id="STR_DSM_QQBROWSER"/>
    <STRING id="STR_DSM_SKYBUDDY"/>
    <STRING id="STR_DSM_BYDR"/>
    <STRING id="STR_DSM_SHOUXIN"/>

    <!----- Image Resource Area ----------------------------------------------->
    <IMAGE id="IMG_DSM_STATUS_ICON_ONLINE" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\online.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_AWAY" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\away.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_HIDE" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\hide.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_OFFLINE" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\offline.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_QQ_ONLINE" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\qqonline.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_QQ_AWAY" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\qqaway.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_QQ_HIDE" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\qqhide.png"</IMAGE>
    <IMAGE id="IMG_DSM_STATUS_ICON_QQ_OFFLINE" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\qqoffline.png"</IMAGE>

#ifdef __SKY_SECURITY_GUARD__   
    <IMAGE id="IMG_ID_SKY_SG_STATUS_ICON" force_type = "FORCE_ABM">DSM_RES_PATH"\\\\status_icon\\\\sg_icon.png"</IMAGE>
    <IMAGE id="IMG_DSM_LIST_GUARD">DSM_LIST_RES_ICONS_PATH"\\\\guard.png"</IMAGE>
#endif
    
  <!--图片资源会占用比较多的ROM空间，在52上ROM空间紧张的情况下请不要添加多余的图标-->

    <IMAGE id="IMG_DSM_LIST_GAME">DSM_LIST_RES_ICONS_PATH"\\\\mpzone.png"</IMAGE>

#ifdef __MR_CFG_ENTRY_MSN__    
    <IMAGE id="IMG_DSM_LIST_MSN">DSM_LIST_RES_ICONS_PATH"\\\\msn.png"</IMAGE>
#endif    
#ifdef __MR_CFG_ENTRY_QQ__
    <IMAGE id="IMG_DSM_LIST_QQ">DSM_LIST_RES_ICONS_PATH"\\\\qq.png"</IMAGE>
#endif
#ifdef __MR_CFG_ENTRY_FETION__    
    <IMAGE id="IMG_DSM_LIST_FETION">DSM_LIST_RES_ICONS_PATH"\\\\fetion.png"</IMAGE>
#endif
#ifdef __MR_CFG_ENTRY_EBOOK__
    <IMAGE id="IMG_DSM_LIST_BOOK">DSM_LIST_RES_ICONS_PATH"\\\\book.png"</IMAGE>     
#endif    
#ifdef __MR_CFG_ENTRY_NETGAME__
    <IMAGE id="IMG_DSM_LIST_MPZONE">DSM_LIST_RES_ICONS_PATH"\\\\mpzone.png"</IMAGE>  
#endif

#ifdef __MR_CFG_ENTRY_MPLIVE__
    <IMAGE id="IMG_DSM_LIST_MPLIVE">DSM_LIST_RES_ICONS_PATH"\\\\mptv.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_MPCHAT__
    <IMAGE id="IMG_DSM_LIST_MPCHAT">DSM_LIST_RES_ICONS_PATH"\\\\mpchat.png"</IMAGE>
#endif
 
#ifdef __MR_CFG_ENTRY_TOMCAT__
    <IMAGE id="IMG_DSM_LIST_TOMCAT">DSM_LIST_RES_ICONS_PATH"\\\\talkcat.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_FRUIT__
    <IMAGE id="IMG_DSM_LIST_FRUIT">DSM_LIST_RES_ICONS_PATH"\\\\fruit.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_JS__
    <IMAGE id="IMG_DSM_LIST_JS">DSM_LIST_RES_ICONS_PATH"\\\\js.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
    <IMAGE id="IMG_DSM_LIST_ANGRYBIRD">DSM_LIST_RES_ICONS_PATH"\\\\bird.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_SKIRT__
    <IMAGE id="IMG_DSM_LIST_SKIRT">DSM_LIST_RES_ICONS_PATH"\\\\skirt.png"</IMAGE>
#endif

#ifdef __SKY_SECURITY_GUARD__
    <IMAGE id="IMG_DSM_LIST_GUARD">DSM_LIST_RES_ICONS_PATH"\\\\guard.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_KAIXIN__
    <IMAGE id="IMG_DSM_LIST_KAIXIN">DSM_LIST_RES_ICONS_PATH"\\\\kaixin.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_TIANQI__
    <IMAGE id="IMG_DSM_LIST_TIANQI">DSM_LIST_RES_ICONS_PATH"\\\\weather.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_SINAWEIBO__
    <IMAGE id="IMG_DSM_LIST_SINAWEIBO">DSM_LIST_RES_ICONS_PATH"\\\\weibo.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_NETCLOCK__
    <IMAGE id="IMG_DSM_LIST_NETCLOCK">DSM_LIST_RES_ICONS_PATH"\\\\netclock.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_BYDR__  
    <IMAGE id="IMG_DSM_LIST_BYDR">DSM_LIST_RES_ICONS_PATH"\\\\bydr.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_SHOUXIN__  
    <IMAGE id="IMG_DSM_LIST_SHOUXIN">DSM_LIST_RES_ICONS_PATH"\\\\shouxin.png"</IMAGE>
#endif

#ifdef __MR_CFG_ENTRY_SKYBUDDY__  
    <IMAGE id="IMG_DSM_LIST_SKYBUDDY">DSM_LIST_RES_ICONS_PATH"\\\\skybuddy.png"</IMAGE>
#endif

    
#ifndef __MR_CFG_FEATURE_SLIM__    
    <IMAGE id="IMG_DSM_QIPAO_A">DSM_RES_PATH"\\\\qipao_icon\\\\A.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_B">DSM_RES_PATH"\\\\qipao_icon\\\\B.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_C">DSM_RES_PATH"\\\\qipao_icon\\\\C.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_D">DSM_RES_PATH"\\\\qipao_icon\\\\D.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_E">DSM_RES_PATH"\\\\qipao_icon\\\\E.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_F">DSM_RES_PATH"\\\\qipao_icon\\\\F.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_G">DSM_RES_PATH"\\\\qipao_icon\\\\G.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_H">DSM_RES_PATH"\\\\qipao_icon\\\\H.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_I">DSM_RES_PATH"\\\\qipao_icon\\\\I.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_J">DSM_RES_PATH"\\\\qipao_icon\\\\J.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_K">DSM_RES_PATH"\\\\qipao_icon\\\\K.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_L">DSM_RES_PATH"\\\\qipao_icon\\\\L.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_M1">DSM_RES_PATH"\\\\qipao_icon\\\\M1.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_M2">DSM_RES_PATH"\\\\qipao_icon\\\\M2.png"</IMAGE>
    <IMAGE id="IMG_DSM_QIPAO_M3">DSM_RES_PATH"\\\\qipao_icon\\\\M3.png"</IMAGE>
#endif
    <!----- Screen Resource Area ---------------------------------------------->
    <SCREEN id="GRP_ID_MYTHROAD"/>
    <SCREEN id="DSM_SCREEN_BASE"/>
    <SCREEN id="SCR_DSM_DUMMY_001"/>
    <SCREEN id="SCR_DSM_DUMMY_002"/>
    <SCREEN id="SCR_DSM_DUMMY_003"/>
    <SCREEN id="SCR_DSM_DUMMY_004"/>
    <SCREEN id="SCR_DSM_DUMMY_005"/>
    <SCREEN id="SCR_DSM_DUMMY_006"/>
    <SCREEN id="SCR_DSM_DUMMY_007"/>
    <SCREEN id="SCR_DSM_DUMMY_008"/>
    <SCREEN id="SCR_DSM_DUMMY_009"/>
    <SCREEN id="SCR_DSM_DUMMY_010"/>
    <SCREEN id="SCR_DSM_DUMMY_011"/>
    <SCREEN id="SCR_DSM_DUMMY_012"/>
    <SCREEN id="SCR_DSM_DUMMY_013"/>
    <SCREEN id="SCR_DSM_DUMMY_014"/>
    <SCREEN id="SCR_DSM_DUMMY_015"/>
    <SCREEN id="SCR_DSM_DUMMY_016"/>
    <SCREEN id="SCR_DSM_DUMMY_017"/>
    <SCREEN id="SCR_DSM_DUMMY_018"/>
    <SCREEN id="SCR_DSM_DUMMY_019"/>
    <SCREEN id="SCR_DSM_DUMMY_020"/>
    <SCREEN id="SCR_DSM_DUMMY_021"/>
    <SCREEN id="SCR_DSM_DUMMY_022"/>
    <SCREEN id="SCR_DSM_DUMMY_023"/>
    <SCREEN id="SCR_DSM_DUMMY_024"/>
    <SCREEN id="SCR_DSM_DUMMY_025"/>
    <SCREEN id="SCR_DSM_DUMMY_026"/>
    <SCREEN id="SCR_DSM_DUMMY_027"/>
    <SCREEN id="SCR_DSM_DUMMY_028"/>
    <SCREEN id="SCR_DSM_DUMMY_029"/>
    <SCREEN id="SCR_DSM_DUMMY_030"/>
    <SCREEN id="SCR_DSM_DUMMY_031"/>
    <SCREEN id="SCR_DSM_DUMMY_032"/>
    <SCREEN id="SCR_DSM_DUMMY_033"/>
    <SCREEN id="SCR_DSM_DUMMY_034"/>
    <SCREEN id="SCR_DSM_DUMMY_035"/>
    <SCREEN id="SCR_DSM_DUMMY_036"/>
    <SCREEN id="SCR_DSM_DUMMY_037"/>
    <SCREEN id="SCR_DSM_DUMMY_038"/>
    <SCREEN id="SCR_DSM_DUMMY_039"/>
    <SCREEN id="SCR_DSM_DUMMY_040"/>
    <SCREEN id="SCR_DSM_DUMMY_041"/>
    <SCREEN id="SCR_DSM_DUMMY_042"/>
    <SCREEN id="SCR_DSM_DUMMY_043"/>
    <SCREEN id="SCR_DSM_DUMMY_044"/>
    <SCREEN id="SCR_DSM_DUMMY_045"/>
    <SCREEN id="SCR_DSM_DUMMY_046"/>
    <SCREEN id="SCR_DSM_DUMMY_047"/>
    <SCREEN id="SCR_DSM_DUMMY_048"/>
    <SCREEN id="SCR_DSM_DUMMY_049"/>
    <SCREEN id="SCR_DSM_DUMMY_050"/>
    <SCREEN id="SCR_DSM_DUMMY_051"/>
    <SCREEN id="SCR_DSM_DUMMY_052"/>
    <SCREEN id="SCR_DSM_DUMMY_053"/>
    <SCREEN id="SCR_DSM_DUMMY_054"/>
    <SCREEN id="SCR_DSM_DUMMY_055"/>
    <SCREEN id="SCR_DSM_DUMMY_056"/>
    <SCREEN id="SCR_DSM_DUMMY_057"/>
    <SCREEN id="SCR_DSM_DUMMY_058"/>
    <SCREEN id="SCR_DSM_DUMMY_059"/>
    <SCREEN id="SCR_DSM_DUMMY_060"/>
    <SCREEN id="SCR_DSM_DUMMY_061"/>
    <SCREEN id="SCR_DSM_DUMMY_062"/>
    <SCREEN id="SCR_DSM_DUMMY_063"/>
    <SCREEN id="SCR_DSM_DUMMY_064"/>
    <SCREEN id="SCR_DSM_DUMMY_065"/>
    <SCREEN id="SCR_DSM_DUMMY_066"/>
    <SCREEN id="SCR_DSM_DUMMY_067"/>
    <SCREEN id="SCR_DSM_DUMMY_068"/>
    <SCREEN id="SCR_DSM_DUMMY_069"/>
    <SCREEN id="SCR_DSM_DUMMY_070"/>
    <SCREEN id="SCR_DSM_DUMMY_071"/>
    <SCREEN id="SCR_DSM_DUMMY_072"/>
    <SCREEN id="SCR_DSM_DUMMY_073"/>
    <SCREEN id="SCR_DSM_DUMMY_074"/>
    <SCREEN id="SCR_DSM_DUMMY_075"/>
    <SCREEN id="SCR_DSM_DUMMY_076"/>
    <SCREEN id="SCR_DSM_DUMMY_077"/>
    <SCREEN id="SCR_DSM_DUMMY_078"/>
    <SCREEN id="SCR_DSM_DUMMY_079"/>
    <SCREEN id="SCR_DSM_DUMMY_080"/>
    <SCREEN id="SCR_DSM_DUMMY_081"/>
    <SCREEN id="SCR_DSM_DUMMY_082"/>
    <SCREEN id="SCR_DSM_DUMMY_083"/>
    <SCREEN id="SCR_DSM_DUMMY_084"/>
    <SCREEN id="SCR_DSM_DUMMY_085"/>
    <SCREEN id="SCR_DSM_DUMMY_086"/>
    <SCREEN id="SCR_DSM_DUMMY_087"/>
    <SCREEN id="SCR_DSM_DUMMY_088"/>
    <SCREEN id="SCR_DSM_DUMMY_089"/>
    <SCREEN id="SCR_DSM_DUMMY_090"/>
    <SCREEN id="SCR_DSM_DUMMY_091"/>
    <SCREEN id="SCR_DSM_DUMMY_092"/>
    <SCREEN id="SCR_DSM_DUMMY_093"/>
    <SCREEN id="SCR_DSM_DUMMY_094"/>
    <SCREEN id="SCR_DSM_DUMMY_095"/>
    <SCREEN id="SCR_DSM_DUMMY_096"/>
    <SCREEN id="SCR_DSM_DUMMY_097"/>
    <SCREEN id="SCR_DSM_DUMMY_098"/>
    <SCREEN id="SCR_DSM_DUMMY_099"/>
    <SCREEN id="SCR_DSM_DUMMY_100"/>
    <SCREEN id="SCR_DSM_DUMMY_101"/>
    <SCREEN id="SCR_DSM_DUMMY_102"/>
    <SCREEN id="SCR_DSM_DUMMY_103"/>
    <SCREEN id="SCR_DSM_DUMMY_104"/>
    <SCREEN id="SCR_DSM_DUMMY_105"/>
    <SCREEN id="SCR_DSM_DUMMY_106"/>
    <SCREEN id="SCR_DSM_DUMMY_107"/>
    <SCREEN id="SCR_DSM_DUMMY_108"/>
    <SCREEN id="SCR_DSM_DUMMY_109"/>
    <SCREEN id="SCR_DSM_DUMMY_110"/>
    <SCREEN id="SCR_DSM_DUMMY_111"/>
    <SCREEN id="SCR_DSM_DUMMY_112"/>
    <SCREEN id="SCR_DSM_DUMMY_113"/>
    <SCREEN id="SCR_DSM_DUMMY_114"/>
    <SCREEN id="SCR_DSM_DUMMY_115"/>
    <SCREEN id="SCR_DSM_DUMMY_116"/>
    <SCREEN id="SCR_DSM_DUMMY_117"/>
    <SCREEN id="SCR_DSM_DUMMY_118"/>
    <SCREEN id="SCR_DSM_DUMMY_119"/>
    <SCREEN id="SCR_DSM_DUMMY_120"/>
    <SCREEN id="SCR_DSM_DUMMY_121"/>
    <SCREEN id="SCR_DSM_DUMMY_122"/>
    <SCREEN id="SCR_DSM_DUMMY_123"/>
    <SCREEN id="SCR_DSM_DUMMY_124"/>
    <SCREEN id="SCR_DSM_DUMMY_125"/>
    <SCREEN id="SCR_DSM_DUMMY_126"/>
    <SCREEN id="SCR_DSM_DUMMY_127"/>
    <SCREEN id="SCR_DSM_DUMMY_128"/>
    <SCREEN id="SCR_DSM_DUMMY_129"/>
    <SCREEN id="SCR_DSM_DUMMY_130"/>
    <SCREEN id="SCR_DSM_DUMMY_131"/>
    <SCREEN id="SCR_DSM_DUMMY_132"/>
    <SCREEN id="SCR_DSM_DUMMY_133"/>
    <SCREEN id="SCR_DSM_DUMMY_134"/>
    <SCREEN id="SCR_DSM_DUMMY_135"/>
    <SCREEN id="SCR_DSM_DUMMY_136"/>
    <SCREEN id="SCR_DSM_DUMMY_137"/>
    <SCREEN id="SCR_DSM_DUMMY_138"/>
    <SCREEN id="SCR_DSM_DUMMY_139"/>
    <SCREEN id="SCR_DSM_DUMMY_140"/>
    <SCREEN id="SCR_DSM_DUMMY_141"/>
    <SCREEN id="SCR_DSM_DUMMY_142"/>
    <SCREEN id="SCR_DSM_DUMMY_143"/>
    <SCREEN id="SCR_DSM_DUMMY_144"/>
    <SCREEN id="SCR_DSM_DUMMY_145"/>
    <SCREEN id="SCR_DSM_DUMMY_146"/>
    <SCREEN id="SCR_DSM_DUMMY_147"/>
    <SCREEN id="SCR_DSM_DUMMY_148"/>
    <SCREEN id="SCR_DSM_DUMMY_149"/>
    <SCREEN id="SCR_DSM_DUMMY_150"/>
    <SCREEN id="SCR_DSM_DUMMY_151"/>
    <SCREEN id="SCR_DSM_DUMMY_152"/>
    <SCREEN id="SCR_DSM_DUMMY_153"/>
    <SCREEN id="SCR_DSM_DUMMY_154"/>
    <SCREEN id="SCR_DSM_DUMMY_155"/>
    <SCREEN id="SCR_DSM_DUMMY_156"/>
    <SCREEN id="SCR_DSM_DUMMY_157"/>
    <SCREEN id="SCR_DSM_DUMMY_158"/>
    <SCREEN id="SCR_DSM_DUMMY_159"/>
    <SCREEN id="SCR_DSM_DUMMY_160"/>
    <SCREEN id="SCR_DSM_DUMMY_161"/>
    <SCREEN id="SCR_DSM_DUMMY_162"/>
    <SCREEN id="SCR_DSM_DUMMY_163"/>
    <SCREEN id="SCR_DSM_DUMMY_164"/>
    <SCREEN id="SCR_DSM_DUMMY_165"/>
    <SCREEN id="SCR_DSM_DUMMY_166"/>
    <SCREEN id="SCR_DSM_DUMMY_167"/>
    <SCREEN id="SCR_DSM_DUMMY_168"/>
    <SCREEN id="SCR_DSM_DUMMY_169"/>
    <SCREEN id="SCR_DSM_DUMMY_170"/>
    <SCREEN id="SCR_DSM_DUMMY_171"/>
    <SCREEN id="SCR_DSM_DUMMY_172"/>
    <SCREEN id="SCR_DSM_DUMMY_173"/>
    <SCREEN id="SCR_DSM_DUMMY_174"/>
    <SCREEN id="SCR_DSM_DUMMY_175"/>
    <SCREEN id="SCR_DSM_DUMMY_176"/>
    <SCREEN id="SCR_DSM_DUMMY_177"/>
    <SCREEN id="SCR_DSM_DUMMY_178"/>
    <SCREEN id="SCR_DSM_DUMMY_179"/>
    <SCREEN id="SCR_DSM_DUMMY_180"/>
    <SCREEN id="SCR_DSM_DUMMY_181"/>
    <SCREEN id="SCR_DSM_DUMMY_182"/>
    <SCREEN id="SCR_DSM_DUMMY_183"/>
    <SCREEN id="SCR_DSM_DUMMY_184"/>
    <SCREEN id="SCR_DSM_DUMMY_185"/>
    <SCREEN id="SCR_DSM_DUMMY_186"/>
    <SCREEN id="SCR_DSM_DUMMY_187"/>
    <SCREEN id="SCR_DSM_DUMMY_188"/>
    <SCREEN id="SCR_DSM_DUMMY_189"/>
    <SCREEN id="SCR_DSM_DUMMY_190"/>
    <SCREEN id="SCR_DSM_DUMMY_191"/>
    <SCREEN id="SCR_DSM_DUMMY_192"/>
    <SCREEN id="SCR_DSM_DUMMY_193"/>
    <SCREEN id="SCR_DSM_DUMMY_194"/>
    <SCREEN id="SCR_DSM_DUMMY_195"/>
    <SCREEN id="SCR_DSM_DUMMY_196"/>
    <SCREEN id="SCR_DSM_DUMMY_197"/>
    <SCREEN id="SCR_DSM_DUMMY_198"/>
    <SCREEN id="SCR_DSM_DUMMY_199"/>
    <SCREEN id="SCR_DSM_SCREEN_MENU"/>
    <SCREEN id="MENU_DSM_FMGR_APP"/>
    <SCREEN id="SCR_DSM_MAINMENU_ID"/>
    <SCREEN id="SCR_ID_DSM_APP_FILE"/>
    <SCREEN id="SCR_KEYLOCK_LOCK"/>
    <SCREEN id="SCR_TEST_SCREEN_ID"/>
    <SCREEN id="SCR_DL_APPLIST"/>
#ifdef __MMI_BMP__    
    <SCREEN id="SCR_BMP_STYLE_SETTING_ID"/>
    <SCREEN id="SCR_BMP_MAINMENU_SETTING_ID"/>
    <SCREEN id="SCR_BMP_LIST_SETTING_ID"/>
    <SCREEN id="SCR_BMP_SCREEN_SETTING_ID"/>
#endif    
    <SCREEN id="SCR_GPRS_ACCOUNT_LIST"/>
#ifdef __MMI_SKY_FONT__    
    <SCREEN id="SCR_SKY_FONT_SETTING"/>
    <SCREEN id="SCR_SKY_FONT_SELECT"/>
    <SCREEN id="SCR_SKY_FONT_SIZE"/>
#endif    
    <SCREEN id="SCR_SENDING_SMS"/>        

#ifdef __SKY_SECURITY_GUARD__    
    <SCREEN id="SCR_ID_SKY_SG_REPLY"/>     
#endif

    <!----- Menu Resource Area ------------------------------------------------>


    <MENUITEM id="MENU_DSM_GAME_CENTER" str="STR_DSM_GAME_CENTER"  img="IMG_DSM_LIST_GAME"/>
  
#ifdef __MR_CFG_ENTRY_NETGAME__
    <MENUITEM id="MENU_DSM_NETGAME" str="STR_DSM_NETGAME"  img="IMG_DSM_LIST_MPZONE"/>
#endif  

#ifdef __MR_CFG_ENTRY_QQ__
    <MENUITEM id="MENU_DSM_QQ" str="STR_DSM_QQ"  img="IMG_DSM_LIST_QQ"/>
#endif  

#ifdef __MR_CFG_ENTRY_EBOOK__
    <MENUITEM id="MENU_DSM_BOOK" str="STR_DSM_BOOK"  img="IMG_DSM_LIST_BOOK"/>
#endif  

#ifdef __SKY_SECURITY_GUARD__    
    <MENUITEM id="MENU_DSM_SAFE" str="STR_ID_SKY_SG"  img="IMG_DSM_LIST_GUARD"/>   
#endif

#ifdef __MR_CFG_ENTRY_MSN__
    <MENUITEM id="MENU_DSM_MSN" str="STR_DSM_MSN"  img="IMG_DSM_LIST_MSN"/>
#endif    

#ifdef __MR_CFG_ENTRY_FETION__
    <MENUITEM id="MENU_DSM_FETION" str="STR_DSM_FETION"  img="IMG_DSM_LIST_FETION"/>
#endif   

#ifdef __MR_CFG_ENTRY_KAIXIN__
    <MENUITEM id="MENU_DSM_KAIXIN" str="STR_DSM_KAIXIN"  img="IMG_DSM_LIST_KAIXIN"/>
#endif   

#ifdef __MR_CFG_ENTRY_TIANQI__
    <MENUITEM id="MENU_DSM_TIANQI" str="STR_DSM_TIANQI"  img="IMG_DSM_LIST_TIANQI"/>
#endif   

#ifdef __MR_CFG_ENTRY_MPLIVE__
    <MENUITEM id="MENU_DSM_MPLIVE" str="STR_DSM_MPLIVE"  img="IMG_DSM_LIST_MPLIVE"/>
#endif

#ifdef __MR_CFG_ENTRY_MPCHAT__  
    <MENUITEM id="MENU_DSM_MPCHAT" str="STR_DSM_MPCHAT"  img="IMG_DSM_LIST_MPCHAT"/>
#endif 

#ifdef __MR_CFG_ENTRY_TOMCAT__
    <MENUITEM id="MENU_DSM_TOMCAT" str="STR_DSM_TOMCAT"  img="IMG_DSM_LIST_TOMCAT"/>
#endif

#ifdef __MR_CFG_ENTRY_FRUIT__
    <MENUITEM id="MENU_DSM_FRUIT" str="STR_DSM_FRUIT"  img="IMG_DSM_LIST_FRUIT"/>
#endif


#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
    <MENUITEM id="MENU_DSM_ANGRYBIRD" str="STR_DSM_BIRD"  img="IMG_DSM_LIST_ANGRYBIRD"/>
#endif

#ifdef __MR_CFG_ENTRY_SKIRT__
    <MENUITEM id="MENU_DSM_SKIRT" str="STR_DSM_SKIRT"  img="IMG_DSM_LIST_SKIRT"/>
#endif

#ifdef __MR_CFG_ENTRY_BYDR__  
    <MENUITEM id="MENU_DSM_BYDR" str="STR_DSM_BYDR"  img="IMG_DSM_LIST_BYDR"/>
#endif

#ifdef __MR_CFG_ENTRY_JS__
    <MENUITEM id="MENU_DSM_JS" str="STR_DSM_JS"  img="IMG_DSM_LIST_JS"/>
#endif

#ifdef __MR_CFG_ENTRY_SINAWEIBO__
    <MENUITEM id="MENU_DSM_SINAWEIBO" str="STR_DSM_SINAWEIBO"  img="IMG_DSM_LIST_SINAWEIBO"/>
#endif

#ifdef __MR_CFG_ENTRY_NETCLOCK__
    <MENUITEM id="MENU_DSM_NETCLOCK" str="STR_DSM_NCLOCK"  img="IMG_DSM_LIST_NETCLOCK"/>
#endif


    <MENUITEM id="MENU_DSM_MRPSTORE" str="STR_DSM_MRPSTORE"  img="IMG_DSM_LIST_GAME"/>   


#ifdef __MR_CFG_ENTRY_YAHOO__
    <MENUITEM id="MENU_DSM_YAHOO" str="STR_DSM_YAHOO"  img="IMG_DSM_LIST_GAME"/>
#endif  

#ifdef __MR_CFG_ENTRY_FACEBOOK__
    <MENUITEM id="MENU_DSM_FACEBOOK" str="STR_DSM_FACEBOOK"  img="IMG_DSM_LIST_GAME"/>
#endif  

#ifdef __MR_CFG_ENTRY_SKYPE__
    <MENUITEM id="MENU_DSM_SKYPE" str="STR_DSM_SKYPE"  img="IMG_DSM_LIST_GAME"/>
#endif     

#ifdef __MR_CFG_ENTRY_TWITTER__
    <MENUITEM id="MENU_DSM_TWITTER" str="STR_DSM_TWITTER"  img="IMG_DSM_LIST_GAME"/>
#endif  

#ifdef __MR_CFG_ENTRY_SKYBUDDY__
    <MENUITEM id="MENU_DSM_SKYBUDDY" str="STR_DSM_SKYBUDDY"  img="IMG_DSM_LIST_SKYBUDDY"/>
#endif    

#ifdef __MR_CFG_ENTRY_SHOUXIN__  
    <MENUITEM id="MENU_DSM_SHOUXIN" str="STR_DSM_SHOUXIN"  img="IMG_DSM_LIST_SHOUXIN"/>
#endif

    <!-----------------Events begin--------------------------------------------------->
    <SENDER id="EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND" hfile="Vapp_mythroad_gprot.h"/>
    
    #ifdef __PME_SUPPORT__
    <SENDER id="EVT_ID_VAPP_MYTHROAD_PME_MSG_EMAIL" hfile="Vapp_mythroad_gprot.h"/>
    <SENDER id="EVT_ID_VAPP_MYTHROAD_PME_MSG_IM" hfile="Vapp_mythroad_gprot.h"/>
    <SENDER id="EVT_ID_VAPP_MYTHROAD_PME_MSG_SYSTEM" hfile="Vapp_mythroad_gprot.h"/>
    #endif
    
    <RECEIVER id="EVT_ID_SRV_SMS_NEW_MSG" proc="mr_app_events_handler"/>
    <RECEIVER id="EVT_ID_USB_ENTER_MS_MODE" proc="mr_app_events_handler"/>
    <RECEIVER id="EVT_ID_VAPP_HOME_ACTIVE" proc="mr_app_events_handler"/>
    <RECEIVER id="EVT_ID_VAPP_HOME_INACTIVE" proc="mr_app_events_handler"/>
    <RECEIVER id="EVT_ID_SRV_MODE_SWITCH_FINISH_NOTIFY" proc="mr_app_events_handler"/>
    <!-----------------Events end--------------------------------------------------->
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1224)	    
    <!--------------------------------------- Timer Resources --------------------------------------------->  
    <TIMER id="DSM_TIMER_START"/>
    <TIMER id="DSM_TIMER"/>
    <TIMER id="DSM_AUDIO_COUNT_TIMER"/>
    <TIMER id="DSM_WEATHER_TIMER"/>       
    <TIMER id="DSM_NETCLOCK_TIMER"/>
    <TIMER id="DSM_VIBRATOR_TIMER"/>
    <TIMER id="DSM_GLANCE_TIMER"/>
    <TIMER id="DSM_DL_APPLIST_SOCKET"/> 
    <TIMER id="DSM_DL_APPLIST_TIMER"/>
    <TIMER id="DSM_TIMER_DUMMY_1"/>
    <TIMER id="DSM_TIMER_MAX"/>         
#endif    
    
</APP>

<FILE_TYPES>
    #ifdef __MMI_DSM_NEW__
    <FILE_TYPE type_id="FMGR_TYPE_MRP" extension="MRP" small_icon="0" large_icon="0"/>
    <FILE_TYPE type_id="FMGR_TYPE_NES" extension="NES" small_icon="0" large_icon="0"/>
    <FILE_TYPE type_id="FMGR_TYPE_ZIP" extension="ZIP" small_icon="0" large_icon="0"/>
    <FILE_TYPE type_id="FMGR_TYPE_ZPK" extension="ZPK" small_icon="0" large_icon="0"/>    
    #endif
</FILE_TYPES>
    