#ifndef __MMIDMENU_WINTAB_H__
#define __MMIDMENU_WINTAB_H__

#ifndef MRAPP_SUPPORT
#include"mrporting_dmenu.h"
#else
#include"mrporting.h"
#endif
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_dualSimSelectWin(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_exitDmenu(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_showDynMenuWin(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_showTextWin(char* title, char* content);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void* MMIDMENU_memMalloc(int32 size);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_memFree(void* ptr);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getActivedPlmn(char* plmn);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getLen(const char* filename);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_open(const char* filename,  uint32 mode);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_close(int32 f);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_read(int32 f,void *p,uint32 l,int32 *readlen);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_write(int32 f,void *p,uint32 l);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_seek(int32 f, int32 pos, int method);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
#ifndef MRAPP_SUPPORT
int32 MMIDMENU_getUserInfo(mr_userinfo* info);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getScreenInfo(mr_screeninfo* screeninfo);
#endif
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_sendSms(char* pNumber, char*pContent, int32 flags);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_connectWAP(char* wap_url);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_popNoSimWin(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void MMIDMENU_popNoStoreWin(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_storeIsOk(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_initDualSimInfo(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getActiveSim(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
int32 MMIDMENU_getSimNum(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//	Note: 
/*****************************************************************************/
void EntryDsmDynamicMenu(void);


#endif

