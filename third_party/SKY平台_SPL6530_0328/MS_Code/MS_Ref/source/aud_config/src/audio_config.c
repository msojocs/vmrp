/******************************************************************************
 ** File Name:      audio_sp7320.c                                            *
 ** Author:         Jimmy.Jia                                                 *
 ** DATE:           07/27/2004                                                *
 ** Copyright:      2005 Spreadtrum, Incoporated. All Rights Reserved.        *
 ** Description:                                                              *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE            NAME            DESCRIPTION                               *
 ** 03/02/2005      Jimmy.Jia       Create for sp7320                         *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "ms_ref_aud_config_trc.h"
#include "os_api.h"
#include "tb_dal.h"

#include "audio_config.h"

#include "Layer1_audio.h"
#include "layer1_sbc.h"

#include "aud_gen.h"
#include "audio_api.h"
#include "acm_class_lib.h"
#include "adm.h"
#include "asm_hc.h"

#include "voice_adp.h"
#include "vt_adp.h"
#include "custom_tone_adp.h"
#include "dtmf_tone_adp.h"
#include "generic_tone_adp.h"
#include "dsp_codec_adp.h"
#include "adpcmrecord_adp.h"

#ifdef NULL_CODEC_SUPPORT
#include "null_codec_adp.h"
#endif

#include "clone_codec_adp.h"

#ifdef AD_OUTPUT_ADP_SUPPORT
#include "ad_output_adp.h"
#endif

#ifdef ARM_AD_ADP_SUPPORT
#include "arm_ad_adp.h"
#endif

#ifdef UPLINK_CODEC_SUPPORT
#include "uplink_codec_adp.h"
#endif

#ifdef DOWNLINK_CODEC_SUPPORT
#include "downlink_codec_adp.h"
#endif

#ifdef MP3_SUPPORT
#ifdef MP3_DECODE_IN_DSP
#include "mp3_dsp_adp.h"
#else
#include "mp3_adp.h"
#endif
#endif

#ifdef AAC_SUPPORT
#ifdef AAC_DECODE_IN_ARM
#include "aac_adp.h"
#else
#include "aac_lc_adp.h"
#endif
#endif

#ifdef WMA_SUPPORT
#include "wma_adp.h"
#endif

#ifdef AMR_SUPPORT
#ifdef AMR_DECODE_IN_DSP
#include "amr_dsp_adp.h"
#else
#include "amr_adp.h"
#endif
#endif

#ifdef MIDI_SUPPORT
#include "midi_adp.h"
#endif

#ifdef WAV_SUPPORT
#include "wav_adp.h"
#endif


#ifdef CAT_MVOICE_SUPPORT  
#include "cat_mvoice.h"
#endif

#ifdef DRA_SUPPORT
#include "dra_adp.h"
#endif

#ifdef TA8LBR_SUPPORT
#include "ra8lbr_adp.h"
#endif

#include "voiceprocess_adp.h"
#include "transSamplerate.h"

#ifdef MP3_EQ_SUPPORT
#ifndef EQ_EXE_ON_CHIP
#include "eq_exp.h"
#include "agc_exp.h"
#endif
#else
#include "agc_exp.h"
#endif

#include "aud_proc_exp.h"
#include "aud_enha_exp.h"
#ifdef DIGITAL_GAIN_SUPPORT
#include "digitalgain.h"
#endif
#ifdef RATE_CONTROL_SUPPORT
#include "ratecontrol_exp.h"
#endif

#include "deep_sleep.h"
#include "gpio_prod_api.h"
#include "vb_drv.h"
#include "pcm_dev.h"
#include "armvb_as.h"
#include "dspvb_as.h"
#include "l1rec_dev.h"

#ifdef BLUETOOTH_SUPPORT
#include "l1rec_dev.h"
#ifdef BT_A2DP_SUPPORT
#include "a2dp_api.h"
#endif
#endif /* BLUETOOTH_SUPPORT  */

#ifdef BACKGROUND_MUSIC_ENABLE
#include "bkmusic_as.h"
#endif

#ifdef MET_MEX_SUPPORT
#include "mex_ref.h"
#endif
/**---------------------------------------------------------------------------*
 **                         Debugging Flag                                    *
 **---------------------------------------------------------------------------*/
/*lint -save -e765 */
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
extern   "C"
{
#endif
#ifndef __arm
#ifndef __MODULE__
#define __MODULE__ "audio_config.c"
#endif
#endif
#include "priority_system.h"

/**---------------------------------------------------------------------------**
 **                         MACRO Definations                                 **
 **---------------------------------------------------------------------------**/
#define PAD (0)
#define TASK_AUD_DSP_PRI  PRI_AUD_DSP_TASK  //15
/**---------------------------------------------------------------------------*
 **                            Data structure                                 *
 **---------------------------------------------------------------------------*/
typedef enum
{
    MIDI_EXTINFO,
    AAC_EXTINFO,
    WAV_EXTINFO,
    AACLC_EXTINFO,
    MP3_EXTINFO,
    WMA_EXTINFO,
    AMR_EXTINFO,
    ExtPCM_EXTINFO,
    RECORD_EXIINFO,
    MP3_DSP_EXTINFO,
    AMR_DSP_EXTINFO,
    DRA_EXTINFO,
    RA8LBR_EXTINFO,
    CODEC_NOCODECTASK_EXTINFO,
    CODEC_TYPE_MAX
}AUDIO_CODEC_EXTINFO_E;

//hExp is not used for hDev.
typedef struct
{
	HAUDIOEXP hExp;
	HAUDIODEV hDev;
}AUDIO_EXPNOTWORKED_FORTHISDEV_CONFIG;

LOCAL AUDIO_CODEC_EXT_CFGINFO_T AUDIO_Codec_ExtCfgInfo_Table[CODEC_TYPE_MAX] =
{
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_HIGHPRIORITY,       80,      20,        40,        80, SCI_FALSE,PAD,PAD},//MIDI
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,       80, SCI_TRUE ,PAD,PAD },//AAC
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,       80, SCI_TRUE ,PAD,PAD},//WAV
    {AUDIO_CODEC_PROD_HIGHPRIORITY, AUDIO_CODEC_PROD_HIGHPRIORITY,     80,      20,        40,      80, SCI_FALSE,PAD,PAD },//AACLC
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,       80, SCI_TRUE ,PAD,PAD}, //MP3
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,       80, SCI_TRUE ,PAD,PAD},//WMA
    {AUDIO_CODEC_PROD_HIGHPRIORITY,    AUDIO_CODEC_PROD_LOWPRIORITY,      80,       20,         40,       80, SCI_TRUE,PAD,PAD},//AMR
    {AUDIO_CODEC_PROD_LOWPRIORITY,    AUDIO_CODEC_PROD_LOWPRIORITY,      100,       0,         0,        0, SCI_FALSE,PAD,PAD},//ExtPCM
    {AUDIO_CODEC_PROD_HIGHPRIORITY,    AUDIO_CODEC_PROD_LOWPRIORITY,      100,       0,         0,        0, SCI_FALSE,PAD,PAD},//RECORD
    {AUDIO_CODEC_PROD_HIGHPRIORITY, AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,      80, SCI_TRUE,PAD,PAD},//MP3_DSP
    {AUDIO_CODEC_PROD_HIGHPRIORITY, AUDIO_CODEC_PROD_HIGHPRIORITY,     80,      20,        40,      80, SCI_FALSE,PAD,PAD},//AMR_DSP
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,       80, SCI_TRUE ,PAD,PAD },//DRA
    {AUDIO_CODEC_PROD_HIGHPRIORITY,   AUDIO_CODEC_PROD_LOWPRIORITY,     80,      20,        40,       80, SCI_TRUE ,PAD,PAD },//RA8LBR
    {                    SCI_NULL,                        SCI_NULL, SCI_NULL, SCI_NULL, SCI_NULL,  SCI_NULL,SCI_NULL ,PAD,PAD},//No Codec Task
};

/**---------------------------------------------------------------------------*
 **                         Const variables                                   *
 **---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
 **                         Global variables                                  *
 **---------------------------------------------------------------------------*/
HAUDIOCODEC hAMRCodec;
HAUDIOCODEC hVoiceCodec;
HAUDIOCODEC hCustomCodec;
HAUDIOCODEC hDtmfCodec;
HAUDIOCODEC hGenericCodec;
HAUDIOCODEC hDSPCodec;
HAUDIOCODEC hWAVCodec;
HAUDIOCODEC hMIDICodec;
HAUDIOCODEC hMP3Codec;
HAUDIOCODEC hAACCodec;
HAUDIOCODEC hRecordAdpcmCodec;
HAUDIOCODEC hRecordCodec;
HAUDIOCODEC hMP3DSPCodec;
HAUDIOCODEC hAMRDSPCodec;
HAUDIOCODEC hNullCodec;
HAUDIOCODEC hArmAdCodec;
HAUDIOCODEC hAdOutputCodec;
#ifdef DRA_SUPPORT
HAUDIOCODEC hDRACodec;
#endif

#ifdef TA8LBR_SUPPORT
HAUDIOCODEC hRA8LBRCodec;
#endif

#ifndef WIN32
#ifdef UPLINK_CODEC_SUPPORT
HAUDIOCODEC hUplinkCodec;
#endif

#ifdef DOWNLINK_CODEC_SUPPORT
HAUDIOCODEC hDownlinkCodec;
#endif
#endif

#ifdef VT_SUPPORT
HAUDIOCODEC hVtCodec;
#endif

HAUDIOCODEC hAACLCCodec;
HAUDIOCODEC hWMACodec;
HAUDIOCODEC hVoiceProcessCodec;

#ifdef MTV_SUPPORT
LOCAL HCOLLISION hCollision_ExtPCM = 0;
LOCAL HCOLLISION hCollision_MTV    = 0;
#elif defined  MRAPP_SUPPORT
HCOLLISION hCollision_ExtPCM = 0;
#endif
LOCAL HCOLLISION hCollision_MIDI = 0;
LOCAL HCOLLISION hCollision_AACLC = 0;
LOCAL HCOLLISION hCollision_WAV = 0;
LOCAL HCOLLISION hCollision_AMR = 0;
LOCAL HCOLLISION hCollision_MP3 = 0;
LOCAL HCOLLISION hCollision_MP3DSP = 0;
LOCAL HCOLLISION hCollision_WMA = 0;
LOCAL HCOLLISION hCollision_Voice = 0;
LOCAL HCOLLISION hCollision_Custom = 0;
LOCAL HCOLLISION hCollision_Dtmf = 0;
LOCAL HCOLLISION hCollision_Generic = 0;
LOCAL HCOLLISION hCollision_Record = 0;
LOCAL HCOLLISION hCollision_DspCodec = 0;
LOCAL HCOLLISION hCollision_AMRDSP = 0;
LOCAL HCOLLISION hCollision_NULL = 0;
LOCAL HCOLLISION hCollision_ARMAD = 0;
LOCAL HCOLLISION hCollision_ADOUTPUT = 0;

#ifdef UPLINK_CODEC_SUPPORT
LOCAL HCOLLISION hCollision_UplinkCodec = 0;
#endif
#ifdef DOWNLINK_CODEC_SUPPORT
LOCAL HCOLLISION hCollision_DownlinkCodec = 0;
#endif


#ifdef VT_SUPPORT
LOCAL HCOLLISION hCollision_VtCodec = 0;
#endif

#ifdef CAT_MVOICE_SUPPORT  
HCOLLISION hCollision_CatMic = 0;
#endif

LOCAL uint32 hClass1;
LOCAL uint32 hClass2;
LOCAL uint32 hClass3;
LOCAL uint32 hClass4;
LOCAL uint32 hClass5;

#ifdef DRA_SUPPORT
HCOLLISION hCollision_DRA = 0;
#endif

#ifdef TA8LBR_SUPPORT
HCOLLISION hCollision_RA8LBR = 0;
#endif
HCOLLISION hCollision_VOICEPROCESS = 0;
HAUDIODEV hARMVB;
HAUDIODEV hARMVBRECORD;
HAUDIODEV hLAYER1;
HAUDIODEV hLOOPBACKDEV;
HAUDIODEV hRECORDVB;
HAUDIODEV hMP4RECORDVB;
HAUDIODEV hDSPDATADEVICE;
// @bluetooth
HAUDIODEV hPCMDev;

HAUDIODEV hLINEINDev;
HAUDIODEV hLINEINADDev;
HAUDIODEV hMTVVB;

#ifdef WIN32
HAUDIODEV  hWIN32_DEV;
#endif

#ifdef BLUETOOTH_SUPPORT

HAUDIODEV hL1Rec;
#ifdef BT_A2DP_SUPPORT
#endif /* BT_A2DP_SUPPORT */
// --bluetooth
#endif

HAUDIODEV hA2DPDev;

#ifdef BACKGROUND_MUSIC_ENABLE
HAUDIODEV hBackgroundMusicDev;
static unsigned short const s_bkmusic_gain_table[AUD_BKMUSIC_TOTAL_LEVEL] =
{0x0, 0x150, 0x29f, 0x5a7, 0xb51, 0x16a7};
#endif

HAUDIOEXP hTransSamExp;

#ifdef MP3_EQ_SUPPORT
#ifndef EQ_EXE_ON_CHIP
HAUDIOEXP hEQExp;
HAUDIOEXP hAGCExp;
#endif
#else
HAUDIOEXP hAGCExp;
#endif

HAUDIOEXP hAUDPROCExp = INVALID_HANDLE;
HAUDIOEXP hAUDENHAExp = INVALID_HANDLE;
HAUDIOEXP hDigitalGainExp = INVALID_HANDLE;
#ifdef RATE_CONTROL_SUPPORT
HAUDIOEXP hRateControlExp;
#endif

#ifdef CAT_MVOICE_SUPPORT
HAUDIOEXP hCatMvMixExp;
HAUDIOCODEC hCatMvSampleCodec;
HAUDIO hCatMvsample;
#endif

#ifdef MIC_SAMPLE_SUPPORT

#include "mic_sample.h"

HAUDIOCODEC hMicSampleCodec=0;
HCOLLISION  hCollision_MicSample = 0;
HAUDIO      hMicSample=0;

#endif


#ifndef TIANLONG_MOCOR
extern uint32 LAYER1_GetAudioEngineeringParameter (// If succeed, return SCI_SUCCESS,
    // else return SCI_ERROR
    AUDIO_DEVICE_MODE_TYPE_E  parameter_type,
    AUDIO_TEST_DATA_T   *param_data
);
#endif


LOCAL void AudioDummyNotifyCallback(
                HAUDIO hAudio, 
                uint32 notify_info, 
                uint32 affix_info
                );


/**---------------------------------------------------------------------------*
 **                     Local Function Prototypes                             *
 **---------------------------------------------------------------------------*/
#if defined(DRA_SUPPORT) || defined(TA8LBR_SUPPORT)
LOCAL void CfgAudCodecExp(HAUDIOCODEC hAudCodec);
#endif 
/**---------------------------------------------------------------------------*
 **                         Function Definitions                              *
 **---------------------------------------------------------------------------*/
#if defined(DRA_SUPPORT) || defined(TA8LBR_SUPPORT)
/*****************************************************************************/
//  Description:    This function config dra codec expression.
//  Author:         TH
//  Note:
/*****************************************************************************/
LOCAL void CfgAudCodecExp(HAUDIOCODEC hAudCodec)
{
#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hAudCodec, hTransSamExp);
#endif

#ifdef RATE_CONTROL_SUPPORT
    AUDIO_PM_AddCodecExpress (hAudCodec, hRateControlExp);
#endif
#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hAudCodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hAudCodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hAudCodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hAudCodec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hAudCodec, hAGCExp);
#endif

}
#endif
LOCAL void VoiceDummyNotifyCallback(
                HAUDIO hAudio, 
                uint32 notify_info, 
                uint32 affix_info
                )
{
}
/*****************************************************************************/
//  Description:    This function initializes audio custome interface.
//  Author:         Xueliang.Wang
//  Note:
/*****************************************************************************/
#ifndef MODEM_PLATFORM
LOCAL void AUD_CUS_Init (void)
{
    CLASS1PRIVATEINFO_T class1_priv_info;
    CLASS3PRIVATEINFO_T class3_priv_info;
    uint32 t = 0;
	for(t = 0; t < sizeof(AUDIO_Codec_ExtCfgInfo_Table)/sizeof(AUDIO_Codec_ExtCfgInfo_Table[0]); t++)
	{
		if(AUDIO_Codec_ExtCfgInfo_Table[t].uiHighPriority)
		{
			AUDIO_Codec_ExtCfgInfo_Table[t].uiHighPriority = PRI_AUDIO_CODEC_HIGH & 0xFF;
		}
		else
		{
			AUDIO_Codec_ExtCfgInfo_Table[t].uiHighPriority = PRI_AUDIO_CODEC_LOW  & 0xFF;
		}

		if(AUDIO_Codec_ExtCfgInfo_Table[t].uiLowPriority)
		{
			AUDIO_Codec_ExtCfgInfo_Table[t].uiLowPriority = PRI_AUDIO_CODEC_HIGH  & 0xFF;
		}
		else
		{
			AUDIO_Codec_ExtCfgInfo_Table[t].uiLowPriority = PRI_AUDIO_CODEC_LOW  & 0xFF;
		}
	}
    AUDIO_Init();

    //create audio collison object

    //create audio collison object
#ifdef MTV_SUPPORT
    hCollision_ExtPCM = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_ExtPCM, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_MTV = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_MTV, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }
#elif defined  MRAPP_SUPPORT
    hCollision_ExtPCM = AUDIO_CM_CreateCollisionObject(AUDIO_PRIORITY_MIDDLE);
    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle(hCollision_ExtPCM, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#endif
    //create audio collison object
    hCollision_MIDI = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_MIDI, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_WAV = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_WAV, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }
    
    hCollision_NULL = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_NULL, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    } 

    hCollision_ARMAD = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_ARMAD, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    } 

    hCollision_ADOUTPUT = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_ADOUTPUT, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    } 
    
    hCollision_AMR = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_AMR, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_AACLC = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_AACLC, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_MP3 = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_MP3, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

  #ifdef CAT_MVOICE_SUPPORT  
    hCollision_CatMic = AUDIO_CM_CreateCollisionObject(AUDIO_PRIORITY_MIDDLE);;
    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle(hCollision_CatMic, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT(FALSE); /*assert verified*/
    }
  #endif  
  
 #ifdef MIC_SAMPLE_SUPPORT  
    hCollision_MicSample = AUDIO_CM_CreateCollisionObject(AUDIO_PRIORITY_MIDDLE);
    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle(hCollision_MicSample, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT(FALSE); /*assert verified*/
    }
  #endif 
 
    hCollision_MP3DSP = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_MP3DSP, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_AMRDSP = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_AMRDSP, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Record = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Record, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_WMA = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_WMA, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Voice = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_HIGHEST);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Voice, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Custom = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Custom, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Dtmf = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Dtmf, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Generic = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Generic, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_DspCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_DspCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }


#ifdef UPLINK_CODEC_SUPPORT
    hCollision_UplinkCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_UplinkCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#endif
#ifdef DOWNLINK_CODEC_SUPPORT
    hCollision_DownlinkCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_DownlinkCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#endif

#ifdef VT_SUPPORT
    hCollision_VtCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_HIGHEST);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_VtCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#endif

#ifdef DRA_SUPPORT
    hCollision_DRA = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_DRA, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }
#endif    

#ifdef TA8LBR_SUPPORT
    hCollision_RA8LBR = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);
    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_RA8LBR, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }
#endif
    hCollision_VOICEPROCESS = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);
    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_VOICEPROCESS, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //create audio collision class1
    hClass1 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS1", ACM_Policy_DifferentLevelCollide, sizeof (CLASS1PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass1, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        //audio collision class handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class1 to collision object
    //DSP voice band, level = 0; ARM voice band, level = 1.
    //    codecs used DSP VB is a level, including AMR, voice,
    //                                 custom tone, dtmf tone, generic tone.
    //    codecs used ARM VB is another level, including MIDI, MP3,
    //                                                  ADPCM, AAC, PCM.
    class1_priv_info.level = 1;
    //AUDIO_CM_RegCollisionClass(hClass1, hCollision_WAV, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_MIDI, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_AACLC, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_MP3, &class1_priv_info);
    
#ifdef TA8LBR_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_RA8LBR, &class1_priv_info);
#endif
    
#ifdef CAT_MVOICE_SUPPORT
    AUDIO_CM_RegCollisionClass(hClass1, hCollision_CatMic, &class1_priv_info);
#endif

#ifdef MIC_SAMPLE_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_MicSample, &class1_priv_info);
#endif

    AUDIO_CM_RegCollisionClass (hClass1, hCollision_WMA, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_MP3DSP, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_AMRDSP, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_AMR, &class1_priv_info);
#ifdef DRA_SUPPORT    
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_DRA, &class1_priv_info);
#endif
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_ARMAD, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_ADOUTPUT, &class1_priv_info);
#ifdef MTV_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_ExtPCM, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_MTV, &class1_priv_info);
#endif

    class1_priv_info.level = 0;
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Voice, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Custom, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Dtmf, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Generic, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_DspCodec, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Record, &class1_priv_info);

#ifdef UPLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_UplinkCodec, &class1_priv_info);
#endif

#ifdef DOWNLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_DownlinkCodec, &class1_priv_info);
#endif

#ifdef VT_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_VtCodec, &class1_priv_info);
#endif

    //create audio collision class2
    hClass2 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS2", ACM_Policy_Exclusive, 0);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass2, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class2 to collision object
    //    this policy uses no private informations.
    //     following codec is exclusive: MIDI, MP3, AAC.
    //      so they must use the same class which adopts this policy.
#ifdef MTV_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_ExtPCM, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_MTV, NULL);
#elif defined  MRAPP_SUPPORT
    AUDIO_CM_RegCollisionClass(hClass2, hCollision_ExtPCM, NULL);  
#endif
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_MIDI, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_AACLC, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_MP3, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_WMA, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_DspCodec, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_MP3DSP, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_AMRDSP, NULL);
//    AUDIO_CM_RegCollisionClass (hClass2, hCollision_WAV, NULL);
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_AMR, NULL);
    
#ifdef TA8LBR_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_RA8LBR,NULL);
#endif

#ifdef DRA_SUPPORT    
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_DRA, NULL);
#endif
    //    AUDIO_CM_RegCollisionClass(hClass2, hCollision_Voice, NULL);
#ifdef VT_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_VtCodec, &class1_priv_info);
#endif

    //create audio collision class3
    hClass3 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS3", ACM_Policy_SinglePlay, sizeof (CLASS3PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass3, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class3 to collision object
    //      codec with isSinglePlay equal to 1 includes MIDI, MP3, AAC, vioce,
    //                                   custom tone, dtmf tone, generic tone
    //      codec with isSinglePlay equal to 0 includes ADPCM, PCM.
    class3_priv_info.isSinglePlay = 1;
#ifdef MTV_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_ExtPCM, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_MTV, &class3_priv_info);
#elif defined  MRAPP_SUPPORT
    AUDIO_CM_RegCollisionClass(hClass3, hCollision_ExtPCM, &class3_priv_info);  
#endif
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_MIDI, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_MP3, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_AACLC, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_Record, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_MP3DSP, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_AMRDSP, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_AMR, &class3_priv_info);
//    AUDIO_CM_RegCollisionClass (hClass3, hCollision_WAV, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_DspCodec, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_WMA, &class3_priv_info);
    
#ifdef TA8LBR_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_RA8LBR,&class3_priv_info);
#endif

#ifdef DRA_SUPPORT    
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_DRA, &class3_priv_info);
#endif
    class3_priv_info.isSinglePlay = 0;
    //AUDIO_CM_RegCollisionClass(hClass3, hCollision_WAV, &class3_priv_info);
    //AUDIO_CM_RegCollisionClass(hClass3, hCollision_PCM, &class3_priv_info);

    hClass4 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS4", ACM_Policy_DifferentLevelCollide, sizeof (CLASS1PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass4, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        //audio collision class handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class4 to collision object
    //use this class to describe the relationship betwwen adpcm_record and other codecs.
    //level = 0: adpcm_record, hCollision_Voice
    //level = 1. hCollision_PCM, hCollision_AACLC, hCollision_MIDI, hCollision_MP3
    class1_priv_info.level = 0;
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_Voice, &class1_priv_info);
#ifdef VT_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_VtCodec, &class1_priv_info);
#endif
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_Record, &class1_priv_info);

#ifdef UPLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_UplinkCodec, &class1_priv_info);
#endif

#ifdef DOWNLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_DownlinkCodec, &class1_priv_info);
#endif

    class1_priv_info.level = 1;
#ifdef MTV_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_ExtPCM, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_MTV, &class1_priv_info);
#elif defined  MRAPP_SUPPORT
    AUDIO_CM_RegCollisionClass(hClass4, hCollision_ExtPCM, &class1_priv_info);   
#endif
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_MIDI, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_AACLC, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_MP3, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_WMA, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_MP3DSP, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_AMRDSP, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_AMR, &class1_priv_info);
    
#ifdef TA8LBR_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_RA8LBR,&class1_priv_info);
#endif

#ifdef DRA_SUPPORT    
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_DRA, &class1_priv_info);
#endif

    //create audio collision class3
    hClass5 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS5", ACM_Policy_SinglePlay, sizeof (CLASS3PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass5, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class3 to collision object
    //      codec with isSinglePlay equal to 1 includes MIDI, MP3, AAC, vioce,
    //                                   custom tone, dtmf tone, generic tone
    //      codec with isSinglePlay equal to 0 includes ADPCM, PCM.
    class3_priv_info.isSinglePlay = 1;
    AUDIO_CM_RegCollisionClass (hClass5, hCollision_Custom, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass5, hCollision_Dtmf, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass5, hCollision_Generic, &class3_priv_info);


#ifdef MP3_SUPPORT
#ifdef MP3_DECODE_IN_DSP
    hMP3DSPCodec = MP3_DSP_RegCodecPlugger (hCollision_MP3DSP, & (AUDIO_Codec_ExtCfgInfo_Table[MP3_DSP_EXTINFO]));
    hMP3Codec = hMP3DSPCodec;
#else
    hMP3Codec = MP3_RegCodecPlugger (hCollision_MP3, & (AUDIO_Codec_ExtCfgInfo_Table[MP3_EXTINFO]));
#endif
#endif

#ifdef AAC_SUPPORT
#ifdef AAC_DECODE_IN_ARM
    hAACCodec = AAC_RegCodecPlugger (hCollision_AACLC, & (AUDIO_Codec_ExtCfgInfo_Table[AAC_EXTINFO]));
    hAACLCCodec = hAACCodec;
#else
    hAACCodec = AAC_LC_RegCodecPlugger (hCollision_AACLC, & (AUDIO_Codec_ExtCfgInfo_Table[AACLC_EXTINFO]));
    hAACLCCodec = hAACCodec;
#endif
#endif

#ifdef AMR_SUPPORT
#ifdef AMR_DECODE_IN_DSP
    hAMRDSPCodec = AMR_DSP_RegCodecPlugger (hCollision_AMRDSP, & (AUDIO_Codec_ExtCfgInfo_Table[AMR_DSP_EXTINFO]));
    hAMRCodec = hAMRDSPCodec;
#else
    hAMRCodec = AMR_RegCodecPlugger (hCollision_AMR, & (AUDIO_Codec_ExtCfgInfo_Table[AMR_EXTINFO]));
#endif
#endif

#ifdef MIDI_SUPPORT
    hMIDICodec = MIDI_RegCodecPlugger (hCollision_MIDI, & (AUDIO_Codec_ExtCfgInfo_Table[MIDI_EXTINFO]));
#endif

#ifdef WAV_SUPPORT
    hWAVCodec = WAV_RegCodecPlugger (hCollision_WAV, & (AUDIO_Codec_ExtCfgInfo_Table[WAV_EXTINFO]));
#endif

    hRecordCodec = Record_RegCodecPlugger (hCollision_Record, & (AUDIO_Codec_ExtCfgInfo_Table[RECORD_EXIINFO]));
    hRecordAdpcmCodec = hRecordCodec;
    hVoiceCodec = VOICE_RegCodecPlugger (hCollision_Voice, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#ifdef VT_SUPPORT
    hVtCodec = VT_RegCodecPlugger (hCollision_VtCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
    hCustomCodec = CUSTOM_TONE_RegCodecPlugger (hCollision_Custom, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hDtmfCodec = DTMF_TONE_RegCodecPlugger (hCollision_Dtmf, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hGenericCodec = GENERIC_TONE_RegCodecPlugger (hCollision_Generic, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hDSPCodec =  DSP_CODEC_RegCodecPlugger (hCollision_DspCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    
#ifndef WIN32
#ifdef UPLINK_CODEC_SUPPORT
    hUplinkCodec = UPLINK_RegCodecPlugger (hCollision_UplinkCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
#ifdef DOWNLINK_CODEC_SUPPORT
    hDownlinkCodec = DOWNLINK_RegCodecPlugger (hCollision_DownlinkCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
#endif

#ifndef WIN32
#ifdef NULL_CODEC_SUPPORT
    hNullCodec = NULL_CODEC_RegCodecPlugger (hCollision_NULL, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
    hCloneCodec = CLONE_CODEC_RegCodecPlugger (hCollision_NULL, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));

#ifdef ARM_AD_ADP_SUPPORT    
    hArmAdCodec = ARM_AD_RegCodecPlugger (hCollision_ARMAD, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
#ifdef AD_OUTPUT_ADP_SUPPORT
    hAdOutputCodec = AD_OUTPUT_RegCodecPlugger (hCollision_ADOUTPUT, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
#ifdef WMA_SUPPORT
    hWMACodec = WMA_RegCodecPlugger (hCollision_WMA, & (AUDIO_Codec_ExtCfgInfo_Table[WMA_EXTINFO]));
#endif
#endif


#ifdef DRA_SUPPORT
    hDRACodec = DRA_RegCodecPlugger(hCollision_DRA, &(AUDIO_Codec_ExtCfgInfo_Table[DRA_EXTINFO]));
#endif

#ifdef TA8LBR_SUPPORT
    hRA8LBRCodec = RA8LBR_RegCodecPlugger(hCollision_RA8LBR, &(AUDIO_Codec_ExtCfgInfo_Table[RA8LBR_EXTINFO]));
#endif
    hVoiceProcessCodec = VoiceProcess_RegCodecPlugger(hCollision_VOICEPROCESS, &(AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO])); 

#ifdef WIN32
    hWIN32_DEV=WIN32_DEV_RegAudioDevice();
    hMTVVB=hWIN32_DEV;
    hARMVB =hWIN32_DEV;
    hLAYER1 =hWIN32_DEV;
    hLOOPBACKDEV = hWIN32_DEV;
    hRECORDVB =hWIN32_DEV;
    hARMVBRECORD =hWIN32_DEV;
    hMP4RECORDVB =hWIN32_DEV;
    hDSPDATADEVICE =hWIN32_DEV;
    hPCMDev =NULL;
    hLINEINDev = hWIN32_DEV;
    hLINEINADDev = hWIN32_DEV;
#ifdef BLUETOOTH_SUPPORT
    hL1Rec =hWIN32_DEV;
#ifdef BT_A2DP_SUPPORT
    hA2DPDev =hWIN32_DEV;
#endif    
#endif



#ifdef BACKGROUND_MUSIC_ENABLE
    hBackgroundMusicDev =hWIN32_DEV;
#endif

#else

#ifdef MTV_SUPPORT
    hMTVVB = MTVVB_RegAudioDevice();
#endif
    hARMVB = ARMVB_RegAudioDevice();
#ifdef ARMVB_RECORD_SUPPORT
    hARMVBRECORD = ARMVB_Record_RegAudioDevice();
#endif
    hLAYER1 = LAYER1_RegAudioDevice();
    hLOOPBACKDEV = LOOPBACK_RegAudioDevice();
    hRECORDVB = RECORD_RegAudioDevice();
    hMP4RECORDVB = MP4RECORD_RegAudioDevice();
    hDSPDATADEVICE = DSPDATA_RegAudioDevice();

    hPCMDev = PCMDEV_RegAudioDevice();
    hLINEINDev = ARMVB_Linein_RegAudioDevice();
#ifdef ARMVB_RECORD_SUPPORT    
    hLINEINADDev = ARMVB_LineinAd_RegAudioDevice();
#endif    
#ifdef BLUETOOTH_SUPPORT

    hL1Rec = L1REC_RegAudioDevice();
#ifdef BT_A2DP_SUPPORT
    hA2DPDev = A2DP_RegAudioDevice();
#endif /* BT_A2DP_SUPPORT */
#endif /* _BLUETOOTH */

#ifdef BACKGROUND_MUSIC_ENABLE
    hBackgroundMusicDev = BKMUSIC_RegAudioDevice();
#endif

#endif
    hTransSamExp = TransSam_RegExpPlugger();
#ifndef WIN32
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        //this pluuger includes: lcf,agc,dggain,eq
        hAUDPROCExp = AUDPROC_EXP_RegExpPlugger();
        AUDPROC_EXP_GetEqParaFromNv();
    #else
        //this pluuger data process includes: upgrade samplerate,lcf,agc,dggain,eq
        hAUDENHAExp = AUDENHA_EXP_RegExpPlugger();
        AUDENHA_EXP_GetEqParaFromNv();
    #endif
#endif
#ifdef DIGITAL_GAIN_SUPPORT    
    hDigitalGainExp = DG_RegExpPlugger();
#endif
#endif

#ifdef MP3_EQ_SUPPORT
#ifndef EQ_EXE_ON_CHIP
    hEQExp = EQ_EXP_RegExpPlugger();
    hAGCExp = AGC_EXP_RegExpPlugger();
#endif
#else
    hAGCExp = AGC_EXP_RegExpPlugger();
#endif

#ifdef RATE_CONTROL_SUPPORT
    hRateControlExp = RATECONTROL_EXP_RegExpPlugger();
#endif

#ifdef MIDI_SUPPORT

#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hMIDICodec, hTransSamExp);
#endif

#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hMIDICodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hMIDICodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hMIDICodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hMIDICodec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hMIDICodec, hAGCExp);
#endif
#endif

#ifdef AAC_SUPPORT
#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hAACLCCodec, hTransSamExp);
#endif
#ifdef RATE_CONTROL_SUPPORT
    AUDIO_PM_AddCodecExpress (hAACLCCodec, hRateControlExp);
#endif
#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hAACLCCodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hAACLCCodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hAACLCCodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hAACLCCodec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hAACLCCodec, hAGCExp);
#endif
#endif

#ifdef AMR_SUPPORT
#ifdef AMR_DECODE_IN_DSP
#ifdef AMR_EQ_SUPPORT
    AUDIO_PM_AddCodecExpress (hAMRCodec, hEQExp);
#endif
#else

#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hAMRCodec, hTransSamExp);
#endif
#ifdef RATE_CONTROL_SUPPORT
    AUDIO_PM_AddCodecExpress (hAMRCodec, hRateControlExp);
#endif

#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hAMRCodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hAMRCodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hAMRCodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hAMRCodec, hAGCExp);
#endif
#endif

#endif
#endif

#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hCloneCodec, hTransSamExp);
#endif
#ifdef RATE_CONTROL_SUPPORT
    AUDIO_PM_AddCodecExpress (hCloneCodec, hRateControlExp);
#endif
#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hCloneCodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hCloneCodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hCloneCodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hCloneCodec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hCloneCodec, hAGCExp);
#endif

#ifdef MP3_SUPPORT
#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hMP3Codec, hTransSamExp);
#endif
#ifdef RATE_CONTROL_SUPPORT
    AUDIO_PM_AddCodecExpress (hMP3Codec, hRateControlExp);
#endif
#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hMP3Codec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hMP3Codec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hMP3Codec, hEQExp);
    AUDIO_PM_AddCodecExpress (hMP3Codec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hMP3Codec, hAGCExp);
#endif
#endif

#ifndef WIN32
#ifdef WMA_SUPPORT
#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hWMACodec, hTransSamExp);
#endif
#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hWMACodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hWMACodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hWMACodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hWMACodec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hWMACodec, hAGCExp);
#endif
#endif
#endif

#ifdef WAV_SUPPORT

#ifndef WIN32
#ifdef CAT_MVOICE_SUPPORT
	 hCatMvMixExp = CatMv_RegExpPlugger();
	 AUDIO_PM_AddCodecExpress(hWAVCodec, hCatMvMixExp);
#endif 
#endif 

#if defined(TRANS_SAM_SUPPORT)
    AUDIO_PM_AddCodecExpress (hWAVCodec, hTransSamExp);
#endif

    
#ifdef RATE_CONTROL_SUPPORT
    AUDIO_PM_AddCodecExpress (hWAVCodec, hRateControlExp);
#endif
#ifdef MP3_EQ_SUPPORT
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
        AUDIO_PM_AddCodecExpress (hWAVCodec, hAUDPROCExp);
    #else
        AUDIO_PM_AddCodecExpress (hWAVCodec, hAUDENHAExp);
    #endif
#else
    AUDIO_PM_AddCodecExpress (hWAVCodec, hEQExp);
    AUDIO_PM_AddCodecExpress (hWAVCodec, hAGCExp);
#endif
#else
    AUDIO_PM_AddCodecExpress (hWAVCodec, hAGCExp);
#endif



#endif

//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT
{
    MexMdi_InitPlugger();
}
#endif //MET_MEX_SUPPORT

#ifdef DRA_SUPPORT
    CfgAudCodecExp(hDRACodec);
#endif

#ifdef TA8LBR_SUPPORT
    CfgAudCodecExp(hRA8LBRCodec);
#endif

#ifndef WIN32
#ifdef CAT_MVOICE_SUPPORT
   hCatMvSampleCodec=CatMv_RegCodecPlugger(hCollision_CatMic, &(AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO])); 
#endif 

#ifdef MIC_SAMPLE_SUPPORT
   hMicSampleCodec=MicSample_RegCodecPlugger(hCollision_MicSample, &(AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO])); 
#endif 
#endif 
}

#ifndef WIN32

#ifdef MIC_SAMPLE_SUPPORT

#include "dal_audio.h"

#define MIC_DEFAULT_SAMPLE_RATE       (16000) //8K--16K--32K--48K
#define MIC_DEFAULT_SAMPLE_VOLUME     (2)     //1->9


//*********************************************************************************
//  Description:    This function control MicSample.
//  Author:         hai.li
//  Note:           Default mic samplerate is 16k, you can set samplerate
//                  through function AUDIO_SetOutputSampleRate();
//********************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_EnableMicSample( BOOLEAN is_enable )                                
{   
    static uint8 s_micsample_flag=0;
    
    //SCI_TRACE_LOW:"[AUD_EnableMicSample]: is_enable = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1396_112_2_18_1_2_3_18,(uint8*)"d", is_enable);

    if(hMicSample == 0)
    {
        hMicSample = AUDIO_CreateNormalHandle(hMicSampleCodec,SCI_NULL, hARMVBRECORD,SCI_NULL, AudioDummyNotifyCallback);
    }
      
    if(s_micsample_flag==1)
    {
         if(is_enable)
         {
           //SCI_TRACE_LOW:"[AUD_EnableMicSample]: Have open!!"
           SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1407_112_2_18_1_2_3_19,(uint8*)"");
           return AUDIO_ERROR;
         }
    }
    else
    {
         if(!is_enable)
         {
            //SCI_TRACE_LOW:"[AUD_EnableMicSample]: Have close!!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1415_112_2_18_1_2_3_20,(uint8*)"");
            return AUDIO_ERROR;
         }
    }

    if(is_enable)
    {    
        //set mic sample rate
        AUDIO_SetOutputSampleRate (hMicSample, MIC_DEFAULT_SAMPLE_RATE);
        //SCI_TRACE_LOW:"[AUD_EnableMicSample]: mic default sample rate is=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1424_112_2_18_1_2_3_21,(uint8*)"d",MIC_DEFAULT_SAMPLE_RATE);

        //start mic sample
        AUDIO_Play(hMicSample, 0);

        /*
        //set volume meaning set mic PGA, it will change dB value,
        //The get dB value will change with different volume value.
        //The get dB value will more and more big with enhancing volume
.                                                  
        */
        AUDIO_SetVolume(MIC_DEFAULT_SAMPLE_VOLUME);
        //SCI_TRACE_LOW:"[AUD_EnableMicSample]: mic default sample volume is=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1436_112_2_18_1_2_3_22,(uint8*)"d",MIC_DEFAULT_SAMPLE_VOLUME);

        //For get mic dB value from the same PGA value, delay 500ms
        SCI_Sleep(500);       
        s_micsample_flag=1;
    }
    else
    {
        AUDIO_Stop(hMicSample);
        s_micsample_flag=0;
    }
    
    return AUDIO_NO_ERROR;    
}
#endif 
#endif 


#else

LOCAL void AUD_CUS_Init_Modem (void)
{
    CLASS1PRIVATEINFO_T class1_priv_info;
    CLASS3PRIVATEINFO_T class3_priv_info;

    AUDIO_Init();

    //create audio collison object
    hCollision_Voice = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_HIGHEST);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Voice, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Custom = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Custom, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Dtmf = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Dtmf, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_Generic = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);;

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_Generic, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    hCollision_DspCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_DspCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

		hCollision_VOICEPROCESS = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);
    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_VOICEPROCESS, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#ifdef UPLINK_CODEC_SUPPORT
    hCollision_UplinkCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_UplinkCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#endif
#ifdef DOWNLINK_CODEC_SUPPORT
    hCollision_DownlinkCodec = AUDIO_CM_CreateCollisionObject (AUDIO_PRIORITY_MIDDLE);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hCollision_DownlinkCodec, COLLISION_HANDLE, COLLISION_OBJECT_HANDLE))
    {
        //audio collision object handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

#endif

    //create audio collision class1
    hClass1 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS1", ACM_Policy_DifferentLevelCollide, sizeof (CLASS1PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass1, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        //audio collision class handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class1 to collision object
    //DSP voice band, level = 0; ARM voice band, level = 1.
    //    codecs used DSP VB is a level, including AMR, voice,
    //                                 custom tone, dtmf tone, generic tone.
    //    codecs used ARM VB is another level, including MIDI, MP3,
    //                                                  ADPCM, AAC, PCM.
    class1_priv_info.level = 0;
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Voice, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Custom, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Dtmf, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_Generic, &class1_priv_info);
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_DspCodec, &class1_priv_info);

#ifdef UPLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_UplinkCodec, &class1_priv_info);
#endif

#ifdef DOWNLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass1, hCollision_DownlinkCodec, &class1_priv_info);
#endif

    //create audio collision class2
    hClass2 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS2", ACM_Policy_Exclusive, 0);

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass2, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class2 to collision object
    //    this policy uses no private informations.
    //     following codec is exclusive: MIDI, MP3, AAC.
    //      so they must use the same class which adopts this policy.
    AUDIO_CM_RegCollisionClass (hClass2, hCollision_DspCodec, NULL);

    //create audio collision class3
    hClass3 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS3", ACM_Policy_SinglePlay, sizeof (CLASS3PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass3, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class3 to collision object
    //      codec with isSinglePlay equal to 1 includes MIDI, MP3, AAC, vioce,
    //                                   custom tone, dtmf tone, generic tone
    //      codec with isSinglePlay equal to 0 includes ADPCM, PCM.
    class3_priv_info.isSinglePlay = 1;
    AUDIO_CM_RegCollisionClass (hClass3, hCollision_DspCodec, &class3_priv_info);
    
    class3_priv_info.isSinglePlay = 0;
    //AUDIO_CM_RegCollisionClass(hClass3, hCollision_WAV, &class3_priv_info);
    //AUDIO_CM_RegCollisionClass(hClass3, hCollision_PCM, &class3_priv_info);

    hClass4 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS4", ACM_Policy_DifferentLevelCollide, sizeof (CLASS1PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass4, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        //audio collision class handle is invalid
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class4 to collision object
    //use this class to describe the relationship betwwen adpcm_record and other codecs.
    //level = 0: adpcm_record, hCollision_Voice
    //level = 1. hCollision_PCM, hCollision_AACLC, hCollision_MIDI, hCollision_MP3
    class1_priv_info.level = 0;
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_Voice, &class1_priv_info);

#ifdef UPLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_UplinkCodec, &class1_priv_info);
#endif

#ifdef DOWNLINK_CODEC_SUPPORT
    AUDIO_CM_RegCollisionClass (hClass4, hCollision_DownlinkCodec, &class1_priv_info);
#endif

    class1_priv_info.level = 1;

    //create audio collision class3
    hClass5 = AUDIO_CM_CreateCollisionClass ( (uint16 *) "CLASS5", ACM_Policy_SinglePlay, sizeof (CLASS3PRIVATEINFO_T));

    if (AUDIO_NO_ERROR != AUDIO_SMHC_VerifyHandle (hClass5, COLLISION_HANDLE, COLLISION_CLASS_HANDLE))
    {
        SCI_ASSERT (FALSE); /*assert verified*/
    }

    //register class3 to collision object
    //      codec with isSinglePlay equal to 1 includes MIDI, MP3, AAC, vioce,
    //                                   custom tone, dtmf tone, generic tone
    //      codec with isSinglePlay equal to 0 includes ADPCM, PCM.
    class3_priv_info.isSinglePlay = 1;
    AUDIO_CM_RegCollisionClass (hClass5, hCollision_Custom, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass5, hCollision_Dtmf, &class3_priv_info);
    AUDIO_CM_RegCollisionClass (hClass5, hCollision_Generic, &class3_priv_info);
    hVoiceProcessCodec = VoiceProcess_RegCodecPlugger(hCollision_VOICEPROCESS, &(AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO])); 

    hVoiceCodec = VOICE_RegCodecPlugger (hCollision_Voice, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hCustomCodec = CUSTOM_TONE_RegCodecPlugger (hCollision_Custom, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hDtmfCodec = DTMF_TONE_RegCodecPlugger (hCollision_Dtmf, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hGenericCodec = GENERIC_TONE_RegCodecPlugger (hCollision_Generic, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    hDSPCodec =  DSP_CODEC_RegCodecPlugger (hCollision_DspCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
    
#ifndef WIN32
#ifdef UPLINK_CODEC_SUPPORT
    hUplinkCodec = UPLINK_RegCodecPlugger (hCollision_UplinkCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
#ifdef DOWNLINK_CODEC_SUPPORT
    hDownlinkCodec = DOWNLINK_RegCodecPlugger (hCollision_DownlinkCodec, & (AUDIO_Codec_ExtCfgInfo_Table[CODEC_NOCODECTASK_EXTINFO]));
#endif
#endif

#ifdef WIN32
    hWIN32_DEV=WIN32_DEV_RegAudioDevice();
    hMTVVB=hWIN32_DEV;
    hARMVB =hWIN32_DEV;
    hLAYER1 =hWIN32_DEV;
    hLOOPBACKDEV =hWIN32_DEV; 
    hRECORDVB =hWIN32_DEV;
    hARMVBRECORD =hWIN32_DEV;
    hMP4RECORDVB =hWIN32_DEV;
    hDSPDATADEVICE =hWIN32_DEV;
    hPCMDev =NULL;
    hLINEINDev = hWIN32_DEV;
    hLINEINADDev = hWIN32_DEV;
#ifdef BLUETOOTH_SUPPORT
    hL1Rec =hWIN32_DEV;
#ifdef BT_A2DP_SUPPORT
    hA2DPDev =hWIN32_DEV;
#endif    
#endif



#ifdef BACKGROUND_MUSIC_ENABLE
    hBackgroundMusicDev =hWIN32_DEV;
#endif

#else
    hLAYER1 = LAYER1_RegAudioDevice();
    hDSPDATADEVICE = DSPDATA_RegAudioDevice();
    hPCMDev = PCMDEV_RegAudioDevice();
#endif
}
#endif

#if defined(DSP_DEC_SBC_PROCESS)
extern   int32 aud_dsp_init(uint32 priority);
#endif


/*****************************************************************************/
//  Description:    This function init audio interface.
//  Author:         Xueliang.Wang
//  Note:           This function should be called after initializing RTOS.
/*****************************************************************************/
PUBLIC void AUD_Init (void)
{
    //AUDIO_PRINT("AUD_Init: Initialize audio interface.");
    // Initialize custome audio interface.
        // Audio device init
//    AUDDEV_Init ();
	AUDIO_HAL_Init();
#ifndef MODEM_PLATFORM    
    AUD_CUS_Init();
#if defined(DSP_DEC_SBC_PROCESS)
    aud_dsp_init(TASK_AUD_DSP_PRI);
#endif    
#else
    AUD_CUS_Init_Modem();
#endif


}

/*****************************************************************************/
//  Description:    This function is used to set engineering audio parameters.
//  Author:         Xueliang.Wang
//  Note:
/*****************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_SetEngineeringParameter (  // If succeed, return AUDIO_NO_ERROR,
    // else return error value
    AUDIO_DEVICE_MODE_TYPE_E  parameter_type,
    AUDIO_TEST_DATA_T   parameter_data
)
{
    //AUDIO_PRINT:"AUD_SetEngineeringParameter: parameter_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1788_112_2_18_1_2_3_28,(uint8*)"d", parameter_type);

    LAYER1_SetAudioEngineeringParameter (parameter_type, parameter_data);

    return AUDIO_NO_ERROR;
}


/*****************************************************************************/
//  Description:    This function is used to get engineering audio parameters.
//  Author:         Sunsome.Ju
//  Note:
/*****************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_GetEngineeringParameter (  // If succeed, return AUDIO_NO_ERROR,
    // else return error value
    AUDIO_DEVICE_MODE_TYPE_E  parameter_type,
    AUDIO_TEST_DATA_T  *parameter_data
)
{
    //AUDIO_PRINT:"AUD_GetEngineeringParameter: parameter_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1805_112_2_18_1_2_3_29,(uint8*)"d", parameter_type);

    LAYER1_GetAudioEngineeringParameter (parameter_type, parameter_data);

    return AUDIO_NO_ERROR;
}

/*****************************************************************************/
//  Description:    This function switchs layer1 mode to test mode.
//  Author:         fancier.fan
//  Note:
/*****************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_SwitchTestMode ( // If succeed, return AUDIO_NO_ERROR,
    // else return error value
    BOOLEAN is_test_mode
)
{
    //AUDIO_PRINT:"AUD_SwitchTestMode: is_test_mode = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1820_112_2_18_1_2_3_30,(uint8*)"d", is_test_mode);

    LAYER1_SwitchTestMode (is_test_mode);

    return AUDIO_NO_ERROR;
}
/*****************************************************************************/
//  Description:    This function is used to enable/disable volice loopback
//                  test.
//  Author:         shujing.dong
//  Note:
/*****************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_EnableVoiceLoopback (  // If succeed, return ERR_AUD_NONE,
    // else return error value
    HAUDIO hAudio,
    BOOLEAN is_enable,      // 0: disable  1:enable
    uint16  delay_time      // millisecones
)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;    

    //AUDIO_PRINT:"AUD_EnableVoiceLoopback: is_enable = %d, delay_time = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1839_112_2_18_1_2_3_31,(uint8*)"dd", is_enable, delay_time);

    if (is_enable)
    {
        AUDIO_SetUpLinkMute(SCI_FALSE);
		AUD_EnableVoiceProcessConfig(SCI_TRUE, SCI_TRUE, PNULL);
        result = AUDIO_Play (hAudio, 0);

        if (result!=AUDIO_NO_ERROR)
        {
            //SCI_TRACE_LOW:"AUD_EnableVoiceLoopback play failed:%d."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1849_112_2_18_1_2_3_32,(uint8*)"d", result);
            return AUDIO_ERROR;
        }
    }
    else
    {
        result = AUDIO_Stop (hAudio);
		AUD_EnableVoiceProcessConfig(SCI_FALSE, SCI_FALSE, PNULL);
        if (result!=AUDIO_NO_ERROR)
        {
            //SCI_TRACE_LOW:"AUD_EnableVoiceLoopback stop failed:%d."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_1859_112_2_18_1_2_3_33,(uint8*)"d", result);
            return AUDIO_ERROR;
        }
    }
#ifndef NXP_LVVE_SUPPORT
	  LAYER1_EnableVoiceLoopback (is_enable, delay_time);
#else 
	{
		// Loopback_Type
	    // 0: AD->DA loop, 
	    // 1: AD->ul_process->dl_process->DA loop, 
	    // 2: AD->ul_process->encoder->decoder->dl_process->DA loop
	    uint16 usLoopbackType = 1;
	    // Voice_Format
	    // 1: EFS vocoder
		// 2: HR Vocoder
		// 3: AMR Vocoder
		uint16 usVoiceFormat = 0;
		uint16 usReserved = 0;
    	LAYER1_EnableVoiceLoopback (is_enable, usLoopbackType, usVoiceFormat, usReserved);
   	}
#endif
    return AUDIO_NO_ERROR;
}

/*****************************************************************************/
// Description: United operation for POP noise restrain.
// Author: Benjamin
// Note:
/*****************************************************************************/
PUBLIC void AUD_DSPUnitedOperation (void)
{
}

/*****************************************************************************/
//  Description:     Get AGC Support Flag corresponding music type base on NV configuration
//  Author:            binggo
/*****************************************************************************/
#ifndef EQ_EXE_ON_CHIP
PUBLIC BOOLEAN AUD_GetAGC_SupportFlag (AUDIO_AGC_INPUG_GAIN_TYPE_E eType)
{
    AUDIO_DEVICE_MODE_TYPE_E aud_dev_mode=AUDIO_DEVICE_MODE_HANDHOLD;
    int32 result=FALSE;
    AUDIO_AGC_INPUG_GAIN_TYPE_E agc_type=AUDIO_AGC_INPUG_GAIN_MP3;

    for (aud_dev_mode=AUDIO_DEVICE_MODE_HANDHOLD; aud_dev_mode<AUDIO_DEVICE_MODE_MAX; aud_dev_mode++)
    {
        for (agc_type=AUDIO_AGC_INPUG_GAIN_MP3; agc_type<AUDIO_AGC_INPUG_GAIN_MAX; agc_type++)
        {
            result=AUD_IsAGC_Support (aud_dev_mode,agc_type);

            if (result)
            {
                return TRUE;
            }
        }
    }

    return FALSE;

}
#endif

/*****************************************************************************/
//  Description:    This function is used to set Audio PA.
//  Author:         Johnson.sun
//  Note:   provide for external interface, only reserved.
/*****************************************************************************/
PUBLIC void AUD_EnableAmplifier (BOOLEAN bEnable)
{
    ARMVB_EnableAmplifier (bEnable);
}

/*****************************************************************************/
//  Description:    This function is used to set Audio Codec Output Data Type.
/*****************************************************************************/
PUBLIC void AUD_SetOutputDataType (AUDIO_OUTPUT_DATA_TYPE_E eDataType)
{
}



/*****************************************************************************/
//  Description:    This function is used to get Audio Codec Output Data Type.
/*****************************************************************************/
PUBLIC AUDIO_OUTPUT_DATA_TYPE_E AUD_GetOutputDataType (void)
{
    return 0;
}


/*****************************************************************************/
//  Description:    This function is used to get record dev type.
/*****************************************************************************/
PUBLIC AUD_REC_DEV_TYPE_E AUD_GetRecordDevType (HAUDIODEV hDevice)
{
		if ( (hLAYER1 == hDevice) 
    	|| (hDSPDATADEVICE == hDevice) 
    	|| (hRECORDVB == hDevice)
    	|| (hLOOPBACKDEV == hDevice))
    {
        return REC_BY_DSPVB;
    }
    else
    {
        return REC_BY_ARMVB;
    }
}

PUBLIC void AUD_Cfg_PcmTrans_Param (uint16 dest_samplerate)
{
}

PUBLIC uint16 AUD_Get_PcmTrans_Samplerate (void)
{
    return 8000;
}


/*****************************************************************************/
//  Description:    This function config the amr parameters
//  Author:         yujun.ke
//  Note:
/*****************************************************************************/
void AUD_CfgAmrParam (AMR_MODE amr_mode, uint8 channels, uint8 sample_rate)
{
    LAYER1_CfgAmrParam (amr_mode, channels, sample_rate);
}

#ifdef BACKGROUND_MUSIC_ENABLE
/*****************************************************************************/
//  Description:    This function sets volume of background music.
//  Author:         Cherry.Liu
//  Note:
/*****************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_SetBkMusicVolume (
    AUD_BKMUSIC_PARAM_TYPE_E  eParamType,
    uint16  uiUplinkLevel,
    uint16  uiDownlinkLevel
)
{
    uint16   uplink_gain_value ;
    uint16   downlink_gain_value ;

    //SCI_TRACE_LOW:"audio_prod.c,[AUD_SetBkMusicVolume]:param type:0x%08x,uiUplinkLevel:0x%08x,uiDownlinkLevel:0x%08x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2011_112_2_18_1_2_4_35,(uint8*)"ddd",eParamType,uiUplinkLevel,uiDownlinkLevel);

    //check input levels
    if (uiUplinkLevel > AUD_BKMUSIC_HIGHEST_LEVEL)
    {
        uiUplinkLevel = AUD_BKMUSIC_HIGHEST_LEVEL;
    }

    if (uiDownlinkLevel > AUD_BKMUSIC_HIGHEST_LEVEL)
    {
        uiDownlinkLevel = AUD_BKMUSIC_HIGHEST_LEVEL;
    }

    //look levels up in the background music gain table
    uplink_gain_value   =  s_bkmusic_gain_table[uiUplinkLevel];
    downlink_gain_value =  s_bkmusic_gain_table[uiDownlinkLevel];


    //adjust the volume of background music
    BKMUSIC_SetGain ( (uint32) eParamType,uplink_gain_value,downlink_gain_value);

    return AUDIO_NO_ERROR;

}
#endif

PUBLIC void AUD_SetDgGain (
    int32 dac_gain
)
{

#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
    {
        AUD_PROC_EXP_PARA_T  aud_para = {0};

        aud_para.eParaType = AUD_PROC_PARA_AGC;
        aud_para.unAudProcPara.digitalGain = dac_gain;

        AUDIO_ExeExpSetParaOpe (
            hAUDPROCExp, // hEQExp即为eq注册的eq handle.
            PNULL,
            AUD_PROC_EXP_PARA_COUNT,
            &aud_para
        );

        //SCI_TRACE_LOW:"AUD_SetDG_Express hAUDPROCExp: dac_gain = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2057_112_2_18_1_2_4_36,(uint8*)"d",dac_gain);
    }
    #else
    {
        AUD_ENHA_EXP_PARA_T  aud_para = {0};

        aud_para.eParaType = AUD_ENHA_PARA_DIGI_GAIN;
        aud_para.unAudProcPara.digitalGain = dac_gain;

        AUDIO_ExeExpSetParaOpe (
            hAUDENHAExp, // hEQExp即为eq注册的eq handle.
            PNULL,
            AUD_ENHA_EXP_PARA_COUNT,
            &aud_para
        );

        //SCI_TRACE_LOW:"AUD_SetDG_Express hAUDPROCExp: dac_gain = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2073_112_2_18_1_2_4_37,(uint8*)"d",dac_gain);
    }
    #endif
#else
    {
        AGC_Set_DigitalGain (dac_gain);

        //SCI_TRACE_LOW:"AUD_SetDgGain hAGCExp :dac_gain = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2080_112_2_18_1_2_4_38,(uint8*)"d",dac_gain);
    }
#endif

    return ;
}

/*****************************************************************************/
//  Description:    interface for mmi to set eq mode.
//  Author:         Cherry.Liu
//  Note:
//****************************************************************************/
PUBLIC BOOLEAN AUD_SetEqMode (
    int32 eq_mode
)
{
#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS
    {
        AUDIO_RESULT_E result = AUDIO_NO_ERROR;
        AUD_PROC_EXP_PARA_T  mode_para = {0};

        mode_para.eParaType = AUD_PROC_PARA_EQ_MODE;
        mode_para.unAudProcPara.eqMode = eq_mode;

        result = AUDIO_ExeExpSetParaOpe (
            hAUDPROCExp, // hEQExp即为eq注册的eq handle.
            PNULL,
            AUD_PROC_EXP_PARA_COUNT,
            &mode_para
        );

        if (result == AUDIO_NO_ERROR)
        {
            return SCI_TRUE;
        }
        else
        {
            return SCI_FALSE;
        }
    }
    #else
    {
        AUDIO_RESULT_E result = AUDIO_NO_ERROR;
        AUD_ENHA_EXP_PARA_T  mode_para = {0};

        mode_para.eParaType = AUD_ENHA_PARA_EQ_MODE;
        mode_para.unAudProcPara.eqMode = eq_mode;

        result = AUDIO_ExeExpSetParaOpe (
            hAUDENHAExp, 
            PNULL,
            AUD_ENHA_EXP_PARA_COUNT,
            &mode_para
        );

        if (result == AUDIO_NO_ERROR)
        {
            return SCI_TRUE;
        }
        else
        {
            return SCI_FALSE;
        }
    }
    #endif
    
#else
    {
        AUDIO_RESULT_E result = AUDIO_NO_ERROR;
        EQ_EXP_PARA_T  mode_para = {0};

        mode_para.uiSamplerate = 0;
        mode_para.uiModeNo     = eq_mode;

        result = AUDIO_ExeExpSetParaOpe (
            hEQExp, // hEQExp即为eq注册的eq handle.
            PNULL,
            EQ_EXP_PARA_COUNT,
            &mode_para
        );

        if (result == AUDIO_NO_ERROR)
        {
            return SCI_TRUE;
        }
        else
        {
            return SCI_FALSE;
        }
    }
#endif
}


/*****************************************************************************/
//  Description:    interface for mmi to get current eq mode.
//  Author:         Cherry.Liu
//  Note:
//****************************************************************************/
PUBLIC int32 AUD_GetEqMode (void)
{

#ifdef EQ_EXE_ON_CHIP
    #ifndef EQ_HAS_MULTI_BANDS  //proc
    {
		return -1;
    }
    #else    //enha
    {
    	 AUDIO_RESULT_E result = AUDIO_NO_ERROR;
        AUD_ENHA_EXP_EQ_PARA_INFO_T  mode_para = {0};
		
        result = AUDIO_ExeExpGetParaOpe (
            hAUDENHAExp, 
            (uint16 *)("AUDENHA_EXP"),
            10,
            &mode_para
        );
        if (result == AUDIO_NO_ERROR)
        {
            return mode_para.eqMode;
        }
        else
        {
            return -1;
        }
	 }
	 #endif
#else    //soft eq
	{
		return -1;
	}
#endif		

}


/*****************************************************************************/
//  Description: Audio interface to call sbc encoder for dsp codec.
//  Author: TH
//  Note: NONE.
//****************************************************************************/
PUBLIC void AUD_SBC_Encode_For_DSPCodec (
    uint16 source_samplerate,
    uint16 frame_address,
    uint16 frame_size,
    void *callback
)
{
}

LOCAL void AudioDummyNotifyCallback(
                HAUDIO hAudio, 
                uint32 notify_info, 
                uint32 affix_info
                )
{
    //do nothing
}

#ifndef WIN32
void	SBC_Encode_For_DSPCodec(uint16 source_samplerate,uint16 frame_address,uint16 frame_size/*in word*/, void * callback)
{
}
#endif
/*****************************************************************************/
//  Description:    This function is used to set voice codec enable/disable.
//  Author:         Xueliang.Wang
//  Note:
/*****************************************************************************/
#ifdef _PCM_DUMP_VOICE_
void Voice_dump_Start(HAUDIO hAudioHandle)
{
	int result;
	//shujng start
	//if hope to recording the pcm data of positon 0 and 1 to files for debugging, do like:
	VOICE_ENALBE_RECORD_DEBUG_DATA_T tEnableRecordDebugData = {0};
	tEnableRecordDebugData.bEnable = SCI_TRUE;
	tEnableRecordDebugData.uiRecordPos = 0;
	tEnableRecordDebugData.pucDebugFileName = L"/Mount/Mmc/PCM/Pcm_original.pcm";//@shujing: baek needs to modify this part to samsung's file path.

	AUDIO_ExeCodecExtOpe(
		hAudioHandle, 
		ANSI2UINT16("VOICE_EnableRecordDebugData"), 
		VOICE_ENABLE_RECORD_DEBUG_DATA_PARAM_COUNT, 
		(void *)&tEnableRecordDebugData, 
		&result);
	tEnableRecordDebugData.bEnable = SCI_TRUE;
	tEnableRecordDebugData.uiRecordPos = 1;
	tEnableRecordDebugData.pucDebugFileName = L"/Mount/Mmc/PCM/Pcm_changed.pcm";//@shujing: baek needs to modify this part to samsung's file path.

	AUDIO_ExeCodecExtOpe(
		hAudioHandle, 
		ANSI2UINT16("VOICE_EnableRecordDebugData"), 
		VOICE_ENABLE_RECORD_DEBUG_DATA_PARAM_COUNT, 
		(void *)&tEnableRecordDebugData, 
		&result);	
}

void Voice_dump_Stop(HAUDIO hAudioHandle)
{
	int result;
	//shujing start
	//if hope to stop recording the pcm data of positon 0 and 1, do like:

	VOICE_ENALBE_RECORD_DEBUG_DATA_T tEnableRecordDebugData = {0};	
	tEnableRecordDebugData.bEnable = SCI_FALSE;
	tEnableRecordDebugData.uiRecordPos = 0;

	AUDIO_ExeCodecExtOpe(
		hAudioHandle, 
		ANSI2UINT16("VOICE_EnableRecordDebugData"), 
		VOICE_ENABLE_RECORD_DEBUG_DATA_PARAM_COUNT, 
		(void *)&tEnableRecordDebugData, 
		&result);

	tEnableRecordDebugData.bEnable = SCI_FALSE;
	tEnableRecordDebugData.uiRecordPos = 1;

	AUDIO_ExeCodecExtOpe(
		hAudioHandle, 
		ANSI2UINT16("VOICE_EnableRecordDebugData"), 
		VOICE_ENABLE_RECORD_DEBUG_DATA_PARAM_COUNT, 
		(void *)&tEnableRecordDebugData, 
		&result);
}
#endif

//#define OTHER_LOG_DISABLE
#ifdef OTHER_LOG_DISABLE
extern uint32 SCI_TraceLow_Dump(
    const char *x_format, ...);
#define VOICE_DUMP_PCM_TRACE     SCI_TraceLow_Dump
#else  
#define VOICE_DUMP_PCM_TRACE     SCI_TRACE_LOW
#endif

LOCAL BOOLEAN sbVoicePlaying = SCI_FALSE;
#ifdef _PCM_DUMP_VOICE_MODEM_
LOCAL uint32 uiSampleOnce = 160*4;
LOCAL uint32 uiDumpCallbackTimes[VOICE_DUMP_PCM_DATA_POSITION_TOTAL_COUNT] = {0};
LOCAL BOOLEAN sbVoiceProcessPlaying = SCI_FALSE;
//#define FILE_SAVE_COMPARE_ENABLE

#ifdef FILE_SAVE_COMPARE_ENABLE
LOCAL SFS_HANDLE sFileHandle = SCI_NULL;
#endif    
LOCAL  void Voice_Dump_Pcm_Data_Callback(
    uint32 uiPositionIndex,
    int16 *psBuf, 
    uint32 uiLength //its unit is bytes.
    )
{
    uint32 i=0, uiCount = uiSampleOnce/16;  
    
    SCI_PASSERT((uiPositionIndex<VOICE_DUMP_PCM_DATA_POSITION_TOTAL_COUNT),
        ("Voice_Dump_Pcm_Data_Callback:%d", uiPositionIndex)); /*assert verified*/

    SCI_PASSERT((uiLength==(uiSampleOnce<<1)),
            ("Voice_Dump_Pcm_Data_Callback, len: %d, %d", uiLength, uiSampleOnce)); /*assert verified*/   
            
    uiDumpCallbackTimes[uiPositionIndex]++;
#ifdef FILE_SAVE_COMPARE_ENABLE
    if((0==uiPositionIndex) && (SCI_NULL!=sFileHandle))
    {
        uint32 fs_trans = 0; 
        SFS_WriteFile(sFileHandle, psBuf, uiLength, &fs_trans, SCI_NULL);
    }			
#endif    
    //VOICE_DUMP_PCM_TRACE:"Voice_Dump_Pcm_Data_Callback index:%d, %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2302_112_2_18_1_2_4_39,(uint8*)"dd", uiPositionIndex, uiDumpCallbackTimes[uiPositionIndex]);
    
    for(i=0; i<uiCount; i++)
    {
        uint32 uiBase = 16*i;
        //VOICE_DUMP_PCM_TRACE:"Voice_Dump_Pcm_Data_Callback data:%02d,%02d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d,%05d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2325_112_2_18_1_2_4_40,(uint8*)"dddddddddddddddddd",uiPositionIndex,i,psBuf[uiBase+0]&0xffff, psBuf[uiBase+1]&0xffff, psBuf[uiBase+2]&0xffff, psBuf[uiBase+3]&0xffff, psBuf[uiBase+4]&0xffff, psBuf[uiBase+5]&0xffff,psBuf[uiBase+6]&0xffff, psBuf[uiBase+7]&0xffff, psBuf[uiBase+8]&0xffff, psBuf[uiBase+9]&0xffff, psBuf[uiBase+10]&0xffff, psBuf[uiBase+11]&0xffff,psBuf[uiBase+12]&0xffff, psBuf[uiBase+13]&0xffff, psBuf[uiBase+14]&0xffff, psBuf[uiBase+15]&0xffff);

    }
}
#endif    

/*****************************************************************************/
//  Description:    This function is used to set voice codec enable/disable.
//  Author:         Xueliang.Wang
//  Note:
/*****************************************************************************/
PUBLIC AUDIO_RESULT_E AUD_EnableVoiceProcessConfig ( // If succeed, return ERR_AUD_NONE,
    // else return error values
    BOOLEAN is_enable,                   // SCI_TRUE: Enable,   SCI_FALSE: Disable
    BOOLEAN is_loopback,
    void * para_reserve
)
{

	static HAUDIO g_voice_process_handle = 0;
#if 1
{  
    if (0 == g_voice_process_handle)
    {
        g_voice_process_handle = AUDIO_CreateNormalHandle(
                                            hVoiceProcessCodec,
                                            NULL,//NULL,
                                            hLAYER1,//hLAYER1,
                                            NULL,//NULL,
                                            VoiceDummyNotifyCallback
                                            );
        SCI_ASSERT(0 != g_voice_process_handle); /*assert verified*/
    }
    }
#endif

    if (is_enable)
    {
#ifdef _PCM_DUMP_VOICE_MODEM_    
        if(sbVoiceProcessPlaying)
        {
            //VOICE_DUMP_PCM_TRACE:"AUD_EnableVoiceProcessConfig is playing and needs not to play again."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2364_112_2_18_1_2_4_41,(uint8*)"");
            return AUDIO_NO_ERROR;
        }
        sbVoiceProcessPlaying = SCI_TRUE;
#endif        

#if defined(__Diamond_VB__)  || defined(__Diamond_SF__)
	_SamsungDiamond_init(g_voice_process_handle);// jinwon.baek 110302 for voice booster   
#endif	
#ifdef _PCM_DUMP_VOICE_MODEM_
        {   
            AUDIO_RESULT_E result = AUDIO_NO_ERROR;
            VOICE_CODEC_EXT_OPE_CONFIG_DUMP_PCM_DATA_PARAM_T tPara = {0};
            tPara.uiSampleOnce = uiSampleOnce;
            tPara.uiCachFrameCount = 3;
            tPara.pDumpPcmDataCallbackFunc = Voice_Dump_Pcm_Data_Callback; //this is realized by app.
            AUDIO_ExeCodecExtOpe(g_voice_process_handle, ANSI2UINT16("VOICE_DUMP_PCM_DATA_CONFIG"), 0, (void *)&tPara, &result);
#ifdef FILE_SAVE_COMPARE_ENABLE
            sFileHandle = SFS_CreateFile( (const uint16 *)(L"/Mount/Mmc/PCM/TEST0.wav"), SFS_MODE_CREATE_ALWAYS|SFS_MODE_WRITE,NULL,NULL);
            if(SCI_NULL==sFileHandle)
            {
                
//VOICE_DUMP_PCM_TRACE:"AUD_EnableVoiceProcessConfig SFS_CreateFile failed."
SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2386_112_2_18_1_2_4_42,(uint8*)"");
            }
#endif            
            SCI_MEMSET(uiDumpCallbackTimes, 0, sizeof(uint32) *VOICE_DUMP_PCM_DATA_POSITION_TOTAL_COUNT);
        }
#endif
#ifdef _PCM_DUMP_VOICE_
	Voice_dump_Start(g_voice_process_handle);
#endif
		if(is_loopback)
		{
			AUDIO_RESULT_E result = AUDIO_NO_ERROR;
			VOICE_IF_LOOPBACK_T tLoopback = {0};    
			tLoopback.bIsLoopback = SCI_TRUE;
			AUDIO_ExeCodecExtOpe(g_voice_process_handle, ANSI2UINT16("VOICE_IF_LOOPBACK"), 0, (void *)&tLoopback, &result);

		}
		#if 1
	    {
	        AUDIO_RESULT_E aud_result = AUDIO_NO_ERROR;
	        AUDIO_RESULT_E ext_result  = AUDIO_NO_ERROR;
	        aud_result = AUDIO_ExeCodecExtOpe(g_voice_process_handle, ANSI2UINT16("VOICE_PROCESS_START"), 0, NULL, &ext_result);
	        //SCI_TRACE_LOW:"\"aud_result=%d, ext_result=%d"
	        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2408_112_2_18_1_2_5_43,(uint8*)"dd", aud_result, ext_result);
	    }
			#endif 
    }
    else
    {
#ifdef _PCM_DUMP_VOICE_MODEM_	
        int i=0;	
        if(!sbVoiceProcessPlaying)
        {
            //VOICE_DUMP_PCM_TRACE:"AUD_EnableVoiceProcessConfig is stopped and needs not to stop again."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2418_112_2_18_1_2_5_44,(uint8*)"");
            return AUDIO_NO_ERROR;
        }
        sbVoiceProcessPlaying = SCI_FALSE;
#endif	 
		#if 1
	    {
	        AUDIO_RESULT_E aud_result = AUDIO_NO_ERROR;
	        AUDIO_RESULT_E ext_result  = AUDIO_NO_ERROR;
	        aud_result = AUDIO_ExeCodecExtOpe(g_voice_process_handle, ANSI2UINT16("VOICE_PROCESS_STOP"), 0, NULL, &ext_result);
	        //SCI_TRACE_LOW:"\"stop aud_result=%d, ext_result=%d"
	        SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2428_112_2_18_1_2_5_45,(uint8*)"dd", aud_result, ext_result);
	    }
#endif	 
#if defined(__Diamond_VB__)  || defined(__Diamond_SF__)
    	Diamond_Sol_int=0; //voice booster initialization should be done every calling     	
#endif	

#ifdef _PCM_DUMP_VOICE_
	Voice_dump_Stop(g_voice_process_handle);
#endif

#ifdef _PCM_DUMP_VOICE_MODEM_        
        for(i=0; i<VOICE_DUMP_PCM_DATA_POSITION_TOTAL_COUNT;i++)
        {            
            //VOICE_DUMP_PCM_TRACE:"AUD_EnableVoiceProcessConfig stop. data size should save:%d(bytes), index:%d."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2443_112_2_18_1_2_5_46,(uint8*)"dd",(uiDumpCallbackTimes[i]*uiSampleOnce)<<1,i);
        }
#ifdef FILE_SAVE_COMPARE_ENABLE        
        if(SCI_NULL!=sFileHandle)
        {
            SFS_CloseFile(sFileHandle);
        }
#endif        
#endif        
    }
    return AUDIO_NO_ERROR;
}  
PUBLIC AUDIO_RESULT_E AUD_EnableVoiceCodec ( // If succeed, return ERR_AUD_NONE,
    // else return error value
    BOOLEAN is_enable                   // SCI_TRUE: Enable,   SCI_FALSE: Disable
)
{
    static HAUDIO hVoiceRing = NULL;
    //AUDIO_PRINT:"AUD_EnableVoiceCodec: is_enable = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,AUDIO_CONFIG_2461_112_2_18_1_2_5_47,(uint8*)"d", is_enable);

    if(NULL==hVoiceRing)
    {
        hVoiceRing    = AUDIO_CreateNormalHandle(hVoiceCodec,    SCI_NULL, hLAYER1,     SCI_NULL, AudioDummyNotifyCallback);
        SCI_ASSERT(NULL != hVoiceRing);/*assert verified*/  
    }
    if (is_enable)
    {
			AUD_EnableVoiceProcessConfig(SCI_TRUE, SCI_FALSE, PNULL);
        AUDIO_Play (hVoiceRing, 0);
    }
    else
    {
        AUDIO_Stop (hVoiceRing);
        AUD_EnableVoiceProcessConfig(SCI_FALSE, SCI_FALSE, PNULL);
    }

    return AUDIO_NO_ERROR;
}


#if defined(BLUETOOTH_SUPPORT) && defined(BT_A2DP_SUPPORT) 
extern uint16 SBC_getPacketCount(void);
#endif

PUBLIC int32 AUD_A2dp_Is_Busy(void )
{
#if defined(BLUETOOTH_SUPPORT) && defined(BT_A2DP_SUPPORT) 
{
    uint32 is_busy=0;
#ifndef WIN32
    uint32 packet_count=SBC_getPacketCount();
    if(packet_count >=20)
    {
        is_busy=0;
    }
    else
    {
        is_busy=1;
    }
#endif
    return is_busy;
}    
#else
    return 0;

#endif
}

/*****************************************************************************/
//  Description: check if some exp need to work or not when some dev is working.
//               for example, hAUDPROCExp need not to work when output dev is bt.
//               need to work, return true; need not to work, return false.
//  Author: shujing.dong
//  Note: NONE.
//****************************************************************************/
PUBLIC BOOLEAN AUD_ExpWorked_ForSomeDev(HAUDIOEXP hExp, HAUDIODEV hDev)
{
/*
	AUDIO_EXPNOTWORKED_FORTHISDEV_CONFIG AUDIO_ExpNotWorked_ForThisDev_Config[]=
	{
		{hAUDPROCExp, hA2DPDev},
		{hAUDENHAExp, hA2DPDev),
		{hAUDPROCExp, hPCMDev},
		{hAUDPROCExp, hPCMDev},
	};
	HAUDIOEXP hTransSamExp;
HAUDIOEXP hEQExp;
HAUDIOEXP hAGCExp;
HAUDIOEXP hAUDPROCExp = INVALID_HANDLE;
HAUDIOEXP hAUDENHAExp = INVALID_HANDLE;
HAUDIOEXP hDigitalGainExp = INVALID_HANDLE;
#ifdef RATE_CONTROL_SUPPORT
HAUDIOEXP hRateControlExp;
#endif
*/	
	if((hAUDPROCExp==hExp)||(hAUDENHAExp==hExp)) 
	{
		if(
#ifdef BT_A2DP_SUPPORT
			(hA2DPDev==hDev)||
#endif
			(hPCMDev==hDev))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else if(hDigitalGainExp==hExp)
	{
		if(
#ifdef BT_A2DP_SUPPORT
			(hA2DPDev==hDev)||
#endif
			(hPCMDev==hDev))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	
    return TRUE;
}

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
    
#endif  // End of asm_main.c
