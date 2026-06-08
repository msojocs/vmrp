
#if defined(__DSM_SPLAYER__) && defined(__MTK_TARGET__)

/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/
#include "SKY_MM_def.h"

#include "MMI_include.h"
#include "lcd_sw_rnd.h"
#include "MMI_features_video.h" /* features */


#include "gui.h"                /* Gui functions */
#include "gpioInc.h"            /* LED */
#include "SettingDefs.h"        /* popup sound */
#include "CommonScreens.h"      /* Popup */
#include "MainMenuDef.h"        /* Multi-media icon */
#include "Conversions.h"        /* char set conversion */
#include "PhoneSetupGprots.h"   /* PhnsetGetDefEncodingType() */
#include "CallManagementGprot.h"/* CM */
#ifndef MT6255
#include "SimDetectionGprot.h"  /* flight mode */
#endif
#include "med_api.h"            /* media task */
#include "med_main.h"           /* media task */

#include "lcd_sw_rnd.h"
#include "lcd_if.h"

#include "mdi_datatype.h"
#include "mdi_audio.h"          /* audio lib */
#include "mdi_video.h"          /* video lib */

#include "SettingProfile.h"     /* warning tone */
#include "FileManagerGProt.h"   /* file path / file error  */
#include "FileManagerDef.h"     /* error string id */
#include "FileMgr.h"
#include "USBDeviceGprot.h"     /* USB MS mode */
#include "DataAccountGprot.h"

#ifdef __VDOPLY_FEATURE_TOUCH_SCREEN__
#include "wgui_touch_screen.h"
#endif 

#include "resource_video_skins.h"       /* UI custom config */
#include "VdoRecResDef.h"
#include "VdoPlyResDef.h"
#include "VdoRecGProt.h"
#include "VdoPlyApp.h"
#include "VdoPlyGProt.h"
#ifndef MT6255
#include "ImageViewerGprot.h"
#endif

#include "mdi_include.h"

#include "med_api.h"
#include "med_main.h"
#include "Med_utility.h"

#include "sky_mm_SPlayer_api.h"
#include "mmiDsmDef.h"
#include "sky_mm_port.h"
#include "sky_mm_def.h"

extern gdi_handle splayer_get_gdi_handle(int id);
extern BOOL splayer_is_full_screen(void);

/***************************************************************************** 
* Define
*****************************************************************************/

#if defined(MDI_VIDEO_MT6238_SERIES) && defined(GDI_USING_2D_ENGINE_V3)
#define __MMI_VIDEO_PLY_USE_IDP_ROTATE__
#endif
#if defined(MT6236)
#include "lcd_if_6236_series_hw.h"
/*SET ROTATE*/
#define SET_LCD_LAYER0_ROTATE(n) SET_LCD_LAYER_ROTATE(0,n)
#define SET_LCD_LAYER1_ROTATE(n) SET_LCD_LAYER_ROTATE(1,n)
#define SET_LCD_LAYER2_ROTATE(n) SET_LCD_LAYER_ROTATE(2,n)
#define SET_LCD_LAYER3_ROTATE(n) SET_LCD_LAYER_ROTATE(3,n)
#define SET_LCD_LAYER0_WINDOW_SIZE(column,row) SET_LCD_LAYER_WINDOW_SIZE(0,column,row)
#define SET_LCD_LAYER0_WINDOW_OFFSET( x, y) SET_LCD_LAYER_WINDOW_OFFSET(0, x, y)
#define SET_LCD_LAYER1_WINDOW_SIZE(column,row) SET_LCD_LAYER_WINDOW_SIZE(1,column,row)
#define SET_LCD_LAYER1_WINDOW_OFFSET( x, y) SET_LCD_LAYER_WINDOW_OFFSET(1, x, y)
#define SET_LCD_LAYER2_WINDOW_SIZE(column,row) SET_LCD_LAYER_WINDOW_SIZE(2,column,row)
#define SET_LCD_LAYER2_WINDOW_OFFSET( x, y) SET_LCD_LAYER_WINDOW_OFFSET(2, x, y)
#define SET_LCD_LAYER3_WINDOW_SIZE(column,row) SET_LCD_LAYER_WINDOW_SIZE(3,column,row)
#define SET_LCD_LAYER3_WINDOW_OFFSET( x, y) SET_LCD_LAYER_WINDOW_OFFSET(3, x, y)
#endif
/***************************************************************************** 
* Local Variable
*****************************************************************************/

static volatile kal_uint32 display_lock = 0;
extern vdoply_context_struct g_vdoply_cntx;
extern vdoply_context_struct *const mmi_vdoply_cntx_p;


/***************************************************************************** 
* Local Function
*****************************************************************************/

extern void sk_dbg_print(char *fmt,...);

extern volatile unsigned int g_splayer_event;
extern volatile unsigned int g_splayer_result;
#ifdef SPLAYER_DUOBLE_BUFFER
void *gSPlayerContextBuffer = NULL;
int gSPlayerContextBuffer_len = 0;
#endif

int g_SplayerColorSpace = 0;
int Splayer_SetColorSpace(int colorspace)
{
	g_SplayerColorSpace = colorspace;
	return g_SplayerColorSpace;
}
int Splayer_GetColorSpace(void)
{

	#if defined(MT6236)
		return 8;
	#elif defined(__MDI_DISPLAY_YUYV422_SUPPORT__)
	       return 9;
	#elif defined(MT6253)
	       return 1;
	#else
	      return 0;
	#endif
}

void Splayer_play_start(void)
{
	//gdi_enable_non_block_blt();
#ifdef SPLAYER_DUOBLE_BUFFER
        if(gSPlayerContextBuffer)
           {
               MMFree(gSPlayerContextBuffer);
               gSPlayerContextBuffer = 0;
           }
        gSPlayerContextBuffer_len = gSPlayerContext.play_req_st.display_width * gSPlayerContext.play_req_st.display_height * 2;
        gSPlayerContextBuffer = (void *)MMMalloc(gSPlayerContextBuffer_len);
        if(!gSPlayerContextBuffer)
           {
               sk_dbg_print("[Splayer_play_start] gSPlayerContextBuffer alloc fail");
               gSPlayerContextBuffer_len = 0;
               return;
           }
#endif

}

void Splayer_play_stop(void)
{
	//gdi_disable_non_block_blt();
#ifdef SPLAYER_DUOBLE_BUFFER
    if(gSPlayerContextBuffer)
        {
            MMFree(gSPlayerContextBuffer);
            gSPlayerContextBuffer = NULL;
            gSPlayerContextBuffer_len = 0;
        }
#endif
}
extern char* szSkythumbnail;
void Splayer_play_close(void)
{
    if(szSkythumbnail)
    {
        MMFreeNoncache(szSkythumbnail);
        szSkythumbnail = NULL;
    }

}

/*****************************************************************************
 * FUNCTION
 *  mmi_vdoply_draw_play_region_bg
 * DESCRIPTION
 *  draw audio only animation
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void vid_stop_unfinished_audio(void)
{
}

BOOL mmi_gpio_is_lcd_sleep(void)
{
	return 0;//rwei
}
//char szSkythumbnail[100*100*2];// = 0;//[10];//
char* szSkythumbnail = NULL;//[100*100*2];// = 0;//[10];//

void sky_rgb565_2_rgb888(unsigned short *pSrc,unsigned char *pDst,unsigned short w,unsigned short h)
{
    int cnt = 0;
    unsigned short temp = 0;
    //cnt = w*h;
    while(cnt < w*h)
    {
        //temp = ((pSrc[cnt]&0xff) <<8)|((pSrc[cnt]&0xff00) >>8);
        temp = pSrc[cnt];
        pDst[cnt*3+0] = (unsigned char)((temp&0xf800)>>11)<<3;
        pDst[cnt*3+1] = (unsigned char)((temp&0x7e0)>>5)<<2;
        pDst[cnt*3+2] = (unsigned char)(temp&0x1f)<<3;
        cnt++;
    }
}
void mmi_vdoply_get_display_info(kal_uint32 fs, kal_uint16 * pWidth, kal_uint16 * pHeight, kal_uint32 * pLayerBuffer, kal_uint32 * pFormat)
{
	kal_uint8 imgFormat;
	static int g_skycount = 0;
	if(mmi_gpio_is_lcd_sleep() == 0) 
	{
#if 0
		int count = 0;
		while(display_lock != 0 && count < 10)
		{
			sk_dbg_print("[DISPLAY] LCD LOCKED, SLEEP!!!!\r\n");
			count ++;
			kal_sleep_task(1);
		}
		display_lock = 0;
#else
		if(display_lock != 0)
		{
			pWidth[0] = 0;
			pHeight[0] = 0;
			pLayerBuffer[0] = 0;
			g_skycount++;
			if(g_skycount >10)
				display_lock = 0;
			return;
		}
#endif
		g_skycount = 0;
		if(SPlayer_IsStatePlaying() == SPLAYER_STATE_PLAYING)
		{
			if(gSPlayerContext.play_req_st.image_buffer_p)
			{
				pWidth[0] = gSPlayerContext.play_req_st.display_width;
				pHeight[0] =gSPlayerContext.play_req_st.display_height;
                #ifdef SPLAYER_DUOBLE_BUFFER
                    if(gSPlayerContextBuffer)
                        {
                            pLayerBuffer[0] = (kal_uint32)gSPlayerContextBuffer;
                        }
                    else
                        {
                            pLayerBuffer[0] = (kal_uint32)gSPlayerContext.play_req_st.image_buffer_p;
                        }
                #else
				    pLayerBuffer[0] = (kal_uint32)gSPlayerContext.play_req_st.image_buffer_p;
                #endif
                imgFormat = gSPlayerContext.play_req_st.image_data_format;
			}
		}
		else if(SPlayer_IsStatePlaying() == SPLAYER_STATE_GETFRAME)
		{
			if(gSPlayerContext.getFrame_req_st.image_buffer_p)
			{
				pWidth[0] = gSPlayerContext.getFrame_req_st.display_width;
				pHeight[0] =gSPlayerContext.getFrame_req_st.display_height;
                if((pWidth[0] * pHeight[0] < 100*100)&&
                    (gSPlayerContext.getFrame_req_st.image_data_format == 3))
                    {
                        if(szSkythumbnail)
                        {
                            MMFreeNoncache(szSkythumbnail);
                            szSkythumbnail = NULL;
                        }
                        szSkythumbnail = (char *)MMMallocNoncache(pWidth[0] * pHeight[0]*2);
                        pLayerBuffer[0] = (kal_uint32)szSkythumbnail;
                        memset(szSkythumbnail,0,pWidth[0] * pHeight[0]*2);
                    }
                else
    				pLayerBuffer[0] = (kal_uint32)gSPlayerContext.getFrame_req_st.image_buffer_p;
				imgFormat = gSPlayerContext.getFrame_req_st.image_data_format;
			}
		}
		else 
		{
			if(gSPlayerContext.seek_req_st.image_buffer_p)
			{
				pWidth[0] = gSPlayerContext.seek_req_st.display_width;
				pHeight[0] = gSPlayerContext.seek_req_st.display_height;
				pLayerBuffer[0] = (kal_uint32)gSPlayerContext.seek_req_st.image_buffer_p;
				imgFormat = gSPlayerContext.seek_req_st.image_data_format;
			}
		}
	}
	
	
	if(pFormat != NULL)
	{
			*pFormat = Splayer_GetColorSpace();//0;//rgb565
	}
	
	if(imgFormat == IMG_COLOR_FMT_PACKET_YUYV422)
	{
		*pFormat = 9;
		mr_media_hwRsz_setYUV2RGB(0);
	}
	else
	{
		*pFormat = 0;
		mr_media_hwRsz_setYUV2RGB(1);
	}
       sk_dbg_print("pFormat2 =%d format %d state %d, w=%d,h=%d",*pFormat, imgFormat, SPlayer_IsStatePlaying(),pWidth[0],pHeight[0]);
}
 
 void dummy_RM()
{
	 display_lock = 0;
}

int g_skymmVideoType = 0;
void sky_mm_SetVideoType(int type)
{
	 g_skymmVideoType = type;
     sk_dbg_print("g_skymmVideoType =%d",g_skymmVideoType);
}
 
 int sky_mm_GetVideoType(void)
 {
 	#if defined(MT6236)||defined(MT6255)
	      return g_skymmVideoType;
	#else
		return 0;
	#endif
 }
extern int skymm_IsOSDExist(void );

#if defined(__SKYMM__DISPLAY_IN_MMI__)
typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8  state;
}SkyMM_Disp_msg_struct;
void mmi_vdoply_display_mmi(kal_uint32 full_screen);
 void SkyMMSendMsg(kal_uint32 full_screen)
{
	#if 1
	SkyMM_Disp_msg_struct *msg_p = NULL;
	ilm_struct *ilm_ptr = NULL;

	msg_p = (SkyMM_Disp_msg_struct*)construct_local_para(sizeof(SkyMM_Disp_msg_struct), TD_CTRL);

	msg_p->state = full_screen;

	ilm_ptr = allocate_ilm(MOD_MULTIMEDIA);
	ilm_ptr->src_mod_id = MOD_MULTIMEDIA;
	ilm_ptr->dest_mod_id    =  MOD_MMI;
	ilm_ptr->msg_id 	      =       MSG_ID_SPLAYER_DISPLAY;
	ilm_ptr->local_para_ptr  = (local_para_struct*)msg_p;
	ilm_ptr->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_ptr);
	sk_dbg_print("[   SkyMMSendMsg   ]");
	#endif
}


void SkyMMSetDisplayProtocol(void)
{
	SetProtocolEventHandler((PsIntFuncPtr)mmi_vdoply_display_mmi, MSG_ID_SPLAYER_DISPLAY);
}

void mmi_vdoply_display(kal_uint32 full_screen)
{
	if(SPlayer_IsStatePlaying() == SPLAYER_STATE_GETFRAME)
	{
		sk_dbg_print("[display] Dont show!!");
        if(((gSPlayerContext.getFrame_req_st.display_width*
            gSPlayerContext.getFrame_req_st.display_height )< (100*100))
            &&szSkythumbnail
            &&(gSPlayerContext.getFrame_req_st.image_data_format == 3)
            )
            
        {
            sky_rgb565_2_rgb888((unsigned short *)szSkythumbnail,
                (unsigned char  *)gSPlayerContext.getFrame_req_st.image_buffer_p,
                gSPlayerContext.getFrame_req_st.display_width,
                gSPlayerContext.getFrame_req_st.display_height);
        }
		return;
	}
	display_lock = 1;
	sk_dbg_print("[display] mmi_vdoply_display");
	SkyMMSetDisplayProtocol();
	SkyMMSendMsg(full_screen);
}
#endif

#if defined(__SKYMM__DISPLAY_IN_MMI__)
void mmi_vdoply_display_mmi(kal_uint32 full_screen)
#else
void mmi_vdoply_display_mmi(kal_uint32 full_screen)
{}
void mmi_vdoply_display(kal_uint32 full_screen)
#endif
{
#if 1
	gdi_handle baseLayer;
	kal_uint16 width, height;
	lcd_frame_update_struct lcd_data = {0};
	int SysCurTime = 0;
	int ret = 0;
	#ifdef SKY_MM_CALC_FPS
	SysCurTime = SPlayer_getSysTime();
	
	{
		static int g_count = 0;
	       static int SysStartTime = 0;

		   
		g_count++;
		if(SysStartTime==0)
		{
			SysStartTime = SPlayer_getSysTime();
			g_count = 0;
		}
		
		if(((SysCurTime - SysStartTime)>10000)||((SysCurTime - SysStartTime)<=0))
		{
			g_count = 0;
			SysStartTime = SPlayer_getSysTime();
		}
 		else
		{
 			if((SysCurTime - SysStartTime)>5000)
			{
				sk_dbg_print("[mmi_vdoply_display]display FPS =%d",g_count*10000/(SysCurTime - SysStartTime));
				SysStartTime = SPlayer_getSysTime();
				g_count = 0;
			}
		}
	}
	#endif
	
	sk_dbg_print("mmi_vdoply_display()");
	//display_lock = 0;
	
	if(SPlayer_IsStatePlaying() == SPLAYER_STATE_GETFRAME && gSPlayerContext.getFrame_req_st.image_buffer_p != gSPlayerContext.seek_req_st.image_buffer_p)
	{
		sk_dbg_print("[display] Dont show!!");
        if(((gSPlayerContext.getFrame_req_st.display_width*
            gSPlayerContext.getFrame_req_st.display_height )< (100*100))
            &&szSkythumbnail
            &&(gSPlayerContext.getFrame_req_st.image_data_format == 3)
            )
            
        {
            sky_rgb565_2_rgb888((unsigned short *)szSkythumbnail,
                (unsigned char  *)gSPlayerContext.getFrame_req_st.image_buffer_p,
                gSPlayerContext.getFrame_req_st.display_width,
                gSPlayerContext.getFrame_req_st.display_height);
        }
		display_lock = 0;//rwei
		return;
	}

	//gdi_enable_non_block_blt();
       // gdi_layer_blt_previous(0, 0, UI_device_width - 1, UI_device_height - 1);
		//return;
		//gdi_enable_non_block_blt();
    #ifdef SPLAYER_DUOBLE_BUFFER
        if(gSPlayerContextBuffer)
            MMMemCpy(gSPlayerContext.play_req_st.image_buffer_p,gSPlayerContextBuffer,gSPlayerContextBuffer_len);
    #endif
	if(1) //(!SPlayer_IsStatePlaying() || gSPlayerContext.play_req_st.force_lcd_hw_trigger || skymm_IsOSDExist())
	{
	#if !defined(__SKYMM__DISPLAY_IN_MMI__)
		if(GDI_LOCK_NO_WAIT == GDI_SUCCEED)
		{
			GDI_UNLOCK;
	#endif
            SKY_FroceRefresh();//rwei
		    #if 1//def __VDOPLY_FEATURE_HORIZONTAL_VIEW__        
		        ret = gdi_layer_blt_previous(0, 0, UI_device_height - 1, UI_device_width - 1);
		    #else
		        gdi_layer_blt_previous(0, 0, UI_device_width - 1, UI_device_height - 1);
		    #endif
			//kal_sleep_task(25);
	#if !defined(__SKYMM__DISPLAY_IN_MMI__)
		}
	#endif
    sk_dbg_print("mmi_vdoply_display() end");
		display_lock = 0;
		#ifdef SKY_MM_CALC_FPS
		sk_dbg_print("mmi_vdoply_display -blt-time-%d,ret =%d",SPlayer_getSysTime() - SysCurTime,ret);
		#endif
		return;
	}
#if 0
		/*
		width = gSPlayerContext.play_req_st.display_width; 
		height = gSPlayerContext.play_req_st.display_height; 
        gdi_layer_blt_previous((UI_device_width - width)/2, (UI_device_height - height)/2, (UI_device_width + width)/2, (UI_device_height + height)/2);
		return;
		*/
	
        sk_dbg_print("mmi_vdoply_display  update_layer=%x, hw_update_layer=%x---",gSPlayerContext.play_req_st.update_layer, gSPlayerContext.play_req_st.hw_update_layer);
#if !defined(__SKYMM__DISPLAY_IN_MMI__)
	GDI_LOCK;
#endif
	lcd_power_ctrl(KAL_TRUE);
    	if(splayer_is_full_screen())
        {
            //if( skymm_IsOSDExist() )
            {   
            //    lcd_data.update_layer = gSPlayerContext.play_req_st.update_layer;
            }
            //else
            {
                lcd_data.update_layer = gSPlayerContext.play_req_st.update_layer; //gSPlayerContext.play_req_st.hw_update_layer;
            }
        }   
        else
        {   
		width = gSPlayerContext.play_req_st.display_width; 
		height = gSPlayerContext.play_req_st.display_height; 

		if ((gSPlayerContext.play_req_st.hw_update_layer & LCD_LAYER0_ENABLE) != 0)
		{
	 		SET_LCD_LAYER0_WINDOW_SIZE(width,height);
	 		SET_LCD_LAYER0_WINDOW_OFFSET(0,0);
	 		//SET_LCD_LAYER0_ROTATE(gSPlayerContext.play_req_st.lcd_rotate);
	 		REG_LCD_LAYER0_BUFF_ADDR = (kal_uint32) gSPlayerContext.play_req_st.image_buffer_p;
			lcd_data.update_layer=LCD_LAYER0_ENABLE;
		}
		else if( (gSPlayerContext.play_req_st.hw_update_layer & LCD_LAYER1_ENABLE) != 0)
		{
	 		SET_LCD_LAYER1_WINDOW_SIZE(width,height);
	 		SET_LCD_LAYER1_WINDOW_OFFSET(0,0);
	 		//SET_LCD_LAYER1_ROTATE(gSPlayerContext.play_req_st.lcd_rotate);
	 		REG_LCD_LAYER1_BUFF_ADDR = (kal_uint32) gSPlayerContext.play_req_st.image_buffer_p;
			lcd_data.update_layer=LCD_LAYER1_ENABLE;
		}
		else if ((gSPlayerContext.play_req_st.hw_update_layer & LCD_LAYER2_ENABLE) != 0)
		{
		 	SET_LCD_LAYER2_WINDOW_SIZE(width,height);
	 		SET_LCD_LAYER2_WINDOW_OFFSET(0,0);
	 		//SET_LCD_LAYER2_ROTATE(gSPlayerContext.play_req_st.lcd_rotate);
	 		REG_LCD_LAYER2_BUFF_ADDR = (kal_uint32) gSPlayerContext.play_req_st.image_buffer_p;	
			lcd_data.update_layer=LCD_LAYER2_ENABLE;
		}
		else if ((gSPlayerContext.play_req_st.hw_update_layer & LCD_LAYER3_ENABLE) != 0)
		{
	 		SET_LCD_LAYER3_WINDOW_SIZE(width,height);
	 		SET_LCD_LAYER3_WINDOW_OFFSET(0,0);
	 		//SET_LCD_LAYER3_ROTATE(gSPlayerContext.play_req_st.lcd_rotate);
	 		REG_LCD_LAYER3_BUFF_ADDR = (kal_uint32) gSPlayerContext.play_req_st.image_buffer_p;
			lcd_data.update_layer=LCD_LAYER3_ENABLE;
		}

		/*
		if (gSPlayerContext.play_req_st.lcd_rotate == 0)
		{
			lcd_data.roi_offset_x = 0;
			lcd_data.roi_offset_y = 0;
		}
		else
		{
			lcd_data.roi_offset_x = gSPlayerContext.play_req_st.roi_offset_x;
			lcd_data.roi_offset_y = gSPlayerContext.play_req_st.roi_offset_y;
		}
		*/
        }
		
	lcd_data.lcm_start_x = (LCD_WIDTH - gSPlayerContext.play_req_st.display_height)/2;
	lcd_data.lcm_start_y = (LCD_HEIGHT - gSPlayerContext.play_req_st.display_width)/2;
	lcd_data.lcm_end_x = (LCD_WIDTH + gSPlayerContext.play_req_st.display_height)/2;
	lcd_data.lcm_end_y = (LCD_HEIGHT + gSPlayerContext.play_req_st.display_width)/2;
	lcd_data.rotate_value = gSPlayerContext.play_req_st.idp_rotate;
			lcd_data.roi_offset_x = 0;
			lcd_data.roi_offset_y = 0;

	lcd_data.hw_update_layer = gSPlayerContext.play_req_st.hw_update_layer;

    	lcd_data.module_id=LCD_UPDATE_MODULE_MMI;//LCD_UPDATE_MODULE_JPEG_VIDEO;
    	lcd_data.lcd_id=MAIN_LCD;
    	lcd_data.fb_update_mode=LCD_SW_TRIGGER_MODE;
    	lcd_data.block_mode_flag=KAL_FALSE;
    	lcd_data.lcd_block_mode_cb = dummy_RM;
    	lcd_data.memory_output=KAL_FALSE;

    	lcd_fb_update(&lcd_data); 
#if !defined(__SKYMM__DISPLAY_IN_MMI__)
    	GDI_UNLOCK;
#endif
	display_lock = 1;
	#ifdef SKY_MM_CALC_FPS
	sk_dbg_print("mmi_vdoply_display --time-%d",SPlayer_getSysTime() - SysCurTime);
	#endif
#endif
#else	
	sk_dbg_print("mmi_vdoply_display");

#endif
}


void SKYShowYUVFrame( unsigned char * pY,
                      unsigned char * pU,
                      unsigned char * pV,
                      unsigned int    uWidth,
                      unsigned int    uHeight,
                      unsigned int    uStride,
                      unsigned int    uStrideUV )
{
	kal_uint16 w, h;
	kal_uint32 format;
	kal_uint8 * pLayerBuffer = NULL;
	
	sk_dbg_print("SKYShowYUVFrame ");
	mmi_vdoply_get_display_info(0, &w, &h, (kal_uint32*)&pLayerBuffer,&format);
	if(pLayerBuffer && w > 0 && h > 0)
	{
		if(mr_media_hwRsz_init(w) == 0)
		{
		mr_media_hwRsz_image(pY, pU, pV, pLayerBuffer, uWidth, uHeight, w, h);
		mr_media_hwRsz_free();
		}
	}
}

int mmi_vdoply_get_layer_region(int full_screen, kal_uint16 *x, kal_uint16 *y, kal_uint16 *w, kal_uint16 *h)
{
		if (splayer_is_full_screen()) {
			gdi_layer_push_and_set_active(splayer_get_gdi_handle(0));
		} else {
			gdi_layer_push_and_set_active(splayer_get_gdi_handle(1));
		}
		
		*w = gdi_act_layer->width;
		*h = gdi_act_layer->height;
		*x = gdi_act_layer->offset_x;
		*y = gdi_act_layer->offset_y;
		gdi_layer_pop_and_restore_active();
		return 0;
}
#ifndef __MMI_DSM_NEW__
enum{
    STR_ID_SPLAYER_MORE,
    STR_ID_SPLAYER_MORE2,
    STR_ID_SPLAYER_MORE_KEY,
};
#endif

//extern void mmi_vdoply_drawEndStr(unsigned int x, unsigned int y, unsigned int w, unsigned int h, int isTouch);
void mmi_vdoply_drawEndStr(unsigned int x, unsigned int y, unsigned int w, unsigned int h, int isTouch)
{
	int i;
	S32 strH, strW, strW2;
	UI_string_type show_text = NULL;
	UI_string_type show_text2 = NULL;
	S32 len;
	unsigned char txt3[8]={0xbe,0x7c,0x69,0x5f,0x01,0xff,0x00,0x00};
	color textColor ={0xff,0xff,0xff,100};

#if !defined(__MMI_TOUCH_SCREEN__)
	show_text = (UI_string_type)GetString(STR_ID_SPLAYER_MORE_KEY);
#else
	show_text = (UI_string_type)GetString(STR_ID_SPLAYER_MORE);
#endif
	show_text2 = (UI_string_type)GetString(STR_ID_SPLAYER_MORE2);
	
	if(!show_text)
		show_text = (UI_string_type)txt3;

       gui_measure_string((UI_string_type) show_text, &strW, &strH);
       gui_measure_string((UI_string_type) show_text2, &strW2, &strH);
	gui_set_text_color(textColor);

	strW = strW > strW2 ? strW : strW2;
	if(x + strW > w)
	{
		x = (w > strW) ? w-strW : 0;
	}

	y -= strH+3;
	gui_move_text_cursor(x,y);
	gui_print_text((UI_string_type)show_text);
	gui_move_text_cursor(x,y+strH+2);
	gui_print_text((UI_string_type)show_text2);

}

void mmi_vdoply_draw_finished(void)
{
	short *layerBuf=NULL, *layerBuf2=NULL;
	short *tmpBuf, *temp2;
	S32 orgW, orgH, newW, newH;
	S32 margeW, margeH;
	S32 i, j, x,y;
	gdi_handle layerHdlr = splayer_get_gdi_handle(1);
	#if 0 //def __MDI_DISPLAY_YUYV422_SUPPORT__ 
		gdi_color_format cf;
	#endif
 
	
	gdi_layer_push_and_set_active(layerHdlr);
	#if 0 //def __MDI_DISPLAY_YUYV422_SUPPORT__ 
	     gdi_layer_get_color_format(&cf);
		 sk_dbg_print("SPLAYER draw finish Layer color %d", cf);
		 if(cf != GDI_COLOR_FORMAT_16)
		 {
            gdi_layer_set_color_format(GDI_COLOR_FORMAT_16);
            mdi_util_convert_color_format(
                layerHdlr, cf, GDI_COLOR_FORMAT_16);
		 }
	#endif
	gdi_layer_get_buffer_ptr((U8 **)&layerBuf);
	gdi_layer_get_dimension((S32 *)&orgW, (S32 *)&orgH);
	gdi_layer_get_position((S32 *)&x, (S32 *)&y);

	gdi_layer_push_clip();
	gdi_layer_push_text_clip();

	newW = orgW >> 1;
	newH = orgH >> 1;
	margeH = orgH >> 2;
	margeW = 14;

	tmpBuf = (short*)med_alloc_ext_mem(newW * newH * sizeof(short));
	if(tmpBuf)
	{
		for(i=0;i<newH;i++)
		{
			for(j=0;j<newW;j++)
			{
				*(tmpBuf+newW*i+j) = *(layerBuf+orgW*i*2+j*2);
			}
		}
		
		temp2 = tmpBuf;
		layerBuf2 = layerBuf + orgW*margeH + margeW;
		memset(layerBuf, 0, orgW*orgH*sizeof(short));	
		for(i=0;i<newH;i++)
		{
			memcpy(layerBuf2, temp2, newW*sizeof(short));
			layerBuf2 += orgW;
			temp2 += newW;
		}
		med_free_ext_mem((void**)&tmpBuf);
	}
else
{
	memset(layerBuf, 0, orgW*orgH*sizeof(short));	
}
	x = newW + margeW*2;
	y = orgH/2;
	
	
	//gui_set_font(&MMI_medium_font);
#if !defined(__MMI_TOUCH_SCREEN__)
	mmi_vdoply_drawEndStr(x, y, orgW, orgH, 0);
#else
	mmi_vdoply_drawEndStr(x, y, orgW, orgH, 1);
#endif

	gdi_layer_pop_text_clip();
	gdi_layer_pop_clip();
	#if 0 //def __MDI_DISPLAY_YUYV422_SUPPORT__ 
		 if(cf != GDI_COLOR_FORMAT_16)
		 {
            gdi_layer_set_color_format(cf);
            mdi_util_convert_color_format(
                layerHdlr, GDI_COLOR_FORMAT_16, cf);
		 }
	#endif
	
	gdi_layer_pop_and_restore_active();	
	
}
int SPLAYER_Get_MaxWidth()
{
#if (defined(MT6235)||defined(MT6235B)||defined(MT6236)||defined(MT6255))
	return 1280;
#else
	return 352;
#endif
}
int SPLAYER_Get_MaxHeight()
{
#if (defined(MT6235)||defined(MT6235B)||defined(MT6236)||defined(MT6255))
	return 720;
#else
	return 288;
#endif
}

#include "custom_config.h"

void mr_media_VideoDec_send_decode_req(void* pData, kal_uint32 dataSize)
{
}

void mr_media_VideoDec_send_decode_cnf(kal_int32 result)
{
}



#define SKY_MM_SPLAYER_WAIT_EVENT( evt ) \
	do { \
		do { sk_dbg_print("*********** WATTING EVENT %d ***********\r\n", evt); kal_sleep_task(10); } while(g_splayer_event != evt); \
		if (g_splayer_result != SPLAYER_OKAY) { \
			sk_dbg_print("[RMVB] ********* RMVB FAIL %d **********\r\n", g_splayer_result); \
		} \
	} while(0) \
	
#define SKY_MM_SPLAYER_SET_EVENT( evt ) \
	do { \
		sk_dbg_print("[RMVB] ******** EVENT %d SETTING ********\r\n", evt); \
		g_splayer_event = evt; \
	} while(0) \


void SPLAYER_WAIT_EVENT(int evt)
{
	SKY_MM_SPLAYER_WAIT_EVENT(evt);
}
void SPLAYER_SET_EVENT(int evt)
{
	SKY_MM_SPLAYER_SET_EVENT( evt );
}


#ifdef __ARM9_MMU__
#define SPLYAER_MAX_RESET_AUDIO_FRAME    50
#else
#define SPLYAER_MAX_RESET_AUDIO_FRAME    50
#endif

#if (defined(MT6235)||defined(MT6235B)||defined(MT6236)||defined(MT6255))
#define  SPLAYER_AUD_BUF_LEN (8*1024) //500ms - 1k 
#define  SPLAYER_WAV_BUF_LEN (60*1024) //500ms - 1k 
#else
#define  SPLAYER_AUD_BUF_LEN (8*1024) //500ms - 1k 
#define  SPLAYER_WAV_BUF_LEN (60*1024) //500ms - 1k 
#endif

int splayerGetMaxResetAudioFrame(void )
{
	return SPLYAER_MAX_RESET_AUDIO_FRAME;
}
int splayerGetAudBufLen(void )
{
	return SPLAYER_AUD_BUF_LEN;//*SPlayer_get_vid_audio_channel_no();
}
int splayerGetWavBufLen(void )
{
	return SPLAYER_WAV_BUF_LEN;
}
int Splayer_IsMediaplayer(void)
{
	
	#if defined( __MMI_VIDEO_PLAYER__)
		return 0;
	#else
		return 1;
	#endif
}
#endif
