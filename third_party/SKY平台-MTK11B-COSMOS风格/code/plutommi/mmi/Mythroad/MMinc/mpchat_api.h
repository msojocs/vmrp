
#ifndef __MPCHAT_API_H__
#define __MPCHAT_API_H__

typedef struct vmSizerColorspaceParam_Tag {
	VCVoid * m_pRGBBuffer;
	VCI32   m_iRGBWidth;
    VCI32   m_iRGBHeight;
	VCI32   m_iRGBStride;
    VCVoid * m_pYBuffer;
    VCVoid * m_pUBuffer;
    VCVoid * m_pVBuffer;
    VCI32   m_iWidth;
    VCI32   m_iHeight;
    VCI32   m_iStepLuma;
    VCI32   m_iStepChroma;
} vmSizerColorspaceParam;

VCVoid mr_media_YUVRotation_0 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_YUVRotation_90 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_YUVRotation_180 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_YUVRotation_270 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_RGBRotation_0 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_RGBRotation_90 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_RGBRotation_270 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );
VCVoid mr_media_RGBRotation_180 ( VCU8 * pSrc, 
					  VCU8 * pDst,
		 	          VCI32    uWidth,
		 	          VCI32    uHeight );

VCI32 mr_media_audio_is_recording(VCVoid);

VCVoid mr_media_record_data_ind(VCU16* buf, VCU32 buf_size);

VCI32 mr_media_cam_app(VCVoid);

VCVoid mr_media_send_aud_data(VCU8* pData, VCU32 dataSize);

VCVoid mr_media_cam_GetData_Rsp_Process(VCVoid);

VCI32 mr_media_get_version(VCVoid);

VCI32 mr_media_cam_get_yuvBuf(VCU8* pYUV[3]);

VCI32 VM_SizerAndColorspace(VCVoid * pInput, VCI32 input_len);

VCU8* mr_media_get_audio_buf(VCU32* pSize);
#endif

