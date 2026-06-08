
#ifndef __VM_H264ENGINE_H__
#define __VM_H264ENGINE_H__

/* SKY VMMEDIA API */

/* VIDEO API */
#define SKY_VMMEDIA_H264_INTRA_PRED_LUMA_4x4     0x31001
#define SKY_VMMEDIA_H264_INTRA_PRED_LUMA_16x16   0x31002
#define SKY_VMMEDIA_H264_INTRA_PRED_CHMA_8x8     0x31003
#define SKY_VMMEDIA_H264_DECODE_INTRA_I4MB       0x31004
#define SKY_VMMEDIA_H264_DECODE_INTRA_I16MB      0x31005
#define SKY_VMMEDIA_H264_INVTRANSFORM_4x4        0x31006
#define SKY_VMMEDIA_H264_INVTRANSFORM_MB         0x31007
#define SKY_VMMEDIA_H264_DEBLOCK_LUMAEDGE        0x31008
#define SKY_VMMEDIA_H264_DEBLOCK_CHMAEDGE        0x31009
#define SKY_VMMEDIA_H264_INTERPOLATE_LUMA        0x31010
#define SKY_VMMEDIA_H264_INTERPOLATE_CHMA        0x31011
#define SKY_VMMEDIA_H264_READ_CAVLC_COEFF4x4     0x31012
#define SKY_VMMEDIA_H264_READ_CAVLC_MACROBLOCK   0x31013
#define SKY_VMMEDIA_H264_SIZER_AND_COLORSPACE    0x31014
#define SKY_VMMEDIA_H264_DECODER_FUNCTION_INIT   0x31015
#define SKY_VMMEDIA_GET_VERSION_INFO			 0x31080

/* AUDIO API */

/* 返回状态*/
typedef long vmStatus;

/* 参数定义 */

/* ----------------------------------------------------------------------- */
/* 帧内亮度4x4模式预测 */
/* ----------------------------------------------------------------------- */
typedef struct vmIntraPredLuma4x4Param_Tag {
	void * m_pImg;
	long   m_iIOff;
	long   m_iJOff;
	long   m_iThreshold;
} vmIntraPredLuma4x4Param;

/* ----------------------------------------------------------------------- */
/* 帧内亮度16x16模式预测 */
/* ----------------------------------------------------------------------- */
typedef struct vmIntraPredLuma16x16Param_Tag {
	void * m_pImg;
	void * m_pCurrMB;
	long   m_iThreshold;
} vmIntraPredLuma16x16Param;

/* ----------------------------------------------------------------------- */
/* 帧内色度8x8模式预测 */
/* ----------------------------------------------------------------------- */
typedef struct vmIntraPredChma8x8Param_Tag {
	void * m_pImg;
	long   m_iUV;
	void * m_pCurrMB;
	long   m_iThreshold;
} vmIntraPredChma8x8Param;

/* ----------------------------------------------------------------------- */
/* 解码帧内4x4模式宏块 */
/* ----------------------------------------------------------------------- */
typedef struct vmDecodeI4MBParam_Tag {
    void * m_pImg;
    long   m_iIntraThreshold;
    long   m_iCbpBlk;
} vmDecodeI4MBParam;

/* ----------------------------------------------------------------------- */
/* 解码帧内16x16模式宏块 */
/* ----------------------------------------------------------------------- */
typedef struct vmDecodeI16MBParam_Tag {
    void * m_pImg;
    long   m_iIntraThreshold;
    long   m_iCbpBlk;
} vmDecodeI16MBParam;

/* ----------------------------------------------------------------------- */
/* 反变换4x4子块 */
/* ----------------------------------------------------------------------- */
typedef struct vmInvTransform4x4Param_Tag {
    void * m_pCof;
    void * m_pMpr;
    long   m_iStride;
} vmInvTransform4x4Param;

/* ----------------------------------------------------------------------- */
/* 反变换宏块 */
/* ----------------------------------------------------------------------- */
typedef struct vmInvTransformMBParam_Tag {
    void * m_pImg;
    void * m_pCofLocal;
    long   m_iCbpBlk;
} vmInvTransformMBParam;

/* ----------------------------------------------------------------------- */
/* 宏块亮度滤波 */
/* ----------------------------------------------------------------------- */
typedef struct vmDeblockLumaParam_Tag {
    void * m_pSrcDst;
    long   m_iSrcDstStep;
    void * m_pIndexA;
	void * m_pIndexB;
	void * m_pBS;
	long   m_iFlag;
	void * m_pAlpha;
	void * m_pBeta;
	void * m_pClip;
} vmDeblockLumaParam;

/* ----------------------------------------------------------------------- */
/* 宏块色度滤波 */
/* ----------------------------------------------------------------------- */
typedef struct vmDeblockChmaParam_Tag {
    void * m_pSrcDst;
    long   m_iSrcDstStep;
    void * m_pIndexA;
	void * m_pIndexB;
	void * m_pBS;
	long   m_iFlag;
	void * m_pAlpha;
	void * m_pBeta;
	void * m_pClip;
} vmDeblockChmaParam;

/* ----------------------------------------------------------------------- */
/* 亮度差值 */
/* ----------------------------------------------------------------------- */
typedef struct vmInterpolateLumaParam_Tag {
	long   m_iIndx;
    void * m_pSrcY;
    long   m_iSrcYStep;
	void * m_pDstY;
	long   m_iDstYStep;
	long   m_iFractionalX;
	long   m_iFractionalY;
	long   m_iBlockWidth;
	long   m_iBlockHeight;
} vmInterpolateLumaParam;

/* ----------------------------------------------------------------------- */
/* 色度差值 */
/* ----------------------------------------------------------------------- */
typedef struct vmInterpolateChmaParam_Tag {
	long   m_iIndx;
    void * m_pSrcY;
    long   m_iSrcYStep;
	void * m_pDstY;
	long   m_iDstYStep;
	long   m_iDx;
	long   m_iDy;
} vmInterpolateChmaParam;

/* ----------------------------------------------------------------------- */
/* 解码一个CAVLC子块 */
/* ----------------------------------------------------------------------- */
typedef struct vmReadCoeff4x4CAVLCParam_Tag {
    void * m_pImg;
    long   m_iBlockType;
	long   m_iI;
	long   m_iJ;
	void * m_pLevarr;
	void * m_pRunarr;
	void * m_pNumCoeff;
	void **m_ppBsCurByte;
	void * m_pBitOffset;
	void **m_ppHufTables;
} vmReadCoeff4x4CAVLCParam;

/* ----------------------------------------------------------------------- */
/* 解码一个CAVLC宏块 */
/* ----------------------------------------------------------------------- */
typedef struct vmReadCAVLCMBParam_Tag {
    void * m_pImg;
	void * m_pBitStream;
	long   m_iCBP;
	void **m_ppHufTables;
} vmReadOneMBParam;

/* ----------------------------------------------------------------------- */
/* 缩放并色彩空间转换 */
/* ----------------------------------------------------------------------- */


/* PROTOTYPE OF VMMEDIA API */

vmStatus VM_H264IntraPredLuma4x4(void * pInput, int input_len);

vmStatus VM_H264IntraPredLuma16x16(void * pInput, int input_len);

vmStatus VM_H264IntraPredChma8x8(void * pInput, int input_len);

vmStatus VM_H264DecodeIntraI4MB(void * pInput, int input_len);

vmStatus VM_H264DecodeIntraI16MB(void * pInput, int input_len);

vmStatus VM_H264InvTransform4x4(void * pInput, int input_len);

vmStatus VM_H264InvTransformMB(void * pInput, int input_len);

vmStatus VM_H264DeblockLumaEdge(void * pInput, int input_len);

vmStatus VM_H264DeblockChmaEdge(void * pInput, int input_len);

vmStatus VM_H264InterpolateLuma(void * pInput, int input_len);

vmStatus VM_H264InterpolateChma(void * pInput, int input_len);

vmStatus VM_H264ReadCAVLCCoeff4x4(void * pInput, int input_len);

vmStatus VM_H264ReadCAVLCMacroblock(void * pInput, int input_len);


vmStatus VM_H264DecoderFunctionInit(void * pInput, int input_len);

#endif /*__VM_H264ENGINE_H__*/
