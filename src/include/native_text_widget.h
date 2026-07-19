#ifndef __VMRP_NATIVE_TEXT_WIDGET_H__
#define __VMRP_NATIVE_TEXT_WIDGET_H__

#include <stdint.h>

/*
 * 平台文本框(SKYENGINE mr_textCreate/mr_textRelease/mr_textRefresh 的宿主实现)。
 *
 * 真机上文本框是手机平台绘制的全屏只读文本窗口:显示期间按键归平台所有,
 * 平台把软键选择转换成 mr_event(MR_DIALOG_EVENT, MR_DIALOG_KEY_OK/CANCEL)
 * 通知应用,应用收到后调用 mr_textRelease 关闭。此前宿主对 mr_textCreate
 * 直接返回 MR_FAILED,而真实应用(如 gtdgdq 帮助页)先注册模态状态再调
 * mr_textCreate 且不回滚失败,导致应用永远等不到 MR_DIALOG_EVENT,所有
 * 普通按键被模态分发器吞掉,表现为"卡死在菜单"。
 */

/* DSM 桥接:native_dsm_funcs.c 的 textCreate/Release/Refresh 委托到这里。
 * title/text 为 UCS2 大端(网络字节序)字符串,与 SKYENGINE API 手册一致。 */
int32_t native_text_widget_create(const char *title_ucs2be, const char *text_ucs2be, int32_t type);
int32_t native_text_widget_release(int32_t handle);
int32_t native_text_widget_refresh(int32_t handle, const char *title_ucs2be, const char *text_ucs2be);

/* 文本框是否正在显示。 */
int native_text_widget_active(void);

/*
 * 上屏路径钩子:所有 guest 帧上屏(guiDrawBitmapWithStride)前先经过这里。
 * 把该帧写入"guest 显示镜像"(文本框关闭时用它恢复被遮盖的画面),并在
 * 文本框显示期间返回 1 表示本帧不要上屏(真机语义:平台窗口盖在应用画面
 * 之上,应用的绘制发生在窗口之下不可见)。返回 0 表示照常上屏。
 */
int native_text_widget_capture_frame(const uint16_t *bmp, int32_t x, int32_t y,
                                     int32_t w, int32_t h, int32_t stride,
                                     int32_t sx, int32_t sy);

/*
 * 事件钩子:guest 事件入口(skyengine_runtime_event)前置过滤。
 * 返回 0=未激活不拦截;1=事件已被平台消费(不再投递给应用);
 * 2=软键命中文本框按钮,调用方应改投 MR_DIALOG_EVENT,参数写入 *dialog_param。
 */
int native_text_widget_filter_event(int32_t code, int32_t p0, int32_t *dialog_param);

#endif
