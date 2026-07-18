#include "./include/arm_ext_executor.h"
#include "./include/compat_msvc.h"
#include "./include/arm_ext_internal.h"
#include "./include/arm_ext_priv.h"
#include "./include/vmrp.h"
#include "./include/app_compat.h"
#include "./include/bridge.h"
#include "./include/network.h"
#include "./include/fileLib.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/mman.h>
#else
#include <windows.h>
#endif
#include <zlib.h>

#include "./include/utils.h"
#include "./include/vmrp.h"

extern void mr_printf(const char *format, ...);
extern uint16 *mr_screenBuf;
extern int32 mr_screen_w;
extern int32 mr_screen_h;
extern int32 mr_rand(void);
extern void mr_free(void *p, uint32 len);
extern int32 mr_mem_get(char **mem_base, uint32 *mem_len);
extern int32 mr_timerStart(uint16 t);
extern int32 mr_timerStop(void);
extern int32 mr_state;
extern int32 mr_timer_state;
extern int32 mr_stop_ex(int16 freemem);
extern uint32 mr_getTime(void);
extern int32 mr_getDatetime(mr_datetime *datetime);
extern int32 mr_getUserInfo(void *info);
extern void mr_md5_init(void *pms);
extern void mr_md5_append(void *pms, const void *data, int nbytes);
extern void mr_md5_finish(void *pms, void *digest);
extern int32 mr_sleep(uint32 ms);
extern int32 mr_plat(int32 code, int32 param);
extern const char *mr_get_pack_filename(void);
extern const char *mr_get_start_filename(void);
extern const char *mr_get_old_pack_filename(void);
extern const char *mr_get_old_start_filename(void);
extern const char *mr_get_start_fileparameter(void);
extern void mr_set_pack_filename(const char *name);
extern void mr_set_start_filename(const char *name);
extern void mr_set_old_pack_filename(const char *name);
extern void mr_set_old_start_filename(const char *name);
extern void mr_set_start_fileparameter(const char *name);
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
extern int32 mr_sendSms(char *pNumber, char *pContent, int32 encode);
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

/* mrporting.h 中定义的 mr_info 返回值，此处直接使用数值避免引入额外头文件 */
/* mr_sendSms flag requesting an asynchronous MR_SMS_RESULT event. */

extern void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
extern void _DrawBitmap(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
extern void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
extern int32 _DrawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
extern int _BitmapCheck(uint16 *p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
extern void *_mr_readFile(const char *filename, int *filelen, int lookfor);
extern void *mr_readFile_from_ram(const char *filename, int *filelen, int lookfor, char *ram_file, int ram_file_len);
extern void *mr_readFile_from_pack(const char *pack_filename, const char *filename, int *filelen, int lookfor);
extern char *LG_mem_base;
extern char *LG_mem_end;
extern uint8 *mr_gzInBuf;
extern uint8 *mr_gzOutBuf;
extern unsigned LG_gzinptr;
extern unsigned LG_gzoutcnt;
extern int32 mr_registerAPP(uint8 *p, int32 len, int32 index);
extern int _mr_TestCom(void *L, int input0, int input1);
extern int _mr_TestCom1(void *L, int input0, char *input1, int32 len);
extern int32 mr_socket(int32 type, int32 protocol);
extern int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type);
extern int32 mr_closeSocket(int32 s);
extern int32 mr_getSocketState(int32 s);
extern int32 mr_initNetwork(MR_INIT_NETWORK_CB_t cb, const char *mode);
extern int32 mr_recv(int32 s, char *buf, int len);
extern int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);
extern int32 mr_send(int32 s, const char *buf, int len);
extern int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);
extern int32 my_hasOpenSockets(void);
extern int32 my_openSocketCount(void);
extern const char *mr_get_last_written_mrp_path(void);
extern const uint8 *mr_get_last_written_mrp_data(uint32 *len);

uint32_t arm_addr(ArmExtModule *m, const void *ptr) {
    if (ptr == NULL) return 0;
    const uint8_t *p = (const uint8_t *)ptr;
    if (m && m->mem && p >= m->mem && p < m->mem + EXT_MEM_SIZE)
        return (uint32_t)(p - m->mem) + EXT_BASE_ADDR;
    if (m && m->platform_mem && p >= m->platform_mem &&
        p < m->platform_mem + EXT_PLATFORM_MEM_SIZE)
        return (uint32_t)(p - m->platform_mem) + EXT_PLATFORM_MEM_ADDR;
    if (m && m->scrram_mem && p >= m->scrram_mem &&
        p < m->scrram_mem + EXT_SCRRAM_SIZE)
        return (uint32_t)(p - m->scrram_mem) + EXT_SCRRAM_ADDR;
    if (m && m->platform_io_mem && p >= m->platform_io_mem &&
        p < m->platform_io_mem + EXT_PLATFORM_IO_MEM_SIZE)
        return (uint32_t)(p - m->platform_io_mem) + EXT_PLATFORM_IO_MEM_ADDR;
    if (m && m->platform_alt_mem && p >= m->platform_alt_mem &&
        p < m->platform_alt_mem + EXT_PLATFORM_ALT_MEM_SIZE)
        return (uint32_t)(p - m->platform_alt_mem) + EXT_PLATFORM_ALT_MEM_ADDR;
    if (m && m->executor_meta_mem && p >= m->executor_meta_mem &&
        p < m->executor_meta_mem + EXT_EXECUTOR_META_SIZE)
        return (uint32_t)(p - m->executor_meta_mem) + EXT_EXECUTOR_META_ADDR;
    return 0;
}


static int arm_ext_screen_owner_is_visible(ArmExtModule *m,
                                           uint32_t owner_p_addr,
                                           uint32_t owner_helper_addr) {
    if (!m || !owner_p_addr) return 1;
    if (!arm_ext_has_foreground_child(m)) return 1;
    /*
     * A loaded child helper is not a visible occlusion by itself.  Private
     * loaders also register support EXTs through the same table[25] ABI; only
     * an accepted child present creates foreground cover rows that must block
     * lower-layer framebuffer writes.
     */
    if (!arm_ext_has_foreground_cover(m)) return 1;
    return owner_p_addr == m->active_p_addr ||
           owner_helper_addr == m->active_helper_addr;
}

static int arm_ext_current_screen_owner_is_visible(ArmExtModule *m) {
    if (!m) return 1;
    uint32_t helper_addr = 0;
    uint32_t p_addr =
        arm_ext_p_for_code_addr(m, reg_read32(m->uc, UC_ARM_REG_PC),
                                &helper_addr);
    if (!p_addr) {
        p_addr = m->current_p_addr;
        helper_addr = m->current_helper_addr;
    }
    return arm_ext_screen_owner_is_visible(m, p_addr, helper_addr);
}

static void arm_ext_sync_r9_for_code_addr(ArmExtModule *m, uint32_t addr) {
    uint32_t pc = addr & ~1u;
    uint32_t p_addr = 0;
    if (pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
        uint32_t current_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
        for (int i = 0; i < m->nested_module_count; ++i) {
            uint32_t nested_rw = 0;
            if (arm_ptr(m, m->nested_modules[i].p_addr)) {
                memcpy(&nested_rw, arm_ptr(m, m->nested_modules[i].p_addr), 4);
            }
            if (nested_rw && current_r9 == nested_rw) {
                p_addr = m->p_addr;
                break;
            }
        }
    } else {
        ArmExtNestedModule *mod = arm_ext_find_nested_module(m, pc);
        if (mod) {
            p_addr = mod->p_addr;
            /*
             * 同一份 EXT 代码被 wrapper 实例化多次时（dota 的 screen 模块
             * 0x661AC0 同时有 loading / menu 等多个实例，file_addr 相同、rw 各异），
             * arm_ext_find_nested_module 只能按 PC 区间匹配，返回“最后登记”的实例。
             * 当正在执行的是一个 wrapper 刚 BLX 进去、但尚未登记进 nested_modules
             * 的新实例时（game 自己已经把 sb 装成新实例的 rw），按“最后登记实例”纠
             * R9 会把 game 设好的正确值覆盖成旧实例的 rw，使该模块所有 sb 相对全局
             * 访问错位。dota 实证：menu 屏消息处理器以 loading 实例的 R9 运行，
             * memset(sb-0xA44) 落到 primary 共享的 cg_1 背景条结构、清 0 其指针，
             * 主界面 y=80..159 变黑带。
             *
             * 判据（数据驱动，无 app 指纹）：仅当该代码段被同 file_addr 的多个实例
             * 共享（歧义），且当前 R9 不等于任何“已知上下文”的 rw —— 既不是任何
             * 已登记 nested 实例、也不是 primary、也不是 wrapper(m->p_addr) 的 rw ——
             * 时，认定它是 game 刚设好的“新建未登记实例”的 sb，予以信任、不覆盖。
             * 残留的旧 R9 必然等于某个已知上下文的 rw，仍走原纠正逻辑；单实例模块
             * （same_code==1）完全不受影响。验证：dota/gghjt/gxdzc e2e
             * 回归通过。
             */
            int same_code = 0;
            for (int i = 0; i < m->nested_module_count; ++i) {
                if (m->nested_modules[i].file_addr == mod->file_addr &&
                    m->nested_modules[i].file_len)
                    same_code++;
            }
            if (same_code > 1) {
                uint32_t cur_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
                int recognized = 0;
                if (cur_r9 == arm_ext_read_u32_or_zero_(m, m->primary_p_addr) ||
                    cur_r9 == arm_ext_read_u32_or_zero_(m, m->p_addr))
                    recognized = 1;
                for (int i = 0; !recognized && i < m->nested_module_count; ++i) {
                    if (cur_r9 ==
                        arm_ext_read_u32_or_zero_(m, m->nested_modules[i].p_addr))
                        recognized = 1;
                }
                if (cur_r9 && !recognized) return; /* 新建未登记实例的 sb，信任 game */
            }
        }
    }
    if (!p_addr || !arm_ptr(m, p_addr)) return;

    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (!rw_base) return;

    uint32_t current_r9 = reg_read32(m->uc, UC_ARM_REG_R9);
    if (current_r9 != rw_base) {
        reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    }
}

ArmExtNestedModule *arm_ext_resource_owner_for_lr(ArmExtModule *m,
                                                         uint32_t *owner_p,
                                                         uint32_t *owner_helper) {
    uint32_t helper = 0;
    uint32_t p = 0;
    ArmExtNestedModule *owner = NULL;
    if (owner_p) *owner_p = 0;
    if (owner_helper) *owner_helper = 0;
    if (!m) return NULL;

    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    p = arm_ext_p_for_code_addr(m, lr, &helper);
    owner = arm_ext_find_nested_module_by_p(m, p);
    if (!owner) {
        uint32_t return_pc = lr & ~1u;
        uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
        /* Shared wrapper thunks hide the child caller from table bridges: LR
         * names the thunk's instruction after BLX, while the thunk has saved
         * the child's LR in its small stack frame.  Recover only a structurally
         * valid Thumb call return into a registered child, and only when the
         * direct return belongs to the wrapper.  The bounded scan avoids using
         * unrelated active/foreground state as a resource-owner fallback. */
        if (return_pc >= EXT_CODE_ADDR &&
            return_pc < EXT_CODE_ADDR + m->code_len) {
            for (uint32_t off = 0; off < 16u * 4u; off += 4u) {
                uint32_t candidate = 0;
                if (!arm_ptr_span(m, sp + off, 4u)) break;
                memcpy(&candidate, arm_ptr(m, sp + off), 4u);
                if (!(candidate & 1u) || (candidate & ~1u) < 2u) continue;

                uint32_t caller_pc = candidate & ~1u;
                uint32_t stack_helper = 0;
                uint32_t stack_p = arm_ext_p_for_code_addr(
                    m, candidate, &stack_helper);
                ArmExtNestedModule *stack_owner =
                    arm_ext_find_nested_module_by_p(m, stack_p);
                if (!stack_owner) continue;
                if (!arm_ptr_span(m, caller_pc - 2u, 2u)) continue;
                uint16_t call = 0;
                memcpy(&call, arm_ptr(m, caller_pc - 2u), 2u);
                /* Private children enter wrapper bridges through BLX Rm.  A
                 * matching return address proves this stack word is a call
                 * frame, rather than data that happens to fall in code RAM. */
                if ((call & 0xFF87u) != 0x4780u) continue;
                p = stack_p;
                helper = stack_helper;
                owner = stack_owner;
                break;
            }
        }
    }
    if (owner_p) *owner_p = p;
    if (owner_helper) *owner_helper = helper;
    return owner;
}

void *arm_ext_read_child_resource(ArmExtModule *m,
                                         ArmExtNestedModule *owner,
                                         const char *read_name,
                                         int *file_len,
                                         int lookfor,
                                         char *read_pack_host_path,
                                         uint32_t *read_pack_ram_addr,
                                         uint32_t *read_pack_ram_len) {
    void *hp = NULL;
    if (!m || !owner || !read_name) return NULL;
    /*
     * Private loaders give each child a stable package owner when the staged
     * executable is byte-proven.  A later shared table[100] value can still be
     * the outer app because wrappers reuse one writable pack_filename buffer;
     * child resource calls are scoped by LR to the child's recorded owner.
     */
    if (owner->package_ram_addr && owner->package_ram_len &&
        arm_ptr(m, owner->package_ram_addr)) {
        hp = mr_readFile_from_ram(read_name, file_len, lookfor,
                                  arm_ptr(m, owner->package_ram_addr),
                                  (int)owner->package_ram_len);
        if (hp) {
            if (read_pack_ram_addr) *read_pack_ram_addr = owner->package_ram_addr;
            if (read_pack_ram_len) *read_pack_ram_len = owner->package_ram_len;
        }
        return hp;
    }
    if (owner->package_host_path[0]) {
        hp = mr_readFile_from_pack(owner->package_host_path, read_name,
                                   file_len, lookfor);
        if (hp && read_pack_host_path) {
            snprintf(read_pack_host_path, PATH_MAX, "%s",
                     owner->package_host_path);
        }
    }
    return hp;
}

int arm_ext_child_has_package_owner(ArmExtModule *m,
                                           ArmExtNestedModule *owner) {
    if (!owner) return 0;
    if (owner->package_host_path[0]) return 1;
    return owner->package_ram_addr && owner->package_ram_len &&
           arm_ptr(m, owner->package_ram_addr);
}


static void write_table_entry(ArmExtModule *m, uint32_t index, uint32_t value) {
    memcpy((uint8_t *)arm_ptr(m, EXT_TABLE_ADDR + index * 4), &value, 4);
}

static uint32_t alloc_u32_slot(ArmExtModule *m, uint32_t value) {
    uint32_t slot = arm_alloc(m, 4);
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
    return slot;
}

static uint32_t alloc_filename_table_slot(ArmExtModule *m, const char *value) {
    uint32_t slot = arm_alloc(m, ARM_EXT_PACK_TABLE_SIZE);
    char *dst = slot ? (char *)arm_ptr(m, slot) : NULL;
    if (!dst) return 0;
    /* table[100..103] are arrays in native Mythroad, not exact-length strings;
     * guests legitimately clear/copy a fixed prefix when switching packages. */
    memset(dst, 0, ARM_EXT_PACK_TABLE_SIZE);
    snprintf(dst, ARM_EXT_PACK_TABLE_SIZE, "%s", value ? value : "");
    return slot;
}

static uint32_t alloc_start_parameter_table_slot(ArmExtModule *m,
                                                 const char *value) {
    uint32_t slot = arm_alloc(m, ARM_EXT_PACK_TABLE_SIZE);
    char *dst = slot ? (char *)arm_ptr(m, slot) : NULL;
    if (!dst) return 0;
    /* table[138] mirrors the host start_fileparameter[128] verbatim.  The
     * buffer carries binary continuation records past the first NUL (Cookie's
     * "_RL\0" + trailing big-endian view words), so seed the whole array. */
    memcpy(dst, value, ARM_EXT_PACK_TABLE_SIZE);
    return slot;
}

static int arm_ext_host_path_is_absolute(const char *path) {
    if (!path || !path[0]) return 0;
#ifdef _WIN32
    if (isalpha((unsigned char)path[0]) && path[1] == ':') return 1;
    return (path[0] == '\\' && path[1] == '\\');
#else
    return path[0] == '/';
#endif
}

static const char *arm_ext_guest_pack_alias_for_handoff(ArmExtModule *m,
                                                       const char *host_value) {
    if (!host_value || !host_value[0]) return "";
    if (!arm_ext_host_path_is_absolute(host_value)) return host_value;
    /* Return packages are copied by legacy helpers through fixed 32-byte ARM
     * buffers.  Keep the guest token short and retain the host path in the
     * executor alias map for lifecycle synchronization and file bridges. */
    const char *alias = arm_ext_register_short_pack_alias(m, host_value);
    return (alias && alias[0]) ? alias : host_value;
}

uint32_t alloc_bytes(ArmExtModule *m, const void *value, uint32_t len) {
    uint32_t slot = arm_alloc(m, len);
    if (slot && value && len) memcpy(arm_ptr(m, slot), value, len);
    return slot;
}

static uint32_t arm_ext_pack_table_slot(ArmExtModule *m) {
    if (!m) return 0;
    if (!m->pack_table_addr) {
        m->pack_table_addr = arm_alloc(m, ARM_EXT_PACK_TABLE_SIZE);
    }
    return m->pack_table_addr;
}

void arm_ext_set_pack_table_name(ArmExtModule *m, const char *name) {
    uint32_t slot = arm_ext_pack_table_slot(m);
    char *dst;
    if (!m || !slot) return;
    dst = (char *)arm_ptr(m, slot);
    if (!dst) return;
    /*
     * Native table[100] points at writable pack_filename[128], not an
     * immutable exact-length string.  EXT helpers copy shorter package names
     * such as c:/mythroad/ into that storage; zero-fill first so stale bytes
     * from a previous longer host path cannot survive past the new NUL.
     */
    memset(dst, 0, ARM_EXT_PACK_TABLE_SIZE);
    snprintf(dst, ARM_EXT_PACK_TABLE_SIZE, "%s", name ? name : "");
    write_table_entry(m, 100, slot);
}

void arm_ext_set_child_record_pack_name(ArmExtModule *m,
                                               uint32_t file_addr,
                                               const char *name) {
    if (!m || !file_addr || !name || !name[0]) return;
    uint32_t record = arm_ext_read_u32_or_zero_(m, file_addr);
    if (!record || !arm_ptr(m, record + 100u * 4u)) return;
    ArmExtNestedModule *mod = arm_ext_find_nested_module(m, file_addr + 8u);
    uint32_t slot = (mod && mod->file_addr == file_addr)
                  ? mod->pack_name_addr
                  : 0;
    if (!slot) {
        uint32_t existing = arm_ext_read_u32_or_zero_(
            m, record + 100u * 4u);
        if (existing >= EXT_EXECUTOR_META_ADDR &&
            existing < EXT_EXECUTOR_META_ADDR + EXT_EXECUTOR_META_SIZE &&
            arm_ptr(m, existing)) {
            slot = existing;
        }
    }
    if (!slot) {
        slot = arm_ext_meta_alloc(m, ARM_EXT_PACK_TABLE_SIZE);
    }
    if (!slot) return;
    if (mod && mod->file_addr == file_addr)
        mod->pack_name_addr = slot;
    char *dst = (char *)arm_ptr(m, slot);
    if (!dst) return;
    /*
     * Private loaders give each child a per-module record that mirrors the
     * EXT table.  DOTA frame.ext's package getter at 0x2C916C reads
     * record[100], not the shared EXT_TABLE[100], before the fixed 32-byte
     * copies at 0x2C9810.  brwmain.ext startup repeats the same ABI hazard at
     * 0x2D1E0C: memset 32 bytes, strlen(pack), memcpy(strlen).  Keep that
     * ARM-visible record slot short too; host readFile ownership still comes
     * from the recorded package provenance.
     * The pointer is executor metadata, not a late low-heap allocation: the
     * browser crash proved record[100] at child P+0x14 can be executed as a
     * wrapper callback when it is placed immediately after the private P.
     */
    memset(dst, 0, ARM_EXT_PACK_TABLE_SIZE);
    snprintf(dst, ARM_EXT_PACK_TABLE_SIZE, "%s", name);
    memcpy(arm_ptr(m, record + 100u * 4u), &slot, 4);
}

void internal_slot_write(ArmExtModule *m, uint32_t slot, uint32_t value) {
    if (slot) memcpy(arm_ptr(m, slot), &value, 4);
}

int internal_slot_read(ArmExtModule *m, uint32_t slot, uint32_t *value) {
    if (!m || !slot || !value || !arm_ptr(m, slot)) return 0;
    memcpy(value, arm_ptr(m, slot), 4);
    return 1;
}

static uint32_t arm_ext_map_read_file_host_ptr(ArmExtModule *m,
                                               const void *host_ptr,
                                               const void *read_file_ret,
                                               uint32_t arm_ret) {
    if (!m || !host_ptr) return 0;
    if (read_file_ret && host_ptr == read_file_ret) return arm_ret;
    return arm_addr(m, host_ptr);
}

void arm_ext_sync_read_file_gzip_slots(ArmExtModule *m,
                                              const void *read_file_ret,
                                              uint32_t arm_ret) {
    if (!m || !read_file_ret || !arm_ret) return;

    uint32_t arm_gzin = arm_ext_map_read_file_host_ptr(
        m, mr_gzInBuf, read_file_ret, arm_ret);
    uint32_t arm_gzout = arm_ext_map_read_file_host_ptr(
        m, mr_gzOutBuf, read_file_ret, arm_ret);
    if (arm_gzin) {
        internal_slot_write(m, m->mr_gzin_buf_slot, arm_gzin);
    }
    if (arm_gzout) {
        internal_slot_write(m, m->mr_gzout_buf_slot, arm_gzout);
    }
    internal_slot_write(m, m->lg_gzinptr_slot, (uint32_t)LG_gzinptr);
    internal_slot_write(m, m->lg_gzoutcnt_slot, (uint32_t)LG_gzoutcnt);
}

static int arm_ext_host_lg_mem_contains(const void *p, uint32_t len) {
    const char *c = (const char *)p;
    if (!c || !LG_mem_base || !LG_mem_end || c < LG_mem_base || c >= LG_mem_end)
        return 0;
    if (!len) return 1;
    return (size_t)(LG_mem_end - c) >= (size_t)len;
}

void arm_ext_release_host_read_file_buffer(const void *hp,
                                                  uint32_t len) {
    /*
     * table[125] copies host _mr_readFile output into ARM-visible LG_mem before
     * returning.  The original host mr_malloc buffer is therefore only bridge
     * staging; ARM code can later free the ARM copy, but it can never free this
     * host allocation.  Releasing it here keeps repeated download/cancel loops
     * from exhausting host LG_mem while preserving direct RAM-package pointers.
     */
    if (len && arm_ext_host_lg_mem_contains(hp, len)) {
        mr_free((void *)hp, len);
    }
}

/*
 * 已删除 arm_ext_mirror_read_file_to_ram_source():该逻辑在 table[125]
 * (mr_readFile_from_ram, pack='$') 返回后,把解压输出(fl 字节)memcpy 回
 * guest 传入的压缩源缓冲区(raml 字节)。原生实现(src/mythroad/mythroad.c
 * _mr_readFile 的 '$' 分支)解压到新分配的 mr_gzOutBuf 并返回该指针,
 * 从不改写 RAM 源镜像。由于压缩数据必然 fl > raml,该回写每次越界
 * fl-raml 字节,连续踩坏 guest 堆中相邻的位图数据(talkcat 主界面短横线
 * 花屏)和 wrapper 元数据(偶发 0xE83A55 定时器派发后 PC 落入栈页的
 * UC_ERR_INSN_INVALID 崩溃)。验证:清空 workdir 运行 talkcat.mrp,
 * PPM 无横线噪点且无 uc_emu_start 失败。
 */

static void sync_internal_state_to_arm(ArmExtModule *m) {
    if (!m) return;
    internal_slot_write(m, m->mr_state_slot, (uint32_t)mr_state);
    internal_slot_write(m, m->mr_timer_state_slot, (uint32_t)mr_timer_state);
}

/*
 * Guest 侧 LG_mem first-fit 分配器,链表操作逐语句对应 src/mythroad/mem.c
 * 的 mr_malloc/mr_free。原生 ABI 中 table[0]/[1]/[2] 就是这三个函数,且
 * table[146] 直接暴露 &LG_mem_free 头节点,应用可以读改空闲链表。
 * mythroad.c:1290 记载的"骚操作"依赖该语义:应用先 mr_free 一块自有
 * 内存到空闲链表,再调 readFile(table[125]),其内部 mr_malloc(解压
 * 输出)按 first-fit 必然落回该块,应用随后【丢弃 readFile 返回值】
 * 直接从预测地址读取像素(talkcat 图标路径,cfunction.ext 0xE82744
 * 反汇编确认 0xE827A8 调用后 r0 未被保存)。
 * 此前 table[0] 用 arm_alloc(bump,从不复用)、table[1] 仅做统计,
 * 空闲链表从不运转,预测地址处残留的 MRP 索引字节被当作位图渲染,
 * 即 talkcat 主界面短横线花屏的根因。
 *
 * 容量设计(兼容性权衡,经 e2e 回归校准):
 * - 空闲链表只管理 512KB origin_mem 池,首次尝试从池内 first-fit 分配,
 *   使"free 后 readFile 复用"的地址预测在池内成立;
 * - 池内无合适块时由调用方退回 arm_alloc(bump)。此前 table[0] 一直是
 *   bump(实际容量 16MB),dota/opbzqe/optwar 等游戏的内存检测和资源
 *   加载依赖远超 512KB 的累计分配,若像真机一样在池满时直接失败会
 *   黑屏(e2e 全量回归证实);bump 后备保持与修复前完全相同的容量。
 * - LG_mem_left/min/top 统计沿用 note_origin_mem_alloc/free 的既有账本
 *   (所有 table[0]/[1] 调用无论落在池内或 bump 都记账),与修复前的
 *   ARM 可见值一致,不影响依赖该预算的游戏(见 origin_mem_len 注释)。
 * - 部分旧 wrapper 会临时把 0x40000000 映射带的 arena 接到 free-list,
 *   并直接改 table[108]/[110]/[136] 和 table[146] 指向的链表。
 *   table bridge 每次从这些 ARM 可见 slot 读取当前 base/end/head/统计,
 *   不能继续使用 init_table 时的固定副本;
 *   否则 host 与 guest 会遍历两个不同的链表边界并在 teardown 中成环。
 * 验证:talkcat.mrp 主界面 PPM 无花屏;e2e 全量回归与基线一致。
 */

/* mem.c realLGmemSize:空闲节点 {next,len} 占 8 字节,块长按 8 对齐 */
static void cb_ret(ArmExtModule *m, uint32_t ret) {
    reg_write32(m->uc, UC_ARM_REG_R0, ret);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
    arm_ext_note_table_return_guard(m, lr, sp);
    set_arm_mode_for_addr(m, lr);
    reg_write32(m->uc, UC_ARM_REG_PC, lr);
}

/* ARM ext 跳转到低地址区（0x0 ~ EXT_LOW_TABLE_SIZE）时的处理。
 *
 * 两种场景会到这里：
 * a) 未经重定位的嵌套 ext 用低地址 table 指针（如 0x8 = table[2]），
 *    发生在宿主侧预解压后跳过了 ARM 侧的重定位。此时应将执行重定向
 *    到主表对应条目，由 hook_table 正常分发。
 * b) 无效跳转（如 0x1000），按 return-to-LR 兜底。 */
static void hook_low_zero(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    /* 场景 a：未经重定位的 ext 通过低地址调用 table 函数。
     * 不做真实分发（参数可能是给嵌套 ext 函数而非 table 函数的），
     * 安全地返回 0（NULL/失败），让调用方走错误处理路径。 */
    if (address < EXT_TABLE_COUNT * 4) {
        if (m && arm_ext_diag_on()) {
            static uint32_t low_zero_diag_count = 0;
            uint32_t idx = (uint32_t)(address / 4u);
            uint32_t lr_helper = 0;
            uint32_t lr_p = arm_ext_p_for_code_addr(m, lr, &lr_helper);
            ArmExtNestedModule *lr_mod =
                arm_ext_find_nested_module_by_p(m, lr_p);
            /*
             * Downloader disassembly calls SDK bridges through BLX-loaded
             * table slots.  If a private child still holds an unresolved low
             * self-pointer (for example 81*4 for mr_initNetwork), this hook is
             * the only place it can disappear before hook_table sees it.
             * Keep the diagnostic bounded because bad bridge state can loop.
             */
            if (low_zero_diag_count < 512u) {
                printf("DIAG low_zero slot=%u addr=0x%llX lr=0x%X lrP=0x%X lrH=0x%X lrFile=0x%X/%u r0=0x%X r1=0x%X r2=0x%X r3=0x%X r9=0x%X activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X\n",
                       idx, (unsigned long long)address, lr, lr_p,
                       lr_helper, lr_mod ? lr_mod->file_addr : 0,
                       lr_mod ? lr_mod->file_len : 0,
                       reg_read32(uc, UC_ARM_REG_R0),
                       reg_read32(uc, UC_ARM_REG_R1),
                       reg_read32(uc, UC_ARM_REG_R2),
                       reg_read32(uc, UC_ARM_REG_R3),
                       reg_read32(uc, UC_ARM_REG_R9),
                       m->active_p_addr, m->active_helper_addr,
                       m->current_p_addr, m->current_helper_addr);
            } else if (low_zero_diag_count == 512u) {
                printf("DIAG low_zero truncated after 512 entries\n");
            }
            low_zero_diag_count++;
        }
        reg_write32(uc, UC_ARM_REG_R0, 0);
    }
    /* 场景 b：超出 table 范围 → return to LR */
    if (lr) {
        set_arm_mode_for_addr(m, lr);
        reg_write32(uc, UC_ARM_REG_PC, lr);
    } else {
        reg_write32(uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
        uc_emu_stop(uc);
    }
}

static void hook_table(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)uc; /* 统一通过 m->uc 访问引擎;参数由 Unicorn hook ABI 固定 */
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (address < EXT_TABLE_ADDR || address >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) return;
    uint32_t idx = (uint32_t)((address - EXT_TABLE_ADDR) / 4);
    if (m->pending_internal_file_addr) {
        arm_ext_sync_internal_nested_module(m, m->pending_internal_file_addr,
                                            m->pending_internal_file_len);
    }
    capture_timer_dispatches(m);
    uint32_t r0 = arg_read(m, 0), r1 = arg_read(m, 1), r2 = arg_read(m, 2), r3 = arg_read(m, 3);
    int trace_table = arm_ext_trace_on();
    if (trace_table) {
        uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
        printf("arm_ext_executor: table[%u](0x%X,0x%X,0x%X,0x%X) lr=0x%X\n", idx, r0, r1, r2, r3, lr);
    }
    /* ARM ext 常用 mr_getTime 忙等等待动画/定时器间隔。timerStart/Stop
     * 可能夹在同一段等待循环里，因此与 getTime 一起保留连续计数。 */
    if (idx != 33 && idx != 31 && idx != 32) {
        m->busy_wait_count = 0;
        m->busy_wait_start_ms = 0;
    }

    /* Phase 3 完成:全部 case 已函数化(aex_table.c);未实现编号保持
     * 原 default 语义(打印 + MR_IGNORE) */
    AexTableHandler h = aex_table_handlers[idx];
    if (h) {
        AexTableCtx tc = { idx, r0, r1, r2, r3, MR_SUCCESS };
        h(m, &tc);
        cb_ret(m, tc.ret);
        if (vmrp_is_exited()) {
            /* table[54] may return into guest code after requesting a platform
             * exit. Stop here so a guest-side exit loop cannot pin the worker. */
            reg_write32(uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
            uc_emu_stop(uc);
        }
        return;
    }
    printf("arm_ext_executor: table[%u] not implemented (r0=0x%X r1=0x%X r2=0x%X r3=0x%X)\n", idx, r0, r1, r2, r3);
    cb_ret(m, (uint32_t)MR_IGNORE);
}


static bool hook_invalid(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (type == UC_MEM_FETCH_UNMAPPED && address == 0) {
        uint32_t lr = 0;
        uc_reg_read(uc, UC_ARM_REG_LR, &lr);
        if (arm_ext_trace_on()) {
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
    {
        uint32_t pc = reg_read32(uc, UC_ARM_REG_PC);
        uint32_t sp = reg_read32(uc, UC_ARM_REG_SP);
        uint32_t cpsr = reg_read32(uc, UC_ARM_REG_CPSR);
        int thumb = (cpsr >> 5) & 1;
        printf("arm_ext_executor: invalid memory %s addr=0x%llX size=%d value=0x%llX\n",
               memTypeStr(type), (unsigned long long)address, size, (unsigned long long)value);
        printf("  crash PC=0x%X (%s) last_file=0x%X..0x%X\n",
               pc, thumb ? "thumb" : "arm",
               m->last_file_addr, m->last_file_addr + m->last_file_len);
        if (arm_ext_diag_on()) {
            uint32_t r9 = reg_read32(uc, UC_ARM_REG_R9);
            arm_ext_diag_dump_layer_state(m, "invalid");
            arm_ext_diag_dump_rw_timer_state(m, "invalid-r9", r9);
        }
        /* 打印 crash PC 前32字节和后16字节，便于反汇编定位上下文 */
        {
            uint32_t pre_start = (pc > 32) ? (pc - 32) : 0;
            uint32_t pre_len = pc - pre_start;
            uint8_t pre_bytes[32];
            if (pre_len > 0 && uc_mem_read(uc, pre_start, pre_bytes, pre_len) == UC_ERR_OK) {
                printf("  bytes @0x%X (before PC):", pre_start);
                for (uint32_t i = 0; i < pre_len; i++) printf(" %02X", pre_bytes[i]);
                printf("\n");
            }
        }
        uint8_t bytes[16];
        if (uc_mem_read(uc, pc, bytes, 16) == UC_ERR_OK) {
            printf("  bytes @0x%X (at PC):", pc);
            for (int i = 0; i < 16; i++) printf(" %02X", bytes[i]);
            printf("\n");
        }
        /* 导出 crash PC 前后各256字节的二进制，供 arm-none-eabi-objdump 反汇编 */
        {
            uint32_t dump_start = (pc > 256) ? (pc - 256) : 0;
            uint32_t dump_end = pc + 256;
            uint32_t dump_len = dump_end - dump_start;
            uint8_t *dump_buf = malloc(dump_len);
            if (dump_buf && uc_mem_read(uc, dump_start, dump_buf, dump_len) == UC_ERR_OK) {
                FILE *df = fopen("/tmp/vmrp_crash.bin", "wb");
                if (df) {
                    fwrite(dump_buf, 1, dump_len, df);
                    fclose(df);
                    printf("  [CRASH_DUMP] saved %u bytes [0x%X..0x%X] to /tmp/vmrp_crash.bin\n",
                           dump_len, dump_start, dump_end);
                    printf("  [CRASH_DUMP] disassemble: arm-none-eabi-objdump -D -b binary "
                           "-m arm -M force-thumb --adjust-vma=0x%X /tmp/vmrp_crash.bin\n",
                           dump_start);
                }
            }
            free(dump_buf);
        }
        /* 导出栈内容（SP 向上64字节） */
        {
            uint8_t stack_buf[64];
            if (uc_mem_read(uc, sp, stack_buf, sizeof(stack_buf)) == UC_ERR_OK) {
                printf("  stack @0x%X:", sp);
                for (int i = 0; i < 64; i += 4) {
                    uint32_t val = stack_buf[i] | (stack_buf[i+1]<<8)
                                 | (stack_buf[i+2]<<16) | (stack_buf[i+3]<<24);
                    printf(" 0x%08X", val);
                }
                printf("\n");
            }
        }
    }
    dumpREG(uc);
    return false;
}

/* GOT 写监控 */
static void hook_got_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)type;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (size != 4) return;
    uint32_t r9 = reg_read32(uc, UC_ARM_REG_R9);
    uint32_t got_size = EXT_TABLE_COUNT * 4;
    if (!r9 || (uint32_t)address < r9 || (uint32_t)address >= r9 + got_size) return;
    uint32_t idx = ((uint32_t)address - r9) / 4;
    if (idx >= EXT_TABLE_COUNT) return;
    if ((uint32_t)value >= EXT_TABLE_ADDR &&
        (uint32_t)value < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4) {
        /*
         * 观测真实重定位偏移：ARM 代码把 memcpy 桥(table[3]=EXT_TABLE_ADDR+0xC)
         * 写入某个 rw_base 的 GOT 时，记录其 R9 相对偏移到对应嵌套模块记录上。
         * 这是私有 loader 子模块 GOT 桥块真实基址的可靠信号——bridge 修复据此把
         * 描述符平移到模块代码实际读取的偏移，而不是写死的 app 专用偏移。 */
        if ((uint32_t)value == EXT_TABLE_ADDR + 0xCu) {
            for (int gi = 0; gi < m->nested_module_count; ++gi) {
                uint32_t nrw = 0;
                if (arm_ptr(m, m->nested_modules[gi].p_addr))
                    memcpy(&nrw, arm_ptr(m, m->nested_modules[gi].p_addr), 4);
                if (nrw && nrw == r9) {
                    m->nested_modules[gi].got_memcpy_off = (uint32_t)address - r9;
                    break;
                }
            }
        }
        if (app_should_protect_got_addr(m, (uint32_t)address)) {
            return;
        }
        /* bridge 函数指针写入 → 记录快照。只在 got_snapshot_base 尚未
         * 设置或与当前 R9 一致时更新 base，防止嵌套插件（如 netpay）
         * 的 GOT 初始化覆盖 wrapper 的 snapshot base */
        if (!m->got_snapshot_base || m->got_snapshot_base == r9) {
            m->got_snapshot_base = r9;
        }
        /* 只记录属于 snapshot base 所属 GOT 的写入 */
        if (m->got_snapshot_base == r9) {
            m->got_snapshot[idx] = (uint32_t)value;
        }
    }
    /* 非 bridge 值覆盖时保留已有的快照不清除——netpay 等嵌套插件会将
     * GOT 中的 bridge 指针临时替换为自己的回调地址，但 dump0 restore
     * 读回整块内存后需要把这些槽位恢复为原始 bridge 指针 */
}

static void hook_screen_write(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data) {
    (void)uc;
    (void)type;
    (void)address;
    (void)size;
    (void)value;
    ArmExtModule *m = (ArmExtModule *)user_data;
    m->screen_write_count++;
    /*
     * The ARM screen buffer is shared storage, not the visible layer owner.
     * When a foreground child is active, covered primary/wrapper code may
     * still repaint that memory; those writes must not synthesize a host
     * present over the child that already owns the visible screen.
     */
    if (!arm_ext_has_foreground_child(m) &&
        arm_ext_current_screen_owner_is_visible(m)) {
        m->screen_dirty = 1;
        arm_ext_note_screen_damage_addr_range(m, address, size);
    }
    if (arm_ext_trace_on() && m->screen_write_count <= 20) {
        printf("arm_ext_executor: screen write #%u addr=0x%llX size=%d value=0x%llX\n",
               m->screen_write_count, (unsigned long long)address, size, (unsigned long long)value);
    }
}

static void hook_restore_r9(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    if (arm_ext_guard_table_return_block(uc, m, (uint32_t)address)) {
        return;
    }
    if (m->outer_r9 && m->nested_return_addr && (uint32_t)address == (m->nested_return_addr & ~1u)) {
        uc_reg_write(uc, UC_ARM_REG_R9, &m->outer_r9);
        if (arm_ext_trace_on()) {
            printf("arm_ext_executor: restored outer R9=0x%X at 0x%X\n",
                   m->outer_r9, (uint32_t)address);
        }
        m->outer_r9 = 0;
        m->nested_return_addr = 0;
        return;
    }
    /*
     * wrapper dispatch can BLX into callbacks retained by older nested EXTs
     * (gxdzc netpay: 0xE83B40 -> smsend.ext 0x66FFBD). Those callbacks use
     * R9-relative globals, so R9 must follow the code range currently being
     * executed rather than the last active helper. The hook runs per basic
     * block and only writes R9 when PC enters a known EXT image. Verification:
     * gxdzc e2e reaches the payment UI.
     */
    arm_ext_sync_r9_for_code_addr(m, (uint32_t)address);
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
                if (arm_ext_trace_on()) {
                    printf("arm_ext_executor: patched wrapper stack at 0x%X\n", EXT_CODE_ADDR + off);
                }
                break;
            }
        }
    }
}

static void init_table(ArmExtModule *m) {
    arm_ext_init_pack_names(m);
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

    int32_t sw = mr_screen_w > 0 ? mr_screen_w : vmrp_config.screen_width;
    int32_t sh = mr_screen_h > 0 ? mr_screen_h : vmrp_config.screen_height;
    int32_t bit = 16;
    m->screen_w = sw;
    m->screen_h = sh;
    m->screen_len = (uint32_t)(sw * sh * 2);
    /* 屏幕缓冲保持为 bump 堆的首个分配(地址/布局与既有应用完全一致——
     * gghjt game.ext 会扫描 RAM 定位 framebuffer,netpay dump0 恢复也依赖
     * 既有堆地址)。应用改写 ARM 可见 mr_screen_w/h 把逻辑画布改大时,由
     * slot 写监视钩子把缓冲迁移到顶部保留区(见 hook_screen_dim_write)。 */
    m->screen_cap = m->screen_len;
    m->screen_addr = arm_alloc(m, m->screen_len);
    /* B6:堆首分配理论上不会失败(堆空、screen_len≤1MB),但一旦失败,0 会被
     * 当作屏幕地址写进 table[91]/bitmap 描述符;无条件大声报错便于定位 */
    if (!m->screen_addr) {
        printf("arm_ext_executor: FATAL screen buffer alloc failed (len=%u)\n",
               m->screen_len);
    }
    if (m->screen_addr && mr_screenBuf) {
        memcpy(arm_ptr(m, m->screen_addr), mr_screenBuf, m->screen_len);
    }
    m->screen_dirty = 0;

    write_table_entry(m, 91, alloc_u32_slot(m, m->screen_addr));
    m->screen_w_slot = alloc_u32_slot(m, (uint32_t)sw);
    m->screen_h_slot = alloc_u32_slot(m, (uint32_t)sh);
    write_table_entry(m, 92, m->screen_w_slot);
    write_table_entry(m, 93, m->screen_h_slot);
    write_table_entry(m, 94, alloc_u32_slot(m, (uint32_t)bit));
    /* 监视 guest 对 mr_screen_w/h 变量的写入:应用(如 gtcm 的 SphinxJoy
     * 引擎)在 init 时直接改写这两个变量配置逻辑画布,必须在其后续绘制
     * (graphics.ext 会缓存 framebuffer 指针)之前完成缓冲迁移。两个 slot
     * 由连续的 alloc_u32_slot 分配,地址相邻。 */
    if (m->screen_w_slot && m->screen_h_slot) {
        uc_hook dim_hook;
        uc_hook dim_read_hook;
        uint32_t lo = m->screen_w_slot < m->screen_h_slot ? m->screen_w_slot : m->screen_h_slot;
        uint32_t hi = m->screen_w_slot < m->screen_h_slot ? m->screen_h_slot : m->screen_w_slot;
        uc_hook_add(m->uc, &dim_hook, UC_HOOK_MEM_WRITE, hook_screen_dim_write,
                    m, lo, hi + 3);
        /* 读钩子:guest 写入转置尺寸(撤销旋转)后,在读回前恢复宿主尺寸,
         * 见 hook_screen_dim_read 注释 */
        uc_hook_add(m->uc, &dim_read_hook, UC_HOOK_MEM_READ,
                    hook_screen_dim_read, m, lo, hi + 3);
    }

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

    /*
     * table[100] is ARM-visible package identity and code commonly copies it
     * into fixed 32-byte buffers.  arm_ext_init_pack_names() keeps the host path
     * separately and derives the shortest openable alias, so expose the alias
     * from startup and resolve it back to the host path in file bridges.
     */
    arm_ext_set_pack_table_name(m, m->pack_alias[0] ?
                                   m->pack_alias :
                                   mr_get_pack_filename());
    m->start_table_addr = alloc_filename_table_slot(m, mr_get_start_filename());
    m->old_pack_table_addr = alloc_filename_table_slot(
        m, arm_ext_guest_pack_alias_for_handoff(m, mr_get_old_pack_filename()));
    m->old_start_table_addr = alloc_filename_table_slot(m, mr_get_old_start_filename());
    m->start_parameter_table_addr =
        alloc_start_parameter_table_slot(m, mr_get_start_fileparameter());
    write_table_entry(m, 101, m->start_table_addr);
    write_table_entry(m, 102, m->old_pack_table_addr);
    write_table_entry(m, 103, m->old_start_table_addr);
    /* table[138] is start_fileparameter[128] in native Mythroad.  Some launchers
     * store return/continuation state there before publishing RESTART. */
    write_table_entry(m, 138, m->start_parameter_table_addr);

    /* 真机 MR 平台的应用堆一般为 512KB–1MB；origin_mem_len 过大会使
     * 部分游戏的 exRam 预算计算（= 固定值 − origin_mem_len）溢出为负，
     * 导致 SCRRAM 分配被跳过、图像资源无法渲染。默认取与真机一致的
     * 1MB(sky_istore 启动时校验堆容量,512KB 会报"内存不足");需要
     * 更大堆的应用用 --memory/VMRP_MEMORY 调整(1M-16M)。arm_alloc 仍
     * 可提供远超此值的实际内存。
     * 池在 ARM 16MB 空间的 bump 堆(0x200000 起)上分配,还需避开栈区/
     * 代码区与顶部屏幕保留区,过大的档位(16M)放不下,钳制到 8MB。 */
    m->origin_mem_len = vmrp_memory_bytes(vmrp_config.memory_mb);
    {
        uint32_t ext_pool_max = 8u * 1024u * 1024u;
        if (m->origin_mem_len > ext_pool_max) {
            printf("arm_ext_executor: clamp app heap %uMB -> 8MB (ARM address space limit)\n",
                   m->origin_mem_len >> 20);
            m->origin_mem_len = ext_pool_max;
        }
    }
    m->origin_mem_left = m->origin_mem_len;
    m->origin_mem_min = m->origin_mem_len;
    m->origin_mem_top = 0;
    /* 先分配统计 slot 和 free_head，再分配 origin_mem 池。ext 的 ARM
     * 分配器会在 origin_mem 池末尾写入元数据，如果 slot 紧跟池后面就会被覆盖。 */
    uint32_t slot108 = alloc_u32_slot(m, 0);
    uint32_t slot109 = alloc_u32_slot(m, 0);
    uint32_t slot110 = alloc_u32_slot(m, 0);
    uint32_t slot111 = alloc_u32_slot(m, 0);
    uint32_t slot135 = alloc_u32_slot(m, 0);
    uint32_t slot136 = alloc_u32_slot(m, 0);
    uint32_t free_head = arm_alloc(m, 8);
    m->origin_mem_addr = arm_alloc(m, m->origin_mem_len);
    if (m->origin_mem_addr) {
        /* mem.c _mr_mem_init:池首形成唯一空闲节点 {next=len,len=len}
         * (next==len 即"指向池尾"的链表终止哨),头节点 {next=0,len=0}
         * 指向该池首节点。头节点经 table[146] 暴露给 ARM(原生为
         * &LG_mem_free),guest first-fit 分配器(table[0]/[1]/[2] 与
         * table[125] 输出)据此运转。 */
        uint32_t free_next = m->origin_mem_len;
        uint32_t free_len = m->origin_mem_len;
        memcpy(arm_ptr(m, m->origin_mem_addr), &free_next, 4);
        memcpy((uint8_t *)arm_ptr(m, m->origin_mem_addr) + 4, &free_len, 4);
        if (free_head) {
            uint32_t zero = 0;
            memcpy(arm_ptr(m, free_head), &zero, 4);
            memcpy((uint8_t *)arm_ptr(m, free_head) + 4, &zero, 4);
        }
        m->origin_mem_head_addr = free_head;

        if (slot108) { uint32_t v = m->origin_mem_addr; memcpy(arm_ptr(m, slot108), &v, 4); }
        if (slot109) { uint32_t v = m->origin_mem_len; memcpy(arm_ptr(m, slot109), &v, 4); }
        if (slot110) { uint32_t v = m->origin_mem_addr + m->origin_mem_len; memcpy(arm_ptr(m, slot110), &v, 4); }
        if (slot111) { uint32_t v = m->origin_mem_left; memcpy(arm_ptr(m, slot111), &v, 4); }
        if (slot135) { uint32_t v = m->origin_mem_min; memcpy(arm_ptr(m, slot135), &v, 4); }
        if (slot136) { uint32_t v = m->origin_mem_top; memcpy(arm_ptr(m, slot136), &v, 4); }
        /* 保存 slot 地址，以便 note_origin_mem_alloc/free 后同步 ARM 侧 */
        m->origin_mem_left_slot = slot111;
        m->origin_mem_min_slot = slot135;
        m->origin_mem_top_slot = slot136;
        write_table_entry(m, 108, slot108);
        write_table_entry(m, 109, slot109);
        write_table_entry(m, 110, slot110);
        write_table_entry(m, 111, slot111);
        write_table_entry(m, 135, slot135);
        write_table_entry(m, 136, slot136);
        if (free_head) write_table_entry(m, 146, free_head);
    }
}

int arm_ext_load(ArmExtModule **out, const uint8 *code, uint32 len, int32 load_code, int32 *ext_ret) {
    if (!out || !code || len < 12) return MR_FAILED;
    ArmExtModule *m = calloc(1, sizeof(*m));
    if (!m) return MR_FAILED;
    /* 在 EXT_BASE_ADDR 固定地址分配 ARM 内存，使 ARM 虚拟地址等于宿主指针，
     * 与 unicorn 模式下 uc_mem_map_ptr 的行为一致——ext 返回的指针可被 Lua 直接使用 */
#ifdef _MSC_VER
    m->mem = VirtualAlloc((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (m->mem == NULL) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
#else
    m->mem = mmap((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m->mem == MAP_FAILED) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
#endif
    if (!m->mem) goto fail;
    m->platform_mem = calloc(1, EXT_PLATFORM_MEM_SIZE);
    if (!m->platform_mem) goto fail;
    m->platform_io_mem = calloc(1, EXT_PLATFORM_IO_MEM_SIZE);
    if (!m->platform_io_mem) goto fail;
    m->platform_alt_mem = calloc(1, EXT_PLATFORM_ALT_MEM_SIZE);
    if (!m->platform_alt_mem) goto fail;
    m->executor_meta_mem = calloc(1, EXT_EXECUTOR_META_SIZE);
    if (!m->executor_meta_mem) goto fail;
    m->low_table = calloc(1, EXT_LOW_TABLE_SIZE);
    if (!m->low_table) goto fail;
    m->heap_top = EXT_HEAP_ADDR;
    m->code_len = len;
    m->host_code = code;

    arm_ext_init_pack_names(m);
    m->profile = app_compat_select(m->pack_host_path[0] ?
                                   m->pack_host_path :
                                   mr_get_pack_filename());
    if (m->profile && m->profile->init)
        m->app_state = m->profile->init(m);

    uc_err err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &m->uc);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_BASE_ADDR, EXT_MEM_SIZE, UC_PROT_ALL, m->mem);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_PLATFORM_MEM_ADDR,
                         EXT_PLATFORM_MEM_SIZE, UC_PROT_ALL,
                         m->platform_mem);
    if (err != UC_ERR_OK) goto fail;
    /*
     * MTK firmware uses 0x80000000-class addresses for ROM/MMIO/platform
     * state. Some EXT libraries probe these addresses and then keep small
     * halfword tables under that band (for example base+4, +8, ...). Keep the
     * band ARM-visible so those data probes behave like handset memory
     * instead of crashing on an unmapped read.
     */
    err = uc_mem_map_ptr(m->uc, EXT_PLATFORM_IO_MEM_ADDR,
                         EXT_PLATFORM_IO_MEM_SIZE, UC_PROT_ALL,
                         m->platform_io_mem);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_PLATFORM_ALT_MEM_ADDR,
                         EXT_PLATFORM_ALT_MEM_SIZE, UC_PROT_ALL,
                         m->platform_alt_mem);
    if (err != UC_ERR_OK) goto fail;
    err = uc_mem_map_ptr(m->uc, EXT_EXECUTOR_META_ADDR,
                         EXT_EXECUTOR_META_SIZE, UC_PROT_ALL,
                         m->executor_meta_mem);
    if (err != UC_ERR_OK) goto fail;
    init_table(m);
    /* 预解析 MRP，缓存所有条目，避免 ARM ext 做极慢的 MRP 索引扫描 */
    parse_mrp_cache(m, m->pack_host_path[0] ?
                    m->pack_host_path :
                    mr_get_pack_filename());
    memcpy(m->low_table, m->mem, EXT_TABLE_COUNT * 4);
    err = uc_mem_map_ptr(m->uc, 0, EXT_LOW_TABLE_SIZE, UC_PROT_ALL, m->low_table);
    if (err != UC_ERR_OK) goto fail;
    void *code_dst = arm_ptr(m, EXT_CODE_ADDR);
    if (!code_dst) goto fail;
    memcpy(code_dst, code, len);
    m->wrapper_timer_dispatch_addr = find_wrapper_timer_dispatch(
        code, len, &m->chain_walker_thunk_addr,
        &m->wrapper_compact_timer_scheduler_off);
    m->wrapper_compact_free_return_addr =
        find_wrapper_compact_heap_free_return(
            code, len, &m->wrapper_compact_heap_ctrl_off);
    /* P4.1:wrapper 二进制模式探测结果摘要,默认可见(每次 arm_ext_load
     * 一行)。此前失配时静默降级——compact 堆 sanitize/timer 直连路由整体
     * 不生效且无任何输出,新 SDK 变体表现为"莫名其妙的花屏/卡死";现在
     * 一行日志即可判断哪个探测没命中(值为 0 即未命中)。 */
    printf("arm_ext_executor: wrapper probes len=%u timer_dispatch=0x%X compact_sched_off=0x%X compact_heap_ctrl=0x%X compact_free_ret=0x%X chain_walker=0x%X\n",
           len,
           m->wrapper_timer_dispatch_addr,
           m->wrapper_compact_timer_scheduler_off,
           m->wrapper_compact_heap_ctrl_off,
           m->wrapper_compact_free_return_addr,
           m->chain_walker_thunk_addr);
    patch_wrapper_stack_size(m);
    uint32_t table = EXT_TABLE_ADDR;
    memcpy(arm_ptr(m, EXT_CODE_ADDR), &table, 4);
    err = uc_hook_add(m->uc, &m->hook, UC_HOOK_CODE, hook_table, m, EXT_TABLE_ADDR, EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4);
    if (err != UC_ERR_OK) goto fail;
    if (m->wrapper_compact_free_return_addr &&
        m->wrapper_compact_heap_ctrl_off) {
        uc_hook compact_free_hook;
        err = uc_hook_add(m->uc, &compact_free_hook, UC_HOOK_CODE,
                          hook_compact_heap_free_return, m,
                          m->wrapper_compact_free_return_addr,
                          m->wrapper_compact_free_return_addr);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook low_zero_hook;
    err = uc_hook_add(m->uc, &low_zero_hook, UC_HOOK_CODE, hook_low_zero, m, 0, EXT_LOW_TABLE_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    if (arm_ext_trace_pc_on()) {
        uc_hook pc_hook;
        err = uc_hook_add(m->uc, &pc_hook, UC_HOOK_CODE, trace_pc, m,
                          EXT_CODE_ADDR, EXT_CODE_ADDR + len);
        if (err != UC_ERR_OK) goto fail;
    }
    uc_hook restore_hook;
    err = uc_hook_add(m->uc, &restore_hook, UC_HOOK_BLOCK, hook_restore_r9, m,
                      EXT_BASE_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook restore_platform_hook;
    err = uc_hook_add(m->uc, &restore_platform_hook, UC_HOOK_BLOCK,
                      hook_restore_r9, m,
                      EXT_PLATFORM_MEM_ADDR,
                      EXT_PLATFORM_MEM_ADDR + EXT_PLATFORM_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook restore_platform_io_hook;
    err = uc_hook_add(m->uc, &restore_platform_io_hook, UC_HOOK_BLOCK,
                      hook_restore_r9, m,
                      EXT_PLATFORM_IO_MEM_ADDR,
                      EXT_PLATFORM_IO_MEM_ADDR + EXT_PLATFORM_IO_MEM_SIZE - 1);
    if (err != UC_ERR_OK) goto fail;
    uc_hook restore_platform_alt_hook;
    err = uc_hook_add(m->uc, &restore_platform_alt_hook, UC_HOOK_BLOCK,
                      hook_restore_r9, m,
                      EXT_PLATFORM_ALT_MEM_ADDR,
                      EXT_PLATFORM_ALT_MEM_ADDR + EXT_PLATFORM_ALT_MEM_SIZE - 1);
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
    uc_hook intr_hook;
    err = uc_hook_add(m->uc, &intr_hook, UC_HOOK_INTR, hook_intr, m, 1, 0);
    if (err != UC_ERR_OK) goto fail;
    /* GOT bridge 指针追踪：ARM 代码向 R9 数据区写入 bridge 地址时记录，
     * 后续 dump/restore 覆盖时自动修复 */
    {
        uc_hook got_hook;
        uc_hook_add(m->uc, &got_hook, UC_HOOK_MEM_WRITE, hook_got_write, m,
                    EXT_HEAP_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    }

    reg_write32(m->uc, UC_ARM_REG_R0, (uint32_t)load_code);
    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    int load_ret = run_arm(m, EXT_CODE_ADDR + 8);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    if (load_ret != MR_SUCCESS) goto fail;
    capture_timer_dispatches(m);
    /* 将 ARM 代码 mr_c_function_load() 的返回值 (R0) 传给调用者，
     * 与非 native 路径中 mr_load_c_function(code) 的返回值语义一致 */
    if (ext_ret) *ext_ret = (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
    *out = m;
    return MR_SUCCESS;
fail:
    arm_ext_unload(m);
    return MR_FAILED;
}


typedef void (*ArmExtFilenameSetter)(const char *name);

static void arm_ext_sync_filename_slot(ArmExtModule *m, uint32_t slot,
                                       ArmExtFilenameSetter setter) {
    char value[ARM_EXT_PACK_TABLE_SIZE];
    const void *src = slot ? arm_ptr(m, slot) : NULL;
    if (!src || !setter) return;
    memcpy(value, src, sizeof(value));
    value[sizeof(value) - 1] = '\0';
    setter(value);
}

static void arm_ext_sync_pack_filename_slot(ArmExtModule *m, uint32_t slot,
                                            ArmExtFilenameSetter setter) {
    char value[ARM_EXT_PACK_TABLE_SIZE];
    const char *host_value;
    const void *src = slot ? arm_ptr(m, slot) : NULL;
    if (!src || !setter) return;
    memcpy(value, src, sizeof(value));
    value[sizeof(value) - 1] = '\0';
    /* ARM-visible package names are often shortened to fit legacy fixed
     * buffers.  Native restart needs the host-openable spelling, so resolve the
     * token through the same alias map used by file bridges. */
    host_value = arm_ext_pack_to_host_path(m, value);
    setter(host_value ? host_value : value);
}

static void arm_ext_sync_start_parameter_slot(ArmExtModule *m, uint32_t slot) {
    const void *src = slot ? arm_ptr(m, slot) : NULL;
    char value[ARM_EXT_PACK_TABLE_SIZE];
    if (!src) return;
    /* table[138] 在真机上就是宿主的 start_fileparameter[128]。launcher 在这
     * 128 字节里保存二进制续传记录（如 Cookie 的 "_RL\0"+尾部大端视图字段，
     * 末字节 +0x7F 也是有效数据），因此不能按 C 字符串截断，也不能补 NUL。 */
    memcpy(value, src, sizeof(value));
    mr_set_start_fileparameter(value);
}

static const char *arm_ext_diag_filename_slot(ArmExtModule *m, uint32_t slot,
                                              char *out, size_t out_len) {
    const void *src = slot ? arm_ptr(m, slot) : NULL;
    if (!out || !out_len) return "";
    out[0] = '\0';
    if (!src) return out;
    memcpy(out, src, out_len - 1);
    out[out_len - 1] = '\0';
    return out;
}

static int arm_ext_lifecycle_diag(void) {
    static int cached = -1;
    if (cached < 0) cached = getenv("VMRP_LIFECYCLE_DIAG") ? 1 : 0;
    return cached;
}

static void arm_ext_sync_handoff_filenames(ArmExtModule *m) {
    /* Native EXT code writes these host arrays directly.  Copying them only at
     * a published lifecycle transition recreates that shared-memory ABI without
     * treating ordinary transient package aliases as a handoff. */
    arm_ext_sync_pack_filename_slot(m, m->pack_table_addr, mr_set_pack_filename);
    arm_ext_sync_filename_slot(m, m->start_table_addr, mr_set_start_filename);
    arm_ext_sync_pack_filename_slot(m, m->old_pack_table_addr, mr_set_old_pack_filename);
    arm_ext_sync_filename_slot(m, m->old_start_table_addr, mr_set_old_start_filename);
    arm_ext_sync_start_parameter_slot(m, m->start_parameter_table_addr);
}

static int arm_ext_finish_callback_state(ArmExtModule *m, uint32_t ext_chunk) {
    if (!m) return 0;

    /* ARM ext 的退出/重启逻辑会直接写 mr_state_slot 设置
     * MR_STATE_RESTART(3) 或 MR_STATE_STOP(4)。真机上 ARM 代码与宿主共享
     * 同一全局变量；Unicorn 下 ARM 内存独立，所以回调结束后要同步一次。 */
    if (m->mr_state_slot) {
        enum {
            ARM_MR_STATE_RUN = 1,
            ARM_MR_STATE_RESTART = 3,
            ARM_MR_STATE_STOP = 4,
            ARM_MR_TIMER_RUNNING = 1
        };
        uint32_t arm_state = 0;
        memcpy(&arm_state, arm_ptr(m, m->mr_state_slot), 4);
        if (arm_state == ARM_MR_STATE_RESTART) {
            if (arm_ext_lifecycle_diag()) {
                char pack[ARM_EXT_PACK_TABLE_SIZE];
                char start[ARM_EXT_PACK_TABLE_SIZE];
                char old_pack[ARM_EXT_PACK_TABLE_SIZE];
                char old_start[ARM_EXT_PACK_TABLE_SIZE];
                char param[ARM_EXT_PACK_TABLE_SIZE];
                printf("LIFE arm state=RESTART pack='%s' start='%s' old_pack='%s' old_start='%s' param='%s'\n",
                       arm_ext_diag_filename_slot(m, m->pack_table_addr, pack, sizeof(pack)),
                       arm_ext_diag_filename_slot(m, m->start_table_addr, start, sizeof(start)),
                       arm_ext_diag_filename_slot(m, m->old_pack_table_addr, old_pack, sizeof(old_pack)),
                       arm_ext_diag_filename_slot(m, m->old_start_table_addr, old_start, sizeof(old_start)),
                       arm_ext_diag_filename_slot(m, m->start_parameter_table_addr, param, sizeof(param)));
            }
            arm_ext_sync_handoff_filenames(m);
            /* The guest has already armed the restart timer.  Publish RESTART
             * to native Mythroad so mr_timer() performs its normal stop/start. */
            mr_state = ARM_MR_STATE_RESTART;
            return 1;
        }
        if (arm_state == ARM_MR_STATE_STOP) {
            if (arm_ext_lifecycle_diag()) {
                char pack[ARM_EXT_PACK_TABLE_SIZE];
                char start[ARM_EXT_PACK_TABLE_SIZE];
                char old_pack[ARM_EXT_PACK_TABLE_SIZE];
                char old_start[ARM_EXT_PACK_TABLE_SIZE];
                char param[ARM_EXT_PACK_TABLE_SIZE];
                printf("LIFE arm state=STOP pack='%s' start='%s' old_pack='%s' old_start='%s' param='%s'\n",
                       arm_ext_diag_filename_slot(m, m->pack_table_addr, pack, sizeof(pack)),
                       arm_ext_diag_filename_slot(m, m->start_table_addr, start, sizeof(start)),
                       arm_ext_diag_filename_slot(m, m->old_pack_table_addr, old_pack, sizeof(old_pack)),
                       arm_ext_diag_filename_slot(m, m->old_start_table_addr, old_start, sizeof(old_start)),
                       arm_ext_diag_filename_slot(m, m->start_parameter_table_addr, param, sizeof(param)));
            }
            arm_ext_sync_handoff_filenames(m);
            mr_state = ARM_MR_STATE_STOP;
            /* STOP follows the platform exit contract; mr_exit() returns to a
             * registered old app when present, otherwise it terminates VMRP. */
            mr_exit();
            return 1;
        }
    }

    if (!ext_chunk || !arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF)) {
        return 0;
    }

    /* wrapper 反汇编确认 extChunk[0x34] 是 suspend 深度：
     *   0xE831A4 suspend: [0x34]++，首次进入时摘掉 wrapper timer 节点
     *   0xE83220 resume:  [0x34]--，回到 0 时重新挂回 timer 节点
     * 普通 wrapper 模态/子流程会把 active_helper_addr 切到后加载的子模块；
     * gxdzc 的资源下载提示则只摘掉 game timer，并把旧链表头保存在
     * saved_game_timer_head，取消时还要恢复。真正的“返回上层/停止自驱”
     * 路径没有前台子模块、没有可恢复的 game timer，且宿主定时器空闲。
     * 模拟器没有真机平台的上层调度器，因此只在这些条件都满足时调用
     * mr_exit()，把 wrapper 状态同步到宿主平台层。 */
    enum { MR_TIMER_STATE_IDLE = 0 };
    uint32_t suspend_depth = 0;
    memcpy(&suspend_depth, arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
    int foreground_child =
        m->primary_helper_addr &&
        m->active_helper_addr &&
        m->active_helper_addr != m->primary_helper_addr;
    if (suspend_depth > 0 &&
        mr_timer_state == MR_TIMER_STATE_IDLE &&
        !foreground_child &&
        m->saved_game_timer_head == 0) {
        mr_exit();
        return 1;
    }
    return 0;
}

static int arm_ext_save_modal_screen_snapshot(ArmExtModule *m) {
    if (!m || !m->screen_addr || !m->screen_len ||
        !arm_ptr(m, m->screen_addr)) {
        return 0;
    }
    if (m->modal_screen_snapshot_len != m->screen_len) {
        free(m->modal_screen_snapshot);
        m->modal_screen_snapshot = NULL;
        m->modal_screen_snapshot_len = 0;
        m->modal_screen_snapshot_valid = 0;
    }
    if (!m->modal_screen_snapshot) {
        m->modal_screen_snapshot = (uint8_t *)malloc(m->screen_len);
        if (m->modal_screen_snapshot) m->modal_screen_snapshot_len = m->screen_len;
    }
    if (!m->modal_screen_snapshot) return 0;
    memcpy(m->modal_screen_snapshot, arm_ptr(m, m->screen_addr), m->screen_len);
    m->modal_screen_snapshot_valid = 0;
    return 1;
}

/* wrapper RW 前台分发区快照范围：覆盖 wrapper 在 RW 内维护的前台模块分发表
 * 与前台 helper 槽。偏移与长度来自对 cfunction.ext 事件分发路径的反汇编与
 * 运行时 RW 对比（dota 浏览器插件下载：进入子模块时 wrapper 改写 +0xF4 分发
 * 表项及 +0x18C/+0x1A0 helper 槽，关闭后未还原导致事件仍走子模块）。 */
#define MODAL_FG_SNAPSHOT_OFF 0xE0u
#define MODAL_FG_SNAPSHOT_LEN 0xD0u

static int arm_ext_save_modal_fg_snapshot(ArmExtModule *m) {
    if (!m || !m->p_addr || !arm_ptr(m, m->p_addr)) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->p_addr), 4);
    if (!rw || !arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF + MODAL_FG_SNAPSHOT_LEN - 1)) {
        return 0;
    }
    if (!m->modal_fg_snapshot) {
        m->modal_fg_snapshot = (uint8_t *)malloc(MODAL_FG_SNAPSHOT_LEN);
        if (!m->modal_fg_snapshot) return 0;
    }
    memcpy(m->modal_fg_snapshot, arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF),
           MODAL_FG_SNAPSHOT_LEN);
    m->modal_fg_snapshot_rw = rw;
    m->modal_fg_snapshot_valid = 0;
    return 1;
}

/* 模态子模块关闭时还原 wrapper 前台分发区，使事件路由回到下层页面。
 * 仅当 wrapper RW 基址未变（同一 wrapper 实例）时还原，避免误写。 */
static void arm_ext_restore_modal_fg_snapshot(ArmExtModule *m) {
    if (!m || !m->modal_fg_snapshot || !m->modal_fg_snapshot_valid) return;
    uint32_t rw = 0;
    if (m->p_addr && arm_ptr(m, m->p_addr))
        memcpy(&rw, arm_ptr(m, m->p_addr), 4);
    if (rw && rw == m->modal_fg_snapshot_rw &&
        arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF + MODAL_FG_SNAPSHOT_LEN - 1)) {
        memcpy(arm_ptr(m, rw + MODAL_FG_SNAPSHOT_OFF), m->modal_fg_snapshot,
               MODAL_FG_SNAPSHOT_LEN);
    }
    m->modal_fg_snapshot_valid = 0;
}

/* code=2 之后宿主 timer 已停但仍有活跃队列(四处判活条件各异)时,统一
 * 保持 50ms 宿主 tick,动作与 table[31] mr_timerStart 语义一致。
 * Phase 3 去重:此前四段逐字重复(issues doc M1)。 */
static void arm_ext_keep_host_tick_alive(ArmExtModule *m) {
    mr_timerStart(50);
    mr_timer_state = 1;
    m->host_timer_pending = 1;
    internal_slot_write(m, m->mr_timer_state_slot, 1);
}

static void arm_ext_dispatch_pending_sms_result(ArmExtModule *m) {
    if (!m || !m->pending_sms_result || m->dispatching_sms_result) return;
    int32_t result = m->pending_sms_result_value;
    m->pending_sms_result = 0;
    m->dispatching_sms_result = 1;
    int32_t event[5] = {MR_SMS_RESULT, result, 0, 0, 0};
    uint8 *output = NULL;
    int32 output_len = 0;
    (void)arm_ext_call(m, 1, event, sizeof(event), &output, &output_len);
    m->dispatching_sms_result = 0;
}

int arm_ext_call(ArmExtModule *m, int32 code, const void *input, uint32 input_len,
                 uint8 **output, int32 *output_len) {
    if (output) *output = NULL;
    if (output_len) *output_len = 0;
    if (!m || !m->helper_addr || !m->p_addr) return MR_FAILED;
    /* 不变量入口检查(P0.3):事件/定时器高频进入,入口检查等价于上一次
     * 调用的出口检查(只差一拍),避免给 550 行函数的每个 return 加桩 */
    arm_ext_verify_invariants(m, "call-entry");
    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
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
    /* B6:与上方 input_addr 一致地检查分配失败,否则 helper 会向地址 0 写输出 */
    if (!outp_addr || !outl_addr) return MR_FAILED;
    /* 真机上事件(code=1)始终先经 wrapper 的 mrc_extHelper（反汇编
     * docs/反汇编研究.c:428-476），wrapper 通过 mrc_event 的内部回调链
     * 分发给 game。code=0/3-5 为生命周期调用，路由到 primary game helper。
     * code=2 路由到当前定时器所有者（wrapper 或 game）。 */
    int has_separate_wrapper = m->helper_addr && m->primary_helper_addr &&
                               m->helper_addr != m->primary_helper_addr;
    uint32_t call_p_addr, call_helper_addr;
    if (code == 1 && has_separate_wrapper) {
        call_p_addr = m->p_addr;
        call_helper_addr = m->helper_addr;
    } else if (code >= 0 && code <= 5 && m->primary_p_addr) {
        call_p_addr = m->primary_p_addr;
        call_helper_addr = m->primary_helper_addr;
    } else {
        call_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
        call_helper_addr = m->active_helper_addr ? m->active_helper_addr : m->helper_addr;
    }
    if (code == 2 && m->timer_p_addr && m->timer_helper_addr) {
        call_p_addr = m->timer_p_addr;
        call_helper_addr = m->timer_helper_addr;
    }
    int wrapper_raw_event_routed = 0;
    int reopen_set_this_call = 0;
    uint32_t modal_ext_chunk = 0;
    uint32_t modal_suspend_depth_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + AEX_P_EXT_CHUNK_OFF))
        memcpy(&modal_ext_chunk, arm_ptr(m, m->primary_p_addr + AEX_P_EXT_CHUNK_OFF), 4);
    if (modal_ext_chunk && arm_ptr(m, modal_ext_chunk + AEX_CHUNK_SUSPEND_OFF))
        memcpy(&modal_suspend_depth_pre, arm_ptr(m, modal_ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
    int suspended_foreground_child =
        arm_ext_has_suspended_foreground_child(m, modal_ext_chunk);
    int foreground_child_has_private_timer =
        arm_ext_has_foreground_child(m) &&
        (arm_ext_foreground_child_has_frame_timer_queue(m) ||
         arm_ext_foreground_child_has_compact_timer_queue(m));
    int foreground_child_owns_timer =
        m->active_p_addr && m->active_helper_addr &&
        m->timer_p_addr == m->active_p_addr &&
        m->timer_helper_addr == m->active_helper_addr;
    if (code == 2 && foreground_child_has_private_timer &&
        (foreground_child_owns_timer ||
         !m->timer_p_addr || !m->timer_helper_addr)) {
        /*
         * A painted foreground child is not enough timer-liveness evidence:
         * optwar's compact downloader walker returns immediately when
         * R9+0x250 is null, while the wrapper still owns the host timer.
         * Likewise advbar can keep a private cleanup node while wrapper-owned
         * modal/download transitions are pending.  Route direct child ticks
         * only when the child owns the timer, or no owner has been recorded
         * yet.
         */
        call_p_addr = m->active_p_addr;
        call_helper_addr = m->active_helper_addr;
    }
    int foreground_child_active =
        m->active_p_addr &&
        m->active_helper_addr &&
        m->active_p_addr != m->primary_p_addr &&
        m->active_helper_addr != m->primary_helper_addr &&
        m->active_p_addr != m->p_addr &&
        m->active_helper_addr != m->helper_addr;
    /* 真机上 code=1 事件始终先经 wrapper 的 mrc_extHelper（反汇编
     * docs/反汇编研究.c:428-476），wrapper 通过内部回调链分发给 game
     * 或前台子模块。不绕过 wrapper，否则 wrapper 的 suspend/resume
     * 机制无法正确管理模态框生命周期。
     *
     * 预处理：
     * - 修复 extChunk[8]（game helper 指针），确保 wrapper 的 resume
     *   (0xE83220) 能通过 blx extChunk[8] 正确调用 game 的 resumeApp。
     *   cfunction.ext ARM 代码运行中会将该槽位清零（gxdzc 反汇编证实）。 */
    if (code == 1 && has_separate_wrapper) {
        uint32_t _ec = modal_ext_chunk;
        if (_ec && m->primary_helper_addr && arm_ptr(m, _ec + 8)) {
            memcpy(arm_ptr(m, _ec + 8), &m->primary_helper_addr, 4);
        }
        /*
         * Host-originated code=1 packets carry five int32 fields.  The wrapper
         * sees those raw events before Lua dealevent; return success for them
         * so a key press is not replayed by Lua's later 12-byte _strCom(801)
         * forwarding path.  Keep 12-byte internal forwards on the helper's real
         * return value, which lets non-modal overlays decline events after
         * their visible state has already been cleared.
         */
        if (input_len >= 20 && input_addr) {
            wrapper_raw_event_routed = 1;
        }
    }

    /* chain walker dispatch 的 supplementary_init_done 计数在
     * arm_ext_call_dispatch 内部管理，这里不需要额外门控。 */

    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, call_p_addr), 4);
    if (arm_ext_diag_on()) {
        uint32_t ev[5] = {0, 0, 0, 0, 0};
        if (code == 1 && input_addr && input_len >= sizeof(ev) &&
            arm_ptr(m, input_addr + sizeof(ev) - 1)) {
            memcpy(ev, arm_ptr(m, input_addr), sizeof(ev));
        }
        printf("DIAG arm_ext_call code=%d input_len=%u callP=0x%X callH=0x%X rw=0x%X activeP=0x%X activeH=0x%X primaryP=0x%X primaryH=0x%X timerP=0x%X timerH=0x%X modalDepth=%u ev=%u,%u,%u,%u,%u\n",
               (int)code, input_len, call_p_addr, call_helper_addr, rw_base,
               m->active_p_addr, m->active_helper_addr, m->primary_p_addr,
               m->primary_helper_addr, m->timer_p_addr, m->timer_helper_addr,
               modal_suspend_depth_pre, ev[0], ev[1], ev[2], ev[3], ev[4]);
        if (code == 2 && suspended_foreground_child) {
            /*
             * Browser private children keep their timer queue under child
             * R9+0x94 (frame.ext 0x2C96A0), not the primary game timer head.
             * Dump that window around each routed tick so stalled foreground
             * progress can be tied to the child ABI instead of broad tracing.
             */
            arm_ext_diag_dump_wrapper_timer_state(m, "call-pre");
            arm_ext_diag_dump_rw_timer_state(m, "call-pre-active", rw_base);
        }
    }
    arm_ext_diag_dump_layer_state(m, "call-pre");
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
    reg_write32(m->uc, UC_ARM_REG_R0, call_p_addr);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)code);
    reg_write32(m->uc, UC_ARM_REG_R2, input_addr);
    reg_write32(m->uc, UC_ARM_REG_R3, input_len);
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 16;
    uint32_t helper_args[4] = {outp_addr, outl_addr, 0, 0};
    uc_mem_write(m->uc, sp, helper_args, sizeof(helper_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    /* 保存 arm_ext_call 前的 game state[8] */
    uint32_t _ac_grw = 0, _ac_s8_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&_ac_grw, arm_ptr(m, m->primary_p_addr), 4);
    _ac_s8_pre = read_game_timer_head(m, _ac_grw);
    int call_modal_snapshot_candidate = 0;
    /* 非模态状态下的 code=1 事件可能触发子模块加载，需要事先保存 game
     * 的快照以便子模块关闭后恢复。事件经 wrapper 路由时 call_p_addr
     * 指向 wrapper 而非 primary，但语义不变：game 仍然会收到该事件。 */
    /* gxdzc 的下载提示可能在普通事件回调或 primary timer(code=2) 中
     * 直接触发 wrapper suspend，而不是等到 wrapper dispatch；回调前
     * 保存，回调后确认 [0x34] 0->1 才允许 cancel 使用这张底层页面截图。 */
    if ((code == 1 || code == 2) && modal_suspend_depth_pre == 0) {
        if (_ac_s8_pre != 0)
            call_modal_snapshot_candidate = arm_ext_save_modal_screen_snapshot(m);
        /* 同步快照 wrapper 前台分发区，供模态子模块关闭后还原事件路由。 */
        arm_ext_save_modal_fg_snapshot(m);
    }

    /* extChunk[8]（game helper 指针）在 wrapper ARM 代码执行期间会被清零，
     * code=2 定时器回调中 wrapper 通过 blx extChunk[8] 转发给 game helper，
     * 若该槽已被先前回调清零，后续定时器将无法到达 game，导致 game 的图像
     * 加载初始化仅执行一次。对所有经 wrapper 转发的事件统一修复。 */
    if (has_separate_wrapper && modal_ext_chunk &&
        m->primary_helper_addr && arm_ptr(m, modal_ext_chunk + 8)) {
        uint32_t cur_helper = 0;
        memcpy(&cur_helper, arm_ptr(m, modal_ext_chunk + 8), 4);
        if (!cur_helper) {
            memcpy(arm_ptr(m, modal_ext_chunk + 8), &m->primary_helper_addr, 4);
        }
    }

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    if (code == 2) {
        arm_ext_sanitize_compact_timer_heaps(m);
    }
    int was_host_timer_pending = m->host_timer_pending;
    if (code == 2) {
        m->host_timer_pending = 0;
    }
    /* wrapper 的 code=2 回调中可能通过 table[25]（mr_load_c_function）加载
     * 新的前台子模块（如下载完成后启动 promote.mrp），此时 active_p_addr
     * 会在 run_arm_with_sp 内部被更新。记录回调前的值，回调后比对以检测
     * 前台子模块替换。 */
    uint32_t active_p_addr_pre = m->active_p_addr;
    m->current_p_addr = call_p_addr;
    m->current_helper_addr = call_helper_addr;
    int prev_in_dispatch = m->in_dispatch;
    /*
     * Runtime event/timer callbacks may query the ARM EXT device band while
     * building child graphics dispatch tables.  Lifecycle calls such as code=0
     * still run during startVmrp(), before the E2E control socket exists, and
     * must keep native startup behavior.
     */
    if (code == 1 || code == 2) m->in_dispatch = 1;
    int call_ret = run_arm_with_sp(m, call_helper_addr, sp);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    if (code == 2 && was_host_timer_pending && m->host_timer_pending) {
        internal_slot_write(m, m->mr_timer_state_slot, (uint32_t)mr_timer_state);
    }
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    capture_timer_dispatches(m);
    if (code == 2) {
        arm_ext_sanitize_compact_timer_heaps(m);
    }
    arm_ext_diag_dump_layer_state(m, "call-post");
    if (arm_ext_diag_on() && code == 2 &&
        suspended_foreground_child) {
        uint32_t active_rw_post = 0;
        if (m->active_p_addr && arm_ptr(m, m->active_p_addr)) {
            memcpy(&active_rw_post, arm_ptr(m, m->active_p_addr), 4);
        }
        arm_ext_diag_dump_wrapper_timer_state(m, "call-post");
        arm_ext_diag_dump_rw_timer_state(m, "call-post-active",
                                         active_rw_post);
    }

    /* wrapper 无 primary module 时（如"请稍后"加载阶段），wrapper 的
     * code=2 处理完后不会通过 table[31] 重启宿主 timer。宿主需要持续
     * 给 wrapper 发送 timer tick，直到 wrapper 完成 game 模块的加载并
     * 注册 primary module。 */
    if (code == 2 && !m->host_timer_pending &&
        !m->primary_p_addr && m->wrapper_timer_dispatch_addr) {
        arm_ext_keep_host_tick_alive(m);
    }
    /* arm_ext_call 后检查 game timer head (state[8]) 变化：wrapper 的
     * suspend 会清零 game_rw[0x8C]，保存旧值以便 resume 时恢复。 */
    int primary_game_timer_live_post =
        arm_ext_primary_has_compact_timer_queue(m);
    if (_ac_grw && _ac_s8_pre) {
        uint32_t _ac_s8_post = read_game_timer_head(m, _ac_grw);
        primary_game_timer_live_post |= _ac_s8_post != 0;
        if (arm_ext_diag_on()) {
            printf("DIAG game_timer_head grw=0x%X pre=0x%X post=0x%X code=%d\n",
                   _ac_grw, _ac_s8_pre, _ac_s8_post, code);
        }
        if (_ac_s8_post == 0) {
            m->saved_game_timer_head = _ac_s8_pre;
        }
    } else if (_ac_grw && !_ac_s8_pre && arm_ext_diag_on()) {
        printf("DIAG game_timer_head_zero grw=0x%X code=%d\n", _ac_grw, code);
    }
    int wrapper_timer_live_post = arm_ext_wrapper_has_timer_queue(m);
    int foreground_child_live_post = arm_ext_has_foreground_child(m);
    int foreground_child_timer_live_post =
        foreground_child_live_post &&
        (arm_ext_foreground_child_has_frame_timer_queue(m) ||
         arm_ext_foreground_child_has_compact_timer_queue(m));
    int wrapper_dispatch_busy_post = arm_ext_wrapper_dispatch_is_busy(m);
    if (code == 2 && m->wrapper_compact_timer_scheduler_off &&
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr &&
        !wrapper_timer_live_post && primary_game_timer_live_post &&
        !foreground_child_timer_live_post) {
        /* The compact wrapper bridge can drain while the primary module still
         * has a structurally valid scheduler node.  Release the empty wrapper
         * owner so one host tick reaches the primary walker and can rearm it. */
        m->timer_p_addr = m->primary_p_addr;
        m->timer_helper_addr = m->primary_helper_addr;
        if (!m->host_timer_pending) {
            arm_ext_keep_host_tick_alive(m);
        }
        if (arm_ext_diag_on()) {
            printf("DIAG timer_owner_clear emptyWrapper primaryCompact=1\n");
        }
    }
    if (code == 2 && m->wrapper_compact_timer_scheduler_off &&
        call_p_addr == m->primary_p_addr && wrapper_timer_live_post &&
        m->timer_p_addr == m->primary_p_addr &&
        m->timer_helper_addr == m->primary_helper_addr) {
        /* A primary compact callback rearms its outer extChunk node through the
         * wrapper timerStart veneer.  Once that real wrapper queue reappears,
         * its dispatcher owns the next tick again. */
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
        if (arm_ext_diag_on()) {
            printf("DIAG timer_owner_transfer primaryCompact->wrapper\n");
        }
    }
    if (arm_ext_timer_liveness_diag_on() && code == 2) {
        printf("DIAG timer_liveness suspended=%d fgChildPre=%d fgChildPost=%d childTimerPre=%d childTimerPost=%d wrapperBusy=%d wrapperTimer=%d primaryTimer=%d openSockets=%d hostTimer=%d mrTimer=%d activeP=0x%X activeH=0x%X timerP=0x%X timerH=0x%X callP=0x%X callH=0x%X\n",
               suspended_foreground_child, foreground_child_active,
               foreground_child_live_post, foreground_child_has_private_timer,
               foreground_child_timer_live_post, wrapper_dispatch_busy_post,
               wrapper_timer_live_post,
               primary_game_timer_live_post, my_openSocketCount(),
               m->host_timer_pending, mr_timer_state,
               m->active_p_addr, m->active_helper_addr,
               m->timer_p_addr, m->timer_helper_addr,
               call_p_addr, call_helper_addr);
        arm_ext_diag_dump_wrapper_compact_timer_nodes(m, "liveness");
    }
    if (code == 2 &&
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr &&
        suspended_foreground_child &&
        !foreground_child_has_private_timer &&
        !wrapper_timer_live_post &&
        !wrapper_dispatch_busy_post &&
        primary_game_timer_live_post) {
        /*
         * A wrapper-owned due-list tick can legitimately invoke a foreground
         * child and drain the wrapper queue without installing a new wrapper
         * timer.  At that point keeping timer_p_addr pinned to the wrapper
         * spins empty wrapper code=2 calls and starves the still-live primary
         * game timer head.  Drop the stale owner only when all wrapper/child
         * private liveness checks are empty so the next host tick routes
         * through the normal primary helper path.
         */
        m->timer_p_addr = 0;
        m->timer_helper_addr = 0;
        if (arm_ext_diag_on()) {
            printf("DIAG timer_owner_clear staleWrapper primaryTimer=0x%X activeP=0x%X activeH=0x%X\n",
                   read_game_timer_head(m, _ac_grw),
                   m->active_p_addr, m->active_helper_addr);
        }
    }
    /* timer_p_addr 指向一个已失活的子模块（无私有定时器队列），wrapper
     * 仍有待处理的定时器节点。子模块在下载期间通过 wrapper veneer 调用
     * table[31] 获得 timer 所有权，下载完成后清空了自己的队列但
     * timer_p_addr 未被重置。宿主 tick 被路由到空的子模块 helper，
     * wrapper 的定时器队列永远得不到消费。将 timer 所有权还给 wrapper，
     * 让后续 tick 到达 wrapper 的 code=2 处理，推进定时器队列。 */
    if (code == 2 &&
        m->timer_p_addr &&
        m->timer_p_addr != m->p_addr &&
        m->timer_p_addr != m->primary_p_addr &&
        suspended_foreground_child &&
        !foreground_child_has_private_timer &&
        wrapper_timer_live_post) {
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
        if (arm_ext_diag_on()) {
            printf("DIAG timer_owner_transfer staleChild->wrapper wrapperTimer=%d activeP=0x%X activeH=0x%X\n",
                   wrapper_timer_live_post,
                   m->active_p_addr, m->active_helper_addr);
        }
    }
    if (code == 2 && !m->host_timer_pending &&
        suspended_foreground_child &&
        (foreground_child_timer_live_post ||
         wrapper_dispatch_busy_post ||
         wrapper_timer_live_post ||
         primary_game_timer_live_post)) {
        /*
         * Wrapper suspend/resume keeps the primary extChunk depth at +0x34
         * while a child module owns the foreground.  Direct child ticks are
         * useful only after the child's disassembled scheduler queue contains
         * runnable nodes; before that, a still-linked wrapper/game timer queue
         * is the generic liveness signal for wrapper/game-side progress.
         * Keep the host tick alive without faking wrapper flags or calling
         * code=5 directly.
         */
        arm_ext_keep_host_tick_alive(m);
    }
    if (code == 2 && !m->host_timer_pending &&
        foreground_child_live_post &&
        !foreground_child_timer_live_post &&
        (wrapper_timer_live_post || wrapper_dispatch_busy_post)) {
        /*
         * Private-loader foreground children do not always use the primary
         * extChunk suspend-depth word.  After such a child drains its compact
         * queue, a real wrapper timer queue can still own the transition; keep
         * one host tick alive and route it through wrapper code=2 so the
         * wrapper can finish replacing/resuming the foreground.  The broad
         * wrapper busy bit is only a fallback for older wrappers without a
         * discovered scheduler layout.
         */
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
        arm_ext_keep_host_tick_alive(m);
    }
    if (code == 2 && !m->host_timer_pending &&
        foreground_child_timer_live_post) {
        /*
         * A proven foreground child timer queue is runnable work by itself.
         * Socket-driven children also use this path while polling network
         * responses, but the error/close transition after an HTTP failure can
         * leave no sockets open while the child still has a cleanup timer node.
         */
        arm_ext_keep_host_tick_alive(m);
    }

    /* wrapper 的定时器回调中，前台子模块可能已经完成（如下载器
     * verdload.ext 下载完毕后清空了自己的定时器队列）。此时子模块虽然
     * 仍占据 active_p_addr，但已不再需要前台绘制权——它的所有定时器节点
     * 已耗尽，不再有后续回调会推进其 UI 状态。
     *
     * 真机上 wrapper 的 resume() 会递减 extChunk[0x34] 并归还前台，但
     * 某些 wrapper 路径（如下载完成后直接启动新应用 promote.mrp）不走
     * resume，导致 extChunk[0x34] 一直 > 0。模拟器需要在此检测前台子模块
     * 已失活（无私有定时器），并将前台控制权交还给 wrapper/primary，
     * 使后续 wrapper 的画面输出（或新加载的子模块的画面输出）能被宿主
     * 接受并呈现。
     *
     * 条件：
     *   - code=2 定时器回调
     *   - 存在已暂停的前台子模块
     *   - 子模块无私有定时器队列（compact/frame timer 均空）
     *   - wrapper 已接管定时器（或回调刚路由给 wrapper） */
    if (code == 2 &&
        suspended_foreground_child &&
        !foreground_child_has_private_timer &&
        m->active_p_addr != active_p_addr_pre) {
        /* active_p_addr 在回调中被 table[25] 更新：wrapper 加载了新的
         * 前台子模块（如从 verdload 过渡到 promote），旧子模块的前台
         * 覆盖区域需要清除，让新模块能正常绘制。 */
        arm_ext_clear_foreground_screen_owner(m);
        m->modal_screen_snapshot_valid = 0;
        m->modal_fg_snapshot_valid = 0;
    } else if (code == 2 &&
               suspended_foreground_child &&
               !foreground_child_has_private_timer &&
               (call_p_addr == m->p_addr ||
                m->timer_p_addr == m->p_addr ||
                wrapper_timer_live_post)) {
        /* wrapper 已接管定时器但 active_p_addr 未在本次回调中变化：
         * 旧的前台子模块已失活（定时器队列空），但 wrapper 尚未通过
         * table[25] 加载新模块。清除前台状态让 wrapper 后续的画面
         * 输出能通过宿主的 screen_write/present 接受路径。 */
        arm_ext_clear_foreground_screen_owner(m);
        m->modal_screen_snapshot_valid = 0;
        m->modal_fg_snapshot_valid = 0;
    }

    /* wrapper helper code=2 自己消费 timer delta chain。宿主侧只负责在
     * timer tick 到达时调用 helper，不伪造 wrapper busy 状态或 code=5。 */
    uint32_t modal_suspend_depth_post = modal_suspend_depth_pre;
    if (modal_ext_chunk && arm_ptr(m, modal_ext_chunk + AEX_CHUNK_SUSPEND_OFF))
        memcpy(&modal_suspend_depth_post, arm_ptr(m, modal_ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
    int wrapper_entered_modal =
        code == 1 && has_separate_wrapper && input_len >= 12 && input_addr &&
        modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0;
    if (modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0 &&
        call_modal_snapshot_candidate) {
        m->modal_screen_snapshot_valid = 1;
    }
    /* 进入模态（0->1）：标记前台分发区快照有效，供关闭时还原事件路由。 */
    if (modal_suspend_depth_pre == 0 && modal_suspend_depth_post > 0 &&
        m->modal_fg_snapshot) {
        m->modal_fg_snapshot_valid = 1;
    }
    /* 通用模态框关闭处理（suspend depth >0 → 0）。
     * 真机上 wrapper 的 resume(0xE83220) 恢复 game 状态（定时器链表头、
     * pauseApp/resumeApp 回调），模拟器只需做模块级清理：
     *  - 将前台归还给 primary game
     *  - 清除子模块残留的定时器归属
     *  - 恢复 game timer head（wrapper 的 suspend 清零了它）
     *  - 恢复模态前屏幕快照
     *  - 补启宿主定时器让 wrapper 完成 resume 队列 */
    if (modal_suspend_depth_pre > 0 && modal_suspend_depth_post == 0) {
        m->active_helper_addr = m->primary_helper_addr;
        m->active_p_addr = m->primary_p_addr;
        arm_ext_clear_foreground_screen_owner(m);
        /* 还原 wrapper 前台分发区，使事件路由回到下层页面（修复返回后
         * 无法二次进入子模块界面）。 */
        arm_ext_restore_modal_fg_snapshot(m);
        if (foreground_child_active) {
            arm_ext_reset_child_modules(m);
            if (m->timer_p_addr &&
                m->timer_p_addr != m->primary_p_addr &&
                m->timer_p_addr != m->p_addr) {
                m->timer_p_addr = 0;
                m->timer_helper_addr = 0;
            }
        }
        if (m->saved_game_timer_head) {
            uint32_t grw = 0;
            if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
                memcpy(&grw, arm_ptr(m, m->primary_p_addr), 4);
            /* wrapper suspend 会临时清空 game timer head；仅当 resume/close
             * 结束后仍为空时恢复旧 head。若 game 在关闭回调里已经安装了
             * 新 timer，覆盖它会丢失后续异步请求处理。 */
            if (grw && read_game_timer_head(m, grw) == 0)
                write_game_timer_head(m, grw, m->saved_game_timer_head);
            m->saved_game_timer_head = 0;
        }
        arm_ext_restore_modal_screen_snapshot(m);
        m->primary_child_reopen_timer_needed = 1;
        reopen_set_this_call = 1;
    }

    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (arm_ext_finish_callback_state(m, modal_ext_chunk)) {
        /* mr_exit() 在存在 old app 时会重启上层并返回；当前 ARM 回调的
         * 控制权已经交给平台层，不能继续读取旧 helper 的 output/R0。 */
        return MR_SUCCESS;
    }
    /* 模态框关闭后的定时器恢复。
     * 真机上 game 的 table[31/32] 经 mrc_extTimerStart/Stop 路由到
     * wrapper 的定时器队列，不影响宿主定时器。模拟器里 game 的
     * table[32] 直接停掉宿主定时器，需要反复补启直到 game 完成
     * 内部状态转换。仅在新模态进入时清除标志。 */
    if (m->primary_child_reopen_timer_needed &&
        !m->host_timer_pending && mr_timer_state == 0) {
        mr_timerStart(100);
        mr_timer_state = 1;
        m->host_timer_pending = 1;
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
    }
    if (m->primary_child_reopen_timer_needed && !reopen_set_this_call) {
        /* 新模态进入：child 加载成功并进入前台层 */
        if (modal_suspend_depth_post > 0 && modal_suspend_depth_pre == 0)
            m->primary_child_reopen_timer_needed = 0;
    }

    uint32_t arm_output = 0;
    uint32_t arm_output_len = 0;
    uc_mem_read(m->uc, outp_addr, &arm_output, 4);
    uc_mem_read(m->uc, outl_addr, &arm_output_len, 4);
    if (output_len) *output_len = (int32)arm_output_len;
    if (output) *output = arm_output ? (uint8 *)arm_ptr(m, arm_output) : NULL;
    int32_t call_result = (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
    if (arm_ext_diag_on()) {
        printf("DIAG arm_ext_return code=%d input_len=%u callR0=%d wrapperRaw=%d enteredModal=%d modalPre=%u modalPost=%u activeP=0x%X activeH=0x%X timerP=0x%X timerH=0x%X hostTimer=%d mrTimer=%d\n",
               (int)code, input_len, call_result, wrapper_raw_event_routed,
               wrapper_entered_modal, modal_suspend_depth_pre,
               modal_suspend_depth_post, m->active_p_addr,
               m->active_helper_addr, m->timer_p_addr, m->timer_helper_addr,
               m->host_timer_pending, mr_timer_state);
    }
    arm_ext_dispatch_pending_sms_result(m);

    /* 20-byte 原始事件已经先经过 wrapper；这里向宿主返回 MR_SUCCESS，
     * 避免同一个输入随后又经 Lua 的 12-byte _strCom(801) 转发重复处理。
     * 事件导致 0->1 进入模态时也必须消费；12-byte 内部转发仍返回
     * helper 的真实 R0，供非模态前台模块在清掉可见层后继续把事件交给 game/Lua。 */
    if (wrapper_raw_event_routed || wrapper_entered_modal) return MR_SUCCESS;
    return call_result;
}

uint32 arm_ext_helper_addr(ArmExtModule *m) {
    return m ? m->helper_addr : 0;
}

uint32 arm_ext_primary_helper(ArmExtModule *m) {
    if (!m) return 0;
    uint32_t ret = 0;
    uint32_t ext_chunk = 0, dispatch = 0;
    if (m->timer_p_addr && m->timer_helper_addr &&
        !(m->timer_p_addr == m->p_addr && m->timer_helper_addr == m->helper_addr)) {
        ArmExtNestedModule *primary_mod = arm_ext_find_nested_module_by_p(
            m, m->primary_p_addr);
        if (m->timer_p_addr == m->primary_p_addr &&
            m->timer_helper_addr == m->primary_helper_addr &&
            primary_mod && primary_mod->compact_timer_walker_addr) {
            /* mr_timer only tests this return value for truth.  Report the
             * byte-proven direct walker so call_dispatch can preserve primary
             * R9 instead of re-entering the helper's wrapper routing path. */
            ret = primary_mod->compact_timer_walker_addr;
        }
        /* gxdzc/netpay 的“请稍后...”路径由 wrapper 自己在 0xE83A69
         * 启动 10ms timer；该 owner 不能屏蔽宿主侧 wrapper timer 路由。
         * 只有 timer 属于后加载子插件时，才跳过 wrapper timer，避免把
         * 独立子插件 timer 当成 wrapper tick。 */
        goto out;
    }
    if (!(m->primary_helper_addr && m->primary_helper_addr != m->helper_addr)) {
        goto out;
    }
    if (m->wrapper_compact_timer_scheduler_off &&
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr) {
        /* The compact wrapper's public code=2 branch calls its discovered
         * walker and then runs wrapper state post-processing.  Directly
         * entering only the walker can leave a primary due node detached from
         * the scheduler current head when the outer bridge drains. */
        goto out;
    }
    if (m->primary_child_reopen_timer_needed) {
        /*
         * After a modal child closes, the wrapper still owns one or more
         * resume/reopen timer ticks.  The normal helper code=2 path preserves
         * the wrapper's ARM-side argument setup for that transition; direct
         * 0xE83A81 dispatch fabricates only the primary extChunk ABI.  DOTA's
         * browser launch hits this boundary immediately after the download
         * result modal closes, before brw.ext/frame.ext are registered.
         */
        goto out;
    }
    if (arm_ext_has_foreground_child(m)) {
        int child_has_private_timer =
            arm_ext_foreground_child_has_frame_timer_queue(m) ||
            arm_ext_foreground_child_has_compact_timer_queue(m);
        int wrapper_compact_timer_live =
            m->wrapper_compact_timer_scheduler_off &&
            arm_ext_wrapper_has_timer_queue(m);
        uint32_t suspend_depth = 0;
        int active_modal =
            arm_ext_suspend_depth_for_p(m, m->active_p_addr,
                                        &suspend_depth) &&
            suspend_depth > 0;
        uint32_t active_chunk = 0;
        uint32_t active_dispatch = 0;
        if (m->active_p_addr && arm_ptr(m, m->active_p_addr + AEX_P_EXT_CHUNK_OFF)) {
            memcpy(&active_chunk, arm_ptr(m, m->active_p_addr + AEX_P_EXT_CHUNK_OFF), 4);
        }
        if (active_chunk && arm_ptr(m, active_chunk + AEX_CHUNK_EVENT_FUNC_OFF)) {
            memcpy(&active_dispatch, arm_ptr(m, active_chunk + AEX_CHUNK_EVENT_FUNC_OFF), 4);
        }
        uint32_t active_dispatch_addr = active_dispatch & ~1u;
        /*
         * Old direct-dispatch wrappers require the normal helper code=2 path
         * while a non-modal foreground child is active; the synthesized ABI
         * otherwise reaches child helpers with wrapper arguments. Compact
         * wrapper walkers are different: disassembly proves they consume a
         * real R9-relative node queue, and runtime diagnostics show live nodes
         * shaped as cb=wrapperThunk/data=extChunk. Let only that proven compact
         * queue bypass the foreground-child guard so the wrapper can dispatch
         * the node under its own ARM ABI.
         */
        if (!wrapper_compact_timer_live &&
            (child_has_private_timer || !active_modal ||
             !active_dispatch ||
             active_dispatch_addr < EXT_CODE_ADDR ||
             active_dispatch_addr >= EXT_CODE_ADDR + m->code_len)) {
            goto out;
        }
    }
    if (!m->primary_p_addr || !arm_ptr(m, m->primary_p_addr + AEX_P_EXT_CHUNK_OFF)) {
        goto out;
    }
    memcpy(&ext_chunk, arm_ptr(m, m->primary_p_addr + AEX_P_EXT_CHUNK_OFF), 4);
    if (ext_chunk && arm_ptr(m, ext_chunk + AEX_CHUNK_EVENT_FUNC_OFF)) {
        memcpy(&dispatch, arm_ptr(m, ext_chunk + AEX_CHUNK_EVENT_FUNC_OFF), 4);
    }
    uint32_t dispatch_addr = dispatch & ~1u;
    /* 只有指向外层 wrapper 代码段、且已定位到 wrapper timer queue consumer
     * 的 extChunk dispatch 才需要宿主从 arm_ext_call_dispatch() 直接驱动。
     * 另一些 wrapper（gghjt cfunction.ext 反汇编：helper code=2 分支进入
     * 0xE83E6C）把 timer consumer 收在 helper switch 里；这类应走普通
     * code=2 路由，由当前 timer owner 选择 wrapper helper。mpc/game.ext 的
     * extChunk[0x28] 还可指向后续嵌套 EXT 的 code dispatcher，它的 timer
     * 已通过 table[31]/[32] veneer 接入，不能按 wrapper tick 调用。 */
    /* chain_walker_thunk 通过运行时 ARM 代码 patch 集成到 wrapper 的
     * code=2 分支中，不依赖外部时序门控。这是通用的 wrapper timer dispatch
     * 机制修复。仅对有 wrapper_timer_dispatch_addr 的 wrapper 启用 dispatch。 */
    ret = (dispatch && (dispatch & 1u) &&
           dispatch_addr >= EXT_CODE_ADDR &&
           dispatch_addr < EXT_CODE_ADDR + m->code_len &&
           m->wrapper_timer_dispatch_addr)
          ? m->primary_helper_addr : 0;
out:
    if (arm_ext_diag_on()) {
        printf("DIAG primary_helper ret=0x%X timerP=0x%X timerH=0x%X wrapperP=0x%X wrapperH=0x%X primaryP=0x%X primaryH=0x%X chunk=0x%X dispatch=0x%X wrapperTimer=0x%X\n",
               ret, m->timer_p_addr, m->timer_helper_addr, m->p_addr, m->helper_addr,
               m->primary_p_addr, m->primary_helper_addr, ext_chunk, dispatch,
               m->wrapper_timer_dispatch_addr);
    }
    return ret;
}

static int arm_ext_call_primary_compact_dispatch(
    ArmExtModule *m, ArmExtNestedModule *primary_mod, uint32_t timer_interval) {
    if (!m || !primary_mod || !primary_mod->compact_timer_walker_addr)
        return MR_FAILED;
    uint32_t rw = arm_ext_primary_rw_base_(m);
    if (!rw) return MR_FAILED;

    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 32u;
    uint32_t stack_args[8] = {0};
    uc_mem_write(m->uc, sp, stack_args, sizeof(stack_args));
    reg_write32(m->uc, UC_ARM_REG_R9, rw);
    reg_write32(m->uc, UC_ARM_REG_R0, 0);
    reg_write32(m->uc, UC_ARM_REG_R1, 0);
    reg_write32(m->uc, UC_ARM_REG_R2, 0);
    reg_write32(m->uc, UC_ARM_REG_R3, 0);
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    uint16 *saved_screen_buf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screen_buf, &present_depth_before);
    /* The host event has been consumed.  A guest table[31] call below records
     * the exact next interval and sets this flag again. */
    m->host_timer_pending = 0;
    sync_internal_state_to_arm(m);
    arm_ext_diag_dump_primary_compact_timer_nodes(m, "primary_dispatch_pre");
    arm_ext_sanitize_compact_timer_heaps(m);

    m->current_p_addr = m->primary_p_addr;
    m->current_helper_addr = primary_mod->compact_timer_walker_addr;
    int previous_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int ret = run_arm_with_sp(m, primary_mod->compact_timer_walker_addr, sp);
    m->in_dispatch = previous_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;

    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screen_buf, present_depth_before);
    capture_timer_dispatches(m);
    arm_ext_sanitize_compact_timer_heaps(m);
    arm_ext_diag_dump_primary_compact_timer_nodes(m, "primary_dispatch_post");

    int wrapper_live = arm_ext_wrapper_has_timer_queue(m);
    int primary_live = arm_ext_primary_has_compact_timer_queue(m);
    int third_party_owner =
        m->timer_p_addr && m->timer_helper_addr &&
        !(m->timer_p_addr == m->p_addr &&
          m->timer_helper_addr == m->helper_addr) &&
        !(m->timer_p_addr == m->primary_p_addr &&
          m->timer_helper_addr == m->primary_helper_addr);
    /* A due callback may start a timer for a newly loaded child.  Its
     * table[31] owner is newer than the compact queues sampled here. */
    if (!third_party_owner && wrapper_live) {
        /* The primary walker rearmed its outer bridge through the wrapper
         * timer veneer, so the next guest-scheduled tick belongs there. */
        m->timer_p_addr = m->p_addr;
        m->timer_helper_addr = m->helper_addr;
    } else if (!third_party_owner && primary_live) {
        m->timer_p_addr = m->primary_p_addr;
        m->timer_helper_addr = m->primary_helper_addr;
    } else if (!third_party_owner) {
        m->timer_p_addr = 0;
        m->timer_helper_addr = 0;
    }
    if (arm_ext_diag_on() || arm_ext_timer_owner_diag_on()) {
        printf("DIAG primary_compact_dispatch walker=0x%X wrapperLive=%d primaryLive=%d thirdParty=%d hostTimer=%d interval=%u nextP=0x%X nextH=0x%X\n",
               primary_mod->compact_timer_walker_addr, wrapper_live,
               primary_live, third_party_owner, m->host_timer_pending,
               timer_interval,
               m->timer_p_addr, m->timer_helper_addr);
    }
    arm_ext_dispatch_pending_sms_result(m);
    return ret;
}

int arm_ext_consume_primary_host_init(ArmExtModule *m) {
    if (!m || !m->primary_host_init_pending) return 0;
    m->primary_host_init_pending = 0;
    return 1;
}

void arm_ext_reset_child_modules(ArmExtModule *m) {
    if (!m) return;
    /* 保留 primary 模块，清除所有其他嵌套模块注册。
     * child 流程关闭后 nested_modules 里残留的子插件条目会干扰
     * hook_restore_r9 的 R9 同步和 arm_ext_p_for_code_addr 的
     * 代码归属判定，导致 wrapper 内部分发走错路径。 */
    int out = 0;
    for (int i = 0; i < m->nested_module_count; ++i) {
        ArmExtNestedModule *mod = &m->nested_modules[i];
        int is_primary = m->primary_file_addr &&
                         mod->file_addr == m->primary_file_addr &&
                         mod->file_len == m->primary_file_len;
        if (is_primary) {
            m->nested_modules[out++] = *mod;
        }
    }
    m->nested_module_count = out;
    m->active_p_addr = m->primary_p_addr;
    m->active_helper_addr = m->primary_helper_addr;
    arm_ext_clear_foreground_screen_owner(m);
}

uint32 arm_ext_read_timer_queue(ArmExtModule *m) {
    if (!m || !m->primary_p_addr) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->primary_p_addr), 4);
    return read_game_timer_head(m, rw);
}


int arm_ext_call_dispatch(ArmExtModule *m, int is_stop, uint32_t timer_interval) {
    if (!m || !m->p_addr) return MR_FAILED;
    /* 不变量入口检查(P0.3),同 arm_ext_call */
    arm_ext_verify_invariants(m, "dispatch-entry");
    ArmExtNestedModule *primary_mod = arm_ext_find_nested_module_by_p(
        m, m->primary_p_addr);
    if (m->timer_p_addr == m->primary_p_addr &&
        m->timer_helper_addr == m->primary_helper_addr &&
        primary_mod && primary_mod->compact_timer_walker_addr) {
        /* A primary compact owner must enter the discovered walker directly.
         * Calling code=2 on its public helper can route back through the outer
         * wrapper and strand a due-list node outside the scheduler head. */
        return arm_ext_call_primary_compact_dispatch(
            m, primary_mod, timer_interval);
    }
    uint32_t nested_p = m->primary_p_addr ? m->primary_p_addr : 0;
    if (!nested_p && m->last_file_addr) {
        void *p4 = arm_ptr(m, m->last_file_addr + 4);
        if (p4) memcpy(&nested_p, p4, 4);
    }
    if (!nested_p || !arm_ptr(m, nested_p + AEX_P_EXT_CHUNK_OFF)) return MR_FAILED;
    /* 读取 P->mrc_extChunk (offset 12) */
    uint32_t ext_chunk = 0;
    memcpy(&ext_chunk, arm_ptr(m, nested_p + AEX_P_EXT_CHUNK_OFF), 4);
    if (!ext_chunk || !arm_ptr(m, ext_chunk + AEX_CHUNK_EVENT_FUNC_OFF)) return MR_FAILED;
    /* 读取目标函数 extChunk[0x28] 和参数 extChunk[0x24] */
    uint32_t func = 0, param = 0;
    memcpy(&func, arm_ptr(m, ext_chunk + AEX_CHUNK_EVENT_FUNC_OFF), 4);
    memcpy(&param, arm_ptr(m, ext_chunk + AEX_CHUNK_EVENT_DATA_OFF), 4);
    int wrapper_timer_owner =
        m->timer_p_addr == m->p_addr &&
        m->timer_helper_addr == m->helper_addr &&
        m->wrapper_timer_dispatch_addr != 0;
    int compact_wrapper_timer_owner =
        wrapper_timer_owner && m->wrapper_compact_timer_scheduler_off != 0;
    /* 19KB cfunction.ext 的 chain walker thunk：wrapper 不使用宿主直接
     * 驱动（wrapper_timer_dispatch_addr==0），但有 chain_walker_thunk_addr。
     * 宿主 timer 每次触发时让 chain walker 遍历 timer chain，通过
     * 0xE83590 在 wrapper 栈上下文中安全调用 game helper(code=5)。 */
    int chain_walker_owner = !wrapper_timer_owner && m->chain_walker_thunk_addr != 0;
    if (wrapper_timer_owner) {
        /*
         * wrapper 自己的 table[31] timerStart 来自 timer queue 管理函数
         * 0xE83A34，下一次宿主 timer 必须进入 0xE83A80 的队列消费逻辑；
         * extChunk[0x28] 是事件/控件分发入口 0xE830BD，会把节点重新挂回
         * 队列而不是执行到 0xE83B46 的回调。验证：gxdzc 付费点击后
         * 0x66FFBD 回调被消费并显示 netpay 付费界面。
         */
        func = m->wrapper_timer_dispatch_addr;
    } else if (chain_walker_owner) {
        func = m->chain_walker_thunk_addr;
    }
    if (arm_ext_diag_on()) {
        printf("DIAG call_dispatch nestedP=0x%X chunk=0x%X func=0x%X param=0x%X wrapperOwner=%d timerP=0x%X timerH=0x%X\n",
               nested_p, ext_chunk, func, param, wrapper_timer_owner,
               m->timer_p_addr, m->timer_helper_addr);
    }
    if (!func) return MR_FAILED;
    /* 以 wrapper 的 R9 调用目标函数:
     * R0=extChunk, R1=is_stop, R2=param, R3=timer_interval */
    uint32_t wrapper_rw = 0;
    memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
    /* wrapper 的 RW 段基址由 ARM ext 在 mr_c_function_load 期间设置。
     * 部分 wrapper（如 mrpinfo 的 cfunction.ext）在 Unicorn 环境下
     * 不初始化 R9，导致 P[0]=0。此时 dispatch 无法正确访问 wrapper 数据，
     * 跳过即可——primary ext 的定时器仍可通过 arm_ext_call(code=2) 驱动。 */
    if (!wrapper_rw) return MR_FAILED;

    reg_write32(m->uc, UC_ARM_REG_R9, wrapper_rw);
    reg_write32(m->uc, UC_ARM_REG_R0, ext_chunk);
    reg_write32(m->uc, UC_ARM_REG_R1, (uint32_t)is_stop);
    reg_write32(m->uc, UC_ARM_REG_R2, param);
    reg_write32(m->uc, UC_ARM_REG_R3, timer_interval);
    /* dispatch 的栈参数全为0 */
    uint32_t sp = EXT_STACK_ADDR + EXT_STACK_SIZE - 32;
    uint32_t stack_args[8] = {0};
    uc_mem_write(m->uc, sp, stack_args, sizeof(stack_args));
    reg_write32(m->uc, UC_ARM_REG_SP, sp);

    /* 保存 dispatch 前的 game timer head，用于模态框取消时恢复 */
    uint32_t _game_rw = 0, _s8_pre = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&_game_rw, arm_ptr(m, m->primary_p_addr), 4);
    if (_game_rw)
        _s8_pre = read_game_timer_head(m, _game_rw);
    uint32_t suspend_depth_pre = 0;
    if (arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF))
        memcpy(&suspend_depth_pre, arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
    int modal_snapshot_candidate = 0;
    /* cfunction.ext 的 suspend(0xE831A4) 在同一次 dispatch 内把
     * extChunk[0x34] 从 0 加到 1 并绘制模态层。这里只在尚未 suspend
     * 时抓取 framebuffer，dispatch 后确认 0->1 再标记为可用于 cancel 恢复。 */
    if (suspend_depth_pre == 0) {
        if (_s8_pre != 0)
            modal_snapshot_candidate = arm_ext_save_modal_screen_snapshot(m);
        /* 同步快照 wrapper 前台分发区（timer 内进入模态子模块的路径）。 */
        arm_ext_save_modal_fg_snapshot(m);
    }

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    /* This entry consumes the pending host tick.  A guest table[31] call below
     * will set the flag again with the scheduler's calculated next interval. */
    m->host_timer_pending = 0;
    sync_internal_state_to_arm(m);
    if (compact_wrapper_timer_owner) {
        arm_ext_diag_dump_wrapper_compact_timer_nodes(m, "dispatch_pre");
        arm_ext_diag_dump_primary_compact_timer_nodes(m, "dispatch_pre");
    }
    arm_ext_sanitize_compact_timer_heaps(m);
    /* 19KB cfunction.ext 的 timer dispatcher 是 chain walker；它从
     * wrapper_rw+0x190 取节点并调用节点回调 0xE83590。该回调会读取
     * extChunk[8] 调 game helper(code=5)，所以这里只修复可能被 wrapper
     * 清零的 helper 槽，不篡改 suspend depth。 */
    if ((wrapper_timer_owner || chain_walker_owner) && suspend_depth_pre == 0 &&
        arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF)) {
        /* 确保 extChunk[8] = game helper (wrapper 代码可能已清零) */
        if (m->primary_helper_addr && arm_ptr(m, ext_chunk + 8)) {
            memcpy(arm_ptr(m, ext_chunk + 8), &m->primary_helper_addr, 4);
        }
        /*
         * Chain walkers and compact wrapper queues both dispatch through node
         * data. The compact callback at 0xE83BD0 loads helper from data+8
         * before blx, so repair every registered extChunk helper slot instead
         * of only the primary module.
         */
        if (chain_walker_owner || compact_wrapper_timer_owner) {
            for (int ci = 0; ci < m->nested_module_count; ++ci) {
                ArmExtNestedModule *mod = &m->nested_modules[ci];
                if (!mod->p_addr || !mod->helper_addr) continue;
                uint32_t child_chunk = 0;
                if (arm_ptr(m, mod->p_addr + AEX_P_EXT_CHUNK_OFF))
                    memcpy(&child_chunk, arm_ptr(m, mod->p_addr + AEX_P_EXT_CHUNK_OFF), 4);
                if (child_chunk && arm_ptr(m, child_chunk + 8)) {
                    uint32_t cur = 0;
                    memcpy(&cur, arm_ptr(m, child_chunk + 8), 4);
                    if (!cur) {
                        memcpy(arm_ptr(m, child_chunk + 8), &mod->helper_addr, 4);
                    }
                }
            }
        }
    }
    m->current_p_addr = m->p_addr;
    m->current_helper_addr = m->helper_addr;
    int prev_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int ret = run_arm_with_sp(m, func, sp);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    if (compact_wrapper_timer_owner) {
        arm_ext_diag_dump_wrapper_compact_timer_nodes(m, "dispatch_post");
        arm_ext_diag_dump_primary_compact_timer_nodes(m, "dispatch_post");
    }
    arm_ext_sanitize_compact_timer_heaps(m);
    /* D1:旧版 depth_patched 恢复块已删——该标志在函数内从未被置位,恢复
     * 分支自始不可达(19KB chain walker 不需要 suspend depth 补丁) */
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    capture_timer_dispatches(m);
    if (wrapper_timer_owner) {
        int queue_live = 0;
        uint32_t queue_base_ms = 0;
        uint32_t q = 0;
        if (m->wrapper_compact_timer_scheduler_off) {
            /*
             * The wrapper walker scan proved the scheduler offset.  Keep the
             * host timer alive only when that real compact queue still has
             * nodes; unrelated wrapper state must not pin ownership here.
             */
            q = wrapper_rw + m->wrapper_compact_timer_scheduler_off;
            queue_live = arm_ext_wrapper_has_timer_queue(m);
            if (arm_ptr(m, q + 20))
                memcpy(&queue_base_ms, arm_ptr(m, q + 20), 4);
        } else {
            uint32_t queue_head = 0;
            q = wrapper_rw + 0x1FCu;
            if (arm_ptr(m, q + 20)) {
                memcpy(&queue_head, arm_ptr(m, q + 12), 4);
                memcpy(&queue_base_ms, arm_ptr(m, q + 20), 4);
            }
            /* 19KB wrapper timer walker 使用 wrapper_rw+0x190 的桶数组；旧
             * 0x274 位置只是早期误判的诊断偏移，不能用来决定宿主 timer 生死。 */
            uint32_t active_index = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x18u);
            uint32_t tc_after = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x190u + active_index * 8u);
            queue_live = queue_head || tc_after;
        }
        if (queue_live && !m->host_timer_pending) {
            /* The guest normally rearms table[31] itself.  Supply a short host
             * tick only when it left runnable nodes without scheduling one. */
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            mr_timerStart((uint16)timer_interval);
        }
        if (queue_live && q && queue_base_ms == 0 && arm_ptr(m, q + 20)) {
            uint32_t now = mr_getTime();
            memcpy(arm_ptr(m, q + 20), &now, 4);
        }
        if (!queue_live && compact_wrapper_timer_owner &&
            arm_ext_primary_has_compact_timer_queue(m)) {
            /* Direct compact dispatch can consume the outer bridge node while
             * the primary module still has a valid inner scheduler node.  The
             * next tick must enter the primary helper once so it can rearm the
             * wrapper bridge; an empty wrapper queue alone is not global idle. */
            m->timer_p_addr = m->primary_p_addr;
            m->timer_helper_addr = m->primary_helper_addr;
            if (!m->host_timer_pending) {
                mr_timer_state = 1;
                m->host_timer_pending = 1;
                internal_slot_write(m, m->mr_timer_state_slot, 1);
                mr_timerStart((uint16)timer_interval);
            }
            if (arm_ext_diag_on() || arm_ext_timer_owner_diag_on()) {
                printf("DIAG dispatch_owner_clear emptyWrapper primaryCompact=1\n");
            }
        }
    }
    /* chain walker owner: 保持宿主 timer 运行并恢复 suspend depth */
    if (chain_walker_owner) {
        uint32_t active_index = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x18u);
        uint32_t tc_after = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x190u + active_index * 8u);
        if (tc_after && !m->host_timer_pending) {
            mr_timer_state = 1;
            m->host_timer_pending = 1;
            internal_slot_write(m, m->mr_timer_state_slot, 1);
            mr_timerStart((uint16)timer_interval);
        }
        /* 0xE83590 递减了 extChunk[0x34]，恢复所有嵌套模块的 suspend depth */
        if (arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF))
            memcpy(arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF), &suspend_depth_pre, 4);
        for (int ci = 0; ci < m->nested_module_count; ++ci) {
            ArmExtNestedModule *mod = &m->nested_modules[ci];
            if (!mod->p_addr) continue;
            uint32_t child_chunk = 0;
            if (arm_ptr(m, mod->p_addr + AEX_P_EXT_CHUNK_OFF))
                memcpy(&child_chunk, arm_ptr(m, mod->p_addr + AEX_P_EXT_CHUNK_OFF), 4);
            if (child_chunk && child_chunk != ext_chunk &&
                arm_ptr(m, child_chunk + AEX_CHUNK_SUSPEND_OFF)) {
                uint32_t zero = 0;
                memcpy(arm_ptr(m, child_chunk + AEX_CHUNK_SUSPEND_OFF), &zero, 4);
            }
        }
    }

    /* wrapper dispatch 可能清零 game timer head（模态框进入时），保存旧值 */
    /* dispatch 后重新读 game_rw（ARM 代码可能修改了 primary_p_addr[0]） */
    {
        uint32_t _grw2 = 0;
        if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
            memcpy(&_grw2, arm_ptr(m, m->primary_p_addr), 4);
        uint32_t _s8_post = read_game_timer_head(m, _grw2);
        if (_s8_post == 0 && _s8_pre != 0) {
            m->saved_game_timer_head = _s8_pre;
        }
        uint32_t suspend_depth_post = 0;
        if (arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF))
            memcpy(&suspend_depth_post, arm_ptr(m, ext_chunk + AEX_CHUNK_SUSPEND_OFF), 4);
        if (suspend_depth_pre == 0 && suspend_depth_post > 0 && modal_snapshot_candidate) {
            m->modal_screen_snapshot_valid = 1;
        }
        if (suspend_depth_pre == 0 && suspend_depth_post > 0 && m->modal_fg_snapshot) {
            m->modal_fg_snapshot_valid = 1;
        }
        /* 通用模态框关闭：与 arm_ext_call 中相同的通用清理 */
        if (suspend_depth_pre > 0 && suspend_depth_post == 0) {
            m->active_helper_addr = m->primary_helper_addr;
            m->active_p_addr = m->primary_p_addr;
            arm_ext_clear_foreground_screen_owner(m);
            /* 还原 wrapper 前台分发区，使事件路由回到下层页面。 */
            arm_ext_restore_modal_fg_snapshot(m);
            arm_ext_reset_child_modules(m);
            if (m->saved_game_timer_head) {
                /* 与 arm_ext_call 的模态关闭路径保持一致：只填补仍为空的
                 * game timer head，不覆盖关闭回调中新挂上的 timer。 */
                if (_grw2 && read_game_timer_head(m, _grw2) == 0)
                    write_game_timer_head(m, _grw2, m->saved_game_timer_head);
                m->saved_game_timer_head = 0;
            }
            arm_ext_restore_modal_screen_snapshot(m);
            m->primary_child_reopen_timer_needed = 1;
        }
    }

    if (arm_ext_finish_callback_state(m, ext_chunk)) {
        /* dispatch 已经触发平台退出/重启语义；返回成功让外层停止继续解释
         * 本次 wrapper timer 的旧返回值。 */
        return MR_SUCCESS;
    }
    arm_ext_dispatch_pending_sms_result(m);
    return ret;
}

int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, 0);
    reg_write32(m->uc, UC_ARM_REG_R1, 0);
    reg_write32(m->uc, UC_ARM_REG_R2, 0);
    reg_write32(m->uc, UC_ARM_REG_R3, 0);

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    m->current_helper_addr = func;
    int prev_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int call_ret = run_arm(m, func);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    arm_ext_dispatch_pending_sms_result(m);
    return MR_SUCCESS;
}

int arm_ext_invoke3(ArmExtModule *m, uint32 func, uint32 arg0, uint32 arg1,
                    uint32 arg2, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
    restore_ext_r9(m);
    reg_write32(m->uc, UC_ARM_REG_R0, arg0);
    reg_write32(m->uc, UC_ARM_REG_R1, arg1);
    reg_write32(m->uc, UC_ARM_REG_R2, arg2);

    uint16 *saved_screenBuf = NULL;
    uint32_t present_depth_before = 0;
    enter_screen_context(m, &saved_screenBuf, &present_depth_before);
    sync_internal_state_to_arm(m);
    m->current_p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    m->current_helper_addr = func;
    int prev_in_dispatch = m->in_dispatch;
    m->in_dispatch = 1;
    int call_ret = run_arm(m, func);
    m->in_dispatch = prev_in_dispatch;
    m->current_p_addr = 0;
    m->current_helper_addr = 0;
    sync_timer_state_from_arm(m);
    leave_screen_context(m, saved_screenBuf, present_depth_before);
    if (call_ret != MR_SUCCESS) return MR_FAILED;

    if (ret_out) *ret_out = (int32)reg_read32(m->uc, UC_ARM_REG_R0);
    arm_ext_dispatch_pending_sms_result(m);
    return MR_SUCCESS;
}

void arm_ext_host_cache_sync(ArmExtModule *m, const void *host_data, uint32 len) {
    if (!m || !host_data || !len) return;
    if (!m->pending_internal_file_addr || !m->pending_internal_file_len) return;
    uint32_t addr = m->pending_internal_file_addr;
    uint32_t flen = m->pending_internal_file_len;
    uint32_t copy_len = len < flen ? len : flen;
    void *dst = arm_ptr(m, addr);
    if (!dst) return;
    memcpy(dst, host_data, copy_len);
    uc_ctl_remove_cache(m->uc, addr, addr + copy_len);
    /* B4:按实际拷入的 copy_len 解析。宿主数据比 pending 声明短时,
     * [addr+copy_len, addr+flen) 是陈旧字节,按 flen 解析会越过有效数据 */
    arm_ext_sync_internal_nested_module(m, addr, copy_len);
}

uint32_t arm_ext_host_motion_acc_slot(ArmExtModule *m,
                                      int32 x, int32 y, int32 z) {
    /* 动感芯片 T_MOTION_ACC{int32 x,y,z} 上送槽位:MR_MOTION_EVENT 的第三
     * 参数是 guest 可读指针,真机上指向平台侧静态缓冲。这里懒分配一次
     * 12 字节并按样本覆写复用(arm_alloc 是 bump 分配器,不能按事件分配),
     * guest 在事件处理中同步读取,复用同一地址与真机语义一致。 */
    if (!m) return 0;
    if (!m->motion_acc_addr) {
        m->motion_acc_addr = arm_alloc(m, 12u);
        if (!m->motion_acc_addr) return 0;
    }
    uint8_t *dst = (uint8_t *)arm_ptr(m, m->motion_acc_addr);
    if (!dst) return 0;
    memcpy(dst + 0, &x, 4);
    memcpy(dst + 4, &y, 4);
    memcpy(dst + 8, &z, 4);
    return m->motion_acc_addr;
}

void arm_ext_unload(ArmExtModule *m) {
    if (!m) return;
    if (m->profile && m->profile->cleanup)
        m->profile->cleanup(m->app_state);
    m->app_state = NULL;
    mrp_cache_free(m);
    if (m->uc) uc_close(m->uc);
    free(m->last_file_copy);
    free(m->modal_screen_snapshot);
    free(m->modal_fg_snapshot);
    free(m->short_pack_aliases);
    free(m->bump_live);
    free(m->bump_free_blocks);
    free(m->app_live_blocks);
    arm_ext_free_row_spans(&m->screen_damage);
    arm_ext_free_row_spans(&m->screen_present);
    arm_ext_free_row_spans(&m->foreground_cover);
    free(m->low_table);
    free(m->platform_mem);
    free(m->scrram_mem);
    free(m->platform_io_mem);
    free(m->platform_alt_mem);
    free(m->executor_meta_mem);
    if (m->mem_is_mmap) {
#ifdef _MSC_VER
        VirtualFree(m->mem, 0, MEM_RELEASE);
#else
        munmap(m->mem, EXT_MEM_SIZE);
#endif
    } else {
        free(m->mem);
    }
    free(m);
}
