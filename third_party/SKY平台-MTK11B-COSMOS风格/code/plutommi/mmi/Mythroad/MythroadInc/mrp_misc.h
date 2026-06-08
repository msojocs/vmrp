#ifndef __MRP_PAL_MISC_H__
#define __MRP_PAL_MISC_H__


#include "syscomp_config.h"


typedef enum{
	MR_PLATEX_CODE_MEM_SUPPORT = 0,
	MR_PLATEX_CODE_MEM_MALLOC,
	MR_PALTEX_CODE_MEM_FREE,
	MR_PLATEX_CODE_MEM_IRAM_ALLOC,
	MR_PLATEX_CODE_MEM_IRAM_FREE,
}mr_platEx_code_mem_enum;


typedef struct{
	int32 handle;
	int32 w;
	int32 h;
	char *buffer;
}mr_layer_info_t;


typedef struct{
	int32 desLayer;
	int32 layer1;
	int32 layer2;
	int32 layer3;
	int32 layer4;
}mr_layer_flatten_info_t;


typedef struct{
	int32 layer1;
	int32 layer2;
	int32 layer3;
	int32 layer4;
}mr_layer_blt_info_t;


typedef struct{
	int32 x;
	int32 y;
	int32 w;
	int32 h;
	int32 size;
	char *buffer;
}mr_layer_create_info_t;


typedef struct{
	int32 x;
	int32 y;
}mr_layer_position_info_t;


typedef struct{   
   int16 x;  /*x coordinate*/
   int16 y;  /*y coordinate*/ 
}mr_touchpanel_coord_t; 


/* __MMI_DSM_NEW_JSKY__ begin support multitouch */
typedef struct{   
   int16 event; /* MR_MOUSE_DOWN, MR_MOUSE_UP, MR_MOUSE_MOVE */
   int16 x;  /*x coordinate*/
   int16 y;  /*y coordinate*/ 
   int16 z;
}mr_touchpanel_coord_ex_t; 


#define MAX_TOUCH_POINTS 5
typedef struct{   
   int16 touch_count;
   int16 padding;
   mr_touchpanel_coord_ex_t points[5];
}mr_touchpanel_coord_list_t; 
/* __MMI_DSM_NEW_JSKY__ end support multitouch */


extern mr_layer_info_t dsmLayerInfo;
///////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * \brief 背景灯常亮
 */
int32 mr_backlight_turnon(void);


/**
 * \brief 背景灯自动关闭
 */
int32 mr_backlight_turnoff(void);


/**
 * \brief 获取背光灯状态
 */
int32 mr_backlight_get_status(int32 param);


/**
 * \brief 设置旋转
 */
int32 mr_lcd_set_rotation(int32 param);


/**
 * \brief 异步刷屏，通过DMA，不占CPU时间
 *
 * \param adrs	[in] framebuffer地址
 * \param w		[in] buffer宽，指像素
 * \param h		[in] buffer高，指像素
 */
int32 dsm_LCD_Display(void *adrs, kal_uint16 w, kal_uint16 h);


/**
 * \brief 绘制菜单背景
 */
int32 mr_mainmenu_draw_background(int32 param);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * \brief med 内存分配
 */
void *mr_mem_get_ex(int32 size);


/**
 * \brief med 内存释放
 */
void mr_mem_free_ex(void **p);


/**
 * \brief internal 内存申请
 */
int32 mr_mem_malloc_int_mem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief internal 内存释放
 */
int32 mr_mem_free_int_mem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 屏幕内存申请
 */
int32 mr_mem_malloc_scrmem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 屏幕内存释放
 */
int32 mr_mem_free_scrmem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
int32 mr_layer_zoom_get_width(void);

int32 mr_layer_zoom_get_height(void);

int32 mr_layer_zoom_is_enabled(void);

int32 mr_layer_zoom_convert_touch_position(int16 *x, int16 *y);

int32 mr_layer_zoom_disable(void);

int32 mr_layer_zoom_resize(S32 x, S32 y, S32 width, S32 height);

int32 mr_layer_zoom_pause(void);

int32 mr_layer_zoom_resume(void);

int32 mr_layer_zoom_enable(uint16 width, uint16 height);
#endif


/**
 * \brief lock frame buffer
 */
int32 mr_layer_lock_frame_buffer(int32 param);


/**
 * \brief unlock frame buffer
 */
int32 mr_layer_unlock_frame_buffer(int32 param);


/**
 * \brief set opacity
 */
int32 mr_layer_set_opacity(BOOL enable, int32 param);


/**
 * \brief enable source key
 */
int32 mr_layer_enable_source_key(BOOL enable);


/**
 * \brief free
 */
int32 mr_layer_free(int32 param);


/**
 * \brief set active layer
 */
int32 mr_layer_set_active_layer(int32 param);


/**
 * \brief set abm layer
 */
int32 mr_layer_set_abm_layer(int32 param);


/**
 * \brief set blt infomation
 */
int32 mr_layer_set_blt_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief create
 */
int32 mr_layer_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief get base layer
 */
int32 mr_layer_get_base_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief get active layer
 */
int32 mr_layer_get_active_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief set position
 */
int32 mr_layer_set_layer_position(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief flatten
 */
int32 mr_layer_flatten(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief blt
 */
int32 mr_layer_blt_previous(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief get act layer
 */
int32 mr_layer_get_act_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 获取BASE LAYER的内存缓存区
 */
int32 mr_layer_get_base_layer_buffer(uint8**output, int32 *output_len);


/**
 * \brief 获取当前触摸屏坐标
 */
int32 mr_touch_panel_get_position(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __MR_CFG_FEATURE_DAM__
#define MR_DAM_TITLE_LENGHT_MAX	32  //bytes
#define MR_DAM_PATH_LENGTH_MAX	128  //bytes
#define MR_DAM_DATA_LENGTH_MAX	128  //bytes


typedef enum{
	MR_PLATEX_CODE_DAM_SUPPORT = 0,
	MR_PLATEX_CODE_DAM_INSTALL,
	MR_PALTEX_CODE_DAM_REMOVE,
	MR_PLATEX_CODE_DAM_UPDATE_PROGRESS,
}mr_platEx_code_dam_enum;


//应用提供者需要支持的操作
typedef enum{
	MR_DAM_CHECK_APP, //检查应用是否存在，返回MR_SUCCESS表示应用存在，MR_FAILED表示不存在
	MR_DAM_START_APP, //启动应用，MR_SUCCESS成功，MR_FAILED失败
	MR_DAM_UNINSTALL_APP //卸载应用, 隐藏就等于uninstall的时候不删除应用，通过参数来处理，成功返回MR_SUCCESS, 失败返回MR_FAILED
}mr_dam_command_enum;


//应用提供者类型
typedef enum{
	MR_DAM_PROVIDER_NATIVE = 0, //本地应用
	MR_DAM_PROVIDER_SKY, //斯凯的应用
	MR_DAM_PROVIDER_MEX, //杨汛的应用
	MR_DAM_PROVIDER_VRE, //沃勤的应用
}mr_dam_provider_enum;


//sky bitmap fileheader
typedef struct _MR_BMP_T_
{
	char code[4];//斯凯图片标识，“SKBM”
	uint16 w;//图片宽
	uint16 h;//图片高
	uint8* data;//BMP图片数据,RGB565小端;
}mr_bmp_t;


typedef struct {
	char title_ch[MR_DAM_TITLE_LENGHT_MAX]; //中文名称
	char title_en[MR_DAM_TITLE_LENGHT_MAX]; //英文名称
	char  image_path[MR_DAM_PATH_LENGTH_MAX]; //图片路径,绝对路径；支持格式：曲奇BMP格式
	int32 provider; //提供者标识
	int32 appid; //应用标识
	uint32 total_size; //文件大小
	uint32 cur_size; //当前已经下载大小
	uint8 start_data[MR_DAM_DATA_LENGTH_MAX]; //启动信息，各个平台可以根据这个字段来启动
	uint8 filepath[MR_DAM_PATH_LENGTH_MAX]; //可以是任何能定位及删除应用的信息，在mrp应用里是mrp文件的绝对路径
}mr_dam_appinfo_t;


//应用下载进度信息
typedef struct {
	int32 provider; 
	int32 appid;
	int32 total_size; //总大小(字节数)
	int32 progress; //当前已下载（字节数）
}mr_dam_progress_t;


//平台提供的应用处理函数
typedef int32 (*mr_dam_cmd_hdlr_f)(mr_dam_appinfo_t* appinfo, mr_dam_command_enum cmd, void* param); 


//内部注册的数据结构
typedef struct {
	mr_dam_provider_enum provider;
	mr_dam_cmd_hdlr_f cmd;
}mr_dam_cmd_hdlr_t;


/*删除参数*/
typedef struct {
	int32 provider; 
	int32 appid;
}mr_dam_remove_info_t;


int32 mr_dam_dispatch_cmd(mr_dam_appinfo_t* appinfo, mr_dam_command_enum cmd, void* param);
int32 mr_dam_sky_cmd_hdlr(mr_dam_appinfo_t* appinfo, mr_dam_command_enum cmd, void* param);
char* mr_dam_start(void);

int32 mr_skybmp_open(const WCHAR* path);
void mr_skybmp_get_size(int32 f, int16* w, int16* h);
uint32 mr_skybmp_get_data(int32 f, void* data, int32 len);
void mr_skybmp_close(int32 f);
int32 mr_skybmp2bmp(const WCHAR* src, const WCHAR* dest);

#endif

#endif
