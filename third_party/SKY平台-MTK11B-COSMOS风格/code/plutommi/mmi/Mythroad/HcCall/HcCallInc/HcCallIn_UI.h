#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#ifndef __HCCALL_UI_H_20090803__
#define __HCCALL_UI_H_20090803__


#define HCNUMBER_LEN 	  65
#define HCSTRLOCATION_LEN 128

#define MAX_SETIONITEMS   31
#define SETION_ITEMLEN    21


#define HC_SERCHER_MAX_LEN	    15+1
#ifndef HC_IMSI_MAX_LEN
#define HC_IMSI_MAX_LEN	        16+1
#endif
#ifndef HC_FIELDVALUE_MAX_LEN
#define HC_FIELDVALUE_MAX_LEN	32
#endif

#define __HCCALL_ADD_TIP__
#endif//__HCCALL_UI_H_20090803__
#endif//__HC_CALL_NEW__

