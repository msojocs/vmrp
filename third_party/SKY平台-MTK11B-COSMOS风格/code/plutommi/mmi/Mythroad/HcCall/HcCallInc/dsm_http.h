#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#ifndef  _DSM_HTTP_H_
#define _DSM_HTTP_H_

#include "dsm_socket.h"

#ifndef	DSM_HTTP_PORT
#define	DSM_HTTP_PORT   80
#endif

#ifndef	DSM_HTTP_HEADER_SIZE 
#define	DSM_HTTP_HEADER_SIZE 1024
#endif

#ifndef  DSM_HTTP_FIELDVALUE_SIZE
#define	DSM_HTTP_FIELDVALUE_SIZE 128
#endif
#define	DSM_HTTP_RESPONSECODE_OFFSET 9

#define 	DSM_HTTP_HEADER_FORMAT "POST %s HTTP/1.1\r\n"

#define	DSM_HTTP_FIELD_CONTENTLENGTH "Content-Length"
#define	DSM_HTTP_FIELD_SERVER "SkyId"
#define	DSM_HTTP_FIELD_LAST_MODIFIED				"Last-Modified" 
#define	DSM_HTTP_VALUE_CONTENTLENGTH " 0"
#define	DSM_HTTP_FIELD_HOST "Host"

#define	DSM_HTTP_FIELD_XONLINE_HOST "X-Online-Host"

#define	DSM_HTTP_FIELD_UA "User-Agent"
#define	DSM_HTTP_VALUE_UA " Nokia6681/2.0 (3.10.6) SymbianOS/8.0 Series60/2.6 Profile/MIDP-2.0 Configuration/CLDC-1.1"
#define	DSM_HTTP_FIELD_ACCEPT "Accept"
#define	DSM_HTTP_VALUE_ACCEPT " */*"
#define	DSM_HTTP_FIELD_CONTENTTYPE "Content-Type"
#define	DSM_HTTP_VALUE_CONTENTTYPE "application/x-www-form-urlencoded"
#define	DSM_HTTP_FIELD_CONNECTION "Connection"
#define	DSM_HTTP_VALUE_CONNECTION_KEEPALIVE "Keep-Alive"
#define	DSM_HTTP_VALUE_CONNECTION_CLOSED "Closed"
#define	DSM_HTTP_FIELD_PROXYCONNECTION "Proxy-Connection"
#define	DSM_HTTP_FIELD_RANGE "Range"
#define	DSM_HTTP_VALUE_RANGE " bytes=0-"

#define	DSM_MAX_HOSTNAME_LEN   112


typedef enum DSM_HTTP_METHOD
{
	GET =0,
	POST 
}DSM_HTTPMETHOD;


typedef enum DSM_HTTP_EVENT
{
	DSM_HTTPEVT_CONNECTED = DSM_SOCKET_EVENT_CONNECTED,
	DSM_HTTPEVT_CONNECTFAILED = DSM_SOCKET_EVENT_CONNECTFAILED,
	DSM_HTTPEVT_ERROR = DSM_SOCKET_EVENT_ERROR,
	DSM_HTTPEVT_CLOSED = DSM_SOCKET_EVENT_CLOSED,
	DSM_HTTPEVT_RESPONSE_HEADER,
	DSM_HTTPEVT_RESPONSE_DATA,
	DSM_HTTPEVT_RESPONSE_END,
	DSM_HTTPEVT_NODATATIMEOUT
}DSM_HTTPEVENT;

typedef int32 (*FN_DSM_HTTPEVENT)(int32 userData, int32 evt, void* data);

typedef struct DSM_HttpData
{
	int32 id;
	FN_DSM_HTTPEVENT fnEvent;
	uint32 responseCode;
	int32 contentLength;
	uint32 hdrSize;
	int32 userData;
	uint8 header[DSM_HTTP_HEADER_SIZE];

	uint32 msgsize;
	uint8 msg[1024];
}DSM_HTTPDATA, *PDSM_HTTPDATA;


void DSM_HttpInitialize(void);
void DSM_HttpTerminal(void);

PDSM_SOCKET DSM_HttpOpen(uint32 ip, uint16 port, FN_DSM_HTTPEVENT fnEvent, int32 userData, int32 appHandle);
int32 DSM_HttpClose(PDSM_SOCKET socket, int32 evt );
const char* DSM_HttpFormatHeader(uint32* size, DSM_HTTPMETHOD method, const char* url,...);
int32 DSM_HttpSend(PDSM_SOCKET socket, DSM_HTTPMETHOD method, uint8 * buffer, uint32 size);
int32 DSM_HttpPost(PDSM_SOCKET socket, const char* url, const char* host, const char* buffer, uint32 size);
int32 DSM_HttpGetResponseCode(PDSM_SOCKET socket);
const char* DSM_HttpGetResponseField(PDSM_SOCKET socket, const char* field);
uint32 DSM_Read_Dword(uint8 * pData);


#endif
#endif
