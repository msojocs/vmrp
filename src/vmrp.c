#include "./include/vmrp.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#ifdef _WIN32
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "./include/fileLib.h"
#include "./include/utils.h"
#include "./include/debug.h"
#include "./include/network.h"
#include "./include/runtime.h"
#include "./include/arm_ext_executor.h"

#define VMRP_LOG_ENABLED() (getenv("VMRP_LOG") != NULL)
#define VMRP_LOG(...)                          \
    do {                                      \
        if (VMRP_LOG_ENABLED()) {             \
            fprintf(stderr, __VA_ARGS__);     \
            fflush(stderr);                   \
        }                                     \
    } while (0)

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

VmrpConfig vmrp_config = {
    .screen_width = DEFAULT_SCREEN_WIDTH,
    .screen_height = DEFAULT_SCREEN_HEIGHT,
};

static VmrpRuntime runtime;
static int vmrp_exit_requested = 0;

static uint32_t rd32le(const uint8 *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static int gunzip_buffer(const uint8 *input, uint32 input_len, uint8 **output, uint32 *output_len) {
    z_stream zs;
    uint32 cap = input_len * 4 + 1024;
    int zret;

    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) return MR_FAILED;

    if (cap < 4096) cap = 4096;
    *output = (uint8 *)malloc(cap);
    if (!*output) {
        inflateEnd(&zs);
        return MR_FAILED;
    }

    zs.next_in = (Bytef *)input;
    zs.avail_in = input_len;
    do {
        if (zs.total_out == cap) {
            uint32 new_cap = cap * 2;
            uint8 *new_buf = (uint8 *)realloc(*output, new_cap);
            if (!new_buf) {
                free(*output);
                *output = NULL;
                inflateEnd(&zs);
                return MR_FAILED;
            }
            *output = new_buf;
            cap = new_cap;
        }
        zs.next_out = *output + zs.total_out;
        zs.avail_out = cap - (uint32)zs.total_out;
        zret = inflate(&zs, Z_NO_FLUSH);
    } while (zret == Z_OK);

    if (zret != Z_STREAM_END) {
        free(*output);
        *output = NULL;
        inflateEnd(&zs);
        return MR_FAILED;
    }
    *output_len = (uint32)zs.total_out;
    inflateEnd(&zs);
    return MR_SUCCESS;
}

static int extract_ext_from_mrp(uint8 *raw, uint32 raw_len, const char *name,
                                uint8 **code, uint32 *code_len) {
    uint32 pos = 240;
    if (raw_len < pos || memcmp(raw, "MRPG", 4) != 0) return MR_FAILED;
    while (pos + 16 <= raw_len) {
        uint32 name_len = rd32le(raw + pos);
        pos += 4;
        if (name_len == 0 || name_len > 255 || pos + name_len + 12 > raw_len) break;
        char entry_name[256];
        uint32 copy_len = name_len < sizeof(entry_name) - 1 ? name_len : sizeof(entry_name) - 1;
        memcpy(entry_name, raw + pos, copy_len);
        entry_name[copy_len] = '\0';
        pos += name_len;
        uint32 off = rd32le(raw + pos);
        uint32 packed_len = rd32le(raw + pos + 4);
        pos += 12;
        if (off > raw_len || packed_len > raw_len - off) return MR_FAILED;
        if (strcmp(entry_name, name) == 0) {
            if (packed_len >= 2 && raw[off] == 0x1f && raw[off + 1] == 0x8b) {
                return gunzip_buffer(raw + off, packed_len, code, code_len);
            }
            *code = (uint8 *)malloc(packed_len);
            if (!*code) return MR_FAILED;
            memcpy(*code, raw + off, packed_len);
            *code_len = packed_len;
            return MR_SUCCESS;
        }
    }
    return MR_FAILED;
}

static int smoke_arm_ext(const char *path) {
    FILE *fp = vmrp_host_fopen(path, "rb");
    if (!fp) {
        perror(path);
        return MR_FAILED;
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(fp);
        return MR_FAILED;
    }
    uint8 *buf = (uint8 *)malloc((size_t)sz);
    if (!buf) {
        fclose(fp);
        return MR_FAILED;
    }
    if (fread(buf, 1, (size_t)sz, fp) != (size_t)sz) {
        fclose(fp);
        free(buf);
        return MR_FAILED;
    }
    fclose(fp);

    uint8 *code = buf;
    uint32 code_len = (uint32)sz;
    if ((uint32)sz >= 4 && memcmp(buf, "MRPG", 4) == 0) {
        const char *ext_name = getenv("VMRP_ARM_EXT_NAME");
        uint8 *extracted = NULL;
        uint32 extracted_len = 0;
        if (!ext_name || !*ext_name) ext_name = "cfunction.ext";
        if (extract_ext_from_mrp(buf, (uint32)sz, ext_name, &extracted, &extracted_len) != MR_SUCCESS) {
            printf("arm_ext_smoke_extract('%s','%s') failed\n", path, ext_name);
            free(buf);
            return MR_FAILED;
        }
        printf("arm_ext_smoke_extract('%s','%s'): %u bytes\n", path, ext_name, extracted_len);
        code = extracted;
        code_len = extracted_len;
    }

    ArmExtModule *mod = NULL;
    const char *load_code_env = getenv("VMRP_ARM_EXT_LOAD_CODE");
    int load_code = (load_code_env && *load_code_env) ? atoi(load_code_env) : 1;
    int ret = arm_ext_load(&mod, code, code_len, load_code, NULL);
    printf("arm_ext_smoke_load('%s'): %d\n", path, ret);
    if (ret == MR_SUCCESS) {
        uint8 *output = NULL;
        int32 output_len = 0;
        ret = arm_ext_call(mod, 6, code, 2011, &output, &output_len);
        printf("arm_ext_smoke_call(code=6): %d output_len=%d\n", ret, output_len);
        ret = arm_ext_call(mod, 0, code, 2011, &output, &output_len);
        printf("arm_ext_smoke_call(code=0/mrc_init): %d output_len=%d\n", ret, output_len);
    }
    arm_ext_unload(mod);
    if (code != buf) free(code);
    free(buf);
    return ret;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
int32_t c_event(int32_t code, int32_t p1, int32_t p2) {
    return vmrp_runtime_event(&runtime, code, p1, p2);
}
#endif

int32_t event(int32_t code, int32_t p1, int32_t p2) {
    if (vmrp_exit_requested) return MR_FAILED;
    return vmrp_runtime_event(&runtime, code, p1, p2);
}

int32_t timer(void) {
    if (vmrp_exit_requested) return MR_FAILED;
    return vmrp_runtime_timer(&runtime);
}

int configureVmrpDnsMap(const char *map) {
    return my_configureDnsMap(map);
}

static int apply_config_paths(const VmrpArgs *args) {
    if (args->work_dir[0]) {
        snprintf(vmrp_config.work_dir, sizeof(vmrp_config.work_dir), "%s", args->work_dir);
    } else if (!vmrp_config.work_dir[0]) {
        snprintf(vmrp_config.work_dir, sizeof(vmrp_config.work_dir), ".");
    }

    if (vmrp_host_chdir(vmrp_config.work_dir) != MR_SUCCESS) {
        fprintf(stderr, "vmrp: unable to switch working directory to '%s': %s\n",
                vmrp_config.work_dir, strerror(errno));
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

int startVmrp(const VmrpArgs *args) {
    vmrp_exit_requested = 0;
    vmrp_config.screen_width = args->screen_width;
    vmrp_config.screen_height = args->screen_height;
    vmrp_config.memory_mb = args->memory_mb;
    if (apply_config_paths(args) != MR_SUCCESS) {
        return MR_FAILED;
    }

    if (args->dns_map[0]) {
        if (my_configureDnsMap(args->dns_map) != MR_SUCCESS) {
            fprintf(stderr, "vmrp: invalid DNS map '%s'\n", args->dns_map);
            return MR_FAILED;
        }
    }

    VMRP_LOG("[startVmrp] vmrp_runtime_init...\n");
    if (vmrp_runtime_init(&runtime) != MR_SUCCESS) return MR_FAILED;
    VMRP_LOG("[startVmrp] vmrp_runtime_init OK\n");

    const char *arm_ext_smoke = getenv("VMRP_ARM_EXT_SMOKE");
    if (arm_ext_smoke && *arm_ext_smoke) {
        int smoke_ret = smoke_arm_ext(arm_ext_smoke);
        vmrp_runtime_destroy(&runtime);
        exit(smoke_ret == MR_SUCCESS ? 0 : 1);
    }

    const char *filename = args->mrp_path;
    const char *extName = args->ext_name;
    const char *entry = args->entry[0] ? args->entry : NULL;

    if (!filename || !*filename) filename = "dsm_gm.mrp";
    if (!extName || !*extName) extName = "start.mr";

    if (strlen(filename) >= VMRP_MRP_NAME_LIMIT) {
        fprintf(stderr, "vmrp: MRP path is too long for Mythroad runtime (%zu >= %d): %s\n",
                strlen(filename), VMRP_MRP_NAME_LIMIT, filename);
        vmrp_runtime_destroy(&runtime);
        return MR_FAILED;
    }

    VMRP_LOG("[startVmrp] vmrp_runtime_start_dsm('%s','%s','%s')...\n",
             filename, extName, entry ? entry : "");
    uint32_t ret = vmrp_runtime_start_dsm(&runtime, filename, extName, entry);
    VMRP_LOG("[startVmrp] vmrp_runtime_start_dsm returned 0x%X\n", ret);
    if (ret != MR_SUCCESS) {
        vmrp_runtime_destroy(&runtime);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

void stopVmrp(void) {
    vmrp_runtime_destroy(&runtime);
    vmrp_exit_requested = 0;
}

void vmrp_request_exit(void) {
    vmrp_exit_requested = 1;
}

int vmrp_is_exited(void) {
    return vmrp_exit_requested;
}
