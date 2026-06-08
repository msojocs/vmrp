
#if (defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__)) && defined(__MTK_TARGET__)
#include "SKY_MM_def.h"

#include "reg_base.h"
#include "Drv_comm.h"

#if (defined(DRV_IDP_OLD_DESIGN) && defined(DRV_IDP_6253_SERIES))||defined(MT6236)||defined(MT6236B) || defined(MT6252) //|| defined(MT6255)

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
#include "SKY_MM_resizer_mem.h"


static kal_uint32 g_mrMediaYuv2Rgb;
int mr_media_hwRsz_init(kal_uint32 width)
{
	kal_uint32  readBytes;

	sk_dbg_print("mr_media_hwRsz_init w %d", width);
    //return -1;
	//g_mrMediaYuv2Rgb = 0;
	RESZ_Mem_Init();
	RESZ_Mem_Open(width);
	return 0;
}

void mr_media_hwRsz_image(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, int srcwidth, int srcheight,int dstwidth,int dstheight)
{
	RESZ_MEM_CFG_STRUCT rez_cfg = {0};
	kal_uint32 SysCurTime = SPlayer_getSysTime();
	kal_uint32 i = 0;
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
	//#if (MTK_VERSION <= 0x10A1032)
	//gdi_waiting_blt_finish();
	//#endif
	RESZ_Mem_Start();
#if defined(DRV_IDP_6253_SERIES)
	while(!(RESZ_Mem_Status() & RESZ_MEM_STATUS_MEM1O))
#elif (defined(MT6236) || defined(MT6255))//Allan 
	while(RESZ_Mem_Status() && i<30)
#endif
	{
		//sk_dbg_print("VM_LOG:RESZ_Mem_Complete  %s,%d, %d", __FILE__, __LINE__, mr_getTime());
		//mr_sleep(1);
		sk_dbg_print("RESZ_Mem_Status wait i=%d",i);
				i++;
        kal_sleep_task(1);
	}	
sk_dbg_print("mr_media_hwRsz_image --time-%d",SPlayer_getSysTime() - SysCurTime);
}


void mr_media_hwRsz_imageEX(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, 
									int srcwidth, int srcheight,
									int dstwidth,int dstheight,
									int pitch, int pitch_uv)
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
		sk_dbg_print("mr_media_hwRsz_imageEX srcwidth %d dstwidth %d, pY %x", srcwidth, dstwidth, pY);
		return;
	}
	
	sk_dbg_print("mr_media_hwRsz_imageEX yuv2rgb %d", g_mrMediaYuv2Rgb);
	SysCurTime = SPlayer_getSysTime();

{
	pYUV_noncache = (char *)MMMallocNoncache(srcwidth*srcheight*3>>1);
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
	sk_dbg_print("mr_media_hwRsz_image copy --time-%d",SPlayer_getSysTime() - SysCurTime);
	SysCurTime = SPlayer_getSysTime();
	
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
	//#if (MTK_VERSION <= 0x10A1032)
	//gdi_waiting_blt_finish();
	//#endif
    i=0;
	RESZ_Mem_Start();
#if defined(DRV_IDP_6253_SERIES)
	while(!(RESZ_Mem_Status() & RESZ_MEM_STATUS_MEM1O))
#elif (defined(MT6236) || defined(MT6255))
	while(RESZ_Mem_Status()   && i<30)
#endif
	{
		sk_dbg_print("RESZ_Mem_Status wait");

        i++;
		kal_sleep_task(1);
	}	
	if(pYUV_noncache)
	{
		MMFreeNoncache(pYUV_noncache);
	}
sk_dbg_print("mr_media_hwRsz_image --time-%d",SPlayer_getSysTime() - SysCurTime);
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
	SysCurTime = SPlayer_getSysTime();

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
	sk_dbg_print("mr_media_hwRsz_image copy --time-%d",SPlayer_getSysTime() - SysCurTime);
	SysCurTime = SPlayer_getSysTime();
	
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
	
	gdi_waiting_blt_finish();
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

	sk_dbg_print("mr_media_hwRsz_image --time-%d",SPlayer_getSysTime() - SysCurTime);
}
void mr_media_hwRsz_free(void)
{
	sk_dbg_print("mr_media_hwRsz_free");
	RESZ_Mem_Close();
}

void mr_media_hwRsz_set_yuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV)
{
#if ((!defined(MT6236))&&(!defined(MT6252))&&(!defined(MT6255)))
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
	sk_dbg_print("mr_media_hwRsz_init FAIL");
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

#endif /* defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__) */

