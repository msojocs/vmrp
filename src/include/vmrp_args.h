#ifndef __VMRP_ARGS_H__
#define __VMRP_ARGS_H__

#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define DEFAULT_SCREEN_WIDTH 240
#define DEFAULT_SCREEN_HEIGHT 320
#define DEFAULT_MEMORY_MB 1
/* Archived MRP software expects a handset-era RTC unless the caller opts
 * into the host wall clock with --device-date host. */
#define DEFAULT_DEVICE_YEAR 2011
#define DEFAULT_DEVICE_MONTH 1
#define DEFAULT_DEVICE_DAY 1
#define VMRP_MRP_NAME_LIMIT 128
#define VMRP_DNS_MAP_LIMIT 2048

typedef struct VmrpArgs {
    int screen_width;
    int screen_height;
    int memory_mb; /* 应用可见内存(MB):1/2/4/6/8/16 */
    int device_year;  /* 0 表示直接使用宿主日期 */
    int device_month;
    int device_day;
    char work_dir[PATH_MAX];
    char mrp_path[PATH_MAX];
    char ext_name[256];
    char entry[256];
    char dns_map[VMRP_DNS_MAP_LIMIT];
} VmrpArgs;

VmrpArgs vmrp_args_default(void);
/* Shared calendar validation for CLI and embedding API configuration. */
int vmrp_args_parse_device_date(const char *str, int *year, int *month, int *day);
int vmrp_args_parse(int argc, char *argv[], VmrpArgs *out);
void vmrp_args_print_usage(const char *program);

#endif
