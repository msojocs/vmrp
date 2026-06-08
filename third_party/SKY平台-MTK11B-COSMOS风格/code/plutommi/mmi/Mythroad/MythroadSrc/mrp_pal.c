#ifdef __MMI_DSM_NEW__

#include "MMI_include.h"
#include "mrp_include.h"

//status icons
#if (MTK_VERSION >= 0x09b0952)
#include "gui_status_icon.h"
extern gui_status_icon_bar_icon_struct wgui_status_icon_bar_icons[];
extern gui_status_icon_bar_struct wgui_status_icon_bar_bars[];
#else
#include "wgui_status_icons.h"
extern MMI_status_icon MMI_status_icons[];
extern MMI_status_icon_bar MMI_status_icon_bars[];
#define TEST_STATUS_ICON_DISPLAY_ON(flags)   (flags & (STATUS_ICON_ANIMATE|STATUS_ICON_REVERSE_ANIMATE|STATUS_ICON_FORWARD_ANIMATE|STATUS_ICON_BLINK|STATUS_ICON_DISABLED|STATUS_ICON_DISPLAY))
#endif


#ifdef __MMI_UCM__
#include "ucmgprot.h"
#endif

#include "Wgui_categories_util.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


MR_BOOL mr_pal_is_in_call(void)
{
#if (MTK_VERSION>=0x09B0952)
	if(srv_ucm_query_call_count(SRV_UCM_CALL_STATE_ALL, SRV_UCM_CALL_TYPE_ALL, NULL))
		return MR_TRUE;
	else
		return MR_FALSE;
#else
	return isInCall();
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//memory


void* mr_pal_mem_malloc(uint32 size )
{
	return OslMalloc(size);
}


void mr_pal_mem_free(void* ptr)
{
	OslMfree(ptr);
}


void mr_pal_mem_scrmem_free(void *mem_ptr)
{
	applib_mem_ap_free(mem_ptr);
}


void *mr_pal_mem_scrmem_alloc(U32 mem_size)
{
	return applib_mem_ap_alloc_framebuffer(APPLIB_MEM_AP_ID_MYTHROAD, mem_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//status icon

void mr_pal_status_icon_hide_status_icon(int16 icon_id)
{
	HideStatusIcon(icon_id);
}


void mr_pal_status_icon_show_status_icon(int16 icon_id)
{
	ShowStatusIcon(icon_id);
}


void mr_pal_status_icon_blink_status_icon(int16 icon_id)
{
	BlinkStatusIcon(icon_id);
}


void mr_pal_status_icon_update_status_icons(void)
{
	UpdateStatusIcons();
}


void mr_pal_register_status_icon_pen_event_hdlr(int16 icon_id, void (* f)(void))
{
#ifdef __MMI_TOUCH_SCREEN__
#if (MTK_VERSION>=0x09B0952)
 	wgui_status_icon_bar_register_pen_event_handler(icon_id, WGUI_STATUS_ICON_BAR_PEN_SELECT_ICON, f);
#elif (MTK_VERSION >= 0x06b)
	wgui_register_status_icon_pen_event_hdlr(icon_id, WGUI_STATUS_ICON_BAR_PEN_SELECT_ICON, f);
#else			
	mmi_wgui_register_status_icon_pen_event_hdlr(icon_id, WGUI_STATUS_ICON_BAR_PEN_SELECT_ICON, f);
#endif
#endif
}


MR_BOOL mr_pal_status_icon_whether_icon_display(int16 icon_id)
{
#if(MTK_VERSION >= 0x09B0952)
	return gui_status_icon_bar_whether_icon_display(&wgui_status_icon_bar_icons[icon_id]);
#else
	if(TEST_STATUS_ICON_DISPLAY_ON(get_status_icon_flag(icon_id)))
		return MR_TRUE;
	return MR_FALSE;
#endif
}


void mr_pal_status_icon_get_position(int16 icon_id, int32 * x, int32 * y, int32 * w, int32 * h)
{
#if(MTK_VERSION >= 0x09B0952)
	*x = wgui_status_icon_bar_icons[icon_id].x;
	*y = wgui_status_icon_bar_icons[icon_id].y;
	*w = wgui_status_icon_bar_icons[icon_id].width;
	*h = wgui_status_icon_bar_icons[icon_id].height;
#else
	*x = MMI_status_icons[icon_id].x;
	*y = MMI_status_icons[icon_id].y;
	*w = MMI_status_icons[icon_id].width;
	*h = MMI_status_icons[icon_id].height;
#endif

	//mr_trace("mr_pal_status_icon_get_position: %d, %d, %d, %d, %d, %d", icon_id, mr_pal_status_icon_whether_icon_display(icon_id), *x, *y, *w, *h);
}


void mr_pal_status_icon_bar_get_position(int32 * x, int32 * y, int32 * w, int32 * h)
{
#if (MTK_VERSION >= 0x09b0952)
	*x = wgui_status_icon_bar_bars[WGUI_STATUS_ICON_BAR_H_BAR].x;
	*y = wgui_status_icon_bar_bars[WGUI_STATUS_ICON_BAR_H_BAR].y;
	*w = wgui_status_icon_bar_bars[WGUI_STATUS_ICON_BAR_H_BAR].width;
	*h = wgui_status_icon_bar_bars[WGUI_STATUS_ICON_BAR_H_BAR].height;
#else
	*x = MMI_status_icon_bars[0].x1;
	*y = MMI_status_icon_bars[0].y1;
	*w = MMI_status_icon_bars[0].x2 - MMI_status_icon_bars[0].x1;
	*h = MMI_status_icon_bars[0].y2 - MMI_status_icon_bars[0].y1;
#endif
}


void mr_pal_pen_register_handler(mmi_pen_hdlr pen_tbl[])
{
#ifdef __MMI_TOUCH_SCREEN__
	mmi_pen_register_down_handler(pen_tbl[MR_PEN_EVENT_DOWN]);
	mmi_pen_register_move_handler(pen_tbl[MR_PEN_EVENT_MOVE]);
	mmi_pen_register_up_handler(pen_tbl[MR_PEN_EVENT_UP]);
	mmi_pen_register_long_tap_handler(pen_tbl[MR_PEN_EVENT_LONG_TAP]);
	mmi_pen_register_repeat_handler(pen_tbl[MR_PEN_EVENT_REPEAT]);
	mmi_pen_register_abort_handler(pen_tbl[MR_PEN_EVENT_ABORT]);
	mmi_pen_register_double_click_handler(pen_tbl[MR_PEN_EVENT_DOUBLE_CLICK]);
#endif 
}


void mr_pal_pen_get_handler(mmi_pen_hdlr pen_tbl[])
{
#ifdef __MMI_TOUCH_SCREEN__
	pen_tbl[MR_PEN_EVENT_DOWN] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_DOWN);
	pen_tbl[MR_PEN_EVENT_MOVE] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_MOVE);
	pen_tbl[MR_PEN_EVENT_UP] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_UP);
	pen_tbl[MR_PEN_EVENT_LONG_TAP] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_LONG_TAP);
	pen_tbl[MR_PEN_EVENT_REPEAT] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_REPEAT);
	pen_tbl[MR_PEN_EVENT_ABORT] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_ABORT);
	pen_tbl[MR_PEN_EVENT_DOUBLE_CLICK] = mmi_pen_get_pen_handler(MMI_PEN_EVENT_DOUBLE_CLICK);
#endif 
}


int32 mr_pal_status_icon_get_gprs_status_icon(int16** status_icons)
{
#if 0
	static const int16 gprs_status_icons [] = {
		STATUS_ICON_GPRS_ATT_NO_PDP_INDICATOR,
		STATUS_ICON_GPRS_SERVICE,
		STATUS_ICON_EDGEC,
		STATUS_ICON_EDGE,
#ifdef __MMI_DUAL_SIM_MASTER__
		STATUS_ICON_SLAVE_EDGE,
		STATUS_ICON_SLAVE_GPRS_SERVICE,
		STATUS_ICON_SLAVE_EDGEC,
		STATUS_ICON_SLAVE_GPRS_ATT_NO_PDP_INDICATOR
#endif
	};

	if(status_icons) *status_icons = (int16*)gprs_status_icons;
	return sizeof(gprs_status_icons)/sizeof(gprs_status_icons[0]);
#else
	return 0;
#endif
}


#endif

