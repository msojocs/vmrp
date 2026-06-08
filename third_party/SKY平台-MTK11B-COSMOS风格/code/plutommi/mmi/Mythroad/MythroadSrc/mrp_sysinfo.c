#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
//#include "SimDetectionDef.h"
//#include "SimDetectionGprot.h " // 模拟器编译通不过
#include "ProtocolEvents.h"
#include "SSCStringHandle.h"
//#include "SimDetectionHwMapFile.h"
#include "NVRAMEnum.h"
#include "NVRAMType.h"
#include "nvram_enums.h"
#include "nvram_interface.h"
//#include "Mtpnp_pfal_gemini_network.h"

#ifdef __MMI_SKYQQ__
#include "sky_qq_open.h"
#endif
//#ifndef WIN32
#include "EngineerModeDef.h"
//t. #include "EngineerModeProt.h"
#include "EngineerModeType.h"
//#endif

#include "mrp_include.h"
#if (MTK_VERSION > 0x0852)
//#include "EngineerModePCSNetwork.h"
#endif

#include "nwinfosrvgprot.h"
#include "modeswitchsrvgprot.h"
//#include "mmi_rp_app_mainmenu_def.h"

#ifndef __MR_CFG_FEATURE_OVERSEA__
#define DSM_MASTER_SIM_NAME                        "\x00\x53\x00\x49\x00\x4d\x53\x61\x4e\x00\x00\x00"//SIM卡一
#define DSM_SLAVE_SIM_NAME                           "\x00\x53\x00\x49\x00\x4d\x53\x61\x4e\x8c\x00\x00"//SIM卡二
#else
#define DSM_MASTER_SIM_NAME                        "\x00\x53\x00\x49\x00\x4d\x00\x31\x00\x00"//SIM1
#define DSM_SLAVE_SIM_NAME                           "\x00\x53\x00\x49\x00\x4d\x00\x32\x00\x00"//SIM2
#endif

static const char* dsm_sim_names[] = {
	"\x00\x53\x00\x49\x00\x4d\x00\x31\x00\x00",	// SIM1
	"\x00\x53\x00\x49\x00\x4d\x00\x32\x00\x00",	// SIM2
	"\x00\x53\x00\x49\x00\x4d\x00\x33\x00\x00",	// SIM3
	"\x00\x53\x00\x49\x00\x4d\x00\x34\x00\x00",	// SIM4	
};

typedef struct 
{
	uint8 level;
	uint8 current_band;
	uint8 rat;
	uint8 flag;
}T_RX;


char gIMSI[MAX_IMSI_LEN]= {0};
#ifdef __MMI_DUAL_SIM_MASTER__
char gIMSI_2[MAX_IMSI_LEN];
#endif

dsm_rr_em_lai_info_t dsm_rr_em_lai_info = {0};

T_DSM_CELL_INFO dsmCellInfo;
static T_RX *gDsmRx = NULL;
int gThemeMainmenuIndex;
T_DSM_DUALSIM_SET dsmDualSim = {0};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32 mr_sim_is_inserted(int32 simid)
{
	if(srv_sim_ctrl_get_unavailable_cause(simid) ==SRV_SIM_CTRL_UA_CAUSE_NOT_INSERTED)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
void mr_sim_initialize(void)
{
	mmi_sim_enum sim;
	int i;

	memset(&dsmDualSim,0,sizeof(dsmDualSim));

	// 初始化为卡一，避免系统ASSERT
	for (i = 0; i < 4; ++i)
	{
		dsmDualSim.simId[i] = DSM_MASTER_SIM;
	}

	sim = 1;
	for (i = 0; i < SRV_SIM_CTRL_MAX_SIM_NUM; ++i)
	{
		if (srv_sim_ctrl_is_available(sim) && srv_nw_info_get_service_availability(sim) == SRV_NW_INFO_SA_FULL_SERVICE) //&& (srv_mode_switch_get_current_mode() & sim))
		{
			dsmDualSim.simId[dsmDualSim.totalNum] = -(i + 1);
			dsmDualSim.simNamePtr[dsmDualSim.totalNum] = (char*)dsm_sim_names[i];
			dsmDualSim.simNameLen[dsmDualSim.totalNum] = mr_str_wstrlen((char*)dsm_sim_names[i]);	

			dsmDualSim.totalNum++;

			mr_trace("mr_sim_initialize SIM%d OK", i + 1);
		}

		sim <<= 1;
	}
	
}


int32 mr_sim_get_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	*output = (uint8 *)(&dsmDualSim);
	*output_len = sizeof(dsmDualSim);
	return MR_SUCCESS;
}


int32 mr_sim_set_active(int32 simId)
{
	int i = 0;	
	
	for(i =0;i<dsmDualSim.totalNum;i++)
	{
		if(dsmDualSim.simId[i] == simId)
		{			
			dsmDualSim.curActive = i;
			#ifdef __MMI_SKYQQ__
                     QQ2008_saveCurSimSet();
                   #endif       
			return MR_SUCCESS;
		}
	}
	
	return MR_FAILED;
}


int32 mr_sim_get_active(void)
{
	return dsmDualSim.simId[dsmDualSim.curActive];
}

void mr_imei_initialize(void)
{
	/*
	* 在以前老的版本中获取IMEI要是异步操作,在新版本中已提供接口直接获取
	*/
}


void mr_imsi_initialize(void)
{
	/*
	* 在以前老的版本中获取IMSI要是异步操作,在新版本中已提供接口直接获取
	*/
}


int32 mr_bsid_initialize(int32 param)
{
	return MR_SUCCESS;
}


int32 mr_get_cellinfo(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mmi_sim_enum sim_id;
	srv_nw_info_location_info_struct nw_info;

	sim_id = mr_sim_active_id_dsm2mmi();

	if (!srv_nw_info_get_location_info(sim_id, &nw_info)) return MR_FAILED;

	dsmCellInfo.lac = nw_info.lac;
	dsmCellInfo.cell_id = nw_info.cell_id;
	dsmCellInfo.mcc[0] = 0;
	dsmCellInfo.mcc[1] = 0;
	dsmCellInfo.mcc[2] = 0;
	dsmCellInfo.mnc[0] = 0;
	dsmCellInfo.mnc[1] = 0;
	dsmCellInfo.mnc3[0] = 0;

	*output = (uint8*)&dsmCellInfo;
	*output_len = sizeof(dsmCellInfo);

	return MR_SUCCESS;
}


int32 mr_bsid_terminate(int32 param)
{
	return MR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////


int32 mr_signal_get_level(uint8**output,int32 *output_len)
{
	mmi_sim_enum sim_id;
	static T_RX rx;

	sim_id = mr_sim_active_id_dsm2mmi();

	rx.level = srv_nw_info_get_signal_strength_in_percentage(sim_id);
	rx.current_band = srv_nw_info_get_camp_on_band(sim_id);
	rx.rat = srv_nw_info_is_roaming(sim_id);
	rx.flag = 1;

	*output = (uint8*)&rx;
	*output_len = sizeof(T_RX);

	return MR_SUCCESS;		
}


int32 mr_signal_initialize(int32 param)
{	
	return MR_SUCCESS;
}


int32 mr_signal_terminate(int32 param)
{
	return MR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////


static void mr_mainmenu_release(uint8* output, int32 output_len)
{
	OslMfree((void *)output);
}


int32 mr_get_mainmenu_info(uint8**output,int32 *output_len,MR_PLAT_EX_CB *cb)
{
#if 0
	// 这段代码会造成进入MRP应用后按OK键会进入系统的快捷方式菜单
	// 用处不大的话可否去掉
	int i=0;
	MMI_ID_TYPE mm_stringIDs[MAX_SUB_MENUS];
	U16 nMenuItemList[MAX_SUB_MENUS];
	T_MAINMENU *pMMenu;
	FuncPtr leftFuncDown,rightFuncDown,leftFuncUp,rightFuncUp;
	FuncPtr arrowLeftDown,arrowLeftUp,arrowRightDown,arrowRightUp,arrowUpDown,arrowUpUp,arrowDownDown,arrowDownUp;
#if (MTK_VERSION < 0x0816)	
	extern hiliteInfo maxHiliteInfo[];
#endif
	extern void DsmBook(void);
	extern void DsmGame(void);
	
	if(output ==NULL||output_len == NULL||cb == NULL)
		return MR_FAILED;
			
	pMMenu = OslMalloc(sizeof(T_MAINMENU));

	if(pMMenu == NULL)
		return MR_FAILED;

	memset(pMMenu,0,sizeof(T_MAINMENU));
	GetSequenceItemIds(IDLE_SCREEN_MENU_ID, nMenuItemList);
	GetSequenceStringIds(IDLE_SCREEN_MENU_ID, mm_stringIDs);
	
	pMMenu->num = GetNumOfChild(IDLE_SCREEN_MENU_ID);

	pMMenu->index = gThemeMainmenuIndex;
	
	ASSERT(pMMenu->num > 0);

	leftFuncDown = GetKeyHandler(KEY_LSK,KEY_EVENT_DOWN);
	leftFuncUp = GetKeyHandler(KEY_LSK,KEY_EVENT_UP);

	rightFuncDown = GetKeyHandler(KEY_RSK,KEY_EVENT_DOWN);
	rightFuncUp = GetKeyHandler(KEY_RSK,KEY_EVENT_UP);

	arrowLeftDown = GetKeyHandler(KEY_LEFT_ARROW,KEY_EVENT_DOWN);
	arrowLeftUp = GetKeyHandler(KEY_LEFT_ARROW,KEY_EVENT_UP);

	arrowRightDown = GetKeyHandler(KEY_RIGHT_ARROW,KEY_EVENT_DOWN);
	arrowRightUp = GetKeyHandler(KEY_RIGHT_ARROW,KEY_EVENT_UP);

	arrowUpDown = GetKeyHandler(KEY_UP_ARROW,KEY_EVENT_DOWN);
	arrowUpUp = GetKeyHandler(KEY_UP_ARROW,KEY_EVENT_UP);

	arrowDownDown = GetKeyHandler(KEY_DOWN_ARROW,KEY_EVENT_DOWN);
	arrowDownUp = GetKeyHandler(KEY_DOWN_ARROW,KEY_EVENT_UP);
	
	gdi_layer_lock_frame_buffer();
	for(i = 0;i<pMMenu->num;i++)
	{
		pMMenu->item[i].name = GetString(mm_stringIDs[i]);
#if (MTK_VERSION < 0x0816)
		if(maxHiliteInfo[nMenuItemList[i]].entryFuncPtr)
		{
			maxHiliteInfo[nMenuItemList[i]].entryFuncPtr();
			pMMenu->item[i].func = get_left_softkey_function(KEY_EVENT_UP);
		}
#else
		{
			FuncPtr func= mmi_frm_get_hilite_hdlr(nMenuItemList[i]);
			if(func)
			{
				func();
				pMMenu->item[i].func = get_left_softkey_function(KEY_EVENT_UP);
			}
		}
#endif
	}
	gdi_layer_unlock_frame_buffer();
	
	pMMenu->exitFunc = GoBackHistory;
	#ifdef __MR_CFG_ENTRY_SKYREAD__
	pMMenu->bookFunc = DsmBook;
	#endif
	pMMenu->gameFunc = DsmGame;
	SetKeyHandler(leftFuncDown, KEY_LSK, KEY_EVENT_DOWN);
	SetKeyHandler(leftFuncUp, KEY_LSK, KEY_EVENT_UP);

	SetKeyHandler(rightFuncDown, KEY_RSK, KEY_EVENT_DOWN);
	SetKeyHandler(rightFuncUp, KEY_RSK, KEY_EVENT_UP);

	SetKeyHandler(arrowLeftDown, KEY_LEFT_ARROW, KEY_EVENT_DOWN);
	SetKeyHandler(arrowLeftUp, KEY_LEFT_ARROW, KEY_EVENT_UP);

	SetKeyHandler(arrowRightDown, KEY_RIGHT_ARROW, KEY_EVENT_DOWN);
	SetKeyHandler(arrowRightUp, KEY_RIGHT_ARROW, KEY_EVENT_UP);

	SetKeyHandler(arrowUpDown, KEY_UP_ARROW, KEY_EVENT_DOWN);
	SetKeyHandler(arrowUpUp, KEY_UP_ARROW, KEY_EVENT_UP);

	SetKeyHandler(arrowDownDown, KEY_DOWN_ARROW, KEY_EVENT_DOWN);
	SetKeyHandler(arrowDownUp, KEY_DOWN_ARROW, KEY_EVENT_UP);
	
	*output = (uint8 *)pMMenu;
	*output_len = sizeof(T_MAINMENU);
	*cb = mr_mainmenu_release;
	return MR_SUCCESS;

#else

	return MR_FAILED;

#endif
}

int mr_sim_active_id_dsm2mmi(void)
{
	mmi_sim_enum sim;
	int32 dsm_sim;

	dsm_sim = mr_sim_get_active();

	mr_trace("mr_sim_active_id_dsm2mmi sim=%d", dsm_sim);
	
	/*
	* 下边的代码中sim直接使用数值而不使用宏是为了和以前的版本兼容而且也不需要
	* 使用版本宏控制， 例如10A1032版本中就没有定义MMI_SIM3和MMI_SIM4
	*/
	if (dsm_sim == DSM_SLAVE_SIM)
	{
		sim = 0x0002;	// MMI_SIM2
	}
	else if (dsm_sim == DSM_THIRD_SIM)
	{
		sim = 0x0004;	// MMI_SIM3
	}
	else if (dsm_sim == DSM_FOURTH_SIM)
	{
		sim = 0x0008;	// MMI_SIM4
	}
	else
	{
		sim = 0x0001;	// MMI_SIM1
	}

	return sim;
}

int mr_sim_active_id_dsm2phb(void)
{
	int phb_sim;
	int32 dsm_sim;

	dsm_sim = mr_sim_get_active();

	/*
	* 下边的代码中sim直接使用数值而不使用宏是为了和以前的版本兼容而且也不需要
	*  使用版本宏控制， 例如10A1032版本中就没有定义PHB_STORAGE_SIM3和PHB_STORAGE_SIM4
	*/
	if (dsm_sim == DSM_SLAVE_SIM)
	{
		phb_sim = 3;	// PHB_STORAGE_SIM2
	}
	else if (dsm_sim == DSM_THIRD_SIM)
	{
		phb_sim = 4;	// PHB_STORAGE_SIM3
	}
	else if (dsm_sim == DSM_FOURTH_SIM)
	{
		phb_sim = 5;	// PHB_STORAGE_SIM4
	}
	else
	{
		phb_sim = 1;	// PHB_STORAGE_SIM
	}

	return phb_sim;	
}

int32 mr_sim_active_is_full_service(void)
{
	mmi_sim_enum sim;
	srv_nw_info_service_availability_enum nw_info_service;

	sim = mr_sim_active_id_dsm2mmi();

	nw_info_service = srv_nw_info_get_service_availability(sim);

	if (nw_info_service == SRV_NW_INFO_SA_FULL_SERVICE)
	{
		return TRUE;
	}	
	else
	{
		return FALSE;
	}
}


#include "vapp_cal_prot.h"
#include "todolistsrvgprot.h"
#include "app_datetime.h"
#include "datetimetype.h"

#define MR_TASK_SUBJECT_SIZE  MAX_TODO_LIST_NOTE
#define MR_TASK_DETAILS_SIZE  MAX_TDL_DETAILS_SIZE * ENCODING_LENGTH
typedef struct
{
	/*主题和详情注意大小端字节序,UNICODE 编码*/
	char        subject[MR_TASK_SUBJECT_SIZE];/*主题*/
	char        details[MR_TASK_DETAILS_SIZE];/*详情*/
	
	int32            alarmtype;                           /*闹钟提醒参数*/
	int32            repeat;				 /*重复提醒参数 */
	int32            priority;				 /*优先级*/
	int32            status;				 /*状态*/
	uint8            days[8] ;                       /*周内每天的设置,只用前7天*/
	mr_datetime dtime;				 /*日期加时间,秒参数无用*/	
	
}MR_TASK_STRUCT;

/*重复提醒参数*/
typedef enum
{ 
	MR_TASK_RTYPE_ONCE,
	MR_TASK_RTYPE_DAILY,
	MR_TASK_RTYPE_DAYS,
	MR_TASK_RTYPE_WEEKLY,
	MR_TASK_RTYPE_MONTHLY,
	MR_TASK_RTYPE_YEARLY,
}MR_TASK_REPEAT_TYPE;

/*闹钟提醒参数*/
typedef enum
{
	MR_TASK_ATYPE_CLOSE,
	MR_TASK_ATYPE_OPEN,
	MR_TASK_ATYPE_FIVE,
	MR_TASK_ATYPE_TEN,
	MR_TASK_ATYPE_FIFTEEN,
	MR_TASK_ATYPE_THIRTY,
}MR_TASK_ALARM_TYPE;


srv_tdl_task_struct g_task = {0};


static const U8 g_task_days_in_week[] = 
{
    0x01,   /* DAY_SUN */
    0x02,   /* DAY_MON */
    0x04,   /* DAY_TUE */
    0x08,   /* DAY_WED */
    0x10,   /* DAY_THU */
    0x20,   /* DAY_FRI */
    0x40    /* DAY_SAT */
};
void mr_task_setfirst_launchtime(const MYTIME *baseTime,  MYTIME *remimderTime,int32  reminderType)
{
	MYTIME decrement;
	MYTIME *base;

	base = (MYTIME*)baseTime;

	memset(&decrement, 0, sizeof(MYTIME));
	switch(reminderType)
	{
	case VAPP_CAL_REMINDER_5MINS_EARLIER:
		decrement.nMin= 5;
		break;

	case VAPP_CAL_REMINDER_15MINS_EARLIER:
		decrement.nMin = 15;
		break;

	case VAPP_CAL_REMINDER_30MINS_EARLIER:
		decrement.nMin = 30;
		break;

	case VAPP_CAL_REMINDER_1HOUR_EARLIER:
		decrement.nHour = 1;
		break;

	case VAPP_CAL_REMINDER_1DAY_EARLIER:
		decrement.nDay = 1;
		break;

	case VAPP_CAL_REMINDER_3DAYS_EARLIER:
		decrement.nDay = 3;
		break;

	case VAPP_CAL_REMINDER_1WEEK_EARLIER:
		decrement.nDay = 7;
		break;

	default:
		break;
	}

	applib_dt_decrease_time(
		(applib_time_struct*)base,
		(applib_time_struct*)&decrement,
		(applib_time_struct*)remimderTime);
}
int32 mr_task_set_func(MR_TASK_STRUCT* ptask)
{
#ifdef __MMI_SRV_TODOLIST__
	U16 i,j;
	uint8 type = 0;
	applib_time_struct curtime;
	int32 ret = 0;
	memset(&g_task,0,sizeof(srv_tdl_task_struct));
	
	/*时间*/
	g_task.due_time.nYear = ptask->dtime.year;
	g_task.due_time.nMonth= ptask->dtime.month;
	g_task.due_time.nDay= ptask->dtime.day;
	g_task.due_time.nHour= ptask->dtime.hour;
	g_task.due_time.nMin= ptask->dtime.minute;
	g_task.due_time.nSec= ptask->dtime.second;
	if (LastDayOfMonth(ptask->dtime.month,ptask->dtime.year)  <   ptask->dtime.day)
	{
		/*日期有误*/
		return MR_FAILED;
	}

	/*闹钟类型*/
	switch(ptask->alarmtype)
	{
		case MR_TASK_ATYPE_CLOSE:
			type = VAPP_CAL_REMINDER_NEVER;
			break;
		case MR_TASK_ATYPE_OPEN:
			type = VAPP_CAL_REMINDER_5MINS_EARLIER;
			break;
		case MR_TASK_ATYPE_FIVE:
			type = VAPP_CAL_REMINDER_5MINS_EARLIER;
			break;
		case MR_TASK_ATYPE_TEN:
			type = VAPP_CAL_REMINDER_5MINS_EARLIER;
			break;
		case MR_TASK_ATYPE_FIFTEEN:
			type = VAPP_CAL_REMINDER_15MINS_EARLIER;
			break;
		case MR_TASK_ATYPE_THIRTY:
			type = VAPP_CAL_REMINDER_30MINS_EARLIER;
			break;
		default:
			type = VAPP_CAL_REMINDER_5MINS_EARLIER;
			break;
		
	}
	g_task.alarm.type = type;
	g_task.alarm.snooze = 1;
	mr_task_setfirst_launchtime(&(g_task.due_time),&(g_task.alarm.first_launch),type);
	applib_dt_get_rtc_time(&curtime);
	ret = applib_dt_compare_time((applib_time_struct *)&(g_task.alarm.first_launch),&curtime, NULL);
	if (ret == DT_TIME_LESS)
	{
	    return MR_FAILED;
	}
	
	switch(ptask->repeat)
	{
		case MR_TASK_RTYPE_ONCE:
			type = VAPP_CAL_REPEAT_NEVER;
			break;
		case MR_TASK_RTYPE_DAILY:
			type = VAPP_CAL_REPEAT_DAILY;
			break;
		case MR_TASK_RTYPE_DAYS:
			type = VAPP_CAL_REPEAT_NEVER;
			break;
		case MR_TASK_RTYPE_WEEKLY:
			type = VAPP_CAL_REPEAT_WEEKLY;
			break;
		case MR_TASK_RTYPE_MONTHLY:
			type = VAPP_CAL_REPEAT_MONTHLY;
			break;
		case MR_TASK_RTYPE_YEARLY:
			type = VAPP_CAL_REPEAT_YEARLY;
			break;
		default:
			type = VAPP_CAL_REPEAT_NEVER;
		
	}
	
	g_task.repeat.rule = type;
	#if 0
	g_task.repeat.dows = 0;
	if (g_task.repeat.rule == MMI_TDL_RULE_DAYS)
	{
		for (j = 0; j < MMI_TDL_MAX_DAYS_PER_WEEK; j++)
		{
			if (ptask->days[j])
			{
				g_task.repeat.dows |= g_task_days_in_week[j];
			}
		}

		/* Reset to once */
		if (g_task.repeat.dows == 0)
		{
			g_task.repeat.rule = MMI_TDL_RULE_ONCE;
		}
	}
	#endif
	/* subject */
	if (wcslen((WCHAR*)ptask->subject) == 0)
	{
		/*内容为空*/
		return MR_FAILED;
	}
	else
	{
		if(wcslen((WCHAR*)ptask->subject) < MR_TASK_SUBJECT_SIZE / ENCODING_LENGTH)
		{
			wcscpy((WCHAR*)g_task.subject, (WCHAR*)ptask->subject);
		}
		else
		{
			memcpy(g_task.subject,ptask->subject,MAX_TODO_LIST_NOTE);
		}
	}
    
	/* Details */   
	if(wcslen((WCHAR*)ptask->details) < MR_TASK_DETAILS_SIZE / ENCODING_LENGTH )
	{
		wcscpy((WCHAR*)g_task.details, (WCHAR*)ptask->details);
	}
	else
	{
		memcpy(g_task.details,ptask->details,MR_TASK_DETAILS_SIZE);
	}

	/*优先 级*/
	g_task.priority = ptask->priority;/*低中高三级,0,1,2*/
	if(g_task.priority < 0)
	{
		g_task.priority = 0;
	}
	else if(g_task.priority > 2)
	{
		g_task.priority = 2;
	}
 
	/*状态*/
	g_task.status = ptask->status;   /*0:undo,1:done*/
	if(g_task.status < 0)
	{
		g_task.status = 0;
	}
	else if(g_task.status > 1)
	{
		g_task.status = 1;
	}
	
	
	g_task.present =  1;
 	g_task.vcal = SRV_TDL_VCAL_TASK;
 	if(SRV_TDL_RESULT_OK != srv_tdl_add(
                    (void*)&g_task, 
                    SRV_TDL_VCAL_TASK, 
                    &i))
 	{
		return MR_FAILED;		
	}
	if(g_task.alarm.type > VAPP_CAL_REMINDER_NEVER)
	{
		g_task.alarm.first_launch.nSec = 0;
		srv_reminder_set(2, &(g_task.alarm.first_launch), i);
//		srv_reminder_set(SRV_REMINDER_TYPE_TASK, &(g_task.alarm.first_launch), i);
	}
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif

} 

int32 mr_set_add_task(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	MR_TASK_STRUCT* ptask = (MR_TASK_STRUCT*)input;
	if(ptask && (sizeof(MR_TASK_STRUCT) == input_len))
	{
		return mr_task_set_func(ptask);
	}
	return MR_FAILED;
}

//以下是测试代码
#if 0
#define CONTENT "Test Test" 

#define DETIALS "Detail Detail"

MR_TASK_STRUCT g_test;


int mr_set_task(void)
{
#ifdef __MMI_SRV_TODOLIST__
		memset(&g_test,0,sizeof(MR_TASK_STRUCT));

		ascii_to_ucs2(CONTENT, (WCHAR*)g_test.subject);
		ascii_to_ucs2(DETIALS, (WCHAR*)g_test.details);
		
		g_test.alarmtype = MR_TASK_ATYPE_FIVE;
		g_test.repeat = MR_TASK_RTYPE_DAILY;
//		g_test.repeat = MR_TASK_RTYPE_DAYS;
		g_test.days[0] = 1;
		g_test.days[1] = 0;
		g_test.days[2] = 1;
		g_test.days[3] = 0;
		g_test.days[4] = 1;
		g_test.days[5] = 0;
		g_test.days[6] = 1;
		
		g_test.priority = 2;
		g_test.status= 0;
		/*
		g_test.dtime.year = 2011;
		g_test.dtime.month= 4;
		g_test.dtime.day= 7;
		g_test.dtime.hour = 21;
		g_test.dtime.minute = 00;
		*/
		mr_getDatetime(&(g_test.dtime));
		
		if(g_test.dtime.minute < 50)
		{
			g_test.dtime.minute += 7;
		}
		else
		{
			g_test.dtime.hour += 1; /*测试代码，小时再超，先不处理*/
			g_test.dtime.minute = 7;
		}
		return mr_set_add_task((uint8*)&g_test,sizeof(MR_TASK_STRUCT),0,0,0);

#else
		return MR_IGNORE;	
#endif
}
#endif
#endif

