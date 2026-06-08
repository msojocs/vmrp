#ifdef MRAPP_SUPPORT

#include "mmimrapp_export.h"
#include "mmimrapp_id.h"
#include "mmk_gsensor.h"
#include "mmimrapp_motion.h"

#ifdef __MRAPP_MOTION_SUPPORT__

static T_MOTION_STATE gdsmMotionState = T_MOTION_PWROFF;
static void (*gDsmMotionCb)(void) = NULL;
static uint8   g_mr_motion_timer = 0;

static void dsm_motion_read_acc_handler(void)
{
	if((gdsmMotionState == T_MOTION_LISTENING) &&gDsmMotionCb)
	{
		gDsmMotionCb();
	}
}

static void dsm_motion_check_data(T_MOTION_ACC *p,T_MOTION_ACC *data)
{
	/*将平台的xyz坐标系和VM的坐标系对应*/	
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
}

static void dsm_motion_shake_cb(void)
{
    T_MOTION_ACC p = {0};
    T_MOTION_ACC data = {0};
    MMK_GSENSOR_VECTOR_T vector = {0};
    
    if(gdsmMotionState == T_MOTION_LISTENING)
    {
    	if(TRUE == MMK_GetGsensorVector(&vector))
        {   
            data.x = vector.x;
            data.y = vector.y;
            data.z = vector.z;
    		dsm_motion_check_data(&p, &data);
            if((p.x > (MOTION_MAX_ACC/2))||(p.x < -(MOTION_MAX_ACC/2))
            	||(p.y > (MOTION_MAX_ACC/2))||(p.y < -(MOTION_MAX_ACC/2))
            	||(p.z > (MOTION_MAX_ACC/2))||(p.z < -(MOTION_MAX_ACC/2))
              )
            {      
    			mr_event(MR_MOTION_EVENT, MR_MOTION_EVENT_SHAKE, (int32)&p);	
            }
        }
    }
}

static void dsm_motion_tilt_cb(void)
{
    T_MOTION_ACC p = {0};
    T_MOTION_ACC data = {0};
    MMK_GSENSOR_VECTOR_T vector = {0};
    if(gdsmMotionState == T_MOTION_LISTENING)
    {
    	if(TRUE == MMK_GetGsensorVector(&vector))
        {   
            data.x = vector.x;
            data.y = vector.y;
            data.z = vector.z;
    		dsm_motion_check_data(&p,&data);
            if((p.x > (MOTION_MAX_ACC/2))||(p.x < -(MOTION_MAX_ACC/2))
            	||(p.y > (MOTION_MAX_ACC/2))||(p.y < -(MOTION_MAX_ACC/2))
            	||(p.z > (MOTION_MAX_ACC/2))||(p.z < -(MOTION_MAX_ACC/2))
              )
            {      
    			mr_event(MR_MOTION_EVENT, MR_MOTION_EVENT_TILT, (int32)&p);	
            }
        }
    }
}	

int32 mr_motion_stop_listen(int32 param)
{
	gDsmMotionCb = NULL;
	gdsmMotionState = T_MOTION_IDLE;
    MMK_CloseGsensor(MMIMRAPP_BASE_WIN_ID);
    MMK_StopTimer(g_mr_motion_timer);
    g_mr_motion_timer = 0;
	return MR_SUCCESS;
}

int32 mr_motion_power_on(int32 param)
{
	gDsmMotionCb = NULL;

    if(TRUE == MMK_OpenGsensor(MMIMRAPP_BASE_WIN_ID))
    {
    	gdsmMotionState = T_MOTION_PWERON;
    	return MR_SUCCESS;
    }
    else
    {
    	gdsmMotionState = T_MOTION_PWROFF;
    	return MR_FAILED;
    }
}

int32 mr_motion_power_off(int32 param)
{
	gDsmMotionCb = NULL;
	gdsmMotionState = T_MOTION_PWROFF;
    MMK_StopTimer(g_mr_motion_timer);
    g_mr_motion_timer = 0;
	return MR_SUCCESS;
}

int32 mr_motion_listen_shake(int32 param)
{
	if(param >=MR_MOTION_SENSITIVE_MAX)
		return MR_FAILED;

	if(gdsmMotionState == T_MOTION_PWROFF)
		return MR_FAILED;

	gdsmMotionState = T_MOTION_LISTENING;
	gDsmMotionCb = dsm_motion_shake_cb;
    if(g_mr_motion_timer != 0)
    {
        MMK_StopTimer(g_mr_motion_timer);
    }
    g_mr_motion_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, MOTION_FREQUENCY, TRUE);
	return MR_SUCCESS;
}

int32 mr_motion_listen_tilt(int32 param)
{
	if(param >=MR_MOTION_SENSITIVE_MAX)
		return MR_FAILED;

	if(gdsmMotionState == T_MOTION_PWROFF)
		return MR_FAILED;

	gdsmMotionState = T_MOTION_LISTENING;
	gDsmMotionCb = dsm_motion_shake_cb;
    if(g_mr_motion_timer != 0)
    {
        MMK_StopTimer(g_mr_motion_timer);
    }
    g_mr_motion_timer = MMK_CreateWinTimer(VIRTUAL_WIN_ID, MOTION_FREQUENCY, TRUE);
	return MR_SUCCESS;
}

int32 mr_motion_get_max_acc(int32 param)
{
	return (MR_PLAT_VALUE_BASE+MOTION_MAX_ACC);
}

int32 mr_motion_handle_timer(uint8 timer_id)
{
    int32 ret = 0;
    if(timer_id == g_mr_motion_timer)
    {
        dsm_motion_read_acc_handler();
        ret = 1;
    }
    return ret;
}

#endif
#endif

