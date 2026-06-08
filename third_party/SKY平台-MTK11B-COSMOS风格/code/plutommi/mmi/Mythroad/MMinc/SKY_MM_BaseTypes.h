#ifndef _SKY_MM_BASE_TYPES_H
#define _SKY_MM_BASE_TYPES_H


//#define fflush(nx)
#define MMStdOut MMNULL


#define MMNULL	0
#define INT_MAX 0x7fffffff

#ifndef UINT_MAX
#define UINT_MAX      0xffffffff
#endif

#ifndef IN
#define IN
#endif	//IN
#ifndef OUT
#define OUT
#endif	//OUT

#define MMVoid				void

typedef signed char			MMInt8;
typedef unsigned char		MMUInt8;
typedef short int			MMInt16;
typedef unsigned short		MMUInt16;
typedef int					MMInt32;
typedef unsigned int		MMUInt32;
typedef __int64			MMInt64;
typedef unsigned __int64	MMUInt64;
typedef double				MMDouble;

typedef unsigned int	DWORD;
typedef int				HRESULT;
typedef void*			PVOID;
typedef unsigned char	BYTE;
typedef unsigned int	ULONG;


#define E_FAIL	0x80000008
#define S_OK	0x00000000
#define S_FALSE 0x00000001
#define FALSE	0
#define TRUE	1
    
#define MMAssert(x)			my_assert((x))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
#define STRIDE_ALIGN 4
#define INT32_ALIGN(x)		(((x)+3)>>2<<2)


#define FAILED(Status) ((HRESULT)(Status)<0)
#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)

#define min(x,y) (((x) > (y)) ? (y) : (x))


#endif	//_SKY_MM_BASE_TYPES_H

