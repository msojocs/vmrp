#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "IdleAppResDef.h"
#include "IdleAppDef.h"
#include "idlegprot.h"
#include "mrp_include.h"


#if (MTK_VERSION < 0x09b0952)
#include "wgui_status_icons.h"
#else
#include "gui_status_icon.h"
#endif
#include "gui_typedef.h"

#include "vapp_mythroad_gprot.h"

typedef struct {
	S16 actived_status_icon;
	S16 bEndKeyActived;
	FuncPtr hEndKeyUp;
	FuncPtr hEndKeyDown;
	mr_backstage_st appinfo;
	mr_pic_req picinfo;
}mr_backstage_ctrl_t;

static mr_backstage_ctrl_t s_backstage_ctrl = {0};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void mr_backstage_get_img_set(int32 appid, MR_IMAGE_SET_T *pImgSet)
{
	switch (appid)
	{
		case MR_APPID_QQ2007:
		case MR_APPID_QQ2008:
		case MR_APPID_MTKQQ2008:
			pImgSet->online = IMG_DSM_STATUS_ICON_QQ_ONLINE;
			pImgSet->away = IMG_DSM_STATUS_ICON_QQ_AWAY;
			pImgSet->hide = IMG_DSM_STATUS_ICON_QQ_HIDE;
			pImgSet->offline = IMG_DSM_STATUS_ICON_QQ_OFFLINE;
			break;
			
		default:
			pImgSet->online = IMG_DSM_STATUS_ICON_ONLINE;
			pImgSet->away = IMG_DSM_STATUS_ICON_AWAY;
			pImgSet->hide = IMG_DSM_STATUS_ICON_HIDE;
			pImgSet->offline = IMG_DSM_STATUS_ICON_OFFLINE;
			break;
	}
}


static S32 mr_backstage_get_img_id(int state, int appid)
{
	MR_IMAGE_SET_T imgset;
	
	mr_trace("mr_backstage_get_img_id: %d, %d", state, appid);

	mr_backstage_get_img_set(appid, &imgset);

	if (state == DSM_GRAY_ICON)
	{
		return imgset.offline;
	}
	
	switch(s_backstage_ctrl.picinfo.time & 0x3)
	{
		case 0: return imgset.online;
		case 1: return imgset.away;
		case 2: return imgset.hide;
		case 3: 
		default:
			return imgset.offline;
	}
}


static S32 mr_backstage_get_str_id(int state, int appid)
{
	switch (appid)
	{
		case MR_APPID_QQ2007:
		case MR_APPID_QQ2008:
		case MR_APPID_MTKQQ2008:
			return STR_DSM_QQ;
			
		default:
			return STR_DSM_DSM_MAIN;
	}
}


static void mr_backstage_get_status_iconset(int32 appid, MR_STATUS_ICONSET_T *pIconSet)
{
	switch (appid)
	{
		case MR_APPID_QQ2007:
		case MR_APPID_QQ2008:
		case MR_APPID_MTKQQ2008:
			pIconSet->online = STATUS_ICON_DSM_QQ_ONLINE;
			pIconSet->away = STATUS_ICON_DSM_QQ_AWAY;
			pIconSet->hide = STATUS_ICON_DSM_QQ_HIDE;
			pIconSet->offline = STATUS_ICON_DSM_QQ_OFFLINE;
			break;
			
		default:
			pIconSet->online = STATUS_ICON_DSM_ONLINE;
			pIconSet->away = STATUS_ICON_DSM_AWAY;
			pIconSet->hide = STATUS_ICON_DSM_HIDE;
			pIconSet->offline = STATUS_ICON_DSM_OFFLINE;
			break;
	}
}


/**
 * \brief 根据应用的appid和状态来获取显示的icon
 *
 * \param state	[in] 应用的状态
 * \param appid	[in] 应用的APPID
 * \return 
 *	- STATUS_ICON_INVALID_ID 只是清楚掉全部的icon 显示
 *	- -2 表示不做任何动作
 *	- 其他值为对应的ICON ID
 */
static S16 mr_backstage_get_status_icon_id(int state, int appid)
{
	MR_STATUS_ICONSET_T iconset;
	
	mr_trace("mr_backstage_get_status_icon_id: %d, %d", state, appid);

	if (s_backstage_ctrl.picinfo.time == 5)
	{
		//skyqq special function for mstar, just ignore
		return -2;
	}

	mr_backstage_get_status_iconset(appid, &iconset);
	if (state == DSM_HIDE_ICON)
	{
		return -1; //STATUS_ICON_INVALID_ID
	}
	else if (state == DSM_GRAY_ICON)
	{
		return iconset.offline;
	}
	else
	{
		switch(s_backstage_ctrl.picinfo.time & 0x3)
		{
			case 0: return iconset.online;
			case 1: return iconset.away;
			case 2: return iconset.hide;
			case 3: 
			default:
				return iconset.offline;
		}
	}
}


/**
 * \brief 根据APPID和应用状态来显示相关桌面图标
 *
 * \param state	[in]应用状态
 * \param appid [in]当前应用appid
 * \return void
 */
static void mr_backstage_show_status_icon(int state, int appid)
{
	S16 status_icon = s_backstage_ctrl.actived_status_icon;
	
	s_backstage_ctrl.actived_status_icon = mr_backstage_get_status_icon_id(state, appid);
	
	mr_trace("status_icon_id: %d, %d, %d", state, status_icon, s_backstage_ctrl.actived_status_icon);
	
	//just ignore this status icon id
	if (status_icon == -2)
	{
		return;
	}

	if(status_icon > 0){
		mr_pal_status_icon_hide_status_icon(status_icon);
	}
	
	if(state == DSM_HIDE_ICON){
		mr_pal_status_icon_update_status_icons();
		return;
	}else if(state == DSM_SHOW_ICON || state == DSM_GRAY_ICON){
		mr_pal_status_icon_show_status_icon(s_backstage_ctrl.actived_status_icon);
	}else if(state == DSM_BLINK_ICON){
		mr_pal_status_icon_blink_status_icon(s_backstage_ctrl.actived_status_icon);
	}

	//mr_pal_register_status_icon_pen_event_hdlr(s_backstage_ctrl.actived_status_icon, mr_backstage_activate_cur_app); //set in enter idle screen
	mr_pal_status_icon_update_status_icons();
	if(mr_backstage_is_idle_screen() == MR_SUCCESS){
		/* 为了设置键盘事件 */
		mr_backstage_on_enter_idlescreen(); 
	}
}


int32 mr_backstage_initialize(void)
{
	mr_trace("mr_backstage_initialize: %d", __LINE__);
	memset(&s_backstage_ctrl, 0, sizeof(s_backstage_ctrl));
	return MR_SUCCESS;
}


int32 mr_backstage_terminate(void)
{
	mr_trace("mr_backstage_terminate: %d", __LINE__);
	mr_backstage_hide_idle_pic(0);
	/* remove the ncenter cell */
	vapp_mythraod_goto_background(MMI_FALSE, 0, 0, 0);
	s_backstage_ctrl.appinfo.appid = 0;
	memset(&s_backstage_ctrl.picinfo, 0, sizeof(s_backstage_ctrl.picinfo));
	mr_backstage_hold_endkey(0);
#ifndef __MR_CFG_FEATURE_SLIM__	
	mr_backstage_on_terminate_vm(); //这里还得加上判断QQ是否还在运行	
#endif
	return MR_SUCCESS;
}


int32 mr_backstage_set_appinfo(mr_backstage_st* appinfo)
{
	if(!appinfo) return MR_FAILED;
	s_backstage_ctrl.appinfo = *appinfo;
	mr_trace("mr_backstage_set_appinfo: %d", s_backstage_ctrl.appinfo.appid);
	return MR_SUCCESS;
}


int16 mr_backstage_get_actived_status_icon(void)
{
	return s_backstage_ctrl.actived_status_icon;
}


int mr_backstage_draw_idle_background(gdi_handle gdih, U8 mode)
{
	U8 *pData;
	U16 ox =0,oy=0;
	gdi_layer_struct *glayer;

	if(gdih == GDI_ERROR_HANDLE)
		glayer = gdi_act_layer;
	else
		glayer =(gdi_layer_struct *)gdih;
	
	if((s_backstage_ctrl.picinfo.appid > 0) && (s_backstage_ctrl.picinfo.destId == DSM_PIC_DESTID_IDLE_BG))
	{
		pData = s_backstage_ctrl.picinfo.buff;
		if(pData == NULL)
			return 0;

		gdi_layer_push_clip();
		gdi_layer_set_clip(0,0, LCD_WIDTH-1,LCD_HEIGHT-1);
		if(s_backstage_ctrl.picinfo.height < LCD_HEIGHT||s_backstage_ctrl.picinfo.width < LCD_WIDTH)
		{
			ox = (LCD_WIDTH-s_backstage_ctrl.picinfo.width)/2;
			oy = (LCD_HEIGHT - s_backstage_ctrl.picinfo.height)/2;
		
			if(mode == 1)
			{
				// DSM_MTK_PREFIX(draw_filled_area)(0,0,UI_device_width-1,UI_device_height-1,current_MMI_theme->general_background_filler);	// 11A编译问题
			}
		}
	
		mr_draw_bitmap((U16 *)(glayer->buf_ptr), (U16 *)s_backstage_ctrl.picinfo.buff, 
			ox, oy,  s_backstage_ctrl.picinfo.width, s_backstage_ctrl.picinfo.height,  gdi_act_layer->width, s_backstage_ctrl.picinfo.color);
		
		if(mode == 1)
		{
			gdi_layer_blt_previous(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
		}
		
		gdi_layer_pop_clip();
		return 1;
	}

	return 0;
}


int32 mr_backstage_run(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_backstage_st *pBackStag = (mr_backstage_st *)input;

	if(pBackStag == NULL 
		|| input_len < sizeof(mr_backstage_st) 
		|| pBackStag->appid <= 0
		|| mr_app_get_state() != DSM_RUN)
	{
		return MR_FAILED;
	}
	
	mr_backstage_set_appinfo(pBackStag);
	mr_app_set_state(DSM_BACK_RUN);

	/* add the ncenter cell */
	vapp_mythraod_goto_background(
		MMI_TRUE,
		pBackStag->appid,
		mr_backstage_get_img_id(DSM_SHOW_ICON, pBackStag->appid),
		mr_backstage_get_str_id(DSM_SHOW_ICON, pBackStag->appid));

	if(pBackStag->appid != 2914)
	{
	//如果SDK QQ2008，通过菜单里面的挂Q操作无法返回到idle,那么修改为#if 0
	#if 0 //只回到菜单
	if(IsScreenPresent(DSM_SCREEN_BASE))
	{
		DeleteUptoScrID(DSM_SCREEN_BASE);
		DeleteNHistory(1);	
	}
	#else
		//DeleteUptoScrID(IDLE_SCREEN_ID);
	#endif

	if(s_backstage_ctrl.actived_status_icon <= 0){
		/* 如果图标已经在了就不需要了，应用要改变需要单独设置*/
		mr_backstage_show_status_icon(DSM_SHOW_ICON, s_backstage_ctrl.appinfo.appid);	
	}
	
	GoBackHistory();
	}

	mr_localui_free_all_screen();	 
  	dsmFreeScrIdAll();
	mdi_audio_resume_background_play();
	return MR_SUCCESS;
}


int32 mr_backstage_show_idle_pic(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_pic_req* pPicReq = (mr_pic_req *)input;

	if(pPicReq == NULL 
		||input_len < sizeof(mr_pic_req) 
		|| pPicReq->appid <= 0)
	{
		return MR_FAILED;
	}
		
	if(pPicReq->destId == DSM_PIC_DESTID_IDLE_BG 
		&& pPicReq->img_type != IMG_BMP)
	{
		return MR_FAILED;
	}

	s_backstage_ctrl.picinfo = *pPicReq;	
	if(s_backstage_ctrl.picinfo.destId == DSM_PIC_DESTID_IDLE_ICON)
	{
		int state;
		if(s_backstage_ctrl.picinfo.time  < 0)
		{
			state = DSM_GRAY_ICON;
			mr_backstage_show_status_icon(DSM_GRAY_ICON, s_backstage_ctrl.picinfo.appid);
		}
		else if(s_backstage_ctrl.picinfo.time > 10) /* 1000 1001 1002 */
		{
			state = DSM_BLINK_ICON;
			mr_backstage_show_status_icon(DSM_BLINK_ICON, s_backstage_ctrl.picinfo.appid);
		}
		else /* 0 1 2 */
		{
			state = DSM_SHOW_ICON;
			mr_backstage_show_status_icon(DSM_SHOW_ICON, s_backstage_ctrl.picinfo.appid);
		}
		/* update the ncenter cell */
		vapp_mythraod_goto_background(
			MMI_TRUE,
			s_backstage_ctrl.picinfo.appid,
			mr_backstage_get_img_id(state, s_backstage_ctrl.picinfo.appid),
			mr_backstage_get_str_id(state, s_backstage_ctrl.picinfo.appid));
	}else{
		if(mr_backstage_is_idle_screen() == MR_SUCCESS)
		{
			gdi_layer_push_and_set_active(dm_get_wallpaper_layer());
			mr_backstage_draw_idle_background(dm_get_wallpaper_layer(),1);
			gdi_layer_pop_and_restore_active();
		}
	}
	return MR_SUCCESS;
}


int32 mr_backstage_get_picinfo(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(!output || !output_len){
		return MR_FAILED;
	}

	*output = (uint8*)&s_backstage_ctrl.picinfo;
	*output_len = sizeof(mr_pic_req);
	return MR_SUCCESS;
}


int32 mr_backstage_hide_idle_pic(int32 param)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	mr_backstage_hide_idle_widget(NULL, 0, 0, 0, 0);
#endif	
	mr_backstage_show_status_icon(DSM_HIDE_ICON, 0);
	mr_backstage_idle_widget_restore_event_hdlr();
	
	if(s_backstage_ctrl.picinfo.appid > 0) /*防止没有调用画，就停止*/
	{
		memset(&s_backstage_ctrl.picinfo, 0, sizeof(s_backstage_ctrl.picinfo));
	}
	return MR_SUCCESS;
}


int32 mr_backstage_support(int32 appid)
{
 #ifdef __MR_CFG_FEATURE_OVERSEA__
            return MR_FAILED;
        #else
            return MR_SUCCESS;
   #endif
}


int32 mr_backstage_activate_app(int32 appid)
{
	mr_trace("mr_backstage_activate_app: %d", appid);
	
	if((mr_app_get_state()!= DSM_BACK_RUN) 
		|| (appid != s_backstage_ctrl.appinfo.appid))
	{
		return MR_FAILED;
	}		
#if 0	// 这个保护可以去掉吧
	if(IsScreenPresent(DSM_SCREEN_BASE) 
		|| (GetActiveScreenId() ==DSM_SCREEN_BASE))
	{
		return MR_FAILED;
	}
#endif
	if(mr_pal_is_in_call()){
		return MR_FAILED;
	}

	mr_app_show_dsm_base_screen();
	//DeleteScreenIfPresent(IDLE_SCREEN_DIGIT_HANDLER_ID);
		
	mr_app_initialize();
	mr_app_set_state(DSM_RUN);

	/* remove the ncenter cell */
	vapp_mythraod_goto_background(MMI_FALSE, 0, 0, 0);

	mr_trace("MR_LOCALUI_EVENT: MR_LOCALUI_ACTIVE");
	mr_event(MR_LOCALUI_EVENT, MR_LOCALUI_ACTIVE, 0);
	return MR_SUCCESS;
}


void mr_backstage_activate_cur_app(void)
{
	mr_trace("mr_backstage_activate_cur_app: %d", s_backstage_ctrl.appinfo.appid);
	mr_backstage_activate_app(s_backstage_ctrl.appinfo.appid);
}


int32 mr_backstage_get_appid(void)
{
	return s_backstage_ctrl.appinfo.appid;
}


void mr_backstage_setup_endkey_hdlr(void)
{
	if(!s_backstage_ctrl.bEndKeyActived) return;

	if(GetKeyHandler(KEY_HOME, KEY_EVENT_UP) != mr_app_dsm_screen_key_hdlr){
		s_backstage_ctrl.hEndKeyUp = GetKeyHandler(KEY_HOME, KEY_EVENT_UP);
		s_backstage_ctrl.hEndKeyDown= GetKeyHandler(KEY_HOME, KEY_EVENT_DOWN);
		mr_trace("mr_backstage_setup_endkey_hdlr: %x, %x", s_backstage_ctrl.hEndKeyUp, s_backstage_ctrl.hEndKeyDown);
	}

	SetKeyHandler(mr_app_dsm_screen_key_hdlr, KEY_HOME, KEY_EVENT_UP);
	SetKeyHandler(mr_app_dsm_screen_key_hdlr, KEY_HOME, KEY_EVENT_DOWN);	
}


static void mr_backstage_release_endkey_hdlr(void)
{
	if(!s_backstage_ctrl.bEndKeyActived) return;

	mr_trace("mr_backstage_release_endkey_hdlr: %x, %x", s_backstage_ctrl.hEndKeyUp, s_backstage_ctrl.hEndKeyDown);
	if(GetKeyHandler(KEY_HOME, KEY_EVENT_UP) == mr_app_dsm_screen_key_hdlr 
		)//&& (s_backstage_ctrl.hEndKeyUp || s_backstage_ctrl.hEndKeyDown))
	{
		//SetKeyHandler(s_backstage_ctrl.hEndKeyUp, KEY_HOME, KEY_EVENT_UP);
		//SetKeyHandler(s_backstage_ctrl.hEndKeyDown, KEY_HOME, KEY_EVENT_DOWN);
		SetKeyHandler(mmi_idle_display, KEY_HOME, KEY_EVENT_UP);
		SetKeyHandler(mmi_idle_display, KEY_HOME, KEY_EVENT_DOWN);		
	}
	
	s_backstage_ctrl.hEndKeyUp = NULL;
	s_backstage_ctrl.hEndKeyDown = NULL;
}


int32 mr_backstage_hold_endkey(int32 param)
{
	mr_trace("mr_backstage_hold_endkey: %d, %x, %x", param, s_backstage_ctrl.hEndKeyUp, s_backstage_ctrl.hEndKeyDown);

	if(param){
		if(s_backstage_ctrl.bEndKeyActived) return MR_FAILED;
		s_backstage_ctrl.bEndKeyActived = 1;
		if(mr_app_get_state() == DSM_RUN) mr_backstage_setup_endkey_hdlr();
	}else{
		if(!s_backstage_ctrl.bEndKeyActived) return MR_FAILED;
		if(mr_app_get_state() == DSM_RUN) mr_backstage_release_endkey_hdlr();
		s_backstage_ctrl.bEndKeyActived = 0;
	}
	
	return MR_SUCCESS;
}


int32 mr_backstage_is_idle_screen(void)
{
	if(!mmi_idle_is_active())
		return MR_FAILED;
		
	//return mmi_idle_is_on_idle_screen()? MR_SUCCESS: MR_FAILED;
	//return (IDLE_SCREEN_ID == GetActiveScreenId())? MR_SUCCESS : MR_FAILED;
	return MR_SUCCESS;
}


int32 mr_backstage_is_keypad_lock(void)
{
	return mmi_scr_locker_is_locked() ? MR_SUCCESS : MR_FAILED;
}


int32 mr_backstage_refresh_status_icon_bar(int32 param)
{
	if(param)
	{
		wgui_set_wallpaper_on_bottom(TRUE);	
		gdi_layer_lock_frame_buffer();
		show_title_status_icon();
		gdi_layer_unlock_frame_buffer();
	}
	else
	{
		hide_status_icon_bar(0);
		hide_status_icon_bar(1);
	}
	
	return MR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MR_BACKSTAGE_WIDGET_A_WIDTH			24
#define MR_BACKSTAGE_WIDGET_A_HEIGHT		19
#define MR_BACKSTAGE_WIDGET_C_WIDTH			10
#define MR_BACKSTAGE_WIDGET_C_HEIGHT		13
#define MR_BACKSTAGE_WIDGET_E_WIDTH			21
#define MR_BACKSTAGE_WIDGET_E_HEIGHT		16
#define MR_BACKSTAGE_WIDGET_M_WIDTH			71
#define MR_BACKSTAGE_WIDGET_M_HEIGHT		78
#define MR_BACKSTAGE_WIDGET_M_Y				23
#define MR_BACKSTAGE_WIDGET_M1_WIDTH		4
#define MR_BACKSTAGE_WIDGET_J_WIDTH			7
#define MR_BACKSTAGE_WIDGET_J_HEIGHT		32
#define MR_BACKSTAGE_WIDGET_F_WIDTH			8

#define MR_BACKSTAGE_WIDGET_NEWMSG_BAR_Y	33
#define MR_BACKSTAGE_WIDGET_MARGIN_DOWN	13
#define MR_BACKSTAGE_WIDGET_ICON_X			15
#define MR_BACKSTAGE_WIDGET_ICON_Y			27
#define MR_BACKSTAGE_WIDGET_ICON_TCOLOR		0xf81f
#define MR_BACKSTAGE_WIDGET_LINE_HEIGHT		14
#define MR_BACKSTAGE_WIDGET_LINE_MARGIN		(MR_BACKSTAGE_WIDGET_LINE_HEIGHT/2-1)
#define MR_BACKSTAGE_WIDGET_BUTTON_SPACE	8

#define MR_BACKSTAGE_WIDGET_BG_COLOR		gdi_act_color_from_rgb(255, 0xea, 0xe6, 0xc4)
#define MR_BACKSTAGE_WIDGET_LINE1_COLOR		gdi_act_color_from_rgb(255, 0xca, 0xaf, 0x5b)
#define MR_BACKSTAGE_WIDGET_LINE2_COLOR		gdi_act_color_from_rgb(255, 0xff, 0xff, 0xff)
#define MR_BACKSTAGE_WIDGET_TEXT_COLOR		gui_color(0x00,0x00,0x00)
#define MR_BACKSTAGE_WIDGET_BTN_COLOR		gui_color(0xff,0xff,0xff)

#define MR_BACKSTAGE_WIDGET_MARGIN					7
#define MR_BACKSTAGE_WIDGET_ARROW_HEIGHT			21
#define MR_BACKSTAGE_WIDGET_ARROW_WIDTH			MR_BACKSTAGE_WIDGET_E_WIDTH
#define MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT	MR_BACKSTAGE_WIDGET_C_WIDTH
#define MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_TOP	28
#define MR_BACKSTAGE_WIDGET_CONTENT_LINESCAPE		4

typedef enum{
	MR_WIDGET_MEM_TYPE_MED = 1,
	MR_WIDGET_MEM_TYPE_SCR = 2,
}MR_WIDGET_MEM_TYPE_E;


mr_backstage_widget_ctrl_t s_idle_widget_ctrl;
static int g_widget_layer_mem_type;


int32 mr_backstage_idle_widget_support(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_SUCCESS;
}


int32 mr_backstage_setup_widget_layer(void)
{
	GDI_RESULT r;

	if(s_idle_widget_ctrl.w <= 0){
		mr_trace("backstage widget layout: %d", __LINE__);
		mr_backstage_idle_widget_layout();
	}	
		
	if(!s_idle_widget_ctrl.layer_buf){
		s_idle_widget_ctrl.layer_buf = mr_mem_get_ex(s_idle_widget_ctrl.w * s_idle_widget_ctrl.h * sizeof(uint16));
		g_widget_layer_mem_type = MR_WIDGET_MEM_TYPE_MED;
		if(!s_idle_widget_ctrl.layer_buf){
			s_idle_widget_ctrl.layer_buf = mr_pal_mem_scrmem_alloc(s_idle_widget_ctrl.w * s_idle_widget_ctrl.h * sizeof(uint16));
			g_widget_layer_mem_type = MR_WIDGET_MEM_TYPE_SCR;
		}
	}
	
	if(!s_idle_widget_ctrl.layer_buf){
		mr_trace("mr_backstage_setup_widget_layer failed!!");
		return MR_FAILED;
	}

	r = gdi_layer_create_using_outside_memory(s_idle_widget_ctrl.x, s_idle_widget_ctrl.y, 
		s_idle_widget_ctrl.w, s_idle_widget_ctrl.h, &s_idle_widget_ctrl.layer_handle, s_idle_widget_ctrl.layer_buf, s_idle_widget_ctrl.w * s_idle_widget_ctrl.h * sizeof(uint16));

	return MR_SUCCESS;
}


int32 mr_backstage_release_widget_layer(void)
{
	if(s_idle_widget_ctrl.layer_handle){
		gdi_layer_free(s_idle_widget_ctrl.layer_handle);
		s_idle_widget_ctrl.layer_handle = NULL;
	}

	if(s_idle_widget_ctrl.layer_buf){
		if(g_widget_layer_mem_type == MR_WIDGET_MEM_TYPE_MED)
			mr_mem_free_ex((void**)&s_idle_widget_ctrl.layer_buf);
		else if(g_widget_layer_mem_type == MR_WIDGET_MEM_TYPE_SCR)
			mr_pal_mem_scrmem_free(s_idle_widget_ctrl.layer_buf);
		
		s_idle_widget_ctrl.layer_buf = NULL;
	}

	return MR_SUCCESS;
}


int32 mr_backstage_get_status_icon_position(void)
{
	int32 x, y, w, h;
	mr_pal_status_icon_get_position(mr_backstage_get_actived_status_icon(), &x, &y, &w, &h);
	x = x + w/2 - MR_BACKSTAGE_WIDGET_MARGIN;
	if(x <= MR_BACKSTAGE_WIDGET_ARROW_WIDTH /2) x = MR_BACKSTAGE_WIDGET_ARROW_WIDTH /2 + MR_BACKSTAGE_WIDGET_MARGIN;
	if(x >= s_idle_widget_ctrl.w -MR_BACKSTAGE_WIDGET_ARROW_WIDTH /2) x = s_idle_widget_ctrl.w -MR_BACKSTAGE_WIDGET_ARROW_WIDTH /2 - MR_BACKSTAGE_WIDGET_MARGIN;
	return x;
}


static uint8* mr_backstage_load_icon(const char* filename)
{
	int32 fd = mr_open(filename, MR_FILE_RDONLY);
	if(fd > 0){
		UINT readBytes;
		UINT len = s_idle_widget_ctrl.tipinfo.icon_height * s_idle_widget_ctrl.tipinfo.icon_width * sizeof(int16);
		uint8* bmp = (uint8*)mr_mem_get_ex(len);
		if(bmp){
			readBytes = mr_read(fd, bmp, len);
		}
		
		mr_close(fd);
		return bmp;
	}

	return NULL;
}


static void mr_backstage_draw_hbar(int x1, int x2, int y, U16 img, int step_w)
{
	int i;

	gdi_image_draw_id(x2-step_w, y, img);
	for(i = x1; i < x2 - step_w; i+=step_w){
		gdi_image_draw_id(i, y, img);
	}
}


static void mr_backstage_draw_vbar(int x, int y1, int y2, U16 img, int step_h)
{
	int i;

	gdi_image_draw_id(x, y2-step_h, img);
	for(i = y1; i < y2 -step_h; i+=step_h){
		gdi_image_draw_id(x, i, img);
	}
}


static void mr_backstage_draw_hilite_bar(int y)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	gdi_image_draw_id(0, y, IMG_DSM_QIPAO_J);
	gdi_image_draw_id(s_idle_widget_ctrl.w-MR_BACKSTAGE_WIDGET_J_WIDTH, y, IMG_DSM_QIPAO_K);
	mr_backstage_draw_hbar(MR_BACKSTAGE_WIDGET_J_WIDTH, s_idle_widget_ctrl.w-MR_BACKSTAGE_WIDGET_J_WIDTH, y, IMG_DSM_QIPAO_L, MR_BACKSTAGE_WIDGET_F_WIDTH);
#endif	
}


static void mr_backstage_draw_idle_widget_background(void)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	int i, y;

	gdi_layer_clear(GDI_COLOR_TRANSPARENT);
	
	s_idle_widget_ctrl.arrow_x = mr_backstage_get_status_icon_position();	
	gdi_image_draw_id(0, MR_BACKSTAGE_WIDGET_E_HEIGHT, IMG_DSM_QIPAO_A);
	gdi_image_draw_id(s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_A_WIDTH, MR_BACKSTAGE_WIDGET_E_HEIGHT, IMG_DSM_QIPAO_B);
	gdi_image_draw_id(0, s_idle_widget_ctrl.h-MR_BACKSTAGE_WIDGET_C_HEIGHT, IMG_DSM_QIPAO_C);
	gdi_image_draw_id(s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_C_WIDTH, s_idle_widget_ctrl.h-MR_BACKSTAGE_WIDGET_C_HEIGHT, IMG_DSM_QIPAO_D);
	gdi_draw_solid_rect(MR_BACKSTAGE_WIDGET_C_WIDTH, MR_BACKSTAGE_WIDGET_A_HEIGHT+MR_BACKSTAGE_WIDGET_E_HEIGHT, 
		s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_C_WIDTH -1, s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_C_HEIGHT -1, 
		MR_BACKSTAGE_WIDGET_BG_COLOR);
	
	mr_backstage_draw_hbar(MR_BACKSTAGE_WIDGET_A_WIDTH, s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_A_WIDTH, MR_BACKSTAGE_WIDGET_E_HEIGHT, IMG_DSM_QIPAO_F, MR_BACKSTAGE_WIDGET_F_WIDTH);
	mr_backstage_draw_hbar(MR_BACKSTAGE_WIDGET_C_WIDTH, s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_C_WIDTH, s_idle_widget_ctrl.h-MR_BACKSTAGE_WIDGET_C_HEIGHT, IMG_DSM_QIPAO_G, MR_BACKSTAGE_WIDGET_F_WIDTH);
	mr_backstage_draw_vbar(0, MR_BACKSTAGE_WIDGET_A_HEIGHT+MR_BACKSTAGE_WIDGET_E_HEIGHT, s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_C_HEIGHT, IMG_DSM_QIPAO_H, MR_BACKSTAGE_WIDGET_F_WIDTH);
	mr_backstage_draw_vbar(s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_C_WIDTH, MR_BACKSTAGE_WIDGET_A_HEIGHT+MR_BACKSTAGE_WIDGET_E_HEIGHT, s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_C_HEIGHT, IMG_DSM_QIPAO_I, MR_BACKSTAGE_WIDGET_F_WIDTH);
	gdi_image_draw_id(s_idle_widget_ctrl.arrow_x -MR_BACKSTAGE_WIDGET_E_WIDTH/2, 0, IMG_DSM_QIPAO_E);

	if(s_idle_widget_ctrl.tipinfo.icon_path){
		mr_backstage_draw_hilite_bar(MR_BACKSTAGE_WIDGET_NEWMSG_BAR_Y);
		gdi_image_draw_id(MR_BACKSTAGE_WIDGET_C_WIDTH, MR_BACKSTAGE_WIDGET_M_Y, IMG_DSM_QIPAO_M1);
		gdi_image_draw_id(MR_BACKSTAGE_WIDGET_C_WIDTH + MR_BACKSTAGE_WIDGET_M_WIDTH -MR_BACKSTAGE_WIDGET_M1_WIDTH, MR_BACKSTAGE_WIDGET_M_Y, IMG_DSM_QIPAO_M3);
		mr_backstage_draw_hbar(MR_BACKSTAGE_WIDGET_C_WIDTH+MR_BACKSTAGE_WIDGET_M1_WIDTH, MR_BACKSTAGE_WIDGET_C_WIDTH + MR_BACKSTAGE_WIDGET_M_WIDTH - MR_BACKSTAGE_WIDGET_M1_WIDTH, MR_BACKSTAGE_WIDGET_M_Y, IMG_DSM_QIPAO_M2, MR_BACKSTAGE_WIDGET_M1_WIDTH);
	}

	y = s_idle_widget_ctrl.line_y + MR_BACKSTAGE_WIDGET_LINE_MARGIN;
	gdi_draw_line(MR_BACKSTAGE_WIDGET_C_WIDTH, y, s_idle_widget_ctrl.w -MR_BACKSTAGE_WIDGET_C_WIDTH, y, MR_BACKSTAGE_WIDGET_LINE1_COLOR);
	gdi_draw_line(MR_BACKSTAGE_WIDGET_C_WIDTH, y + 1, s_idle_widget_ctrl.w -MR_BACKSTAGE_WIDGET_C_WIDTH, y+1, MR_BACKSTAGE_WIDGET_LINE2_COLOR);

	if(s_idle_widget_ctrl.tipinfo.style){
		mr_backstage_draw_hilite_bar(s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_MARGIN_DOWN -MR_BACKSTAGE_WIDGET_J_HEIGHT);
	}
#endif	
}


static int32 mr_backstage_idle_widget_measure_string(UI_string_type str, S32 xwidth, S32* width, S32* height)
{
	int i;
	S32 tmp_width, line_width;	

	for(i = 0; str[i]; i++)
	{
		gui_measure_string_n(str, i+1, &tmp_width, height);
		if(tmp_width > xwidth)
			return i;
		line_width = tmp_width;
	}

	return i;
}


static void mr_backstage_draw_msgcount(MR_BOOL erase_bg)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	int tx, ty, tw;
	WCHAR msgcount[16];
	S32 width, height; 

	if(s_idle_widget_ctrl.tipinfo.msgcount <= 0)
		return;

	if(erase_bg){
		mr_backstage_draw_hbar(MR_BACKSTAGE_WIDGET_C_WIDTH + MR_BACKSTAGE_WIDGET_M_WIDTH, s_idle_widget_ctrl.w-MR_BACKSTAGE_WIDGET_J_WIDTH, MR_BACKSTAGE_WIDGET_NEWMSG_BAR_Y, IMG_DSM_QIPAO_L, MR_BACKSTAGE_WIDGET_M1_WIDTH);
	}

	//draw the message count
	tw = s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT*3 - MR_BACKSTAGE_WIDGET_M_WIDTH;
	kal_wsprintf(msgcount, "%d", s_idle_widget_ctrl.tipinfo.msgcount);
	kal_wstrcat(msgcount, (WCHAR*)GetString(STR_DSM_NEWMSG));
	gui_measure_string(msgcount, &width, &height);	
	if(width > tw){
		tx = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT*2 + MR_BACKSTAGE_WIDGET_M_WIDTH;
	}else{
		tx = s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT -width;
	}
	
	gui_set_text_color(MR_BACKSTAGE_WIDGET_BTN_COLOR);
	gui_print_truncated_text(tx, MR_BACKSTAGE_WIDGET_NEWMSG_BAR_Y + (MR_BACKSTAGE_WIDGET_J_HEIGHT-height)/2, tw, msgcount);
#endif	
}


static gdi_handle s_saved_layer;
static void mr_backstage_setup_draw_context(void)
{
	stFontAttribute	 MMI_medium_font = { 0,0,0,MEDIUM_FONT,0,1 };
	/*setup the context*/
	gdi_layer_push_and_set_active(s_idle_widget_ctrl.layer_handle);	
	gdi_layer_set_source_key(TRUE, GDI_COLOR_TRANSPARENT);
	gdi_layer_push_clip();
	gdi_layer_reset_clip();
	gdi_layer_lock_frame_buffer();
	gui_push_text_clip();
	gui_set_text_clip(0, 0, s_idle_widget_ctrl.w, s_idle_widget_ctrl.h);
	gui_set_font(&MMI_medium_font);
	gdi_get_alpha_blending_source_layer(&s_saved_layer);
	gdi_set_alpha_blending_source_layer(s_idle_widget_ctrl.layer_handle);
}


static void mr_backstage_restore_draw_context(void)
{
	gdi_layer_pop_clip();
	gui_pop_text_clip();
	gdi_layer_pop_and_restore_active();	
	gdi_set_alpha_blending_source_layer(s_saved_layer);
	gdi_layer_unlock_frame_buffer();
}


void mr_backstage_draw_idle_widget(void)
{
	int tx, ty, tw;
	int32 start_time = mr_getTime();

	mr_backstage_setup_draw_context();
	mr_backstage_draw_idle_widget_background();

	if(s_idle_widget_ctrl.tipinfo.icon_path){
		/* 如果有图片的话，界面布局会不一样 */
		WCHAR msgcount[16];
		S32 width, height; 
		uint8* bmp = mr_backstage_load_icon((char*)s_idle_widget_ctrl.tipinfo.icon_path);
		if(bmp){
			mr_draw_bitmap((U16*)s_idle_widget_ctrl.layer_buf, (U16*)bmp, MR_BACKSTAGE_WIDGET_ICON_X, MR_BACKSTAGE_WIDGET_ICON_Y
				, s_idle_widget_ctrl.tipinfo.icon_width, s_idle_widget_ctrl.tipinfo.icon_height, s_idle_widget_ctrl.w, MR_BACKSTAGE_WIDGET_ICON_TCOLOR);
			mr_mem_free_ex((void**)&bmp);
		}

		//draw the message count
		mr_backstage_draw_msgcount(MR_FALSE);
		
		//cal the title position
		tw = s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT*3 - MR_BACKSTAGE_WIDGET_M_WIDTH;		
		ty = s_idle_widget_ctrl.line_y - s_idle_widget_ctrl.title_h;
		if(s_idle_widget_ctrl.title_w > tw){
			tx = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT*2 + MR_BACKSTAGE_WIDGET_M_WIDTH;
		}else{
			tx = s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT -s_idle_widget_ctrl.title_w;
		}
	}else{
		tw = s_idle_widget_ctrl.text_w;
		ty = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_TOP;	
		if(s_idle_widget_ctrl.title_w > s_idle_widget_ctrl.text_w){
			tx = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT;
		}else{
			tx = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT + (s_idle_widget_ctrl.text_w - s_idle_widget_ctrl.title_w)/2;
		}
	}

	/* 绘制标题 */
	gui_set_text_color(MR_BACKSTAGE_WIDGET_TEXT_COLOR);
	gui_print_truncated_text(tx, ty, tw, s_idle_widget_ctrl.tipinfo.title);

	/* 绘制内容 */
	if(s_idle_widget_ctrl.tipinfo.content){
		int i;
		UI_string_type tmpstr = (UI_string_type)s_idle_widget_ctrl.tipinfo.content;
		ty = s_idle_widget_ctrl.line_y + MR_BACKSTAGE_WIDGET_LINE_HEIGHT;
		gui_set_text_color(MR_BACKSTAGE_WIDGET_TEXT_COLOR);
		for(i = 0; i < s_idle_widget_ctrl.linecount-1; i++){
			gui_move_text_cursor(MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT, ty);
			//mr_trace("backstage show context, %d, %d", i, s_idle_widget_ctrl.linesinfo[i]);
			gui_print_text_n(tmpstr, s_idle_widget_ctrl.linesinfo[i]);
			ty += s_idle_widget_ctrl.title_h + MR_BACKSTAGE_WIDGET_CONTENT_LINESCAPE;
			tmpstr += s_idle_widget_ctrl.linesinfo[i];
		}
		
		//print the last line
		gui_print_truncated_text(MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT, ty, s_idle_widget_ctrl.text_w, tmpstr);
	}

	/* 如果是模态的话，需要显示确认按钮 */
	if(s_idle_widget_ctrl.tipinfo.style){
		ty = s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_MARGIN_DOWN - MR_BACKSTAGE_WIDGET_J_HEIGHT + (MR_BACKSTAGE_WIDGET_J_HEIGHT-s_idle_widget_ctrl.text_h)/2;
		gui_set_text_color(MR_BACKSTAGE_WIDGET_BTN_COLOR);
		mmi_fe_show_string_baseline(MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT,  ty, (U8*)s_idle_widget_ctrl.tipinfo.lsk, -1, 0);
		mmi_fe_show_string_baseline(s_idle_widget_ctrl.w - s_idle_widget_ctrl.rsk_w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT,  ty, (U8*)s_idle_widget_ctrl.tipinfo.rsk, -1, 0);
	}

	mr_trace("mr_backstage_draw_idle_widget: %d", mr_getTime() - start_time);
	mr_backstage_restore_draw_context();
}


void mr_backstage_idle_widget_layout(void)
{
	S32 width, height, text_h;
	int32 x, y, w, h;
	int i, next = 0;	
	stFontAttribute	 MMI_medium_font = { 0,0,0,MEDIUM_FONT,0,1 };

	mr_trace("mr_backstage_idle_widget_layout: %d", __LINE__);

	s_idle_widget_ctrl.x = MR_BACKSTAGE_WIDGET_MARGIN;
	mr_pal_status_icon_bar_get_position(&x, &y, &w, &h);
	s_idle_widget_ctrl.y = y + h + MR_BACKSTAGE_WIDGET_MARGIN/2;
	s_idle_widget_ctrl.w = UI_device_width - 2*MR_BACKSTAGE_WIDGET_MARGIN;

	s_idle_widget_ctrl.text_w = s_idle_widget_ctrl.w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT*2; 

	//title
	gui_set_font(&MMI_medium_font);
	gui_measure_string(s_idle_widget_ctrl.tipinfo.title, &width, &height);
	s_idle_widget_ctrl.title_w = width;
	s_idle_widget_ctrl.title_h = height;
	
	if(s_idle_widget_ctrl.tipinfo.icon_path){ 
		s_idle_widget_ctrl.h =  MR_BACKSTAGE_WIDGET_M_Y + MR_BACKSTAGE_WIDGET_M_HEIGHT + MR_BACKSTAGE_WIDGET_LINE_HEIGHT + MR_BACKSTAGE_WIDGET_MARGIN_DOWN;
		s_idle_widget_ctrl.line_y = MR_BACKSTAGE_WIDGET_M_Y + MR_BACKSTAGE_WIDGET_M_HEIGHT;
	}else{
		s_idle_widget_ctrl.h = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_TOP + MR_BACKSTAGE_WIDGET_LINE_HEIGHT + MR_BACKSTAGE_WIDGET_MARGIN_DOWN + s_idle_widget_ctrl.title_h;
		s_idle_widget_ctrl.line_y = MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_TOP + s_idle_widget_ctrl.title_h;
	}

	if(s_idle_widget_ctrl.tipinfo.style){
		s_idle_widget_ctrl.h += MR_BACKSTAGE_WIDGET_J_HEIGHT + MR_BACKSTAGE_WIDGET_BUTTON_SPACE;
		if(s_idle_widget_ctrl.tipinfo.lsk){
			gui_measure_string(s_idle_widget_ctrl.tipinfo.lsk, &width, &height);
			s_idle_widget_ctrl.lsk_w = width;
		}

		if(s_idle_widget_ctrl.tipinfo.rsk){
			gui_measure_string(s_idle_widget_ctrl.tipinfo.rsk, &width, &height);
			s_idle_widget_ctrl.rsk_w = width;
		}
	}

	height = text_h = 0 ;
	s_idle_widget_ctrl.linecount = 0;
	for(i = 0; i < MR_BACKSTAGE_IDLE_WIDGET_CONTENT_MAXLINE; i++){
		s_idle_widget_ctrl.linesinfo[i] = mr_backstage_idle_widget_measure_string(s_idle_widget_ctrl.tipinfo.content + next, s_idle_widget_ctrl.text_w, &width, &height);
		if(s_idle_widget_ctrl.y  + s_idle_widget_ctrl.h + text_h + height> UI_device_height - MR_BACKSTAGE_WIDGET_CONTENT_LINESCAPE)
			break;
			
		next += s_idle_widget_ctrl.linesinfo[i];
		s_idle_widget_ctrl.linecount++;
		text_h += height + MR_BACKSTAGE_WIDGET_CONTENT_LINESCAPE;
		if(s_idle_widget_ctrl.tipinfo.content[next] == 0){
			break;
		}
	}
	
	s_idle_widget_ctrl.text_h = height;
	s_idle_widget_ctrl.h += text_h - MR_BACKSTAGE_WIDGET_CONTENT_LINESCAPE;
}


int32 mr_backstage_idle_widget_in_widget(int x, int y)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	return MR_PTINRECT(x, y, s_idle_widget_ctrl.x, s_idle_widget_ctrl.y + MR_BACKSTAGE_WIDGET_ARROW_HEIGHT, s_idle_widget_ctrl.w, s_idle_widget_ctrl.h );
#else
	return 0;
#endif
}


int32 mr_backstage_idle_widget_in_lsk(int x, int y)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	int32 ty;
	
	x -= s_idle_widget_ctrl.x;
	y -= s_idle_widget_ctrl.y;
	ty =ty = s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_MARGIN_DOWN - MR_BACKSTAGE_WIDGET_J_HEIGHT;

	return MR_PTINRECT(x, y, MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT, ty ,s_idle_widget_ctrl.lsk_w, MR_BACKSTAGE_WIDGET_J_HEIGHT);
#else
	return 0;
#endif
}


int32 mr_backstage_idle_widget_in_rsk(int x, int y)
{
#ifndef __MR_CFG_FEATURE_SLIM__
	int32 ty;
	
	x -= s_idle_widget_ctrl.x;
	y -= s_idle_widget_ctrl.y;
	ty =ty = s_idle_widget_ctrl.h - MR_BACKSTAGE_WIDGET_MARGIN_DOWN - MR_BACKSTAGE_WIDGET_J_HEIGHT;

 	return MR_PTINRECT(x, y, s_idle_widget_ctrl.w - s_idle_widget_ctrl.rsk_w - MR_BACKSTAGE_WIDGET_CONTENT_MARGIN_LEFT, ty, s_idle_widget_ctrl.rsk_w, MR_BACKSTAGE_WIDGET_J_HEIGHT);
#else
	return 0;
#endif
}


int32 mr_backstage_idle_widget_set_msgcount(int32 param)
{
	s_idle_widget_ctrl.tipinfo.msgcount = (int32)param;
	
	if(mr_backstage_is_idle_screen() == MR_SUCCESS && s_idle_widget_ctrl.layer_handle){
		mr_backstage_setup_draw_context();
		mr_backstage_draw_msgcount(MR_TRUE);
		mr_backstage_restore_draw_context();
		gdi_lcd_repaint_all();
	}

	return MR_SUCCESS;
}


int32 mr_backstage_online_mgr_support(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef __MR_CFG_FEATURE_ONLINE_MGR__
	if(output){
		int32* cfg = (int32*)output;
		*cfg = 0;
	}
	
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}

int32 mr_backstage_get_idle_shortcut_key(void)
{
	return MR_KEY_NONE;
}

#endif

