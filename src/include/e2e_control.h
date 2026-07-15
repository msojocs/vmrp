#ifndef __VMRP_E2E_CONTROL_H__
#define __VMRP_E2E_CONTROL_H__

#include <stdint.h>

typedef union SDL_Event SDL_Event;

typedef struct VmrpE2eControl VmrpE2eControl;

/* 合成 SDL_KeyboardEvent 的 windowID 标记；未使用的 scancode 字段携带唯一 token。 */
#define VMRP_E2E_KEY_WINDOW_ID 0xE2E00001u

/* 输入由 e2e_control 注入 SDL 队列；这些只读 hook 让控制线程按主线程的
 * 真实 timer 状态建立完成边界，而不是用固定延迟猜测 guest 是否已调度。 */
typedef struct VmrpE2eHooks {
    int (*dump_screen_ppm)(const char *path, void *userdata);
    const char *(*screen_dump_path)(void *userdata);
    int (*draw_count)(void *userdata);
    uint32_t (*timer_arm_generation)(void *userdata);
    uint32_t (*timer_dispatched_generation)(void *userdata);
    uint32_t (*timer_pending_generation)(void *userdata);
    int (*timer_dispatch_in_progress)(void *userdata);
    int (*runtime_exited)(void *userdata);
    void *userdata;
} VmrpE2eHooks;

int vmrp_e2e_control_available(void);
VmrpE2eControl *vmrp_e2e_control_create(uint32_t event_type, const VmrpE2eHooks *hooks);
void vmrp_e2e_control_start_if_requested(VmrpE2eControl *control);
void vmrp_e2e_control_execute(VmrpE2eControl *control, SDL_Event *event);
/* main.c 只确认带匹配 token 的合成键，并报告是否实际进入 guest/编辑处理。 */
void vmrp_e2e_control_key_event_completed(VmrpE2eControl *control,
                                          uint32_t event_type,
                                          int32_t keycode,
                                          uint32_t window_id,
                                          uint32_t token,
                                          int delivered);
/* 默认短按在有效 timer 返回后，或 KEYDOWN 确认无 timer 时，由主线程同步释放。 */
int vmrp_e2e_control_take_key_up(VmrpE2eControl *control,
                                 int after_timer,
                                 int32_t *keycode,
                                 uint32_t *token);
void vmrp_e2e_control_destroy(VmrpE2eControl *control);

#endif
