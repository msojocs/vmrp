#ifndef __VMRP_MEMORY_H__
#define __VMRP_MEMORY_H__

#include <stdlib.h>
#include <string.h>

#include "utils.h"

void *my_mallocExt(uint32 len);
void my_freeExt(void *p);

#endif
