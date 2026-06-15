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

    void (*on_child_synced)(ArmExtModule *m, void *app_state,
                            uint32 file_addr, uint32 file_len,
                            uint32 p_addr, uint32 helper_addr,
                            uint32 rw_base);
    void (*on_child_confirmed)(ArmExtModule *m, void *app_state,
                               uint32 child_p, uint32 child_helper);

    int (*should_protect_got_addr)(ArmExtModule *m, void *app_state,
                                   uint32 addr);

    int (*intercept_write)(ArmExtModule *m, void *app_state,
                           uint32 fd, uint32 src_addr, uint32 len,
                           void **new_src, uint32 *new_len);
    void (*post_write_cleanup)(void *app_state);
    void (*post_read_hook)(ArmExtModule *m, void *app_state,
                           uint32 dst_addr, uint32 requested,
                           int32 actual_read, void *host_buf);
};

const AppCompatProfile *app_compat_select(const char *pack_filename);

extern const AppCompatProfile app_compat_gghjt;

#endif
