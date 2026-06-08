#ifdef MRAPP_SUPPORT
#ifdef __MR_CFG_CAMERA_SUPPORT__ 

#define DC_RIGHT_TRUNK(x, y)    (((x)+(y)-1)/(y)*(y))

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
	int32 osd_layer;
	int32 preview_layer;
	int32 base_layer;
	int32 is_from_jsky;
	void *preview_layer_buffer;
	void *osd_layer_buffer;
	void *pImg;
	int32 ImgSize;
	int32 state;
	int32 last_error;
	int32 camera_handler; /*照相机句柄*/
	int32 continue_capture;
	uint16 zoom_max;
	uint16 zoom_steps;
	uint16 zoom_index;
	uint16 tmpName[32];
	uint8 *pTmp;
#if defined(__CAMERA_OSD_HORIZONTAL__) 
	int32 req_x;
    int32 req_y;
	int32 req_w;
	int32 req_h;
#endif	
}mr_camera_ctrl_t;


/**
 * \brief 初始化CAMERA
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_camera_init(int32 param);


/**
 * \brief 创建CAMERA
 */
int32 mr_camera_create(void);


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
int32 mr_camera_resume(int32 param);


/**
 * \brief 获取预览数据
 */
int32 mr_camera_get_preview_data(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 拍照
 */
int32 mr_camera_capture(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

    
#endif
#endif//MRAPP_SUPPORT
