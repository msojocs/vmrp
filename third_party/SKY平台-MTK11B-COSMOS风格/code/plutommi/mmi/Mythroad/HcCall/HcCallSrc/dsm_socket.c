#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#include "dsm_datatype.h"
#include "dsm_socket.h"
#include "dsm_http.h"
#include "MMI_include.h"
//#include "mmidsmunet.h"
#include "mrp_unet.h"
#include "TimerEvents.h"

static uint32 sSocketNum;
DSM_SOCKET sSocketPools[DSM_SOCKET_MAX_NUM];

DSM_SOCKEVTDATA EvtData;

extern void StartTimer(U16 timerid, U32 delay, FuncPtr funcPtr);
extern void StopTimer(uint16 timerid);
extern int32 mr_check_connect(int32 param);


/********************************************************************
Function:		SocketAllocate
Description: 	
Input:		void
Output:
Return:
Notice:
*********************************************************************/
static PDSM_SOCKET SocketAllocate(void)
{
	int32 i;
	
	for (i = 0; i < DSM_SOCKET_MAX_NUM; i++)
	{
		if (sSocketPools[i].socketid < 0)
		{
			return &sSocketPools[i];
		}
	}
	
	return NULL;
}


/********************************************************************
Function:		SocketRecv
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static void SocketRecv(PDSM_SOCKET socket)
{
	int32 recvLen =0;
	int32 remainLen;

	if(!socket)
	{
		return;
	}

	remainLen = DSM_SOCKET_RECVBUFFER_SIZE - socket->dataLen;	
	if( remainLen > 0 )
	{
		recvLen = unet_Soc_Recv(socket->socketid, (char*)(socket->recvBuffer +   socket->dataLen), remainLen);
		if (recvLen > 0)
		{	
			socket->waittime = 0;
			socket->dataLen +=recvLen;
			remainLen -= recvLen;	
		}
		else if(recvLen == 0)
		{
			socket->waittime++;
			if (socket->waittime > 30*(1000/DSM_SOCKET_SELECT_INTERNAL))
			{
				socket->waittime = 0;
				socket->fnEvent(socket, DSM_SOCKET_EVENT_NODATATIMEOUT, NULL);
			}
		}
		else if (recvLen < 0)
		{
			DSM_SocketClosed(socket, DSM_SOCKET_EVENT_ERROR);
			return;
		}			
	}
	
	// respond 
	if ( socket->dataLen > 0)
	{	
		int32 eatLen =0;
		
		EvtData.buffer = (char* )socket->recvBuffer;
		EvtData.size =  socket->dataLen;
		
		eatLen = socket->fnEvent(socket, DSM_SOCKET_EVENT_RECVDATA, &EvtData);

		if (DSM_SocketIsNull(socket))//maybe the socket has been closed
		{
			return;
		}

		remainLen += eatLen;
		socket->dataLen -= eatLen;

		// memery move 
		if (eatLen > 0 && socket->recvBuffer)
		{				
			memmove((char *)socket->recvBuffer, socket->recvBuffer + eatLen,  socket->dataLen);
			memset((char *)socket->recvBuffer +  socket->dataLen, 0, remainLen);
		}
	}	
}


/********************************************************************
Function:		SocketTCPSend
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static int32 SocketTCPSend(PDSM_SOCKET socket, char* buffer, uint32 size)
{
	int32 sendbytes = 0;
	int32 sendTotal = 0;
	char* pointer = buffer;
	
	if(!socket || !buffer)
	{
		return MR_FAILED;
	}
	
	while (size >0 &&(sendbytes = unet_Soc_Send(socket->socketid, pointer,size))>0)
	{
		size -= sendbytes;
		pointer += sendbytes;
		sendTotal += sendbytes;
	}

	if (sendbytes < 0)
	{
		DSM_SocketClosed(socket,DSM_SOCKET_EVENT_ERROR);
		return MR_FAILED;
	}
	
	return sendTotal;
}


/********************************************************************
Function:		SocketSelect
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static void SocketSelect(int32 data)
{
	int32 i, r, sendbytes;
	PDSM_SOCKET socket;

	for (i = 0; i<DSM_SOCKET_MAX_NUM; i++)
	{
		socket = &sSocketPools[i];

		if (socket->socketid <0 
			||DSM_SOCKET_STATE_CREATED == socket->socketstate
			||DSM_SOCKET_STATE_CLOSED == socket->socketstate )
		{
			continue;
		}

		if ( DSM_SOCKET_STATE_CONNECTING == socket->socketstate)
		{
			r = unet_Soc_State(socket->socketid);
			if (r == MR_SUCCESS)
			{
				socket->waittime = 0;
				socket->socketstate = DSM_SOCKET_STATE_CONNECTED;
				socket->fnEvent(socket, DSM_SOCKET_EVENT_CONNECTED,NULL);
			}
			else if ( MR_FAILED == r)
			{
				socket->waittime = 0;
				DSM_SocketClosed( socket, DSM_SOCKET_EVENT_CONNECTFAILED); 
			}
			else if (MR_WAITING == r)
			{
				socket->waittime++;
				if (socket->waittime > 30*(1000/DSM_SOCKET_SELECT_INTERNAL))
				{
					socket->waittime = 0;
					DSM_SocketClosed(socket, DSM_SOCKET_EVENT_CONNECTFAILED);
				}
			}
			continue;
		}

		if (socket->end > socket->begin)
		{
			sendbytes = SocketTCPSend(socket, (char *)(socket->sendbuff + socket->begin), socket->end - socket->begin);	
			if (sendbytes < 0 )
			{
				break;
			}
			socket->begin += sendbytes;
		}
			
		socket->fnRecv(socket);
	}
}


/********************************************************************
Function:		SocketAppendTCPData
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static int32 SocketAppendTCPData(PDSM_SOCKET socket, uint8* buffer, uint32 size)
{
	if(!socket || !buffer)
	{
		return FALSE;
	}
	
	if (socket->end + size > DSM_SOCKET_SENDBUFFER_SIZE && socket->begin > 0  )
	{
		if ( socket->end  > socket->begin )
		{
			memmove(socket->sendbuff, socket->sendbuff +socket->begin, socket->end - socket->begin);
			socket->end -= socket->begin;
			socket->begin =0;
		}
		else
		{
			socket->begin = 0;
			socket->end = 0;
		}		
	}

	if (socket->end + size <= DSM_SOCKET_SENDBUFFER_SIZE)
	{
		memcpy(socket->sendbuff + socket->end,buffer,size);
		socket->end += size;
		return TRUE;
	}
	
	return FALSE;
}



/********************************************************************
Function:		DSM_SocketInitialize
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void DSM_SocketInitialize(void)
{
	int32 i;
	
	sSocketNum = 0;
	for ( i = 0; i < DSM_SOCKET_MAX_NUM; i++)
	{
		sSocketPools[i].socketid = -1;
	}

	memset(&EvtData, 0, sizeof(DSM_SOCKEVTDATA));
}


/********************************************************************
Function:		DSM_SocketTerminal
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void DSM_SocketTerminal(void)
{
	DSM_SocketCloseAll(DSM_SOCKET_EVENT_CLOSED);
}


static void DSM_Socket_TimerCb(void)
{
	SocketSelect(0);
	StartTimer(HCCALL_SYS_TIMER, DSM_SOCKET_SELECT_INTERNAL, DSM_Socket_TimerCb);
}


/********************************************************************
Function:		DSM_SocketCreate
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
PDSM_SOCKET DSM_SocketCreate(FN_DSM_SOCKEVENT fnEvent, int32 userdata, int32 appHandle)
{
	PDSM_SOCKET socket = SocketAllocate();
	if (!socket)
	{
		return NULL;
	}

	socket->socketstate = DSM_SOCKET_STATE_CREATED;
	socket->end = socket->begin = 0;
	socket->userdata =  userdata;
	socket->fnEvent = fnEvent;
	
	socket->fnRecv = SocketRecv;
	socket->socketid = unet_Soc_Create(MR_SOCK_STREAM, MR_IPPROTO_TCP, appHandle);
	
	if (socket->socketid < 0)
	{
		fnEvent(socket, DSM_SOCKET_EVENT_ERROR, NULL);
		return NULL;
	}
	
	if (++ sSocketNum == 1)
	{
		StartTimer(HCCALL_SYS_TIMER, DSM_SOCKET_SELECT_INTERNAL, DSM_Socket_TimerCb);
	}
	return socket;
}


/********************************************************************
Function:		DSM_SocketConnect
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_SocketConnect(PDSM_SOCKET socket, uint32 ip, uint16 port, int32 block)
{
	int32 r;

	if (DSM_SocketIsNull(socket))
	{
		return FALSE;
	}

#ifdef  WIN32
	if((r = unet_Soc_Connect(socket->socketid, (uint32)ip, (uint16)port, MR_SOCKET_NONBLOCK)) == MR_FAILED)	
#else
	if((r = unet_Soc_Connect(socket->socketid, (uint32)ip, (uint16)port, MR_SOCKET_NONBLOCK)) == MR_FAILED)	
#endif
	{
		DSM_SocketClosed(socket, DSM_SOCKET_EVENT_CONNECTFAILED);
		return FALSE;
	}

	if ( r == MR_SUCCESS)
	{
		socket->socketstate = DSM_SOCKET_STATE_CONNECTED;
		socket->fnEvent(socket, DSM_SOCKET_EVENT_CONNECTED, NULL);
	}
	else
	{
		socket->socketstate = DSM_SOCKET_STATE_CONNECTING;
	}
	
	return TRUE;
}


/********************************************************************
Function:		DSM_SocketSend
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_SocketSend(PDSM_SOCKET socket, uint8* buffer, int32 size)
{
	int32 sendbytes=0;
	
	if ( !socket || socket->socketid < 0 || !buffer || size <=0 )
	{
		return FALSE;
	}
	
	if ( DSM_SOCKET_STATE_CONNECTED == socket->socketstate && socket->begin == socket->end )	
	{
		if ((sendbytes = SocketTCPSend(socket, (char *)buffer, size))<0)
		{
			return FALSE;
		}
	}
	
	if (sendbytes < size)
	{
		return SocketAppendTCPData(socket, buffer + sendbytes, size - sendbytes);
	}
	
	return TRUE;
}


/********************************************************************
Function:		DSM_SocketIsNull
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_SocketIsNull(PDSM_SOCKET socket)
{
	return   (!socket || socket->socketid<0);
}


/********************************************************************
Function:		DSM_SocketClosed
Description: 	Close a socket
Input:		socket: the socket handle to closed
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_SocketClosed(PDSM_SOCKET socket, DSM_SOCKEVENT evt)
{
	if (socket && socket->socketid >= 0 )
	{
		if (--sSocketNum == 0 )
		{
			StopTimer(HCCALL_SYS_TIMER);
		}
		unet_Soc_Close(socket->socketid);

		socket->socketid = -1;
		socket->socketstate = DSM_SOCKET_STATE_CLOSED;
		socket->dataLen = 0;
		socket->waittime = 0;
		
		socket->fnEvent(socket, evt, NULL);
		return  TRUE;
	}
	
	return FALSE;
}


/********************************************************************
Function:		DSM_SocketCloseAll
Description: 	Close all the sockets
Input:		
Output:
Return:
Notice:
*********************************************************************/
void DSM_SocketCloseAll(DSM_SOCKEVENT evt)
{
	int32 i;
	
	for (i = 0; i < DSM_SOCKET_MAX_NUM; i++)
	{
		if(sSocketPools[i].socketid < 0)
		{
			continue;
		}
		
		DSM_SocketClosed(&sSocketPools[i],evt);
	}
} 

#endif
