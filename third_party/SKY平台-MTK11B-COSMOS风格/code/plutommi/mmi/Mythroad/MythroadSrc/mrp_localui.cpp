
#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"

#include "vfx_mc_include.h"
#include "vcp_include.h"
#include "../xml/vfx_xml_loader.h"
extern "C"
{
#include "GlobalResDef.h"   
}
#include "mmi_rp_all_defs.h"

#include "mrp_include.h"


#define DSM_EDIT_BUF_MAX                              512
#define DSM_TEXT_LEN                                       (1024*2)
#define DSM_DIALOG_LEN                                   (512)
#define DSM_TITLE_LEN                                      50
#define DSM_STACK_SIZE                                    20


#if (MTK_VERSION >= 0x0824)
#define INPUT_TYPE_PHONE_NUMBER						IMM_INPUT_TYPE_NUMERIC
#define INPUT_TYPE_ALPHANUMERIC_PASSWORD			IMM_INPUT_TYPE_ALPHANUMERIC_PASSWORD
#define INPUT_TYPE_ALPHANUMERIC_SENTENCECASE			IMM_INPUT_TYPE_SENTENCE
#endif


typedef enum
{
    MR_LIST_NO_ICON = 0,
    MR_LIST_NORMAL_ICON = 1,
    MR_LIST_SINGLE_SEL = 2,
    MR_LIST_MUTI_SEL = 3,
    MR_LIST_TYPE_MAX
}E_DSM_LISTTYPE;


typedef enum
{
	 DSM_TEXT,
	 DSM_EDIT,
	 DSM_MENU,
	 DSM_DIALOG,
	 DSM_WIN,
	 DSM_WEATHER
}T_DSM_WIN_TYPE;


typedef struct 
{
	 int32 CurrSrcID;
	 char title[DSM_TITLE_LEN];
	 T_DSM_WIN_TYPE type;
}T_DSM_NODE;


typedef struct 
{
	T_DSM_NODE node;
	int32 texttype;
	char *data;
}T_DSM_TEXT;


typedef struct 
{
	T_DSM_NODE node;
	int32 edittype;
	int32 max;
	char *data;
	char *pTmp;
}T_DSM_EDIT;


typedef struct 
{
	T_DSM_NODE node;
	int32 dialogtype;
	char *data;
}T_DSM_DIALOG;


typedef struct 
{ 
	T_DSM_NODE node;
	int16 hilight;
	int16 num;
	char *item[MAX_SUB_MENUS];
	char select[MAX_SUB_MENUS];
	int16 listType;
}T_DSM_MENU;


MR_BOOL  internalCovered = MR_FALSE;


class VappLocalUIApp : public VfxApp
{
	VFX_DECLARE_CLASS(VappLocalUIApp);

public:
	virtual void onRun(void* args, VfxU32 argSize);
	virtual void onDeinit();

protected:
	virtual void onGroupActive();
	virtual void onGroupInactive();
};

class VappLocalUIScreen : public VfxMainScr
{
	VFX_DECLARE_CLASS(VappLocalUIScreen);

public:
	VappLocalUIScreen();
	
protected:
	virtual void onInit();
	virtual void onDeinit();
};

class VappMenuPage : public VfxPage, public IVcpListMenuContentProvider
{
	VFX_DECLARE_CLASS(VappMenuPage);
	
public:
	VappMenuPage() {}
	VappMenuPage(const VfxWString& title, VfxU32 count, VfxS32 type, mr_localui_event_handler handler);
	virtual void onInit();
	void refresh();
	
	VfxWString m_itemTexts[MAX_SUB_MENUS];
	VfxS32 m_itemSelects[MAX_SUB_MENUS];
	VfxWString m_title;
	VfxU32 m_itemCount;
	VfxS32 m_type;
	mr_localui_event_handler m_handler;
	void postHandler(mmi_id handle, int16 type, int32 param1,int32 param2);
	
protected:
	virtual VfxU32 getCount() const { return m_itemCount; };
	virtual VfxBool getItemText(VfxU32 index, VcpListMenuFieldEnum fieldType, 
		VfxWString &text, VcpListMenuTextColorEnum &color
        );
	virtual VcpListMenuItemStateEnum getItemState(VfxU32 index) const;

	virtual VfxBool onKeyInput(VfxKeyEvent &event);	
	
private:
	void onToolBarTap(VfxObject* sender, VfxId senderId);
	void onSelectItem(VcpListMenu *listMenu, VfxU32 selectIdx);

	VcpTitleBar* m_titleBar;
	VcpListMenu* m_listMenu;
	VcpToolBar* m_toolBar;
};

class VappTextPage : public VfxPage
{
	VFX_DECLARE_CLASS(VappTextPage);
	
public:
	VappTextPage() {}
	VappTextPage(const VfxWString& title, const VfxWString& text, VfxS32 type, mr_localui_event_handler handler);
	virtual void onInit();
	void refresh(const VfxWString& title, const VfxWString& text, VfxS32 type);

	VfxWString m_title;
	VfxWString m_text;
	VfxS32 m_type;
	mr_localui_event_handler m_handler;
	void postHandler(mmi_id handle, int16 type, int32 param1,int32 param2);
	
protected:
	virtual VfxBool onKeyInput(VfxKeyEvent &event);
	
private:
	void onToolBarTap(VfxObject* sender, VfxId senderId);
	
	VcpTitleBar* m_titleBar;
	VcpToolBar* m_toolBar;
	VcpTextView* m_textView;
};

class VappEditPage : public VfxPage
{
	VFX_DECLARE_CLASS(VappEditPage);
	
public:
	VappEditPage() {}
	VappEditPage(const VfxWString& title, const VfxWString& text, VfxS32 type, VfxS32 max_size, mr_localui_event_handler handler);
	virtual void onInit();
	VfxWString getText();

	VfxWString m_title;
	VfxWString m_text;
	VfxS32 m_type;
	VfxS32 m_maxSize;
	mr_localui_event_handler m_handler;
	void postHandler(mmi_id handle, int16 type, int32 param1,int32 param2);
	
protected:
	virtual VfxBool onKeyInput(VfxKeyEvent &event);
	
private:
	void onToolBarTap(VfxObject* sender, VfxId senderId);
	
	VcpTitleBar* m_titleBar;
	VcpToolBar* m_toolBar;
	VcpTextEditor* m_textEditor;
};

class VappWinPage : public VfxPage
{
	VFX_DECLARE_CLASS(VappWinPage);
	
public:
	VappWinPage() {}
	VappWinPage(mr_localui_event_handler handler);
	virtual void onInit();

	mr_localui_event_handler m_handler;
	void postHandler(mmi_id handle, int16 type, int32 param1,int32 param2);

protected:
	virtual VfxBool onKeyInput(VfxKeyEvent &event);
	virtual VfxBool onPenInput(VfxPenEvent &event);
};


static VappLocalUIApp* localui_app = NULL;

// 这个函数在创建本地窗口的时候调用
static VappLocalUIApp* mr_localui_get_app_instance()
{
	mmi_id gid;
	mmi_frm_asm_property_struct asm_property;
	
	if (localui_app == NULL)
	{
		internalCovered = MR_TRUE;
		gid = VfxAppLauncher::launch(VAPP_MYTHROAD, VFX_OBJ_CLASS_INFO(VappLocalUIApp), GRP_ID_ROOT);
		localui_app = (VappLocalUIApp*)VfxApp::getObject(gid);

		mmi_frm_asm_get_property(gid, &asm_property);
		asm_property.f_hide_in_oom = 1;   
		mmi_frm_asm_set_property(gid, &asm_property);
	}
	
	return localui_app;
}

// 这个在刷新和关闭本地窗口的时候调用
static VappLocalUIApp* mr_localui_get_app()
{	
	return localui_app;
}


VFX_IMPLEMENT_CLASS("VappLocalUIApp", VappLocalUIApp, VfxApp);

void VappLocalUIApp::onRun(void * args, VfxU32 argSize)
{
	VfxApp::onRun(args, argSize);
}

void VappLocalUIApp::onDeinit()
{
	localui_app = NULL;
}

void VappLocalUIApp::onGroupActive()
{
	if (getScreenCount())
		mr_app_resume();
}

void VappLocalUIApp::onGroupInactive()
{
	if (getScreenCount())
		mr_app_pause();
}


VFX_IMPLEMENT_CLASS("VappLocalUIScreen", VappLocalUIScreen, VfxMainScr);

VappLocalUIScreen::VappLocalUIScreen()
{

}

void VappLocalUIScreen::onInit()
{
	VfxMainScr::onInit();

	setTransitType(VFX_SCR_TRANSIT_TYPE_NONE);
}

void VappLocalUIScreen::onDeinit()
{

}


VFX_IMPLEMENT_CLASS("VappMenuPage", VappMenuPage, VfxPage);

VappMenuPage::VappMenuPage(const VfxWString& title, VfxU32 count, VfxS32 type, mr_localui_event_handler handler) : m_title(title), m_itemCount(count), m_type(type), m_handler(handler)
{
	
}

void VappMenuPage::onInit()
{
	VfxPage::onInit();

	memset(m_itemSelects, 0 , sizeof(m_itemSelects));

	VFX_OBJ_CREATE(m_titleBar, VcpTitleBar, this);
	m_titleBar->setTitle(m_title);
	setTopBar(m_titleBar);  

	VFX_OBJ_CREATE(m_toolBar, VcpToolBar, this);
	m_toolBar->addItem(1, VFX_WSTR_RES(STR_GLOBAL_BACK), VCP_IMG_TOOL_BAR_COMMON_ITEM_CANCEL);
	m_toolBar->m_signalButtonTap.connect(this, &VappMenuPage::onToolBarTap);
	setBottomBar(m_toolBar);

	VFX_OBJ_CREATE(m_listMenu, VcpListMenu, this);
	m_listMenu->setCellStyle(VCP_LIST_MENU_CELL_STYLE_SINGLE_TEXT);
	m_listMenu->setContentProvider(this);
	m_listMenu->setSize(getSize());		 
	m_listMenu->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);
	m_listMenu->m_signalItemTapped.connect(this, &VappMenuPage::onSelectItem); 

	if (m_type == MR_LIST_SINGLE_SEL)
	{
		m_listMenu->setMenuMode(VCP_LIST_MENU_MODE_SINGLE_SELECTION, VFX_TRUE);    
	}
	else if (m_type == MR_LIST_MUTI_SEL)
	{
		m_listMenu->setMenuMode(VCP_LIST_MENU_MODE_MULTI_SELECTION, VFX_TRUE);    
	}
	else
	{
		m_listMenu->setMenuMode(VCP_LIST_MENU_MODE_NORMAL, VFX_TRUE);    
	}
}

void VappMenuPage::onToolBarTap(VfxObject* sender, VfxId senderId)
{
	if (senderId == 1)
	{
		vfxPostInvoke4(this, &VappMenuPage::postHandler, getMainScr()->getScreenId(), (int16)MR_MENU_RETURN, 0, 0);
	}
}

void VappMenuPage::onSelectItem(VcpListMenu *listMenu, VfxU32 selectIdx)
{
	vfxPostInvoke4(this, &VappMenuPage::postHandler, getMainScr()->getScreenId(), (int16)MR_MENU_SELECT, (int32)selectIdx, 0);
}

VfxBool VappMenuPage::getItemText(VfxU32 index, VcpListMenuFieldEnum fieldType, VfxWString &text, VcpListMenuTextColorEnum &color)
{
	if (index >= getCount())
		return VFX_FALSE;   

	if (fieldType == VCP_LIST_MENU_FIELD_TEXT)
	{
		color = VCP_LIST_MENU_TEXT_COLOR_NORMAL;
		text = m_itemTexts[index];
	}

	return VFX_TRUE;
}

VcpListMenuItemStateEnum VappMenuPage::getItemState(VfxU32 index) const
{
	if (index < getCount())
	{
		if (m_itemSelects[index])
			return VCP_LIST_MENU_ITEM_STATE_SELECTED;
		else
			return VCP_LIST_MENU_ITEM_STATE_UNSELECTED;
	}

	return VCP_LIST_MENU_ITEM_STATE_UNSELECTED;
}

VfxBool VappMenuPage::onKeyInput(VfxKeyEvent &event)
{
	if (event.keyCode == VFX_KEY_CODE_BACK)
	{
		if (event.type == VFX_KEY_EVENT_TYPE_UP)
		{
			vfxPostInvoke4(this, &VappMenuPage::postHandler, getMainScr()->getScreenId(), (int16)MR_MENU_RETURN, 0, 0);	
		}
		return VFX_TRUE;
	}

	return VfxPage::onKeyInput(event);		
}

void VappMenuPage::refresh()
{
	m_listMenu->updateAllItems();	
}

void VappMenuPage::postHandler(mmi_id handle, int16 type ,int32 param1,int32 param2)
{
	m_handler(handle, type, param1, param2);
}


VFX_IMPLEMENT_CLASS("VappTextPage", VappTextPage, VfxPage);

VappTextPage::VappTextPage(const VfxWString& title, const VfxWString& text, VfxS32 type, mr_localui_event_handler handler) : m_title(title), m_text(text), m_type(type), m_handler(handler)
{
	
}

void VappTextPage::onInit()
{
	VfxPage::onInit();

	VFX_OBJ_CREATE(m_titleBar, VcpTitleBar, this);
	m_titleBar->setTitle(m_title);
	setTopBar(m_titleBar);  

	VFX_OBJ_CREATE(m_toolBar, VcpToolBar, this);
	
	if (m_type == MR_DIALOG_OK)
	{
		m_toolBar->addItem(0, VFX_WSTR_RES(STR_GLOBAL_OK), VCP_IMG_TOOL_BAR_COMMON_ITEM_OK);
	}
	else if (m_type == MR_DIALOG_OK_CANCEL)
	{
		m_toolBar->addItem(0, VFX_WSTR_RES(STR_GLOBAL_OK), VCP_IMG_TOOL_BAR_COMMON_ITEM_OK);
		m_toolBar->addItem(1, VFX_WSTR_RES(STR_GLOBAL_BACK), VCP_IMG_TOOL_BAR_COMMON_ITEM_CANCEL);	
	}
	else if (m_type == MR_DIALOG_CANCEL)
	{
		m_toolBar->addItem(1, VFX_WSTR_RES(STR_GLOBAL_BACK), VCP_IMG_TOOL_BAR_COMMON_ITEM_CANCEL);	
	}
	
	m_toolBar->m_signalButtonTap.connect(this, &VappTextPage::onToolBarTap);
	setBottomBar(m_toolBar);

	VFX_OBJ_CREATE(m_textView, VcpTextView, this);
	m_textView->setText(m_text);
	m_textView->setPos(0, 0);
	m_textView->setLineMode(VCP_TEXT_LINE_MODE_MULTI);
	m_textView->setSize(getSize());
	m_textView->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);
	m_textView->setMargins(10, 10, 10, 10);
}

VfxBool VappTextPage::onKeyInput(VfxKeyEvent &event)
{
	if (event.keyCode == VFX_KEY_CODE_BACK)
	{
		if (event.type == VFX_KEY_EVENT_TYPE_UP)
		{
			if (m_type == MR_DIALOG_OK)
			{
				vfxPostInvoke4(this, &VappTextPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_OK, 0);
			}
			else
			{
				vfxPostInvoke4(this, &VappTextPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_CANCEL, 0);	
			}
		}
		return VFX_TRUE;
	}

	return VfxPage::onKeyInput(event);	
}

void VappTextPage::onToolBarTap(VfxObject* sender, VfxId senderId)
{
	if (senderId == 0)
	{
		vfxPostInvoke4(this, &VappTextPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_OK, 0);
	}
	else if (senderId == 1)
	{
		vfxPostInvoke4(this, &VappTextPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_CANCEL, 0);
	}
}

void VappTextPage::refresh(const VfxWString& title, const VfxWString& text, VfxS32 type)
{
	m_title = title;
	m_text = text;
	m_type = type;

	m_titleBar->setTitle(m_title);

	m_toolBar->removeItem(0);
	m_toolBar->removeItem(1);
	
	if (m_type == MR_DIALOG_OK)
	{
		m_toolBar->addItem(0, VFX_WSTR_RES(STR_GLOBAL_OK), VCP_IMG_TOOL_BAR_COMMON_ITEM_OK);
	}
	else if (m_type == MR_DIALOG_OK_CANCEL)
	{
		m_toolBar->addItem(0, VFX_WSTR_RES(STR_GLOBAL_OK), VCP_IMG_TOOL_BAR_COMMON_ITEM_OK);
		m_toolBar->addItem(1, VFX_WSTR_RES(STR_GLOBAL_BACK), VCP_IMG_TOOL_BAR_COMMON_ITEM_CANCEL);	
	}
	else if (m_type == MR_DIALOG_CANCEL)
	{
		m_toolBar->addItem(1, VFX_WSTR_RES(STR_GLOBAL_BACK), VCP_IMG_TOOL_BAR_COMMON_ITEM_CANCEL);	
	}

	m_textView->setText(m_text);
}

void VappTextPage::postHandler(mmi_id handle, int16 type ,int32 param1,int32 param2)
{
	m_handler(handle, type, param1, param2);
}


VFX_IMPLEMENT_CLASS("VappEditPage", VappEditPage, VfxPage);

VappEditPage::VappEditPage(const VfxWString& title, const VfxWString& text, VfxS32 type, VfxS32 max_size, mr_localui_event_handler handler) : m_title(title), m_text(text), m_type(type), m_maxSize(max_size), m_handler(handler)
{
	
}

void VappEditPage::onInit()
{
	VfxPage::onInit();

	VFX_OBJ_CREATE(m_titleBar, VcpTitleBar, this);
	m_titleBar->setTitle(m_title);
	setTopBar(m_titleBar);  

	VFX_OBJ_CREATE(m_toolBar, VcpToolBar, this);
	
	m_toolBar->addItem(0, VFX_WSTR_RES(STR_GLOBAL_OK), VCP_IMG_TOOL_BAR_COMMON_ITEM_OK);
	m_toolBar->addItem(1, VFX_WSTR_RES(STR_GLOBAL_BACK), VCP_IMG_TOOL_BAR_COMMON_ITEM_CANCEL);	
	
	m_toolBar->m_signalButtonTap.connect(this, &VappEditPage::onToolBarTap);
	setBottomBar(m_toolBar);

	VFX_OBJ_CREATE(m_textEditor, VcpTextEditor, this);
	m_textEditor->setText(m_text, m_maxSize);
	m_textEditor->setPos(0, 0);
	m_textEditor->setSize(getSize());
	m_textEditor->setLineMode(VCP_TEXT_LINE_MODE_MULTI);
	m_textEditor->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);
	m_textEditor->setMargins(10, 10, 10, 10);
	m_textEditor->activate();

	VfxU32 input_type;
	if (m_type == MR_EDIT_ANY)
	{
		input_type = INPUT_TYPE_ALPHANUMERIC_SENTENCECASE;
	}
	else if (m_type == MR_EDIT_NUMERIC)
	{
		input_type = INPUT_TYPE_PHONE_NUMBER;
	}
	else if (m_type == MR_EDIT_PASSWORD)
	{
		input_type = INPUT_TYPE_ALPHANUMERIC_PASSWORD|INPUT_TYPE_USE_ONLY_ENGLISH_MODES;
	}
	else if (m_type == MR_EDIT_ALPHA)
	{
		input_type = IMM_INPUT_TYPE_ENGLISH_SENTENCE|INPUT_TYPE_USE_ONLY_ENGLISH_MODES;
	}
	else
	{
		input_type = INPUT_TYPE_ALPHANUMERIC_SENTENCECASE;
	}
	m_textEditor->setIME(input_type);
	//m_textEditor->activate();//huangsunbo,进入编辑框，自动弹出虚拟键盘
}

VfxBool VappEditPage::onKeyInput(VfxKeyEvent &event)
{
	if (event.keyCode == VFX_KEY_CODE_BACK)
	{
		if (event.type == VFX_KEY_EVENT_TYPE_UP)
		{
			vfxPostInvoke4(this, &VappEditPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_CANCEL, 0);	
		}
		return VFX_TRUE;
	}

	return VfxPage::onKeyInput(event);
}

void VappEditPage::onToolBarTap(VfxObject* sender, VfxId senderId)
{
	if (senderId == 0)
	{
		vfxPostInvoke4(this, &VappEditPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_OK, 0);
	}
	else if (senderId == 1)
	{
		vfxPostInvoke4(this, &VappEditPage::postHandler, getMainScr()->getScreenId(), (int16)MR_DIALOG_EVENT, (int32)MR_DIALOG_KEY_CANCEL, 0);
	}
}

VfxWString VappEditPage::getText()
{
	return VFX_WSTR_MEM(m_textEditor->getText());
}

void VappEditPage::postHandler(mmi_id handle, int16 type ,int32 param1,int32 param2)
{
	m_handler(handle, type, param1, param2);
}


VFX_IMPLEMENT_CLASS("VappWinPage", VappWinPage, VfxPage);

VappWinPage::VappWinPage(mr_localui_event_handler handler) : m_handler(handler)
{
	
}

void VappWinPage::onInit()
{
	VfxPage::onInit();
	setStatusBar(VFX_FALSE);
	setLayerHandle(GDI_LAYER_MAIN_BASE_LAYER_HANDLE);
}

VfxBool VappWinPage::onKeyInput(VfxKeyEvent& event)
{
	if (event.keyCode == VFX_KEY_CODE_BACK)
	{
		if (event.type == VFX_KEY_EVENT_TYPE_UP)
		{
			vfxPostInvoke4(this, &VappWinPage::postHandler, getMainScr()->getScreenId(), (int16)MR_KEY_RELEASE, (int32)MR_KEY_RIGHT, 0);
		}
		else if (event.type == VFX_KEY_EVENT_TYPE_DOWN)
		{
			vfxPostInvoke4(this, &VappWinPage::postHandler, getMainScr()->getScreenId(), (int16)MR_KEY_PRESS, (int32)MR_KEY_RIGHT, 0);	
		}
		return VFX_TRUE;
	}

	return VfxPage::onKeyInput(event);
}

VfxBool VappWinPage::onPenInput(VfxPenEvent& event)
{
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	mr_layer_zoom_convert_touch_position((int16 *)&event.pos.x, (int16 *)&event.pos.y);
#endif
	if (event.type == VFX_PEN_EVENT_TYPE_DOWN)
	{
		vfxPostInvoke4(this, &VappWinPage::postHandler, getMainScr()->getScreenId(), (int16)MR_MOUSE_DOWN, event.pos.x, event.pos.y);
	}
	else if (event.type == VFX_PEN_EVENT_TYPE_UP)
	{
		vfxPostInvoke4(this, &VappWinPage::postHandler, getMainScr()->getScreenId(), (int16)MR_MOUSE_UP, event.pos.x, event.pos.y);
	}
	else if (event.type == VFX_PEN_EVENT_TYPE_MOVE)
	{
		vfxPostInvoke4(this, &VappWinPage::postHandler, getMainScr()->getScreenId(), (int16)MR_MOUSE_MOVE, event.pos.x, event.pos.y);
	}

	return VFX_TRUE;
}

void VappWinPage::postHandler(mmi_id handle, int16 type ,int32 param1,int32 param2)
{
	m_handler(handle, type, param1, param2);
}

void dsm_set_title(U16 **pTitle)
{
	// 不需要了
}

void dsmFreeScrIdAll(void)
{
	// 不需要了
}

void mr_localui_free_all_screen(void)
{
	VappLocalUIApp* app;

	app = mr_localui_get_app();
	if (app != NULL)
		VfxAppLauncher::terminate(app->getGroupId());
}

int32 mr_localui_initialize(void)
{
	// 不需要了	
	return MR_TRUE;
}

int32 mr_localui_terminate(void)
{
	// 不需要了
	return MR_TRUE;
}

static int32 localui_event_handler(int32 handle, int16 type ,int32 param1,int32 param2)
{
	return mr_app_send_event_wrapper(type, param1, param2);
}

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
extern "C" U8 current_screen_rotation;
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
static void localui_try_rotate(VappLocalUIApp *app)
{
	if (app != NULL && app->getScreenCount() == 0)
	{
	#if defined(__MMI_SCREEN_ROTATE__)
		if (current_screen_rotation != MMI_FRM_SCREEN_ROTATE_0)
		{
			mmi_frm_screen_rotate((mmi_frm_screen_rotate_enum)current_screen_rotation);
			if (mr_layer_zoom_is_enabled())
			{
				gdi_layer_resize(mr_layer_zoom_get_width(), mr_layer_zoom_get_height());
			}
		}
	#endif
	}
}
#endif

int32 mr_winCreate(void)
{
	mr_trace("mr_winCreate");

	int32 ret = mr_winCreate_ex(localui_event_handler);
		
	mr_trace("mr_winCreate:ret=%d", ret);	
	
	return ret;
}

int32 mr_winRelease(int32 win)
{
	mr_trace("mr_winRelease:win=%d", win);

	VappLocalUIApp* app;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	app->closeScreen(win);

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	localui_try_rotate(app);
#endif

	return MR_SUCCESS;
}


int32 mr_editCreate(const char* title, const char* text, int32 type, int32 max_size)
{
	mr_trace("mr_editCreate:type=%d,max_size=%d", type, max_size);

	int32 ret = mr_editCreate_ex(title, text, type, max_size, localui_event_handler);

	mr_trace("mr_editCreate:ret=%d", ret);	
	
	return ret;
}

const char* mr_editGetText(int32 edit)
{
	mr_trace("mr_editGetText:edit=%d", edit);

	VappLocalUIScreen* screen;
	VappLocalUIApp* app;
	VappEditPage* page;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	screen = (VappLocalUIScreen*)app->getScreen(edit);
	page = VFX_OBJ_DYNAMIC_CAST(screen->getPage(screen->getTopPageId()), VappEditPage);
	if (page == NULL)
		return MR_SUCCESS;

	page->m_text = page->getText();

#ifndef WIN32	// 方便模拟器调试	
	mr_str_convert_endian((char*)page->m_text.getBuf());
#endif
	
	return (const char*)page->m_text.getBuf();
}

int32 mr_editRelease(int32 edit)
{
	mr_trace("mr_editRelease:edit=%d", edit);

	VappLocalUIApp* app;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	app->closeScreen(edit);

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	localui_try_rotate(app);
#endif
	
	return MR_SUCCESS;
}


int32 mr_textCreate(const char * title, const char * text, int32 type)
{
	mr_trace("mr_textCreate:type=%d", type);

	int32 ret = mr_textCreate_ex(title, text, type, localui_event_handler);

	mr_trace("mr_textCreate:ret=%d", ret);	
	
	return ret;
}

int32 mr_textRefresh(int32 textid, const char * title, const char * text)
{
	mr_trace("mr_textRefresh:textid=%d", textid);

	VappLocalUIScreen* screen;
	VappLocalUIApp* app;
	VappTextPage* page;
	VfxWString tmp1(VFX_WSTR_DYNAMIC((const VfxWChar*)title));
	VfxWString tmp2(VFX_WSTR_DYNAMIC((const VfxWChar*)text));	

#ifndef WIN32	// 方便模拟器调试
	mr_str_convert_endian((char*)tmp1.getBuf());
	mr_str_convert_endian((char*)tmp2.getBuf());
#endif	

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;
		
	screen = (VappLocalUIScreen*)app->getScreen(textid);
	if (screen == NULL)
		return MR_SUCCESS;

	page = VFX_OBJ_DYNAMIC_CAST(screen->getPage(screen->getTopPageId()), VappTextPage);
	if (page == NULL)
		return MR_SUCCESS;

	page->refresh(tmp1, tmp2, page->m_type);

	screen->checkUpdate();
	
	return MR_SUCCESS;
}

int32 mr_textRelease(int32 text)
{
	mr_trace("mr_textRelease:text=%d", text);

	VappLocalUIApp* app;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	app->closeScreen(text);

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	localui_try_rotate(app);
#endif
	
	return MR_SUCCESS;
}


int32 mr_dialogCreate(const char * title, const char * text, int32 type)
{
	mr_trace("mr_dialogCreate:type=%d", type);

	int32 ret = mr_dialogCreate_ex(title, text, type, localui_event_handler);

	mr_trace("mr_dialogCreate:ret=%d", ret);	
	
	return ret;
}

int32 mr_dialogRefresh(int32 dialog, const char * title, const char * text, int32 type)
{
	mr_trace("mr_dialogRefresh:dialog=%d,type=%d", dialog, type);

	VappLocalUIScreen* screen;
	VappLocalUIApp* app;
	VappTextPage* page;
	VfxWString tmp1(VFX_WSTR_DYNAMIC((const VfxWChar*)title));
	VfxWString tmp2(VFX_WSTR_DYNAMIC((const VfxWChar*)text));	

#ifndef WIN32	// 方便模拟器调试
	mr_str_convert_endian((char*)tmp1.getBuf());
	mr_str_convert_endian((char*)tmp2.getBuf());
#endif	

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	screen = (VappLocalUIScreen*)app->getScreen(dialog);
	if (screen == NULL)	// 避免释放窗口后在刷新的情况
		return MR_SUCCESS;

	page = VFX_OBJ_DYNAMIC_CAST(screen->getPage(screen->getTopPageId()), VappTextPage);
	if (page == NULL)
		return MR_SUCCESS;	

	page->refresh(tmp1, tmp2, type);

	screen->checkUpdate();
	
	return MR_SUCCESS;

}

int32 mr_dialogRelease(int32 dialog)
{
	mr_trace("mr_dialogRelease:dialog=%d", dialog);

	VappLocalUIApp* app;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	app->closeScreen(dialog);

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	localui_try_rotate(app);
#endif
	
	return MR_SUCCESS;	
}

int32 mr_menuSetItemEx(int32 menu, const char* text, int32 index, int32 select)
{
	VappLocalUIScreen* screen;
	VappLocalUIApp* app;
	VappMenuPage* page;
	VfxWString tmp(VFX_WSTR_DYNAMIC((const VfxWChar*)text));

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	screen = (VappLocalUIScreen*)app->getScreen(menu);
	if (screen == NULL)
		return MR_SUCCESS;

	page = VFX_OBJ_DYNAMIC_CAST(screen->getPage(screen->getTopPageId()), VappMenuPage);
	if (page == NULL)
		return MR_SUCCESS;

	if (index >= (int32)page->m_itemCount)
		return MR_FAILED;

#ifndef WIN32	// 方便模拟器调试
	mr_str_convert_endian((char*)tmp.getBuf());
#endif
	page->m_itemTexts[index] = tmp;
	page->m_itemSelects[index] = select;
	
	return MR_SUCCESS;
}

int32 mr_menuCreate(const char * title, int16 num)
{
	mr_trace("mr_menuCreate:num=%d", num);
	
	int32 ret = mr_menuCreate_ex(title, num, MR_LIST_NORMAL_ICON, localui_event_handler);

	mr_trace("mr_menuCreate:ret=%d", ret);
	
	return ret;
}

int32 mr_menuSetItem(int32 menu, const char *text, int32 index)
{
	mr_trace("mr_menuSetItem:menu=%d,index=%d", menu, index);

	return mr_menuSetItemEx(menu, text, index, 0);
}

int32 mr_menuShow(int32 menu)
{
	mr_trace("mr_menuShow:menu=%d", menu);

	VappLocalUIScreen* screen;
	VappLocalUIApp* app;
	VappMenuPage* page;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	screen = (VappLocalUIScreen*)app->getScreen(menu);
	if (screen == NULL)
		return MR_SUCCESS;

	page = VFX_OBJ_DYNAMIC_CAST(screen->getPage(screen->getTopPageId()), VappMenuPage);
	page->refresh();
	
	screen->checkUpdate();
	
	return MR_SUCCESS;
}

int32 mr_menuRefresh(int32 menu)
{
	mr_trace("mr_menuRefresh:menu=%d", menu);

	VappLocalUIScreen* screen;
	VappLocalUIApp* app;
	VappMenuPage* page;
	
	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;

	screen = (VappLocalUIScreen*)app->getScreen(menu);
	if (screen == NULL)
		return MR_SUCCESS;

	page = VFX_OBJ_DYNAMIC_CAST(screen->getPage(screen->getTopPageId()), VappMenuPage);
	page->refresh();
	
	screen->checkUpdate();
	
	return MR_SUCCESS;
}

int32 mr_menuRelease(int32 menu)
{
	mr_trace("mr_menuRelease:menu=%d", menu);

	VappLocalUIApp* app;

	app = mr_localui_get_app();
	if (app == NULL)
		return MR_SUCCESS;
	
	app->closeScreen(menu);

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	localui_try_rotate(app);
#endif
	
	return MR_SUCCESS;
}


int32 mr_localui_menu_create_ex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_trace("mr_localui_menu_create_ex");

	T_LISTBOXWIN* listwin = (T_LISTBOXWIN*)input;
	return mr_menuCreate_ex(listwin->title, listwin->itemNum, listwin->listType, localui_event_handler);
}

int32 mr_localui_menu_set_menuitem_ex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_trace("mr_localui_menu_set_menuitem_ex");

	T_LISTBOXITEM* listitem = (T_LISTBOXITEM*)input;
	return mr_menuSetItemEx(listitem->menu, listitem->text, listitem->index, listitem->selected);
}

int32 mr_localui_menu_set_focus(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_trace("mr_localui_menu_set_focus");

	//COSMOS是全触摸不需要设置
	return MR_SUCCESS;
}

int32 mr_menuCreate_ex(const char * title, int16 num,int16 type, mr_localui_event_handler handler)
{
	VappMenuPage* page;
	VfxWString tmp(VFX_WSTR_DYNAMIC((const VfxWChar*)title));
	int32 ret;
	VappLocalUIScreen* screen;
	VappLocalUIApp* app;

	app = mr_localui_get_app_instance();
#ifndef WIN32	// 方便模拟器调试	
	mr_str_convert_endian((char*)tmp.getBuf());
#endif

	VFX_OBJ_CREATE(screen, VappLocalUIScreen, app);
	VFX_OBJ_CREATE_EX(page, VappMenuPage, screen, (tmp, num, type, handler));
	
	screen->pushPage(VFX_ID_NULL, page);
	ret = screen->show();
	
	return ret;	
}

int32 mr_dialogCreate_ex(const char * title, const char * text, int32 type, mr_localui_event_handler handler)
{
	VappTextPage* page;
	VfxWString tmp1(VfxWStringConstHelper((const VfxWChar*)L""));	//VfxWString tmp1(VFX_WSTR_DYNAMIC((const VfxWChar*)title));
	VfxWString tmp2(VFX_WSTR_DYNAMIC((const VfxWChar*)text));
	int32 ret;
	VappLocalUIScreen* screen;
	VappLocalUIApp* app;

	app = mr_localui_get_app_instance();
	
#ifndef WIN32	// 方便模拟器调试
	mr_str_convert_endian((char*)tmp1.getBuf());
	mr_str_convert_endian((char*)tmp2.getBuf());
#endif	

	VFX_OBJ_CREATE(screen, VappLocalUIScreen, app);
	VFX_OBJ_CREATE_EX(page, VappTextPage, screen, (tmp1, tmp2, type, handler));

	screen->pushPage(VFX_ID_NULL, page);
	ret = screen->show();
	
	return ret;
}

int32 mr_textCreate_ex(const char * title, const char * text, int32 type, mr_localui_event_handler handler)
{
	VappTextPage* page;
	VfxWString tmp1(VFX_WSTR_DYNAMIC((const VfxWChar*)title));
	VfxWString tmp2(VFX_WSTR_DYNAMIC((const VfxWChar*)text));
	int32 ret;
	VappLocalUIScreen* screen;
	VappLocalUIApp* app;

	app = mr_localui_get_app_instance();
	
#ifndef WIN32	// 方便模拟器调试
	mr_str_convert_endian((char*)tmp1.getBuf());
	mr_str_convert_endian((char*)tmp2.getBuf());
#endif	

	VFX_OBJ_CREATE(screen, VappLocalUIScreen, app);
	VFX_OBJ_CREATE_EX(page, VappTextPage, screen, (tmp1, tmp2, type, handler));

	screen->pushPage(VFX_ID_NULL, page);
	ret = screen->show();
	
	return ret;
}

int32 mr_editCreate_ex(const char * title, const char * text, int32 type, int32 max_size, mr_localui_event_handler handler)
{
	VappEditPage* page;
	VfxWString tmp1(VFX_WSTR_DYNAMIC((const VfxWChar*)title));
	VfxWString tmp2(VFX_WSTR_DYNAMIC((const VfxWChar*)text));
	int32 ret;
	VappLocalUIScreen* screen;
	VappLocalUIApp* app;

	app = mr_localui_get_app_instance();
	
#ifndef WIN32	// 方便模拟器调试
	mr_str_convert_endian((char*)tmp1.getBuf());
	mr_str_convert_endian((char*)tmp2.getBuf());
#endif	

	VFX_OBJ_CREATE(screen, VappLocalUIScreen, app);
	VFX_OBJ_CREATE_EX(page, VappEditPage, screen, (tmp1, tmp2, type, max_size, handler));

	screen->pushPage(VFX_ID_NULL, page);
	ret = screen->show();
	
	return ret;
}

int32 mr_winCreate_ex(mr_localui_event_handler handler)
{
	VappWinPage* page;
	int32 ret;
	VappLocalUIScreen* screen;
	VappLocalUIApp* app;

	app = mr_localui_get_app_instance();
	
	VFX_OBJ_CREATE(screen, VappLocalUIScreen, app);
	VFX_OBJ_CREATE_EX(page, VappWinPage, screen, (handler));

	screen->pushPage(VFX_ID_NULL, page);
	ret = screen->show();	
	
	return ret;
}

#endif

