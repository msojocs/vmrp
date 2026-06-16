#ifndef __VMRP_NATIVE_DSM_FUNCS_H__
#define __VMRP_NATIVE_DSM_FUNCS_H__

#include "../mythroad/include/dsm.h"

DSM_REQUIRE_FUNCS *native_dsm_funcs_get(void);
void native_dsm_funcs_destroy(void);

int native_audio_sample_rate(void);
int native_audio_channels(void);
int native_audio_is_active(void);
int native_audio_render_s16le(void *buffer, int frames);
void native_audio_stop(void);

#endif
