#ifndef __VMRP__H__
#define __VMRP__H__

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

int32_t event(int32_t code, int32_t p1, int32_t p2);
int32_t timer();
int prepareVmrpArgs(int argc, char *argv[]);
int startVmrp(int argc, char *argv[]);
void printVmrpUsage(const char *program);

#endif
