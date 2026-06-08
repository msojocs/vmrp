/****************************************************************************
** File Name:      game_wintab.c                            
** Author:                                                                  
** Date:            2006/04/01
** Copyright:      2004 Spreadtrum, Incorporated. All Rights Reserved.      
** Description:    This file is used to create windows of game
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE               NAME             DESCRIPTION                             
** 04/01/2006     annie         Create
** 
****************************************************************************/


/**---------------------------------------------------------------------------*
**                         Dependencies                                      *
**---------------------------------------------------------------------------*/
#include "mmi_app_game_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#ifdef GAME_SUPPORT
#include "window_parse.h"
#include "mmk_app.h"
#include "guitext.h"
#include "guimsgbox.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmigame_menutable.h"
#include "mmigame_id.h"
#include "tb_dal.h"
//#include "mmiacc_winmain.h"
#include "mmialarm_export.h"
//#include "mmigame.h"
#include "mmigame_internal.h"
#include "mmi_textfun.h"
#include "mmiphone_export.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmigame_link_internal.h"
#include "mmigame_boxman_internal.h"
#ifdef GAME_BUBBLEBOBBLE_SUPPORT
#include "mmigame_bubblebobble_internal.h"
#endif
#ifdef GAME_SNAKE_SUPPORT
#include "mmigame_snake_internal.h"
#endif
#ifdef GAME_TETRIS_SUPPORT
#include "mmigame_square_internal.h"
#endif
//michael wang for5
#include "mmigame_quintet_internal.h"
//michael wang
#ifdef GAME_GOLDMINER_SUPPORT
#include "mmigame_goldminer_internal.h "
#endif
/**-------------------------------------------------------------------------*
**                         GLOBAL DEFINITION                                *
**--------------------------------------------------------------------------*/            
//*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
//*****************************************************************************/
//  Description : 游戏主菜单
//  Global resource dependence : 
//  Author:ycd 01
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleGameMainMenuWindow(
                                             MMI_WIN_ID_T win_id, 
                                             MMI_MESSAGE_ID_E msg_id, 
                                             DPARAM param
                                             );
/*****************************************************************************/
//  Description : HandleGameSetMusicWinMsg
//         
//  Global resource dependence : 
//   Author:        taul.zhu
//  Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleGameSetMusicWinMsg(
                                             MMI_WIN_ID_T win_id,           
                                             MMI_MESSAGE_ID_E msg_id,       
                                             DPARAM param);
                                             /**---------------------------------------------------------------------------*
                                             **                         Constant Variables                                *
**---------------------------------------------------------------------------*/
// 游戏主菜单窗口
WINDOW_TABLE( MMIGAM_MAIN_MENU_WIN_TAB ) = 
{
    //CLEAR_LCD,
        //WIN_PRIO( WIN_ONE_LEVEL ),
        WIN_FUNC( (uint32)HandleGameMainMenuWindow ),    
        WIN_ID( MMIGAME_MAIN_MENU_WIN_ID ),
        WIN_TITLE(TXT_ENTERTAIMENT_GAME),
		WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
        CREATE_MENU_CTRL(MENU_GM_LIST, MMIGAME_MAIN_MENU_CTRL_ID),
        END_WIN
};

WINDOW_TABLE( MMIGAM_ICONMAIN_MENU_WIN_TAB ) = 
{
    //CLEAR_LCD,
        //WIN_PRIO( WIN_ONE_LEVEL ),
        WIN_FUNC( (uint32)HandleGameMainMenuWindow ),    
        WIN_ID( MMIGAME_MAIN_MENU_WIN_ID ),
        WIN_TITLE(TXT_ENTERTAIMENT_GAME),
		WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
        CREATE_MENU_CTRL(MENU_ICON_GM_LIST, MMIGAME_MAIN_MENU_CTRL_ID),
        END_WIN
};

// set music of game
WINDOW_TABLE( GAME_SET_MUSIC_WIN_TAB ) = 
{
    //CLEAR_LCD,
        //WIN_PRIO( WIN_ONE_LEVEL ),
        WIN_FUNC((uint32) HandleGameSetMusicWinMsg ),    
        WIN_ID( GAME_SET_MUSIC_WIN_ID),
        WIN_TITLE(TXT_NULL),
		WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
        CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMIGAME_SETMUSIC_CTRL_ID),
        END_WIN
}; 

/*****************************************************************************/
//  Description :  进入游戏主菜单
//  Global resource dependence : 
//  Author:ycd
//  Note: 
/*****************************************************************************/
void MMIGAM_OpenMainMenuWindow(void)
{
    MMK_CreateWin((uint32 *)MMIGAM_MAIN_MENU_WIN_TAB, PNULL);
}
/*****************************************************************************/
//  Description :  进入游戏主菜单
//  Global resource dependence : 
//  Author:ycd
//  Note: 
/*****************************************************************************/
void MMIGMSetMusic_OpenWin(void)
{
    MMK_CreateWin((uint32 *)GAME_SET_MUSIC_WIN_TAB, PNULL);
}

/*****************************************************************************/
//  Description : 游戏主菜单
//  Global resource dependence : 
//  Author:ycd 01
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleGameMainMenuWindow(
                                             MMI_WIN_ID_T win_id, 
                                             MMI_MESSAGE_ID_E msg_id, 
                                             DPARAM param
                                             )
{
    uint32          group_id = 0; 
    uint32          menu_id  = 0;
    MMI_RESULT_E    recode   = MMI_RESULT_TRUE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_COMMON_OK, TXT_NULL, STXT_RETURN, FALSE);
        MMK_SetAtvCtrl(win_id, MMIGAME_MAIN_MENU_CTRL_ID);
        break;
        
#ifdef TOUCH_PANEL_SUPPORT //IGNORE9527
    case MSG_CTL_PENOK:
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
        GUIMENU_GetId(MMIGAME_MAIN_MENU_CTRL_ID,&group_id,&menu_id);
        switch(menu_id)
        {
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
#ifdef GAME_BUBBLEBOBBLE_SUPPORT           
        case IDGAME_MENU_BUBBLEBOBBLE_ITEM_ID:    
            MMIAPIGMBUBBLEBOBBLE_OpenGameWin();
            break;
#endif                  
#ifdef GAME_GOLDMINER_SUPPORT           
        case IDGAME_MENU_GOLDMINER_ITEM_ID:    
            MMIGMGOLDMINER_OpenGameWin();
            break;
#endif  
//michael wang for5
#ifdef GAME_QUINTET_SUPPORT
	     case IDGAME_MENU_QUT_ITEM_ID:    
            MMIGMQUT_OpenGameWin();
            break;
#endif			
//michael wang	
#ifdef GAME_SNAKE_SUPPORT
        case IDGAME_MENU_SNAKE_ITEM_ID:
           MMIGMSNAKE_OpenGameWin();
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
#if defined(JAVA_SUPPORT_JOY)
        case ID_Menu_JoyRoot:
            {
				MMIAPIJAVA_OpenMainMenu();
            }
            break;
#endif

#ifdef GAME_TETRIS_SUPPORT
        case IDGAME_MENU_SQUARE_ITEM_ID:
            MMIGMSQU_OpenGameWin();
            break;
#endif
        case ID_GM_MUSIC:   
            break;      

        default:
            //SCI_PASSERT(FALSE,("HandleGameMainMenuWindow menu_id is error:%d",menu_id));
            //SCI_TRACE_LOW:"HandleGameMainMenuWindow menu_id is error:%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIGAME_WINTAB_177_112_2_18_2_21_36_23,(uint8*)"d",menu_id);
            break;
        }

        
        break;
        
        
        case MSG_APP_CANCEL:
        case MSG_CTL_CANCEL:
            MMK_CloseWin(win_id);
            break;
            
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    
    return recode;
}

/*****************************************************************************/
//  Description : HandleGameSetMusicWinMsg
//         
//  Global resource dependence : 
//   Author:        taul.zhu
//  Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleGameSetMusicWinMsg(
                                             MMI_WIN_ID_T win_id,           
                                             MMI_MESSAGE_ID_E msg_id,       
                                             DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    return recode;
    
}

/*****************************************************************************/
//  Description :  Reset the game data
//  Global resource dependence : 
//  Author:Annie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIGAME_RsetGameDate(void)
{
#ifdef GAME_SNAKE_SUPPORT
	MMIGMSNAKE_RsetGameDate();
#endif
#ifdef GAME_TETRIS_SUPPORT
	MMIGMSQU_ResetGameDate();
#endif
}

/*****************************************************************************/
//  Description :  Go to the game main menu
//  Global resource dependence : 
//  Author:Annie
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIGAME_OpenMainMenuWindow(void)
{
    GUIMENU_STYLE_E     mainmenu_style = GUIMENU_STYLE_NULL;
    
    //get main menu style
    mainmenu_style = MMIAPISET_GetMainmenuStyle();
    switch (mainmenu_style)
    {
    case GUIMENU_STYLE_ICON:
    case GUIMENU_STYLE_TITLE_ICON:

#ifdef MENU_CYCLONE_SUPPORT
    case GUIMENU_STYLE_CYCLONE:
#endif
#ifdef MENU_CRYSTALCUBE_SUPPORT
    case GUIMENU_STYLE_CRYSTAL_CUBE:
#endif
    case GUIMENU_ISTYLE:    
#ifdef CUBE_MAIN_MENU_SUPPORT
    case GUIMENU_STYLE_CUBE:
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
#ifdef MENU_SWEETS_SUPPORT
   case GUIMENU_STYLE_SWEETS:
#endif		
        MMK_CreateWin((uint32 *)MMIGAM_ICONMAIN_MENU_WIN_TAB, PNULL);
        break;
        
    case GUIMENU_STYLE_OPTION_PAGE:
        MMK_CreateWin((uint32 *)MMIGAM_MAIN_MENU_WIN_TAB, PNULL);
        break;
        
    default:
        //SCI_PASSERT(FALSE,("MMIAPIGAME_OpenMainMenuWindow:mainmenu_style %d is error!",mainmenu_style));
        //SCI_TRACE_LOW:"MMIAPIGAME_OpenMainMenuWindow:mainmenu_style %d is error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIGAME_WINTAB_269_112_2_18_2_21_36_24,(uint8*)"d",mainmenu_style);
        break;
    }
}
#endif



/*Edit by script, ignore 1 case. Fri Apr 27 09:38:50 2012*/ //IGNORE9527
