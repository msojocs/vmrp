#ifndef __MMIDMENU_LIB_H__
#define __MMIDMENU_LIB_H__

#define DSM_MAX_List_ITEM	60

typedef enum
{
    DSM_MENU_ENTERY = 0,
    DSM_MENU_BACK = 1,
    DSM_MENU_REFRESH  = 2,
}DSM_MENU_TYPE;

typedef struct
{
	const uint8*	data;		//运营商对应的预置数组
	uint8*			op_code;		//运营商号码
}DSM_SHEET_T;

int dsmDymInitMenuData(DSM_SHEET_T * menu,int size,int8 *title, int32 titlelen);
int8 * dsmDymGetCurTitle(void);
int dsmDymGetCurHilight(void);
uint8* dsmDymGetCurDescription(void);
uint8* dsmDesScreenGetTitle(void);
void mmi_dynamic_set_index(int32 nIndex);
int dsmDymGetMenuItem(int8 **items_list);
void dsm_left_handle(void);
void dsmGoBackHistory(void);
int32 EntryDynamicMenu(char *title);
void SetSupportGSMOnly(int gsm);
void SetMenuEntryType(int type);


void Debug_FilePrintf(const char *pcFormat, ...);
#define LOG(a) Debug_FilePrintf a
#endif
