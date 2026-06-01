#ifndef __VMRP_API_H__
#define __VMRP_API_H__

#include <stdint.h>

#ifdef _WIN32
#define VMRP_EXPORT __declspec(dllexport)
#else
#define VMRP_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* MRP event codes (same as mrporting.h) */
#define VMRP_KEY_PRESS      0
#define VMRP_KEY_RELEASE    1
#define VMRP_MOUSE_DOWN     2
#define VMRP_MOUSE_UP       3
#define VMRP_MOUSE_MOVE     12

/* MRP key codes */
#define VMRP_KEY_0          0
#define VMRP_KEY_1          1
#define VMRP_KEY_2          2
#define VMRP_KEY_3          3
#define VMRP_KEY_4          4
#define VMRP_KEY_5          5
#define VMRP_KEY_6          6
#define VMRP_KEY_7          7
#define VMRP_KEY_8          8
#define VMRP_KEY_9          9
#define VMRP_KEY_STAR       10
#define VMRP_KEY_POUND      11
#define VMRP_KEY_UP         12
#define VMRP_KEY_DOWN       13
#define VMRP_KEY_LEFT       14
#define VMRP_KEY_RIGHT      15
#define VMRP_KEY_POWER      16
#define VMRP_KEY_SOFTLEFT   17
#define VMRP_KEY_SOFTRIGHT  18
#define VMRP_KEY_SEND       19
#define VMRP_KEY_SELECT     20

/* Lifecycle */
VMRP_EXPORT int vmrp_api_init(int screen_w, int screen_h);
VMRP_EXPORT int vmrp_api_start(const char *mrp_path, const char *ext, const char *entry);
VMRP_EXPORT void vmrp_api_destroy(void);

/* Input events */
VMRP_EXPORT int vmrp_api_event(int code, int p0, int p1);

/*
 * Timer: the host is responsible for scheduling.
 * After calling vmrp_api_start() or vmrp_api_event() or vmrp_api_timer(),
 * check vmrp_api_get_timer_interval(). If > 0, schedule a call to
 * vmrp_api_timer() after that many milliseconds.
 */
VMRP_EXPORT int vmrp_api_timer(void);
VMRP_EXPORT int vmrp_api_get_timer_interval(void);

/*
 * Screen buffer: RGB565 format, row-major, size = width * height * 2 bytes.
 * The pointer remains valid until vmrp_api_destroy().
 * Call vmrp_api_get_screen_dirty() to check if the buffer has been updated
 * since the last call (it auto-clears the flag).
 */
VMRP_EXPORT const uint16_t *vmrp_api_get_screen_buffer(void);
VMRP_EXPORT int vmrp_api_get_screen_dirty(void);
VMRP_EXPORT int vmrp_api_get_screen_width(void);
VMRP_EXPORT int vmrp_api_get_screen_height(void);

/*
 * Text edit: when MRP requests text input, vmrp_api_is_edit_active()
 * returns 1. The host should display a text input UI, then call
 * vmrp_api_set_edit_text() to confirm or vmrp_api_cancel_edit() to cancel.
 */
VMRP_EXPORT int vmrp_api_is_edit_active(void);
VMRP_EXPORT int vmrp_api_set_edit_text(const char *text);
VMRP_EXPORT int vmrp_api_cancel_edit(void);

#ifdef __cplusplus
}
#endif

#endif
