/******************************************************************************
 ** File Name:      audio_extpcm.c                                            *
 ** Author:         Jimmy.Jia                                                 *
 ** DATE:           16/4/2007                                                 *
 ** Copyright:      2007 Spreadtrum, Incoporated. All Rights Reserved.        *
 ** Description:    an adapter layer of external PCM device for Audio Service.*
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 16/4/2007      Jimmy.Jia    	Create.                                   *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "ms_ref_aud_config_trc.h"
#include "sci_types.h"
#include "os_api.h"

#include "audio_api.h"
#include "aud_gen.h"
#include "asm.h"
#include "adm.h"
#include "apm_codec.h"
#include "adm_dp.h"

#include "audio_extpcm.h"
#include "audio_config.h"
#ifdef MTV_NOR_VERSION
#include "block_mem.h"
#endif
/**---------------------------------------------------------------------------*
 **                         Debugging Flag                                    *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    extern   "C" 
    {
#endif
/*lint -save -e527 */  
/**---------------------------------------------------------------------------*
 **                         Macro Definition                                  *
 **---------------------------------------------------------------------------*/
#define ExtPCM_SAMPLE_RATE 		(8000)
#ifdef MTV_NOR_VERSION
#define ExtPCM_BUFFER_SIZE      (48000)
#elif defined MRAPP_SUPPORT
#define ExtPCM_BUFFER_SIZE      (48000)
#else
#define ExtPCM_BUFFER_SIZE      (192000)
#endif
/**---------------------------------------------------------------------------*
 **                         Data Structures                                   *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Global Variables                                  *
 **---------------------------------------------------------------------------*/
LOCAL uint32 		s_play_track_num = ADM_INVALID_TRACK;
LOCAL HAUDIOCODEC 	hExtPCMCodecHandle;
LOCAL HAUDIO 		hExtPCMRing = SCI_NULL;
LOCAL HAUDIOCODEC 	hExtPCMCodec = SCI_NULL;
#ifdef MTV_NOR_VERSION
LOCAL	uint32* puiTrackBuffer;
#else
LOCAL uint32    s_uiTrackBuffer[ExtPCM_BUFFER_SIZE /4] = {0};
LOCAL	uint32* puiTrackBuffer = &s_uiTrackBuffer[0];
#endif
LOCAL BOOLEAN s_open_flag=SCI_FALSE;
extern HCOLLISION 	hCollision_ExtPCM;
const AUDIO_CODEC_EXT_CFGINFO_T AUDIO_EXTPCM_Codec_ExtCfgInfo = 
	{AUDIO_CODEC_PROD_HIGHPRIORITY-1,   AUDIO_CODEC_PROD_LOWPRIORITY,       80,      20,        40,        0, SCI_TRUE, 0, 0 };

/**---------------------------------------------------------------------------*
 **                         Constant Variables                                *
 **---------------------------------------------------------------------------*/
 
/**---------------------------------------------------------------------------*
 **                         Function Definitions - General Interface          *
 **---------------------------------------------------------------------------*/
LOCAL AUDIO_RESULT_E ExtPCM_Play(AUDIO_OBJECT_T* ptAudioObj);
LOCAL AUDIO_RESULT_E ExtPCM_Stop(AUDIO_OBJECT_T* ptAudioObj);
LOCAL 		  uint32 ExtPCM_GetSampleRate(AUDIO_OBJECT_T* ptAudioObj);
//LOCAL AUDIO_RESULT_E ExtPCM_SetSampleRate(HAUDIO hAudioHandle, uint32 uiSamplerate);

AUDIO_RESULT_E ExtPCM_IdentifyFormat(                 //return MIDI_NO_ERROR - midi file format ok
                                                    //       MIDI_FORMAT_ERROR - midi format is wrong 
    AUDIO_OBJECT_T* ptAudioObject
    )
{
    return AUDIO_NO_ERROR;
}

/*****************************************************************************/
//  Description:    This Function is used to initialize the audio server. 
//  Author:         Jimmy.Jia
//  Note:           
/*****************************************************************************/
PUBLIC HAUDIOCODEC ExtPCM_RegCodecPlugger(HCOLLISION hCollision, uint32 uiPriority)
{
    AUDIO_CODEC_T codec_extpcm = {0};
    
    codec_extpcm.pusCodecName = ANSI2UINT16("PCM_STACK_PLAY");
    codec_extpcm.hCollisionHandle = hCollision;
    codec_extpcm.pusCodecDescribe = SCI_NULL;
    codec_extpcm.bVerifyAudioFormat = SCI_FALSE;
    codec_extpcm.uiTrackOnPoint = ADM_DEFAULT_TRACKON_POINT;

    codec_extpcm.tExtOperation.ptExtFirstOpe = SCI_NULL;
    codec_extpcm.tExtOperation.pusExtOpeDescribe = SCI_NULL;\

    codec_extpcm.tExtCfgInfo = AUDIO_EXTPCM_Codec_ExtCfgInfo;

    codec_extpcm.tGenOperation.pConstruct        = SCI_NULL;
    codec_extpcm.tGenOperation.pDeconstruct      = SCI_NULL;
    codec_extpcm.tGenOperation.pPlayConstruct    = SCI_NULL;
    codec_extpcm.tGenOperation.pStopDeconstruct  = SCI_NULL; 
    codec_extpcm.tGenOperation.pResumeConstruct  = SCI_NULL;
    codec_extpcm.tGenOperation.pPauseDeconstruct = SCI_NULL;

    codec_extpcm.tGenOperation.pPlay = ExtPCM_Play;
    codec_extpcm.tGenOperation.pStop = ExtPCM_Stop;
    codec_extpcm.tGenOperation.pPause = SCI_NULL;
    codec_extpcm.tGenOperation.pResume = SCI_NULL;
    codec_extpcm.tGenOperation.pSeek = SCI_NULL;
    codec_extpcm.tGenOperation.pSetSpeed = SCI_NULL;
    codec_extpcm.tGenOperation.pSetDigitalGain = SCI_NULL;
    codec_extpcm.tGenOperation.pGetDigitalGain = SCI_NULL;
//    codec_extpcm.tGenOperation.pGetFileInfo = SCI_NULL;
    codec_extpcm.tGenOperation.pIdentifyAudioFormat = ExtPCM_IdentifyFormat;
    codec_extpcm.tGenOperation.pGetPlayingInfo = SCI_NULL;
    codec_extpcm.tGenOperation.pGetOutputSampleRate = ExtPCM_GetSampleRate;
//    codec_extpcm.tGenOperation.pSetSampleRate = ExtPCM_SetSampleRate;

    codec_extpcm.tExtOperation.ptExtFirstOpe = SCI_NULL;
    
    hExtPCMCodecHandle = AUDIO_PM_RegCodecPlugger(&codec_extpcm);
    return hExtPCMCodecHandle;
}

/*****************************************************************************/
//  Description:    This function plays midi.
//  Author:         Jimmy.Jia
//	Note:           
/*****************************************************************************/
LOCAL AUDIO_RESULT_E ExtPCM_Play(
    AUDIO_OBJECT_T* ptAudioObj
    )
{   

	//SCI_TRACE_LOW:"InfoNES_Sound_threadx.cpp: NES_Play"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_141_112_2_18_1_2_5_48,(uint8*)"");
    /*-------------------check input paras ------------------*/
    if(SCI_NULL == ptAudioObj) 
    {
      ASM_PASSERT_WARNING(SCI_FALSE,("SCI_NULL == ptAudioObj."));    
      return AUDIO_ERROR;
    }

	s_play_track_num = ptAudioObj->uiTrackNum;
	return AUDIO_NO_ERROR;
}    

/*****************************************************************************/
//  Description:     
//  Author:         Jimmy.Jia
//  Note:           
/*****************************************************************************/
LOCAL AUDIO_RESULT_E ExtPCM_Stop(
    AUDIO_OBJECT_T* ptAudioObj
    )
{
	//SCI_TRACE_LOW:"InfoNES_Sound_threadx.cpp: NES_Stop"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_160_112_2_18_1_2_5_49,(uint8*)"");
    /*-------------------check input paras ------------------*/
    if(SCI_NULL == ptAudioObj) 
    {
        ASM_PASSERT_WARNING(SCI_FALSE,("SCI_NULL == ptAudioObj."));    
        return AUDIO_ERROR;
    }
    return AUDIO_NO_ERROR;    
}

/*****************************************************************************/
//  Description:     
//  Author:         Jimmy.Jia
//  Note:           
/*****************************************************************************/
LOCAL uint32 ExtPCM_GetSampleRate(
    AUDIO_OBJECT_T* ptAudioObj
    )
{
     /*-------------------check input paras ------------------*/
    if(SCI_NULL == ptAudioObj) 
    {
        ASM_PASSERT_WARNING(SCI_FALSE,("SCI_NULL == ptAudioObj."));    
        return AUDIO_ERROR;
    }

    if(ptAudioObj->uiOutputSamplerate == 0)
    {
        return ExtPCM_SAMPLE_RATE;
    }
    
    return ptAudioObj->uiOutputSamplerate;
}

/*****************************************************************************/
//  Description:    This function is used to set sample rate.
//  Author:         Jimmy.Jia
//	Note:           
/*****************************************************************************/
#if 0
LOCAL AUDIO_RESULT_E ExtPCM_SetSampleRate(HAUDIO hAudioHandle, uint32 uiSamplerate)
{
    AUDIO_OBJECT_T* audio_obj = PNULL;

    audio_obj = AUDIO_SM_GetAudioObject(hAudioHandle);
    
    if(audio_obj == SCI_NULL)    
    {
        // This handle is not exist in the linklist.
        SCI_PASSERT(SCI_FALSE, ("%s, %d PNULL==audio_obj.", __MODULE__,__LINE__));/*assert verified*/
        return AUDIO_UNOPENED_HANDLE;
    }
    
    audio_obj->uiOutputSamplerate = uiSamplerate;
        
    return AUDIO_NO_ERROR;
}
#endif // 0

/*****************************************************************************/
//  Description : Audio notify callback function.
//  Global resource dependence : none
//  Author: kai.wan
//  Note: 
/*****************************************************************************/
LOCAL void AudioEXTPCMNotifyCallback(
                HAUDIO hAudio, 
                uint32 notify_info, 
                uint32 affix_info
                )
{
    //SCI_TRACE_LOW:"[AUDIO_EXTPCM]:AudioEXTPCMNotifyCallback:enter:hAudio = 0x%X, notify_info = 0x%X, affix_info = %ld"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_226_112_2_18_1_2_5_50,(uint8*)"ddd",hAudio, notify_info, affix_info);
}

/*****************************************************************************/
/*                                                                   		 */
/*       ExtPCM_SoundInit() : Extended PCM Sound Initialize				 		 */
/*                                                                   		 */
/*****************************************************************************/
void ExtPCM_SoundInit( int samples, int sample_rate )
{
	uint32	uiTrackBufferSize;
	//uint32* puiOldTrackBuffer;
	
	//uiTrackBufferSize = sample_rate*4 + 4;
		
	//SCI_TRACE_LOW:"\rExtPCM_SoundInit"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_238_112_2_18_1_2_6_51,(uint8*)"");
	
	// Avoid to register again
	if (hExtPCMCodec == SCI_NULL)
	{
		hExtPCMCodec  = ExtPCM_RegCodecPlugger(hCollision_ExtPCM, AUDIO_CODEC_PROD_HIGHPRIORITY);	
	}
	if (hExtPCMRing == SCI_NULL)
	{
#ifdef MTV_NOR_VERSION
		puiTrackBuffer = (uint32 *)BL_Malloc(BLOCK_MEM_POOL_MTV_EXTPCM);
        uiTrackBufferSize = BL_GetSize(BLOCK_MEM_POOL_MTV_EXTPCM);
		SCI_ASSERT (puiTrackBuffer != SCI_NULL); /*assert verified*/
		SCI_MEMSET(puiTrackBuffer, 0, uiTrackBufferSize);
#else		
        uiTrackBufferSize = ExtPCM_BUFFER_SIZE;
		SCI_MEMSET(&s_uiTrackBuffer[0], 0, uiTrackBufferSize);
#endif
		// Alligned by Word
		#if 0
		puiTrackBuffer = (uint32 *)(((uint32)puiTrackBuffer + 0x3) & ~0x3);
		uiTrackBufferSize -= 4;
		#else
		//puiOldTrackBuffer = puiTrackBuffer;
		//puiTrackBuffer = (uint32 *)(((uint32)puiTrackBuffer + 0x3) & ~0x3);
		//uiTrackBufferSize -= 4;
		#endif
        
		hExtPCMRing = AUDIO_CreateBufferHandle (hExtPCMCodec, 
											 SCI_NULL, 
											 hARMVB ,
											 SCI_NULL, 
											 SCI_NULL, 
											 0, 
											 (uint32 *)puiTrackBuffer, 
											 (uiTrackBufferSize/4),
											 AudioEXTPCMNotifyCallback);
		
		SCI_ASSERT (hExtPCMRing != SCI_NULL);/*assert verified*/
		
		#if 1
		//puiTrackBuffer = puiOldTrackBuffer;
		#endif
	}	
}

void ExtPCM_SoundExit( void )
{
	//SCI_TRACE_LOW:"\rExtPCM_SoundExit"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_286_112_2_18_1_2_6_52,(uint8*)"");
	if (hExtPCMRing != SCI_NULL)
	{
		AUDIO_CloseHandle(hExtPCMRing);
		hExtPCMRing = SCI_NULL;
	}
#ifdef MTV_NOR_VERSION
	if (puiTrackBuffer != SCI_NULL)
	{
		BL_Free(BLOCK_MEM_POOL_MTV_EXTPCM);
	}
#endif
}

/*****************************************************************************/
/*                                                                   		 */
/*        ExtPCM_SoundOpen() : Extend PCM Sound Open                      	 */
/*                                                                   		 */
/*        samples: PCM number per calling Soundoutput				 		 */
/*        sample_rate: PCM samplerate								 		 */
/*****************************************************************************/
int ExtPCM_SoundOpen( int samples, int sample_rate )
{
	AUDIO_RESULT_E audio_result;
		
	SCI_ASSERT (hExtPCMRing != SCI_NULL);/*assert verified*/

	//SCI_TRACE_LOW:"\rExtPCM_SoundOpen samples %d, sample_rate %d"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_309_112_2_18_1_2_6_53,(uint8*)"dd", samples, sample_rate);
	
	// Update the samplerate
	AUDIO_SetOutputSampleRate (hExtPCMRing, sample_rate);

	audio_result = AUDIO_Play (hExtPCMRing, 0);

	SCI_ASSERT( audio_result == AUDIO_NO_ERROR);/*assert verified*/
	s_open_flag = SCI_TRUE;
	// Always Successful
	return 1;
}

/*****************************************************************************/
/*                                                                   		 */
/*        ExtPCM_SoundClose() : Extended PCM Sound Close                    		 */
/*                                                                   		 */
/*****************************************************************************/
void ExtPCM_SoundClose( void )
{
	AUDIO_RESULT_E audio_result;

	if(s_open_flag == SCI_FALSE)
	{
		return;
	}
	SCI_ASSERT (hExtPCMRing != SCI_NULL);/*assert verified*/
	
	//SCI_TRACE_LOW:"\rExtPCM_SoundClose"
	SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_334_112_2_18_1_2_6_54,(uint8*)"");
	
	audio_result = AUDIO_Stop (hExtPCMRing);

	SCI_ASSERT( audio_result == AUDIO_NO_ERROR);/*assert verified*/
	s_open_flag = SCI_FALSE;
}

/*****************************************************************************/
/*                                                                   		 */
/*       ExtPCM_SoundOutput() :  Extended Sound Output samples in word  		 */
/*       system need to write the PCM in ringbuffer                 	     */
/*                                                                  		 */
/*        samples: PCM number per calling Soundoutput						 */
/*        buff_ptr: PCM buffer pointer										 */
/*****************************************************************************/
void ExtPCM_SoundOutput( int samples, int16 *buff_ptr)
{    
    if(SCI_NULL == buff_ptr)
    {
        //SCI_TRACE_LOW:"ExtPCM_SoundOutput error, SCI_NULL == buff_ptr."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_350_112_2_18_1_2_6_55,(uint8*)"");
        return;
    }
    SCI_ASSERT (hExtPCMRing != SCI_NULL);/*assert verified*/
	SCI_ASSERT (s_play_track_num != ADM_INVALID_TRACK);/*assert verified*/
	
	// Write back to track buffer managed by ADM
	if (samples != 0)
	{
	    AUDIO_DP_WriteRingBuffer(	s_play_track_num, 
									(int16*)buff_ptr, 
									(int16*)buff_ptr, 
									samples);    	
	}
}

/*****************************************************************************/
/*                                                                   		 */
/*       ExtPCM_SoundOutputStereo() :  Extended Sound Output samples in word  		 */
/*       system need to write the PCM in ringbuffer                 	     */
/*                                                                  		 */
/*        samples: PCM number per calling Soundoutput						 */
/*        lbuff_ptr: left channel PCM buffer pointer										 */
/*        rbuff_ptr: right channel PCM buffer pointer										 */
/*****************************************************************************/
void ExtPCM_SoundOutputStereo( int samples, int16 *lbuff_ptr, int16 *rbuff_ptr)
{ 
    if(SCI_NULL == lbuff_ptr)
    {
        //SCI_TRACE_LOW:"ExtPCM_SoundOutputStereo error, SCI_NULL == lbuff_ptr."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_374_112_2_18_1_2_6_56,(uint8*)"");
        return;
    }
    if(SCI_NULL == rbuff_ptr)
    {
        //SCI_TRACE_LOW:"ExtPCM_SoundOutputStereo error, SCI_NULL == rbuff_ptr."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_EXTPCM_379_112_2_18_1_2_6_57,(uint8*)"");
        return;
    }
    SCI_ASSERT (hExtPCMRing != SCI_NULL);/*assert verified*/
	SCI_ASSERT (s_play_track_num != ADM_INVALID_TRACK);/*assert verified*/
	
	// Write back to track buffer managed by ADM
	if (samples != 0)
	{
	    AUDIO_DP_WriteRingBuffer(	s_play_track_num, 
									(int16*)lbuff_ptr, 
									(int16*)rbuff_ptr, 
									samples);    	
	}
}

/*****************************************************************************/
/*                                                                  		 */
/*       ExtPCM_SoundGetPCMNum() :  Extended PCM Sound Output samples in word 		 */
/*       system need to write the PCM in ringbuffer                 		 */
/*                                                                  		 */
/*       return PCM in halfword num								 			 */
/*****************************************************************************/
int ExtPCM_SoundGetPCMNum(void)
{
	SCI_ASSERT (s_play_track_num != ADM_INVALID_TRACK);/*assert verified*/

	return (AUDIO_DP_GetDecoderDataLen (s_play_track_num));
}
/*lint -restore */  

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

