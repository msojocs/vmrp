#include "./include/vmrp_args.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <direct.h>
#include <windows.h>
#define getcwd _getcwd
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

#include "./include/fileLib.h"
#include "./include/utils.h"

#define VMRP_DEFAULT_DNS_MAP "wap.skmeg.com->159.75.119.124;rop.skymobiapp.com->159.75.119.124;spd.skymobiapp.com->159.75.119.124;freeads.51mrp.com->159.75.119.124;proxy.51mrp.com->159.75.119.124;proxy2.51mrp.com->159.75.119.124;help.proxy.51mrp.com->159.75.119.124"

#ifdef _WIN32
static int wide_to_utf8(const wchar_t *wide, char *output, size_t output_size) {
    int n;
    if (!wide || !output || output_size == 0) return MR_FAILED;
    n = WideCharToMultiByte(CP_UTF8, 0, wide, -1, output, (int)output_size, NULL, NULL);
    return n > 0 ? MR_SUCCESS : MR_FAILED;
}

static wchar_t *utf8_to_wide(const char *text) {
    int len;
    wchar_t *wide;
    if (!text) return NULL;
    len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
    if (len <= 0) return NULL;
    wide = (wchar_t *)malloc((size_t)len * sizeof(wchar_t));
    if (!wide) return NULL;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, wide, len) <= 0) {
        free(wide);
        return NULL;
    }
    return wide;
}
#endif

static int is_host_absolute_path(const char *path) {
    if (!path || !*path) {
        return 0;
    }
    if (path[0] == '/') {
        return 1;
    }
#ifdef _WIN32
    if (path[0] == '\\') {
        return 1;
    }
    if (((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        return 1;
    }
#endif
    return 0;
}

static void normalize_dir_path(char *path) {
    size_t len;
    if (!path) return;
    for (char *p = path; *p; ++p) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    len = strlen(path);
    while (len > 1 && path[len - 1] == '/') {
#ifdef _WIN32
        if (len == 3 && path[1] == ':') {
            break;
        }
#endif
        path[--len] = '\0';
    }
}

static void dirname_inplace(char *path) {
    char *slash;
    if (!path || !*path) {
        return;
    }
    normalize_dir_path(path);
    slash = strrchr(path, '/');
    if (!slash) {
        snprintf(path, PATH_MAX, ".");
        return;
    }
    if (slash == path) {
        path[1] = '\0';
        return;
    }
#ifdef _WIN32
    if (slash == path + 2 && path[1] == ':') {
        slash[1] = '\0';
        return;
    }
#endif
    *slash = '\0';
}

static int resolve_config_dir(const char *input, char *output, size_t output_size) {
    char cwd[PATH_MAX];
    int n;
    if (!input || !*input || !output || output_size == 0) {
        return MR_FAILED;
    }
    if (is_host_absolute_path(input)) {
        n = snprintf(output, output_size, "%s", input);
    } else {
        if (!getcwd(cwd, sizeof(cwd))) {
            return MR_FAILED;
        }
        n = snprintf(output, output_size, "%s/%s", cwd, input);
    }
    if (n < 0 || (size_t)n >= output_size) {
        return MR_FAILED;
    }
    normalize_dir_path(output);
    return MR_SUCCESS;
}

static int resolve_executable_dir(const char *argv0, char *output, size_t output_size) {
    char path[PATH_MAX];
    if (!output || output_size == 0) {
        return MR_FAILED;
    }
    path[0] = '\0';

#ifdef _WIN32
    wchar_t wpath[PATH_MAX];
    DWORD n = GetModuleFileNameW(NULL, wpath, (DWORD)(sizeof(wpath) / sizeof(wpath[0])));
    if (n > 0 && n < sizeof(wpath) / sizeof(wpath[0]) &&
        wide_to_utf8(wpath, path, sizeof(path)) == MR_SUCCESS) {
        dirname_inplace(path);
        snprintf(output, output_size, "%s", path);
        normalize_dir_path(output);
        return MR_SUCCESS;
    }
#elif defined(__APPLE__)
    uint32_t len = sizeof(path);
    if (_NSGetExecutablePath(path, &len) == 0) {
        char resolved[PATH_MAX];
        if (realpath(path, resolved)) {
            snprintf(path, sizeof(path), "%s", resolved);
        }
        dirname_inplace(path);
        snprintf(output, output_size, "%s", path);
        normalize_dir_path(output);
        return MR_SUCCESS;
    }
#elif defined(__linux__)
    ssize_t n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n > 0 && (size_t)n < sizeof(path)) {
        path[n] = '\0';
        dirname_inplace(path);
        snprintf(output, output_size, "%s", path);
        normalize_dir_path(output);
        return MR_SUCCESS;
    }
#endif

    if (argv0 && *argv0 && (strchr(argv0, '/') || strchr(argv0, '\\'))) {
        char resolved[PATH_MAX];
#ifdef _WIN32
        wchar_t *wargv0 = utf8_to_wide(argv0);
        wchar_t wresolved[PATH_MAX];
        if (wargv0 && _wfullpath(wresolved, wargv0,
                                 sizeof(wresolved) / sizeof(wresolved[0])) &&
            wide_to_utf8(wresolved, resolved, sizeof(resolved)) == MR_SUCCESS) {
            snprintf(path, sizeof(path), "%s", resolved);
        } else {
            snprintf(path, sizeof(path), "%s", argv0);
        }
        free(wargv0);
#else
        if (realpath(argv0, resolved)) {
            snprintf(path, sizeof(path), "%s", resolved);
        } else {
            snprintf(path, sizeof(path), "%s", argv0);
        }
#endif
        dirname_inplace(path);
        snprintf(output, output_size, "%s", path);
        normalize_dir_path(output);
        return MR_SUCCESS;
    }
    if (getcwd(output, output_size)) {
        normalize_dir_path(output);
        return MR_SUCCESS;
    }
    snprintf(output, output_size, ".");
    return MR_SUCCESS;
}

static void vmrp_args_set_default_dirs(VmrpArgs *args, const char *argv0) {
    if (!args) return;
    if (resolve_executable_dir(argv0, args->work_dir, sizeof(args->work_dir)) != MR_SUCCESS) {
        snprintf(args->work_dir, sizeof(args->work_dir), ".");
    }
}

VmrpArgs vmrp_args_default(void) {
    VmrpArgs args;
    memset(&args, 0, sizeof(args));
    args.screen_width = DEFAULT_SCREEN_WIDTH;
    args.screen_height = DEFAULT_SCREEN_HEIGHT;
    args.memory_mb = DEFAULT_MEMORY_MB;
    args.device_year = DEFAULT_DEVICE_YEAR;
    args.device_month = DEFAULT_DEVICE_MONTH;
    args.device_day = DEFAULT_DEVICE_DAY;
    vmrp_args_set_default_dirs(&args, NULL);
    snprintf(args.ext_name, sizeof(args.ext_name), "start.mr");
    snprintf(args.dns_map, sizeof(args.dns_map), "%s", VMRP_DEFAULT_DNS_MAP);
    return args;
}

void vmrp_args_print_usage(const char *program) {
    const char *name = (program && *program) ? program : "vmrp";
    printf("Usage: %s [OPTIONS] [MRP_PATH] [EXT_NAME] [ENTRY]\n", name);
    printf("       %s --help\n", name);
    printf("\n");
    printf("Options:\n");
    printf("  --screen WxH        Set screen resolution (default: 240x320)\n");
    printf("  --memory SIZE       Set app-visible memory: 1M,2M,4M,6M,8M,16M (default: 1M)\n");
    printf("  --device-date DATE  Set handset date (YYYY-MM-DD, or host; default: 2011-01-01)\n");
    printf("  --work-dir DIR      Set working directory (default: executable directory)\n");
    printf("  --dns-map MAP       Resolve original domains using fake domains\n");
    printf("\n");
    printf("Environment variables:\n");
    printf("  VMRP_SCREEN_WIDTH   Screen width  (overridden by --screen)\n");
    printf("  VMRP_SCREEN_HEIGHT  Screen height (overridden by --screen)\n");
    printf("  VMRP_MEMORY         App-visible memory (overridden by --memory)\n");
    printf("  VMRP_DEVICE_DATE    Handset date (overridden by --device-date)\n");
    printf("  VMRP_WORK_DIR       Working directory (overridden by --work-dir)\n");
    printf("  VMRP_DNS_MAP        Domain map, e.g. old.example->new.example\n");
    printf("  VMRP_PPM_PATH       PPM screen dump path for SIGUSR1/verification\n");
    printf("  VMRP_E2E_SOCKET     Unix socket path for stepwise E2E control\n");
    printf("\n");
    printf("Without arguments, vmrp keeps the old behavior and starts VMRP_MRP or dsm_gm.mrp.\n");
    printf("Examples:\n");
    printf("  %s /path/to/app.mrp\n", name);
    printf("  %s --screen 176x220 /path/to/app.mrp\n", name);
    printf("  %s --memory 4M /path/to/app.mrp\n", name);
    printf("  %s --device-date host /path/to/app.mrp\n", name);
    printf("  %s --dns-map old.example->new.example /path/to/app.mrp\n", name);
    printf("  %s /path/to/app.mrp start.mr _dsm\n", name);
}

static int resolve_cli_mrp_path(const char *input, char *output, size_t output_size) {
    if (!input || !*input) {
        fprintf(stderr, "vmrp: empty MRP path\n");
        return MR_FAILED;
    }

#ifdef _WIN32
    wchar_t *winput = utf8_to_wide(input);
    wchar_t woutput[PATH_MAX];
    if (!winput || _wfullpath(woutput, winput, sizeof(woutput) / sizeof(woutput[0])) == NULL ||
        wide_to_utf8(woutput, output, output_size) != MR_SUCCESS) {
        free(winput);
        fprintf(stderr, "vmrp: unable to resolve '%s'\n", input);
        return MR_FAILED;
    }
    free(winput);
#else
    (void)output_size;
    if (realpath(input, output) == NULL) {
        fprintf(stderr, "vmrp: unable to resolve '%s': %s\n", input, strerror(errno));
        return MR_FAILED;
    }
#endif

    if (my_info(output) != MR_IS_FILE) {
        fprintf(stderr, "vmrp: MRP file not found: %s\n", output);
        return MR_FAILED;
    }
    if (strlen(output) >= VMRP_MRP_NAME_LIMIT) {
        fprintf(stderr, "vmrp: MRP path is too long for Mythroad runtime (%zu >= %d): %s\n",
                strlen(output), VMRP_MRP_NAME_LIMIT, output);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

static int parse_screen_size(const char *str, int *w, int *h) {
    int tw = 0, th = 0;
    if (sscanf(str, "%dx%d", &tw, &th) != 2 || tw <= 0 || th <= 0) {
        return MR_FAILED;
    }
    *w = tw;
    *h = th;
    return MR_SUCCESS;
}

/* 解析应用可见内存档位:数字 + 可选 M/MB 后缀(大小写均可),
 * 仅允许真机常见档位 1/2/4/6/8/16 MB。 */
static int parse_memory_size(const char *str, int *mb) {
    char suffix[3] = {0};
    int value = 0;
    int n = sscanf(str, "%d%2s", &value, suffix);
    if (n < 1) return MR_FAILED;
    if (n == 2 &&
        strcmp(suffix, "M") != 0 && strcmp(suffix, "m") != 0 &&
        strcmp(suffix, "MB") != 0 && strcmp(suffix, "mb") != 0 &&
        strcmp(suffix, "Mb") != 0 && strcmp(suffix, "mB") != 0) {
        return MR_FAILED;
    }
    if (value != 1 && value != 2 && value != 4 &&
        value != 6 && value != 8 && value != 16) {
        return MR_FAILED;
    }
    *mb = value;
    return MR_SUCCESS;
}

int vmrp_args_parse_device_date(const char *str, int *year, int *month, int *day) {
    static const int days_per_month[] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    int y, m, d, max_day;
    size_t i;

    if (!str || !year || !month || !day) return MR_FAILED;
    if (strcmp(str, "host") == 0) {
        *year = 0;
        *month = 0;
        *day = 0;
        return MR_SUCCESS;
    }

    /* Keep the public format unambiguous across libc sscanf variants. */
    if (strlen(str) != 10 || str[4] != '-' || str[7] != '-') return MR_FAILED;
    for (i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (str[i] < '0' || str[i] > '9') return MR_FAILED;
    }
    if (sscanf(str, "%d-%d-%d", &y, &m, &d) != 3 ||
        y < 1 || y > 9999 || m < 1 || m > 12) {
        return MR_FAILED;
    }
    max_day = days_per_month[m];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) {
        max_day = 29;
    }
    if (d < 1 || d > max_day) return MR_FAILED;
    *year = y;
    *month = m;
    *day = d;
    return MR_SUCCESS;
}

static int parse_positional_args(int argc, char *argv[], const char **mrp_arg,
                                 const char **ext_arg, const char **entry_arg,
                                 const char **screen_arg, const char **work_dir_arg,
                                 const char **dns_map_arg, const char **memory_arg,
                                 const char **device_date_arg) {
    int positional = 0;
    int after_dashdash = 0;
    *mrp_arg = NULL;
    *ext_arg = NULL;
    *entry_arg = NULL;
    *screen_arg = NULL;
    *work_dir_arg = NULL;
    *dns_map_arg = NULL;
    *memory_arg = NULL;
    *device_date_arg = NULL;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (!after_dashdash && strcmp(arg, "--") == 0) {
            after_dashdash = 1;
            continue;
        }
        if (!after_dashdash && strcmp(arg, "--screen") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "vmrp: --screen requires an argument (e.g. 176x220)\n");
                return MR_FAILED;
            }
            *screen_arg = argv[++i];
            continue;
        }
        if (!after_dashdash && strcmp(arg, "--memory") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "vmrp: --memory requires an argument (e.g. 4M)\n");
                return MR_FAILED;
            }
            *memory_arg = argv[++i];
            continue;
        }
        if (!after_dashdash && strcmp(arg, "--device-date") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "vmrp: --device-date requires YYYY-MM-DD or host\n");
                return MR_FAILED;
            }
            *device_date_arg = argv[++i];
            continue;
        }
        if (!after_dashdash && strcmp(arg, "--dns-map") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "vmrp: --dns-map requires an argument (e.g. old.example->new.example)\n");
                return MR_FAILED;
            }
            *dns_map_arg = argv[++i];
            continue;
        }
        if (!after_dashdash && strcmp(arg, "--work-dir") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "vmrp: --work-dir requires an argument\n");
                return MR_FAILED;
            }
            *work_dir_arg = argv[++i];
            continue;
        }
        if (!after_dashdash && arg[0] == '-') {
            fprintf(stderr, "vmrp: unknown option: %s\n", arg);
            vmrp_args_print_usage(argv[0]);
            return MR_FAILED;
        }

        if (positional == 0) {
            *mrp_arg = arg;
        } else if (positional == 1) {
            *ext_arg = arg;
        } else if (positional == 2) {
            *entry_arg = arg;
        } else {
            fprintf(stderr, "vmrp: too many arguments\n");
            vmrp_args_print_usage(argv[0]);
            return MR_FAILED;
        }
        positional++;
    }
    return MR_SUCCESS;
}

int vmrp_args_parse(int argc, char *argv[], VmrpArgs *out) {
    const char *mrp_arg = NULL;
    const char *ext_arg = NULL;
    const char *entry_arg = NULL;
    const char *screen_arg = NULL;
    const char *work_dir_arg = NULL;
    const char *dns_map_arg = NULL;
    const char *memory_arg = NULL;
    const char *device_date_arg = NULL;

    *out = vmrp_args_default();
    vmrp_args_set_default_dirs(out, (argc > 0) ? argv[0] : NULL);

    if (parse_positional_args(argc, argv, &mrp_arg, &ext_arg, &entry_arg,
                              &screen_arg, &work_dir_arg, &dns_map_arg,
                              &memory_arg, &device_date_arg) != MR_SUCCESS) {
        return MR_FAILED;
    }

    /* Working directory: CLI --work-dir > env var > executable directory */
    if (work_dir_arg) {
        if (resolve_config_dir(work_dir_arg, out->work_dir,
                               sizeof(out->work_dir)) != MR_SUCCESS) {
            fprintf(stderr, "vmrp: invalid work dir '%s'\n", work_dir_arg);
            return MR_FAILED;
        }
    } else {
        const char *env_work_dir = getenv("VMRP_WORK_DIR");
        if (env_work_dir && *env_work_dir) {
            if (resolve_config_dir(env_work_dir, out->work_dir,
                                   sizeof(out->work_dir)) != MR_SUCCESS) {
                fprintf(stderr, "vmrp: invalid VMRP_WORK_DIR '%s'\n", env_work_dir);
                return MR_FAILED;
            }
        }
    }

    /* MRP path: CLI arg > env var > default */
    if (mrp_arg) {
        if (resolve_cli_mrp_path(mrp_arg, out->mrp_path,
                                 sizeof(out->mrp_path)) != MR_SUCCESS) {
            return MR_FAILED;
        }
    } else {
        const char *env_mrp = getenv("VMRP_MRP");
        if (env_mrp && *env_mrp) {
            snprintf(out->mrp_path, sizeof(out->mrp_path), "%s", env_mrp);
        } else {
            snprintf(out->mrp_path, sizeof(out->mrp_path), "dsm_gm.mrp");
        }
    }

    /* Extension name: CLI arg > env var > default */
    if (ext_arg) {
        snprintf(out->ext_name, sizeof(out->ext_name), "%s", ext_arg);
    } else {
        const char *env_ext = getenv("VMRP_EXT");
        if (env_ext && *env_ext) {
            snprintf(out->ext_name, sizeof(out->ext_name), "%s", env_ext);
        }
    }

    /* Entry point: CLI arg > env var > none */
    if (entry_arg) {
        snprintf(out->entry, sizeof(out->entry), "%s", entry_arg);
    } else {
        const char *env_entry = getenv("VMRP_ENTRY");
        if (env_entry && *env_entry) {
            snprintf(out->entry, sizeof(out->entry), "%s", env_entry);
        }
    }

    /* Screen size: CLI --screen > env vars > default */
    if (screen_arg) {
        int w, h;
        if (parse_screen_size(screen_arg, &w, &h) != MR_SUCCESS) {
            fprintf(stderr, "vmrp: invalid screen size '%s' (expected WxH, e.g. 176x220)\n", screen_arg);
            return MR_FAILED;
        }
        out->screen_width = w;
        out->screen_height = h;
    } else {
        const char *env_w = getenv("VMRP_SCREEN_WIDTH");
        const char *env_h = getenv("VMRP_SCREEN_HEIGHT");
        if (env_w && *env_w) out->screen_width = atoi(env_w);
        if (env_h && *env_h) out->screen_height = atoi(env_h);
    }

    /* Memory size: CLI --memory > env var > default */
    if (!memory_arg) {
        const char *env_memory = getenv("VMRP_MEMORY");
        if (env_memory && *env_memory) memory_arg = env_memory;
    }
    if (memory_arg) {
        int mb;
        if (parse_memory_size(memory_arg, &mb) != MR_SUCCESS) {
            fprintf(stderr, "vmrp: invalid memory size '%s' (allowed: 1M,2M,4M,6M,8M,16M)\n", memory_arg);
            return MR_FAILED;
        }
        out->memory_mb = mb;
    }

    /* Device date: CLI --device-date > env var > deterministic legacy RTC. */
    if (!device_date_arg) {
        const char *env_device_date = getenv("VMRP_DEVICE_DATE");
        if (env_device_date && *env_device_date) device_date_arg = env_device_date;
    }
    if (device_date_arg &&
        vmrp_args_parse_device_date(device_date_arg, &out->device_year,
                                    &out->device_month,
                                    &out->device_day) != MR_SUCCESS) {
        fprintf(stderr,
                "vmrp: invalid device date '%s' (expected YYYY-MM-DD or host)\n",
                device_date_arg);
        return MR_FAILED;
    }

    /* DNS map: CLI --dns-map > env var > default */
    if (dns_map_arg) {
        snprintf(out->dns_map, sizeof(out->dns_map), "%s", dns_map_arg);
    } else {
        const char *env_dns_map = getenv("VMRP_DNS_MAP");
        if (env_dns_map) {
            snprintf(out->dns_map, sizeof(out->dns_map), "%s", env_dns_map);
        }
    }

    return MR_SUCCESS;
}
