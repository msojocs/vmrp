#ifdef DMENU_SUPPORT
/****************************************************************************
** File Name:      mmidmenu.c                            
** Author:           sky-mobi                                                      
** Date:            2010/09/20
** Copyright:          
** Description:    
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** 
****************************************************************************/
#ifndef __MMIDMENU_C__
#define __MMIDMENU_C__  

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "window_parse.h"
//#include "mrporting.h"
#include "mmidmenu.h"
#include "mmidmenu_lib.h"
#include "mmidmenu_wintab.h"
#include "mmidmenu_data.h"
#include "Mmiset_id.h"
#include "dmenu_mmi_text.h"
 
void MMIDMENU_Entry(int entry)
{
	MMI_STRING_T string = {0};
	
#ifdef __DSM_GSM_ONLY__
	SetSupportGSMOnly(1);
#endif	
	MMIRES_GetText(TXT_DMENU_MAINZONE_TITLE, MMIDMENU_MENU_WIN_ID, &string);
	dsmDymInitMenuData(operators_sheet,
				sizeof(operators_sheet)/sizeof(DSM_SHEET_T), 
				(char*)string.wstr_ptr,
				string.wstr_len*2
			);
			//(char*)"\x4D\x00\x4D\x00\x49\x00\x44\x00\x4D\x00\x45\x00\x4E\x00\x55\x00\x20\x00\x54\x00\x45\x00\x53\x00\x54\x00\x00\x00");

	EntryDsmDynamicMenu();
}
 

#endif
#endif

