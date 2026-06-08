include Makefile.rules
include ./project_$(PROJECT).mk
include Makefile.verify
include Makefile.rules

#
#  project config
#

MMI_RES_DIR := $(strip $(MMI_RES_DIR))

#define resource replace flag
REP_FLAG := TRUE
#ifeq ($(strip $(SPRD_CUSTOMER)), TRUE)
#REP_FLAG := FALSE
#endif

ifeq ($(strip $(VIDEO_PLAYER_SUPPORT)), TRUE)
SOURCES	 += videoplayer_mdu_def.h
#else
#SOURCES	 += mpeg4_mdu_def.h
endif

ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
ifeq ($(strip $(MOBILE_VIDEO_SUPPORT)), TRUE)
SOURCES += mobile_video_mdu_def.h
endif
endif
SOURCES	 += common_mdu_def.h tools_mdu_def.h clock_mdu_def.h bt_mdu_def.h camera_mdu_def.h cc_mdu_def.h ussd_mdu_def.h\
 connection_mdu_def.h dyna_mdu_def.h ebook_mdu_def.h engtd_mdu_def.h eng_mdu_def.h files_manager_mdu_def.h drm_mdu_def.h fm_mdu_def.h \
 game_boxman_mdu_def.h game_quintet_mdu_def.h game_link_mdu_def.h game_manager_mdu_def.h gps_mdu_def.h im_mdu_def.h \
 kuro_mdu_def.h mms_mdu_def.h audioplayer_mdu_def.h  multim_mdu_def.h \
 ocr_mdu_def.h pb_mdu_def.h pic_mdu_def.h pic_viewer_mdu_def.h record_mdu_def.h set_mdu_def.h sms_mdu_def.h stk_mdu_def.h \
 tp_mdu_def.h vt_mdu_def.h mobiletv_mdu_def.h browser_mdu_def.h sample_mdu_def.h otapush_mdu_def.h\
 dcd_mdu_def.h cs_mdu_def.h www_mdu_def.h dl_mdu_def.h dm_mdu_def.h pim_mdu_def.h qq_mdu_def.h clipbrd_mdu_def.h search_mdu_def.h  asp_mdu_def.h\
 widget_mdu_def.h atv_mdu_def.h memo_mdu_def.h browser_manager_mdu_def.h sns_mdu_def.h

ifneq ($(strip $(JAVA_SUPPORT)), NONE)
SOURCES	 += java_mdu_def.h 
endif

ifeq ($(strip $(PUSH_EMAIL_SUPPORT)), TRUE)
SOURCES	 += push_mail_mdu_def.h
endif

ifneq ($(strip $(WIFI_SUPPORT)), NONE)
SOURCES	 += wifi_mdu_def.h
endif

ifeq ($(strip $(QBTHEME_SUPPORT)), TRUE)
SOURCES	 += qbtheme_mdu_def.h
endif

ifeq ($(strip $(WRE_SUPPORT)), TRUE)
SOURCES	 += wre_boot_mdu_def.h
endif

ifeq ($(strip $(IKEYBACKUP_SUPPORT)), TRUE)
SOURCES	 += ikeybackup_mdu_def.h
endif

ifeq ($(strip $(WEATHER_SUPPORT)), TRUE)
SOURCES	 += weather_mdu_def.h
endif

ifeq ($(strip $(KING_MOVIE_SUPPORT)), ALL)
SOURCES	 += kmovie_mdu_def.h
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), EXTEND)
SOURCES	 += kmovie_mdu_def.h
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), STAND)
SOURCES	 += kmovie_mdu_def.h
endif
endif
endif

ifeq ($(strip $(PIC_CROP_SUPPORT)), TRUE)
SOURCES	 += pic_crop_mdu_def.h
SRCPATH  += MS_MMI/source/mmi_app/app/pic_crop/h
endif

ifeq ($(strip $(PDA_UI_DROPDOWN_WIN)), TRUE) 
SOURCES	 += dropdownwin_mdu_def.h
endif

ifeq ($(strip $(CSC_SUPPORT)),TRUE)
SOURCES	 += customer_mdu_def.h
endif
ifeq ($(strip $(MMIEMAIL_SUPPORT)),TRUE)
SOURCES	 += email_mdu_def.h
endif 
ifeq ($(strip $(SXH_APPLE_SUPPORT)),TRUE)
	MCFLAG_OPT		+= -DSXH_APPLE_SUPPORT
#	MCFLAG_OPT		+= -DSXH_APPLE_3GVEDIO_SUPPORT
	SOURCES		+= apple_mdu_def.h
endif
PROJECTS = mmi_res_prj_def.h

ifeq ($(strip $(VIDEO_PLAYER_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/videoplayer/h
#else
#SRCPATH += MS_MMI/source/mmi_app/app/mpeg4/h
endif

ifeq ($(strip $(JAVA_SUPPORT)), MYRIAD)
SRCPATH += MS_MMI/source/mmi_app/app/myriad_java/mmijava/h
endif

ifeq ($(strip $(JAVA_SUPPORT)), IA)
SRCPATH += MS_MMI/source/mmi_app/app/ia_java/h/jas
endif

ifeq ($(strip $(JAVA_SUPPORT)), SUN)
SRCPATH += MS_MMI/source/mmi_app/app/java/h
endif

ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
ifeq ($(strip $(MOBILE_VIDEO_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/mobile_video/h
endif
endif

ifeq ($(strip $(MCARE_V31_SUPPORT)),TRUE)
SOURCES	 += TencentMcareV31_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/TencentMcareV31/inc

endif

SRCPATH += MS_MMI/source/mmi_app/app/mms/h MS_MMI/source/mmi_app/app/audio/h MS_MMI/source/mmi_app/app/accessory/h \
 MS_MMI/source/mmi_app/app/cc/h MS_MMI/source/mmi_app/app/cl/h MS_MMI/source/mmi_app/common/h MS_MMI/source/mmi_app/app/ussd/h\
 MS_MMI/source/mmi_app/app/menu/h MS_MMI/source/mmi_app/app/multim/h MS_MMI/source/mmi_app/app/pb/h \
 MS_MMI/source/mmi_app/app/phone/h MS_MMI/source/mmi_app/app/setting/h MS_MMI/source/mmi_app/app/sms/h \
 MS_MMI/source/mmi_app/app/ss/h MS_MMI/source/mmi_app/app/stk/h \ MS_MMI/source/mmi_app/app/www/h \
 MS_MMI/source/mmi_app/app/eng/h MS_MMI/source/mmi_app/app/mb/h \ MS_MMI/source/mmi_app/app/dl/h \
 MS_MMI/source/mmi_app/app/envset/h MS_MMI/source/mmi_app/app/msd/h  \
 MS_MMI/source/mmi_app/app/udisk/h MS_MMI/source/mmi_app/app/audioplayer/h MS_MMI/source/mmi_app/app/record/h \
 MS_MMI/source/mmi_app/app/fm/h \
 MS_MMI/source/mmi_app/app/ebook/h MS_MMI/source/mmi_app/app/game/game_boxman/h MS_MMI/source/mmi_app/app/camera/h \
 MS_MMI/source/mmi_app/app/game/game_link/h MS_MMI/source/mmi_app/app/game/game_manager/h \
 MS_MMI/source/mmi_app/app/game/game_quintet/h MS_MMI/source/mmi_app/app/game/game_square/h \
 MS_MMI/source/mmi_app/app/idle/h MS_MMI/source/mmi_app/app/connection/h \
 MS_MMI/source/mmi_app/app/im/h MS_MMI/source/mmi_app/app/vt/h MS_MMI/source/mmi_app/app/engtd/h \
 MS_MMI/source/mmi_app/app/block/h MS_MMI/source/mmi_app/app/pic/h \
 MS_MMI/source/mmi_app/app/vcard/h MS_MMI/source/mmi_app/app/fmm/h \
 MS_MMI/source/mmi_app/app/drm/h \
 MS_MMI/source/mmi_app/app/im/im_cstar/h \
 MS_MMI/source/mmi_app/app/im/im_t9/h \
 MS_MMI/source/mmi_app/app/gps/h \
 MS_MMI/source/mmi_app/app/tp/h MS_MMI/source/mmi_app/app/ocr/h MS_MMI/source/mmi_app/app/dynamic/h \
 MS_MMI/source/mmi_app/app/bt/h \
 MS_MMI/source/mmi_app/app/theme/h \
 MS_MMI/source/mmi_app/app/kuro/h \
 MS_MMI/source/mmi_app/app/pic_viewer/h \
 MS_MMI/source/mmi_app/app/mobiletv/h \
 MS_MMI/source/mmi_app/app/browser/h \
 MS_MMI/source/mmi_app/app/dcd/h \
 MS_MMI/source/mmi_app/app/cs/h \
 MS_MMI/source/mmi_app/app/sample/h \
 MS_MMI/source/mmi_app/app/otapush/h \
 MS_MMI/source/mmi_app/app/dm/h \
 MS_MMI/source/mmi_app/app/pim/h \
 MS_MMI/source/mmi_app/app/clipbrd/h \
 MS_MMI/source/mmi_app/app/asp/h \
 MS_MMI/source/pim/h \
 MS_MMI/source/mmi_app/app/search/h \
 MS_MMI/source/mmi_app/app/qq/h \
 MS_MMI/source/mmi_app/app/atv/h \
 MS_MMI/source/mmi_app/app/widget/h \
 MS_MMI/source/mmi_app/app/memo/h\
 MS_MMI/source/mmi_app/app/browser_manager/h\
 MS_MMI/source/mmi_app/app/sns/h\
 MS_MMI/source/resource \
 MS_MMI/source/resource/$(MMI_RES_DIR)

ifeq ($(strip $(PUSH_EMAIL_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/push_mail/h
endif

ifeq ($(strip $(KING_MOVIE_SUPPORT)), ALL)
SRCPATH += MS_MMI/source/mmi_app/app/kmovie/h
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), EXTEND)
SRCPATH += MS_MMI/source/mmi_app/app/kmovie/h
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), STAND)
SRCPATH += MS_MMI/source/mmi_app/app/kmovie/h
endif
endif
endif

ifeq ($(strip $(WEATHER_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/weather/h
endif

ifeq ($(strip $(PDA_UI_DROPDOWN_WIN)), TRUE) 
SRCPATH += MS_MMI/source/mmi_app/app/dropdownwin/h
endif
#ifeq ($(strip $(PDA_UI_SUPPORT)), ANDROID)


ifneq ($(strip $(WIFI_SUPPORT)), NONE)
SRCPATH += MS_MMI/source/mmi_app/app/wlan/h
endif

ifeq ($(strip $(QBTHEME_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/qbtheme/h
endif
ifeq ($(strip $(MET_MEX_SUPPORT)), TRUE)
SOURCES	 += met_mex_mdu_def.h
SRCPATH  += MS_MMI/source/mmi_app/app/met_mex/h
endif

ifeq ($(strip $(WRE_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/wre_boot/h
endif

ifeq ($(strip $(IKEYBACKUP_SUPPORT)), TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/ikeybackup/h
endif

SRCPATH += ms_mmi\source\mmi_app\app\wallpaper\h
SOURCES	 += wallpaper_mdu_def.h

ifeq ($(strip $(LIVE_WALLPAPER_FRAMEWORK_SUPPORT)), TRUE)
SRCPATH += ms_mmi\source\mmi_app\app\livewallpaper\h
SOURCES	 += livewallpaper_mdu_def.h
endif
#add by zack@20110117 for sky start
ifeq ($(strip $(MRAPP_SUPPORT)), TRUE)
SOURCES += mrapp_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/mrapp/h

ifeq ($(strip $(__MMI_SKYQQ__)), TRUE)
SOURCES += skyqq_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/skyqq/h
endif
endif
#add by zack@20110117 for sky end
ifeq ($(strip $(DMENU_SUPPORT)), TRUE)
SOURCES += dmenu_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/dmenu/h
endif

ifeq ($(strip $(MMIEMAIL_SUPPORT)),TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/email/h
endif

ifeq ($(strip $(OPERA_MINI_SUPPORT)), VER42)
SOURCES	 += operamini_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/operamini4
endif
ifeq ($(strip $(OPERA_MINI_SUPPORT)), VER6)
SOURCES	 += operamini_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/operamini6
endif
ifeq ($(strip $(OPERA_MOBILE_SUPPORT)), V11)
SOURCES	 += operamobile_mdu_def.h
SRCPATH += MS_MMI/source/mmi_app/app/operamobile
endif


ifeq ($(strip $(CSC_SUPPORT)),TRUE)
SRCPATH += MS_MMI/source/mmi_app/app/customer/h
endif 

ifneq ($(strip $(MMIUZONE_SUPPORT)), NONE)
SOURCES	 += uzone_mdu_def.h 
SRCPATH += MS_MMI/source/mmi_app/app/uzone/h
endif

ifeq ($(strip $(SXH_APPLE_SUPPORT)),TRUE)
	SRCPATH	+= MS_MMI/source/mmi_app/app/apple/h
endif
#
#  make process ,don't modify
#

vpath
vpath %.h        $(subst \,/,$(SRCPATH))

SOURCE_OBJ	= $(patsubst %.h, %.source, $(SOURCES))
PROJECT_OBJ	= $(patsubst %.h, %.project, $(PROJECTS))
SOURCE_DIR	= $(patsubst %_mdu_def.h,%, $(SOURCES))
ROOT_DIR	= $(shell $(PERL) ./make/perl_script/get_root_fullpath.pl)

