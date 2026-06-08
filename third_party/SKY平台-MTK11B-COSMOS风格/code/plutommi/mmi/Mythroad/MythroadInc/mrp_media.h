#ifndef __MMIDSM_MEDIA_H__
#define __MMIDSM_MEDIA_H__
#include "mrporting.h"
#include "PixtelDataTypes.h"
#include "mdi_datatype.h"
#include "gdi_datatype.h"

#define ACI_PLAY_COMPLETE   0  //播放结束
#define ACI_PLAY_ERROR       1  //播放时遇到错误


typedef void (*ACI_PLAY_CB)(int32 result);


typedef struct
{
	ACI_PLAY_CB cb;  //回调函数
	int32 loop;
	int32 block;
}mr_audio_play_info_t; 


typedef struct
{
	char* buf;
	int32 buf_len;
}mr_audio_load_info_t;


typedef struct
{
    int32 pos;
}mr_audio_position_info_t;


typedef struct
{
	uint8 *data;
	uint32 len;
	int32 loop;
}mr_bg_play_info_t;


typedef struct 
{
	int32 load_type;
	int32 buflen;
	int32 media_format;
	U32 const_seconds;   //单位不是秒，需要通过一个计算公式转换成s
	U32	play_seconds;    //单位不是秒，需要通过一个计算公式转换成s
	kal_uint32 play_time;
	char *pbuf;
	ACI_PLAY_CB playCB;
	mdi_handle mdi_h;
	U16 filename[DSM_MAX_FILE_LEN];
	U8 state;
	U8 volume;
	U8 play_type;
	BOOL need_restore;
	BOOL isBlock;
}mr_audio_ctrl_t;


typedef enum
{
	DSM_CAMERA_QT_LOW,
	DSM_CAMERA_QT_MID,
	DSM_CAMEAR_QT_HIGHT
}mr_camera_quality_enum;


typedef enum
{
	DSM_CAMERA_UNINIT,
	DSM_CAMERA_INITED,
	DSM_CAMERA_PREVIEW,
	DSM_CAMERA_CAPTURED,
	DSM_CAMERA_PAUSED
}mr_camera_state_enum;


typedef struct
{
	uint32 preview_x;
	uint32 preview_y;
	uint32 preview_w;
	uint32 preview_h;
	uint32 img_w;
	uint32 img_h;
	uint32 img_qt;
}mr_camera_launch_info_t;


typedef struct
{
	mr_camera_launch_info_t pre_req;
	gdi_handle osd_layer;
	gdi_handle preview_layer;
	gdi_handle base_layer;
	/* __MMI_DSM_NEW_JSKY__ begin support camera */
	int32 is_from_jsky;
	void *preview_layer_buffer;
	/* __MMI_DSM_NEW_JSKY__ end support camera */
	void *osd_layer_buffer;
	void *pImg;
	int32 ImgSize;
	int32 state;
	int32 last_error;
	int32 camera_handler; /*照相机句柄*/
	int32 continue_capture;
	U16 capsize;
	U16 zoom_max;
	U16 zoom_steps;
	U16 zoom_index;
	U16 tmpName[32];
	U8 *pTmp;
#if defined(__CAMERA_OSD_HORIZONTAL__) 
	int32 req_x;
       int32 req_y;
	int32 req_w;
	int32 req_h;
#endif	
}mr_camera_ctrl_t;


typedef struct
{
	char *src;
	int32 src_len;
	int32 src_type;
	int32 format;
}mr_voice_record_req_info_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * \brief 设备初始化
 *
 * \returm 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_init_device(void);


/**
 * \brief 关闭音频设备
 */
int32 mr_media_close_device(void);


/**
 * \brief 设备释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_free_device(void);


/**
 * \brief 相关变量重置
 */
void mr_media_audio_reset(void);


/**
 * \brief 获取AUDIO状态
 *
 * \return MR_MEDIA_IDLE,MR_MEDIA_INITED等状态值加上MR_PLAT_VALUE_BASE
 */
int32 mr_media_audio_get_state(void);


/**
 * \brief 播放请求
 *
 * \param playReq	[in] 播放请求数据结构
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_audio_play_req(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 暂停播放请求
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_audio_pause_req(void);


/**
 * \brief 停止播放请求
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_audio_stop_req(void);


/**
 * \brief 恢复播放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_audio_resume_req(void);


/**
 * \brief 设置播放音量
 *
 * \param vol	[in]音量值 0-5 从小到大
 */
int32 mr_media_audio_set_vol(int32 vol);


/**
 * \brief 获取当前的音量值
 *
 * \return 音量值
 */
int32 mr_media_audio_get_vol(void);


/**
 * \brief 用于VM被PAUSE后的恢复运行时调用
 */
void mr_media_audio_restore_play(void);


/**
 * \brief IRAM 占用申请
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_apm_alloc_buffer(void);


/**
 * \brief IRAM释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_apm_release_buffer(void);


/**
 * \brief 设置进度
 *
 * \param ms	[in] 毫秒值
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_media_audio_set_progress_time(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_media_audio_get_play_milliseconds(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_media_audio_get_play_seconds(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_media_audio_get_total_time(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_media_audio_load(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


int32 mr_media_pcm_set_write_addr(int32 param);
int32 mr_media_pcm_get_read_addr(int32 param);


int32 mr_media_mma_close(int32 code ,uint8 *input,int32 input_len);
int32 mr_media_mma_stop(int32 code ,uint8 *input,int32 input_len);
int32 mr_media_mma_play(int32 code ,uint8 *input,int32 input_len);
int32 mr_media_mma_open(int32 code ,uint8 *input,int32 input_len);


///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief 初始化CAMERA
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_camera_init(int32 param);


/**
 * \brief 创建CAMERA
 */
int32 mr_camera_create(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 销毁CAMERA
 */
int32 mr_camera_destroy(int32 param);


/**
 * \brief 暂停CAMERA
 */
int32 mr_camera_pause(int32 param);


/**
 * \brief 获取最大的缩放倍数
 *
 * \return the zoom max value
 */
int32 mr_camera_get_zoom_max(int32 param);


/**
 * \brief 缩小
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_camera_zoom_in(int32 param);


/**
 * \brief 放大
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_camera_zoom_out(int32 param);


/**
 * \brief 回复CAMERA扩展
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_camera_resume_ex(int32 param);


/**
 * \brief 获取预览数据
 */
int32 mr_camera_get_preview_data(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 拍照
 */
int32 mr_camera_capture(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief 录音请求
 *
 * \param req	[in]请求参数
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_voice_record_start(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 录音暂停
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_voice_record_pause(int32 param);


/**
 * \brief 录音恢复
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_voice_record_resume(int32 param);


/**
 * \brief 录音停止
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_voice_record_stop(int32 param);


/**
 * \brief 获取录音数据
 */
int32 mr_voice_record_get_buffer(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 是否在录音状态
 */
char dsm_voice_record_stream_isStart(void);

#ifdef __MMI_DSM_NEW_JSKY__
S32 dsm_get_audio_time_ms(void);
#endif /*__MMI_DSM_NEW_JSKY__*/
#endif

