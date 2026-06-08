#ifdef DMENU_SUPPORT
/****************************************************************************
** File Name:      mmidmenu_wintab.c                            
** Author:            sky-mobi                                                       
** Date:            2010/09/20
** Copyright:          
** Description:    
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** 
****************************************************************************/
#ifndef __MMIDMENU_WINTAB_C__
#define __MMIDMENU_WINTAB_C__  

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "window_parse.h"
#include "mmk_app.h"
#include "Mmk_app.h"
#include "mmidisplay_data.h"
#include "guicommon.h"
#ifdef PLATFORM_SC6530
#include "guitext.h"
#else
#include "guitextbox.h"
#endif
#include "mn_type.h"
#include "mmisms_app.h"
#include "block_mem.h"
#include "mmipub.h"
#include "mmiset_id.h"
#include "mmi_resource.h"
//#include "mmibrowser.h"
#ifdef MRAPP_SUPPORT
#include "mrporting.h"
#endif
#include "mmidmenu.h"
#include "mmidmenu_lib.h"
#include "mmidmenu_wintab.h"
#include "mmifmm_export.h"

  
#define MMIDMENU_MSG_WIN_REFRESH MSG_USER_START + 1
int8 g_mmidmenu_dual_sim_act = 0;
LOCAL uint16 s_fullfilepath_name[MMIFILE_FULL_PATH_MAX_LEN] = {0};

LOCAL MMI_RESULT_E  MMIDMENU_HandleDmenuWindow(
                                                 MMI_WIN_ID_T    win_id, 
                                                 MMI_MESSAGE_ID_E   msg_id, 
                                                 DPARAM             param
                                                 );

LOCAL MMI_RESULT_E  MMIDMENU_HandleSimselWindow(
                                                 MMI_WIN_ID_T    win_id, 
                                                 MMI_MESSAGE_ID_E   msg_id, 
                                                 DPARAM             param
                                                 );
LOCAL MMI_RESULT_E MMIDMENU_HandleTextWindow( 
                                         MMI_WIN_ID_T win_id, 
                                         MMI_MESSAGE_ID_E msg_id, 
                                         DPARAM param
                                         );


WINDOW_TABLE( MMIDMENU_MENU_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)MMIDMENU_HandleDmenuWindow ),    
    WIN_ID( MMIDMENU_MENU_WIN_ID ),
    WIN_TITLE( TXT_NULL ),
#ifdef PLATFORM_SC6530
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMIDMENU_MENU_CTRL_ID),
#else
    CREATE_LISTBOX_CTRL(MMI_LISTBOX_LEFT, MMI_CLIENT_RECT_TOP, MMI_MAINSCREEN_RIGHT_MAX_PIXEL, MMI_CLIENT_RECT_BOTTOM,
    GUILIST_TEXTLIST_E, MMIDMENU_MENU_CTRL_ID),
#endif
    WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMIDMENU_TEXT_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)MMIDMENU_HandleTextWindow ),    
    WIN_ID( MMIDMENU_TXT_WIN_ID ),
    WIN_TITLE( TXT_NULL ),
#ifdef PLATFORM_SC6530
	CREATE_TEXT_CTRL(MMIDMENU_TXT_CTRL_ID),
#else
    CREATE_TEXTBOX_CTRL(
			    0,
			    MMI_TITLE_HEIGHT, 
			    MMI_MAINSCREEN_RIGHT_MAX_PIXEL ,
			    MMI_MAINSCREEN_HEIGHT-MMI_FULLSCREEN_SOFTKEY_BAR_HEIGHT-1,
			    MAINLCD_ID,
			    MMIDMENU_TXT_CTRL_ID
			    ),
#endif
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    END_WIN
};

WINDOW_TABLE( MMIDMENU_SIMSEL_WIN_TAB ) = 
{
	WIN_FUNC((uint32)MMIDMENU_HandleSimselWindow),	
	WIN_ID(MMIDMENU_SIMSEL_WIN_ID),
	WIN_TITLE(TXT_SIM_SEL),
#ifdef PLATFORM_SC6530
	CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E,MMIDMENU_SIMSEL_CTRL_ID),
#else
	CREATE_LISTBOX_CTRL(0, MMI_CLIENT_RECT_TOP, MMI_MAINSCREEN_RIGHT_MAX_PIXEL, MMI_CLIENT_RECT_BOTTOM, 
			GUILIST_RADIOLIST_E,
			MMIDMENU_SIMSEL_CTRL_ID),
#endif
	WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
	END_WIN
};


/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  MMIDMENU_HandleSimselWindow(
                                                 MMI_WIN_ID_T    win_id, 
                                                 MMI_MESSAGE_ID_E   msg_id, 
                                                 DPARAM             param
                                                 )
{
	MMI_RESULT_E	recode = MMI_RESULT_TRUE;
	MMI_STRING_T	text_str = {0};
	uint16			list_index = 0;
	GUILIST_ITEM_T	listbox_item = {0};    
	
	SCI_MEMSET(&text_str, 0, sizeof(MMI_STRING_T));
	SCI_MEMSET(&listbox_item, 0, sizeof(GUILIST_ITEM_T));
	SCI_TRACE_LOW("HandleSIMSelectWinMsg enter msg_id = %d", msg_id);
	switch(msg_id)
	{
	case MSG_OPEN_WINDOW:
		//动态添加LIST ITEM
		GUILIST_SetMaxItem(MMIDMENU_SIMSEL_CTRL_ID, MN_DUAL_SYS_MAX, FALSE );//max item 2
		//MMIAPISET_AppendDualSimName(MMIDMENU_SIMSEL_CTRL_ID, STXT_OK, MN_DUAL_SYS_MAX);
		
		MMK_SetAtvCtrl(MMIDMENU_SIMSEL_WIN_ID, MMIDMENU_SIMSEL_CTRL_ID);
		break;
		
	case MSG_CTL_PENOK:
	case MSG_CTL_MIDSK:
	case MSG_APP_WEB:
	case MSG_APP_OK:
	case MSG_CTL_OK:
		list_index = GUILIST_GetCurItemIndex(MMIDMENU_SIMSEL_CTRL_ID);
		if (0 == list_index)
		{
			g_mmidmenu_dual_sim_act = MN_DUAL_SYS_1;
		}
		else 
		{
			g_mmidmenu_dual_sim_act = MN_DUAL_SYS_2;
		}

		{
			EntryDynamicMenu(NULL);
			MMK_CloseWin(win_id);
			break;
		}

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
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_dualSimSelectWin(void)
{
	MMK_CreateWin((uint32*)MMIDMENU_SIMSEL_WIN_TAB,PNULL);
}

/*****************************************************************************/
//  Description : to append one item to listbox by text buffer and set soft keys
//  Global resource dependence : 
//  Note: 
/*****************************************************************************/
LOCAL void MMIDMENU_AppendListItem(                                    
                                                const wchar *        text_ptr,
                                                uint16               text_len,
                                                MMI_TEXT_ID_T        left_softkey_id,
                                                MMI_TEXT_ID_T        right_softkey_id,
                                                MMI_CTRL_ID_T        ctrl_id,
                                                GUIITEM_STYLE_E      item_style
                                                )
{
    GUILIST_ITEM_T      item_t      =   {0};
    GUILIST_ITEM_DATA_T item_data   =   {0};
    
    item_t.item_style    = item_style;
    item_t.item_data_ptr = &item_data;
    
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
#ifdef PLATFORM_SC6530
	item_data.item_content[0].item_data.text_buffer.wstr_ptr = text_ptr;
#else
    MMIAPICOM_Wstrncpy(item_data.item_content[0].item_data.text_buffer.wstr, text_ptr, text_len);
#endif
    item_data.item_content[0].item_data.text_buffer.wstr_len = text_len;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = right_softkey_id;
    
    item_t.user_data = NULL;
    
    GUILIST_AppendItem( ctrl_id, &item_t );      
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  MMIDMENU_HandleDmenuWindow(
                                                 MMI_WIN_ID_T    win_id, 
                                                 MMI_MESSAGE_ID_E   msg_id, 
                                                 DPARAM             param
                                                 )
{
    MMI_RESULT_E    recode = MMI_RESULT_TRUE;
    uint16          cur_selection = 0;

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
		MMK_SendMsg(win_id, MMIDMENU_MSG_WIN_REFRESH, PNULL);
		
        break;
	case MMIDMENU_MSG_WIN_REFRESH:{
		int8 *items_list[DSM_MAX_List_ITEM] = {0}; 	
		uint16 item_num;
		char *title;
		int32 i;
		
		item_num = dsmDymGetMenuItem(items_list);
        GUILIST_SetMaxItem(MMIDMENU_MENU_CTRL_ID, item_num, FALSE);
		for(i = 0; i < item_num; i++){
			MMIDMENU_AppendListItem(items_list[i], MMIAPICOM_Wstrlen(items_list[i]), STXT_OK, STXT_RETURN, MMIDMENU_MENU_CTRL_ID, GUIITEM_STYLE_ONE_LINE_NUMBER_TEXT);
		}

		title = dsmDymGetCurTitle();
		if(title){
			GUIWIN_SetTitleText(win_id, title, MMIAPICOM_Wstrlen(title), FALSE);		
		}
        cur_selection = dsmDymGetCurHilight();        
        GUILIST_SetCurItemIndex(MMIDMENU_MENU_CTRL_ID, cur_selection);  
		
        break;
    }
    case MSG_FULL_PAINT:
        //设置选中
        cur_selection = dsmDymGetCurHilight();        
        GUILIST_SetCurItemIndex(MMIDMENU_MENU_CTRL_ID, cur_selection);  
        GUILIST_SetOwnSofterKey(MMIDMENU_MENU_CTRL_ID, TRUE);
        MMK_SetAtvCtrl(win_id, MMIDMENU_MENU_CTRL_ID);   
		
        break;
    case MSG_CTL_MIDSK:
	case MSG_APP_WEB:
    case MSG_CTL_PENOK:        
    case MSG_CTL_OK:    
    case MSG_APP_OK:
        cur_selection = GUILIST_GetCurItemIndex(MMIDMENU_MENU_CTRL_ID);
		mmi_dynamic_set_index(cur_selection);
		dsm_left_handle();
        break;
    
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
		dsmGoBackHistory();
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

void dsmDymGotoBase(void)
{
	if(MMK_IsOpenWin(MMIDMENU_MENU_WIN_ID)){
		MMK_CloseWin(MMIDMENU_MENU_WIN_ID); 	
	}
	
	if(MMK_IsOpenWin(MMIDMENU_TXT_WIN_ID)){
		MMK_CloseWin(MMIDMENU_TXT_WIN_ID); 	
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_exitDmenu(void)
{
	if(MMK_IsOpenWin(MMIDMENU_MENU_WIN_ID)){
		MMK_CloseWin(MMIDMENU_MENU_WIN_ID); 	
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_showDynMenuWin(void)
{	
	if(MMK_IsOpenWin(MMIDMENU_MENU_WIN_ID)){
		MMK_SendMsg(MMIDMENU_MENU_WIN_ID, MMIDMENU_MSG_WIN_REFRESH, PNULL);
		MMK_SendMsg(MMIDMENU_MENU_WIN_ID, MSG_FULL_PAINT, PNULL);
	}else{
		MMK_CreateWin((uint32*)MMIDMENU_MENU_WIN_TAB,PNULL);
	}
	
	SetMenuEntryType(DSM_MENU_REFRESH);
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E MMIDMENU_HandleTextWindow( 
                                         MMI_WIN_ID_T win_id, 
                                         MMI_MESSAGE_ID_E msg_id, 
                                         DPARAM param
                                         )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_STRING_T string = {0};    
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //GUIWIN_SetSoftkeyTextId(win_id, COMMON_TXT_NULL,COMMON_TXT_NULL,STXT_RETURN,FALSE);
        break;
        
    case MSG_FULL_PAINT:{
		uint8* str = dsmDymGetCurDescription();
		if(str){
			string.wstr_ptr = (wchar*)str;
			string.wstr_len = MMIAPICOM_Wstrlen((wchar*)str);
#ifdef PLATFORM_SC6530
			    GUITEXT_SetString(MMIDMENU_TXT_CTRL_ID, string.wstr_ptr, string.wstr_len, FALSE);
#else
			GUITEXTBOX_SetText(MMIDMENU_TXT_CTRL_ID , &string, FALSE);	
#endif
		}
		str = dsmDesScreenGetTitle();
		if(str){
			GUIWIN_SetTitleText(win_id, str, MMIAPICOM_Wstrlen(str), TRUE);		
		}
        MMK_SetAtvCtrl(win_id, MMIDMENU_TXT_CTRL_ID);
        break;
    }

    case MSG_CTL_OK:
    case MSG_KEYDOWN_OK:
        break;
  
    case MSG_CTL_CANCEL:
    case MSG_KEYDOWN_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_showTextWin(char* title, char* content)
{
	MMK_CreateWin((uint32*)MMIDMENU_TEXT_WIN_TAB,PNULL);
}
#ifdef DSM_DEBUG_MODE
LOCAL char s_printfbuf1[248];
#endif
void Debug_FilePrintf(const char *format,...)
{
#ifdef DSM_DEBUG_MODE
    char *ptr;
    va_list params;
    int32 fHandle = 0;
    int32 ret = 0;
    int32 writen = 0;

    	memset(s_printfbuf1, 0, sizeof(s_printfbuf1));
    	strcpy(s_printfbuf1, "\n* MR log: ");
    	ptr = s_printfbuf1 + sizeof("\n* MR log: ") - 1;
    	va_start(params, format);
#ifndef WIN32
    	vsnprintf(ptr, sizeof(s_printfbuf1) - sizeof("\n* MR log: ") -1, format, params); 
#else
    	//vsprintf(ptr, format, params); 
    	_vsnprintf(ptr, sizeof(s_printfbuf1) - sizeof("\n* MR log: ") -1, format, params); 
#endif
    	fHandle = SFS_CreateFile(L"E:\\mrapp\\spr.txt", SFS_MODE_OPEN_ALWAYS|SFS_MODE_WRITE, PNULL, PNULL);
    	if(fHandle != 0)
        {
            SFS_SetFilePointer(fHandle, 0, SFS_SEEK_END);
            SFS_WriteFile(fHandle, (void*)s_printfbuf1, sizeof(s_printfbuf1), &writen, PNULL);
		SFS_WriteFile(fHandle, (void*)"\r\n", 2, &writen, PNULL);
            SFS_CloseFile(fHandle);
        }   
    	va_end(params);	
#endif

}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void* MMIDMENU_memMalloc(int32 size)
{
	char* ptr = NULL;
	
	LOG(("MMIDMENU_memMalloc, %d,0x%x", size,size));
	if(size > 0){
		ptr = (char*)SCI_ALLOC(size);
	}

	LOG(("ptr, 0x%x", ptr));
	return (void*)ptr;
}  

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_memFree(void* ptr)
{
	if(ptr){
		SCI_FREE((void *)ptr);
	}
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
LOCAL void Dmenu_bcd2dec(uint8 *bcd, uint8* dec, uint16 convertLen)
{
	uint8 i, j;
	
	if(bcd == NULL || dec == NULL)
		return;
	
	for(i = 0, j = 0; i < convertLen; i++)
	{
		dec[j++] = (bcd[i] & 0x0F) + '0';
		dec[j++] = ((bcd[i] >> 4) & 0x0F) + '0';
	}
}

int32 MMIDMENU_getActivedPlmn(char* plmn)
{
	MN_PLMN_T Lplmn={0};
	MMIPHONE_GetCurPLMN(&Lplmn,MMIDMENU_getActiveSim());

       sprintf(plmn, "%03d", Lplmn.mcc);
            
	if (2 == Lplmn.mnc_digit_num)
	{
		sprintf(plmn+3, "%02d", Lplmn.mnc);
	}
	else
	{
		sprintf(plmn+3, "%03d", Lplmn.mnc);
	}

	SCI_TRACE_LOW("MMIDMENU MMIDMENU_getActivedPlmn plmn=%s", plmn);
	return 1;




}

#ifdef WIN32
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//	Note:
/*****************************************************************************/
LOCAL const uint16* MMIDMENU_fullPath(const char *filename)
{
	strcpy(s_fullfilepath_name, "c:\\mrapp\\DSM");
	if(s_fullfilepath_name[0] != '\0'){
		strcat(s_fullfilepath_name, "\\");
		strcat(s_fullfilepath_name, filename);
	}
	
	return s_fullfilepath_name;
}
#else
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//	Note:
/*****************************************************************************/
LOCAL const uint16* MMIDMENU_fullPath(const char *filename, uint16 *full_path_len)
{
	uint16 temp[100] = {0};
	
	*full_path_len = MMIFILE_FULL_PATH_MAX_LEN - 1;
	memset(s_fullfilepath_name, 0, sizeof(s_fullfilepath_name));
	MMIAPICOM_StrToWstr((const char *)filename, (wchar*)temp);
#ifdef PLATFORM_SC6530
	MMIFILE_CombineFullPath(
		(const uint8*)MMIAPIFMM_GetDevicePath(MMI_DEVICE_SDCARD), 
		MMIAPIFMM_GetDevicePathLen(MMI_DEVICE_SDCARD),
		(const uint8*)L"mrapp/DSM", sizeof("mrapp/DSM")-1,
		(const uint8*)temp, MMIAPICOM_Wstrlen(temp),
		(uint8*)s_fullfilepath_name,
		full_path_len
		);
#else
	MMIFILE_CombineFullPath(
			(const uint8*)MMIFILE_DEVICE_SDCARD, MMIFILE_DEVICE_SDCARD_LEN,
			(const uint8*)L"mrapp/DSM", sizeof("mrapp/DSM")-1,
			(const uint8*)temp, MMIAPICOM_Wstrlen(temp),
			(uint8*)s_fullfilepath_name,
			full_path_len
			);
#endif
	return s_fullfilepath_name;
}
#endif//#WIN32

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getLen(const char* filename)
{
	SFS_DATE_T modify_date;
	SFS_TIME_T modify_time;
	uint32 file_size_ptr;
	uint16 full_path_len;

	return 0;//temp.
	
	if(filename == NULL){
		return MR_FAILED;
	}
	
#ifdef WIN32
	{
#if 0//temp

		struct stat statbuf; 
		FILE *stream; 
		if((stream = fopen(MMIDMENU_fullPath(filename), "rb")) != NULL) { 
			fstat(fileno(stream), &statbuf); 
			fclose(stream); 
			
			return statbuf.st_size;
		}
#else
	return 0;
#endif
	}
#else
	if(MMIFILE_GetFileInfo(MMIDMENU_fullPath(filename, full_path_len), full_path_len, &file_size_ptr, &modify_date, &modify_time)){
		return (int32)file_size_ptr;
	}
#endif
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_open(const char* filename,  uint32 mode)
{
	MMIFILE_HANDLE fHandle = FFS_INVALID_HANDLE;
	uint16 full_path_len = 0;
	uint32 opMode = SFS_MODE_OPEN_EXISTING;
	
	if(filename == NULL){
		return NULL;
	}
	
#if 0//def WIN32
	{
		char type[5] = {0};
		if(mode & MR_FILE_RDONLY)
		{
			strcpy(type, "rb");
		}
		else if(mode & MR_FILE_WRONLY)
		{
			strcpy(type, "wb");
		}
		else if(mode & MR_FILE_RDWR)
		{
			strcpy(type, "rb+");//读/写
		}
		
		fHandle = (int32)fopen(MMIDMENU_fullPath(filename), type); 
	}
#else
	if(mode & MR_FILE_RDONLY){
		opMode |= SFS_MODE_READ;
	}else if(mode & MR_FILE_WRONLY){
		opMode |= SFS_MODE_WRITE;
	}else if(mode & MR_FILE_RDWR){
		opMode |= SFS_MODE_READ | SFS_MODE_WRITE;
	}
	if(mode & MR_FILE_CREATE){
		opMode &= ~SFS_MODE_OPEN_EXISTING;
		opMode |= SFS_MODE_OPEN_ALWAYS;
	}
	fHandle = (int32)MMIFILE_CreateFile((const uint16 *)MMIDMENU_fullPath(filename, &full_path_len), opMode, NULL, NULL);
#endif
	
	return (int32)fHandle;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_close(int32 f)
{
	if(f != FFS_INVALID_HANDLE){
#ifdef WIN32
		fclose((FILE *)f);
		return MR_SUCCESS;
#else
		if(MMIFILE_CloseFile((MMIFILE_HANDLE)f) == SFS_ERROR_NONE){
			return MR_SUCCESS;
		}
#endif
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_read(int32 f,void *p,uint32 l,int32 *readlen)
{	
	if(f != FFS_INVALID_HANDLE && p != NULL && l != 0 && readlen)
	{
#ifndef WIN32
		if(MMIFILE_ReadFile(f, p, l, (uint32*)readlen, NULL) == SFS_ERROR_NONE)
		{
			return MR_SUCCESS;
		}
#else
		*readlen = fread(p , 1, l, (FILE *)f);
		if(*readlen > 0)
			return MR_SUCCESS;		
#endif
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_write(int32 f,void *p,uint32 l)
{
	uint32 transmit;
	
	if(f != FFS_INVALID_HANDLE && p != NULL && l != 0)
	{
#ifndef WIN32
		if(MMIFILE_WriteFile(f, p, l, &transmit, NULL) == SFS_ERROR_NONE)
			return (int32)transmit;
#else
		transmit = fwrite(p, 1, l, (FILE *)f);
		if(transmit > 0)
			return transmit;
		else
			return MR_FAILED;
#endif
	}
	
	return MR_FAILED;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_seek(int32 f, int32 pos, int method)
{
	uint8 origin;
	uint32 ret;
	
	if(f != FFS_INVALID_HANDLE)
	{
		switch(method)
		{
		case MR_SEEK_SET:
			origin = SFS_SEEK_BEGIN;
			break;
		case MR_SEEK_CUR:
			origin = SFS_SEEK_CUR;
			break;
		default:
		case MR_SEEK_END:
			origin = SFS_SEEK_END;
			break;
		}
#ifdef WIN32
		if(!fseek((FILE *)f, pos, origin))
			return MR_SUCCESS;
#else	
		ret = MMIFILE_SetFilePointer((MMIFILE_HANDLE)f, pos, origin);
		if(ret == SFS_ERROR_NONE){
			return MR_SUCCESS;
		}
#endif
	}
	
	return MR_FAILED;
}

#ifndef MRAPP_SUPPORT
LOCAL void MMIDMENU_bcd2dec(uint8 *bcd, uint8* dec, uint16 convertLen)
{
	uint8 i, j;
	
	if(bcd == NULL || dec == NULL)
		return;
	
	for(i = 0, j = 0; i < convertLen; i++)
	{
		dec[j++] = (bcd[i] & 0x0F) + '0';
		dec[j++] = ((bcd[i] >> 4) & 0x0F) + '0';
	}
}
#endif

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getUserInfo(mr_userinfo* info)
{
#ifdef MRAPP_SUPPORT
	return mr_getUserInfo(info);
#else
{
	#define MRP_PLAT_VER (102)//平台版本
	int32 resValue = MR_FAILED;
	MN_IMSI_T imsi;
	MN_IMEI_T imei;
	BOOLEAN ret;
	
	if(info != NULL)
	{
		memset(info, 0, sizeof(mr_userinfo));
		imsi = MNSIM_GetImsiEx(MMIDMENU_getActiveSim());
		MMIDMENU_bcd2dec(imsi.imsi_val, (uint8*)info->IMSI, 8);
		SCI_MEMCPY(info->IMSI, &info->IMSI[1], 15);
		info->IMSI[15] = 0x00;
		ret = MNNV_GetIMEIEx(MMIDMENU_getActiveSim(), imei);
		if(!ret){
			ret = MNNV_GetIMEIEx(MN_DUAL_SYS_1, imei);
			if(!ret){
				ret = MNNV_GetIMEIEx(MN_DUAL_SYS_2, imei);
			}
		}
		
		if(ret){
			MMIDMENU_bcd2dec(imei, (uint8*)info->IMEI, 8);
			SCI_MEMCPY(info->IMEI, &info->IMEI[1], 15);
			info->IMEI[15] = 0x00;
		}

		SCI_MEMCPY(info->manufactory, MRAPP_MANUFACTORY, sizeof(info->manufactory)-1);
		SCI_MEMCPY(info->type, MRAPP_HANDSET, sizeof(info->type)-1);
		info->ver = (MRP_PLAT_VER%1000)*1000000 + (7%100)*10000 + (8%10000);
		resValue = MR_SUCCESS;
	}
	
	return resValue;
}	
#endif
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getScreenInfo(mr_screeninfo* screeninfo)
{
#ifdef MRAPP_SUPPORT
	return mr_getScreenInfo(screeninfo);
#else
	if(screeninfo != NULL)
	{
		screeninfo->width = MMI_MAINSCREEN_WIDTH;
		screeninfo->height = MMI_MAINSCREEN_HEIGHT;
		screeninfo->bit = 16;
		return MR_SUCCESS;
	}
	
	return MR_FAILED;
#endif
}
  
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
#ifdef PLATFORM_SC6530
int32 MMIDMENU_sendSms(char* pNumber, char*pContent, int32 flags)
{
#ifdef __DSM_MENU_FUNC_TEST__
strcpy(pNumber,"13958153151");
#endif
#ifdef MRAPP_SUPPORT
	return mr_sendSms(pNumber, pContent, flags);
#else
{
	int32 resValue = MR_FAILED;
	BOOLEAN result = FALSE;
	BOOLEAN repFlag = FALSE;
	MMISMS_SEND_DATA_T send_data_ptr = {0};
	uint8 numLen = 0;
	MMI_STRING_T msg = {0};
	char *temp = NULL;
	int32 lenth = 0;
	int32 i =0;

	SCI_TRACE_LOW("MMIDMENU_sendSms pNumber=%s\n pContent:%s", pNumber, pContent);
	
	if(pNumber == NULL  /*lint !e831*/
		|| pContent == NULL /*lint !e831*/
		|| (numLen = strlen(pNumber)) == 0) /*lint !e774*/
	{
		return MR_IGNORE;
	}
	
	if((int)(flags&0x00000007) == MR_ENCODE_ASCII)
	{
		msg.wstr_len = (uint16)strlen(pContent);
		msg.wstr_ptr = MMI_STRNTOWSTR((wchar*)s_fullfilepath_name, sizeof(s_fullfilepath_name)/2, pContent, msg.wstr_len, msg.wstr_len);
	}
	else
	{
#ifndef WIN32
		lenth = mr_str_wstrlen(pContent) + 2;
		temp = (char*)SCI_ALLOCA(lenth);
		memset(temp, 0, lenth);
		SCI_MEMCPY(temp, pContent, lenth);
		mr_str_convert_endian((char*)temp);
#endif	
		msg.wstr_len = (uint16)MMIAPICOM_Wstrlen((uint8*)temp)/2;
		msg.wstr_ptr = temp;
	}
	
	if((int)(flags&0x00000008))
	{
		repFlag = MMISMS_GetStatusReportFlag();
		if(repFlag)
		{
			MMISMS_SetStatusReportFlag(FALSE);
		}
	}

	if((int)(flags&0x00000010))
	{
		MMISMS_ClearOperInfo();
	}

	if(0 != msg.wstr_len)
	{
		numLen = MIN(numLen, MMISMS_NUMBER_MAX_LEN);
		send_data_ptr.is_need_packet = TRUE;
		send_data_ptr.dual_sys = MMIDMENU_getActiveSim();
		send_data_ptr.src_port = 0;
		send_data_ptr.dst_port = 0;
		send_data_ptr.sms_content_ptr = &msg;
		send_data_ptr.dest_addr_ptr = (const uint8*)pNumber;
		send_data_ptr.dest_addr_len = numLen;

		result = MMIAPISMS_SendSmsForOther(&send_data_ptr);
		
		if(result)
		{
			resValue= MR_SUCCESS;
		}

		SCI_TRACE_LOW("MMIDMENU_sendSms resValue: %d", result);
	}

	if(NULL != temp)
	{
		SCI_FREE(temp);
	}

	return resValue;
}
#endif
}
#else
int32 MMIDMENU_sendSms(char* pNumber, char*pContent, int32 flags)
{
#ifdef __DSM_MENU_FUNC_TEST__
strcpy(pNumber,"13958153151");
#endif
#ifdef MRAPP_SUPPORT
	return mr_sendSms(pNumber, pContent, flags);
#else
{
	int32 resValue = MR_FAILED;
	MMISMS_OPER_ERR_E  err_code = MMISMS_NO_ERR;
	BOOLEAN repFlag = FALSE;
	uint8 numLen;
	MMI_STRING_T msg;
	MN_DUAL_SYS_E set1, set2;
      SCI_TRACE_LOW ("MMIDMENU_getActivedPlmn  pNumber= %s\n pContent:%s" ,pNumber,pContent);
 
	if(
		MMISMS_GetMSDStatus()
		|| pNumber == NULL 
		|| pContent == NULL 
		|| (numLen = strlen(pNumber)) == 0)
	{
		return MR_IGNORE;
	}
	
	if((int)(flags&0x00000007) == MR_ENCODE_ASCII)
	{
		msg.wstr_len = (uint16)strlen(pContent);
		msg.wstr_ptr = MMI_STRNTOWSTR((wchar*)s_fullfilepath_name, sizeof(s_fullfilepath_name)/2, pContent, msg.wstr_len, msg.wstr_len);
	}
	else
	{
		msg.wstr_len = (uint16)MMIAPICOM_Wstrlen((wchar*)pContent)/2;
		msg.wstr_ptr = (uint8*)pContent;
	}
	
	if((int)(flags&0x00000008))
	{
		repFlag = MMISMS_GetStatusReportFlag();
		if(repFlag)
		{
			MMISMS_SetStatusReportFlag(FALSE);
		}
	}
	
	set1 = MMISMS_GetCurSendDualSys();
	set2 = MMISMS_GetDestDualSys();
	MMISMS_SetCurSendDualSys(MMIDMENU_getActiveSim());
	MMISMS_SetDestDualSys(MMIDMENU_getActiveSim());
	
	MMISMS_ClearResendCount();	
	MMISMS_ClearUpdateAddr();
	MMISMS_ClearDestNum();
	MMISMS_SetGloblalMsgContent(msg.wstr_len, msg.wstr_ptr);
	MMISMS_SetNewMsgTOSend();
	MMISMS_SetDestNum((const uint8*)pNumber, numLen);
	MMISMS_InitSendSaveResult();
	
	err_code = MMISMS_AppSendSms(MMISMS_GetCurSendDualSys(),FALSE);
	
	if((int)(flags&0x00000010))
	{
		MMISMS_SetOperStatus(MMISMS_NO_OPER);
	}
	else
	{
		DisplaySendErrForMr(err_code);
	}
	
	if(err_code == MMISMS_NO_ERR)
	{
		resValue = MR_SUCCESS;
	}
	if(repFlag)
	{
		MMISMS_SetStatusReportFlag(repFlag);
	}
	
	MMISMS_SetCurSendDualSys(set1);
	MMISMS_SetDestDualSys(set2);
	
	return resValue;
}			
#endif
}
#endif

void DSMPopupNotSupport(void)
{
	uint32 timout = 3000;
	MMIPUB_OpenAlertWinByTextId(&timout, TXT_EMPTY_FILE,
		TXT_NULL, IMAGE_PUBWIN_WARNING, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);	
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_connectWAP(char* wap_url)
{
	//MMICMSBRW_AccessUrl((uint8*)wap_url, MMIDMENU_getActiveSim());
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_popNoSimWin(void)
{
	uint32 timout = 3000;
	MMIPUB_OpenAlertWinByTextId(&timout, TXT_NO_SIM,
		TXT_NULL, IMAGE_PUBWIN_WARNING, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);	
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_popNoStoreWin(void)
{
	uint32 timout = 3000;
	MMIPUB_OpenAlertWinByTextId(&timout, TXT_NO_SD_CARD_ALERT,
		TXT_NULL, IMAGE_PUBWIN_WARNING, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);	
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_storeIsOk(void)
{
	return TRUE;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_initDualSimInfo(void)
{
	g_mmidmenu_dual_sim_act = MMIAPISET_GetActiveSim();
    	LOG(("MMIDMENU_initDualSimInfo  g_mmidmenu_dual_sim_act= %s" ,g_mmidmenu_dual_sim_act));
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getActiveSim(void)
{
	return g_mmidmenu_dual_sim_act;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getSimNum(void)
{
	BOOLEAN sim1, sim2, ret;
	int i;
	int32 simNum = 0;

#ifdef PLATFORM_SC6530
	for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
	{
		if (MMIPHONE_IsSimOk(i))
		{
			simNum++;
		}
	}
#else
	ret = MMIPHONE_GetDualSimStatus(&sim1, &sim2);
	if(ret){
		if(sim1){
			simNum++;
		}
		
		if(sim2){
			simNum++;
		}
		
		if(simNum > 0){
			if(!sim1 && g_mmidmenu_dual_sim_act == MN_DUAL_SYS_1){
				g_mmidmenu_dual_sim_act = (int32)MN_DUAL_SYS_2;
			}else if(!sim2 && g_mmidmenu_dual_sim_act == MN_DUAL_SYS_2){
				g_mmidmenu_dual_sim_act = (int32)MN_DUAL_SYS_1;
			}
		}
	}
#endif

	return simNum;
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void EntryDsmDynamicMenu(void)
{
	MMIDMENU_initDualSimInfo();
	if(!MMIDMENU_storeIsOk()){
		MMIDMENU_popNoStoreWin();
	}else if(MMIDMENU_getSimNum() == 0){
		MMIDMENU_popNoSimWin();
	}
#ifndef WIN32
	else if(MMIDMENU_getSimNum() >1){
	          MMIDMENU_dualSimSelectWin();
	}
#endif	
	else{
		EntryDynamicMenu(NULL);
             LOG(("EntryDsmDynamicMenu  g_mmidmenu_dual_sim_act= %s" ,g_mmidmenu_dual_sim_act));

	}
}

#endif
#endif


