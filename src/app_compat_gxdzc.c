#include "./include/arm_ext_internal.h"

extern uint16 *mr_screenBuf;
extern void mr_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h);
extern int32 mr_exit(void);

typedef struct {
    int modal_cancel_cleared;
} GxdzcState;

/* --- lifecycle --- */

static void *gxdzc_init(ArmExtModule *m) {
    (void)m;
    return calloc(1, sizeof(GxdzcState));
}

static void gxdzc_cleanup(void *app_state) {
    free(app_state);
}

/* --- printf interception --- */

static int gxdzc_on_printf(ArmExtModule *m, void *app_state, const char *msg) {
    (void)m;
    GxdzcState *gs = app_state;
    if (!gs || !gs->modal_cancel_cleared) return 0;
    if (strcmp(msg, "ht_Exit") != 0) return 0;
    gs->modal_cancel_cleared = 0;
    mr_exit();
    return 1;
}

/* --- modal cancel handling --- */

static int gxdzc_on_modal_cancel(ArmExtModule *m, void *app_state,
                                 uint32 ext_chunk, uint32 modal_depth_pre,
                                 uint32 modal_depth_post,
                                 int wrapper_event_routed,
                                 int32 *event_data, int event_len) {
    (void)ext_chunk;
    (void)modal_depth_post;
    (void)wrapper_event_routed;
    GxdzcState *gs = app_state;
    if (!gs || !m) return 0;

    int is_softright_cancel = 0;
    if (event_data && event_len >= 3)
        is_softright_cancel = (event_data[0] == 1 && event_data[1] == 18);

    int is_wrapper_close = (wrapper_event_routed &&
                            modal_depth_pre > 0 && modal_depth_post == 0 &&
                            !is_softright_cancel);

    if (!is_softright_cancel && !is_wrapper_close) return 0;

    uint32_t ec = 0, sd = 0, grw = 0;
    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr + 12))
        memcpy(&ec, arm_ptr(m, m->primary_p_addr + 12), 4);
    if (ec && arm_ptr(m, ec + 0x34))
        memcpy(&sd, arm_ptr(m, ec + 0x34), 4);

    if (is_softright_cancel && sd > 0 && ec && arm_ptr(m, ec + 0x34)) {
        uint32_t zero_depth = 0;
        memcpy(arm_ptr(m, ec + 0x34), &zero_depth, 4);
        sd = 0;
    }

    if (sd != 0) return 0;

    if (m->primary_p_addr && arm_ptr(m, m->primary_p_addr))
        memcpy(&grw, arm_ptr(m, m->primary_p_addr), 4);

    if (grw && arm_ptr(m, grw + 0x1A37)) {
        uint32_t resource_state = 0x53;
        uint8_t zero_byte = 0;
        if (arm_ptr(m, grw + 0x09F4 + 0x7F))
            memset(arm_ptr(m, grw + 0x09F4), 0, 0x80);
        memcpy(arm_ptr(m, grw + 0x03C8), &resource_state, 4);
        memcpy(arm_ptr(m, grw + 0x0388), &zero_byte, 1);
        if (arm_ptr(m, grw + 0x1A34 + 0x17))
            memset(arm_ptr(m, grw + 0x1A34), 0, 0x18);
        memcpy(arm_ptr(m, grw + 0x0DC1), &zero_byte, 1);

        if (m->modal_screen_snapshot_valid &&
            m->modal_screen_snapshot &&
            m->modal_screen_snapshot_len == m->screen_len &&
            m->screen_addr && arm_ptr(m, m->screen_addr)) {
            memcpy(arm_ptr(m, m->screen_addr), m->modal_screen_snapshot, m->screen_len);
            if (mr_screenBuf) {
                memcpy(mr_screenBuf, m->modal_screen_snapshot, m->screen_len);
                mr_drawBitmap(mr_screenBuf, 0, 0, (uint16)m->screen_w, (uint16)m->screen_h);
            }
            m->modal_screen_snapshot_valid = 0;
        }
    }

    gs->modal_cancel_cleared = 1;
    return 1;
}

/* --- clear modal tail --- */

static int gxdzc_clear_modal_tail(ArmExtModule *m, void *app_state,
                                  uint32 ext_chunk, uint32 game_rw,
                                  uint32 timer_head) {
    GxdzcState *gs = app_state;
    if (!gs || !gs->modal_cancel_cleared) return 0;

    if (game_rw && timer_head && arm_ptr(m, game_rw + GAME_TIMER_HEAD_OFFSET))
        memcpy(arm_ptr(m, game_rw + GAME_TIMER_HEAD_OFFSET), &timer_head, 4);
    if (ext_chunk && arm_ptr(m, ext_chunk + 0x34)) {
        uint32_t zero_depth = 0;
        memcpy(arm_ptr(m, ext_chunk + 0x34), &zero_depth, 4);
    }
    m->saved_game_timer_head = 0;
    m->active_helper_addr = m->primary_helper_addr;
    m->active_p_addr = m->primary_p_addr;
    return 1;
}

/* --- profile definition --- */

const AppCompatProfile app_compat_gxdzc = {
    .name = "gxdzc.mrp",
    .init = gxdzc_init,
    .cleanup = gxdzc_cleanup,
    .on_printf = gxdzc_on_printf,
    .on_modal_cancel = gxdzc_on_modal_cancel,
    .clear_modal_tail = gxdzc_clear_modal_tail,
};
