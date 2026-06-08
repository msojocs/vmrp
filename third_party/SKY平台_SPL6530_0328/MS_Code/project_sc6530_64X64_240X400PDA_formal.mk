# *************************************************************************
# Feature Options (For User)
# !!!!!!!! The customer value can be modified in this option part.!!!!!!!!!
# !!!!!!!! If modification is in need,please find support from SPRD CPM or FAE.!!!!!!!!!
# !!!!!!!! 此部分Options 的值客户一般可以修改.!!!!!!!!!
# !!!!!!!! 此部分修改，最好与展讯客户经理(CPM)或技术支持工程师(FAE)沟通寻求支持.!!!!!!!!! 
# *************************************************************************

RELEASE_INFO = FALSE                                 ### RELEASE version
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MAINLCD_DEV_SIZE = 240X400                           ### Resolution Of Main LCD Device,  MAINLCD_SIZE is the size of MMI
                                                     # Option1:176X220    #define MAINLCD_DEV_SIZE_176X220
                                                     # Option2:220X176    #define MAINLCD_DEV_SIZE_220X176
                                                     # Option3:240X320    #define MAINLCD_DEV_SIZE_240X320
                                                     # Option4:320X240    #define MAINLCD_DEV_SIZE_320X240
                                                     # Option5:240X400    #define MAINLCD_DEV_SIZE_240X400                                    
                                                     # Option6:400X240    #define MAINLCD_DEV_SIZE_400X240
                                                     # Option7:320X480    #define MAINLCD_DEV_SIZE_320X480
                                                     # Option8:480X320    #define MAINLCD_DEV_SIZE_480X320
                                                     
MAINLCD_SIZE = 240X400                               ### Resolution Of Main LCD: 
                                                     # Option1:320X480    #define MAINLCD_SIZE_320X480
                                                     # Option2:240X400    #define MAINLCD_SIZE_240X400                                        
                                                     # Option3:240X320    #define MAINLCD_SIZE_240X320
                                                     # Option4:176X220    #define MAINLCD_SIZE_176X220
                                                             
FLASH_TYPE = SPI               	                     ### FLASH TYPE
                                                     # Option1:NOR        #define NOR_FLASH
                                                     # Option2:SPI
                                                     
FLASH_SIZE = 128MBIT                                 ### FLASH_SIZE SIZE
                                                     # Option1:NONE       #FALSH_SIZE_NONE
                                                     # Option2:16MBIT     #define FLASH_SIZE_16MBIT
                                                     # Option3:32MBIT     #define FLASH_SIZE_32MBIT
                                                     # Option4:64MBIT     #define FLASH_SIZE_64MBIT
                                                     # Option5:128MBIT    #define FLASH_SIZE_128MBIT
                                                         
SRAM_SIZE = 64MBIT                                   ### SRAM SIZE
                                                     # Option1:NONE       #SRAM_SIZE_NONE
                                                     # Option2:16MBIT     #define SRAM_SIZE_16MBIT
                                                     # Option3:32MBIT     #define SRAM_SIZE_32MBIT
                                                     # Option4:64MBIT     #define SRAM_SIZE_64MBIT
                                                                                                                                        
MAINLCM_INTERFACE = LCM                              #### LCM interface support
                                                     # Option1:LCDC
                                                     # Option2:LCM
                                                     # Option3:EMC
                                                     # Option4:SPI
                                                     # Option5:NONE
                                                     
SUBLCM_INTERFACE = NONE                              #### Sub LCM interface support
                                                     # Option1:LCDC
                                                     # Option2:LCM
                                                     # Option3:EMC
                                                     # Option4:SPI
                                                     # Option5:NONE
                                                         
LCD_DATA_WIDTH = 8BIT                                ### FPGA LCM LCD 8BIT

LCD_ROT_MODE = NONE                                  ### LCD display rotation direction
                                                     # Option1:LCD180
                                                     # Option2:NONE 
                                                                                                           
CUSTOM_CFLAG = -g                                    ### CUSTOM CFLAG
                                                     # Option1:-g

FONT_TYPE_SUPPORT = VECTOR                           ### font type support:
                                                     # Option1:NONE     #define FONT_TYPE_SUPPORT_NONE
                                                     # Option2:GRAY     #define FONT_TYPE_SUPPORT_GRAY
                                                     # Option3:VECTOR   #define FONT_TYPE_SUPPORT_VECTOR

VECTOR_FONT_SUPPORT = ETRUMP                         ### vector font support:
                                                     # Option1:DEFAULT
                                                     # Option2:ETRUMP 
                                                     
MMI_MULTI_GREEN_KEY = DUAL                           ### MMI_MULTI_GREEN_KEY Support for Dial Key
                                                     # Option1:SINGLE    #define MMI_MULTI_GREEN_KEY_SINGLE
                                                     # Option2:DUAL      #define MMI_MULTI_GREEN_KEY_DUAL
                                                     # Option3:TRI       #define MMI_MULTI_GREEN_KEY_TRI
                                                     # OPtion4:QUAD      #define MMI_MULTI_GREEN_KEY_QUAD
                                                     
MMI_MULTI_SIM_SYS = TRI                              ### Platform Support Multi Sim Card Number
                                                     # Option1:SINGLE    #define MMI_MULTI_SIM_SYS_SINGLE
                                                     # Option2:DUAL      #define MMI_MULTI_SIM_SYS_DUAL
                                                     # Option3:TRI       #define MMI_MULTI_SIM_SYS_TRI
                                                     # OPtion4:QUAD      #define MMI_MULTI_SIM_SYS_QUAD
                                                         
MULTI_SIM_SYS_QUAD_TO = NONE                         ### Platform Support Multi Sim Card Number
                                                     # Option1:DUAL_WITHOUT6302
                                                     # Option2:TRI_SINGLE6302_BBSIM0
                                                     # Option3:TRI_SINGLE6302_BBSIM1
                                                     # Option4:NONE
                                       
DUAL_BATTERY_SUPPORT = FALSE                         ### Dual battery Support
                                                     # Option2:FALSE 
                                                                                                
SIM_PLUG_IN_SUPPORT = FALSE                          ### Platform Support SIM HOT PLUGIN
                                                     # Option2:FALSE
                                                           
GPIO_SIMULATE_SPI_SUPPORT = FALSE                    ### To simulate SPI by GPIO
                                                     # Option2:FALSE  
                                                     
GPIO_SPI_SUPPORT = NONE                              ### Quadsim SPI by GPIO mode:
                                                     # Option1:NONE                 #define GPIO_SPI_SUPPORT_NONE        
                                                     
KEYPAD_TYPE = FOUR_KEY_UNIVERSE			             		 ### KEYPAD TYPE
                                                     # Option1:TYPICAL_KEYPAD        #define KEYPAD_TYPE_TYPICAL_KEYPAD
                                                     # Option2:QWERTY_KEYPAD         #define KEYPAD_TYPE_QWERTY_KEYPAD 
                                                     # Option3:COMPACT_KEYPAD        #define KEYPAD_TYPE_COMPACT_KEYPAD 
                                                     # Option4:FOUR_KEY_UNIVERSE     #define KEYPAD_TYPE_FOUR_KEY_UNIVERSE
                                                     # Option5:SINGLE_KEY_UNIVERSE   #define KEYPAD_TYPE_SINGLE_KEY_UNIVERSE  Apple Type
                                                     
MMI_PDA_SUPPORT = TRUE                               ### Platform Support PDA Style MMI
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                      
MMI_ANDROID_SUPPORT  = TRUE                          ### Platform Support Android style PDA UI
                                                     # Option1:TRUE   
                                                     # Option2:FALSE
									                                   
MMI_ISTYLE_SUPPORT   = TRUE                          ### Platform Support Iphone style PDA UI
                                                     # Option1:TRUE
                                                     # Option2:FALSE 

MMI_SMART_IDLE_SUPPORT = FALSE                       ### Smart Idle Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MMI_COMMON_IDLE_SUPPORT = FALSE                      ### Common Idle Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                       
MMI_GRID_IDLE_SUPPORT = TRUE                         ### Platform Support Grid idle support
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                 
                                                     
QWERTY_KEYPAD = NONE                                 ### QWERTY_KEYPAD TYPE
                                                     # Option1:NOKIA         #define QWERTY_KEYPAD_NOKIA
                                                     # Option2:BLACKBERRY    #define QWERTY_KEYPAD_BLACKBERRY 
                                                     # Option3:CUST          #define QWERTY_KEYPAD_CUST 
                                                     # Option4:NONE          #define QWERTY_KEYPAD_NONE    
                                                     
TOUCHPANEL_TYPE	= HW                                 ### Touch panel type
                                                     # Option1:HW            #define TOUCHPANEL_TYPE_HW
                                                     # Option2:NONE          #define TOUCHPANEL_TYPE_NONE
                                                     # Option3:MULTITP       #define TOUCHPANEL_TYPE_MULTITP

TPC_TYPE = SINGLE                                    ### tpc sampling type
                                                     # Option1 SINGLE  #define TPC_TYPE_SINGLE
                                                     # Option2 DOUBLE  #define TPC_TYPE_DOUBLE
                                                     # Option3 NONE    #define TPC_TYPE_NONE
                                                     
TOUCHPANEL_SW_HW = FALSE                             ### Touch panel SW
                                                     # Option1:TRUE          #define TOUCHPANEL_SW_HW_TRUE
                                                     # Option2:FALSE         #define TOUCHPANEL_SW_HW_FALSE
                                                         
CAP_TP_SUPPORT = NONE                                ### Capacitive Touch panel chip
                                                     # Option0:NONE          #define CAP_TP_SUPPORT_NONE
                                                     # Option1:FT5206        #define CAP_TP_SUPPORT_FT5206
                                                     # Option1:CY8C21434     #define CAP_TP_SUPPORT_CY8C21434
                                                     
TOUCH_PANEL_HWICON_SUPPORT  =  NONE                  ### touch panel hw support: 
                                                     # Option1:DIAL_PB_SMS_MAINMENU    #define TOUCH_PANEL_HWICON_SUPPORT_DIAL_PB_SMS_MAINMENU
                                                     # Option2:MAINMENU_PB_SMS_CAMERA_MP4  #define TOUCH_PANEL_HWICON_SUPPORT_MAINMENU_PB_SMS_CAMERA_MP4
                                                     # Option3:MAINMENU_SMS_PB_DIAL_MP3    #define TOUCH_PANEL_HWICON_SUPPORT_MAINMENU_SMS_PB_DIAL_MP3
                                                     # Option4:VIRTUAL_2KEY                 #define TOUCH_PANEL_HWICON_SUPPORT_VIRTUAL_2KEY
                                                     # Option5:NONE

TP_VK_SUPPORT = TRUE                                 ### touch panel virtual key driver support
                                                     # Option1:TRUE 
                                                     # Option2:FALSE

BB_DRAM_TYPE = NONE                                  ### DRAM Type
                                                     # Option1:NONE           #define BB_DRAM_TYPE_NONE
                                                     # Option1:32MB_16BIT     #define BB_DRAM_TYPE_32MB_16BIT
                                                     # Option2:32MB_32BIT_13R #define BB_DRAM_TYPE_32MB_32BIT_13R
                                                     # Option3:32MB_32BIT_12R #define BB_DRAM_TYPE_32MB_32BIT_12R
                                                     # Option4:64MB_16BIT     #define BB_DRAM_TYPE_64MB_16BIT
                                                     # Option5:64MB_32BIT     #define BB_DRAM_TYPE_64MB_32BIT  
                                   
MMI_RES_DIR = mmi_res_240x400_universe               ### resource directory
                                                     # Option1:mmi_res_240x320_universe
                                                     # Option2:mmi_res_240x400_universe
                                                     # Option3:mmi_res_320x480_universe
                                                     # Option4:mmi_res_176x220
                                                     # Option5:mmi_res_240x320
                                                     # Option6:mmi_res_240x400
                                                     # Option7:mmi_res_320x480
                                                                                                         
MMI_RES_ORIENT = PORTRAIT                            ### screen orientation
                                                     # Option1:PORTRAIT     #define MMI_RES_ORIENT_PORTRAIT
                                                     # Option2:LANDSCAPE    #define MMI_RES_ORIENT_LANDSCAPE
                                                     # Option3:BOTH         #define MMI_RES_ORIENT_BOTH
                                                     
MAINLCD_LOGIC_ANGLE = 0                              ### Mainlcd logic angle
                                                     # Option1:0        #define MAINLCD_LOGIC_ANGLE_0
                                                     # Option2:90       #define MAINLCD_LOGIC_ANGLE_90
                                                     # Option3:180      #define MAINLCD_LOGIC_ANGLE_180
                                                     # Option4:270      #define MAINLCD_LOGIC_ANGLE_270
                                                     
LCD_SLIDE_SUPPORT = FALSE                            ### LCD Slide Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE										
                                                     
LCD_SLIDE_SIMU = FALSE                               ### LCD Slide SIMU, use star key replace lcd_slide key: 
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SUBLCD_DEV_SIZE = NONE                               ### Resolution Of Main LCD(None To Disable Related Opeartions Of SUBLCD):
                                                     # Option1:NONE       #define SUBLCD_DEV_SIZE_NONE
                                                     # Option2:96X64      #define SUBLCD_DEV_SIZE_96X64
                                                     # Option3:96X96      #define SUBLCD_DEV_SIZE_96X96
                                                     
SUBLCD_LOGIC_ANGLE = 0                               ### Sublcd logic angle
                                                     # Option1:0        #define SUBLCD_LOGIC_ANGLE_0
                                                     # Option2:90       #define SUBLCD_LOGIC_ANGLE_90
                                                     # Option3:180      #define SUBLCD_LOGIC_ANGLE_180
                                                     # Option4:270      #define SUBLCD_LOGIC_ANGLE_270
                                                     
SUBLCD_SIZE = NONE                                   ### Resolution Of Main LCD(None To Disable Related Opeartions Of SUBLCD):
                                                     # Option1:NONE       #define SUBLCD_SIZE_NONE
                                                     # Option2:96X64      #define SUBLCD_SIZE_96X64
                                                     # Option3:96X96      #define SUBLCD_SIZE_96X96
                                                     
MAIN_LCD_DISPLAY_LAYER_NUM = 1                       ### Support display layer num of main lcd
                                                     # Option1:1        #define MAIN_LCD_DISPLAY_LAYER_NUM_1
                                                     # Option2:2        #define MAIN_LCD_DISPLAY_LAYER_NUM_2 
                                                     # Option3:3        #define MAIN_LCD_DISPLAY_LAYER_NUM_3  
                                                     
SUB_LCD_DISPLAY_LAYER_NUM = 0                        ### Support display layer num of sub lcd
                                                     # Option1:0         #define SUB_LCD_DISPLAY_LAYER_NUM_0  
                                                     # Option2:1         #define SUB_LCD_DISPLAY_LAYER_NUM_1
                                                     
DISPLAY_PIXELDEPTH_MAX = 16                          ### depth of display pixel on lcd
                                                     # Option1:16       #define DISPLAY_PIXELDEPTH_MAX_16
                                                     # Option2:32       #define DISPLAY_PIXELDEPTH_MAX_32
                                                     
SPECIAL_EFFECT_ICON_SIZE = 48X48                     ### Wide x Height of special effect icon! Must equal Main menu icon size
                                                     ### If none,not support Mainmenu 3d icon moving
                                                     # Option1:48X48    
                                                     
PRELOAD_SUPPORT = FALSE                              ### pre-load support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
UI_P3D_SUPPORT = FALSE                               ### UI P3D Effect Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MMI_PUBWIN_ANIM_SUPPORT = FALSE                      ### MMI PUBWIN Anim Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
FM_SUPPORT =  NMI601                                 ### FM Function Support:
                                                     # Option0:NONE      #define FM_SUPPORT_NONE
                                                     # Option1:CL6012X   #define FM_SUPPORT_CL6012X
                                                     # Option2:NMI601    #define FM_SUPPORT_NMI601
                                                     
TTS_SUPPORT = EJ                                     ### TTS Support
                                                     # Option1:NONE        #define TTS_SUPPORT_NONE
                                                     # Option2:IFLYTEK     #define TTS_SUPPORT_IFLYTEK
                                                     # Option3:EJ          #define TTS_SUPPORT_EJ
                                                     # Option4:SHYOUNGTONE #define TTS_SUPPORT_YOUNGTONE
                                                     
MV_SUPPORT = NONE                                    ### Magic voice Support
                                                     # Option1:NONE     #define MV_SUPPORT_NONE
                                                     # Option2:IFLYTEK  #define MV_SUPPORT_IFLYTEK
                                                     
ASP_SUPPORT = FALSE                                  ### ASP Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                        
                                                                                                                           
MULTI_THEME_SUPPORT = FALSE                          ### theme Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
HOME_LOCATION_INQUIRE_SUPPORT = TCARD                ### HOME_LOCATION_INQUIRE_SUPPORT Support: ONLY VALIDATE WHEN CHINESE LANGUAGE SUPPORTED
                                                     # Option1:CODE    #define HOME_LOCATION_INQUIRE_SUPPORT_CODE
                                                     # Option2:TCARD   #define HOME_LOCATION_INQUIRE_SUPPORT_TCARD   //归属地信息放入T卡中。
                                                     # Option3:NONE    #define HOME_LOCATION_INQUIRE_SUPPORT_NONE
                                                       
CAMERA_SENSOR_ANGLE = 0                              ### Sensor Angle
                                                     # Option1:0	    #define CAMERA_SENSOR_ANGLE_0
                                                     # Option2:90	  #define CAMERA_SENSOR_ANGLE_90   
                                                     
ATV_SUPPORT = NONE                                   ### ATV_SUPPORT Player And Driver Support:
                                                     # Option1:TLG1120     #define ATV_TYPE_TLG1120
                                                     # Option2:QN8610      #define ATV_TYPE_QN8610
                                                     # Option3:NMI601      #define ATV_SUPPORT_NMI601
                                                     # Option4:RDA5888     #define ATV_SUPPORT_RDA5888
                                                     # Option5:NONE
                                                     
CMMB_SUPPORT = FALSE                                 ### CMMB Player And Driver Support:
                                                     # Option2:FALSE
                                                     
MBBMS_SUPPORT = FALSE                                ### MBBMS Player And Driver Support:
                                                     # Option2:FALSE
                                                     
CMMB_DAC_TYPE = HYBRID                               ### support 3 dac mode
                                                     # Option1:AP        #define CMMB_DAC_TYPE_AP
                                                     # Option2:V         #define CMMB_DAC_TYPE_V
                                                     # Option3:HYBRID    #define CMMB_DAC_TYPE_HYBRID
                                                     
STREAMING_SUPPORT = FALSE                            ### Switch Streaming Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                         
                                                     
VIDEO_PLAYER_SUPPORT = TRUE                          ### video player:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
KING_MOVIE_SUPPORT = ALL                             ### kingmovie play:
                                                     # Option1:STAND
                                                     # Option2:EXTEND
                                                     # Option3:ALL
                                                     # Option4:NONE
                                                     
TYPE_3GP_SUPPORT = MP4                               ### 3GP type format Support:
                                                     # Option1:NONE       #define TYPE_3GP_SUPPORT_NONE
                                                     # Option2:MP4        #define TYPE_3GP_SUPPORT_MP4
                                                     # Option3:MJPEG      #define TYPE_3GP_SUPPORT_MJPEG
                                                     
PIC_EDITOR_SUPPORT = FALSE													 ### Picture Editor Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
PIC_GLIDE_SUPPORT = FALSE                            ### Picture glide preview support
                                                     # Option1:TRUE  
                                                     # Option2:FALSE
                                                     
PIC_ZOOM_SUPPORT = FALSE                             ### Picture zoom
                                                     # Option1:TRUE  
                                                     # Option2:FALSE
                                                     
WIFI_SUPPORT = NONE                                  ### wifi Support
                                                     # Option1:NONE           #define WIFI_SUPPORT_NONE
                                                     # Option2:CSR6030        #define WIFI_SUPPORT_CSR6030
                                                     # Option3:CSR6027A05     #define WIFI_SUPPORT_CSR6027A05
                                                     # Option4:CSR6027B07     #define WIFI_SUPPORT_CSR6027B07
                                                     # Option5:RTL8723        #defube WIFI_SUPPORT_RTL8723
                                                                                                          
WLAN_PORTAL_SUPPORT = FALSE                          ### wlan portal support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WLAN_RESELECT_SUPPORT = FALSE                        ### wlan reselect support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SQLITE_SUPPORT = FALSE                               ### Sqlite
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
DEV_MANAGE_SUPPORT = FALSE                           ### device manage support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

BROWSER_SUPPORT = TRUE                               ### Browser SW Supplier:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 

BROWSER_SUPPORT_DORADO = TRUE                        ### dorado Browser support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

BROWSER_SUPPORT_NETFRONT = FALSE                     ### netfront Browser support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE	   
                                                     
CSS_SUPPORT = TRUE                                  ### Browser SW Supplier:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
BROWSER_OPERA_START_PAGE = FALSE                     ### Browser SW Supplier:
                                                     # Option1:TRUE
                                                     # Option2:FALSE      
                                            
BROWSER_OPERA_FAVORITE_WITH_DEFAULT = FALSE          ### Browser SW Supplier:
    	                                             # Option1:TRUE
    	                                             # Option2:FALSE

NETFRONT_SUPPORT = NONE                              ### Browser SW Supplier:
                                                     # Option1:NONE
                                                     # Option2:VER35     #define NETFRONT_SUPPORT_VER35
                                                     # Option3:VER40     #define NETFRONT_SUPPORT_VER40
                                                     # Option4:VER42     #define NETFRONT_SUPPORT_VER42
                                                     
PIM_SUPPORT = FALSE                                  ### PIM(personal Information Manage) Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
FLASH_SUPPORT = FALSE                                ### If flash UI support, MCAREV30_FLASH_SUPPORT must be true
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
UPDATE_TIME_SUPPORT = FALSE                          ### UPDATE_TIME_SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SNTP_SUPPORT = FALSE                                 ### Simple network time protocol
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
DCD_SUPPORT = FALSE                                  ### dcd support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MMS_SUPPORT = DORADO                                 ### MMS SW Supplier:
                                                     # Option1:NONE      #define MMS_SUPPORT_NONE
                                                     # Option2:DORADO    #define MMS_SUPPORT_DORADO
                                                     
JAVA_SUPPORT = NONE                                ### JAVA SW Supplier:
                                                     # Option1:IA          #define JAVA_SUPPORT_IA
                                                     # Option1:MYRIAD      #define JAVA_SUPPORT_MYRIAD
                                                     # Option2:NONE        #define JAVA_SUPPORT_NONE

JAVA_SUPPORT_TYPE = SVM                              ###  JAVA_SUPPORT_TYPE
                                                     # Option1: SVM        #difine JAVA_SUPPORT_TYPE_Myriad svm                                                   
                                                     # Option2: MVM        #define JAVA_SUPPORT_TYPE_Myriad mvm
                                                     # Option3: NONE       #define JAVA_SUPPORT_TYPE_NONE
                                                     
QQ_SUPPORT = NONE                                    ### QQ Support
                                                     # Option1:NONE        #define QQ_SUPPORT_NONE 
                                                     
MCARE_V31_SUPPORT = FALSE                            ###MACARE QQ
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                           
WEATHER_SUPPORT = FALSE                               ### Weather forecast support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                        
                                                     
EMAIL_SUPPORT = FALSE                                ### EMAIL Client:
                                                     # Option1:FALSE                                                                                
                                                     
PCLINK_SUPPORT = TRUE                                ### PCLink function
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     # work for TCPIP_SUPPORT = TRUE  
                                                     
USB_WEBCAMERA_SUPPORT = FALSE                        ### USB Web Camera Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SCREENSAVER_SUPPORT = TRUE                           ### screensaver support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SPLOAD_ENABLE = FALSE                                ### lcd and sensor seperate bin dload support
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                    
                                                     
PRODUCT_CONFIG = fp6530_pda                          ### MS_Customize/source/product/config;scf file;nv;resource
                                                     # default: project name 
                                                     
PRODUCT_BASELIB_DIR = $(PROJECT)                     ### Dir name for base lib in MS_Code\lib\
                                                     # default: project name 
                                                     
MOTION_SENSOR_TYPE = NONE                            ### MOTION SENSOR ACCELEROMETER SUPPORT:
                                                     # Option1:NONE
                                                     # Option2:ST_LIS35DE
                                                     # Option3:MEMS_DTOS                                        
                                                     # Option4:TS1003
                                                     # Option5:ADXL346
                                                     
MOTION_SENSOR_TYPE_OPTICAL = NONE                    ### MOTION SENSOR OPTICAL SUPPORT:
                                                     # Option1:NONE
                                                     # Option2:SCM013
                                                     # Option3:LTR501                                                     
                                                     
EXSTERN_WIFI_CO_EXIST = FALSE                        #### option true atheros wifi and csr bt co-exist
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
FONT_HAN_SIMP_ONLY = TRUE                            ### FONT SIMP Only:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                                         
BLUETOOTH_SUPPORT = NONE                ### Bluetooth Chip Type: 
                                                     # Option1:NONE      #define BLUETOOTH_SUPPORT_NONE
                                                     # Option2:CSR_BC4_6 #define BLUETOOTH_SUPPORT_CRS_BC4_6
                                                     # Option3:CSR_BC4_6_LOW_MEM: low mem and code size , only support OPP HFG
                                                     # Option4:RTL8723   Realtek RTL8723AS wifi/bt combo chip
                                                     # Option5:IS1662    ISSC IS1662S bt chip
                                                     
BT_SHARE_XTL_SUPPORT = FALSE                         ### Switch Of BT Share Crystal Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SHELL_DEBUG_SUPPORT = FALSE                          ### SHELL_DEBUG_SUPPOR
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
GPS_SUPPORT = FALSE                                  ### Switch Of GPS Module Support
                                                     # Option1:FALSE
                                                     
MULTI_SIM_GSM_CTA_SUPPORT = FALSE                    ### MULTI SIM GSM CTA Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
TP_SELFADAPTIVE_ENABLE = FALSE                       ### TP_SELFADAPTIVE_ENABLE Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE	
                                                     
MMI_AZAN_SUPPORT = FALSE                             ### MMI AZAN Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
IM_ENGINE = SOGOU                                    ### Im Engine Support:
                                                     # Option1:CSTAR    #define IM_ENGINE_CSTAR
                                                     # Option2:SOGOU
                                                     
IM_HANDWRITING = SOGOU                               ### Handwriting SW Supplier(None to Disable):
                                                     # Option1:HANWANG   #define IM_HANDWRITING_HANWANG
                                                     # Option2:SOGOU
                                                     # Option3:NONE      #define IM_HANDWRITING_NONE
                                                     
MMIIM_HW_FULLSCREEN_SUPPORT = FALSE                  ### FULL SCREEN HANDWRITING SUPPORT
                                                     # Option1:TRUE    only support cstar,hanwang and pda
                                                     # Option2:FALSE                                                       
                                                     
IM_SIMP_CHINESE_SUPPORT = FALSE	                     ### SIMPLE CHINESE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_TRAD_CHINESE_SUPPORT = FALSE	                     ### TRAD CHINESE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ARABIC_SUPPORT = FALSE                            ### ARABIC SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_THAI_SUPPORT = FALSE                              ### THAI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_VIETNAMESE_SUPPORT = FALSE                        ### VIETNAMESE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_PORTUGUESE_SUPPORT = FALSE                        ### PORTUGUESE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_RUSSIAN_SUPPORT = FALSE                           ### RUSSIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_HINDI_SUPPORT = FALSE                             ### HINDI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_INDONESIAN_SUPPORT = FALSE                        ### INDONESIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_GREEK_SUPPORT = FALSE                             ### GREEK SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_PERSIAN_SUPPORT = FALSE                           ### PERSIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_MALAY_SUPPORT = FALSE                             ### MALAY SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_FRENCH_SUPPORT = FALSE                            ### FRENCH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_TAGALOG_SUPPORT = FALSE                           ### TAGALOG SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_URDU_SUPPORT = FALSE                              ### URDU SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_HUNGARIAN_SUPPORT = FALSE                         ### HUNGARIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ITALIAN_SUPPORT = FALSE                           ### ITALIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_TURKISH_SUPPORT = FALSE                           ### TURKISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SPANISH_SUPPORT = FALSE                           ### SPANISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_GERMAN_SUPPORT = FALSE                            ### GERMAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ROMANIAN_SUPPORT = FALSE                          ### ROMANIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SLOVENIAN_SUPPORT = FALSE                         ### SLOVENIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_HEBREW_SUPPORT = FALSE                            ### HEBREW SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_BENGALI_SUPPORT = FALSE                           ### BENGALI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_CZECH_SUPPORT = FALSE                             ### CZECH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_DUTCH_SUPPORT = FALSE                             ### DUTCH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_TELUGU_SUPPORT = FALSE                            ### TELUGU SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_MARATHI_SUPPORT = FALSE                           ### MARATHI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_TAMIL_SUPPORT = FALSE                             ### TAMIL SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_GUJARATI_SUPPORT = FALSE                          ### GUJARATI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_KANNADA_SUPPORT = FALSE                           ### KANNADA SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_MALAYALAM_SUPPORT = FALSE                         ### MALAYALAM SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ORIYA_SUPPORT = FALSE                             ### ORIYA SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_PUNJABI_SUPPORT = FALSE                           ### PUNJABI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_AFRIKAANS_SUPPORT = FALSE                         ### AFRIKAANS SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ALBANIAN_SUPPORT = FALSE                          ### ALBANIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ARMENIAN_SUPPORT = FALSE                          ### ARMENIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_AZERBAIJANI_SUPPORT = FALSE                       ### AZERBAIJANI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_BASQUE_SUPPORT = FALSE                            ### BASQUE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_BULGARIAN_SUPPORT = FALSE                         ### BULGARIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_CATALAN_SUPPORT = FALSE                           ### CATALAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_CROATIAN_SUPPORT = FALSE                          ### CROATIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_DANISH_SUPPORT = FALSE                            ### DANISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ESTONIAN_SUPPORT = FALSE                          ### ESTONIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_FILIPINO_SUPPORT = FALSE                          ### FILIPINO SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_FINNISH_SUPPORT = FALSE                           ### FINNISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_GALICIAN_SUPPORT = FALSE                          ### GALICIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_GEORGIAN_SUPPORT = FALSE                          ### GEORGIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_HAUSA_SUPPORT = FALSE                             ### HAUSA SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ICELANDIC_SUPPORT = FALSE                         ### ICELANDIC SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_IGBO_SUPPORT = FALSE                              ### IGBO SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_IRISH_SUPPORT = FALSE                             ### IRISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_KAZAKH_SUPPORT = FALSE                            ### KAZAKH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_LATVIAN_SUPPORT = FALSE                           ### LATVIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_LITHUANIAN_SUPPORT = FALSE                        ### LITHUANIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_MACEDONIAN_SUPPORT = FALSE                        ### MACEDONIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_MOLDOVAN_SUPPORT = FALSE                          ### MOLDOVAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_NORWEGIAN_SUPPORT = FALSE                         ### NORWEGIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_POLISH_SUPPORT = FALSE                            ### POLISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SERBIAN_SUPPORT = FALSE                           ### SERBIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SESOTHO_SUPPORT = FALSE                           ### SESOTHO SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SLOVAK_SUPPORT = FALSE                            ### SLOVAK SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SWEDISH_SUPPORT = FALSE                           ### SWEDISH SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_UKRAINIAN_SUPPORT = FALSE                         ### UKRAINIAN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_YORUBA_SUPPORT = FALSE                            ### YORUBA SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_XHOSA_SUPPORT = FALSE                             ### XHOSA_ SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ZULU_SUPPORT = FALSE                              ### ZULU SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_ASSAMESE_SUPPORT = FALSE                          ### ASSAMESE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IM_SWAHILI_SUPPORT = FALSE                           ### SWAHILI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
IM_MYANMAR_SUPPORT = FALSE                           ### MYANMAR SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
IM_AMHARIC_SUPPORT = FALSE                           ### AMHARIC SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                        						 
IM_KHMER_SUPPORT = FALSE                             ### KHMER SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE 

IM_LAO_SUPPORT = FALSE                               ### LAO SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                                                                                                                                                  
 
IM_UYGHUR_SUPPORT = FALSE                            ###UYGHUR SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
  
MMI_3DUI_SUPPORT = TRUE                              ### algorithm collection(sin cos)
                                                     # Option1:TRUE
                                                     # Option2:FALSE   
                                                                                             
                        
WEATHER_INFORMATION_BUFFER_STORE_SUPPORT = FALSE      ### BUFFER STORE
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                        
CM_SUPPORT  = FALSE                                  ### Collision special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE  
                                                     
SE_RIPPLE_SUPPORT = FALSE                            ### Ripple special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE  
                                                     
SE_FIRE_SUPPORT = FALSE                              ### Fire special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE  
                                                     
SE_VISUAL_SUPPORT = FALSE                            ### Visual special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE  
                                                     
SE_PAGE_TURN_SUPPORT = FALSE                          ### Page turn special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE 
                                                     
SE_ROLL_SUPPORT = TRUE                               ### Roll special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE 
                                                     
SE_SOLID_SUPPORT = FALSE                              ### Solid special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE 
                                                     
SE_GRID_SUPPORT = TRUE                               ### Grid special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE 
                                                     
SE_LEAF_SUPPORT = FALSE                              ### Leaf special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE  
                                                                                                                          
SE_CRT_SUPPORT = FALSE                               ### CRT special effect Type:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE   
                                                     
ANALOG_CLK_SUPPORT = FALSE                           ### analog clock support:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE  
                                                     
ROTATE_2D_SUPPORT = TRUE                             ### Arbitrary rotate support:
                                                     # Option0:TRUE     
                                                     # Option1:FALSE 
                                                     
MMI_POWER_SAVING_MODE_SUPPORT = TRUE                 ### Power Saving mode Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
PUSH_EMAIL_SUPPORT = FALSE                           ### PUSH MAIL SUPPORT 
                                                     # Option2:FALSE
                                                     
PUSH_EMAIL_3_IN_1_SUPPORT = FALSE                    ### PUSH EMAIL 3_IN_1 SUPPORT
                                                     # Option2:FALSE                                                                                            
                                                     
MTV_MIDDLEWARE_ON_BB = FALSE                         ### MTV Middleware deployed on BB:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
DEMOD_HW = NONE                                      ### demod chip type:
                                                     # Option1:NONE      
                                                     # Option1:SIANO      DEMOD_HW_SIANO
                                                     # Option2:INNOFIDEI  DEMOD_HW_INNOFIDEI
                                                     
                                                     
OPERA_MOBILE_SUPPORT = NONE                          #opera mobile support
                                                     # Option1:V11      OPERA_MOBILE_SUPPORT_V11
                                                     # Option2:NONE     OPERA_MOBILE_SUPPORT_NONE
                                                     
OPERA_MINI_SUPPORT = NONE                            #opera mini support
                                                     # Option1:VER6     OPERA_MINI_SUPPORT_VER6
                                                     # Option2:VER42    OPERA_MINI_SUPPORT_VER42
                                                     # Option3:NONE     OPERA_MINI_SUPPORT_NONE   
                                                     
BROWSER_ALL_RUNNING_SUPPORT = FALSE                  ### Browser running at the same time Supplier:
                                                     # Option1:TRUE  
                                                     # Option2:FALSE(default)     
                                                     
MEMORY_DEBUG_SUPPORT = TRUE                          ### MEMORY DEBUG SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
TRACE_INFO_SUPPORT = TRUE                            ### TRACE INFO SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                                                                                                     
                                                     
                                                     
TRACE_TYPE = ID                                      ### TRACE TYPE SUPPORT:
                                                     # Option1:CLASSIC
                                                     # Option2:ID                                                                                                                      
                                                     
PIC_CROP_SUPPORT = FALSE                              ### Picture crop support
                                                     # Option1:TRUE  
                                                     # Option2:FALSE
                                                     
PDA_UI_DROPDOWN_WIN = TRUE                           ### Platform Support dropdown window Style MMI
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MMIEMAIL_SUPPORT = FALSE                             ### EMAIL Client Need Browser not set NONE
                                                     # Option1:TRUE  
                                                     # Option2:FALSE
                                                     
MMI_TASK_MANAGER = TRUE                              ### Task manager
                                                     # Option1:TRUE  
                                                     # Option2:FALSE
                                                     
MMI_MEMO_SUPPORT = FALSE                              ### Memo
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
QBTHEME_SUPPORT = TRUE                               ### qb theme support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IKEYBACKUP_SUPPORT = FALSE                           ### 1-key backup Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MMS_SMS_IN_1_SUPPORT = FALSE                          ### MMS SMS IN 1 Support:
                                                     # Option1:TRUE(default)                                                                             
                                                     # Option2:FALSE
                                                     
UZONE_SUPPORT = NONE                                 ### UZONE: 
                                                     # Option1: BOOKRACK      #define UZONE_SUPPORT_BOOKRACK
                                                     # Option2: SINGLE        #define UZONE_SUPPORT_SINGLE
                                                     # Option3: GLASS         #define UZONE_SUPPORT_GLASS
                                                     # Option3: NONE          Not suppport 

SMS_AUTO_SIGNATURE_SUPPORT = FALSE                   ### sms auto signature Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                      	 						 
DATA_ROAMING_SUPPORT = FALSE                         ### data roam Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                      	 						 
SNS_SUPPORT = FALSE                                  ### sns support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MMI_GUI_STYLE_TYPICAL = FALSE                        ### GUI STYLE
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
JS_SUPPORT = FALSE                                   ### Java script support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                    
CHATMODE_ONLY_SUPPORT = FALSE                        ### CHATMODE_ONLY_SUPPORT Support:
                                                     # Option1:TRUE(default)
                                                     # Option2:FALSE
                                                     
SMS_CHAT_SUPPORT = FALSE                              ### CHATMODE SUPPORT Support:
                                                     # Option1:TRUE(default)
                                                     # Option2:FALSE

MMI_FUNC_COMPATIBLE_SUPPORT = TRUE                  ### MMI Old Export Function Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

PDA_UI_SUPPORT_MANIMENU_GO =  TRUE                   ### PDA UI SUPPORT main menu :
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                        
                                       
PDA_UI_MAINMENU_SUPPORT_SELECT_ANIM = FALSE          ## MAINMENU_SUPPORT_SELECT_ANIM
                                                     # Option1:TRUE   ANIM  SELECT
                                                     # Option2:FALSE   STATIC SELECT

U_CAMERA_SUPPORT = FALSE                              ### MORE CAMERA SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
IMG_EDITER_SUPPORT = FALSE                            ### Image Editer Support(u_camera/img_editer):
                                                     # Option1:TRUE
                                                     # Option2:FALSE

CAMERA_SUPPORT = TRUE                                ### CAMERA Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                      
EBOOK_SUPPORT = TRUE                                 ### EBOOK Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
CALCULATOR_SUPPORT = TRUE                            ### CALCULATOR Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SYNCTOOL_SUPPORT = TRUE                              ### Synctool Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WORLD_CLOCK_SUPPORT = TRUE                           ### World Clock Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SEARCH_SUPPORT = TRUE                                ### Local Search Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
COUNTEDTIME_SUPPORT = TRUE                           ### counted time Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
STOPWATCH_SUPPORT = TRUE                             ### stopwatch Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MMI_UNITCONVERSION_SUPPORT = TRUE                    ### unitconversion Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                                                                                                                                                                                                                                                                                                                      
GAME_SUPPORT = TRUE                                  ### game Support:
                                                     # Option1:TRUE                                                     
                                                     # Option2:FALSE

LIVE_WALLPAPER_FRAMEWORK_SUPPORT = TRUE              ###LIVE WALLPAPER
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
LIGHTGRID_WALLPAPER_SUPPORT = ROM                   ### Live WallPaper LightGrid Effect
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                     
LINEAR_WALLPAPER_SUPPORT = FALSE                     ### Live WallPaper Linear Effect
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                                                                          
GALAXY_WALLPAPER_SUPPORT = FALSE                     ### Live WallPaper Galaxy Effect
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                     
COLLISION_WALLPAPER_SUPPORT = FALSE                   ### Live WallPaper Collision
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                     
COLLISION_BALLOON_WALLPAPER_SUPPORT = FALSE           ### Live WallPaper Collision
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                     
COLLISION_MENU_WALLPAPER_SUPPORT = FALSE             ### Live WallPaper Collision
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE                                                                                                                                                              

POP_WALLPAPER_SUPPORT = FALSE                         ### Live WallPaper Pop
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE  

BEE_WALLPAPER_SUPPORT = FALSE                         ### Live WallPaper Bee
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE

SAKURA_WALLPAPER_SUPPORT = FALSE                     ### Live WallPaper Sakura
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE 
                                                     
SEAWORLD_WALLPAPER_SUPPORT = FALSE                   ### Live WallPaper seaworld
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE 
                                                     
RIPPLE_WALLPAPER_SUPPORT = FALSE                     ### Live WallPaper Ripple
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE 
                                                     
LITTLECAT_WALLPAPER_SUPPORT = FALSE                  ### Live WallPaper Littlecat
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE  
                                                     
WINDMILL_WALLPAPER_SUPPORT = ROM                   ### Live WallPaper Windmill
                                                      # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE

RAINBOWCITY_WALLPAPER_SUPPORT = FALSE                ### Live WallPaper Rainbowcity
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                     
PAPERTOWN_WALLPAPER_SUPPORT = FALSE                  ### Live WallPaper Papertown
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE

AQUARIUM_WALLPAPER_SUPPORT = FALSE                   ### Live WallPaper AQuarium
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE

GIRLSWING_WALLPAPER_SUPPORT = FALSE                  ### Live WallPaper girlswing
                                                     # Option1:ROM
                                                     # Option2:TCARD
                                                     # Option2:FALSE
                                                     
MMI_CHECK_MEMINFO_SUPPORT = FALSE                    ### Check Memory Overrun Info Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

WRE_SUPPORT = FALSE                                  ### Mwin support
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                    

WRE_VIRTUALDISK_SUPPORT = FALSE                      ### wre Virtual Disk Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

NES_SUPPORT = FALSE                                  ### Mwin support
                                                     # Option1:TRUE
                                                     # Option2:FALSE      
                                                     
WRE_FACEBOOK_SUPPORT = FALSE                         ### Wre facebook Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WRE_WEIBO_SUPPORT = FALSE                            ### Wre weibo Support
                                                     # Option1:TRUE                                                                                                          
                                                     # Option2:FALSE
                                                     
WRE_TWITTER_SUPPORT = FALSE                          ### Wre twitter Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

WRE_CONTRA_SUPPORT = FALSE                           ### Wre twitter Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WRE_SUPERMARIO_SUPPORT = FALSE                       ### Wre twitter Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

WRE_TSLAND_SUPPORT = FALSE                           ### Wre twitter Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MET_MEX_SUPPORT = FALSE                              ### MET MEX SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MET_MEX_MERIG_SUPPORT = FALSE                        ### MET MEX MERIGSUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                                
MET_MEX_QQ_SUPPORT = FALSE                           ### MET MEX SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                        
MET_MEX_TB_SUPPORT = FALSE                           ### MET MEX SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MET_MEX_ZT_SUPPORT = FALSE                           ### MET MEX SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_MEX_AB_SUPPORT = FALSE                           ### MET MEX AngryBirds SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_MEX_MSN_SUPPORT = FALSE                          ### MET MEX MSN SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_MEX_FETION_SUPPORT = FALSE                       ### MET MEX Fetion SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
  
MET_MEX_AQ_SUPPORT = FALSE                           ### MET_MEX_AQ_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_MEX_DOUBLE_QQ_SUPPORT = FALSE                    ### MET_MEX_DOUBLE_QQ_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_PLAT_CMOTION_SUPPORT = FALSE                     ### MET_PALT_CMOTION_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MEX_G7_UI_SUPPORT = FALSE                            ### MET MEX FAUI SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MET_MEX_CQZ_SUPPORT = TRUE                           ### MET_MEX_CQZ_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MET_MEX_JSCB_SUPPORT = FALSE                         ### MET_MEX_JSCB_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MET_MEX_TOMCAT_SUPPORT = TRUE                        ### MET MEX TOMCAT SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_MEX_WEIBO_SUPPORT = FALSE                        ### MET MEX SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MET_MEX_WOMIVIDEO_SUPPORT = FALSE                    ### MET MEX SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MRAPP_SUPPORT = TRUE                                 ### MRAPP_SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
MRAPP_OVERSEA_SUPPORT = TRUE    			 ### MRAPP_OVERSEA_SUPPORT:
                                                         			# Option1:TRUE
						     					# Option2:FALSE
ifeq ($(strip $(MRAPP_SUPPORT)), TRUE)
__MMI_SKYQQ__   = FALSE   			     		### __MMI_SKYQQ__:
                                                     				# Option1:TRUE
						     					# Option2:FALSE
endif		
#DSM 增值菜单
ifeq ($(strip $(MRAPP_OVERSEA_SUPPORT)), TRUE)
DMENU_SUPPORT =TRUE     						### DMENU_SUPPORT:
                        								# Option1:TRUE
											# Option2:FALSE
endif    

SXH_APPLE_SUPPORT = FALSE                            ### SXH APPLE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

AUTODEMO_SUPPORT = FALSE                             ### AUTODEMO_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
# *************************************************************************
# Feature Options FOR SPRD
# !!!!!!!! Attention:please don't modify the customer value of this option part. !!!!!!!!!
# !!!!!!!! If modification is in need, the new version should be released by SPRD. !!!!!!!!!
# !!!!!!!! 注意: 此部分Options 的取值客户请勿修改!!!!!!!!!
# !!!!!!!! 若需要修改，需展讯重新发布版本!!!!!!!!!
# *************************************************************************

SPRD_CUSTOMER = TRUE                                ### SPRD Customer 
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
                                                     
PLATFORM = SC6530                                    ### Chip Platform Definition:
                                                     # Option1:SC6530   #define PLATFORM_SC6530
                                                     
CHIP_VER = 6530                                      ### Chip version:
                                                     # Option1:6800H1     #define CHIP_VER_6800H1
                                                     # Option2:6800H3     #define CHIP_VER_6800H3
                                                     
FPGA_SUPPORT = FALSE                                 ### FPGA_SUPPORT_SC6530
                                                     
FPGA_VERIFICATION = FALSE                            ### Debug Version
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
CHIP_MARK = NONE                                     ### Chip Mark Number:
                                                     # Option1:NONE       #define CHIP_MARK_NONE
                                                     
CHIP_ENDIAN = LITTLE                                 ### Chip endianess:
                                                     # Option1: BIG
                                                     # Option2: LITTLE
                                                     
COMPILER = RVCT                                      ### Compiler version
                                                     # Option1:ADS  ( ADS v1.2 )
                                                     # Option2:RVCT ( RVCT v2.2, RVCT v3.1 )
                                                     
RVCT_VERSION = RVCT31                                ### RVCT version
                                                     # Option1:NONE
                                                     # Option2:ADS12
                                                     # Option3:RVCT22
                                                     # Option4:RVCT31 
                                                                                         
NANDBOOT_SUPPORT = FALSE                             ### Boot From Nandflash Support: 
                                                     # Option1:TRUE
                                                     # Option2:FALSE	
                                                     
                                                                                         		 
POWER_OFF_PROTECT_SUPPORT = TRUE                     ### Switch power off protect support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
USB_VCOM_AUTORUN_SUPPORT = FALSE                     ### support to instacll UCOM drivre and ewalk software:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                                                                          
RF_TYPE = SR528                                      ### RF Chip Type:
                                                     # Option1:QUORUM        #define RF_TYPE_QUORUM
                                                     # Option2:QUORUM518 	#define RF_TYPE_QUORUM518
                                                     # Option2:SR528	        #define RF_TYPE_SR528
                                                     
SBC_DSP_SUPPORT = VERSION1                           ### dsp support sbc or mp3-sbc. VERSION1:sbc,VERSION2:sbc-mp3
                                                     # Option1:VERSION1      #define SBC_DSP_SUPPORT_VERSION1
                                                     # Option2:VERSION2	#define SBC_DSP_SUPPORT_VERSION2
                                                     
SBC_OVERLAY = FALSE                                  ### implement sbc with memory overlay  HR L = false, M = TRUE, default = FALSE
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
DOLPHIN_SUPPORT = TRUE                               ### Switch Of dolphin device Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
EXPRESS_PROCESS_IN_DSP = TRUE                        ### Switch Of express process(transamplerate and sbc encode) in dsp 
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
HALFRATE_SUPPORT = TRUE                              ### halfrate support feature
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ATC_SUPPORT	= FULL                                   ### ATC support feature
                                                     # Option1:FULL
                                                     # Option2:COMPACT
                                                     
                                                     
PRODUCT_DM = FALSE                                   ### TD/GSM DualModule product
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
GPRSMPDP_SUPPORT = TRUE                              ### GPRS Multi PDP support
                                                     # Option1:TRUE
                                                     
VM_SUPPORT    = TRUE                                 ### VM support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
BZP_SUPPORT  = TRUE                                  ###new memory management,now only be used in 128X32 Nor 
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
FDN_SUPPORT = TRUE                                   ### FDN Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
UI_MULTILAYER_SUPPORT = TRUE                         ### ui layer support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MSDC_CARD_SUPPORT = TRUE                             ### SD Card Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
USB_VCOM_SUPPORT = TRUE                              ### USB Virtual COM Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
USB_SUPPORT = TRUE                                   ### USB TXer Type: 
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
TCPIP_SUPPORT = TRUE                                 ### TCPIP Profile Support:
                                                     # Option1:TRUE
                                                     
SSL_SUPPORT = TRUE                                   ### SSL Profile Support:
                                                     # Option1:FALSE
                                                     
HTTP_SUPPORT = TRUE                                  ### HTTP Profile Support:
                                                     # Option1:FALSE
                                                     
RTP_SUPPORT = FALSE                                  ### RTP Support:
                                                     # Option1:FALSE
                                                     # Option2:TRUE                                        
                                                     
RTSP_SUPPORT = FALSE                                 ### RTSP Support:
                                                     # Option1:FALSE
                                                     # Option2:TRUE
                                                     
WSP_SUPPORT = TRUE                                   ### WSP Profile Support:
                                                     # Option1:FALSE
                                                     
XML_SUPPORT = TRUE                                   ### EXPAT XML Support:
                                                     # Option1:FALSE
                                                     
OMADL_SUPPORT = FALSE                                ### OMA download Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     # work for XML_SUPPORT = TRUE && HTTP_SUPPORT = TRUE
                                                     
GPRS_SUPPORT = TRUE                                  ### GRPS Support:该宏的取值用户不能修改
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
DRM_SUPPORT = NONE                                   ### DRM Support:ONLY VALIDATE WHEN MMI_AUTO_UPDATE_TIME_SUPPORT SET TRUE 
                                                     # Option0:NONE 
                                                     # Option1:V1 

STACK_CHECK_SUPPORT = FALSE                          ### STACK CHECK SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
CARD_LOG = FALSE                                      ### CARD SAVE LOG SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
SPRD_RAW_CARD_LOG = FALSE                            ### T-CARD SAVE LOG SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
UMEM_SUPPORT = TRUE                                  ### Switch Of nor flash udisk support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
UDISK_PC_CONNECT = TRUE                              ### Norflash disk pc access support UMEM_SUPPORT MUST set TRUE
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                       
                                                     
MULTI_SIM_SYS = TRI                                 ### Platform Support Multi Sim Card Number:
                                                     # Option1:SINGLE    #define MULTI_SIM_SYS_SINGLE
                                                     # Option2:DUAL      #define MULTI_SIM_SYS_DUAL
                                                     # Option3:TRI       #define MULTI_SIM_SYS_TRI
                                                     # Option4:QUAD      #define MULTI_SIM_SYS_QUAD
                                                     
JTAG_SUPPORT = FALSE                                  ### JTAG Debug Version:
                                                     # Option1: TRUE
                                                     # Option2: FALSE
                                                     
SOUNDBANK_VER = NEWMIDI_75KB                         ### Soundbank Talbe Type
                                                     # Option1:ARM_512KB #define SOUNDBANK_VER_ARM_512KB
                                                     # Option2:ARM_2MB   #define SOUNDBANK_VER_ARM_2MB
                                                     # Option3:ARM_256KB #define SOUNDBANK_VER_ARM_256KB
                                                     # Option4:ARM_700KB #define SOUNDBANK_VER_ARM_700KB
                                                     # Option5:ARM_300KB #define SOUNDBANK_VER_ARM_300KB
                                                     # Option6:NEWMIDI_75KB #define SOUNDBANK_VER_NEWMIDI_75KB
                                                     # Option7:NEWMIDI_170KB #define SOUNDBANK_VER_NEWMIDI170KB
                                                     # Option8:NEWMIDI_400KB #define SOUNDBANK_VER_NEWMIDI_400KB                                        
                                                                   
BT_BQB_SUPPORT = FALSE                               ### BQB SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                  
BT_OPP_SUPPORT = TRUE                                ### Switch Of OPP Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
BT_FTS_SUPPORT = FALSE                               ### Switch Of FTP Server Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
BT_DUN_SUPPORT = FALSE                               ### Switch Of DUN Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
BT_SPP_SUPPORT = FALSE                               ### Switch Of SPP Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
BT_HFG_SUPPORT = TRUE                                ### Switch Of Hand Free Gateway Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
BT_A2DP_SUPPORT = FALSE                              ### Switch Of A2DP Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

BT_A2DP_SAMPLERATE_RECONFIG_SUPPORT = FALSE	         ### a2dp sample rate reconfig Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

MP3_A2DP_SUPPORT = TRUE                              ### MP3 Support Bluetooth Stereo:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WAV_A2DP_SUPPORT = TRUE                              ### WAV Support Bluetooth Stereo:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AMR_A2DP_SUPPORT = TRUE                              ### AMR_DSP Support Bluetooth Stereo:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MIDI_A2DP_SUPPORT = TRUE                             ### MIDI Support Bluetooth Stereo:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AAC_A2DP_SUPPORT = TRUE                              ### AAC Support Bluetooth Stereo:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WMA_A2DP_SUPPORT = FALSE                             ### WMA Support Bluetooth Stereo:
                                                     # Option1:FALSE
                                                     
MP3_SUPPORT = TRUE                                   ### MP3 Decoder Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AAC_SUPPORT = TRUE                                   ### AAC Decoder Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WMA_SUPPORT = FALSE                                  ### WMA Decoder Support:
                                                     # Option1:FALSE
                                                     
AMR_SUPPORT = TRUE                                   ### AMR Decoder Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MIDI_SUPPORT = TRUE                                  ### MIDI Decoder Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                                            
MIC_SAMPLE_SUPPORT = TRUE                            ### MIC SAMPLE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE       
                                                                                                                             
CAT_MVOICE_SUPPORT = FALSE                           ### CAT_MVOICE_SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE     
                                                     
H264_DEC_SUPPORT = TRUE                              ### H264 Decoder Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                        
                                                     
WAV_SUPPORT = TRUE                                   ### WAV Decoder Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MP3_BACKGROUND_SUPPORT = TRUE                        ### MP3 BACKGROUND SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WMA_BACKGROUND_SUPPORT = FALSE                       ### WMA BACKGROUND SUPPORT
                                                     # Option1:FALSE
                                                     
AAC_BACKGROUND_SUPPORT = TRUE                        ### AAC BACKGROUND SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AMR_BACKGROUND_SUPPORT = TRUE                        ### AMR BACKGROUND SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MIDI_BACKGROUND_SUPPORT = TRUE                       ### MIDI BACKGROUND SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WAV_BACKGROUND_SUPPORT = TRUE                        ### WAV BACKGROUND SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MP3_EQ_SUPPORT = TRUE                                ### MP3 EQ Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
EQ_EXE_ON_CHIP = TRUE                                ### EQ Type:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
MP3_SEEK_SUPPORT = TRUE                              ### MP3 Seek Function Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AAC_SEEK_SUPPORT = TRUE                              ### AAC Seek Function Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WMA_SEEK_SUPPORT = FALSE                             ### WMA Seek Function Support:
                                                     # Option1:FALSE
                                                     
AMR_SEEK_SUPPORT = TRUE                              ### AMR Seek Function Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
MIDI_SEEK_SUPPORT = TRUE                             ### MIDI Seek Function Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
WAV_SEEK_SUPPORT = TRUE                              ### WAV Seek Function Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
GIF_DEC_SUPPORT = GIF_SW                             ### GIF Decoder Lib Type:
                                                     # Option0:NONE     #define GIF_DEC_SUPPORT_NONE
                                                     # Option1:GIF_SW   #define GIF_DEC_SUPPORT_GIF_SW
                                                     # Option2:GIF_HW   #define GIF_DEC_SUPPORT_GIF_HW
                                                     
PNG_DEC_SUPPORT = PNG_SW                             ### PNG Decoder Lib Type:
                                                     # Option0:NONE     #define PNG_DEC_SUPPORT_NONE
                                                     # Option1:PNG_SW   #define PNG_DEC_SUPPORT_PNG_SW
                                                     
BMP_DEC_SUPPORT = BMP_SW                             ### BMP Decoder Lib Type:
                                                     # Option0:NONE     #define BMP_DEC_SUPPORT_NONE
                                                     # Option1:BMP_SW   #define BMP_DEC_SUPPORT_BMP_SW
                                                     
JPG_DEC_SUPPORT = JPG_HW                             ### JPG Decoder Lib Type:
                                                     # Option0:NONE     #define JPG_DEC_SUPPORT_NONE
                                                     # Option1:JPG_SW   #define JPG_DEC_SUPPORT_JPG_SW
                                                     # Option2:JPG_HW   #define JPG_DEC_SUPPORT_JPG_HW
                                        
JPG_DEC_TYPE_SUPPORT = BASELINE_PROGRESSIVE          ### decode type support:
                                                     # Option0:BASELINE
                                                     # Option1:BASELINE_PROGRESSIVE  
                                        

JPG_ENC_SUPPORT = JPG_HW                             ### JPG Encoder Lib Type:
                                                     # Option0:NONE     #define JPG_ENC_SUPPORT_NONE
                                                     # Option1:JPG_SW   #define JPG_ENC_SUPPORT_JPG_SW
                                                     # Option2:JPG_HW   #define JPG_ENC_SUPPORT_JPG_HW 
                                                     
WMA_DEC_SUPPORT = FALSE                              ### WMA Decoder Lib Support:
                                                     # Option1:FALSE
                                                     
AVI_DEC_SUPPORT = TRUE                               ### AVI Decoder Lib Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
FLV_DEC_SUPPORT = TRUE                               ### FLV Decoder Lib Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AVI_ENC_SUPPORT = TRUE                               ### AVI Encoder Lib Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
TMVB_DEC_SUPPORT = FALSE                             ### Switch RMVB video Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE     
                                                     
TA8LBR_SUPPORT = FALSE                               ### Switch RA8 Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
GZIP_DEC_SUPPORT = GZIP_SW                           ### GZIP Decoder Lib Type:
                                                     # Option0:NONE     #define GZIP_DEC_SUPPORT_NONE
                                                     # Option1:GZIP_SW  #define GZIP_DEC_SUPPORT_GZIP_SW      
                                                     
NULL_CODEC_SUPPORT = TRUE                            ### NULL_CODEC  Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ARM_AD_ADP_SUPPORT = TRUE                            ### ARM_AD_ADP CODEC Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
AD_OUTPUT_ADP_SUPPORT = TRUE                         ### AD_OUTPUT_ADP CODEC Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
DYNAMIC_MODULE_SUPPORT = FALSE                       ### caf Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE	
                                                     
AUDIO_PARAM_ARM = TRUE                               #### move audio param from dsp audio nv to arm audio nv
                                                     
DUAL_TCARD_SUPPORT = FALSE                           ### 6800H openphone Dual TCards SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE    
                                                        
STRM_SUPPORT_REDUCE_MEMORY = TRUE                    ### To reduce memory for low memory system
                                                     # Option1:TRUE
                                                     # Option2:FALSE  
                                                     
NEW_MEMORY_MM = TRUE                                 ###new memory management，now only be used in 128X32 Nor 
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                       
STRM_SUPPORT_LOW_MEMORY = BUFFER                     ### streaming low memory type
                                                     # Option1:BOTH
                                                     # Option2:BUFFER     #define STRM_SUPPORT_LOW_MEMORY_BUFFER
                                                     # Option3:CONFLICT   #define STRM_SUPPORT_LOW_MEMORY_CONFLICT
                                                     # Option4:NONE
                                                     
TF_LOAD_SUPPORT = FALSE                              ### TF UPDATE SUPPORT:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
TF_LOAD_PROTECT = FALSE                              ### TF UPDATE SUPPORT:
                                                     # Option1:TRUE   TF BOOTLOADER will put in first sector
                                                     # Option2:FALSE  TF BOOTLOADER WILL load in TF CARD
                                                     
STONE_IMAGE_SUPPORT = TRUE                           ###Merge PS and resource to one image
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
DSP_HW_COMPRESS = TRUE                              ### DSP HW Compress       
                                                     # Option1:TRUE
                                                     # Option2:FALSE 

WIFI_VENDOR = NONE                                    ### wifi vendor
                                                     # Option1:NONE
                                                     # Option2:CSR
                                                     # Option3:ATHEROS
                                                     
MMI_BT_MID_QUALITY = MMI_BT_HIGH_QUALITY             ### A2DP  Quality Option:
                                                     # Option1:MMI_BT_HIGH_QUALITY   #select this option, the Bluetooth A2DP default sample rate will be set 48K
                                                     # Option2:MMI_BT_MID_QUALITY  #select this option, the Bluetooth A2DP  default sample rate will be set 36K
                                                     # Option3:MMI_BT_LOW_QUALITY  #select this option, the Bluetooth A2DP default sample rate will be set 16K
                                                     
SMART_PAGING_SUPPORT = FALSE                         ### Smart Paging Support Option:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ORANGE_SUPPORT = FALSE                               ### Orange Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                                                          
NXP_LVVE_SUPPORT = FALSE                              ### NXP Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE 

NV_PRTITION = NORMAL                                 ### NV Prtition 
                                                     # Option1:TINY
                                                     # Option2:SMALL
                                                     # Option3:NORMAL
                                                     # Option4:LARGE
                                                                                                                                                                                                                   
#****************************************************************************************#
#  Segment: SPRD MACRO
#  Description: Get all sprd macro list 
#****************************************************************************************#
SPRD_MACRO_LIST := $(call GET_SPRD_MACRO_LIST)
##########################################################################################
#  OLD MACRO OR NO USE 
#   
##########################################################################################
ASSERT_TYPE_SUPPORT = ASSERT                         ### ASSERT TYPE SUPPORT
                                                     # Option1:ASSERT
                                                     # Option2:WARNING   
                                                                                 
NOR_SINGLEBANK_SUPPORT = FALSE                       ### Support Single Bank Type Norflash:
                                                     # Option1:FALSE
                                                     # Option1:TRUE     
                                                                                      
KURO_SUPPORT = FALSE                                 ### KURO Player Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
LOW_MEMORY_SUPPORT = ESTIMATE                        ### Low Memory Size Requirement:
                                                     # Option1:256X64
                                                     # Option1:128X64
                                                     # Option2:128X32
                                                     # Option3:64x64
                                                     # Option4:32x64
                                                     # Option5:16x16
                                                     # Option6:ESTIMATE
                                                     # Option6:NONE
                                                     
CSC_SUPPORT = FALSE                                  ### Customer Specific Customization support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
CSC_XML_SUPPORT = FALSE                              ### Customer Specific Customization XML File:
                                                     # Option1:TRUE
                                                     # Option2:FALSE                                   