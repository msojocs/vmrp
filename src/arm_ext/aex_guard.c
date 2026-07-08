/*
 * aex_guard.c —— table return 守卫与 Thumb 指令解码助手(Phase 2 拆分)。
 *
 * 防御 table[n] 返回地址重入/栈帧欺骗(gzwdzjs-table-bridge-stale-epilogue
 * 一类历史 bug 的通用守卫)。函数体自 arm_ext_executor.c 原样迁移。
 */
#include "../include/arm_ext_priv.h"

static unsigned arm_ext_popcount8(uint32_t v) {
    v &= 0xffu;
    unsigned n = 0;
    while (v) {
        n += v & 1u;
        v >>= 1;
    }
    return n;
}

static int arm_ext_read_thumb16(ArmExtModule *m, uint32_t addr,
                                uint16_t *value) {
    if (value) *value = 0;
    if (!m || !value || !arm_ptr(m, addr) || !arm_ptr(m, addr + 1u))
        return 0;
    uint8_t *p = (uint8_t *)arm_ptr(m, addr);
    *value = (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
    return 1;
}

static int arm_ext_thumb_is_blx_reg(uint16_t hw) {
    return (hw & 0xff87u) == 0x4780u;
}

static int arm_ext_thumb_is_pop_pc(uint16_t hw) {
    return (hw & 0xfe00u) == 0xbc00u && (hw & 0x0100u) != 0;
}

static int arm_ext_table_return_site_is_guarded(ArmExtModule *m,
                                                uint32_t pc) {
    uint16_t prev = 0;
    uint16_t cur = 0;
    if (pc < 2u) return 0;
    if (!arm_ext_read_thumb16(m, pc - 2u, &prev) ||
        !arm_ext_read_thumb16(m, pc, &cur)) {
        return 0;
    }
    return arm_ext_thumb_is_blx_reg(prev) && arm_ext_thumb_is_pop_pc(cur);
}

static int arm_ext_pop_pc_target(ArmExtModule *m, uint32_t pc, uint32_t sp,
                                 uint32_t *target) {
    uint16_t hw = 0;
    if (target) *target = 0;
    if (!arm_ext_read_thumb16(m, pc, &hw) || !arm_ext_thumb_is_pop_pc(hw))
        return 0;

    uint32_t target_slot = sp + arm_ext_popcount8(hw & 0xffu) * 4u;
    if (target_slot < sp || !arm_ptr(m, target_slot) ||
        !arm_ptr(m, target_slot + 3u)) {
        return 0;
    }
    if (target) memcpy(target, arm_ptr(m, target_slot), 4);
    return 1;
}

static int arm_ext_pop_pc_stack_bytes(ArmExtModule *m, uint32_t pc,
                                      uint32_t *bytes) {
    uint16_t hw = 0;
    if (bytes) *bytes = 0;
    if (!arm_ext_read_thumb16(m, pc, &hw) || !arm_ext_thumb_is_pop_pc(hw))
        return 0;
    uint32_t count = arm_ext_popcount8(hw & 0xffu) + 1u;
    if (bytes) *bytes = count * 4u;
    return 1;
}

static int arm_ext_table_return_guard_index(ArmExtModule *m, uint32_t pc) {
    if (!m || !pc) return -1;
    for (uint32_t i = 0; i < ARM_EXT_TABLE_RETURN_GUARD_MAX; ++i) {
        if (m->table_return_guard_pc[i] == pc) return (int)i;
    }
    return -1;
}

void arm_ext_note_table_return_guard(ArmExtModule *m, uint32_t lr,
                                            uint32_t sp) {
    if (!m || !(lr & 1u)) return;
    uint32_t pc = lr & ~1u;
    if (!arm_ext_table_return_site_is_guarded(m, pc)) return;

    for (uint32_t i = 0; i < ARM_EXT_TABLE_RETURN_GUARD_MAX; ++i) {
        if (m->table_return_guard_pending[i] &&
            m->table_return_guard_pc[i] == pc &&
            m->table_return_guard_sp[i] == sp) {
            return;
        }
    }
    for (uint32_t i = 0; i < ARM_EXT_TABLE_RETURN_GUARD_MAX; ++i) {
        if (!m->table_return_guard_pending[i] &&
            (!m->table_return_guard_pc[i] ||
             m->table_return_guard_pc[i] == pc)) {
            m->table_return_guard_pc[i] = pc;
            m->table_return_guard_sp[i] = sp;
            m->table_return_guard_pending[i] = 1;
            return;
        }
    }

    uint32_t i = m->table_return_guard_next++ %
                 ARM_EXT_TABLE_RETURN_GUARD_MAX;
    m->table_return_guard_pc[i] = pc;
    m->table_return_guard_sp[i] = sp;
    m->table_return_guard_pending[i] = 1;
}

static int arm_ext_consume_table_return_guard(ArmExtModule *m, uint32_t pc,
                                              uint32_t sp) {
    if (!m || !pc) return 0;
    for (uint32_t i = 0; i < ARM_EXT_TABLE_RETURN_GUARD_MAX; ++i) {
        if (m->table_return_guard_pending[i] &&
            m->table_return_guard_pc[i] == pc &&
            m->table_return_guard_sp[i] == sp) {
            uint32_t pop_bytes = 0;
            if (!arm_ext_pop_pc_stack_bytes(m, pc, &pop_bytes) ||
                sp + pop_bytes < sp) {
                return 0;
            }
            /*
             * This first hit is the real return from a native table bridge.
             * Remember the SP after the epilogue's pop so a duplicate entry into
             * the same epilogue can be identified structurally, without blocking
             * a later legitimate call that reuses the same return site.
             */
            m->table_return_guard_sp[i] = sp + pop_bytes;
            m->table_return_guard_pending[i] = 0;
            return 1;
        }
    }
    return 0;
}

static int arm_ext_table_return_guard_is_stale(ArmExtModule *m, uint32_t pc,
                                               uint32_t sp) {
    int idx = arm_ext_table_return_guard_index(m, pc);
    return idx >= 0 &&
           !m->table_return_guard_pending[idx] &&
           m->table_return_guard_sp[idx] == sp;
}

int arm_ext_guard_table_return_block(uc_engine *uc, ArmExtModule *m,
                                            uint32_t address) {
    if (!m) return 0;
    uint32_t pc = address & ~1u;
    if (arm_ext_table_return_guard_index(m, pc) < 0) return 0;

    uint32_t sp = reg_read32(uc, UC_ARM_REG_SP);
    if (arm_ext_consume_table_return_guard(m, pc, sp)) return 0;

    uint32_t lr = reg_read32(uc, UC_ARM_REG_LR);
    if ((lr & ~1u) != pc || !(lr & 1u) ||
        !arm_ext_table_return_guard_is_stale(m, pc, sp) ||
        !arm_ext_table_return_site_is_guarded(m, pc)) {
        return 0;
    }

    if (arm_ext_trace_on()) {
        uint32_t target = 0;
        int have_target = arm_ext_pop_pc_target(m, pc, sp, &target);
        printf("arm_ext_executor: stopped stale table return epilogue pc=0x%X sp=0x%X lr=0x%X pop_pc=0x%X\n",
               pc, sp, lr, have_target ? target : 0);
    }
    reg_write32(uc, UC_ARM_REG_PC, EXT_STOP_ADDR);
    uc_emu_stop(uc);
    return 1;
}

