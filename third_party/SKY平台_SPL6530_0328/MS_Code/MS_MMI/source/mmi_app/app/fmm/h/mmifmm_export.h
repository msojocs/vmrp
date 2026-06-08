/*****************************************************************************
** File Name:      mmifmm_export.h                                                  *
** Author:                                                                   *
** Date:           06/04/2007                                                *
** Copyright:      2003 Spreadtrum, Incoporated. All Rights Reserved.        *
** Description:     file manager modue                                                                              *
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 04/2007       Robert           Create
******************************************************************************/
/*******************************************************************************
** Full Path Format: 
**      full path = device + ':\' + directory + '\' + filename
**      eg:  "D:\picture\abc.jpg"
**      "D" is device name of Udisk, SD card is represented by "E"
**      "picture" is directory name
**      "abc.jpg" is filename
**      use ":", "\" to seperate device, directory, filename (don't use '/')
**
** 注:
** 1. 文件名长度、文件夹名长度、设备名长度、全路径名长度都以unicode个数
******************************************************************************/

#ifndef _MMIFMM_EXPORT_H_
#define _MMIFMM_EXPORT_H_ 

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "sfs.h"
#include "mmk_type.h"
#include "guianim.h"
#include "mime_type.h"
//#include "mmi_filetask.h"
#include "mmi_custom_define.h"
#include "mmi_common.h"
#include "ffs.h"
#if defined(DRM_SUPPORT)
#include "drm_api.h"
#endif
#include "mmi_filemgr.h"
#include "mmifilearray_export.h"
//#include "mmiset.h"
#include "mmiset_export.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define MMIFMM_FULL_FILENAME_LEN            MMIFMM_FILE_FILE_NAME_MAX_LEN   // 包括设备名：\路径名\文件名
#define MMIFMM_PATHNAME_LEN                 MMIFMM_FILE_FULL_PATH_MAX_LEN   // 路径的最大长度
#define MMIFMM_FILENAME_LEN                 MMIFMM_FILE_FILE_NAME_MAX_LEN   // 文件名的最大长度
#define MMIFMM_PATH_NAME_OFFSET             3
        
#define MMIFMM_ELLIPSIS                     (g_mmifmm_ellipsis_wstr)//"\x00\x2e\x00\x2e\x00\x5c"    //"..\"
#define MMIFMM_ELLIPSIS_LEN                 3

#define MMIFMM_ELLIPSIS_3                   (g_mmifmm_ellipsis_3_wstr)//"\x00\x2e\x00\x2e\x00\x2e"    //"..."
#define MMIFMM_ELLIPSIS_3_LEN               3

#define MMIFMM_CSK2_DEFAULT_DIR             (g_mmifmm_csk2_default_dir_wstr)//"\x00\x43\x00\x73\x00\x74\x00\x61\x00\x72"          // Cstar 存储的目录,"Cstar"
#define MMIFMM_CSK2_DEFAULT_DIR_LEN         5       

#define MMIFMM_PLAY_RING_DEFAULT_VOL   3        

#define MMIFMM_FILE_NUM                     MMIFMM_FILE_MAX_NUM     // 内存中保存的数量

#define MMIFMM_PATH_DEPTH                   MMIFMM_PATH_MAX_DEPTH      // 可访问路径的深度

#define MMIFMM_STORE_PATH_NUM               MMIFMM_STORE_PATH_MAX_NUM      // 保存路径的数量
        
#define MMIMULTIM_FILEDATA_BUFFER_SIZE      (200*1024)      // 图片读取的最大buffer size

#define MMIMULTIM_WBMP_BUF_MAX_SIZE         (200*1024)  //Wbmp图片解压缩buf最大值

#ifdef CMCC_UI_STYLE
//"Picture"
#define MMIMULTIM_DIR_PICTURE               (g_mmimultim_picture_dir_wstr)//"\x00\x50\x00\x69\x00\x63\x00\x74\x00\x75\x00\x72\x00\x65\x00\x00"
#define MMIMULTIM_DIR_PIC_LEN               7

//"Photos"
#define MMIMULTIM_DIR_PHOTOS               (g_mmimultim_photos_dir_wstr)//"\x00\x50\x00\x69\x00\x63\x00\x74\x00\x75\x00\x72\x00\x65\x00\x00"
#define MMIMULTIM_DIR_PHOTOS_LEN               6

#else
//"Picture"
#define MMIMULTIM_DIR_PICTURE               (g_mmimultim_picture_dir_wstr)//"\x00\x50\x00\x69\x00\x63\x00\x74\x00\x75\x00\x72\x00\x65\x00\x00"
#define MMIMULTIM_DIR_PIC_LEN               6
#endif

//"Music"
#define MMIMULTIM_DIR_MUSIC                 (g_mmimultim_music_dir_wstr)//"\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x00"
#define MMIMULTIM_DIR_MUSIC_LEN             5
//"Movie"
#define MMIMULTIM_DIR_MOVIE                 (g_mmimultim_movie_dir_wstr)//"\x00\x4d\x00\x6f\x00\x76\x00\x69\x00\x65\x00\x00"
#define MMIMULTIM_DIR_MOVIE_LEN             5

#ifdef MMI_KING_MOVIE_SUPPORT
//"KingMovie"
#define MMIMULTIM_DIR_KINGMOVIE             (g_mmimultim_kingmovie_dir_wstr)
#define MMIMULTIM_DIR_KINGMOVIE_LEN         9
#endif

//vcard
#if defined MMI_VCARD_SUPPORT
#define MMIMULTIM_DIR_VCARD                 (g_mmimultim_vcard_dir_wstr)
#endif
#ifdef MMI_VCALENDAR_SUPPORT
//vcalendar
#define MMIMULTIM_DIR_VCALENDAR 				(g_mmimultim_vcalendar_dir_wstr)
#endif

//ebook
#define MMIMULTIM_DIR_EBOOK                 (g_mmimultim_ebook_dir_wstr)

//filearray
//ebook
#define MMIMULTIM_DIR_FILEARRAY               (g_file_array_dir_wstr)
//"PB"
#define MMIMULTIM_DIR_PB                    (g_mmimultim_pb_dir_wstr)//"\x00\x50\x00\x42\x00\x00"
#define MMIMULTIM_DIR_PB_LEN                2
//"Alarm"
#define MMIMULTIM_DIR_ALARM                   (g_mmimultim_alarm_dir_wstr)//"\x00\x50\x00\x42\x00\x00"
#define MMIMULTIM_DIR_ALARM_LEN                5
#define MMIMULTIM_OTHER_DEFAULT_DIR          (g_other_dir_default)   
#define MMIMULTIM_OTHER_DEFAULT_DIR_LEN     5

#define MMIMULTIM_DIR_JAVA            (g_mmimultim_java_dir_wstr)   
#define MMMIMULTIM_DIR_JAVA_LEN     4

//"Font"
#define MMIMULTIM_DIR_FONT                 (g_mmimultim_font_dir_wstr)//"\x00\x4d\x00\x6f\x00\x76\x00\x69\x00\x65\x00\x00"
#define MMIMULTIM_DIR_FONT_LEN             4

#define MMIMULTIM_DIR_SYSTEM_CHAR           'S', 'y', 's', 't', 'e', 'm'
#define MMIMULTIM_DIR_SYSTEM                (g_mmimultim_dir_system)   
#define MMIMULTIM_DIR_SYSTEM_LEN            6
        
#define MMIMULTIM_READ_PICTURE_FILE_MAX     (MMIDC_MAX_PHOTO_SIZE)  //please refer to function AllocPictureDataBuf (BLOCK_MEM_POOL_DC)
#define MMIMULTIM_BMP_PREVIEW_MAX_SIZE      (1024*100)              //please refer to GUI
#define MMIMULTIM_JPG_PREVIEW_MAX_SIZE      (MMIDC_MAX_PHOTO_SIZE)  //please refer to MMIDC_MAX_PHOTO_SIZE(mmidc_export.h)
#define MMIMULTIM_MIDI_PLAY_MAX_SIZE        (1024*300)              //max size of midi play, because when playing midi, it first read all file data,
                                                                    //while playing mp3(wave or aac), it read a little data once
//UDISK represented by
//SD Card Type: E:, F:, G:...
#ifdef DUAL_TCARD_SUPPORT
	#define MMI_SDCARD_MAX_NUM 2
#else
	#define MMI_SDCARD_MAX_NUM 1
#endif
#define MMIFMM_DEVICE_NAME_LEN              1
        //":"
#define MMIFILE_COLON                       0x003a
        //"\"
#define MMIFILE_SLASH                       0x005c
        //"."
#define MMIFILE_DOT                         0x002e
        //"*"
#define MMIFILE_STAR                        0x002a
        //" "
#define MMIFILE_SPACE                       0x0020
        //"C"
#define MMIFILE_C_LETTER                    0x0043
        //"D"
#define MMIFILE_D_LETTER                    0x0044
        //"E"
#define MMIFILE_E_LETTER                    0x0045
        //"F"
#define MMIFILE_F_LETTER                    0x0046
        //"G"
#define MMIFILE_G_LETTER                    0x0047
        //"H"
#define MMIFILE_H_LETTER                    0x0048

#ifdef MAINLCD_SIZE_176X220
    #define MMIFMM_WIN_TITLE_MAX_LEN            7
#else
#ifdef MMI_FMM_TITLE_BUTTON_SUPPORT
    #define MMIFMM_WIN_TITLE_MAX_LEN            15
#else
    #define MMIFMM_WIN_TITLE_MAX_LEN            8
#endif
#endif
#define MMIFMM_TITLE_LABEL_MARGIN       24//udisk tab 窗口的前后缩进值

//#if 1
//文件存储位置分类
typedef enum
{
 	MMIFMM_STORE_TYPE_FIXED,     //不能转储的文件，固定存储在U盘或SD卡上
// 	MMIFMM_STORE_TYPE_PORTABLE,  //可以转储的文件，优先存储在U盘，存满存到SD卡上
    MMIFMM_STORE_TYPE_PREFER,    //指定优先存储位置

    MMIFMM_STORE_TYPE_MAX
}MMIFMM_STORE_TYPE_E;

//图片作为墙纸,记录其详细信息
typedef struct
{
    wchar          full_path_name[MMIFILE_FULL_PATH_MAX_LEN+1];    //wallpaper name
    uint16          full_path_len;
    SFS_DATE_T	    wallpaper_modify_date;
    SFS_TIME_T	    wallpaper_modify_time;
} MMIFMM_AS_WALLPAPER_INFO;

typedef MMIFMM_AS_WALLPAPER_INFO        MMIMULTIM_AS_WALLPAPER_INFO;

        //文件类型
typedef enum
{
    MMIMULTIM_FILE_TYPE_NULL,

    MMIMULTIM_FILE_TYPE_PIC,
    MMIMULTIM_FILE_TYPE_MUSIC,
    MMIMULTIM_FILE_TYPE_MOVIE,
    
    MMIMULTIM_FILE_TYPE_MAX
} MMIMULTIM_FILE_TYPE_E;
//typedef MMIFMM_FILE_TYPE_E              MMIMULTIM_FILE_TYPE_E;

//铃声设置类型
typedef enum
{
    MMIMULTIM_SET_RING_NULL,
    MMIMULTIM_SET_CALL_RING,        //来电铃声
    MMIMULTIM_SET_MSG_RING,         //信息铃声
    MMIMULTIM_SET_ALARM_RING,       //闹钟铃声
    MMIMULTIM_SET_RING_MAX
} MMIFMM_SET_RING_TYPE_E;
typedef MMIFMM_SET_RING_TYPE_E          MMIMULTIM_SET_RING_TYPE_E;

//file information
typedef struct
{
	wchar			    file_name[MMIFILE_FILE_NAME_MAX_LEN+1];
	uint16				file_name_len;
    wchar				device_name[MMIFILE_DEVICE_NAME_MAX_LEN+1];
    uint16				device_name_len;
    uint32				create_time;
    uint32			    file_size;
} MMIMULTIM_FILE_INFO_T;

//disk format
typedef enum
{
    MMIFILE_FORMAT_NULL,

    MMIFILE_FORMAT_FAT12,
    MMIFILE_FORMAT_FAT16,
    MMIFILE_FORMAT_FAT32,
    MMIFILE_FORMAT_AUTO,  
    MMIFILE_FORMAT_OTHER,
    MMIFILE_FORMAT_MAX
}MMIFILE_DEVICE_FORMAT_E;
#if 0
// 设备类型
typedef enum
{
    MMI_DEVICE_SYSTEM,    //C盘，不显示给用户，只供应用保存配置文件
    MMI_DEVICE_UDISK,    // U盘
    MMI_DEVICE_SDCARD,   // 卡1
	MMI_DEVICE_SDCARD_1, //卡2
    MMI_DEVICE_NUM = MMI_SDCARD_MAX_NUM + 2
} MMIFILE_DEVICE_E;
#endif
//file information
typedef struct
{
    wchar               file_name[MMIFILE_FILE_NAME_MAX_LEN+1];
    uint16              file_name_len;
    uint32              create_time;
    uint32              file_size;
    wchar               device_name[MMIFILE_DEVICE_NAME_MAX_LEN+1];
    uint16              device_name_len;
    wchar               dir_name[MMIFILE_DIR_NAME_MAX_LEN+1];
    uint16              dir_name_len;
} MMIFILE_FILE_INFO_T;

//this is move from mmi_filemgr.h
extern const wchar g_file_device_udisk[];
extern const wchar g_file_device_sdcard[];
extern const wchar g_file_device_sysname[];

#ifdef CMCC_UI_STYLE
extern wchar g_mmimultim_photos_dir_wstr[];
#endif
extern wchar g_mmimultim_picture_dir_wstr[];
extern wchar g_mmimultim_music_dir_wstr[];
extern wchar g_mmimultim_movie_dir_wstr[];
#ifdef MMI_KING_MOVIE_SUPPORT
extern wchar g_mmimultim_kingmovie_dir_wstr[];
#endif
extern wchar g_mmimultim_pb_dir_wstr[];
extern wchar g_other_dir_default[];
extern wchar g_mmimultim_dir_system[];
extern wchar g_mmimultim_alarm_dir_wstr[];
extern wchar g_mmimultim_java_dir_wstr[];
extern wchar g_mmimultim_font_dir_wstr[];
extern wchar g_mmimultim_vcard_dir_wstr[];
extern wchar g_mmimultim_ebook_dir_wstr[];
extern wchar g_file_array_dir_wstr[];
#ifdef MMI_VCALENDAR_SUPPORT
extern wchar g_mmimultim_vcalendar_dir_wstr[];
#endif
//#endif
/**--------------------------------------------------------------------------*
**                         TYPE AND CONSTANT                                *
**--------------------------------------------------------------------------*/



// 图片类型
typedef enum
{
    MMIFMM_PICTURE_TYPE_BMP,
    MMIFMM_PICTURE_TYPE_WBMP,
    MMIFMM_PICTURE_TYPE_JPG,
    MMIFMM_PICTURE_TYPE_GIF,
    MMIFMM_PICTURE_TYPE_PNG,
    MMIFMM_PICTURE_TYPE_UNKNOWN
} MMIFMM_PICTURE_TYPE_E;

// 资源管理器的窗口的状态类型
typedef enum 
{
    MMIFMM_FMI_STATE_BROWSER,       // 浏览状态
    MMIFMM_FMI_STATE_MARK,          // 标记状态
    MMIFMM_FMI_STATE_LOOKUP_PATH,   // 查找路径状态
    MMIFMM_FMI_STATE_LOOKUP_FILE,   // 查找文件状态
    MMIFMM_FMI_STATE_UNKNOWN        // 不确定状态
} MMIFMM_FMI_STATE_E;

// 排序类型
typedef enum
{
    MMIFMM_SORT_TYPE_NAME,  // 按名称
    MMIFMM_SORT_TYPE_TIME,  // 按时间
    MMIFMM_SORT_TYPE_SIZE,  // 按大小
    MMIFMM_SORT_TYPE_TYPE   // 按类型
} MMIFMM_SORT_TYPE_E;

// 文件夹标志
typedef enum
{
    MMIFMM_FOLDER_TYPE_NORMAL,      // 普通文件夹
    MMIFMM_FOLDER_TYPE_PICTURE,     // 图片文件夹
    MMIFMM_FOLDER_TYPE_MUSIC,       // 音乐文件夹
    MMIFMM_FOLDER_TYPE_MOVIE,       // 影像文件夹
#ifdef MMI_KING_MOVIE_SUPPORT
    MMIFMM_FOLDER_TYPE_KINGMOVIE,   // 电影王影像文件夹
#endif
    MMIFMM_FOLDER_TYPE_EBOOK,       // 电子书文件夹
    MMIFMM_FOLDER_TYPE_JAVA,        // Java 文件夹
	MMIFMM_FOLDER_TYPE_VCARD,        // Java 文件夹
    MMIFMM_FOLDER_TYPE_NUM     // 保留文件夹，不显示
}MMIFMM_FOLDER_TYPE_E;

// 文件是否选中状态
typedef enum
{
    MMIFMM_FILE_STATE_UNCHECKED,    // 未选中
    MMIFMM_FILE_STATE_CHECKED,      // 选中
    MMIFMM_FILE_STATE_ALL_CHECKED   // 全选中
} MMIFMM_FILE_STATE_E;

// 数据类型
typedef enum
{
    MMIFMM_READ_TYPE_ONLY_FILE, // 只读文件
    MMIFMM_READ_TYPE_ONLY_DIR,  // 只读目录
    MMIFMM_READ_TYPE_FILE_DIR,  // 同时读文件和目录
    MMIFMM_READ_TYPE_FILE_SUBDIR// 只读文件，包括递归查找子目录中文件
} MMIFMM_READ_TYPE_E;

//异步复制和删除过程的状态
typedef enum
{
    MMIFMM_COPYDEL_NON_STATE,   //无操作状态
    MMIFMM_COPY_INIT_STATE,     //复制初始化状态
    MMIFMM_COPY_QUERY_STATE,    //查询当前复制状态，决定下步操作
    MMIFMM_COPY_START_STATE,    //复制开始
    MMIFMM_COPY_READ_STATE,     //异步读取状态
    MMIFMM_COPY_WRITE_STATE,    //异步写文件状态
    MMIFMM_COPY_PROGRESS_STATE, //异步复制处理状态
    MMIFMM_COPY_END_STATE,      //复制结束状态
    MMIFMM_DEL_INIT_STATE,      //删除初始化状态
    MMIFMM_DEL_QUERY_STATE,     //删除查询状态，决定下一步操作
    MMIFMM_DEL_PROGRESS_STATE,  //删除正在进行的状态
    MMIFMM_DEL_END_STATE        //删除结束状态
} MMIFMM_COPYDEL_STATE_E;
// 文件类型
typedef enum
{
    MMIFMM_FILE_TYPE_NORMAL,    // 普通文件
    MMIFMM_FILE_TYPE_PICTURE,   // 图片文件
    MMIFMM_FILE_TYPE_MUSIC,     // 音乐文件
    MMIFMM_FILE_TYPE_MOVIE,     // 影像文件
    MMIFMM_FILE_TYPE_EBOOK,     // 电子书
#ifdef  MMI_VCARD_SUPPORT
    MMIFMM_FILE_TYPE_VCARD,     // VCARD
#endif
#ifdef MMI_VCALENDAR_SUPPORT
    MMIFMM_FILE_TYPE_VCALENDAR, // VCALENDAR
#endif
    MMIFMM_FILE_TYPE_SZIP,      // COM压缩文件
    MMIFMM_FILE_TYPE_JAVA,     // JAVA
	MMIFMM_FILE_TYPE_THEME,		// Theme
    MMIFMM_FILE_TYPE_FONT,     // Font
#ifdef DRM_SUPPORT 
    MMIFMM_FILE_TYPE_DRM,       //DRM 文件
#endif        
#ifdef MRAPP_SUPPORT
    MMIFMM_FILE_TYPE_MRP,       //MRP 文件
#endif
    MMIFMM_FILE_TYPE_BROWSER,   // NF浏览器支持的文件
    MMIFMM_FILE_TYPE_MTV,       //MTV类型
    MMIFMM_FILE_TYPE_FOLDER,    // 文件夹	
    MMIFMM_FILE_TYPE_NUM        // 文件类型数目
} MMIFMM_FILE_TYPE_E;

typedef struct 
{
    wchar                       file_name_ptr[MMIFILE_FULL_PATH_MAX_LEN+1];
    uint16                      file_name_len;  //wstr_len of uint8
    uint32                      file_size;
    MMIFMM_FILE_TYPE_E          file_type;      //picture or music or movie
    GUIANIM_TYPE_E              pic_type;       //if file_type is picutre, this variable is used
    MMISRVAUD_RING_FMT_E             music_type;     //if file_type is music, this variable is used
    MMICOM_VIDEO_TYPE_E             movie_type;    
    uint8                       *data_buf_ptr;
    uint32                      data_buf_offset;
}MMIFMM_SELECT_RETURN_T;

typedef MMIFMM_SELECT_RETURN_T          MULTIM_SELECT_RETURN_T;

// 文件详情数据
typedef struct mmifmm_detail_data_tag
{
    uint32              time;                           // 修改时间 
    uint32              file_size;                      // 文件大小             
    MMIFMM_FILE_TYPE_E  type;                           // 文件类型，
    uint16              filename_len;                   // 文件名长度，字符数
    wchar               filename[MMIFMM_FILENAME_LEN+1];// 文件名
    uint16              resolution_x;                   // 水平分辨率，用于图片
    uint16              resolution_y;                   // 垂直分辨率，用于图片
    uint32              file_num;                       // 文件数目
    uint32              folder_num;                     // 目录数目
#ifdef DRM_SUPPORT
    DRM_RIGHTS_T        drm_rights;                     //drm 版权信息
#endif

} MMIFMM_DETAIL_DATA_T;

// 文件信息
typedef struct mmifmm_file_info_tag
{

    uint32                  time;                                   // 访问时间
    uint32                  file_size;                              // 文件大小       
    uint16                  filename_len;                           // 文件名长度，字符数
    uint16                  pad;                                    // 填充
    wchar                   *filename_ptr;//[MMIFMM_FULL_FILENAME_LEN +1];  // 文件名
    MMIFMM_FILE_STATE_E     state;                                  // 是否选中，用于标记状态；
    MMIFMM_FILE_TYPE_E      type;                                   // 文件类型，
} MMIFMM_FILE_INFO_T;

// 保存路径的数据
typedef struct mmifmm_path_info_tag
{
    uint16      path_depth;                         // 路径深度
    uint16      pathname_len;                       // 路径名长度，字符数
    uint32      reserved_pos;                       // 保留的位置
    wchar       pathname[MMIFMM_PATHNAME_LEN + 1];   // 文件名
} MMIFMM_PATH_INFO_T;

typedef struct mmifmm_path_data_info_tag
{
    uint16                  num;            // 保存路径信息的个数
    uint16                  path_depth;     // 路径深度
    uint16                  begin_pos;      // 开始位置 , 用于保存路径
    uint16                  end_pos;        // 结束位置
    MMIFMM_PATH_INFO_T      path_info[MMIFMM_STORE_PATH_NUM];
} MMIFMM_PATH_DATA_INFO_T;

// 数据信息
typedef struct mmifmm_data_info_tag
{
    MMIFMM_FMI_STATE_E      state;                              // 窗口状态
    MMIFMM_FOLDER_TYPE_E    folder;                             // 文件夹类型
    MMIFMM_SORT_TYPE_E      sort;                               // 排序类型
    MMIFMM_FILE_STATE_E     checked;                            // 是否有选中
    MMIFILE_DEVICE_E        device;                             // 设备
    uint8                   path_is_valid;                      // 路径是否有效
    uint16                  path_depth;                         // 路径深度
    uint16                  pathname_len;                       // 路径字符长度
    wchar                   pathname[MMIFMM_PATHNAME_LEN+1];    // 始终保存当前路径
    uint16                  file_num;                           // 文件数据的数量
    uint16                  folder_num;                         // 文件夹数据的数量
    uint16                  current_pos;                        // 当前的位置，用于分屏装载
    uint16                  mark_num;                           // 标记数
    MMIFMM_FILE_INFO_T      *data[MMIFMM_FILE_NUM];             // 根据path_is_valid是否大于0，确认data中文件是否包含全路径
} MMIFMM_DATA_INFO_T;

// 路径参数
typedef struct  mmifmm_path_param_tag
{
    uint32      win_id;         //[IN] 通知的窗口
    uint32      msg_id;         //[IN] 通知的消息
    wchar       *full_path_ptr; //[OUT] 全路径 
} MMIFMM_PATH_PARAM_T;

typedef struct mmifmm_findfile_win_param_tag
{
    uint16              *find_path_ptr;  //查找全路径(目录)
    MMIFMM_DATA_INFO_T  *result_data_ptr;  //查找结果保存位置
    uint16              max_result_file_num; //查找结果保存最大文件数
    uint32              win_id;  //通知窗口
    uint32              msg_id;  //通知消息
} MMIFMM_FINDFILE_WIN_PARAM_T;

typedef struct mmifmm_createfolder_win_param_tag
{
    wchar       *full_path_ptr;  //当前目录全路径
    uint16      new_folder_max_len; //新建目录最大名字长度,ucs2 len
    wchar       *new_folder_ptr;
    uint16      *new_folder_len_ptr;
    uint32      win_id;  //通知窗口
    uint32      msg_id;  //通知消息
} MMIFMM_CREATEFOLDER_WIN_PARAM_T;

//记录复制和删除操作的位置，以及需要保存的一些静态变量
typedef struct  mmifmm_copy_del_path_data_info_tag
{
    uint16                  path_depth;     //记录当前目录的深度
    wchar                   cur_pathname[MMIFMM_PATHNAME_LEN +1];
    wchar                   copyfile_name[MMIFMM_PATHNAME_LEN +1];  //需要复制的文件名字
    wchar                   cur_target_path[MMIFMM_PATHNAME_LEN +1];
    wchar                   copy_choose_path[MMIFMM_PATHNAME_LEN +1];  //保存用户选择路径
    wchar                   user_scr_path[MMIFMM_PATHNAME_LEN +1];  //保存用户初始路径
    MMIFMM_PATH_INFO_T      cur_path_info;
    MMIFMM_PATH_INFO_T      next_path_info;
    MMIFMM_PATH_INFO_T      target_path_info;
    uint32                  win_id;                 //通知的窗口
    uint32                  msg_id;                 //通知的消息
    MMIFMM_COPYDEL_STATE_E  copy_state;
    SFS_HANDLE              sfs_find_handle;
    SFS_FIND_DATA_T         sfs_find_data;
    SFS_OVERLAPPED_T        s_r_overlapped;
    SFS_OVERLAPPED_T        s_w_overlapped;
    uint16                  search_folder_flag;     //用来标记是否已经搜完文件夹或者文件夹已满
    uint16                  copy_or_search_state;   //记录当前是在查找还是复制的状态
    uint16                  mark_flag;              //记录是否有已经标记过的文件
    uint16                  cur_mark_num;           //当前正在操作的列表位置
    uint16                  select_file_num;        // 选择的删除或复制文件数量
    uint16                  completed_file_num;     // 完成删除或复制的文件数量
}MMIFMM_COPY_DEL_PATH_DATA_INFO_T;


typedef struct
{
    SFS_HANDLE          handle;
    SFS_ERROR_E         error;
    MMI_WIN_ID_T        win_id;   
    MMI_MESSAGE_ID_E    msg_id;    
} MMIFMM_FILE_ASYN_CALLBACK_T;


typedef struct mmifmm_rename_win_param_tag
{
    wchar       *full_path_ptr; //全路径,改名之前的名字，改名之后再保存到这里。
    MMIFMM_FILE_TYPE_E     type;      //文件类型
    uint32      win_id;         //通知窗口
    uint32      msg_id;         //通知消息
    uint32      max_file_lan;   //后续目录的最大长度
} MMIFMM_RENAME_WIN_PARAM_T;

typedef struct mmifmm_getfile_win_param_tag
{
    MMIFMM_FILE_TYPE_E  type;     // in,文件的类型
    uint32              win_id;   // in, 通知的窗口
    uint32              msg_id;     // in,通知的消息
    wchar               *full_path_ptr;  // out,全路径文件名
    uint32              max_size;
    BOOLEAN             need_raw_data;
    BOOLEAN             need_resize_data;
} MMIFMM_GETFILE_WIN_PARAM_T;

typedef void (*MMIFMM_OpenWinCallBack)(MMIFMM_DATA_INFO_T *list_data_ptr,uint32 index);

typedef struct
{
    MMI_TEXT_ID_T leftsoft_id;
    MMI_TEXT_ID_T middlesoft_id;
    MMI_TEXT_ID_T rightsoft_id;   
}MMIFMM_SOFTKEY_TEXT_T;
typedef struct
{
    MMIFMM_OpenWinCallBack callback_ok;
    MMIFMM_OpenWinCallBack callback_web;
    MMIFMM_OpenWinCallBack callback_can;
}MMIFMM_OPENWIN_CALLBACK_T;

typedef struct
{
    wchar               * path_ptr;
    uint16              path_len;
    wchar               * filename;
    uint16              file_len;
    BOOLEAN             is_need_display;
    uint32              find_suffix_type;
    BOOLEAN             privacy_protect;
    MMIFMM_SOFTKEY_TEXT_T sk_text_id;
    MMIFMM_OPENWIN_CALLBACK_T callback;
}MMIFMM_OPENWIN_INFO_T;

typedef enum
{
    MMUFMM_BT_SEND_OPT,
    MMIFMM_DELETE_OPT,
    MMIFMM_DETAIL_OPT,
    MMIFMM_SUB_MARK_OPT,
    MMIFMM_CANCEL_MARK_OPT,
    MMIFMM_MARK_ALL_OPT,
    MMIFMM_CANCEL_ALL_MARK_OPT,
    MMIFMM_RENAME_OPT,
    MMIFMM_COPY_OPT,
    MMIFMM_MAX_OPT
} MMIFMM_OPTION_TYPE_E;

typedef struct
{
	const wchar      filepath_name[MMIFMM_FULL_FILENAME_LEN+1];    //发送文件的全路径（含文件名）
	uint16           filepath_len;		    //全路径长度
	uint32		     file_size;             //文件的大小
	BOOLEAN          is_temp_file;		
}MMIFMM_BT_SENDFILE_INFO_T;

typedef struct
{
    wchar    file_name[MMIFMM_FULL_FILENAME_LEN +1];
    wchar    path_name[MMIFMM_PATHNAME_LEN + 1];
}MMIFMM_MOVE_DATA_INFO_T;

/*MMIFMM_SEARCH_FILE_INFO_T: 搜索的数据结构
getKey是一个callback函数，输出一个32位的唯一值，该值与每个文件或文件夹一一对应
提供给文件系统提高排序效率，一般来说，sfs只关注getkey的返回值，只有碰到相同的key
的文件，才去load这些文件信息，调用comp函数去进行真正的比较,
当xfind_param->sort_type == FILEARRAY_SORT_TYPE_CUST 时，这两项必须有实现，默认为PNULL*/
typedef struct
{
    FILEARRAY           file_array;     
    FILEARRAY_SORT_E    sort_type;      //排序方式
    BOOLEAN             is_recurse;   //IN:是否需要递归搜索
    MMI_WIN_ID_T        win_id;         //如果win_id和msg_id都为0，则采用同步方式搜索文件
    MMI_MESSAGE_ID_E    msg_id;
    SFS_XFIND_GETKEY    getKey;			
    SFS_XFIND_COMPARE   compareFunc;
}MMIFMM_SEARCH_FILE_INFO_T;

extern const wchar g_mmifmm_ellipsis_wstr[];
extern const wchar g_mmifmm_ellipsis_3_wstr[];
extern const wchar g_mmifmm_csk2_default_dir_wstr[];

#define FMM_FILTER_STRING_MAXLEN            240
#define FMM_FILTER_MAX_NUM                  10
#define FMM_SEARCH_FILENAME_MAXLEN          MMIFILE_FULL_PATH_MAX_LEN
#define MMIFMM_SIZE_STR_LEN                 30
#define MMIFMM_INVALID_INDEX                0xFFFFFFFF

#define MMIFMM_XFIND_SUPPORT 1
/**--------------------------------------------------------------------------*
 **                         Type Definition                                  *
 **--------------------------------------------------------------------------*/
typedef enum
{
    FMM_OPENWIN_RET_NULL,

    FMM_OPENWIN_SUCCESS,
    FMM_OPENWIN_FAIL,
    FMM_OPENWIN_FILE_NOT_EXIST
}MMIFMM_OPENWIN_RET_E;

typedef enum
{
    FUNC_FIND_NULL, 

    FUNC_FIND_FILE,                 //搜索文件
    FUNC_FIND_FOLDER,               //搜索文件夹
    FUNC_FIND_FILE_AND_FOLDER,      //搜索文件或文件夹
    FUNC_FIND_FILE_AND_ALLFOLER,    //搜索文件和所有文件夹
    FUNC_FIND_FOLDERS_WITH_SUBFILES  //搜索文件和包含文件的文件夹，并且文件夹要和它的文件相关联
}MMIFMM_FUNC_TYPE_E;

typedef enum
{
    WIN_DISP_NULL,  

    WIN_DISP_1LINE,     //单行文本方式
    WIN_DISP_2LINE      //双行缩略图方式
}MMIFMM_WINDISP_TYPE_E;

//搜索文件用的筛选结构
typedef struct 
{
    //用于筛选的关键字符串, 如"*.mp3"; 并且可以有多个关键字符串，当中用空格隔开, 如"*.mp3 *.mid *.wav"
    //"*" 表示列出所有文件或文件夹
    uint8   filter_str[FMM_FILTER_STRING_MAXLEN+2]; 
}MMIFMM_FILTER_T;

typedef enum
{
    MMIFMM_SELECT_MAIN_TYPE,
    MMIFMM_SELECT_SECOND_TYPE,
    MMIFMM_SELECT_MAX_TYPE
}MMIFMM_SELECT_TYPE_E;

typedef struct 
{
    //MMIFMM_DEVICE_E              select_device; //udisk or sd card or both
    uint32                       select_file[MMIFMM_SELECT_MAX_TYPE];//select music type, one bit present one type,//0xff represent all music type
    uint32                       max_size;//allow music max size, if maxsize = 0, represent no limited
    MMI_WIN_ID_T                 win_id;//win id which need to be returned
    const wchar                  *ticked_file_wstr;//file need to be ticked on
    uint16                       ticked_file_len;//file name len
    int32                        ring_vol;      //ring vol number
}MMIFMM_SELECT_INFO_T;


//回调函数类型, is_success表示操作结果是否成功, 结果以 FILEARRAY 列表的方式返回
typedef void (*MMIFMM_OpenFileCallBack)(
                                        BOOLEAN     is_success,
                                        FILEARRAY   file_array
                                        );

typedef BOOLEAN (*MMIFMM_SelectFileCallBack)(
                                            uint16             total_mark_num,
                                            FILEARRAY_DATA_T   * file_data_ptr
                                          );
typedef struct
{
    wchar                   *path_ptr;      //默认打开目录，如果是NULL，则从根目录打开
    uint16                  path_len;
    MMIFMM_FILTER_T         *filter_ptr;    //筛选参数
    MMIFMM_FUNC_TYPE_E      func_type;      //类型
    BOOLEAN                 is_multi_sel;   //是否多选
    BOOLEAN                 is_sel_folder; //是否选择文件夹，传给s_is_sel_folder
    PROCESSMSG_FUNC         handle_func;    //自定义处理函数
    FILEARRAY               ret_array;      //输出数组
    MMIFMM_OpenFileCallBack callback;        //回调函数
    MMIFMM_SelectFileCallBack select_cbk; //针对标记状态的callback,针对有些应用要对标记的文件或文件夹进行操作,
                                            //如果没有这项需求，传PNULL
}MMIFMM_OPEN_LAYER_WIN_INFO_T;

#define MMIFMM_FILE_TYPE(type)         (type & 0xFF000000)//24-31 bits:file type 
#define MMIFMM_MEDIA_TYPE(type)        (type & 0x00FFFFFF) //0-23 bits:media type ,22,23 only for drm
#define MMIFMM_UNKNOW_TYPE              0x00000000      //unknuow type
#define MMIFMM_SUFFIX_BASE              0x00000001      //base type //0x00000001
#define MMIFMM_FILE_TYPE_BASE           (MMIFMM_SUFFIX_BASE << 24)      //base type //0x00000001

//picture type
#define MMIFMM_PICTURE_GIF              ((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<0))//0x00000002
#define MMIFMM_PICTURE_BMP              ((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<1))//0x00000004
#define MMIFMM_PICTURE_WBMP             ((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<2))//0x00000008
#define MMIFMM_PICTURE_PNG              ((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<3))//0x00000010
#define MMIFMM_PICTURE_JPEG             ((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<4))//0x00000020
#define MMIFMM_PICTURE_JPG              MMIFMM_PICTURE_JPEG//((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<5))
//drm type
//#ifdef DRM_SUPPORT 
#define MMIFMM_DM_FILE                  (MMIFMM_SUFFIX_BASE<<22)//0x00400000
#define MMIFMM_DCF_FILE                 (MMIFMM_SUFFIX_BASE<<23)//0x00800000
//#endif
//music type 
#define MMIFMM_MUSIC_MP3                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<0)) //0x00000100
#define MMIFMM_MUSIC_WMA                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<1)) //0x00000200
#define MMIFMM_MUSIC_MID                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<2))//0x00000400
#define MMIFMM_MUSIC_MIDI               MMIFMM_MUSIC_MID
#define MMIFMM_MUSIC_AMR                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<3))//0x00000800
#define MMIFMM_MUSIC_ACC                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<4))//0x00001000
#define MMIFMM_MUSIC_M4A                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<5))//0x00002000
#define MMIFMM_MUSIC_WAV                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<6))//0x00004000

// 影像文件
#define MMIFMM_MOVIE_RMVB               ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<0))//0x00008000
#define MMIFMM_MOVIE_FLV                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<1))//0x00010000
#define MMIFMM_MOVIE_MP4                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<2))//0x00020000
#define MMIFMM_MOVIE_3GP                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<3))//0x00040000
#define MMIFMM_MOVIE_AVI                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<4))//0x01000000
#ifdef MMI_KING_MOVIE_SUPPORT
#define MMIFMM_MOVIE_KMV                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<5))//32位用完了，暂时这样处理吧。
#define MMIFMM_MOVIE_SMV                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<6))
#ifdef MMI_KING_MOVIE_SUPPORT_HMV
#define MMIFMM_MOVIE_HMV                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<7))
#endif
#endif
//电子书
#define MMIFMM_MOVIE_TXT                ((MMIFMM_FILE_TYPE_EBOOK * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE))//0x00080000
#define MMIFMM_MOVIE_LRC                ((MMIFMM_FILE_TYPE_EBOOK * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<< 1))//0x00100000
// COM压缩文件
#define MMIFMM_MOVIE_SZIP               ((MMIFMM_FILE_TYPE_SZIP * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE))//0x00200000
//VCARD
#if defined MMI_VCARD_SUPPORT
#define MMIFMM_MOVIE_VCF                ((MMIFMM_FILE_TYPE_VCARD * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE))//0x00400000
#endif
//Vcalendar
#ifdef MMI_VCALENDAR_SUPPORT
#define MMIFMM_MOVIE_VCS				((MMIFMM_FILE_TYPE_VCALENDAR * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE))//32位用完了，暂时这样处理吧。
#endif
//MTV
#define MMIFMM_MOVIE_MTV                ((MMIFMM_FILE_TYPE_MTV * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE))//Ox00800000
//JAVA
#define MMIFMM_JAVA_JAD                 ((MMIFMM_FILE_TYPE_JAVA * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE))//Ox02000000
#define MMIFMM_JAVA_JAR                 ((MMIFMM_FILE_TYPE_JAVA * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<1))//0x04000000

//THEME
#define MMIFMM_THEME_QB					((MMIFMM_FILE_TYPE_THEME * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE))

// NF浏览器支持的文件
#define MMIFMM_BROWSER_HTM              ((MMIFMM_FILE_TYPE_BROWSER * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE))//Ox08000000
#define MMIFMM_BROWSER_HTML             ((MMIFMM_FILE_TYPE_BROWSER * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<1))//0x10000000
#define MMIFMM_BROWSER_XML              ((MMIFMM_FILE_TYPE_BROWSER * MMIFMM_FILE_TYPE_BASE) +(MMIFMM_SUFFIX_BASE<<2))//0x20000000
// Font
#define MMIFMM_FONT                     ((MMIFMM_FILE_TYPE_FONT * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE))//0x40000000

#ifdef MRAPP_SUPPORT
#define MMIFMM_MRAPP_MRP                ((MMIFMM_FILE_TYPE_MRP * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE))
#define MMIFMM_MRAPP_NES                ((MMIFMM_FILE_TYPE_MRP * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_SUFFIX_BASE<<1))
#endif

#define MMIFMM_PIC_ALL                  ((MMIFMM_FILE_TYPE_PICTURE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_FILE_ALL >> 10)) //(MMIFMM_PICTURE_JPG|MMIFMM_PICTURE_GIF|MMIFMM_PICTURE_BMP|MMIFMM_PICTURE_WBMP|MMIFMM_PICTURE_PNG)//0x0000001f
#define MMIFMM_MUSIC_ALL                ((MMIFMM_FILE_TYPE_MUSIC * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_FILE_ALL >> 10))//(MMIFMM_MUSIC_MP3|MMIFMM_MUSIC_WMA|MMIFMM_MUSIC_MID|MMIFMM_MUSIC_AMR|MMIFMM_MUSIC_ACC|MMIFMM_MUSIC_M4A|MMIFMM_MUSIC_WAV|MMIFMM_MUSIC_MIDI)//0x0001ff00
#define MMIFMM_MOVIE_ALL                ((MMIFMM_FILE_TYPE_MOVIE * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_FILE_ALL >> 10))//(MMIFMM_MOVIE_MP4|MMIFMM_MOVIE_3GP|MMIFMM_MOVIE_AVI|MMIFMM_MOVIE_FLV|MMIFMM_MOVIE_RMVB)//cr227510 maryxiao
#define MMIFMM_TXT_ALL                  ((MMIFMM_FILE_TYPE_EBOOK * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_FILE_ALL >> 10))//(MMIFMM_MOVIE_TXT|MMIFMM_MOVIE_LRC)
#define MMIFMM_DRM_ALL                  (MMIFMM_DM_FILE|MMIFMM_DCF_FILE)
#define MMIFMM_JAVA_ALL                 ((MMIFMM_FILE_TYPE_JAVA * MMIFMM_FILE_TYPE_BASE) + (MMIFMM_FILE_ALL >> 10))
#define MMIFMM_FILE_ALL                 0xffffffff

#ifdef DRM_SUPPORT
typedef struct
{
    uint16 cons_mode_limit;             //受限的消费模式如count, interval, time
    uint16 method_level_limit;          //受限的DRM文件类型如fl, cd, sd
    BOOLEAN is_rights_valid;            //是否只可选版权有效的文件    
}MMIFMM_DRM_LIMIT_TYPE_T;               //在做查找文件时的最后选择时，设置应用限制的类型

#define MMIFMM_DRM_CONS_LIMIT_TYPE      (0x0001)
#define MMIFMM_DRM_CONS_LIMIT_COUNT     (MMIFMM_DRM_CONS_LIMIT_TYPE<<0)
#define MMIFMM_DRM_CONS_LIMIT_INTERVAL  (MMIFMM_DRM_CONS_LIMIT_TYPE<<1)
#define MMIFMM_DRM_CONS_LIMIT_DATETIME  (MMIFMM_DRM_CONS_LIMIT_TYPE<<2)

#define MMIFMM_DRM_METHOD_LIMIT_TYPE    (0x0001)
#define MMIFMM_DRM_METHOD_LIMIT_FL      (MMIFMM_DRM_METHOD_LIMIT_TYPE<<0)
#define MMIFMM_DRM_METHOD_LIMIT_CD      (MMIFMM_DRM_METHOD_LIMIT_TYPE<<1)
#define MMIFMM_DRM_METHOD_LIMIT_SD      (MMIFMM_DRM_METHOD_LIMIT_TYPE<<2)

#endif
#ifdef SEARCH_SUPPORT
#define MMISEARCH_SUFFIX_MAX_LEN           10  //include dot
#endif
/**--------------------------------------------------------------------------*
**                         GLOBAL VALUE DEFINITION                              *
**--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : initialize FMM module
//  Global resource dependence : none
//  Author: robert.lu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitModule(void);

/*****************************************************************************/
//  Description : open FMM
//  Global resource dependence : none
//  Author: Robert.Lu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenExplorer(void);

/*****************************************************************************/
//  Description : open any fmm win
//  Global resource dependence : none
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenExplorerExt(
                                      const wchar* path_ptr,
                                      uint16   path_len,
                                      const wchar * filename,
                                      uint16   file_len,
                                      BOOLEAN is_need_display,
                                      uint32  find_suffix_type
                                      );

/*****************************************************************************/
//  Description : open any fmm win 
//  Global resource dependence : none
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenExplorerWithoutProtect(
                                      const wchar * path_ptr,
                                      uint16   path_len,
                                      const wchar * filename,
                                      uint16   file_len,
                                      BOOLEAN is_need_display,
                                      uint32  find_suffix_type
                                      );

/*****************************************************************************/
//  Description : open any fmm win with callback function
//  Global resource dependence : none
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenExplorerWin(MMIFMM_OPENWIN_INFO_T * openwin_info);

/*****************************************************************************/
//  Description : handle menu option function
//  Global resource dependence : 
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_HandleMenuOpt(MMIFMM_OPTION_TYPE_E type);

/*****************************************************************************/
//  Description : 比较名称
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileName(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     ) ;

/*****************************************************************************/
//  Description : 比较文件大小
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileSize(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     );

/*****************************************************************************/
//  Description : 比较文件时间
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileTime(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     );

/*****************************************************************************/
//  Description : 比较类型
//  Global resource dependence : 
//  Author: Robert Lu
//  Note: 
/*****************************************************************************/
PUBLIC int MMIAPIFMM_CompareFileType(
                                     const void     *i1_data,   // first data
                                     const void     *i2_data    // second data
                                     );

/*****************************************************************************/
//  Description : 读当前目录中的文件数据，
//  Global resource dependence :
//  Author: robert.lu
//  Note: 文件保存，没有实现
/*****************************************************************************/
PUBLIC uint16 MMIAPIFMM_GetCurrentPathFile(// 所读文件和目录的总数
                                           const wchar          *full_path_ptr,     // 全路径,查找条件
                                           //MMIFMM_READ_TYPE_E   type,               // 类型
                                           MMIFMM_DATA_INFO_T   *data_ptr         // 输出数据
                                           //uint16               *output_filename_ptr// 输出数据的文件名
                                           );
/*****************************************************************************/
//  Description : get file type
//  Global resource dependence : 
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMIAPIFMM_GetFileType(
                                                const wchar      *suffix_name_ptr,
                                                uint16      suffix_name_len
                                                );

/*****************************************************************************/
//  Description : convert file type to fmm file type
//  Global resource dependence : 
//  Author: Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_FILE_TYPE_E MMIAPIFMM_ConvertFileType(
                                                const wchar      *suffix_name_ptr,
                                                uint16      suffix_name_len
                                                );

/*****************************************************************************/
//  Description : convert file type to movie type
//  Global resource dependence : 
//  Author: Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC MMICOM_VIDEO_TYPE_E MMIAPIFMM_ConvertMovieFileType(
                                                 const wchar    *suffix_name_ptr,
                                                  uint16    suffix_name_len
                                                  );
/*****************************************************************************/
//  Description : convert file type to music type
//  Global resource dependence : 
//  Author: Robert Lu
//  Note:
/*****************************************************************************/
PUBLIC MMISRVAUD_RING_FMT_E MMIAPIFMM_ConvertMusicFileType(
                                                  const wchar    *suffix_name_ptr,
                                                  uint16    suffix_name_len
                                                  );

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
                                                    );

/*****************************************************************************/
//  Description : sort
//  Global resource dependence :
//  Author: robert.lu
//  Note:
/*****************************************************************************/
PUBLIC int MMIAPIFMM_FileDataSort(
                                  MMIFMM_DATA_INFO_T    *list_data_ptr,
                                  MMIFMM_SORT_TYPE_E    sort
                                  );

/*****************************************************************************/
//  Description : stop current operation when sd plug
//  Global resource dependence : 
//  Author:robert.lu
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_StopOperaSDPlug(void);

                                       
/*****************************************************************************/
//  Description : handle copy task callback
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CopyCallBack(
                         uint16     result
                         );

/*****************************************************************************/
//  Description : handle copy task callback
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CopyFileExistInform(void);

/*****************************************************************************/
//  Description : set preview picture anim control param
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SetPreviewPicParam(
                                     BOOLEAN        is_update,          //in:是否立即刷新
                                     wchar          *full_path_wstr_ptr,//in
                                     uint16         full_path_wstr_len, //in:字节数
                                     MMI_CTRL_ID_T  ctrl_id
                                     );

/*****************************************************************************/
//  Description : get fmm data
//  Global resource dependence : none
//  Author: guanyong.zhang
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_DATA_INFO_T * MMIAPIFMM_GetFmmData(void);

/*****************************************************************************/
//  Description : 停止复制/删除等文件操作
//  Global resource dependence :
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_ExitFileOption(void);

/*****************************************************************************/
//  Description :是否在文件复制/删除操作
//  Global resource dependence :
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsFileOption(void);

/*****************************************************************************/
//  Description : 更新当前的文件列表
//  Global resource dependence :
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_UpdateCurrentFileLIst(void);

/*****************************************************************************/
//  Description : get dev info and check if have enough space
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsDevEnoughSpace(uint32 size,MMIFILE_DEVICE_E dev);

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
                                        );
/*****************************************************************************/
//  Description : initialize Multim
//  Global resource dependence : none
//  Author: Liqing.Peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitFolders(void);

/*****************************************************************************/
//  Description : initialize NV about multim wallpaper
//  Global resource dependence : none
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitWallPaper(void);

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
                                           );

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
                                           );

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
                                  );

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
                                );
/*****************************************************************************/
//  Description : is file wallpaper file
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsWallPaperFile(
                                  const wchar *full_path_ptr,
                                  uint16    full_path_len
                                  );

/*****************************************************************************/
//  Description : change wallpaper file name
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_ChangeWallPaperFileName(
                                      const wchar *full_path_ptr,
                                      uint16    full_path_len
                                      );

/*****************************************************************************/
//  Description : write wallpaper file info NV
//  Global resource dependence : none
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_WriteWallPaperNV(
                                       MMIMULTIM_AS_WALLPAPER_INFO  *wallpaper_file_ptr
                                       );

/*****************************************************************************/
//  Description : set wallpaper,close waiting window
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseWaitSetWallpaper(
                                               GUIANIM_RESULT_E     result
                                               );

/*****************************************************************************/
//  Description : 设置图片为墙纸,处理请稍候消息
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_HandleWallpaperWaitMsg(void);

/*****************************************************************************/
// 	Description : reset pic wallpaper setting
//	Global resource dependence : 
//  Author: liqing.peng
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_ResetWallpaperSetting(void);

/*****************************************************************************/
// 	Description : set wallpaper,open waiting window
//	Global resource dependence : 
//  Author: Jassmine
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenWaitSetWallpaper(void);
/*****************************************************************************/
// 	Description : get wallpaper information
//	Global resource dependence : none
//  Author: liqing.peng
//	Note: 
/*****************************************************************************/
PUBLIC MMIFMM_AS_WALLPAPER_INFO *MMIAPIFMM_GetWallPaperInfo(void);

/*****************************************************************************/
// 	Description : 产生随机的文件名，并且设置.
//	Global resource dependence : 
//  Author:liqing.peng
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetRandomNameEditbox(
                                   const uint8              *prefix_ptr, //in    
                                   MMI_CTRL_ID_T            ctrl_id     //in
                                   );

/*****************************************************************************/
//  Description :if is FmmMainWinOpen
//  Global resource dependence : 
//  Author: zhaohui
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsFmmMainWinOpen( void );


/*****************************************************************************/
// Description : update file list
// Global resource dependence : 
// Author:  jian.ma
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_UpdateFmmList(uint32 device,BOOLEAN is_enter_root);

/*****************************************************************************/
// Description : show file error type  
// Global resource dependence : 
// Author:  
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_PromptFileErrorType(MMI_WIN_ID_T alert_win_id, MMIFILE_ERROR_E fs_error, MMIFILE_DEVICE_E filedev);

/*****************************************************************************/
//  Description : check memory card status
//  Return: 
//  Global resource dependence : 
//  Author: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CheckMemoryCard(MMIFILE_DEVICE_E file_dev);

/*****************************************************************************/
//  Description : handle del task callback
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
void MMIAPIFMM_DelCallBack(uint16     result);

/*****************************************************************************/
//  Description : get file info form fullpath
//  Global resource dependence :
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetFileInfoFormFullPath(
                                      const wchar *full_file_name,       //in
                                      const uint32 filename_len,          //in
                                      MMIFILE_FILE_INFO_T *file_info//out
                                      );

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
                                     );

/*****************************************************************************/
// 	Description: get the prefer disk
//	Global resource dependence:
//  Author: bin.ji
//	Note:
/*****************************************************************************/
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetDefaultDisk(void);

/*****************************************************************************/
// 	Description: is enough space
//	Global resource dependence:
//  Author: aoke.hu
//	Note:新增参数，获取满足空间要求的盘符.有转储需求的模块可以使用，比如拍照，录像，录音等
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsEnoughSpace(
                                     MMIFMM_STORE_TYPE_E store_type,
                                     MMIFILE_DEVICE_E prefer_devie,
                                     uint32 min_free_size,
                                     MMIFILE_DEVICE_E *device_ptr  //[out]
                                     );

/*****************************************************************************/
//  Description : get system file full path for read
//  Author:aoke.hu
//  Note: check from MMI_DEVICE_SYSTEM
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetSysFileFullPathForReadEx(
                                                 BOOLEAN        is_fix_store,           //[IN]
                                                 BOOLEAN        is_from_cdisk,          //[IN]
                                                 const wchar    *path_name_ptr,         //[IN]
                                                 uint16         path_name_len,          //[IN]                                                 
                                                 const wchar    *file_name_ptr,         //[IN]
                                                 uint16         file_name_len,          //[IN]
                                                 wchar          *full_path_name_ptr,    //[OUT]
                                                 uint16         *full_path_len_ptr      //[OUT]                                                 
                                                 );

/*****************************************************************************/
//  Description : get system file full path for write
//  Author:aoke.hu
//  Note: check from MMI_DEVICE_SYSTEM
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetSysFileFullPathForWriteEx(
                                                  BOOLEAN        is_fix_store,          //[IN]TRUE, is fixed. FALSE, is portable
                                                  BOOLEAN        is_from_cdisk,         //[IN]
                                                  const wchar    *path_name_ptr,        //[IN]
                                                  uint16         path_name_len,         //[IN]                                                 
                                                  const wchar    *file_name_ptr,        //[IN]
                                                  uint16         file_name_len,         //[IN]
                                                  uint32         min_free_size,         //[IN]
                                                  wchar          *full_path_name_ptr,   //[OUT]
                                                  uint16         *full_path_len_ptr     //[OUT]                                                 
                                                  );

/*****************************************************************************/
//  Description : get system file full path for read
//  Author:bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetSysFileFullPathForRead(
                                                 BOOLEAN        is_fix_store,           //[IN]
                                                 const wchar    *path_name_ptr,         //[IN]
                                                 uint16         path_name_len,          //[IN]                                                 
                                                 const wchar    *file_name_ptr,         //[IN]
                                                 uint16         file_name_len,          //[IN]
                                                 wchar          *full_path_name_ptr,    //[OUT]
                                                 uint16         *full_path_len_ptr      //[OUT]                                                 
                                                 );

/*****************************************************************************/
//  Description : get system file full path for write
//  Author:bin.ji
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetSysFileFullPathForWrite(
                                                  BOOLEAN        is_fix_store,          //[IN]TRUE, is fixed. FALSE, is portable
                                                  const wchar    *path_name_ptr,        //[IN]
                                                  uint16         path_name_len,         //[IN]                                                 
                                                  const wchar    *file_name_ptr,        //[IN]
                                                  uint16         file_name_len,         //[IN]
                                                  uint32         min_free_size,         //[IN]
                                                  wchar          *full_path_name_ptr,   //[OUT]
                                                  uint16         *full_path_len_ptr     //[OUT]                                                 
                                                  );

/*****************************************************************************/
//  Description : create sysfile dir, if success return TRUE, else return FALSE
//  Global resource dependence : none
//  Author: bin.ji
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CreateSysFileDir(
                                        MMIFILE_DEVICE_E    dev_type,               //[IN]
                                        const wchar     *path_name_ptr,         //[IN]
                                        uint16          path_name_len,          //[IN]
                                        MMIFILE_ERROR_E *file_err_ptr           //[OUT] can be null
                                        );

/*****************************************************************************/
//  Description : Combine Sys Path
//  Global resource dependence : none
//  Author: xingdong.li
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CombineSysPath(
                         MMIFILE_DEVICE_E    dev_type,              //[IN]
                         const wchar    *path_name_ptr,         //[IN]
                         uint16         path_name_len,          //[IN]
                         const wchar    *file_name_ptr,         //[IN]
                         uint16         file_name_len,          //[IN]
                         wchar          *full_path_name_ptr,    //[OUT]
                         uint16         *full_path_len_ptr      //[OUT]
                         );

/*****************************************************************************/
//  Description : 和MMIFMM_SearchFileInPath配合使用
//  Return: 
//  Global resource dependence : 
//  Author: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileStop(void);

/*****************************************************************************/
// Description : show file error type  
// Global resource dependence : 
// Author:  
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_PreviewPicture(wchar *full_path_name_ptr);

/*****************************************************************************/
// Description : preview current video file
// Global resource dependence : 
// Author:  murphy.xie
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_PreviewVideo(wchar *full_path_name_ptr);


/*****************************************************************************/
// Description : copy next  
// Global resource dependence : 
// Author:  
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CopyNext(void);

/*****************************************************************************/
// Description : setting file detail
// Global resource dependence : 
// Author: YING.XU
// Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetFolderDetail(
                                 MMIFMM_DETAIL_DATA_T  *detail_data,
                                 MMI_CTRL_ID_T              ctrl_id,
                                 MMI_WIN_ID_T            win_id
                                 );

/*****************************************************************************/
// Description : setting folder detail cnf
// Global resource dependence : 
// Author: YING.XU
// Note: 
/*****************************************************************************/
PUBLIC void   MMIAPIFMM_SetFolderDetailCNF(
                                 MMIFMM_DETAIL_DATA_T  *detail_data,
                                 MMI_CTRL_ID_T              ctrl_id,
                                 MMI_WIN_ID_T            win_id
                                 );
/*****************************************************************************/
// Description : 搜索串是否有效
// Global resource dependence : 
// Author: Ming.Song
// Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_FmmFindStringIsValid(
                                   MMI_STRING_T   *file_name_ptr  //in:
                                   );

/*****************************************************************************/
// Description : 打开文件列表窗口
// Global resource dependence : 
// Author: 
// Note:
/*****************************************************************************/ 
PUBLIC void MMIAPIFMM_OpenFMMMainWin(void);

/*****************************************************************************/
// Description : close文件列表窗口
// Global resource dependence : 
// Author: 
// Note:
/*****************************************************************************/ 
PUBLIC void MMIAPIFMM_CloseFMMMainWin(void);

/*****************************************************************************/
// Description : 文件详情
// Global resource dependence : 
// Author:aoke.hu
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_DetailFileData(
                                     MMIFMM_DETAIL_DATA_T* detail_data  // 数据
                                     );

/*****************************************************************************/
//  Description : convert mime type
//  Global resource dependence : 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_FILE_TYPE_E MMIFMM_ConvertMimeType(
                                                MIME_TYPE_E mime_type
                                                );

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
);

/*****************************************************************************/
//  Description : can this drm file execute
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 是否可以预览，如果不能预览，则需要提示下载
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_UpdateListIconData(
    MMI_CTRL_ID_T ctrl_id, 
    uint16 index,
    const wchar *file_dir_ptr,              //文件所在文件夹名称
    const wchar *file_path_ptr              //文件全路径
);

/*****************************************************************************/
//  Description : can this drm file execute
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 本函数用于更新选择文件夹中的图标
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_UpdateSelectListIconData(
    wchar *file_path_ptr,
    uint16 file_path_len
);

/*****************************************************************************/
//  Description : create drm pop menu
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 
/*****************************************************************************/
PUBLIC MMI_RESULT_E MMIAPIFMM_DrmOpenDownloadMenuWin(wchar *full_path);
#endif

/*****************************************************************************/
// Description : open file  
// Global resource dependence : 
// Author: xiaoming.ren
// RETRUN: 
// Note:   
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenFile(wchar *full_path_name_ptr);

/*****************************************************************************/
//  Description : MMIAPIFMM_ShowTxtContent
//  Global resource dependence :
//  Author: renyi.hu
//  Note:
/*****************************************************************************/
PUBLIC  void  MMIAPIFMM_ShowTxtContent(wchar *full_path_name_ptr);


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////// 以下接口部分是从mmi_filemgr.h移过来的      /////////////
///////////////////今后的对外接口都只能放在mmifmm_export.h///////////////
///////////////////**************************************************//////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
//#if 1
/*****************************************************************************/
//  Description : split full pathname to filepath and filename/dir
//  Global resource dependence : 
//  Author:aoke.hu
//  Note: 将输入的全路径（full_path_ptr）分解成路径名（full_path_ptr）+文件名（file_name_ptr）
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SplitLastName(uint16* full_path_ptr,             //[in],[out]
                                     uint16* full_path_len_ptr,         //[in],[out]
                                     uint16* file_name_ptr,             //[out]
                                     uint16* file_name_len_ptr          //[out]
                                     );

/*****************************************************************************/
//  Description : split full pathname to device, dir, filename
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SplitFullPath(      //支持多级目录
                              const wchar      *full_path_ptr,     //in
                              uint16           full_path_len,      //in, unicode个数
                              wchar            *device_ptr,        //out, device_xxx_ptr, dir_xxx_ptr, file_name_xxx_ptr 可以是NULL, 但是至少有一个不是NULL
                              uint16           *device_len_ptr,    //out, unicode个数
                              wchar            *dir_ptr,           //out
                              uint16           *dir_len_ptr,       //out, unicode个数
                              wchar            *file_name_ptr,     //out
                              uint16           *file_name_len_ptr  //out, unicode个数
                              );

/*****************************************************************************/
// 	Description : split full path name to path,name and suffix
//	Global resource dependence : 
//  Author: Jassmine
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SplitFullPathExt(
                                     const wchar     *full_path_ptr, //in
                                     uint16          full_path_len,  //in
                                     wchar           *path_ptr,      //in/out:may PNULL
                                     uint16          *path_len_ptr,  //in/out:may PNULL,path_ptr不为空时,此处必须填max len
                                     wchar           *name_ptr,      //in/out:may PNULL,don't include suffix
                                     uint16          *name_len_ptr,  //in/out:may PNULL,name_ptr不为空时,此处必须填max len
                                     wchar           *suffix_ptr,    //in/out:may PNULL,include dot
                                     uint16          *suffix_len_ptr //in/out:may PNULL,suffix_ptr不为空时,此处必须填max len
                                     );

/*****************************************************************************/
//  Description : combine full pathname from device, file type, filename
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CombineFullPath(    //支持多级目录
                              const wchar     *device_ptr,        //in
                              uint16           device_len,         //in, unicode个数
                              const wchar     *dir_ptr,           //in
                              uint16           dir_len,            //in, unicode个数
                              const wchar     *file_name_ptr,     //in, file_name_ptr 可以是NULL
                              uint16           file_name_len,      //in, , unicode个数
                              wchar           *full_path_ptr,     //out
                              uint16           *full_path_len_ptr  //out, unicode个数
                              );

/*****************************************************************************/
//  Description : combin absolut path according to root, dir, name
//  Global resource dependence :                                
//  Author: gang.tong
//  Note:
/*****************************************************************************/
PUBLIC uint16 MMIAPIFMM_CombineFullPathEx(
                                          wchar * abs_path_ptr,   // [out] the absolute path name.
                                          uint32 abs_path_len,    // [in]  the buffer length of abs_path_ptr.
                                          MMIFILE_DEVICE_E file_dev,  // [in]  the file dev type.
                                          const wchar* dir_ptr,  // [in]  the directory name
                                          //                    BOOLEAN dir_is_ucs2,    // [in]  whether the directory is in ucs2
                                          const wchar* name_ptr//, // [in]  the file name 
                                          //                    BOOLEAN name_is_ucs2    // [in]  whether the file is in ucs2
                                          );
/*****************************************************************************/
//  Description : split file name to two part: name and suffix
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SplitFileName(
                              const wchar      *file_name_ptr,     //in
                              uint16           file_name_len,      //in, unicode个数
                              wchar            *name_ptr,          //out, name_xxx_ptr, suffix_xxx_ptr 可以是NULL, 但是至少有一个不是NULL
                              uint16           *name_len_ptr,      //out, unicode个数
                              wchar            *suffix_ptr,        //out
                              uint16           *suffix_len_ptr     //out, unicode个数
                              );

/*****************************************************************************/
//  Description : get file size string
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_GetFileSizeString(
                             uint32  size,
                             wchar*  str_ptr,
                             uint16  wstr_len,
							 BOOLEAN is_rounding
                             );

/*****************************************************************************/
//  Description : Init file system
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_InitFileTasks(void);


/*****************************************************************************/
//  Description : Init file system
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC SFS_ERROR_E MMIAPIFMM_RegisterDevice(
                                const wchar *device_ptr,    //in
                                uint16      device_len      //in, unicode个数
                                );

/*****************************************************************************/
//  Description : unregister device 
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_UnRegisterDevice(
                                const wchar *device_ptr,    //in
                                uint16      device_len      //in, unicode个数
                                );


/*****************************************************************************/
//  Description : get device status, ok or not ok
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_FormatDevice(
                            const wchar *device_ptr,    //in
                            uint16      device_len,     //in, unicode个数
                            MMIFILE_DEVICE_FORMAT_E     format  //in
                            );

/*****************************************************************************/
//  Description : format device asyn
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_FormatDeviceAsyn(
                                 const wchar                *device_ptr,        //in
                                 uint16                     device_len,         //in, unicode个数
                                 MMIFILE_DEVICE_FORMAT_E    format,             //in
                                 MMI_WIN_ID_T               win_id,             //in
                                 MMI_MESSAGE_ID_E           msg_id              //in
                                );

/*****************************************************************************/
//  Description : get device status, ok or not ok
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetDeviceStatus(
                                const wchar *device_ptr,    //in
                                uint16      device_len      //in, unicode个数
                                );

/*****************************************************************************/
//  Description : set sd status
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetDeviceTypeStatus(MMIFILE_DEVICE_E memory_type);

/*****************************************************************************/
//  Description : 根据设备类型返回给设备的TEXT ID
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_TEXT_ID_T  MMIAPIFMM_GetDeviceName(MMIFILE_DEVICE_E device_type);

/*****************************************************************************/
//  Description : 根据设备类型返回没有该设备的字符串
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_TEXT_ID_T  MMIAPIFMM_GetAlertTextIDByDev(MMIFILE_DEVICE_E device_type);

/*****************************************************************************/
//  Description : 根据设备类型返回给设备容量的Text id
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_TEXT_ID_T  MMIAPIFMM_GetDeviceMemStatusText(MMIFILE_DEVICE_E device_type);

/*****************************************************************************/
//  Description : 根据设备类型返回该设备的复制字符串的Text id
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_TEXT_ID_T  MMIAPIFMM_GetDeviceCopyText(MMIFILE_DEVICE_E device_type);

/*****************************************************************************/
//  Description : Callback
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC wchar* MMIAPIFMM_GetDevicePath(MMIFILE_DEVICE_E device_type);

/*****************************************************************************/
//  Description : Callback
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC uint16 MMIAPIFMM_GetDevicePathLen(MMIFILE_DEVICE_E device_type);
//
///*****************************************************************************/
////  Description : Callback
////  Global resource dependence : 
////  Author:
////  Note: 
///*****************************************************************************/
// PUBLIC wchar*  MMIFILE_GetRootDir(MMIFILE_DEVICE_E file_dev);

/*****************************************************************************/
//  Description : Callback
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
// PUBLIC wchar* MMIFILE_GetPathByFileDevice(MMIFILE_DEVICE_E file_dev);

/*****************************************************************************/
//  Description : Callback
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
// PUBLIC uint16 MMIFILE_GetPathLenByFileDevice(MMIFILE_DEVICE_E file_dev);

/*****************************************************************************/
//  Description : get file device type by sd card type
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
// PUBLIC FILE_DEV_E_T MMIFILE_GetFileDeviceType(MMIFILE_DEVICE_E sd_type);

/*****************************************************************************/
//  Description : get sd card type type by file device
//  Global resource dependence : 
//  Author: gang.tong
//  Note:
/*****************************************************************************/ 
// PUBLIC MMIFILE_DEVICE_E MMIFILE_GetDeviceTypeByFileDev(FILE_DEV_E_T fs_dev);



/*****************************************************************************/
//  Description : get file device type by sd card type
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
// PUBLIC FILE_DEV_E_T MMIFILE_GetFileDeviceByFilter(uint8 file_filter);

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
// PUBLIC uint8 MMIFILE_GetFileFilterByDevice(FILE_DEV_E_T file_dev);

/*****************************************************************************/
//  Description : 根据用户选择的设备类型，返回最适合的设备类型
//  Global resource dependence : 
//  Author: 
//  Note: 如果用户选择的设备剩余空间满足最小空间要求，返回用户设置；
//        如果用户选择的设备剩余空间不满足满足最小空间要求，返回到一个最大剩余空间的设备类型
//        如果U盘和所有的存储卡都不存在，返回默认的sd卡
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_GetSuitableFileDev(
												  MMIFILE_DEVICE_E user_sel_dev,//IN: 
												   uint32 min_free_space,//IN:
												   MMIFILE_DEVICE_E *suitable_dev_ptr//OUT:
												   );
											
/*****************************************************************************/
//  Description : create directory
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CreateDir(
                          const wchar      *full_path_ptr,     //in
                          uint16           full_path_len       //in, unicode个数
                          );

/*****************************************************************************/
//  Description : create directory with RO or HIDDEN attr
//  Global resource dependence : 
//  Author:aoke.hu
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_SetAttr(
                               const wchar       *full_path_ptr,     //in
                               uint16            full_path_len,       //in, 双字节为单位
                               BOOLEAN           is_hidden,         //in, 是否为隐藏属性 
                               BOOLEAN           is_ro,             //in, 是否为只读属性 
                               BOOLEAN           is_system,         //in, 是否为系统属性
                               BOOLEAN           is_arch           //in, 是否为存档属性
                               );

/*****************************************************************************/
//  Description : get file information
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetFileInfo(
                            const wchar             *full_path_ptr,     //in, 全文件名
                            uint16                  full_path_len,      //in, 全文件名长度, unicode个数
                            uint32                  *file_size_ptr,     //out,文件大小
							SFS_DATE_T				*modify_date,
							SFS_TIME_T				*modify_time
                            );


/*****************************************************************************/
//  Description : check whether file is exist
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsFileExist(
                            const wchar     *full_path_ptr,     //in
                            uint16          full_path_len       //in, unicode个数
                            );

/*****************************************************************************/
//  Description : check whether folder is exist
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsFolderExist(
                            const wchar     *full_path_ptr,     //in
                            uint16          full_path_len       //in, unicode个数
                            );

/*****************************************************************************/
//  Description : get device free space
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetDeviceFreeSpace(
                                    const wchar *device_ptr,        //in
                                    uint16      device_len,         //in, unicode个数
                                    uint32      *free_high_ptr,     //out
                                    uint32      *free_low_ptr       //out
                                    );

/*****************************************************************************/
//  Description : get device used space
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetDeviceUsedSpace(
                                    const wchar *device_ptr,        //in
                                    uint16      device_len,         //in, unicode个数
                                    uint32      *used_high_ptr,     //out
                                    uint32      *used_low_ptr       //out
                                    );

/*****************************************************************************/
//  Description : rename file
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_RenameFile(
                            const wchar     *full_path_ptr,     //in
                            uint16          full_path_len,      //in, unicode个数
                            wchar          *new_file_name_ptr, //in
                            uint16          new_file_name_len   //in, unicode个数
                            );

/*****************************************************************************/
//  Description : check device name whether it is Udisk
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetDeviceTypeByPath(
                              const wchar *device_ptr,    //in
                              uint16    device_len      //in, 双字节为单位
                              );

/*****************************************************************************/
//  Description : read file data syn, file must exist
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_ReadFilesDataSyn(
                                const wchar             *full_path_ptr,     //in
                                uint16                  full_path_len,      //in, unicode个数
                                uint8                   *buf_ptr,           //out
                                uint32                  to_read_size        //in
                                );

/*****************************************************************************/
//  Description : write file data syn, create file always and will delete file
//                existing
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_WriteFilesDataSyn(
                                  const wchar            *full_path_ptr,     //in
                                  uint16                  full_path_len,      //in, unicode个数
                                  const uint8             *buf_ptr,           //in
                                  uint32                  to_write_size       //in
                                  );

/*****************************************************************************/
//  Description : delete file syn
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_DeleteFileSyn(
                              const wchar                  *full_path_ptr,     //in
                              uint16                  full_path_len       //in, unicode个数
                              );

/*****************************************************************************/
//  Description : copy file
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_CopyFile(const wchar *sour_filename, const wchar * dest_filename);

/*****************************************************************************/
//  Description : read file data by offset syn, file must exist
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_ReadFileDataByOffsetSyn(
                                        const wchar             *full_path_ptr,     //in
                                        uint16                  full_path_len,      //in, unicode个数
                                        uint8                   *buf_ptr,           //out
                                        uint32                  to_read_size,       //in
                                        uint32                  offset              //in
                                        );


/*****************************************************************************/
//  Description : read file data asyn
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_ReadFilesDataAsyn(
                                  const wchar             *full_path_ptr,   //in
                                  uint16                  full_path_len,    //in, unicode个数
                                  uint8                   *buf_ptr,         //out
                                  uint32                  to_read_size,     //in
                                  SFS_HANDLE              *file_handle_ptr, //in/out
                                  MMI_WIN_ID_T            win_id,           //in:往窗口发消息,0则往控件发消息
                                  MMI_CTRL_ID_T           ctrl_id,          //in:往控件发消息,0则往窗口发消息
                                  MMI_MESSAGE_ID_E        msg_id            //in
                                  );

/*****************************************************************************/
//  Description : read file data asyn with file handle
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_ReadFilesDataAsynEx(
    MMIFILE_HANDLE          file_handle,        //in
    uint8                   *buf_ptr,           //out
    uint32                  to_read_size,       //in
    MMI_WIN_ID_T            win_id,             //in:往窗口发消息,0则往控件发消息
    MMI_CTRL_ID_T           ctrl_id,            //in:往控件发消息,0则往窗口发消息
    MMI_MESSAGE_ID_E        msg_id              //in
);

/*****************************************************************************/
//  Description : write file data asyn
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_WriteFilesDataAsyn(
                                   const wchar             *full_path_ptr,     //in
                                   uint16                  full_path_len,      //in, unicode个数
                                   uint8                   *buf_ptr,           //in
                                   uint32                  to_write_size,      //in
                                   MMI_WIN_ID_T            win_id,             //in, 异步写文件数据结束后,需要发消息通知的 win id
                                   MMI_MESSAGE_ID_E        msg_id              //in, 异步写文件数据结束后,需要发送的消息, 带一个BOOLEAN的参数
                                   );

/*****************************************************************************/
//  Description : read file data asyn with file handle
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_ReadFilesDataAsynEx(
    MMIFILE_HANDLE          file_handle,        //in
    uint8                   *buf_ptr,           //out
    uint32                  to_read_size,       //in
    MMI_WIN_ID_T            win_id,             //in:往窗口发消息,0则往控件发消息
    MMI_CTRL_ID_T           ctrl_id,            //in:往控件发消息,0则往窗口发消息
    MMI_MESSAGE_ID_E        msg_id              //in
);

/*****************************************************************************/
//  Description : write file data by offset asyn, file must exist
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_WriteFileDataByOffsetAsyn(
                                          const wchar             *full_path_ptr,     //in
                                          uint16                  full_path_len,      //in, unicode个数
                                          uint8                   *buf_ptr,           //in
                                          uint32                  to_write_size,      //in
                                          uint32                  offset,             //in
                                          MMI_WIN_ID_T            win_id,             //in
                                          MMI_MESSAGE_ID_E        msg_id              //in
                                          );

/*****************************************************************************/
//  Description : delete file asyn
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_DeleteFileAsyn(
                               const wchar             *full_path_ptr,     //in
                               uint16                  full_path_len,      //in, unicode个数
                               MMI_WIN_ID_T            win_id,             //in, 异步删除文件结束后,需要发消息通知的 win id
                               MMI_MESSAGE_ID_E        msg_id              //in, 异步删除文件结束后,需要发送的消息, 带一个BOOLEAN的参数
                               );

/*****************************************************************************/
//  Description : asyn operation confirm
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/ 
PUBLIC void MMIAPIFMM_OperationAsynCnf(
                              void                  *param_ptr, 
                              MMI_MESSAGE_ID_E      callback_msg_id
                              );

/*****************************************************************************/
//  Description :start copy one file
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CopyOneFile(
                            const wchar                 *src_full_path,
                            uint16                      src_path_len,       //in, unicode个数
                            const wchar                 *dest_full_path,
                            uint16                      dest_path_len,      //in, unicode个数
                            MMI_WIN_ID_T                win_id,
                            MMI_MESSAGE_ID_E            msg_id
                            );

/*****************************************************************************/
//  Description : read file data in copy file, can't be used alone
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CopyReadSrc(
                            SFS_ERROR_E     error
                            );

/*****************************************************************************/
//  Description : write file data in copy file, can't be used alone
//  Global resource dependence : 
//  Author:liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_CopyWriteDest(
                              SFS_ERROR_E   error
                              );

/*****************************************************************************/
//  Description : is file end
//  Global resource dependence:
//  Author: haiyang.hu
//  Note:
/*****************************************************************************/ 
PUBLIC BOOLEAN MMIAPIFMM_IsEndOfFile(  //TRUE: cur position is the end of the file.
                                   SFS_HANDLE handle
                                   );

/*****************************************************************************/
//  Description : Get a char from handle
//  Global resource dependence : 
//  Author: haiyang.hu
//  Note:
/*****************************************************************************/ 
PUBLIC SFS_ERROR_E MMIAPIFMM_GetChar(
                                   SFS_HANDLE handle, //[in]
                                   void*  char_ptr //[out]
                                   );

/*****************************************************************************/
//  Description : create file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_HANDLE MMIAPIFMM_CreateFile(
                                    const wchar *file_name, 
                                    uint32 access_mode,
                                    uint32 share_mode,  //Must be NULL,File System have not realize it 
                                    uint32 file_attri       //Must be NULL,File System have not realize it 
                                    );

/*****************************************************************************/
//  Description : create file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_CreateFileAsyn(
                                    const wchar *file_name, 
                                    uint32 access_mode,
                                    uint32 share_mode,  //Must be NULL,File System have not realize it 
                                    uint32 file_attri,     //Must be NULL,File System have not realize it 
									MMI_WIN_ID_T            win_id,   //in
									MMI_MESSAGE_ID_E        msg_id,    //in
									MMIFILE_HANDLE        *file_handle_ptr//OUT
                                    );

/*****************************************************************************/
//  Description : delete file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_DeleteFile(
                                    const wchar *name,
                                    MMIFILE_OVERLAPPED_T *overlapped_ptr
                                    );

/*****************************************************************************/
//  Description : create directiory
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_CreateDirectory(
                                        const wchar *path
                                        );

/*****************************************************************************/
//  Description : delete all files in folder
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_DeleteTreeEx(
	const wchar *pathName,
	const wchar *fileName,
    BOOLEAN is_del_dir);

/*****************************************************************************/
//  Description : delete folder and it's files in it, only can delete it's sub empty folder
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_DeleteTree(
	const wchar *pathName,
	const wchar *fileName);

/*****************************************************************************/
//  Description : delete diretory
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_DeleteDirectory(
                                        const wchar *path
                                        );

/*****************************************************************************/
//  Description : find first file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_HANDLE MMIAPIFMM_FindFirstFile(
                                        const wchar *find, 
                                        uint16 find_len,
                                        MMIFILE_FIND_DATA_T *fd
                                        );

/*****************************************************************************/
//  Description : find nezt file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_FindNextFile(
                                        MMIFILE_HANDLE file_handle, 
                                        MMIFILE_FIND_DATA_T *fd
                                        );

/*****************************************************************************/
//  Description : find close
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_FindClose(
                                    MMIFILE_HANDLE file_handle
                                    );

/*****************************************************************************/
//  Description : get file size
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC uint32 MMIAPIFMM_GetFileSize(
                            MMIFILE_HANDLE file_handle
                            );

/*****************************************************************************/
//  Description : set file size
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_SetFileSize(
                                    MMIFILE_HANDLE file_handle, 
                                    uint32 size
                                    );

/*****************************************************************************/
//  Description : read file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_ReadFile(
                                MMIFILE_HANDLE file_handle,
                                void *buffer,
                                uint32 bytes_to_read,
                                uint32 *bytes_read,
                                MMIFILE_OVERLAPPED_T *overlapped
                                );

/*****************************************************************************/
//  Description : write file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_WriteFile(
                                MMIFILE_HANDLE file_handle,
                                const void *buffer,
                                uint32 bytes_to_write,
                                uint32 *bytes_written,
                                MMIFILE_OVERLAPPED_T *overlapped
                                );

/*****************************************************************************/
//  Description : set file pointer
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC uint32 MMIAPIFMM_SetFilePointer(
                                MMIFILE_HANDLE file_handle,
                                int32 offset,
                                uint32 origin
                                );

/*****************************************************************************/
//  Description : get file pointer
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC int32 MMIAPIFMM_GetFilePointer(
                                MMIFILE_HANDLE file_handle,
                                uint32 origin
                                );

/*****************************************************************************/
//  Description : close file
//  Global resource dependence : 
//  Author: songbin.zeng
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E MMIAPIFMM_CloseFile(
                                    MMIFILE_HANDLE file_handle
                                    );
/*****************************************************************************/
//  Description : check and get validate device
//  Global resource dependence : 
//  Author: gang.tong
//  Note: 指定设备类型无效时，依次从SD卡向UDISK查找设备
/*****************************************************************************/ 
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetValidateDevice(MMIFILE_DEVICE_E original_device);

/*****************************************************************************/
//  Description : get file path len
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
//PUBLIC uint32 MMIAPIFILE_GetFilePathLen( const wchar * pWString );

/*****************************************************************************/
//  Description : Callback
//  Global resource dependence : 
//  Author:jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_Callback( uint16     result);

/*****************************************************************************/
//  Description : append file
//  Global resource dependence : 
//  Author: 
//  Note: append append_file_name from append_postion to base_file_name,
//        if is_delete_append_file is TRUE, after append, the append_file must be deleted
//  Return: return file result
/*****************************************************************************/ 
PUBLIC MMIFILE_ERROR_E  MMIAPIFMM_AppendFile(
                                    wchar *base_file_name,
									wchar *append_file_name,
									uint32 append_postion,
									BOOLEAN is_delete_append_file,
									uint32  *appended_size_ptr
                                    );
                                    
/*****************************************************************************/
//  Description : 返回剩余空间较大的device
//  Global resource dependence : 
//  Author: charlie.wang
//  Note:
/*****************************************************************************/ 
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetMaxFreeDeviceType(void);

/*****************************************************************************/
//  Description : 返回第一个有效的device
//  Global resource dependence : 
//  Author: 
//  Note:如果没有有效的返回FS_INVALID
/*****************************************************************************/ 
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetFirstValidDevice(void);

/*****************************************************************************/
//  Description : 返回第一个有效的sd card device type 
//  Global resource dependence : 
//  Author: 
//  Note:如果没有有效的返回MMI_DEVICE_NUM
/*****************************************************************************/ 
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetFirstValidSD(void);

/*****************************************************************************/
//  Description : 返回第一个存在的device，这个device可能是无效的
//  Global resource dependence : 
//  Author: 
//  Note:如果没有存在的返回FS_INVALID
/*****************************************************************************/ 
PUBLIC MMIFILE_DEVICE_E MMIAPIFMM_GetFirstExistDevice(void);

/*****************************************************************************/
//  Description : 根据设备类型返回给设备剩余容量的Text id
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_TEXT_ID_T  MMIAPIFMM_GetDeviceFreeSpaceText(MMIFILE_DEVICE_E device_type);

/*****************************************************************************/
//  Description : 根据设备类型返回该设备的图标
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_IMAGE_ID_T  MMIAPIFMM_GetOSDImageIDByDeviceType(MMIFILE_DEVICE_E device_type);
/*****************************************************************************/
//  Description : has exist file when move
//  Global resource dependence : none
//  Author: juan.wu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_MoveFileExist(void);

/*****************************************************************************/
//  Description : 根据设备类型返回该设备的图标
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/ 
PUBLIC MMI_IMAGE_ID_T  MMIAPIFMM_GetStorageIcon(MMIFILE_DEVICE_E device_type);
//#endif
#ifdef DRM_SUPPORT
/*****************************************************************************/
//  Description : is drm file by suffix name
//  Global resource dependence : 
//  Author: haiwu.chen
//  Note: 通过后缀检查是不是drm文件(此接口待废弃，最好使用MMIAPIDRM_IsDRMFile接口)
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_IsDrmFileBySuffix(
    wchar *file_name_ptr, //in:
    uint16  file_name_len   //in:
);
#endif

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////// 以下接口部分是从mmifmm_interface.h移过来的      /////////////
///////////////////今后的对外接口都只能放在mmifmm_export.h///////////////
///////////////////**************************************************//////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
//  Description : get icon id by fmm data info
//  Global resource dependence :
//  Author: james.zhang
//  Note:
/*****************************************************************************/
PUBLIC MMI_IMAGE_ID_T MMIAPIFMM_GetIconIdByFileType( MMIFMM_FILE_TYPE_E  file_type, MMIFILE_DEVICE_E device);

/*****************************************************************************/
//  Description : 打开一个窗口, 显示某一级目录, 并把当前目录下所有文件夹和
//                所有符合筛选条件的文件列出来
//  Return: 用户在窗口中选中文件后, 通过回调函数返回结果给上一级窗口
//  参数说明:
//      ret_array： 一定要的，如果为NULL，则不返回文件
//      callback：  一定要的，如果为NULL，则不返回文件
// 
//      path_ptr/path_len：打开目录，如果是NULL，则从根目录打开
//      filter_ptr：  搜索关键字符串，例如"*.mp3"
// 
//      func_type:  一般填 FUNC_FIND_FILE_AND_ALLFOLER，在选择文件的同时会把所有文件夹显示出来
//      is_multi_sel：是否返回多个文件，对于选择单个文件，填FALSE
// 
//      handle_func：   基本用不到，填NULL
//
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_OPENWIN_RET_E MMIAPIFMM_OpenFileWinByLayer(
                                wchar                   *path_ptr,      //默认打开目录，如果是NULL，则从根目录打开
                                uint16                  path_len,
                                MMIFMM_FILTER_T         *filter_ptr,    //筛选参数
                                MMIFMM_FUNC_TYPE_E      func_type,      //类型
                                BOOLEAN                 is_multi_sel,   //是否多选
                                PROCESSMSG_FUNC         handle_func,    //自定义处理函数
                                FILEARRAY               ret_array,      //输出数组
                                MMIFMM_OpenFileCallBack callback,       //回调函数
                                BOOLEAN                 is_sel_folder
                                );


/*****************************************************************************/
//  Description : 在指定目录下搜索文件
//  Return: 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileInPath(
                            const wchar         *path_ptr,      //in, 搜索目录名，如果是NULL，则搜索所有存储器
                            uint16              path_len,
                            MMIFMM_FILTER_T     *filter_ptr,    //筛选参数
                            BOOLEAN             is_recurse,     //搜索是否递归
                            MMIFMM_FUNC_TYPE_E  func_type,      //类型
                            FILEARRAY           file_array,     //out, 输出
                            MMI_WIN_ID_T        win_id,         //如果win_id和msg_id都为0，则采用同步方式搜索文件
                            MMI_MESSAGE_ID_E    msg_id
                            );

/*****************************************************************************/
//  Description : 在指定目录下搜索文件同时排序
//  Return: 
//  Global resource dependence : 
//  Author: yaye.jiang
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileInPathAndSort(
                            const wchar             *path_ptr,      //in, 搜索目录名，如果是NULL，则搜索所有存储器
                            uint16              path_len,
                            MMIFMM_FILTER_T     *filter_ptr,    //筛选参数
                            BOOLEAN             is_recurse,     //搜索是否递归
                            MMIFMM_FUNC_TYPE_E  func_type,      //类型
                            FILEARRAY           file_array,     //out, 输出
                            MMI_WIN_ID_T        win_id,         //如果win_id和msg_id都为0，则采用同步方式搜索文件
                            MMI_MESSAGE_ID_E    msg_id,
                            FILEARRAY_SORT_E    sort_type       //排序类型
                            );

/*****************************************************************************/
//  Description : 在指定目录下搜索文件
//  Return: 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileInPathEx(
                            const wchar        *path_ptr,      //in, 搜索目录名，如果是NULL，则搜索所有存储器
                            uint16             path_len,
							const wchar        *default_path_ptr,//预设路径
                            MMIFMM_FILTER_T     *filter_ptr,    //筛选参数
                            BOOLEAN             is_recurse,     //搜索是否递归
                            MMIFMM_FUNC_TYPE_E  func_type,      //类型
                            FILEARRAY_SORT_E    sort_type,      //排序类型
                            FILEARRAY           file_array,     //out, 输出
                            MMI_WIN_ID_T        win_id,         //如果win_id和msg_id都为0，则采用同步方式搜索文件
                            MMI_MESSAGE_ID_E    msg_id
                            );

//@CR241955 2011.05.27
/*****************************************************************************/
//  Description :在指定目录下搜索文同时指定排序类型
//  Return: 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileInPathBySort(
                            const wchar         *path_ptr,      //in, 搜索目录名，如果是NULL，则搜索所有存储器
                            uint16              path_len,
                            MMIFMM_FILTER_T     *filter_ptr,    //筛选参数
                            BOOLEAN             is_recurse,     //搜索是否递归
                            MMIFMM_FUNC_TYPE_E  func_type,      //类型
                            FILEARRAY_SORT_E    sort_type,      //排序类型
                            FILEARRAY           file_array,     //out, 输出
                            MMI_WIN_ID_T        win_id,         //如果win_id和msg_id都为0，则采用同步方式搜索文件
                            MMI_MESSAGE_ID_E    msg_id
                            );

/*****************************************************************************/
//  Description : 和MMIFMM_SearchFileInPath配合使用
//  Return: 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFileCallBack(void);

/*****************************************************************************/
//  Description : open any fmm win
//  Global resource dependence : none
//  Author: jian.ma
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenExplorerExt(
                                      const wchar * path_ptr,
                                      uint16   path_len,
                                      const wchar * filename,
                                      uint16   file_len,
                                      BOOLEAN  is_need_display,
                                      uint32   find_suffix_type
                                      );

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
                        );

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
                        );

/*****************************************************************************/
//  Description : select a music from fmm
//  Global resource dependence : 
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectMusicWin(MMIFMM_SELECT_INFO_T *select_info);

/*****************************************************************************/
//  Description : select a pic from fmm
//  Global resource dependence : 
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectPictureWin(MMIFMM_SELECT_INFO_T *select_info);

/*****************************************************************************/
//  Description : select a file from fmm
//  Global resource dependence : 
//  Author: 
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectAllFileWin(MMIFMM_SELECT_INFO_T *select_info);

/*****************************************************************************/
//  Description : select a movie from fmm
//  Global resource dependence : 
//  Author: jian.ma
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectMovieWin(MMIFMM_SELECT_INFO_T *select_info);

/*****************************************************************************/
//  Description : close select music window
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseSelectFileWin(void);

/*****************************************************************************/
//  Description : close select music window
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseSelectMusicWin(void);

/*****************************************************************************/
//  Description : close select movie window
//  Global resource dependence : 
//  Author: liqing.peng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_CloseSelectMovieWin(void);

/*****************************************************************************/
//  Description :
//  Return: 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_GetOpenFileWinCurFile(
                                       FILEARRAY_DATA_T *f_array_data
                                       );

/*****************************************************************************/
//  Description : 把搜索类型转化成字符串形式
//  Global resource dependence : 
//  Author:liqingpeng
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_GetFilerInfo(MMIFMM_FILTER_T* filter,uint32 suffix_type);

/*****************************************************************************/
//  Description :
//  Return: 
//  Global resource dependence : 
//  Author: liqing.peng
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetOpenFileWinSoftkey(
                                       MMI_TEXT_ID_T   leftsoft_id,   // the left softkey id
                                       MMI_TEXT_ID_T   middlesoft_id, // the middle softkey id
                                       MMI_TEXT_ID_T   rightsoft_id   // the right softkey id
                                       );

/*****************************************************************************/
//  Description : get icon id by file name
//  Global resource dependence :
//  Author: james.zhang
//  Note:
/*****************************************************************************/
PUBLIC MMIFMM_FILE_TYPE_E MMIAPIFMM_GetFileTypeByFileName(
                                                       const wchar  *file_name,
                                                       uint16       file_name_len
                                                       );

#if defined FONT_TYPE_SUPPORT_VECTOR && defined FILE_FONT_SUPPORT
/*****************************************************************************/
//  Description : select a font from fmm
//  Global resource dependence : 
//  Author: james.zhang
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIFMM_OpenSelectFontWin(MMIFMM_SELECT_INFO_T *select_info);
#endif

#ifdef CMCC_UI_STYLE
/*****************************************************************************/
//  Description : open music file
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenMusicFileList(void);

/*****************************************************************************/
//  Description : open movie file
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenMovieFileList(void);
#endif //CMCC_UI_STYLE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                                    Start of Atest  Function  Declare                                                     */
/* Description:  Hanlde at cmd for fmm auto test cases                                                           */
/* Author: Shuyan.hu                                                                                                          */
/* Date: 30/12/2010                                                                                                           */ 
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifdef MMI_SYNCTOOL_SUPPORT
/*****************************************************************************/
//  Description : create folders in same path
//  Global resource dependence : s_fmm_current_path
//  Parameter: folders number
//  Author: shuyan.hu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN  Atest_FMMCreateFolderHori(uint16 hori_num);

/*****************************************************************************/
//  Description : create folders across path
//  Global resource dependence : s_fmm_current_path
//  Parameter: folders number
//  Author: shuyan.hu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN  Atest_FMMCreateFolderVerti(uint16 Veri_num);
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                                    End of Atest  Function  Declare                                                       */
/* Description:  Hanlde at cmd for fmm auto test cases                                                           */
/* Author: Shuyan.hu                                                                                                          */
/* Date: 30/12/2010                                                                                                           */ 
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#endif

#ifdef DRM_SUPPORT
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
PUBLIC BOOLEAN MMIAPIFMM_SetSelDrmLimitValue(MMIFMM_DRM_LIMIT_TYPE_T limit_value);
#endif

#ifdef SEARCH_SUPPORT
/*****************************************************************************/
//  Description : 文件搜索结果的打开函数 
//  Global resource dependence : 
//  Author: hermann liu
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_OpenMatchedItem(uint32 user_data);
/*****************************************************************************/
// 	Description : 文件搜索结果的显示回调函数
//	Global resource dependence : 
//    Author:hermann liu
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SetMatchItemData(
                            GUILIST_NEED_ITEM_DATA_T *need_item_data_ptr,
                            uint32 user_data
                             );
/*****************************************************************************/
// 	Description : start search for local search
//	Global resource dependence : 
//    Author:hermann liu
//	Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_StartLocalSearch(
                                                    MMI_STRING_T  *keyword_str_ptr,
                                                    MMI_WIN_ID_T        notice_win_id,         //如果win_id和msg_id都为0，则采用同步方式搜索文件
                                                    MMI_MESSAGE_ID_E    notice_msg_id);
/*****************************************************************************/
//  Description : free file search resource 
//  Global resource dependence : 
//  Author: hermann liu
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPIFMM_FreeLocalSearchResource(void);
/*****************************************************************************/
//  Description : get file search matched num 
//  Global resource dependence : 
//  Author: hermann liu
//  Note: 
/*****************************************************************************/
PUBLIC uint32 MMIAPIFMM_GetFileSearchMatchedNum(void);
#endif
/*****************************************************************************/
//  Description : move file call back
//  Global resource dependence : none
//  Author: juan.wu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_MoveCallBack(uint16 result);
/*****************************************************************************/
//  Description : copy is from move
//  Global resource dependence : none
//  Author: juan.wu
//  Note:如果从move呼叫copy,则为TRUE
/*****************************************************************************/
PUBLIC BOOLEAN MMIFMM_IsCopyFromMove(void);
/*****************************************************************************/
//  Description : has exist file when move
//  Global resource dependence : none
//  Author: juan.wu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_MoveFileExist(void);

/*****************************************************************************/
//  Description : 
//  Global resource dependence : none
//  Author: juan.wu
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIFMM_SearchFile(                            
                                const wchar             *path_ptr,      //in, 搜索目录名，如果是NULL，则搜索所有存储器
                                uint16              path_len,
                                MMIFMM_FILTER_T     *filter_ptr,    //筛选参数
                                MMIFMM_FUNC_TYPE_E  func_type,      //类型
                                MMIFMM_SEARCH_FILE_INFO_T * search_file_ptr
                                );
                                
/*****************************************************************************/
//  Description : 打开一个窗口, 显示某一级目录, 并把当前目录下所有文件夹和
//                所有符合筛选条件的文件列出来
//  Return: 用户在窗口中选中文件后, 通过回调函数返回结果给上一级窗口
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
MMIFMM_OPENWIN_RET_E MMIAPIFMM_OpenFileWinByLayerEx(MMIFMM_OPEN_LAYER_WIN_INFO_T *open_layer_info);  

/**---------------------------------------------------------------------------*
 **                         Macro Declaration    				   *
 **---------------------------------------------------------------------------*/
/*****************************************************************************/
//  如下宏定义用以实现对外接口更名前后的兼容
/*****************************************************************************/
#ifdef MMI_FUNC_COMPATIBLE_SUPPORT

#define MMIFMM_ConvertFileType					 MMIAPIFMM_ConvertFileType

#define MMIFMM_ConvertMovieFileType	 		 MMIAPIFMM_ConvertMovieFileType

#define MMIFMM_ConvertMusicFileType 	 		 MMIAPIFMM_ConvertMusicFileType

#define MMIFMM_CopyCallBack		 			 MMIAPIFMM_CopyCallBack

#define MMIFMM_CopyFileExistInform 				 MMIAPIFMM_CopyFileExistInform

#define MMIFMM_UpdateCurrentFileLIst	 		 MMIAPIFMM_UpdateCurrentFileLIst

#define MMIFMM_IsDevEnoughSpace			 	 MMIAPIFMM_IsDevEnoughSpace

#define MMIFMM_GetPictureWidthHeight	 		 MMIAPIFMM_GetPictureWidthHeight

#define MMIAPIMULTIM_Init				 		 MMIAPIFMM_InitFolders

#define MMIAPIMULTIM_InitWallPaper		 		 MMIAPIFMM_InitWallPaper

#define MMIAPIMULTIM_SetPictureToWallpaper		 MMIAPIFMM_SetPictureToWallpaper

#define MMIAPIMULTIM_SetPictureToWallpaperEx	 MMIAPIFMM_SetPictureToWallpaperEx

#define MMIAPIMULTIM_SetMusicToRing			 MMIAPIFMM_SetMusicToRing

#define MMIAPIMULTIM_IsWallPaperFile			 MMIAPIFMM_IsWallPaperFile

#define MMIFMM_ChangeWallPaperFileName		 MMIAPIFMM_ChangeWallPaperFileName

#define MMIAPIMULTIM_CloseWaitSetWallpaper		 MMIAPIFMM_CloseWaitSetWallpaper

#define MMIAPIMULTIM_HandleWallpaperWaitMsg	 MMIAPIFMM_HandleWallpaperWaitMsg

#define MMIAPIMULTIM_OpenWaitSetWallpaper		 MMIAPIFMM_OpenWaitSetWallpaper

#define MMIMULTIM_GetWallPaperInfo			 MMIAPIFMM_GetWallPaperInfo

#define MMIMULTIM_SetRandomNameEditbox		 MMIAPIFMM_SetRandomNameEditbox

#define MMIFMM_PromptFileErrorType				 MMIAPIFMM_PromptFileErrorType

#define MMIFMM_DelCallBack					 MMIAPIFMM_DelCallBack

#define MMIFILE_InitSystemFolder				 MMIAPIFMM_InitSystemFolder

#define MMIFILE_GetDefaultDisk					 MMIAPIFMM_GetDefaultDisk

#define MMIFILE_IsEnoughSpace					 MMIAPIFMM_IsEnoughSpace

#define MMIFILE_GetSysFileFullPathForReadEx		 MMIAPIFMM_GetSysFileFullPathForReadEx

#define MMIFILE_GetSysFileFullPathForWriteEx	         MMIAPIFMM_GetSysFileFullPathForWriteEx

#define MMIFILE_GetSysFileFullPathForRead		 MMIAPIFMM_GetSysFileFullPathForRead

#define MMIFILE_GetSysFileFullPathForWrite		 MMIAPIFMM_GetSysFileFullPathForWrite

#define MMIFILE_CreateSysFileDir				 MMIAPIFMM_CreateSysFileDir

#define MMIFILE_CombineSysPath				MMIAPIFMM_CombineSysPath

#define MMIFMM_PreviewPicture					 MMIAPIFMM_PreviewPicture

#define MMIFMM_PreviewVideo					 MMIAPIFMM_PreviewVideo

#define MMIFMM_SetFolderDetail					 MMIAPIFMM_SetFolderDetail

#define MMIFMM_SetFolderDetailCNF				 MMIAPIFMM_SetFolderDetailCNF

#define MMIFMM_GetDrmIconId 					 MMIAPIFMM_GetDrmIconId

#define MMIFMM_UpdateListIconData				 MMIAPIFMM_UpdateListIconData

#define MMIFMM_UpdateSelectListIconData		 MMIAPIFMM_UpdateSelectListIconData

//////////////////////////////////define for mmi_filemgr.h   ////////////////////////////////////////

#define MMIFILE_SplitLastName					 MMIAPIFMM_SplitLastName

#define MMIFILE_SplitFullPath 					 MMIAPIFMM_SplitFullPath

#define MMIFILE_SplitFullPathExt				         MMIAPIFMM_SplitFullPathExt

#define MMIFILE_CombineFullPath				 MMIAPIFMM_CombineFullPath

#define MMIFILE_CombineFullPathEx				 MMIAPIFMM_CombineFullPathEx

#define MMIFILE_SplitFileName					 MMIAPIFMM_SplitFileName

#define MMIFMM_GetFileSizeString				 MMIAPIFMM_GetFileSizeString

#define MMIFILE_Init							 MMIAPIFMM_InitFileTasks

#define MMIFILE_RegisterDevice					 MMIAPIFMM_RegisterDevice

#define MMIFILE_UnRegisterDevice				 MMIAPIFMM_UnRegisterDevice

#define MMIFILE_FormatDevice					 MMIAPIFMM_FormatDevice

#define MMIFILE_FormatDeviceAsyn				 MMIAPIFMM_FormatDeviceAsyn

#define MMIFILE_GetDeviceStatus				 MMIAPIFMM_GetDeviceStatus

#define MMIAPISD_GetStatus					 MMIAPIFMM_GetDeviceTypeStatus

#define MMIFILE_GetDeviceName					 MMIAPIFMM_GetDeviceName

#define MMIFILE_GetAlertTextIDByDev			 MMIAPIFMM_GetAlertTextIDByDev

#define MMIFILE_GetDeviceMemStatusText		 MMIAPIFMM_GetDeviceMemStatusText

#define MMIFILE_GetDeviceCopyText				 MMIAPIFMM_GetDeviceCopyText

#define MMIFILE_GetDevicePath					 MMIAPIFMM_GetDevicePath

#define MMIFILE_GetDevicePathLen				 MMIAPIFMM_GetDevicePathLen

#define MMIFILE_GetSuitableFileDev				 MMIAPIFMM_GetSuitableFileDev

#define MMIFILE_CreateDir						 MMIAPIFMM_CreateDir

#define MMIFILE_SetAttr						 MMIAPIFMM_SetAttr

#define MMIFILE_GetFileInfo						 MMIAPIFMM_GetFileInfo

//#define MMIFILE_IsFileExist						 MMIAPIFMM_IsFileExist

//#define MMIFILE_IsFolderExist					 MMIAPIFMM_IsFolderExist

#define MMIFILE_GetDeviceFreeSpace				 MMIAPIFMM_GetDeviceFreeSpace

#define MMIFILE_GetDeviceUsedSpace				 MMIAPIFMM_GetDeviceUsedSpace

#define MMIFILE_RenameFile					 MMIAPIFMM_RenameFile

#define MMIFILE_GetDeviceTypeByPath			 MMIAPIFMM_GetDeviceTypeByPath

#define MMIFILE_ReadFilesDataSyn				 MMIAPIFMM_ReadFilesDataSyn

#define MMIFILE_WriteFilesDataSyn				 MMIAPIFMM_WriteFilesDataSyn

#define MMIFILE_DeleteFileSyn					 MMIAPIFMM_DeleteFileSyn.

#define MMIFILE_CopyFile						 MMIAPIFMM_CopyFile

#define MMIFILE_ReadFileDataByOffsetSyn			 MMIAPIFMM_ReadFileDataByOffsetSyn

#define MMIFILE_ReadFilesDataAsyn				 MMIAPIFMM_ReadFilesDataAsyn

#define MMIFILE_ReadFilesDataAsynEx			 MMIAPIFMM_ReadFilesDataAsynEx

#define MMIFILE_WriteFilesDataAsyn				 MMIAPIFMM_WriteFilesDataAsyn

#define MMIFILE_ReadFilesDataAsynEx			 MMIAPIFMM_ReadFilesDataAsynEx

#define MMIFILE_WriteFileDataByOffsetAsyn		 MMIAPIFMM_WriteFileDataByOffsetAsyn

#define MMIFILE_DeleteFileAsyn					 MMIAPIFMM_DeleteFileAsyn

#define MMIFILE_OperationAsynCnf				 MMIAPIFMM_OperationAsynCnf

#define MMIFILE_CopyOneFile					 MMIAPIFMM_CopyOneFile

#define MMIFILE_CopyReadSrc					 MMIAPIFMM_CopyReadSrc

#define MMIFILE_CopyWriteDest					 MMIAPIFMM_CopyWriteDest

#define MMIFILE_IsEndOfFile					 MMIAPIFMM_IsEndOfFile

#define MMIFILE_GetChar						 MMIAPIFMM_GetChar

//#define MMIFILE_CreateFile						 MMIAPIFMM_CreateFile

#define MMIFILE_CreateFileAsyn					 MMIAPIFMM_CreateFileAsyn

//#define MMIFILE_DeleteFile						 MMIAPIFMM_DeleteFile

#define MMIFILE_CreateDirectory					 MMIAPIFMM_CreateDirectory

#define MMIFILE_DeleteTreeEx					 MMIAPIFMM_DeleteTreeEx

#define MMIFILE_DeleteTree						 MMIAPIFMM_DeleteTree

//#define MMIFILE_DeleteDirectory					 MMIAPIFMM_DeleteDirectory

//#define MMIFILE_FindFirstFile					 MMIAPIFMM_FindFirstFile

//#define MMIFILE_FindNextFile					 MMIAPIFMM_FindNextFile

//#define MMIFILE_FindClose						 MMIAPIFMM_FindClose

//#define MMIFILE_GetFileSize						 MMIAPIFMM_GetFileSize

#define MMIFILE_SetFileSize						 MMIAPIFMM_SetFileSize

//#define MMIFILE_ReadFile						 MMIAPIFMM_ReadFile

//#define MMIFILE_WriteFile						 MMIAPIFMM_WriteFile

#define MMIFILE_SetFilePointer					 MMIAPIFMM_SetFilePointer

#define MMIFILE_GetFilePointer					 MMIAPIFMM_GetFilePointer

//#define MMIFILE_CloseFile						 MMIAPIFMM_CloseFile

#define MMIFILE_GetValidateDevice				 MMIAPIFMM_GetValidateDevice

#define MMIAPIFILE_Callback					 MMIAPIFMM_Callback

#define MMIFILE_AppendFile						 MMIAPIFMM_AppendFile

#define MMIFILE_GetMaxFreeDeviceType			 MMIAPIFMM_GetMaxFreeDeviceType

#define MMIFILE_GetFirstValidDevice				 MMIAPIFMM_GetFirstValidDevice

#define MMIFILE_GetFirstValidSD					 MMIAPIFMM_GetFirstValidSD

#define MMIFILE_GetFirstExistDevice				 MMIAPIFMM_GetFirstExistDevice

#define MMIFILE_GetDeviceFreeSpaceText			 MMIAPIFMM_GetDeviceFreeSpaceText

#define MMIFILE_GetOSDImageIDByDeviceType	 MMIAPIFMM_GetOSDImageIDByDeviceType

#define MMIFILE_GetStorageIcon					 MMIAPIFMM_GetStorageIcon

#ifdef DRM_SUPPORT
#define MMIFILE_IsDrmFileBySuffix				 MMIAPIFMM_IsDrmFileBySuffix
#endif

//////////////////////////////////      mmifmm_interface.h    ///////////////////////////////////

#define MMIFMM_GetIconIdByFileType				 MMIAPIFMM_GetIconIdByFileType

#define MMIFMM_OpenFileWinByLayer				 MMIAPIFMM_OpenFileWinByLayer

#define MMIFMM_SearchFileInPath				 MMIAPIFMM_SearchFileInPath

#define MMIFMM_SearchFileInPathEx				 MMIAPIFMM_SearchFileInPathEx

#define MMIFMM_SearchFileInPathBySort			 MMIAPIFMM_SearchFileInPathBySort

#define MMIFMM_SearchFileCallBack			         MMIAPIFMM_SearchFileCallBack

#define MMIFMM_GetOpenFileWinCurFile			 MMIAPIFMM_GetOpenFileWinCurFile
 
#define MMIFMM_GetFilerInfo					 MMIAPIFMM_GetFilerInfo

#define MMIFMM_SetOpenFileWinSoftkey			 MMIAPIFMM_SetOpenFileWinSoftkey

#define MMIFMM_GetFileTypeByFileName			 MMIAPIFMM_GetFileTypeByFileName

#define FILE_OpenMatchedItem					 MMIAPIFMM_OpenMatchedItem

#define FILE_SetMatchItemData					 MMIAPIFMM_SetMatchItemData

#define MMIAPIFILE_StartLocalSearch				 MMIAPIFMM_StartLocalSearch

#define MMIAPIFILE_FreeLocalSearchResource		 MMIAPIFMM_FreeLocalSearchResource

#define MMIAPIFILE_GetFileSearchMatchedNum		 MMIAPIFMM_GetFileSearchMatchedNum

#endif //MMI_FUNC_COMPATIBLE_SUPPORT

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif
