#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "gdi_image.h"
//#include "gdi_internal.h"

#include "mrp_include.h"
#include "Cache_sw.h"
#include "mmu.h"


extern const gdi_image_codecs_struct gdi_image_codecs[];


static int mr_image_get_type(void* src, int32 src_type);
static const gdi_image_codecs_struct* mr_image_get_codec(void* src, int32 src_type);


int32 mr_image_initialize(void)
{
	return MR_SUCCESS;
}

int32 mr_image_terminate(void)
{
	return MR_SUCCESS;
}

int32 mr_image_draw_img(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	const gdi_image_codecs_struct* codec;
	T_DRAW_DIRECT_REQ* draw_req;
	int32 ret;
	U16 file_name[DSM_MAX_FILE_LEN] = {0};
	U32 flag;
	U8* data_ptr;
	S32 img_w, img_h;
	S32 disp_w, disp_h;

	draw_req = (T_DRAW_DIRECT_REQ*)input;

	MMI_ASSERT(draw_req && draw_req->src);
	
	codec = mr_image_get_codec(draw_req->src, draw_req->src_type);
	if (!(codec && codec->draw && codec->get_dimension))
		return MR_FAILED;

	if (draw_req->src_type == SRC_NAME)
	{
		mr_fs_get_filename((char*)file_name, draw_req->src);
		flag = GDI_IMAGE_CODEC_FLAG_IS_FILE;
		data_ptr = (U8*)file_name;
	}
	else if (draw_req->src_type == SRC_STREAM)
	{
		flag = GDI_IMAGE_CODEC_FLAG_IS_MEM;
		data_ptr = (U8*)draw_req->src;
	}
	else
	{
		return MR_FAILED;
	}

	if ((codec->get_dimension)(flag, data_ptr, draw_req->src_len, &img_w, &img_h) != GDI_SUCCEED)
		return MR_FAILED;

	if ((img_w <= draw_req->w) && (img_h <= draw_req->h))
	{
		// 设置为0底层可能有优化
		disp_w = 0;
		disp_h = 0;
	}
	else
	{
		disp_w = draw_req->w;
		disp_h = draw_req->h;
	}
	
	if ((codec->draw)(flag, 0, draw_req->ox, draw_req->oy, disp_w, disp_h, data_ptr, draw_req->src_len) == GDI_SUCCEED)
	{
		ret = MR_SUCCESS;
	}
	else
	{
		ret = MR_FAILED;
	}
				
	return ret;
}

int32 mr_image_gif_decode(T_DSM_IMG_DECODE *pDecInfo,uint8**output,int32 *output_len)
{
	return mr_image_decode((uint8*)pDecInfo, sizeof(T_DSM_IMG_DECODE), output, output_len, NULL);
}

int32 mr_image_gif_release(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_SUCCESS;
}

int32 mr_image_draw_mtk_img(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_IGNORE;
}

gdi_handle gdi_dsm_abm_set_source_layer(gdi_handle src_layer_handle)
{
	return gdi_image_abm_set_source_layer(src_layer_handle);
}

int32 dsm_draw_anim_direct(T_DRAW_DIRECT_REQ* req,gdi_handle* handle)
{
	S32 width,height;
	int32 ret=0;
	
	if(req == NULL||req->src == NULL)
		return MR_FAILED;

	if(req->src_type == SRC_NAME)
	{
		U16 fullpathname[DSM_MAX_FILE_LEN] = {0};
		memset(fullpathname,0,sizeof(fullpathname));
		mr_fs_get_filename((char *)fullpathname,req->src);
		
		if(gdi_image_get_dimension_file((S8*)fullpathname,&width,&height)!= GDI_SUCCEED)
			return MR_FAILED;
		if((width <= req->w)&&(height<=req->h))
		{
			ret = gdi_anim_draw_file(req->ox,req->oy,(S8*)fullpathname,handle);
		}
		else
		{
			ret = gdi_anim_draw_file_resized(req->ox,req->oy, req->w, req->h,(S8*)fullpathname,handle);
		}
		
		if(ret == GDI_SUCCEED)
			return MR_SUCCESS;
		else
			return MR_FAILED;
		
	}
	else if(req->src_type == SRC_STREAM)
	{
		if((width <= req->w)&&(height<=req->h))
		{
			ret = gdi_anim_draw_mem(req->ox,req->oy,(U8 *)req->src,0, req->src_len,handle);
		}
		else
		{
			ret =gdi_anim_draw_mem_resized(req->ox,req->oy,req->w,req->h,(U8 *)req->src,0, req->src_len,handle);
		}
		
		if(ret == GDI_SUCCEED)
			return MR_SUCCESS;
		else
			return MR_FAILED;
	}
	else
	{
		return MR_FAILED;
	}
}

int32 mr_image_draw_animation(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	gdi_handle handle;
	
	if(input_len < sizeof(T_DRAW_DIRECT_REQ)||output == NULL||output_len ==NULL)
		return MR_FAILED;
	if(dsm_draw_anim_direct((T_DRAW_DIRECT_REQ *)input,&handle)== MR_SUCCESS)
	{
		g_mr_common_rsp.p1 = handle;
		*output = (uint8 *)&g_mr_common_rsp;
		*output_len = sizeof(gdi_handle);
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}

}

int32 mr_image_get_image_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{	
	static T_DSM_IMG_INFO img_info;	// 必须使用静态变量
	const gdi_image_codecs_struct* codec;
	T_DSM_GET_IMG_INFO* img;
	int32 ret;
	U16 file_name[DSM_MAX_FILE_LEN] = {0};
	S32 w, h;
	U32 flag;
	U8* data_ptr;
	

	img = (T_DSM_GET_IMG_INFO*)input;

	MMI_ASSERT(img && img->src);
	
	codec = mr_image_get_codec(img->src, img->src_type);
	if (!(codec && codec->get_dimension))
		return MR_FAILED;

	if (img->src_type == SRC_NAME)
	{
		mr_fs_get_filename((char*)file_name, img->src);
		flag = GDI_IMAGE_CODEC_FLAG_IS_FILE;
		data_ptr = (U8*)file_name;
	}
	else if (img->src_type == SRC_STREAM)
	{
		flag = GDI_IMAGE_CODEC_FLAG_IS_MEM;
		data_ptr = (U8*)img->src;
	}
	else
	{
		return MR_FAILED;
	}
	
	if ((codec->get_dimension)(flag, data_ptr, img->len, &w, &h) == GDI_SUCCEED)
	{
		ret = MR_SUCCESS;
	}
	else
	{
		ret = MR_FAILED;
	}
		
	img_info.width = w;
	img_info.height = h;
	img_info.img_type = mr_image_get_type(img->src, img->src_type);

	*output = (uint8*)&img_info;
	*output_len = sizeof(T_DSM_IMG_INFO);
				
	return ret;
}

#ifndef GDI_ABS
#define GDI_ABS(x)   ((x)<0?-(x):(x))
#endif
static void mr_image_fit_box_zoom_in(
        S32 bbox_width,
        S32 bbox_height,
        S32 src_width,
        S32 src_height,
        S32 *offset_x,
        S32 *offset_y,
        S32 *dest_width,
        S32 *dest_height)
{
	/*缩小*/
	if (src_width == 0 || src_height == 0 || bbox_width == 0 || bbox_height == 0)
        {
            *dest_width = 0;
            *dest_height = 0;
        }
        else if (src_width * bbox_height > src_height * bbox_width)
        {
            *dest_width = bbox_width;
            *dest_height = src_height * bbox_width / src_width;
        }
        else
        {
            *dest_width = src_width * bbox_height / src_height;
            *dest_height = bbox_height;
        }

    {
        int diff;

        diff = GDI_ABS(*dest_height - bbox_height);
        if (diff <= 2 && *dest_width == bbox_width)
        {
            *dest_height = bbox_height;
        }

        diff = GDI_ABS(*dest_width - bbox_width);
        if (diff <= 2 && *dest_height == bbox_height)
        {
            *dest_width = bbox_width;
        }
    }

    *offset_x = (bbox_width - *dest_width) / 2;
    *offset_y = (bbox_height - *dest_height) / 2;

    if(*dest_width==0) *dest_width = 1;
    if(*dest_height==0) *dest_height = 1;
}

static void mr_image_fit_box_zoom_out(
        S32 bbox_width,
        S32 bbox_height,
        S32 src_width,
        S32 src_height,
        S32 *offset_x,
        S32 *offset_y,
        S32 *dest_width,
        S32 *dest_height)
{
	/*放大*/
	if (src_width == 0 || src_height == 0 || bbox_width == 0 || bbox_height == 0)
        {
            *dest_width = 0;
            *dest_height = 0;
        }
        else if (src_width * bbox_height > src_height * bbox_width)
        {
            *dest_width = bbox_width;
            *dest_height = src_height * bbox_width / src_width;
        }
        else
        {
           *dest_height = bbox_height;
           *dest_width = src_width * bbox_height / src_height;
        }

    {
        int diff;

        diff = GDI_ABS(*dest_height - bbox_height);
        if (diff <= 2 && *dest_width == bbox_width)
        {
            *dest_height = bbox_height;
        }

        diff = GDI_ABS(*dest_width - bbox_width);
        if (diff <= 2 && *dest_height == bbox_height)
        {
            *dest_width = bbox_width;
        }
    }

    *offset_x = (bbox_width - *dest_width) / 2;
    *offset_y = (bbox_height - *dest_height) / 2;

    if(*dest_width==0) *dest_width = 1;
    if(*dest_height==0) *dest_height = 1;
}

static GDI_RESULT mr_gdi_image_create_temp_layer(
            gdi_handle* layer,
            U8* buffer,
            gdi_color_format cf,
            S32 x,
            S32 y,
            S32 width,
            S32 height)
{
	U32 bytes_per_pixel;
	U32 buf_size;
	kal_int32 bChange = 0;
	kal_int32 buffer_32len = 0;
	kal_int32 remain_bits = 0;
	kal_uint32* invalidPtr = (kal_uint32*)buffer;

	MMI_ASSERT(layer != NULL);
	MMI_ASSERT(buffer != NULL);

	*layer = NULL;

	bytes_per_pixel = gdi_bits_per_pixel(cf) >> 3;
	buf_size = width * height * bytes_per_pixel;

#if defined (__DYNAMIC_SWITCH_CACHEABILITY__)
        if (INT_QueryIsCachedRAM(buffer, buf_size) == KAL_TRUE)
	{
	    	remain_bits =  ((kal_int32) invalidPtr & (0x1f));
	    	invalidPtr = (kal_uint32*) ((kal_int32) invalidPtr & (~0x1f));
	    	buffer_32len= buf_size >> 5;
		    	
	    	if ((kal_uint32*)buffer  != invalidPtr)
	    	{
	        	buffer_32len += 2;
	    	}
	    	else
	    	{
	        	buffer_32len++;
	    	}              
	    	dynamic_switch_cacheable_region((void *)&invalidPtr, (buffer_32len<<5), PAGE_NO_CACHE);            
	    	invalidPtr = (kal_uint32 *)((kal_int32)invalidPtr | remain_bits);
	    	bChange = 1;
	}
	    	
#endif
        
    gdi_layer_create_cf_using_outside_memory(
        cf, 
        x, 
        y,
        width, 
        height, 
        layer,
        (U8*)invalidPtr, 
        buf_size);
        
	if (*layer == GDI_NULL_HANDLE)
	{
#if defined (__DYNAMIC_SWITCH_CACHEABILITY__)
		if(bChange)
		{
		    	remain_bits =  ((kal_int32) invalidPtr & (0x1f));
		    	invalidPtr = (kal_uint32*) ((kal_int32) invalidPtr & (~0x1f));
		    	
		    	dynamic_switch_cacheable_region((void *)&invalidPtr, (buffer_32len<<5), PAGE_CACHEABLE);   
		    	bChange = 0;
		}
#endif	
	    return GDI_FAILED;
	}
	
#if defined (__DYNAMIC_SWITCH_CACHEABILITY__)
	if(bChange)
	{
	    	remain_bits =  ((kal_int32) invalidPtr & (0x1f));
	    	invalidPtr = (kal_uint32*) ((kal_int32) invalidPtr & (~0x1f));
	    	
	    	dynamic_switch_cacheable_region((void *)&invalidPtr, (buffer_32len<<5), PAGE_CACHEABLE);   
	    	bChange = 0;
	}
#endif	

	return GDI_SUCCEED;
}

static void mr_gdi_image_free_temp_layer(gdi_handle layer)
{
	gdi_layer_free(layer);
}
            
int32 mr_image_decode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_DSM_IMG_DECODE* img_decode;
	gdi_handle temp_layer;
	int32 ret;
	U8* buf;
	const gdi_image_codecs_struct* codec;
	S32 img_w, img_h;	// 图片原始大小
	S32 disp_x, disp_y, disp_w, disp_h;	// 解码后图片的显示区域
	int32 type;
	U16 file_name[DSM_MAX_FILE_LEN] = {0};
	U32 flag;
	U8* data_ptr;

	img_decode = (T_DSM_IMG_DECODE*)input;
	
	MMI_ASSERT(img_decode && img_decode->src);

	codec = mr_image_get_codec(img_decode->src, img_decode->src_type);
	if (!(codec && codec->get_dimension && codec->draw))
		return MR_FAILED;

	if (img_decode->src_type == SRC_NAME)
	{
		mr_fs_get_filename((char*)file_name, img_decode->src);
		flag = GDI_IMAGE_CODEC_FLAG_IS_FILE;
		data_ptr = (U8*)file_name;
	}
	else if (img_decode->src_type == SRC_STREAM)
	{
		flag = GDI_IMAGE_CODEC_FLAG_IS_MEM;
		data_ptr = (U8*)img_decode->src;
	}
	else
	{
		return MR_FAILED;
	}

	// 获取图片原始大小
	if ((codec->get_dimension)(flag, data_ptr, img_decode->len, &img_w, &img_h) != GDI_SUCCEED)
		return MR_FAILED;	
	
	// 计算各种解码类型下的显示区域
	if (input_len < sizeof(T_DSM_IMG_DECODE))
	{
		type = MR_DEOCDE_TYPE_NORMAL;
	}
	else
	{
		type = img_decode->type;
	}	

	
	if (type == MR_DEOCDE_TYPE_DBL)
	{
		if( ((img_decode->width) > img_w) || ((img_decode->height) > img_h))
		{
			// 放大
			mr_image_fit_box_zoom_out(img_decode->width, img_decode->height, img_w, img_h, &disp_x, &disp_y, &disp_w, &disp_h);
		}
		else
		{
			// 缩小
			mr_image_fit_box_zoom_in(img_decode->width, img_decode->height, img_w, img_h, &disp_x, &disp_y, &disp_w, &disp_h);
		}	
	}
	else if (type == MR_DEOCDE_TYPE_WG)
	{
		disp_x = 0;
		disp_y = 0;
		disp_w = img_decode->width;
		disp_h = img_decode->height;
	}
	else
	{
		if( ((img_decode->width) >= img_w) && ((img_decode->height) >= img_h))
		{
			disp_x = (img_decode->width - img_w) >> 1;
			disp_y = (img_decode->height - img_h) >> 1;
			disp_w = img_w;
			disp_h = img_h;
		}
		else
		{
			mr_image_fit_box_zoom_in(img_decode->width, img_decode->height, img_w, img_h, &disp_x, &disp_y, &disp_w, &disp_h);		
		}
	}	
	
	// 图片DECODE是绘制到传入的BUFFER
	mr_gdi_image_create_temp_layer(&temp_layer, img_decode->dest, GDI_COLOR_FORMAT_16, 0, 0, img_decode->width, img_decode->height);

	if (temp_layer == GDI_NULL_HANDLE) return MR_FAILED;

	gdi_layer_push_and_set_active(temp_layer);
	gdi_layer_clear(GDI_COLOR_WHITE);

	if ((codec->draw)(flag, 0, disp_x, disp_y, disp_w, disp_h, data_ptr, img_decode->len) == GDI_SUCCEED)
	{
		ret = MR_SUCCESS;
	}
	else
	{
		ret = MR_FAILED;
	}

	gdi_layer_pop_and_restore_active();
	mr_gdi_image_free_temp_layer(temp_layer);

	return ret;
}

int32 mr_image_draw_mtk_animation(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_IGNORE;
}

int32 mr_image_stop_animation(int32 param)
{
	if(param == 0)
		return MR_FAILED;
		
	if(gdi_image_stop_animation(param) == GDI_SUCCEED)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

static int mr_image_get_type(void* src, int32 src_type)
{
	U16 file_name[DSM_MAX_FILE_LEN + 1] = {0};
	U16 img_type;
	int ret;	// IMG_TYPE

	if (src_type == SRC_NAME)
	{
		mr_fs_get_filename((char*)file_name, (const char*)src);
		img_type = gdi_image_get_type_from_file((S8*)file_name);

		if (img_type == GDI_IMAGE_TYPE_BMP_FILE)
			ret = IMG_BMP;
		else if (img_type == GDI_IMAGE_TYPE_JPG_FILE)
			ret = IMG_JPG;
		else if (img_type == GDI_IMAGE_TYPE_PNG_FILE)
			ret = IMG_PNG;
		else if (img_type == GDI_IMAGE_TYPE_GIF_FILE)
			ret = IMG_GIF;
		else
			ret = -1;		// 应该需要定义一个无效的格式类型，但是没有只能用-1表示
	}
	else if (src_type == SRC_STREAM)
	{
		img_type = gdi_image_get_type_from_mem((PS8)src);

		if (img_type == GDI_IMAGE_TYPE_BMP)
			ret = IMG_BMP;
		else if (img_type == GDI_IMAGE_TYPE_JPG)
			ret = IMG_JPG;
		else if (img_type == GDI_IMAGE_TYPE_PNG)
			ret = IMG_PNG;
		else if (img_type == GDI_IMAGE_TYPE_GIF)
			ret = IMG_GIF;
		else
			ret = -1;		// 应该需要定义一个无效的格式类型，但是没有只能用-1表示
	}
	else
	{
		ret = -1;
	}

	return ret;
}

static const gdi_image_codecs_struct* mr_image_get_codec(void* src, int32 src_type)
{
	const gdi_image_codecs_struct* codec;
	U16 file_name[DSM_MAX_FILE_LEN] = {0};
	U16 img_type;

	MMI_ASSERT(src);

	if (src_type == SRC_NAME)
	{
		mr_fs_get_filename((char*)file_name, (const char*)src);

		img_type = gdi_image_get_type_from_file((S8*)file_name);
	}
	else if (src_type == SRC_STREAM)
	{
		img_type = gdi_image_get_type_from_mem((PS8)src);	
	}
	else
	{
		return NULL;
	}

	if ((img_type > GDI_IMAGE_TYPE_INVALID) && (img_type < GDI_IMAGE_TYPE_SUM))
	{		
		return &gdi_image_codecs[img_type];
	}
	else
	{
		return NULL;
	}
}

#endif

