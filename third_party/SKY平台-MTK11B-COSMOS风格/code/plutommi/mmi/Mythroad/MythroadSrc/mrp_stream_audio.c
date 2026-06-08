/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/
// SKY_MULTIMEDIA 

#ifdef  __MMI_DSM_NEW__

/***************************************************************************** 
 * Include
 *****************************************************************************/

#include "kal_release.h"
#include "kal_trace.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"

/* global includes */
#include "l1audio.h"
#include "device.h"
#include "resource_audio.h"
#include "fat_fs.h"
/* DRM_REPLACE */
#include "drm_gprot.h"

#include "nvram_enums.h"
#include "nvram_struct.h"
#include "nvram_user_defs.h"
#include "nvram_data_items.h"
#include "custom_nvram_editor_data_item.h"
#include "custom_equipment.h"

#include "FSAL.h"
#include "MP4_Parser.h"


/* local includes */
#include "med_global.h"
#include "med_status.h"
#include "aud_defs.h"
#include "med_struct.h"
#include "med_v_struct.h"
#include "med_api.h"
#include "med_context.h"
#include "med_main.h"
#include "aud_main.h"
#include "med_utility.h"
#include "mdi_datatype.h"
#include "mdi_audio.h"


#ifdef __AUD_TRACE_ON__
#include "med_trc.h"
#endif 


#include "mrp_stream_audio.h"
#include "mrp_features.h"

typedef MHdl *(*media_open_func_ptr) (void (*handler) (MHdl *hdl, Media_Event event), STFSAL *pstFSAL, void *param);


#define UNKNOWN_MEDIA_FORMAT 255

#define AMR_HEADER   "#!AMR\n"
#define AWB_HEADER   "#!AMR-WB\n"

static void (*splayer_aud_handle)( Media_Event event );
static int32 sSkyMMAudPlaying = 0;

static uint32 gPcmSampleFreq = 8000, gPcmIsStereo = 1, gPcmBitPerSample = 16;
static uint32 gAudFormat;
static MHdl* sSkyMMAudHdl = NULL;
static uint32 audio_sample_rate = 0;

extern vid_context_struct *vid_context_p;

#ifndef MDI_AUD_PTH_EX
#if defined(__BTVCSR_HCI_BCHS__) && defined(__MMI_A2DP_SUPPORT__)
#define MDI_AUD_PTH_EX(x) ((x) | MDI_DEVICE_BT_HEADSET)
#else
#define MDI_AUD_PTH_EX(x)  (x)
#endif
#endif

static int32 SPlayer_get_audiopath(void)
{
	return (MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
}

#ifdef MT6255
typedef struct
{
	LOCAL_PARA_HDR
	Media_Event event;
	Media_Format audio_format;
	int32 handler;
	uint8 *buffer;
	int32 buf_len;
}splayer_med_aud_player_mp4_event;

typedef struct
{
	LOCAL_PARA_HDR
	Media_Event event;
}splayer_med_aud_datareq;

int g_skymm_msgcnt = 0;
int g_skymm_Intmsgcnt = 0;
void splayer_aud_sendMsg(module_type src,int msg,void *pdata,int size);
void skyplayer_mp4_event_callback_hdlr(void* data_p,kal_uint16 data_len)
{
    if(sSkyMMAudHdl&&splayer_aud_is_playing())
    {
        splayer_med_aud_player_mp4_event* mp4_event_p = (splayer_med_aud_player_mp4_event*)data_p;
        Media_Event event = mp4_event_p->event;
		
        //event = sSkyMMAudHdl->Process(sSkyMMAudHdl,event);
     /* ---- SKY_MULTIMEDIA start ---- */
     #if defined(__DSM_SPLAYER__) && defined(__MTK_TARGET__)
     	//if((event == MEDIA_DATA_REQUEST)||((event == MEDIA_BUFFER_UNDERFLOW)))
		if(((event == MEDIA_DATA_REQUEST)||(event == MEDIA_BUFFER_UNDERFLOW))&&SPlayer_isActive())
     		splayer_audioAppend(NULL);
     #endif
         /* ---- SKY_MULTIMEDIA end ---- */
    }

	if(g_skymm_msgcnt >0)
		g_skymm_msgcnt --;
	
	if(g_skymm_msgcnt < 0)
		g_skymm_msgcnt = 0;
}
void skyplayer_mp4_event_process(void* data_p,kal_uint16 data_len)
{

    if(sSkyMMAudHdl)
    {
        splayer_med_aud_datareq* mp4_event_p = (splayer_med_aud_datareq*)data_p;
        Media_Event event = mp4_event_p->event;
		//sk_dbg_print("skyplayer_mp4_event_process =%d",g_skymm_msgcnt);
        event = sSkyMMAudHdl->Process(sSkyMMAudHdl,event);
      /* ---- SKY_MULTIMEDIA start ---- */
      #if defined(__DSM_SPLAYER__) && defined(__MTK_TARGET__)
		//if((event == MEDIA_DATA_REQUEST)||((event == MEDIA_BUFFER_UNDERFLOW)))
		if(((event == MEDIA_DATA_REQUEST)||(event == MEDIA_BUFFER_UNDERFLOW))&&SPlayer_isActive())
			splayer_audioAppend(NULL);
      #endif
      /* ---- SKY_MULTIMEDIA end ---- */
    }

	if(g_skymm_Intmsgcnt >0)
		g_skymm_Intmsgcnt --;
	
	if(g_skymm_Intmsgcnt < 0)
		g_skymm_Intmsgcnt = 0;
}
#endif
static void splayer_aud_stream_event_callback (MHdl *hdl, Media_Event event)
{
#ifdef MT6255

	kal_prompt_trace(MOD_MED, "[Allan] splayer_aud_stream_event_callback++ g_skymm_Intmsgcnt %d,event=%d", g_skymm_Intmsgcnt,event);
	if(sSkyMMAudHdl)
	{
		splayer_med_aud_datareq mp4_event;
		mp4_event.event = event;
		if((g_skymm_Intmsgcnt < 1)&&
		((event == MEDIA_DATA_REQUEST)||
		 ((event == MEDIA_BUFFER_UNDERFLOW))))
		{
			g_skymm_Intmsgcnt ++;
			aud_util_proc_in_med_ext(MOD_L1SP,
		                         skyplayer_mp4_event_process,
		                         (void*)&mp4_event,
		                         sizeof(splayer_med_aud_datareq));
		}
	}
    
	#if 0
	if(SPlayer_isActive()&&
		(g_skymm_msgcnt <1)&&
		((event == MEDIA_DATA_REQUEST)||
		 ((event == MEDIA_BUFFER_UNDERFLOW))))
	{
		splayer_med_aud_player_mp4_event msg_p;
		int ret = 0;
		
		msg_p.audio_format = NULL;
		msg_p.buffer = NULL;
		msg_p.buf_len = NULL;
		msg_p.event = event;
		msg_p.handler = NULL;
		g_skymm_msgcnt ++;
		splayer_aud_sendMsg(MOD_L1SP,MSG_ID_SPLAYER_AUDIO_CALLBACK,&msg_p,sizeof(splayer_med_aud_player_mp4_event));
	}
	

	#endif
#else
	if(splayer_aud_handle)
	{
		splayer_aud_handle(event);
	}
#endif
}
#ifdef MT6255
kal_eventgrpid g_skymmEvent = 0;
int g_skyMsgRet = 0;
typedef enum
{
    SKYAUD_EVT_NOWAIT		= 0x00000000,
    SKYAUD_EVT_OPEN		= 0x00000001,
    SKYAUD_EVT_PLAY			= 0x00000002,
    SKYAUD_EVT_STOP		= 0x00000004,
    SKYAUD_EVT_PAUSE		= 0x00000008,
    SKYAUD_EVT_RESUME		= 0x00000010,
    SKYAUD_EVT_CALLBACK	= 0x00000020,
}SKYAUD_EVT_MSG_TYPE;
kal_int32 skymm_wait_event(kal_uint32 event)
{
	kal_uint32 retrieved_events;
	if(g_skymmEvent)
	{
		kal_retrieve_eg_events(
						        g_skymmEvent,
						        event,
						        KAL_OR_CONSUME,
						        &retrieved_events,
						        KAL_SUSPEND);
	}
}
kal_int32 skymm_set_event(kal_uint32 event)
{
	if(g_skymmEvent)
	    kal_set_eg_events(g_skymmEvent, event, KAL_OR);

}
splayer_med_aud_player_mp4_event g_msg_p;
void splayer_aud_sendMsg(module_type src,int msg,void *pdata,int size)
{
	ilm_struct *ilm_ptr = NULL;
	char *ptemp = 0;
	module_type src_id = kal_get_active_module_id();
	
	//sk_dbg_print("[AUDIO] splayer_aud_sendMsg -size=%d \n",size);
	//kal_sleep_task(1);
	if(!g_skymmEvent)
		g_skymmEvent = kal_create_event_group("skyaud_events");
	
	if(size)
		ptemp = (char*)construct_local_para(size, TD_CTRL);
	
	memcpy(ptemp +sizeof(local_para_struct),(char *)pdata + sizeof(local_para_struct),size- sizeof(local_para_struct));
	
	ilm_ptr = allocate_ilm(src_id);
	ilm_ptr->src_mod_id = src_id;

	ilm_ptr->dest_mod_id    =  MOD_MED;
	ilm_ptr->msg_id 	      =       msg;
	
	ilm_ptr->local_para_ptr  = (local_para_struct*)ptemp;
	ilm_ptr->peer_buff_ptr = NULL;
	
	msg_send_ext_queue(ilm_ptr);
}

void splayer_aud_msg_req_hdlr(ilm_struct *ilm_ptr)
{
	splayer_med_aud_player_mp4_event *msg_p = (splayer_med_aud_player_mp4_event*)ilm_ptr->local_para_ptr;
	SKYAUD_EVT_MSG_TYPE evt_msgtype = SKYAUD_EVT_NOWAIT;
	int ret = 0;
	//sk_dbg_print("[AUDIO] splayer_aud_msg_req_hdlr - \n");
	//kal_sleep_task(1);
	
	switch(ilm_ptr->msg_id)
	{
		case MSG_ID_SPLAYER_AUDIO_OPEN:
			evt_msgtype = SKYAUD_EVT_OPEN;
			ret = splayer_aud_stream_openEx(msg_p->audio_format,
										msg_p->handler,
										msg_p->buffer,
										msg_p->buf_len);
			break;
		case MSG_ID_SPLAYER_AUDIO_PLAY:
			evt_msgtype = SKYAUD_EVT_PLAY;
			ret = splayer_aud_stream_playEx();
			break;
		case MSG_ID_SPLAYER_AUDIO_STOP:
			evt_msgtype = SKYAUD_EVT_STOP;
			ret = splayer_aud_stream_stopEx();
			break;
		case MSG_ID_SPLAYER_AUDIO_PAUSE:
			evt_msgtype = SKYAUD_EVT_PAUSE;
			ret = splayer_aud_stream_pauseEx();
			break;
		case MSG_ID_SPLAYER_AUDIO_RESUME:
			evt_msgtype = SKYAUD_EVT_RESUME;
			ret = splayer_aud_stream_resumeEx();
			break;
		case MSG_ID_SPLAYER_AUDIO_CALLBACK:
			evt_msgtype = SKYAUD_EVT_CALLBACK;
			//msg_p = (splayer_med_aud_player_mp4_event  *)ilm_ptr->local_para_ptr;
			skyplayer_mp4_event_callback_hdlr(msg_p,sizeof(splayer_med_aud_player_mp4_event));
			break;
				
	}
	g_skyMsgRet = ret ;
	//sk_dbg_print("[AUDIO] splayer_aud_msg_req_hdlr - ret=%d\n",ret);
	//kal_sleep_task(1);
	skymm_set_event(evt_msgtype);
}
#endif
void splayer_aud_set_pcm_param(uint32 sampleFreq, uint32 bitPerSample, uint32 isStereo)
{
	gPcmSampleFreq = sampleFreq;
	gPcmBitPerSample = bitPerSample;
	gPcmIsStereo = isStereo;
}

#ifdef MT6255
int32 splayer_aud_stream_open(Media_Format audio_format, int32 handler, uint8 *buffer, int32 buf_len)
{
	splayer_med_aud_player_mp4_event msg_p;
	int ret = 0;

	//splayer_aud_stream_stopEx();//Allan add
	
	msg_p.audio_format = audio_format;
	msg_p.buffer = buffer;
	msg_p.buf_len = buf_len;
	msg_p.event = NULL;
	msg_p.handler = handler;
	splayer_aud_sendMsg(NULL/*MOD_MULTIMEDIA*/,MSG_ID_SPLAYER_AUDIO_OPEN,&msg_p,sizeof(splayer_med_aud_player_mp4_event));
	skymm_wait_event(SKYAUD_EVT_OPEN);
	ret = g_skyMsgRet;
	return ret;
}
int32 splayer_aud_stream_openEx(Media_Format audio_format, int32 handler, uint8 *buffer, int32 buf_len)
{
#else
int32 splayer_aud_stream_open(Media_Format audio_format, int32 handler, uint8 *buffer, int32 buf_len)
{
#endif
	media_open_func_ptr openFunc = NULL;
	Media_VM_PCM_Param vpFormat;
	Media_PCM_Stream_Param vpFormat_pcm;
	void *param = NULL;
	int32 pcm = 0;
	
#ifdef MT6255
	g_skymm_msgcnt = 0;
#endif
	
    kal_prompt_trace(MOD_MMI,"[Allan] splayer_aud_stream_open - %d, %d\n", audio_format, buf_len);

#ifndef MT6255
	splayer_aud_stream_stop();//Allan add
#endif

	gAudFormat = audio_format;
    /* The audio format in 3GP/MP4 file */
	switch (audio_format)
	{
	case MED_TYPE_PCM_8K:
	case MED_TYPE_PCM_16K:
	case MED_TYPE_WAV:
        memset(&vpFormat_pcm,0x00,sizeof(Media_PCM_Stream_Param));
		vpFormat_pcm.sampleFreq = gPcmSampleFreq;
		vpFormat_pcm.isStereo = gPcmIsStereo; 
		vpFormat_pcm.bitPerSample = gPcmBitPerSample; 
		vpFormat_pcm.forceVoice = 0;
		param = &vpFormat_pcm;
		pcm = 1;
		break;

#ifdef AMR_DECODE
	case MED_TYPE_AMR: 
#ifdef AMRWB_DECODE
	case MED_TYPE_AMR_WB:
#endif 
		vpFormat.mediaType = audio_format;
		vpFormat.vmParam = (kal_uint32) MEDIA_VMP_AS_RINGTONE;
		param = &vpFormat;
		openFunc = AMR_Open;
		break;
		
#endif /* AMR_DECODE */ 
#ifdef DAF_DECODE
	case MED_TYPE_DAF:
#ifdef MUSICAM_DECODE
	case MED_TYPE_MUSICAM:
#endif
		openFunc = DAF_Open;
		break;
		
#endif /* DAF_DECODE */ 			
#ifdef AAC_DECODE
	case MED_TYPE_AAC:
		openFunc = AAC_Open;
		break;
		
#endif /* AAC_DECODE */ 		
	default:
		kal_prompt_trace(MOD_MMI,"[Allan] splayer_aud_stream_open UNKNOW FORMAT %d \r\n", audio_format);
		return -1;
		break;
       }	

	if(!mdi_audio_is_idle())
	{
		//sk_dbg_print("Audio is Busy!!!");
		return -1;
	}
	
       splayer_aud_handle=((void (*)( Media_Event ))handler);  
	if(pcm == 0)
	{
		sSkyMMAudHdl = openFunc(splayer_aud_stream_event_callback, NULL, param);
	}
	else
	{//pcm
		sSkyMMAudHdl = PCM_Strm_Open(splayer_aud_stream_event_callback, param);
		sSkyMMAudHdl->mediaType = audio_format;//ALlan add 20111210
	}

	if (sSkyMMAudHdl == NULL)
	{
		kal_prompt_trace(MOD_MMI,"[Allan] splayer_aud_stream_open openFunc error \r\n");
		return -1;
	}	
	/*
	if(buffer==NULL&&buf_len==0)
	{
        buffer = (uint8*)aud_context_p->ring_buf;
        buf_len = AUD_RING_BUFFER_LEN*2;
        }
    */
	sSkyMMAudHdl->SetBuffer(sSkyMMAudHdl, buffer, buf_len);
	sSkyMMAudHdl->ResetMediaBuf(sSkyMMAudHdl);
	
	return 0;
}
#ifdef MT6255
int32 splayer_aud_stream_play(void)
{
	splayer_med_aud_player_mp4_event msg_p;
	int ret = 0;
	msg_p.audio_format = NULL;
	msg_p.buffer = NULL;
	msg_p.buf_len = NULL;
	msg_p.event = NULL;
	msg_p.handler = NULL;
	splayer_aud_sendMsg(NULL/*MOD_MULTIMEDIA*/,MSG_ID_SPLAYER_AUDIO_PLAY,&msg_p,sizeof(splayer_med_aud_player_mp4_event));
	skymm_wait_event(SKYAUD_EVT_PLAY);
	ret = g_skyMsgRet;
	return ret;
}
int32 splayer_aud_stream_playEx(void)
{
#else
int32 splayer_aud_stream_play(void)
{
#endif
	int32 result = MEDIA_FAIL;
		
	aud_get_active_device_path_by_mode(SPlayer_get_audiopath(), aud_melody_set_output_device);
		
	if (sSkyMMAudHdl != NULL)
	{	
                #if defined(__BTMTK__) && defined(__MED_BT_A2DP_MOD__)
                    kal_trace(TRACE_GROUP_4, VID_TRC_CALLING_AUD_BT_A2DP_OPEN_CODEC, __LINE__);                          
                    aud_bt_a2dp_open_codec(gAudFormat);                        
                #endif
		result = sSkyMMAudHdl->Play(sSkyMMAudHdl);
		sSkyMMAudPlaying = 1;
	}
	if(result != MEDIA_SUCCESS)
		result = 1;
	else
		result = 0;

	return result;
}
#ifdef MT6255
int32 splayer_aud_stream_stop(void)
{
	splayer_med_aud_player_mp4_event msg_p;
	int ret = 0;
	msg_p.audio_format = NULL;
	msg_p.buffer = NULL;
	msg_p.buf_len = NULL;
	msg_p.event = NULL;
	msg_p.handler = NULL;
	splayer_aud_sendMsg(NULL/*MOD_MULTIMEDIA*/,MSG_ID_SPLAYER_AUDIO_STOP,&msg_p,sizeof(splayer_med_aud_player_mp4_event));
	skymm_wait_event(SKYAUD_EVT_STOP);
	ret = g_skyMsgRet;
	return ret;
}
int32 splayer_aud_stream_stopEx(void)
{
#else
int32 splayer_aud_stream_stop(void)
{
#endif
	int32 result = MEDIA_FAIL;
    	if (sSkyMMAudHdl != NULL)
    	{
        	sSkyMMAudHdl->Stop(sSkyMMAudHdl); 
        	sSkyMMAudHdl->Close(sSkyMMAudHdl);
        	sSkyMMAudHdl = NULL;
		result = MEDIA_SUCCESS;	
		sSkyMMAudPlaying = 0;
    	}
    if(result != MEDIA_SUCCESS)
		result = 1;
	else
		result = 0;
	return result;
}

int32 splayer_aud_is_playing(void)
{
	return sSkyMMAudPlaying;
}

//#include "wav.h"
//#include "daf_drv.h"
//#include "aac_drv.h"
#ifdef MT6255
int32 splayer_aud_stream_pause(void)
{
	splayer_med_aud_player_mp4_event msg_p;
	int ret = 0;
	msg_p.audio_format = NULL;
	msg_p.buffer = NULL;
	msg_p.buf_len = NULL;
	msg_p.event = NULL;
	msg_p.handler = NULL;
	splayer_aud_sendMsg(NULL/*MOD_MULTIMEDIA*/,MSG_ID_SPLAYER_AUDIO_PAUSE,&msg_p,sizeof(splayer_med_aud_player_mp4_event));
	skymm_wait_event(SKYAUD_EVT_PAUSE);
	ret = g_skyMsgRet;
	return ret;
}
int32 splayer_aud_stream_pauseEx(void)
{
#else
int32 splayer_aud_stream_pause(void)
{
#endif
	int32 result = MEDIA_FAIL;
	kal_uint8 format_state = 0;
	switch(sSkyMMAudHdl->mediaType)
	{
		case MED_TYPE_PCM_8K:
		case MED_TYPE_PCM_16K:
		case MED_TYPE_WAV:
			format_state = 4;//WAV_STATE_PLAYING;
			break;//Allan add 20111210
	#ifdef AMR_DECODE
		case MED_TYPE_AMR: 
	#ifdef AMRWB_DECODE
		case MED_TYPE_AMR_WB:
	#endif
		format_state = 1;//AMR_STATE_PLAY;
		break;			
	#endif
	#ifdef DAF_DECODE
		case MED_TYPE_DAF:
	#ifdef MUSICAM_DECODE
		case MED_TYPE_MUSICAM:
	#endif
			format_state = 1;//DAF_STATE_PLAY;
			break;	
	#endif
    #ifdef AAC_DECODE
    	case MED_TYPE_AAC:
			format_state = 1;//AAC_STATE_PLAY;
			break;
    #endif /* AAC_DECODE */ 		
	default:
		return 1;
		break;
	}
	
	if ((sSkyMMAudHdl != NULL)&&(format_state == sSkyMMAudHdl->state))
	{
		result = sSkyMMAudHdl->Pause(sSkyMMAudHdl);
	}
	if(result != MEDIA_SUCCESS)
		result = 1;
	else
		result = 0;
	return result;
}
#ifdef MT6255
int32 splayer_aud_stream_resume(void)
{
	splayer_med_aud_player_mp4_event msg_p;
	int ret = 0;
	msg_p.audio_format = NULL;
	msg_p.buffer = NULL;
	msg_p.buf_len = NULL;
	msg_p.event = NULL;
	msg_p.handler = NULL;
	splayer_aud_sendMsg(NULL/*MOD_MULTIMEDIA*/,MSG_ID_SPLAYER_AUDIO_RESUME,&msg_p,sizeof(splayer_med_aud_player_mp4_event));
	skymm_wait_event(SKYAUD_EVT_RESUME);
	ret = g_skyMsgRet;
	return ret;
}
int32 splayer_aud_stream_resumeEx(void)
{
#else
int32 splayer_aud_stream_resume(void)
{
#endif
	int32 result = MEDIA_FAIL;
	
	aud_get_active_device_path_by_mode(SPlayer_get_audiopath(), aud_melody_set_output_device);
	if (sSkyMMAudHdl != NULL)
	{
                #if defined(__BTMTK__) && defined(__MED_BT_A2DP_MOD__)
                    kal_trace(TRACE_GROUP_4, VID_TRC_CALLING_AUD_BT_A2DP_OPEN_CODEC, __LINE__);                          
                    aud_bt_a2dp_open_codec(gAudFormat);                        
                #endif
		result = sSkyMMAudHdl->Resume(sSkyMMAudHdl);
	}
	if(result != MEDIA_SUCCESS)
		result = 1;
	else
		result = 0;

	return result;
}
int32 splayer_aud_stream_GetCurrentTime(void)
{
	int32 result = 0;
	
	if (sSkyMMAudHdl != NULL)
	{
		result = sSkyMMAudHdl->GetCurrentTime(sSkyMMAudHdl);
	}
	return result;
}//Allan 20120106
int32 SPlayer_aud_data_append(uint8* data, uint32 size)
{
	int data_write,result;	
	uint8 *buf_p;
	uint32 uBufLen;					
	
	result = 0;
	if(NULL == sSkyMMAudHdl)
		return result;
	
	sSkyMMAudHdl->GetWriteBuffer(sSkyMMAudHdl,&buf_p, &uBufLen);	

	data_write = uBufLen;
	if(data_write>size)
		data_write=size;
	data_write = (data_write>>1)<<1;
	
	memcpy(buf_p, data, data_write);
	result += data_write;
	
	sSkyMMAudHdl->WriteDataDone(sSkyMMAudHdl,data_write);
	sSkyMMAudHdl->FinishWriteData(sSkyMMAudHdl); 
	
	return result;	

}

void SPlayer_aud_Media_SetBuffer(uint16 *buffer, uint32 buf_len)
{

	if(sSkyMMAudHdl)
	{
		sSkyMMAudHdl->SetBuffer(sSkyMMAudHdl, (uint8*)buffer, buf_len);
	}
}

void SPlayer_aud_Media_GetWriteBuffer(uint16 **buffer, uint32 *buf_len)
{
	if(sSkyMMAudHdl)
	{
		sSkyMMAudHdl->GetWriteBuffer(sSkyMMAudHdl, (uint8 **)buffer, buf_len);
	}
}

void SPlayer_aud_Media_WriteDataDone(uint32 len)
{

	if(sSkyMMAudHdl)
	{	
		sSkyMMAudHdl->WriteDataDone(sSkyMMAudHdl,len);	
		sSkyMMAudHdl->FinishWriteData(sSkyMMAudHdl); 
	}
}

int32 SPlayer_aud_Media_GetFreeSpace( void )
{
	int32 rv = 0;

	if(sSkyMMAudHdl)
	{
		rv = sSkyMMAudHdl->GetFreeSpace(sSkyMMAudHdl);
	}
	return rv;
}

void SPlayer_aud_Media_ResetMediaBuff( void )
{
	if(sSkyMMAudHdl)
	{
		sSkyMMAudHdl->ResetMediaBuf(sSkyMMAudHdl);
	}
}


void SPlayer_aud_print_state(void)
{
	if(sSkyMMAudHdl)
	{
		kal_prompt_trace(MOD_MMI, "[Allan] audio state: %d", sSkyMMAudHdl->state);
	}
}

int32 SPlayer_aud_get_CurrTime(void)
{
	if(sSkyMMAudHdl)
	{
		return sSkyMMAudHdl->GetCurrentTime(sSkyMMAudHdl);
	}
	return -1;
}
unsigned int skymm_getaudio_offset(void)
{
	unsigned int pauseOffset = 0;
	switch(sSkyMMAudHdl->mediaType)
	{
	case MEDIA_FORMAT_PCM_8K:
	case MEDIA_FORMAT_PCM_16K:
		pauseOffset = 20;
		break;
		
	case MEDIA_FORMAT_AMR: 
	case MEDIA_FORMAT_AMR_WB:
		pauseOffset = 20;
		break;
		
	case MEDIA_FORMAT_DAF:
	case MED_TYPE_MUSICAM:
		pauseOffset = (1152*1000/vid_context_p->audio_sample_rate)<<3;//;
		break;
		
	case MEDIA_FORMAT_AAC:
		pauseOffset = (1024*1000/vid_context_p->audio_sample_rate)<<3;
		break;
	default:
		pauseOffset = 1;//;
		break;
       }
	return pauseOffset;
}//Allan 20120106
void Sky_MM_Audio_SetLevel(int32 nLevel)	
{
	int32 temp = nLevel;
	 
	kal_prompt_trace(MOD_MED, "[Allan] Sky_MM_Audio_SetLevel  level: %d", nLevel);
	if (nLevel > MPC_AUD_VOL_MAX)
	{
		temp = MPC_AUD_VOL_MAX;
	}
	else if (nLevel < MPC_AUD_VOL_MIN)
	{
		temp = MPC_AUD_VOL_MIN;
	}
	if(nLevel == MPC_AUD_VOL_MIN)
	{
		mdi_audio_set_mute(MDI_VOLUME_MEDIA, TRUE);
		return;
	}

	mdi_audio_set_volume(MDI_VOLUME_MEDIA, temp*LEVEL7/MPC_AUD_VOL_MAX);
}

#if defined(__DSM_MEDIA_EXT_AUD_REC__)
int32 Sky_MM_Audio_GetLevelCount()
{
	return MPC_AUD_VOL_MAX;
}


int32 (*g_mr_media_aud_rec_cb)(int32 event, void *pdata, uint32 datasize, int32 pridata) = NULL;
int32 g_mr_media_aud_rec_pridata = 0;
extern uint8 isEarphonePlugged;

static void mr_media_aud_record_callback (mdi_result result)//event callback
{
	if(g_mr_media_aud_rec_cb)
		g_mr_media_aud_rec_cb(result, 0, 0, g_mr_media_aud_rec_pridata);
}

int32 mr_media_aud_record_start(void *file_name, int32 format,  void *callback, void *pData)
{
	uint8 aud_format;
	switch(format)
	{
		case 0:
			aud_format = MED_TYPE_AMR;
			break;
		case 1:
			aud_format = MED_TYPE_AMR_WB;
			break;
		case 2:
			aud_format = MED_TYPE_PCM_8K;
			break;
		case 3:
			aud_format = MED_TYPE_PCM_16K;
		default:
			return -1;
	}
	
	g_mr_media_aud_rec_cb = (int32 (*)(int32 , void *, uint32, int32 ))callback;
	g_mr_media_aud_rec_pridata = (int32)pData;
	#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	if(mdi_audio_start_record(file_name, aud_format, 0, (mdi_ext_callback )mr_media_aud_record_callback, NULL) == MDI_AUDIO_SUCCESS)
	#else
	if(mdi_audio_start_record(file_name, aud_format, 0, NULL, mr_media_aud_record_callback) == MDI_AUDIO_SUCCESS)
	#endif
	{
		return 0;
	}
	return -1;
}

int32 mr_media_aud_record_stop(void)
{
	mdi_result result;

	g_mr_media_aud_rec_cb = NULL;
	g_mr_media_aud_rec_pridata = 0;
	result = mdi_audio_stop_record();

	return result;
}

void mr_media_record_data_hdlr(void* msg)
{
	mmi_media_aud_mr_data_ind_struct *local_data = (mmi_media_aud_mr_data_ind_struct*) msg;

	mr_media_send_aud_data(local_data->pData, local_data->nDataSize);
}


void mr_media_record_data_ind(uint16*buf, uint32 buf_size)
{
	mmi_media_aud_mr_data_ind_struct *msg_p = NULL;
	ilm_struct *ilm_ptr = NULL;

	msg_p = (mmi_media_aud_mr_data_ind_struct*)construct_local_para(sizeof(mmi_media_aud_mr_data_ind_struct), TD_CTRL);
	msg_p->pData        = buf;
	msg_p->nDataSize  = buf_size;

	ilm_ptr = allocate_ilm(MOD_MED);
	ilm_ptr->src_mod_id = MOD_MED;
	ilm_ptr->dest_mod_id    =  MOD_MMI;
	ilm_ptr->msg_id 	      =       MSG_ID_MPCHAT_AUD_DATA_IND;
	ilm_ptr->local_para_ptr  = (local_para_struct*)msg_p;
	ilm_ptr->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_ptr);
}
#endif

#endif


