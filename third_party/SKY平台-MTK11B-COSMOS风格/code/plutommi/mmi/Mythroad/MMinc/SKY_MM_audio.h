/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/

#ifndef __SKY_MM_AUDIO_H__
#define __SKY_MM_AUDIO_H__

#include "l1audio.h"
#include "SKY_MM_BaseTypes.h"
#include "sky_mm_VC_BaseTypes.h"

#define AUDIO_PLAY  (1<<1)
#define AUDIO_STOP  (1<<2)
#define AUDIO_PAUSE (1<<3)

#define LEVEL1 0
#define LEVEL2 1
#define LEVEL3 2
#define LEVEL4 3
#define LEVEL5 4
#define LEVEL6 5
#define LEVEL7 6

#define MPC_AUD_VOL_MAX  5
#define MPC_AUD_VOL_MIN   0

typedef enum __tagSkyCodecAudioType
{
	SKY_AUDIO_UNKNOWN = 0,
	SKY_AUDIO_PCM,
	SKY_AUDIO_AMR,
	SKY_AUDIO_AMR_IF2,
	SKY_AUDIO_G723,
	SKY_AUDIO_G726_64,
	SKY_AUDIO_G711_A,
	SKY_AUDIO_G711_U,
	SKY_AUDIO_IMA,
	SKY_AUDIO_DIVX,
	SKY_AUDIO_MP3,
	SKY_AUDIO_AC3,
	SKY_AUDIO_AAC,
	SKY_AUDIO_DTS,
	SKY_AUDIO_VORBIS
}SkyAudioCodecType;


VCVoid splayer_aud_set_pcm_param(VCU32 sampleFreq, VCU32 bitPerSample, VCU32 isStereo);
VCI32 splayer_aud_stream_open(Media_Format audio_format,VCI32 handler, VCU8 *buffer,VCI32 buf_len);
VCI32 splayer_aud_stream_play(VCVoid);
VCI32 splayer_aud_stream_stop(VCVoid);
VCI32 splayer_aud_stream_pause(VCVoid);
VCI32 splayer_aud_stream_resume(VCVoid);
VCI32 SPlayer_aud_data_append(VCU8* data, VCU32 size);
VCVoid SPlayer_aud_Media_SetBuffer(VCU16 *buffer, VCU32 buf_len);
VCVoid SPlayer_aud_Media_GetWriteBuffer(VCU16 **buffer, VCU32 *buf_len);
VCVoid SPlayer_aud_Media_WriteDataDone(VCU32 len);
VCI32 SPlayer_aud_Media_GetFreeSpace( VCVoid );
VCVoid SPlayer_aud_print_state(VCVoid);
VCI32 SPlayer_aud_get_CurrTime(VCVoid);

VCI32 Sky_MM_Audio_GetLevelCount(VCVoid);
VCVoid Sky_MM_Audio_SetLevel(VCI32 nLevel);

VCI32 mr_media_aud_record_start(VCVoid *file_name, VCI32 format,  VCVoid *callback, VCVoid *pData);
VCI32 mr_media_aud_record_stop(VCVoid);
VCVoid mr_media_record_data_hdlr(VCVoid* msg);
VCVoid mr_media_record_data_ind(VCU16*buf, VCU32 buf_size);

#endif

