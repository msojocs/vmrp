/*****************************************************************************
** File Name:      mmifmm.c                                                  *
** Author:                                                                   *
** Date:             06/04/2007                                              *
** Copyright:      2004 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe file manager module         *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 04/2007       Robert.Lu          Create                                   *
******************************************************************************/

#define _MMIFMM_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_app_fmm_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmifmm_export.h"
#include "mmifmm_internal.h"
#include "gui_ucs2b_converter.h"
//#include "mmiebook_file.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
#include "window_parse.h"
#include "mmifmm_menutable.h"
#include "mmi_common.h"
#include "guifont.h"
#include "mmi_appmsg.h"
#include "block_mem.h"
#include "mmiidle_export.h"
#include "mmifmm_id.h"
#include "mmi_signal.h"
#ifndef WIN32
#include "chng_freq.h"
#include "chip.h"
#endif
#include "mmimms_export.h"
#include "mmimms_text.h"
#include "mmipb_export.h"
#include "mmisd_export.h"
#include "mmifmm_text.h"
#include "mmimultim_text.h"
#include "mmipub.h"
#include "mmifmm_nv.h"
#include "mmimp3_text.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "guitext.h"
#include "mmiudisk_export.h"
#include "mmi_default.h"
#include "mmk_timer.h"
//#include "mmienvset.h"
#include "mmienvset_export.h"
#include "mmialarm_export.h"
#include "guirichtext.h"
#ifdef VIDEO_PLAYER_SUPPORT
#include "mmivp_export.h"
#include "mmivp_text.h"
#endif
#include "dal_player.h"

#include "mmifmm_interface.h"
#include "guiedit.h"
#include "mmicc_export.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmipb_text.h"
#include "mmiset_text.h"
#include "mmi_filemgr.h"
//#include "mmiset_func.h"

//+CR230144
#include "mmiim.h"
//-CR230144

#if defined(DRM_SUPPORT)
#include "mmidrm_export.h"
#include "mmi_drmfilemgr.h"
#endif
#ifdef LIVE_WALLPAPER_FRAMEWORK_SUPPORT
#include "mmi_wallpaper_export.h"
#include "mmilivewallpaper_export.h"
#endif 
#ifdef MMI_PIC_CROP_SUPPORT
#include "mmipic_crop_export.h"
#endif
#include "mmifmm_interface.h "
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define MMI_FMM_MAX_ALLOC_NUM  100
#define MMI_FMM_MAX_BUFFER_NUM  (MMIFMM_FILE_NUM/MMI_FMM_MAX_ALLOC_NUM)
#ifdef MRAPP_SUPPORT
#define MRAPP_FILE_SUFFIX_NUM               1
#endif

#define MMIFMM_FILE_SUFFIX_MAX_LEN          7

#define MMIFMM_PLAYPER_TIME      300


#ifdef KURO_SUPPORT
#define MMIFMM_IS_KUR_SUPPORT            TRUE
#else
#define MMIFMM_IS_KUR_SUPPORT            FALSE
#endif

#ifdef AAC_SUPPORT
#define MMIFMM_IS_AAC_SUPPORT            TRUE
#else
#define MMIFMM_IS_AAC_SUPPORT            FALSE
#endif

#ifdef WMA_SUPPORT
#define MMIFMM_IS_WMA_SUPPORT            TRUE
#else
#define MMIFMM_IS_WMA_SUPPORT            FALSE
#endif

#ifdef AMR_SUPPORT
#define MMIFMM_IS_AMR_SUPPORT            TRUE
#else
#define MMIFMM_IS_AMR_SUPPORT            FALSE
#endif

#ifdef MIDI_SUPPORT
#define MMIFMM_IS_MID_SUPPORT            TRUE
#else
#define MMIFMM_IS_MID_SUPPORT            FALSE
#endif

#ifdef MP3_SUPPORT
#define MMIFMM_IS_MP3_SUPPORT            TRUE
#else
#define MMIFMM_IS_MP3_SUPPORT            FALSE
#endif

#ifdef WAV_SUPPORT
#define MMIFMM_IS_WAV_SUPPORT            TRUE
#else
#define MMIFMM_IS_WAV_SUPPORT            FALSE
#endif
//macro replace
//#ifdef MMIMPEG4_AVI_ENABLE
#ifdef AVI_DEC_SUPPORT
#define MMIFMM_IS_AVI_SUPPORT            TRUE
#else
#define MMIFMM_IS_AVI_SUPPORT            FALSE
#endif

#ifdef MMI_RMVB_SUPPORT
#define MMIFMM_IS_RMVB_SUPPORT            TRUE
#else
#define MMIFMM_IS_RMVB_SUPPORT            FALSE
#endif

#ifdef FLV_DEC_SUPPORT
#define MMIFMM_IS_FLV_SUPPORT            TRUE
#else
#define MMIFMM_IS_FLV_SUPPORT            FALSE
#endif

#ifdef CMMB_SUPPORT
#define MMIFMM_IS_MTV_SUPPORT            TRUE
#else
#define MMIFMM_IS_MTV_SUPPORT            FALSE
#endif

#ifdef JAVA_SUPPORT
#define MMIFMM_IS_JAVA_SUPPORT            TRUE
#else
#define MMIFMM_IS_JAVA_SUPPORT            FALSE
#endif

#ifdef QBTHEME_SUPPORT
#define MMIFMM_IS_QBTHEME_SUPPORT			TRUE
#else
#define MMIFMM_IS_QBTHEME_SUPPORT			FALSE
#endif

#ifdef BROWSER_SUPPORT_NETFRONT    
#define MMIFMM_IS_HTM_SUPPORT               TRUE
#define MMIFMM_IS_HTML_SUPPORT              TRUE
#define MMIFMM_IS_XML_SUPPORT               TRUE

#else
#define MMIFMM_IS_HTM_SUPPORT               FALSE
#define MMIFMM_IS_HTML_SUPPORT              FALSE
#define MMIFMM_IS_XML_SUPPORT               FALSE
#endif

//+CR230144
#define SBC_CHAR_START                      (0xFF01)            //全角符号:"!"
#define SBC_CHAR_END                        (0xFF5E)            //全角符号:"~"
#define SBC_TO_DBC_STEP                     (0xFEE0)            //全角半角转换间隔
#define SBC_SPACE                           (0x3000)            //全角符号:空格
#define DBC_SPACE                           (0x20)              //半角空格
//-CR230144
/**--------------------------------------------------------------------------*
 **                         LOCAL DEFINITION                                 *
 **--------------------------------------------------------------------------*/
typedef struct 
{
    uint8   suffix[MMIFMM_FILE_SUFFIX_MAX_LEN]; 
    uint32  type;
    BOOLEAN is_support;
}MMIFMM_SUFFIX_T;

LOCAL const MMIFMM_SUFFIX_T s_fmm_all_suffix[] = 
{   {"jpg",     MMIFMM_PICTURE_JPG, TRUE}, 
    {"gif",     MMIFMM_PICTURE_GIF, TRUE}, 
    {"bmp",     MMIFMM_PICTURE_BMP, TRUE}, 
    {"wbmp",    MMIFMM_PICTURE_WBMP, TRUE}, 
    {"png",     MMIFMM_PICTURE_PNG, TRUE}, 
    {"jpeg",    MMIFMM_PICTURE_JPG, TRUE}, 
#ifdef DRM_SUPPORT
    {"dm",      MMIFMM_DM_FILE,      TRUE}, 
    {"dcf",     MMIFMM_DCF_FILE,     TRUE}, 
#endif
    {"mp3",     MMIFMM_MUSIC_MP3,   MMIFMM_IS_MP3_SUPPORT},
    //@CR241798 2011.05.30
    {"mp2",     MMIFMM_MUSIC_MP3,   MMIFMM_IS_MP3_SUPPORT},
    {"mp1",     MMIFMM_MUSIC_MP3,   MMIFMM_IS_MP3_SUPPORT},
    //@CR241798 2011.05.30
    {"wma",     MMIFMM_MUSIC_WMA,   MMIFMM_IS_WMA_SUPPORT}, 
    {"mid",     MMIFMM_MUSIC_MID,   MMIFMM_IS_MID_SUPPORT}, 
    {"amr",     MMIFMM_MUSIC_AMR,   MMIFMM_IS_AMR_SUPPORT}, 
    {"aac",     MMIFMM_MUSIC_ACC,   MMIFMM_IS_AAC_SUPPORT}, 
    {"m4a",     MMIFMM_MUSIC_M4A,   MMIFMM_IS_AAC_SUPPORT}, 
    {"wav",     MMIFMM_MUSIC_WAV,   MMIFMM_IS_WAV_SUPPORT}, 
    {"midi",    MMIFMM_MUSIC_MIDI,  MMIFMM_IS_MID_SUPPORT},
    {"rm",      MMIFMM_MOVIE_RMVB,  MMIFMM_IS_RMVB_SUPPORT},
    {"rmvb",    MMIFMM_MOVIE_RMVB,  MMIFMM_IS_RMVB_SUPPORT},
    {"rv",      MMIFMM_MOVIE_RMVB,  MMIFMM_IS_RMVB_SUPPORT},
    {"flv",     MMIFMM_MOVIE_FLV,   MMIFMM_IS_FLV_SUPPORT},
    {"mp4",     MMIFMM_MOVIE_MP4,   TRUE}, 
    {"3gp",     MMIFMM_MOVIE_3GP,   TRUE}, 
#ifdef MMI_KING_MOVIE_SUPPORT    
    {"kmv",     MMIFMM_MOVIE_KMV,   TRUE}, 
    {"smv",     MMIFMM_MOVIE_SMV,   TRUE},//smv_support
#ifdef MMI_KING_MOVIE_SUPPORT_HMV
    {"hmv",     MMIFMM_MOVIE_HMV,   TRUE},//hmv_support
#endif
#endif    
    {"txt",     MMIFMM_MOVIE_TXT,   TRUE}, 
    {"lrc",     MMIFMM_MOVIE_LRC,   TRUE}, 
    {"szip",    MMIFMM_MOVIE_SZIP,  TRUE}, 
#if defined MMI_VCARD_SUPPORT
    {"vcf",     MMIFMM_MOVIE_VCF,   TRUE},
#endif
    {"mtv",     MMIFMM_MOVIE_MTV,   MMIFMM_IS_MTV_SUPPORT},  
    {"avi",     MMIFMM_MOVIE_AVI,   MMIFMM_IS_AVI_SUPPORT},
    {"jad",     MMIFMM_JAVA_JAD,    MMIFMM_IS_JAVA_SUPPORT},
    {"jar",     MMIFMM_JAVA_JAR,    MMIFMM_IS_JAVA_SUPPORT},
#ifdef QBTHEME_SUPPORT
	{"qbt",     MMIFMM_THEME_QB,    MMIFMM_IS_QBTHEME_SUPPORT},
#endif
    {"htm",     MMIFMM_BROWSER_HTM,    MMIFMM_IS_HTM_SUPPORT},
    {"html",     MMIFMM_BROWSER_HTML,    MMIFMM_IS_HTML_SUPPORT},
    {"xml",     MMIFMM_BROWSER_XML,    MMIFMM_IS_XML_SUPPORT},
    {"lib",     MMIFMM_FONT,        TRUE},
#ifdef MMI_VCALENDAR_SUPPORT
    {"vcs",     MMIFMM_MOVIE_VCS,   TRUE},
#endif
#ifdef MRAPP_SUPPORT
    {"mrp",     MMIFMM_MRAPP_MRP,   TRUE},
    {"nes",     MMIFMM_MRAPP_NES,   TRUE},
#endif
    {0,}, 		/*lint !e651*/   
};

typedef struct  
{
    uint32                      max_size;       //下载铃声最大值
    BOOLEAN                     is_need_data;   //是否需要返回图片数据
    MMI_WIN_ID_T                win_id;         //发送消息的目的窗口
    wchar                       ticked_file[MMIFILE_FULL_PATH_MAX_LEN+1];  //file need to be ticked on
    uint16                      ticked_file_len; //file name len
    int32                       ring_vol;
#ifdef DRM_SUPPORT
    MMIFMM_DRM_LIMIT_TYPE_T     drm_limit;      //用于设置是否次数受限或者是否转发类型受限等
#endif
}MMIFMM_SELECT_WIN_PARAM_T;

typedef enum
{
    MMIFMM_MUSIC_INFO_NONE,

    MMIFMM_MUSIC_INFO_UNKNOWN,
    MMIFMM_MUSIC_INFO_NOT_SUPPORT,
    MMIFMM_MUSIC_INFO_SUPPORT,
	MMIFMM_MUSIC_INFO_DRM_FAIL,
	MMIFMM_MUSIC_INFO_IN_CALL_STATE

    //MMIFMM_MUSIC_INFO_MAX
} MMIFMM_MUSIC_INFO_STATUS_E;

typedef enum
{
    MMIFMM_SELECT_FAIL,             //select file fail
    MMIFMM_SELECT_SUCCESS,          //select file success
    MMIFMM_SELECT_DRM_NOCOPYRIGHTS //select file is drm have no copyrights
} MMIFMM_SELECT_FILE_RESULT_E;

//LOCAL MMIFMM_COPY_FILE_INFO_T        s_fmm_copy_info = {0};
//for file buffer malloc and free pointer
LOCAL void  * s_pointer_buffer[MMI_FMM_MAX_BUFFER_NUM] ={PNULL};
LOCAL BOOLEAN s_buffer[MMI_FMM_MAX_ALLOC_NUM]={FALSE};
LOCAL int32 s_buffer_index = 0;
LOCAL uint32 s_suffix_type =MMIFMM_FILE_ALL;
LOCAL MMIFMM_SELECT_WIN_PARAM_T *s_select_win_param_ptr = PNULL;

LOCAL uint8     *s_multimfiledata_ptr = PNULL;

LOCAL MMIFMM_AS_WALLPAPER_INFO   s_file_as_wallpaper_info = {0}; //墙纸的信息
LOCAL MMISET_WALLPAPER_SET_T        s_pre_wallpaper = {0};          //前墙纸的信息
LOCAL BOOLEAN                       s_is_reset_pre_wallpaper = FALSE;//是否会赋之前的墙纸设置
LOCAL MMIFMM_AS_WALLPAPER_INFO   s_pre_wallpaper_info = {0};     //前墙纸文件名,设置失败后恢复

LOCAL MMISET_CALL_MORE_RING_T      s_set_ring_info = {0};      //设置铃声文件
LOCAL MN_DUAL_SYS_E                s_set_ring_dual_sys = MN_DUAL_SYS_1;
LOCAL uint16                       s_set_ring_alarm_id = 0;
LOCAL MMIMULTIM_SET_RING_TYPE_E    s_set_ring_type = MMIMULTIM_SET_RING_NULL;
#ifdef MMI_PDA_SUPPORT
LOCAL BOOLEAN               s_is_to_select_foler = FALSE;
#endif
LOCAL MMIFMM_FILTER_T        s_filter = {0};
/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DECLARE                                 *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

const wchar g_mmifmm_ellipsis_wstr[] = { '.', '.', '.', 0 };
const wchar g_mmifmm_ellipsis_3_wstr[] = { '.', '.', '.', 0 };
const wchar g_mmifmm_csk2_default_dir_wstr[] = { 'C', 's', 't', 'a', 'r' ,0 };
#ifdef CMCC_UI_STYLE
wchar g_mmimultim_picture_dir_wstr[] = { 'P', 'i', 'c', 't', 'u', 'r', 'e', 0 }/*lint !e785*/;
wchar g_mmimultim_photos_dir_wstr[] = { 'P', 'h', 'o', 't', 'o', 's', 0 }/*lint !e785*/;
#else
wchar g_mmimultim_picture_dir_wstr[] = { 'P', 'h', 'o', 't', 'o', 's', 0 }/*lint !e785*/;
#endif
wchar g_mmimultim_music_dir_wstr[]   = { 'A', 'u', 'd', 'i', 'o', 0 }/*lint !e785*/;
wchar g_mmimultim_movie_dir_wstr[]   = { 'V', 'i', 'd', 'e', 'o' , 0 }/*lint !e785*/;
#ifdef MMI_KING_MOVIE_SUPPORT
wchar g_mmimultim_kingmovie_dir_wstr[]   = { 'K','i','n','g','M', 'o', 'v', 'i', 'e' , 0 }/*lint !e785*/;
#endif
wchar g_mmimultim_ebook_dir_wstr[]   = { 'E', 'b', 'o', 'o', 'k' , 0 }/*lint !e785*/;
wchar g_mmimultim_java_dir_wstr[]   = { 'J', 'A', 'V', 'A',  0 }/*lint !e785*/;
#if defined MMI_VCARD_SUPPORT
wchar g_mmimultim_vcard_dir_wstr[]   = { 'v', 'C', 'a', 'r', 'd' , 0 }/*lint !e785*/;
#endif
#ifdef MMI_VCALENDAR_SUPPORT
wchar g_mmimultim_vcalendar_dir_wstr[]   = { 'v', 'C', 'a', 'l', 'e' ,'n' ,'d' ,'a' ,'r' , 0 }/*lint !e785*/;
#endif
#ifdef MCARE_V31_SUPPORT
wchar g_tencentmcare_tmz_dir_wstr[]   = { 'Q', 'Q', 'D', 'o', 'w', 'n', 'L', 'o', 'a', 'd' , 0 }/*lint !e785*/;      
#endif
//const wchar g_mmimultim_music_dir_wstr[]   = { 'A', 'u', 'd', 'i', 'o', 0 };
//const wchar g_mmimultim_movie_dir_wstr[]   = { 'V', 'i', 'd', 'e', 'o' , 0 };
wchar g_mmimultim_pb_dir_wstr[]      = { 'P', 'B' , 0 };
wchar g_other_dir_default[] = {'O', 't', 'h', 'e', 'r','s', 0}/*lint !e785*/;
//const wchar g_mmimultim_java_dir_wstr[] = {'j', 'a', 'v', 'a', 0};
wchar g_mmimultim_dir_system[] = {MMIMULTIM_DIR_SYSTEM_CHAR, 0};
wchar g_mmimultim_alarm_dir_wstr[]      = { 'A', 'l' ,'a','r','m', 0 };
wchar g_mmimultim_font_dir_wstr[]   = { 'F', 'o', 'n', 't', 0 };

wchar g_file_array_dir_wstr[]   = { 'F', 'i', 'l', 'e','a','r','r','a','y',0 };
LOCAL BOOLEAN s_is_music_preview = FALSE;

/*LOCAL const wchar *s_folder_buffer[MMIFMM_FOLDER_TYPE_NUM] = {
	g_other_dir_default,
	g_mmimultim_picture_dir_wstr,
    g_mmimultim_music_dir_wstr, 
	g_mmimultim_movie_dir_wstr,
	g_mmimultim_ebook_dir_wstr,
    g_mmimultim_java_dir_wstr,
    g_mmimultim_vcard_dir_wstr
};

LOCAL const MMI_TEXT_ID_T s_folder_text_id[MMIFMM_FOLDER_TYPE_NUM] = {
	TXT_TOOLS_OTHERS,
	TXT_COMMON_PIC,
    TXT_COMMON_MUSIC, 
	TXT_COMMON_MOVIE,
	TXT_COMM_EBOOK,
    TXT_JAVA,
    TXT_COMMON_VCARD
};*/
/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
//  Description : handle get icon from icon list preview win msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectPicWinMsg(
                                       MMI_WIN_ID_T        win_id,
                                       MMI_MESSAGE_ID_E    msg_id,
                                       DPARAM              param
                                       );

/*****************************************************************************/
//  Description : handle get selecte all file
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectAllFileWinMsg(
                                       MMI_WIN_ID_T        win_id,
                                       MMI_MESSAGE_ID_E    msg_id,
                                       DPARAM              param
                                       );

/*****************************************************************************/
//  Description : handle select ring list win
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectMusicWinMsg(
                                  MMI_WIN_ID_T          win_id,     
                                  MMI_MESSAGE_ID_E      msg_id, 
                                  DPARAM                param
                                  );

/*****************************************************************************/
//  Description : handle movie select win
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectMovWinMsg(
                                      MMI_WIN_ID_T          win_id,     
                                      MMI_MESSAGE_ID_E      msg_id, 
                                      DPARAM                param
                                      );

/*****************************************************************************/
//  Description : handle select ring list win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectMusicWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                );

/*****************************************************************************/
//  Description : handle select ring list win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL MMIFMM_SELECT_FILE_RESULT_E OpenSelectMovWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                );

/*****************************************************************************/
//  Description : handle select movie list from mms win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectMovWinMMSCbk(
                                     FILEARRAY_DATA_T    *file_data_ptr
                                     );

LOCAL BOOLEAN OpenSelectPicWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                );

/*****************************************************************************/
//  Description : handle select file call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectAllFileWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                );

/*****************************************************************************/
// Description : set common character
// Global resource dependence : 
// Author:jian.ma
// Note: 
/*****************************************************************************/
LOCAL void SetCommonCharacter( GUIRICHTEXT_ITEM_T* p_item );

/*****************************************************************************/
//  Description : add one detail item
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
LOCAL void AddOneDetailItem(
                            MMI_CTRL_ID_T   ctrl_id,
                            MMI_TEXT_ID_T   label_title,
                            MMI_TEXT_ID_T   label_name,
                            uint16          *index_ptr,
                            wchar           *detail_ptr,
                            uint32          detail_len
                            );

/*****************************************************************************/
//  Description : init multim folder
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void InitMultimFolder(
                            const wchar   *device_ptr,
                            uint16  device_len,
                            const wchar   *dir_ptr,
                            uint16  dir_len
                            );


/*****************************************************************************/
//  Description : fmm detail information waiting win msg hanlement
//  Global resource dependence : none
//  Author:     ying.xu
//  Note:       20100920
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleFolderDetailWaitWinMsg(
                                       MMI_WIN_ID_T              win_id,
                                       MMI_MESSAGE_ID_E      msg_id,
                                       DPARAM                        param
                                       )    ;

//+CR230144
/*****************************************************************************
 	Description : Convert the characer from SBC to DBC.
	Global resource dependence : none
       Author: 
	Note:
*****************************************************************************/
LOCAL BOOLEAN ConvertSBC2DBC(
                              uint16 *ch    //IN/OUT:全角符号转换成半角
                            );

/*****************************************************************************
 	Description : Convert the character from upper to lower 
	Global resource dependence : none
       Author: 
	Note:
*****************************************************************************/
LOCAL BOOLEAN ConvertToLower(uint16 *ch);

/*****************************************************************************
 	Description : Compare the double string. Compare the double characters by pinyin format if 
 	                    the characters are ucs2.
	Global resource dependence : none
       Author: 
	Note:
*****************************************************************************/
LOCAL int CompareName(
                      const wchar       *wstr1_ptr, 
                      uint16             wstr1_len,
                      const wchar       *wstr2_ptr, 
                      uint16             wstr2_len                                                   
                     );
//-CR230144

#ifdef DRM_SUPPORT
/*****************************************************************************/
//  Description : set the select drm file limit value
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsSelDrmConstraintFile(
    uint16 *filename, 
    uint16 name_len, 
    MMIFMM_DRM_LIMIT_TYPE_T limit_value
);
#endif


/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : initialize FMM module
//  Global resource dependence : none
//  Author: robert.lu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitModule(void)
{
    //register menu
    MMIFMM_RegMenuGroup();
    MMIFMM_RegFMMNv();
    MMIFMM_RegWinIdNameArr();
}

//+CR230144
/*****************************************************************************
 	Description : Convert the characer from SBC to DBC.
	Global resource dependence : none
       Author: 
	Note:
*****************************************************************************/
LOCAL BOOLEAN ConvertSBC2DBC(
                                uint16 *ch    //IN/OUT:全角符号转换成半角
                            )
{
    BOOLEAN result = FALSE;

    if ((*ch >= SBC_CHAR_START) && (*ch <= SBC_CHAR_END))
    {
        *ch -= SBC_TO_DBC_STEP;
        
        result = TRUE;
    }
    else if (*ch == SBC_SPACE)
    {
        *ch = DBC_SPACE;   

        result = TRUE;
    }

    return result;
}

/*****************************************************************************
 	Description : Convert the character from upper to lower 
	Global resource dependence : none
       Author: 
	Note:
*****************************************************************************/
LOCAL BOOLEAN ConvertToLower(uint16 *ch)
{
    if ('A' <= *ch && *ch <= 'Z')
    {
        //upper case change to low case
        *ch = *ch + ('a' - 'A');
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 	Description : Compare the double string. Compare the double characters by pinyin format if 
 	                    the characters are ucs2.
	Global resource dependence : none
       Author: 
	Note:
*****************************************************************************/
LOCAL int CompareName(
                          const wchar       *wstr1_ptr, 
                          uint16             wstr1_len,
                          const wchar       *wstr2_ptr, 
                          uint16             wstr2_len                                                   
                      )
{
    int32   result = 0;
    int16  min_len = 0;
    BOOLEAN is_ucs2_ch1 = FALSE; 
    BOOLEAN is_ucs2_ch2 = FALSE; 
    uint16 diff_char1 = 0;
    uint16 diff_char2 = 0;
    uint8   flag = 0;                                   
    wchar ucs2_str1[10] = {0};
    wchar ucs2_str2[10] = {0}; 

    //check string length.
    if ((0 == wstr1_len) && (0 == wstr2_len))
    {
         return(0);
    }
    else if ((0 == wstr1_len) || (0 == wstr2_len))
    {
        return ((wstr1_len < wstr2_len )?(-1):(1));
    }

    //check string pointer.
    if( wstr1_ptr == PNULL && wstr2_ptr == PNULL )
    {
        return (0);
    }
    else if( wstr1_ptr == PNULL && wstr2_ptr != PNULL )
    {
        return (-1);
    }
    else if( wstr1_ptr != PNULL && wstr2_ptr == PNULL )
    {
        return (1);
    }    
    
    min_len = MIN( wstr1_len, wstr2_len);
    
/*lint -save -e613 -e794 */
    while ( min_len && *wstr1_ptr && *wstr1_ptr == *wstr2_ptr )
    {
        wstr1_ptr++;
        wstr2_ptr++;
		min_len--;
    }
    if ( 0 == min_len && wstr1_len == wstr2_len)
    {
        return 0;
    }
    diff_char1 = (uint16)*wstr1_ptr;
    diff_char2 = (uint16)*wstr2_ptr;    
/*lint -restore */
    
    is_ucs2_ch1 = MMIAPICOM_IsUcs2Char(diff_char1);
    is_ucs2_ch2 = MMIAPICOM_IsUcs2Char(diff_char2);

    //全角符号转化成半角符号
    if (is_ucs2_ch1 && ConvertSBC2DBC(&diff_char1) )
    {
        flag |= BIT_0;
        is_ucs2_ch1 = FALSE;
    }
    
    if (is_ucs2_ch2 && ConvertSBC2DBC(&diff_char2) )
    {
        flag |= BIT_1;
        is_ucs2_ch2 = FALSE;
    }
    
    if (is_ucs2_ch1 && is_ucs2_ch2) 
    {
        //Compare pinyin index if the double ucs2 characters.
        MMIAPIIM_GetTextLetters(diff_char1, ucs2_str1);
        MMIAPIIM_GetTextLetters(diff_char2, ucs2_str2);

        result = MMIAPICOM_Wstrcmp(ucs2_str1, ucs2_str2);
    }
    else if (!is_ucs2_ch1 && !is_ucs2_ch2)
    {
        //Both charaters is not ucs2 charater.        
        flag |= ConvertToLower(&diff_char1)?BIT_0:0;
        flag |= ConvertToLower(&diff_char2)?BIT_1:0;

        result = (diff_char1 - diff_char2);
    }
    else
    {
        //Either of characters is ucs2 charater.
        result = is_ucs2_ch1? (1):(-1);
    }

    if (0 == result)
    {
        if (0 == flag)
        {
             //Compare the name length if the charaters is equal.
             result = (wstr1_len > wstr2_len)? (1): (-1);
        }
        else 
        {
            //Switch the position if the first charater is SBC.
            result = (flag & BIT_0) ? (-1):(1);
        }
    }
    
    return ( result > 0 ? 1 : ( result < 0 ? -1 : 0 ) ); 
}
//-CR230144

/*****************************************************************************/
//  Description : 比较名称
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileName(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     )                            
{
    int                 result  = 0;
    MMIFMM_FILE_INFO_T  *data1_ptr = PNULL,*data2_ptr = PNULL;

    //SCI_ASSERT(PNULL != i1_data && PNULL != i2_data);
    if((PNULL == i1_data) || (PNULL == i2_data))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_CompareFileName param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_746_112_2_18_2_20_7_212,(uint8*)"");
        return result;
    }

    data1_ptr = *(MMIFMM_FILE_INFO_T **) i1_data;
    data2_ptr = *(MMIFMM_FILE_INFO_T **) i2_data;   
    
    //+CR230144
    result = CompareName(data1_ptr->filename_ptr, data1_ptr->filename_len,
                 data2_ptr->filename_ptr, data2_ptr->filename_len);
    
    //result = MMIAPICOM_CompareTwoWstrExt(data1_ptr->filename, data1_ptr->filename_len,
    //    data2_ptr->filename, data2_ptr->filename_len, FALSE);
    //-CR230144
    
    return (result);
}

/*****************************************************************************/
//  Description : 比较文件大小
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileSize(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     )
{
    int32               result  = 0;
    MMIFMM_FILE_INFO_T  *data1_ptr = PNULL,*data2_ptr = PNULL;

    //SCI_ASSERT(PNULL != i1_data && PNULL != i2_data);
    if((PNULL == i1_data) || (PNULL == i2_data))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_CompareFileSize param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_779_112_2_18_2_20_7_213,(uint8*)"");
        return result;
    }

    data1_ptr = *(MMIFMM_FILE_INFO_T **) i1_data;
    data2_ptr = *(MMIFMM_FILE_INFO_T **) i2_data;   

    if (data1_ptr->file_size > data2_ptr->file_size)
    {
        result = 1;
    }
    else if (data1_ptr->file_size < data2_ptr->file_size)
    {
        result = -1;
    }
    else 
    {
        result = MMIAPICOM_CompareTwoWstr( data1_ptr->filename_ptr, data1_ptr->filename_len,
            data2_ptr->filename_ptr, data2_ptr->filename_len );
    }

    return (result);
}

/*****************************************************************************/
//  Description : 比较文件时间
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileTime(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     )
{
    MMIFMM_FILE_INFO_T *data1_ptr = PNULL,   *data2_ptr = PNULL;
    int32          result  = 0;

    //SCI_ASSERT(PNULL != i1_data && PNULL != i2_data);
    if((PNULL == i1_data) || (PNULL == i2_data))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_CompareFileTime param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_818_112_2_18_2_20_7_214,(uint8*)"");
        return result;
    }

    data1_ptr = *(MMIFMM_FILE_INFO_T **) i1_data;
    data2_ptr = *(MMIFMM_FILE_INFO_T **) i2_data;   

    if (data1_ptr->time > data2_ptr->time )
    {
        result = 1;
    }
    else if (data1_ptr->time < data2_ptr->time)
    {
        result = -1;
    }
    else 
    {
        result = MMIAPICOM_CompareTwoWstr( data1_ptr->filename_ptr, data1_ptr->filename_len,
            data2_ptr->filename_ptr, data2_ptr->filename_len );
    }

    return (result);
}

/*****************************************************************************/
//  Description : 比较文件类型
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileType(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     )
{
    int32               result  = 0;
    uint16              data1_suffix_offset = 0;
    uint16              data2_suffix_offset = 0;
    MMIFMM_FILE_INFO_T  *data1_ptr = PNULL,*data2_ptr = PNULL;

    //SCI_ASSERT(PNULL != i1_data && PNULL != i2_data);
    if((PNULL == i1_data) || (PNULL == i2_data))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_CompareFileType param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_859_112_2_18_2_20_7_215,(uint8*)"");
        return result;
    }

    data1_ptr = *(MMIFMM_FILE_INFO_T **) i1_data;
    data2_ptr = *(MMIFMM_FILE_INFO_T **) i2_data; 

    for (data1_suffix_offset = (uint16)(data1_ptr->filename_len -1); data1_suffix_offset > 0; data1_suffix_offset--)
    {
        if (MMIFILE_DOT == data1_ptr->filename_ptr[data1_suffix_offset])
        {
            break;
        }
    }
    for (data2_suffix_offset = (uint16)(data2_ptr->filename_len -1);  data2_suffix_offset > 0; data2_suffix_offset--)
    {
        if (MMIFILE_DOT == data2_ptr->filename_ptr[data2_suffix_offset])
        {
            break;
        }
    }

    if (0 == data1_suffix_offset  &&  data2_suffix_offset > 0)
    {
        // 第一个文件没有后缀
        result  = -1;
    }
    else if (0 == data2_suffix_offset  &&  data1_suffix_offset > 0)
    {
        // 第二个文夹没有后缀
        return 1;
    }
    else if (0 == data1_suffix_offset   && 0 == data2_suffix_offset)
    {
        // 都没有后缀，比较文件名
        result = MMIAPICOM_CompareTwoWstrExt( data1_ptr->filename_ptr, data1_ptr->filename_len,
            data2_ptr->filename_ptr, data2_ptr->filename_len, FALSE);
    }
    else
    {
        // 都有后缀，比较后缀
        result = MMIAPICOM_CompareTwoWstrExt( &data1_ptr->filename_ptr[data1_suffix_offset+1], (uint16)(data1_ptr->filename_len - data1_suffix_offset-1),
            &data2_ptr->filename_ptr[data2_suffix_offset+1], (uint16)(data2_ptr->filename_len- data2_suffix_offset-1),FALSE);
    }

    return (result);
}

/*****************************************************************************/
//  Description : 预排序，将文件夹排前，文件排后
//  Global resource dependence :
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_FilePreSort(MMIFMM_DATA_INFO_T* data_ptr)
{
    BOOLEAN result = FALSE;
    if (PNULL == data_ptr)
    {
        SCI_TRACE_LOW("MMIFMM_FilePreSort PNULL == data_ptr!");
        return result;
    }
    if(0 != data_ptr->file_num || 0 != data_ptr->folder_num)    // 移动文件
    {
        MMIFMM_FILE_INFO_T ** file_data_ptr = PNULL;
        MMIFMM_FILE_INFO_T ** folder_data_ptr = PNULL;
        MMIFMM_FILE_INFO_T ** current_ptr = PNULL;
        uint16 filenum = 0;
        uint16 foldernum = 0;
        uint16 j = 0;
        uint16 i = 0;
        if(data_ptr->file_num > 0 && 0 == data_ptr->folder_num)//这种情况不要排文件夹
        {
            return TRUE;
        }
        if( 0 == data_ptr->folder_num)
        {
            return TRUE;
        }
        folder_data_ptr = (MMIFMM_FILE_INFO_T **)SCI_ALLOCA(
            sizeof(MMIFMM_FILE_INFO_T **)*data_ptr->folder_num);
        file_data_ptr = (MMIFMM_FILE_INFO_T **)SCI_ALLOCA(
            sizeof(MMIFMM_FILE_INFO_T **)*data_ptr->file_num);

        if ( PNULL != file_data_ptr && PNULL != folder_data_ptr)
        {
            SCI_MEMSET(folder_data_ptr, 0x00, (sizeof(MMIFMM_FILE_INFO_T**)*data_ptr->folder_num));
            SCI_MEMSET(file_data_ptr, 0x00, (sizeof(MMIFMM_FILE_INFO_T**)*data_ptr->file_num));

            for (i = 0; i < data_ptr->folder_num + data_ptr->file_num; i++)
            {
                if (data_ptr->data[i]->type == MMIFMM_FILE_TYPE_FOLDER)
                {
                    current_ptr = folder_data_ptr + foldernum;
                    *current_ptr = (data_ptr->data[i]);
                    foldernum++;
                }
                else
                {
                    current_ptr = file_data_ptr + filenum;
                    *current_ptr = (data_ptr->data[i]);
                    filenum++;
                }                
            }

            for (i = 0; i < data_ptr->folder_num; i++)
            {
                data_ptr->data[i] = *(folder_data_ptr + i);
            }
            for (i = foldernum,j = 0;j < filenum; j++)
            {
                data_ptr->data[i++] = *(file_data_ptr + j);
            }
            result = TRUE;
            SCI_FREE(folder_data_ptr);
            SCI_FREE(file_data_ptr);
        }
        else
        {
            SCI_TRACE_LOW("MMIFMM_FilePreSort alloc fail!");
        }
    }
    return result;
}

/*****************************************************************************/
//  Description : 读当前目录中的文件数据，
//  Global resource dependence :
//  Author: robert.lu
//  Note: 
/*****************************************************************************/
PUBLIC uint16 MMIFMM_GetCurrentPathFile(// 所读文件和目录的总数
                                           const wchar          *full_path_ptr,     // 全路径,查找条件
                                           //MMIFMM_READ_TYPE_E   type,               // 类型
                                           MMIFMM_DATA_INFO_T   *data_ptr          // 输出数据
                                           //uint16               *output_filename_ptr// 输出数据的文件名
                                           )
{
    uint16                  i = 0;
    uint32                  full_path_len = 0;
    uint32                  find_len = 0;
    uint16                  file_num = 0, folder_num = 0;
    uint16                  index = 0;
    uint16                  filename_len = 0;  
    wchar                   *find_ptr = PNULL;
    MMIFILE_HANDLE          file_handle = 0;
	SFS_ERROR_E             result = SFS_ERROR_NONE;
    MMIFILE_FIND_DATA_T     find_data = {0};
    uint16                  suffix_len = 0;
    wchar                   suffix_name[MMIFMM_FILENAME_LEN+1] = {0};
    MMIFMM_SORT_TYPE_E      sort  = MMIFMM_SORT_TYPE_NAME;
    uint16                  path_depth = 0;
	uint16                  ctlStr[] = L"/A:+NF-H-S";
	uint16                  filter[] = L"*";
    uint16                  *full_file_path_ptr = PNULL;
    uint16                  full_file_path_len = 0;
    
    if((PNULL == full_path_ptr) || (PNULL == data_ptr))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_GetCurrentPathFile param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_983_112_2_18_2_20_7_216,(uint8*)"");
        return 0;
    }

    sort = data_ptr->sort;

    // 计算路径的长度
    full_path_len = MMIAPICOM_Wstrlen( full_path_ptr);

    // 建立查找条件
    find_len  = (full_path_len + 3) * sizeof(wchar);   //需要增加\*,
    find_ptr  = (wchar *)SCI_ALLOC_APP(find_len);    
    if (PNULL == find_ptr)
    {
        //SCI_TRACE_LOW:"MMIFMM: MMIFMM_GetCurrentPathFile, allocate find_ptr failed!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_997_112_2_18_2_20_7_217,(uint8*)"");
        return 0;
    }
    
    SCI_MEMSET(find_ptr, 0, find_len);
    MMI_WSTRNCPY( find_ptr, find_len, full_path_ptr, full_path_len, full_path_len );

    full_file_path_ptr = (wchar *)SCI_ALLOC_APP(sizeof(wchar)*(MMIFMM_PATHNAME_LEN+1)); 
    if (PNULL == full_file_path_ptr)
    {
        SCI_FREE(find_ptr);
        //SCI_TRACE_LOW:"MMIFMM: MMIFMM_GetCurrentPathFile, allocate full_file_path_ptr failed!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1008_112_2_18_2_20_7_218,(uint8*)"");
        return 0;
    }

    //get path depth
    path_depth = MMIFMM_GetPathDepth(full_path_ptr, (uint16)full_path_len);

    // 查找第一个数据
	result = SFS_FindFirstFileCtrl(find_ptr,filter,ctlStr,&file_handle,&find_data,PNULL);

    // 查找下一个数据
    if (SFS_INVALID_HANDLE != file_handle && (SFS_ERROR_NONE == result))
    {
        do 
        {
            filename_len =  (uint16)MMIAPICOM_Wstrlen( find_data.name);
            // 全路径文件名不能超过MMIFMM_FULL_FILENAME_LEN
            if ((full_path_len+filename_len+1) <= MMIFMM_FULL_FILENAME_LEN && (filename_len > 0))
            {
                if (PNULL == data_ptr->data[index])
                {
                    data_ptr->data[index] = FMM_GetFileInfoBuffer();
                    if(PNULL==data_ptr->data[index])
                    {
                        break;
                    }
                }
                SCI_MEMSET(data_ptr->data[index],0x00,sizeof(MMIFMM_FILE_INFO_T));
                
                if (data_ptr->path_is_valid > 0)
                {
                    data_ptr->data[index]->filename_len = (uint16)(full_path_len + filename_len +1);
                }
                else
                {
                    data_ptr->data[index]->filename_len = filename_len;
                }
                if (PNULL != data_ptr->data[index]->filename_ptr)
                {
                    SCI_FREE(data_ptr->data[index]->filename_ptr);
                }
                data_ptr->data[index]->filename_ptr = SCI_ALLOCA((sizeof(wchar)*data_ptr->data[index]->filename_len));
                if (PNULL == data_ptr->data[index]->filename_ptr)
                {
                    SCI_FREE(data_ptr->data[index]);
                    data_ptr->data[index] = PNULL;
                    break;
                }
                SCI_MEMSET(data_ptr->data[index]->filename_ptr,0x00,(sizeof(wchar)*data_ptr->data[index]->filename_len));

                if ((find_data.attr & SFS_ATTR_DIR) > 0)
                {
                    // 目录数据
                    if (MMIFMM_PATH_DEPTH <= path_depth)
                    {
                        //已经达到最大目录深度
                        SCI_FREE(data_ptr->data[index]->filename_ptr);
                        data_ptr->data[index]->filename_ptr = PNULL;
                        continue;
                    }
                    data_ptr->data[index]->type = MMIFMM_FILE_TYPE_FOLDER;
                    data_ptr->data[index]->file_size = 0;
                    folder_num++;
                }
                else
                {
                    SCI_MEMSET(full_file_path_ptr, 0, sizeof(wchar)*(MMIFMM_PATHNAME_LEN+1));
                    full_file_path_len = MMIFMM_PATHNAME_LEN;
                    
                    MMI_WSTRNCPY(full_file_path_ptr, full_file_path_len,
                        full_path_ptr, full_path_len, full_path_len );
                    *(full_file_path_ptr+full_path_len) = MMIFILE_SLASH;
                    MMI_WSTRNCPY(full_file_path_ptr+full_path_len+1, full_file_path_len - full_path_len - 1,
                        find_data.name, filename_len, filename_len );                    
                    full_file_path_len = MMIAPICOM_Wstrlen(full_file_path_ptr);                    
                    
                    // 文件数据
                    if (!MMIFMM_IsFileNeedDisplay(s_suffix_type,full_file_path_ptr,full_file_path_len))
                    {
                        SCI_FREE(data_ptr->data[index]->filename_ptr);
                        data_ptr->data[index]->filename_ptr = PNULL;
                        continue;
                    }
                    data_ptr->data[index]->type = MMIFMM_FILE_TYPE_NORMAL;
                    data_ptr->data[index]->file_size = find_data.length;

                    file_num++;
                }
                data_ptr->data[index]->time = MMIAPICOM_Tm2Second(find_data.modify_time.sec, find_data.modify_time.min, find_data.modify_time.hour,
                                                find_data.modify_Date.mday, find_data.modify_Date.mon, find_data.modify_Date.year);

                if (data_ptr->path_is_valid > 0)
                {
                    // 包含路径  
                    MMI_WSTRNCPY( data_ptr->data[index]->filename_ptr, 
                        data_ptr->data[index]->filename_len, 
                        full_path_ptr, full_path_len, full_path_len );
                    data_ptr->data[index]->filename_ptr[full_path_len] = MMIFILE_SLASH;
                    MMI_WSTRNCPY(&(data_ptr->data[index]->filename_ptr[full_path_len+1]), data_ptr->data[index]->filename_len - full_path_len - 1,
                        find_data.name, filename_len, filename_len );
                }
                else
                {
                    // 不包含路径
                    MMI_WSTRNCPY(data_ptr->data[index]->filename_ptr, data_ptr->data[index]->filename_len,
                        find_data.name, filename_len, filename_len );
                    
                }  
                // 普通文件需要进一步解析文件类型
                if (MMIFMM_FILE_TYPE_NORMAL == data_ptr->data[index]->type)
                {
                    SCI_MEMSET(suffix_name , 0, ((MMIFMM_FILENAME_LEN+1)*sizeof(wchar)));
                    suffix_len = MMIFMM_FILENAME_LEN;
                    if (MMIAPIFMM_SplitFileName(find_data.name,filename_len,
                            PNULL,PNULL,
                            suffix_name,&suffix_len))
                    {
                        data_ptr->data[index]->type = MMIAPIFMM_ConvertFileType(suffix_name, suffix_len);
                    }
                }
            }
        
            if (MMIFMM_FILE_NUM == (file_num +folder_num))
            {
                break;
            }
            index= (uint16)(file_num + folder_num);
        }
        while ((SFS_NO_ERROR == MMIAPIFMM_FindNextFile(file_handle, &find_data))); 
        MMIAPIFMM_FindClose(file_handle);
        data_ptr->file_num = file_num;
        data_ptr->folder_num = folder_num;
    }

    SCI_FREE(find_ptr);
    SCI_FREE(full_file_path_ptr);
    
    i = (uint16)(folder_num+file_num);

    if (i > 0)
    {
        if(MMIFMM_FilePreSort(data_ptr))
        {
            MMIAPIFMM_FileDataSort(data_ptr, sort);
        }
    }
    return (i);
}

/*****************************************************************************/
//  Description : get file type
//  Global resource dependence : 
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMIAPIFMM_GetFileType(
                                    const wchar      *suffix_name_ptr,
                                    uint16      suffix_name_len
                                    )
{
    uint32 i = 0;
    uint8 gb_suffix_name[MMIFMM_FILENAME_LEN + 1] = {0};
    uint16 gb_suffix_name_len = 0;

    //gb_suffix_name_len = GUI_UCS2GB((uint8 *) gb_suffix_name, (uint8 *) suffix_name_ptr, suffix_name_len);
    gb_suffix_name_len = suffix_name_len;
    MMI_WSTRNTOSTR(gb_suffix_name, MMIFMM_FILENAME_LEN, suffix_name_ptr, suffix_name_len, suffix_name_len );

    //忽略大小写
    for(i=0; i<gb_suffix_name_len; i++)
    {
        if('A' <= gb_suffix_name[i] && 'Z' >= gb_suffix_name[i])
        {
           gb_suffix_name[i] = (uint8)(gb_suffix_name[i] - 'A' + 'a');
        }
    }
    for (i = 0; i < ARR_SIZE(s_fmm_all_suffix); i++)
    {
		if(suffix_name_len == strlen((char*)s_fmm_all_suffix[i].suffix))
		{
			if (0 == MMIAPICOM_Stricmp((uint8 *) gb_suffix_name, (uint8 *) s_fmm_all_suffix[i].suffix))
			{
				if (s_fmm_all_suffix[i].is_support)
				{
					return s_fmm_all_suffix[i].type;
				}
				else
				{
					return MMIFMM_UNKNOW_TYPE;
				}
			}
		}
    }
    return  MMIFMM_UNKNOW_TYPE;
}

/*****************************************************************************/
//  Description : convert mime type
//  Global resource dependence : 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_FILE_TYPE_E MMIFMM_ConvertMimeType(
                                                MIME_TYPE_E mime_type
                                                )
{
    MMIFMM_FILE_TYPE_E media_type = MMIFMM_FILE_TYPE_NORMAL;

    switch (mime_type)
    {
    case MIME_TYPE_IMAGE_ANY:                // image start
    case MIME_TYPE_IMAGE_GIF:
    case MIME_TYPE_IMAGE_JPEG:
    case MIME_TYPE_IMAGE_TIFF:
    case MIME_TYPE_IMAGE_PNG:
    case MIME_TYPE_IMAGE_VND_WAP_WBMP:
    case MIME_TYPE_IMAGE_SVG:
    case MIME_TYPE_IMAGE_BMP:
    case MIME_TYPE_IMAGE_X_UP_WPNG:
        media_type = MMIFMM_FILE_TYPE_PICTURE;   // 图片文件
        break;
        
    case MIME_TYPE_TEXT_ANY:         // "text/*"
    case MIME_TYPE_TEXT_HTML:        // "text/html"
    case MIME_TYPE_TEXT_PLAIN:       // "text/plain"
    //case MIME_TYPE_TEXT_X_VCALENDAR: // "text/x-vcalendar
    //case MIME_TYPE_TEXT_X_VCARD:     // "text/x-vcard"
    case MIME_TYPE_TEXT_XML:         // "text/xml"
        media_type = MMIFMM_FILE_TYPE_EBOOK;     // 电子书
        break;
#if defined MMI_VCARD_SUPPORT
    case MIME_TYPE_TEXT_X_VCARD:                // "text/x-vcard"
        media_type = MMIFMM_FILE_TYPE_VCARD;    // 电子书
        break;
#endif
#ifdef JAVA_SUPPORT 
    case MIME_TYPE_APPLICATION_JAVA_VM:
    case MIME_TYPE_APPLICATION_JAVA_JAR:
    case MIME_TYPE_APPLICATION_JAVA_JAD:
        media_type = MMIFMM_FILE_TYPE_JAVA; 	//JAVA 文件
        break;
#endif

    case MIME_TYPE_AUDIO_ANY:                // audio start
    case MIME_TYPE_AUDIO_MID:
    case MIME_TYPE_AUDIO_MPEG:
    case MIME_TYPE_AUDIO_X_WAV:
    case MIME_TYPE_AUDIO_AMR:
    case MIME_TYPE_AUDIO_X_AAC:
    case MIME_TYPE_AUDIO_MIDI:
    case MIME_TYPE_AUDIO_X_MIDI:
    case MIME_TYPE_AUDIO_SP_MIDI:
    case MIME_TYPE_AUDIO_IMELODY:
        media_type = MMIFMM_FILE_TYPE_MUSIC;     // 音乐文件
        break;    

    case MIME_TYPE_VIDEO_MPEG:
    case MIME_TYPE_VIDEO_3GPP:
    case MIME_TYPE_VIDEO_AVI:
    case MIME_TYPE_VIDEO_MP4:
        media_type = MMIFMM_FILE_TYPE_MOVIE;     // 影像文件
        break;

    default:
        break;

    }
    return media_type;
}

/*****************************************************************************/
//  Description : convert suffix type
//  Global resource dependence : 
//  Author: haiwu.chen`
/*****************************************************************************/
PUBLIC uint32 MMIFMM_ConvertMimeTypeToSuffix(
    MIME_TYPE_E mime_type
)
{
    uint32 suffix_type = 0;

    switch (mime_type)
    {        
    case MIME_TYPE_TEXT_PLAIN:                
         suffix_type = MMIFMM_TXT_ALL;
         break;
                    
    case MIME_TYPE_IMAGE_ANY:                // image start
        suffix_type = MMIFMM_PIC_ALL;
        break;
        
    case MIME_TYPE_IMAGE_GIF:
        suffix_type = MMIFMM_PICTURE_GIF;
        break;
        
    case MIME_TYPE_IMAGE_JPEG:
        suffix_type = MMIFMM_PICTURE_JPG;
        break;
        
//    case MIME_TYPE_IMAGE_TIFF:
//        suffix_type = MMIFMM_PICTURE_GIF;
//        break;
        
    case MIME_TYPE_IMAGE_PNG:
        suffix_type = MMIFMM_PICTURE_PNG;
        break;
        
    case MIME_TYPE_IMAGE_VND_WAP_WBMP:
        suffix_type = MMIFMM_PICTURE_WBMP;
        break;
        
//    case MIME_TYPE_IMAGE_SVG:
//        suffix_type = MMIFMM_PIC_ALL;
//        break;
        
    case MIME_TYPE_IMAGE_BMP:
        suffix_type = MMIFMM_PICTURE_BMP;
        break;
        
//    case MIME_TYPE_IMAGE_X_UP_WPNG:
//        suffix_type = MMIFMM_PIC_ALL;
//       break;        
        
    case MIME_TYPE_TEXT_ANY:         // "text/*"
        suffix_type = MMIFMM_TXT_ALL;
        break;
        
//    case MIME_TYPE_TEXT_HTML:        // "text/html"
//        suffix_type = MMIFMM_PIC_ALL;
//        break;
        
//    case MIME_TYPE_TEXT_PLAIN:       // "text/plain"
//        suffix_type = MMIFMM_PIC_ALL;
//         break;
        
//    case MIME_TYPE_TEXT_X_VCALENDAR: // "text/x-vcalendar
//        suffix_type = MMIFMM_PIC_ALL;
//        break;
        
//    case MIME_TYPE_TEXT_X_VCARD:     // "text/x-vcard"
//        suffix_type = MMIFMM_PIC_ALL;
//        break;
        
//    case MIME_TYPE_TEXT_XML:         // "text/xml"
//        suffix_type = MMIFMM_PIC_ALL;
//        break;

#ifdef JAVA_SUPPORT 
//    case MIME_TYPE_APPLICATION_JAVA_VM:
//        suffix_type = MMIFMM_PIC_ALL;
//        break;
        
    case MIME_TYPE_APPLICATION_JAVA_JAR:
        suffix_type = MMIFMM_JAVA_JAR;
        break;
        
    case MIME_TYPE_APPLICATION_JAVA_JAD:
        suffix_type = MMIFMM_JAVA_JAD;
        break;
#endif

    case MIME_TYPE_AUDIO_ANY:                // audio start
        suffix_type = MMIFMM_MUSIC_ALL;
        break;
        
    case MIME_TYPE_AUDIO_MID:
        suffix_type = MMIFMM_MUSIC_MID;
        break;
        
    case MIME_TYPE_AUDIO_MPEG:
        suffix_type = MMIFMM_MUSIC_MP3;
        break;
        
    case MIME_TYPE_AUDIO_X_WAV:
        suffix_type = MMIFMM_MUSIC_WAV;
        break;
        
    case MIME_TYPE_AUDIO_AMR:
        suffix_type = MMIFMM_MUSIC_AMR;
        break;     
 
    case MIME_TYPE_AUDIO_X_AAC:
        suffix_type = MMIFMM_MUSIC_ACC;
        break;
		
    case MIME_TYPE_AUDIO_WMA:
        suffix_type = MMIFMM_MUSIC_WMA;
        break;
		
    case MIME_TYPE_AUDIO_MIDI:
    case MIME_TYPE_AUDIO_X_MIDI:
    case MIME_TYPE_AUDIO_SP_MIDI:
        suffix_type = MMIFMM_MUSIC_MIDI;
        break;      
		
    case MIME_TYPE_VIDEO_MP4:
    case MIME_TYPE_VIDEO_MPEG:
        suffix_type = MMIFMM_MOVIE_MP4;
        break;
        
    case MIME_TYPE_VIDEO_3GPP:
        suffix_type = MMIFMM_MOVIE_3GP;
        break;
#ifdef MMI_KING_MOVIE_SUPPORT
     case MIME_TYPE_VIDEO_KMV:
        suffix_type = MMIFMM_MOVIE_KMV;
        break;
    case MIME_TYPE_VIDEO_SMV:    //smv_support
        suffix_type = MMIFMM_MOVIE_SMV;
        break;
#ifdef MMI_KING_MOVIE_SUPPORT_HMV
    case MIME_TYPE_VIDEO_HMV:    //hmv_support
        suffix_type = MMIFMM_MOVIE_HMV;
        break;
#endif
#endif
    case MIME_TYPE_VIDEO_AVI:
        suffix_type = MMIFMM_MOVIE_AVI;
        break;        

    default:
        break;

    }
    return suffix_type;
}

/*****************************************************************************/
//  Description : get file type
//  Global resource dependence : 
//  Author: Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_FILE_TYPE_E MMIAPIFMM_ConvertFileType(
                                                const wchar      *suffix_name_ptr,
                                                uint16      suffix_name_len
                                                )
{
    MMIFMM_FILE_TYPE_E type = MMIFMM_FILE_TYPE_NORMAL;
    switch(MMIAPIFMM_GetFileType(suffix_name_ptr,suffix_name_len))
    {
        case MMIFMM_PICTURE_JPG:
        case MMIFMM_PICTURE_GIF:
        case MMIFMM_PICTURE_BMP:
        case MMIFMM_PICTURE_WBMP:
        case MMIFMM_PICTURE_PNG:
            type = MMIFMM_FILE_TYPE_PICTURE;
            break;
        case MMIFMM_MUSIC_MP3:
        case MMIFMM_MUSIC_WMA:
        case MMIFMM_MUSIC_MID:
        case MMIFMM_MUSIC_AMR:
        case MMIFMM_MUSIC_ACC:
        case MMIFMM_MUSIC_M4A:
        case MMIFMM_MUSIC_WAV:
        //case MMIFMM_MUSIC_MIDI: ==MMIFMM_MUSIC_MID
//        case MMIFMM_MOVIE_KUR:
            type = MMIFMM_FILE_TYPE_MUSIC;
            break;
#ifdef VIDEO_PLAYER_SUPPORT
        case MMIFMM_MOVIE_RMVB:
        case MMIFMM_MOVIE_MP4:
        case MMIFMM_MOVIE_3GP:
        case MMIFMM_MOVIE_AVI:
        case MMIFMM_MOVIE_FLV: 
#ifdef MMI_KING_MOVIE_SUPPORT
        case MMIFMM_MOVIE_KMV: 
        case MMIFMM_MOVIE_SMV: //smv_support
#ifdef MMI_KING_MOVIE_SUPPORT_HMV
        case MMIFMM_MOVIE_HMV: 
#endif
#endif
            type = MMIFMM_FILE_TYPE_MOVIE;
            break;
#endif
#ifdef EBOOK_SUPPORT			
        case MMIFMM_MOVIE_TXT:
        case MMIFMM_MOVIE_LRC:
            type = MMIFMM_FILE_TYPE_EBOOK;
            break;
#endif
#if defined MMI_VCARD_SUPPORT
        case MMIFMM_MOVIE_VCF:
            type = MMIFMM_FILE_TYPE_VCARD;
            break;
#endif
#ifdef MMI_VCALENDAR_SUPPORT
		case MMIFMM_MOVIE_VCS:
            type = MMIFMM_FILE_TYPE_VCALENDAR;
            break;
#endif
        case MMIFMM_MOVIE_SZIP:
            type = MMIFMM_FILE_TYPE_SZIP;
            break;
        case MMIFMM_FONT:
            type = MMIFMM_FILE_TYPE_FONT;
            break;
#ifdef  JAVA_SUPPORT
		case MMIFMM_JAVA_JAD:
		case MMIFMM_JAVA_JAR:
            type = MMIFMM_FILE_TYPE_JAVA;			
			break;
#endif
#ifdef	QBTHEME_SUPPORT
		case MMIFMM_THEME_QB:
			type = MMIFMM_FILE_TYPE_THEME;
			break;
#endif
        case MMIFMM_BROWSER_HTM:
        case MMIFMM_BROWSER_HTML:
        case MMIFMM_BROWSER_XML :
            type = MMIFMM_FILE_TYPE_BROWSER;	
            break;
			
#ifdef DRM_SUPPORT
        case MMIFMM_DM_FILE:
        case MMIFMM_DCF_FILE:
            type = MMIFMM_FILE_TYPE_DRM;
            break;
#endif

        case MMIFMM_UNKNOW_TYPE:
        default:
            type = MMIFMM_FILE_TYPE_NORMAL;
            break;
    }
    return type;
}
/*****************************************************************************/
//  Description : get movie type
//  Global resource dependence : 
//  Author: Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC MMICOM_VIDEO_TYPE_E MMIAPIFMM_ConvertMovieFileType(
                                                  const wchar    *suffix_name_ptr,
                                                  uint16    suffix_name_len
                                                  )
{

    MMICOM_VIDEO_TYPE_E  type = MMICOM_VIDEO_TYPE_MAX;
    switch (MMIAPIFMM_GetFileType(suffix_name_ptr,suffix_name_len))
    {
        case MMIFMM_MOVIE_MP4:
            type = MMICOM_VIDEO_TYPE_MP4;
            break;
        case MMIFMM_MOVIE_3GP:
            type = MMICOM_VIDEO_TYPE_3GP;
            break;
#ifdef MMI_KING_MOVIE_SUPPORT
        case MMIFMM_MOVIE_KMV:
            type = MMICOM_VIDEO_TYPE_KMV;
            break;
        case MMIFMM_MOVIE_SMV://smv_support
            type=MMICOM_VIDEO_TYPE_SMV;
            break;
#ifdef MMI_KING_MOVIE_SUPPORT_HMV
        case MMIFMM_MOVIE_HMV://hmv_support
            type=MMICOM_VIDEO_TYPE_HMV;
            break;
#endif
#endif
            
#ifdef AVI_DEC_SUPPORT
        case MMIFMM_MOVIE_AVI:
            type = MMICOM_VIDEO_TYPE_AVI;
            break;
#endif
        case MMIFMM_MOVIE_FLV:
            type = MMICOM_VIDEO_TYPE_FLV;
            break;

        case MMIFMM_MOVIE_RMVB:
            type = MMICOM_VIDEO_TYPE_RMVB;
            break;
        default:
            type = MMICOM_VIDEO_TYPE_MAX;
            break;
    }
    return (type);
}

/*****************************************************************************/
//  Description : get music type
//  Global resource dependence : 
//  Author: Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC MMISRVAUD_RING_FMT_E MMIAPIFMM_ConvertMusicFileType(
                                                  const wchar    *suffix_name_ptr,
                                                  uint16    suffix_name_len
                                                  )
{
    MMISRVAUD_RING_FMT_E  type = MMISRVAUD_RING_FMT_MAX;
	uint32 			file_type = 0;
	file_type = MMIAPIFMM_GetFileType(suffix_name_ptr,suffix_name_len);
    switch (file_type) 
    {
        case MMIFMM_MUSIC_MP3:
            type = MMISRVAUD_RING_FMT_MP3;
            break;
        case MMIFMM_MUSIC_WMA:
            type = MMISRVAUD_RING_FMT_WMA;
            break;
        //case MMIFMM_MUSIC_MIDI: == MMIFMM_MUSIC_MID
        case MMIFMM_MUSIC_MID:
            type = MMISRVAUD_RING_FMT_MIDI;
            break;
        case MMIFMM_MUSIC_AMR:
            type = MMISRVAUD_RING_FMT_AMR;
            break;
        case MMIFMM_MUSIC_ACC:
            type = MMISRVAUD_RING_FMT_AAC;
            break;
        case MMIFMM_MUSIC_M4A:
            type = MMISRVAUD_RING_FMT_M4A;
            break;
        case MMIFMM_MUSIC_WAV:
            type = MMISRVAUD_RING_FMT_WAVE;
            break;
//        case MMIFMM_MOVIE_KUR:
//            type = MMISRVAUD_RING_FMT_MAX;
//            break;
        default:
            type = MMISRVAUD_RING_FMT_MAX;
            break;
    }
    return  type;
}

/*****************************************************************************/
//  Description : get the type of folder
//  Global resource dependence :
//  Author: robert.lu
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_FOLDER_TYPE_E MMIAPIFMM_GetFolderType(
                                                    const wchar      *path_ptr,
                                                    uint16      path_len,
                                                    BOOLEAN     is_include_path
                                                    )
{
    uint16     offset = 0;
    
    //SCI_TRACE_LOW:"MMIFMM: MMIAPIFMM_GetFolderType, path len = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1637_112_2_18_2_20_9_219,(uint8*)"d", path_len);
    //SCI_ASSERT(PNULL != path_ptr);
    //SCI_PASSERT(path_len > 0,(("path_len = %d"),path_len));
    if(PNULL == path_ptr)
    {
        //SCI_TRACE_LOW:"MMIFMM:MMIAPIFMM_GetFolderType PNULL == path_ptr is error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1642_112_2_18_2_20_9_220,(uint8*)"");
        return MMIFMM_FOLDER_TYPE_NUM;
    }
    
    if(0 == path_len)
    {
        //SCI_TRACE_LOW:"MMIFMM:MMIAPIFMM_GetFolderType path_len = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1648_112_2_18_2_20_9_221,(uint8*)"d",path_len);
        return MMIFMM_FOLDER_TYPE_NUM;
    }
    
    // 是否包含路径
    if (is_include_path)
    {
        offset = MMIFMM_PATH_NAME_OFFSET;
    }
#ifdef CMCC_UI_STYLE    
    // 判断是否是Photos
    if (MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PHOTOS) == (path_len - offset)) // path 去掉设备缅（1)， ":\"(2)长度
    {
        if(0 == MMIAPICOM_CompareTwoWstrExt( &path_ptr[offset],(uint16)(path_len -offset),
            MMIMULTIM_DIR_PHOTOS,MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PHOTOS),FALSE))
        {
            return MMIFMM_FOLDER_TYPE_PICTURE;
        }
    }
#endif    
    // 判断是否是Photos
    if (MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE) == (path_len - offset)) // path 去掉设备缅（1)， ":\"(2)长度
    {
        if(0 == MMIAPICOM_CompareTwoWstrExt( &path_ptr[offset],(uint16)(path_len -offset),
            MMIMULTIM_DIR_PICTURE,MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE),FALSE))
        {
            return MMIFMM_FOLDER_TYPE_PICTURE;
        }
    }	
    // 判断是否是Audio
    if (MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC) == (path_len - offset)) // path 去掉设备缅（1)， ":\"(2)长度
    {
        if(0 == MMIAPICOM_CompareTwoWstrExt(  &path_ptr[offset],(uint16)(path_len -offset),
            MMIMULTIM_DIR_MUSIC,MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC),FALSE))
        {
            return MMIFMM_FOLDER_TYPE_MUSIC;
        }
    }
    
    
    // 判断是否是Video
    if (MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE) == (path_len - offset)) // path 去掉设备缅（1)， ":\"(2)长度
    {
        if(0 == MMIAPICOM_CompareTwoWstrExt( &path_ptr[offset],(uint16)(path_len -offset),
            MMIMULTIM_DIR_MOVIE,MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE),FALSE))
        {
            return MMIFMM_FOLDER_TYPE_MOVIE;
        }
    }
#ifdef MMI_KING_MOVIE_SUPPORT
    // 判断是否是KingMovie
    if (MMIAPICOM_Wstrlen(MMIMULTIM_DIR_KINGMOVIE) == (path_len - offset)) // path 去掉设备缅（1)， ":\"(2)长度
    {
        if(0 == MMIAPICOM_CompareTwoWstrExt( &path_ptr[offset],(uint16)(path_len -offset),
            MMIMULTIM_DIR_KINGMOVIE,MMIAPICOM_Wstrlen(MMIMULTIM_DIR_KINGMOVIE),FALSE))
        {
            return MMIFMM_FOLDER_TYPE_KINGMOVIE;
        }
    }
#endif
#ifdef EBOOK_SUPPORT    
    // 判断是否是EBOOK
    if (MMIAPICOM_Wstrlen(MMIMULTIM_DIR_EBOOK) == (path_len - offset)) // path 去掉设备缅（1)， ":\"(2)长度
    {
        if(0 == MMIAPICOM_CompareTwoWstrExt( &path_ptr[offset],(uint16)(path_len -offset),
                             MMIMULTIM_DIR_EBOOK,MMIAPICOM_Wstrlen(MMIMULTIM_DIR_EBOOK),FALSE))
        {
            return MMIFMM_FOLDER_TYPE_EBOOK;
        }
    }
#endif

    return (MMIFMM_FOLDER_TYPE_NORMAL);
}

/*****************************************************************************/
//  Description : sort
//  Global resource dependence :
//  Author: robert.lu
//  Note:
/*****************************************************************************/
PUBLIC int MMIAPIFMM_FileDataSort(
                                  MMIFMM_DATA_INFO_T    *list_data_ptr,
                                  MMIFMM_SORT_TYPE_E    sort
                                  )
{
    //SCI_TRACE_LOW:"MMIFMM: MMIAPIFMM_FileDataSort, sort =%d."
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1732_112_2_18_2_20_9_222,(uint8*)"d", sort);
    //SCI_ASSERT(PNULL != list_data_ptr);
    if(PNULL == list_data_ptr)
    {
        //SCI_TRACE_LOW:"MMIFMM: MMIAPIFMM_FileDataSort, param error:PNULL == list_data_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1736_112_2_18_2_20_9_223,(uint8*)"");
        return 0;
    }

    switch(sort)
    {
    case MMIFMM_SORT_TYPE_NAME:
        // 排序文件夹
        if (0 < list_data_ptr->folder_num)
        {
            qsort(list_data_ptr->data, list_data_ptr->folder_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileName);
        }

        // 排序文件
        qsort(list_data_ptr->data+list_data_ptr->folder_num, list_data_ptr->file_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileName);
        list_data_ptr->sort  = MMIFMM_SORT_TYPE_NAME;
         break;

    case MMIFMM_SORT_TYPE_TIME:
        // 排序文件夹
        if (0 < list_data_ptr->folder_num)
        {
            qsort(list_data_ptr->data, list_data_ptr->folder_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileTime);
        }

        // 排序文件
        qsort(list_data_ptr->data+list_data_ptr->folder_num, list_data_ptr->file_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileTime);
        list_data_ptr->sort  = MMIFMM_SORT_TYPE_TIME;
        break;
    case MMIFMM_SORT_TYPE_SIZE:
        if (0 < list_data_ptr->folder_num)
        {
            qsort(list_data_ptr->data, list_data_ptr->folder_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileSize);
        }

        // 排序文件
        qsort(list_data_ptr->data+list_data_ptr->folder_num, list_data_ptr->file_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileSize);
        list_data_ptr->sort  = MMIFMM_SORT_TYPE_SIZE;
        break;

    case MMIFMM_SORT_TYPE_TYPE:
        // 排序文件夹
        if (0 < list_data_ptr->folder_num)
        {
            qsort(list_data_ptr->data, list_data_ptr->folder_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileType);
        }

        // 排序文件
        qsort(list_data_ptr->data+list_data_ptr->folder_num, list_data_ptr->file_num, sizeof(MMIFMM_FILE_INFO_T*), MMIAPIFMM_CompareFileType);
        list_data_ptr->sort  = MMIFMM_SORT_TYPE_TYPE;
        break;

    default:
     //SCI_PASSERT(0,(("sort = %d"),sort));    
        //SCI_TRACE_LOW:"MMIAPIFMM_FileDataSort sort = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1778_112_2_18_2_20_9_224,(uint8*)"d",sort);
        break;
    }

    return (1);
}

/*****************************************************************************/
//  Description : get file information
//  Global resource dependence : 
//  Author:liqing.peng
//  Modify: xinrui.wang
//  Note: 
/*****************************************************************************/
BOOLEAN MMIFMM_GetFileInfo(
                           const wchar        *full_path_ptr,     //in
                           uint16       full_path_len,      //in
                           uint32       *create_time_ptr,   //out
                           uint32       *file_size_ptr      //out
                           )
{
    BOOLEAN             result = FALSE;
    SFS_HANDLE          sfs_handle = 0;
    SFS_FIND_DATA_T     find_data = {0};

    //SCI_ASSERT(NULL != full_path_ptr);
    if(PNULL == full_path_ptr)
    {
        //SCI_TRACE_LOW:"MMIFMM_GetFileInfo param error:PNULL == full_path_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1804_112_2_18_2_20_9_225,(uint8*)"");
        return FALSE;
    }
//    SCI_ASSERT(0 < full_path_len && MMIFMM_PATHNAME_LEN>= full_path_len);

    sfs_handle = MMIAPIFMM_FindFirstFile(full_path_ptr, full_path_len,&find_data);

    if(SFS_INVALID_HANDLE != sfs_handle)
    {
        if(!(SFS_ATTR_DIR & find_data.attr))
        {
            if(NULL != create_time_ptr)
            {
                *create_time_ptr = MMIAPICOM_TansferSFSTime(find_data.modify_Date, find_data.modify_time);
            }

            if(NULL != file_size_ptr)
            {
                *file_size_ptr = find_data.length;
            }

            result = TRUE;
        }
    }

    SFS_FindClose(sfs_handle);


    return (result);
}

/*****************************************************************************/
//  Description : delete file syn
//  Global resource dependence : 
//  Author: liqing.peng
//  Modify: xinrui.wang
//  Note:
/*****************************************************************************/ 
BOOLEAN MMIFMM_DeleteFileSyn(
                             const wchar *full_path_ptr, //in
                             uint16     full_path_len   //in
                             )
{
    BOOLEAN     result = FALSE;

    //SCI_ASSERT(NULL != full_path_ptr);
    //SCI_ASSERT(0 < full_path_len && MMIFMM_PATHNAME_LEN >= full_path_len);
    if((PNULL == full_path_ptr) || (0 == full_path_len) || (MMIFMM_PATHNAME_LEN < full_path_len))
    {
        //SCI_TRACE_LOW:"MMIFMM_DeleteFileSyn param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1851_112_2_18_2_20_9_226,(uint8*)"");
        return FALSE;
    }

    if(SFS_ERROR_NONE == SFS_DeleteFile( full_path_ptr, NULL))
    {
        result = TRUE;
    }

    return (result);
}


/*****************************************************************************/
//  Description : 更新当前的文件列表
//  Global resource dependence :
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_UpdateCurrentFileLIst(void)
{                        
    //update file list
    MMK_CloseWin(MMIFMM_PICTURE_PREVIEW_WIN_ID);
	MMK_CloseWin(MMIFMM_PIC_PREVIEW_OPT_MENU_WIN_ID);
	MMK_SendMsg(MMIFMM_UDISK_CHILD_WIN_ID, MSG_FMM_UPDATE_LIST_DATA, (ADD_DATA)TRUE);


    if(MMK_IsOpenWin(MMIFMM_OPEN_FILE_BYLAYER_WIN_ID))
    {
        MMK_PostMsg(MMIFMM_OPEN_FILE_BYLAYER_WIN_ID, MSG_FMM_OPENFILEWIN_RELOAD, NULL, 0);
    }
}

/*****************************************************************************/
//  Description : malloc file info buffer
//  Global resource dependence :
//  Author: jian ma
//  Note:
/*****************************************************************************/
PUBLIC void * FMM_GetFileInfoBuffer(void)
{
    int32   i = 0;
    int32   j = s_buffer_index;
    while(1)/*lint !e716*/
    {
        if(PNULL==s_pointer_buffer[j])
        {
            s_pointer_buffer[j] = (MMIFMM_FILE_INFO_T *)SCI_ALLOCA(sizeof(MMIFMM_FILE_INFO_T)*MMI_FMM_MAX_ALLOC_NUM);

            if(PNULL==s_pointer_buffer[j])
            {
                //SCI_TRACE_LOW:"FMM: can't malloc memory "
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_1953_112_2_18_2_20_10_227,(uint8*)"");
                if(s_buffer_index>0)
                {
                    s_buffer_index--;
                }
                return PNULL;
            }
            SCI_MEMSET( s_pointer_buffer[j], 0, (sizeof(MMIFMM_FILE_INFO_T)*MMI_FMM_MAX_ALLOC_NUM));
        }
        for (i=0; i<MMI_FMM_MAX_ALLOC_NUM; i++)
        {
            if(!s_buffer[i])
            {
                s_buffer[i]=TRUE;
                break;
            }
        }
        if(i==MMI_FMM_MAX_ALLOC_NUM)
        {
            j++;
            if(j<MMI_FMM_MAX_BUFFER_NUM)
            {
                SCI_MEMSET(s_buffer,0x00,(MMI_FMM_MAX_ALLOC_NUM*sizeof(BOOLEAN)));
                s_buffer_index = j;
            }
            else
            {
                break;
            }
        }
        else 
        {
            break;
        }
    }
    if(j<MMI_FMM_MAX_BUFFER_NUM)
    {
        return ((MMIFMM_FILE_INFO_T *)s_pointer_buffer[j]+i);
    }
    else
    {
        return PNULL;
    }
 }

/*****************************************************************************/
//  Description : free file info buffer
//  Global resource dependence :
//  Author: jian ma
//  Note:
/*****************************************************************************/
PUBLIC void MMIFMM_FreeFileInfoBuffer(BOOLEAN is_free_all)
{
    int32 i = 0;
    if (is_free_all)
    {
        for(i = 0 ; i<MMI_FMM_MAX_BUFFER_NUM; i++)
        {
            if (PNULL!=s_pointer_buffer[i])
            {
                SCI_FREE(s_pointer_buffer[i]);
            }
        }
    }
    else
    {
        for(i = 0 ; i<MMI_FMM_MAX_BUFFER_NUM; i++)
        {
            if (PNULL!=s_pointer_buffer[i])
            {
                SCI_MEMSET(s_pointer_buffer[i],0x00,(sizeof(MMIFMM_FILE_INFO_T)*MMI_FMM_MAX_ALLOC_NUM));
            }
        }
    }
    s_buffer_index = 0;
    SCI_MEMSET(s_buffer,0x00,(MMI_FMM_MAX_ALLOC_NUM*sizeof(BOOLEAN)));

 }
/*****************************************************************************/
//  Description : free file info 
//  Global resource dependence :
//  Author: jian ma
//  Note:
/*****************************************************************************/
PUBLIC void MMIFMM_FreeFileInfo(MMIFMM_DATA_INFO_T *list_data_ptr,BOOLEAN is_free_all)
{
    uint16 i = 0;
    if ( PNULL != list_data_ptr)
    {
        for (i = 0 ; i < list_data_ptr->file_num+list_data_ptr->folder_num;i++)
        {
            if (PNULL != list_data_ptr->data[i])
            {
                if (PNULL != list_data_ptr->data[i]->filename_ptr)
                {
                    SCI_FREE(list_data_ptr->data[i]->filename_ptr);
                }
            }
        }
    }
    MMIFMM_FreeFileInfoBuffer(is_free_all);
}
/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author:liqing.peng
//  Modify: xinrui.wang
//  Note: 
/*****************************************************************************/
uint16 MMIFMM_GetPathDepth(
                           const wchar *path_ptr,
                           uint16   path_len
                           )
{
    int32   i = 0;
    uint16  depth = 1;

    for(i=0; i<path_len; i++)
    {
        if(MMIFILE_SLASH == path_ptr[i])
        {
            depth++;
        }
    }

    return depth;
}

/*****************************************************************************/
//  Description : get all device info and check if have enough space 
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_IsEnoughSpace(uint32 size)
{
    //uint32 free_high_word = 0;
    //uint32 free_low_word = 0;
    BOOLEAN result = FALSE;
    MMIFILE_DEVICE_E fs_dev = MMI_DEVICE_SDCARD;
    
    if(SFS_ERROR_NONE == MMIAPIFMM_GetSuitableFileDev(fs_dev, size, &fs_dev))
    {
        result = TRUE;
    }
        
    return result;
}


/*****************************************************************************/
//  Description : get dev info and check if have enough space
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsDevEnoughSpace(uint32 size,MMIFILE_DEVICE_E dev)
{
    uint32                  free_space_low = 0;
    uint32                  free_space_high = 0;
    BOOLEAN                 result = TRUE;

	if(dev < MMI_DEVICE_NUM)
	{
		MMIAPIFMM_GetDeviceFreeSpace(MMIAPIFMM_GetDevicePath(dev), MMIAPIFMM_GetDevicePathLen(dev),
            &free_space_high, &free_space_low);
	}
    else 
    {
        return FALSE;
    }
	
	if ( 0 ==free_space_high && size > free_space_low )
	{
		//SCI_TRACE_LOW:"MMIAPIFMM_IsDevEnoughSpace: free spaced high value is = %d free space low value is = %d"
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2139_112_2_18_2_20_10_228,(uint8*)"dd", free_space_high,free_space_low);
		result = FALSE;
	}
    
    return result;
}
/*****************************************************************************/
//  Description : set display file type
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC void MMIFMM_SetDisplayFileType(uint32 suffix_type)
{
    s_suffix_type = suffix_type;
}

/*****************************************************************************/
//  Description : get display file type
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC uint32 MMIFMM_GetDisplayFileType(void)
{
    return s_suffix_type;
}

/*****************************************************************************/
//  Description : is file need display
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_IsFileNeedDisplay(uint32 suffix_type,const wchar *name_ptr,uint16 name_len)
{
    wchar  suffix_name[MMIFMM_FILENAME_LEN + 1] = {0};
    uint16 suffix_len = MMIFMM_FILENAME_LEN;    
    uint32 actual_file_type = 0;
    
    if (MMIFMM_FILE_ALL == suffix_type)
    {
        return TRUE;
    }
    if(MMIFMM_UNKNOW_TYPE == suffix_type)
    {
        return FALSE;
    }   

    if(!MMIAPIFMM_SplitFileName(name_ptr,name_len, PNULL,PNULL, suffix_name,&suffix_len))    
    {
        return FALSE;
    }   
    actual_file_type = MMIAPIFMM_GetFileType(suffix_name,suffix_len);

	if((MMIFMM_DRM_ALL&(suffix_type)) && (MMIFMM_DRM_ALL&(actual_file_type)))
	{
		//drm file
        if(!(MMIFMM_MEDIA_TYPE(suffix_type)&MMIFMM_MEDIA_TYPE(actual_file_type)))
		{
           return FALSE;
		}
	}
    else if((MMIFMM_MEDIA_TYPE(suffix_type)&MMIFMM_MEDIA_TYPE(actual_file_type)) == 0//小类不相同
		  ||(MMIFMM_FILE_TYPE(suffix_type) != MMIFMM_FILE_TYPE(actual_file_type))//不是drm
		)
    {
		//not drm file, not same file type or not same media type
        return FALSE;
    }
#ifdef DRM_SUPPORT
    if(MMIFMM_DRM_ALL == suffix_type || MMIFMM_DCF_FILE == suffix_type || MMIFMM_DM_FILE == suffix_type)
    {
        return TRUE;
    }
    if((actual_file_type & MMIFMM_DRM_ALL) != 0)
    {
        MMIFILE_HANDLE drm_handle = 0;
        DRM_RIGHTS_T rights_info = {0};
        uint32 drm_suffix_type = 0;
        
        drm_handle = SFS_CreateFile(name_ptr, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, 0, 0);
        if (!MMIAPIDRM_IsDRMFile(drm_handle, PNULL))
        {                        
            //非drm文件，则继续原有处理
            MMIAPIFMM_CloseFile(drm_handle);
            return FALSE;
        }
        
        /*是DRM文件，则求出此文件的mine type*/
        MMIAPIDRM_GetRightsinfo(drm_handle, PNULL, &rights_info);
        drm_suffix_type = MMIFMM_ConvertMimeTypeToSuffix(rights_info.mime_type);
        MMIAPIFMM_CloseFile(drm_handle);
        if (0 == (drm_suffix_type & suffix_type)) 
        {
            return FALSE;
        }
    }
#endif
        return TRUE;
}
/*****************************************************************************/
// Description : set common character
// Global resource dependence : 
// Author:jian.ma
// Note: 
/*****************************************************************************/
LOCAL void SetCommonCharacter( GUIRICHTEXT_ITEM_T* p_item )
{
    //SCI_PASSERT(PNULL != p_item,("[MMIMPEG4]:SetCommonCharacter,param error"));   
    if(PNULL == p_item)
    {
        //SCI_TRACE_LOW:"[MMIFMM]:SetCommonCharacter,param error:PNULL == p_item"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2243_112_2_18_2_20_10_229,(uint8*)"");
        return;
    }

    p_item->img_type = GUIRICHTEXT_IMAGE_NONE;
    p_item->text_type = GUIRICHTEXT_TEXT_BUF;
}
/*****************************************************************************/
// Description : setting file detail
// Global resource dependence : 
// Author:jian.ma
// Note: 
/*****************************************************************************/
PUBLIC void MMIFMM_SetFileDetail(
                                 MMIFMM_DETAIL_DATA_T       *detail_data,
                                 MMI_CTRL_ID_T              ctrl_id,
                                 MMI_WIN_ID_T            win_id

                                 )
{
    MMI_TM_T                    tm = {0};
    int32                       i = 0;
    uint8                       detail_arr[MMIFMM_FULL_FILENAME_LEN] = {0};
    wchar                       wchar_detail_arr[MMIFMM_FULL_FILENAME_LEN + 1] = {0};
    uint16                      nIndex = 0;

    uint16                      pic_width = 0;
    uint16                      pic_height = 0; 
    uint16                      string_len=0;
#ifdef MMI_AUDIO_PLAYER_SUPPORT
    MMISRVAUD_RING_FMT_E             audio_type = MMISRVAUD_RING_FMT_MAX;
    MMISRVAUD_CONTENT_INFO_T          mp3_file_info = {0};
#endif

//set name title
    SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));   
    string_len = (uint16)MMIAPICOM_Wstrlen( detail_data->filename);
    for(i=string_len-1;MMIFILE_SLASH !=detail_data->filename[i] && i> 0;i--)
    {
        NULL;
    }

    AddOneDetailItem(ctrl_id,TXT_INPUT_FILE_NAME,PNULL,&nIndex,&detail_data->filename[i+1],(string_len-i-1));

//set date
    tm = MMIAPICOM_Second2Tm(detail_data->time);
    MMIAPISET_FormatDateStrByDateStyle(tm.tm_year, tm.tm_mon, tm.tm_mday,'.',detail_arr,MMIFMM_FULL_FILENAME_LEN);

    MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
    AddOneDetailItem(ctrl_id,TXT_COMMON_DATE,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));

//set time
    SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));

    MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,detail_arr,MMIFMM_FULL_FILENAME_LEN);
    MMIAPICOM_StrToWstr(detail_arr,wchar_detail_arr );
    AddOneDetailItem(ctrl_id,TXT_COMMON_TIME,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));

//set size 
    //SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));   
    //sprintf((char *)detail_arr, "%ld.%03ldK", (detail_data->file_size /1024), ((detail_data->file_size %1024)*1000/1024));
	MMIAPIFMM_GetFileSizeString(detail_data->file_size, wchar_detail_arr, MMIFMM_SIZE_STR_LEN, FALSE);
	string_len = MMIAPICOM_Wstrlen(wchar_detail_arr);
    //MMIAPICOM_StrToWstr((char*) detail_arr,wchar_detail_arr );
    AddOneDetailItem(ctrl_id,TXT_DETAIL_SIZE,PNULL,&nIndex,wchar_detail_arr, string_len);
     
//set location
    AddOneDetailItem(ctrl_id,
             TXT_LOCATION ,
             MMIAPIFMM_GetDeviceName(MMIAPIFMM_GetDeviceTypeByPath(&detail_data->filename[0], MMIFILE_DEVICE_LEN)),
             &nIndex,PNULL,0);
    
    if(MMIFMM_FILE_TYPE_PICTURE ==detail_data->type)
    {
        SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
        SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
        if(0 < detail_data->file_size)
        {
            MMIAPIFMM_GetPictureWidthHeight(
                                        detail_data->filename, detail_data->filename_len,
                                        detail_data->file_size,
                                        &pic_width,
                                        &pic_height);
            if(0 != pic_width && 0 != pic_height)
            {
                sprintf((char *)detail_arr, "%dX%d",(uint16)pic_width, (uint16)pic_height);
                MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
                AddOneDetailItem(ctrl_id,TXT_COMM_RESOLUTION,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));
            }
            else
            {
                AddOneDetailItem(ctrl_id,TXT_COMM_RESOLUTION,TXT_COMMON_NO_SUPPORT,&nIndex,PNULL,0);
            }
        }
        else
        {
            AddOneDetailItem(ctrl_id,TXT_COMM_RESOLUTION,TXT_EMPTY_FILE,&nIndex,PNULL,0);    
        }
    }

#ifdef MMI_AUDIO_PLAYER_SUPPORT
    //get time duration,song and singer
    if (MMIFMM_FILE_TYPE_MUSIC ==detail_data->type)
    {
#ifdef DRM_SUPPORT
        if(!MMIAPIMP3_IsDrmFile(
            detail_data->filename, 
            detail_data->filename_len, 
            &audio_type,PNULL,PNULL))
#endif
        {
            audio_type = MMIAPICOM_GetMusicType(detail_data->filename,detail_data->filename_len);
        }

        switch (audio_type)
        {
        case MMISRVAUD_RING_FMT_MP3:
        case MMISRVAUD_RING_FMT_AAC:
        case MMISRVAUD_RING_FMT_M4A:
        case MMISRVAUD_RING_FMT_WMA:
        case MMISRVAUD_RING_FMT_WAVE:
        case MMISRVAUD_RING_FMT_MIDI:
        case MMISRVAUD_RING_FMT_AMR:
            MMISRVAUD_GetFileContentInfo(
                detail_data->filename, 
                MMIAPICOM_Wstrlen(detail_data->filename),
                &mp3_file_info
                );
            {
                //set duration
                SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
                SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
                sprintf((char*)detail_arr, "%.2ld:%.2ld:%.2ld", ((mp3_file_info.total_time)/(60*60)), (((mp3_file_info.total_time)/60)%60), ((mp3_file_info.total_time)%60));
                MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
                AddOneDetailItem(ctrl_id,TXT_TOTAL_TIME,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));
                //SCI_TRACE_LOW:"[MMIFMM]: tag_code_type=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2379_112_2_18_2_20_10_230,(uint8*)"d",mp3_file_info.id3_tag.tag_code_type);
                //song name title
                if((1 == mp3_file_info.id3_tag.tag_code_type && MMIAPICOM_Wstrlen((wchar*)mp3_file_info.id3_tag.title) > 0)
                    ||(0 == mp3_file_info.id3_tag.tag_code_type && SCI_STRLEN(mp3_file_info.id3_tag.title) > 0))
                {
                    //SCI_TRACE_LOW:"[MMIFMM]: SONG title DETAIL!"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2384_112_2_18_2_20_10_231,(uint8*)"");
                    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
                    if (1 == mp3_file_info.id3_tag.tag_code_type) //unicode;
                    {
                        string_len = (sizeof(mp3_file_info.id3_tag.title))/(sizeof(wchar));
                        //这里不能用strlen(),否则遇到包含0x00的unicode会被截断，导致显示不全；
                        string_len = MIN(string_len,MMIFMM_FULL_FILENAME_LEN);
                        MMIAPICOM_Wstrncpy(wchar_detail_arr,(wchar*)mp3_file_info.id3_tag.title,string_len);                      
                    }
                    else
                    {
                        string_len = strlen(mp3_file_info.id3_tag.title);
                        string_len = MIN(string_len,MMIFMM_FULL_FILENAME_LEN);
                        string_len = GUI_GBToWstr(wchar_detail_arr, (uint8*)mp3_file_info.id3_tag.title,string_len);
                    }
                    AddOneDetailItem(ctrl_id,TXT_MP3_SONG,PNULL,&nIndex,wchar_detail_arr,string_len);
                    
                }
                //artist name title
                if((1 == mp3_file_info.id3_tag.tag_code_type && MMIAPICOM_Wstrlen((wchar*)mp3_file_info.id3_tag.artist) > 0)
                    ||(0 == mp3_file_info.id3_tag.tag_code_type && SCI_STRLEN(mp3_file_info.id3_tag.artist) > 0))
                {
                    //SCI_TRACE_LOW:"[MMIFMM]: artist artist DETAIL!"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2406_112_2_18_2_20_10_232,(uint8*)"");
                    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
                    if (1 == mp3_file_info.id3_tag.tag_code_type) //unicode;
                    {
                        string_len = (sizeof(mp3_file_info.id3_tag.artist))/(sizeof(wchar));
                        //这里不能用strlen(),否则遇到包含0x00的unicode会被截断，导致显示不全；
                        string_len = MIN(string_len,MMIFMM_FULL_FILENAME_LEN);
                        MMIAPICOM_Wstrncpy(wchar_detail_arr,(wchar*)mp3_file_info.id3_tag.artist,string_len);                      
                    }
                    else
                    {
                        string_len = strlen(mp3_file_info.id3_tag.artist);
                        string_len = MIN(string_len,MMIFMM_FULL_FILENAME_LEN);
                        string_len = GUI_GBToWstr(wchar_detail_arr, (uint8*)mp3_file_info.id3_tag.artist,string_len);
                    }
                    AddOneDetailItem(ctrl_id,TXT_MP3_SINGER,PNULL,&nIndex,wchar_detail_arr,string_len);
                    
                }
            }
            break;
            
        default:
            break;
        }
    }
#endif

#ifdef VIDEO_PLAYER_SUPPORT
    if(MMIFMM_FILE_TYPE_MOVIE ==detail_data->type)
    {
        //set pixel title
        DPLAYER_MEDIA_INFO_T media_info = {0};        
        //MMIAUDIO_PauseBgPlay(MMIBGPLAY_MODULE_FMM);//video interface need

        MMIAPIVP_GetInfoFromFullPathName(detail_data->filename, detail_data->filename_len,&media_info);

        SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
        SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
        sprintf((char*)detail_arr, "%ldX%ld",media_info.video_info.width,media_info.video_info.height);
        MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
        AddOneDetailItem(ctrl_id,TXT_COMM_RESOLUTION,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));

        //set duration title
        SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
        SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
        sprintf((char*)detail_arr, "%.2ld:%.2ld:%.2ld", ((media_info.media_length)/(1000*3600)), (((media_info.media_length)/(1000*60))%60), (((media_info.media_length)/1000)%60));
        MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
        AddOneDetailItem(ctrl_id,TXT_TOTAL_TIME,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));
        
        //MMIAUDIO_ResumeBgPlay(MMIBGPLAY_MODULE_FMM);
    }
#endif

#ifdef DRM_SUPPORT
    if(MMIFMM_FILE_TYPE_DRM == detail_data->type && 0 < detail_data->filename_len)
    {
        MMIAPIFMM_AddDrmFileDetail(detail_data->filename, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, 0, 0, ctrl_id);
    }    
#endif

}

/*****************************************************************************/
//  Description : add one detail item
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
LOCAL void AddOneDetailItem(
                            MMI_CTRL_ID_T   ctrl_id,
                            MMI_TEXT_ID_T   label_title,
                            MMI_TEXT_ID_T   label_name,
                            uint16          *index_ptr,
                            wchar           *detail_ptr,
                            uint32          detail_len
                            )
{
    GUIRICHTEXT_ITEM_T item_data = {0};/*lint !e64*/
    MMI_STRING_T detail_str = {0};/*lint !e64*/

    //set XXX title
    SetCommonCharacter(&item_data);
    MMI_GetLabelTextByLang( label_title, &detail_str );
    item_data.text_data.buf.len = detail_str.wstr_len;
    item_data.text_data.buf.str_ptr = detail_str.wstr_ptr;
    GUIRICHTEXT_AddItem (ctrl_id,&item_data,index_ptr);

    //set XXX name
    if ( PNULL == label_name && PNULL!=detail_ptr )
    {
        SCI_MEMSET(&item_data,0,sizeof(item_data));
        SetCommonCharacter(&item_data);
        item_data.text_data.buf.len = (uint16)detail_len;   
        item_data.text_data.buf.str_ptr = detail_ptr;
        GUIRICHTEXT_AddItem (ctrl_id,&item_data,index_ptr);
    }
    else if (PNULL != label_name)
    {
        SCI_MEMSET(&item_data,0,sizeof(item_data));
        SCI_MEMSET(&detail_str,0,sizeof(detail_str));
        SetCommonCharacter(&item_data);
        MMI_GetLabelTextByLang( label_name, &detail_str );
        item_data.text_data.buf.len = detail_str.wstr_len;
        item_data.text_data.buf.str_ptr = detail_str.wstr_ptr;
        GUIRICHTEXT_AddItem (ctrl_id,&item_data,index_ptr);
    }
    else
    {
        //SCI_TRACE_LOW:"AddOneDetailItem fail "
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2512_112_2_18_2_20_11_233,(uint8*)"");
    }
    
}
/*****************************************************************************/
//  Description : get pic data for pic detail info
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetPictureWidthHeight(
                                        const wchar       *full_path_ptr,
                                        const uint16      full_path_len,
                                        uint32            file_size,
                                        uint16            *width_ptr,
                                        uint16            *height_ptr
                                        )
{
    BOOLEAN     result = FALSE;
    //uint8       *data_ptr = NULL;
    //uint32      data_size = 0;

    //SCI_ASSERT(NULL != full_path_ptr);
    //SCI_ASSERT(0 < full_path_len);
    //SCI_ASSERT(NULL != width_ptr);
    //SCI_ASSERT(NULL != height_ptr);
    if((PNULL == full_path_ptr) || (0 == full_path_len) || (PNULL == width_ptr) || (PNULL == height_ptr))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_GetPictureWidthHeight param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2538_112_2_18_2_20_11_234,(uint8*)"");
        return FALSE;
    }
    
    if (0 != file_size)
    {
        result = GUIANIM_GetImgWidthHeight(width_ptr,height_ptr,full_path_ptr,full_path_len);/*lint !e605*/
    }

    return result;
}
/*****************************************************************************/
//  Description : 把搜索类型转化成字符串形式
//  Global resource dependence : 
//  Author:liqingpeng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_GetFilerInfo(MMIFMM_FILTER_T * filter,uint32 suffix_type)
{
    int32 i = 0;   
	int32 total_count = 0;  
	uint32       str_len = 0;   
    if (MMIFMM_FILE_ALL == suffix_type)
    {
        MMI_MEMCPY(filter->filter_str , FMM_FILTER_STRING_MAXLEN ,
               "*.*", 3, 3);
        return;
    }

	total_count = ARR_SIZE(s_fmm_all_suffix);
    for (i = 0; i < total_count; i++)
    {

		if (((MMIFMM_DRM_ALL & MMIFMM_MEDIA_TYPE(s_fmm_all_suffix[i].type))//drm type
			 ||(MMIFMM_FILE_TYPE(suffix_type) == MMIFMM_FILE_TYPE(s_fmm_all_suffix[i].type))//大类别相同
			 )//drm type or same file type
			&&(MMIFMM_MEDIA_TYPE(suffix_type)&MMIFMM_MEDIA_TYPE(s_fmm_all_suffix[i].type))//小类别包含
			&&(s_fmm_all_suffix[i].is_support)
			)
		{
           str_len = strlen((char*)filter->filter_str);
           MMI_MEMCPY(filter->filter_str + str_len, (FMM_FILTER_STRING_MAXLEN - str_len),
               "*.", 2, 2);
           
           str_len += 2;
           MMI_MEMCPY(filter->filter_str + str_len, (FMM_FILTER_STRING_MAXLEN - str_len),
               s_fmm_all_suffix[i].suffix, strlen((char*)s_fmm_all_suffix[i].suffix), strlen((char*)s_fmm_all_suffix[i].suffix));/*lint !e666*/

           str_len = strlen((char*)filter->filter_str);
           MMI_MEMCPY(filter->filter_str + str_len, (FMM_FILTER_STRING_MAXLEN - str_len),
               " ", 1, 1);
       }
    }
}

/*****************************************************************************/
//  Description : select a music from fmm
//  Global resource dependence : 
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectMusicWin(MMIFMM_SELECT_INFO_T *select_info)
{
    uint16  dev[MMIFILE_DEVICE_NAME_MAX_LEN+1] = {0};
    uint16  dev_len = 0;
    uint16  dir[MMIFILE_DIR_NAME_MAX_LEN+1] = {0};
    uint16  dir_len = 0;
    uint16  find_path[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16 find_path_len = 0;
    uint16 filter_len = 0;
    MMIFMM_FILTER_T         filter = {0};
    MMIFMM_FUNC_TYPE_E      func_type = 0;/*lint !e64*/
	MMIFILE_DEVICE_E     device = MMI_DEVICE_SDCARD;
    int32   i = 0;
    
    if (PNULL == select_info)
    {
        return FALSE;
    }
    MMIAPIFMM_CloseSelectFileWin();
    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    else if (MMIAPIENG_GetIQModeStatus())
    {
        MMIPUB_OpenAlertWarningWin(TXT_IQ_DATA_PROCESSING);
        return FALSE;
    }

    if (!MMIAPIUDISK_IsDeviceReady())
    {
        return FALSE;
    }

    if (PNULL == s_select_win_param_ptr)
    {
        s_select_win_param_ptr = (MMIFMM_SELECT_WIN_PARAM_T *)SCI_ALLOCA(sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    }
    //保存参数

    SCI_MEMSET(s_select_win_param_ptr, 0, sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    s_select_win_param_ptr->win_id = select_info->win_id;
    s_select_win_param_ptr->max_size = select_info->max_size;
    if(NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        MMI_WSTRNCPY(s_select_win_param_ptr->ticked_file, MMIFILE_FULL_PATH_MAX_LEN,
                   select_info->ticked_file_wstr, select_info->ticked_file_len, select_info->ticked_file_len);
        s_select_win_param_ptr->ticked_file_len = select_info->ticked_file_len;
    }
    else
    {
        SCI_MEMSET(s_select_win_param_ptr->ticked_file, 0, (MMIFILE_FULL_PATH_MAX_LEN*sizeof(wchar)));
        s_select_win_param_ptr->ticked_file_len = 0;
    }

    //计算打开的目录
    if (NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        dev_len = MMIFILE_DEVICE_NAME_MAX_LEN;
        dir_len = MMIFILE_DIR_NAME_MAX_LEN;
        find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
        MMIAPIFMM_SplitFullPath(select_info->ticked_file_wstr, select_info->ticked_file_len,dev, &dev_len, dir, &dir_len, NULL, NULL);
        MMIAPIFMM_CombineFullPath(dev, dev_len, dir, dir_len, NULL, 0, find_path, &find_path_len);
    }
    else 
	{
        device = MMIAPIFMM_GetFirstValidDevice();
        if(MMI_DEVICE_NUM > device)
        {
            find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
		    if (!MMIAPIFMM_CombineFullPath(MMIAPIFMM_GetDevicePath(device), MMIAPIFMM_GetDevicePathLen(device),
			    MMIMULTIM_DIR_MUSIC, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC),
			    PNULL, 0,
			    find_path, &find_path_len))
		    {
			    if (PNULL != s_select_win_param_ptr)
			    {
				    SCI_FREE(s_select_win_param_ptr);
				    s_select_win_param_ptr = PNULL;
			    }
			    return FALSE;
		    }
        }
        else
        {
			if (PNULL != s_select_win_param_ptr)
			{
				SCI_FREE(s_select_win_param_ptr);
				s_select_win_param_ptr = PNULL;
			}
			return FALSE;
        }
    }

    s_select_win_param_ptr->ring_vol = select_info->ring_vol;
    //把搜索类型转化成字符串形式
    for (i = 0; i<ARR_SIZE(select_info->select_file);i++)/*lint !e574 !e737*/
    {
        MMIAPIFMM_GetFilerInfo(&filter,select_info->select_file[i]);
    }
    SCI_MEMSET(s_filter.filter_str,0,(sizeof(char))*(FMM_FILTER_STRING_MAXLEN+2));
    filter_len = strlen((char*)filter.filter_str);
    SCI_MEMCPY(s_filter.filter_str, filter.filter_str, (sizeof(uint8)*(filter_len+1)));
    MMIFMM_SetOpenFileWinDisp(WIN_DISP_1LINE);  
    func_type = FUNC_FIND_FILE_AND_ALLFOLER;
    MMIFMM_OpenFileWinByLayerInternal(find_path, find_path_len,
       &filter, func_type, FALSE,
       HandleSelectMusicWinMsg, NULL, NULL, FALSE, FALSE);
    
    return TRUE;
}


/*****************************************************************************/
//  Description : handle select ring list win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectMusicWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                )
{
    BOOLEAN                         result = FALSE;
    MMIFMM_SELECT_RETURN_T          selected_info = {0};

    //SCI_ASSERT(NULL != file_data_ptr);
    if(PNULL == file_data_ptr)
    {
        //SCI_TRACE_LOW:"OpenSelectMusicWinCbk param error:PNULL == file_data_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_2727_112_2_18_2_20_11_235,(uint8*)"");
        return FALSE;
    }

#ifdef DRM_SUPPORT
    if (PNULL != s_select_win_param_ptr
        && ((0 != s_select_win_param_ptr->drm_limit.cons_mode_limit)
        || (0 != s_select_win_param_ptr->drm_limit.method_level_limit)))
    {
        //如果用户设置了限制类型，则检查此文件是否支持用户的要求
        if (IsSelDrmConstraintFile(file_data_ptr->filename, 
            file_data_ptr->name_len, s_select_win_param_ptr->drm_limit))
        {
            return FALSE;
        }
    }
#endif

    selected_info.file_size = file_data_ptr->size;
    selected_info.file_type = MMIFMM_FILE_TYPE_MUSIC;
    selected_info.music_type = MMIAPICOM_GetMusicType(file_data_ptr->filename, file_data_ptr->name_len);

    MMIAPICOM_Wstrncpy(selected_info.file_name_ptr, file_data_ptr->filename, file_data_ptr->name_len);
    selected_info.file_name_len = file_data_ptr->name_len;

    selected_info.data_buf_ptr = NULL;
    MMK_SendMsg(s_select_win_param_ptr->win_id, MSG_MULTIM_SELECTED_RETURN, (void*)&selected_info);

    result = TRUE;

	return result;

}

/*****************************************************************************/
//  Description : handle select ring list win
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectMusicWinMsg(
                                  MMI_WIN_ID_T          win_id,     
                                  MMI_MESSAGE_ID_E      msg_id, 
                                  DPARAM                param
                                  )
{
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    static uint8        s_splay_timer = 0;
    static MMIFMM_MUSIC_INFO_STATUS_E     s_music_info_status = MMIFMM_MUSIC_INFO_NONE;
    FILEARRAY_DATA_T    file_data = {0};

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        if(0 != s_splay_timer)
        {
            MMK_StopTimer(s_splay_timer);
            s_splay_timer = 0;
        }
        s_music_info_status = MMIFMM_MUSIC_INFO_UNKNOWN;
#ifndef MMI_PDA_SUPPORT
        s_splay_timer =  MMK_CreateWinTimer(win_id, MMIFMM_PLAYPER_TIME, FALSE);
#endif

#ifdef MMI_PDA_SUPPORT
        MMIFMM_SetIsHavePreviewIcon(TRUE);
#endif
        result = MMI_RESULT_TRUE;
        /* Pause Back ground sound */
        MMISRVAUD_ReqVirtualHandle("FMM SELECT RING", MMISRVAUD_PRI_NORMAL);
        break;

    case MSG_FMM_OPENFILEWIN_RELOAD:    
        if(MMIFMM_MUSIC_INFO_NONE != s_music_info_status)
        {
#ifndef MMI_PDA_SUPPORT
            s_splay_timer =  MMK_CreateWinTimer(win_id, MMIFMM_PLAYPER_TIME, FALSE);
#endif
            s_music_info_status = MMIFMM_MUSIC_INFO_UNKNOWN;
        }
        break;
    case MSG_APP_UPSIDE:
    case MSG_APP_DOWNSIDE:
    case MSG_KEYREPEAT_UPSIDE:
    case MSG_KEYREPEAT_DOWNSIDE:
        result = MMI_RESULT_TRUE;
        break;        

    case MSG_APP_DOWN:
    case MSG_APP_UP:
    case MSG_CTL_LIST_MOVEDOWN:
    case MSG_CTL_LIST_MOVEUP: 
#ifndef MMI_PDA_SUPPORT
    case MSG_CTL_LIST_MOVEBOTTOM:
    case MSG_CTL_LIST_MOVETOP:
    case MSG_CTL_LIST_NXTPAGE:
    case MSG_CTL_LIST_PREPAGE:
#endif
#ifdef MMI_PDA_SUPPORT      
   case MSG_NOTIFY_LIST_CHECK_CONTENT:
#endif
        MMIAPISET_StopAppRing();

        if(0 != s_splay_timer)
        {
            MMK_StopTimer(s_splay_timer);
            s_splay_timer = 0;
        }
        s_splay_timer =  MMK_CreateWinTimer(win_id, MMIFMM_PLAYPER_TIME, FALSE);
        s_music_info_status = MMIFMM_MUSIC_INFO_UNKNOWN;
        result = MMI_RESULT_TRUE;
        break;

    case MSG_TIMER:
        if (0 != s_splay_timer && s_splay_timer ==  *( GUI_TIMER_ID_T*)param)
        {
            //start play file
            s_is_music_preview = TRUE;
            if(MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
			{
				if ( -1 == s_select_win_param_ptr->ring_vol)
				{
					s_select_win_param_ptr->ring_vol = MMIFMM_PLAY_RING_DEFAULT_VOL;
				}
				if (FILEARRAY_TYPE_FILE == file_data.type)
				{
					if (0 == file_data.size)
					{
						MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
						s_music_info_status = MMIFMM_MUSIC_INFO_NOT_SUPPORT;
					}
             	   else
               	   {
#ifdef DRM_SUPPORT
                    DRMFILE_PRE_CHECK_STATUS_E pre_check_drmfile_status = 
                        MMIAPIDRM_PreCheckFileStatus(file_data.filename, DRM_PERMISSION_PLAY);
                    
                    if (DRMFILE_PRE_CHECK_NORMAL != pre_check_drmfile_status)
                    {
                        if (DRMFILE_PRE_CHECK_NO_RIGHTS == pre_check_drmfile_status)
                        {
                            MMIAPIFMM_UpdateSelectListIconData(file_data.filename, file_data.name_len);
                        }
                    
                        s_music_info_status = MMIFMM_MUSIC_INFO_DRM_FAIL;
                        break;
                    }
#endif					
					if (MMIAPIFMM_PlayMusicFile(MMISET_RING_TYPE_OTHER, FALSE,
						file_data.filename, file_data.name_len, 
						1, 0, s_select_win_param_ptr->ring_vol, NULL))
					{
						s_music_info_status = MMIFMM_MUSIC_INFO_SUPPORT;
					}
					else
					{
						if(MMIAPICC_IsInState(CC_IN_CALL_STATE))
						{
							s_music_info_status = MMIFMM_MUSIC_INFO_SUPPORT;
						}
						else
						{
							MMIPUB_OpenAlertWarningWin(TXT_COMMON_NO_SUPPORT);
							s_music_info_status = MMIFMM_MUSIC_INFO_NOT_SUPPORT;
						}
					}
				}
			}
        }
       }
        result = MMI_RESULT_TRUE;
        break;
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        if (PNULL != param)
        {
            uint32 src_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(src_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
            {
                break;
            }
            else
            {
                result = MMI_RESULT_TRUE;
                MMK_SendMsg(win_id, MSG_APP_OK, PNULL);
                break;
            }
        }
        break;
#else
    case MSG_CTL_PENOK:
#endif
    case MSG_APP_OK:    
    case MSG_CTL_OK:
    case MSG_APP_WEB:   
    case MSG_CTL_MIDSK:
//        if ( MMK_IsSingleClick(msg_id, param) && !MMIAPIMMS_IsEditMMS())
//        {
//            break;
//        }
        MMIAPISET_StopAppRing();
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
		{
			break;
		}
        if (FILEARRAY_TYPE_FILE == file_data.type)
        {
            if(0 == file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
            }
            else if(!MMIAPIMMS_IsEditMMS() && 0 != s_select_win_param_ptr->max_size && s_select_win_param_ptr->max_size < file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_FILESIZE_TOOBIG);
            }
            else
            {                
#ifdef DRM_SUPPORT
                DRMFILE_PRE_CHECK_STATUS_E pre_check_drmfile_status = DRMFILE_PRE_CHECK_NORMAL;

                if (MMIAPIMMS_IsEditMMS())
                {
                    pre_check_drmfile_status = MMIAPIDRM_PreCheckFileStatusExForMMS(file_data.filename, DRM_PERMISSION_PLAY, FALSE);
                }
                else
                {
                    pre_check_drmfile_status = MMIAPIDRM_PreCheckFileStatus(file_data.filename, DRM_PERMISSION_PLAY);
                }                    
                
                if (DRMFILE_PRE_CHECK_NORMAL != pre_check_drmfile_status)
                {
                    if (DRMFILE_PRE_CHECK_NO_RIGHTS == pre_check_drmfile_status)
                    {
                        MMIAPIFMM_UpdateSelectListIconData(file_data.filename, file_data.name_len);
                    }
                    
                    s_music_info_status = MMIFMM_MUSIC_INFO_DRM_FAIL;
                }
                else
#endif	
                {
                    if(MMIAPICC_IsInState(CC_IN_CALL_STATE))
                    {
                        s_music_info_status = MMIFMM_MUSIC_INFO_IN_CALL_STATE;
                    }
                    else if(!MMIAPIFMM_PlayMusicFileEx(MMISET_RING_TYPE_OTHER,FALSE,FALSE,
                        file_data.filename, file_data.name_len,
                        1, 0, s_select_win_param_ptr->ring_vol, NULL))
                    {
                        s_music_info_status = MMIFMM_MUSIC_INFO_NOT_SUPPORT;
                    }
                    MMIAPISET_StopAppRing();
                }
           
                if (MMIAPIMMS_IsEditMMS())
                {
                    if (!OpenSelectMusicWinCbk(&file_data))
                    {
#ifdef DRM_SUPPORT                        
                        MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
#endif
                    }
                    
                    //MMK_CloseWin(win_id);
                    return result;
                }
                else
                {
                    if (MMIFMM_MUSIC_INFO_NOT_SUPPORT == s_music_info_status)
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_COMMON_NO_SUPPORT);
                    }
#ifdef DRM_SUPPORT
                    else if (MMIFMM_MUSIC_INFO_DRM_FAIL == s_music_info_status)
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
                    }
#endif
                    else if(MMIFMM_MUSIC_INFO_IN_CALL_STATE == s_music_info_status)
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_FMM_CC_IN_CALL_STATE_CANNOT_CHOOSE_MUSIC);
                    }
                    else
                    {
                        if (!OpenSelectMusicWinCbk(&file_data))
                        {
#ifdef DRM_SUPPORT                    	
                            MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
#endif
                            return result;
                        }
                        //MMK_CloseWin(win_id);
                        return result;
                    }                    
                }                  
            }            
            result = MMI_RESULT_TRUE;
        }
        break;

    case MSG_LOSE_FOCUS:
        if(0 != s_splay_timer)
        {
            MMK_StopTimer(s_splay_timer);
            s_splay_timer = 0;
        }
        MMIAPISET_StopAppRing();
        s_is_music_preview = FALSE;
        //enable key ring
        //MMIDEFAULT_EnableKeyRing(TRUE);
        //MMIDEFAULT_EnableTpRing(TRUE);
        break;

    case MSG_GET_FOCUS:
        //disable key ring
        //MMIDEFAULT_EnableKeyRing(FALSE);
        //MMIDEFAULT_EnableTpRing(FALSE);
        break;

    case MSG_CLOSE_WINDOW:
        s_is_music_preview = FALSE;
        MMIFMM_SetOpenFileWinDisp(WIN_DISP_1LINE);
        if(0 != s_splay_timer)
        {
            MMK_StopTimer(s_splay_timer);
            s_splay_timer = 0;
        }
        s_music_info_status = MMIFMM_MUSIC_INFO_NONE;
        MMIAPISET_StopAppRing();

        if (PNULL != s_select_win_param_ptr)
        {
           SCI_FREE(s_select_win_param_ptr);
           s_select_win_param_ptr = PNULL;
        }

#ifdef MMI_PDA_SUPPORT
        MMIFMM_SetIsHavePreviewIcon(FALSE);
#endif
        /* resume background sound */
        MMISRVAUD_FreeVirtualHandle("FMM SELECT RING");
        result = MMI_RESULT_TRUE;
        break;

    default:
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : play music file 
//  Global resource dependence : 
//  Author: aoke.hu
//  Note: add is_consume para
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_PlayMusicFileEx(
                        MMISET_ALL_RING_TYPE_E         ring_type,
                        BOOLEAN                        is_a2dp,
                        BOOLEAN                        is_consume,
                        const wchar                    *full_path_ptr,
                        uint16                         full_path_len,
                        uint32                         play_times,
                        uint32                         play_offset,
                        uint32                         vol,
                        MMISET_RING_CALLBACK_PFUNC    call_back
                        )
{
#ifdef MMISRV_AUDIO_SUPPORT
    return MMIAPISET_PlayMusicFile(ring_type, is_a2dp, is_consume, full_path_ptr, full_path_len, play_times, play_offset, vol, call_back);
#else
    return FALSE;
#endif
}

/*****************************************************************************/
//  Description : play music file 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_PlayMusicFile(
                        MMISET_ALL_RING_TYPE_E         ring_type,
                        BOOLEAN                        is_a2dp,
                        const wchar                    *full_path_ptr,
                        uint16                         full_path_len,
                        uint32                         play_times,
                        uint32                         play_offset,
                        uint32                         vol,
                        MMISET_RING_CALLBACK_PFUNC    call_back
                        )
{
    return MMIAPIFMM_PlayMusicFileEx(ring_type,is_a2dp,TRUE,full_path_ptr,full_path_len,play_times,play_offset,vol,call_back);
}



/*****************************************************************************/
//  Description : select a movie from fmm
//  Global resource dependence : 
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectMovieWin(MMIFMM_SELECT_INFO_T *select_info)
{
    uint16  dev[MMIFILE_DEVICE_NAME_MAX_LEN+1] = {0};
    uint16  dev_len = 0;
    uint16  dir[MMIFILE_DIR_NAME_MAX_LEN+1] = {0};
    uint16  dir_len = 0;
    uint16  find_path[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16 find_path_len = 0;
    uint16 filter_len = 0;
    MMIFMM_FILTER_T         filter = {0};
    MMIFMM_FUNC_TYPE_E      func_type = 0;/*lint !e64*/
    MMIFILE_DEVICE_E     device = MMI_DEVICE_SDCARD;
    int32 i = 0;

    if (PNULL == select_info)
    {
        return FALSE;
    }
    MMIAPIFMM_CloseSelectFileWin();

    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    else if (MMIAPIENG_GetIQModeStatus())
    {
        MMIPUB_OpenAlertWarningWin(TXT_IQ_DATA_PROCESSING);
        return FALSE;
    }

    if (!MMIAPIUDISK_IsDeviceReady())
    {
        return FALSE;
    }

    if ( PNULL == s_select_win_param_ptr)
    {
        s_select_win_param_ptr = (MMIFMM_SELECT_WIN_PARAM_T *)SCI_ALLOCA(sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    }
    //保存参数
    SCI_MEMSET(s_select_win_param_ptr, 0x00, sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    s_select_win_param_ptr->win_id = select_info->win_id;
    s_select_win_param_ptr->max_size = select_info->max_size;
    if(NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
    MMI_WSTRNCPY(s_select_win_param_ptr->ticked_file, MMIFILE_FULL_PATH_MAX_LEN,
                select_info->ticked_file_wstr, select_info->ticked_file_len, select_info->ticked_file_len);
                s_select_win_param_ptr->ticked_file_len = select_info->ticked_file_len;
    }
    else
    {
        SCI_MEMSET(s_select_win_param_ptr->ticked_file, 0, (MMIFILE_FULL_PATH_MAX_LEN*sizeof(wchar)));
        s_select_win_param_ptr->ticked_file_len = 0;
    }

    //计算打开的目录
    if (NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        dev_len = MMIFILE_DEVICE_NAME_MAX_LEN;
        dir_len = MMIFILE_DIR_NAME_MAX_LEN;
        find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
        MMIAPIFMM_SplitFullPath(select_info->ticked_file_wstr, select_info->ticked_file_len,dev, &dev_len, dir, &dir_len, NULL, NULL);
        MMIAPIFMM_CombineFullPath(dev, dev_len, dir, dir_len, NULL, 0, find_path, &find_path_len);
    }
    else 
	{

        device = MMIAPIFMM_GetFirstValidDevice();

        if (MMIAPIFMM_GetDeviceStatus(MMIAPIFMM_GetDevicePath(device), MMIAPIFMM_GetDevicePathLen(device)))
        {
            find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
            if (!MMIAPIFMM_CombineFullPath(MMIAPIFMM_GetDevicePath(device), MMIAPIFMM_GetDevicePathLen(device),
                MMIMULTIM_DIR_MOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE),
                PNULL, 0,
                find_path, &find_path_len))
            {
                if (PNULL != s_select_win_param_ptr)
                {
                    SCI_FREE(s_select_win_param_ptr);
                    s_select_win_param_ptr = PNULL;
                }
                return FALSE;
            }
        }          
        else
        {
			//no valid device
			if (PNULL != s_select_win_param_ptr)
			{
				SCI_FREE(s_select_win_param_ptr);
				s_select_win_param_ptr = PNULL;
			}
			return FALSE;
        }
	}

    //把搜索类型转化成字符串形式
    for (i = 0; i<ARR_SIZE(select_info->select_file);i++)/*lint !e574 !e737*/
    {
        MMIAPIFMM_GetFilerInfo(&filter,select_info->select_file[i]);
    }
    
    MMIFMM_SetOpenFileWinDisp(WIN_DISP_1LINE);
    func_type = FUNC_FIND_FILE_AND_ALLFOLER;
    SCI_MEMSET(s_filter.filter_str,0,(sizeof(char))*(FMM_FILTER_STRING_MAXLEN+2));
    filter_len = strlen((char*)filter.filter_str);
    SCI_MEMCPY(s_filter.filter_str, filter.filter_str, (sizeof(uint8)*(filter_len+1)));
    MMIFMM_OpenFileWinByLayerInternal(find_path, find_path_len,
                            &filter, func_type, FALSE,
                            HandleSelectMovWinMsg, NULL, NULL, FALSE, FALSE);

    return TRUE;
}

#if defined FONT_TYPE_SUPPORT_VECTOR && defined FILE_FONT_SUPPORT
/*****************************************************************************/
//  Description : handle select font win msg
//  Global resource dependence : 
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectFontWinMsg(
                                          MMI_WIN_ID_T        win_id,
                                          MMI_MESSAGE_ID_E    msg_id,
                                          DPARAM              param
                                          )
{
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    FILEARRAY_DATA_T    file_data = {0};

    switch(msg_id)
    {
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        if (PNULL != param)
        {
            uint32 src_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(src_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
            {
                break;
            }
            else
            {
                result = MMI_RESULT_TRUE;
                MMK_SendMsg(win_id, MSG_APP_OK, PNULL);
                break;
            }
        }
        break;
#else
    case MSG_CTL_PENOK:
#endif
    case MSG_APP_WEB:   
    case MSG_CTL_MIDSK:
    case MSG_APP_OK:    
    case MSG_CTL_OK:
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
		{
			break;
		}
        
        if (FILEARRAY_TYPE_FILE == file_data.type)
        {
            MMIFMM_SELECT_RETURN_T selected_info = {0};
            
            selected_info.file_size = file_data.size;
            selected_info.file_type = MMIFMM_FILE_TYPE_FONT;
            
            MMIAPICOM_Wstrncpy(selected_info.file_name_ptr, file_data.filename, file_data.name_len);
            selected_info.file_name_len = file_data.name_len;

            MMK_SendMsg(s_select_win_param_ptr->win_id, MSG_MULTIM_SELECTED_RETURN, (void*)&selected_info);

            //MMK_CloseWin(win_id);
            result = MMI_RESULT_TRUE;
        }
        break;

    case MSG_CLOSE_WINDOW:
        if (PNULL != s_select_win_param_ptr)
        {
           SCI_FREE(s_select_win_param_ptr);
           s_select_win_param_ptr = PNULL;
        }
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }

    //if (MMI_RESULT_FALSE == result)
    //{
        //选择文件窗口默认处理函数
    //    result = MMIFMM_HandleOpenFileByLayerWin(win_id, msg_id, param);
    //}

    return result;
}

/*****************************************************************************/
//  Description : select a font from fmm
//  Global resource dependence : 
//  Author: james.zhang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectFontWin(MMIFMM_SELECT_INFO_T *select_info)
{
    uint16  dev[MMIFILE_DEVICE_NAME_MAX_LEN+1] = {0};
    uint16  dev_len = 0;
    uint16  dir[MMIFILE_DIR_NAME_MAX_LEN+1] = {0};
    uint16  dir_len = 0;
    uint16  find_path[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16 find_path_len = 0;
    uint16 filter_len = 0;
    MMIFMM_FILTER_T         filter = {0};
    MMIFMM_FUNC_TYPE_E      func_type = 0;/*lint !e64*/
    MMIFILE_DEVICE_E     device = MMI_DEVICE_SDCARD;
    int32 i = 0;
    if (PNULL == select_info)
    {
        return FALSE;
    }
    MMIAPIFMM_CloseSelectFileWin();

    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    else if (MMIAPIENG_GetIQModeStatus())
    {
        MMIPUB_OpenAlertWarningWin(TXT_IQ_DATA_PROCESSING);
        return FALSE;
    }

    if (!MMIAPIUDISK_IsDeviceReady())
    {
        return FALSE;
    }

    if ( PNULL == s_select_win_param_ptr)
    {
        s_select_win_param_ptr = (MMIFMM_SELECT_WIN_PARAM_T *)SCI_ALLOCA(sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    }
    //保存参数
    SCI_MEMSET(s_select_win_param_ptr, 0x00, sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    s_select_win_param_ptr->win_id = select_info->win_id;
    s_select_win_param_ptr->max_size = select_info->max_size;
    if(NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
    MMI_WSTRNCPY(s_select_win_param_ptr->ticked_file, MMIFILE_FULL_PATH_MAX_LEN,
                select_info->ticked_file_wstr, select_info->ticked_file_len, select_info->ticked_file_len);
                s_select_win_param_ptr->ticked_file_len = select_info->ticked_file_len;
    }
    else
    {
        SCI_MEMSET(s_select_win_param_ptr->ticked_file, 0, (MMIFILE_FULL_PATH_MAX_LEN*sizeof(wchar)));
        s_select_win_param_ptr->ticked_file_len = 0;
    }

    //计算打开的目录
    if (NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        dev_len = MMIFILE_DEVICE_NAME_MAX_LEN;
        dir_len = MMIFILE_DIR_NAME_MAX_LEN;
        find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
        MMIAPIFMM_SplitFullPath(select_info->ticked_file_wstr, select_info->ticked_file_len,dev, &dev_len, dir, &dir_len, NULL, NULL);
        MMIAPIFMM_CombineFullPath(dev, dev_len, dir, dir_len, NULL, 0, find_path, &find_path_len);
    }
    else 
	{
        device = MMIAPIFMM_GetFirstValidDevice();
        if(MMI_DEVICE_NUM > device)
        {
			find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
			if (!MMIAPIFMM_CombineFullPath(MMIAPIFMM_GetDevicePath(device), MMIAPIFMM_GetDevicePathLen(device),
				MMIMULTIM_DIR_FONT, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_FONT),
				PNULL, 0,
				find_path, &find_path_len))
			{
				if (PNULL != s_select_win_param_ptr)
				{
					SCI_FREE(s_select_win_param_ptr);
					s_select_win_param_ptr = PNULL;
				}
				return FALSE;
			}
        }
        else
        {
			//no valid device
			if (PNULL != s_select_win_param_ptr)
			{
				SCI_FREE(s_select_win_param_ptr);
				s_select_win_param_ptr = PNULL;
			}
			return FALSE;
        }
	}
    //把搜索类型转化成字符串形式
    for (i = 0; i<ARR_SIZE(select_info->select_file);i++) /*lint !e574 !e737*/
    {
        MMIAPIFMM_GetFilerInfo(&filter,select_info->select_file[i]);
    }

    func_type = FUNC_FIND_FILE_AND_ALLFOLER;
    SCI_MEMSET(s_filter.filter_str,0,(sizeof(char))*(FMM_FILTER_STRING_MAXLEN+2));
    filter_len = strlen((char*)filter.filter_str);
    SCI_MEMCPY(s_filter.filter_str, filter.filter_str, (sizeof(uint8)*(filter_len+1)));
    MMIFMM_OpenFileWinByLayerInternal(find_path, find_path_len,
                            &filter, func_type, FALSE,
                            HandleSelectFontWinMsg, NULL, NULL, FALSE, FALSE);

    return TRUE;
}
#endif

/*****************************************************************************/
//  Description : close select music window
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseSelectMusicWin(void)
{
    //SCI_TRACE_LOW:"MMIAPIFMM_CloseSelectMusicWin, exit"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_3448_112_2_18_2_20_12_236,(uint8*)"");
    MMK_CloseWin(MMIFMM_SELECT_MUSIC_WIN_ID);
}

/*****************************************************************************/
//  Description : close select file window
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseSelectFileWin(void)
{
    //SCI_TRACE_LOW:"MMIAPIFMM_CloseSelectFileWin, exit"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_3458_112_2_18_2_20_12_237,(uint8*)"");
    MMK_CloseWin(MMIFMM_OPEN_FILE_BYLAYER_WAIT_WIN_ID);
    if(MMK_IsOpenWin(MMIFMM_OPEN_FILE_WIN_ID))
    {
        MMK_CloseParentWin(MMIFMM_OPEN_FILE_WIN_ID);
    }
    MMK_CloseWin(MMIFMM_SEL_PICTURE_PREVIEW_WIN_ID);
}

/*****************************************************************************/
//  Description : handle select ring list win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL MMIFMM_SELECT_FILE_RESULT_E OpenSelectMovWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                )
{
    MMIFMM_SELECT_FILE_RESULT_E result = MMIFMM_SELECT_FAIL;
#ifdef VIDEO_PLAYER_SUPPORT
    MMIFMM_SELECT_RETURN_T      selected_info = {0};
    VP_SUPPORTED_STATUS_TYPE_E supported_type = VP_SUPPORTED_STATUS_TYPE_MAX;


    //SCI_ASSERT(NULL != file_data_ptr);
#ifdef DRM_SUPPORT
    if (PNULL != s_select_win_param_ptr
        && ((0 != s_select_win_param_ptr->drm_limit.cons_mode_limit)
        || (0 != s_select_win_param_ptr->drm_limit.method_level_limit)))
    {
        //如果用户设置了限制类型，则检查此文件是否支持用户的要求
        if (IsSelDrmConstraintFile(file_data_ptr->filename, 
            file_data_ptr->name_len, s_select_win_param_ptr->drm_limit))
        {
            return MMIFMM_SELECT_DRM_NOCOPYRIGHTS;
        }
    }
#endif
    
    if(PNULL == file_data_ptr)
    {
        //SCI_TRACE_LOW:"OpenSelectMovWinCbk param error:PNULL == file_data_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_3498_112_2_18_2_20_13_238,(uint8*)"");
        return MMIFMM_SELECT_FAIL;
    }
    

    supported_type = MMIAPIVP_GetSupportedStatus(file_data_ptr->filename,file_data_ptr->name_len);
    if (VP_SUPPORTED_BOTH       == supported_type ||
        VP_SUPPORTED_VIDEO_ONLY == supported_type)

    {
        selected_info.file_size = file_data_ptr->size;
        selected_info.file_type = MMIFMM_FILE_TYPE_MOVIE;
        selected_info.movie_type = MMIAPICOM_GetMovieType(file_data_ptr->filename, file_data_ptr->name_len);

        MMIAPICOM_Wstrncpy(selected_info.file_name_ptr, file_data_ptr->filename, file_data_ptr->name_len);
        selected_info.file_name_len = file_data_ptr->name_len;

        selected_info.data_buf_ptr = NULL;
        MMK_SendMsg(s_select_win_param_ptr->win_id, MSG_MULTIM_SELECTED_RETURN, (void*)&selected_info);

        result = MMIFMM_SELECT_SUCCESS;
    }
    else

    {
        //SCI_TRACE_LOW:"OpenSelectMovWinCbk, supported_type = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_3523_112_2_18_2_20_13_239,(uint8*)"d",supported_type);
    }
    
#endif    
    return result;
}

/*****************************************************************************/
//  Description : handle select movie list from mms win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectMovWinMMSCbk(
                                     FILEARRAY_DATA_T    *file_data_ptr
                                     )
{
    BOOLEAN                     result = TRUE;
    MMIFMM_SELECT_RETURN_T      selected_info = {0};

    
    //SCI_ASSERT(NULL != file_data_ptr);
    if(PNULL == file_data_ptr)
    {
        //SCI_TRACE_LOW:"OpenSelectMovWinMMSCbk param error:PNULL == file_data_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_3545_112_2_18_2_20_13_240,(uint8*)"");
        return FALSE;
    }

#ifdef DRM_SUPPORT
    if (PNULL != s_select_win_param_ptr
        && ((0 != s_select_win_param_ptr->drm_limit.cons_mode_limit)
        || (0 != s_select_win_param_ptr->drm_limit.method_level_limit)))
    {
        //如果用户设置了限制类型，则检查此文件是否支持用户的要求
        if (IsSelDrmConstraintFile(file_data_ptr->filename, 
            file_data_ptr->name_len, s_select_win_param_ptr->drm_limit))
        {
            return FALSE;
        }
    }
#endif
        
    selected_info.file_size = file_data_ptr->size;
    selected_info.file_type = MMIFMM_FILE_TYPE_MOVIE;
    selected_info.movie_type = MMIAPICOM_GetMovieType(file_data_ptr->filename, file_data_ptr->name_len);        
    MMIAPICOM_Wstrncpy(selected_info.file_name_ptr, file_data_ptr->filename, file_data_ptr->name_len);
    selected_info.file_name_len = file_data_ptr->name_len;        
    selected_info.data_buf_ptr = NULL;
    MMK_SendMsg(s_select_win_param_ptr->win_id, MSG_MULTIM_SELECTED_RETURN, (void*)&selected_info);
    
    return result;
}

/*****************************************************************************/
//  Description : handle movie select win
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectMovWinMsg(
                                        MMI_WIN_ID_T            win_id,
                                        MMI_MESSAGE_ID_E        msg_id,
                                        DPARAM                  param
                                        )
{
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
#ifdef VIDEO_PLAYER_SUPPORT 
    FILEARRAY_DATA_T    file_data = {0};    
    uint16              dev_name[MMIFILE_DEVICE_NAME_MAX_LEN+1] = {0};
    uint16              dev_len = 0;
    //FILE_DEV_E_T        device = FS_HS;
    MMICOM_VIDEO_TYPE_E       movie_type = MMICOM_VIDEO_TYPE_3GP;          
    //VP_SUPPORTED_STATUS_TYPE_E   supported_type = VP_SUPPORTED_STATUS_TYPE_MAX;


    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        if ( !MMIAPIMMS_IsBlockConflict(BLOCK_MEM_POOL_MP4ENC) )
        {
            MMIAPIFMM_SetOpenFileWinSoftkey(STXT_SELECT, STXT_PREVIEW, STXT_RETURN);
        }
        else
        {
            MMIAPIFMM_SetOpenFileWinSoftkey(STXT_SELECT, TXT_NULL, STXT_RETURN);
        }
        #ifdef MMI_PDA_SUPPORT
        if(!MMIAPIMMS_IsEditMMS())//MMS not allow preview
        {
            MMIFMM_SetIsHavePreviewIcon(TRUE);
        }
        #endif
        result = MMI_RESULT_TRUE;
    break;


    case MSG_APP_OK:
    case MSG_CTL_OK:
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
		{
			break;
		}
        if (FILEARRAY_TYPE_FILE == file_data.type)
        {
            if(0 == file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
            }
            else if(!MMIAPIMMS_IsEditMMS() && 0 != s_select_win_param_ptr->max_size && s_select_win_param_ptr->max_size < file_data.size)
            {                
                MMIPUB_OpenAlertWarningWin(TXT_FILESIZE_TOOBIG);
            }
            else
            {
                if (MMIAPIMMS_IsEditMMS())
                {
                    if(!OpenSelectMovWinMMSCbk(&file_data))
                    {
#ifdef DRM_SUPPORT                        
                		MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
#endif
                    }
                    //MMK_CloseWin(win_id);
                    return result;
                }
                else
                {
                    MMIFMM_SELECT_FILE_RESULT_E sel_result = OpenSelectMovWinCbk(&file_data);
                    if(MMIFMM_SELECT_SUCCESS == sel_result )
                    {
                        //MMK_CloseWin(win_id);
                        return MMI_RESULT_TRUE;
                    }
#ifdef DRM_SUPPORT 
                    else if (MMIFMM_SELECT_DRM_NOCOPYRIGHTS == sel_result)
                    {                        
                        MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
                    }
#endif
                    else
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_COMMON_NO_SUPPORT);
                    }
                }                
            }

            result = MMI_RESULT_TRUE;
        }
    break;

#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        if (PNULL != param)
        {
            uint32 src_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(src_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
            {
                break;
            }
            else
            {
                result = MMI_RESULT_TRUE;
                MMK_SendMsg(win_id, MSG_APP_OK, PNULL);
                break;
            }
        }
        break;
#else
    case MSG_CTL_PENOK:
#endif
    case MSG_APP_WEB:   
    case MSG_CTL_MIDSK:
#ifdef MMI_PDA_SUPPORT      
   case MSG_NOTIFY_LIST_CHECK_CONTENT:
#endif
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
		{
            break;
		}
        if (FILEARRAY_TYPE_FILE == file_data.type)
        {
            if (!MMIAPIMMS_IsBlockConflict(BLOCK_MEM_POOL_MP4ENC))
            {
                if(0 == file_data.size)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
                }
                else
                {

                    movie_type = MMIAPICOM_GetMovieType(file_data.filename, file_data.name_len);
                    MMIAPIFMM_SplitFullPath(file_data.filename, file_data.name_len,
                                          dev_name, &dev_len, NULL, NULL, NULL, NULL);
                   
#ifdef BROWSER_SUPPORT
                    if(MMIAPIBROWSER_IsRunning())
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_BROWSER_VEDIO_NOT_SUPPORT);
                    }
                    else
#endif
                    {
#ifdef DRM_SUPPORT
                        {

                            DRMFILE_PRE_CHECK_STATUS_E pre_check_drmfile_status =
                            MMIAPIDRM_PreCheckFileStatus(file_data.filename, DRM_PERMISSION_PLAY);

                            if (DRMFILE_PRE_CHECK_NORMAL != pre_check_drmfile_status)
                            {
                                if (DRMFILE_PRE_CHECK_NO_RIGHTS == pre_check_drmfile_status)
                                {
                                    MMIAPIFMM_UpdateSelectListIconData(file_data.filename, file_data.name_len);
                                }
                                break;
                            }
                        }
#endif
                        MMIAPIFMM_PreviewVideo(file_data.filename);
                        //MMIAPIVP_PlayVideoFromVideoPlayer(VP_ENTRY_FROM_FILE,file_data.filename,file_data.name_len,PNULL);

                    }
                }
            }
            else
            {
                if(0 == file_data.size)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
                }
                else if(0 != s_select_win_param_ptr->max_size && s_select_win_param_ptr->max_size < file_data.size)
                {
                    if (MMIAPIMMS_IsEditMMS())
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_MMS_MULTIFILE_TOO_BIG);
                        if(MMK_IsOpenWin(MMIFMM_OPEN_FILE_WIN_ID))
                        {
                            MMK_CloseParentWin(MMIFMM_OPEN_FILE_WIN_ID); 
                        }
                    }
                    else
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_FILESIZE_TOOBIG);
                    } 
                }
                else
                {
                    if (MMIAPIMMS_IsEditMMS())
                    {
                        if(!OpenSelectMovWinMMSCbk(&file_data))
                        {
#ifdef DRM_SUPPORT                        
                        	MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
#endif
                        }                    

                        //MMK_CloseWin(win_id);
                        return result;
                    }
                    else
                    {
                        MMIFMM_SELECT_FILE_RESULT_E sel_result = OpenSelectMovWinCbk(&file_data);
                        if(MMIFMM_SELECT_SUCCESS == sel_result)
                        {
                           // MMK_CloseWin(win_id);
                            return MMI_RESULT_TRUE;
                        }
#ifdef DRM_SUPPORT 
                        else if (MMIFMM_SELECT_DRM_NOCOPYRIGHTS == sel_result)
                        {                        
                            MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
                        }
#endif
                        else
                        {
                            MMIPUB_OpenAlertWarningWin(TXT_COMMON_NO_SUPPORT);
                        }
                    }                    
                }
            }

            result = MMI_RESULT_TRUE;
        }
        break;

    case MSG_CLOSE_WINDOW:
        MMIFMM_SetOpenFileWinDisp(WIN_DISP_1LINE);
        if (PNULL != s_select_win_param_ptr)
        {
            SCI_FREE(s_select_win_param_ptr);
            s_select_win_param_ptr = PNULL;
        }
        
        #ifdef MMI_PDA_SUPPORT
        if(!MMIAPIMMS_IsEditMMS())//MMS not allow preview
        {
            MMIFMM_SetIsHavePreviewIcon(FALSE);
        }
        #endif
        result = MMI_RESULT_TRUE;

        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
#endif
    return result;
}
/*****************************************************************************/
//  Description : close select movie window
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseSelectMovieWin(void)
{
    if(MMK_IsOpenWin(MMIFMM_SELECT_MOV_WIN_ID))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_CloseSelectMovieWin, exit"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_3857_112_2_18_2_20_13_241,(uint8*)"");
#ifdef VIDEO_PLAYER_SUPPORT
        MMIAPIVP_ExitVideoPlayer();
#endif
        MMK_CloseWin(MMIFMM_SELECT_MOV_OPT_WIN_ID);
        MMK_CloseWin(MMIFMM_SELECT_MOV_WIN_ID);
    }
}

/*****************************************************************************/
//  Description : select a pic from fmm
//  Global resource dependence : 
//  Author: robert.lu
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectPictureWin(MMIFMM_SELECT_INFO_T *select_info)
{
    uint16  dev[MMIFILE_DEVICE_NAME_MAX_LEN+1] = {0};
    uint16  dev_len = 0;
    uint16  dir[MMIFILE_DIR_NAME_MAX_LEN+1] = {0};
    uint16  dir_len = 0;
    uint16  find_path[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16 find_path_len = 0;
    uint16 filter_len = 0;
    MMIFMM_FILTER_T         filter = {0};
    MMIFMM_FUNC_TYPE_E      func_type = 0;/*lint !e64*/
	MMIFILE_DEVICE_E        device = MMI_DEVICE_SDCARD;
    int32   i = 0;

    if ( PNULL == select_info)
    {
        return FALSE;
    }
    MMIAPIFMM_CloseSelectFileWin();
    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    else if (MMIAPIENG_GetIQModeStatus())
    {
        MMIPUB_OpenAlertWarningWin(TXT_IQ_DATA_PROCESSING);
        return FALSE;
    }

    if (!MMIAPIUDISK_IsDeviceReady())
    {
        MMIPUB_OpenAlertWarningWin(TXT_NO_SD_CARD_ALERT);
        return FALSE;
    }

    if (PNULL == s_select_win_param_ptr)
    {
        s_select_win_param_ptr = (MMIFMM_SELECT_WIN_PARAM_T *)SCI_ALLOCA(sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    }
    //保存参数
    SCI_MEMSET(s_select_win_param_ptr, 0x00, sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    s_select_win_param_ptr->win_id = select_info->win_id;
    s_select_win_param_ptr->max_size = select_info->max_size;
    if(NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        MMI_WSTRNCPY(s_select_win_param_ptr->ticked_file,MMIFILE_FULL_PATH_MAX_LEN,
            select_info->ticked_file_wstr, select_info->ticked_file_len, select_info->ticked_file_len);
        s_select_win_param_ptr->ticked_file_len = select_info->ticked_file_len;
    }
    else
    {
        SCI_MEMSET(s_select_win_param_ptr->ticked_file, 0, (MMIFILE_FULL_PATH_MAX_LEN*sizeof(wchar)));
        s_select_win_param_ptr->ticked_file_len = 0;
    }

    //计算打开的目录
    if (NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        dev_len = MMIFILE_DEVICE_NAME_MAX_LEN;
        dir_len = MMIFILE_DIR_NAME_MAX_LEN;
        find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
        MMIAPIFMM_SplitFullPath(select_info->ticked_file_wstr, select_info->ticked_file_len,dev, &dev_len, dir, &dir_len, NULL, NULL);
        MMIAPIFMM_CombineFullPath(dev, dev_len, dir, dir_len, NULL, 0, find_path, &find_path_len);
    }
    else 
	{
        device = MMIAPIFMM_GetFirstValidDevice();
        if(MMI_DEVICE_NUM > device)
        {
			find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
			if (!MMIAPIFMM_CombineFullPath(MMIAPIFMM_GetDevicePath(device), MMIAPIFMM_GetDevicePathLen(device),
				MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE),
				PNULL, 0,
				find_path, &find_path_len))
			{
				if (PNULL != s_select_win_param_ptr)
				{
					SCI_FREE(s_select_win_param_ptr);
					s_select_win_param_ptr = PNULL;
				}
				return FALSE;
			}
        }
        else
        {
			//no valid device
			if (PNULL != s_select_win_param_ptr)
			{
				SCI_FREE(s_select_win_param_ptr);
				s_select_win_param_ptr = PNULL;
			}
			return FALSE;
        }
	}
   
    //把搜索类型转化成字符串形式
    for (i = 0; i<ARR_SIZE(select_info->select_file);i++)/*lint !e574 !e737*/
    {
        MMIAPIFMM_GetFilerInfo(&filter,select_info->select_file[i]);
    }

    func_type = FUNC_FIND_FILE_AND_ALLFOLER;  
    MMIFMM_SetOpenFileWinDisp(WIN_DISP_2LINE);  
    SCI_MEMSET(s_filter.filter_str,0,(sizeof(char))*(FMM_FILTER_STRING_MAXLEN+2));
    filter_len = strlen((char*)filter.filter_str);
    SCI_MEMCPY(s_filter.filter_str, filter.filter_str, (sizeof(uint8)*(filter_len+1)));
    MMIFMM_OpenFileWinByLayerInternal(find_path, find_path_len,
      	 &filter, func_type, FALSE,    	HandleSelectPicWinMsg, NULL, NULL, FALSE, FALSE);
    
    return TRUE;
}

/*****************************************************************************/
//  Description : select a pic from fmm
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectAllFileWin(MMIFMM_SELECT_INFO_T *select_info)
{
    uint16  dev[MMIFILE_DEVICE_NAME_MAX_LEN+1] = {0};
    uint16  dev_len = 0;
    uint16  dir[MMIFILE_DIR_NAME_MAX_LEN+1] = {0};
    uint16  dir_len = 0;
    uint16  find_path[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16 find_path_len = 0;
    uint16  filter_len = 0;
    MMIFMM_FILTER_T         filter = {0};
    MMIFMM_FUNC_TYPE_E      func_type = 0;/*lint !e64*/
	MMIFILE_DEVICE_E      device = MMI_DEVICE_SDCARD;
    int32   i = 0;

    if ( PNULL == select_info)
    {
        return FALSE;
    }
    MMIAPIFMM_CloseSelectFileWin();
    if (MMIAPIUDISK_UdiskIsRun()) //U盘使用中
    {
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
        return FALSE;
    }
    else if (MMIAPIENG_GetIQModeStatus())
    {
        MMIPUB_OpenAlertWarningWin(TXT_IQ_DATA_PROCESSING);
        return FALSE;
    }

    if (!MMIAPIUDISK_IsDeviceReady())
    {
        MMIPUB_OpenAlertWarningWin(TXT_NO_SD_CARD_ALERT);
        return FALSE;
    }

    if (PNULL == s_select_win_param_ptr)
    {
        s_select_win_param_ptr = (MMIFMM_SELECT_WIN_PARAM_T *)SCI_ALLOCA(sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    }
    //保存参数
    SCI_MEMSET(s_select_win_param_ptr, 0x00, sizeof(MMIFMM_SELECT_WIN_PARAM_T));
    s_select_win_param_ptr->win_id = select_info->win_id;
    s_select_win_param_ptr->max_size = select_info->max_size;
    if(NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        MMI_WSTRNCPY(s_select_win_param_ptr->ticked_file,MMIFILE_FULL_PATH_MAX_LEN,
            select_info->ticked_file_wstr, select_info->ticked_file_len, select_info->ticked_file_len);
        s_select_win_param_ptr->ticked_file_len = select_info->ticked_file_len;
    }
    else
    {
        SCI_MEMSET(s_select_win_param_ptr->ticked_file, 0, (MMIFILE_FULL_PATH_MAX_LEN*sizeof(wchar)));
        s_select_win_param_ptr->ticked_file_len = 0;
    }

    //计算打开的目录
    if (NULL != select_info->ticked_file_wstr && 0 < select_info->ticked_file_len)
    {
        dev_len = MMIFILE_DEVICE_NAME_MAX_LEN;
        dir_len = MMIFILE_DIR_NAME_MAX_LEN;
        find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
        MMIAPIFMM_SplitFullPath(select_info->ticked_file_wstr, select_info->ticked_file_len,dev, &dev_len, dir, &dir_len, NULL, NULL);
        MMIAPIFMM_CombineFullPath(dev, dev_len, dir, dir_len, NULL, 0, find_path, &find_path_len);
    }
    else 
	{

        device = MMIAPIFMM_GetFirstValidDevice();
        if(MMI_DEVICE_NUM > device)
        {
			find_path_len = MMIFILE_FULL_PATH_MAX_LEN;
			if (!MMIAPIFMM_CombineFullPath(MMIAPIFMM_GetDevicePath(device), MMIAPIFMM_GetDevicePathLen(device),
				MMIMULTIM_OTHER_DEFAULT_DIR, MMIAPICOM_Wstrlen(MMIMULTIM_OTHER_DEFAULT_DIR),
				PNULL, 0,
				find_path, &find_path_len))
			{
				if (PNULL != s_select_win_param_ptr)
				{
					SCI_FREE(s_select_win_param_ptr);
					s_select_win_param_ptr = PNULL;
				}
				return FALSE;
			}
        }
        else
        {
			//no valid device
			if (PNULL != s_select_win_param_ptr)
			{
				SCI_FREE(s_select_win_param_ptr);
				s_select_win_param_ptr = PNULL;
			}
			return FALSE;
        }
	}
    //把搜索类型转化成字符串形式
    for (i = 0; i<ARR_SIZE(select_info->select_file);i++)/*lint !e574 !e737*/
    {
        MMIAPIFMM_GetFilerInfo(&filter,select_info->select_file[i]);
    }

    func_type = FUNC_FIND_FILE_AND_ALLFOLER;
    SCI_MEMSET(s_filter.filter_str,0,(sizeof(char))*(FMM_FILTER_STRING_MAXLEN+2));
    filter_len = strlen((char*)filter.filter_str);
    SCI_MEMCPY(s_filter.filter_str, filter.filter_str, (sizeof(uint8)*(filter_len+1)));
    MMIFMM_OpenFileWinByLayerInternal(find_path, find_path_len,
       &filter, func_type, FALSE,       HandleSelectAllFileWinMsg, NULL, NULL, FALSE, FALSE);
    
    return TRUE;
}

/*****************************************************************************/
//  Description : handle select ring list win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectAllFileWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                )
{
    BOOLEAN                         result = FALSE;
    MMIFMM_SELECT_RETURN_T          selected_info = {0};

    //SCI_ASSERT(NULL != file_data_ptr);
#ifdef DRM_SUPPORT
    if (PNULL != s_select_win_param_ptr
        && ((0 != s_select_win_param_ptr->drm_limit.cons_mode_limit)
        || (0 != s_select_win_param_ptr->drm_limit.method_level_limit)))
    {
        //如果用户设置了限制类型，则检查此文件是否支持用户的要求
        if (IsSelDrmConstraintFile(file_data_ptr->filename, 
            file_data_ptr->name_len, s_select_win_param_ptr->drm_limit))
        {
            return FALSE;
        }
    }
#endif
    if(PNULL == file_data_ptr)
    {
        //SCI_TRACE_LOW:"OpenSelectAllFileWinCbk param error:PNULL == file_data_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4131_112_2_18_2_20_14_242,(uint8*)"");
        return TRUE;
    }
    selected_info.file_size = file_data_ptr->size;
    selected_info.file_type = MMIAPIFMM_GetFileTypeByFileName(file_data_ptr->filename, file_data_ptr->name_len);
    //selected_info.pic_type = MMIAPICOM_GetImgaeType(file_data_ptr->filename, file_data_ptr->name_len);

    MMIAPICOM_Wstrncpy(selected_info.file_name_ptr, file_data_ptr->filename, file_data_ptr->name_len);
    selected_info.file_name_len = file_data_ptr->name_len;

    selected_info.data_buf_ptr = NULL;
    MMK_SendMsg(s_select_win_param_ptr->win_id, MSG_MULTIM_SELECTED_RETURN, (void*)&selected_info);

    result = TRUE;
    return result;
}

/*****************************************************************************/
//  Description : handle select ring list win call back
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN OpenSelectPicWinCbk(
                                FILEARRAY_DATA_T    *file_data_ptr
                                )
{
    BOOLEAN                         result = FALSE;
    MMIFMM_SELECT_RETURN_T          selected_info = {0};

    //SCI_ASSERT(NULL != file_data_ptr);
    if(PNULL == file_data_ptr)
    {
        //SCI_TRACE_LOW:"OpenSelectPicWinCbk param error:PNULL == file_data_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4162_112_2_18_2_20_14_243,(uint8*)"");
        return FALSE;
    }

#ifdef DRM_SUPPORT
    if (PNULL != s_select_win_param_ptr
        && ((0 != s_select_win_param_ptr->drm_limit.cons_mode_limit)
        || (0 != s_select_win_param_ptr->drm_limit.method_level_limit)))
    {
        //如果用户设置了限制类型，则检查此文件是否支持用户的要求
        if (IsSelDrmConstraintFile(file_data_ptr->filename, 
            file_data_ptr->name_len, s_select_win_param_ptr->drm_limit))
        {
            return FALSE;
        }
    }
#endif

    selected_info.file_size = file_data_ptr->size;
    selected_info.file_type = MMIFMM_FILE_TYPE_PICTURE;
    selected_info.pic_type = MMIAPICOM_GetImgaeType(file_data_ptr->filename, file_data_ptr->name_len);

    MMIAPICOM_Wstrncpy(selected_info.file_name_ptr, file_data_ptr->filename, file_data_ptr->name_len);
    selected_info.file_name_len = file_data_ptr->name_len;

    selected_info.data_buf_ptr = NULL;
    if(MMIAPISET_IsWallpaperCall())
    {
        MMIAPIFMM_PreviewPicture(file_data_ptr->filename);
    }
    else
    {
        MMK_SendMsg(s_select_win_param_ptr->win_id, MSG_MULTIM_SELECTED_RETURN, (void*)&selected_info);
        if(MMK_IsOpenWin(MMIFMM_OPEN_FILE_WIN_ID))
        {
            MMK_CloseParentWin(MMIFMM_OPEN_FILE_WIN_ID); 
        }
    }
    result = TRUE;
    return result;
}

/*****************************************************************************/
//  Description : handle get icon from icon list preview win msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectPicWinMsg(
                                       MMI_WIN_ID_T        win_id,
                                       MMI_MESSAGE_ID_E    msg_id,
                                       DPARAM              param
                                       )
{
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    FILEARRAY_DATA_T    file_data = {0};

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifndef MMI_PDA_SUPPORT
		MMIAPIFMM_SetOpenFileWinSoftkey(STXT_SELECT, STXT_PREVIEW, STXT_RETURN);
#endif
#ifdef MMI_PDA_SUPPORT
        if(!MMIAPISET_IsWallpaperCall())
        {
            MMIFMM_SetIsHavePreviewIcon(TRUE);
        }
        result = MMI_RESULT_TRUE;
#endif
        break;
		
    case MSG_APP_WEB:   
    case MSG_CTL_MIDSK:
#ifdef MMI_PDA_SUPPORT      
   case MSG_NOTIFY_LIST_CHECK_CONTENT:
#endif
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
		{
			break;
		}
		if (FILEARRAY_TYPE_FILE == file_data.type)
		{
            result = MMI_RESULT_TRUE;
#ifdef DRM_SUPPORT
            {
                DRMFILE_PRE_CHECK_STATUS_E pre_check_drmfile_status =
                MMIAPIDRM_PreCheckFileStatus(file_data.filename, DRM_PERMISSION_DISPLAY);

                if (DRMFILE_PRE_CHECK_NORMAL != pre_check_drmfile_status)
                {
                    if (DRMFILE_PRE_CHECK_NO_RIGHTS == pre_check_drmfile_status)
                    {
                        MMIAPIFMM_UpdateSelectListIconData(file_data.filename, file_data.name_len);
                    }
                    break;
                }
            }
#endif
            //OpenSelectPicWinCbk(&file_data);
            MMIAPIFMM_PreviewPicture(file_data.filename);
        }
        result = MMI_RESULT_TRUE;
        break;
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        if (PNULL != param)
        {
            uint32 src_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(src_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
            {
                break;
            }
            else
            {
                result = MMI_RESULT_TRUE;
                MMK_SendMsg(win_id, MSG_APP_OK, PNULL);
                break;
            }
        }
        break;
#else
    case MSG_CTL_PENOK:
#endif
    case MSG_APP_OK:    
    case MSG_CTL_OK:
    //case MSG_APP_WEB:   
    //case MSG_CTL_MIDSK:
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
		{
			break;
		}
        if (FILEARRAY_TYPE_FILE == file_data.type)
        {
            if(0 == file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
            }
            else if(!MMIAPIMMS_IsEditMMS() && 0 != s_select_win_param_ptr->max_size && s_select_win_param_ptr->max_size < file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_FILESIZE_TOOBIG);            
            }
            else
            {
                if (!OpenSelectPicWinCbk(&file_data))
                {
#ifdef DRM_SUPPORT                	
    				MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
#endif    				
                    return result;    
                }
                //MMK_CloseWin(win_id);
                return result;
            }

            result = MMI_RESULT_TRUE;
        }
        break;

    case MSG_CLOSE_WINDOW:
        MMIFMM_SetOpenFileWinDisp(WIN_DISP_1LINE);
        if (PNULL != s_select_win_param_ptr)
        {
           SCI_FREE(s_select_win_param_ptr);
           s_select_win_param_ptr = PNULL;
        }
#ifdef  MMI_PDA_SUPPORT
        if(!MMIAPISET_IsWallpaperCall())
        {
            MMIFMM_SetIsHavePreviewIcon(FALSE);
        }
#endif
        result = MMI_RESULT_TRUE;
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}

/*****************************************************************************/
//  Description : handle get icon from icon list preview win msg
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSelectAllFileWinMsg(
                                       MMI_WIN_ID_T        win_id,
                                       MMI_MESSAGE_ID_E    msg_id,
                                       DPARAM              param
                                       )
{
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    FILEARRAY_DATA_T    file_data = {0};

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMIAPIFMM_SetOpenFileWinSoftkey(STXT_SELECT, TXT_NULL, STXT_RETURN);
        break;
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_PENOK:
        if (PNULL != param)
        {
            uint32 src_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(src_id == MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_MIDDLE_CTRL_ID)
            {
                break;
            }
            else if(src_id == MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID)
            {
                break;
            }
            else
            {
                result = MMI_RESULT_TRUE;
                MMK_SendMsg(win_id, MSG_APP_OK, PNULL);
                break;
            }
        }
        break;
#else
    case MSG_CTL_PENOK:
#endif
    case MSG_APP_OK:    
    case MSG_CTL_OK:
    case MSG_APP_WEB:   
    case MSG_CTL_MIDSK:		
    //case MSG_APP_WEB:   
    //case MSG_CTL_MIDSK:
        if(!MMIAPIFMM_GetOpenFileWinCurFile(&file_data))
        {
            break;
        }
        if (FILEARRAY_TYPE_FILE == file_data.type)
        {
            if(0 == file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
            }
            else if(!MMIAPIMMS_IsEditMMS() && 0 != s_select_win_param_ptr->max_size && s_select_win_param_ptr->max_size < file_data.size)
            {
                MMIPUB_OpenAlertWarningWin(TXT_FILESIZE_TOOBIG);            
            }
            else
            {
                if (!OpenSelectAllFileWinCbk(&file_data))
                {
#ifdef DRM_SUPPORT                	
    				MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_PROTECTION_NOT_OPERATE);
#endif    				
                    return result;
                }                
                //MMK_CloseWin(win_id);
                return result;
            }

            result = MMI_RESULT_TRUE;
        }
        break;

    case MSG_APP_CAMERA:
        //MMK_CloseWin(win_id);
        result = MMI_RESULT_TRUE;
        break;

    case MSG_CLOSE_WINDOW:
        if (PNULL != s_select_win_param_ptr)
        {
           SCI_FREE(s_select_win_param_ptr);
           s_select_win_param_ptr = PNULL;
        }
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return result;
}


/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
LOCAL void InitMultimFolder(
                            const wchar   *device_ptr,
                            uint16  device_len,
                            const wchar   *dir_ptr,
                            uint16  dir_len
                            )
{
    uint16  full_path_name[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16  full_path_len = MMIFILE_FULL_PATH_MAX_LEN+2;
    //SCI_ASSERT(NULL != device_ptr);
    //SCI_ASSERT(0 < device_len);
    //SCI_ASSERT(NULL != dir_ptr);
    //SCI_ASSERT(0 < dir_len);
    if((PNULL == device_ptr) || (0 == device_len) || (PNULL == dir_ptr) || (0 == dir_len))
    {
        //SCI_TRACE_LOW:"InitMultimFolder param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4471_112_2_18_2_20_14_244,(uint8*)"");
        return;
    }

    if (MMIAPIFMM_CombineFullPath( device_ptr, device_len,
                            dir_ptr, dir_len, 
                            NULL, 0, 
                            full_path_name, &full_path_len))
    {
        if(!MMIAPIFMM_IsFolderExist(full_path_name, full_path_len))
        {
            MMIAPIFMM_CreateDir(full_path_name, full_path_len);
        }
    }
}

/*****************************************************************************/
//  Description : init system folder
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitSystemFolder(
                                     const wchar   *device_ptr,
                                     uint16  device_len,
                                     const wchar   *dir_ptr,
                                     uint16  dir_len
                                     )
{
    uint16  full_path_name[MMIFILE_FULL_PATH_MAX_LEN+2] = {0};
    uint16  full_path_len = MMIFILE_FULL_PATH_MAX_LEN+2;

    if(PNULL == device_ptr || 0 == device_len || PNULL == dir_ptr || 0 == dir_len)
    {
        return;
    }

    if (MMIAPIFMM_CombineFullPath( device_ptr, device_len,
                            dir_ptr, dir_len, 
                            NULL, 0, 
                            full_path_name, &full_path_len))
    {
        MMIAPIFMM_CreateDir(full_path_name, full_path_len);
        MMIAPIFMM_SetAttr(full_path_name, full_path_len, TRUE, TRUE, TRUE, FALSE);
    }

}

/*****************************************************************************/
//  Description : initialize Multim
//  Global resource dependence : none
//  Author: Liqing.Peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitFolders(void)
{
    MMIFILE_DEVICE_E fmm_dev = MMI_DEVICE_UDISK;
    
    //create default dir
    // D:/Photos, D:/Audio, D:/Video
    do
    {
        if(MMIAPIFMM_GetDeviceTypeStatus(fmm_dev))
        {
            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_PICTURE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_PICTURE));

            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_MUSIC, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MUSIC));

            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_MOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE));

#ifdef MMI_KING_MOVIE_SUPPORT
            //u盘下不创建kingmovie文件夹
            if(fmm_dev != MMI_DEVICE_UDISK)
            {
                InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_KINGMOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_KINGMOVIE));
            }    
#endif
            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_EBOOK, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_EBOOK));
#if defined MMI_VCARD_SUPPORT
            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_VCARD, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_VCARD));
#endif
#ifdef MMI_VCALENDAR_SUPPORT
	        InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_VCALENDAR, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_VCALENDAR));
#endif
            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_OTHER_DEFAULT_DIR, MMIAPICOM_Wstrlen(MMIMULTIM_OTHER_DEFAULT_DIR));

            MMIAPIFMM_InitSystemFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_SYSTEM, MMIMULTIM_DIR_SYSTEM_LEN);
            InitMultimFolder(MMIAPIFMM_GetDevicePath(fmm_dev), MMIAPIFMM_GetDevicePathLen(fmm_dev),
                            MMIMULTIM_DIR_ALARM, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_ALARM));
        }
    }while(++fmm_dev < MMI_DEVICE_NUM);

    MMIAPIFMM_InitWallPaper();

#ifdef DRM_SUPPORT
        MMIDRM_CreateDrmRoDir();
#endif
}

/*****************************************************************************/
//  Description : 设置图片为墙纸,处理请稍候消息
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_HandleWallpaperWaitMsg(void)
{
    MMISET_WALLPAPER_SET_T  wallpaper_info = {0};

    wallpaper_info.wallpaper_id   = MMISET_WALLPAPER_DEFAULT_ID;
    wallpaper_info.wallpaper_type = MMISET_WALLPAPER_DOWNLOAD;

    MMIAPISET_SetWallpaperSetInfo(wallpaper_info);
}

/*****************************************************************************/
//  Description : set wallpaper,close waiting window
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseWaitSetWallpaper(
                                               GUIANIM_RESULT_E     result
                                               )
{
    LOCAL GUIANIM_RESULT_E  s_wallpaper_result = GUIANIM_RESULT_SUCC;
    MMI_WIN_ID_T            win_id = MMIMULTIM_SET_WALLPAPER_WAIT_WIN_ID;

    if (MMK_IsOpenWin(win_id))
    {
        //prompt
        if (GUIANIM_RESULT_SUCC == result)
        {
            if (s_is_reset_pre_wallpaper)
            {
                switch (s_wallpaper_result)
                {
                case GUIANIM_RESULT_BIG:
                    //prompt too big
                    MMIPUB_OpenAlertWarningWin(TXT_MULTIM_PICTOO_LARGE);
                    break;

                case GUIANIM_RESULT_BIG_RESOLUTION:
                    //prompt fail
                    MMIPUB_OpenAlertWarningWin(TXT_RESOLUTION_TOO_BIG);
                    break;

                default:
                    //prompt fail
                    MMIPUB_OpenAlertFailWin(TXT_ERROR);
                    break;
                }

                MMIAPISET_SetWallPaperName();//如果设置失败，则需要重新设置一下label的内容
            }
            else
            {
                //prompt success
                /* Modify by michael on 2/23/2012 for  NEWMS00171609:删除"成功"提示框 */
                //MMIPUB_OpenAlertSuccessWin(TXT_SUCCESS);
                /************************Modify over*************************/
            }
            MMK_CloseWin(win_id);
        }
        else
        {
            if (s_is_reset_pre_wallpaper)
            {
                //错误墙纸反复恢复,会导致死循环.如果固定墙纸显示错误,或者设置不支持的图片为墙纸过程中断电
                //SCI_TRACE_LOW:"MMIAPIFMM_CloseWaitSetWallpaper: reset previous wallpaper error!"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4641_112_2_18_2_20_15_245,(uint8*)"");
                //prompt fail
                MMIPUB_OpenAlertFailWin(TXT_ERROR);

                MMK_CloseWin(win_id);
            }
            else
            {
                s_wallpaper_result = result;
                #ifdef LIVE_WALLPAPER_FRAMEWORK_SUPPORT
                if(MMISET_WALLPAPER_LIVE == s_pre_wallpaper.wallpaper_type)
                {

                }
                else
                #endif
                {
                    
                    //恢复之前的墙纸
                    switch (s_pre_wallpaper.wallpaper_type)
                    {
                    case MMISET_WALLPAPER_FIXED_PIC:
                        s_is_reset_pre_wallpaper = TRUE;
                        MMIAPISET_SetWallpaperSetInfo(s_pre_wallpaper);
                        break;

                    case MMISET_WALLPAPER_FIXED_ANIM:
                        break;

                    case MMISET_WALLPAPER_ALBUM:
                    case MMISET_WALLPAPER_DOWNLOAD:
                        MMIAPIFMM_SetPictureToWallpaper(TRUE, s_pre_wallpaper_info.full_path_name,s_pre_wallpaper_info.full_path_len);
                        break;
                    default:
                        //SCI_PASSERT(FALSE,("MMIAPIFMM_CloseWaitSetWallpaper:wallpaper_type %d is error!",s_pre_wallpaper.wallpaper_type));
                        //SCI_TRACE_LOW:"MMIAPIFMM_CloseWaitSetWallpaper:wallpaper_type %d is error!"
                        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4676_112_2_18_2_20_15_246,(uint8*)"d",s_pre_wallpaper.wallpaper_type);
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if ((GUIANIM_RESULT_SUCC != result) && 
            (!s_is_reset_pre_wallpaper))
        {
            //恢复之前的墙纸
            #ifdef LIVE_WALLPAPER_FRAMEWORK_SUPPORT
            if(MMISET_WALLPAPER_LIVE == s_pre_wallpaper.wallpaper_type)
            {

            }
            else
            #endif
            {
                switch (s_pre_wallpaper.wallpaper_type)
                {
                case MMISET_WALLPAPER_FIXED_PIC:
                    s_is_reset_pre_wallpaper = TRUE;
                    MMIAPISET_SetWallpaperSetInfo(s_pre_wallpaper);
                    break;

                case MMISET_WALLPAPER_FIXED_ANIM:
                    break;

                case MMISET_WALLPAPER_ALBUM:
                case MMISET_WALLPAPER_DOWNLOAD:
                    MMIAPIFMM_SetPictureToWallpaper(TRUE, s_pre_wallpaper_info.full_path_name,s_pre_wallpaper_info.full_path_len);
                    MMK_CloseWin(win_id);
                    break;
                
                default:
                    //SCI_PASSERT(FALSE,("MMIAPIFMM_CloseWaitSetWallpaper:wallpaper_type %d is error!",s_pre_wallpaper.wallpaper_type));
                    //SCI_TRACE_LOW:"MMIAPIFMM_CloseWaitSetWallpaper:wallpaper_type %d is error!"
                    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4715_112_2_18_2_20_15_247,(uint8*)"d",s_pre_wallpaper.wallpaper_type);
                    break;
                }
            }
        }
    }
}

/*****************************************************************************/
//  Description : initialize NV about multim wallpaper
//  Global resource dependence : none
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitWallPaper(void)
{
    MN_RETURN_RESULT_E      return_value = MN_RETURN_FAILURE;
    MMISET_WALLPAPER_SET_T  wallpaper_info = {0};
    

    MMINV_READ(MMINV_MULTIM_AS_WALLPAPER_INFO,&s_file_as_wallpaper_info,return_value);
    if (MN_RETURN_SUCCESS != return_value)
    {
        SCI_MEMSET(&s_file_as_wallpaper_info,0,sizeof(MMIMULTIM_AS_WALLPAPER_INFO));
        MMINV_WRITE(MMINV_MULTIM_AS_WALLPAPER_INFO,&s_file_as_wallpaper_info);
    }

    SCI_MEMSET(&wallpaper_info,0,sizeof(MMISET_WALLPAPER_SET_T));
    MMIAPISET_GetWallpaperSetInfo(&wallpaper_info);
    switch (wallpaper_info.wallpaper_type)
    {
    case MMISET_WALLPAPER_FIXED_PIC:
        MMIAPIFMM_ResetWallpaperSetting();
        break;

    case MMISET_WALLPAPER_DOWNLOAD:
        break;

    default:
        break;
    }
}

/*****************************************************************************/
//  Description : change wallpaper file name
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_ChangeWallPaperFileName(
                                      const wchar *full_path_ptr,
                                      uint16    full_path_len
                                      )
{
    SFS_DATE_T			wallpaper_modify_date = {0};
	SFS_TIME_T			wallpaper_modify_time = {0};
    uint32  file_size = 0;
    //SCI_ASSERT(NULL != full_path_ptr);
    //SCI_ASSERT(0 < full_path_len);
    if((PNULL == full_path_ptr) || (0 == full_path_len))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_ChangeWallPaperFileName param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4772_112_2_18_2_20_15_248,(uint8*)"");
        return;
    }

    s_file_as_wallpaper_info.full_path_len = full_path_len;
    SCI_MEMSET(s_file_as_wallpaper_info.full_path_name, 0, sizeof(s_file_as_wallpaper_info.full_path_name)); 
    MMI_WSTRNCPY(s_file_as_wallpaper_info.full_path_name, MMIFILE_FULL_PATH_MAX_LEN,
                full_path_ptr, full_path_len, full_path_len);
    MMIAPIFMM_GetFileInfo(s_file_as_wallpaper_info.full_path_name, s_file_as_wallpaper_info.full_path_len, &file_size, &wallpaper_modify_date, &wallpaper_modify_time);
    s_file_as_wallpaper_info.wallpaper_modify_date.mday = wallpaper_modify_date.mday;
    s_file_as_wallpaper_info.wallpaper_modify_date.mon = wallpaper_modify_date.mon;
    s_file_as_wallpaper_info.wallpaper_modify_date.year = wallpaper_modify_date.year;
    s_file_as_wallpaper_info.wallpaper_modify_time.hour = wallpaper_modify_time.hour;
    s_file_as_wallpaper_info.wallpaper_modify_time.min = wallpaper_modify_time.min;
    s_file_as_wallpaper_info.wallpaper_modify_time.sec = wallpaper_modify_time.sec;

    MMINV_WRITE(MMINV_MULTIM_AS_WALLPAPER_INFO,&s_file_as_wallpaper_info);
}

/*****************************************************************************/
//  Description : write wallpaper file info NV
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_WriteWallPaperNV(
                                       MMIMULTIM_AS_WALLPAPER_INFO  *wallpaper_file_ptr
                                       )
{
    MMINV_WRITE(MMINV_MULTIM_AS_WALLPAPER_INFO,wallpaper_file_ptr);
}

/*****************************************************************************/
//  Description : is file wallpaper file
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsWallPaperFile(
                                  const wchar *full_path_ptr,
                                  uint16    full_path_len
                                  )
{
    //SCI_ASSERT(NULL != full_path_ptr);
    //SCI_ASSERT(0 < full_path_len);
    if((PNULL == full_path_ptr) || (0 == full_path_len))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_IsWallPaperFile param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4815_112_2_18_2_20_15_249,(uint8*)"");
        return FALSE;
    }

    if(0 == MMIAPICOM_CompareTwoWstr( full_path_ptr, full_path_len,
                                       s_file_as_wallpaper_info.full_path_name, 
                                       s_file_as_wallpaper_info.full_path_len))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
//  Description : 设置图片为墙纸
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SetPictureToWallpaper(
                                           BOOLEAN      is_pre_wallpaper,   //是否是恢复前一张墙纸
                                           const wchar  *full_path_ptr,
                                           uint16       full_path_len
                                           )
{
    BOOLEAN     result = FALSE;
    SFS_DATE_T			wallpaper_modify_date = {0};
	SFS_TIME_T			wallpaper_modify_time = {0};
    uint32  file_size = 0;
    //SCI_ASSERT(NULL != full_path_ptr);
    //SCI_ASSERT(0 < full_path_len);
    if((PNULL == full_path_ptr) || (0 == full_path_len))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_SetPictureToWallpaper param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4849_112_2_18_2_20_15_250,(uint8*)"");
        return FALSE;
    }

    s_is_reset_pre_wallpaper = is_pre_wallpaper;
    if (!is_pre_wallpaper)
    {
        //save previous wallpaper file name
        SCI_MEMSET(&s_pre_wallpaper_info,0,sizeof(MMIMULTIM_AS_WALLPAPER_INFO));
        MMI_MEMCPY(&s_pre_wallpaper_info,sizeof(MMIMULTIM_AS_WALLPAPER_INFO),
            &s_file_as_wallpaper_info,sizeof(MMIMULTIM_AS_WALLPAPER_INFO),
            sizeof(MMIMULTIM_AS_WALLPAPER_INFO));
    }

    //set previous wallpaper info
    MMIAPISET_GetWallpaperSetInfo(&s_pre_wallpaper);

    //set global variable
    SCI_MEMSET(s_file_as_wallpaper_info.full_path_name, 0, sizeof(s_file_as_wallpaper_info.full_path_name));
    s_file_as_wallpaper_info.full_path_len = full_path_len;
    MMI_WSTRNCPY(s_file_as_wallpaper_info.full_path_name,  MMIFILE_FULL_PATH_MAX_LEN,
               full_path_ptr, full_path_len, full_path_len);
    if (MMIAPIFMM_GetFileInfo(s_file_as_wallpaper_info.full_path_name,
            s_file_as_wallpaper_info.full_path_len, 
            &file_size, 
            &wallpaper_modify_date, 
            &wallpaper_modify_time))
    {
        s_file_as_wallpaper_info.wallpaper_modify_date.mday = wallpaper_modify_date.mday;
        s_file_as_wallpaper_info.wallpaper_modify_date.mon = wallpaper_modify_date.mon;
        s_file_as_wallpaper_info.wallpaper_modify_date.year = wallpaper_modify_date.year;
        s_file_as_wallpaper_info.wallpaper_modify_time.hour = wallpaper_modify_time.hour;
        s_file_as_wallpaper_info.wallpaper_modify_time.min = wallpaper_modify_time.min;
        s_file_as_wallpaper_info.wallpaper_modify_time.sec = wallpaper_modify_time.sec;
        MMINV_WRITE(MMINV_MULTIM_AS_WALLPAPER_INFO,&s_file_as_wallpaper_info);

        //prompt waiting
        MMIAPIFMM_OpenWaitSetWallpaper();

        MMK_PostMsg(VIRTUAL_WIN_ID,MSG_SET_WALLPAPER_WAIT,PNULL,0);

        result = TRUE;
    }
    else
    {
        if (!is_pre_wallpaper)
        {
            //save previous wallpaper file name
            SCI_MEMSET(&s_file_as_wallpaper_info,0,sizeof(MMIMULTIM_AS_WALLPAPER_INFO));
            MMI_MEMCPY(&s_file_as_wallpaper_info,sizeof(MMIMULTIM_AS_WALLPAPER_INFO),
                &s_pre_wallpaper_info,sizeof(MMIMULTIM_AS_WALLPAPER_INFO),
                sizeof(MMIMULTIM_AS_WALLPAPER_INFO));
        }

        //prompt fail
        MMIPUB_OpenAlertFailWin(TXT_ERROR);

        //SCI_TRACE_LOW:"MMIAPIFMM_SetPictureToWallpaper:get file info fail!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4906_112_2_18_2_20_15_251,(uint8*)"");
        result = FALSE;       
    }

    return (result);
}

#ifdef MMI_PIC_CROP_SUPPORT
/*****************************************************************************/
//  Description : pic crop callback
//  Global resource dependence : none
//  Author: chunyou
//  Note:
/*****************************************************************************/
LOCAL void SetPictureToWallpaperCallBack(MMIPICCROP_CB_T* callback)
{
    if(PNULL != callback)
    {
        switch (callback->err_type)
        {
        case MMI_PICCROP_SUCCESS:
        case MMI_PICCROP_SUCCESS_NO_CROP:
        case MMI_PICCROP_ERR_NOT_SUPPORT://格式不支持时直接设为墙纸，路径为传入的路径
            MMIAPIFMM_SetPictureToWallpaper(callback->is_pre_wallpaper, callback->full_path_ptr, callback->full_path_len);
            break;        
        default:
            break;
        }        
    }
}
#endif

/*****************************************************************************/
//  Description : 设置图片为墙纸,如果有图片裁剪，并格式支持，则先进入图片裁剪
//  Global resource dependence : 
//  Author: chunyou
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SetPictureToWallpaperEx(
                                           BOOLEAN      is_pre_wallpaper,   //是否是恢复前一张墙纸
                                           const wchar  *full_path_ptr,
                                           uint16       full_path_len
                                           )
{
    BOOLEAN result = TRUE;    

    if(PNULL == full_path_ptr)
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_SetPictureToWallpaperEx full_path_ptr is PNULL !!!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4950_112_2_18_2_20_15_252,(uint8*)"");
        return FALSE;
    }
    if(0 == full_path_len)
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_SetPictureToWallpaperEx full_path_len is PNULL !!!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_4955_112_2_18_2_20_15_253,(uint8*)"");
        return FALSE;
    }
#ifdef MMI_PIC_CROP_SUPPORT
    if(GUIANIM_TYPE_IMAGE_JPG == MMIAPICOM_GetImgaeType(full_path_ptr,full_path_len))
    {
        uint16  lcd_width = 0;
        uint16  lcd_height = 0;
        MMIPICCROP_SRC_T crop_src = {0};
        
        crop_src.crop_type = MMIPIC_CROP_WALLPAPER;
        crop_src.callback = SetPictureToWallpaperCallBack;
        crop_src.full_path_ptr = (wchar*)full_path_ptr;
        crop_src.full_path_len = full_path_len;

        GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);
#ifdef GRID_SLIDE_WALLPAPER
        crop_src.min_width = lcd_width * 2;
#else        
        crop_src.min_width = lcd_width;
#endif
        crop_src.min_height = lcd_height;
        MMIAPIPICCROP_OpenPicCrop(&crop_src);
    }
    else
#endif
    {
        result = MMIAPIFMM_SetPictureToWallpaper(is_pre_wallpaper, full_path_ptr, full_path_len);
    }

    return result;
}
/*****************************************************************************/
//  Description : set music file to ring
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetMusicToRing(
                                MMIFMM_SET_RING_TYPE_E   ring_type,
                                DPARAM                      param,
                                uint16                      *full_name_ptr,
                                uint16                      full_name_len
                                )
{
    MMISRVAUD_CONTENT_INFO_T content_info = {0};
    BOOLEAN                     is_success = TRUE;
//  MMI_STRING_T                wait_text = {0};

    uint32          file_size = 0;
    BOOLEAN         is_file_exist = FALSE;
    wchar           file_name[MMIFILE_FILE_NAME_MAX_LEN+1] = {0};
    uint16          file_name_len = 0;
#ifdef DRM_SUPPORT
    BOOLEAN                 is_audio_drm_file = FALSE;
    BOOLEAN                 is_drm_valid = FALSE;
    MMISRVAUD_RING_FMT_E         drm_audio_type = MMISRVAUD_RING_FMT_MAX;
#endif

    //SCI_ASSERT(NULL != full_name_ptr);
    if(PNULL == full_name_ptr) 
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_SetMusicToRing param error: PNULL == full_name_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5015_112_2_18_2_20_16_254,(uint8*)"");
        return;
    }

    s_set_ring_type = ring_type;
    
    switch(ring_type)
    {
        case MMIMULTIM_SET_CALL_RING:
            s_set_ring_dual_sys = *(MN_DUAL_SYS_E *)param;
            break;
        case MMIMULTIM_SET_MSG_RING:
            break;
        case MMIMULTIM_SET_ALARM_RING:
            s_set_ring_alarm_id = *(uint16 *)param;
            break;
        default:
            break;
    }

    is_file_exist = MMIAPIFMM_IsFileExist(full_name_ptr, full_name_len);
    if(is_file_exist)
    {
        MMIAPIFMM_GetFileInfo(full_name_ptr, full_name_len, &file_size, NULL, NULL);
    }

    if(is_file_exist && 0 < file_size)
    {
        MMIAPIFMM_SplitFullPath(
            full_name_ptr, full_name_len,
            NULL, NULL,NULL, NULL,
            file_name, &file_name_len);
		
#ifdef DRM_SUPPORT
        if(MMIAPIMP3_IsDrmFile(
                full_name_ptr, full_name_len,
                &drm_audio_type,&is_audio_drm_file,&is_drm_valid
                ))
        {
            if(!is_drm_valid)
            {
    			MMIPUB_OpenAlertWarningWin(TXT_DRM_COPYRIGHTS_INVLIAD_NOT_OPERATE);
                return;
            }
            else if(!is_audio_drm_file)
            {
    			MMIPUB_OpenAlertWarningWin(TXT_COMMON_NO_SUPPORT);
                return;
            }
        }        
#endif

        SCI_MEMSET(&s_set_ring_info, 0, sizeof(s_set_ring_info));
        MMI_WSTRNCPY(s_set_ring_info.name_wstr, MMISET_CALL_RING_NAME_MAX_LEN,
                   full_name_ptr, full_name_len, full_name_len);
        s_set_ring_info.name_wstr_len = (uint16)MIN(MMISET_CALL_RING_NAME_MAX_LEN,full_name_len);

        MMIAPISET_StopAppRing();

        if(MMISRVAUD_GetFileContentInfo(
            s_set_ring_info.name_wstr, 
            (uint32)s_set_ring_info.name_wstr_len,
            &content_info))
        {
            switch(s_set_ring_type)
            {
                case MMIMULTIM_SET_CALL_RING:
                    s_set_ring_info.name_wstr_len = s_set_ring_info.name_wstr_len;
                    if(!MMIAPIENVSET_SetMoreCallRingInfo(s_set_ring_dual_sys,s_set_ring_info,MMIAPIENVSET_GetActiveModeId()))
                    {
                        is_success = FALSE;
                    }
                    break;
                case MMIMULTIM_SET_MSG_RING:
                    break;
                case MMIMULTIM_SET_ALARM_RING:
                    if (!MMIAPIALARM_SetAlarmRing(s_set_ring_alarm_id, s_set_ring_info.name_wstr, s_set_ring_info.name_wstr_len))
					{
						is_success = FALSE;
					}
                    break;
                default:
                    break;
            }
            if (is_success)
            {
                //MMIPUB_OpenAlertSuccessWin(TXT_SUCCESS);
            }
            else
            {
                if(MMIMULTIM_SET_ALARM_RING==s_set_ring_type)
                {
                    MMIPUB_OpenAlertWarningWin(TXT_NO_SPACE);
                }
                else
                {
                    MMIPUB_OpenAlertFailWin(TXT_ERROR);
                }
            }
            
            //MMK_PostMsg(MMIMULTIM_FILE_LIST_WIN_ID, MSG_MULTIM_FILE_LIST_UPDATE, NULL, 0);
        }
        else
        {
            //SCI_TRACE_LOW:"MMIAPIFMM_SetMusicToRing: create audio file handle error!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5119_112_2_18_2_20_16_255,(uint8*)"");
#ifdef MMI_AUDIO_PLAYER_SUPPORT
            MMIPUB_OpenAlertWarningWin(TXT_COMMON_NO_SUPPORT);
#endif
        }
    }
    else
    {
        if(!is_file_exist)
        {
            MMIPUB_OpenAlertWarningWin(TXT_COM_FILE_NO_EXIST);
        }
        else if(0 == file_size)
        {
            MMIPUB_OpenAlertWarningWin(TXT_EMPTY_FILE);
        }
    }
}

/*****************************************************************************/
//  Description : set win title
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetFileNameWinTitle(
                                  MMI_WIN_ID_T    win_id, 
                                  const wchar     *file_name_ptr,
                                  uint16          file_name_len,
                                  BOOLEAN         is_refresh
                                  )
{
    wchar                       name[MMIFILE_FILE_NAME_MAX_LEN+1] = {0};
    uint16                      name_len = 0;
    uint16                      title_max_len = 0;
    
    //SCI_ASSERT(NULL != file_name_ptr);
    //SCI_ASSERT(0 < file_name_len);
    if((PNULL == file_name_ptr) || (0 == file_name_len))
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_SetFileNameWinTitle param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5157_112_2_18_2_20_16_256,(uint8*)"");
        return;
    }

    title_max_len = MMITHEME_GetWinTitleMaxLen();

    MMI_WSTRNCPY( name, MMIFILE_FILE_NAME_MAX_LEN, file_name_ptr, file_name_len, file_name_len );
    name_len = file_name_len;

    if(title_max_len < name_len && title_max_len> MMIFMM_ELLIPSIS_LEN)
    {
        name_len = title_max_len;

        MMI_WSTRNCPY(&name[title_max_len-MMIFMM_ELLIPSIS_LEN], MMIFMM_ELLIPSIS_LEN,
            MMIFMM_ELLIPSIS, MMIFMM_ELLIPSIS_LEN, MMIFMM_ELLIPSIS_LEN);
    }

    GUIWIN_SetTitleText(win_id, name, name_len, is_refresh);
    GUIWIN_SetTitleTextAlign(win_id, ALIGN_HMIDDLE);
}

/*****************************************************************************/
//  Description : reset pic wallpaper setting
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_ResetWallpaperSetting(void)
{
    MMISET_WALLPAPER_SET_T  wallpaper_info = {0};

    SCI_MEMSET(&wallpaper_info,0,sizeof(MMISET_WALLPAPER_SET_T));

    //reset wallpaper name
    SCI_MEMSET(&s_file_as_wallpaper_info, 0, sizeof(MMIMULTIM_AS_WALLPAPER_INFO));
    MMINV_WRITE(MMINV_MULTIM_AS_WALLPAPER_INFO, &s_file_as_wallpaper_info);

    MMIAPISET_GetWallpaperSetInfo(&wallpaper_info);
    if(MMISET_WALLPAPER_DOWNLOAD == wallpaper_info.wallpaper_type)
    {
        wallpaper_info.wallpaper_id = MMISET_WALLPAPER_DEFAULT_ID;
        wallpaper_info.wallpaper_type = MMISET_WALLPAPER_FIXED_PIC;
        MMIAPISET_SetWallpaperSetInfo(wallpaper_info);
    }
}

/*****************************************************************************/
//  Description : set wallpaper,open waiting window
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenWaitSetWallpaper(void)
{
    uint32          time_out = 0;
    MMI_STRING_T    wait_text = {0};
    MMI_WIN_ID_T    win_id = MMIMULTIM_SET_WALLPAPER_WAIT_WIN_ID;

    //进入等待界面,必须是单张图片,否则低优先级别的图片decode无法得到时间运行
    MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);
    MMIPUB_OpenAlertWinByTextId(&time_out,TXT_COMMON_WAITING,TXT_NULL,IMAGE_PUBWIN_WAIT,
        &win_id,PNULL,MMIPUB_SOFTKEY_NONE,PNULL);
}

/*****************************************************************************/
//  Description : get wallpaper information
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC MMIFMM_AS_WALLPAPER_INFO *MMIAPIFMM_GetWallPaperInfo(void)
{
    return &s_file_as_wallpaper_info;
}

/*****************************************************************************/
//  Description : 产生随机的文件名，并且设置.
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetRandomNameEditbox(
                                   const uint8              *prefix_ptr, //in    
                                   MMI_CTRL_ID_T            ctrl_id     //in
                                   )
{
    uint16          random = 0;
    //MMI_STRING_T    ucs2_str = {0};
    wchar temp_wstr[MMIFILE_FILE_NAME_MAX_LEN+1] = {0};
    char  temp_str[MMIFILE_FILE_NAME_MAX_LEN+1] = {0};
    uint16 len = 0;
    
    //SCI_ASSERT(NULL != prefix_ptr);
    if(PNULL == prefix_ptr)
    {
        //SCI_TRACE_LOW:"MMIAPIFMM_SetRandomNameEditbox param error:PNULL == prefix_ptr"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5244_112_2_18_2_20_16_257,(uint8*)"");
        return;
    }

    //generate random
    srand(SCI_GetTickCount());
    random = (uint16)rand();
    sprintf( temp_str,"%s%05d", prefix_ptr,random); 
    len = (uint16)strlen( temp_str );
    //ucs2_str.is_ucs2 = FALSE;

    MMI_STRNTOWSTR( temp_wstr, MMIFILE_FILE_NAME_MAX_LEN, (uint8*)temp_str, MMIFILE_FILE_NAME_MAX_LEN, len );

    GUIEDIT_SetString(ctrl_id,
                                        temp_wstr,
                                        len );
}

/*****************************************************************************/
//  Description : get ticked file name in open file window
//  Global resource dependence : 
//  Author: Jassmine 
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_GetOpenFileWinTickedFile(
                                               wchar   *file_name_ptr, //in:
                                               uint16  *name_len_ptr   //in/out:in传入file name max len
                                               )
{
    BOOLEAN     result = FALSE;

    //SCI_ASSERT(PNULL != file_name_ptr);
    //SCI_ASSERT(PNULL != name_len_ptr);
    if((PNULL == file_name_ptr) || (PNULL == name_len_ptr))
    {
        //SCI_TRACE_LOW:"MMIFMM_GetOpenFileWinTickedFile param error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5277_112_2_18_2_20_16_258,(uint8*)"");
        return FALSE;
    }

    if ((PNULL != s_select_win_param_ptr) &&
        (0 < s_select_win_param_ptr->ticked_file_len))
    {
        MMI_WSTRNCPY(file_name_ptr,*name_len_ptr,
            s_select_win_param_ptr->ticked_file,s_select_win_param_ptr->ticked_file_len,
            s_select_win_param_ptr->ticked_file_len);
        *name_len_ptr = s_select_win_param_ptr->ticked_file_len;

        result = TRUE;
    }

    return (result);
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIFMM_AlertNoSpaceInDisk(void)
{
	MMIPUB_OpenAlertWarningWin(TXT_NO_SPACE);			
}

/*****************************************************************************/
//  Description : 和MMIFMM_SearchFileInPath配合使用
//  Return: 
//  Global resource dependence : 
//  Author: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileStop(void)
{
	MMIFMM_SearchFileStop();
}

/*****************************************************************************/
//  Description : check memory card status
//  Return: 
//  Global resource dependence : 
//  Author: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CheckMemoryCard(MMIFILE_DEVICE_E file_dev)
{
	//BOOLEAN udisk_status = FALSE;
	//BOOLEAN sd_status = FALSE;
    BOOLEAN result = FALSE;
	BOOLEAN need_format = FALSE;
    MMI_TEXT_ID_T alert_text_id = TXT_NULL;
    MMI_TEXT_ID_T device_text_id = TXT_NULL;
	//MMIFILE_DEVICE_E device_type = MMI_DEVICE_UDISK;
    MMIFILE_ERROR_E device_error = SFS_NO_ERROR;
	if(MMIAPIUDISK_UdiskIsRun())
	{
		//u disk runing
        alert_text_id = TXT_COMMON_UDISK_USING;
		result = FALSE;
	}
	else if(file_dev >= MMI_DEVICE_NUM)
	{
		//no sd card
        /* Modify by michael on 2/27/2012 for NEWMS00172443 : 统一修改弹出信息*/
        //alert_text_id = TXT_SD_NO_EXIST;
        alert_text_id = TXT_PLEASE_INSERT_SD;
        /************************Modify over*************************/
		result = FALSE;
	}
	else if((MMI_DEVICE_SDCARD == file_dev) && MMIAPISD_IsCardLogOn())
	{
		//card log runging
		device_text_id = MMIAPIFMM_GetDeviceName(file_dev);
		alert_text_id = TXT_COMMON_CARDLOG_USING;
		result = FALSE;
	}
	else
	{
		//device_type = MMIFILE_GetDeviceTypeByFileDev(file_dev);
		device_text_id = MMIAPIFMM_GetDeviceName(file_dev);
		if(MMIAPIFMM_GetDeviceStatus((uint16 *)MMIAPIFMM_GetDevicePath(file_dev), MMIAPIFMM_GetDevicePathLen(file_dev)))
        {                    
            result = TRUE;
        }
        else
        {   
			device_error = MMIAPIFMM_RegisterDevice((uint16 *)MMIAPIFMM_GetDevicePath(file_dev), MMIAPIFMM_GetDevicePathLen(file_dev));
			if(SFS_NO_ERROR == device_error)
			{
				result = TRUE;
			}
			if(SFS_ERROR_INVALID_FORMAT == device_error)
			{
				need_format = TRUE;
			}	
		}
		if(!result && (!need_format))
		{
			//device not exist
			alert_text_id = TXT_COMMON_NOT_EXIST;
			//result = FALSE;
		}
		else if(!result)
		{
			alert_text_id = TXT_COMMON_FORMAT_ERROR;
		}
	}
	if(!result)
	{
		//show error message
		MMIPUB_OpenAlertWinByTextId(PNULL,device_text_id,alert_text_id,IMAGE_PUBWIN_WARNING,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
	}
	//SCI_TRACE_LOW("[baokun] MMIAPIFMM_CheckMemoryCard file_dev %d result %d", file_dev, result);
	return result;
	//MMIAPIFMM_SearchFileStop();
}

/*****************************************************************************/
// Description : MMIAPIFMM_SetFolderDetail
// Global resource dependence : 
// Author: ying.xu
// Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetFolderDetail(
                                 MMIFMM_DETAIL_DATA_T       *detail_data,
                                 MMI_CTRL_ID_T              ctrl_id ,
                                 MMI_WIN_ID_T            win_id
                                 )
{  
     //SCI_TRACE_LOW:"MMIFMM: MMIAPIFMM_SetFolderDetail, detail_data =%x"
     SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5400_112_2_18_2_20_16_259,(uint8*)"d", detail_data );
  
    if(PNULL == detail_data )
    {      
        return ;
     }
    if(MMIFMM_FILE_TYPE_FOLDER == detail_data->type)
    {            
          MMI_STRING_T                wait_text = {0};      
            MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);
            MMIPUB_OpenWaitWinEx(MMK_GetFirstAppletHandle(), 
                1,
                &wait_text,
                PNULL,
                PNULL,
                MMIFMM_FOLDER_DETAIL_INFOMATION_WAITING_WIN_ID,
                IMAGE_NULL,
                ANIM_PUBWIN_WAIT,
                WIN_ONE_LEVEL,
                MMIPUB_SOFTKEY_ONE,
                (MMIPUB_HANDLE_FUNC)HandleFolderDetailWaitWinMsg,
                (uint32)detail_data);                
    }   
      return ;
}
/*****************************************************************************/
//  Description : fmm detail information waiting win msg hanlement
//  Global resource dependence : none
//  Author:     ying.xu
//  Note:       20100920
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleFolderDetailWaitWinMsg(
                                       MMI_WIN_ID_T              win_id,
                                       MMI_MESSAGE_ID_E      msg_id,
                                       DPARAM                        param
                                       )   
{
    MMI_RESULT_E                result = MMI_RESULT_TRUE;
    static FILEARRAY            file_array = PNULL;  
    MMIPUB_INFO_T*              add_data  =  MMIPUB_GetWinAddDataPtr(win_id );
    
    MMIFMM_DETAIL_DATA_T*  detail_data = (MMIFMM_DETAIL_DATA_T*)(add_data->user_data);
    
    //SCI_TRACE_LOW:"MMIFMM: HandleFolderDetailWaitWinMsg, win_id =%d, msg_id = 0x%x."
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5441_112_2_18_2_20_16_260,(uint8*)"dd", win_id, msg_id);
    
    switch( msg_id)
    {
    case MSG_OPEN_WINDOW:      
        {   
            MMIFMM_FILTER_T             filter = {"*"};       
            file_array = MMIAPIFILEARRAY_Create();
            // MMIAPIFMM_SearchFileInPath(detail_data->filename,detail_data->filename_len,&filter,TRUE,FUNC_FIND_FILE_AND_FOLDER,file_array,0,0);
            MMIFMM_SearchFileInPathNoSort(detail_data->filename,detail_data->filename_len,&filter,
            TRUE,FUNC_FIND_FILE_AND_FOLDER,file_array, win_id, MSG_FMM_SEARCH_CNF);
        }
       result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, param);
        break;

        
      case MSG_FMM_SEARCH_CNF:   
        {                      
                uint16 total_num = 0;
                uint16 i=0;               
                FILEARRAY_DATA_T    f_array_data = {0} ;
                total_num = (uint16)MMIAPIFILEARRAY_GetArraySize(file_array);
                
                for (i = 0;i<total_num;i++)
                {
                    SCI_MEMSET(&f_array_data, 0, sizeof(f_array_data));
                    if(MMIAPIFILEARRAY_Read(file_array, i, &f_array_data))
        			{
        				if (FILEARRAY_TYPE_FOLDER ==f_array_data.type)
        				{
        					detail_data->folder_num++;
        				}
        				else
        				{
        					detail_data->file_num++;
        				}
        				detail_data->file_size +=f_array_data.size;
        			}
                }
              MMIAPIFILEARRAY_Destroy(&file_array);         
         }          
       MMK_PostMsg(MMIFMM_FOLDER_DETAIL_WIN_ID, MSG_FMM_SEARCH_CNF, PNULL, 0);
       MMK_CloseWin( win_id );   
        break; 

    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMIFMM_SearchFileStop();
        MMK_SendMsg(MMIFMM_FOLDER_DETAIL_WIN_ID, msg_id, PNULL);
        MMK_CloseWin( win_id );  
        //MMK_PostMsg(MMIFMM_FILE_DETAIL_WIN_TAB, MSG_CLOSE_WINDOW, PNULL, 0);
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, param);
        break;
        
    case MSG_CLOSE_WINDOW:
         MMIAPIFILEARRAY_Destroy(&file_array);
         MMIFMM_SearchFileStop();       
        break;
    default:
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, param);
        break;
    }

    return (result);
}

/*****************************************************************************/
// Description : MMIAPIFMM_SetFolderDetailCNF
// Global resource dependence : 
// Author: YING.XU
// Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetFolderDetailCNF(
                                 MMIFMM_DETAIL_DATA_T       *detail_data,
                                 MMI_CTRL_ID_T              ctrl_id ,
                                 MMI_WIN_ID_T            win_id
                                 )
{
     MMI_TM_T                    tm = {0};
    int32                       i = 0; 
    uint8                       detail_arr[MMIFMM_FULL_FILENAME_LEN] = {0};
    wchar                       wchar_detail_arr[MMIFMM_FULL_FILENAME_LEN + 1] = {0};
    uint16                      nIndex = 0;
//     uint16                      total_num = 0;
//     uint16                      pic_width = 0;
//     uint16                      pic_height = 0; 
    uint16                      string_len=0;

       //SCI_TRACE_LOW:"MMIFMM: MMIAPIFMM_SetFolderDetailCNF, detail_data =%x"
       SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5527_112_2_18_2_20_17_261,(uint8*)"d", detail_data );
  
    if(PNULL == detail_data )
    {      
        return ;
     }
    
//set name title
    SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));   
    string_len = (uint16)MMIAPICOM_Wstrlen( detail_data->filename);
    for(i=string_len-1;MMIFILE_SLASH !=detail_data->filename[i] && i> 0;i--)
    {
        NULL;
    }

    AddOneDetailItem(ctrl_id,TXT_INPUT_FILE_NAME,PNULL,&nIndex,&detail_data->filename[i+1],(string_len-i-1));

//set date
    tm = MMIAPICOM_Second2Tm(detail_data->time);
    MMIAPISET_FormatDateStrByDateStyle(tm.tm_year, tm.tm_mon, tm.tm_mday,'.',detail_arr,MMIFMM_FULL_FILENAME_LEN);

    MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
    AddOneDetailItem(ctrl_id,TXT_COMMON_DATE,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));

//set time
    SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));

    MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,detail_arr,MMIFMM_FULL_FILENAME_LEN);
    MMIAPICOM_StrToWstr(detail_arr,wchar_detail_arr );
    AddOneDetailItem(ctrl_id,TXT_COMMON_TIME,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));

//set size 
    //SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
    SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));   
    //sprintf((char *)detail_arr, "%ld.%03ldK", (detail_data->file_size /1024), ((detail_data->file_size %1024)*1000/1024));
	MMIAPIFMM_GetFileSizeString(detail_data->file_size, wchar_detail_arr, MMIFMM_SIZE_STR_LEN, FALSE);
	string_len = MMIAPICOM_Wstrlen(wchar_detail_arr);
    //MMIAPICOM_StrToWstr((char*) detail_arr,wchar_detail_arr );
    AddOneDetailItem(ctrl_id,TXT_DETAIL_SIZE,PNULL,&nIndex,wchar_detail_arr, string_len);
     
//set location
    
        AddOneDetailItem(ctrl_id,
            TXT_LOCATION ,
            MMIAPIFMM_GetDeviceName(MMIAPIFMM_GetDeviceTypeByPath(&detail_data->filename[0], MMIFILE_DEVICE_LEN)),
            &nIndex,PNULL,0);
    
        //set folder
        SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
        SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
        sprintf((char *)detail_arr, "%ld", detail_data->folder_num);
        MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
        AddOneDetailItem(ctrl_id,TXT_FMM_SUB_FOLDER,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));

        //set sub file num
        SCI_MEMSET(detail_arr,0,sizeof(detail_arr));
        SCI_MEMSET(wchar_detail_arr,0,sizeof(wchar_detail_arr));
        sprintf((char *)detail_arr, "%ld", detail_data->file_num);
        MMIAPICOM_StrToWstr( detail_arr,wchar_detail_arr );
        AddOneDetailItem(ctrl_id,TXT_FMM_SUB_FILE_NUM,PNULL,&nIndex,wchar_detail_arr,(uint16)strlen((char*)detail_arr));
   
    
}

/*****************************************************************************/
//  Description : get folder name by current language type
//  Global 
//  Author: 
//  Note: 
/*****************************************************************************/
/*PUBLIC void MMIAPIFMM_InitDefaultFoldeName(
                                     MMIFMM_FOLDER_TYPE_E   folder_type,
                                     MMISET_LANGUAGE_TYPE_E lang_type
                                     )
{
    wchar         *folder_name_ptr = PNULL;
	MMI_STRING_T  folder_name_str = {0};
	MMI_TEXT_ID_T folder_name_id = TXT_NULL;

	if(folder_type < MMIFMM_FOLDER_TYPE_NUM)
	{
		folder_name_ptr = s_folder_buffer[folder_type];
       folder_name_id = s_folder_text_id[folder_type];
        MMI_GetLabelTextByLang(folder_name_id, &folder_name_str);
		if(folder_name_ptr != PNULL)
		{
              SCI_MEMSET(folder_name_ptr, 0x00, (sizeof(wchar)*MMIFILE_FULL_PATH_MAX_LEN));
			  MMIAPICOM_Wstrncpy(folder_name_ptr, folder_name_str.wstr_ptr, MIN(folder_name_str.wstr_len, MMIFILE_FULL_PATH_MAX_LEN));
		}
	}
}*/


#ifdef CMCC_UI_STYLE
/*****************************************************************************/
//  Description : open music file
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenMusicFileList(void)
{
    wchar save_path[MMIFMM_FILE_FULL_PATH_MAX_LEN+1] = {0};
    uint16 save_path_len = MMIFMM_FILE_FULL_PATH_MAX_LEN+1;
    MMIFILE_DEVICE_E devie = MMI_DEVICE_SDCARD;
    
	devie = MMIAPIFMM_GetFirstValidDevice();
	if(devie < MMI_DEVICE_NUM)
	{
        MMIAPIFMM_CombineFullPath(
                    MMIAPIFMM_GetDevicePath(devie), MMIAPIFMM_GetDevicePathLen(devie), 
                    MMIMULTIM_DIR_MUSIC, MMIMULTIM_DIR_MUSIC_LEN, PNULL, NULL, 
                    save_path, &save_path_len
                    );
        MMIAPIFMM_OpenExplorerExt(save_path, save_path_len, PNULL, 
            0, TRUE, MMIFMM_MUSIC_ALL);
	}
    
}

/*****************************************************************************/
//  Description : open movie file
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenMovieFileList(void)
{
    wchar save_path[MMIFMM_FILE_FULL_PATH_MAX_LEN+1] = {0};
    uint16 save_path_len = MMIFMM_FILE_FULL_PATH_MAX_LEN+1;
    MMIFILE_DEVICE_E devie = MMI_DEVICE_SDCARD;
    
	devie = MMIAPIFMM_GetFirstValidDevice();
	if(devie < MMI_DEVICE_NUM)
	{
        MMIAPIFMM_CombineFullPath(
                    MMIAPIFMM_GetDevicePath(devie), MMIAPIFMM_GetDevicePathLen(devie), 
                    MMIMULTIM_DIR_MOVIE, MMIAPICOM_Wstrlen(MMIMULTIM_DIR_MOVIE), 
					PNULL, NULL, 
                    save_path, &save_path_len
                    );
        MMIAPIFMM_OpenExplorerExt(save_path, save_path_len, PNULL, 
            0, TRUE, MMIFMM_MOVIE_ALL);
	}    
}
#endif//#ifdef CMCC_UI_STYLE

#ifdef DRM_SUPPORT
/*****************************************************************************/
// Description : add drm file detail
// Global resource dependence : 
// Author:haiwu.chen
// Note: 由于协议目前只能给出单个版权，所以目前实现单个版权的详情显示
/*****************************************************************************/
PUBLIC void MMIAPIFMM_AddDrmFileDetail(
    const uint16 *file_name,
    uint32 access_mode, 
    uint32 share_mode, 
    uint32 file_attri,
    MMI_CTRL_ID_T ctrl_id
)
{
    MMIDRMFILE_HANDLE drm_handle = 0;
    MMI_TEXT_ID_T title_name = 0;
    MMI_TEXT_ID_T title_value = 0;
    uint8 detail_arr[MMIFMM_FULL_FILENAME_LEN] = {0};
    wchar wchar_detail_arr[MMIFMM_FULL_FILENAME_LEN + 1] = {0};
    uint16  nIndex = 0;
    MMI_TM_T tm = {0};
    DRM_RIGHTS_T drm_rights = {0};
    DRM_PERMISSION_MODE_E drm_permission = DRM_PERMISSION_NONE;
    BOOLEAN is_forward = FALSE;
	
    if (PNULL == file_name)
    {
        return;
    }

    drm_handle = SFS_CreateFile(file_name, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, 0, 0);
    if (SFS_INVALID_HANDLE == drm_handle)
    {
        return;
    }
    
    SCI_MEMSET(&drm_rights, 0, sizeof(DRM_RIGHTS_T));    
    MMIAPIDRM_GetRightsinfo(drm_handle, PNULL, &drm_rights);
    drm_permission = MMIAPIDRM_GetFilePermission(drm_handle, PNULL);
    is_forward = MMIAPIDRM_IsCanForward(&drm_rights, SFS_INVALID_HANDLE, PNULL);
    if (MMIAPIDRM_IsRightsValid(drm_handle, PNULL, TRUE, drm_permission))
    {
        const DRM_CONSTRAINT_T *rights_cons_ptr = PNULL;
        
        //数字版权:有效
        title_name = TXT_DRM_DIGITAL_RIGHTS;
        title_value = TXT_DRM_DIGITAL_RIGHTS_VALID;
        AddOneDetailItem(ctrl_id, title_name, title_value, &nIndex, PNULL, 0);

        //转发禁止:是
        title_name = TXT_DRM_FORWARD_FORBIDDEN;
        title_value = is_forward ? TXT_DRM_FORWARD_FBN_NO : TXT_DRM_FORWARD_FBN_YES;
        AddOneDetailItem(ctrl_id, title_name, title_value, &nIndex, PNULL, 0);

        //只有下面这两种类型的才可能会有各种限制等
        if (DRM_LEVEL_CD == drm_rights.method_level
            || DRM_LEVEL_SD == drm_rights.method_level)
        {
            //先得到各个限制的数据
            rights_cons_ptr = &(drm_rights.constraint);
//            if (PNULL == rights_cons_ptr)
//            {
//                SFS_CloseFile(drm_handle);
//                return;
//            }
            
            //限制可能是组合的，也可以单独存在，但是datetime 与interval只能取一个
            if (0 != (rights_cons_ptr->type & DRM_CONSTRAINT_COUNT))
            {
                //剩余次数:6/100
                SCI_MEMSET(detail_arr, 0, sizeof(detail_arr));
                SCI_MEMSET(wchar_detail_arr, 0, sizeof(wchar_detail_arr)); 
                title_name = TXT_DRM_REMAIND_NUMBER;
                sprintf((char*)detail_arr, "%u/%u", rights_cons_ptr->total_count - rights_cons_ptr->used_count, rights_cons_ptr->total_count);
                MMIAPICOM_StrToWstr(detail_arr, wchar_detail_arr);
                AddOneDetailItem(ctrl_id, title_name, PNULL, &nIndex, wchar_detail_arr, (uint16)strlen((char*)detail_arr));
            }

            if (0 != (rights_cons_ptr->type & DRM_CONSTRAINT_INTERVAL))
            {
                uint8 detail_arr1[MMIFMM_FULL_FILENAME_LEN] = {0};
                DRM_RIGHTS_INTERVAL_T interval_time = {0};
                
                MMIAPIDRM_GetDateIntervalTime(drm_handle, &interval_time);
                
                //版权开始日期:2011.3.9 12:10
                SCI_MEMSET(detail_arr1, 0, sizeof(detail_arr1));
                SCI_MEMSET(wchar_detail_arr, 0, sizeof(wchar_detail_arr)); 
                title_name = TXT_DRM_REMAIND_TIME;
                
                sprintf((char*)detail_arr1, "%4dY-%2dM-%2dD %2dH:%2dM:%2dS", 
                    interval_time.date.year, interval_time.date.mon, interval_time.date.mday,
                    interval_time.time.hour, interval_time.time.min, interval_time.time.sec);
                MMIAPICOM_StrToWstr(detail_arr1, wchar_detail_arr);
                AddOneDetailItem(ctrl_id, title_name, PNULL, &nIndex, wchar_detail_arr, (uint16)strlen((char*)detail_arr1));                
            }
            
            if (0 != (rights_cons_ptr->type & DRM_CONSTRAINT_DATETIME))
            {
                //等明天需求定义清楚了再来添加剩余时间                    
                uint8 detail_arr1[MMIFMM_FULL_FILENAME_LEN] = {0};
                uint8 detail_arr2[MMIFMM_FULL_FILENAME_LEN] = {0};
                MMIDRM_NITZ_TYPE_T nitz_time = {0};
                uint32 time_value = 0;
                
                MMIAPIDRM_GetDrmNitzTime(&nitz_time);
                time_value = nitz_time.is_syn_time ? nitz_time.sys_time_value : 0;
                
                //版权开始日期:2011.3.9 12:10
                SCI_MEMSET(detail_arr1, 0, sizeof(detail_arr1));
                SCI_MEMSET(detail_arr2, 0, sizeof(detail_arr2));
                SCI_MEMSET(wchar_detail_arr, 0, sizeof(wchar_detail_arr)); 
                title_name = TXT_DRM_BEGIN_TIME;
                
                if (DRM_NET_TIME_ADD == nitz_time.net_time_type)
                {
                    time_value = rights_cons_ptr->start_time + time_value;
                }
                else if (DRM_NET_TIME_REDUCE == nitz_time.net_time_type)
                {
                    time_value = rights_cons_ptr->start_time - time_value;
                }
                else
                {
                    time_value = rights_cons_ptr->start_time;
                }
                
                tm = MMIAPICOM_Second2Tm(time_value);
                MMIAPISET_FormatDateStrByDateStyle(tm.tm_year, tm.tm_mon, tm.tm_mday,'.',detail_arr1,MMIFMM_FULL_FILENAME_LEN);
                MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min,detail_arr2,MMIFMM_FULL_FILENAME_LEN);
                strcat((char*)detail_arr1, " ");
                strcat((char*)detail_arr1, (char*)detail_arr2);                
                MMIAPICOM_StrToWstr(detail_arr1, wchar_detail_arr);    
                AddOneDetailItem(ctrl_id, title_name, PNULL, &nIndex, wchar_detail_arr, (uint16)strlen((char*)detail_arr1));
                

                //版权结束日期:2011.3.9 12:10
                SCI_MEMSET(detail_arr1, 0, sizeof(detail_arr1));
                SCI_MEMSET(detail_arr2, 0, sizeof(detail_arr2));
                SCI_MEMSET(wchar_detail_arr, 0, sizeof(wchar_detail_arr)); 
                title_name = TXT_DRM_END_TIME;
                time_value = nitz_time.is_syn_time ? nitz_time.sys_time_value : 0;

                if (DRM_NET_TIME_ADD == nitz_time.net_time_type)
                {
                    time_value = rights_cons_ptr->end_time + time_value;
                }
                else if (DRM_NET_TIME_REDUCE == nitz_time.net_time_type)
                {
                    time_value = rights_cons_ptr->end_time - time_value;
                }
                else
                {
                    time_value = rights_cons_ptr->end_time;
                }
                
                tm = MMIAPICOM_Second2Tm(time_value);
                MMIAPISET_FormatDateStrByDateStyle(tm.tm_year, tm.tm_mon, tm.tm_mday,'.',detail_arr1,MMIFMM_FULL_FILENAME_LEN);
                MMIAPISET_FormatTimeStrByTime(tm.tm_hour, tm.tm_min, detail_arr2, MMIFMM_FULL_FILENAME_LEN);
                strcat((char*)detail_arr1, " ");
                strcat((char*)detail_arr1, (char*)detail_arr2);                
                MMIAPICOM_StrToWstr(detail_arr1, wchar_detail_arr);    
                AddOneDetailItem(ctrl_id, title_name, PNULL, &nIndex, wchar_detail_arr, (uint16)strlen((char*)detail_arr1));
            }           
        }
        else
        {
            //fl类型的,无内容再显示了
        }
    }
    else
    {
        //数字版权:无效
        title_name = TXT_DRM_DIGITAL_RIGHTS;
        title_value = TXT_DRM_DIGITAL_RIGHTS_INVALID;
        AddOneDetailItem(ctrl_id, title_name, title_value, &nIndex, PNULL, 0);
        //转发禁止:是
        title_name = TXT_DRM_FORWARD_FORBIDDEN;
        title_value = is_forward ? TXT_DRM_FORWARD_FBN_NO : TXT_DRM_FORWARD_FBN_YES;
        AddOneDetailItem(ctrl_id, title_name, title_value, &nIndex, PNULL, 0);
    }

    SFS_CloseFile(drm_handle);
    return;
}

/*****************************************************************************/
//  Description : combine drm suffix
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 通过非DRM的后缀，返回组合上drm的后缀
/*****************************************************************************/
PUBLIC uint32 MMIFMM_CombineDrmSuffixInfo(uint32 suffix_type)
{
    //如果此种后缀drm支持，则需要查找时需要加上drm的相关后缀

    BOOLEAN is_support = FALSE;
    uint32 new_suffix = suffix_type;

    is_support = (
        (MMIFMM_PIC_ALL & suffix_type)
        || (MMIFMM_MUSIC_ALL & suffix_type)
        || (MMIFMM_MOVIE_ALL & suffix_type)
        || (MMIFMM_TXT_ALL & suffix_type)
        || (MMIFMM_JAVA_JAR & suffix_type)
        || (MMIFMM_JAVA_JAD & suffix_type)
        );
    
    if (is_support)
    {
        new_suffix = suffix_type | MMIFMM_DM_FILE;
        new_suffix = new_suffix | MMIFMM_DCF_FILE;
    }

    return new_suffix;
}

/*****************************************************************************/
//  Description : 通过给定的字符，查出当前支持的后缀类型
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
PUBLIC void MMIFMM_GetSuffixTypeInfo(char *filter_str, uint32 *suffix_type_ptr)
{
    uint16 i = 0;    
    uint32 suffix_type = 0;

    if (PNULL == filter_str
        || PNULL == suffix_type_ptr)
    {
        //SCI_TRACE_LOW:"[MMIDRM] MMIFMM_GetSuffixTypeInfo: invalid param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5896_112_2_18_2_20_17_262,(uint8*)"");
        return;
    }

    if (0 == strcmp(filter_str, "*.*")
        || 0 == strcmp(filter_str, "*"))
    {
        //如果是*.*，则所有的都支持
        suffix_type = MMIFMM_FILE_ALL;
        *suffix_type_ptr = suffix_type;
        //SCI_TRACE_LOW:"[MMIDRM] MMIFMM_GetSuffixTypeInfo: user set all file type is %x!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5906_112_2_18_2_20_17_263,(uint8*)"d", suffix_type);
        return;
    }
    
    for (i = 0; i < ARR_SIZE(s_fmm_all_suffix); i++)
    {
        char *find_ptr = PNULL;
   
        if (s_fmm_all_suffix[i].is_support)
        {
            find_ptr = strstr(filter_str, (char*)s_fmm_all_suffix[i].suffix);
            if (PNULL != find_ptr)
            {
                suffix_type |= s_fmm_all_suffix[i].type;
            }
        }
    }
    
    *suffix_type_ptr = suffix_type;

    //SCI_TRACE_LOW:"[MMIDRM] MMIFMM_GetSuffixTypeInfo: user set file type is %x!"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_5926_112_2_18_2_20_17_264,(uint8*)"d", suffix_type);
    return;
}

/*****************************************************************************/
//  Description : drm support type
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_IsDrmSupportType(uint32 suffix_type)
{
    BOOLEAN is_ret = FALSE;
    uint32 i = 0;
    uint32 support_type[] = {MMIFMM_PIC_ALL, MMIFMM_MUSIC_ALL, MMIFMM_MOVIE_ALL, MMIFMM_TXT_ALL};
    for (i = 0; i < ARR_SIZE(support_type); i++)
    {
		is_ret = ((MMIFMM_FILE_TYPE(suffix_type) == MMIFMM_FILE_TYPE(support_type[i]))//大类别相同
			&&(MMIFMM_MEDIA_TYPE(suffix_type) & MMIFMM_MEDIA_TYPE(support_type[i]))//小类别包含
			);
        //is_ret = (suffix_type & support_type[i]);
        if (is_ret)
        {
            return is_ret;
        }
    }
    
    return is_ret;
}

/*****************************************************************************/
//  Description : get drm icon id by full path
//  Global resource dependence :
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MMIAPIFMM_GetDrmIconId(uint16 *file_path_ptr)
{
    MMI_IMAGE_ID_T icon_id = IMAGE_DRM_UNKNOWN_LOCK_ICON;
    BOOLEAN is_valid = FALSE;
    MMIFMM_FILE_TYPE_E file_type = MMIFMM_FILE_TYPE_NORMAL;
    MMIDRMFILE_HANDLE drm_handle = 0;
    DRM_PERMISSION_MODE_E drm_permission = DRM_PERMISSION_NONE;
	
    if (PNULL == file_path_ptr)
    {
        return icon_id;
    }
    
    drm_handle = SFS_CreateFile(file_path_ptr, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, 0, 0);
    if (SFS_INVALID_HANDLE == drm_handle)
    {
        return icon_id;
    }

    file_type = MMIAPIDRM_GetMediaFileType(drm_handle, PNULL);
    drm_permission = MMIAPIDRM_GetFilePermission(drm_handle, PNULL);    
    is_valid = MMIAPIDRM_IsRightsValid(drm_handle, PNULL, TRUE, drm_permission);

    switch( file_type )
    {
    case MMIFMM_FILE_TYPE_PICTURE:
        icon_id = is_valid ? IMAGE_DRM_IMAGE_UNLOCK_ICON: IMAGE_DRM_IMAGE_LOCK_ICON;
        break;

    case MMIFMM_FILE_TYPE_MUSIC:
        icon_id = is_valid ? IMAGE_DRM_AUDIO_UNLOCK_ICON: IMAGE_DRM_AUDIO_LOCK_ICON;
        break;

    case MMIFMM_FILE_TYPE_MOVIE:
        icon_id = is_valid ? IMAGE_DRM_VIDEO_UNLOCK_ICON: IMAGE_DRM_VIDEO_LOCK_ICON;
        break;
#if defined MMI_VCARD_SUPPORT
    case MMIFMM_FILE_TYPE_VCARD:
        icon_id = is_valid ? IMAGE_DRM_UNKNOWN_UNLOCK_ICON: IMAGE_DRM_UNKNOWN_LOCK_ICON;
        break;
#endif
#ifdef EBOOK_SUPPORT
    case MMIFMM_FILE_TYPE_EBOOK:
        icon_id = is_valid ? IMAGE_DRM_TEXT_UNLOCK_ICON: IMAGE_DRM_TEXT_LOCK_ICON;
        break;
#endif
        
#ifdef JAVA_SUPPORT
    case MMIFMM_FILE_TYPE_JAVA:
        icon_id = is_valid ? IMAGE_DRM_JAVA_UNLOCK_ICON: IMAGE_DRM_JAVA_LOCK_ICON;
        break;
#endif

     default:
        icon_id = is_valid ? IMAGE_DRM_UNKNOWN_UNLOCK_ICON: IMAGE_DRM_UNKNOWN_LOCK_ICON;
         break;
     }
    
    SFS_CloseFile(drm_handle);
    
    return icon_id;
}

/*****************************************************************************/
//  Description : set the select drm file limit type
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 由用户首先设置一个查找文件窗口的后缀形式，
//  然后针对DRM如果还有一些限制，则会在选中后根据用户的相关设置进行提示
//  注意本函数跟在选择窗口的后面才会起作用，如MMIAPIFMM_OpenSelectMovieWin等
//  这函数只针对DRM起作用 值设置成:MMIFMM_DRMFILE_LIMIT_COUNT，
//  MMIFMM_DRMFILE_LIMIT_INTERVAL, MMIFMM_DRMFILE_LIMIT_INTERVAL and so on
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SetSelDrmLimitValue(MMIFMM_DRM_LIMIT_TYPE_T limit_value)
{    
    if (PNULL == s_select_win_param_ptr)
    {
        //SCI_TRACE_LOW:"[MMIDRM] MMIAPIFMM_SetSelDrmLimitValue: invalid param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_6033_112_2_18_2_20_18_265,(uint8*)"");
        return FALSE;
    }

    s_select_win_param_ptr->drm_limit.cons_mode_limit 
        = limit_value.cons_mode_limit;
    s_select_win_param_ptr->drm_limit.method_level_limit 
        = limit_value.method_level_limit;    
	s_select_win_param_ptr->drm_limit.is_rights_valid
        = limit_value.is_rights_valid;

    //SCI_TRACE_LOW:"[MMIDRM] MMIAPIFMM_SetSelDrmLimitValue: cons_mode_limit = %x, method_level_limit = %x, is_rights_valid=%d!"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_6047_112_2_18_2_20_18_266,(uint8*)"ddd", s_select_win_param_ptr->drm_limit.cons_mode_limit, s_select_win_param_ptr->drm_limit.method_level_limit,s_select_win_param_ptr->drm_limit.is_rights_valid);

    return TRUE;
}

/*****************************************************************************/
//  Description : set the select drm file constraint
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN IsSelDrmConstraintFile(
    uint16 *filename, 
    uint16 name_len, 
    MMIFMM_DRM_LIMIT_TYPE_T limit_value
)
{
    MMIFILE_HANDLE file_handle = SFS_INVALID_HANDLE;
    DRM_RIGHTS_T drm_rights = {0};
    MMIDRMFILE_ERROR_E file_error = SFS_NO_ERROR;
    BOOLEAN is_ret = FALSE;
    DRM_PERMISSION_MODE_E drm_permission = DRM_PERMISSION_NONE;
    BOOLEAN is_valid_check = FALSE;
    
    if (PNULL == filename
        || 0 == name_len
        || (0 == limit_value.cons_mode_limit
        && 0 == limit_value.method_level_limit))
    {
        //SCI_TRACE_LOW:"[MMIDRM] IsSelDrmConstraintFile: invalid param!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_6074_112_2_18_2_20_18_267,(uint8*)"");
        return FALSE;
    }

    if (0 != limit_value.cons_mode_limit
        || 0 != limit_value.method_level_limit)
    {
        file_handle = MMIAPIFMM_CreateFile(filename, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, NULL, NULL);

        if (SFS_INVALID_HANDLE != file_handle)
        {
            if (MMIAPIDRM_IsDRMFile(file_handle, PNULL))
            {
                //优先检查是否需要版权有效
                if (limit_value.is_rights_valid)
                {
                    drm_permission = MMIAPIDRM_GetFilePermission(file_handle, PNULL);
                    is_valid_check = MMIAPIDRM_IsRightsValid(file_handle, PNULL, TRUE, drm_permission);
                }

                //需要版权检查，而且版权无效，则立马返回
                if (limit_value.is_rights_valid && !is_valid_check)
                {
                    is_ret = TRUE;
                }
                else
                {                
                file_error = MMIAPIDRM_GetRightsinfo(file_handle, PNULL, &drm_rights);
                if (SFS_NO_ERROR == file_error)
                {
                    //先检查是否消费类型的限制
                    if (0 != limit_value.cons_mode_limit)
                    {
                        if (0 != (drm_rights.constraint.type & DRM_CONSTRAINT_COUNT)	/*lint !e655*/
                        && (0 != (limit_value.cons_mode_limit & MMIFMM_DRM_CONS_LIMIT_COUNT)))
                        {
                            is_ret = TRUE;
                        }
                        else if(0 != (drm_rights.constraint.type & DRM_CONSTRAINT_INTERVAL)	/*lint !e655*/
                            && (0 != (limit_value.cons_mode_limit & MMIFMM_DRM_CONS_LIMIT_INTERVAL)))
                        {
                            is_ret = TRUE;
                        }
                        else if(0 != (drm_rights.constraint.type & DRM_CONSTRAINT_DATETIME)	/*lint !e655*/
                            && (0 != (limit_value.cons_mode_limit & MMIFMM_DRM_CONS_LIMIT_DATETIME)))
                        {
                            is_ret = TRUE;
                        }
                        else
                        {
                            is_ret = FALSE;
                        }
                    }

                    //如果消费类型不限，则接着检查是否转发类型限制
                    if (!is_ret && 0 != limit_value.method_level_limit)
                    {
                        if (0 != (drm_rights.method_level & DRM_LEVEL_FL)	/*lint !e655*/
                        && (0 != (limit_value.method_level_limit & MMIFMM_DRM_METHOD_LIMIT_FL)))
                        {
                            is_ret = TRUE;
                        }
                        else if(0 != (drm_rights.method_level & DRM_LEVEL_CD)	/*lint !e655*/
                            && (0 != (limit_value.method_level_limit & MMIFMM_DRM_METHOD_LIMIT_CD)))
                        {
                            is_ret = TRUE;
                        }
                        else if(0 != (drm_rights.method_level & DRM_LEVEL_SD)	/*lint !e655*/
                            && (0 != (limit_value.method_level_limit & MMIFMM_DRM_METHOD_LIMIT_SD)))
                        {
                            is_ret = TRUE;
                        }
                        else
                        {
                            is_ret = FALSE;
                        }
                    }
                }
                else
                {
                    //是DRM文件，但是又取不到版权的非法文件，则一样不让往下走
                    is_ret = TRUE;
                }
            }
            }
            else
            {
                //非drm文件
                is_ret = FALSE;
            }
        }
            
        MMIAPIFMM_CloseFile(file_handle);
    }
    else
    {
        //无任何受限
        //SCI_TRACE_LOW:"[MMIDRM] IsSelDrmConstraintFile: no any limit value!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIFMM_6171_112_2_18_2_20_18_268,(uint8*)"");
        is_ret = FALSE;
    }

    return is_ret;    
}

/*****************************************************************************/
//  Description : can this drm file execute
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 本函数用于更新选择文件夹中的图标
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_UpdateSelectListIconData(
    wchar *file_path_ptr,
    uint16 file_path_len
)
{
    BOOLEAN is_ret = FALSE;
    uint16 path_len = MMIFILE_FILE_NAME_MAX_LEN;
    wchar *new_path_ptr = PNULL;
    uint16 cur_index = 0;

    if (PNULL != file_path_ptr && 0 != file_path_len)
    {
        cur_index = MMIFMM_GetCurrentItemIndex(MMIFMM_OPEN_FILE_BYLAYER_LIST_CTRL_ID);
        new_path_ptr = SCI_ALLOC_APPZ((path_len+1)*sizeof(wchar));

        MMIAPIFMM_SplitFullPathExt(file_path_ptr, file_path_len, 
            new_path_ptr, &path_len, PNULL, PNULL, PNULL, PNULL);
        
        //如果是无效的，则需要提示guilist去刷新本行   
        MMIAPIFMM_UpdateListIconData(MMIFMM_OPEN_FILE_BYLAYER_LIST_CTRL_ID, cur_index, new_path_ptr, file_path_ptr);

        SCI_FREE(new_path_ptr);

        is_ret = TRUE;
    }
    else
    {
        is_ret = FALSE;
    }

    return is_ret;
}

#endif

/*****************************************************************************/
// Description : 是否选中的是同一种文件类型文件
// Global resource dependence : 
// Author: renyi.hu
// Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_IsSelectedSameFileType(MMIFMM_DATA_INFO_T    *list_data_ptr,
                                                MMIFMM_FILE_TYPE_E     type
                                             )
{
    int i=0;
    int num=list_data_ptr->file_num+list_data_ptr->folder_num;
    BOOLEAN result=FALSE;
    for(i=0;i<num;i++)
    {
        if((MMIFMM_FILE_STATE_CHECKED==list_data_ptr->data[i]->state))
        {
            if(type==list_data_ptr->data[i]->type)
            {
                result=TRUE;
            }
            else
            {
                result=FALSE;
                break;
            }
        }
    }

    return result;
}
/*****************************************************************************/
// Description : 给选中的文件打勾
// Global resource dependence : 
// Author: juan.wu
// Note: 
/*****************************************************************************/
PUBLIC void MMIFMM_DrawTickedOnFile(uint16 tick_file_index)
{
    FILEARRAY_DATA_T    file_data = {0};
    uint16 suffix_wstr[MMIFMM_FILE_SUFFIX_MAX_LEN + 1]        = {0};
    uint16 suffix_len                                = MMIFMM_FILE_SUFFIX_MAX_LEN;
    MMIFMM_FILE_TYPE_E file_type                     = MMIFMM_FILE_TYPE_NORMAL;

    if(PNULL == s_select_win_param_ptr)
    {
        return;
    }
    
    MMIAPIFMM_SplitFileName(s_select_win_param_ptr->ticked_file, s_select_win_param_ptr->ticked_file_len, PNULL, PNULL, suffix_wstr, &suffix_len);
    file_type = MMIAPIFMM_ConvertFileType(suffix_wstr, suffix_len);
    if(MMIFMM_FILE_TYPE_PICTURE == file_type)
    {
        return;
    }
    else if(0 < s_select_win_param_ptr->ticked_file_len)
    {
        //if(MSG_CTL_LIST_NEED_ITEM_DATA == msg_id)
        //{
            //GUILIST_NEED_ITEM_DATA_T    *need_item_data_ptr = (GUILIST_NEED_ITEM_DATA_T*)param;
            uint16                      item_index = 0;
            GUILIST_ITEM_DATA_T         item_data = {0};/*lint !e64*/
            wchar           file_name[FILEARRAY_FILENAME_MAX_LEN+1] = {0};
			wchar           size_wstr[GUILIST_STRING_MAX_NUM + 1] = {0};
            uint16          file_name_len = 0;

            item_index = tick_file_index;

            item_data.softkey_id[0] = STXT_SELECT;
            item_data.softkey_id[2] = STXT_RETURN;

			if(MMIFMM_GetOpenFileWinFile(item_index, &file_data))
			{
				if (0 == MMIAPICOM_CompareTwoWstr(s_select_win_param_ptr->ticked_file, 
					s_select_win_param_ptr->ticked_file_len, 
					file_data.filename, file_data.name_len))
				{
					item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
					item_data.item_content[0].item_data.image_id = IMAGE_CHECK_SELECTED_ICON;
					
					MMIAPIFMM_SplitFullPath(file_data.filename, file_data.name_len,
						NULL, NULL, NULL, NULL,
						file_name, &file_name_len);
					
					item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
					item_data.item_content[1].item_data.text_buffer.wstr_len = file_name_len;
					
					item_data.item_content[1].item_data.text_buffer.wstr_ptr = file_name;
					if(FILEARRAY_TYPE_FILE == file_data.type )
					{
						item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
						MMIAPIFMM_GetFileSizeString(file_data.size, 
							size_wstr,
							MMIFMM_SIZE_STR_LEN,
							TRUE);
						item_data.item_content[2].item_data.text_buffer.wstr_ptr = size_wstr;
						item_data.item_content[2].item_data.text_buffer.wstr_len = (uint16)MMIAPICOM_Wstrlen(size_wstr);
                        //增加预览 icon
                        #ifdef MMI_PDA_SUPPORT
                        item_data.item_content[3].item_data_type = GUIITEM_DATA_IMAGE_ID;
                        item_data.item_content[3].item_data.image_id = IMAGE_LIST_TITLE_ARROW_RIGHT_UP;
                        #endif
                    }

					GUILIST_SetItemData(MMIFMM_OPEN_FILE_BYLAYER_LIST_CTRL_ID, &item_data, item_index );
				}
			}
    //    }
    }
}
/*****************************************************************************/
// Description :是否正在预览铃声
// Global resource dependence : 
// Author: juan.wu
// Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_IsPreviewMusic(void)
{
    return s_is_music_preview;
}
