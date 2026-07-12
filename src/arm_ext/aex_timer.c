/*
 * aex_timer.c —— 定时器队列扫描与状态同步(Phase 2 收尾)。
 *
 * frame/compact/wrapper 三类 timer 节点判活与队列扫描(节点布局见
 * docs/arm-ext-abi.md 第 5 节)、game timer head 读写、timer owner 记录、
 * mr_timer_state 同步、dispatch 捕获。函数体原样迁移。
 */
#include "../include/arm_ext_priv.h"

static int arm_ext_frame_timer_node_is_valid(ArmExtModule *m,
                                             uint32_t node) {
    if (!m || !node || !arm_ptr(m, node) || !arm_ptr(m, node + 0x1Cu))
        return 0;
    /*
     * frame.ext allocates timer nodes at 0x2C9486 and stamps the first word
     * with 0x79ABBCCF before 0x2C9538 links them into the R9+0x94 queue.
     * Require that structural marker so unrelated child RW data is not
     * mistaken for a private timer queue.
     */
    return arm_ext_read_u32_or_zero_(m, node) == ARM_EXT_COMPACT_TIMER_MAGIC;
}

int arm_ext_foreground_child_has_frame_timer_queue(ArmExtModule *m) {
    if (!arm_ext_has_foreground_child(m)) return 0;
    uint32_t rw_base = arm_ext_active_rw_base(m);
    if (!rw_base ||
        !arm_ptr(m, rw_base + AEX_FRAME_TIMER_SCHED_OFF) ||
        !arm_ptr(m, rw_base + AEX_FRAME_TIMER_SCHED_OFF + 0x0Cu)) {
        return 0;
    }
    /*
     * DOTA frame.ext code=2 (0x2C96A0) consumes the queue at R9+0x94:
     * [base+8] is the queued head and [base+12] is the active/current list.
     * If both are empty, direct child code=2 immediately returns and can
     * starve the wrapper-owned loading queue that installed the child.
     */
    uint32_t queued = arm_ext_read_u32_or_zero_(
        m, rw_base + AEX_FRAME_TIMER_SCHED_OFF + 0x08u);
    uint32_t active = arm_ext_read_u32_or_zero_(
        m, rw_base + AEX_FRAME_TIMER_SCHED_OFF + 0x0Cu);
    return arm_ext_frame_timer_node_is_valid(m, queued) ||
           arm_ext_frame_timer_node_is_valid(m, active);
}

static int arm_ext_compact_timer_node_is_valid(ArmExtModule *m,
                                               uint32_t node) {
    return m && node && arm_ptr(m, node) && arm_ptr(m, node + 0x1Cu) &&
           arm_ext_read_u32_or_zero_(m, node) == ARM_EXT_COMPACT_TIMER_MAGIC;
}

static int arm_ext_compact_timer_queue_at(ArmExtModule *m,
                                          uint32_t rw_base,
                                          uint32_t scheduler_off) {
    if (!rw_base || !arm_ptr(m, rw_base + scheduler_off + 0x10u))
        return 0;

    /* The matched walker supplies this module's R9-relative scheduler offset.
     * Its header ABI keeps queued at +8 and active/current at +12 even when
     * different SDK layouts place the header at widely different offsets. */
    uint32_t queued = arm_ext_read_u32_or_zero_(
        m, rw_base + scheduler_off + 0x08u);
    uint32_t current = arm_ext_read_u32_or_zero_(
        m, rw_base + scheduler_off + 0x0Cu);
    return arm_ext_compact_timer_node_is_valid(m, queued) ||
           arm_ext_compact_timer_node_is_valid(m, current);
}

int arm_ext_foreground_child_has_compact_timer_queue(ArmExtModule *m) {
    if (!arm_ext_has_foreground_child(m)) return 0;
    ArmExtNestedModule *mod = arm_ext_find_nested_module_by_p(
        m, m->active_p_addr);
    if (!mod || !mod->file_addr || !mod->file_len) return 0;
    if (!mod->compact_timer_scheduler_off)
        return 0;

    /*
     * The compact walker returns immediately when its queue/current heads are
     * null; the timestamp word is cleared before that return and is not
     * runnable work by itself.  Only real node pointers justify stealing a
     * wrapper-owned host timer tick for the foreground child.
     */
    uint32_t rw_base = arm_ext_active_rw_base(m);
    return arm_ext_compact_timer_queue_at(
        m, rw_base, mod->compact_timer_scheduler_off);
}

int arm_ext_primary_has_compact_timer_queue(ArmExtModule *m) {
    if (!m || !m->primary_p_addr) return 0;
    ArmExtNestedModule *mod = arm_ext_find_nested_module_by_p(
        m, m->primary_p_addr);
    if (!mod || !mod->compact_timer_scheduler_off) return 0;
    return arm_ext_compact_timer_queue_at(
        m, arm_ext_primary_rw_base_(m), mod->compact_timer_scheduler_off);
}

int arm_ext_wrapper_dispatch_is_busy(ArmExtModule *m) {
    /*
     * wrapper_rw+4 is only a queue/busy flag in the older direct-dispatch
     * layout identified by find_wrapper_timer_dispatch().  The 19KB
     * chain-walker wrapper keeps unrelated nonzero data at the same offset, so
     * using it as generic liveness pins stale wrapper timer ownership forever.
     */
    if (!m || !m->wrapper_timer_dispatch_addr) return 0;
    if (m->wrapper_compact_timer_scheduler_off) {
        /*
         * Compact wrapper walkers load an R9-relative scheduler literal and
         * consume real node lists from that header.  In that ABI wrapper_rw+4
         * is broader state, not runnable timer evidence; using it after the
         * scheduler is empty can keep code=2 pinned to the wrapper forever.
         */
        return 0;
    }
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw || !arm_ptr(m, wrapper_rw + 0x04u)) return 0;
    /*
     * cfunction.ext wrapper dispatch uses wrapper_rw+4 as its queue/busy flag
     * (see the 0x1F50 dispatch notes).  Read it only as liveness evidence:
     * mutating this flag changes wrapper queue semantics and previously left
     * pending events undrained.
     */
    return arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x04u) != 0;
}

static int arm_ext_wrapper_timer_node_is_valid(ArmExtModule *m,
                                               uint32_t node) {
    if (!m || !node || !arm_ptr(m, node) || !arm_ptr(m, node + 0x20u))
        return 0;
    /*
     * optwar's cfunction.ext timer queue at 0xE84920 stores next/prev at
     * node+0/+4, stamps 0x79ABBCCF at node+8, compares due time at +0x0C,
     * and invokes callback/data fields through +0x14/+0x18/+0x1C.
     */
    return arm_ext_read_u32_or_zero_(m, node + 0x08u) ==
           ARM_EXT_COMPACT_TIMER_MAGIC;
}

static int arm_ext_wrapper_compact_timer_node_is_valid(ArmExtModule *m,
                                                       uint32_t node) {
    if (!m || !node || !arm_ptr(m, node) || !arm_ptr(m, node + 0x1Cu))
        return 0;
    /*
     * The compact wrapper add/remove path compares node[0] with this SDK
     * marker before linking through node+0x18/node+0x1c.
     */
    return arm_ext_read_u32_or_zero_(m, node) == ARM_EXT_COMPACT_TIMER_MAGIC;
}

static int arm_ext_wrapper_compact_timer_queue_at(ArmExtModule *m,
                                                  uint32_t wrapper_rw,
                                                  uint32_t scheduler_off) {
    if (!wrapper_rw || !scheduler_off ||
        !arm_ptr(m, wrapper_rw + scheduler_off + 0x14u)) {
        return 0;
    }

    /*
     * The matched wrapper walker loads R6=R9+scheduler_off, then consumes
     * [R6+0x0c] as the sorted queue, [R6+0x10] as the active/due chain, and
     * [R6+0x14] as its last-tick timestamp.
     */
    uint32_t queued = arm_ext_read_u32_or_zero_(
        m, wrapper_rw + scheduler_off + 0x0Cu);
    uint32_t current = arm_ext_read_u32_or_zero_(
        m, wrapper_rw + scheduler_off + 0x10u);
    return arm_ext_wrapper_compact_timer_node_is_valid(m, queued) ||
           arm_ext_wrapper_compact_timer_node_is_valid(m, current);
}

int arm_ext_wrapper_has_timer_queue(ArmExtModule *m) {
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw) {
        return 0;
    }

    if (arm_ext_wrapper_compact_timer_queue_at(
            m, wrapper_rw, m->wrapper_compact_timer_scheduler_off)) {
        return 1;
    }

    if (arm_ptr(m, wrapper_rw + AEX_WRAPPER_LEGACY_TIMER_QUEUED_OFF) &&
        arm_ptr(m, wrapper_rw + AEX_WRAPPER_LEGACY_TIMER_CURRENT_OFF)) {
        uint32_t queued = arm_ext_read_u32_or_zero_(
            m, wrapper_rw + AEX_WRAPPER_LEGACY_TIMER_QUEUED_OFF);
        uint32_t current = arm_ext_read_u32_or_zero_(
            m, wrapper_rw + AEX_WRAPPER_LEGACY_TIMER_CURRENT_OFF);
        return arm_ext_wrapper_timer_node_is_valid(m, queued) ||
               arm_ext_wrapper_timer_node_is_valid(m, current);
    }
    return 0;
}


void sync_timer_state_from_arm(ArmExtModule *m) {
    enum { ARM_TIMER_IDLE = 0, ARM_TIMER_RUNNING = 1 };
    uint32_t arm_timer_state = 0;
    if (!internal_slot_read(m, m ? m->mr_timer_state_slot : 0, &arm_timer_state)) {
        return;
    }
    if (arm_timer_state <= 3) {
        if (m->host_timer_pending &&
            mr_timer_state == ARM_TIMER_RUNNING &&
            arm_timer_state == ARM_TIMER_IDLE) {
            internal_slot_write(m, m->mr_timer_state_slot, ARM_TIMER_RUNNING);
            return;
        }
        mr_timer_state = (int32)arm_timer_state;
        if (mr_timer_state != ARM_TIMER_RUNNING) {
            m->host_timer_pending = 0;
        }
    }
}

void arm_ext_record_timer_owner(ArmExtModule *m) {
    if (!m) return;
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR) & ~1u;
    uint32_t owner_p = 0;
    uint32_t owner_helper = 0;
    int owner_from_current_dispatch = 0;

    owner_p = arm_ext_p_for_code_addr(m, lr, &owner_helper);
    if (owner_p == m->p_addr && owner_helper == m->helper_addr &&
        m->current_p_addr && m->current_helper_addr &&
        (m->current_p_addr != m->p_addr ||
         m->current_helper_addr != m->helper_addr)) {
        /*
         * Some wrapper EXT builds expose table[31] through a cfunction.ext
         * veneer.  While dispatching a child helper, LR then points into the
         * wrapper even though the timer belongs to the child that made the
         * call.  Prefer the active helper context only for that wrapper-veneer
         * case; direct calls from game/child code keep the LR-derived owner.
         */
        owner_p = m->current_p_addr;
        owner_helper = m->current_helper_addr;
        owner_from_current_dispatch = 1;
    }
    if (!owner_p || !owner_helper) {
        /*
         * table[31] owner is normally identified from LR's code range. The
         * current/active path is kept for host-synthetic entries whose LR is
         * outside every loaded EXT image. Verification: gxdzc e2e payment
         * routing remains stable.
         */
        owner_p = m->current_p_addr ? m->current_p_addr :
                  (m->active_p_addr ? m->active_p_addr : m->p_addr);
        owner_helper = m->current_helper_addr ? m->current_helper_addr :
                       (m->active_helper_addr ? m->active_helper_addr : m->helper_addr);
        owner_from_current_dispatch = m->current_p_addr && m->current_helper_addr;
    }

    if (owner_p && owner_helper) {
        m->timer_p_addr = owner_p;
        m->timer_helper_addr = owner_helper;
        if (arm_ext_diag_on()) {
            printf("DIAG timer_owner lr=0x%X ownerP=0x%X ownerH=0x%X fromCurrent=%d activeP=0x%X activeH=0x%X currentP=0x%X currentH=0x%X\n",
                   lr, owner_p, owner_helper, owner_from_current_dispatch,
                   m->active_p_addr, m->active_helper_addr,
                   m->current_p_addr, m->current_helper_addr);
        }
    }
}

/* origin_mem 统计同步。
 * 宿主 table[0]/table[1]/table[2] 使用 arm_alloc 而非 origin_mem
 * 池本身来分配内存，ARM 侧自带的 free-list 分配器因此不会运行，
 * 对应的 slot 值也不会被 ARM 代码更新。这里在宿主侧 accounting
 * 后立即将最新值写回 ARM slot，让 ext 读到一致的统计。 */

static int arm_ext_is_standard_ext_self_pointer(uint32_t value) {
    if (value < EXT_TABLE_ADDR ||
        value >= EXT_TABLE_ADDR + EXT_TABLE_COUNT * 4u) {
        return 0;
    }
    uint32_t idx = (value - EXT_TABLE_ADDR) / 4u;
    return value == EXT_TABLE_ADDR + idx * 4u;
}

/* 读取 game 的定时器链表头。不同版本的 game.ext SDK 把 timer head 写在
 * game_rw 的不同偏移（0x8C 或 0x88），这里依次尝试两个已知偏移。 */
uint32_t read_game_timer_head(ArmExtModule *m, uint32_t grw) {
    if (!grw) return 0;
    uint32_t val = 0;
    if (arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET))
        memcpy(&val, arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET), 4);
    if (val && !arm_ext_is_standard_ext_self_pointer(val)) return val;
    val = 0;
    if (arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET_ALT))
        memcpy(&val, arm_ptr(m, grw + GAME_TIMER_HEAD_OFFSET_ALT), 4);
    return arm_ext_is_standard_ext_self_pointer(val) ? 0 : val;
}

/* 写回 game 的定时器链表头。使用与 read 相同的偏移搜索逻辑：
 * 首先尝试主偏移 0x8C，若该处本身就有值则写到 0x8C；
 * 否则写到备选偏移 0x88。 */
void write_game_timer_head(ArmExtModule *m, uint32_t grw, uint32_t val) {
    if (!grw) return;
    uint32_t cur = 0;
    void *main_slot = arm_ptr_span(m, grw + GAME_TIMER_HEAD_OFFSET, 4u);
    void *alt_slot = arm_ptr_span(m, grw + GAME_TIMER_HEAD_OFFSET_ALT, 4u);
    if (main_slot) memcpy(&cur, main_slot, 4);
    if ((cur && !arm_ext_is_standard_ext_self_pointer(cur)) || !alt_slot) {
        /* B2:两个候选槽都不可映射时放弃写入(此前主槽为 NULL 仍 memcpy,
         * 宿主段错误;读侧本就判空,写侧对齐) */
        if (main_slot) memcpy(main_slot, &val, 4);
    } else {
        memcpy(alt_slot, &val, 4);
    }
}

void capture_timer_dispatches(ArmExtModule *m) {
    if (!m || !m->primary_helper_addr) return;
    uint32_t t31a = EXT_TABLE_ADDR + 31 * 4;
    uint32_t t32a = EXT_TABLE_ADDR + 32 * 4;
    uint32_t t31v = 0, t32v = 0;
    memcpy(&t31v, arm_ptr(m, t31a), 4);
    memcpy(&t32v, arm_ptr(m, t32a), 4);
    uint32_t primary_start = m->primary_file_addr;
    uint32_t primary_end = primary_start + m->primary_file_len;
    int t31_primary = primary_start && t31v >= primary_start && t31v < primary_end;
    int t32_primary = primary_start && t32v >= primary_start && t32v < primary_end;

    if (t31v != t31a && arm_ptr(m, t31v)) {
        /* 嵌套 EXT 会把 SDK 的 timerStart 表项改成自己的 dispatch veneer
         * （mpc/game.ext 反汇编为 str ..., [table,#0x7c]）。模拟器必须保留
         * table hook，同时记录真实 veneer，用 wrapper R9 转调。 */
        int captured = (!m->dispatch_timer_start || t31_primary);
        if (captured) {
            m->dispatch_timer_start = t31v;
        }
        memcpy(arm_ptr(m, t31a), &t31a, 4);
        if (arm_ext_trace_on()) {
            printf("arm_ext_executor: %s timerStart dispatch=0x%X\n",
                   captured ? "captured" : "ignored", t31v);
        }
    }
    if (t32v != t32a && arm_ptr(m, t32v)) {
        /* 同上，timerStop 也可能被嵌套 EXT 替换为 stop dispatch veneer。 */
        int captured = (!m->dispatch_timer_stop || t32_primary);
        if (captured) {
            m->dispatch_timer_stop = t32v;
        }
        memcpy(arm_ptr(m, t32a), &t32a, 4);
        if (arm_ext_trace_on()) {
            printf("arm_ext_executor: %s timerStop dispatch=0x%X\n",
                   captured ? "captured" : "ignored", t32v);
        }
    }
}
