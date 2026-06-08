
ifneq ($(strip $(PLATFORM)), SC8800H)
MCFLAG_OPT		+=-DBACKGROUND_MUSIC_ENABLE  
endif
#macro definition --temp code


MINCPATH =  MS_Ref/source/aud_dev/inc \
 MS_Ref/source/aud_exp/eq/inc \
 MS_Ref/source/aud_exp/digitalgain/inc \
 MS_Ref/source/aud_exp/transsamplerate/inc \
 MS_Ref/source/codec/aac/inc_export \
 MS_Ref/source/codec/record/inc_export \
 MS_Ref/source/codec/amr/inc_export \
 MS_Ref/source/codec/custom_tone/inc_export \
 MS_Ref/source/codec/dsp_codec/inc_export \
 MS_Ref/source/codec/dtmf_tone/inc_export \
 MS_Ref/source/codec/generic_tone/inc_export \
 MS_Ref/source/codec/midi/inc_export \
 MS_Ref/source/codec/mp3/inc_export \
 MS_Ref/source/codec/voice/inc_export \
 MS_Ref/source/codec/wma/inc_export \
 MS_Ref/source/audioservice/inc_export \
 MS_Ref/source/audioservice/inc \
 MS_Ref/source/aud_config/inc_export \
 MS_Ref/source/aud_config/inc \
 MS_Ref/source/bt_pal_csr/kernel/low_level\
 MS_Ref/export/inc
MINCPATH += chip_drv/export/inc/outdated Base/ps/export/inc
MINCPATH +=$(L1_INCPATH) 	

MSRCPATH =  MS_Ref/source/aud_config/src


SOURCES				= audio_config.c    \
					acm_class_lib.c    \
					audio_app.c		

ifneq ($(strip $(PLATFORM)), SC8800H)
SOURCES				+= bkmusic_as.c
endif

ifeq ($(strip $(PLATFORM)), SC6600L)
MCFLAG_OPT		+=-DMP3_DECODE_IN_DSP -DTRANS_SAM_SUPPORT -DARMVB_RECORD_SUPPORT -DDSP_DEC_SBC_PROCESS -D_OLD_MIX_VOICE_INTERFACE
endif

ifeq ($(strip $(PLATFORM)), SC6800H)
MCFLAG_OPT		+=-DMP3_DECODE_IN_DSP -DTRANS_SAM_SUPPORT -DARMVB_RECORD_SUPPORT -DDSP_DEC_SBC_PROCESS -D_OLD_MIX_VOICE_INTERFACE
endif

ifeq ($(strip $(PLATFORM)), SC6530)
MCFLAG_OPT		+=-DMP3_DECODE_IN_DSP -DTRANS_SAM_SUPPORT -DARMVB_RECORD_SUPPORT -DEQ_HAS_MULTI_BANDS -DDSP_DEC_SBC_PROCESS -D_OLD_MIX_VOICE_INTERFACE
endif

ifeq ($(strip $(PLATFORM)), SC8800H)
MCFLAG_OPT		+=-DAAC_DECODE_IN_ARM -DRATE_CONTROL_SUPPORT -DTRANS_SAM_SUPPORT -DMP3_DECODE_IN_DSP 
endif
ifeq ($(strip $(PLATFORM)), SC8800G)
MCFLAG_OPT		+=-DAAC_DECODE_IN_ARM -DRATE_CONTROL_SUPPORT -DTRANS_SAM_SUPPORT -DMP3_DECODE_IN_DSP -DARMVB_RECORD_SUPPORT -DEQ_HAS_MULTI_BANDS
endif

#pls modify in fut
ifeq ($(strip $(CMMB_SUPPORT)), TRUE)
MINCPATH		+=MS_MMI/source/mmi_app/kernel/h
SOURCES			+=audio_mtv.c audio_extpcm.c
MCFLAG_OPT		+=-DMTV_SUPPORT

ifeq ($(strip $(NANDBOOT_SUPPORT)), FALSE)
MCFLAG_OPT		+=-DMTV_NOR_VERSION
endif

#add by sky start
else
ifeq ($(strip $(MRAPP_SUPPORT)), TRUE)
MINCPATH		+=MS_MMI/source/mmi_app/kernel/h
SOURCES			+=audio_extpcm.c
endif
#add by sky end
endif

ifeq ($(strip $(PLATFORM)), SC6800H)
ifneq ($(strip $(BLUETOOTH_SUPPORT)),NONE)
MCFLAG_OPT        += -DDIGITAL_GAIN_SUPPORT
endif
endif

ifeq ($(strip $(PLATFORM)), SC6530)
ifneq ($(strip $(BLUETOOTH_SUPPORT)),NONE)
MCFLAG_OPT        += -DDIGITAL_GAIN_SUPPORT
endif
endif

ifeq ($(strip $(MODEM_PLATFORM)), TRUE)
MCFLAG_OPT      +=-D_PCM_DUMP_VOICE_MODEM_
endif

MCFLAG_OPT		+=-DUPLINK_CODEC_SUPPORT -DDOWNLINK_CODEC_SUPPORT
