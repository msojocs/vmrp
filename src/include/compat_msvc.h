#ifndef COMPAT_MSVC_H
#define COMPAT_MSVC_H

#ifdef _MSC_VER

#include <io.h>
#include <direct.h>
#include <process.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <stdint.h>
#include <time.h>

#ifndef F_OK
#define F_OK 0
#endif

#ifndef S_IRWXU
#define S_IRWXU 0
#endif
#ifndef S_IRWXG
#define S_IRWXG 0
#endif
#ifndef S_IRWXO
#define S_IRWXO 0
#endif

#ifndef O_RAW
#define O_RAW _O_BINARY
#endif

#ifndef getcwd
#define getcwd _getcwd
#endif

#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif

#ifndef strtok_r
#define strtok_r(str, delim, saveptr) strtok_s((str), (delim), (saveptr))
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

static inline int clock_gettime(int clk_id, struct timespec *tp) {
    (void)clk_id;
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    tp->tv_sec = (long)(cnt.QuadPart / freq.QuadPart);
    tp->tv_nsec = (long)((cnt.QuadPart % freq.QuadPart) * 1000000000LL / freq.QuadPart);
    return 0;
}

#include <winsock2.h>
static inline int gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64_t tt = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    tt -= 116444736000000000ULL;
    tv->tv_sec = (long)(tt / 10000000ULL);
    tv->tv_usec = (long)((tt % 10000000ULL) / 10);
    return 0;
}

#endif /* _MSC_VER */
#endif /* COMPAT_MSVC_H */
