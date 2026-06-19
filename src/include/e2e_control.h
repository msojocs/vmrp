#ifndef __VMRP_E2E_CONTROL_H__
#define __VMRP_E2E_CONTROL_H__

#include <stdint.h>

typedef union SDL_Event SDL_Event;

typedef struct VmrpE2eControl VmrpE2eControl;

/* 输入（CLICK/KEY）现在由 e2e_control 直接注入 SDL 事件，不再走宿主回调；
 * 这里只保留确实需要宿主协助的能力：截屏与绘制计数。 */
typedef struct VmrpE2eHooks {
    int (*dump_screen_ppm)(const char *path, void *userdata);
    const char *(*screen_dump_path)(void *userdata);
    int (*draw_count)(void *userdata);
    void *userdata;
} VmrpE2eHooks;

int vmrp_e2e_control_available(void);
VmrpE2eControl *vmrp_e2e_control_create(uint32_t event_type, const VmrpE2eHooks *hooks);
void vmrp_e2e_control_start_if_requested(VmrpE2eControl *control);
void vmrp_e2e_control_execute(VmrpE2eControl *control, SDL_Event *event);
void vmrp_e2e_control_destroy(VmrpE2eControl *control);

#endif
