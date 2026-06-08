#ifndef __MRP_PAL_MOTION_H__
#define __MRP_PAL_MOTION_H__


#ifdef __DSM_BOSCH_MOTION__
#define MOTION_MAX_ACC   (511)
#else
#define MOTION_MAX_ACC   (64)
#endif

#define MOTION_FREQUENCY   (50)


typedef enum
{
	T_MOTION_PWROFF,
	T_MOTION_PWERON,
	T_MOTION_IDLE,
	T_MOTION_LISTENING
}T_MOTION_STATE;


typedef enum
{
	MR_MOTION_SENSITIVE_LOW,
	MR_MOTION_SENSITIVE_NORMAL,
	MR_MOTION_SENSITIVE_HIGH,
	MR_MOTION_SENSITIVE_MAX
} MR_MOTION_SENSITIVE_ENUM;


typedef enum
{
	MR_MOTION_EVENT_SHAKE,
	MR_MOTION_EVENT_TILT
} mr_motion_event_enum;


typedef struct
{
	int32 x;
	int32 y;
	int32 z;
}T_MOTION_ACC;


/**
 * \brief motion 开启
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_motion_power_on(int32 param);


/**
 * \brief motion 开启
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_motion_power_off(int32 param);


/**
 * \brief 监听shake
 */
int32 mr_motion_listen_shake(int32 param);


/**
 * \brief 
 */
int32 mr_motion_listen_tilt(int32 param);


/**
 * \brief 停止监听
 */
int32 mr_motion_stop_listen(int32 param);


/**
 * \brief 获取 max acc
 */
int32 mr_motion_get_max_acc(int32 param);


#endif
