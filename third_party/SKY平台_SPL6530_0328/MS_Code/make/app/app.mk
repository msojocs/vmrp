include make/app/app_macro.mk
include Makefile_App.mk

#--------------------------------------------SRC & INC-----------------------------------------------------------------#

MINCPATH = MS_MMI/source/mmi_app/app/accessory/h
MINCPATH += MS_MMI/source/mmi_app/app/audio/h 
MINCPATH += MS_MMI/source/mmi_app/app/bt/h 
MINCPATH += MS_MMI/source/mmi_app/app/camera/h 
MINCPATH += MS_MMI/source/mmi_app/app/cc/h 
MINCPATH += MS_MMI/source/mmi_app/app/cl/h 
MINCPATH += MS_MMI/source/mmi_app/app/connection/h 
MINCPATH += MS_MMI/source/mmi_app/app/cs/h 
MINCPATH += MS_MMI/source/mmi_app/app/dcd/h 
MINCPATH += MS_MMI/source/mmi_app/app/dl/h 
MINCPATH += MS_MMI/source/mmi_app/app/dm/h 
MINCPATH += MS_MMI/source/mmi_app/app/dynamic/h 
MINCPATH += MS_MMI/source/mmi_app/app/ebook/h 
MINCPATH += MS_MMI/source/mmi_app/app/eng/h 
MINCPATH += MS_MMI/source/mmi_app/app/engtd/h 
MINCPATH += MS_MMI/source/mmi_app/app/envset/h 
MINCPATH += MS_MMI/source/mmi_app/app/fdn/h
MINCPATH += MS_MMI/source/mmi_app/app/fm/h 
MINCPATH += MS_MMI/source/mmi_app/app/fmm/h
MINCPATH += MS_MMI/source/mmi_app/app/ussd/h
MINCPATH += MS_MMI/source/mmi_app/app/parse/h

ifneq ($(strip $(DRM_SUPPORT)),)  
    ifneq ($(strip $(DRM_SUPPORT)), NONE)
    MINCPATH += MS_MMI/source/mmi_app/app/drm/h 
    endif
endif 

MINCPATH += MS_MMI/source/mmi_app/app/game/game_boxman/h 
MINCPATH += MS_MMI/source/mmi_app/app/game/game_quintet/h 
MINCPATH += MS_MMI/source/mmi_app/app/game/game_link/h 
MINCPATH += MS_MMI/source/mmi_app/app/game/game_manager/h 
MINCPATH += MS_MMI/source/mmi_app/app/screensaver/h 
MINCPATH += MS_MMI/source/mmi_app/app/keylock/h 
MINCPATH += MS_MMI/source/mmi_app/app/wallpaper/h 
MINCPATH += MS_MMI/source/mmi_app/app/taskmanager/h 
MINCPATH += MS_MMI/source/mmi_app/app/idle/h 
MINCPATH += MS_MMI/source/mmi_app/app/im/h 
MINCPATH += MS_MMI/source/mmi_app/app/menu/h 
MINCPATH += MS_MMI/source/mmi_app/app/mmi3d/h 
MINCPATH += MS_MMI/source/mmi_app/app/mms/h 
MINCPATH += MS_MMI/source/mmi_app/app/mobiletv/h
MINCPATH += MS_MMI/source/mmi_app/app/atv/h 
MINCPATH += MS_MMI/source/mmi_app/app/audioplayer/h 
MINCPATH += MS_MMI/source/mmi_app/app/msd/h 
MINCPATH += MS_MMI/source/mmi_app/app/multim/h 
MINCPATH += MS_MMI/source/mmi_app/app/ocr/h 
MINCPATH += MS_MMI/source/mmi_app/app/otapush/h  
MINCPATH += MS_MMI/source/mmi_app/app/pb/h 
MINCPATH += MS_MMI/source/mmi_app/app/pdp/h 
MINCPATH += MS_MMI/source/mmi_app/app/phone/h 
MINCPATH += MS_MMI/source/mmi_app/app/pic/h 
MINCPATH += MS_MMI/source/mmi_app/app/pic_viewer/h 
MINCPATH += MS_MMI/source/mmi_app/app/pim/h
MINCPATH += MS_MMI/source/mmi_app/app/record/h 
MINCPATH += MS_MMI/source/mmi_app/app/setting/h 
MINCPATH += MS_MMI/source/mmi_app/app/sms/h 
MINCPATH += MS_MMI/source/mmi_app/app/ss/h 
MINCPATH += MS_MMI/source/mmi_app/app/stk/h 
MINCPATH += MS_MMI/source/mmi_app/app/printscreen/h 
MINCPATH += MS_MMI/source/mmi_app/app/takescreenphoto/h 
#MINCPATH += MS_MMI/source/mmi_app/app/streaming/h 
MINCPATH += MS_MMI/source/mmi_app/app/theme/h 
MINCPATH += MS_MMI/source/mmi_app/app/tp/h 
MINCPATH += MS_MMI/source/mmi_app/app/udisk/h 
MINCPATH += MS_MMI/source/mmi_app/app/vcard/h 
MINCPATH += MS_MMI/source/mmi_app/app/vt/h 
MINCPATH += MS_MMI/source/mmi_app/app/clipbrd/h 
MINCPATH += MS_MMI/source/mmi_app/app/search/h
MINCPATH += MS_MMI/source/mmi_app/common/h 
MINCPATH += MS_MMI/source/mmi_app/custom/h 
MINCPATH += MS_MMI/source/mmi_app/kernel/h 
MINCPATH += MS_MMI/source/mmi_gui/include 
MINCPATH += MS_MMI/source/mmi_kernel/include 
MINCPATH += MS_MMI/source/mmi_spml/include
MINCPATH += MS_MMI/source/mmi_utility/h 
MINCPATH += MS_MMI/source/spim/include
MINCPATH += MS_MMI/source/pim/h
MINCPATH += MS_MMI/source/resource
MINCPATH += chip_drv/export/inc/outdated 
MINCPATH += CAF/Interface/source/resource 
MINCPATH += CAF/Template/include 
MINCPATH += DAPS/source/wbxml_parser/inc 
MINCPATH += DAPS/source/wsp_header_decoder/inc
MINCPATH += DAPS/source/xml_expat/inc 
MINCPATH += Base/l4/export/inc 
MINCPATH += PARSER/export/inc 
MINCPATH += Base/sim/export/inc 
MINCPATH += Third-party/cstar/include 
ifeq ($(strip $(IM_ENGINE)), SOGOU)
MINCPATH += Third-party/sogou/include 
endif

MINCPATH += external/juniversalchardet/include
MINCPATH += external/juniversalchardet/c/include

ifeq ($(strip $(IKEYBACKUP_SUPPORT)),TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/ikeybackup/h
endif

ifeq ($(strip $(WRE_SUPPORT)),TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/wre_boot/h
MINCPATH += Third-party\zlib\include
ifeq ($(strip $(NES_SUPPORT)),TRUE)
MINCPATH += nes/inc
MINCPATH += nes
endif
endif

#API文件
#MINCPATH += CMMB/source/mbbms/inc
MINCPATH += MS_MMI/source/mmi_app/app/kuro/h 

#ifeq ($(strip $(MMIWIDGET_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/widget/h 
#endif

ifeq ($(strip $(MMI_GRID_IDLE_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/widget/h 
endif

ifeq ($(strip $(QBTHEME_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/widget/qbtheme/h
endif

ifeq ($(strip $(UI_P3D_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/winse/h 
endif

ifeq ($(strip $(CMMB_SUPPORT)), TRUE)
MINCPATH += CMMB/export/inc 
endif

ifeq ($(strip $(TTS_SUPPORT)), IFLYTEK)
MINCPATH += Third-party/InterSound4/h
MINCPATH += Third-party/InterSound4/Lib
MINCPATH += MS_MMI/source/mmi_app/app/tts/h
MINCPATH += MS_MMI/source/mmi_app/app/tts/iflytek/h
endif

ifeq ($(strip $(TTS_SUPPORT)), SHYOUNGTONE)
MINCPATH += Third-party/youngtone/h
MINCPATH += Third-party/youngtone/lib
MINCPATH += MS_MMI/source/mmi_app/app/tts/h
MINCPATH += MS_MMI/source/mmi_app/app/tts/youngtong/h
endif

ifeq ($(strip $(TTS_SUPPORT)), EJ)
MINCPATH += Third-party/ejTTS/h
MINCPATH += Third-party/ejTTS/LE_Lib
MINCPATH += MS_MMI/source/mmi_app/app/tts/h
MINCPATH += MS_MMI/source/mmi_app/app/tts/ej/h
endif

ifeq ($(strip $(CARD_LOG)), TRUE)
MINCPATH += Base/logsave/export/inc
endif

ifeq ($(strip $(DEV_MANAGE_SUPPORT)), TRUE)
ifeq ($(strip $(SPRD_CUSTOMER)), FALSE)    
MINCPATH += Third-party/formal/Dm/import 
MINCPATH += Third-party/formal/Dm/include 
else
MINCPATH += Third-party/Dm/import 
MINCPATH += Third-party/Dm/include 
endif
endif

ifeq ($(strip $(BROWSER_SUPPORT_NETFRONT)), TRUE)
    ifeq ($(strip $(NETFRONT_SUPPORT)), VER35)
        ifeq ($(strip $(SPRD_CUSTOMER)), FALSE)    
            MINCPATH += Third-party/formal/netfront/Netfront3.5/include 
            MINCPATH += Third-party/formal/netfront/Netfront3.5/include/config
        else
            MINCPATH += Third-party/netfront/Netfront3.5/include 
            MINCPATH += Third-party/netfront/Netfront3.5/include/config
        endif
    else
        ifeq ($(strip $(NETFRONT_SUPPORT)), VER40)
            ifeq ($(strip $(SPRD_CUSTOMER)), FALSE)    
                MINCPATH += Third-party/formal/netfront/Netfront4.0/include 
                MINCPATH += Third-party/formal/netfront/Netfront4.0/include/config
            else
                MINCPATH += Third-party/netfront/Netfront4.0/include 
                MINCPATH += Third-party/netfront/Netfront4.0/include/config
            endif
        else
            ifeq ($(strip $(SPRD_CUSTOMER)), FALSE)    
                MINCPATH += Third-party/formal/netfront/Netfront4.2/include 
                MINCPATH += Third-party/formal/netfront/Netfront4.2/include/config
            else
                MINCPATH += Third-party/netfront/Netfront4.2/include 
                MINCPATH += Third-party/netfront/Netfront4.2/include/config
            endif
        endif
    endif
endif

ifeq ($(strip $(VT_SUPPORT)), TRUE)
ifeq ($(strip $(SPRD_CUSTOMER)), FALSE)    
MINCPATH += Third-party/formal/Mplapi/include 
MINCPATH += Third-party/formal/Mplapi/Arm_lib/normal
else
MINCPATH += Third-party/Mplapi/include 
MINCPATH += Third-party/Mplapi/Arm_lib/normal
endif
endif


#sun java api              
ifeq ($(strip $(JAVA_SUPPORT)), SUN)
MINCPATH        +=      MS_MMI/source/mmi_app/app/java/h\
			MS_MMI/source/sun_java/h \
                        MS_MMI/source/sun_java/h/javacall/midp \
                        MS_MMI/source/sun_java/h/javacall/common \
                        MS_MMI/source/sun_java/h/javacall \
                        MS_MMI/source/sun_java/h/javacall/jsr135_mmapi \
                        MS_MMI/source/sun_java/h/javacall/jsr120_wma \
                        MS_MMI/source/sun_java/h/platform \
                        MS_MMI/source/sun_java/h/javacall/jsr75_pim_fc\
                        MS_MMI/source/sun_java/c/javacall/midp
endif

ifeq ($(strip $(JAVA_SUPPORT)), IA)
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/jblend
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/jblend/gui/common
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/jblend/gui/inner
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/jblend/gui/user
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/extlib
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/ajsc
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/port
MINCPATH += MS_MMI/source/mmi_app/app/ia_java/h/jas
endif

ifeq ($(strip $(JAVA_SUPPORT)), MYRIAD)
ifeq ($(strip $(MODULES)), simulator)
	ifneq ($(strip $(LOW_MEMORY_SUPPORT)), NONE)
	MINCPATH += MS_MMI/source/mmi_app/app/myriad_java/jbed/h/svm_sim
	else
	MINCPATH += MS_MMI/source/mmi_app/app/myriad_java/jbed/h/sim
	endif
else
	ifneq ($(strip $(LOW_MEMORY_SUPPORT)), NONE)
	MINCPATH += MS_MMI/source/mmi_app/app/myriad_java/jbed/h/svm_tgt
	else
	MINCPATH += MS_MMI/source/mmi_app/app/myriad_java/jbed/h/tgt
	endif
endif

MINCPATH += MS_MMI/source/mmi_app/app/myriad_java/jbed/h
MINCPATH += MS_MMI/source/mmi_app/app/myriad_java/mmijava/h
endif

ifeq ($(strip $(SPRD_CUSTOMER)), FALSE)
    ifeq ($(strip $(PLATFORM)), SC8800H)
	MINCPATH += Third-party/formal/hw/include
    else
    ifeq ($(strip $(PLATFORM)), SC8800G)
    MINCPATH += Third-party/formal/hw/include
    else
       ifeq ($(strip $(IM_HANDWRITING)), HANWANG)
	MINCPATH += Third-party/hw/include 
	endif
    endif
   endif
else
  ifeq ($(strip $(IM_HANDWRITING)), HANWANG)
	MINCPATH += Third-party/hw/include 
  endif
endif
				
ifeq ($(strip $(BROWSER_SUPPORT_NETFRONT)), TRUE)
 MINCPATH += MS_MMI/source/mmi_app/app/www/h
endif

ifeq ($(strip $(VT_SUPPORT)), TRUE)
 MINCPATH += MS_MMI/source/mmi_app/app/vt/h
endif

ifeq ($(strip $(BROWSER_SUPPORT)), TRUE)
 MINCPATH += MS_MMI/source/mmi_app/app/browser_manager/h
endif

ifeq ($(strip $(BROWSER_SUPPORT_DORADO)), TRUE)
 MINCPATH += MS_MMI/source/mmi_app/app/browser/h
 MINCPATH += MS_MMI/source/mmi_app/app/browser/control/inc
endif
ifeq ($(strip $(SNS_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/sns/h
endif


ifeq ($(strip $(MMI_MOCOR_SAMPLE)), TRUE)
 MINCPATH		+=  MS_MMI/source/mmi_app/app/sample/h
endif

#ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
# MINCPATH +=    MS_MMI/source/mmi_app/app/streaming/h/nex_inc \
#                MS_MMI/source/mmi_app/app/streaming/h/nex_inc/nxsys
#endif

ifeq ($(strip $(OCR_SUPPORT)), TRUE)
 MINCPATH += Third-party/ocr/include
endif

ifeq ($(strip $(GPS_SUPPORT)), TRUE)
 MINCPATH += MS_MMI/source/mmi_app/app/gps/h
endif

ifeq ($(strip $(VIDEO_PLAYER_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/videoplayer/h
endif

ifeq ($(strip $(KING_MOVIE_SUPPORT)), ALL)
MINCPATH += MS_MMI/source/mmi_app/app/kmovie/h
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), EXTEND)
MINCPATH += MS_MMI/source/mmi_app/app/kmovie/h
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), STAND)
MINCPATH += MS_MMI/source/mmi_app/app/kmovie/h
endif
endif
endif


ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
ifeq ($(strip $(MOBILE_VIDEO_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/mobile_video/h \
            MS_MMI/source/mmi_app/app/mobile_video/h/include \
            MS_MMI/source/mmi_app/app/mobile_video/h/mv
endif
endif

ifeq ($(strip $(ASP_SUPPORT)), TRUE)
 MINCPATH += MS_MMI/source/mmi_app/app/asp/h
endif

ifeq ($(strip $(PUSH_EMAIL_SUPPORT)), TRUE)
MINCPATH  +=  MS_MMI/source/mmi_app/app/push_mail/h
MINCPATH  +=  MS_MMI/source/push_mail/comm/h
MINCPATH  +=  MS_MMI/source/push_mail/push_mail/h
endif

ifeq ($(strip $(CSC_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/customer/h
endif 

MINCPATH += $(PS_INCPATH) $(L1_INCPATH) $(ATC_INCPATH)						

ifeq ($(strip $(TF_LOAD_SUPPORT)), TRUE)
MINCPATH += fdl_bootloader/tf_fdl/inc
endif
             
MSRCPATH = MS_MMI/source/mmi_app 
MSRCPATH += MS_MMI/source/mmi_app/kernel/c 
MSRCPATH += MS_MMI/source/mmi_app/common/c 
MSRCPATH += MS_MMI/source/mmi_app/app/theme/c 
MSRCPATH += MS_MMI/source/mmi_app/app/accessory/c 
MSRCPATH += MS_MMI/source/mmi_app/app/audio/c 
MSRCPATH += MS_MMI/source/mmi_app/app/cc/c 
MSRCPATH += MS_MMI/source/mmi_app/app/cl/c 
MSRCPATH += MS_MMI/source/mmi_app/app/connection/c 
MSRCPATH += MS_MMI/source/mmi_app/app/ebook/c 
MSRCPATH += MS_MMI/source/mmi_app/app/eng/c 
MSRCPATH += MS_MMI/source/mmi_app/app/envset/c 
MSRCPATH += MS_MMI/source/mmi_app/app/fm/c 
MSRCPATH += MS_MMI/source/mmi_app/app/fmm/c 
ifneq ($(strip $(DRM_SUPPORT)),)  
    ifneq ($(strip $(DRM_SUPPORT)), NONE)
    MSRCPATH += MS_MMI/source/mmi_app/app/drm/c
    endif
endif
MSRCPATH += MS_MMI/source/mmi_app/app/game/game_boxman/c 
MSRCPATH += MS_MMI/source/mmi_app/app/game/game_quintet/c
MSRCPATH += MS_MMI/source/mmi_app/app/game/game_link/c 
MSRCPATH += MS_MMI/source/mmi_app/app/game/game_manager/c 
MSRCPATH += MS_MMI/source/mmi_app/app/screensaver/c
MSRCPATH += MS_MMI/source/mmi_app/app/keylock/c 
MSRCPATH += MS_MMI/source/mmi_app/app/wallpaper/c 
MSRCPATH += MS_MMI/source/mmi_app/app/taskmanager/c 
MSRCPATH += MS_MMI/source/mmi_app/app/idle/c  
MSRCPATH += MS_MMI/source/mmi_app/app/menu/c 
MSRCPATH += MS_MMI/source/mmi_app/app/mms/c  
MSRCPATH += MS_MMI/source/mmi_app/app/otapush/c 
MSRCPATH += MS_MMI/source/mmi_app/app/msd/c 
MSRCPATH += MS_MMI/source/mmi_app/app/pb/c 
MSRCPATH += MS_MMI/source/mmi_app/app/fdn/c
MSRCPATH += MS_MMI/source/mmi_app/app/phone/c 
MSRCPATH += MS_MMI/source/mmi_app/app/pic/c 
MSRCPATH += MS_MMI/source/mmi_app/app/pic_viewer/c
MSRCPATH += MS_MMI/source/mmi_app/app/record/c 
MSRCPATH += MS_MMI/source/mmi_app/app/setting/c 
MSRCPATH += MS_MMI/source/mmi_app/app/sms/c 
MSRCPATH += MS_MMI/source/mmi_app/app/stk/c 
MSRCPATH += MS_MMI/source/mmi_app/app/printscreen/c 
MSRCPATH += MS_MMI/source/mmi_app/app/takescreenphoto/c 
MSRCPATH += MS_MMI/source/mmi_app/app/udisk/c 
MSRCPATH += MS_MMI/source/mmi_app/app/vcard/c 
MSRCPATH += MS_MMI/source/mmi_app/app/vt/c 
MSRCPATH += MS_MMI/source/mmi_app/app/engtd/c 
MSRCPATH += MS_MMI/source/mmi_app/app/audioplayer/c 
#MSRCPATH += MS_MMI/source/mmi_app/app/streaming/c 
MSRCPATH += MS_MMI/source/mmi_app/app/tp/c 
MSRCPATH += MS_MMI/source/mmi_app/app/bt/c 
MSRCPATH += MS_MMI/source/mmi_app/app/pdp/c 
MSRCPATH += MS_MMI/source/mmi_app/app/dcd/c 
MSRCPATH += MS_MMI/source/mmi_app/app/cs/c 
MSRCPATH += MS_MMI/source/mmi_app/app/mmi3d/c 
MSRCPATH += MS_MMI/source/mmi_app/app/ocr/c 
MSRCPATH += MS_MMI/source/mmi_app/app/dynamic/c 
MSRCPATH += MS_MMI/source/mmi_app/app/im/c 
MSRCPATH += MS_MMI/source/mmi_app/app/dm/c 
MSRCPATH += MS_MMI/source/mmi_app/app/camera/c 
MSRCPATH += MS_MMI/source/mmi_app/app/mobiletv/c
MSRCPATH += MS_MMI/source/mmi_app/app/atv/c 
MSRCPATH += MS_MMI/source/mmi_app/app/www/c 
MSRCPATH += MS_MMI/source/mmi_app/app/dl/c 
MSRCPATH += MS_MMI/source/mmi_app/app/ussd/c 
MSRCPATH += MS_MMI/source/mmi_app/app/parse/c 

ifeq ($(strip $(WRE_SUPPORT)),TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/wre_boot/c
endif

ifeq ($(strip $(IKEYBACKUP_SUPPORT)),TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/ikeybackup/c
endif

ifeq ($(strip $(JAVA_SUPPORT)), SUN)
MSRCPATH += MS_MMI/source/mmi_app/app/java/c
endif
MSRCPATH += MS_MMI/source/mmi_app/app/pim/c
ifeq ($(strip $(BROWSER_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/browser_manager/c 
endif
ifeq ($(strip $(BROWSER_SUPPORT_DORADO)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/browser/c 
MSRCPATH += MS_MMI/source/mmi_app/app/browser/control/src
endif
ifeq ($(strip $(SNS_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/sns/c 
endif

MSRCPATH += MS_MMI/source/mmi_app/app/clipbrd/c 
MSRCPATH += MS_MMI/source/mmi_app/app/search/c 

ifeq ($(strip $(JAVA_SUPPORT)), IA)
MSRCPATH += MS_MMI/source/mmi_app/app/ia_java/c/port
MSRCPATH += MS_MMI/source/mmi_app/app/ia_java/c/port/ksc
MSRCPATH += MS_MMI/source/mmi_app/app/ia_java/c/port/jsc
MSRCPATH += MS_MMI/source/mmi_app/app/ia_java/c/jas
endif

ifeq ($(strip $(JAVA_SUPPORT)), MYRIAD)
MSRCPATH += MS_MMI/source/mmi_app/app/myriad_java/jbed/c
MSRCPATH += MS_MMI/source/mmi_app/app/myriad_java/mmijava/c
endif

ifeq ($(strip $(SQLITE_SUPPORT)), TRUE)
MINCPATH += external/libc
MINCPATH += external/sqlite
endif

#API文件
MSRCPATH += MS_MMI/source/mmi_app/app/kuro/c 

ifeq ($(strip $(GPS_SUPPORT)), TRUE)
MSRCPATH		+=  MS_MMI/source/mmi_app/app/gps/c 
endif

ifeq ($(strip $(ASP_SUPPORT)), TRUE)
MSRCPATH		+=  MS_MMI/source/mmi_app/app/asp/c 
endif

ifeq ($(strip $(MMI_MOCOR_SAMPLE)), TRUE)
MSRCPATH		+=  MS_MMI/source/mmi_app/app/sample/c
endif

ifeq ($(strip $(TTS_SUPPORT)), IFLYTEK)
MSRCPATH += MS_MMI/source/mmi_app/app/tts/c
MSRCPATH += MS_MMI/source/mmi_app/app/tts/iflytek/c
endif

ifeq ($(strip $(TTS_SUPPORT)), SHYOUNGTONE)
MSRCPATH += MS_MMI/source/mmi_app/app/tts/c
MSRCPATH += MS_MMI/source/mmi_app/app/tts/youngtong/c
endif

ifeq ($(strip $(TTS_SUPPORT)), EJ)
MSRCPATH += MS_MMI/source/mmi_app/app/tts/c
MSRCPATH += MS_MMI/source/mmi_app/app/tts/ej/c
endif

ifeq ($(strip $(VIDEO_PLAYER_SUPPORT)), TRUE)
 MSRCPATH += MS_MMI/source/mmi_app/app/videoplayer/c
endif

ifeq ($(strip $(KING_MOVIE_SUPPORT)), ALL)
MSRCPATH += MS_MMI/source/mmi_app/app/kmovie/c
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), EXTEND)
MSRCPATH += MS_MMI/source/mmi_app/app/kmovie/c
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), STAND)
MSRCPATH += MS_MMI/source/mmi_app/app/kmovie/c
endif
endif
endif


ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
ifeq ($(strip $(MOBILE_VIDEO_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/mobile_video/c \
            MS_MMI/source/mmi_app/app/mobile_video/c/mv
endif
endif

ifeq ($(strip $(MMIWIDGET_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/widget/c
endif

ifeq ($(strip $(MMI_GRID_IDLE_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/widget/c
endif

ifeq ($(strip $(QBTHEME_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/widget/qbtheme/c
endif
ifeq ($(strip $(UI_P3D_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/winse/c 
endif
ifeq ($(strip $(PUSH_EMAIL_SUPPORT)), TRUE)
MSRCPATH  +=  MS_MMI/source/mmi_app/app/push_mail/c
endif

ifeq ($(strip $(CSC_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/customer/c
endif 

MINCPATH +=  build/$(PROJECT)_builddir/tmp  


SOURCES	       = app_tasks.c block_mem.c mmi_atc.c mmi_atc_wintab.c mmi_common.c mmi_filetask.c mmi_modu_main.c mmi_osbridge.c mmi_port.c mmi_crc16.c\
                mmi_rglexpr.c mmimain.c mmi_nv.c mmi_default.c mmi_menutable.c \
                mmiparse_dial.c mmidisplay_data.c mmi_applet_table.c\
                mmi_solarlunar.c mmiacc_winmain.c mmicalc_main.c mmiwclk_main.c mmialarm.c mmialarm_service.c mmicalendar_main.c mmiacc_menutable.c \
                mmialarm_menutable.c mmischedule.c mmiacc_nv.c mmialarm_nv.c  mmischedule_atc.c  mmialarm_clock.c mmialarm_jump_clock.c\
                mmiaudio.c mmiaudio_ctrl.c mmiaudio_vibrate.c mmicc_app.c mmicc_audio.c mmicc_nv.c mmicc_speeddial_wintab.c mmiussd.c mmicc_menutable.c mmicc_speeddial.c mmicc_wintab.c mmicc_main.c\
                mmicl.c mmicl_costs_wintab.c mmicl_wintab.c mmicl_ldn.c mmikl_keylock.c mainapp.c mmiidle_hwicon.c mmi_subwintab.c window_parse.c mmi_applet_manager.c\
                mmiconn_manager_wintab.c mmiconnection_menutable.c mmiconnection.c mmitv_out.c \
                mmiebook_file.c mmiebook_nv.c mmiebook_wintab.c mmiebook_menutable.c mmiebook_task.c \
                mmiebook_grid.c mmiebook_bookshelf.c \
                mmieng_main.c mmieng_nv.c mmieng_win.c mmieng_menutable.c mmieng_uitestwin.c mmieng_base.c\
                mmieng_dynamic_main.c \
                mmienvset.c mmienvset_wintab.c \
                mmifmm.c mmifmm_menutable.c mmifmm_wintab.c mmifmm_id.c mmifmm_interface.c mmifmm_nv.c mmifmm_pic.c \
                mmigame_boxman_func.c mmigame_boxman_nv.c mmigame_boxman_menutable.c mmigame_boxman_wintab.c \
                mmigame_quintet_func.c mmigame_quintet_menutable.c mmigame_quintet_nv.c mmigame_quintet_wintab.c\
                mmigame_link_func.c mmigame_link_nv.c mmigame_link_menutable.c mmigame_link_wintab.c \
                mmigame.c mmigame_menutable.c mmigame_wintab.c \
                mainmenu_win.c mmimenu_optionpage.c mmimenu_data.c mmimenu_second.c mmimenu_popup.c mmimenu_slide_page.c mmimenu_cube.c mmimenu_rotation.c mmimenu_options.c mmimenu_synchronize.c mmimenu_anim.c\
                mmimenu_istyle.c mmiidle_istyle.c mmimenu_qbtheme.c mmimenu_internal.c\
                mmiota_file.c mmiota_id.c mmiota_main.c mmiota_menutable.c mmiota_wintable.c \
                mmimp3.c mmimp3_sort.c mmimp3_lyric.c mmimp3_music.c mmimp3_wintab.c mmimp3_bt.c mmimp3_menutable.c mmimp3_nv.c \
                mmimsd.c mmimsd_wintab.c mmi_filemgr.c \
                mmipb_atc.c  mmipb_view.c\
                mmipb_adapt.c mmipb_interface.c mmipb_storage.c mmipb_menu.c  mmipb_app.c mmipb_task.c mmipb_datalist.c  mmipb_mem.c\
		            mmipb_nv.c mmipb_search.c mmifdn.c mmipb_set.c\
                mmiphone.c mmiphone_wintab.c mmiphone_nitz.c mmiphone_network_name.c mmiphone_charge.c mmiphone_onoff.c \
                mmipicview.c mmipicview_id.c mmipicview_menutable.c mmipicview_nv.c mmipicview_list.c mmipicview_preview.c mmipicview_wintab.c  mmipicview_zoom.c \
                mmi_pubwin.c mmi_pubeditwin.c mmi_pubmenuwin.c mmi_pubtextwin.c mmi_pubrichtextwin.c mmi_publistwin.c mmi_pubformwin.c\
                mmi_setlist_win.c \
                mmirecord.c mmirecord_menutable.c mmirecord_nv.c mmirecord_wintab.c \
                mmiset_call.c mmiset_func.c mmiset_nv.c mmiset_security.c mmiset_display.c mmiset_menutable.c mmiset_ring.c mmiset_wintab.c mmiset_lock_effect.c \
                mmiset_shortcut.c mmiset_videowallpaper.c mmiset_mainmenu.c mmiset_longpower.c mmiset_wallpaper.c mmiset_datetime.c mmiset_tts.c mmiset_screensaver.c mmiset_theme.c mmiset_displaywin.c mmiset_phonewin.c mmiset_mv.c mmiset_callwin.c\
                mmisms_api.c mmisms_nv.c mmismsapp_order.c mmismscb_api.c mmisms_menutable.c mmismsapp_main.c mmismsapp_wintab.c mmismscb_wintab.c mmisms_delallwin.c mmisms_settingwin.c \
                mmisms_delete.c mmisms_edit.c mmisms_read.c mmisms_receive.c mmisms_save.c mmisms_send.c mmisms_set.c mmisms_voicemail.c mmisms_file.c mmisms_timermsg.c \
                mmistk_app.c mmistk_win.c mmitheme_anim.c mmitheme_iconfolder.c mmitheme_iconlist.c mmitheme_menu.c mmitheme_prgbox.c mmitheme_pubwin.c mmi_theme.c mmitheme_tab.c mmitheme_label.c \
                mmitheme_statusbar.c mmitheme_tips.c mmitheme_dropdownlist.c mmitheme_edit.c mmitheme_im.c mmitheme_text.c mmitheme_setlist.c mmitheme_form.c mmitheme_button.c \
                mmitheme_block.c mmitheme_pos.c mmitheme_update.c mmitheme_toolbar.c mmitheme_softkey.c mmitheme_list.c mmitheme_title.c mmitheme_richtext.c mmitheme_coverbar.c mmitheme_graph_data.c\
                mmi_sd.c mmi_sd_plug.c mmiudisk.c mmiudisk_wintab.c \
                mmivcard.c vf_create_object.c vf_modified.c vf_strings.c vf_access.c vf_delete.c vf_parser.c vf_writer.c \
                vf_access_calendar.c vf_malloc.c vf_search.c vf_access_wrappers.c vf_malloc_stdlib.c vf_string_arrays.c \
                mmipdp_api.c mmipdp_list.c mmipdp_ps_adap.c \
                coordinate_wintab.c tp_func.c tp_nv.c \
                mmibt_a2dp.c mmibt_menutable.c mmibt_nv.c mmibt_wintab.c  mmibt_app.c \
                mmi_id.c mmiacc_id.c mmibt_id.c mmicc_id.c mmiconnection_id.c mmieng_id.c mmigame_boxman_id.c mmigame_link_id.c \
                mmimp3_id.c mmipb_id.c mmirecord_id.c mmiset_id.c mmisms_id.c mmistk_id.c tp_id.c mmialarm_id.c mmiebook_id.c mmigame_id.c\
                mmimenu_icon.c mmi_wallpaper.c mmimp_location.c mmiphonesearch_wintab.c \
                mmiset_multisim.c mmicountedtime_main.c mmistopwatch_main.c mmi_timer_main.c\
                mmicom_panel.c mmicom_data.c mmicom_string.c mmicom_time.c mmicom_extract.c\
                mmitheme_special.c mmitheme_special_func.c mmitheme_layer_special.c mmitheme_frame_engine.c \
                mmiidle_shortcut.c mmiidle_dial.c\
                mmiss_screensaver.c mmiss_seed.c mmiss_snake.c mmiss_special.c mmiss_star.c \
                mmidyna_win.c mmidyna_id.c mmidyna_menutable.c mmi_resource.c mmi3d.c mmicamera_manager.c mmidv_manager.c \
                mmi_event_api.c mmi_event_manager.c \
                mmiunitconversion.c mmi_autotest.c mmiptscr_main.c mmiptscr_save.c \
                mmitheme_clipbrd.c mmiclipbrd.c mmiclipbrd_id.c mmiclipbrd_menutable.c mmiclipbrd_wintab.c mmiclipbrd_toolbar.c mmiclipbrd_slider.c mmiclipbrd_magnify.c\
                mmisearch_app.c mmisearch_searchapplication.c mmisearch_wintab.c mmisearch_main.c mmisearch_id.c mmisearch_menutable.c mmivcalendar.c mmivc_common.c\
                mmiidle_func.c mmiidle_statusbar.c mmiidle_cstyle.c mmiidle_display.c mmitakescrphoto_main.c\

ifeq ($(strip $(PDA_UI_SUPPORT_MANIMENU_GO)), TRUE)
			SOURCES	 +=     mmimenu_slide_page_go.c
endif

ifneq ($(strip $(DRM_SUPPORT)),)  
    ifneq ($(strip $(DRM_SUPPORT)), NONE)
    SOURCES	 +=     mmi_drmfilemgr.c mmidrm.c mmidrm_export.c mmidrm_nv.c mmipicview_drm.c
    endif
endif


ifeq ($(strip $(KING_MOVIE_SUPPORT)), ALL)
MINCPATH  +=  MS_Ref/source/video_security/export
MINCPATH  +=  MS_Ref/source/video_security_kmv/export
MINCPATH  +=  MS_Ref/source/video_security_hmv/export
SOURCES	 += mmikm_wintab.c mmikm_main.c mmikm_menutable.c mmikm_playlist.c
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), EXTEND)
MINCPATH  +=  MS_Ref/source/video_security/export
MINCPATH  +=  MS_Ref/source/video_security_kmv/export
SOURCES	 += mmikm_wintab.c mmikm_main.c mmikm_menutable.c mmikm_playlist.c
else
ifeq ($(strip $(KING_MOVIE_SUPPORT)), STAND)
MINCPATH  +=  MS_Ref/source/video_security/export
SOURCES	 += mmikm_wintab.c mmikm_main.c mmikm_menutable.c mmikm_playlist.c
endif
endif
endif



ifeq ($(strip $(MEDIA_DEMO_SUPPORT)), TRUE)

MSRCPATH  +=  MS_MMI/source/mmi_app/app/eng/media_demo/audio_demo/src
MINCPATH  +=  MS_MMI/source/mmi_app/app/eng/media_demo/audio_demo/data
MINCPATH  +=  MS_MMI/source/mmi_app/app/eng/media_demo/audio_demo/inc

MSRCPATH  +=  MS_MMI/source/mmi_app/app/eng/media_demo/video_demo/src
MINCPATH  +=  MS_MMI/source/mmi_app/app/eng/media_demo/video_demo/data
MINCPATH  +=  MS_MMI/source/mmi_app/app/eng/media_demo/video_demo/inc


MINCPATH += MS_Ref/export/inc
MINCPATH += MS_Ref/source/dc/dc_6x00/inc
MINCPATH += MS_Ref/source/isp_service/inc
MINCPATH += MS_Ref/source/dc/dc_common/inc
MINCPATH += chip_drv/export/inc/outdated
MINCPATH += MS_Ref/source/image_proc/inc

SOURCES += display_yuv_demo.c

SOURCES	+= aac_stream_demo.c amr_stream_demo.c amr_record_demo.c audio_express_demo.c mp3_stream_demo.c \
           wav_stream_demo.c  
                      
SOURCES += dc_video_demo.c


ifeq ($(strip $(CAT_MVOICE_SUPPORT)), TRUE)
SOURCES			+= cat_mvoice_demo.c 
endif

ifeq ($(strip $(MIC_SAMPLE_SUPPORT)), TRUE)
MCFLAG_OPT		+=     -DMIC_SAMPLE_DEMO_SUPPORT
SOURCES			+= mic_sample_demo.c
endif

endif

#API文件

SOURCES	 +=     mmikur.c mmipim_export.c mmiocr_export.c \
                mmidm_export.c mmiengtd_export.c mmifm_export.c mmidcd_export.c mmimms_export.c mmipb_export.c mmipic_edit_export.c \
                mmiudisk_export.c tp_export.c mmibt_export.c mmimtv_export.c mmiatv_export.c                               

SOURCES	 +=   mmiim_im_switching.c mmiim_tp_ui.c \
							mmiim_base.c mmiim.c mmiim_wintab.c mmiim_id.c mmiim_nv.c mmiim_menutable.c \
						  mmiim_sp_basic.c mmiim_sp_tp_basic.c mmiim_sp_hw.c mmiim_sp_symbol.c mmiim_sp_symbol_data.c \
						  mmiim_sp_multitap.c mmiim_sp_ml_data.c mmiim_sp_ml9key_data.c mmiim_sp_ml26key_data.c \
						  mmiim_tp_multitap_26key.c mmiim_tp_multitap_9key.c
ifeq ($(strip $(IM_ENGINE)), SOGOU)
#SOURCES	 +=     mmiim_sp_stroke.c mmiim_sp_py9key.c mmiim_sp_en9key.c mmiim_tp_sp.c mmiim_sp_py26key.c mmiim_sp_en26key.c  
MSRCPATH	+= MS_MMI/source/mmi_app/app/im/sogou/c
SOURCES	 +=     mmiim_sogou.c  mmiim_tp_sogou.c wdp_encrpty.c wdp_update.c wdp_sg_zhanxunfunc.c wdp_update_wintab.c \
						  wdp_SG_Interface_net.c mmiim_tp_sogou_keyboard_cfg.c 
SOURCES   += wd_publicIme.c SG_English_Public.c  SG_Bengali_Public.c \
			SG_Hindi_Public.c SG_Indonesian_Public.c   SG_Arabic_Public.c SG_French_Public.c SG_Malay_Public.c SG_Persian_Public.c  SG_Portuguese_Public.c \
			SG_Russian_Public.c  SG_Spanish_Public.c  SG_Thai_Public.c  SG_Turkish_Public.c  SG_Vietnamese_Public.c SG_Urdu_Public.c  SG_Tagalog_Public.c \
			SG_Slovenian_Public.c SG_Romanian_Public.c SG_Italian_Public.c  SG_Hungarian_Public.c  SG_Greek_Public.c  SG_German_Public.c  SG_Czech_Public.c \
			SG_Hebrew_Public.c  SG_Swahili_Public.c SG_Polish_Public.c SG_Dutch_Public.c SG_Bulgarian_Public.c SG_Telugu_Public.c SG_Danish_Public.c SG_Swedish_Public.c \
			SG_Finnish_Public.c  SG_Slovak_Public.c SG_Marathi_Public.c SG_Tamil_Public.c SG_Burmese_Public.c SG_Gujarati_Public.c SG_Hausa_Public.c \
			SG_Khmer_Public.c SG_Lao_Public.c SG_Punjabi_Public.c 
#SOURCES	 +=     mmiim_sp_tp_common.c mmiim_sp_tp_en26key.c mmiim_sp_tp_abc26key.c mmiim_sp_tp_digital.c \
#                mmiim_sp_tp_py26key.c mmiim_sp_tp_stroke.c mmiim_sp_tp_en9key.c mmiim_sp_tp_abc9key.c
else
SOURCES	 +=     mmiim_cstar.c  mmiim_tp_cstar.c mmiim_tp_cstar_keyboard_cfg.c 
endif
ifeq ($(strip $(IM_HANDWRITING)), SOGOU)
SOURCES   += wd_hwDatas.c  
endif

###use pda im, when exist touch panel
ifneq ($(strip $(TOUCHPANEL_TYPE)), NONE)
SOURCES	 +=     mmiim_touch_draw.c mmiim_touch_common.c mmiim_touch_ime_common.c mmiim_touch_set.c
ifeq ($(strip $(IM_ENGINE)), SOGOU)
SOURCES	 +=     mmiim_touch_ime_sogou.c mmiim_touch_ime_hw.c mmiim_touch_app_sogou.c 
else
SOURCES	 +=     mmiim_touch_ime_cstar.c mmiim_touch_ime_hw.c mmiim_touch_app_cstar.c 
endif
endif

ifeq ($(strip $(UPDATE_TIME_SUPPORT)),TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/aut/h
MSRCPATH += MS_MMI/source/mmi_app/app/aut/c
SOURCES	 += mmiaut_wintab.c  mmiaut_net.c  mmiaut_export.c 
endif

### mmi slide support
SOURCES	 += mmi_slide.c  #.c和.h均用MMI_SLIDE_SUPPORT 控制，如果MMI_SLIDE_SUPPORT屏蔽，则所有slide相关code不参与编译和link

ifeq ($(strip $(CMCC_UI_STYLE)), TRUE)
SOURCES	 +=     mmics_export.c mmics_id.c mmics_main.c mmics_menutable.c mmics_nv.c mmics_wintab.c               
endif 
  				
  
ifeq ($(strip $(APP_PRODUCT_DM)), TRUE)   
SOURCES	 +=     mmiengtd_menutable.c mmiengtd_win.c mmiengtd_nv.c mmiengtd_main.c mmiengtd_id.c               
endif 

ifeq ($(strip $(TTS_SUPPORT)), IFLYTEK)
SOURCES	 +=     TTS_api.c tts_iflytek_api.c tts_iflytek_audio.c 
endif 
  
ifeq ($(strip $(TTS_SUPPORT)), SHYOUNGTONE)
SOURCES	 +=     TTS_api.c tts_youngtong_api.c tts_youngtong_audio.c youngtone_tts_interface.c
endif

ifeq ($(strip $(TTS_SUPPORT)), EJ)
SOURCES	 +=    TTS_api.c ejTTSPlayer_audio.c file_func.c ejTTSPlayer_api.c
endif

ifneq ($(strip $(FM_SUPPORT)), NONE)
SOURCES	 +=     mmifm_app.c mmifm_nv.c mmifm_wintab.c mmifm_id.c               
endif 
     
ifeq ($(strip $(PIC_EDITOR_SUPPORT)), TRUE)
SOURCES	 +=     mmipic_edit.c mmipic_edit_wintab.c mmipic_id.c mmipic_menutable.c
endif 
            
ifeq ($(strip $(CMMB_SUPPORT)), TRUE)
SOURCES	 +=     mmimtv_data.c mmimtv_main.c mmimtv_nv.c mmimtv_osd_display.c mmimtv_wintab.c mbbms_api_dummy.c mmimbbms_main.c mmimbbms_wintab.c mmimtv_simu.c mmimtv_menutable.c
endif

ifneq ($(strip $(ATV_SUPPORT)), NONE)
SOURCES	 +=     mmiatv_data.c mmiatv_main.c mmiatv_wintab.c mmiatv_nv.c mmiatv_menutable.c mmiatv_simu.c
endif                                 

ifeq ($(strip $(KURO_SUPPORT)), TRUE)
SOURCES	 +=     kdbparser.c mmikur_bt.c mmikur_lyric.c mmikur_menutable.c mmikur_musicstyle.c mmikur_nv.c mmikur_wintab.c kuro_depends.c
endif   

ifeq ($(strip $(UI_SPRITE_ENABLE)), TRUE)
SOURCES	 +=     mmiss_sprite.c
endif   

#ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
#SOURCES	 +=     mmist_id.c tester_cal_ral.c mmist.c mmist_nv.c app_common_event_handlers.c mmist_wintab.c nexsal.c \
#                app_reg_sys2sal.c nexcr_bbamr.c app_streaming_event_handlers.c mmist_menutable.c nexcr_mv8602.c
#endif   
                
                
ifeq ($(strip $(OCR_SUPPORT)), TRUE)
SOURCES	 +=     mmiocr_id.c mmiocr.c mmiocr_lib.c mmiocr_wintab.c
endif                                

#ifeq ($(strip $(VT_SUPPORT)), TRUE)
SOURCES	 +=    mmivt_kernel_324Adapter.c mmivt_kernel_process.c mmivt_kernel_refAdapter.c mmivt_main.c \
                mmivt_ui.c mmivt_id.c mmivt_nv.c
#endif


ifneq ($(strip $(QQ_SUPPORT)), NONE)
ifeq ($(strip $(QQ_SUPPORT)),2009)
MCFLAG_OPT	+= -DQQ_SUPPORT_2009
endif
MSRCPATH 	+= MS_MMI/source/mmi_app/app/qq/c
MINCPATH 	+= MS_MMI/source/mmi_app/app/qq/h
SOURCES	+= mmiqq_ui.c  mmiqq_file.c mmiqq_input.c mmiqq_main.c mmiqq_socket.c mmiqq_system.c 
endif

ifeq ($(strip $(MET_MEX_SUPPORT)), TRUE)

MINCPATH += MS_MMI/source/mmi_app/app/met_mex/h
MINCPATH += MS_MMI/source/mmi_app/app/met_mex/Lib
MINCPATH += MS_MMI/source/mmi_app/app/met_mex/System

MSRCPATH += MS_MMI/source/mmi_app/app/met_mex/c
MSRCPATH += MS_MMI/source/mmi_app/app/met_mex/Lib
MSRCPATH += MS_MMI/source/mmi_app/app/met_mex/System
SOURCES  += met_mex_wintab.c MexConfig.c MexInterface.c met_mex_export.c
ifeq ($(strip $(MET_MEX_AQ_SUPPORT)), TRUE)
SOURCES    += met_mex_aq.c 
endif
endif

ifeq ($(strip $(WEATHER_SUPPORT)), TRUE)
MSRCPATH 	+= MS_MMI/source/mmi_app/app/weather/c
MINCPATH 	+= MS_MMI/source/mmi_app/app/weather/h

SOURCES	    += mmiweather.c mmiweather_export.c mmiweather_funcs.c mmiweather_id.c mmiweather_menutable.c \
               mmiweather_nv.c mmiweather_parse.c mmiweather_win.c 
endif

ifeq ($(strip $(GPS_SUPPORT)), TRUE)
SOURCES	 +=     mmigps_wintab.c mmigps_nv.c mmigps.c mmgps_menutable.c mmigps_simu.c mmigps_osapi.c
endif

ifeq ($(strip $(BROWSER_SUPPORT_NETFRONT)), TRUE)
SOURCES	 +=    mmiwww.c mmiwww_func.c mmiwww_menutab.c mmiwww_netfront.c mmiwww_netfront_cmd.c mmiwww_nv.c \
                mmiwww_slim_peer_alt.c mmiwww_slim_peer_debug.c mmiwww_slim_peer_file.c mmiwww_slim_peer_net.c \
                mmiwww_slim_peer_system.c mmiwww_slim_peer_time.c mmiwww_slim_peer_wave.c mmiwww_wintab.c mmiwww_bookmark.c
MCFLAG_OPT		+=-DDL_SUPPORT
SOURCES	 +=    mmidl.c mmidl_file.c mmidl_func.c mmidl_menutab.c mmidl_nv.c mmidl_wintab.c mmiwww_id.c
endif

ifeq ($(strip $(MMI_MOCOR_SAMPLE)), TRUE)
SOURCES		+=  sample_anim_ctrl.c sample_dropdownlist_ctrl.c sample_editbox_ctrl.c sample_form_ctrl.c \
					sample_iconlist_ctrl.c sample_id.c sample_im.c sample_main.c sample_menutable.c sample_nv.c \
					sample_richtext_ctrl.c sample_tab_ctrl.c sample_test_ctrl.c
endif

ifeq ($(strip $(PIM_SUPPORT)), TRUE)
SOURCES		+=  mmipim_id.c mmipim_menutable.c mmipim_nv.c mmipim_wintab.c mmipim.c mmipim_pb.c Vcalendar_parse.c Vcard_parse.c 
endif

ifeq ($(strip $(BROWSER_SUPPORT)), TRUE)
SOURCES	 +=    mmibrowser_manager_api.c mmibrowser_manager_func.c mmibrowser_manager_wintable.c mmibrowser_manager_nv.c mmibrowser_manager_menutable.c
endif

ifeq ($(strip $(BROWSER_SUPPORT_DORADO)), TRUE)
SOURCES	 +=    brw_wtai.c brw_temp.c brw_core_adapter.c \
               brw_gwt.c brw_dlinterface.c brw_history.c brw_control.c brw_httpinterface.c brw_snapshot.c brw_plinterface.c brw_utility.c \
               mmibrowser_dorado_api.c mmibrowser_memutable.c mmibrowser_bookmark.c mmibrowser_wintable_history.c mmibrowser_wintable_snapshot.c \
               mmibrowser_setting.c mmibrowser_wintable.c mmibrowser_nv.c mmibrowser_wintable_bookmark.c mmibrowser_wintable_setting.c \
               mmibrowser_dummy.c mmibrowser_wintable_flowrate.c brw_input.c mmibrowser_func.c mmibrowser_wintable_search.c
endif   

ifeq ($(strip $(SNS_SUPPORT)), TRUE)
SOURCES	 += mmisns_nv.c mmisns_export.c mmisns_srv.c mmisns_sqlite.c
endif

ifeq ($(strip $(MMS_SUPPORT)), DORADO)
SOURCES	 += mmimms_control_main.c mmimms_edit.c mmimms_editwin.c  mmimms_file.c mmimms_menutable_new.c mmimms_net.c mmimms_id.c\
                    mmimms_previewwin.c mmimms_push.c mmimms_setting.c mmimms_settingwin.c mmimms_main_new.c mmimms_waitingwin.c
endif 

ifeq ($(strip $(ASP_SUPPORT)), TRUE)
SOURCES	 += mmiasp_api.c mmiasp_cortrol_layer.c mmiasp_id.c mmiasp_menutable.c mmiasp_nv.c mmiasp_wintab.c
endif  

ifeq ($(strip $(DCD_SUPPORT)), TRUE)
SOURCES	 += mmidcd_file.c mmidcd_http.c mmidcd_id.c mmidcd_main.c mmidcd_menutable.c mmidcd_nv.c mmidcd_wintab.c mmidcd_xml_parse.c mmidcd_setting.c
endif  


ifeq ($(strip $(DEV_MANAGE_SUPPORT)), TRUE)
SOURCES	 += mmdm_id.c mmidm_app.c mmidm_menutable.c mmidm_nv.c mmidm_wintab.c mmidm_task.c mmidm_internal.c \
            mmidm_pl_mmi.c mmidm_pl_os.c mmidm_pl_comm.c mmidm_pl_string.c
ifeq ($(strip $(DM_SUPPORT)), DFRESH)
SOURCES	 += mmidm_parsexml.c mmidm_md5_b64.c
endif
else
ifeq ($(strip $(DM_ONLY_SELFREG_SUPPORT)), TRUE)
SOURCES  += mmdm_id.c mmidm_app.c mmidm_wintab.c mmidm_menutable.c mmidm_nv.c 
endif
endif  

#camera module
#export CAMERA_SUPPORT    = TRUE         ### Camera support:
                                        # Option1:TRUE
                                        # Option2:FALSE

#ifeq ($(strip $(CAMERA_SUPPORT)), TRUE)                                        
    ifeq ($(strip $(CAMERA_SENSOR_ANGLE)), "")
    export CAMERA_SENSOR_ANGLE = 0    ### camera sensor direction
                                      # Option1:0	    #define CAMERA_SENSOR_ANGLE_0
                                      # Option2:90   	#define CAMERA_SENSOR_ANGLE_90
    endif                                  
                                            
    export BURST_VOICE = EVERY        ### camera sensor direction
                                      # Option1:ONCE	    #define BURST_VOICE_ONCE
                                      # Option2:EVERY   	#define BURST_VOICE_EVERY        
                                      
                                      
    # camera sensor direction 
    #screen mode CAMERA_SCREEN_PORTRAIT and CAMERA_SCREEN_LANDSCAPE
    #if define macro CAMERA_SENSOR_ANGLE_90 
    #then define macro CAMERA_SCREEN_LANDSCAPE                                                        
    ifeq ($(strip $(CAMERA_SENSOR_ANGLE)), 90)
        MCFLAG_OPT += -DCAMERA_SENSOR_ANGLE_90
        MCFLAG_OPT += -DCAMERA_SCREEN_LANDSCAPE
    else
        MCFLAG_OPT += -DCAMERA_SENSOR_ANGLE_0
        
        ifeq ($(strip $(MAINLCD_LOGIC_ANGLE)), 0)
            MCFLAG_OPT += -DCAMERA_SCREEN_PORTRAIT
        endif
    
        ifeq ($(strip $(MAINLCD_LOGIC_ANGLE)), 90)
            MCFLAG_OPT += -DCAMERA_SCREEN_LANDSCAPE
        endif
    endif
    
    #burst voice 
    ifeq ($(strip $(BURST_VOICE)), ONCE)
        MCFLAG_OPT += -DBURST_VOICE_ONCE
    else
        MCFLAG_OPT += -DBURST_VOICE_EVERY
    endif
    
    #photo date
    ifeq ($(strip $(PLATFORM)), SC6800H)
    ifeq ($(strip $(LOW_MEMORY_SUPPORT)), NONE)
        MCFLAG_OPT += -DMMI_CAMERA_F_PHOTO_DATE
    endif    
    endif
    
    ifeq ($(strip $(PLATFORM)), SC6530)
        MCFLAG_OPT += -DMMI_CAMERA_F_PHOTO_DATE
    endif
    
    #desktop bar
    ifeq ($(strip $(MAINLCD_SIZE)), 240X320)
        MMI_CAMERA_UI_STYLE = DESKTOP_ICON_BAR
    endif
    
    ifeq ($(strip $(MAINLCD_SIZE)), 240X400)
        MMI_CAMERA_UI_STYLE = DESKTOP_ICON_BAR
    endif
    
    ifeq ($(strip $(MAINLCD_SIZE)), 320X480)
        MMI_CAMERA_UI_STYLE = DESKTOP_ICON_BAR
    endif
    
    ifeq ($(strip $(MMI_PDA_SUPPORT)), TRUE)
        MMI_CAMERA_UI_STYLE = DESKTOP_PDA
    endif
    
    #PDA data type 16 or 32 BIT
     ifeq ($(strip $(DISPLAY_PIXELDEPTH_MAX)), 16)
        MCFLAG_OPT += -DMMIDC_F_PDA_BIT_16
        
        MMI_CAMERA_F_ANIM = FALSE
    endif
    
    ifeq ($(strip $(MMI_CAMERA_UI_STYLE)), DESKTOP_ICON_BAR)
        MCFLAG_OPT += -DMMIDC_DESKTOP_ICON_BAR
    endif
    
    ifeq ($(strip $(MMI_CAMERA_UI_STYLE)), DESKTOP_PDA)
        MCFLAG_OPT += -DMMIDC_DESKTOP_PDA
        
        ifneq ($(strip $(MMI_CAMERA_F_ANIM)), FALSE)
            MCFLAG_OPT += -DMMIDC_F_PREVIEW_ANIM
            MCFLAG_OPT += -DMMIDC_F_CAPTURE_ANIM 
        endif
    endif
    
    ifeq ($(strip $(PRODUCT_CONFIG)), sc6800h_sp6804h)
        MCFLAG_OPT += -DMMIDC_F_SOFTWARE_ANGLE
    endif
    
    MCFLAG_OPT += -DMMIDC_F_DV_ZOOM
    MCFLAG_OPT += -DMMIDC_F_WORK_MODE
    
    ##u camera
    ifeq ($(strip $(U_CAMERA_SUPPORT)), TRUE)
        MMI_CAMERA_U_CAMERA = TRUE
        
        MMI_CAMERA_F_N_IN_ONE = TRUE
        MMI_CAMERA_F_EFFECT_360 = TRUE
        MMI_CAMERA_F_SMILE_AUTO_PHOTO = TRUE
    endif
 
   
    
    ifeq ($(strip $(MMI_CAMERA_U_CAMERA)), TRUE)
        MCFLAG_OPT += -DMMIDC_F_U_CAMERA
        
        ifeq ($(strip $(MMI_CAMERA_F_N_IN_ONE)), TRUE)
            MCFLAG_OPT += -DMMIDC_F_N_IN_ONE
        endif
        
      
        ifeq ($(strip $(MMI_CAMERA_F_SMILE_AUTO_PHOTO)), TRUE)
            MCFLAG_OPT += -DMMIDC_F_SMILE_AUTO_PHOTO
        endif

        ##effect 360
        ifeq ($(strip $(MMI_CAMERA_F_EFFECT_360)), TRUE)
            MCFLAG_OPT += -DMMIDC_F_EFFECT_360
            
            MMI_CAMERA_F_LOMO = TRUE
            MMI_CAMERA_F_FISH_EYE = TRUE
            MMI_CAMERA_F_HDR = TRUE            
            MMI_CAMERA_F_PENCIL_SKETCH = TRUE
            MMI_CAMERA_F_X_RAY = TRUE
            MMI_CAMERA_F_INFRARED = TRUE
            MMI_CAMERA_F_EXPOSAL = TRUE
            MMI_CAMERA_F_NEGATIVE = TRUE

            
            ifeq ($(strip $(MMI_CAMERA_F_LOMO)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_LOMO
            endif
            
            ifeq ($(strip $(MMI_CAMERA_F_FISH_EYE)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_FISH_EYE
            endif
               
            ifeq ($(strip $(MMI_CAMERA_F_HDR)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_HDR
            endif
            
            ifeq ($(strip $(MMI_CAMERA_F_PENCIL_SKETCH)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_PENCIL_SKETCH
            endif
            
            ifeq ($(strip $(MMI_CAMERA_F_X_RAY)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_X_RAY
            endif
            
            ifeq ($(strip $(MMI_CAMERA_F_INFRARED)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_INFRARED
            endif
            
            ifeq ($(strip $(MMI_CAMERA_F_EXPOSAL)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_EXPOSAL
            endif
            
            ifeq ($(strip $(MMI_CAMERA_F_NEGATIVE)), TRUE)
                MCFLAG_OPT += -DMMIDC_F_NEGATIVE
            endif            
        endif
        ##effect 360
        
    endif 
    ##u camera
    
    #----------------------------------------------#
    MMI_CAMERA_VIDEO_352X288 = TRUE
    
    ifeq ($(strip $(MMI_CAMERA_VIDEO_352X288)), TRUE)
        MCFLAG_OPT += -DMMIDC_F_VIDEO_352X288
    endif
    #----------------------------------------------#

    SOURCES  += mmidc_full_osd_display.c mmidc_flow.c mmidc_gui.c \
                mmidc_save.c mmidc_setting.c mmidc_main_wintab.c \
                mmidc_window_option.c mmidc_setting_nv.c mmidc_osd_option.c \
                mmidc_photodate.c mmidc_guipda.c mmidc_guiiconbar.c \
                mmidc_workmode.c \
                mmidc_imageprocess.c mmidc_ninone.c mmidc_effect360.c mmidc_smileautophoto.c \
                mmidc_ucamera_wintab.c
#endif

#always include this file mmidc_export.c
SOURCES  += mmidc_export.c

#end camera module

ifeq ($(strip $(JAVA_SUPPORT)), IA)               
SOURCES	       += mmijava_func.c mmijava_nv.c mmijava_wintab.c java_menutable.c mmijava_id.c \
		  AJEvent.c jblend_logFiler.c jwe.c jwe_eime.c jwe_log.c jwe_log_chk.c \
		  jwe_thread_com.c jwe_thread_kve.c \
		  jas_gfx_font.c AudioClient.c recordclient.c\
		  mediaControl.c media_midiControl.c media_toneControl.c media_mp3Control.c media_waveControl.c media_amrControl.c media_aacControl.c media_fileStream.c media_recordAmrControl.c media_cameraControl.c\
		  JgurToolKit.c JkMediaControl.c JkMediaCustomProtocol.c \
		  JkMediaMIDIDrv.c JkMediaStream.c JkMediaTone.c JkSoftkey.c JkUDP.c UDPResource.c \
		  UDPTaskBase.c UDPTaskProcess.c \
		  kjava_adv_fs.c kjava_sys.c kjava_sys_core.c kjava_sys_data_content.c \
		  kjava_sys_device.c kjava_sys_ext.c kjava_sys_ext_cert.c kjava_sys_fs.c kjava_sys_gfx.c \
		  kjava_sys_gfx_font_intra.c kjava_sys_http.c kjava_sys_log_out.c kjava_sys_pe.c \
		  kjava_sys_push.c kjava_sys_serial.c kjava_sys_sock.c kjava_sys_wma.c kjava_dev_fs.c JkPIM.c pim_contact.c pim_common.c JkVmBackground.c \
		  JkVmTimer.c JkCpuCache.c JkCommConnection.c
endif

ifeq ($(strip $(JAVA_SUPPORT)), SUN)
SOURCES	 += preload00_gen.c mmijava_main.c  mmijava_export.c mmijava_id.c mmijava_nv.c
endif 

ifeq ($(strip $(JAVA_SUPPORT)), MYRIAD)
SOURCES	 += mmijava_export.c mmijava_menutable.c mmijava_id.c mmijava_wintab_taskmgr.c jpl_ams_signalEvent.c \
            mmijava_wintab_about.c mmijava_nv.c mmijava_wintab_download.c mmijava_wintab_redkey.c os_main.c cpl_pim.c \
            nativeGUI.c wma.c cpl_file.c cpl_font.c cpl_gfx.c cpl_media.c cpl_midletinfo.c \
            cpl_midletinfo_helper.c cpl_net.c cpl_tm.c cpl_tpal.c jbed_app.c jbed_configuration.c \
            jbed_os.c jbed_props.c jpl_textfield.c jpl_phone.c mmijava_wintab_common.c mmijava_wintab_javalist.c \
            mmijava_wintab_optionlist.c mmijava_wintab_setting.c mmijava_wintab_install.c \
            mmijava_wintab_localinstall.c mmijava_wintab_comminfo.c mmijava_wintab_runningapp.c
endif

ifeq ($(strip $(UI_P3D_SUPPORT)), TRUE)
MSRCPATH +=  build/$(PROJECT)_builddir/tmp  
SOURCES	 +=  $(PROJECT)_ui_special_effect_table.c
endif       

    
ifeq ($(strip $(VIDEO_PLAYER_SUPPORT)), TRUE)
 SOURCES	 += mmivp_api.c mmivp_bookmarklist.c mmivp_fsm.c mmivp_historylist.c mmivp_locallist.c \
			mmivp_nv.c mmivp_setting.c mmivp_wintable.c mmivp_id.c mmivp_menutable.c mmivp_thumbnail.c 
ifneq ($(strip $(STREAMING_SUPPORT)), TRUE)
 SOURCES	 +=mmivp_dummy.c
endif
endif


ifeq ($(strip $(STREAMING_SUPPORT)), TRUE)
ifeq ($(strip $(MOBILE_VIDEO_SUPPORT)), TRUE)
SOURCES	      += mv_fs_api.c  mv_mm_api.c  mv_net_api.c  mv_sms_api.c mv_base_api.c  \
                 mv_contact_api.c mv_date_api.c mv_output_api.c mv_player_api.c mv_string_api.c mv_ui_api.c mv_timer_api.c \
                 mv_path_transfer.c mmimv_wintab.c mmimv_id.c mmimv.c mmimv_export.c
endif
endif


#keylock effect start
SOURCES	      += mmikl_effectmain.c          #MMI_KEY_LOCK_EFFECT_SUPPORT
SOURCES	      += mmikl_slidedown.c           #MMI_KEY_LOCK_EFFECT_TYPE1_SUPPORT
SOURCES	      += mmikl_slidescrollkey.c      #MMI_KEY_LOCK_EFFECT_TYPE2_SUPPORT
SOURCES	      += mmikl_doublecirclelock.c    #MMI_KEY_LOCK_DOUBLE_CIRCLE_SUPPORT
SOURCES	      += mmikl_smartslide.c          #MMI_KEY_LOCK_EFFECT_TYPE3_SUPPORT 
SOURCES	      += mmikl_drawgraphic.c         #MMI_KEY_LOCK_EFFECT_TYPE5_SUPPORT
SOURCES	      += mmikl_dragunlock.c          #MMI_KEY_LOCK_EFFECT_TYPE6_SUPPORT
SOURCES	      += mmikl_fruitsplit.c          #MMI_KEY_LOCK_FRUIT_SPLIT_SUPPORT
SOURCES	      += mmikl_effectpsw.c           #MMI_KEY_LOCK_EFFECT_PSW_SUPPORT
SOURCES	      += mmikl_Istyleslide.c         #MMI_KEY_LOCK_EFFECT_TYPE4_SUPPORT
#keylock effect end
	
#灭屏即锁屏
MCFLAG_OPT		+=  -DMMI_KEY_LOCK_BACKLIGHT_IN_1_SUPPORT

ifeq ($(strip $(MMI_PDA_SUPPORT)), TRUE) 
ifeq ($(strip $(CSC_SUPPORT)), TRUE)
SOURCES	 +=     mmicus_export.c mmicus_xmlparser.c mmicus_func.c mmicus_nv.c
endif 

#MCFLAG_OPT		+=  -DEFFECT_LIGHT_GRID_WALLPAPER_SUPPORT

#call log不显示主菜单
MCFLAG_OPT		+=  -DMMI_CALLLOG_HIDE_MAINMENU

MSRCPATH           += MS_MMI/source/mmi_app/app/mmise/c 
MINCPATH            += MS_MMI/source/mmi_app/app/mmise/h

endif


ifeq ($(strip $(MMI_MEMO_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/memo/h
MSRCPATH += MS_MMI/source/mmi_app/app/memo/c
SOURCES	 +=    mmimemo_api.c mmimemo_memutable.c mmimemo_wintable.c mmimemo_nv.c mmimemo_func.c
endif 

ifeq ($(strip $(VIDEO_PLAYER_SUPPORT)), TRUE)
MCFLAG_OPT		+=  -DMMI_VIDEOWALLPAPER_SUPPORT
endif

#ifeq ($(strip $(MMI_SERVICE_SUPPORT)), TRUE)
    MCFLAG_OPT += -DMMISRV_SUPPORT
    MINCPATH  +=  MS_MMI/source/mmi_service/export
    MINCPATH  +=  MS_MMI/source/mmi_service/source/mmisrvmgr/h
    MINCPATH  +=  MS_MMI/source/mmi_service/source/mmisrvapi/h
    MSRCPATH  += MS_MMI/source/mmi_service/source/mmisrvmgr/c
    MSRCPATH  += MS_MMI/source/mmi_service/source/mmisrvapi/c
    SOURCES += mmisrvmgr.c Mmisrv_debug.c

#    ifeq ($(strip $(MMI_SERVICE_AUDIO_SUPPORT)), TRUE)
        MCFLAG_OPT  += -DMMISRV_AUDIO_SUPPORT
        MCFLAG_OPT  += -DMMISRV_AUDIO_TEST_SUPPORT
        MINCPATH  +=  MS_MMI/source/mmi_service/source/mmisrvaud/h
        MSRCPATH  += MS_MMI/source/mmi_service/source/mmisrvaud/c
        MINCPATH  +=  MS_MMI/source/mmi_service/source/mmisrvaud_test/h
        MSRCPATH  += MS_MMI/source/mmi_service/source/mmisrvaud_test/c        
        SOURCES  += Mmisrvaud_api.c mmisrvaud.c Mmisrvaud_drv.c mmisrvaud_util.c mmisrvaud_test.c
#    endif
##endif

ifeq ($(strip $(LIVE_WALLPAPER_FRAMEWORK_SUPPORT)), TRUE)
MINCPATH += MS_MMI/source/mmi_app/app/livewallpaper/h 
MINCPATH += MS_MMI/source/mmi_app/app/livewallpaper/h/def 
endif

ifeq ($(strip $(LIVE_WALLPAPER_FRAMEWORK_SUPPORT)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/livewallpaper/c 
MSRCPATH += MS_MMI/source/mmi_app/app/livewallpaper/c/framework 
MSRCPATH += MS_MMI/source/mmi_app/app/livewallpaper/c/instance 
MSRCPATH += MS_MMI/source/mmi_app/app/livewallpaper/c/wincfg
endif

ifeq ($(strip $(LIVE_WALLPAPER_FRAMEWORK_SUPPORT)), TRUE)
SOURCES += mmi_livewallpaper.c mmi_livewallpaper_nv.c mmi_livewallpaper_win_cfg.c mmi_livewallpaper_instance_cfg.c mmi_livewallpaper_option.c mmi_livewallpaper_preview_wintab.c mmi_livewallpaper_pop.c \
           mmi_livewallpaper_bee.c mmi_livewallpaper_linear.c mmi_livewallpaper_galaxy.c mmi_livewallpaper_sakura.c mmi_livewallpaper_seaworld.c mmi_livewallpaper_lightgrid.c mmi_livewallpaper_ripple.c\
           mmi_livewallpaper_collision.c mmi_livewallpaper_littlecat.c mmi_livewallpaper_sweettree.c mmi_livewallpaper_collision_balloon.c mmi_livewallpaper_collision_menu.c\
	   mmi_livewallpaper_windmill.c mmi_livewallpaper_rainbowcity.c mmi_livewallpaper_papertown.c mmi_livewallpaper_aquarium.c mmi_livewallpaper_girlswing.c        
endif

ifeq ($(strip $(PLATFORM)), SC8800H)
SOURCES		+=   dummy_mneng_api.c    
endif  
ifeq ($(strip $(PLATFORM)), SC8800G)
SOURCES		+=   dummy_mneng_api.c    
endif             
ifeq ($(strip $(PLATFORM)), SC6600L)
MCFLAG_OPT += -DSC6600L_ARM7
endif    

ifeq ($(strip $(PLATFORM)), SC6800H)
SOURCES		+=   dummy_mneng_api.c    
endif

ifeq ($(strip $(PLATFORM)), SC6530)
SOURCES		+=   dummy_mneng_api.c    
endif

ifneq ($(strip $(WIFI_SUPPORT)), NONE)
		MINCPATH += MS_MMI/source/mmi_app/app/wlan/h 
		MINCPATH += MS_Customize/source/product/driver/wifi/wifisupp
		MSRCPATH += MS_MMI/source/mmi_app/app/wlan/c 
		SOURCES	 += mmiwlan_id.c mmiwlan_manager.c mmiwlan_menutable.c mmiwlan_nv.c mmiwlan_profile.c mmiwlan_win.c mmiwlan_portal.c mmiwlan_portal_net.c
endif

ifeq ($(strip $(MMIWIDGET_SUPPORT)), TRUE)
SOURCES		+=   mmiwidget.c mmiwidget_clock.c mmiwidget_display.c mmi_widget_grid.c mmiwidget_mp3.c \
                mmiwidget_nv.c mmiwidget_pb.c mmiwidget_id.c mmitheme_widget.c mmiwidget_memo.c mmiwidget_shortcut_menu.c \
                mmiwidget_calendar.c  mmiwidget_network_name.c  mmiwidget_custom_word.c  mmiwidget_NewSms.c \
                mmiwidget_calllog.c   mmiwidget_fm.c mmiwidget_menutable.c mmiwidget_alarm.c mmiwidget_shortcut_set.c \
                mmiwidget_Aclock.c mmiwidget_elve.c mmiwidget_Search.c mmiwidget_dcd.c mmiwidget_dual_clock.c mmiwidget_dual_aclock.c \
                mmiwidget_autoanswer_log.c mmiwidget_weather.c mmiwidget_schedule.c mmiwidget_sns.c mmiwidget_time.c
endif
ifeq ($(strip $(UI_P3D_SUPPORT)), TRUE)
SOURCES		+=   mmi_winse.c mmi_winse_imp.c mmi_winse_imp_app.c mmi_winse_imp_fade_inout.c mmi_winse_imp_vertical_inout.c
endif
ifeq ($(strip $(MMI_GRID_IDLE_SUPPORT)), TRUE)
SOURCES		+=  mmigrid_idle.c  mmigrid_display.c mmi_widget_grid.c \
                mmigrid_pb.c mmigrid_imgviewer.c mmiwidget_nv.c mmiwidget_id.c mmitheme_widget.c \
								mmiwidget_clock.c  mmiwidget_mp3.c \
                mmiwidget_memo.c mmiwidget_shortcut_menu.c mmiwidget_calendar.c  mmiwidget_network_name.c \
                mmiwidget_fm.c mmiwidget_menutable.c mmiwidget_alarm.c mmiwidget_shortcut_set.c \
                mmiwidget_Aclock.c mmiwidget_Search.c mmiwidget_dcd.c mmiwidget_dual_clock.c mmiwidget_dual_aclock.c \
                mmiwidget_weather.c mmiwidget_netsearch.c mmiwidget_countedtime.c mmigrid_alarm.c mmiwidget_schedule.c mmiwidget_sns.c\
                mmiwidget_time.c mmiwidget_weather2.c 
SOURCES	 +=     mmiwidget_speed_dial.c
endif

ifeq ($(strip $(QBTHEME_SUPPORT)), TRUE)
SOURCES		+=  mmiqbtheme_idlewin.c mmiqbtheme_idlefunction.c mmiqbtheme_init.c mmiqbtheme_menuwin.c \
							mmiqbtheme_widget.c mmiqbtheme_dockbar.c mmiqbtheme_virtualwin.c mmiqbtheme_page.c \
							mmiqbtheme_item.c  mmiqbtheme_grid.c  
endif

ifeq ($(strip $(PUSH_EMAIL_SUPPORT)), TRUE)
SOURCES += mail_file.c mail_util.c mail_setting.c mail_main.c mail_welcom.c mmimail_wintab.c mmimail_edit.c \
           mmimail_read.c mmimail_menutable.c mmimail_id.c mail_export.c 
endif
# Auto Testing Support
ifeq ($(strip $(ATEST_SUPPORT)), TRUE)
MSRCPATH +=  MS_MMI/source/mmi_app/app/atest/c
MINCPATH +=  MS_MMI/source/mmi_app/app/atest/h
SOURCES	 +=  atest_main.c atest_key_step.c atest_location.c atest_pb.c atest_r8.c atest_ota.c atest_inputmethod.c
endif

ifeq ($(strip $(SQLITE_SUPPORT)), TRUE)
# MCFLAG_OPT		+= -DSQLITE_FILEARRAY_SUPPORT
endif

ifeq ($(strip $(SQLITE_FILEARRAY_SUPPORT)), TRUE)
SOURCES	 += mmivirtualarray_sql.c mmifilearray_sql.c
else
SOURCES	 += mmivirtualarray.c mmifilearray.c
endif

#存在复杂文本,需要严格的计算字符串的宽高
#需要临时变量做或判断
#arabic, thai, hindi, hebrew, tibet等语言为复杂文本
ifeq ($(strip $(IM_ARABIC_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_PERSIAN_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_URDU_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_THAI_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_HINDI_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_HEBREW_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_BENGALI_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

ifeq ($(strip $(IM_MYANMAR_SUPPORT)), TRUE)
MCFLAG_OPT_COMPLEX_TEMP		?=  -DCOMPLEX_TEXT_SCRIPT
endif

MCFLAG_OPT += $(MCFLAG_OPT_COMPLEX_TEMP)
#存在复杂文本,需要严格的计算字符串的宽高

#下拉窗口
ifeq ($(strip $(PDA_UI_DROPDOWN_WIN)), TRUE)
MSRCPATH 	+= MS_MMI/source/mmi_app/app/dropdownwin/c
MINCPATH 	+= MS_MMI/source/mmi_app/app/dropdownwin/h
SOURCES	+= mmidropdownwin_main.c mmidropdownwin_nv.c mmidropdownwin_id.c
endif



ifeq ($(strip $(MMI_AZAN_SUPPORT)), TRUE)
MCFLAG_OPT	+= -DMMI_AZAN_MATRIX_SUPPORT
MINCPATH	+= MS_MMI/source/mmi_app/app/azan/h
MINCPATH	+= pri_code/source/azan/h
MSRCPATH	+= MS_MMI/source/mmi_app/app/azan/c
SOURCES		+= mmiazan_wintab.c
endif

ifeq ($(strip $(WRE_SUPPORT)),TRUE)
SOURCES		+= mmiwre_adaptor.c mmiwre_app.c mmiwre_boot_stub.c mmiwre_cfg.c mmiwre_charset.c mmiwre_dial.c mmiwre_export.c mmiwre_file.c mmiwre_font.c \
               mmiwre_gdi.c mmiwre_id.c mmiwre_image.c mmiwre_ime.c mmiwre_layer.c mmiwre_media.c mmiwre_menutable.c mmiwre_nv.c mmiwre_other.c mmiwre_sema.c \
               mmiwre_socket.c mmiwre_startup.c mmiwre_tapi.c mmiwre_thread.c mmiwre_timer.c mmiwre_wintab.c mmiwre_videoplayer.c mmiwre_pb.c mmiwre_device.c
endif

ifeq ($(strip $(IKEYBACKUP_SUPPORT)),TRUE)
SOURCES		+= mmiikeybackup_api.c mmiikeybackup_data.c mmiikeybackup_id.c mmiikeybackup_menutable.c mmiikeybackup_nv.c mmiikeybackup_task.c mmiikeybackup_wintab.c
endif

ifeq ($(strip $(MMIEMAIL_SUPPORT)),TRUE)
MSRCPATH 	+= MS_MMI/source/mmi_app/app/email/c
MINCPATH 	+= MS_MMI/source/mmi_app/app/email/h
SOURCES	+= mmiemail_edit.c mmiemail_export.c  mmiemail_comm.c mmiemail_view.c \
           mmiemail_main.c mmiemail_menutable.c mmiemail_setting.c mmiemail_sp.c \
					 mmiemail_sp_config.c mmiemail_sp_store.c mmiemail_sp_transaction.c mmiemail_vector.c mmiemail_utils.c\
					 mmiemail_dlg.c mmiemail_box.c mmiemail_net.c mmiemail_signal.c
					 
					 
MINCPATH 	+= Third-party/email/engine/email/inc
MINCPATH 	+= Third-party/email/engine/wep/inc
MINCPATH 	+= Third-party/email/engine/qc_inc

endif

ifeq ($(strip $(QBTHEME_SUPPORT)),TRUE)
MSRCPATH 	+= MS_MMI/source/mmi_app/app/qbtheme/c
MINCPATH 	+= MS_MMI/source/mmi_app/app/qbtheme/h
SOURCES	+=  mmi_qbtheme_map.c mmiqbtheme_app.c mmiqbtheme_export.c mmiqbtheme_id.c mmiqbtheme_nv.c mmiqbtheme_wintab.c

endif



ifeq ($(strip $(MMI_3DUI_SUPPORT)), TRUE)
    SOURCES 	+= mmise_calculation.c

    MSRCPATH        +=   MS_MMI/source/mmi_app/app/mmise/c 

    MINCPATH         +=  MS_MMI/source/mmi_app/app/mmise/h

    ifeq ($(strip $(MENU_CRYSTALCUBE_SUPPORT)), TRUE)
        SOURCES 	+= mmimenu_crystalcube.c crystal_cube.c
    endif

    ifeq ($(strip $(MENU_CYCLONE_SUPPORT)), TRUE)
        SOURCES 	+= mmimenu_cyclone.c cyclone.c
    endif
endif

#增加灭屏特效的宏
ifeq ($(strip $(UI_P3D_SUPPORT)), TRUE)
    ifeq ($(strip $(SE_CRT_SUPPORT)), TRUE)
    	ifeq ($(strip $(MMI_PDA_SUPPORT)),TRUE)
        LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT = TRUE
    		MCFLAG_OPT	+= -DLOCK_SCREEN_SPECIAL_EFFECT_SUPPORT
    	endif   
    endif  
endif 

ifeq ($(strip $(LOCK_SCREEN_SPECIAL_EFFECT_SUPPORT)),TRUE)
	SOURCES	 +=  mmiss_screenclose_special.c
endif#存在复杂文本,需要严格的计算字符串的宽高


OPERA_SUPPORT=FALSE
ifeq ($(strip $(OPERA_MINI_SUPPORT)), VER6)
MSRCPATH += MS_MMI/source/mmi_app/app/operamini6
SOURCES  += OpDevOperaMini.c
MCFLAG_OPT += -DOD_HAVE_SCHEME_API -DOD_TWEAK_IMPLEMENT_OPERAMINI_API -DOD_HAVE_ENTRY -DOD_TWEAK_PHYSICALKEYS=12 -DOD_NO_SIMULATOR -DOD_6530_SUPPORT
OPERA_SUPPORT=TRUE
endif
ifeq ($(strip $(OPERA_MINI_SUPPORT)), VER42)
MSRCPATH += MS_MMI/source/mmi_app/app/operamini4
SOURCES  += OpDevOperaMini.c
MCFLAG_OPT += -DOD_HAVE_SCHEME_API -DOD_TWEAK_IMPLEMENT_OPERAMINI_API -DOD_HAVE_ENTRY -DOD_NO_SIMULATOR -DOD_6530_SUPPORT
endif
ifeq ($(strip $(OPERA_MOBILE_SUPPORT)), V11)
OPERA_SUPPORT=TRUE
endif



#图片浏览器快速浏览界面
#ifeq ($(strip $(PIC_QUICKVIEW_SUPPORT)),TRUE)
#SOURCES  += mmipicview_quickview.c
#endif

ifneq ($(strip $(KEYPAD_TYPE)),FOUR_KEY_UNIVERSE)
		MCFLAG_OPT	+= -DMMI_WIDGET_DIR_KEY_SUPPORT
endif

ifneq ($(strip $(UZONE_SUPPORT)), NONE)
MINCPATH += MS_MMI/source/mmi_app/app/uzone/h
MSRCPATH += MS_MMI/source/mmi_app/app/uzone/c
ifeq ($(strip $(UZONE_SUPPORT)), GLASS)
MCFLAG_OPT	+= -DMMIUZONE_STYLE_GLASS
endif
SOURCES	 +=     mmiuzone.c mmiuzone_id.c mmiuzone_nv.c mmiuzone_wintab.c mmiuzone_export.c mmiuzone_display.c mmiuzone_se.c
endif

ifeq ($(strip $(MMS_SMS_IN_1_SUPPORT)), TRUE)
MCFLAG_OPT += -DMMIMMS_SMS_IN_1_SUPPORT
#for mms/sms contact search
MCFLAG_OPT += -DMMI_MESSAGE_CONTACT_NUM_MATCH_SUPPORT
endif

ifeq ($(strip $(CHATMODE_ONLY_SUPPORT)), TRUE)
MCFLAG_OPT += -DMMISMS_CHATMODE_ONLY_SUPPORT
endif

ifeq ($(strip $(PIC_CROP_SUPPORT)), TRUE)
ifneq ($(strip $(TOUCHPANEL_TYPE)), NONE) 
MCFLAG_OPT		+=  -DMMI_PIC_CROP_SUPPORT
endif
endif

ifneq ($(strip $(TOUCHPANEL_TYPE)), NONE) 
MCFLAG_OPT		+=  -DMMI_FMM_TITLE_BUTTON_SUPPORT
endif

#add by zack@20110117 for sky start
ifeq ($(strip $(MRAPP_SUPPORT)), TRUE)
MINCPATH   +=  MS_MMI/source/mmi_app/app/mrapp/h \
		MS_MMI/source/mmi_gui/source/anim/h \
		MS_Ref/export/inc \
		MS_Ref/source/dc/dc_6x00/inc \
		MS_Ref/source/isp_service/inc \
		MS_Ref/source/dc/dc_common/inc \
		MS_Ref/source/image_proc/inc 
MSRCPATH   +=  MS_MMI/source/mmi_app/app/mrapp/c
SOURCES	   +=  mmimrapp_func.c mmimrapp_wintab.c mmimrapp.c mmimrapp_motion.c mmimrapp_phonebook.c mmimrapp_camera.c
ifeq ($(strip $(__MMI_SKYQQ__)), TRUE)
MSRCPATH += MS_MMI/source/mmi_app/app/skyqq/c
MINCPATH += MS_MMI/source/mmi_app/app/skyqq/h
SOURCES	 += sky_qq_open.c
endif
endif
#add by zack@20110117 for sky end
ifeq ($(strip $(DMENU_SUPPORT)), TRUE)
              MINCPATH   +=  MS_MMI/source/mmi_app/app/dmenu/h 
		MSRCPATH   +=  MS_MMI/source/mmi_app/app/dmenu/c
		SOURCES     +=  mmidmenu.c mmidmenu_wintab.c mmidmenuApi.c
endif


ifeq ($(strip $(PIC_CROP_SUPPORT)), TRUE)
ifneq ($(strip $(TOUCHPANEL_TYPE)), NONE) 
MSRCPATH += MS_MMI/source/mmi_app/app/pic_crop/c
MINCPATH += MS_MMI/source/mmi_app/app/pic_crop/h
SOURCES	 += mmipic_crop.c mmipic_crop_wintab.c mmipic_crop_id.c mmipic_crop_menutable.c mmipic_crop_nv.c
endif
endif

ifeq ($(strip $(SXH_APPLE_SUPPORT)), TRUE)
MINCPATH  += MS_MMI/source/mmi_app/app/apple/h \
			 MS_Ref/source/isp_service/inc \
			 MS_Ref/source/dc/dc_6x00/inc \
			 MS_Ref/source/image_proc/inc \
			 MS_Ref/source/display/inc \
			 MS_Ref/source/image_proc/inc \
			 MS_Ref/source/lcd/inc \
			 MS_MMI/source/mmi_gui/source/listbox/h
#MS_Ref/source/dc/dc_6600l/inc \
			 
MSRCPATH +=  MS_MMI/source/mmi_app/app/apple/c	

SOURCES   += apple_func.c apple_id.c apple_nv.c apple_wintab.c apple_export.c
endif

ifeq ($(strip $(MCARE_V31_SUPPORT)), TRUE)
MCFLAG_OPT += -DSPREAD_PLAT_SC6530
ifeq ($(strip $(MMI_PDA_SUPPORT)), TRUE)
MCFLAG_OPT += -DSPREAD_PLAT_SC6530_PDA
endif

MINCPATH += MS_MMI/source/mmi_app/app/TencentMcareV31/inc
MINCPATH += MS_MMI/source/mmi_app/app/TencentMcareV31/src/resource/romizeddata
MSRCPATH += MS_MMI/source/mmi_app/app/TencentMcareV31/src
MSRCPATH += MS_MMI/source/mmi_app/app/TencentMcareV31/src/resource/romizeddata
MINCPATH += MS_MMI/MS_Ref/source/display/inc
MINCPATH += MS_Ref/source/display/inc

SOURCES += amr_record.c \
			McfInterface.c \
			McfZlib.c \
			MAdpCfgRomData.c
endif

ifneq ($(strip $(TPC_TYPE)), NONE)
ifneq ($(strip $(TOUCHPANEL_TYPE)),	NONE)
MCFLAG_OPT += -DTP_TEST_SUPPORT
endif
endif
MSRCPATH +=  MS_MMI/source/mmi_app/app/U_launcher/c	
MINCPATH +=  MS_MMI/source/mmi_app/app/U_launcher/h	

ifeq ($(strip $(PDA_UI_SUPPORT_MANIMENU_GO)), TRUE)
SOURCES	 += u_menu_se.c
endif

ifeq ($(strip $(PDA_UI_SUPPORT_U_IDLE)), TRUE)
SOURCES	 += u_idle_se.c mmigrid_uidle_se.c
endif
