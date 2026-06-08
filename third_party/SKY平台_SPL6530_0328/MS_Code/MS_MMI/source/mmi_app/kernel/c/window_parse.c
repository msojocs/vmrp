/****************************************************************************
** File Name:      gui_listbox.c                                           *
** Author:         Great.Tian                                              *
** Date:           03/23/2004                                              *
** Copyright:      2003 Spreadtrum, Incorporated. All Rights Reserved.       *
** Description:    This file is used to describe the listbox control.      *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2004        Great.Tian       Create
** 
****************************************************************************/
#define WINDOW_PARSE_C

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
//#include "std_header.h"
#include "mmi_app_kernel_trc.h"
#include "window_parse.h"
#include "mmk_app.h" 
#include "mmi_port.h"
#include "mmipub.h"
#include "guicommon.h"
#include "guimenu.h"
#include "guitext.h"
#include "guimsgbox.h"
#include "guiprgbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "mmi_menutable.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guidropdownlist.h"
#include "guiiconlist.h"
#include "guiiconfolder.h"
#include "guirichtext.h"
#include "guianim.h"
#include "guistatusbar.h"
#include "mmidisplay_data.h"
#include "guitab.h"
#include "guisoftkey.h"
#include "guiform.h"
#include "guiownerdraw.h"
#include "guitoolbar.h"
#include "guiwin.h"
#include "cafdc.h"
#include "cafctrllist.h"
#include "cafctrltextbox.h"
#include "guitips.h"
#include "guisetlist.h"
#include "cafcontrol.h"
#include "mmi_default.h"
#ifdef GUI_CTRL_SCROLL_KEY
#include "guiscrollkey.h"
#endif
#ifdef MMI_PDA_SUPPORT
//#include "mmiset.h"
#include "mmiset_export.h"
#endif
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         TYPE AND CONSTANT                                 */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DEFINITION                              *
 **--------------------------------------------------------------------------*/
extern const MMK_KEYWORD_INFO_T FormChildCtrlParseInfo[];

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
// 	Description : the function , set window priority
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinPrio(
                         MMI_HANDLE_T win_handle,
                         uint32**      value_pptr
                         );

/*****************************************************************************/
// 	Description : the function , set window identify
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinId(
                       MMI_HANDLE_T win_handle,
                       uint32**      value_pptr
                       );

/*****************************************************************************/
// 	Description : the function , set window identify
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinFunc(
                         MMI_HANDLE_T win_handle,
                         uint32**      value_pptr
                         );

/*****************************************************************************/
// 	Description : create the menu control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateMenuCtrl(
                             MMI_HANDLE_T   win_handle,
                             uint32         **value_pptr
                             );

/*****************************************************************************/
// 	Description : create the pop_up menu control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreatePopmenuCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );

/*****************************************************************************/
// 	Description : create the message box control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateMsgBoxCtrl(
                               MMI_HANDLE_T     win_handle,
                               uint32           **value_pptr
                               );

/*****************************************************************************/
// 	Description : create the progress box control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreatePrgBoxCtrl(
                               MMI_HANDLE_T     win_handle,
                               uint32           **value_pptr
                               );

/*****************************************************************************/
// 	Description : create the text control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTextCtrl(
                             MMI_HANDLE_T   win_handle,
                             uint32         **value_pptr
                             );

/*****************************************************************************/
// 	Description : create the list box control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateListBoxCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );

/*****************************************************************************/
// 	Description : create the edit box text control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTextCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the edit box phone number control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditPhonenumCtrl(
                                     MMI_HANDLE_T   win_handle,
                                     uint32         **value_pptr
                                     );

/*****************************************************************************/
// 	Description : create the edit box digital control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditDigitalCtrl(
                                    MMI_HANDLE_T    win_handle,
                                    uint32          **value_pptr
                                    );

/*****************************************************************************/
// 	Description : create the edit box password control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditPasswordCtrl(
                                     MMI_HANDLE_T   win_handle,
                                     uint32         **value_pptr
                                     );

/*****************************************************************************/
// 	Description : create the edit box list control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditListCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the edit box date control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditDateCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the edit box time control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTimeCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the edit box touch date control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTouchDateCtrl(
                                      MMI_HANDLE_T   win_handle,
                                      uint32         **value_pptr
                                      );

/*****************************************************************************/
// 	Description : create the edit box touch time control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTouchTimeCtrl(
                                      MMI_HANDLE_T   win_handle,
                                      uint32         **value_pptr
                                      );

/*****************************************************************************/
// 	Description : create the edit box picker control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditPickerCtrl(
                                   MMI_HANDLE_T   win_handle,
                                   uint32         **value_pptr
                                   );

/*****************************************************************************/
// 	Description : create the edit box IP control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditIPCtrl(
                               MMI_HANDLE_T     win_handle,
                               uint32           **value_pptr
                               );

/*****************************************************************************/
// 	Description : create the animation control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateAnimCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**       value_pptr
                             );

/*****************************************************************************/
// 	Description : set the window background image id
//	Global resource dependence : 
//  Author: Tracy Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinBackgroundID(
                                 MMI_HANDLE_T win_handle,
                                 uint32**      value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the label control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateLabelCtrl(
                              MMI_HANDLE_T  win_handle,
                              uint32        **value_pptr
                              );

/*****************************************************************************/
// 	Description : create the button control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateButtonCtrl(
                               MMI_HANDLE_T  win_handle,
                               uint32        **value_pptr
                               );

/*****************************************************************************/
// 	Description : create the dropdown list control
//	Global resource dependence : 
//  Author:YELIANNA
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateDropDownListCtrl(
                                     MMI_HANDLE_T   win_handle,
                                     uint32         **value_pptr
                                     );

/*****************************************************************************/
// 	Description : create the icon list control
//	Global resource dependence : 
//  Author: jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateIconListCtrl(
                                 MMI_HANDLE_T win_handle,
                                 uint32         **value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the icon folder control
//	Global resource dependence : 
//  Author: jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateIconFolderCtrl(
                                   MMI_HANDLE_T win_handle,
                                   uint32       **value_pptr
                                   );

/*****************************************************************************/
// 	Description : create the richtext control
//	Global resource dependence : 
//  Author: liqing.peng
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateRichTextCtrl( 
                                 MMI_HANDLE_T win_handle,
                                 uint32**      	value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the tab control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTabCtrl(
                            MMI_HANDLE_T    win_handle,
                            uint32          **value_pptr
                            );

/*****************************************************************************/
// Description : create the statusbar control
// Global resource dependence : 
// Author:Ming.Song
// Note:2007-7-31
/*****************************************************************************/
LOCAL BOOLEAN CreateStatusBarCtrl(
                                  MMI_HANDLE_T win_handle,
                                  uint32**      value_pptr
                                  );

/*****************************************************************************/
// 	Description : set status bar style
//	Global resource dependence : 
//  Author: hua.fang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetStatusBarStyle(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );

/*****************************************************************************/
// Description : set no status bar ctrl
// Global resource dependence : 
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN SetHideStatusBarStyle(
                                    MMI_HANDLE_T    win_handle,
                                    uint32**        value_pptr
                                    );

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinStyle(
                          MMI_HANDLE_T win_handle,
                          uint32**      value_pptr
                          );

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinMoveStyle(
                              MMI_HANDLE_T win_handle,
                              uint32**      value_pptr
                              );

/*****************************************************************************/
// 	Description : set the window support angle
//	Global resource dependence : 
//  Author: James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinSupportAngle(
                                 MMI_HANDLE_T win_handle,
                                 uint32**      value_pptr
                                 );

#ifdef GUI_CTRL_SCROLL_KEY
/*****************************************************************************/
// 	Description : create scroll key control
//	Global resource dependence : 
//  Author: Cheney Wu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateScrollKeyCtrl(
                                 MMI_HANDLE_T win_handle,
                                 uint32**      value_pptr
                                 );
#endif

/*****************************************************************************/
// 	Description : create the listbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateListBoxCtrl(
                                   MMI_HANDLE_T win_handle,
                                   uint32**      value_pptr
                                   );

/*****************************************************************************/
// 	Description : create the text editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditTextCtrl(
                                       MMI_HANDLE_T win_handle,
                                       uint32**      value_pptr
                                       );

/*****************************************************************************/
// 	Description : create the label control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateLabelCtrl(
                                 MMI_HANDLE_T  win_handle,
                                 uint32**      value_pptr
                                 );

/*****************************************************************************/
// 	Description : create the textbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateTextBoxCtrl(
                                   MMI_HANDLE_T win_handle,
                                   uint32**      value_pptr
                                   );

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateSoftkeyCtrl(
							  MMI_HANDLE_T win_handle,
							  uint32**      value_pptr
							  );

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetSoftkeyStyle(
							  MMI_HANDLE_T win_handle,
							  uint32**      value_pptr
							  );

/*****************************************************************************/
// Description : create the tips control
// Global resource dependence : 
// Author:
// Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTipsCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**      value_pptr
                             );

/*****************************************************************************/
// 	Description : create the setting list control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateSettingList(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );

/*****************************************************************************/
// 	Description : create the form control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateFormCtrl(
                             MMI_HANDLE_T   win_handle,
                             uint32         **value_pptr
                             );

/*****************************************************************************/
// 	Description : create the owner draw control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateOwnDrawCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );

/*****************************************************************************/
// 	Description : create the toolbar control
//	Global resource dependence : 
//  Author: James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateToolbarCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTitleCtrl(
							  MMI_HANDLE_T win_handle,
							  uint32**      value_pptr
							  );

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetTitleStyle(
                            MMI_HANDLE_T win_handle,
                            uint32**      value_pptr
                            );

//added by andrew.zhang at 2009/03/24
LOCAL BOOLEAN CAFCreateEditPhonenumCtrl(
                                        MMI_HANDLE_T win_handle,
                                        uint32**      value_pptr
                                        );
LOCAL BOOLEAN CAFCreateEditDigitalCtrl(
                                       MMI_HANDLE_T win_handle,
                                       uint32**      value_pptr
                                       );
LOCAL BOOLEAN CAFCreateEditPasswordCtrl(
                                        MMI_HANDLE_T win_handle,
                                        uint32**      value_pptr
                                        );
LOCAL BOOLEAN CAFCreateEditDateCtrl(
                                    MMI_HANDLE_T win_handle,
                                    uint32**      value_pptr
                                    );
LOCAL BOOLEAN CAFCreateEditTimeCtrl(
                                    MMI_HANDLE_T win_handle,
                                    uint32**      value_pptr
                                    );
LOCAL BOOLEAN CAFCreateAnimCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**     value_pptr
                             );
LOCAL BOOLEAN CAFCreateButtonCtrl( MMI_HANDLE_T win_handle,   uint32**      value_pptr  );
LOCAL BOOLEAN CAFCreateDropDownListCtrl( MMI_HANDLE_T win_handle,   uint32**      value_pptr  );
LOCAL BOOLEAN CAFCreateIconListCtrl(
                                 MMI_HANDLE_T win_handle,
                                 uint32       **value_pptr
                                 );

LOCAL BOOLEAN CAFCreateMenuCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**      value_pptr
                             );

LOCAL BOOLEAN CAFCreatePopmenuCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                );
//end

#ifdef MMIPUBWIN_PDASTYLE_SUPPORT
/*****************************************************************************/
// 	Description : dummy0
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse0(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : dummy1
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse1(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : dummy2
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse2(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : dummy3
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse3(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : dummy4
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse4(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : dummy5
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse5(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : dummy6
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse6(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : set pub win func
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinFunc(
                            uint32    param,
                            uint32**  value_pptr
                            );

/*****************************************************************************/
// 	Description : set pub win id
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinId(
                          uint32    param,
                          uint32**  value_pptr
                          );

/*****************************************************************************/
// 	Description : set pub win list
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinList(
                            uint32    param,
                            uint32**  value_pptr
                            );

/*****************************************************************************/
// 	Description : set pub win label
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinLabel(
                             uint32    param,
                             uint32**  value_pptr
                             );

/*****************************************************************************/
// 	Description : set pub win edit text
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditText(
                                uint32    param,
                                uint32**  value_pptr
                                );

/*****************************************************************************/
// 	Description : set pub win edit phonenum
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditPhonenum(
                                    uint32    param,
                                    uint32**  value_pptr
                                    );

/*****************************************************************************/
// 	Description : set pub win edit digital
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditDigital(
                                    uint32    param,
                                    uint32**  value_pptr
                                    );

/*****************************************************************************/
// 	Description : set pub win edit password
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditPassword(
                                    uint32    param,
                                    uint32**  value_pptr
                                    );

/*****************************************************************************/
// 	Description : set pub win edit list
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditList(
                                uint32    param,
                                uint32**  value_pptr
                                );

/*****************************************************************************/
// 	Description : set pub win edit date
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditDate(
                                uint32    param,
                                uint32**  value_pptr
                                );

/*****************************************************************************/
// 	Description : set pub win edit date
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditTime(
                                uint32    param,
                                uint32**  value_pptr
                                );

/*****************************************************************************/
// 	Description : set pub win edit touch date
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditTouchDate(
                                     uint32    param,
                                     uint32**  value_pptr
                                     );

/*****************************************************************************/
// 	Description : set pub win edit touch time
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditTouchTime(
                                     uint32    param,
                                     uint32**  value_pptr
                                     );

/*****************************************************************************/
// 	Description : set pub win edit picker
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditPicker(
                                  uint32    param,
                                  uint32**  value_pptr
                                  );

/*****************************************************************************/
// 	Description : set pub win edit ip
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditIP(
                              uint32    param,
                              uint32**  value_pptr
                              );

/*****************************************************************************/
// 	Description : set pub win softkey
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinSoftkey(
                               uint32    param,
                               uint32**  value_pptr
                               );

/*****************************************************************************/
// 	Description : set pub win title
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinTitle(
                             uint32    param,
                             uint32**  value_pptr
                             );

/*****************************************************************************/
// 	Description : set pub win form
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinForm(
                            uint32    param,
                            uint32**  value_pptr
                            );
#endif

#ifdef GUISOFTKEY_PDASTYLE_SUPPORT
/*****************************************************************************/
// 	Description : 获取softkey状态
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
LOCAL void GetSoftkeyState(
                           MMI_HANDLE_T        win_handle,
                           uint32              leftsoft_id,
                           uint32              midsoft_id,
                           uint32              rightsoft_id,
                           GUISOFTKEY_BUTTONSTYLE_E  *softkey_style_ptr,
                           BOOLEAN             *is_visible_ptr
                           );
#endif

/*****************************************************************************/
// 	Description : preparse window tab
//	Global resource dependence : 
//  Author: Jasmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN PreParseWinTab(
                             MMI_HANDLE_T  win_handle,
                             uint32        *win_tab_ptr
                             );

/*****************************************************************************/
// 	Description : parse window tab
//	Global resource dependence : 
//  Author: Jasmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN ParseWinTab(
                          MMI_HANDLE_T  win_handle,
                          uint32        *win_tab_ptr
                          );

/*****************************************************************************/
// 	Description : dummy
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse(
                         uint32**  value_pptr,
                         uint32    num
                         );

/**--------------------------------------------------------------------------*
 **                         CONSTANT VARIABLES                               *
 **--------------------------------------------------------------------------*/
//parse window
//修改form parse时请一并修改PubWinFormParseFunc!
LOCAL const MMK_KEYWORD_INFO_T MMKParseInfo[] =
{
    { WIN_KEYWORD(MMK_WINPRIO,              SetWinPrio,             1) },
    { WIN_KEYWORD(MMK_WINFUNC,              SetWinFunc,             1) },
    { WIN_KEYWORD(MMK_WINID,                SetWinId,               1) },

    { WIN_KEYWORD(MMK_CREATE_MENU,          CreateMenuCtrl,         2) },
    { WIN_KEYWORD(MMK_CREATE_POPMENU,       CreatePopmenuCtrl,      2) },
    { WIN_KEYWORD(MMK_CREATE_MSGBOX,        CreateMsgBoxCtrl,       2) },
    { WIN_KEYWORD(MMK_CREATE_PRGBOX,        CreatePrgBoxCtrl,       2) },
    { WIN_KEYWORD(MMK_CREATE_TEXT,          CreateTextCtrl,         1) },
    { WIN_KEYWORD(MMK_CREATE_LISTBOX,       CreateListBoxCtrl,      2) },

    { WIN_KEYWORD(MMK_CREATE_EDIT_TEXT,     CreateEditTextCtrl,     2) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_PHONENUM, CreateEditPhonenumCtrl, 2) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_DIGITAL,  CreateEditDigitalCtrl,  2) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_PASSWORD, CreateEditPasswordCtrl, 2) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_LIST,     CreateEditListCtrl,     3) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_DATE,     CreateEditDateCtrl,     1) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_TIME,     CreateEditTimeCtrl,     1) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_TOUCH_DATE,  CreateEditTouchDateCtrl, 1) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_TOUCH_TIME,  CreateEditTouchTimeCtrl, 1) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_PICKER,   CreateEditPickerCtrl,   1) },
    { WIN_KEYWORD(MMK_CREATE_EDIT_IP,       CreateEditIPCtrl,       1) },

    { WIN_KEYWORD(MMK_CREATE_ANIM,          CreateAnimCtrl,         2) },
    { WIN_KEYWORD(MMK_WINBACKGROUNDID,      SetWinBackgroundID,     1) },
    { WIN_KEYWORD(MMK_CREATE_LABEL,         CreateLabelCtrl,        2) },
    { WIN_KEYWORD(MMK_CREATE_BUTTON,        CreateButtonCtrl,       2) },

    { WIN_KEYWORD(MMK_WINDOW_STYLE,         SetWinStyle,            1) },
    { WIN_KEYWORD(MMK_CREATE_DROPDOWNLIST,  CreateDropDownListCtrl, 3) },
    { WIN_KEYWORD(MMK_CREATE_ICONLIST,      CreateIconListCtrl,     1) },
    { WIN_KEYWORD(MMK_CREATE_RICHTEXT,      CreateRichTextCtrl,     1) },
    { WIN_KEYWORD(MMK_CREATE_TAB,           CreateTabCtrl,          3) },
    { WIN_KEYWORD(MMK_CREATE_STATUSBAR,     CreateStatusBarCtrl,    0) },
    { WIN_KEYWORD(MMK_CREATE_SOFTKEY,       CreateSoftkeyCtrl,      3) },
    { WIN_KEYWORD(MMK_CREATE_TITLE,         CreateTitleCtrl,        1) },
    { WIN_KEYWORD(MMK_WINDOW_ANIM_MOVE_SYTLE,  SetWinMoveStyle,     1) },
    { WIN_KEYWORD(MMK_CREATE_TIPS,          CreateTipsCtrl,         0) },
    { WIN_KEYWORD(MMK_CREATE_SETLIST,       CreateSettingList,      1) },

    { WIN_KEYWORD(MMK_CREATE_FORM,          CreateFormCtrl,         2) },
    { WIN_KEYWORD(MMK_CREATE_OWNDRAW,       CreateOwnDrawCtrl,      2) },
    { WIN_KEYWORD(MMK_CREATE_TOOLBAR,       CreateToolbarCtrl,      1) },
    { WIN_KEYWORD(MMK_CREATE_ICONFOLDER,    CreateIconFolderCtrl,   1) },
    { WIN_KEYWORD(MMK_HIDE_STATUSBAR,       SetHideStatusBarStyle,  0) },
    { WIN_KEYWORD(MMK_WINDOW_SUPPORT_ANGLE, SetWinSupportAngle,     1) },
#ifdef GUI_CTRL_SCROLL_KEY
    { WIN_KEYWORD(MMK_CREATE_SCROLLKEY,     CreateScrollKeyCtrl,    6) },
#endif
};

//caf window parse
//if caf has new interface, please implement function
LOCAL const MMK_KEYWORD_INFO_T CAFParseInfo[] =
{
    { WIN_KEYWORD(PNULL, PNULL,     0) },
    { WIN_KEYWORD(CAF_PARSEWIN_WINID, SetWinId, 1) },
    { WIN_KEYWORD(CAF_PARSEWIN_WINDOW_STYLE, SetWinStyle, 1) },
    { WIN_KEYWORD(CAF_PARSEWIN_WINTITLEID,  CreateTitleCtrl, 1) },
    { WIN_KEYWORD(CAF_PARSEWIN_WINBACKGROUNDID, SetWinBackgroundID, 1) },

    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_SOFTKEY, CreateSoftkeyCtrl, 3) },

    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_LISTBOX, CAFCreateListBoxCtrl, 6) },

    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_TEXTEDITBOX, CAFCreateEditTextCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_PHONENUMEDITBOX, CAFCreateEditPhonenumCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_DIGITALEDITBOX, CAFCreateEditDigitalCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_PASSWORDEDITBOX, CAFCreateEditPasswordCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_DATEEDITBOX, CAFCreateEditDateCtrl, 5) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_TIMEEDITBOX, CAFCreateEditTimeCtrl, 5) },

    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_LABEL, CAFCreateLabelCtrl, 8) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_TEXTBOX, CAFCreateTextBoxCtrl, 6) },

    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_ANIM, CAFCreateAnimCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_BUTTON, CAFCreateButtonCtrl, 9) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_DROPDOWNLIST, CAFCreateDropDownListCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_ICONLIST, CAFCreateIconListCtrl, 5) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_MENU, CAFCreateMenuCtrl, 6) },
    { WIN_KEYWORD(CAF_PARSEWIN_CREATE_POPMENU, CAFCreatePopmenuCtrl, 1) },
};

#ifdef MMIPUBWIN_PDASTYLE_SUPPORT
//parse pub list win
LOCAL const PARSE_FUNC PubWinParseFunc[] =
{
    DummyParse1,           // SetWinPrio,     
    SetPubWinFunc,         // SetWinFunc,
    SetPubWinId,           // SetWinId,
    DummyParse2,           // CreateMenuCtrl,
    DummyParse2,           // CreatePopmenuCtrl,
    DummyParse2,           // CreateMsgBoxCtrl,
    DummyParse2,           // CreatePrgBoxCtrl,
    DummyParse1,           // CreateTextCtrl,
    SetPubWinList,         // CreateListBoxCtrl,
    SetPubWinEditText,     // CreateEditTextCtrl,
    SetPubWinEditPhonenum, // CreateEditPhonenumCtrl,
    SetPubWinEditDigital,  // CreateEditDigitalCtrl,
    SetPubWinEditPassword, // CreateEditPasswordCtrl,
    SetPubWinEditList,     // CreateEditListCtrl,
    SetPubWinEditDate,     // CreateEditDateCtrl,
    SetPubWinEditTime,     // CreateEditTimeCtrl,
    SetPubWinEditTouchDate,// CreateEditTouchDateCtrl,
    SetPubWinEditTouchTime,// CreateEditTouchTimeCtrl,
    SetPubWinEditPicker,   // CreateEditPickerCtrl,
    SetPubWinEditIP,       // CreateEditIPCtrl,
    DummyParse2,           // CreateAnimCtrl,
    DummyParse1,           // SetWinBackgroundID,
    SetPubWinLabel,        // CreateLabelCtrl,
    DummyParse2,           // CreateButtonCtrl,
    DummyParse1,           // SetWinStyle,
    DummyParse3,           // CreateDropDownListCtrl,
    DummyParse1,           // CreateIconListCtrl,
    DummyParse1,           // CreateRichTextCtrl,
    DummyParse3,           // CreateTabCtrl,
    DummyParse0,           // CreateStatusBarCtrl,
    SetPubWinSoftkey,      // CreateSoftkeyCtrl,
    SetPubWinTitle,        // CreateTitleCtrl,
    DummyParse1,           // SetWinMoveStyle,
    DummyParse0,           // CreateTipsCtrl,
    DummyParse1,           // CreateSettingList,
    SetPubWinForm,         // CreateFormCtrl,
    DummyParse2,           // CreateOwnDrawCtrl,
    DummyParse1,           // CreateToolbarCtrl,
    DummyParse1,           // CreateIconFolderCtrl,
    DummyParse0,           // SetHideStatusBarStyle
    DummyParse1,           // SetWinSupportAngle,
#ifdef GUI_CTRL_SCROLL_KEY
    DummyParse6,           // CreateScrollKeyCtrl,
#endif
};

//parse child control
LOCAL const PARSE_FUNC PubWinFormParseFunc[] =
{
    DummyParse3, //CreateChildTextCtrl,
    DummyParse4, //CreateChildEditTextCtrl,
    DummyParse4, //CreateChildEditPhonenumCtrl,
    DummyParse4, //CreateChildEditDigitalCtrl,
    DummyParse4, //CreateChildEditPasswordCtrl,
    DummyParse5, //CreateChildEditListCtrl,
    DummyParse3, //CreateChildEditDateCtrl,
    DummyParse3, //CreateChildEditTimeCtrl,
    DummyParse3, //CreateChildEditIPCtrl,
    DummyParse4, //CreateChildAnimCtrl,
    DummyParse4, //CreateChildLabelCtrl,
    DummyParse4, //CreateChildButtonCtrl,
    DummyParse3, //CreateChildDropdownlistCtrl,
    DummyParse3, //CreateChildSetListCtrl,
    DummyParse4, //CreateChildOwnDrawCtrl,
    DummyParse4, //CreateChildFormCtrl,
    DummyParse4, //CreateChildListCtrl,
    DummyParse5, //CreateChildSoftkeyCtrl,
    PNULL,
    DummyParse3, //CreateChildEditTouchDate,
    DummyParse3, //CreateChildEditTouchTime,
    DummyParse3, //CreateChildEditPicker,
};
#endif

// 系统默认控件的参数
LOCAL MMK_DEFAULT_CTRL_PARAM_T s_default_ctrl_param = {0};

/**---------------------------------------------------------------------------*
 **                         Functions                                         *
 **---------------------------------------------------------------------------*/
/*****************************************************************************/
// 	Description : parse window tab
//	Global resource dependence : 
//  Author: Jasmine
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_ParseWinTab(    
                               MMI_HANDLE_T  win_handle,
                               uint32       *win_tab_ptr
                               )
{
    BOOLEAN     result = FALSE;
    
    SCI_PASSERT(PNULL != win_tab_ptr,("MMK_ParseWinTab: the param is PNULL!")); /*assert verified*/

    // 预先解析的关键字，置下状态
    result = PreParseWinTab(win_handle, win_tab_ptr);
    if (result)
    {
        // 正式解析
        result = ParseWinTab(win_handle, win_tab_ptr);

        if (result)
        {
            // 如果设置了状态栏，在这里创建它
            result = MMK_CreateDefaultControl(win_handle, &s_default_ctrl_param);
        }
    }

    if (!result)
    {
        GUI_DumpMemData(win_tab_ptr, 4, 50);
    }

    return (result);
}

/*****************************************************************************/
// 	Description : preparse window tab
//	Global resource dependence : 
//  Author: Jasmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN PreParseWinTab(
                             MMI_HANDLE_T  win_handle,
                             uint32        *win_tab_ptr
                             )
{
    BOOLEAN     result = FALSE;
    uint32      *value_ptr = PNULL;
    uint32      ins = 0;
    
    value_ptr = win_tab_ptr;

    while ((END_WIN != *value_ptr) && (CAF_END_WIN != *value_ptr))
    {
        ins = *value_ptr;

        value_ptr++;

        if ((MMK_WINDOW_STYLE == ins) || 
            (CAF_PARSEWIN_WINDOW_STYLE == ins))
        {
            result = SetWinStyle(win_handle,&value_ptr);
        }
        else if (MMK_CREATE_TITLE == ins
            || CAF_PARSEWIN_WINTITLEID == ins)
        {
            result = SetTitleStyle(win_handle,&value_ptr);
        }
        else if (MMK_CREATE_STATUSBAR == ins)
        {
#ifdef MRAPP_SUPPORT
        	if((MMIMRAPP_GetVMState() != VM_RUN) && (MMIMRAPP_GetVMState() != VM_PAUSE))
        	{
            	result = SetStatusBarStyle(win_handle,&value_ptr);
            }
            else
            {
                result = SetHideStatusBarStyle(win_handle,&value_ptr);
            }
#else
            result = SetStatusBarStyle(win_handle,&value_ptr);
#endif
        }
        else if (MMK_CREATE_SOFTKEY == ins
            || CAF_PARSEWIN_CREATE_SOFTKEY == ins)
        {
            result = SetSoftkeyStyle(win_handle,&value_ptr);
        }
        else if ((FIRST_PARSE <= ins) && (END_WIN > ins))
        {
            result = DummyParse(&value_ptr, MMKParseInfo[ins - FIRST_PARSE].param_num);
        }
        else if ((CHILD_CTRL_FIRST <= ins) && (LAST_CHILD > ins))
        {
            result = DummyParse(&value_ptr, FormChildCtrlParseInfo[ins - CHILD_CTRL_FIRST].param_num);
        }
        else if ((CAF_PARSEWIN_FIRST <= ins) && (CAF_PARSEWIN_MAX > ins))
        {
            result = DummyParse(&value_ptr, CAFParseInfo[ins - CAF_PARSEWIN_FIRST].param_num);
        }
        else
        {
            SCI_ASSERT(FALSE); /*assert verified*/
        }

        if (!result)
        {
            break;
        }
    }

    return (result);
}

/*****************************************************************************/
// 	Description : parse window tab
//	Global resource dependence : 
//  Author: Jasmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN ParseWinTab(
                          MMI_HANDLE_T  win_handle,
                          uint32        *win_tab_ptr
                          )
{
    BOOLEAN     result = FALSE;
    uint32      *value_ptr = PNULL;
    uint32      ins = 0;
    
    value_ptr = win_tab_ptr;

    while ((END_WIN != *value_ptr) && (CAF_END_WIN != *value_ptr))
    {
        ins = *value_ptr;

        value_ptr++;

        if ((FIRST_PARSE <= ins) && (END_WIN > ins))
        {
            result = MMKParseInfo[ins - FIRST_PARSE].parse_func(win_handle,&value_ptr);
        }
        else if ((CHILD_CTRL_FIRST <= ins) && (LAST_CHILD > ins))
        {
            result = GUIFORM_ParseChild((ins - CHILD_CTRL_FIRST),win_handle,&value_ptr);
        }
        else if ((CAF_PARSEWIN_FIRST <= ins) && (CAF_PARSEWIN_MAX > ins))
        {
            result = CAFParseInfo[ins - CAF_PARSEWIN_FIRST].parse_func(win_handle,&value_ptr);
        }
        else
        {
            SCI_ASSERT(FALSE); /*assert verified*/
        }

        if (!result)
        {
            break;
        }
    }

    return result;
}

/*****************************************************************************/
// 	Description : dummy
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse(
                         uint32**  value_pptr,
                         uint32    num
                         )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;

    value_ptr += num;

    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

#ifdef MMIPUBWIN_PDASTYLE_SUPPORT
/*****************************************************************************/
// 	Description : dummy0
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse0(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : dummy1
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse1(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;
    
    value_ptr++;
    
    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : dummy2
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse2(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;
    
    value_ptr += 2;
    
    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : dummy3
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse3(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;
    
    value_ptr += 3;
    
    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : dummy4
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse4(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;
    
    value_ptr += 4;
    
    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : dummy5
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse5(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;
    
    value_ptr += 5;
    
    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : dummy6
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN DummyParse6(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32* value_ptr = PNULL;

    value_ptr = *value_pptr;
    
    value_ptr += 6;
    
    *value_pptr = value_ptr;

    //do nothing

    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win func
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinFunc(
                            uint32    param,
                            uint32**  value_pptr
                            )
{
    uint32*             value_ptr = PNULL;
    WINDOW_HANDLE_EVENT user_func = PNULL;
    MMIPUB_PARAM_T*     param_ptr = (MMIPUB_PARAM_T*)param;

    value_ptr = *value_pptr;
    
    user_func = (WINDOW_HANDLE_EVENT)(*value_ptr++);
    
    *value_pptr = value_ptr;

    param_ptr->user_func = user_func;

    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win id
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinId(
                          uint32    param,
                          uint32**  value_pptr
                          )
{
    uint32*         value_ptr = PNULL;
    MMI_WIN_ID_T    win_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    win_id = (MMI_WIN_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;

    param_ptr->win_id = win_id;

    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win list
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinList(
                            uint32    param,
                            uint32**  value_pptr
                            )
{
    uint32*         value_ptr = PNULL;
    GUILIST_TYPE_E  list_type = GUILIST_TYPE_NONE;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    list_type = (GUILIST_TYPE_E)*value_ptr++;
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_LIST_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.list_param.list_type = list_type;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win label
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinLabel(
                             uint32    param,
                             uint32**  value_pptr
                             )
{
    uint32*          value_ptr = PNULL;
    MMI_CTRL_ID_T    ctrl_id = 0;
    GUILABEL_ALIGN_E align = GUILABEL_ALIGN_LEFT;
    MMIPUB_PARAM_T*  param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    align       = (GUILABEL_ALIGN_E)(*value_ptr++);
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.label_id    = ctrl_id;
        param_ptr->ctrl_param.edit_param.label_align = align;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit text
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditText(
                                uint32    param,
                                uint32**  value_pptr
                                )
{
    uint32*         value_ptr = PNULL;
    uint16          max_len = 0;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    max_len = (uint16)(*value_ptr++);
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_TEXT;
        param_ptr->ctrl_param.edit_param.max_len   = max_len;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit phonenum
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditPhonenum(
                                    uint32    param,
                                    uint32**  value_pptr
                                    )
{
    uint32*         value_ptr = PNULL;
    uint16          max_len = 0;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    max_len = (uint16)(*value_ptr++);
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_PHONENUM;
        param_ptr->ctrl_param.edit_param.max_len   = max_len;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit digital
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditDigital(
                                    uint32    param,
                                    uint32**  value_pptr
                                    )
{
    uint32*         value_ptr = PNULL;
    uint16          max_len = 0;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    max_len = (uint16)(*value_ptr++);
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_DIGITAL;
        param_ptr->ctrl_param.edit_param.max_len   = max_len;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}


/*****************************************************************************/
// 	Description : set pub win edit password
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditPassword(
                                    uint32    param,
                                    uint32**  value_pptr
                                    )
{
    uint32*         value_ptr = PNULL;
    uint16          max_len = 0;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    max_len = (uint16)(*value_ptr++);
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_PASSWORD;
        param_ptr->ctrl_param.edit_param.max_len   = max_len;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit list
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditList(
                                uint32    param,
                                uint32**  value_pptr
                                )
{
    uint32*         value_ptr = PNULL;
    uint16          item_num = 0;
    uint16          item_max_len = 0;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    item_num     = (uint16)(*value_ptr++);
    item_max_len = (uint16)(*value_ptr++);
    ctrl_id      = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_DIGITAL;
        param_ptr->ctrl_param.edit_param.add_data.list.item_num = item_num;
        param_ptr->ctrl_param.edit_param.add_data.list.item_max_len = item_max_len;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit date
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditDate(
                                uint32    param,
                                uint32**  value_pptr
                                )
{
    uint32*         value_ptr  = PNULL;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_DATE;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit date
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditTime(
                                uint32    param,
                                uint32**  value_pptr
                                )
{
    uint32*         value_ptr = PNULL;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_TIME;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit touch date
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditTouchDate(
                                     uint32    param,
                                     uint32**  value_pptr
                                     )
{
    uint32*         value_ptr = PNULL;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_TOUCH_DATE;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit touch time
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditTouchTime(
                                     uint32    param,
                                     uint32**  value_pptr
                                     )
{
    uint32*         value_ptr = PNULL;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_TOUCH_TIME;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit picker
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditPicker(
                                  uint32    param,
                                  uint32**  value_pptr
                                  )
{
    uint32*         value_ptr = PNULL;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_PICKER;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win edit ip
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinEditIP(
                              uint32    param,
                              uint32**  value_pptr
                              )
{
    uint32*         value_ptr = PNULL;
    MMI_CTRL_ID_T   ctrl_id = 0;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;
    
    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_EDITBOX_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.edit_param.edit_type = GUIEDIT_TYPE_IP;
        param_ptr->ctrl_id = ctrl_id;
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win softkey
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinSoftkey(
                               uint32    param,
                               uint32**  value_pptr
                               )
{
    uint32*         value_ptr = PNULL;
    MMI_TEXT_ID_T   left_sk_id = TXT_NULL;
    MMI_TEXT_ID_T   middle_sk_id = TXT_NULL;
    MMI_TEXT_ID_T   right_sk_id = TXT_NULL;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;

    value_ptr = *value_pptr;

	left_sk_id   = *value_ptr++;
	middle_sk_id = *value_ptr++;
	right_sk_id	 = *value_ptr++;

    *value_pptr = value_ptr;
	
    param_ptr->left_sk_id   = left_sk_id;
    param_ptr->middle_sk_id = middle_sk_id;

    param_ptr->right_sk_id  = right_sk_id;

    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win title
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinTitle(
                             uint32    param,
                             uint32**  value_pptr
                             )
{
    uint32*         value_ptr = PNULL;
    MMI_TEXT_ID_T   title_id  = TXT_NULL;
    MMIPUB_PARAM_T* param_ptr = (MMIPUB_PARAM_T*)param;

    value_ptr = *value_pptr;

	title_id = *value_ptr++;

    *value_pptr = value_ptr;
    
    param_ptr->title_id = title_id;

    return TRUE;
}

/*****************************************************************************/
// 	Description : set pub win form
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetPubWinForm(
                            uint32    param,
                            uint32**  value_pptr
                            )
{
    uint32*          value_ptr = PNULL;
    MMI_CTRL_ID_T    ctrl_id = 0;
    GUIFORM_LAYOUT_E layout_type = GUIFORM_LAYOUT_ORDER;
    MMIPUB_PARAM_T*  param_ptr = (MMIPUB_PARAM_T*)param;

    value_ptr = *value_pptr;

    layout_type = (GUIFORM_LAYOUT_E)(*value_ptr++);
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    *value_pptr = value_ptr;
    
    if ( SPRD_GUI_FORM_ID == param_ptr->guid )
    {
        param_ptr->ctrl_param.form_param.layout_type = layout_type;
        param_ptr->ctrl_id = ctrl_id;
    }

    return TRUE;
}

/*****************************************************************************/
// 	Description : parse pub list window tab
//	Global resource dependence : 
//  Author: Jasmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN ParsePubWinTab(    
                             MMIPUB_PARAM_T* param,
                             uint32*         win_tab_ptr
                             )
{
    uint32      *value_ptr = PNULL;
    uint32      ins = 0;
    
    if ( PNULL == param
        || PNULL == win_tab_ptr)
    {
        return FALSE;
    }
    
    value_ptr = win_tab_ptr;
    while ((END_WIN != *value_ptr) && (CAF_END_WIN != *value_ptr))
    {
        ins = *value_ptr;
        
        value_ptr++;
        
        if ((FIRST_PARSE <= ins) && (END_WIN > ins))
        {
            PubWinParseFunc[ins - FIRST_PARSE]( (uint32)param, &value_ptr );
        }
        
    }
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create pub list win tab
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_CreatePubwinTabEx(
                                     MMI_HANDLE_T applet_handle,
                                     CAF_GUID_T   guid,
                                     uint32		*win_tab_ptr,
                                     ADD_DATA     add_data_ptr 
                                     )
{
    MMI_HANDLE_T  win_handle = FALSE;
    uint32        *value_ptr = PNULL;
    uint32         ins       = 0;
    MMIPUB_PARAM_T param     = {0};

    if ( PNULL == win_tab_ptr)
    {
        return FALSE;
    }
    param.applet_handle = applet_handle;
    param.guid = guid;
    param.add_data_ptr = add_data_ptr;
    
    value_ptr = win_tab_ptr;

    while ((END_WIN != *value_ptr) && (CAF_END_WIN != *value_ptr))
    {
        ins = *value_ptr;

        value_ptr++;

        if ((FIRST_PARSE <= ins) && (END_WIN > ins))
        {
            PubWinParseFunc[ins - FIRST_PARSE]( (uint32)&param, &value_ptr );
        }
        else if ((CHILD_CTRL_FIRST <= ins) && (LAST_CHILD > ins))
        {
            PubWinFormParseFunc[ins - CHILD_CTRL_FIRST]( (uint32)&param, &value_ptr );
        }
        else
        {
            SCI_ASSERT(FALSE); /*assert verified*/
        }
    }

    if ( 0 != ( win_handle = MMIPUB_OpenWin( &param ) ) )
    {
        value_ptr = win_tab_ptr;
        
        while ((END_WIN != *value_ptr) && (CAF_END_WIN != *value_ptr))
        {
            ins = *value_ptr;
            
            value_ptr++;
            
            if ((FIRST_PARSE <= ins) && (END_WIN > ins))
            {
                PubWinParseFunc[ins - FIRST_PARSE]( (uint32)&param, &value_ptr );
            }
            else if ((CHILD_CTRL_FIRST <= ins) && (LAST_CHILD > ins))
            {
                GUIFORM_ParseChild((ins - CHILD_CTRL_FIRST),win_handle,&value_ptr);
            }
            else
            {
                SCI_ASSERT(FALSE); /*assert verified*/
            }
        }
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************/
// 	Description : create pub list win tab
//	Global resource dependence : 
//  Author:James.Zhang
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_CreatePubwinTab(
                                   CAF_GUID_T   guid,
                                   uint32		*win_tab_ptr,
                                   ADD_DATA     add_data_ptr 
                                   )
{
    return MMK_CreatePubwinTabEx(MMK_GetFirstAppletHandle(),
        guid,  win_tab_ptr, add_data_ptr);
}
#endif

/*****************************************************************************/
// 	Description : create control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMK_CreateCtrlByWinTab(    
									 MMI_WIN_ID_T	win_id,
									 uint32			*win_tab_ptr
									 )
{
    BOOLEAN       result     = FALSE;
	MMI_HANDLE_T  win_handle = 0;
	
	SCI_ASSERT(NULL != win_tab_ptr); /*assert verified*/

	win_handle = MMK_ConvertIdToHandle( win_id );

	result = MMK_ParseWinTab(win_handle, win_tab_ptr);

    return result;
}

/*****************************************************************************/
// 	Description : create control table
//	Global resource dependence : 
//  Author:James Zhang
//	Note:destroy all control first
/*****************************************************************************/
PUBLIC BOOLEAN MMK_CreateCtrlByWinTabEx(    
                                        MMI_WIN_ID_T	win_id,
                                        uint32			*win_tab_ptr
                                        )
{
    BOOLEAN       result     = FALSE;
    MMI_HANDLE_T  win_handle = 0;
    
    SCI_ASSERT(NULL != win_tab_ptr); /*assert verified*/
    
    win_handle = MMK_ConvertIdToHandle( win_id );
    
    MMK_DestroyAllControl( win_handle );
    
    result = MMK_ParseWinTab(win_handle, win_tab_ptr);

    return result;
}

/*****************************************************************************/
// 	Description : the function , set window priority
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinPrio(
                         MMI_HANDLE_T win_handle,
                         uint32**      value_pptr
                         )
{
    uint32*            value_ptr    = PNULL;
    MMI_WIN_PRIORITY_E win_priority = 0;
    BOOLEAN            result       = FALSE;
    
    value_ptr = *value_pptr;
    
    win_priority = (MMI_WIN_PRIORITY_E)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    // check the priority
    if ( win_priority > WIN_THREE_LEVEL || win_priority < WIN_LOWEST_LEVEL )/*lint !e685 */
    {
        //SCI_TRACE_LOW:"SetWinPrio: ERROR, the priority = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,WINDOW_PARSE_2142_112_2_18_2_0_36_303,(uint8*)"d", win_priority );
        result = FALSE;
    }
    else
    {
        MMK_SetWinPriority( win_handle, win_priority );
        
        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
// 	Description : the function , set window identify
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinId(
                       MMI_HANDLE_T win_handle,
                       uint32**      value_pptr
                       )
{
    uint32*  value_ptr   = PNULL;
    uint32   win_id      = 0;
    BOOLEAN  result      = FALSE;
    
    value_ptr = *value_pptr;
    
    win_id = (MMI_WIN_ID_T)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    // check the win_id, 使用MMK_IsExistWin, 解决关闭窗口消息中打开id相同窗口的问题
    if ( MMK_IsExistWin( MMK_GetAppletHandleByWin( win_handle ), win_id ) )
    //if ( MMK_GetWinHandle( MMK_GetAppletHandleByWin( win_handle ), win_id ) )
    {
        //SCI_TRACE_LOW:"SetWinId: the window is open! the win_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,WINDOW_PARSE_2178_112_2_18_2_0_36_304,(uint8*)"d", win_id );
        result = FALSE;
    }
    else
    {
        MMK_SetWinId( win_handle, win_id );
        
        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
// 	Description : the function , set window identify
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinFunc(
                         MMI_HANDLE_T win_handle,
                         uint32**      value_pptr
                         )
{
    uint32*               value_ptr   = PNULL;
    WINDOW_HANDLE_EVENT   func        = 0;
    
    value_ptr = *value_pptr;
    
    func = (WINDOW_HANDLE_EVENT)(*value_ptr++);
    
    *value_pptr = value_ptr;
    
    MMK_SetWinFunc( win_handle, func );
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create the menu control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateMenuCtrl(
                             MMI_HANDLE_T   win_handle,
                             uint32         **value_pptr
                             )
{
    BOOLEAN                 result = TRUE;
    uint32*                 value_ptr = PNULL;
    uint32                  group_id = 0;
    MMI_CTRL_ID_T           ctrl_id   = 0;
    GUIMENU_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    create = {0};
    
    value_ptr = *value_pptr;
    
    group_id = *value_ptr++;
    ctrl_id  = (MMI_CTRL_ID_T)(*value_ptr++);
    
    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    if (MENU_DYNA == group_id)
    {
        init_data.is_static  = FALSE;
        init_data.menu_style = GUIMENU_STYLE_SECOND;
    }
    else
    {
        init_data.is_static = TRUE;
        init_data.group_id  = group_id;
    }

    //create
    create.ctrl_id           = ctrl_id;
    create.guid              = SPRD_GUI_MENU_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr     = &init_data;
    
    MMK_CreateControl(&create);
    
    *value_pptr = value_ptr;
    
    return (result);
}

/*****************************************************************************/
// 	Description : create the pop_up menu control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreatePopmenuCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    uint32                  group_id = 0;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIMENU_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    group_id     = *value_ptr++;
    ctrl_id      = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    if (MENU_DYNA == group_id)
    {
        init_data.is_static = FALSE;
    }
    else
    {
        init_data.is_static = TRUE;
        init_data.group_id  = group_id;
    }

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_MENU_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the message box control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateMsgBoxCtrl(
                               MMI_HANDLE_T     win_handle,
                               uint32           **value_pptr
                               )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIMSGBOX_STYLE_T       style = 0;
    MMI_CONTROL_CREATE_T    ctrl_create = {0};
    GUIMSGBOX_INIT_DATA_T   init_data = {0};

    value_ptr = *value_pptr;

    style   = (GUIMSGBOX_STYLE_T)(*value_ptr++);
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    rect    = MMITHEME_GetMsgBoxRect();

    //set init data
    init_data.style    = style;
    init_data.rect_ptr = &rect;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_MSGBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the progress box control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreatePrgBoxCtrl(
                               MMI_HANDLE_T     win_handle,
                               uint32           **value_pptr
                               )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIPRGBOX_STYLE_E       prgbox_style = GUIPRGBOX_STYLE_VERTICAL_SCROLL;
    MMI_CONTROL_CREATE_T    ctrl_create = {0};
    GUIPRGBOX_INIT_DATA_T   init_data = {0};

    value_ptr = *value_pptr;

    prgbox_style = (GUIPRGBOX_STYLE_E)*value_ptr++;
    ctrl_id      = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.style  = prgbox_style;
    init_data.both_rect   = MMITHEME_GetWinClientBothRect(win_handle);

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_PRGBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the text control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTextCtrl(
                             MMI_HANDLE_T   win_handle,
                             uint32         **value_pptr
                             )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUITEXT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_TEXTBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the list box control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateListBoxCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    uint32*                 value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id   = 0;
    GUILIST_TYPE_E          list_type = GUILIST_TYPE_NONE;
    GUILIST_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    create = {0};
    
    value_ptr = *value_pptr;
    
    list_type = (GUILIST_TYPE_E)*value_ptr++;
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);

    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = list_type;
    
    create.ctrl_id           = ctrl_id;
    create.guid              = SPRD_GUI_LIST_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr     = &init_data;
    
    MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create the edit box text control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTextCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect   = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type        = GUIEDIT_TYPE_TEXT;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box phone number control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditPhonenumCtrl(
                                     MMI_HANDLE_T   win_handle,
                                     uint32         **value_pptr
                                     )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect   = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type        = GUIEDIT_TYPE_PHONENUM;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box digital control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditDigitalCtrl(
                                    MMI_HANDLE_T    win_handle,
                                    uint32          **value_pptr
                                    )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect   = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type        = GUIEDIT_TYPE_DIGITAL;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box password control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditPasswordCtrl(
                                     MMI_HANDLE_T   win_handle,
                                     uint32         **value_pptr
                                     )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect   = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type        = GUIEDIT_TYPE_PASSWORD;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box list control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditListCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 )
{
    BOOLEAN                 result = TRUE;
    uint16                  item_num = 0;
    uint16                  item_max_len = 0;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    item_num = (uint16)(*value_ptr++);
    item_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect                  = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type                       = GUIEDIT_TYPE_LIST;
    init_data.add_data.list.item_num     = item_num;
    init_data.add_data.list.item_max_len = item_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box date control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditDateCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = GUIEDIT_TYPE_DATE;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box time control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTimeCtrl(
                                 MMI_HANDLE_T   win_handle,
                                 uint32         **value_pptr
                                 )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = GUIEDIT_TYPE_TIME;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box touch date control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTouchDateCtrl(
                                      MMI_HANDLE_T   win_handle,
                                      uint32         **value_pptr
                                      )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = GUIEDIT_TYPE_TOUCH_DATE;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box touch time control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditTouchTimeCtrl(
                                      MMI_HANDLE_T   win_handle,
                                      uint32         **value_pptr
                                      )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = GUIEDIT_TYPE_TOUCH_TIME;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box picker control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditPickerCtrl(
                                   MMI_HANDLE_T   win_handle,
                                   uint32         **value_pptr
                                   )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = GUIEDIT_TYPE_PICKER;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the edit box IP control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateEditIPCtrl(
                               MMI_HANDLE_T     win_handle,
                               uint32           **value_pptr
                               )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.type = GUIEDIT_TYPE_IP;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the animation control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateAnimCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**     value_pptr
                             )
{
    void                    *ctrl_ptr = PNULL;
    uint32                  *value_ptr = PNULL;
    MMI_WIN_ID_T            win_id = 0;
    MMI_CTRL_ID_T           ctrl_id = 0;
    MMI_CONTROL_CREATE_T    create = {0};
    GUIANIM_INIT_DATA_T     init_data = {0};

    value_ptr = *value_pptr;
    
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    win_id  = (MMI_WIN_ID_T)(*value_ptr++);

    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_ANIM_ID;
    create.parent_win_handle = win_handle;    
    create.init_data_ptr = &init_data;
    
    ctrl_ptr = MMK_CreateControl( &create );

    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : set the window background image id
//	Global resource dependence : 
//  Author: Tracy Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinBackgroundID(
                                 MMI_HANDLE_T win_handle,
                                 uint32**      value_pptr
                                 )
{
    uint32*  value_ptr   = PNULL;
    MMI_IMAGE_ID_T background_image_id = IMAGE_NULL;
    
    value_ptr = *value_pptr;
    
    background_image_id = (MMI_IMAGE_ID_T)(*value_ptr++);
    
    MMK_SetWinBackgroundImageId( win_handle, background_image_id );
    
    *value_pptr = value_ptr;
    
    return (TRUE);
}

/*****************************************************************************/
// 	Description : create the label control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateLabelCtrl(
                              MMI_HANDLE_T  win_handle,
                              uint32        **value_pptr
                              )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUILABEL_ALIGN_E        align = GUILABEL_ALIGN_LEFT;
    GUILABEL_INIT_DATA_T   init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    align       = (GUILABEL_ALIGN_E)(*value_ptr++);
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.align = align;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_LABEL_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the button control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateButtonCtrl(
                               MMI_HANDLE_T  win_handle,
                               uint32        **value_pptr
                               )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_IMAGE_ID_T          bg_img_id = 0;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIBUTTON_INIT_DATA_T   init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    bg_img_id = (MMI_IMAGE_ID_T)(*value_ptr++);
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    if (MMITHEME_CheckImageID(bg_img_id))
    {
        init_data.bg.bg_type = GUI_BG_IMG;
        init_data.bg.img_id  = bg_img_id;
    }
    else
    {
        init_data.bg.bg_type = GUI_BG_NONE;
    }

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_BUTTON_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the dropdown list control
//	Global resource dependence : 
//  Author:YELIANNA
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateDropDownListCtrl(
                                     MMI_HANDLE_T   win_handle,
                                     uint32         **value_pptr
                                     )
{
    uint16                      page_item_num = 0;
    uint32                      *value_ptr = PNULL;
    MMI_CTRL_ID_T               ctrl_id   = 0;
    MMI_CONTROL_CREATE_T        create = {0};
    GUIDROPDOWNLIST_SHOW_TYPE_E      show_type;
    GUIDROPDOWNLIST_INIT_DATA_T init_data = {0};
    
    value_ptr = *value_pptr;
    
    page_item_num = *value_ptr++;
    show_type     = *value_ptr++;
    ctrl_id       = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect     = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.page_item_num = page_item_num;
    init_data.show_type     = show_type;

    create.ctrl_id           = ctrl_id;
    create.guid              = SPRD_GUI_DROPDOWNLIST_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr     = &init_data;

    MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create the icon list control
//	Global resource dependence : 
//  Author: jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateIconListCtrl(
                                 MMI_HANDLE_T win_handle,
                                 uint32       **value_pptr
                                 )
{
    void                    *ctrl_ptr = PNULL;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    MMI_CONTROL_CREATE_T    create = {0};
    GUIICONLIST_INIT_DATA_T init_data = {0};

    value_ptr = *value_pptr;
    
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    create.ctrl_id           = ctrl_id;
    create.guid              = SPRD_GUI_ICONLIST_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr     = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create the icon folder control
//	Global resource dependence : 
//  Author: jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateIconFolderCtrl(
                                   MMI_HANDLE_T win_handle,
                                   uint32       **value_pptr
                                   )
{
    void                        *ctrl_ptr = PNULL;
    uint32                      *value_ptr = PNULL;
    MMI_CTRL_ID_T               ctrl_id = 0;
    MMI_CONTROL_CREATE_T        create = {0};
    GUIICONFOLDER_INIT_DATA_T   init_data = {0};

    value_ptr = *value_pptr;
    
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    create.ctrl_id           = ctrl_id;
    create.guid              = SPRD_GUI_ICONFOLDER_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr     = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create the richtext control
//	Global resource dependence : 
//  Author:liqing.peng
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateRichTextCtrl(MMI_HANDLE_T win_handle,   
                                 uint32**      value_pptr)
{

    uint32*               value_ptr = PNULL;
    MMI_CONTROL_CREATE_T create = {0};
    GUIRICHTEXT_INIT_DATA_T init_data = {0};
    MMI_CTRL_ID_T         ctrl_id   = 0;
    void*                 ctrl_ptr = PNULL;
    
    value_ptr = *value_pptr;
    
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_RICHTEXT_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create the tab control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTabCtrl(
                            MMI_HANDLE_T    win_handle,
                            uint32          **value_pptr
                            )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUITAB_STATE_T          state = 0;
    GUI_TAB_STYLE_E         style = GUI_TAB_ITEM_NONE;
    GUITAB_INIT_DATA_T      init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    style   = (GUI_TAB_STYLE_E)*value_ptr++;
    state   = (GUITAB_STATE_T)*value_ptr++;
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinTabBothRect(win_handle);
    init_data.style = style;
    init_data.state = state;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_TAB_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// Description : create the statusbar control
// Global resource dependence : 
// Author:Ming.Song
// Note:2007-7-31
/*****************************************************************************/
LOCAL BOOLEAN CreateStatusBarCtrl(
                                  MMI_HANDLE_T win_handle,
                                  uint32**      value_pptr
                                  )
{
    void*          ctrl_ptr      = PNULL;
    uint32*        value_ptr     = PNULL;
    MMI_CTRL_ID_T  ctrl_id       = MMITHEME_GetStatusBarCtrlId();
    MMI_CONTROL_CREATE_T create  = {0};
    GUISTATUSBAR_INIT_DATA_T init_data = {0};

    value_ptr = *value_pptr;

    init_data.both_rect = MMITHEME_GetStatusBarBothRect();

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_STATUSBAR_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

#ifdef MRAPP_SUPPORT
	if((MMIMRAPP_GetVMState() != VM_RUN) && (MMIMRAPP_GetVMState() != VM_PAUSE))
	{
		ctrl_ptr = MMK_CreateControl( &create );
	}
#else
    ctrl_ptr = MMK_CreateControl( &create );
#endif
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : set status bar style
//	Global resource dependence : 
//  Author: hua.fang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetStatusBarStyle(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    uint32*  value_ptr   = PNULL;

    value_ptr = *value_pptr;

    // 只设标志不创建，后续会创建
    MMK_SetWinDisplayStyleState( win_handle, WS_HAS_STATUSBAR, TRUE );

    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// Description : set no status bar ctrl
// Global resource dependence : 
// Author:jin.wang
// Note:
/*****************************************************************************/
LOCAL BOOLEAN SetHideStatusBarStyle(
                                    MMI_HANDLE_T    win_handle,
                                    uint32**        value_pptr
                                    )
{
    uint32*  value_ptr   = PNULL;

    value_ptr = *value_pptr;

    MMK_SetWinDisplayStyleState( win_handle, WS_HAS_STATUSBAR, FALSE );

    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinStyle(
                          MMI_HANDLE_T  win_handle,
                          uint32**      value_pptr
                          )
{
    uint32*  value_ptr   = PNULL;
    WINDOW_DISPLAY_STYLE    window_style = 0;
    WINDOW_DISPLAY_STYLE    default_win_style = 0;
    
    value_ptr = *value_pptr;

    window_style = (WINDOW_DISPLAY_STYLE) (*value_ptr++);

    // 如果设置了WS_NO_DEFAULT_STYLE，将默认风格去掉
    // 默认风格在创建窗口时已经赋值，这里是将默认风格去掉
    if (0 != (window_style&WS_NO_DEFAULT_STYLE))
    {
        default_win_style = MMITHEME_GetDefaultWinStyle();
        MMK_SetWinDisplayStyleState(win_handle, default_win_style, FALSE);
    }
        
    MMK_SetWinDisplayStyleState( win_handle, window_style, TRUE );
    
    *value_pptr = value_ptr;
    
    return (TRUE);
}

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinMoveStyle(
                              MMI_HANDLE_T win_handle,
                              uint32**      value_pptr
                              )
{
    uint32*  value_ptr   = PNULL;
    WINDOW_MOVE_STYLE window_move_style = 0;
    
    value_ptr = *value_pptr;
    
    window_move_style = (WINDOW_MOVE_STYLE)(*value_ptr++);
    
    MMK_SetWinMoveStyle( win_handle, window_move_style );
    
    *value_pptr = value_ptr;

    return (TRUE);
    
}

/*****************************************************************************/
// 	Description : set the window support angle
//	Global resource dependence : 
//  Author: James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetWinSupportAngle(
                                 MMI_HANDLE_T win_handle,
                                 uint32**      value_pptr
                                 )
{
    uint32*  value_ptr   = PNULL;
    WINDOW_SUPPORT_ANGLE support_angle = 0;
    
    value_ptr = *value_pptr;
    
    support_angle = (WINDOW_SUPPORT_ANGLE)(*value_ptr++);
    
    MMK_SetWinSupportAngle( win_handle, support_angle );
    
    *value_pptr = value_ptr;

    return (TRUE);
}

#ifdef GUI_CTRL_SCROLL_KEY
/*****************************************************************************/
// 	Description : create scrollkey control 
//	Global resource dependence : 
//  Author: Cheney Wu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateScrollKeyCtrl(
                                MMI_HANDLE_T win_handle,
                                uint32**      value_pptr
                                )
{
    uint32*  value_ptr   = PNULL;

	uint32 left_scroll_key_id    = 0;
	uint32 right_scroll_key_id   = 0;
	uint32 left_scroll_text_id   = 0;
	uint32 right_scroll_text_id  = 0;
	uint32 left_scroll_skin_idx  = 0;
	uint32 right_scroll_skin_idx = 0;	
	MMI_CONTROL_CREATE_T create = {0};
	GUISCROLLKEY_INIT_DATA_T init_data = {0};
	void *ctrl_ptr = PNULL;
	MMI_CTRL_ID_T	ctrl_id = 0;

    value_ptr = *value_pptr;

	left_scroll_key_id    = *value_ptr++;
	right_scroll_key_id   = *value_ptr++;
	left_scroll_text_id   = *value_ptr++;
	right_scroll_text_id  = *value_ptr++;	
	left_scroll_skin_idx  = *value_ptr++;
	right_scroll_skin_idx = *value_ptr++;	

	init_data.left_scroll_key_id    = left_scroll_key_id;
	init_data.right_scroll_key_id   = right_scroll_key_id;
	init_data.left_scroll_text_id   = left_scroll_text_id;
	init_data.right_scroll_text_id  = right_scroll_text_id;
	init_data.left_scroll_skin_idx  = left_scroll_skin_idx;
	init_data.right_scroll_skin_idx = right_scroll_skin_idx;	

	ctrl_id = MMITHEME_GetScrollkeyCtrlId();
	create.ctrl_id = ctrl_id;
	create.guid    = SPRD_GUI_SCROLLKEY_ID;
	create.parent_win_handle = win_handle;
	create.init_data_ptr = &init_data;

	ctrl_ptr = MMK_CreateControl( &create );
#ifdef GUI_CTRL_SCROLL_KEY
	MMK_SetWinScrollkeyCtrlId(win_handle, ctrl_id);
#endif
    *value_pptr = value_ptr;
    
    return (TRUE);
}
#endif

/*****************************************************************************/
// 	Description : 转化类型
//	Global resource dependence : 
//  Author:
//	Note:
/*****************************************************************************/
LOCAL GUILIST_TYPE_E ConvertListTypeFromCAF2GUI(CAF_GUILIST_TYPE_E caf_type)
{
    GUILIST_TYPE_E gui_type = GUILIST_TEXTLIST_E;

    switch(caf_type)
    {
    case CAF_GUILIST_TYPE_NONE:
        gui_type = GUILIST_TYPE_NONE;
        break;

    case CAF_GUILIST_RADIOLIST_E:
        gui_type = GUILIST_RADIOLIST_E;
        break;

    case CAF_GUILIST_CHECKLIST_E:
        gui_type = GUILIST_CHECKLIST_E;
        break;

    case CAF_GUILIST_TEXTLIST_E:
        gui_type = GUILIST_TEXTLIST_E;
        break;  
        
    default:
        gui_type = GUILIST_TEXTLIST_E;
        break;
    }

    return gui_type;
}

/*****************************************************************************/
// 	Description : create the listbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateListBoxCtrl(
                                   MMI_HANDLE_T win_handle,
                                   uint32**      value_pptr
                                   )
{
    uint32*              value_ptr = PNULL;
    GUI_RECT_T           rect = {0};
    //note: maybe need convert to GUILIST_TYPE_E!!!
    CAF_GUILIST_TYPE_E   list_type = 0;
    MMI_CTRL_ID_T        ctrl_id   = 0;
    GUILIST_INIT_DATA_T  init_data = {0};
    MMI_CONTROL_CREATE_T create = {0};
    void* ctrl_ptr = PNULL;
    
    value_ptr = *value_pptr;
    
    rect.left = *value_ptr++;
    rect.top  = *value_ptr++;
    rect.right = *value_ptr++;
    rect.bottom = *value_ptr++;
    
    list_type = (CAF_GUILIST_TYPE_E)*value_ptr++;
    
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = ConvertListTypeFromCAF2GUI(list_type);
    
    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_LIST_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;
    ctrl_ptr = MMK_CreateControl( &create );
        
    /*    GUILIST_Init( ctrl_ptr, &init_data );*/
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : caf create the text editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditTextCtrl(
                                       MMI_HANDLE_T win_handle,
                                       uint32**      value_pptr
                                       )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = GUIEDIT_TYPE_TEXT;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the label control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateLabelCtrl(
                                 MMI_HANDLE_T  win_handle,
                                 uint32**      value_pptr
                                 )
{
    uint32*                   value_ptr = PNULL;
    GUI_RECT_T                rect      = {0};
    CAF_ALIGN_T               alignment = 0;
    MMI_IMAGE_ID_T            image_id  = IMAGE_NULL;
    MMI_TEXT_ID_T             text_id   = TXT_NULL;
    MMI_CTRL_ID_T             ctrl_id   = 0;
    MMI_CONTROL_CREATE_T      create    = {0};
    GUILABEL_INIT_DATA_T      init_data = {0};
    IGUICTRL_T*               ctrl_ptr  = PNULL;
    
    value_ptr = *value_pptr;
    
    rect.left = *value_ptr++;
    rect.top  = *value_ptr++;
    rect.right = *value_ptr++;
    rect.bottom = *value_ptr++;
    
    alignment = (GUILABEL_ALIGN_E)(*value_ptr++);

    text_id  = *value_ptr++; 
    image_id = *value_ptr++;
    
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;

    init_data.align = alignment;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_LABEL_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create the textbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateTextBoxCtrl(
                                   MMI_HANDLE_T win_handle,
                                   uint32**      value_pptr
                                   )
{
    uint32*           value_ptr = PNULL;
    GUI_RECT_T        rect = {0};
    MMI_CTRL_ID_T     ctrl_id   = 0;
    MMI_TEXT_ID_T     text_id   = TXT_NULL;
    MMI_CONTROL_CREATE_T create = {0};
    GUITEXT_INIT_DATA_T init_data = {0};
    void* ctrl_ptr = PNULL;
    
    value_ptr = *value_pptr;
    
    rect.left = *value_ptr++;
    rect.top  = *value_ptr++;
    rect.right = *value_ptr++;
    rect.bottom = *value_ptr++;
    
    text_id = *value_ptr++;
    
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_TEXTBOX_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );

    IGUITEXTBOX_SetTextId( ctrl_ptr, text_id, FALSE );
    
    *value_pptr = value_ptr;

    return TRUE;
}

#ifdef GUISOFTKEY_PDASTYLE_SUPPORT
/*****************************************************************************/
// 	Description : 获取softkey状态
//	Global resource dependence : 
//  Author:hua.fang
//	Note:
/*****************************************************************************/
LOCAL void GetSoftkeyState(
                           MMI_HANDLE_T        win_handle,
                           uint32              leftsoft_id,
                           uint32              midsoft_id,
                           uint32              rightsoft_id,
                           GUISOFTKEY_BUTTONSTYLE_E  *softkey_style_ptr,
                           BOOLEAN             *is_visible_ptr
                           )
{
    BOOLEAN is_visible = TRUE;
    GUISOFTKEY_BUTTONSTYLE_E softkey_style = GUISOFTKEY_BUTTONSTYLE_NONE;

    if (MMITHEME_IsIstyle()
        && MMK_GetWinDisplayStyleState(win_handle, WS_HAS_TITLE)
        && !MMK_GetWinDisplayStyleState(win_handle, WS_DISABLE_SOFTKEY_TO_TITLE))
    {

        softkey_style = GUIBUTTONSOFTKEY_GetStyleFromText(leftsoft_id, midsoft_id, rightsoft_id);

        if (GUISOFTKEY_BUTTONSTYLE_1BUTTON == softkey_style
            || GUISOFTKEY_BUTTONSTYLE_2BUTTON == softkey_style)
        {
            is_visible = FALSE;
        }
    }

    if (PNULL != softkey_style_ptr)
    {
        *softkey_style_ptr = softkey_style;
    }

    if (PNULL != is_visible_ptr)
    {
        *is_visible_ptr = is_visible;
    }
}
#endif

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateSoftkeyCtrl(
                                MMI_HANDLE_T win_handle,
                                uint32**      value_pptr
                                )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
	uint32                  leftsoft_id	= 0;
	uint32                  midsoft_id = 0;
	uint32                  rightsoft_id = 0;

    value_ptr = *value_pptr;

	leftsoft_id		= *value_ptr++;
	midsoft_id		= *value_ptr++;
	rightsoft_id	= *value_ptr++;

#ifdef GUISOFTKEY_PDASTYLE_SUPPORT
    //pda大多数场景不显示softkey
    if (MMK_GetWinDisplayStyleState(win_handle,WS_HAS_BUTTON_SOFTKEY))
#endif
    {
        BOOLEAN is_visible = TRUE;
#ifdef GUISOFTKEY_PDASTYLE_SUPPORT
        GUISOFTKEY_BUTTONSTYLE_E softkey_style = GUISOFTKEY_BUTTONSTYLE_NONE;

        GetSoftkeyState(
                win_handle, leftsoft_id, midsoft_id, rightsoft_id,
                &softkey_style, &is_visible);

        switch (softkey_style)
        {
        case GUISOFTKEY_BUTTONSTYLE_1BUTTON:
            if (MMITHEME_GetCancelTextID() == midsoft_id
                || MMITHEME_GetReturnTextID() == midsoft_id)
            {
                GUIWIN_SetTitleButtonTextId(win_handle, GUITITLE_BUTTON_LEFT, midsoft_id, FALSE);
                GUIWIN_SetTitleButtonHandleMsgInfo(win_handle, GUITITLE_BUTTON_LEFT, MSG_CTL_PENOK, MMITHEME_GetSoftkeyMidCtrlId());
            }
            else
            {
                GUIWIN_SetTitleButtonTextId(win_handle, GUITITLE_BUTTON_RIGHT, midsoft_id, FALSE);
                GUIWIN_SetTitleButtonHandleMsgInfo(win_handle, GUITITLE_BUTTON_RIGHT, MSG_CTL_PENOK, MMITHEME_GetSoftkeyMidCtrlId());
                GUIWIN_SetTitleButtonState(win_handle, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
            }
            break;

        case GUISOFTKEY_BUTTONSTYLE_2BUTTON:
            GUIWIN_SetTitleButtonTextId(win_handle, GUITITLE_BUTTON_RIGHT, leftsoft_id, FALSE);
            GUIWIN_SetTitleButtonHandleMsgInfo(win_handle, GUITITLE_BUTTON_RIGHT, MSG_NOTIFY_PENOK, MMITHEME_GetSoftkeyLeftCtrlId());
            GUIWIN_SetTitleButtonState(win_handle, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);

            GUIWIN_SetTitleButtonTextId(win_handle, GUITITLE_BUTTON_LEFT, rightsoft_id, FALSE);
            GUIWIN_SetTitleButtonHandleMsgInfo(win_handle, GUITITLE_BUTTON_LEFT, MSG_NOTIFY_PENOK, MMITHEME_GetSoftkeyRightCtrlId());
            break;

        case GUISOFTKEY_BUTTONSTYLE_3BUTTON:
            GUIWIN_SetTitleButtonTextId(win_handle, GUITITLE_BUTTON_RIGHT, leftsoft_id, FALSE);
            GUIWIN_SetTitleButtonHandleMsgInfo(win_handle, GUITITLE_BUTTON_RIGHT, MSG_NOTIFY_PENOK, MMITHEME_GetSoftkeyLeftCtrlId());
            GUIWIN_SetTitleButtonState(win_handle, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);

            GUIWIN_SetTitleButtonTextId(win_handle, GUITITLE_BUTTON_LEFT, rightsoft_id, FALSE);
            GUIWIN_SetTitleButtonHandleMsgInfo(win_handle, GUITITLE_BUTTON_LEFT, MSG_NOTIFY_PENOK, MMITHEME_GetSoftkeyRightCtrlId());

            leftsoft_id = MMITHEME_GetInvalidTextID();
            rightsoft_id = MMITHEME_GetInvalidTextID();
            break;

        default:
            break;
        }
#endif

        result = GUIWIN_CreateSoftkeyDyn(
                win_handle,
                leftsoft_id,
                midsoft_id,
                rightsoft_id);

#ifdef MMI_PDA_SUPPORT
        if (MMITHEME_IsIstyle())
        {
            GUIWIN_SetSoftkeyVisible(win_handle, is_visible);
        }
#endif
    }

    *value_pptr = value_ptr;
	
    return (result);
}

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetSoftkeyStyle(
                              MMI_HANDLE_T win_handle,
                              uint32**      value_pptr
                              )
{
    uint32 *value_ptr = PNULL;

    value_ptr = *value_pptr;

	s_default_ctrl_param.softkey_left_id    = *value_ptr++;
	s_default_ctrl_param.softkey_middle_id  = *value_ptr++;
	s_default_ctrl_param.softkey_right_id   = *value_ptr++;

#ifdef GUISOFTKEY_PDASTYLE_SUPPORT
    //pda大多数场景不显示softkey
    if (MMK_GetWinDisplayStyleState(win_handle,WS_HAS_BUTTON_SOFTKEY))
#endif
    {
#ifdef GUISOFTKEY_PDASTYLE_SUPPORT
        BOOLEAN is_visible = TRUE;

        GetSoftkeyState(
                win_handle,
                s_default_ctrl_param.softkey_left_id,
                s_default_ctrl_param.softkey_middle_id,
                s_default_ctrl_param.softkey_right_id,
                PNULL,
                &is_visible);

        if (is_visible)
#endif
        {
            MMK_SetWinDisplayStyleState( win_handle, WS_HAS_SOFTKEY, TRUE );
        }
    }

    *value_pptr = value_ptr;
	
    return (TRUE);
}

/*****************************************************************************/
// Description : create the tips control
// Global resource dependence : 
// Author:
// Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTipsCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**      value_pptr
                             )
{
    void*          ctrl_ptr      = PNULL;
    uint32*        value_ptr     = PNULL;
    MMI_CTRL_ID_T  ctrl_id       = MMITHEME_GetTipsCtrlId();
    MMI_CONTROL_CREATE_T create  = {0};
    GUITIPS_INIT_DATA_T init_data = {0};

    value_ptr = *value_pptr;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_TIPS_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;
    
    ctrl_ptr = MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create the setting list control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateSettingList(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    MMI_CONTROL_CREATE_T    ctrl_create = {0};
    GUISETLIST_INIT_DATA_T  init_data = {0};

    value_ptr = *value_pptr;

    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_SETLIST_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the container control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateFormCtrl(
                             MMI_HANDLE_T   win_handle,
                             uint32         **value_pptr
                             )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIFORM_LAYOUT_E        layout_type = 0;
    GUIFORM_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

    layout_type = (GUIFORM_LAYOUT_E)(*value_ptr++);
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.layout_type = layout_type;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_FORM_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the owner draw control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateOwnDrawCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    BOOLEAN                     result = TRUE;
    uint32                      *value_ptr = PNULL;
    MMI_CTRL_ID_T               ctrl_id = 0;
    MMI_CONTROL_CREATE_T        ctrl_create = {0};
    GUIOWNDRAW_INIT_DATA_T      init_data = {0};
    GUIOWNDRAW_CALL_BACK_FUNC   OwnDrawFunc = PNULL;

    value_ptr = *value_pptr;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);
    OwnDrawFunc = (GUIOWNDRAW_CALL_BACK_FUNC)(*value_ptr++);

    //set init data
    init_data.both_rect   = MMITHEME_GetWinClientBothRect(win_handle);
    init_data.OwnDrawFunc = OwnDrawFunc;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_OWNDRAW_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the toolbar control
//	Global resource dependence : 
//  Author: James.Zhang
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateToolbarCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    BOOLEAN                     result = TRUE;
    uint32                      *value_ptr = PNULL;
    MMI_CTRL_ID_T               ctrl_id = 0;
    MMI_CONTROL_CREATE_T        ctrl_create = {0};
    GUITOOLBAR_INIT_DATA_T      init_data = {0};

    value_ptr = *value_pptr;
    ctrl_id   = (MMI_CTRL_ID_T)(*value_ptr++);

    init_data.both_rect = MMITHEME_GetWinClientBothRect(win_handle);

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_TOOLBAR_ID;
    ctrl_create.parent_win_handle = win_handle;
    ctrl_create.init_data_ptr     = &init_data;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CreateTitleCtrl(
                                MMI_HANDLE_T win_handle,
                                uint32**      value_pptr
                                )
{
    uint32*  value_ptr   = PNULL;

	MMI_CONTROL_CREATE_T	create = {0};
	GUITITLE_INIT_DATA_T	init_data = {0};
	void	*ctrl_ptr = PNULL;
	MMI_CTRL_ID_T	ctrl_id = 0;
	MMI_TEXT_ID_T	text_id = TXT_NULL;

    value_ptr = *value_pptr;

	text_id		= *value_ptr++;

	init_data.text_id	= text_id;

	ctrl_id = MMITHEME_GetTitleCtrlId();
	create.ctrl_id = ctrl_id;
	create.guid    = SPRD_GUI_TITLE_ID;
	create.parent_win_handle = win_handle;
	create.init_data_ptr = &init_data;

	ctrl_ptr = MMK_CreateControl( &create );

	MMK_SetWinTitleCtrlId(win_handle, ctrl_id);

    *value_pptr = value_ptr;
    
    return (TRUE);
}

/*****************************************************************************/
// 	Description : set the window style
//	Global resource dependence : 
//  Author:Robert.Lu
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN SetTitleStyle(
                            MMI_HANDLE_T win_handle,
                            uint32**      value_pptr
                            )
{
    uint32*  value_ptr   = PNULL;

    value_ptr = *value_pptr;

	s_default_ctrl_param.title_text_id = *value_ptr++;

    // 只设标志不创建，后续会创建
    MMK_SetWinDisplayStyleState( win_handle, WS_HAS_TITLE, TRUE );

    *value_pptr = value_ptr;
    
    return (TRUE);
}

//added by andrew.zhang at 2009/03/24
/*****************************************************************************/
// 	Description : create the phone number editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditPhonenumCtrl(
                                        MMI_HANDLE_T win_handle,
                                        uint32**      value_pptr
                                        )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = GUIEDIT_TYPE_PHONENUM;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}
/*****************************************************************************/
// 	Description : create the digital editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditDigitalCtrl(
                                       MMI_HANDLE_T win_handle,
                                       uint32**      value_pptr
                                       )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = GUIEDIT_TYPE_DIGITAL;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the password editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditPasswordCtrl(
                                        MMI_HANDLE_T win_handle,
                                        uint32**      value_pptr
                                        )
{
    BOOLEAN                 result = TRUE;
    uint16                  str_max_len = 0;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;

    str_max_len = (uint16)(*value_ptr++);

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = GUIEDIT_TYPE_PASSWORD;
    init_data.str_max_len = str_max_len;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the date editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditDateCtrl(
                                    MMI_HANDLE_T win_handle,
                                    uint32**      value_pptr
                                    )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = GUIEDIT_TYPE_DATE;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

/*****************************************************************************/
// 	Description : create the time editbox control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateEditTimeCtrl(
                                    MMI_HANDLE_T win_handle,
                                    uint32**      value_pptr
                                    )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIEDIT_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;

    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);

    //set init data
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.type = GUIEDIT_TYPE_TIME;

    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_EDITBOX_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}


/*****************************************************************************/
// 	Description : create the animation control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateAnimCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**     value_pptr
                             )
{
    void                    *ctrl_ptr = PNULL;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_WIN_ID_T            win_id = 0;
    MMI_CTRL_ID_T           ctrl_id = 0;
    MMI_CONTROL_CREATE_T    create = {0};
    GUIANIM_INIT_DATA_T     init_data = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);
    win_id      = (MMI_WIN_ID_T)(*value_ptr++);

    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_ANIM_ID;
    create.parent_win_handle = win_handle;    
    create.init_data_ptr = &init_data;
    
    ctrl_ptr = MMK_CreateControl( &create );

    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create the BUTTON control
//	Global resource dependence : 
//  Author:YELIANNA
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateButtonCtrl( MMI_HANDLE_T win_handle,   uint32**      value_pptr  )
{
    uint32*                   value_ptr = PNULL;
    GUI_RECT_T             rect = {0};
    uint32                  align = 0;
    MMI_IMAGE_ID_T          bg_pressed_image_id = IMAGE_NULL;
    MMI_IMAGE_ID_T          bg_released_image_id = IMAGE_NULL;
    MMI_IMAGE_ID_T          grayed_fg_image_id = IMAGE_NULL;
    MMI_CTRL_ID_T          ctrl_id   = 0;
    MMI_CONTROL_CREATE_T    create = {0};
    GUIBUTTON_INIT_DATA_T   init_data = {0};
    void* ctrl_ptr = PNULL;
    
    value_ptr = *value_pptr;
    
    rect.left = *value_ptr++;
    rect.top  = *value_ptr++;
    rect.right = *value_ptr++;
    rect.bottom = *value_ptr++;
    align = (uint32)(*value_ptr++);
    bg_released_image_id = (MMI_IMAGE_ID_T)(*value_ptr++);
    bg_pressed_image_id = (MMI_IMAGE_ID_T)(*value_ptr++);
    grayed_fg_image_id = (MMI_IMAGE_ID_T)(*value_ptr++);
    
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);

    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.bg.bg_type = GUI_BG_IMG;
    init_data.bg.img_id = bg_released_image_id;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_BUTTON_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );

//     GUIBUTTON_Init(
//         ctrl_ptr,
//         &rect,
//         bg_released_image_id,
//         bg_pressed_image_id,
//         grayed_fg_image_id,
//         align );
    
    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create the dropdownlist control
//	Global resource dependence : 
//  Author:YELIANNA
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateDropDownListCtrl( MMI_HANDLE_T win_handle,   uint32**      value_pptr  )
{
    uint32*                value_ptr = PNULL;
    uint16                 page_item_num = 0;
    GUI_RECT_T             rect = {0};
    MMI_CTRL_ID_T          ctrl_id   = 0;
    MMI_CONTROL_CREATE_T   create = {0};
    GUIDROPDOWNLIST_INIT_DATA_T init_data = {0};
    void*                  ctrl_ptr = PNULL;
    
    value_ptr = *value_pptr;
    
    rect.left = *value_ptr++;
    rect.top  = *value_ptr++;
    rect.right = *value_ptr++;
    rect.bottom = *value_ptr++;
    page_item_num = *value_ptr++;
        
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
    init_data.page_item_num = page_item_num;
    init_data.show_type = GUIDNLIST_SHOW_AUTOSET;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_DROPDOWNLIST_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );

/*    GUIDROPDOWNLIST_Init( ctrl_ptr, &rect, page_item_num );*/
    
    *value_pptr = value_ptr;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create the icon list control
//	Global resource dependence : 
//  Author: jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateIconListCtrl(
                                 MMI_HANDLE_T win_handle,
                                 uint32       **value_pptr
                                 )
{
    void                    *ctrl_ptr = PNULL;
    uint32                  *value_ptr = PNULL;
    GUI_RECT_T              rect = {0};
    MMI_CTRL_ID_T           ctrl_id = 0;
    MMI_CONTROL_CREATE_T    create = {0};
    GUIICONLIST_INIT_DATA_T init_data = {0};

    value_ptr = *value_pptr;
    
    rect.left   = *value_ptr++;
    rect.top    = *value_ptr++;
    rect.right  = *value_ptr++;
    rect.bottom = *value_ptr++;
    ctrl_id     = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_ICONLIST_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    ctrl_ptr = MMK_CreateControl( &create );
    
//     //init anim
//     GUIICONLIST_Init( ctrl_ptr, rect );
    
    *value_pptr = value_ptr;
    
    return TRUE;
}


/*****************************************************************************/
// 	Description : create the menu control
//	Global resource dependence : 
//  Author:Great.Tian
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreateMenuCtrl(
                             MMI_HANDLE_T win_handle,
                             uint32**      value_pptr
                             )
{
    uint32*          value_ptr = PNULL;
    GUI_RECT_T       rect = {0};
    uint32           group_id = 0;
    MMI_CTRL_ID_T    ctrl_id   = 0;
    MMI_CONTROL_CREATE_T create = {0};
    GUIMENU_INIT_DATA_T init_data = {0};
    IGUICTRL_T*          ctrl_ptr  = PNULL;
    
    value_ptr = *value_pptr;
    
    rect.left = *value_ptr++;
    rect.top  = *value_ptr++;
    rect.right = *value_ptr++;
    rect.bottom = *value_ptr++;
    
    group_id = *value_ptr++; // 这是一个uint32赋给uint16的过程，可能有问题，注意Debug
    ctrl_id = (MMI_CTRL_ID_T)(*value_ptr++);
    
    if ( MENU_DYNA == group_id )
    {
        init_data.is_static = FALSE;
        init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
        /*        GUIMENU_InitDynamic( ctrl_ptr, rect );*/
    }
    else
    {
        init_data.is_static = TRUE;
        init_data.both_rect.h_rect = init_data.both_rect.v_rect = rect;
        init_data.group_id = group_id;
        /*        GUIMENU_InitStatic( ctrl_ptr, rect, group_id );*/
    }

    create.ctrl_id = ctrl_id;
    create.guid    = SPRD_GUI_MENU_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;
    
    ctrl_ptr = MMK_CreateControl( &create );
    
    *value_pptr = value_ptr;
    
    return TRUE;
}

/*****************************************************************************/
// 	Description : create the pop_up menu control
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN CAFCreatePopmenuCtrl(
                                MMI_HANDLE_T    win_handle,
                                uint32          **value_pptr
                                )
{
    BOOLEAN                 result = TRUE;
    uint32                  *value_ptr = PNULL;
    uint32                  group_id = 0;
    MMI_CTRL_ID_T           ctrl_id = 0;
    GUIMENU_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    value_ptr = *value_pptr;

	//只支持动态POPMENU
    group_id     = MENU_DYNA;
    ctrl_id      = (MMI_CTRL_ID_T)(*value_ptr++);
    
    init_data.is_static = FALSE;
    init_data.menu_style = GUIMENU_STYLE_POPUP;
    
    //set control param
    ctrl_create.ctrl_id           = ctrl_id;
    ctrl_create.guid              = SPRD_GUI_MENU_ID;
    ctrl_create.init_data_ptr     = &init_data;
    ctrl_create.parent_win_handle = win_handle;

    //creat control
    MMK_CreateControl(&ctrl_create);

    *value_pptr = value_ptr;

    return (result);
}

//end
