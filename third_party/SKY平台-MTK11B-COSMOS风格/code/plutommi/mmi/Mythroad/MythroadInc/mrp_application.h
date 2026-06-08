#ifndef __MRP_APPLICATION_H__
#define __MRP_APPLICATION_H__


//#include "TouchScreenGprot.h"		// 11A COSMOS


#ifdef __SENDKEY2_SUPPORT__
#define DSM_DEAL_KEYS  \
	KEY_0,\
	KEY_1,\
	KEY_2,\
	KEY_3,\
	KEY_4,\
	KEY_5,\
	KEY_6,\
	KEY_7,\
	KEY_8,\
	KEY_9,\
	KEY_STAR,\
	KEY_POUND,\
	KEY_LSK,\
	KEY_RSK,\
	KEY_UP_ARROW,\
	KEY_DOWN_ARROW,\
	KEY_LEFT_ARROW,	\
	KEY_RIGHT_ARROW,\
	KEY_VOL_UP,\
	KEY_VOL_DOWN,\
	KEY_SEND1,\
	KEY_ENTER,\
	KEY_CAMERA,\
	KEY_BACK,\
	KEY_SEND2
#else
#define DSM_DEAL_KEYS  \
	KEY_0,\
	KEY_1,\
	KEY_2,\
	KEY_3,\
	KEY_4,\
	KEY_5,\
	KEY_6,\
	KEY_7,\
	KEY_8,\
	KEY_9,\
	KEY_STAR,\
	KEY_POUND,\
	KEY_LSK,	\
	KEY_RSK,\
	KEY_UP_ARROW,\
	KEY_DOWN_ARROW,\
	KEY_LEFT_ARROW,	\
	KEY_RIGHT_ARROW,\
	KEY_VOL_UP,\
	KEY_VOL_DOWN,\
	KEY_SEND,\
	KEY_ENTER,\
	KEY_BACK,\
	KEY_CAMERA
#endif


#ifdef __MR_CFG_FEATURE_QWERTY_KEYPAD__
#define DSM_DEAL_QWERTY_KEYS  \
    KEY_A,\
    KEY_B,\
    KEY_C,\
    KEY_D,\
    KEY_E,\
    KEY_F,\
    KEY_G,\
    KEY_H,\
    KEY_I,\
    KEY_J,\
    KEY_K,\
    KEY_L,\
    KEY_M,\
    KEY_N,\
    KEY_O,\
    KEY_P,\
    KEY_Q,\
    KEY_R,\
    KEY_S,\
    KEY_T,\
    KEY_U,\
    KEY_V,\
    KEY_W,\
    KEY_X,\
    KEY_Y,\
    KEY_Z,\
    KEY_SPACE,\
    KEY_TAB,\
    KEY_DEL,\
    KEY_ALT,\
    KEY_CTRL,\
    KEY_WIN,\
    KEY_SHIFT,\
    KEY_QUESTION,\
    KEY_PERIOD,\
    KEY_COMMA,\
    KEY_EXCLAMATION,\
    KEY_APOSTROPHE,\
    KEY_AT,\
    KEY_BACKSPACE,\
    KEY_QWERTY_ENTER,\
    KEY_FN,\
    KEY_SYMBOL,\
    KEY_NUM_LOCK,\
    KEY_QWERTY_MENU,\
    KEY_OK
#endif


/*

在VM的移植包中，我们发现有很多地方会修改MTK本身的代码。同样，在我们的其他平台产品中会有很多地方要修改VM移植层的代码。
而这些修改的地方都比较随意，他们的目的只是为了完成功能。这样导致了一个致命的结果,高度的代码耦合。
在无论是MTK或者虚拟机的代码有变更的情况下,都会迫使其他模块要做大规模的调整(比如之前MTK从09A到09B的迁移)。


在介绍我们的设计方案前，先看看目前有哪些修改方式:

对MTK平台代码的修改:
	1, 进入或者退出某些应用的时候(或来调SMS等)需要通知虚拟机做相应的处理，比如提示是否退出等等
	2，某些功能直接修改到平台的代码，要改变原来代码的行为方式。通常改动的代码比较多。
	3，一些资源性的代码。

其他平台产品对我们虚拟机的修改:
	1, 在某些状态变化的时候，调用其他产品的相应函数。比如VM状态变化，如退出虚拟机释放资源等。
	2，有些是需要调整我们现有代码的功能实现。如，3DUI的功能，需要修改我们本地窗口的实现。


这里我们发现，MTK会有些事件通知到VM，VM有些事件会通知到其他平台产品。这也是我们所能处理的。
我们可以把这样的对内和对外的事件通知机制统一和规范起来。这样就可以减少与外部不必要的耦合。

针对这两个方向上的事件，这里提供了2套机制:
一，外部通知到VM的事件

	所有可以以事件方式通知到VM移植层的事件，都通过下面的API调用来走。
	传入的参数分别为:事件及事件的数据
	int32 mr_app_system_event_hdlr(int32 msg, uint32 param1, uint32 param2);

	详细定义请参考函数说明。

二，VM通知到外部的事件

	外部应用需要接受事件必须先实现一个消息响应函数，再调用注册函数来注册。
	int32 my_app_event_hdlr(int32 msg, uint32 param1, uint32 param2)
	{
		switch(msg)
		{
		case event1:
			....
			break;

			...
		}
	}

	注册回调函数
	int32 mr_app_register_notify_event_hdlr(mr_app_event_handler_f handler);

	清除注册函数
	int32 mr_app_clear_notify_event_hdlr(mr_app_event_handler_f handler);
*/


/**
 * \brief MTK平台各个应用将会往虚拟机发送的相关事件
 */
typedef enum
{
	MR_SYSMSG_SYSTEM_INIT, /*系统初始化事件*/
	MR_SYSMSG_ENTRY_IDLESCREEN, /*进入IDLE SCREEN*/
	MR_SYSMSG_EXIT_IDLESCREEN, /*退出IDLE SCREEN*/
	MR_SYSMSG_SUSPEND_TIMER, /*GPIO 暂停定时器*/
	MR_SYSMSG_ENTER_USB, /*进入U盘*/
	MR_SYSMSG_SMS_INDICATION, /*短信显示*/
	MR_SYSMSG_LAUNCH_CAMERA, /*CAMERA启动*/
	MR_SYSMSG_LAUNCH_VDOPLY, /*视频播放器启动*/
	MR_SYSMSG_LAUNCH_3RD, /*其他互斥应用启动*/	
	MR_SYSMSG_MMI_ROTATION, /*界面旋转*/
	MR_SYSMSG_FORCE_EXIT, /*模式切换强制退出*/
}MR_SYSMSG_E;


/**
 * \brief 虚拟机及移植层要通知到外部的事件
 */
typedef enum
{
	MR_APPMSG_STATE_CHANGED,	/* 虚拟机状态变化 */
	MR_APPMSG_ENTRY_IDLESCREEN, /* 进入待机界面 */
	MR_APPMSG_EXIT_IDLESCREEN, /* 退出待机界面 */
	MR_APPMSG_COUNT,
}MR_APPMSG_E;


/**
 * \breif the application event handler prototype
 *
 * \param msg		[in] the application message
 * \param param		[in] the event parameter
 * \return MR_SUCCESS on SUCCESS, MR_FAILED otherwise.
 */
typedef int32 (*mr_app_event_handler_f)(int32 msg, uint32 param1, uint32 param2);


/**
 * \brief mrp application state.
 */
typedef enum 
{
	DSM_STOP,	/* 虚拟机空闲 */
	DSM_RUN,	/* 运行状态 */
	DSM_PAUSE,	/* 暂停状态 */
	DSM_BACK_RUN  /* 后台运行 */
}T_DSM_STATE;


/**
 * \brief 启动的方式
 */
typedef enum
{
	MR_RUN_NORMAL = 0,
	MR_RUN_BACK = 0x01,
}MR_RUN_STYLE_E;


/**
 * \brief 退出的方式
 */
typedef enum 
{
	MR_EXIT_NORMAL = 0,
	MR_EXIT_FORCE = 0x01,
}MR_EXIT_REASON_E;


/**
 * \brief mrp application entry string.
 */
extern char * dsmEntry;


/**
 * \brief app module initialize.
 *	启动应用时用于初始化相关模块，为mrp运行做准备。
 * 
 * \return MR_SUCCESS on SUCCESS, MR_FAILED otherwise.
 */
int32 mr_app_initialize(void);


/**
 * \brief app module destroy
 *	在退出虚拟机的时候，用于释放资源，相当于原来的returnToIdle函数。
 *
 * \return MR_SUCCESS on SUCCESS, MR_FAILED otherwise.
 */
int32 mr_app_terminate(void);


/**
 * \brief register handler to listen application events.
 *	
 * 在平台上，通常除了虚拟机本身外，还有其他的应用，比如SKYQQ。而这些应用需要在虚拟状态发生变化的时候做相关的处理，
 * 比如退出，或者进入到idle screen等等，之前的方式是直接将相关代码加到虚拟机相关的函数，这样导致高度的耦合。
 * 为了处理这样的高耦合，我们定义了很多虚拟的事件，外部应用可以在应用初始化的是通过调用相关的函数来监听。
 *
 * \param handler	[in] the event handler
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 *
 * \sa MR_APPEVENT_E
 */
int32 mr_app_register_notify_event_hdlr(mr_app_event_handler_f handler);


/**
 * \brief clear the specific event handler
 *
 * \param handler	[in] the event handler
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_app_clear_notify_event_hdlr(mr_app_event_handler_f handler);


/**
 * \brief 将事件发给全部的监听者，各个监听者之间没有先后顺序。
 *
 * \param e		[in] the application event
 * \param p		[in] the event data
 * \return the event handler result
 */
int32 mr_app_send_notify_event(int32 msg, uint32 param1, uint32 param2);


/**
 * \brief 在MTK本地应用进入,退出或任何需要通知VM的时候能将对应的事件通知到虚拟机
 *
 * \param app	[in] 应用ID
 * \param state	[in] 应用状态
 * \param param	[in] 调用参数，每个事件可以定义自己不同的消息参数
 * \return 
 *		- MR_SUCCESS 表示虚拟机已经处理了这个事件，并且外部流程不应该继续下去
 *		- MR_IGNORE/MR_FAILED 表示外部流程正常进行
 */
int32 mr_app_system_event_hdlr(int32 msg, uint32 param1, uint32 param2);


/**
 * \brief set application state
 *
 * \param state	[in] the state to switch
 * \return nothing
 */
void mr_app_set_state(T_DSM_STATE state);


/**
 * \brief get the application state
 *
 * \return current state
 */
T_DSM_STATE mr_app_get_state(void);


/**
 * \brief set run attributes
 * 
 * \param attr	[in]
 */
void mr_app_set_run_style(uint32 attr);


/**
 * \brief set the vm exit reason
 */
void mr_app_set_exit_reason(uint32 reason);


/**
 * \brief get the exit reason
 */
uint32 mr_app_get_exit_reason(void);


/**
 * \brief run the application
 *
 * \param entry		[in] the entry parameter
 * \param param		[in] the second parameter
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_app_run(char* entry, uint32 param);


/**
 * \brief pause the application
 *
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_app_pause(void);


/**
 * \brief resume the application from pause
 *
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_app_resume(void);


/**
 * \brief 强制退出虚拟机，不管应用是处于后台还是前台。
 *
 * \return nothing
 */
void mr_app_exit(void);


/**
 * \brief 当系统初始化的时候调用，用来初始化相关系统变量
 */
void mr_system_environment_setup(void);


/**
 * \brief 对传入VM的按键进行保护，以保证传入VM的案件都是成对的
 *
 * \param ktype 支持以下事件类型
 *		MR_KEY_PRESS,
 *		MR_KEY_RELEASE,
 *		MR_MOUSE_DOWN,
 *		MR_MOUSE_UP,
 *		MR_MENU_SELECT,
 *		MR_MENU_RETURN,
 *		MR_DIALOG_EVENT
 * \param param1 指定确切按下的是哪个按键
 * \param param2 保留参数
 *
 * \return 
 *		MR_SUCCESS  成功
 * 		MR_FAILED   失败
 * 		MR_INGOR    VM忽略此事件
 */
 int32 mr_app_send_event_wrapper(int16 ktype , int32 param1, int32 param2);


/**
 * \brief 键值转义
 */
int32 mr_app_translate_keycode(U16 keycode);


/**
 * \brief 显示主窗口界面
 *
 * \return nothing
 */
void mr_app_entry_dsm_screen(void);


/**
 * \brief 显示base screen
 */
void mr_app_show_dsm_base_screen(void);


/**
 * \brief 在主窗口被隐藏或覆盖时调用
 *
 * \return nothing
 */
void mr_app_exit_dsm_screen(void);


/**
 * \brief 强制退出主窗口界面，如果应用处于后台则不影响
 *
 * \return nothing
 * 
 * \sa mr_app_exit
 */
void mr_app_force_exit_dsm_screen(void);


/**
 * \brief 设置按键及触摸屏事件
 */
void mr_app_setup_dsm_screen_event_hdlr(void);


/**
 * \brief dsm screen的按键处理函数
 *
 * \return nothing
 */
void mr_app_dsm_screen_key_hdlr(void);


/**
 * \brief 触摸屏的move事件处理函数
 *
 * \return nothing
 */
void mr_app_dsm_screen_mouse_move_hdlr(mmi_pen_point_struct pos);


/**
 * \brief 触摸屏mouse up的事件处理函数
 *
 * \return nothing
 */
void mr_app_dsm_screen_mouse_up_hdlr(mmi_pen_point_struct pos);


/**
 * \brief 触摸屏mouse down的时间处理函数
 *
 * \return nothing
 */
void mr_app_dsm_screen_mouse_down_hdlr(mmi_pen_point_struct pos);


/**
 * \brief 直接返回到主窗口界面
 *
 * \return nothing
 */
int32 mr_app_goto_dsm_screen(int32 param);

MMI_ID mr_app_get_group_id(void);

MMI_ID mr_app_get_group(void);

void mr_app_EntryNewScreen(U16 newscrnID, FuncPtr newExitHandler, FuncPtr newEntryHandler, mmi_frm_scrn_type_enum flag);

/////////////////////////////////////////////////////////////////////////////

/**
 * \brief 获取当前应用的状态
 * 
 * \note 老的接口，不建议使用
 */
int GetDsmState(void); 


/**
 * \brief 强制退出
 * 
 * \note 老的接口，不建议使用
 */
void dsmForceExitEx(void);


/* 52_xsgrz_patch begin */
/**
 * \brief 配置move事件产生的频率
 * 
 * \param input [in] move产生的频率，单位为10毫秒的整数倍,若为0恢复成缺省值(MTK缺省值为80毫秒)
 * \retval
 * #MR_SUCCESS : 成功
 * \retval
 * #MR_FAILED : 失败
 * \retval
 * #MR_IGNORE : 不支持该功能
 * \remarks
 * 在应用刚起来的时候设置你需要的值，在应用退出的时候恢复成缺省值
 * \par 举例说明
 * \code
 * mrc_init()
 * {
 * uint8 sampling_period  = 2;//为20毫秒
 * mrc_platEx(0x90030, &sampling_period, 1, NULL, NULL, NULL);
 * }
 * mrc_exit
 * {
 * uint8 sampling_period  = 0;//恢复成缺省值
 * mrc_platEx(0x90030, &sampling_period, 1, NULL, NULL, NULL);
 * }
 */
int32 mr_app_config_sampling_period(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 配置move事件产生的最小距离
 * 
 * \param input [in] move产生的最小距离，单位为像素(MTK缺省为5个像素)
 * \retval
 * #MR_SUCCESS : 成功
 * \retval
 * #MR_FAILED : 失败
 * \retval
 * #MR_IGNORE : 不支持该功能
 * \remarks
 * 在应用刚起来的时候设置你需要的值，在应用退出的时候恢复成缺省值
 * \par 举例说明
 * \code
 * mrc_init()
 * {
 * uint8 move_offset  = 2;//为2个像素
 * mrc_platEx(0x90031, &move_offset, 1, NULL, NULL, NULL);
 * }
 * mrc_exit
 * {
 * uint8 move_offset  = 0;//恢复成缺省值
 * mrc_platEx(0x90031, &move_offset, 1, NULL, NULL, NULL);
 * }
 */
int32 mr_app_config_move_offset(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
/* 52_xsgrz_patch end */

#endif

