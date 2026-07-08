/*
 * aex_exec.c —— Unicorn 执行引擎入口与 guest 字符串/格式化(Phase 2 拆分)。
 *
 * run_arm(_with_sp)、中断入口的 PC 环形记录、arm_str/format_arm
 * (原生 mr_printf 转换符语义,见 memory/format-arm-native-semantics)。
 * 函数体自 arm_ext_executor.c 原样迁移。
 */
#include "../include/arm_ext_priv.h"
#include "../include/utils.h"

#include <ctype.h>

extern int32 mr_read(int32 f, void *p, uint32 l);

void set_arm_mode_for_addr(ArmExtModule *m, uint32_t addr) {
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


void trace_pc(uc_engine *uc, uint64_t address, uint32_t size, void *user_data) {
    (void)size;
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t pos = m->pc_ring_pos++ % EXT_TRACE_PC_RING;
    m->pc_ring[pos] = (uint32_t)address;
    m->cpsr_ring[pos] = reg_read32(uc, UC_ARM_REG_CPSR);
}

static void dump_pc_ring(ArmExtModule *m) {
    if (!arm_ext_trace_pc_on()) return;
    uint32_t total = m->pc_ring_pos < EXT_TRACE_PC_RING ? m->pc_ring_pos : EXT_TRACE_PC_RING;
    printf("arm_ext_executor: recent PCs:\n");
    for (uint32_t i = 0; i < total; ++i) {
        uint32_t idx = (m->pc_ring_pos - total + i) % EXT_TRACE_PC_RING;
        printf("  #%02u pc=0x%X cpsr=0x%X %s\n",
               i, m->pc_ring[idx], m->cpsr_ring[idx],
               (m->cpsr_ring[idx] & (1u << 5)) ? "thumb" : "arm");
    }
}

static void arm_ext_mark_unhandled_intr(ArmExtModule *m, uint32_t intno) {
    if (!m) return;
    m->pending_intr_no = intno;
    m->pending_intr_pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
    m->pending_intr_r0 = reg_read32(m->uc, UC_ARM_REG_R0);
    m->pending_intr_r1 = reg_read32(m->uc, UC_ARM_REG_R1);
}

void hook_intr(uc_engine *uc, uint32_t intno, void *user_data) {
    ArmExtModule *m = (ArmExtModule *)user_data;
    uint32_t pc = reg_read32(uc, UC_ARM_REG_PC) & ~1u;
    uint32_t r0 = reg_read32(uc, UC_ARM_REG_R0);
    uint32_t r1 = reg_read32(uc, UC_ARM_REG_R1);
    /*
     * ARM semihosting uses SVC #0xAB in Thumb state.  Vendor browser
     * components keep a tiny debug putchar stub in their runtime image:
     *   movs r0,#3; mov r1,sp; svc #0xAB
     * where operation 3 is SYS_WRITEC and r1 points at one byte.  Unicorn
     * reports the trap through UC_HOOK_INTR and does not emulate the ABI, so
     * consume only this documented print operation.  Other SVC/HVC traps are
     * preserved as real execution errors by stopping with pending_intr_* set.
     */
    if (intno == 2u && r0 == 3u && arm_ptr(m, r1)) {
        if (arm_ext_diag_on()) {
            static uint32_t semihost_diag_count = 0;
            if (semihost_diag_count < 64u) {
                unsigned ch = arm_ext_read_u8_or_zero_(m, r1);
                printf("DIAG semihost_writec pc=0x%X ch=0x%02X r1=0x%X\n",
                       pc, ch, r1);
            } else if (semihost_diag_count == 64u) {
                printf("DIAG semihost_writec truncated after 64 entries\n");
            }
            semihost_diag_count++;
        }
        return;
    }

    arm_ext_mark_unhandled_intr(m, intno);
    uc_emu_stop(uc);
}

uint32_t arg_read(ArmExtModule *m, unsigned n) {
    if (n < 4) return reg_read32(m->uc, UC_ARM_REG_R0 + n);
    uint32_t sp = reg_read32(m->uc, UC_ARM_REG_SP);
    uint32_t v = 0;
    uc_mem_read(m->uc, sp + (n - 4) * 4, &v, 4);
    return v;
}

int run_arm_with_sp(ArmExtModule *m, uint32_t start, uint32_t sp) {
    set_arm_mode_for_addr(m, start);
    m->pending_intr_no = 0;
    m->pending_intr_pc = 0;
    m->pending_intr_r0 = 0;
    m->pending_intr_r1 = 0;
    reg_write32(m->uc, UC_ARM_REG_SP, sp);
    reg_write32(m->uc, UC_ARM_REG_LR, EXT_STOP_ADDR);
    uc_err err = uc_emu_start(m->uc, arm_exec_addr(start), EXT_STOP_ADDR, 0, 0);
    if (err == UC_ERR_OK && m->pending_intr_no) {
        err = UC_ERR_EXCEPTION;
        reg_write32(m->uc, UC_ARM_REG_PC, m->pending_intr_pc);
    }
    /*
     * Some nested .mrp plug-ins (e.g. netpay.mrp loaded by gghjt.mrp) keep
     * function pointers without the Thumb bit, so a BLX into them lands in
     * ARM mode while the bytes are Thumb-2.  Unicorn reports that mismatch as
     * UC_ERR_INSN_INVALID for some Thumb instruction pairs and UC_ERR_EXCEPTION
     * for others, so do the same one-shot mode correction for both errors. If
     * the same PC still fails after being retried in Thumb mode, it is real
     * invalid code/data, not a missing Thumb bit.  gghjt's 60s timeout-return
     * path exposes this after dump0 restore: a copied timer node (e.g.
     * table[3] to 0x738B34) is data, and an unbounded retry here pins the UI on
     * “请稍等”.  Let the normal invalid-PC handling below stop the callback
     * cleanly instead of spinning forever.
     */
    if (err == UC_ERR_INSN_INVALID || err == UC_ERR_EXCEPTION) {
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC);
        if (pc != EXT_STOP_ADDR && pc != 0) {
            /* 只在 PC 位于已知可执行代码段时切 Thumb 重试。
             * 被踩的栈帧可能把 extChunk/P 等数据地址弹入 PC，该地址
             * 在 ARM 模式下解码失败后若盲目切 Thumb 重试，数据字节恰好
             * 构成有效 Thumb 指令(如 strb)就会写入未映射地址导致崩溃。
             * 只对 wrapper 代码段或已注册的子模块代码段做 Thumb 修正。 */
            int pc_in_code = (pc >= EXT_CODE_ADDR &&
                              pc < EXT_CODE_ADDR + m->code_len);
            if (!pc_in_code) {
                uint32_t _fa = 0, _fl = 0;
                pc_in_code = arm_ext_nested_exec_range_for_lr(m, pc, &_fa, &_fl);
            }
            uint32_t cpsr = reg_read32(m->uc, UC_ARM_REG_CPSR);
            if (pc_in_code && (cpsr & (1u << 5)) == 0) {
                cpsr |= (1u << 5);
                reg_write32(m->uc, UC_ARM_REG_CPSR, cpsr);
                if (arm_ext_trace_on()) {
                    printf("arm_ext_executor: retrying in Thumb mode at pc=0x%X\n", pc);
                }
                err = uc_emu_start(m->uc, pc | 1u, EXT_STOP_ADDR, 0, 0);
                if (err == UC_ERR_OK && m->pending_intr_no) {
                    err = UC_ERR_EXCEPTION;
                    reg_write32(m->uc, UC_ARM_REG_PC, m->pending_intr_pc);
                }
            }
        }
    }
    if (err != UC_ERR_OK) {
        if (reg_read32(m->uc, UC_ARM_REG_PC) == EXT_STOP_ADDR) return MR_SUCCESS;
        uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC) & ~1u;
        uint32_t cur_sp = reg_read32(m->uc, UC_ARM_REG_SP);
        if (m->pending_intr_no && arm_ext_diag_on()) {
            printf("DIAG unhandled_intr intno=%u pc=0x%X r0=0x%X r1=0x%X start=0x%X\n",
                   m->pending_intr_no, m->pending_intr_pc,
                   m->pending_intr_r0, m->pending_intr_r1, start);
        }
        if (err == UC_ERR_INSN_INVALID && pc >= EXT_CODE_ADDR && pc < EXT_CODE_ADDR + m->code_len) {
            /*
             * ARM 侧栈漂进了代码段后无法继续安全解释。这里只结束当前
             * callback，让平台层保持原状态继续调度；退出/重启必须来自
             * ARM 程序自己的 mr_exit/mr_state 路径，不能在异常处理里推断。
             */
            if (arm_ext_trace_on()) {
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
                    if (arm_ext_trace_on()) {
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
            /* wrapper 内部 loader BLX 到 game code 时 LR 自然指向 wrapper，
             * 不能把这种合法跳转当作无效回调。game code 在 pending file 范围内
             * 时排除 lr_in_wrapper 判定，让后续 "fallback to LR" 继续执行。 */
            if (lr_in_wrapper && m->pending_internal_file_addr &&
                pc >= m->pending_internal_file_addr &&
                pc < m->pending_internal_file_addr + m->pending_internal_file_len) {
                lr_in_wrapper = 0;
            }
            uint8_t *pcp = (uint8_t *)arm_ptr(m, pc);
            if (pcp && ((pcp[0] == 0xFF && pcp[1] == 0xFF) ||
                       (pcp[0] == 0x00 && pcp[1] == 0x00 && pcp[2] == 0x00 && pcp[3] == 0x00) ||
                       lr_in_wrapper)) {
                if (arm_ext_trace_on()) {
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
                if (arm_ext_trace_on()) {
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

int run_arm(ArmExtModule *m, uint32_t start) {
    return run_arm_with_sp(m, start, EXT_STACK_ADDR + EXT_STACK_SIZE);
}

void restore_ext_r9(ArmExtModule *m) {
    if (!m) return;
    uint32_t p_addr = m->active_p_addr ? m->active_p_addr : m->p_addr;
    if (!p_addr) return;
    uint32_t rw_base = 0;
    memcpy(&rw_base, arm_ptr(m, p_addr), 4);
    if (rw_base) reg_write32(m->uc, UC_ARM_REG_R9, rw_base);
}

char *arm_str(ArmExtModule *m, uint32_t addr) {
    char *p = (char *)arm_ptr(m, addr);
    /* 空串退路必须 ≥2 字节全零:_DrawText 等 UCS2 消费方按双字节读首字符,
     * 1 字节的 "" 字面量会越界读 1 字节(ASan global-buffer-overflow 实测
     * 命中,mythroad.c _DrawText)。返回值语义不变:窄串/宽串解释都是空串。 */
    static const char arm_str_empty[4] = {0, 0, 0, 0};
    return p ? p : (char *)arm_str_empty;
}


int format_arm(ArmExtModule *m, char *dst, size_t dst_size, const char *fmt, unsigned first_arg) {
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
        char tmp[512];
        switch (*p) {
            case 's': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), spec, arm_str(m, av));
            } break;
            case 'c': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), spec, (int)av);
            } break;
            case 'p': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), "0x%X", av);
            } break;
            case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': {
                uint32_t av = arg_read(m, ai++);
                snprintf(tmp, sizeof(tmp), spec, av);
            } break;
            default:
                /* 原生 sprintf(src/mythroad/printf.c 的 default 分支)对
                 * 未知转换符只输出该字符本身且【不消耗可变参数】。此前
                 * 这里把未知 spec 直接交给宿主 snprintf:glibc 会把 %m
                 * 展开成 strerror(errno)(真实视频驱动的 socket I/O 常置
                 * errno,展开成 33 字节长串),且多消耗一个参数使后续
                 * %d 读到错位的表槽值(0x10044→"65604")。talkcat 的
                 * "%m%d.jpg" 路径名因此从原生的 "m5.jpg" 级长度膨胀到
                 * 42 字节,溢出 guest 30 字节栈缓冲,覆盖相邻指针后在
                 * 定时器回调触发 UC_MEM_WRITE_UNMAPPED addr=0x3036353A
                 * (= 溢出文本 "6560"+4)。验证:真实视频驱动下启动
                 * talkcat 不再于 ~4.5s 崩溃,headless PPM 正常。 */
                tmp[0] = *p;
                tmp[1] = '\0';
                break;
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


