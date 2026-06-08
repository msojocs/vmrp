#ifndef __MRP_PAL_H__
#define __MRP_PAL_H__


typedef enum
{
    MR_PEN_EVENT_DOWN,
    MR_PEN_EVENT_UP,
    MR_PEN_EVENT_LONG_TAP,
    MR_PEN_EVENT_REPEAT,
    MR_PEN_EVENT_MOVE,
    MR_PEN_EVENT_ABORT,
    MR_PEN_EVENT_DOUBLE_CLICK,
    MR_PEN_EVENT_TYPE_MAX
} MR_PEN_EVENT_TYPE_E;



MR_BOOL mr_pal_is_in_call(void);


//memory
void* mr_pal_mem_malloc(uint32 size);
void mr_pal_mem_free(void* ptr);
/**
 * \brief scrmem分配函数
 *
 * \param mem_size	[in]欲分配的内存大小
 * \return 成功返回分配的内存起始地址，失败返回NULL
 */
void *mr_pal_mem_scrmem_alloc(U32 mem_size);


/**
 * \brief 释放scrmem内存
 *
 * \param mem_ptr	[in] 待释放的buffer
 */
void mr_pal_mem_scrmem_free(void *mem_ptr);


//status icon api
void mr_pal_status_icon_hide_status_icon(int16 icon_id);
void mr_pal_status_icon_show_status_icon(int16 icon_id);
void mr_pal_status_icon_blink_status_icon(int16 icon_id);
void mr_pal_status_icon_update_status_icons(void);
void mr_pal_register_status_icon_pen_event_hdlr(int16 icon_id, void (*f)(void));
MR_BOOL mr_pal_status_icon_whether_icon_display(int16 icon_id);
void mr_pal_status_icon_get_position(int16 icon_id, int32* x, int32* y, int32* w, int32* h);
void mr_pal_status_icon_bar_get_position(int32* x, int32* y, int32* w, int32* h);
int32 mr_pal_status_icon_get_gprs_status_icon(int16** status_icons);

#endif
