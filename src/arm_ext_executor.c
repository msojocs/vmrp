#include "./include/arm_ext_executor.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* for usleep in busy-wait detection */
#include <sys/mman.h>
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
#define MRP_IS_FILE 1

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
/* 部分 ARM ext 会跳转到 0x1000 等超出原 0x1000 边界的地址，
 * 扩大低地址映射以覆盖这些跳转目标。 */
#define EXT_LOW_TABLE_SIZE 0x2000u
#define EXT_TRACE_PC_RING 64u

typedef struct mr_c_function_P_t {
    uint32 start_of_ER_RW;
    uint32 ER_RW_Length;
    int32 ext_type;
    uint32 mrc_extChunk;
    int32 stack;
} mr_c_function_P_t;

/* MRP 文件缓存：预解析 MRP 包内的所有条目并缓存解压后的数据，
 * 避免 ARM ext 在 Unicorn 下做极慢的 MRP 索引扫描。 */
#define MRP_CACHE_MAX 16
#define MRP_VFD_MAX 4
#define MRP_VFD_BASE 0x7FFF0000u

typedef struct MrpCacheEntry {
    char name[128];
    uint8_t *data;
    uint32_t data_len;
} MrpCacheEntry;

/* 虚拟文件描述符：缓存条目被 open 后通过虚拟 fd 提供 read/seek/close */
typedef struct MrpVirtualFd {
    int in_use;
    const uint8_t *data;
    uint32_t data_len;
    uint32_t pos;
} MrpVirtualFd;

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
    /* gghjt netpay extraction workaround state (see case 43 below) */
    uint32_t pending_chk_arm_buf;
    uint32_t pending_chk_len;
    uint8_t *pending_chk_decomp;
    uint32_t pending_chk_decomp_len;
    uint32_t last_alloc_addr;
    uint32_t last_alloc_len;
    uint32_t nested_p_addr;
    uint32_t active_p_addr;
    uint32_t active_helper_addr;
    uint32_t primary_p_addr;       // First nested EXT is the app logic; later nested EXTs can be helpers.
    uint32_t primary_helper_addr;
    uint32_t primary_file_addr;    // Code base of the first nested EXT (for code[4] updates).
    uint32_t outer_r9;
    uint32_t nested_return_addr;
    uint32_t screen_write_count;
    uint32_t thumb_fix_count;
    uint32_t pc_ring[EXT_TRACE_PC_RING];
    uint32_t cpsr_ring[EXT_TRACE_PC_RING];
    uint32_t pc_ring_pos;
    uint32_t busy_wait_count;
    /* 忙等开始时的真实 mr_getTime 返回值；用于在合法实时延时不能自然完成时
     * 识别"卡死"并触发 mr_exit。 */
    uint32_t busy_wait_start_ms;
    /* 当前 ARM 事件 / 回调中是否做过"真实工作"（除了 mr_getTime 与 arm_alloc
     * 之外的 table 调用，例如绘图、文件 IO 等）。用于区分两种 SP 漂移崩溃
     * 场景：
     *   a) 正常的菜单切换：会先大量绘制后才进入计时器忙等 ⇒ event_did_work=1
     *   b) mpc 的退出路径：跳过 UI 渲染直接进入计时器自旋 ⇒ event_did_work=0
     * 在 (b) 上把 SP-in-code 崩溃当作"应当退出"，调用 mr_exit() 收尾。 */
    int event_did_work;
    int nested_loading;
    int screen_dirty;
    int mem_is_mmap;
    /* R9（rw_base）区域的 GOT 快照：dump/restore 恢复模块内存时用于修复 GOT */
    uint32_t got_snapshot_base;
    uint32_t got_snapshot[EXT_TABLE_COUNT];
    /* 嵌套 ext 覆盖 table[31/32] 的 dispatch 函数地址 */
    uint32_t dispatch_timer_start;
    uint32_t dispatch_timer_stop;
    int in_dispatch; /* 防止 dispatch -> wrapper -> table[31/32] -> dispatch 重入 */
    uc_hook hook;
    /* MRP 文件缓存：避免 ARM ext 在 Unicorn 下做极慢的 MRP 索引扫描 */
    MrpCacheEntry mrp_cache[MRP_CACHE_MAX];
    int mrp_cache_count;
    MrpVirtualFd mrp_vfds[MRP_VFD_MAX];
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
    /*
     * Some nested .mrp plug-ins (e.g. netpay.mrp loaded by gghjt.mrp) keep
     * function pointers without the Thumb bit, so a BLX into them lands in
     * ARM mode while the bytes are Thumb-2.  Do the mode correction once per
     * callback entry.  If the same PC still raises UC_ERR_INSN_INVALID after
     * being retried in Thumb mode, it is real invalid code/data, not a missing
     * Thumb bit.  gghjt's 60s timeout-return path exposes this after dump0
     * restore: a copied timer node (e.g. table[3] to 0x738B34) is data, and an
     * unbounded retry here pins the UI on “请稍等”.  Let the normal invalid-PC
     * handling below stop the callback cleanly instead of spinning forever.
     */
    if (err == UC_ERR_INSN_INVALID) {
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC);
        if (pc != EXT_STOP_ADDR && pc != 0) {
            uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
            if ((cpsr & (1u << 5)) == 0) {
                cpsr |= (1u << 5);
                reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: retrying in Thumb mode at pc=0x%X\n", pc);
                }
                err = uc_emu_start(m->uc, pc, EXT_STOP_ADDR, 0, 0);
            }
        }
    }
    if (err != UC_ERR_OK) {
        if (reg_read32(m->uc, UC_ARM_REG_PC) == EXT_STOP_ADDR) return MR_SUCCESS;
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
        uint32_t cur_sp = reg_read32(m->uc, UC_ARM_REG_SP);
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
            /*
             * ARM 侧栈漂进了代码段——通常是反复忙等导致栈被破坏。这种崩溃
             * 不可恢复，但应当怎么收尾要看回调里之前都做了什么：
             *
             *   • event_did_work == 1：本次事件先完成了真实工作（绘图/IO/
             *     对话框等），然后才在结尾的计时器自旋里崩了。这种属于"正
             *     常 UI 路径"——例如 mpc 的菜单切换、对话框打开等动画；
             *     调用方期望事件回调返回后 app 继续运行。沿用既有的静默
             *     清退处理。
             *
             *   • event_did_work == 0 且确实进入过 mr_getTime 忙等
             *     （busy_wait_count > 0）：本次事件跳过所有渲染，进来就一头
             *     扎进自旋等待。这种是 mpc 的退出回调，原始 ARM 代码期望
             *     在自旋"完成"后调用 mr_exit() 结束进程；在我们的仿真器里
             *     自旋走不到那一步就崩了，因此这里替它把 mr_exit() 补上，
             *     恢复历史上 a5ca8ab "无法退出 mpc" 修复时点的体验。
             */
            int is_exit_spin = (m->event_did_work == 0 && m->busy_wait_count > 0);
            if (is_exit_spin) {
                printf("arm_ext_executor: code stack reached executable area pc=0x%X sp=0x%X (no UI work seen, treating as mpc exit) -> mr_exit\n", pc, cur_sp);
                if (getenv("VMRP_ARM_EXT_TRACE")) dump_pc_ring(m);
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                m->busy_wait_count = 0;
                m->busy_wait_start_ms = 0;
                mr_exit();
                return MR_SUCCESS;
            }
            if (getenv("VMRP_ARM_EXT_TRACE")) {
                printf("arm_ext_executor: ARM callback stopped at non-instruction pc=0x%X sp=0x%X\n", pc, cur_sp);
                dump_pc_ring(m);
            }
            reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
            return MR_SUCCESS;
        }
        /*
         * Nested plug-ins loaded into the heap (e.g. netpay.mrp inside
         * gghjt.mrp) sometimes encode return addresses that drop us a
         * halfword into a 32-bit Thumb-2 instruction. The fetched bytes
         * are then undefined, but a quick check confirms PC-2 starts a
         * valid 32-bit Thumb-2 op -- treat it as a clean stop instead of
         * spamming the console; the caller already handles MR_SUCCESS the
         * same way it handles a normal return.
         */
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_HEAP_ADDR && pc + 2 < EXT_BASE_ADDR + EXT_MEM_SIZE) {
            uint8_t *p = (uint8_t *)arm_ptr(m, pc - 2);
            if (p) {
                uint16_t prev_hw = (uint16_t)(p[0] | (p[1] << 8));
                uint32_t top5 = prev_hw >> 11;
                if (top5 == 0x1d || top5 == 0x1e || top5 == 0x1f) {
                    if (getenv("VMRP_ARM_EXT_TRACE")) {
                        printf("arm_ext_executor: plugin returned to mid-Thumb2 pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                        dump_pc_ring(m);
                    }
                    reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                    return MR_SUCCESS;
                }
            }
        }
        /*
         * gghjt 60s 超时返回路径：wrapper 的定时器内联分发跳转到堆上的
         * 定时器/链表节点（如 0x738B58 含 FB FF FF FF …）而非代码。
         *
         * 正常流程中 wrapper 在分发前会先完成 dump0 读取（0x96000 字节
         * → 0x646100），还原游戏内存后分发到的回调指向有效游戏代码。
         * 超时路径中 dump0 虽已打开但大块读取尚未发生就崩溃了。
         *
         * 修复：从宿主侧完成 dump0 读取 + RW 区清理（与正常路径中
         * table[14] memset(0x645C70,0,0x80) / memset(0x645CF0,0,0x20) 等
         * 效），然后终止本次 ARM 执行。下一次定时器触发时 wrapper 会
         * 基于已恢复的游戏状态分发到有效游戏代码，继续加载 pak 资源，
         * 实现「返回游戏菜单」而非重启。
         */
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_HEAP_ADDR && pc < EXT_BASE_ADDR + EXT_MEM_SIZE) {
            uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
            /* LR 在 wrapper 代码范围内，说明是 wrapper 内联分发到了堆上
             * 的无效回调地址——netpay 超时返回的标志性崩溃模式 */
            int lr_in_wrapper = (lr & ~1u) >= EXT_CODE_ADDR &&
                                (lr & ~1u) < EXT_CODE_ADDR + m->code_len;
            uint8_t *pcp = (uint8_t *)arm_ptr(m, pc);
            if (pcp && ((pcp[0] == 0xFF && pcp[1] == 0xFF) ||
                       (pcp[0] == 0x00 && pcp[1] == 0x00 && pcp[2] == 0x00 && pcp[3] == 0x00) ||
                       lr_in_wrapper)) {
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: plugin returned to heap data pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                    dump_pc_ring(m);
                }
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                return MR_SUCCESS;
            }
        }
        /*
         * Some plug-in callbacks (e.g. the netpay timer event) corrupt
         * their own stack in our emulator, popping a return address that
         * lands inside ARM ext data (commonly a copy of the mrp pathname).
         * Detect that pattern -- PC sitting on the stack page near SP --
         * and treat it as a clean stop so the caller can carry on; we'd
         * otherwise just spam an Unhandled CPU exception line every tick.
         */
        if (err == UC_ERR_EXCEPTION) {
            uint32_t sp_diff = (pc > cur_sp) ? pc - cur_sp : cur_sp - pc;
            if (sp_diff < 0x4000) {
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: stack-corrupt plugin callback pc=0x%X sp=0x%X (treated as clean exit)\n", pc, cur_sp);
                    dump_pc_ring(m);
                }
                reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                return MR_SUCCESS;
            }
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

/* ---- MRP 文件缓存实现 ---- */

static uint32_t mrp_rd32le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static const char *path_basename(const char *path) {
    const char *p = strrchr(path, '/');
    const char *q = strrchr(path, '\\');
    if (q && (!p || q > p)) p = q;
    return p ? p + 1 : path;
}

/* 解析 MRP 并缓存所有条目（解压 gzip） */
static void parse_mrp_cache(ArmExtModule *m, const char *mrp_path) {
    m->mrp_cache_count = 0;
    if (!mrp_path || !*mrp_path) return;

    FILE *fp = fopen(mrp_path, "rb");
    if (!fp) return;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 240) { fclose(fp); return; }

    uint8_t *raw = (uint8_t *)malloc((size_t)sz);
    if (!raw) { fclose(fp); return; }
    if (fread(raw, 1, (size_t)sz, fp) != (size_t)sz) { free(raw); fclose(fp); return; }
    fclose(fp);

    if (memcmp(raw, "MRPG", 4) != 0) { free(raw); return; }

    uint32_t pos = 240;
    while (pos + 16 <= (uint32_t)sz && m->mrp_cache_count < MRP_CACHE_MAX) {
        uint32_t name_len = mrp_rd32le(raw + pos);
        pos += 4;
        if (name_len == 0 || name_len > 255 || pos + name_len + 12 > (uint32_t)sz) break;
        char entry_name[256];
        uint32_t copy_len = name_len < sizeof(entry_name) - 1 ? name_len : sizeof(entry_name) - 1;
        memcpy(entry_name, raw + pos, copy_len);
        entry_name[copy_len] = '\0';
        /* 去除尾部 null（MRP 条目名含 null 终止符算在 name_len 中） */
        if (copy_len > 0 && entry_name[copy_len - 1] == '\0') copy_len--;
        pos += name_len;
        uint32_t off = mrp_rd32le(raw + pos);
        uint32_t packed_len = mrp_rd32le(raw + pos + 4);
        pos += 12;
        if (off >= (uint32_t)sz || packed_len > (uint32_t)sz - off) continue;

        MrpCacheEntry *e = &m->mrp_cache[m->mrp_cache_count];
        strncpy(e->name, entry_name, sizeof(e->name) - 1);
        e->name[sizeof(e->name) - 1] = '\0';

        /* gzip 压缩的条目在宿主侧解压 */
        if (packed_len >= 2 && raw[off] == 0x1f && raw[off + 1] == 0x8b) {
            z_stream zs;
            memset(&zs, 0, sizeof(zs));
            if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) continue;
            uint32_t cap = packed_len * 4 + 4096;
            uint8_t *out = (uint8_t *)malloc(cap);
            if (!out) { inflateEnd(&zs); continue; }
            zs.next_in = raw + off;
            zs.avail_in = packed_len;
            int zret;
            do {
                if (zs.total_out == cap) {
                    uint32_t new_cap = cap * 2;
                    uint8_t *nb = (uint8_t *)realloc(out, new_cap);
                    if (!nb) { free(out); out = NULL; break; }
                    out = nb;
                    cap = new_cap;
                }
                zs.next_out = out + zs.total_out;
                zs.avail_out = cap - (uint32_t)zs.total_out;
                zret = inflate(&zs, Z_NO_FLUSH);
            } while (zret == Z_OK);
            if (!out || zret != Z_STREAM_END) { free(out); inflateEnd(&zs); continue; }
            e->data = out;
            e->data_len = (uint32_t)zs.total_out;
            inflateEnd(&zs);
        } else {
            e->data = (uint8_t *)malloc(packed_len);
            if (!e->data) continue;
            memcpy(e->data, raw + off, packed_len);
            e->data_len = packed_len;
        }
        m->mrp_cache_count++;
    }
    free(raw);
}

/* 按 basename 在缓存中查找条目 */
static MrpCacheEntry *mrp_cache_find(ArmExtModule *m, const char *filename) {
    const char *base = path_basename(filename);
    for (int i = 0; i < m->mrp_cache_count; i++) {
        if (strcmp(path_basename(m->mrp_cache[i].name), base) == 0)
            return &m->mrp_cache[i];
    }
    return NULL;
}

/* 分配一个虚拟 fd 并绑定到指定数据 */
static uint32_t mrp_vfd_open(ArmExtModule *m, const uint8_t *data, uint32_t len) {
    for (int i = 0; i < MRP_VFD_MAX; i++) {
        if (!m->mrp_vfds[i].in_use) {
            m->mrp_vfds[i].in_use = 1;
            m->mrp_vfds[i].data = data;
            m->mrp_vfds[i].data_len = len;
            m->mrp_vfds[i].pos = 0;
            return MRP_VFD_BASE + (uint32_t)i + 1;
        }
    }
    return 0;
}

static MrpVirtualFd *mrp_vfd_get(ArmExtModule *m, uint32_t fd) {
    if (fd <= MRP_VFD_BASE || fd > MRP_VFD_BASE + MRP_VFD_MAX) return NULL;
    MrpVirtualFd *v = &m->mrp_vfds[fd - MRP_VFD_BASE - 1];
    return v->in_use ? v : NULL;
}

static void mrp_cache_free(ArmExtModule *m) {
    for (int i = 0; i < m->mrp_cache_count; i++) {
        free(m->mrp_cache[i].data);
        m->mrp_cache[i].data = NULL;
    }
    m->mrp_cache_count = 0;
}

/* ---- 结束 MRP 文件缓存 ---- */

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

/* origin_mem 池由 ext 自身的 ARM 内存管理器维护，ext 直接更新
 * table[111/135/136] 对应的 slot 值。宿主侧不应覆写这些 slot，
 * 否则 ext 读到的 origin_mem_left 等统计会与实际不符。
 * 这里仅维护 m->origin_mem_left 用于其他宿主逻辑（如 arm_alloc
 * 的上限检查），不再调用 sync_origin_mem_stats。 */
static void note_origin_mem_alloc(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    len = align4(len ? len : 1);
    m->origin_mem_left = len < m->origin_mem_left ? m->origin_mem_left - len : 0;
    if (m->origin_mem_left < m->origin_mem_min) {
        m->origin_mem_min = m->origin_mem_left;
        m->origin_mem_top = m->origin_mem_len - m->origin_mem_min;
    }
}

static void note_origin_mem_free(ArmExtModule *m, uint32_t len) {
    if (!m->origin_mem_addr) return;
    m->origin_mem_left += align4(len);
    if (m->origin_mem_left > m->origin_mem_len) m->origin_mem_left = m->origin_mem_len;
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
    /* 场景 a：未经重定位的 ext 通过低地址调用 table 函数。
     * 不做真实分发（参数可能是给嵌套 ext 函数而非 table 函数的），
     * 安全地返回 0（NULL/失败），让调用方走错误处理路径。 */
    if (address < EXT_TABLE_COUNT * 4) {
        reg_write32(uc, UC_ARM_REG_R0, 0);
    }
    /* 场景 b：超出 table 范围 → return to LR */
    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    if (lr) {
        set_arm_mode_for_addr(m, lr);
        reg_write32(uc, UC_ARM_REG_PC, lr);
    } else {
        reg_write32(uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
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
    /* mpc 退出路径的忙等循环交替调用 table[33](getTime)、table[31](timerStart)、
     * table[32](timerStop)；若后两者重置 busy_wait_count，单次回调内忙等检测
     * 永远达不到阈值，导致 ARM 执行无法被打断、回调永不返回。 */
    if (idx != 33 && idx != 31 && idx != 32) {
        m->busy_wait_count = 0;
        m->busy_wait_start_ms = 0;
    }
    /* timerStart/timerStop 不算 work——同上，退出自旋每轮穿插它们。 */
    if (idx != 33 && idx != 0 && idx != 1 && idx != 3 && idx != 31 && idx != 32 && idx != 132 && idx != 133 && idx != 134) {
        m->event_did_work = 1;
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
        case 3:
            memcpy(arm_ptr(m, r0), arm_ptr(m, r1), r2);
            /* memcpy 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t cpy_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= cpy_end) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; break;
        case 4: memmove(arm_ptr(m, r0), arm_ptr(m, r1), r2); ret = r0; break;
        case 5: strcpy(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 6: strncpy(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 7: strcat(arm_ptr(m, r0), arm_str(m, r1)); ret = r0; break;
        case 8: strncat(arm_ptr(m, r0), arm_str(m, r1), r2); ret = r0; break;
        case 9: ret = memcmp(arm_ptr(m, r0), arm_ptr(m, r1), r2); break;
        case 10: ret = strcmp(arm_str(m, r0), arm_str(m, r1)); break;
        case 11: ret = strncmp(arm_str(m, r0), arm_str(m, r1), r2); break;
        case 13: { void *p = memchr(arm_ptr(m, r0), (int)r1, r2); ret = p ? arm_addr(m, p) : 0; } break;
        case 14:
            memset(arm_ptr(m, r0), (int)r1, r2);
            /* memset 后修复 GOT 中的 bridge 指针 */
            if (m->got_snapshot_base) {
                uint32_t got_base = m->got_snapshot_base;
                uint32_t set_end = r0 + r2;
                for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                    uint32_t addr = got_base + i * 4;
                    if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                        m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                        addr >= r0 && addr + 4 <= set_end) {
                        memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                    }
                }
            }
            ret = r0; break;
        case 12: ret = strcoll(arm_str(m, r0), arm_str(m, r1)); break;
        case 15: ret = strlen(arm_str(m, r0)); break;
        case 16: {
            char *hay = (char *)arm_ptr(m, r0);
            char *nee = (char *)arm_ptr(m, r1);
            if (!hay || !nee) { ret = 0; break; }
            char *p = strstr(hay, nee);
            ret = p ? arm_addr(m, p) : 0;
        } break;
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
                uint32_t wrapper_rw = 0;
                memcpy(&wrapper_rw, arm_ptr(m, m->p_addr), 4);
                if (wrapper_rw) {
                    uint32_t callback_slot = wrapper_rw + 0x1A0u;
                    if (arm_ptr(m, callback_slot))
                        memcpy(arm_ptr(m, callback_slot), &r0, 4);
                }
                m->active_helper_addr = r0;
                m->active_p_addr = p_addr;
                if (!m->primary_helper_addr) {
                    m->primary_helper_addr = r0;
                    m->primary_p_addr = p_addr;
                    m->primary_file_addr = m->last_file_addr;
                } else if (m->primary_file_addr) {
                    memcpy(arm_ptr(m, m->primary_file_addr + 4), &p_addr, 4);
                }
                if (getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: nested helper=0x%X P=0x%X len=%u primary=0x%X/0x%X\n",
                           r0, p_addr, r1, m->primary_helper_addr, m->primary_p_addr);
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
            // 根据字体实际尺寸计算位图大小：gb12=18字节(12行×12位紧凑), gb16=32字节(16行×2字节)
            int bitmap_size = ((width * height) + 7) >> 3;
            if (r2 && arm_ptr(m, r2)) memcpy(arm_ptr(m, r2), &width, 4);
            if (r3 && arm_ptr(m, r3)) memcpy(arm_ptr(m, r3), &height, 4);
            if (bitmap) {
                if (!m->char_bitmap_addr) m->char_bitmap_addr = arm_alloc(m, 32);
                if (m->char_bitmap_addr) {
                    memcpy(arm_ptr(m, m->char_bitmap_addr), bitmap, bitmap_size);
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
            mr_timer_state = 1;
            break;
        case 32:
            ret = mr_timerStop();
            internal_slot_write(m, m->mr_timer_state_slot, 0);
            mr_timer_state = 0;
            break;
        case 33: {
            ret = mr_getTime();
            /*
             * 部分 .mrp（如 mpc.mrp 的菜单切换、退出动画）会进入忙等循环，反复
             * 调用 mr_getTime() 等待真实时间流逝。在真机上 OS 抢占允许定时器/
             * 事件穿插进来；在本模拟器里若不让出 CPU，宿主线程的时钟也无法及
             * 时推进，导致这种合法的"实时延时"看上去像死锁。
             *
             * 策略分两段：
             *  1) 一旦连续 200 次 mr_getTime 调用都没有夹杂其它 table 调用，
             *     就 usleep 5ms 让宿主真实时间推进。这样大多数 UI 动画 /
             *     短延时循环会因 mr_getTime() 返回值变化而自然退出。
             *  2) 如果忙等已经持续了一个"明显超出任何合理 UI 动画"的真实
             *     时长（这里取 3s），说明循环并不是单纯等时间——很可能是
             *     mpc 退出流程里的最终自旋，对应代码本应在循环结束后调用
             *     mr_exit。但我们的仿真器在那之前往往会因为栈漂移而崩溃，
             *     根本到不了 mr_exit。这种情况下主动调用 mr_exit() 帮它把
             *     进程收尾，复刻早期 a5ca8ab/6f0ca40 修复"无法退出 mpc"的
             *     效果，同时阈值足够大不会误伤正常的菜单切换动画（实测约
             *     1~1.5s 即可完成）。
             */
            m->busy_wait_count++;
            if (m->busy_wait_count == 1) {
                m->busy_wait_start_ms = ret;
            }
            if (m->busy_wait_count >= 200) {
                usleep(5 * 1000);
                m->busy_wait_count = 100;
                /* ret 是刚拿到的 mr_getTime() 返回值，单位 ms。 */
                uint32_t elapsed_ms = ret - m->busy_wait_start_ms;
                if (elapsed_ms >= 3000) {
                    m->busy_wait_count = 0;
                    m->busy_wait_start_ms = 0;
                    if (m->event_did_work) {
                        /* ext 做了实质性 work（加载资源/绘制等）后进入忙等，
                         * 说明它在等定时器驱动主循环。强制停止 ARM 执行，
                         * 让调用方有机会启动定时器再重新进入。 */
                        reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                        uc_emu_stop(m->uc);
                    } else {
                        printf("[arm_ext_executor] mr_getTime busy-wait exceeded %ums, assuming app is exiting -> mr_exit\n",
                               elapsed_ms);
                        reg_write32(m->uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
                        uc_emu_stop(m->uc);
                        mr_exit();
                    }
                }
            }
        } break;
        case 34: ret = mr_getDatetime(arm_ptr(m, r0)); break;
        case 35: ret = mr_getUserInfo(arm_ptr(m, r0)); break;
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
        case 40: {
            const char *open_name = arm_str(m, r0);
            ret = mr_open(open_name, r1);
            /* 磁盘上找不到时尝试从 MRP 缓存提供虚拟 fd */
            if (ret == 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, open_name);
                if (ce) {
                    ret = mrp_vfd_open(m, ce->data, ce->data_len);
                }
            }
        } break;
        case 41: {
            MrpVirtualFd *vf = mrp_vfd_get(m, r0);
            if (vf) { vf->in_use = 0; ret = MR_SUCCESS; }
            else ret = mr_close((int32)r0);
        } break;
        case 42: {
            const char *info_name = arm_str(m, r0);
            ret = mr_info(info_name);
            /* 磁盘上不存在时检查 MRP 缓存 */
            if (ret != MRP_IS_FILE && m->mrp_cache_count > 0) {
                if (mrp_cache_find(m, info_name))
                    ret = MRP_IS_FILE;
            }
        } break;
        case 43: {
            void *src = arm_ptr(m, r1);
            uint32_t len = r2;
            /*
             * Workaround for gghjt.mrp netpay extraction.
             *
             * netpay walks its mrp index, reads each chk?.xchk entry, then
             * writes the decompressed payload to mythroad/gghjt/gghjt.pak via
             * the fixed scratch buffer at ARM addr 0x2001BC (length 0xBE).
             * On a real handset its inline inflate runs for every entry; in
             * our Unicorn emulation the inflate only succeeds on the first
             * iteration (res.list, 90 -> 190 bytes) and stays stale for the
             * chk?.xchk entries, so the script writes the same 190 bytes
             * forever and loops re-extracting.
             *
             * Detect the symptom -- writing 0xBE bytes from 0x2001BC right
             * after a large gzip-headered read -- and substitute the
             * decompressed payload we captured in case 44 below. We return
             * the caller-requested byte count so netpay's loop bookkeeping
             * advances by one record instead of treating the extra bytes as
             * multiple writes.
             */
            uint8_t *consumed_decomp = NULL;
            int substituted = 0;
            if (r1 == 0x2001BCu && r2 == 0xBEu && m->pending_chk_len > 14000u) {
                size_t out_cap = (size_t)m->pending_chk_decomp_len;
                if (m->pending_chk_decomp && out_cap > 0) {
                    src = m->pending_chk_decomp;
                    len = (uint32_t)out_cap;
                    substituted = 1;
                }
                /* Hand ownership of the buffer to this call and only free
                 * after mr_write consumes it. Single-shot: a subsequent
                 * unrelated write goes through unchanged unless a fresh chk
                 * read repopulates the buffer. */
                consumed_decomp = m->pending_chk_decomp;
                m->pending_chk_decomp = NULL;
                m->pending_chk_decomp_len = 0;
                m->pending_chk_len = 0;
            }
            ret = mr_write((int32)r0, src, len);
            free(consumed_decomp);
            if (substituted && ret == (int32_t)len) ret = (int32_t)r2;
        } break;
        case 44: {
            void *hp = arm_ptr(m, r1);
            MrpVirtualFd *vf44 = mrp_vfd_get(m, r0);
            if (vf44) {
                /* 从 MRP 缓存读取 */
                uint32_t avail = vf44->pos < vf44->data_len ? vf44->data_len - vf44->pos : 0;
                uint32_t n = r2 < avail ? r2 : avail;
                if (n && hp) memcpy(hp, vf44->data + vf44->pos, n);
                vf44->pos += n;
                ret = (int32_t)n;
            } else {
                ret = mr_read((int32)r0, hp, r2);
            }
            /* See case 43 for the netpay extraction workaround. Track every
             * sizable read that looks like a gzip-compressed chunk so we can
             * substitute the decompressed payload at write time. */
            if ((int32_t)ret > 256 && hp) {
                const unsigned char *bp = (const unsigned char *)hp;
                if (bp[0] == 0x1f && bp[1] == 0x8b) {
                    m->pending_chk_arm_buf = r1;
                    m->pending_chk_len = r2;
                    free(m->pending_chk_decomp);
                    m->pending_chk_decomp = NULL;
                    m->pending_chk_decomp_len = 0;
                    /* Decompress with raw zlib (gzip header) to a host buffer
                     * sized generously; netpay records exceed 64KB. */
                    z_stream zs = {0};
                    zs.next_in = (Bytef *)bp;
                    zs.avail_in = (uInt)ret;
                    if (inflateInit2(&zs, 16 + MAX_WBITS) == Z_OK) {
                        size_t cap = (size_t)ret * 4 + 4096;
                        for (int tries = 0; tries < 6; ++tries) {
                            uint8_t *out = (uint8_t *)malloc(cap);
                            if (!out) break;
                            zs.next_out = out;
                            zs.avail_out = (uInt)cap;
                            int z = inflate(&zs, Z_FINISH);
                            if (z == Z_STREAM_END) {
                                m->pending_chk_decomp = out;
                                m->pending_chk_decomp_len = (uint32_t)zs.total_out;
                                break;
                            } else if (z == Z_BUF_ERROR) {
                                free(out);
                                inflateReset2(&zs, 16 + MAX_WBITS);
                                zs.next_in = (Bytef *)bp;
                                zs.avail_in = (uInt)ret;
                                cap *= 2;
                                continue;
                            } else {
                                free(out);
                                break;
                            }
                        }
                        inflateEnd(&zs);
                    }
                }
            }
            /* dump/restore 读回整块模块内存后，需要：
             * 1) 修复 GOT 中的 bridge 函数指针（文件数据中是原始未重定位的值）
             * 2) invalidate Unicorn TB cache（否则执行旧翻译） */
            if ((int32_t)ret > 0 && (uint32_t)ret > 0x1000) {
                /* 仅在 dump0 恢复时（目标地址匹配）才恢复间隙数据 */
                uc_ctl_remove_cache(m->uc, r1, r1 + (uint32_t)ret);
                /* 修复 GOT：只恢复已记录的 bridge 函数指针 */
                if (m->got_snapshot_base) {
                    uint32_t got_base = m->got_snapshot_base;
                    uint32_t read_end = r1 + (uint32_t)ret;
                    for (uint32_t i = 0; i < EXT_TABLE_COUNT; i++) {
                        uint32_t addr = got_base + i * 4;
                        if (m->got_snapshot[i] >= EXT_TABLE_ADDR &&
                            m->got_snapshot[i] < EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4 &&
                            addr >= r1 && addr + 4 <= read_end) {
                            memcpy(arm_ptr(m, addr), &m->got_snapshot[i], 4);
                        }
                    }
                }
            }
        } break;
        case 45: {
            MrpVirtualFd *vf45 = mrp_vfd_get(m, r0);
            if (vf45) {
                /* SEEK_SET=0, SEEK_CUR=1, SEEK_END=2 */
                int32_t new_pos;
                if ((int)r2 == 0) new_pos = (int32_t)r1;
                else if ((int)r2 == 1) new_pos = (int32_t)vf45->pos + (int32_t)r1;
                else new_pos = (int32_t)vf45->data_len + (int32_t)r1;
                if (new_pos < 0) new_pos = 0;
                if ((uint32_t)new_pos > vf45->data_len) new_pos = (int32_t)vf45->data_len;
                vf45->pos = (uint32_t)new_pos;
                ret = MR_SUCCESS;
            } else {
                ret = mr_seek((int32)r0, (int32)r1, (int)r2);
            }
        } break;
        case 46: {
            ret = mr_getLen(arm_str(m, r0));
            if (ret < 0 && m->mrp_cache_count > 0) {
                MrpCacheEntry *ce = mrp_cache_find(m, arm_str(m, r0));
                if (ce) ret = (int32_t)ce->data_len;
            }
        } break;
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
        /* table[59] mr_sendSms(pNumber, pContent, encode) */
        case 59: ret = mr_sendSms(arm_str(m, r0), arm_str(m, r1), (int32)r2); break;
        case 60: mr_call(arm_str(m, r0)); ret = MR_SUCCESS; break;
        /* table[61] mr_getNetworkID()：返回 0 表示移动网络（MR_NET_ID_MOBILE） */
        case 61: ret = 0; break;
        /* table[81] mr_initNetwork(cb, mode)：同步返回成功，与 reference
         * 实现（temp/jni/src/network.c）一致。桌面端没有真实网络，但 ARM
         * 代码只关心初始化是否成功以决定后续流程 */
        case 81: ret = MR_SUCCESS; break;
        /* table[82] mr_closeNetwork()：return success，跟 mr_initNetwork 配对。 */
        case 82: ret = MR_SUCCESS; break;
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
        /* table[78] mr_winCreate / table[79] mr_winRelease: 窗口功能不支持 */
        case 78: ret = MR_IGNORE; break;
        case 79: ret = MR_IGNORE; break;
        case 80: ret = mr_getScreenInfo(arm_ptr(m, r0)); break;
        case 113: {
            void *p = arm_ptr(m, r0);
            if (p) mr_md5_init(p);
            ret = 0;
        } break;
        case 114: {
            void *p = arm_ptr(m, r0);
            void *d = arm_ptr(m, r1);
            if (p && d) mr_md5_append(p, d, (int)r2);
            ret = 0;
        } break;
        case 115: {
            void *p = arm_ptr(m, r0);
            void *digest = arm_ptr(m, r1);
            if (p && digest) mr_md5_finish(p, digest);
            ret = 0;
        } break;
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
            /*
             * 关键修复：case 131 (r1=9) 是 mr_cacheSync。原意是同步指令缓存，
             * 因为 wrapper 刚把新的 .ext 二进制写到了 [r2, r2+r3) 范围。在真机
             * 上这会清掉 D-cache 并 invalidate I-cache，让后续取指看到新指令。
             *
             * 我们用 Unicorn 跑 ARM，Unicorn 通过 QEMU TCG 做 JIT 翻译并 cache
             * translation blocks（TB）。当 gghjt 把 netpay 的 smcheck/advsms
             * 等子插件 staging 到同一段内存时，TB cache 仍然是上一个插件留下的
             * 翻译结果，结果就是 PC 进了新 .ext 的代码区，执行的却是旧代码。
             *
             * 表现为 r3 不被 LDR 重新加载、movs r1/r2 立即数被跳过等"鬼指令"，
             * 最后 BLX r3 跳到一个看着像数据/字符串的地址，崩在 0x627A60 一类
             * 位置。修复方式就是 mimic 真机的 mr_cacheSync：在 staging 完成后
             * 通过 uc_ctl_remove_cache 让 Unicorn 把这段地址的 TB 翻译丢掉，
             * 下次执行强制重新翻译现在的字节。
             */
            if (r1 == 9 && r2 && r3 > 0 && arm_ptr(m, r2) && arm_ptr(m, r2 + r3 - 1)) {
                uc_err cerr = uc_ctl_remove_cache(m->uc, r2, r2 + r3);
                if (cerr != UC_ERR_OK && getenv("VMRP_ARM_EXT_TRACE")) {
                    printf("arm_ext_executor: uc_ctl_remove_cache(0x%X, 0x%X) failed: %u\n",
                           r2, r2 + r3, cerr);
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
    ArmExtModule *m = (ArmExtModule *)user_data;
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
    m->screen_dirty = 1;
    /* 直接写屏幕缓冲区（如 nes 游戏的逐像素渲染）也算"实质性 work"，
     * 否则退出检测会把纯 ARM 渲染的游戏误判为退出自旋。 */
    m->event_did_work = 1;
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

    int32_t sw = mr_screen_w > 0 ? mr_screen_w : vmrp_config.screen_width;
    int32_t sh = mr_screen_h > 0 ? mr_screen_h : vmrp_config.screen_height;
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
        uint32_t free_next = m->origin_mem_len;
        uint32_t free_len = m->origin_mem_len;
        memcpy(arm_ptr(m, m->origin_mem_addr), &free_next, 4);
        memcpy((uint8_t *)arm_ptr(m, m->origin_mem_addr) + 4, &free_len, 4);

        if (slot108) { uint32_t v = m->origin_mem_addr; memcpy(arm_ptr(m, slot108), &v, 4); }
        if (slot109) { uint32_t v = m->origin_mem_len; memcpy(arm_ptr(m, slot109), &v, 4); }
        if (slot110) { uint32_t v = m->origin_mem_addr + m->origin_mem_len; memcpy(arm_ptr(m, slot110), &v, 4); }
        if (slot111) { uint32_t v = m->origin_mem_left; memcpy(arm_ptr(m, slot111), &v, 4); }
        if (slot135) { uint32_t v = m->origin_mem_min; memcpy(arm_ptr(m, slot135), &v, 4); }
        if (slot136) { uint32_t v = m->origin_mem_top; memcpy(arm_ptr(m, slot136), &v, 4); }
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
    m->mem = mmap((void *)(uintptr_t)EXT_BASE_ADDR, EXT_MEM_SIZE,
                  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m->mem == MAP_FAILED) {
        m->mem = calloc(1, EXT_MEM_SIZE);
        m->mem_is_mmap = 0;
    } else {
        m->mem_is_mmap = 1;
        memset(m->mem, 0, EXT_MEM_SIZE);
    }
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
    /* 预解析 MRP，缓存所有条目，避免 ARM ext 做极慢的 MRP 索引扫描 */
    parse_mrp_cache(m, mr_get_pack_filename());
    memcpy(m->low_table, m->mem, EXT_TABLE_COUNT * 4);
    err = uc_mem_map_ptr(m->uc, 0, EXT_LOW_TABLE_SIZE, UC_PROT_ALL, m->low_table);
    if (err != UC_ERR_OK) goto fail;
    void *code_dst = arm_ptr(m, EXT_CODE_ADDR);
    if (!code_dst) goto fail;
    memcpy(code_dst, code, len);
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
    /* GOT bridge 指针追踪：ARM 代码向 R9 数据区写入 bridge 地址时记录，
     * 后续 dump/restore 覆盖时自动修复 */
    {
        uc_hook got_hook;
        uc_hook_add(m->uc, &got_hook, UC_HOOK_MEM_WRITE, hook_got_write, m,
                    EXT_HEAP_ADDR, EXT_BASE_ADDR + EXT_MEM_SIZE - 1);
    }

    reg_write32(m->uc, UC_ARM_REG_R0, (uint32_t)load_code);
    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int load_ret = run_arm(m, EXT_CODE_ADDR + 8);
    leave_screen_context(m, saved_screenBuf);
    if (load_ret != MR_SUCCESS) goto fail;
    /* 嵌套 ext 可能在 mr_c_function_load 期间就被加载（如 dota.mrp 的
     * cfunction.ext 在 load 阶段就加载 game.ext），此时 table[31/32]
     * 已被覆盖为 dispatch 地址。提前捕获并恢复为 hook。 */
    if (m->primary_helper_addr && m->primary_helper_addr != m->helper_addr) {
        uint32_t t31v = 0, t32v = 0;
        memcpy(&t31v, arm_ptr(m, EXT_TABLE_ADDR + 31 * 4), 4);
        memcpy(&t32v, arm_ptr(m, EXT_TABLE_ADDR + 32 * 4), 4);
        if (t31v != EXT_TABLE_ADDR + 31 * 4) {
            uint32_t hook = EXT_TABLE_ADDR + 31 * 4;
            memcpy(arm_ptr(m, EXT_TABLE_ADDR + 31 * 4), &hook, 4);
        }
        if (t32v != EXT_TABLE_ADDR + 32 * 4) {
            uint32_t hook = EXT_TABLE_ADDR + 32 * 4;
            memcpy(arm_ptr(m, EXT_TABLE_ADDR + 32 * 4), &hook, 4);
        }
    }
    /* 将 ARM 代码 mr_c_function_load() 的返回值 (R0) 传给调用者，
     * 与非 native 路径中 mr_load_c_function(code) 的返回值语义一致 */
    if (ext_ret) *ext_ret = (int32_t)reg_read32(m->uc, UC_ARM_REG_R0);
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
    m->event_did_work = 0;
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

    int patch_primary_table = call_helper_addr == m->primary_helper_addr &&
                              m->primary_helper_addr != m->helper_addr;
    uint32_t table31_addr = EXT_TABLE_ADDR + 31 * 4;
    uint32_t table32_addr = EXT_TABLE_ADDR + 32 * 4;
    uint32_t saved_t31 = 0, saved_t32 = 0;
    if (patch_primary_table) {
        /* nested ext 会覆盖 table[31/32]，调用 primary helper 时临时恢复 hook 以保持正确 R9/context。 */
        memcpy(&saved_t31, arm_ptr(m, table31_addr), 4);
        memcpy(&saved_t32, arm_ptr(m, table32_addr), 4);
        memcpy(arm_ptr(m, table31_addr), &table31_addr, 4);
        memcpy(arm_ptr(m, table32_addr), &table32_addr, 4);
    }

    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int call_ret = run_arm_with_sp(m, call_helper_addr, sp);
    leave_screen_context(m, saved_screenBuf);

    if (patch_primary_table) {
        memcpy(arm_ptr(m, table31_addr), &saved_t31, 4);
        memcpy(arm_ptr(m, table32_addr), &saved_t32, 4);
    }

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

uint32 arm_ext_primary_helper(ArmExtModule *m) {
    if (!m) return 0;
    return (m->primary_helper_addr && m->primary_helper_addr != m->helper_addr)
           ? m->primary_helper_addr : 0;
}

uint32 arm_ext_read_timer_queue(ArmExtModule *m) {
    if (!m || !m->primary_p_addr) return 0;
    uint32_t rw = 0;
    memcpy(&rw, arm_ptr(m, m->primary_p_addr), 4);
    if (!rw) return 0;
    uint32_t val = 0;
    if (arm_ptr(m, rw + 0x8C))
        memcpy(&val, arm_ptr(m, rw + 0x8C), 4);
    return val;
}


int arm_ext_call_dispatch(ArmExtModule *m, int is_stop, uint32_t timer_interval) {
    if (!m || !m->p_addr) return MR_FAILED;
    uint32_t nested_p = m->primary_p_addr ? m->primary_p_addr : 0;
    if (!nested_p && m->last_file_addr) {
        void *p4 = arm_ptr(m, m->last_file_addr + 4);
        if (p4) memcpy(&nested_p, p4, 4);
    }
    if (!nested_p || !arm_ptr(m, nested_p + 12)) return MR_FAILED;
    /* 读取 P->mrc_extChunk (offset 12) */
    uint32_t ext_chunk = 0;
    memcpy(&ext_chunk, arm_ptr(m, nested_p + 12), 4);
    if (!ext_chunk || !arm_ptr(m, ext_chunk + 0x28)) return MR_FAILED;
    /* 读取目标函数 extChunk[0x28] 和参数 extChunk[0x24] */
    uint32_t func = 0, param = 0;
    memcpy(&func, arm_ptr(m, ext_chunk + 0x28), 4);
    memcpy(&param, arm_ptr(m, ext_chunk + 0x24), 4);
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
    uint16 *saved_screenBuf = NULL;
    enter_screen_context(m, &saved_screenBuf);
    int ret = run_arm_with_sp(m, func, sp);
    leave_screen_context(m, saved_screenBuf);

    /* wrapper 的 dispatch 函数执行后检查 ext_chunk[0x34]（退出标志位）。
     * 游戏在确认退出的事件处理中设置此字段为非零值；wrapper 的 ARM 代码
     * 检测到后会移除定时器节点，但不会主动调用 mr_exit。由宿主侧补上。 */
    {
        uint32_t exit_flag = 0;
        if (arm_ptr(m, ext_chunk + 0x34))
            memcpy(&exit_flag, arm_ptr(m, ext_chunk + 0x34), 4);
        if (exit_flag) {
            mr_exit();
        }
    }

    return ret;
}

int arm_ext_invoke0(ArmExtModule *m, uint32 func, int32 *ret_out) {
    if (ret_out) *ret_out = MR_FAILED;
    if (!m || !func) return MR_FAILED;

    /* 每次进入 ARM 事件 / 定时器回调前清空"是否做过实质工作"的标志，便于
     * 在 SP 漂移崩溃时判断该回调走的是哪条路径（参见 ArmExtModule 注释）。 */
    m->event_did_work = 0;
    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
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

    /* 见 arm_ext_invoke0 中的同名注释。 */
    m->event_did_work = 0;
    m->busy_wait_count = 0;
    m->busy_wait_start_ms = 0;
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
    mrp_cache_free(m);
    if (m->uc) uc_close(m->uc);
    free(m->last_file_copy);
    free(m->low_table);
    if (m->mem_is_mmap)
        munmap(m->mem, EXT_MEM_SIZE);
    else
        free(m->mem);
    free(m);
}
