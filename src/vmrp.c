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
#include <direct.h>
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define VMRP_MRP_NAME_LIMIT 128

#include "./include/bridge.h"
#include "./include/fileLib.h"
#include "./include/memory.h"
#include "./include/utils.h"
#include "./include/debug.h"
#include "./include/runtime.h"
#if VMRP_USE_NATIVE_MYTHROAD
#include "./include/arm_ext_executor.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static uint8_t *mrpMem;  // 模拟器的全部内存
static uc_engine *uc = NULL;
static VmrpRuntime runtime;

typedef struct VmrpStartupConfig {
    int prepared;
    const char *mrp_arg;
    const char *ext_arg;
    const char *entry_arg;
    char cli_mrp_path[PATH_MAX];
} VmrpStartupConfig;

static VmrpStartupConfig startup_config;

void printVmrpUsage(const char *program) {
    const char *name = (program && *program) ? program : "vmrp";
    printf("Usage: %s [MRP_PATH] [EXT_NAME] [ENTRY]\n", name);
    printf("       %s --help\n", name);
    printf("\n");
    printf("Without arguments, vmrp keeps the old behavior and starts VMRP_MRP or dsm_gm.mrp.\n");
    printf("Examples:\n");
    printf("  %s /path/to/app.mrp\n", name);
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

static int parse_positional_args(int argc, char *argv[], const char **mrp_arg,
                                 const char **ext_arg, const char **entry_arg) {
    int positional = 0;
    int after_dashdash = 0;
    *mrp_arg = NULL;
    *ext_arg = NULL;
    *entry_arg = NULL;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (!after_dashdash && strcmp(arg, "--") == 0) {
            after_dashdash = 1;
            continue;
        }
        if (!after_dashdash && arg[0] == '-') {
            fprintf(stderr, "vmrp: unknown option: %s\n", arg);
            printVmrpUsage(argv[0]);
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
            printVmrpUsage(argv[0]);
            return MR_FAILED;
        }
        positional++;
    }
    return MR_SUCCESS;
}

int prepareVmrpArgs(int argc, char *argv[]) {
    const char *mrp_arg = NULL;
    const char *ext_arg = NULL;
    const char *entry_arg = NULL;

    memset(&startup_config, 0, sizeof(startup_config));
    if (parse_positional_args(argc, argv, &mrp_arg, &ext_arg, &entry_arg) != MR_SUCCESS) {
        return MR_FAILED;
    }
    if (mrp_arg && resolve_cli_mrp_path(mrp_arg, startup_config.cli_mrp_path,
                                        sizeof(startup_config.cli_mrp_path)) != MR_SUCCESS) {
        return MR_FAILED;
    }

    startup_config.mrp_arg = mrp_arg;
    startup_config.ext_arg = ext_arg;
    startup_config.entry_arg = entry_arg;
    startup_config.prepared = 1;
    return MR_SUCCESS;
}

#if VMRP_USE_NATIVE_MYTHROAD
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
    FILE *fp = fopen(path, "rb");
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
    int ret = arm_ext_load(&mod, code, code_len, load_code);
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
#endif

// 返回的内存禁止free
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void *getMrpMemPtr(uint32_t addr) {
#if VMRP_USE_NATIVE_MYTHROAD
    (void)addr;
    return NULL;
#else
    return mrpMem + (addr - START_ADDRESS);
#endif
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
uint32_t toMrpMemAddr(void *ptr) {
#if VMRP_USE_NATIVE_MYTHROAD
    (void)ptr;
    return 0;
#else
    return ((uint8_t *)ptr - mrpMem) + START_ADDRESS;
#endif
}

#if !VMRP_USE_NATIVE_MYTHROAD
#ifdef DEBUG
static void hook_block(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    printf(">>> Tracing basic block at 0x%" PRIx64 ", block size = 0x%x\n", address, size);
}
static void hook_mem_valid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    printf(">>> Tracing mem_valid mem_type:%s at 0x%" PRIx64 ", size:0x%x, value:0x%" PRIx64 "\n",
           memTypeStr(type), address, size, value);
    if (type == UC_MEM_READ && size <= 4) {
        uint32_t v, pc;
        uc_mem_read(uc, address, &v, size);
        uc_reg_read(uc, UC_ARM_REG_PC, &pc);
        printf("PC:0x%X,read:0x%X\n", pc, v);
    }
}

#endif

static bool hook_mem_invalid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    printf(">>> Tracing mem_invalid mem_type:%s at 0x%" PRIx64 ", size:0x%x, value:0x%" PRIx64 "\n",
           memTypeStr(type), address, size, value);
    dumpREG(uc);
    return false;
}

int freeVmrp(uc_engine *uc) {
    free(mrpMem);
    uc_close(uc);
    return 0;
}

uc_engine *initVmrp() {
    uc_engine *uc;
    uc_err err;
    uc_hook trace;

    err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &uc);
    if (err) {
        printf("Failed on uc_open() with error returned: %u (%s)\n", err, uc_strerror(err));
        return NULL;
    }

    mrpMem = malloc(TOTAL_MEMORY);
    // unicorn存在BUG，UC_HOOK_MEM_INVALID只能拦截第一次UC_MEM_FETCH_PROT，所以干脆设置成可执行，统一在UC_HOOK_CODE事件中处理
    err = uc_mem_map_ptr(uc, START_ADDRESS, TOTAL_MEMORY, UC_PROT_ALL, mrpMem);
    if (err) {
        printf("Failed mem map: %u (%s)\n", err, uc_strerror(err));
        goto end;
    }
    initMemoryManager(MEMORY_MANAGER_ADDRESS, MEMORY_MANAGER_SIZE);

    err = bridge_init(uc);
    if (err) {
        printf("Failed bridge_init(): %u (%s)\n", err, uc_strerror(err));
        goto end;
    }

#if 0 
    // 一些游戏检测内存时会去读写0-CODE_ADDRESS地址的值, 不清楚为什么要这样做，如果这块内存没有映射到unicorn会因为无效的内存访问导致程序退出
    // 映射一块内存后那些无法运行的游戏就能打开了，没有深入去研究，先记录在这里
    char *ppp = malloc(CODE_ADDRESS);
    // memset(ppp, 0xFF, CODE_ADDRESS);
    memset(ppp, 0, CODE_ADDRESS);
    uc_mem_map_ptr(uc, 0, CODE_ADDRESS, UC_PROT_ALL, ppp);

    // uc_mem_map(uc, 0, CODE_ADDRESS, UC_PROT_ALL);
    // uc_hook_add(uc, &trace, UC_HOOK_MEM_VALID, hook_mem_valid, NULL, 0, CODE_ADDRESS);
#endif

#ifdef DEBUG
    uc_hook_add(uc, &trace, UC_HOOK_BLOCK, hook_block, NULL, 1, 0);
    uc_hook_add(uc, &trace, UC_HOOK_MEM_VALID, hook_mem_valid, NULL, 1, 0);
    uc_hook_add(uc, &trace, UC_HOOK_CODE, hook_code_debug, NULL, 1, 0);
#endif
    uc_hook_add(uc, &trace, UC_HOOK_MEM_INVALID, hook_mem_invalid, NULL, 1, 0, 0);

    // 设置栈
    uint32_t value = STACK_ADDRESS + STACK_SIZE;  // 满递减
    uc_reg_write(uc, UC_ARM_REG_SP, &value);

    return uc;
end:
    uc_close(uc);
    return NULL;
}
#endif

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
int32_t c_event(int32_t code, int32_t p1, int32_t p2) {
    return vmrp_runtime_event(&runtime, code, p1, p2);
}
#endif

int32_t event(int32_t code, int32_t p1, int32_t p2) {
    return vmrp_runtime_event(&runtime, code, p1, p2);
}

int32_t timer() {
    return vmrp_runtime_timer(&runtime);
}

#if !VMRP_USE_NATIVE_MYTHROAD
int32_t loadCode(uc_engine *engine) {
    char *filename = "cfunction.ext";
    int32_t len = my_getLen(filename);
    char *buf = readFile(filename);
    if (buf == NULL) {
        return MR_FAILED;
    }
    uc_mem_write(engine, CODE_ADDRESS, buf, len);
    free(buf);
    return MR_SUCCESS;
}
#endif

int startVmrp(int argc, char *argv[]) {
    if (!startup_config.prepared && prepareVmrpArgs(argc, argv) != MR_SUCCESS) {
        return MR_FAILED;
    }

    if (vmrp_runtime_init(&runtime) != MR_SUCCESS) return MR_FAILED;
    uc = runtime.uc;

#if VMRP_USE_NATIVE_MYTHROAD
    const char *arm_ext_smoke = getenv("VMRP_ARM_EXT_SMOKE");
    if (arm_ext_smoke && *arm_ext_smoke) {
        int smoke_ret = smoke_arm_ext(arm_ext_smoke);
        vmrp_runtime_destroy(&runtime);
        exit(smoke_ret == MR_SUCCESS ? 0 : 1);
    }
#endif

    const char *filename = getenv("VMRP_MRP");
    const char *extName = getenv("VMRP_EXT");
    const char *entry = getenv("VMRP_ENTRY");
    if (startup_config.mrp_arg) {
        filename = startup_config.cli_mrp_path;
    }
    if (startup_config.ext_arg) extName = startup_config.ext_arg;
    if (startup_config.entry_arg) entry = startup_config.entry_arg;
    if (!filename || !*filename) filename = "dsm_gm.mrp";
    if (!extName || !*extName) extName = "start.mr";
    if (entry && !*entry) entry = NULL;
    if (strlen(filename) >= VMRP_MRP_NAME_LIMIT) {
        fprintf(stderr, "vmrp: MRP path is too long for Mythroad runtime (%zu >= %d): %s\n",
                strlen(filename), VMRP_MRP_NAME_LIMIT, filename);
        vmrp_runtime_destroy(&runtime);
        return MR_FAILED;
    }

    uint32_t ret = vmrp_runtime_start_dsm(&runtime, filename, extName, entry);
    printf("vmrp_runtime_start_dsm('%s','%s','%s'): 0x%X\n",
           filename, extName, entry ? entry : "", ret);
    if (ret != MR_SUCCESS) {
        vmrp_runtime_destroy(&runtime);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}
