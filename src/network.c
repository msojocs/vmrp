
#ifndef _MSC_VER
#include <pthread.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "./include/network.h"
#include "./include/posix_sockets.h"
#include "include/types.h"

// #define NETWORK_SUPPORT

#if defined(__EMSCRIPTEN__) && defined(NETWORK_SUPPORT)
#include <emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/threading.h>

EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;

EMSCRIPTEN_WEBSOCKET_T emscripten_init_websocket_to_posix_socket_bridge(const char* bridgeUrl);
#endif

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

typedef struct {
    SOCKET_T s;
    uint32_t sendCounter;
    int32_t realState;  // 真正的连接状态
    int32_t state;      // cmwap模式下是一个伪状态，cmnet模式下与realState的值始终相同
    int32_t cmwapMode;  // Socket 创建时的网络模式；不能被后续 mr_initNetwork(CMNET) 污染
    int32_t cmwapProxyAck;  // cmwap模式：真实连接建立后需要返回伪造的代理200响应
} mSocket;

static int isCMWAP = FALSE;  // 当前 mr_initNetwork 模式，只用于之后新建的 socket
static struct rb_root sockets = RB_ROOT;

#define VMRP_DNS_MAP_MAX 32
#define VMRP_DNS_NAME_MAX 255

typedef struct {
    char original[VMRP_DNS_NAME_MAX + 1];
    char fake[VMRP_DNS_NAME_MAX + 1];
} DnsMapEntry;

static DnsMapEntry dnsMap[VMRP_DNS_MAP_MAX];
static int dnsMapCount = 0;

static char* trimSpaces(char* s) {
    char* end;
    while (*s && isspace((unsigned char)*s)) s++;
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1))) end--;
    *end = '\0';
    return s;
}

static int copyNormalizedDomain(char* dst, size_t dstSize, const char* src) {
    size_t len;
    if (!dst || dstSize == 0 || !src) return MR_FAILED;
    while (*src && isspace((unsigned char)*src)) src++;
    len = strlen(src);
    while (len > 0 && isspace((unsigned char)src[len - 1])) len--;
    while (len > 0 && src[len - 1] == '.') len--;
    if (len == 0 || len >= dstSize) return MR_FAILED;
    for (size_t i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)src[i];
        if (isspace(ch)) return MR_FAILED;
        dst[i] = (char)tolower(ch);
    }
    dst[len] = '\0';
    return MR_SUCCESS;
}

static int parseDnsMapEntry(char* entry) {
    char* sep;
    char* original;
    char* fake;
    char originalNorm[VMRP_DNS_NAME_MAX + 1];
    char fakeNorm[VMRP_DNS_NAME_MAX + 1];

    entry = trimSpaces(entry);
    if (!*entry) return MR_SUCCESS;

    sep = strstr(entry, "->");
    if (sep) {
        *sep = '\0';
        fake = sep + 2;
    } else {
        sep = strchr(entry, '=');
        if (!sep) return MR_FAILED;
        *sep = '\0';
        fake = sep + 1;
    }

    original = trimSpaces(entry);
    fake = trimSpaces(fake);
    if (copyNormalizedDomain(originalNorm, sizeof(originalNorm), original) != MR_SUCCESS ||
        copyNormalizedDomain(fakeNorm, sizeof(fakeNorm), fake) != MR_SUCCESS) {
        return MR_FAILED;
    }
    if (dnsMapCount >= VMRP_DNS_MAP_MAX) return MR_FAILED;

    strcpy(dnsMap[dnsMapCount].original, originalNorm);
    strcpy(dnsMap[dnsMapCount].fake, fakeNorm);
    dnsMapCount++;
    return MR_SUCCESS;
}

int32 my_configureDnsMap(const char* map) {
    char* buf;
    char* entry;
    char* next;

    dnsMapCount = 0;
    if (!map || !*map) return MR_SUCCESS;

    buf = malloc(strlen(map) + 1);
    if (!buf) return MR_FAILED;
    strcpy(buf, map);

    entry = buf;
    while (entry && *entry) {
        next = entry;
        while (*next && *next != ',' && *next != ';' && *next != '\n' && *next != '\r') {
            next++;
        }
        if (*next) {
            *next = '\0';
            next++;
        } else {
            next = NULL;
        }
        if (parseDnsMapEntry(entry) != MR_SUCCESS) {
            free(buf);
            dnsMapCount = 0;
            return MR_FAILED;
        }
        entry = next;
    }

    free(buf);
    return MR_SUCCESS;
}

static const char* getDnsLookupName(const char* name, char* mappedName, size_t mappedNameSize) {
    char normalized[VMRP_DNS_NAME_MAX + 1];
    if (copyNormalizedDomain(normalized, sizeof(normalized), name) != MR_SUCCESS) {
        return name;
    }
    for (int i = dnsMapCount - 1; i >= 0; i--) {
        if (strcmp(dnsMap[i].original, normalized) == 0) {
            snprintf(mappedName, mappedNameSize, "%s", dnsMap[i].fake);
            printf("dns map: %s -> %s\n", name, mappedName);
            return mappedName;
        }
    }
    return name;
}

/* 从 "host" 或 "host:port" 形式的字符串里提取 host 和 port。
 * h 指向起始位置，遇到 '\0'、'\r'、'\n'、' '、'/' 停止。 */
static int extractHostPort(const char* h, char* outHost, int outHostLen, uint16_t* outPort) {
    int i;
    for (i = 0; i < outHostLen - 1; i++) {
        if (*h == '\0' || *h == ':' || *h == '/' || *h == '\r' || *h == '\n' || *h == ' ') {
            break;
        }
        outHost[i] = *h;
        h++;
    }
    outHost[i] = '\0';
    if (i == 0) return -1;  // 没提取到任何内容

    if (*h == ':') {
        char port[6];
        h++;  // 跳过':'
        for (i = 0; i < (int)sizeof(port) - 1; i++) {
            if (*h == '\0' || *h == '/' || *h == '\r' || *h == '\n' || *h == ' ') {
                break;
            }
            port[i] = *h;
            h++;
        }
        port[i] = '\0';
        *outPort = (uint16_t)atoi(port);
    } else {
        *outPort = 80;
    }
    return 0;
}

/* 从 HTTP 请求数据中解析目标 host:port。
 * 支持三种格式：
 *   1) "CONNECT host:port HTTP/x.x"          — HTTPS 代理隧道
 *   2) "GET http://host:port/path HTTP/x.x"   — 绝对 URL
 *   3) "POST /path HTTP/x.x\r\nHost: host:port\r\n..."  — Host 头（netpay 走这个）
 * buf 是完整的 HTTP 请求数据（不只首行）。 */
static int parseHostPort(const char* buf, int bufLen, char* outHost, int outHostLen, uint16_t* outPort) {
    const char* h;

    // 格式 1: CONNECT host:port
    if (strncmp(buf, "CONNECT ", 8) == 0) {
        return extractHostPort(buf + 8, outHost, outHostLen, outPort);
    }

    // 格式 2: 绝对 URL (GET http://host/...)
    h = strstr(buf, "://");
    if (h != NULL && h < buf + bufLen) {
        return extractHostPort(h + 3, outHost, outHostLen, outPort);
    }

    // 格式 3: 从 Host 头提取（不区分大小写）
    // 在 buf 中逐行扫描，查找 "Host:" 头
    const char* p = buf;
    const char* end = buf + bufLen;
    while (p < end) {
        // 跳到下一行
        const char* lineEnd = p;
        while (lineEnd < end && *lineEnd != '\r' && *lineEnd != '\n') lineEnd++;

        int lineLen = (int)(lineEnd - p);
        if (lineLen >= 5 && strncasecmp(p, "Host:", 5) == 0) {
            const char* val = p + 5;
            while (val < lineEnd && *val == ' ') val++;  // 跳过空格
            return extractHostPort(val, outHost, outHostLen, outPort);
        }

        // 跳过 \r\n
        if (lineEnd < end && *lineEnd == '\r') lineEnd++;
        if (lineEnd < end && *lineEnd == '\n') lineEnd++;
        if (lineEnd == p) break;  // 防止死循环
        p = lineEnd;
    }

    return -1;
}

#ifndef _MSC_VER
typedef struct {
    pthread_t th;
    mSocket* s;
    uint32_t ip;
    uint16_t port;
} connectData_t;
#endif

static int32 my_connectSync(SOCKET_T s, int32 ip, uint16 port) {
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(port);
    clientService.sin_addr.s_addr = htonl(ip);  //inet_addr("127.0.0.1");

    printf("my_connect(fd:%d, '%s', %d)\n", (int)s, inet_ntoa(clientService.sin_addr), port);

    if (connect(s, (struct sockaddr*)&clientService, sizeof(clientService)) != 0) {
        printf("my_connect(0x%X) fail\n", ip);
        return MR_FAILED;
    }
    printf("my_connect(0x%X) suc\n", ip);
    return MR_SUCCESS;
}

#ifndef _MSC_VER
static void* my_connectAsync(void* arg) {
    connectData_t* data = (connectData_t*)arg;
    int32_t r = my_connectSync(data->s->s, data->ip, data->port);
    data->s->realState = r;
    if (!data->s->cmwapMode) {  // cmnet模式下保持相同的连接状态
        data->s->state = r;
    } else if (r == MR_SUCCESS) {
        data->s->cmwapProxyAck = 1;  // 触发伪造的代理200响应
    }
    free(data);
    return NULL;
}
#endif
/*
   MR_SUCCESS 成功
   MR_FAILED 失败
   MR_WAITING 使用异步方式进行连接，应用需要轮询该socket的状态以获知连接状况 

   IP地址,如果一个主机的IP地址为218.18.95.203，则值为218<<24 + 18<<16 + 95<<8 + 203= 0xda125fcb
*/
int32 my_connect(int32 s, int32 ip, uint16 port, int32 type) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* data = (mSocket*)obj->data;
    if (ip == 0x0A0000AC && data->cmwapMode) {
        // 10.0.0.172 是 CMWAP 代理地址，桌面端不存在该代理
        // 伪装连接成功，实际连接在 my_send 第一次发送时根据 CONNECT 头建立
        data->state = MR_SUCCESS;
        data->realState = MR_WAITING;
        return MR_SUCCESS;
    }
    printf("my_connect() type: %s\n", type == MR_SOCKET_BLOCK ? "block" : "async");
    if (type == MR_SOCKET_NONBLOCK) {
        connectData_t* d = malloc(sizeof(connectData_t));
        d->s = data;
        d->ip = ip;
        d->port = port;
        int ret = pthread_create(&d->th, NULL, my_connectAsync, d);
        if (ret != 0) {
            data->state = MR_FAILED;
            data->realState = MR_FAILED;
            return MR_FAILED;
        }
        return MR_WAITING;
    }
    return my_connectSync(data->s, ip, port);
#else
    return MR_FAILED;
#endif
}

/*
   MR_SUCCESS ： 连接成功
   MR_FAILED ： 连接失败
   MR_WAITING ： 连接中
   MR_IGNORE ： 不支持该功能
*/
int32 my_getSocketState(int32 s) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* p = ((mSocket*)obj->data);
    printf("my_getSocketState(%d): %d\n", s, p->state);
    return p->state;
#else
    return MR_FAILED;
#endif
}

static int32_t socketCounter = 0;
int32 my_hasOpenSockets(void) {
#ifdef NETWORK_SUPPORT
    return rb_first(&sockets) != NULL;
#else
    return FALSE;
#endif
}

int32 my_openSocketCount(void) {
#ifdef NETWORK_SUPPORT
    int32 count = 0;
    for (struct rb_node* node = rb_first(&sockets); node; node = rb_next(node))
        count++;
    return count;
#else
    return 0;
#endif
}

/*
 >=0 返回的Socket句柄 
   MR_FAILED 失败 
*/
int32 my_socket(int32 type, int32 protocol) {
#ifdef NETWORK_SUPPORT
    type = (type == MR_SOCK_STREAM) ? SOCK_STREAM : SOCK_DGRAM;
    protocol = (protocol == MR_IPPROTO_TCP) ? IPPROTO_TCP : IPPROTO_UDP;
    SOCKET_T sock = socket(AF_INET, type, protocol);
    if (sock == -1) {
        printf("my_socket() fail\n");
        return MR_FAILED;
    }
    socketCounter++;

    mSocket* data = malloc(sizeof(mSocket));
    data->s = sock;
    data->realState = MR_WAITING;
    data->state = MR_WAITING;
    data->sendCounter = 0;
    data->cmwapMode = isCMWAP;
    data->cmwapProxyAck = 0;

    uIntMap* obj = malloc(sizeof(uIntMap));
    obj->key = socketCounter;
    obj->data = (void*)data;
    uIntMap_insert(&sockets, obj);
    // 打印宿主 fd: 排查 fd 复用/串线问题时必须能把 guest socket id 对应到宿主 fd
    printf("my_socket(): s=%d fd=%d\n", socketCounter, (int)sock);
    return socketCounter;
#else
    return MR_FAILED;
#endif
}

int32 my_closeSocket(int32 s) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_delete(&sockets, (uint32_t)s);
    if (obj == NULL) {
        return MR_FAILED;
    }
    mSocket* data = (mSocket*)obj->data;
    SOCKET_T sock = data->s;
    free(data);
    free(obj);
    printf("my_closeSocket(s:%d): fd=%d\n", s, (int)sock);
    shutdown(sock, SHUTDOWN_BIDIRECTIONAL);
    if (CLOSE_SOCKET(sock) != 0) {
        return MR_FAILED;
    }
    return MR_SUCCESS;
#else
    return MR_FAILED;
#endif
}

int32 my_closeNetwork(void) {
#ifdef NETWORK_SUPPORT
    struct rb_node* p;
    while ((p = rb_first(&sockets)) != NULL) {
        uIntMap* obj = rb_entry(p, uIntMap, node);
        my_closeSocket((int32)obj->key);
    }
#ifdef WIN_PLAT
    WSACleanup();
#endif
    return MR_SUCCESS;
#else
    return MR_FAILED;
#endif
}

#ifndef _MSC_VER
typedef struct {
    MR_INIT_NETWORK_CB cb;
    void* userData;
    uc_engine* uc;
    pthread_t th;
} initNetworkAsyncData_t;
#endif

static int32 my_initNetworkSync(void) {
#ifdef WIN_PLAT
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
#elif defined(__EMSCRIPTEN__) && defined(NETWORK_SUPPORT)
    bridgeSocket = emscripten_init_websocket_to_posix_socket_bridge("ws://127.0.0.1:8888/socket");
    // Synchronously wait until connection has been established.
    uint16_t readyState = 0;
    do {
        emscripten_websocket_get_ready_state(bridgeSocket, &readyState);
        emscripten_thread_sleep(100);
        printf("readyState:%d\n", readyState);
    } while (readyState == 0);
#endif
    return MR_SUCCESS;
}

#ifndef _MSC_VER
static void* my_initNetworkAsync(void* arg) {
    initNetworkAsyncData_t* data = (initNetworkAsyncData_t*)arg;
    int32 r = my_initNetworkSync();
    printf("my_initNetworkAsync(): %d\n", r);
    /* cb/userData 装的是 guest 32 位地址(经指针类型转运),显式经 uintptr_t
     * 截断,语义与原先的直接窄化转换一致 */
    bridge_dsm_network_cb(data->uc, (uint32_t)(uintptr_t)data->cb, r,
                          (uint32_t)(uintptr_t)data->userData);
    free(data);
    return NULL;
}
#endif

/*  
   MR_SUCCESS 同步模式，初始化成功，不再调用cb
   MR_FAILED （立即感知的）失败，不再调用cb
   MR_WAITING 使用回调函数通知引擎初始化结果 
*/
int32 my_initNetwork(uc_engine* uc, MR_INIT_NETWORK_CB cb, const char* mode, void* userData) {
#ifdef NETWORK_SUPPORT
    printf("my_initNetwork(0x%p, '%s')\n", cb, mode);
    /* Mythroad apps can reinitialize the network with CMNET after a CMWAP
     * download.  The active mode must therefore be replaced, not only ever
     * promoted to CMWAP, otherwise later async connects stay visibly WAITING. */
    isCMWAP = (strncasecmp("cmwap", mode, 5) == 0) ? TRUE : FALSE;
    if (cb != NULL) {
        initNetworkAsyncData_t* data = malloc(sizeof(initNetworkAsyncData_t));
        data->cb = cb;
        data->userData = userData;
        data->uc = uc;
        if (pthread_create(&data->th, NULL, my_initNetworkAsync, data) != 0) {
            return MR_FAILED;
        }
        return MR_WAITING;
    }
    return my_initNetworkSync();
#else
    return MR_FAILED;
#endif
}

#ifndef _MSC_VER
typedef struct {
    char* name;
    MR_GET_HOST_CB cb;
    void* userData;
    uc_engine* uc;
    pthread_t th;
} getHostByNameAsyncData_t;
#endif

static int32 my_getHostByNameSync(const char* name) {
    int32 ret = MR_FAILED;
    char mappedName[VMRP_DNS_NAME_MAX + 1];
    const char* lookupName = getDnsLookupName(name, mappedName, sizeof(mappedName));

#if 1
    struct addrinfo *result, *res;
    printf("getaddrinfo of %s\n", lookupName);
    if (getaddrinfo(lookupName, NULL, NULL, &result) != 0) {
        printf("getaddrinfo failed!\n");
        return ret;
    }
    for (res = result; res; res = res->ai_next) {
        if (res->ai_family == AF_INET) {
            struct in_addr* addr = &((struct sockaddr_in*)res->ai_addr)->sin_addr;
            // char addrstr[100];
            // printf("--- IPv4 address: %s\n", inet_ntop(res->ai_family, addr, addrstr, sizeof(addrstr)));
            printf("--- IPv4 address: %s\n", inet_ntoa(*addr));
            ret = ntohl((*addr).s_addr);
            break;
        }
    }
    freeaddrinfo(result);
#else
    struct hostent* remoteHost = gethostbyname(name);
    if (remoteHost != NULL) {
        if (remoteHost->h_addrtype == AF_INET) {
            if (remoteHost->h_addr_list[0] != NULL) {
                struct in_addr addr;
                addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
                printf("%s\n", inet_ntoa(addr));
                return ntohl(addr.s_addr);
            }
        }
    }
#endif
    return ret;
}

#ifndef _MSC_VER
static void* my_getHostByNameAsync(void* arg) {
    getHostByNameAsyncData_t* data = (getHostByNameAsyncData_t*)arg;
    int32 r = my_getHostByNameSync(data->name);
    printf("my_getHostByNameAsync(): 0x%X\n", r);
    /* 同 my_initNetworkAsync:guest 32 位地址显式经 uintptr_t 截断 */
    bridge_dsm_network_cb(data->uc, (uint32_t)(uintptr_t)data->cb, r,
                          (uint32_t)(uintptr_t)data->userData);
    free(data->name);
    free(data);
    return NULL;
}
#endif

/*
   MR_FAILED （立即感知的）失败，不再调用cb
   MR_WAITING 使用回调函数通知引擎获取IP的结果
   其他值 同步模式，立即返回的IP地址，不再调用cb 
*/
int32 my_getHostByName(uc_engine* uc, const char* name, MR_GET_HOST_CB cb, void* userData) {
    printf("my_getHostByName\n");
#ifdef NETWORK_SUPPORT
    printf("my_getHostByName('%s', 0x%p)\n", name, cb);
    if (cb != NULL) {
        getHostByNameAsyncData_t* data = malloc(sizeof(getHostByNameAsyncData_t));
        int len = strlen(name);
        data->name = malloc(len + 1);
        strcpy(data->name, name);
        data->name[len] = '\0';
        data->cb = cb;
        data->userData = userData;
        data->uc = uc;
        int ret = pthread_create(&data->th, NULL, my_getHostByNameAsync, data);
        if (ret != 0) {
            return MR_FAILED;
        }
        return MR_WAITING;
    }
    return my_getHostByNameSync(name);
#else
    return MR_FAILED;
#endif
}

// 返回-1表示失败，0表示不可写，1表示可写
int checkWritable(SOCKET_T socket) {
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(socket, &writefds);

    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = 1000 * 50  // 50ms
    };

    SOCKET_T max_sd = socket;
    int ret = select(max_sd + 1, NULL, &writefds, NULL, &timeout);
    if (ret == 0) {  // timeout
        return 0;
    } else if (ret == -1) {
        return -1;
    }

    if (FD_ISSET(socket, &writefds)) {
        return 1;
    }
    return 0;
}

int32 my_sendto(int32 s, const char* buf, int len, int32 ip, uint16 port) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* data = (mSocket*)obj->data;

    struct sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_port = htons(port);
    to.sin_addr.s_addr = htonl(ip);

    printf("my_sendto(len:%d, '%s:%d')\n", len, inet_ntoa(to.sin_addr), port);

    int ret = sendto(data->s, buf, len, 0, (struct sockaddr*)&to, sizeof(to));
    if (ret == -1) {
        return MR_FAILED;
    }
    return ret;
#else
    return MR_FAILED;
#endif
}

/*
   >=0 实际发送的数据字节个数
   MR_FAILED Socket已经被关闭或遇到了无法修复的错误。 
*/
int32 my_send(int32 s, const char* buf, int len) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* data = (mSocket*)obj->data;

    data->sendCounter++;
    if (data->cmwapMode) {  // cmwap模式需要通过代理，这里模拟代理的功能
        printf("[my_send] cmwap on.\n");
        printf("[my_send] realState:%d.\n", data->realState);
        if (data->realState == MR_WAITING) {
            printf("[my_send] sendCounter:%d.\n", data->sendCounter);
            if (data->sendCounter == 1) {  // 第一次发送数据，尝试连接
                char host[256];
                uint16_t port;
                if (parseHostPort(buf, len, host, sizeof(host), &port) == MR_FAILED) {
                    printf("[my_send] Failed to parse host.\n");
                    return MR_FAILED;
                }
                int32 ip = my_getHostByNameSync(host);
                if (ip == MR_FAILED) {
                    printf("[my_send] Failed to get ip.\n");
                    return MR_FAILED;
                }
                // if (port == 6009) port = 8080;
                if (my_connect(s, ip, port, MR_SOCKET_BLOCK) == MR_FAILED) {
                    printf("[my_send] Failed to connect to ip.\n");
                    return MR_FAILED;
                }
                data->realState = MR_SUCCESS;
            }
            /* The CMWAP bridge resolves the proxy target and connects with a
             * blocking call above.  The socket is writable now, so send the
             * original request in this mr_send call; returning zero would make
             * one-shot HTTP downloaders wait for a retry that may never occur. */
        } else if (data->realState == MR_FAILED) {
            printf("[my_send] realState MR_FAILED\n");
            return MR_FAILED;
        }
    } else {
        printf("[my_send] cmwap off.\n");
    }
    int ret = checkWritable(data->s);
    if (ret == -1) {
        return MR_FAILED;
    } else if (ret == 0) {
        return 0;
    }
    ret = send(data->s, buf, len, 0);
    printf("my_send(s:%d, fd:%d, len:%d): sent=%d, errno=%d\n", s, (int)data->s, len, ret, errno);
    printf("[my_send] data: %s\n", buf);
    if (ret == -1) {
        return MR_FAILED;
    }
    return ret;
#else
    return MR_FAILED;
#endif
}

// 返回-1表示失败，0表示不可读，1表示可读
int checkReadable(SOCKET_T socket) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);

    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = 1000 * 50  // 50ms
    };

    SOCKET_T max_sd = socket;
    int ret = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
    if (ret == 0) {  // timeout
        return 0;
    } else if (ret == -1) {
        return -1;
    }
    if (FD_ISSET(socket, &readfds)) {
        return 1;
    }
    return 0;
}

int32 my_recvfrom(int32 s, char* buf, int len, int32* ip, uint16* port) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* data = (mSocket*)obj->data;
    int ret = checkReadable(data->s);
    if (ret == -1) {
        return MR_FAILED;
    } else if (ret == 0) {
        return 0;
    }
    struct sockaddr_in from;
    socklen_t fromLen = sizeof(from);
    ret = recvfrom(data->s, buf, len, 0, (struct sockaddr*)&from, &fromLen);
    if (ret == -1) {
        return MR_FAILED;
    }

    if (from.sin_family != AF_INET) {
        printf("warning my_recvfrom() recv not ipv4\n");
    }
    *port = ntohs(from.sin_port);
    *ip = ntohl(from.sin_addr.s_addr);
    printf("my_recvfrom(len:%d, '%s:%d')\n", len, inet_ntoa(from.sin_addr), *port);

    return ret;
#else
    return MR_FAILED;
#endif
}

/*
   >=0的整数 实际接收的数据字节个数
   MR_FAILED Socket已经被关闭或遇到了无法修复的错误。 
*/
int32 my_recv(int32 s, char* buf, int len) {
#ifdef NETWORK_SUPPORT
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* data = (mSocket*)obj->data;
    if (data->realState == MR_WAITING) {
        return 0;
    }
    if (data->realState == MR_FAILED) {
        return MR_FAILED;
    }
    // cmwap模式：真实连接建立后，伪造代理的"200 Connection established"响应
    // if (isCMWAP && data->cmwapProxyAck) {
    //     static const char fakeResp[] = "HTTP/1.0 200 Connection established\r\n\r\n";
    //     int respLen = (int)(sizeof(fakeResp) - 1);
    //     if (len >= respLen) {
    //         memcpy(buf, fakeResp, respLen);
    //         data->cmwapProxyAck = 0;
    //         printf("my_recv: injected CMWAP fake proxy ack\n");
    //         return respLen;
    //     }
    // }
    int ret = checkReadable(data->s);
    printf("my_recv(s:%d, fd:%d, len:%d): checkReadable=%d\n", s, (int)data->s, len, ret);
    if (ret == -1) {
        return MR_FAILED;
    } else if (ret == 0) {
        return 0;
    }
    ret = recv(data->s, buf, len, 0);
    printf("my_recv(s:%d): recv=%d, errno=%d\n", s, ret, errno);
    if (ret == 0) {
        /*
         * select()+readable followed by recv()==0 is TCP FIN, not "no data".
         * Mythroad sockets use 0 for temporary EAGAIN-style polling; keeping a
         * closed socket in that state makes browser/update state machines spin
         * forever instead of completing their response parse.
         */
        my_closeSocket(s);
        return MR_FAILED;
    }
    if (ret > 0) {
        char preview[65];
        int plen = ret < 64 ? ret : 64;
        memcpy(preview, buf, plen);
        preview[plen] = '\0';
        printf("my_recv data: [%s]\n", preview);
    }
    if (ret == -1) {
        return MR_FAILED;
    }
    return ret;
#else
    return MR_FAILED;
#endif
}
