
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "./header/types.h"

enum {
    MR_SOCK_STREAM,
    MR_SOCK_DGRAM
};

enum {
    MR_IPPROTO_TCP,
    MR_IPPROTO_UDP
};

enum {
    MR_SOCKET_BLOCK,
    MR_SOCKET_NONBLOCK
};

/*
   MR_SUCCESS 成功
   MR_FAILED 失败
   MR_WAITING 使用异步方式进行连接，应用需要轮询该socket的状态以获知连接状况 
*/
int32 my_connect(int32 s, int32 ip, uint16 port, int32 type) {
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = ip;  //inet_addr("127.0.0.1");
    clientService.sin_port = htons(port);

    if (connect((SOCKET)s, (SOCKADDR *)&clientService, sizeof(clientService)) != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

/*
 >=0 返回的Socket句柄 
   MR_FAILED 失败 
*/
int32 my_socket(int32 type, int32 protocol) {
    type = (type == MR_SOCK_STREAM) ? SOCK_STREAM : SOCK_DGRAM;
    protocol = (protocol == MR_IPPROTO_TCP) ? IPPROTO_TCP : IPPROTO_UDP;
    SOCKET sock = socket(AF_INET, type, protocol);
    if (sock == INVALID_SOCKET) {
        return MR_FAILED;
    }
    return (int)sock;
}

int32 my_closeSocket(int32 s) {
    if (shutdown((SOCKET)s, SD_BOTH) != 0) {
        return MR_FAILED;
    }
    if (closesocket((SOCKET)s) != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int32 my_closeNetwork(void) {
    WSACleanup();
    return MR_SUCCESS;
}

/*  
   MR_SUCCESS 同步模式，初始化成功，不再调用cb
   MR_FAILED （立即感知的）失败，不再调用cb
   MR_WAITING 使用回调函数通知引擎初始化结果 
*/
int32 my_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode) {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return MR_FAILED;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        my_closeNetwork();
        return MR_FAILED;
    }
    return MR_SUCCESS;
}
/*
   MR_FAILED （立即感知的）失败，不再调用cb
   MR_WAITING 使用回调函数通知引擎获取IP的结果
   其他值 同步模式，立即返回的IP地址，不再调用cb 
*/
int32 my_getHostByName(const char *name, MR_GET_HOST_CB cb) {
    struct hostent *remoteHost = gethostbyname(name);
    if (remoteHost != NULL) {
        if (remoteHost->h_addrtype == AF_INET) {
            if (remoteHost->h_addr_list[0] != NULL) {
                struct in_addr addr;
                addr.s_addr = *(u_long *)remoteHost->h_addr_list[0];
                // printf("%d\n", addr.S_un.S_addr);
                // printf("%s\n", inet_ntoa(addr));
                return addr.S_un.S_addr;
            }
        }
    }
    return MR_FAILED;
}

/*
   >=0 实际发送的数据字节个数
   MR_FAILED Socket已经被关闭或遇到了无法修复的错误。 
*/
int32 my_send(int32 s, const char *buf, int len) {
    int32 ret = send((SOCKET)s, buf, len, 0);
    if (ret == SOCKET_ERROR) {
        return MR_FAILED;
    }
    return ret;
}

/*
   >=0的整数 实际接收的数据字节个数
   MR_FAILED Socket已经被关闭或遇到了无法修复的错误。 
*/
int32 my_recv(int32 s, char *buf, int len) {
    int32 ret = recv((SOCKET)s, buf, len, 0);
    if (ret == SOCKET_ERROR) {
        return MR_FAILED;
    }
    return ret;
}

int test() {
    my_initNetwork(NULL, "cmnet");

    // gcc -Wall a.c -m32 -lws2_32 && ./a.exe
    int ip = my_getHostByName("qq.com", NULL);
    printf("ip:0x%X\n", ip);

    int sh = my_socket(MR_SOCK_STREAM, MR_IPPROTO_TCP);
    printf("sh:0x%X\n", sh);

    int ret = my_connect(sh, ip, 80, MR_SOCKET_NONBLOCK);
    printf("ret:%d\n", ret);

    char *sendData = "GET / HTTP/1.1\r\nHost: qq.com\r\nAccept: */*\r\nConnection: close\r\n\r\n";
    ret = my_send(sh, sendData, strlen(sendData));
    printf("len:%d, ret:%d\n", strlen(sendData), ret);

    do {
        char buf[1024 * 1024];
        ret = my_recv(sh, buf, sizeof(buf));
        if (ret > 0) {
            buf[ret] = 0;
            printf("Bytes received: %d, %s\n", ret, buf);
        } else if (ret == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (ret > 0);

    my_closeNetwork();
    return 0;
}