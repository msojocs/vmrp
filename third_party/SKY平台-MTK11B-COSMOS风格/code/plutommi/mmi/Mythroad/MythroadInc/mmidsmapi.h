#ifdef __MMI_DSM_NEW__

#ifndef __MMI_DSM_API__
#define __MMI_DSM_API__
#include "mrporting.h"


typedef struct 
{
	int32 p1;
	int32 p2;
}mr_common_rsp_t;


extern mr_common_rsp_t  g_mr_common_rsp;
extern int32 MAIN_LCD_device_height;
extern int32 MAIN_LCD_device_width;

#endif
#endif


/*#auto end#*/