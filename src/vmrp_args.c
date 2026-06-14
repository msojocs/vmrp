#include "./include/vmrp_args.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#else
#include <unistd.h>
#endif

#include "./include/fileLib.h"
#include "./include/utils.h"

#define VMRP_DEFAULT_DNS_MAP "wap.skmeg.com->159.75.119.124;rop.skymobiapp.com->127.0.0.1;spd.skymobiapp.com->159.75.119.124"

VmrpArgs vmrp_args_default(void) {
    VmrpArgs args;
    memset(&args, 0, sizeof(args));
    args.screen_width = DEFAULT_SCREEN_WIDTH;
    args.screen_height = DEFAULT_SCREEN_HEIGHT;
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
    printf("  --dns-map MAP       Resolve original domains using fake domains\n");
    printf("\n");
    printf("Environment variables:\n");
    printf("  VMRP_SCREEN_WIDTH   Screen width  (overridden by --screen)\n");
    printf("  VMRP_SCREEN_HEIGHT  Screen height (overridden by --screen)\n");
    printf("  VMRP_DNS_MAP        Domain map, e.g. old.example->new.example\n");
    printf("\n");
    printf("Without arguments, vmrp keeps the old behavior and starts VMRP_MRP or dsm_gm.mrp.\n");
    printf("Examples:\n");
    printf("  %s /path/to/app.mrp\n", name);
    printf("  %s --screen 176x220 /path/to/app.mrp\n", name);
    printf("  %s --dns-map old.example->new.example /path/to/app.mrp\n", name);
    printf("  %s /path/to/app.mrp start.mr _dsm\n", name);
}

static int resolve_cli_mrp_path(const char *input, char *output, size_t output_size) {
    if (!input || !*input) {
        fprintf(stderr, "vmrp: empty MRP path\n");
        return MR_FAILED;
    }

#ifdef _WIN32
    if (_fullpath(output, input, output_size) == NULL) {
        fprintf(stderr, "vmrp: unable to resolve '%s'\n", input);
        return MR_FAILED;
    }
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

static int parse_positional_args(int argc, char *argv[], const char **mrp_arg,
                                 const char **ext_arg, const char **entry_arg,
                                 const char **screen_arg, const char **dns_map_arg) {
    int positional = 0;
    int after_dashdash = 0;
    *mrp_arg = NULL;
    *ext_arg = NULL;
    *entry_arg = NULL;
    *screen_arg = NULL;
    *dns_map_arg = NULL;

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
        if (!after_dashdash && strcmp(arg, "--dns-map") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "vmrp: --dns-map requires an argument (e.g. old.example->new.example)\n");
                return MR_FAILED;
            }
            *dns_map_arg = argv[++i];
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
    const char *dns_map_arg = NULL;

    *out = vmrp_args_default();

    if (parse_positional_args(argc, argv, &mrp_arg, &ext_arg, &entry_arg,
                              &screen_arg, &dns_map_arg) != MR_SUCCESS) {
        return MR_FAILED;
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
