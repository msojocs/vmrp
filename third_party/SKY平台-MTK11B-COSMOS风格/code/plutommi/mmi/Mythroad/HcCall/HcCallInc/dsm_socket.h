#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#ifndef  _DSM_SOCKET_H_
#define _DSM_SOCKET_H_

#include "dsm_datatype.h" 


#define	DSM_SOCKET_MAX_NUM				(2)
#define	DSM_SOCKET_SELECT_INTERNAL		(300)
#define	DSM_SOCKET_RECVBUFFER_SIZE		(4*1024)
#define	DSM_SOCKET_SENDBUFFER_SIZE		(1024)


typedef struct DSM_SOCKET_T* PDSM_SOCKET;
typedef struct DSM_SOCKEVTDATA_T *PDSM_SOCKEVTDATA;

typedef int32 (*FN_DSM_SOCKEVENT)(PDSM_SOCKET socket, int32 evt, PDSM_SOCKEVTDATA data);
typedef void (*FN_DSM_SOCKETRECV)(PDSM_SOCKET socket);

typedef enum DSM_SOCKET_EVENT_E
{
	DSM_SOCKET_EVENT_CONNECTED,
	DSM_SOCKET_EVENT_CONNECTFAILED,
	DSM_SOCKET_EVENT_ERROR,
	DSM_SOCKET_EVENT_BUFFERFULL,
	DSM_SOCKET_EVENT_CLOSED,
	DSM_SOCKET_EVENT_RECVDATA,
	DSM_SOCKET_EVENT_NODATATIMEOUT
}DSM_SOCKEVENT;

typedef enum DSM_SOCKET_STATE_E
{
	DSM_SOCKET_STATE_CREATED,
	DSM_SOCKET_STATE_CONNECTING,
	DSM_SOCKET_STATE_CONNECTED,	
	DSM_SOCKET_STATE_BUFFERFULL,
	DSM_SOCKET_STATE_CLOSED
}DSM_SOCKSTATE;

typedef struct   DSM_SOCKEVTDATA_T
{
	char* buffer;
	int32 size;
	uint32 ip;
	uint16 port;
}DSM_SOCKEVTDATA;


typedef struct DSM_SOCKET_T
{
	int32 socketid;
	DSM_SOCKSTATE socketstate;
	FN_DSM_SOCKETRECV fnRecv;
	FN_DSM_SOCKEVENT fnEvent;
	uint32 userdata;
	
	uint8 sendbuff[DSM_SOCKET_SENDBUFFER_SIZE];
	uint8 recvBuffer[DSM_SOCKET_RECVBUFFER_SIZE];
	
	uint32 begin;
	uint32 end;
	
	uint32 dataLen;
	int32 waittime;
}DSM_SOCKET,*PDSM_SOCKET;

void DSM_SocketInitialize(void);
void DSM_SocketTerminal(void);

PDSM_SOCKET DSM_SocketCreate(FN_DSM_SOCKEVENT fnEvent, int32 userdata, int32 appHandle);
int32 DSM_SocketConnect(PDSM_SOCKET socket, uint32 ip, uint16 port, int32 block);
int32 DSM_SocketSend(PDSM_SOCKET socket, uint8* buffer, int32 size);
int32 DSM_SocketIsNull(PDSM_SOCKET socket);
int32 DSM_SocketClosed(PDSM_SOCKET socket, DSM_SOCKEVENT evt);
void DSM_SocketCloseAll(DSM_SOCKEVENT evt);

#endif
#endif
