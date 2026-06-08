/*
 * Header file for H264 decoder API prototype
 */
#ifndef __SKY_MM_PLATFORM_API_H__
#define __SKY_MM_PLATFORM_API_H__

#include "sky_mm_VC_BaseTypes.h"

#define MR_AUD_REC_DATA_IND 	1000
#define MR_VIDEO_ENC_DATA_IND 	1001
#define MR_VIDEO_DEC_RES_IND 	1002

typedef enum {
		VM_BT_NONE,		//没有蓝牙设备
		VM_BT_OPEN,		//蓝牙设备已打开
		VM_BT_CLOSE,		//蓝牙设备已关闭
} VMBlueToothStatus;

typedef struct VMSTACKCALL_TAG {
	VCVoid (*pFunc)(VCVoid *pParam);
	VCVoid *pParam;
}VMStackCall_T;

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
		VCU32 		format;
		VCU8 * 	pPlane[3];
		VCI32   	nWidth;
		VCI32   	nHeight;
		VCI32  	nLumaStride; 	//亮度BUFFER步长
		VCI32   	nChmaStride; 	//色度BUFFER步长
} VMYUVBitmap;

typedef struct VMRGBBitmap_Tag {
		VCU32		format;
		VCU16 * 	pBuffer;
		VCI32   	nWidth;
		VCI32   	nHeight;
		VCI32   	nImgStride; 		//图像BUFFER步长
} VMRGBBitmap;

typedef struct VMMediaDisp_s_tag{
			VCU32 format;
			VCVoid * img;
			VCI32 x;
			VCI32 y; 
			VCI32 width;
			VCI32 height;
			VCU32 rotation;
}VMMediaDisp_S;

typedef struct VMMediaEncCamData_tag
{
    VCU8* pData;
    VCU32 nDataSize;
    VCU32 nSrc;
    VCU32 nState;
} VMMediaEncCamData;


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

typedef struct VMPlatInfo_Tag {
		VCU32 	m_uAudRecFormat;
		VCU32 	m_uAudPlayFormat;
		VCU32 	m_uVidRecFormat;
		VCU32 	m_uVidDecFormat;
		VCU32 	m_uVidEncFormat;
		VCU32 	m_uDemuxFormat;
		VCU32 	m_uVMVersion;
} VMPlatInfo;

VCVoid* VCMalloc  (VCI32   iSize);
VCVoid	VCFree    (VCVoid * pMem);
VCVoid* VCRealloc (VCVoid * pMem, VCI32   iSize);
VCVoid* VCMemset  (VCVoid * pMem, VCI32   iData, VCI32 iSize);
VCVoid* VCMemcpy  (VCVoid * pDst, VCVoid * pSrc, VCI32 iSize);
VCVoid* VCMemmove (VCVoid * pDst, VCVoid * pSrc, VCI32 iSize);

VCVoid mr_media_cam_YUVRotation ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_cam_RGBRotation ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_cam_init_callback(VCVoid);
VCI32 mr_media_cam_get_num(VCVoid);
VCI32 mr_media_cam_CamsSwitch(VCU8 set);
VCI32 mr_media_cam_nightProf_switch(VCU8* input, VCI32 input_len);
char mr_media_get_root_drive(VCVoid);
VCI32 mr_media_get_screen_info(VCVoid);
VCI32 mr_media_get_keyboard_info(VCVoid);
VCI32 mr_media_cam_get_dataType(VCVoid);

#if defined(__DSM_CAM_ENC_SUPPORT__ ) 
VCVoid mr_media_init_enc_callback(VCVoid);
VCVoid mr_media_H264Enc_fastCall(VC_Handle * pHandle,  VCU8 * p1, VCU8 * p2,
 								VCU8 * p3, VCU32 * p4, VCU8 * p5, VCU32 * p6, VCU32 p7);
VCVoid mr_media_Send_Enocde_REQ(VCVoid* buffer, VCU32 len, VCU8 state);
VCVoid mr_media_Send_Enocde_CNF(VCU8 result);
VCVoid mr_media_Encode_req_hdlr(VCVoid* pMsg);
VCVoid mr_media_H264Enc_fastCall(VC_Handle * pHandle,  VCU8 * p1, VCU8 * p2,
 								VCU8 * p3, VCU32 * p4, VCU8 * p5, VCU32 * p6, VCU32 p7);
#endif

VCI32 mr_media_cam_preview_start(VCU32 width, VCU32 height);
VCI32 mr_media_cam_preview_stop(VCVoid);
VCI32 mr_media_cam_GetData(void);
VCI32 mr_media_cam_DataRsp(VCU8* pData, VCU32 nDataSize);

VCI32 mr_media_LCD_Display(VCVoid* input, VCI32 input_len);
VCI32 mr_media_resizer_and_colorspace(VCVoid* input, VCI32 input_len);
VCI32 mr_media_resizer_close(VCVoid);

VCVoid mr_media_fast_call(VCVoid (*pFunc)(VCVoid *pParam),VCVoid *pParam);
VCVoid mr_media_timer_start(VCI32 count, VCVoid (*callback) (VCVoid));

VCI32 mr_media_get_avbt_state(VCVoid);
VCI32 mr_media_avbt_open(VCVoid);
VCI32 mr_media_avbt_close(VCVoid);

VCI32 mr_media_getPlatInfo(VCU8** output, VCI32* output_len);


#endif

