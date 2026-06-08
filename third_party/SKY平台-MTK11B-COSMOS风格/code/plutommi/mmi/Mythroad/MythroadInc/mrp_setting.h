#ifndef __MRP_PAL_SETTING_H__
#define __MRP_PAL_SETTING_H__


#ifndef __MR_CFG_FEATURE_OVERSEA__
#define MR_APPLIST_TITLE                                   "游戏中心"
#else
#define MR_APPLIST_TITLE                                   "Game Center"
#endif


typedef enum{
	MR_PLATEX_CODE_SETTING_SUPPORT = 0,
	MR_PLATEX_CODE_SETTING_GET_GMT_TZ,
	MR_PALTEX_CODE_SETTING_GET_SUPPORT_LANGS,
	MR_PLATEX_CODE_SETTING_WIFI_EXIST,
	MR_PLATEX_CODE_SETTING_GET_HANDSETID_EX, //扩展的厂商机型
	MR_PLATEX_CODE_SETTING_GET_SYSTEM_IMAGE_PATH = 10,
	MR_PLATEX_CODE_SETTING_IMAGE_ENCODE,
#ifdef __MR_CFG_SET_SMS_STATUS__
	MR_PLATEX_CODE_SETTING_SMS_SET_STATUS  = 15,
#endif
/* 52_xsgrz_patch begin */
	MR_PLATEX_CODE_SETTING_CONFIG_SAMPLING_PERIOD = 0x30,
	MR_PLATEX_CODE_SETTING_CONFIG_MOVE_OFFSET = 0x31,
	MR_PLATEX_CODE_SETTING_MAX  = 0x7FFF
/* 52_xsgrz_patch end */
}mr_platEx_code_setting_enum;


typedef struct{
	int32 setType;
	U8 *filename;
}mr_wallpaper_info_t;


typedef struct{
	int32 themeid;
	char *folder;
}mr_theme_info_t;


/**
 * \brief 设置WALLPAPER和SCREENSAVER
 *
 */
void mr_setting_theme_set_wallpaper_and_screensaver(S8* path);


/**
 * \brief 设置wallpaper
 *
 * \param req	[in] 请求数据结构
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILD
 */
int32 mr_setting_set_wallpaper(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 设置铃声
 *
 * \param ringSet	[in]请求数据结构
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILD
 */
int32 mr_setting_set_ring(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 设置主题
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILD
 */
int32 mr_setting_set_theme(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 回复主题
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILD
 */
int32 mr_setting_restore_theme(int32 param);


/**
 * \brief 获取当前主题
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILD
 */
int32 mr_setting_get_active_theme_id(int32 param);


/**
 * \brief 获取当前的情景模式
 *
 * \return 当前的情景模式
 */
int32 mr_setting_get_cur_scene(int32 param);


/**
 * \brief 获取当前的语言设置
 *
 * \return MR_LANGUAGE
 */
int32 mr_setting_get_cur_lang(int32 param);


/**
 * \brief 检查是否支持WIFI
 *
 */
int32 mr_setting_check_wifi_support(int32 param);


/**
 * \brief WIFI开关
 *
 * \param s	[in]
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILD
 */
int32  mr_setting_wifi_switch(int32 s);


/**
 * \brief 检查屏幕类型
 *
 * \return MR_SCREEN_TYPE
 */
int32 mr_setting_get_screen_type(int32 param);


/**
 * \brief 设置日期
 */
int32 mr_setting_set_datetime(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 
 */
int32 mr_setting_set_theme_mainmenu_index(int32 index);


/**
 * \brief 获取APPLIST标题
 */
int32 mr_setting_get_applist_title(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 字体信息
 */
int32 mr_setting_get_font_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

/**
  * 获取系统默认的相册路径
  *
  * @param[out] output 指向路径首地址，UNICODE编码
  *
  * @return MR_SUCCESS 成功，MR_FAILED 失败，MR_IGNORE 不支持
  */
int32 mr_setting_get_system_image_path(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

typedef struct {
	int16 w;	// 图像宽度
	int16 h;	// 图像高度
	void* buf;	// 图像数据地址
	int16* path;	// 保存的文件路径，UNICODE编码
	int32 type;	// 保存的文件类型，IMG_TYPE
} mr_image_encode_info;

/**
  * 把RGB565的数据保存为指定格式的图片
  *
  * @param input 指向mr_image_encode_info
  *
  * @return MR_SUCCESS 成功，MR_FAILED 失败，MR_IGNORE 不支持
  */
int32 mr_setting_image_encode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

#endif
