/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/

#if 0//(defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__)) && defined(__MTK_TARGET__)

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "SKY_MM_def.h"

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

#if defined(__MED_VID_MOD__) || defined(__MED_MJPG_MOD__)
#include "vid_main.h"
//#include "vid_avi.h"
#endif /* defined(__MED_VID_MOD__) || defined(__MED_MJPG_MOD__) */ 

#ifdef __AUD_TRACE_ON__
#include "med_trc.h"
#endif 


#include "SKY_MM_BaseTypes.h"
#include "SKY_MM_audio.h"

typedef MHdl *(*media_open_func_ptr) (void (*handler) (MHdl *hdl, Media_Event event), STFSAL *pstFSAL, void *param);
typedef Media_Status(*media_ctrl_func_ptr) (MHdl *hdl);
typedef void (*media_set_buf_func_ptr) (MHdl *hdl, kal_uint8 *buffer, kal_uint32 buf_len);
typedef Media_Status(*media_set_time_func_ptr) (MHdl *hdl, kal_uint32 msStartTime);
typedef Media_Event(*media_proc_func_ptr) (MHdl *hdl, Media_Event event);
typedef Media_Status(*media_get_info_func_ptr) (STFSAL *pstFSAL, audInfoStruct *contentInfo, void *param);

#define UNKNOWN_MEDIA_FORMAT 255

#define AMR_HEADER   "#!AMR\n"
#define AWB_HEADER   "#!AMR-WB\n"

static void (*splayer_aud_handle)( Media_Event event );
static VCI32 sSkyMMAudPlaying = 0;

static VCU32 gPcmSampleFreq = 8000, gPcmIsStereo = 1, gPcmBitPerSample = 16;
static VCU32 gAudFormat;
static MHdl* sSkyMMAudHdl;

extern vid_context_struct *vid_context_p;

#ifndef MDI_AUD_PTH_EX
#if defined(__BTVCSR_HCI_BCHS__) && defined(__MMI_A2DP_SUPPORT__)
#define MDI_AUD_PTH_EX(x) ((x) | MDI_DEVICE_BT_HEADSET)
#else
#define MDI_AUD_PTH_EX(x)  (x)
#endif
#endif

static VCI32 SPlayer_get_audiopath(VCVoid)
{
	return (MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
}

MHdl *RA_Open(void(*handler)( MHdl *handle, Media_Event event ), STFSAL *pstFSAL, void *param);
static VCVoid splayer_aud_stream_event_callback (MHdl *hdl, Media_Event event)
{
	if(splayer_aud_handle)
	{
		splayer_aud_handle(event);
	}
}

VCVoid splayer_aud_set_pcm_param(VCU32 sampleFreq, VCU32 bitPerSample, VCU32 isStereo)
{
	gPcmSampleFreq = sampleFreq;
	gPcmBitPerSample = bitPerSample;
	gPcmIsStereo = isStereo;
}

VCI32 splayer_aud_stream_open(Media_Format audio_format, VCI32 handler, VCU8 *buffer, VCI32 buf_len)
{
	media_open_func_ptr openFunc = NULL;
	Media_VM_PCM_Param vpFormat;
	Media_PCM_Stream_Param vpFormat_pcm;
	VCVoid *param = NULL;
	VCI32 pcm = 0;
	splayer_aud_stream_stop();//Allan add

	gAudFormat = audio_format;
    /* The audio format in 3GP/MP4 file */
	switch (audio_format)
	{
	case MED_TYPE_PCM_8K:
	case MED_TYPE_PCM_16K:
	case MED_TYPE_WAV:
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
#if 0 //def __DSM_SPLAYER_RMVB_SUPPORT__
	case MED_TYPE_WAV:
		openFunc =  RA_Open; 
		break;
#endif
	default:
		sk_dbg_print("splayer_aud_stream_open UNKNOW FORMAT %d \r\n", audio_format);
		return -1;
		break;
       }	

	if(!mdi_audio_is_idle())
	{
		sk_dbg_print("Audio is Busy!!!");
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
		sk_dbg_print("splayer_aud_stream_open openFunc error \r\n");
		return -1;
	}	
	
	/*
	if(buffer==NULL&&buf_len==0)
	{
		buffer = (VCU8*)aud_context_p->ring_buf;
            	buf_len = AUD_RING_BUFFER_LEN*2;
    	}
    	*/
	sSkyMMAudHdl->SetBuffer(sSkyMMAudHdl, buffer, buf_len);
	sSkyMMAudHdl->ResetMediaBuf(sSkyMMAudHdl);
	
	return 0;
}

VCI32 splayer_aud_stream_play(VCVoid)
{
	VCI32 result = MEDIA_FAIL;
		
	aud_get_active_device_path_by_mode(SPlayer_get_audiopath(), aud_melody_set_output_device);
		
	if (sSkyMMAudHdl != NULL)
	{	
                #if defined(__BTMTK__) && defined(__MED_BT_A2DP_MOD__)
                    kal_trace(TRACE_GROUP_4, VID_TRC_CALLING_AUD_BT_A2DP_OPEN_CODEC, __LINE__);                          
                    aud_bt_a2dp_open_codec(gAudFormat);                        
                #endif
		result = sSkyMMAudHdl->Play(sSkyMMAudHdl);
		if(result == MEDIA_SUCCESS)
		{
			sSkyMMAudPlaying = 1;
		}

	}

	sk_dbg_print("splayer_aud_stream_play result %d", result);
	if(result == MEDIA_SUCCESS)
		return 0;
	else
		return -1;
}

VCI32 splayer_aud_stream_stop(VCVoid)
{
	VCI32 result = MEDIA_FAIL;

	sk_dbg_print("splayer_aud_stream_stop");
    	if (sSkyMMAudHdl != NULL)
    	{
        	sSkyMMAudHdl->Stop(sSkyMMAudHdl); 
        	sSkyMMAudHdl->Close(sSkyMMAudHdl);
        	sSkyMMAudHdl = NULL;
		result = MEDIA_SUCCESS;	
		sSkyMMAudPlaying = 0;
    	}
	if(result == MEDIA_SUCCESS)
		return 0;
	else
		return -1;
}

VCI32 splayer_aud_is_playing(VCVoid)
{
	return sSkyMMAudPlaying;
}

#define AMR_STATE_PLAY 1
#define WAV_STATE_PLAYING 4

VCI32 splayer_aud_stream_pause(VCVoid)
{
	VCI32 result = MEDIA_FAIL;
	int aud_state = 0;
	
	switch(sSkyMMAudHdl->mediaType)
	{
	case MEDIA_FORMAT_PCM_8K:
	case MEDIA_FORMAT_PCM_16K:
	case MEDIA_FORMAT_WAV:
		aud_state = WAV_STATE_PLAYING;
		break;
		
#ifdef AMR_DECODE
	case MEDIA_FORMAT_AMR: 
#ifdef AMRWB_DECODE
	case MEDIA_FORMAT_AMR_WB:
#endif 
		aud_state = AMR_STATE_PLAY;
		break;
		
#endif /* AMR_DECODE */ 
#ifdef DAF_DECODE
	case MEDIA_FORMAT_DAF:
#ifdef MUSICAM_DECODE
	case MED_TYPE_MUSICAM:
#endif
		aud_state = 1;//DAF_STATE_PLAY;
		break;
		
#endif /* DAF_DECODE */ 			
#ifdef AAC_DECODE
	case MEDIA_FORMAT_AAC:
		aud_state = 1;//AAC_STATE_PLAY;
		break;
#endif /* AAC_DECODE */ 		
	default:
		aud_state = 1;//AAC_STATE_PLAY;
		break;
       }
	
	if ((sSkyMMAudHdl != NULL)&&(aud_state == sSkyMMAudHdl->state))
	{
		result = sSkyMMAudHdl->Pause(sSkyMMAudHdl);
	}
	if(result == MEDIA_SUCCESS)
		return 0;
	else
		return -1;
}

VCI32 splayer_aud_stream_resume(VCVoid)
{
	VCI32 result = MEDIA_FAIL;
		aud_get_active_device_path_by_mode(SPlayer_get_audiopath(), aud_melody_set_output_device);

	if (sSkyMMAudHdl != NULL)
	{
	                #if defined(__BTMTK__) && defined(__MED_BT_A2DP_MOD__)
                    kal_trace(TRACE_GROUP_4, VID_TRC_CALLING_AUD_BT_A2DP_OPEN_CODEC, __LINE__);                          
                    aud_bt_a2dp_open_codec(gAudFormat);                        
                #endif
		result = sSkyMMAudHdl->Resume(sSkyMMAudHdl);
	}
	if(result == MEDIA_SUCCESS)
		return 0;
	else
		return -1;
}

VCI32 SPlayer_aud_data_append(VCU8* data, VCU32 size)
{
	int data_write,result;	
	VCU8 *buf_p;
	VCU32 uBufLen;					
	
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

VCVoid SPlayer_aud_Media_SetBuffer(VCU16 *buffer, VCU32 buf_len)
{

	if(sSkyMMAudHdl)
	{
		sSkyMMAudHdl->SetBuffer(sSkyMMAudHdl, (VCU8*)buffer, buf_len);
	}
}


VCVoid SPlayer_aud_Media_GetWriteBuffer(VCU16 **buffer, VCU32 *buf_len)
{
	if(sSkyMMAudHdl)
	{
		sSkyMMAudHdl->GetWriteBuffer(sSkyMMAudHdl, (VCU8 **)buffer, buf_len);
	}
}

VCVoid SPlayer_aud_Media_WriteDataDone(VCU32 len)
{

	if(sSkyMMAudHdl)
	{	
		sSkyMMAudHdl->WriteDataDone(sSkyMMAudHdl,len);	
		sSkyMMAudHdl->FinishWriteData(sSkyMMAudHdl); 
	}
}

VCI32 SPlayer_aud_Media_GetFreeSpace( VCVoid )
{
	VCI32 rv = 0;

	if(sSkyMMAudHdl)
	{
		rv = sSkyMMAudHdl->GetFreeSpace(sSkyMMAudHdl);
	}
	return rv;
}

VCVoid SPlayer_aud_Media_ResetMediaBuff( VCVoid )
{
	if(sSkyMMAudHdl)
	{
		sSkyMMAudHdl->ResetMediaBuf(sSkyMMAudHdl);
	}
}


VCVoid SPlayer_aud_print_state(VCVoid)
{
	if(sSkyMMAudHdl)
	{
		kal_prompt_trace(MOD_MMI, "++++ audio state: %d ++++", sSkyMMAudHdl->state);
	}
}

VCI32 SPlayer_aud_get_CurrTime(VCVoid)
{
	if(sSkyMMAudHdl)
	{
		return sSkyMMAudHdl->GetCurrentTime(sSkyMMAudHdl);
	}
	return -1;
}

#if defined(__MMI_DSM_MEDIA_EXT__)
VCI32 Sky_MM_Audio_GetLevelCount()
{
	return MPC_AUD_VOL_MAX;
}
VCVoid Sky_MM_Audio_SetLevel(VCI32 nLevel)	
{
	VCI32 temp = nLevel;
	 
	kal_prompt_trace(MOD_MED, "Sky_MM_Audio_SetLevel  level: %d", nLevel);
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


VCI32 (*g_mr_media_aud_rec_cb)(VCI32 event, VCVoid *pdata, VCU32 datasize, VCI32 pridata) = NULL;
VCI32 g_mr_media_aud_rec_pridata = 0;
extern VCU8 isEarphonePlugged;

static VCVoid mr_media_aud_record_callback (mdi_result result)//event callback
{
	if(g_mr_media_aud_rec_cb)
		g_mr_media_aud_rec_cb(result, 0, 0, g_mr_media_aud_rec_pridata);
}

VCI32 mr_media_aud_record_start(VCVoid *file_name, VCI32 format,  VCVoid *callback, VCVoid *pData)
{
	VCU8 aud_format;
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
	
	g_mr_media_aud_rec_cb = (VCI32 (*)(VCI32 , VCVoid *, VCU32, VCI32 ))callback;
	g_mr_media_aud_rec_pridata = (VCI32)pData;
	if(mdi_audio_start_record(file_name, aud_format, 0, mr_media_aud_record_callback, NULL) == MDI_AUDIO_SUCCESS)
	{
		return 0;
	}
	return -1;
}

VCI32 mr_media_aud_record_stop(VCVoid)
{
	mdi_result result;

	g_mr_media_aud_rec_cb = NULL;
	g_mr_media_aud_rec_pridata = 0;
	result = mdi_audio_stop_record();

	return result;
}

VCVoid mr_media_record_data_hdlr(VCVoid* msg)
{
	mmi_media_aud_mr_data_ind_struct *local_data = (mmi_media_aud_mr_data_ind_struct*) msg;

	mr_media_send_aud_data(local_data->pData, local_data->nDataSize);
}


VCVoid mr_media_record_data_ind(VCU16*buf, VCU32 buf_size)
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

