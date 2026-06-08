#ifdef __HC_CALL_NEW__
#ifndef __DSM_DATATYPE_H
#define __DSM_DATATYPE_H

#ifndef NULL
#define NULL (void*) 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;

typedef char		S8;
typedef short		S16;
typedef int		S32;

#if 0
typedef  unsigned short	uint16;
typedef  unsigned long int	uint32;
typedef  long int			int32; 
typedef  unsigned char		uint8; 
typedef  signed char		int8; 
typedef  signed short		int16;
#endif

typedef void (*FuncPtr) (void);

#include "MMI_include.h"
#include "mrporting.h" 

#endif 

#endif
