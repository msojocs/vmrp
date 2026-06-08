/* Needed header files of the compile option in XML files, if you need others need to add here */
#include "mmi_features.h"
#include "custresdef.h"

#ifdef __PME_SUPPORT__

#define PME_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\pme"                       

#if (defined(__MMI_MAINLCD_128X160__)||defined(__MMI_MAINLCD_176X220__)||__MMI_MAINLCD_220X176__)
#define PME_STATUS_RES_ICONS_PATH             PME_RES_PATH"\\\\status_icon"
#define PME_LIST_RES_ICONS_PATH             PME_RES_PATH"\\\\list_icon\\\\16"
#else
#define PME_STATUS_RES_ICONS_PATH             PME_RES_PATH"\\\\status_icon"
#define PME_LIST_RES_ICONS_PATH             PME_RES_PATH"\\\\list_icon\\\\24"
#endif

#define PME_MAINMENU_ICON_PATH              PME_RES_PATH

/* Need this line to tell parser that XML start, must after all #include. */
<?xml version="1.0" encoding="UTF-8"?>

/* APP tag, include your app name defined in MMIDataType.h */
<APP id="VAPP_PME" package_name="native.mtk.pme" name="STR_PME_MAINSCR" img="IMG_PME_LIST_IN_MAINMENU" launch="vapp_pme_launch" type="venus">

    /* When you use any ID of other module, you need to add
       that header file here, so that Resgen can find the ID */
    <!--Include Area-->
    <INCLUDE file="GlobalResDef.h"/>

    <!-----------------------------------------------------String Resource Area----------------------------------------------------->
    /* String ID of you Application, we will get string from ref_list.txt for all languages */
    <STRING id="STR_PME_MAINSCR"/>
	<STRING id="STR_PME_SUBSCR"/>
	<STRING id="STR_PME_NEWMSG"/>
    <STRING id="STR_PME_SNDMSG"/>
    <STRING id="STR_PME_SETTING"/>
	<STRING id="STR_PME_NAVI_PREV"/>
    <STRING id="STR_PME_NAVI_NEXT"/>
	<STRING id="STR_PME_MOD_SET"/>
	<STRING id="STR_PME_MOD_NORMAL"/>
	<STRING id="STR_PME_MOD_URGENT"/>
	<STRING id="STR_PME_MOD_SLEEP"/>
	<STRING id="STR_PME_MOD_CLOSED"/>
	<STRING id="STR_PME_RING_SET"/>
	<STRING id="STR_PME_RING_CLOSE"/>
	<STRING id="STR_PME_RING_OPEN"/>
	<STRING id="STR_PME_STORAGE_SET"/>
	<STRING id="STR_PME_STORAGE_TCARD"/>
	<STRING id="STR_PME_STORAGE_PHONE"/>
	
	<STRING id="STR_PME_REPLY_MSG"/>
	<STRING id="STR_PME_START_IM"/>
	<STRING id="STR_PME_START_EMAIL"/>
	<STRING id="STR_PME_START_APP"/>
	<STRING id="STR_PME_DOWN_LAOD"/>
	<STRING id="STR_PME_START_CALL"/>
	<STRING id="STR_PME_STORAGE_LOWER"/>

    <!-----------------------------------------------------Image Resource Area------------------------------------------------------>
    /* Image Id and path of you Application , you can use compile option in Path, but need out of "" */
    <IMAGE id="IMG_PME_STATUS_ICON_SYS">PME_STATUS_RES_ICONS_PATH"\\\\sys.png"</IMAGE>
    <IMAGE id="IMG_PME_STATUS_ICON_EMAIL">PME_STATUS_RES_ICONS_PATH"\\\\mail.png"</IMAGE>
    <IMAGE id="IMG_PME_STATUS_ICON_IM">PME_STATUS_RES_ICONS_PATH"\\\\im.png"</IMAGE>
    
	<IMAGE id="IMG_PME_LIST_MAIN">PME_LIST_RES_ICONS_PATH"\\\\icon.png"</IMAGE>
	<IMAGE id="IMG_PME_LIST_MSG_NEW">PME_LIST_RES_ICONS_PATH"\\\\msg_new.png"</IMAGE>
    <IMAGE id="IMG_PME_LIST_MSG_READ">PME_LIST_RES_ICONS_PATH"\\\\msg.png"</IMAGE>
	<IMAGE id="IMG_PME_LIST_ACNT_NEW">PME_LIST_RES_ICONS_PATH"\\\\acnt_new.png"</IMAGE>
    <IMAGE id="IMG_PME_LIST_ACNT_READ">PME_LIST_RES_ICONS_PATH"\\\\acnt.png"</IMAGE>
    
    <IMAGE id="IMG_PME_LIST_IN_MAINMENU">PME_MAINMENU_ICON_PATH"\\\\pme.png"</IMAGE>
    
    <!----- Screen Resource Area ---------------------------------------------->
 	//#ifdef __PME_USE_GPROUP__	
 	<SCREEN id="GRP_ID_PME_MAIN"/>
	//#endif
    //<SCREEN id="GRP_ID_CALC"/>
	<SCREEN id="SCR_PME_ENTRY_LOGO"/>
    <SCREEN id="SCR_PME_POP_NEWMSG"/>
  	<SCREEN id="SCR_PME_POP_SNDMSG"/>
  	<SCREEN id="SCR_PME_POP_DELMSG"/>
  	<SCREEN id="SCR_PME_POP_DELACC"/>
	<SCREEN id="SCR_PME_MAIN_SCREEN"/>
	<SCREEN id="SCR_PME_SUB_LIST"/>
	<SCREEN id="SCR_PME_MAIN_OPTION"/>
	<SCREEN id="SCR_PME_SUB_OPTION"/>
 	<SCREEN id="SCR_PME_TEXT_DETAIL"/>
 	<SCREEN id="SCR_PME_EDIT_SNDMSG"/>
 	<SCREEN id="SCR_PME_OPTION_SETTING"/>
  	<SCREEN id="SCR_PME_SETTING_MOD"/>
 	<SCREEN id="SCR_PME_SETTING_RING"/>
 	<SCREEN id="SCR_PME_SETTING_STORAGE"/>
 	<SCREEN id="SCR_PME_SETTING_TIMES"/>
 	

    <!------------------------------------------------------Menu Resource Area------------------------------------------------------>
    <MENU id="MENU_PME_MAIN" str="STR_PME_MAINSCR" img="IMG_PME_LIST_MAIN" highlight="pme_main_highlight_handler" 
    shortcut="ON" shortcut_img="IMG_PME_LIST_MAIN" launch="pme_main_entry">
    </MENU>
    

</APP>

#endif /* __PME_SUPPORT__ */

