/*
 * Header file for h264 codec API prototype
 */
#ifndef __VC_BASE_TYPES_H__
#define __VC_BASE_TYPES_H__

typedef void VC_Handle;

/* status enum */
typedef enum {
    VC_OKAY = 0,
    VC_FAIL,
    VC_MEMORY_ERR,
	VC_FORMAT_ERR,
} VC_STATUS;

/* base types */
typedef signed char			VCI8;
typedef unsigned char		VCU8;
typedef short int			VCI16;
typedef unsigned short		VCU16;
typedef signed int			VCI32;
typedef unsigned int		VCU32;
typedef long long			VCI64;
typedef unsigned long long  VCU64;

#define VCNULL 0
#define VCVoid void

#define MPC_FAIL     (0x01<<31)

#define MPC_SCREEN_NOT_SUPPORT_ROTATE     (0x01<<4)

#define MPC_KEYBOARD_NO_NUM_KEY     (0x01<<4)
#define MPC_KEYBOARD_NO_OK_KEY     (0x01<<5)


#endif //__VC_BASE_TYPES_H__
