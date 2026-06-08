#ifdef __HC_CALL_NEW__
#ifndef __HCCALL_NET_H__
#define __HCCALL_NET_H__

#include "dsm_socket.h"
#include "dsm_http.h"


typedef struct  HCCALL_DOWNLOADER_T
{
	int32 total;
	int32 accept;
	int32 hFile;
}HCCALL_DOWNLOADER;


typedef enum   
{
	HCCALL_NET_STATE_IDLE = 0X00,
	HCCALL_NET_STATE_DIALING,
	HCCALL_NET_STATE_CONNECTING,
	HCCALL_NET_STATE_CONNECTED,
	HCCALL_NET_STATE_ERROR,
	HCCALL_NET_STATE_UNKNOWN
}HCCALL_NETSTATE;

typedef struct  HCCALL_HttpEngine_T
{
	PDSM_SOCKET socket;
	int32 respCode;
	HCCALL_NETSTATE state;

	uint32 buscode;
	uint16 DnsRetry;

	HCCALL_DOWNLOADER downloader;
}HCCALL_HttpEngine, *PHCCALL_HttpEngine;


int32 hccall_net_dial(int32 WifiId);
int32 hccall_net_close(void);

#endif


typedef struct HCCALL_CONFIG_T
{
	U16 	networkset;
	U16 	functionset;
	int32	CurrSimId;		// current selected sim id, not the menu index.
	U16 	autoupdate;
	U32 	lastmonth;
}HCCALL_CONFIG;

extern HCCALL_CONFIG g_hccall_confg;

#endif
