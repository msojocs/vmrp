#include "./include/arm_ext_internal.h"

extern int32 mr_exit(void);

typedef struct {
    int modal_exit_pending;
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
    if (!gs || !gs->modal_exit_pending) return 0;
    if (strcmp(msg, "ht_Exit") != 0) return 0;
    gs->modal_exit_pending = 0;
    mr_exit();
    return 1;
}

/* --- profile definition --- */

const AppCompatProfile app_compat_gxdzc = {
    .name = "gxdzc.mrp",
    .init = gxdzc_init,
    .cleanup = gxdzc_cleanup,
    .on_printf = gxdzc_on_printf,
};
