
#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "Gdi_include.h"
#include "Gdi_datatype.h"

#include "Gdi_image_bmp.h"
//#include "gdi_internal.h"
#include "med_utility.h"
#include "mrp_include.h"
#include "gpiosrvgprot.h"
#include "screenrotationgprot.h"
#include "gdi_layer.h"
#ifndef WIN32
#include "touch_panel.h"
#include "lcd_if_hw.h"
#endif
#include "Cache_sw.h"
#if defined(__DYNAMIC_SWITCH_CACHEABILITY__) && defined(__MTK_TARGET__)
#include "mmu.h"
#endif
   
#define GDI_MAINLCD_BUFFER_SIZE	              ((GDI_LCD_WIDTH*GDI_LCD_HEIGHT*GDI_MAINLCD_BIT_PER_PIXEL+7)>>3)


extern U16 gdi_base_layer_buffer_pool[];
mr_layer_info_t dsmLayerInfo={0};

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_SCREEN_ROTATE__)
extern U8 current_screen_rotation;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32 mr_backlight_turnon(void)
{
	srv_backlight_turn_on(0);
	return MR_SUCCESS;
}


int32 mr_backlight_turnoff(void)
{
	srv_backlight_turn_off();
	return MR_SUCCESS;
}


int32 mr_backlight_get_status(int32 param)
{
	if (srv_backlight_is_on(SRV_BACKLIGHT_TYPE_MAINLCD) && srv_backlight_get_timer_state() == SRV_BACKLIGHT_TIMER_STATE_NO_TIMER)
		return 1001;
	else
		return 1000;
}

int32 dsm_LCD_Display(void *adrs, kal_uint16 w, kal_uint16 h)
{
	mr_layer_create_info_t info;
	mr_common_rsp_t* rsp;
	int32 len;
	static gdi_handle layer = GDI_NULL_HANDLE;
	
	info.x = 0;
	info.y = 0;
	info.w = w;
	info.h = h;
	info.size = w * h * 2;
	info.buffer = adrs;

	if (layer != GDI_NULL_HANDLE)
	{
		mr_layer_free(layer);
		layer = GDI_NULL_HANDLE;
	}
	
	if (mr_layer_create((uint8*)&info, sizeof(info), (uint8**)&rsp, &len, NULL) == MR_SUCCESS)
	{
		layer = rsp->p1;
		//gdi_enable_non_block_blt();
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)
	gdi_layer_blt_ext(layer, GDI_NULL_HANDLE, GDI_NULL_HANDLE, GDI_NULL_HANDLE,
			GDI_NULL_HANDLE, GDI_NULL_HANDLE,			
			0, 0, w - 1, h - 1);
#else
		gdi_layer_blt_ext(layer, GDI_NULL_HANDLE, GDI_NULL_HANDLE, GDI_NULL_HANDLE,
#ifdef GDI_6_LAYERS
			GDI_NULL_HANDLE, GDI_NULL_HANDLE,
#endif			
			0, 0, w - 1, h - 1);
#endif			
		//gdi_disable_non_block_blt();
		
		return MR_SUCCESS;
	}
	
	return MR_FAILED;
}

int32 mr_lcd_set_rotation(int32 param)
{
	U8 rotate_value;
	
	if(MR_LCD_ROTATE_NORMAL == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_0;
	else if(MR_LCD_ROTATE_90 ==param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_90;
	else if (MR_LCD_ROTATE_180 == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_180;
	else if (MR_LCD_ROTATE_270 == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_270;
#ifdef __MMI_DSM_TODO__    
	else if (MR_LCD_MIRROR == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_0;
	else if (MR_LCD_MIRROR_ROTATE_90 == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_90;
	else if (MR_LCD_MIRROR_ROTATE_180 == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_180;
	else if (MR_LCD_MIRROR_ROTATE_270 == param)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_270;
#endif
	else
		return MR_FAILED;

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_SCREEN_ROTATE__)
	mr_trace("mr_lcd_set_rotation rotate_value = %d", rotate_value);
	current_screen_rotation = rotate_value;
	mmi_frm_screen_rotate((mmi_frm_screen_rotate_enum)rotate_value);
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	if (mr_layer_zoom_is_enabled())
	{
		gdi_layer_resize(mr_layer_zoom_get_width(), mr_layer_zoom_get_height());
	}
#endif
	return MR_SUCCESS;
#else
	 return MR_FAILED;
#endif
}


int32 mr_mainmenu_draw_background(int32 param)
{
	// gui_draw_filled_area(0,0,LCD_WIDTH-1,LCD_HEIGHT-1,current_MMI_theme->main_menu_bkg_filler);		// 11A编译问题
	return MR_IGNORE;
}


int32 mr_entry_todo_list(int32 param)
{
#ifdef __ENTRY_TODO_LIST__		
	extern void EntryTDLTaskList(void);
	EntryTDLTaskList();
	return MR_SUCCESS;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32 mr_mem_malloc_scrmem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(output == NULL||output_len == NULL||input_len == 0)
		return MR_FAILED;
	
	*output = (uint8*)mr_pal_mem_scrmem_alloc(input_len);
	*output_len =input_len;
	return MR_SUCCESS;
}


int32 mr_mem_free_scrmem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(input == NULL)
		return MR_FAILED;
	
	mr_pal_mem_scrmem_free((void *)input);
	return MR_SUCCESS;
}


void *mr_mem_get_ex(int32 size)
{
	return med_alloc_ext_mem(size);
}


void mr_mem_free_ex(void **p)
{
	med_free_ext_mem(p);
}


int32 mr_mem_malloc_int_mem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#if (defined(__MTK_TARGET__) && defined(DRV_FEATURE__MM_INTMEM_IF))
	// 内部分配函数在定义了这两个宏时会返回一个无效的地址
	return MR_FAILED;
#endif

	if(output == NULL||output_len == NULL||input_len == 0)
		return MR_FAILED;

	*output = (uint8*) med_alloc_int_mem(input_len);
	*output_len =sizeof(int32);
	return MR_SUCCESS;
}


int32 mr_mem_free_int_mem(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(input == NULL)
		return MR_FAILED;

	med_free_int_mem((void **)&input);
	return MR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
#include "idp_image_resize.h"
#include "img_common_enum.h"
#define MRP_USING_GDI_WORK_BUFFER  
#if !defined(GDI_USING_LCD_WORK_BUFFER) || !defined(MRP_USING_GDI_WORK_BUFFER)
static gdi_handle g_current_app_layer = GDI_LAYER_EMPTY_HANDLE;
static U8 *g_current_app_layer_buf = NULL;
#endif
static MMI_BOOL g_is_enable_zoom = MMI_FALSE;
static uint16 g_app_width;
static uint16 g_app_height;

int32 mr_layer_zoom_get_width(void)
{
#if defined(__MMI_SCREEN_ROTATE__)
	switch (current_screen_rotation)
	{
		case MMI_FRM_SCREEN_ROTATE_0:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_0:
		case MMI_FRM_SCREEN_ROTATE_180:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_180:
			return g_app_width;

		default:
			return g_app_height;
	}
#else
	return g_app_width;
#endif
}

int32 mr_layer_zoom_get_height(void)
{
#if defined(__MMI_SCREEN_ROTATE__)
	switch (current_screen_rotation)
	{
		case MMI_FRM_SCREEN_ROTATE_0:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_0:
		case MMI_FRM_SCREEN_ROTATE_180:
		case MMI_FRM_SCREEN_ROTATE_MIRROR_180:
			return g_app_height;

		default:
			return g_app_width;
	}
#else
	return g_app_height;
#endif
}

int32 mr_layer_zoom_is_enabled(void)
{
	return g_is_enable_zoom;
}

int32 mr_layer_zoom_convert_touch_position(int16 *x, int16 *y)
{
	if (mr_layer_zoom_is_enabled())
	{
		uint16 ori_width = (uint16)UI_device_width;
		uint16 ori_height = (uint16)UI_device_height;
		uint16 cur_width = mr_layer_zoom_get_width();
		uint16 cur_height = mr_layer_zoom_get_height();

		*x = *x*cur_width/ori_width;
		*y = *y*cur_height/ori_height;
	}
	return MR_SUCCESS;
}

int32 mr_layer_zoom_disable(void)
{
	gdi_handle base_layer;
	
	if (!g_is_enable_zoom)
	{
		return MR_SUCCESS;
	}
#if !defined(GDI_USING_LCD_WORK_BUFFER) || !defined(MRP_USING_GDI_WORK_BUFFER)
	if (g_current_app_layer != GDI_LAYER_EMPTY_HANDLE)
	{
		gdi_layer_free(g_current_app_layer);
		g_current_app_layer = GDI_LAYER_EMPTY_HANDLE;
	}
	if (g_current_app_layer_buf != NULL)
	{
		applib_asm_free_anonymous(g_current_app_layer_buf);
		g_current_app_layer_buf = NULL;
	}
#endif
	g_app_width = 0;
	g_app_height = 0;
	g_is_enable_zoom = MMI_FALSE;
	gdi_layer_get_base_handle(&base_layer);
	gdi_layer_push_and_set_active(base_layer);
	gdi_layer_resize(UI_device_width, UI_device_height);
	gdi_layer_set_blt_layer(base_layer, 0, 0, 0);
	gdi_layer_pop_and_restore_active();
	return MR_SUCCESS;
}


/* because 11A cosmos not support HW resizer, so we direct use idp to resize it */
int32 mr_layer_zoom_bitblt_resized(
		gdi_handle src_layer_handle, 
		S32 sx1,
		S32 sy1,
		S32 sx2,
		S32 sy2,
		S32 dx1,
		S32 dy1,
		S32 dx2,
		S32 dy2)
{
#define MR_RETURN(X) {return X;}
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
	gdi_layer_struct *src_layer = (gdi_layer_struct*)src_layer_handle;
	U8 *src;
	U16 src_width;
	U16 src_height;
	U16 src_pitch;
	U32 src_cf;
	U8 *dest;
	U16 dest_width;
	U16 dest_height;
	U16 dest_buf_width;
	U32 dest_cf;
	U16 output_clip_x1;
	U16 output_clip_y1;
	U16 output_clip_x2;
	U16 output_clip_y2;
	
	kal_bool result;
	kal_uint32 key;
	kal_uint16 output_width;
	
	int32 ret_value = MR_SUCCESS;

	kal_uint32 *intmem_ptr = NULL, *extmem_ptr = NULL;
	kal_uint32 intmem_size, extmem_size;

	BOOL source_key_enable = FALSE;
	gdi_color source_key_value = 0;

	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/

	/* return GDI_FAILED if area is out of layer region */
	{
		S32 tx1, tx2, ty1, ty2;
		tx1 = sx1;
		tx2 = sx2;
		ty1 = sy1;
		ty2 = sy2;
		GDI_CLIP_TWO_RECT(tx1, ty1, tx2, ty2, 0, 0, src_layer->width - 1, src_layer->height - 1, MR_RETURN(MR_FAILED));
		if ((tx1 != sx1) || (tx2 != sx2) || (ty1 != sy1) || (ty2 != sy2))
		{
			MR_RETURN(MR_FAILED);
		}
		
		tx1 = dx1;
		tx2 = dx2;
		ty1 = dy1;
		ty2 = dy2;
		GDI_CLIP_TWO_RECT(tx1, ty1, tx2, ty2, 0, 0, gdi_act_layer->width - 1, gdi_act_layer->height - 1, MR_RETURN(MR_FAILED));
		if ((tx1 != dx1) || (tx2 != dx2) || (ty1 != dy1) || (ty2 != dy2))
		{
			MR_RETURN(MR_FAILED);
		}
	}

	/* Color Format */
	switch(src_layer->cf)
	{
		case GDI_COLOR_FORMAT_16:
			src_cf = IMG_COLOR_FMT_RGB565;
			break;
		case GDI_COLOR_FORMAT_24:
			src_cf = IMG_COLOR_FMT_RGB888;
			break;
		case GDI_COLOR_FORMAT_32:
			src_cf = IMG_COLOR_FMT_ARGB8888;
			break;
		default:
			MR_RETURN(MR_FAILED);
			break;
	}
	switch(gdi_act_layer->cf)
	{
		case GDI_COLOR_FORMAT_16:
			dest_cf = IMG_COLOR_FMT_RGB565;
			break;
		case GDI_COLOR_FORMAT_24:
			dest_cf = IMG_COLOR_FMT_RGB888;
			break;
		case GDI_COLOR_FORMAT_32:
			dest_cf = IMG_COLOR_FMT_ARGB8888;
			break;
		default:
			MR_RETURN(MR_FAILED);
			break;
	}
	
	src = src_layer->buf_ptr + (((src_layer->width * sy1) + sx1) * (src_layer->bits_per_pixel >> 3));
	src_width = sx2 - sx1 + 1;
	src_height = sy2 - sy1 + 1;
	src_pitch = src_layer->width;
	
	dest = gdi_act_layer->buf_ptr + (((gdi_act_layer->width * dy1) + dx1) * (gdi_act_layer->bits_per_pixel >> 3));
	dest_width = dx2 - dx1 + 1;
	dest_height = dy2 - dy1 + 1;
	dest_buf_width = gdi_act_layer->width;
	
	/* dest clip */
	{
		output_clip_x1 = 0; /* use (dx1,dy1) as ori point */
		output_clip_y1 = 0;
		output_clip_x2 = output_clip_x1 + dest_width - 1;
		output_clip_y2 = output_clip_y1 + dest_height - 1;

		/* return GDI_SUCCEED if dest clip is out of layer clip */
		GDI_CLIP_TWO_RECT(
			output_clip_x1, 
			output_clip_y1, 
			output_clip_x2, 
			output_clip_y2, 
			gdi_act_layer->clipx1 - dx1,
			gdi_act_layer->clipy1 - dy1, 
			gdi_act_layer->clipx2 - dx1, 
			gdi_act_layer->clipy2 - dy1, 
			MR_RETURN(MR_FAILED));
	}

	/* check 4-byte alignment */
	if (src_cf == IMG_COLOR_FMT_RGB565)
	{
		if ((kal_int32)src % 4 == 2)	
		{
			/* if 2-byte alignment, move src ptr to the next pixel */
			src += 2;
			src_width--;
		}
		else if ((kal_int32)src % 4 != 0)
		{
			MR_RETURN(MR_FAILED);
		}
	}
	else if (src_cf == IMG_COLOR_FMT_RGB888)
	{
		switch((kal_int32)src % 4)
		{
			case 1:
				src += 3;
				src_width -= 1;
				break;
				
			case 2:
				src += 6;
				src_width -= 2;
				break;
				
			case 3:
				src += 9;
				src_width -= 3;
				break;
				
			default:
				break;
		}
	}
	/* If src_width is modified to less than 0 because of checking 4-byte alignment, return error */
	if (src_width <= 0) 
	{
		MR_RETURN(MR_FAILED);
	}
	
	/* open, to get key */
	result = idp_image_resize_open(&key);
	if (KAL_TRUE != result)
	{
		MR_RETURN(MR_FAILED);
	}	 

	do
	{
		intmem_ptr = 0;
		intmem_size = 0;
			
		extmem_ptr = 0;
		extmem_size = 0;

		/* calculate dest pitch */
		if (src_width == src_pitch)
		{
			output_width = dest_width;
		}
		else if (src_width == 1)
		{
			output_width = src_layer->width * dest_width;
		}
		else
		{
			/*
			 * HW resizer doesn't support src pitch and support dest pitch.
			 * But GDI resizer has to support src pitch.
			 * 
			 * To keep the right ratio of the region that user really want to output,
			 * GDI do the following two things to simulate src pitch fuction
			 * 1. modify the output_width 
			 *	  (src_width : src_pitch = dest_buf_width : output_width)
			 * 2. enable dest pitch mode but the dest pitch can only be dest_buf_width.
			 */
			output_width = (src_pitch - 1) * dest_width / (src_width - 1) + 1;
			if (output_clip_x2 >= output_width)
			{
				output_clip_x2 = output_width - 1;
			}
		}

			
			
		/* config resizer, 0 represent configing over */
		/* config memory */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_INTMEM_START_ADDRESS, 
					intmem_ptr,
					IDP_IMAGE_RESIZE_INTMEM_SIZE, 
					intmem_size,
					IDP_IMAGE_RESIZE_EXTMEM_START_ADDRESS, 
					extmem_ptr,
					IDP_IMAGE_RESIZE_EXTMEM_SIZE, 
					extmem_size,
					0);
		/* config color format */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_SRC_COLOR_FMT, 
					src_cf, 
					IDP_IMAGE_RESIZE_DEST_COLOR_FMT, 
					dest_cf, 
					0);
		/* config image address */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_SRC_IMAGE_ADDRESS, 
					src, 
					IDP_IMAGE_RESIZE_DEST_IMAGE_ADDRESS, 
					dest, 
					0);
		/* config src dimension */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_SRC_WIDTH, 
					src_pitch, 
					IDP_IMAGE_RESIZE_SRC_HEIGHT, 
					src_height, 
					0);
		/* config target dimension */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_TARGET_WIDTH, 
					output_width, 
					IDP_IMAGE_RESIZE_TARGET_HEIGHT, 
					dest_height, 
					0);
		/* config clipping */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_ENABLE_CLIP, 
					KAL_TRUE, 
					IDP_IMAGE_RESIZE_CLIP_LEFT, 
					output_clip_x1, 
					IDP_IMAGE_RESIZE_CLIP_RIGHT, 
					output_clip_x2, 
					IDP_IMAGE_RESIZE_CLIP_TOP, 
					output_clip_y1, 
					IDP_IMAGE_RESIZE_CLIP_BOTTOM, 
					output_clip_y2, 
					0);
		/* config destnation pitch */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_ENABLE_PITCH,		/* dest pitch mode */
					KAL_TRUE, 
					IDP_IMAGE_RESIZE_BG_IMAGE_WIDTH,	/* dest pitch */
					dest_buf_width, 
					0);
		/* config rotate */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_ROT_ANGLE, 
					IMG_ROT_ANGLE_0, 
					0);
		/* config u value and v value */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_RESIZER_U_VALUE, 
					8,
					IDP_IMAGE_RESIZE_RESIZER_V_VALUE, 
					15,
					0);
					
		/* config dithering, disable dithering */
		result = idp_image_resize_config(
					key,
					IDP_IMAGE_RESIZE_IPP_ENABLE_SPATIAL_DITHERING, 
					KAL_FALSE,
					0);
					
		if (KAL_TRUE != result)
		{
			ret_value = MR_FAILED;
			break;
		}
		
		/* start resizer */
		result = idp_image_resize_start_all(key);
		if (KAL_TRUE != result)
		{
			ret_value = MR_FAILED;
			break;
		}

		/* wait until resizer finished */
		while (1)
		{
			kal_bool busy;

			result = idp_image_resize_is_busy(key, &busy);
			if (KAL_TRUE != result)
			{
				ret_value = MR_FAILED;
				break;
			}

			if (KAL_FALSE == busy)
			{
				break;
			}
		}

		if (ret_value == MR_FAILED)
        {
            break;
        }

		gdi_layer_get_source_key(&source_key_enable, &source_key_value);
        if(source_key_enable)
        {
            gdi_act_replace_src_key(
                    gdi_act_layer->buf_ptr,
                    gdi_act_layer->width,
                    dx1,
                    dy1,
                    gdi_act_layer->clipx1,
                    gdi_act_layer->clipy1,
                    gdi_act_layer->clipx2,
                    gdi_act_layer->clipy2,
                    source_key_value,
                    dx2 - dx1 + 1,
                    dy2 - dy1 + 1);
        }
	}while(0);
	
	/* close resizer */
	result = idp_image_resize_close(key);
	if (KAL_TRUE != result)
	{
		ret_value = MR_FAILED;
	}

	MR_RETURN(ret_value);
}


int32 mr_layer_zoom_resize(S32 x, S32 y, S32 width, S32 height)
{
	if (mr_layer_zoom_is_enabled())
	{
		gdi_handle base_layer;
		gdi_handle working_layer;
		gdi_handle handle0 = 0;
		gdi_handle handle1 = 0;
		gdi_handle handle2 = 0;
		gdi_handle handle3 = 0;
		S32 dx1, dy1, dwidth, dheight;

		mr_trace("mr_layer_zoom_resize x = %d, y = %d, width = %d, height = %d", x, y, width, height);
		
		if (width <= 0 || x < 0 || x >= mr_layer_zoom_get_width() - 1 ||
			height <= 0 || y < 0 || y >= mr_layer_zoom_get_height() - 1)
		{
			mr_trace("mr_layer_zoom_resize error,check the parameter");
			return MR_FAILED;
		}

		if (x + width > mr_layer_zoom_get_width()) {width = mr_layer_zoom_get_width() - x;}
		if (y + height > mr_layer_zoom_get_height()) {height = mr_layer_zoom_get_height() - y;}
		
		gdi_layer_get_base_handle(&base_layer);
		
		gdi_layer_push_and_set_active(base_layer);
		gdi_layer_resize(mr_layer_zoom_get_width(), mr_layer_zoom_get_height());
		gdi_layer_pop_and_restore_active();

		gdi_layer_get_blt_layer(&handle0, &handle1, &handle2, &handle3);
		if (handle1 != 0 || handle2 != 0 || handle3 != 0)
		{
			mr_trace("need flatten before resize");
			gdi_layer_push_and_set_active(base_layer);
			gdi_layer_flatten(handle0, handle1, handle2, handle3);
			gdi_layer_pop_and_restore_active();
		}

	#if !defined(GDI_USING_LCD_WORK_BUFFER) || !defined(MRP_USING_GDI_WORK_BUFFER)
		working_layer = g_current_app_layer;
	#else
		working_layer = gdi_work_buffer_handle;
	#endif
		gdi_layer_push_and_set_active(working_layer);
		gdi_layer_reset_clip();
		gdi_layer_resize(UI_device_width, UI_device_height);
		/* must do this thing, other hw resizer can't work */
		gdi_layer_set_source_key(FALSE, GDI_COLOR_TRANSPARENT);

		dx1 = x*UI_device_width/mr_layer_zoom_get_width();
		dy1 = y*UI_device_height/mr_layer_zoom_get_height();
		dwidth = width*UI_device_width/mr_layer_zoom_get_width();
		dheight = height*UI_device_height/mr_layer_zoom_get_height();

		mr_layer_zoom_bitblt_resized(
			base_layer,
			x, y,
			x + width - 1,
			y + height - 1,
			dx1, dy1,
			dx1 + dwidth - 1, 
			dy1 + dheight - 1);

		gdi_layer_pop_and_restore_active();

		gdi_layer_blt(working_layer, 0, 0, 0, dx1, dy1, dx1 + dwidth - 1, dy1 + dheight - 1);

		//gdi_layer_set_blt_layer(handle0, handle1, handle2, handle3);

		if (mmi_is_redrawing_bk_screens())
		{
			mr_layer_zoom_pause();
		}

		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}

int32 mr_layer_zoom_pause(void)
{
	if (mr_layer_zoom_is_enabled())
	{
		gdi_handle base_layer;
		
		mr_trace("mr_layer_zoom_pause");

		gdi_layer_get_base_handle(&base_layer);
		gdi_layer_push_and_set_active(base_layer);
		gdi_layer_resize(UI_device_width, UI_device_height);
	#if !defined(GDI_USING_LCD_WORK_BUFFER) || !defined(MRP_USING_GDI_WORK_BUFFER)
		gdi_layer_flatten(g_current_app_layer, 0, 0, 0);
	#else
		gdi_layer_flatten(gdi_work_buffer_handle, 0, 0, 0);
	#endif
		gdi_layer_pop_and_restore_active();
	}

	return MR_SUCCESS;
}


int32 mr_layer_zoom_resume(void)
{
	if (mr_layer_zoom_is_enabled())
	{
		gdi_handle base_layer;
		
		mr_trace("mr_layer_zoom_resume");
		
		gdi_layer_get_base_handle(&base_layer);
		gdi_layer_push_and_set_active(base_layer);
		gdi_layer_resize(mr_layer_zoom_get_width(), mr_layer_zoom_get_height());
		gdi_layer_pop_and_restore_active();
	}

	return MR_SUCCESS;
}


int32 mr_layer_zoom_enable(uint16 width, uint16 height)
{
	mr_trace("mr_layer_zoom_enable, width = %d, height = %d", width, height);

	mr_layer_zoom_disable();

	if (width == LCD_WIDTH && height == LCD_HEIGHT)
	{
		return MR_IGNORE;
	}

	if (width > LCD_WIDTH || height > LCD_HEIGHT)
	{
		return MR_FAILED;
	}
	
	if (width == 0 || height == 0)
	{
		/* do nothing, already diable */
		mr_trace("mr_layer_zoom_enable, zoom disabled");
	}
	else
	{
	#if !defined(GDI_USING_LCD_WORK_BUFFER) || !defined(MRP_USING_GDI_WORK_BUFFER)
		/* when screen rotate, the active layer will be resized to LCD width&height, will assert  at gdi_layer_resize */
		/* of GDI_DEBUG_ASSERT(gdi_sizeof_pixels(gdi_act_layer->cf, width, height) <= gdi_act_layer->layer_size); */
		/* so we should use the whole screen buffer */
		S32 buf_size = (LCD_WIDTH * LCD_HEIGHT * gdi_layer_get_bit_per_pixel()) >> 3;
		GDI_RESULT result;

		mr_trace("mr_layer_zoom_enable, buf_size = %d", buf_size);
		
		g_current_app_layer_buf = applib_asm_alloc_anonymous_nc(buf_size);
		if (g_current_app_layer_buf == NULL)
		{
			return MR_FAILED;
		}

		result = gdi_layer_create_using_outside_memory(
					0, 0,
					LCD_WIDTH, LCD_HEIGHT,
					&g_current_app_layer,
					g_current_app_layer_buf, 
					buf_size);
		if (result != GDI_SUCCEED)
		{
			applib_asm_free_anonymous(g_current_app_layer_buf);
			g_current_app_layer_buf = NULL;
			return MR_FAILED;
		}

		gdi_layer_push_and_set_active(g_current_app_layer);
		gdi_layer_clear_background(GDI_COLOR_TRANSPARENT);
		gdi_layer_set_source_key(TRUE, GDI_COLOR_TRANSPARENT);
		gdi_layer_pop_and_restore_active();
	#endif

		mr_trace("mr_layer_zoom_enable, zoom enabled");
		/* rotate first, then enable */
		g_app_width = width;
		g_app_height = height;
		g_is_enable_zoom = MMI_TRUE;
		mr_layer_zoom_resume();
	}

	return MR_SUCCESS;
}
#endif


int32 mr_layer_get_base_layer_buffer(uint8**output,int32 *output_len)
{
	if(output == NULL||output_len == NULL)
		return MR_FAILED;
	gdi_layer_push_and_set_active(GDI_LAYER_MAIN_BASE_LAYER_HANDLE);
	*output = (uint8*)gdi_act_layer->buf_ptr;
	*output_len = gdi_act_layer->layer_size;
	gdi_layer_pop_and_restore_active();
	return MR_SUCCESS;
	
}


int32 mr_layer_lock_frame_buffer(int32 param)
{
	gdi_layer_lock_frame_buffer();
	return MR_SUCCESS;
}


int32 mr_layer_unlock_frame_buffer(int32 param)
{
	gdi_layer_unlock_frame_buffer();
	return MR_SUCCESS;
}


int32 mr_layer_set_opacity(BOOL enable, int32 param)
{
	if(param >255)
		param = 255;
	
	gdi_layer_set_opacity(enable, param);
	return MR_SUCCESS;
}


int32 mr_layer_enable_source_key(BOOL enable)
{
	gdi_layer_set_source_key(enable, GDI_COLOR_TRANSPARENT);
	return MR_SUCCESS;
}


int32 mr_layer_free(int32 param)
{
	if(param == 0)
		return MR_FAILED;
	
	kal_prompt_trace(MOD_MMI,"param=%d",param);
	gdi_layer_free((gdi_handle)param);
	return MR_SUCCESS;
}


int32 mr_layer_set_active_layer(int32 param)
{
	gdi_handle oldLayer;
	gdi_layer_get_active(&oldLayer);
	if(param == 0)
		return MR_FAILED;
	
	gdi_layer_set_active((gdi_handle)param);
	return (oldLayer+MR_PLAT_VALUE_BASE);
}


int32 mr_layer_set_abm_layer(int32 param)
{
	gdi_handle oldAbm;
	if(param == 0)
		return MR_FAILED;
	
	oldAbm =gdi_dsm_abm_set_source_layer((gdi_handle)param);
	return (oldAbm+MR_PLAT_VALUE_BASE);
}


int32 mr_layer_set_blt_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_layer_blt_info_t *pReq = (mr_layer_blt_info_t *)input;
	if(input== NULL||input_len < sizeof(mr_layer_blt_info_t))
		return MR_FAILED;
	
	gdi_layer_set_blt_layer(pReq->layer1, pReq->layer2, pReq->layer3, pReq->layer4);
	return MR_SUCCESS;
}

#if defined(__ARM9_MMU__)  || defined (__DYNAMIC_SWITCH_CACHEABILITY__)
int32 mr_layer_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_layer_create_info_t *pReq = (mr_layer_create_info_t *)input;
	kal_uint32* invalidPtr = (kal_uint32*)pReq->buffer;
	kal_int32 bChange = 0;
    	kal_int32 buffer_32len = 0;
    	kal_int32 remain_bits = 0;
	if(pReq == NULL || output == NULL || output_len == NULL || input_len < sizeof(mr_layer_create_info_t) 
		||pReq->buffer== NULL || ((pReq->size) < (pReq->w*pReq->h*2)))
	{
		return MR_FAILED;
	}
	
#if defined (__DYNAMIC_SWITCH_CACHEABILITY__)

        if (INT_QueryIsCachedRAM(pReq->buffer, pReq->size) == KAL_TRUE)
	{
	    	remain_bits =  ((kal_int32) invalidPtr & (0x1f));
	    	invalidPtr = (kal_uint32*) ((kal_int32) invalidPtr & (~0x1f));
	    	buffer_32len= pReq->size >> 5;
		    	
	    	if ((kal_uint32*)pReq->buffer  != invalidPtr)
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

	
	if(gdi_layer_create_using_outside_memory(pReq->x, pReq->y, pReq->w, pReq->h, 
		(gdi_handle*)&g_mr_common_rsp.p1, (U8*)(invalidPtr), pReq->size) != GDI_SUCCEED)
	{
		#if defined (__DYNAMIC_SWITCH_CACHEABILITY__)
			if(bChange)
			{
			    	remain_bits =  ((kal_int32) invalidPtr & (0x1f));
			    	invalidPtr = (kal_uint32*) ((kal_int32) invalidPtr & (~0x1f));
			    	
			    	dynamic_switch_cacheable_region((void *)&invalidPtr, (buffer_32len<<5), PAGE_CACHEABLE);   
			    	//不需要了
			    	//invalidPtr = (kal_uint32 *)((kal_int32)invalidPtr | remain_bits);
			    	bChange = 0;
			}
		#endif	
		
		return MR_FAILED;
	}

	#if defined (__DYNAMIC_SWITCH_CACHEABILITY__)
		if(bChange)
		{
		    	remain_bits =  ((kal_int32) invalidPtr & (0x1f));
		    	invalidPtr = (kal_uint32*) ((kal_int32) invalidPtr & (~0x1f));
		    	
		    	dynamic_switch_cacheable_region((void *)&invalidPtr, (buffer_32len<<5), PAGE_CACHEABLE);   
		    	//不需要了
		    	//invalidPtr = (kal_uint32 *)((kal_int32)invalidPtr | remain_bits);
		    	bChange = 0;
		}
	#endif	
	
	*output = (uint8*)&g_mr_common_rsp;
	*output_len = sizeof(gdi_handle);
	return MR_SUCCESS;
}
#else
int32 mr_layer_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_layer_create_info_t *pReq = (mr_layer_create_info_t *)input;
	if(pReq == NULL || output == NULL || output_len == NULL || input_len < sizeof(mr_layer_create_info_t) 
		||pReq->buffer== NULL || ((pReq->size) < (pReq->w*pReq->h*2)))
	{
		return MR_FAILED;
	}
	  
	if(gdi_layer_create_using_outside_memory(pReq->x, pReq->y, pReq->w, pReq->h, 
		(gdi_handle*)&g_mr_common_rsp.p1, (U8*)(pReq->buffer), pReq->size) != GDI_SUCCEED)
	{
		return MR_FAILED;
	}

	
	*output = (uint8*)&g_mr_common_rsp;
	*output_len = sizeof(gdi_handle);
	return MR_SUCCESS;
}
#endif


int32 mr_layer_get_base_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(output == NULL||output_len == NULL)
		return MR_FAILED;

	g_mr_common_rsp.p1 = GDI_LAYER_MAIN_BASE_LAYER_HANDLE;
	if(g_mr_common_rsp.p1 == 0)
		return MR_FAILED;
	
	*output = (uint8*)&g_mr_common_rsp;
	*output_len = sizeof(gdi_handle);
	return MR_SUCCESS;
}


int32 mr_layer_get_active_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(output == NULL||output_len == NULL)
		return MR_FAILED;

	dsmLayerInfo.handle = (int32)gdi_act_layer;
	dsmLayerInfo.w = gdi_act_layer->width;
	dsmLayerInfo.h = gdi_act_layer->height;
	dsmLayerInfo.buffer = (char*)gdi_act_layer->buf_ptr;
	*output = (uint8*)&dsmLayerInfo;
	*output_len = sizeof(dsmLayerInfo);
	return MR_SUCCESS;
}


int32 mr_layer_set_layer_position(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_layer_position_info_t *pReq = (mr_layer_position_info_t *)input;
	if(pReq== NULL||input_len < sizeof(mr_layer_position_info_t))
		return MR_FAILED;
	
	gdi_layer_set_position(pReq->x, pReq->y);
	return MR_SUCCESS;
}


int32 mr_layer_flatten(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_layer_flatten_info_t *pReq = (mr_layer_flatten_info_t *)input;
	if(pReq== NULL||input_len < sizeof(mr_layer_flatten_info_t)||pReq->desLayer == 0)
		return MR_FAILED;
	
	gdi_layer_push_and_set_active(pReq->desLayer);

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)	

	gdi_layer_flatten_ext(pReq->layer1, pReq->layer2, pReq->layer3, pReq->layer4, 0, 0);

#else

	gdi_layer_flatten_ext(pReq->layer1, pReq->layer2, pReq->layer3, pReq->layer4
#ifdef GDI_6_LAYERS	
		, 0, 0
#endif
	);
	
#endif

	gdi_layer_pop_and_restore_active();
	return MR_SUCCESS;
}

int32 mr_layer_blt_previous(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	gdi_enable_non_block_blt();
	GDI_LAYER_SET_FLAG(gdi_act_layer,GDI_LAYER_FLAG_FROZEN);
	gdi_layer_blt_previous(0,0, GDI_LAYER.width-1,GDI_LAYER.height-1);
	GDI_LAYER_CLEAR_FLAG(gdi_act_layer,GDI_LAYER_FLAG_FROZEN);
	return MR_SUCCESS;
}


int32 mr_layer_get_act_layer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	extern gdi_layer_struct *gdi_act_layer;
	*output = (uint8*)gdi_act_layer;
	*output_len = sizeof(gdi_layer_struct);
	return MR_SUCCESS;
}


/* __MMI_DSM_NEW_JSKY__ begin support multitouch */
#if defined(__MMI_TOUCH_SCREEN__) && !defined(WIN32) && (defined(__TOUCH_PANEL_CAPACITY__) || defined(__TP_AUX_909_SUPPORT__))
extern TouchPanelDataStruct TP;
static mr_touchpanel_coord_list_t g_last_touch_info;
static uint16 mr_touch_panel_down_func(uint16 x_diff, uint16 y_diff, uint16 count)
{
	if (x_diff > TP.longtap_pen_offset || y_diff > TP.longtap_pen_offset)
	{
		return MR_MOUSE_MOVE;
	}
	else
	{
		return MR_MOUSE_DOWN | 0x0100;
	}
}

static uint16 mr_touch_panel_up_func(uint16 x_diff, uint16 y_diff, uint16 count)
{
	mr_trace("mr_touch_panel_up_func x_diff = %d, y_diff = %d", x_diff, y_diff);
	return MR_MOUSE_UP;
}

static uint16 mr_touch_panel_move_func(uint16 x_diff, uint16 y_diff, uint16 count)
{
	if (x_diff > TP.longtap_pen_offset || y_diff > TP.longtap_pen_offset)
	{
		return MR_MOUSE_MOVE;
	}
	else
	{
		return MR_MOUSE_DOWN | 0x0100;
	}
}

typedef uint16 (* STATUS_FUNC)(uint16 x_diff, uint16 y_diff, uint16 count);

static const STATUS_FUNC g_status_func[] =  
{
	mr_touch_panel_down_func,    /*Down */
	mr_touch_panel_up_func,      /*Up*/
	mr_touch_panel_move_func,    /*Move*/
};


static float mr_touch_panel_inv_sqrt(float x)
{
	/* 0x5f375a86 这是一个非常神奇的数字*/
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating VALUE 
	i = 0x5f375a86 - (i>>1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
	return x;
}

static int16 mr_touch_panel_get_diff(int16 x, int16 y)
{
	int i,max,min,xx,yy,tmp;
	
	if (x == 0)
	{
		return y;
	}
	else if (y == 0)
	{
		return x;
	}
	else
	{
	#if 0
		if (x > y)
		{
			if (y <= x*3/4)
			{
				max = x*5/4;
				min = x;
			}
			else
			{
				max = x*3/2;
				min = x*5/4;
			}
		}
		else
		{
			if (x <= y*3/4)
			{
				max = y*5/4;
				min = y;
			}
			else
			{
				max = y*3/2;
				min = y*5/4;
			}
		}
		
		xx = x*x; yy = y*y; tmp = xx + yy;
		for (i = max; i >= min; i--)
		{
			if (tmp >= (i*i ))
				break;
		}

		return i;
	#else
		xx = x*x; yy = y*y; tmp = xx + yy;
		return (int16)(1/mr_touch_panel_inv_sqrt(tmp));
	#endif
	}
}


static int16 mr_touch_panel_find_nearest(int16 diff[MAX_TOUCH_POINTS][MAX_TOUCH_POINTS], int16 *row, int16 *col)
{
#define VERY_LARGE 0x7FFF
	uint16 i, j;
	int16 min_diff = VERY_LARGE;
	int16 k = -1, l = -1;

	for (i = 0; i < *row; i++)
	{
		for (j = 0; j < *col; j++)
		{
			if (diff[i][j] >= 0 && min_diff > diff[i][j])
			{
				min_diff = diff[i][j];
				k = i;
				l = j;
			}
		}
	}

	if (k != -1 && l != -1)
	{
		for (i = 0; i < *row; i++)
		{
			if (diff[i][l] >= 0)
			{
				diff[i][l] = -1;
			}
		}
		for (i = 0; i < *col; i++)
		{
			diff[k][i] = -2;
		}
		*row = k;
		*col = l;
		return 1;
	}
	else
	{
		for (i = 0; i < *row; i++)
		{
			if (diff[i][0] > -2)
			{
				for (j = 0; j < *col; j++)
				{
					diff[i][j] = -2;
				}
				*row = i;
				*col = -1;
				return 1;
			}
			
		}
		return 0;
	}
}


static uint16 mr_touch_panel_state_machine(kal_uint16 x_diff, kal_uint16 y_diff, kal_uint16 pre_event, kal_uint16 count)
{
	int index;
	
	if (pre_event == MR_MOUSE_DOWN)
	{
		index = 0;
	}
	else if (pre_event == MR_MOUSE_UP)
	{
		index = 1;
	}
	else
	{
		index = 2;
	}
	
	return g_status_func[index](x_diff, y_diff, count);
}

void mr_touch_panel_convert_event(mr_touchpanel_coord_list_t *current_event, mr_touchpanel_coord_list_t *last_event)
{
	int16 i, j;
	uint32 event;
	int16 x_diff;
	int16 y_diff;
	uint32 touch_count = 0;
	uint32 ori_touch_count = 0;
	mr_touchpanel_coord_list_t tmp_event;
	int16 diff[MAX_TOUCH_POINTS][MAX_TOUCH_POINTS];
	uint8 dir;

	if (current_event->touch_count == 0) //all touch up
	{
		for (i = 0; i < last_event->touch_count; i++)
		{
			if (last_event->points[i].event != MR_MOUSE_UP)
			{
				current_event->points[i].x = last_event->points[i].x;
				current_event->points[i].y = last_event->points[i].y;
				current_event->points[i].z = last_event->points[i].z;
				current_event->points[i].event = MR_MOUSE_UP;
				touch_count++;
			}
		}

		current_event->touch_count = touch_count;

		return ;
	}

	for (i = 0; i < last_event->touch_count; i++)
	{
		if (last_event->points[i].event != MR_MOUSE_UP)
		{
			ori_touch_count++;
		}
	}

	if (ori_touch_count == 0)
	{
		/* no need convert */
		return ;
	}

	if (ori_touch_count != last_event->touch_count)
	{
		ori_touch_count = 0;
		for (i = 0; i < last_event->touch_count; i++)
		{
			if (last_event->points[i].event != MR_MOUSE_UP)
			{
				last_event->points[ori_touch_count].x = last_event->points[i].x;
				last_event->points[ori_touch_count].y = last_event->points[i].y;
				last_event->points[ori_touch_count].z = last_event->points[i].z;
				last_event->points[ori_touch_count].event = last_event->points[i].event;
				ori_touch_count++;
			}
		}
		last_event->touch_count = ori_touch_count;
	}

	if (current_event->touch_count < last_event->touch_count)
	{
		dir = 1;
	}
	else
	{
		dir = 0;
	}

	/* calc the diff of all the down point */
	for (i = 0; i < current_event->touch_count; i++)
	{
		for (j = 0; j < last_event->touch_count; j++)
		{
			if (current_event->points[i].x > last_event->points[j].x)   
				x_diff = current_event->points[i].x - last_event->points[j].x;
			else
				x_diff = last_event->points[j].x - current_event->points[i].x;
			if (current_event->points[i].y > last_event->points[j].y)
				y_diff = current_event->points[i].y - last_event->points[j].y;
			else
				y_diff = last_event->points[j].y - current_event->points[i].y;
			if (dir)
			{
				diff[j][i] = mr_touch_panel_get_diff(x_diff, y_diff);
			}
			else
			{
				diff[i][j] = mr_touch_panel_get_diff(x_diff, y_diff);
			}
		}
	}

	if (dir)
	{
		j = current_event->touch_count;
		i = last_event->touch_count;
	}
	else
	{
		i = current_event->touch_count;
		j = last_event->touch_count;
	}

	touch_count = 0;
	while (mr_touch_panel_find_nearest(diff, &i, &j))
	{
		if (i != -1 && j != -1)
		{
			if (dir)
			{
				if (current_event->points[j].x > last_event->points[i].x)   
					x_diff = current_event->points[j].x - last_event->points[i].x;
				else
					x_diff = last_event->points[i].x - current_event->points[j].x;
				if (current_event->points[j].y > last_event->points[i].y)
					y_diff = current_event->points[j].y - last_event->points[i].y;
				else
					y_diff = last_event->points[i].y - current_event->points[j].y;

				event = mr_touch_panel_state_machine(x_diff, y_diff, last_event->points[i].event, i);

				tmp_event.points[touch_count].x = current_event->points[j].x;
				tmp_event.points[touch_count].y = current_event->points[j].y;
				tmp_event.points[touch_count].z = current_event->points[j].z;
				tmp_event.points[touch_count].event = event;
			}
			else
			{
				if (current_event->points[i].x > last_event->points[j].x)   
					x_diff = current_event->points[i].x - last_event->points[j].x;
				else
					x_diff = last_event->points[j].x - current_event->points[i].x;
				if (current_event->points[i].y > last_event->points[j].y)
					y_diff = current_event->points[i].y - last_event->points[j].y;
				else
					y_diff = last_event->points[j].y - current_event->points[i].y;

				event = mr_touch_panel_state_machine(x_diff, y_diff, last_event->points[j].event, j);

				tmp_event.points[touch_count].x = current_event->points[i].x;
				tmp_event.points[touch_count].y = current_event->points[i].y;
				tmp_event.points[touch_count].z = current_event->points[i].z;
				tmp_event.points[touch_count].event = event;
			}
			
			touch_count++;
		}
		else if (i != -1)
		{
			if (dir)
			{
				tmp_event.points[touch_count].x = last_event->points[i].x;
				tmp_event.points[touch_count].y = last_event->points[i].y;
				tmp_event.points[touch_count].z = last_event->points[i].z;
				tmp_event.points[touch_count].event = MR_MOUSE_UP;
			}
			else
			{
				tmp_event.points[touch_count].x = current_event->points[i].x;
				tmp_event.points[touch_count].y = current_event->points[i].y;
				tmp_event.points[touch_count].z = current_event->points[i].z;
				tmp_event.points[touch_count].event = MR_MOUSE_DOWN;
			}
			touch_count++;
		}
		
		if (dir)
		{
			j = current_event->touch_count;
			i = last_event->touch_count;
		}
		else
		{
			i = current_event->touch_count;
			j = last_event->touch_count;
		}
	}
	
	tmp_event.touch_count = touch_count;

	memcpy(current_event, &tmp_event, sizeof(tmp_event));
}
#endif


int32 mr_touch_panel_get_position(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#if defined(__MMI_TOUCH_SCREEN__) && !defined(WIN32)
#ifdef __TOUCH_PANEL_CAPACITY__
	extern TouchPanelMultipleEventStruct CTP_g_event;
#endif
#ifdef __TP_AUX_909_SUPPORT__
	#include "I2C_PP_26Bytes.h"
	extern ST_TOUCH_INFO fte_data;
#endif
	S32 x;
	S32 y;

	mr_touchpanel_coord_t* pCoord = (mr_touchpanel_coord_t*)input;
	
	if (!input || input_len != sizeof(mr_touchpanel_coord_t))
		return MR_FAILED;

#ifdef __TOUCH_PANEL_CAPACITY__
	pCoord->x = (int16)CTP_g_event.points[0].x;
	pCoord->y = (int16)CTP_g_event.points[0].y;
#else
	pCoord->x = TP.pre.x;
	pCoord->y = TP.pre.y;
#endif

#ifdef __MMI_SCREEN_ROTATE__
	if (mmi_frm_get_screen_rotate() != MMI_FRM_SCREEN_ROTATE_0)
	{
		x = (S32)pCoord->x;
		y = (S32)pCoord->y;
		gdi_rotate_map_absolute_hw_to_lcd(&x, &y);
		pCoord->x = (int16)x;
		pCoord->y = (int16)y;
	}
#endif /* __MMI_SCREEN_ROTATE__ */

#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_convert_touch_position((int16 *)&pCoord->x, (int16 *)&pCoord->y);
#endif

	if (output_len != NULL)
	{
		*output_len = (int32)TP.state;
	}

#if defined(__TOUCH_PANEL_CAPACITY__) || defined(__TP_AUX_909_SUPPORT__)
	if (output != NULL)
	{
		mr_touchpanel_coord_list_t *list = (mr_touchpanel_coord_list_t *)output;
		int i;
		int16 touch_count = 0;
	#define CONVER_EVENT(drv_event) (drv_event == PEN_DOWN ? MR_MOUSE_DOWN : MR_MOUSE_UP)
	#ifdef __TOUCH_PANEL_CAPACITY__
		for (i = 0; i < CTP_g_event.model; i++)
		{
			list->points[i].event = CONVER_EVENT(CTP_g_event.points[i].event);
			list->points[i].x = CTP_g_event.points[i].x;
			list->points[i].y = CTP_g_event.points[i].y;
			list->points[i].z = CTP_g_event.points[i].z;
		#ifdef __MMI_SCREEN_ROTATE__
			if (mmi_frm_get_screen_rotate() != MMI_FRM_SCREEN_ROTATE_0)
			{
				x = (S32)list->points[i].x;
				y = (S32)list->points[i].y;
				gdi_rotate_map_absolute_hw_to_lcd(&x, &y);
				list->points[i].x = (int16)x;
				list->points[i].y = (int16)y;
			}
		#endif /* __MMI_SCREEN_ROTATE__ */
		#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
			mr_layer_zoom_convert_touch_position((int16 *)&list->points[i].x, (int16 *)&list->points[i].y);
		#endif
		}
		list->touch_count = CTP_g_event.model;
	#elif defined(__TP_AUX_909_SUPPORT__)
		//mr_trace("fte_data.bt_tp_num = %d", fte_data.bt_tp_num);
		for (i = 0; i < fte_data.bt_tp_num; i++)
		{
			list->points[i].event = CONVER_EVENT(fte_data.pst_point_info[i].bt_tp_property);
			list->points[i].x = fte_data.pst_point_info[i].w_tp_x;
			list->points[i].y = fte_data.pst_point_info[i].w_tp_y;
			list->points[i].z = fte_data.pst_point_info[i].w_tp_strenth;
		#ifdef __MMI_SCREEN_ROTATE__
			if (mmi_frm_get_screen_rotate() != MMI_FRM_SCREEN_ROTATE_0)
			{
				x = (S32)list->points[i].x;
				y = (S32)list->points[i].y;
				gdi_rotate_map_absolute_hw_to_lcd(&x, &y);
				list->points[i].x = (int16)x;
				list->points[i].y = (int16)y;
			}
		#endif /* __MMI_SCREEN_ROTATE__ */
		#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
			mr_layer_zoom_convert_touch_position((int16 *)&list->points[i].x, (int16 *)&list->points[i].y);
		#endif
		}
		list->touch_count = fte_data.bt_tp_num;
	#endif
		mr_touch_panel_convert_event(list, &g_last_touch_info);
		memcpy(&g_last_touch_info, list, sizeof(mr_touchpanel_coord_list_t));
		for (i = 0; i < g_last_touch_info.touch_count; i++)
		{
			if ((g_last_touch_info.points[i].event & 0xff00) != 0)
			{
				g_last_touch_info.points[i].event &= 0x00ff;
			}
		}
		for (i = 0; i < list->touch_count; i++)
		{
			if ((list->points[i].event & 0xff00) == 0)
			{
				mr_trace("point[%d] = [%d, %d, %d]", touch_count, list->points[i].x, list->points[i].y, list->points[i].event);
				memcpy(&list->points[touch_count], &list->points[i], sizeof(mr_touchpanel_coord_ex_t));
				touch_count++;
			}
		}
		list->touch_count = touch_count;
	}
	else
	{
		memset(&g_last_touch_info, 0, sizeof(g_last_touch_info));
	}
#endif
	
	//mr_trace("mr_touch_panel_get_position: %d, %d, %d", pCoord->x, pCoord->y, TP.state);
	
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}
/* __MMI_DSM_NEW_JSKY__ end support multitouch */


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __MR_CFG_FEATURE_DRM__
#include "drm_gprot.h"


typedef struct {
	const char* mime_type;
	const char* src_path;
}MR_DRM_INSTALL_INFO_T;


typedef enum{
	MR_PLATEX_CODE_DRM_SUPPORT = 0,
	MR_PLATEX_CODE_DRM_INSTALL_OBJECT = 1
}mr_platEx_code_drm_enum;


static int32 mr_drm_install_object(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb) 
{
	kal_uint8 serial;
	kal_int32 result;
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};
	
	MR_DRM_INSTALL_INFO_T* pInfo = (MR_DRM_INSTALL_INFO_T*)input;

	if(!pInfo || !pInfo->mime_type || !pInfo->src_path) {
		return MR_FAILED;
	}
	
	result = DRM_install_object((kal_uint8*)pInfo->mime_type, (kal_wchar*)mr_fs_get_filename((char*)fullpathname, pInfo->src_path), MMI_FALSE, NULL, 0, &serial, NULL, NULL);
	return result == DRM_RESULT_OK? MR_SUCCESS : MR_FAILED; 
}


MR_PLATEX_FUNC_BEGIN(drm)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_DRM_INSTALL_OBJECT, mr_drm_install_object)
MR_PLATEX_FUNC_END()

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static int32 mr_platEx_mem_malloc(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(!output_len || *output_len <= 0){
		return MR_FAILED;
	}

	*output = (uint8*)mr_mem_get_ex(*output_len);
	mr_trace("mr_platEx_mem_malloc: %d, %x, %d", *output_len, *output, med_ext_left_size()/1024);	
	return *output? MR_SUCCESS : MR_FAILED;
}


static int32 mr_platEx_mem_free(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(!input){
		return MR_FAILED;
	}
	
	mr_trace("mr_platEx_mem_free: %x", input);
	mr_mem_free_ex((void**)&input);
	return MR_SUCCESS;
}


MR_PLATEX_FUNC_BEGIN(mem)
	MR_PLATEX_FUNC_ENTRY_SUPPORT(MR_PLATEX_CODE_MEM_SUPPORT, MR_SUCCESS)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_MEM_MALLOC, mr_platEx_mem_malloc)
	MR_PLATEX_FUNC_ENTRY(MR_PALTEX_CODE_MEM_FREE, mr_platEx_mem_free)
MR_PLATEX_FUNC_END()


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __MR_CFG_FEATURE_DAM__
int32 mr_dam_sky_cmd_hdlr(mr_dam_appinfo_t* appinfo, mr_dam_command_enum cmd, void* param);

//入口注册配置项表，需要支持的应用必须在这里注册
const static mr_dam_cmd_hdlr_t s_mr_dam_cmd_hdlr_tbl[] = {
	{MR_DAM_PROVIDER_SKY, mr_dam_sky_cmd_hdlr}, //冒泡平台的应用处理函数
};



//冒泡平台的应用处理函数
int32 mr_dam_sky_cmd_hdlr(mr_dam_appinfo_t* appinfo, mr_dam_command_enum cmd, void* param)
{
	switch(cmd)
	{
	case MR_DAM_CHECK_APP:
		//检查应用是否存在，如果应用存在返回MR_SUCCESS, 不存在返回MR_FAILED
		 break;
		 
	case MR_DAM_START_APP:
	{	
		//启动应用，启动成功返回MR_SUCCESS, 失败返回MR_FAILED
		char tmp[100];
		sprintf(tmp, "%%%s", appinfo->filepath);
		mr_app_start_mrp_by_path(tmp);
		break;
	}
	
	case MR_DAM_UNINSTALL_APP:
		//卸载应用, 成功返回MR_SUCCESS, 失败返回MR_FAILED
		break;
	}
	
	return MR_IGNORE;
}


//操作分发函数
int32 mr_dam_dispatch_cmd(mr_dam_appinfo_t* appinfo, mr_dam_command_enum cmd, void* param)
{
	int i;
	
	for(i =0; i < sizeof(s_mr_dam_cmd_hdlr_tbl)/sizeof(*s_mr_dam_cmd_hdlr_tbl); i++)
	{
		if(appinfo->provider == s_mr_dam_cmd_hdlr_tbl[i].provider)
		{
			if(!s_mr_dam_cmd_hdlr_tbl[i].cmd)
				return MR_FAILED;
				
			return s_mr_dam_cmd_hdlr_tbl[i].cmd(appinfo, cmd, param);
		}
	}	
	
	return MR_IGNORE;
}


void mr_dam_convert_filepath(char* filepath)
{
	char fullpathname[DSM_MAX_FILE_LEN] = {0};
	
	if(strlen((char *)mr_fs_get_work_path()) == 0)
		sprintf( fullpathname, "%c:\\", (S8)mr_fs_get_work_drv());
	else
		sprintf( fullpathname, "%c:\\%s\\", (S8)mr_fs_get_work_drv(), mr_fs_get_work_path());

	strcat(fullpathname, filepath);
       mr_fs_separator_vm_to_local((U8 *)fullpathname);	 
	strcpy(filepath, fullpathname);
}

void srv_mrpfactory_install_app(const mr_dam_appinfo_t* info);
void srv_mrpfactory_remove_app(const mr_dam_remove_info_t* info);

int32 mr_dam_install_app(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_dam_appinfo_t* appinfo = (mr_dam_appinfo_t* )input;
	if(!input || input_len != sizeof(mr_dam_appinfo_t)
		|| appinfo->filepath[0] == 0)
	{
		return MR_FAILED;
	}
	
	//mr_dam_convert_filepath(appinfo->filepath);
	//mr_dam_convert_filepath(appinfo->image_path);
	mr_trace("dam install: %d, %d, %d, %d", appinfo->provider, appinfo->appid, appinfo->total_size, appinfo->cur_size);
	mr_trace("%s", appinfo->title_en);
	mr_trace("%s", appinfo->start_data);
	mr_trace("%s", appinfo->filepath);
	mr_trace("%s", appinfo->image_path);
	
#ifdef __MMI_SKY_IPHONE__
	mr_dam_install_app_iphone(appinfo);
#endif

	srv_mrpfactory_install_app(appinfo);
	
	return MR_SUCCESS;
}


int32 mr_dam_update_progress(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_dam_progress_t* progress = (mr_dam_progress_t*)input;
	if(!input || input_len != sizeof(mr_dam_progress_t))
		return MR_FAILED;

	mr_trace("dam update: %d, %d, %d, %d", progress->provider, progress->appid, progress->total_size, progress->progress);
	return MR_SUCCESS;
}


int32 mr_dam_remove_app(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_dam_remove_info_t* pInfo = (mr_dam_remove_info_t*)input;
	if(!input || input_len != sizeof(mr_dam_remove_info_t))
		return MR_FAILED;
	
	mr_trace("dam remove: %d, %d", pInfo->provider, pInfo->appid);
	
#ifdef __MMI_SKY_IPHONE__
	mr_dam_remove_app_iphone(pInfo->provider,pInfo->appid);
#endif

	srv_mrpfactory_remove_app(pInfo);
	
	return MR_SUCCESS;
}

int32 mr_skybmp_open(const WCHAR* path)
{
	int f;
	char type[5] = {0};
	UINT read;
	
	f = FS_Open(path, FS_READ_ONLY);

	if (f < 0)
		return 0;

	FS_Read(f, type, sizeof(type) - 1, &read);

	if(strcmp(type, "SKBM") != 0)
	{
		FS_Close(f);
		f = 0;
	}

	return f;
}

void mr_skybmp_get_size(int32 f, int16* w, int16* h)
{
	UINT read;
	
	FS_Seek(f, 4, FS_FILE_BEGIN);	// 调过4个字节的头

	FS_Read(f, w, 2, &read);
	FS_Read(f, h, 2, &read);
}

uint32 mr_skybmp_get_data(int32 f, void* data, int32 len)
{
	UINT read;

	FS_Seek(f, 8, FS_FILE_BEGIN);
	FS_Read(f, data, len, &read);

	return read;
}

void mr_skybmp_close(int32 f)
{
	FS_Close(f);
}

int32 mr_skybmp2bmp(const WCHAR* src, const WCHAR* dest)
{
	int f;
	int16 w, h;
	int len;
	void* buf;	
	int32 ret = MR_TRUE;
	
	f = mr_skybmp_open(src);
	if (f <= 0) return MR_FALSE;
	
	mr_skybmp_get_size(f, &w, &h);
	len = w * h * 2;
	buf = applib_mem_ap_alloc_framebuffer(APPLIB_MEM_AP_ID_MYTHROAD, len);
	
	if (buf == NULL)
	{
		mr_skybmp_close(f);		
		ret = MR_FALSE;
	}
	else
	{
		mr_skybmp_get_data(f, buf, len);
		mr_skybmp_close(f);
		
		if (gdi_image_bmp_encode_file(w, h, 2, buf, (PS8)dest) == GDI_SUCCEED)
		{
			ret = MR_TRUE;
		}
		else
		{
			ret = MR_FALSE;
		}
		
		applib_mem_ap_free(buf);
	}

	return ret;
}

void test_dam(void)
{
	mr_dam_appinfo_t appinfo;

	memset(&appinfo, 0, sizeof(appinfo));

	appinfo.appid = 100;
	strcpy(appinfo.title_ch, "hello");
	strcpy(appinfo.title_en, "hello");
	strcpy(appinfo.image_path, "app320480/test.ico");
	strcpy(appinfo.filepath, "test.mrp");

	mr_dam_install_app((uint8*)&appinfo, sizeof(appinfo), NULL, NULL, NULL);
}

MR_PLATEX_FUNC_BEGIN(dam)
	MR_PLATEX_FUNC_ENTRY_SUPPORT(MR_PLATEX_CODE_DAM_SUPPORT, MR_SUCCESS)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_DAM_INSTALL, mr_dam_install_app)
	MR_PLATEX_FUNC_ENTRY(MR_PALTEX_CODE_DAM_REMOVE, mr_dam_remove_app)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_DAM_UPDATE_PROGRESS, mr_dam_update_progress)	
MR_PLATEX_FUNC_END()

#endif

#ifdef  __PME_SUPPORT__

typedef enum
{
	MR_PLATEX_CODE_OVS_GET_CUSTENTRY_INDEX = 1,
	MR_PLATEX_CODE_OVS_GET_IDLEDRAW_BUFFER = 2,
	MR_PLATEX_CODE_OVS_GET_PME_HID = 3,
	MR_PLATEX_CODE_OVS_GET_ENTRY_PARAM = 4,
	MR_PLATEX_CODE_OVS_SET_PME = 5,
} mr_platEx_code_ovs_enum;


static int32 mr_pme_get_hid(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb) 
{	
	if(!input) {
		return MR_FAILED;
	}
	
	return mr_app_get_pme_hids(input, input_len);
}

static int32 mr_pme_set_mode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb) 
{	
	if(!input) {
		return MR_FAILED;
	}
	
	return mr_app_set_pme(input, input_len);
}

static int32 mr_get_entry_param(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb) 
{	
	if(!output || !output_len) {
		return MR_FAILED;
	}
	
	return mr_app_get_cust_param(input, input_len, output, output_len);
}

MR_PLATEX_FUNC_BEGIN(ovs)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_OVS_GET_PME_HID, mr_pme_get_hid)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_OVS_SET_PME, mr_pme_set_mode)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_OVS_GET_ENTRY_PARAM, mr_get_entry_param)
MR_PLATEX_FUNC_END()

#endif


#endif

