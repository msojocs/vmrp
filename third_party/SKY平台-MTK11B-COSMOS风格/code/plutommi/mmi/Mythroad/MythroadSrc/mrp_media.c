#ifdef __MMI_DSM_NEW__
#define __NEWSIMULATOR 

#include "MMI_features.h" 
#include "stdC.h"
#include "PixtelDataTypes.h"

#include "lcd_if.h"							/* LCD layer enable flag */
#include "lcd_sw_rnd.h"						/* for LCD size */
#include "conversions.h"
#include "lcd_sw_inc.h"
#include "MMI_features_camera.h"

#include "mrp_include.h"

#if(MTK_VERSION >=0x09B0952)
#include "GlobalConstants.h"
#include "mmi_frm_events_gprot.h"
#endif
#include "FontRes.h"
#if (__MR_CFG_VAR_MTK_VERSION__ < 0x11B1224)	
#include "TimerEvents.h"
#endif
#if (MTK_VERSION < 0x0936 )//huangsunbo 20100112
#include "EventsGprot.h"
#endif
#include "gpioInc.h"
#include "Mdi_audio.h"
#include "Mdi_datatype.h"
#include "FileManagerGProt.h"
#include "Fs_type.h"
#include "Fs_func.h"
#if (MTK_VERSION < 0x09B0952)
#include "SMSApi.h"
#endif
#include "Ems.h"
#include "DateTimeGprot.h"
#include "DateTimeType.h"
//#include "SimDetectionDef.h"
#include "SmsGuiInterfaceType.h"
#include "GlobalConstants.h"
#include "CommonScreens.h"
#include "customer_ps_inc.h"
#include "mmi_msg_context.h"
#include "wgui_categories_inputs.h"
#include "PixtelDataTypes.h"
#include "FileMgr.h"
#include "Gui_data_types.h"
#include "AudioPlayerDef.h"
#include "AudioPlayerType.h"
#include "aud_id3_parser.h"
#include "Gdi_include.h"
#include "Gdi_datatype.h"
#include "Gdi_image_bmp.h"
#include "app_ltlcom.h"
#include "stack_ltlcom.h"
#include "SSCStringHandle.h"
#include "GlobalMenuItems.h"
#include "MessagesResourceData.h"
#include "med_global.h"
#include "APP2SOC_STRUCT.H"
#include "ProtocolEvents.h"
#include "pixcomfontengine.h"
#include "l1audio.h"
#include "device.h"
#include "resource_audio.h"
#include "fat_fs.h"
#include "nvram_enums.h"
#include "nvram_struct.h"
#include "nvram_user_defs.h"
#include "nvram_data_items.h"
#include "custom_nvram_editor_data_item.h"
#include "custom_equipment.h"
#include "ScrMemMgrGprot.h"
#include "med_global.h"
#include "aud_defs.h"
#include "med_struct.h"
#include "med_api.h"
#include "med_context.h"
#include "lcd_sw_rnd.h"
#include "mdi_camera.h"
#include "cameraapp.h"
#include "mdi_camera.h"
#include "gdi_include.h"
#include "med_main.h"
#include "mdi_audio.h"
#include "mdi_include.h"
#include "app_mem_med.h"
#include "mdi_bitstream.h"

#define APM_SUPPORT  ( ( defined(MT6225) || defined(MT6228) || defined(MT6229) || defined(MT6230) || defined(MT6268T) || defined(MT6235) || defined(MT6238) || defined(MT6235B) || defined(MT6239) || defined(MT6268A) || defined(MT6268) || defined(MT6253T) || defined(MT6253) ) && !defined( MED_MODEM ) && !defined( MED_NOT_PRESENT ) )

#undef __NEWSIMULATOR
#if (MTK_VERSION >= 0x0736)//0x0808
typedef enum
{
    QUALITY_LOW,
    QUALITY_HIGH,
    NO_OF_QUALITY
}SoundRecorderQualityEnum;
#endif

#if(MTK_VERSION > 0x0848)
	#ifndef MDI_AUD_VOL_EX_MUTE
	#define MDI_AUD_VOL_EX_MUTE(x)  (x)
	#endif
#else
	#ifndef MDI_AUD_VOL_EX_MUTE
	   #define MDI_AUD_VOL_EX_MUTE(x)  ((x) | 0xC0)
	#else
	   #undef MDI_AUD_VOL_EX_MUTE
	   #define MDI_AUD_VOL_EX_MUTE(x)  ((x) | 0xC0)
	#endif
#endif

#ifndef MDI_AUD_PTH_EX
#if defined(__BTVCSR_HCI_BCHS__) && defined(__MMI_A2DP_SUPPORT__)
#define MDI_AUD_PTH_EX(x) ((x) | MDI_DEVICE_BT_HEADSET)
#else
#define MDI_AUD_PTH_EX(x)  (x)
#endif
#endif

#define DSM_MAX_VOL     5

int32 dsmReadAddr = 0;
int32 dsmWriteAddr = 0;
unsigned char  dsmLoopPCM = 0;

#if APM_SUPPORT //( defined(MT6225))
static U8  dsm_is_apm_alloc = 0;
#endif

static mr_audio_ctrl_t gDsmAudio = {0};
static U32 dsm_restore_const_seconds = 0;
static U32 dsm_restore_play_seconds =0;
static char s_dsm_vr_stream_started = FALSE;

static void dsm_play_callback( mdi_result result );
static void dsm_pause_callback( mdi_result result );
static void dsm_resume_callback( mdi_result result );
static void dsm_play_audio(void);
static void dsm_pause_audio(void);
static void dsm_stop_audio(void);
static void dsm_resume_audio(void);
static S32 dsm_play_action_without_handle_result(void);
static void dsm_audply_count_timer(void);
static S32 dsm_restore_action_without_handle_result(void);
static void dsm_start_record_play_seconds(void);
static void dsm_update_play_seconds(void);
static void dsm_clear_play_seconds(void);
static void dsm_init_audio(void);
static S32 dsm_get_audio_time(void);
static int32 dsm_audio_load(mr_audio_load_info_t *media, BOOL is_file, int32 media_type);
static S32 dsm_get_play_seconds(void);
static void dsm_set_bgplay_vol(void);
static S32 dsm_get_play_milliseconds(void);


#if APM_SUPPORT //( defined(MT6225) )
extern unsigned char *APM_Allocate_Buffer(unsigned int bufSize);
extern void APM_Release_Buffer(void);
#endif 


#if(MTK_VERSION > 0x05c)
#define DSM_PLATFORM_MAX_VOL     (MDI_AUD_VOL_EX_NUM)
#else
#define DSM_PLATFORM_MAX_VOL     (MAX_VOL_LEVEL)
#endif

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144) && defined(__BITSTREAM_API_SUPPORT__)
typedef struct {
	MDI_HANDLE handle;
} mr_pcm_stream;

static mr_pcm_stream pcm_stream;

static void pcm_stream_callback(MDI_HANDLE handle, MDI_RESULT result)
{
	mr_trace("pcm_stream_callback:handle=%d, result=%d", handle, result);

	if (result == MDI_RES_BITSTREAM_EVENT_DATA_REQUEST)
	{
		U32 actLen =0;
		kal_int32 leftLen = 0;
		kal_int32 temp =0;
		
		if(dsmReadAddr == dsmWriteAddr)
			return;

		if(dsmWriteAddr < dsmReadAddr)
		{
			leftLen= gDsmAudio.pbuf + gDsmAudio.buflen - (char*)dsmReadAddr;
			mdi_bitstream_audio_put_data(pcm_stream.handle, (U8*)dsmReadAddr, leftLen, &actLen);
			dsmReadAddr += actLen;
			if (dsmReadAddr == (int32)(gDsmAudio.pbuf + gDsmAudio.buflen))
				dsmReadAddr = (int32)gDsmAudio.pbuf;
		}

		if(dsmWriteAddr > dsmReadAddr)
		{
			leftLen= dsmWriteAddr-dsmReadAddr;
			mdi_bitstream_audio_put_data(pcm_stream.handle, (U8*)dsmReadAddr, leftLen, &actLen);
			dsmReadAddr += actLen;
		}		
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mr_media_audio_reset(void)
{
	dsmLoopPCM = 0;	
}


int32 mr_media_audio_set_vol(int32 vol)
{
	int32 temp;
	
	temp = vol > DSM_MAX_VOL?DSM_MAX_VOL:vol;

	gDsmAudio.volume = (temp*(DSM_PLATFORM_MAX_VOL-1))/DSM_MAX_VOL;
	
	if((gDsmAudio.state == MR_MEDIA_PLAYING) ||(gDsmAudio.state == MR_MEDIA_PAUSED)||(mdi_audio_get_state() ==1))
		mdi_audio_set_volume(VOL_TYPE_MEDIA, MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume));

	return MR_SUCCESS;
}


static void dsm_set_bgplay_vol(void)
{
	U8 volume = mr_media_audio_get_vol();
	mdi_audio_set_headset_mode_output_path(MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
	mdi_audio_set_volume(VOL_TYPE_MEDIA,MDI_AUD_VOL_EX_MUTE(volume));
}


int32 mr_media_audio_get_vol(void)
{
	return gDsmAudio.volume;
}


static void dsm_init_audio(void)
{	
#ifdef __MMI_FM_RADIO__
	//mmi_fmrdo_power_on( FALSE ); /*2009-04-14 */
#endif // __MMI_FM_RADIO__
	memset(&gDsmAudio,0,sizeof(mr_audio_ctrl_t));

	gDsmAudio.state = MR_MEDIA_INITED;
	gDsmAudio.volume = (4*(DSM_PLATFORM_MAX_VOL-1))/DSM_MAX_VOL;
	dsm_clear_play_seconds();
}


int32 mr_media_audio_get_state(void)
{
	return (gDsmAudio.state +MR_PLAT_VALUE_BASE);
}


static S32 dsm_get_audio_time(void)
{
	U32 time = 0;
	
	if(gDsmAudio.state < MR_MEDIA_LOADED)
		return -1;
	
	if(gDsmAudio.load_type == MR_MEDIA_FILE_LOAD)
	{
		if(mdi_audio_get_duration(gDsmAudio.filename,&time) == MDI_AUDIO_SUCCESS)
		{
#if (MTK_VERSION == 0x05c)
			if(gDsmAudio.media_format == MDI_FORMAT_M4A)
				return time/2000;
			else
#endif				
				return time/1000;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}



int32 mr_media_audio_get_total_time(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	S32 time;
	time = dsm_get_audio_time();
	
	if(time == -1)
	{
		return MR_FAILED;
	}
	else
	{
		g_mr_common_rsp.p1 = time;
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = sizeof(int32);
		return MR_SUCCESS;
	}
}


int32 mr_media_init_device(void)
{
	dsm_init_audio();
	return MR_SUCCESS;
}


static int32 dsm_audio_load(mr_audio_load_info_t *media,BOOL is_file,int32 media_type)
{
	if (media == NULL || media->buf == NULL)
	{
		return MR_FAILED;
	}
	
	if((gDsmAudio.state > MR_MEDIA_LOADED))
		return MR_FAILED;
	
	switch(media_type)
	{
		case ACI_MIDI_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_SMF;
			break;
		case ACI_WAVE_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_WAV;
			break;
		case ACI_MP3_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_DAF;
			break;
		case ACI_AMR_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_AMR;
			break;
		case ACI_AMR_WB_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_AMR_WB;
			break;
		case ACI_PCM_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_PCM_8K;
			break;
		case ACI_M4A_DEVICE:
			gDsmAudio.media_format = MDI_FORMAT_M4A;
			break;
		default:
			return MR_FAILED;
	}
#ifdef MMI_ON_HARDWARE_P
	kal_prompt_trace(MOD_MMI,"dsm_audio_load format = %d,media_type = %d",gDsmAudio.media_format,media_type);
#endif	
	gDsmAudio.state = MR_MEDIA_LOADED;
	
	if(is_file)
	{
		gDsmAudio.load_type = MR_MEDIA_FILE_LOAD;
		memset(gDsmAudio.filename,0,sizeof(gDsmAudio.filename));
		mr_fs_get_filename((char *)gDsmAudio.filename,media->buf);
		gDsmAudio.buflen  = mr_str_wstrlen((char *)gDsmAudio.filename);
		return MR_SUCCESS;
	}
	else
	{
		gDsmAudio.load_type = MR_MEDIA_BUF_LOAD;
		gDsmAudio.pbuf = media->buf;
		gDsmAudio.buflen = media->buf_len;
		return MR_SUCCESS;
	}
}


int32 mr_media_audio_load(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	switch(code /10)
	{
		case MR_MEDIA_BUF_LOAD:
		{
			return dsm_audio_load((mr_audio_load_info_t * )input, FALSE, code%10);
		}
		
		case MR_MEDIA_FILE_LOAD:
		{
			mr_audio_load_info_t load = {0};
			load.buf = (char *)input;
			load.buf_len = input_len;
			return dsm_audio_load(&load, TRUE, code%10);
		}
	}
}


static void dsm_play_audio(void)
{	
	gDsmAudio.state = MR_MEDIA_PLAYING;
	dsm_start_record_play_seconds();
       StartTimer( DSM_AUDIO_COUNT_TIMER, 500, dsm_audply_count_timer );
}


static void dsm_pause_audio(void)
{
	gDsmAudio.state = MR_MEDIA_PAUSED;
	dsm_update_play_seconds();
	StopTimer(DSM_AUDIO_COUNT_TIMER);
}


static void dsm_stop_audio(void)
{
	gDsmAudio.state = MR_MEDIA_LOADED;
	gDsmAudio.need_restore = FALSE;
	dsmLoopPCM = 0;
	StopTimer(DSM_AUDIO_COUNT_TIMER);
	dsm_clear_play_seconds();
}


static void dsm_resume_audio(void)
{	
	gDsmAudio.state = MR_MEDIA_PLAYING;
	dsm_start_record_play_seconds();
	StartTimer(DSM_AUDIO_COUNT_TIMER, 500, dsm_audply_count_timer );
}


static void dsm_play_callback( mdi_result result )
{	
	if(gDsmAudio.state != MR_MEDIA_PLAYING)
		return;
	
	switch( result )
	{
		case MDI_AUDIO_END_OF_FILE:
//#ifdef __MMI_DSM_NEW_JSKY__			
		    // 有些应用没有做上下文切换可能造成死机，如果有应用有问题的话，把传入的参数gDsmAudio.playCB设置为NULL
			if(gDsmAudio.playCB)
			{
				gDsmAudio.playCB(ACI_PLAY_COMPLETE);
				gDsmAudio.playCB = NULL;
			}
			else
			{
				mr_event(MR_AUDIO_EVENT, ACI_PLAY_COMPLETE, 0);
			}
			dsm_stop_audio();
//#endif			
			break;
		case MDI_AUDIO_TERMINATED:
			dsm_restore_const_seconds = gDsmAudio.const_seconds;
			dsm_restore_play_seconds = gDsmAudio.play_seconds;
			dsm_stop_audio();

			gDsmAudio.need_restore = FALSE;
			
			if(gDsmAudio.load_type == MR_MEDIA_FILE_LOAD)
			{
				if( mdi_audio_store_file(0) == MDI_AUDIO_SUCCESS)
				{
					gDsmAudio.need_restore = TRUE;
					gDsmAudio.state = MR_MEDIA_SUSPENDED;
				}
			}
			break;
		default:
			dsm_stop_audio();
//#ifdef __MMI_DSM_NEW_JSKY__			
		       //此处如果栈切换不到位,CB调用会造成当机.
			if(gDsmAudio.playCB)
			{
				gDsmAudio.playCB(ACI_PLAY_ERROR);
				gDsmAudio.playCB = NULL;
			}
			else
			{
				mr_event(MR_AUDIO_EVENT, ACI_PLAY_ERROR, 0);
			}
//#endif			
			
			break;
	}
}


static void dsm_pause_callback( mdi_result result )
{	
	switch( result )
	{
		case MDI_AUDIO_END_OF_FILE:
		case MDI_AUDIO_BAD_FORMAT:
		case MDI_AUDIO_TERMINATED:
			dsm_stop_audio();
			break;
		default:
			break;
	}
}


static void dsm_resume_callback( mdi_result result )
{	
	switch( result )
	{
		case MDI_AUDIO_END_OF_FILE:
		case MDI_AUDIO_BAD_FORMAT:
		case MDI_AUDIO_TERMINATED:
			dsm_stop_audio();
			break;
		default:
			break;
	}
}


int32 mr_media_audio_play_req(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	S32 result;
	mr_audio_play_info_t *playReq = NULL;
	
	if(input_len >= sizeof(mr_audio_play_info_t) && input!= NULL)
		playReq = (mr_audio_play_info_t *)input;
		
	if(gDsmAudio.state != MR_MEDIA_LOADED)
		return MR_FAILED;
	else
	{	
		gDsmAudio.play_type = DEVICE_AUDIO_PLAY_ONCE;
		gDsmAudio.playCB = NULL;
		gDsmAudio.isBlock = TRUE;
		dsmLoopPCM = 0;
		
		if(playReq)
		{
			gDsmAudio.playCB = playReq->cb;
			
			if(playReq->loop)
			{
				if(playReq->loop == 2)
					dsmLoopPCM = 1;
				
				gDsmAudio.play_type = DEVICE_AUDIO_PLAY_INFINITE;
			}
			
			if(!playReq->block)
				gDsmAudio.isBlock = FALSE;
		}

		if(gDsmAudio.load_type == MR_MEDIA_FILE_LOAD)
		{
			result = dsm_play_action_without_handle_result();
		}
		else 
		{
			dsmReadAddr = (int32)gDsmAudio.pbuf;
			
			if(gDsmAudio.isBlock)
			{
			#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
				result = mdi_audio_play_string_with_vol_path((void*)gDsmAudio.pbuf,(U32)gDsmAudio.buflen,gDsmAudio.media_format,
					                                                                     gDsmAudio.play_type,dsm_play_callback,NULL,MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume),MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));			
			#else
				result = mdi_audio_play_string_with_vol_path((void*)gDsmAudio.pbuf,(U32)gDsmAudio.buflen,gDsmAudio.media_format,
					                                                                     gDsmAudio.play_type,NULL,dsm_play_callback,MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume),MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
			#endif		                                                                
			}
			else
			{
			#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144) && defined(__BITSTREAM_API_SUPPORT__)	
				if (dsmLoopPCM)
				{
					mdi_bitstream_audio_cfg_struct cfg;

					cfg.codec_type = MDI_BITSTREAM_CODEC_TYPE_PCM;
					result = mdi_bitstream_audio_open_handle(&pcm_stream.handle, &cfg, pcm_stream_callback);
					mr_trace("mr_media_audio_play_req:result=%d", result);
					if (result == MDI_AUDIO_SUCCESS)
					{
						mdi_bitstream_audio_start_param param;
						U32 used_size = 0;

						mdi_bitstream_audio_put_data(pcm_stream.handle, gDsmAudio.pbuf, gDsmAudio.buflen - 1, &used_size);
						dsmReadAddr += used_size;
						
						param.start_time = 0;
						param.volume = MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume);
						param.audio_path = MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2);
						result = mdi_bitstream_audio_start(pcm_stream.handle, &param);
					}
					mr_trace("mr_media_audio_play_req:result=%d", result);
				}
				else
			#endif
			
			#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
				result = mdi_audio_play_string_with_vol_path_non_block((void*)gDsmAudio.pbuf,(U32)gDsmAudio.buflen,gDsmAudio.media_format,
					                                                                     gDsmAudio.play_type,dsm_play_callback,NULL,MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume),MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));			
			#else
				result = mdi_audio_play_string_with_vol_path_non_block((void*)gDsmAudio.pbuf,(U32)gDsmAudio.buflen,gDsmAudio.media_format,
					                                                                     gDsmAudio.play_type,NULL,dsm_play_callback,MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume),MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
			#endif		                                                                     
			}
		}
		
		if(result == MDI_AUDIO_SUCCESS)
		{
			dsm_play_audio();
			return MR_SUCCESS;
		}
		else
		{
			dsm_stop_audio();
			return MR_FAILED;
		}
		
	}
}


int32 mr_media_audio_pause_req(void)
{
	mdi_result result;
	
	if(gDsmAudio.state != MR_MEDIA_PLAYING)
		return MR_FAILED;

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	result = mdi_audio_pause(dsm_pause_callback, NULL);
#else
	result = mdi_audio_pause(gDsmAudio.mdi_h, dsm_pause_callback);
#endif	
	
	if( result == MDI_AUDIO_SUCCESS )
	{
		dsm_pause_audio();
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_media_audio_stop_req(void)
{
	mdi_result result;
	
	if(gDsmAudio.state == MR_MEDIA_PLAYING ||gDsmAudio.state == MR_MEDIA_PAUSED)
	{
		#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1144) && defined(__BITSTREAM_API_SUPPORT__)
		if (dsmLoopPCM)
		{
			mdi_bitstream_audio_stop(pcm_stream.handle);
			mdi_bitstream_audio_close(pcm_stream.handle);
			dsm_stop_audio();

			pcm_stream.handle = 0;
			return MR_SUCCESS;
		}
		#endif
		
		if(gDsmAudio.load_type == MR_MEDIA_FILE_LOAD)
			result = mdi_audio_stop_file();
		else
			result = mdi_audio_stop_string();
	
		if(result == MDI_AUDIO_SUCCESS)
		{
			dsm_stop_audio();
			return MR_SUCCESS;
		}
		else
		{
			return MR_FAILED;
		}
	}
	else
		return MR_FAILED;
}


int32 mr_media_audio_resume_req(void)
{
	mdi_result result;
	
	if(gDsmAudio.state != MR_MEDIA_PAUSED)
		return MR_FAILED;

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	result = mdi_audio_resume(dsm_resume_callback, NULL);
#else
	result = mdi_audio_resume(gDsmAudio.mdi_h, dsm_resume_callback);
#endif	
	
	if( result == MDI_AUDIO_SUCCESS )
	{
		dsm_resume_audio();
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
	
}


static void dsm_start_record_play_seconds(void)
{
	gDsmAudio.const_seconds += gDsmAudio.play_seconds;
	gDsmAudio.play_seconds = 0;
	kal_get_time( &gDsmAudio.play_time );
}


static S32 dsm_get_play_milliseconds(void)
{
	U32 progress;
	
	mdi_audio_get_progress_time(&progress);

	return (S32)progress;
}


int32 mr_media_audio_get_play_milliseconds(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	S32 time;
	time = dsm_get_play_milliseconds();
	
	if(time == -1)
	{
		return MR_FAILED;
	}
	else
	{
		g_mr_common_rsp.p1 = time;
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = sizeof(int32);
		return MR_SUCCESS;
	}
}


static S32 dsm_get_play_seconds(void)
{
	return (S32)((gDsmAudio.const_seconds + gDsmAudio.play_seconds) * 0.004615 + 0.5);
}


int32 mr_media_audio_get_play_seconds(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	S32 time;
	time = dsm_get_play_seconds();
	
	if(time == -1)
	{
		return MR_FAILED;
	}
	else
	{
		g_mr_common_rsp.p1 = time;
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = sizeof(int32);
		return MR_SUCCESS;
	}
}

#ifdef __MMI_DSM_NEW_JSKY__
int32 mr_media_audio_get_play_seconds_ms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	S32 time;
	time = dsm_get_audio_time_ms();
		
	if(time == -1)
	{
		return MR_FAILED;
	}
	else
	{
		g_mr_common_rsp.p1 = time;
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = sizeof(int32);
		return MR_SUCCESS;
	}
}
#endif /*__MMI_DSM_NEW_JSKY__*/

int32 mr_media_close_device(void)
{
	mr_media_audio_stop_req();
	memset(&gDsmAudio,0,sizeof(gDsmAudio));
	gDsmAudio.state = MR_MEDIA_IDLE;
	return MR_SUCCESS;
}


static void dsm_update_play_seconds(void)
{
	kal_uint32 current_time;		
	kal_get_time( &current_time );
	gDsmAudio.play_seconds = current_time - gDsmAudio.play_time;
}


static void dsm_clear_play_seconds(void)
{
	gDsmAudio.const_seconds = 0;
	gDsmAudio.play_seconds = 0;
}


static void dsm_audply_count_timer(void)
{
	
	if( gDsmAudio.state == MR_MEDIA_PLAYING )
	{
		dsm_update_play_seconds();
		//StartTimer( DSM_AUDIO_COUNT_TIMER, 500, dsm_audply_count_timer );
		gui_start_timer(500,dsm_audply_count_timer);
	}
}

static S32 dsm_play_action_without_handle_result(void)
{
	mdi_result result;	
	
	dsm_clear_play_seconds();
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	result = mdi_audio_play_file_with_vol_path( gDsmAudio.filename, gDsmAudio.play_type,NULL,
        		dsm_play_callback, NULL, MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume), MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
#else
	result = mdi_audio_play_file_with_vol_path( gDsmAudio.filename, gDsmAudio.play_type,NULL,
        		dsm_play_callback, MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume), MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
#endif


	return (S32) result;
}


void mr_media_audio_restore_play(void)
{
	S32 result = 0;
	
	if(gDsmAudio.need_restore == TRUE &&gDsmAudio.load_type == MR_MEDIA_FILE_LOAD)
	{
		gDsmAudio.need_restore = FALSE;
		gDsmAudio.const_seconds = dsm_restore_const_seconds;
		gDsmAudio.play_seconds = dsm_restore_play_seconds;

		result = dsm_restore_action_without_handle_result();

		if(result == MDI_AUDIO_SUCCESS)
		{
			dsm_play_audio();
		}
		else 
		{
			dsm_stop_audio();
		}
	}
}

static S32 dsm_restore_action_without_handle_result(void)
{
	mdi_result result;
#ifdef __MMI_AUDIO_TIME_STRETCH__
	S32 check_result;
#endif			
	{
		gDsmAudio.state = MR_MEDIA_LOADED; 
		
		/*Play Request*/
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
		result = mdi_audio_restore_file_with_vol_path( gDsmAudio.filename, gDsmAudio.play_type, NULL,
			dsm_play_callback, NULL, MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume), MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
#else
		result = mdi_audio_restore_file_with_vol_path( gDsmAudio.filename, gDsmAudio.play_type, NULL,
			dsm_play_callback, MDI_AUD_VOL_EX_MUTE(gDsmAudio.volume), MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
#endif			

#ifdef __SLT_MMI_AUDIO_BUG49__ 
		if( result == MDI_AUDIO_SUCCESS||result == MDI_AUDIO_BAD_FORMAT)
#else
		if( result == MDI_AUDIO_SUCCESS )
#endif
		{
			gDsmAudio.state = MR_MEDIA_PLAYING;
		}
	}

	return (S32) result;
}

int32 mr_media_audio_set_progress_time(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mdi_result result;
	mr_audio_position_info_t *pos = (mr_audio_position_info_t *)input;
	int32 ms = pos->pos*1000;
	
	if(gDsmAudio.load_type != MR_MEDIA_FILE_LOAD)
		return MR_FAILED;
	
	result = (mdi_result)media_aud_set_progress_time(MOD_MMI, ms);
	
	if(result == MDI_AUDIO_SUCCESS)
	{
		gDsmAudio.const_seconds = ms/4.615;
		return MR_SUCCESS;
	}
	else
		return MR_FAILED;
		
}


int32 mr_media_apm_alloc_buffer(void)
{
#ifndef WIN32
#if APM_SUPPORT //( defined(MT6225)||defined(MT6223P)||defined(MT6223)||defined(MT6223P_S00))
	APM_Allocate_Buffer(1024);
	dsm_is_apm_alloc = 1;
#endif
#endif
	return MR_SUCCESS;
}


int32 mr_media_apm_release_buffer(void)
{
#ifndef WIN32
#if APM_SUPPORT //( defined(MT6225)||defined(MT6223P)||defined(MT6223)||defined(MT6223P_S00))
	if(dsm_is_apm_alloc == 1)
	{
		APM_Release_Buffer();
		dsm_is_apm_alloc = 0;  
	}
#endif
#endif
	return MR_SUCCESS;
}


int32 mr_media_free_device(void)
{
	return MR_SUCCESS;
}


int32 mr_media_pcm_get_read_addr(int32 param)
{
	return dsmReadAddr;
}


int32 mr_media_pcm_set_write_addr(int32 param)
{
	dsmWriteAddr = param;
	return MR_SUCCESS;
}


int32 mr_media_mma_open(int32 code ,uint8 *input,int32 input_len)
{
	int ret ;
	U8 repeats = 1;
	
	mr_bg_play_info_t *req = (mr_bg_play_info_t*)input;

	if((req == NULL)
	||(input_len <sizeof(mr_bg_play_info_t))
	||(req->data == NULL)
	||(req->len==0)
	)
		return MR_FAILED;

	if(req->loop == 1)
		repeats = 0;
		
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	switch(code%10)
	{
		case ACI_WAVE_DEVICE:
			ret = mdi_audio_mma_open_string(0, req->data, req->len, MDI_FORMAT_WAV, repeats, NULL, NULL);
			break;
		case ACI_MIDI_DEVICE:
			ret = mdi_audio_mma_open_string(0, req->data, req->len, MDI_FORMAT_SMF, repeats, NULL, NULL);
			break;
		default:
			return MR_FAILED;
	}
#else		
	switch(code%10)
	{
		case ACI_WAVE_DEVICE:
			ret = mdi_audio_mma_open_wav((void *)req->data, req->len, repeats,NULL);
			break;
		case ACI_MIDI_DEVICE:
			ret = mdi_audio_mma_open_midi((void *)req->data, req->len, repeats,NULL);
			break;
		default:
			return MR_FAILED;
	}
#endif
	
	if(ret == 0)
		return MR_FAILED;
	else
		return (ret+MR_PLAT_VALUE_BASE);
}


int32 mr_media_mma_play(int32 code ,uint8 *input,int32 input_len)
{				
	dsm_set_bgplay_vol();
	
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	mdi_audio_mma_play(*((int32 *)input)-MR_PLAT_VALUE_BASE);
#else
	switch(code%10)
	{
		case ACI_WAVE_DEVICE:
		{
			mdi_audio_mma_play_wav(*((int32 *)input)-MR_PLAT_VALUE_BASE);
			break;
		}
		case ACI_MIDI_DEVICE:
		{
			mdi_audio_mma_play_midi(*((int32 *)input)-MR_PLAT_VALUE_BASE);
			break;
		}
		default:
			return MR_FAILED;
	}
#endif	
	return MR_SUCCESS;
}


int32 mr_media_mma_stop(int32 code ,uint8 *input,int32 input_len)
{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	mdi_audio_mma_stop(*((int32 *)input)-MR_PLAT_VALUE_BASE);
#else
	switch(code%10)
	{
		case ACI_WAVE_DEVICE:
			mdi_audio_mma_stop_wav(*((int32 *)input)-MR_PLAT_VALUE_BASE);
			break;
		case ACI_MIDI_DEVICE:
			mdi_audio_mma_stop_midi(*((int32 *)input)-MR_PLAT_VALUE_BASE);
			break;
		default:
			return MR_FAILED;
	}
#endif	
	return MR_SUCCESS;
}

int32 mr_media_mma_close(int32 code ,uint8 *input,int32 input_len)
{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	mdi_audio_mma_close(*((int32 *)input)-MR_PLAT_VALUE_BASE);
#else
	switch(code%10)
	{
		case ACI_WAVE_DEVICE:
			mdi_audio_mma_close_wav(*((int32 *)input)-MR_PLAT_VALUE_BASE);
			break;
		case ACI_MIDI_DEVICE:

			mdi_audio_mma_close_midi(*((int32 *)input)-MR_PLAT_VALUE_BASE);
			break;
		default:
			return MR_FAILED;
	}
#endif	
	return MR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

/* camera function */
/* you can adjust the value */
#define DSM_DEFAULT_ZOOM_STEP 8

static mr_camera_ctrl_t gDsmCameraReq ={0};

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_SCREEN_ROTATE__)
extern U8 current_screen_rotation;
#endif


extern pBOOL mmi_usb_is_in_mass_storage_mode(void);


static void dsm_camera_init_tmp_info(void)
{
	char asciiPath[32] = {0};
	sprintf(asciiPath,"%c:\\dsm_capture.jpg", MMI_PUBLIC_DRV);
	mmi_asc_to_ucs2((S8 *)gDsmCameraReq.tmpName, (S8 *)asciiPath);
}

static void dsm_camera_uninit_tmp_info(void)
{
}


static void dsm_camera_uninit(void)
{
	//FS_Delete((WCHAR *)gDsmCameraReq.tmpName);
	memset(&gDsmCameraReq, 0, sizeof(gDsmCameraReq));
	gDsmCameraReq.state = DSM_CAMERA_UNINIT;
}



// SKY_MULTIMEDIA 修改获取camera数据格式不对导致绿屏的问题
#if defined(MT6252) || defined(MT6255) || defined(MT6250) 
#include "med_utility.h"
unsigned char *gDsmCamBuffer = NULL;
#ifdef __MED_IN_ASM__
#define med_alloc_ext_mem(size) applib_asm_alloc_anonymous_nc(size)
#define med_alloc_ext_mem_cacheable(size) applib_asm_alloc_anonymous(size)
#define med_free_ext_mem(ptr) applib_asm_free_anonymous(*ptr)
#endif
#endif
static int32 dsm_camera_get_preview_data(mr_layer_info_t *layer)
{
	if (gDsmCameraReq.state == DSM_CAMERA_PREVIEW && layer != NULL)
	{
		gdi_layer_push_and_set_active(gDsmCameraReq.preview_layer);
		gdi_layer_get_active((gdi_handle *)&layer->handle);
		gdi_layer_get_dimension((S32 *)&layer->w, (S32 *)&layer->h);
		gdi_layer_get_buffer_ptr((U8 **)&layer->buffer);

	// SKY_MULTIMEDIA 修改获取camera数据格式不对导致绿屏的问题
	#if defined(MT6252) || defined(MT6255) || defined(MT6250) 
		if (GDI_COLOR_FORMAT_UYVY422 == gdi_act_layer->cf)//rwei
		{
			if (!gDsmCamBuffer)
			{
				gDsmCamBuffer = (unsigned char *)med_alloc_ext_mem(gdi_act_layer->width*gdi_act_layer->height*2);
				kal_prompt_trace(MOD_MMI,"[YUZF] malloc gDsmCamBuffer %x", gDsmCamBuffer);
			}
			if (gDsmCamBuffer)
			{
				iul_csc_uyvy422_to_rgb565_fxp_dithering(
				    gdi_act_layer->buf_ptr, gDsmCamBuffer,  gdi_act_layer->width, gdi_act_layer->height);
				layer->buffer = gDsmCamBuffer;
			}
			else
			{
				iul_csc_uyvy422_to_rgb565_fxp_dithering(
				    gdi_act_layer->buf_ptr, gdi_act_layer->buf_ptr,  gdi_act_layer->width, gdi_act_layer->height);
			}
		 
		}
	#endif
		gdi_layer_pop_and_restore_active();
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


static U16 dsm_camera_preview_get_zoom_factor(void)
{
	return gDsmCameraReq.zoom_index*gDsmCameraReq.zoom_steps;
}


static void dsm_camera_preview_start(void)
{
#if ((!defined(MT6223P))&&(!defined(MT6223))&&(!defined(MT6223P_S00))) 

	extern U8 camera_image_qty_command_map[];
	mdi_camera_setting_struct camera_setting_data = {0};
	mdi_camera_preview_struct	preview_setting_data = {0};

	mdi_camera_load_default_setting(&camera_setting_data);

	camera_setting_data.zoom = dsm_camera_preview_get_zoom_factor();
	//camera_setting_data.image_qty			= camera_image_qty_command_map[gDsmCameraReq.pre_req.img_qt];
	camera_setting_data.image_qty			= gDsmCameraReq.pre_req.img_qt;
	camera_setting_data.image_width		= gDsmCameraReq.pre_req.img_w;
	camera_setting_data.image_height		= gDsmCameraReq.pre_req.img_h;	
	camera_setting_data.lcm					= MDI_CAMERA_PREVIEW_LCM_MAINLCD;
#if !(__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)	
	camera_setting_data.lcm_rotate = MDI_CAMERA_LCM_ROTATE_0;
	camera_setting_data.preview_rotate = MDI_CAMERA_PREVIEW_ROTATE_0;
	camera_setting_data.continue_capture = gDsmCameraReq.continue_capture;	
#endif	

#if defined(HORIZONTAL_CAMERA) && defined(__MDI_CAMERA_HW_ROTATOR_BY_IDP__)
    camera_setting_data.preview_width = gDsmCameraReq.pre_req.preview_h;;
    camera_setting_data.preview_height = gDsmCameraReq.pre_req.preview_w;    
#else /*  defined(HORIZONTAL_CAMERA) && defined(__MDI_CAMERA_HW_ROTATOR_BY_IDP__) */
    camera_setting_data.preview_width = gDsmCameraReq.pre_req.preview_w;
    camera_setting_data.preview_height = gDsmCameraReq.pre_req.preview_h;;
#endif /*  defined(HORIZONTAL_CAMERA) && defined(__MDI_CAMERA_HW_ROTATOR_BY_IDP__) */

#if defined(HORIZONTAL_CAMERA) && defined(__MDI_CAMERA_HW_ROTATOR_SUPPORT__)
	mdi_camera_set_hw_rotator(MDI_CAMERA_PREVIEW_ROTATE_90);
#endif /*  defined(HORIZONTAL_CAMERA) && defined(__MDI_CAMERA_HW_ROTATOR_SUPPORT__) */

	preview_setting_data.blt_layer_flag = GDI_LAYER_ENABLE_LAYER_0|GDI_LAYER_ENABLE_LAYER_1;
	preview_setting_data.is_lcd_update = TRUE;
	preview_setting_data.is_tvout = FALSE;
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	preview_setting_data.preview_layer_flag = gDsmCameraReq.is_from_jsky ? GDI_LAYER_ENABLE_LAYER_1 : GDI_LAYER_ENABLE_LAYER_0;
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	preview_setting_data.preview_layer_handle = gDsmCameraReq.preview_layer;
#if !(__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)	
	preview_setting_data.preview_wnd_offset_x = gDsmCameraReq.pre_req.preview_x;
	preview_setting_data.preview_wnd_offset_y = gDsmCameraReq.pre_req.preview_y;
	preview_setting_data.preview_wnd_width = gDsmCameraReq.pre_req.preview_w;
	preview_setting_data.preview_wnd_height = gDsmCameraReq.pre_req.preview_h;
#endif	
	preview_setting_data.src_key_color = GDI_COLOR_TRANSPARENT;	

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	gDsmCameraReq.last_error = mdi_camera_preview_start(&preview_setting_data, &camera_setting_data, NULL);
#else
	gDsmCameraReq.last_error = mdi_camera_preview_start(&preview_setting_data, &camera_setting_data);
#endif	
	
#ifdef MMI_ON_HARDWARE_P		   
	kal_prompt_trace(MOD_MMI,"gDsmCameraReq.last_error =%d",gDsmCameraReq.last_error);
#endif

#endif
}


static void dsm_camera_free_osd_resource(void)
{
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	gdi_handle baseHandler;
	
   	mr_trace("dsm_camera_free_osd_resource, gDsmCameraReq.osd_layer = 0X%08x", gDsmCameraReq.osd_layer);

   	gDsmCameraReq.osd_layer = GDI_LAYER_EMPTY_HANDLE;
   	gDsmCameraReq.osd_layer_buffer = NULL;
  
	gdi_layer_restore_base_active();
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	// SKY_MULTIMEDIA 修改获取camera数据格式不对导致绿屏的问题
#if defined(MT6252) || defined(MT6255) || defined(MT6250) 
	if (gDsmCamBuffer)//rwei
	{
		med_free_ext_mem((void **)&gDsmCamBuffer);
		gDsmCamBuffer = NULL;
	}
#endif
}


static BOOL dsm_camera_create_osd_resource(void)
{
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	gdi_layer_get_base_handle(&gDsmCameraReq.osd_layer);
	gdi_layer_push_and_set_active(gDsmCameraReq.osd_layer);
	gdi_layer_get_buffer_ptr((U8 **)&gDsmCameraReq.osd_layer_buffer);
	if (!gDsmCameraReq.is_from_jsky)
	{
		gdi_layer_set_source_key(TRUE, GDI_COLOR_TRANSPARENT);
		gdi_draw_solid_rect(
			gDsmCameraReq.pre_req.preview_x,
			gDsmCameraReq.pre_req.preview_y,
			gDsmCameraReq.pre_req.preview_x + gDsmCameraReq.pre_req.preview_w - 1,
			gDsmCameraReq.pre_req.preview_y + gDsmCameraReq.pre_req.preview_h - 1,
			GDI_COLOR_TRANSPARENT);
	}
	gdi_layer_pop_and_restore_active();
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	return TRUE;
}


static void dsm_camera_free_preview_resource(void)
{
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
   	mr_trace("dsm_camera_free_preview_resource gDsmCameraReq.preview_layer = 0X%08x", gDsmCameraReq.preview_layer);

	if (gDsmCameraReq.preview_layer != GDI_LAYER_EMPTY_HANDLE)
	{
		gdi_layer_free(gDsmCameraReq.preview_layer);
		gDsmCameraReq.preview_layer	= GDI_LAYER_EMPTY_HANDLE;
		applib_asm_free_anonymous((void*)gDsmCameraReq.preview_layer_buffer);
		gDsmCameraReq.preview_layer_buffer = NULL;
	}
	/* __MMI_DSM_NEW_JSKY__ end support camera */
}


static void dsm_camera_create_preview_resource(void)
{
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	U8 *buf_ptr;
	int32 buf_size;

	buf_size = ((gDsmCameraReq.pre_req.preview_w*gDsmCameraReq.pre_req.preview_h*GDI_MAINLCD_BIT_PER_PIXEL)>>3);
	gDsmCameraReq.preview_layer_buffer = applib_asm_alloc_anonymous_nc(buf_size);

	mr_trace("dsm_camera_create_preview_resource, pre_w = %d, pre_h = %d, buf_size = %d, ", 
			gDsmCameraReq.pre_req.preview_w, gDsmCameraReq.pre_req.preview_h, buf_size);

	gdi_layer_create_using_outside_memory(
		gDsmCameraReq.pre_req.preview_x,
		gDsmCameraReq.pre_req.preview_y,
		gDsmCameraReq.pre_req.preview_w,
		gDsmCameraReq.pre_req.preview_h,
		&gDsmCameraReq.preview_layer,
		(PU8)gDsmCameraReq.preview_layer_buffer, 
		buf_size);

	gdi_layer_push_and_set_active(gDsmCameraReq.preview_layer);
	gdi_layer_clear(GDI_COLOR_BLACK);
#ifdef __MDI_DISPLAY_YUYV422_SUPPORT__
    gdi_layer_set_color_format(GDI_COLOR_FORMAT_UYVY422);
#endif	
    gdi_layer_set_hw_update_layer(gDsmCameraReq.preview_layer, KAL_FALSE);
#ifdef HORIZONTAL_CAMERA
	if (gDsmCameraReq.capsize != MDI_CAMERA_CAP_SIZE_WALLPAPER)
	{
   		gdi_layer_set_rotate(GDI_LAYER_ROTATE_90);
    }
#endif
	gdi_layer_pop_and_restore_active();
	/* __MMI_DSM_NEW_JSKY__ end support camera */
}


static void dsm_camera_preview_set_blt_layer(BOOL is_previewing)
{
#if ((!defined(MT6223P))&&(!defined(MT6223))&&(!defined(MT6223P_S00))) 

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
#ifdef MMI_ON_HARDWARE_P
	kal_prompt_trace(MOD_MMI,"dsm_camera_preview_set_blt_layer");
	kal_prompt_trace(MOD_MMI,"gDsmCameraReq.preview_layer = %d",gDsmCameraReq.preview_layer);
	kal_prompt_trace(MOD_MMI,"gDsmCameraReq.base_layer = %d",gDsmCameraReq.base_layer);
	kal_prompt_trace(MOD_MMI,"gDsmCameraReq.osd_layer = %d",gDsmCameraReq.osd_layer);
#endif

	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	if (is_previewing)
	{
		if (!gDsmCameraReq.is_from_jsky)
		{
			gdi_layer_set_blt_layer(gDsmCameraReq.preview_layer, gDsmCameraReq.osd_layer, 0, 0);
		}
		else
		{
			gdi_layer_set_blt_layer(gDsmCameraReq.osd_layer, gDsmCameraReq.preview_layer, 0, 0);
		}
	}
	else
	{
		gdi_layer_set_blt_layer(gDsmCameraReq.base_layer, 0, 0, 0);
	}
	/* __MMI_DSM_NEW_JSKY__ end support camera */
#endif
}


static void dsm_camera_entry_preview(void)
{
#if ((!defined(MT6223P))&&(!defined(MT6223))&&(!defined(MT6223P_S00))) 
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	if (!gDsmCameraReq.is_from_jsky)
	{
		gdi_layer_push_and_set_active(gDsmCameraReq.base_layer);
		//gdi_layer_clear_background(GDI_COLOR_BLACK);
		gdi_layer_pop_and_restore_active();
	}
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	dsm_camera_preview_set_blt_layer(FALSE);
	gdi_layer_blt_previous(0, 0, UI_device_width-1,UI_device_height-1);
	dsm_camera_free_preview_resource();
	dsm_camera_create_preview_resource();
	dsm_camera_preview_set_blt_layer(TRUE);
	dsm_camera_preview_start();
	gDsmCameraReq.state = DSM_CAMERA_PREVIEW;
#endif
}


static void dsm_camera_exit(void)
{
	gdi_handle baseHandle;
	extern pBOOL IsBackHistory;	 
	
/*保证不会出现重入mythroad的情况********/
	if(gDsmCameraReq.state != DSM_CAMERA_UNINIT)
		mr_event(MR_OSD_EVENT, NULL, NULL);
/****************************************************/	

	if(gDsmCameraReq.state == DSM_CAMERA_CAPTURED)
		gDsmCameraReq.state = DSM_CAMERA_PAUSED;
	
	if (internalCovered == MR_FALSE)
	{
		if((mr_app_get_state() == DSM_RUN))
		{		
			mr_app_pause();
		}
	}
	else
	{
		internalCovered = MR_FALSE;
	}	

	// SKY_MULTIMEDIA 修改打开camera交叉事件偶现花屏问题
	mdi_camera_preview_stop();
	
	mdi_camera_power_off();

	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	if (!gDsmCameraReq.is_from_jsky)
	{
		gdi_layer_get_base_handle(&baseHandle);
		gdi_layer_push_and_set_active(baseHandle);
		gdi_layer_clear(GDI_COLOR_BLACK);
		gdi_layer_pop_and_restore_active();
	}
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	dsm_camera_uninit_tmp_info();

	/* resume alignment timer */
	UI_enable_alignment_timers();

	/* resume LED patten */
	//GpioCtrlReq(MMIAPI_DEV_CTRL_RELEASE);

	mdi_audio_resume_background_play();
	/* let MMI can sleep */
	//TurnOffBacklight();

	/* enable multi-layer */
	gdi_layer_multi_layer_disable();	

	dsm_camera_free_preview_resource();
	dsm_camera_free_osd_resource();
	entry_full_screen();
	
}


static void dsm_camera_entry(void)
{	
	internalCovered = MR_TRUE;
	mr_app_EntryNewScreen(SCR_DSM_DUMMY_199, dsm_camera_exit, dsm_camera_entry, MMI_FRM_FULL_SCRN);
	gDsmCameraReq.camera_handler = SCR_DSM_DUMMY_199;
	internalCovered = MR_FALSE;
	
	mr_app_setup_dsm_screen_event_hdlr();
	entry_full_screen();

/* __MMI_DSM_NEW_JSKY__ begin support camera */
#if defined(__MMI_SCREEN_ROTATE__)
	if (current_screen_rotation != MMI_FRM_SCREEN_ROTATE_0)
	{
		mmi_frm_screen_rotate((mmi_frm_screen_rotate_enum)current_screen_rotation);
	}
#endif
/* __MMI_DSM_NEW_JSKY__ end support camera */

	dsm_camera_init_tmp_info();

	// pause the background audio 
	mdi_audio_suspend_background_play();
	// stop mmi sleep
	//TurnOnBacklight(0);
	//prohibit the keypad tone
	mmi_frm_kbd_set_tone_state(MMI_KEY_TONE_DISABLED);

	dsm_camera_free_osd_resource();
	dsm_camera_create_osd_resource();
	
	if ((gDsmCameraReq.state != DSM_CAMERA_PAUSED) && (gDsmCameraReq.state != DSM_CAMERA_CAPTURED))
	{
		//mdi_camera_set_camera_id(MDI_CAMERA_MAIN);
		gDsmCameraReq.last_error = mdi_camera_power_on(FRAMEWORK_ASM);
		mr_trace("dsm_camera_entry 1 =%d", gDsmCameraReq.last_error);
		dsm_camera_entry_preview();
	}
		
	if (mr_app_get_state() == DSM_PAUSE)
	{
		mr_app_resume();
		mr_event(MR_OSD_EVENT,(int32)gDsmCameraReq.osd_layer_buffer,NULL);
	}
	
}


static void dsm_camera_adjust(uint32 img_w, uint32 img_h)
{
	U16 capsize = MDI_CAMERA_CAP_SIZE_OTHER;
	U16 temp_width = 0;
	U16 temp_height = 0;
	U16 temp_factor = 0;
	U16 lcd_width = 0;
	U16 lcd_height = 0;
	U16 cam_id = MDI_CAMERA_MAIN;
	BOOL isRot = FALSE;

	if (img_w <= LCD_WIDTH && img_h <= LCD_HEIGHT ||
		img_w <= LCD_HEIGHT && img_h <= LCD_WIDTH)
	{
		capsize = MDI_CAMERA_CAP_SIZE_WALLPAPER;
	}
			
#ifdef HORIZONTAL_CAMERA
	if (cam_id == MDI_CAMERA_MAIN)
	{
		if (capsize == MDI_CAMERA_CAP_SIZE_WALLPAPER)
		{
			lcd_width = LCD_HEIGHT;
			lcd_height = LCD_WIDTH;
			isRot = TRUE;
		}
		else
		{
			lcd_width = LCD_HEIGHT;
			lcd_height = LCD_WIDTH;
			isRot = FALSE;
		}
	}
	else
#endif
	{
		lcd_width = LCD_WIDTH;
		lcd_height = LCD_HEIGHT;
		isRot = FALSE;					  
	}		 

	if (capsize == MDI_CAMERA_CAP_SIZE_WALLPAPER)
	{
	#ifdef HORIZONTAL_CAMERA        
		if (cam_id == MDI_CAMERA_MAIN)
		{
			temp_width = LCD_HEIGHT;
			temp_height = LCD_WIDTH;				
		}
		else
	#endif        
		{
			temp_width = LCD_WIDTH;
			temp_height = LCD_HEIGHT;	   
		}
	}
	else
	{
		temp_factor = (lcd_height / 12);
		temp_width = temp_factor * 16;
		while (temp_width > lcd_width)
		{
			temp_factor--;
			temp_width = temp_factor * 16;
		}
		temp_height = temp_factor * 12;
	}

	if(!isRot)
	{
		gDsmCameraReq.pre_req.preview_w = temp_width;
		gDsmCameraReq.pre_req.preview_h = temp_height;
		gDsmCameraReq.pre_req.preview_x = (lcd_width - temp_width)/2;
		gDsmCameraReq.pre_req.preview_y = (lcd_height - temp_height)/2;
	}			 
	else
	{
		gDsmCameraReq.pre_req.preview_w = temp_height;
		gDsmCameraReq.pre_req.preview_h = temp_width;
		gDsmCameraReq.pre_req.preview_x = (lcd_height - temp_height)/2;
		gDsmCameraReq.pre_req.preview_y = (lcd_width - temp_width)/2;
	}

	gDsmCameraReq.capsize = capsize;
}


static int32 dsm_camera_create(mr_camera_launch_info_t *pReq, int32 *handler, int32* osd_buffer)
{	
	if (handler == NULL || osd_buffer == NULL)
	{
		return MR_FAILED;
	}

	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	if (gDsmCameraReq.state != DSM_CAMERA_INITED)
	{
		return MR_FAILED;
	}

	if ((pReq->img_qt & 0x0000ffff) > DSM_CAMEAR_QT_HIGHT)
	{
		return MR_FAILED;
	}
	
	gDsmCameraReq.is_from_jsky = (int32)((pReq->img_qt & 0xffff0000) != 0);

#ifdef __USB_IN_NORMAL_MODE__
	if (mmi_usb_is_in_mass_storage_mode())
	{
		return MR_FAILED;
	}
#endif

	gdi_layer_get_base_handle(&gDsmCameraReq.base_layer);
		
	memcpy(&gDsmCameraReq.pre_req, pReq, sizeof(mr_camera_launch_info_t));

	gDsmCameraReq.pre_req.img_qt = (uint32)(gDsmCameraReq.pre_req.img_qt & 0x0000ffff);
	/* __MMI_DSM_NEW_JSKY__ end support camera */

#if defined(__CAMERA_OSD_HORIZONTAL__) 
	gDsmCameraReq.req_x = gDsmCameraReq.pre_req.preview_x;
	gDsmCameraReq.req_y = gDsmCameraReq.pre_req.preview_y;
	gDsmCameraReq.req_w = gDsmCameraReq.pre_req.preview_w;
	gDsmCameraReq.req_h = gDsmCameraReq.pre_req.preview_h;
	gDsmCameraReq.pre_req.preview_x = 0;
	gDsmCameraReq.pre_req.preview_y = 0;
	gDsmCameraReq.pre_req.preview_w = LCD_HEIGHT;
	gDsmCameraReq.pre_req.preview_h = LCD_WIDTH;

	if (gDsmCameraReq.req_x != 0 || gDsmCameraReq.req_y != 0)
	{
		gDsmCameraReq.pre_req.img_w = LCD_WIDTH;
		gDsmCameraReq.pre_req.img_h = LCD_HEIGHT;
	}
#endif	

	gDsmCameraReq.capsize = MDI_CAMERA_CAP_SIZE_WALLPAPER;
	/* 不能这样简单调整，大小和位置不能改变*/
	/* 视频聊天有这个需求的 */
	//dsm_camera_adjust(gDsmCameraReq.pre_req.img_w, gDsmCameraReq.pre_req.img_h);
	
	dsm_camera_entry();

	*handler = gDsmCameraReq.camera_handler;
	*osd_buffer = (int32)gDsmCameraReq.osd_layer_buffer;
	
	return MR_SUCCESS;
}


static mmi_ret dsm_camera_capture_cb_post_evt(mmi_event_struct *param)
{
	dsm_camera_preview_start();
	if (gDsmCameraReq.last_error == MDI_RES_CAMERA_SUCCEED)
	{
		gDsmCameraReq.state = DSM_CAMERA_PREVIEW;
	}
}


static int32 dsm_camera_capture(uint8 flag,int32 *ptr,int32 *size)
{
	void *capture_data;
	uint32 capture_data_size;

	if (size == NULL || ptr == NULL)
	{
		return MR_FAILED;
	}

	if (gDsmCameraReq.state != DSM_CAMERA_PREVIEW)
	{
		return MR_FAILED;
	}
	
	mr_trace("gDsmCameraReq.pre_req.img_w=%d",gDsmCameraReq.pre_req.img_w);
	mr_trace("gDsmCameraReq.pre_req.img_h=%d",gDsmCameraReq.pre_req.img_h);
	mr_trace("gDsmCameraReq.tmpName=%s",gDsmCameraReq.tmpName);
	gDsmCameraReq.last_error = mdi_camera_capture_to_memory(
								(U8**)&capture_data, 
								&capture_data_size, 
								(S8*)gDsmCameraReq.tmpName);
	mr_trace("capture gDsmCameraReq.last_error=%d",gDsmCameraReq.last_error);
	if (gDsmCameraReq.last_error != MDI_RES_CAMERA_SUCCEED)
	{
		return MR_FAILED;
	}

	mr_trace("capture_data_size=%d", capture_data_size);
#if 0	// 只CAPTURE到内存，不保存文件
	gDsmCameraReq.last_error = mdi_camera_save_captured_image();
	mr_trace("save gDsmCameraReq.last_error=%d",gDsmCameraReq.last_error);
	if (gDsmCameraReq.last_error != MDI_RES_CAMERA_SUCCEED)
	{
		return MR_FAILED;
	}
#endif				
	gDsmCameraReq.state = DSM_CAMERA_CAPTURED;

	*ptr = (int32)gDsmCameraReq.pImg = (int32)capture_data;
	*size = gDsmCameraReq.ImgSize = (int32)capture_data_size;

	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	if (flag == 1)
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	{
		mmi_event_struct evt;

		MMI_FRM_INIT_EVENT(&evt, 0);
		MMI_FRM_POST_EVENT(&evt, dsm_camera_capture_cb_post_evt, NULL);
	}

	return MR_SUCCESS;
}


int32 mr_camera_init(int32 param)
{
	U16 zoom_steps;
	mdi_camera_zoom_info zoom_info;
	
	memset(&gDsmCameraReq, 0, sizeof(gDsmCameraReq));
	gDsmCameraReq.state = DSM_CAMERA_INITED;
	gDsmCameraReq.last_error = MDI_RES_CAMERA_SUCCEED;
	mdi_camera_query_zoom_info(&zoom_info);
	gDsmCameraReq.zoom_index = 0;
	gDsmCameraReq.zoom_max = DSM_DEFAULT_ZOOM_STEP;
	gDsmCameraReq.zoom_steps = zoom_info.zoom_steps/DSM_DEFAULT_ZOOM_STEP;

	mr_trace("max_zoom_factor = %d, zoom_steps = (%d, %d)", 
		zoom_info.max_zoom_factor, zoom_info.zoom_steps, gDsmCameraReq.zoom_steps);
	return MR_SUCCESS;
}


int32 mr_camera_get_zoom_max(int32 param)
{
	return gDsmCameraReq.zoom_max + MR_PLAT_VALUE_BASE;
}


int32 mr_camera_zoom_in(int32 param)
{
	U16 zoom_factor;
	U32 new_zoom_factor = 100;
	
	mr_trace("mr_camera_zoom_in, gDsmCameraReq.state = %d, zoom_index = %d", 
		gDsmCameraReq.state, gDsmCameraReq.zoom_index);

	if (gDsmCameraReq.state != DSM_CAMERA_PREVIEW)
	{
		return MR_FAILED;
	}
		
	if (gDsmCameraReq.zoom_index < gDsmCameraReq.zoom_max)
	{			
		gDsmCameraReq.zoom_index += 1;

		zoom_factor = dsm_camera_preview_get_zoom_factor();	

		mdi_camera_update_para_zoom(zoom_factor);
		mdi_camera_get_fast_zoom_factor(&new_zoom_factor);
		mr_trace("mr_camera_zoom_in, new_zoom_factor = %d", new_zoom_factor);
	}
	
	return MR_SUCCESS;
}


int32 mr_camera_zoom_out(int32 param)
{
	U16 zoom_factor;
	U32 new_zoom_factor = 100;

	mr_trace("mr_camera_zoom_out, gDsmCameraReq.state = %d", gDsmCameraReq.state);

	if (gDsmCameraReq.state != DSM_CAMERA_PREVIEW)
	{
		return MR_FAILED;
	}
		
	if (gDsmCameraReq.zoom_index > 0)
	{
		gDsmCameraReq.zoom_index -= 1;
		
		zoom_factor = dsm_camera_preview_get_zoom_factor();	
	
		mdi_camera_update_para_zoom(zoom_factor);
		mdi_camera_get_fast_zoom_factor(&new_zoom_factor);
		mr_trace("mr_camera_zoom_out, new_zoom_factor = %d", new_zoom_factor);
	}
	
	return MR_SUCCESS;
}


int32 mr_camera_resume_ex(int32 param)
{
	mr_trace("mr_camera_resume_ex, gDsmCameraReq.state = %d", gDsmCameraReq.state);
	
	if (gDsmCameraReq.state == DSM_CAMERA_PAUSED || gDsmCameraReq.state == DSM_CAMERA_CAPTURED)
	{
		dsm_camera_preview_start();
		if (gDsmCameraReq.last_error == MDI_RES_CAMERA_SUCCEED)
		{
			gDsmCameraReq.state = DSM_CAMERA_PREVIEW;
			/* __MMI_DSM_NEW_JSKY__ begin support camera */
			if (gDsmCameraReq.is_from_jsky)
			{
				dsm_camera_preview_set_blt_layer(TRUE);
			}
			/* __MMI_DSM_NEW_JSKY__ end support camera */
		}
		return gDsmCameraReq.last_error == MDI_RES_CAMERA_SUCCEED ? MR_SUCCESS : MR_FAILED;
	}
	
	return MR_FAILED;
}


int32 mr_camera_pause(int32 param)
{
	if (gDsmCameraReq.state == DSM_CAMERA_PREVIEW || gDsmCameraReq.state == DSM_CAMERA_CAPTURED)
	{
		mdi_camera_preview_stop();
		//mdi_camera_power_off();
		//dsm_camera_preview_set_blt_layer(FALSE);
		//gdi_layer_push_and_set_active(gDsmCameraReq.osd_layer);
		//gdi_layer_clear_background(GDI_COLOR_TRANSPARENT);
		//gdi_layer_set_source_key(FALSE, GDI_COLOR_TRANSPARENT);	
		//gdi_layer_pop_and_restore_active();	
		/* __MMI_DSM_NEW_JSKY__ begin support camera */
		if (gDsmCameraReq.is_from_jsky)
		{
			dsm_camera_preview_set_blt_layer(FALSE);
		}
		/* __MMI_DSM_NEW_JSKY__ end support camera */
		gDsmCameraReq.state = DSM_CAMERA_PAUSED;
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_camera_destroy(int32 param)
{
	if (gDsmCameraReq.state > DSM_CAMERA_UNINIT)
	{
		if (param == gDsmCameraReq.camera_handler)
		{
#if 0		
			if(GetActiveScreenId() == param)
			{
				gDsmCameraReq.state = DSM_CAMERA_UNINIT;
				GoBackHistory();
			}
			else
			{
				DeleteScreenIfPresent(param);
			}
#endif
			/* __MMI_DSM_NEW_JSKY__ begin support camera */
			internalCovered = MR_TRUE;
			gDsmCameraReq.state = DSM_CAMERA_UNINIT;
			/* __MMI_DSM_NEW_JSKY__ end support camera */
			mmi_frm_scrn_close(mr_app_get_group_id(), param);
			
			dsm_camera_uninit();
			return MR_SUCCESS;
		}
	}
	
	return MR_FAILED;
}


int32 mr_camera_get_preview_data(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if (output == NULL || output_len == NULL)
	{
		return MR_FAILED;
	}
	
	if (dsm_camera_get_preview_data(&dsmLayerInfo) == MR_SUCCESS)
	{
		*output = (uint8*)&dsmLayerInfo;
		*output_len = sizeof(dsmLayerInfo);
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_camera_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_camera_launch_info_t *pReq = (mr_camera_launch_info_t*)input;
	
	if (input_len < sizeof(mr_camera_launch_info_t) || input == NULL || output == NULL || output_len == NULL)
	{
		return MR_FAILED;
	}

	if (dsm_camera_create(pReq, &g_mr_common_rsp.p1, &g_mr_common_rsp.p2)== MR_SUCCESS)
	{
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = 8;
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_camera_capture(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(input == NULL||output == NULL||output_len == NULL)
		return MR_FAILED;
	
	if (dsm_camera_capture(*input, &g_mr_common_rsp.p1, &g_mr_common_rsp.p2) == MR_SUCCESS)
	{
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = 8;
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////

static void dsm_voice_record_callback(mdi_result ret)
{
	int32 p1;
	
	switch(ret)
	{
		case MED_RES_DISC_FULL:
			p1 = DSM_ERROR_NO_SPACE;
			break;
		case MED_RES_TERMINATED:
			p1 = DSM_ERROR_TERMINATED;
			if(s_dsm_vr_stream_started){
				char asciiPath[32] ={0};
				U16 fullpathname[32] ={0};
				sprintf(asciiPath,"%c:\\dummy.pcm",MMI_CARD_DRV);
				mmi_asc_to_ucs2((S8 *) fullpathname,( S8 *) asciiPath);
				FS_Delete((const WCHAR *)fullpathname);
				s_dsm_vr_stream_started = FALSE;
			}
		default:
			p1 = DSM_ERROR_UNKNOW;
	}
	
	mr_event(MR_ERROR_EVENT,p1,0);
}


int32 mr_voice_record_start(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};
	int32 ret;
	U8 format;
	mr_voice_record_req_info_t *req = (mr_voice_record_req_info_t *)input;

	if(input_len < sizeof(mr_voice_record_req_info_t))
		return MR_FAILED;
	
	/*if(req== NULL||req->src == NULL)
		return MR_FAILED;
	
	if(req->src_type == SRC_STREAM)
		return MR_IGNORE;
	*/
	if(req == NULL)
		return MR_FAILED;
	
	switch(req->format)
	{
		case MR_SOUND_WAV:
			format = MDI_FORMAT_WAV;
			break;
		case MR_SOUND_PCM:
			format = MDI_FORMAT_PCM_8K;
			break;
		case MR_SOUND_AMR:
			format = MDI_FORMAT_AMR;
			break;
		case MR_SOUND_AMR_WB:
			format = MDI_FORMAT_AMR_WB;
			break;
		default:
			return MR_IGNORE;
			break;
	}

	memset(fullpathname,0,sizeof(fullpathname));
	s_dsm_vr_stream_started = FALSE;
	
	if(req->src_type == SRC_STREAM)
	{
		char asciiPath[32] ={0};
		sprintf(asciiPath,"%c:\\dummy.pcm",MMI_CARD_DRV);
		mmi_asc_to_ucs2((S8 *) fullpathname,( S8 *) asciiPath);
		FS_Delete((const WCHAR *)fullpathname);
		
		ret = mdi_audio_start_record((void *)fullpathname, format,
#if (MTK_VERSION >= 0x0808)
										QUALITY_LOW,
#endif
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
										dsm_voice_record_callback, NULL);
#else
										NULL, dsm_voice_record_callback);
#endif										
		if(ret == MDI_AUDIO_SUCCESS){
			s_dsm_vr_stream_started = TRUE;
		}
	}
	else
	{
	ret = mdi_audio_start_record((void *)mr_fs_get_filename((char *)fullpathname,req->src),format,
#if (MTK_VERSION >= 0x0736)//0x0808
		QUALITY_LOW,
#endif
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
		dsm_voice_record_callback, NULL);
#else
		NULL, dsm_voice_record_callback);
#endif		
	}

	if(ret == MDI_AUDIO_SUCCESS)		
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 mr_voice_record_pause(int32 param)
{
	if( mdi_audio_get_state() !=MDI_AUDIO_RECORD)
		return MR_FAILED;
	if(mdi_audio_pause(NULL,NULL)== MDI_AUDIO_SUCCESS)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 mr_voice_record_resume(int32 param)
{
	if( mdi_audio_get_state() !=MDI_AUDIO_RECORD_PAUSED)
		return MR_FAILED;
	if(mdi_audio_resume(NULL,NULL) == MDI_AUDIO_SUCCESS)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 mr_voice_record_stop(int32 param)
{
	U16 stat = mdi_audio_get_state();
	int32 ret = MR_FAILED;
	
	if(stat == MDI_AUDIO_RECORD_PAUSED||stat == MDI_AUDIO_RECORD)
	{
		if(mdi_audio_stop_record() == MDI_AUDIO_SUCCESS)
			ret = MR_SUCCESS;
		else
			ret = MR_FAILED;
	}
	else
	{
		ret = MR_FAILED;
	}
	
	if(s_dsm_vr_stream_started){
		char asciiPath[32] ={0};
		U16 fullpathname[32] ={0};
		sprintf(asciiPath,"%c:\\dummy.pcm",MMI_CARD_DRV);
		mmi_asc_to_ucs2((S8 *) fullpathname,( S8 *) asciiPath);
		
		FS_Delete((const WCHAR *)fullpathname);
		s_dsm_vr_stream_started = FALSE;
	}
	return ret;
}


static void mr_voice_record_read_done(uint8* output, int32 output_len)
{
	if(dsm_voice_record_stream_isStart() && output_len > 0)
	{
		Media_ReadDataDone((unsigned int)output_len>>1);//byte to word
	}
}


int32 mr_voice_record_get_buffer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	uint32 uBufLen = 0;         
	if(output && output_len && cb && dsm_voice_record_stream_isStart())
	{
		Media_GetReadBuffer((kal_uint16**)output, (kal_uint32*)&uBufLen);
		*output_len = uBufLen<<1;// word to byte
		*cb = mr_voice_record_read_done;
		return MR_SUCCESS;	
	}
	return MR_FAILED;

}


char dsm_voice_record_stream_isStart(void)
{
	return s_dsm_vr_stream_started;
}

#ifdef __MMI_DSM_NEW_JSKY__
S32 dsm_get_audio_time_ms(void)
{
	U32 time = 0;
	
	if(gDsmAudio.state < MR_MEDIA_LOADED)
		return -1;
	
	if(gDsmAudio.load_type == MR_MEDIA_FILE_LOAD)
	{
		if(mdi_audio_get_duration(gDsmAudio.filename,&time) == MDI_AUDIO_SUCCESS)
		{
#if (MTK_VERSION == 0x05c)
			if(gDsmAudio.media_format == MDI_FORMAT_M4A)
				return time/2;
			else
#endif				
				return time;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}
#endif /*__MMI_DSM_NEW_JSKY__*/

#endif/*__MMI_DSM_NEW__*/

