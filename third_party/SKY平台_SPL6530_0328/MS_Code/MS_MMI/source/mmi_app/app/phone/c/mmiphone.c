/*****************************************************************************
** File Name:      mmiphone.c                                                *
** Author:                                                                   *
** Date:           12/2003                                                   *
** Copyright:      2003 Spreadtrum, Incorporated. All Rights Reserved.         *
** Description:    This file is used to describe phone module                *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 12/2003       Jassmine       Create
** 03/2004       Jassmine.Meng     Modify
******************************************************************************/

#define _MMIPHONE_C_



/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "mmi_app_phone_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmiphone_export.h"
#include "mmiphone_internal.h"
#include "mmk_app.h"
#include "mmk_timer.h"
#include "tb_dal.h"
#include "window_parse.h"
#include "mmi_image.h"
#include "mmi_text.h"
#include "mmisms_text.h"
#include "mmiidle_export.h"
#include "mmi_common.h"
#include "mmi_nv.h"
#include "mmidisplay_data.h"
#include "mmicl_export.h"
#include "mmi_default.h"
#include "mmialarm_export.h"
#include "mmiset_export.h"
#include "mmiacc.h"
#include "guilcd.h"
//#include "guilabel.h"
#include "guilistbox.h"
#include "guianim.h"
#include "dal_chr.h"
#include "mmiudisk_export.h"
#include "mmi_appmsg.h"
#include "mmimms_export.h"
#include "mmimp3_export.h"
#include "mmiacc.h"
#include "mmirecord_export.h"
#include "mmi_imagefun.h"
#if defined(MMI_MSD_SUPPORT)
#include "mmimsd_export.h"
#endif
#include "mmi_textfun.h"
#include "mmicc_export.h"
#include "mmicc_id.h"
#include "mn_api.h"
#include "mmiidle_export.h"
#include "sig_code.h"
#include "dal_power.h"
#include "mmi_wallpaper_export.h"
#include "mmiengtd_export.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#endif
#ifdef WIN32
#include "mmiudisk_simu.h"
#else
#include "usbservice_api.h"
#include "gpio_prod_api.h"
#endif
#include "sci_api.h"
#include "mmipub.h"
#include "mmiidle_subwintab.h"
#include "mmisd_export.h"
#ifdef AOC_SUPPORT
#include "mmicl_costs_wintab.h"
#endif
#include "mmifmm_export.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif

#ifdef TOUCH_PANEL_SUPPORT
#include "tp_export.h"
#endif
//#include "mmieng.h"
#include "mmieng_export.h"
#include "mn.h"
#include "mmifm_export.h"
#include "mmivcard_export.h"
#include "mmiset_nv.h"
#include "mn_type.h"
//macro replace
//#ifdef MMI_STREAMING_ENABLE 
#ifdef STREAMING_SUPPORT_NEXTREAMING 
#include "mmist_id.h"
#endif
#ifndef MMI_MULTI_SIM_SYS_SINGLE
#include "mmiset_multisim_export.h"
#endif
#include "mn_api.h"

#include "mmipb_export.h"
#include "mmidc_export.h"
//#include "mmicountedtime_export.h"
#include "mmikur.h"
#include "mmisms_export.h"
#if defined(MMI_SMSCB_SUPPORT)
#include "mmismscb_export.h"
#endif
#include "guiedit.h"
#include "guires.h"
#include "mmipicview_export.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#ifndef WIN32
#ifndef MMI_DUALMODE_ENABLE
#include "atc_ppp.h"/*lint -e766*/
#endif
#endif
#include "mmistk_export.h"
//#include "brw_export.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef MMI_ENABLE_DCD
#include "mmidcd_export.h"
#endif
#include "mmidm_export.h"
#ifdef DL_SUPPORT
#include "mmidl_export.h"
#endif
#include "mmimtv_export.h"

#ifdef MBBMS_SUPPORT
#include "mmimbbms_wintab.h"
#endif

#ifdef ASP_SUPPORT
#include "mmiasp_export.h"
#endif

#ifdef VT_SUPPORT
#include "mmivt_export.h"
#endif
#ifdef TTS_SUPPORT
#ifndef WIN32
//#include"mmiebook.h"
#include "mmiebook_export.h"
#include "mmitts_export.h"
#include "mmiebook_id.h"
#endif
#endif

#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif

#ifdef QQ_SUPPORT
#include "mmiqq_export.h"
#endif
#ifdef ENGTD_SUPPORT
#include "mmieng_dynamic_main.h"
#endif
#if defined(MMI_UPDATE_TIME_SUPPORT)
#include "mmiaut_export.h"
#endif /*MMI_UPDATE_TIME_SUPPORT*/
#include "mmiphone_nitz.h"
 
#ifdef BLUETOOTH_SUPPORT
#include "mmibt_export.h"
#endif

#include "mmiudisk_export.h"

#include "IN_Message.h"
#ifdef DRM_SUPPORT
#include "mmidrm.h"
#endif

#ifdef IKEYBACKUP_SUPPORT
#include "mmiikeybackup_api.h"
#endif

#ifdef MMI_VCALENDAR_SUPPORT
#include "Mmialarm_id.h"
#include "Mmivcalendar.h"
#include "mmischedule_export.h"
#endif

#ifdef MCARE_V31_SUPPORT
#include "Mcare_Interface.h"
#endif

#ifdef WRE_SUPPORT
#include "mmiwre_export.h"
#endif

#ifdef MOBILE_VIDEO_SUPPORT
#include "mmimv_export.h"
#endif

//#ifdef MMI_DUAL_BATTERY_SUPPORT
#include "dualbat_drvapi.h"
//#endif
#ifdef PDA_UI_DROPDOWN_WIN
#include "mmidropdownwin_export.h"
#endif
#ifdef LIVE_WALLPAPER_FRAMEWORK_SUPPORT
#include "mmilivewallpaper_export.h"
#endif
#include "mmicalendar_export.h"
#include "mmiphone_charge.h"
#include "mmiidle_statusbar.h"
#ifdef BROWSER_SUPPORT_DORADO
#include "mmibrowser_dorado_api.h"
#ifndef WIN32
//#include "OperaMiniApi.h"
#endif
#endif
#ifdef MMI_CSTAR_UDB_SUPPORT
#include "mmiim.h"
#endif
/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/

#define MAX_NETWORK_NUM (ARR_SIZE(s_network_nametable))

#define MAX_NETWORK_MNC_DIGIT_NUM_3_NUM (ARR_SIZE(s_network_mnc_digit_num_3_nametable))//mnc_digit_num=3

#define MAX_MCC_MNC_DIGIT_DISPLAY 20

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/
typedef struct
{
    BOOLEAN need_handle_pin[MMI_DUAL_SYS_MAX];		//need to handle pin
    BOOLEAN is_handling_pin[MMI_DUAL_SYS_MAX];		//is handling pin
    MN_DUAL_SYS_E   cur_handling_sim_id;					//current handling sim id
    MnSimGetPinIndS sim_pin_ind[MMI_DUAL_SYS_MAX];	        //save SIM_PIN_IND
}MMIPHONE_HANDLE_PIN_STATUS_T;                                  //add for dual sim


LOCAL const MMI_TEXT_ID_T s_sim_status_string[SIM_STATUS_MAX + 1] = 
{
    TXT_NULL,                               //SIM_STATUS_OK,
    TXT_SIM_REJECTED,               //SIM_STATUS_REJECTED,
    TXT_SIM_REGISTRATION_FAILED,//SIM_STATUS_REGISTRATION_FAILED,
    TXT_PIN_BLOCKED,                //SIM_STATUS_PIN_BLOCKED
    TXT_NOT_SUPPORT_SIM,	//SIM_STATUS_NOT_SUPPORT_SIM, //@zhaohui,cr109170                                                            
    TXT_PUK_BLOCKED,                //SIM_STATUS_PUK_BLOCKED,
    TXT_PUK_BLOCKED,                //SIM_STATUS_PUK2_BLOCKED,
    TXT_INSERT_SIM,                   //SIM_STATUS_NO_SIM,
    TXT_PROCESS_SEARCHING_NETWORK, //SIM_NOT_INITED
#if defined(MMI_SIM_LOCK_SUPPORT)
    TXT_SIM_LOCKED,                                //SIM_LOCKED
#endif    
    TXT_NULL                                //SIM_STATUS_MAX
};



LOCAL const MMI_NETWORK_ACI_INFO_T s_network_acitable[] = 
{
    001, "001",
    460, "CHN",//China
    466, "TPE",//Chinese Taipei
    886, "TPE",//Chinese Taipei
    455, "MAC",//Macao China
    454, "HKG",//Hongkong China
//    188, "188",
    202, "GRE",///Greece
    204, "NED",//Netherlands
    206, "BEL",//Belgium
    208, "FRA",//France
    213, "AND",//Andorra
    214, "ESP",//Spain
    216, "HUN",//Hungary
    218, "BIH",//Bosnia
    219, "CRO",//Croatia
    220, "SCG",//Serbia
    222, "ITA",//Italy
    226, "ROM",//Romania
    228, "SUI",//Switzerland
    230, "CZE",//Czech Republic
    231, "SVK",//Slovakia
    232, "AUT",//Austria
//    234, "234",//Guernsey
    238, "DEN",//Denmark
    240, "SWE",//Sweden
    242, "NOR",//Norway
    244, "FIN",//Finland
    246, "LTU",//Lithuania
    247, "LAT",//Latvia
    248, "EST",//Estonia
    250, "RUS",//Russian
    255, "UKR",//Ukraine
    257, "BLR",//Belarus
    259, "MDA",//Moldova
    260, "POL",//Poland
    262, "GER",//Germany
    266, "GIB",//Gibraltar
    268, "POR",//Portugal
    270, "LUX",//Luxembourg
    272, "IRL",//Ireland
    274, "ISL",//Iceland
    276, "ALB",//Albania
    278, "MLT",//Malta
    280, "CYP",//Cyprus
    282, "GEO",//Georgia
    283, "ARM",//Armenia
    284, "BUL",//Bulgaria
    286, "TUR",//Turkey
//    288, "288",//Faroe Islands
//    290, "290",//Greenland
//    293, "293",//Kosovo
    294, "MKD",//Republic of Macedonia
    295, "LIE",//Liechtenstein
    302, "CAN",//Canada
    310, "USA",//USA
    334, "MEX",//Mexico
    338, "ANT",//Antigua and Barbuda
//    340, "340",//Guadeloupe
    344, "ANT",//Antigua and Barbuda
//    350, "350",//Bermuda
//    362, "362",//Netherlands Antilles
    368, "CUB",//Cuba
    370, "DOM",//Dominican Republic
    400, "AZE",//Azerbaijan
    401, "KAZ",//Kazakhstan
    404, "IND",//India
    405, "IND",//India
    410, "PAK",//Pakistan
    412, "AFG",//Afghanistan
    413, "SRI",//Sri Lanka
    414, "MYA",//Myanmar
    415, "LIB",//Lebanon
    416, "JOR",//Jordan
    417, "SYR",//Syria
    419, "KUW",//Kuwait
    420, "KSA",//Saudi Arabia
    421, "YEM",//Yemen
    422, "OMA",//Oman
    424, "UAE",//United Arab Emirates
    425, "ISR",//Israel
    426, "BRN",//Bahrain
    427, "QAT",//Qatar
    428, "MGL",//Mongolia
    429, "NEP",//Nepal
    432, "IRI",//Iran
    434, "UZB",//Uzbekistan
    437, "KGZ",//Kyrgyzstan
    438, "TKM",//Turkmenistan
    440, "JAP",//JAPAN,NIPPON
    450, "KOR",//South Korea
    452, "VIE",//Vietnam
    456, "CAM",//Cambodia
    457, "LAO",//Laos
    470, "BAN",//Bangladesh
    472, "MDV",//Maldives
    502, "MAS",//Malaysia
    505, "AUS",//Australia
    510, "INA",//Indonesia
    515, "PHI",//Philippines
    520, "THA",//Thailand
    525, "SIN",//Singapore
    528, "BRU",//Brunei
    530, "NZL",//New Zealand
    539, "TGA",//Tonga
    541, "VAN",//Vanuatu
    542, "FIJ",//Fiji
//    544, "544",
//    546, "546",//New Caledonia
//    547, "547",//French Polynesia
    550, "FSM",//Federated States of Micronesia
    602, "EGY",//Egypt
    603, "ALG",//Algeria
    604, "MAR",//Morocco
    605, "TUN",//Tunisia
    607, "GAM",//Gambia
    608, "SEN",//Senegal
    610, "MLI",//Mali
    611, "GUI",//Guinea
    612, "COT",//Ivory Coast,Cote D'lvoire
    613, "BUR",//Burkina Faso
    614, "NIG",//Niger
    615, "TOG",//Togo
    616, "BEN",//Benin
    617, "MRI",//Mauritius
    618, "LBR",//Liberia
    620, "GHA",//Ghana
    621, "NGR",//Nigeria
    622, "CHA",//Chad
    624, "CMR",//Cameroon
    625, "CPV",//Cape Verde
    628, "GAB",//Gabon
    629, "CGO",//Republic of the Congo
    630, "COD",//Democratic Republic of the Congo
    631, "ANG",//Angola
    633, "SEY",//Seychelles
    634, "SUD",//Sudan
    635, "RWA",//Rwanda
    636, "ETH",//Ethiopia
    637, "SOM",//Somalia
    639, "KEN",//Kenya
    640, "TAN",//Tanzania
    641, "UGA",//Uganda
    642, "BDI",//Burundi
    643, "MOZ",//Mozambique
    645, "ZAM",//Zambia
    646, "MAD",//Madagascar
    647, "647",//Réunion
    648, "648",//Zimbabwe
    649, "NAM",//Namibia
    650, "MAW",//Malawi
    651, "LES",//Lesotho
    652, "BOT",//Botswana
    653, "SWZ",//Swaziland
    655, "RSA",//South Africa
    704, "GUA",//Guatemala
    706, "ESA",//El Salvador
    708, "HON",//Honduras
    710, "NIC",//Nicaragua
    714, "PAN",//Panama
    716, "PER",//Peru
    722, "ARG",//Argentina
    724, "BRA",//Brazil
    730, "CHI",//Chile
    734, "VEN",//Venezuela
    736, "BOL",//Bolivia
    740, "ECU",//Ecuador
    744, "PAR",//Paraguay
    746, "SUR",//Suriname
};

LOCAL const MMI_NETWORK_NAME_T s_network_nametable[] =
{
    001, 01, TXT_NET_TEST,          PLMN_TEST,          //测试卡
    460, 00, TXT_NET_CMCC,          PLMN_CMCC,          //中国移动
    460, 01, TXT_NET_CUCC,          PLMN_UNICOM,        //中国联通
    460, 02, TXT_NET_CMCC,          PLMN_460_02,          //中国移动
    460, 03, TXT_NET_CTCC,          PLMN_460_03,          //中国电信
    460, 05, TXT_NET_CTCC,          PLMN_460_05,          //中国电信
    460, 06, TXT_NET_CUCC,          PLMN_460_06,       //中国联通
    460, 07, TXT_NET_CMCC,          PLMN_460_07,          //中国移动
    460, 20, TXT_NET_CTT,             PLMN_460_20,          //中国铁通
    466, 01, TXT_NET_FAREASTONE1,   PLMN_FAREASTONE1,   //远传电信
    466, 02, TXT_NET_FAREASTONE1,   PLMN_FAREASTONE2,   //远传电信
    466, 06, TXT_NET_TUNTEX,        PLMN_TUNTEX,        //东荣电信
    466, 11, TXT_NET_TWLDTADCS,     PLMN_TWLDTADCS,     //中华电信
    466, 88, TXT_NET_KG_ONLINE,     PLMN_KG_ONLINE,     //和信电讯
    466, 89, TXT_NET_VIBO,     PLMN_466_89, 
    466, 92, TXT_NET_CHUNGHWA,      PLMN_CHUNGHWA,      //中华电信
    466, 93, TXT_NET_MOBITAIGSM,    PLMN_MOBITAIGSM,    //东信电讯
    466, 97, TXT_NET_TWNGSM1800,    PLMN_TWNGSM1800,    //台湾大哥大
    466, 99, TXT_NET_TRANSASIA,     PLMN_TRANSASIA,     //泛亚电信
    886, 92, TXT_NET_TWLDGSM,       PLMN_TWLDGSM,       //…
    455, 00, TXT_NET_SMARTONE,      PLMN_SMARTONE,      //…
    455, 01, TXT_NET_MACCTMGSM,     PLMN_MACCTMGSM,     //…
    455, 02, TXT_NET_CTCC,     PLMN_455_02,     //…
    455, 03, TXT_NET_HUTCHISON_MAC,     PLMN_455_03,     //…
    455, 04, TXT_NET_MACCTMGSM,     PLMN_455_04,     //…
    455, 05, TXT_NET_3_3G          ,  PLMN_455_05           ,
    454, 00, TXT_NET_CSL,     PLMN_HKTELECOM,     //香港电讯
    454, 01, TXT_NET_HKCITIC,     PLMN_454_01,     //中信电信
    454, 02, TXT_NET_CSL,     PLMN_454_02,     //香港电讯
    454, 03, TXT_NET_3_3G,        PLMN_454_03,        //…
    454, 04, TXT_NET_3_2G,        PLMN_454_04,        //…
    454, 05, TXT_NET_3_CDMA,        PLMN_454_05,        //…
    454, 06, TXT_NET_HKSMC,         PLMN_HKSMC,         //数码通
    454, 07, TXT_NET_CUCC,         PLMN_454_07,         //中国联通
    454, 8,   TXT_NET_TRIDENT,         PLMN_454_08,         //
    454, 9,   TXT_NET_CMCC,         PLMN_454_09,         //
    454, 10, TXT_NET_NEWWORLD,      PLMN_NEWWORD,       //新世纪传动网
    454, 11, TXT_NET_HKCTCC,         PLMN_454_11,         //
    454, 12, TXT_NET_CMCC_HK,       PLMN_454_12,       //中国移动(香港)
    454, 14, TXT_NET_HUTCH,         PLMN_454_14,         //
    454, 15, TXT_NET_HKSMC,         PLMN_454_15,
    454, 16, TXT_NET_PCCW_2G,        PLMN_SUNDAY,        //…
    454, 17, TXT_NET_HKSMC,         PLMN_454_17,
    454, 18, TXT_NET_CSL,   PLMN_PACIFICLINK,   //…
    454, 19, TXT_NET_PCCW_3G,   PLMN_454_19,
    454, 22, TXT_NET_HKPPLUS,       PLMN_HKPPLUS,        //…
    454, 29, TXT_NET_PCCW_CDMA,   PLMN_454_29,
    188, 1  ,  TXT_NET_ICE                     ,  PLMN_ICE                     ,
    202, 1  ,  TXT_NET_COSMOTE                 ,  PLMN_COSMOTE                 ,
    202, 5  ,  TXT_NET_GR_VODA                 ,  PLMN_GR_VODA                 ,
    202, 9  ,  TXT_NET_Q_TELCOM                ,  PLMN_Q_TELCOM                ,
    202, 10 ,  TXT_NET_GR_TELESTET             ,  PLMN_GR_TELESTET             ,
    204, 4  ,  TXT_NET_NLVODA                  ,  PLMN_NLVODA                  ,
    204, 8  ,  TXT_NET_NL_KPN_TELECOM          ,  PLMN_NL_KPN_TELECOM          ,
    204, 12 ,  TXT_NET_NL_TELFORT              ,  PLMN_NL_TELFORT              ,
    204, 16 ,  TXT_NET_BEN_NL                  ,  PLMN_BEN_NL                  ,
    204, 20 ,  TXT_NET_DUTCHTONE               ,  PLMN_DUTCHTONE               ,
    206, 1  ,  TXT_NET_BEL_PROXIMUS            ,  PLMN_BEL_PROXIMUS            ,
    206, 5 ,    TXT_NET_BEL_TELENET                ,  PLMN_206_05,
    206, 10 ,  TXT_NET_MOBISTAR                ,  PLMN_MOBISTAR                ,
    206, 20 ,  TXT_NET_BASE                    ,  PLMN_BASE                    ,
    208, 00  ,  TXT_NET_ORANGE                 ,  PLMN_208_00,
    208, 1  ,  TXT_NET_ORANGE              ,  PLMN_F_ITINERIS              ,
    208, 02  ,  TXT_NET_F_CONTACT                 ,  PLMN_208_02,
    208, 10  ,  TXT_NET_SFR                 ,  PLMN_208_10,
    208, 11  ,  TXT_NET_SFR                 ,  PLMN_208_11,
    208, 13  ,  TXT_NET_F_CONTACT                 ,  PLMN_208_13,
    208, 14  ,  TXT_NET_FREE                 ,  PLMN_208_14,
    208, 15  ,  TXT_NET_FREE                 ,  PLMN_208_15,
    208, 20  ,  TXT_NET_BOUYGUES                 ,  PLMN_208_20,
    208, 21  ,  TXT_NET_BOUYGUES                 ,  PLMN_208_21,
    208, 22  ,  TXT_NET_TRANSATEL                 ,  PLMN_208_22,
    208, 88  ,  TXT_NET_F_CONTACT                 ,  PLMN_208_88,
    213, 3  ,  TXT_NET_STA_MOBILAND            ,  PLMN_STA_MOBILAND            ,
    214, 1  ,  TXT_NET_VODAFONE_1                ,  PLMN_E_AIRTEL                ,
    214, 3  ,  TXT_NET_ORANGE                 ,  PLMN_RET_MOV                 ,
    214, 4  ,  TXT_NET_YOIGO                 ,  PLMN_214_04                 ,
    214, 5  ,  TXT_NET_TME                 ,  PLMN_214_05                 ,
    214, 6  ,  TXT_NET_VODAFONE_1                 ,  PLMN_214_06                 ,
    214, 7  ,  TXT_NET_MOVISTAR              ,  PLMN_E_MOVISTAR              ,
    214, 8  ,  TXT_NET_EUSKALTEL                 ,  PLMN_214_08                 ,
    214, 9  ,  TXT_NET_ORANGE                 ,  PLMN_214_09,
    214, 15  ,  TXT_NET_UK_BT                 ,  PLMN_214_15,
    214, 16  ,  TXT_NET_TELECABLE                 ,  PLMN_214_16,
    214, 17  ,  TXT_NET_MOBIL_R                 ,  PLMN_214_17,
    214, 18  ,  TXT_NET_ONO                 ,  PLMN_214_18,
    214, 19  ,  TXT_NET_SIMYO                 ,  PLMN_214_19,
    214, 20  ,  TXT_NET_FONYOU                 ,  PLMN_214_20,
    214, 21  ,  TXT_NET_JAZZTEL                 ,  PLMN_214_21,
    214, 22  ,  TXT_NET_DIGIMOBIL                 ,  PLMN_214_22,
    214, 23  ,  TXT_NET_BARABLU                 ,  PLMN_214_23,
    214, 24  ,  TXT_NET_EROSKI                 ,  PLMN_214_24,
    214, 25  ,  TXT_NET_LYCAMOBILE                 ,  PLMN_214_25,
    216, 1  ,  TXT_NET_PANNON_GSM              ,  PLMN_PANNON_GSM              ,
    216, 30 ,  TXT_NET_WESTEL                  ,  PLMN_WESTEL                  ,
    216, 70 ,  TXT_NET_VODAFONE_1              ,  PLMN_VODAFONE_1              ,
    218, 3  ,  TXT_NET_BIH_ERONET              ,  PLMN_BIH_ERONET              ,
    218, 5  ,  TXT_NET_GSM_MS1                 ,  PLMN_GSM_MS1                 ,
    218, 90 ,  TXT_NET_PTT_GSMBIH              ,  PLMN_PTT_GSMBIH              ,
    219, 1  ,  TXT_NET_HR_CRONET               ,  PLMN_HR_CRONET               ,
    219, 2  ,  TXT_NET_219_02               ,  PLMN_219_02               ,
    219, 10 ,  TXT_NET_HR_VIP                  ,  PLMN_HR_VIP                  ,
    220, 1  ,  TXT_NET_YU_MOBTEL               ,  PLMN_YU_MOBTEL               ,
    220, 2  ,  TXT_NET_PROMONTE_GSM            ,  PLMN_PROMONTE_GSM            ,
    220, 3  ,  TXT_NET_YUG_03                  ,  PLMN_YUG_03                  ,
    220, 4  ,  TXT_NET_MONET                   ,  PLMN_MONET                   ,
    222, 1  ,  TXT_NET_I_TELECOM               ,  PLMN_I_TELECOM               ,
    222, 10 ,  TXT_NET_GR_VODA             ,  PLMN_VODAFONE_IT             ,
    222, 88 ,  TXT_NET_I_WIND                  ,  PLMN_I_WIND                  ,
    222, 98 ,  TXT_NET_I_BLU                   ,  PLMN_I_BLU                   ,
    226, 1  ,  TXT_NET_CONNEX_GSM              ,  PLMN_CONNEX_GSM              ,
    226, 2  ,  TXT_NET_ROMTELECOM              ,  PLMN_226_02              ,
    226, 3  ,  TXT_NET_COSMOROM                ,  PLMN_COSMOROM                ,//GSM
    226, 4  ,  TXT_NET_COSMOROM                ,  PLMN_226_04               ,   //CDMA2000
    226, 5  ,  TXT_NET_DIGI                ,  PLMN_226_05                ,  //UMTS2100
    226, 6  ,  TXT_NET_COSMOROM                ,  PLMN_226_06                ,  //UMTS2100
    226, 10 ,  TXT_NET_ORANGE_CH               ,  PLMN_RO_DIALOG               ,
    228, 1  ,  TXT_NET_SWISS_GSM               ,  PLMN_SWISS_GSM               ,
    228, 2  ,  TXT_NET_SUNRISE                 ,  PLMN_SUNRISE                 ,
    228, 3  ,  TXT_NET_ORANGE_CH               ,  PLMN_ORANGE_CH               ,
    228, 6  ,  TXT_NET_405_48               ,  PLMN_228_06,     //GSM-R
    228, 8  ,  TXT_NET_TELE2               ,  PLMN_228_08,
    230, 1  ,  TXT_NET_CZ_PAEGAS               ,  PLMN_CZ_PAEGAS               ,
    230, 2  ,  TXT_NET_EUROTEL_CZ              ,  PLMN_EUROTEL_CZ              ,
    230, 3  ,  TXT_NET_OSKAR                   ,  PLMN_OSKAR                   ,
    231, 1  ,  TXT_NET_ORANGE                  ,  PLMN_231_01                  ,
    231, 2  ,  TXT_NET_EUROTEL_SK              ,  PLMN_EUROTEL_SK              , //T-mobile
    231, 4  ,  TXT_NET_EUROTEL_SK              ,  PLMN_231_04              ,
    231, 6  ,  TXT_NET_O2_SK              ,  PLMN_231_06              ,    //o2-SK
    231, 99  ,  TXT_NET_405_48              ,  PLMN_231_99              ,
    232, 1  ,  TXT_NET_A1                      ,  PLMN_A1                      ,
    232, 3  ,  TXT_NET_T_MOBILE                ,  PLMN_T_MOBILE                ,
    232, 5  ,  TXT_NET_A_ONE                  ,  PLMN_A_ONE                   ,
    232, 7  ,  TXT_NET_A_TELE_RING             ,  PLMN_A_TELE_RING             ,
    232, 9  ,  TXT_NET_A1                      ,  PLMN_232_09                      ,
    232, 10  ,  TXT_NET_3_3G                      ,  PLMN_232_10                      ,
    232, 11  ,  TXT_NET_A1                      ,  PLMN_232_11                      ,
    232, 12 ,  TXT_NET_A_ONE                   ,  PLMN_232_12                   ,
    232, 14 ,  TXT_NET_3_3G                   ,  PLMN_232_14                   ,  // 3 test                                  
    232, 15 ,  TXT_NET_BARABLU                   ,  PLMN_232_15                  ,                         
    232, 91 ,  TXT_NET_405_48                   ,  PLMN_232_91                  , // R-GSM railway communication
    234, 0 ,  TXT_NET_UK_BT                   ,  PLMN_234_00                   ,
    234, 1 ,  TXT_NET_UK_01                   ,  PLMN_234_01                   ,
    234, 2 ,  TXT_NET_O2_UK                   ,  PLMN_234_02                   ,
    234, 3 ,  TXT_NET_AIRTEL                   ,  PLMN_234_03                   ,   //airtel-vodafone
    234, 4 ,  TXT_NET_FMS_UK                   ,  PLMN_234_04                   ,
    234, 10 ,  TXT_NET_O2_UK                   ,  PLMN_O2_UK                   ,
    234, 11 ,  TXT_NET_O2_UK                   ,  PLMN_234_11                   ,
    234, 12 ,  TXT_NET_RAILTRACK                   ,  PLMN_234_12                   ,
    234, 15 ,  TXT_NET_UK_VODAFONE             ,  PLMN_UK_VODAFONE             ,
    234, 16 ,  TXT_NET_OPAL             ,  PLMN_234_16             ,
    234, 18 ,  TXT_NET_CLOUND9             ,  PLMN_234_18             ,
    234, 20 ,  TXT_NET_3_3G             ,  PLMN_234_20             ,
    234, 25 ,  TXT_NET_TRUPHONE             ,  PLMN_234_25             ,
    234, 30 ,  TXT_NET_TMO_UK                  ,  PLMN_TMO_UK                  ,
    234, 31 ,  TXT_NET_VIRGIN                  ,  PLMN_234_31                  ,
    234, 32 ,  TXT_NET_VIRGIN                  ,  PLMN_234_32                  ,
    234, 33 ,  TXT_NET_ORANGE                ,  PLMN_ORANGE_1                ,
    234, 34 ,  TXT_NET_ORANGE                 ,  PLMN_234_34                ,
    234, 50 ,  TXT_NET_234_50                ,  PLMN_ORANGE_2                ,
    234, 55 ,  TXT_NET_UK_GSY_TEL              ,  PLMN_UK_GSY_TEL              ,
    234, 58 ,  TXT_NET_PRONTO_GSM              ,  PLMN_PRONTO_GSM              ,
    234, 75 ,  TXT_NET_234_75              ,  PLMN_234_75              ,
    234, 77 ,  TXT_NET_UK_BT              ,  PLMN_234_77              ,
    234, 78 ,  TXT_NET_234_78              ,  PLMN_234_78              ,
    238, 1  ,  TXT_NET_DK_TDK_MOBIL            ,  PLMN_DK_TDK_MOBIL            ,
    238, 2  ,  TXT_NET_SONOFON                 ,  PLMN_SONOFON                 ,
    238, 20 ,  TXT_NET_DK_TELIA                ,  PLMN_DK_TELIA                ,
    238, 30 ,  TXT_NET_ORANGE               ,  PLMN_DK_ORANGE               ,
    240, 1  ,  TXT_NET_DK_TELIA         ,  PLMN_S_TELIA_MOBITEL         ,
    240, 7  ,  TXT_NET_S_COMVIQ                ,  PLMN_S_COMVIQ                ,
    240, 8  ,  TXT_NET_S_EUROPOLITAN           ,  PLMN_S_EUROPOLITAN           ,
    242, 1  ,  TXT_NET_N_TELE_MOBIL            ,  PLMN_N_TELE_MOBIL            ,
    242, 2  ,  TXT_NET_N_NETCOM_GSM            ,  PLMN_N_NETCOM_GSM            ,
    244, 3  ,  TXT_NET_DK_TELIA                ,  PLMN_FI_TELIA                ,
    244, 5  ,  TXT_NET_FI_RL                   ,  PLMN_FI_RL                   ,
    244, 9  ,  TXT_NET_FI_FINNET               ,  PLMN_FI_FINNET               ,
    244, 12 ,  TXT_NET_FI_2G                   ,  PLMN_FI_2G                   ,
    244, 14 ,  TXT_NET_AMT                     ,  PLMN_AMT                     ,
    244, 91 ,  TXT_NET_FI_SONERA               ,  PLMN_FI_SONERA               ,
    246, 1  ,  TXT_NET_OMNITEL_LITH            ,  PLMN_OMNITEL_LITH            ,
    246, 2  ,  TXT_NET_LT_BITE_GSM             ,  PLMN_LT_BITE_GSM             ,
    246, 3  ,  TXT_NET_TELE2                   ,  PLMN_TELE2                   ,
    247, 1  ,  TXT_NET_LV_LMT_GSM              ,  PLMN_LV_LMT_GSM              ,
    247, 2  ,  TXT_NET_LV_BALTCOM              ,  PLMN_LV_BALTCOM              ,
    248, 1  ,  TXT_NET_EMT_GSM                 ,  PLMN_EMT_GSM                 ,
    248, 2  ,  TXT_NET_EE_RLE_GSM              ,  PLMN_EE_RLE_GSM              ,
    248, 3  ,  TXT_NET_TELE2                ,  PLMN_EE_Q_GSM                ,
    250, 1  ,  TXT_NET_250_01                  ,  PLMN_250_01                  ,
    250, 2  ,  TXT_NET_RUS_02                  ,  PLMN_RUS_02                  ,
    250, 3  ,  TXT_NET_RUS_03                  ,  PLMN_RUS_03                  ,
    250, 4  ,  TXT_NET_250_04                  ,  PLMN_250_04                  ,
    250, 5  ,  TXT_NET_RUS_05                  ,  PLMN_RUS_05                  ,
    250, 7  ,  TXT_NET_250_15                  ,  PLMN_RUS_07                  ,
    250, 10 ,  TXT_NET_RUS_10                  ,  PLMN_RUS_10                  ,
    250, 11 ,  TXT_NET_250_11                  ,  PLMN_250_11                  ,
    250, 12 ,  TXT_NET_RUS_12                  ,  PLMN_RUS_12                  ,
    250, 13 ,  TXT_NET_RUS_KUBAN_GSM           ,  PLMN_RUS_KUBAN_GSM           ,
    250, 15 ,  TXT_NET_250_15                  ,  PLMN_250_15                  ,
    250, 16 ,  TXT_NET_250_16                  ,  PLMN_250_16                  ,
    250, 17 ,  TXT_NET_UTEL               ,  PLMN_250_17               ,
    250, 19 ,  TXT_NET_250_19               ,  PLMN_250_19               ,
    250, 20 ,  TXT_NET_250_20               ,  PLMN_250_20               ,
    250, 23 ,  TXT_NET_250_23               ,  PLMN_250_23               ,
    250, 28 ,  TXT_NET_BEELINE                  ,  PLMN_RUS_28                  ,
    250, 35 ,  TXT_NET_250_35                  ,  PLMN_250_35                  ,
    250, 38 ,  TXT_NET_250_38                  ,  PLMN_250_38                  ,
    250, 39 ,  TXT_NET_UTEL                  ,  PLMN_RUS_39                  ,
    250, 44 ,  TXT_NET_NC_GSM                  ,  PLMN_NC_GSM                  ,
    250, 92 ,  TXT_NET_RUS_92                  ,  PLMN_RUS_92                  ,
    250, 93 ,  TXT_NET_RUS_93                  ,  PLMN_RUS_93                  ,
    250, 99 ,  TXT_NET_BEELINE                 ,  PLMN_BEELINE                 ,
    255, 1  ,  TXT_NET_UMC_UA                  ,  PLMN_UMC_UA                  ,
    255, 2  ,  TXT_NET_FLASH_UKR               ,  PLMN_FLASH_UKR               ,
    255, 3  ,  TXT_NET_UA_KYIVSTAR             ,  PLMN_UA_KYIVSTAR             ,
    255, 5  ,  TXT_NET_UA_GOLDEN_TELECOM       ,  PLMN_UA_GOLDEN_TELECOM       ,
    257, 1  ,  TXT_NET_BY_VELCOM               ,  PLMN_BY_VELCOM               ,
    259, 1  ,  TXT_NET_MD_VOXTEL               ,  PLMN_MD_VOXTEL               ,
    259, 2  ,  TXT_NET_MD_02                   ,  PLMN_MD_02                   ,
    260, 1  ,  TXT_NET_PL_PLUS                 ,  PLMN_PL_PLUS                 ,
    260, 2  ,  TXT_NET_T_MOBILE              ,  PLMN_PL_ERA_GSM              ,
    260, 3  ,  TXT_NET_ORANGE                 ,  PLMN_PL_IDEA                 ,
    260, 6  ,  TXT_NET_PLAY                 ,  PLMN_260_06                 ,
    260, 7  ,  TXT_NET_NETIA                 ,  PLMN_260_07                 ,
    260, 9  ,  TXT_NET_405_48                 ,  PLMN_260_09                 ,  //railway communication
    260, 10  ,  TXT_NET_SFERIA                 ,  PLMN_260_10                 ,
    260, 11  ,  TXT_NET_NORDISK                 ,  PLMN_260_11,
    260, 12  ,  TXT_NET_CYFROWY                 ,  PLMN_260_12,
    260, 15  ,  TXT_NET_CENTERNET                 ,  PLMN_260_15,
    260, 16  ,  TXT_NET_MOBYLAND                 ,  PLMN_260_16,
    260, 17  ,  TXT_NET_AERO                 ,  PLMN_260_17,
    262, 1  ,  TXT_NET_T_MOBILE              ,  PLMN_T_MOBILE_D              ,
    262, 2  ,  TXT_NET_VODAFONE_1             ,  PLMN_VODAFONE_D2             ,
    262, 3  ,  TXT_NET_E_PLUS                  ,  PLMN_E_PLUS                  ,
    262, 7  ,  TXT_NET_O2_DE                   ,  PLMN_O2_DE                   ,
    266, 1  ,  TXT_NET_GIBTEL                  ,  PLMN_GIBTEL                  ,
    268, 1  ,  TXT_NET_VODAFONE_1              ,  PLMN_P_VODAFONE              ,
    268, 3  ,  TXT_NET_P_OPTIMUS               ,  PLMN_P_OPTIMUS               ,
    268, 6  ,  TXT_NET_P_TELEMOVEL             ,  PLMN_P_TELEMOVEL             ,
    270, 1  ,  TXT_NET_L_LUXGSM                ,  PLMN_L_LUXGSM                ,
    270, 77 ,  TXT_NET_L_TANGO                 ,  PLMN_L_TANGO                 ,
    270, 99  ,  TXT_NET_ORANGE                 ,  PLMN_270_99,
    272, 1  ,  TXT_NET_IRL_EIRCELL             ,  PLMN_IRL_EIRCELL             ,
    272, 2  ,  TXT_NET_02_IRL                  ,  PLMN_02_IRL                  ,
    272, 3  ,  TXT_NET_METEOR                  ,  PLMN_METEOR                  ,
    274, 1  ,  TXT_NET_IS_SIMINN               ,  PLMN_IS_SIMINN               ,
    274, 2  ,  TXT_NET_IS_TAL                  ,  PLMN_IS_TAL                  ,
    274, 3  ,  TXT_NET_ISLANDSSIMI             ,  PLMN_ISLANDSSIMI             ,
    274, 4  ,  TXT_NET_274_04                  ,  PLMN_274_04                  ,
    276, 1  ,  TXT_NET_AMC_AL                  ,  PLMN_AMC_AL                  ,
    276, 2  ,  TXT_NET_VODAFONE_2              ,  PLMN_VODAFONE_2              ,
    278, 1  ,  TXT_NET_VODAFONE_MLA            ,  PLMN_VODAFONE_MLA            ,
    278, 21 ,  TXT_NET_GO_MOBILE               ,  PLMN_GO_MOBILE               ,
    280, 1  ,  TXT_NET_CY_CYTAGSM              ,  PLMN_CY_CYTAGSM              ,
    282, 1  ,  TXT_NET_GEO_GEOCELL             ,  PLMN_GEO_GEOCELL             ,
    282, 2  ,  TXT_NET_GEO_MAGTI               ,  PLMN_GEO_MAGTI               ,
    283, 1  ,  TXT_NET_RA_ARMGSM               ,  PLMN_RA_ARMGSM               ,
    283, 5  ,  TXT_NET_NUEVATEL               ,  PLMN_283_05               ,
    283, 10  ,  TXT_NET_ORANGE                 ,  PLMN_283_10,
    284, 1  ,  TXT_NET_M_TEL_GSM_BG            ,  PLMN_M_TEL_GSM_BG            ,
    284, 5  ,  TXT_NET_GLOBUL                  ,  PLMN_GLOBUL                  ,
    286, 1  ,  TXT_NET_TR_TCELL                ,  PLMN_TR_TCELL                ,
    286, 2  ,  TXT_NET_TR_TELSIM               ,  PLMN_TR_TELSIM               ,
    286, 3  ,  TXT_NET_ARIA                    ,  PLMN_ARIA                    ,
    286, 4  ,  TXT_NET_AYCELL                  ,  PLMN_AYCELL                  ,
    288, 1  ,  TXT_NET_FROFT                   ,  PLMN_FROFT                   ,
    290, 1  ,  TXT_NET_TELE_GRL                ,  PLMN_TELE_GRL                ,
    293, 40 ,  TXT_NET_SI_MOBIL                ,  PLMN_SI_MOBIL                ,
    293, 41 ,  TXT_NET_SI_GSM                  ,  PLMN_SI_GSM                  ,
    293, 70 ,  TXT_NET_VEGA_070                ,  PLMN_VEGA_070                ,
    294, 1  ,  TXT_NET_MKD_MOBIMAK             ,  PLMN_MKD_MOBIMAK             ,
    295, 1  ,  TXT_NET_295_01                  ,  PLMN_295_01                  ,
    295, 2  ,  TXT_NET_EUROPLATFORM            ,  PLMN_EUROPLATFORM            ,
    295, 5  ,  TXT_NET_FL1                     ,  PLMN_FL1                     ,
    295, 77 ,  TXT_NET_LI_TANGO                ,  PLMN_LI_TANGO                ,
    302, 370,  TXT_NET_MCELL               ,  PLMN_MICROCELL               ,
    302, 720,  TXT_NET_ROGERS_AT_T_WIRELESS    ,  PLMN_ROGERS_AT_T_WIRELESS    ,
    302, 37 ,  TXT_NET_MCELL                   ,  PLMN_MCELL                   ,
    310, 1  ,  TXT_NET_310_01                  ,  PLMN_310_01                  ,
    310, 110,  TXT_NET_WTTCKY_2                ,  PLMN_WTTCKY_2                ,
    310, 140,  TXT_NET_SPROCKET                ,  PLMN_SPROCKET                ,
    310, 150,  TXT_NET_AT_T_WIRELESS_1     ,  PLMN_CINGULAR_WIRELESS_1     ,
    310, 160,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_1           ,
    310, 170,  TXT_NET_T_MOBILE     ,  PLMN_CINGULAR_WIRELESS_2     ,
    310, 180,  TXT_NET_CINGULAR_WIRELESS_3     ,  PLMN_CINGULAR_WIRELESS_3     ,
    310, 200,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_2           ,
    310, 210,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_3           ,
    310, 220,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_4           ,
    310, 230,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_5           ,
    310, 240,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_6           ,
    310, 250,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_7           ,
    310, 260,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_8           ,
    310, 270,  TXT_NET_T_MOBILE           ,  PLMN_VOICESTREAM_9           ,
    310, 310,  TXT_NET_T_MOBILE          ,  PLMN_VOICESTREAM_10          ,
    310, 340,  TXT_NET_WESTLINK_COMM           ,  PLMN_WESTLINK_COMM           ,
    310, 350,  TXT_NET_CAROLINA_PHONE          ,  PLMN_CAROLINA_PHONE          ,
    310, 380,  TXT_NET_AT_T_WIRELESS_1         ,  PLMN_AT_T_WIRELESS_1         ,          
    310, 410,  TXT_NET_AT_T_WIRELESS_1                ,  PLMN_310_410                ,
    310, 460,  TXT_NET_310_460                 ,  PLMN_310_460                 ,
    310, 560,  TXT_NET_AT_T_WIRELESS_1                ,  PLMN_DOBSONUS                ,
    310, 580,  TXT_NET_T_MOBILE                 ,  PLMN_PCS_ONE                 ,
    310, 610,  TXT_NET_EPIC_TOUCH              ,  PLMN_EPIC_TOUCH              ,
    310, 630,  TXT_NET_AMERILINK_PCS           ,  PLMN_AMERILINK_PCS           ,
    310, 640,  TXT_NET_EINSTEIN_PCS            ,  PLMN_EINSTEIN_PCS            ,
    310, 660,  TXT_NET_T_MOBILE          ,  PLMN_VOICESTREAM_11          ,
    310, 670,  TXT_NET_WIRELESS_2000_PCS       ,  PLMN_WIRELESS_2000_PCS       ,
    310, 680,  TXT_NET_AT_T_WIRELESS_1            ,  PLMN_NPI_WIRELESS            ,
    310, 690,  TXT_NET_CONESTOGA               ,  PLMN_CONESTOGA               ,
    310, 740,  TXT_NET_TELEMETRIX              ,  PLMN_TELEMETRIX              ,
    310, 760,  TXT_NET_PTSI                    ,  PLMN_PTSI                    ,
    310, 770,  TXT_NET_IWS                     ,  PLMN_IWS                     ,
    310, 780,  TXT_NET_AIRLINK_PCS             ,  PLMN_AIRLINK_PCS             ,
    310, 790,  TXT_NET_PINPOINT                ,  PLMN_PINPOINT                ,
    310, 800,  TXT_NET_VOICESTREAM_12          ,  PLMN_VOICESTREAM_12          ,
    310, 980,  TXT_NET_AT_T_WIRELESS_1         ,  PLMN_AT_T_WIRELESS_2         ,
    310, 11 ,  TXT_NET_WTTCKY_1                ,  PLMN_WTTCKY_1                ,
    310, 31 ,  TXT_NET_AERIAL                  ,  PLMN_AERIAL                  ,
    310, 15 ,  TXT_NET_BSMDCS                  ,  PLMN_BSMDCS                  ,
    310, 20 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_1                   ,
    310, 21 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_2                   ,
    310, 22 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_3                   ,
    310, 23 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_4                   ,
    310, 24 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_5                   ,
    310, 25 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_6                   ,
    310, 26 ,  TXT_NET_T_MOBILE                   ,  PLMN_WWC_7                   ,
    310, 27 ,  TXT_NET_USA27                   ,  PLMN_USA27                   ,
    334, 01 ,  TXT_NET_NEXTEL                  ,  PLMN_334_01                  ,
    334, 03 ,  TXT_NET_MOVISTAR                  ,  PLMN_334_03                  ,
    334, 04 ,  TXT_NET_334_04                  ,  PLMN_334_04                  ,
    334, 20 ,  TXT_NET_TELCEL                  ,  PLMN_334_20                  ,
    334, 30 ,  TXT_NET_MOVISTAR                  ,  PLMN_334_30                  ,
    334, 50 ,  TXT_NET_334_050                  ,  PLMN_334_050                  ,
    338, 5  ,  TXT_NET_338_05                  ,  PLMN_338_05                  ,
    340, 1  ,  TXT_NET_ORANGE                  ,  PLMN_AMERIS                  ,
    340, 2  ,  TXT_NET_OUTREMER                  ,  PLMN_340_02                  ,
    340, 20 ,  TXT_NET_DIGICEL                  ,  PLMN_340_20                  ,
    344, 30 ,  TXT_NET_APUA_PCS_ANTIGUA        ,  PLMN_APUA_PCS_ANTIGUA        ,
    350, 1  ,  TXT_NET_TELECOM                 ,  PLMN_TELECOM                 ,
    362, 51 ,  TXT_NET_TELCELL_GSM             ,  PLMN_TELCELL_GSM             ,
    362, 91 ,  TXT_NET_UTS                     ,  PLMN_UTS                     ,
    368, 1  ,  TXT_NET_C_COM                   ,  PLMN_C_COM                   ,
    370, 1  ,  TXT_NET_ORANGE_3                ,  PLMN_ORANGE_3                ,
    370, 2  ,  TXT_NET_CLARO_BRASIL                ,  PLMN_370_02,
    370, 3  ,  TXT_NET_TRICOM                ,  PLMN_370_03                ,
    370, 4  ,  TXT_NET_NUEVATEL                ,  PLMN_370_04                ,
    400, 1  ,  TXT_NET_AZE_AZERCELL_GSM        ,  PLMN_AZE_AZERCELL_GSM        ,
    400, 2  ,  TXT_NET_BAKCELL_GSM_2000        ,  PLMN_BAKCELL_GSM_2000        ,
    401, 1  ,  TXT_NET_KZ_K_MOBILE             ,  PLMN_KZ_K_MOBILE             ,
    401, 2  ,  TXT_NET_KCELL_KZ                ,  PLMN_KCELL_KZ                ,
    404, 0  ,  TXT_NET_MTS_IN                   ,  PLMN_MTS_IN                   ,    
    404, 1  ,  TXT_NET_VODAFONE_IN                   ,  PLMN_ESSAH                   ,    
    404, 2  ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_01               ,
    404, 3  ,  TXT_NET_AIRTEL                 ,  PLMN_TELENET                 ,
    404, 4  ,  TXT_NET_IDEA                    ,  PLMN_IDEA_01                 ,
    404, 5  ,  TXT_NET_VODAFONE_IN                ,  PLMN_CELFORCE                ,
    404, 6  ,  TXT_NET_AIRTEL                ,  PLMN_AIRTEL_06                ,
    404, 7  ,  TXT_NET_IDEA                    ,  PLMN_TATA                    ,
    404, 8  ,  TXT_NET_INA_08                  ,  PLMN_INA_08                  ,
    404, 9  ,  TXT_NET_INA_09                  ,  PLMN_INA_09                  ,
    404, 10 ,  TXT_NET_AIRTEL              ,  PLMN_INA_AIRTEL              ,
    404, 11 ,  TXT_NET_VODAFONE_IN               ,  PLMN_INA_ESSAR               ,
    404, 12 ,  TXT_NET_IDEA                  ,  PLMN_INA_12                  ,
    404, 13 ,  TXT_NET_VODAFONE_IN                 ,  PLMN_HUTCH_1                 ,
    404, 14 ,  TXT_NET_IDEA                   ,  PLMN_IN_14                   ,
    404, 15 ,  TXT_NET_VODAFONE_IN                ,  PLMN_AIRCEL_1                ,
    404, 16 ,  TXT_NET_AIRTEL              ,  PLMN_AIRTEL_16              ,
    404, 17 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_AIRTEL_02               ,
    404, 18 ,  TXT_NET_INA_09                  ,  PLMN_INA_18                  ,
    404, 19 ,  TXT_NET_IDEA                  ,  PLMN_INA_19                  ,
    404, 20 ,  TXT_NET_VODAFONE_IN              ,  PLMN_INA_ORANGE              ,
    404, 21 ,  TXT_NET_LOOP_MOBILE                     ,  PLMN_BPL                     ,
    404, 22 ,  TXT_NET_IDEA                  ,  PLMN_INA_22                  ,
    404, 23 ,  TXT_NET_INA_08                  ,  PLMN_INA_23                  ,
    404, 24 ,  TXT_NET_IDEA                  ,  PLMN_INA_24                  ,
    404, 25 ,  TXT_NET_AIRCEL_NEW              ,  PLMN_AIRCEL_NEW_01           ,
    404, 26 ,  TXT_NET_INA_08                  ,  PLMN_INA_26                  ,
    404, 27 ,  TXT_NET_VODAFONE_IN                  ,  PLMN_BPL_MO                  ,
    404, 28 ,  TXT_NET_AIRCEL_NEW              ,  PLMN_AIRCEL_NEW_02           ,
    404, 29 ,  TXT_NET_AIRCEL_NEW              ,  PLMN_AIRCEL_NEW_03           ,
    404, 30 ,  TXT_NET_VODAFONE_IN             ,  PLMN_INA_COMMAND             ,
    404, 31 ,  TXT_NET_AIRTEL                  ,  PLMN_INA_31                  ,
    404, 32 ,  TXT_NET_INA_08                  ,  PLMN_INA_32                  ,
    404, 33 ,  TXT_NET_AIRCEL_NEW              ,  PLMN_AIRCEL_NEW_04           ,
    404, 34 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_01              ,
    404, 35 ,  TXT_NET_AIRCEL_NEW              ,  PLMN_AIRCEL_NEW_05           ,   
    404, 36 ,  TXT_NET_INA_09                  ,  PLMN_INA_36                  ,
    404, 37 ,  TXT_NET_AIRCEL_NEW              ,  PLMN_AIRCEL_NEW_06           ,   
    404, 38 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_02              ,	
    404, 40 ,  TXT_NET_AIRTEL                 ,  PLMN_SKYCELL                 ,
    404, 41 ,  TXT_NET_AIRCEL_NEW                     ,  PLMN_RPG                     ,
    404, 42 ,  TXT_NET_AIRCEL_NEW                ,  PLMN_AIRCEL_2                ,
    404, 43 ,  TXT_NET_VODAFONE_IN                  ,  PLMN_INA_43                  ,
    404, 44 ,  TXT_NET_IDEA                   ,  PLMN_IN_44                   ,
    404, 45 ,  TXT_NET_AIRTEL                  ,  PLMN_INA_45                  ,
    404, 46 ,  TXT_NET_VODAFONE_IN                  ,  PLMN_INA_46                  ,
    404, 48 ,  TXT_NET_404_48                  ,  PLMN_404_48                  ,
    404, 49 ,  TXT_NET_AIRTEL                  ,  PLMN_INA_49                  ,
    404, 50 ,  TXT_NET_INA_09                  ,  PLMN_INA_50                  ,
    404, 51 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_03              , 
    404, 52 ,  TXT_NET_INA_09                  ,  PLMN_INA_52                  ,
    404, 53 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_04              , 
    404, 54 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_05              , 
    404, 55 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_06              , 
    404, 56 ,  TXT_NET_IDEA                  ,  PLMN_INA_56                  ,
    404, 57 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_07              , 
    404, 58 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_08              , 
    404, 59 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_09              ,
    404, 60 ,  TXT_NET_VODAFONE_IN             ,  PLMN_VODAFONE_IN_02          ,    
    404, 62 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_10              , 
    404, 64 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_11              , 
    404, 66 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_12              ,	
    404, 67 ,  TXT_NET_INA_09                  ,  PLMN_INA_67                  ,
    404, 68 ,  TXT_NET_DOLPHIN            ,  PLMN_IN_DOLPHIN_1            ,
    404, 69 ,  TXT_NET_DOLPHIN            ,  PLMN_IN_DOLPHIN_2            ,
    404, 70 ,  TXT_NET_AIRTEL                 ,  PLMN_HEXACOM                 ,
    404, 71 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_13              , 
    404, 72 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_14              , 
    404, 73 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_15              ,
    404, 74 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_16              , 
    404, 75 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_17              , 
    404, 76 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_18              ,
    404, 77 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_19              , 
    404, 78 ,  TXT_NET_IDEA                ,  PLMN_CELLULAR                ,
    404, 79 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_20              , 
    404, 80 ,  TXT_NET_BSNL                 ,  PLMN_CELLONE_21              ,
    404, 81 ,  TXT_NET_CELLONE                 ,  PLMN_CELLONE_22              ,
    404, 82 ,  TXT_NET_IDEA                  ,  PLMN_INA_82                  ,
    404, 83 ,  TXT_NET_INA_09                  ,  PLMN_INA_83                  ,
    404, 84 ,  TXT_NET_VODAFONE_IN             ,  PLMN_VODAFONE_IN_03          ,
    404, 85 ,  TXT_NET_INA_09                  ,  PLMN_INA_85                  ,
    404, 86 ,  TXT_NET_VODAFONE_IN                 ,  PLMN_HUTCH_2                 ,
    404, 87 ,  TXT_NET_IDEA                 ,  PLMN_404_87                 ,
    404, 88 ,  TXT_NET_VODAFONE_IN             ,  PLMN_VODAFONE_IN_04          ,
    404, 89 ,  TXT_NET_IDEA                 ,  PLMN_404_89                 ,
    404, 90 ,  TXT_NET_AIRTEL                  ,  PLMN_404_90                  ,
    404, 91 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_404_91                  ,
    404, 92 ,  TXT_NET_AIRTEL                  ,  PLMN_404_92                  ,
    404, 93 ,  TXT_NET_AIRTEL                  ,  PLMN_404_93                  ,
    404, 94 ,  TXT_NET_AIRTEL                  ,  PLMN_404_94                  ,
    404, 95 ,  TXT_NET_AIRTEL                  ,  PLMN_404_95                  ,
    404, 96 ,  TXT_NET_AIRTEL                  ,  PLMN_404_96                  ,
    404, 97 ,  TXT_NET_AIRTEL                  ,  PLMN_404_97                  ,
    404, 98 ,  TXT_NET_AIRTEL                  ,  PLMN_404_98                  ,
    405, 1 ,  TXT_NET_INA_09                  ,  PLMN_405_01              ,
    405, 3 ,  TXT_NET_INA_09                  ,  PLMN_405_03              ,
    405, 4 ,  TXT_NET_INA_09                  ,  PLMN_405_04              ,
    405, 5 ,  TXT_NET_INA_09                  ,  PLMN_405_05              ,
    405, 6 ,  TXT_NET_INA_09                  ,  PLMN_405_06              ,
    405, 7 ,  TXT_NET_INA_09                  ,  PLMN_405_07              ,
    405, 8 ,  TXT_NET_INA_09                  ,  PLMN_405_08              ,
    405, 9 ,  TXT_NET_INA_09                  ,  PLMN_405_09              ,
    405, 10 ,  TXT_NET_INA_09                  ,  PLMN_405_10              ,
    405, 11 ,  TXT_NET_INA_09                  ,  PLMN_405_11              ,
    405, 12 ,  TXT_NET_INA_09                  ,  PLMN_405_12              ,
    405, 13 ,  TXT_NET_INA_09                  ,  PLMN_405_13              ,
    405, 14 ,  TXT_NET_INA_09                  ,  PLMN_405_14              ,
    405, 15 ,  TXT_NET_INA_09                  ,  PLMN_405_15              ,
    405, 17 ,  TXT_NET_INA_09                  ,  PLMN_405_17              ,
    405, 18 ,  TXT_NET_INA_09                  ,  PLMN_405_18              ,
    405, 19 ,  TXT_NET_INA_09                  ,  PLMN_405_19              ,
    405, 20 ,  TXT_NET_INA_09                  ,  PLMN_405_20              ,
    405, 21 ,  TXT_NET_INA_09                  ,  PLMN_405_21              ,
    405, 22 ,  TXT_NET_INA_09                  ,  PLMN_405_22              ,
    405, 23 ,  TXT_NET_INA_09                  ,  PLMN_405_23              ,
    405, 25 ,  TXT_NET_TATADOCO                  ,  PLMN_405_25              ,
    405, 26 ,  TXT_NET_TATADOCO                  ,  PLMN_405_26              ,
    405, 27 ,  TXT_NET_TATADOCO                  ,  PLMN_405_27              ,
    405, 29 ,  TXT_NET_TATADOCO                  ,  PLMN_405_29              ,
    405, 30 ,  TXT_NET_TATADOCO                  ,  PLMN_405_30              ,
    405, 31 ,  TXT_NET_TATADOCO                  ,  PLMN_405_31              ,
    405, 32 ,  TXT_NET_TATADOCO                  ,  PLMN_405_32              ,
    405, 34 ,  TXT_NET_TATADOCO                  ,  PLMN_405_34              ,
    405, 35 ,  TXT_NET_TATADOCO                  ,  PLMN_405_35              ,
    405, 36 ,  TXT_NET_TATADOCO                  ,  PLMN_405_36              ,
    405, 37 ,  TXT_NET_TATADOCO                  ,  PLMN_405_37              ,
    405, 38 ,  TXT_NET_TATADOCO                  ,  PLMN_405_38              ,
    405, 39 ,  TXT_NET_TATADOCO                  ,  PLMN_405_39              ,
    405, 41 ,  TXT_NET_TATADOCO                  ,  PLMN_405_41              ,
    405, 42 ,  TXT_NET_TATADOCO                  ,  PLMN_405_42              ,
    405, 43 ,  TXT_NET_TATADOCO                  ,  PLMN_405_43              ,
    405, 44 ,  TXT_NET_TATADOCO                  ,  PLMN_405_44              ,
    405, 45 ,  TXT_NET_TATADOCO                  ,  PLMN_405_45              ,
    405, 46 ,  TXT_NET_TATADOCO                  ,  PLMN_405_46              ,
    405, 47 ,  TXT_NET_TATADOCO                  ,  PLMN_405_47              ,
    405, 48 ,  TXT_NET_405_48                  ,  PLMN_405_48              ,
    405, 51 ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_03               ,
    405, 52 ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_04               ,
    405, 53 ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_05               ,
    405, 54 ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_06               ,
    405, 55 ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_07               ,
    405, 56 ,  TXT_NET_AIRTEL                  ,  PLMN_AIRTEL_08               ,
    405, 66 ,  TXT_NET_VODAFONE_IN             ,  PLMN_VODAFONE_IN_05          ,
    405, 67 ,  TXT_NET_VODAFONE_IN             ,  PLMN_VODAFONE_IN_06          ,
    405, 70 ,  TXT_NET_IDEA                  ,  PLMN_405_70              ,
    405, 750 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_750          ,
    405, 751 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_751          ,
    405, 752 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_752          ,
    405, 753 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_753          ,
    405, 754 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_754          ,
    405, 755 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_755          ,
    405, 756 ,  TXT_NET_VODAFONE_IN             ,  PLMN_405_756          ,
    405, 799 ,  TXT_NET_IDEA             ,  PLMN_MUMBAI_IDEA          ,
    405, 800 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_800             ,
    405, 801 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_801             ,
    405, 802 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_802             ,
    405, 803 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_803             ,
    405, 804 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_804             ,
    405, 805 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_805             ,
    405, 806 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_806             ,
    405, 807 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_807             ,
    405, 808 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_808             ,
    405, 809 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_809             ,
    405, 810 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_810             ,
    405, 811 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_811             ,
    405, 812 ,  TXT_NET_AIRCEL_NEW                  ,  PLMN_405_812             ,
    405, 813 ,  TXT_NET_UNINOR                  ,  PLMN_405_813            ,
    405, 814 ,  TXT_NET_UNINOR                  ,  PLMN_405_814             ,
    405, 815 ,  TXT_NET_UNINOR                  ,  PLMN_405_815             ,
    405, 816 ,  TXT_NET_UNINOR                  ,  PLMN_405_816             ,
    405, 817 ,  TXT_NET_UNINOR                  ,  PLMN_405_817             ,
    405, 818 ,  TXT_NET_UNINOR                  ,  PLMN_405_818             ,
    405, 819 ,  TXT_NET_UNINOR                  ,  PLMN_405_819             ,
    405, 820 ,  TXT_NET_UNINOR                  ,  PLMN_405_820             ,
    405, 821 ,  TXT_NET_UNINOR                  ,  PLMN_405_821             ,
    405, 822 ,  TXT_NET_UNINOR                  ,  PLMN_405_822             ,
    405, 823 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_823             ,
    405, 824 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_824             ,
    405, 825 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_825             ,
    405, 826 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_826,
    405, 827 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_827             ,
    405, 828 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_828             ,
    405, 829 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_829,
    405, 830 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_830,
    405, 831 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_831,
    405, 832 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_832,
    405, 833 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_833,
    405, 834 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_834             ,
    405, 835 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_835,
    405, 836 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_836,
    405, 837 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_837,
    405, 838 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_838,
    405, 839 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_839,
    405, 840 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_840             ,
    405, 841 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_841,
    405, 842 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_842,
    405, 843 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_843,
    405, 844 ,  TXT_NET_UNINOR                  ,  PLMN_405_844             ,
    405, 845 ,  TXT_NET_IDEA                  ,  PLMN_405_845             ,
    405, 846 ,  TXT_NET_IDEA                  ,  PLMN_405_846,
    405, 847 ,  TXT_NET_IDEA                  ,  PLMN_405_847,
    405, 848 ,  TXT_NET_IDEA                  ,  PLMN_405_848             ,
    405, 849 ,  TXT_NET_IDEA                  ,  PLMN_405_849             ,
    405, 850 ,  TXT_NET_IDEA                  ,  PLMN_405_850             ,
    405, 851 ,  TXT_NET_IDEA                  ,  PLMN_405_851             ,
    405, 852 ,  TXT_NET_IDEA                  ,  PLMN_405_852             ,
    405, 853 ,  TXT_NET_IDEA                  ,  PLMN_405_853             ,
    405, 854 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_854             ,
    405, 855 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_855             ,
    405, 856 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_856             ,
    405, 857 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_857             ,
    405, 858 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_858             ,
    405, 859 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_859             ,
    405, 860 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_860             ,
    405, 861 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_861             ,
    405, 862 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_862             ,
    405, 863 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_863             ,
    405, 864 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_864             ,
    405, 865 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_865             ,
    405, 866 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_866,
    405, 867 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_867,
    405, 868 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_868,
    405, 869 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_869             ,
    405, 870 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_870             ,
    405, 871 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_871,
    405, 872 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_872,
    405, 873 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_873,
    405, 874 ,  TXT_NET_LOOP_MOBILE                  ,  PLMN_405_874,
    405, 875 ,  TXT_NET_UNINOR                  ,  PLMN_405_875             ,
    405, 876 ,  TXT_NET_UNINOR                  ,  PLMN_405_876,
    405, 877 ,  TXT_NET_UNINOR                  ,  PLMN_405_877,
    405, 878 ,  TXT_NET_UNINOR                  ,  PLMN_405_878             ,
    405, 879 ,  TXT_NET_UNINOR                  ,  PLMN_405_879             ,
    405, 880 ,  TXT_NET_UNINOR                  ,  PLMN_405_880             ,
    405, 881 ,  TXT_NET_STEL                  ,  PLMN_405_881             ,
    405, 882 ,  TXT_NET_STEL                  ,  PLMN_405_882             ,
    405, 883 ,  TXT_NET_STEL                  ,  PLMN_405_883             ,
    405, 884 ,  TXT_NET_STEL                  ,  PLMN_405_884             ,
    405, 885 ,  TXT_NET_STEL                  ,  PLMN_405_885             ,
    405, 886 ,  TXT_NET_STEL                  ,  PLMN_405_886             ,
    405, 887 ,  TXT_NET_MTS_IN                  ,  PLMN_405_887,
    405, 888 ,  TXT_NET_MTS_IN                  ,  PLMN_405_888,
    405, 889 ,  TXT_NET_MTS_IN                  ,  PLMN_405_889,
    405, 890 ,  TXT_NET_MTS_IN                  ,  PLMN_405_890,
    405, 891 ,  TXT_NET_MTS_IN                  ,  PLMN_405_891,
    405, 892 ,  TXT_NET_MTS_IN                  ,  PLMN_405_892,
    405, 893 ,  TXT_NET_MTS_IN                  ,  PLMN_405_893,
    405, 894 ,  TXT_NET_MTS_IN                  ,  PLMN_405_894,
    405, 895 ,  TXT_NET_MTS_IN                  ,  PLMN_405_895,
    405, 896 ,  TXT_NET_MTS_IN                  ,  PLMN_405_896,
    405, 897 ,  TXT_NET_MTS_IN                  ,  PLMN_405_897,
    405, 898 ,  TXT_NET_MTS_IN                  ,  PLMN_405_898,
    405, 899 ,  TXT_NET_MTS_IN                  ,  PLMN_405_899,
    405, 900 ,  TXT_NET_MTS_IN                  ,  PLMN_405_900,
    405, 901 ,  TXT_NET_MTS_IN                  ,  PLMN_405_901,
    405, 902 ,  TXT_NET_MTS_IN                  ,  PLMN_405_902,
    405, 903 ,  TXT_NET_MTS_IN                  ,  PLMN_405_903,
    405, 904 ,  TXT_NET_MTS_IN                  ,  PLMN_405_904,
    405, 905 ,  TXT_NET_MTS_IN                  ,  PLMN_405_905,
    405, 906 ,  TXT_NET_MTS_IN                  ,  PLMN_405_906,
    405, 907 ,  TXT_NET_MTS_IN                  ,  PLMN_405_907,
    405, 908 ,  TXT_NET_IDEA                  ,  PLMN_405_908,
    405, 909 ,  TXT_NET_IDEA                  ,  PLMN_405_909,
    405, 910 ,  TXT_NET_IDEA                  ,  PLMN_405_910,
    405, 911 ,  TXT_NET_IDEA                  ,  PLMN_405_911,
    405, 912 ,  TXT_NET_ETISALAT                  ,  PLMN_405_912             ,
    405, 913 ,  TXT_NET_ETISALAT                  ,  PLMN_405_913             ,
    405, 914 ,  TXT_NET_ETISALAT                  ,  PLMN_405_914             ,
    405, 915 ,  TXT_NET_ETISALAT                  ,  PLMN_405_915             ,
    405, 916 ,  TXT_NET_ETISALAT                  ,  PLMN_405_916             ,
    405, 917 ,  TXT_NET_ETISALAT                  ,  PLMN_405_917             ,
    405, 918 ,  TXT_NET_ETISALAT                  ,  PLMN_405_918             ,
    405, 919 ,  TXT_NET_ETISALAT                  ,  PLMN_405_919             ,
    405, 920 ,  TXT_NET_ETISALAT                  ,  PLMN_405_920             ,
    405, 921 ,  TXT_NET_ETISALAT                  ,  PLMN_405_921             ,
    405, 922 ,  TXT_NET_ETISALAT                  ,  PLMN_405_922             ,
    405, 923 ,  TXT_NET_ETISALAT                  ,  PLMN_405_923             ,
    405, 924 ,  TXT_NET_ETISALAT                  ,  PLMN_405_924             ,
    405, 925 ,  TXT_NET_UNINOR                  ,  PLMN_405_925             ,
    405, 926 ,  TXT_NET_UNINOR                  ,  PLMN_405_926             ,
    405, 927 ,  TXT_NET_UNINOR                  ,  PLMN_405_927             ,
    405, 928 ,  TXT_NET_UNINOR                  ,  PLMN_405_928             ,
    405, 929 ,  TXT_NET_UNINOR                  ,  PLMN_405_929             ,
    405, 930 ,  TXT_NET_ALLIANZ                  ,  PLMN_405_930             ,
    405, 931 ,  TXT_NET_ALLIANZ                  ,  PLMN_405_931             ,
    405, 932 ,  TXT_NET_VIDEOCON                  ,  PLMN_405_932             ,
    412, 1  ,  TXT_NET_412_01                  ,  PLMN_412_01                  ,
    413, 2  ,  TXT_NET_SRI_DIALOG              ,  PLMN_SRI_DIALOG              ,
    413, 3  ,  TXT_NET_SRI_CELLTEL             ,  PLMN_SRI_CELLTEL             ,
    414, 1  ,  TXT_NET_414_01                  ,  PLMN_414_01                  ,
    415, 1  ,  TXT_NET_RL_CELLIS               ,  PLMN_RL_CELLIS               ,
    415, 3  ,  TXT_NET_RL_LIBANCELL            ,  PLMN_RL_LIBANCELL            ,
    416, 1  ,  TXT_NET_JOR_FASTLINK            ,  PLMN_JOR_FASTLINK            ,
    416, 2  ,  TXT_NET_XPRESS            ,  PLMN_XPRESS            ,
    416, 2  ,  TXT_NET_UMNIAH            ,  PLMN_UMNIAH,
    416, 77 ,  TXT_NET_MOBILECOM               ,  PLMN_MOBILECOM               ,
    417, 2  ,  TXT_NET_417_02                  ,  PLMN_417_02                  ,
    417, 9  ,  TXT_NET_SYR_MOB                 ,  PLMN_SYR_MOB                 ,
    417, 93 ,  TXT_NET_417_93                  ,  PLMN_417_93                  ,
    419, 2  ,  TXT_NET_KT_MTCNET               ,  PLMN_KT_MTCNET               ,
    419, 3  ,  TXT_NET_419_03                  ,  PLMN_419_03                  ,
    420, 1  ,  TXT_NET_KSA_AL_JAWAL            ,  PLMN_KSA_AL_JAWAL            ,
    420, 3  ,  TXT_NET_420_03            ,  PLMN_420_03            ,
    420, 4  ,  TXT_NET_420_04            ,  PLMN_420_04            ,
    421, 2  ,  TXT_NET_421_02                  ,  PLMN_421_02                  ,
    422, 2  ,  TXT_NET_OMAN_MOBILE             ,  PLMN_OMAN_MOBILE             ,
    424, 2  ,  TXT_NET_ETSLT                   ,  PLMN_ETSLT                   ,
    424, 3  ,  TXT_NET_424_03                   ,  PLMN_424_03                   ,
    425, 1  ,  TXT_NET_ORANGE               ,  PLMN_IL_ORANGE               ,
    425, 5  ,  TXT_NET_JAWWAL_PALESTINE        ,  PLMN_JAWWAL_PALESTINE        ,
    426, 1  ,  TXT_NET_BHR_MOBILE_PLUS         ,  PLMN_BHR_MOBILE_PLUS         ,
    427, 1  ,  TXT_NET_QATARNET                ,  PLMN_QATARNET                ,
    428, 99 ,  TXT_NET_MOBICOM                 ,  PLMN_MOBICOM                 ,
    429, 1  ,  TXT_NET_250_16                  ,  PLMN_429_01                  ,
    432, 11 ,  TXT_NET_IR_TCI                  ,  PLMN_IR_TCI                  ,
    432, 14 ,  TXT_NET_IR_KISH                 ,  PLMN_IR_KISH                 ,
    432, 19 , TXT_NET_432_19                   ,PLMN_432_19    ,
    432, 32 , TXT_NET_TALIYA                   ,PLMN_IR_TALIYA    ,
    432, 35,  TXT_NET_IRANCELL              ,   PLMN_IR_IRANCELL, 
    432, 70,  TXT_NET_432_70              ,   PLMN_432_70, 
    434, 1  ,  TXT_NET_BUZTEL                  ,  PLMN_BUZTEL                  ,
    434, 2  ,  TXT_NET_UZMACOM                 ,  PLMN_UZMACOM                 ,
    434, 4  ,  TXT_NET_UZB_DAEWOO_GSM          ,  PLMN_UZB_DAEWOO_GSM          ,
    434, 5  ,  TXT_NET_UZB_COSCOM_GSM          ,  PLMN_UZB_COSCOM_GSM          ,
    437, 1  ,  TXT_NET_BITEL               ,  PLMN_BITEL_KGZ               ,
    437, 5  ,  TXT_NET_BITEL                   ,  PLMN_BITEL                   ,
    438, 1  ,  TXT_NET_BCTI                    ,  PLMN_BCTI                    ,
    440, 0  ,  TXT_NET_EMOBILE                  ,  PLMN_EMOBILE                  ,
    440, 1  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_NTT                  ,
    440, 2  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_02                  ,
    440, 3  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_03                  ,
    440, 4  ,  TXT_NET_SOFTBANK                  ,  PLMN_SOFTBANK,
    440, 6  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_06                  ,
    440, 7  ,  TXT_NET_KDDI                  ,  PLMN_KDDI,
    440, 8  ,  TXT_NET_KDDI                  ,  PLMN_440_08                  ,
    440, 9  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_NTT_09                  ,
    440, 10  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_10                  ,
    440, 11  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_11                  ,
    440, 12  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_12                  ,
    440, 13  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_13                  ,
    440, 14  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_14                  ,
    440, 15  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_15                  ,
    440, 16  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_16                  ,
    440, 17  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_17                  ,
    440, 18  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_18                  ,
    440, 19  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_19                  ,
    440, 20  ,  TXT_NET_SOFTBANK                  ,  PLMN_SOFTBANK_20                  ,
    440, 21  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_21                  ,
    440, 22  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_22                  ,
    440, 23  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_23                  ,
    440, 24  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_24                  ,
    440, 25  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_25                  ,
    440, 26  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_26                  ,
    440, 27  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_27                  ,
    440, 28  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_28                  ,
    440, 29  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_29                  ,
    440, 30  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_30                  ,
    440, 31  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_31                  ,
    440, 32  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_32                  ,
    440, 33  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_33                  ,
    440, 34  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_34                  ,
    440, 35  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_35                  ,
    440, 36  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_36                  ,
    440, 37  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_37                  ,
    440, 38  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_38                  ,
    440, 39  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_39                  ,
    440, 40  ,  TXT_NET_SOFTBANK                  ,  PLMN_SOFTBANK_40                  ,
    440, 41  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_41                  ,
    440, 42  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_42                  ,
    440, 43  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_43                  ,
    440, 44  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_44                  ,
    440, 45  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_45                  ,
    440, 46  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_46                  ,
    440, 47  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_47                  ,
    440, 48  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_48                  ,
    440, 49  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_NTT_49,
    440, 50  ,  TXT_NET_KDDI                  ,  PLMN_KDDI_50,
    440, 51  ,  TXT_NET_KDDI                  ,  PLMN_440_51,
    440, 52  ,  TXT_NET_KDDI                  ,  PLMN_440_52,
    440, 53  ,  TXT_NET_KDDI                  ,  PLMN_440_53,
    440, 54  ,  TXT_NET_KDDI                  ,  PLMN_440_54,
    440, 55  ,  TXT_NET_KDDI                  ,  PLMN_440_55,
    440, 56  ,  TXT_NET_KDDI                  ,  PLMN_440_56,
    440, 58  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_NTT_58,
    440, 60  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_60                  ,
    440, 61  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_61                  ,
    440, 62  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_62                  ,
    440, 63  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_63                  ,
    440, 64  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_64                  ,
    440, 65  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_65                  ,
    440, 66  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_66                  ,
    440, 67  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_67                  ,
    440, 68  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_68                  ,
    440, 69  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_440_69                  ,
    440, 70  ,  TXT_NET_KDDI                  ,  PLMN_KDDI_70,
    440, 71  ,  TXT_NET_KDDI                  ,  PLMN_440_71,
    440, 72  ,  TXT_NET_KDDI                  ,  PLMN_440_72,
    440, 73  ,  TXT_NET_KDDI                  ,  PLMN_440_73,
    440, 74  ,  TXT_NET_KDDI                  ,  PLMN_440_74,
    440, 75  ,  TXT_NET_KDDI                  ,  PLMN_440_75,
    440, 76  ,  TXT_NET_KDDI                  ,  PLMN_440_76,
    440, 77  ,  TXT_NET_KDDI                  ,  PLMN_440_77,
    440, 79  ,  TXT_NET_KDDI                  ,  PLMN_440_79,
    440, 80  ,  TXT_NET_TU_KA                  ,  PLMN_TU_KA,
    440, 81  ,  TXT_NET_TU_KA                  ,  PLMN_440_81,
    440, 82  ,  TXT_NET_TU_KA                  ,  PLMN_440_82,
    440, 83  ,  TXT_NET_TU_KA                  ,  PLMN_440_83,
    440, 84  ,  TXT_NET_TU_KA                  ,  PLMN_440_84,
    440, 85  ,  TXT_NET_TU_KA                  ,  PLMN_440_85,
    440, 86  ,  TXT_NET_TU_KA                  ,  PLMN_440_86,
    440, 87  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_NTT_87,
    440, 88  ,  TXT_NET_KDDI                  ,  PLMN_KDDI_88,
    440, 89  ,  TXT_NET_KDDI                  ,  PLMN_440_89,
    440, 90  ,  TXT_NET_SOFTBANK                  ,  PLMN_SOFTBANK_90,
    440, 92  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_92,
    440, 93  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_93,
    440, 94  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_94,
    440, 95  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_95,
    440, 96  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_96,
    440, 97  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_97,
    440, 98  ,  TXT_NET_SOFTBANK                  ,  PLMN_440_98,
    440, 99  ,  TXT_NET_NTT_DOCOMO                  ,  PLMN_NTT_99,
    450, 2  ,  TXT_NET_KTF                  ,  PLMN_450_02                  ,
    450, 3  ,  TXT_NET_DIGITAL                  ,  PLMN_450_03                  ,
    450, 4  ,  TXT_NET_KTF                  ,  PLMN_450_04                  ,
    450, 5  ,  TXT_NET_SKT                  ,  PLMN_450_05                  ,
    450, 6  ,  TXT_NET_LGT                  ,  PLMN_450_06                  ,
    450, 8  ,  TXT_NET_KTFSHOW                  ,  PLMN_450_08                  ,
    452, 1  ,  TXT_NET_452_01                  ,  PLMN_452_01                  ,
    452, 2  ,  TXT_NET_452_02                  ,  PLMN_452_02                  ,
    452, 3  ,  TXT_NET_452_03				   ,  PLMN_452_03				   ,
    452, 4  ,  TXT_NET_452_04				   ,  PLMN_452_04				   ,
    452, 5  ,  TXT_NET_452_05				   ,  PLMN_452_05				   ,
    452, 6  ,  TXT_NET_452_06				   ,  PLMN_452_06				   ,
    452, 7  ,  TXT_NET_452_07                  ,  PLMN_452_07                  ,
    454, 20 ,  TXT_NET_HUTCHISON               ,  PLMN_HUTCHISON               ,
    455, 3  ,  TXT_NET_HUTCHISON_MAC           ,  PLMN_HUTCHISON_MAC           ,
    456, 1  ,  TXT_NET_MOBITEL_KHM             ,  PLMN_MOBITEL_KHM             ,
    456, 2  ,  TXT_NET_KHM_SAMART_GSM          ,  PLMN_KHM_SAMART_GSM          ,
    456, 4  ,  TXT_NET_KH_QB          ,  PLMN_456_04          ,
    456, 5  ,  TXT_NET_KH_STARCELL          ,  PLMN_456_05          ,
    456, 6  ,  TXT_NET_KH_SMART          ,  PLMN_456_06          ,
    456, 8  ,  TXT_NET_KH_METFONE          ,  PLMN_456_08          ,
    456, 9  ,  TXT_NET_KH_BEELINE          ,  PLMN_456_09          ,
    456, 18 ,  TXT_NET_CAMSHIN                 ,  PLMN_CAMSHIN                 ,
    457, 1  ,  TXT_NET_457_01                  ,  PLMN_457_01                  ,
    457, 2  ,  TXT_NET_457_02                  ,  PLMN_457_02                  ,
    470, 1  ,  TXT_NET_GRAMEENPHONE                  ,  PLMN_BGD_GP                  ,
    470, 2  ,  TXT_NET_ROBI                   ,  PLMN_AKTEL                   ,
    470, 3  ,  TXT_NET_BANGLALINK                ,  PLMN_BANGLALINK                   ,
    470, 4  ,  TXT_NET_TELETALK                   ,  PLMN_TELTALK                   ,
    470, 5  ,  TXT_NET_470_05                   ,  PLMN_470_05                   ,
    470, 6  ,  TXT_NET_AIRTEL                   ,  PLMN_470_06                   ,
    470, 7  ,  TXT_NET_AIRTEL                   ,  PLMN_WARID                   ,
    472, 1  ,  TXT_NET_D_MOBILE                ,  PLMN_D_MOBILE                ,
    502, 12 ,  TXT_NET_MY_MAXIS                ,  PLMN_MY_MAXIS                ,
    502, 13 ,  TXT_NET_TM_TOUCH                ,  PLMN_TM_TOUCH                ,
    502, 16 ,  TXT_NET_DIGI                    ,  PLMN_DIGI                    ,
    502, 17 ,  TXT_NET_MY_ADAM                 ,  PLMN_MY_ADAM                 ,
    502, 19 ,  TXT_NET_MY_CELCOM               ,  PLMN_MY_CELCOM               ,
    505, 1  ,  TXT_NET_TELSTRA_MOBILENET       ,  PLMN_TELSTRA_MOBILENET       ,
    505, 2  ,  TXT_NET_CABLE_WIRELESS_OPTUS    ,  PLMN_CABLE_WIRELESS_OPTUS    ,
    505, 3  ,  TXT_NET_VODAFONE_AUS            ,  PLMN_VODAFONE_AUS            ,
    510, 0  ,  TXT_NET_IND_PSN        ,  PLMN_510_00        ,
    510, 1  ,  TXT_NET_IND_SATELINDOCEL        ,  PLMN_IND_SATELINDOCEL        ,
    510, 3  ,  TXT_NET_IND_STARONE        ,  PLMN_510_03        ,
    510, 7  ,  TXT_NET_IND_TELKOMFLEXI        ,  PLMN_510_07        ,
    510, 8  ,  TXT_NET_LIPPO_TEL               ,  PLMN_LIPPO_TEL               ,
    510, 9  ,  TXT_NET_IND_SMART               ,  PLMN_510_09               ,
    510, 10 ,  TXT_NET_TELKOMSEL               ,  PLMN_TELKOMSEL               ,
    510, 11 ,  TXT_NET_PROXL                   ,  PLMN_PROXL                   ,
    510, 20 ,  TXT_NET_IND_TELKOMMOBILE                   ,  PLMN_510_20                   ,
    510, 21 ,  TXT_NET_IM_3                    ,  PLMN_IM_3                    ,
    510, 27 ,  TXT_NET_IND_CERIA                    ,  PLMN_510_27                    ,
    510, 28 ,  TXT_NET_IND_FREN                    ,  PLMN_510_28                    ,
    510, 89 ,  TXT_NET_IND_3                    ,  PLMN_510_89                    ,
    510, 99 ,  TXT_NET_IND_ESIA                    ,  PLMN_510_99                    ,
    515, 1  ,  TXT_NET_ISLACOM                 ,  PLMN_ISLACOM                 ,
    515, 2  ,  TXT_NET_GLOBE_TELECOM_PH        ,  PLMN_GLOBE_TELECOM_PH        ,
    515, 3  ,  TXT_NET_SMART_PH03              ,  PLMN_SMART_PH03              ,
    515, 5  ,  TXT_NET_PH_DIGITEL              ,  PLMN_PH_DIGITEL              ,
    520, 1  ,  TXT_NET_TH_AIS_GSM              ,  PLMN_TH_AIS_GSM              ,
    520, 15 ,  TXT_NET_TH_ACT_1900             ,  PLMN_TH_ACT_1900             ,
    520, 18 ,  TXT_NET_WP_1800                 ,  PLMN_WP_1800                 ,
    520, 23 ,  TXT_NET_TH_HELLO                ,  PLMN_TH_HELLO                ,
    520, 99 ,  TXT_NET_TH_ORANGE               ,  PLMN_TH_ORANGE               ,
    520, 10 ,  TXT_NET_WCS                     ,  PLMN_WCS                     ,
    525, 1  ,  TXT_NET_ST_GSM_SGP              ,  PLMN_ST_GSM_SGP              ,
    525, 2  ,  TXT_NET_STPCN                   ,  PLMN_STPCN                   ,
    525, 3  ,  TXT_NET_M1_GSM_SGP              ,  PLMN_M1_GSM_SGP              ,
    525, 5  ,  TXT_NET_STARHUB                 ,  PLMN_STARHUB                 ,
    528, 11 ,  TXT_NET_BRU_DSTCOM              ,  PLMN_BRU_DSTCOM              ,
    530, 1  ,  TXT_NET_VODAFONE_1             ,  PLMN_NZ_VODAFONE             ,
    539, 1  ,  TXT_NET_U_CALL                  ,  PLMN_U_CALL                  ,
    541, 1  ,  TXT_NET_SMILE                   ,  PLMN_SMILE                   ,
    542, 1  ,  TXT_NET_VODAFONE_1           ,  PLMN_VODAFONE_FIJI           ,
    544, 11 ,  TXT_NET_BLUE_SKY                ,  PLMN_BLUE_SKY                ,
    546, 1  ,  TXT_NET_MOBILIS                 ,  PLMN_MOBILIS                 ,
    547, 20 ,  TXT_NET_VINI                    ,  PLMN_VINI                    ,
    550, 1  ,  TXT_NET_550_01                  ,  PLMN_550_01                  ,
    602, 1  ,  TXT_NET_ECMS_MOBINIL            ,  PLMN_ECMS_MOBINIL            ,
    602, 2  ,  TXT_NET_VODAFONE_1               ,  PLMN_EGY_CLICK               ,
    602, 3  ,  TXT_NET_ETSLT               ,  PLMN_602_03               ,
    603, 1  ,  TXT_NET_ALGERIAN_MOBILE_NETWORK ,  PLMN_ALGERIAN_MOBILE_NETWORK ,
    603, 2  ,  TXT_NET_603_02                  ,  PLMN_603_02                  ,
    604, 0  ,  TXT_NET_MEDITEL                 ,  PLMN_MEDITEL                 ,
    604, 1  ,  TXT_NET_MORIAM                  ,  PLMN_MORIAM                  ,
    605, 01  ,  TXT_NET_ORANGE                 ,  PLMN_605_01,
    605, 2  ,  TXT_NET_TUNICELL                ,  PLMN_TUNICELL                ,
    605, 3  ,  TXT_NET_TUNISIANA                ,  PLMN_TUNICELL                ,
    607, 1  ,  TXT_NET_607_01                  ,  PLMN_607_01                  ,
    607, 2  ,  TXT_NET_607_02                  ,  PLMN_607_02                  ,
    608, 1  ,  TXT_NET_SEN_ALIZE               ,  PLMN_SEN_ALIZE               ,
    608, 2  ,  TXT_NET_SENTEL                  ,  PLMN_SENTEL                  ,
    608, 3  ,  TXT_NET_EXPRESSO                  ,  PLMN_608_03,
    610, 1  ,  TXT_NET_MALITEL                 ,  PLMN_MALITEL                 ,
    610, 2  ,  TXT_NET_610_02                ,  PLMN_610_02                 ,
    611, 1  ,  TXT_NET_MOBILIS              ,  PLMN_GN_MOBILIS              ,
    611, 2  ,  TXT_NET_GN_LAGUI                ,  PLMN_GN_LAGUI                ,
    612, 1  ,  TXT_NET_CORA                    ,  PLMN_CORA                    ,
    612, 2  ,  TXT_NET_MOOV                    ,  PLMN_612_02,
    612, 3  ,  TXT_NET_ORANGE              ,  PLMN_CI_IVOIRIS              ,
    612, 4  ,  TXT_NET_KOZ              ,  PLMN_612_04,
    612, 5  ,  TXT_NET_TELCEL              ,  PLMN_TELECEL_CI              ,
    612, 6  ,  TXT_NET_ORICEL              ,  PLMN_612_06,
    613, 1  ,  TXT_NET_ONATEL                  ,  PLMN_ONATEL                  ,
    613, 2  ,  TXT_NET_CELTEL               ,  PLMN_CELTEL_BF               ,
    614, 1  ,  TXT_NET_SAHELCOM               ,  PLMN_614_01               ,
    614, 2  ,  TXT_NET_CELTEL               ,  PLMN_RN_CELTEL               ,
    614, 3  ,  TXT_NET_TELCEL               ,  PLMN_614_03               ,
    614, 04  ,  TXT_NET_ORANGE                 ,  PLMN_614_04,
    615, 1  ,  TXT_NET_TOGOCEL                 ,  PLMN_TOGOCEL                 ,
    616, 1  ,  TXT_NET_LIBERCOM                ,  PLMN_LIBERCOM                ,
    616, 2  ,  TXT_NET_TELECEL_BENIN           ,  PLMN_TELECEL_BENIN           ,
    616, 3  ,  TXT_NET_BENCELL                 ,  PLMN_BENCELL                 ,
    617, 1  ,  TXT_NET_CELLPLUS_MRU            ,  PLMN_CELLPLUS_MRU            ,
    617, 10 ,  TXT_NET_EMTEL                   ,  PLMN_EMTEL                   ,
    618, 1  ,  TXT_NET_LONESTAR                ,  PLMN_LONESTAR                ,
    620, 1  ,  TXT_NET_MTN_SA             ,  PLMN_GH_SPACEFON             ,
    620, 2  ,  TXT_NET_VODAFONE_1             ,  PLMN_GH_ONETOUCH             ,
    620, 3  ,  TXT_NET_744_04                 ,  PLMN_MOBITEL                 ,
    620, 6  ,  TXT_NET_AIRTEL                 ,  PLMN_AIRTEL_620                 ,
    621, 20 ,  TXT_NET_ECONET                  ,  PLMN_ECONET                  ,
    621, 30 ,  TXT_NET_MTN_NG                  ,  PLMN_MTN_NG                  ,
    621, 40 ,  TXT_NET_M_TEL_GSM_BG                  ,  PLMN_621_40                  ,
    621, 50 ,  TXT_NET_621_50                  ,  PLMN_621_50                  ,
    621, 60 ,  TXT_NET_ETSLT                  ,  PLMN_621_60                  ,
    622, 1  ,  TXT_NET_CELTEL            ,  PLMN_CHA_CELTEL_1            ,
    622, 2  ,  TXT_NET_GO_LIBERTIS          ,  PLMN_CHA_LIBERTIS_1          ,
    623, 1  ,  TXT_NET_CTP                 ,  PLMN_623_01,
    623, 2  ,  TXT_NET_TC                 ,  PLMN_623_02,
    623, 03  ,  TXT_NET_ORANGE                 ,  PLMN_623_03,
    623, 4  ,  TXT_NET_NATIONLINK                 ,  PLMN_623_04,
    624, 1  ,  TXT_NET_CAM_CELLNET             ,  PLMN_CAM_CELLNET             ,
    624, 2  ,  TXT_NET_ORANGE             ,  PLMN_CAM_MOBILIS             ,
    625, 1  ,  TXT_NET_CPVMOVEL                ,  PLMN_CPVMOVEL                ,
    627, 01  ,  TXT_NET_ORANGE                 ,  PLMN_627_01,
    627, 03  ,  TXT_NET_HITS                 ,  PLMN_627_03,
    628, 1  ,  TXT_NET_GO_LIBERTIS             ,  PLMN_GO_LIBERTIS             ,
    628, 2  ,  TXT_NET_628_02                  ,  PLMN_628_02                  ,
    628, 3  ,  TXT_NET_CELTEL                  ,  PLMN_CELTEL                  ,
    629, 10 ,  TXT_NET_GO_LIBERTIS            ,  PLMN_COG_LIBERTIS            ,
    630, 1  ,  TXT_NET_RCB_CELLNET             ,  PLMN_RCB_CELLNET             ,
    630, 2  ,  TXT_NET_CELTEL              ,  PLMN_RCB_CELTEL              ,
    630, 4  ,  TXT_NET_CELLCO                  ,  PLMN_CELLCO                  ,
    630, 89 ,  TXT_NET_OASIS                   ,  PLMN_OASIS                   ,
    631, 2  ,  TXT_NET_UNITEL                  ,  PLMN_UNITEL                  ,
    632, 02  ,  TXT_NET_AREEBA                 ,  PLMN_632_02,
    632, 03  ,  TXT_NET_ORANGE                 ,  PLMN_632_03,
    633, 1  ,  TXT_NET_SEZ_SEYCEL              ,  PLMN_SEZ_SEYCEL              ,
    633, 10 ,  TXT_NET_AIRTEL              ,  PLMN_SEZ_AIRTEL              ,
    634, 1  ,  TXT_NET_SDN_MOB                 ,  PLMN_SDN_MOB                 ,
    634, 2  ,  TXT_NET_634_02                 ,  PLMN_634_02                 ,
    634, 7  ,  TXT_NET_634_07                 ,  PLMN_634_07                 ,
    635, 10 ,  TXT_NET_RCELL                   ,  PLMN_RCELL                   ,
    636, 1  ,  TXT_NET_ETH_ETA                 ,  PLMN_ETH_ETA                 ,
    637, 1  ,  TXT_NET_BARAKAAT                ,  PLMN_BARAKAAT                ,
    637, 10 ,  TXT_NET_NATIONLINK              ,  PLMN_NATIONLINK              ,
    637, 82 ,  TXT_NET_TELSOM                  ,  PLMN_TELSOM                  ,
    639, 2  ,  TXT_NET_SAFARICOM                     ,  PLMN_PTT                     ,
    639, 3  ,  TXT_NET_ZAINKE              ,  PLMN_KE_KENCELL              ,
    639, 5  ,  TXT_NET_YU              ,  PLMN_YU              ,
    639, 7  ,  TXT_NET_ORANGE              ,  PLMN_639_07              ,
    640, 1  ,  TXT_NET_TRITEL_TZ               ,  PLMN_TRITEL_TZ               ,
    640, 2  ,  TXT_NET_640_02                  ,  PLMN_640_02                  ,
    640, 3  ,  TXT_NET_TZ_03                   ,  PLMN_TZ_03                   ,
    640, 4  ,  TXT_NET_VODACOM                 ,  PLMN_VODACOM                 ,
    640, 5  ,  TXT_NET_640_05                  ,  PLMN_640_05                  ,
    641, 1  ,  TXT_NET_CELTEL_CELLULAR         ,  PLMN_CELTEL_CELLULAR         ,
    641, 10 ,  TXT_NET_MTN_UGANDA              ,  PLMN_MTN_UGANDA              ,
    641, 11 ,  TXT_NET_UTL_MOBILE              ,  PLMN_UTL_MOBILE              ,
    641, 14 ,  TXT_NET_ORANGE_UG                 ,  PLMN_641_14,
    641, 22 ,  TXT_NET_WARID             ,  PLMN_641_22              ,//TXT_NET_ORANGE
    642, 1  ,  TXT_NET_SPACETEL_BDI            ,  PLMN_SPACETEL_BDI            ,
    642, 2  ,  TXT_NET_SAFARIS                 ,  PLMN_SAFARIS                 ,
    643, 1  ,  TXT_NET_MOZ_MCEL                ,  PLMN_MOZ_MCEL                ,
    643, 4  ,  TXT_NET_VODACOM                ,  PLMN_643_04                ,
    645, 1  ,  TXT_NET_645_01                  ,  PLMN_645_01                  ,
    645, 2  ,  TXT_NET_654_02                  ,  PLMN_654_02                  ,
    646, 1  ,  TXT_NET_AIRTEL                  ,  PLMN_646_01                  ,
    646, 2  ,  TXT_NET_MDG_ANTARIS             ,  PLMN_MDG_ANTARIS             ,
    646, 3  ,  TXT_NET_MDG_SACEL             ,  PLMN_646_03             ,
    646, 4  ,  TXT_NET_MDG_TELMA             ,  PLMN_646_04             ,
    647, 0  ,  TXT_NET_ORANGE               ,  PLMN_ORANGE_RE               ,
    647, 2  ,  TXT_NET_OUTREMER                  ,  PLMN_647_02                  ,
    647, 10 ,  TXT_NET_SFR_REUNION             ,  PLMN_SFR_REUNION             ,
    648, 1  ,  TXT_NET_ZWNET_ONE               ,  PLMN_ZWNET_ONE               ,
    648, 3  ,  TXT_NET_648_03                  ,  PLMN_648_03                  ,
    648, 4  ,  TXT_NET_ZW_04                   ,  PLMN_ZW_04                   ,
    649, 1  ,  TXT_NET_MTC_NAM                 ,  PLMN_MTC_NAM                 ,
    650, 1  ,  TXT_NET_MW_CP_900               ,  PLMN_MW_CP_900               ,
    650, 10 ,  TXT_NET_650_10                  ,  PLMN_650_10                  ,
    651, 1  ,  TXT_NET_VCL_COMMS               ,  PLMN_VCL_COMMS               ,
    652, 1  ,  TXT_NET_BW_MASCOM               ,  PLMN_BW_MASCOM               ,
    652, 2  ,  TXT_NET_ORANGE          ,  PLMN_VISTA_CELLULAR          ,
    652, 4  ,  TXT_NET_BTC_MOBLIE          ,  PLMN_652_04          ,
    653, 10 ,  TXT_NET_653_10                  ,  PLMN_653_10                  ,
    655, 1  ,  TXT_NET_GR_VODA              ,  PLMN_VODACOM_SA              ,
    655, 7  ,  TXT_NET_655_07                  ,  PLMN_655_07                  ,
    655, 10 ,  TXT_NET_MTN_SA                  ,  PLMN_MTN_SA                  ,
    704, 1  ,  TXT_NET_CLARO_BRASIL            ,  PLMN_704_01            ,
    704, 2  ,  TXT_NET_744_04                 ,  PLMN_704_02                 ,
    704, 3  ,  TXT_NET_MOVISTAR                 ,  PLMN_704_03                 ,
    706, 1  ,  TXT_NET_ESV_PERSONAL            ,  PLMN_ESV_PERSONAL            ,
    706, 2  ,  TXT_NET_DIGICEL                 ,  PLMN_DIGICEL                 ,
    706, 3  ,  TXT_NET_744_04                 ,  PLMN_706_03                 ,
    706, 4  ,  TXT_NET_MOVISTAR                 ,  PLMN_706_04                 ,
    706, 11  ,  TXT_NET_CLARO_BRASIL                 ,  PLMN_706_11                 ,
    708, 1  ,  TXT_NET_CLARO_BRASIL                 ,  PLMN_708_01                 ,
    708, 2  ,  TXT_NET_CELTEL                 ,  PLMN_708_02                 ,
    708, 30  ,  TXT_NET_HONDUTEL                 ,  PLMN_708_30                 ,
    708, 40  ,  TXT_NET_DIGICEL                 ,  PLMN_708_40                 ,
    710, 21,  TXT_NET_CLARO_BRASIL                ,  PLMN_710_21,
    710, 30 ,  TXT_NET_MOVISTAR                ,  PLMN_710_30               ,
    710, 73 ,  TXT_NET_SERCOMTEL_BRASIL                ,  PLMN_710_73                ,
    714, 1,  TXT_NET_CABLE_WIRELESS             ,  PLMN_714_01,
    714, 2 ,  TXT_NET_MOVISTAR                ,  PLMN_714_02               ,
    714, 3 ,  TXT_NET_CLARO_BRASIL                ,  PLMN_714_03                ,
    714, 4 ,  TXT_NET_DIGICEL                ,  PLMN_714_04                ,
    716, 6 ,  TXT_NET_MOVISTAR                ,  PLMN_716_06               ,
    716, 7 ,  TXT_NET_NEXTEL                ,  PLMN_716_07               ,
    716, 10 ,  TXT_NET_TIM_PERU                ,  PLMN_TIM_PERU                ,
    716, 17 ,  TXT_NET_NEXTEL                ,  PLMN_716_17                ,
    722, 7  ,  TXT_NET_MOVISTAR                  ,  PLMN_UNIFON                  ,
    722, 10  ,  TXT_NET_MOVISTAR                  ,  PLMN_722_10                  ,
    722, 20  ,  TXT_NET_NEXTEL                  ,  PLMN_722_20                  ,
    722, 70  ,  TXT_NET_MOVISTAR                  ,  PLMN_722_70                  ,
    722, 310  ,  TXT_NET_CLARO_BRASIL                  ,  PLMN_722_310                  ,
    722, 320  ,  TXT_NET_CLARO_BRASIL                  ,  PLMN_722_320                  ,
    722, 330  ,  TXT_NET_CLARO_BRASIL                  ,  PLMN_722_330                  ,
    722, 34 ,  TXT_NET_744_05                ,  PLMN_PERSONAL                ,
    722, 341 ,  TXT_NET_744_05                ,  PLMN_722_341                ,
    722, 35 ,  TXT_NET_P_HABLE                 ,  PLMN_P_HABLE                 ,
    722, 350 ,  TXT_NET_P_HABLE                 ,  PLMN_722_350                 ,
    722, 36 ,  TXT_NET_744_05                 ,  PLMN_722_36                 ,
    730, 1  ,  TXT_NET_ENTEL_PCS            ,  PLMN_CHA_CELTEL_2            ,
    730, 2  ,  TXT_NET_MOVISTAR          ,  PLMN_CHA_LIBERTIS_2          ,
    730, 3  ,  TXT_NET_CLARO_BRASIL          ,  PLMN_730_03          ,
    730, 4  ,  TXT_NET_NEXTEL_BRASIL          ,  PLMN_730_04          ,
    730, 10 ,  TXT_NET_ENTEL_PCS               ,  PLMN_ENTEL_PCS               ,
    730, 99 ,  TXT_NET_WILL               ,  PLMN_730_99               ,
    734, 1  ,  TXT_NET_VZ_INFONET              ,  PLMN_VZ_INFONET              ,
    734, 2  ,  TXT_NET_DIGITEL                 ,  PLMN_DIGITEL                 ,
    736, 1  ,  TXT_NET_NUEVATEL                ,  PLMN_NUEVATEL                ,
    736, 2  ,  TXT_NET_EMOVIL                  ,  PLMN_EMOVIL                  ,
    740, 0  ,  TXT_NET_MOVISTAR           ,  PLMN_740_00           ,
    740, 1  ,  TXT_NET_CLARO_BRASIL           ,  PLMN_740_01           ,
    740, 2  ,  TXT_NET_740_02          ,  PLMN_740_02           ,
    744, 1  ,  TXT_NET_HOLA_PARAGUAY           ,  PLMN_HOLA_PARAGUAY           ,
    744, 2  ,  TXT_NET_PY_02                   ,  PLMN_PY_02                   ,
    744, 4  ,  TXT_NET_744_04                   ,  PLMN_744_04                   ,
    744, 5  ,  TXT_NET_744_05                   ,  PLMN_744_05                   ,
    746, 1  ,  TXT_NET_ICMS_SR                 ,  PLMN_ICMS_SR,
    724, 2  ,  TXT_NET_TIM_BRASIL                 ,  PLMN_724_02,
    724, 3  ,  TXT_NET_TIM_BRASIL                 ,  PLMN_724_03,
    724, 4  ,  TXT_NET_TIM_BRASIL                 ,  PLMN_724_04,
    724, 8  ,  TXT_NET_TIM_BRASIL                 ,  PLMN_724_08,
    724, 16  ,  TXT_NET_OI_BRASIL                 ,  PLMN_724_16,
    724, 24  ,  TXT_NET_OI_BRASIL                 ,  PLMN_724_24,
    724, 31  ,  TXT_NET_OI_BRASIL                 ,  PLMN_724_31,
    724, 05  ,  TXT_NET_CLARO_BRASIL                 ,  PLMN_724_05,
    724, 06  ,  TXT_NET_VIVO_BRASIL                 ,  PLMN_724_06,
    724, 10  ,  TXT_NET_VIVO_BRASIL                 ,  PLMN_724_10,
    724, 11  ,  TXT_NET_VIVO_BRASIL                 ,  PLMN_724_11,
    724, 07  ,  TXT_NET_CTBC_BRASIL                 ,  PLMN_724_07,
    724, 15  ,  TXT_NET_SERCOMTEL_BRASIL                 ,  PLMN_724_15,
    724, 23  ,  TXT_NET_VIVO_BRASIL                 ,  PLMN_724_23,
    724, 32  ,  TXT_NET_CTBC_BRASIL                 ,  PLMN_724_32,
    724, 33  ,  TXT_NET_CTBC_BRASIL                 ,  PLMN_724_33,
    724, 34  ,  TXT_NET_CTBC_BRASIL                 ,  PLMN_724_34,
    724, 37  ,  TXT_NET_AEIOU_BRASIL                 ,  PLMN_724_37,
    724, 00  ,  TXT_NET_NEXTEL_BRASIL                 ,  PLMN_724_00,
    410, 01  ,  TXT_NET_410_01                 ,  PLMN_410_01,
    410, 03  ,  TXT_NET_410_03                 ,  PLMN_410_03,
    410, 04  ,  TXT_NET_410_04                 ,  PLMN_410_04,
    410, 06  ,  TXT_NET_410_06                 ,  PLMN_410_06,
    410, 07  ,  TXT_NET_WARID                 ,  PLMN_410_07,
};

LOCAL const MMI_NETWORK_NAME_T s_network_mnc_digit_num_3_nametable[] =//mnc_digit_num=3
{
    310, 20 ,  TXT_NET_WWC_1                   ,  PLMN_WWC_1                   ,
    310, 26 ,  TXT_NET_T_MOBILE                   ,  PLMN_WWC_7                   ,
    405, 25 ,  TXT_NET_TATADOCO                  ,  PLMN_405_25              ,
    405, 26 ,  TXT_NET_TATADOCO                  ,  PLMN_405_26              ,
    405, 27 ,  TXT_NET_TATADOCO                  ,  PLMN_405_27              ,
    405, 28 ,  TXT_NET_TATADOCO                  ,  PLMN_405_28              ,
    405, 29 ,  TXT_NET_TATADOCO                  ,  PLMN_405_29              ,
    405, 30 ,  TXT_NET_TATADOCO                  ,  PLMN_405_30              ,
    405, 31 ,  TXT_NET_TATADOCO                  ,  PLMN_405_31              ,
    405, 32 ,  TXT_NET_TATADOCO                  ,  PLMN_405_32              ,
    405, 33 ,  TXT_NET_TATADOCO                  ,  PLMN_405_33              ,
    405, 34 ,  TXT_NET_TATADOCO                  ,  PLMN_405_34              ,
    405, 35 ,  TXT_NET_TATADOCO                  ,  PLMN_405_35              ,
    405, 36 ,  TXT_NET_TATADOCO                  ,  PLMN_405_36              ,
    405, 37 ,  TXT_NET_TATADOCO                  ,  PLMN_405_37              ,
    405, 38 ,  TXT_NET_TATADOCO                  ,  PLMN_405_38              ,
    405, 39 ,  TXT_NET_TATADOCO                  ,  PLMN_405_39              ,
    405, 40 ,  TXT_NET_TATADOCO                  ,  PLMN_405_40              ,
    405, 41 ,  TXT_NET_TATADOCO                  ,  PLMN_405_41              ,
    405, 42 ,  TXT_NET_TATADOCO                  ,  PLMN_405_42              ,
    405, 43 ,  TXT_NET_TATADOCO                  ,  PLMN_405_43              ,
    405, 44 ,  TXT_NET_TATADOCO                  ,  PLMN_405_44              ,
    405, 45 ,  TXT_NET_TATADOCO                  ,  PLMN_405_45              ,
    405, 46 ,  TXT_NET_TATADOCO                  ,  PLMN_405_46              ,
    405, 47 ,  TXT_NET_TATADOCO                  ,  PLMN_405_47
};

typedef enum
{
    MMIPHONE_PS_POWER_ON_NONE,
    MMIPHONE_PS_POWER_ON_EMERGE,
    MMIPHONE_PS_POWER_ON_NORMAL
}MMIPHONE_PS_POWER_ON_TYPE;

#ifdef CSC_XML_SUPPORT
typedef struct
{
    uint16 net_work_num;
    MMICUS_CSC_GENERALINFO_NETWORKINFO_T *csc_network_info;
}MMI_CSC_NETWORK_NAME_T;
#endif

/**--------------------------------------------------------------------------*
**                         STATIC DEFINITION                                *
**--------------------------------------------------------------------------*/
LOCAL PHONE_SIM_STATUS_T          s_sim_status[MMI_DUAL_SYS_MAX];        //the status of sim

LOCAL BOOLEAN               s_sim_existed_status[MMI_DUAL_SYS_MAX] = {0}; //变量用来记录SIM卡是否存在
LOCAL BOOLEAN s_is_power_flag = FALSE;

#ifdef MMI_DUALMODE_ENABLE
LOCAL MMI_GMMREG_RAT_E  s_is_TD = MMI_GMMREG_RAT_UNKNOWN;//MN_GMMREG_RAT_GPRS;
#endif
LOCAL MMIPHONE_HANDLE_PIN_STATUS_T s_handle_pin_status;         //add for dual sim
LOCAL BOOLEAN s_is_same_sim[MMI_DUAL_SYS_MAX] = {0};

#ifndef MMI_MULTI_SIM_SYS_SINGLE
LOCAL SIM_QUERY_STATUS_E s_query_sim_status = SIM_QUERY_STATUS_NONE;
#endif

LOCAL BOOLEAN s_is_power_on_sim_cnf[MMI_DUAL_SYS_MAX] = {FALSE};
LOCAL SIM_QUERY_STATUS_E s_query_sim_present_status = SIM_QUERY_STATUS_NONE;
LOCAL MMIPHONE_PS_POWER_ON_TYPE s_ps_ready[MMI_DUAL_SYS_MAX] = {0};/*lint !e64*/
LOCAL BOOLEAN s_is_ps_deactive_cnf[MMI_DUAL_SYS_MAX] = {FALSE};
LOCAL BOOLEAN s_is_ps_ready[MMI_DUAL_SYS_MAX] = {FALSE};
LOCAL BOOLEAN s_is_sim_present_ind[MMI_DUAL_SYS_MAX] = {FALSE};
LOCAL BOOLEAN s_is_network_status_ready[MMI_DUAL_SYS_MAX] = {FALSE};
#if defined(MMI_SMSCB_SUPPORT)
LOCAL BOOLEAN s_is_startup_smscb[MMI_DUAL_SYS_MAX] = {FALSE};
#endif
LOCAL MMIPHONE_NOTIFY_LIST_T *s_mmiphone_subscriber_list_ptr = PNULL;
LOCAL MMIPHONE_SIM_PLUG_NOTIFY_LIST_T *s_mmiphone_sim_plug_subscriber_list_ptr = PNULL;
MN_DUAL_SYS_E s_plug_sim_id = MN_DUAL_SYS_1;
MMIPHONE_SIM_PLUG_EVENT_E s_plug_event = MMIPHONE_NOTIFY_SIM_PLUG_MAX;
LOCAL MMI_OPERATE_PIN_T s_operate_pin_blocked_info;//用于pin被锁后传递参数
LOCAL MN_DUAL_SYS_E s_cur_handling_sim_id = MMI_DUAL_SYS_MAX;
LOCAL MMI_HANDLE_T s_need_update_network_status_win = 0;//网络状态更新时候需要刷新网络信息的窗口
#ifdef MMI_SIM_LOCK_SUPPORT
LOCAL MN_SIM_LOCK_STATUS_T s_sim_unlock_config_data = {0};//记下 SIM lock 解锁用计数器和定时器的配置信息
LOCAL MMI_OPERATE_SIM_LOCK_T s_sim_lock_operate_value = {0}; 
LOCAL MMIPHONE_HANDLE_SIMLOCK_STATUS_T  s_handle_sim_lock_status = {0};//为多卡记住SIM Lock status
#endif
#ifdef MMI_SIM_LANGUAGE_SUPPORT
// 是否做过优选语言的动作。
LOCAL BOOLEAN s_is_language_auto_set = FALSE; 

//做优选语言的sim卡，系统运行中只选定一张固定的sim卡做操作。
LOCAL MN_DUAL_SYS_E s_prefer_language_sim = MN_DUAL_SYS_1; 
#endif

#ifdef CSC_XML_SUPPORT
LOCAL MMI_CSC_NETWORK_NAME_T *s_csc_network_name_ptr = PNULL;
#endif

/**--------------------------------------------------------------------------*
**                         EXTERNAL DECLARE                                 *
**--------------------------------------------------------------------------*/
//extern MMI_SYSTEM_T g_mmi_system; 
extern uint32   g_operate_pin_win_tab;
extern BOOLEAN g_is_operate_pin_special_func;

/**--------------------------------------------------------------------------*
**                         GLOBAL DEFINITION                                *
**--------------------------------------------------------------------------*/
MMI_APPLICATION_T   g_mmiphone_app;         //phone的实体
PHONE_SERVICE_STATUS_T  g_service_status[MMI_DUAL_SYS_MAX];         //the status of service
MN_BLOCK_T g_pin_value[MMI_DUAL_SYS_MAX] ={0};
#ifdef DPHONE_SUPPORT 
BOOLEAN g_pin_lock_hookhandfree = FALSE;
#endif
BOOLEAN g_is_sim_only_check_status[MMI_DUAL_SYS_MAX] = {0};

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
// 	Description : create input pin window
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
LOCAL MMI_RESULT_E CreateInputPinWindow(MnSimGetPinIndS sim_get_pin_ind);

/*****************************************************************************/
// 	Description : IsNeedSetGPRSAttachMode
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN IsNeedSetGPRSAttachMode(void);

/*****************************************************************************/
// 	Description : 处理PHONE APP的PS消息
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePsAndRefMsg(
                                     PWND                app_ptr, 
                                     uint16              msg_id, 
                                     DPARAM              param
                                     );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_NETWORK_STATUS_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleNetworkStatusInd(
                                          DPARAM              param
                                          );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PLMN_LIST_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePreferRatRsp( 
                                        DPARAM  param
                                        );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PLMN_SELECT_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectPlmnCnf(
                                       DPARAM              param
                                       );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_POWER_ON_CNF
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePowerOnSimCnf(
                                       DPARAM              param
                                       );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_POWER_OFF_CNF
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePowerOffSimCnf(
                                       DPARAM              param
                                       );

#ifdef MMI_PREFER_PLMN_SUPPORT
/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PREFER_PLMN_CHANGE_CNF
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePreferPlmnChangeCnf(
                                       DPARAM              param
                                       );
#endif

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PS_POWER_OFF_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePowerPsOffCnf(
                                        DPARAM              param
                                        );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_DEACTIVE_PS_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDeactivePsCnf(
                                        DPARAM              param
                                        );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PLMN_LIST_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePlmnListCnf(
                                     DPARAM              param
                                     );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SET_BAND_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectBandCnf(
                                       DPARAM              param
                                       );

/*****************************************************************************/
// 	Description : get the network name
//	Global resource dependence : 
//  Author: Great.Tian
//	Note:
/*****************************************************************************/
LOCAL MMI_TEXT_ID_T GetNetWorkNameId(
                                     MN_PLMN_T *plmn_ptr,
                                     MN_PHONE_PLMN_STATUS_E  plmn_status,
                                     BOOLEAN                 is_slecting
                                     );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_GET_PIN_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPinInd(
                                   DPARAM              param
                                   );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_PIN_FUNC_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPinCnf(
                                   DPARAM              param
                                   );

/*****************************************************************************/
// 	Description : 保存网络状态
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StoreNetworkStatus(
                               MnNetworkStatusIndS *status_ptr
                               );

/*****************************************************************************/
// 	Description : 在提示还可以 输入多少次密码后，启动pin输入框
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePin1Maxtimes(
                                      MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                      MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                      DPARAM 		        param		// 相应消息的参数
                                      );

/*****************************************************************************/
// 	Description : 在pin blocked的情况下关闭窗口，则启动输入puk的窗口
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePinBlocked(
                                    MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                    MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                    DPARAM 		        param		// 相应消息的参数
                                    );

/*****************************************************************************/
// 	Description : 在pin1或者pin2 blocked的情况下关闭窗口，则启动输入puk的窗口
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePinBlockedExceptPhone(
                                               MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                               MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                               DPARAM 		        param		// 相应消息的参数
                                               );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_READY_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimReadyInd(
                                     DPARAM              param
                                     );

/*****************************************************************************/
// 	Description : 保存 SIM 状态
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StoreSimReadyStatus(
                               MnSimReadyIndS  *sim_status_ptr
                               );

/*****************************************************************************/
// 	Description : 保存 SIM  not ready 状态
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StoreSimNotReadyStatus(
                                  MnSimNotReadyIndS  sim_status_ptr
                                  );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_NOT_READY_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimNotReadyInd(
                                        DPARAM              param
                                        );

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SCELL_RSSI_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleScellRssiInd(
                                      DPARAM              param
                                      );

#if defined(MMI_SMSCB_SUPPORT)
/*****************************************************************************/
// 	Description : startup sms cb
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StartupSmsCB(MN_DUAL_SYS_E dual_sys);

/*****************************************************************************/
// 	Description : ClearCbEnable
//	Global resource dependence : 
//  Author: fengming.huang
//	Note:
/*****************************************************************************/
LOCAL void ClearCbEnable(MN_DUAL_SYS_E dual_sys);
#endif

/*****************************************************************************/
// 	Description : get opn display string
//	Global resource dependence : 
//  Author: wancan.you
//	Note:  
/*****************************************************************************/
LOCAL void GetOpnString(MN_DUAL_SYS_E dual_sys,//IN:
                                            MMI_STRING_T *opn_str_ptr,//IN/OUT
                                            uint16 max_opn_len
                                            );

/*****************************************************************************/
// 	Description : get spn display string
//	Global resource dependence : 
//  Author: wancan.you
//	Note:  
/*****************************************************************************/
LOCAL void GetSpnString(MN_DUAL_SYS_E dual_sys,//IN:
                                            MMI_STRING_T *spn_str_ptr,//IN/OUT
                                            uint16 max_spn_len
                                            );

/*****************************************************************************/
// 	Description : compare spn and opn
//	Global resource dependence : 
//  Author: kun.yu
//	Note:  
/*****************************************************************************/
LOCAL BOOLEAN MMIPHONE_IsSPNEqualtoOPN(MMI_STRING_T *opn_str_ptr,//IN:
                                                                                    MMI_STRING_T *spn_str_ptr//IN:
                                                                                    );

/*****************************************************************************/
//     Description : 
//    Global resource dependence : 
//  Author: wancan.you
//    Note: 
/*****************************************************************************/
//LOCAL void MMIPHONE_SetDualSysSetting(BOOLEAN is_need_start_ps);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPresentInd(
                                     DPARAM param
                                     );

/*****************************************************************************/
// 	Description : PowerOnPsForEmerge
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
LOCAL void PowerOnPsForEmerge(MN_DUAL_SYS_E dual_sys);

/*****************************************************************************/
//  Description : the process message function of the virtual window
//  Global resource dependence : 
//  Author:
//  Note:
/*****************************************************************************/
MMI_RESULT_E  IdleApplet_HandleEvent(    
                                     IAPPLET_T*          iapplet_ptr,
                                     MMI_MESSAGE_ID_E    msg_id, 
                                     DPARAM              param
                                     );

/*****************************************************************************/
//  Description : Notify Rx level Change
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL void NotifyRxLevelChange(MN_DUAL_SYS_E dual_sys, uint8 rx_level);

/*****************************************************************************/
//  Description : Notify PLMN Status Change
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL void NotifyPlmnStatusChange(MN_DUAL_SYS_E dual_sys, MN_PHONE_PLMN_STATUS_E plmn_status);

/*****************************************************************************/
//  Description : Notify Service Change
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN NotifyServiceChange(MN_DUAL_SYS_E dual_sys, MMIPHONE_NOTIFY_EVENT_E notify_event);



//applet的数组
LOCAL const CAF_STATIC_APPLET_T s_mmiphone_applet_array[] = 
{
    { SPRD_IDLE_APPLET_ID, sizeof(CAF_APPLET_T), IdleApplet_HandleEvent, IMAGE_MAINMENU_ICON_SET, TXT_IDLE },
};

//applet的信息
LOCAL const CAF_STATIC_APPLET_INFO_T s_mmiphone_applet_info = 
{
    (CAF_STATIC_APPLET_T *)s_mmiphone_applet_array,
    ARR_SIZE(s_mmiphone_applet_array)
};

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPlugInInd(
                                     DPARAM param
                                     );

#ifdef MMI_LDN_SUPPORT
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleLdnReadyInd(
                                     DPARAM param
                                     );

/*****************************************************************************/
// 	Description : ldn update confirm
//	Global resource dependence : none
//	Author:dave.ruan
//	Note:delete all/datlete both type is erase
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleLDNUpdateCnf( DPARAM param);
#endif

/*****************************************************************************/
// 	Description : Handle SIM Plug Alert Win Msg
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSIMPlugAlertWinMsg(
                                                                                              MMI_WIN_ID_T win_id,
                                                                                              MMI_MESSAGE_ID_E 	msg_id,
                                                                                              DPARAM param
                                                                                              );

/*****************************************************************************/
//  Description : Notify SIM Plug Ind
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN NotifySIMPlugInd(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E notify_event);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL void OpenSetStandByModeWaiting(void);

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL void ImplementStandByModeAsync(void);

/*****************************************************************************/
//  Description : HandleSetStandbyMdoeWaitingWindow
//  Global resource dependence : 
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSetStandbyMdoeWaitingWindow(
                                         MMI_WIN_ID_T    win_id, // 窗口的ID
                                         MMI_MESSAGE_ID_E   msg_id, // 窗口的内部消息ID
                                         DPARAM             param   // 相应消息的参数
                                         );
/*****************************************************************************/
// 	Description : HandleSetStandByModeQueryWinMsg
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSetStandByModeQueryWinMsg( 
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E    msg_id, 
                                           DPARAM              param
                                           );


#ifdef MMI_SIM_LOCK_SUPPORT
/*****************************************************************************/
// 	Description : 在SIM Lock 解锁次数达到限制的情况下弹出"SIM Restricted"窗口
//	Global resource dependence : 
//  Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSIMLocked(
                                    MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                    MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                    DPARAM 		        param		// 相应消息的参数
                                    );
#endif
/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/
/*****************************************************************************/
// 	Description : register common module applet info
//	Global resource dependence : none
//  Author: James.Zhang
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_CommonMduRegAppletInfo(void)
{
    MMI_RegAppletInfo( MMI_MODULE_COMMON, &s_mmiphone_applet_info );
}

/*****************************************************************************/
//  Description : the process message function of the virtual window
//  Global resource dependence : 
//  Author:
//  Note:
/*****************************************************************************/
MMI_RESULT_E  IdleApplet_HandleEvent(    
                                     IAPPLET_T*          iapplet_ptr,
                                     MMI_MESSAGE_ID_E    msg_id, 
                                     DPARAM              param
                                     )
{

    MMI_RESULT_E    result = MMI_RESULT_FALSE;
	//MMIFILE_DEVICE_E device_type = MMI_DEVICE_SDCARD;    
	
#ifdef MRAPP_SUPPORT
	result = MMIMRAPP_handleMrMsg(msg_id, param);
#endif 
    //SCI_TRACE_LOW:"VirWin_HandleMsg THE msg_id = 0x%04x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2039_112_2_18_2_45_3_0,(uint8*)"d",msg_id);
    switch (msg_id)
    {
    case MSG_KEYDOWN_SDCARD_DETECT:
    case MSG_KEYUP_SDCARD_DETECT:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive SD 0x%x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2044_112_2_18_2_45_3_1,(uint8*)"d",msg_id);
		MMIAPISD_HandleSDPlug((void *)param, msg_id);
        //MMIAPISD_HandleSDPlug(msg_id);
        break;

#ifdef DPHONE_SUPPORT 
#else       
    case MSG_HANDLE_SD_ERR:	
		MMIAPISD_HandleSDPlug((void *)param, msg_id);	
        break;   
#endif   

#ifdef JAVA_SUPPORT_MYRIAD
    case MSG_JAVA_MMI_EVENT:
        MMIAPIJAVA_HandleJbedMMIEvent(param);
        break;
    case MSG_JAVA_OTA_INSTALL_START:
        {
            SCI_TRACE_LOW("IdleApplet_HandleEvent:receive MSG_JAVA_OTA_INSTALL_START!");
            if(MMIBROWSER_SUCC == MMIAPIBROWSER_Close())
            {
                if(PNULL != param)
                {
                    uint16* path_name = (uint16*)param;

                    MMIAPIJAVA_Install(path_name,MMIAPICOM_Wstrlen(path_name));
                }
            }
        }
        break;
#endif

    case MSG_APP_RED:
        result = MMI_RESULT_TRUE;
        break;
        
    case MSG_FILE_READFILE_ASYN_CALLBACK:
    case MSG_FILE_WRITEFILE_ASYN_CALLBACK:
    case MSG_FILE_READFILE_BYOFFSET_ASYN_CALLBACK:
    case MSG_FILE_WRITEFILE_BYOFFSET_ASYN_CALLBACK:
        MMIAPIFMM_OperationAsynCnf((void*)param, msg_id);
        break;
        
//    case MSG_FMM_COPY_FILE_READ_CNF:
//        MMIFMM_FileCopyWriteDest(*(SFS_ERROR_E *)param);
//        break;
//        
//    case MSG_FMM_COPY_FILE_WRITE_CNF:
//        MMIFMM_FileCopyReadSrc(*(SFS_ERROR_E *)param);
//         break;
        
    case MSG_FILE_COPY_READ_CNF:
        MMIAPIFMM_CopyWriteDest(*(SFS_ERROR_E *)param);
        break;
        
    case MSG_FILE_COPY_WRITE_CNF:
        MMIAPIFMM_CopyReadSrc(*(SFS_ERROR_E *)param);
        break;
        
    case MSG_ALM_ALERTING_END:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received MSG_ALM_ALERTING_END"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2090_112_2_18_2_45_3_2,(uint8*)"");
        if (!MMK_IsFocusWin(PHONE_POWER_OFF_WIN_ID))
        {
            if(MMK_IsOpenWin(PHONE_STARTUP_ALARM_WIN_ID))
            {
                MMK_CloseWin(PHONE_STARTUP_ALARM_WIN_ID);
            }
            MMK_CreateWin((uint32*)PHONE_STARTUP_ALARM_WIN_TAB,PNULL);
        }
        result = MMI_RESULT_TRUE;
        break;

    case MSG_MP3_PLAY_CALL_BACK:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received MSG_MP3_PLAY_CALL_BACK"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2103_112_2_18_2_45_3_3,(uint8*)"");
        //Mp3正常播放结束的回调函数。
        MMIAPIMP3_PlayMp3CallBack();
        break;

    case MSG_MP3_FATAL_ERROR:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received MSG_MP3_FATAL_ERROR"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2109_112_2_18_2_45_3_4,(uint8*)"");
        MMIAPIMP3_HandleFatalError();
        break;
#ifdef  MMIAP_LYRIC_SUPPORT    
    case MSG_MP3_START_LIRIC:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received [MMIMP3] MSG_MP3_START_LIRIC"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2114_112_2_18_2_45_3_5,(uint8*)"");
        MMIAPIMP3_StartLyric();
        break;
#endif
    case MSG_MP3_FOLDER_FULL:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received [MMIMP3] MSG_MP3_FOLDER_FULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2119_112_2_18_2_45_3_6,(uint8*)"");
        MMIAPIMP3_FolderFull();
        break;

    case MSG_KUR_PLAY_CALL_BACK:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received MSG_KUR_PLAY_CALL_BACK"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2124_112_2_18_2_45_3_7,(uint8*)"");
        MMIAPIKUR_PlayKurCallBack();
        break;

    case MSG_KUR_FATAL_ERROR:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received MSG_KUR_FATAL_ERROR"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2129_112_2_18_2_45_3_8,(uint8*)"");
        MMIAPIKUR_HandleFatalError();
        break;

    case MSG_MPEG4_DELETE_ONE:
        //SCI_TRACE_LOW:"VirWin_HandleMsg: received MSG_MPEG4_DELETE_ONE"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2134_112_2_18_2_45_3_9,(uint8*)"");
        break;
        
   case MSG_MMS_ACTIVE_RECV_SEND:
       //SCI_TRACE_LOW:"VirWin_HandleMsg: MMIAPIMMS_ActiveRecvSendModule"
       SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2138_112_2_18_2_45_3_10,(uint8*)"");
       //启动彩信发送和接收，通过异步实现
       MMIAPIMMS_ActiveRecvSendModule();
       break;
        
    case MSG_MMIDV_DEL_UNSAVED_MPEG4:
        MMITHEME_SetUpdateDelayCount(2);
        break;
        
    case MSG_MMIDV_CLOSE_RECORD_WIN:
        MMIAPIDC_Exit();
        break;
        
//    case MSG_CMSBRW_FS_READ:	//wanqiangzh modify for STK
//    case MSG_CMSBRW_FS_WRITE:	//wanqiangzh modify for STK
//        MMICMSBRW_HandleWriteAndRead(msg_id, param);
//        break;
//    case MSG_CMSBRW_STOP_BGSOUND:
//        MMIAPISET_StopRing(MMISET_RING_TYPE_OTHER);
//        break;

    case MSG_RECORD_END:
        MMIAPIDC_Exit();
        break;
//    case MSG_CMSBRW_BRW_OP:
//        SCI_TRACE_LOW("VirWin_HandleMsg:MSG_CMSBRW_BRW_OP");
//        {			
//            if(param!=PNULL)
//            {
//                MMIBRW_VIRWIN_MESSAGE_T      *message=NULL;
//                message = (MMIBRW_VIRWIN_MESSAGE_T *)param;
//                MMIBRW_HandleBrwOpMsg(READ_FILE_RESP,&message->param);	
//                //SCI_Free(message->param);
//                //SCI_Free(message);
//            }
//            else
//            {
//                SCI_TRACE_LOW("warning:param null");
//            }
//        }
//        
//        break;
#ifdef  CAMERA_SUPPORT        
    case MSG_DV_OPEN_MSG:
        MMIAPIDC_OpenVideoWin();
        break;
        
    case MSG_DC_OPEN_MSG:
        MMIAPIDC_OpenPhotoWin();
        break;
#endif
        //msg for MobileTV
    case MSG_MBBMS_PRE_EXIT_MSG:
#ifdef VT_SUPPORT
        if(!MMIAPIVT_IsVtCalling())
        {
            MMIAPIMTV_PreExitCallback();
        }
#else
        MMIAPIMTV_PreExitCallback();
#endif
        break;
#ifdef MMI_VCALENDAR_SUPPORT
    case MSG_VCALENDAR_WRITE_SMS_CNF:
    case MSG_VCALENDAR_SAVE_SCH_CNF:
        MMIVCAL_HandleWriteCNF(msg_id, param);
        break;
#endif
#ifdef 	JAVA_SUPPORT_IA		
#if defined JAVA_OTA_SUPPORT && defined BROWSER_SUPPORT
    case MSG_JAVA_OTA_DOWNLOAD_START:
        MMIAPIJAVA_OpenWapWin();        
        break;
#endif
#endif

    case MSG_SET_OPENALARM_EXPIRED_WIN:
        MMIAPIALM_OpenAlarmExpiredWin();
        break;   
        
    case MSG_SLEEP_LCD:
        MMIDEFAULT_TurnOffBackLight();
        break;

    case MSG_AUDIO_CALLACK:
        MMISRVAUD_HandleCallBackMsg((void *)param);
        break;

    case MSG_SET_WALLPAPER_WAIT:
        MMIAPIFMM_HandleWallpaperWaitMsg();
        break;
        
#ifdef BROWSER_SUPPORT_DORADO
	case BRW_SNAPSHOT_FFS_PAGE_SAVE_CNF:			
	case BRW_SNAPSHOT_FFS_CSS_SAVE_CNF:		
	case BRW_SNAPSHOT_FFS_IMAGE_SAVE_ONE_CNF:	
    case BRW_SNAPSHOT_FFS_IMAGE_SAVE_CNF:
	case BRW_SNAPSHOT_FFS_PAGE_OPEN_CNF:
	case BRW_SNAPSHOT_FFS_IMAGE_OPEN_ONE_CNF:
		BRW_SnapshotHandleFfsMsg(msg_id,*((BOOLEAN*)param));
	break;
    
    case MSG_BRW_MSG_INIT_CNF:
    case MSG_BRW_MSG_EXIT_CNF:
    case MSG_BRW_MSG_TYPE_ENTER_FILENM_REQ:
    case MSG_BRW_MSG_TYPE_DL_CONFIRM_REQ:     
    case MSG_BRW_MSG_TYPE_AUTH_REQ:           
    case MSG_BRW_MSG_TYPE_CHANGE_URL_IND:     
    case MSG_BRW_MSG_TYPE_NEW_PAGE_REQ_IND:   
    case MSG_BRW_MSG_TYPE_UPDATE_CONTENT_IND: 
    case MSG_BRW_MSG_TYPE_PROGRESS_IND:       
    case MSG_BRW_MSG_TYPE_ERROR_IND:            
    case MSG_BRW_MSG_TYPE_DOWNLOAD_OK_IND:     
    case MSG_BRW_MSG_TYPE_DOWNLOAD_ERR_IND:   
    case MSG_BRW_MSG_TYPE_PAGE_GET_IND:
    case MSG_BRW_MSG_TYPE_MEDIA_GET_ONE_IND:
    case MSG_BRW_MSG_TYPE_DISPLAY_READY_IND:  
    case MSG_BRW_MSG_TYPE_SSL_CERT_IND:       
    case MSG_BRW_MSG_TYPE_REPAINT_READY_IND:
    case MSG_BRW_MSG_TYPE_PAGE_STOP_IND:
    case MSG_BRW_MSG_TYPE_OMA_DL_CONFIRM_REQ:        
    case MSG_BRW_MSG_TYPE_LAYOUTCTRL_DESTROY_REQ:
//    case MSG_BRW_MSG_TYPE_PDP_ACTIVE_CNF:
    case MSG_BRW_STOP_BGSOUND:
        MMIAPIBROWSER_HandleAppMsg(msg_id,param);
        break;
        
#endif

#ifdef BROWSER_SUPPORT
    case MSG_BROWSER_MANAGER_MAKECALL_REQ:
    case MSG_BROWSER_MANAGER_SENDDTMF_REQ:
    case MSG_BROWSER_MANAGER_ADD_PBITEM_REQ:
    case MSG_BROWSER_MANAGER_OPEN_RTSP_IND:
    case MSG_BROWSER_MANAGER_SENDSMS_REQ:
    case MSG_BROWSER_MANAGER_SENDSMSTO_REQ:
    case MSG_BROWSER_MANAGER_SENDMMS_REQ:
    case MSG_BROWSER_MANAGER_SENDMMSTO_REQ:
    case MSG_BROWSER_MANAGER_SENDMAIL_REQ:
    case MSG_BROWSER_MANAGER_SENDMAILTO_REQ:
    case MSG_BROWSER_MANAGER_TEL_REQ:
        MMIAPIBROWSER_HandleAppMsg(msg_id,param);
        break;
#endif

    case MSG_APPLET_CLEAR_FREE_MODULE:
        MMK_PostMsg( MMIAPPLET_MANAGER_LIST_WIN_ID, MSG_APPLET_MANAGER_UPDATE, PNULL, 0 );
        break;

    case MSG_APPLET_SWITCH:
        MMK_ReturnIdleWin();
        break;
        
#if defined(OPERA_MOBILE_SUPPORT_V11) || defined(OPERA_MINI_SUPPORT_VER42) || defined(OPERA_MINI_SUPPORT_VER6)
    case MSG_MMI_OPERA_ASYNC_EXIT:
         {
            MMIBROWSER_ENTRY_PARAM *ptr = (MMIBROWSER_ENTRY_PARAM *)param;
            
#ifdef BROWSER_SUPPORT_DORADO
#ifndef WIN32
            //opera exit
            OperaMini_Exit();
#endif
            //dorado entry
            if (ptr != 0)
            {
                MMIBROWSER_Entry(ptr);
                if (ptr->url_ptr != 0)
                {
                    SCI_FREE(ptr->url_ptr);
                }
            }
        }
#else
            //only free
            if (ptr != 0)
            {
                if (ptr->url_ptr != 0)
                {
                    SCI_FREE(ptr->url_ptr);
                }
            }
        }
#endif
        break;
#endif        
        
#ifdef TTS_SUPPORT
#ifndef WIN32
    case MSG_TTS_PLAY:
        //SCI_TRACE_LOW:"virwin_MSG_TTS_AUDIO_PLAY"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2302_112_2_18_2_45_4_11,(uint8*)"");
        MMIAPITTS_SynthTextByTask();
        break;
    case MSG_TTS_STOP:
        //SCI_TRACE_LOW:"virwin_MSG_TTS_AUDIO_STOP"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2306_112_2_18_2_45_4_12,(uint8*)"");
        MMIAPITTS_StopSynthTextByTask();
        break;
    case MSG_TTS_OVER:
#ifdef TTS_SUPPORT_EJ	
        //SCI_TRACE_LOW:"virwin_MSG_TTS_OVER"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2311_112_2_18_2_45_4_13,(uint8*)"");
        MMK_PostMsg( MMIKL_CLOCK_DISP_WIN_ID == MMK_GetFocusWinId() ? MMK_GetPrevWinId(MMIKL_CLOCK_DISP_WIN_ID) : MMK_GetFocusWinId(), MSG_TTS_OVER, PNULL, 0 );
#endif
        break;
    case MSG_TTS_AUDIO_PLAY:
        {
            uint32 pcm_stream_id = *(uint32*)param;
            //SCI_TRACE_LOW:"virwin_MSG_TTS_AUDIO_PLAY"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2318_112_2_18_2_45_4_14,(uint8*)"");
            MMIAPITTS_PlayPcmStream(pcm_stream_id);
        }
        break;

    case MSG_TTS_ERROR_RESOURCE:
#ifdef EBOOK_SUPPORT
        if(MMK_IsOpenWin(MMIEBOOK_TXT_SHOW_WIN_ID))
        {
            MMIAPIEBOOK_SetTTSOnOFF(FALSE);
        }
#endif
#if 0        
        MMIPUB_OpenAlertWinByTextId(PNULL, TXT_TTS_NO_RESOURCE, 
            TXT_NULL, IMAGE_PUBWIN_WARNING, PNULL, PNULL, MMIPUB_SOFTKEY_ONE, PNULL);
#endif
        break;
#endif
#endif
	default:
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : init the phone application
//	Global resource dependence : g_service_status,g_phone_app
//  Author: Jassmine
//	Note:
/*****************************************************************************/
void MMIAPIPHONE_AppInit(void)
{
    MN_DUAL_SYS_E i = MN_DUAL_SYS_1;
    
    // 初始化记录服务状态的结构，主要用于IDLE界面显示
    SCI_MEMSET(&s_sim_status[0], 0, sizeof(s_sim_status));
    SCI_MEMSET(&g_service_status[0], 0, sizeof(g_service_status));

    //Udisk没有在运行
    MMIAPIUdisk_SetRunStatus(FALSE);
    
    g_mmiphone_app.ProcessMsg = HandlePsAndRefMsg;
    
    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        s_sim_status[i].simStatus = SIM_NOT_INITED;

        g_service_status[i].is_first_rssi    = TRUE;
        g_service_status[i].is_slecting = TRUE; 
        
        s_is_ps_ready[i]= FALSE;
        s_ps_ready[i] = MMIPHONE_PS_POWER_ON_NONE;

        s_is_ps_deactive_cnf[i] = TRUE;

        s_sim_existed_status[i] = TRUE;
    } 
#ifdef DPHONE_SUPPORT
	MMIDEFAULT_InitHookAndHF();
#endif

#if defined(MMI_UPDATE_TIME_SUPPORT)
    MMIAPIAUT_AutoUpdateTimeInit();
#endif/*MMI_UPDATE_TIME_SUPPORT*/

#ifdef DRM_SUPPORT
#ifdef BROWSER_SUPPORT    
    {
	MMIBROWSER_MIME_DOWNLOAD_PARAM_T drm_callback = {0};

	drm_callback.call_back = MMIDRM_HandleDLDrmCB;
	
	drm_callback.mime_type = MMIBROWSER_MIME_TYPE_DM;
	MMIAPIBROWSER_RegMimeFunc(&drm_callback);
	
	drm_callback.mime_type = MMIBROWSER_MIME_TYPE_DCF;
	MMIAPIBROWSER_RegMimeFunc(&drm_callback);
    }
#endif
#endif
    MMIPHONE_InitNITZ();
    MMIPHONE_InitNetworkNameInfo();
}

/*****************************************************************************/
// 	Description : 处理PHONE APP的PS消息
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePsAndRefMsg(
                                     PWND                app_ptr, 
                                     uint16              msg_id, 
                                     DPARAM              param
                                     )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MN_DUAL_SYS_E   e_dual_sys = MN_DUAL_SYS_1;
    // MnSimGetPinIndS *sim_get_pin_ind_ptr = PNULL;

#ifdef MRAPP_SUPPORT
	result = MMIMRAPP_handleMrMsg(msg_id, param);
#endif 

    switch (msg_id)
    {
    case APP_MN_NETWORK_STATUS_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_NETWORK_STATUS_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2430_112_2_18_2_45_4_15,(uint8*)"");
        // 该消息用于在网络服务状态改变时，汇报当前网络服务状态，是紧急呼叫状态，
        // 还是无服务状态，还是GSM服务状态，还是GSM、GPRS状态。
        // 获得网络的状态需在idle界面显示。
        result = HandleNetworkStatusInd(param);
        break;

    case APP_MN_NETWORK_INFO_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_NETWORK_INFO_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2438_112_2_18_2_45_4_16,(uint8*)"");
        // 该消息用于在网络信息改变时，上报当前网络PLMN
        // 网络名称,包括full name/short name,其中每个结构都有状态、字符类型、长度等
        // 还有当前的网络时间LSA等信息
        result = MMIPHONE_HandleNetworkInfoInd(param);
        break;
        
    case APP_MN_PREFER_RAT_RSP:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PREFER_RAT_RSP"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2446_112_2_18_2_45_4_17,(uint8*)"");
        result  = HandlePreferRatRsp(param);
        break;
        
    case APP_MN_PLMN_LIST_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PLMN_LIST_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2451_112_2_18_2_45_4_18,(uint8*)"");
        //该消息用于通知用户MNPHONE_ListPLMN()的操作结果。
        // 结构中包含存在网络的列表和每个网络的具体信息。
        result = HandlePlmnListCnf(param);
        break;
        
    case APP_MN_PLMN_SELECT_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PLMN_SELECT_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2458_112_2_18_2_45_4_19,(uint8*)"");
        // 该消息用于通知用户MNPHONE_SelectPLMN()的操作结果。
        // 结构中包含选择操作完成后当前的网络状况。
        result = HandleSelectPlmnCnf(param);
        break;
        
    case APP_MN_DISPLAY_PLMN_IND:
        e_dual_sys = ((MnDisplayPlmnIndS *)param)->dual_sys;
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_DISPLAY_PLMN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2466_112_2_18_2_45_4_20,(uint8*)"");
        //该消息用于通知用户进行手动选网	
        //if (!MMK_IsFocusWin(PHONE_POWER_OFF_WIN_ID))
        //{
        //    result = MMIAPISET_HandleDisplayPlmnInd(e_dual_sys);
        //}
        break;
        
    case APP_MN_SIM_PRESENT_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_PRESENT_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2475_112_2_18_2_45_4_21,(uint8*)"");
        result = HandleSimPresentInd(param);
        break;
        
    case APP_MN_SIM_PLUG_IN_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_PLUG_IN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2480_112_2_18_2_45_4_22,(uint8*)"");
        result = HandleSimPlugInInd(param);
        break;

#ifdef MMI_LDN_SUPPORT
    case APP_MN_LND_READY_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_LND_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2486_112_2_18_2_45_4_23,(uint8*)"");
        result = HandleLdnReadyInd(param);
        break;
    case APP_MN_LND_UPDATE_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_LND_UPDATE_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2490_112_2_18_2_45_4_24,(uint8*)"");
        result = HandleLDNUpdateCnf(param);
        break;
#endif
        
    case APP_MN_SET_BAND_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SET_BAND_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2496_112_2_18_2_45_4_25,(uint8*)"");
        // 该消息用于通知用户MNPHONE_SelectBand()的操作结果。
        // 结构中包含选择操作完成后当前的Band状况。
        result = HandleSelectBandCnf(param);
        MMIAPIENG_BandSelectCnf(param);
        break;
        
    case APP_MN_SIM_GET_PIN_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_GET_PIN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2504_112_2_18_2_45_4_26,(uint8*)"");
        // 该消息用于通知用户要求输入PIN1或者PUK1，并可获得PIN1是否已经被锁。
#ifdef MMI_SIM_LOCK_SUPPORT
        if (MMK_IsOpenWin(PHONE_SIM_LOCK_WAIT_WIN_ID))
        {
            MnSimGetPinIndS *sim_get_pin_ind_ptr = PNULL;
            
            sim_get_pin_ind_ptr = (MnSimGetPinIndS *)param;
            if(MMI_DUAL_SYS_MAX<= sim_get_pin_ind_ptr->dual_sys)
            {
                //SCI_TRACE_LOW:"HandlePsAndRefMsg:dual_sys=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2514_112_2_18_2_45_4_27,(uint8*)"d",sim_get_pin_ind_ptr->dual_sys);
                result = MMI_RESULT_FALSE;
                break;
            }
            if (MNSIM_IsPin1EnableEx(sim_get_pin_ind_ptr->dual_sys)
                && MMI_PIN_MIN_LENGTH <= g_pin_value[sim_get_pin_ind_ptr->dual_sys].blocklen)//deactive ps后，由于底层不会自动重新输入pin码，需要 手动
            {
                //response pin before receive signal of sim is ready
                MNPHONE_ResponsePinEx(sim_get_pin_ind_ptr->dual_sys, MNSIM_PIN1, FALSE, &g_pin_value[sim_get_pin_ind_ptr->dual_sys], PNULL);
            }
        }
        else
#endif
        {
        result = HandleSimPinInd(param);
        }
        break;
        
    case APP_MN_SIM_PIN_FUNC_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_PIN_FUNC_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2533_112_2_18_2_45_4_28,(uint8*)"");
        // 该消息用于通知用户输入的PIN1，PUK1或者PIN2,PUK2，并可获得PIN1,PIN2是否已经被锁。
        result = HandleSimPinCnf(param);
        break;
        
    case APP_MN_SIM_READY_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2539_112_2_18_2_45_4_29,(uint8*)"");
        // 该消息用于汇报SIM卡准备就绪，同时会附带PIN的状况，HPLMN 等SIM卡的信息。
        HandleSimReadyInd(param);
		#ifdef FLASH_SUPPORT
			if(!MMK_IsOpenWin(PHONE_PIN_INPUT_WIN_ID)&&//pin 码
				!MMK_IsOpenWin(PHONE_PUK_INPUT_WIN_ID)//puk 码
                #ifdef MMI_SIM_LOCK_SUPPORT
                      &&!MMK_IsOpenWin(PHONE_SIM_LOCK_PASSWORD_INPUTING_WIN_ID)
                      &&!MMK_IsOpenWin(PHONE_SIM_LOCK_COUNTDOWN_WIN_ID)
                #endif	   
				&&MMIAPIPHONE_GetOpenIdleFlag()//这里需要打开IDLE窗口
				)
			{
			   //只有在所有的pin码和puk码都输入完成之后，才尝试进入idle，
			   //且，只有在不存在防盗保护和USB窗口的情况下才能成功进入IDLE.
			   MMIAPIPHONE_SetOpenIdleFlag(FALSE);
			   MMIAPIPHONE_AtemptEnterIdle();
			}
        #endif
        //init call log
        MMIAPICL_Init();
        
        result = MMI_RESULT_FALSE;
        break;

#ifdef MMI_NETWORK_NAME_SUPPORT
    case APP_MN_NETNAME_READY_IND:
        SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_NETNAME_READY_IND");
        // 该消息用于在SIM卡初始化完成时，上报当前那张卡可以使用PNN
        result = MMIPHONE_HandleNetNameReadyInd(param);
        break;
        
    case APP_MN_CPHS_READY_IND:
        SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_CPHS_READY_IND");
        // 该消息用于在SIM卡初始化完成时，上报当前那张卡可以使用ONS
        result = MMIPHONE_HandleCPHSReadyInd(param);
        break;
        
    case APP_MN_OPER_NAME_IND:
        SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_OPER_NAME_IND");
        // 该消息用于在SIM卡初始化完成时，上报当前那张卡可以使用PNN
        //result = MMIPHONE_HandleOperNameInd(param);
        break;

    case APP_MN_SIM_RECORD_NUM_CNF:
        // 该消息用于汇报SIM卡中PNN,OPL PNNI等总数
        SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_SIM_RECORD_NUM_CNF");
        result = MMIPHONE_HandleSIMRecordNumCnf(param);
        break;
    
    case APP_MN_SIM_RECORD_READ_CNF:
        // 该消息用于汇报读取SIM卡中PNN,OPL PNNI等具体某条的内容
        SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_SIM_RECORD_READ_CNF");
        result = MMIPHONE_HandleSIMRecordReadCnf(param);
        break;
    
    case APP_MN_SIM_BINARY_READ_CNF:
        // 该消息用于汇报读取SIM卡中SPNI具体某条的内容
        SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_SIM_BINARY_READ_CNF");
        //get SPNI cnf
        result = MMIPHONE_HandleSIMBinaryReadCnf(param);
        break;
#endif
            
    case APP_MN_FDN_SERVICE_IND://mmi fdn service ind
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_FDN_SERVICE_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2565_112_2_18_2_45_4_30,(uint8*)"");
        result = MMI_RESULT_FALSE;
        break;
        
    case APP_MN_PS_READY_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PS_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2570_112_2_18_2_45_4_31,(uint8*)"");
        e_dual_sys = ((MnPSReadyIndS *)param)->dual_sys;
        MMIAPISET_GetCallLineInfo(e_dual_sys);
        MMIAPIPHONE_SetIsPsReady(e_dual_sys, TRUE);
        break;
        
    case APP_MN_SIM_NOT_READY_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_NOT_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2577_112_2_18_2_45_4_32,(uint8*)"");
        // 该消息用于汇报SIM卡未能成功初始化或是SIMCARD被拔出。从该消息中可获得具体的原因，
        // 并显示在界面上。
        HandleSimNotReadyInd(param);
       
        result = MMI_RESULT_FALSE;
        break;
        
    case APP_MN_SCELL_RSSI_IND:
        //SCI_TRACE_LOW("HandlePsAndRefMsg:receive APP_MN_SCELL_RSSI_IND");
        // 该消息用于在服务小区信号强度改变时，汇报当前服务小区信号强度。
        // 例：在idle界面显示。
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SCELL_RSSI_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2589_112_2_18_2_45_5_33,(uint8*)"");
        result = HandleScellRssiInd(param);
        break;
        
    case APP_MN_PS_POWER_ON_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PS_POWER_ON_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2594_112_2_18_2_45_5_34,(uint8*)"");
        e_dual_sys = ((MnPsPowerOnCnfS *)param)->dual_sys;
        MMIAPISET_GetCallLineInfo(e_dual_sys);
        MMIAPIPHONE_SetIsPsReady(e_dual_sys, TRUE);
        break;
        
    case APP_MN_PS_POWER_OFF_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PS_POWER_OFF_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2601_112_2_18_2_45_5_35,(uint8*)"");
        HandlePowerPsOffCnf(param);
        break;
        
    case APP_MN_DEACTIVE_PS_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_DEACTIVE_PS_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2606_112_2_18_2_45_5_36,(uint8*)"");
        HandleDeactivePsCnf(param);
        break;
        
    case APP_MN_SIM_POWER_ON_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_POWER_ON_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2611_112_2_18_2_45_5_37,(uint8*)"");
#ifdef MMI_SIM_LOCK_SUPPORT
        if(MMK_IsOpenWin(PHONE_SIM_LOCK_WAIT_WIN_ID))
        {
            MMK_CloseWin(PHONE_SIM_LOCK_WAIT_WIN_ID);
        }
#endif
        HandlePowerOnSimCnf(param);
        break;

    case APP_MN_SIM_POWER_OFF_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_SIM_POWER_OFF_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2622_112_2_18_2_45_5_38,(uint8*)"");
        HandlePowerOffSimCnf(param);
        break;

#ifdef MMI_PREFER_PLMN_SUPPORT
    //更新优选网络的列表信息
    case APP_MN_PREFER_PLMN_CHANGE_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_MN_PREFER_PLMN_CHANGE_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2629_112_2_18_2_45_5_39,(uint8*)"");
        HandlePreferPlmnChangeCnf(param);
        break;
#endif

#ifdef AOC_SUPPORT
        //获取PUCT计费标准信号相应处理
    case APP_MN_PUCT_INFO_CNF:
        //SCI_TRACE_LOW:"HandlePuctInfoCnf: receive APP_MN_PUCT_INFO_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2637_112_2_18_2_45_5_40,(uint8*)"");
        HandlePuctInfoCnf(param);
        break;
        
    case APP_MN_AOC_MODIFIED_IND:
        //SCI_TRACE_LOW:"None Handle: receive APP_MN_AOC_MODIFIED_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2642_112_2_18_2_45_5_41,(uint8*)"");
        break;
#endif
#ifdef MMI_DUAL_BATTERY_SUPPORT
    case DBAT_MSG_UPDATE_STATE_IND:
    case DBAT_MSG_BAT_PLUGIN_IND:
    case DBAT_MSG_BAT_PLUGOUT_IND:
    case DBAT_MSG_SWITCH_BAT_IND://目前不处理
    case DBAT_MSG_CHARGE_START_IND:
    case DBAT_MSG_WARNING_IND:
    case DBAT_MSG_SHUTDOWN_IND://和单电池处理一致
    case DBAT_MSG_CHARGE_FINISH:
    case DBAT_MSG_CHARGE_DISCONNECT:
    case DBAT_MSG_CHARGE_FAULT:
        result = MMIPHONE_ChargeHandlePsAndRefMsg(app_ptr,msg_id,param);
        break;
#else
    case CHR_MSG_CAP_IND:
    case CHR_MSG_CHARGE_START_IND:
    case CHR_MSG_CHARGE_END_IND:
    case CHR_MSG_WARNING_IND:
    case CHR_MSG_SHUTDOWN_IND:
    case CHR_MSG_CHARGE_FINISH_IND:
    case CHR_MSG_CHARGE_DISCONNECT_IND:
    case CHR_MSG_CHARGE_FAULT_IND:
        result = MMIPHONE_ChargeHandlePsAndRefMsg(app_ptr,msg_id,param);
        break;
#endif     
    case MISC_MSG_RTC_ALARM:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive MISC_MSG_RTC_ALARM"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2671_112_2_18_2_45_5_42,(uint8*)"");
        // 该消息用于汇报闹钟所设定的时间到，开始闹铃。
        // 注：设备关机时，启动闹铃的方式与该消息无关
        // MMIAPIALM_StartAlarm(TRUE);
        MMIALMSVC_StartService();
        break;
	case MSG_SET_DEL_ALARMSET_CNF:
		//delete one alarm event finished confirm
		MMIALM_DeleteCnf();
		break;        
    case USB_PLUGIN_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive UPM_PLUGIN_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2682_112_2_18_2_45_5_43,(uint8*)"");
        // 该消息用于汇报U盘插入。
        result = MMIAPIUdisk_HandleUsbCablePlugIn();
        break;
        
    case USB_PLUGOUT_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive UPM_PLUGOUT_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2688_112_2_18_2_45_5_44,(uint8*)"");
        // 该消息用于汇报U盘被拔出。
        //   MMIAPIUdisk_SetIsUse(FALSE);
        result = MMIAPIUdisk_HandleUsbCablePlugOut();
        MMIAPIPHONE_CleanEventDelaied(MMI_USB_DELAY);
        //MMIAPIUdisk_SetUSBDelay(FALSE);
        break;
	case USB_SERVICE_UDISK_STARTED_IND:
		//usb service started
		MMIAPIUdisk_USBServiceStartInd(MMI_USB_SERVICE_UDISK);
		break;
	case USB_SERVICE_UDISK_STOPPED_IND:
		//usb service stopped
        MMIAPIUdisk_USBServiceStopInd(MMI_USB_SERVICE_UDISK);
		break;
	case USB_SERVICE_UCOM_STARTED_IND:
		MMIAPIUdisk_USBServiceStartInd(MMI_USB_SERVICE_UCOM);
		break;
	case USB_SERVICE_UCOM_STOPPED_IND:
		MMIAPIUdisk_USBServiceStopInd(MMI_USB_SERVICE_UCOM);
		break;
	case USB_SERVICE_UPCC_STARTED_IND:
		MMIAPIUdisk_USBServiceStartInd(MMI_USB_SERVICE_UPCC);
		break;
	case USB_SERVICE_UPCC_STOPPED_IND:
		MMIAPIUdisk_USBServiceStopInd(MMI_USB_SERVICE_UPCC);
		break;
	case USB_SERVICE_LOG_STARTED_IND:
		MMIAPIUdisk_USBServiceStartInd(MMI_USB_SERVICE_LOG);
		break;
	case USB_SERVICE_LOG_STOPPED_IND:
		MMIAPIUdisk_USBServiceStopInd(MMI_USB_SERVICE_LOG);
		break;
    case USB_READY_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive UPM_READY_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2722_112_2_18_2_45_5_45,(uint8*)"");
        // 该消息用于汇报U盘已经就绪
        //USB要在菜单中开启，所以处理也在开启处
    //    MMIAPIUdisk_SetUPMIsReady(TRUE);
        break;
        
    case USB_ERROR_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive UPM_ERROR_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2729_112_2_18_2_45_5_46,(uint8*)"");
        // 该消息用于汇报U盘操作失败
        //result = MMIUDISK_Handlefail();
        break;

    case APP_AUDIO_END_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_AUDIO_END_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2735_112_2_18_2_45_5_47,(uint8*)"");
        MMISRVAUD_HandleCallBackMsg((void *)param);
        break;
      
#ifdef WRE_SUPPORT
    case APP_WRE_AUDIO_END_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive APP_WRE_AUDIO_END_IND"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2741_112_2_18_2_45_5_48,(uint8*)"");
        MMIWRE_HandleCallBackMsg((void *)param);
        break;
#endif

#ifdef MMIAP_SORT_SUPPORT
    case MSG_APP_MP3_SORT:
        MMIAPIMP3_HandleSortMsg();
        break;
#endif
#ifdef ASP_SUPPORT
    case APP_ASP_PLAY_AUDIO_END_IND:
        MMIAPIASP_HandlePlayAudioEnd();
        break;
#endif

#ifdef JAVA_SUPPORT_SUN
    case APP_JAVA_MP4_DISPLAY_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg(),receive APP_JAVA_MP4_DISPLAY_IND!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2759_112_2_18_2_45_5_49,(uint8*)"");
        MMIAPIJAVA_CallbackFrameEndNotifyProc();
        break;

    case  APP_JAVA_MP4_END_IND:
         //SCI_TRACE_LOW:"HandlePsAndRefMsg(),receive APP_JAVA_MP4_END_IND!"
         SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2764_112_2_18_2_45_5_50,(uint8*)"");
         MMIAPIJAVA_CallbackEndNotifyProc();
         break;
    case APP_JAVA_SNAPSHOT_END_IND:
         //SCI_TRACE_LOW:"HandlePsAndRefMsg(),receive APP_JAVA_SNAPSHOT_END_IND!"
         SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2768_112_2_18_2_45_5_51,(uint8*)"");
         MMIAPIJAVA_CallbackSnapshotEndNotifyProc();
         break;
    case APP_JAVA_END_IND:
    case APP_JAVA_LAUNCH_INPUT_METHOD_IND:
    case APP_JAVA_LAUNCH_FILE_BROWSER_IND:
    case APP_JAVA_LAUNCH_WAP_BROWSER_IND:
    case APP_JAVA_START_AMS_IND:
    case APP_JAVA_STATUSBAR_OPERATION_IND:
    case APP_JAVA_SCREEN_REFLUSH_IND:
    case APP_JAVA_LAUNCH_PRELOAD_MIDLET_IND:
    case APP_JAVA_LAUNCH_VM_EXIT_WIN_IND:
    case APP_JAVA_BACKGROUND_TIMER_EVENT:
    case APP_JAVA_PDP_ACTIVE:
    case APP_JAVA_PDP_DEACTIVE:
    case APP_JAVA_VIDEO:
        //SCI_TRACE_LOW("HandlePsAndRefMsg:receive Java related message");
        MMIAPIJAVA_DispatchVmMessage(param);
        break;
#endif
#ifdef JAVA_SUPPORT_IA
    case MSG_OPEN_TEXTEDIT_WIN:
        //SCI_TRACE_LOW:"apple phone param = %x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2790_112_2_18_2_45_5_52,(uint8*)"d",param);
        MMIAPIJAVA_OpenTextEditWindow(PNULL);
        break;
    case MSG_JAVA_OUT_OF_MEMORY:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg msg_id = %x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2794_112_2_18_2_45_5_53,(uint8*)"d", msg_id);
        MMIAPIJAVA_jwe_vmErrorHandle(*(int32*)param);
        break;
    case MSG_JAVA_CLOSE_APP_WIN:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_CLOSE_APP_WIN param = %d, %x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2798_112_2_18_2_45_5_54,(uint8*)"dd",param,*(MMI_HANDLE_T *)param);
		if(MMK_IsOpenWin(*(MMI_HANDLE_T *)param))
	    {
    	    MMK_CloseWin(*(MMI_HANDLE_T *)param);
	    }
        break;
    case MSG_JAVA_AUTORUNAPP:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_AUTORUNAPP param = %x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2805_112_2_18_2_45_5_55,(uint8*)"d",param);
	    MMIAPIJAVA_AutoRunApp(*(int32*)param);	
        break;
    case  MSG_JAVA_SEND_TIMER:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_SEND_TIMER param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2809_112_2_18_2_45_5_56,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_StartSendTimer(*(int32 *)param);
        break;
    case MSG_JAVA_LINK_TIMER:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_LINK_TIMER param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2813_112_2_18_2_45_5_57,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_StartLinkTimer(*(int32 *)param);
        break;
    case MSG_JAVA_CONNECT_TIMER:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_CONNECT_TIMER param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2817_112_2_18_2_45_5_58,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_StartConnectTimer(*(int32 *)param);
        break;
    case MSG_JAVA_RECEIVE_TIMER:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_RECEIVE_TIMER param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2821_112_2_18_2_45_5_59,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_StartRecvTimer(*(int32 *)param);
        break;
    case MSG_JAVA_STOP_SOCKET_TIMER:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_STOP_SOCKET_TIMER param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2825_112_2_18_2_45_5_60,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_StopSockTimer(*(int32 *)param);
        break;
    case MSG_JAVA_UDP_LINK:
        MMIAPIJAVA_StartUDPLink(*(int32 *)param);
        break;
    case MSG_JAVA_PAINT:
        MMIAPIJAVA_CreateWinPaintTimer();
        break;
    case MSG_JAVA_VM_END_IND:
        MMIAPIJAVA_Exit_FinalizeResource();
        //if(MMK_IsOpenWin(*(MMI_HANDLE_T *)param))
        //	MMK_CloseWin(*(MMI_HANDLE_T *)param);
        break;
    case MSG_JAVA_VM_SUSPEND_IND:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_VM_SUSPENDED_IND param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2840_112_2_18_2_45_5_61,(uint8*)"dd", param,*(int32 *)param);
	    MMIAPIJAVA_StartCheckVmState(MSG_JAVA_VM_SUSPEND_IND,param);	 
        break;
    case MSG_JAVA_MEDIA_STOPTIME_TIMER:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_MEDIA_STOPTIME_TIMER param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2844_112_2_18_2_45_5_62,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_StartMediaStopTimer(*(int32 *)param);
        break;
     case MSG_JAVA_DEACTIVE_PDP:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_DEACTIVE_PDP param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2848_112_2_18_2_45_5_63,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_PDPDeactive();
        break;
     case MSG_JAVA_PLATFORM_REQUEST:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_PLATFORM_REQUEST param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2852_112_2_18_2_45_5_64,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_PlatformRequest(*(uint32 *)param);
        break;
     case MSG_JAVA_AUDIO_PLAY_AGAIN:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg MSG_JAVA_AUDIO_PLAY_AGAIN param = %d %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2856_112_2_18_2_45_5_65,(uint8*)"dd", param,*(int32 *)param);
        MMIAPIJAVA_AudioClientPlayAgain();
        break;
#endif
     case MSG_FS_DELETE_CNF:
         {
			 MMIFILE_ASYN_CALLBACK_T  *callback_ptr = (MMIFILE_ASYN_CALLBACK_T*) param;
			 BOOLEAN                  fs_result = FALSE;

			 if(SFS_ERROR_NONE == callback_ptr->error)
			 {
				 fs_result = TRUE;
			 }
			 MMK_PostMsg(callback_ptr->win_id, callback_ptr->msg_id, &fs_result, sizeof(fs_result));
			 MMIAPICOM_TriggerMMITask();
         }
		 break;

#ifdef DRM_SUPPORT
    case DRM_SIG_DH_INSTALL_FILE_REQ:
    case DRM_SIG_DH_INSTALL_FILE_CNF:
    case DRM_SIG_GET_FILE_MIME_CNF:
	case DRM_SIG_CANCEL_INSTALL_FILE_CNF:
    case DRM_SIG_DH_DELETE_RIGHTS_FILE_CNF:

        MMIDRM_HandleDrmControlMsg(msg_id, param);
        break;
#endif

#ifdef IKEYBACKUP_SUPPORT
    case MSG_SMS_IKB_BACKUP_ITEM_DATA_REQ:
        {
            void *original_param = PNULL;

            original_param = (void*)((uint32)param-sizeof(xSignalHeaderRec));
            MMIAPISMS_IKeyBackup(original_param);
        }
        
        break;
    case MSG_SMS_IKB_RESTORE_ITEM_DATA_REQ:
        {
            void *original_param = PNULL;

            original_param = (void*)((uint32)param-sizeof(xSignalHeaderRec));
            MMIAPISMS_IKeyRestore(original_param);
        }
        break;
    case MSG_IKB_LOAD_ALL_NODE_RES:
    case MSG_IKB_BACKUP_DATA_RES:
    case MSG_IKB_BACKUP_ITEM_DATA_REQ:
    case MSG_IKB_RESTORE_DATA_RES:
    case MSG_IKB_RESTORE_ITEM_DATA_REQ:
    case MSG_IKB_GET_ALL_PACK_DATA_CNF:
    case MSG_IKB_PROGRESS_VALUE_RES:
    case MSG_IKB_CANCEL_ACTION_RES:
        MMIAPIIKB_HandleAppSigRes(msg_id, param);
        break;
#endif

#ifdef MMI_VCALENDAR_SUPPORT
    case MSG_VCALENDAR_SAVE_IND:
        MMIVCAL_HandleWriteCNF(MSG_VCALENDAR_SAVE_SCH_CNF,PNULL);
        break;
    case   MSG_SCH_GET_ALL_FILE_FINISH:
        MMK_SendMsg(MMISCH_VCALENDAR_LOADFILE_WAITING_WIN_ID, MSG_SCH_GET_ALL_FILE_FINISH,PNULL);
        break;
    case  MSG_SCH_LOAD_NEXT_VCALENDAR:
        MMK_SendMsg(MMISCH_VCALENDAR_LOADFILE_WAITING_WIN_ID, MSG_SCH_LOAD_NEXT_VCALENDAR,PNULL);
        break;
    case MSG_SCH_DELETE_IND:
        MMIAPISCH_DelSchNotify();
        break;
#endif
#ifdef FLASH_SUPPORT
    case MSG_FLASH_UPDATE_WINDOW:
		//SCI_TRACE_LOW:"flash_printf:@@@@@MSG_FLASH_UPDATE_WINDOW  0  ,received the message."
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2931_112_2_18_2_45_5_66,(uint8*)"");
        if(MMIAPIFlash_FlashIsVisiable() &&  MMK_IsFocusWin(MAIN_IDLE_WIN_ID))
        {
			//SCI_TRACE_LOW:"flash_printf:@@@@@MSG_FLASH_UPDATE_WINDOW  1  ,received the message."
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2934_112_2_18_2_45_5_67,(uint8*)"");
            MMK_PostMsg(MAIN_IDLE_WIN_ID, MSG_FULL_PAINT, 0, 0);
        }
        break;
#endif
#ifdef MMI_SNTP_SUPPORT
    case SNTP_SIG_GET_TIME_CNF:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:receive SNTP_SIG_GET_TIME_CNF"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2941_112_2_18_2_45_5_68,(uint8*)"");
        MMIAPIAUT_ManualUpdateTimeHandleSNTPcnf(msg_id, param);
    break;
#endif
    default:
        //SCI_TRACE_LOW:"HandlePsAndRefMsg:msg_id = 0x%04x"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2946_112_2_18_2_45_5_69,(uint8*)"d", msg_id);
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : Interrogate Cfu for idle icon display
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_InterrogateCfuForIdle(MN_DUAL_SYS_E dual_sys)
{
    uint32 i = 0;
    BOOLEAN is_allow_do_cfu = TRUE;
    MN_DUAL_SYS_E cfu_dual_sys = MN_DUAL_SYS_MAX;
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMISET_DAULSYS_ONLY_SIM1_ON;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    e_dualsys_setting = MMIAPISET_GetMultiSysSetting();

    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if ((e_dualsys_setting & (0x01 << i)) && MMIAPIPHONE_GetSimExistedStatus(i)
            && !s_is_network_status_ready[i])
        {
            is_allow_do_cfu = FALSE;

            break;
        }
    }
    
    if (is_allow_do_cfu && !MMIAPISET_GetIsQueryCfu())
    {
        if (!MMIAPISET_GetCfuInfo(dual_sys)
            &&((PLMN_NORMAL_GSM_ONLY == g_service_status[dual_sys].plmn_status)
            || (PLMN_NORMAL_GSM_GPRS_BOTH == g_service_status[dual_sys].plmn_status)))
        {
            cfu_dual_sys = dual_sys;
        }
        
        //SCI_TRACE_LOW:"MMIAPIPHONE_InterrogateCfuForIdle dual_sys=%d,cfu_dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_2992_112_2_18_2_45_5_70,(uint8*)"dd", dual_sys, cfu_dual_sys);
        
        if (MN_DUAL_SYS_MAX != cfu_dual_sys)
        {
            if(MMIAPIENG_GetCallForwardQuery())
            {
                //从网络中查询无条件转移状态,只需查询线路1是否设置了CFU，因为线路2无法设置CFU
                MMIAPISET_InterrogateCfuStatusFromPs(cfu_dual_sys, 0);
            }
            else
            {
                if (!MMIAPISET_GetIsQueryCfu())
                {
                    MMIAPISET_SetIsQueryCfu(TRUE,dual_sys);
                }
            }
        }
    }
    else
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_InterrogateCfuForIdle is_allow_do_cfu=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3012_112_2_18_2_45_6_71,(uint8*)"d", is_allow_do_cfu);
    }
}

#if defined(MMI_SMSCB_SUPPORT)
/*****************************************************************************/
// 	Description : startup sms cb
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StartupSmsCB(MN_DUAL_SYS_E dual_sys)
{
    if (!s_is_startup_smscb[dual_sys])
    {
        s_is_startup_smscb[dual_sys] = TRUE;

        MMIAPISMSCB_Init(dual_sys);
    }
}

/*****************************************************************************/
// 	Description : ClearCbEnable
//	Global resource dependence : 
//  Author: fengming.huang
//	Note:
/*****************************************************************************/
LOCAL void ClearCbEnable(MN_DUAL_SYS_E dual_sys)
{
    s_is_startup_smscb[dual_sys] = FALSE;
}
#endif

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_NETWORK_STATUS_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleNetworkStatusInd(
                                          DPARAM              param
                                          )
{
    MnNetworkStatusIndS network_status_ind = *((MnNetworkStatusIndS *)param);
    MMI_RESULT_E    result = MMI_RESULT_TRUE;    
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMISET_DAULSYS_ONLY_SIM1_ON;


#ifdef WRE_SUPPORT
	{
		MMIWRE_NetworkIndHandle(param);
	}
#endif
    
#ifdef MMI_MULTI_SIM_SYS_SINGLE    
    network_status_ind.dual_sys = MN_DUAL_SYS_1;
#endif

#ifdef MMI_NETWORK_NAME_SUPPORT
#ifdef WIN32
    if(0 == network_status_ind.lac)
    {
         network_status_ind.lac = 0x1001;// set lac temp
    }
#endif
    if(0 != network_status_ind.lac)
    {
        MN_PLMN_T  cur_plmn = {0};

        cur_plmn.mcc  = network_status_ind.mcc;
        cur_plmn.mnc  = network_status_ind.mnc;
        cur_plmn.mnc_digit_num = network_status_ind.mnc_digit_num;
        //set  PNN str when lac changes
        MMIPHONE_SetPNNWithLac(network_status_ind.dual_sys, cur_plmn, network_status_ind.lac);
    }
#endif

	//更新设置界面窗口
#if !defined(MMI_MULTI_SIM_SYS_SINGLE) 
    if ((network_status_ind.plmn_is_selecting != s_is_network_status_ready[network_status_ind.dual_sys])
        ||(network_status_ind.plmn_status != g_service_status[network_status_ind.dual_sys].plmn_status)
        )
    {
        MMIAPIPHONE_UpdateNetworkStatusWin();
    }
#endif   
    
    if (!network_status_ind.plmn_is_selecting)
    {
        s_is_network_status_ready[network_status_ind.dual_sys] = TRUE;
    }
    
    e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    
    // 保存网络状态信息
    StoreNetworkStatus(&network_status_ind);
    
    #if defined(BLUETOOTH_SUPPORT) && defined(BT_BQB_SUPPORT)
    MMIAPIBT_SetNetWorkRoamStatus(MMIAPIPHONE_GetIsNetworkRoaming());
    MMIAPIBT_SetOperatorName();
    #endif

#ifdef MMI_DUALMODE_ENABLE
    MMIAPIPHONE_TDOrGsm(network_status_ind);    
#endif

    MMIAPIPHONE_SetIdleGprsState(network_status_ind.dual_sys);

    NotifyPlmnStatusChange(network_status_ind.dual_sys, network_status_ind.plmn_status);
    
    //set idle string
    MAIN_SetIdleNetWorkName();

    MAIN_SetIdleRoamingState(network_status_ind.dual_sys, network_status_ind.plmn_is_roaming);
    
    MMIAPIPHONE_InterrogateCfuForIdle(network_status_ind.dual_sys);        //查询Cfu
    
    MAIN_UpdateLineState();  //重新刷新line state的显示

    if ((PLMN_NORMAL_GSM_ONLY == g_service_status[network_status_ind.dual_sys].plmn_status)
        || (PLMN_NORMAL_GPRS_ONLY == g_service_status[network_status_ind.dual_sys].plmn_status)
        || (PLMN_NORMAL_GSM_GPRS_BOTH == g_service_status[network_status_ind.dual_sys].plmn_status))
    {
#if defined(MMI_SMSCB_SUPPORT)
        StartupSmsCB(network_status_ind.dual_sys);
#endif
        
//#ifdef MMI_ENABLE_DCD    //for dcd internation           
//        MMIAPIDCD_SetCurMCC(g_service_status[network_status_ind.dual_sys].plmn.mcc, g_service_status[network_status_ind.dual_sys].plmn.mnc);                		    
//#endif
            
        MMIAPIDM_HandleNetChange();
    }
    //if no service, the rx display 0 directly
    else if (!g_service_status[network_status_ind.dual_sys].is_slecting
        && PLMN_NO_SERVICE == g_service_status[network_status_ind.dual_sys].plmn_status)
    {
        g_service_status[network_status_ind.dual_sys].is_first_rssi = TRUE;//跳到No Service时，直接显示格数
        g_service_status[network_status_ind.dual_sys].rssiLevel = 0;

        MAIN_SetIdleRxLevel(network_status_ind.dual_sys, 0, TRUE);
    }
#ifdef PDA_UI_DROPDOWN_WIN
    MMIDROPDOWNWIN_UpdateMainWin();
#endif
    
    return (result);
}

/*****************************************************************************/
// 	Description : 设置IDLE GPRS显示状态
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetIdleGprsState(MN_DUAL_SYS_E dual_sys)
{
#ifdef MMI_DUALMODE_ENABLE
    if(MMI_DUAL_SYS_MAX <= dual_sys)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_SetIdleGprsState err dual_sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3145_112_2_18_2_45_6_72,(uint8*)"d", dual_sys);
        return;
    }

    //SCI_TRACE_LOW:"[dual mode] MMIAPIPHONE_SetIdleGprsState: rat = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3149_112_2_18_2_45_6_73,(uint8*)"d", g_service_status[dual_sys].rat);

    switch(g_service_status[dual_sys].rat) 
    {
    case MMI_GMMREG_RAT_GPRS:
        MMIAPIPHONE_StopSTTDOrGsm();
        // update
        GUIWIN_UpdateStb();
        //刷新小屏的显示，Gprs状态
        MMISUB_UpdateDisplaySubLcd(); 
        break;
        
    case MMI_GMMREG_RAT_3G:
        MAIN_SetIdleTdState();
        break;
        
    case MMI_GMMREG_RAT_UNKNOWN:
        MMIAPIPHONE_StopSTTDOrGsm();
        MAIN_SetIdleUnknownState();
        break;
        
    default:
        break;
    }
    if( (PLMN_NORMAL_GPRS_ONLY == g_service_status[dual_sys].plmn_status)
        || (PLMN_NORMAL_GSM_GPRS_BOTH == g_service_status[dual_sys].plmn_status) )
    {
#ifdef MMI_GPRS_SUPPORT
        if (MMIPDP_NOTIFY_EVENT_ACTIVED == MMIAPIPDP_GetPdpLastEvent(dual_sys)) /*@CR255469 modify 2011.08.23*/
        {
            MMIAPIIDLE_SetGprsInfo(dual_sys, MMIPDP_NOTIFY_EVENT_ACTIVED, FALSE); /*@CR255469 modify 2011.08.23*/
        }
        else
#endif
        {
            MAIN_SetIdleGprsState(dual_sys, TRUE);
        }
    }
    else
    {
#ifdef MMI_GPRS_SUPPORT
        if (MMIPDP_NOTIFY_EVENT_DEACTIVED == MMIAPIPDP_GetPdpLastEvent(dual_sys)) /*@CR255469 modify 2011.08.23*/
        {
            MMIAPIIDLE_SetGprsInfo(dual_sys, MMIPDP_NOTIFY_EVENT_DEACTIVED, FALSE); /*@CR255469 modify 2011.08.23*/
        }
        else
#endif
        {
            MAIN_SetIdleGprsState(dual_sys, FALSE);
        }
    }
#else
    //SCI_TRACE_LOW:"MMIAPIPHONE_SetIdleGprsState dual_sys=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3201_112_2_18_2_45_6_74,(uint8*)"d", dual_sys);

    if(MMI_DUAL_SYS_MAX <= dual_sys)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_SetIdleGprsState err dual_sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3205_112_2_18_2_45_6_75,(uint8*)"d", dual_sys);
        return;
    }

#if defined(MMI_GPRS_ATTACH_ICON_SUPPORT)
    //只有当进行GPRS连接时才显示G图标
    if( (PLMN_NORMAL_GPRS_ONLY == g_service_status[dual_sys].plmn_status)
        || (PLMN_NORMAL_GSM_GPRS_BOTH == g_service_status[dual_sys].plmn_status))
    {
        MAIN_SetIdleGprsState(dual_sys, TRUE);
    }
    else
    {
        MAIN_SetIdleGprsState(dual_sys, FALSE);
    }
#else
    //只要网络支持GPRS就显示G图标
    if (MMIAPISET_GetMultiSysSettingNum(PNULL, 0) > 1)
    {
        if( (PLMN_NO_SERVICE == g_service_status[dual_sys].plmn_status)
            || (PLMN_EMERGENCY_ONLY == g_service_status[dual_sys].plmn_status) )
        {
            MAIN_SetIdleGprsState(dual_sys, FALSE);
        }
        else
        {
            MAIN_SetIdleGprsState(dual_sys, g_service_status[dual_sys].gprs_support);
        }
    }
    else
    {
        if( (PLMN_NORMAL_GPRS_ONLY == g_service_status[dual_sys].plmn_status)
            || (PLMN_NORMAL_GSM_GPRS_BOTH == g_service_status[dual_sys].plmn_status) )
        {
            MAIN_SetIdleGprsState(dual_sys, TRUE);
        }
        else
        {
            MAIN_SetIdleGprsState(dual_sys, FALSE);
        }
    }
#endif
#endif
}

/*****************************************************************************/
// 	Description : 保存网络状态
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StoreNetworkStatus(
                               MnNetworkStatusIndS *status_ptr
                               )
{
    MN_DUAL_SYS_E dual_sys = status_ptr->dual_sys;
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"StoreNetworkStatus error dual_sys = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3261_112_2_18_2_45_6_76,(uint8*)"d", dual_sys);
        return;
    }
    g_service_status[dual_sys].plmn_status = status_ptr->plmn_status;
    g_service_status[dual_sys].gprs_support = status_ptr->gprs_support;
    g_service_status[dual_sys].mbms_support = status_ptr->mbms_support;
    g_service_status[dual_sys].edge_support = status_ptr->edge_support;
    g_service_status[dual_sys].hsdpa_support = status_ptr->hsdpa_support;
    g_service_status[dual_sys].hsupa_support = status_ptr->hsupa_support;
    g_service_status[dual_sys].lac = status_ptr->lac;
    g_service_status[dual_sys].plmn.mcc = status_ptr->mcc;
    g_service_status[dual_sys].plmn.mnc = status_ptr->mnc;
    g_service_status[dual_sys].plmn.mnc_digit_num	= status_ptr->mnc_digit_num;
    g_service_status[dual_sys].cell_id = status_ptr->cell_id;
    g_service_status[dual_sys].isRoaming = status_ptr->plmn_is_roaming;
    g_service_status[dual_sys].is_slecting = status_ptr->plmn_is_selecting;    

    g_service_status[dual_sys].is_add_ci = status_ptr->is_add_ci;
    g_service_status[dual_sys].is_spn_need = status_ptr->is_spn_need;
    SCI_MEMSET(g_service_status[dual_sys].spn, 0x00, sizeof(g_service_status[dual_sys].spn));

    if (status_ptr->is_spn_need && 0 < status_ptr->spn_len)
    {
        g_service_status[dual_sys].spn_len = MIN(status_ptr->spn_len, sizeof(g_service_status[dual_sys].spn));
        SCI_MEMCPY(g_service_status[dual_sys].spn, status_ptr->spn, g_service_status[dual_sys].spn_len);
    }
    else
    {
        g_service_status[dual_sys].spn_len = 0;
    }

    g_service_status[dual_sys].opn_dcs	= status_ptr->opn_dcs;
    g_service_status[dual_sys].is_opn_need	= status_ptr->is_opn_need;
    g_service_status[dual_sys].is_opn_exist	= status_ptr->is_opn_exist;
    SCI_MEMSET(g_service_status[dual_sys].opn, 0x00, sizeof(g_service_status[dual_sys].opn));

    if (status_ptr->is_opn_exist && 0 < status_ptr->opn_len)
    {
        g_service_status[dual_sys].opn_len = MIN(status_ptr->opn_len, sizeof(g_service_status[dual_sys].opn));
        SCI_MEMCPY(g_service_status[dual_sys].opn, status_ptr->opn, g_service_status[dual_sys].opn_len);
    }
    else
    {
        g_service_status[dual_sys].opn_len = 0;
    }
    g_service_status[dual_sys].rat = (MMI_GMMREG_RAT_E)status_ptr->rat;
    //SCI_TRACE_LOW:"NetworkStatus g:%d, e:%d, h:%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3307_112_2_18_2_45_6_77,(uint8*)"ddd", status_ptr->gprs_support, status_ptr->edge_support, status_ptr->hsdpa_support);

}

/*****************************************************************************/
// 	Description : ????plmn
//	Global resource dependence : 
//  Author: yongwei.he
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_GetCurPLMN(
                                MN_PLMN_T * cur_plmn_ptr,
                                MN_DUAL_SYS_E dual_sys
                                )
{
    cur_plmn_ptr->mcc = 	g_service_status[dual_sys].plmn.mcc;
    cur_plmn_ptr->mnc = 	g_service_status[dual_sys].plmn.mnc;
    cur_plmn_ptr->mnc_digit_num = g_service_status[dual_sys].plmn.mnc_digit_num;	
}

/*****************************************************************************/
// 	Description : get the sim hplmn
//	Global resource dependence : 
//  Author: fen.xie
//	Note:   
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_GetSimHPLMN(
                                MN_PLMN_T * cur_plmn_ptr,
                                MN_DUAL_SYS_E dual_sys
                                )
{
    cur_plmn_ptr->mcc = 	s_sim_status[dual_sys].hplmn.mcc;
    cur_plmn_ptr->mnc = 	s_sim_status[dual_sys].hplmn.mnc;
    cur_plmn_ptr->mnc_digit_num = 	s_sim_status[dual_sys].hplmn.mnc_digit_num;	
}

/*****************************************************************************/
// 	Description : Gen PLMN Display
//	Global resource dependence : 
//  Author: wancan.you
//	Note:is_display_digit_format:TRUE Display 460 00
//          is_display_digit_format:FALSE Display CMCC
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GenPLMNDisplay(
                                                                            MMI_GMMREG_RAT_E rat,//IN
                                                                            MN_PLMN_T *plmn_ptr,//IN
                                                                            wchar *display_str,//[IN/OUT]
                                                                            uint16 *display_str_len,//[IN/OUT]
                                                                            BOOLEAN is_display_digit_format)
{
    MMI_TEXT_ID_T network_name_id = TXT_NULL;
    MMI_STRING_T network_str = {0};
    BOOLEAN is_digit_output = FALSE;
#ifdef CSC_XML_SUPPORT
    BOOLEAN is_csc_name = FALSE;
#endif

    if (PNULL == plmn_ptr ||PNULL == display_str ||PNULL == display_str_len)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GenPLMNDisplay plmn_ptr=%d, display_str=%d, display_str_len=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3407_112_2_18_2_45_6_78,(uint8*)"ddd",plmn_ptr, display_str, is_display_digit_format);
        return FALSE;
    }
 
    //SCI_TRACE_LOW:"MMIAPIPHONE_GenPLMNDisplay mcc=%d, mnc=%d, mnc_digit_num=%d, str_len=%d, is_display_digit_format=%d,rat=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3412_112_2_18_2_45_6_79,(uint8*)"dddddd",plmn_ptr->mcc, plmn_ptr->mnc, plmn_ptr->mnc_digit_num, *display_str_len, is_display_digit_format, rat);

    SCI_MEMSET(display_str, 0x00, (*display_str_len * sizeof(wchar)));

#ifdef CSC_XML_SUPPORT
    is_csc_name = MMIAPIPHONE_GetNetWorkNameByCSC(plmn_ptr->mcc, plmn_ptr->mnc, &network_str);

    if (!is_csc_name)
#endif
    {
        network_name_id = MMIAPIPHONE_GetNetWorkNameId(rat, plmn_ptr);

        MMI_GetLabelTextByLang(network_name_id, &network_str);
    }

    if (PNULL != network_str.wstr_ptr
        && 0 != network_str.wstr_len
        && !is_display_digit_format)
    {
        *display_str_len = MIN(*display_str_len, network_str.wstr_len);

        MMI_WSTRNCPY(display_str,
                                *display_str_len,
                                network_str.wstr_ptr,
                                *display_str_len,
                                *display_str_len);

        is_digit_output = FALSE;
    }
    else
    {
        uint8 temp_str[MAX_MCC_MNC_DIGIT_DISPLAY + 1] = {0};
        uint16 temp_str_len = 0;

        switch (plmn_ptr->mnc_digit_num)
        {
        case 2:
            sprintf((char*)temp_str, "%03d %02d", plmn_ptr->mcc, plmn_ptr->mnc);
            break;

        case 3:
            sprintf((char*)temp_str, "%03d %03d", plmn_ptr->mcc, plmn_ptr->mnc);
            break;

        default:
            sprintf((char*)temp_str, "%03d %d", plmn_ptr->mcc, plmn_ptr->mnc);
            break;
        }

        temp_str_len = strlen((char*)temp_str);

        *display_str_len = MIN(*display_str_len, temp_str_len);

        MMI_STRNTOWSTR(display_str,
                                *display_str_len,
                                temp_str,
                                *display_str_len,
                                *display_str_len);

        is_digit_output = TRUE;
    }

    //SCI_TRACE_LOW:"MMIAPIPHONE_GenPLMNDisplay is_digit_output=%d, network_name_id=%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3474_112_2_18_2_45_6_80,(uint8*)"dd", is_digit_output, network_name_id);

#ifdef CSC_XML_SUPPORT
    if (is_csc_name && PNULL != network_str.wstr_ptr)
    {
        SCI_FREE(network_str.wstr_ptr);
    }
#endif

    return is_digit_output;
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PLMN_LIST_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePreferRatRsp( 
                                        DPARAM  param
                                        )
{
    MnPreferRatRspS* prefer_rat_rsp = (MnPreferRatRspS*)param;
    MMI_RESULT_E        result = MMI_RESULT_TRUE;
    
    result = MMIAPISET_HandlePreferRatRspResult(prefer_rat_rsp->set_result);

    return result;
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PLMN_LIST_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePlmnListCnf(
                                     DPARAM              param
                                     )
{
    MnPlmnListCnfS      plmn_list_cnf = *((MnPlmnListCnfS *)param);
    MMI_RESULT_E        result = MMI_RESULT_TRUE;

    if(plmn_list_cnf.dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandlePlmnListCnf error dual_sys = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3508_112_2_18_2_45_7_81,(uint8*)"d", plmn_list_cnf.dual_sys);
        return MMI_RESULT_FALSE;
    }
    
    if ((MN_PHONE_OPERATE_SUCCESS == plmn_list_cnf.is_valid_result) &&
        (0 < plmn_list_cnf.plmn_detail_list.plmn_num))
    {
        MMIAPISET_OpenPlmnListWin(plmn_list_cnf.dual_sys, &(plmn_list_cnf.plmn_detail_list));
    }
#ifdef MMI_DUALMODE_ENABLE//@fen.xie MS00174570
    else if (MMIAPISET_IsPromptRetry()//单模下手动搜网且搜网前已经是无网络
        && (MN_PHONE_OPERATE_SUCCESS == plmn_list_cnf.is_valid_result) &&  (0 == plmn_list_cnf.plmn_detail_list.plmn_num)//结果为无网络
        )
    {
        //提示"切换到双模再试"
        MMIPUB_OpenAlertWarningWin( TXT_TO_DUAL_AND_TRY_LATER);
    }
#endif
    else//fail
    {
        MMIPUB_OpenAlertWarningWin( TXT_SYS_WAITING_AND_TRY_LATER);
    }
    
    //关闭网络连接窗口
    MMIAPISET_CloseConnectNetworkWin();
    
    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PS_POWER_OFF_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePowerPsOffCnf(
                                        DPARAM              param
                                        )
{
    MnPsPowerOffCnfS ps_power_off_cnf   = *((MnPsPowerOffCnfS *)param);
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    MMI_RESULT_E        result = MMI_RESULT_TRUE;

    dual_sys = ps_power_off_cnf.dual_sys;
    
    g_service_status[dual_sys].is_slecting = TRUE;
    MMIAPIPHONE_SetIsPsDeactiveCnf(dual_sys, TRUE);
    g_service_status[dual_sys].is_first_rssi = TRUE;
    g_service_status[dual_sys].rssiLevel = 0;
    MAIN_SetIdleGprsState(dual_sys, FALSE);//关闭gprs图标
    MAIN_SetIdleRxLevel(dual_sys, 0, TRUE);//信号显示0格
    MMIAPIPHONE_SetIsPsReady(dual_sys, FALSE);
    MAIN_SetIdleRoamingState(dual_sys, FALSE);
    MAIN_SetIdleForwardState(dual_sys, FALSE);
    MAIN_SetIdleGprsState(dual_sys, FALSE);
    MAIN_UpdateLineState(); //重新刷新line state的显示

    ImplementStandByModeAsync();
    
    if (MMIAPISTK_IsStkRefresh())
    {
        MMIAPIPHONE_PowerReset();
    }

    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_DEACTIVE_PS_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDeactivePsCnf(
                                        DPARAM              param
                                        )
{
    MnDeactivePSCnfS ps_deactive_cnf   = *((MnDeactivePSCnfS *)param);
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    MMI_RESULT_E        result = MMI_RESULT_TRUE;

    dual_sys = ps_deactive_cnf.dual_sys;
    
    g_service_status[dual_sys].is_slecting = TRUE;
    MMIPHONE_SetSimStatus(dual_sys, SIM_NOT_INITED);
    MMIAPIPHONE_SetIsPsDeactiveCnf(dual_sys, TRUE);
    g_service_status[dual_sys].is_first_rssi = TRUE;
    g_service_status[dual_sys].rssiLevel = 0;
    MAIN_SetIdleGprsState(dual_sys, FALSE);//关闭gprs图标
    MAIN_SetIdleRxLevel(dual_sys, 0, TRUE);//信号显示0格
    MMIAPIPHONE_SetIsPsReady(dual_sys, FALSE);
    MAIN_SetIdleRoamingState(dual_sys, FALSE);
    MAIN_SetIdleForwardState(dual_sys, FALSE);
    MAIN_SetIdleGprsState(dual_sys, FALSE);    
    MAIN_UpdateLineState();  //重新刷新line state的显示

    ImplementStandByModeAsync();

#ifdef MULTI_SIM_GSM_CTA_SUPPORT
    MMIAPISET_ImplementCTAStandbyModeAsync(dual_sys);
#endif
#ifdef MMI_SIM_LOCK_SUPPORT
        //deactive ps and normal init
        if(MMIPHONE_GetPhoneNeedRestart(ps_deactive_cnf.dual_sys))
        {
            MMIAPIPHONE_PowerOnSIM(ps_deactive_cnf.dual_sys);
            MMIPHONE_SetPhoneNeedRestart(ps_deactive_cnf.dual_sys,FALSE);
        }
#endif        
    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_POWER_ON_CNF
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePowerOnSimCnf(
                                       DPARAM              param
                                       )
{
    MnSimPowerOnCnfS power_on_sim_cnf = *((MnSimPowerOnCnfS *)param);
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;

#ifndef MMI_MULTI_SIM_SYS_SINGLE
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
#endif
    
    dual_sys = power_on_sim_cnf.dual_sys;
    
    //SCI_TRACE_LOW:"HandlePowerOnSimCnf:dual_sys=%d,is_ok=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3634_112_2_18_2_45_7_82,(uint8*)"dd", dual_sys, power_on_sim_cnf.is_ok);
    
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandlePowerOnSimCnf error dual_sys = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3638_112_2_18_2_45_7_83,(uint8*)"d", dual_sys);
        return MMI_RESULT_FALSE;
    }
    
    MMIAPIPHONE_SetSimExistedStatus(dual_sys, power_on_sim_cnf.is_ok);

    s_is_power_on_sim_cnf[dual_sys] = TRUE;
    
#ifdef MMI_MULTI_SIM_SYS_DUAL
    if (SIM_QUERY_STATUS_START == s_query_sim_status)
    {
        uint32 i = 0;
        BOOLEAN is_query_end = TRUE;
        
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
        {
            if(!s_is_power_on_sim_cnf[i])
            {
                is_query_end = FALSE;
                break;
            }
        }

        if (is_query_end)
        {
            s_query_sim_status = SIM_QUERY_STATUS_END;//设置查询结束
        }

        if (!g_is_sim_only_check_status[dual_sys])
        {
            if ((e_dualsys_setting & (0x01 << dual_sys))
                && MMIAPIPHONE_GetSimExistedStatus(dual_sys))
            {
                MMIAPIPHONE_PowerOnPs(dual_sys);
            }
        }
        else
        {
            MNPHONE_PowerOffSimEx(dual_sys);
        }
    }       
    
    if (SIM_QUERY_STATUS_END == s_query_sim_status && !g_is_sim_only_check_status[dual_sys])
    {
        uint32 i = 0;

        if (0 == MMIAPISET_GetMultiSysSettingNum(PNULL, 0))//无待机卡
        {
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
            {
                if (e_dualsys_setting & (0x01 << i))
                {
                    MMIAPIPHONE_PowerOnPs((MN_DUAL_SYS_E)i);
                    break;
                }
            }
        }
    
        MAIN_UpdateLineState(); //重新刷新line state的显示
        
        MMIAPISET_SetIsQueryCfu(FALSE,dual_sys);
        //MMIAPIPHONE_SetGPRSAttachMode();

        //卡1不存在不用排序
        if (!MMIAPIPHONE_GetSimExistedStatus(MN_DUAL_SYS_1))
        {
            MMIAPIPB_InitSimStatus(MN_DUAL_SYS_1);
        }
        
        //卡2不存在不用排序
        if (!MMIAPIPHONE_GetSimExistedStatus(MN_DUAL_SYS_2))
        {
            MMIAPIPB_InitSimStatus(MN_DUAL_SYS_2);
        }
        
        s_query_sim_status = SIM_QUERY_STATUS_NONE;

#if defined(MMI_MSD_SUPPORT)
#ifdef MMI_SIM_LOCK_SUPPORT
        if(!MMK_IsOpenWin(PHONE_SIM_LOCK_PASSWORD_INPUTING_WIN_ID)
            &&!MMK_IsOpenWin(PHONE_SIM_LOCK_COUNTDOWN_WIN_ID)
            &&!MMK_IsOpenWin(PHONE_SIM_LOCK_ALERT_WIN_ID))
#endif	
        {
            MMIAPIMSD_SimLegalityCheck();//待机确认后验证防盗密码
        }
#endif//MMI_MSD_SUPPORT
    }    
#elif defined(MMI_MULTI_SIM_SYS_TRI) || defined(MMI_MULTI_SIM_SYS_QUAD)
    if (SIM_QUERY_STATUS_START == s_query_sim_status)
    {
        uint32 i = 0;
        BOOLEAN is_query_end = TRUE;
        
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
        {
            if(!s_is_power_on_sim_cnf[i])
            {
                is_query_end = FALSE;
                break;
            }
        }

        if (is_query_end)
        {
            s_query_sim_status = SIM_QUERY_STATUS_END;//设置查询结束
        }
        
        if (!g_is_sim_only_check_status[dual_sys])
        {
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
            {
                if ((dual_sys == i) && (e_dualsys_setting & (0x01 << i))
                    && MMIAPIPHONE_GetSimExistedStatus(i))
                {
                    if (MMIAPIIDLE_IdleWinIsOpen())
                    {
                        MMIAPIPHONE_PowerOnPs(dual_sys);
                    }
                    else
                    {
                        //SCI_TRACE_LOW:"HandlePowerOnSimCnf: Idle is not open"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3759_112_2_18_2_45_7_84,(uint8*)"");
                    }
                    break;
                }
            }
        }
        else
        {
            MNPHONE_PowerOffSimEx(dual_sys);
        }
    }       

    if (SIM_QUERY_STATUS_END == s_query_sim_status && !g_is_sim_only_check_status[dual_sys])
    {
        uint32 i = 0;

        if (0 == MMIAPISET_GetMultiSysSettingNum(PNULL, 0))//无待机卡
        {
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
            {
                if (e_dualsys_setting & (0x01 << i))
                {
                    MMIAPIPHONE_PowerOnPs((MN_DUAL_SYS_E)i);
                    break;
                }
            }
        }

        MAIN_UpdateLineState();
        
        MMIAPISET_SetIsQueryCfu(FALSE,dual_sys);
        //MMIAPIPHONE_SetGPRSAttachMode();

        //卡不存在不用排序
        for (i = 0; i < MMI_DUAL_SYS_MAX; i ++)
        {
            if (!MMIAPIPHONE_GetSimExistedStatus(i))
            {
                MMIAPIPB_InitSimStatus(i);
            }
        }
        s_query_sim_status = SIM_QUERY_STATUS_NONE;

#if defined(MMI_MSD_SUPPORT)
#ifdef MMI_SIM_LOCK_SUPPORT
        if(!MMK_IsOpenWin(PHONE_SIM_LOCK_PASSWORD_INPUTING_WIN_ID)
            &&!MMK_IsOpenWin(PHONE_SIM_LOCK_COUNTDOWN_WIN_ID)
            &&!MMK_IsOpenWin(PHONE_SIM_LOCK_ALERT_WIN_ID))
#endif	
        {
            MMIAPIMSD_SimLegalityCheck();//待机确认后验证防盗密码
        }
#endif//MMI_MSD_SUPPORT
    }    
#else
    MMIAPIPHONE_StartUpPsAccordingToSetting(FALSE, TRUE);

#if defined(MMI_MSD_SUPPORT)
#ifdef MMI_SIM_LOCK_SUPPORT
    if(!MMK_IsOpenWin(PHONE_SIM_LOCK_PASSWORD_INPUTING_WIN_ID)
            &&!MMK_IsOpenWin(PHONE_SIM_LOCK_COUNTDOWN_WIN_ID)
            &&!MMK_IsOpenWin(PHONE_SIM_LOCK_ALERT_WIN_ID))
#endif
    {
        MMIAPIMSD_SimLegalityCheck();
    }
#endif//MMI_MSD_SUPPORT
#endif

#ifdef MMI_SIM_LANGUAGE_SUPPORT
    //只做某一张SIM卡的优选语言的配置，其他的sim 卡不做配置。
    if(( !s_is_language_auto_set )&&( power_on_sim_cnf.is_ok ))
    {
        MMISET_LANGUAGE_TYPE_E nv_language_type = (MMISET_LANGUAGE_TYPE_E)MMIAPISET_GetNVLanguageType();

        //语言是否需要优选配置。
        if(MMISET_LANGUAGE_AUTO == nv_language_type ) 
        {
            MMISET_LANGUAGE_TYPE_E prefer_language = {0};
            BOOLEAN is_prefer_set = MMIAPISET_GetPreferLanguageTypeFromSim(dual_sys, &prefer_language);

            //SCI_TRACE_LOW:"HandlePowerOnSimCnf,dual_sys=%d,is_prefer_set=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3840_112_2_18_2_45_7_85,(uint8*)"dd",dual_sys,is_prefer_set);

            if( is_prefer_set )
            {
                s_is_language_auto_set = TRUE; //标记某张sim卡成功做了优选语言
                
                s_prefer_language_sim = dual_sys; //记录当前优选语言采用的SIM卡。

                //做系统资源的加载和设置。   
                MMIAPISET_SetLanguageType(nv_language_type);    
            }
        }
    }
#endif

    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_POWER_OFF_CNF
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePowerOffSimCnf(
                                       DPARAM              param
                                       )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MnSimPowerOffCnfS power_off_sim_cnf = *((MnSimPowerOffCnfS *)param);
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    uint32 i = 0;
    BOOLEAN is_sim_status_checked = TRUE;

    dual_sys = power_off_sim_cnf.dual_sys;

    //SCI_TRACE_LOW:"HandlePowerOffSimCnf:dual_sys=%d,is_ok=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3874_112_2_18_2_45_7_86,(uint8*)"dd", dual_sys, power_off_sim_cnf.is_ok);

    g_is_sim_only_check_status[dual_sys] = FALSE;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (g_is_sim_only_check_status[i])
        {
            is_sim_status_checked = FALSE;
            break;
        }
    }

#ifdef MULTI_SIM_GSM_CTA_SUPPORT
    if (is_sim_status_checked)
    {
        MMIAPISET_OpenCTAMultiSysStandbyWin(TRUE);
    }
#else
    MMIAPIPHONE_SetSimExistedStatus(dual_sys, FALSE);
    MMIPHONE_SetSimStatus(dual_sys, SIM_NOT_INITED);
#endif

    return result;
}

#ifdef MMI_PREFER_PLMN_SUPPORT
/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PREFER_PLMN_CHANGE_CNF
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePreferPlmnChangeCnf(
                                       DPARAM              param
                                       )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MnPreferPlmnChangeCnfS prefer_plmn_sim_cnf = *((MnPreferPlmnChangeCnfS *)param);
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;

    dual_sys = prefer_plmn_sim_cnf.dual_sys;
   
    //SCI_TRACE_LOW:"HandlePowerOffSimCnf:dual_sys=%d,is_ok=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3915_112_2_18_2_45_7_87,(uint8*)"dd", dual_sys, prefer_plmn_sim_cnf.is_ok);

    MMIAPISET_HandlePreferNetworkListCnf(prefer_plmn_sim_cnf);
    
    return result;
}
#endif

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_PLMN_SELECT_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectPlmnCnf(
                                       DPARAM              param
                                       )
{
    MnPlmnSelectCnfS    plmn_select_cnf = *((MnPlmnSelectCnfS *)param);
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    MN_DUAL_SYS_E   dual_sys = MN_DUAL_SYS_1;
    
    dual_sys = plmn_select_cnf.dual_sys;
    
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandleSelectPlmnCnf error dual_sys = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3939_112_2_18_2_45_7_88,(uint8*)"d", dual_sys);
        return MMI_RESULT_FALSE;
    }

    if (plmn_select_cnf.plmn_is_selected)
    {
        //success!
        // 保存网络状态信息
        g_service_status[dual_sys].plmn_status = plmn_select_cnf.plmn_status;
        g_service_status[dual_sys].lac         = plmn_select_cnf.lac;
        g_service_status[dual_sys].plmn.mcc         = plmn_select_cnf.mcc;
        g_service_status[dual_sys].plmn.mnc         = plmn_select_cnf.mnc;
        g_service_status[dual_sys].plmn.mnc_digit_num = plmn_select_cnf.mnc_digit_num;
        g_service_status[dual_sys].cell_id     = plmn_select_cnf.cell_id;
           
        MMIPUB_OpenAlertSuccessWin(TXT_PLMN_SELECT_COMPLETE);

        //关闭网络列表窗口
        MMIAPISET_ClosePlmnListWin();
    }
    else
    {
        MMIPUB_OpenAlertWarningWin(TXT_PLMN_SELECT_FAIL);
    }
    
    //关闭网络连接窗口
    MMIAPISET_CloseConnectNetworkWin();
    
    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SET_BAND_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectBandCnf(
                                       DPARAM              param
                                       )
{
    MnBandSelectCnfS    band_select_cnf = *((MnBandSelectCnfS *)param);
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    
    MMIPUB_CloseAlertWin();
    if (!MMIAPIENG_IsBandSelectWinOpen())
    {
        //SCI_TRACE_LOW:"HandleSelectBandCnf band_select_cnf.is_ok = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_3984_112_2_18_2_45_8_89,(uint8*)"d",band_select_cnf.is_ok);
        if (band_select_cnf.is_ok)
        {
            //success!
            MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_SYS_WAITING_AND_TRY_LATER);
        }
        
        //关闭网络连接窗口
        MMIAPISET_CloseConnectNetworkWin();
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : get the network name
//	Global resource dependence : 
//  Author: Great.Tian
//	Note:
/*****************************************************************************/
LOCAL MMI_TEXT_ID_T GetNetWorkNameId(
                                     MN_PLMN_T *plmn_ptr,
                                     MN_PHONE_PLMN_STATUS_E  plmn_status,
                                     BOOLEAN                 is_slecting
                                     )
{
    MMI_TEXT_ID_T   network_name_id = TXT_NULL;

    if(PNULL == plmn_ptr)
    {
        //SCI_TRACE_LOW:"GetNetWorkNameId: the network plmn_ptr is error! %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_4016_112_2_18_2_45_8_90,(uint8*)"d",plmn_ptr);
        return  TXT_NULL;
    }
    
    if (is_slecting)
    {
        network_name_id = TXT_PROCESS_SEARCHING_NETWORK;
    }
    else
    {
        switch(plmn_status)
        {
        case PLMN_NO_SERVICE:
            network_name_id = TXT_NO_SERVICE;
            break;
            
        case PLMN_NORMAL_GPRS_ONLY:
        case PLMN_EMERGENCY_ONLY:
        case PLMN_EMERGENCY_SIM_INVALID:
        case PLMN_EMERGENCY_GPRS_ONLY:
        case PLMN_EMERGENCY_SIM_INVALID_GPRS_ONLY:
        case PLMN_FULL_PS_SERVICE:
            network_name_id = TXT_KL_PSW_EMERGENCY_CALL;
            break;
            
        case PLMN_REGISTER_GPRS_ONLY:
        case PLMN_REGISTER_SERVICE:    
            network_name_id = TXT_REGISTER_SERVICE;
            break;
            
        case PLMN_NORMAL_GSM_ONLY:
        case PLMN_NORMAL_GSM_GPRS_BOTH:
            network_name_id = MMIAPIPHONE_GetNetWorkNameId(MMIAPIPHONE_GetTDOrGsm(), plmn_ptr);

            if (TXT_NULL == network_name_id)
            {
                network_name_id = TXT_FULL_SERVICE;
            }
            break;
            
        default:
            //SCI_TRACE_LOW:"GetNetWorkNameId: the network status is error! %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_4057_112_2_18_2_45_8_91,(uint8*)"d", plmn_status);
            network_name_id = TXT_KL_PSW_EMERGENCY_CALL;
            break;
        }
    }
    
    return (network_name_id);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_GET_PIN_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPinInd(
                                   DPARAM              param
                                   )
{    
    MnSimGetPinIndS         sim_get_pin_ind = *((MnSimGetPinIndS *)param);
    MMI_RESULT_E            result = MMI_RESULT_TRUE;
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
#ifndef MMI_MULTI_SIM_SYS_SINGLE
    uint32 i = 0;
    BOOLEAN is_need_handle = FALSE;
#endif

    if(sim_get_pin_ind.dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandleSimPinInd:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_4084_112_2_18_2_45_8_92,(uint8*)"d", sim_get_pin_ind.dual_sys);
        return MMI_RESULT_FALSE;
    }

    if (e_dualsys_setting & (0x01 << sim_get_pin_ind.dual_sys))
    {
        PowerOnPsForEmerge(sim_get_pin_ind.dual_sys);
    }

#ifndef MMI_MULTI_SIM_SYS_SINGLE
    s_handle_pin_status.sim_pin_ind[sim_get_pin_ind.dual_sys] = sim_get_pin_ind;

    for(i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if(( i != sim_get_pin_ind.dual_sys)&&(s_handle_pin_status.is_handling_pin[i]))
        {
            is_need_handle = TRUE;
            break;            
        }
    }
    
    if(is_need_handle)
    {
        s_handle_pin_status.need_handle_pin[sim_get_pin_ind.dual_sys] = TRUE;
		
        return result;
    }
    else
    {            
        s_handle_pin_status.is_handling_pin[sim_get_pin_ind.dual_sys] = TRUE;   //set the status (lock)
        s_handle_pin_status.cur_handling_sim_id = sim_get_pin_ind.dual_sys;
    }    
#endif
    
    CreateInputPinWindow(sim_get_pin_ind);

    //MMIAPIPHONE_SetGPRSAttachMode();
    
    return result;
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_PIN_FUNC_CNF
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPinCnf(
                                   DPARAM              param
                                   )
{
    MnSimPinFuncCnfS        sim_pin_cnf = *((MnSimPinFuncCnfS *)param);
    MMI_WIN_ID_T         window_id = MMK_GetFocusWinId();
    MMI_RESULT_E            result = MMI_RESULT_TRUE;
    
    //cr17666,请稍候界面来闹钟，接收到ps消息仍需要处理
    if (MMK_IsOpenWin(PHONE_PIN_WAIT_WIN_ID))
    {
        window_id = PHONE_PIN_WAIT_WIN_ID;
    }
    else if (MMK_IsOpenWin(IDLE_SPECIAL_WAIT_WIN_ID))
    {
        window_id = IDLE_SPECIAL_WAIT_WIN_ID;
    }
    
    switch (window_id)
    {
    case PHONE_PIN_WAIT_WIN_ID:
        if ((sim_pin_cnf.pin_operate_result) == MNSIM_PIN_REQ_OK)
        {
            if (0 != g_operate_pin_win_tab)
            {
                MMIAPISET_OpenMsgBox(g_operate_pin_win_tab, g_is_operate_pin_special_func);
            }
            
            if (MMK_IsOpenWin(MMI_PIN_INPUTING_WIN_ID) && (MNSIM_PIN_CHANGE != sim_pin_cnf.pin_function_type)) //记录下正确的pin码
            {
                MMI_STRING_T   pin_value = {0};   
                GUIEDIT_GetString(MMI_PIN_INPUT_EDITBOX_CTRL_ID, &pin_value);
                g_pin_value[sim_pin_cnf.dual_sys].blocklen = pin_value.wstr_len;

                MMI_WSTRNTOSTR(
                    g_pin_value[sim_pin_cnf.dual_sys].blockbuf,
                    MN_MAX_BLOCK_LEN,
                    pin_value.wstr_ptr,
                    g_pin_value[sim_pin_cnf.dual_sys].blocklen,
                    g_pin_value[sim_pin_cnf.dual_sys].blocklen
                    );
            }
            
            MMK_CloseWin(MMI_PIN_INPUTING_WIN_ID);
            MMK_CloseWin(MMI_PUK_INPUTING_WIN_ID);

            if (!g_is_operate_pin_special_func || 0 == g_operate_pin_win_tab)
            {
                // 关闭等待窗口
                MMK_CloseWin(PHONE_PIN_WAIT_WIN_ID);
            }
        }
        else
        {
            if (sim_pin_cnf.pin_status.pin_blocked) //PIN被锁,要求输入PUK
            {
                if (MMK_IsOpenWin(MMI_PUK_INPUTING_WIN_ID))
                {
                    if (MNSIM_PIN1 == sim_pin_cnf.pin_num)//PIN1
                    {
                        if (sim_pin_cnf.unblock_pin_status.pin_blocked)
                        {
                            //提示错误窗口,PUK错被锁!
                            MMIPUB_OpenAlertWarningWin(TXT_PUK_BLOCKED);

                            MMK_CloseWin(MMI_PUK_INPUTING_WIN_ID);
                        }
                        else
                        {
                            //提示错误窗口,PUK错误!
                            MMIPUB_OpenAlertWarningWin(TXT_PUK_ERROR);

                            //清空password
                            GUIEDIT_ClearAllStr(MMI_PUK_INPUT_EDITBOX_CTRL_ID);
                        }
                    }
                    else//PIN2
                    {
                        if (sim_pin_cnf.unblock_pin_status.pin_blocked)
                        {
                            //提示错误窗口,PUK2被锁!
                            MMIPUB_OpenAlertWarningWin(TXT_PUK2_BLOCKED);

                            MMK_CloseWin(MMI_PUK_INPUTING_WIN_ID);
                        }
                        else
                        {
                            //提示错误窗口,PUK2错误!
                            MMIPUB_OpenAlertWarningWin(TXT_PUK2_ERROR);

                            //清空password
                            GUIEDIT_ClearAllStr(MMI_PUK_INPUT_EDITBOX_CTRL_ID);
                        }
                    }
                }
                else
                {
                    MMI_OPERATE_PIN_T   *operate_pin_ptr = (MMI_OPERATE_PIN_T *)MMK_GetWinAddDataPtr(MMI_PIN_INPUTING_WIN_ID);

                    SCI_MEMSET(&s_operate_pin_blocked_info,0,sizeof(MMI_OPERATE_PIN_T));
                    SCI_MEMCPY(&s_operate_pin_blocked_info,operate_pin_ptr,sizeof(MMI_OPERATE_PIN_T));

                    if (MNSIM_PIN1 == sim_pin_cnf.pin_num)//PIN1
                    {
                        //提示错误窗口,PIN被锁!
                        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PIN_BLOCKED,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,HandlePinBlockedExceptPhone);
                    }
                    else//PIN2
                    {
                        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PIN2_BLOCKED,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,HandlePinBlockedExceptPhone);
                    }
                }
            }
            else
            {
                if (MNSIM_PIN1 == sim_pin_cnf.pin_num)//PIN1
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PIN_ERROR);
                }
                else
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PIN2_ERROR);
                }

                if (MMK_IsOpenWin(MMI_PIN_INPUTING_WIN_ID))
                {
                    //清空password
                    GUIEDIT_ClearAllStr(MMI_PIN_INPUT_EDITBOX_CTRL_ID);
                }
            }

            // 关闭等待窗口
            MMK_CloseWin(PHONE_PIN_WAIT_WIN_ID);
        }
        break;
        
    case IDLE_SPECIAL_WAIT_WIN_ID://idle win,input expecial string
        //SCI_TRACE_LOW:"HandleSimPinCnf:pin_operate_result=%d, pin_num=%d, pin_function_type=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_4267_112_2_18_2_45_8_93,(uint8*)"ddd",sim_pin_cnf.pin_operate_result, sim_pin_cnf.pin_num, sim_pin_cnf.pin_function_type);

        switch (sim_pin_cnf.pin_operate_result)
        {
        case MNSIM_PIN_REQ_OK:
            if (MNSIM_PIN_UNBLOCK == sim_pin_cnf.pin_function_type)
            {
                MMIPUB_OpenAlertSuccessWin(TXT_CODE_ACCEPT);
            }
            else
            {
                if (MNSIM_PIN1 == sim_pin_cnf.pin_num)
                {
                    MMIPUB_OpenAlertSuccessWin(TXT_PIN_CHANGED);
                }
                else
                {
                    MMIPUB_OpenAlertSuccessWin(TXT_PIN2_CHANGED);
                }
            }  
            break;

        case MNSIM_PIN_ERROR_WITH_BLOCKED:
            if (MNSIM_PIN1 == sim_pin_cnf.pin_num)
            {
                if (sim_pin_cnf.unblock_pin_status.pin_blocked)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PUK_BLOCKED);
                }
                else
                {
                    SCI_MEMSET(&s_operate_pin_blocked_info,0,sizeof(MMI_OPERATE_PIN_T));
                    s_operate_pin_blocked_info.operate_type = sim_pin_cnf.pin_function_type;
                    s_operate_pin_blocked_info.pin_num = sim_pin_cnf.pin_num;

                    MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PIN_BLOCKED,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,HandlePinBlockedExceptPhone);
                }
            }
            else
            {
                if (sim_pin_cnf.unblock_pin_status.pin_blocked)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PUK2_BLOCKED);
                }
                else
                {
                    SCI_MEMSET(&s_operate_pin_blocked_info,0,sizeof(MMI_OPERATE_PIN_T));
                    s_operate_pin_blocked_info.operate_type = sim_pin_cnf.pin_function_type;
                    s_operate_pin_blocked_info.pin_num = sim_pin_cnf.pin_num;

                    MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PIN2_BLOCKED,TXT_NULL,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,HandlePinBlockedExceptPhone);
                }
            }
            break;

        case MNSIM_PIN_ERROR_CONTRADICT_STATUS:
            if (MNSIM_PIN1 == sim_pin_cnf.pin_num)
            {
                MMIPUB_OpenAlertWarningWin(TXT_ENABLE_PIN1);
            }
            else
            {
                MMIPUB_OpenAlertWarningWin(TXT_ENABLE_PIN2);
            }
            break;

        case MNSIM_PIN_ERROR_PIN_FAIL:
            if (MNSIM_PIN1 == sim_pin_cnf.pin_num)
            {
                if (MNSIM_PIN_UNBLOCK == sim_pin_cnf.pin_function_type)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PUK_ERROR);
                }
                else
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PIN_ERROR);
                }
            }
            else
            {
                if (MNSIM_PIN_UNBLOCK == sim_pin_cnf.pin_function_type)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PUK2_ERROR);
                }
                else
                {
                    MMIPUB_OpenAlertWarningWin(TXT_PIN2_ERROR);
                }
            }
            break;

        default:
            MMIPUB_OpenAlertWarningWin(TXT_ERROR);
            break;
        }
        // 关闭等待窗口
        MMK_CloseWin(IDLE_SPECIAL_WAIT_WIN_ID);
        // close idle dialing win
        MMIAPIIDLE_CloseDialWin();
        break;
        
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : 在提示还可以 输入多少次密码后，启动pin输入框
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePin1Maxtimes(
                                      MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                      MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                      DPARAM 		        param		// 相应消息的参数
                                      )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    switch (msg_id)
    {
    case MSG_CLOSE_WINDOW:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        // close input pin window
        MMK_CloseWin(PHONE_PIN_INPUT_WIN_ID);
        
        // 打开输入PIN的界面
        MMK_CreatePubEditWin((uint32*)PHONE_PIN_INPUTING_WIN_TAB,PNULL);
        break;
        
    case MSG_APP_CANCEL:
    case MSG_APP_RED:
        MMK_CloseWin(win_id);
        break;

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone.        
    case MSG_APP_FLIP:
        MMIDEFAULT_HandleFlipKey(FALSE);
        break;
#endif

    case MSG_LOSE_FOCUS:
        break;
        
    default:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}


/*****************************************************************************/
// 	Description : 在pin blocked的情况下关闭窗口，则启动输入puk的窗口
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePinBlocked(
                                    MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                    MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                    DPARAM 		        param		// 相应消息的参数
                                    )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    GUI_POINT_T         dis_point = {0};
    GUI_LCD_DEV_INFO	lcd_dev_info = {0};
    
    lcd_dev_info.lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id = GUI_BLOCK_MAIN;
    
    switch (msg_id)
    {
    case MSG_CLOSE_WINDOW:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        // close input pin window
        MMK_CloseWin(PHONE_PIN_INPUT_WIN_ID);

        MMK_CloseWin(PHONE_PUK_INPUT_WIN_ID);
        // 打开输入PUK的界面
        MMK_CreatePubEditWin((uint32*)PHONE_PUK_INPUTING_WIN_TAB,PNULL);
        break;

    case MSG_APP_CANCEL://pin被锁输入界面不允许返回
    case MSG_APP_RED:
        MMK_CloseWin(win_id);
        break;

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone.        
    case MSG_APP_FLIP:
        MMK_CloseWin(win_id);
        MMIDEFAULT_HandleFlipKey(FALSE);
        break;
#endif

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        GUIRES_DisplayImg(&dis_point,
            PNULL,
            PNULL,
            win_id,
            IMAGE_COMMON_BG,
            &lcd_dev_info);
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
        
    default:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : 在pin1或者pin2 blocked的情况下关闭窗口，则启动输入puk的窗口
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePinBlockedExceptPhone(
                                               MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                               MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                               DPARAM 		        param		// 相应消息的参数
                                               )
{
    MMI_OPERATE_PIN_T   *operate_pin_ptr = &s_operate_pin_blocked_info;
    MMI_RESULT_E        result = MMI_RESULT_TRUE;
    
    switch (msg_id)
    {
    case MSG_CLOSE_WINDOW:
        {
            MMI_OPERATE_PIN_T   *new_opreate_pin_ptr = SCI_ALLOC_APP(sizeof(MMI_OPERATE_PIN_T));
            
            MMI_MEMCPY(new_opreate_pin_ptr,
                sizeof(MMI_OPERATE_PIN_T),
                operate_pin_ptr,
                sizeof(MMI_OPERATE_PIN_T),
                sizeof(MMI_OPERATE_PIN_T));
            
            result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
            
            new_opreate_pin_ptr->operate_type = MNSIM_PIN_UNBLOCK;
            // 打开输入PUK的界面
            MMK_CreatePubEditWin((uint32*)MMI_PUK_INPUTING_WIN_TAB,(ADD_DATA)new_opreate_pin_ptr);
            
            // close input pin window
            MMK_CloseWin(MMI_PIN_INPUTING_WIN_ID);
        }
        break;
        
    case MSG_APP_CANCEL://pin被锁输入界面不允许返回
    case MSG_APP_RED:
        MMK_CloseWin(win_id);
        break;

    case MSG_LOSE_FOCUS:
        break;

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone.        
    case MSG_APP_FLIP:
        MMK_CloseWin(win_id);
        MMIDEFAULT_HandleFlipKey(FALSE);
        break;
#endif        
        
    default:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_READY_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimReadyInd(
                                     DPARAM              param
                                     )
{
    MnSimReadyIndS      sim_ready_ind   = *((MnSimReadyIndS *)param);
    MMI_WIN_ID_T     window_id       = MMK_GetFocusWinId();
    MMI_RESULT_E        result = MMI_RESULT_TRUE;
    uint32 i = 0;
    BOOLEAN is_handling_pin = FALSE;
    MMI_STRING_T   pin_value = {0};   

    if (sim_ready_ind.dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return result;
    }

    // 保存 SIM 状态信息
    StoreSimReadyStatus(&sim_ready_ind);
    
    MMIAPIPHONE_SetSimExistedStatus(sim_ready_ind.dual_sys, TRUE);

#ifdef MMI_GPRS_SUPPORT
    MMIAPICONNECTION_AutoAdapting(sim_ready_ind.dual_sys);//
#endif
#ifdef MMI_NETWORK_NAME_SUPPORT
    //get PNN num when SIM ready
    MMIPHONE_GetPNNum(sim_ready_ind.dual_sys);
#endif
    
    if ( (MMK_IsOpenWin(PHONE_PIN_WAIT_WIN_ID)
        || MMK_IsOpenWin(PHONE_PIN_ALERT_WIN_ID)
        || MMK_IsOpenWin(PHONE_PIN_INPUT_WIN_ID)
        || MMK_IsOpenWin(PHONE_NEW_PIN_INPUT_WIN_ID)
        || MMK_IsOpenWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID)
        || MMK_IsOpenWin(PHONE_PUK_INPUT_WIN_ID))
        && s_handle_pin_status.cur_handling_sim_id == sim_ready_ind.dual_sys)
    {
        window_id = PHONE_PIN_WAIT_WIN_ID;
        
        if (MMK_IsOpenWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID))//记录下正确的pin码
        {
            GUIEDIT_GetString(PHONE_NEW_PIN_AGAIN_EDITBOX_CTRL_ID, &pin_value);
            g_pin_value[sim_ready_ind.dual_sys].blocklen = pin_value.wstr_len;

            MMI_WSTRNTOSTR(
                g_pin_value[sim_ready_ind.dual_sys].blockbuf,
                MN_MAX_BLOCK_LEN,
                pin_value.wstr_ptr,
                g_pin_value[sim_ready_ind.dual_sys].blocklen,
                g_pin_value[sim_ready_ind.dual_sys].blocklen
                );
        }
        else if (MMK_IsOpenWin(PHONE_PIN_INPUT_WIN_ID)) //记录下正确的pin码
        {
            GUIEDIT_GetString(PHONE_PIN_EDITBOX_CTRL_ID, &pin_value);
            g_pin_value[sim_ready_ind.dual_sys].blocklen = pin_value.wstr_len;

            MMI_WSTRNTOSTR(
                g_pin_value[sim_ready_ind.dual_sys].blockbuf,
                MN_MAX_BLOCK_LEN,
                pin_value.wstr_ptr,
                g_pin_value[sim_ready_ind.dual_sys].blocklen,
                g_pin_value[sim_ready_ind.dual_sys].blocklen
                );
        }
        
        if (MMIPHONE_PS_POWER_ON_NORMAL != s_ps_ready[sim_ready_ind.dual_sys])
        {
            g_service_status[sim_ready_ind.dual_sys].is_slecting = TRUE;
        }

        //关闭pin和puk输入窗口
        MMK_CloseWin(PHONE_PIN_ALERT_WIN_ID);
        MMK_CloseWin(PHONE_PIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_NEW_PIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_PUK_INPUT_WIN_ID);
    }
    else if (MMK_IsOpenWin(IDLE_SPECIAL_WAIT_WIN_ID))
    {
        window_id = IDLE_SPECIAL_WAIT_WIN_ID;
    }
        
    //add for dual sys
    s_handle_pin_status.need_handle_pin[sim_ready_ind.dual_sys] = FALSE;
    s_handle_pin_status.is_handling_pin[sim_ready_ind.dual_sys] = FALSE;

    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (s_handle_pin_status.is_handling_pin[i])
        {
            is_handling_pin = TRUE;
            break;
        }
    }
    
    if (!is_handling_pin)
    {
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            if (i != sim_ready_ind.dual_sys)
            {
                if (s_handle_pin_status.need_handle_pin[i])
                {
                    //set s_handle_pin_status
                    s_handle_pin_status.cur_handling_sim_id = (MN_DUAL_SYS_E)i;
                    s_handle_pin_status.is_handling_pin[i] = TRUE;
                    
                    //create input pin window
                    CreateInputPinWindow(s_handle_pin_status.sim_pin_ind[i]);
                    return result;
                }
            }
        }
    }
    
    // 如果在输入PIN或者PUK之后，收到该消息
    switch (window_id)
    {
    case PHONE_PIN_WAIT_WIN_ID:
        // 关闭等待窗口        
        if (s_handle_pin_status.cur_handling_sim_id == sim_ready_ind.dual_sys)
        {
            MMK_CloseWin(PHONE_PIN_WAIT_WIN_ID);
        }
        break;

    case IDLE_SPECIAL_WAIT_WIN_ID:
        break;
        
    default:
        //关闭pin和puk输入窗口
        if (s_handle_pin_status.cur_handling_sim_id == sim_ready_ind.dual_sys)
        {
            if (MMIPHONE_PS_POWER_ON_NORMAL != s_ps_ready[sim_ready_ind.dual_sys])
            {
                g_service_status[sim_ready_ind.dual_sys].is_slecting = TRUE;
            }

            MMK_CloseWin(PHONE_PIN_ALERT_WIN_ID);   
            MMK_CloseWin(PHONE_PIN_INPUT_WIN_ID);
            MMK_CloseWin(PHONE_NEW_PIN_INPUT_WIN_ID);
            MMK_CloseWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID);
            MMK_CloseWin(PHONE_PUK_INPUT_WIN_ID);
            MMK_CloseWin(PHONE_PIN_WAIT_WIN_ID);
        }
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : 保存 SIM 状态
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StoreSimReadyStatus(
                               MnSimReadyIndS  *sim_status_ptr
                               )
{
    MN_IMSI_T imsi[MMI_DUAL_SYS_MAX] = {0};
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    MN_DUAL_SYS_E   dual_sys = MN_DUAL_SYS_1;
    
    dual_sys = sim_status_ptr->dual_sys;
    
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"StoreSimReadyStatus:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_4715_112_2_18_2_45_9_94,(uint8*)"d", dual_sys);
        return;
    }    
    MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_OK);
    s_sim_status[dual_sys].pin1_is_enabled               = sim_status_ptr->pin1_is_enabled ; 	
    s_sim_status[dual_sys].pin1_is_blocked               = sim_status_ptr->pin1_is_blocked;		
    s_sim_status[dual_sys].pin1_status                   = sim_status_ptr->pin1_status;         
    s_sim_status[dual_sys].pin2_status                   = sim_status_ptr->pin2_status;         
    s_sim_status[dual_sys].unblock_pin1_status           = sim_status_ptr->unblock_pin1_status;	
    s_sim_status[dual_sys].unblock_pin2_Status           = sim_status_ptr->unblock_pin2_Status;	
    s_sim_status[dual_sys].is_pin2_required_acm_max      = sim_status_ptr->is_pin2_required_acm_max; 
    s_sim_status[dual_sys].is_pin2_Required_acm          = sim_status_ptr->is_pin2_Required_acm;	 
    s_sim_status[dual_sys].is_pin2_Required_puct         = sim_status_ptr->is_pin2_Required_puct;	 
    s_sim_status[dual_sys].is_aoc_support         = sim_status_ptr->AOC_is_support;	 
    s_sim_status[dual_sys].imsi                          = sim_status_ptr->imsi;			    
    s_sim_status[dual_sys].hplmn                         = sim_status_ptr->hplmn;
    s_sim_status[dual_sys].location_info                 = sim_status_ptr->location_info;		
    s_sim_status[dual_sys].phase                         = sim_status_ptr->phase;			    
    s_sim_status[dual_sys].sim_service                   = sim_status_ptr->sim_service;		    
    s_sim_status[dual_sys].admin                         = sim_status_ptr->admin;   

    //get imsi from nv
    MMINV_READ(MMINV_PHONE_IMSI,imsi,return_value);
    if (MN_RETURN_SUCCESS != return_value)
    {
        SCI_MEMSET(imsi,0,sizeof(imsi));
        MMINV_WRITE(MMINV_PHONE_IMSI,imsi);
    }
    
    //the current sim card is same to the last sim card
    if ((imsi[dual_sys].imsi_len == s_sim_status[dual_sys].imsi.imsi_len) &&
        (0 == memcmp(imsi[dual_sys].imsi_val, s_sim_status[dual_sys].imsi.imsi_val,imsi[dual_sys].imsi_len)))
    {
        s_is_same_sim[dual_sys] = TRUE;
    }
    else
    {
        SCI_MEMSET(&(imsi[dual_sys]), 0x00, sizeof(imsi[dual_sys]));
        SCI_MEMCPY(&(imsi[dual_sys]), &(s_sim_status[dual_sys].imsi), sizeof(imsi[dual_sys]));
        MMINV_WRITE(MMINV_PHONE_IMSI, imsi);
        s_is_same_sim[dual_sys] = FALSE;
    }
#if 0 //def WIN32
    {
        MN_PLMN_T plmn_list[] = 
        {
            {460, 0, 2},
            {460, 0, 2},
            {460, 0, 2},
            {460, 1, 2},
        };
        
        s_sim_status[dual_sys].hplmn = plmn_list[dual_sys];
    }
#endif
}

/*****************************************************************************/
// 	Description : 保存 SIM  not ready 状态
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL void StoreSimNotReadyStatus(
                                  MnSimNotReadyIndS  sim_status_ptr
                                  )
{
    MN_DUAL_SYS_E   dual_sys = MN_DUAL_SYS_1;
    
    dual_sys = sim_status_ptr.dual_sys;
    
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"StoreSimNotReadyStatus:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_4786_112_2_18_2_45_9_95,(uint8*)"d", dual_sys);
        return;
    }
    
    switch (sim_status_ptr.reason)
    {
    case MNSIM_ERROR_NO_SECRETCODE_INITIALIZED:
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_REJECTED);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, TRUE);
        break;
        
    case MNSIM_ERROR_REJECT_BY_NET:
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_REGISTRATION_FAILED);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, TRUE);
        break;
        
    case MNSIM_ERROR_PIN1_BLOCKED:
    case MNSIM_ERROR_PIN2_BLOCKED:
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_PIN_BLOCKED);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, TRUE);
        break;
        
    case MNSIM_ERROR_PIN1_UNBLOCK_BLOCKED:
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_PUK_BLOCKED);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, TRUE);
        break;
        
    case MNSIM_ERROR_PIN2_UNBLOCK_BLOCKED:
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_PUK2_BLOCKED);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, TRUE);
        break;

    case MNSIM_ERROR_REMOVED:
#ifdef MCARE_V31_SUPPORT
        McareV31_ExitMcare();
#endif
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_NO_SIM);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, FALSE);
        NotifySIMPlugInd(dual_sys, MMIPHONE_NOTIFY_SIM_PLUG_OUT);
        break;
        
#ifdef MMI_SIM_LOCK_SUPPORT
    case MNSIM_ERROR_SIM_DATA_INTEGRITY_FAIL:
    case MNSIM_ERROR_SIM_NETWORT_LOCKED:
    case MNSIM_ERROR_SIM_NETWORT_SUBSET_LOCKED:
    case MNSIM_ERROR_SIM_SP_LOCKED:
    case MNSIM_ERROR_SIM_CORPORATE_LOCKED:
    case MNSIM_ERROR_SIM_USER_LOCKED:
        MMIPHONE_SetSimStatus(dual_sys, SIM_LOCKED);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, FALSE);
        break;
#endif 

    default:
        MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_NO_SIM);
        MMIAPIPHONE_SetSimExistedStatus(dual_sys, FALSE);
        break;
    }
}

/*****************************************************************************/
// 	Description : the current sim card is same to the last sim card
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsSameSim(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return FALSE;
    }

    return s_is_same_sim[dual_sys];
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SIM_NOT_READY_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimNotReadyInd(
                                        DPARAM              param
                                        )
{
    MnSimNotReadyIndS   sim_not_ready_ind = *(MnSimNotReadyIndS *)param;
    MMI_RESULT_E        result = MMI_RESULT_FALSE;//MMI_RESULT_TRUE;
    SUB_CONTENT_E       sub_content = SUB_CONTENT_NONE;
    uint32 i = 0;
    BOOLEAN is_handling_pin = FALSE;
    
    StoreSimNotReadyStatus(sim_not_ready_ind);

#ifdef MMI_LDN_SUPPORT
    MMIAPICL_InitCallInfo(sim_not_ready_ind.dual_sys);
#endif
    
#if defined(MMI_SIM_LOCK_SUPPORT)
    switch (sim_not_ready_ind.reason)
    {
    case MNSIM_ERROR_SIM_NETWORT_LOCKED:
    case MNSIM_ERROR_SIM_NETWORT_SUBSET_LOCKED:
    case MNSIM_ERROR_SIM_SP_LOCKED:
    case MNSIM_ERROR_SIM_CORPORATE_LOCKED:
    case MNSIM_ERROR_SIM_USER_LOCKED:
        {
            MMI_STRING_T pin_value ={0};
            
            if (MMK_IsOpenWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID))//记录下正确的pin码
            {
                GUIEDIT_GetString(PHONE_NEW_PIN_AGAIN_EDITBOX_CTRL_ID, &pin_value);
                g_pin_value[sim_not_ready_ind.dual_sys].blocklen = pin_value.wstr_len;

                MMI_WSTRNTOSTR(
                    g_pin_value[sim_not_ready_ind.dual_sys].blockbuf,
                    MN_MAX_BLOCK_LEN,
                    pin_value.wstr_ptr,
                    g_pin_value[sim_not_ready_ind.dual_sys].blocklen,
                    g_pin_value[sim_not_ready_ind.dual_sys].blocklen
                    );
            }
            else if (MMK_IsOpenWin(PHONE_PIN_INPUT_WIN_ID)) //记录下正确的pin码
            {
                GUIEDIT_GetString(PHONE_PIN_EDITBOX_CTRL_ID, &pin_value);
                g_pin_value[sim_not_ready_ind.dual_sys].blocklen = pin_value.wstr_len;

                MMI_WSTRNTOSTR(
                    g_pin_value[sim_not_ready_ind.dual_sys].blockbuf,
                    MN_MAX_BLOCK_LEN,
                    pin_value.wstr_ptr,
                    g_pin_value[sim_not_ready_ind.dual_sys].blocklen,
                    g_pin_value[sim_not_ready_ind.dual_sys].blocklen
                    );
            }
            #if 0 //def MMI_MULTI_SIM_SYS_SINGLE  //reserved for Multi SIM
                    BOOLEAN is_need_handle = FALSE;
                    BOOLEAN is_handling_simlock = FALSE;
                    /*set sim_not_ready_ind*/
                    SCI_MEMCPY(&s_handle_sim_lock_status.sim_not_ready_ind[sim_not_ready_ind.dual_sys],&sim_not_ready_ind,sizeof(MnSimNotReadyIndS));

                    for(i = 0; i < MMI_DUAL_SYS_MAX; i++)
                    {
                        /*有其他卡正处在SIM Lock handling 状态，设置当前卡为need handle*/
                        if(( i != sim_not_ready_ind.dual_sys)&&(s_handle_sim_lock_status.is_handling_simlock[i]))
                        {
                            is_need_handle = TRUE;
                            break;            
                        }
                    } 
                    if(is_need_handle)
                    {
                        s_handle_sim_lock_status.need_handle_simlock[sim_not_ready_ind.dual_sys] = TRUE;
                		
                        return result;
                    }
                    else
                    {
                        /*当前卡需要处理SIM Lock,记下状态，进入密码输入界面*/
                        s_handle_sim_lock_status.is_handling_simlock[sim_not_ready_ind.dual_sys] = TRUE;//set the status (lock)
                        s_handle_sim_lock_status.cur_handling_sim_id = sim_not_ready_ind.dual_sys;
                        //create input SIM lock window
                        MMIPHONE_CreateInputSimLockWindow(sim_not_ready_ind);
                        return result;
                    }    
            #else            
                    MMIPHONE_CreateInputSimLockWindow(sim_not_ready_ind);
                    return result;
            #endif
         }
                
        case MNSIM_ERROR_SIM_DATA_INTEGRITY_FAIL:
            return result;
                    
        default:
            break;
        }
 #endif
    if ( (MMK_IsOpenWin(PHONE_PIN_WAIT_WIN_ID)
        || MMK_IsOpenWin(PHONE_PIN_ALERT_WIN_ID)
        || MMK_IsOpenWin(PHONE_PIN_INPUT_WIN_ID)
        || MMK_IsOpenWin(PHONE_NEW_PIN_INPUT_WIN_ID)
        || MMK_IsOpenWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID)
        || MMK_IsOpenWin(PHONE_PUK_INPUT_WIN_ID))
        && s_handle_pin_status.cur_handling_sim_id == sim_not_ready_ind.dual_sys)
    {
        if (MMIPHONE_PS_POWER_ON_NORMAL != s_ps_ready[sim_not_ready_ind.dual_sys])
        {
            g_service_status[sim_not_ready_ind.dual_sys].is_slecting = TRUE;
        }

        //关闭pin和puk输入窗口
        MMK_CloseWin(PHONE_PIN_ALERT_WIN_ID);
        MMK_CloseWin(PHONE_PIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_NEW_PIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_PUK_INPUT_WIN_ID);
    }

    //add for dual sys
    s_handle_pin_status.need_handle_pin[sim_not_ready_ind.dual_sys] = FALSE;
    s_handle_pin_status.is_handling_pin[sim_not_ready_ind.dual_sys] = FALSE;

    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (s_handle_pin_status.is_handling_pin[i])
        {
            is_handling_pin = TRUE;
            break;
        }
    }
    
    if (!is_handling_pin)
    {
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            if (i != sim_not_ready_ind.dual_sys)
            {
                if (s_handle_pin_status.need_handle_pin[i])
                {
                    //set s_handle_pin_status
                    s_handle_pin_status.cur_handling_sim_id = (MN_DUAL_SYS_E)i;
                    s_handle_pin_status.is_handling_pin[i] = TRUE;
                    
                    //create input pin window
                    CreateInputPinWindow(s_handle_pin_status.sim_pin_ind[i]);
                    return result;
                }
            }
        }
    }
    
    switch (sim_not_ready_ind.reason)
    {
    case MNSIM_ERROR_NO_SECRETCODE_INITIALIZED:
        // 打开SIM卡被拒界面! 
        sub_content = SUB_CONTENT_SIM_REJECTED;
        break;
        
    case MNSIM_ERROR_REJECT_BY_NET:
        // 打开SIM卡注册失败界面! 
        sub_content = SUB_CONTENT_SIM_REGISTRATION_FAILED;
        break;
        
    case MNSIM_ERROR_NOT_SUPPORT_GSM_ONLY://@zhaohui,cr109170
        MMIPUB_OpenAlertWarningWin(TXT_NOT_SUPPORT_SIM);	
        sub_content = SUB_CONTENT_SIM_REGISTRATION_FAILED;
        break;
        
    case MNSIM_ERROR_PIN1_BLOCKED:
    case MNSIM_ERROR_PIN2_BLOCKED:
        //jassmine cr14034 PIN被锁，不用处理此消息
        break;
        
    case MNSIM_ERROR_PIN1_UNBLOCK_BLOCKED:
        sub_content = SUB_CONTENT_PUK_BLOCKED;
        break;
        
    case MNSIM_ERROR_PIN2_UNBLOCK_BLOCKED:
        sub_content = SUB_CONTENT_PUK2_BLOCKED;
        break;
        
    case MNSIM_ERROR_REMOVED:
        sub_content = SUB_CONTENT_NO_SIM;
        break;
        
    default:
        sub_content = SUB_CONTENT_NO_SIM;
        break;
    }
    
    MAIN_SetIdleNetWorkName();    
    
    //关闭pin和puk输入窗口
    if (s_handle_pin_status.cur_handling_sim_id == sim_not_ready_ind.dual_sys)
    {
        if (MMIPHONE_PS_POWER_ON_NORMAL != s_ps_ready[sim_not_ready_ind.dual_sys])
        {
            g_service_status[sim_not_ready_ind.dual_sys].is_slecting = TRUE;
        }

        MMK_CloseWin(PHONE_PIN_ALERT_WIN_ID);
        MMK_CloseWin(PHONE_PIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_NEW_PIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_NEW_PIN_AGAIN_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_PUK_INPUT_WIN_ID);
        MMK_CloseWin(PHONE_PIN_WAIT_WIN_ID);
    }
    
    //刷新小屏,显示相应的Sim消息
    if(sub_content != SUB_CONTENT_NONE &&  SUB_CONTENT_NO_SIM != sub_content )
    {
        MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;

        //有任意一张sim卡是NO SIM 的状态就直接return !!!
        for(dual_sys = MN_DUAL_SYS_1;dual_sys < MMI_DUAL_SYS_MAX;dual_sys++)
        {
            if(SIM_STATUS_NO_SIM !=MMIAPIPHONE_GetSimStatus(dual_sys))
            {
                return (result);
            }
        }
        
        //刷新小屏,显示相应的Sim消息
        MMISUB_IsPermitUpdate(TRUE);  
        MMISUB_SetSubLcdDisplay(TRUE,TRUE,sub_content,PNULL);
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : deal with signal of APP_MN_SCELL_RSSI_IND
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleScellRssiInd(
                                      DPARAM              param
                                      )
{
    MnScellRssiIndS     srri_ind = *(MnScellRssiIndS *)param;
    MMI_RESULT_E        result = MMI_RESULT_TRUE;
    uint8               rxLevel = 0;
    MN_DUAL_SYS_E       dual_sys = MN_DUAL_SYS_1;
    BOOLEAN is_direct_draw = FALSE;
    
    dual_sys = srri_ind.dual_sys;
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandleScellRssiInd:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5110_112_2_18_2_45_10_96,(uint8*)"d", dual_sys);
        return MMI_RESULT_FALSE;
    }
    
    //设置全局变量
    if (g_service_status[dual_sys].is_first_rssi
        && 0 == g_service_status[dual_sys].rssiLevel
        && 0 < srri_ind.rxlev)//第一次收到非0的rssi直接显示
    {
        g_service_status[dual_sys].is_first_rssi = FALSE;
        is_direct_draw = TRUE;
    }
 
    g_service_status[dual_sys].rssiLevel = srri_ind.rxlev;
    
    //设置信强度,根据参考手机sagem得到的数据，将idle界面下的信号强度划分成5级
    // 1：1-7； 2：8-11； 3：12-15； 4：16-19； 5：20-63
    if (20 <= srri_ind.rxlev)
    {
        rxLevel = PHONE_RX_BAT_DEFAULT_LEVEL;
    }
    else if (16 <= srri_ind.rxlev)
    {
        rxLevel = PHONE_RX_BAT_DEFAULT_LEVEL - 1;
    }
    else if (12 <= srri_ind.rxlev)
    {
        rxLevel = 3;
    }
    else if (8 <= srri_ind.rxlev)
    {
        rxLevel = 2;
    }
    else if (1 <= srri_ind.rxlev)
    {
        rxLevel = 1;
    }
    else
    {
        rxLevel = 0;
    }

    MAIN_SetIdleRxLevel(dual_sys, rxLevel, is_direct_draw);

    NotifyRxLevelChange(dual_sys, rxLevel);
    #if defined(BLUETOOTH_SUPPORT) && defined(BT_BQB_SUPPORT)
    MMIAPIBT_SetSignalStrength(rxLevel); //mdy by qing.yu@for cr
    #endif
    return (result);
}



/*****************************************************************************/
// 	Description : power off and query
//	Global resource dependence : 
//  Author: Jassmine
//	Note:sometimes we should query and confirm
/*****************************************************************************/
void MMIAPIPHONE_PowerOff(void)
{
#ifdef DL_SUPPORT
    if (MMIAPIDL_GetDownloadStatus())
    {
        MMIPHONE_OpenQueryPowerOff(MMIPHONE_QUERY_POWER_OFF_DOWNLOAD);
    }
    else
#endif
    {
        MMIPHONE_PowerOff();
    }
}

/*****************************************************************************/
// 	Description : power off directly
//	Global resource dependence : 
//  Author: Jassmine
//	Note: sometimes we should poweroff directly
/*****************************************************************************/
void MMIAPIPHONE_PowerOffEx(void)
{
    MMIPHONE_PowerOff();
}

/*****************************************************************************/
// 	Description : power off MS
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
void MMIPHONE_PowerOff(void)
{
    uint32 i=0;
    POWER_RESTART_CONDITION_E   restart_condition = POWER_GetRestartCondition();
    
    //SCI_TRACE_LOW:"MMIAPIPHONE_PowerOff:restart_condition=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5199_112_2_18_2_45_10_97,(uint8*)"d", restart_condition);
    
#if defined(MMI_SIM_LOCK_SUPPORT)    
        /*在倒计时界面关机时需要记住还剩下的时间*/
    if(MMK_IsOpenWin( PHONE_SIM_LOCK_COUNTDOWN_WIN_ID))
    {
        MMI_OPERATE_SIM_LOCK_T sim_lock_operate = {0};
        uint32 trials_counter = 0;
        uint32 time_out_value = 0;

        //SCI_TRACE_LOW:"MMIPHONE_PowerOff PHONE_SIM_LOCK_COUNTDOWN_WIN_ID open"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5209_112_2_18_2_45_10_98,(uint8*)"");
        sim_lock_operate.lock_type = s_sim_lock_operate_value.lock_type;
        MMIAPIPHONE_GetSIMLockRemainTrialsTimer(sim_lock_operate.lock_type,&trials_counter, &time_out_value);
        time_out_value = MMIPHONE_GetCurCountdownTime();
        MNPHONE_UpdateSIMUnlockRetryData(sim_lock_operate.lock_type, trials_counter, time_out_value);
        MMIAPIPHONE_UpdateSIMUnlockData();
    }
#endif    
    
    if(MMIAPIMTV_IsMTVRunning())
    {
        //SCI_TRACE_LOW:"MMIPHONE_PowerOff MMIAPIMTV_IsMTVRunning delay poweroff"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5220_112_2_18_2_45_10_99,(uint8*)"");
        MMIAPIMTV_SetDelayPowerOff(TRUE);
        MMIAPIMTV_Exit();
        return;
    }
        
#ifdef MMI_CSTAR_UDB_SUPPORT
    MMIIM_SaveUdbEntry();
#endif

    MMIAPIMP3_StopAudioPlayer();
    MMIAPIRECORD_StopRecord();

    // 调用MN层函数 请求关机
    for(i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        MNPHONE_DeactivePsEx(i);
    }
    
    if( MMK_IsOpenWin(PHONE_STARTUP_NORMAL_WIN_ID))
    {
        MMK_CloseWin(PHONE_STARTUP_NORMAL_WIN_ID);
    }

    MMIAPIENG_StopQData();

    MMI_FreeAllRes();
    // 打开关机窗口
    MMIAPIUDISK_ClosePCCamera();
    
#ifdef WIFI_SUPPORT
    MMIAPIWIFI_Off();
#endif

#ifdef MCARE_V31_SUPPORT 
	McareV31_ExitMcare();
#endif
    if (RESTART_BY_CHARGE == restart_condition)
    {
        POWER_PowerOff();
    }
    else
    {
        MMK_CreateWin((uint32*)PHONE_POWER_OFF_WIN_TAB,PNULL);
        MMIAPIBT_AppDeInit();
    }
}

/*****************************************************************************/
// 	Description : power reset the phone should de_init the BT
//	Global resource dependence : 
//  Author: kelly.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_PowerReset(void)
{   
    POWER_RESTART_CONDITION_E   restart_condition = POWER_GetRestartCondition();
    
    if (RESTART_BY_CHARGE != restart_condition)
    {
        //Exit Blue tooth
        MMIAPIBT_AppDeInit();
    }
    
    MMIAPISET_StopAllRing(FALSE);
    
    //在MMIPHONE_PowerReset之前关闭背光
    MMIDEFAULT_TurnOffBackLight();
    MMIDEFAULT_StopKeybadBackLightTimer();
    MMIDEFAULT_SetKeybadBackLight(FALSE);
    
    POWER_Reset();
}

/*****************************************************************************/
// 	Description : get plmn name, cmcc, unicom or others
//	Global resource dependence : 
//  Author: Lin.Lin
//	Note:
/*****************************************************************************/
PUBLIC PHONE_PLMN_NAME_E MMIAPIPHONE_GetNetworkName(MN_DUAL_SYS_E dual_sys)
{
    PHONE_PLMN_NAME_E name = PLMN_INVALID;
    uint16 i = 0;
    const MMI_NETWORK_NAME_T *network_table_ptr = PNULL;
    uint16 network_table_len = 0;
    
    if(MMI_DUAL_SYS_MAX <= dual_sys)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GetNetworkName err dual_sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5298_112_2_18_2_45_10_100,(uint8*)"d", dual_sys);
        return PLMN_INVALID;
    }

    if ((g_service_status[dual_sys].plmn.mnc < 100 && (3 == g_service_status[dual_sys].plmn.mnc_digit_num)))
    {
        network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_mnc_digit_num_3_nametable;
        network_table_len = MAX_NETWORK_MNC_DIGIT_NUM_3_NUM;
    }
    else
    {
        network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_nametable;
        network_table_len = MAX_NETWORK_NUM;
    }
    
    for (i = 0; i < network_table_len; i++)
    {
        if ((network_table_ptr[i].mnc == g_service_status[dual_sys].plmn.mnc)
            && (network_table_ptr[i].mcc == g_service_status[dual_sys].plmn.mcc))
        {
            name = network_table_ptr[i].name;
            break;
        }
    }
    
    return name;   
}

/*****************************************************************************/
// 	Description : get the network name
//	Global resource dependence : 
//  Author: Great.Tian
//	Note:
/*****************************************************************************/
PUBLIC MMI_TEXT_ID_T MMIAPIPHONE_GetNetWorkNameId(MMI_GMMREG_RAT_E rat, MN_PLMN_T *plmn_ptr)
{
    uint16 i = 0;
    MMI_TEXT_ID_T network_name_id = TXT_NULL;
    const MMI_NETWORK_NAME_T *network_table_ptr = PNULL;
    uint16 network_table_len = 0;

    if (PNULL == plmn_ptr)
    {
        return network_name_id;
    }

    if ((plmn_ptr->mnc < 100 && (3 == plmn_ptr->mnc_digit_num)))
    {
        network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_mnc_digit_num_3_nametable;
        network_table_len = MAX_NETWORK_MNC_DIGIT_NUM_3_NUM;
    }
    else
    {
        network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_nametable;
        network_table_len = MAX_NETWORK_NUM;
    }
    
    for (i = 0 ; i < network_table_len ; i++)
    {
        if ((network_table_ptr[i].mnc == plmn_ptr->mnc)
            && (network_table_ptr[i].mcc == plmn_ptr->mcc))
        {
            network_name_id = network_table_ptr[i].name_index;

#ifdef CMCC_UI_STYLE
            if(MMI_GMMREG_RAT_3G == rat && TXT_NET_CMCC == network_name_id)
            {
                network_name_id = TXT_NET_CMCC_3G;
            }
#endif
            break;
        }
    }
    
    return (network_name_id);
}

/*****************************************************************************/
// 	Description : get the network id
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC PHONE_PLMN_NAME_E MMIAPIPHONE_GetNetWorkId(MN_PLMN_T *plmn_ptr)
{
    uint16 i = 0;
    PHONE_PLMN_NAME_E plmn_name = PLMN_INVALID;
    const MMI_NETWORK_NAME_T *network_table_ptr = PNULL;
    uint16 network_table_len = 0;

    if (PNULL == plmn_ptr)
    {
        return plmn_name;
    }

    network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_nametable;
    network_table_len = MAX_NETWORK_NUM;
    
    for (i = 0 ; i < network_table_len ; i++)
    {
        if ((network_table_ptr[i].mnc == plmn_ptr->mnc)
            && (network_table_ptr[i].mcc == plmn_ptr->mcc))
        {
            plmn_name = network_table_ptr[i].name;

            break;
        }
    }

    if (PLMN_INVALID == plmn_name)
    {
        network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_mnc_digit_num_3_nametable;
        network_table_len = MAX_NETWORK_MNC_DIGIT_NUM_3_NUM;

        for (i = 0 ; i < network_table_len ; i++)
        {
            if ((network_table_ptr[i].mnc == plmn_ptr->mnc)
                && (network_table_ptr[i].mcc == plmn_ptr->mcc))
            {
                plmn_name = network_table_ptr[i].name;

                break;
            }
        }
    }
    
    return (plmn_name);
}

/*****************************************************************************/
// 	Description : get the network info
//	Global resource dependence : 
//  Author: dave.ruan
//	Note:in gprs data account,we only use one mnc to represent one operator,but it may have other mnc
//           if so,in auto adapting ,we will not find the right network id;so we use mcc and networkname index to get network name
/*****************************************************************************/
PUBLIC const MMI_NETWORK_NAME_T *MMIAPIPHONE_GetNetWorkInfo(PHONE_PLMN_NAME_E plmn_name)
{
    uint16 i = 0;
    const MMI_NETWORK_NAME_T *network_table_ptr = PNULL;
    uint16 network_table_len = 0;

    network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_nametable;
    network_table_len = MAX_NETWORK_NUM;
    
    /*search from 0 to higher,so we need to set data account mnc as the smallest  of all mnc for the oprator*/
    for (i = 0 ; i < network_table_len ; i++)
    {
        if (network_table_ptr[i].name == plmn_name)
        {
            return &network_table_ptr[i];
        }
    }

    network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_mnc_digit_num_3_nametable;
    network_table_len = MAX_NETWORK_MNC_DIGIT_NUM_3_NUM;

    for (i = 0 ; i < network_table_len ; i++)
    {
        if (network_table_ptr[i].name == plmn_name)
        {
            return &network_table_ptr[i];
        }
    }
    
    return PNULL;
}

/*****************************************************************************/
// 	Description : get full plmn name, cmcc, unicom or others
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_GetFullPlmn(MN_DUAL_SYS_E dual_sys, PHONE_PLMN_NAME_E *plmn_name, PHONE_PLMN_NAME_E *hplmn_name)
{
    const MMI_NETWORK_NAME_T *network_table_ptr = PNULL;
    uint16 network_table_len = 0;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    if (PNULL != plmn_name)
    {
        *plmn_name = MMIAPIPHONE_GetNetworkName(dual_sys);
    }

    if (PNULL != hplmn_name)
    {
        uint16 i = 0;

        *hplmn_name = PLMN_INVALID;

        if ((s_sim_status[dual_sys].hplmn.mnc < 100 && (3 == s_sim_status[dual_sys].hplmn.mnc_digit_num)))
        {
            network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_mnc_digit_num_3_nametable;
            network_table_len = MAX_NETWORK_MNC_DIGIT_NUM_3_NUM;
        }
        else
        {
            network_table_ptr = (const MMI_NETWORK_NAME_T *)s_network_nametable;
            network_table_len = MAX_NETWORK_NUM;
        }

        if (SIM_STATUS_OK == MMIAPIPHONE_GetSimStatus(dual_sys))
        {
            for (i = 0; i < network_table_len; i++)
            {
                if ((network_table_ptr[i].mnc == s_sim_status[dual_sys].hplmn.mnc) &&
                    (network_table_ptr[i].mcc == s_sim_status[dual_sys].hplmn.mcc))
                {
                    *hplmn_name = network_table_ptr[i].name;
                    break;
                }
            }
        }
    }
}


/*****************************************************************************/
// 	Description : 当sim卡注册失败和注册成功的时候都应该可以设置pin1
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsPermitSetPin1(MN_DUAL_SYS_E dual_sys)
{
    BOOLEAN     is_permit = FALSE;
    SIM_STATUS_E sim_status = MMIAPIPHONE_GetSimStatus(dual_sys);
    
    is_permit = (BOOLEAN)((SIM_STATUS_OK == sim_status) 
                                        || (SIM_STATUS_REGISTRATION_FAILED == sim_status)
                                        || (SIM_STATUS_PIN_BLOCKED == sim_status));
    
    return(is_permit);
}

/*****************************************************************************/
// 	Description : 获取设置pin1 的sim 个数
//	Global resource dependence : 
//  Author: Michael.Shi
//	Note:  
/*****************************************************************************/
PUBLIC uint32 MMIAPIPHONE_GetPermitSetPin1Num(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MMIAPIPHONE_IsPermitSetPin1(i))
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : according to ps status, judge if wap & mms is available
//	Global resource dependence : 
//  Author: samboo.shen
//	Note:
/*****************************************************************************/
PUBLIC MMIPHONE_ROAMING_STATUS_E MMIAPIPHONE_GetRoamingStatus(MN_DUAL_SYS_E dual_sys)
{
    MMIPHONE_ROAMING_STATUS_E roaming_status = MMIPHONE_ROAMING_STATUS_MAX;
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GetRoamingStatus:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5561_112_2_18_2_45_11_101,(uint8*)"d", dual_sys);
        return MMIPHONE_ROAMING_STATUS_NO_ROMING;
    }
    if(!g_service_status[dual_sys].isRoaming)
    {
        roaming_status = MMIPHONE_ROAMING_STATUS_NO_ROMING;
    }
    else
    {
        if(g_service_status[dual_sys].plmn.mcc == s_sim_status[dual_sys].hplmn.mcc
            ||((g_service_status[dual_sys].plmn.mcc == 404)&&(s_sim_status[dual_sys].hplmn.mcc == 405))    
            ||((g_service_status[dual_sys].plmn.mcc == 405)&&(s_sim_status[dual_sys].hplmn.mcc == 404))//india MCC
        )
        {
            roaming_status = MMIPHONE_ROAMING_STATUS_NATIVE_ROMING;
        }
        else
        {
            roaming_status = MMIPHONE_ROAMING_STATUS_NATIONAL_ROMING;
        }
    }
    //SCI_TRACE_LOW:"roaming status: %d, net isroaming: %d, net mcc: %d, sim mcc: %d "
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5583_112_2_18_2_45_11_102,(uint8*)"dddd", roaming_status, g_service_status[dual_sys].isRoaming, g_service_status[dual_sys].plmn.mcc, s_sim_status[dual_sys].hplmn.mcc);
    return roaming_status;
}

/*****************************************************************************/
// 	Description : get opn display string
//	Global resource dependence : 
//  Author: kun.yu
//	Note:  
/*****************************************************************************/
PUBLIC void MMIPHONE_GetAciString(PHONE_SERVICE_STATUS_T *service_status,//IN:
                                         MMI_STRING_T *aci_str_ptr, //OUT
                                         uint16 max_aci_len //IN
                                         )
{
    uint16 i = 0;
    uint16 aci_index = 0;
    char   aic_info[MAX_ACI_INFO_LEN + 1] = {0};

    if((!service_status->is_add_ci) || (PNULL == service_status) || (PNULL == aci_str_ptr))
    {
        return;
    }
    
    for(i = 0; i < ARR_SIZE(s_network_acitable); i++)
    {
        if(s_network_acitable[i].mcc == service_status->plmn.mcc)
        {
            aci_index = i;
            break;
        }
    }

    if(aci_index > 0)
    {
        aci_str_ptr->wstr_len = SCI_STRLEN((char*)s_network_acitable[aci_index].aci_str);
        aci_str_ptr->wstr_len = MIN(aci_str_ptr->wstr_len, max_aci_len);
        MMI_STRNTOWSTR(aci_str_ptr->wstr_ptr,
                                aci_str_ptr->wstr_len,
                                (uint8 *)s_network_acitable[aci_index].aci_str,
                                aci_str_ptr->wstr_len,
                                aci_str_ptr->wstr_len
                                );
    }
    else
    {
        sprintf((char *)aic_info, "%03d", service_status->plmn.mcc);
        aci_str_ptr->wstr_len = SCI_STRLEN((char *)aic_info);
        aci_str_ptr->wstr_len = MIN(aci_str_ptr->wstr_len, max_aci_len);
        MMI_STRNTOWSTR(aci_str_ptr->wstr_ptr,
                                    aci_str_ptr->wstr_len,
                                    (uint8 *)aic_info,
                                    aci_str_ptr->wstr_len,
                                    aci_str_ptr->wstr_len
                                    );
    }
}

/*****************************************************************************/
// 	Description : get opn display string
//	Global resource dependence : 
//  Author: wancan.you
//	Note:  
/*****************************************************************************/
LOCAL void GetOpnString(MN_DUAL_SYS_E dual_sys,//IN:
                                            MMI_STRING_T *opn_str_ptr,//IN/OUT
                                            uint16 max_opn_len
                                            )
{
    PHONE_SERVICE_STATUS_T  *service_status = &g_service_status[dual_sys];         //the status of service

    if(PNULL == opn_str_ptr ||dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"GetOpnString dual_sys = %d OR opn_str_ptr is NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5652_112_2_18_2_45_11_103,(uint8*)"d", dual_sys);
        return;
    }

    if (service_status->is_opn_exist && 0 < service_status->opn_len)
    {
        if (MN_OPN_DCS_8BIT == service_status->opn_dcs)
        {
            opn_str_ptr->wstr_len = MIN(service_status->opn_len, max_opn_len);

            opn_str_ptr->wstr_len = MMIAPICOM_Default2Wchar(
                                                            (uint8 *)service_status->opn,
                                                            opn_str_ptr->wstr_ptr,
                                                            opn_str_ptr->wstr_len
                                                            );
        }
        else
        {
            opn_str_ptr->wstr_len = MIN(service_status->opn_len/sizeof(wchar), max_opn_len);

            MMI_WSTRNCPY(opn_str_ptr->wstr_ptr,
                                    opn_str_ptr->wstr_len,
                                    (wchar *)service_status->opn,
                                    opn_str_ptr->wstr_len,
                                    opn_str_ptr->wstr_len
                                    );
        }
    }
    else
    {
        MMIPHONE_NV_NETWORK_NAME_T* nv_network_name = MMIPHONE_GetNVNetworkNameInfo();
            //是否相同卡,同一张卡才能使用之前的OPN
        if(MMIAPIPHONE_IsSameSim(dual_sys)
            &&(nv_network_name[dual_sys].is_opn_exist)
            &&(0 < nv_network_name[dual_sys].opn_len))
        {
            if (MN_OPN_DCS_UCS2 == nv_network_name[dual_sys].opn_dcs)
            {
                opn_str_ptr->wstr_len = MIN(nv_network_name[dual_sys].opn_len/sizeof(wchar), max_opn_len);

                MMI_WSTRNCPY(opn_str_ptr->wstr_ptr,
                                        opn_str_ptr->wstr_len,
                                        (wchar *)nv_network_name[dual_sys].opn,
                                        opn_str_ptr->wstr_len,
                                        opn_str_ptr->wstr_len
                                        );
            }
            else
            {
                opn_str_ptr->wstr_len = MIN(nv_network_name[dual_sys].opn_len, max_opn_len);

                opn_str_ptr->wstr_len = MMIAPICOM_Default2Wchar(
                                                                (uint8 *)nv_network_name[dual_sys].opn,
                                                                opn_str_ptr->wstr_ptr,
                                                                opn_str_ptr->wstr_len
                                                                );
            }
        }
        else
        {
            opn_str_ptr->wstr_len = max_opn_len;

            MMIAPIPHONE_GenPLMNDisplay(
                                                    MMIAPIPHONE_GetTDOrGsm(),
                                                    &service_status->plmn,
                                                    opn_str_ptr->wstr_ptr,
                                                    &opn_str_ptr->wstr_len,
                                                    FALSE);
        }
    }
}

/*****************************************************************************/
// 	Description : get spn display string
//	Global resource dependence : 
//  Author: wancan.you
//	Note:  
/*****************************************************************************/
LOCAL void GetSpnString(MN_DUAL_SYS_E dual_sys,//IN:
                                            MMI_STRING_T *spn_str_ptr,//IN/OUT
                                            uint16 max_spn_len
                                            )
{
    PHONE_SERVICE_STATUS_T  *service_status = &g_service_status[dual_sys];         //the status of service

    if(PNULL == spn_str_ptr || dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"GetOpnString dual_sys = %d OR spn_str_ptr is NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5736_112_2_18_2_45_11_104,(uint8*)"d", dual_sys);
        return;
    }

    if (0 < service_status->spn_len)
    {
        MMIAPICOM_ParseSimStr(service_status->spn,
                                            service_status->spn_len,
                                            spn_str_ptr,
                                            max_spn_len);
    }
}

/*****************************************************************************/
// 	Description : compare spn and opn
//	Global resource dependence : 
//  Author: kun.yu
//	Note:  
/*****************************************************************************/
LOCAL BOOLEAN MMIPHONE_IsSPNEqualtoOPN(MMI_STRING_T *opn_str_ptr,//IN:
                                                                                    MMI_STRING_T *spn_str_ptr//IN:
                                                                                    )
{
    BOOLEAN result = FALSE;

    if((PNULL == opn_str_ptr->wstr_ptr) || (PNULL == spn_str_ptr->wstr_ptr))
    {
        return FALSE;
    }
    
    if(0 == MMIAPICOM_CompareTwoWstrExt(opn_str_ptr->wstr_ptr,
                                                        opn_str_ptr->wstr_len,
                                                        spn_str_ptr->wstr_ptr,
                                                        spn_str_ptr->wstr_len,
                                                        FALSE
                                                        ))
    {
        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
// 	Description : cat opn spn display string
//	Global resource dependence : 
//   Author: kun.yu
//	Note:  
/*****************************************************************************/
PUBLIC void MMIPHONE_CatAciOpnSpnString(MMI_STRING_T *aci_str_ptr,//IN 
                                                MMI_STRING_T *opn_str_ptr,//IN:
                                                MMI_STRING_T *spn_str_ptr,//IN:
                                                MMI_STRING_T *network_str_ptr//OUT
                                                )
{
    const char *lnk = "_";
    uint16 length = 0;

    if (aci_str_ptr->wstr_len + opn_str_ptr->wstr_len + spn_str_ptr->wstr_len >= MMIPHONE_MAX_OPER_NAME_LEN - 1)
    {
        return;
    }

    if(aci_str_ptr->wstr_len > 0)
    {
        MMI_WSTRNCPY(network_str_ptr->wstr_ptr,
                                aci_str_ptr->wstr_len,
                                aci_str_ptr->wstr_ptr,
                                aci_str_ptr->wstr_len,
                                aci_str_ptr->wstr_len
                                );

        network_str_ptr->wstr_len = aci_str_ptr->wstr_len;
    }

    if(opn_str_ptr->wstr_len > 0)
    {
        if(aci_str_ptr->wstr_len > 0)
        {
            length = strlen(lnk);
    
            MMI_STRNTOWSTR(network_str_ptr->wstr_ptr + network_str_ptr->wstr_len,
                                    length,
                                    (uint8 *)lnk,
                                    length,
                                    length
                                    );

            network_str_ptr->wstr_len += length;

            //opn
            MMI_WSTRNCPY(network_str_ptr->wstr_ptr + network_str_ptr->wstr_len,
                                    opn_str_ptr->wstr_len,
                                    opn_str_ptr->wstr_ptr,
                                    opn_str_ptr->wstr_len,
                                    opn_str_ptr->wstr_len
                                    );
            network_str_ptr->wstr_len += opn_str_ptr->wstr_len;

        }
        else
        {
            MMI_WSTRNCPY(network_str_ptr->wstr_ptr,
                                    opn_str_ptr->wstr_len,
                                    opn_str_ptr->wstr_ptr,
                                    opn_str_ptr->wstr_len,
                                    opn_str_ptr->wstr_len
                                    );

            network_str_ptr->wstr_len = opn_str_ptr->wstr_len;
        }
    }

    if((spn_str_ptr->wstr_len > 0) && (!MMIPHONE_IsSPNEqualtoOPN(opn_str_ptr, spn_str_ptr)))
    {
        if(aci_str_ptr->wstr_len + opn_str_ptr->wstr_len > 0)
        {
            length = strlen(lnk);
    
            MMI_STRNTOWSTR(network_str_ptr->wstr_ptr + network_str_ptr->wstr_len,
                                    length,
                                    (uint8 *)lnk,
                                    length,
                                    length
                                    );

            network_str_ptr->wstr_len += length;

            MMI_WSTRNCPY(network_str_ptr->wstr_ptr + network_str_ptr->wstr_len,
                                    spn_str_ptr->wstr_len,
                                    spn_str_ptr->wstr_ptr,
                                    spn_str_ptr->wstr_len,
                                    spn_str_ptr->wstr_len
                                    );

            network_str_ptr->wstr_len += spn_str_ptr->wstr_len;
        }
        else
        {
            MMI_WSTRNCPY(network_str_ptr->wstr_ptr + network_str_ptr->wstr_len,
                                    spn_str_ptr->wstr_len,
                                    spn_str_ptr->wstr_ptr,
                                    spn_str_ptr->wstr_len,
                                    spn_str_ptr->wstr_len
                                    );

            network_str_ptr->wstr_len = spn_str_ptr->wstr_len;
        }
    }
}

/*****************************************************************************/
// 	Description : is network selecting
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsNetworkSelecting(MN_DUAL_SYS_E dual_sys)
{
    MN_PHONE_PLMN_STATUS_E plmn_status = PLMN_NO_SERVICE;

    if(MMI_DUAL_SYS_MAX <= dual_sys)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsNetworkSelecting dual_sys = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5905_112_2_18_2_45_12_105,(uint8*)"d",dual_sys);
        return FALSE;
    }
    
    plmn_status = MMIAPIPHONE_GetPlmnStatus(dual_sys);

    //SCI_TRACE_LOW:"MMIAPIPHONE_IsNetworkSelecting plmn_status=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5911_112_2_18_2_45_12_106,(uint8*)"d", plmn_status);
    
    if(( PLMN_REGISTER_GPRS_ONLY == plmn_status)
        || ( PLMN_REGISTER_SERVICE == plmn_status)
        || g_service_status[dual_sys].is_slecting)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
// 	Description : get sim card and plmn service status string to display in idle
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:  
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_GetSimAndServiceString(MN_DUAL_SYS_E dual_sys, MMI_STRING_T *network_str_ptr)
{
    MMI_TEXT_ID_T	idle_str_id = TXT_NULL;
    SIM_STATUS_E sim_status = MMIAPIPHONE_GetSimStatus(dual_sys);

    if(PNULL == network_str_ptr  ||PNULL == network_str_ptr->wstr_ptr|| dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"GetSimAndServiceString: the network_str_ptr || network_str_ptr->wstr_ptr ||dual_sys"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5936_112_2_18_2_45_12_107,(uint8*)"");
        return;
    }

    //SCI_TRACE_LOW:"MMIAPIPHONE_GetSimAndServiceString s_sim_status[%d].simStatus %d,is_opn_need=%d,opn_len=%d,is_opn_exist=%d,is_spn_need=%d,spn_len=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_5942_112_2_18_2_45_12_108,(uint8*)"ddddddd",dual_sys, sim_status,g_service_status[dual_sys].is_opn_need,g_service_status[dual_sys].opn_len,g_service_status[dual_sys].is_opn_exist,g_service_status[dual_sys].is_spn_need,g_service_status[dual_sys].spn_len);

    network_str_ptr->wstr_len = 0;

    //if sim error then display sim string in sim_status_string
    if (sim_status < SIM_STATUS_MAX)
    {
        if (sim_status != SIM_STATUS_OK) //sim not ready
        {
            if(SIM_STATUS_NO_SIM == sim_status)
            {
                 idle_str_id = TXT_NO_SIM;
            }
            else
            {
                idle_str_id = s_sim_status_string[sim_status];
            }

            MMI_GetLabelTextByLang(idle_str_id, network_str_ptr);
        }
        else
        {
            if (g_service_status[dual_sys].is_slecting //searching
                || ((PLMN_NORMAL_GSM_ONLY != g_service_status[dual_sys].plmn_status) 
                && (PLMN_NORMAL_GSM_GPRS_BOTH != g_service_status[dual_sys].plmn_status)))
            {
                idle_str_id = GetNetWorkNameId(
                    &g_service_status[dual_sys].plmn,
                    g_service_status[dual_sys].plmn_status,
                    g_service_status[dual_sys].is_slecting
                    );

                MMI_GetLabelTextByLang(idle_str_id, network_str_ptr);
            }
            else
            {
            #ifdef MMI_NETWORK_NAME_SUPPORT
                MMIPHONE_GetNetworkNameString(dual_sys,network_str_ptr);
            #else
                MMI_STRING_T aci_string = {0}; 
                MMI_STRING_T opn_string = {0};
                MMI_STRING_T spn_string = {0};
                wchar aci[MAX_ACI_INFO_LEN + 1] = {0};
                wchar opn[MAX_ONSHORTFORM_LEN + 1] = {0};
                wchar spn[MN_SPN_ID_NUM_LEN + 1] = {0};
                //get aci string
                if(g_service_status[dual_sys].is_add_ci)
                {
                    aci_string.wstr_ptr = (wchar *)aci;
                    MMIPHONE_GetAciString(&g_service_status[dual_sys], &aci_string, MAX_ACI_INFO_LEN);
                }

                //get opn string
                if (g_service_status[dual_sys].is_opn_need)
                {
                    opn_string.wstr_ptr = (wchar *)opn;
                    GetOpnString(dual_sys, &opn_string, MAX_ONSHORTFORM_LEN);
                }

                //get spn string
                if (g_service_status[dual_sys].is_spn_need)
                {
                    spn_string.wstr_ptr = (wchar *)spn;
                    GetSpnString(dual_sys, &spn_string, MN_SPN_ID_NUM_LEN);
                }

                MMIPHONE_CatAciOpnSpnString(&aci_string, &opn_string, &spn_string, network_str_ptr);
            #endif
                //异常处理
                if (0 == network_str_ptr->wstr_len)
                {
                    network_str_ptr->wstr_len = MMIPHONE_MAX_OPER_NAME_LEN;

                    MMIAPIPHONE_GenPLMNDisplay(
                                                            MMIAPIPHONE_GetTDOrGsm(),
                                                            &g_service_status[dual_sys].plmn,
                                                            network_str_ptr->wstr_ptr,
                                                            &network_str_ptr->wstr_len,
                                                            FALSE);
                }
            }
        }
    }
}

/*****************************************************************************/
// 	Description : 获得sim card状态
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
PUBLIC SIM_STATUS_E MMIAPIPHONE_GetSimStatus(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GetSimStatus err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6032_112_2_18_2_45_12_109,(uint8*)"d", dual_sys);
        return SIM_NOT_INITED;
    }
    
    return s_sim_status[dual_sys].simStatus;
}

/*****************************************************************************/
// 	Description : 设置sim card状态
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
PUBLIC void MMIPHONE_SetSimStatus(MN_DUAL_SYS_E dual_sys, SIM_STATUS_E sim_status)
{
    //SCI_TRACE_LOW:"MMIPHONE_SetSimStatus:dual sys = %d, dual status = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6045_112_2_18_2_45_12_110,(uint8*)"dd",dual_sys,sim_status);
    
    if(dual_sys < MMI_DUAL_SYS_MAX)
    {
            s_sim_status[dual_sys].simStatus = sim_status;
    }
    else
    {
        //SCI_TRACE_LOW:"MMIPHONE_SetSimStatus err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6053_112_2_18_2_45_12_111,(uint8*)"d", dual_sys);
    }
}

/*****************************************************************************/
// 	Description : set sim card exited status
//	Global resource dependence : 
//  Author:kelly.li
//	Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_SetSimExistedStatus(
                                            MN_DUAL_SYS_E dual_sys,
                                            BOOLEAN       is_existed
                                            )
{
    //SCI_TRACE_LOW:"MMIAPIPHONE_SetSimExistedStatus:dual sys = %d, is_existed = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6066_112_2_18_2_45_12_112,(uint8*)"dd",dual_sys,is_existed);
    
    if(dual_sys < MMI_DUAL_SYS_MAX)
    {
        s_sim_existed_status[dual_sys] = is_existed;

        if (MMIAPIIDLE_IdleWinIsOpen())
        {
            MAIN_UpdateIdleSignalState();
        }
        return TRUE;
    }
    else
    {

        //SCI_TRACE_LOW:"MMIAPIPHONE_SetSimExistedStatus:err dual sys = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6081_112_2_18_2_45_12_113,(uint8*)"d",dual_sys);
        return FALSE;
    }

}

/*****************************************************************************/
// 	Description : Get sim card exited status
//	Global resource dependence : 
//  Author:kelly.li
//	Note:  
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GetSimExistedStatus(MN_DUAL_SYS_E dual_sys)
{
    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return FALSE;
    }
    
    return(s_sim_existed_status[dual_sys]);
}

/*****************************************************************************/
// 	Description : Sim卡是否OK，包括PS
//	Global resource dependence : 
//  Author:
//	Note:  SIM OK 的状态: 'SIM_STATUS_OK' and 'PS_Ready_Normal' and '!fly_Mode'
//     Pay attention to this interface used,should better considered MMIAPIPHONE_IsSimAvailable     
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_E dual_sys)
{
    return MMIPHONE_IsSimOk(dual_sys);
}

/*****************************************************************************/
// 	Description : Sim卡是否OK，包括PS
//	Global resource dependence : 
//  Author:
//	Note:  SIM OK 的状态: 'SIM_STATUS_OK' and 'PS_Ready_Normal' and '!fly_Mode'
//     Pay attention to this interface used,should better considered MMIAPIPHONE_IsSimAvailable     
/*****************************************************************************/
BOOLEAN MMIPHONE_IsSimOk(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsSimOk err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6120_112_2_18_2_45_12_114,(uint8*)"d", dual_sys);
        return FALSE;
    }
    
    return((BOOLEAN)(SIM_STATUS_OK == MMIAPIPHONE_GetSimStatus(dual_sys)
                                && (MMIPHONE_PS_POWER_ON_NORMAL == s_ps_ready[dual_sys]) && !MMIAPISET_GetFlyMode()));
}

/*****************************************************************************/
// 	Description : Sim卡是否OK，不包括PS
//	Global resource dependence : 
//  Author:
//	Note:  SIM Card OK 的状态包括两种: SIM_STATUS_OK / SIM_STATUS_REGISTRATION_FAILED
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsSimCardOk(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsSimCardOk err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6136_112_2_18_2_45_12_115,(uint8*)"d", dual_sys);
        return FALSE;
    }

    
    return((BOOLEAN)(SIM_STATUS_OK == MMIAPIPHONE_GetSimStatus(dual_sys)
                                || SIM_STATUS_REGISTRATION_FAILED == MMIAPIPHONE_GetSimStatus(dual_sys)));
}

/*****************************************************************************/
// 	Description : Sim卡是否可用
//	Global resource dependence : 
//  Author: Michael.shi
//	Note:  SIM 卡是否可用的判断条件 : sim 卡存在并且处于待机设置！！
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsSimAvailable(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsSimAvailable err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6153_112_2_18_2_45_12_116,(uint8*)"d", dual_sys);
        return FALSE;
    }

    // 非飞行模式才做处理!!!
    if( !MMIAPISET_GetFlyMode() )
    {
        MMISET_DUALSYS_TYPE_E e_dual_sys = MMIAPISET_GetMultiSysSetting();
        //sim 卡存在并且待机!!!
        if((e_dual_sys & (1<<dual_sys)) && MMIAPIPHONE_GetSimExistedStatus(dual_sys))
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

/*****************************************************************************/
// 	Description : 可用的Sim卡个数
//	Global resource dependence : 
//  Author: Michael.Shi
//	Note:  获取存在并且设置为待机的SIM卡的个数
/*****************************************************************************/
uint32 MMIAPIPHONE_GetSimAvailableNum(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MMIAPIPHONE_IsSimAvailable(i))
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : Sim卡OK个数和卡，包括PS
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
uint32 MMIAPIPHONE_GetSimOkNum(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MMIAPIPHONE_IsSimOk(i))
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : Sim卡OK个数和卡，不包括PS
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
uint32 MMIAPIPHONE_GetSimCardOkNum(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MMIAPIPHONE_IsSimCardOk(i))
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : Get sim card existed status and num
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
uint32 MMIAPIPHONE_GetSimExistedNum(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MMIAPIPHONE_GetSimExistedStatus(i))
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : Get ps ready status and num
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
uint32 MMIAPIPHONE_GetPSReadyNum(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if ((MMIPHONE_PS_POWER_ON_NONE != s_ps_ready[i]))
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : get sys ok num
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
uint32 MMIAPIPHONE_GetSysOkNum(BOOLEAN  *p, uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    if(PNULL == p)
    {
        //SCI_TRACE_LOW:"GetSysOkNum: the p=%d || idx_ptr = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6302_112_2_18_2_45_13_117,(uint8*)"dd", p, idx_ptr);
        return 0;
    }

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (p[i])
        {
            if ((idx_ptr != PNULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : get national roaming num
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
uint32 MMIAPIPHONE_GetNationalRoamingNum(uint16 *idx_ptr, uint16 array_len)
{
    uint32 num = 0;
    uint32 i = 0;

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MMIAPIPHONE_GetRoamingStatus(i) != MMIPHONE_ROAMING_STATUS_NO_ROMING)
        {
            if ((idx_ptr != NULL) && (num < array_len))
            {
                idx_ptr[num] = i;
            }
            num++;
        }
    }

    return num;
}

/*****************************************************************************/
// 	Description : pop up an alert window to show sim wrong status
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:  
/*****************************************************************************/
void MMIAPIPHONE_AlertSimNorOKStatusII(MN_DUAL_SYS_E dual_sys)
{
    MMI_TEXT_ID_T 	text_id = TXT_NULL;
    MMI_WIN_ID_T	alert_win_id = MMIPHONE_ALERT_SIM_NOT_OK_WIN_ID;
    SIM_STATUS_E sim_status = MMIAPIPHONE_GetSimStatus(dual_sys);
    MMI_IMAGE_ID_T image_id = IMAGE_PUBWIN_WARNING;
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    
    if(sim_status >= SIM_STATUS_MAX)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_AlertSimNorOKStatus:error simStatus=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6359_112_2_18_2_45_13_118,(uint8*)"d", sim_status);
        return;
    }
    
    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_AlertSimNorOKStatus:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6365_112_2_18_2_45_13_119,(uint8*)"d", dual_sys);
        return;
    }
    
    if (MMIAPISET_GetFlyMode())
    {
        MMIAPISET_AlertFlyMode();
    }    
    else if(!MMIAPIPHONE_GetSimExistedStatus(dual_sys))
    {
        sim_status = SIM_STATUS_NO_SIM;        
        text_id = s_sim_status_string[sim_status];
        
        if (text_id != TXT_NULL)
        {
            MMIPUB_OpenAlertWinByTextId(PNULL,text_id,TXT_NULL,image_id,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }
    }
    else if (!(e_dualsys_setting & (0x01 << dual_sys)))
    {
        text_id = TXT_SIM_DEACTIVATED;

        MMIPUB_OpenAlertWinByTextId(PNULL,text_id,TXT_NULL,image_id,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
    }
    else
    {
        text_id = s_sim_status_string[sim_status];
        image_id = IMAGE_PUBWIN_WARNING;

        if (TXT_NULL == text_id)
        {
            if (MMIPHONE_PS_POWER_ON_NORMAL != s_ps_ready[dual_sys])
            {
                text_id = TXT_SYS_WAITING_AND_TRY_LATER;
                image_id = IMAGE_PUBWIN_WAIT;
            }
        }
        
        if (text_id != TXT_NULL)
        {
            MMIPUB_OpenAlertWinByTextId(PNULL,text_id,TXT_NULL,image_id,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }
    }
}

/*****************************************************************************/
// 	Description : pop up an alert window to show sim wrong status
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:  
/*****************************************************************************/
void MMIAPIPHONE_AlertSimNorOKStatus(void)
{
    MMI_TEXT_ID_T 	text_id = TXT_NULL;
    MMI_WIN_ID_T	alert_win_id = MMIPHONE_ALERT_SIM_NOT_OK_WIN_ID;
    SIM_STATUS_E sim_status = SIM_STATUS_OK;
    MMI_IMAGE_ID_T image_id = IMAGE_PUBWIN_WARNING;
    uint16 sys_ok = MN_DUAL_SYS_1;
    
    if (MMIAPISET_GetFlyMode())
    {
        MMIAPISET_AlertFlyMode();
    }
    else
    {
        uint32 sys_num = MMIAPISET_GetMultiSysSettingNum(&sys_ok, 1) ;
        uint32 exist_sim_num = MMIAPIPHONE_GetSimExistedNum(PNULL,0);
        
        image_id = IMAGE_PUBWIN_WARNING;
        
        if (0 == sys_num)
        {
             ///没有可用的SIM 卡，可能是确实没有卡存在，也可能是待机卡为零
             if((0 < exist_sim_num )/* && ( MMI_DUAL_SYS_MAX > exist_sim_num)*/)
             {
                text_id = TXT_INSERT_SIM_AND_INFO;
                MMIPUB_OpenAlertWinByTextId(PNULL,text_id,TXT_NULL,image_id,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);

                return; /// return directly!!
             }
             else
             {
                sim_status = SIM_STATUS_NO_SIM; //// call text id :insert_sim or insert_sim_and_info
             }
        }
        else
        {
            sim_status = MMIAPIPHONE_GetSimStatus((MN_DUAL_SYS_E)sys_ok);
        }
        
        if(sim_status <= SIM_STATUS_MAX)
        {
            text_id = s_sim_status_string[sim_status];
            
            if (text_id != TXT_NULL)
            {
                MMIPUB_OpenAlertWinByTextId(PNULL,text_id,TXT_NULL,image_id,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
            }
            else
            {
                if (MMIPHONE_PS_POWER_ON_NORMAL != s_ps_ready[sys_ok])
                {
                    text_id = TXT_SYS_WAITING_AND_TRY_LATER;
                    image_id = IMAGE_PUBWIN_WAIT;
                    
                    MMIPUB_OpenAlertWinByTextId(PNULL,text_id,TXT_NULL,image_id,&alert_win_id,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
                }
            }
        }
        else
        {
            //SCI_TRACE_LOW:"MMIAPIPHONE_AlertSimNorOKStatus err sim status: %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6474_112_2_18_2_45_13_120,(uint8*)"d", sim_status);
        }
    }
}


/*****************************************************************************/
// 	Description : Is Pin2 Required ACM
//	Global resource dependence : 
//  Author:bruce.chi
//	Note:  
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsPin2RequiredACM(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsPin2RequiredACM err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6488_112_2_18_2_45_13_121,(uint8*)"d", dual_sys);
        return FALSE;
    }    
    return s_sim_status[dual_sys].is_pin2_Required_acm;
}

/*****************************************************************************/
// 	Description : Is Pin2 Required ACM Max
//	Global resource dependence : 
//  Author:bruce.chi
//	Note:  
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsPin2RequiredACMMax(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsPin2RequiredACMMax err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6502_112_2_18_2_45_13_122,(uint8*)"d", dual_sys);
        return FALSE;
    }    
    
    return s_sim_status[dual_sys].is_pin2_required_acm_max;
}

/*****************************************************************************/
// 	Description : Is Pin2 Required Puct
//	Global resource dependence : 
//  Author:bruce.chi
//	Note:  
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsPin2RequiredPuct(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsPin2RequiredPuct err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6517_112_2_18_2_45_13_123,(uint8*)"d", dual_sys);
        return FALSE;
    }    
    
    return s_sim_status[dual_sys].is_pin2_Required_puct;
}

/*****************************************************************************/
// 	Description : Is aoc support
//	Global resource dependence : 
//  Author:bruce.chi
//	Note:  
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_IsAocSupport(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsAocSupport err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6532_112_2_18_2_45_13_124,(uint8*)"d", dual_sys);
        return FALSE;
    }    

#ifdef MMI_DUALMODE_ENABLE
    {
        MNSIM_AOC_T aoc_status = {0};

        aoc_status = MNSIM_GetAocEx(dual_sys);

        //SCI_TRACE_LOW:"MMIAPIPHONE_IsAocSupport aoc_is_support=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6542_112_2_18_2_45_13_125,(uint8*)"d", aoc_status.aoc_is_support);

        return aoc_status.aoc_is_support;
    }
#else
    return s_sim_status[dual_sys].is_aoc_support;
#endif
}

/*****************************************************************************/
// 	Description : get plmn status
//	Global resource dependence : 
//  Author:bruce.chi
//	Note:  
/*****************************************************************************/
PUBLIC MN_PHONE_PLMN_STATUS_E MMIAPIPHONE_GetPlmnStatus(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GetPlmnStatus err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6559_112_2_18_2_45_13_126,(uint8*)"d", dual_sys);
        return PLMN_NO_SERVICE;
    }    
    
    return g_service_status[dual_sys].plmn_status;
}


/*****************************************************************************/
// 	Description : create input pin window
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
LOCAL MMI_RESULT_E CreateInputPinWindow(MnSimGetPinIndS sim_get_pin_ind)
{    
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    MMI_WIN_ID_T alert_win = PHONE_PIN_ALERT_WIN_ID;
    
    //SCI_TRACE_LOW:"mmiphone:CreateInputPinWindow dual_sys=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6576_112_2_18_2_45_13_127,(uint8*)"d", sim_get_pin_ind.dual_sys);
    
    if (MNSIM_PIN1 == sim_get_pin_ind.pin_num)
    {
        MMK_CloseWin(alert_win);
        
        if (sim_get_pin_ind.pin_is_blocked)//PIN被锁，要求输入PUK
        {
            if (MMK_IsOpenWin(PHONE_PUK_INPUT_WIN_ID))
            {
                //提示错误窗口,PUK错误!
                MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PUK_ERROR,TXT_NULL,IMAGE_PUBWIN_WARNING,&alert_win,PNULL,MMIPUB_SOFTKEY_ONE,HandlePinBlocked);
                
                //清空password
                GUIEDIT_ClearAllStr(PHONE_PUK_EDITBOX_CTRL_ID);
            }
            else
            {
                //提示错误窗口,PIN被锁!
                MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PIN_BLOCKED,TXT_NULL,IMAGE_PUBWIN_WARNING,&alert_win,PNULL,MMIPUB_SOFTKEY_ONE,HandlePinBlocked);
            }
        }
        else//PIN没有被锁，要求输入PIN
        {            
            if (MMK_IsOpenWin(PHONE_PIN_INPUT_WIN_ID))
            {
                //提示错误窗口,PIN 错误!
                MMIPUB_OpenAlertWinByTextId(PNULL,TXT_PIN_ERROR,TXT_NULL,IMAGE_PUBWIN_WARNING,&alert_win,PNULL,MMIPUB_SOFTKEY_ONE,HandlePin1Maxtimes);

                //清空password
                GUIEDIT_ClearAllStr(PHONE_PIN_EDITBOX_CTRL_ID);
            }
            else
            {
                // 打开输入PIN的界面
                MMK_CreatePubEditWin((uint32*)PHONE_PIN_INPUTING_WIN_TAB,PNULL);
            }
        }
    }
    
    // 关闭等待窗口
    MMK_CloseWin(PHONE_PIN_WAIT_WIN_ID);
    
    return (result);
}

#if defined(MMI_MULTI_SIM_SYS_SINGLE) || defined(MMI_MULTI_SIM_SYS_DUAL)
/*****************************************************************************/
// Description : get dual sim status
// Global resource dependence : 
// Author: bruce.chi
// Note: This API could only be used in SINGLE-SIM or DUAL SIM project,
// If in trisim or quad sim project,Please use another api(eg:MMIAPIPHONE_GetSimOkNum...) or other method.
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_GetDualSimStatus(BOOLEAN *is_sim1_ok_ptr, BOOLEAN *is_sim2_ok_ptr)
{
    if(PNULL == is_sim1_ok_ptr  ||PNULL == is_sim2_ok_ptr)
    {
        //SCI_TRACE_LOW:"GetSimAndServiceString: the is_sim1_ok_ptr=%d || is_sim2_ok_ptr =%d "
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6636_112_2_18_2_45_13_128,(uint8*)"dd", is_sim1_ok_ptr, is_sim2_ok_ptr);
        return FALSE;
    }
    
    if (MMIAPISET_GetFlyMode())
    {
        *is_sim1_ok_ptr = FALSE;
        *is_sim2_ok_ptr = FALSE;
    }
    else
    {
        *is_sim1_ok_ptr = MMIAPIPHONE_IsSimAvailable(MN_DUAL_SYS_1);
        
#ifdef MMI_MULTI_SIM_SYS_DUAL
        *is_sim2_ok_ptr = MMIAPIPHONE_IsSimAvailable(MN_DUAL_SYS_2);
#else
        *is_sim2_ok_ptr = MMIAPIPHONE_IsSimAvailable(MN_DUAL_SYS_1);
#endif
    }
    
    //SCI_TRACE_LOW:"MMIAPIPHONE_GetDualSimStatus, is_sim1_ok=%d, is_sim2_ok=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6656_112_2_18_2_45_13_129,(uint8*)"dd", *is_sim1_ok_ptr, *is_sim2_ok_ptr);
    return TRUE;
}
#endif

/*****************************************************************************/
// 	Description : IsNeedSetGPRSAttachMode
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN IsNeedSetGPRSAttachMode(void)
{
    uint32 i = 0;

    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (!s_is_sim_present_ind[i])//(!s_is_power_on_sim_cnf[i] && !s_handle_pin_status.is_handling_pin[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_SetGPRSAttachMode
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetGPRSAttachMode(void)
{
#ifndef WIN32
    uint32 i =0;

    if (!IsNeedSetGPRSAttachMode())
    {
        return;
    }

#ifdef MMI_DUALMODE_ENABLE
    //SCI_TRACE_LOW:"MMIAPIENG_GetFtaFlag() = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6695_112_2_18_2_45_13_130,(uint8*)"d", MMIAPIENG_GetFtaFlag());
    //set gprs manual attach mode
#ifdef ENGTD_SUPPORT  
    if (MMIAPIENG_GetFtaFlag() && !MNPHONE_GetGprsAttachMode())
    {
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            MNPHONE_SetGprsAttachModeEx(i, TRUE);
        }
    }
    else
#endif
    {
        //多卡存在或者需要时连接模式
        if (MMIAPISET_GetMultiSysSettingNum(PNULL, 0) > 1
            || (!MMIAPICONNECTION_isGPRSNeedAutoAttach()))
        {
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
            {
                MNPHONE_SetGprsAttachModeEx(i, TRUE);
                MNGPRS_SetEnergySavingMode(i, FALSE);
            }
        }
        else
        {
            MMICONNECTION_GPRS_SETTINGS_E grps_status = MMIAPICONNECTION_GetGPRSStatus();
        
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
            {
                MNPHONE_SetGprsAttachModeEx(i, FALSE);

                if (MMICONNECTION_GPRS_ALWAYS_LC == grps_status)
                {
                    MNGPRS_SetEnergySavingMode(i, TRUE);
                }
                else
                {
                    MNGPRS_SetEnergySavingMode(i, FALSE);
                }
            }
        }
    }
#else
    //set gprs manual attach mode
#ifdef MMI_GPRS_SUPPORT
    if (MMIAPISET_GetMultiSysSettingNum(PNULL, 0) > 1
        || (!MMIAPICONNECTION_isGPRSNeedAutoAttach()))
    {
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            MNPHONE_SetGprsAttachModeEx(i, TRUE);
        }
    }
    else
    {
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            MNPHONE_SetGprsAttachModeEx(i, FALSE);
        }
    }
#else
    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        MNPHONE_SetGprsAttachModeEx(i, TRUE);
    }
#endif
#endif
#endif
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_SetGPRSAttach
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetGPRSAttach(void)
{
    uint16 sys_ok = 0;

    if (MMIAPISET_GetMultiSysSettingNum(&sys_ok, 1) > 1)
    {
        //gprs manual, do nothing
    }
    else
    {
        if(MMIAPICONNECTION_isGPRSSwitchedOff())
        {
            return;
        }
        MNPHONE_GprsAttachEx((MN_DUAL_SYS_E)sys_ok, MN_PHONE_GPRS_ATTACH);
    }
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_SetGPRSDetach
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetGPRSDetach(void)
{
#ifndef WIN32
#ifdef MMI_DUALMODE_ENABLE
#ifdef ENGTD_SUPPORT 
    if (!MMIAPIENG_GetFtaFlag())
#endif
#endif
    {
#if defined(MMI_MULTI_SIM_SYS_SINGLE)
        MNPHONE_GprsDetachEx(MN_DUAL_SYS_1, MN_PHONE_GPRS_DETACH);
#else
        uint32 i =0;
        MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
        
        //SCI_TRACE_LOW:"MMIAPIPHONE_SetGPRSDetach e_dualsys_setting = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6802_112_2_18_2_45_14_131,(uint8*)"d", e_dualsys_setting);

        for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
        {
            if (e_dualsys_setting & (0x01 << i))
            {
                MMIAPIPHONE_SetGPRSDetachEx(i);
            }
        }
#endif
    }
#endif
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_SetGPRSDetachEx
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetGPRSDetachEx(MN_DUAL_SYS_E dual_sys)
{
#ifndef WIN32    
    //SCI_TRACE_LOW:"MMIAPIPHONE_SetGPRSDetachEx dual_sys = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6823_112_2_18_2_45_14_132,(uint8*)"d", dual_sys);

#ifdef MMI_DUALMODE_ENABLE
#ifdef ENGTD_SUPPORT 
    if (!MMIAPIENG_GetFtaFlag())
#endif
#endif
    {
        MNPHONE_GprsDetachEx(dual_sys, MN_PHONE_GPRS_DETACH);
    }
#endif
}

/*****************************************************************************/
// 	Description : start up ps according to setting
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_StartUpPsAccordingToSetting(BOOLEAN is_power_on, BOOLEAN is_init_sim)
{
    uint32 i =0;
    if (is_power_on)
    {
#ifdef MULTI_SIM_GSM_CTA_SUPPORT
        MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
#endif

#ifndef MMI_MULTI_SIM_SYS_SINGLE
        s_query_sim_status = SIM_QUERY_STATUS_START;//开始查询SIM状态
#endif
        s_is_power_flag = TRUE;
        
        //SCI_TRACE_LOW:"MMIAPIPHONE_StartUpPsAccordingToSetting, start power on sim"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6854_112_2_18_2_45_14_133,(uint8*)"");
        
        //设置默认状态
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            MMIPHONE_SetSimStatus(i, SIM_NOT_INITED);

            s_is_power_on_sim_cnf[i] = FALSE;

#ifdef MULTI_SIM_GSM_CTA_SUPPORT
            if (is_init_sim)
            {
                if (e_dualsys_setting & (0x01 << i))
                {
                    MNPHONE_PowerOnSimEx(i, TRUE);
                }
                else
                {
                    MNPHONE_PowerOnSimEx(i, FALSE);
                }
            }
            else
            {
                MNPHONE_PowerOnSimEx(i, FALSE);
            }
#else
            MNPHONE_PowerOnSimEx(i, is_init_sim);
#endif
        }
    }
    else
    {
        MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
        
        //SCI_TRACE_LOW:"MMIAPIPHONE_StartUpPsAccordingToSetting, e_dualsys_setting=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6888_112_2_18_2_45_14_134,(uint8*)"d", e_dualsys_setting);

        MMIAPISET_SetIsQueryCfu(FALSE,MMI_DUAL_SYS_MAX);
        
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            s_is_network_status_ready[i] = FALSE;

            if ((e_dualsys_setting & (0x01 << i))
                && MMIAPIPHONE_GetSimExistedStatus(i))
            {
#ifdef ENGTD_SUPPORT

                MMIAPIPHONE_PowerOnPs(i);
#else
                MMIAPIPHONE_PowerOnPs(i);
#endif
            }
        }

        if (0 == MMIAPISET_GetMultiSysSettingNum(PNULL, 0))//无待机卡
        {
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
            {
                if (e_dualsys_setting & (0x01 << i))
                {
                    MMIAPIPHONE_PowerOnPs((MN_DUAL_SYS_E)i);
                    break;
                }
            }
        }
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : deactive ps according to setting
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_DeactivePsAccordingToSetting(void)
{
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    MN_DUAL_SYS_E i = MN_DUAL_SYS_1;

    MMIAPISET_SetIsQueryCfu(FALSE,MMI_DUAL_SYS_MAX);

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        MAIN_SetIdleRoamingState(i, FALSE);
        MAIN_SetIdleForwardState(i, FALSE);
        MAIN_SetIdleGprsState(i, FALSE);
    }

    //set gprs manual attach mode and start up ps 
    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (e_dualsys_setting & (0x01 << i))
        {
            MMIAPIPHONE_PowerOffPs(i);
        }
        else
        {
            MMIAPIPHONE_SetIsPsDeactiveCnf(i, TRUE);
        }
    }
    return TRUE;
}

/*****************************************************************************/
// 	Description : PowerOnPsForEmerge
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
LOCAL void PowerOnPsForEmerge(MN_DUAL_SYS_E dual_sys)
{
    BOOLEAN  is_fly_mode_on = FALSE;

    if (MMIPHONE_PS_POWER_ON_NORMAL == s_ps_ready[dual_sys]
        ||MMIPHONE_PS_POWER_ON_EMERGE == s_ps_ready[dual_sys] )
    {
        //SCI_TRACE_LOW:"PowerOnPsForEmerge, s_ps_ready[%d]=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6968_112_2_18_2_45_14_135,(uint8*)"dd", dual_sys, s_ps_ready[dual_sys]);

        return;
    }

    is_fly_mode_on = MMIAPISET_GetFlyMode();

    if (is_fly_mode_on)
    {
        //SCI_TRACE_LOW:"PowerOnPsForEmerge Fly mode!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_6977_112_2_18_2_45_14_136,(uint8*)"");

        return;
    }
    
    s_ps_ready[dual_sys] = MMIPHONE_PS_POWER_ON_EMERGE;
    s_is_network_status_ready[dual_sys] = FALSE;

    MMIAPIPHONE_SetIsPsReady(dual_sys, FALSE);
    
    MNPHONE_PowerOnPsEx(dual_sys);
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_PowerOnPs
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
void MMIAPIPHONE_PowerOnPs(MN_DUAL_SYS_E dual_sys)
{
    BOOLEAN  is_fly_mode_on = FALSE;

    if (MMIPHONE_PS_POWER_ON_NORMAL == s_ps_ready[dual_sys])
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_PowerOnPs, dual_sys=%d ps is already"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7000_112_2_18_2_45_14_137,(uint8*)"d", dual_sys);

        return;
    }

    is_fly_mode_on = MMIAPISET_GetFlyMode();

    if (is_fly_mode_on)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_PowerOnPs Fly mode!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7009_112_2_18_2_45_14_138,(uint8*)"");

        return;
    }
    
    s_ps_ready[dual_sys] = MMIPHONE_PS_POWER_ON_NORMAL;
    s_is_network_status_ready[dual_sys] = FALSE;

    MMIAPIPHONE_SetIsPsReady(dual_sys, FALSE);

    MNPHONE_PowerOnPsEx(dual_sys);

    MMIAPIPHONE_SetGPRSAttachMode();
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_PowerOffPs
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
void MMIAPIPHONE_PowerOffPs(MN_DUAL_SYS_E dual_sys)
{
    //SCI_TRACE_LOW:"MMIAPIPHONE_PowerOffPs, dual_sys=%d,s_ps_ready=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7030_112_2_18_2_45_14_139,(uint8*)"dd", dual_sys, s_ps_ready[dual_sys]);

    if (MMIPHONE_PS_POWER_ON_NONE == s_ps_ready[dual_sys])//ps not ready
    {
        return;
    }

    s_ps_ready[dual_sys] = MMIPHONE_PS_POWER_ON_NONE;
    
    s_is_network_status_ready[dual_sys] = FALSE;

    MAIN_SetIdleRoamingState(dual_sys, FALSE);
    MAIN_SetIdleForwardState(dual_sys, FALSE);
    MAIN_SetIdleGprsState(dual_sys, FALSE);
    
    MMIAPIPHONE_SetIsPsDeactiveCnf(dual_sys, FALSE);
#if defined(MMI_SMSCB_SUPPORT)
    ClearCbEnable(dual_sys);
#endif
    MNPHONE_PowerOffPsEx(dual_sys);
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_SetIsPsDeactiveCnf
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetIsPsDeactiveCnf(MN_DUAL_SYS_E dual_sys, BOOLEAN is_ps_deactive_cnf)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_SetIsPsDeactiveCnf err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7060_112_2_18_2_45_14_140,(uint8*)"d", dual_sys);
        return;
    }
    
    s_is_ps_deactive_cnf[dual_sys] = is_ps_deactive_cnf;
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_GetIsPsDeactiveCnf
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GetIsPsDeactiveCnf(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GetIsPsDeactiveCnf err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7075_112_2_18_2_45_14_141,(uint8*)"d", dual_sys);
        return FALSE;
    }
    
    return s_is_ps_deactive_cnf[dual_sys];
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_SetIsPsReady
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetIsPsReady(MN_DUAL_SYS_E dual_sys, BOOLEAN is_ps_ready)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_SetIsPsReady err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7090_112_2_18_2_45_14_142,(uint8*)"d", dual_sys);
        return;
    }
    
    s_is_ps_ready[dual_sys] = is_ps_ready;
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_GetIsPsReady
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GetIsPsReady(MN_DUAL_SYS_E dual_sys)
{
    if(MMI_DUAL_SYS_MAX  <= dual_sys )
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_GetIsPsReady err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7105_112_2_18_2_45_14_143,(uint8*)"d", dual_sys);
        return FALSE;
    }
    
    return s_is_ps_ready[dual_sys];
}

/*****************************************************************************/
// 	Description : get current handling Pin Ind sim id(response)
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMIAPIPHONE_GetCurResponsePinSimID(void)
{
    //SCI_TRACE_LOW:"MMIAPIPHONE_GetCurResponsePinSimID:sim_id=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7118_112_2_18_2_45_14_144,(uint8*)"d", s_handle_pin_status.cur_handling_sim_id);
    
    return s_handle_pin_status.cur_handling_sim_id;
}

/*****************************************************************************/
// 	Description : get current handling sim id(operate pin)
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMIAPIPHONE_GetCurHandlingPinSimID(void)
{
    //SCI_TRACE_LOW:"MMIAPIPHONE_GetCurHandlingPinSimID:sim_id=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7129_112_2_18_2_45_14_145,(uint8*)"d", s_cur_handling_sim_id);
    return s_cur_handling_sim_id;
}

/*****************************************************************************/
// 	Description : set current handling sim id(operate pin)
//	Global resource dependence : 
//  Author:
//	Note:  
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetCurHandlingPinSimID(MN_DUAL_SYS_E  cur_sim_id)
{
    s_cur_handling_sim_id = cur_sim_id;
}

/*****************************************************************************/
//     Description : 
//    Global resource dependence : 
//  Author: maggie ren
//    Note:
/*****************************************************************************/
void MMIAPIPHONE_InitTDOrGsm(void)
{
#ifdef MMI_DUALMODE_ENABLE
#ifndef WIN32
    MN_GMMREG_RAT_E configue_rat = MNPHONE_GetConfiguredRAT();

    //SCI_TRACE_LOW:"MMIAPIPHONE_InitTDOrGsm configue_rat=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7152_112_2_18_2_45_14_146,(uint8*)"d", configue_rat);

    switch(configue_rat)
    {
    case MN_GMMREG_RAT_GPRS:
        s_is_TD = MMI_GMMREG_RAT_GPRS;
        break;

    case MN_GMMREG_RAT_TD:
        s_is_TD = MMI_GMMREG_RAT_TD;
        break;

    default:
        s_is_TD = MMI_GMMREG_RAT_UNKNOWN;
        break;
    }

    MAIN_UpdateIdleSignalState();
#endif
#endif
}

/*****************************************************************************/
//     Description : 
//    Global resource dependence : 
//  Author: maggie ren
//    Note:
/*****************************************************************************/
void MMIAPIPHONE_TDOrGsm(MnNetworkStatusIndS     network_status_ind)
{
#ifdef  MMI_DUALMODE_ENABLE
    switch(network_status_ind.rat) 
    {
    case MN_GMMREG_RAT_GPRS:
        s_is_TD = MMI_GMMREG_RAT_GPRS;
        break;
        
    case MN_GMMREG_RAT_3G:
        s_is_TD = MMI_GMMREG_RAT_3G;
        break;
        
    case MN_GMMREG_RAT_UNKNOWN:
        s_is_TD = MMI_GMMREG_RAT_UNKNOWN;
        break;
        
    default:
        break;
    }
#endif
}
/*****************************************************************************/
//     Description : 
//    Global resource dependence : 
//  Author: maggie ren
//    Note:
/*****************************************************************************/
MMI_GMMREG_RAT_E MMIAPIPHONE_GetTDOrGsm(void)
{
#ifdef MMI_DUALMODE_ENABLE
    return s_is_TD;
#else
    return MMI_GMMREG_RAT_GPRS;
#endif
}


/*****************************************************************************/
// 	Description :get current service status
//	Global resource dependence : 
//  Author: kelly.li
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_GetCurServiceStatus(
                                            MN_DUAL_SYS_E dual_sys,
                                            PHONE_SERVICE_STATUS_T *status_ptr
                                            )
{
    SCI_MEMCPY(status_ptr,&g_service_status[dual_sys],sizeof(PHONE_SERVICE_STATUS_T));
}

/*****************************************************************************/
// 	Description :get current service status
//	Global resource dependence : 
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
PUBLIC PHONE_SERVICE_STATUS_T* MMIAPIPHONE_GetServiceStatus(void)
{
    return g_service_status;
}


/*****************************************************************************/
//     Description : 
//    Global resource dependence : 
//  Author: maggie ren
//    Note: modify by yongwei.he 20070621
/*****************************************************************************/
void MMIAPIPHONE_StopSTTDOrGsm(void)
{
//macro replace
//#ifdef MMI_STREAMING_ENABLE
#ifdef STREAMING_SUPPORT_NEXTREAMING
    MMI_WIN_ID_T  focus_win_id;
    focus_win_id = MMK_GetFocusWinId();
    
    if(MMIST_MAIN_PLAY_WIN_ID == focus_win_id)
    {
        MMIAPIST_Close();
    }
    for (focus_win_id = MMIST_MAIN_PLAY_WIN_ID;focus_win_id < MMIST_MAX_WIN_ID;focus_win_id++)
    {
        MMK_CloseWin(focus_win_id);    
    }
#endif
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPresentInd(
                                     DPARAM param
                                     )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MnSimPresentIndS sim_present_ind = *(MnSimPresentIndS *)param;

    if(sim_present_ind.dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandleSimPresentInd:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7263_112_2_18_2_45_14_147,(uint8*)"d", sim_present_ind.dual_sys);
        return MMI_RESULT_FALSE;
    }

    //SCI_TRACE_LOW:"HandleSimPresentInd:dual_sys:%d, sim_sms_num:%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7267_112_2_18_2_45_14_148,(uint8*)"dd", sim_present_ind.dual_sys, sim_present_ind.sim_sms_num);

    s_is_sim_present_ind[sim_present_ind.dual_sys] = TRUE;

    MMIAPISMS_SetMaxSimSmsNum(sim_present_ind.dual_sys, sim_present_ind.sim_sms_num);

    MMIAPIPHONE_SetSimExistedStatus(sim_present_ind.dual_sys, sim_present_ind.is_sim_present);

    MMIAPIPHONE_SetGPRSAttachMode();

    return result;
}

#ifdef MMI_LDN_SUPPORT
/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleLdnReadyInd(
                                     DPARAM param
                                     )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MnLdnReadyIndS ldn_ready_ind = *(MnLdnReadyIndS *)param;

    //SCI_TRACE_LOW:"HandleLdnReadyInd:dual_sys:%d,is_lnd_ok=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7292_112_2_18_2_45_15_149,(uint8*)"dd", ldn_ready_ind.dual_sys, ldn_ready_ind.is_lnd_ok);
    
    MMIAPICL_InitCallInfo(ldn_ready_ind.dual_sys);

    return result;
}
#endif

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSimPlugInInd(
                                     DPARAM param
                                     )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MnSimPlugInIndS sim_plug_in_ind = *(MnSimPlugInIndS *)param;

    if(sim_plug_in_ind.dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"HandleSimPlugInInd:error dual_sys=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7313_112_2_18_2_45_15_150,(uint8*)"d", sim_plug_in_ind.dual_sys);
        return MMI_RESULT_FALSE;
    }
    //SCI_TRACE_LOW:"HandleSimPlugInInd:dual_sys:%d,"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7316_112_2_18_2_45_15_151,(uint8*)"d", sim_plug_in_ind.dual_sys);
    
    MMIPHONE_SetSimStatus(sim_plug_in_ind.dual_sys, SIM_NOT_INITED);

    MMIAPIPHONE_SetSimExistedStatus(sim_plug_in_ind.dual_sys, TRUE);

    NotifySIMPlugInd(sim_plug_in_ind.dual_sys, MMIPHONE_NOTIFY_SIM_PLUG_IN);

    return result;
}

/*****************************************************************************/
// 	Description : Handle SIM Plug Alert Win Msg
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSIMPlugAlertWinMsg(
                                                                                              MMI_WIN_ID_T win_id,
                                                                                              MMI_MESSAGE_ID_E 	msg_id,
                                                                                              DPARAM param
                                                                                              )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    LOCAL MN_DUAL_SYS_E plug_sim_id = MN_DUAL_SYS_1;
    LOCAL MMIPHONE_SIM_PLUG_EVENT_E plug_event = MMIPHONE_NOTIFY_SIM_PLUG_MAX;
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        plug_sim_id = s_plug_sim_id;
        plug_event = s_plug_event;
        result = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
        break;

    case MSG_CLOSE_WINDOW:
        if (plug_sim_id == s_plug_sim_id && plug_event == s_plug_event)
        {
            MMIAPIPHONE_SetStandByMode(s_plug_sim_id, s_plug_event);
        }

        result = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
        break;
        
    case MSG_APP_CANCEL:
    case MSG_APP_RED:
        MMK_CloseWin(win_id);
        break;
        
    default:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
//  Description : Notify SIM Plug Ind
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN NotifySIMPlugInd(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E notify_event)
{
    MMIPHONE_SIM_PLUG_NOTIFY_LIST_T *cursor = PNULL;
    MMI_WIN_ID_T alert_win_id = PHONE_SIM_PLUG_ALERT_WIN_ID;
    MMI_WIN_PRIORITY_E win_priority = WIN_ONE_LEVEL;

    //SCI_TRACE_LOW:"NotifySIMPlugInd dual_sys=%d, notify_event=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7381_112_2_18_2_45_15_152,(uint8*)"dd", dual_sys, notify_event);

    if (dual_sys >= MMI_DUAL_SYS_MAX || notify_event >= MMIPHONE_NOTIFY_SIM_PLUG_MAX)
    {
        return FALSE;
    }

    if (!s_is_power_flag)
    {
        //SCI_TRACE_LOW:"NotifySIMPlugInd Intial not start, donot handle!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7390_112_2_18_2_45_15_153,(uint8*)"");

        return FALSE;
    }

    MMIDEFAULT_TurnOnBackLight();

    s_plug_sim_id = dual_sys;
    s_plug_event = notify_event;

    MMK_CloseWin(alert_win_id);

    if (MMK_IsOpenWin(PHONE_STARTUP_NORMAL_WIN_ID)
        || MMK_IsOpenWin(PHONE_POWER_OFF_WIN_ID))
    {
        win_priority = WIN_ONE_LEVEL;
    }
    else
    {
        win_priority = WIN_THREE_LEVEL;
    }

    if (MMIPHONE_NOTIFY_SIM_PLUG_IN == notify_event)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL, TXT_SIM_PLUG_IN, TXT_NULL, IMAGE_PUBWIN_WARNING, &alert_win_id, &win_priority, MMIPUB_SOFTKEY_ONE, HandleSIMPlugAlertWinMsg);
    }
    else if (MMIPHONE_NOTIFY_SIM_PLUG_OUT == notify_event)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL, TXT_SIM_PLUG_OUT, TXT_NULL, IMAGE_PUBWIN_WARNING, &alert_win_id, &win_priority, MMIPUB_SOFTKEY_ONE, HandleSIMPlugAlertWinMsg);
    }

    cursor = s_mmiphone_sim_plug_subscriber_list_ptr;
    while(PNULL != cursor)
    {
        cursor->notify_info.notify_func(dual_sys, notify_event, NULL);
        cursor = cursor->next;
    }

    return TRUE;
}

/*****************************************************************************/
// 	Description : register SIM Plug Notify info
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_RegisterSIMPlugNotifyInfo(MMIPHONE_SIM_PLUG_NOTIFY_INFO_T *notify_info_ptr)
{
    BOOLEAN return_val = FALSE;
    MMIPHONE_SIM_PLUG_NOTIFY_LIST_T *cursor = PNULL;

    if(PNULL != notify_info_ptr)
    {
        if(PNULL == s_mmiphone_sim_plug_subscriber_list_ptr)
        {
            s_mmiphone_sim_plug_subscriber_list_ptr = SCI_ALLOC_APP(sizeof(MMIPHONE_SIM_PLUG_NOTIFY_LIST_T));
            SCI_MEMSET(s_mmiphone_sim_plug_subscriber_list_ptr, 0, sizeof(MMIPHONE_SIM_PLUG_NOTIFY_LIST_T));
            SCI_MEMCPY(&(s_mmiphone_sim_plug_subscriber_list_ptr->notify_info), notify_info_ptr, sizeof(MMIPHONE_SIM_PLUG_NOTIFY_INFO_T));
        }
        else
        {
            cursor = s_mmiphone_sim_plug_subscriber_list_ptr;
            while(PNULL != cursor->next)
            {
                if(cursor->notify_info.module_id == notify_info_ptr->module_id)
                {
                    return FALSE;   //already register
                }
                cursor = cursor->next;
            }
            if(cursor->notify_info.module_id == notify_info_ptr->module_id)
            {
                return FALSE;   //already register
            }
            cursor->next = SCI_ALLOC_APP(sizeof(MMIPHONE_SIM_PLUG_NOTIFY_LIST_T));
            SCI_MEMSET(cursor->next, 0, sizeof(MMIPHONE_SIM_PLUG_NOTIFY_LIST_T));
            SCI_MEMCPY(&(cursor->next->notify_info), notify_info_ptr, sizeof(MMIPHONE_SIM_PLUG_NOTIFY_INFO_T));
        }

        return_val = TRUE;
    }

    return return_val;
}

/*****************************************************************************/
// 	Description : Unregister SIM Plug Notify info
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_UnregisterSIMPlugNotifyInfo(uint32 module_id)
{
    MMIPHONE_SIM_PLUG_NOTIFY_LIST_T *cursor = PNULL;
    MMIPHONE_SIM_PLUG_NOTIFY_LIST_T *cursor_free = PNULL;

    cursor = s_mmiphone_sim_plug_subscriber_list_ptr;

    if(PNULL != cursor)
    {
        if(cursor->notify_info.module_id == module_id)
        {
            s_mmiphone_sim_plug_subscriber_list_ptr = cursor->next;
            SCI_FREE(cursor);
        }
        else
        {
            while(PNULL != cursor->next)
            {
                if (cursor->next->notify_info.module_id == module_id)
                {
                    cursor_free = cursor->next;
                    cursor->next = cursor_free->next;
                    SCI_FREE(cursor_free);
                    break;
                }
                cursor = cursor->next;
            }
        }
    }
}

/*****************************************************************************/
// 	Description : Power On SIM
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_PowerOnSIM(MN_DUAL_SYS_E dual_sys)
{
    //SCI_TRACE_LOW:"MMIAPIPHONE_PowerOnSIM, start power on sim dual_sys=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7515_112_2_18_2_45_15_154,(uint8*)"d", dual_sys);

#ifndef MMI_MULTI_SIM_SYS_SINGLE
    s_query_sim_status = SIM_QUERY_STATUS_START;//开始查询SIM状态
#endif
    s_is_power_on_sim_cnf[dual_sys] = FALSE;
    
    MMIPHONE_SetSimStatus(dual_sys, SIM_NOT_INITED);

    MNPHONE_PowerOnSimEx(dual_sys, TRUE);
}

/*****************************************************************************/
// 	Description : Deactive PS
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_DeactivePS(MN_DUAL_SYS_E dual_sys)
{
    //SCI_TRACE_LOW:"MMIAPIPHONE_DeactivePS, dual_sys=%d,s_ps_ready=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7533_112_2_18_2_45_15_155,(uint8*)"dd", dual_sys, s_ps_ready);

    s_ps_ready[dual_sys] = MMIPHONE_PS_POWER_ON_NONE;
    s_is_network_status_ready[dual_sys] = FALSE;
    MMIPHONE_SetSimStatus(dual_sys, SIM_STATUS_NO_SIM);
    
    MMIAPIPHONE_SetIsPsDeactiveCnf(dual_sys, FALSE);
#if defined(MMI_SMSCB_SUPPORT)
    ClearCbEnable(dual_sys);
#endif
    MNPHONE_DeactivePsEx(dual_sys);
}

/*****************************************************************************/
// 	Description : HandleSetStandByModeQueryWinMsg
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSetStandByModeQueryWinMsg( 
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E    msg_id, 
                                           DPARAM              param
                                           )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    switch(msg_id)
    {
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        MMIAPISET_OpenMultiSimStandbySettingsWindow();

        MMK_CloseWin(win_id);
        break;
        
    case MSG_APP_RED:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        result = MMIPUB_HandleQueryWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
//  Description : HandleSetStandbyMdoeWaitingWindow
//  Global resource dependence : 
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSetStandbyMdoeWaitingWindow(
                                         MMI_WIN_ID_T    win_id, // ???ID
                                         MMI_MESSAGE_ID_E   msg_id, // ???????ID
                                         DPARAM             param   // ???????
                                         )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    
    //SCI_TRACE_LOW:"HandleSetStandbyMdoeWaitingWindow msg_id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7595_112_2_18_2_45_15_156,(uint8*)"d", msg_id);
    
    switch (msg_id)
    {
    case MSG_CTL_OK:
    case MSG_APP_OK:
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:       
        break;

    case MSG_APP_RED:
        break;

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone.
    case MSG_APP_FLIP:
        MMIDEFAULT_HandleFlipKey(FALSE);
        break;
#endif
        
    default:
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL void OpenSetStandByModeWaiting(void)
{
    MMI_WIN_PRIORITY_E win_priority = WIN_ONE_LEVEL;
    MMI_WIN_ID_T wait_win_id = PHONE_SET_STANDBY_MODE_WAITING_WIN_ID;
    MMI_STRING_T wait_text = {0};

    MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);

    if (MMIAPICC_IsInState(CC_IN_CALL_STATE)
        || MMIAPISET_IsNetworkConnecting()
#ifdef BROWSER_SUPPORT
        || MMIAPIBROWSER_IsRunning()
#endif
        )
    {
        win_priority = WIN_LOWEST_LEVEL;
    }
    else
    {
        win_priority = WIN_ONE_LEVEL;
    }

    MMIPUB_OpenWaitWin(1, &wait_text, PNULL, PNULL, wait_win_id, IMAGE_NULL,
                                        ANIM_PUBWIN_WAIT, win_priority, MMIPUB_SOFTKEY_NONE, HandleSetStandbyMdoeWaitingWindow);

    MMK_SetWinDisplayStyleState(wait_win_id, WS_DISABLE_RETURN_WIN, TRUE);
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
LOCAL void ImplementStandByModeAsync(void)
{
    //SCI_TRACE_LOW:"ImplementStandByModeAsync s_plug_sim_id=%d, s_plug_event=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7662_112_2_18_2_45_15_157,(uint8*)"dd", s_plug_sim_id, s_plug_event);

    if (s_plug_sim_id >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    if (MMK_IsOpenWin(PHONE_SET_STANDBY_MODE_WAITING_WIN_ID))
    {
        switch (s_plug_event)
        {
        case MMIPHONE_NOTIFY_SIM_PLUG_IN:
            MMIAPIPHONE_PowerOnSIM(s_plug_sim_id);
            break;

        default:
            break;
        }
    }

    MMK_CloseWin(PHONE_SET_STANDBY_MODE_WAITING_WIN_ID);
}

/*****************************************************************************/
// 	Description : set standby mode
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetStandByMode(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E plug_event)
{
    uint32 i = 0;
    BOOLEAN is_need_standby[MMI_DUAL_SYS_MAX] = {0};//是否需要待机
    uint32 sys_standby_num = 0;
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    MMI_WIN_ID_T query_win_id = PHONE_SET_STANDBY_MODE_QUERY_WIN_ID;

    //SCI_TRACE_LOW:"MMIAPIPHONE_SetStandByMode dual_sys=%d, plug_event=%d, e_sys_setting=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7698_112_2_18_2_45_15_158,(uint8*)"ddd",dual_sys, plug_event, e_dualsys_setting);

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (e_dualsys_setting & (0x01 << i))
        {
            is_need_standby[i] = TRUE;
            sys_standby_num ++;
        }
    }

    MMIPUB_CloseQuerytWin(&query_win_id);

    MMK_CloseWin(PHONE_SET_STANDBY_MODE_WAITING_WIN_ID);

    for (i = 0; i < MMI_DUAL_SYS_MAX; i++)
    {
        MMIAPIPHONE_SetIdleGprsState(i);
    }

    switch (plug_event)
    {
    case MMIPHONE_NOTIFY_SIM_PLUG_IN:
        MMIAPISET_SetIsQueryCfu(FALSE,dual_sys);

#ifdef MMI_GPRS_SUPPORT
        if (!MMIAPIPDP_PsIsPdpLinkExist())
        {
            if ((!MMIAPICONNECTION_isGPRSNeedAutoAttach())
                 || (sys_standby_num > 1))
            {
                MMIAPIPHONE_SetGPRSDetach();
            }
        }
#endif

        if (e_dualsys_setting & (0x01 << dual_sys))//待机卡插入
        {
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i ++)
            {
                if ((e_dualsys_setting & (0x01 << i))
                    && !MMIAPIPHONE_GetSimExistedStatus(i)
                    && (dual_sys != i))//关闭不存在SIM卡的PS
                {
                    MMIAPIPHONE_PowerOffPs((MN_DUAL_SYS_E)i);
                }
            }
        }

        OpenSetStandByModeWaiting();

        MMIAPIPHONE_DeactivePS(dual_sys);
        break;

    case MMIPHONE_NOTIFY_SIM_PLUG_OUT:
#ifdef MMI_GPRS_SUPPORT
#ifndef WIN32
        MMIAPIPHONE_SetGPRSAttachMode();

        MMIAPIPDP_ResetPdp(dual_sys);

        if (!MMIAPIPDP_PsIsPdpLinkExist())
        {
            if (1 == MMIAPISET_GetMultiSysSettingNum(PNULL, 0))
            {
                if ((!MMIAPICONNECTION_isGPRSNeedAutoAttach()))
                {
                    MMIAPIPHONE_SetGPRSDetach();
                }
                else
                {
                    MMIAPIPHONE_SetGPRSAttach();
                }
            }
        }
#endif
#endif

        MAIN_SetIdleForwardState(dual_sys, FALSE);
        MAIN_SetIdleRoamingState(dual_sys, FALSE);

        if (MMIAPIPHONE_GetPSReadyNum(PNULL, 0) > 1)
        {
            MMIAPIPHONE_PowerOffPs(dual_sys);
#ifdef QQ_SUPPORT
            MMIAPIQQ_UpdateSimSelect(dual_sys);
#endif
        }

#ifndef MMI_MULTI_SIM_SYS_SINGLE
        if ((1 == sys_standby_num) && is_need_standby[dual_sys])
        {
            MMIPUB_OpenQueryWinByTextId(TXT_SIM_PLUG_OUT_SET_STANDBY_MODE_QUERY, IMAGE_PUBWIN_QUERY, &query_win_id, HandleSetStandByModeQueryWinMsg);
        }
#endif
        break;

    default:
        break;
    }
}

/*****************************************************************************/
// 	Description : according to ps status, judge if wap & mms is available
//	Global resource dependence : 
//  Author: samboo.shen
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsPsServiceAvailble(void)
{
#ifdef WIN32
    return TRUE;
#else
#ifdef MMI_DUALMODE_ENABLE  //
    return TRUE;
#else
    return (ATC_GetModemStatus() != ATC_MODEM_ONDIAL);
#endif
#endif
}

/*****************************************************************************/
// 	Description :get current data service type
//	Global resource dependence : 
//  Author: kelly.li
//	Note:
/*****************************************************************************/
PUBLIC MMIPHONE_DATA_SERVICE_TYPE_E MMIAPIPHONE_GetDataServiceType( MN_DUAL_SYS_E dual_sys )
{
    MMIPHONE_DATA_SERVICE_TYPE_E service_type = NO_DATA_SERVICE_TYPE;
    if (g_service_status[dual_sys].hsdpa_support)
    {
        service_type = HSDPA_DATA_SERVICE_TYPE;
    }
    else if (g_service_status[dual_sys].edge_support)
    {
        service_type = EDGE_DATA_SERVICE_TYPE;
    }
    else if ((MMI_GMMREG_RAT_GPRS_AND_3G == g_service_status[dual_sys].rat)
            ||(MMI_GMMREG_RAT_TD == g_service_status[dual_sys].rat))
    {
        service_type = TD_DATA_SERVICE_TYPE;
    }
    else if(g_service_status[dual_sys].gprs_support)
    {
        service_type = GPRS_DATA_SERVICE_TYPE;
    }
    else
    {
        //service_type = NO_DATA_SERVICE_TYPE;
    }

    //SCI_TRACE_LOW:"MMIAPIPHONE_GetDataServiceType: %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7851_112_2_18_2_45_16_159,(uint8*)"d", service_type);
    return service_type;
}

/*****************************************************************************/
//  Description : Notify Rx level Change
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL void NotifyRxLevelChange(MN_DUAL_SYS_E dual_sys, uint8 rx_level)
{
    LOCAL uint8 s_notify_rx_level[MMI_DUAL_SYS_MAX] = {0};/*lint !e64*/

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"NotifyRxLevelChange error dual_sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7865_112_2_18_2_45_16_160,(uint8*)"d", dual_sys);
        return;
    }

    //SCI_TRACE_LOW:"NotifyRxLevelChange rx_level=%d, s_notify_rx_level=%d, PlmnStatus=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7870_112_2_18_2_45_16_161,(uint8*)"ddd",rx_level, s_notify_rx_level[dual_sys], MMIAPIPHONE_GetPlmnStatus(dual_sys));

    if (0 == rx_level && 0 != s_notify_rx_level[dual_sys])
    {
        if (PLMN_NORMAL_GSM_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys)
            || PLMN_NORMAL_GPRS_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys)
            || PLMN_NORMAL_GSM_GPRS_BOTH == MMIAPIPHONE_GetPlmnStatus(dual_sys))
        {
            NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_UNAVAIL);

            NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_UNAVAIL);
        }
    }
    else if (0 != rx_level && 0 == s_notify_rx_level[dual_sys])
    {
        if (PLMN_NORMAL_GSM_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys))
        {
            NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_AVAIL);
        }
        else if (PLMN_NORMAL_GPRS_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys))
        {
            NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_AVAIL);
        }
        else if(PLMN_NORMAL_GSM_GPRS_BOTH == MMIAPIPHONE_GetPlmnStatus(dual_sys))
        {
            NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_AVAIL);
            NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_AVAIL);
        }
    }

    s_notify_rx_level[dual_sys] = rx_level;
}

/*****************************************************************************/
//  Description : Notify PLMN Status Change
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL void NotifyPlmnStatusChange(MN_DUAL_SYS_E dual_sys, MN_PHONE_PLMN_STATUS_E plmn_status)
{
    LOCAL MN_PHONE_PLMN_STATUS_E s_notify_plmn_status[MMI_DUAL_SYS_MAX] = {0};/*lint !e64*/

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    //SCI_TRACE_LOW:"NotifyPlmnStatusChange plmn_status=%d, s_notify_plmn_status=%d, rssiLevel=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_7917_112_2_18_2_45_16_162,(uint8*)"ddd",plmn_status, s_notify_plmn_status[dual_sys], g_service_status[dual_sys].rssiLevel);

    if (0 < g_service_status[dual_sys].rssiLevel)
    {
        switch (plmn_status)
        {
        case PLMN_NORMAL_GSM_ONLY:
            switch (s_notify_plmn_status[dual_sys])
            {
            case PLMN_NORMAL_GSM_ONLY:
                //plmn status not change
                break;

            case PLMN_NORMAL_GPRS_ONLY:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_UNAVAIL);

                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_AVAIL);
                break;

            case PLMN_NORMAL_GSM_GPRS_BOTH:
                //NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_UNAVAIL);
                break;

            default:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_AVAIL);
                break;
            }
            break;

        case PLMN_NORMAL_GPRS_ONLY:
            switch (s_notify_plmn_status[dual_sys])
            {
            case PLMN_NORMAL_GSM_ONLY:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_UNAVAIL);

                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_AVAIL);
                break;

            case PLMN_NORMAL_GPRS_ONLY:
                //plmn status not change
                break;

            case PLMN_NORMAL_GSM_GPRS_BOTH:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_UNAVAIL);
                break;

            default:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_AVAIL);
                break;
            }
            break;

        case PLMN_NORMAL_GSM_GPRS_BOTH:
            switch (s_notify_plmn_status[dual_sys])
            {
            case PLMN_NORMAL_GSM_ONLY:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_AVAIL);
                break;

            case PLMN_NORMAL_GPRS_ONLY:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_AVAIL);
                break;

            case PLMN_NORMAL_GSM_GPRS_BOTH:
                //plmn status not change
                break;

            default:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_AVAIL);

                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_AVAIL);
                break;
            }
            break;

        default:
            switch (s_notify_plmn_status[dual_sys])
            {
            case PLMN_NORMAL_GSM_ONLY:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_UNAVAIL);
                break;

            case PLMN_NORMAL_GPRS_ONLY:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_UNAVAIL);
                break;

            case PLMN_NORMAL_GSM_GPRS_BOTH:
                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_CS_UNAVAIL);

                NotifyServiceChange(dual_sys, MMIPHONE_NOTIFY_EVENT_PS_UNAVAIL);
                break;

            default:
                //need not notify
                break;
            }
            break;
        }
    }

    s_notify_plmn_status[dual_sys] = plmn_status;
}

/*****************************************************************************/
//  Description : Notify Service Change
//  Global resource dependence : none
//  Author: wancan.you
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN NotifyServiceChange(MN_DUAL_SYS_E dual_sys, MMIPHONE_NOTIFY_EVENT_E notify_event)
{
    MMIPHONE_NOTIFY_LIST_T *cursor = PNULL;

    //SCI_TRACE_LOW:"NotifyServiceChange dual_sys=%d, notify_event=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8028_112_2_18_2_45_16_163,(uint8*)"dd", dual_sys, notify_event);

    if (dual_sys >= MMI_DUAL_SYS_MAX || notify_event >= MMIPHONE_NOTIFY_EVENT_MAX)
    {
        return FALSE;
    }

    cursor = s_mmiphone_subscriber_list_ptr;
    while(PNULL != cursor)
    {
        cursor->notify_info.notify_func(dual_sys, notify_event, NULL);
        cursor = cursor->next;
    }

    return TRUE;
}

/*****************************************************************************/
// 	Description : register PS/CS Notify info
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_RegisterNotifyInfo(MMIPHONE_NOTIFY_INFO_T *notify_info_ptr)
{
    BOOLEAN return_val = FALSE;
    MMIPHONE_NOTIFY_LIST_T *cursor = PNULL;

    if(PNULL != notify_info_ptr)
    {
        if(PNULL == s_mmiphone_subscriber_list_ptr)
        {
            s_mmiphone_subscriber_list_ptr = SCI_ALLOC_APP(sizeof(MMIPHONE_NOTIFY_LIST_T));
            SCI_MEMSET(s_mmiphone_subscriber_list_ptr, 0, sizeof(MMIPHONE_NOTIFY_LIST_T));
            SCI_MEMCPY(&(s_mmiphone_subscriber_list_ptr->notify_info), notify_info_ptr, sizeof(MMIPHONE_NOTIFY_INFO_T));
        }
        else
        {
            cursor = s_mmiphone_subscriber_list_ptr;
            while(PNULL != cursor->next)
            {
                if(cursor->notify_info.module_id == notify_info_ptr->module_id)
                {
                    return FALSE;   //already register
                }
                cursor = cursor->next;
            }
            if(cursor->notify_info.module_id == notify_info_ptr->module_id)
            {
                return FALSE;   //already register
            }
            cursor->next = SCI_ALLOC_APP(sizeof(MMIPHONE_NOTIFY_LIST_T));
            SCI_MEMSET(cursor->next, 0, sizeof(MMIPHONE_NOTIFY_LIST_T));
            SCI_MEMCPY(&(cursor->next->notify_info), notify_info_ptr, sizeof(MMIPHONE_NOTIFY_INFO_T));
        }

        return_val = TRUE;
    }

    return return_val;
}

/*****************************************************************************/
// 	Description : Unregister PS/CS Notify info
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_UnregisterNotifyInfo(uint32 module_id)
{
    MMIPHONE_NOTIFY_LIST_T *cursor = PNULL;
    MMIPHONE_NOTIFY_LIST_T *cursor_free = PNULL;

    cursor = s_mmiphone_subscriber_list_ptr;
    if(PNULL != cursor)
    {
        if(cursor->notify_info.module_id == module_id)
        {
            s_mmiphone_subscriber_list_ptr = cursor->next;
            SCI_FREE(cursor);
        }
        else
        {
            while(PNULL != cursor->next)
            {
                if (cursor->next->notify_info.module_id == module_id)
                {
                    cursor_free = cursor->next;
                    cursor->next = cursor_free->next;
                    SCI_FREE(cursor_free);
                    break;
                }
                cursor = cursor->next;
            }
        }
    }
    
}

/*****************************************************************************/
// 	Description : Is CS Available
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsCSAvail(MN_DUAL_SYS_E dual_sys)
{
    BOOLEAN is_cs_avail = FALSE;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return is_cs_avail;
    }

    if (0 < g_service_status[dual_sys].rssiLevel
        && (PLMN_NORMAL_GSM_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys)
            || PLMN_NORMAL_GSM_GPRS_BOTH == MMIAPIPHONE_GetPlmnStatus(dual_sys))
        && !g_service_status[dual_sys].is_slecting)
    {
        is_cs_avail = TRUE;
    }

    return is_cs_avail;
}

/*****************************************************************************/
// 	Description : Is PS Available
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsPSAvail(MN_DUAL_SYS_E dual_sys)
{
    BOOLEAN is_ps_avail = FALSE;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsPSAvail error dual_sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8157_112_2_18_2_45_17_164,(uint8*)"d", dual_sys);
        return is_ps_avail;
    }

    if (0 < g_service_status[dual_sys].rssiLevel
        && (PLMN_NORMAL_GSM_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys)
            || PLMN_NORMAL_GPRS_ONLY == MMIAPIPHONE_GetPlmnStatus(dual_sys)
            || PLMN_NORMAL_GSM_GPRS_BOTH == MMIAPIPHONE_GetPlmnStatus(dual_sys))
        && !g_service_status[dual_sys].is_slecting)
    {
        is_ps_avail = TRUE;
    }

    return is_ps_avail;
}

#if !defined(MMI_MULTI_SIM_SYS_SINGLE)
/*****************************************************************************/
// 	Description : MMIAPIPHONE_PowerOnPsEx
//	Global resource dependence : 
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_PowerOnPsEx(void)
{
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    MN_DUAL_SYS_E i = MN_DUAL_SYS_1;

    for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (e_dualsys_setting & (0x01 << i))
        {
            if (MMIAPIIDLE_IdleWinIsOpen()
                && s_is_power_on_sim_cnf[i]
                && MMIAPIPHONE_GetSimExistedStatus(i))
            {
                MMIAPIPHONE_PowerOnPs((MN_DUAL_SYS_E)i);
            }
            else
            {
                //SCI_TRACE_LOW:"MMIAPIPHONE_PowerOnPsEx s_is_power_on_sim_cnf[%d]=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8195_112_2_18_2_45_17_165,(uint8*)"dd", i, s_is_power_on_sim_cnf[i]);
            }
        }
    }
}

/*****************************************************************************/
// 	Description : MMIAPIPHONE_GetIsPsReadyBySys
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GetIsPsReadyBySys(MN_DUAL_SYS_E sys_type)
{
    MMISET_DUALSYS_TYPE_E e_dualsys_setting = 0;
    BOOLEAN is_ps_ready = FALSE;

    e_dualsys_setting = MMIAPISET_GetMultiSysSetting();
    if (e_dualsys_setting & (1 << sys_type))
    {
        is_ps_ready = (MMIPHONE_PS_POWER_ON_NONE != s_ps_ready[sys_type]);
    }

    return is_ps_ready;
}
#endif /* !defined(MMI_MULTI_SIM_SYS_SINGLE) */

#ifdef DPHONE_SUPPORT 
/*****************************************************************************/
// 	Description : Get Pin Lock Hook Handfree Status
//	Global resource dependence : 
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
BOOLEAN MMIAPIPHONE_GetPinLockHookHandfreeStatus(void)
{
    return g_pin_lock_hookhandfree;
}

/*****************************************************************************/
// 	Description : Set Pin Lock Hook Handfree Status
//	Global resource dependence : 
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
void MMIAPIPHONE_SetPinLockHookHandfreeStatus(BOOLEAN hook_locked)
{
    g_pin_lock_hookhandfree = hook_locked;
}
#endif

#if defined(MMI_UPDATE_TIME_SUPPORT)
/*****************************************************************************/
// 	Description : 当前SIM是否注册网络 成功
//	Global resource dependence : 
//  Author: xiaoming.ren
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsRegNetSuccess(MN_DUAL_SYS_E cur_sim_index)
{
    BOOLEAN    result = FALSE;
    
    if(cur_sim_index < MMI_DUAL_SYS_MAX)
    {
#ifndef WIN32
        if (s_is_network_status_ready[cur_sim_index])
#endif
        {
            result = TRUE;
        }
    }
    else
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsRegNetSuccess err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8260_112_2_18_2_45_17_166,(uint8*)"d", cur_sim_index);
    }

    return result;
}

/*****************************************************************************/
// 	Description : 判断SIM是否Attach gprs 成功
//	Global resource dependence : 
//  Author: xiaoming.ren
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_IsAttachGprsSuccess(MN_DUAL_SYS_E cur_sim_index)
{
    BOOLEAN    result = FALSE;
    
    if(cur_sim_index >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_IsAttachGprsSuccess err dual sys: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8276_112_2_18_2_45_17_167,(uint8*)"d", cur_sim_index);
        return FALSE;        
    }
    
    if ((PLMN_NORMAL_GPRS_ONLY == g_service_status[cur_sim_index].plmn_status)
        || (PLMN_NORMAL_GSM_GPRS_BOTH == g_service_status[cur_sim_index].plmn_status))
    {
        result = TRUE;
    }
    
    return result;
}
#endif /*MMI_UPDATE_TIME_SUPPORT*/

#if defined(BLUETOOTH_SUPPORT) && defined(BT_BQB_SUPPORT)
//+++ mdy by qing.yu@for cr238585
/*****************************************************************************/
// 	Description : get sim card and plmn service status string text id
//	Global resource dependence : 
//  Author: qing.yu
//	Note:  
/*****************************************************************************/
PUBLIC MMI_TEXT_ID_T MMIAPIPHONE_OpeartorTextID(MN_DUAL_SYS_E dual_sys)
{
    MMI_TEXT_ID_T	idle_str_id = TXT_NULL;
    SIM_STATUS_E sim_status = MMIAPIPHONE_GetSimStatus(dual_sys);

    if(dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //SCI_TRACE_LOW:"MMIAPIPHONE_OpeartorTextID param error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8303_112_2_18_2_45_17_168,(uint8*)"");
        return idle_str_id;
    }

    //if sim error then display sim string in sim_status_string
    if (sim_status < SIM_STATUS_MAX)
    {
        if (sim_status != SIM_STATUS_OK) //sim not ready
        {
            idle_str_id = s_sim_status_string[sim_status];

        }
        else
        {
            idle_str_id = GetNetWorkNameId(
				&g_service_status[dual_sys].plmn,
                g_service_status[dual_sys].plmn_status,
                g_service_status[dual_sys].is_slecting
                );
        }
    }
	return idle_str_id;
}

PUBLIC BOOLEAN MMIAPIPHONE_GetIsNetworkRoaming(void)
{
	MN_DUAL_SYS_E dual_sys = MMIAPISET_GetActiveSim();

    return g_service_status[dual_sys].isRoaming;
}
//--- mdy by qing.yu@for cr238585
#endif

/*****************************************************************************/
// 	Description : Get Update NetworkStatus Win
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:  
/*****************************************************************************/
PUBLIC MMI_HANDLE_T MMIAPIPHONE_GetUpdateNetworkStatusWin(void)
{
    return s_need_update_network_status_win;
}

/*****************************************************************************/
// 	Description : is network selecting
//	Global resource dependence : 
//  Author: bruce.chi
//	Note:  
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_SetNeedUpdateNetworkStatusWin(MMI_HANDLE_T win_handle)
{
    s_need_update_network_status_win = win_handle;    
}

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: wancan.you
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_UpdateNetworkStatusWin(void)
{
    if (MMK_IsFocusWin(s_need_update_network_status_win))
    {
        MMK_PostMsg(s_need_update_network_status_win, MSG_FULL_PAINT, PNULL, 0);
    }
}

#ifdef MMI_SIM_LOCK_SUPPORT
/*****************************************************************************/
// 	Description : 在SIM Lock 解锁次数达到限制的情况下弹出"SIM Restricted"窗口
//	Global resource dependence : 
//  Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSIMLocked(
                                    MMI_WIN_ID_T     win_id, 	// Pin boloked窗口的ID
                                    MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                    DPARAM 		        param		// 相应消息的参数
                                    )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    GUI_POINT_T         dis_point = {0};
    GUI_LCD_DEV_INFO	lcd_dev_info = {0};
    
    lcd_dev_info.lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id = GUI_BLOCK_MAIN;
    
    switch (msg_id)
    {
    case MSG_APP_CANCEL://pin被锁输入界面不允许返回
    case MSG_APP_RED:
        break;

#ifdef FLIP_PHONE_SUPPORT  //close flip ring codes, just open for flip phone.        
    case MSG_APP_FLIP:
        MMIDEFAULT_HandleFlipKey(FALSE);
        break;
#endif

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        GUIRES_DisplayImg(&dis_point,
            PNULL,
            PNULL,
            win_id,
            IMAGE_COMMON_BG,
            &lcd_dev_info);
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
        
    default:
        result = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (result);
}

/*****************************************************************************/
// 	Description : create input sim lock window
//	Global resource dependence : 
//  Author: dave.ruan
//	Note:  
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIPHONE_CreateInputSimLockWindow(MnSimNotReadyIndS sim_not_ready_ind)
{    
    MN_SIM_LOCK_STATUS_T *sim_unlock_data = MMIAPIPHONE_GetSIMUnlockConfigDataPtr();
    MMI_OPERATE_SIM_LOCK_T   *opreate_sim_lock_ptr =SCI_ALLOCAZ(sizeof(MMI_OPERATE_SIM_LOCK_T));
    uint32 unlock_trials = 0;
    uint32 time_out_value = 0;
    int32 left_trials = 0;
    MMI_RESULT_E result =MMI_RESULT_TRUE;
    
    if(PNULL == opreate_sim_lock_ptr)
    {
        //SCI_TRACE_LOW:"mmiphone:CreateInputSimLockWindow opreate_sim_lock_ptr = NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8423_112_2_18_2_45_17_169,(uint8*)"");
        return result;
    }
    
    //SCI_TRACE_LOW:"mmiphone:CreateInputSimLockWindow dual_sys=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8427_112_2_18_2_45_17_170,(uint8*)"d", sim_not_ready_ind.dual_sys);
    
    SCI_MEMSET(opreate_sim_lock_ptr,0x00,sizeof(MMI_OPERATE_SIM_LOCK_T));
    opreate_sim_lock_ptr->dual_sys = sim_not_ready_ind.dual_sys;
    opreate_sim_lock_ptr->lock_type = MMI_SIM_NETWORK_LOCK<<((uint32)sim_not_ready_ind.reason - (uint32)MNSIM_ERROR_SIM_NETWORT_LOCKED);
    opreate_sim_lock_ptr->is_enable = TRUE;
    
    /*remember SIM Lock type in case power off the phone*/
    SCI_MEMSET(&s_sim_lock_operate_value,0x00,sizeof(MMI_OPERATE_SIM_LOCK_T));
    s_sim_lock_operate_value.dual_sys = opreate_sim_lock_ptr->dual_sys;
    s_sim_lock_operate_value.lock_type = opreate_sim_lock_ptr->lock_type;
    s_sim_lock_operate_value.is_enable = opreate_sim_lock_ptr->is_enable;
    
    MMIAPIPHONE_GetSIMLockRemainTrialsTimer(opreate_sim_lock_ptr->lock_type,&unlock_trials,&time_out_value);
    left_trials = sim_unlock_data->max_num_trials - unlock_trials;
    
    /*open sim unlock window*/
    if( 0 != time_out_value && 0 < left_trials)
    {
        if(!MMK_IsOpenWin(PHONE_SIM_LOCK_COUNTDOWN_WIN_ID))
        {
            MMK_CreateWin((uint32*)SIM_LOCK_COUNTDOWN_RUN_WIN_TAB,(ADD_DATA)opreate_sim_lock_ptr);
        }
        return MMI_RESULT_TRUE;//
    }
    else if(!MMIPHONE_GetIsSIMUnlocked() && 0 < left_trials)
    {
        if(!MMK_IsOpenWin( PHONE_SIM_LOCK_PASSWORD_INPUTING_WIN_ID))
        {
            MMK_CreatePubEditWin((uint32*)SIM_LOCK_PASSWORD_INPUTING_WIN_TAB,(ADD_DATA)opreate_sim_lock_ptr);
        }
        return MMI_RESULT_TRUE;//
    }      
    else if(0 >= left_trials)
    {
        MMI_WIN_ID_T alert_win_id = PHONE_SIM_LOCK_ALERT_WIN_ID;
        uint32 alert_time = 0;
        
        MMIPUB_OpenAlertWinByTextId(&alert_time,TXT_SIM_LOCKED,TXT_NULL,
                                                        IMAGE_PUBWIN_WARNING,&alert_win_id,
                                                        PNULL,MMIPUB_SOFTKEY_NONE,HandleSIMLocked);
      
        MMK_SetWinDisplayStyleState(alert_win_id, WS_DISABLE_RETURN_WIN, TRUE);
        SCI_FREE(opreate_sim_lock_ptr);
    }
    else
    {
        SCI_FREE(opreate_sim_lock_ptr);
    }    
    return (result);
}

/*****************************************************************************/
// 	Description : get handle sim lock status ptr for multi sim card
//	Global resource dependence : none
//  Author: dave.ruan
//	Note:
/*****************************************************************************/
PUBLIC MMIPHONE_HANDLE_SIMLOCK_STATUS_T *MMIPHONE_GetHandleSIMLockStatus(void)
{
    return &s_handle_sim_lock_status;
}
/*****************************************************************************/
// 	Description : MMIPHONE_Get SIM lock Config Data Ptr
//	Global resource dependence : none
//  Author: dave.ruan
//	Note:
/*****************************************************************************/
PUBLIC MN_SIM_LOCK_STATUS_T *MMIAPIPHONE_GetSIMUnlockConfigDataPtr(void)
{
    return  &s_sim_unlock_config_data;
}

/*****************************************************************************/
// 	Description : MMIPHONE Update SIM Unlock Data if MMI update sim lock data to MN
//	Global resource dependence : none
//  Author: dave.ruan
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_UpdateSIMUnlockData(void)
{
    return MNPHONE_GetSIMLockStatus(&s_sim_unlock_config_data);
}
#endif

#ifdef MMI_SIM_LANGUAGE_SUPPORT
/*****************************************************************************/
// 	Description : 判断是哪张sim卡做优选语言。
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
PUBLIC MN_DUAL_SYS_E MMIAPIPHONE_GetPreferLangSim(void)
{
    return s_prefer_language_sim;
}
#endif

#ifdef MMI_PREFER_PLMN_SUPPORT
/*****************************************************************************/
// 	Description : Append All Plmn List ...
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_AppendPlmnList(
                                                                                MMI_CTRL_ID_T ctrl_id,                                                                               
                                                                                uint16 index
                                                                                )
{   
    GUILIST_ITEM_DATA_T item_data = {0};
    MN_PLMN_T plmn = {0};
    MMI_STRING_T plmn_name = {0};
    
    wchar plmn_code[MAX_MCC_MNC_DIGIT_DISPLAY +1] = {0};        
    uint16 plmn_code_len = MAX_MCC_MNC_DIGIT_DISPLAY;
    
    //SCI_TRACE_LOW:"MMIAPIPHONE_AppendPlmnList,index=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8534_112_2_18_2_45_18_171,(uint8*)"d",index);
    
    if(index < MAX_NETWORK_NUM -1 )
    {
        plmn.mcc = s_network_nametable[index + 1].mcc;
        plmn.mnc = s_network_nametable[index + 1].mnc;        
        if(s_network_nametable[index+1].mnc >= 100)
        {
            plmn.mnc_digit_num = 3; // mnc 的长度有可能为2 或3
        }
        else
        {
            plmn.mnc_digit_num = 2; // mnc 的长度有可能为2 或3
        }        

        //PLMN NAME Content.
        //get plmn name info    
        MMI_GetLabelTextByLang(s_network_nametable[index+1].name_index, &plmn_name);
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;        
        item_data.item_content[0].item_data.text_buffer.wstr_len = plmn_name.wstr_len;
        item_data.item_content[0].item_data.text_buffer.wstr_ptr = plmn_name.wstr_ptr;

        //PLMN CODE Content .        
        //get plmn code info
        MMIAPIPHONE_GenPLMNDisplay(MMIAPIPHONE_GetTDOrGsm(), &plmn, plmn_code, &plmn_code_len, TRUE);
        item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;        
        item_data.item_content[1].item_data.text_buffer.wstr_len = plmn_code_len;
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = plmn_code;

        //set softkey
        item_data.softkey_id[0] = TXT_COMMON_OK;
        item_data.softkey_id[1] = TXT_NULL;
        item_data.softkey_id[2] = STXT_RETURN;

        GUILIST_SetItemData( ctrl_id, &item_data, index );    
        GUILIST_SetOwnSofterKey(ctrl_id, TRUE);
    }
}

/*****************************************************************************/
// 	Description : Append prefer plmn List ...
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPIPHONE_AppendPreferPlmnList(
                                                                                MMI_CTRL_ID_T ctrl_id,
                                                                                MN_PLMN_T *prefer_network,
                                                                                uint16 index)
{    
    GUILIST_ITEM_DATA_T item_data = {0};
    MN_PLMN_T plmn = {0};
    MMI_STRING_T plmn_name = {0};        
    wchar plmn_name_arr[MAX_ONSHORTFORM_LEN+1]={0};
    wchar plmn_code[MAX_MCC_MNC_DIGIT_DISPLAY +1] = {0};
    uint16 plmn_code_len = MAX_MCC_MNC_DIGIT_DISPLAY;

    //SCI_TRACE_LOW:"MMIAPIPHONE_AppendPreferPlmnList,index=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8589_112_2_18_2_45_18_172,(uint8*)"d",index);

    if((PNULL!=prefer_network) && (index < MMISET_PREFER_PLMN_LIST_MAX) )
    {
        plmn.mcc = prefer_network[index].mcc;
        plmn.mnc = prefer_network[index].mnc;
        plmn.mnc_digit_num = prefer_network[index].mnc_digit_num;

        //PLMN NAME Content.
        //get plmn name info
        plmn_name.wstr_ptr =(wchar *)plmn_name_arr;
        MMIAPIPHONE_GenPLMNDisplay(MMIAPIPHONE_GetTDOrGsm(), &plmn, plmn_name.wstr_ptr, &plmn_name.wstr_len, FALSE);
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;        
        item_data.item_content[0].item_data.text_buffer.wstr_len = plmn_name.wstr_len;
        item_data.item_content[0].item_data.text_buffer.wstr_ptr = plmn_name.wstr_ptr;

        //PLMN CODE Content .        
        //get plmn code info
        MMIAPIPHONE_GenPLMNDisplay(MMIAPIPHONE_GetTDOrGsm(), &plmn,plmn_code, &plmn_code_len, TRUE);
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;        
        item_data.item_content[1].item_data.text_buffer.wstr_len = plmn_code_len;
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = plmn_code;
        
        //set softkey
        item_data.softkey_id[0] = STXT_OPTION;
        item_data.softkey_id[1] = TXT_NULL;
        item_data.softkey_id[2] = STXT_RETURN;

        GUILIST_SetItemData( ctrl_id, &item_data, index );    
        GUILIST_SetOwnSofterKey(ctrl_id, TRUE);
    }
}
#endif

/*****************************************************************************/
// 	Description : get Plmn List Num...
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
PUBLIC uint16 MMIAPIPHONE_GetPlmnListNum(void)
{
    //s_network_nametable[]数组的第一项是test项，非实际的plmn。
    return MAX_NETWORK_NUM - 1;
}

/*****************************************************************************/
// 	Description :MMIAPIPHONE_GetPlmnInfoByIndex
//	Global resource dependence : 
//  Author: michael.shi
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GetPlmnInfoByIndex(                                                                                
                                                                                MN_PLMN_T *prefer_network,
                                                                                uint32 index)
{
    if(PNULL == prefer_network )
    {
        //SCI_TRACE_LOW:"GetSimAndServiceString: the prefer_network =%d "
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8643_112_2_18_2_45_18_173,(uint8*)"d", prefer_network);
        return FALSE;
    }
    
    //SCI_TRACE_LOW:"MMIAPIPHONE_GetPlmnInfoByIndex,index=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8647_112_2_18_2_45_18_174,(uint8*)"d",index);

    if( index < MAX_NETWORK_NUM)
    {
        prefer_network->mcc = s_network_nametable[index].mcc;
        prefer_network->mnc = s_network_nametable[index].mnc;
        if( s_network_nametable[index].mnc >= 100)
        {
            prefer_network->mnc_digit_num = 3; /// set mnc length as 3...
        }
        else
        {
            prefer_network->mnc_digit_num = 2; /// set mnc length as 2...
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#ifdef MMI_LDN_SUPPORT
/*****************************************************************************/
// 	Description : ldn update confirm
//	Global resource dependence : none
//	Author:dave.ruan
//	Note:delete all/datlete both type is erase
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleLDNUpdateCnf( DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    MnLndUpdateCnfS *ldn_update_cnf_ptr = (MnLndUpdateCnfS *)param;
    BOOLEAN  is_ldn_all_deleted = TRUE;
    uint32  i = 0;
    
    //SCI_TRACE_LOW:"ccapp.c HandleLDNUpdateCnf,type= %d, result =%d, dual_sys= %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8683_112_2_18_2_45_18_175,(uint8*)"ddd",ldn_update_cnf_ptr->operate_type,ldn_update_cnf_ptr->is_ok,ldn_update_cnf_ptr->dual_sys);
                                
    if(MN_LND_ERASE == ldn_update_cnf_ptr->operate_type)
    {
	    if(MMIAPICL_GetLdnIsDeleteAll())
	    {
	        MMIAPICL_SetLdnIsDeleteAll(FALSE);
	        if(MMIAPICL_DeleteAllLdnInfo(MMICL_CALL_DIALED))
	        {
	            return result;
	        }
	    }
	       
	    MMK_CloseWin(MMICC_LDN_UPDATE_WAITING_WIN_ID);

	    if(ldn_update_cnf_ptr != PNULL && ldn_update_cnf_ptr->is_ok)
	    {
	        //success
	        MMIPUB_OpenAlertSuccessWin(TXT_DELETED);
	    }
	    else
	    {
	        //reject
	        MMIPUB_OpenAlertWarningWin(TXT_COMMON_DEL_FAIL);
	    }
	    //close progess waiting win
	    //CC_UpdateCurrentCallStatus(TRUE);
	}
    return result;
}
#endif

/*****************************************************************************/
// 	Description : 获取状态正常的SIM卡总数和具体卡的位置
//	Global resource dependence : 
//  Author: dave.ruan
//	Note:   sim_idex_ptr :point to ok sim index,if null,do not set sim index
//              array_len :ok sim index len
/*****************************************************************************/
PUBLIC uint8 MMIAPIPHONE_GetAllSimStateOKCount(uint16 *sim_idex_ptr, uint16 array_len)
{
    uint8 active_sim_count = 0;
    int32 sim_index = MMI_DUAL_SYS_MAX;

    for (sim_index = MN_DUAL_SYS_1; sim_index < MMI_DUAL_SYS_MAX; sim_index++)
    {
        if (MMIAPIPHONE_IsSimOk(sim_index))
        {
            if ((sim_idex_ptr != NULL) && (active_sim_count < array_len))
            {
                sim_idex_ptr[active_sim_count] = sim_index;
            }

            active_sim_count++;
        }
    }
    //SCI_TRACE_LOW:"[mmiphone]: MMIAPIPHONE_GetAllSimStateOKCount, active_sim_count=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPHONE_8760_112_2_18_2_45_18_176,(uint8*)"d",active_sim_count);
    return active_sim_count;
}

#ifdef CSC_XML_SUPPORT
/*****************************************************************************/
// 	Description : Set the network name from csc
//	Global resource dependence : 
//  Author: minghu.mao
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_SetNetWorkNameByCSC(MMICUS_CSC_GENERALINFO_NETWORKINFO_T *csc_network_name_ptr,
                                                                                                            uint16 network_num)
{
    MMICUS_CSC_GENERALINFO_NETWORKINFO_T *temp_csc_network_ptr = PNULL;
    MMICUS_CSC_GENERALINFO_NETWORKINFO_T *cur_csc_network_ptr = PNULL;
    MMICUS_CSC_GENERALINFO_NETWORKINFO_T *temp_cur_csc_network_ptr = PNULL;

    if (PNULL == csc_network_name_ptr || 0 == network_num)
    {
        return FALSE;
    }

    if (PNULL != s_csc_network_name_ptr)
    {
        cur_csc_network_ptr = s_csc_network_name_ptr->csc_network_info;

        while (PNULL != cur_csc_network_ptr)
        {
            temp_csc_network_ptr = cur_csc_network_ptr;
            cur_csc_network_ptr = cur_csc_network_ptr->next;

            SCI_FREE(temp_csc_network_ptr);
            temp_csc_network_ptr = PNULL;
        }

        SCI_FREE(s_csc_network_name_ptr);
        s_csc_network_name_ptr = PNULL;
    }

    s_csc_network_name_ptr = (MMI_CSC_NETWORK_NAME_T *)SCI_ALLOCAZ(sizeof(MMI_CSC_NETWORK_NAME_T));

    if (PNULL == s_csc_network_name_ptr)
    {
        return FALSE;
    }

    s_csc_network_name_ptr->net_work_num = network_num;

    temp_cur_csc_network_ptr = csc_network_name_ptr;

    while (PNULL != temp_cur_csc_network_ptr)
    {
        cur_csc_network_ptr = (MMICUS_CSC_GENERALINFO_NETWORKINFO_T *)SCI_ALLOCAZ(sizeof(MMICUS_CSC_GENERALINFO_NETWORKINFO_T));

        if (PNULL == cur_csc_network_ptr)
        {
            return FALSE;
        }
        SCI_MEMCPY(cur_csc_network_ptr, temp_cur_csc_network_ptr, sizeof(MMICUS_CSC_GENERALINFO_NETWORKINFO_T));
        cur_csc_network_ptr->next = PNULL;
        temp_csc_network_ptr = s_csc_network_name_ptr->csc_network_info;

        if (PNULL == temp_csc_network_ptr)
        {
            s_csc_network_name_ptr->csc_network_info = cur_csc_network_ptr;
        }
        else
        {
            while (PNULL != temp_csc_network_ptr->next)
            {
                temp_csc_network_ptr = temp_csc_network_ptr->next;
            }
            temp_csc_network_ptr->next = cur_csc_network_ptr;
        }

        temp_cur_csc_network_ptr = temp_cur_csc_network_ptr->next;
     }    

    return TRUE;
}

/*****************************************************************************/
// 	Description : Get the network name from csc
//	Global resource dependence : 
//  Author: minghu.mao
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPHONE_GetNetWorkNameByCSC(uint16 mcc, 
                                                                                                            uint16 mnc,
                                                                                                            MMI_STRING_T *name_string_str)
{
    MMICUS_CSC_GENERALINFO_NETWORKINFO_T *cur_csc_network_ptr = PNULL;
    uint16 network_num = 0;
    
    if (PNULL == s_csc_network_name_ptr || PNULL == name_string_str)
    {
        return FALSE;
    }

    network_num = s_csc_network_name_ptr->net_work_num;
    cur_csc_network_ptr = s_csc_network_name_ptr->csc_network_info;
    while (PNULL != cur_csc_network_ptr && 0 < network_num--)
    {
        if (cur_csc_network_ptr->mcc == mcc && cur_csc_network_ptr->mnc == mnc)
        {
            name_string_str->wstr_len = strlen((char *)cur_csc_network_ptr->network_name);
            name_string_str->wstr_len = MIN(name_string_str->wstr_len, XML_TAG_NAME_MAX_LEN);
            name_string_str->wstr_ptr = (wchar *)SCI_ALLOCAZ((name_string_str->wstr_len + 1)*sizeof(wchar));
            if (PNULL == name_string_str->wstr_ptr)
            {
                return FALSE;
            }

            MMI_STRNTOWSTR(name_string_str->wstr_ptr, name_string_str->wstr_len, cur_csc_network_ptr->network_name, name_string_str->wstr_len, name_string_str->wstr_len);
            return TRUE;
        }
    }

    return FALSE;
}
#endif

