
 /**************************************************************************/
 #include "SKY_MM_def.h"
 
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

#include "rtc_sw.h"
#include "app_mine.h"           /* mine type */
#include "custom_data_account.h"


#include "med_utility.h"
#include "mrporting.h"

#include "sky_mm_VC_BaseTypes.h"
#include "sky_mm_platform_api.h"

#include "CameraApp.h"
#include "mdi_camera.h"
#include "sky_mm_resizer_mem_6253_series.h"
#include "Mpchat_api.h"
#include "sky_mm_port.h"

#ifdef MT6236
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


#ifdef SKY_MM_USE_APP_MEM
#define med_alloc_ext_mem(size) applib_asm_alloc_anonymous_nc(size)
#define med_alloc_ext_mem_cacheable(size) applib_asm_alloc_anonymous(size)
#define med_free_ext_mem(ptr) applib_asm_free_anonymous(*ptr)
#endif

#if defined(__MMI_DSM_MEDIA_EXT__) && defined(__MTK_TARGET__)

/* 设置摄像头旋转 */
//#define MPCHAT_CAMERA_ROTATE_90
//#define MPCHAT_CAMERA_ROTATE_270
//#define MPCHAT_CAMERA_ROTATE_180

#define SKY_FAST_STACK_SIZE	(2800)

gdi_handle gCameraLayerPreview = GDI_LAYER_EMPTY_HANDLE;
extern VCU8 * pYUV2RGBOutputStreamA;
extern VCU8 gMrCamExtRunning;

static VCU32 gCamWidth, gCamHeight;

#ifndef SetProtocolEventHandler
#define SetProtocolEventHandler(func, event)   \
 mmi_frm_set_protocol_event_handler(event, (PsIntFuncPtr)func, MMI_FALSE)
#endif


VCVoid* VCMalloc (VCI32 iSize)
{
	MMInt32 *pRet = (MMInt32*)med_alloc_ext_mem((unsigned int)(iSize+4));
	
	if(!pRet)
	{
		sk_dbg_print("#####Malloc Memory Failed!!!");
		return MMNULL;
	}
	MMMemSet(pRet,0,iSize+4);
	return pRet;
}

VCVoid* VCIMalloc (VCI32 iSize)
{
	VCU32 * pIMem;
	iSize  = (iSize+3)>>2;
	pIMem = med_alloc_ext_mem_cacheable(iSize<<2);
	if(!pIMem)
	{
		sk_dbg_print("#####Malloc INT Memory Failed!!!");
		return MMNULL;
	}

	return pIMem;
}

/* FREE */
VCVoid VCFree (VCVoid * pMem)
{
	if(!pMem)
		return;
	med_free_ext_mem((void **)&pMem);
}

VCVoid VCIFree (VCVoid * pMem)
{ 
	if(!pMem)
		return;
	med_free_ext_mem((void **)&pMem);
}

/* MEMSET */
VCVoid* VCMemset (VCVoid * pMem, VCI32 iData, VCI32 iSize)
{
	return memset(pMem, iData, iSize);
}

/* MEMCPY */
VCVoid* VCMemcpy (VCVoid * pDst, VCVoid * pSrc, VCI32 iSize)
{
	return memcpy(pDst, pSrc, iSize);
}

/* MEMMOVE */
VCVoid* VCMemmove (VCVoid * pDst, VCVoid * pSrc, VCI32 iSize)
{
	return memmove(pDst, pSrc, iSize);
}

VCVoid* VCAssert(VCI32 flag)
{
	ASSERT(flag);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


extern  VCVoid mr_media_YUV_GetNotify(VCU8* pData, VCI32 nDataSize);
static VCVoid mr_media_cam_YUVData_ind(VCVoid* msg)
{
	media_cam_mr_data_cnf_struct *local_data = (media_cam_mr_data_cnf_struct*) msg;
	mr_media_YUV_GetNotify(local_data->pData, local_data->nDataSize);
}

VCVoid mr_media_cam_init_callback(VCVoid)
{
	SetProtocolEventHandler(mr_media_cam_YUVData_ind, MSG_ID_MEDIA_CAM_MPCHAT_DATARSP_IND);	
}

// 双camera切换
VCI32 mr_media_cam_get_num(VCVoid)
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
VCU32 gMrMedaiCamIdx = 1;
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

VCI32 mr_media_cam_CamsSwitch(VCU8 set)
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

// 夜景模式切换
static VCU16 sMrMediaCamNightStat = 0;
VCI32 mr_media_cam_nightProf_switch(VCU8* input, VCI32 input_len)
{
#if defined(__CAMERA_DSC_MODE_NIGHT__) ||defined(__CAMERA_FEATURE_NIGHT__)
	if(*input == 1 || *input == 2) // 设置
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


char mr_media_get_root_drive(VCVoid)
{
	return FS_GetDrive(FS_DRIVE_V_REMOVABLE, 1, FS_NO_ALT_DRIVE);
}

VCI32 mr_media_get_screen_info(VCVoid)
{
	VCI32 lScreenInfo = 0;
	
	#if !defined(__MMI_SCREEN_ROTATE__)
	lScreenInfo |= MPC_SCREEN_NOT_SUPPORT_ROTATE;
	#endif
	return lScreenInfo;
}

VCI32 mr_media_get_keyboard_info(VCVoid)
{
	VCI32 lKeyInfo = 0;
	
	#if 0/*没有数字键*/
	lKeyInfo |= MPC_KEYBOARD_NO_NUM_KEY;
	#endif
	
	#if 0/*没有OK键*/
	lKeyInfo |= MPC_KEYBOARD_NO_OK_KEY;
	#endif
	
	return lKeyInfo;
}

VCI32 mr_media_cam_get_dataType(VCVoid)
{
#if defined(MT6238) || defined(MT6239) || defined(MT6236) 
	return 1;
#else
	return 0;
#endif
}

#if defined(__DSM_CAM_ENC_SUPPORT__ ) 
VCVoid mr_media_init_enc_callback(VCVoid)
{
	extern VCVoid mr_media_cam_Send_DataReq(VCVoid);
	SetProtocolEventHandler(mr_media_cam_Send_DataReq, MSG_ID_MPCHAT_ENCODE_CNF);	
}

extern VC_STATUS VC_H264Encoder_Encode(VC_Handle * pHandle,  VCU8 * p1, VCU8 * p2,
 								VCU8 * p3, VCU32 * p4, VCU8 * p5, VCU32 * p6, VCU32 p7);
VCVoid mr_media_H264Enc_fastCall(VC_Handle * pHandle,  VCU8 * p1, VCU8 * p2,
 								VCU8 * p3, VCU32 * p4, VCU8 * p5, VCU32 * p6, VCU32 p7)
{
#ifndef  __ARM9_MMU__
		VCU32 stack_begin;
//		dbg_print("alloc cache stack size = %d\r\n", MED_INT_STACK_MEM_SIZE);

		stack_begin = (U32)drv_gfx_stkmgr_get_stack_internal(SKY_FAST_STACK_SIZE);
		if(stack_begin)
		{
			sk_dbg_print("alloc cache stack succeeded!\r\n");
			INT_SwitchStackToRun((VCVoid*)stack_begin, 3*1024,
									(kal_func_ptr)VC_H264Encoder_Encode,8,
									pHandle,  p1, p2, p3,
									p4, p5, p6, p7);
			
			drv_gfx_stkmgr_release_stack_internal((VCVoid*)stack_begin);
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

VCVoid mr_media_Send_Enocde_REQ(VCVoid* buffer, VCU32 len, VCU8 state)
{
	mr_media_enc_data_ind_struct *msg_p = NULL;
	ilm_struct *ilm_ptr = NULL;

	msg_p = (mr_media_enc_data_ind_struct*)construct_local_para(sizeof(mr_media_enc_data_ind_struct), TD_CTRL);

	msg_p->pData        = (VCU8*)buffer;
	msg_p->nDataSize  = len;
	msg_p->nState = state;

	ilm_ptr = allocate_ilm(MOD_MMI);
	ilm_ptr->src_mod_id = MOD_MMI;
	ilm_ptr->dest_mod_id    =  MOD_MULTIMEDIA;
	ilm_ptr->msg_id 	      =       MSG_ID_MPCHAT_ENCODE_REQ;
	ilm_ptr->local_para_ptr  = (local_para_struct*)msg_p;
	ilm_ptr->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_ptr);
}

VCVoid mr_media_Send_Enocde_CNF(VCU8 result)
{
	mr_media_enc_data_cnf_struct *msg_p = NULL;
	ilm_struct *ilm_ptr = NULL;

	msg_p = (mr_media_enc_data_cnf_struct*)construct_local_para(sizeof(mr_media_enc_data_cnf_struct), TD_CTRL);

	msg_p->result = result;

	ilm_ptr = allocate_ilm(MOD_MULTIMEDIA);
	ilm_ptr->src_mod_id = MOD_MULTIMEDIA;
	ilm_ptr->dest_mod_id    =  MOD_MMI;
	ilm_ptr->msg_id 	      =       MSG_ID_MPCHAT_ENCODE_CNF;
	ilm_ptr->local_para_ptr  = (local_para_struct*)msg_p;
	ilm_ptr->peer_buff_ptr = NULL;

	msg_send_ext_queue(ilm_ptr);
}

VCVoid mr_media_Encode_req_hdlr(VCVoid* pMsg)
{
	mr_media_enc_data_ind_struct *msg_p = (mr_media_enc_data_ind_struct*)pMsg;
	VMMediaEncCamData encParam;

	encParam.pData = msg_p->pData;
	encParam.nDataSize = msg_p->nDataSize;
	encParam.nSrc = msg_p->nSrc;
	encParam.nState = msg_p->nState;
	MPchat_Encode_req_handler((VCVoid *)&encParam);
}

#endif

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

#if defined(MT6236) || defined(MT6255)//Allan modify 20120401
#define MPC_AUD_BUF_LEN (16*1024) 
#else
#define MPC_AUD_BUF_LEN (4*1024) 
#endif
VCU8* mr_media_get_audio_buf(VCU32 *pSize)
{
	VCU8 *pBuf = VCMalloc(MPC_AUD_BUF_LEN);
	*pSize = 0;
	if(pBuf)
		*pSize = MPC_AUD_BUF_LEN;
	return pBuf;
}

VCVoid mr_media_cam_YUVRotation ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight )
{
#if defined(MT6252)||defined(MT6255)
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

VCVoid mr_media_cam_RGBRotation ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight )
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

static VCI32 mr_media_cam_create_preview_resource(VCU32 width, VCU32 height)
{
	VCU8 *buf_ptr;
	VCI32 buf_size;

	sk_dbg_print("mr_media_cam_create_preview_resource");

	buf_size = ((width*height*GDI_MAINLCD_BIT_PER_PIXEL)>>3);
	
	buf_ptr = VCMalloc(buf_size);
	
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

static VCVoid mr_media_cam_free_preview_resource(VCVoid)
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

static VCVoid mr_media_cam_entry_preview(VCU32 width, VCU32 height)
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
#if defined(__MMI_CAMERA__) && !defined(__MMI_CAMCORDER__)
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
	//g_camera_cntx.last_error	= 
		mdi_camera_preview_start( &camera_preview_data,&camera_setting_data, NULL);
	sk_dbg_print( "----call mdi_camera_preview_start---end-");

	gCamWidth = width;
	gCamHeight = height;
	
	{
		VCU8* pYUV[3] = {0};
		mr_media_cam_get_yuvBuf(pYUV);
		mr_media_hwRsz_set_yuvOut(pYUV[0], pYUV[1], pYUV[2]);
	}
}

VCI32 mr_media_cam_preview_start(VCU32 width, VCU32 height)
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
	
	mdi_camera_power_on(FRAMEWORK_ASM);
	mr_media_cam_entry_preview(width, height);

	return MR_SUCCESS;
}

VCI32 mr_media_cam_preview_stop(VCVoid)
{
	mdi_camera_preview_stop();
	mdi_camera_power_off();
	SKSleep(50);
	mr_media_cam_free_preview_resource();
	return MR_SUCCESS;
}

VCI32 mr_media_cam_GetData(VCVoid)
{
	mr_media_cam_send_data_req(MOD_MMI);

	return MR_SUCCESS;
}

VCI32 mr_media_cam_DataRsp(VCU8* pData, VCU32 nDataSize)
{
	mr_media_cam_send_data_rsp(pData, nDataSize);
    return MR_SUCCESS;
}

static int mr_lcdref_in_proccess = 0;
static VCVoid mr_media_display_dump(VCVoid)
{
	mr_lcdref_in_proccess = 0;
}
int splayer_getLayerindex(gdi_handle handlecur)
{
	gdi_handle handle0 = 0;
	gdi_handle handle1 = 0;
	gdi_handle handle2 = 0;
	gdi_handle handle3 = 0;
	
	gdi_layer_get_blt_layer(&handle0,&handle1,&handle2,&handle3);
	

	if(handlecur == handle0)
		return 0;
	else if(handlecur == handle1)
		return 1;
	else if(handlecur == handle2)
		return 2;
	else if(handlecur == handle3)
		return 3;
	else 
		return 0;
}
#define LAYER_OFFSET 0
void splayer_calc_blt_para(VMMediaDisp_S* pInput,mdi_blt_para_struct *blt_data_p)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	S32 layer_width;
	S32 layer_height;
	S32 layer_offset_x;
	S32 layer_offset_y;
	S32 lcd_offset_x;
	S32 lcd_offset_y;
	S32 lcd_start_x;
	S32 lcd_start_y;
	S32 lcd_end_x;
	S32 lcd_end_y;
	S32 lcd_width;
	S32 lcd_height;
	int lcd_rotate = pInput->rotation;
	
	lcd_width = LCD_WIDTH;
	lcd_height = LCD_HEIGHT;
	layer_height = pInput->height;
	layer_offset_x = pInput->x;
	layer_offset_y = pInput->y;
	layer_width = pInput->width;
	
    /* transform the coordinate to phiscal LCD's coordinate */
    if ((lcd_rotate == MDI_LCD_ROTATE_270) || 
        (lcd_rotate == MDI_LCD_ROTATE_270_MIRROR))
    {
        lcd_offset_x = lcd_width - layer_offset_y - layer_height;
        lcd_offset_y = layer_offset_x;
    }
    else if ((lcd_rotate == MDI_LCD_ROTATE_90) || 
             (lcd_rotate == MDI_LCD_ROTATE_90_MIRROR))
    {
        lcd_offset_x = layer_offset_y;
        lcd_offset_y = lcd_height - layer_offset_x - layer_width;
    }
    else
    {
            lcd_offset_x = layer_offset_x;
            lcd_offset_y = layer_offset_y;
    }

    lcd_start_x = lcd_offset_x;
    lcd_start_y = lcd_offset_y;

    /* if rotate 90 or 270 degree */
    if ((lcd_rotate == GDI_LAYER_ROTATE_90) || 
        (lcd_rotate == GDI_LAYER_ROTATE_90_MIRROR) ||
        (lcd_rotate == GDI_LAYER_ROTATE_270) || 
        (lcd_rotate == GDI_LAYER_ROTATE_270_MIRROR))
    {
        lcd_end_x = lcd_offset_x + layer_height - 1;
        lcd_end_y = lcd_offset_y + layer_width - 1;
    }
    else
    {
        lcd_end_x = lcd_offset_x + layer_width - 1;
        lcd_end_y = lcd_offset_y + layer_height - 1;
    }


    /* force inside lcd region */
    if (lcd_start_x < 0)
    {
        lcd_start_x = 0;
    }
    else if (lcd_start_x > lcd_width - 1)
    {
        lcd_start_x = lcd_width - 1;
    }

    if (lcd_start_y < 0)
    {
        lcd_start_y = 0;
    }
    else if (lcd_start_y > lcd_height - 1)
    {
        lcd_start_y = lcd_height - 1;
    }

    if (lcd_end_x < 0)
    {
        lcd_end_x = 0;
    }
    else if (lcd_end_x > lcd_width - 1)
    {
        lcd_end_x = lcd_width - 1;
    }

    if (lcd_end_y < 0)
    {
        lcd_end_y = 0;
    }
    else if (lcd_end_y > lcd_height - 1)
    {
        lcd_end_y = lcd_height - 1;
    }

    blt_data_p->lcd_start_x = lcd_start_x;
    blt_data_p->lcd_start_y = lcd_start_y;
    blt_data_p->lcd_end_x = lcd_end_x;
    blt_data_p->lcd_end_y = lcd_end_y;
    blt_data_p->display_width = layer_width;
    blt_data_p->display_height = layer_height;

    if (lcd_rotate)
    {
        /* if using LCD rotate, the roi region will be the phisical potion as lcd */
        blt_data_p->roi_offset_x = LAYER_OFFSET + lcd_offset_x;
        blt_data_p->roi_offset_y = LAYER_OFFSET + lcd_offset_y;
    }
    else
    {
        /* if using LCM rotate, layers are rotated, so the position is the same as layer pos */
        blt_data_p->roi_offset_x = LAYER_OFFSET + layer_offset_x;
        blt_data_p->roi_offset_y = LAYER_OFFSET + layer_offset_y;
    }


    /* force roi inside lcd region */
    if (blt_data_p->roi_offset_x < LAYER_OFFSET)
    {
        blt_data_p->roi_offset_x = LAYER_OFFSET;
    }
    else if (blt_data_p->roi_offset_x > LAYER_OFFSET + lcd_width - 1)
    {
        blt_data_p->roi_offset_x = LAYER_OFFSET + lcd_width - 1;
    }

    if (blt_data_p->roi_offset_y < LAYER_OFFSET)
    {
        blt_data_p->roi_offset_y = LAYER_OFFSET;
    }
    else if (blt_data_p->roi_offset_y > LAYER_OFFSET + lcd_height - 1)
    {
        blt_data_p->roi_offset_y = LAYER_OFFSET + lcd_height - 1;
    }

}

// TODO: LCD旋转需要MRP自己设置
VCI32 mr_media_LCD_Display(VCVoid* input, int input_len)
{
	VMMediaDisp_S* pInput = (VMMediaDisp_S*)input;
	VCU8* dispBuf;
	gdi_handle baseLayer, layer;
	VCI32 maxWidth = UI_device_width > UI_device_height ? UI_device_width : UI_device_height;
	U32 layerFlay;
	int index = 0;
	
	if(!pInput || pInput->img == NULL)
		return MR_FAILED;
	
	if(pInput->x < 0 || pInput->x > maxWidth || pInput->y < 0 || pInput->y > maxWidth
		|| pInput->width <=0 || pInput->width > maxWidth || pInput->height <=0 || pInput->height > maxWidth)
		return MR_FAILED;

	if(mr_lcdref_in_proccess)
		return MR_FAILED;
	
	gdi_layer_get_active(&layer);
	gdi_layer_get_blt_layer_flag(&layerFlay);
	gdi_layer_get_buffer_ptr((VCU8 * *)&dispBuf);
	index = splayer_getLayerindex(layer);
	//GDI_LOCK;
	if(pInput->format == VM_IMAGE_YUV420) // TODO: 暂不支持旋转
	{
		VMYUVBitmap* pYuv = (VMYUVBitmap *)pInput->img;
		vmSizerColorspaceParam param;
		param.m_pYBuffer = pYuv->pPlane[0];
		param.m_pUBuffer = pYuv->pPlane[1];
		param.m_pVBuffer = pYuv->pPlane[2];
		param.m_pRGBBuffer = dispBuf;
		param.m_iWidth = pYuv->nWidth;
		param.m_iHeight = pYuv->nHeight;
		param.m_iRGBWidth = pInput->width;
		param.m_iRGBHeight = pInput->height;
		param.m_iStepLuma = pYuv->nLumaStride;
		param.m_iStepChroma = pYuv->nChmaStride;
		param.m_iRGBStride = pInput->width;
		
		VM_SizerAndColorspace((VCVoid*)&param, sizeof(vmSizerColorspaceParam));
	}
	else if(pInput->format == VM_IMAGE_RGB565)  // TODO: 暂不支持旋转
	{
		 VMRGBBitmap* pRgb = (VMRGBBitmap *)pInput->img;
		//memcpy(dispBuf, pRgb->pBuffer, pRgb->nWidth*pRgb->nHeight*2); // TODO:
		dispBuf = (VCU8 *)pRgb->pBuffer;
	}
	else{
		//GDI_UNLOCK;
		return MR_FAILED;
	}
	
	#if 1
		 gdi_layer_blt_previous(0, 0, UI_device_width - 1, UI_device_height - 1);
	#else
	{
		
		lcd_frame_update_struct lcd_data = {0};
		VCU16 x = 0, y = 0, w = LCD_WIDTH, h = LCD_HEIGHT;
		VCU8 rotate_v = pInput->rotation; //gdi_layer_get_rotate();
		mdi_blt_para_struct blt_para = {0};
		
		splayer_calc_blt_para(pInput,&blt_para);
		
		 sk_dbg_print("[VM MEDIA] Show RGB565, rotate %d l rot %d", pInput->rotation, gdi_layer_get_rotate());
		mr_lcdref_in_proccess = TRUE;
		lcd_power_ctrl(KAL_TRUE);
		{
			x = blt_para.lcd_start_x;
			y = blt_para.lcd_start_y;
			w = blt_para.display_width;
			h = blt_para.display_height;
		}

		sk_dbg_print("[VM MEDIA] Show RGB565, index %d ", index);
		if(index == 0)
		{
			SET_LCD_LAYER0_WINDOW_SIZE(w, h);
			SET_LCD_LAYER0_WINDOW_OFFSET(0, 0);
	 		SET_LCD_LAYER0_ROTATE(rotate_v);
			REG_LCD_LAYER0_BUFF_ADDR = (VCU32) dispBuf;
			lcd_data.update_layer=  LCD_LAYER0_ENABLE;
#ifdef MT6236
//			SET_LCD_LAYER_WINDOW_WMEM_OFFSET(0,0,0);
			SET_LCD_LAYER_WINDOW_WMEM_PITCH(0,w);
#endif
		}
		else if(index == 1)
		{
			SET_LCD_LAYER1_WINDOW_SIZE(w, h);
			SET_LCD_LAYER1_WINDOW_OFFSET(0, 0);
	 		SET_LCD_LAYER1_ROTATE(rotate_v);
			REG_LCD_LAYER1_BUFF_ADDR = (VCU32) dispBuf;
			lcd_data.update_layer=  LCD_LAYER1_ENABLE;
#ifdef MT6236
//			SET_LCD_LAYER_WINDOW_WMEM_OFFSET(1,0,0);
			SET_LCD_LAYER_WINDOW_WMEM_PITCH(1,w);
#endif
		}
		else if(index == 2)
		{
			SET_LCD_LAYER2_WINDOW_SIZE(w, h);
			SET_LCD_LAYER2_WINDOW_OFFSET(0, 0);
	 		SET_LCD_LAYER2_ROTATE(rotate_v);
			REG_LCD_LAYER2_BUFF_ADDR = (VCU32) dispBuf;
			lcd_data.update_layer=  LCD_LAYER2_ENABLE;
#ifdef MT6236
			SET_LCD_LAYER_WINDOW_WMEM_PITCH(2,w);
#endif
		}
		else if(index == 3)
		{
			SET_LCD_LAYER3_WINDOW_SIZE(w, h);
			SET_LCD_LAYER3_WINDOW_OFFSET(0, 0);
	 		SET_LCD_LAYER3_ROTATE(rotate_v);
			REG_LCD_LAYER3_BUFF_ADDR = (VCU32) dispBuf;
			lcd_data.update_layer=  LCD_LAYER3_ENABLE;
#ifdef MT6236
			SET_LCD_LAYER_WINDOW_WMEM_PITCH(3,w);
#endif
		}
		

		lcd_data.module_id=LCD_UPDATE_MODULE_MMI;
		lcd_data.lcd_id=MAIN_LCD;
		lcd_data.fb_update_mode=LCD_SW_TRIGGER_MODE;
		lcd_data.lcm_start_x=blt_para.lcd_start_x;
		lcd_data.lcm_start_y=blt_para.lcd_start_y;
		lcd_data.lcm_end_x= blt_para.lcd_end_x;
		lcd_data.lcm_end_y= blt_para.lcd_end_y;
		lcd_data.roi_offset_x=0;
		lcd_data.roi_offset_y=0;
		lcd_data.rotate_value = rotate_v;
		lcd_data.block_mode_flag=KAL_FALSE;
		lcd_data.lcd_block_mode_cb = mr_media_display_dump;
		lcd_data.hw_update_layer =  layerFlay;

		lcd_data.memory_output=KAL_FALSE;
		lcd_fb_update(&lcd_data);	
		//GDI_UNLOCK;
	}
	#endif
	return MR_SUCCESS;
}

VCI32 mr_media_get_avbt_state(VCVoid)
{
    VCI32 result = VM_BT_NONE;

#if 0
#if defined(__MMI_BT_MTK_SUPPORT__) && defined(__MMI_A2DP_SUPPORT__)
    result = VM_BT_CLOSE;
    //if(av_bt_is_codec_open()) 
    if(mmi_a2dp_is_output_to_bt())  // TODO:
    {
	result = VM_BT_OPEN;
    }
    else
#endif
#if 0 //defined(__MMI_BT_PROFILE__)
    if(mmi_profiles_bt_is_activated()) 
    {
	result = VM_BT_OPEN;
    }
    else
    {
	result = VM_BT_CLOSE;
    }
#else
{}
#endif
#endif
    return result ;
}

VCI32 mr_media_avbt_open(VCVoid)
{
	// TODO:
	//av_bt_open_codec(TRUE); 
	return 0;
}

VCI32 mr_media_avbt_close(VCVoid)
{
	// TODO:
	//av_bt_close_codec();
#if 0 //def __MMI_A2DP_SUPPORT__
	av_bt_close(FALSE);
#endif
	return 0;
}

static VCU32 sResizerOpen = 0;
static VCU8* pResizerBuf = NULL;
extern VCI32 VM_SizerAndColorspace(VCVoid * pInput, VCI32 input_len);
VCI32 mr_media_resizer_and_colorspace(VCVoid* input, VCI32 input_len)
{
	vmSizerColorspaceParam* pInput = (vmSizerColorspaceParam *)input;
	if(!input)
		return MR_FAILED;
#if (defined(MT6253) || defined(MT6253T)|| defined(MT6236))
	if(!gMrCamExtRunning && !sResizerOpen)
	{
		if(mr_media_hwRsz_init(pInput->m_iRGBStride) == 0)
		{
			sResizerOpen = 1;
		}
	}

	if(sResizerOpen)
	{		
		mr_media_hwRsz_setYUV2RGB(1);
		mr_media_hwRsz_imageEX(pInput->m_pYBuffer,
					pInput->m_pUBuffer,
					pInput->m_pVBuffer,
					pInput->m_pRGBBuffer,
					pInput->m_iWidth,
					pInput->m_iHeight,
					pInput->m_iRGBWidth,
					pInput->m_iRGBHeight,
					pInput->m_iStepLuma, 
					pInput->m_iStepChroma);

		if(pInput->m_iRGBStride > pInput->m_iRGBWidth)
		{
			int i;
			memmove((VCU8 *)pInput->m_pRGBBuffer+(pInput->m_iRGBHeight-1)*pInput->m_iRGBStride*2, (VCU8 *)pInput->m_pRGBBuffer+(pInput->m_iRGBHeight-1)*pInput->m_iRGBWidth*2, pInput->m_iRGBWidth*2);
			for(i=pInput->m_iRGBHeight-2; i>0; i--)
			{
				memmove((VCU8 *)pInput->m_pRGBBuffer+i*pInput->m_iRGBStride*2, (VCU8 *)pInput->m_pRGBBuffer+i*pInput->m_iRGBWidth*2, pInput->m_iRGBWidth*2);
				memset((VCU8 *)pInput->m_pRGBBuffer+i*pInput->m_iRGBStride*2+pInput->m_iRGBWidth*2, 0, (pInput->m_iRGBStride-pInput->m_iRGBWidth)*2);
			}
			memset((VCU8 *)pInput->m_pRGBBuffer+pInput->m_iRGBWidth*2, 0, (pInput->m_iRGBStride-pInput->m_iRGBWidth)*2);
		}

		mr_media_resizer_close();
	}
	else
#endif
	
	VM_SizerAndColorspace((VCVoid*)input, input_len);
	return MR_SUCCESS;
}

VCI32 mr_media_resizer_close(VCVoid)
{
#if (defined(MT6253) || defined(MT6253T)|| defined(MT6236))
	if(sResizerOpen)
	{
		mr_media_hwRsz_free();
		if(pResizerBuf)
			VCFree(pResizerBuf);
		pResizerBuf = NULL;
		sResizerOpen = 0;
	}
#endif
	return MR_SUCCESS;
}

VCVoid mr_media_fast_call(VCVoid (*pFunc)(VCVoid *pParam),VCVoid *pParam)
{
	//	sk_dbg_print("alloc cache stack size = %d,pFunc = %d\r\n", SKY_FAST_STACK_SIZE,(int)pFunc);
	if(pFunc)
	{
#if !defined(__ARM9_MMU__) && !defined(MT6252)  && !defined(MT6252H)
		VCU32 stack_begin;
		//		dbg_print("alloc cache stack size = %d\r\n", MED_INT_STACK_MEM_SIZE);

		stack_begin = (U32)drv_gfx_stkmgr_get_stack_internal(SKY_FAST_STACK_SIZE);
		if(stack_begin)
		{
			sk_dbg_print("alloc cache stack succeeded!\r\n");
			INT_SwitchStackToRun((VCVoid*)stack_begin,SKY_FAST_STACK_SIZE,(kal_func_ptr)pFunc, 1, pParam,0);

			drv_gfx_stkmgr_release_stack_internal((VCVoid*)stack_begin);
		}
		else
#endif
		{
			sk_dbg_print("alloc cache stack failed!\r\n");
			pFunc(pParam);
		}
	}

	return;
}

VCVoid mr_media_timer_start(VCI32 count, VCVoid (*callback) (VCVoid))
{
	gui_start_timer(count, callback);
}

static VMPlatInfo sVmMediaInfo = {0};
VCI32 mr_media_getPlatInfo(VCU8** output, VCI32* output_len)
{
	if(!output || !output_len)
		return MR_FAILED;
	
	sVmMediaInfo.m_uAudRecFormat = D_VM_AUDIO_PCM8 | D_VM_AUDIO_PCM16;
	sVmMediaInfo.m_uAudPlayFormat = D_VM_AUDIO_PCM8 | D_VM_AUDIO_PCM16;
#ifdef AMR_DECODE
	sVmMediaInfo.m_uAudRecFormat |= D_VM_AUDIO_AMR;
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_AMR;
#ifdef AMRWB_DECODE
	sVmMediaInfo.m_uAudRecFormat |= D_VM_AUDIO_AMR_IF2;
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_AMR_IF2;
#endif
#endif
#ifdef DAF_DECODE
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_MP3;
#endif
#ifdef AAC_DECODE
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_AAC;
#endif

	sVmMediaInfo.m_uVidRecFormat |= D_VM_IMAGE_RGB565 | D_VM_IMAGE_YUV420;

	sVmMediaInfo.m_uVidDecFormat |= D_VM_VIDEO_H264;
	
#if 0 //def __DSM_SPLAYER__
	sVmMediaInfo.m_uVidDecFormat |= D_VM_VIDEO_H263 | D_VM_VIDEO_FLV1 | D_VM_VIDEO_MPEG4;
#endif
#ifdef __DSM_CAM_ENC_SUPPORT__
	sVmMediaInfo.m_uVidEncFormat |= D_VM_VIDEO_H264;
#endif
	
#if 0 //def __DSM_SPLAYER__
	sVmMediaInfo.m_uDemuxFormat |= D_VM_DEMUX_AVI | D_VM_DEMUX_3GP | D_VM_DEMUX_MP4 | D_VM_DEMUX_FLV;
#ifdef __DSM_SPLAYER_RMVB_SUPPORT__
	sVmMediaInfo.m_uDemuxFormat |= D_VM_DEMUX_RM | D_VM_DEMUX_RMVB;
#endif
#endif
	sVmMediaInfo.m_uVMVersion = mr_media_get_version();

	*output = (VCU8 *)&sVmMediaInfo;
	*output_len = sizeof(VMPlatInfo);

	return MR_SUCCESS;
}

/////////////////////////////////////////////////////

#if 1 //!defined(__DSM_SPLAYER__)
VCI32 mr_platEx_splayer(VCI32 code, VCU8* input, VCI32 input_len, VCU8** output, VCI32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_IGNORE;
}
#endif

#endif

