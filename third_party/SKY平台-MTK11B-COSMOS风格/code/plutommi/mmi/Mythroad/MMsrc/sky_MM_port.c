/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/

#if (defined(__DSM_SPLAYER__) || defined(__MMI_DSM_MEDIA_EXT__))&& defined(__MTK_TARGET__)

/*************************************************************************
* Include Statements
 *************************************************************************/
#include "SKY_MM_def.h"
 
#include "MMI_include.h"
#include "lcd_sw_rnd.h"
#include "MMI_features.h" 
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

#include "mdi_include.h"
#include "mdi_datatype.h"
#include "mdi_audio.h"          /* audio lib */
#include "mdi_video.h"          /* video lib */

#include "SettingProfile.h"     /* warning tone */
#include "FileManagerGProt.h"   /* file path / file error  */
#include "FileManagerDef.h"     /* error string id */
#include "FileMgr.h"
#include "USBDeviceGprot.h"     /* USB MS mode */
#include "DataAccountGprot.h"


#include "resource_video_skins.h"       /* UI custom config */
#include "VdoRecResDef.h"
#include "VdoPlyResDef.h"
#include "VdoRecGProt.h"
#include "VdoPlyApp.h"
#include "VdoPlyGProt.h"
#ifndef MT6255
#include "ImageViewerGprot.h"
#endif
#include "app_mine.h"           /* mine type */
#include "custom_data_account.h"


#if defined(__VDOPLY_FEATURE_BT__)
#include "SimDetectionGprot.h"
#include "BTMMIScrGprots.h"
#include "av_bt.h"
#include "app_str.h"
#include "AudioPlayerProt.h"
#endif /* __VDOPLY_FEATURE_BT__ */


#include "med_utility.h"


#include "aud_defs.h"
#include "med_struct.h"
#include "l1audio.h"
#include "med_main.h"

#include "med_global.h"
#include "med_status.h"
#include "fsal.h"
#include "vid_main.h"

#include "med_api.h"
#include "med_context.h"

#include "sky_mm_basetypes.h"
#include "sky_mm_port.h"

#if defined(__DSM_SPLAYER__) 
#include "sky_mm_SPlayer_api.h"
#endif


#ifdef SKY_MM_USE_APP_MEM
#define med_alloc_ext_mem(size) applib_asm_alloc_anonymous_nc(size)
#define med_alloc_ext_mem_cacheable(size) applib_asm_alloc_anonymous(size)
#define med_free_ext_mem(ptr) applib_asm_free_anonymous(*ptr)
#endif

//#define DSM_MULTIMEDIA_DEBUG2
#define DSM_MULTIMEDIA_DEBUG
#ifdef DSM_MULTIMEDIA_DEBUG
#define MAXCHARS 512
static char sk_print_buf[MAXCHARS];
void sk_dbg_print(char *fmt,...)
{
   va_list ap;
va_start (ap, fmt);
vsprintf(sk_print_buf, fmt, ap);
va_end(ap);
//kal_prompt_trace(MOD_MED,  sk_print_buf);
#if 1
kal_prompt_trace(MOD_MED,  sk_print_buf);
#else
{
		int ff;
		ff = FS_Open(L"e:\\splayer.log", FS_CREATE|FS_READ_WRITE);
		if(ff >= 0)
		{
			UINT w;
			FS_Seek(ff, 0, FS_FILE_END);
			FS_Write(ff, sk_print_buf, strlen(sk_print_buf), &w);
			FS_Write(ff, "\r\n", strlen("\r\n"), &w);
			FS_Close(ff);
		}
	}
#endif

}

#else
void sk_dbg_print(char *fmt,...){}
#endif

#ifdef __DSM_SPLAYER_RMVB_SUPPORT__
#ifdef __DSM_SPLAYER_RMVB_HD_SUPPORT__
#define MAX_MALLOC_SIZE 	(1.5*1024*1024)
#else
#define MAX_MALLOC_SIZE 	(1*1024*1024)
#endif
#define MIN_BUFFER_SIZE	(3*1024*1024)
#endif

#define ID_EVENT_SDK_MALLC	101
#define ID_EVENT_SDK_FREE	102

typedef struct _tag_SKY_STREAMING_SDK_MEM
{
	MMVoid *pMem;
	MMInt32 nSize;
}SKY_STREAMING_SDK_MEM;

MMInt32 mr_event(MMInt16 type, MMInt32 param1, MMInt32 param2);


#if defined(__DSM_SPLAYER__)
extern vid_context_struct *vid_context_p;

const static splayer_port_context_t s_splayer_port_context =
{
	MSG_ID_SPLAYER_OPEN, //msg_type splayer_open_msgid;
	MSG_ID_SPLAYER_PLAY, //msg_type splayer_play_msgid;
	MSG_ID_SPLAYER_PAUSE, //msg_type splayer_pause_msgid;
	MSG_ID_SPLAYER_SEEK, //msg_type splayer_seek_msgid;
	MSG_ID_SPLAYER_STOP, //msg_type splayer_stop_msgid;
	MSG_ID_SPLAYER_CLOSE, //msg_type splayer_close_msgid;
	MSG_ID_SPLAYER_GET_DES,

	MSG_ID_MEDIA_VID_OPEN_REQ, //msg_type media_open_msgid;
	MSG_ID_MEDIA_VID_PLAY_REQ, //msg_type media_play_msgid;
	MSG_ID_MEDIA_VID_PAUSE_REQ, //msg_type media_pause_msgid;
	MSG_ID_MEDIA_VID_SEEK_REQ, //msg_type media_seek_msgid;
	MSG_ID_MEDIA_VID_STOP_REQ, //msg_type media_stop_msgid;
	MSG_ID_MEDIA_VID_CLOSE_REQ, //msg_type media_close_msgid;
	MSG_ID_MEDIA_VID_GET_CONTENT_DESC_REQ,

	MSG_ID_MEDIA_VID_FILE_READY_IND, //msg_type media_vid_file_ready_ind;

	MOD_MULTIMEDIA, //custom_module_type mod_multimedia;
	MOD_MED,					//custom_module_type mod_med;
	MED_SAP, //sap_type	med_sap;

	MED_RES_OK, //int32			med_res_ok;
	MED_RES_FAIL, //int32 		med_res_fail;
	MED_RES_VIDEO_UNSUPPORT_DECODE_SIZE, //int32			med_res_video_unsupport_decode_size;

	MED_MODE_FILE, //int32			med_mode_file;
	MED_MODE_ARRAY, //int32			med_mode_array;
	MED_MODE_PDL_FILE, //int32			med_mode_pdl_file;

	VID_EVT_OPEN, //int32			vid_evt_open;
	VID_EVT_PLAY, //int32 		vid_evt_play;
	VID_EVT_SEEK, //int32			vid_evt_seek;
	VID_EVT_STOP, //int32			vid_evt_stop;
	VID_EVT_CLOSE, //int32			vid_evt_close;
	VID_EVT_BLOCK,
	VID_EVT_PAUSE,

	KAL_OR,
	KAL_NO_WAIT,

	MSG_ID_SPLAYER_GET_FRAME,
	MSG_ID_SPLAYER_GET_PLAY_TIME,
	MSG_ID_SPLAYER_SET_STOP_TIME,
	MSG_ID_SPLAYER_SET_SLEEP_TIME,
	MSG_ID_SPLAYER_TIME_TO_FRAME,
	MSG_ID_SPLAYER_FRAME_TO_TIME,
	MSG_ID_SPLAYER_GET_IFRAME,
	MSG_ID_SPLAYER_SET_PARAM,
	MSG_ID_SPLAYER_CHECK_MED_READY,
	MSG_ID_SPLAYER_PAUSE_VISUAL_UPDATE,
	MSG_ID_SPLAYER_RESUME_VISUAL_UPDATE,

	MSG_ID_MEDIA_VID_GET_FRAME_REQ,
	MSG_ID_MEDIA_VID_GET_PLAY_TIME_REQ,
	MSG_ID_MEDIA_VID_SET_STOP_TIME_REQ,
	MSG_ID_MEDIA_VID_SET_SLEEP_TIME_REQ,
	MSG_ID_MEDIA_VID_TIME_TO_FRAME_REQ,
	MSG_ID_MEDIA_VID_FRAME_TO_TIME_REQ,
	#if (MTK_VERSION <= 0x11A1132)
	MSG_ID_MEDIA_VID_GET_IFRAME_REQ,
	#else
	NULL,
	#endif
	MSG_ID_MEDIA_VID_SET_PARAM_REQ,
	MSG_ID_MEDIA_VID_CHECK_MED_READY_REQ,
	MSG_ID_MEDIA_VID_PAUSE_VISUAL_UPDATE_REQ,
	MSG_ID_MEDIA_VID_RESUME_VISUAL_UPDATE_REQ,

	VID_EVT_GET_INFO,
	VID_EVT_GET_FRAME,
	VID_EVT_GET_IFRAME,
	MEDIA_DATA_REQUEST,
	
};

const splayer_port_context_t* p_splayer_port_context = &s_splayer_port_context;

void SPlayer_set_result(kal_int32 event, kal_int32 result)
{
		vid_set_result(result, 0);
		kal_set_eg_events(vid_context_p->vid_event, (event),KAL_OR);
}

kal_uint16 SPlayer_get_vid_image_width(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->image_width;
}

kal_uint16 SPlayer_get_vid_image_height(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->image_height;
}

kal_uint32 SPlayer_get_vid_total_frame_num(void)
{
	//ASSERT(vid_context_p);
	//return vid_context_p->total_frame_num;
return NULL;
}

kal_uint64 SPlayer_get_vid_total_time(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->total_time;
}

kal_uint16 SPlayer_get_vid_seq_num(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->seq_num;
}

kal_int32 SPlayer_get_vid_handle(void)
{
#if (MTK_VERSION <= 0x11A1132)
	ASSERT(vid_context_p);
	return vid_context_p->handle;
#else
	return NULL;//vid_context_p->handle;
#endif
}

kal_int32 SPlayer_get_vid_file_handle(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->file_handle;
}

module_type SPlayer_get_vid_src_mod(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->src_mod;
}

kal_eventgrpid SPlayer_get_vid_event(void)
{
	ASSERT(vid_context_p);
	return vid_context_p->vid_event;
}

void SPlayer_set_vid_src_mod(int module)
{
	ASSERT(vid_context_p);
	vid_context_p->src_mod =(module_type) module;
}

void SPlayer_set_vid_seq_num(kal_uint16 seq_num)
{
	ASSERT(vid_context_p);
	vid_context_p->seq_num = seq_num;
}

void SPlayer_set_vid_media_mode(kal_uint8 media_mode)
{
	ASSERT(vid_context_p);
	vid_context_p->media_mode = media_mode;
}

void SPlayer_set_vid_media_type(kal_uint8 media_type)
{
	ASSERT(vid_context_p);
	vid_context_p->media_type = media_type;
}

void SPlayer_set_vid_image_width(kal_uint16 image_width)
{
	ASSERT(vid_context_p);
	vid_context_p->image_width = image_width;
}

void SPlayer_set_vid_image_height(kal_uint16 image_height)
{
	ASSERT(vid_context_p);
	vid_context_p->image_height = image_height;
}

void SPlayer_set_vid_total_frame_num(kal_uint32 total_frame_num)
{
	//ASSERT(vid_context_p);
	//vid_context_p->total_frame_num = total_frame_num;
}

void SPlayer_set_vid_total_time(kal_uint64 total_time)
{
	ASSERT(vid_context_p);
	vid_context_p->total_time = total_time;
}

void SPlayer_set_vid_audio_channel_no(kal_uint8 audio_channel_no)
{
	ASSERT(vid_context_p);
	vid_context_p->audio_channel_no = audio_channel_no;
}
int SPlayer_get_vid_audio_channel_no(void)
{
	//ASSERT(vid_context_p);
	return vid_context_p->audio_channel_no;
}
void SPlayer_set_vid_audio_sample_rate(kal_uint8 audio_sample_rate)
{
	ASSERT(vid_context_p);
	vid_context_p->audio_sample_rate = audio_sample_rate;
}

void SPlayer_set_vid_file_handle(kal_uint32 file_handle)
{
	ASSERT(vid_context_p);
	vid_context_p->file_handle = file_handle;
}

void SPlayer_set_file_buffer_address(kal_uint32 buffer_address, kal_uint32 size)
{
	ASSERT(vid_context_p);
	vid_context_p->file_buffer_address = buffer_address;
	vid_context_p->file_size = size;
}

void SPlayer_copy_current_file_name(void* p_data)
{
	ASSERT(vid_context_p);
	ASSERT(p_data);
	kal_mem_cpy(vid_context_p->current_file_name, p_data, MAX_FILE_NAME_LEN * 2);
}

void SPlayer_set_vid_is_seekable(kal_uint8 isSeekable)
{
	//1112_V36以下编译出错，请注释这一行
	vid_context_p->is_seekable = isSeekable;
}

kal_uint8 SPlayer_get_vid_is_seekable(void)
{
	//1112_V36以下编译出错，请注释这一行
	return vid_context_p->is_seekable;
	//return KAL_TRUE;
}
void media_vid_open_req_struct_cpy(media_vid_open_req_struct_cpy_t* pData_Dst,void* pData_Src)
{
	media_vid_open_req_struct* pData =(media_vid_open_req_struct*) pData_Src;
	
	pData_Dst->media_mode = pData->media_mode;
	pData_Dst->media_type = pData->media_type;
	pData_Dst->data = pData->data;
	pData_Dst->data_size = pData->data_size;
	pData_Dst->seq_num = pData->seq_num;
	pData_Dst->open_track = pData->open_track;
	pData_Dst->scenario = pData->scenario;
	pData_Dst->blocking = pData->blocking;
	pData_Dst->enable_aud_only = pData->enable_aud_only;
	pData_Dst->image_width = pData->image_width;
	pData_Dst->image_height = pData->image_height;
	pData_Dst->total_time = pData->total_time;
	pData_Dst->handle = pData->handle;
	pData_Dst->app_id = pData->app_id;
	pData_Dst->file_handle = pData->file_handle;
	
}
void media_vid_play_req_struct_cpy(media_vid_play_req_struct_cpy_t* pData_Dst,void* pData_Src)
{
	media_vid_play_req_struct* pData =(media_vid_play_req_struct*) pData_Src;
	pData_Dst->media_mode = pData->media_mode;
	pData_Dst->media_type = pData->media_type;
	pData_Dst->data = pData->data;
	pData_Dst->display_width = pData->display_width;
	pData_Dst->display_height = pData->display_height;
	pData_Dst->image_buffer_p = pData->image_buffer_p;
	pData_Dst->zoom_factor = pData->zoom_factor;
	pData_Dst->frame_rate = pData->frame_rate;
	pData_Dst->contrast = pData->contrast;
	pData_Dst->brightness = pData->brightness;
	pData_Dst->play_speed = pData->play_speed;
	pData_Dst->play_audio = pData->play_audio;
	pData_Dst->display_device = pData->display_device;
	pData_Dst->audio_path = pData->audio_path;
	pData_Dst->seq_num = pData->seq_num;
	pData_Dst->repeats = pData->repeats;
	pData_Dst->lcd_id = pData->lcd_id;
	pData_Dst->update_layer = pData->update_layer;
	pData_Dst->hw_update_layer = pData->hw_update_layer;
	pData_Dst->image_data_format = pData->image_data_format;
	pData_Dst->force_lcd_hw_trigger = pData->force_lcd_hw_trigger;
	pData_Dst->idp_rotate = pData->idp_rotate;
	pData_Dst->layer_element = pData->layer_element;
	pData_Dst->blt_ctx = pData->blt_ctx;
	pData_Dst->blt_dev = pData->blt_dev;
	
}
void media_vid_seek_req_struct_cpy(media_vid_seek_req_struct_cpy_t* pData_Dst,void* pData_Src)
{
	media_vid_seek_req_struct* pData =(media_vid_seek_req_struct*) pData_Src;
	pData_Dst->get_frame = pData->get_frame;
	pData_Dst->frame_num = pData->frame_num;
	pData_Dst->time = pData->time;
	pData_Dst->display_width = pData->display_width;
	pData_Dst->display_height = pData->display_height;
	pData_Dst->image_buffer_p = pData->image_buffer_p;
	pData_Dst->blocking = pData->blocking;
	pData_Dst->seq_num = pData->seq_num;
	pData_Dst->force_seek = pData->force_seek;
	pData_Dst->seek_track = pData->seek_track;
	pData_Dst->idp_rotate = pData->idp_rotate;
	pData_Dst->image_data_format = pData->image_data_format;
}

void media_vid_get_frame_req_struct_cpy(media_vid_get_frame_req_struct_cpy_t* pData_Dst,void* pData_Src)
{
	media_vid_get_frame_req_struct* pData =(media_vid_get_frame_req_struct*) pData_Src;
	pData_Dst->display_width = pData->display_width;
	pData_Dst->display_height = pData->display_height;
	pData_Dst->image_buffer_p = pData->image_buffer_p;
	pData_Dst->idp_rotate = pData->idp_rotate;
	pData_Dst->image_data_format = pData->image_data_format;
	pData_Dst->frame_mode = pData->frame_mode;
}
void media_vid_get_play_time_req_struct_cpy(media_vid_get_play_time_req_struct_cpy_t* pData_Dst,void* pData_Src)
{
	media_vid_get_play_time_req_struct* pData =(media_vid_get_play_time_req_struct*) pData_Src;
	pData_Dst->cur_time_p = pData->cur_time_p;

}

void SPlayer_vid_working_item(void)
{
#if defined(__MED_VID_EDT_MOD__)
	vid_add_video_working_data_item(vid_context_p->media_mode, &vid_context_p->handle);
	vid_load_video_working_data_buf(vid_context_p->handle);
#endif
}

void SPlayer_Get_LCDSize(kal_uint32* width, kal_uint32* height, kal_uint32* pFormat)
{
	*width = LCD_WIDTH;
	*height = LCD_HEIGHT;
	if(pFormat != NULL)
	{
	    *pFormat = Splayer_GetColorSpace();
	    sk_dbg_print("pFormat1 =%d",*pFormat);
	}
}

#ifdef __DSM_SPLAYER_RMVB_SUPPORT__
static void* gSkyRmvbMem = NULL;
VOID* SKYRMVB_GetMemory(kal_uint32* size)
{
	int mLeft;

	if(gSkyRmvbMem)
		MMFree(gSkyRmvbMem);
	gSkyRmvbMem = NULL;
	
	/*
	mLeft = SKY_MM_LEFT_SIZE();
	if(mLeft < MAX_MALLOC_SIZE+MIN_BUFFER_SIZE)
	{
		*size = 0;
		return NULL;
	}
	*/
	mLeft = MAX_MALLOC_SIZE;
	gSkyRmvbMem = (void*)MMMalloc(mLeft);
	*size = mLeft;
	
	return gSkyRmvbMem;
}

VOID SKYRMVB_FreeMemory()
{
	if(gSkyRmvbMem)
		MMFree(gSkyRmvbMem);
	gSkyRmvbMem = NULL;
}
#endif

unsigned int SPlayer_getSysTime(void)
{
	kal_uint32 time = 0;
	kal_uint32 return_t;
	kal_get_time(&time);
	//return_t = (time* 0.004615 + 0.5)*1000;
    	return_t = kal_ticks_to_milli_secs(time);
	return return_t;
}

extern vdoply_context_struct g_vdoply_cntx;
extern kal_int32 SPlayer_isActive(void);
int SPlayer_GetVdoPlyStatus(void)
{
	SPLAYER_STATE state = SPLAYER_INVALID;
#if defined(__MMI_VIDEO_PLAYER__)
	if(SPlayer_isActive())
	{
		switch(g_vdoply_cntx.state)
		{
		case VDOPLY_STATE_IDLE:
			state = SPLAYER_IDLE;
			break;
		case VDOPLY_STATE_OPENING:
		case VDOPLY_STATE_INTERRUPTED_OPENING:
			state = SPLAYER_OPENING;
			break;
		case VDOPLY_STATE_PLAY:
			state = SPLAYER_PLAY;
			break;
		case VDOPLY_STATE_FULLSCR_PLAY:
			state = SPLAYER_FULL_PLAY;
			break;
		case VDOPLY_STATE_PAUSE:
		case VDOPLY_STATE_INTERRUPTED_PAUSE:
			state = SPLAYER_PAUSE;
			break;
		}
	}
#else
	state = SPLAYER_PLAY;
#endif
	return state;
}

void splayer_register_pen_down_handler(mmi_pen_hdlr pen_down_hdlr)
{
#if defined(__MMI_TOUCH_SCREEN__)
	wgui_register_pen_down_handler(pen_down_hdlr);
#endif
}

void splayer_register_pen_up_handler(mmi_pen_hdlr pen_up_hdlr)
{
#if defined(__MMI_TOUCH_SCREEN__)
	wgui_register_pen_up_handler(pen_up_hdlr);
#endif
}

MMVoid *MMMallocNoncache(MMInt32 nSize)
{
	MMInt32 *pRet = NULL;
	pRet = (MMInt32*)med_alloc_ext_mem((unsigned int)(nSize+4));
	if (pRet) {
		memset(pRet, 0, nSize+4);
	}
	else
	{
		sk_dbg_print("MMMallocNoncache failed!!!size %d left %d", nSize, med_ext_left_size());
	}
	return pRet;
}
MMVoid  MMFreeNoncache(MMVoid *pMem)
{
	if(pMem)
	{
		med_free_ext_mem((MMVoid **)&pMem);
	}
}

#include "timerEvents.h"
MMVoid  SKYStartMediaTimer(MMInt32 elapsed, MMVoid *pFunc, MMVoid *pParam)
{
	//med_start_timer(STREAM_VID_EOF_TIMER, elapsed, (kal_timer_func_ptr)pFunc, pParam);
	StartNonAlignTimer(J2ME_VM_EVENT_TIMER, elapsed, (kal_timer_func_ptr)pFunc);
}
MMVoid  SKYStopMediaTimer(MMVoid)
{
	//med_stop_timer(STREAM_VID_EOF_TIMER);
	StopTimer(J2ME_VM_EVENT_TIMER);
}

/* SIGNAL */
static volatile kal_mutexid g_SKYMutex = 0;

MMVoid  MMMutexInit(MMVoid)
{
	if(g_SKYMutex == 0)
	{
	g_SKYMutex = kal_create_mutex("SKYMutex");
	}
}
MMVoid  MMMutexTake(MMVoid)
{
	kal_take_mutex(g_SKYMutex);
}
MMVoid  MMMutexGive(MMVoid)
{
	kal_give_mutex(g_SKYMutex);
}

#endif
/////////////////////////////////////////////////////////////////////

void SKSleep(int time)
{
	if(time < 0)
		return;
	if(time < 5)
		kal_sleep_task(1);
	else
		kal_sleep_task(kal_milli_secs_to_ticks(time));
	return;
}

unsigned int get_cur_sys_time()
{
	unsigned int ticks;
	kal_get_time(&ticks);
	return  kal_ticks_to_milli_secs(ticks);
}

#define SKY_DATAFILE
#ifdef SKY_DATAFILE
//#define MULTI_DATAFILE
#if MULTI_DATAFILE
#define MAX_DATA_FILE 5
#define MAX_DATA_FILENAME_LEN 10

#else
#define MAX_DATA_FILE 1
#define MAX_DATA_FILENAME_LEN 10
#endif

#define MAX_FILEHANDLE 10
#define CHECK_FILE_HANDLE(fp)  if(!fp) \
{ \
	sk_dbg_print("BAD FILE HANDLE \r\n");\
	return 0; \
}\

typedef struct 
{
	void *pFileData;
	int lFileLen;
	MMUInt16 szFilename[MAX_DATA_FILENAME_LEN];
}DataFile_t;

typedef enum
{
	SKY_FILE_NULL = 0,
	SKY_FILE_NORMAL ,
	SKY_FILE_DATA,
	SKY_FILE_MAX
}SKY_FILETYPE_t;

typedef struct 
{
	SKY_FILETYPE_t filetype;
	MMFile hFile;

   /// For ROM file. Point to the start memory address of the file.
   kal_uint8 *pbFile;//file name
   kal_uint32 uRamFileSize;
   kal_uint32 uRamFileOffset;
//   kal_uint32 uFileSize;
//   kal_uint32 uFileOffset;
//   kal_bool bBuffering;
   kal_uint8 *pbRamBuf;
   kal_uint32 uRamBufSize;
//   kal_uint32 uCachedBlock;
//   kal_bool bDirty; /* whether the cache has been written to */
//   kal_uint32 uLastReadCount;
//   kal_uint32 uDRMpermission;
//   FSAL_ByteOrder uByteOrder;
}SKY_FileHandle_t;

SKY_FileHandle_t g_FileHandle[MAX_FILEHANDLE] = {0};

DataFile_t g_DataFile[MAX_DATA_FILE] = {0};

int MMFSetDataFile(void *pData,unsigned int lSize,MMUInt16 *szFilename)
{
	int count = 0;
#if MULTI_DATAFILE
	for(count = 0;count <MAX_DATA_FILE;count++)
	{
		if(g_DataFile[count].pFileData == NULL)
		{
#endif		
			g_DataFile[count].pFileData = pData;
			g_DataFile[count].lFileLen = lSize;
			kal_wstrncpy((WCHAR *)g_DataFile[count].szFilename,(const WCHAR*)szFilename, MAX_DATA_FILENAME_LEN);
			return 1;
#if MULTI_DATAFILE
		}
	}
	
	return 0;
#endif
}

void * MMFGetDummyFileName(int index,int *plLen)
{
	static unsigned short * g_pDummyFileName[4] = {L"dummy.mp4",
		L"dummy.avi",
		L"dummy.3gp",
		L"dummy.flv",
		};
	if(index >= 4)
		index = 0;
	
	*plLen = kal_wstrlen(g_pDummyFileName[index]);
	return (void *)g_pDummyFileName[index];
}
/********************************************************
*返回的只有FLV和MP4这两种格式区分
*
*********************************************************/
int MMFCheckDataFormat(void *pData,unsigned int lDataSize)
{
	unsigned char *pDataTemp = 0;
	pDataTemp = (unsigned char *)pData;

	if((pDataTemp[0] == 'F')&&(pDataTemp[1] == 'L')&&(pDataTemp[2] == 'V'))
		return 3;
	else if((pDataTemp[8] == 'A')&&(pDataTemp[9] == 'V')&&(pDataTemp[10] == 'I'))
		return 2;
	else if((pDataTemp[8] == '3')&&(pDataTemp[9] == 'g')&&(pDataTemp[10] == 'p'))
		return 1;
	else
		return 0;
}
int MMFGetDataFile(MMUInt16 *szFilename,void **pData,int *plSize)
{
	int index = 0;
	while(index <4)
	{
		if(kal_wstrncmp((void*)g_DataFile[index].szFilename, (void*)szFilename,MAX_DATA_FILENAME_LEN ) == 0)
		{
			*pData = g_DataFile[index].pFileData;
			*plSize = g_DataFile[index].lFileLen;
			return 1;
		}
		index++;
	}
	return 0;
}
SKY_FileHandle_t* MMSeachEmptyHandle()
{
	int count =0;
	for(count = 0;count <MAX_FILEHANDLE;count ++)
	{
		if(g_FileHandle[count].filetype == SKY_FILE_NULL)
		{
			return &(g_FileHandle[count]);
		}
	}
	
	return NULL;
}
#endif

MMFile MMWFOpen(MMUInt16 *strFileName,MMUInt32 flag)
{
	int l_iOpenFlag = 0;
	FS_HANDLE l_ifd = 0;
	
#ifdef SKY_DATAFILE
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = 0;
	
	sk_dbg_print("MMWFOpen %d",strFileName[0]);
	if(MMFGetDataFile(strFileName,&pdata,&lSize))
	{
		sk_dbg_print("MMWFOpen lSize = %d",lSize);
		pFilehandle = (SKY_FileHandle_t *)MMSeachEmptyHandle();
		memset((void *)pFilehandle,0,sizeof(SKY_FileHandle_t));
		pFilehandle->filetype = SKY_FILE_DATA;
		pFilehandle->pbFile = (kal_uint8 *)strFileName;
		pFilehandle->uRamFileSize= lSize;
		pFilehandle->pbRamBuf= pdata;
		pFilehandle->uRamFileOffset = 0;
		return (MMFile)pFilehandle;
	}
#endif

	if((flag & MM_FILE_READONLY) && (flag & MM_FILE_WRITEONLY))
	{
		return MMNULL;
	}
	if(flag & MM_FILE_READONLY)
	{
		l_iOpenFlag = l_iOpenFlag | FS_READ_ONLY;
	}
	else if(flag & MM_FILE_WRITEONLY)
	{

		l_iOpenFlag = l_iOpenFlag | FS_READ_WRITE;
		
	}
	else if(flag & MM_FILE_READWRITE)
	{
		if(flag & MM_FILE_CREATE)
		{
			l_iOpenFlag = l_iOpenFlag | FS_READ_WRITE | FS_CREATE;
		}
		else
		{
			l_iOpenFlag = l_iOpenFlag | FS_READ_WRITE ;
		}
	}
	else
	{
		return MMNULL;
	}

	l_ifd = FS_Open((WCHAR *)strFileName,l_iOpenFlag);

	if(l_ifd < 0)
	{
		l_iOpenFlag =  l_iOpenFlag | FS_CREATE;
		l_ifd = FS_Open((WCHAR *)strFileName,l_iOpenFlag);
	}
	if(l_ifd < 0)
	{
		return MMNULL;
	}

#ifdef SKY_DATAFILE
	pFilehandle = (SKY_FileHandle_t *)MMSeachEmptyHandle();
	memset(pFilehandle,0,sizeof(SKY_FileHandle_t));
	pFilehandle->filetype = SKY_FILE_NORMAL;
	pFilehandle->hFile = (MMFile)l_ifd;
	return (MMFile)pFilehandle;
#else
	return (MMFile) l_ifd;
#endif
	
}


MMFile	MMFOpen(MMUInt8 *strFileName,MMUInt32 flag)
{

	int l_iOpenFlag = 0;
	FS_HANDLE l_ifd = 0;
	
#ifdef SKY_DATAFILE
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = 0;
	
	if(MMFGetDataFile((MMUInt16 *)strFileName,&pdata,&lSize))
	{
		pFilehandle = (SKY_FileHandle_t *)MMSeachEmptyHandle();
		memset(pFilehandle,0,sizeof(SKY_FileHandle_t));
		pFilehandle->filetype = SKY_FILE_DATA;
		pFilehandle->pbFile = (kal_uint8 *)strFileName;
		pFilehandle->uRamFileSize= lSize;
		pFilehandle->pbRamBuf= pdata;
		pFilehandle->uRamFileOffset = 0;
		return (MMFile)pFilehandle;
	}
#endif

	if((flag & MM_FILE_READONLY) && (flag & MM_FILE_WRITEONLY))
	{
		return MMNULL;
	}
	if(flag & MM_FILE_READONLY)
	{
		l_iOpenFlag = l_iOpenFlag | FS_READ_ONLY;
	}
	else if(flag & MM_FILE_WRITEONLY)
	{

		l_iOpenFlag = l_iOpenFlag | FS_READ_WRITE;
		
	}
	else if(flag & MM_FILE_READWRITE)
	{
		if(flag & MM_FILE_CREATE)
		{
			l_iOpenFlag = l_iOpenFlag | FS_READ_WRITE | FS_CREATE;
		}
		else
		{
			l_iOpenFlag = l_iOpenFlag | FS_READ_WRITE ;
		}
	}
	else
	{
		return MMNULL;
	}

	l_ifd = FS_Open((WCHAR *)strFileName,l_iOpenFlag);

	if(l_ifd < 0)
	{
		l_iOpenFlag =  l_iOpenFlag | FS_CREATE;
		l_ifd = FS_Open((WCHAR *)strFileName,l_iOpenFlag);
	}
	if(l_ifd < 0)
	{
		return MMNULL;
	}

#ifdef SKY_DATAFILE
	pFilehandle = (SKY_FileHandle_t *)MMSeachEmptyHandle();
	memset(pFilehandle,0,sizeof(SKY_FileHandle_t));
	pFilehandle->filetype = SKY_FILE_NORMAL;
	pFilehandle->hFile = (MMFile)l_ifd;
	return (MMFile)pFilehandle;
#else
	return (MMFile) l_ifd;
#endif


}


MMInt32	MMFRead(MMVoid *pBuf,MMUInt32 nSize,MMUInt32 nCount,MMFile fp)
{
	int l_iReadLength = nSize * nCount;

	UINT l_iHaveReadLength = 0;
	
#ifdef SKY_DATAFILE
	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = fp;
	CHECK_FILE_HANDLE(fp);
	if(pFilehandle->filetype == SKY_FILE_DATA)
	{
		if(pFilehandle->uRamFileOffset < (pFilehandle->uRamFileSize + 1))
		{
			
			if((pFilehandle->uRamFileOffset + l_iReadLength) < pFilehandle->uRamFileSize)
			{
				lSize = l_iReadLength;
			}
			else
				lSize = pFilehandle->uRamFileSize - pFilehandle->uRamFileOffset;
			
			memcpy(pBuf,pFilehandle->pbRamBuf + pFilehandle->uRamFileOffset,lSize);
			pFilehandle->uRamFileOffset += lSize;
			
			return lSize/nSize;
		}
		return 0;
	}
	else if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		int l_iRet =  FS_Read((FS_HANDLE)  pFilehandle->hFile, pBuf, l_iReadLength, &l_iHaveReadLength);
		if(l_iRet < 0)
			return 0;
		
		if(l_iHaveReadLength == l_iReadLength)
		{
			return nCount;
		}else
		{
			return l_iHaveReadLength / nSize;
		}
	}
	else
	{
		return 0;
	}
#else

	int l_iRet =  FS_Read((FS_HANDLE)  fp, pBuf, l_iReadLength, &l_iHaveReadLength);

	if(l_iRet < 0)
	{
		return 0;
	}
	if(l_iHaveReadLength == l_iReadLength)
	{
		return nCount;
	}else
	{
		return l_iHaveReadLength / nSize;
	}
#endif
}

MMInt32	MMFWrite(MMVoid *pBuf,MMUInt32 nSize,MMUInt32 nCount,MMFile fp)
{

	int l_iWriteLength = nSize * nCount;

	UINT l_iHaveWriteLength = 0;
#ifdef SKY_DATAFILE
	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = fp;
	
	CHECK_FILE_HANDLE(fp);
	if(pFilehandle->filetype == SKY_FILE_DATA)
	{
		return 0;
	}
	else if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		int l_iRet =  FS_Write((FS_HANDLE)  pFilehandle->hFile, pBuf, l_iWriteLength, &l_iHaveWriteLength);

		if(l_iHaveWriteLength == l_iWriteLength)
		{
			return nCount;
		}else
		{
			return l_iHaveWriteLength / nSize;
		}
	}
	else
	{
		return 0;
	}
#else
	int l_iRet =  FS_Write((FS_HANDLE)  fp, pBuf, l_iWriteLength, &l_iHaveWriteLength);

	if(l_iHaveWriteLength == l_iWriteLength)
	{
		return nCount;
	}else
	{
		return l_iHaveWriteLength / nSize;
	}
#endif
}
MMInt32 MMFSetSeekHint(MMFile fp, MMUInt32 HintNum, FS_FileLocationHint * Hint)
{
#if 0//def SKY_DATAFILE
    SKY_FileHandle_t *pFilehandle = fp;
    MMInt32 ret  = -1;
    CHECK_FILE_HANDLE(fp);
    if(pFilehandle->filetype == SKY_FILE_NORMAL)
    {
        ret = FS_SetSeekHint((FS_HANDLE)pFilehandle->hFile,HintNum,Hint);
    }   
    sk_dbg_print("MMFSeekHint pos(%d),ret(%d)HintNum(%d) \n",Hint[0].Index,ret,HintNum);
    return ret;

#endif
    return -1;
}//Allan 20120208

MMInt32 MMFSeek(MMUInt32 pos, MMInt32 whence,MMFile fd)
{	
	MMInt32 nWhence;
#ifdef SKY_DATAFILE

	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = fd;
	
	CHECK_FILE_HANDLE(fd);
	if(pFilehandle->filetype == SKY_FILE_DATA)
	{
		if(whence == MM_FILE_SEEK_SET)
		{
			if((pFilehandle->uRamFileSize -1  > pos)&&(pos >= 0))
			{
				pFilehandle->uRamFileOffset = pos;
				return 1;
			}
			else
				return 0;
		}
		
		if(whence == MM_FILE_SEEK_CUR)
		{
			if((pFilehandle->uRamFileSize - 1 > (pFilehandle->uRamFileOffset + pos ))
				&&((pFilehandle->uRamFileOffset + pos )>= 0))
			{
				pFilehandle->uRamFileOffset += pos;
				return 1;
			}
			else
				return 0;
		}
		if(whence == MM_FILE_SEEK_END)
		{
			if((pFilehandle->uRamFileSize - 1 > -pos )&&(-pos >= 0))
			{
				pFilehandle->uRamFileOffset = pFilehandle->uRamFileSize +  pos;
				return 1;
			}
			else
				return 0;
		}
		
		return 0;
	}
	else if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		switch(whence)
		{
		case MM_FILE_SEEK_SET:
			nWhence = FS_FILE_BEGIN;
			break;
		case MM_FILE_SEEK_CUR:
			nWhence = FS_FILE_CURRENT;
			break;
		case MM_FILE_SEEK_END:
			nWhence = FS_FILE_END;
			break;
		default:
			return -1;
		}
		
		return  (FS_Seek((FS_HANDLE )pFilehandle->hFile, pos, nWhence) >= 0) ? 0 : 1;
	}
	else
	{
		return 0;
	}
#else
	switch(whence)
	{
	case MM_FILE_SEEK_SET:
		nWhence = FS_FILE_BEGIN;
		break;
	case MM_FILE_SEEK_CUR:
		nWhence = FS_FILE_CURRENT;
		break;
	case MM_FILE_SEEK_END:
		nWhence = FS_FILE_END;
		break;
	default:
		return -1;
	}
	return  (FS_Seek((FS_HANDLE )fd, pos, nWhence) >= 0) ? 0 : 1;
#endif
}

MMInt32 MMFTell(MMFile fp)
{
	UINT nPos = -1;
#ifdef SKY_DATAFILE
	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = fp;
	
	CHECK_FILE_HANDLE(fp);
	if(pFilehandle->filetype == SKY_FILE_DATA)
	{
		nPos = pFilehandle->uRamFileOffset;
	}
	else if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		FS_GetFilePosition((FS_HANDLE )pFilehandle->hFile,&nPos);
	}
	else
	{
		return 0;
	}
#else
	FS_GetFilePosition((FS_HANDLE )fp,&nPos);
#endif
	return nPos;
}

MMInt32 MMFClose(MMFile fp)
{
#ifdef SKY_DATAFILE
	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = fp;
	
	CHECK_FILE_HANDLE(fp);
	if(pFilehandle->filetype == SKY_FILE_DATA)
	{
		memset(pFilehandle,0,sizeof(SKY_FileHandle_t));
	}
	else if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		if(pFilehandle->hFile)
		{
		FS_Close((FS_HANDLE)pFilehandle->hFile);
		}
		memset(pFilehandle,0,sizeof(SKY_FileHandle_t));
	}
	else
	{
	}
#else
	if(fp)
	{
		FS_Close((FS_HANDLE)fp);
	}
#endif
	return 0;
}

MMInt32 MMSprintf(char *str, const char *format, ...)
{
	MMInt32 nLen = 0;
	va_list args;   
	va_start(args, format);   
	nLen = vsprintf( str,format, args); 
	va_end(args);
	return nLen;
}

MMInt32 MMFprintf(MMFile fp, const char *format, ...)
{
	return 0;
}

MMInt32 MMFFlush (MMVoid *fp)
{
#ifdef SKY_DATAFILE
	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	SKY_FileHandle_t *pFilehandle = fp;
	
	CHECK_FILE_HANDLE(fp);
	
	if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		if(pFilehandle->hFile)
		{
			return FS_Commit((FS_HANDLE )pFilehandle->hFile);
		}
	}
	else
	{
	}
#else
	return FS_Commit((FS_HANDLE )fp);
#endif
}

MMUInt8* MMStrCpy(MMUInt8 *str1, const MMUInt8 *str2)
{
	return (MMUInt8*)strcpy((char*)str1,(char*)str2);
}

MMUInt32 MMStrLen(const MMUInt8 *str)
{
	return strlen((char*)str);
}
#ifdef DSM_MULTIMEDIA_DEBUG2
int g_mallocCnt = 0;
#define MAX_MALLOC_CNT (10*1024)
void *g_mallocPtr[MAX_MALLOC_CNT] = {0};
#endif
MMVoid*	MMMalloc(MMInt32 nSize)
{
	MMInt32 *pRet = (MMInt32*)SKY_MM_MALLOC_CACHEABLE((unsigned int)(nSize+4));
	int cnt = 0;
	
	if(!pRet)
	{
		sk_dbg_print("#####Malloc Memory Failed!!!#####\r\n");
		return MMNULL;
	}
	*pRet++ = nSize;
	MMMemSet(pRet,0,nSize);
    
#ifdef DSM_MULTIMEDIA_DEBUG2
	g_mallocCnt ++;
	while(cnt <MAX_MALLOC_CNT)
	{
		if(g_mallocPtr[cnt] == 0)
		{
			g_mallocPtr[cnt] = pRet;
			break;
		}
		cnt ++;
	}
#endif
	//sk_dbg_print("MMMalloc g_mallocCnt=%d\r\n",g_mallocCnt);
	return pRet;
}

MMVoid	MMFree(MMVoid *pMem)
{
	MMInt32 *pMem1;
	int cnt = 0;
	
	if(!pMem)
		return;
#ifdef DSM_MULTIMEDIA_DEBUG2
	g_mallocCnt--;
	while(cnt <MAX_MALLOC_CNT)
	{
		if(g_mallocPtr[cnt] == pMem)
		{
			g_mallocPtr[cnt] = 0;
			break;
		}
		cnt ++;
	}
#endif
	pMem1 = (MMInt32*)pMem;
	pMem1--;
	SKY_MM_FREE(pMem1);
	//sk_dbg_print("MMFree g_mallocCnt=%d\r\n",g_mallocCnt);
}

MMVoid* MMRealloc(MMVoid* pMem,MMInt32 nSize)
{
	MMInt32 bMTK = 1;
	MMInt32 *pRet,*pMem1;
	MMInt32 nCpySize;
	pRet = (MMInt32 *)SKY_MM_MALLOC_CACHEABLE((unsigned int)(nSize+4));
	if(!pRet)
	{
		sk_dbg_print("#####Malloc Memory NO cacheable!!!");
		pRet = (MMUInt32*)SKY_MM_MALLOC((unsigned int)(nSize+4));
	}
	if(!pRet)
	{
		sk_dbg_print("#####Malloc Memory Failed!!!#####\r\n");
		return MMNULL;
	}
	*pRet++ = bMTK ? nSize : (-nSize);
	if(pMem)
	{
		pMem1 = (MMInt32*)pMem;
		pMem1--;
		nCpySize = *pMem1;
		if(nSize < nCpySize)
		{
			nCpySize = nSize;
		}
		memcpy(pRet,pMem,nCpySize);
		MMFree(pMem);
	}
	return pRet;
}

MMVoid* MMMemSet(MMVoid *pMem,MMInt32 nValue,MMInt32 nSize)
{
	return (MMVoid* ) memset(pMem,nValue,nSize);
}

MMVoid* MMMemCpy(MMVoid *pDst,MMVoid *pSrc,MMInt32 nSize)
{
	return (MMVoid* ) memcpy(pDst,pSrc,nSize);
}

MMVoid* MMMemMove(MMVoid *pDst,MMVoid *pSrc,MMInt32 nSize)
{
	return (MMVoid* ) memmove(pDst,pSrc,nSize);
}

MMInt32 MMMemCmp(const MMVoid *pBuf1, const MMVoid *pBuf2, MMInt32 nSize)
{
	return memcmp(pBuf1,pBuf2,nSize);
}

MMInt32	MMFreeExit(void )
{

}

MMInt32 MMFSize(MMFile fp)
{
	kal_uint32 size;
#ifdef SKY_DATAFILE
	int l_iRet =  0;
	void *pdata = 0;
	int lSize = 0;
	int result =-1;
	
	SKY_FileHandle_t *pFilehandle = fp;
	
	CHECK_FILE_HANDLE(fp);
	if(pFilehandle->filetype == SKY_FILE_DATA)
	{
		size = pFilehandle->uRamFileSize;
		result = FS_NO_ERROR;
	}
	else if(pFilehandle->filetype == SKY_FILE_NORMAL)
	{
		result = FS_GetFileSize((FS_HANDLE)pFilehandle->hFile, &size);
	}
	else
	{
		return 0;
	}
#else
	int result = FS_GetFileSize((FS_HANDLE)fp, &size);
#endif
	if(result == FS_NO_ERROR)
		return size;
	return 0;
}

MMInt32 MMGetRemainMemory(void)
{
	return SKY_MM_LEFT_SIZE();
}


MMVoid* SKY_IRMalloc(MMInt32 nSize)
{
	MMInt32 * pIMem;
	//sk_dbg_print("SKY_IRMalloc");
	nSize  = (nSize+3)>>2;
	pIMem = MMMalloc(nSize<<2);
	return pIMem;
}
MMVoid SKY_IRFree (MMVoid * pMem)
{ 
	MMInt32 * pIMem = pMem;
	//sk_dbg_print("SKY_IRFree");
	MMFree(pIMem);
}

#ifdef SKY_MM_USE_APP_MEM

#include "app_mem.h"
SKY_MM_MEMMGR_t  *pSkyMmMemMgr = 0;
#ifdef __ARM9_MMU__
#define SKYMM_TOTAL_MEMSIZE  (7*1024*1024)
#else
#define SKYMM_TOTAL_MEMSIZE  (1024*1024)
#endif
void SkyMMRamDeinit(void *pHande);
void SKYMM_Stop(void)
{
	sk_dbg_print("SKYMM_Stop \r\n");
	//ASSERT(0);
	SPlayer_Close_Task();
	SkyMMRamDeinit(pSkyMmMemMgr);
	applib_mem_ap_notify_stop_finished(APPLIB_MEM_AP_ID_MYTHROAD, KAL_TRUE);
}
#if 1
MMInt32 SkyMMRamInit(void *pHande)
{
	SKY_MM_MEMMGR_t  *pHMgr = pHande;
	if(!pHMgr)
		return -1;
	sk_dbg_print("MMIntRamInit() pHMgr %d\r\n", pHMgr);
	if(pHMgr->gSkyMMRamCntx.pMemPtr== NULL)
	{
		unsigned long freeSize = 0;
		memset(pHMgr, 0, sizeof(pHMgr));
		
		pHMgr->gSkyMMRamCntx.totalSize = SKYMM_TOTAL_MEMSIZE;
		freeSize = applib_mem_ap_get_max_alloc_size();	
		sk_dbg_print("MMIntRamInit() freeSize %d\r\n", freeSize);
			pHMgr->gSkyMMRamCntx.pMemPtr = applib_asm_alloc_anonymous(SKYMM_TOTAL_MEMSIZE);
		
		if(!pHMgr->gSkyMMRamCntx.pMemPtr)
		{
			return -1;
		}
		pHMgr->gSkyMMRamCntx.leftSize = pHMgr->gSkyMMRamCntx.totalSize;

		pHMgr->pSkyMMRamPtr[0].pMemPtr = pHMgr->gSkyMMRamCntx.pMemPtr;
		pHMgr->pSkyMMRamPtr[0].uSize = 0;
		pHMgr->pSkyMMRamPtr[1].pMemPtr = pHMgr->gSkyMMRamCntx.pMemPtr+pHMgr->gSkyMMRamCntx.totalSize;
		pHMgr->pSkyMMRamPtr[1].uSize = 0;
		sk_dbg_print("MMIntRamInit() success ptr %d totalsize %d\r\n", pHMgr->gSkyMMRamCntx.pMemPtr, pHMgr->gSkyMMRamCntx.totalSize);
	}
	return 0;
}
void SkyMMRamDeinit(void *pHande)
{
	SKY_MM_MEMMGR_t  *pHMgr = pHande;
	if(!pHMgr)
		return ;
	sk_dbg_print("MMIntRamDeinit() ptr %d size %d\r\n", pHMgr->gSkyMMRamCntx.pMemPtr, pHMgr->gSkyMMRamCntx.totalSize);
	if(pHMgr->gSkyMMRamCntx.pMemPtr)
	{
		//applib_mem_ap_free(pHMgr->gSkyMMRamCntx.pMemPtr);
		applib_asm_free_anonymous(pHMgr->gSkyMMRamCntx.pMemPtr);
		//applib_mem_ap_notify_stop_finished(APPLIB_MEM_APP_ID_SKYMM, KAL_TRUE);
	}
	memset(pHMgr->pSkyMMRamPtr, 0, sizeof(pHMgr->pSkyMMRamPtr));
	memset(&pHMgr->gSkyMMRamCntx, 0, sizeof(SKY_INTRAM_CNTX));

}
void* SkyMMRamMalloc(void *pHande,kal_uint32 size)
{
	SKY_MM_MEMMGR_t  *pHMgr = pHande;
	kal_uint8* ptr;
	kal_uint32 i, freeSize;
	
	if(!pHMgr)
		return NULL;
	
	if(size == 0 ||pHMgr->gSkyMMRamCntx.allocCnt >= (SKYMM_MEM_BLOCK_MAX-2))
	{
		sk_dbg_print("size == 0 or allocCnt > 98\r\n");
		return NULL;
	}
	if(pHMgr->gSkyMMRamCntx.pMemPtr == NULL)
	{
		if(SkyMMRamInit(pHande) != 0)
		{
			sk_dbg_print("MMIntRamInit failed\r\n");
			return NULL;
		}
	}
	size = (size +3) & 0xfffffffc;
	for(i=1; i<pHMgr->gSkyMMRamCntx.allocCnt+2; i++)
	{
		ptr = pHMgr->pSkyMMRamPtr[i-1].pMemPtr+pHMgr->pSkyMMRamPtr[i-1].uSize;
		if(pHMgr->pSkyMMRamPtr[i].pMemPtr - ptr > size)
		{
			break;
		}
	}
	if(i >= pHMgr->gSkyMMRamCntx.allocCnt+2)
	{
		sk_dbg_print("Can't find free memory\r\n");
		return NULL;
	}
	//sk_dbg_print("MMIntRamMalloc() p: %d total %d size: %d left: %d cnt %d", ptr, pHMgr->gSkyMMRamCntx.totalSize, size, pHMgr->gSkyMMRamCntx.leftSize, pHMgr->gSkyMMRamCntx.allocCnt);
	memmove(&pHMgr->pSkyMMRamPtr[i+1], &pHMgr->pSkyMMRamPtr[i], (pHMgr->gSkyMMRamCntx.allocCnt+2-i)*sizeof(SKY_MEM_T));
	pHMgr->pSkyMMRamPtr[i].pMemPtr = ptr;
	pHMgr->pSkyMMRamPtr[i].uSize = size;
	pHMgr->gSkyMMRamCntx.allocCnt += 1;
	pHMgr->gSkyMMRamCntx.leftSize -= size;
	return ptr;

}

void SkyMMRamFree(void *pHande,void** ptr)
{
	SKY_MM_MEMMGR_t  *pHMgr = pHande;
	kal_uint32 i, size;
	
	if(!pHMgr)
		return ;
	
	if(!ptr || !(*ptr) || !pHMgr->gSkyMMRamCntx.pMemPtr)
	{
		sk_dbg_print("MMIntRamFree, free NULL!\r\n");
		return;
	}
	//sk_dbg_print("MMIntRamFree() ptr %d total: %d left: %d cnt %d", *ptr, pHMgr->gSkyMMRamCntx.totalSize, pHMgr->gSkyMMRamCntx.leftSize, pHMgr->gSkyMMRamCntx.allocCnt);
	for(i=1; i<pHMgr->gSkyMMRamCntx.allocCnt+1; i++)
	{
		if((*ptr) == pHMgr->pSkyMMRamPtr[i].pMemPtr)
		{
			size = pHMgr->pSkyMMRamPtr[i].uSize;
			break;
		}
	}
	if(i >= pHMgr->gSkyMMRamCntx.allocCnt+1)
	{
		sk_dbg_print("Free invalid memory!\r\n");
		return;
	}
	memmove(&pHMgr->pSkyMMRamPtr[i], &pHMgr->pSkyMMRamPtr[i+1], (pHMgr->gSkyMMRamCntx.allocCnt+1-i)*sizeof(SKY_MEM_T));
	pHMgr->gSkyMMRamCntx.allocCnt -= 1;
	pHMgr->gSkyMMRamCntx.leftSize += size;
	*ptr = NULL;

}
SKY_MM_MEMMGR_t * SKY_MM_GetHandle(SKY_MM_MEM_MOD_t mod)
{
	//sk_dbg_print("SKY_MM_GetHandle() mod %x\r\n", mod);
	switch(mod)
	{
		case SKY_MOD_MED:
			return pSkyMmMemMgr;
		default:
			return NULL;
	}
	
	return NULL;
}

int SKY_MM_SetHandle(SKY_MM_MEM_MOD_t mod,SKY_MM_MEMMGR_t *handle)
{
	
	switch(mod)
	{
		case SKY_MOD_MED:
			pSkyMmMemMgr = handle;
			return 1;
		default:
			return NULL;
	}
	
	return NULL;
}
MMVoid* SKY_Applib_Malloc(SKY_MM_MEM_MOD_t mod,MMInt32 nSize)
{
	return NULL;
}

MMVoid* SKY_Applib_Malloc_cacheable(SKY_MM_MEM_MOD_t mod,MMInt32 nSize)
{
	SKY_MM_MEMMGR_t  *pHMgr = SKY_MM_GetHandle(mod);
	MMInt32 * pIMem;

	if(nSize <= 0)
	{
		sk_dbg_print("SKY_Applib_Malloc() size %d", nSize);
		return NULL;
	}
	//sk_dbg_print("SKY_Applib_Malloc() pHMgr %x\r\n", pHMgr);
	if(!pHMgr)
	{
		pHMgr = med_alloc_ext_mem_cacheable(sizeof(SKY_MM_MEMMGR_t));
		sk_dbg_print("SKY_Applib_Malloc() -1--pHMgr %x\r\n", pHMgr);
		if(pHMgr)
		{
			memset(pHMgr,0,sizeof(SKY_MM_MEMMGR_t));
			SKY_MM_SetHandle(mod,pHMgr);
			SkyMMRamInit(pHMgr);
		}
		else
			return NULL;
	}
	
	nSize  = (nSize+3)>>2;
	nSize += 1;
	pIMem = SkyMMRamMalloc(pHMgr,nSize<<2);
	//sk_dbg_print("SKY_Applib_Malloc() -1--pIMem %x\r\n", pIMem);
	if (pIMem != NULL)
	{
		*pIMem++ = 1;
	} else
	{
		sk_dbg_print("malloc from ext!!\r\n");
		//pIMem = med_alloc_ext_mem_cacheable(nSize<<2);
		//*pIMem++ = 0;
	}
	return pIMem;
}
MMVoid SKY_Applib_Free (SKY_MM_MEM_MOD_t mod,MMVoid * pMem)
{ 
	SKY_MM_MEMMGR_t  *pHMgr = SKY_MM_GetHandle(mod);
	MMInt32 * pIMem = pMem;
	
	if(!pHMgr)
		return ;
	
	//sk_dbg_print("MMIntFree() pMem: %d\r\n", pMem);
	pIMem -= 1;
	if (pIMem[0] == 1) 
	{
	}
	else if (pIMem[0] == 0) 
	{
			sk_dbg_print("Free from ext!!\r\n");
	}
	else
	{
			sk_dbg_print("[SKY_MM] Failed on internal memory free\n");
	}
		SkyMMRamFree(pHMgr,(void**)(&pIMem));
		if(SKY_Applib_MallocCnt(mod) == 0)
		{
			sk_dbg_print("SKY_Applib_Free() -1--\r\n");
			SkyMMRamDeinit(pHMgr);
			med_free_ext_mem((void **)&pHMgr);
			SKY_MM_SetHandle(mod,NULL);
		}
}
int  SKY_Applib_Left (SKY_MM_MEM_MOD_t mod)
{
	SKY_MM_MEMMGR_t  *pHMgr = SKY_MM_GetHandle(mod);
	sk_dbg_print("SKY_Applib_Left() -1-%d-\r\n",pHMgr->gSkyMMRamCntx.leftSize);
	if(!pHMgr)
	{
		switch(mod)
		{
			case SKY_MOD_MED:
				return SKYMM_TOTAL_MEMSIZE;
			default:
				return 0;
		}
	}
	
	return pHMgr->gSkyMMRamCntx.leftSize;
}
int  SKY_Applib_MallocCnt (SKY_MM_MEM_MOD_t mod)
{
	SKY_MM_MEMMGR_t  *pHMgr = SKY_MM_GetHandle(mod);
	
	if(!pHMgr)
		return -1;
	//sk_dbg_print("SKY_Applib_MallocCnt() -1-%d-\r\n",pHMgr->gSkyMMRamCntx.allocCnt);
	return pHMgr->gSkyMMRamCntx.allocCnt;
}
#else
static kal_uint32 appMemCnt = 0;
MMInt32 SkyMMRamInit(void)
{
}
void SkyMMRamDeinit(void *pHande)
{
}
MMVoid* SKY_Applib_Malloc_cacheable(SKY_MM_MEM_MOD_t mod,MMInt32 nSize)
{
	MMInt32 * pIMem;

	
		sk_dbg_print("SKY_Applib_Malloc_cacheable max left %d", applib_asm_get_max_alloc_anonymous());
	nSize  = (nSize+3)>>2;
	pIMem = applib_asm_alloc_anonymous(nSize<<2);
	if(!pIMem)
	{
		sk_dbg_print("malloc Failed!!\r\n");
	}
	return pIMem;
}
MMVoid* SKY_Applib_Malloc(SKY_MM_MEM_MOD_t mod,MMInt32 nSize)
{
	MMInt32 * pIMem;

		sk_dbg_print("SKY_Applib_Malloc max %d", applib_asm_get_max_alloc_anonymous());
	nSize  = (nSize+3)>>2;
	pIMem = applib_asm_alloc_anonymous_nc(nSize<<2);
	if(!pIMem)
	{
		sk_dbg_print("malloc Failed!!\r\n");
	}
	return pIMem;
}
MMVoid SKY_Applib_Free (SKY_MM_MEM_MOD_t mod,MMVoid * pMem)
{ 
	if(pMem)
	{
		applib_asm_free_anonymous(pMem);
	}
	
}
int  SKY_Applib_Left (SKY_MM_MEM_MOD_t mod)
{
	return (int)applib_mem_ap_get_total_left_size();
}
#endif
#endif

#ifdef  __DSM_SPLAYER__
int Skymm_MaxDisplayFrame()
{
	return 12;
}
int Skymm_MinDisplayFrame()
{
	return 7;
}
#endif
#endif


