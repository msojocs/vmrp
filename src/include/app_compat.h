#ifndef __VMRP_APP_COMPAT_H__
#define __VMRP_APP_COMPAT_H__

#if !defined(_TYPES_H) && !defined(_M_TYPE__)
#include "types.h"
#endif

typedef struct ArmExtModule ArmExtModule;
typedef struct AppCompatProfile AppCompatProfile;

struct AppCompatProfile {
    const char *name;

    void *(*init)(ArmExtModule *m);
    void (*cleanup)(void *app_state);

    int (*is_known_child)(ArmExtModule *m, void *app_state,
                          uint32 file_addr, uint32 file_len,
                          uint32 helper_addr);
    void (*on_child_synced)(ArmExtModule *m, void *app_state,
                            uint32 file_addr, uint32 file_len,
                            uint32 p_addr, uint32 helper_addr,
                            uint32 rw_base);
    void (*on_child_confirmed)(ArmExtModule *m, void *app_state,
                               uint32 child_p, uint32 child_helper);

    int (*should_protect_got_addr)(ArmExtModule *m, void *app_state,
                                   uint32 addr);

    void (*save_wrapper_state)(ArmExtModule *m, void *app_state);
    void (*restore_wrapper_state)(ArmExtModule *m, void *app_state);

    int (*intercept_write)(ArmExtModule *m, void *app_state,
                           uint32 fd, uint32 src_addr, uint32 len,
                           void **new_src, uint32 *new_len);
    void (*post_write_cleanup)(void *app_state);
    void (*post_read_hook)(ArmExtModule *m, void *app_state,
                           uint32 dst_addr, uint32 requested,
                           int32 actual_read, void *host_buf);

    int (*save_child_snapshot)(ArmExtModule *m, void *app_state);
    int (*restore_child_if_closed)(ArmExtModule *m, void *app_state,
                                   int32 code, uint32 input_addr,
                                   uint32 input_len);

    void (*on_child_reopen_check)(ArmExtModule *m, void *app_state);
    void (*on_child_close_complete)(ArmExtModule *m, void *app_state);

    int (*on_modal_cancel)(ArmExtModule *m, void *app_state,
                           uint32 ext_chunk, uint32 modal_depth_pre,
                           uint32 modal_depth_post,
                           int wrapper_event_routed,
                           int32 *event_data, int event_len);
    int (*clear_modal_tail)(ArmExtModule *m, void *app_state,
                            uint32 ext_chunk, uint32 game_rw,
                            uint32 timer_head);

    int (*on_printf)(ArmExtModule *m, void *app_state, const char *msg);

    void (*pc_diagnostic)(void *uc, uint64 addr, ArmExtModule *m,
                          void *app_state);
    void (*dump_state)(ArmExtModule *m, void *app_state,
                       const char *tag, int32 code,
                       uint32 input_addr, uint32 input_len);
};

const AppCompatProfile *app_compat_select(const char *pack_filename);

extern const AppCompatProfile app_compat_gghjt;
extern const AppCompatProfile app_compat_gxdzc;

#endif
