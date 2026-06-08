/*
** 
** Copyright (C) 2008-2009 SKY-MOBI AS.  All rights reserved.
**
*/

#ifndef __SPLAYER_API_H__
#define __SPLAYER_API_H__


/* Status */
typedef enum {
	SVP_OK = 0,
	SVP_MEM_ERR,
	SVP_FILE_ERR,
	SVP_FILE_END,
	SVP_FAIL
} SVP_STATUS;

typedef enum {
	SPLAYER_IDLE,
	SPLAYER_OPENING,
	SPLAYER_PLAY,
	SPLAYER_FULL_PLAY,
	SPLAYER_PAUSE,
	SPLAYER_STOP,

	SPLAYER_INVALID
} SPLAYER_STATE;




/* Information */
typedef struct {	
	/* Audio Info */	
	unsigned int uChannels;	
	unsigned int uSampleRate;	
	unsigned int uSampleSize;	
	unsigned int uABitRate;	
	unsigned int uATotalTime;	
	unsigned int uAFormat;	
	/* Video Info */	
	unsigned int uWidth;	
	unsigned int uHeight;	
	unsigned int uFPS;	
	unsigned int uVBitRate;	
	unsigned int uVTotalTime;	
	unsigned int uVFormat;
} SVP_AVInfo;

/* Handle */
typedef struct {	
	unsigned int uFileHandle;
} SVP_Handle;

typedef struct {
	unsigned char media_mode;
	unsigned char media_type;
	unsigned short seq_num;
	
	unsigned short *image_width;
	unsigned short *image_height;
	unsigned int *total_time;

	void *data;
	unsigned int data_size;
	int *handle;

	unsigned short max_width;
	unsigned short max_height;
	
	unsigned char blocking;
	unsigned char play_audio; 

} SPLAYER_VID_OPEN_REQ_ST;

typedef struct
{
    void *image_buffer_p;
    void *data;                   
    unsigned int update_layer;
    unsigned int hw_update_layer;
    unsigned int layer_element;
    unsigned int blt_ctx;
    unsigned int blt_dev;
    unsigned short display_width;
    unsigned short display_height;
    unsigned short idp_rotate;    
    unsigned short seq_num;
    unsigned short repeats;   
    short zoom_factor;   
    short frame_rate;      
    short contrast;     
    short brightness;  
    short play_speed;
    unsigned char play_audio;
    unsigned char display_device;
    unsigned char audio_path;
    unsigned char media_mode;   
    unsigned char media_type;            

    unsigned char lcd_id;
    unsigned char image_data_format;            /* IMGDMA_IBW_OUTPUT_RGB565 or IMGDMA_IBW_OUTPUT_RGB888 */
    
    unsigned char force_lcd_hw_trigger;
} SPLAYER_VID_PLAY_REQ_ST;

typedef struct
{
    unsigned int time;
    
    void *image_buffer_p;
    unsigned int frame_num;
    unsigned short display_width;
    unsigned short display_height;
    unsigned short seq_num;
    short idp_rotate;

    unsigned char seek_track; /* Only for streaming currently */
    unsigned char force_seek;
    unsigned char blocking;
    unsigned char get_frame;
    unsigned char image_data_format;            /* IMGDMA_IBW_OUTPUT_RGB565 or IMGDMA_IBW_OUTPUT_RGB888 */
} SPLAYER_VID_SEEK_REQ_ST;

typedef struct
{
    unsigned short  display_width;
    unsigned short  display_height;
    void *image_buffer_p;
    unsigned short  idp_rotate;
    unsigned char  image_data_format;
    unsigned char  frame_mode;
} SPLAYER_VID_GET_FRAME_REQ_ST;

typedef struct {
	int 		isRun;
	int 		state;
	SVP_Handle 	pHdl;
	SPLAYER_VID_OPEN_REQ_ST  open_req_st;
	SPLAYER_VID_PLAY_REQ_ST   play_req_st;
	SPLAYER_VID_SEEK_REQ_ST   seek_req_st;
	SPLAYER_VID_GET_FRAME_REQ_ST getFrame_req_st;
} SPLAYER_CONTEXT;

typedef enum rmvb_wait_event_enum {
	SPLAYER_EVT_IDLE = 1<<1,
	SPLAYER_EVT_OPEN = 1<<2,
	SPLAYER_EVT_PLAY = 1<<3,
	SPLAYER_EVT_STOP = 1<<4,
	SPLAYER_EVT_SEEK = 1<<5,
	SPLAYER_EVT_PAUSE= 1<<6,
	SPLAYER_EVT_CLOSE= 1<<7
} rmvb_wait_event_enum;
typedef enum rmvb_result_enum {
	SPLAYER_FAIL = -1,
	SPLAYER_OKAY =  0,
	SPLAYER_BUSY =  1,
	SPLAYER_NULL =  2,
	SPLAYER_SIZE_NOSUPPORT =  3,
	SPLAYER_BAD_FILE =  4
} rmvb_result_enum;

typedef enum rmvb_state_enum {
	SPLAYER_STATE_IDLE = 0,
	SPLAYER_STATE_PLAYING = 1,
	SPLAYER_STATE_GETFRAME = 2
} rmvb_state_enum;

extern SPLAYER_CONTEXT gSPlayerContext;

void SPLAYER_WAIT_EVENT(int evt);

void SPLAYER_SET_EVENT(int evt);

/* AUDIO DECODE TASK */
unsigned int   SKYRMVB_DecodeAudio_Task(unsigned char * pData, unsigned int size);


/* GET AV INFORMATION */
SVP_STATUS   SPlayer_GetAVInfo(SVP_Handle * pHandle, SVP_AVInfo * pAVInfo);

/* CHECK FILE TYPE */
SVP_STATUS   SPlayer_CheckFileType (unsigned short * pFileName);

/* SET DISPLAY SIZE */
SVP_STATUS   SPlayer_SetDispRegion (unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short rotation);

/* GET CURRENT TIME */
SVP_STATUS   SPlayer_GetCurrentTime(SVP_Handle * pHandle, unsigned int * pTime);

kal_uint32 SPlayer_IsStatePlaying(void);

kal_uint32 SPlayer_IsAudioPlaying(void);

int Splayer_GetColorSpace(void);
int Splayer_SetColorSpace(int colorspace);

typedef struct {
   kal_uint8       *rb_base;         // Pointer to the ring buffer
   kal_int32       rb_size;          // Size of the ring buffer
   kal_int32       write;
   kal_int32       read;
} SkyMMAUD_RB_INFO;
void SkymmSetBuffer(SkyMMAUD_RB_INFO *rb  ,kal_uint8 *buffer, kal_uint32 buf_len );
void SkymmrbGetWriteBuffer( SkyMMAUD_RB_INFO *rb, kal_uint8 **buffer, kal_uint32 *buf_len );
void SkymmrbGetReadBuffer( SkyMMAUD_RB_INFO *rb, kal_uint8 **buffer, kal_uint32 *buf_len );// in bytes
void SkymmrbReadDataDone( SkyMMAUD_RB_INFO *rb, kal_uint32 len ); // in bytes
void SkymmrbWriteDataDone( SkyMMAUD_RB_INFO *rb, kal_uint32 len ); // in bytes
int SkymmrbGetFreeSpace( SkyMMAUD_RB_INFO *rb );
int SkymmrbGetDataCount( SkyMMAUD_RB_INFO *rb );
void SkymmResetMediaBuf( SkyMMAUD_RB_INFO *rb );
SVP_STATUS splayer_audioAppend(SVP_Handle* handle);
#endif //__SKY_RMVB_API__
