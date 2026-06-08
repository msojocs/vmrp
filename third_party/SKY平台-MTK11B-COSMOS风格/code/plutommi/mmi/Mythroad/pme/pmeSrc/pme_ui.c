#ifdef __PME_SUPPORT__

#include "MMI_Include.h"

#include "mrporting.h"
#include "pme_i.h"
#include "pme_def.h"
#include "pme.h"

#ifdef __IS_10A__
#include "ProfilesSrvGprot.h"
#include "mmi_rp_app_sms_def.h"
#elif defined(__IS_11A__)
#include "ProfilesSrvGprot.h"
#include "wgui_inline_edit.h"
#include "gui_typedef.h"
#include "wgui_categories_util.h"
#include "vapp_mythroad_gprot.h"
#else
#include "FileManagerGprot.h"
#include "SettingProfileEnum.h"
#include "mmidsmdef.h"
#include "MessagesResourceData.h"
#endif

#include "resource_audio.h"
#include "IdleAppDef.h"
#include "..\plutommi\framework\gui\gui_inc\Wgui_inline_edit.h"
#include "wgui_categories_util.h"

/*****************************************************************************
 * CONST & STRUCT
 * 
 *****************************************************************************/

#define PME_MAIN_LOAD_TIME 2000//ms
#define PME_MAIN_SCREEN_ITEM_MAX 5
#define PME_SUB_SCREEN_ITEM_MAX 10
#define PME_REPLY_TEXT_MAX_LEN 512//字符

typedef struct
{
	//uint16 *title;	
	uint8 mainHili;//主菜单hili
	uint8 subHili;//list 子菜单HILI
	uint8 opHili;//option菜单hili
	uint8 setHili;//setting菜单hili
	uint8 setSubHili;//setting 菜单hili
	uint8 opType;//
	
	int8 stype;
	int8 otype;
	int8 canReply;
	uint8 opNum;
	int32 vonder;

	/*for reply msg*/
	char *edtBuf;
	
}pme_main_screen_st;

static pme_main_screen_st s_pme_list_screen = {0};
U16 pme_list_icons_image_list[PME_SUB_SCREEN_ITEM_MAX + 2] = {0};




extern U8 hintData[MAX_SUB_MENUS][MAX_SUB_MENU_HINT_SIZE];
extern PU8 hintDataPtrs[MAX_SUB_MENUS];
extern U8 subMenuData[MAX_SUB_MENUS][MAX_SUB_MENU_SIZE];
extern PU8 subMenuDataPtrs[MAX_SUB_MENUS];
extern wgui_inline_item wgui_inline_items[];

extern const U16 gIndexIconsImageList[];
extern pBOOL IsBackHistory;
static void pme_main_screen_show(int32 refresh);
extern void ShowCategory74Screen_ext(U8 * title,U16 title_icon,U16 left_softkey,U16 left_softkey_icon,U16 right_softkey,U16 right_softkey_icon,U8* buffer,S32 buffer_size,U8* history_buffer);



#define __PME_USE_GPROUP__






/*****************************************************************************
 * SUPPORT FUNCTION
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_list_get_cust_str
 * DESCRIPTION: 
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
char* pme_list_get_cust_str(pme_list_str_en id)
{
	switch(id){
		case DAPLS_NAV_PREV:
			return (char*)get_string(STR_PME_NAVI_PREV);
		case DAPLS_NAV_NEXT:
			return (char*)get_string(STR_PME_NAVI_NEXT);
		default:
			break;	
	}
	
	return NULL;
}

/*****************************************************************************
 * FUNCTION: pme_list_storage_space_is_lower
 * DESCRIPTION: 
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
static int32 pme_list_storage_space_check(void)
{	
	if(pme_port_storage_space_is_lower(pme_port_get_drv_name(pme_get_storage()), PME_STORAGE_FREE_SPACE_MIN)){
		//mmi_popup_display((WCHAR*)text, event_id, NULL);
		DisplayPopup((PU8) GetString(STR_GLOBAL_NOT_ENOUGH_MEMORY), 
							IMG_GLOBAL_WARNING, 
							1, 
							ST_NOTIFYDURATION, 
							WARNING_TONE 
							);
		return TRUE;
	}

	return FALSE;
}


/*****************************************************************************
 * MSG REPLY
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_msg_replyscreen_free_res
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_replyscreen_free_res(void)
{
	pme_ui_trace((MOD_MMI,"pme_msg_replyscreen_free_res()"));
	if(s_pme_list_screen.edtBuf){
		pme_port_mem_free((void**)&s_pme_list_screen.edtBuf);
		s_pme_list_screen.edtBuf = NULL;
	}
}

/*****************************************************************************
 * FUNCTION: pme_msg_replyscreen_lsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_replyscreen_lsk(void)
{
	int32 ret;
	char* destBuf = s_pme_list_screen.edtBuf + (PME_REPLY_TEXT_MAX_LEN + 1)*2;
	pme_ui_trace((MOD_MMI,"pme_msg_replyscreen_lsk()"));
 
	pme_port_ucs2_to_utf8_string((uint8 *)destBuf, (PME_REPLY_TEXT_MAX_LEN + 1)*4, (uint8 *)s_pme_list_screen.edtBuf);
 	ret = pme_list_msg_send_context(destBuf, strlen(destBuf));
	if(ret == MR_FAILED){
	    DisplayPopup((PU8) GetString(STR_GLOBAL_FAILED_TO_SEND), 
	                        IMG_GLOBAL_FAIL, 
	                        1, 
	                        ST_NOTIFYDURATION, 
	                        0 
	                        );
	}else if(ret == MR_WAITING){
		DisplayPopup((PU8) GetString(STR_GLOBAL_BUSY_TRY_LATER), 
	                        IMG_GLOBAL_FAIL, 
	                        1, 
	                        ST_NOTIFYDURATION, 
	                        0 
	                        );
	}else{
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
	    DisplayPopup((PU8) GetString(STR_GLOBAL_SENDING), 
	                        IMG_GLOBAL_OK, 
	                        1, 
	                        ST_NOTIFYDURATION, 
	                        0 
	                        );
		//mmi_popup_display((WCHAR*)text, event_id, NULL);
		pme_msg_replyscreen_free_res();
	}
}

/*****************************************************************************
 * FUNCTION: pme_msg_replyscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_replyscreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_msg_replyscreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
	pme_msg_replyscreen_free_res();
}

/*****************************************************************************
 * FUNCTION: pme_msg_replyscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_replyscreen_exit(void)
{
     //screen out.
	 pme_ui_trace((MOD_MMI,"pme_msg_replyscreen_exit()"));
}

/*****************************************************************************
 * FUNCTION: pme_msg_replyscreen_del
 * DESCRIPTION: screen been  deleted
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static U8 pme_msg_replyscreen_del(void *data)
{
	pme_ui_trace((MOD_MMI,"pme_msg_replyscreen_del(), %d", data));
	pme_msg_replyscreen_free_res();
	return MMI_FALSE;
}

#ifdef __PME_USE_GPROUP__
static mmi_ret pme_msg_group_proc(mmi_event_struct *evt)
{
	pme_ui_trace((MOD_MMI,"pme_msg_group_proc(), %d", evt->evt_id));
	switch(evt->evt_id)
	{
		case EVT_ID_SCRN_DEINIT:
		mmi_frm_group_close(GRP_ID_PME_MAIN);
		break;
	}	
	
	return MMI_RET_OK;
}

void pme_msg_enter_group(void)
{
	if(mmi_frm_group_is_present(GRP_ID_PME_MAIN))
	{
		mmi_frm_set_active_group(GRP_ID_PME_MAIN);
	}
	else
	{
		mmi_frm_group_create(GRP_ID_ROOT, GRP_ID_PME_MAIN, pme_msg_group_proc, (void*)NULL);
		mmi_frm_group_enter(GRP_ID_PME_MAIN, MMI_FRM_NODE_SMART_CLOSE_FLAG);
	}	
}
static mmi_ret pme_msg_screen_levave_SNDMSG(mmi_event_struct *evt)
{
	mmi_ret ret = MMI_RET_OK;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
 	pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_SNDMSG(),%d",evt->evt_id)); 
	switch (evt->evt_id)
	{
		case EVT_ID_SCRN_DELETE_REQ_IN_END_KEY:
		case EVT_ID_SCRN_DELETE_REQ:
 		case EVT_ID_SCRN_GOBACK_IN_END_KEY:
 			pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_SNDMSG() 222 ")); 
			pme_msg_replyscreen_del(0);
			break;

		case EVT_ID_SCRN_DEINIT:
			break;
	}

	return ret;

};
#endif
/*****************************************************************************
 * FUNCTION: pme_msg_replyscreen_entry
 * DESCRIPTION: show text screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
 #ifdef __PME_USE_GPROUP__
 void pme_msg_replyscreen_entry(void)
{
	U8* gui_buffer = NULL;
 	pme_ui_trace((MOD_MMI, "pme_msg_replyscreen_entry()")); 

	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_POP_SNDMSG, 
	     pme_msg_replyscreen_exit, 
	     (FuncPtr)pme_msg_replyscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	/* set leave proc */
	pme_ui_trace((MOD_MMI,"pme_msg_screen_levave_SNDMSG set"));
	mmi_frm_scrn_set_leave_proc(GRP_ID_PME_MAIN, SCR_PME_POP_SNDMSG, pme_msg_screen_levave_SNDMSG);
	gui_buffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_POP_SNDMSG);
	if(!gui_buffer)
	{
		s_pme_list_screen.edtBuf = (char*)pme_port_mem_malloc((PME_REPLY_TEXT_MAX_LEN + 1)*6);
 		pme_ui_trace((MOD_MMI, "pme_msg_replyscreen_entry() 222")); 
		if(!s_pme_list_screen.edtBuf)
		{
			mmi_frm_scrn_close_active_id();
			return;
		}
		memset(s_pme_list_screen.edtBuf, 0, (PME_REPLY_TEXT_MAX_LEN + 1)*6);
	}

	ShowCategory5Screen_ext(0, 0,
                                 STR_GLOBAL_OK, 0,
                                 STR_GLOBAL_BACK, 0,
                                 IMM_INPUT_TYPE_SENTENCE,
                                 (U8*)s_pme_list_screen.edtBuf, (PME_REPLY_TEXT_MAX_LEN + 1), 
                                 gui_buffer,0,NULL
                                 );
	SetLeftSoftkeyFunction(pme_msg_replyscreen_lsk, KEY_EVENT_UP);
	SetCategory5RightSoftkeyFunction(pme_msg_replyscreen_rsk , KEY_EVENT_UP);
	pme_ui_trace((MOD_MMI, "pme_msg_replyscreen_entry() 333")); 
}
 #else
void pme_msg_replyscreen_entry(void)
{
	U8* gui_buffer = NULL;
 	pme_ui_trace((MOD_MMI, "pme_msg_replyscreen_entry()")); 

	EntryNewScreen(SCR_PME_POP_SNDMSG, pme_msg_replyscreen_exit, pme_msg_replyscreen_entry, NULL);	 
	SetDelScrnIDCallbackHandler(SCR_PME_POP_SNDMSG, pme_msg_replyscreen_del);
	gui_buffer = GetCurrGuiBuffer(SCR_PME_POP_SNDMSG);
	if(!gui_buffer){
		s_pme_list_screen.edtBuf = (char*)pme_port_mem_malloc((PME_REPLY_TEXT_MAX_LEN + 1)*6);
		if(!s_pme_list_screen.edtBuf){
			GoBackHistory();
			return;
		}
		memset(s_pme_list_screen.edtBuf, 0, (PME_REPLY_TEXT_MAX_LEN + 1)*6);
	}

	ShowCategory5Screen_ext(0, 0,
                                 STR_GLOBAL_OK, 0,
                                 STR_GLOBAL_BACK, 0,
                                 IMM_INPUT_TYPE_SENTENCE,
                                 (U8*)s_pme_list_screen.edtBuf, (PME_REPLY_TEXT_MAX_LEN + 1), 
                                 gui_buffer,0,NULL
                                 );
	SetLeftSoftkeyFunction(pme_msg_replyscreen_lsk, KEY_EVENT_UP);
	SetCategory5RightSoftkeyFunction(pme_msg_replyscreen_rsk , KEY_EVENT_UP);
}
#endif
/*****************************************************************************
 * MSG OPTION
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_msg_optionscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_optionscreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_msg_optionscreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.opHili = hilight;
}

/*****************************************************************************
 * FUNCTION: pme_msg_optionscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_optionscreen_exit(void)
{
     //screen out.
	 pme_ui_trace((MOD_MMI,"pme_msg_optionscreen_exit()"));
}

/*****************************************************************************
 * FUNCTION: pme_msg_optionscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_optionscreen_lsk(void)
{
	int32 nNumofItems = 0;

	if(s_pme_list_screen.canReply){//直接回复
		if(nNumofItems == s_pme_list_screen.opHili){
			pme_list_msg_option(PME_MSG_OPTION_REPLY);
		}
		
		nNumofItems++;
	}

	if(s_pme_list_screen.vonder > 0){//登录app
		switch(s_pme_list_screen.stype){
			case PME_STYPE_IM://STR_PME_START_APP
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_IM);
				}
				break;
			case PME_STYPE_EMAIL://STR_PME_START_EMAIL
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_EMAIL);
				}
				break;
			default://STR_PME_START_APP
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_APPENTRY);
				}
				break;
		}
		
		nNumofItems++;
	}
	
	if(s_pme_list_screen.otype > PME_OTYPE_NONE){//启动附加功能
		switch(s_pme_list_screen.otype){
			case PME_OTYPE_WAP:
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_WAP);
				}
				break;
			case PME_OTYPE_CALL:
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_CALL);
				}
				break;
			case PME_OTYPE_SMS:
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_SMS);
				}
				break;
			case PME_OTYPE_APPENTRY:
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_APPENTRY);
				}
				break;
			case PME_OTYPE_APPLOAD:
				if(nNumofItems == s_pme_list_screen.opHili){
					pme_list_msg_option(PME_MSG_OPTION_APPLOAD);
				}
				break;
		}
		
		nNumofItems++;
	}
	
}

/*****************************************************************************
 * FUNCTION: pme_msg_optionscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_optionscreen_rsk(void)
{
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_msg_optionscreen_entry
 * DESCRIPTION: start app option
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_optionscreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_msg_optionscreen_entry()"));
	
#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_SUB_OPTION, 
	     pme_msg_optionscreen_exit, 
	     (FuncPtr)pme_msg_optionscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SUB_OPTION);	
#else
	EntryNewScreen(SCR_PME_SUB_OPTION, pme_msg_optionscreen_exit, pme_msg_optionscreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SUB_OPTION);
#endif
	RegisterHighlightHandler(pme_msg_optionscreen_hilite);
	if(!guiBuffer){
		s_pme_list_screen.opHili = 0;
	}

	if(s_pme_list_screen.canReply){//直接回复
		menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_REPLY_MSG);
	}

	if(s_pme_list_screen.vonder > 0){//登录app
		switch(s_pme_list_screen.stype){
			case PME_STYPE_IM://STR_PME_START_APP
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_APP);
				break;
			case PME_STYPE_EMAIL://STR_PME_START_EMAIL
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_EMAIL);
				break;
			default://STR_PME_START_APP
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_APP);
				break;
		}	
	}
	
	if(s_pme_list_screen.otype > PME_OTYPE_NONE){//启动附加功能
		switch(s_pme_list_screen.otype){
			case PME_OTYPE_WAP:
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_APP);
				break;
			case PME_OTYPE_CALL:
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_CALL);
				break;
			case PME_OTYPE_SMS:
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_APP);
				break;
			case PME_OTYPE_APPENTRY:
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_START_APP);
				break;
			case PME_OTYPE_APPLOAD:
				menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_DOWN_LAOD);
				break;
		}
	}
	
	ShowCategory353Screen((U8 *)GetString(STR_GLOBAL_OK), 0,
						  STR_GLOBAL_OK, IMG_GLOBAL_OK,
						  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
						  (S32)nNumofItems, 
						  (U8**)menuStringList,
						  (U16*)gIndexIconsImageList,
						  NULL, 0, 
						  (S32)s_pme_list_screen.opHili, 
						  guiBuffer);

	SetRightSoftkeyFunction(pme_msg_optionscreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_msg_optionscreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_msg_optionscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * SCR - MSG DETAIL
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: 
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_textscreen_exit(void)
{
	pme_ui_trace((MOD_MMI,"pme_msg_textscreen_exit()"));
	//screen out.
}

/*****************************************************************************
 * FUNCTION: pme_msg_textscreen_lsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_textscreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_msg_textscreen_lsk()"));

	if(s_pme_list_screen.opNum == 1){//ok
		if(s_pme_list_screen.vonder > 0){
			switch(s_pme_list_screen.stype){
				case PME_STYPE_IM://STR_PME_START_APP
					pme_list_msg_option(PME_MSG_OPTION_IM);
					break;
				case PME_STYPE_EMAIL://STR_PME_START_EMAIL
					pme_list_msg_option(PME_MSG_OPTION_EMAIL);
					break;
				default://STR_PME_START_APP
					pme_list_msg_option(PME_MSG_OPTION_APPENTRY);
					break;
			}
		}else if(s_pme_list_screen.canReply){
			pme_list_msg_option(PME_MSG_OPTION_REPLY);
		}else{//(otype > PME_OTYPE_NONE)
			switch(s_pme_list_screen.otype){
				case PME_OTYPE_WAP:
					pme_list_msg_option(PME_MSG_OPTION_WAP);
					break;
				case PME_OTYPE_CALL:
					pme_list_msg_option(PME_MSG_OPTION_CALL);
					break;
				case PME_OTYPE_SMS:
					pme_list_msg_option(PME_MSG_OPTION_SMS);
					break;
				case PME_OTYPE_APPENTRY:
					pme_list_msg_option(PME_MSG_OPTION_APPENTRY);
					break;
				case PME_OTYPE_APPLOAD:
					pme_list_msg_option(PME_MSG_OPTION_APPLOAD);
					break;
			}
		}
		
	}else if(s_pme_list_screen.opNum > 1){//option
		pme_msg_optionscreen_entry();
	}
	
}

/*****************************************************************************
 * FUNCTION: 
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_textscreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_msg_textscreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_msg_textscreen_entry
 * DESCRIPTION: show text screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_msg_textscreen_entry(void)
{
	U8* gui_buffer; 
	S32 iBufLength = 0;
	uint16 *txtData;
	uint16 *txtTitle;
	pme_ui_trace((MOD_MMI,"pme_msg_textscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_TEXT_DETAIL, 
	     pme_msg_textscreen_exit, 
	     (FuncPtr)pme_msg_textscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	gui_buffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_TEXT_DETAIL);	
#else
	EntryNewScreen(SCR_PME_TEXT_DETAIL, pme_msg_textscreen_exit, pme_msg_textscreen_entry, NULL);
	gui_buffer = GetCurrGuiBuffer(SCR_PME_TEXT_DETAIL);
#endif
	iBufLength = pme_list_msg_get_context(&txtData, &txtTitle);
	ShowCategory74Screen(STR_GLOBAL_DETAILS, 0,
							NULL, NULL,
							STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							(U8*)txtData,
							iBufLength,
							gui_buffer); 

	s_pme_list_screen.opNum = 0;
	s_pme_list_screen.vonder = pme_list_msg_get_type(&s_pme_list_screen.stype, &s_pme_list_screen.otype, &s_pme_list_screen.canReply);
	if(s_pme_list_screen.vonder != MR_FAILED){
		if(s_pme_list_screen.vonder > 0){//登录app
			s_pme_list_screen.opNum++;
		}
		
		if(s_pme_list_screen.canReply){//直接回复
			s_pme_list_screen.opNum++;
		}

		if(s_pme_list_screen.otype > PME_OTYPE_NONE){//启动附加功能
			s_pme_list_screen.opNum++;
		}
	}

	if(s_pme_list_screen.opNum == 1){
		if(s_pme_list_screen.vonder > 0){
			switch(s_pme_list_screen.stype){
				case PME_STYPE_IM:
					ChangeLeftSoftkey(STR_PME_START_APP, IMG_GLOBAL_OK);
					break;
				case PME_STYPE_EMAIL:
					ChangeLeftSoftkey(STR_PME_START_EMAIL, IMG_GLOBAL_OK);
					break;
				default:
					ChangeLeftSoftkey(STR_PME_START_APP, IMG_GLOBAL_OK);
					break;
			}
		}else if(s_pme_list_screen.canReply){
			ChangeLeftSoftkey(STR_PME_REPLY_MSG, IMG_GLOBAL_OK);
		}else{//(otype > PME_OTYPE_NONE)
			switch(s_pme_list_screen.otype){
				case PME_OTYPE_WAP:
					ChangeLeftSoftkey(STR_PME_START_APP, IMG_GLOBAL_OK);
					break;
				case PME_OTYPE_CALL:
					ChangeLeftSoftkey(STR_PME_START_CALL, IMG_GLOBAL_OK);
					break;
				case PME_OTYPE_SMS:
					ChangeLeftSoftkey(STR_PME_START_APP, IMG_GLOBAL_OK);
					break;
				case PME_OTYPE_APPENTRY:
					ChangeLeftSoftkey(STR_PME_START_APP, IMG_GLOBAL_OK);
					break;
				case PME_OTYPE_APPLOAD:
					ChangeLeftSoftkey(STR_PME_DOWN_LAOD, IMG_GLOBAL_OK);
					break;
			}
		}
		
		SetLeftSoftkeyFunction(pme_msg_textscreen_lsk, KEY_EVENT_UP);
		SetKeyHandler(pme_msg_textscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
	}else if(s_pme_list_screen.opNum > 1){
		ChangeLeftSoftkey(STR_GLOBAL_OPTIONS, IMG_GLOBAL_OK);
		
		SetLeftSoftkeyFunction(pme_msg_textscreen_lsk, KEY_EVENT_UP);
		SetKeyHandler(pme_msg_textscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
	}else{
		ChangeLeftSoftkey(NULL, NULL);
	}
	
	SetRightSoftkeyFunction(pme_msg_textscreen_rsk, KEY_EVENT_UP);
}

/*****************************************************************************
 * FUNCTION: pme_msg_textscreen_shown
 * DESCRIPTION: show text screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_msg_textscreen_shown(void)
{
	pme_msg_textscreen_entry();
}

/*****************************************************************************
 * SCR - SUB LIST
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: dsm_apl_optionscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_optionscreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_list_optionscreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.opHili = hilight;
}

/*****************************************************************************
 * FUNCTION: dsm_apl_optionscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_optionscreen_exit(void)
{
     //screen out.
	 pme_ui_trace((MOD_MMI,"pme_list_optionscreen_exit()"));
}

/*****************************************************************************
 * FUNCTION: dsm_apl_popscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_optionscreen_del_msg(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_optionscreen_del_msg()"));
#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_SUB_OPTION,MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN, SCR_PME_SUB_OPTION);
		}
		mmi_frm_scrn_close_active_id();	
#else
		DeleteScreenIfPresent(SCR_PME_SUB_OPTION);
		GoBackHistory();
#endif
	pme_list_sub_option(DAPLN_MSG, DAPLO_DELETE);
}

/*****************************************************************************
 * FUNCTION: dsm_apl_optionscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_optionscreen_lsk(void)
{
	pme_list_option_en option = DAPLO_MAX;

	switch(s_pme_list_screen.opType){
		case DAPLN_NAV:
			if(s_pme_list_screen.opHili == 0){
				option = DAPLO_SELECT;
			}else if(s_pme_list_screen.opHili == 1){
				option = DAPLO_EXIT;
			}
			
			break;
		case DAPLN_MSG:
			if(s_pme_list_screen.opHili == 0){
				option = DAPLO_SELECT;
			}else if(s_pme_list_screen.opHili == 1){
				option = DAPLO_DELETE;
			}else if(s_pme_list_screen.opHili == 2){
				option = DAPLO_EXIT;
			}
			
			break;
		default:
			option = DAPLO_EXIT;
			
			break;
	}

	pme_ui_trace((MOD_MMI,"pme_list_optionscreen_lsk(), %d, %d", s_pme_list_screen.opType, option));
	//if(option == DAPLO_DELETE){
	//	pme_list_optionscreen_del_msg();
	//}else
	{
#ifdef __PME_USE_GPROUP__
		{
			MMI_ID group,scrn;
			mmi_frm_get_active_scrn_id(&group,&scrn);
			if(scrn == SCR_PME_SUB_OPTION)
			{
				mmi_frm_scrn_close_active_id();	
			}
		}
#else
		if(SCR_PME_SUB_OPTION == GetActiveScreenId()){
			gdi_layer_lock_frame_buffer();
			GoBackHistory();
			gdi_layer_unlock_frame_buffer();
		}
#endif
		if(s_pme_list_screen.opType == DAPLN_NAV && option == DAPLO_SELECT){
			s_pme_list_screen.subHili = 0;
		}
		pme_list_sub_option(s_pme_list_screen.opType, option);
#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_SUB_OPTION,MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN, SCR_PME_SUB_OPTION);
		}
#else
		DeleteScreenIfPresent(SCR_PME_SUB_OPTION);
#endif
	}
}

/*****************************************************************************
 * FUNCTION: dsm_apl_optionscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_optionscreen_rsk(void)
{
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: dsm_apl_optionscreen_entry
 * DESCRIPTION: start app option
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_optionscreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_list_optionscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_SUB_OPTION, 
	     pme_list_optionscreen_exit, 
	     (FuncPtr)pme_list_optionscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SUB_OPTION);	
#else
	EntryNewScreen(SCR_PME_SUB_OPTION, pme_list_optionscreen_exit, pme_list_optionscreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SUB_OPTION);
#endif
	RegisterHighlightHandler(pme_list_optionscreen_hilite);
	if(!guiBuffer){
		s_pme_list_screen.opHili = 0;
	}
	
	switch(s_pme_list_screen.opType){
		case DAPLN_NAV:
			menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_VIEW);
			menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_EXIT);
			
			break;
		case DAPLN_MSG:
			menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_VIEW);
			menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_DELETE);
			//menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_HELP);
			menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_EXIT);
			
			break;		
		default: 
			menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_EXIT);
			break;
	}
	
	ShowCategory353Screen((U8 *)GetString(STR_GLOBAL_OK), 0,
						  STR_GLOBAL_OK, IMG_GLOBAL_OK,
						  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
						  (S32)nNumofItems, 
						  (U8**)menuStringList,
						  (U16*)gIndexIconsImageList,
						  NULL, 0, 
						  (S32)s_pme_list_screen.opHili, 
						  guiBuffer);

	SetRightSoftkeyFunction(pme_list_optionscreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_list_optionscreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_list_optionscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_free_res
 * DESCRIPTION: free all the screen res
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_free_res(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_free_res()"));
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_exit(void)
{
     //screen out.
	wgui_restore_list_menu_slim_style();
	 pme_ui_trace((MOD_MMI,"pme_list_screen_exit()"));
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_lsk()"));
	s_pme_list_screen.opType = pme_list_sub_get_item_type(s_pme_list_screen.subHili);
	pme_list_sub_set_focut_msg(s_pme_list_screen.subHili);
	pme_list_optionscreen_entry();
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_rsk()"));
	pme_list_sub_screen_back();
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_ok(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_ok(), subHili: %d", s_pme_list_screen.subHili));
	s_pme_list_screen.opHili = 0;
	s_pme_list_screen.opType = pme_list_sub_get_item_type(s_pme_list_screen.subHili);
	pme_list_sub_set_focut_msg(s_pme_list_screen.subHili);
	pme_list_optionscreen_lsk();
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_del
 * DESCRIPTION: screen been  deleted
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static U8 pme_list_screen_del(void *data)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_del(), %d", data));
	pme_list_sub_exit();
	pme_list_screen_free_res();
	wgui_restore_list_menu_slim_style();
	return MMI_FALSE;
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_set_sk
 * DESCRIPTION: set soft key handle
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_set_sk(void)
{
	s_pme_list_screen.opType = pme_list_sub_get_item_type(s_pme_list_screen.subHili);
	pme_ui_trace((MOD_MMI,"pme_list_screen_set_sk(), subHili: %d, opType: %d", s_pme_list_screen.subHili, s_pme_list_screen.opType));

	if(pme_list_sub_is_first_page()){
		ChangeRightSoftkey(STR_GLOBAL_EXIT, IMG_GLOBAL_BACK);
	}else{
		ChangeRightSoftkey(STR_GLOBAL_BACK, IMG_GLOBAL_BACK);
	}
	SetRightSoftkeyFunction(pme_list_screen_rsk, KEY_EVENT_UP);
	switch(s_pme_list_screen.opType){
		case DAPLN_NAV:
			ChangeLeftSoftkey(STR_GLOBAL_OK, IMG_GLOBAL_OK);
			SetLeftSoftkeyFunction(pme_list_screen_ok, KEY_EVENT_UP);
			
			break;
		default: 
			ChangeLeftSoftkey(STR_GLOBAL_OPTIONS, IMG_GLOBAL_OK);
			SetLeftSoftkeyFunction(pme_list_screen_lsk, KEY_EVENT_UP);
			
			break;
	}
	
	SetKeyHandler(pme_list_screen_ok, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_list_screen_show(), hilight: %d", hilight));
	s_pme_list_screen.subHili = hilight;
	pme_list_screen_set_sk();
}

/*****************************************************************************
 * FUNCTION: pme_list_screen_get_list_icon
 * DESCRIPTION: 
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
uint16 pme_list_screen_get_list_icon(int32 isNew)
{
	if(isNew){
		return IMG_PME_LIST_MSG_NEW;
	}else{
		return IMG_PME_LIST_MSG_READ;
	}
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_entry
 * DESCRIPTION: start app list
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_list_screen_show(int32 refresh)
{
	U8 *guiBuffer;
	U8 *menuStringList[PME_SUB_SCREEN_ITEM_MAX + 1] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_list_screen_show(), refresh: %d", refresh));

	if(refresh){
		gdi_layer_lock_frame_buffer();
		UI_common_screen_pre_exit();
		UI_common_screen_exit();
		gdi_layer_unlock_frame_buffer();
	}
#ifdef __PME_USE_GPROUP__
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SUB_LIST);
#else
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SUB_LIST);
#endif
	RegisterHighlightHandler(pme_list_screen_hilite);
	
	nNumofItems = pme_list_sub_get_items(menuStringList, PME_SUB_SCREEN_ITEM_MAX + 1, hintDataPtrs, pme_list_icons_image_list);	
	wgui_override_list_menu_slim_style(WGUI_LIST_MENU_SLIM_STYLE_DRAW_ICON);
	ShowCategory353Screen((U8 *)GetString(STR_PME_SUBSCR), NULL,
						  STR_GLOBAL_OPTIONS, IMG_GLOBAL_OK,
						  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
						  (S32)nNumofItems, 
						  (U8**)menuStringList,
						  (U16*)pme_list_icons_image_list,
						  hintDataPtrs, 
						  0, 
						  (S32)s_pme_list_screen.subHili, 
						  guiBuffer);
	pme_list_screen_set_sk();
}

/*****************************************************************************
 * FUNCTION: dsm_apl_listscreen_entry
 * DESCRIPTION: start app list
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
#ifdef __PME_USE_GPROUP__
static mmi_ret pme_msg_screen_levave_SUB_LIST(mmi_event_struct *evt)
{
	mmi_ret ret = MMI_RET_OK;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
 	pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_SUB_LIST(),%d",evt->evt_id)); 
	switch (evt->evt_id)
	{
 		case EVT_ID_SCRN_DELETE_REQ_IN_END_KEY:
		case EVT_ID_SCRN_GOBACK_IN_END_KEY:
		case EVT_ID_SCRN_DELETE_REQ:
 			pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_SUB_LIST() 222 ")); 
			pme_list_screen_del(0);
			break;

		case EVT_ID_SCRN_DEINIT:
			break;
	}

	return ret;

};
#endif
static void pme_list_screen_entry(void)
{
	//wsl change pBOOL isback = IsBackHistory;
	pBOOL isback = 0;
	U8 *guiBuffer;

	pme_ui_trace((MOD_MMI,"pme_list_screen_entry()"));
	
#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_SUB_LIST, 
	     pme_list_screen_exit, 
	     (FuncPtr)pme_list_screen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	pme_ui_trace((MOD_MMI,"pme_msg_screen_levave_SUB_LIST set"));
	mmi_frm_scrn_set_leave_proc(GRP_ID_PME_MAIN, SCR_PME_SUB_LIST, pme_msg_screen_levave_SUB_LIST);
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SUB_LIST);	
#else
	EntryNewScreen(SCR_PME_SUB_LIST, pme_list_screen_exit, pme_list_screen_entry, NULL);
	SetDelScrnIDCallbackHandler(SCR_PME_SUB_LIST, pme_list_screen_del);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SUB_LIST);
#endif
	if(!guiBuffer)
	{
		s_pme_list_screen.subHili = 0;
	}

	if(pme_list_sub_init() || isback){
		pme_list_screen_show(FALSE);
	}
	else
	{
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
	}
}

/*****************************************************************************
 * FUNCTION: pme_list_screen_distroy
 * DESCRIPTION: distroy screen 
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
#ifdef __PME_USE_GPROUP__
void pme_list_screen_distroy(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_distroy(), %d, %d", mmi_frm_scrn_get_active_id(), SCR_PME_SUB_LIST));
	pme_list_sub_exit();
	pme_list_screen_free_res();
	if(mmi_frm_scrn_get_active_id() == SCR_PME_SUB_LIST)
	{
		mmi_frm_scrn_close_active_id();
	}
	else
	{
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN,SCR_PME_SUB_LIST,MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_group_close(GRP_ID_PME_MAIN);
		}
	}
}
#else
void pme_list_screen_distroy(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_distroy(), %d, %d", GetActiveScreenId(), SCR_PME_SUB_LIST));
	pme_list_sub_exit();
	pme_list_screen_free_res();
	if(GetActiveScreenId() == SCR_PME_SUB_LIST)
	{
		GoBackHistory();
	}
	else
	{
		if(IsScreenPresent(SCR_PME_SUB_LIST))
		{
			DeleteUptoScrID(SCR_PME_SUB_LIST);//del up SCR_DSM_APL_LIST history
			DeleteNHistory(1);//del SCR_DSM_APL_LIST
			GoBackHistory();
		}
	}
	pme_ui_trace((MOD_MMI,"pme_list_screen_distroy(), OUT"));
}
#endif
/*****************************************************************************
 * FUNCTION: pme_list_screen_refresh
 * DESCRIPTION: refresh screen 
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_list_screen_refresh(void)
{
	pme_ui_trace((MOD_MMI,"pme_list_screen_refresh()"));
	pme_list_screen_show(TRUE);
}

/*****************************************************************************
 * SCR - STORAGE SETTING
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_set_storagescreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_storagescreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_set_storagescreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.setSubHili = hilight;
}

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_storagescreen_exit(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_storagescreen_exit()"));
     //screen out.
}

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_storagescreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_storagescreen_lsk(), opHili: %d", s_pme_list_screen.setSubHili));
	if(s_pme_list_screen.setSubHili >= 0 && s_pme_list_screen.setSubHili < PME_MAX_MODE)
	{
		pme_set_storage('B' + s_pme_list_screen.setSubHili);
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE), IMG_GLOBAL_ACTIVATED, 1, ST_NOTIFYDURATION, SUCCESS_TONE);

#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_SETTING_STORAGE, MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN, SCR_PME_SETTING_STORAGE);
		}
#else
		DeleteScreenIfPresent(SCR_PME_SETTING_STORAGE);
#endif
		/*切换盘符，重新刷新列表*/
		pme_ui_update_list_screen();
		pme_list_storage_space_check();
	}
}

/*****************************************************************************
 * FUNCTION: pme_set_storagescreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_storagescreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_storagescreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_set_storagescreen_entry
 * DESCRIPTION: mod setting screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_storagescreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_set_storagescreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_SETTING_STORAGE, 
	     pme_set_storagescreen_exit, 
	     (FuncPtr)pme_set_storagescreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SETTING_STORAGE);	
#else
	EntryNewScreen(SCR_PME_SETTING_STORAGE, pme_set_storagescreen_exit, pme_set_storagescreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SETTING_STORAGE);
#endif
	RegisterHighlightHandler(pme_set_storagescreen_hilite);
	//if(!guiBuffer)
	{
		s_pme_list_screen.setSubHili = ( pme_get_storage() == 'B' ? 0 : 1);
	}

	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_STORAGE_PHONE);
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_STORAGE_TCARD);

	ShowCategory109Screen(STR_PME_STORAGE_SET, 0,
							  STR_GLOBAL_OK, IMG_GLOBAL_OK, 
							  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							  (S32)nNumofItems, (U8**)menuStringList, NULL,
							  (S32)s_pme_list_screen.setSubHili, guiBuffer); 

	SetRightSoftkeyFunction(pme_set_storagescreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_set_storagescreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_set_storagescreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * SCR - RING SETTING
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_ringscreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_set_ringscreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.setSubHili = hilight;
}

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_ringscreen_exit(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_ringscreen_exit()"));
     //screen out.
}

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_ringscreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_ringscreen_lsk(), opHili: %d", s_pme_list_screen.setSubHili));
	if(s_pme_list_screen.setSubHili >= 0 && s_pme_list_screen.setSubHili < PME_MAX_MODE){
		pme_set_ring(s_pme_list_screen.setSubHili);
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE), IMG_GLOBAL_ACTIVATED, 1, ST_NOTIFYDURATION, SUCCESS_TONE);
#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_SETTING_RING, MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN, SCR_PME_SETTING_RING);
		}
#else
		DeleteScreenIfPresent(SCR_PME_SETTING_RING);
#endif
	
	}
}

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_ringscreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_ringscreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_set_ringscreen_entry
 * DESCRIPTION: mod setting screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_ringscreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_set_modscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_SETTING_RING, 
	     pme_set_ringscreen_exit, 
	     (FuncPtr)pme_set_ringscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SETTING_RING);	
#else
	EntryNewScreen(SCR_PME_SETTING_RING, pme_set_ringscreen_exit, pme_set_ringscreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SETTING_RING);
#endif
	RegisterHighlightHandler(pme_set_ringscreen_hilite);
	//if(!guiBuffer)
	{
		int32 mod = pme_get_ring();
		s_pme_list_screen.setSubHili = mod >= 0 ? mod : 0;
	}

	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_RING_CLOSE);
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_RING_OPEN);

	ShowCategory109Screen(STR_PME_RING_SET, 0,
							  STR_GLOBAL_OK, IMG_GLOBAL_OK, 
							  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							  (S32)nNumofItems, (U8**)menuStringList, NULL,
							  (S32)s_pme_list_screen.setSubHili, guiBuffer); 

	SetRightSoftkeyFunction(pme_set_ringscreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_set_ringscreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_set_ringscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * SCR - MOD SETTING
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_set_modscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_modscreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_set_modscreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.setSubHili = hilight;
}

/*****************************************************************************
 * FUNCTION: pme_set_modscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_modscreen_exit(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_modscreen_exit()"));
     //screen out.
}

/*****************************************************************************
 * FUNCTION: pme_set_modscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_modscreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_modscreen_lsk(), opHili: %d", s_pme_list_screen.setSubHili));
	if(s_pme_list_screen.setSubHili >= 0 && s_pme_list_screen.setSubHili < PME_MAX_MODE){
		pme_set_mode(s_pme_list_screen.setSubHili);
		DisplayPopup((PU8) GetString(STR_GLOBAL_DONE), IMG_GLOBAL_ACTIVATED, 1, ST_NOTIFYDURATION, SUCCESS_TONE);
#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_SETTING_MOD, MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN, SCR_PME_SETTING_MOD);
		}
#else
		DeleteScreenIfPresent(SCR_PME_SETTING_MOD);
#endif
	}
}

/*****************************************************************************
 * FUNCTION: pme_set_modscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_modscreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_set_modscreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_set_modscreen_entry
 * DESCRIPTION: mod setting screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_set_modscreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_set_modscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_SETTING_MOD, 
	     pme_set_modscreen_exit, 
	     (FuncPtr)pme_set_modscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_SETTING_MOD);	
#else
	EntryNewScreen(SCR_PME_SETTING_MOD, pme_set_modscreen_exit, pme_set_modscreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_SETTING_MOD);
#endif
	RegisterHighlightHandler(pme_set_modscreen_hilite);
	//if(!guiBuffer)
	{
		int32 mod = pme_get_mode();
		s_pme_list_screen.setSubHili = mod >= 0 ? mod : PME_CLOSE;
	}

	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_MOD_NORMAL);
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_MOD_URGENT);
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_MOD_SLEEP);
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_MOD_CLOSED);

	ShowCategory109Screen(STR_PME_MOD_SET, 0,
							  STR_GLOBAL_OK, IMG_GLOBAL_OK, 
							  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							  (S32)nNumofItems, (U8**)menuStringList, NULL,
							  (S32)s_pme_list_screen.setSubHili, guiBuffer); 

	SetRightSoftkeyFunction(pme_set_modscreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_set_modscreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_set_modscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * SCR -SETTINGS
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_option_settingscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_option_settingscreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_option_settingscreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.setHili = hilight;
}

/*****************************************************************************
 * FUNCTION: pme_option_settingscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_option_settingscreen_exit(void)
{
	pme_ui_trace((MOD_MMI,"pme_option_settingscreen_exit()"));
     //screen out.
}

/*****************************************************************************
 * FUNCTION: pme_option_settingscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_option_settingscreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_option_settingscreen_lsk(), opHili: %d", s_pme_list_screen.setHili));
	
	if(s_pme_list_screen.setHili == 0){//mod
		pme_set_modscreen_entry();
	}else if(s_pme_list_screen.setHili == 1){//ring
		pme_set_ringscreen_entry();
	}else if(s_pme_list_screen.setHili == 2){//storage
		pme_set_storagescreen_entry();
	}
}

/*****************************************************************************
 * FUNCTION: pme_main_optionscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_option_settingscreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_option_settingscreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_option_settingscreen_init_hint
 * DESCRIPTION: set hint string
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_option_settingscreen_init_hint(void)
{
	int32 set;
	U16 strId;
	int32 i;

    memset(hintData, 0, MAX_SUB_MENU_HINT_SIZE * 3);
	for(i = 0; i < 3; i++){
		hintDataPtrs[i] = hintData[i];
	}
	i = 0;

	set = pme_get_mode();
	switch(set){
		case PME_NORMAL_MODE:
			strId = STR_PME_MOD_NORMAL;
			break;
		case PME_EMERGENCY_MODE:
			strId = STR_PME_MOD_URGENT;
			break;
		case PME_SLEEP_MODE:
			strId = STR_PME_MOD_SLEEP;
			break;
		default:
		case PME_CLOSE:
			strId = STR_PME_MOD_CLOSED;
			break;
			
	}
    mmi_ucs2cpy((S8*)hintData[i++], (const S8*)GetString(strId));

	set = pme_get_ring();
	if(!set){
		strId = STR_PME_RING_CLOSE;
	}else{
		strId = STR_PME_RING_OPEN;
	}
    mmi_ucs2cpy((S8*)hintData[i++], (const S8*)GetString(strId));

	set = pme_get_storage();
	if(set == 'B'){
		strId = STR_PME_STORAGE_PHONE;
	}else{
		strId = STR_PME_STORAGE_TCARD;
	}
    mmi_ucs2cpy((S8*)hintData[i++], (const S8*)GetString(strId));
}

/*****************************************************************************
 * FUNCTION: pme_option_settingscreen_entry
 * DESCRIPTION: start app main settings option
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_option_settingscreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_option_settingscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_OPTION_SETTING, 
	     pme_option_settingscreen_exit, 
	     (FuncPtr)pme_option_settingscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_OPTION_SETTING);	
#else
	EntryNewScreen(SCR_PME_OPTION_SETTING, pme_option_settingscreen_exit, pme_option_settingscreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_OPTION_SETTING);
#endif
	RegisterHighlightHandler(pme_option_settingscreen_hilite);
	if(!guiBuffer){
		s_pme_list_screen.setHili = 0;
	}
	
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_MOD_SET);
	menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_RING_SET);
	if(pme_get_storage_num() > 1){
		menuStringList[nNumofItems++] = (U8*)get_string(STR_PME_STORAGE_SET);
	}

	pme_option_settingscreen_init_hint();
	ShowCategory353Screen((U8 *)GetString(STR_PME_SETTING), 0,
						  STR_GLOBAL_OK, IMG_GLOBAL_OK,
						  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
						  (S32)nNumofItems, 
						  (U8**)menuStringList,
						  (U16*)gIndexIconsImageList,
						  (U8**)hintDataPtrs, 
						  0, 
						  (S32)s_pme_list_screen.setHili, 
						  guiBuffer);

	SetRightSoftkeyFunction(pme_option_settingscreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_option_settingscreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_option_settingscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * SCR - MAIN OPTION
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_delacc_popscreen_exit
 * DESCRIPTION: exit the pop screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_delacc_popscreen_exit(void)
{
	//screen out.
	pme_ui_trace((MOD_MMI,"pme_delacc_popscreen_exit()"));
}

/*****************************************************************************
 * FUNCTION: dsm_apl_popscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_delacc_popscreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_delacc_popscreen_lsk()"));
#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_MAIN_OPTION, MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN,SCR_PME_MAIN_OPTION);
		}
		mmi_frm_scrn_close_active_id();	
#else
		DeleteScreenIfPresent(SCR_PME_MAIN_OPTION);
		GoBackHistory();
#endif
	/*del the accont*/
	pme_list_main_delete_account();
}

/*****************************************************************************
 * FUNCTION: pme_delacc_popscreen_entry
 * DESCRIPTION: show prompt screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_delacc_popscreen_entry(void)
{
	U8* gui_buffer; 
	pme_ui_trace((MOD_MMI,"pme_delacc_popscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_POP_DELACC, 
	     pme_delacc_popscreen_exit, 
	     (FuncPtr)pme_delacc_popscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	gui_buffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_POP_DELACC);	
#else
	EntryNewScreen(SCR_PME_POP_DELACC, pme_delacc_popscreen_exit, pme_delacc_popscreen_entry, NULL);
	gui_buffer = GetCurrGuiBuffer(SCR_PME_POP_DELACC);
#endif
	ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK,
							STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							(UI_string_type)get_string(STR_GLOBAL_DELETE),//temp.
							IMG_GLOBAL_QUESTION,
							gui_buffer);
	SetLeftSoftkeyFunction(pme_delacc_popscreen_lsk, KEY_EVENT_UP);
#ifdef __PME_USE_GPROUP__
	SetRightSoftkeyFunction(mmi_frm_scrn_close_active_id, KEY_EVENT_UP);
#else
	SetRightSoftkeyFunction(GoBackHistory, KEY_EVENT_UP);
#endif
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_free_res
 * DESCRIPTION: free all the screen res
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_free_res(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_screen_free_res()"));
}

/*****************************************************************************
 * FUNCTION: pme_main_optionscreen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_optionscreen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_main_optionscreen_hilite(), hilight: %d", hilight));
	s_pme_list_screen.opHili = hilight;
}

/*****************************************************************************
 * FUNCTION: pme_main_optionscreen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_optionscreen_exit(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_optionscreen_exit()"));
     //screen out.
}

/*****************************************************************************
 * FUNCTION: pme_main_optionscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
 #ifdef __PME_USE_GPROUP__
static void pme_main_optionscreen_lsk(void)
{
	int8 select = 0;
	int8 del = 1;
	int8 setting = 2;
	pme_ui_trace((MOD_MMI,"pme_main_optionscreen_lsk(), opHili: %d", s_pme_list_screen.opHili));

	if(pme_list_main_get_items(NULL, 0, NULL, NULL) == 0){
		select = -1;
		del = -1;
		setting = 0;
	}
	
	if(s_pme_list_screen.opHili == del){//del
		pme_delacc_popscreen_entry();
	}
	else
	{
		/*
		if(SCR_PME_MAIN_OPTION == mmi_frm_scrn_get_active_id())
		{
			gdi_layer_lock_frame_buffer();
			mmi_frm_scrn_close_active_id();
			gdi_layer_unlock_frame_buffer();
		}*/
		
		if(s_pme_list_screen.opHili == select){//select
			pme_list_screen_entry();
		}else if(s_pme_list_screen.opHili == setting){//setting
			pme_option_settingscreen_entry();
		}
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_MAIN_OPTION, MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN,SCR_PME_MAIN_OPTION);
		}
	}
}
#else

static void pme_main_optionscreen_lsk(void)
{
	int8 select = 0;
	int8 del = 1;
	int8 setting = 2;
	
	pme_ui_trace((MOD_MMI,"pme_main_optionscreen_lsk(), opHili: %d", s_pme_list_screen.opHili));

	if(pme_list_main_get_items(NULL, 0, NULL, NULL) == 0){
		select = -1;
		del = -1;
		setting = 0;
	}
	
	if(s_pme_list_screen.opHili == del){//del
		pme_delacc_popscreen_entry();
	}else{
		if(SCR_PME_MAIN_OPTION == GetActiveScreenId()){
			gdi_layer_lock_frame_buffer();
			GoBackHistory();
			gdi_layer_unlock_frame_buffer();
		}
		
		if(s_pme_list_screen.opHili == select){//select
			pme_list_screen_entry();
		}else if(s_pme_list_screen.opHili == setting){//setting
			pme_option_settingscreen_entry();
		}
		DeleteScreenIfPresent(SCR_PME_MAIN_OPTION);
	}
}
#endif

/*****************************************************************************
 * FUNCTION: pme_main_optionscreen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_optionscreen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_optionscreen_rsk()"));
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

/*****************************************************************************
 * FUNCTION: pme_main_optionscreen_entry
 * DESCRIPTION: start app main screen option
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_optionscreen_entry(void)
{
	U8 *guiBuffer;
	U8 *menuStringList[10] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_main_optionscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_MAIN_OPTION, 
	     pme_main_optionscreen_exit, 
	     (FuncPtr)pme_main_optionscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_MAIN_OPTION);	
#else
	EntryNewScreen(SCR_PME_MAIN_OPTION, pme_main_optionscreen_exit, pme_main_optionscreen_entry, NULL);
	guiBuffer = GetCurrGuiBuffer(SCR_PME_MAIN_OPTION);
#endif
	RegisterHighlightHandler(pme_main_optionscreen_hilite);
	if(!guiBuffer){
		s_pme_list_screen.opHili = 0;
	}

	if(pme_list_main_get_items(NULL, 0, NULL, NULL) > 0){
		menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_SELECT);
		menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_DELETE);
	}
	menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_SETTINGS);
	//menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_HELP);
	//menuStringList[nNumofItems++] = (U8*)get_string(STR_GLOBAL_EXIT);

	ShowCategory353Screen((U8 *)GetString(STR_GLOBAL_OK), 0,
						  STR_GLOBAL_OK, IMG_GLOBAL_OK,
						  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
						  (S32)nNumofItems, 
						  (U8**)menuStringList,
						  (U16*)gIndexIconsImageList,
						  NULL, 0, 
						  (S32)s_pme_list_screen.opHili, 
						  guiBuffer);

	SetRightSoftkeyFunction(pme_main_optionscreen_rsk, KEY_EVENT_UP);
	SetLeftSoftkeyFunction(pme_main_optionscreen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_main_optionscreen_lsk, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * SCR - MAIN LIST
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_main_screen_exit
 * DESCRIPTION: exit the list screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_exit(void)
{
	//screen out.
	wgui_restore_list_menu_slim_style();
	pme_ui_trace((MOD_MMI,"pme_main_screen_exit()"));
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_screen_lsk()"));
	pme_list_main_set_focut_account(s_pme_list_screen.mainHili);
	pme_main_optionscreen_entry();
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_rsk
 * DESCRIPTION: right soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_rsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_screen_rsk()"));
	pme_main_screen_distroy();
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_ok(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_screen_ok()"));
	pme_list_main_set_focut_account(s_pme_list_screen.mainHili);
	s_pme_list_screen.opHili = 0;
	pme_main_optionscreen_lsk();
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_del
 * DESCRIPTION: screen been  deleted
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static U8 pme_main_screen_del(void *data)
{
	pme_ui_trace((MOD_MMI,"pme_main_screen_del(), %d", data));
	pme_list_main_exit();
	pme_main_screen_free_res();
	wgui_restore_list_menu_slim_style();
	return MMI_FALSE;
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_set_sk
 * DESCRIPTION: set soft key handle
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_set_sk(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_screen_set_sk()"));
	ChangeRightSoftkey(STR_GLOBAL_EXIT, IMG_GLOBAL_BACK);
	SetRightSoftkeyFunction(pme_main_screen_rsk, KEY_EVENT_UP);
	ChangeLeftSoftkey(STR_GLOBAL_OPTIONS, IMG_GLOBAL_OK);
	SetLeftSoftkeyFunction(pme_main_screen_lsk, KEY_EVENT_UP);
	SetKeyHandler(pme_main_screen_ok, KEY_ENTER, KEY_EVENT_UP);
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_hilite
 * DESCRIPTION: hilite changed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_hilite(S32 hilight)
{     
	pme_ui_trace((MOD_MMI,"pme_main_screen_hilite(): hilight: %d", hilight));
	s_pme_list_screen.mainHili = hilight;
	pme_main_screen_set_sk();
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_get_list_icon
 * DESCRIPTION: 
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
uint16 pme_main_screen_get_list_icon(int32 haveNew)
{
	if(haveNew){
		return IMG_PME_LIST_ACNT_NEW;
	}else{
		return IMG_PME_LIST_ACNT_READ;
	}
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_entry
 * DESCRIPTION: show app main screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_main_screen_show(int32 refresh)
{
	U8 *guiBuffer;
	U8 *menuStringList[PME_MAIN_SCREEN_ITEM_MAX + 1] = {0};
	U16 nNumofItems = 0;
	pme_ui_trace((MOD_MMI,"pme_main_screen_show(): refresh: %d", refresh));

	if(refresh){
		gdi_layer_lock_frame_buffer();
		UI_common_screen_pre_exit();
		UI_common_screen_exit();
		gdi_layer_unlock_frame_buffer();
	}
#ifdef __PME_USE_GPROUP__
	guiBuffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_MAIN_SCREEN);
#else
	guiBuffer = GetCurrGuiBuffer(SCR_PME_MAIN_SCREEN);
#endif
	RegisterHighlightHandler(pme_main_screen_hilite);
	
	nNumofItems = pme_list_main_get_items(menuStringList, PME_MAIN_SCREEN_ITEM_MAX + 1, hintDataPtrs, pme_list_icons_image_list);
	wgui_override_list_menu_slim_style(WGUI_LIST_MENU_SLIM_STYLE_DRAW_ICON);
	ShowCategory353Screen((U8 *)GetString(STR_PME_MAINSCR), 0,
						  STR_GLOBAL_OPTIONS, IMG_GLOBAL_OK,
						  STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
						  (S32)nNumofItems, 
						  (U8**)menuStringList,
						  (U16*)pme_list_icons_image_list,
						  (U8**)hintDataPtrs, 
						  0, 
						  (S32)s_pme_list_screen.mainHili, 
						  guiBuffer);
	pme_main_screen_set_sk();
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_entry
 * DESCRIPTION: start app main screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
#ifdef __PME_USE_GPROUP__
static mmi_ret pme_msg_screen_levave_MAIN_SCREEN(mmi_event_struct *evt)
{
	mmi_ret ret = MMI_RET_OK;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
 	pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_MAIN_SCREEN(),%d",evt->evt_id)); 
	switch (evt->evt_id)
	{
		case EVT_ID_SCRN_DELETE_REQ_IN_END_KEY:
		case EVT_ID_SCRN_DELETE_REQ:
 		case EVT_ID_SCRN_GOBACK:
 		case EVT_ID_SCRN_GOBACK_IN_END_KEY:
			pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_MAIN_SCREEN() 222 ")); 
			pme_main_screen_del(0);
			break;

		case EVT_ID_SCRN_DEINIT:
			break;
	}

 	pme_ui_trace((MOD_MMI, "pme_msg_screen_levave_MAIN_SCREEN() 222,%d",evt->evt_id)); 
	return ret;

};
#endif
static void pme_main_screen_entry(void)
{
	int32 time, time0 = mr_getTime();
	int32 logo = FALSE;
	pBOOL isback = 0;
	int32 init = 0;
	//pme_ui_trace((MOD_MMI,"pme_main_screen_entry(): isback: %d", IsBackHistory));
	//wsl add 先检查空间.
	init = pme_list_main_init();
	if(pme_list_storage_space_check())
	{
		return ;
	}
	//end wsl
#ifdef __PME_USE_GPROUP__
    		
	logo = (mmi_frm_scrn_get_active_id() == SCR_PME_ENTRY_LOGO);

	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_MAIN_SCREEN, 
	     pme_main_screen_exit, 
	     (FuncPtr)pme_main_screen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	pme_ui_trace((MOD_MMI,"pme_msg_screen_levave_MAIN_SCREEN set"));
	mmi_frm_scrn_set_leave_proc(GRP_ID_PME_MAIN, SCR_PME_MAIN_SCREEN, pme_msg_screen_levave_MAIN_SCREEN);
	
        if (mmi_frm_scrn_is_present(GRP_ID_PME_MAIN,SCR_PME_ENTRY_LOGO,MMI_FRM_NODE_ALL_FLAG))
        {
		mmi_frm_scrn_close(GRP_ID_PME_MAIN,SCR_PME_ENTRY_LOGO);
        }
        
#else
	logo = (GetActiveScreenId() == SCR_PME_ENTRY_LOGO);
	EntryNewScreen(SCR_PME_MAIN_SCREEN, pme_main_screen_exit, pme_main_screen_entry, NULL);
	SetDelScrnIDCallbackHandler(SCR_PME_MAIN_SCREEN, pme_main_screen_del);
	/*del SCR_PME_ENTRY_LOGO*/
	gdi_layer_lock_frame_buffer();
	if(IsScreenPresent(SCR_PME_ENTRY_LOGO)){
		DeleteNHistory(1);
	}
	gdi_layer_unlock_frame_buffer();
#endif	
	if(init || isback){
//	if(pme_list_main_init() || isback){
		if(logo){
			time = mr_getTime() - time0;
			while(time < PME_MAIN_LOAD_TIME){
				kal_sleep_task(10);//50ms
				time = mr_getTime() - time0;
			}
		}
		pme_main_screen_show(FALSE);
		//wsl del
		/*
		if(!isback)
			pme_list_storage_space_check();
			*/
	}else{
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
	}
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_distroy
 * DESCRIPTION: distroy screen 
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
void pme_main_screen_distroy(void)
{
	//if(!IsScreenPresent(SCR_DSM_APL_APPS)){
		pme_list_main_exit();
	//}

	pme_main_screen_free_res();
#ifdef __PME_USE_GPROUP__
	if(mmi_frm_scrn_get_active_id() == SCR_PME_MAIN_SCREEN)
	{
		mmi_frm_scrn_close_active_id();	
	}
#else
	if(GetActiveScreenId() == SCR_PME_MAIN_SCREEN)
	{
		GoBackHistory();
	}
#endif
	else
	{
#ifdef __PME_USE_GPROUP__
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN,SCR_PME_MAIN_SCREEN,MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_group_close(GRP_ID_PME_MAIN);
		}
#else
		if(IsScreenPresent(SCR_PME_MAIN_SCREEN))
		{
			DeleteUptoScrID(SCR_PME_MAIN_SCREEN);//del up SCR_PME_MAIN_SCREEN history
			DeleteNHistory(1);//del SCR_PME_MAIN_SCREEN
			GoBackHistory();
		}
#endif
	}
}

/*****************************************************************************
 * FUNCTION: pme_main_screen_refresh
 * DESCRIPTION: refresh screen 
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_main_screen_refresh(void)
{
	pme_main_screen_show(TRUE);
}

/*****************************************************************************
 * SCR - MAIN OPTION
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_new_popscreen_distroy
 * DESCRIPTION: distroy screen 
 * PARAMETERS: 
 * RETURNS: void
 *****************************************************************************/
#ifdef __PME_USE_GPROUP__
static void pme_new_popscreen_distroy(void)
{
	if(mmi_frm_scrn_get_active_id() == SCR_PME_POP_NEWMSG)
	{
		mmi_frm_scrn_close_active_id();	
	}
	else
	{
		if(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN, SCR_PME_POP_NEWMSG, MMI_FRM_NODE_ALL_FLAG))
		{
			mmi_frm_scrn_close(GRP_ID_PME_MAIN,SCR_PME_POP_NEWMSG);
		}
	}
}
#else
static void pme_new_popscreen_distroy(void)
{
	if(GetActiveScreenId() == SCR_PME_POP_NEWMSG)
	{
		GoBackHistory();
	}
	else
	{
		DeleteScreenIfPresent(SCR_PME_POP_NEWMSG);
	}
}
#endif
/*****************************************************************************
 * FUNCTION: pme_delacc_popscreen_exit
 * DESCRIPTION: exit the pop screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_new_popscreen_exit(void)
{
	//screen out.
	pme_ui_trace((MOD_MMI,"pme_new_popscreen_exit()"));
}

/*****************************************************************************
 * FUNCTION: dsm_apl_popscreen_lsk
 * DESCRIPTION: left soft key pressed
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_new_popscreen_lsk(void)
{
	pme_ui_trace((MOD_MMI,"pme_new_popscreen_lsk()"));
	
	pme_clear_idle_msg_ind();
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
	/*open the new list*/
	pme_main_entry();//temp.
}

/*****************************************************************************
 * FUNCTION: pme_delacc_popscreen_entry
 * DESCRIPTION: show prompt screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_new_popscreen_entry(void)
{
	U8* gui_buffer; 
	pme_ui_trace((MOD_MMI,"pme_new_popscreen_entry()"));

#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_POP_NEWMSG, 
	     pme_new_popscreen_exit, 
	     (FuncPtr)pme_new_popscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	gui_buffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_POP_NEWMSG);	
#else
	EntryNewScreen(SCR_PME_POP_NEWMSG, pme_new_popscreen_exit, pme_new_popscreen_entry, NULL);
	gui_buffer = GetCurrGuiBuffer(SCR_PME_POP_NEWMSG);
#endif
#ifdef WIN32
	ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK,
							STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							(UI_string_type)get_string(STR_PME_NEWMSG),							
							IMG_GLOBAL_ACTIVATED,							
							gui_buffer);
#else
	ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK,
							STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							(UI_string_type)get_string(STR_PME_NEWMSG),
							#if (defined(__IS_10A__) || defined(__IS_11A__) )
							IMG_GLOBAL_ACTIVATED,
							#else
							IMG_NEW_MESSAGE_NOTIFICATION_MSG,
							#endif
							gui_buffer);
#endif
	SetLeftSoftkeyFunction(pme_new_popscreen_lsk, KEY_EVENT_UP);
#ifdef __PME_USE_GPROUP__
	SetRightSoftkeyFunction(mmi_frm_scrn_close_active_id, KEY_EVENT_UP);
#else
	SetRightSoftkeyFunction(GoBackHistory, KEY_EVENT_UP);
#endif
}

/*****************************************************************************
 * SCR - MAIN OPTION
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_screen_logo
 * DESCRIPTION: logo
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_screen_logo(void)
{
	S32 x, y;
	S32 w = 0;
	S32 h = 0;
#if 0	
	EntryNewScreen(SCR_PME_ENTRY_LOGO, NULL, NULL, NULL);
	/*draw logo*/
	gdi_image_get_dimension((U8*)DSM_LOGO_BMP, &w, &h);
	x = (LCD_WIDTH - w)/2;
	y = (LCD_HEIGHT - h)/2;
	gdi_layer_clear(GDI_COLOR_WHITE);
	gdi_image_draw(x, y, (U8*)DSM_LOGO_BMP);
	gdi_layer_blt_previous(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
#endif
}

/*****************************************************************************
 * FUNCTION: pme_main_highlight_handler
 * DESCRIPTION: menu highlight handler
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_main_highlight_handler(void)
{
	pme_ui_trace((MOD_MMI,"pme_main_highlight_handler()"));
    SetLeftSoftkeyFunction(pme_main_entry, KEY_EVENT_UP);
	SetKeyHandler(pme_main_entry, KEY_ENTER, KEY_EVENT_UP);
#ifdef __MMI_TOUCH_SCREEN__
    wgui_reset_list_item_selected_callback();
#endif
} 

/*****************************************************************************
 * FUNCTION: pme_ui_play_tone
 * DESCRIPTION: 播放提示音
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
static void pme_ui_play_tone(void)
{
	pme_ui_trace((MOD_MMI,"pme_ui_play_tone(), %d", mdi_audio_is_idle()));


#if defined( __IS_10A__)||defined(__IS_11A__)
	if(MMI_FALSE == srv_backlight_is_on(0))
	{
		srv_backlight_turn_on(1);
	}
#else
	if(IsBacklightOn()==MMI_FALSE){
		TurnOnBacklight(1);	
	}
#endif

#if defined( __IS_10A__)||defined(__IS_11A__)
	if(!srv_prof_if_can_ring())
		return;
#else
	if(!mmi_profiles_check_set_mute()){
		return;
	}
#endif

	if(!mdi_audio_is_idle()){
		return;
	}
 
#if defined( __IS_10A__)||defined(__IS_11A__)
	srv_prof_play_tone(SRV_PROF_TONE_SMS, NULL);
#else
	playRequestedTone(SMS_TONE);
#endif
}

/*****************************************************************************
 * FUNCTION: pme_ui_set_status_icon
 * DESCRIPTION: 开关状态栏icon
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_ui_set_status_icon(int8 type, int32 open)
{
	int32 icon_id;
	
	pme_ui_trace((MOD_MMI, "pme_ui_set_status_icon(), %d", open));
	switch(type){
		case PME_STYPE_EMAIL:
			icon_id = STATUS_ICON_PME_EMAIL;
			#ifdef __IS_11A__
			vapp_mythraod_goto_pme_msg_email(open,IMG_PME_STATUS_ICON_EMAIL,STR_PME_NEWMSG);
			#endif
			break;
		case PME_STYPE_IM:
			icon_id = STATUS_ICON_PME_IM;
			#ifdef __IS_11A__
			vapp_mythraod_goto_pme_msg_im(open,IMG_PME_STATUS_ICON_IM,STR_PME_NEWMSG);
			#endif
			break;
		default:
			#ifdef __IS_11A__
			vapp_mythraod_goto_pme_msg_system(open,IMG_PME_STATUS_ICON_SYS,STR_PME_NEWMSG);
			#endif
			icon_id = STATUS_ICON_PME_SYS;
			break;
	}

	if(open){
		BlinkStatusIcon(icon_id);
	}else{
		HideStatusIcon(icon_id);
	}
	
	UpdateStatusIcons();
}

/*****************************************************************************
 * FUNCTION: pme_ui_update_list_screen
 * DESCRIPTION: 刷新列表
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_ui_update_list_screen(void)
{
	if(GetActiveScreenId() == SCR_PME_MAIN_SCREEN){
		pme_list_main_init();
		pme_main_screen_show(TRUE);
	}else if(GetActiveScreenId() == SCR_PME_SUB_LIST){
		pme_list_sub_init();
		pme_list_screen_show(TRUE);
	}
}

/*****************************************************************************
 * FUNCTION: pme_ui_is_list_screen_open
 * DESCRIPTION: 列表是否开启
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
int32 pme_ui_is_list_screen_open(void)
{
#ifdef __PME_USE_GPROUP__
	if( (mmi_frm_scrn_get_active_id()== SCR_PME_MAIN_SCREEN)
		||(mmi_frm_scrn_is_present(GRP_ID_PME_MAIN,SCR_PME_MAIN_SCREEN,MMI_FRM_NODE_ALL_FLAG)))
	{
		return TRUE;
	}
#else
	if(GetActiveScreenId() == SCR_PME_MAIN_SCREEN
		|| IsScreenPresent(SCR_PME_MAIN_SCREEN)
		){
		return TRUE;
	}
#endif
	return FALSE;
}

/*****************************************************************************
 * FUNCTION: pme_ui_new_msg_notify
 * DESCRIPTION: a new msg comming
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_ui_new_msg_notify(int8 type, char* sender)
{
	pme_ui_trace((MOD_MMI,"pme_ui_new_msg_notify()"));

	
	if(pme_ui_is_list_screen_open()){
		/*更新列表*/
		pme_ui_update_list_screen();
	}else{
		/*显示状态图标*/
		//pme_ui_set_status_icon(type, TRUE);

#ifdef PME_NEWS_POP_ANYWHERE
			
#else
		//pme_ui_trace((MOD_MMI,"IsOnIdleScreen, %d", g_idle_context.IsOnIdleScreen));
		//if(g_idle_context.IsOnIdleScreen){//idle上，可以弹出pop提示框
			pme_new_popscreen_distroy();//temp.
			pme_new_popscreen_entry();
		//}
#endif
	}

	/*铃声提示*/
	if(pme_get_ring()){
		pme_ui_play_tone();
	}
	
}

/*****************************************************************************
 * FUNCTION: pme_ui_intialize
 * DESCRIPTION: menu 开机初始化
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_ui_intialize(void)
{
       extern void mr_pal_register_status_icon_pen_event_hdlr(int16 icon_id, void (* f)(void));
	mr_pal_register_status_icon_pen_event_hdlr(STATUS_ICON_PME_SYS,pme_main_entry);
	mr_pal_register_status_icon_pen_event_hdlr(STATUS_ICON_PME_EMAIL,pme_main_entry);
	mr_pal_register_status_icon_pen_event_hdlr(STATUS_ICON_PME_IM,pme_main_entry);
	SetHiliteHandler(MENU_PME_MAIN, pme_main_highlight_handler);
}

/*****************************************************************************
 * GLOBAL FUNCTION for MTK MODules
 * 
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION: pme_main_entry
 * DESCRIPTION: enter main screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_main_entry(void)
{
	if(!pme_port_storage_is_ready()){
		return;
	}
	memset(&s_pme_list_screen, 0, sizeof(s_pme_list_screen));
	pme_screen_logo();
	pme_list_init(PME_MAIN_SCREEN_ITEM_MAX, PME_SUB_SCREEN_ITEM_MAX);
	pme_main_screen_entry();
}

/*****************************************************************************
 * FUNCTION: pme_main_setting
 * DESCRIPTION: enter setting screen
 * PARAMETERS:
 * RETURNS: void
 *****************************************************************************/
void pme_main_setting(void)
{
	pme_option_settingscreen_entry();
}

/*void pme_goback_idle_msg_ind(void)
{
	pme_clear_idle_msg_ind();
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
}

static void pme_idle_new_msg_power_key_handle(void )
{
	FuncPtr endKeyFunc =NULL;
	pme_clear_idle_msg_ind();
	PowerAndEndKeyHandler();
	endKeyFunc = GetKeyHandler(KEY_END, KEY_EVENT_DOWN);
	if(endKeyFunc)
		endKeyFunc();
	else
	{
#ifdef __PME_USE_GPROUP__
		mmi_frm_scrn_close_active_id();	
#else
		GoBackHistory();
#endif
	}
}
*/
void pme_show_new_msg_ind(void)
{
	U8* gui_buffer;
#ifdef __PME_USE_GPROUP__
	pme_msg_enter_group();
	if (!mmi_frm_scrn_enter(
	     GRP_ID_PME_MAIN, 
	     SCR_PME_POP_NEWMSG, 
	     pme_new_popscreen_exit, 
	     (FuncPtr)pme_new_popscreen_entry, 
	     MMI_FRM_FULL_SCRN))
	{
		return;
	}
	gui_buffer = mmi_frm_scrn_get_gui_buf(GRP_ID_PME_MAIN, SCR_PME_POP_NEWMSG);	
#else
	EntryNewScreen(SCR_PME_POP_NEWMSG, pme_new_popscreen_exit, pme_new_popscreen_entry, NULL);
	gui_buffer = GetCurrGuiBuffer(SCR_PME_POP_NEWMSG);
#endif

	ShowCategory165Screen(STR_GLOBAL_OK, IMG_GLOBAL_OK,
							STR_GLOBAL_BACK, IMG_GLOBAL_BACK,
							(UI_string_type)get_string(STR_PME_NEWMSG),//temp.
							#ifdef __IS_11A__
							IMG_GLOBAL_ACTIVATED,
							#else
							IMG_NEW_MESSAGE_NOTIFICATION_MSG,
							#endif
							gui_buffer);
	
	ClearAllKeyHandler();
	pme_clear_idle_msg_ind();
	//SetKeyHandler(pme_idle_new_msg_power_key_handle,KEY_END, KEY_EVENT_DOWN);
	SetLeftSoftkeyFunction(pme_new_popscreen_lsk, KEY_EVENT_UP);
//	SetRightSoftkeyFunction(pme_goback_idle_msg_ind, KEY_EVENT_UP);
#ifdef __PME_USE_GPROUP__
	SetRightSoftkeyFunction(mmi_frm_scrn_close_active_id, KEY_EVENT_UP);
#else
	SetRightSoftkeyFunction(GoBackHistory, KEY_EVENT_UP);
#endif
}

#ifdef __IS_11A__
MMI_ID vapp_pme_launch(void *param, U32 param_size)
{
	pme_main_entry();
	return MMI_RET_OK;
}
#endif
#endif

