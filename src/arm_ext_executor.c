#include "./include/arm_ext_executor.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/utils.h"

extern void mr_printf(const char *format, ...);
extern int32 mr_rand(void);
extern int32 mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 mr_timerStart(uint16 t);
extern int32 mr_timerStop(void);
extern uint32 mr_getTime(void);
extern int32 mr_getDatetime(mr_datetime *datetime);
extern int32 mr_sleep(uint32 ms);
extern int32 mr_plat(int32 code, int32 param);
extern int32 mr_ferrno(void);
extern int32 mr_open(const char *filename, uint32 mode);
extern int32 mr_close(int32 f);
extern int32 mr_info(const char *filename);
extern int32 mr_write(int32 f, void *p, uint32 l);
extern int32 mr_read(int32 f, void *p, uint32 l);
extern int32 mr_seek(int32 f, int32 pos, int method);
extern int32 mr_getLen(const char *filename);
extern int32 mr_remove(const char *filename);
extern int32 mr_rename(const char *oldname, const char *newname);
extern int32 mr_mkDir(const char *name);
extern int32 mr_rmDir(const char *name);
extern int32 mr_exit(void);
extern int32 mr_startShake(int32 ms);
extern int32 mr_stopShake(void);
extern int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop);
extern int32 mr_stopSound(int type);
extern void mr_call(char *number);
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern int32 mr_getScreenInfo(void *s);
extern void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color);
extern int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
extern int wstrlen(char *txt);

#define EXT_BASE_ADDR 0x00010000u
#define EXT_MEM_SIZE  (16u * 1024u * 1024u)
#define EXT_TABLE_ADDR EXT_BASE_ADDR
#define EXT_TABLE_COUNT 150u
#define EXT_CODE_ADDR 0x00080000u
#define EXT_STACK_ADDR 0x00180000u
#define EXT_STACK_SIZE (512u * 1024u)
#define EXT_HEAP_ADDR 0x00200000u
#define EXT_STOP_ADDR 0x0007FFF0u

typedef struct mr_c_function_P_t {
    uint32 start_of_ER_RW;
    uint32 ER_RW_Length;
    int32 ext_type;
    uint32 mrc_extChunk;
    int32 stack;
} mr_c_function_P_t;

struct ArmExtModule {
    uc_engine *uc;
    uint8_t *mem;
    uint32_t heap_top;
    uint32_t code_len;
    const uint8_t *host_code;
    uint32_t helper_addr;
    uint32_t p_addr;
    uc_hook hook;
};

extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *_mr_readFile(const char *filename, int *filelen, int lookfor);
extern int32 mr_registerAPP(uint8 *p, int32 len, int32 index);
extern int _mr_TestCom(void *L, int input0, int input1);
extern int _mr_TestCom1(void *L, int input0, char *input1, int32 len);

static void *arm_ptr(ArmExtModule *m, uint32_t addr) {
    if (addr < EXT_BASE_ADDR || addr >= EXT_BASE_ADDR + EXT_MEM_SIZE) return NULL;
    return m->mem + (addr - EXT_BASE_ADDR);
}

static uint32_t arm_addr(ArmExtModule *m, const void *ptr) {
    if (ptr == NULL) return 0;
    return (uint32_t)((const uint8_t *)ptr - m->mem) + EXT_BASE_ADDR;
}

static uint32_t align4(uint32_t v) { return (v + 3u) & ~3u; }

static uint32_t arm_alloc(ArmExtModule *m, uint32_t len) {
    len = align4(len ? len : 1);
    if (m->heap_top + len >= EXT_BASE_ADDR + EXT_MEM_SIZE) return 0;
    uint32_t ret = m->heap_top;
    m->heap_top += len;
    return ret;
}

static uint32_t reg_read32(uc_engine *uc, int reg) {
    uint32_t v = 0;
    uc_reg_read(uc, reg, &v);
    return v;
}

static void reg_write32(uc_engine *uc, int reg, uint32_t v) {
    uc_reg_write(uc, reg, &v);
}

static uint32_t arg_read(ArmExtModule *m, unsigned n) {
    if (n < 4) return reg_read32(m->uc, UC_ARM_REG_R0 + n);
    uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
    uint32_t v = 0;
    uc_mem_read(m->uc, sp + (n - 4) * 4, &v, 4);
    return v;
}

static int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp) {
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    reg_write32(m->uc, UC_ARM_REG_LR, EXT_STOP_ADDR);
    uc_err err = uc_emu_start(m->uc, start, EXT_STOP_ADDR, 0, 0);
    if (err != UC_ERR_OK) {
        printf("arm_ext_executor: uc_emu_start(0x%X) failed: %u (%s)\n", start, err, uc_strerror(err));
        dumpREG(m->uc);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

static int run_arm(ArmExtModule *m, uint32_t start) {
    return run_arm_with_sp(m, start, EXT_STACK_ADDR + EXT_STACK_SIZE);
}

static char *arm_str(ArmExtModule *m, uint32_t addr) {
    char *p = (char *)arm_ptr(m, addr);
    return p ? p : (char *)"";
}

static int format_arm(ArmExtModule *m, char *dst, size_t dst_size, const char *fmt, unsigned first_arg) {
    size_t out = 0;
    unsigned ai = first_arg;
    for (const char *p = fmt; *p && out + 1 < dst_size; ++p) {
        if (*p != '%') {
            dst[out++] = *p;
            continue;
        }
        p++;
        if (*p == '%') { dst[out++] = '%'; continue; }
        char spec[32];
        size_t si = 0;
        spec[si++] = '%';
        while (*p && strchr("-+ #0", *p) && si + 1 < sizeof(spec)) spec[si++] = *p++;
        while (*p && isdigit((unsigned char)*p) && si + 1 < sizeof(spec)) spec[si++] = *p++;
        if (*p == 'l' && si + 1 < sizeof(spec)) spec[si++] = *p++;
        if (!*p) break;
        spec[si++] = *p;
        spec[si] = '\0';
        uint32_t av = arg_read(m, ai++);
        char tmp[512];
        switch (*p) {
            case 's': snprintf(tmp, sizeof(tmp), spec, arm_str(m, av)); break;
            case 'c': snprintf(tmp, sizeof(tmp), spec, (int)av); break;
            case 'p': snprintf(tmp, sizeof(tmp), "0x%X", av); break;
            default: snprintf(tmp, sizeof(tmp), spec, av); break;
        }
        size_t l = strlen(tmp);
        if (l > dst_size - out - 1) l = dst_size - out - 1;
        memcpy(dst + out, tmp, l);
        out += l;
    }
    dst[out] = '\0';
    return (int)out;
}

static void write_table_entry(ArmExtModule *m, uint32_t index, uint32_t value) {
    memcpy((uint8_t *)arm_ptr(m, EXT_TABLE_ADDR + index * 4), &value, 4);
}

static void cb_ret(ArmExtModule *m, uint32_t ret) {
    reg_write32(m->uc, UC_ARM_REG_R0, ret);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    reg_write32(m->uc, UC_ARM_REG_PC, lr);
}

static void hook_table(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (address < EXT_TABLE_ADDR || address >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) return;
    uint32_t idx = (uint32_t)((address - EXT_TABLE_ADDR) / 4);
    uint32_t r0 = arg_read(m, 0), r1 = arg_read(m, 1), r2 = arg_read(m, 2), r3 = arg_read(m, 3);
    uint32_t ret = MR_SUCCESS;

    switch (idx) {
        case 0:
            ret = arm_alloc(m, r0);
            break;
        case 1: ret = MR_SUCCESS; break;
        case 2: {
            uint32_t p = arm_alloc(m, r2);
            if (p && r0 && r1) memcpy(arm_ptr(m, p), arm_ptr(m, r0), r1 < r2 ? r1 : r2);
            ret = p;
        } break;
        case 3: memcpy(arm_ptr(m, r0), arm_ptr(m, r1), r2); ret = r0; break;
        case 4: memmove(arm_ptr(m, r0), arm_ptr(m, r1), r2); ret = r0; break;
        case 5: strcpy(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 6: strncpy(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 7: strcat(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 8: strncat(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 9: ret = memcmp(arm_ptr(m, r0), arm_ptr(m, r1), r2); break;
        case 10: ret = strcmp(arm_str(m, r0), arm_str(m, r1)); break;
        case 11: ret = strncmp(arm_str(m, r0), arm_str(m, r1), r2); break;
        case 13: { void *p = memchr(arm_ptr(m, r0), (int)r1, r2); ret = p ? arm_addr(m, p) : 0; } break;
        case 14: memset(arm_ptr(m, r0), (int)r1, r2); ret = r0; break;
        case 15: ret = strlen(arm_str(m, r0)); break;
        case 17: { char buf[1024]; ret = format_arm(m, buf, sizeof(buf), arm_str(m, r1), 2); strcpy(arm_ptr(m, r0), buf); } break;
        case 18: ret = atoi(arm_str(m, r0)); break;
        case 19: ret = (uint32_t)strtoul(arm_str(m, r0), NULL, r1); break;
        case 20: ret = mr_rand(); break;
        case 25: {
            m->helper_addr = r0;
            m->p_addr = arm_alloc(m, r1);
            memset(arm_ptr(m, m->p_addr), 0, r1);
            memcpy(arm_ptr(m, EXT_CODE_ADDR + 4), &m->p_addr, 4);
            ret = MR_SUCCESS;
            printf("arm_ext_executor: helper=0x%X P=0x%X len=%u\n", m->helper_addr, m->p_addr, r1);
        } break;
        case 26: { char buf[1024]; format_arm(m, buf, sizeof(buf), arm_str(m, r0), 1); mr_printf("%s", buf); ret = 0; } break;
        case 27: { uint32_t basep = r0, lenp = r1, base = 0, len = 0; char *hb = NULL; uint32 hl = 0; ret = mr_mem_get(&hb, &hl); base = (uint32)(uintptr_t)hb; len = hl; memcpy(arm_ptr(m, basep), &base, 4); memcpy(arm_ptr(m, lenp), &len, 4); } break;
        case 28: ret = MR_SUCCESS; break;
        case 29: {
            void *bmp = arm_ptr(m, r0);
            if (bmp) {
                mr_drawBitmap(bmp, (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4));
            }
            ret = MR_SUCCESS;
        } break;
        case 31: ret = mr_timerStart((uint16)r0); break;
        case 32: ret = mr_timerStop(); break;
        case 33: ret = mr_getTime(); break;
        case 34: ret = mr_getDatetime(arm_ptr(m, r0)); break;
        case 36: ret = mr_sleep(r0); break;
        case 37: ret = mr_plat((int32)r0, (int32)r1); break;
        case 39: ret = mr_ferrno(); break;
        case 40: ret = mr_open(arm_str(m, r0), r1); break;
        case 41: ret = mr_close((int32)r0); break;
        case 42: ret = mr_info(arm_str(m, r0)); break;
        case 43: ret = mr_write((int32)r0, arm_ptr(m, r1), r2); break;
        case 44: ret = mr_read((int32)r0, arm_ptr(m, r1), r2); break;
        case 45: ret = mr_seek((int32)r0, (int32)r1, (int)r2); break;
        case 46: ret = mr_getLen(arm_str(m, r0)); break;
        case 47: ret = mr_remove(arm_str(m, r0)); break;
        case 48: ret = mr_rename(arm_str(m, r0), arm_str(m, r1)); break;
        case 49: ret = mr_mkDir(arm_str(m, r0)); break;
        case 50: ret = mr_rmDir(arm_str(m, r0)); break;
        case 54: ret = mr_exit(); break;
        case 55: ret = mr_startShake((int32)r0); break;
        case 56: ret = mr_stopShake(); break;
        case 57: ret = mr_playSound((int)r0, arm_ptr(m, r1), r2, (int32)r3); break;
        case 58: ret = mr_stopSound((int)r0); break;
        case 60: mr_call(arm_str(m, r0)); ret = MR_SUCCESS; break;
        case 80: ret = mr_getScreenInfo(arm_ptr(m, r0)); break;
        case 118: ret = _DispUpEx((int16)r0, (int16)r1, (uint16)r2, (uint16)r3); break;
        case 119: _DrawPoint((int16)r0, (int16)r1, (uint16)r2); ret = 0; break;
        case 120: _DrawBitmap(arm_ptr(m, r0), (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4), (uint16)arg_read(m, 5), (uint16)arg_read(m, 6), (int16)arg_read(m, 7), (int16)arg_read(m, 8), (int16)arg_read(m, 9)); ret = 0; break;
        case 122: DrawRect((int16)r0, (int16)r1, (int16)r2, (int16)r3, (uint8)arg_read(m, 4), (uint8)arg_read(m, 5), (uint8)arg_read(m, 6)); ret = 0; break;
        case 123:
            printf("arm_ext_executor: mrc_drawText \"%s\"\n", arm_str(m, r0));
            ret = _DrawText(arm_str(m, r0), (int16)r1, (int16)r2, (uint8)r3, (uint8)arg_read(m, 4), (uint8)arg_read(m, 5), (int)arg_read(m, 6), (uint16)arg_read(m, 7));
            break;
        case 124: ret = _BitmapCheck(arm_ptr(m, r0), (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4), (uint16)arg_read(m, 5), (uint16)arg_read(m, 6)); break;
        case 125: {
            int fl = 0;
            void *hp = _mr_readFile(arm_str(m, r0), &fl, (int)r2);
            if (!hp) { ret = 0; break; }
            uint32_t ap = arm_alloc(m, (uint32_t)fl);
            memcpy(arm_ptr(m, ap), hp, fl);
            if (r1) memcpy(arm_ptr(m, r1), &fl, 4);
            ret = ap;
        } break;
        case 126: ret = wstrlen(arm_str(m, r0)); break;
        case 127: ret = mr_registerAPP(arm_ptr(m, r0), (int32)r1, (int32)r2); break;
        case 130: ret = _mr_TestCom(NULL, (int)r1, (int)r2); break;
        case 131: ret = _mr_TestCom1(NULL, (int)r1, arm_ptr(m, r2), (int32)r3); break;
        case 133: ret = ((int32)r1 == 0) ? 0 : (int32)r0 / (int32)r1; break;
        case 134: ret = ((int32)r1 == 0) ? 0 : (int32)r0 % (int32)r1; break;
        case 145: mr_platDrawChar((uint16)r0, (int32)r1, (int32)r2, (uint32)r3); ret = 0; break;
        default:
            printf("arm_ext_executor: table[%u] not implemented (r0=0x%X r1=0x%X r2=0x%X r3=0x%X)\n", idx, r0, r1, r2, r3);
            ret = MR_IGNORE;
            break;
    }
    cb_ret(m, ret);
}

static bool hook_invalid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)user_data;
    printf("arm_ext_executor: invalid memory %s addr=0x%llX size=%d value=0x%llX\n",
           memTypeStr(type), (unsigned long long)address, size, (unsigned long long)value);
    dumpREG(uc);
    return false;
}

static void init_table(ArmExtModule *m) {
    for (uint32_t i = 0; i < EXT_TABLE_COUNT; ++i) {
        write_table_entry(m, i, EXT_TABLE_ADDR + i * 4);
    }
    /* Shadow storage for table data slots used by C EXT code. */
    for (uint32_t i = 91; i <= 112; ++i) {
        uint32_t p = arm_alloc(m, 4096);
        write_table_entry(m, i, p);
    }
    for (uint32_t i = 135; i <= 146; ++i) {
        uint32_t p = arm_alloc(m, 4096);
        write_table_entry(m, i, p);
    }
}

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code) {
    if (!out || !code || len < 12) return MR_FAILED;
    printf("arm_ext_load len=%u code=%d head=%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
           len, load_code, code[0], code[1], code[2], code[3], code[4], code[5], code[6], code[7], code[8], code[9], code[10], code[11]);
    ArmExtModule *m = calloc(1, sizeof(*m));
    if (!m) return MR_FAILED;
    m->mem = calloc(1, EXT_MEM_SIZE);
    if (!m->mem) goto fail;
    m->heap_top = EXT_HEAP_ADDR;
    m->code_len = len;
    m->host_code = code;

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &m->uc);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_BASE_ADDR, EXT_MEM_SIZE, UC_PROT_ALL, m->mem);
    if (err != UC_ERR_OK) goto fail;
    init_table(m);
    memcpy(arm_ptr(m, EXT_CODE_ADDR), code, len);
    uint32_t table = EXT_TABLE_ADDR;
    memcpy(arm_ptr(m, EXT_CODE_ADDR), &table, 4);
    err = uc_hook_add(m->uc, &m->hook, UC_HOOK_CODE, hook_table, m, EXT_TABLE_ADDR, EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4);
    if (err != UC_ERR_OK) goto fail;
    uc_hook invalid_hook;
    err = uc_hook_add(m->uc, &invalid_hook, UC_HOOK_MEM_INVALID, hook_invalid, m, 1, 0);
    if (err != UC_ERR_OK) goto fail;

    reg_write32(m->uc, UC_ARM_REG_R0, (uint32_t)load_code);
    if (run_arm(m, EXT_CODE_ADDR + 8) != MR_SUCCESS) goto fail;
    *out = m;
    return MR_SUCCESS;
fail:
    arm_ext_unload(m);
    return MR_FAILED;
}

int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len) {
    if (output) *output = NULL;
    if (output_len) *output_len = 0;
    if (!m || !m->helper_addr || !m->p_addr) return MR_FAILED;

    uint32_t input_addr = 0;
    if (input != NULL) {
        if (input == m->host_code) {
            input_addr = EXT_CODE_ADDR;
        } else {
            input_addr = arm_alloc(m, input_len ? input_len : 1);
            if (!input_addr) return MR_FAILED;
            if (input_len) memcpy(arm_ptr(m, input_addr), input, input_len);
        }
    }
    uint32_t outp_addr = arm_alloc(m, 4);
    uint32_t outl_addr = arm_alloc(m, 4);
    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, m->p_addr), 4);
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    reg_write32(m->uc, UC_ARM_REG_R0, m->p_addr);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)code);
    reg_write32(m->uc, UC_ARM_REG_R2, input_addr);
    reg_write32(m->uc, UC_ARM_REG_R3, input_len);
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 8;
    uc_mem_write(m->uc, sp, &outp_addr, 4);
    uc_mem_write(m->uc, sp + 4, &outl_addr, 4);
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    if (run_arm_with_sp(m, m->helper_addr, sp) != MR_SUCCESS) return MR_FAILED;

    uint32_t arm_output = 0;
    uint32_t arm_output_len = 0;
    uc_mem_read(m->uc, outp_addr, &arm_output, 4);
    uc_mem_read(m->uc, outl_addr, &arm_output_len, 4);
    if (output_len) *output_len = (int32)arm_output_len;
    if (output) *output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
    return (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
}

void arm_ext_unload(ArmExtModule *m) {
    if (!m) return;
    if (m->uc) uc_close(m->uc);
    free(m->mem);
    free(m);
}
