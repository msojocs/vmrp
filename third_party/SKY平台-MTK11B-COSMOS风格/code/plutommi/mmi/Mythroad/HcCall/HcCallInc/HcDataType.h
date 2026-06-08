#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#ifndef _PIM_DATA_TYPE_H
#define _PIM_DATA_TYPE_H

#include "dsm_datatype.h" 

//#define DEBUG_FILE_TRACE
#define HCCALL_DEFAULT_AUTOUPDATE	(0)//0-Close; 1-one month; 2-two month; 3-three month
//#define __HCCALL_DATABASE_FIRM__

#define HCUSER_CONFIG_F		"hccallconfig.dat"
#define HC_FILELOG				"hccall.log"
#define HCPIMFILE_SETTINGS		"Settings.dat"
#define INDEX_DATA				"index.sky"
#define SETION_DATAFILE		"section.sky"

#define HC_FIELDVALUE_MAX_LEN	32



#ifdef DEBUG_FILE_TRACE
void FilePrintf(const char *pcFormat, ...);
#define FILE_TRACE    FilePrintf
#else
#ifdef WIN32
#define FILE_TRACE
#else
#define FILE_TRACE(...)
#endif
#endif


enum{
    HC_MENU_DSM_GAME_CENTER,
    HC_STR_GLOBAL_OK,
    HC_IMG_GLOBAL_OK,
    HC_STR_GLOBAL_BACK,
    HC_IMG_GLOBAL_BACK,
    HC_KEY_EVENT_UP,
    HC_MOD_MMI
};

enum{
    HC_DRIVER_SYSTEM,
    HC_DRIVER_PUBLIC,
    HC_DRIVER_CARD
};

typedef enum HC_DLD_EVT_E
{
	HC_DLD_FINDNEW,
	HC_DLD_ISNEW,
	HC_DLD_CONERROR,
	HC_DLD_NETERROR,
	HC_DLD_SERVERERROR
}HC_DLD_EVT;

#endif //_PIM_DATA_TYPE_H
#endif //__HC_CALL_NEW__
