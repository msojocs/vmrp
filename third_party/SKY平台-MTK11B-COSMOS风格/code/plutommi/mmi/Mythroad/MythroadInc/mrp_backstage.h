#ifndef __MRP_PAL_BACKSTAGE_H__
#define __MRP_PAL_BACKSTAGE_H__


#define MR_BACKSTAGE_IDLE_WIDGET_CONTENT_MAXLINE		4
#define MR_PTINRECT(x, y, rx, ry, rw, rh) (x >= rx && x <= rx+rw && y >= ry && y < ry + rh)


/**
 * \brief ICON 操作
 */
typedef enum{
	DSM_HIDE_ICON = 0,
	DSM_SHOW_ICON,
	DSM_BLINK_ICON,
	DSM_GRAY_ICON
}MR_BACKSTAGE_STATE_E;


/**
 * \brief 一个应用支持的各个情况下的status id集合
 */
typedef struct {
	S16 online;
	S16 away;
	S16 hide;
	S16 offline;
}MR_STATUS_ICONSET_T;


/**
 * \brief 一个应用支持的各个情况下的status id集合
 */
typedef struct 
{
	S32 online;
	S32 away;
	S32 hide;
	S32 offline;
}MR_IMAGE_SET_T;


/** 
 * \brief 显示提示信息的界面
 */
typedef struct{
	uint32 style;  /*这里可以是浮动的，也可以是模态的*/
	uint8* icon_path; /*可以是文件名，也可以是BUFFER指针*/
	int16 icon_width, icon_height; /*图片宽高*/
	uint16* title; /*对话框标题*/
	uint16* content; /*对话框内容*/
	uint16* lsk; //left sofkt key
	uint16* rsk; //right soft key
	int32 msgcount; //new msg count
}mr_backstage_tipinfo_t;


typedef struct{
	uint16 visible; /*是否显示*/
	uint16 arrow_x; /*箭头的位置*/
	uint16 w, h, x, y; /*显示位置*/
	gdi_handle layer_handle; /*layer句柄*/
	uint8* layer_buf; /*内存buffer*/
	mr_backstage_tipinfo_t tipinfo;

	uint16 line_y;
	uint16 title_w, title_h;
	uint16 text_h, text_w;
	uint16 lsk_w, rsk_w;/*文本及左右软件相关的信息*/	
	uint16 newmsg_x, newmsg_y;

	uint16 linecount;/*多少行文本*/
	uint8 linesinfo[MR_BACKSTAGE_IDLE_WIDGET_CONTENT_MAXLINE];	/*每行的情况*/	
}mr_backstage_widget_ctrl_t;


/**
 * \brief 后台模块初始化
 *
 * \return MR_SUCCESS表示成功，MR_FAILED表示失败
 */
int32 mr_backstage_initialize(void);


/**
 * \brief 后台模块释放
 *
 * \return MR_SUCCESS表示成功，MR_FAILED表示失败
 */
int32 mr_backstage_terminate(void);


/**
 * \brief 绘制桌面背景
 *
 * \param gdih	
 * \param mode
 * \return 成功返回1， 失败返回0
 */
 int mr_backstage_draw_idle_background(gdi_handle gdih, U8 mode);


/**
 * \brief 使应用程序进入后台运行
 *
 * \param input			[in]后台运行相关信息结构,mr_backstage_st 数据结构指针
 * \param input_len		[in]mr_backstage_st数据结构大小
 * \param output		[out]未定义
 * \param output_len	[out]未定义
 * \param cb			[out]未定义
 * \return 成功返回MR_SUCCESS,失败返回MR_FAILED.
 */
int32 mr_backstage_run(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 在status bar 上显示后台图片
 *
 * \param input			[in] 图片信息结构,mr_pic_req 数据结构指针
 * \param input_len		[in] mr_pic_req 数据结构大小
 * \param output		[out]未定义
 * \param output_len	[out]未定义
 * \param cb			[out]未定义
 * \return 成功返回MR_SUCCESS,失败返回MR_FAILED.
 */
int32 mr_backstage_show_idle_pic(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 隐藏status bar上的图片显示
 *
 * \param param		[in]未定义
 * \return 成功返回MR_SUCCESS,失败返回MR_FAILED.
 */
int32 mr_backstage_hide_idle_pic(int32 param);


/**
 * \brief 查询后台支持情况,在一个支持后台的程序运行时，需要调用这个函数来查询平台是否支持后台相关接口。
 *
 * \param appid		[in]当前应用的APPID
 * \return 如果平台对这个应用支持后台返回MR_SUCCESS, 如果不支持返回MR_FAILED,如果是不支持本接口的返回MR_IGNORE.
 */
int32 mr_backstage_support(int32 appid);


/**
 * \brief 将后台应用切换到前台运行
 *
 * \param 	[in] 需要激活的应用的appid
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED.
 *
 * \note 这个函数不做对当前的状态判断，也就是说他的前提一定是有后台程序在运行
 */
int32 mr_backstage_activate_app(int32 appid);


/**
 * \brief 如果当前有后台程序在运行，唤醒到当前运行
 *
 * \return void
 */
void mr_backstage_activate_cur_app(void);


/**
 * \brief 获取当前后台程序的APPID
 *
 * \return 后台程序的APPID
 */
int32 mr_backstage_get_appid(void);


/**
 * \brief 拦截endkey相应，将endkey发送到vm
 *
 * \param param	[in] 1 表示要拦截，0表示恢复
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED.
 * \note 这个函数必须成对调用，在应用起来时默认不拦截
 */
int32 mr_backstage_hold_endkey(int32 param);


/**
 * \brief hold the endkey
 */
void mr_backstage_setup_endkey_hdlr(void);


/**
 *\brief 是否IDLE 界面
 */
int32 mr_backstage_is_idle_screen(void);


/**
 * \brief 是否锁屏
 */
int32 mr_backstage_is_keypad_lock(void);


/**
 * \brief 是否交互模式
 *
 * \return 
 *	MR_TRUE	- 交互模式
 *	MR_FALSE - 非交互模式
 */
MR_BOOL mr_backstage_is_interactive(void);


/**
 * \brief 是否支持气泡框
 */
int32 mr_backstage_online_mgr_support(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_backstage_idle_widget_support(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_backstage_refresh_status_icon_bar(int32 param);

/**
 * \brief 气泡框上的位置检测
 */
int32 mr_backstage_idle_widget_in_widget(int x, int y);
int32 mr_backstage_idle_widget_in_lsk(int x, int y);
int32 mr_backstage_idle_widget_in_rsk(int x, int y);

/**
 * \brief 气泡框的排版及绘制
 */
int32 mr_backstage_setup_widget_layer(void);
int32 mr_backstage_release_widget_layer(void);
void mr_backstage_draw_idle_widget(void);
void mr_backstage_idle_widget_layout(void);
void mr_backstage_on_rotation(void);
void mr_backstage_set_online_mgr_running(void);

/**
 * \brief 获取启动后台应用的快捷键
 */
int32 mr_backstage_get_idle_shortcut_key(void);

#endif

