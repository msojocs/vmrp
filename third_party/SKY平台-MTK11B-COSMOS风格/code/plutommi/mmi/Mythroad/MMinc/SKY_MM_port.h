
/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/

#ifndef __SKY_MM_PORT_H__
#define __SKY_MM_PORT_H__

#include "SKY_MM_BaseTypes.h"

#ifdef __MED_IN_ASM__
#define SKY_MM_USE_APP_MEM
#endif
//#define  __SKYMM__DISPLAY_IN_MMI__

#define SPLAYER_DUOBLE_BUFFER

#ifdef SKY_MM_USE_APP_MEM

#ifdef __DSM_SPLAYER_RMVB_SUPPORT__
#define SKYMM_MEM_BLOCK_MAX 1000
#else
#define SKYMM_MEM_BLOCK_MAX 500
#endif

typedef struct _memory_used_t_
{
	kal_uint32 uSize;
	kal_uint8*    pMemPtr;
} SKY_MEM_T;
typedef struct _sky_int_ram_cntx_
{
	kal_uint8* pMemPtr;
	kal_uint32 totalSize;
	kal_uint32 leftSize;
	kal_uint32 allocCnt;
} SKY_INTRAM_CNTX;
typedef struct
{
	void *p_MemStart;
	SKY_INTRAM_CNTX gSkyMMRamCntx;
	SKY_MEM_T pSkyMMRamPtr[SKYMM_MEM_BLOCK_MAX];
}SKY_MM_MEMMGR_t;
typedef enum
{
	SKY_MOD_NONE = 0,
	SKY_MOD_MED,
	SKY_MOD_VM,
	
}SKY_MM_MEM_MOD_t;
#endif

#define MM_FILE_CREATE		0x01
#define MM_FILE_READONLY	0x02
#define MM_FILE_WRITEONLY	0x04
#define MM_FILE_READWRITE	0x08

#define MM_FILE_SEEK_SET    0
#define MM_FILE_SEEK_CUR    1
#define MM_FILE_SEEK_END    2

/* Memory debug */
//#define SPLAYER_MEM_DEBUG

typedef struct splayer_port_context
{
	msg_type splayer_open_msgid;
	msg_type splayer_play_msgid;
	msg_type splayer_pause_msgid;
	msg_type splayer_seek_msgid;
	msg_type splayer_stop_msgid;
	msg_type splayer_close_msgid;
	msg_type splayer_get_des_msgid;

	msg_type media_open_msgid;
	msg_type media_play_msgid;
	msg_type media_pause_msgid;
	msg_type media_seek_msgid;
	msg_type media_stop_msgid;
	msg_type media_close_msgid;
	msg_type media_get_des_msgid;

	msg_type media_vid_file_ready_ind;

	kal_int32			mod_multimedia;
	kal_int32			mod_med;
	kal_int32			med_sap;

	kal_int32			med_res_ok;
	kal_int32 		med_res_fail;
	kal_int32			med_res_video_unsupport_decode_size;

	kal_int32			med_mode_file;
	kal_int32			med_mode_array;
	kal_int32			med_mode_pdl_file;

	kal_int32			vid_evt_open;
	kal_int32 			vid_evt_play;
	kal_int32			vid_evt_seek;
	kal_int32			vid_evt_stop;
	kal_int32			vid_evt_close;
	kal_int32			vid_evt_block;
	kal_int32			vid_evt_pause;

	kal_uint32			kal_or;
	kal_uint32			kal_no_wait;

	kal_uint32 splayer_get_frame_msgid;
	kal_uint32 splayer_get_play_time_msgid;
	kal_uint32 splayer_set_stop_time_msgid;
	kal_uint32 splayer_set_sleep_msgid;
	kal_uint32 splayer_time_to_frame_msgid;
	kal_uint32 splayer_frame_to_time_msgid;
	kal_uint32 splayer_get_iframe_msgid;
	kal_uint32 splayer_set_param_msgid;
	kal_uint32 splayer_check_med_ready_msgid;
	kal_uint32 splayer_pause_visual_update_msgid;
	kal_uint32 splayer_resume_visual_update_msgid;

	kal_uint32 media_get_frame_msgid;
	kal_uint32 media_get_play_time_msgid;
	kal_uint32 media_set_stop_time_msgid;
	kal_uint32 media_set_sleep_msgid;
	kal_uint32 media_time_to_frame_msgid;
	kal_uint32 media_frame_to_time_msgid;
	kal_uint32 media_get_iframe_msgid;
	kal_uint32 media_set_param_msgid;
	kal_uint32 media_check_med_ready_msgid;
	kal_uint32 media_pause_visual_update_msgid;
	kal_uint32 media_resume_visual_update_msgid;

	kal_uint32 vid_evt_get_info;
	kal_uint32 vid_evt_get_frame;
	kal_uint32 vid_evt_get_iframe;
	kal_uint32 media_volume_msg;
}splayer_port_context_t;

typedef struct {
   int       src_mod_id;
   int       dest_mod_id;
   int          sap_id;
   int          msg_id;
   void *local_para_ptr;
   void  *peer_buff_ptr;
} ilm_struct_cpy_t;


/*FOR 11B*/
typedef struct
{
    kal_uint64 *cur_time_p;
}
media_vid_get_play_time_req_struct_cpy_t;

typedef struct
{
    kal_uint8 media_mode;
    kal_uint8 media_type;
    void *data;
    kal_uint32 data_size;
    kal_uint16 seq_num;
    med_track_enum open_track;
    mpl_scenario_enum scenario;
    kal_uint8 blocking;
    kal_bool enable_aud_only;   /* allows to open aud only video */

    /* content info */
    kal_uint16 *image_width;
    kal_uint16 *image_height;
    kal_uint64 *total_time;
    kal_int32 *handle;
    kal_uint16 app_id;
    /* file handle */
    kal_int32 *file_handle;      /* FS_HANDLE */

}
media_vid_open_req_struct_cpy_t;

typedef struct
{
    kal_uint8 media_mode;   
    kal_uint8 media_type;            
    void *data;                   
    kal_uint16 display_width;
    kal_uint16 display_height;
    void *image_buffer_p;
    kal_int16 zoom_factor;   
    kal_int16 frame_rate;      
    kal_int16 contrast;     
    kal_int16 brightness;  
    kal_int16 play_speed;
    kal_uint8 play_audio;
    kal_uint8 display_device;
    kal_uint8 audio_path;
    kal_uint16 seq_num;
    kal_uint16 repeats;   

    kal_uint8 lcd_id;
    kal_uint32 update_layer;
    kal_uint32 hw_update_layer;
    kal_uint8 image_data_format;            /* IMGDMA_IBW_OUTPUT_RGB565 or IMGDMA_IBW_OUTPUT_RGB888 */
    
    kal_bool force_lcd_hw_trigger;
    kal_uint16 idp_rotate;    
    kal_uint32 layer_element;
    kal_uint32 blt_ctx;
    kal_uint32 blt_dev;
}
media_vid_play_req_struct_cpy_t;
typedef struct
{
    kal_uint8 get_frame;
    kal_uint32 frame_num;
    kal_uint64 time;
    kal_uint16 display_width;
    kal_uint16 display_height;
    void *image_buffer_p;
    kal_uint8 blocking;
    kal_uint16 seq_num;
    kal_bool force_seek;
    med_track_enum seek_track; /* Only for streaming currently */
    kal_int16 idp_rotate;

    kal_uint8 image_data_format;            /* IMGDMA_IBW_OUTPUT_RGB565 or IMGDMA_IBW_OUTPUT_RGB888 */
    
}
media_vid_seek_req_struct_cpy_t;
typedef struct
{
    kal_uint16 display_width;
    kal_uint16 display_height;
    void *image_buffer_p;
    kal_int16 idp_rotate;
    kal_uint8 image_data_format;
    med_frame_enum frame_mode;
}
media_vid_get_frame_req_struct_cpy_t;

/*END*/
unsigned int SPlayer_getSysTime(void);
void SPlayer_Get_LCDSize(kal_uint32* width, kal_uint32* height, kal_uint32* pFormat);
void splayer_register_pen_down_handler(mmi_pen_hdlr pen_down_hdlr);
void splayer_register_pen_up_handler(mmi_pen_hdlr pen_up_hdlr);

void SKSleep(int time);
unsigned int get_cur_sys_time();

typedef MMVoid*	MMFile;
MMFile	MMWFOpen(MMUInt16 *strFileName,MMUInt32 flag);
MMFile	MMFOpen(MMUInt8 *strFileName,MMUInt32 flag);
MMInt32	MMFRead(MMVoid *pBuf,MMUInt32 nSize,MMUInt32 nCount,MMFile fp);
MMInt32	MMFWrite(MMVoid *pBuf,MMUInt32 nSize,MMUInt32 nCount,MMFile fp);
MMInt32 MMFSeek(MMUInt32 pos, MMInt32 whence,MMFile fd);
MMInt32 MMFTell(MMFile fp);
MMInt32 MMFClose(MMFile fp);
MMInt32 MMSprintf(char *, const char *, ...);
MMInt32 MMFprintf(MMVoid *, const char *, ...);
MMInt32 MMFFlush (MMVoid *);
MMUInt8* MMStrCpy(MMUInt8 *, const MMUInt8 *);
MMUInt32 MMStrLen(const MMUInt8 *);
MMInt32 MMFSize(MMFile fp);

MMVoid* MMMalloc (MMInt32 nSize);
MMVoid	MMFree (MMVoid *pMem);
MMVoid* cMMMalloc (MMInt32 nSize);
MMVoid	cMMFree (MMVoid *pMem);
MMVoid* MMRealloc (MMVoid* pMem,MMInt32 nSize);
MMVoid* MMMemSet(MMVoid *pMem,MMInt32 nValue,MMInt32 nSize);
MMVoid* MMMemCpy(MMVoid *pDst,MMVoid *pSrc,MMInt32 nSize);
MMVoid* MMMemMove(MMVoid *pDst,MMVoid *pSrc,MMInt32 nSize);
MMInt32 MMMemCmp(const MMVoid *pBuf1, const MMVoid *pBuf2, MMInt32 nSize);
MMInt32 MMFreeExit(void);
MMVoid* MMSDKMalloc (MMInt32 nSize);
MMVoid* MMSDKRealloc (MMVoid* pMem,MMInt32 nSize);
MMVoid	MMSDKFree (MMVoid *pMem);

MMInt32 MMGetRemainMemory(void);


#ifdef SKY_MM_USE_APP_MEM

MMVoid* SKY_Applib_Malloc_cacheable(SKY_MM_MEM_MOD_t mod,MMInt32 nSize);
MMVoid* SKY_Applib_Malloc(SKY_MM_MEM_MOD_t mod,MMInt32 nSize);
MMVoid SKY_Applib_Free (SKY_MM_MEM_MOD_t mod,MMVoid * pMem);

#define SKY_MM_MALLOC(nSize) SKY_Applib_Malloc(SKY_MOD_MED,nSize)
#define SKY_MM_MALLOC_CACHEABLE(nSize) SKY_Applib_Malloc_cacheable(SKY_MOD_MED,nSize)
#define SKY_MM_FREE(pMem) SKY_Applib_Free(SKY_MOD_MED,pMem)
#define SKY_MM_LEFT_SIZE()  SKY_Applib_Left(SKY_MOD_MED)
#else
#define SKY_MM_MALLOC(nSize) med_alloc_ext_mem(nSize)
#define SKY_MM_MALLOC_CACHEABLE(nSize) med_alloc_ext_mem_cacheable(nSize)
#define SKY_MM_FREE(pMem) med_free_ext_mem((void **)&pMem)
#define SKY_MM_LEFT_SIZE()  med_ext_left_size()
#endif
#endif

