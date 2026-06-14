#include "./include/arm_ext_internal.h"
#include "./include/arm_ext_executor.h"
#include <zlib.h>

typedef struct {
    uint32_t wrapper_child_event_lists[5][2];
    int wrapper_child_event_lists_valid;
} GghjtWrapperState;

typedef struct {
    uint32_t pending_chk_arm_buf;
    uint32_t pending_chk_len;
    uint8_t *pending_chk_decomp;
    uint32_t pending_chk_decomp_len;
    GghjtWrapperState wrapper;
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
                                                  const GghjtWrapperState *s) {
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
    GghjtWrapperState *s = &gs->wrapper;
    s->wrapper_child_event_lists_valid =
        gghjt_read_wrapper_child_event_lists(m, s->wrapper_child_event_lists);
}

static void gghjt_restore_wrapper_state(ArmExtModule *m, void *app_state) {
    GghjtState *gs = app_state;
    if (!gs) return;
    gghjt_write_wrapper_child_event_lists(m, &gs->wrapper);
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

/* --- child confirmed --- */

static void gghjt_on_child_confirmed(ArmExtModule *m, void *app_state,
                                     uint32 child_p, uint32 child_helper) {
    GghjtState *gs = app_state;
    if (!gs) return;
    if (child_p && child_helper && m->primary_child_reopen_timer_needed)
        gghjt_write_wrapper_child_event_lists(m, &gs->wrapper);
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
    .pc_diagnostic = gghjt_pc_diagnostic,
};
