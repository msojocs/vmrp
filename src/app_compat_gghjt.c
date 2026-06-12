#include "./include/arm_ext_internal.h"
#include <zlib.h>

typedef struct {
    uint32_t menu_entry_state;
    uint32_t child_download_state;
    uint32_t touch_x_state;
    uint32_t touch_y_state;
    uint32_t request_active;
    uint8_t request_block[0x2Cu];
    uint8_t async_request_block[0x2Cu];
    uint8_t child_slot_flags[8];
    uint8_t child_slot_tail[4];
    uint8_t input_gate_block[0x20];
    uint8_t child_complete_flag;
    uint8_t async_complete_flag;
    uint32_t wrapper_child_event_lists[5][2];
    int wrapper_child_event_lists_valid;
    int valid;
    int loader_confirmed;
} GghjtChildSnapshot;

typedef struct {
    uint32_t pending_chk_arm_buf;
    uint32_t pending_chk_len;
    uint8_t *pending_chk_decomp;
    uint32_t pending_chk_decomp_len;
    GghjtChildSnapshot snapshot;
} GghjtState;

/* --- lifecycle --- */

static void *gghjt_init(ArmExtModule *m) {
    (void)m;
    GghjtState *s = calloc(1, sizeof(GghjtState));
    return s;
}

static void gghjt_cleanup(void *app_state) {
    GghjtState *s = app_state;
    if (!s) return;
    free(s->pending_chk_decomp);
    free(s);
}

/* --- child module detection --- */

enum {
    GGHJT_VERDLOAD_LEN = 0x47E4u,
    GGHJT_VERDLOAD_HELPER_OFF = 0x36DCu,
    GGHJT_VERDLOAD_ENTRY_OFF = 0x08u
};

static const uint8_t verdload_entry_prologue[] = {
    0x10, 0x40, 0x2D, 0xE9
};

static int gghjt_is_known_child(ArmExtModule *m, void *app_state,
                                uint32 file_addr, uint32 file_len,
                                uint32 helper_addr) {
    (void)app_state;
    if (!m) return 0;
    if (file_len != GGHJT_VERDLOAD_LEN) return 0;
    uint32_t helper_pc = helper_addr & ~1u;
    if (helper_pc < file_addr ||
        helper_pc - file_addr != GGHJT_VERDLOAD_HELPER_OFF)
        return 0;
    uint8_t *entry = arm_ptr(m, file_addr + GGHJT_VERDLOAD_ENTRY_OFF);
    if (!entry || !arm_ptr(m, file_addr + file_len - 1u)) return 0;
    return memcmp(entry, verdload_entry_prologue,
                  sizeof(verdload_entry_prologue)) == 0;
}

/* --- child synced (verdload bridge slots) --- */

enum {
    VERDLOAD_EXTRA_SRC_OFF = 0x68u,
    VERDLOAD_EXTRA_DST_OFF = 0x16Cu,
    VERDLOAD_EXTRA_TABLE_INDEX = 26u,
    VERDLOAD_BRIDGE_SRC_OFF = 0x0Cu,
    VERDLOAD_BRIDGE_DST_OFF = 0x170u,
    VERDLOAD_BRIDGE_FIRST_INDEX = 3u,
    VERDLOAD_BRIDGE_COUNT = 17u
};

static void gghjt_on_child_synced(ArmExtModule *m, void *app_state,
                                  uint32 file_addr, uint32 file_len,
                                  uint32 p_addr, uint32 helper_addr,
                                  uint32 rw_base) {
    (void)app_state;
    if (!gghjt_is_known_child(m, app_state, file_addr, file_len, helper_addr))
        return;
    if (!p_addr || arm_ext_read_u32_or_zero_(m, p_addr) != rw_base)
        return;

    uint32_t module_record = arm_ext_read_u32_or_zero_(m, file_addr);
    if (!module_record ||
        !arm_ptr(m, module_record + VERDLOAD_EXTRA_SRC_OFF) ||
        !arm_ptr(m, module_record + VERDLOAD_BRIDGE_SRC_OFF +
                 (VERDLOAD_BRIDGE_COUNT - 1u) * 4u) ||
        !arm_ptr(m, rw_base + VERDLOAD_BRIDGE_DST_OFF +
                 (VERDLOAD_BRIDGE_COUNT - 1u) * 4u))
        return;

    uint32_t bridge = EXT_TABLE_ADDR + VERDLOAD_EXTRA_TABLE_INDEX * 4u;
    memcpy(arm_ptr(m, module_record + VERDLOAD_EXTRA_SRC_OFF), &bridge, 4);
    memcpy(arm_ptr(m, rw_base + VERDLOAD_EXTRA_DST_OFF), &bridge, 4);

    for (uint32_t i = 0; i < VERDLOAD_BRIDGE_COUNT; ++i) {
        bridge = EXT_TABLE_ADDR + (VERDLOAD_BRIDGE_FIRST_INDEX + i) * 4u;
        memcpy(arm_ptr(m, module_record + VERDLOAD_BRIDGE_SRC_OFF + i * 4u),
               &bridge, 4);
        memcpy(arm_ptr(m, rw_base + VERDLOAD_BRIDGE_DST_OFF + i * 4u),
               &bridge, 4);
    }
}

/* --- GOT protection --- */

static int gghjt_should_protect_got_addr(ArmExtModule *m, void *app_state,
                                         uint32 addr) {
    (void)app_state;
    if (!m) return 0;
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    uint32_t event_base = wrapper_rw + 0x190u;
    uint32_t event_end = event_base + 5u * 8u;
    return wrapper_rw && addr >= event_base && addr < event_end;
}

/* --- wrapper event list state --- */

static int gghjt_read_wrapper_child_event_lists(ArmExtModule *m,
                                                uint32_t out[5][2]) {
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw || !arm_ptr(m, wrapper_rw + 0x1B7u)) return 0;

    for (uint32_t slot = 0; slot < 5; ++slot) {
        uint32_t head_addr = wrapper_rw + 0x190u + slot * 8u;
        memcpy(&out[slot][0], arm_ptr(m, head_addr), 4);
        memcpy(&out[slot][1], arm_ptr(m, head_addr + 4u), 4);
    }
    return 1;
}

static void gghjt_write_wrapper_child_event_lists(ArmExtModule *m,
                                                  const GghjtChildSnapshot *s) {
    if (!s->wrapper_child_event_lists_valid) return;
    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    if (!wrapper_rw || !arm_ptr(m, wrapper_rw + 0x1B7u)) return;

    for (uint32_t slot = 0; slot < 5; ++slot) {
        uint32_t head_addr = wrapper_rw + 0x190u + slot * 8u;
        memcpy(arm_ptr(m, head_addr), &s->wrapper_child_event_lists[slot][0], 4);
        memcpy(arm_ptr(m, head_addr + 4u),
               &s->wrapper_child_event_lists[slot][1], 4);
    }

    if (m->got_snapshot_base == wrapper_rw) {
        for (uint32_t off = 0x190u; off < 0x190u + 5u * 8u; off += 4) {
            uint32_t idx = off / 4u;
            if (idx < EXT_TABLE_COUNT)
                m->got_snapshot[idx] = 0;
        }
    }
}

static void gghjt_save_wrapper_state(ArmExtModule *m, void *app_state) {
    GghjtState *gs = app_state;
    if (!gs) return;
    GghjtChildSnapshot *s = &gs->snapshot;
    s->wrapper_child_event_lists_valid =
        gghjt_read_wrapper_child_event_lists(m, s->wrapper_child_event_lists);
}

static void gghjt_restore_wrapper_state(ArmExtModule *m, void *app_state) {
    GghjtState *gs = app_state;
    if (!gs) return;
    gghjt_write_wrapper_child_event_lists(m, &gs->snapshot);
}

/* --- write/read interception (netpay extraction workaround) --- */

static int gghjt_intercept_write(ArmExtModule *m, void *app_state,
                                 uint32 fd, uint32 src_addr, uint32 len,
                                 void **new_src, uint32 *new_len) {
    (void)fd;
    GghjtState *gs = app_state;
    if (!gs) return 0;

    if (src_addr == 0x2001BCu && len == 0xBEu && gs->pending_chk_len > 14000u) {
        size_t out_cap = (size_t)gs->pending_chk_decomp_len;
        if (gs->pending_chk_decomp && out_cap > 0) {
            *new_src = gs->pending_chk_decomp;
            *new_len = (uint32_t)out_cap;
            return 1;
        }
    }
    return 0;
}

static void gghjt_post_write_cleanup(void *app_state) {
    GghjtState *gs = app_state;
    if (!gs) return;
    free(gs->pending_chk_decomp);
    gs->pending_chk_decomp = NULL;
    gs->pending_chk_decomp_len = 0;
    gs->pending_chk_len = 0;
}

static void gghjt_post_read_hook(ArmExtModule *m, void *app_state,
                                 uint32 dst_addr, uint32 requested,
                                 int32 actual_read, void *host_buf) {
    (void)dst_addr;
    (void)requested;
    GghjtState *gs = app_state;
    if (!gs || !m) return;

    if (actual_read > 256 && host_buf) {
        const unsigned char *bp = (const unsigned char *)host_buf;
        if (bp[0] == 0x1f && bp[1] == 0x8b) {
            gs->pending_chk_arm_buf = dst_addr;
            gs->pending_chk_len = requested;
            free(gs->pending_chk_decomp);
            gs->pending_chk_decomp = NULL;
            gs->pending_chk_decomp_len = 0;

            z_stream zs = {0};
            zs.next_in = (Bytef *)bp;
            zs.avail_in = (uInt)actual_read;
            if (inflateInit2(&zs, 16 + MAX_WBITS) == Z_OK) {
                size_t cap = (size_t)actual_read * 4 + 4096;
                for (int tries = 0; tries < 6; ++tries) {
                    uint8_t *out = (uint8_t *)malloc(cap);
                    if (!out) break;
                    zs.next_out = out;
                    zs.avail_out = (uInt)cap;
                    int z = inflate(&zs, Z_FINISH);
                    if (z == Z_STREAM_END) {
                        gs->pending_chk_decomp = out;
                        gs->pending_chk_decomp_len = (uint32_t)zs.total_out;
                        break;
                    } else if (z == Z_BUF_ERROR) {
                        free(out);
                        inflateReset2(&zs, 16 + MAX_WBITS);
                        zs.next_in = (Bytef *)bp;
                        zs.avail_in = (uInt)actual_read;
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
}

/* --- child snapshot --- */

static int gghjt_save_child_snapshot(ArmExtModule *m, void *app_state) {
    GghjtState *gs = app_state;
    if (!gs) return 0;
    uint32_t rw = arm_ext_primary_rw_base_(m);
    if (!m || !rw || !arm_ptr(m, rw + 0x14E7u)) return 0;

    GghjtChildSnapshot *s = &gs->snapshot;
    memcpy(&s->menu_entry_state, arm_ptr(m, rw + 0x0218u), 4);
    memcpy(&s->child_download_state, arm_ptr(m, rw + 0x022Cu), 4);
    memcpy(&s->touch_x_state, arm_ptr(m, rw + 0x033Cu), 4);
    memcpy(&s->touch_y_state, arm_ptr(m, rw + 0x0340u), 4);
    memcpy(&s->request_active, arm_ptr(m, rw + 0x00F8u), 4);
    memcpy(s->request_block, arm_ptr(m, rw + 0x012Cu), sizeof(s->request_block));
    memcpy(s->async_request_block, arm_ptr(m, rw + 0x14BCu),
           sizeof(s->async_request_block));
    memcpy(s->child_slot_flags, arm_ptr(m, rw + 0x0854u), sizeof(s->child_slot_flags));
    memcpy(s->child_slot_tail, arm_ptr(m, rw + 0x0860u), sizeof(s->child_slot_tail));
    memcpy(s->input_gate_block, arm_ptr(m, rw + 0x13ACu),
           sizeof(s->input_gate_block));
    memcpy(&s->child_complete_flag, arm_ptr(m, rw + 0x0751u), 1);
    memcpy(&s->async_complete_flag, arm_ptr(m, rw + 0x12F4u), 1);
    s->wrapper_child_event_lists_valid =
        gghjt_read_wrapper_child_event_lists(m, s->wrapper_child_event_lists);
    s->valid = 1;
    s->loader_confirmed = 0;
    return 1;
}

/* --- child confirmed --- */

static void gghjt_on_child_confirmed(ArmExtModule *m, void *app_state,
                                     uint32 child_p, uint32 child_helper) {
    GghjtState *gs = app_state;
    if (!gs) return;
    GghjtChildSnapshot *s = &gs->snapshot;
    if (!s->valid) return;

    if (child_p && child_helper) {
        s->loader_confirmed = 1;
        if (m->primary_child_reopen_timer_needed)
            gghjt_write_wrapper_child_event_lists(m, s);
    }
}

static void gghjt_on_child_close_complete(ArmExtModule *m, void *app_state);

/* --- restore child if closed --- */

static void gghjt_dump_state_impl(ArmExtModule *m, GghjtState *gs,
                                  const char *tag, int32 code,
                                  uint32 input_addr, uint32 input_len);

static int gghjt_restore_child_if_closed(ArmExtModule *m, void *app_state,
                                         int32 code, uint32 input_addr,
                                         uint32 input_len) {
    GghjtState *gs = app_state;
    if (!gs) return 0;
    uint32_t rw = arm_ext_primary_rw_base_(m);
    if (!rw || !arm_ptr(m, rw + 0x14E7u)) return 0;

    GghjtChildSnapshot *s = &gs->snapshot;
    if (!s->valid || !s->loader_confirmed) return 0;

    uint32_t child_state = 0;
    uint8_t child_complete = 0, async_complete = 0;
    memcpy(&child_state, arm_ptr(m, rw + 0x022Cu), 4);
    memcpy(&child_complete, arm_ptr(m, rw + 0x0751u), 1);
    memcpy(&async_complete, arm_ptr(m, rw + 0x12F4u), 1);
    if (child_state != 12u || child_complete == 0 || async_complete == 0)
        return 0;

    gghjt_dump_state_impl(m, gs, "child_close_before_clear", code,
                          input_addr, input_len);

    memcpy(arm_ptr(m, rw + 0x0218u), &s->menu_entry_state, 4);
    memcpy(arm_ptr(m, rw + 0x022Cu), &s->child_download_state, 4);
    memcpy(arm_ptr(m, rw + 0x033Cu), &s->touch_x_state, 4);
    memcpy(arm_ptr(m, rw + 0x0340u), &s->touch_y_state, 4);
    memcpy(arm_ptr(m, rw + 0x00F8u), &s->request_active, 4);
    memcpy(arm_ptr(m, rw + 0x012Cu), s->request_block, sizeof(s->request_block));
    memcpy(arm_ptr(m, rw + 0x14BCu), s->async_request_block,
           sizeof(s->async_request_block));
    memcpy(arm_ptr(m, rw + 0x0854u), s->child_slot_flags, sizeof(s->child_slot_flags));
    memcpy(arm_ptr(m, rw + 0x0860u), s->child_slot_tail, sizeof(s->child_slot_tail));
    memcpy(arm_ptr(m, rw + 0x13ACu), s->input_gate_block,
           sizeof(s->input_gate_block));
    memcpy(arm_ptr(m, rw + 0x0751u), &s->child_complete_flag, 1);
    memcpy(arm_ptr(m, rw + 0x12F4u), &s->async_complete_flag, 1);
    gghjt_write_wrapper_child_event_lists(m, s);

    s->valid = 0;
    s->loader_confirmed = 0;
    s->wrapper_child_event_lists_valid = 0;
    m->primary_child_reopen_timer_needed = 1;

    /* child 关闭后立即调度 wrapper resume timer，让 cfunction.ext
     * 完成 resume 队列并重绘菜单。 */
    gghjt_on_child_close_complete(m, app_state);

    gghjt_dump_state_impl(m, gs, "child_close_after_clear", code,
                          input_addr, input_len);
    return 1;
}

/* --- timer scheduling --- */

static void gghjt_on_child_reopen_check(ArmExtModule *m, void *app_state) {
    (void)app_state;
    enum { MR_TIMER_STATE_RUNNING_LOCAL = 1 };
    if (!m || !m->primary_child_reopen_timer_needed) return;
    if (m->host_timer_pending || mr_timer_state == MR_TIMER_STATE_RUNNING_LOCAL)
        return;

    mr_timerStart(100);
    mr_timer_state = MR_TIMER_STATE_RUNNING_LOCAL;
    m->host_timer_pending = 1;
    m->timer_p_addr = m->p_addr;
    m->timer_helper_addr = m->helper_addr;
}

static void gghjt_on_child_close_complete(ArmExtModule *m, void *app_state) {
    (void)app_state;
    enum { MR_TIMER_STATE_RUNNING_LOCAL = 1 };
    if (!m || !m->p_addr || !m->helper_addr) return;
    if (m->host_timer_pending || mr_timer_state == MR_TIMER_STATE_RUNNING_LOCAL)
        return;

    mr_timerStart(50);
    mr_timer_state = MR_TIMER_STATE_RUNNING_LOCAL;
    m->host_timer_pending = 1;
    m->timer_p_addr = m->p_addr;
    m->timer_helper_addr = m->helper_addr;
}

/* --- diagnostics --- */

static void gghjt_pc_diagnostic(void *uc_raw, uint64 addr,
                                ArmExtModule *m, void *app_state) {
    (void)app_state;
    if (!getenv("VMRP_GGHJT_PC_DIAG")) return;
    uc_engine *uc = (uc_engine *)uc_raw;

    uint32_t pc = (uint32_t)addr & ~1u;
    const char *tag = NULL;
    switch (pc) {
    case 0x6570CAu: tag = "page_load_0218"; break;
    case 0x6570E4u: tag = "enter_6570e4"; break;
    case 0x6570EAu: tag = "after_65f764"; break;
    case 0x6570F0u: tag = "call_666244_2"; break;
    case 0x659200u: tag = "enter_659200"; break;
    case 0x659260u: tag = "check_65f764_alt"; break;
    case 0x6592BCu: tag = "select_gate_ok"; break;
    case 0x6592F6u: tag = "select_case0"; break;
    case 0x65930Au: tag = "call_666244_0"; break;
    case 0x659334u: tag = "select_done"; break;
    case 0x659336u: tag = "select_case0_fail"; break;
    case 0x65A2ACu: tag = "dispatch_case_to_6570e4"; break;
    case 0x65A464u: tag = "enter_65a464"; break;
    case 0x65F764u: tag = "enter_65f764"; break;
    case 0x65F772u: tag = "leave_65f764"; break;
    case 0x65F790u: tag = "enter_65f790"; break;
    case 0x65F7A0u: tag = "leave_65f790"; break;
    case 0x666244u: tag = "enter_666244"; break;
    case 0x6662CEu: tag = "call_65dc9c"; break;
    case 0x65DC9Cu: tag = "enter_65dc9c"; break;
    case 0x663070u: tag = "enter_663070"; break;
    default: return;
    }

    uint32_t rw = arm_ext_primary_rw_base_(m);
    uint32_t r0 = 0, r1 = 0, r2 = 0, lr = 0, r9 = 0;
    uc_reg_read(uc, UC_ARM_REG_R0, &r0);
    uc_reg_read(uc, UC_ARM_REG_R1, &r1);
    uc_reg_read(uc, UC_ARM_REG_R2, &r2);
    uc_reg_read(uc, UC_ARM_REG_LR, &lr);
    uc_reg_read(uc, UC_ARM_REG_R9, &r9);
    printf("GGHJT_PC %s pc=0x%X r0=0x%X r1=0x%X r2=0x%X lr=0x%X r9=0x%X "
           "menu=0x%X child=0x%X req=0x%X sel1e7=0x%02X sel1e8=0x%02X "
           "idx224=0x%X max228=0x%X mode13f8=0x%X gate13b0=0x%X gate13c8=0x%X "
           "async14bc=0x%X md=%u timerP=0x%X timerH=0x%X activeP=0x%X activeH=0x%X reopen=%d\n",
           tag, pc, r0, r1, r2, lr, r9,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x0218u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x022Cu) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x00F8u) : 0,
           rw ? arm_ext_read_u8_or_zero_(m, rw + 0x01E7u) : 0,
           rw ? arm_ext_read_u8_or_zero_(m, rw + 0x01E8u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x0224u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x0228u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x13F8u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x13B0u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x13C8u) : 0,
           rw ? arm_ext_read_u32_or_zero_(m, rw + 0x14BCu) : 0,
           m && m->primary_p_addr && arm_ptr(m, m->primary_p_addr + 12)
               ? arm_ext_read_u32_or_zero_(
                     m, arm_ext_read_u32_or_zero_(m, m->primary_p_addr + 12) + 0x34u)
               : 0,
           m->timer_p_addr, m->timer_helper_addr,
           m->active_p_addr, m->active_helper_addr,
           m->primary_child_reopen_timer_needed);
}

static void gghjt_dump_state_impl(ArmExtModule *m, GghjtState *gs,
                                  const char *tag, int32 code,
                                  uint32 input_addr, uint32 input_len) {
    (void)gs;
    if (!getenv("VMRP_GGHJT_RW_DIAG")) return;
    uint32_t rw = arm_ext_primary_rw_base_(m);
    if (!rw || !arm_ptr(m, rw + 0x14E7u)) return;

    int32_t ev[3] = {0, 0, 0};
    if (input_addr && input_len >= sizeof(ev) && arm_ptr(m, input_addr + sizeof(ev) - 1))
        memcpy(ev, arm_ptr(m, input_addr), sizeof(ev));

    uint32_t wrapper_rw = arm_ext_wrapper_rw_base_(m);
    uint32_t q = wrapper_rw ? wrapper_rw + 0x1FCu : 0;
    uint32_t ext_chunk = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + 12))
        memcpy(&ext_chunk, arm_ptr(m, m->primary_p_addr + 12), 4);

    uint8_t f751 = 0, f12f4 = 0;
    if (arm_ptr(m, rw + 0x0751u)) memcpy(&f751, arm_ptr(m, rw + 0x0751u), 1);
    if (arm_ptr(m, rw + 0x12F4u)) memcpy(&f12f4, arm_ptr(m, rw + 0x12F4u), 1);

    printf("GGHJT_RW %s code=%d ev=%d,%d,%d rw=0x%X menu=0x%X child=0x%X req=0x%X tx=0x%X ty=0x%X f751=%u f12f4=%u "
           "slot854=%08X,%08X tail860=%08X sel1e7=%02X sel1e8=%02X idx224=%08X max228=%08X mode13f8=%08X gate13b0=%08X gate13c8=%08X async14bc=%08X,%08X qh=0x%X qb=0x%X md=%u timerP=0x%X timerH=0x%X activeP=0x%X activeH=0x%X armTimer=%u hostTimer=%d pending=%d reopen=%d\n",
           tag ? tag : "?",
           (int)code, ev[0], ev[1], ev[2], rw,
           arm_ext_read_u32_or_zero_(m, rw + 0x0218u),
           arm_ext_read_u32_or_zero_(m, rw + 0x022Cu),
           arm_ext_read_u32_or_zero_(m, rw + 0x00F8u),
           arm_ext_read_u32_or_zero_(m, rw + 0x033Cu),
           arm_ext_read_u32_or_zero_(m, rw + 0x0340u),
           (unsigned)f751, (unsigned)f12f4,
           arm_ext_read_u32_or_zero_(m, rw + 0x0854u),
           arm_ext_read_u32_or_zero_(m, rw + 0x0858u),
           arm_ext_read_u32_or_zero_(m, rw + 0x0860u),
           arm_ext_read_u8_or_zero_(m, rw + 0x01E7u),
           arm_ext_read_u8_or_zero_(m, rw + 0x01E8u),
           arm_ext_read_u32_or_zero_(m, rw + 0x0224u),
           arm_ext_read_u32_or_zero_(m, rw + 0x0228u),
           arm_ext_read_u32_or_zero_(m, rw + 0x13F8u),
           arm_ext_read_u32_or_zero_(m, rw + 0x13B0u),
           arm_ext_read_u32_or_zero_(m, rw + 0x13C8u),
           arm_ext_read_u32_or_zero_(m, rw + 0x14BCu),
           arm_ext_read_u32_or_zero_(m, rw + 0x14C0u),
           q ? arm_ext_read_u32_or_zero_(m, q + 12) : 0,
           q ? arm_ext_read_u32_or_zero_(m, q + 20) : 0,
           ext_chunk ? arm_ext_read_u32_or_zero_(m, ext_chunk + 0x34u) : 0,
           m->timer_p_addr, m->timer_helper_addr,
           m->active_p_addr, m->active_helper_addr,
           arm_ext_read_u32_or_zero_(m, m->mr_timer_state_slot),
           mr_timer_state, m->host_timer_pending,
           m->primary_child_reopen_timer_needed);
}

static void gghjt_dump_state(ArmExtModule *m, void *app_state,
                             const char *tag, int32 code,
                             uint32 input_addr, uint32 input_len) {
    gghjt_dump_state_impl(m, app_state, tag, code, input_addr, input_len);
}

/* --- profile definition --- */

const AppCompatProfile app_compat_gghjt = {
    .name = "gghjt.mrp",
    .init = gghjt_init,
    .cleanup = gghjt_cleanup,
    .is_known_child = gghjt_is_known_child,
    .on_child_synced = gghjt_on_child_synced,
    .on_child_confirmed = gghjt_on_child_confirmed,
    .should_protect_got_addr = gghjt_should_protect_got_addr,
    .save_wrapper_state = gghjt_save_wrapper_state,
    .restore_wrapper_state = gghjt_restore_wrapper_state,
    .intercept_write = gghjt_intercept_write,
    .post_write_cleanup = gghjt_post_write_cleanup,
    .post_read_hook = gghjt_post_read_hook,
    .save_child_snapshot = gghjt_save_child_snapshot,
    .restore_child_if_closed = gghjt_restore_child_if_closed,
    .on_child_reopen_check = gghjt_on_child_reopen_check,
    .on_child_close_complete = gghjt_on_child_close_complete,
    .pc_diagnostic = gghjt_pc_diagnostic,
    .dump_state = gghjt_dump_state,
};
