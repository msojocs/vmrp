

#ifndef _MPCHAT_APP_H_
#define _MPCHAT_APP_H_

#ifndef __MMI_DSM_MEDIA_EXT__

typedef struct
{
    uint8   handle;
    uint8*       input;
}decode_input_struct;


/* 启动摄像机 */
typedef struct cameraStart_Tag {
    uint32    m_uWidth;
    uint32    m_uHeight;
    uint32    m_uFPS;
    uint32    m_uInitQP;
    uint32    m_uIntraPeriod;
    uint32    m_uRCFlag;
    uint32    m_uRCBitrate;
    uint32    m_uMediaType;  //uint8 * m_pMemory;
    uint32    m_uReserve;      //m_uMemLength;
} cameraStart;

/* 获取摄像机图像 */
typedef struct cameraGetImage_Tag {
    uint8 * m_pRGBBuffer;
	uint32    m_uFormat; // 565, 666, 24, yuv420
    uint32    m_uWidth;
    uint32    m_uHeight;
} cameraGetImage;

/* 获取编码后的码流参数 */
typedef struct encoderGetBits_Tag {
    uint8 **m_pBuffer;
	uint32  * m_uLength;
	uint32 *m_uIsKeyFrame;
} encoderGetBits;

typedef struct encoderDataInd_Tag {
    uint8 *m_pBuffer;
	uint32  m_uLength;
	uint32 m_uIsKeyFrame;
} encoderDataInd;

/* 打开解码器 */
typedef struct decoderOpen_Tag {
    uint32  * m_uDecHandle;
	uint32    m_uWidth;
	uint32    m_uHeight;
} decoderOpen;

/* 关闭解码器 */
typedef struct decoderClose_Tag {
    uint32    m_uDecHandle;
} decoderClose;

/* 解码器解码 */
typedef struct decoderDecode_Tag {
    uint32    m_uDecHandle;
    uint8 * m_pStream;
    uint32    m_uLength;
	uint32    m_uFormat;
    uint32    m_uLayerWidth;
    uint32    m_uLayerHeight;
    uint8 * m_pLayerBuffer;
} decoderDecode;


/* 图像缩放和颜色转换 */
typedef struct imageSizerCC_Tag {
	uint8  * m_pRGB; // LCD layer buffer
    uint8  * m_pY;   // image Y
    uint8  * m_pU;   // image U
    uint8  * m_pV;   // image V
    uint32     m_uFormat;
    uint32     m_uLayer_width;       // LCD layer width
    uint32     m_uLayer_height;      // LCD layer height
    uint32     m_uWidth;             // image width
    uint32     m_uHeight;            // image height
    uint32     m_uPitch;             // image L buffer stride
    uint32     m_uPitch_uv;          // image
} imageSizerCC;

 
typedef struct VMEncParam_Tag {
	uint32 m_uMediaType;
	uint32 m_uWidth;
	uint32 m_uHeight;
	uint32 m_uFPS;
	uint32 m_uInitQP;
	uint32 m_uIntraPeriod;
	uint32 m_uRCFlag;
	uint32 m_uRCBitrate;
} VMEncParam;

typedef struct VMEncEncodeParam_Tag {
	uint32 handler;
	VMYUVBitmap* pYUV;
	uint32 m_uForceKey;
	uint32 m_uSeverse;
} VMEncEncodeParam;

/* 启动摄像机 */
int32 MPC_CameraStart(cameraStart * pInput, uint32 inputLen);


/* 关闭摄像机 */
int32 MPC_CameraStop(void * pInput, uint32 inputLen);


/* 获取摄像机图像 */
int32 MPC_CameraGetImage(cameraGetImage * pInput, uint32 inputLen);


/* 获取编码后的码流 */
int32 MPC_EncoderGetBits(encoderGetBits * pInput, uint32 inputLen);


/* 即时刷新关键帧编码 */
int32 MPC_EncoderForceIDR(void * pInput, uint32 inputLen);


/* 启动录音设备 */
int32 MPC_AudioStart(audioStart * pInput, uint32 input_len);


/* 关闭音频设备 */
int32 MPC_AudioStop(audioStart * pInput, uint32 input_len);


/* 播放声音 */
int32 MPC_AudioPlay(audioPlay * pInput, uint32 input_len);

/* 缩放和颜色转换 */
int32 MPC_ImageSizerCC(imageSizerCC * pInput, uint32 input_len);

int32 MPC_AudioPlayOpen(audioPlay * pInput, uint32 input_len, uint8** output, int32* output_len);
int32 MPC_AudioPlayAppendData(audioPlay * pInput, uint32 input_len, uint8** output, int32* output_len);
int32 MPC_AudioPlayClose(uint8 * pInput, uint32 input_len);

#endif
#endif

