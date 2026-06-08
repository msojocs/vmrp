/****************************************************************************
** File Name:      mmisrvaud_drv.c                                                              *
** Author:          Yintang.ren                                                               *
** Date:             27/04/2011                                                              *
** Copyright:      2003 Spreatrum, Incoporated. All Rights Reserved.           *
** Description:    This file is to packet some interface base on the audio APIs of REF.
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE                NAME                 DESCRIPTION                                 *
** 27/04/2011       Yintang.ren         Create
** 
****************************************************************************/
#include "mmi_service_trc.h"
#include "mmisrvmgr.h"
#include "mmisrvaud.h"
#include "mmisrvaud_util.h"
#include "mmisrvaud_drv.h"
#include "mmi_osbridge.h"
#include "sig_code.h"
#include "tasks_id.h"
#include "eq_exp.h"

#include "audio_config.h"
#include "mmibt_export.h"
#include "aud_gen.h"
#include "ratecontrol_exp.h"
#include "generic_tone_adp.h"
#include "custom_tone_adp.h"
#include "dtmf_tone_adp.h"
#include "wav_adp.h"
#include "AdpcmRecord_adp.h"
#include "dal_player.h"
#ifdef ATV_SUPPORT
#include "dal_recorder.h"
#include "sensor_atv.h"
#endif
#ifdef MBBMS_SUPPORT
#include "mbbms_service.h"
#endif
#include "gpio_prod_api.h"
#include "bt_abs.h"

#define MMISRVAUD_IQ_DATA_BUF_SIZE (300<<8)

LOCAL uint32* s_IQ_data_buf = PNULL; 

/*****************************************************************************/
//  Description : Audio notify callback function.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL void AudioDrvCallback(
                HAUDIO hAudio, 
                uint32 notify_info, 
                uint32 affix_info
                )
{
    MmiSignalS *sendSignal = PNULL;
    //MMIAUDIO_CALLBACK_INFO_T callback_info = {0};

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AudioDrvCallback() entry, hAudio=0x%x, notify_info=%d,affix_info=%d, END_IND=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_58_112_2_18_3_23_53_227,(uint8*)"dddd", hAudio, notify_info, affix_info, AUDIO_PLAYEND_IND));

    if(AUDIO_PLAYEND_IND == notify_info)
    {
        MmiCreateSignal(APP_AUDIO_END_IND, sizeof(MmiAudioCallbackMsgS), &sendSignal);
        sendSignal->send = P_APP;
        sendSignal->sig.audio_info.hAudio      = hAudio;
        sendSignal->sig.audio_info.notify_info = notify_info;
        sendSignal->sig.audio_info.affix_info  = affix_info;
        MmiSendSignal(P_APP, sendSignal);
    }
}

/*****************************************************************************/
//  Description : Video drv call back function.
//  Global resource dependence : 
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL void VideoDrvCallback(int32 i_type)
{
    MmiSignalS *sendSignal = PNULL;    

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: VideoDrvCallback entry, i_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_79_112_2_18_3_23_53_228,(uint8*)"d", i_type));
  
    MmiCreateSignal(APP_AUDIO_END_IND, sizeof(MmiAudioCallbackMsgS), &sendSignal);   
    sendSignal->send = P_APP;
    sendSignal->sig.audio_info.hAudio = MMISRVAUD_SearchHandleByType(MMISRVAUD_TYPE_VIDEO);
    sendSignal->sig.audio_info.notify_info = AUDIO_PLAYEND_IND;
    sendSignal->sig.audio_info.affix_info  = (uint32)i_type;
    sendSignal->sig.audio_info.type  = MMISRVAUD_TYPE_VIDEO;
    
    MmiSendSignal(P_APP, sendSignal);
}

/*****************************************************************************/
//  Description : Video drv call back function.
//  Global resource dependence : 
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL void VideoWpDrvCallback(int32 i_type)
{
    MmiSignalS *sendSignal = PNULL;    

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: VideoDrvCallback entry, i_type = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_98_112_2_18_3_23_53_229,(uint8*)"d", i_type));
  
    MmiCreateSignal(APP_AUDIO_END_IND, sizeof(MmiAudioCallbackMsgS), &sendSignal);   
    sendSignal->send = P_APP;
    sendSignal->sig.audio_info.hAudio = MMISRVAUD_SearchHandleByType(MMISRVAUD_TYPE_VIDEO_WP);    
    sendSignal->sig.audio_info.notify_info = AUDIO_PLAYEND_IND;
    sendSignal->sig.audio_info.affix_info  = (uint32)i_type;
    sendSignal->sig.audio_info.type  = MMISRVAUD_TYPE_VIDEO_WP;
    
    MmiSendSignal(P_APP, sendSignal);
}

#ifdef MBBMS_SUPPORT
/*****************************************************************************/
//  Description : Mv drv call back function.
//  Global resource dependence : 
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL void MvDrvCallback (MBBMS_SERVICE_INFO_T *service_info_ptr)
{
    MMISRVAUD_TYPE_U *type_ptr = PNULL;
    static MTVSERVICE_CALLBACK cb = PNULL;
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvDrvCallback entry, service_info_ptr = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_118_112_2_18_3_23_53_230,(uint8*)"d", service_info_ptr));
    type_ptr = MMISRVAUD_SearchTypeInfo(MMISRVAUD_TYPE_MV);
    
    /* Firstly, we use the call back func that founded in the current service list;
        But if dosn't found any service, we use the last callback function.
    */
    if(type_ptr != 0)
    {
        cb = (MTVSERVICE_CALLBACK)type_ptr->mv.cb;
    }
    if( cb != PNULL)
    {
        cb(service_info_ptr);
    }    
}
#endif

/*****************************************************************************/
//  Description : Conver service route to driver value.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL AUDIO_DEVICE_MODE_TYPE_E ConvertRoute(MMISRVAUD_ROUTE_T route)
{
    AUDIO_DEVICE_MODE_TYPE_E mode = AUDIO_DEVICE_MODE_HANDHOLD;
    switch(route)
    {
    case MMISRVAUD_ROUTE_HANDHOLD:
        mode = AUDIO_DEVICE_MODE_HANDHOLD;
        break;
    case MMISRVAUD_ROUTE_SPEAKER:
        mode = AUDIO_DEVICE_MODE_HANDFREE;
        break;
    case MMISRVAUD_ROUTE_EARPHONE:
        mode = AUDIO_DEVICE_MODE_EARPHONE;
        break;
    case MMISRVAUD_ROUTE_EARFREE:
        mode = AUDIO_DEVICE_MODE_EARFREE;
        break;
    case MMISRVAUD_ROUTE_TVOUT:
        mode = AUDIO_DEVICE_MODE_TVOUT;
        break;
    case MMISRVAUD_ROUTE_BLUETOOTH:
        mode = AUDIO_DEVICE_MODE_BLUEPHONE;
        break;
    default:
        break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: ConvertRoute(), route to device mode, route=%d, mode=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_167_112_2_18_3_23_53_231,(uint8*)"dd", route, mode));
    
    return mode;
}

/*****************************************************************************/
//  Description : Conver service eq mode to driver value.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL EQ_EXP_MODE_E ConvertEQType(MMISRVAUD_EQ_MODE_E eq_mode)
{
    EQ_EXP_MODE_E ref_eq_mode = EQ_EXP_MODE_PASS;
    switch(eq_mode)
    {
    case MMISRVAUD_EQ_REGULAR:
        ref_eq_mode = EQ_EXP_MODE_PASS;
        break;
    case MMISRVAUD_EQ_CLASSIC:
        ref_eq_mode = EQ_EXP_MODE_CLASSIC;
        break;
    case MMISRVAUD_EQ_ODEUM:
        ref_eq_mode = EQ_EXP_MODE_POP;
        break;
    case MMISRVAUD_EQ_JAZZ:
        ref_eq_mode = EQ_EXP_MODE_VOCAL;
        break;
    case MMISRVAUD_EQ_ROCK:
        ref_eq_mode = EQ_EXP_MODE_POWER;
        break;
    case MMISRVAUD_EQ_SOFT_ROCK:
        ref_eq_mode = EQ_EXP_MODE_ROCK;
        break;
    default:
        break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: ConvertEQType() eq to ref eq, eq_mode=%d, ref_eq_mode=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_203_112_2_18_3_23_53_232,(uint8*)"dd", eq_mode, ref_eq_mode));
    
    return ref_eq_mode;   
}

/*****************************************************************************/
//  Description : Conver driver eq mode to service value.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL MMISRVAUD_EQ_MODE_E ConvertToMMIEQType(EQ_EXP_MODE_E eq)
{
    MMISRVAUD_EQ_MODE_E eq_mode = MMISRVAUD_EQ_REGULAR;
    switch(eq)
    {
    case EQ_EXP_MODE_PASS:
        eq_mode = MMISRVAUD_EQ_REGULAR;
        break;
    case EQ_EXP_MODE_CLASSIC:
        eq_mode = MMISRVAUD_EQ_CLASSIC;
        break;
    case EQ_EXP_MODE_POP:
        eq_mode = MMISRVAUD_EQ_ODEUM;
        break;
    case EQ_EXP_MODE_VOCAL:
        eq_mode = MMISRVAUD_EQ_JAZZ;
        break;
    case EQ_EXP_MODE_POWER:
        eq_mode = MMISRVAUD_EQ_ROCK;
        break;
    case EQ_EXP_MODE_ROCK:
        eq_mode = MMISRVAUD_EQ_SOFT_ROCK;
        break;
    default:
        break;
    }
    
    return eq_mode;   
}
/*****************************************************************************/
//  Description : Convert device mode to audio service route.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL MMISRVAUD_ROUTE_T ConvertDevMode(AUDIO_DEVICE_MODE_TYPE_E mode)
{
    MMISRVAUD_ROUTE_T route = MMISRVAUD_ROUTE_NONE;
    switch(mode)
    {
    case AUDIO_DEVICE_MODE_HANDHOLD:
        route = AUDIO_DEVICE_MODE_HANDHOLD;
        break;
    case AUDIO_DEVICE_MODE_HANDFREE:
        route = MMISRVAUD_ROUTE_SPEAKER;
        break;
    case AUDIO_DEVICE_MODE_EARPHONE:
        route = MMISRVAUD_ROUTE_EARPHONE;
        break;
    case AUDIO_DEVICE_MODE_EARFREE:
        route = MMISRVAUD_ROUTE_SPEAKER|MMISRVAUD_ROUTE_EARPHONE;
        break;
    case AUDIO_DEVICE_MODE_TVOUT:
        route = MMISRVAUD_ROUTE_TVOUT;
        break;
    default:
        break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: ConvertDevMode() exit, mode=%d, route=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_236_112_2_18_3_23_53_233,(uint8*)"dd", mode, route));

    return route;
}

/*****************************************************************************/
//  Description : Convert audio service play rate to DRV rate.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL RATECONTROL_EXP_RATE_E ConverPlayRate(MMISRVAUD_PLAY_RATE_E rate)
{
    RATECONTROL_EXP_RATE_E audio_rate = RATECONTROL_EXP_RATE_MAX;
    switch(rate)
    {
    case MMISRVAUD_PLAY_RATE_SLOW_2:
        audio_rate = RATECONTROL_EXP_RATE_SLOWEST;
        break;
    case MMISRVAUD_PLAY_RATE_SLOW_1:
        audio_rate = RATECONTROL_EXP_RATE_SLOWER;
        break;
    case MMISRVAUD_PLAY_RATE_NORMAL:
        audio_rate = RATECONTROL_EXP_RATE_NORMAL;
        break;
    case MMISRVAUD_PLAY_RATE_FAST_1:
        audio_rate = RATECONTROL_EXP_RATE_FASTER;
        break;
    case MMISRVAUD_PLAY_RATE_FAST_2:
        audio_rate = RATECONTROL_EXP_RATE_FASTEST;
        break;
    default:
        break;
    }
    return audio_rate;
}

/*****************************************************************************/
//  Description : get ring audio codec
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL HAUDIOCODEC GetRingCodec(MMISRVAUD_RING_FMT_E ring_type)
{
    HAUDIOCODEC hAudioCodec = 0;

    switch(ring_type)
    {
        case MMISRVAUD_RING_FMT_MIDI:
            hAudioCodec = hMIDICodec;
            break;

        case MMISRVAUD_RING_FMT_SMAF:
            hAudioCodec = hMIDICodec;
            break;

        case MMISRVAUD_RING_FMT_MP3:
			
#if defined(PLATFORM_SC6600L) || defined(AUDIO_SC6800H)
            hAudioCodec = hMP3DSPCodec;
#else
		hAudioCodec = hMP3Codec;
#endif
            break;

        case MMISRVAUD_RING_FMT_WMA:
#ifdef WMA_SUPPORT
            hAudioCodec = hWMACodec;
#endif
            break;

        case MMISRVAUD_RING_FMT_AMR:
            hAudioCodec = hAMRCodec;
            break;
            
        case MMISRVAUD_RING_FMT_WAVE:
            hAudioCodec = hWAVCodec;
            break;
            
        case MMISRVAUD_RING_FMT_AAC:
        case MMISRVAUD_RING_FMT_M4A:
            hAudioCodec = hAACLCCodec;
            break;
            
        default:
            hAudioCodec = hMIDICodec;
            break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: GetRingCodec() exit, ring_type=%d, codec=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_322_112_2_18_3_23_53_234,(uint8*)"dd", ring_type, hAudioCodec));

    return hAudioCodec;
}


/*****************************************************************************/
//  Description : get tone audio codec
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL HAUDIOCODEC GetToneCodec(MMISRVAUD_TONE_TYPE_E tone_type)
{
    HAUDIOCODEC hAudioCodec = 0;
    
    switch(tone_type)
    {
    case MMISRVAUD_GENERIC_TONE:
        hAudioCodec = hGenericCodec;
        break;

    case MMISRVAUD_CUSTOM_TONE:
        hAudioCodec = hCustomCodec;
        break;

    case MMISRVAUD_DTMF_TONE:
        hAudioCodec = hDtmfCodec;
        break;
    default:
        hAudioCodec = hGenericCodec;
        break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: GetToneCodec() exit, tone_type=%d, hAudioCodec=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_354_112_2_18_3_23_53_235,(uint8*)"dd", tone_type, hAudioCodec));
    
    return hAudioCodec;
}

/*****************************************************************************/
//  Description : get audio device
//  Global resource dependence : none
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL HAUDIODEV GetAudioDevice(BOOLEAN is_a2dp, MMISRVAUD_RING_FMT_E ring_fmt)
{
    HAUDIODEV hDevice = 0;

    switch(ring_fmt)
    {
        case MMISRVAUD_RING_FMT_MP3:
        case MMISRVAUD_RING_FMT_AAC:
        case MMISRVAUD_RING_FMT_M4A:
        case MMISRVAUD_RING_FMT_WMA:
        case MMISRVAUD_RING_FMT_MIDI:
        case MMISRVAUD_RING_FMT_WAVE:
#ifdef AMR_A2DP_SUPPORT
        case MMISRVAUD_RING_FMT_AMR:
#endif            
            #ifdef BLUETOOTH_SUPPORT
            if (is_a2dp)
            {
                if(MMIBT_HFG_HEADSET == MMIAPIBT_GetActiveHeadsetType())
                {
                    //HFG device play
                    hDevice = hPCMDev; 
                }   
                #ifdef BT_A2DP_SUPPORT
                else
                {
                    //A2DP Device play
                    hDevice = hA2DPDev; 
                }
                #endif
            }
            else
            #endif
            {
                hDevice = hARMVB;
            }
            break;
        default:
            hDevice = hLAYER1;
            break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: GetAudioDevice() exit, is_a2dp=%d, ring_fmt=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_405_112_2_18_3_23_53_236,(uint8*)"dd", is_a2dp, ring_fmt));

    return hDevice;
}

/*****************************************************************************/
//  Description : get audio type
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL MMISRVAUD_RING_FMT_E  GetAudioType(HAUDIOCODEC audio_codec)
{
    MMISRVAUD_RING_FMT_E fmt = MMISRVAUD_RING_FMT_NONE;
    if(hMIDICodec == audio_codec)
    {
        fmt = MMISRVAUD_RING_FMT_MIDI;//or MMIAUDIO_SMAF
    }
    
    else if(hMP3Codec == audio_codec)
    {
        fmt = MMISRVAUD_RING_FMT_MP3;
    }

#ifdef WMA_SUPPORT
    else if (hWMACodec == audio_codec)
    {
        fmt = MMISRVAUD_RING_FMT_WMA;
    }
#endif    

    else if(hAMRCodec == audio_codec)
    {
        fmt = MMISRVAUD_RING_FMT_AMR;
    }
/*    
    else if(hGenericCodec == audio_codec)
    {
        return MMIAUDIO_GENERIC_TONE;
    }
    
    else if(hCustomCodec == audio_codec)
    {
        return MMIAUDIO_CUSTOM_TONE;
    }
    
    else if(hDtmfCodec == audio_codec)
    {
        return MMIAUDIO_DTMF_TONE;
    }
*/
    else if(hWAVCodec == audio_codec)
    {
        fmt = MMISRVAUD_RING_FMT_WAVE;
    }
    
    else if(hAACCodec == audio_codec)
    {
        fmt = MMISRVAUD_RING_FMT_AAC;
    }

    else
    {
        fmt = MMISRVAUD_RING_FMT_MAX;
    }
    
    return fmt;
}

/*****************************************************************************/
//  Description : get audio format by audio type.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL MMISRVAUD_RING_FMT_E  GetAudioFmt(MMISRVAUD_TYPE_U *type_ptr)
{
    MMISRVAUD_RING_FMT_E fmt = MMISRVAUD_RING_FMT_NONE;

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: GetAudioFmt() entry, type_ptr=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_479_112_2_18_3_23_53_237,(uint8*)"d", type_ptr));
    
    if(type_ptr != PNULL)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_RING_FILE:
            fmt = type_ptr->ring_file.fmt;
            break;
        case MMISRVAUD_TYPE_RING_BUF:
            fmt = type_ptr->ring_buf.fmt;
            break;
        case MMISRVAUD_TYPE_STREAMING:
            fmt = type_ptr->streaming.fmt;
            break;
        case MMISRVAUD_TYPE_TONE_DTMF:
            fmt = MMISRVAUD_RING_FMT_DTMF_TONE;
            break;
        case MMISRVAUD_TYPE_TONE_GENERIC:
            fmt = MMISRVAUD_RING_FMT_GENERIC_TONE;
            break;
        case MMISRVAUD_TYPE_TONE_CUSTOM:
            fmt = MMISRVAUD_RING_FMT_CUSTOM_TONE;;
            break;        
        case MMISRVAUD_TYPE_RECORD_FILE:            
        case MMISRVAUD_TYPE_VOICE:
        case MMISRVAUD_TYPE_VOICE_LOOPBACK:
        case MMISRVAUD_TYPE_FM:
        case MMISRVAUD_TYPE_RECORD_BUF:
        case MMISRVAUD_TYPE_IQ:
        case MMISRVAUD_TYPE_ASP_FILE:
        case MMISRVAUD_TYPE_ASP_BUF:        
        case MMISRVAUD_TYPE_VIBERATE:
        case MMISRVAUD_TYPE_VIRTUAL:
        default:
            break;
        }
    }
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: GetAudioFmt() exit, fmt=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_518_112_2_18_3_23_54_238,(uint8*)"d", fmt));
    
    return fmt;
}

/*****************************************************************************/
//  Description : create audio buffer handle
//  Global resource dependence : none
//  Author: 
//  Note: 
//              SUCCESS - return the audio handle created
//              FAILURE - return SCI_NULL
/*****************************************************************************/
LOCAL HAUDIO CreateAudioBufferHandle(
                    BOOLEAN is_a2dp,
                    MMISRVAUD_RING_FMT_E ring_fmt,    // the codec handle which will be used.
                    const uint8* pucData,   // the music data pointer
                    uint32 uiDataLength,    // the music data length
                    uint32* puitrackbuffer,
                    uint32 uiTrackBufferSize
                    )
{
    HAUDIOCODEC hAudioCodec = 0;
    uint16* pusCodecName = PNULL;
    HAUDIODEV hDevice = 0;
    uint16* pusDevName = PNULL;
    HAUDIO audiohandle = INVALID_HANDLE;
    HAUDIOCODEC real_audio_codec = 0;
    AUDIO_RESULT_E result = AUDIO_ERROR;

        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAudioBufferHandle() entry, is_a2dp=%d, ring_fmt=%d, pucData=0x%x, uiDataLenght=%d,buff=0x%x, size=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_547_112_2_18_3_23_54_239,(uint8*)"dddddd",is_a2dp, ring_fmt, pucData, uiDataLength, puitrackbuffer, uiTrackBufferSize));
    
    hAudioCodec = GetRingCodec(ring_fmt);
    hDevice = GetAudioDevice(is_a2dp,ring_fmt);

    audiohandle = AUDIO_CreateBufferHandle(
                        hAudioCodec,    // the codec handle which will be used.
                        pusCodecName,     // the codec name which will be used.
                        hDevice, 
                        pusDevName,     
                        pucData,   // the music data pointer
                        uiDataLength,    // the music data length
                        puitrackbuffer,     // decode data output buffer.
                        uiTrackBufferSize,   // the track buffer size.
                        AudioDrvCallback  // audio notify callback funtion
                        );

    if(INVALID_HANDLE != audiohandle)
    {
        real_audio_codec = AUDIO_GetCodecHandle(audiohandle);
        if(real_audio_codec != hAudioCodec)
        {
            hDevice = GetAudioDevice(is_a2dp,GetAudioType(real_audio_codec));
            result = AUDIO_SwitchDevice(audiohandle, hDevice, pusDevName);
        }
    }
        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAudioBufferHandle() exit,"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_576_112_2_18_3_23_54_240,(uint8*)"ddddd",audiohandle, hAudioCodec, real_audio_codec, hDevice, result));

    return audiohandle;
}

/*****************************************************************************/
//  Description : Create a file audio  handle.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL HAUDIO CreateAudioFileHandle(
                    BOOLEAN is_a2dp,
                    MMISRVAUD_RING_FMT_E ring_fmt,
                    const wchar *name_ptr,
                    uint32* puitrackbuffer,
                    uint32 uiTrackBufferSize
                    )
{
    HAUDIO audiohandle = INVALID_HANDLE;
    HAUDIOCODEC hAudioCodec = 0;
    uint16* pusCodecName = PNULL;
    HAUDIODEV hDevice = 0;
    uint16* pusDevName = PNULL;
    HAUDIOCODEC real_audio_codec = 0;
    AUDIO_RESULT_E result = AUDIO_ERROR;

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAudioFileHandle() entry"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_601_112_2_18_3_23_54_241,(uint8*)""));
    
    hAudioCodec = GetRingCodec(ring_fmt);
    hDevice = GetAudioDevice(is_a2dp, ring_fmt);

    audiohandle = AUDIO_CreateFileHandle(
                        hAudioCodec,    // the codec handle which will be used.
                        pusCodecName,     // the codec name which will be used.
                        hDevice, 
                        pusDevName,
                        (const int16 *)name_ptr,
                        puitrackbuffer,     // decode data output buffer.
                        uiTrackBufferSize,    // the track buffer size.
                        AudioDrvCallback
                        );

    if(INVALID_HANDLE != audiohandle)
    {
        real_audio_codec = AUDIO_GetCodecHandle(audiohandle);
        if(real_audio_codec != hAudioCodec)
        {
            hDevice = GetAudioDevice(is_a2dp, GetAudioType(real_audio_codec));
            result = AUDIO_SwitchDevice(audiohandle, hDevice, pusDevName);
        }
    }

        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAudioFileHandle() exit,"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_630_112_2_18_3_23_54_242,(uint8*)"ddddd",audiohandle, hAudioCodec, real_audio_codec, hDevice, result));

    return audiohandle;
}

/*****************************************************************************/
//  Description : Create a streaming audio  handle.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL DRVAUD_HANDLE CreateAudioStreamingHandle(
                    BOOLEAN is_a2dp,
                    MMISRVAUD_RING_FMT_E ring_fmt,
                    const uint8 *pucData,
                    uint32  uiDataLength,
                    uint32  *puitrackbuffer,
                    uint32  uiTrackBufferSize,
                    AUDIO_GETSRCDATA_CALLBACK_PFUNC AudioGetSrcDataCallback
                    )
{
    HAUDIOCODEC     hAudioCodec     = 0;
    HAUDIODEV       hDevice         = 0;
    HAUDIO          audiohandle     = INVALID_HANDLE;
    uint16          *pusCodecName   = PNULL;
    uint16          *pusDevName     = PNULL;
    HAUDIOCODEC real_audio_codec = 0;
    AUDIO_RESULT_E result = AUDIO_ERROR;

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAudioStreamingHandle() entry"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_657_112_2_18_3_23_54_243,(uint8*)""));
    
    hAudioCodec = GetRingCodec(ring_fmt);
    hDevice = GetAudioDevice(is_a2dp,ring_fmt);

    audiohandle =  AUDIO_CreateStreamBufferHandle(
                        hAudioCodec,
                        pusCodecName,
                        hDevice, 
                        pusDevName,
                        pucData,
                        uiDataLength,
                        puitrackbuffer,
                        uiTrackBufferSize,
                        AudioDrvCallback,
                        AudioGetSrcDataCallback
                        );

    if(INVALID_HANDLE != audiohandle)
    {
        real_audio_codec = AUDIO_GetCodecHandle(audiohandle);
        if(real_audio_codec != hAudioCodec)
        {
            hDevice = GetAudioDevice(is_a2dp,GetAudioType(real_audio_codec));
            result = AUDIO_SwitchDevice(audiohandle, hDevice, pusDevName);
        }
    }
    
        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAudioStreamingHandle() exit,"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_688_112_2_18_3_23_54_244,(uint8*)"ddddd",audiohandle, hAudioCodec, real_audio_codec, hDevice, result));

    return audiohandle;
}

/*****************************************************************************/
//  Description : Create a IQ type handle.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateIQHandle(
                        const wchar *name_ptr, 
                        void *record_param
                        )
{
    HAUDIOCODEC hAudioCodec = 0;
    const uint16* pusCodecName = PNULL;
    HAUDIODEV hDevice = 0;
    const uint16* pusDevName = PNULL;
    uint32 result = 0;
    DRVAUD_HANDLE handle = 0;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateIQHandle() entry, name_ptr=0x%x,record_param=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_709_112_2_18_3_23_54_245,(uint8*)"dd", name_ptr, record_param));

    //SCI_PASSERT((name_ptr != PNULL && record_param != PNULL),("[MMISRV]aud drv: IQ param error!!"));
    if(PNULL == name_ptr || PNULL == record_param)
    {
        return 0;
    }
    
    s_IQ_data_buf = SCI_ALLOC_APP( MMISRVAUD_IQ_DATA_BUF_SIZE << 2);
    //SCI_PASSERT(PNULL != s_IQ_data_buf,("[MMISRV]aud drv: malloc IQ buffer failed!!"));
    if(PNULL == s_IQ_data_buf)
    {
        return 0;
    }

    hAudioCodec = hRecordCodec;
    hDevice = hDSPDATADEVICE;
    handle = AUDIO_CreateRecordFileHandle(
                                hAudioCodec,
                                pusCodecName,
                                hDevice,
                                pusDevName,
                                (const int16 *)name_ptr,
                                s_IQ_data_buf,
                                MMISRVAUD_IQ_DATA_BUF_SIZE,
                                PNULL
                                );

    if (handle != 0)
    {
        AUDIO_ExeCodecExtOpe(
                handle,
                ANSI2UINT16("SET_RECORDPARAM"),//扩展操作的标志 
                RECORD_PARAM_SET_COUNT,
                record_param,
                &result
                );
    }
    else
    {
        SCI_FREE(s_IQ_data_buf);
        s_IQ_data_buf = PNULL;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateIQHandle() exit, handle=0x%x,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_744_112_2_18_3_23_54_246,(uint8*)"dd", handle, result));

    return handle;
}

/*****************************************************************************/
//  Description : Create record file handle.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL DRVAUD_HANDLE CreateRecordFileHandle(
                    MMIFILE_HANDLE file_handle,
                    wchar *name,
                    MMISRVAUD_RECORD_SOURCE_E src,
                    MMISRVAUD_RECORD_FMT_E fmt,
                    MMISRVAUD_RECORD_FRAME_LEN_E frame_len,
                    uint32* puitrackbuffer,
                    uint32 uiTrackBufferSize
                    )
{    
    DRVAUD_HANDLE handle = 0;
    HAUDIOCODEC hAudioCodec = 0;
    const uint16* pusCodecName = PNULL;
    HAUDIODEV hDevice = 0;
    const uint16* pusDevName = PNULL;
    const uint32 record_frame_length[MMISRVAUD_RECORD_FRAME_LEN_MAX] = {32,27,21,20,18,16,14,13}; 

    //如果运行SKY的应用，采样率为8K
#ifdef MRAPP_SUPPORT
    RECORD_PARAM_T record_param = {AUD_REC_PCM, RECORD_OUTPUT_SAVE_TO_FILE, 0, 160, 0,0,0,8000,0};
#else
	RECORD_PARAM_T record_param = {AUD_REC_PCM, RECORD_OUTPUT_SAVE_TO_FILE, 0, 160, 0,0,0,0,0};
#endif
    RECORD_IF_FROM_NET_T record_from_net = {0};
    uint32 param_result = 0;
    BOOLEAN is_support = TRUE;
    
    switch(fmt)
    {
    case MMISRVAUD_RECORD_FMT_AMR:
        if(Record_QueryEncoderCapability(AUD_REC_AMR_SW))
        {
            record_param.format = AUD_REC_AMR_SW;
        }
        else if(Record_QueryEncoderCapability(AUD_REC_AMR))
        {
            record_param.format = AUD_REC_AMR;            
        }
        else
        {
            is_support = FALSE;
        }
        record_param.frame_length = record_frame_length[frame_len];
        break;
    case MMISRVAUD_RECORD_FMT_PCM:
        if(Record_QueryEncoderCapability(AUD_REC_PCM))
        {
            record_param.format = AUD_REC_PCM;
        }
        else
        {
            is_support = FALSE;
        }
        break;
    case MMISRVAUD_RECORD_FMT_ADPCM:
        if(Record_QueryEncoderCapability(AUD_REC_ADPCM))
        {
            record_param.format = AUD_REC_ADPCM;
        }
        else
        {
            is_support = FALSE;
        }
        break;
    default:
        is_support = FALSE;
        break;
    }

    _DBG(MMISRV_TRACE_LOW("[MMISRV]aud drv: CreateRecordFileHandle() entry, src=%d, fmt=%d,frame_len=%d, file_handle=0x%x,is_support=%d",
        src, fmt, frame_len, file_handle, is_support));

    if(!is_support)
    {
        return 0;
    }
    switch(src)
    {
    case MMISRVAUD_RECORD_SOURCE_NORMAL:
        hDevice = hRECORDVB;
        hAudioCodec = hRecordCodec;
        record_from_net.b_if_from_net = FALSE;
        break;
        
    case MMISRVAUD_RECORD_SOURCE_CALL:
        hDevice = hLAYER1;
        hAudioCodec = hRecordCodec;
        record_from_net.b_if_from_net = TRUE;
        break;
        
    case MMISRVAUD_RECORD_SOURCE_FM:
        hDevice = hLINEINADDev;
        hAudioCodec = hRecordCodec;
        record_from_net.b_if_from_net = FALSE;
        break;

    default:
        break;
    }

    if(SFS_INVALID_HANDLE != file_handle)
    {
        handle = AUDIO_CreateRecordFileHandle_FileCreated(
                                hAudioCodec,
                                pusCodecName,
                                hDevice,
                                pusDevName,
                                (const int16 *)name,
                                file_handle,
                                puitrackbuffer,
                                uiTrackBufferSize,
                                AudioDrvCallback
                                );
    }
    else
    {
        handle = AUDIO_CreateRecordFileHandle(
                                hAudioCodec,
                                pusCodecName,
                                hDevice,
                                pusDevName,
                                (const int16 *)name,
                                puitrackbuffer,
                                uiTrackBufferSize,
                                AudioDrvCallback
                                );
    }
    if(handle > 0)
    {
        if(MMISRVAUD_RECORD_FMT_MP3 == fmt)
        {
            AUDIO_ExeCodecExtOpe(
                handle,
                ANSI2UINT16("SET_RECORDPARAM"),
                RECORD_MP3_PARAM_SET_COUNT,
                &record_param,
                &param_result
                );
        }
        else
        {
            AUDIO_ExeCodecExtOpe(
                handle,
                ANSI2UINT16("SET_RECORDPARAM"),//扩展操作的标志 
                RECORD_PARAM_SET_COUNT,
                &record_param,
                &param_result
                );
        }
        AUDIO_ExeCodecExtOpe(
            handle,
            ANSI2UINT16("SET_RECORDIFNET"),//扩展操作的标志 
            RECORD_SET_IF_FROM_NET_COUNT,
            &record_from_net,
            &param_result
            );
    }
    return handle;
}

/*****************************************************************************/
//  Description : Create Asp file handle.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL HAUDIO CreateAspFileHandle(
                    BOOLEAN is_a2dp,
                    MMISRVAUD_RING_FMT_E ring_fmt,
                    const wchar *name_ptr,
                    uint32* puitrackbuffer,
                    uint32 uiTrackBufferSize
                    )
{
    HAUDIO  handle = INVALID_HANDLE;
    uint32 result = 0;

        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAspFileHandle() entry, is_a2dp=%d, ring_fmt=%d, name_ptr=0x%x, buff=0x%x, size=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_888_112_2_18_3_23_54_248,(uint8*)"ddddd",is_a2dp, ring_fmt, name_ptr, puitrackbuffer, uiTrackBufferSize));
    
    handle = AUDIO_CreateFileHandle (
        hWAVCodec,                              
        SCI_NULL,                               
        hBackgroundMusicDev, 
        SCI_NULL,
        (int16 *)name_ptr,
        puitrackbuffer,     
        uiTrackBufferSize,
        AudioDrvCallback                    
        );
    
    if(handle != INVALID_HANDLE)
    {
        WAV_EXT_OPE_SHAREDMODE_T tSharedMode = {0};
        
        tSharedMode.hAudioHandle = handle;
        tSharedMode.bIsSharedMode = TRUE;
        AUDIO_ExeCodecExtOpe(
            handle,
            ANSI2UINT16("EXT_SHAREDMODE"),
            WAV_EXT_OPE_SHAREDMODE_COUNT,
            &tSharedMode,
            &result
            );        
    }
        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAspFileHandle() exit, handle=0x%x, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_916_112_2_18_3_23_54_249,(uint8*)"dd",handle, result));
    
    return handle;
}

/*****************************************************************************/
//  Description : Create Asp buff handle.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL HAUDIO CreateAspBufferHandle(
                    BOOLEAN is_a2dp,
                    MMISRVAUD_RING_FMT_E ring_fmt,    // the codec handle which will be used.
                    const uint8* pucData,   // the music data pointer
                    uint32 uiDataLength,    // the music data length
                    uint32* puitrackbuffer,
                    uint32 uiTrackBufferSize
                    )
{
    HAUDIO  handle = INVALID_HANDLE;
    uint32 result = 0;

        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAspBufferHandle() entry, is_a2dp=%d, ring_fmt=%d, pucData=0x%x, uiDataLenght=%d,buff=0x%x, size=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_938_112_2_18_3_23_54_250,(uint8*)"dddddd",is_a2dp, ring_fmt, pucData, uiDataLength, puitrackbuffer, uiTrackBufferSize));
    
    handle = AUDIO_CreateBufferHandle (
        hWAVCodec,                              /* the codec handle which will be used */
        SCI_NULL,                               /* the codec name which will be used   */
        hBackgroundMusicDev, 
        SCI_NULL,
        pucData,                    /* data buffer form resource */
        uiDataLength,                    /* data buffer len */
        puitrackbuffer,     /* decode data output buffer */
        uiTrackBufferSize, /* the track buffer size     */
        AudioDrvCallback                     /* audio notify callback funtion */
        );
    
    if(handle != INVALID_HANDLE)
    {
        WAV_EXT_OPE_SHAREDMODE_T tSharedMode = {0};
        
        tSharedMode.hAudioHandle = handle;
        tSharedMode.bIsSharedMode = TRUE;    
        AUDIO_ExeCodecExtOpe(
            handle,
            ANSI2UINT16("EXT_SHAREDMODE"),
            WAV_EXT_OPE_SHAREDMODE_COUNT,
            &tSharedMode,
            &result
            );        
    }
        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: CreateAspBufferHandle() exit, handle=0x%x, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_967_112_2_18_3_23_54_251,(uint8*)"dd",handle, result));
    
    return handle;
}

/*****************************************************************************/
//  Description : Create record buffer handle.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL DRVAUD_HANDLE CreateRecordBuffHandle(
                    uint8 *buf,
                    uint32 buf_len,
                    MMISRVAUD_RECORD_SOURCE_E src,
                    MMISRVAUD_RECORD_FMT_E fmt,
                    MMISRVAUD_RECORD_FRAME_LEN_E frame_len,
                    uint32* puitrackbuffer,
                    uint32 uiTrackBufferSize
                    )
{
    /* To be done later */
    /* 
    ---------------------
    ---------------------    
    ---------------------
    ---------------------
    */
    return 0;
}

/*****************************************************************************/
//  Description : Create an audio handle.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC DRVAUD_HANDLE DRVAUD_CreateHandle(MMISRVAUD_TYPE_U *audio_data, BOOLEAN is_a2dp, uint32 *buff, uint32 buff_size)
{
    HAUDIO handle = 0;
    static uint32 auto_handle = 0xFFFFFFFF;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_CreateHandle() entry, audio_data->type=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1005_112_2_18_3_23_55_252,(uint8*)"d", audio_data->type));

    switch(audio_data->type)
    {
    case MMISRVAUD_TYPE_RING_FILE:
         handle = CreateAudioFileHandle(
                        is_a2dp,
                        audio_data->ring_file.fmt,
                        audio_data->ring_file.name,
                        buff,
                        buff_size
                        );
        break;
    case MMISRVAUD_TYPE_RING_BUF:
         handle = CreateAudioBufferHandle(
                        is_a2dp,
                        audio_data->ring_buf.fmt,
                        audio_data->ring_buf.data,
                        audio_data->ring_buf.data_len,
                        buff,
                        buff_size
                        );
        break;
    case MMISRVAUD_TYPE_STREAMING:
    {
        AUDIO_GETSRCDATA_CALLBACK_PFUNC func = (AUDIO_GETSRCDATA_CALLBACK_PFUNC)(audio_data->streaming.cb); /*lint !e611*/

        handle = CreateAudioStreamingHandle(
                        is_a2dp,
                        audio_data->streaming.fmt,
                        audio_data->streaming.data,
                        audio_data->streaming.data_len,
                        buff,
                        buff_size,
                        func
                        );
        break;
    }
    case MMISRVAUD_TYPE_RECORD_FILE:        
        handle = CreateRecordFileHandle(
                        audio_data->record_file.file_handle,
                        audio_data->record_file.name,
                        audio_data->record_file.source,
                        audio_data->record_file.fmt,
                        audio_data->record_file.frame_len,
                        buff,
                        buff_size
                        );
        break;
    case MMISRVAUD_TYPE_RECORD_BUF:
        handle = CreateRecordBuffHandle(
                        audio_data->record_buf.data,
                        audio_data->record_buf.data_len,
                        audio_data->record_file.source,
                        audio_data->record_file.fmt,
                        audio_data->record_file.frame_len,
                        buff,
                        buff_size            
                        );
        break;
    case MMISRVAUD_TYPE_IQ:
        handle = CreateIQHandle(
                        audio_data->iq_file.name_ptr, 
                        audio_data->iq_file.param
                        );
        break;
    case MMISRVAUD_TYPE_ASP_FILE:
         handle = CreateAspFileHandle(
                        is_a2dp,
                        audio_data->ring_file.fmt,
                        audio_data->ring_file.name,
                        buff,
                        buff_size
                        );        
        break;
    case MMISRVAUD_TYPE_ASP_BUF:
         handle = CreateAspBufferHandle(
                        is_a2dp,
                        audio_data->ring_buf.fmt,
                        audio_data->ring_buf.data,
                        audio_data->ring_buf.data_len,
                        buff,
                        buff_size
                        );
        break;
    case MMISRVAUD_TYPE_TONE_DTMF:
        handle = AUDIO_CreateNormalHandle(hDtmfCodec, PNULL, hLAYER1, PNULL, AudioDrvCallback);
        break;
    case MMISRVAUD_TYPE_TONE_GENERIC:
        handle = AUDIO_CreateNormalHandle(hGenericCodec, PNULL, hLAYER1, PNULL, AudioDrvCallback);
         break;
    case MMISRVAUD_TYPE_TONE_CUSTOM:
        handle = AUDIO_CreateNormalHandle(hCustomCodec, PNULL, hLAYER1, PNULL, AudioDrvCallback);
        break;
    case MMISRVAUD_TYPE_VOICE:
        handle = AUDIO_CreateNormalHandle(hVoiceCodec, PNULL, hLAYER1, PNULL, AudioDrvCallback);
        break;
    case MMISRVAUD_TYPE_VOICE_LOOPBACK:
        handle = AUDIO_CreateNormalHandle(hVoiceCodec, SCI_NULL, hLOOPBACKDEV, SCI_NULL, AudioDrvCallback);
        break;
    case MMISRVAUD_TYPE_FM:    
#ifdef WIN32        
        handle = 0xffff;
#else
        handle = AUDIO_CreateNormalHandle(hNullCodec, PNULL, hLINEINDev, PNULL, AudioDrvCallback);
#endif
        break;
    case MMISRVAUD_TYPE_VIRTUAL:
    case MMISRVAUD_TYPE_VIBERATE:
        /* Any value that is not zero */
        handle = auto_handle--;
        break;
    case MMISRVAUD_TYPE_VIDEO:
        handle = audio_data->video.data;
        DPLAYER_SetEndNotify(handle, VideoDrvCallback);
        break;
    case MMISRVAUD_TYPE_VIDEO_WP:        
        handle = audio_data->video.data;
        DPLAYER_SetEndNotify(handle, VideoWpDrvCallback);
        break;
#ifdef ATV_SUPPORT        
    case MMISRVAUD_TYPE_ATV:
        if(audio_data->atv.param_size <= sizeof(audio_data->atv.param))
        {
            handle = auto_handle--;
            SCI_MEMCPY(&audio_data->atv.param, audio_data->atv.param_ptr, audio_data->atv.param_size);
        }
        else
        {
            _DBG(            //MMISRV_TRACE_LOW:"[MMISRV]aud drv: Error!!! ATV param is exeeded the store memory!!!!!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1134_112_2_18_3_23_55_253,(uint8*)""));
        }
        break;
#endif        
#ifdef MBBMS_SUPPORT
    case MMISRVAUD_TYPE_MV:
        MTVSERVICE_Init((MBBMS_Service_Init_Para_T *)audio_data->mv.init_data, MvDrvCallback);    
        break;
#endif
    default:
        break;
    }

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_CreateHandle() exit, handle=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1147_112_2_18_3_23_55_254,(uint8*)"d", handle));
    
    return (DRVAUD_HANDLE)handle;
}

/*****************************************************************************/
//  Description : Close an audio handle.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_CloseHandle(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    if(handle > 0)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:        
            //DPLAYER_Close(handle);
            result = AUDIO_NO_ERROR;
            break;
        case MMISRVAUD_TYPE_ATV:
            //DRECORDER_Close();
            result = AUDIO_NO_ERROR;
            break;
        case MMISRVAUD_TYPE_MV:
            result = AUDIO_NO_ERROR;            
            break;
        case MMISRVAUD_TYPE_VIRTUAL:
        case MMISRVAUD_TYPE_VIBERATE:
            result = AUDIO_NO_ERROR;            
            break;
        case MMISRVAUD_TYPE_IQ:
            if(s_IQ_data_buf != PNULL)
            {
                SCI_FREE(s_IQ_data_buf);
                s_IQ_data_buf = PNULL;
            }
            result = AUDIO_CloseHandle(handle);
            break;
        default:
            result = AUDIO_CloseHandle(handle);
            break;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_CloseHandle() exit, handle=0x%x, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1192_112_2_18_3_23_55_255,(uint8*)"dd", handle, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Play video.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL AUDIO_RESULT_E VideoPlay(uint32 handle)
{
    DPLAYER_RETURN_E result = DPLAYER_SUCCESS;
    AUDIO_RESULT_E ret = AUDIO_ERROR;
    result = DPLAYER_Play(handle);
    if(DPLAYER_SUCCESS == result)
    {
        ret = AUDIO_NO_ERROR;
    }
    else
    {
        ret = AUDIO_ERROR;
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: VideoPlay() exit, handle=0x%x, result=%d, ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1221_112_2_18_3_23_55_256,(uint8*)"ddd", handle, result, ret));
    
    return ret;
}

/*****************************************************************************/
//  Description : Pause video.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL AUDIO_RESULT_E VideoPause(uint32 handle)
{
    DPLAYER_RETURN_E result = DPLAYER_SUCCESS;
    AUDIO_RESULT_E ret = AUDIO_ERROR;
    result = DPLAYER_PauseEx(handle);
    if(DPLAYER_SUCCESS == result)
    {
        ret = AUDIO_NO_ERROR;
    }
    else
    {
        ret = AUDIO_ERROR;
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: VideoPause() exit, handle=0x%x, result=%d, ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1243_112_2_18_3_23_55_257,(uint8*)"ddd", handle, result, ret));
    
    return ret;
}

/*****************************************************************************/
//  Description : Resume video.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL AUDIO_RESULT_E VideoResume(uint32 handle)
{
    DPLAYER_RETURN_E result = DPLAYER_SUCCESS;
    AUDIO_RESULT_E ret = AUDIO_ERROR;
    result = DPLAYER_ResumeEx(handle);
    if(DPLAYER_SUCCESS == result)
    {
        ret = AUDIO_NO_ERROR;
    }
    else
    {
        ret = AUDIO_ERROR;
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: VideoResume() exit, handle=0x%x, result=%d, ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1265_112_2_18_3_23_55_258,(uint8*)"ddd", handle, result, ret));
    
    return ret;
}

/*****************************************************************************/
//  Description : Stop video.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL AUDIO_RESULT_E VideoStop(uint32 handle)
{
    DPLAYER_RETURN_E result = DPLAYER_SUCCESS;
    AUDIO_RESULT_E ret = AUDIO_ERROR;
    result = DPLAYER_Stop(handle);
    if(DPLAYER_SUCCESS == result)
    {
        ret = AUDIO_NO_ERROR;
    }
    else
    {
        ret = AUDIO_ERROR;
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: VideoStop() exit, handle=0x%x, result=%d, ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1287_112_2_18_3_23_55_259,(uint8*)"ddd", handle, result, ret));
    
    return ret;
}

#ifdef ATV_SUPPORT
/*****************************************************************************/
// 	Description : ParamCtrlCallBack
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL uint32 ParamCtrlCallBack(void *param_ptr, uint32 param_size)
{
    //SCI_TRACE_LOW:"[MMISRV]: ParamCtrlCallBack"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1299_112_2_18_3_23_55_260,(uint8*)"");
    return 0;
}

/*****************************************************************************/
//  Description : Stop ATV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL AUDIO_RESULT_E AtvPlay(MMISRVAUD_ATV_T *atv_ptr)
{
    DRECORDER_RETURN_E ret = DRECORDER_SUCCESS;
    uint32 service_id = 0;
    if(atv_ptr != PNULL)
    {
        //ret = DRECORDER_SelectSrcDevice(DRECORDER_SRC_DEV_ATV);
        //_DBG(MMISRV_TRACE_LOW("[MMISRV]aud drv: AtvPlay(),select dev, ret=%d", ret));

        //ret = DRECORDER_Open();
        //_DBG(MMISRV_TRACE_LOW("[MMISRV]aud drv: AtvPlay(),open, ret=%d", ret));

        ret = DRECORDER_ParamCtrl(DRECORDER_PARAM_CTRL_ATV_REGION, (ATV_REGION_E)atv_ptr->region_id, ParamCtrlCallBack);
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvPlay(),set region, ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1320_112_2_18_3_23_55_261,(uint8*)"d", ret));

        service_id = ((1) << 16) | (atv_ptr->service_id&0xffff);
        ret = DRECORDER_ParamCtrl(DRECORDER_PARAM_CTRL_ATV_CHN, service_id, ParamCtrlCallBack);
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvPlay(),set region, ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1324_112_2_18_3_23_55_262,(uint8*)"d", ret));

        ret = DRECORDER_StartPreview((DRECORDER_PREVIEW_PARAM_T *)&atv_ptr->param);
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvPlay(), preview, ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1327_112_2_18_3_23_55_263,(uint8*)"d", ret));

        service_id = ((2) << 16) | (atv_ptr->service_id&0xffff);
        DRECORDER_ParamCtrl(DRECORDER_PARAM_CTRL_ATV_CHN, service_id, ParamCtrlCallBack);
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvPlay() exit, error!!!param is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1334_112_2_18_3_23_55_264,(uint8*)""));
    }
    if(DRECORDER_SUCCESS == ret)
    {
        return AUDIO_NO_ERROR;
    }
    else
    {
        return AUDIO_ERROR;
    }
}


/*****************************************************************************/
//  Description : Stop ATV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: Stop is called by App, do not close device.
/*****************************************************************************/
LOCAL AUDIO_RESULT_E AtvStop(void)
{
    DRECORDER_RETURN_E ret = DRECORDER_SUCCESS;
    ret = DRECORDER_StopPreview();

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvStop() exit, ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1356_112_2_18_3_23_55_265,(uint8*)"d", ret));
    
    if(DRECORDER_SUCCESS == ret)
    {
        return AUDIO_NO_ERROR;
    }
    else
    {
        return AUDIO_ERROR;
    }
}

/*****************************************************************************/
//  Description : Stop ATV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: Pause is called by audio service, we should close device to free the resource.
/*****************************************************************************/
LOCAL AUDIO_RESULT_E AtvPause(void)
{
    DRECORDER_RETURN_E ret = DRECORDER_SUCCESS;
    ret = DRECORDER_StopPreview();
    //DRECORDER_Close();

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvPause() exit, ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1378_112_2_18_3_23_56_266,(uint8*)"d", ret));
    
    if(DRECORDER_SUCCESS == ret)
    {
        return AUDIO_NO_ERROR;
    }
    else
    {
        return AUDIO_ERROR;
    }
}

/*****************************************************************************/
//  Description : Stop ATV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: Pause is called by audio service, we should close device to free the resource.
/*****************************************************************************/
LOCAL AUDIO_RESULT_E AtvResume(MMISRVAUD_ATV_T *atv_ptr)
{
    DRECORDER_RETURN_E ret = DRECORDER_SUCCESS;

    if(atv_ptr != PNULL)
    {
        //ret = DRECORDER_Open();
        //_DBG(MMISRV_TRACE_LOW("[MMISRV]aud drv: AtvResume(),open, ret=%d", ret));
        
        ret = DRECORDER_StartPreview((DRECORDER_PREVIEW_PARAM_T *)&atv_ptr->param);

        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvResume() exit, ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1405_112_2_18_3_23_56_267,(uint8*)"d", ret));
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: AtvResume() exit, error!!! param is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1409_112_2_18_3_23_56_268,(uint8*)""));
    }
    if(DRECORDER_SUCCESS == ret)
    {
        return AUDIO_NO_ERROR;
    }
    else
    {
        return AUDIO_ERROR;
    }
}

/*****************************************************************************/
//  Description : Play MV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_StartRecordAtv(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr, void *param)
{
    DRECORDER_RETURN_E ret = DRECORDER_SUCCESS;
    BOOLEAN res = FALSE;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StartReocrdATV() entry, param=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1430_112_2_18_3_23_56_269,(uint8*)"d", param));

    if(param != PNULL)
    {
        ret = DRECORDER_StopPreview();
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StartReocrdATV(), stop preview result=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1435_112_2_18_3_23_56_270,(uint8*)"d", ret));

        ret = DRECORDER_StartRecord((DRECORDER_START_RECORD_T *)param);

        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StartReocrdATV(), start record result=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1439_112_2_18_3_23_56_271,(uint8*)"d", ret));

        if(DRECORDER_SUCCESS == ret)
        {
            res = TRUE;
        }
    }
    
    return res;
}

/*****************************************************************************/
//  Description : Play MV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_StopRecordAtv(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    DRECORDER_RETURN_E ret = DRECORDER_SUCCESS;
    BOOLEAN res = FALSE;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StopReocrdATV() entry, type_ptr=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1459_112_2_18_3_23_56_272,(uint8*)"d", type_ptr));

    ret = DRECORDER_StopRecord();
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StopReocrdATV(), stop record result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1462_112_2_18_3_23_56_273,(uint8*)"d", ret));

#if 0
    /* start preview */
    if(type_ptr != PNULL)
    {
        uint32 service_id = 0;
        MMISRVAUD_ATV_T *atv_ptr = type_ptr->atv;

        if(atv_ptr != PNULL)
        {
            ret = DRECORDER_ParamCtrl(DRECORDER_PARAM_CTRL_ATV_REGION, (ATV_REGION_E)atv_ptr->region_id, ParamCtrlCallBack);
            _DBG(            //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StopReocrdATV(),set region, ret=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1474_112_2_18_3_23_56_274,(uint8*)"d", ret));

            service_id = ((1) << 16) | (service_id&0xffff);
            ret = DRECORDER_ParamCtrl(DRECORDER_PARAM_CTRL_ATV_CHN, atv_ptr->service_id, ParamCtrlCallBack);
            _DBG(            //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StopReocrdATV(),set region, ret=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1478_112_2_18_3_23_56_275,(uint8*)"d", ret));

            ret = DRECORDER_StartPreview((DRECORDER_PREVIEW_PARAM_T *)&atv_ptr->param);
            _DBG(            //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StopReocrdATV(), preview, ret=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1481_112_2_18_3_23_56_276,(uint8*)"d", ret));

            service_id = ((2) << 16) | (service_id&0xffff);
            ret = DRECORDER_ParamCtrl(DRECORDER_PARAM_CTRL_ATV_CHN, atv_ptr->service_id, ParamCtrlCallBack);
        }

        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_StopReocrdATV(), start record result=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1487_112_2_18_3_23_56_277,(uint8*)"d", ret));

    }
#endif

    if(DRECORDER_SUCCESS == ret)
    {
        res = TRUE;
    }

    return res;

}
#else
PUBLIC BOOLEAN DRVAUD_StartRecordAtv(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr, void *param)
{
    return FALSE;
}

/*****************************************************************************/
//  Description : Play MV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_StopRecordAtv(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    return FALSE;
}
#endif

#ifdef MBBMS_SUPPORT
/*****************************************************************************/
//  Description : Play MV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL AUDIO_RESULT_E MvPlay(MMISRVAUD_MV_T *mv)
{
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvPlay() entry, mv=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1523_112_2_18_3_23_56_278,(uint8*)"d", mv));

    if(mv != PNULL)
    {
        MTVSERVICE_Play(mv->frequency_num, mv->service_id);
        return AUDIO_NO_ERROR;
    }
    else
    {
        return AUDIO_ERROR;
    }
}

/*****************************************************************************/
//  Description : Stop ATV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL AUDIO_RESULT_E MvStop(void)
{
    if(MTVSERVICE_Stop())
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvStop() exit, OK"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1544_112_2_18_3_23_56_279,(uint8*)""));
    
        return AUDIO_NO_ERROR;
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvStop() exit, error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1550_112_2_18_3_23_56_280,(uint8*)""));
        return AUDIO_ERROR;
    }
}

/*****************************************************************************/
//  Description : Pause MV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL AUDIO_RESULT_E MvPause(void)
{
    if(MTVSERVICE_Stop())
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvStop() exit, OK"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1563_112_2_18_3_23_56_281,(uint8*)""));
    
        return AUDIO_NO_ERROR;
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvStop() exit, error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1569_112_2_18_3_23_56_282,(uint8*)""));
        return AUDIO_ERROR;
    }
}

/*****************************************************************************/
//  Description : Resume MV.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: 
/*****************************************************************************/
LOCAL AUDIO_RESULT_E MvResume(MMISRVAUD_MV_T *mv)
{
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: MvPlay() entry, mv=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1580_112_2_18_3_23_56_283,(uint8*)"d", mv));

    if(mv != PNULL)
    {
        MTVSERVICE_Play(mv->frequency_num, mv->service_id);
        return AUDIO_NO_ERROR;
    }
    else
    {
        return AUDIO_ERROR;
    }    
}
#endif

#define BT_VOICE_BUFFER_SIZE  640
LOCAL uint32 *s_bt_voice_play_buff = PNULL;
LOCAL uint32 *s_bt_voice_rec_buff = PNULL;
LOCAL DRVAUD_HANDLE s_bt_voice_play_handle = 0;
LOCAL DRVAUD_HANDLE s_bt_voice_rec_handle = 0;

/*****************************************************************************/
//  Description : Start bluetooth voice play.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN BtVoicePlay(void)
{
    AUDIO_RESULT_E  play_ret = 0, rec_ret = 0;
    
#ifndef WIN32    
    if(PNULL == s_bt_voice_play_buff)
    {
        s_bt_voice_play_buff = (uint32 *)SCI_ALLOC_APP( BT_VOICE_BUFFER_SIZE*4*2);
        if(PNULL == s_bt_voice_play_buff)
        {
           _DBG(           //MMISRV_TRACE_LOW:"[MMISRV]aud drv: BtVoicePlay() exit, no memory, error!!!"
           SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1614_112_2_18_3_23_56_284,(uint8*)""));
        }
        else
        {
            s_bt_voice_rec_buff = s_bt_voice_play_buff + BT_VOICE_BUFFER_SIZE;
        }
    }

    if((0 == s_bt_voice_play_handle) )
    {
        s_bt_voice_play_handle = AUDIO_CreateBufferHandle(hDownlinkCodec, SCI_NULL, hPCMDev, SCI_NULL, 
            SCI_NULL, 0, s_bt_voice_play_buff, BT_VOICE_BUFFER_SIZE, PNULL);
        play_ret = AUDIO_Play(s_bt_voice_play_handle, 0);
    }
        
    if (0 == s_bt_voice_rec_handle)
    {
        s_bt_voice_rec_handle = AUDIO_CreateRecordBufferHandle(hUplinkCodec, SCI_NULL, hPCMDev, SCI_NULL,
            (uint8*)s_bt_voice_rec_buff, 0,  s_bt_voice_rec_buff, 0, PNULL); 
        rec_ret = AUDIO_Play(s_bt_voice_rec_handle, 0);  
    }


        _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: BtVoicePlay() exit, play handle=0x%x, rec handle=0x%x, play_ret=%d, rec_ret=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1638_112_2_18_3_23_56_285,(uint8*)"dddd",s_bt_voice_play_handle, s_bt_voice_rec_handle, play_ret, rec_ret));
#endif

    if(AUDIO_NO_ERROR == play_ret && AUDIO_NO_ERROR == rec_ret)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Stop bluetooth voice play.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN BtVoiceStop(void)
{
    AUDIO_RESULT_E  play_ret1 = 0, rec_ret1 = 0;
    AUDIO_RESULT_E  play_ret2 = 0, rec_ret2 = 0;

    if(0 == s_bt_voice_play_handle && 0 == s_bt_voice_rec_handle)
    {
        return FALSE;
    }
    else
    {
        play_ret1 = AUDIO_Stop(s_bt_voice_play_handle);
        rec_ret1 = AUDIO_Stop(s_bt_voice_rec_handle);
        
        play_ret2 = AUDIO_CloseHandle(s_bt_voice_play_handle);
        s_bt_voice_play_handle = 0;
        
        rec_ret2 = AUDIO_CloseHandle(s_bt_voice_rec_handle);
        s_bt_voice_rec_handle = 0;

        if(s_bt_voice_play_buff != PNULL)
        {
            SCI_FREE(s_bt_voice_play_buff);
            s_bt_voice_play_buff = PNULL;
            s_bt_voice_rec_buff = PNULL;
        }
                _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: BtVoiceStop() exit, play handle=0x%x, rec handle=0x%x, stop play_ret=%d, stop rec_ret=%d, close play_ret=%d, close rec_ret=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1682_112_2_18_3_23_56_286,(uint8*)"dddddd",s_bt_voice_play_handle, s_bt_voice_rec_handle, play_ret1, rec_ret1, play_ret2, rec_ret2));
    }
    return TRUE;
}

/*****************************************************************************/
//  Description : Play audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_Play(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr, uint32 offset, MMISRVAUD_ROUTE_T route)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    MMISRVAUD_TYPE_E type = 0;

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Play() entry, handle=0x%x, type_ptr=%d,offset=%d, route=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1695_112_2_18_3_23_56_287,(uint8*)"dddd", handle, type_ptr, offset, route));

    if(PNULL == type_ptr || 0 == handle)
    {
        return FALSE;
    }
    
    type = type_ptr->type;
    switch(type)
    {
    case MMISRVAUD_TYPE_RING_FILE:
    case MMISRVAUD_TYPE_RING_BUF:
    case MMISRVAUD_TYPE_STREAMING:
    case MMISRVAUD_TYPE_ASP_FILE:
    case MMISRVAUD_TYPE_ASP_BUF:
        result = AUDIO_Play(handle, offset);
        if(result == AUDIO_NO_ERROR)
        {
            AUDIO_ChangeCodecPriority(handle, TRUE);
        }
        break;
    case MMISRVAUD_TYPE_RECORD_FILE:
    case MMISRVAUD_TYPE_RECORD_BUF:
        /* record file and record buff have the save address for the member of 'source' */
        if(MMISRVAUD_RECORD_SOURCE_NORMAL == type_ptr->record_file.source)
        {
            AUDIO_SetDevMode(AUDIO_DEVICE_MODE_HANDFREE);
        }
        result = AUDIO_Play(handle, offset);
        break;        
    case MMISRVAUD_TYPE_VOICE:
        {
            BOOLEAN res = 0;
            AUD_EnableVoiceProcessConfig(SCI_TRUE, SCI_FALSE, PNULL);
            result = AUDIO_Play(handle, offset);
            if(result!=AUDIO_NO_ERROR)
            {
                _DBG(                //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Play(), AUDIO_Play fail, res=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1732_112_2_18_3_23_56_288,(uint8*)"d", result));
                AUD_EnableVoiceProcessConfig(SCI_FALSE, SCI_FALSE, PNULL);
            }
            if(MMISRVAUD_ROUTE_BLUETOOTH == route)
            {
                res = BtVoicePlay();
                _DBG(                //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Play(), BtVoicePlay, res=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1738_112_2_18_3_23_56_289,(uint8*)"d", res));
            }
        }
        break;
    case MMISRVAUD_TYPE_IQ:
    case MMISRVAUD_TYPE_FM:
#ifndef WIN32        
        result = AUDIO_Play(handle, offset);
#else
        result = AUDIO_NO_ERROR;
#endif
        break;
    case MMISRVAUD_TYPE_VOICE_LOOPBACK:
        result = AUD_EnableVoiceLoopback(handle, TRUE, 100);
        break;
    case MMISRVAUD_TYPE_TONE_DTMF:
        {
            DTMF_TONE_ADP_EXT_OPE_PLAY_PARA_T tDtmfPara = {0};
            MMISRVAUD_TONE_DTMF_T *tone = (MMISRVAUD_TONE_DTMF_T *)(type_ptr);
            uint32 tone_res = 0;
            
            tDtmfPara.tone_id = (AUD_DTMF_TONE_ID_E)tone->id;
            tDtmfPara.duration = tone->duation;
            tDtmfPara.is_uplink = TRUE;
            tDtmfPara.hAudioHandle = handle;
            tDtmfPara.volume = 4096;

            result = AUDIO_ExeCodecExtOpe(
                handle,
                ANSI2UINT16("EXT_PLAY"),
                DTMF_TONE_ADP_EXT_OPE_PLAY_PARA_COUNT,
                &tDtmfPara,
                &tone_res
                );
            break;            
        }
    case MMISRVAUD_TYPE_TONE_GENERIC:
        {
            GENERIC_TONE_ADP_EXT_OPE_PLAY_PARA_T tGenericPara = {0};
            MMISRVAUD_TONE_GENERIC_T *tone = (MMISRVAUD_TONE_GENERIC_T *)(type_ptr);
            uint32 tone_res = 0;
            
            tGenericPara.tone_id = AUD_GENERIC_PRESS_KEY_TONE + tone->id;
            tGenericPara.duration = tone->duation;
            tGenericPara.hAudioHandle = handle;
            tGenericPara.volume = 16384;

            result = AUDIO_ExeCodecExtOpe(
                handle,
                ANSI2UINT16("EXT_PLAY"),
                GENERIC_TONE_ADP_EXT_OPE_PLAY_PARA_COUNT,
                &tGenericPara,
                &tone_res
                );
            break;        
        }
    case MMISRVAUD_TYPE_TONE_CUSTOM:
        {
            CUSTOM_TONE_ADP_EXT_OPE_PLAY_PARA_T tCustomPara = {0};    
            MMISRVAUD_TONE_CUSTOM_T *tone = (MMISRVAUD_TONE_CUSTOM_T *)(type_ptr);
            uint32 tone_res = 0;
            tCustomPara.freq1 = tone->freq;
            tCustomPara.freq2 = tone->freq;
            tCustomPara.freq3 = tone->freq;
            tCustomPara.duration = tone->duation;
            tCustomPara.hAudioHandle = handle;
            tCustomPara.volume = 16384;
            
            result = AUDIO_ExeCodecExtOpe(
                handle,
                ANSI2UINT16("EXT_PLAY"),
                CUSTOM_TONE_ADP_EXT_OPE_PLAY_PARA_COUNT,
                &tCustomPara,
                &tone_res
                );
            break;
        }
    case MMISRVAUD_TYPE_VIBERATE:
        GPIO_SetVibrator(TRUE);
        result = AUDIO_NO_ERROR;
        break;
    case MMISRVAUD_TYPE_VIDEO:
    case MMISRVAUD_TYPE_VIDEO_WP:
        result = VideoPlay(handle);
        break;
#ifdef ATV_SUPPORT        
    case MMISRVAUD_TYPE_ATV:
        result = AtvPlay(&type_ptr->atv);
        break;
#endif        
#ifdef MBBMS_SUPPORT        
    case MMISRVAUD_TYPE_MV:
        result = MvPlay(&type_ptr->mv);
        break;
#endif        
    default:
        break;
    }
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Play() exit, type=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1837_112_2_18_3_23_56_290,(uint8*)"dd", type, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Play audio asynchronus..
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
#if 0
PUBLIC BOOLEAN DRVAUD_PlayAsy(DRVAUD_HANDLE handle, uint32 offset)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    result = AUDIO_ASYPlay(handle, offset);
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PlayAsy() exit, handle=0x%x, offset=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1858_112_2_18_3_23_56_291,(uint8*)"ddd", handle, offset, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        AUDIO_ChangeCodecPriority(handle, TRUE);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#else
PUBLIC BOOLEAN DRVAUD_PlayAsy(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr, uint32 offset)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    MMISRVAUD_TYPE_E type = 0;
    BOOLEAN res = FALSE;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PlayAsy() entry, handle=0x%x, type_ptr=0x%x, offset=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1877_112_2_18_3_23_57_292,(uint8*)"ddd", handle, type_ptr, offset));

    if(PNULL == type_ptr || 0 == handle)
    {
        return FALSE;
    }
    
    type = type_ptr->type;
    switch(type)
    {
    case MMISRVAUD_TYPE_RING_FILE:
    case MMISRVAUD_TYPE_RING_BUF:
    case MMISRVAUD_TYPE_STREAMING:
        result = AUDIO_ASYPlay(handle, offset);
        if(result == AUDIO_NO_ERROR)
        {
            AUDIO_ChangeCodecPriority(handle, TRUE);
        }
        break;
    case MMISRVAUD_TYPE_RECORD_FILE:
    case MMISRVAUD_TYPE_RECORD_BUF:
    case MMISRVAUD_TYPE_VOICE:
    case MMISRVAUD_TYPE_FM:
        result = AUDIO_ASYPlay(handle, offset);
        break;
    case MMISRVAUD_TYPE_TONE_DTMF:
    case MMISRVAUD_TYPE_TONE_GENERIC:
    case MMISRVAUD_TYPE_TONE_CUSTOM:
    case MMISRVAUD_TYPE_VIBERATE:
        res = DRVAUD_Play(handle, type_ptr, offset, 0);
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PlayAsy() exit, type=%d, res=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1907_112_2_18_3_23_57_293,(uint8*)"dd", type, res));
        return res;
    default:
        break;
    }
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PlayAsy() exit, type=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1913_112_2_18_3_23_57_294,(uint8*)"dd", type, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

/*****************************************************************************/
//  Description : Pause audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_Pause(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    if(handle > 0 && type_ptr != PNULL)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_FM:
#ifndef WIN32            
            result = AUDIO_Stop(handle);
#else
            result = AUDIO_NO_ERROR;
#endif
            break;
        case MMISRVAUD_TYPE_VIBERATE:
            GPIO_SetVibrator(FALSE);
            result = AUDIO_NO_ERROR;
            break;
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:            
            result = VideoPause(handle);
            break;
#ifdef ATV_SUPPORT            
        case MMISRVAUD_TYPE_ATV:
            result = AtvPause();
            break;
#endif            
#ifdef MBBMS_SUPPORT
        case MMISRVAUD_TYPE_MV:
            result = MvPause();
            break;
#endif
        default:
            result = AUDIO_Pause(handle);
            break;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Pause() exit, handle=0x%x, type_ptr=0x%x, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_1967_112_2_18_3_23_57_295,(uint8*)"ddd", handle, type_ptr, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Stop playing audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_Stop(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    if(handle > 0)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_VIBERATE:
            GPIO_SetVibrator(FALSE);
            result = AUDIO_NO_ERROR;
            break;
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:        
            result = VideoStop(handle);
            break;
#ifdef ATV_SUPPORT            
        case MMISRVAUD_TYPE_ATV:
            result = AtvStop();
            break;
#endif            
#ifdef MBBMS_SUPPORT
        case MMISRVAUD_TYPE_MV:
            result = MvStop();
            break;
#endif
        case MMISRVAUD_TYPE_FM:
#ifndef WIN32
            result = AUDIO_Stop(handle);
#else
            result = AUDIO_NO_ERROR;
#endif
            break;
        case MMISRVAUD_TYPE_VOICE:
            {
                BOOLEAN res = 0;
                result = AUDIO_Stop(handle);   
                AUD_EnableVoiceProcessConfig(SCI_FALSE, SCI_FALSE, PNULL);
                res = BtVoiceStop();
                _DBG(                //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Stop(), HFG_AudioStop, res=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2021_112_2_18_3_23_57_296,(uint8*)"d", res));
            }
            break;
        case MMISRVAUD_TYPE_VOICE_LOOPBACK:
            AUD_EnableVoiceLoopback(handle, FALSE, 100);
            break;
        default:
            result = AUDIO_Stop(handle);
            break;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Stop() exit, handle=0x%x,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2029_112_2_18_3_23_57_297,(uint8*)"dd", handle, result));
    if(AUDIO_NO_ERROR == result || AUDIO_MULTI_STOP == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
}

/*****************************************************************************/
//  Description : Resume audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_Resume(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    if(handle > 0 && type_ptr != PNULL)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_FM:
#ifndef WIN32            
            result = AUDIO_Play(handle, 0);
#else
            result = AUDIO_NO_ERROR;
#endif
            break;
        case MMISRVAUD_TYPE_VIBERATE:
            GPIO_SetVibrator(TRUE);
            result = AUDIO_NO_ERROR;
            break;
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:        
            result = VideoResume(handle);
            break;
#ifdef ATV_SUPPORT            
        case MMISRVAUD_TYPE_ATV:
            result = AtvResume(&type_ptr->atv);
            break;
#endif            
#ifdef MBBMS_SUPPORT
        case MMISRVAUD_TYPE_MV:
            result = MvResume(&type_ptr->mv);
            break;
#endif
        default:
            result = AUDIO_Resume(handle);
            break;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Resume() exit, handle=0x%x, type_ptr=0x%s, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2082_112_2_18_3_23_57_298,(uint8*)"dsd", handle, type_ptr, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
}

/*****************************************************************************/
//  Description : Seek audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: Only used for video.
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_PauseSlight(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    DPLAYER_RETURN_E result = DPLAYER_ERROR;
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PauseSlight() entry, handle=0x%x, type_ptr=0x%"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2102_112_2_18_3_23_57_299,(uint8*)"d", handle, type_ptr));


    if(handle > 0 && type_ptr != PNULL)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:            
            result = DPLAYER_Pause(handle);
            break;            
        default:
            _DBG(            //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PauseSlight(), error type!!!!! type=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2114_112_2_18_3_23_57_300,(uint8*)"d", type_ptr->type));
            break;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_PauseSlight() exit, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2118_112_2_18_3_23_57_301,(uint8*)"d", result));

    if(DPLAYER_SUCCESS == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }    }

/*****************************************************************************/
//  Description : Seek audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note: Only used for video.
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_ResumeSlight(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr)
{
    DPLAYER_RETURN_E result = DPLAYER_ERROR;
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_ResumeSlight() entry, handle=0x%x, type_ptr=0x%"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2136_112_2_18_3_23_57_302,(uint8*)"d", handle, type_ptr));


    if(handle > 0 && type_ptr != PNULL)
    {
        switch(type_ptr->type)
        {
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:            
            result = DPLAYER_Resume(handle);
            break;            
        default:
            _DBG(            //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_ResumeSlight(), error type!!!!! type=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2148_112_2_18_3_23_57_303,(uint8*)"d", type_ptr->type));
            break;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_ResumeSlight() exit, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2152_112_2_18_3_23_57_304,(uint8*)"d", result));

    if(DPLAYER_SUCCESS == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

/*****************************************************************************/
//  Description : Seek audio.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_Seek(DRVAUD_HANDLE handle, uint32 offset)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    if(handle > 0)
    {
        result = AUDIO_Seek(handle, offset);
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_Seek() exit, handle=0x%x, offset=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2175_112_2_18_3_23_57_305,(uint8*)"ddd", handle, offset, result));
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get content info
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetContentInfo(DRVAUD_HANDLE handle, MMISRVAUD_CONTENT_INFO_T *content_info)
{
    AUDIO_FORMATINFO_T fmt_info = {0};
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetContentInfo() entry, handle=0x%x, content_info=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2193_112_2_18_3_23_57_306,(uint8*)"dd", handle, content_info));

    if(0 == handle || PNULL == content_info)
    {
        return FALSE;
    }
    
    if(AUDIO_IdentifyAudioFormat(handle, &fmt_info) == AUDIO_NO_ERROR)
    {
        content_info->total_data_length = fmt_info.uiTotalDataLength;
        content_info->total_time = fmt_info.uiTotalTime;
        content_info->samplerate = fmt_info.uiSamplerate;
        content_info->channel_num = fmt_info.uiChannelNum;
        content_info->bitrate = fmt_info.uiBitrate ;
        SCI_MEMCPY(&content_info->id3_tag, &fmt_info.tAudioId3Tag, sizeof(MMISRVAUD_ID3_TAG_T));
/*
        content_info->id3_tag.tag_code_type = fmt_info.tAudioId3Tag.tag_code_type;
        strncpy(content_info->id3_tag.year, fmt_info.tAudioId3Tag.year, 4);
        content_info->id3_tag.res0 = fmt_info.tAudioId3Tag.res0;
        content_info->id3_tag.res1 = fmt_info.tAudioId3Tag.res1;
        strncpy(content_info->id3_tag.tag, fmt_info.tAudioId3Tag.tag, 3);
        strncpy(content_info->id3_tag.title, fmt_info.tAudioId3Tag.title, 30);
        strncpy(content_info->id3_tag.artist, fmt_info.tAudioId3Tag.artist, 30);
        strncpy(content_info->id3_tag.album, fmt_info.tAudioId3Tag.album, 30);
        strncpy(content_info->id3_tag.comment, fmt_info.tAudioId3Tag.comment, 30);
        strncpy(content_info->id3_tag.genre, fmt_info.tAudioId3Tag.genre, 30);
        strncpy(content_info->id3_tag.content, fmt_info.tAudioId3Tag.content, 30);
        strncpy(content_info->id3_tag.track, fmt_info.tAudioId3Tag.track, 30);            
*/        
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetContentInfo() exit, ok!!!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2221_112_2_18_3_23_57_307,(uint8*)""));
        
        return TRUE;
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetContentInfo() exit, error!!!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2227_112_2_18_3_23_57_308,(uint8*)""));
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get playing info
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetPlayingInfo(DRVAUD_HANDLE handle, MMISRVAUD_PLAY_INFO_T *play_info)
{
    AUDIO_PLAYINFO_T info = {0};

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetPlayingInfo() entry, handle=0x%x, play_info=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2240_112_2_18_3_23_57_309,(uint8*)"dd", handle, play_info));

    if(PNULL == play_info || handle == 0)
    {
        return FALSE;
    }
    
    if(AUDIO_GetPlayingInfo(handle, &info) == AUDIO_NO_ERROR)
    {
        play_info->total_time = info.uiTotalTime;
        play_info->cur_time = info.uiCurrentTime;
        play_info->total_data_length = info.uiTotalDataLength;
        play_info->cur_data_offset = info.uiCurrentDataOffset;        
       _DBG(       //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetPlayingInfo() exit, ok!!! current time=%d, total time=%d"
       SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2253_112_2_18_3_23_57_310,(uint8*)"dd", info.uiCurrentTime, info.uiTotalTime));
        return TRUE;
    }
    else
    {
       _DBG(       //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetPlayingInfo() exit, error!!!"
       SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2258_112_2_18_3_23_57_311,(uint8*)""));
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get record info
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetRecordInfo(DRVAUD_HANDLE handle, MMISRVAUD_RECORD_INFO_T *record_info)
{
    //AUDIO_PLAYINFO_T info = {0};

    RECORD_TIME_IN_MS_T record_param = {0};
    uint32 result = 0;
    AUDIO_RESULT_E res; 

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordInfo() entry, handle=0x%x, record_info"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2275_112_2_18_3_23_57_312,(uint8*)"d", handle, record_info));
    
    if(PNULL == record_info)
    {
        return FALSE;
    }
    res = AUDIO_ExeCodecExtOpe(
                handle, 
                ANSI2UINT16("GET_RECORDTIME"), 
                RECORD_GET_RECORD_TIME_COUNT,
                 &record_param,
                &result);
    
    if(AUDIO_NO_ERROR == res)
    {
        record_info->total_time = record_param.ui_record_time_in_ms;
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordInfo() exit, total_time=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2291_112_2_18_3_23_57_313,(uint8*)"d", record_info->total_time));
        return TRUE;
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordInfo() exit, error!!!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2296_112_2_18_3_23_57_314,(uint8*)""));
        return FALSE;
    }
}


/*****************************************************************************/
//  Description : Get record info
//  Global resource dependence : 
//  Author:lei.he
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetRecordSize(DRVAUD_HANDLE handle, MMISRVAUD_RECORD_SIZE_T *record_info)
{
    //AUDIO_PLAYINFO_T info = {0};
    RECORD_FILE_SIZE rec_file_size_t = {0};
    uint32 result = 0;
    AUDIO_RESULT_E res; 

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordSize() entry, handle=0x%x, record_info"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2313_112_2_18_3_23_57_315,(uint8*)"d", handle, record_info));
    
    if(PNULL == record_info)
    {
        return FALSE;
    }
    res = AUDIO_ExeCodecExtOpe(
                handle, 
                ANSI2UINT16("GET_FILESIZE"), 
                RECORD_GET_FILE_SIZE_COUNT,
                 &rec_file_size_t,
                &result);
    
    if(AUDIO_NO_ERROR == res)
    {
        record_info->total_size = rec_file_size_t.cur_file_size;
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordSize() exit, total_time=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2329_112_2_18_3_23_57_316,(uint8*)"d", record_info->total_size));
        return TRUE;
    }
    else
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordSize() exit, error!!!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2334_112_2_18_3_23_57_317,(uint8*)""));
        return FALSE;
    }
}


/*****************************************************************************/
//  Description : Set play speed.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetPlaySpeed(DRVAUD_HANDLE handle, MMISRVAUD_SPEED_T speed)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    if(handle > 0)
    {
        result = AUDIO_SetSpeed(handle, (int32)speed);
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetPlaySpeed() exit, handle=0x%x, speed=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2351_112_2_18_3_23_57_318,(uint8*)"ddd", handle, speed, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Set EQ mode.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetEQMode(DRVAUD_HANDLE handle, MMISRVAUD_EQ_MODE_E eq_mode)
{
    BOOLEAN result = FALSE;
    
#ifdef MP3_EQ_SUPPORT 

    if(eq_mode >= MMISRVAUD_EQ_MODE_MAX)
    {
        return FALSE;
    }

    /*
      If just setting eq mode but not setting sampling frequency, then should set argc to 0
      If just setting sampling frequency but not setting eq mode, then should set argv to 0xFF
      If setting argv to 0xFF and setting argc to 0, then it will set eq mode to PASS(regular mode)
      sample frequency: 8000， 11025，12000，16000，22050，24000，32000，44100，48000 (in the unit of Hz)
      mmi should not set sample frequency!
    */
    
    result = AUD_SetEqMode(ConvertEQType(eq_mode));

#endif /* MP3_EQ_SUPPORT */

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetEQMode() exit, handle=0x%x, eq_mode=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2390_112_2_18_3_23_58_319,(uint8*)"ddd", handle, eq_mode, result));

    return result;
}

/*****************************************************************************/
//  Description : Get EQ mode.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC MMISRVAUD_EQ_MODE_E DRVAUD_GetEQMode(DRVAUD_HANDLE handle)
{
    EQ_EXP_MODE_E eq = 0;
    MMISRVAUD_EQ_MODE_E eq_mode = 0;
    
#ifdef MP3_EQ_SUPPORT 
    eq = AUD_GetEqMode();
    eq_mode = ConvertToMMIEQType(eq);
#endif

    _DBG(MMISRV_TRACE_LOW("[MMISRV]aud drv: DRVAUD_GetEQMode() exit, handle=0x%x, eq_mode=%d", handle, eq_mode));

    return eq_mode;
}
/*****************************************************************************/
//  Description : Set volume.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetVolume(DRVAUD_HANDLE handle, MMISRVAUD_VOLUME_T volume)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    BOOLEAN is_cur_mute = FALSE;

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetVolume() entry, handle=0x%x, volume=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2404_112_2_18_3_23_58_320,(uint8*)"dd", handle, volume));
    
    if(AUDIO_GetDownLinkMute(&is_cur_mute) != AUDIO_NO_ERROR)
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetVolume() exit, don't know the mute state!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2408_112_2_18_3_23_58_321,(uint8*)""));
        return FALSE;
    }
    
    if(0 == volume)
    {
        if(is_cur_mute)
        {
            result = AUDIO_NO_ERROR;
        }
        else
        {
            result = AUDIO_SetDownLinkMute(TRUE);
        }
    }
    else if(volume <= MMISRVAUD_VOLUME_LEVEL_MAX)
    {
        if(is_cur_mute)
        {
            AUDIO_SetDownLinkMute(FALSE);            
            result = AUDIO_SetVolume(volume);
        }
        else
        {
            result = AUDIO_SetVolume(volume);
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetVolume() exit, is_cur_mute=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2435_112_2_18_3_23_58_322,(uint8*)"dd", is_cur_mute, result));

    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get volume.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetVolume(DRVAUD_HANDLE handle, uint32 *volume)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    uint32 vol = 0;
    result = AUDIO_GetVolume(&vol);

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetVolume() exit, handle=0x%x, volume=0x%x, vol=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2457_112_2_18_3_23_58_323,(uint8*)"dddd", handle, volume, vol, result));

    if(AUDIO_NO_ERROR == result && volume != PNULL)
    {
        *volume = vol;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if 0
/*****************************************************************************/
//  Description : Set route.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetRoute(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr, MMISRVAUD_ROUTE_T route)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    AUDIO_DEVICE_MODE_TYPE_E cur_dev_mode = 0;
    AUDIO_DEVICE_MODE_TYPE_E set_dev_mode = ConvertRoute(route);
    BOOLEAN ret = FALSE;
    
    if(PNULL == type_ptr || 0 == route)
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetRoute() exit, error! handle=0x%x, route=0x%x, type_ptr=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2484_112_2_18_3_23_58_324,(uint8*)"ddd", handle, route, type_ptr));
        return FALSE;
    }
    if(AUDIO_GetDevMode(&cur_dev_mode) == AUDIO_NO_ERROR) 
    {
        if(set_dev_mode != cur_dev_mode)
        {
            switch(type_ptr->type)
            {
            /* To ref, voice is a special type, it need the handle */
            case MMISRVAUD_TYPE_VOICE:
                result = AUDIO_SetDevModeEx(handle, set_dev_mode);
                break;
            default:
                result = AUDIO_SetDevMode(set_dev_mode);
                break;            
            }
            if(result == AUDIO_NO_ERROR)
            {
                ret = TRUE;
            }
        }
        else
        {
            ret = TRUE;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetRoute() exit, handle=0x%x, route=0x%x, set_dev_mode=%d, cur_dev_mode=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2511_112_2_18_3_23_58_325,(uint8*)"dddd", handle, route, set_dev_mode, cur_dev_mode));
    
    return ret;
}

#else
/*****************************************************************************/
//  Description : Set route.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetRoute(DRVAUD_HANDLE handle, MMISRVAUD_TYPE_U *type_ptr, MMISRVAUD_ROUTE_T route)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    AUDIO_DEVICE_MODE_TYPE_E cur_dev_mode = 0;
    AUDIO_DEVICE_MODE_TYPE_E set_dev_mode = ConvertRoute(route);
    MMISRVAUD_RING_FMT_E ring_fmt = MMISRVAUD_RING_FMT_NONE;
    HAUDIODEV  hDevice = 0;
    BOOLEAN ret = FALSE;
    DPLAYER_AUDIO_PARAM_T    audio_param    = {0};
#ifdef ATV_SUPPORT    
    DRECORDER_AUD_DEV_PARAM_T atv_param = {0};
#endif    
    if(PNULL == type_ptr || 0 == route || 0 == handle)
    {
        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetRoute() exit, error! handle=0x%x, route=%d, type_ptr=0x%x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2535_112_2_18_3_23_58_326,(uint8*)"ddd", handle, route, type_ptr));
        return FALSE;
    }
    switch(type_ptr->type)
    {
    case MMISRVAUD_TYPE_VIDEO:
    case MMISRVAUD_TYPE_VIDEO_WP:            
        if(route & MMISRVAUD_ROUTE_BLUETOOTH)
        {
            audio_param.output_device = DPLAYER_AUD_DEV_BT;
            if(MMISRVAUD_BtIsA2dp())
            {
                audio_param.output_device_info.bt_info.b_a2dp_support = TRUE;
            }
            else
            {
                audio_param.output_device_info.bt_info.b_a2dp_support = FALSE;
            }
        }
        else
        {
            audio_param.output_device = DPLAYER_AUD_DEV_SPEAKER;
            audio_param.output_device_info.bt_info.b_a2dp_support = FALSE;
        }

        DPLAYER_SetAudioParam(handle, &audio_param);
        
        break;
#ifdef ATV_SUPPORT            
    case MMISRVAUD_TYPE_ATV:
    {
        DRECORDER_RETURN_E drecorder_ret = 0;
        if(route & MMISRVAUD_ROUTE_BLUETOOTH)
        {
            atv_param.aud_dev = DRECORDER_AUD_DEV_BT;
        }
        else
        {
            atv_param.aud_dev = DRECORDER_AUD_DEV_SPEAKER;
        }
        drecorder_ret = DRECORDER_ATV_SetAudioOutParam(&atv_param); 
        
        _DBG(MMISRV_TRACE_LOW("[MMISRV]aud drv: DRVAUD_SetRoute(), MMISRVAUD_TYPE_ATV type, drecorder_ret=%d, dev=0x%x", drecorder_ret, atv_param.aud_dev));
        
        break;
    }
#endif        
    case MMISRVAUD_TYPE_MV:
        break;
    case MMISRVAUD_TYPE_VIRTUAL:
        break;   
    case MMISRVAUD_TYPE_FM:
        break;
    case MMISRVAUD_TYPE_RING_BUF:
    case MMISRVAUD_TYPE_RING_FILE:
    case MMISRVAUD_TYPE_STREAMING:
        ring_fmt = GetAudioFmt(type_ptr);
        if(route & MMISRVAUD_ROUTE_BLUETOOTH)
        {
            hDevice = GetAudioDevice(TRUE, ring_fmt);
            result = AUDIO_SwitchDevice(handle, hDevice, PNULL);
        }
        else
        {
            hDevice = GetAudioDevice(FALSE, ring_fmt);
            result = AUDIO_SwitchDevice(handle, hDevice, PNULL);
        }

        _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetRoute(), normal audio, switch result=%d, hDevice=0x%x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2596_112_2_18_3_23_58_328,(uint8*)"dd", result, hDevice));

        break;
    case MMISRVAUD_TYPE_VOICE:
        if(route & MMISRVAUD_ROUTE_BLUETOOTH)
        {
            BtVoicePlay();
            //AUDIO_APP_EnableDataFetch(TRUE);
        }
        else
        {
            BtVoiceStop();
            //AUDIO_APP_EnableDataFetch(FALSE);            
        }
        break;
    default:
        break;
    }

    if(AUDIO_GetDevMode(&cur_dev_mode) == AUDIO_NO_ERROR) 
    {
        if(set_dev_mode != cur_dev_mode)
        {
            result = AUDIO_SetDevMode(set_dev_mode);
            if(result == AUDIO_NO_ERROR)
            {
                ret = TRUE;
            }
        }
        else
        {
            ret = TRUE;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetRoute() exit, handle=0x%x, route=%d, set_dev_mode=%d, cur_dev_mode=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2636_112_2_18_3_23_58_329,(uint8*)"dddd", handle, route, set_dev_mode, cur_dev_mode));
    
    return ret;
}

#endif
/*****************************************************************************/
//  Description : Get  route.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetRoute(DRVAUD_HANDLE handle, MMISRVAUD_ROUTE_T *route)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    AUDIO_DEVICE_MODE_TYPE_E cur_dev_mode = 0;
    //BOOLEAN ret = FALSE;
    result = AUDIO_GetDevMode(&cur_dev_mode);

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRoute() exit, handle=0x%x, route=%d, cur_dev_mode=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2653_112_2_18_3_23_58_330,(uint8*)"dddd", handle, route, cur_dev_mode, result));
    
    if(result == AUDIO_NO_ERROR && route != PNULL) 
    {
        *route = ConvertDevMode(cur_dev_mode);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*****************************************************************************/
//  Description : Get current audio configure.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetAudCfg(DRVAUD_HANDLE handle, MMISRVAUD_AUD_CFG_T *cfg)
{
    //AUDIO_RESULT_E result = AUDIO_ERROR;
    MMISRVAUD_ROUTE_T route = 0;
    MMISRVAUD_VOLUME_T volume = 0;
    MMISRVAUD_EQ_MODE_E eq_mode = 0;
    MMISRVAUD_SPEED_T speed = 0;

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetAudCfg() exit, handle=0x%x, cfg=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2679_112_2_18_3_23_58_331,(uint8*)"dd", handle, cfg));
    if(cfg != PNULL && DRVAUD_GetRoute(handle, &route) && DRVAUD_GetVolume(handle, &volume) )
    {
        cfg->volume = volume;
        cfg->route = route;
        cfg->eq_mode = eq_mode;
        cfg->speed = speed;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*****************************************************************************/
//  Description : Get current audio configure.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetSpectrum(DRVAUD_HANDLE handle, MMISRVAUD_SPECTRUM_INFO_T *info)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    //uint16 *spec_buf_ptr = PNULL;
    //uint16 *spec_num_ptr = PNULL;
    //uint16 *max_spec_value_ptr = PNULL;

    if(handle > 0 && info != PNULL)
    {
        result = AUDIO_GetSpectrum(handle, info->spec_buf_ptr, &info->spec_num, &info->max_spec_value);
    }
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetAudCfg() exit, handle=0x%x, info=0x%x, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2711_112_2_18_3_23_58_332,(uint8*)"ddd", handle, info, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

/*****************************************************************************/
//  Description : Get current audio configure.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_OpenSpectrum(DRVAUD_HANDLE handle)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    
    if(handle > 0)
    {
        result = AUDIO_OpenSpectrum(handle);
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_OpenSpectrum() exit, handle=0x%x,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2735_112_2_18_3_23_58_333,(uint8*)"dd", handle, result));

    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get current audio configure.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_CloseSpectrum(DRVAUD_HANDLE handle)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    
    if(handle > 0)
    {
        result = AUDIO_CloseSpectrum(handle);
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_CloseSpectrum() exit, handle=0x%x,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2759_112_2_18_3_23_58_334,(uint8*)"dd", handle, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get current audio configure.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetVoiceLB(DRVAUD_HANDLE handle, BOOLEAN is_enable)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    
    if(handle > 0)
    {
        result = AUD_EnableVoiceLoopback(handle, is_enable, 100);
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetVoiceLB() exit, handle=0x%x,is_enable=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2783_112_2_18_3_23_58_335,(uint8*)"ddd", handle, is_enable, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*****************************************************************************/
//  Description : Set all route mute.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetMute(DRVAUD_HANDLE handle, BOOLEAN is_mute)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    BOOLEAN is_cur_mute = FALSE;
    
    if(AUDIO_GetDownLinkMute(&is_cur_mute) == AUDIO_NO_ERROR)
    {
        if (is_mute != is_cur_mute)
        {
            result = AUDIO_SetDownLinkMute(is_mute);
        }
        else
        {
            result = AUDIO_NO_ERROR;
        }
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetMute() exit, handle=0x%x,is_mute=%d, is_cur_mute=%d,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2816_112_2_18_3_23_58_336,(uint8*)"dddd", handle, is_mute, is_cur_mute,result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get if all route is mute.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetMute(DRVAUD_HANDLE handle, BOOLEAN *is_mute)
{
    AUDIO_RESULT_E result = AUDIO_ERROR;
    BOOLEAN is_cur_mute = FALSE;

    result = AUDIO_GetDownLinkMute(&is_cur_mute);
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetMute() exit, handle=0x%x,is_mute=0x%x, is_cur_mute=%d,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2838_112_2_18_3_23_58_337,(uint8*)"dddd", handle, is_mute, is_cur_mute,result));
    
    if(result == AUDIO_NO_ERROR && is_mute != PNULL)
    {
        *is_mute = is_cur_mute;
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

/*****************************************************************************/
//  Description : Set up link mute.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetUplinkMute(DRVAUD_HANDLE handle, BOOLEAN is_mute)
{
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;

    result = AUDIO_SetUpLinkMute(is_mute);

    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetUplinkMute() exit, handle=0x%x,is_mute=%d, result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2861_112_2_18_3_23_58_338,(uint8*)"ddd", handle, is_mute, result));
    
    if(AUDIO_NO_ERROR == result)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Set play rate.
//  Global resource dependence : 
//  Author:Yintang.Ren
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_SetPlayRate(DRVAUD_HANDLE handle, MMISRVAUD_PLAY_RATE_E rate)
{
#ifdef AUDIO_RATE_SUPPORT
    AUDIO_RESULT_E result = AUDIO_NO_ERROR;
    RATECONTROL_EXP_PARA_T rate_para = {0};
    uint32  param_count = 0;
    static MMISRVAUD_PLAY_RATE_E play_rate = MMISRVAUD_PLAY_RATE_MAX;
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv:DRVAUD_SetPlayRate entry, handle=0x%x,rate =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2884_112_2_18_3_23_59_339,(uint8*)"dd",handle, rate));

    if(rate != play_rate)
    {
        extern HAUDIOEXP hRateControlExp;
        rate_para.eRateControl = ConverPlayRate(rate);
        result = AUDIO_ExeExpSetParaOpe(
                    hRateControlExp,
                    ANSI2UINT16("RATECONTROL_EXP"),
                    param_count, 
                    &rate_para
                    );
    }
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_SetPlayRate() exit, handle=0x%x,rate=%d,play_rate=%d,result=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2897_112_2_18_3_23_59_340,(uint8*)"dddd", handle, rate, play_rate, result));

    if(AUDIO_NO_ERROR == result)
    {
        play_rate = rate;
        return TRUE;
    }
    else
#endif
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : Get record db by handle.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN DRVAUD_GetRecordDb(DRVAUD_HANDLE handle, uint32 *db)
{
    RECORD_VOICEDB_T record_param = {0};
    BOOLEAN result = 0;
    AUDIO_RESULT_E res = AUDIO_NO_ERROR;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]aud drv:DRVAUD_GetRecordDb entry, handle=0x%x, db=-0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2921_112_2_18_3_23_59_341,(uint8*)"dd",handle, db));

    if(handle > 0 && db != PNULL)
    {
        res = AUDIO_ExeCodecExtOpe(
            handle,
            ANSI2UINT16("GET_VOICEDB"),
            RECORD_GET_RECORD_VOICEDB_COUNT,
            &record_param,
            &result
            );
        if(AUDIO_NO_ERROR == res)
        {
            *db = record_param.db;
            result = TRUE;
        }
    }
    
    //SCI_TRACE_LOW:"[MMISRV]aud drv: DRVAUD_GetRecordDb exit, res=%d,db =%d!"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2939_112_2_18_3_23_59_342,(uint8*)"dd", res, record_param.db);
    
    return result;
}

/*****************************************************************************/
//  Description : Handle audio callback message.
//  Global resource dependence : none
//  Author: Yintang.Ren
//  Note: 
/*****************************************************************************/
PUBLIC void MMISRVAUD_HandleCallBackMsg(
                    void *msg_ptr
                    )
{
    MMISRVAUD_REPORT_T rpt_data = {0};

    DRVAUD_HANDLE handle = 0; 
    AUDIO_RESULT_E  audio_result = AUDIO_NO_ERROR;

    MmiAudioCallbackMsgS *callback_info_ptr = (MmiAudioCallbackMsgS *)msg_ptr;
    
    _DBG(    //MMISRV_TRACE_LOW:"[MMISRV]: Audio MMISRVAUD_HandleCallBackMsg() entry, callback_info_ptr=0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_2959_112_2_18_3_23_59_343,(uint8*)"d", callback_info_ptr));

    if(callback_info_ptr != PNULL)
    {
        switch(callback_info_ptr->type)
        {
        case MMISRVAUD_TYPE_VIDEO:
        case MMISRVAUD_TYPE_VIDEO_WP:
            handle = callback_info_ptr->hAudio; 
            if(handle != 0)
            {
                rpt_data.report = MMISRVAUD_REPORT_END;
                if (0 == callback_info_ptr->affix_info)
                {
                    rpt_data.data1 = (uint32)(MMISRVAUD_REPORT_RESULT_SUCESS);
                }
                else if (0 > (int32)callback_info_ptr->affix_info)
                {
                    if (-11 == (int32)callback_info_ptr->affix_info)
                    {
                        rpt_data.data1 = (uint32)(MMISRVAUD_REPORT_RESULT_NOT_SUPPORT);
                    }
                    else
                    {
                        rpt_data.data1 = (uint32)(MMISRVAUD_REPORT_RESULT_ERROR);
                    }
                }
                else
                {
                    rpt_data.data1 = (uint32)(MMISRVAUD_REPORT_RESULT_STOP);
                }
                MMISRVAUD_HandleAudioReport(handle, &rpt_data);
            }
            break;
#ifdef ATV_SUPPORT            
        case MMISRVAUD_TYPE_ATV:
            break;
#endif            
#ifdef MBBMS_SUPPORT            
        case MMISRVAUD_TYPE_MV:
            break;
#endif            
        default:
            if(AUDIO_PLAYEND_IND == callback_info_ptr->notify_info)
            {
                rpt_data.report = MMISRVAUD_REPORT_END;
                handle = callback_info_ptr->hAudio;
                audio_result = (AUDIO_RESULT_E)callback_info_ptr->affix_info;
                if(handle != 0)
                {
                    switch(audio_result)
                    {
                    case AUDIO_NO_ERROR:
                        rpt_data.data1 = (uint32)(MMISRVAUD_REPORT_RESULT_SUCESS);
                        break;
                    default:
                        rpt_data.data1 = (uint32)(MMISRVAUD_REPORT_RESULT_ERROR);
                        break;
                    }
                    MMISRVAUD_HandleAudioReport(handle, &rpt_data);            
                }
            }
            break;
        }

                _DBG(        //MMISRV_TRACE_LOW:"[MMISRV]: Audio MMISRVAUD_HandleCallBackMsg() exit, handle=0x%x, result=%d, notify_info=%d,affix_info=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMISRVAUD_DRV_3025_112_2_18_3_23_59_344,(uint8*)"dddd", handle, audio_result, callback_info_ptr->notify_info, callback_info_ptr->affix_info));
    }
}


