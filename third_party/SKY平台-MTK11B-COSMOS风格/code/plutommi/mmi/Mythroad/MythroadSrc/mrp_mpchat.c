// SKY_MULTIMEDIA 
#ifdef __MMI_DSM_NEW__
#ifndef __MMI_DSM_MEDIA_EXT__

#include "med_global.h"
#include "mrporting.h"

#include "mrp_stream_audio.h"

/* OTA Implement */
#include "mrp_mpchat.h"

#include "vmH264Engine.h"
#include "mrp_Mpchat_i.h"
#ifdef __MED_IN_ASM__
#define med_alloc_ext_mem(size) applib_asm_alloc_anonymous_nc(size)
#define med_alloc_ext_mem_cacheable(size) applib_asm_alloc_anonymous(size)
#define med_free_ext_mem(ptr) applib_asm_free_anonymous(*ptr)
#endif


static VMPlatInfo sVmMediaInfo = {0};
static int mr_lcdref_in_proccess = 0;
static uint8 * mpc_aud_ring_buf; 
static int32 preSpace;


int32 mpchat_recording = 0;


#define LAYER_OFFSET 0

#if defined(MT6255)||defined(MT6236)
#define MPC_AUD_BUF_LEN (16*1024)//4096  
#else
#define MPC_AUD_BUF_LEN (4096)  
#endif//Allan slove MT6255 , audio not playing

extern S32 UI_device_width;
extern S32 UI_device_height;


typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);

#ifndef __DSM_SPLAYER__
#define MAXCHARS 512
static char sk_print_buf[MAXCHARS];
void sk_dbg_print(char *fmt,...)
{
#if 1
	va_list ap;
	va_start (ap, fmt);
	vsprintf(sk_print_buf, fmt, ap);
	va_end(ap);
	kal_prompt_trace(MOD_MED,  sk_print_buf);
#endif
}
#endif
void * VCMallocNoncache(int32 nSize)
{
	int32 *pRet = (int32*)med_alloc_ext_mem((unsigned int)(nSize+4));
	if (pRet) {
		memset(pRet, 0, nSize+4);
	}
	return pRet;
}
void   VCFreeNoncache(void *pMem)
{
	med_free_ext_mem((void**)&pMem);
}
/* MEMSET */
void* VCMemset (void * pMem, int32 iData, int32 iSize)
{
	return memset(pMem, iData, iSize);
}

void* VCMalloc (int32 iSize)
{
	int32 *pRet = (int32*)med_alloc_ext_mem((unsigned int)(iSize+4));
	
	if(!pRet)
	{
		sk_dbg_print("#####Malloc Memory Failed!!!");
		return NULL;
	}
	memset(pRet,0,iSize+4);
	return pRet;
}

void* VCIMalloc (int32 iSize)
{
	uint32 * pIMem;
	iSize  = (iSize+3)>>2;
	pIMem = med_alloc_ext_mem_cacheable(iSize<<2);
	if(!pIMem)
	{
		sk_dbg_print("#####Malloc INT Memory Failed!!!");
		return NULL;
	}

	return pIMem;
}

/* FREE */
void VCFree (void * pMem)
{
	if(!pMem)
		return;
	med_free_ext_mem((void **)&pMem);
}

void VCIFree (void * pMem)
{ 
	if(!pMem)
		return;
	med_free_ext_mem((void **)&pMem);
}
void* VCAssert(int32 flag)
{
	ASSERT(flag);
}

int32 mr_media_get_version(void)
{
	return 1110;//Allan 20120217
}/*获取当前多媒体版本*/
int32 mr_media_getPlatInfo(uint8** output, int32* output_len)
{
	if(!output || !output_len)
		return MR_FAILED;
	
	sVmMediaInfo.m_uAudRecFormat = 0;
	sVmMediaInfo.m_uAudPlayFormat = D_VM_AUDIO_PCM8 | D_VM_AUDIO_PCM16;
//#ifdef AMR_DECODE
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_AMR;
#ifdef AMRWB_DECODE
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_AMR_IF2;
#endif
//#endif

#if defined(__DSM_MEDIA_EXT_AUD_REC__)	
//#ifdef AMR_DECODE
	sVmMediaInfo.m_uAudRecFormat |= D_VM_AUDIO_AMR;
#ifdef AMRWB_DECODE
	sVmMediaInfo.m_uAudRecFormat |= D_VM_AUDIO_AMR_IF2;
#endif
//#endif
#endif


#ifdef DAF_DECODE
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_MP3;
#endif
#ifdef AAC_DECODE
	sVmMediaInfo.m_uAudPlayFormat |= D_VM_AUDIO_AAC;
#endif

	sVmMediaInfo.m_uVidRecFormat |= D_VM_IMAGE_RGB565 | D_VM_IMAGE_YUV420;
//	sVmMediaInfo.m_uVidRecFormat = 0;

#if defined(__DSM_MEDIA_EXT_H264_DECODE__)		
	sVmMediaInfo.m_uVidDecFormat |= D_VM_VIDEO_H264;
#endif
#if defined(__DSM_MEDIA_EXT_H264_ENCODE__)	
	sVmMediaInfo.m_uVidEncFormat |= D_VM_VIDEO_H264;
#endif

	sVmMediaInfo.m_uVMVersion = mr_media_get_version();

	*output = (uint8 *)&sVmMediaInfo;
	*output_len = sizeof(VMPlatInfo);

	return MR_SUCCESS;
}/*获取平台信息*/

// TODO: LCD旋转需要MRP自己设置
int32 mr_media_LCD_Display(void* input, int input_len)
{
	VMMediaDisp_S* pInput = (VMMediaDisp_S*)input;
	uint8* dispBuf;
	gdi_handle baseLayer, layer;
	int32 maxWidth = UI_device_width > UI_device_height ? UI_device_width : UI_device_height;
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
	gdi_layer_get_buffer_ptr((uint8 * *)&dispBuf);
	//GDI_LOCK;
	if(pInput->format == VM_IMAGE_RGB565)  // TODO: 暂不支持旋转
	{
		 VMRGBBitmap* pRgb = (VMRGBBitmap *)pInput->img;
		//memcpy(dispBuf, pRgb->pBuffer, pRgb->nWidth*pRgb->nHeight*2); // TODO:
		dispBuf = (uint8 *)pRgb->pBuffer;
	}
	else{
		//GDI_UNLOCK;
		return MR_FAILED;
	}

	gdi_layer_blt_previous(0, 0, UI_device_width - 1, UI_device_height - 1);

	return MR_SUCCESS;
}/* 将数据刷到显示屏上进行显示   */

int32 VM_Media_Aud_Get_CurrTime(uint8** output, int32* output_len)
{
	uint32 ret = SPlayer_aud_get_CurrTime();
	*output = 0;
	*output_len = 4;
	if(ret == 0xFFFFFFFF)
		return MR_FAILED;
	*output = (uint8 *)ret;
	return MR_SUCCESS;
}/*获取音频的当前时间戳*/

char mr_media_get_root_drive(void)
{
	return FS_GetDrive(FS_DRIVE_V_REMOVABLE, 1, FS_NO_ALT_DRIVE);
}/*获取多媒体根目录盘符*/

int32 MPC_AudioPlayAppendData(audioPlay * pInput, uint32 input_len, uint8** output, int32* output_len)
{
	uint16* buf = NULL;
	uint32 buf_len = 0, readLen;
	if(!pInput || !output || !output_len)
		return -1;

 	
	*output = NULL;
	SPlayer_aud_Media_GetWriteBuffer(&buf, &buf_len);

	if(buf && buf_len)
	{
		readLen = buf_len;
		if(buf_len > pInput->m_uLength)
			readLen = pInput->m_uLength;

		kal_prompt_trace(MOD_MMI, "[Allan] MPC_AudioPlayAppendData, inputLen:%d, readLen %d", pInput->m_uLength, readLen);
		if(readLen > 0)
		{
			memcpy(buf, pInput->m_pBuffer, readLen);
			SPlayer_aud_Media_WriteDataDone(readLen);
		}
		*output = (uint8 *)readLen;
		*output_len = 4;

 	}

	return 0;
}/*音频播放数据贴到输出buffer上*/


int32 MPC_AudioPlayOpen(audioPlay * pInput, uint32 input_len, uint8** output, int32* output_len)
{
	
	if(!pInput || !output || !output_len)
		return -1;
 		
	mpc_aud_ring_buf = VCMalloc(MPC_AUD_BUF_LEN);
	if(!mpc_aud_ring_buf)
    {   
        kal_prompt_trace(MOD_MED,"[Allan] MPC_AudioPlayOpen alloc fail");
		return -1;
    }
	if(splayer_aud_stream_open(pInput->m_uFormat, NULL, mpc_aud_ring_buf, MPC_AUD_BUF_LEN) != 0)
		return -1;

	if(MPC_AudioPlayAppendData(pInput, input_len, output, output_len) != 0)
		return -1;
    

	splayer_aud_stream_play();

 	
	return 0;
}/*音频打开并播放*/
extern void (*gui_start_timer) (S32 count, void (*callback) (void));
void mr_media_timer_start(int32 count, void (*callback) (void))
{
	gui_start_timer(count, callback);
}/*定时器*/

static void mpc_audioPlayCloseCb(void)
{
	int32 space = SPlayer_aud_Media_GetFreeSpace();
	
	if(space >= MPC_AUD_BUF_LEN -1 ||(space != 0 && space == preSpace))
	{
		splayer_aud_stream_stop();
		if(mpc_aud_ring_buf)
			VCFree(mpc_aud_ring_buf);
		mpc_aud_ring_buf = NULL;
	}
	else {
		preSpace = space;
		mr_media_timer_start(500, mpc_audioPlayCloseCb);
	}

}/*音频播放关闭的计时器回调*/
int32 MPC_AudioPlayClose(uint8 * pInput, uint32 input_len)
{
	if(!pInput)
		return -1;

	if(*pInput == 1)
	{ // 立即关闭
		splayer_aud_stream_stop();
		if(mpc_aud_ring_buf)
			VCFree(mpc_aud_ring_buf);
		mpc_aud_ring_buf = NULL;
	}
	else{
		preSpace = 0;
		mpc_audioPlayCloseCb();
	}
	return 0;
}/*关闭音频播放*/
int32 MPC_AudioPlayPause(void)
{
	if(splayer_aud_stream_pause() == MR_SUCCESS)//Allan 20120217
		return MR_SUCCESS;
	return MR_FAILED;
}/*暂停音频播放*/
int32 MPC_AudioPlayResume(void)
{
	if(splayer_aud_stream_resume() == MR_SUCCESS)//Allan 20120217
		return MR_SUCCESS;
	return MR_FAILED;
}/*重新播放音频*/
int32 mr_media_get_screen_info(void)
{
	int32 lScreenInfo = 0;
/*************MODIFY ROTATE start*******************/
#if !defined(__MMI_SCREEN_ROTATE__)
	lScreenInfo |= MPC_SCREEN_NOT_SUPPORT_ROTATE;
    lScreenInfo |= MPC_SCREEN_SUPPORT_SELF_ROTATE;//Allan 20120217
#endif
/*************MODIFY ROTATE end*******************/
	return lScreenInfo;
}/*获取屏幕状态信息*/

int32 mr_media_get_keyboard_info(void)
{
	int32 lKeyInfo = 0;
	
	#ifdef __MR_CFG_FEATURE_NO_NUM_KEY__/*没有数字键*/
	lKeyInfo |= MPC_KEYBOARD_NO_NUM_KEY;
	#endif
	
	#ifdef  __MR_CFG_FEATURE_NO_OK_KEY__/*没有OK键*/
	lKeyInfo |= MPC_KEYBOARD_NO_OK_KEY;
	#endif
	
	return lKeyInfo;
}
#if defined(__DSM_MEDIA_EXT_AUD_REC__)	
int32 mr_media_audio_is_recording(void)
{
	return mpchat_recording;
}


void mr_media_send_aud_data(uint8* pData, uint32 dataSize)
{
	mr_event(MR_AUD_REC_DATA_IND, (int32)pData, dataSize);
	sk_dbg_print("mr_media_send_aud_data  aud buff address=%d   size=%d ", pData, dataSize);
}

int32 MPC_AudioStart(audioStart * pInput, uint32 input_len)
{
	uint8 result;
	char filepath[32];

	mpchat_recording = 1;	
	
	kal_wsprintf((uint16 *)filepath, "%c:\\mpc1.amr", mr_media_get_root_drive());
	result =  mr_media_aud_record_start(filepath, 0, NULL, NULL);

	if(0 == result) {
	} else {
		mpchat_recording = 0;	
	}

	return result;
}

int32 MPC_AudioStop(audioStart * pInput, uint32 input_len)
{
	uint8 result;

	result = mr_media_aud_record_stop();
	mpchat_recording = 0;
	return (int32)result;
}

#endif
#if defined(__DSM_MEDIA_EXT_H264_ENCODE__ ) 

#define MR_PLAT_VALUE_BASE 1000    //用于某些返回值的基础值

typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
extern int32 mr_event(int16 type, int32 param1, int32 param2);


/* JFW 04.27.2010 */
#define ENCODER_NULL  (1<<1)
#define ENCODER_IDLE  (1<<2)
#define ENCODER_BUSY  (1<<3)

static volatile uint32 g_uEncoderStatus = ENCODER_NULL;
//static uint32 g_total_frames = 0;
//static uint32 g_total_clocks = 0;
static uint32 g_decoder_frames = 0;
static uint32 g_decoder_clocks = 0;


/* GLOBAL */
uint32  g_uImgWidth  = 96;
uint32  g_uImgHeight = 80;
uint8 * g_pYBuffer = NULL;
uint8 * g_pUBuffer = NULL;
uint8 * g_pVBuffer = NULL;
static void (*g_pAudioRecordCallback)(void * pBuffer, uint32 uLength, uint32 m_uParam);
static uint32  g_uAudioRecordParam = 0;

static uint8 * g_pYUVBuffer1 = NULL;
static uint8 * g_pYUVBuffer2 = NULL;
static uint8 * g_pYBuffer2 = NULL;
static uint8 * g_pUBuffer2 = NULL;
static uint8 * g_pVBuffer2 = NULL;

/* Camera 运行标识 */
uint8 gMrCamExtRunning = 0;
/* Camera数据实时视频编码标示 */
uint8 gMrCamEncRunning = 0;

uint8 gMrCamSysInitialized = 0;
uint8 gMrCam_datareq_ind_flag = 0;
uint8 gMPchatMMIState = 0;
uint8 gEncodeStateCode = 0;

void * pEncoderHandle = NULL;
VC_H264EncParam * pEncoderParam = NULL;
uint8 * pEncodeOutputStreamA = NULL;
uint8 * pEncodeOutputStreamB = NULL;
uint8 * pYUV2RGBOutputStreamA=NULL;
uint8 * pYUV2RGBOutputStreamB=NULL;

uint8    A_Flag=0;
uint8    B_Flag=0;
uint8    PutIndex=0;
uint8	 GetIndex = 0;

int32 iEncodeSizeA = 0;
int32 iEncodeSizeB = 0;
uint32 usedBytes = 0;



uint8  mpchat_preview_handle=0Xff;

static VMYUVBitmap gMpchatYUVbuf = {0};


extern void mr_media_Send_Enocde_REQ( void* buffer, uint32 len, uint8 state);
extern void mr_media_cam_Send_DataReq(void);

int32 MPC_H264EncodeOpen(void* pInput, uint32 inputLen);
void MPchat_Release_Memory(void);


typedef enum
{
    MPCHAT_EVT_NOWAIT = 0,

	MPCHAT_EVT_POWERON = 0x01,
	MPCHAT_EVT_POWEROFF = 0x02
}MPCHAT_EVT_E;
	
static int32 gMpchatEvent = 0;
//encoderDataInd gMrEncBits = {0};

int32 MPC_H264EncoderOpen(void* pInput, uint32 inputLen)
{
	VMEncParam* encParam = (VMEncParam *)pInput;
	
	sk_dbg_print( "[MPVC] MPC_H264EncodeOpen\n");
	if (pInput == NULL) {
		sk_dbg_print( "[MPVC] MPC_H264EncodeOpen - Parameter Error\n");
		return MR_FAILED;
	}

	if(encParam->m_uMediaType == VM_VIDEO_H264)
	{
		VC_H264EncParam* h264Param = (VC_H264EncParam *)((uint32*)encParam + 1);
		/* PROFILE */
		//g_total_frames = 0;
		//g_total_clocks = 0;
		g_decoder_frames = 0;
		g_decoder_clocks = 0;
		
			
		pEncodeOutputStreamA = (uint8 *)VCMalloc(h264Param->m_uWidth*h264Param->m_uHeight*2);//rwei //(/2
		if (pEncodeOutputStreamA == NULL) {
			sk_dbg_print( "[MPVC] No more memory to allocate\n");
			MPchat_Release_Memory();
			return MR_FAILED;
		}
		memset(pEncodeOutputStreamA, 0, h264Param->m_uWidth*h264Param->m_uHeight*2);//rwei  //(/2
		pEncodeOutputStreamB = (uint8 *)VCMalloc(h264Param->m_uWidth*h264Param->m_uHeight*2);//rwei  //(/2
		if (pEncodeOutputStreamB == NULL) {
			sk_dbg_print( "[MPVC] No more memory to allocate\n");
			MPchat_Release_Memory();
			return MR_FAILED;
		}	
		memset(pEncodeOutputStreamB, 0, h264Param->m_uWidth*h264Param->m_uHeight*2);//rwei  //(/2

		/* Open video encoder */
		pEncoderHandle = VC_H264Encoder_Open(h264Param);
		if (pEncoderHandle == NULL) {
			sk_dbg_print( "[MPVC] Can not open video encoder\n");
			return MR_FAILED;
		}

		A_Flag = 0;
		B_Flag = 0;
		PutIndex=0;
		GetIndex = 0;
		
		gEncodeStateCode = 1;
		
		mr_media_init_enc_callback();	

		gMpchatEvent = MPCHAT_EVT_POWERON;
		
		return MR_SUCCESS;
	}
	return MR_FAILED;
}

int32 MPC_H264EncoderClose(void)
{
	if(gMpchatEvent == MPCHAT_EVT_POWERON)
	{
		mr_media_Send_Enocde_REQ(NULL, 0, 0);
		while(gMpchatEvent != MPCHAT_EVT_POWEROFF)
		{
			kal_sleep_task(3);
		}
	}
	return 0;
}

int32 MPC_H264EncoderEncode(void *pInput, uint32 inputLen)
{
	VMEncEncodeParam* pEncParam = (VMEncEncodeParam *)pInput;

	if(!pEncParam ||!pEncParam->pYUV)
		return 1;
	if(pEncParam->m_uForceKey)
	{
		MPC_EncoderForceIDR(NULL, 0);
	}
	memcpy(&gMpchatYUVbuf, pEncParam->pYUV, sizeof(VMYUVBitmap));
	mr_media_Send_Enocde_REQ((void *)&gMpchatYUVbuf, sizeof(VMYUVBitmap), 1);

	return 0;
}

void MPchat_task_init(void)
{
	g_uEncoderStatus = ENCODER_NULL;
}


int32 MPC_EncoderGetBits(encoderGetBits * pInput, uint32 inputLen)
{
	sk_dbg_print( "[MPVC] MPC_EncoderGetBits\n");
	
	if (A_Flag == 0 && B_Flag == 0) {
		/* NULL */	
		*pInput->m_uLength = 0;
		*pInput->m_pBuffer = NULL;	
		sk_dbg_print( "[MPVC] mr_platEx encoder  buffer NULL \n");	   				
		return MR_SUCCESS;	
	}
	if (GetIndex == 0 && A_Flag == 0) {
		/* ERROR */
		sk_dbg_print( "[MPVC] mr_platEx encoder  error GetIndex == 0 && A_Flag == 0 \n");	   				
		return MR_FAILED;				
	}
	if (GetIndex == 1 && B_Flag == 0) {
		/* ERROR */
		sk_dbg_print( "[MPVC] mr_platEx encoder  error GetIndex == 1 && B_Flag == 0 \n");	   					
		return MR_FAILED;				
	}
	if((iEncodeSizeA>10240)||(iEncodeSizeB>10240))
	{
		/* ERROR */
		sk_dbg_print( "[MPVC] mr_platEx encoder  error iEncodeSizeA %d \n",iEncodeSizeA);	
		sk_dbg_print( "[MPVC] mr_platEx encoder  error iEncodeSizeB %d \n",iEncodeSizeB);
		*pInput->m_uLength =0;
		*pInput->m_pBuffer = NULL;		
		return MR_SUCCESS;				
	}		
	if (GetIndex == 0) {
		*pInput->m_pBuffer = pEncodeOutputStreamA;
		*pInput->m_uLength = iEncodeSizeA;
		if(A_Flag == 2)
			*pInput->m_uIsKeyFrame = 1;
		A_Flag = 0;	
		sk_dbg_print( "[MPVC] get encode buffer A len  %d \n", iEncodeSizeA);
	}
	if(GetIndex == 1){
		*pInput->m_pBuffer = pEncodeOutputStreamB;
		*pInput->m_uLength = iEncodeSizeB;
		if(B_Flag == 2)
			*pInput->m_uIsKeyFrame = 1;
		B_Flag = 0;
		sk_dbg_print( "[MPVC] get encode buffer B len  %d \n", iEncodeSizeB);		
	}
	GetIndex = !GetIndex;
	return MR_SUCCESS;
}

int32 MPC_EncoderForceIDR(void * pInput, uint32 inputLen)
{
	/* TODO */
}

int32 mr_media_cam_get_yuvBuf(uint8* pYUV[3])
{
	if(g_pYUVBuffer1)
	{
		pYUV[0] = g_pYBuffer;
		/* U Buffer */
		pYUV[1] = g_pUBuffer;
		/* V Buffer */
		pYUV[2] = g_pVBuffer;
		return g_uImgWidth*g_uImgHeight;
	}
	return 0;
}


static void MPC_Camera_Release_Memory()
{
	if (g_pYUVBuffer1) {
		VCFree((void *)g_pYUVBuffer1);
		g_pYUVBuffer1 = NULL;
	}
	g_pYBuffer = g_pUBuffer = g_pVBuffer = NULL;
	
	if (g_pYUVBuffer2) {
		VCFree((void *)g_pYUVBuffer2);
		g_pYUVBuffer2 = NULL;
	}
	g_pYBuffer2 = g_pUBuffer2 = g_pVBuffer2 = NULL;

}

int32 MPC_CameraStart(cameraStart * pInput, uint32 inputLen)
{
	
	sk_dbg_print( "[MPVC] MPC_CameraStart w %d h %d,size(%d)\n", pInput->m_uWidth, pInput->m_uHeight,(pInput->m_uWidth*pInput->m_uHeight*3/2  +16));
	if (pInput == NULL) {
		sk_dbg_print( "[MPVC] MPC_CameraStart - Parameter Error\n");
		return 1;
	}
	if(gMrCamSysInitialized)
	{
		ASSERT(0);
	}
	/* GLOBAL */
	gMPchatMMIState = 1;
	
		
	/* Resolution */
	g_uImgWidth  = pInput->m_uWidth;
	g_uImgHeight = pInput->m_uHeight;
	
	
	if(mr_media_cam_get_dataType() == 1)
	{
		g_pYUVBuffer1 = (uint8 *)VCMalloc(pInput->m_uWidth*pInput->m_uHeight*2);
		if (g_pYUVBuffer1 == NULL) {
			sk_dbg_print( "[MPVC] No more memory to allocate\n");
			return 1;
		}
		memset(g_pYUVBuffer1, 0, pInput->m_uWidth*pInput->m_uHeight*2);
		g_pYBuffer = g_pYUVBuffer1;
	}
	else {
		g_pYUVBuffer1 = (uint8 *)VCMalloc(pInput->m_uWidth*pInput->m_uHeight*3/2  +16);
		if (g_pYUVBuffer1 == NULL) {
			sk_dbg_print( "[MPVC] No more memory to allocate\n");
			return 1;
		}
		/* Y Buffer  */
		g_pYBuffer = (uint8 *)(((int32)g_pYUVBuffer1+15) & 0xfffffff0);
		/* U Buffer */
		g_pUBuffer = g_pYBuffer + (pInput->m_uWidth*pInput->m_uHeight);
		/* V Buffer */
		g_pVBuffer = g_pUBuffer+ (pInput->m_uWidth*pInput->m_uHeight/4);
		memset(g_pYBuffer, 0x00, pInput->m_uWidth*pInput->m_uHeight);
		memset(g_pUBuffer, 0x80, pInput->m_uWidth*pInput->m_uHeight/4);
		memset(g_pVBuffer, 0x80, pInput->m_uWidth*pInput->m_uHeight/4);
	}

	g_pYUVBuffer2 = (uint8 *)VCMalloc(pInput->m_uWidth*pInput->m_uHeight*3/2  +16);
	if (g_pYUVBuffer2 == NULL) {
		sk_dbg_print( "[MPVC] No more memory to allocate\n");
		MPC_Camera_Release_Memory();
		return 1;
	}
	/* Y Buffer  */
	g_pYBuffer2 = (uint8 *)(((int32)g_pYUVBuffer2+15) & 0xfffffff0);
	/* U Buffer */
	g_pUBuffer2 = g_pYBuffer2 + (pInput->m_uWidth*pInput->m_uHeight);
	/* V Buffer */
	g_pVBuffer2 = g_pUBuffer2 + (pInput->m_uWidth*pInput->m_uHeight/4);
	memset(g_pYBuffer2, 0x00, pInput->m_uWidth*pInput->m_uHeight);
	memset(g_pUBuffer2, 0x80, pInput->m_uWidth*pInput->m_uHeight/4);
	memset(g_pVBuffer2, 0x80, pInput->m_uWidth*pInput->m_uHeight/4);

	gMrCamExtRunning = 1;
	gMrCamSysInitialized = 1;
	if(mr_media_cam_preview_start(pInput->m_uWidth, pInput->m_uHeight) != MR_SUCCESS)
	{
		gMrCamExtRunning = 0;
		gMrCamSysInitialized = 0;
		MPC_Camera_Release_Memory();
		return 1;
	}
	
	mr_media_cam_init_callback();
	

	sk_dbg_print( "[MPVC] MPC_CameraStart FPS %d QP %d\n", pInput->m_uFPS, pInput->m_uInitQP);
	if(pInput->m_uFPS && pInput->m_uInitQP)
	{
		/* Parameters */
		VMEncParam encParam = {0};
		encParam.m_uMediaType = pInput->m_uMediaType;
		encParam.m_uWidth       = pInput->m_uWidth;
		encParam.m_uHeight      = pInput->m_uHeight;
		encParam.m_uFPS         = pInput->m_uFPS;
		encParam.m_uIntraPeriod = pInput->m_uIntraPeriod;
		encParam.m_uRCFlag      = pInput->m_uRCFlag;
		encParam.m_uRCBitrate   = pInput->m_uRCBitrate;
		encParam.m_uInitQP      = pInput->m_uInitQP;

		gMrCamEncRunning = 1;
		MPC_H264EncoderOpen((void *)&encParam, sizeof(VC_H264EncParam));
	}

	
	mr_media_cam_GetData();
	sk_dbg_print( "[MPVC] MPC_CameraStart End\n");
	
	return 0;
}
void mr_media_Send_Enocde_CNF(void );//rwei 
extern void (*gui_cancel_timer) (void (*callback) (void));
int32 MPC_CameraStop(void * pInput, uint32 inputLen)
{
	sk_dbg_print( "[MPVC] MPC_CameraStop\n");
	
	gui_cancel_timer(mr_media_Send_Enocde_CNF);//rwei 
	
	gMPchatMMIState   = 0;
	gEncodeStateCode  = 0;
	gMrCamSysInitialized = 0;
	
	
	mr_media_cam_preview_stop();//rwei 

	
	if(gMrCamEncRunning == 1)
	{
		MPC_H264EncoderClose();
	}

	
	MPC_Camera_Release_Memory();

	gMrCamExtRunning = 0;
	return 0;
}

int32 MPC_CameraGetImage(cameraGetImage * pInput, uint32 inputLen)
{
	//turn MR_FAILED;
	//memset(pInput->m_pRGBBuffer, 0, pInput->m_uWidth*pInput->m_uHeight*2);
	//return MR_SUCCESS;
	if((!pInput && !pInput->m_pRGBBuffer)||(!gMrCamSysInitialized))
	{
		return MR_FAILED;
	}
	
	if (mr_media_cam_get_dataType() == 1)
	{
		sk_dbg_print("MPC_CameraGetImage w %d h %d\r\n", pInput->m_uWidth, pInput->m_uHeight);
		memcpy(pInput->m_pRGBBuffer, g_pYUVBuffer1, pInput->m_uWidth*pInput->m_uHeight*2);
	}
	else
	{
		vmSizerColorspaceParam param;
		param.m_pYBuffer = g_pYBuffer2;
		param.m_pUBuffer = g_pUBuffer2;
		param.m_pVBuffer = g_pVBuffer2;
		param.m_pRGBBuffer =  pInput->m_pRGBBuffer;
		param.m_iWidth = g_uImgWidth;
		param.m_iHeight = g_uImgHeight;
		param.m_iRGBWidth = pInput->m_uWidth;
		param.m_iRGBHeight = pInput->m_uHeight;
		param.m_iStepLuma = g_uImgWidth;
		param.m_iStepChroma = g_uImgWidth/2;
		param.m_iRGBStride = pInput->m_uWidth;
		
		VM_SizerAndColorspace((void*)&param, sizeof(vmSizerColorspaceParam));
	}
	return MR_SUCCESS;
}


void mr_media_cam_GetData_Ind_Hdr(void)
{
	sk_dbg_print( "^^^mr_media_cam_GetData_Ind_Hdr^^^");
	kal_sleep_task(15);
       if(!gMrCamSysInitialized)
	{
		return ;
	}
	mr_media_cam_GetData_Rsp_Process();	

}

void mr_media_cam_GetData_Rsp_Process(void)
{
	sk_dbg_print( "^^^mr_media_cam_GetData_Rsp_Process^^^");

	if(!g_pYUVBuffer1)
	{
		sk_dbg_print("Error, 111 buffer had been free!!!!!!");
		return;
	}
	if(mr_media_cam_get_dataType() == 1) // RGB
	{
		mr_media_cam_RGBRotation(pYUV2RGBOutputStreamA, g_pYUVBuffer1, g_uImgWidth, g_uImgHeight);
		gMPchatMMIState++;
		mr_media_cam_DataRsp(g_pYUVBuffer1, g_uImgWidth*g_uImgHeight);
	}
	else /// if(mr_media_cam_get_dataType() == 0) // YUV
	{
		mr_media_cam_YUVRotation(g_pYBuffer, g_pYBuffer2, g_uImgWidth, g_uImgHeight);
		gMPchatMMIState++;
		mr_media_cam_DataRsp(g_pYBuffer2, g_uImgWidth*g_uImgHeight);
	}
	
}

void mr_media_YUV_GetNotify(uint8* pData, int32 nDataSize)
{   	
	sk_dbg_print( "[MPVC] mr_media_YUV_GetNotify - dataPtr = %x, dataSize = %d, encoderStatus = %d\n", pData, nDataSize, g_uEncoderStatus);	

	if(gMPchatMMIState >1 )
		gMPchatMMIState--; /* added by yuzf for resolve MT6253 crash after camera switch 10times  */
	if(gMPchatMMIState ==1 )
	{
		if(!g_pYUVBuffer1 || !g_pYUVBuffer2)
		{
			sk_dbg_print("Error, 22 buffer had been free!!!!!!");
			return;
		}
		if(mr_media_cam_get_dataType() == 1) // RGB
		{
			rgb565_to_yv12_c(pData, g_pYBuffer2, g_pUBuffer2, g_pVBuffer2, g_uImgWidth, g_uImgHeight, g_uImgWidth);
			pData = g_pYBuffer2;
		}
	

		//开始encode
		if(gMrCamEncRunning == 1 && gEncodeStateCode == 1)
		{
			gMpchatYUVbuf.pPlane[0] = pData;
			gMpchatYUVbuf.pPlane[1] = pData + nDataSize;
			gMpchatYUVbuf.pPlane[2] = gMpchatYUVbuf.pPlane[1] + nDataSize/4;
			gMpchatYUVbuf.nWidth = g_uImgWidth;
			gMpchatYUVbuf.nHeight = g_uImgHeight;
			gMpchatYUVbuf.nLumaStride = g_uImgWidth;
			gMpchatYUVbuf.nChmaStride = g_uImgWidth/2;
			
			mr_media_Send_Enocde_REQ((void *)&gMpchatYUVbuf, sizeof(VMYUVBitmap), 1);
		} 

	}	
}	


int32 mr_media_cam_app(void)
{
	return gMrCamExtRunning;
}

void mr_media_cam_Send_DataReq(void)
{
	if(gMPchatMMIState==1){
		mr_media_cam_GetData();}
}

void MPchat_Close_Handle(void)
{
	sk_dbg_print( "[MPVC] ***MPchat_Close_Handle*******\n");
	if (pEncoderHandle) {
		VC_H264Encoder_Close(pEncoderHandle);  
		pEncoderHandle = NULL;
	}
}
void MPchat_Release_Memory(void)
{
	sk_dbg_print( "[MPVC] ***MPchat_Release_Memory***enter****\n");

	/* STACK */
	
	if(pEncodeOutputStreamA){
		VCFree((void *)pEncodeOutputStreamA);
		pEncodeOutputStreamA=NULL;
	}
	if(pEncodeOutputStreamB){
		VCFree((void *)pEncodeOutputStreamB);
		pEncodeOutputStreamB=NULL;
	}
	
}


void MPchat_Encode_req_handler(void* pMsg)
{
	VMMediaEncCamData *msg_p = (VMMediaEncCamData*)pMsg;

	if((msg_p->pData==NULL)&&(msg_p->nDataSize==0)&&(msg_p->nState==0)) {
		MPchat_Release_Memory();
		MPchat_Close_Handle();
		//MPCHAT_SET_EVENT(MPCHAT_EVT_POWEROFF);
		gMpchatEvent = MPCHAT_EVT_POWEROFF;
	} else{
		g_uEncoderStatus = ENCODER_BUSY;
		MPchat_Encode_Process(msg_p->pData, msg_p->nDataSize);
		g_uEncoderStatus = ENCODER_IDLE;
	}
}
#endif
#if defined(__DSM_MEDIA_EXT_H264_DECODE__)		//rwei
static uint32 sResizerOpen = 0;
static uint8* pResizerBuf = NULL;
extern int32 VM_SizerAndColorspace(void * pInput, int32 input_len);
int32 mr_media_resizer_and_colorspace(void* input, int32 input_len)
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
			memmove((uint8 *)pInput->m_pRGBBuffer+(pInput->m_iRGBHeight-1)*pInput->m_iRGBStride*2, (uint8 *)pInput->m_pRGBBuffer+(pInput->m_iRGBHeight-1)*pInput->m_iRGBWidth*2, pInput->m_iRGBWidth*2);
			for(i=pInput->m_iRGBHeight-2; i>0; i--)
			{
				memmove((uint8 *)pInput->m_pRGBBuffer+i*pInput->m_iRGBStride*2, (uint8 *)pInput->m_pRGBBuffer+i*pInput->m_iRGBWidth*2, pInput->m_iRGBWidth*2);
				memset((uint8 *)pInput->m_pRGBBuffer+i*pInput->m_iRGBStride*2+pInput->m_iRGBWidth*2, 0, (pInput->m_iRGBStride-pInput->m_iRGBWidth)*2);
			}
			memset((uint8 *)pInput->m_pRGBBuffer+pInput->m_iRGBWidth*2, 0, (pInput->m_iRGBStride-pInput->m_iRGBWidth)*2);
		}

		mr_media_resizer_close();
	}
	else
#endif
	
	VM_SizerAndColorspace((void*)input, input_len);
	return MR_SUCCESS;
}

int32 mr_media_resizer_close(void)
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

void mr_media_fast_call(void (*pFunc)(void *pParam),void *pParam)
{
	//	sk_dbg_print("alloc cache stack size = %d,pFunc = %d\r\n", SKY_FAST_STACK_SIZE,(int)pFunc);
	if(pFunc)
	{
#if !defined(__ARM9_MMU__) && !defined(MT6252)  && !defined(MT6252H)
		uint32 stack_begin;
		//		dbg_print("alloc cache stack size = %d\r\n", MED_INT_STACK_MEM_SIZE);

		stack_begin = (U32)drv_gfx_stkmgr_get_stack_internal(SKY_FAST_STACK_SIZE);
		if(stack_begin)
		{
			sk_dbg_print("alloc cache stack succeeded!\r\n");
			INT_SwitchStackToRun((void*)stack_begin,SKY_FAST_STACK_SIZE,(kal_func_ptr)pFunc, 1, pParam,0);

			drv_gfx_stkmgr_release_stack_internal((void*)stack_begin);
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

#endif
int32 mr_media_set_full_screen(int32 * pInput,MPCRotateInfo** output)
{
	U8 rotate_value;
	MPCRotateInfo *out = *output;
	/*************MODIFY ROTATE start*******************/
	U8 pre_rotate_value = 0;
    S32 width;
    S32 height;
	S32 temp;
	/*************MODIFY ROTATE end*******************/
	if(MR_LCD_ROTATE_NORMAL == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_0;
	else if(MR_LCD_ROTATE_90 ==*pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_90;
	else if (MR_LCD_ROTATE_180 == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_180;
	else if (MR_LCD_ROTATE_270 == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_270;
	else if (MR_LCD_MIRROR == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_0;
	else if (MR_LCD_MIRROR_ROTATE_90 == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_90;
	else if (MR_LCD_MIRROR_ROTATE_180 == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_180;
	else if (MR_LCD_MIRROR_ROTATE_270 == *pInput)
		rotate_value = MMI_FRM_SCREEN_ROTATE_MIRROR_270;
	else
		return MR_FAILED;
		/*************MODIFY ROTATE start*******************/
	pre_rotate_value = gdi_layer_get_rotate();
	gdi_layer_set_rotate((4-rotate_value)&0x03);

	gdi_layer_get_dimension(&width,&height);
	if((rotate_value&0x01) != (pre_rotate_value&0x01))
	{
		temp = width;
		width = height;
		height = temp;
	}
	gdi_layer_resize(width,height); 
	out->width  = width;
	out->height = height;
	
	/*************MODIFY ROTATE end*******************/
	return MR_SUCCESS;
}/*获取屏幕状态信息*///Allan 20120217
int32 mr_platEx_mpchat(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	
	kal_prompt_trace(MOD_MMI, "[Allan] mr_platEx_mpchat %x", code);
	switch (code )
	{
		// VM MEDIA
		case MR_MPCHAT_GET_VESION:
		{
			return mr_media_get_version();
		}
		case SKY_VMMEDIA_GET_PLAT_INFO:
		{
			mr_media_getPlatInfo(output, output_len);
			return MR_SUCCESS;
		}
		
		case SKY_VMMEDIA_INTSRAM_MALLOC:
		{
			uint8 * pMem;
			if(!input || !output)
				return MR_FAILED;
			pMem = (uint8 *)VCIMalloc(*((int32 *)input));
			if(pMem)
			{
				*output = pMem;
				*output_len = *input;
				return MR_SUCCESS;
			}
			return MR_FAILED;
		}
		case SKY_VMMEDIA_INTSRAM_FREE:
		{
			if(!input)
				return MR_FAILED;
			VCIFree(input);
			return MR_SUCCESS;
		}
		case SKY_VMMEDIA_STACK_FUNCCALL:
		{
			return MR_FAILED;
		}
		case SKY_VMMEDIA_DISPLAY_FREME:
		{
			return mr_media_LCD_Display((void*)input, input_len);
		}
		case SKY_VMMEDIA_AUD_GET_CURRTIME:
		{
			return VM_Media_Aud_Get_CurrTime(output, output_len);
		}
		case MR_MPCHAT_AUDIO_PLAY_START:
		{
			return MPC_AudioPlayOpen((audioPlay *)input, input_len, output, output_len);
		}
		case MR_MPCHAT_AUDIO_PLAY_APPEND:
		{
			return MPC_AudioPlayAppendData((audioPlay *)input, input_len, output, output_len);
		}
		case MR_MPCHAT_AUDIO_PLAY_STOP:
		{
			return MPC_AudioPlayClose(input, input_len);
		}
		case MR_MPCHAT_AUDIO_SET_VOLUME:
		{
			if(!input)
				return MR_FAILED;
			Sky_MM_Audio_SetLevel(*input);
			return MR_SUCCESS;
		}
		case MR_MPCHAT_AUDIO_PLAY_PAUSE:
		{
			return MPC_AudioPlayPause();
		}
		case MR_MPCHAT_AUDIO_PLAY_RESUME:
		{
			return MPC_AudioPlayResume();
		}
		case MR_MPCHAT_SCREEN_INFO:
		{
			return mr_media_get_screen_info();
		}
		case MR_MPCHAT_KEYBOARD:
		{
			return mr_media_get_keyboard_info();
		}
		case MR_MPCHAT_MPLIVE_FULLSCREEN:
		{
			return mr_media_set_full_screen((int32*)input,(MPCRotateInfo**)output);
		}//Allan 20120217
#if defined(__DSM_MEDIA_EXT_H264_DECODE__)		
		case 0x31001:
		case 0x31002:
		case 0x31003:
		case 0x31004:
		case 0x31005:
		case 0x31006:
		case 0x31007:
		case 0x31008:
		case 0x31009:
		case 0x31010:
		case 0x31011:
		case 0x31012:
		case 0x31013:
		case 0x31014:
		case 0x31015:
		case 0x31016:
		case 0x31017:
		{
			return mr_platEx_mpchat_dec( code,  input,  input_len,  output,  output_len, cb);
		}
#endif

#if defined(__DSM_MEDIA_EXT_H264_ENCODE__)	
		case SKY_VMMEDIA_ENCODER_OPEN:
		{
			return MPC_H264EncoderOpen((void*)input, input_len);
		}
		case SKY_VMMEDIA_ENCODER_CLOSE:
		{
			MPC_H264EncoderClose();
			return MR_SUCCESS;
		}
		case SKY_VMMEDIA_ENCODER_ENCODE:
		{
			MPC_H264EncoderEncode((void *)input, input_len);
			return MR_SUCCESS;
		}
		// PLATFORM INTERFACE
		case MR_MPCHAT_GET_CAMERA_NUM:
		{
			return 1000+mr_media_cam_get_num();
		}
		case MR_MPCHAT_SWITCH_CAMERA:
		{
			uint8 set = 0;
			if(input)
				set = *input;
			return mr_media_cam_CamsSwitch(set);
		}
		case MR_MPCHAT_SWITCH_NIGHT:
		{
			return mr_media_cam_nightProf_switch(input, input_len);
		}
		/* MP Video Chat */
		case MR_MPCHAT_CAMERA_START:
		{
			MPC_CameraStart((cameraStart*)input, input_len);
			
			return MR_SUCCESS;
		}
		case MR_MPCHAT_CAMERA_STOP:
		{
			MPC_CameraStop((void*)input, input_len);
			
			return MR_SUCCESS;
		}
		case MR_MPCHAT_CAMERA_GETIMAGE:
		{
			MPC_CameraGetImage((cameraGetImage*)input, input_len);
			
			return MR_SUCCESS;
		}
		case MR_MPCHAT_ENCODER_GETBITS:
		{
			MPC_EncoderGetBits((encoderGetBits*)input, input_len);
			
			return MR_SUCCESS;
		}
		case MR_MPCHAT_ENCODER_FORCEIDR:
		{
			MPC_EncoderForceIDR((void*)input, input_len);
			
			return MR_SUCCESS;
		}
#endif
#if defined(__DSM_MEDIA_EXT_AUD_REC__)	
		case MR_MPCHAT_AUDIO_START:
		{
			MPC_AudioStart((audioStart*)input, input_len);
			
			return MR_SUCCESS;
		}
		case MR_MPCHAT_AUDIO_STOP:
		{
			MPC_AudioStop((void*)input, input_len);
			
			return MR_SUCCESS;
		}
#endif
		default:
			return MR_IGNORE;
	}

	return MR_IGNORE;
}

#endif
#endif

