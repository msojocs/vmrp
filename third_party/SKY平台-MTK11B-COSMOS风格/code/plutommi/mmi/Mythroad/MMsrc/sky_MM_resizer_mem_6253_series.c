
#if (defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__)) && defined(__MTK_TARGET__)
#include "SKY_MM_def.h"

#include "reg_base.h"
#include "Drv_comm.h"
#include "mrp_features.h"
#if defined(DRV_IDP_OLD_DESIGN) && defined(DRV_IDP_6253_SERIES)

#include "intrCtrl.h"
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "SKY_MM_Resizer_mem_6253_series.h"
#include "sky_mm_Resizer_mem.h"
#include "med_utility.h"

static RESZ_MEM_DCB_STRUCT resz_dcb;
///////////////////////////////////////////////////////////////////////////////////////////////

void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT *resize_struct)
{
	kal_uint32 config_reg = 0;
	kal_uint32 horizontal_ratio = 0, vertical_ratio = 0;
	kal_uint32 horizontal_res = 0, vertical_res = 0;

	RESET_RESZ()
	
	config_reg |= RESZ_CFG_SRC1;    
	DRV_WriteReg32(RESZ_CFG, config_reg);

	DRV_WriteReg32(RESZ_SMBASE_Y, resize_struct->y_src_base);
	DRV_WriteReg32(RESZ_SMBASE_U, resize_struct->u_src_base);     
	DRV_WriteReg32(RESZ_SMBASE_V, resize_struct->v_src_base);     

	if (resize_struct->yuv_to_rgb == KAL_TRUE)
	{ 
		DRV_WriteReg32(RESZ_TMBASE_RGB1, resize_struct->rgb_tar_base1);
		DRV_WriteReg32(RESZ_TMBASE_RGB2, resize_struct->rgb_tar_base2); 
		RESZ_RGB_2_YUB_ENABLE();
	}
	else
	{
		DRV_WriteReg32(RESZ_TMBASE_Y, resize_struct->y_tar_base);
		DRV_WriteReg32(RESZ_TMBASE_U, resize_struct->u_tar_base);     
		DRV_WriteReg32(RESZ_TMBASE_V, resize_struct->v_tar_base);     
		RESZ_RGB_2_YUB_DISABLE();
	}

	if (0) //(resize_struct->int_en == KAL_TRUE)
	{ 
		RESZ_FSTART_INT_ENABLE();
		RESZ_MEM_INT_ENABLE();
	}
	else
	{
		RESZ_FSTART_INT_DISABLE();
		RESZ_MEM_INT_DISABLE();
	}       

	SET_RESZ_COARSE_RESIZE_NONE;	

	SET_RESZ_SRC_SIZE(resize_struct->source_width, resize_struct->source_height);
	SET_RESZ_TARGET_SIZE(resize_struct->target_width, resize_struct->target_height);

	// [MAUI_01097417] clear force resizing bits.
	RESZ_CLEAR_FORCE_RESIZE_BITS();
	// [MAUI_01097417] For working around a PRZ HW bug, force PRZ to scale even if the target width is the same as source width.
	if (resize_struct->source_width == resize_struct->target_width)
	{
		RESZ_FORCE_H_RESIZING();
	}
	// [MAUI_01097417] For working around a PRZ HW bug, force PRZ to scale even if the target height is the same as source height.
	// [MAUI_01101749] Resizer HW limitation, when target height is 1, cannot force PRZ to resize.
	if ((resize_struct->source_height == resize_struct->target_height) && 
		(1 != resize_struct->target_height))
	{
		RESZ_FORCE_V_RESIZING();
	}

	/* width ratio */
	if (resize_struct->source_width == resize_struct->target_width)
	{
		horizontal_ratio = (1<<RESZ_H_RATIO_SHIFT_BITS);
	}
	else
	{     
		horizontal_ratio = ((1<<RESZ_H_RATIO_SHIFT_BITS) * (kal_uint32)resize_struct->source_width/(kal_uint32)resize_struct->target_width);
	}   

	if (resize_struct->source_height == resize_struct->target_height)
	{
		vertical_ratio=(1<<RESZ_V_RATIO_SHIFT_BITS);
	}
	else
	{
		vertical_ratio=((1<<RESZ_V_RATIO_SHIFT_BITS)*(kal_uint32)resize_struct->source_height/(kal_uint32)resize_struct->target_height);
	}

	horizontal_res = resize_struct->source_width % resize_struct->target_width;
	vertical_res = resize_struct->source_height % resize_struct->target_height;
	 
	DRV_WriteReg32(RESZ_HRATIO1, horizontal_ratio);
	DRV_WriteReg32(RESZ_VRATIO1, vertical_ratio);   
	DRV_WriteReg32(RESZ_HRES1, horizontal_res);
	DRV_WriteReg32(RESZ_VRES1, vertical_res);   

	SET_RESZ_FINE_RESIZE_WORK_MEM_SIZE(resz_dcb.wmsize);   
	DRV_WriteReg32(RESZ_PRWMBASE, resz_dcb.wmbase);   

	DRV_WriteReg32(RESZ_TMBASE_RGB1, resize_struct->rgb_tar_base1);
	DRV_WriteReg32(RESZ_TMBASE_RGB2, resize_struct->rgb_tar_base2);    
}


kal_int32 RESZ_Mem_Init(void)
{
#if 0
	//IRQ_Retrieve_LISR
	IRQ_Register_LISR(IRQ_RESZ_CODE, RESZ_LISR_Mem, "RESIZER");
	IRQSensitivity(IRQ_RESZ_CODE, LEVEL_SENSITIVE);
	VISUAL_Register_HISR(VISUAL_RESIZER_HISR_ID, RESZ_HISR_Mem);
	IRQUnmask(IRQ_RESZ_CODE);	
#endif

	return NO_ERROR;
}


kal_int32 RESZ_Mem_Start(void)
{
	RESET_RESZ();
	RESZ_START();
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Open(kal_uint32 width)
{
	kal_mem_set(&resz_dcb, 0, sizeof(resz_dcb));
	//resz_dcb.wmbase = (kal_uint32)med_alloc_int_mem(IDP_CP_MAXIMUM_RESZ_WM_SIZE);
	resz_dcb.wmbase = (kal_uint32)MMIntRamMalloc(((width+15)&0xfffffff0)*4*3);
	resz_dcb.wmsize = 4;

	if(resz_dcb.wmbase == 0){
		return RESZ_ERR_OUTOFMEMORY;
	}
	#if (MTK_BIG_VERSION >= 0x9B)
	DRVPDN_Disable(PDN_RESZ);
	#else
	RESZ_POWERON();
	#endif
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Stop(void)
{
	RESET_RESZ();
  	return NO_ERROR;
}


kal_int32 RESZ_Mem_Close(void)
{
	RESZ_Mem_Stop();
	DRV_WriteReg32(RESZ_CON, 0);

	#if (MTK_BIG_VERSION >= 0x9B)
	DRVPDN_Enable(PDN_RESZ);
	#else
	RESZ_POWEROFF();
	#endif

	if(resz_dcb.wmbase){
		//med_free_int_mem((void**)&resz_dcb.wmbase);	
		MMIntRamFree((void**)&resz_dcb.wmbase);	
		resz_dcb.wmbase = NULL;
	}
	
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Status(void)
{
    	kal_uint32 reg, sta = 0;
	reg = DRV_Reg32(RESZ_STA);

	if(!(reg & RESZ_STA_OUTBUSY)){
		sta |= RESZ_MEM_STATUS_MEM1O;
	}

	return sta;
}

void RESZ_Mem_SetYuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV)
{
	REG_RESZ_YUV2RGB |= 1;
	DRV_WriteReg32(RESZ_TMBASE_Y, pY);
	DRV_WriteReg32(RESZ_TMBASE_U, pU);
	DRV_WriteReg32(RESZ_TMBASE_V, pV);
}

#elif (defined(MT6236)||defined(MT6236B))&&(MTK_VERSION <= 0x10A1032)
#include "intrCtrl.h"
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "SKY_MM_Resizer_mem_6253_series.h"
#include "sky_mm_Resizer_mem.h"
#include "med_utility.h"

#include "idp_val.h"
///////////////////////////////////////////////////////////////////////////////////////////////
/*
 * 1. idp_val_video_decode_open() \n
 * 2. idp_val_video_decode_memorymode_config() \n
 * 3. idp_val_video_decode_is_busy() \n
 * 4. idp_val_video_decode_set_yuv_address() \n
 * 5. idp_val_video_decode_restart_all() \n
 * 6. idp_val_video_decode_close() \n
 */
 
 #ifndef __MTK_TARGET__
#define __align(x)
#endif 

#pragma arm section rwdata = "SKYMM_DYNAMIC_ZI", zidata = "SKYMM_DYNAMIC_ZI"
__align(4)     unsigned char g_skymm_idp_int_addr[1280*4*3 + 1024];
#pragma arm section rwdata , zidata

unsigned char g_skymm_idp_ext_addr[1280*4*3 + 1024];
 
kal_uint8* skymm_get_idp_intmem(void)
{
    if(g_skymm_idp_int_addr == NULL)
    {
        ASSERT(0);
    }
    return g_skymm_idp_int_addr; 
}

kal_uint32 skymm_get_idp_intmem_size(void)
{
   return sizeof(g_skymm_idp_int_addr);
}

kal_uint8* skymm_get_idp_extmem(void)
{
    if(g_skymm_idp_ext_addr == NULL)
    {
        ASSERT(0);
    }
    return g_skymm_idp_ext_addr; 
}

kal_uint32 skymm_get_idp_extmem_size(void)
{
   return sizeof(g_skymm_idp_ext_addr);
}
kal_uint32 g_SkyResizerKey = 0;
static int img_status = 0;
void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT *resize_struct)
{
	kal_uint32  key;
	kal_uint32  idp_int_addr;
	kal_uint32  idp_int_size;
	kal_uint32  idp_ext_addr;
	kal_uint32  idp_ext_size;
	kal_uint32  video_src_width_with_dummy;
	kal_uint32  video_src_height_with_dummy;
	kal_uint32  clip_left;
	kal_uint32  clip_right;
	kal_uint32  clip_top;
	kal_uint32  clip_bottom;
	kal_bool    b_quant;
	kal_uint32  quant_addr;
	kal_bool    FMT_4X4;
	MP4DEC_MEMMODE_STRUCT  data_input;


	idp_int_addr = (kal_uint32)skymm_get_idp_intmem();
	idp_int_size = skymm_get_idp_intmem_size();
	idp_ext_addr = 0;//(kal_uint32)skymm_get_idp_extmem();
	idp_ext_size = 0;//skymm_get_idp_extmem_size();
	video_src_width_with_dummy = resize_struct->source_width;
	video_src_height_with_dummy = resize_struct->source_height;
	clip_left = 0;
	clip_right = video_src_width_with_dummy - 1;
	clip_top = 0;
	clip_bottom = video_src_height_with_dummy -1;
	b_quant = 0;
	quant_addr = 0;
	FMT_4X4 = KAL_FALSE;

	data_input.brightness_level = 128;
	data_input.contrast_level = 128;
	data_input.disp_height = resize_struct->target_height;
	data_input.disp_width = resize_struct->target_width;
	if(resize_struct->yuv_to_rgb)
		data_input.image_data_format = IMGDMA_IBW_OUTPUT_RGB565;
	else
	{
		ASSERT(1);
		//data_input.image_data_format = IMGDMA_IBW_OUTPUT_RGB565;
	}
	sk_dbg_print("data_input.image_data_format =%d",data_input.image_data_format);
	
	data_input.image_pitch_bytes = NULL;
	data_input.image_pitch_mode = 0;
	data_input.lcd_buf_addr = resize_struct->rgb_tar_base1;
	data_input.lcd_buf_addr_2 = resize_struct->rgb_tar_base2;
	data_input.lcd_buf_size_2= data_input.lcd_buf_size = data_input.disp_height*data_input.disp_width*2;
	data_input.rotate_angle = 0;
	data_input.tv_output = 0;
	
	idp_val_video_decode_memorymode_config(
		g_SkyResizerKey,
		idp_int_addr,
		idp_int_size,
		idp_ext_addr,
		idp_ext_size,
		video_src_width_with_dummy,
		video_src_height_with_dummy,
		clip_left,
		clip_right,
		clip_top,
		clip_bottom,
		b_quant,
		quant_addr,
		FMT_4X4,
  		&data_input);
	
	while(RESZ_Mem_Status() )
	{
		sk_dbg_print("RESZ_Mem_Config  ");
		kal_sleep_task(1);
	}
	
	idp_val_video_decode_set_yuv_address(g_SkyResizerKey,
										(kal_uint8 *)resize_struct->y_src_base,
										(kal_uint8 *)resize_struct->u_src_base,
										(kal_uint8 *)resize_struct->v_src_base);
	#if 0
	if(img_status == 0)
	{
		int fp = 0;
		unsigned int len =0;
		unsigned int size =0;

		size = resize_struct->source_height * resize_struct->source_width;
		fp = FS_Open(L"e:\\imgx.yuv420",FS_CREATE|FS_READ_WRITE);
		if(fp)
		{
			FS_Write(fp,(void*)resize_struct->y_src_base,size,&len);
			FS_Write(fp,(void*)resize_struct->u_src_base,size/4,&len);
			FS_Write(fp,(void*)resize_struct->v_src_base,size/4,&len);
			FS_Close(fp);
		}
		img_status = 1;
	}
	#endif

}


kal_int32 RESZ_Mem_Init(void)
{
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Start(void)
{
	int ret = 0;
	ret = idp_val_video_decode_restart_all(g_SkyResizerKey);
	sk_dbg_print("RESZ_Mem_Start =%d",ret);
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Open(kal_uint32 width)
{
	int ret = 0;
	
	ret = idp_val_video_decode_open(&g_SkyResizerKey);
	sk_dbg_print("RESZ_Mem_Open =%d, g_SkyResizerKey=%d",ret,g_SkyResizerKey);
	return  ret;
}


kal_int32 RESZ_Mem_Stop(void)
{
  	return NO_ERROR;
}


kal_int32 RESZ_Mem_Close(void)
{
	idp_val_video_decode_close(g_SkyResizerKey);
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Status(void)
{
	int ret = 0;
	int busy = 0;
	ret = idp_val_video_decode_is_busy(g_SkyResizerKey,&busy);
	sk_dbg_print("RESZ_Mem_Status =%d,ret=%d",busy,ret);
 	return busy;
}

#elif defined(MT6236)||defined(MT6236B)//||defined(MT6255)
#include "intrCtrl.h"
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "SKY_MM_Resizer_mem_6253_series.h"
#include "sky_mm_Resizer_mem.h"
#include "med_utility.h"

#include "idp_val.h"
///////////////////////////////////////////////////////////////////////////////////////////////
/*
 * 1. idp_val_video_decode_open() \n
 * 2. idp_val_video_decode_memorymode_config() \n
 * 3. idp_val_video_decode_is_busy() \n
 * 4. idp_val_video_decode_set_yuv_address() \n
 * 5. idp_val_video_decode_restart_all() \n
 * 6. idp_val_video_decode_close() \n
 */
 
 #ifndef __MTK_TARGET__
#define __align(x)
#endif 

#if 0
#pragma arm section rwdata = "SKYMM_DYNAMIC_ZI", zidata = "SKYMM_DYNAMIC_ZI"
__align(4)     unsigned char g_skymm_idp_int_addr[1280*4*3 + 1024];
#pragma arm section rwdata , zidata

unsigned char g_skymm_idp_ext_addr[1280*4*3 + 1024];
 
kal_uint8* skymm_get_idp_intmem(void)
{
    if(g_skymm_idp_int_addr == NULL)
    {
        ASSERT(0);
    }
    return g_skymm_idp_int_addr; 
}

kal_uint32 skymm_get_idp_intmem_size(void)
{
   return sizeof(g_skymm_idp_int_addr);
}

kal_uint8* skymm_get_idp_extmem(void)
{
    if(g_skymm_idp_ext_addr == NULL)
    {
        ASSERT(0);
    }
    return g_skymm_idp_ext_addr; 
}

kal_uint32 skymm_get_idp_extmem_size(void)
{
   return sizeof(g_skymm_idp_ext_addr);
}
#endif

static kal_bool
skymm_idp_val_vp_callback_func(void* param)
{
    sk_dbg_print("skymm_idp_val_vp_callback_func");
    return KAL_TRUE;
}

kal_uint32 g_SkyResizerKey = 0;
static int img_status = 0;
static kal_uint32 g_SkyRszState = 0;
void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT *resize_struct)
{
	kal_uint32  idp_int_addr;
	kal_uint32  idp_int_size;
	kal_uint32  idp_ext_addr;
	kal_uint32  idp_ext_size;
	kal_uint32  video_src_width_with_dummy;
	kal_uint32  video_src_height_with_dummy;
	kal_uint32  clip_left;
	kal_uint32  clip_right;
	kal_uint32  clip_top;
	kal_uint32  clip_bottom;
	kal_bool    b_quant;
	kal_uint32  quant_addr;
	kal_bool    FMT_4X4;
        kal_bool busy;
    idp_val_vp_param_t param;

	sk_dbg_print("RESZ_Mem_Config sw %d sh %d tw %d th %d",resize_struct->source_width, 
		resize_struct->source_height, resize_struct->target_width, resize_struct->target_height);

	if(resize_struct->source_width == 0 || resize_struct->source_height == 0 ||
		resize_struct->target_width == 0 || resize_struct->target_height == 0)
            {
				g_SkyRszState = 1;
                return; // check busy fail, do error handling...
            }
		

        do 
        {
            if (KAL_FALSE == idp_val_vp_is_busy(g_SkyResizerKey, &busy))
            {
				g_SkyRszState = 1;
                return; // check busy fail, do error handling...
            }
            if (KAL_TRUE == busy)
            {
				sk_dbg_print("IDP BUSY!!!!!!!");
					kal_sleep_task(1);
                // Do sleep like kal_sleep_task(1); for 4.615ms to yield CPU to other tasks.
            }
            else
            {
                break;
            }
        } while (1);

        param.ext_working_buffer_addr = 0x0;
        param.ext_working_buffer_size = 0x0;
        param.video_src_width = resize_struct->source_width;
        param.video_src_height = resize_struct->source_height;
        param.src_color_format = IMG_COLOR_FMT_YUV420; //IMG_COLOR_FMT_PACKET_YUYV422;
        param.src_clip_left = 0;
        param.src_clip_right = resize_struct->source_width-1;
        param.src_clip_top = 0;
        param.src_clip_bottom = resize_struct->source_height-1;
        param.contrast_level = 0x80;
        param.brightness_level = 0x80;
        // Assume both MT6236/MT6276 EVB both use HVGA (480x320) LCM
        param.dst_width = resize_struct->target_width; 
        param.dst_height = resize_struct->target_height;
        // Destination clip is currently not supported, just ignore those or set them as no dst clipping
        param.dst_clip_left = 0; 
        param.dst_clip_right = resize_struct->target_width -1;
        param.dst_clip_top = 0;
        param.dst_clip_bottom = resize_struct->target_height -1;
        param.rotation_angle = IMG_ROT_ANGLE_0; //IMG_ROT_ANGLE_90;
        // Use RGB565 or UYVY, suggest to use UYVY on MT6236 since no HW spatial dithering is supported.
	if(resize_struct->yuv_to_rgb == 1)
           param.dst_color_format = IMG_COLOR_FMT_RGB565;
	else	
           param.dst_color_format = IMG_COLOR_FMT_PACKET_YUYV422; 
        param.pitch_enable = KAL_FALSE;
        param.pitch_width = 320; // Since the destinaation frame buffer is for rotated image, the pitch width should be 320 rather than 426.
        param.frame_done_cb = skymm_idp_val_vp_callback_func; // Will be invoked in HISR once done...
        param.frame_done_cb_param = NULL;

        if (KAL_FALSE == idp_val_vp_config(g_SkyResizerKey, &param))
        {
        sk_dbg_print("RESZ_Mem_Config end 1");
				g_SkyRszState = 1;
            return; // config error, do error handling...
        }

        // Please be careful the Y, U, V addresses cannot be cacheable, i.e., cannot be any address higher than 0xF000_0000.
        // Please notice the addresses need to be format aligned...they cannot be odd number
        if (KAL_FALSE == idp_val_vp_config_src_buffer_addr(g_SkyResizerKey,
										resize_struct->y_src_base,
										resize_struct->u_src_base,
										resize_struct->v_src_base))
        {
        sk_dbg_print("RESZ_Mem_Config end 2");
				g_SkyRszState = 1;
            return; // config error, do error handling...
        }

        // Do something to toggle dst double frame buffer switch.
        // Assume to use color formats those have only single plane, such as RGB or UYVY packed format.
        if (KAL_FALSE == idp_val_vp_config_dst_buffer_addr(g_SkyResizerKey, resize_struct->rgb_tar_base1, 0, 0))
        {
        sk_dbg_print("RESZ_Mem_Config end 3");
				g_SkyRszState = 1;
            return;
        }
sk_dbg_print("RESZ_Mem_Config end 444");
}


kal_int32 RESZ_Mem_Init(void)
{
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Start(void)
{
	int ret = 0;
	if(g_SkyRszState == 0)
	{
	ret = idp_val_vp_start(g_SkyResizerKey);
	}
	sk_dbg_print("RESZ_Mem_Start =%d,key=%d",ret,g_SkyResizerKey);
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Open(kal_uint32 width)
{
	int ret = 0;
	
	g_SkyResizerKey = 0;
	ret = idp_val_vp_open(&g_SkyResizerKey);
	sk_dbg_print("RESZ_Mem_Open =%d, g_SkyResizerKey=%d",ret,g_SkyResizerKey);
	return  ret;
}


kal_int32 RESZ_Mem_Stop(void)
{
	idp_val_vp_stop(g_SkyResizerKey);
  	return NO_ERROR;
}


kal_int32 RESZ_Mem_Close(void)
{
    sk_dbg_print("RESZ_Mem_Close start");

	if(g_SkyResizerKey != 0)
	{
		idp_val_vp_close(g_SkyResizerKey);
		g_SkyResizerKey = 0;
	}
	g_SkyRszState = 0;
    sk_dbg_print("RESZ_Mem_Close end");
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Status(void)
{
	kal_bool ret = 0;
	kal_bool busy = 0;
	ret = idp_val_vp_is_busy(g_SkyResizerKey,&busy);
	sk_dbg_print("RESZ_Mem_Status =%d,ret=%d",busy,ret);
 	return busy;
}

#elif defined(MT6252)&&((MTK_VERSION >= 0x10A0000)&&(MTK_VERSION <= 0x10AFFFF))//for 52 10A RESZ
#include "intrCtrl.h"
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "SKY_MM_Resizer_mem_6253_series.h"
#include "sky_mm_Resizer_mem.h"
#include "med_utility.h"
#include "mt6252\\idp_hw\\idp_resz_crz_mt6252.h"
#include "mt6252\\idp_hw\\idp_resz_crz_mt6252.h"
#include "mt6252\\idp_hw\\idp_imgdma_rotdma_mt6252.h"

#include "img_common_enum.h"

extern camera_preview_process_struct isp_preview_config_data;
extern kal_uint32 rotdma_preview_buffer_idx;
extern RESZ_DCB_STRUCT resz_dcb;
extern ROTDMA_DCB_STRUCT rotdma_dcb;

static kal_bool RESZ_img_buffer_resize_frame_start_hisr(void *param)
{
    // TODO: Debug use.

    return KAL_TRUE;
}

static kal_bool RESZ_img_buffer_resize_frame_end_hisr(void *param)
{
    // TODO: Debug use.

    return KAL_TRUE;
}

void ROTDMA_img_buffer_resize_update(void)  //HISR
{
#if 1
  kal_uint32 preview_buffer_idx;
  kal_uint32 savedMask;
  

  savedMask = SaveAndSetIRQMask();
  preview_buffer_idx = rotdma_preview_buffer_idx;
  RestoreIRQMask(savedMask);


  return;
#else
    #if defined(__MTK_TARGET__)
    	  kal_uint32 preview_buffer_idx;
    	  kal_uint32 savedMask;
    
    	  savedMask = SaveAndSetIRQMask();
    	  preview_buffer_idx = rotdma_preview_buffer_idx;
    	  RestoreIRQMask(savedMask);
    
    #ifdef MDP_FW_CTRL_FLOW_DEBUG
    	  _rszr_add_debug_trace(ROTDMA_TRACE_ID_HISR_PREVIEW_UPDATE_LCD, preview_buffer_idx);
    #endif
    
    	  if (1 == preview_buffer_idx)  // means buffer 0 is done.
    	  {
    	          LCD_preview(isp_preview_config_data.hw_update_layer,
    	                      isp_preview_config_data.frame_buffer_address, 
    	                      isp_preview_config_data.frame_buffer_address);       
    	  }
    	  else if (0 == preview_buffer_idx) // means buffer 1 is done.
    	  {
    	          LCD_preview(isp_preview_config_data.hw_update_layer,
    	                      isp_preview_config_data.frame_buffer_address1,
    	                      isp_preview_config_data.frame_buffer_address1);                                      
    	  } 
    	  else
    	  {
    	      ASSERT(0);
    	  }
    #endif  //#if defined(__MTK_TARGET__)
    
    	  return;
#endif

}/*硬缩放旋转的刷新*/

static void RESZ_img_buffer_resize_config(
		idp_resz_crz_struct *RESZ_struct,
	    kal_uint32 const y_address,
	    kal_uint32 const u_address,
	    kal_uint32 const v_address,
	   	kal_uint16 t_src_width,
	    kal_uint16 t_src_height,
	   	kal_uint16 t_tar_width,
	    kal_uint16 t_tar_height)
{
    RESZ_struct->two_pass_resize_en = KAL_FALSE;
    RESZ_struct->input_src1 = IDP_MODULE_OUTER_MEMORY_IN;

	RESZ_struct->yuv_color_fmt1 = IMG_COLOR_FMT_YUV420;

    RESZ_struct->src_buff_y_addr = y_address;
    RESZ_struct->src_buff_u_addr = u_address;
    RESZ_struct->src_buff_v_addr = v_address;

    RESZ_struct->fend_intr_en = KAL_TRUE;
    RESZ_struct->fend_intr_cb = RESZ_img_buffer_resize_frame_end_hisr;
    RESZ_struct->fend_intr_cb_param = NULL;

    RESZ_struct->fstart1_intr_en = KAL_TRUE;
    RESZ_struct->fstart1_intr_cb = RESZ_img_buffer_resize_frame_start_hisr;
    RESZ_struct->fstart1_intr_cb_param = NULL;

    RESZ_struct->mem_in_done_intr_en = KAL_FALSE;

    RESZ_struct->lock_drop_frame_intr_en = KAL_TRUE;
    RESZ_struct->lock_drop_frame_intr_cb = NULL;
    RESZ_struct->lock_drop_frame_intr_cb_param = NULL;

    RESZ_struct->auto_restart = KAL_FALSE;
    RESZ_struct->hw_frame_sync = KAL_FALSE;

    RESZ_struct->src_width = t_src_width;
    RESZ_struct->src_height = t_src_height;

    RESZ_struct->tar_width = t_tar_width;
    RESZ_struct->tar_height = t_tar_height;

    RESZ_struct->clip_en = KAL_FALSE; //only memory input valid

    RESZ_struct->crop_en1 = KAL_FALSE; //pass 1

    RESZ_struct->cam_dz_cb = NULL;

#if defined(MP4_ENCODE)
    RESZ_struct->get_buffer_cb = NULL;
#endif // #if defined(MP4_ENCODE) 


    
     return;
}/*硬缩放 BUFFER的配置*/ 
static void ROTDMA_img_buffer_resize_fe_lisr(void *param)
{

	visual_active_hisr(VISUAL_IMGDMA_HISR_ID);

  	return;
}

static void ROTDMA_img_buffer_resize_config(idp_imgdma_rotdma_struct *ROTDMA_struct,
														kal_uint32 player_buffer)
{

    ROTDMA_struct->yuv_color_fmt = IMG_COLOR_FMT_PACKET_YUYV422;

    ROTDMA_struct->y_dest_start_addr = player_buffer;//(kal_uint32)&(img_buffer_resize_dest[0]);

    ROTDMA_struct->src_width = resz_dcb.cfg.tar_width;
    ROTDMA_struct->src_height = resz_dcb.cfg.tar_height;

    ROTDMA_struct->pitch_enable = KAL_FALSE;

    ROTDMA_struct->fend_intr_en = KAL_TRUE;
    ROTDMA_struct->fend_intr_cb = ROTDMA_img_buffer_resize_fe_lisr;  
    ROTDMA_struct->fend_intr_cb_param = NULL;

    ROTDMA_struct->q_empty_intr_en = KAL_FALSE;
    ROTDMA_struct->descriptor_mode = KAL_FALSE;
    ROTDMA_struct->auto_loop = KAL_FALSE;
    ROTDMA_struct->hw_frame_sync = KAL_TRUE;
    ROTDMA_struct->drop_prev_engine_when_off = KAL_FALSE;

    ROTDMA_struct->slow_down_en = KAL_FALSE;
    ROTDMA_struct->protect_en = KAL_TRUE;   //According to spec. From Camera, enable protect
    ROTDMA_struct->ultra_high_en = KAL_FALSE;
    ROTDMA_struct->bus_control_threshold = ROTDMA_BUS_CONTROL_THRESHOLD;

	ROTDMA_struct->rot_angle = IMG_ROT_ANGLE_0;

	rotdma_preview_buffer_idx = 0;

    return;
} /*旋转后buffer配置*/
kal_int32 RESZ_Mem_Init(void)
{
	//sk_dbg_print("RESZ_Mem_Init");

	return NO_ERROR;
}

kal_int32 RESZ_Mem_Status(void)
{
	kal_bool ret = 0;
	ret = RESZ_CheckBusy();

	//sk_dbg_print("RESZ_Mem_Status ret=%d",ret);
 	return (kal_int32)ret;
}
kal_int32 RESZ_Mem_Start(void)
{

	//sk_dbg_print("RESZ_Mem_Start ");
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Open(kal_uint32 width)
{

	kal_int32 ret =0;

	//sk_dbg_print("RESZ_Mem_Open ");
	return  ret;
}

kal_int32 RESZ_Mem_Close(void)
{
	//sk_dbg_print("RESZ_Mem_Close ");
	return;
}
void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT *resize_struct)
{
	idp_resz_crz_struct *p_resizer_cfg = NULL;
	idp_imgdma_rotdma_struct *p_rotdma_cfg = NULL;
	MMDI_SCENERIO_ID scenario_id = SCENARIO_SKYMM_IMG_BUFFER_HW_RESIZE_ID;
	lcd_frame_update_struct lcd_data;

	kal_uint16  video_src_width_with_dummy = 0;
	kal_uint16  video_src_height_with_dummy = 0;
	kal_uint16 target_width = 0;
	kal_uint16 target_height = 0;
	kal_uint32 y_address = 0;
	kal_uint32 u_address = 0;
	kal_uint32 v_address= 0;
	kal_uint32 player_buffer = 0;

	RESZ_Open(scenario_id);
	ROTDMA_Open(scenario_id);

	p_resizer_cfg = &(resz_dcb.cfg);
	p_rotdma_cfg = &(rotdma_dcb.cfg);
	video_src_width_with_dummy = (kal_uint16)resize_struct->source_width;
	video_src_height_with_dummy = (kal_uint16)resize_struct->source_height;
	target_width = (kal_uint16)resize_struct->target_width;
	target_height = (kal_uint16)resize_struct->target_height;
	y_address = resize_struct->y_src_base;
	u_address = resize_struct->u_src_base;
	v_address = resize_struct->v_src_base;
	player_buffer = resize_struct->rgb_tar_base1;
	
	//sk_dbg_print("Allan RESZ_Mem_Config  y_address=%d,u_address=%d,v_address=%d",y_address,u_address,v_address);
	//sk_dbg_print("Allan RESZ_Mem_Config  src_width=%d,src_height=%d,target_width=%d,target_height=%d",video_src_width_with_dummy,video_src_height_with_dummy,target_width,target_height);

	RESZ_img_buffer_resize_config((idp_resz_crz_struct *) p_resizer_cfg,
									y_address,
									u_address,
									v_address,
									video_src_width_with_dummy,
									video_src_height_with_dummy,
									target_width,
									target_height);
	ROTDMA_img_buffer_resize_config((idp_imgdma_rotdma_struct *) p_rotdma_cfg,
										player_buffer);

	RESZ_Config((idp_resz_crz_struct *) p_resizer_cfg);
	ROTDMA_Config((idp_imgdma_rotdma_struct *) p_rotdma_cfg);

	// Start sequence from output to input
	//RESZ_Mem_Start();
	ROTDMA_Start(scenario_id);
	RESZ_Start(scenario_id);


	while (RESZ_Mem_Status())
	{
		//sk_dbg_print("RESZ_Mem_Config  ");
		//sk_dbg_print("VM_LOG:RESZ_Mem_Complete  %s,%d, %d", __FILE__, __LINE__, mr_getTime());
		kal_sleep_task(1);
	}
	
	RESZ_Close(scenario_id);
	//ROTDMA_Close(scenario_id);			// It already be closed in RESZ_Close.

}/*硬缩放实现的配置主函数*/
/* ---- SKY_MULTIMEDIA end --for MT6252 RESZ-- */

#elif (defined(MT6252)/*||defined(MT6255)*/)&&((MTK_VERSION >= 0x11B0000)&&(MTK_VERSION <= 0x11BFFFF))//for 52 11B RESZ
#define __MR_MT6252_SUPPORT_HW_RSZ__
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "SKY_MM_Resizer_mem_6253_series.h"
#include "sky_mm_Resizer_mem.h"

#define THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL
#include <idp_core.h>

//#include <idp_mem.h>
#include <img_common_enum.h>
//#include <mt6252/idp_engines.h>
#include <mm_comm_def.h>

// From first to last

#include <mt6252/idp_hw/idp_resz_crz.h>
#include <mt6252/idp_hw/idp_imgdma_rotdma0.h>
kal_uint32 g_SkyResizerKey = 0;
kal_uint32 g_skyDmaKey = 0;
//idp_resz_crz_struct g_SkyCrzStruct; 
static kal_uint32 g_SkyRszState = 0;

extern idp_resz_crz_struct         crz_struct;
extern idp_imgdma_rotdma0_struct    rotdma0_struct;

//#define __MR_MT6252_SUPPORT_HW_RSZ__

void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT *resize_struct)
{
#ifdef __MR_MT6252_SUPPORT_HW_RSZ__
        kal_bool busy;
	 kal_uint32 result;
		
	sk_dbg_print("RESZ_Mem_Config sw %d sh %d tw %d th %d",resize_struct->source_width, 
		resize_struct->source_height, resize_struct->target_width, resize_struct->target_height);

	if(resize_struct->source_width == 0 || resize_struct->source_height == 0 ||
		resize_struct->target_width == 0 || resize_struct->target_height == 0)
            {
				g_SkyRszState = 1;
                return; // check busy fail, do error handling...
            }
	
	result = idp_resz_crz_is_busy(g_SkyResizerKey,&busy, &crz_struct);
	sk_dbg_print("RESZ_Mem_Status =%d,ret=%d",busy,result);

    memset(&crz_struct, 0, sizeof(crz_struct));
    kal_mem_set(&(crz_struct.mask[0]), 0xff, sizeof(crz_struct.mask));


	
    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;	

    crz_struct.src_buff_y_addr = resize_struct->y_src_base;
    crz_struct.src_buff_u_addr = resize_struct->u_src_base;
    crz_struct.src_buff_v_addr = resize_struct->v_src_base;

    //crz_struct.auto_restart = KAL_TRUE;
    //crz_struct.hw_frame_sync = KAL_TRUE;

    crz_struct.src_width = resize_struct->source_width;         //original source height, before crop
    crz_struct.src_height = resize_struct->source_height;           //original source width, before crop
    crz_struct.tar_width = resize_struct->target_width;
    crz_struct.tar_height = resize_struct->target_height;

	/*
    crz_struct.fend_intr_en = KAL_TRUE;
    crz_struct.fend_intr_cb = NULL;
    crz_struct.fend_intr_cb_param = NULL;

    crz_struct.fstart1_intr_en = KAL_TRUE;
    crz_struct.fstart1_intr_cb = NULL;
    crz_struct.fstart1_intr_cb_param = NULL;

    crz_struct.pixel_dropped_intr_en = KAL_TRUE;
    crz_struct.pixel_dropped_intr_cb = NULL;
    crz_struct.pixel_dropped_intr_cb_param = NULL;

    crz_struct.lock_drop_frame_intr_en = KAL_TRUE;
    crz_struct.lock_drop_frame_intr_cb = NULL;
    crz_struct.lock_drop_frame_intr_cb_param = NULL;


    //crz_struct.clip_en = KAL_FALSE; //only memory input valid
    //crz_struct.org_width = 1000; //only memory input valid

    if (KAL_TRUE == crz_struct.crop_en1)
    {
        if ((0 == config.crop_width) ||
            (0 == config.crop_height) ||
            (crz_struct.src_width < config.crop_width) ||
            (crz_struct.src_height < config.crop_height))
        {
            return KAL_FALSE;
        }
        crz_struct.crop_left1 = (crz_struct.src_width - config.crop_width) >> 1;
        crz_struct.crop_right1 = crz_struct.crop_left1 + (config.crop_width - 1);
        crz_struct.crop_top1 = (crz_struct.src_height - config.crop_height) >> 1;
        crz_struct.crop_bottom1 = crz_struct.crop_top1 + (config.crop_height - 1);
    }
    */

    result = idp_resz_crz_config(g_SkyResizerKey, &crz_struct, KAL_TRUE);
    if (KAL_FALSE == result)
    {
		g_SkyRszState = 1;
        return;
    }

    kal_mem_set(&(crz_struct.mask[0]), 0x00, sizeof(crz_struct.mask));

    memset(&rotdma0_struct, 0, sizeof(rotdma0_struct));
    rotdma0_struct.yuv_color_fmt = MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
    rotdma0_struct.y_dest_start_addr = resize_struct->rgb_tar_base1;
    //rotdma0_struct.u_dest_start_addr = config.frame_buffer_address;
    //rotdma0_struct.v_dest_start_addr = config.frame_buffer_address;



    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    //rotdma0_struct.pitch_enable = KAL_FALSE;
    //rotdma0_struct.pitch_btyes = 1000;

    rotdma0_struct.fend_intr_en = KAL_TRUE;
    rotdma0_struct.fend_intr_cb = NULL;
    rotdma0_struct.fend_intr_cb_param = NULL;

    //rotdma0_struct.q_empty_intr_en = KAL_FALSE;
    //rotdma0_struct.q_empty_intr_cb = NULL;
    //rotdma0_struct.q_empty_intr_cb_param = NULL;

    //rotdma0_struct.descriptor_mode = KAL_FALSE;
    //rotdma0_struct.queue_depth = 2; /**[1,2] */
    //rotdma0_struct.queue_empty_drop = KAL_TRUE; /** otherwise: stall previous engine */
    //rotdma0_struct.des_queue_data_addr = NULL; //new: 6573 for future WinMo driver


    rotdma0_struct.auto_loop = KAL_FALSE;
    rotdma0_struct.hw_frame_sync = KAL_TRUE;
    rotdma0_struct.drop_prev_engine_when_off = KAL_FALSE;

    //rotdma0_struct.slow_down_en = KAL_FALSE;
    //rotdma0_struct.slow_down_cnt = 10;

    rotdma0_struct.protect_en = KAL_FALSE; //KAL_TRUE;   //According to spec. From Camera, enable protect
    rotdma0_struct.ultra_high_en = KAL_FALSE;
    rotdma0_struct.bus_control_threshold = ROTDMA_BUS_CONTROL_THRESHOLD;


    result = idp_imgdma_rotdma0_config(g_skyDmaKey, &rotdma0_struct, KAL_TRUE);
    if (KAL_FALSE == result)
    {
        return;
    }
#endif
    return;
}

kal_int32 RESZ_Mem_Init(void)
{
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Start(void)
{
#ifdef __MR_MT6252_SUPPORT_HW_RSZ__
	int ret = 0;
	if(g_SkyRszState == 0)
	{
	ret = idp_resz_crz_start(g_SkyResizerKey, &crz_struct);
	sk_dbg_print("RESZ_Mem_Start =%d",ret);
    ret = idp_imgdma_rotdma0_start(g_skyDmaKey, &rotdma0_struct);
	
	}
	sk_dbg_print("RESZ_Mem_Start2 =%d",ret);
#endif	
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Open(kal_uint32 width)
{
	int ret = 0;
	
#ifdef __MR_MT6252_SUPPORT_HW_RSZ__
	g_SkyResizerKey = 0;
	ret = idp_resz_crz_open(&g_SkyResizerKey);
	sk_dbg_print("RESZ_Mem_Open =%d, g_SkyResizerKey=%d",ret,g_SkyResizerKey);
	if(ret == KAL_FALSE || g_SkyResizerKey == 0)
	{
		return ret;
	}
	ret = idp_imgdma_rotdma0_open(&g_skyDmaKey);
	sk_dbg_print("DMA =%d, g_skyDmaKey=%d",ret,g_skyDmaKey);
	if(ret == KAL_FALSE || g_skyDmaKey == 0)
	{
		RESZ_Mem_Close();
		return ret;
	}
#endif
	return  ret;
}


kal_int32 RESZ_Mem_Stop(void)
{
#ifdef __MR_MT6252_SUPPORT_HW_RSZ__
	idp_resz_crz_stop(g_SkyResizerKey, &crz_struct);
    idp_imgdma_rotdma0_stop(g_skyDmaKey, &rotdma0_struct);
#endif
  	return NO_ERROR;
}


kal_int32 RESZ_Mem_Close(void)
{
#ifdef __MR_MT6252_SUPPORT_HW_RSZ__
	if(g_SkyResizerKey != 0)
	{
		idp_resz_crz_close(g_SkyResizerKey, &crz_struct);
		g_SkyResizerKey = 0;
	}
	if(g_skyDmaKey != 0)
	{
		idp_imgdma_rotdma0_close(g_skyDmaKey, &rotdma0_struct);
		g_skyDmaKey = 0;
	}
	g_SkyRszState = 0;
#endif
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Status(void)
{
	kal_bool ret = 0;
	kal_bool busy = 0;
#ifdef __MR_MT6252_SUPPORT_HW_RSZ__
    idp_imgdma_rotdma0_is_busy(g_skyDmaKey, &busy, &rotdma0_struct);
	sk_dbg_print("RESZ_Mem_Status DMA =%d,ret=%d",busy,ret);
    if (KAL_TRUE == (busy))
    {
        return KAL_TRUE;
    }
	ret = idp_resz_crz_is_busy(g_SkyResizerKey,&busy, &crz_struct);
	sk_dbg_print("RESZ_Mem_Status =%d,ret=%d",busy,ret);
#endif
 	return busy;
}



void RESZ_Mem_SetYuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV)
{
}
#elif defined(MT625000)//defined(MT6255)//Allan
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "SKY_MM_Resizer_mem_6253_series.h"
#include "sky_mm_Resizer_mem.h"

#define THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL
#include <idp_core.h>

//#include <idp_mem.h>
#include <img_common_enum.h>
//#include <mt6252/idp_engines.h>
#include <mm_comm_def.h>

// From first to last

#include <mt6255/idp_hw/idp_resz_crz.h>
#include "mt6255\idp_engines.h"
#if defined(IDP_HISR_SUPPORT)
#include <visualhisr.h>
#include <mt6255/idp_hisr.h>
#endif // #if defined(IDP_HISR_SUPPORT)

kal_uint32 g_SkyResizerKey = 0;
kal_uint32 g_skyIppKey = 0;
kal_uint32 g_skyRdmaKey = 0;
kal_uint32 g_skyMoutKey = 0;
kal_uint32 g_skyVrzKey = 0;
kal_uint32 g_skyRgbrot0Key = 0;

//idp_resz_crz_struct g_SkyCrzStruct; 
static kal_uint32 g_SkyRszState = 0;

extern idp_resz_crz_struct         crz_struct;
extern idp_imgproc_ipp_struct      ipp_struct;
extern idp_imgdma_rdma_struct      rdma_struct;
extern idp_imgdma_mout_struct      mout_struct;
extern idp_resz_vrz_struct         vrz_struct;
extern idp_imgdma_rgbrot0_struct   rgbrot0_struct;

extern idp_hisr_handle_t _idp_scen_image_resize_frame_done_hisr_handle;
//#define __MR_MT6252_SUPPORT_HW_RSZ__
#include "custom_idp.h"
static void
sky_idp_scen_image_resize__lisr_crz__frame_start(void *user_data)
{
  idp_add_traced_lisr(IDP_TRACED_API_image_resize__________CRZ_START_LISR);
}


static void
sky_idp_scen_image_resize__lisr_crz__frame_done(void *user_data)
{
  idp_add_traced_lisr(IDP_TRACED_API_image_resize__________CRZ_DONE_LISR);
}
static void
sky_idp_scen_image_resize__lisr_rdma(void *user_data)
{
  idp_add_traced_lisr(IDP_TRACED_API_image_resize__________IBR0_LISR);
}
static void
sky_idp_scen_image_resize__lisr_rgbrot0(void *user_data)
{
  idp_add_traced_lisr(IDP_TRACED_API_image_resize__________IRT1_LISR);

#if defined(IDP_HISR_SUPPORT)
  idp_hisr_activate(_idp_scen_image_resize_frame_done_hisr_handle);
#endif  // #if defined(IDP_HISR_SUPPORT)

}

void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT *resize_struct)
{
        kal_bool busy;
	 kal_uint32 result;
		
	sk_dbg_print("RESZ_Mem_Config sw %d sh %d tw %d th %d",resize_struct->source_width, 
		resize_struct->source_height, resize_struct->target_width, resize_struct->target_height);

	if(resize_struct->source_width == 0 || resize_struct->source_height == 0 ||
		resize_struct->target_width == 0 || resize_struct->target_height == 0)
            {
				g_SkyRszState = 1;
                return; // check busy fail, do error handling...
            }
	
	//result = idp_resz_crz_is_busy(g_SkyResizerKey,&busy, &crz_struct);
	//sk_dbg_print("RESZ_Mem_Status =%d,ret=%d",busy,result);

 memset(&crz_struct, 0, sizeof(crz_struct));
    kal_mem_set(&(crz_struct.mask[0]), 0xff, sizeof(crz_struct.mask));
    // path control parameters
     crz_struct.input_src = IDP_MODULE_IMGDMA_RDMA;
     crz_struct.bypass = KAL_FALSE;
     crz_struct.continuous = KAL_FALSE;
     crz_struct.hw_frame_sync = KAL_FALSE;
    
     // interrupt parameters
     crz_struct.done_intr_en = KAL_TRUE;
     crz_struct.done_intr_cb = sky_idp_scen_image_resize__lisr_crz__frame_done;
     crz_struct.done_intr_cb_param = NULL;
     crz_struct.start_intr_en = KAL_TRUE;
     crz_struct.start_intr_cb = sky_idp_scen_image_resize__lisr_crz__frame_start;
     crz_struct.start_intr_cb_param = NULL;
    
     // input/output sizes configs
     
     crz_struct.src_width = resize_struct->source_width;         //original source height, before crop
     crz_struct.src_height = resize_struct->source_height;           //original source width, before crop
     crz_struct.tar_width = resize_struct->target_width;
     crz_struct.tar_height = resize_struct->target_height;

    
     // quality configs
     crz_struct.set_usel_dsel = KAL_TRUE;
     idp_internal_crz_usel_dsel(CUSTOM_SCENARIO_IMAGE_RESIZE, 
                                crz_struct.src_width, crz_struct.src_height, 
                                crz_struct.tar_width, crz_struct.tar_height, 
                                &crz_struct.usel, &crz_struct.dsel);
     crz_struct.set_ecv = KAL_FALSE;
    
       
     result = idp_resz_crz_config(g_SkyResizerKey, &crz_struct, KAL_TRUE);
     if (KAL_FALSE == result) 
        {
            g_SkyRszState = 1;
            sk_dbg_print("config  resz false");
            return ;
        }

     kal_mem_set(&(crz_struct.mask[0]), 0x00, sizeof(crz_struct.mask));
     memset(&ipp_struct, 0, sizeof(ipp_struct));
     ipp_struct.hw_frame_sync = KAL_FALSE;
     ipp_struct.bypass = KAL_FALSE;
     
     ipp_struct.cba = KAL_FALSE;
     
     result = idp_imgproc_ipp_config(g_skyIppKey, &ipp_struct, KAL_TRUE);
     if (KAL_FALSE == result) 
        {
           g_SkyRszState = 1;
           sk_dbg_print("config  ipp false");
        return ;
        }
     memset(&rdma_struct, 0, sizeof(rdma_struct));

     
     rdma_struct.output_dst = IDP_MODULE_RESZ_CRZ;
     rdma_struct.descriptor_mode = KAL_FALSE;
     rdma_struct.queue_depth = 1;
     
     rdma_struct.bypass = KAL_FALSE;
     rdma_struct.stall_prev_engine = KAL_FALSE;
     rdma_struct.drop_prev_engine = KAL_FALSE;
     rdma_struct.auto_loop = KAL_FALSE;
     rdma_struct.frame_sync_en = KAL_FALSE;
     
     rdma_struct.rgb_rounding = KAL_FALSE;
     
     /* Configs for register mode. */
     rdma_struct.input_rgb = KAL_FALSE;
     rdma_struct.rgb_color_fmt = IMG_COLOR_FMT_YUV420;
     rdma_struct.yuv_color_fmt = IMG_COLOR_FMT_PACKET_YUYV422;
     
     rdma_struct.pixel_number = resize_struct->source_width*resize_struct->source_height; // for check if cacheable
     // TODO: check source width/height here...
     rdma_struct.source_width = resize_struct->source_width;
     rdma_struct.source_height = resize_struct->source_height;
     
    
     rdma_struct.y_base_addr = resize_struct->y_src_base;
     rdma_struct.u_base_addr = resize_struct->u_src_base;
     rdma_struct.v_base_addr = resize_struct->v_src_base;
     
     rdma_struct.clip = KAL_FALSE; /**< clip enable, this was called "pan" in MT6238 */
     if (KAL_TRUE == rdma_struct.clip)
     {
     }
     else
     {
       rdma_struct.clip_left = 0;
       rdma_struct.clip_right = rdma_struct.source_width - 1;
       rdma_struct.clip_top = 0;
       rdma_struct.clip_bottom = rdma_struct.source_height - 1;
     }
     
     rdma_struct.intr_en = KAL_TRUE;
     rdma_struct.intr_cb = sky_idp_scen_image_resize__lisr_rdma;
     rdma_struct.intr_cb_param = NULL;
     
       
     result = idp_imgdma_rdma_config(g_skyRdmaKey, &rdma_struct, KAL_TRUE);
     if (KAL_FALSE == result) 
        {
         g_SkyRszState = 1;
         sk_dbg_print("config  rdma false");
             return;
        }
     memset(&mout_struct, 0, sizeof(mout_struct));
     mout_struct.output_vrz = KAL_TRUE; 
      mout_struct.frame_sync_en = KAL_FALSE;
       
      result = idp_imgdma_mout_config(g_skyMoutKey, &mout_struct, KAL_TRUE);
      if (KAL_FALSE == result) 
        {
         g_SkyRszState = 1;
         sk_dbg_print("config  mout false");
           return;
          }

      memset(&vrz_struct, 0, sizeof(vrz_struct));
      vrz_struct.input_src = IDP_MODULE_MMSYS_MOUT;
       vrz_struct.output_dst = IDP_MODULE_IMGDMA_RGBROT0;
       vrz_struct.auto_restart = KAL_FALSE;
      
       vrz_struct.bypass = KAL_TRUE;
       vrz_struct.hw_frame_sync = KAL_FALSE;
       vrz_struct.crop_enable = KAL_FALSE;
               
       result = idp_resz_vrz_config(g_skyVrzKey, &vrz_struct, KAL_TRUE);
       if (KAL_FALSE == result) 
        {
        g_SkyRszState = 1;
        sk_dbg_print("config  vrz false");
            return;
        }
       memset(&rgbrot0_struct, 0, sizeof(rgbrot0_struct));

         // interrupt
         rgbrot0_struct.intr_en = KAL_TRUE; /**< enable int or not */
         rgbrot0_struct.intr_cb = sky_idp_scen_image_resize__lisr_rgbrot0;
         rgbrot0_struct.intr_cb_param = NULL;
       
         // path control
         rgbrot0_struct.input_src = IDP_MODULE_RESZ_VRZ;
         rgbrot0_struct.auto_loop = KAL_FALSE; /**< auto loop mode */
         rgbrot0_struct.stall_prev_engine_when_off = KAL_FALSE;
         rgbrot0_struct.drop_prev_engine_when_off = KAL_FALSE;
         rgbrot0_struct.hw_frame_sync = KAL_FALSE;
         
       
         // descriptor mode
         rgbrot0_struct.descriptor_mode = KAL_FALSE;
         rgbrot0_struct.queue_depth = 1;  //Need be set 1 to ensure the depth = 0 in register mdoe
       
       
         // src size
         rgbrot0_struct.src_width = resize_struct->target_width; /**< width */
         rgbrot0_struct.src_height = resize_struct->target_height; /**< height */
       
         // clip is not supported
         rgbrot0_struct.clip = KAL_FALSE;
         rgbrot0_struct.clip_top = 0;
         rgbrot0_struct.clip_bottom = resize_struct->source_height - 1;
         rgbrot0_struct.clip_left = 0;  
         rgbrot0_struct.clip_right = resize_struct->source_width - 1;
       
         // rotation/flip  

           rgbrot0_struct.flip = KAL_FALSE;
           rgbrot0_struct.rot_angle = IMG_ROT_ANGLE_0;
       
         // color format
         // TODO: configure as PACKET_YUYV422 first...
         rgbrot0_struct.rgb_output = KAL_FALSE;
         rgbrot0_struct.rgb_color_fmt = IMG_COLOR_FMT_PACKET_YUYV422;
         rgbrot0_struct.yuv_color_fmt = IMG_COLOR_FMT_PACKET_YUYV422;
         rgbrot0_struct.rgb_rounding = KAL_TRUE;
         //dither

           rgbrot0_struct.dither_en = KAL_FALSE;
       
         rgbrot0_struct.alpha_value = 0xFF;
       
         // output buffer
         {
           rgbrot0_struct.y_dest_start_addr = resize_struct->rgb_tar_base1;
         }
       
         // pitch
         // TODO: check this!!
         rgbrot0_struct.pitch_enable = KAL_FALSE; /**< pitch */
         rgbrot0_struct.pitch_bytes = 320; /**< horizontal pitch WARNING! the pitch is applied to three buffers */ 
             
         result = idp_imgdma_rgbrot0_config(g_skyRgbrot0Key, &rgbrot0_struct, KAL_TRUE);
         if (KAL_FALSE == result) 
            {
            g_SkyRszState = 1;
            sk_dbg_print("config  rgbrot0 false");
            return ;
            }

    sk_dbg_print("config  end");


    return;
}

kal_int32 RESZ_Mem_Init(void)
{
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Start(void)
{
	int ret = 0;
	if(g_SkyRszState == 0)
	{
	ret = idp_resz_crz_start(g_SkyResizerKey,&crz_struct);
    ret = idp_imgproc_ipp_start(g_skyIppKey,&ipp_struct);
    ret = idp_imgdma_rdma_start(g_skyRdmaKey,&rdma_struct);
    ret = idp_imgdma_mout_start(g_skyMoutKey,&mout_struct);
    ret = idp_resz_vrz_start(g_skyVrzKey,&vrz_struct);
    ret = idp_imgdma_rgbrot0_start(g_skyRgbrot0Key,&rgbrot0_struct);
	}
	sk_dbg_print("RESZ_Mem_Start2 =%d",ret);
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Open(kal_uint32 width)
{
	int ret = 0;
	
	g_SkyResizerKey = 0;
	ret = idp_resz_crz_open(&g_SkyResizerKey);
	sk_dbg_print("RESZ_Mem_Open =%d, g_SkyResizerKey=%d",ret,g_SkyResizerKey);
	if(ret == KAL_FALSE || g_SkyResizerKey == 0)
	{
		return ret;
	}
    g_skyIppKey = 0;
    ret = idp_imgproc_ipp_open(&g_skyIppKey);
	sk_dbg_print("RESZ_Mem_Open =%d, g_skyIppKey=%d",ret,g_skyIppKey);
	if(ret == KAL_FALSE || g_skyIppKey == 0)
	{
		return ret;
	}
    g_skyRdmaKey = 0;
    ret = idp_imgdma_rdma_open(&g_skyRdmaKey);
	sk_dbg_print("RESZ_Mem_Open =%d, g_skyRdmaKey=%d",ret,g_skyRdmaKey);
	if(ret == KAL_FALSE || g_skyRdmaKey == 0)
	{
		return ret;
	}
    g_skyMoutKey = 0;

    ret = idp_imgdma_mout_open(&g_skyMoutKey);
 	sk_dbg_print("RESZ_Mem_Open =%d, g_skyMoutKey=%d",ret,g_skyMoutKey);
	if(ret == KAL_FALSE || g_skyMoutKey == 0)
	{
		return ret;
	}
    g_skyVrzKey = 0;
    ret = idp_resz_vrz_open(&g_skyVrzKey);
 	sk_dbg_print("RESZ_Mem_Open =%d, g_skyVrzKey=%d",ret,g_skyVrzKey);
	if(ret == KAL_FALSE || g_skyVrzKey == 0)
	{
		return ret;
	}
    g_skyRgbrot0Key = 0;

    ret = idp_imgdma_rgbrot0_open(&g_skyRgbrot0Key);
 	sk_dbg_print("RESZ_Mem_Open =%d, g_skyRgbrot0Key=%d",ret,g_skyRgbrot0Key);
	if(ret == KAL_FALSE || g_skyRgbrot0Key == 0)
	{
		return ret;
	}

	return  ret;
}


kal_int32 RESZ_Mem_Stop(void)
{
	idp_resz_crz_stop(g_SkyResizerKey, &crz_struct);
    idp_imgproc_ipp_stop(g_skyIppKey,&ipp_struct);
    idp_imgdma_rdma_stop(g_skyRdmaKey,&rdma_struct);
    idp_imgdma_mout_stop(g_skyMoutKey,&mout_struct);
    idp_resz_vrz_stop(g_skyVrzKey,&vrz_struct);
    idp_imgdma_rgbrot0_stop(g_skyRgbrot0Key,&rgbrot0_struct);

  	return NO_ERROR;
}


kal_int32 RESZ_Mem_Close(void)
{
    sk_dbg_print("RESZ_Mem_Close start");

	if(g_SkyResizerKey != 0)
	{
	        sk_dbg_print("crz close +");
    		idp_resz_crz_close(g_SkyResizerKey, &crz_struct);
            sk_dbg_print("crz close -");
    		g_SkyResizerKey = 0;
	}
    if(g_skyIppKey != 0)
	{
	        sk_dbg_print("ipp close +");
            idp_imgproc_ipp_close(g_skyIppKey,&ipp_struct);
            sk_dbg_print("ipp close -");
    		g_skyIppKey = 0;
	}
    if(g_skyRdmaKey != 0)
	{
	        sk_dbg_print("rdma close +");
            idp_imgdma_rdma_close(g_skyRdmaKey,&rdma_struct);
            sk_dbg_print("rdma close -");
    		g_skyRdmaKey = 0;
	}
    if(g_skyMoutKey != 0)
	{
	        sk_dbg_print("mout close +");
            idp_imgdma_mout_close(g_skyMoutKey,&mout_struct);
            sk_dbg_print("mout close -");
    		g_skyMoutKey = 0;
	}
    if(g_skyVrzKey != 0)
	{
	        sk_dbg_print("vrz close +");
            idp_resz_vrz_close(g_skyVrzKey,&vrz_struct);
            sk_dbg_print("vrz close -");
    		g_skyVrzKey = 0;
	}
    if(g_skyRgbrot0Key != 0)
	{
	        sk_dbg_print("rgbrot close +");
            idp_imgdma_rgbrot0_close(g_skyRgbrot0Key,&rgbrot0_struct);
            sk_dbg_print("rgbrot close -");
    		g_skyRgbrot0Key = 0;
	}
sk_dbg_print("RESZ_Mem_Close end");
	g_SkyRszState = 0;
	return NO_ERROR;
}


kal_int32 RESZ_Mem_Status(void)
{
	kal_bool ret = 0;
	kal_bool busy = 0;
    ret = idp_imgproc_ipp_is_busy(g_skyIppKey,&busy,&ipp_struct);
    if (KAL_TRUE == (busy))
    {
        sk_dbg_print("staus  ipp  busy");
        return KAL_TRUE;
    }
    ret = idp_imgdma_rdma_is_busy(g_skyRdmaKey,&busy,&rdma_struct);
    if (KAL_TRUE == (busy))
    {
        sk_dbg_print("staus  rdma  busy");
        return KAL_TRUE;
    }
    ret = idp_imgdma_mout_is_busy(g_skyMoutKey,&busy,&mout_struct);
    if (KAL_TRUE == (busy))
    {
        sk_dbg_print("staus  mout  busy");
        return KAL_TRUE;
    }
    ret = idp_resz_vrz_is_busy(g_skyVrzKey,&busy,&vrz_struct);
    if (KAL_TRUE == (busy))
    {
        sk_dbg_print("staus  vrz  busy");
        return KAL_TRUE;
    }
    ret = idp_imgdma_rgbrot0_is_busy(g_skyRgbrot0Key,&busy,&rgbrot0_struct);
    if (KAL_TRUE == (busy))
    {
        sk_dbg_print("staus  rgbrot0  busy");
        return KAL_TRUE;
    }

	ret = idp_resz_crz_is_busy(g_SkyResizerKey,&busy, &crz_struct);
	sk_dbg_print("RESZ_Mem_Status =%d,ret=%d",busy,ret);
 	return busy;
}



void RESZ_Mem_SetYuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV)
{
}

#endif /* defined(DRV_IDP_OLD_DESIGN) && defined(DRV_IDP_6253_SERIES) */

#endif /* defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__) */
