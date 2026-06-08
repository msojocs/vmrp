/****************************************************************************
** File Name:      game_menutable.c                            
** Author:    Annie.an                                                              
** Date:            2006/04/01
** Copyright:      2004 Spreadtrum, Incorporated. All Rights Reserved.       
** Description:    This file is used to define the menu and related information for game.
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE               NAME             DESCRIPTION                             
** 04/01/2006     annie.an          Create
** 
****************************************************************************/
#ifdef WIN32
#include "std_header.h"
#endif
#ifdef GAME_SUPPORT
#define _GAME_MENUTABLE_C_  



#include "window_parse.h"
#include "guimenu.h"
#include "mmigame_text.h"
#include "mmi_image.h"
#include "mmigame_menutable.h"
#include "mmi_modu_main.h"
#include "mmigame_id.h"
#include "mmi_module.h"
//#include "mmigame_link_menutable.h"
#include "mmigame_link_image.h"
#include "mmigame_boxman_menutable.h"
#include "mmigame_boxman_image.h"
#ifdef GAME_BUBBLEBOBBLE_SUPPORT
#include "mmigame_bubblebobble_menutable.h"
#include "mmigame_bubblebobble_image.h"
#include "mmigame_bubblebobble_text.h"
#endif
#include "mmi_menutable.h"
#include "mmigame_internal.h"
#ifdef GAME_LINK_SUPPORT
#include "mmigame_link_text.h"
#endif
#ifdef GAME_SNAKE_SUPPORT
#include "mmigame_snake_text.h"
#endif
#ifdef GAME_TETRIS_SUPPORT
#include "mmigame_square_text.h"
#endif
#ifdef GAME_GOLDMINER_SUPPORT
#include "mmigame_goldminer_text.h"
#endif
#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif
#ifdef DMENU_SUPPORT
#include "dmenu_mmi_text.h"
#endif
//michael wang for5
#include "mmigame_quintet_text.h"
//michael wang 
//============== MET MEX start ===================
#ifdef MET_MEX_SUPPORT
#include "met_mex_text.h"
#include "met_mex_image.h"
#endif // MET_MEX_SUPPORT
//============== MET MEX start ===================
// begin of game menu 
#if defined (MAINLCD_SIZE_128X160) || defined (MAINLCD_SIZE_176X220)|| defined (MAINLCD_SIZE_128X128)
const GUIMENU_ITEM_T  menu_gm_list[] =
{
#ifdef JAVA_SUPPORT_JOY
	{ID_Menu_JoyRoot, TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN}, TXT_JAVA, NULL, 0, 0, 0},
#endif
#if (defined(TOUCH_PANEL_SUPPORT)&& defined(GAME_LINK_SUPPORT))
    {IDGAME_MENU_LINK_ITEM_ID,  TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN},TXT_COMMON_GAME_LINK,   0,   0,0,0},
#endif
#ifdef GAME_BOXMAN_SUPPORT
    {IDGAME_MENU_BOXMAN_ITEM_ID,TIP_NULL, {TXT_COMMON_OK, TXT_NULL, STXT_RETURN},TXT_BOXMAN,  0,  0,0,0},
#endif
#ifdef GAME_BUBBLEBOBBLE_SUPPORT
    {IDGAME_MENU_BUBBLEBOBBLE_ITEM_ID,TIP_NULL, {TXT_COMMON_OK, TXT_NULL, STXT_RETURN},TXT_BUBBLEBOBBLE,  0,  0,0,0},
#endif
//michael wang for5
#ifdef GAME_QUINTET_SUPPORT
      {IDGAME_MENU_QUT_ITEM_ID,TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN},TXT_GAME_QUINTET, 0 ,  0,0,0},
#endif

#if defined(GAME_SNAKE_SUPPORT) 
    {IDGAME_MENU_SNAKE_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_SNAKE,   0,   0,0,MENU_NULL},
#endif
	
#if defined(GAME_TETRIS_SUPPORT) 
    {IDGAME_MENU_SQUARE_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GMSQU_TETRIS, 0,   0,0,MENU_NULL},
#endif

#if defined(GAME_GOLDMINER_SUPPORT)
    {IDGAME_MENU_GOLDMINER_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GOLDMINER,   0,   0,0,MENU_NULL},
#endif

#if (!defined(GAME_BOXMAN_SUPPORT) && !defined(GAME_LINK_SUPPORT) && !defined(GAME_TETRIS_SUPPORT) && !defined(GAME_SNAKE_SUPPORT) && !defined(GAME_QUINTET_SUPPORT))
    {0,0, {TXT_NULL, TXT_NULL, TXT_NULL},0,  0,  0,0,0}
#endif

#ifdef MRAPP_SUPPORT
#ifdef MRAPP_NETGAME_OPEN
    {ID_MRAPP_NETGAME,TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN},TXT_MRAPP_NETGAME,  0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_COOKIE_OPEN
    {ID_MRAPP_COOKIE, TIP_NULL,{STXT_OK, TXT_NULL, STXT_RETURN},TXT_MRAPP_GAMEC,    0,0,  0,MENU_NULL},
#endif

#ifdef MRAPP_MRPSTORE_OPEN
    {ID_MRAPP_MRPSTORE, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_MRPSTORE,    0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_SKYBUDDY_OPEN
    {ID_MRAPP_SKYBUDDY,         TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_SKYBUDDY,   0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_FETION_OPEN
    {ID_MRAPP_FETION,           TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_FETION,    0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_XLWB_OPEN
    {ID_MRAPP_XLWB,           TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_XLWB,    0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_KAIXIN_OPEN
    {ID_MRAPP_KAIXIN,           TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_KAIXIN,    0,0, 0,MENU_NULL},
#endif
#if defined(MRAPP_SDKQQ_OPEN) || defined(__MMI_SKYQQ__)
    {ID_MRAPP_QQ,                 TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_QQ,   0,0, 0,MENU_NULL},
#endif

#ifdef MRAPP_FACEBOOK_OPEN
    {ID_MRAPP_FACEBOOK, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_FBOOK,  0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_YAHOO_OPEN
    {ID_MRAPP_YAHOO, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_YAHOO,  0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_TWITTER_OPEN
    {ID_MRAPP_TWITTER, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_TWITTER,  0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_CLOCK_OPEN
    {ID_MRAPP_CLOCK,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_CLOCK,  0,0,0,MENU_NULL},
#endif
#ifdef MRAPP_TIANQI_OPEN
    {ID_MRAPP_TIANQI,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_TIANQI,   0,0,0,MENU_NULL},
#endif

#ifdef MRAPP_TALKCAT_OPEN
	{ID_MRAPP_TALKCAT,		TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_TALKCAT,	0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_SKIRT_OPEN
	{ID_MRAPP_SKIRT,		TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_SKIRT,   0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_BIRD_OPEN
	{ID_MRAPP_BIRD,			TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_BIRD,	  0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_FRUIT_OPEN
	{ID_MRAPP_FRUIT,		TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_FRUIT,	0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_BYDR_OPEN
	{ID_MRAPP_BYDR,			TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_BYDR,0,0,  0,MENU_NULL},
#endif
#endif
#ifdef DMENU_SUPPORT
	{ID_DMENU,			TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_DMENU_MAINZONE_TITLE,	   0,0,  0,MENU_NULL},
#endif
};
#else
const GUIMENU_ITEM_T  menu_gm_list[] =
{
#ifdef JAVA_SUPPORT_JOY
	{ID_Menu_JoyRoot, TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN}, TXT_JAVA, NULL, 0, 0, 0},
#endif
#if (defined(TOUCH_PANEL_SUPPORT)&& defined(GAME_LINK_SUPPORT))
    {IDGAME_MENU_LINK_ITEM_ID,  TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN},TXT_COMMON_GAME_LINK,   IMAGE_SECMENU_ICON_PASTIME_LINKS,   0,0,0},
#endif
#ifdef GAME_BOXMAN_SUPPORT
#ifndef MMI_SECMENU_ICON_DEFAULT			//add by zhangming @20130106
    {IDGAME_MENU_BOXMAN_ITEM_ID,TIP_NULL, {TXT_COMMON_OK, TXT_NULL, STXT_RETURN},TXT_BOXMAN,  IMAGE_SECMENU_ICON_PASTIME_BOXMAN,  0,0,0},
#else
    {IDGAME_MENU_BOXMAN_ITEM_ID,TIP_NULL, {TXT_COMMON_OK, TXT_NULL, STXT_RETURN},TXT_BOXMAN,  0,  0,0,0},
#endif
#endif
#if defined(GAME_GOLDMINER_SUPPORT)
    {IDGAME_MENU_GOLDMINER_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GOLDMINER,   IMAGE_NULL,   0,0,MENU_NULL},
#endif
//michael wang for5
#ifdef GAME_BUBBLEBOBBLE_SUPPORT
    {IDGAME_MENU_BUBBLEBOBBLE_ITEM_ID,TIP_NULL, {TXT_COMMON_OK, TXT_NULL, STXT_RETURN},TXT_BUBBLEBOBBLE,  IMAGE_SECMENU_ICON_PASTIME_BUBBLEBOBBLE,  0,0,0},
#endif
#ifdef GAME_QUINTET_SUPPORT
      {IDGAME_MENU_QUT_ITEM_ID,TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN},TXT_GAME_QUINTET, IMAGE_SECMENU_ICON_PASTIME_QUT ,  0,0,0},
#endif
#if defined(GAME_SNAKE_SUPPORT) 
#ifndef MMI_SECMENU_ICON_DEFAULT
    {IDGAME_MENU_SNAKE_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_SNAKE,   IMAGE_SECMENU_ICON_PASTIME_SNAKE,   0,0,MENU_NULL},
#else
    {IDGAME_MENU_SNAKE_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_SNAKE,   0,   0,0,MENU_NULL},
#endif
#endif

#if defined(GAME_TETRIS_SUPPORT) 
#ifndef MMI_SECMENU_ICON_DEFAULT
    {IDGAME_MENU_SQUARE_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GMSQU_TETRIS, IMAGE_SECMENU_ICON_PASTIME_TETRIS,   0,0,MENU_NULL},
#else
    {IDGAME_MENU_SQUARE_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GMSQU_TETRIS, 0,   0,0,MENU_NULL},
#endif
#endif

#if defined(GAME_GOLDMINER_SUPPORT) 
#ifndef MMI_SECMENU_ICON_DEFAULT
    {IDGAME_MENU_GOLDMINER_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GOLDMINER, IMAGE_SECMENU_ICON_PASTIME_GOLDMINER,   0,0,MENU_NULL},
#else
    {IDGAME_MENU_GOLDMINER_ITEM_ID,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_GOLDMINER, 0,   0,0,MENU_NULL},
#endif
#endif

#if (!defined(GAME_BOXMAN_SUPPORT) && !defined(GAME_LINK_SUPPORT) && !defined(GAME_TETRIS_SUPPORT) && !defined(GAME_SNAKE_SUPPORT) && !defined(GAME_QUINTET_SUPPORT))
    {0,0, {TXT_NULL, TXT_NULL, TXT_NULL},0,  0,  0,0,0}
#endif
#ifdef MRAPP_SUPPORT
#ifdef MRAPP_NETGAME_OPEN
    {ID_MRAPP_NETGAME,TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN},TXT_MRAPP_NETGAME,  0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_COOKIE_OPEN
    {ID_MRAPP_COOKIE, TIP_NULL,{STXT_OK, TXT_NULL, STXT_RETURN},TXT_MRAPP_GAMEC,    0,0,  0,MENU_NULL},
#endif
 
#ifdef MRAPP_MRPSTORE_OPEN
    {ID_MRAPP_MRPSTORE, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_MRPSTORE,    0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_SKYBUDDY_OPEN
    {ID_MRAPP_SKYBUDDY,         TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_SKYBUDDY,   0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_FETION_OPEN
    {ID_MRAPP_FETION,           TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_FETION,    0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_XLWB_OPEN
    {ID_MRAPP_XLWB,           TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_XLWB,    0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_KAIXIN_OPEN
    {ID_MRAPP_KAIXIN,           TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_KAIXIN,    0,0, 0,MENU_NULL},
#endif

#if defined(MRAPP_SDKQQ_OPEN) || defined(__MMI_SKYQQ__)
    {ID_MRAPP_QQ,                 TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_QQ,   0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_FACEBOOK_OPEN
    {ID_MRAPP_FACEBOOK, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_FBOOK,  0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_YAHOO_OPEN
    {ID_MRAPP_YAHOO, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_YAHOO,  0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_TWITTER_OPEN
    {ID_MRAPP_TWITTER, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_TWITTER,  0,0, 0,MENU_NULL},
#endif
#ifdef MRAPP_CLOCK_OPEN
    {ID_MRAPP_CLOCK,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_CLOCK,  0,0,0,MENU_NULL},
#endif
#ifdef MRAPP_TIANQI_OPEN
    {ID_MRAPP_TIANQI,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_TIANQI,   0,0,0,MENU_NULL},
#endif
#ifdef MRAPP_TALKCAT_OPEN
	{ID_MRAPP_TALKCAT,		TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_TALKCAT,	0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_SKIRT_OPEN
	{ID_MRAPP_SKIRT,		TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_SKIRT,   0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_BIRD_OPEN
	{ID_MRAPP_BIRD,			TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_BIRD,	  0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_FRUIT_OPEN
	{ID_MRAPP_FRUIT,		TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_FRUIT,	0,0,  0,MENU_NULL},
#endif
#ifdef MRAPP_BYDR_OPEN
	{ID_MRAPP_BYDR,			TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MRAPP_BYDR,0,0,  0,MENU_NULL},
#endif
#endif
#ifdef DMENU_SUPPORT
{ID_DMENU,			TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_DMENU_MAINZONE_TITLE,	   0,0,  0,MENU_NULL},
#endif

};
#endif
#define MENU_DEF(_TABLEID, _ITEMPTR, _STYLE, _TITLESTR, _TITLE_ICON, _TITLE_NUMICON, _TITLE_BACKGROUND, \
    _ITEMCOUNT) \
_ITEMPTR, _STYLE, _TITLESTR, _TITLE_ICON, _TITLE_NUMICON, _TITLE_BACKGROUND, _ITEMCOUNT, 

const GUIMENU_GROUP_T game_menu_table[] = 
{
    {NULL},
#include "mmigame_menutable.def"
        //#include "mmieng_menutable.def"
};
#undef  MENU_DEF

/*****************************************************************************/
// 	Description :  Register menu group
//	Global resource dependence : 
//  Author:KELLY.LI
//	Note: 
/*****************************************************************************/
PUBLIC void MMIGAME_RegMenuGroup(void)
{
    MMI_RegMenuGroup(MMI_MODULE_GAME_MANAGER, game_menu_table);
}
#endif

