#ifndef __VMRP__H__
#define __VMRP__H__

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#define DEFAULT_SCREEN_WIDTH 240
#define DEFAULT_SCREEN_HEIGHT 320

typedef struct VmrpConfig {
    int screen_width;
    int screen_height;
} VmrpConfig;

extern VmrpConfig vmrp_config;

int32_t event(int32_t code, int32_t p1, int32_t p2);
int32_t timer();
int prepareVmrpArgs(int argc, char *argv[]);
int startVmrp(int argc, char *argv[]);
void printVmrpUsage(const char *program);

#endif
