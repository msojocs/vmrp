#ifndef __MRP_PAL_IMAGE_H__
#define __MRP_PAL_IMAGE_H__


/**
 * \brief draw direct请求参数
 */
typedef struct 
{
	char *src;
	int32 src_len;
	int32 src_type;
	int32 ox;
	int32 oy;
	int32 w;
	int32 h;
}T_DRAW_DIRECT_REQ;


/**
 * \brief image 模块初始化
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_image_initialize(void);


/**
 * \brief image 模块释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_image_terminate(void);


/**
 * \brief 查找空闲曹
 *
 * \param id	[in]图片ID
 * \return 空闲位置的下标，-1表示没有空闲位置
 */
int32 findSlotByID(int32 id);


/**
 * \brief 释放gif相关内存
 * 
 * \param index	[in] 下标
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED.
 */
 int32 dsmFreeGifMem(int32 index);


/**
 * \brief 工具文件名或者内存获取图片类型
 *
 * \param filename	[in] 如果是文件名方式传文件名，否则NULL
 * \param pbuf		[in] 如果是内存方式，传内存起始地址，否则NULL
 * \param len		[in] 内存长度
 * \return 返回图片类型，参考GDI_IMAGE_TYPE_**
 */
U16 dsmGetImgType(char *filename,char *pbuf,int32 len);


/**
 * \brief 直接在屏幕上绘制图片
 * 
 * \param req		[in] 请求数据结构
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_image_draw_img(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief GIF 解码请求
 *
 * \param pDecInfo		[in] 请求编码信息
 * \param output		[out] 
 * \param output_len	[out]
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_image_gif_decode(T_DSM_IMG_DECODE *pDecInfo, uint8**output, int32 *output_len);


/**
 * \brief 获取PNG图片尺寸信息
 *
 * \param png_src	[in] png 图片数据
 * \param size		[in] buffer大小字节数
 * \param width		[out]图片的宽
 * \param height	[out]图片的高
 * \return GDI_RESULT
 */
int32 dsm_image_png_get_dimension(U8 *png_src, U32 size, S32 *width, S32 *height);


/**
 * \brief 直接在屏幕上绘制图片
 *
 * \param req		[in] 图片请求信息
 * \param handle	[in] gdi句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 dsm_draw_anim_direct(T_DRAW_DIRECT_REQ* req,gdi_handle* handle);


/**
 * \brief 绘制图片
 *
 * \param x
 * \param y
 * \param ptr 
 * \return 
 */
int32 mr_image_draw_mtk_img(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 绘制动画
 *
 * \param x
 * \param y
 * \param ptr
 * \return
 */
int32 dsm_anima_draw(int32 x,int32 y,uint8*ptr);


/**
 * \brief 设置图层
 *
 * \param src_layer_handle
 * \return
 */
gdi_handle gdi_dsm_abm_set_source_layer(gdi_handle src_layer_handle);


int32 mr_image_get_image_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_image_gif_release(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_image_decode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_image_draw_mtk_animation(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_image_draw_animation(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_image_stop_animation(int32 param);


#endif
