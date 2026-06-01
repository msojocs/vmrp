#include "./include/memory.h"

void *my_mallocExt(uint32 len) {
    uint32 *p = malloc((size_t)len + sizeof(uint32));
    if (!p) return NULL;
    *p = len;
    return p + 1;
}

void my_freeExt(void *p) {
    if (p) free(((uint32 *)p) - 1);
}
