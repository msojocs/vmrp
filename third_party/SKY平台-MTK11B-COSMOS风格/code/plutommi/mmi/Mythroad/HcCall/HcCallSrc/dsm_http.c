#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#include "dsm_http.h"
#include "stdarg.h"
#include <string.h>
 
static const uint8 sHttpMethods[][5]=
{
	"GET",
	"POST"
};


static DSM_HTTPDATA sHttpDataPools[DSM_SOCKET_MAX_NUM] = {0};


/********************************************************************
Function:		DSM_Find_HttpHeaderEnding
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static int DSM_Find_HttpHeaderEnding(PDSM_HTTPDATA pHttpData)
{
	if ((pHttpData->hdrSize >= 4)
		&&(pHttpData->header[pHttpData->hdrSize - 4] == '\r')
		&&(pHttpData->header[pHttpData->hdrSize - 3] == '\n')
		&&(pHttpData->header[pHttpData->hdrSize - 2] == '\r')
		&&(pHttpData->header[pHttpData->hdrSize - 1] == '\n'))
	{
		return TRUE;
	}

	return FALSE;
}


/********************************************************************
Function:		DSM_Reset_Http
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static void DSM_Reset_Http(PDSM_HTTPDATA pHttpData)
{
	pHttpData->contentLength = 0;
	pHttpData->hdrSize = 0;
	pHttpData->responseCode = 0;
}


/********************************************************************
Function:		DSM_Handle_ResponseEnd
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static void DSM_Handle_ResponseEnd(PDSM_SOCKET socket, PDSM_HTTPDATA pHttpData)
{
	pHttpData->fnEvent(pHttpData->userData, DSM_HTTPEVT_RESPONSE_END, NULL);
	DSM_Reset_Http(pHttpData);
}


/********************************************************************
Function:		DSM_host
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
const char* DSM_host(const char* url, uint16*port, uint16 def)
{
	static char host[DSM_MAX_HOSTNAME_LEN];
	int32 i, j;

	if(port) 
	{
		*port = def;
	}
	
	for(i = j = 0; url[i]; i++)
	{
		if(url[i] == ':' && url[i+1] == '/' && url[i+2] == '/')
		{
			i+=3;
			while(url[i] && url[i]!=':' && url[i]!='/') 
			{
				host[j++] = url[i++];
			}
			
			if(port && url[i] == ':') 
			{
				*port = atoi(url+i+1);
			}
			break;
		}
	}

	host[j] = 0;
	return host;	
}


/********************************************************************
Function:		DSM_HttpDestroy
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static void DSM_HttpDestroy(PDSM_SOCKET socket)
{
	PDSM_HTTPDATA pHttpData = (PDSM_HTTPDATA)socket->userdata;

	memset(pHttpData, 0, sizeof(DSM_HTTPDATA));
	pHttpData->id = -1;
}

/********************************************************************
Function:		DSM_Str2Low
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void DSM_Str2Low(char* pstr, int32 len)
{
	int i = 0;

	for(; i<len; i++)
	{
		if(pstr[i] >= 'A' && pstr[i] <= 'Z') 
		{
			pstr[i] += 'a' - 'A';
		}
	}
}


/********************************************************************
Function:		DSM_HttpHandleResponseData
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static int32 DSM_HttpHandleResponseData(PDSM_SOCKET socket, PDSM_SOCKEVTDATA data)
{
	int32 i;
	const char* len;
	PDSM_HTTPDATA pHttpData = (PDSM_HTTPDATA)socket->userdata;
	int32 dataSize = data->size;
	int32 remainbytes = 0;

	if (pHttpData->responseCode == 0)
	{
		for (i = 0; i < data->size; i++)
		{
			if (pHttpData->hdrSize >= DSM_HTTP_HEADER_SIZE)
			{
				DSM_HttpClose(socket, DSM_HTTPEVT_ERROR);
				return dataSize;
			}

			pHttpData->header[pHttpData->hdrSize++] = data->buffer[i];
			
			if (!DSM_Find_HttpHeaderEnding(pHttpData))
			{
				continue;
			}
			pHttpData->header[pHttpData->hdrSize] =0;

			DSM_Str2Low((char*)pHttpData->header, strlen((const char*)pHttpData->header));

			pHttpData->responseCode = DSM_HttpGetResponseCode(socket);
			if (pHttpData->responseCode == 200)
			{
				const char* server = NULL;
			
				server = DSM_HttpGetResponseField(socket, DSM_HTTP_FIELD_SERVER);
				//if (server)
				{
					//if (!strstr(server, "cgss"))
					if(server == NULL || !strstr(server, "cgss"))//huangsunbo 20120202 解决cmwap 无法联网问题
					{
						pHttpData->contentLength = 0; 
						pHttpData->hdrSize = 0; 
						pHttpData->responseCode = 0;
						DSM_HttpSend(socket, POST, (uint8*)pHttpData->msg, pHttpData->msgsize);
						return dataSize;
					}
				}
			}
		
			if ( pHttpData->fnEvent(pHttpData->userData, DSM_HTTPEVT_RESPONSE_HEADER, NULL) )
			{
				return dataSize;
			}
	
			if (NULL == (len = DSM_HttpGetResponseField(socket, DSM_HTTP_FIELD_CONTENTLENGTH)))
			{				
				DSM_HttpClose(socket, DSM_HTTPEVT_ERROR);
				return dataSize;
			}
			pHttpData->contentLength = atoi(len);
			if (pHttpData->contentLength == 0)
			{
				DSM_Handle_ResponseEnd(socket, pHttpData);
			}
			i++;
			break;
		}

		data->buffer += i;
		data->size -= i;
	}

	remainbytes = data->size;	
	if (data->size > 0)
	{
		pHttpData->fnEvent(pHttpData->userData, DSM_HTTPEVT_RESPONSE_DATA, data);
		pHttpData->contentLength -= (remainbytes - data->size);
		if (pHttpData->contentLength == 0)
		{
			DSM_Handle_ResponseEnd(socket, pHttpData);
		}
	}
	
	return (dataSize - data->size);
}


/********************************************************************
Function:		DSM_HttpHandleSocketEvents
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
static int32 DSM_HttpHandleSocketEvents(PDSM_SOCKET socket, int32 evt, PDSM_SOCKEVTDATA data)
{
	PDSM_HTTPDATA pHttpData = (PDSM_HTTPDATA)socket->userdata;
	FN_DSM_HTTPEVENT fnEvent = pHttpData->fnEvent;

	switch(evt)
	{
	case DSM_SOCKET_EVENT_CONNECTED:
		{
			return fnEvent(pHttpData->userData, evt, data);
		}
	
	case DSM_SOCKET_EVENT_RECVDATA:
		{
			return DSM_HttpHandleResponseData(socket, data);
		}
	
	case DSM_SOCKET_EVENT_BUFFERFULL:
		break;

	case DSM_SOCKET_EVENT_NODATATIMEOUT:
		return fnEvent(pHttpData->userData, DSM_HTTPEVT_NODATATIMEOUT, data);

	case DSM_SOCKET_EVENT_CONNECTFAILED:
	case DSM_SOCKET_EVENT_ERROR:
	case DSM_SOCKET_EVENT_CLOSED:
		{
			int32 userData = pHttpData->userData;
			DSM_HttpDestroy(socket);
			return fnEvent(userData, evt, data);
		}
	}
	return 0;
}


/********************************************************************
Function:		SVR_HttpGetResponseField
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
const char* DSM_HttpGetResponseField(PDSM_SOCKET socket, const char* field)
{
	static char value[DSM_HTTP_FIELDVALUE_SIZE];
	int32 i = 0;
	char* pTmp = NULL;
	PDSM_HTTPDATA pHttpData = (PDSM_HTTPDATA)socket->userdata;

	strncpy(value,(const char*)field, (DSM_HTTP_FIELDVALUE_SIZE - 1));
	value[DSM_HTTP_FIELDVALUE_SIZE - 1] = 0;

	DSM_Str2Low((char*)value, strlen(value));

	pTmp = strstr((const char*)pHttpData->header, (const char*)value);
	if (NULL == pTmp)
	{
		return NULL;
	}

	pTmp += strlen((const char*)field);
	while( *pTmp == ' ' || *pTmp == ':' )
	{
		pTmp++;
	}

	while (!(pTmp[i] == '\r' && pTmp[i+1] == '\n'))
	{
		value[i] = pTmp[i];
		i++;
	}
	value[i] = 0;
	return (const char*)value;
}


/********************************************************************
Function:		DSM_HttpInitialize
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void DSM_HttpInitialize(void)
{
	int32 i;
	
	for ( i = 0; i < DSM_SOCKET_MAX_NUM; i++)
	{
		sHttpDataPools[i].id = -1;
	}
}


/********************************************************************
Function:		DSM_HttpTerminal
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
void DSM_HttpTerminal(void)
{
}


/********************************************************************
Function:		DSM_HttpAllocate
Description: 	
Input:		void
Output:
Return:
Notice:
*********************************************************************/
static PDSM_HTTPDATA DSM_HttpAllocate(void)
{
	int32 i;
	
	for (i = 0; i < DSM_SOCKET_MAX_NUM; i++)
	{
		if (sHttpDataPools[i].id < 0)
		{
			return &sHttpDataPools[i];
		}
	}
	
	return NULL;
}


/********************************************************************
Function:		SVR_HttpOpen
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
PDSM_SOCKET DSM_HttpOpen(uint32 ip, uint16 port, FN_DSM_HTTPEVENT fnEvent, int32 userData, int32 appHandle)
{
	PDSM_HTTPDATA pHttpData;
	PDSM_SOCKET socket;

	pHttpData = DSM_HttpAllocate();
	if (NULL == pHttpData)
	{
		return NULL;
	}

	pHttpData->fnEvent = fnEvent;
	pHttpData->contentLength = 0;
	pHttpData->responseCode = 0;
	pHttpData->hdrSize = 0;
	pHttpData->userData = userData;

	socket = DSM_SocketCreate((FN_DSM_SOCKEVENT)DSM_HttpHandleSocketEvents, (int32)pHttpData, appHandle);
	if (NULL != socket)
	{
		if (DSM_SocketConnect(socket,ip,port,FALSE))
		{
			return socket;
		}
	}

	return NULL;
}


/********************************************************************
Function:		DSM_HttpClose
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_HttpClose(PDSM_SOCKET socket, int32 evt)
{
	return  DSM_SocketClosed(socket, evt);
}


/********************************************************************
Function:		SVR_HttpFormatHeader
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
const char* DSM_HttpFormatHeader(uint32 *size, DSM_HTTPMETHOD method, const char* url, ... )
{
	static char header[DSM_HTTP_HEADER_SIZE];
	uint32 hdrsize;
	const char* field;
	const char* value;
	va_list args;

	va_start(args, url);
	memset(header, 0, DSM_HTTP_HEADER_SIZE);

	sprintf((char*)header, DSM_HTTP_HEADER_FORMAT, url);

	hdrsize = strlen((const char*)header);

	while (NULL != (field = va_arg(args, const char*)))
	{
		strcpy((char*)(header + hdrsize), field);
		hdrsize += strlen(field);
		header[hdrsize++] = ':';

		value = va_arg(args, const char*);
		strcpy((char*)header + hdrsize, (const char*)value);
		hdrsize += strlen((const char*)value);

		header[hdrsize++] = '\r';
		header[hdrsize++] = '\n';
	}

	header[hdrsize++] = '\r';
	header[hdrsize++] = '\n';

	va_end(args);
	*size = hdrsize;
	
	return (const char*)header;
}


/********************************************************************
Function:		DSM_HttpSend
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_HttpSend(PDSM_SOCKET socket, DSM_HTTPMETHOD method, uint8 * buffer, uint32 size)
{
	if (!DSM_SocketSend(socket, buffer, size))
	{
		return FALSE;
	}

	return TRUE;
}



/********************************************************************
Function:		DSM_HttpPost
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_HttpPost(PDSM_SOCKET socket, const char* url, const char* host, const char* buffer, uint32 size)
{
	const char* pHeader;
	uint32 hdrsize;
	uint8 str_host[120];
	uint8 bodylen[25] = {0};
	PDSM_HTTPDATA pHttpData = (PDSM_HTTPDATA)socket->userdata;
	
	sprintf((char*)str_host, "%s",host);
	sprintf((char*)bodylen, "%d",size);
	
	pHeader = DSM_HttpFormatHeader(&hdrsize, POST, url,
		DSM_HTTP_FIELD_HOST, str_host,
		DSM_HTTP_FIELD_XONLINE_HOST, str_host,
		DSM_HTTP_FIELD_CONTENTLENGTH,bodylen ,
		DSM_HTTP_FIELD_CONTENTTYPE, DSM_HTTP_VALUE_CONTENTTYPE,
		DSM_HTTP_FIELD_ACCEPT, DSM_HTTP_VALUE_ACCEPT,
		DSM_HTTP_FIELD_UA,DSM_HTTP_VALUE_UA,
		NULL);

	sprintf((char*)pHttpData->msg, "%s%s", pHeader, buffer);
	pHttpData->msgsize= hdrsize+size;

	if (socket->begin + DSM_SOCKET_SENDBUFFER_SIZE - socket->end < hdrsize + size)
	{
		return FALSE;
	}
	if (!DSM_HttpSend(socket, POST,(uint8*)pHeader, hdrsize))
	{
		return FALSE;
	}
	if (size >0 &&!DSM_SocketSend(socket, (uint8*)buffer,size))
	{
		return FALSE;
	}
	return TRUE;
}


/********************************************************************
Function:		DSM_HttpGetResponseCode
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
int32 DSM_HttpGetResponseCode(PDSM_SOCKET socket)
{
	PDSM_HTTPDATA pHttpData = (PDSM_HTTPDATA)socket->userdata;
	return atoi((const char*)(pHttpData->header + DSM_HTTP_RESPONSECODE_OFFSET));
}



/********************************************************************
Function:		DSM_Read_Dword
Description: 
Input:		
Output:
Return:
Notice:
*********************************************************************/
uint32 DSM_Read_Dword(uint8 * pData)
{
	uint32 ret = 0;
	
	ret = ((pData[0] & 0xFF) << 24 ) |
			((pData[1] & 0xFF) << 16 ) |
			((pData[2] & 0xFF) <<  8 ) |
			((pData[3] & 0xFF));
	
	return (ret);
}

#endif
