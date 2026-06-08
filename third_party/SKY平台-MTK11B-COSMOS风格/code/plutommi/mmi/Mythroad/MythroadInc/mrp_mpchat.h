// SKY_MULTIMEDIA 
#ifdef __MMI_DSM_NEW__
#ifndef _MRP_MPCHAT_H_
#define _MRP_MPCHAT_H_

#include "MMIDataType.h"

#include "mrporting.h"
#include "mrp_extension.h"
#include "mrp_features.h"

#include "l1audio.h"

#include "gdi_datatype.h"
#include "mdi_datatype.h"
#include "lcd_if.h"
#include "mdi_audio.h"
#include "med_utility.h"

#include "mrp_stream_audio.h"

#include "ScreenRotationGprot.h"//Allan 20120217
#define MPC_SCREEN_NOT_SUPPORT_ROTATE     (0x01<<4)
#define MPC_SCREEN_SUPPORT_SELF_ROTATE     (0x01<<2)//Allan 20120217

#define MPC_KEYBOARD_NO_NUM_KEY     (0x01<<4)
#define MPC_KEYBOARD_NO_OK_KEY     (0x01<<5)

/////////////////////////////////////////////////////////////////////////////////////////////

#define MR_AUD_REC_DATA_IND 	1000
#define MR_VIDEO_ENC_DATA_IND 	1001
#define MR_VIDEO_DEC_RES_IND 	1002

/* MPCHAT OTA CODE */
#define MR_MPCHAT_MSGID_BEGIN       0x30000
#define MR_MPCHAT_MSGID_END         0x3FFFF
#define MR_MPCHAT_GET_VESION 		0x38030
#define SKY_VMMEDIA_GET_PLAT_INFO 		0x31081
#define SKY_VMMEDIA_INTSRAM_MALLOC 	0x32020
#define SKY_VMMEDIA_INTSRAM_FREE		0x32021
#define SKY_VMMEDIA_STACK_FUNCCALL 	0x32022
#define SKY_VMMEDIA_DISPLAY_FREME 	0x32001

#define MR_MPCHAT_AUDIO_PLAY_START 	0x38020
#define MR_MPCHAT_AUDIO_PLAY_APPEND 0x38021
#define MR_MPCHAT_AUDIO_PLAY_STOP 	0x38022
#define MR_MPCHAT_AUDIO_SET_VOLUME 	0x38023
#define MR_MPCHAT_AUDIO_PLAY_PAUSE 	0x38024
#define MR_MPCHAT_AUDIO_PLAY_RESUME 	0x38025
#define SKY_VMMEDIA_AUD_GET_CURRTIME 	0x38026 // 0x32010

#define MR_MPCHAT_SCREEN_INFO 0x38031
#define MR_MPCHAT_KEYBOARD 	0x38032

#define MR_MPCHAT_MPLIVE_FULLSCREEN 	0x38088  //Allan 20120217

typedef struct{
	int32 width;
	int32 height;
}MPCRotateInfo; //Allan 20120217
// ENCODER
#define SKY_VMMEDIA_ENCODER_OPEN 	0x31030
#define SKY_VMMEDIA_ENCODER_CLOSE 	0x31031
#define SKY_VMMEDIA_ENCODER_ENCODE 	0x31032


#define MR_MPCHAT_IMAGE_SIZERCC     0x38011
#define MR_MPCHAT_IMAGE_FILTER      0x38012
#define MR_MPCHAT_IMAGE_DECODE      0x38013
#define MR_MPCHAT_GET_CAMERA_NUM 	0x38014
#define MR_MPCHAT_SWITCH_CAMERA 	0x38015
#define MR_MPCHAT_SWITCH_NIGHT 	0x38016

#define MR_MPCHAT_CAMERA_START      0x38000
#define MR_MPCHAT_CAMERA_STOP       0x38001
#define MR_MPCHAT_CAMERA_GETIMAGE   0x38002
#define MR_MPCHAT_ENCODER_GETBITS   0x38003
#define MR_MPCHAT_ENCODER_FORCEIDR  0x38004
#define MR_MPCHAT_DECODER_OPEN      0x38005
#define MR_MPCHAT_DECODER_CLOSE     0x38006
#define MR_MPCHAT_DECODER_DECODE    0x38007
#define MR_MPCHAT_AUDIO_START       0x38008
#define MR_MPCHAT_AUDIO_STOP        0x38009
#define MR_MPCHAT_AUDIO_PLAY        0x38010


#define D_VM_AUDIO_PCM8 		(1)
#define D_VM_AUDIO_PCM16 		(1<<1)
#define D_VM_AUDIO_WAV 		(1<<2)		
#define D_VM_AUDIO_AMR 		(1<<3)		
#define D_VM_AUDIO_AMR_IF2 	(1<<4)		
#define D_VM_AUDIO_G723 		(1<<5)		
#define D_VM_AUDIO_G726_64 	(1<<6)	
#define D_VM_AUDIO_G711_A 		(1<<7)	
#define D_VM_AUDIO_G711_U 		(1<<8)
#define D_VM_AUDIO_IMA 		(1<<9)
#define D_VM_AUDIO_DIVX 		(1<<10)
#define D_VM_AUDIO_MP3 		(1<<11)
#define D_VM_AUDIO_AC3 		(1<<12)
#define D_VM_AUDIO_AAC 		(1<<13)
#define D_VM_AUDIO_DTS 		(1<<14)
#define D_VM_AUDIO_VORBIS 		(1<<15)

#define D_VM_IMAGE_RGB565		(1)
#define D_VM_IMAGE_RGB888		(1<<1)
#define D_VM_IMAGE_YUV420		(1<<2)
#define D_VM_IMAGE_YUV422 		(1<<3)
#define D_VM_VIDEO_JPG 			(1<<4)
#define D_VM_VIDEO_3GP 		(1<<5)
#define D_VM_VIDEO_MP4 		(1<<6)

#define D_VM_VIDEO_JPEG		(1)
#define D_VM_VIDEO_H263 		(1<<1)
#define D_VM_VIDEO_H264 		(1<<2)
#define D_VM_VIDEO_FLV1		(1<<3)
#define D_VM_VIDEO_MPEG4 		(1<<4)

#define D_VM_DEMUX_AVI 		(1)
#define D_VM_DEMUX_3GP 		(1<<1)
#define D_VM_DEMUX_MP4 		(1<<2)
#define D_VM_DEMUX_MOV 		(1<<3)
#define D_VM_DEMUX_RM 			(1<<4)
#define D_VM_DEMUX_RMVB 		(1<<5)
#define D_VM_DEMUX_FLV 		(1<<6)

typedef enum {
	VM_IMAGE_UNKNOWN = -1,
		VM_IMAGE_RGB565,
		VM_IMAGE_RGB888,
		VM_IMAGE_YUV420,
		VM_IMAGE_YUV422,
		VM_VIDEO_JPG,
		VM_VIDEO_3GP,
		VM_VIDEO_MP4,
} VMImageFormat;

typedef enum {
		VM_VIDEO_UNKNOWN = -1,	
		VM_VIDEO_H264,
		VM_VIDEO_H263,
		VM_VIDEO_FLV1,
		VM_VIDEO_MPEG4,
		VM_VIDEO_JPEG,
} VMVideoCodecType;

typedef enum {
	VM_DEMUX_UNKNOWN = -1,
		VM_DEMUX_AVI,
		VM_DEMUX_3GP,
		VM_DEMUX_MP4,
		VM_DEMUX_MOV,
		VM_DEMUX_RM,
		VM_DEMUX_RMVB,
		VM_DEMUX_FLV,
} VMDemuxerFormat;

typedef enum {
	VM_ROTATION_0 = 0,
	VM_ROTATION_90,
	VM_ROTATION_180,
	VM_ROTATION_270,
} VMImageRotation;

typedef struct VMYUVBitmap_Tag {
		uint32 		format;
		uint8 * 	pPlane[3];
		int32   	nWidth;
		int32   	nHeight;
		int32  	nLumaStride; 	//亮度BUFFER步长
		int32   	nChmaStride; 	//色度BUFFER步长
} VMYUVBitmap;


typedef struct VMPlatInfo_Tag {
		uint32 	m_uAudRecFormat;
		uint32 	m_uAudPlayFormat;
		uint32 	m_uVidRecFormat;
		uint32 	m_uVidDecFormat;
		uint32 	m_uVidEncFormat;
		uint32 	m_uDemuxFormat;
		uint32 	m_uVMVersion;
} VMPlatInfo;



typedef struct VMMediaDisp_s_tag{
			uint32 format;
			void * img;
			int32 x;
			int32 y; 
			int32 width;
			int32 height;
			uint32 rotation;
}VMMediaDisp_S;

typedef struct VMRGBBitmap_Tag {
		uint32		format;
		uint16 * 	pBuffer;
		int32   	nWidth;
		int32   	nHeight;
		int32   	nImgStride; 		//图像BUFFER步长
} VMRGBBitmap;

typedef struct VMMediaEncCamData_tag
{
    uint8* pData;
    uint32 nDataSize;
    uint32 nSrc;
    uint32 nState;
} VMMediaEncCamData;

/* 启动录音设备 */
typedef struct audioStart_Tag {
	uint32    m_uInterval; // ms
	uint32    m_uFormat;
} audioStart;
/* 播放声音 (AMR-WB) */
typedef struct audioPlay_Tag {
	uint8 * m_pBuffer;
	uint32    m_uLength;
	uint32    m_uFormat;
} audioPlay;

void* VCMemset (void * pMem, int32 iData, int32 iSize);
void* VCMalloc (int32 iSize);
void* VCIMalloc (int32 iSize);
void VCFree (void * pMem);
void VCIFree (void * pMem);

int32 mr_media_get_version(void);
int32 mr_media_getPlatInfo(uint8** output, int32* output_len);
void mr_media_fast_call(void (*pFunc)(void *pParam),void *pParam);

int splayer_getLayerindex(gdi_handle handlecur);

int32 mr_media_LCD_Display(void* input, int input_len);

int32 VM_Media_Aud_Get_CurrTime(uint8** output, int32* output_len);
char mr_media_get_root_drive(void);
int32 MPC_AudioStart(audioStart * pInput, uint32 input_len);
int32 MPC_AudioStop(audioStart * pInput, uint32 input_len);

int32 MPC_AudioPlayAppendData(audioPlay * pInput, uint32 input_len, uint8** output, int32* output_len);
int32 MPC_AudioPlayOpen(audioPlay * pInput, uint32 input_len, uint8** output, int32* output_len);
void mr_media_timer_start(int32 count, void (*callback) (void));
int32 MPC_AudioPlayClose(uint8 * pInput, uint32 input_len);
int32 MPC_AudioPlayPause(void);
int32 MPC_AudioPlayResume(void);
int32 mr_media_get_screen_info(void);
int32 mr_media_get_keyboard_info(void);


typedef struct VC_H264DecParam_Tag {
	uint32 m_uWidth;
	uint32 m_uHeight;
	uint32 m_uLCDWidht;
	uint32 m_uLCDHeight;
	uint32 m_uLCDFormat;
} VC_H264DecParam;

/*
 * Initialize h264 decoder  
 */
void * VC_H264Decoder_Open(VC_H264DecParam * pParam);

/*
 * Bitstream decoder 
 */
int VC_H264Decoder_Decode(void * pHandle,        // pointer to h264 handle
                                uint8      * pStream,        // stream buffer
                                uint32       uSize,          // stream buffer size
                                uint32     * pUsedSize,      // consumed bytes by decoder
                                uint32       uLayerWidth,    // layer width
                                uint32       uLayerHeight,   // layer height
                                uint8      * pLayerBuffer ); // layer buffer, only RGB565 is supported

/* 
 * Close h264 decoder 
 */
int VC_H264Decoder_Close(void * pHandle);


typedef struct VC_H264EncParam_Tag {
	uint32 m_uWidth;
	uint32 m_uHeight;
	uint32 m_uFPS;
	uint32 m_uInitQP;
	uint32 m_uIntraPeriod;
	uint32 m_uRCFlag;
	uint32 m_uRCBitrate;
} VC_H264EncParam;

/*
 * Initialize h264 decoder  
 */
void * VC_H264Encoder_Open(VC_H264EncParam * pParam);

/*
 * Bitstream decoder 
 */
int VC_H264Encoder_Encode(void * pHandle,        // pointer to h264 handle
                                uint8      * pYBuffer,
								uint8      * pUBuffer,
								uint8      * pVBuffer,
								uint32     * pSize,
								uint8      * pStream,
								uint32     * pKeyFrameFlag,
								uint32       uForceKeyFrame);

/* 
 * Close h264 decoder 
 */
int VC_H264Encoder_Close(void * pHandle);

typedef struct vmSizerColorspaceParam_Tag {
	void * m_pRGBBuffer;
	int32   m_iRGBWidth;
    int32   m_iRGBHeight;
	int32   m_iRGBStride;
    void * m_pYBuffer;
    void * m_pUBuffer;
    void * m_pVBuffer;
    int32   m_iWidth;
    int32   m_iHeight;
    int32   m_iStepLuma;
    int32   m_iStepChroma;
} vmSizerColorspaceParam;

void mr_media_YUVRotation_0 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_YUVRotation_90 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_YUVRotation_180 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_YUVRotation_270 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_RGBRotation_0 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_RGBRotation_90 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_RGBRotation_270 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );
void mr_media_RGBRotation_180 ( uint8 * pSrc, 
					  uint8 * pDst,
		 	          int32    uWidth,
		 	          int32    uHeight );

int32 mr_media_audio_is_recording(void);

void mr_media_record_data_ind(uint16* buf, uint32 buf_size);

int32 mr_media_cam_app(void);

void mr_media_send_aud_data(uint8* pData, uint32 dataSize);

void mr_media_cam_GetData_Rsp_Process(void);

int32 mr_media_get_version(void);

int32 mr_media_cam_get_yuvBuf(uint8* pYUV[3]);

int32 VM_SizerAndColorspace(void * pInput, int32 input_len);

uint8* mr_media_get_audio_buf(uint32* pSize);
#endif//_MRP_MPCHAT_H_
#endif

