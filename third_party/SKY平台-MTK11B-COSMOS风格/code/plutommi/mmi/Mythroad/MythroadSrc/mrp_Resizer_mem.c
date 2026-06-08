
#ifdef __MMI_DSM_NEW__
/* ---- SKY_MULTIMEDIA start ---- */
#ifndef __DSM_SPLAYER__
/* ---- SKY_MULTIMEDIA end ---- */

#include "reg_base.h"
#include "Drv_comm.h"
#include "mrp_features.h"

static unsigned int Sk_getSysTime(void)
{
	kal_uint32 time = 0;
	kal_uint32 return_t = 0;
	///kal_get_time(&time);
	//return_t = (time* 0.004615 + 0.5)*1000;
    	//return_t = kal_ticks_to_milli_secs(time);
	return return_t;
}

#if (defined(DRV_IDP_OLD_DESIGN) && defined(DRV_IDP_6253_SERIES))||defined(MT6236)||defined(MT6236B) || defined(MT6252)

#include "intrCtrl.h"
#include "visual_comm.h"
#include "drv_features.h"
#include "drvpdn.h"   
#include "isp_if.h"   
#include "visualhisr.h"
#include "lcd_if.h"
#include "mrp_Resizer_mem_6253_series.h"
#include "mrp_Resizer_mem.h"
#include "med_utility.h"


static kal_uint32 g_mrMediaYuv2Rgb;

int mr_media_hwRsz_init(kal_uint32 width)
{
	kal_uint32  readBytes;

	sk_dbg_print("mr_media_hwRsz_init w %d", width);
	//g_mrMediaYuv2Rgb = 0;
	RESZ_Mem_Init();
	RESZ_Mem_Open(width);
	return 0;
}

void mr_media_hwRsz_image(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, int srcwidth, int srcheight,int dstwidth,int dstheight)
{
	RESZ_MEM_CFG_STRUCT rez_cfg = {0};
	kal_uint32 SysCurTime = Sk_getSysTime();
	
	if(srcwidth == 0 || srcheight == 0 || dstwidth == 0 || dstheight == 0 || pY == NULL)
	{
		sk_dbg_print("mr_media_hwRsz_image srcwidth %d dstwidth %d, pY %x", srcwidth, dstwidth, pY);
		return;
	}
	
	sk_dbg_print("mr_media_hwRsz_image yuv2rgb %d", g_mrMediaYuv2Rgb);
	rez_cfg.yuv_to_rgb = g_mrMediaYuv2Rgb;
	rez_cfg.rgb_tar_base1 = (kal_uint32)pBGR;
	rez_cfg.rgb_tar_base2 = (kal_uint32)pBGR;
	rez_cfg.source_width  = srcwidth;
	rez_cfg.source_height = srcheight;
	rez_cfg.target_width  = dstwidth;
	rez_cfg.target_height = dstheight;
	rez_cfg.y_src_base = (kal_uint32)pY;//(kal_uint32)(yuv);
	rez_cfg.u_src_base = (kal_uint32)pU;//(kal_uint32)(yuv + width*height);
	rez_cfg.v_src_base = (kal_uint32)pV;//(kal_uint32)(yuv + width*height*5/4);
	RESZ_Mem_Config(&rez_cfg);
	#if (MTK_VERSION <= 0x10A1032)
	//gdi_waiting_blt_finish();
	#endif
	RESZ_Mem_Start();
#if defined(DRV_IDP_6253_SERIES)
	while(!(RESZ_Mem_Status() & RESZ_MEM_STATUS_MEM1O))
#elif defined(MT6236)
	while(RESZ_Mem_Status() )
#endif
	{
		//sk_dbg_print("VM_LOG:RESZ_Mem_Complete  %s,%d, %d", __FILE__, __LINE__, mr_getTime());
		//mr_sleep(1);
		kal_sleep_task(1);
	}	
sk_dbg_print("mr_media_hwRsz_image --time-%d",Sk_getSysTime() - SysCurTime);
}

/* ---- SKY_MULTIMEDIA start --for MT6252 RESZ-- */
#if defined(MT6252) //for 52 hw resz
#ifdef __DSM_SPLAYER__
extern void Resizer_And_Colorspace( unsigned short * pLayer, // LCD layer buffer
								 unsigned char	* pY,	// image Y
								 unsigned char	* pU,	// image U
								 unsigned char	* pV,	// image V
								 int layer_width,		// LCD layer width
								 int layer_height,		// LCD layer height
								 int layer_format,		// LCD layer format
								 int width, 			// image width
								 int height,			// image height
								 int chmaWidth,
								 int chmaHeight,
								 int pitch, 			// image L buffer stride
								 int pitch_uv );		  // image C buffer stride
#endif//__DSM_SPLAYER__
static unsigned int g_uTmpChroma[160];

static void sky_image_chroma_copy(unsigned char * src, unsigned char * dst, int width, int height, int stride)
{
	unsigned int i, j, T0, T1, T2, T3,Mask = 0x00FF00FF, AvgMask = 0xFEFEFEFE;
	unsigned char * tmp = (unsigned char*)(&g_uTmpChroma[0]);

	width  &= (~7);
	height &= (~1);

	for (i = 0; i < width; i+=8) {
		T0 = *(unsigned int*)(src+0);
		T1 = *(unsigned int*)(src+4);

		T2 = (T0>>8)|(T1<<24);
		T2 = (T0|T2) - (((T0^T2)&AvgMask)>>1);

		T1 = (T0&Mask) | ((T2&Mask)<<8);
		T0 = ((T0>>8)&Mask) | (T2&(Mask<<8));

		T2 = ((T1<<16)>>16) | (T0<<16);
		T0 = (T1>>16) | ((T0>>16)<<16);

		*(unsigned int*)(tmp+0) = T2;
		*(unsigned int*)(tmp+4) = T0;
		*(unsigned int*)(dst+0) = T2;
		*(unsigned int*)(dst+4) = T0;

		tmp += 8;
		dst += 8;
		src += 4;
	}
	src += stride-(width>>1);
	tmp -= width;

	for (j = 0; j < height; j+=2) {

		for (i = 0; i < width; i+=8) {
			T0 = *(unsigned int*)(src+0);
			T1 = *(unsigned int*)(src+4);

			T2 = (T0>>8)|(T1<<24);
			T2 = (T0|T2) - (((T0^T2)&AvgMask)>>1);

			T1 = (T0&Mask) | ((T2&Mask)<<8);
			T0 = ((T0>>8)&Mask) | (T2&(Mask<<8));

			T2 = ((T1<<16)>>16) | (T0<<16);
			T0 = (T1>>16) | ((T0>>16)<<16);

			T3 = *(unsigned int*)(tmp+0);
			T1 = *(unsigned int*)(tmp+4);
			
			*(unsigned int*)(tmp+0) = T2;
			*(unsigned int*)(tmp+4) = T0;
			*(unsigned int*)(dst+width+0) = T2;
			*(unsigned int*)(dst+width+4) = T0;

			T2 = (T2|T3) - (((T2^T3)&AvgMask)>>1);
			T0 = (T0|T1) - (((T0^T1)&AvgMask)>>1);
						
			*(unsigned int*)(dst+0) = T2;
			*(unsigned int*)(dst+4) = T0;
			
			tmp += 8;
			dst += 8;
			src += 4;
		}

		src += stride-(width>>1);
		dst += width;
		tmp -= width;
	}
}
void mr_media_hwRsz_imageEx_52_Noncache(kal_uint8 * pY, 
														kal_uint8 * pU,
														kal_uint8 * pV,
														unsigned char* pBGR,
														int srcwidth,
														int srcheight,
														int dstwidth,
														int dstheight,
										                unsigned int uStride,
                      									unsigned int uStrideUV)
{
	RESZ_MEM_CFG_STRUCT rez_cfg = {0};
	kal_uint16 h;

	kal_uint8 * pYUVNoncache = med_alloc_ext_mem(srcwidth*(srcheight+1)*3/2);

	srcwidth  &= (~7);
	srcheight &= (~3);
	
	//sk_dbg_print("mr_media_hwRsz_imageEx_52_Noncache ");

	if (!pYUVNoncache) {
		sk_dbg_print("NO MORE MEMORY TO ALLOCATE\n");
		#ifdef __DSM_SPLAYER__
		Resizer_And_Colorspace((unsigned short * )pBGR, // LCD layer buffer
								(unsigned char	* )pY,	// image Y
								(unsigned char	* )pU,	// image U
								(unsigned char	* )pV,	// image V
									 dstwidth,//srcwidth,		// LCD layer width
									 dstheight,//srcheight,		// LCD layer height
									 9,		// LCD layer format
									 srcwidth,//dstwidth, 			// image width
									 srcheight,//dstheight,			// image height
									 ((uStrideUV == (uStride>>2)) ? (srcwidth >>2) : (srcwidth >>1)),
									 ((uStrideUV == (uStride>>2)) ? (srcheight>>2) : (srcheight>>1)),
									 uStride, 			// image L buffer stride
									 uStrideUV);	  // image C buffer stride
		#endif//__DSM_SPLAYER__
		return;
	}
{
	kal_uint8 * pYSrc = pYUVNoncache;
	kal_uint8 * pUSrc = pYUVNoncache + srcwidth*srcheight;
	kal_uint8 * pVSrc = pYUVNoncache + srcwidth*srcheight*5/4;
	// Y
	for (h = 0; h < srcheight; h++) {
		memcpy(pYSrc + h*srcwidth, pY + h*uStride, srcwidth);
	}

	if (uStrideUV == (uStride>>2)) {
		sky_image_chroma_copy(pU, pUSrc, srcwidth>>1, srcheight>>1, uStrideUV);
		sky_image_chroma_copy(pV, pVSrc, srcwidth>>1, srcheight>>1, uStrideUV);       
	} 
	else
	{
		// U
		for (h = 0; h < srcheight/2; h++) {
			//DMA_memcpy(pU + h*uStrideUV, pUSrc + h*srcwidth/2, srcwidth/2);
			memcpy(pUSrc + h*srcwidth/2, pU + h*uStrideUV, srcwidth/2);
		}
		// V
		for (h = 0; h < srcheight/2; h++) {
			//DMA_memcpy(pV + h*uStrideUV, pVSrc + h*srcwidth/2, srcwidth/2);
			memcpy(pVSrc + h*srcwidth/2, pV + h*uStrideUV, srcwidth/2);
		}
	}

	rez_cfg.yuv_to_rgb = KAL_TRUE;
	rez_cfg.rgb_tar_base1 = (kal_uint32)pBGR;
	rez_cfg.rgb_tar_base2 = (kal_uint32)pBGR;
	rez_cfg.source_width  = srcwidth;
	rez_cfg.source_height = srcheight;
	rez_cfg.target_width  = dstwidth;
	rez_cfg.target_height = dstheight;

	rez_cfg.y_src_base = (kal_uint32)pYSrc;//(kal_uint32)(yuv);
	rez_cfg.u_src_base = (kal_uint32)pUSrc;//(kal_uint32)(yuv + width*height);
	rez_cfg.v_src_base = (kal_uint32)pVSrc;//(kal_uint32)(yuv + width*height*5/4);


	RESZ_Mem_Config(&rez_cfg);

#if (MTK_VERSION >= 0x11B0000)&&(MTK_VERSION <= 0x11BFFFF)
{
    unsigned int i = 0;
	gdi_waiting_blt_finish();
	RESZ_Mem_Start();
	while(RESZ_Mem_Status() && i<30)
	{
		sk_dbg_print("Wait HW resizer complete i=%d", i);
		i++;
		kal_sleep_task(1);
	}
}
#endif
	if(pYUVNoncache){
		med_free_ext_mem(&pYUVNoncache);
		pYUVNoncache = NULL;
	}
}
}/*52Ó²Ëõ·Ånoncacheº¯Êý*/
#endif//for MT6252 RESZ
/* ---- SKY_MULTIMEDIA end --for MT6252 RESZ-- */

void mr_media_hwRsz_imageEX(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, 
									int srcwidth, int srcheight,
									int dstwidth,int dstheight,
									int pitch, int pitch_uv)
{
#ifdef MT6252
 mr_media_hwRsz_imageEx_52_Noncache(pY,pU,pV,pBGR,srcwidth,
 								srcheight,dstwidth,dstheight,
 								pitch,pitch_uv);
#else
	RESZ_MEM_CFG_STRUCT rez_cfg = {0};
	int SysCurTime = 0;
	int    i;
	char * pY_noncache = 0;
	char * pU_noncache = 0;
	char * pV_noncache = 0;
	char * pYUV_noncache = 0;
	

	if(srcwidth == 0 || srcheight == 0 || dstwidth == 0 || dstheight == 0 || pY == NULL)
	{
		sk_dbg_print("mr_media_hwRsz_imageEX srcwidth %d dstwidth %d, pY %x", srcwidth, dstwidth, pY);
		return;
	}
	
	sk_dbg_print("mr_media_hwRsz_imageEX yuv2rgb %d", g_mrMediaYuv2Rgb);
	SysCurTime = Sk_getSysTime();

{
	pYUV_noncache = (char *)VCMallocNoncache(srcwidth*srcheight*3>>1);
	if(!pYUV_noncache)
	{
		sk_dbg_print("mr_media_hwRsz_imageEX  FAIL !!!!!");
		return ;
	}
	
	pY_noncache = pYUV_noncache;
	pU_noncache = pY_noncache + (srcwidth*srcheight);
	pV_noncache = pU_noncache + (srcwidth*srcheight>>2);

	srcheight &= ~1;

	for (i = 0; i < srcheight; i+=2) {
		memcpy(pY_noncache, pY, srcwidth);
		memcpy(pY_noncache+srcwidth, pY+pitch, srcwidth);
		pY += pitch<<1;
		pY_noncache += srcwidth<<1;

		memcpy(pU_noncache, pU, srcwidth>>1);
		memcpy(pV_noncache, pV, srcwidth>>1);
		pU += pitch_uv;
		pV += pitch_uv;
		pU_noncache += srcwidth>>1;
		pV_noncache += srcwidth>>1;
	}

	/* Restore */
	pY_noncache -= i*srcwidth;
	//memset(pY_noncache,0,srcwidth*srcheight);
	pU_noncache -= i*srcwidth>>2;
	pV_noncache -= i*srcwidth>>2;

	/* RESIZER & YUV TO RGB565 & OVERLAY */

}
	sk_dbg_print("mr_media_hwRsz_image copy --time-%d",Sk_getSysTime() - SysCurTime);
	SysCurTime = Sk_getSysTime();
	
	rez_cfg.yuv_to_rgb = g_mrMediaYuv2Rgb;
	rez_cfg.rgb_tar_base1 = (kal_uint32)pBGR;
	rez_cfg.rgb_tar_base2 = (kal_uint32)pBGR;
	rez_cfg.source_width  = srcwidth;
	rez_cfg.source_height = srcheight;
	rez_cfg.target_width  = dstwidth;
	rez_cfg.target_height = dstheight;
	rez_cfg.y_src_base = (kal_uint32)pY_noncache;//(kal_uint32)(yuv);
	rez_cfg.u_src_base = (kal_uint32)pU_noncache;//(kal_uint32)(yuv + width*height);
	rez_cfg.v_src_base = (kal_uint32)pV_noncache;//(kal_uint32)(yuv + width*height*5/4);
	RESZ_Mem_Config(&rez_cfg);
	#if (MTK_VERSION <= 0x10A1032)
	//gdi_waiting_blt_finish();
	#endif
	RESZ_Mem_Start();
#if defined(DRV_IDP_6253_SERIES)
	while(!(RESZ_Mem_Status() & RESZ_MEM_STATUS_MEM1O))
#elif defined(MT6236)
	while(RESZ_Mem_Status() )
#endif
	{
		//sk_dbg_print("VM_LOG:RESZ_Mem_Complete  %s,%d, %d", __FILE__, __LINE__, mr_getTime());
		kal_sleep_task(1);
	}	
	if(pYUV_noncache)
	{
		VCFreeNoncache(pYUV_noncache);
	}
sk_dbg_print("mr_media_hwRsz_image --time-%d",Sk_getSysTime() - SysCurTime);
#endif
}


void mr_media_hwRsz_imageEX_Noncache(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, 
									int srcwidth, int srcheight,
									int dstwidth,int dstheight,
									int pitch, int pitch_uv,
									kal_uint8 * pNoncache)
{
	RESZ_MEM_CFG_STRUCT rez_cfg = {0};
	int SysCurTime = 0;
	int    i;
	char * pY_noncache = 0;
	char * pU_noncache = 0;
	char * pV_noncache = 0;
	char * pYUV_noncache = 0;
	
	if(srcwidth == 0 || srcheight == 0 || dstwidth == 0 || dstheight == 0 || pY == NULL)
	{
		sk_dbg_print("mr_media_hwRsz_imageEX_Noncache srcwidth %d dstwidth %d, pY %x", srcwidth, dstwidth, pY);
		return;
	}
	
	sk_dbg_print("mr_media_hwRsz_imageEX_Noncache yuv2rgb %d", g_mrMediaYuv2Rgb);
	SysCurTime = Sk_getSysTime();

{
	pYUV_noncache = pNoncache;
	if(!pYUV_noncache)
	{
		sk_dbg_print("mr_media_hwRsz_imageEX  FAIL !!!!!");
		return ;
	}
	
	pY_noncache = pYUV_noncache;
	pU_noncache = pY_noncache + (srcwidth*srcheight);
	pV_noncache = pU_noncache + (srcwidth*srcheight>>2);

	srcheight &= ~1;

	for (i = 0; i < srcheight; i+=2) {
		memcpy(pY_noncache, pY, srcwidth);
		memcpy(pY_noncache+srcwidth, pY+pitch, srcwidth);
		pY += pitch<<1;
		pY_noncache += srcwidth<<1;

		memcpy(pU_noncache, pU, srcwidth>>1);
		memcpy(pV_noncache, pV, srcwidth>>1);
		pU += pitch_uv;
		pV += pitch_uv;
		pU_noncache += srcwidth>>1;
		pV_noncache += srcwidth>>1;
	}

	/* Restore */
	pY_noncache -= i*srcwidth;
	//memset(pY_noncache,0,srcwidth*srcheight);
	pU_noncache -= i*srcwidth>>2;
	pV_noncache -= i*srcwidth>>2;

	/* RESIZER & YUV TO RGB565 & OVERLAY */

//	MMFree_noncache(pY_noncache);
}
	sk_dbg_print("mr_media_hwRsz_image copy --time-%d",Sk_getSysTime() - SysCurTime);
	SysCurTime = Sk_getSysTime();
	
	rez_cfg.yuv_to_rgb = g_mrMediaYuv2Rgb;
	rez_cfg.rgb_tar_base1 = (kal_uint32)pBGR;
	rez_cfg.rgb_tar_base2 = (kal_uint32)pBGR;
	rez_cfg.source_width  = srcwidth;
	rez_cfg.source_height = srcheight;
	rez_cfg.target_width  = dstwidth;
	rez_cfg.target_height = dstheight;
	rez_cfg.y_src_base = (kal_uint32)pY_noncache;//(kal_uint32)(yuv);
	rez_cfg.u_src_base = (kal_uint32)pU_noncache;//(kal_uint32)(yuv + width*height);
	rez_cfg.v_src_base = (kal_uint32)pV_noncache;//(kal_uint32)(yuv + width*height*5/4);
	RESZ_Mem_Config(&rez_cfg);
	#if (MTK_VERSION <= 0x10A1032)
	//gdi_waiting_blt_finish();
	#endif
	RESZ_Mem_Start();
#if defined(DRV_IDP_6253_SERIES)
	while(!(RESZ_Mem_Status() & RESZ_MEM_STATUS_MEM1O))
#elif defined(MT6236)
	while(RESZ_Mem_Status() )
#endif
	{
		sk_dbg_print("VM_LOG:RESZ_Mem_Complete  %s,%d, %d", __FILE__, __LINE__, mr_getTime());
		//mr_sleep(1);
		kal_sleep_task(1);
	}	

	sk_dbg_print("mr_media_hwRsz_image --time-%d",Sk_getSysTime() - SysCurTime);
}

void mr_media_hwRsz_free(void)
{
	sk_dbg_print("mr_media_hwRsz_free");
	RESZ_Mem_Close();
}

void mr_media_hwRsz_set_yuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV)
{
#if ((!defined(MT6236))&&(!defined(MT6252)))
		RESZ_Mem_SetYuvOut(pY, pU, pV);
#endif
}

void mr_media_hwRsz_setYUV2RGB(kal_uint32 flag)
{
	g_mrMediaYuv2Rgb = flag;
}

#else /* defined(DRV_IDP_OLD_DESIGN) && defined(DRV_IDP_6253_SERIES) */
int mr_media_hwRsz_init(kal_uint32 width)
{
	return -1;
}

void mr_media_hwRsz_image(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, int srcwidth, int srcheight,int dstwidth,int dstheight)
{

}
void mr_media_hwRsz_imageEX(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, 
									int srcwidth, int srcheight,
									int dstwidth,int dstheight,
									int pitch, int pitch_uv)
{
}
void mr_media_hwRsz_imageEX_Noncache(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, 
									int srcwidth, int srcheight,
									int dstwidth,int dstheight,
									int pitch, int pitch_uv,
									kal_uint8 * pNoncache)
{
}

void mr_media_hwRsz_free(void)
{
}

void mr_media_hwRsz_set_yuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV)
{
}

void mr_media_hwRsz_setYUV2RGB(kal_uint32 flag)
{
}
#endif /* defined(DRV_IDP_OLD_DESIGN) && defined(DRV_IDP_6253_SERIES) */
/* ---- SKY_MULTIMEDIA start ---- */
#endif
/* ---- SKY_MULTIMEDIA end ---- */
#endif /* defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__) */

