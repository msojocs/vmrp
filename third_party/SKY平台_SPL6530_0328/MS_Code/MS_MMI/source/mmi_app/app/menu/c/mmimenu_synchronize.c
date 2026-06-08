/*****************************************************************************
** File Name:      mmimenu_syhchronize.c                                      *
** Author:         jin.wang                                                          *
** Date:           05/20/2011                                                *
** Copyright:      2004 Spreadtrum, Incoporated. All Rights Reserved.         *
** Description:    This file is used to describe punctuation                 *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 05/2011      jin.wang              Creat
******************************************************************************/

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_app_menu_trc.h"
#include "img_dec_interface.h"

#include "guires.h"
#include "guiimg.h"
#include "version.h"

#include "mmiidle_export.h"
#include "mmi_menutable.h"
#include "mmimenu_export.h"
#include "mmimenu_internal.h"

#include "mmi_theme.h"
#include "mmitheme_special.h"

#include "mmimenu_synchronize.h"
#include "mmi_nv.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmiudisk_export.h" 
#ifdef SEARCH_SUPPORT
#include "mmisearch_export.h"
#endif
#ifdef DYNAMIC_MAINMENU_SUPPORT

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
//extern const char* VERSION_GetInfo(uint32 type);

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
//动态菜单信息
GUIMENU_MAINMENU_DYNAMIC_T s_dy_data_array[GUIMENU_MAINMENU_STORE_STYLE_NUM] = {0};
//菜单图片数据
uint8 *s_icon_data_array_ptr[GUIMENU_MAINMENU_STORE_STYLE_NUM] = {PNULL};

/*---------------------------------------------------------------------------*/
/*                         TYPE AND CONSTANT                                 */
/*---------------------------------------------------------------------------*/
typedef uint16 GUIMENU_PAGE_POS_T[MMITHEME_MENUMAIN_SLIDE_PAGE_NUM][MMITHEME_MENUMAIN_SLIDE_ITEM_MAX_NUM];

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/


/*****************************************************************************/
// Description :判断动态菜单是否需要同步
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsNeedSyn(void);

/*****************************************************************************/
// Description : get dev info and check if have enough space
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsDevEnoughSpace(GUIMENU_FILE_DEVICE_E dev);

/*****************************************************************************/
// Description : 判断item是否已经存在
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsItemExist(
                          GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
                          );

/*****************************************************************************/
// Description : 验证信息是否正确
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsValidDyMenuInfo(
                                GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
                                );

/*****************************************************************************/
// Description : 验证是否第一次存储
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsFirstTimeAppend(
    GUIMENU_MAINMENU_DYNAMIC_T* all_dy_data_ptr
);

/*****************************************************************************/
// Description : 根据存储方式，读取所有动态菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN ReadDevMenuData(
    uint8 **icon_data_pptr,
    GUIMENU_MAINMENU_DYNAMIC_T *dy_data_ptr,
    GUIMENU_FILE_DEVICE_E store_idx
);

/*****************************************************************************/
// Description : 读取所有菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetAllDyMenuData(uint32 *total_dy_menu_ptr);

/*****************************************************************************/
// Description : 读取菜单解码后图片
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL uint8* GetDecodeIconData(
    GUIMENU_FILE_DEVICE_E store_dev,
    uint16 item_idx
);


/*****************************************************************************/
// Description :存储动态菜单参数
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN StoreParamData(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    GUIMENU_FILE_DEVICE_E store_idx
);

/*****************************************************************************/
// Description : 把动态菜单info数据存入文件
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN StoreInfoData(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    uint32 total_item_num,
    BOOLEAN is_new_item,//是否有新增item
    GUIMENU_FILE_DEVICE_E store_idx,
    BOOLEAN is_first_time,//是否第一次存储
    uint8 *icon_data_ptr,   //图片数据
    GUIMENU_MAINMENU_DYNAMIC_T *all_dy_data_ptr //动态数据信息
);

/*****************************************************************************/
// Description : 修改动态菜单位置信息
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL void ResetItemPosData(
    GUIMENU_MAINMENU_DYNAMIC_T *dy_data_ptr,
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    uint16 page_num,
    BOOLEAN is_move_state,//是否是因为移动位置而改变
    GUIMENU_FILE_DEVICE_E store_index
);

/*****************************************************************************/
// Description : 根据类型存储动态菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN WriteDataInfo(
    GUIMENU_FILE_DEVICE_E store_idx,
    uint8* file_data_ptr,
    uint32 file_data_size,
    uint32 check_info,
    BOOLEAN is_need_set_syn
);

/*****************************************************************************/
// Description: 把已经安装好的动态菜单数据合并进主菜单
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL int32 InsertDyItem(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    MMI_IMAGE_ID_T select_icon_id
);

/*****************************************************************************/
// Description: 清除主菜单中可能存在的空白项
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void TrimPageInfo(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    GUIMENU_PAGE_POS_T pos_info_pptr,
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    uint16 page_max_cnt,
    uint16 item_max_cnt
);

/*****************************************************************************/
// Description: 获取主菜单页面信息
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void GetMenuPageInfo(
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    uint16 *max_page_ptr,
    uint16 *max_item_ptr
);

/*****************************************************************************/
// Description: 把动态菜单按位置编号放回主菜单
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void InsertDyItemByPos(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    uint16 *pos_info_ptr,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dy_data_ptr
);

/*****************************************************************************/
// Description : 查找mainmenu中的空缺位置
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetEmptyInMenu(
    MMIMAINMENU_PAGE_ITEM_INFO_T const *item_page_ptr,//[in]静态菜单数据信息
    uint16 *page_cnt_ptr,
    uint16 *page_idx_ptr,
    uint16 *item_idx_ptr,
    uint16 page_max_cnt,
    uint16 item_max_cnt
);

/*****************************************************************************/
// Description: 清除主菜单中的空白页
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void DeleteEmptyPage(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    uint16 *page_count_ptr //[in/out]
);

/*****************************************************************************/
// Description : 从文件系统中删除动态菜单信息
// Global resource dependence :
// Author:jin.wang
// Note:
//  1 delete - pr1, pr2, icon(dec, ori)
//  2 modify - check info, count, data
/*****************************************************************************/
LOCAL BOOLEAN DeleteItemData(
    GUIMENU_FILE_DEVICE_E store_idx,
    GUIMENU_MAINMENU_DYNAMIC_T *dy_data_ptr,
    uint8* icon_ptr,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
);

/*****************************************************************************/
// Description : 将图标解码后存储
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN DecodeIconData(
    GUIMENU_FILE_DEVICE_E store_idx,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    MMIMENU_SYN_ICON_T *scaled_info_ptr
);

/*****************************************************************************/
// Description : 将图标放大到指定尺寸
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN ZoomOutIconData(
    GUIIMG_DISPLAY_T img_display,
    GUIIMG_DEC_OUT_T *img_output_ptr,
    MMIMENU_SYN_ICON_T   *scaled_page_ptr
);

/*****************************************************************************/
// Description : 根据menu_id获取动态菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetItemInfo(
    MMI_MENU_ID_T menu_id,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr,
    GUIMENU_GET_INFO_T get_type,
    GUIMENU_READFILE_TYPE icon_type
);

/*****************************************************************************/
// Description : 根据文件名生成动态菜单数据文件的完整路径
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetPathByFilename(
    GUIMENU_FILE_DEVICE_E store_idx,
    const wchar *file_name_ptr,
    uint16 file_name_len,
    wchar *full_path_name_ptr,    //[OUT]
    uint16 *full_path_len_ptr     //[OUT]
);

/*****************************************************************************/
// Description : 获取数据文件名
// Global resource dependence : none
// Author: jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN GetFilename_Data(wchar *data_file_name_ptr);

/*****************************************************************************/
// Description : 读取文件系统中动态菜单图标启动参数
// Global resource dependence : none
// Author: jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL void GetFilename_Param(
    MMI_MENU_ID_T menu_id,
    wchar *parm1_name_ptr,   //[IN/OUT]
    wchar *parm2_name_ptr   //[IN/OUT]
);

/*****************************************************************************/
// Description : 获取图片文件名, 获取解码后和原始文件名
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetFilename_Icon(
    MMI_MENU_ID_T menu_id,
    uint16 read_style,
    wchar *ori_file_name_ptr,   //[IN/OUT]
    wchar *decode_file_name_ptr   //[IN/OUT]
);

/*****************************************************************************/
// Description : 从文件系统中删除动态菜单数据
// Global resource dependence : none
// Author: jin.wang
// Note: (所有菜单数据集合的文件)
/*****************************************************************************/
LOCAL BOOLEAN DeleteInfoData(GUIMENU_FILE_DEVICE_E store_idx);

/*****************************************************************************/
// Description : 从文件系统中删除指定文件
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN DeleteDevFile(
    wchar *data_file_name_ptr,
    GUIMENU_FILE_DEVICE_E store_idx
);
/*****************************************************************************/
// Description : 从文件系统中删除动态菜单原始显示数据
// Global resource dependence : none
// Author: nan.ji
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN DeleteOriFile(
    GUIMENU_FILE_DEVICE_E store_idx,
    MMI_MENU_ID_T menu_id
);
/*****************************************************************************/
// Description : 从文件系统中删除动态菜单启动参数数据
// Global resource dependence : none
// Author: jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN DeleteParamFile(
    GUIMENU_FILE_DEVICE_E store_idx,
    MMI_MENU_ID_T menu_id
);

/*****************************************************************************/
// Description : 往文件系统中写数据
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN WriteDataToFile(
    wchar *data_name_ptr,
    void* data_ptr,
    uint32 data_size,
    GUIMENU_FILE_DEVICE_E store_idx
);

/*****************************************************************************/
// Description : 从文件系统中读数据
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL uint8* ReadDataFromFile(
    wchar const *file_name_ptr,
    GUIMENU_FILE_DEVICE_E store_dev
);

/*****************************************************************************/
// Description : 获取要删除的动态菜单menu_id
// Global resource dependence :
// Author:jin.wang
// Note:
// dynamic_item_ptr:要删除的菜单信息
/*****************************************************************************/
LOCAL BOOLEAN GetDeleteItemMenuId(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_all_item_ptr,
    uint32 dynamic_menu_num
);

/*****************************************************************************/
// Description : 根据list_index获取动态菜单menu_id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetListItemMenuId(
    uint16 list_index,
    MMI_MENU_ID_T *menu_id_ptr
);

/*****************************************************************************/
// Description : 根据存储方式，读取所有动态菜单数据的check_info
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL uint32 GetDevCheckInfo(GUIMENU_FILE_DEVICE_E store_idx);

/*****************************************************************************/
// Description : 根据存储方式，设置文件的checkinfo
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL void SetDataCheckInfo(
    GUIMENU_FILE_DEVICE_E store_idx,
    uint32 check_info
);

/*****************************************************************************/
// Description : 获取动态菜单的位置信息
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetItemDispPos(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    MMI_MENU_ID_T dy_menu_id,
    uint16 page_cnt,
    uint16 *page_index_ptr,//[out]
    uint16 *item_index_ptr //[out]
);

/*****************************************************************************/
// Description : 释放动态菜单数据数组内存
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL void ResetData(void);

/*****************************************************************************/
// Description : 生成check标志
// Global resource dependence :
// Author:jin.wang
// Note: arm下用时间，win32下用随机数
/*****************************************************************************/
LOCAL uint32 GenCheckInfo(void);

/*****************************************************************************/
// Description : 为动态菜单创建一个menu_id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL uint32 GenMenuId(void);

/*****************************************************************************/
// Description : 根据版本编译时间创建动态菜单校验码
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL uint32 GenVersionInfo(void);

/*****************************************************************************/
// Description : 判定运行的数据是否合法
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsRunInfoValid(GUI_MENU_MAINMENU_DYNAMIC_DATA_T *data_ptr);

/*****************************************************************************/
// Description : 设置高亮图片id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL void SetSelectedId(MMI_IMAGE_ID_T select_icon_id);

/*****************************************************************************/
//  Description : 显示文件系统中存储的图片信息
//  Global resource dependence : 
//  Author: jin.wang
//  Note:
/*****************************************************************************/
LOCAL void DisplayDyMenuIconInFile(
                            GUIMENU_MAINMENU_DY_ITEM_T *item_ptr, 
                            GUI_RECT_T *disp_rect_ptr,
                            GUI_LCD_DEV_INFO *lcd_dev_ptr 
                            );

/*****************************************************************************/
// Description : 设置语言，menu需要先转化下
// Global resource dependence :
// Author:nan.ji
// Note:
/*****************************************************************************/
LOCAL MMISET_LANGUAGE_TYPE_E GetLangType(
    MMIMENU_LANGUAGE_TYPE_E lang_info
);
/*****************************************************************************/
// Description : 设置语言，menu需要先转化下
// Global resource dependence :
// Author:nan.ji
// Note:
/*****************************************************************************/
LOCAL MMISET_LANGUAGE_TYPE_E GetLangType(
    MMIMENU_LANGUAGE_TYPE_E lang_info
)
{
    MMISET_LANGUAGE_TYPE_E lang_type = MMISET_LANGUAGE_ENGLISH;
	switch(lang_info)
	{
	case MMIMENU_LANGUAGE_ENGLISH:         //英文
        lang_type = MMISET_LANGUAGE_ENGLISH;
		break;
	case  MMIMENU_LANGUAGE_SIMP_CHINESE:   //中文简体 
		lang_type =  MMISET_LANGUAGE_SIMP_CHINESE;
		break; 
	case  MMIMENU_LANGUAGE_TRAD_CHINESE:   //中文繁体
		lang_type =  MMISET_LANGUAGE_TRAD_CHINESE;
		break; 
	case  MMIMENU_LANGUAGE_ARABIC:         //阿拉伯
		lang_type =   MMISET_LANGUAGE_ARABIC;
		break; 
	case  MMIMENU_LANGUAGE_FRENCH:         //法语
		lang_type =   MMISET_LANGUAGE_FRENCH;
		break; 
	case MMIMENU_LANGUAGE_HINDI:          //印第
		lang_type =   MMISET_LANGUAGE_HINDI;
		break; 
	case MMIMENU_LANGUAGE_HUNGARIAN:      //匈牙利
		lang_type =   MMISET_LANGUAGE_HUNGARIAN;
		break; 
	case MMIMENU_LANGUAGE_INDONESIAN:     //印度尼西亚
		lang_type =   MMISET_LANGUAGE_INDONESIAN;
		break; 
    case MMIMENU_LANGUAGE_MALAY:          //马来
		lang_type =   MMISET_LANGUAGE_MALAY;
		break;  
	case MMIMENU_LANGUAGE_PORTUGUESE:     //葡萄牙
		lang_type =   MMISET_LANGUAGE_PORTUGUESE;
		break;  
	case MMIMENU_LANGUAGE_RUSSIAN:        //俄语
		lang_type =   MMISET_LANGUAGE_RUSSIAN;
		break;  
	case MMIMENU_LANGUAGE_SPANISH:        //西班牙
		lang_type =   MMISET_LANGUAGE_SPANISH;
		break;  
	case MMIMENU_LANGUAGE_TAGALOG:        //他加诺
		lang_type =   MMISET_LANGUAGE_TAGALOG;
		break; 
	case MMIMENU_LANGUAGE_THAI:           //泰语
		lang_type =   MMISET_LANGUAGE_THAI;
		break; 
	case MMIMENU_LANGUAGE_VIETNAMESE:     //越南语
		lang_type =   MMISET_LANGUAGE_VIETNAMESE;
		break;  
	case MMIMENU_LANGUAGE_URDU:           //乌尔都
		lang_type =   MMISET_LANGUAGE_URDU;
		break;  
	case MMIMENU_LANGUAGE_ITALIAN:        //意大利
		lang_type =   MMISET_LANGUAGE_ITALIAN;
		break; 
	case MMIMENU_LANGUAGE_PERSIAN:        //波斯
		lang_type =   MMISET_LANGUAGE_PERSIAN;
		break; 
	case MMIMENU_LANGUAGE_TURKISH:        //土耳其
		lang_type =   MMISET_LANGUAGE_TURKISH;
		break;  
	case MMIMENU_LANGUAGE_GERMAN:         //德语
		lang_type =   MMISET_LANGUAGE_GERMAN;
		break; 
	case MMIMENU_LANGUAGE_GREEK:          //希腊
		lang_type =   MMISET_LANGUAGE_GREEK;
		break; 
	case MMIMENU_LANGUAGE_HEBREW:         //希伯莱
		lang_type =   MMISET_LANGUAGE_HEBREW;
		break;  
	case MMIMENU_LANGUAGE_BENGALI:        //孟加拉
		lang_type =   MMISET_LANGUAGE_BENGALI;
		break; 
	case MMIMENU_LANGUAGE_CZECH:          //捷克
		lang_type =   MMISET_LANGUAGE_CZECH;
		break; 
	case MMIMENU_LANGUAGE_SLOVENIAN:      //斯诺文尼亚
		lang_type =   MMISET_LANGUAGE_SLOVENIAN;
		break;  
	case MMIMENU_LANGUAGE_ROMANIAN:       //罗马尼亚
		lang_type =   MMISET_LANGUAGE_ROMANIAN;
		break; 
	case MMIMENU_LANGUAGE_TELUGU:
		lang_type =   MMISET_LANGUAGE_TELUGU;
		break;  
	case MMIMENU_LANGUAGE_MARATHI:
		lang_type =   MMISET_LANGUAGE_MARATHI;
		break;  
	case MMIMENU_LANGUAGE_TAMIL:
		lang_type =   MMISET_LANGUAGE_TAMIL;
		break; 
	case MMIMENU_LANGUAGE_GUJARATI:
		lang_type =   MMISET_LANGUAGE_GUJARATI;
		break; 
	case MMIMENU_LANGUAGE_KANNADA:
		lang_type =   MMISET_LANGUAGE_KANNADA;
		break;  
	case MMIMENU_LANGUAGE_MALAYALAM:
		lang_type =   MMISET_LANGUAGE_MALAYALAM;
		break; 
	case MMIMENU_LANGUAGE_ORIYA:
		lang_type =   MMISET_LANGUAGE_ORIYA;
		break; 
	case MMIMENU_LANGUAGE_PUNJABI:
		lang_type =   MMISET_LANGUAGE_PUNJABI;
		break;  
	case MMIMENU_LANGUAGE_AFRIKAANS:
		lang_type =   MMISET_LANGUAGE_AFRIKAANS;
		break;  
	case MMIMENU_LANGUAGE_ALBANIAN:
		lang_type =   MMISET_LANGUAGE_ALBANIAN;
		break; 
	case MMIMENU_LANGUAGE_ARMENIAN:
		lang_type =   MMISET_LANGUAGE_ARMENIAN;
		break; 
	case MMIMENU_LANGUAGE_AZERBAIJANI:
		lang_type =   MMISET_LANGUAGE_AZERBAIJANI;
		break; 
	case MMIMENU_LANGUAGE_BASQUE:
		lang_type =   MMISET_LANGUAGE_BASQUE;
		break; 
	case MMIMENU_LANGUAGE_BULGARIAN:
		lang_type =   MMISET_LANGUAGE_BULGARIAN;
		break; 
	case MMIMENU_LANGUAGE_CATALAN:
		lang_type =   MMISET_LANGUAGE_CATALAN;
		break;  
	case MMIMENU_LANGUAGE_CROATIAN:
		lang_type =   MMISET_LANGUAGE_CROATIAN;
		break;  
	case MMIMENU_LANGUAGE_DANISH:
		lang_type =   MMISET_LANGUAGE_DANISH;
		break;  
	case MMIMENU_LANGUAGE_DUTCH:
		lang_type =   MMISET_LANGUAGE_DUTCH;
		break; 
	case MMIMENU_LANGUAGE_ESTONIAN:
		lang_type =   MMISET_LANGUAGE_ESTONIAN;
		break; 
	case MMIMENU_LANGUAGE_FILIPINO:
		lang_type =   MMISET_LANGUAGE_FILIPINO;
		break; 
	case MMIMENU_LANGUAGE_FINNISH:
		lang_type =   MMISET_LANGUAGE_FINNISH;
		break; 
	case MMIMENU_LANGUAGE_GALICIAN:
		lang_type =   MMISET_LANGUAGE_GALICIAN;
		break; 
	case MMIMENU_LANGUAGE_GEORGIAN:
		lang_type =   MMISET_LANGUAGE_GEORGIAN;
		break; 
	case MMIMENU_LANGUAGE_HAUSA:
		lang_type =   MMISET_LANGUAGE_HAUSA;
		break; 
	case MMIMENU_LANGUAGE_ICELANDIC:
		lang_type =   MMISET_LANGUAGE_ICELANDIC;
		break; 
	case MMIMENU_LANGUAGE_IGBO:
		lang_type =   MMISET_LANGUAGE_IGBO;
		break; 
	case MMIMENU_LANGUAGE_IRISH:
		lang_type =   MMISET_LANGUAGE_IRISH;
		break; 
	case MMIMENU_LANGUAGE_KAZAKH:
		lang_type =   MMISET_LANGUAGE_KAZAKH;
		break; 
	case MMIMENU_LANGUAGE_LATVIAN:
		lang_type =   MMISET_LANGUAGE_LATVIAN;
		break; 
	case MMIMENU_LANGUAGE_LITHUANIAN:
		lang_type =   MMISET_LANGUAGE_LITHUANIAN;
		break; 
	case MMIMENU_LANGUAGE_MACEDONIAN:
		lang_type =   MMISET_LANGUAGE_MACEDONIAN;
		break; 
	case MMIMENU_LANGUAGE_MOLDOVAN:
		lang_type =   MMISET_LANGUAGE_MOLDOVAN;
		break; 
	case MMIMENU_LANGUAGE_NORWEGIAN:
		lang_type =   MMISET_LANGUAGE_NORWEGIAN;
		break; 
	case MMIMENU_LANGUAGE_POLISH:
		lang_type =   MMISET_LANGUAGE_POLISH;
		break;  
	case MMIMENU_LANGUAGE_SERBIAN:
		lang_type =   MMISET_LANGUAGE_SERBIAN;
		break; 
	case MMIMENU_LANGUAGE_SESOTHO:
		lang_type =   MMISET_LANGUAGE_SESOTHO;
		break;  
	case MMIMENU_LANGUAGE_SLOVAK:
		lang_type =   MMISET_LANGUAGE_SLOVAK;
		break;  
	case MMIMENU_LANGUAGE_SWEDISH:
		lang_type =   MMISET_LANGUAGE_SWEDISH;
		break;  
	case MMIMENU_LANGUAGE_UKRAINIAN:
		lang_type =   MMISET_LANGUAGE_UKRAINIAN;
		break;  
	case MMIMENU_LANGUAGE_YORUBA:
		lang_type =   MMISET_LANGUAGE_YORUBA;
		break;  
	case MMIMENU_LANGUAGE_XHOSA:		//科萨语	add. chenyg@spread. 2011-05-12
		lang_type =   MMISET_LANGUAGE_XHOSA;
		break;  
	case MMIMENU_LANGUAGE_ZULU:		//祖鲁语	add. chenyg@spread. 2011-05-12
		lang_type =   MMISET_LANGUAGE_ZULU;
		break;  
	case MMIMENU_LANGUAGE_ASSAMESE:		//(印度语支的)阿萨姆语	add. chenyg@spread. 2011-05-12
		lang_type =   MMISET_LANGUAGE_ASSAMESE;
		break; 
	case MMIMENU_LANGUAGE_SWAHILI:	//斯瓦西里	add. chenyg@spread. 2011-05-12
		lang_type =   MMISET_LANGUAGE_SWAHILI;
		break;  
	case MMIMENU_LANGUAGE_MYANMAR:    //缅甸语
		lang_type =   MMISET_LANGUAGE_MYANMAR;
		break; 
#ifdef MMI_SIM_LANGUAGE_SUPPORT
    case MMIMENU_LANGUAGE_AUTO:
        lang_type =   MMISET_LANGUAGE_AUTO;
        break;
#endif        
    case MMIMENU_MAX_LANGUAGE:
		lang_type =   MMISET_MAX_LANGUAGE;
		break;  
#if 0
	case MMIMENU_LANGUAGE_AMHARIC:    //阿姆哈拉
		lang_type =   MMISET_LANGUAGE_AMHARIC;
		break;      
	case MMIMENU_LANGUAGE_KHMER:    //柬埔寨
		lang_type =   MMISET_LANGUAGE_KHMER;
		break; 
	case MMIMENU_LANGUAGE_LAO:    //老挝
		lang_type =   MMISET_LANGUAGE_LAO;
		break; 
#endif
	}
    return lang_type;
}
/*****************************************************************************/
// Description :判断动态菜单是否需要同步
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsNeedSyn(void)
{
    GUIMENU_FILE_DEVICE_E store_idx = GUIMENU_DEVICE_SYSTEM;
    uint32 check_info = 0;

    if(MMIAPIUDISK_UdiskIsRun())
    {
        return FALSE;
    }
    
    for (store_idx = GUIMENU_DEVICE_SYSTEM; store_idx < GUIMENU_DEVICE_MAX; store_idx++)
    {
        check_info = GetDevCheckInfo(store_idx);

        if (s_dy_data_array[store_idx].check_info != check_info)
        {
            return TRUE;
        }
    }

    return FALSE;
}


/*****************************************************************************/
// Description : get dev info and check if have enough space
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsDevEnoughSpace(GUIMENU_FILE_DEVICE_E dev)
{
    uint32 free_space_low = 0;
    uint32 free_space_high = 0;

    if (dev > GUIMENU_DEVICE_MAX)
    {
        return FALSE;
    }

    MMIAPIFMM_GetDeviceFreeSpace(MMIAPIFMM_GetDevicePath((MMIFILE_DEVICE_E) dev),
                               MMIAPIFMM_GetDevicePathLen((MMIFILE_DEVICE_E) dev),
                               &free_space_high,
                               &free_space_low);

    if (0 == free_space_high && DYNAMIC_DEV_SPACE_LIMIT > free_space_low)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].IsDevEnoughSpace: free_spaced_high = %d, free_space_low = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_759_112_2_18_2_35_44_224,(uint8*)"dd", free_space_high, free_space_low);
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/
// Description : 判断item是否已经存在
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsItemExist(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
)
{
    uint32 total_menu_count = 0;
    GUIMENU_FILE_DEVICE_E store_idx = GUIMENU_DEVICE_SYSTEM;
    
    if (PNULL == dynamic_item_ptr)
    {
        return FALSE;
    }

    ResetData();

    GetAllDyMenuData(&total_menu_count);
    
    if (0 == total_menu_count)
    {
        ResetData();
        return FALSE;
    }

    for (store_idx = GUIMENU_DEVICE_SYSTEM; store_idx < GUIMENU_DEVICE_MAX; store_idx++)
    {   
		uint16 i        = 0;
        uint16 item_idx = 0;
        GUIMENU_MAINMENU_DYNAMIC_INFO_T *data_ptr = PNULL;
        
        if (0 == s_dy_data_array[store_idx].dy_menu_num) continue;

        if (PNULL == s_dy_data_array[store_idx].data_ptr) continue;

        data_ptr = s_dy_data_array[store_idx].data_ptr;
        
        for (item_idx = 0; item_idx < s_dy_data_array[store_idx].dy_menu_num; item_idx++)
        {
			for (i = 0; i < GUIMENU_DYMAINMENU_STR_LANG_MAX_NUM; i++)
			{
				if (0 == MMIAPICOM_Wstrcmp(dynamic_item_ptr->dynamic_menu_info.text,
					data_ptr[item_idx].dynamic_menu_info.str_lang_info[i].text))
				{
					//SCI_TRACE_LOW:"[Mmimenu_synchronize].IsItemExist: item existed!"
					SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_808_112_2_18_2_35_44_225,(uint8*)"");
					return TRUE;
				}
			}
        }
    }

    return FALSE;
}

/*****************************************************************************/
// Description : 验证信息是否正确
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsValidDyMenuInfo(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
)
{
    if (PNULL == dynamic_item_ptr)
    {
        return FALSE;
    }

    if (!IsRunInfoValid(&(dynamic_item_ptr->dynamic_menu_info)))
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].IsValidDyMenuInfo: Run info invalid."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_832_112_2_18_2_35_44_226,(uint8*)"");

        return FALSE;
    }

    do
    {
        BOOLEAN has_icon_data = (PNULL != dynamic_item_ptr->dynamic_menu_info.icon_ptr);
        BOOLEAN is_valid_icon_size = (dynamic_item_ptr->dynamic_menu_info.icon_datasize < ICON_FILE_SIZE_LIMIT);

        if (!(dynamic_item_ptr->dynamic_menu_info.has_icon)) break;

        if (has_icon_data) break;

        if (!is_valid_icon_size)
        {
            dynamic_item_ptr->dynamic_menu_info.has_icon = FALSE;
            break;
        }

        //SCI_TRACE_LOW:"[Mmimenu_synchronize].IsValidDyMenuInfo: icon info invalid."
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_852_112_2_18_2_35_44_227,(uint8*)"");

        return FALSE;
    } while (0);

    return TRUE;
}

/*****************************************************************************/
// Description : 验证是否第一次存储
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsFirstTimeAppend(
    GUIMENU_MAINMENU_DYNAMIC_T* all_dy_data_ptr
)
{   
    if (PNULL != all_dy_data_ptr)
    {
        if (all_dy_data_ptr->dy_menu_num > 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************/
// Description : 根据存储方式，读取所有动态菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN ReadDevMenuData(
    uint8 **icon_data_pptr,
    GUIMENU_MAINMENU_DYNAMIC_T *dy_data_ptr,
    GUIMENU_FILE_DEVICE_E store_idx
)
{
    BOOLEAN is_all_read = FALSE;
    wchar data_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    void *temp_data_ptr = PNULL;
    MMIMENU_SYN_HEAD_T* file_dead_ptr = PNULL;
    uint32 ver_info = 0;
    uint32 offset = 0;
    uint32 data_size = 0;

    if (PNULL == icon_data_pptr || PNULL == dy_data_ptr)
    {
        return FALSE;
    }

    do
    {
        //data读取整个文件，然后分成两部分
        GetFilename_Data(data_file_name);
        temp_data_ptr = ReadDataFromFile(data_file_name, store_idx);

        if (PNULL == temp_data_ptr) break;

        ver_info = GenVersionInfo();
                
        file_dead_ptr = (MMIMENU_SYN_HEAD_T*)temp_data_ptr;
        
        //版本不同，无法加载 
        if (ver_info != file_dead_ptr->ver_info) break;
        
        if (DYNAMIC_MAINMENU_NUM_LIMIT < file_dead_ptr->dy_menu_num)//个数出错
        {
            //SCI_TRACE_LOW:"[Mmimenu_synchronize].ReadDevMenuData:file_dead_ptr->dy_menu_num = %d, file_dead_ptr->check_info = %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_919_112_2_18_2_35_44_228,(uint8*)"dd", file_dead_ptr->dy_menu_num, file_dead_ptr->check_info);
            break;
        }

        dy_data_ptr->check_info = file_dead_ptr->check_info;
        dy_data_ptr->dy_menu_num = file_dead_ptr->dy_menu_num;

        //复制动态数据信息
        offset = sizeof(MMIMENU_SYN_HEAD_T);
        data_size = (sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T) * file_dead_ptr->dy_menu_num);

        if (data_size > DATA_FILE_SIZE_LIMIT) break;
        
        dy_data_ptr->data_ptr = SCI_ALLOCA(data_size);

        if (PNULL == dy_data_ptr->data_ptr) break;

        SCI_MEMSET(dy_data_ptr->data_ptr, 0, data_size);
        
        SCI_MEMCPY(dy_data_ptr->data_ptr, ((uint8*)temp_data_ptr + offset), data_size);

        //复制图片数据信息
        offset += data_size;
        data_size = file_dead_ptr->dy_menu_num * ICON_FILE_SIZE;

        if (data_size > DATA_FILE_SIZE_LIMIT) break;
        
        *icon_data_pptr = SCI_ALLOCA(data_size);

        if (PNULL == *icon_data_pptr) break;

        SCI_MEMSET(*icon_data_pptr, 0, data_size);

        SCI_MEMCPY(*icon_data_pptr, ((uint8*)temp_data_ptr + offset), data_size);
        
        //over
        is_all_read = TRUE;
    }
    while (0);
    
    if (!is_all_read)
    {
        if (PNULL != dy_data_ptr->data_ptr)
        {
            SCI_FREE(dy_data_ptr->data_ptr);
        }

        if (PNULL != *icon_data_pptr)
        {
            SCI_FREE(*icon_data_pptr);
        }
    }
    
    if (PNULL != temp_data_ptr)
    {
        SCI_FREE(temp_data_ptr);
    }

    return is_all_read;
}


/*****************************************************************************/
// Description : 读取所有菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetAllDyMenuData(uint32 *total_dy_menu_ptr)
{
    GUIMENU_FILE_DEVICE_E store_idx = GUIMENU_DEVICE_SYSTEM;

    if (PNULL == total_dy_menu_ptr)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].GetAllDyMenuData: input is null"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_991_112_2_18_2_35_44_229,(uint8*)"");
        return FALSE;
    }

    *total_dy_menu_ptr = 0;
    
    ResetData();

    for (store_idx = GUIMENU_DEVICE_SYSTEM; store_idx < GUIMENU_DEVICE_MAX; store_idx++)
    {        
        if (ReadDevMenuData(&s_icon_data_array_ptr[store_idx],
                                &s_dy_data_array[store_idx],
                                store_idx))
        {
            *total_dy_menu_ptr += s_dy_data_array[store_idx].dy_menu_num;
        }
        else
        {
            s_dy_data_array[store_idx].dy_menu_num = 0;
            SetDataCheckInfo(store_idx, DYNAMIC_MAINMENU_DEFAULT_CHECK_INFO);
        }
    }

    return TRUE;
}

/*****************************************************************************/
// Description :存储动态菜单参数
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN StoreParamData(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    GUIMENU_FILE_DEVICE_E store_idx
)
{
    wchar parm1_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    wchar parm2_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    BOOLEAN is_param_write = FALSE;

    GetFilename_Param(dynamic_item_ptr->menu_id, parm1_name, parm2_name);

    is_param_write = WriteDataToFile(parm1_name,
                                     dynamic_item_ptr->dynamic_menu_info.param1ptr,
                                     dynamic_item_ptr->dynamic_menu_info.parm1_size,
                                     store_idx);

    if (!is_param_write)
    {
        return FALSE;
    }

    is_param_write = WriteDataToFile(parm2_name,
                                     dynamic_item_ptr->dynamic_menu_info.param2ptr,
                                     dynamic_item_ptr->dynamic_menu_info.parm2_size,
                                     store_idx);

    return is_param_write;
}


/*****************************************************************************/
// Description : 把动态菜单info数据存入文件
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN StoreInfoData(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    uint32 total_item_num,
    BOOLEAN is_new_item,//是否有新增item
    GUIMENU_FILE_DEVICE_E store_idx,
    BOOLEAN is_first_time,//是否第一次存储
    uint8 *icon_data_ptr,   //图片数据
    GUIMENU_MAINMENU_DYNAMIC_T *all_dy_data_ptr
)
{
    BOOLEAN is_stored = FALSE;
    BOOLEAN is_copyed = FALSE;
    uint32 file_head_size = 0;
    uint32 temp_data_size = 0;
    uint32 icon_data_size = 0;
    uint32 file_data_size = 0;
    uint8* file_data_ptr = PNULL;
    MMIMENU_SYN_HEAD_T *file_head_ptr = PNULL;
    MMIMENU_SYN_ICON_T scaled_info = {0};

        
    if (PNULL == dynamic_item_ptr|| 0 == total_item_num
        || store_idx > GUIMENU_DEVICE_MAX 
        || total_item_num > DYNAMIC_MAINMENU_NUM_LIMIT
        || (!is_first_time && (PNULL == icon_data_ptr || PNULL == all_dy_data_ptr)))
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].StoreInfoData: input error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_1081_112_2_18_2_35_44_230,(uint8*)"");
        return FALSE;
    }

    if (is_new_item)
    {
		if (dynamic_item_ptr->dynamic_menu_info.has_icon)
		{
			BOOLEAN is_decoded = FALSE;
            
			is_decoded = DecodeIconData(store_idx, dynamic_item_ptr, &scaled_info);
			
			if (!is_decoded)
			{
				//使用默认图片
				dynamic_item_ptr->dynamic_menu_info.has_icon = FALSE;
			}
		}

        dynamic_item_ptr->dynamic_menu_info.icon_ptr = PNULL;
    }


    //除了启动参数，所有数据均在一个文件里面存储。
    //因为启动参数只有在启动时才使用，没有必要放在内存中
    //文件结构如下:
    //|------------------------------------------|
    //|--文件头---|---数据信息----|---图片数据---|
    //|------------------------------------------|

    do
    {
        //---1---、计算文件总大小
        
        //1.1 文件头
        file_head_size = sizeof(MMIMENU_SYN_HEAD_T);
        
        //1.2数据大小
        temp_data_size = sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T) * total_item_num;

        //1.3图片大小
        icon_data_size = ICON_FILE_SIZE_LIMIT * total_item_num;

        //文件总大小
        file_data_size = file_head_size + temp_data_size + icon_data_size;

        if (file_data_size > DATA_FILE_SIZE_LIMIT) break;

        //---2---、整理数据,拷贝。
        //2.1 申请大片内存，用于整个文件数据使用
        file_data_ptr = SCI_ALLOCA(file_data_size);

        if (PNULL == file_data_ptr) break;

        SCI_MEMSET(file_data_ptr, 0, file_data_size);

        file_head_ptr = (MMIMENU_SYN_HEAD_T *)file_data_ptr;

        file_head_ptr->ver_info = GenVersionInfo();
        file_head_ptr->check_info = GenCheckInfo();
        
        //2.2 数据拷贝
        if (is_first_time)//第一次存储只有一个菜单项的数据
        {
            //复制数据段信息
            SCI_MEMCPY(file_data_ptr + file_head_size, dynamic_item_ptr, temp_data_size);

            if (dynamic_item_ptr->dynamic_menu_info.has_icon
                && PNULL != scaled_info.target_buf_ptr)
            {
                //复制图片数据
                SCI_MEMCPY(file_data_ptr + file_head_size + temp_data_size, 
                    scaled_info.target_buf_ptr, icon_data_size);
            }

            is_copyed = TRUE;
        }
        else
        {
            //合并之前存储的数据
            if (is_new_item)
            {
                uint32 data_offset = 0;
                uint32 data_size = 0;
                
                //a 复制数据段信息---已有数据
                data_offset = file_head_size;
                data_size = sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T) * (total_item_num - 1);

                if (data_size > DATA_FILE_SIZE_LIMIT) break;
                
                SCI_MEMCPY(file_data_ptr + data_offset,
                    all_dy_data_ptr->data_ptr,
                    data_size); 

                //b 复制数据段信息---新增数据
                data_offset += data_size;
                data_size = sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T);
                
                SCI_MEMCPY(file_data_ptr + data_offset,
                           dynamic_item_ptr,
                           data_size);


                //c 复制图片数据----已有数据
                data_offset += data_size;
                data_size = ICON_FILE_SIZE * (total_item_num - 1);
                
                SCI_MEMCPY(file_data_ptr + data_offset,
                           icon_data_ptr,
                           ICON_FILE_SIZE * (total_item_num - 1));

                data_offset += data_size;
                data_size = ICON_FILE_SIZE;

                if (dynamic_item_ptr->dynamic_menu_info.has_icon
                    && PNULL != scaled_info.target_buf_ptr)
                {
                    //d 复制图片数据----新增数据
                    SCI_MEMCPY(file_data_ptr + data_offset,
                               scaled_info.target_buf_ptr,
                               data_size);
                }                    

                is_copyed = TRUE;
            }
            else
            {
                uint32 data_offset = 0;
                uint32 data_size = 0;
                
                //a 复制数据段信息
                data_offset = file_head_size;
                data_size = sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T) * total_item_num;
                
                if (data_size > DATA_FILE_SIZE_LIMIT) break;

                SCI_MEMCPY(file_data_ptr + data_offset,
                    all_dy_data_ptr->data_ptr,
                    data_size); 

                //b 复制图片数据
                data_offset += data_size;
                data_size = ICON_FILE_SIZE * total_item_num;
                if (data_size > DATA_FILE_SIZE_LIMIT) break;
                
                SCI_MEMCPY(file_data_ptr + data_offset,
                           icon_data_ptr,
                           ICON_FILE_SIZE * total_item_num );
                
                is_copyed = TRUE;
            }
        }
    }
    while(0);
    
    if (is_copyed && PNULL != file_head_ptr)
    {
        file_head_ptr->dy_menu_num = total_item_num;

        is_stored = WriteDataInfo(store_idx, 
            file_data_ptr, 
            file_data_size, 
            file_head_ptr->check_info,
            FALSE);
    }

    if (PNULL != scaled_info.target_buf_ptr)
    {
        SCI_FREE(scaled_info.target_buf_ptr);
    }
    
    if (PNULL != file_data_ptr)
    {
        SCI_FREE(file_data_ptr);
    }
    
    return is_stored;
}


/*****************************************************************************/
// Description : 修改动态菜单位置信息
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL void ResetItemPosData(
    GUIMENU_MAINMENU_DYNAMIC_T *dy_data_ptr,
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    uint16 page_num,
    BOOLEAN is_move_state,//是否是因为移动位置而改变
    GUIMENU_FILE_DEVICE_E store_index
)
{
    uint16 temp_page_index = 0;
    uint16 temp_item_index = 0;
    uint32 i = 0;

    if (PNULL == dy_data_ptr)
    {
        return;
    }

    //如果是经过用户移动，需要重新设置位置信息
    if (is_move_state)
    {
        if (PNULL == item_page_ptr)
        {
            return;
        }

        for (i = 0; i < dy_data_ptr->dy_menu_num; i++)
        {
            temp_page_index = 0;
            temp_item_index = 0;

            if (GetItemDispPos(item_page_ptr,
                               dy_data_ptr->data_ptr[i].menu_id,
                               page_num,
                               &temp_page_index,
                               &temp_item_index)) //找到了修改后的位置信息
            {
                dy_data_ptr->data_ptr[i].page_index = temp_page_index;
                dy_data_ptr->data_ptr[i].item_index = temp_item_index;
            }
        }
    }

    if (dy_data_ptr->dy_menu_num > 0)
    {
        uint8* file_ptr = PNULL;
        MMIMENU_SYN_HEAD_T* head_ptr = PNULL;
        uint32 file_size = 0;
        uint32 head_size = 0;
        uint32 data_size = 0;
        uint32 icon_size = 0;

        do
        {
            if (PNULL == s_icon_data_array_ptr[(uint16)store_index]) continue;   

            head_size = sizeof(MMIMENU_SYN_HEAD_T);
            data_size = dy_data_ptr->dy_menu_num * sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T);
            icon_size = dy_data_ptr->dy_menu_num * ICON_FILE_SIZE;
            
            file_size = head_size + data_size + icon_size;

            if (file_size > DATA_FILE_SIZE_LIMIT)  continue;

            file_ptr = SCI_ALLOCA(file_size);

            if (PNULL == file_ptr)  continue;

            head_ptr = (MMIMENU_SYN_HEAD_T*)file_ptr;
            head_ptr->check_info = GenCheckInfo();
            head_ptr->dy_menu_num = dy_data_ptr->dy_menu_num;
            head_ptr->ver_info = GenVersionInfo();

            //复制数据信息
            SCI_MEMCPY(file_ptr + head_size, dy_data_ptr->data_ptr, data_size);

            //复制图片信息
            SCI_MEMCPY(file_ptr + head_size + data_size, 
                s_icon_data_array_ptr[(uint16)store_index], 
                icon_size);
                    
            WriteDataInfo(store_index, 
                file_ptr, 
                file_size, 
                head_ptr->ver_info,
                TRUE);

            SCI_FREE(file_ptr);

        }
        while(0);
        
    }
    else
    {
        DeleteInfoData(store_index);
    }


    return;
}


/*****************************************************************************/
// Description : 根据类型存储动态菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN WriteDataInfo(
    GUIMENU_FILE_DEVICE_E store_idx,
    uint8* file_data_ptr,
    uint32 file_data_size,
    uint32 check_info,
    BOOLEAN is_need_set_syn
)
{
    BOOLEAN is_write = FALSE;
    BOOLEAN is_all_write = FALSE;
    wchar data_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};

    if (store_idx > GUIMENU_DEVICE_MAX || PNULL == file_data_ptr)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].WriteDataInfo: input error"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_1380_112_2_18_2_35_45_231,(uint8*)"");
        return FALSE;
    }
        
    do
    {
        GetFilename_Data(data_file_name);

        is_write = WriteDataToFile(data_file_name,
                                   (void*) file_data_ptr,
                                   file_data_size,
                                   store_idx);

        if (is_write)
        {           
            if (is_need_set_syn)
            {
                SetDataCheckInfo(store_idx, check_info);
            }
        }
        else
        {
            break;
        }

        is_all_write = TRUE;
    }
    while (0);

    return is_all_write;
}


/*****************************************************************************/
// Description: 把已经安装好的动态菜单数据合并进主菜单
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL int32 InsertDyItem(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    MMI_IMAGE_ID_T select_icon_id
)
{
    uint16 page_max_cnt = 0;
    uint16 item_max_cnt = 0;
    GUIMENU_PAGE_POS_T pos_info = {0};
    int32 result = 0;
    uint32 item_idx = 0;
    uint16 i = 0;
    GUIMENU_FILE_DEVICE_E store_idx = 0;
    BOOLEAN is_pos_changed = FALSE;

    if (PNULL == item_page_ptr || PNULL == menu_ctrl_ptr)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].InsertDyItem: input is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_1434_112_2_18_2_35_45_232,(uint8*)"");
        return 2;
    }

    GetMenuPageInfo(menu_ctrl_ptr, &page_max_cnt, &item_max_cnt);

    if (0 == page_max_cnt || 0 == item_max_cnt)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].InsertDyItem: page_max_cnt = %d, item_max_cnt = %d,"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_1442_112_2_18_2_35_45_233,(uint8*)"dd", page_max_cnt, item_max_cnt);
        return result;
    }
    
    // 0 trim the page info
    TrimPageInfo(item_page_ptr, pos_info,menu_ctrl_ptr, page_max_cnt, item_max_cnt);

    // 1 找到id大于当前id
    // 2 将当前item插入
    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        store_idx = i + GUIMENU_DEVICE_SYSTEM;

        if (0 == s_dy_data_array[i].dy_menu_num) continue;

        for (item_idx = 0; item_idx < s_dy_data_array[i].dy_menu_num; item_idx++)
        {
            //BOOLEAN is_item_put = FALSE;

            s_dy_data_array[i].data_ptr[item_idx].dynamic_menu_info.select_icon_id = select_icon_id;

/*
                        if (DYNAMIC_MAINMENU_DEFAULT_PAGE_INDEX != s_dy_data_array[i].data_ptr[item_idx].page_index
                                && DYNAMIC_MAINMENU_DEFAULT_ITEM_INDEX != s_dy_data_array[i].data_ptr[item_idx].item_index)
                        {
                            uint16 cur_page = s_dy_data_array[i].data_ptr[item_idx].page_index;
            
                            // 防止内存越界，必须增加cur_page < page_max_cnt判断
                            if (cur_page < page_max_cnt && item_page_ptr[cur_page].item_num < item_max_cnt)
                            {
                                is_item_put = TRUE;
            
                                InsertDyItemByPos(&item_page_ptr[cur_page], pos_info[cur_page],
                                                  &s_dy_data_array[i].data_ptr[item_idx]);
            
                                if (menu_ctrl_ptr->item_total_num < (cur_page + 1))
                                {
                                    menu_ctrl_ptr->item_total_num
                                    = MIN((cur_page + 1), page_max_cnt);
                                }
                            }
                        }*/
            

            //if (!is_item_put)
            {
                uint16 page_cnt = MIN(menu_ctrl_ptr->item_total_num, page_max_cnt);
                uint16 new_page_idx = 0;
                uint16 new_item_idx = 0;
                BOOLEAN has_empty = GetEmptyInMenu(item_page_ptr,
                                                   &page_cnt,
                                                   &new_page_idx,
                                                   &new_item_idx,
                                                   page_max_cnt,
                                                   item_max_cnt);

                if (!has_empty)
                {
                    result = 1;
                    break;
                }

                is_pos_changed = TRUE;

                menu_ctrl_ptr->item_total_num = page_cnt;
                s_dy_data_array[i].data_ptr[item_idx].page_index = new_page_idx;
                s_dy_data_array[i].data_ptr[item_idx].item_index = new_item_idx;

                item_page_ptr[new_page_idx].item_num += 1;
                item_page_ptr[new_page_idx].menu_item_info[new_item_idx].group_id = DYNAMIC_MAINMENU_DEFAULT_GROUP_ID;
                item_page_ptr[new_page_idx].menu_item_info[new_item_idx].menu_id = s_dy_data_array[i].data_ptr[item_idx].menu_id;
            }
        }

        DeleteEmptyPage(item_page_ptr, &menu_ctrl_ptr->item_total_num);

        if (is_pos_changed)
        {
            uint8* file_ptr = PNULL;
            MMIMENU_SYN_HEAD_T* head_ptr = PNULL;
            uint32 file_size = 0;
            uint32 head_size = 0;
            uint32 data_size = 0;
            uint32 icon_size = 0;

            if (PNULL == s_icon_data_array_ptr[(uint16)store_idx]) break;
            
            head_size = sizeof(MMIMENU_SYN_HEAD_T);
            data_size = s_dy_data_array[i].dy_menu_num * sizeof(GUIMENU_MAINMENU_DYNAMIC_INFO_T);
            icon_size = s_dy_data_array[i].dy_menu_num * ICON_FILE_SIZE;
            
            file_size = head_size + data_size + icon_size;

            if (file_size > DATA_FILE_SIZE_LIMIT) break;

            file_ptr = SCI_ALLOCA(file_size);

            if (PNULL == file_ptr) break;

            head_ptr = (MMIMENU_SYN_HEAD_T*)file_ptr;
            head_ptr->check_info = GenCheckInfo();
            head_ptr->dy_menu_num = s_dy_data_array[i].dy_menu_num;
            head_ptr->ver_info = GenVersionInfo();

            //复制数据信息
            SCI_MEMCPY(file_ptr + head_size, s_dy_data_array[i].data_ptr, data_size);

            //复制图片信息
            SCI_MEMCPY(file_ptr + head_size + data_size, 
                s_icon_data_array_ptr[(uint16)store_idx], 
                icon_size);

            WriteDataInfo(store_idx, 
                file_ptr, 
                file_size,
                head_ptr->check_info,
                TRUE);

            SCI_FREE(file_ptr);
        }
    }

    return result;
}

/*****************************************************************************/
// Description: 获取主菜单页数
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void GetMenuPageInfo(
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    uint16 *max_page_ptr,
    uint16 *max_item_ptr
)
{    
    if (PNULL == menu_ctrl_ptr || PNULL == max_page_ptr || PNULL == max_item_ptr)
    {
        return;
    }
    
	switch(menu_ctrl_ptr->cur_style)
	{
#ifdef MENU_CRYSTALCUBE_SUPPORT
	case GUIMENU_STYLE_CRYSTAL_CUBE:
        *max_page_ptr = MMITHEME_MENUMAIN_CRYSTALCUBE_PAGE_NUM;
        *max_item_ptr = CRYSTALCUBE_MENUMAIN_ICON_ITEM_MAX_NUM;
		break;
#endif
#ifdef QBTHEME_SUPPORT
	case GUIMENU_STYLE_QBTHEME:
        *max_page_ptr = MMITHEME_MENUMAIN_SLIDE_PAGE_NUM;
        *max_item_ptr = MMITHEME_QBMENU_ITEM_MAX_NUM;
		break;
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
	case GUIMENU_STYLE_GO:
        *max_page_ptr = MMITHEME_MENUMAIN_SLIDE_PAGE_NUM;
        *max_item_ptr = MMITHEME_GOMENU_ITEM_MAX_NUM;
		break;
#endif
	default:
        *max_page_ptr = MMITHEME_MENUMAIN_SLIDE_PAGE_NUM;
        *max_item_ptr = MMITHEME_MENUMAIN_SLIDE_ITEM_MAX_NUM;
		break;
	}
    return;
}

/*****************************************************************************/
// Description: 清除主菜单中可能存在的空白项
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void TrimPageInfo(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    GUIMENU_PAGE_POS_T pos_info_pptr,
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    uint16 page_max_cnt,
    uint16 item_max_cnt
)
{
    uint16 page_idx = 0;
    uint16 item_idx = 0;
	
    if (PNULL == item_page_ptr || PNULL == pos_info_pptr)
    {
        return;
    }
    

	for (page_idx = 0; page_idx < page_max_cnt; page_idx++)
	{
		uint16 real_idx = 0;
		
		if (0 == item_page_ptr[page_idx].item_num) continue;
		
		for (item_idx = 0; item_idx < item_max_cnt; item_idx++)
		{
			if (0 != item_page_ptr[page_idx].menu_item_info[item_idx].menu_id)
			{
				pos_info_pptr[page_idx][real_idx] = item_idx;
				
				if (real_idx != item_idx)
				{
					item_page_ptr[page_idx].menu_item_info[real_idx].group_id
						= item_page_ptr[page_idx].menu_item_info[item_idx].group_id;
					item_page_ptr[page_idx].menu_item_info[real_idx].menu_id
						= item_page_ptr[page_idx].menu_item_info[item_idx].menu_id;
				}
				
				real_idx++;
			}
		}
		
		item_page_ptr[page_idx].item_num = real_idx;        
	}
	
    return;
}


/*****************************************************************************/
// Description: 把动态菜单按位置编号放回主菜单
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void InsertDyItemByPos(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    uint16 *pos_info_ptr,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dy_data_ptr
)
{
    uint16 item_idx = 0;

    if (PNULL == item_page_ptr || PNULL == dy_data_ptr)
    {
        return;
    }

    for (item_idx = item_page_ptr->item_num; item_idx > 0;)
    {
        if (dy_data_ptr->item_index < pos_info_ptr[item_idx - 1])
        {
            pos_info_ptr[item_idx] = pos_info_ptr[item_idx - 1];
            item_page_ptr->menu_item_info[item_idx] = item_page_ptr->menu_item_info[item_idx - 1];
            item_idx--;
        }
        else
        {
            break;
        }
    }

    item_page_ptr->item_num += 1;
    item_page_ptr->menu_item_info[item_idx].group_id = DYNAMIC_MAINMENU_DEFAULT_GROUP_ID;
    item_page_ptr->menu_item_info[item_idx].menu_id = dy_data_ptr->menu_id;
    pos_info_ptr[item_idx] = dy_data_ptr->item_index;

    return;
}


/*****************************************************************************/
// Description : 查找mainmenu中的空缺位置
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetEmptyInMenu(
    MMIMAINMENU_PAGE_ITEM_INFO_T const *item_page_ptr,//[in]静态菜单数据信息
    uint16 *page_cnt_ptr,
    uint16 *page_idx_ptr,
    uint16 *item_idx_ptr,
    uint16 page_max_cnt,
    uint16 item_max_cnt
)
{
    uint16 page_idx = 0;
    BOOLEAN has_empty = FALSE;

    if (PNULL == item_page_ptr || PNULL == page_cnt_ptr)
    {
        return FALSE;
    }

    if (0 == *page_cnt_ptr)
    {
        return FALSE;
    }

    //1 从当前页开始找
    //2 从第一页开始找
    for (page_idx = *page_cnt_ptr - 1;
            page_idx < page_max_cnt + *page_cnt_ptr - 1;
            page_idx++)
    {
        uint16 cur_idx = page_idx;

        if (cur_idx >= page_max_cnt)
        {
            cur_idx -= page_max_cnt;
        }

        if (item_page_ptr[cur_idx].item_num >= item_max_cnt)
        {
            continue;
        }

        if (PNULL != page_idx_ptr && PNULL != item_idx_ptr)
        {
            *page_idx_ptr = cur_idx;
            *item_idx_ptr = item_page_ptr[cur_idx].item_num;
        }

        if (*page_cnt_ptr < cur_idx + 1)
        {
            *page_cnt_ptr = cur_idx + 1;
        }

        has_empty = TRUE;

        break;
    }

    return has_empty;
}


/*****************************************************************************/
// Description: 清除主菜单中的空白页
// Global resource dependence:
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL void DeleteEmptyPage(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    uint16 *page_count_ptr //[in/out]
)
{
    uint16 new_page_idx = 0;
    uint16 old_page_idx = 0;

    if (PNULL == item_page_ptr || PNULL == page_count_ptr)
    {
        return;
    }

    for (old_page_idx = 0; old_page_idx < *page_count_ptr; old_page_idx++)
    {
        if (item_page_ptr[old_page_idx].item_num > 0)
        {
            if (new_page_idx != old_page_idx)
            {
                item_page_ptr[new_page_idx] = item_page_ptr[old_page_idx];
            }

            new_page_idx++;
        }
    }

    *page_count_ptr = new_page_idx;

    return;
}


/*****************************************************************************/
// Description : 从文件系统中删除动态菜单信息
// Global resource dependence :
// Author:jin.wang
// Note:
//  1 delete - pr1, pr2, icon(dec, ori)
//  2 modify - check info, count, data
/*****************************************************************************/
LOCAL BOOLEAN DeleteItemData(
    GUIMENU_FILE_DEVICE_E store_idx,
    GUIMENU_MAINMENU_DYNAMIC_T *dy_data_ptr,
    uint8* icon_ptr,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
)
{
    uint32 item_idx = 0;
    uint32 delete_idx = 0;
    BOOLEAN is_data_rewrite = FALSE;

    if (PNULL == dy_data_ptr || PNULL == dynamic_item_ptr || PNULL == dy_data_ptr->data_ptr
        || PNULL == icon_ptr)
    {
        return FALSE;
    }

    DeleteParamFile(store_idx, dynamic_item_ptr->menu_id);
    DeleteOriFile(store_idx, dynamic_item_ptr->menu_id);

    for (item_idx = 0; item_idx < dy_data_ptr->dy_menu_num; item_idx++)
    {
        if (dynamic_item_ptr->menu_id == dy_data_ptr->data_ptr[item_idx].menu_id)
        {
            break;
        }
    }

    if (item_idx < dy_data_ptr->dy_menu_num)
    {
        dy_data_ptr->dy_menu_num -= 1;
    }
    else
    {
        return FALSE;
    }

    delete_idx = item_idx;
    
    for (; item_idx < dy_data_ptr->dy_menu_num; item_idx++)
    {
        dy_data_ptr->data_ptr[item_idx] = dy_data_ptr->data_ptr[item_idx + 1];
    }

    if (delete_idx < dy_data_ptr->dy_menu_num)
    {
        uint32 ori_offset = 0;
        uint32 des_offset = 0;
        uint32 data_size = 0;
        
        des_offset = delete_idx * ICON_FILE_SIZE;
        ori_offset = (delete_idx + 1) * ICON_FILE_SIZE;
        data_size = (dy_data_ptr->dy_menu_num - delete_idx) * ICON_FILE_SIZE;

        SCI_MEMCPY((icon_ptr + des_offset), (icon_ptr + ori_offset), data_size);
    }
    
    if (dy_data_ptr->dy_menu_num > 0)
    {
        is_data_rewrite = StoreInfoData(dynamic_item_ptr,
                                        dy_data_ptr->dy_menu_num,
                                        FALSE,
                                        store_idx,
                                        FALSE,
                                        icon_ptr,
                                        dy_data_ptr);
    }
    else
    {
        //没有动态菜单数据，删除所有数据
        is_data_rewrite = DeleteInfoData(store_idx);
    }

    return  is_data_rewrite;
}


/*****************************************************************************/
// Description : 将图标解码后存储
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN DecodeIconData(
    GUIMENU_FILE_DEVICE_E store_idx,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    MMIMENU_SYN_ICON_T *scaled_info_ptr
)
{
    BOOLEAN result = FALSE;
    GUIIMG_DISPLAY_T img_display = {0};
    GUIIMG_DEC_OUT_T img_output = {0};
    GUIIMG_SRC_T img_info = {0};
    GUIIMG_INFO_T  img_src_info = {0};    
    uint32 img_buffer_size = 0;
    uint8 *item_icon_ptr = PNULL;
    wchar ori_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    BOOLEAN is_all_write = FALSE;
    BOOLEAN is_zoom_out = FALSE;

    if (PNULL == dynamic_item_ptr || PNULL == scaled_info_ptr)
    {
        return FALSE;
    }

    //解码
    GUIIMG_GetImgInfo((uint8 *)dynamic_item_ptr->dynamic_menu_info.icon_ptr,dynamic_item_ptr->dynamic_menu_info.icon_datasize,&img_src_info);
    img_display.is_handle_transparent = TRUE;

    //如果获得的img_info不为空，非ico的格式，使用原始宽，高。
    if(0 != img_src_info.image_height 
       && 0 != img_src_info.image_width )
    {
        img_display.dest_height = img_src_info.image_height;
        img_display.dest_width  = img_src_info.image_width; 
    }
    //如果获得的img_info为{0},表明是ico的格式，使用目标宽，高。
    else
    {
        img_display.dest_height = MENUICON_HEIGHT - MMIMENU_ICON_BORDER_RESERVE;
        img_display.dest_width  = MENUICON_WIDTH  - MMIMENU_ICON_BORDER_RESERVE;
    }

    img_buffer_size = img_display.dest_width * img_display.dest_height * 4;
    item_icon_ptr = SCI_ALLOCA(img_buffer_size);

    if (PNULL == item_icon_ptr)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].DecodeIconData: No Memory!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_1912_112_2_18_2_35_46_234,(uint8*)"");
        return FALSE;
    }

    SCI_MEMSET(item_icon_ptr, 0, img_buffer_size);

    //解码后缓存地址赋值
    img_output.decode_data_ptr = item_icon_ptr;
    img_output.decode_data_size = img_buffer_size;

    img_info.src_buf_ptr = dynamic_item_ptr->dynamic_menu_info.icon_ptr;
    img_info.src_data_size = dynamic_item_ptr->dynamic_menu_info.icon_datasize;

    do
    {
        result = GUIIMG_Decode(&img_info, &img_display, &img_output);

        if (!result) break;

        img_output.decode_data_size = img_output.actual_width * img_output.actual_height * 4;

        //if (img_output.decode_data_size > ICON_FILE_SIZE_LIMIT) break;
        
        //图片大小需要统一
        if (img_output.actual_height != (MENUICON_HEIGHT - MMIMENU_ICON_BORDER_RESERVE) 
            || img_output.actual_width != (MENUICON_WIDTH - MMIMENU_ICON_BORDER_RESERVE))
        {
            GUIIMG_DISPLAY_T img_display = {0};
            img_display.is_handle_transparent = TRUE;
            img_display.dest_height = MENUICON_HEIGHT - MMIMENU_ICON_BORDER_RESERVE;
            img_display.dest_width  = MENUICON_WIDTH - MMIMENU_ICON_BORDER_RESERVE;

            is_zoom_out = ZoomOutIconData(img_display, 
                            &img_output,
                            scaled_info_ptr);
/*
				{
					FILE *fp=fopen("e:\\gif2.raw","wb");
					fwrite(img_output.decode_data_ptr,1,1024*1024,fp);
					fclose(fp);
				}*/
		
            //如果图片解码失败，给应用一个默认图片
            if (!is_zoom_out)
            {
                if (PNULL != scaled_info_ptr->target_buf_ptr)
                {
                    SCI_FREE(scaled_info_ptr->target_buf_ptr);
                }

                dynamic_item_ptr->dynamic_menu_info.has_icon = FALSE;
            }
        }
        else
        {
            //图片无需放大，直接复制
            scaled_info_ptr->target_buf_ptr = SCI_ALLOCA(img_output.decode_data_size);
            
            if(PNULL != scaled_info_ptr->target_buf_ptr)
            {
                SCI_MEMCPY(scaled_info_ptr->target_buf_ptr, 
                    img_output.decode_data_ptr,
                    img_output.decode_data_size);
            }
            else
            {
                dynamic_item_ptr->dynamic_menu_info.has_icon = FALSE;
            }
        }

        dynamic_item_ptr->dynamic_menu_info.icon_height = img_output.actual_height;
        dynamic_item_ptr->dynamic_menu_info.icon_width = img_output.actual_width;
        
        GetFilename_Icon(dynamic_item_ptr->menu_id,
                         GUIMENU_READFILE_ORIG,
                         ori_file_name,
                         PNULL);

        WriteDataToFile(ori_file_name,
                        img_info.src_buf_ptr,
                        img_info.src_data_size,
                        store_idx);

        is_all_write = TRUE;
    }
	while (0);

    SCI_FREE(item_icon_ptr);

    return is_all_write;
}

/*****************************************************************************/
// Description : 将图标放大到指定尺寸
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN ZoomOutIconData(
    GUIIMG_DISPLAY_T img_display,
    GUIIMG_DEC_OUT_T *img_output_ptr,
    MMIMENU_SYN_ICON_T *scaled_page_ptr
)
{
    BOOLEAN             result = FALSE;
    SCALE_IMAGE_IN_T    scale_in  = {0};
    SCALE_IMAGE_OUT_T   scale_out = {0};
    uint8* alloc_ptr = PNULL;
    
    if (PNULL == img_output_ptr || PNULL == scaled_page_ptr || PNULL == img_output_ptr->decode_data_ptr)
    {
        return FALSE;
    }
    
    scale_in.src_format           = IMGREF_FORMAT_ARGB888;
    scale_in.src_chn.chn0.ptr     = img_output_ptr->decode_data_ptr;
    
    if (PNULL == scale_in.src_chn.chn0.ptr)
    {
        return FALSE;
    }
    
    scale_in.src_chn.chn0.size    = img_output_ptr->decode_data_size;
    scale_in.src_size.w           = img_output_ptr->actual_width;
    scale_in.src_size.h           = img_output_ptr->actual_height;
    scale_in.src_trim_rect.x      = 0;
    scale_in.src_trim_rect.y      = 0;
    scale_in.src_trim_rect.w      = img_output_ptr->actual_width;
    scale_in.src_trim_rect.h      = img_output_ptr->actual_height;

    scale_in.target_format        = IMGREF_FORMAT_ARGB888;
    scale_in.target_size.w        = img_display.dest_width;
    scale_in.target_size.h        = img_display.dest_height;
    scale_in.target_buf.size      = img_display.dest_height * img_display.dest_width * 4;
    scale_in.target_buf.ptr       = SCI_ALLOCA(scale_in.target_buf.size);
    scale_in.scale_by_software    = TRUE;
    
    if (PNULL == scale_in.target_buf.ptr)
    {
        return result;
    }
    
    SCI_MEMSET(scale_in.target_buf.ptr, 0, scale_in.target_buf.size);

    alloc_ptr = scale_in.target_buf.ptr;
        
    if (SCI_SUCCESS == GRAPH_ScaleImage(&scale_in,&scale_out))
    {
        scaled_page_ptr->buf_ptr = (uint8*)scale_in.target_buf.ptr;
        scaled_page_ptr->target_buf_ptr = (uint8*)scale_out.output_chn.chn0.ptr;
        scaled_page_ptr->width = scale_out.output_size.w;
        scaled_page_ptr->height = scale_out.output_size.h;

        img_output_ptr->actual_height = scale_out.output_size.h;
        img_output_ptr->actual_width = scale_out.output_size.w;

        result = TRUE;
    }
    else
    {
        if (PNULL != alloc_ptr)
        {
            SCI_FREE(alloc_ptr);
        }
    }

    return result;
}

/*****************************************************************************/
// Description : 根据menu_id获取动态菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetItemInfo(
    MMI_MENU_ID_T menu_id,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr,
    GUIMENU_GET_INFO_T get_type,
    GUIMENU_READFILE_TYPE icon_type
)
{
    uint16 i = 0;
    uint16 item_index = 0;
    BOOLEAN is_find = FALSE;
    wchar parm1_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    wchar parm2_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    void* parm1_ptr = PNULL;
    void* parm2_ptr = PNULL;
    wchar icon_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    void* icon_ptr = PNULL;

    if (PNULL == item_info_ptr)
    {
        return FALSE;
    }

    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        if (s_dy_data_array[i].dy_menu_num == 0) continue;

        if (PNULL == s_dy_data_array[i].data_ptr) continue;

        for (item_index = 0; item_index < s_dy_data_array[i].dy_menu_num; item_index++)
        {
            BOOLEAN is_all_read = FALSE;

            if (menu_id != s_dy_data_array[i].data_ptr[item_index].menu_id)
            {
                continue;
            }

            is_find = TRUE;

            item_info_ptr->store_dev = i + GUIMENU_DEVICE_SYSTEM;

            if (GUIMENU_GET_INFO_PARA & get_type)
            {
                do
                {
                    GetFilename_Param(menu_id, parm1_name, parm2_name);

                    //读取参数1
                    parm1_ptr = ReadDataFromFile(parm1_name, item_info_ptr->store_dev);

                    //if (PNULL == parm1_ptr) break;在没有parm时,同样的给全局变量赋值.

                    s_dy_data_array[i].data_ptr[item_index].dynamic_menu_info.param1ptr
                    = parm1_ptr;

                    //读取参数2
                    parm2_ptr = ReadDataFromFile(parm2_name, item_info_ptr->store_dev);

                    //if (PNULL == parm2_ptr) break;

                    s_dy_data_array[i].data_ptr[item_index].dynamic_menu_info.param2ptr
                    = parm2_ptr;

                    is_all_read = TRUE;
                }
                while (0);
            }

            if (GUIMENU_GET_INFO_ICON & get_type)
            {
                //有图片信息才读取，没有的话不读取
                if (s_dy_data_array[i].data_ptr[item_index].dynamic_menu_info.has_icon)
                {
                    if (GUIMENU_READFILE_DECODE == icon_type)
                    {                        
                        //读取解码后图片
                        icon_ptr = GetDecodeIconData(item_info_ptr->store_dev, item_index);
                    }
                    else if (GUIMENU_READFILE_ORIG == icon_type)
                    {
                        GetFilename_Icon(menu_id,
                                         GUIMENU_READFILE_ORIG,
                                         icon_name,
                                         PNULL);

                        //读取原始图片
                        icon_ptr = ReadDataFromFile(icon_name, item_info_ptr->store_dev);
                    }
                    else
                    {
                        break;
                    }

                    if (PNULL != icon_ptr)
                    {
                        s_dy_data_array[i].data_ptr[item_index].dynamic_menu_info.icon_ptr
                            = icon_ptr;

                        is_all_read = TRUE;
                    }
                }
                else
                {
                    is_all_read = TRUE;
                }
            }

            if (GUIMENU_GET_INFO_NONE == get_type)
            {
                is_all_read = TRUE;
            }

            if (is_all_read)
            {
                item_info_ptr->ori_data_ptr = &s_dy_data_array[i].data_ptr[item_index];
            }
            else
            {
                if (PNULL != parm1_ptr)
                {
                    SCI_FREE(parm1_ptr);
                }

                if (PNULL != parm2_ptr)
                {
                    SCI_FREE(parm2_ptr);
                }

                if (PNULL != icon_ptr)
                {
                    SCI_FREE(icon_ptr);
                }

                item_info_ptr->ori_data_ptr = PNULL;
            }

            break;
        }

        if (is_find)
        {
            break;
        }
    }

    MMIMENU_DyGetItemString(item_info_ptr);
    return is_find;
}

/*****************************************************************************/
// Description : 读取菜单解码后图片
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL uint8* GetDecodeIconData(
    GUIMENU_FILE_DEVICE_E store_dev,
    uint16 item_idx
)
{
    uint8* icon_ptr = PNULL;
    uint32 offset = 0;
    uint16 arr_idx = 0;
    
    if (store_dev > (GUIMENU_DEVICE_MAX - 1))
    {
        return PNULL;
    }

    arr_idx = (uint16)store_dev - GUIMENU_DEVICE_SYSTEM;

    if (PNULL == s_icon_data_array_ptr[arr_idx] || s_dy_data_array[arr_idx].dy_menu_num < (item_idx + 1))
    {
        return PNULL;
    }

    icon_ptr = SCI_ALLOCA(ICON_FILE_SIZE);

    if (PNULL == icon_ptr)
    {
        return PNULL;
    }

    offset = ICON_FILE_SIZE * item_idx;

    if (offset > DATA_FILE_SIZE_LIMIT)
    {
        SCI_FREE(icon_ptr);
        return PNULL;
    }
        
    SCI_MEMCPY(icon_ptr, s_icon_data_array_ptr[arr_idx] + offset, ICON_FILE_SIZE);

    return icon_ptr;
}

/*****************************************************************************/
// Description : 根据文件名生成动态菜单数据文件的完整路径
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetPathByFilename(
    GUIMENU_FILE_DEVICE_E store_idx,
    const wchar *file_name_ptr,
    uint16 file_name_len,
    wchar *full_path_name_ptr,    //[OUT]
    uint16 *full_path_len_ptr     //[OUT]
)
{
    char const syspath_name[] = {MMIMULTIM_DIR_SYSTEM_CHAR, '\\' ,'\0'};
    uint32 version_info = 0;
    char path_name[MMIMULTIM_DIR_SYSTEM_LEN + 1 + DYNAMIC_MAINMENU_DATA_FILE_LEN] = {0};
    wchar dst_path_name[MMIMULTIM_DIR_SYSTEM_LEN + 1 + DYNAMIC_MAINMENU_DATA_FILE_LEN] = {0};
    uint16 dst_path_name_len = 0;
    BOOLEAN result_val = FALSE;

    version_info = GenVersionInfo();

    sprintf(path_name, "%s%s%u", syspath_name, DATA_DIR_NAME, version_info);

    dst_path_name_len = SCI_STRLEN(path_name);

    MMIAPICOM_StrToWstr((const uint8*)path_name, dst_path_name);

    result_val = MMIAPIFMM_CombineFullPath(
                     MMIAPIFMM_GetDevicePath((MMIFILE_DEVICE_E) store_idx),
                     MMIAPIFMM_GetDevicePathLen((MMIFILE_DEVICE_E) store_idx),
                     dst_path_name,
                     dst_path_name_len,
                     file_name_ptr,
                     file_name_len,
                     full_path_name_ptr,
                     full_path_len_ptr);

    return result_val;
}


/*****************************************************************************/
// Description : 获取数据文件名
// Global resource dependence : none
// Author: jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN GetFilename_Data(wchar *data_file_name_ptr)
{
    if (PNULL == data_file_name_ptr)
    {
        return FALSE;
    }

    MMIAPICOM_StrToWstr((const uint8*)DATA_FILE_FIX_DATA, data_file_name_ptr);

    return TRUE;
}


/*****************************************************************************/
// Description : 读取文件系统中动态菜单图标启动参数
// Global resource dependence : none
// Author: jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL void GetFilename_Param(
    MMI_MENU_ID_T menu_id,
    wchar *parm1_name_ptr,   //[IN/OUT]
    wchar *parm2_name_ptr   //[IN/OUT]
)
{
    char prm1_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    char prm2_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};

    //SCI_ASSERT(PNULL != parm1_name_ptr);/*assert verified*/
    //SCI_ASSERT(PNULL != parm2_name_ptr);/*assert verified*/
    if (
        (PNULL == parm1_name_ptr)||
        (PNULL == parm2_name_ptr)
        )
    {
        return;
    }

    sprintf(prm1_name, "%d%s" , menu_id, DATA_FILE_FIX_PARA1);
    sprintf(prm2_name, "%d%s" , menu_id, DATA_FILE_FIX_PARA2);

    MMIAPICOM_StrToWstr((const uint8 *) prm1_name, parm1_name_ptr);
    MMIAPICOM_StrToWstr((const uint8 *) prm2_name, parm2_name_ptr);

    return;
}


/*****************************************************************************/
// Description : 获取图片文件名, 获取解码后和原始文件名
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetFilename_Icon(
    MMI_MENU_ID_T menu_id,
    uint16 read_style,
    wchar *ori_file_name_ptr,   //[IN/OUT]
    wchar *decode_file_name_ptr   //[IN/OUT]
)
{
    char decode_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    char ori_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};

    if (GUIMENU_READFILE_DECODE & read_style)
    {
        if (PNULL == decode_file_name_ptr)
        {
            return FALSE;
        }

        sprintf(decode_name, "%d%s", menu_id, DATA_FILE_FIX_ICON_DEC);
        MMIAPICOM_StrToWstr((const uint8 *) decode_name, decode_file_name_ptr);
    }

    if (GUIMENU_READFILE_ORIG & read_style)
    {
        if (PNULL == ori_file_name_ptr)
        {
            return FALSE;
        }

        sprintf(ori_name, "%d%s", menu_id, DATA_FILE_FIX_ICON_ORI);
        MMIAPICOM_StrToWstr((const uint8 *) ori_name, ori_file_name_ptr);
    }

    return TRUE;
}


/*****************************************************************************/
// Description : 从文件系统中删除动态菜单数据
// Global resource dependence : none
// Author: jin.wang
// Note: (所有菜单数据集合的文件)
/*****************************************************************************/
LOCAL BOOLEAN DeleteInfoData(GUIMENU_FILE_DEVICE_E store_idx)
{
    wchar data_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};

    GetFilename_Data(data_file_name);
    DeleteDevFile(data_file_name, store_idx);

    return TRUE;
}


/*****************************************************************************/
// Description : 从文件系统中删除指定文件
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN DeleteDevFile(
    wchar *data_file_name_ptr,
    GUIMENU_FILE_DEVICE_E store_idx
)
{
    BOOLEAN has_file_path = FALSE;
    wchar file_path[DYNAMIC_MAINMENU_DATA_PATH_MAX + 1] = {0};
    uint16 file_path_len = DYNAMIC_MAINMENU_DATA_PATH_MAX;

    if (PNULL == data_file_name_ptr)
    {
        return FALSE;
    }

    has_file_path = GetPathByFilename(store_idx,
                                      data_file_name_ptr,
                                      DYNAMIC_MAINMENU_DATA_FILE_LEN,
                                      file_path,
                                      &file_path_len);
    if (!has_file_path)
    {
        return TRUE;
    }

    if (MMIAPIFMM_IsFileExist(file_path, file_path_len))
    {
        if (SFS_ERROR_NONE != MMIAPIFMM_DeleteFile(file_path, PNULL))
        {
            //SCI_TRACE_LOW:"[Mmimenu_synchronize].DeleteDevFile: fail..."
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2443_112_2_18_2_35_47_235,(uint8*)"");
            return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************/
// Description : 从文件系统中删除动态菜单原始显示数据
// Global resource dependence : none
// Author: nan.ji
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN DeleteOriFile(
    GUIMENU_FILE_DEVICE_E store_idx,
    MMI_MENU_ID_T menu_id
)
{
    wchar ori_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};

	GetFilename_Icon(menu_id,
		GUIMENU_READFILE_ORIG,
		ori_file_name,
		PNULL);
    DeleteDevFile(ori_file_name, store_idx);

    return TRUE;
}
/*****************************************************************************/
// Description : 从文件系统中删除动态菜单启动参数数据
// Global resource dependence : none
// Author: jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL BOOLEAN DeleteParamFile(
    GUIMENU_FILE_DEVICE_E store_idx,
    MMI_MENU_ID_T menu_id
)
{
    wchar parm1_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    wchar parm2_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};

    GetFilename_Param(menu_id, parm1_name, parm2_name);

    DeleteDevFile(parm1_name, store_idx);
    DeleteDevFile(parm2_name, store_idx);

    return TRUE;
}


/*****************************************************************************/
// Description : 往文件系统中写数据
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN WriteDataToFile(
    wchar *data_name_ptr,
    void* data_ptr,
    uint32 data_size,
    GUIMENU_FILE_DEVICE_E store_idx
)
{
    BOOLEAN has_file_path = FALSE;
    BOOLEAN is_write = FALSE;
    MMIFILE_HANDLE file_handle = 0;
    uint32 writen_size = 0;
    wchar file_path[DYNAMIC_MAINMENU_DATA_PATH_MAX + 1] = {0};
    uint16 file_path_len = DYNAMIC_MAINMENU_DATA_PATH_MAX;
    MMIFILE_ERROR_E sfs_result = SFS_ERROR_NONE;

    if (PNULL == data_name_ptr || PNULL == data_ptr)
    {
        return FALSE;
    }

    if (data_size > DATA_FILE_SIZE_LIMIT)
    {
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].WriteDataToFile: ERROR SIZE!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2517_112_2_18_2_35_47_236,(uint8*)"");
        return FALSE;
    }

    do
    {       
        has_file_path = GetPathByFilename(store_idx,
                                          data_name_ptr,
                                          DYNAMIC_MAINMENU_DATA_FILE_LEN,
                                          file_path,
                                          &file_path_len);

        if (!has_file_path) break;
        
        file_handle = MMIAPIFMM_CreateFile(file_path, SFS_MODE_WRITE | SFS_MODE_CREATE_ALWAYS, NULL, NULL);

        if (0 == file_handle) break;

        sfs_result = MMIAPIFMM_WriteFile(file_handle,
                                       (const void *) data_ptr,
                                       data_size,
                                       &writen_size,
                                       PNULL);

        if (SFS_ERROR_NONE == sfs_result && data_size == writen_size)
        {
            MMIAPIFMM_SetFileSize(file_handle, writen_size);
            is_write = TRUE;
        }
        else
        {
            //SCI_TRACE_LOW:"[Mmimenu_synchronize].WriteDataToFile: fail"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2548_112_2_18_2_35_47_237,(uint8*)"");
        }
    }
    while(0);

    MMIAPIFMM_CloseFile(file_handle);
    
    return is_write;
}

/*****************************************************************************/
// Description : 从文件系统中读数据
// Global resource dependence : none
// Author: jin.wang
// Note:
/*****************************************************************************/
LOCAL uint8* ReadDataFromFile(
    wchar const *file_name_ptr,
    GUIMENU_FILE_DEVICE_E store_dev
)
{
    BOOLEAN has_file_path = FALSE;
    wchar file_path[DYNAMIC_MAINMENU_DATA_PATH_MAX + 1] = {0};
    uint16 file_path_len = DYNAMIC_MAINMENU_DATA_PATH_MAX;
    MMIFILE_HANDLE file_handle = 0;
    uint32 file_size = 0;
    uint32 read_size = 0;
    uint8* data_ptr = PNULL;
    MMIFILE_ERROR_E sfs_result = SFS_ERROR_NONE;

    has_file_path = GetPathByFilename(store_dev,
                                      file_name_ptr,
                                      DYNAMIC_MAINMENU_DATA_FILE_LEN,
                                      file_path,
                                      &file_path_len);

    if (!has_file_path)
    {
        return PNULL;
    }

    file_handle = MMIAPIFMM_CreateFile(file_path,
                                     SFS_MODE_READ | SFS_MODE_OPEN_EXISTING, NULL, NULL);

    if (0 == file_handle)
    {
        return PNULL;
    }

    do
    {
        file_size = MMIAPIFMM_GetFileSize(file_handle);

        if (0 == file_size) break;

        if (file_size > DATA_FILE_SIZE_LIMIT)
        {
            //SCI_TRACE_LOW:"[Mmimenu_synchronize].ReadDataFromFile: ERROR SIZE!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2603_112_2_18_2_35_48_238,(uint8*)"");
            break;
        }

        data_ptr = SCI_ALLOCA(file_size);

        if (PNULL == data_ptr)
        {
            //SCI_TRACE_LOW:"[Mmimenu_synchronize].ReadDataFromFile: No Memory!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2611_112_2_18_2_35_48_239,(uint8*)"");
            break;
        }

        SCI_MEMSET(data_ptr, 0, file_size);

        sfs_result = MMIAPIFMM_ReadFile(file_handle,
                                      data_ptr,
                                      file_size,
                                      &read_size,
                                      NULL);

        if (SFS_ERROR_NONE != sfs_result) break;

        if (file_size != read_size) 
        {
            SCI_FREE(data_ptr);
        }

    }
    while (0);

    MMIAPIFMM_CloseFile(file_handle);

    return data_ptr;
}

/*****************************************************************************/
// Description : 获取要删除的动态菜单menu_id
// Global resource dependence :
// Author:jin.wang
// Note:
// dynamic_item_ptr:要删除的菜单信息
/*****************************************************************************/
LOCAL BOOLEAN GetDeleteItemMenuId(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_all_item_ptr,
    uint32 dynamic_menu_num
)
{
    uint32 i = 0;
    uint32 j = 0;
    BOOLEAN result = FALSE;

    if (PNULL == dynamic_item_ptr || PNULL == dynamic_all_item_ptr
            || PNULL == dynamic_item_ptr->dynamic_menu_info.link_function_ptr)
    {
        return result;
    }

    for (i = 0; i < dynamic_menu_num; i++)
    {
        if (PNULL != dynamic_all_item_ptr[i].dynamic_menu_info.link_function_ptr)
        {
            for(j = 0;j < GUIMENU_DYMAINMENU_STR_LANG_MAX_NUM; j++)
            {
				//if (0 == MMIAPICOM_Wstrcmp(dynamic_item_ptr->dynamic_menu_info.str_lang_info[j].text,
				if (0 == MMIAPICOM_Wstrcmp(dynamic_item_ptr->dynamic_menu_info.text,
					dynamic_all_item_ptr[i].dynamic_menu_info.str_lang_info[j].text)
                    && (dynamic_item_ptr->dynamic_menu_info.link_function_ptr
					== dynamic_all_item_ptr[i].dynamic_menu_info.link_function_ptr)) //字符串相同、启动函数相同
				{
					dynamic_item_ptr->menu_id = dynamic_all_item_ptr[i].menu_id;
					result = TRUE;
					break;
				}
            }    
        }
        if(result)
        {
			break;
        }
    }

    if (dynamic_all_item_ptr[i].menu_id < ID_TOTAL)
    {
        result = FALSE;
    }

    return result;
}


/*****************************************************************************/
// Description : 根据list_index获取动态菜单menu_id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetListItemMenuId(
    uint16 list_index,
    MMI_MENU_ID_T *menu_id_ptr
)
{
    uint16 i = 0;
    uint16 item_index = 0;
    BOOLEAN is_find = FALSE;

    if (PNULL == menu_id_ptr)
    {
        return FALSE;
    }

    item_index = list_index;

    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        if (s_dy_data_array[i].dy_menu_num == 0) continue;

        if (PNULL == s_dy_data_array[i].data_ptr) continue;

        if (item_index >= s_dy_data_array[i].dy_menu_num)
        {
            item_index -= s_dy_data_array[i].dy_menu_num;
            continue;
        }

        *menu_id_ptr = s_dy_data_array[i].data_ptr[item_index].menu_id;
        is_find = TRUE;

        break;
    }

    return is_find;
}


/*****************************************************************************/
// Description : 根据存储方式，读取所有动态菜单数据的check_info
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL uint32 GetDevCheckInfo(GUIMENU_FILE_DEVICE_E store_idx)
{
    wchar data_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1] = {0};
    uint8* data_ptr = PNULL;
    MMIMENU_SYN_HEAD_T* head_ptr = PNULL;
    uint32 check_info = 0;

    if (store_idx < GUIMENU_DEVICE_MAX)
    {
        GetFilename_Data(data_file_name);
        data_ptr = ReadDataFromFile(data_file_name, store_idx);
    }

    if (PNULL != data_ptr)
    {
        head_ptr = (MMIMENU_SYN_HEAD_T*)data_ptr;
        check_info = head_ptr->check_info;
        
        SCI_FREE(data_ptr);
    }
    else
    {
        check_info = DYNAMIC_MAINMENU_DEFAULT_CHECK_INFO;
    }

    return check_info;
}


/*****************************************************************************/
// Description : 根据存储方式，设置文件的checkinfo
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
LOCAL void SetDataCheckInfo(
    GUIMENU_FILE_DEVICE_E store_idx,
    uint32 check_info
)
{
    if (store_idx < GUIMENU_DEVICE_MAX)
    {
        s_dy_data_array[(uint16)store_idx].check_info = check_info;
    }

    return;
}


/*****************************************************************************/
// Description : 获取动态菜单的位置信息
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN GetItemDispPos(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    MMI_MENU_ID_T dy_menu_id,
    uint16 page_cnt,
    uint16 *page_index_ptr,//[out]
    uint16 *item_index_ptr //[out]
)
{
    uint16 page_idx = 0;
    uint16 item_idx = 0;

    if (PNULL == item_page_ptr || PNULL == page_index_ptr || PNULL == item_index_ptr)
    {
        return FALSE;
    }

    for (page_idx = 0; page_idx < page_cnt; page_idx++)
    {
        for (item_idx = 0; item_idx < MMITHEME_MENUMAIN_SLIDE_ITEM_MAX_NUM; item_idx++)
        {
            if (item_page_ptr[page_idx].menu_item_info[item_idx].menu_id == dy_menu_id)
            {
                *page_index_ptr = page_idx;
                *item_index_ptr = item_idx;
                return TRUE;
            }
        }
    }

    return FALSE;
}


/*****************************************************************************/
// Description : 释放动态菜单数据数组内存
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL void ResetData(void)
{
    uint16 i = 0;

    if(MMIAPIUDISK_UdiskIsRun())
    {
        return;
    }
    
    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++) //释放内存
    {
        s_dy_data_array[i].check_info = DYNAMIC_MAINMENU_DEFAULT_CHECK_INFO;
        s_dy_data_array[i].dy_menu_num = 0;

        if (PNULL != s_dy_data_array[i].data_ptr)
        {
            SCI_FREE(s_dy_data_array[i].data_ptr);
        }

        if (PNULL != s_icon_data_array_ptr[i])
        {
            SCI_FREE(s_icon_data_array_ptr[i]);
        }
    }

    return;
}


/*****************************************************************************/
// Description : 生成check标志
// Global resource dependence :
// Author:jin.wang
// Note: arm下用时间，win32下用随机数
/*****************************************************************************/
LOCAL uint32 GenCheckInfo(void)
{
    return SCI_GetTickCount();
}


/*****************************************************************************/
// Description : 为动态菜单创建一个menu_id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL uint32 GenMenuId(void)
{
    return SCI_GetTickCount();
}


/*****************************************************************************/
// Description : 根据版本编译时间创建动态菜单校验码
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL uint32 GenVersionInfo(void)
{
    #define VERSION_INFO_LIMIT 10000000

#ifndef WIN32
    #define VERSION_STRING_LENGTH_LIMIT 32
    
    char *build_time_ptr = PNULL;
    uint32 build_time_str_len = 0;
    uint32 build_time_str_idx = 0;
    uint32 ver_info = 0;

    build_time_ptr = VERSION_GetInfo(BUILD_TIME);     /*lint !e605*/
    build_time_str_len = SCI_STRLEN(build_time_ptr);

    if (build_time_str_len == 0 || build_time_str_len > VERSION_STRING_LENGTH_LIMIT)
    {
        return (VERSION_INFO_LIMIT - 1);
    }
    else
    {
        build_time_str_idx = build_time_str_len - 1;
    }

    while(ver_info < VERSION_INFO_LIMIT)
    {
        char cur_chr = build_time_ptr[build_time_str_idx];
        
        if ('0' <= cur_chr && cur_chr <= '9')
        {
            ver_info *= 10;
            ver_info += (uint8)(cur_chr - '0');
        }
        
        if (0 == build_time_str_idx) break;

        build_time_str_idx--;
    }

    return ver_info;
#else
    return (VERSION_INFO_LIMIT - 1);
#endif
}

/*****************************************************************************/
// Description : 设置高亮图片id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL void SetSelectedId(MMI_IMAGE_ID_T select_icon_id)
{
    GUIMENU_FILE_DEVICE_E store_idx = 0;
    uint16 item_idx = 0;
    uint16 i = 0;
    
    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        store_idx = i + GUIMENU_DEVICE_SYSTEM;

        if (0 == s_dy_data_array[i].dy_menu_num) continue;

        for (item_idx = 0; item_idx < s_dy_data_array[i].dy_menu_num; item_idx++)
        {
            s_dy_data_array[i].data_ptr[item_idx].dynamic_menu_info.select_icon_id
            = select_icon_id;
        }
    }

    return;
}

/*****************************************************************************/
// Description : 判定运行的数据是否合法
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN IsRunInfoValid(GUI_MENU_MAINMENU_DYNAMIC_DATA_T *data_ptr)
{
    BOOLEAN is_func_valid = FALSE;
    BOOLEAN is_delete_func_valid = FALSE;
    BOOLEAN is_para1_valid = FALSE;
    BOOLEAN is_para2_valid = FALSE;

    if (PNULL == data_ptr)
    {
        return FALSE;
    }

    is_func_valid = (PNULL != data_ptr->link_function_ptr);
    is_delete_func_valid = (PNULL != data_ptr->delete_function_ptr);
    is_para1_valid = ((0 == data_ptr->parm1_size)
        || (PNULL != data_ptr->param1ptr && data_ptr->parm1_size < RUNDATA_FILE_SIZE));
    is_para2_valid = ((0 == data_ptr->parm2_size)
        || (PNULL != data_ptr->param2ptr && data_ptr->parm2_size < RUNDATA_FILE_SIZE));

    //SCI_TRACE_LOW:"[Mmimenu_synchronize].MMIMENU_DyListGetRunInfo: is_func_valid = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2976_112_2_18_2_35_48_240,(uint8*)"d" ,is_func_valid);
    //SCI_TRACE_LOW:"[Mmimenu_synchronize].MMIMENU_DyListGetRunInfo: is_para1_valid = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2977_112_2_18_2_35_48_241,(uint8*)"d", is_para1_valid);
    //SCI_TRACE_LOW:"[Mmimenu_synchronize].MMIMENU_DyListGetRunInfo: is_para2_valid = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_2978_112_2_18_2_35_48_242,(uint8*)"d", is_para2_valid);
    return (is_func_valid && is_para1_valid && is_para2_valid && is_delete_func_valid);
}


/*****************************************************************************/
//  Description : 显示文件系统中存储的图片信息
//  Global resource dependence : 
//  Author: jin.wang
//  Note:
/*****************************************************************************/
LOCAL void DisplayDyMenuIconInFile(
                            GUIMENU_MAINMENU_DY_ITEM_T *item_ptr, 
                            GUI_RECT_T *disp_rect_ptr,
                            GUI_LCD_DEV_INFO *lcd_dev_ptr 
                            )
{
    GUI_POINT_T img_point = {0};
    GUIIMG_BITMAP_T img_map_info = {0};
    GUIIMG_DISPLAYBMP_PARAM_T display_param = {0};

    if (PNULL == item_ptr || PNULL == disp_rect_ptr || PNULL == lcd_dev_ptr
        || PNULL == item_ptr->ori_data_ptr
        || PNULL == item_ptr->ori_data_ptr->dynamic_menu_info.icon_ptr)
    {
        //SCI_TRACE_LOW:"DisplayDyMenuIconInFile: input is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3001_112_2_18_2_35_48_243,(uint8*)"");
        return;
    }
    
    img_point.x = disp_rect_ptr->left;
    img_point.y = disp_rect_ptr->top;

    img_map_info.bit_ptr    = item_ptr->ori_data_ptr->dynamic_menu_info.icon_ptr;
    img_map_info.img_width  = item_ptr->ori_data_ptr->dynamic_menu_info.icon_width;
    img_map_info.img_height = item_ptr->ori_data_ptr->dynamic_menu_info.icon_height;
    img_map_info.date_type  = GUIIMG_DATE_TYPE_ARGB888;
    
    display_param.is_horizontal = FALSE;
    display_param.rect_ptr = disp_rect_ptr;
    display_param.point_ptr = &img_point;
    display_param.bitmap_ptr = &img_map_info;
    display_param.lcd_dev_ptr = lcd_dev_ptr;
    display_param.is_transparent_mode = TRUE;
    display_param.transparent_color = 0x00;
    GUIIMG_DisplayBmpEx(&display_param);
}

/*****************************************************************************/
// 	Description : indication to other module that file has updated
//	Global resource dependence : none
//  Author: tao.xue
//	Note:
/*****************************************************************************/
PUBLIC void MMIMENU_UpdateEventInd(void)
{
#ifdef SEARCH_SUPPORT
    MMIAPISEARCH_AppUpdateInd(MMISEARCH_TYPE_APP);
#endif
}

/*****************************************************************************/
// Description : 增加一个动态menu
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
// menu_id输入"0",由系统动态生成
/*****************************************************************************/
PUBLIC MMIMENU_SYN_VALUE_E MMIMENU_DyAppendItem(
												GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
												GUIMENU_FILE_DEVICE_E store_idx
												)
{
    BOOLEAN is_data_valid  = FALSE;
    BOOLEAN is_item_exist = FALSE;
    BOOLEAN is_first_time = FALSE;
    BOOLEAN is_data_added = FALSE;
	MMIMENU_SYN_VALUE_E result = MMIMENU_SYN_VALUE_SUC;
    uint32 total_item_num = 0;
    uint8* icon_ptr = PNULL;
    GUIMENU_MAINMENU_DYNAMIC_T all_dy_data = {0};
        
    if (!IsDevEnoughSpace(store_idx))
    {
		result = MMIMENU_SYN_VALUE_NOSPACE;
        return result;
    }

    is_data_valid = IsValidDyMenuInfo(dynamic_item_ptr);

    if (!is_data_valid)
    {
		result = MMIMENU_SYN_VALUE_INVALID;
        return result;
    }
    
    is_item_exist = IsItemExist(dynamic_item_ptr);

    if (is_item_exist)
    {
		result = MMIMENU_SYN_VALUE_EXIST;
        return result;
    }
    
    //如果是第一次安装读出来的数据可能是空,或者版本不同也无法加载 
    ReadDevMenuData(&icon_ptr, 
                    &all_dy_data,
                    store_idx);    
       
    //清除位置信息，防止应用传入参数异常
    dynamic_item_ptr->page_index = DYNAMIC_MAINMENU_DEFAULT_PAGE_INDEX;
    dynamic_item_ptr->item_index = DYNAMIC_MAINMENU_DEFAULT_ITEM_INDEX;

    //读取已有数据，失败===第一次存储
    is_first_time = IsFirstTimeAppend(&all_dy_data);

    if (is_first_time)
    {
        total_item_num = 0;
    }
    else
    {
        total_item_num = all_dy_data.dy_menu_num;
    }

    total_item_num += 1;
    
    do
    {
        BOOLEAN result_store_param = FALSE;
        BOOLEAN result_store_data = FALSE;

        if (total_item_num > DYNAMIC_MAINMENU_NUM_LIMIT) break;
        
        dynamic_item_ptr->menu_id = GenMenuId();
        
        MMIMENU_DySetItemString(dynamic_item_ptr);
        //存储
        //2.1 数据(包括图片的存储)
        result_store_data = StoreInfoData(dynamic_item_ptr,
                                          total_item_num,
                                          TRUE,
                                          store_idx,
                                          is_first_time,
                                          icon_ptr, 
                                          &all_dy_data);

        if (!result_store_data)
        {
            break;
        }

        // 2.2 参数
        result_store_param = StoreParamData(dynamic_item_ptr,
                                            store_idx);
        
        if (!result_store_param)
        {
            break;
        }
        
        is_data_added = TRUE;
    }
    while (0);

    if (!is_data_added)
    {
		result = MMIMENU_SYN_VALUE_ERROR;
        //文件系统中信息
        DeleteItemData(store_idx, &all_dy_data, icon_ptr, dynamic_item_ptr);
    }

    if (PNULL != icon_ptr)
    {
        SCI_FREE(icon_ptr);
    }
    
    if (PNULL != all_dy_data.data_ptr)
    {
        SCI_FREE(all_dy_data.data_ptr);
    }
    
    MMIMENU_UpdateEventInd();

    return result;
}

/*****************************************************************************/
// Description : 删除一个动态item
// Global resource dependence :
// Author:jin.wang
// Note:成功返回TRUE，失败返回FALSE
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyDeleteItem(
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
)
{
    GUIMENU_MAINMENU_DYNAMIC_T dy_data_info[GUIMENU_MAINMENU_STORE_STYLE_NUM] = {0};
    uint8* icon_ptr[GUIMENU_MAINMENU_STORE_STYLE_NUM] = {PNULL};
    GUIMENU_FILE_DEVICE_E store_idx = 0; 
    BOOLEAN is_got_menu_id = FALSE;
    //uint32 total_menu_count = 0;
    uint16 data_index = 0;

    for (store_idx = GUIMENU_DEVICE_SYSTEM; store_idx < GUIMENU_DEVICE_MAX; store_idx++)
    {        
        BOOLEAN is_read = FALSE;
        
        data_index = (uint16)store_idx - GUIMENU_DEVICE_SYSTEM;
        
        is_read = ReadDevMenuData(&icon_ptr[store_idx], 
                        &dy_data_info[store_idx],
                        store_idx);

        if (!is_read)
        {
            if (PNULL != icon_ptr[data_index])
            {
                SCI_FREE(icon_ptr[data_index]);
            }
            
            if (PNULL != dy_data_info[data_index].data_ptr)
            {
                SCI_FREE(dy_data_info[data_index].data_ptr);
            }

            dy_data_info[data_index].check_info = DYNAMIC_MAINMENU_DEFAULT_CHECK_INFO;
            dy_data_info[data_index].dy_menu_num = 0;
            
            continue;
        }
        
        is_got_menu_id = GetDeleteItemMenuId(dynamic_item_ptr,
                                             dy_data_info[store_idx].data_ptr,
                                             dy_data_info[store_idx].dy_menu_num);

        if (!is_got_menu_id)
        {
            continue;
        }
        
        //先删除文件系统中存储的文件
        DeleteItemData(store_idx, &dy_data_info[data_index], icon_ptr[store_idx], dynamic_item_ptr);

        //更新缓存的数据
        //GetAllDyMenuData(&total_menu_count);
        break;
    }

    for (store_idx = GUIMENU_DEVICE_SYSTEM; store_idx < GUIMENU_DEVICE_MAX; store_idx++)
    {        
        data_index = (uint16)store_idx - GUIMENU_DEVICE_SYSTEM;

        if (PNULL != dy_data_info[data_index].data_ptr)
        {
            SCI_FREE(dy_data_info[data_index].data_ptr);
        }

        if (PNULL != icon_ptr[store_idx])
        {
            SCI_FREE(icon_ptr[store_idx]);
        }        
    }

    MMIMENU_UpdateEventInd();

    return TRUE;
}


/*****************************************************************************/
// Description : 根据启动函数类别清除动态菜单
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyDeleteItemByFunc(
    DY_MENU_ITEM_LINK_FUNC_T link_func_ptr
)
{
    uint32 total_dy_menu = 0;
    uint16 i = 0;

    if (PNULL == link_func_ptr)
    {
        return FALSE;
    }

    GetAllDyMenuData(&total_dy_menu);

    if (0 == total_dy_menu)
    {
        ResetData();
        
        return TRUE;
    }

    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        uint16 item_idx = 0;
        uint16 item_new_idx = 0;
        GUIMENU_FILE_DEVICE_E store_idx = i + GUIMENU_DEVICE_SYSTEM;
        
        for (item_idx = 0; item_idx < s_dy_data_array[i].dy_menu_num;)
        {
            if (s_dy_data_array[i].data_ptr[item_idx].dynamic_menu_info.link_function_ptr
                    == link_func_ptr)
            {
                DeleteItemData(store_idx,
                               &s_dy_data_array[i],
                               s_icon_data_array_ptr[i],
                               &(s_dy_data_array[i].data_ptr[item_idx]));
            }
            else
            {
                if (item_new_idx != item_idx)
                {
                    uint32 ori_offset = 0;
                    uint32 des_offset = 0;
                    uint32 data_size = 0;
                    
                    des_offset = item_new_idx * ICON_FILE_SIZE;
                    ori_offset = item_idx * ICON_FILE_SIZE;
                    data_size = ICON_FILE_SIZE;

                    SCI_MEMCPY((s_icon_data_array_ptr[i] + des_offset), 
                        (s_icon_data_array_ptr[i] + ori_offset), 
                        data_size);
                    
                    s_dy_data_array[i].data_ptr[item_new_idx] = s_dy_data_array[i].data_ptr[item_idx];
                }

                item_idx++;
                item_new_idx++;
            }
        }

        s_dy_data_array[i].dy_menu_num = item_new_idx;
    }

    MMIMENU_UpdateEventInd();

    return TRUE;
}


/*****************************************************************************/
// Description : 判断启动函数、参数是否正确
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyIsRunInfoValid(GUI_MENU_MAINMENU_DYNAMIC_DATA_T *data_ptr)
{
    return IsRunInfoValid(data_ptr);
}


/*****************************************************************************/
// Description : 根据menu_id获取slide模式下动态菜单运行数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySlideGetRunInfo(
    MMI_MENU_ID_T menu_id,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr
)
{
    return GetItemInfo(menu_id, item_info_ptr,
                       GUIMENU_GET_INFO_PARA, GUIMENU_READFILE_NONE_STYLE);
}


/*****************************************************************************/
// Description : 根据list_index获取list模式下动态菜单运行数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyListGetRunInfo(
    uint16 list_index,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr
)
{
    BOOLEAN is_get_menuid = FALSE;
    MMI_MENU_ID_T menu_id = 0;
    BOOLEAN is_get_run_info = FALSE;

    is_get_menuid = GetListItemMenuId(list_index, &menu_id);

    if (is_get_menuid)
    {
        is_get_run_info = GetItemInfo(menu_id,
                                      item_info_ptr,
                                      GUIMENU_GET_INFO_PARA,
                                      GUIMENU_READFILE_NONE_STYLE);
        //SCI_TRACE_LOW:"[Mmimenu_synchronize].MMIMENU_DyListGetRunInfo: is_get_run_info = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3354_112_2_18_2_35_49_244,(uint8*)"d" ,is_get_run_info);
    }

    return is_get_run_info;
}


/*****************************************************************************/
// Description : 根据menu_id获取slide模式下动态菜单图片数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySlideGetIcon(
    MMI_MENU_ID_T menu_id,
    uint8 *icon_info_ptr
)
{
    GUIMENU_MAINMENU_DY_ITEM_T item_info = {0};
    BOOLEAN is_get_info = FALSE;
    BOOLEAN is_get_icon = FALSE;
    
    is_get_info = GetItemInfo(menu_id,
                              &item_info,
                              GUIMENU_GET_INFO_ICON,
                              GUIMENU_READFILE_DECODE);

    if (is_get_info
            && PNULL != item_info.ori_data_ptr
            && PNULL != item_info.ori_data_ptr->dynamic_menu_info.icon_ptr)
    {
        is_get_icon = TRUE;
        icon_info_ptr = item_info.ori_data_ptr->dynamic_menu_info.icon_ptr;
    }
    else
    {
        if (PNULL != item_info.ori_data_ptr && PNULL != item_info.ori_data_ptr->dynamic_menu_info.icon_ptr)
        {
            SCI_FREE(item_info.ori_data_ptr->dynamic_menu_info.icon_ptr);
        }
    }

    return is_get_icon;
}


/*****************************************************************************/
// Description : 根据menu_id获取slide模式下动态菜单显示数据(不包括图片数据)
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySlideGetDispInfo(
    MMI_MENU_ID_T menu_id,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr
)
{
    return GetItemInfo(menu_id,
                       item_info_ptr,
                       GUIMENU_GET_INFO_NONE,
                       GUIMENU_READFILE_NONE_STYLE);
}


/*****************************************************************************/
// Description : 根据menu_id获取list模式下动态菜单显示数据(包括图片数据)
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyListGetDispInfo(
    uint16 list_index,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr
)
{
    BOOLEAN is_get_menuid = FALSE;
    MMI_MENU_ID_T menu_id = 0;
    BOOLEAN is_get_run_info = FALSE;

    is_get_menuid = GetListItemMenuId(list_index, &menu_id);

    if (is_get_menuid)
    {
        is_get_run_info = GetItemInfo(menu_id,
                                      item_info_ptr,
                                      GUIMENU_GET_INFO_ICON,
                                      GUIMENU_READFILE_ORIG);
    }

    return is_get_run_info;
}


/*****************************************************************************/
// Description : 根据menu_id获取list模式下动态菜单显示数据(不包括图片数据)
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyListGetDispInfoEx(
    uint16 list_index,
    GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr
)
{
    BOOLEAN is_get_menuid = FALSE;
    MMI_MENU_ID_T menu_id = 0;
    BOOLEAN is_get_run_info = FALSE;

    is_get_menuid = GetListItemMenuId(list_index, &menu_id);

    if (is_get_menuid)
    {
        is_get_run_info = GetItemInfo(menu_id,
                                      item_info_ptr,
                                      GUIMENU_GET_INFO_NONE,
                                      GUIMENU_READFILE_NONE_STYLE);
    }

    return is_get_run_info;
}


/*****************************************************************************/
// Description :  读取动态菜单数据
// Global resource dependence :
// Author: jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySetItemData(
    MMIMAINMENU_PAGE_ITEM_INFO_T *item_page_ptr,
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    MMI_IMAGE_ID_T select_icon_id
)
{
    uint32 total_dy_menu = 0;
    
    if (PNULL == item_page_ptr || PNULL == menu_ctrl_ptr)
    {
        return FALSE;
    }

    if (IsNeedSyn())
    {
        GetAllDyMenuData(&total_dy_menu);

        if (total_dy_menu == 0)
        {
            ResetData();
            return FALSE;
        }
    }

    InsertDyItem(item_page_ptr, menu_ctrl_ptr, select_icon_id);
    
    return TRUE;
}


/*****************************************************************************/
// Description : 读取所有菜单数据
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyGetAllItemData(void)
{
    BOOLEAN read_result = FALSE;
    uint32 total_menu_count = 0;

    if (!IsNeedSyn())
    {
        //不需要重新读取，也默认为数据读取成功
        return TRUE;
    }

    GetAllDyMenuData(&total_menu_count);
    
    if (total_menu_count > 0)
    {
        read_result = TRUE;
    }
    else
    {
        ResetData();
    }

    return read_result;
}


/*****************************************************************************/
// Description : 读取动态主菜单总个数
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC uint32 MMIMENU_DyGetItemCount(void)
{
    uint16 i = 0;
    uint32 total_menu_count = 0;

    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        total_menu_count += s_dy_data_array[i].dy_menu_num;
    }

    return total_menu_count;
}


/*****************************************************************************/
// Description :设置动态主菜单位置信息
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DySaveItemPos(
    GUIMENU_CTRL_T *menu_ctrl_ptr,
    MMIMAINMENU_PAGE_ITEM_INFO_T *page_info_ptr
)
{
    GUIMENU_FILE_DEVICE_E store_index = GUIMENU_DEVICE_MAX;

    if (PNULL == page_info_ptr || PNULL == menu_ctrl_ptr)
    {
        return;
    }

    for (store_index = GUIMENU_DEVICE_SYSTEM; store_index < GUIMENU_DEVICE_MAX; store_index++)
    {
        if (!IsDevEnoughSpace(store_index)) continue;

        if (s_dy_data_array[store_index].dy_menu_num > 0)
        {
            ResetItemPosData(&s_dy_data_array[store_index],
                             page_info_ptr,
                             menu_ctrl_ptr->item_total_num,
                             menu_ctrl_ptr->is_move_state,
                             store_index);
        }
    }

    return;
}


/*****************************************************************************/
// Description :判断动态菜单是否需要同步
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DyCheckIsSyn(void)
{
    return IsNeedSyn();
}


/*****************************************************************************/
// Description :动态主菜单初始化
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DyInit(void)
{
    uint32 total_menu_count = 0;
    
    ResetData();

    GetAllDyMenuData(&total_menu_count);
    
    if (0 == total_menu_count)
    {
        ResetData();
    }
    
    return;
}


/*****************************************************************************/
// Description : 释放启动参数内存
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DyFreeParmInfo(
    GUIMENU_MAINMENU_DY_ITEM_T *dy_item_ptr
)
{
    if (PNULL == dy_item_ptr)
    {
        return;
    }
    
    if (PNULL != dy_item_ptr->ori_data_ptr->dynamic_menu_info.param1ptr)
    {
        SCI_FREE(dy_item_ptr->ori_data_ptr->dynamic_menu_info.param1ptr);
    }

    if (PNULL != dy_item_ptr->ori_data_ptr->dynamic_menu_info.param2ptr)
    {
        SCI_FREE(dy_item_ptr->ori_data_ptr->dynamic_menu_info.param2ptr);
    }

    return;
}


/*****************************************************************************/
// Description : 释放动态菜单数据数组内存
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DyFreeData(void)
{
    return;
}

/*****************************************************************************/
// Description : 释放动态菜单数据数组内存
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DyDestroy(void)
{
    uint16 i = 0;
    
    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++) //释放内存
    {
        s_dy_data_array[i].check_info = DYNAMIC_MAINMENU_DEFAULT_CHECK_INFO;
        s_dy_data_array[i].dy_menu_num = 0;

        if (PNULL != s_dy_data_array[i].data_ptr)
        {
            SCI_FREE(s_dy_data_array[i].data_ptr);
        }

        if (PNULL != s_icon_data_array_ptr[i])
        {
            SCI_FREE(s_icon_data_array_ptr[i]);
        }
    }
    return;
}

/*****************************************************************************/
// Description : 显示不正确的图片
// Global resource dependence :
// Author:nan.ji
// Note:
/*****************************************************************************/
LOCAL void DrawDefaultImage(MMI_WIN_ID_T win_handle, GUI_LCD_DEV_INFO *lcd_dev_ptr)
{
	GUI_RECT_T icon_rect = {0, 0, MMITHEME_MENU_ITEM_WIDTH - 1, MENUICON_HEIGHT - 1};
	uint16     img_width = 0;
	uint16     img_height = 0;
	
	GUIRES_GetImgWidthHeight(&img_width, &img_height, IMAGE_MAINMENU_ICON_UNKNOWN, win_handle );
	icon_rect = GUI_GetCenterRect( icon_rect, img_width, img_height );		
	GUIRES_DisplayImg(PNULL,
		&icon_rect,
		PNULL,
		win_handle,
		IMAGE_MAINMENU_ICON_UNKNOWN, 
		lcd_dev_ptr);  
}

/*****************************************************************************/
//  Description : 显示动态menu图片
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMIAPIMENU_DisplayDynamicMenuIcon(
                            GUIMENU_MAINMENU_INFO_T *item_ptr, 
                            GUI_RECT_T *icon_rect_ptr,
                            GUI_RECT_T *disp_rect_ptr,
                            GUI_LCD_DEV_INFO *lcd_dev_ptr, 
                            MMI_WIN_ID_T win_handle
                            )
{     
    GUI_RECT_T temp_rect = {0};
    GUI_RECT_T dis_rect = {0};

    //if (MMIAPIUDISK_UdiskIsRun())
	//{
    //    DrawDefaultImage(win_handle,lcd_dev_ptr);
    //    return;
    //}
    if (PNULL == item_ptr /*|| PNULL == item_ptr->menu_data.dynamic_menu_ptr */
        ||PNULL == item_ptr->menu_data.dynamic_menu.ori_data_ptr 
        || PNULL == disp_rect_ptr || PNULL == lcd_dev_ptr || PNULL == icon_rect_ptr)
    {
        return;
    }
    //记录显示区域
    dis_rect.top = disp_rect_ptr->top;
    dis_rect.left = disp_rect_ptr->left;
    dis_rect.right = disp_rect_ptr->right;
    dis_rect.bottom = disp_rect_ptr->bottom;
    
    temp_rect.top = disp_rect_ptr->top;
    temp_rect.left = disp_rect_ptr->left;
    temp_rect.right = disp_rect_ptr->right;
    temp_rect.bottom = disp_rect_ptr->bottom;
    
	//if (!MMIAPIUDISK_UdiskIsRun())
	//{
		//如果有icon数据，显示
		if (item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.has_icon)
		{
			BOOLEAN is_get_icon = FALSE;
			
			//图片可能与主菜单下图片尺寸不统一，需要重新计算
			temp_rect = GUI_GetCenterRect(temp_rect, 
				item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.icon_width, 
				item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.icon_height);
			
			disp_rect_ptr->top = temp_rect.top;
			disp_rect_ptr->left = temp_rect.left;
			disp_rect_ptr->right = temp_rect.right;
			disp_rect_ptr->bottom = temp_rect.bottom;
			
			item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.icon_ptr = PNULL;
			
			is_get_icon = MMIMENU_DySlideGetIcon(item_ptr->menu_data.dynamic_menu.ori_data_ptr->menu_id,
				item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.icon_ptr);
            
			if (is_get_icon
				&& PNULL != item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.icon_ptr)
			{
				DisplayDyMenuIconInFile(&item_ptr->menu_data.dynamic_menu, disp_rect_ptr,lcd_dev_ptr);
				SCI_FREE(item_ptr->menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.icon_ptr);
				//SCI_TRACE_LOW:"DisplayDyMenuIconInFile: sucess!"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3709_112_2_18_2_35_50_245,(uint8*)"");
			}
			else//读取失败
			{
				GUIRES_DisplayImg(PNULL,
					&dis_rect,
					icon_rect_ptr,
					win_handle,
					IMAGE_MAINMENU_ICON_UNKNOWN, 
					lcd_dev_ptr);  
				//SCI_TRACE_LOW:"Display default icon --- IMAGE_MAINMENU_ICON_DYNAMIC_DEFAUL : sucess!"
				SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3719_112_2_18_2_35_50_246,(uint8*)"");
			}
		}
		else
		{
			DrawDefaultImage(win_handle,lcd_dev_ptr);
		}
	//}
	//else
	//{
	//	DrawDefaultImage(win_handle,lcd_dev_ptr);
	//}
}

/*****************************************************************************/
// Description : 判断一个动态菜单数据是否存在
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_IsDyItemExist(
    MMI_MENU_ID_T menu_id
)
{
    GUIMENU_MAINMENU_DY_ITEM_T item_info = {0};
    
    return GetItemInfo(menu_id,
                       &item_info,
                       GUIMENU_GET_INFO_NONE,
                       GUIMENU_READFILE_NONE_STYLE);

}

/*****************************************************************************/
// Description : 设置高亮图片id
// Global resource dependence :
// Author:jin.wang
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_SetSelectedId(
    MMI_IMAGE_ID_T select_icon_id
)
{
    SetSelectedId(select_icon_id);
}

/*****************************************************************************/
// Description : 设置text数据组
// Global resource dependence :
// Author:nan.ji
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DySetItemStringArray(
								 GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr,
                                 GUIMENU_MAINMENU_DYNAMIC_STRING_DATA_T *string_arr,
                                 uint16 arr_num
								 )
{
	uint16 i = 0;

    if (PNULL == dynamic_item_ptr || PNULL == string_arr || 0 == arr_num || arr_num > GUIMENU_DYMAINMENU_STR_LANG_MAX_NUM)
    {
        //SCI_TRACE_LOW:"MMIMENU_DyAppendItemStringArray: input is error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3775_112_2_18_2_35_50_247,(uint8*)"");
        return;
    }
    
    for (i = 0; i < arr_num; i++)
	{
        if(MMIAPICOM_Wstrlen(string_arr[i].text)>GUIMENU_DYMAINMENU_STR_MAX_NUM)
        {
            //SCI_TRACE_LOW:"MMIMENU_DyAppendItemStringArray: input text is too long!"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3783_112_2_18_2_35_50_248,(uint8*)"");
            return;
        }
        dynamic_item_ptr->dynamic_menu_info.str_lang_info[i].language_type = string_arr[i].language_type;
        MMIAPICOM_Wstrcpy(dynamic_item_ptr->dynamic_menu_info.str_lang_info[i].text, string_arr[i].text);		
	}
}

/*****************************************************************************/
// Description : 设置显示用的text
// Global resource dependence :
// Author:nan.ji
// Note:当java和WRE传入不一样时，默认设置显示数据为数据组首个data。
/*****************************************************************************/
PUBLIC void MMIMENU_DySetItemString(
									GUIMENU_MAINMENU_DYNAMIC_INFO_T *dynamic_item_ptr
									)
{
	if (PNULL == dynamic_item_ptr)
	{
        //SCI_TRACE_LOW:"MMIMENU_DySetItemString: input is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3801_112_2_18_2_35_50_249,(uint8*)"");
        return;
	}
	if (0 == dynamic_item_ptr->dynamic_menu_info.text[0])
	{
		MMIAPICOM_Wstrcpy(dynamic_item_ptr->dynamic_menu_info.text,dynamic_item_ptr->dynamic_menu_info.str_lang_info[0].text);
        dynamic_item_ptr->dynamic_menu_info.text_len = MMIAPICOM_Wstrlen(dynamic_item_ptr->dynamic_menu_info.str_lang_info[0].text);
	}
	else
	{
		if(MMIAPICOM_Wstrlen(dynamic_item_ptr->dynamic_menu_info.text)>GUIMENU_DYMAINMENU_STR_MAX_NUM)
		{
			//SCI_TRACE_LOW:"MMIMENU_DySetItemString: input is error!"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3813_112_2_18_2_35_50_250,(uint8*)"");
			return;
		}
		MMIAPICOM_Wstrcpy(dynamic_item_ptr->dynamic_menu_info.str_lang_info[0].text,dynamic_item_ptr->dynamic_menu_info.text);
	}
}

/*****************************************************************************/
// Description : 获取当前语言环境对应的text
// Global resource dependence :
// Author:nan.ji
// Note:
/*****************************************************************************/
PUBLIC void MMIMENU_DyGetItemString(
								 GUIMENU_MAINMENU_DY_ITEM_T *dynamic_item_ptr
                                 )
{
	uint16 i = 0;
	MMISET_LANGUAGE_TYPE_E lang_type = MMISET_LANGUAGE_ENGLISH;
    
	if (PNULL == dynamic_item_ptr)
    {
        //SCI_TRACE_LOW:"MMIMENU_DyGetItemString: input is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3833_112_2_18_2_35_50_251,(uint8*)"");
        return;
    }

        //add
    if (PNULL == dynamic_item_ptr->ori_data_ptr)
    {
        //SCI_TRACE_LOW:"MMIMENU_DyGetItemString: input is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMENU_SYNCHRONIZE_3840_112_2_18_2_35_50_252,(uint8*)"");
        return;
    }
    
	MMIAPISET_GetLanguageType(&lang_type);
    for (i = 0; i < GUIMENU_DYMAINMENU_STR_LANG_MAX_NUM; i++)
	{
        if (lang_type == GetLangType(dynamic_item_ptr->ori_data_ptr->dynamic_menu_info.str_lang_info[i].language_type))
        {
            memset(dynamic_item_ptr->ori_data_ptr->dynamic_menu_info.text, 0, (GUIMENU_DYMAINMENU_STR_MAX_NUM + 1)*sizeof(wchar));
			MMIAPICOM_Wstrcpy(dynamic_item_ptr->ori_data_ptr->dynamic_menu_info.text, dynamic_item_ptr->ori_data_ptr->dynamic_menu_info.str_lang_info[i].text);
            dynamic_item_ptr->ori_data_ptr->dynamic_menu_info.text_len = MMIAPICOM_Wstrlen(dynamic_item_ptr->ori_data_ptr->dynamic_menu_info.str_lang_info[i].text);
            break;
        }
	}
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
// Description : 根据menu_id获取slide模式下菜单项是否需要显示图标
// Global resource dependence :
// Author:zack.zhang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySlideHasIcon(MMI_MENU_ID_T menu_id, int32 *has_icon)
{
    uint16 i = 0;
    uint16 item_index = 0;
    BOOLEAN is_find = FALSE;

    for (i = 0; i < GUIMENU_MAINMENU_STORE_STYLE_NUM; i++)
    {
        if (s_dy_data_array[i].dy_menu_num == 0) continue;

        if (PNULL == s_dy_data_array[i].data_ptr) continue;

        for (item_index = 0; item_index < s_dy_data_array[i].dy_menu_num; item_index++)
        {
            if (menu_id != s_dy_data_array[i].data_ptr[item_index].menu_id)
            {
                continue;
            }
            is_find = TRUE;

            if (s_dy_data_array[i].data_ptr[item_index].dynamic_menu_info.has_icon)
            {
                *has_icon = TRUE;
            }
            else
            {
                *has_icon = FALSE;
            }
            break;
        }

        if (is_find)
        {
            break;
        }
    }

    return is_find;
}

/*****************************************************************************/
// Description : 根据menu_id获取slide模式下菜单项数据
// Global resource dependence :
// Author:zack.zhang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySlideGetItemInfo(
                                            MMI_MENU_ID_T menu_id,
                                            GUIMENU_MAINMENU_DY_ITEM_T *item_info_ptr
                                            )
{
    return GetItemInfo(menu_id, item_info_ptr, GUIMENU_GET_INFO_ALL, GUIMENU_READFILE_BOTH);
}

/*****************************************************************************/
// Description : 保存菜单项数据
// Global resource dependence :
// Author:zack.zhang
// Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIMENU_DySlideSaveItemInfo(
                                              MMI_MENU_ID_T menu_id,
                                              GUIMENU_MAINMENU_DY_ITEM_T *menu_item_ptr
                                              )
{
    int32  i = 0;
	uint8* icon_ptr = PNULL;
    uint32 new_check_info = 0;
    BOOLEAN is_found = FALSE;
    BOOLEAN is_decoded = FALSE;
    BOOLEAN is_data_saved = FALSE;
    wchar data_file_name[DYNAMIC_MAINMENU_DATA_FILE_LEN + 1]  = {0};
    GUIMENU_FILE_DEVICE_E storage = GUIMENU_DEVICE_UDISK;
    GUI_MENU_MAINMENU_DYNAMIC_DATA_T *menu_data_ptr = NULL;
    GUIMENU_MAINMENU_DYNAMIC_T all_dy_data = {0};
    MMIMENU_SYN_ICON_T scaled_info = {0};

    if(menu_item_ptr != NULL)
    {
        is_decoded = DecodeIconData(storage, menu_item_ptr->ori_data_ptr, &scaled_info);

        if(!is_decoded)
        {
            return FALSE;
        }
            
        storage = menu_item_ptr->store_dev;
        menu_data_ptr = &menu_item_ptr->ori_data_ptr->dynamic_menu_info;
        SCI_TRACE_LOW("[SKYSHORTCUT] is_decoded=%d scaled_info.width=%d scaled_info.height=%d", is_decoded, scaled_info.width, scaled_info.height);
        ReadDevMenuData(&icon_ptr, &all_dy_data, storage);    
        SCI_TRACE_LOW("[SKYSHORTCUT] total_num=%d", all_dy_data.dy_menu_num);
        for(i=0; i<all_dy_data.dy_menu_num; i++)
        {
            if(menu_id == all_dy_data.data_ptr[i].menu_id)
            {
                all_dy_data.data_ptr[i].dynamic_menu_info.has_icon = TRUE;
                all_dy_data.data_ptr[i].dynamic_menu_info.icon_datasize = menu_data_ptr->icon_datasize;
                all_dy_data.data_ptr[i].dynamic_menu_info.icon_height = scaled_info.height;
                all_dy_data.data_ptr[i].dynamic_menu_info.icon_width= scaled_info.width;
                all_dy_data.data_ptr[i].dynamic_menu_info.icon_ptr = scaled_info.target_buf_ptr;
                is_found = TRUE;
                break;
            }
        }

        if(is_found == TRUE)
        {
            SCI_MEMCPY(icon_ptr+i*ICON_FILE_SIZE, scaled_info.target_buf_ptr, ICON_FILE_SIZE);
			is_data_saved = StoreInfoData(menu_item_ptr->ori_data_ptr, 
                             all_dy_data.dy_menu_num,
                             FALSE,
                             storage,
                             FALSE,
                             icon_ptr,
                             &all_dy_data);

            if(is_data_saved)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}
#endif

#endif//DYNAMIC_MAINMENU_SUPPORT





