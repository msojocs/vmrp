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
    char work_dir[PATH_MAX];
} VmrpConfig;

extern VmrpConfig vmrp_config;

int32_t event(int32_t code, int32_t p1, int32_t p2);
int32_t timer();
int configureVmrpDnsMap(const char *map);
int startVmrp(const VmrpArgs *args);
void stopVmrp(void);
void vmrp_request_exit(void);
int vmrp_is_exited(void);

#endif
