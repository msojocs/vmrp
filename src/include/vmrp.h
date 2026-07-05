#ifndef __VMRP__H__
#define __VMRP__H__

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "vmrp_args.h"

typedef struct VmrpConfig {
    int screen_width;
    int screen_height;
    int memory_mb; /* 应用可见内存(MB):1/2/4/6/8/16,0 视为默认 */
    char work_dir[PATH_MAX];
} VmrpConfig;

/* 应用可见内存(字节),memory_mb 为 0 时取默认值 */
static inline uint32_t vmrp_memory_bytes(int memory_mb) {
    int mb = memory_mb > 0 ? memory_mb : DEFAULT_MEMORY_MB;
    return (uint32_t)mb * 1024u * 1024u;
}

extern VmrpConfig vmrp_config;

int32_t event(int32_t code, int32_t p1, int32_t p2);
int32_t timer();
int configureVmrpDnsMap(const char *map);
int startVmrp(const VmrpArgs *args);
void stopVmrp(void);
void vmrp_request_exit(void);
int vmrp_is_exited(void);

#endif
