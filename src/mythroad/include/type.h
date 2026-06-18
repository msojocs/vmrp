#ifndef _M_TYPE__
#define _M_TYPE__

#include <stddef.h>
#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

typedef int BOOL;

#define FALSE 0
#define TRUE 1

#ifndef NULL
#define NULL (void*)0
#endif


// typedef char* PSTR;
// typedef const char* PCSTR;
// typedef uint8 U8;
// typedef uint64 U64;

// typedef unsigned int UINT;
// typedef unsigned long DWORD;
// typedef unsigned char BYTE;
// typedef DWORD* DWORD_PTR;


#ifndef offsetof
#define offsetof(type, field) ((size_t) & ((type *)0)->field)
#endif
#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif



#endif
