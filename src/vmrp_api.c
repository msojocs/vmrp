#include "./include/vmrp_api.h"
#include "./include/vmrp.h"
#include "./include/bridge.h"
#include "./include/memory.h"

#include <stdlib.h>
#include <string.h>

static uint16_t *screen_buf = NULL;
static int screen_dirty = 0;
static int pending_timer_ms = 0;

static int edit_active = 0;
static int32_t edit_max_size = 0;
static char *hold_edit_text = NULL;

/* --- bridge.h implementations (linked by native_dsm_funcs.c) --- */

void guiDrawBitmap(uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!screen_buf) return;
    int sw = vmrp_config.screen_width;
    int sh = vmrp_config.screen_height;
    for (int32_t j = 0; j < h; j++) {
        for (int32_t i = 0; i < w; i++) {
            int32_t xx = x + i;
            int32_t yy = y + j;
            if (xx < 0 || yy < 0 || xx >= sw || yy >= sh) continue;
            screen_buf[xx + yy * sw] = bmp[xx + yy * sw];
        }
    }
    screen_dirty = 1;
}

int32_t timerStart(uint16_t t) {
    pending_timer_ms = (int)t;
    return 0;
}

int32_t timerStop(void) {
    pending_timer_ms = 0;
    return 0;
}

int32_t editCreate(const char *title, const char *text, int32_t type, int32_t max_size) {
    (void)title;
    (void)type;
    edit_active = 1;
    edit_max_size = max_size;
    if (hold_edit_text) {
        my_freeExt(hold_edit_text);
        hold_edit_text = NULL;
    }
    if (text) {
        size_t len = strlen(text);
        hold_edit_text = my_mallocExt((uint32)(len + 1));
        memcpy(hold_edit_text, text, len + 1);
    }
    return 1234;
}

int32 editRelease(int32 edit) {
    (void)edit;
    edit_active = 0;
    if (hold_edit_text) {
        my_freeExt(hold_edit_text);
        hold_edit_text = NULL;
    }
    return 0;
}

char *editGetText(int32 edit) {
    (void)edit;
    return hold_edit_text;
}

/* --- Public API --- */

VMRP_EXPORT int vmrp_api_init(int screen_w, int screen_h) {
    if (screen_w <= 0) screen_w = DEFAULT_SCREEN_WIDTH;
    if (screen_h <= 0) screen_h = DEFAULT_SCREEN_HEIGHT;
    vmrp_config.screen_width = screen_w;
    vmrp_config.screen_height = screen_h;

    free(screen_buf);
    screen_buf = (uint16_t *)calloc((size_t)(screen_w * screen_h), sizeof(uint16_t));
    if (!screen_buf) return -1;

    screen_dirty = 0;
    pending_timer_ms = 0;
    edit_active = 0;

    char *fake_argv[] = {"vmrp", NULL};
    return prepareVmrpArgs(1, fake_argv);
}

VMRP_EXPORT int vmrp_api_start(const char *mrp_path, const char *ext, const char *entry) {
    fprintf(stderr, "[vmrp_api] start('%s','%s','%s')\n", mrp_path ? mrp_path : "(null)",
            ext ? ext : "(null)", entry ? entry : "(null)");
    fflush(stderr);
    if (!mrp_path || !*mrp_path) return -1;
    if (!ext || !*ext) ext = "start.mr";

    char *argv[6];
    int argc = 0;
    argv[argc++] = "vmrp";
    argv[argc++] = (char *)mrp_path;
    argv[argc++] = (char *)ext;
    if (entry && *entry) argv[argc++] = (char *)entry;
    argv[argc] = NULL;

    fprintf(stderr, "[vmrp_api] prepareVmrpArgs...\n"); fflush(stderr);
    if (prepareVmrpArgs(argc, argv) != 0) return -1;
    fprintf(stderr, "[vmrp_api] startVmrp...\n"); fflush(stderr);
    int ret = startVmrp(argc, argv);
    fprintf(stderr, "[vmrp_api] startVmrp returned %d\n", ret); fflush(stderr);
    return ret;
}

VMRP_EXPORT void vmrp_api_destroy(void) {
    free(screen_buf);
    screen_buf = NULL;
    screen_dirty = 0;
    pending_timer_ms = 0;
    if (hold_edit_text) {
        my_freeExt(hold_edit_text);
        hold_edit_text = NULL;
    }
    edit_active = 0;
}

VMRP_EXPORT int vmrp_api_event(int code, int p0, int p1) {
    return event((int32_t)code, (int32_t)p0, (int32_t)p1);
}

VMRP_EXPORT int vmrp_api_timer(void) {
    pending_timer_ms = 0;
    return timer();
}

VMRP_EXPORT int vmrp_api_get_timer_interval(void) {
    return pending_timer_ms;
}

VMRP_EXPORT const uint16_t *vmrp_api_get_screen_buffer(void) {
    return screen_buf;
}

VMRP_EXPORT int vmrp_api_get_screen_dirty(void) {
    int d = screen_dirty;
    screen_dirty = 0;
    return d;
}

VMRP_EXPORT int vmrp_api_get_screen_width(void) {
    return vmrp_config.screen_width;
}

VMRP_EXPORT int vmrp_api_get_screen_height(void) {
    return vmrp_config.screen_height;
}

VMRP_EXPORT int vmrp_api_is_edit_active(void) {
    return edit_active;
}

VMRP_EXPORT int vmrp_api_set_edit_text(const char *text) {
    if (!edit_active) return -1;
    if (hold_edit_text) {
        my_freeExt(hold_edit_text);
        hold_edit_text = NULL;
    }
    if (text) {
        const uint8_t *utf8 = (const uint8_t *)text;
        int32_t n = 0;
        while (*utf8 && n < edit_max_size) {
            if (*utf8 < 0x80) utf8 += 1;
            else if ((*utf8 & 0xe0) == 0xc0) utf8 += 2;
            else if ((*utf8 & 0xf0) == 0xe0) utf8 += 3;
            else break;
            n++;
        }
        size_t len = (size_t)(utf8 - (const uint8_t *)text);
        hold_edit_text = my_mallocExt((uint32)(len + 1));
        memcpy(hold_edit_text, text, len);
        hold_edit_text[len] = '\0';
    }
    edit_active = 0;
    /* MR_DIALOG_KEY_OK = 0 */
    return event(6, 0, 0);
}

VMRP_EXPORT int vmrp_api_cancel_edit(void) {
    if (!edit_active) return -1;
    edit_active = 0;
    /* MR_DIALOG_KEY_CANCEL = 1 */
    return event(6, 1, 0);
}
