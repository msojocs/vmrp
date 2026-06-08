#ifndef __MRP_PAL_LOCALUI_H__
#define __MRP_PAL_LOCALUI_H__


/**
 * \brief 通过platEx 创建Menu的参数结构
 *
 * \sa PlatEx code(1)
 */
typedef struct
{
    char *title;
    int16 itemNum;
    int16 listType;
}T_LISTBOXWIN;


/**
 * \brief 通过platEx设置Menu Item的参数结构
 *
 * \sa PlatEx code(2)
 */
typedef struct
{
    int32 menu;
    char *text;
    int32 index;
    int32 selected;
}T_LISTBOXITEM;


/**
 * \brief 高亮选择项
 */
typedef struct
{
	int32 menu;
	int32 hilite;
}T_LISTBOXFOCUS;



/* 用来表示是否内部窗口覆盖 */
extern MR_BOOL  internalCovered;


/**
 * \brief 本地UI模块初始化
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_localui_initialize(void);


/**
 * \brief 本地UI模块释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_localui_terminate(void);


/**
 * \brief 释放全部本地UI节点
 *
 * \return void
 */
void mr_localui_free_all_screen(void);


/**
 * \brief 分配一个screen id
 *
 * \return 新的screen id
 */
int32 dsmAllocScrId(void);


/**
 * \brief 释放全部的screen id
 */
void dsmFreeScrIdAll(void);


/**
 * \brief 创建一个普通的菜单窗口
 *
 * \param title		[in]菜单的标题(_大端模式的unicode)
 * \param num		[in]菜单项的个数
 * \return 创建好的菜单的句柄, 失败返回MR_FAILED
 */
int32 mr_menuCreate(const char * title, int16 num);


/**
 * \brief 改变当前focus的菜单项
 * 
 * \param menu		[in]菜单句柄
 * \param index		[in]选中项的index
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_menuSetFocus(int32 menu, int32 index);


/**
 * \brief 初始化每一个菜单项
 *
 * \param menu	[in] 菜单句柄
 * \param text	[in] 菜单项的名字(大端unicode)
 * \param index	[in] 要初始化的菜单项的索引
 * \param select	[in] 是否选中
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_menuSetItemEx(int32 menu, const char *text, int32 index,int32 select);


/**
 * \brief 设置普通菜单项
 *
 * \param menu	[in] 菜单句柄
 * \param text	[in] 菜单项的名字(大端unicode)
 * \param index	[in] 要初始化的菜单项的索引
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_menuSetItem(int32 menu, const char *text, int32 index);


/**
 * \brief 绘制菜单
 *
 * \param menu	[in]菜单句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_menuShow(int32 menu);


/**
 * \brief 重绘菜单的函数(但是一般mmi架构会自动完成，实现成空就可以了啊)
 *
 * \param menu	[in]要重绘的菜单的句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_menuRefresh(int32 menu);


/**
 * \brief 销毁菜单
 *
 * \param menu	[in] 要销毁的菜单句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_menuRelease(int32 menu);


/**
 * \brief 创建一个dialog对象
 *
 * \param title	[in]对话框的标题
 * \param text	[in]对话框要显示的询问的内容
 * \param type	[in]对话框按键的类型
 * \return 创建好的对话框的句柄, 失败返回MR_FAILED
 */
int32 mr_dialogCreate(const char * title, const char * text, int32 type);


/**
 * \brief 销毁一个对话框
 *
 * \param dialog	[in] 对话框句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_dialogRelease(int32 dialog);


/**
 * \brief 重画dialog
 *
 * \param dialog	[in]对话框句柄
 * \param title		[in]对话框标题
 * \param text		[in]对话框要显示的询问的内容
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_dialogRefresh(int32 dialog, const char * title, const char * text, int32 type);


/**
 * \brief 创建一个文本显示框对象
 *
 * \param title	[in]文本显示框的标题
 * \param text	[in]文本显示框要显示的内容
 * \param type	[in]左右软件显示的类型
 * \return 创建好的文本显示框的句柄, 失败返回MR_FAILED
 */
int32 mr_textCreate(const char * title, const char * text, int32 type);


/**
 * \brief 重画文本显示框
 *
 * \param textid	[in]文本显示框的句柄
 * \param title		[in]文本显示框的标题
 * \param text		[in]文本显示框的内容
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_textRefresh(int32 textid, const char * title, const char * text);


/**
 * \brief 销毁一个文本显示框
 *
 * \param text	[in]文本显示框的句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_textRelease(int32 text);


/**
 * \brief 创建一个编辑框对象
 *
 * \param title		[in]编辑框的标题
 * \param text		[in]编辑框默认的编辑内容
 * \param type		[in]编辑框编辑的内容的类型
 * \param max_size	[in]编辑框编辑的最大个数
 * \return 创建成功的EDIT的句柄, 失败返回MR_FAILED 
 */
int32 mr_editCreate(const char * title, const char * text, int32 type, int32 max_size);


/**
 * \brief 销毁一个编辑框对象
 *
 * \param edit	[in]要销毁的编辑框的对象
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_editRelease(int32 edit);


/**
 * \brief 取得编辑框编辑的内容
 *
 * \param edit	[in] 编辑框的句柄
 * \return 编辑框的内容
 */
const char* mr_editGetText(int32 edit);


/**
 * \brief 创建一个空的界面
 *
 * \return 创建好的界面的句柄, 失败返回MR_FAILED
 */
int32 mr_winCreate(void);


/**
 * \brief 销毁一个空界面
 *
 * \param win	[in]窗口的句柄
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_winRelease(int32 win);


/**
 * \brief 设置标题
 *
 * \param pTitle	[in] 新标题
 * \return void
 */
void dsm_set_title(U16 **pTitle);


int32 mr_localui_menu_create_ex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_localui_menu_set_menuitem_ex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_localui_menu_set_focus(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

// handle 为创建的窗口的句柄
typedef  int32 (*mr_localui_event_handler)(int32 handle, int16 type ,int32 param1,int32 param2);

int32 mr_menuCreate_ex(const char * title, int16 num,int16 type, mr_localui_event_handler handler);
int32 mr_dialogCreate_ex(const char * title, const char * text, int32 type, mr_localui_event_handler handler);
int32 mr_textCreate_ex(const char * title, const char * text, int32 type, mr_localui_event_handler handler);
int32 mr_editCreate_ex(const char * title, const char * text, int32 type, int32 max_size, mr_localui_event_handler handler);
int32 mr_winCreate_ex(mr_localui_event_handler handler);

#endif
