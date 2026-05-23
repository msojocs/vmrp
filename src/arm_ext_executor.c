#include "./include/arm_ext_executor.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/utils.h"

extern void mr_printf(const char *format, ...);
extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern int32 mr_rand(void);
extern void mr_free(void *p, uint32 len);
extern int32 mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 mr_timerStart(uint16 t);
extern int32 mr_timerStop(void);
extern int32 mr_stop_ex(int16 freemem);
extern uint32 mr_getTime(void);
extern int32 mr_getDatetime(mr_datetime *datetime);
extern int32 mr_sleep(uint32 ms);
extern int32 mr_plat(int32 code, int32 param);
extern const char *mr_get_pack_filename(void);
extern const char *mr_get_start_filename(void);
extern const char *mr_get_old_pack_filename(void);
extern const char *mr_get_old_start_filename(void);
extern const char *mr_get_start_fileparameter(void);
typedef void (*MR_PLAT_EX_CB)(uint8 *output, int32 output_len);
extern int32 mr_platEx(int32 code, uint8 *input, int32 input_len,
                       uint8 **output, int32 *output_len, MR_PLAT_EX_CB *cb);
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
extern int32 mr_findStart(const char *name, char *buffer, uint32 len);
extern int32 mr_findGetNext(int32 search_handle, char *buffer, uint32 len);
extern int32 mr_findStop(int32 search_handle);
extern int32 mr_exit(void);
extern int32 mr_startShake(int32 ms);
extern int32 mr_stopShake(void);
extern int32 mr_playSound(int type, const void *data, uint32 dataLen, int32 loop);
extern int32 mr_stopSound(int type);
extern void mr_call(char *number);
extern int32 mr_dialogCreate(const char *title, const char *text, int32 type);
extern int32 mr_dialogRelease(int32 dialog);
extern int32 mr_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type);
extern int32 mr_textCreate(const char *title, const char *text, int32 type);
extern int32 mr_textRelease(int32 text);
extern int32 mr_textRefresh(int32 handle, const char *title, const char *text);
extern int32 mr_editCreate(const char *title, const char *text, int32 type, int32 max_size);
extern int32 mr_editRelease(int32 edit);
extern const char *mr_editGetText(int32 edit);
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);
extern int32 mr_getScreenInfo(void *s);
extern void mr_platDrawChar(uint16 ch, int32 x, int32 y, uint32 color);
extern int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
extern uint16 *c2u(const char *cp, int *err, int *size);
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
#define EXT_WRAPPER_STACK_SIZE 0x20000u
#define EXT_LOW_TABLE_SIZE 0x1000u
#define EXT_TRACE_PC_RING 64u

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
    uint8_t *low_table;
    uint32_t heap_top;
    uint32_t code_len;
    const uint8_t *host_code;
    uint32_t helper_addr;
    uint32_t p_addr;
    uint32_t screen_addr;
    uint32_t screen_len;
    uint32_t char_bitmap_addr;
    int32_t screen_w;
    int32_t screen_h;
    uint32_t origin_mem_addr;
    uint32_t origin_mem_len;
    uint32_t origin_mem_left;
    uint32_t origin_mem_min;
    uint32_t origin_mem_top;
    uint32_t internal_table_addr;
    uint32_t port_table_addr;
    uint32_t mr_m0_files_addr;
    uint32_t vm_state_slot;
    uint32_t mr_state_slot;
    uint32_t bi_slot;
    uint32_t mr_timer_p_slot;
    uint32_t mr_timer_state_slot;
    uint32_t mr_timer_run_without_pause_slot;
    uint32_t mr_gzin_buf_slot;
    uint32_t mr_gzout_buf_slot;
    uint32_t lg_gzinptr_slot;
    uint32_t lg_gzoutcnt_slot;
    uint32_t mr_sms_cfg_need_save_slot;
    uint32_t last_file_addr;
    uint32_t last_file_len;
    uint8_t *last_file_copy;
    uint32_t last_alloc_addr;
    uint32_t last_alloc_len;
    uint32_t nested_p_addr;
    uint32_t active_p_addr;
    uint32_t active_helper_addr;
    uint32_t primary_p_addr;       // First nested EXT is the app logic; later nested EXTs can be helpers.
    uint32_t primary_helper_addr;
    uint32_t outer_r9;
    uint32_t nested_return_addr;
    uint32_t screen_write_count;
    uint32_t thumb_fix_count;
    uint32_t pc_ring[EXT_TRACE_PC_RING];
    uint32_t cpsr_ring[EXT_TRACE_PC_RING];
    uint32_t pc_ring_pos;
    uint32_t busy_wait_count;
    int nested_loading;
    int screen_dirty;
    uc_hook hook;
};

extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *_mr_readFile(const char *filename, int *filelen, int lookfor);
extern void *mr_readFile_from_ram(const char *filename, int *filelen, int lookfor, char *ram_file, int ram_file_len);
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

static void set_arm_mode_for_addr(ArmExtModule *m, uint32_t addr) {
    uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
    if (addr & 1u) {
        cpsr |= (1u << 5);
    } else {
        cpsr &= ~(1u << 5);
    }
    reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
}

static uint32_t arm_exec_addr(uint32_t addr) {
    // Keep bit 0 so Unicorn enters Thumb mode for Thumb entry points.
    return addr;
}

static void trace_pc(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t pos = m->pc_ring_pos++ % EXT_TRACE_PC_RING;
    m->pc_ring[pos] = (uint32_t)address;
    m->cpsr_ring[pos] = reg_read32(uc, UC_ARM_REG_CPSR);
}

static void dump_pc_ring(ArmExtModule *m) {
    if (!getenv("VMRP_ARM_EXT_TRACE_PC")) return;
    uint32_t total = m->pc_ring_pos < EXT_TRACE_PC_RING ? m->pc_ring_pos : EXT_TRACE_PC_RING;
    printf("arm_ext_executor: recent PCs:\n");
    for (uint32_t i = 0; i < total; ++i) {
        uint32_t idx = (m->pc_ring_pos - total + i) % EXT_TRACE_PC_RING;
        printf("  #%02u pc=0x%X cpsr=0x%X %s\n",
               i, m->pc_ring[idx], m->cpsr_ring[idx],
               (m->cpsr_ring[idx] & (1u << 5)) ? "thumb" : "arm");
    }
}

static uint32_t arg_read(ArmExtModule *m, unsigned n) {
    if (n < 4) return reg_read32(m->uc, UC_ARM_REG_R0 + n);
    uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
    uint32_t v = 0;
    uc_mem_read(m->uc, sp + (n - 4) * 4, &v, 4);
    return v;
}

static int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp) {
    set_arm_mode_for_addr(m, start);
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    reg_write32(m->uc, UC_ARM_REG_LR, EXT_STOP_ADDR);
    uc_err err = uc_emu_start(m->uc, arm_exec_addr(start), EXT_STOP_ADDR, 0, 0);
    if (err != UC_ERR_OK) {
        if (reg_read32(m->uc, UC_ARM_REG_PC) == EXT_STOP_ADDR) return MR_SUCCESS;
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
        uint32_t cur_sp = reg_read32(m->uc, UC_ARM_REG_SP);
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
            if (getenv("VMRP_ARM_EXT_TRACE")) {
                printf("arm_ext_executor: ARM callback stopped at non-instruction pc=0x%X sp=0x%X\n", pc, cur_sp);
                dump_pc_ring(m);
            }
            reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
            return MR_SUCCESS;
        }
        printf("arm_ext_executor: uc_emu_start(0x%X) failed: %u (%s)\n", start, err, uc_strerror(err));
        uint8_t *pc_mem = arm_ptr(m, pc);
        if (pc_mem) {
            printf("arm_ext_executor: pc bytes @0x%X:", pc);
            for (uint32_t i = 0; i < 16; ++i) printf(" %02X", pc_mem[i]);
            printf("\n");
        }
        dump_pc_ring(m);
        dumpREG(m->uc);
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

static int run_arm(ArmExtModule *m, uint32_t start) {
    return run_arm_with_sp(m, start, EXT_STACK_ADDR + EXT_STACK_SIZE);
}

static void restore_ext_r9(ArmExtModule *m) {
    if (!m) return;
    uint32_t p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    if (!p_addr) return;
    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
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

static uint32_t alloc_u32_slot(ArmExtModule *m, uint32_t value) {
    uint32_t slot = arm_alloc(m, 4);
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
    return slot;
}

static uint32_t alloc_string(ArmExtModule *m, const char *value) {
    if (!value) value = "";
    size_t len = strlen(value) + 1;
    uint32_t slot = arm_alloc(m, (uint32_t)len);
    if (slot) memcpy(arm_ptr(m, slot), value, len);
    return slot;
}

static void internal_slot_write(ArmExtModule *m, uint32_t slot, uint32_t value) {
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
}

static void sync_origin_mem_stats(ArmExtModule *m) {
    if (!m->origin_mem_addr) return;
    uint32_t left_slot = 0;
    uint32_t min_slot = 0;
    uint32_t top_slot = 0;
    memcpy(&left_slot, arm_ptr(m, EXT_TABLE_ADDR + 111 * 4), 4);
    memcpy(&min_slot, arm_ptr(m, EXT_TABLE_ADDR + 135 * 4), 4);
    memcpy(&top_slot, arm_ptr(m, EXT_TABLE_ADDR + 136 * 4), 4);
    internal_slot_write(m, left_slot, m->origin_mem_left);
    internal_slot_write(m, min_slot, m->origin_mem_min);
    internal_slot_write(m, top_slot, m->origin_mem_top);
}

static void note_origin_mem_alloc(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    len = align4(len ? len : 1);
    m->origin_mem_left = len < m->origin_mem_left ? m->origin_mem_left - len : 0;
    if (m->origin_mem_left < m->origin_mem_min) {
        m->origin_mem_min = m->origin_mem_left;
        m->origin_mem_top = m->origin_mem_len - m->origin_mem_min;
    }
    sync_origin_mem_stats(m);
}

static void note_origin_mem_free(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    m->origin_mem_left += align4(len);
    if (m->origin_mem_left > m->origin_mem_len) m->origin_mem_left = m->origin_mem_len;
    sync_origin_mem_stats(m);
}

static void enter_screen_context(ArmExtModule *m, uint16 **saved_screen) {
    *saved_screen = mr_screenBuf;
    if (m->screen_addr) mr_screenBuf = (uint16 *)arm_ptr(m, m->screen_addr);
}

static void leave_screen_context(ArmExtModule *m, uint16 *saved_screen) {
    if (m->screen_addr && saved_screen && m->screen_len) {
        memcpy(saved_screen, arm_ptr(m, m->screen_addr), m->screen_len);
    }
    mr_screenBuf = saved_screen;
    if (m->screen_dirty && saved_screen) {
        mr_drawBitmap(saved_screen, 0, 0, (uint16)m->screen_w, (uint16)m->screen_h);
        m->screen_dirty = 0;
    }
}

static void cb_ret(ArmExtModule *m, uint32_t ret) {
    reg_write32(m->uc, UC_ARM_REG_R0, ret);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    set_arm_mode_for_addr(m, lr);
    reg_write32(m->uc, UC_ARM_REG_PC, lr);
}

static void hook_low_zero(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (address != 0) return;
    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    if (lr) {
        set_arm_mode_for_addr(m, lr);
        reg_write32(uc, UC_ARM_REG_PC, lr);
    } else {
        uint32_t pc = EXT_STOP_ADDR;
        reg_write32(uc, UC_ARM_REG_PC, pc);
        uc_emu_stop(uc);
    }
}

static void hook_table(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (address < EXT_TABLE_ADDR || address >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) return;
    uint32_t idx = (uint32_t)((address - EXT_TABLE_ADDR) / 4);
    uint32_t r0 = arg_read(m, 0), r1 = arg_read(m, 1), r2 = arg_read(m, 2), r3 = arg_read(m, 3);
    uint32_t ret = MR_SUCCESS;
    int trace_table = getenv("VMRP_ARM_EXT_TRACE") != NULL;
    if (trace_table) {
        uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
        printf("arm_ext_executor: table[%u](0x%X,0x%X,0x%X,0x%X) lr=0x%X\n", idx, r0, r1, r2, r3, lr);
    }
    if (idx != 33) {
        m->busy_wait_count = 0;
    }

    switch (idx) {
        case 0:
            ret = arm_alloc(m, r0);
            m->last_alloc_addr = ret;
            m->last_alloc_len = r0;
            if (ret) note_origin_mem_alloc(m, r0);
            {
                uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
                if (m->nested_loading && m->last_file_addr && lr >= m->last_file_addr && lr < m->last_file_addr + m->last_file_len && r0 > 0x1000) {
                    if (!m->outer_r9) {
                        m->outer_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                    }
                    if (!m->nested_return_addr) {
                        uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
                        uint32_t saved_lr = 0;
                        if (arm_ptr(m, sp + 12)) {
                            uc_mem_read(m->uc, sp + 12, &saved_lr, 4);
                            m->nested_return_addr = saved_lr;
                        }
                    }
                    reg_write32(m->uc, UC_ARM_REG_R9, ret + 4);
                    m->nested_loading = 0;
                    if (getenv("VMRP_ARM_EXT_TRACE")) {
                        printf("arm_ext_executor: nested R9=0x%X after malloc 0x%X outer=0x%X return=0x%X\n",
                               ret + 4, r0, m->outer_r9, m->nested_return_addr);
                    }
                }
            }
            break;
        case 1:
            note_origin_mem_free(m, r1);
            ret = MR_SUCCESS;
            break;
        case 2: {
            uint32_t p = arm_alloc(m, r2);
            if (p) note_origin_mem_alloc(m, r2);
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
        case 22:
            ret = mr_stop_ex((int16)r0);
            internal_slot_write(m, m->mr_state_slot, 0);
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            break;
        case 25: {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            int nested = m->last_file_addr && lr >= m->last_file_addr && lr < m->last_file_addr + m->last_file_len;
            uint32_t p_addr = nested ? m->nested_p_addr : 0;
            int reuse_nested_p = p_addr && r1 == sizeof(mr_c_function_P_t) && arm_ptr(m, p_addr);
            uint32_t p_len = r1;
            if (!nested && p_len >= 0x2000u && p_len < EXT_WRAPPER_STACK_SIZE + sizeof(mr_c_function_P_t)) {
                p_len = EXT_WRAPPER_STACK_SIZE + sizeof(mr_c_function_P_t);
            }
            if (!p_addr || r1 != sizeof(mr_c_function_P_t) || !arm_ptr(m, p_addr)) {
                p_addr = arm_alloc(m, p_len);
            }
            if (p_addr && !reuse_nested_p) memset(arm_ptr(m, p_addr), 0, p_len);
            if (nested) {
                memcpy(arm_ptr(m, m->last_file_addr + 4), &p_addr, 4);
                /*
                 * Plug-in wrapper EXT code keeps the active nested
                 * mr_c_function pointer in its own R9-relative runtime block.
                 * Native _mr_c_function_new updates the global dispatcher; in
                 * this split executor the wrapper remains the dispatcher, so
                 * mirror the nested helper into that slot for subsequent
                 * code-6/code-8/code-0 forwarding.
                 */
                uint32_t outer_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                uint32_t helper_slot = outer_r9 + 0x1c4u;
                if (arm_ptr(m, helper_slot)) {
                    memcpy(arm_ptr(m, helper_slot), &r0, 4);
                }
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                if (!m->primary_helper_addr) {
                    m->primary_helper_addr = r0;
                    m->primary_p_addr = p_addr;
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: nested helper=0x%X P=0x%X len=%u slot=0x%X primary=0x%X/0x%X\n",
                           r0, p_addr, r1, helper_slot, m->primary_helper_addr, m->primary_p_addr);
                }
                m->nested_loading = 1;
                m->nested_p_addr = 0;
            } else {
                m->helper_addr = r0;
                m->p_addr = p_addr;
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                memcpy(arm_ptr(m, EXT_CODE_ADDR + 4), &m->p_addr, 4);
            }
            ret = p_addr ? MR_SUCCESS : MR_FAILED;
        } break;
        case 26: { char buf[1024]; format_arm(m, buf, sizeof(buf), arm_str(m, r0), 1); mr_printf("%s", buf); ret = 0; } break;
        case 27: {
            uint32_t base = m->origin_mem_addr;
            uint32_t len = m->origin_mem_len;
            if (r0) memcpy(arm_ptr(m, r0), &base, 4);
            if (r1) memcpy(arm_ptr(m, r1), &len, 4);
            ret = base ? MR_SUCCESS : MR_FAILED;
        } break;
        case 28: ret = MR_SUCCESS; break;
        case 29: {
            void *bmp = arm_ptr(m, r0);
            if (bmp) {
                mr_drawBitmap(bmp, (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4));
            }
            ret = MR_SUCCESS;
        } break;
        case 30: {
            int width = 0;
            int height = 0;
            const char *bitmap = mr_getCharBitmap((uint16)r0, (uint16)r1, &width, &height);
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &width, 4);
            if (r3 && arm_ptr(m, r3)) memcpy(arm_ptr(m, r3), &height, 4);
            if (bitmap) {
                if (!m->char_bitmap_addr) m->char_bitmap_addr = arm_alloc(m, 32);
                if (m->char_bitmap_addr) {
                    memcpy(arm_ptr(m, m->char_bitmap_addr), bitmap, 32);
                    ret = m->char_bitmap_addr;
                } else {
                    ret = 0;
                }
            } else {
                ret = 0;
            }
        } break;
        case 31:
            ret = mr_timerStart((uint16)r0);
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            break;
        case 32:
            ret = mr_timerStop();
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            break;
        case 33: {
            ret = mr_getTime();
            /*
             * mpc.mrp's exit path enters a busy-wait loop polling mr_getTime
             * indefinitely. On real hardware the wait yields to the OS so
             * other events (timer, exit signal) can fire. In our synchronous
             * emulator nothing can break the loop. Detect a long run of
             * consecutive mr_getTime calls and treat it as an exit request.
             */
            m->busy_wait_count++;
            if (m->busy_wait_count >= 200) {
                printf("[arm_ext_executor] mr_getTime busy-wait detected (count=%u), triggering exit\n",
                       m->busy_wait_count);
                m->busy_wait_count = 0;
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                uc_emu_stop(m->uc);
                mr_exit();
            }
        } break;
        case 34: ret = mr_getDatetime(arm_ptr(m, r0)); break;
        case 36: ret = mr_sleep(r0); break;
        case 37: ret = mr_plat((int32)r0, (int32)r1); break;
        case 38: {
            uint32_t outputp_addr = r3;
            uint32_t output_lenp_addr = arg_read(m, 4);
            uint32_t arm_output = 0;
            int32 host_output_len = 0;
            uint8 *host_output = NULL;
            uint8 *input = (uint8 *)arm_ptr(m, r1);

            if (outputp_addr) {
                uc_mem_read(m->uc, outputp_addr, &arm_output, 4);
                host_output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
            }
            if (output_lenp_addr) {
                uc_mem_read(m->uc, output_lenp_addr, &host_output_len, 4);
            }

            ret = mr_platEx((int32)r0, input, (int32)r2,
                            outputp_addr ? &host_output : NULL,
                            output_lenp_addr ? &host_output_len : NULL,
                            NULL);

            if (outputp_addr) {
                uint32_t new_arm_output = arm_output;
                if (host_output && host_output_len > 0) {
                    void *old_arm_ptr = arm_output ? arm_ptr(m, arm_output) : NULL;
                    if (host_output != old_arm_ptr) {
                        new_arm_output = arm_alloc(m, (uint32_t)host_output_len + 1);
                        if (new_arm_output) {
                            memcpy(arm_ptr(m, new_arm_output), host_output, (uint32_t)host_output_len);
                            ((uint8 *)arm_ptr(m, new_arm_output))[host_output_len] = '\0';
                        }
                    }
                } else {
                    new_arm_output = 0;
                }
                uc_mem_write(m->uc, outputp_addr, &new_arm_output, 4);
            }
            if (output_lenp_addr) {
                uc_mem_write(m->uc, output_lenp_addr, &host_output_len, 4);
            }
        } break;
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
        case 51: ret = mr_findStart(arm_str(m, r0), arm_ptr(m, r1), r2); break;
        case 52: ret = mr_findGetNext((int32)r0, arm_ptr(m, r1), r2); break;
        case 53: ret = mr_findStop((int32)r0); break;
        case 54: ret = mr_exit(); break;
        case 55: ret = mr_startShake((int32)r0); break;
        case 56: ret = mr_stopShake(); break;
        case 57: ret = mr_playSound((int)r0, arm_ptr(m, r1), r2, (int32)r3); break;
        case 58: ret = mr_stopSound((int)r0); break;
        case 60: mr_call(arm_str(m, r0)); ret = MR_SUCCESS; break;
        case 69: ret = mr_dialogCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 70: ret = mr_dialogRelease((int32)r0); break;
        case 71: ret = mr_dialogRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2), (int32)r3); break;
        case 72: ret = mr_textCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 73: ret = mr_textRelease((int32)r0); break;
        case 74: ret = mr_textRefresh((int32)r0, arm_str(m, r1), arm_str(m, r2)); break;
        case 75: ret = mr_editCreate(arm_str(m, r0), arm_str(m, r1), (int32)r2, (int32)r3); break;
        case 76: ret = mr_editRelease((int32)r0); break;
        case 77: {
            const char *text = mr_editGetText((int32)r0);
            ret = alloc_string(m, text ? text : "");
        } break;
        case 80: ret = mr_getScreenInfo(arm_ptr(m, r0)); break;
        case 118:
            ret = _DispUpEx((int16)r0, (int16)r1, (uint16)r2, (uint16)r3);
            break;
        case 119: _DrawPoint((int16)r0, (int16)r1, (uint16)r2); m->screen_dirty = 1; ret = 0; break;
        case 120: _DrawBitmap(arm_ptr(m, r0), (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4), (uint16)arg_read(m, 5), (uint16)arg_read(m, 6), (int16)arg_read(m, 7), (int16)arg_read(m, 8), (int16)arg_read(m, 9)); m->screen_dirty = 1; ret = 0; break;
        case 122: DrawRect((int16)r0, (int16)r1, (int16)r2, (int16)r3, (uint8)arg_read(m, 4), (uint8)arg_read(m, 5), (uint8)arg_read(m, 6)); m->screen_dirty = 1; ret = 0; break;
        case 123:
            ret = _DrawText(arm_str(m, r0), (int16)r1, (int16)r2, (uint8)r3, (uint8)arg_read(m, 4), (uint8)arg_read(m, 5), (int)arg_read(m, 6), (uint16)arg_read(m, 7));
            m->screen_dirty = 1;
            break;
        case 124: ret = _BitmapCheck(arm_ptr(m, r0), (int16)r1, (int16)r2, (uint16)r3, (uint16)arg_read(m, 4), (uint16)arg_read(m, 5), (uint16)arg_read(m, 6)); break;
        case 125: {
            int fl = 0;
            uint32_t packp = 0, ramp_slot = 0, raml_slot = 0, ramp = 0, raml = 0;
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 100 * 4, &packp, 4);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 104 * 4, &ramp_slot, 4);
            uc_mem_read(m->uc, EXT_TABLE_ADDR + 105 * 4, &raml_slot, 4);
            if (ramp_slot) uc_mem_read(m->uc, ramp_slot, &ramp, 4);
            if (raml_slot) uc_mem_read(m->uc, raml_slot, &raml, 4);

            void *hp = NULL;
            const char *pack = arm_str(m, packp);
            if (pack[0] == '$' && ramp && raml) {
                hp = mr_readFile_from_ram(arm_str(m, r0), &fl, (int)r2, arm_ptr(m, ramp), (int)raml);
            } else {
                hp = _mr_readFile(arm_str(m, r0), &fl, (int)r2);
            }
            if (!hp) { ret = 0; break; }
            uint32_t ap = arm_alloc(m, (uint32_t)fl);
            memcpy(arm_ptr(m, ap), hp, fl);
            free(m->last_file_copy);
            m->last_file_copy = malloc((size_t)fl);
            if (m->last_file_copy) {
                memcpy(m->last_file_copy, hp, (size_t)fl);
                m->last_file_addr = ap;
                m->last_file_len = (uint32_t)fl;
            }
            if (r1) memcpy(arm_ptr(m, r1), &fl, 4);
            ret = ap;
        } break;
        case 126: ret = wstrlen(arm_str(m, r0)); break;
        case 127: ret = mr_registerAPP(arm_ptr(m, r0), (int32)r1, (int32)r2); break;
        case 130: ret = _mr_TestCom(NULL, (int)r1, (int)r2); break;
        case 131:
            ret = _mr_TestCom1(NULL, (int)r1, arm_ptr(m, r2), (int32)r3);
            if (r1 == 9 && m->last_file_copy && r2 && r3 <= m->last_file_len && arm_ptr(m, r2)) {
                memcpy(arm_ptr(m, r2), m->last_file_copy, r3);
                uint32_t table_addr = EXT_TABLE_ADDR;
                memcpy(arm_ptr(m, r2), &table_addr, 4);
                m->last_file_addr = r2;
                m->last_file_len = r3;
                if (m->last_alloc_len == sizeof(mr_c_function_P_t) && arm_ptr(m, m->last_alloc_addr)) {
                    m->nested_p_addr = m->last_alloc_addr;
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: staged nested ext at 0x%X len=%u P=0x%X\n",
                           r2, r3, m->nested_p_addr);
                }
            }
            break;
        case 132: {
            int err_pos = -1;
            int out_size = 0;
            uint16 *host = c2u(arm_str(m, r0), r1 ? &err_pos : NULL, &out_size);
            if (r1 && arm_ptr(m, r1)) memcpy(arm_ptr(m, r1), &err_pos, 4);
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &out_size, 4);
            if (host && out_size > 0) {
                uint32_t out_addr = arm_alloc(m, (uint32_t)out_size);
                if (out_addr) {
                    memcpy(arm_ptr(m, out_addr), host, (uint32_t)out_size);
                }
                mr_free(host, (uint32)out_size);
                ret = out_addr;
            } else {
                ret = 0;
            }
        } break;
        case 133: ret = ((int32)r1 == 0) ? 0 : (int32)r0 / (int32)r1; break;
        case 134: ret = ((int32)r1 == 0) ? 0 : (int32)r0 % (int32)r1; break;
        case 145: mr_platDrawChar((uint16)r0, (int32)r1, (int32)r2, (uint32)r3); m->screen_dirty = 1; ret = 0; break;
        default:
            printf("arm_ext_executor: table[%u] not implemented (r0=0x%X r1=0x%X r2=0x%X r3=0x%X)\n", idx, r0, r1, r2, r3);
            ret = MR_IGNORE;
            break;
    }
    cb_ret(m, ret);
}


static bool hook_invalid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)user_data;
    if (type == UC_MEM_FETCH_UNMAPPED && address == 0) {
        uint32_t lr = 0;
        uc_reg_read(uc, UC_ARM_REG_LR, &lr);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: treated fetch from 0 as return to 0x%X\n", lr);
            dumpREG(uc);
        }
        if (lr) {
            uint32_t cpsr = 0;
            uc_reg_read(uc, UC_ARM_REG_CPSR, &cpsr);
            if (lr & 1u) cpsr |= (1u << 5);
            else cpsr &= ~(1u << 5);
            uc_reg_write(uc, UC_ARM_REG_CPSR, &cpsr);
            uc_reg_write(uc, UC_ARM_REG_PC, &lr);
        } else {
            uint32_t pc = EXT_STOP_ADDR;
            uc_reg_write(uc, UC_ARM_REG_PC, &pc);
            uc_emu_stop(uc);
        }
        return true;
    }
    printf("arm_ext_executor: invalid memory %s addr=0x%llX size=%d value=0x%llX\n",
           memTypeStr(type), (unsigned long long)address, size, (unsigned long long)value);
    dumpREG(uc);
    return false;
}

static void hook_screen_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)uc;
    (void)type;
    (void)address;
    (void)size;
    (void)value;
    ArmExtModule *m = (ArmExtModule *)user_data;
    m->screen_write_count++;
    m->screen_dirty = 1;
    if (getenv("VMRP_ARM_EXT_TRACE") && m->screen_write_count <= 20) {
        printf("arm_ext_executor: screen write #%u addr=0x%llX size=%d value=0x%llX\n",
               m->screen_write_count, (unsigned long long)address, size, (unsigned long long)value);
    }
}

static void hook_restore_r9(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (m->outer_r9 && m->nested_return_addr && (uint32_t)address == (m->nested_return_addr & ~1u)) {
        uc_reg_write(uc, UC_ARM_REG_R9, &m->outer_r9);
        if (getenv("VMRP_ARM_EXT_TRACE")) {
            printf("arm_ext_executor: restored outer R9=0x%X at 0x%X\n",
                   m->outer_r9, (uint32_t)address);
        }
        m->outer_r9 = 0;
        m->nested_return_addr = 0;
    }
}

static void patch_wrapper_stack_size(ArmExtModule *m) {
    struct PatchWord {
        uint32_t old_value;
        uint32_t new_value;
    };
    static const struct PatchWord patches[] = {
        {0xe2806d80u, 0xe2806802u}, /* add r6, r0, #0x2000 -> #0x20000 */
        {0xe280bd80u, 0xe280b802u}, /* add fp, r0, #0x2000 -> #0x20000 */
        {0xe2806dc0u, 0xe2806802u}, /* add r6, r0, #0x3000 -> #0x20000 */
        {0xe280bdc0u, 0xe280b802u}, /* add fp, r0, #0x3000 -> #0x20000 */
    };
    uint32_t patch_len = m->code_len < 0x200u ? m->code_len : 0x200u;
    for (uint32_t off = 0; off + 4 <= patch_len; off += 4) {
        uint32_t value = 0;
        memcpy(&value, arm_ptr(m, EXT_CODE_ADDR + off), 4);
        for (size_t i = 0; i < sizeof(patches) / sizeof(patches[0]); ++i) {
            if (value == patches[i].old_value) {
                memcpy(arm_ptr(m, EXT_CODE_ADDR + off), &patches[i].new_value, 4);
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: patched wrapper stack at 0x%X\n", EXT_CODE_ADDR + off);
                }
                break;
            }
        }
    }
}

static void init_table(ArmExtModule *m) {
    for (uint32_t i = 0; i < EXT_TABLE_COUNT; ++i) {
        write_table_entry(m, i, EXT_TABLE_ADDR + i * 4);
    }
    /* Shadow storage for table data slots used by C EXT code. */
    for (uint32_t i = 91; i <= 112; ++i) {
        write_table_entry(m, i, alloc_u32_slot(m, 0));
    }
    write_table_entry(m, 135, alloc_u32_slot(m, 0));
    write_table_entry(m, 136, alloc_u32_slot(m, 0));
    write_table_entry(m, 139, alloc_u32_slot(m, 0));
    write_table_entry(m, 140, alloc_u32_slot(m, 0));
    write_table_entry(m, 142, alloc_u32_slot(m, 0));
    write_table_entry(m, 143, alloc_u32_slot(m, 0));
    write_table_entry(m, 146, alloc_u32_slot(m, 0));

    m->mr_m0_files_addr = arm_alloc(m, 50 * 4);
    m->vm_state_slot = alloc_u32_slot(m, 0);
    m->mr_state_slot = alloc_u32_slot(m, 1); /* MR_STATE_RUN */
    m->bi_slot = alloc_u32_slot(m, 0);
    m->mr_timer_p_slot = alloc_u32_slot(m, 0);
    m->mr_timer_state_slot = alloc_u32_slot(m, 0); /* MR_TIMER_STATE_IDLE */
    m->mr_timer_run_without_pause_slot = alloc_u32_slot(m, 0);
    m->mr_gzin_buf_slot = alloc_u32_slot(m, 0);
    m->mr_gzout_buf_slot = alloc_u32_slot(m, 0);
    m->lg_gzinptr_slot = alloc_u32_slot(m, 0);
    m->lg_gzoutcnt_slot = alloc_u32_slot(m, 0);
    m->mr_sms_cfg_need_save_slot = alloc_u32_slot(m, 0);
    m->internal_table_addr = arm_alloc(m, 17 * 4);
    m->port_table_addr = arm_alloc(m, 4 * 4);
    if (m->internal_table_addr) {
        uint32_t internal[17] = {
            m->mr_m0_files_addr,
            m->vm_state_slot,
            m->mr_state_slot,
            m->bi_slot,
            m->mr_timer_p_slot,
            m->mr_timer_state_slot,
            m->mr_timer_run_without_pause_slot,
            m->mr_gzin_buf_slot,
            m->mr_gzout_buf_slot,
            m->lg_gzinptr_slot,
            m->lg_gzoutcnt_slot,
            m->mr_sms_cfg_need_save_slot,
            EXT_TABLE_ADDR + 12 * 4,
            EXT_TABLE_ADDR + 13 * 4,
            EXT_TABLE_ADDR + 14 * 4,
            EXT_TABLE_ADDR + 15 * 4,
            0,
        };
        memcpy(arm_ptr(m, m->internal_table_addr), internal, sizeof(internal));
        write_table_entry(m, 23, m->internal_table_addr);
    }
    if (m->port_table_addr) {
        write_table_entry(m, 24, m->port_table_addr);
    }

    int32_t sw = mr_screen_w > 0 ? mr_screen_w : 240;
    int32_t sh = mr_screen_h > 0 ? mr_screen_h : 320;
    int32_t bit = 16;
    m->screen_w = sw;
    m->screen_h = sh;
    m->screen_len = (uint32_t)(sw * sh * 2);
    m->screen_addr = arm_alloc(m, m->screen_len);
    if (m->screen_addr && mr_screenBuf) {
        memcpy(arm_ptr(m, m->screen_addr), mr_screenBuf, m->screen_len);
    }
    m->screen_dirty = 0;

    write_table_entry(m, 91, alloc_u32_slot(m, m->screen_addr));
    write_table_entry(m, 92, alloc_u32_slot(m, (uint32_t)sw));
    write_table_entry(m, 93, alloc_u32_slot(m, (uint32_t)sh));
    write_table_entry(m, 94, alloc_u32_slot(m, (uint32_t)bit));

    const uint32_t bitmap_count = 31;
    const uint32_t bitmap_stride = 16;
    const uint32_t bitmapmax = 30;
    uint32_t bitmap_array = arm_alloc(m, bitmap_count * bitmap_stride);
    if (bitmap_array) {
        uint8_t *bm = (uint8_t *)arm_ptr(m, bitmap_array + bitmapmax * bitmap_stride);
        uint16_t sw16 = (uint16_t)sw;
        uint16_t sh16 = (uint16_t)sh;
        uint32_t type = 0;
        memcpy(bm + 0, &sw16, 2);
        memcpy(bm + 2, &sh16, 2);
        memcpy(bm + 4, &m->screen_len, 4);
        memcpy(bm + 8, &type, 4);
        memcpy(bm + 12, &m->screen_addr, 4);
        write_table_entry(m, 95, bitmap_array);
    }

    write_table_entry(m, 100, alloc_string(m, mr_get_pack_filename()));
    write_table_entry(m, 101, alloc_string(m, mr_get_start_filename()));
    write_table_entry(m, 102, alloc_string(m, mr_get_old_pack_filename()));
    write_table_entry(m, 103, alloc_string(m, mr_get_old_start_filename()));
    write_table_entry(m, 138, alloc_string(m, mr_get_start_fileparameter()));

    m->origin_mem_len = 4u * 1024u * 1024u;
    m->origin_mem_left = m->origin_mem_len;
    m->origin_mem_min = m->origin_mem_len;
    m->origin_mem_top = 0;
    m->origin_mem_addr = arm_alloc(m, m->origin_mem_len);
    if (m->origin_mem_addr) {
        uint32_t free_next = m->origin_mem_len;
        uint32_t free_len = m->origin_mem_len;
        memcpy(arm_ptr(m, m->origin_mem_addr), &free_next, 4);
        memcpy((uint8_t *)arm_ptr(m, m->origin_mem_addr) + 4, &free_len, 4);

        write_table_entry(m, 108, alloc_u32_slot(m, m->origin_mem_addr));
        write_table_entry(m, 109, alloc_u32_slot(m, m->origin_mem_len));
        write_table_entry(m, 110, alloc_u32_slot(m, m->origin_mem_addr + m->origin_mem_len));
        write_table_entry(m, 111, alloc_u32_slot(m, m->origin_mem_left));
        write_table_entry(m, 135, alloc_u32_slot(m, m->origin_mem_min));
        write_table_entry(m, 136, alloc_u32_slot(m, m->origin_mem_top));

        uint32_t free_head = arm_alloc(m, 8);
        if (free_head) write_table_entry(m, 146, free_head);
    }
}

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code) {
    if (!out || !code || len < 12) return MR_FAILED;
    ArmExtModule *m = calloc(1, sizeof(*m));
    if (!m) return MR_FAILED;
    m->mem = calloc(1, EXT_MEM_SIZE);
    if (!m->mem) goto fail;
    m->low_table = calloc(1, EXT_LOW_TABLE_SIZE);
    if (!m->low_table) goto fail;
    m->heap_top = EXT_HEAP_ADDR;
    m->code_len = len;
    m->host_code = code;

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &m->uc);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_BASE_ADDR, EXT_MEM_SIZE, UC_PROT_ALL, m->mem);
    if (err != UC_ERR_OK) goto fail;
    init_table(m);
    memcpy(m->low_table, m->mem, EXT_TABLE_COUNT * 4);
    err = uc_mem_map_ptr(m->uc, 0, EXT_LOW_TABLE_SIZE, UC_PROT_ALL, m->low_table);
    if (err != UC_ERR_OK) goto fail;
    memcpy(arm_ptr(m, EXT_CODE_ADDR), code, len);
    patch_wrapper_stack_size(m);
    uint32_t table = EXT_TABLE_ADDR;
    memcpy(arm_ptr(m, EXT_CODE_ADDR), &table, 4);
    err = uc_hook_add(m->uc, &m->hook, UC_HOOK_CODE, hook_table, m, EXT_TABLE_ADDR, EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4);
    if (err != UC_ERR_OK) goto fail;
    uc_hook low_zero_hook;
    err = uc_hook_add(m->uc, &low_zero_hook, UC_HOOK_CODE, hook_low_zero, m, 0, EXT_LOW_TABLE_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    if (getenv("VMRP_ARM_EXT_TRACE_PC")) {
        uc_hook pc_hook;
        err = uc_hook_add(m->uc, &pc_hook, UC_HOOK_CODE, trace_pc, m,
                          EXT_CODE_ADDR, EXT_CODE_ADDR + len);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook restore_hook;
    err = uc_hook_add(m->uc, &restore_hook, UC_HOOK_CODE, hook_restore_r9, m,
                      EXT_CODE_ADDR, EXT_CODE_ADDR + len);
    if (err != UC_ERR_OK) goto fail;
    uc_hook screen_hook;
    if (m->screen_addr && m->screen_len) {
        err = uc_hook_add(m->uc, &screen_hook, UC_HOOK_MEM_WRITE, hook_screen_write, m,
                          m->screen_addr, m->screen_addr + m->screen_len - 1);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook invalid_hook;
    err = uc_hook_add(m->uc, &invalid_hook, UC_HOOK_MEM_INVALID, hook_invalid, m, 1, 0);
    if (err != UC_ERR_OK) goto fail;

    reg_write32(m->uc, UC_ARM_REG_R0, (uint32_t)load_code);
    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int load_ret = run_arm(m, EXT_CODE_ADDR + 8);
    leave_screen_context(m, saved_screenBuf);
    if (load_ret != MR_SUCCESS) goto fail;
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
        } else if (code == 3 || code == 6) {
            /*
             * Some start.mr files pass non-pointer scalar/table markers for
             * these bootstrap messages.  The ARM helper does not dereference
             * them, so keep the raw 32-bit value instead of memcpy'ing from an
             * invalid host address such as 0x1.
             */
            input_addr = (uint32_t)(uintptr_t)input;
        } else {
            input_addr = arm_alloc(m, input_len ? input_len : 1);
            if (!input_addr) return MR_FAILED;
            if (input_len) memcpy(arm_ptr(m, input_addr), input, input_len);
        }
    }
    uint32_t outp_addr = arm_alloc(m, 4);
    uint32_t outl_addr = arm_alloc(m, 4);
    // Lifecycle/event calls belong to the app EXT, not later helper EXT modules.
    uint32_t call_p_addr = (code >= 0 && code <= 5 && m->primary_p_addr) ? m->primary_p_addr : (m->active_p_addr ? m->active_p_addr : m->p_addr);
    uint32_t call_helper_addr = (code >= 0 && code <= 5 && m->primary_helper_addr) ? m->primary_helper_addr : (m->active_helper_addr ? m->active_helper_addr : m->helper_addr);
    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, call_p_addr), 4);
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    reg_write32(m->uc, UC_ARM_REG_R0, call_p_addr);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)code);
    reg_write32(m->uc, UC_ARM_REG_R2, input_addr);
    reg_write32(m->uc, UC_ARM_REG_R3, input_len);
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 16;
    uint32_t helper_args[4] = {outp_addr, outl_addr, 0, 0};
    uc_mem_write(m->uc, sp, helper_args, sizeof(helper_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int call_ret = run_arm_with_sp(m, call_helper_addr, sp);
    leave_screen_context(m, saved_screenBuf);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    uint32_t arm_output = 0;
    uint32_t arm_output_len = 0;
    uc_mem_read(m->uc, outp_addr, &arm_output, 4);
    uc_mem_read(m->uc, outl_addr, &arm_output_len, 4);
    if (output_len) *output_len = (int32)arm_output_len;
    if (output) *output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
    return (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
}

uint32 arm_ext_helper_addr(ArmExtModule *m) {
    return m ? m->helper_addr : 0;
}

int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, 0);
    reg_write32(m->uc, UC_ARM_REG_R1, 0);
    reg_write32(m->uc, UC_ARM_REG_R2, 0);
    reg_write32(m->uc, UC_ARM_REG_R3, 0);

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int call_ret = run_arm(m, func);
    leave_screen_context(m, saved_screenBuf);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    return MR_SUCCESS;
}

int arm_ext_invoke3(ArmExtModule *m, uint32 func, uint32 arg0, uint32 arg1,
                    uint32 arg2, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, arg0);
    reg_write32(m->uc, UC_ARM_REG_R1, arg1);
    reg_write32(m->uc, UC_ARM_REG_R2, arg2);

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int call_ret = run_arm(m, func);
    leave_screen_context(m, saved_screenBuf);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    return MR_SUCCESS;
}

void arm_ext_unload(ArmExtModule *m) {
    if (!m) return;
    if (m->uc) uc_close(m->uc);
    free(m->last_file_copy);
    free(m->low_table);
    free(m->mem);
    free(m);
}
