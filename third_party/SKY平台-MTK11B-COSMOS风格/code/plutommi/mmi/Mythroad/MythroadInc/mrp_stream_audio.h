/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/
// SKY_MULTIMEDIA 

#ifndef __SKY_MM_AUDIO_H__
#define __SKY_MM_AUDIO_H__

#include "mrporting.h"
#include "l1audio.h"

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


void splayer_aud_set_pcm_param(uint32 sampleFreq, uint32 bitPerSample, uint32 isStereo);
int32 splayer_aud_stream_open(Media_Format audio_format,int32 handler, uint8 *buffer,int32 buf_len);
int32 splayer_aud_stream_play(void);
int32 splayer_aud_stream_stop(void);
int32 splayer_aud_stream_pause(void);
int32 splayer_aud_stream_resume(void);

int32 splayer_aud_stream_openEx(Media_Format audio_format,int32 handler, uint8 *buffer,int32 buf_len);
int32 splayer_aud_stream_playEx(void);
int32 splayer_aud_stream_stopEx(void);
int32 splayer_aud_stream_pauseEx(void);
int32 splayer_aud_stream_resumeEx(void);

int32 SPlayer_aud_data_append(uint8* data, uint32 size);
void SPlayer_aud_Media_SetBuffer(uint16 *buffer, uint32 buf_len);

void SPlayer_aud_Media_GetWriteBuffer(uint16 **buffer, uint32 *buf_len);
void SPlayer_aud_Media_WriteDataDone(uint32 len);
int32 SPlayer_aud_Media_GetFreeSpace( void );

void SPlayer_aud_print_state(void);
int32 SPlayer_aud_get_CurrTime(void);

int32 Sky_MM_Audio_GetLevelCount(void);

void Sky_MM_Audio_SetLevel(int32 nLevel);

int32 mr_media_aud_record_start(void *file_name, int32 format,  void *callback, void *pData);
int32 mr_media_aud_record_stop(void);
void mr_media_record_data_hdlr(void* msg);
void mr_media_record_data_ind(uint16*buf, uint32 buf_size);

#endif


