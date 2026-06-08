# *************************************************************************
# Initialization
# *************************************************************************
MODIS_DIS_LIBS = 
MODIS_EN_LIBS = 
MODIS_MTK_LIBS = 
MODIS_EN_OBJS =
MODIS_DIS_INC_PATH = 
MODIS_EN_INC_PATH = 
MODIS_DIS_OPTION = 
MODIS_EN_OPTION = 

# *************************************************************************
# MODULE Config
# *************************************************************************
MODIS_DIS_LIBS += bootloader dp_engine dp_engine16 drv dsp_ram fdm fota
MODIS_DIS_LIBS += wndrv dummy_supc ll1
MODIS_DIS_LIBS += l1 l1_2 l1_classb l1_classb_2 l1_g2 l1_classb_g2 l1_classb_umts l1audio l1audio_sp l1audio32 l1d_lp1 l1d_lp2 l1d_lp3
MODIS_DIS_LIBS += nucleus nucleus_critical_data nucleus_ctrl_code nucleus_debug
MODIS_DIS_LIBS += nucleus_v2 nucleus_v2_critical_data nucleus_v2_ctrl_code nucleus_v2_debug
MODIS_DIS_LIBS += nucleus_ma nucleus_critical_data_ma nucleus_ctrl_code_ma nucleus_debug_ma
MODIS_DIS_LIBS += nucleus_int nucleus_int_ma nucleus_v2_int rompatch romsa usb
MODIS_DIS_LIBS += threadx threadx_debug dhcp
MODIS_DIS_LIBS += drv_display btmt ssf ft bmt fta ftc
MODIS_DIS_LIBS += l1audio_secure l1audio_fc
MODIS_DIS_LIBS += ast_l1_ast1001 ast_l1_ast2001
MODIS_DIS_LIBS += audio audio32 audio_comp mtd
MODIS_DIS_LIBS += sim_driver
MODIS_DIS_LIBS += peripheral
MODIS_DIS_LIBS += hostap mc usb_driver

MODIS_EN_LIBS += comm drv_sim oscar lcd_c_model
MODIS_EN_LIBS += fs

ifdef CUSTOM_RELEASE
  ifneq ($(strip $(CUSTOM_RELEASE)),TRUE)
    MODIS_EN_LIBS += p2p_modis
  endif
endif

ifneq ($(strip $(MMI_VERSION)),NEPTUNE_MMI)
  ifneq ($(strip $(MMI_VERSION)),EMPTY_MMI)
    MODIS_EN_LIBS += modis_lcd
  endif
endif

ifeq ($(call Upper,$(strip $(LEVEL))),VENDOR)
  MODIS_MTK_LIBS += comm drv_sim lcd_c_model modis_lcd
  MODIS_MTK_LIBS += fs
endif

ifeq ($(strip $(CUSTOM_RELEASE)),TRUE)
  ifneq ($(strip $(RELEASE_PACKAGE)),REL_SUB_MBJ_RD3)
    MODIS_MTK_LIBS += oscar
  endif
endif

ifneq ($(filter MT6236 MT6236B MT6276_S01, $(strip $(COM_DEFS))),)
  ifeq ($(strip $(CUSTOM_RELEASE)),FALSE)
    MODIS_EN_LIBS += gfx_hw_c_model
  else
    MODIS_MTK_LIBS += gfx_hw_c_model
  endif
endif

ifdef J2ME_SUPPORT
  ifeq ($(strip $(J2ME_SUPPORT)),IJET)
    ifeq ($(strip $(J2ME_VM)),KVM_SOURCE)
      MODIS_EN_LIBS += j2me_11
    endif
    ifeq ($(strip $(J2ME_VM)),KVM)
      MODIS_MTK_LIBS += j2me_11
    endif
    ifeq ($(strip $(J2ME_VM)),KVM_KDWP_SOURCE)
      MODIS_EN_LIBS += j2me_11_kdwp
    endif
    ifeq ($(strip $(J2ME_VM)),KVM_KDWP)
      MODIS_MTK_LIBS += j2me_11_kdwp
    endif
    ifeq ($(strip $(J2ME_VM)),HI_VM_SOURCE)
      MODIS_DIS_LIBS += j2me_hi
      MODIS_EN_LIBS += j2me_hi_i386
    endif
    ifeq ($(strip $(J2ME_VM)),HI_VM)
      MODIS_DIS_LIBS += j2me_hi
      MODIS_MTK_LIBS += j2me_hi_i386
    endif
  else ifeq ($(strip $(J2ME_SUPPORT)),NEMO)
    ifeq ($(strip $(J2ME_VM)),NEMO_VM_SOURCE_FULL)
      MODIS_EN_LIBS += j2me_nemo j2me_nemo_cldc_i386 j2me_nemo_fdlibm_i386
    else ifeq ($(strip $(J2ME_VM)),NEMO_VM_SOURCE_NORMAL)
      MODIS_EN_LIBS += j2me_nemo
      MODIS_MTK_LIBS += j2me_nemo_cldc_i386 j2me_nemo_fdlibm_i386
    else
      MODIS_MTK_LIBS += j2me_nemo j2me_nemo_cldc_i386 j2me_nemo_fdlibm_i386
    endif
  else ifeq ($(strip $(J2ME_SUPPORT)),JBLENDIA)
      MODIS_EN_OBJS += j2me\vm\jblendia\lib\$(strip $(JBLENDIA_LIB)\ajsc_win32_msvc.lib
      MODIS_EN_OBJS += j2me\vm\jblendia\lib\$(strip $(JBLENDIA_LIB)\libmjb2.lib
      MODIS_EN_OBJS += j2me\vm\jblendia\lib\$(strip $(JBLENDIA_LIB)\libaicrypto.lib
  else ifeq ($(strip $(J2ME_SUPPORT)),JBLENDIA_MVM)
      MODIS_EN_OBJS += j2me\vm\jblendia_mvm\lib\$(strip $(JBLENDIA_LIB)\ajsc_win32_msvc.lib
      MODIS_EN_OBJS += j2me\vm\jblendia_mvm\lib\$(strip $(JBLENDIA_LIB)\libmjb2.lib
      MODIS_EN_OBJS += j2me\vm\jblendia_mvm\lib\$(strip $(JBLENDIA_LIB)\libaicrypto.lib
  else ifeq ($(strip $(J2ME_SUPPORT)),MTK_DUMMY_MVM)
      #JAVA_DEF.mak
  else ifeq ($(strip $(J2ME_SUPPORT)),MTK_DUMMYVM)
      #JAVA_DEF.mak
  else ifeq ($(strip $(J2ME_SUPPORT)),NONE)
      #JAVA_DEF.mak
  endif
endif

# *************************************************************************
# INCLUDE PATH Config
# *************************************************************************
MODIS_EN_INC_PATH += kal\oscar\include 
MODIS_EN_INC_PATH += MoDIS_VC9\oscar\include
MODIS_EN_INC_PATH += tst\database_modis\pstrace_db
MODIS_EN_INC_PATH += image_codec\sw_jpeg_codec\include 
MODIS_EN_INC_PATH += $(strip $(PS_FOLDER))\ratdm\include 

ifneq ($(filter $(strip $(PLATFORM)),$(UL1_HS_PLATFORM)),)
  MODIS_EN_INC_PATH += ul1_hs\common
else
  MODIS_EN_INC_PATH += ul1\common
endif

MODIS_EN_INC_PATH += tst\database_modis\gv
MODIS_EN_INC_PATH += tst\database_modis\unionTag
MODIS_EN_INC_PATH += drv\display\include\lcd_c_model

ifneq ($(strip $(CUSTOM_RELEASE)),TRUE)
  MODIS_EN_INC_PATH += MoDIS_VC9\p2p_modis
endif

MODIS_DIS_INC_PATH += c:\progra~1\arm\adsv1_2\include 
MODIS_DIS_INC_PATH += nucleus\inc 
MODIS_DIS_INC_PATH += kal\nucleus\include 
MODIS_DIS_INC_PATH += nucleus_ma\inc
MODIS_DIS_INC_PATH += C:\Progra~1\ARM\RVCT\Data\2.1\328\include 
MODIS_DIS_INC_PATH += tst\database_classb\pstrace_db 
MODIS_DIS_INC_PATH += tst\database\pstrace_db
MODIS_DIS_INC_PATH += tst\database_classb_umts\pstrace_db

# *************************************************************************
# COMPILE OPTION Config
# *************************************************************************
MODIS_DIS_OPTION += __HW_CHE__ __HW_DIVIDER__ __MULTI_BOOT__ _DEBUG
MODIS_DIS_OPTION += __ROMSA_SUPPORT__ __USB_ENABLE__ __USB_LOGGING__ __SWDBG_SUPPORT__
MODIS_DIS_OPTION += KAL_ON_NUCLEUS L1_CATCHER MTK_KAL_MNT __MTK_TARGET__ WEBCAM_SUPPORT
MODIS_DIS_OPTION += KAL_ON_THREADX
MODIS_DIS_OPTION += VRSI_ENABLE __NVRAM_IN_USB_MS__ _USB_VBUS_DETECT_WITH_NIRQ_ __ENABLE_USB_IN_META__ 
MODIS_DIS_OPTION += __USB_IN_NORMAL_MODE__
MODIS_DIS_OPTION += __MTK_SECURE_PLATFORM__ __SECURITY_OTP__
MODIS_DIS_OPTION += __USB_IN_NORMAL_MODE_IMPROVE__ __ENABLE_USB_IN_META__ __OTG_ENABLE__ __USB_MULTI_CHARGE_CURRENT__
MODIS_DIS_OPTION += __SECURE_RO_ENABLE__ _DM_OMADL_VIA_DLAGENT_ __ARM9_DCACHEABLE__
MODIS_DIS_OPTION += __DYNAMIC_SWITCH_CACHEABILITY__
MODIS_DIS_OPTION += __TST_LMU_LOGGING__ __TST_DCC_ENABLED__ __TST_DNT_LOGGING__
MODIS_DIS_OPTION += __LANDSCAPE_UI_ON_PORTRAIT_LCM__
MODIS_DIS_OPTION += __KEYPAD_DEBUG_TRACE__ __KAL_RECORD_BOOTUP_LOG__ __USB_COM_PORT_ENABLE__ MODULE_TRACE_OFF

MODIS_EN_OPTION += DUMMYL1 L1_NOT_PRESENT KAL_ON_OSCAR UNIT_TEST 
MODIS_EN_OPTION += __OSCAR_ON_WIN32__ MAL1_NOT_PRESENT _WIN32_DCOM
MODIS_EN_OPTION += _MODIS_ON_VC9_


###############################################################
# Check 3rd-party utilities
###############################################################

MODIS_MTK_LIBS += ttl

ifeq ($(strip $(SSL_SUPPORT)),SSL_CIC_LIB)
  MODIS_MTK_LIBS += sb51 sbpki2 sslplus5
endif

ifeq ($(strip $(DRM_SUPPORT)),BSCI)
  MODIS_MTK_LIBS += bsci
endif

ifndef INPUT_METHOD_BASE
  INPUT_METHOD_BASE := $(strip $(MMIDIR))
  ifeq ($(strip $(DEMO_PROJECT)),FALSE)
    ifeq ($(strip $(MMI_VERSION)),PLUTO_MMI)
      INPUT_METHOD_BASE := $(strip $(call Upper,$(CUSTOMER)))
    endif
    ifeq ($(strip $(MMI_VERSION)),COSMOS_MMI)
      INPUT_METHOD_BASE := $(strip $(call Upper,$(CUSTOMER)))
    endif  
    ifeq ($(strip $(MMI_VERSION)),NEPTUNE_MMI)
      INPUT_METHOD_BASE := $(strip $(call Upper,$(CUSTOMER)))
    endif
  endif
  ifneq ($(strip $(MMI_VERSION)),PLUTO_MMI)
    ifneq ($(strip $(MMI_VERSION)),COSMOS_MMI)
    ifneq ($(strip $(MMI_VERSION)),NEPTUNE_MMI)
    ifneq ($(strip $(MMI_VERSION)),NEPTUNE_COLOR_MMI)
      INPUT_METHOD_BASE := $(strip $(call Upper,$(CUSTOMER)))
    endif
    endif
    endif
  endif
endif

ifeq ($(strip $(INPUT_METHOD)),MMI_ZI)
  MODIS_EN_OBJS += vendor\InputMethod\zi\project\$(strip $(INPUT_METHOD_BASE))\v7_official\lib\zi8clib_modis.lib \
                   vendor\InputMethod\zi\project\$(strip $(INPUT_METHOD_BASE))\v7_official\lib\zishape_modis.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_ZI_V6)
  MODIS_EN_OBJS += vendor\InputMethod\zi\project\$(strip $(INPUT_METHOD_BASE))\v6_official\lib\zi8clib_modis.lib \
                   vendor\InputMethod\zi\project\$(strip $(INPUT_METHOD_BASE))\v6_official\lib\zi8ARShape_modis.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_ZI_V7)
  MODIS_EN_OBJS += vendor\InputMethod\zi\project\$(strip $(INPUT_METHOD_BASE))\v7_official\lib\zi8clib_modis.lib \
                   vendor\InputMethod\zi\project\$(strip $(INPUT_METHOD_BASE))\v7_official\lib\zishape_modis.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_T9)
  ifeq ($(strip $(COMPILER)),RVCT)
    MODIS_EN_OBJS += vendor\InputMethod\t9\project\$(strip $(INPUT_METHOD_BASE))\v7_4_official_rvct\lib\T9RelVCChin_modis.obj
  else
    MODIS_EN_OBJS += vendor\InputMethod\t9\project\$(strip $(INPUT_METHOD_BASE))\v7_4_official\lib\T9RelVCChin_modis.obj
  endif
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_T9_ALPHA)
  ifeq ($(strip $(COMPILER)),RVCT)
    MODIS_EN_OBJS += vendor\InputMethod\t9\project\$(strip $(INPUT_METHOD_BASE))\v7_4_official_rvct\lib\T9RelVCAlpha_MoDIS.obj
  endif
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_T9_V7_4)
  ifeq ($(strip $(COMPILER)),RVCT)
    MODIS_EN_OBJS += vendor\InputMethod\t9\project\$(strip $(INPUT_METHOD_BASE))\v7_4_official_rvct\lib\T9RelVCChin_modis.obj
  else
    MODIS_EN_OBJS += vendor\InputMethod\t9\project\$(strip $(INPUT_METHOD_BASE))\v7_4_official\lib\T9RelVCChin_modis.obj
  endif
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_XT9)
  MODIS_EN_OBJS += vendor\InputMethod\t9\project\$(strip $(INPUT_METHOD_BASE))\xt9\lib\xt9.obj
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_FREE_FULL)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\free_full\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_FREE_SLIM_0K)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\free_slim_0K\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_FREE_SLIM_5K)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\free_slim_5K\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_FREE_SLIM_10K)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\free_slim_10K\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_EASTERNEURO_SLIM1)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\eastern_euro_slim1\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_EASTERNEURO_SLIM2)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\eastern_euro_slim2\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_V2)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\v2_official\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_CSTAR_V2_DEMO)
  MODIS_EN_OBJS += vendor\InputMethod\cstar\v2_demo\lib\kmxime_MoDIS.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_KOMOXO)
  MODIS_EN_OBJS += vendor\InputMethod\Komoxo\lib\kmxsip_input_Modis.lib
endif
ifeq ($(strip $(INPUT_METHOD)),MMI_KOMOXO_DEMO)
  MODIS_EN_OBJS += vendor\InputMethod\Komoxo\lib_demo\kmxsip_input_Modis.lib
endif

ifeq ($(strip $(HAND_WRITING)),MMI_HANWANG)
  ifeq ($(strip $(COMPILER)),RVCT)
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_official\lib\RVCT_chshw.lib \
                     vendor\handwriting\hanwang\v1_official\lib\RVCT_mtlhw.lib \
                     vendor\handwriting\hanwang\v1_official\lib\thaihw.lib \
                     vendor\handwriting\hanwang\v1_official\lib\RVCT_arabichw.lib
  else
    ifeq ($(strip $(DEMO_PROJECT)),TRUE)
      ifeq ($(strip $(MMI_FEATURES_SWITCH)),TRUE)
        MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_official\lib\hw_sc.lib
      else
        MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_official\lib\hws.lib
      endif
    else
      MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_official\lib\hws.lib
    endif
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_official\lib\HWRE_MTL_OEM.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_official\lib\thaihw.lib
  endif
endif
ifeq ($(strip $(HAND_WRITING)),MMI_HANWANG_DEMO)
  ifeq ($(strip $(COMPILER)),RVCT)
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\RVCT_chshw.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\RVCT_mtlhw.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\RVCT_thaihw.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\RVCT_arabichw.lib
  else
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\hws.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\HWRE_MTL_OEM.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\thaihw.lib
    MODIS_EN_OBJS += vendor\handwriting\hanwang\v1_demo\lib\arabichw.lib
  endif
endif
ifeq ($(strip $(HAND_WRITING)),MMI_PENPOWER)
  MODIS_EN_OBJS += vendor\handwriting\penpower\v1_official\lib\MediaTek_Hwre.lib
  MODIS_EN_OBJS += vendor\handwriting\penpower\v1_official\lib\MTK_PPhrase_WIN.lib
endif
ifeq ($(strip $(HAND_WRITING)),MMI_PENPOWER_DEMO)
  ifneq ($(strip $(TOUCH_PANEL_SUPPORT)),NONE)
    MODIS_EN_OBJS += vendor\handwriting\penpower\v1_demo\lib\MediaTek_Hwre_demo.lib
  endif
endif
ifeq ($(strip $(HAND_WRITING)),MMI_DECUMA)
  ifneq ($(strip $(TOUCH_PANEL_SUPPORT)),NONE)
    MODIS_EN_OBJS += vendor\handwriting\Zi_decuma\v1_official\lib\scrlib_MODIS.lib
    MODIS_EN_OBJS += vendor\handwriting\Zi_decuma\v1_official\lib\dltlib_MODIS.lib
    MODIS_EN_OBJS += vendor\handwriting\Zi_decuma\v1_official\lib\dltpredict_MODIS.lib
  endif
endif

ifneq ($(filter GE_BROGENT, $(strip $(COM_DEFS))),)
  MODIS_EN_OBJS += vendor\game_engine\brogent\engine_v1\pc\mini_bgl.lib \
                   vendor\game_engine\brogent\game\GoGoRobot2\GAME_LIB\pc\bgt_ggr2_optimize.lib \
                   vendor\game_engine\brogent\game\HeatBreaker2\GAME_LIB\pc\bgt_hb2_utils.lib
endif

# not supported
ifneq ($(filter GE_INTERGRAFX, $(strip $(COM_DEFS))),)
  # 3D game engine
  MODIS_EN_OBJS += vendor\game_engine\intergrafx\engine_v1\pc\$(strip $(MAIN_LCD_SIZE))\ignitem3g.lib \
                   vendor\game_engine\intergrafx\engine_v1\pc\$(strip $(MAIN_LCD_SIZE))\ignitemath.lib \
                   vendor\game_engine\intergrafx\engine_v1\pc\$(strip $(MAIN_LCD_SIZE))\libpng.lib
  # 3D game: petfish
  MODIS_EN_OBJS += vendor\game_engine\intergrafx\game\petfish\$(strip $(MAIN_LCD_SIZE))\modis\fish.obj \
                   vendor\game_engine\intergrafx\game\petfish\$(strip $(MAIN_LCD_SIZE))\modis\fishmenu.obj \
                   vendor\game_engine\intergrafx\game\petfish\$(strip $(MAIN_LCD_SIZE))\modis\gamemain.obj \
                   vendor\game_engine\intergrafx\game\petfish\$(strip $(MAIN_LCD_SIZE))\modis\ig_demogame.obj \
                   vendor\game_engine\intergrafx\game\petfish\$(strip $(MAIN_LCD_SIZE))\modis\logo.obj \
                   vendor\game_engine\intergrafx\game\petfish\$(strip $(MAIN_LCD_SIZE))\modis\utility.obj
  # 3D game: rollingball
  MODIS_EN_OBJS += vendor\game_engine\intergrafx\game\rollingball\$(strip $(MAIN_LCD_SIZE))\modis\ig_rollingball_$(strip $(MAIN_LCD_SIZE)).obj \
                   vendor\game_engine\intergrafx\game\rollingball\$(strip $(MAIN_LCD_SIZE))\modis\ig_rotation_$(strip $(MAIN_LCD_SIZE))_input.obj \
                   vendor\game_engine\intergrafx\game\rollingball\$(strip $(MAIN_LCD_SIZE))\modis\ig_rotation_main_$(strip $(MAIN_LCD_SIZE)).obj \
                   vendor\game_engine\intergrafx\game\rollingball\$(strip $(MAIN_LCD_SIZE))\modis\ig_rotation_title_$(strip $(MAIN_LCD_SIZE)).obj
endif

ifdef SW_FLASH
  ifneq ($(strip $(SW_FLASH)),NONE)
    ifneq ($(filter swflash, $(strip $(COMPLIST))),)
      MODIS_EN_OBJS += vendor\swflash\$(strip $(SW_FLASH))\v1_official\VIS20_WIN32_MTK.lib \
                       vendor\swflash\$(strip $(SW_FLASH))\v1_official\zdd2_1_lib.lib
    endif
  endif
endif

# T-DMB
ifdef TDMB_SUPPORT
  ifneq ($(strip $(TDMB_SUPPORT)),NONE)
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\LibCodecInterface.lib
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\LibDemuxInterface.lib
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\DLS_Parser.lib
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\PAD_Parser.lib
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\SDL.lib
  endif
endif

# CMMB
ifdef CMMB_SUPPORT
  ifneq ($(strip $(CMMB_SUPPORT)),NONE)
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\LibCodecInterface.lib
    MODIS_EN_OBJS += vendor\tdmb\cyberlink\lib\pc\SDL.lib
  endif
endif

# MTV Common
ifneq ($(filter __MTV_SUPPORT__, $(strip $(COM_DEFS))),)
  ifeq ($(strip $(MODIS_CONFIG)),TRUE)
    $(shell copy /y vendor\tdmb\cyberlink\lib\pc\SDL.dll MoDIS_VC9\MoDIS\Debug\ >NUL)
    $(shell copy /y vendor\tdmb\cyberlink\lib\pc\SDL.dll MoDIS_VC9\MoDIS\Release\ >NUL)
  endif
endif

# NMC
ifeq ($(strip $(NMC_SUPPORT)),TRUE)
  MODIS_EN_OBJS += vendor\nmc\corelib\modis\nmccore.lib
endif

# Mobile Video
ifeq ($(strip $(MOBILE_VIDEO_SUPPORT)),WONDER)
  MODIS_EN_OBJS += vendor\mobilevideo\corelib\modis\MTK_CM_MobileVideo.lib
endif

# Language Learning feature
ifdef LANGLN_ENGINE
  ifneq ($(strip $(LANGLN_ENGINE)),NONE)
    MODIS_EN_OBJS += vendor\langlearn\digidea\engine_01\dzdpc.lib
  endif
endif

# Font Engine feature
ifeq ($(strip $(FONT_ENGINE)),FONT_ENGINE_MONOTYPE)
  MODIS_EN_OBJS += vendor\vectorfont\monotype\official\engine_v1\monotype_engine.lib
endif
ifeq ($(strip $(FONT_ENGINE)),FONT_ENGINE_MONOTYPE_DEMO)
  MODIS_EN_OBJS += vendor\vectorfont\monotype\demo\engine_v1\monotype_engine.lib
endif
ifeq ($(strip $(FONT_ENGINE)),FONT_ENGINE_WTLE)
  MODIS_EN_OBJS += vendor\vectorfont\monotype\official\engine_wtle\itype_engine.lib
  MODIS_EN_OBJS += vendor\vectorfont\monotype\official\engine_wtle\wtle_engine.lib
endif

# GIS feature
ifeq ($(strip $(GIS_SUPPORT)),SUNAVI)
  MODIS_EN_OBJS += vendor\gis\sunavi\v1_demo\win32\sunavi_modis.lib
endif
ifeq ($(strip $(GIS_SUPPORT)),MAPBAR_NAVI)
  MODIS_EN_OBJS += vendor\gis\mapbar\v1_demo\win32\mapbar_navi_modis.lib
endif
ifeq ($(strip $(GIS_SUPPORT)),MAPBAR_BUS)
  MODIS_EN_OBJS += vendor\gis\mapbar\v1_demo\win32\mapbar_bus_modis.lib
endif
ifeq ($(strip $(GIS_SUPPORT)),MIGO)
  MODIS_EN_OBJS += vendor\gis\migo\lib\migo_modis.lib
  MODIS_EN_OBJS += vendor\gis\migo\lib\migo_encryption_win32.lib
endif

#DCD feature (dynamic content delivery)
ifeq ($(strip $(DCD_SUPPORT)),INFRAWARE)
  MODIS_EN_OBJS += vendor\dcd\v1_official\Win32\BrowserApplication.lib \
                   vendor\dcd\v1_official\Win32\DCDAgentComponent.lib \
                   vendor\dcd\v1_official\Win32\EFCInterface.lib \
                   vendor\dcd\v1_official\Win32\NetworkComponent.lib \
                   vendor\dcd\v1_official\Win32\PALComponent.lib \
                   vendor\dcd\v1_official\Win32\StorageComponent.lib \
                   vendor\dcd\v1_official\Win32\XMLParsingComponent.lib
endif

# SVG features
ifneq ($(filter IKIVO_SVG, $(strip $(COM_DEFS))),)
  MODIS_EN_OBJS += vendor\svg\ikivo\engine_v1\ikivo_svglib_modis.lib
endif
ifneq ($(filter IKIVO_SVG_SCRIPT_ENGINE, $(strip $(COM_DEFS))),)
  MODIS_EN_OBJS += vendor\svg\ikivo\engine_v1\ikivo_script_engine_modis.lib
endif

# Audio Vedio Library
ifeq ($(strip $(EMPTY_RESOURCE)),FALSE)
  ifeq ($(strip $(MODIS_CONFIG)),TRUE)
    $(shell if exist MoDIS_VC9\MoDIS\AVLib\Debug\AVLib.lib copy /y MoDIS_VC9\MoDIS\AVLib\Debug\AVLib.lib MoDIS_VC9\MoDIS\Debug\ >NUL)
    $(shell if exist MoDIS_VC9\MoDIS\AVLib\Debug\AVLib.dll copy /y MoDIS_VC9\MoDIS\AVLib\Debug\AVLib.dll MoDIS_VC9\MoDIS\Debug\ >NUL)
  endif
  MODIS_EN_OBJS += MoDIS_VC9\MoDIS\Debug\AVLib.lib
else
  ifeq ($(strip $(MODIS_CONFIG)),TRUE)
    $(shell if exist MoDIS_VC9\MoDIS\AVLib\Release\AVLib.lib copy /y MoDIS_VC9\MoDIS\AVLib\Release\AVLib.lib MoDIS_VC9\MoDIS\Release\ >NUL)
    $(shell if exist MoDIS_VC9\MoDIS\AVLib\Release\AVLib.dll copy /y MoDIS_VC9\MoDIS\AVLib\Release\AVLib.dll MoDIS_VC9\MoDIS\Release\ >NUL)
  endif
  MODIS_EN_OBJS += MoDIS_VC9\MoDIS\Release\AVLib.lib
endif

# MNL support
ifeq ($(strip $(GPS_SUPPORT)),MT3326)
  MODIS_EN_OBJS += gps\mnl\lib\MTKNav_WIN32.lib
  MODIS_EN_OBJS += gps\bee\lib\MTK_HS_WIN32.lib
endif

# UcWeb features
ifeq ($(strip $(UCWEB_SUPPORT)),UCWEB6)
  MODIS_EN_OBJS += vendor\ucweb\v1_official\win32\ucweb.lib
endif

# Opera features
ifeq ($(strip $(BROWSER_SUPPORT)),OPERA_V10)
  MODIS_EN_OBJS += vendor\opera\browser\v1_official\lib\win32\opera.lib \
                   vendor\opera\browser\v1_official\lib\win32\ulldvrm.obj
endif

# A8BOX_SUPPORT
ifeq ($(strip $(A8BOX_SUPPORT)),TRUE)
  MODIS_EN_OBJS += vendor\streaming\a8box\lib\official\a8box_w32.lib
endif

# KURO_SUPPORT
ifdef KURO_SUPPORT
  ifneq ($(strip $(KURO_SUPPORT)),NONE)
    MODIS_EN_OBJS += vendor\player\ipeer\lib\MODIS\libkuro.lib \
                     vendor\player\ipeer\lib\MODIS\libkurodep.lib
  endif
endif

# SCALADO_SUPPORT
ifdef SCALADO_SUPPORT
  ifeq ($(strip $(SCALADO_SUPPORT)),TRUE)
    MODIS_EN_OBJS += vendor\scalado\lib\WIN32\baseapi.lib \
                     vendor\scalado\lib\WIN32\capseng.lib \
                     vendor\scalado\lib\WIN32\codecs.lib \
                     vendor\scalado\lib\WIN32\excodecs.lib \
                     vendor\scalado\lib\WIN32\fmexif.lib \
                     vendor\scalado\lib\WIN32\fmextencoder.lib \
                     vendor\scalado\lib\WIN32\fmphotoartclipart.lib \
                     vendor\scalado\lib\WIN32\fmphotoartpack1.lib \
                     vendor\scalado\lib\WIN32\fmphotoartscribble.lib \
                     vendor\scalado\lib\WIN32\fmspeedview.lib \
                     vendor\scalado\lib\WIN32\osljpeg_sw.lib
  endif
endif

# Vector font
MODIS_EN_OBJS += $(shell if exist plutommi\Customer\ResGenerator\vf_modis_obj.txt type plutommi\Customer\ResGenerator\vf_modis_obj.txt)

# SUPC_SUPPORT
ifneq ($(strip $(WIFI_SUPPORT)),NONE)
  MODIS_EN_OBJS += wifi\lib\MoDIS\supc.lib
endif

# INTERGRAFX_SUPPORT
ifdef INTERGRAFX_SUPPORT
  ifneq ($(strip $(INTERGRAFX_SUPPORT)),NONE)
    MODIS_EN_OBJS += vendor\intergrafx\Engine\IGV3Lib\Win32\Debug\IGV3.lib
  endif
endif

# DSM_SUPPORT
ifdef DSM_SUPPORT
  ifneq ($(strip $(DSM_SUPPORT)),NONE)
    MODIS_EN_OBJS += plutommi\mmi\Mythroad\Mythroadlib\dsm-win32.lib \
                     plutommi\mmi\Mythroad\Mythroadlib\mmidsmex-win32.lib
  endif
endif

# *************************************************************************
# MoDIS Config
# *************************************************************************
MODIS_MTK_LIBS := $(filter-out $(strip $(MODIS_DIS_LIBS)),$(MODIS_MTK_LIBS))
MODIS_EN_LIBS := $(filter-out $(strip $(MODIS_DIS_LIBS)) $(strip $(MODIS_MTK_LIBS)),$(MODIS_EN_LIBS))

ifeq ($(strip $(MODIS_CONFIG)),TRUE)
  COMPLIST += $(MODIS_EN_LIBS)
  COMPLIST := $(filter-out $(strip $(MODIS_DIS_LIBS)) $(strip $(MODIS_MTK_LIBS)),$(COMPLIST))
  CUS_REL_SRC_COMP := $(filter-out $(strip $(MODIS_DIS_LIBS)) $(strip $(MODIS_MTK_LIBS)),$(CUS_REL_SRC_COMP))
  CUS_REL_PAR_SRC_COMP := $(filter-out $(strip $(MODIS_DIS_LIBS)) $(strip $(MODIS_MTK_LIBS)),$(CUS_REL_PAR_SRC_COMP))
  CUS_REL_MTK_COMP := $(filter-out $(strip $(MODIS_DIS_LIBS)) $(strip $(MODIS_MTK_LIBS)),$(CUS_REL_MTK_COMP))
  COMMINCDIRS := $(MODIS_EN_INC_PATH) $(COMMINCDIRS)
  COMMINCDIRS := $(filter-out $(strip $(MODIS_DIS_INC_PATH)),$(COMMINCDIRS))
  COM_DEFS := $(MODIS_EN_OPTION) $(COM_DEFS)
  COM_DEFS := $(filter-out $(strip $(MODIS_DIS_OPTION)),$(COM_DEFS))
  ifeq ($(strip $(BT_LIB_SWITCH)),TRUE)
    COMPLIST := $(filter-out $(BT_SWITCH_COMPLIST),$(COMPLIST))
    CUS_REL_MTK_COMP := $(filter-out $(BT_SWITCH_COMPLIST),$(CUS_REL_MTK_COMP))
    ifneq ($(strip $(CUSTOM_RELEASE)),TRUE)
      COMPLIST += $(BT_LINK_COMPLIST)
    else
      CUS_REL_MTK_COMP += $(BT_LINK_COMPLIST)
    endif
  endif
endif

ifneq ($(strip $(CUSTOM_RELEASE)),TRUE)
  CUS_MODIS_LIB := MoDIS_LIB
else
  CUS_MODIS_LIB := $(subst MTK_LIB\,MoDIS_LIB\,$(strip $(call Upper,$(CUS_MTK_LIB))))
endif

###############################################################
# Copy NFB files
###############################################################
ifeq ($(strip $(MODIS_CONFIG)),TRUE)
# MMIDIR is defined in MODEM.mak
  ifeq ($(strip $(NAND_FLASH_BOOTING)),ENFB)
    $(shell copy /y $(strip $(MMIDIR))\Customer\CustResource\CustENFBImgData MoDIS_VC9\MoDIS\CustENFBImgData >NUL)
    $(shell copy /y $(strip $(MMIDIR))\Customer\CustResource\CustENFBStrData MoDIS_VC9\MoDIS\CustENFBStrData >NUL)
  endif
# convert.exe is necessary in MoDIS.
  $(shell copy /y $(strip $(MMIDIR))\Customer\ResGenerator\convert.exe MoDIS_VC9\MoDIS\Debug\ >NUL);
  $(shell copy /y $(strip $(MMIDIR))\Customer\ResGenerator\convert.exe MoDIS_VC9\MoDIS\Release\ >NUL");
# Copy plutommi\Framework\MSLT\MSLTInc\MSLT_INFO to MoDIS_VC9\MoDIS\MSLT_INFO
  $(shell if exist plutommi\Framework\MSLT\MSLTInc\MSLT_INFO xcopy /e/y plutommi\Framework\MSLT\MSLTInc\MSLT_INFO MoDIS_VC9\MoDIS\MSLT_INFO\ >NUL)
endif

