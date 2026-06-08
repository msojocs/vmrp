#if defined(__DSM_MEDIA_EXT_H264_ENCODE__) && defined(__MTK_TARGET__)

 /**************************************************************************/
#include "MMI_include.h"

#include "kal_release.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "task_main_func.h"
#include "app_ltlcom.h"
#include "lcd_ip_cqueue.h"
#include "stack_types.h"
#include "task_config.h"
#include "syscomp_config.h"
#include "custom_config.h"
#include "custom_util.h"

#include "av_bt.h"

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

#include "rtc_sw.h"
#include "app_mine.h"           /* mine type */
#include "custom_data_account.h"


#include "med_utility.h"
#include "mrporting.h"

#include "CameraApp.h"
#include "mdi_camera.h"

#include "mrp_mpchat.h"





/* ÉèÖÃÉãÏñÍ·Ðý×ª */
//#define MPCHAT_CAMERA_ROTATE_90
//#define MPCHAT_CAMERA_ROTATE_270
//#define MPCHAT_CAMERA_ROTATE_180

#define SKY_FAST_STACK_SIZE	(2800)

gdi_handle gCameraLayerPreview = GDI_LAYER_EMPTY_HANDLE;
extern uint8 * pYUV2RGBOutputStreamA;
extern uint8 gMrCamExtRunning;

static uint32 gCamWidth, gCamHeight;

#ifndef SetProtocolEventHandler
#define SetProtocolEventHandler(func, event)   \
 mmi_frm_set_protocol_event_handler(event, (PsIntFuncPtr)func, MMI_FALSE)
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////


extern  void mr_media_YUV_GetNotify(uint8* pData, int32 nDataSize);
static void mr_media_cam_YUVData_ind(void* msg)
{
	media_cam_mr_data_cnf_struct *local_data = (media_cam_mr_data_cnf_struct*) msg;
	mr_media_YUV_GetNotify(local_data->pData, local_data->nDataSize);
}

void mr_media_cam_init_callback(void)
{
	SetProtocolEventHandler(mr_media_cam_YUVData_ind, MSG_ID_MEDIA_CAM_MPCHAT_DATARSP_IND);	
}

// Ë«cameraÇÐ»»
int32 mr_media_cam_get_num(void)
{
#ifdef DUAL_CAMERA_SUPPORT 
	return 2;
#else
	return 1;
#endif
}

#ifdef __MMI_CAMCORDER__
#include "camcorderSetting.h"
#endif
uint32 gMrMedaiCamIdx = 1;
void mr_media_cam_Active(void)
{
#ifdef DUAL_CAMERA_SUPPORT 
	if(gMrMedaiCamIdx == 0)
	{
//#ifdef __MMI_CAMCORDER__
           mdi_camera_set_camera_id(MDI_CAMERA_MAIN);
//#endif
	}
	else if(gMrMedaiCamIdx==1)
	{
//#ifdef __MMI_CAMCORDER__
             mdi_camera_set_camera_id(MDI_CAMERA_SUB);
//#endif
	}
#endif
}

int32 mr_media_cam_CamsSwitch(uint8 set)
{
#ifdef DUAL_CAMERA_SUPPORT 
	if(gMrMedaiCamIdx == 0)
	{
		gMrMedaiCamIdx=1;
	}
	else if(gMrMedaiCamIdx==1)
	{
		gMrMedaiCamIdx=0;
	}
	mr_media_cam_Active();
	kal_sleep_task(3);
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}

// Ò¹¾°Ä£Ê½ÇÐ»»
static uint16 sMrMediaCamNightStat = 0;
int32 mr_media_cam_nightProf_switch(uint8* input, int32 input_len)
{
#if defined(__CAMERA_DSC_MODE_NIGHT__) ||defined(__CAMERA_FEATURE_NIGHT__)
	if(*input == 1 || *input == 2) // ÉèÖÃ
	{
		if(*input == 2)
		{
			sMrMediaCamNightStat = 0;
		}
		else {
			sMrMediaCamNightStat = 1;
		}
	       mdi_camera_update_para_night(sMrMediaCamNightStat);
	}
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}




int32 mr_media_cam_get_dataType(void)
{
#if defined(MT6238) || defined(MT6239) || defined(MT6236) 
	return 1;
#else
	return 0;
#endif
}


void mr_media_init_enc_callback(void)
{
	extern void mr_media_cam_Send_DataReq(void);
	SetProtocolEventHandler(mr_media_cam_Send_DataReq, MSG_ID_MPCHAT_ENCODE_CNF);	
}

extern int VC_H264Encoder_Encode(void * pHandle,  uint8 * p1, uint8 * p2,
 								uint8 * p3, uint32 * p4, uint8 * p5, uint32 * p6, uint32 p7);
void mr_media_H264Enc_fastCall(void * pHandle,  uint8 * p1, uint8 * p2,
 								uint8 * p3, uint32 * p4, uint8 * p5, uint32 * p6, uint32 p7)
{
#ifndef  __ARM9_MMU__
		uint32 stack_begin;
//		dbg_print("alloc cache stack size = %d\r\n", MED_INT_STACK_MEM_SIZE);

		stack_begin = (U32)drv_gfx_stkmgr_get_stack_internal(SKY_FAST_STACK_SIZE);
		if(stack_begin)
		{
			sk_dbg_print("alloc cache stack succeeded!\r\n");
			INT_SwitchStackToRun((void*)stack_begin, SKY_FAST_STACK_SIZE,//rwei
									(kal_func_ptr)VC_H264Encoder_Encode,8,
									pHandle,  p1, p2, p3,
									p4, p5, p6, p7);
			
			drv_gfx_stkmgr_release_stack_internal((void*)stack_begin);
		}
		else
#endif
		{
			sk_dbg_print("alloc cache stack failed!\r\n");
			VC_H264Encoder_Encode(pHandle,  p1, p2, p3,
									p4, p5, p6, p7);
		}
	return;
}

void mr_media_Send_Enocde_REQ(void* buffer, uint32 len, uint8 state)
{
	mr_media_enc_data_ind_struct *msg_p = NULL;
	ilm_struct *ilm_ptr = NULL;

	msg_p = (mr_media_enc_data_ind_struct*)construct_local_para(sizeof(mr_media_enc_data_ind_struct), TD_CTRL);

	msg_p->pData        = (uint8*)buffer;
	msg_p->nDataSize  = len;
	msg_p->nState = state;

	ilm_ptr = allocate_ilm(MOD_MMI);
	ilm_ptr->src_mod_id = MOD_MMI;
	ilm_ptr->dest_mod_id    =  MOD_MED;
	ilm_ptr->msg_id 	      =       MSG_ID_MPCHAT_ENCODE_REQ;
	ilm_ptr->local_para_ptr  = (local_para_struct*)msg_p;
	ilm_ptr->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_ptr);
}

void mr_media_Send_Enocde_CNF(uint8 result)
{
	mr_media_enc_data_cnf_struct *msg_p = NULL;
	ilm_struct *ilm_ptr = NULL;

	msg_p = (mr_media_enc_data_cnf_struct*)construct_local_para(sizeof(mr_media_enc_data_cnf_struct), TD_CTRL);

	msg_p->result = result;

	ilm_ptr = allocate_ilm(MOD_MED);
	ilm_ptr->src_mod_id = MOD_MED;
	ilm_ptr->dest_mod_id    =  MOD_MMI;
	ilm_ptr->msg_id 	      =       MSG_ID_MPCHAT_ENCODE_CNF;
	ilm_ptr->local_para_ptr  = (local_para_struct*)msg_p;
	ilm_ptr->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_ptr);
}

void mr_media_Encode_req_hdlr(void* pMsg)
{
	mr_media_enc_data_ind_struct *msg_p = (mr_media_enc_data_ind_struct*)pMsg;
	VMMediaEncCamData encParam;
       sk_dbg_print("mr_media_Encode_req_hdlr");
	encParam.pData = msg_p->pData;
	encParam.nDataSize = msg_p->nDataSize;
	encParam.nSrc = msg_p->nSrc;
	encParam.nState = msg_p->nState;
	MPchat_Encode_req_handler((void *)&encParam);
}



/////////////////////////////////////////////////////

void YUV422ToYUV420(unsigned char * pYUV422, unsigned char * pY, unsigned char * pU, unsigned char * pV, int width, int height)
{
	int i, j;
	for (j = 0; j < height; j+=2) {
		for (i = 0; i < width; i+=2) {
			unsigned int uyvy = *(unsigned int*)(pYUV422 + (i<<1));
			unsigned int uyvy1 = *(unsigned int*)(pYUV422 + ((width+i)<<1));
			pU[i>>1] = uyvy;
			pV[i>>1] = uyvy>>16;
			*(unsigned short*)(pY+i) = ((uyvy&0xFF00)>>8) | ((uyvy>>24)<<8);
			*(unsigned short*)(pY+width+i) = ((uyvy1&0xFF00)>>8) | ((uyvy1>>24)<<8);
		}
		pU += width>>1;
		pV += width>>1;
		pY += width<<1;
		pYUV422 += width<<2;
	}
}


void mr_media_cam_YUVRotation ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight )
{
#if 1//defined(MT6252)
{
	YUV422ToYUV420(pYUV2RGBOutputStreamA, pSrc, pSrc+uWidth*uHeight, pSrc+uWidth*uHeight*5/4, uWidth, uHeight);
}
#endif
	#if defined(MPCHAT_CAMERA_ROTATE_90)
		mr_media_YUVRotation_90(pSrc, pDst, uHeight, uWidth);
	#elif defined(MPCHAT_CAMERA_ROTATE_270)
		mr_media_YUVRotation_270(pSrc, pDst, uHeight, uWidth);
	#elif defined(MPCHAT_CAMERA_ROTATE_180)
		mr_media_YUVRotation_180(pSrc, pDst, uWidth, uHeight);
	#else
		mr_media_YUVRotation_0(pSrc, pDst, uWidth, uHeight);
	#endif
}

void mr_media_cam_RGBRotation ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight )
{
	#if defined(MPCHAT_CAMERA_ROTATE_90)
		mr_media_RGBRotation_90(pSrc, pDst, uHeight, uWidth);
	#elif defined(MPCHAT_CAMERA_ROTATE_270)
		mr_media_RGBRotation_270(pSrc, pDst, uHeight, uWidth);
	#elif defined(MPCHAT_CAMERA_ROTATE_180)
		mr_media_RGBRotation_180(pSrc, pDst, uHeight, uWidth);
	#else
		mr_media_RGBRotation_0(pSrc, pDst, uHeight, uWidth);
	#endif
}

static int32 mr_media_cam_create_preview_resource(uint32 width, uint32 height)
{
	uint8 *buf_ptr;
	int32 buf_size;

	sk_dbg_print("mr_media_cam_create_preview_resource");

	buf_size = ((width*height*GDI_MAINLCD_BIT_PER_PIXEL)>>3);
	
	buf_ptr = (uint8 *)VCMalloc(buf_size);
	
	if(!buf_ptr)
		return -1;

	gdi_layer_create_using_outside_memory(	0, 0, width, height,
											&gCameraLayerPreview,
											(PU8)buf_ptr, buf_size);	

	gdi_layer_push_and_set_active(gCameraLayerPreview);
	gdi_layer_set_background(GDI_COLOR_TRANSPARENT);
	gdi_layer_pop_and_restore_active();
	pYUV2RGBOutputStreamA = buf_ptr;
	
	return 0;
}

static void mr_media_cam_free_preview_resource(void)
{
	sk_dbg_print(MOD_MMI,"mr_media_cam_free_preview_resource");

	if(gCameraLayerPreview == GDI_LAYER_EMPTY_HANDLE)
		return;
	
	gdi_layer_free(gCameraLayerPreview);
	gCameraLayerPreview = GDI_LAYER_EMPTY_HANDLE;
	if(pYUV2RGBOutputStreamA)
	{
		
		VCFree(pYUV2RGBOutputStreamA);
		pYUV2RGBOutputStreamA = NULL;
	}
}

static void mr_media_cam_entry_preview(uint32 width, uint32 height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mdi_camera_setting_struct camera_setting_data = {0};
    mdi_camera_preview_struct camera_preview_data = {0};
    GDI_HANDLE preview_layer_handle = 0;
    U32 preview_layer_flag = 0;
    U32 blt_layer_flag = 0;
    S32 preview_wnd_offset_x = 0;
    S32 preview_wnd_offset_y = 0;
    U16 preview_wnd_width = 0;
	int ret = 0;
    U16 preview_wnd_height = 0;
    #if defined(__MMI_CAMERA__) && !defined(__MMI_CAMCORDER__)
	extern camera_context_struct g_camera_cntx;
	#endif
	extern U8 camera_wb_command_map[];
	extern U8 camera_ev_command_map[];
	extern U8 camera_banding_command_map[];
	extern U8 camera_flash_command_map[];
	extern U8 camera_iso_command_map[];
	extern U8 camera_ae_meter_command_map[];
	extern U8 camera_autofocus_mode_command_map[];
	extern U8 camera_autofocus_command_map[];
	extern U8 camera_dsc_mode_command_map[];
	extern U8 camera_effect_command_map[];
	extern U8 camera_image_qty_command_map[];
    sk_dbg_print( "----call mdi_camera_preview_start----111");
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* preview parametes */
#if (defined(__MMI_CAMERA__) && !defined(__MMI_CAMCORDER__))&&(MTK_VERSION <= 0x11A1108)
    camera_setting_data.zoom = g_camera_feature_zoom[0];
    camera_setting_data.wb = camera_wb_command_map[g_camera_cntx.setting.wb];//MDI_CAMERA_WB_AUTO;
    //camera_setting_data.manual_wb = FALSE;
    camera_setting_data.ev = camera_ev_command_map[g_camera_cntx.setting.ev];
    camera_setting_data.banding = camera_banding_command_map[g_camera_cntx.setting.banding];
    camera_setting_data.flash = camera_flash_command_map[g_camera_cntx.setting.flash];
    //camera_setting_data.osd_rotate = LCD_LAYER_ROTATE_NORMAL;// g_camera_cntx.osd_rotate;
    //camera_setting_data.night = g_camera_cntx.setting.night;
    
    camera_setting_data.iso = camera_iso_command_map[g_camera_cntx.setting.iso];
    camera_setting_data.ae_meter = camera_ae_meter_command_map[g_camera_cntx.setting.ae_meter];
    //camera_setting_data.dsc_mode = camera_dsc_mode_command_map[g_camera_cntx.setting.dsc_mode];
    camera_setting_data.af_mode = camera_autofocus_mode_command_map[g_camera_cntx.setting.af_mode];
    camera_setting_data.af_metering_mode = MDI_CAMERA_AUTOFOCUS_1_ZONE; //camera_autofocus_command_map[g_camera_cntx.setting.af_meter];
    camera_setting_data.shutter_pri = 0;
    camera_setting_data.aperture_pri = 0;
    
    camera_setting_data.xenon_flash_status_callback = NULL;
    
    /* default effect */
    camera_setting_data.effect = camera_effect_command_map[g_camera_cntx.setting.effect];
    camera_setting_data.brightness = 128;
    camera_setting_data.saturation = 128;
    camera_setting_data.contrast = 128;
    camera_setting_data.hue = 0;
    
    camera_setting_data.overlay_frame_mode = FALSE;
    camera_setting_data.overlay_frame_depth = 0;
    camera_setting_data.overlay_frame_width = 0;
    camera_setting_data.overlay_frame_height = 0;
    camera_setting_data.overlay_frame_source_key = 0;
    camera_setting_data.overlay_frame_buffer_address = 0;
    
#if defined(MPCHAT_CAMERA_ROTATE_90) || defined(MPCHAT_CAMERA_ROTATE_270)
    camera_setting_data.preview_width = height;  
    camera_setting_data.preview_height =width;
#else
    camera_setting_data.preview_width = width; 
    camera_setting_data.preview_height = height;
#endif
    
    /* capture parameters */
    camera_setting_data.image_qty = camera_image_qty_command_map[g_camera_cntx.setting.image_qty];
#if defined(MPCHAT_CAMERA_ROTATE_90) || defined(MPCHAT_CAMERA_ROTATE_270)
    camera_setting_data.image_width = height; 
    camera_setting_data.image_height =width;
#else
    camera_setting_data.image_width = width; 
    camera_setting_data.image_height = height; 
#endif
    
    camera_setting_data.lcm = MDI_CAMERA_PREVIEW_LCM_MAINLCD;
    camera_setting_data.preview_rotate = CAMERA_PREVIEW_MAINLCD_ROTATE;
    camera_setting_data.lcm_rotate = CAMERA_LCM_MAINLCD_ROTATE;
	#if !(defined(MT6235)||defined(MT6235B))
    	camera_setting_data.continue_capture = FALSE; //g_camera_cntx.is_continue_capture;
		#ifdef __MDI_CAMERA_FD_SUPPORT__
		camera_setting_data.fd_enable = FALSE;
		#endif
	#else
	camera_setting_data.continue_capture = FALSE; //g_camera_cntx.is_continue_capture;
	#endif
#else
    camera_setting_data.zoom = 0;
    camera_setting_data.wb = MDI_CAMERA_WB_AUTO;//camera_wb_command_map[g_camera_cntx.setting.wb];
    //camera_setting_data.manual_wb = FALSE;
    camera_setting_data.ev = MDI_CAMERA_EV_0;//camera_ev_command_map[g_camera_cntx.setting.ev];
    camera_setting_data.banding = MDI_CAMERA_BANDING_50HZ;//camera_banding_command_map[g_camera_cntx.setting.banding];
    camera_setting_data.flash = MDI_CAMERA_FLASH_OFF;//camera_flash_command_map[g_camera_cntx.setting.flash];
    //camera_setting_data.osd_rotate = LCD_LAYER_ROTATE_NORMAL;// g_camera_cntx.osd_rotate;
    //camera_setting_data.night = 0;//g_camera_cntx.setting.night;
    
    camera_setting_data.iso = MDI_CAMERA_ISO_AUTO;//camera_iso_command_map[g_camera_cntx.setting.iso];
    camera_setting_data.ae_meter = MDI_CAMERA_AE_METER_AUTO;//camera_ae_meter_command_map[g_camera_cntx.setting.ae_meter];
    //camera_setting_data.dsc_mode = MDI_CAMERA_DSC_MODE_AUTO;//camera_dsc_mode_command_map[g_camera_cntx.setting.dsc_mode];
    //camera_setting_data.af_mode = 0;//MDI_CAMERA_AUTOFOCUS_MODE_AUTO;//camera_autofocus_mode_command_map[g_camera_cntx.setting.af_mode];
    //camera_setting_data.af_metering_mode = CAM_AF_SINGLE_ZONE;//MDI_CAMERA_AUTOFOCUS_1_ZONE; //camera_autofocus_command_map[g_camera_cntx.setting.af_meter];
    //camera_setting_data.shutter_pri = 0;
    //camera_setting_data.aperture_pri = 0;
    
    camera_setting_data.xenon_flash_status_callback = NULL;
    
    /* default effect */
    camera_setting_data.effect = MDI_CAMERA_EFFECT_NOMRAL;//camera_effect_command_map[g_camera_cntx.setting.effect];
    //camera_setting_data.brightness = 128;
    camera_setting_data.saturation = 128;
    camera_setting_data.contrast = 128;
    camera_setting_data.hue = 0;
    
    camera_setting_data.overlay_frame_mode = FALSE;
    camera_setting_data.overlay_frame_depth = 0;
    camera_setting_data.overlay_frame_width = 0;
    camera_setting_data.overlay_frame_height = 0;
    camera_setting_data.overlay_frame_source_key = 0;
    camera_setting_data.overlay_frame_buffer_address = 0;
    
#if defined(MPCHAT_CAMERA_ROTATE_90) || defined(MPCHAT_CAMERA_ROTATE_270)
    camera_setting_data.preview_width = height;
    camera_setting_data.preview_height = width;
#else
    camera_setting_data.preview_width = width;
    camera_setting_data.preview_height = height;
#endif
    
    /* capture parameters */
    camera_setting_data.image_qty = MDI_CAMERA_JPG_QTY_NORMAL;//camera_image_qty_command_map[g_camera_cntx.setting.image_qty];
#if defined(MPCHAT_CAMERA_ROTATE_90) || defined(MPCHAT_CAMERA_ROTATE_270)
    camera_setting_data.image_width = height;
    camera_setting_data.image_height = width;
#else
    camera_setting_data.image_width = width;
    camera_setting_data.image_height = height; 
#endif
    
    camera_setting_data.lcm = MDI_CAMERA_PREVIEW_LCM_MAINLCD;
    //camera_setting_data.preview_rotate = 0;//CAMERA_PREVIEW_MAINLCD_ROTATE;
    //camera_setting_data.lcm_rotate = 0;//CAMERA_LCM_MAINLCD_ROTATE;
	#if !(defined(MT6235)||defined(MT6235B))
    	//camera_setting_data.continue_capture = FALSE; //g_camera_cntx.is_continue_capture;
		#ifdef __MDI_CAMERA_FD_SUPPORT__
		camera_setting_data.fd_enable = FALSE;
		#endif
	#else
	camera_setting_data.continue_capture = 0;//g_camera_cntx.is_continue_capture;
	#endif
#endif
    /* 
    * if using MT6218B series moudle with a Master backend (AIT700, etc),
    * we have to clear the preview layer's background to transparent,
    * and we have to resoret it after preview stop (stopped by capture command)
    */
    {
        
        preview_layer_handle = gCameraLayerPreview;
        
        /* use preview wnd's position */
        preview_wnd_offset_x = 0;
        preview_wnd_offset_y = 0;
#if defined(MPCHAT_CAMERA_ROTATE_90) || defined(MPCHAT_CAMERA_ROTATE_270)
        preview_wnd_width =height ; 
        preview_wnd_height = width;
#else
        preview_wnd_width = height; 
        preview_wnd_height = width;
#endif
        
        /* preview (hw), OSD */
}

	/* preview data for preview start */
	camera_preview_data.preview_layer_handle = preview_layer_handle;
	//camera_preview_data.preview_wnd_offset_x = preview_wnd_offset_x;
	//camera_preview_data.preview_wnd_offset_y = preview_wnd_offset_y;
	//camera_preview_data.preview_wnd_width = preview_wnd_width;
	//camera_preview_data.preview_wnd_height = preview_wnd_height;
	camera_preview_data.blt_layer_flag = GDI_LAYER_ENABLE_LAYER_0|GDI_LAYER_ENABLE_LAYER_1;
	camera_preview_data.preview_layer_flag = GDI_LAYER_ENABLE_LAYER_1;
	camera_preview_data.is_lcd_update = FALSE;
	camera_preview_data.src_key_color = GDI_COLOR_TRANSPARENT;
	camera_preview_data.is_tvout = FALSE;
	


/*@ Add by Vincent */
#if defined(__CAMERA_OSD_HORIZONTAL__) && defined(__MDI_CAMERA_HW_ROTATOR_SUPPORT__)
mdi_camera_set_hw_rotator(MDI_CAMERA_PREVIEW_ROTATE_90);
#endif

	sk_dbg_print( "----call mdi_camera_preview_start----");
#if (MTK_VERSION <= 0x11A1108)
	ret = mdi_camera_preview_start( &camera_preview_data,&camera_setting_data);
#else
	ret = mdi_camera_preview_start( &camera_preview_data,&camera_setting_data, NULL);
#endif
	sk_dbg_print( "----call mdi_camera_preview_start---end-ret=%d",ret);

	gCamWidth = width;
	gCamHeight = height;
	
	{
		uint8* pYUV[3] = {0};
		mr_media_cam_get_yuvBuf(pYUV);
		mr_media_hwRsz_set_yuvOut(pYUV[0], pYUV[1], pYUV[2]);
	}
}

int32 mr_media_cam_preview_start(uint32 width, uint32 height)
{
	sk_dbg_print( "^^^mr_media_cam_preview_start^^^");

 #if defined(__MMI_CAMERA__) && !defined(__MMI_CAMCORDER__)
       mmi_camera_load_setting();
 #else
	//mmi_camco_setting_load_from_nvram();
#endif

	mr_media_cam_Active();
	mr_media_cam_free_preview_resource();
	if(mr_media_cam_create_preview_resource(width, height) != 0)
		return MR_FAILED;

	#if (MTK_VERSION <= 0x11A1108)
	mdi_camera_power_on();
	#else
	mdi_camera_power_on(FRAMEWORK_ASM);
	#endif
	mr_media_cam_entry_preview(width, height);

	return MR_SUCCESS;
}

int32 mr_media_cam_preview_stop(void)
{
	mdi_camera_preview_stop();
	mdi_camera_power_off();
	mr_sleep(50);
	mr_media_cam_free_preview_resource();
	return MR_SUCCESS;
}

int32 mr_media_cam_GetData(void)
{
	mr_media_cam_send_data_req(MOD_MMI);

	return MR_SUCCESS;
}

int32 mr_media_cam_DataRsp(uint8* pData, uint32 nDataSize)
{
	mr_media_cam_send_data_rsp(pData, nDataSize);
    return MR_SUCCESS;
}
#endif
