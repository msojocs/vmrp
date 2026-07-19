/*
 * aex_diag.c —— 诊断输出(Phase 2 收尾)。
 *
 * fd 名称登记表(DIAG 里给 fd 显示文件名)、LR 归属诊断,以及从
 * aex_screen.c 归位的层状态/timer 队列 dump。全部仅在 VMRP_ARM_EXT_DIAG
 * 等开关下输出,函数体原样迁移。
 */
#include "../include/arm_ext_priv.h"

static void arm_ext_diag_dump_timer_node(ArmExtModule *m,
                                         const char *tag,
                                         const char *name,
                                         uint32_t node) {
    if (!m || !(arm_ext_diag_on() || arm_ext_timer_owner_diag_on()) || !node)
        return;
    if (!arm_ptr(m, node) || !arm_ptr(m, node + 0x1Cu)) {
        printf("DIAG timer_node %s %s=0x%X unmapped\n",
               tag ? tag : "?", name ? name : "?", node);
        return;
    }
    uint32_t w[8];
    for (uint32_t i = 0; i < 8; ++i)
        w[i] = arm_ext_read_u32_or_zero_(m, node + i * 4u);
    printf("DIAG timer_node %s %s=0x%X [00]=0x%X [04]=0x%X [08]=0x%X [0C]=0x%X [10]=0x%X [14]=0x%X [18]=0x%X [1C]=0x%X\n",
           tag ? tag : "?", name ? name : "?", node,
           w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
}

void arm_ext_diag_fd_set(ArmExtModule *m, int32_t handle,
                                const char *name) {
    if (!m || handle <= 0 || !name) return;
    ArmExtDiagFd *free_slot = NULL;
    for (uint32_t i = 0; i < ARM_EXT_DIAG_FD_MAX; ++i) {
        if (m->diag_fds[i].handle == handle) {
            snprintf(m->diag_fds[i].name, sizeof(m->diag_fds[i].name),
                     "%s", name);
            return;
        }
        if (!free_slot && m->diag_fds[i].handle == 0)
            free_slot = &m->diag_fds[i];
    }
    if (!free_slot)
        free_slot = &m->diag_fds[(uint32_t)handle % ARM_EXT_DIAG_FD_MAX];
    free_slot->handle = handle;
    snprintf(free_slot->name, sizeof(free_slot->name), "%s", name);
}

const char *arm_ext_diag_fd_name(ArmExtModule *m, int32_t handle) {
    if (!m || handle <= 0) return "";
    for (uint32_t i = 0; i < ARM_EXT_DIAG_FD_MAX; ++i) {
        if (m->diag_fds[i].handle == handle)
            return m->diag_fds[i].name;
    }
    return "";
}

void arm_ext_diag_fd_clear(ArmExtModule *m, int32_t handle) {
    if (!m || handle <= 0) return;
    for (uint32_t i = 0; i < ARM_EXT_DIAG_FD_MAX; ++i) {
        if (m->diag_fds[i].handle == handle) {
            m->diag_fds[i].handle = 0;
            m->diag_fds[i].name[0] = '\0';
            return;
        }
    }
}

void arm_ext_diag_owner_for_lr(ArmExtModule *m,
                                      uint32_t *owner_p,
                                      uint32_t *owner_h,
                                      uint32_t *owner_file,
                                      uint32_t *owner_len) {
    if (owner_p) *owner_p = 0;
    if (owner_h) *owner_h = 0;
    if (owner_file) *owner_file = 0;
    if (owner_len) *owner_len = 0;
    ArmExtNestedModule *owner =
        arm_ext_resource_owner_for_lr(m, owner_p, owner_h);
    if (owner_file) *owner_file = owner ? owner->file_addr : 0;
    if (owner_len) *owner_len = owner ? owner->file_len : 0;
}


void arm_ext_diag_dump_layer_state(ArmExtModule *m, const char *tag) {
    if (!m || !arm_ext_diag_on()) return;
    uint32_t primary_chunk = 0;
    uint32_t active_chunk = 0;
    uint32_t primary_rw = 0;
    uint32_t active_rw = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + AEX_P_EXT_CHUNK_OFF)) {
        memcpy(&primary_rw, arm_ptr(m, m->primary_p_addr), 4);
        memcpy(&primary_chunk, arm_ptr(m, m->primary_p_addr + AEX_P_EXT_CHUNK_OFF), 4);
    }
    if (m->active_p_addr && arm_ptr(m, m->active_p_addr + AEX_P_EXT_CHUNK_OFF)) {
        memcpy(&active_rw, arm_ptr(m, m->active_p_addr), 4);
        memcpy(&active_chunk, arm_ptr(m, m->active_p_addr + AEX_P_EXT_CHUNK_OFF), 4);
    }
    uint32_t pc = reg_read32(m->uc, UC_ARM_REG_PC);
    uint32_t lr = reg_read32(m->uc, UC_ARM_REG_LR);
    uint32_t r9 = reg_read32(m->uc, UC_ARM_REG_R9);
    uint16_t cover0 = UINT16_MAX;
    uint16_t cover1 = 0;
    if (arm_ext_ensure_foreground_cover_regions(m) &&
        m->foreground_cover.rows > 27) {
        cover0 = m->foreground_cover.min_x[27];
        cover1 = m->foreground_cover.max_x[27];
    }
    uint32_t p24 = 0, p28 = 0, p34 = 0;
    uint32_t a24 = 0, a28 = 0, a34 = 0;
    if (primary_chunk) {
        p24 = arm_ext_read_u32_or_zero_(m, primary_chunk + AEX_CHUNK_EVENT_DATA_OFF);
        p28 = arm_ext_read_u32_or_zero_(m, primary_chunk + AEX_CHUNK_EVENT_FUNC_OFF);
        p34 = arm_ext_read_u32_or_zero_(m, primary_chunk + AEX_CHUNK_SUSPEND_OFF);
    }
    if (active_chunk) {
        a24 = arm_ext_read_u32_or_zero_(m, active_chunk + AEX_CHUNK_EVENT_DATA_OFF);
        a28 = arm_ext_read_u32_or_zero_(m, active_chunk + AEX_CHUNK_EVENT_FUNC_OFF);
        a34 = arm_ext_read_u32_or_zero_(m, active_chunk + AEX_CHUNK_SUSPEND_OFF);
    }
    printf("DIAG layer %s pc=0x%X lr=0x%X r9=0x%X activeP=0x%X activeH=0x%X activeRW=0x%X activeChunk=0x%X active[24]=0x%X active[28]=0x%X active[34]=0x%X primaryP=0x%X primaryH=0x%X primaryRW=0x%X primaryChunk=0x%X primary[24]=0x%X primary[28]=0x%X primary[34]=0x%X timerP=0x%X timerH=0x%X cover27=%u..%u fgOwnerP=0x%X fgOwnerH=0x%X fgValid=%d hostTimer=%d mrTimer=%d\n",
           tag ? tag : "?",
           pc, lr, r9,
           m->active_p_addr, m->active_helper_addr, active_rw, active_chunk,
           a24, a28, a34,
           m->primary_p_addr, m->primary_helper_addr, primary_rw, primary_chunk,
           p24, p28, p34,
           m->timer_p_addr, m->timer_helper_addr,
           cover0, cover1,
           m->foreground_screen_owner_p_addr,
           m->foreground_screen_owner_helper_addr,
           (m->foreground_screen_owner_p_addr != 0),
           m->host_timer_pending, mr_timer_state);
}

void arm_ext_diag_dump_rw_timer_state(ArmExtModule *m,
                                             const char *tag,
                                             uint32_t rw_base) {
    if (!m || !arm_ext_diag_on() || !rw_base) return;
    if (!arm_ptr(m, rw_base) || !arm_ptr(m, rw_base + 0x1Cu)) {
        printf("DIAG rw_low %s rw=0x%X unmapped\n",
               tag ? tag : "?", rw_base);
        return;
    }
    uint32_t w[8];
    for (uint32_t i = 0; i < 8; ++i)
        w[i] = arm_ext_read_u32_or_zero_(m, rw_base + i * 4u);
    /*
     * Keep the low words because older helper variants put timer queues near
     * R9+8, but do not treat them as authoritative for every private child.
     */
    printf("DIAG rw_low %s rw=0x%X +00=0x%X +04=0x%X +08=0x%X +0C=0x%X +10=0x%X +14=0x%X +18=0x%X +1C=0x%X\n",
           tag ? tag : "?", rw_base,
           w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    arm_ext_diag_dump_timer_node(m, tag, "rw+08", w[2]);
    arm_ext_diag_dump_timer_node(m, tag, "rw+0C", w[3]);
    /*
     * DOTA's browser frame helper disassembly proves a different timer base:
     * 0x2C96A0 loads literal 0x94, adds R9, then uses [base+8] for the queued
     * head, [base+12] for the active list and [base+16] for the last tick.
     * Dump that structural window for all private children so the evidence is
     * about the module ABI, not a filename or app-specific branch.
     */
    if (arm_ptr(m, rw_base + 0x94u) && arm_ptr(m, rw_base + 0xC0u)) {
        uint32_t fw[12];
        for (uint32_t i = 0; i < 12; ++i)
            fw[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x94u + i * 4u);
        printf("DIAG rw_frame %s rw=0x%X +94=0x%X +98=0x%X +9C=0x%X +A0=0x%X +A4=0x%X +A8=0x%X +AC=0x%X +B0=0x%X +B4=0x%X +B8=0x%X +BC=0x%X +C0=0x%X\n",
               tag ? tag : "?", rw_base,
               fw[0], fw[1], fw[2], fw[3], fw[4], fw[5],
               fw[6], fw[7], fw[8], fw[9], fw[10], fw[11]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+9C", fw[2]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+A0", fw[3]);
    }
    if (arm_ptr(m, rw_base + 0x1B0u) && arm_ptr(m, rw_base + 0x200u)) {
        uint32_t cb[8];
        for (uint32_t i = 0; i < 8; ++i)
            cb[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x1C0u + i * 4u);
        printf("DIAG rw_cb %s rw=0x%X +1C0=0x%X +1C4=0x%X +1C8=0x%X +1CC=0x%X +1D0=0x%X +1D4=0x%X +1D8=0x%X +1DC=0x%X\n",
               tag ? tag : "?", rw_base,
               cb[0], cb[1], cb[2], cb[3], cb[4], cb[5], cb[6], cb[7]);
    }
    if (arm_ptr(m, rw_base + 0x0C0u) && arm_ptr(m, rw_base + 0x0E4u)) {
        uint32_t q[10];
        for (uint32_t i = 0; i < 10; ++i)
            q[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x0C0u + i * 4u);
        /*
         * Compact scheduler variant whose walker loads R9+0xC0 before using
         * +8/+12/+16 as queue/current/last-tick state.
         */
        printf("DIAG rw_sched_low %s rw=0x%X +0C0=0x%X +0C4=0x%X +0C8=0x%X +0CC=0x%X +0D0=0x%X +0D4=0x%X +0D8=0x%X +0DC=0x%X +0E0=0x%X +0E4=0x%X\n",
               tag ? tag : "?", rw_base,
               q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+0C8", q[2]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+0CC", q[3]);
    }
    if (arm_ptr(m, rw_base + 0x248u) && arm_ptr(m, rw_base + 0x26Cu)) {
        uint32_t q[10];
        for (uint32_t i = 0; i < 10; ++i)
            q[i] = arm_ext_read_u32_or_zero_(m, rw_base + 0x248u + i * 4u);
        /*
         * Some private modal children use a compact scheduler just past their
         * copied bridge table.  verdload.ext's code=2 walker loads R9+0x248
         * and then consumes +8/+12/+16 as queue/current/last-tick state.
         */
        printf("DIAG rw_sched %s rw=0x%X +248=0x%X +24C=0x%X +250=0x%X +254=0x%X +258=0x%X +25C=0x%X +260=0x%X +264=0x%X +268=0x%X +26C=0x%X\n",
               tag ? tag : "?", rw_base,
               q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+250", q[2]);
        arm_ext_diag_dump_timer_node(m, tag, "rw+254", q[3]);
    }
    ArmExtNestedModule *owner = arm_ext_find_nested_module_by_rw(m, rw_base);
    if (owner) {
        uint32_t record = arm_ext_read_u32_or_zero_(m, owner->file_addr);
        uint32_t slot0 = arm_ext_read_u32_or_zero_(m, record + 0u * 4u);
        uint32_t slot3 = arm_ext_read_u32_or_zero_(m, record + 3u * 4u);
        uint32_t slot25 = arm_ext_read_u32_or_zero_(m, record + 25u * 4u);
        uint32_t slot100 = arm_ext_read_u32_or_zero_(m, record + 100u * 4u);
        uint32_t slot125 = arm_ext_read_u32_or_zero_(m, record + 125u * 4u);
        uint32_t slot131 = arm_ext_read_u32_or_zero_(m, record + 131u * 4u);
        /*
         * frame.ext 0x2C93E0 dispatches through record[125] and 0x2C97C8
         * stores the result through its caller-provided R2.  These few slots
         * prove whether the private module record still points at wrapper
         * callbacks or has been restored to host table bridges.
        */
        printf("DIAG record %s file=0x%X record=0x%X slot0=0x%X slot3=0x%X slot25=0x%X slot100=0x%X slot125=0x%X slot131=0x%X\n",
               tag ? tag : "?", owner->file_addr, record,
               slot0, slot3, slot25, slot100, slot125, slot131);
    }
}

void arm_ext_diag_dump_wrapper_timer_state(ArmExtModule *m,
                                                  const char *tag) {
    if (!m || !arm_ext_diag_on()) return;
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw) return;

    if (m->wrapper_compact_timer_scheduler_off &&
        arm_ptr(m, wrapper_rw + m->wrapper_compact_timer_scheduler_off + 0x24u)) {
        uint32_t off = m->wrapper_compact_timer_scheduler_off;
        uint32_t q[10];
        for (uint32_t i = 0; i < 10; ++i)
            q[i] = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + i * 4u);
        /*
         * Discovered from the wrapper timer walker literal. The walker uses
         * +0x0c as queue head, +0x10 as active/due list, and +0x14 as last
         * tick, so dump this ABI before older fixed-offset guesses.
         */
        printf("DIAG wrapper_timer_compact %s rw=0x%X off=0x%X +00=0x%X +04=0x%X +08=0x%X +0C=0x%X +10=0x%X +14=0x%X +18=0x%X +1C=0x%X +20=0x%X +24=0x%X live=%d\n",
               tag ? tag : "?", wrapper_rw, off,
               q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9],
               arm_ext_wrapper_has_timer_queue(m));
        char label_queue[32];
        char label_current[32];
        snprintf(label_queue, sizeof(label_queue), "wrw+%03X+0C", off);
        snprintf(label_current, sizeof(label_current), "wrw+%03X+10", off);
        arm_ext_diag_dump_timer_node(m, tag, label_queue, q[3]);
        arm_ext_diag_dump_timer_node(m, tag, label_current, q[4]);
    }

    if (!arm_ptr(m, wrapper_rw + 0x3C8u) ||
        !arm_ptr(m, wrapper_rw + 0x3E8u)) {
        if (!m->wrapper_compact_timer_scheduler_off) {
            printf("DIAG wrapper_timer %s rw=0x%X unmapped\n",
                   tag ? tag : "?", wrapper_rw);
        }
        return;
    }

    uint32_t q[9];
    for (uint32_t i = 0; i < 9; ++i)
        q[i] = arm_ext_read_u32_or_zero_(m, wrapper_rw + 0x3C8u + i * 4u);
    /*
     * optwar cfunction.ext 0xE84920 loads R9+0x3C8 as the sorted timer queue
     * and R9+0x3D8 as the temporary due-list sentinel before invoking nodes.
     */
    printf("DIAG wrapper_timer %s rw=0x%X +3C8=0x%X +3CC=0x%X +3D0=0x%X +3D4=0x%X +3D8=0x%X +3DC=0x%X +3E0=0x%X +3E4=0x%X +3E8=0x%X live=%d\n",
           tag ? tag : "?", wrapper_rw,
           q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8],
           arm_ext_wrapper_has_timer_queue(m));
    arm_ext_diag_dump_timer_node(m, tag, "wrw+3C8", q[0]);
    arm_ext_diag_dump_timer_node(m, tag, "wrw+3D8", q[4]);
}

void arm_ext_diag_dump_wrapper_compact_timer_nodes(ArmExtModule *m,
                                                         const char *tag) {
    if (!m || !arm_ext_timer_liveness_diag_on() ||
        !m->wrapper_compact_timer_scheduler_off) {
        return;
    }

    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    uint32_t off = m->wrapper_compact_timer_scheduler_off;
    if (!wrapper_rw || !arm_ptr(m, wrapper_rw + off + 0x14u)) return;

    uint32_t queued = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x0Cu);
    uint32_t current = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x10u);
    uint32_t last_tick = arm_ext_read_u32_or_zero_(m, wrapper_rw + off + 0x14u);
    printf("DIAG wrapper_compact_nodes %s rw=0x%X off=0x%X queued=0x%X current=0x%X lastTick=0x%X\n",
           tag ? tag : "?", wrapper_rw, off, queued, current, last_tick);

    uint32_t node = queued;
    for (uint32_t i = 0; i < 4 && node && arm_ptr(m, node + 0x1Cu); ++i) {
        uint32_t marker = arm_ext_read_u32_or_zero_(m, node);
        uint32_t delay = arm_ext_read_u32_or_zero_(m, node + 0x08u);
        uint32_t cb = arm_ext_read_u32_or_zero_(m, node + 0x0Cu);
        uint32_t data = arm_ext_read_u32_or_zero_(m, node + 0x10u);
        uint32_t cleanup = arm_ext_read_u32_or_zero_(m, node + 0x14u);
        uint32_t next = arm_ext_read_u32_or_zero_(m, node + 0x18u);
        uint32_t due_next = arm_ext_read_u32_or_zero_(m, node + 0x1Cu);
        uint32_t cb_owner_h = 0;
        uint32_t cb_owner_p = arm_ext_p_for_code_addr(m, cb & ~1u, &cb_owner_h);
        printf("DIAG wrapper_compact_node %s list=queued idx=%u node=0x%X marker=0x%X delay=0x%X cb=0x%X data=0x%X cleanup=0x%X next=0x%X dueNext=0x%X cbP=0x%X cbH=0x%X\n",
               tag ? tag : "?", i, node, marker, delay, cb, data, cleanup,
               next, due_next, cb_owner_p, cb_owner_h);
        node = next;
    }

    node = current;
    for (uint32_t i = 0; i < 4 && node && arm_ptr(m, node + 0x1Cu); ++i) {
        uint32_t marker = arm_ext_read_u32_or_zero_(m, node);
        uint32_t delay = arm_ext_read_u32_or_zero_(m, node + 0x08u);
        uint32_t cb = arm_ext_read_u32_or_zero_(m, node + 0x0Cu);
        uint32_t data = arm_ext_read_u32_or_zero_(m, node + 0x10u);
        uint32_t cleanup = arm_ext_read_u32_or_zero_(m, node + 0x14u);
        uint32_t next = arm_ext_read_u32_or_zero_(m, node + 0x18u);
        uint32_t due_next = arm_ext_read_u32_or_zero_(m, node + 0x1Cu);
        uint32_t cb_owner_h = 0;
        uint32_t cb_owner_p = arm_ext_p_for_code_addr(m, cb & ~1u, &cb_owner_h);
        printf("DIAG wrapper_compact_node %s list=current idx=%u node=0x%X marker=0x%X delay=0x%X cb=0x%X data=0x%X cleanup=0x%X next=0x%X dueNext=0x%X cbP=0x%X cbH=0x%X\n",
               tag ? tag : "?", i, node, marker, delay, cb, data, cleanup,
               next, due_next, cb_owner_p, cb_owner_h);
        node = due_next;
    }
}

void arm_ext_diag_dump_primary_compact_timer_nodes(ArmExtModule *m,
                                                   const char *tag) {
    if (!m ||
        !(arm_ext_timer_liveness_diag_on() || arm_ext_timer_owner_diag_on()) ||
        !m->primary_p_addr)
        return;

    ArmExtNestedModule *mod = arm_ext_find_nested_module_by_p(
        m, m->primary_p_addr);
    uint32_t rw = arm_ext_primary_rw_base_(m);
    if (!mod || !mod->compact_timer_scheduler_off || !rw) return;

    uint32_t off = mod->compact_timer_scheduler_off;
    if (!arm_ptr(m, rw + off + 0x10u)) return;

    uint32_t queued = arm_ext_read_u32_or_zero_(m, rw + off + 0x08u);
    uint32_t current = arm_ext_read_u32_or_zero_(m, rw + off + 0x0Cu);
    uint32_t last_tick = arm_ext_read_u32_or_zero_(m, rw + off + 0x10u);
    printf("DIAG primary_compact_nodes %s rw=0x%X off=0x%X queued=0x%X current=0x%X lastTick=0x%X\n",
           tag ? tag : "?", rw, off, queued, current, last_tick);

    /* Primary and wrapper compact nodes share the same eight-word ABI. Walk
     * enough of both chains to show whether a non-head callback was retained
     * when ownership crossed between the two schedulers. */
    uint32_t node = queued;
    for (uint32_t i = 0; i < 8 && node; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "primaryQueued[%u]", i);
        arm_ext_diag_dump_timer_node(m, tag, name, node);
        node = arm_ext_read_u32_or_zero_(m, node + 0x18u);
    }
    node = current;
    for (uint32_t i = 0; i < 8 && node; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "primaryCurrent[%u]", i);
        arm_ext_diag_dump_timer_node(m, tag, name, node);
        node = arm_ext_read_u32_or_zero_(m, node + 0x1Cu);
    }
}

/*
 * VMRP_ARM_EXT_WATCH_PC —— 精确 PC 观察点。
 *
 * 值为逗号分隔的模块相对文件偏移(hex,可带0x)。嵌套模块注册时按
 * file_addr+off 挂单地址 UC_HOOK_CODE,命中打印基址/相对偏移/R0-R3/LR。
 * 用途:把静态反汇编得到的文件偏移(如 game.ext 0x366c 焦点移动、
 * 0x1edf8 browser_load_url)对到运行时执行流,验证按键→激活链路走没走到。
 * 与全量 VMRP_ARM_EXT_TRACE_PC 不同,只有命中观察点才输出,日志量可控。
 */
typedef struct {
    uint32_t base;
    uint32_t rel;
} ArmExtPcWatch;

static void hook_watch_pc(uc_engine *uc, uint64_t address, uint32_t size,
                          void *user_data) {
    (void)size;
    ArmExtPcWatch *w = (ArmExtPcWatch *)user_data;
    printf("arm_ext_watch: base=0x%X rel=0x%X pc=0x%X r0=0x%X r1=0x%X r2=0x%X r3=0x%X lr=0x%X r9=0x%X\n",
           w->base, w->rel, (uint32_t)address,
           reg_read32(uc, UC_ARM_REG_R0), reg_read32(uc, UC_ARM_REG_R1),
           reg_read32(uc, UC_ARM_REG_R2), reg_read32(uc, UC_ARM_REG_R3),
           reg_read32(uc, UC_ARM_REG_LR), reg_read32(uc, UC_ARM_REG_R9));
}

void arm_ext_install_pc_watches(ArmExtModule *m, uint32_t base, uint32_t len) {
    const char *list = getenv("SKYENGINE_ARM_EXT_WATCH_PC");
    if (!m || !list || !*list) return;
    char buf[512];
    snprintf(buf, sizeof(buf), "%s", list);
    for (char *save = NULL, *tok = strtok_r(buf, ",", &save); tok;
         tok = strtok_r(NULL, ",", &save)) {
        uint32_t off = (uint32_t)strtoul(tok, NULL, 16);
        if (off == 0 || off >= len) continue;
        ArmExtPcWatch *w = (ArmExtPcWatch *)malloc(sizeof(*w));
        if (!w) return;
        w->base = base;
        w->rel = off;
        uc_hook h;
        if (uc_hook_add(m->uc, &h, UC_HOOK_CODE, hook_watch_pc, w,
                        base + off, base + off + 1) != UC_ERR_OK) {
            printf("arm_ext_watch: hook add failed base=0x%X rel=0x%X\n", base, off);
            free(w);
            return;
        }
        printf("arm_ext_watch: armed base=0x%X rel=0x%X\n", base, off);
    }
}
