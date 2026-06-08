#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "motion_sensor.h"
#include "mrp_include.h"


static T_MOTION_STATE gdsmMotionState = T_MOTION_PWROFF;
static FuncPtr gDsmMotionCb  = NULL;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __DSM_ADI_MOTION__
extern signed char GSensor_read_X_value(void);
extern signed char GSensor_read_Y_value(void);
extern signed char GSensor_read_Z_value(void);
#endif


#ifdef __DSM_BOSCH_MOTION__
typedef struct
{
	short x,y,z;
}smb380acc_t;
extern int smb380_read_accel_xyz(smb380acc_t *acc);
extern void acc_sensor_pwr_up(void);
extern void acc_sensor_pwr_down(void);
#endif


#ifdef __DSM_MOTION_MTK_FRAME__
extern void motion_sensor_cb_registration(MS_DATA_FUNC cb_fun, void *para);
static void dsm_moiton_dummy_cb(void *parameter, Motion_Sensor_BuffState_enum state){;}
static void dsm_motion_read_xyz(T_MOTION_ACC *acc)
{
	if(acc != NULL)
	{
		MotionSensorDataStruct data;
		acc->x = 0;
		acc->y = 0;
		acc->z = 0;
		
		if(motion_sensor_get_data(&data))
		{
			acc->x = data.x_acc;
			acc->y = data.y_acc;
			acc->z = data.z_acc;
		}
	}
}

#endif


static void dsm_motion_read_acc_handler(void)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	if(gdsmMotionState == T_MOTION_LISTENING&&gDsmMotionCb)
	{
		gDsmMotionCb();
	}
	gui_start_timer(MOTION_FREQUENCY,dsm_motion_read_acc_handler);
#endif
}


static void dsm_motion_check_data(T_MOTION_ACC *p,T_MOTION_ACC *data)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	/*将平台的xyz坐标系和邋VM的坐标系对应*/	
	p->x = -data->x;
	p->y = data->y;
	p->z = data->z;

	if(p->x > MOTION_MAX_ACC)
		p->x = MOTION_MAX_ACC;
	else if(p->x <-MOTION_MAX_ACC)
		p->x = -MOTION_MAX_ACC;
	
	if(p->y > MOTION_MAX_ACC)
		p->y = MOTION_MAX_ACC;
	else if(p->y <-MOTION_MAX_ACC)
		p->y = -MOTION_MAX_ACC;

	if(p->z > MOTION_MAX_ACC)
		p->z = MOTION_MAX_ACC;
	else if(p->z <-MOTION_MAX_ACC)
		p->z = -MOTION_MAX_ACC;	
#endif	
}


static void dsm_motion_shake_cb(void)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	T_MOTION_ACC p,data;
	if(mr_app_get_state() == DSM_RUN)
	{
#ifdef __DSM_ADI_MOTION__
		data.x = GSensor_read_X_value();
		data.y = GSensor_read_Y_value();
		data.z = GSensor_read_Z_value();
#endif
#ifdef __DSM_BOSCH_MOTION__
		smb380acc_t acc;
		smb380_read_accel_xyz(& acc);
		data.x = acc.x;
		data.y = acc.y;
		data.z = acc.z;
#endif
#ifdef __DSM_MOTION_MTK_FRAME__
		dsm_motion_read_xyz(&data);
#endif
		dsm_motion_check_data(&p,&data);
		if((p.x > (MOTION_MAX_ACC/2))||(p.x < -(MOTION_MAX_ACC/2))
			||(p.y > (MOTION_MAX_ACC/2))||(p.y < -(MOTION_MAX_ACC/2))
			||(p.z > (MOTION_MAX_ACC/2))||(p.z < -(MOTION_MAX_ACC/2))
		)
			mr_event(MR_MOTION_EVENT,MR_MOTION_EVENT_SHAKE, (int32)&p);	
	}
#endif	
}


static void dsm_motion_tilt_cb(void)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	T_MOTION_ACC p,data;
	if(mr_app_get_state() == DSM_RUN)
	{
#ifdef __DSM_ADI_MOTION__
		data.x = GSensor_read_X_value();
		data.y = GSensor_read_Y_value();
		data.z = GSensor_read_Z_value();
#endif

#ifdef __DSM_BOSCH_MOTION__
		smb380acc_t acc;
		smb380_read_accel_xyz(& acc);
		data.x = acc.x;
		data.y = acc.y;
		data.z = acc.z;
#endif
#ifdef __DSM_MOTION_MTK_FRAME__
		dsm_motion_read_xyz(&data);
#endif
		dsm_motion_check_data(&p,&data);
		mr_event(MR_MOTION_EVENT,MR_MOTION_EVENT_TILT, (int32)&p);
	}
#endif	
}	


int32 mr_motion_stop_listen(int32 param)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	gDsmMotionCb = NULL;
	gdsmMotionState = T_MOTION_IDLE;
	gui_cancel_timer(dsm_motion_read_acc_handler);
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_motion_power_on(int32 param)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__

#ifdef __DSM_ADI_MOTION__
	GSensor_init_ADI();
#endif
#ifdef __DSM_BOSCH_MOTION__
	acc_sensor_pwr_up();
#endif
#ifdef __DSM_MOTION_MTK_FRAME__
	mdi_motion_power_on();
#endif
	gDsmMotionCb = NULL;
	gdsmMotionState = T_MOTION_PWERON;
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_motion_power_off(int32 param)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	gDsmMotionCb = NULL;
	gdsmMotionState = T_MOTION_PWROFF;
	gui_cancel_timer(dsm_motion_read_acc_handler);
#ifdef __DSM_ADI_MOTION__
	stop_read_position_status_handler();
	GSensor_entersleep_ADI();
#endif

#ifdef __DSM_BOSCH_MOTION__
	acc_sensor_pwr_down();
#endif

#ifdef __DSM_MOTION_MTK_FRAME__
	mdi_motion_power_off();
#endif
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_motion_listen_shake(int32 param)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	if(param >=MR_MOTION_SENSITIVE_MAX)
		return MR_FAILED;

	if(gdsmMotionState == T_MOTION_PWROFF)
		return MR_FAILED;
	
#ifdef __DSM_MOTION_MTK_FRAME__
	mdi_motion_start_listen_shake(param, dsm_motion_shake_cb);
	motion_sensor_cb_registration(dsm_moiton_dummy_cb,NULL);
#endif

	gdsmMotionState = T_MOTION_LISTENING;
	gDsmMotionCb = dsm_motion_shake_cb;
	gui_cancel_timer(dsm_motion_read_acc_handler);
	gui_start_timer(MOTION_FREQUENCY,dsm_motion_read_acc_handler);
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_motion_listen_tilt(int32 param)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	if(param >= MR_MOTION_SENSITIVE_MAX)
		return MR_FAILED;
	if(gdsmMotionState == T_MOTION_PWROFF)
		return MR_FAILED;

#ifdef __DSM_MOTION_MTK_FRAME__
	mdi_motion_start_listen_shake(param, dsm_motion_tilt_cb);
	motion_sensor_cb_registration(dsm_moiton_dummy_cb,NULL);
#endif

	gdsmMotionState = T_MOTION_LISTENING;
	gDsmMotionCb = dsm_motion_tilt_cb;
	gui_cancel_timer(dsm_motion_read_acc_handler);
	gui_start_timer(MOTION_FREQUENCY,dsm_motion_read_acc_handler);
	
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_motion_get_max_acc(int32 param)
{
#ifdef __MMI_DSM_MOTION_SUPPORT__
	return (MR_PLAT_VALUE_BASE+MOTION_MAX_ACC);
#else
	return MR_IGNORE;
#endif
}


#endif

