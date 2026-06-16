#ifndef __VMRP_ARGS_H__
#define __VMRP_ARGS_H__

#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define DEFAULT_SCREEN_WIDTH 240
#define DEFAULT_SCREEN_HEIGHT 320
#define VMRP_MRP_NAME_LIMIT 128
#define VMRP_DNS_MAP_LIMIT 2048

typedef struct VmrpArgs {
    int screen_width;
    int screen_height;
    char work_dir[PATH_MAX];
    char mrp_path[PATH_MAX];
    char ext_name[256];
    char entry[256];
    char dns_map[VMRP_DNS_MAP_LIMIT];
} VmrpArgs;

VmrpArgs vmrp_args_default(void);
int vmrp_args_parse(int argc, char *argv[], VmrpArgs *out);
void vmrp_args_print_usage(const char *program);

#endif
