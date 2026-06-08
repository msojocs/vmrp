# Define source file folder to SRC_LIST
ifeq ($(strip $(DSP_SOLUTION)),DUALMACDSP)
   AUDFOLDER         =  hal\audio\src\v2
   AUD_SOLUTION      =  V2
else   
   ifneq ($(strip $(SMART_PHONE_CORE)),NONE)
      AUDFOLDER         =  hal\audio\src\v1\sp
      AUD_SOLUTION      =  V1_SP
   else 
      AUDFOLDER         =  hal\audio\src\v1
      AUD_SOLUTION      =  V1
   endif
endif


# Start Define source file lists to SRC_LIST
# Common part in v1, v2 and v1SP
SRC_LIST = $(strip $(AUDFOLDER))\afe2.c \
           $(strip $(AUDFOLDER))\am.c \
           $(strip $(AUDFOLDER))\sp_drv.c \
           $(strip $(AUDFOLDER))\tone2.c \
           $(strip $(AUDFOLDER))\media.c \
           $(strip $(AUDFOLDER))\l1audio_trace.c \
           $(strip $(AUDFOLDER))\pcm4way.c \
           $(strip $(AUDFOLDER))\audio_service.c \
           $(strip $(AUDFOLDER))\wav_codec.c \
           $(strip $(AUDFOLDER))\wav.c \
           $(strip $(AUDFOLDER))\dpmgr.c \
           $(strip $(AUDFOLDER))\l1audio_sph_srv.c \
           interface\hal\audio\audio_create.c
           
ifneq ($(filter __VM_CODEC_SUPPORT__, $(strip $(MODULE_DEFS))),)
   SRC_LIST +=  $(strip $(AUDFOLDER))\vm.c
endif

ifneq ($(filter __CVSD_CODEC_SUPPORT__, $(strip $(MODULE_DEFS))),)
   SRC_LIST +=  $(strip $(AUDFOLDER))\bt_sco_drv.c
   SRC_LIST +=  $(strip $(AUDFOLDER))\bt_sco_app.c
endif

# Common part in V1_SP
ifeq ($(strip $(AUD_SOLUTION)),V1_SP)
   SRC_LIST += $(strip $(AUDFOLDER))\vm_strm_drv.c \
               $(strip $(AUDFOLDER))\bgsnd_drv.c \
               $(strip $(AUDFOLDER))\sp_enhance.c \
               $(strip $(AUDFOLDER))\wav_thumb.c
else
# Common part in V1 and V2
   SRC_LIST += $(strip $(AUDFOLDER))\audioPP.c \
               $(strip $(AUDFOLDER))\tone_loopback_rec.c \
               $(strip $(AUDFOLDER))\audioCF.c

ifneq ($(filter __VM_CODEC_SUPPORT__, $(strip $(MODULE_DEFS))),)
   SRC_LIST +=  $(strip $(AUDFOLDER))\vm_drv.c
endif

ifneq ($(filter __SPECTRUM_DISPLAY_SUPPORT__, $(strip $(MODULE_DEFS))),)
   SRC_LIST +=  $(strip $(AUDFOLDER))\spt_analyzer.c
endif

ifneq ($(filter __VIBRATION_SPEAKER_SUPPORT__, $(strip $(MODULE_DEFS))),)
   SRC_LIST +=  $(strip $(AUDFOLDER))\vibration_drv.c
endif

   ifeq ($(strip $(AUD_SOLUTION)),V1)
      SRC_LIST += $(strip $(AUDFOLDER))\audio_idma.c \
                  $(strip $(AUDFOLDER))\sp_enhance.c

      ifneq ($(filter __DRA_DECODE_SUPPORT__, $(strip $(MODULE_DEFS))),)
        ifeq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
           SRC_LIST += $(strip $(AUDFOLDER))\dra_drv.c
        endif
      endif
      
      ifneq ($(filter __DUAL_TALK_MODEM_SUPPORT__, $(strip $(MODULE_DEFS))),)
        SRC_LIST += $(strip $(AUDFOLDER))\spc_drv.c
      endif      
      
   endif
   
   ifeq ($(strip $(AUD_SOLUTION)),V2)
      ifneq ($(strip $(SMART_PHONE_CORE)),NONE)
         SRC_LIST += $(strip $(AUDFOLDER))\audl_service.c \
                     $(strip $(AUDFOLDER))\spc_drv.c
      endif
      SRC_LIST += $(strip $(AUDFOLDER))\ut_acoustic_loopback.c
      ifneq ($(filter DSP_WT_SYN, $(strip $(MODULE_DEFS))),)
         SRC_LIST += $(strip $(AUDFOLDER))\Wavetable_SW.c
      endif
      ifneq ($(filter __DATA_CARD_SPEECH__, $(strip $(MODULE_DEFS))),)
         SRC_LIST += $(strip $(AUDFOLDER))\audl_service.c \
                     $(strip $(AUDFOLDER))\sp_strm_drv.c
      endif
   endif
   ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
      SRC_LIST += $(strip $(AUDFOLDER))\Wavetable_SW.c
   endif
endif

ifneq ($(filter __UMTS_RAT__, $(strip $(MODULE_DEFS))),)
   SRC_LIST += $(strip $(AUDFOLDER))\amr_table.c \
               $(strip $(AUDFOLDER))\sp_3g.c
else
  ifneq ($(filter __UMTS_TDD128_MODE__, $(strip $(MODULE_DEFS))),)
    SRC_LIST += $(strip $(AUDFOLDER))\amr_table.c \
                $(strip $(AUDFOLDER))\sp_3g.c
  endif
endif

ifneq ($(filter BGSND_ENABLE, $(strip $(MODULE_DEFS))),)
  SRC_LIST += $(strip $(AUDFOLDER))\snd_effect.c
endif
ifneq ($(filter WAV_CODEC, $(strip $(MODULE_DEFS))),)
  SRC_LIST += $(strip $(AUDFOLDER))\wav_thumb.c
  SRC_LIST += $(strip $(AUDFOLDER))\pcm_strm_drv.c
  WAV_DRIVER_NOT_SUPPORT_PLATFORM = MT6516 MT6573
  ifeq ($(filter $(strip $(MODULE_DEFS)) ,$(WAV_DRIVER_NOT_SUPPORT_PLATFORM)),)
    ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\wav_drv.c
    else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\wav_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\wav_drv.c
       endif
    endif     
  endif
  ifneq ($(strip $(AUD_SOLUTION)),V2)
    SRC_LIST += $(strip $(AUDFOLDER))\pcm.c
  endif
endif

ifneq ($(filter __CTM_SUPPORT__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\ctm_drv.c
endif

ifneq ($(filter __ECALL_SUPPORT__, $(strip $(MODULE_DEFS))),)
  SRC_LIST += $(strip $(AUDFOLDER))\eCall_drv.c
endif

ifeq ($(strip $(SMART_PHONE_CORE)),NONE)
   ifneq ($(filter AAC_DECODE, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\aac_drv.c
     else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\aac_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\aac_drv.c
       endif
     endif
   endif
   ifneq ($(filter AAC_PLUS_DECODE, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V1)
       SRC_LIST += $(strip $(AUDFOLDER))\aac_huffman_table.c
     endif
   endif
   ifneq ($(filter AMR_CODEC, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\amr.c \
                 $(strip $(AUDFOLDER))\amr_table.c
     ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\amr_drv.c
     else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\amr_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\amr_drv.c
       endif
     endif
     ifneq ($(filter DEDI_AMR_REC, $(strip $(MODULE_DEFS))),)
       ifeq ($(strip $(AUD_SOLUTION)),V1)
           SRC_LIST += $(strip $(AUDFOLDER))\amr_dedi.c
       endif
     endif
   endif
   ifneq ($(filter AMRWB_ENCODE, $(strip $(MODULE_DEFS))),)
     AWB_SW_ENC_SUPPORT_PLATFORM = MT6255 MT6922
     ifneq ($(filter $(strip $(MODULE_DEFS)), $(AWB_SW_ENC_SUPPORT_PLATFORM)),)
       SRC_LIST += $(strip $(AUDFOLDER))\awb_enc.c
     endif
   endif
   ifneq ($(filter __APE_DECODE__, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\ape_drv.c
     else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\ape_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\ape_drv.c
       endif
     endif
   endif
   ifneq ($(filter __VORBIS_DECODE__, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\vorbis_drv.c
       SRC_LIST += $(strip $(AUDFOLDER))\vorbis_strm_drv.c
     else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\vorbis_comp_drv.c
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\vorbis_strm_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\vorbis_drv.c
          SRC_LIST += $(strip $(AUDFOLDER))\vorbis_strm_drv.c
       endif
     endif
   endif
   ifneq ($(filter __VORBIS_ENCODE__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\vorbis_enc_drv.c
   endif
   ifneq ($(filter __AUDIO_DSP_LOWPOWER_V2__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\audlp2_drv.c
   endif
   ifneq ($(filter __ENABLE_AUDIO_DVT__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\audio_DVT.c
   endif
   ifneq ($(filter __BT_AUDIO_VIA_SCO__, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V1)
       SRC_LIST += $(strip $(AUDFOLDER))\AVB.c
     endif
   endif
   ifneq ($(filter __COOK_DECODE__, $(strip $(MODULE_DEFS))),)
     ifeq ($(filter __COOK_DECODE_DSP__, $(strip $(MODULE_DEFS))),)     
        ifeq ($(strip $(AUD_SOLUTION)),V2)
          SRC_LIST += $(strip $(AUDFOLDER))\cook_drv.c
        else
          ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
             SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\cook_comp_drv.c
          else
             SRC_LIST += $(strip $(AUDFOLDER))\cook_drv.c
          endif
        endif
     endif
   endif
   ifneq ($(filter __COOK_DECODE__, $(strip $(MODULE_DEFS))),)
     ifneq ($(filter __COOK_DECODE_DSP__, $(strip $(MODULE_DEFS))),)
       SRC_LIST += $(strip $(AUDFOLDER))\ra_drv.c
     endif
   endif
   ifneq ($(filter DAF_DECODE, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\daf_drv.c
     else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\daf_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\daf_drv.c
       endif
     endif
   endif
   ifneq ($(filter __FLAC_DECODE__ , $(strip $(MODULE_DEFS))),)
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\flac_comp_drv.c
       endif
   endif
   ifneq ($(filter __BES_EQ_SUPPORT__, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V1)
       SRC_LIST += $(strip $(AUDFOLDER))\eq_table.c
     endif
   endif
   ifneq ($(filter __I2S_INPUT_MODE_SUPPORT__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\i2s_drv.c
   endif
   ifneq ($(filter CYBERON_DIC_TTS, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\tts_drv.c
   else
     ifneq ($(filter IFLY_TTS, $(strip $(MODULE_DEFS))),)
       SRC_LIST += $(strip $(AUDFOLDER))\tts_drv.c
       endif
   endif
   ifneq ($(filter SINOVOICE_TTS, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\tts_drv.c
   endif   
   ifneq ($(filter __BT_A2DP_PROFILE__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\sbc_drv.c
     ifeq ($(filter __LOW_COST_SUPPORT_COMMON__, $(strip $(MODULE_DEFS))),)
       SRC_LIST += $(strip $(AUDFOLDER))\daf_rtpss.c
     endif
   endif
   ifneq ($(filter UT_2WAYWB_RECORD_SUPPORT, $(strip $(MODULE_DEFS))),)
     ifneq ($(filter __UTIL_CLI_SUPPORT__, $(strip $(MODULE_DEFS))),)
       SRC_LIST += $(strip $(AUDFOLDER))\ut_2waywb_record.c
     endif
   endif
   ifneq ($(filter __ACOUSTIC_LOOPBACK_SUPPORT__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\ut_acoustic_loopback.c
   endif
   ifneq ($(filter __UTIL_CLI_SUPPORT__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\util_cli_input.c \
                 $(strip $(AUDFOLDER))\util_cli_parser.c \
                 $(strip $(AUDFOLDER))\util_cli_task.c \
                 $(strip $(AUDFOLDER))\util_aud_cmd.c
   endif
   ifneq ($(filter __LG_VOICE_ENGINE__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\voice_clarity.c
   endif
   ifneq ($(filter VRSI_CYBERON, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\vr_si.c
   endif
   ifneq ($(filter WMA_DECODE, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V2)
       SRC_LIST += $(strip $(AUDFOLDER))\wma_drv.c
     else
       ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
          SRC_LIST += $(strip $(AUDFOLDER))\cmpdrv\wma_comp_drv.c
       else
          SRC_LIST += $(strip $(AUDFOLDER))\wma_drv.c
       endif
     endif
   endif
   ifneq ($(filter __DAF_ENCODE__, $(strip $(MODULE_DEFS))),)
     SRC_LIST +=  $(strip $(AUDFOLDER))\mp3_enc_drv.c
   endif
   ifneq ($(filter __BT_SUPPORT__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\BT_PcmLoopback.c
     ifneq ($(filter BT_Loopback_Test, $(strip $(COMP_DEFS))),)
       SRC_LIST += $(strip $(AUDFOLDER))\BT_loopback.c
     endif   
   endif
   ifneq ($(filter DSP_WT_SYN, $(strip $(MODULE_DEFS))),)
     ifeq ($(strip $(AUD_SOLUTION)),V1)
        DSP_WT_SYN_V1_SUPPORT_PLATFORM = MT6219 MT6223 MT6225 MT6229 MT6253T MT6253 MT6268A MT6268T MT6268H MT6223P
        ifneq ($(filter $(strip $(MODULE_DEFS)) ,$(DSP_WT_SYN_V1_SUPPORT_PLATFORM)),)
           SRC_LIST += $(strip $(AUDFOLDER))\Wavetable_DSPv1.c
        else
           ifeq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
           SRC_LIST += $(strip $(AUDFOLDER))\Wavetable_DSPv2.c
           endif
        endif
     endif
   endif
   ifneq ($(filter STREAM_SUPPORT, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\poc_amr.c
   else
     ifneq ($(filter __VOIP__, $(strip $(MODULE_DEFS))),)
       SRC_LIST += $(strip $(AUDFOLDER))\poc_amr.c
       endif
   endif
   ifneq ($(filter __VOIP__, $(strip $(MODULE_DEFS))),)
     SRC_LIST += $(strip $(AUDFOLDER))\PCMRB.c
   endif
endif           
#  Define include path lists to INC_DIR
INC_DIR = $(strip $(AUDFOLDER)) \
          $(strip $(AUDFOLDER))\inc \
          $(strip $(AUDFOLDER))\comp \
          $(strip $(AUDFOLDER))\comp\VoRTP \
          hal\audio\lib\MTKINC \
          hal\audio\lib\VENDOR\tc01 \
          interface\hal\audio \
          interface\hal\video \
          interface\hal\system \
          hal\dsp_ram \
          hal\dp_engine\ctm \
          hal\dp_engine \
          $(strip $(PS_FOLDER))\l4\include \
          hal\audio\src32_inc \
          drv\include \
          hal\video\demuxer\mp4_parser\inc \
          hal\system\init\inc \
          l1_dm\l1d_ext \
          hal\system\DP\inc \
          hal\system\dcmgr\inc \
          custom\common\hal

ifeq ($(strip $(AUD_SOLUTION)),V1)
  ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
     INC_DIR += hal\audio\src\common\inc
  endif
endif

ifneq ($(filter __MMI_DSM_NEW__ , $(strip $(MODULE_DEFS))),)                  
	INC_DIR += plutommi\mmi\mythroad\mythroadinc
endif 
 
# Define the specified compile options to COMP_DEFS
COMP_DEFS = APCS_INTWORK
 
          
ifeq ($(strip $(AUD_SOLUTION)),V1)
  ifneq ($(filter __AUDIO_COMPONENT_SUPPORT__, $(strip $(MODULE_DEFS))),)
  endif
endif
 

