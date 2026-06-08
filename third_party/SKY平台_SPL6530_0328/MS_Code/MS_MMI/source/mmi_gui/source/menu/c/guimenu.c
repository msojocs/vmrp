/*****************************************************************************
** File Name:      guimenu.c                                                 *
** Author:                                                                   *
** Date:           04/20/2004                                                *
** Copyright:      2004 Spreadtrum, Incoporated. All Rights Reserved.         *
** Description:    This file is used to describe punctuation                 *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2006      Jassmine              Creat
** 04/2010      hua.fang        add function: SetItemVisible/SetNodeVisible
**                              SetItemGrayed
******************************************************************************/

#ifndef _GUIMENU_C_
#define _GUIMENU_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_gui_trc.h"
#include "mmk_type.h"
#include "mmk_timer.h"
#include "guimenu.h"
#include "guimenu_internal.h"
#include "mmi_theme.h"
#include "cafctrlmenu.h"
#include "cafctrlpopmenu.h"
#include "guilcd.h"
#include "mmi_theme.h"
#include "mmitheme_special.h"
#include "mmi_default.h"
//#include "mmitheme_softkey.h"
#include "mmk_tp.h"

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
// the item info address is offset to group info which owns items.
// _group_info_ptr is group info address, and item info is sizeof(GUIMENU_GROUP_INFO_T)
// behind group info.
#define GET_ITEM_INFO(_group_info_ptr) (GUIMENU_ITEM_INFO_T*)((uint32)(_group_info_ptr) + sizeof(GUIMENU_GROUP_INFO_T));

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
LOCAL GUIMENU_NUMKEY_MAP_T  s_guimenu_numkey_map[] = {
                                                      0, MSG_APP_1,
                                                      1, MSG_APP_2,
                                                      2, MSG_APP_3,
                                                      3, MSG_APP_4,
                                                      4, MSG_APP_5,
                                                      5, MSG_APP_6,
                                                      6, MSG_APP_7,
                                                      7, MSG_APP_8,
                                                      8, MSG_APP_9,
                                                      9, MSG_APP_STAR,
                                                      10,MSG_APP_0,
                                                      11,MSG_APP_HASH
                                                      };

/*---------------------------------------------------------------------------*/
/*                         TYPE AND CONSTANT                                 */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         EXTERNAL DEFINITION                              *
 **--------------------------------------------------------------------------*/

/*lint -e526*/
// extern GUIMENU_ITEM_T *MMIICONMENU_GetItemPtr(GUIMENU_CTRL_T *menu_ctrl_ptr, uint16 item_index);

// extern GUIMENULIST_ITEM_INFO_T *MMIICONMENU_GetItemInfoPtr(GUIMENU_CTRL_T *menu_ctrl_ptr, uint16 item_index);

// extern GUIMENU_ITEM_T *MMIOPTMENU_GetItemPtr(GUIMENU_CTRL_T *menu_ctrl_ptr, uint16 page_index, uint16 item_index);  /*lint !e526*/

// extern GUIMENULIST_ITEM_INFO_T *MMIOPTMENU_GetItemInfoPtr(GUIMENU_CTRL_T *menu_ctrl_ptr, uint16 page_index, uint16 item_index); /*lint !e526*/

// extern uint16 MMIOPTMENU_GetTotalItemNum(GUIMENU_CTRL_T *menu_ctrl_ptr, uint16 page_index);  /*lint !e526*/
/*lint +e526*/

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : set state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void SetState( 
                    uint32* dst_state_ptr,
                    uint32  src_state,
                    BOOLEAN is_true
                    );

/*****************************************************************************/
//  Description : get state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetState(
                       uint32 dst_state,
                       uint32 src_state
                       );

/*****************************************************************************/
//  Description : menu construct 
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MenuConstruct(
                            IGUICTRL_T  *iguictrl_ptr,
                            void        *init_data_ptr
                            );

/*****************************************************************************/
//  Description : init static menu control
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void InitStaticMenu(
                          IGUICTRL_T            *ctrl_ptr,  //control pointer
                          GUI_BOTH_RECT_T       both_rect,  //menu rect
                          MMI_MENU_GROUP_ID_T   group_id    //menu group id
                          );

/*****************************************************************************/
//  Description : init dynamic menu control
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void InitDynamicMenu(
                           IGUICTRL_T       *ctrl_ptr,      //control pointer
                           uint16           node_max_num,   //node max number
                           GUI_BOTH_RECT_T  both_rect,      //menu rect
                           GUIMENU_STYLE_E  menu_style      //menu style
                           );

/*****************************************************************************/
//  Description : alloc memory for dynamic menu tree
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void AllocDynamicMenuMemory(
                                  uint16                max_node_num,   //max node number
                                  GUIMENU_NODE_MEMORY_T *node_mem_ptr   //dynamic menu node memory pointer
                                  );

/*****************************************************************************/
//  Description : creat one new node
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* CreatNewNode(
                                   GUIMENU_NODE_MEMORY_T    *node_mem_ptr
                                   );

/*****************************************************************************/
// 	Description : handle menu msg function
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E MenuHandleMsg(
                                 IGUICTRL_T         *ctrl_ptr,  //control pointer
                                 MMI_MESSAGE_ID_E   msg_id,  	//message
                                 DPARAM             param       //add data
                                 );

/*****************************************************************************/
//  Description : calculate pop-up menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL void CalculatePopupRect(
                              uint16            rect_top,       //only for sub pop-up menu
                              GUIMENU_CTRL_T    *menu_ctrl_ptr
                              );

/*****************************************************************************/
//  Description : update previous pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:  当前是pop-up menu,lose focus后get focus需要刷新之前所有的pop-up menu
//         当前是pop-up menu,cancel后需要根据Rect判断是否刷新之前的pop-up menu
/*****************************************************************************/
LOCAL void UpdatePrevPopMenu(
                             uint16         start_level,
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             );

/*****************************************************************************/
//  Description : set menu info,include menu level,current and first index etc.
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void SetMenuInfo(
                       uint16               menu_level,
                       uint16               first_item_index,
                       uint16               cur_item_index,
                       GUI_RECT_T           menu_rect,
                       int16                offset_y,
                       MMI_MENU_GROUP_ID_T  cur_group_id,
                       GUIMENU_NODE_T       *cur_parent_node_ptr,
                       GUIMENU_CTRL_T       *menu_ctrl_ptr
                       );

/*****************************************************************************/
//  Description : HandleMenuOkKey
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleMenuOkKey(
                              GUIMENU_CTRL_T    *menu_ctrl_ptr,
                              MMI_MESSAGE_ID_E  msg_id
                              );

/*****************************************************************************/
//  Description : get current group id
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu style
/*****************************************************************************/
LOCAL MMI_MENU_GROUP_ID_T GetCurGroupId(
                                        GUIMENU_CTRL_T  *menu_ctrl_ptr
                                        );

/*****************************************************************************/
//  Description : get current item pointer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetCurItem(
                         GUIMENU_CTRL_T  *menu_ctrl_ptr,     //in
                         GUIMENU_ITEM_T  *item_ptr,          //in
                         GUIMENU_ITEM_INFO_T *item_info_ptr  //out
                         );

/*****************************************************************************/
//  Description : get item information
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetInfoInOK(
                          GUIMENU_CTRL_T        *menu_ctrl_ptr,
                          BOOLEAN               *is_gray_ptr,
                          BOOLEAN               *is_exist_child_ptr,
                          GUIMENU_ITEM_T        *cur_item_ptr,
                          GUIMENU_NODE_T        **cur_node_pptr
                          );

/*****************************************************************************/
//  Description : restore menu info in ok
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void RestoreInfoInOK(
                           GUIMENU_CTRL_T        *menu_ctrl_ptr,
                           GUIMENU_ITEM_T        *cur_item_ptr,
                           GUIMENU_NODE_T        *cur_node_ptr
                           );

/*****************************************************************************/
//  Description : get current item pointer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void GetCurItemPtr(
						 GUIMENU_CTRL_T  *menu_ctrl_ptr,     //in
						 GUIMENU_ITEM_INFO_T *item_info_ptr,  //out
						 GUIMENU_MAINMENU_INFO_T *cur_item_ptr
						 );

/*****************************************************************************/
//  Description : notify parent control or parent window notify message
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E NotifyParentMsg(
                                   CAF_HANDLE_T     ctrl_handle,
                                   MMI_MESSAGE_ID_E msg_id
                                   );

/*****************************************************************************/
//  Description : push menu in stack
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void PushMenuStack(
                         GUIMENU_CTRL_T     *menu_ctrl_ptr 
                         );

/*****************************************************************************/
//  Description : pop menu from stack
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void PopMenuStack(
                        GUIMENU_CTRL_T      *menu_ctrl_ptr 
                        );

/*****************************************************************************/
//  Description : reset menu, stop timer and free resource etc.
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void ResetMenuInfo(
                         GUIMENU_CTRL_T *menu_ctrl_ptr
                         );

/*****************************************************************************/
//  Description : handle menu cancel key
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleMenuCancelKey(
                                  GUIMENU_CTRL_T    *menu_ctrl_ptr,
                                  MMI_MESSAGE_ID_E  msg_id
                                  );

/*****************************************************************************/
//  Description : 重置经过cancel后menu数据
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL void RestoreInfoInCancel(
                               GUIMENU_CTRL_T    *menu_ctrl_ptr
                               );

/*****************************************************************************/
//  Description : is update previous pop-up and return update start menu level
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 当前是pop-up menu,cancel后需要根据Rect判断是否刷新之前的pop-up menu
/*****************************************************************************/
LOCAL BOOLEAN IsUpdatePrevPopMenu(
                                  uint16            *level_ptr,     //in
                                  GUIMENU_CTRL_T    *menu_ctrl_ptr  //in
                                  );

/*****************************************************************************/
//  Description : handle menu direction key
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMenuDirectionKey(
                                          GUIMENU_CTRL_T    *menu_ctrl_ptr,
                                          MMI_MESSAGE_ID_E  msg_id
                                          );

/*****************************************************************************/
//  Description : handle menu number key
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleMenuNumKey(
                               GUIMENU_CTRL_T   *menu_ctrl_ptr,
                               MMI_MESSAGE_ID_E msg_id
                               );

/*****************************************************************************/
//  Description : handle menu tp down msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpDown(
                            DPARAM          param,
                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                            );

/*****************************************************************************/
//  Description : handle menu tp up msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpUp(
                          DPARAM            param,
                          GUIMENU_CTRL_T    *menu_ctrl_ptr
                          );

/*****************************************************************************/
//  Description : handle menu tp move msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpMove(
                            DPARAM          param,
                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                            );

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void StopItemTextTimer(
                             GUIMENU_CTRL_T     *menu_ctrl_ptr
                             );

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartItemTextTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              );

/*****************************************************************************/
//  Description : handle menu scroll bar touch panel msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpScroll(
                              uint16            first_item_index,
                              GUIMENU_CTRL_T    *menu_ctrl_ptr
                              );

/*****************************************************************************/
//  Description : get menu pointer by control id
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_CTRL_T* GetMenuPtr(
                                 MMI_HANDLE_T   ctrl_handle
                                 );

/*****************************************************************************/
//  Description : menu type of
//  Global resource dependence : 
//  Author:Jassmine
//  Note: judge control type of
/*****************************************************************************/
LOCAL BOOLEAN MenuTypeOf(
                         IGUICTRL_T     *menu_ptr
                         );

/*****************************************************************************/
//  Description : find node pointer by node id
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* FindNodeById(
                                   uint32           node_id,        //node id
                                   GUIMENU_NODE_T   *root_node_ptr  //root node pointer
                                   );

/*****************************************************************************/
//  Description : insert new node in dynamic menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void InsertNewNode(
                         uint16             new_node_index, //new node index,from o
                         GUIMENU_NODE_T     *new_node_ptr,  //new node pointer
                         GUIMENU_NODE_T     *parent_node_ptr//parent node pointer
                         );

/*****************************************************************************/
//  Description : selected item is exist for radio/check pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_POP_SEL_ITEM_T* IsExistSelItem(
                                             GUIMENU_POP_SEL_INFO_T     *selected_info_ptr, //in
                                             GUIMENU_POP_SELECT_LIST_T  *selected_list_ptr  //in
                                             );

/*****************************************************************************/
//  Description : creat one new selected item
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_POP_SEL_ITEM_T* CreatNewSelItem(
                                              GUIMENU_POP_SEL_INFO_T    *selected_info_ptr
                                              );

/*****************************************************************************/
//  Description : add one new selected item to list
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void AddSelItemToList(
                            GUIMENU_POP_SEL_ITEM_T      *sel_item_ptr,      //in
                            GUIMENU_POP_SELECT_LIST_T   *selected_list_ptr  //in
                            );

/*****************************************************************************/
//  Description : delete one selected item from list
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void DeleteSelItem(
                         GUIMENU_POP_SEL_ITEM_T     *sel_item_ptr,      //in
                         GUIMENU_POP_SELECT_LIST_T  *selected_list_ptr  //in
                         );

/*****************************************************************************/
//  Description : free all selected item memory
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void FreeSelListMemory(
                             GUIMENU_CTRL_T     *menu_ctrl_ptr
                             );

/*****************************************************************************/
//  Description : destruct
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MenuDestruct(
                           IGUICTRL_T   *iguictrl_ptr
                           );

/*****************************************************************************/
//  Description : free all string buffer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* FreeStrBuffer(
                                    GUIMENU_NODE_T   *root_node_ptr  //root node pointer
                                    );

/*****************************************************************************/
//  Description : set menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MenuSetRect(
                          IGUICTRL_T        *ctrl_ptr,
                          const CAF_RECT_T  *rect_ptr
                          );

/*****************************************************************************/
//  Description : modify menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL void ModifyMenuRect(
                          GUIMENU_CTRL_T    *menu_ctrl_ptr
                          );

/*****************************************************************************/
//  Description : update previous second menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL void SecondMenuSetRect(
                             GUIMENU_CTRL_T     *menu_ctrl_ptr
                             );

/*****************************************************************************/
//  Description : update previous pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:  当前是pop-up menu,旋转后需要刷新之前所有的pop-up menu
/*****************************************************************************/
LOCAL void PopMenuSetRect(
                          GUIMENU_CTRL_T    *menu_ctrl_ptr
                          );

/*****************************************************************************/
//  Description : get menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MenuGetRect(
                          IGUICTRL_T    *iguictrl_ptr, 
                          CAF_RECT_T    *rect_ptr
                          );

/*****************************************************************************/
//  Description : init group info and item info for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN InitStaticMenuInfo(
                                 GUIMENU_CTRL_T      *menu_ctrl_ptr, //[in]
                                 MMI_MENU_GROUP_ID_T group_id        //[in]
                                 );

/*****************************************************************************/
//  Description : set all menu info, include group info and item info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL GUIMENU_GROUP_INFO_T* SetStaticMenuInfo(                                      // [ret] point to next group info
                                              GUIMENU_GROUP_INFO_T *group_info_ptr, // [in] set in this group info if it has group
                                              MMI_MENU_GROUP_ID_T group_id,         // [in]
                                              uint32 *remain_group_count_ptr        // [in:out] the remaining group count
                                              );

/*****************************************************************************/
//  Description : calculate the size which group info and item info want
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL uint32 CalcStaticMenuInfoSize(                                // [ret] the size
                                    MMI_MENU_GROUP_ID_T group_id,   // [in] current group id
                                    uint32 *group_count_ptr         // [out] how many group in static menu
                                    );

/*****************************************************************************/
//  Description : get item
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu.
//        if it is option page style, get the item in page.
/*****************************************************************************/
LOCAL GUIMENU_ITEM_T* GetItemByVisibleIndex(
                                            GUIMENU_CTRL_T *menu_ctrl_ptr,  //[in]
                                            uint16 visible_index,           //[in]
                                            GUIMENU_ITEM_INFO_T *out_item_info_ptr // [out]
                                            );

/*****************************************************************************/
//  Description : get node pointer by visible index
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* GetNodeByVisibleIndex(
                                            uint16          node_index,      //[in]
                                            GUIMENU_NODE_T  *parent_node_ptr //[in]
                                            );

/*****************************************************************************/
//  Description : get group info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu
/*****************************************************************************/
LOCAL GUIMENU_GROUP_INFO_T* GetGroupInfo(
                                         GUIMENU_CTRL_T *menu_ctrl_ptr, //[in]
                                         MMI_MENU_GROUP_ID_T group_id   //[in]
                                         );

/*****************************************************************************/
//  Description : get group info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu
/*****************************************************************************/
LOCAL void FreeGroupInfoBuffer(
                               GUIMENU_GROUP_INFO_T *root_group_info_ptr //[in]
                               );

/*****************************************************************************/
//  Description : get group id by specified option page index
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL MMI_MENU_GROUP_ID_T GetGroupIdInOptionPage(
                                                 GUIMENU_CTRL_T *menu_ctrl_ptr,     //[in]
                                                 uint16         option_page_index   //[in] visible option page index
                                                 );

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartAutoMoveTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              );

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopAutoMoveTimer(
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             );

/*****************************************************************************/
//  Description : start cube back timer
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void StartCubeBackTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              );

/*****************************************************************************/
//  Description : stop cube back timer
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void StopCubeBackTimer(
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             );

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartMoveStartTimer(
                               GUIMENU_CTRL_T *menu_ctrl_ptr
                               );

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopMoveStartTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              );

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartShakeIconTimer(
                                GUIMENU_CTRL_T *menu_ctrl_ptr
                                );

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopShakeIconTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              );


/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartTitleIconDispTimer(
                                   GUIMENU_CTRL_T *menu_ctrl_ptr
                                   );

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopTitleIconDispTimer(
                                  GUIMENU_CTRL_T *menu_ctrl_ptr
                                  );

/*****************************************************************************/
//  Description : replace string
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void ReplaceString(
                         MMI_STRING_T     *dst_str_ptr,
                         MMI_STRING_T     *src_str_ptr
                         );

/*****************************************************************************/
//  Description : check button style
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void CheckButtonStyle(
                            GUIMENU_CTRL_T *menu_ctrl_ptr
                            );

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
/*****************************************************************************/
//  Description : check sub menu style
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void CheckSubMenuStyle(
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             );
#endif

#ifdef DYNAMIC_MODULE_SUPPORT    
//added by andrew.zhang at 2009/07/06
/*****************************************************************************/
//  Description:   insert node
//  Author: Andrew.Zhang
//  Param
//      me_ptr :control object
//		nodex_index :node index,from 0
//		node_id :node id,from 1
//		parent_node_id :parent node id
//		node_item_ptr :node item
//  Return:
//       TRUE:success;FALSE:fail
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CAFMENU_InsertNode( 
                                 IGUIMENU_T *me_ptr, 
                                 uint16 nodex_index, 
                                 uint32 node_id,
                                 uint32 parent_node_id, 
                                 CAF_GUIMENU_DYNA_ITEM_T *node_item_ptr 
                                 );

/*****************************************************************************/
//  Description:   handle control event
//  Author: Andrew.Zhang
//  Param
//       me_ptr :control object
//  Return:
//       current node id
//  Note: 
/*****************************************************************************/
LOCAL uint32 CAFMENU_GetCurNodeId( 
                                  IGUIMENU_T *me_ptr 
                                  );

/*****************************************************************************/
//  Description:   set menu background color
//  Author: Andrew.Zhang
//  Param
//       me_ptr :control object
//       bg_color :background color
//  Return:
//       TRUE:success;FALSE:fail
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CAFMENU_SetBgColor( 
                                 IGUIMENU_T *me_ptr, 
                                 CAF_COLOR_T bg_color 
                                 );


//end
#endif

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: violent.wei
//  Note: 当有focus item变化时，朗读list内容
/*****************************************************************************/
LOCAL void HandleTextToSpeech(
                           GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                           );

/**--------------------------------------------------------------------------*
 **                         GLOBAL DEFINITION                                *
 **--------------------------------------------------------------------------*/
/*lint -e18*/
const CAF_VTBL_T(IGUIMENU_T) g_guimenu_vtbl =
{
    GUICTRL_QueryInterface,
    GUICTRL_AddRef,
    GUICTRL_Release,
    MenuConstruct,
    MenuDestruct,
    MenuHandleMsg,
    GUICTRL_GetCtrlGuid,
    GUICTRL_SetCtrlHandle,
    GUICTRL_GetCtrlHandle,
    GUICTRL_SetState,
    GUICTRL_GetState,
    MenuSetRect,
    MenuGetRect,
    GUICTRL_SetLcdDevInfo,
    GUICTRL_GetLcdDevInfo,
    //新增基类接口
    GUICTRL_SetProperties,
    GUICTRL_GetProperties,
    GUICTRL_TestProperties,
    GUICTRL_Redraw,
    GUICTRL_SetActive,
    GUICTRL_IsActive,
    GUICTRL_SetBorder,
    GUICTRL_SetBg,
    GUICTRL_SetFont,
    GUICTRL_GetFont,
    GUICTRL_SetProgress,
    GUICTRL_SetCircularHandleUpDown,
    GUICTRL_SetCircularHandleLeftRight,
    GUICTRL_GetHeightByWidth,
    GUICTRL_GetDisplayRect,
    GUICTRL_SetDisplayRect,
    GUICTRL_GetSelectRect,
    GUICTRL_GetSelectIndex,
    GUICTRL_GetBothRect,
    GUICTRL_SetBothRect,

    //其它接口
#ifdef DYNAMIC_MODULE_SUPPORT        
    CAFMENU_InsertNode,
    CAFMENU_GetCurNodeId,
    CAFMENU_SetBgColor
#else    
    PNULL,
    PNULL,    
    PNULL    
#endif
};
/*lint +e18*/

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/*lint -e737*/
/*****************************************************************************/
//  Description : menu construct 
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MenuConstruct(
                            IGUICTRL_T  *iguictrl_ptr,
                            void        *init_data_ptr
                            )
{
    BOOLEAN                 result = TRUE;
    GUIMENU_INIT_DATA_T     *menu_init_ptr = (GUIMENU_INIT_DATA_T*)init_data_ptr;
    GUIMENU_CTRL_T          *menu_ctrl_ptr = (GUIMENU_CTRL_T*)iguictrl_ptr;

    if (PNULL == iguictrl_ptr || PNULL == init_data_ptr)
    {
        return FALSE;
    }
    
    if (menu_init_ptr->is_static)
    {
        //init static menu
        InitStaticMenu(iguictrl_ptr,
            menu_init_ptr->both_rect,
            menu_init_ptr->group_id);
    }
    else
    {
        //init dynamic menu
        InitDynamicMenu(iguictrl_ptr,
            menu_init_ptr->node_max_num,
            menu_init_ptr->both_rect,
            menu_init_ptr->menu_style);
    }

    //set state
    IGUICTRL_SetState(iguictrl_ptr,GUICTRL_STATE_MENU,TRUE);

    menu_ctrl_ptr->is_need_reset_rect = TRUE;

    CheckButtonStyle(menu_ctrl_ptr);

    if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
    {
        //set transparent
        MMK_SetWinDisplayStyleState( menu_ctrl_ptr->win_handle, WS_HAS_TRANSPARENT|WS_POPMENU_WIN, TRUE );

        //no animation
        MMK_SetWinMoveStyle(menu_ctrl_ptr->win_handle,MOVE_NOT_FULL_SCREEN_WINDOW);

// TODO: PDA将动画禁止
#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
        MMK_SetWinMoveStyle(menu_ctrl_ptr->win_handle,MOVE_FORBIDDEN);
#endif
    }
    
	// 默认时窗口title由控件控制
	SetState(&menu_ctrl_ptr->menu_state, GUIMENU_STATE_CUSTOMER_TITLE, FALSE);

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
    // 按象素滚动
    SetState(&menu_ctrl_ptr->menu_state, GUIMENU_STATE_PRG_PIXEL_SCROLL, TRUE);
    SetState(&menu_ctrl_ptr->menu_state, GUIMENU_STATE_DRAW_SPLIT_LINE, TRUE);
#endif

    return (result);
}

/*****************************************************************************/
//  Description : init static menu control
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void InitStaticMenu(
                          IGUICTRL_T            *ctrl_ptr,  //control pointer
                          GUI_BOTH_RECT_T       both_rect,  //menu rect
                          MMI_MENU_GROUP_ID_T   group_id    //menu group id
                          )
{   
    GUIMENU_CTRL_T  *menu_ctrl_ptr = (GUIMENU_CTRL_T*)ctrl_ptr;
//    MMI_HANDLE_T win_handle = 0;

    //get menu process function
    MMITHEME_GetMenuProc(menu_ctrl_ptr->process_func);

    //get menu theme
    MMITHEME_GetCommonMenuTheme(&menu_ctrl_ptr->common_theme);
    MMITHEME_GetMainMenuTheme(&menu_ctrl_ptr->main_menu_theme);
    MMITHEME_GetSecondMenuTheme(&menu_ctrl_ptr->second_menu_theme);
    MMITHEME_GetPopMenuTheme(&menu_ctrl_ptr->pop_menu_theme);
    MMITHEME_GetOptionsMenuTheme(&menu_ctrl_ptr->options_menu_theme);

    InitStaticMenuInfo(menu_ctrl_ptr, group_id);

    menu_ctrl_ptr->lcd_dev_info = menu_ctrl_ptr->common_theme.lcd_dev;

    //set menu group
    menu_ctrl_ptr->cur_group_id  = group_id;
    menu_ctrl_ptr->cur_group_ptr = (GUIMENU_GROUP_T*)MMITHEME_GetMenuGroup(group_id);
    menu_ctrl_ptr->cur_group_info_ptr = menu_ctrl_ptr->root_group_info_ptr;
    menu_ctrl_ptr->item_total_num = menu_ctrl_ptr->cur_group_info_ptr->visible_child_item_num;

    //set other param
    menu_ctrl_ptr->is_static  = TRUE;
    menu_ctrl_ptr->both_rect  = both_rect;
    menu_ctrl_ptr->rect       = both_rect.v_rect;
    menu_ctrl_ptr->cur_style  = menu_ctrl_ptr->cur_group_ptr->menu_style;
//    win_handle = sizeof(GUIMENU_CTRL_T);
//    menu_ctrl_ptr->win_handle = 5;
//    menu_ctrl_ptr->pre_msg_loop_menu_level = 7;
//    menu_ctrl_ptr->win_handle = win_handle;
    menu_ctrl_ptr->win_handle = MMK_GetWinHandleByCtrl(menu_ctrl_ptr->handle);

 // PDA项目，将POPUP menu定义为OptionsMenu
#ifdef GUIPOPMENU_OPTIONS_CONTEXT_SUPPORT
    if (GUIMENU_STYLE_POPUP == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->cur_style = GUIMENU_STYLE_OPTIONS;
    }
#endif
#if defined(PDA_UI_SUPPORT_MANIMENU_GO)
    //默认支持滑动的方向为X，其他另行处理    
    menu_ctrl_ptr->support_slide_type = GO_SLIDE_SUPPORT_ONLY_X;//默认为X方向 
#endif	

    // 是否移动状态，目前主菜单才有效
    menu_ctrl_ptr->is_move_state = FALSE;

    // 默认支持滚动条
    menu_ctrl_ptr->is_need_prgbox = TRUE;

    // 增加滚动条时，是否需要动态调整区域
    menu_ctrl_ptr->is_revise_rect = FALSE;

    menu_ctrl_ptr->max_horz_width = (uint16)-1;
    menu_ctrl_ptr->max_vert_width = (uint16)-1;

    //set icon menu current index
    if (GUIMENU_STYLE_ICON == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_TITLE_ICON == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->enter_anim_type = GUIMENU_ENTER_ANIM_SLIDE_ICON;
        menu_ctrl_ptr->cur_item_index = menu_ctrl_ptr->main_menu_theme.default_item_index;
        MMITHEME_GetIconMenuInfo( menu_ctrl_ptr->win_handle, &menu_ctrl_ptr->main_menu_data.icon_menu_data, menu_ctrl_ptr->cur_style);
    }
    else if (GUIMENU_STYLE_OPTION_PAGE == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->enter_anim_type = GUIMENU_ENTER_ANIM_SLIDE_ICON;
        MMITHEME_GetOptMenuInfo(&menu_ctrl_ptr->main_menu_data.opt_menu_data);
    }
    else if (GUIMENU_STYLE_CUBE == menu_ctrl_ptr->cur_style
        #ifndef PDA_UI_SUPPORT_MANIMENU_GO
        || GUIMENU_STYLE_SLIDE_PAGE == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_SPHERE == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_CYLINDER == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_TRAPEZOID == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_BINARY_STAR== menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_SHUTTER == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_CHARIOT == menu_ctrl_ptr->cur_style
		#endif
        )
    {
        menu_ctrl_ptr->enter_anim_type = GUIMENU_ENTER_ANIM_SLIDE_ICON;
        MMITHEME_GetSlideMenuInfo( menu_ctrl_ptr->win_handle, &menu_ctrl_ptr->main_menu_data.slide_menu_data);
    }
    else if ( GUIMENU_ISTYLE == menu_ctrl_ptr->cur_style)
    {
        MMITHEME_GetIStyleMenuInfo( menu_ctrl_ptr->win_handle, &menu_ctrl_ptr->main_menu_data.istyle_menu_data);
    }
#ifdef QBTHEME_SUPPORT
    else if (GUIMENU_STYLE_QBTHEME == menu_ctrl_ptr->cur_style)
    {
        MMITHEME_GetQbthemeMenuInfo(menu_ctrl_ptr->win_handle, &menu_ctrl_ptr->main_menu_data.qbtheme_menu_data);
    }
#endif    
#ifdef MENU_CRYSTALCUBE_SUPPORT
    else if (GUIMENU_STYLE_CRYSTAL_CUBE == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->enter_anim_type = GUIMENU_ENTER_ANIM_NONE;
        MMITHEME_GetCrystalCubeMenuInfo(&menu_ctrl_ptr->main_menu_data.crystalcube_menu_data);
        //both_rect.v_rect.bottom = 480 - 1;        
        //menu_ctrl_ptr->both_rect  = both_rect;
        //menu_ctrl_ptr->rect       = both_rect.v_rect;
        
    }
#endif

#ifdef MENU_CYCLONE_SUPPORT
    else if (GUIMENU_STYLE_CYCLONE == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->enter_anim_type = GUIMENU_ENTER_ANIM_NONE;
        //MMITHEME_GetCubePageMenuInfo(&menu_ctrl_ptr->main_menu_data.cube_page_menu_data);
        //both_rect.v_rect.bottom = 480 - 1;        
        //menu_ctrl_ptr->both_rect  = both_rect;
        //menu_ctrl_ptr->rect       = both_rect.v_rect;
        
    }
#endif

    menu_ctrl_ptr->is_transparent = TRUE;

    //init menu
    if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style])
        &&(PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu))
    {
        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu(menu_ctrl_ptr);
    }
}

/*****************************************************************************/
//  Description : init dynamic menu control
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void InitDynamicMenu(
                           IGUICTRL_T       *ctrl_ptr,      //control pointer
                           uint16           node_max_num,   //node max number
                           GUI_BOTH_RECT_T  both_rect,      //menu rect
                           GUIMENU_STYLE_E  menu_style      //menu style
                           )
{
    GUIMENU_CTRL_T  *menu_ctrl_ptr = (GUIMENU_CTRL_T*)ctrl_ptr;

    //get menu process function
    MMITHEME_GetMenuProc(menu_ctrl_ptr->process_func);

    //get menu theme
    MMITHEME_GetCommonMenuTheme(&menu_ctrl_ptr->common_theme);
    MMITHEME_GetSecondMenuTheme(&menu_ctrl_ptr->second_menu_theme);
    MMITHEME_GetPopMenuTheme(&menu_ctrl_ptr->pop_menu_theme);
    MMITHEME_GetOptionsMenuTheme(&menu_ctrl_ptr->options_menu_theme);

    //alloc memory for dynamic menu tree
    AllocDynamicMenuMemory(node_max_num,&menu_ctrl_ptr->node_memory);
    menu_ctrl_ptr->root_node_ptr = CreatNewNode(&menu_ctrl_ptr->node_memory);
    menu_ctrl_ptr->root_node_ptr->sub_menu_style = menu_style;
    menu_ctrl_ptr->cur_parent_node_ptr = menu_ctrl_ptr->root_node_ptr;

    //set other param
    menu_ctrl_ptr->is_static  = FALSE;
    menu_ctrl_ptr->both_rect  = both_rect;
    menu_ctrl_ptr->rect       = both_rect.v_rect;
    menu_ctrl_ptr->cur_style  = menu_style;
    menu_ctrl_ptr->win_handle = MMK_GetWinHandleByCtrl(menu_ctrl_ptr->handle);
    
 // PDA项目，将POPUP menu定义为OptionsMenu
#ifdef GUIPOPMENU_OPTIONS_CONTEXT_SUPPORT
    if (GUIMENU_STYLE_POPUP == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->cur_style = GUIMENU_STYLE_OPTIONS;
    }
#endif

    // 是否移动状态，目前主菜单才有效
    menu_ctrl_ptr->is_move_state = FALSE;

    // 默认支持滚动条
    menu_ctrl_ptr->is_need_prgbox = TRUE;

    // 增加滚动条时，是否需要动态调整区域
    menu_ctrl_ptr->is_revise_rect = FALSE;

    menu_ctrl_ptr->max_horz_width = (uint16)-1;
    menu_ctrl_ptr->max_vert_width = (uint16)-1;
    
    //init menu
    if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style])
        &&(PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu))
    {
        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu(menu_ctrl_ptr);
    }

    menu_ctrl_ptr->is_transparent = TRUE;
}

/*****************************************************************************/
//  Description : alloc memory for dynamic menu tree
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void AllocDynamicMenuMemory(
                                  uint16                max_node_num,   //max node number
                                  GUIMENU_NODE_MEMORY_T *node_mem_ptr   //dynamic menu node memory pointer
                                  )
{
    uint16          i = 0;
    GUIMENU_NODE_T  *cur_free_node_ptr = PNULL;

    //kevin.lou modified:delete assert        
    //SCI_ASSERT(PNULL != node_mem_ptr);  /*assert verified*/
    if (PNULL == node_mem_ptr)
    {
        return;
    }
    
    //alloc dynamic menu memory,include root node
    max_node_num = (uint16)(max_node_num + 1);
    node_mem_ptr->init_mem_ptr = SCI_ALLOC_APP(max_node_num * sizeof(GUIMENU_NODE_T));
    SCI_MEMSET(node_mem_ptr->init_mem_ptr,0,(max_node_num*sizeof(GUIMENU_NODE_T)))

    //set free node value
    node_mem_ptr->free_node_num = max_node_num;
    node_mem_ptr->free_node_ptr = node_mem_ptr->init_mem_ptr;

    //set next free node pointer
    cur_free_node_ptr = node_mem_ptr->free_node_ptr;
    for (i=0; i<(max_node_num-1); i++,cur_free_node_ptr++)
    {
        cur_free_node_ptr->next_free_node_ptr = cur_free_node_ptr + 1;
    }
    cur_free_node_ptr->next_free_node_ptr = PNULL;
}

/*****************************************************************************/
//  Description : creat one new node
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* CreatNewNode(
                                   GUIMENU_NODE_MEMORY_T    *node_mem_ptr
                                   )
{
    GUIMENU_NODE_T  *new_node_ptr = PNULL;

    //kevin.lou modified:delete assert        
    //SCI_ASSERT(PNULL != node_mem_ptr);  /*assert verified*/
    if (PNULL == node_mem_ptr)
    {
        return PNULL;
    }

    //creat one new node
    new_node_ptr = node_mem_ptr->free_node_ptr;

    //set free node value
    node_mem_ptr->free_node_num--;
    node_mem_ptr->free_node_ptr = node_mem_ptr->free_node_ptr->next_free_node_ptr;

    //init new node
    SCI_MEMSET(new_node_ptr,0,sizeof(GUIMENU_NODE_T));

    return (new_node_ptr);
}

/*****************************************************************************/
//  Description : check display init ok?
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN IsInitItemFinished(
								 GUIMENU_CTRL_T     *menu_ctrl_ptr
								 )
{
	BOOLEAN result = FALSE;

	if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->init_item_timer_id)
	{
		result = TRUE;
	}

	return result;
}

/*****************************************************************************/
// 	Description : handle menu msg function
//	Global resource dependence : 
//  Author: Jassmine
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E MenuHandleMsg(
                                 IGUICTRL_T         *ctrl_ptr,  //control pointer
                                 MMI_MESSAGE_ID_E   msg_id,  	//message
                                 DPARAM             param       //add data
                                 )
{
    MMI_RESULT_E        result = MMI_RESULT_TRUE;
    GUIMENU_CTRL_T      *menu_ctrl_ptr = (GUIMENU_CTRL_T*)ctrl_ptr;

    if (PNULL == menu_ctrl_ptr)
    {
        return MMI_RESULT_FALSE;
    }


	if (!IsInitItemFinished(menu_ctrl_ptr) && msg_id != MSG_TIMER)
	{
		return MMI_RESULT_FALSE;
	}

    switch (msg_id)
    {
    case MSG_CTL_OPEN:
        menu_ctrl_ptr->is_first_disp = TRUE;    
        SetState(&menu_ctrl_ptr->menu_state, GUIMENU_STATE_ENTER, TRUE);

        // 初始值
        menu_ctrl_ptr->pre_msg_loop_page_index = (uint16)-1;
        menu_ctrl_ptr->pre_msg_loop_item_index = (uint16)-1;
        menu_ctrl_ptr->pre_msg_loop_menu_level = (uint16)-1;

        // 如果支持多层，创建所需动画的层
        if (UILAYER_IsMultiLayerEnable() && 
            PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
        }
        break;

    case MSG_CTL_PAINT:
        //calculate pop-up menu rect
        if (menu_ctrl_ptr->is_need_reset_rect)
        {
            menu_ctrl_ptr->is_need_reset_rect = FALSE;
            CalculatePopupRect(0,menu_ctrl_ptr);
        }

        //pop-up menu need update all,include main and sub etc.
        UpdatePrevPopMenu(0,menu_ctrl_ptr);
        
        //display menu
        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
            DisplayMenu(menu_ctrl_ptr);
        SetState(&menu_ctrl_ptr->menu_state, GUIMENU_STATE_ENTER, FALSE);
                
        if (menu_ctrl_ptr->enter_anim_type > GUIMENU_ENTER_ANIM_NONE && menu_ctrl_ptr->enter_anim_type < GUIMENU_ENTER_ANIM_MAX)
        {
#ifdef EFFECT_ZOOM_SUPPORT
            MMIDEFAULT_SetCurWin3DEffect(MMI_WIN_ANIM_ZOOM_IN);
#endif
            // MMIDEFAULT_CancelCurWin3DEffect();
        }

        HandleTextToSpeech(menu_ctrl_ptr);
        break;

    case MSG_CTL_GET_FOCUS:
        // 初始值
        menu_ctrl_ptr->pre_msg_loop_page_index = (uint16)-1;
        menu_ctrl_ptr->pre_msg_loop_item_index = (uint16)-1;
        menu_ctrl_ptr->pre_msg_loop_menu_level = (uint16)-1;

        // SetState(&menu_ctrl_ptr->menu_state, GUIMENU_STATE_ENTER, TRUE);
        if (menu_ctrl_ptr->is_move_state)
        {
            GUIMENU_StartShakeIconTimer(menu_ctrl_ptr);
        }

        // 如果支持多层，创建所需动画的层
        if (UILAYER_IsMultiLayerEnable() && 
            PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
        }
        break;
        
    case MSG_CTL_GET_ACTIVE:
        menu_ctrl_ptr->is_active = TRUE;

        if (menu_ctrl_ptr->is_move_state)
        {
            GUIMENU_StartShakeIconTimer(menu_ctrl_ptr);
        }
        break;

    case MSG_CTL_LOSE_FOCUS:
        MMITheme_StopControlText();

        //reset menu info
        ResetMenuInfo(menu_ctrl_ptr);
        //StopTPMoveTimer(menu_ctrl_ptr);
        
        // 由于多层画图，失去焦点的时候，需要把主菜单信息画在主层上，因此这里需要在lose focus时画图
        if (UILAYER_IsMultiLayerEnable() && 
            PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
        }
        break;

    case MSG_CTL_LOSE_ACTIVE:
    case MSG_CTL_CLOSE:
        if (MSG_CTL_LOSE_ACTIVE == msg_id)
        {
            menu_ctrl_ptr->is_active = FALSE;
        }

        MMITheme_StopControlText();

        //reset menu info
        ResetMenuInfo(menu_ctrl_ptr);

        // 如果支持多层，动画需要做处理
        if (UILAYER_IsMultiLayerEnable() && 
            PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
        }
        break;

	case MSG_NOTIFY_MENU_INFO:
    case MSG_APP_MENU:
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
        {
            result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
        }
        else
        {
            result = MMI_RESULT_FALSE;
        }
        break;

    case MSG_APP_OK:
        if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_moving)
        {   
#ifdef TOUCHPANEL_TYPE_NONE // 触摸屏的手机，因为要支持选项菜单，OK键执行创建菜单功能
            if ((GUIMENU_STYLE_OPTION_PAGE == menu_ctrl_ptr->cur_style) &&
                (menu_ctrl_ptr->is_focus_title))
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    HandleMenuDownKey(menu_ctrl_ptr);
            }
            else
#endif
            {
                HandleMenuOkKey(menu_ctrl_ptr,msg_id);
            }
            HandleTextToSpeech(menu_ctrl_ptr);
        }
        else
        {
            //SCI_TRACE_LOW:"MenuHandleMsg %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1304_112_2_18_3_20_19_229,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
        }
        break;

    case MSG_APP_WEB:
        if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_moving)
        { 
            if ((GUIMENU_STYLE_OPTION_PAGE == menu_ctrl_ptr->cur_style) &&
                (menu_ctrl_ptr->is_focus_title))
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    HandleMenuDownKey(menu_ctrl_ptr);
            }
            else
            {
                HandleMenuOkKey(menu_ctrl_ptr,msg_id);
            }
            HandleTextToSpeech(menu_ctrl_ptr);
        }
        else
        {
            //SCI_TRACE_LOW:"MenuHandleMsg %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1325_112_2_18_3_20_19_230,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
        }
        break;

    case MSG_APP_RED:
        if (MMK_TP_SLIDE_TP_SCROLL == menu_ctrl_ptr->slide_state || menu_ctrl_ptr->is_moving)
        {
            //SCI_TRACE_LOW:"MenuHandleMsg %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1332_112_2_18_3_20_19_231,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
            result = MMI_RESULT_TRUE;
        }
#ifdef MENU_CRYSTALCUBE_SUPPORT
        else if (GUIMENU_STYLE_CRYSTAL_CUBE == menu_ctrl_ptr->cur_style
            && (GUIMENU_CRYSTALCUBE_STATE_ANIM == menu_ctrl_ptr->guimenu_union.crystalcube_ctrl.crystalcube_state)
            )
        {
            result = MMI_RESULT_TRUE;
        }
#endif
#ifdef MENU_CYCLONE_SUPPORT
        else if (GUIMENU_STYLE_CYCLONE == menu_ctrl_ptr->cur_style
            && (GUIMENU_CYCLONE_STATE_ANIM == menu_ctrl_ptr->guimenu_union.cyclone_ctrl.cyclone_state)
            )
        {
            result = MMI_RESULT_TRUE;
        }
#endif

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
        if (MMK_TP_SLIDE_TP_SCROLL != menu_ctrl_ptr->slide_state && menu_ctrl_ptr->is_move_state)
        { 
            HandleMenuCancelKey(menu_ctrl_ptr, MSG_APP_RED);
        }
#endif
        else
        {
            result = MMI_RESULT_FALSE;
        }
        break;

    case MSG_NOTIFY_CANCEL:    
    case MSG_APP_CANCEL:
        //StopTPMoveTimer(menu_ctrl_ptr);
        if (MMK_TP_SLIDE_TP_SCROLL != menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_moving)
        { 
            HandleMenuCancelKey(menu_ctrl_ptr, MSG_APP_CANCEL);
            HandleTextToSpeech(menu_ctrl_ptr);
        }
        else
        {
            //SCI_TRACE_LOW:"MenuHandleMsg MSG_APP_CANCEL %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1374_112_2_18_3_20_20_232,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
        }
        break;

    case MSG_APP_UP:
    case MSG_APP_DOWN:
    case MSG_APP_LEFT:
    case MSG_APP_RIGHT:
    case MSG_KEYREPEAT_UP:
    case MSG_KEYREPEAT_DOWN:
    case MSG_KEYREPEAT_LEFT:
    case MSG_KEYREPEAT_RIGHT:
        if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_move_state)
        {
            // 先停止，避免和3D产生纠结，导致exp
            MMITheme_StopControlText();

            result = HandleMenuDirectionKey(menu_ctrl_ptr,msg_id);
            HandleTextToSpeech(menu_ctrl_ptr);
        }
        else
        {
            //SCI_TRACE_LOW:"MenuHandleMsg MSG_APP_UP %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1396_112_2_18_3_20_20_233,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_move_state);
        }
        break;

    case MSG_APP_1:
    case MSG_APP_2:
    case MSG_APP_3:
    case MSG_APP_4:
    case MSG_APP_5:
    case MSG_APP_6:
    case MSG_APP_7:
    case MSG_APP_8:
    case MSG_APP_9:
        if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_move_state)
        {
            HandleMenuNumKey(menu_ctrl_ptr,msg_id);
            HandleTextToSpeech(menu_ctrl_ptr);
        }
        else
        {
            //SCI_TRACE_LOW:"MenuHandleMsg MSG_APP_1 %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1416_112_2_18_3_20_20_234,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
        }
        break;

    case MSG_APP_HASH:
        if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_move_state)
        {
            // 这个if语句为测试所用，以后将删除，功能为按"#"之后，直接显示第一页
            if (0 != menu_ctrl_ptr->cur_page_index 
                && GUIMENU_STYLE_SLIDE_PAGE == menu_ctrl_ptr->cur_style) 
            {
                menu_ctrl_ptr->cur_page_index = 0;
                MMK_SendMsg(menu_ctrl_ptr->handle, MSG_CTL_PAINT, PNULL);
            }
            else
            {
                result = MMI_RESULT_FALSE;
            }
        }
        else
        {
            //SCI_TRACE_LOW:"MenuHandleMsg MSG_APP_HASH %d, %d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1437_112_2_18_3_20_20_235,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
        }
        break;

    case MSG_TP_PRESS_DOWN:
        //防止烂屏抖动
        //MMK_DelayTpMove();

        // 先停止，避免和3D产生纠结，导致exp
        MMITheme_StopControlText();

        StopTitleIconDispTimer(menu_ctrl_ptr);
        HandleMenuTpDown(param,menu_ctrl_ptr);
        HandleTextToSpeech(menu_ctrl_ptr);
        break;

    case MSG_TP_PRESS_UP:
        StopTitleIconDispTimer(menu_ctrl_ptr);
        HandleMenuTpUp(param,menu_ctrl_ptr);
        HandleTextToSpeech(menu_ctrl_ptr);
        break;

    case MSG_TP_PRESS_MOVE:
        StopTitleIconDispTimer(menu_ctrl_ptr);
        HandleMenuTpMove(param,menu_ctrl_ptr);
        break;

    case MSG_TP_PRESS_LONG:
    case MSG_TP_PRESS_SHORT:
        // 处理tp short 消息
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
        }
        break;


    case MSG_TIMER:
        if (menu_ctrl_ptr->item_timer_id == *(uint8*)param)
        {
            //stop timer
            StopItemTextTimer(menu_ctrl_ptr);

            //scroll display item text
            if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state
                && MMK_IsFocusWin(menu_ctrl_ptr->win_handle)) // slide状态下，不画
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    ShowScrollItemStr(menu_ctrl_ptr);
            }
        }
        else if (menu_ctrl_ptr->start_move_timer_id == *(uint8*)param)
        {
            GUIMENU_MOVE_STATE_T move_state = GUIMENU_STATE_MOVE_NONE;
            
            StopMoveStartTimer(menu_ctrl_ptr);
            
            move_state = GUIMENU_STATE_MOVE_SET;
            
            MMITHEME_Vibrate(MMITHEME_MENUMAIN_ICON_VIBRATE); // 震动一次

            MMK_SendMsg(menu_ctrl_ptr->win_handle, MSG_NOTIFY_MENU_SET_STATUS, (DPARAM)&move_state);
        }
        else if (menu_ctrl_ptr->shake_icon_timer_id == *(uint8*)param)
        {
			MMITHEME_StopVibrate();
			
            StopShakeIconTimer(menu_ctrl_ptr);
            
            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DisplaySpecialMenu)
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DisplaySpecialMenu(menu_ctrl_ptr);
            }
        }
        else
        {
            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuTimer)
            {
                result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    HandleMenuTimer(menu_ctrl_ptr, msg_id, param);
            }
            else
            {
                result = MMI_RESULT_FALSE;
            }
        }
        break;

    case MSG_CTL_PRGBOX_MOVEUP_ONE:
    case MSG_CTL_PRGBOX_MOVEDOWN_ONE:
    case MSG_CTL_PRGBOX_MOVEUP_PAGE:
    case MSG_CTL_PRGBOX_MOVEDOWN_PAGE:
    case MSG_CTL_PRGBOX_MOVE:
        if (MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_move_state)
        {
            HandleMenuTpScroll((uint16)(*(uint32*)param),menu_ctrl_ptr);
        }
        break;

#ifdef MMI_PDA_SUPPORT
    case MSG_NOTIFY_PENOK:
        if (GUIMENU_IsMainMenu(menu_ctrl_ptr))
        {
            //StopTPMoveTimer(menu_ctrl_ptr);
            if (MMK_TP_SLIDE_TP_SCROLL != menu_ctrl_ptr->slide_state && !menu_ctrl_ptr->is_moving)
            { 
                HandleMenuCancelKey(menu_ctrl_ptr, MSG_APP_CANCEL);
                HandleTextToSpeech(menu_ctrl_ptr);
            }
            else
            {
                //SCI_TRACE_LOW:"MenuHandleMsg MSG_APP_CANCEL %d, %d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_1548_112_2_18_3_20_20_236,(uint8*)"dd", menu_ctrl_ptr->slide_state, menu_ctrl_ptr->is_moving);
            }
        }
        break;
#endif
		
    default:
#if defined(MENU_CRYSTALCUBE_SUPPORT) || defined(MENU_CYCLONE_SUPPORT) 
		if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
		{
			result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, param);
		}       
#else
        result = MMI_RESULT_FALSE;
#endif
        break;
    }
	
    return (result);
}

/*****************************************************************************/
//  Description : is pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsPopMenu(
                                 GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                                 )
{
    BOOLEAN     result = FALSE;

    switch (menu_ctrl_ptr->cur_style)
    {
    case GUIMENU_STYLE_POPUP:
    case GUIMENU_STYLE_POPUP_RADIO:
    case GUIMENU_STYLE_POPUP_CHECK:
    case GUIMENU_STYLE_POPUP_AUTO:
    case GUIMENU_STYLE_POPUP_RECT:
    case GUIMENU_STYLE_OPTIONS:
        result = TRUE;
        break;

    default:
        break;
    }

    return (result);
}

/*****************************************************************************/
//  Description : is main menu
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsMainMenu(
                                 GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                                 )
{
    BOOLEAN     result = FALSE;

    switch (menu_ctrl_ptr->cur_style)
    {
    case GUIMENU_STYLE_ICON:
    case GUIMENU_STYLE_OPTION_PAGE:
    case GUIMENU_STYLE_TITLE_ICON:

    case GUIMENU_ISTYLE:
    case GUIMENU_STYLE_CUBE:
#ifdef MENU_CRYSTALCUBE_SUPPORT
    case GUIMENU_STYLE_CRYSTAL_CUBE:
#endif
#ifdef MENU_CYCLONE_SUPPORT
    case GUIMENU_STYLE_CYCLONE:
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
	case GUIMENU_STYLE_GO:
#else
    case GUIMENU_STYLE_SLIDE_PAGE:
    case GUIMENU_STYLE_SPHERE:
    case GUIMENU_STYLE_CYLINDER:
    case GUIMENU_STYLE_TRAPEZOID:
    case GUIMENU_STYLE_BINARY_STAR:
    case GUIMENU_STYLE_SHUTTER:
    case GUIMENU_STYLE_CHARIOT:
#endif
        result = TRUE;
        break;
        
    default:
        break;
    }

    return (result);
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: violent.wei
//  Note: 当有focus item变化时，朗读list内容
/*****************************************************************************/
LOCAL void HandleTextToSpeech(
                           GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                           )
{
    if (MMITheme_IsControlTTSOpen())
    {
        // 当focus page或focus item有变化时，才进行朗读
        if (menu_ctrl_ptr->pre_msg_loop_page_index != menu_ctrl_ptr->cur_page_index ||
            menu_ctrl_ptr->pre_msg_loop_item_index != menu_ctrl_ptr->cur_item_index ||
            menu_ctrl_ptr->pre_msg_loop_menu_level != menu_ctrl_ptr->menu_level)
        {
            BOOLEAN is_grayed;
            MMI_IMAGE_ID_T img_id = 0;
            MMI_STRING_T wstr[GUIITEM_CONTENT_MAX_NUM] = {0};
            
            if (GUIMENU_GetItem(menu_ctrl_ptr, menu_ctrl_ptr->cur_item_index, &wstr[0], &img_id, &is_grayed))
            {
                MMITheme_PlayControlText(wstr, 1);
                
                menu_ctrl_ptr->pre_msg_loop_page_index = menu_ctrl_ptr->cur_page_index;
                menu_ctrl_ptr->pre_msg_loop_item_index = menu_ctrl_ptr->cur_item_index;
                menu_ctrl_ptr->pre_msg_loop_menu_level = menu_ctrl_ptr->menu_level;
            }
        }
    }
}

/*****************************************************************************/
//  Description : calculate pop-up menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL void CalculatePopupRect(
                              uint16            rect_top,       //only for sub pop-up menu
                              GUIMENU_CTRL_T    *menu_ctrl_ptr
                              )
{
    if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->CalculateRect)
    {
        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->CalculateRect(menu_ctrl_ptr, rect_top);
    }
}

/*****************************************************************************/
//  Description : update previous pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:  当前是pop-up menu,lose focus后get focus需要刷新之前所有的pop-up menu
//         当前是pop-up menu,cancel后需要根据Rect判断是否刷新之前的pop-up menu
/*****************************************************************************/
LOCAL void UpdatePrevPopMenu(
                             uint16         start_level,
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             )
{
#if defined(GUIPOPMENU_POP_POP_SUPPORT)
    int16                   offset_y = 0;
    uint16                  i = 0;
    uint16                  menu_level = 0;
    uint16                  first_item_index = 0;
    uint16                  cur_item_index = 0;
    GUI_RECT_T              menu_rect = {0};
    GUIMENU_NODE_T          *cur_parent_node_ptr = PNULL;
    MMI_MENU_GROUP_ID_T     cur_group_id = 0;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }

    if ((GUIMENU_IsPopMenu(menu_ctrl_ptr)) &&
        (start_level < menu_ctrl_ptr->menu_level))
    {
        //save current menu info
        menu_level          = menu_ctrl_ptr->menu_level;
        cur_group_id        = menu_ctrl_ptr->cur_group_id;
        cur_item_index      = menu_ctrl_ptr->cur_item_index;
        first_item_index    = menu_ctrl_ptr->first_item_index;
        menu_rect           = menu_ctrl_ptr->rect;
        offset_y            = menu_ctrl_ptr->offset_y;
        cur_parent_node_ptr = menu_ctrl_ptr->cur_parent_node_ptr;

        //need update previous all pop-up menu
        for (i=start_level; i<menu_level; i++)
        {
            //set menu info
            SetMenuInfo(i,
                menu_ctrl_ptr->menu_stack[i].first_item_index,
                menu_ctrl_ptr->menu_stack[i].cur_item_index,
                menu_ctrl_ptr->menu_stack[i].rect,
                menu_ctrl_ptr->menu_stack[i].offset_y,
                menu_ctrl_ptr->menu_stack[i].group_id,
                menu_ctrl_ptr->menu_stack[i].parent_node_ptr,
                menu_ctrl_ptr);

            menu_ctrl_ptr->is_first_disp = FALSE;

            //display previous menu
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                DisplayMenu(menu_ctrl_ptr);

            ResetMenuInfo(menu_ctrl_ptr);
        }

        //reset current menu info
        SetMenuInfo(menu_level,
            first_item_index,
            cur_item_index,
            menu_rect,
            offset_y,
            cur_group_id,
            cur_parent_node_ptr,
            menu_ctrl_ptr);
    }
#endif
}

/*****************************************************************************/
//  Description : set menu info,include menu level,current and first index etc.
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void SetMenuInfo(
                       uint16               menu_level,
                       uint16               first_item_index,
                       uint16               cur_item_index,
                       GUI_RECT_T           menu_rect,
                       int16                offset_y,
                       MMI_MENU_GROUP_ID_T  cur_group_id,
                       GUIMENU_NODE_T       *cur_parent_node_ptr,
                       GUIMENU_CTRL_T       *menu_ctrl_ptr
                       )
{
    menu_ctrl_ptr->menu_level       = menu_level;
    menu_ctrl_ptr->cur_group_id     = cur_group_id;
    menu_ctrl_ptr->first_item_index = first_item_index;
    menu_ctrl_ptr->cur_item_index   = cur_item_index;
    menu_ctrl_ptr->rect             = menu_rect;
    menu_ctrl_ptr->offset_y         = offset_y;

    if (menu_ctrl_ptr->is_static)
    {
        //set menu group etc. info
        menu_ctrl_ptr->cur_group_ptr    = (GUIMENU_GROUP_T *)MMITHEME_GetMenuGroup(menu_ctrl_ptr->cur_group_id);
        menu_ctrl_ptr->cur_group_info_ptr = GetGroupInfo(menu_ctrl_ptr, menu_ctrl_ptr->cur_group_id);
        menu_ctrl_ptr->item_total_num   = menu_ctrl_ptr->cur_group_info_ptr->visible_child_item_num;
        menu_ctrl_ptr->cur_style        = menu_ctrl_ptr->cur_group_ptr->menu_style;
    }
    else
    {
        //set menu group etc. info
        menu_ctrl_ptr->cur_parent_node_ptr = cur_parent_node_ptr;
        menu_ctrl_ptr->item_total_num      = menu_ctrl_ptr->cur_parent_node_ptr->visible_child_node_num;
        menu_ctrl_ptr->cur_style           = menu_ctrl_ptr->cur_parent_node_ptr->sub_menu_style;
    }

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
    if (menu_level > 0)
    {
        CheckSubMenuStyle(menu_ctrl_ptr);
    }
#endif

    //destroy progress
    if (PNULL != menu_ctrl_ptr->prgbox_ctrl_ptr)
    {
        MMK_DestroyControl(menu_ctrl_ptr->prgbox_ctrl_ptr->handle);
        menu_ctrl_ptr->prgbox_ctrl_ptr = PNULL;
    }
}

/*****************************************************************************/
//  Description : HandleMenuOkKey
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleMenuOkKey(
                              GUIMENU_CTRL_T    *menu_ctrl_ptr,
                              MMI_MESSAGE_ID_E  msg_id
                              )
{
    BOOLEAN                 is_need_send_msg = FALSE;
    BOOLEAN                 is_handled = FALSE;
    BOOLEAN                 is_grayed = FALSE;
    BOOLEAN                 is_exist_child = FALSE;
    uint16                  cur_item_top = 0;
    GUIMENU_NODE_T          *cur_node_ptr = PNULL;
    GUIMENU_ITEM_T          cur_item = {0};

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return FALSE;
    }

    if (menu_ctrl_ptr->is_move_state)
    {
        is_handled = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuOk(menu_ctrl_ptr, msg_id);
    }
    else
    {
        // 先看mmimenu会不会处理，如果处理了，guimenu就不用处理了
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuOk)
        {
            is_handled = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuOk(menu_ctrl_ptr, msg_id);
        }

        if (!is_handled)
        {
             if (GetInfoInOK(menu_ctrl_ptr, &is_grayed, &is_exist_child, &cur_item, &cur_node_ptr))
             {
                if (!is_grayed)
                {
                    if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->AdjustFirstAndDisplay)
                    {
                        //adjust menu first item
                        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                            AdjustFirstAndDisplay(menu_ctrl_ptr, TRUE);
                    }

                    if (!is_exist_child)
                    {
                        // 需要发消息给应用
                        is_need_send_msg = TRUE;
                    }
                    else
                    {
#ifdef UI_P3D_SUPPORT
                        const GUI_LCD_DEV_INFO *lcd_dev_info = MMITHEME_GetDefaultLcdDev();
                        GUI_RECT_T lcd_rect = MMITHEME_GetFullScreenRect();
#endif 

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
					
						// 全屏的menu 不需要重刷前一窗口 xiyuan edit for CR
						if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
						{
							GUIMENU_UpdatePrevWindow(menu_ctrl_ptr);
						}
#endif

                        //push menu in stack
                        PushMenuStack(menu_ctrl_ptr);
                        
                        //reset menu info
                        ResetMenuInfo(menu_ctrl_ptr);
                        
                        //get current item top
                        if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
                        {
                            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetCurItemTop)
                            {
                                cur_item_top = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                                        GetCurItemTop(menu_ctrl_ptr);
                            }
                        }
                        else
                        {
                            menu_ctrl_ptr->offset_y = menu_ctrl_ptr->rect.top;
                        }

                        RestoreInfoInOK(menu_ctrl_ptr, &cur_item, cur_node_ptr);

                        //calculate pop-up menu rect
                        CalculatePopupRect(cur_item_top,menu_ctrl_ptr);

#ifdef UI_P3D_SUPPORT    
                        //save old lcd buf
                        MMK_HandleWinMoveMsg(menu_ctrl_ptr->win_handle, MSG_LOSE_FOCUS, PNULL);
                        MMK_HandleWinMoveMsg(menu_ctrl_ptr->win_handle, MSG_OPEN_WINDOW, PNULL);                    
#endif

                        //destroy progress
                        if (PNULL != menu_ctrl_ptr->prgbox_ctrl_ptr)
                        {
                            MMK_DestroyControl(menu_ctrl_ptr->prgbox_ctrl_ptr->handle);
                            menu_ctrl_ptr->prgbox_ctrl_ptr = PNULL;
                        }

                        //display next menu
                        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                            DisplayMenu(menu_ctrl_ptr);

#ifdef UI_P3D_SUPPORT    
                        MMITHEME_StoreUpdateRect(lcd_dev_info, lcd_rect);
                        //paint move 3d effect.
                        MMK_HandleWinMoveMsg(menu_ctrl_ptr->win_handle, MSG_FULL_PAINT, PNULL);
#endif
                    }
                }
                else
                {
                    // 处理灰化
                    if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg)
                    {
                        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleSpecialMsg(menu_ctrl_ptr, msg_id, (DPARAM)&is_grayed);
                    }
                }
            }
        }
    }

    if (is_need_send_msg)
    {
        // 在app接到ok以后，可能先干一些事情再关菜单，这样可能会导致关菜单时停止tts失败，
        // 所以在发通知之前就关tts
        MMITheme_StopControlText();

        //notify parent control or parent window
        NotifyParentMsg(menu_ctrl_ptr->handle,msg_id);
    }
    
    return (is_handled);
}

/*****************************************************************************/
//  Description : get current group id
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu style
/*****************************************************************************/
LOCAL MMI_MENU_GROUP_ID_T GetCurGroupId(
                                        GUIMENU_CTRL_T  *menu_ctrl_ptr
                                        )
{
    GUIMENULIST_ITEM_INFO_T *item_info_ptr = PNULL;
    MMI_MENU_GROUP_ID_T     cur_group_id    = 0;

    if (menu_ctrl_ptr->is_static)
    {
        //get current group id
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemInfoPtr)
        {
            item_info_ptr = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemInfoPtr(menu_ctrl_ptr, menu_ctrl_ptr->cur_page_index, menu_ctrl_ptr->cur_item_index);
            if (PNULL != item_info_ptr)
            {
                cur_group_id = item_info_ptr->group_id;
            }
        }
        else
        {
            cur_group_id = menu_ctrl_ptr->cur_group_id;
        }
    }

    return cur_group_id;
}

/*****************************************************************************/
//  Description : get current item pointer
//  Global resource dependence : 
//  Author: Jassmine
//  Note: this method is for static menu style
/*****************************************************************************/
LOCAL BOOLEAN GetCurItem(
                         GUIMENU_CTRL_T  *menu_ctrl_ptr,     //in
                         GUIMENU_ITEM_T  *item_ptr,          //out
                         GUIMENU_ITEM_INFO_T *item_info_ptr  //out
                         )
{
    BOOLEAN         result = TRUE;
    GUIMENU_ITEM_T  *temp_item_ptr = PNULL;
    GUIMENU_MAINMENU_INFO_T cur_item_info = {0};

    if ((PNULL == menu_ctrl_ptr)||(PNULL == item_ptr))
    {
        return FALSE;
    }

	cur_item_info.menu_data_type = GUIMENU_MAINMENU_DATA_TYPE_MAX;

    if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemPtr)
    {
		((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemPtr(menu_ctrl_ptr, menu_ctrl_ptr->cur_page_index, menu_ctrl_ptr->cur_item_index, &cur_item_info);
    }
    else if (PNULL != menu_ctrl_ptr->cur_group_ptr)
    {
        temp_item_ptr = GetItemByVisibleIndex(menu_ctrl_ptr, menu_ctrl_ptr->cur_item_index, item_info_ptr);

        if (PNULL != temp_item_ptr)
        {
            *item_ptr = *temp_item_ptr;
        }
        else
        {
            result = FALSE;
        }
    }

    if (GUIMENU_MAINMENU_DATA_TYPE_MAX != cur_item_info.menu_data_type)
    {
        if (GUIMENU_MAINMENU_DATA_TYPE_STATIC == cur_item_info.menu_data_type)
        {
            item_ptr->menu_id = cur_item_info.menu_data.static_menu_ptr->menu_id;
            item_ptr->link_group_id = cur_item_info.menu_data.static_menu_ptr->link_group_id;
        }
#ifdef DYNAMIC_MAINMENU_SUPPORT
        else if(GUIMENU_MAINMENU_DATA_TYPE_DYNAMIC == cur_item_info.menu_data_type
            && PNULL != cur_item_info.menu_data.dynamic_menu.ori_data_ptr)
        {
            item_ptr->menu_id = cur_item_info.menu_data.dynamic_menu.ori_data_ptr->menu_id;
            item_ptr->link_group_id = 0;
        }
#endif
		else
		{
			PNULL;
		}
    }

    return result;
}


/*****************************************************************************/
//  Description : get current item pointer
//  Global resource dependence : 
//  Author: Jassmine
//  Note: this method is for static menu style
/*****************************************************************************/
LOCAL void GetCurItemPtr(
						 GUIMENU_CTRL_T  *menu_ctrl_ptr,     //in
						 GUIMENU_ITEM_INFO_T *item_info_ptr,  //out
						 GUIMENU_MAINMENU_INFO_T *cur_item_ptr
						 )
{
    // GUIMENU_MAINMENU_INFO_T *cur_item_ptr = PNULL;
    GUIMENU_ITEM_T *temp_item_ptr = PNULL;

    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }

    switch (menu_ctrl_ptr->cur_style)
    {
    case GUIMENU_STYLE_ICON:
    case GUIMENU_STYLE_TITLE_ICON:
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemPtr)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemPtr(menu_ctrl_ptr, 0, menu_ctrl_ptr->cur_item_index, cur_item_ptr);
        }
        break;
        
    case GUIMENU_STYLE_OPTION_PAGE:
    case GUIMENU_ISTYLE:
    case GUIMENU_STYLE_CUBE:
#ifdef MENU_CRYSTALCUBE_SUPPORT
    case GUIMENU_STYLE_CRYSTAL_CUBE:
#endif
#ifdef MENU_CYCLONE_SUPPORT
    case GUIMENU_STYLE_CYCLONE:
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
	case GUIMENU_STYLE_GO:
#else
    case GUIMENU_STYLE_SLIDE_PAGE:
    case GUIMENU_STYLE_SPHERE:
    case GUIMENU_STYLE_CYLINDER:
    case GUIMENU_STYLE_TRAPEZOID:
    case GUIMENU_STYLE_BINARY_STAR:
    case GUIMENU_STYLE_SHUTTER:
    case GUIMENU_STYLE_CHARIOT:
#endif
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemPtr)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetItemPtr(menu_ctrl_ptr, menu_ctrl_ptr->cur_page_index, menu_ctrl_ptr->cur_item_index, cur_item_ptr);
        }
        break;

    case GUIMENU_STYLE_SECOND:
    case GUIMENU_STYLE_THIRD:
    case GUIMENU_STYLE_ROTATE:
    case GUIMENU_STYLE_POPUP:
    case GUIMENU_STYLE_POPUP_RADIO:
    case GUIMENU_STYLE_POPUP_CHECK:
    case GUIMENU_STYLE_POPUP_AUTO:
    case GUIMENU_STYLE_POPUP_RECT:
        if (PNULL != menu_ctrl_ptr->cur_group_ptr)
        {
            temp_item_ptr = GetItemByVisibleIndex(menu_ctrl_ptr, menu_ctrl_ptr->cur_item_index, item_info_ptr);
            cur_item_ptr->menu_data_type = GUIMENU_MAINMENU_DATA_TYPE_STATIC;
            cur_item_ptr->menu_data.static_menu_ptr = temp_item_ptr;
        }
        break;

    default:
        //SCI_TRACE_LOW:"GetCurItemPtr:menu_style = %d is error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_2125_112_2_18_3_20_21_237,(uint8*)"d",menu_ctrl_ptr->cur_style);
        break;
    }

    return;
}

/*****************************************************************************/
//  Description : get item information
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetInfoInOK(
                          GUIMENU_CTRL_T        *menu_ctrl_ptr,
                          BOOLEAN               *is_gray_ptr,
                          BOOLEAN               *is_exist_child_ptr,
                          GUIMENU_ITEM_T        *cur_item_ptr,
                          GUIMENU_NODE_T        **cur_node_pptr
                          )
{
    BOOLEAN             result = FALSE;
    BOOLEAN             is_grayed = FALSE;
    BOOLEAN             is_exist_child = FALSE;
    MMI_MENU_GROUP_ID_T cur_group_id  = 0;
    GUIMENU_ITEM_T      cur_item = {0};
    GUIMENU_ITEM_INFO_T cur_item_info = {0};
    GUIMENU_GROUP_T     *menu_group_ptr = PNULL;
    GUIMENU_NODE_T      *cur_node_ptr = PNULL;

    if (menu_ctrl_ptr->is_static)
    {
        //get current menu item pointer
        if (GetCurItem(menu_ctrl_ptr, &cur_item, &cur_item_info))   // 有可能菜单项为0
        {
            //get current group id
            cur_group_id = GetCurGroupId(menu_ctrl_ptr);

            //judge item is gray
            is_grayed = MMITHEME_IsMenuItemGrayed(menu_ctrl_ptr->win_handle, cur_group_id, cur_item.menu_id);
            if (!is_grayed)
            {
                is_grayed = cur_item_info.is_grayed;
            }

            menu_group_ptr = (GUIMENU_GROUP_T *)MMITHEME_GetMenuGroup(cur_item.link_group_id);
            if (PNULL == menu_group_ptr)
            {
                is_exist_child = FALSE;
            }
            else
            {
                is_exist_child = TRUE;
            }

            result = TRUE;
        }
    }
    else
    {
        cur_node_ptr = GetNodeByVisibleIndex(menu_ctrl_ptr->cur_item_index,menu_ctrl_ptr->cur_parent_node_ptr);
        if (PNULL != cur_node_ptr)
        {
            is_grayed = cur_node_ptr->is_grayed;

            if (PNULL == cur_node_ptr->child_node_ptr)
            {
                is_exist_child = FALSE;
            }
            else
            {
                is_exist_child = TRUE;
            }

            result = TRUE;
        }
    }

    if (PNULL != is_gray_ptr)
    {
        *is_gray_ptr = is_grayed;
    }

    if (PNULL != is_exist_child_ptr)
    {
        *is_exist_child_ptr = is_exist_child;
    }

    if (PNULL != cur_item_ptr)
    {
        *cur_item_ptr = cur_item;
    }

    if (PNULL != cur_node_pptr)
    {
        *cur_node_pptr = cur_node_ptr;
    }

    return result;
}

/*****************************************************************************/
//  Description : restore menu info in ok
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void RestoreInfoInOK(
                           GUIMENU_CTRL_T        *menu_ctrl_ptr,
                           GUIMENU_ITEM_T        *cur_item_ptr,
                           GUIMENU_NODE_T        *cur_node_ptr
                           )
{
    if (menu_ctrl_ptr->is_static)
    {
        SCI_ASSERT(PNULL != cur_item_ptr);  /*assert verified*/

        menu_ctrl_ptr->cur_group_id         = cur_item_ptr->link_group_id;
        menu_ctrl_ptr->cur_group_ptr        = (GUIMENU_GROUP_T *)MMITHEME_GetMenuGroup(menu_ctrl_ptr->cur_group_id);
        menu_ctrl_ptr->cur_group_info_ptr   = GetGroupInfo(menu_ctrl_ptr, menu_ctrl_ptr->cur_group_id);
        menu_ctrl_ptr->item_total_num       = menu_ctrl_ptr->cur_group_info_ptr->visible_child_item_num;
        menu_ctrl_ptr->cur_style            = menu_ctrl_ptr->cur_group_ptr->menu_style;
        menu_ctrl_ptr->cur_button_style     = menu_ctrl_ptr->cur_group_info_ptr->button_style;
    }
    else
    {
        SCI_ASSERT(PNULL != cur_node_ptr);  /*assert verified*/

        menu_ctrl_ptr->cur_parent_node_ptr  = cur_node_ptr;
        menu_ctrl_ptr->item_total_num       = menu_ctrl_ptr->cur_parent_node_ptr->visible_child_node_num;
        menu_ctrl_ptr->cur_style            = menu_ctrl_ptr->cur_parent_node_ptr->sub_menu_style;
        menu_ctrl_ptr->cur_button_style     = menu_ctrl_ptr->cur_parent_node_ptr->button_style;
    }

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
    CheckSubMenuStyle(menu_ctrl_ptr);
#endif

    CheckButtonStyle(menu_ctrl_ptr);

    menu_ctrl_ptr->cur_item_index       = 0;
    menu_ctrl_ptr->first_item_index     = 0;
    menu_ctrl_ptr->is_first_disp        = TRUE;
}

/*****************************************************************************/
//  Description : notify parent control or parent window notify message
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E NotifyParentMsg(
                                   CAF_HANDLE_T     ctrl_handle,
                                   MMI_MESSAGE_ID_E msg_id
                                   )
{
    MMI_RESULT_E        result = MMI_RESULT_FALSE;
    MMI_MESSAGE_ID_E    notify_msg = MSG_NOTIFY_START;

    switch (msg_id)
    {
    case MSG_APP_WEB:
        notify_msg = MSG_NOTIFY_MIDSK;
        break;

    case MSG_APP_OK:
        notify_msg = MSG_NOTIFY_OK;
        break;

    case MSG_TP_PRESS_UP:
        notify_msg = MSG_NOTIFY_PENOK;
        break;

    case MSG_APP_CANCEL:
        notify_msg = MSG_NOTIFY_CANCEL;
        break;

    default:
        //SCI_TRACE_LOW:"== NotifyParentMsg == msg_id %d is not handled"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_2297_112_2_18_3_20_22_238,(uint8*)"d", msg_id);
        break;
    }

    if (MSG_NOTIFY_START != notify_msg)
    {
        //notify parent control
        result = GUICTRL_PostNotify(ctrl_handle,notify_msg);
    }
    
    return (result);
}

/*****************************************************************************/
//  Description : push menu in stack
//  Global resource dependence : 
//  Author: Jassmine
//  Note: this method is to store the current information and show sub menu of the current item.
/*****************************************************************************/
LOCAL void PushMenuStack(
                         GUIMENU_CTRL_T     *menu_ctrl_ptr 
                         )
{
    uint16      menu_level = menu_ctrl_ptr->menu_level;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }

    if (GUIMENU_STACK_MAX_LEVEL > menu_ctrl_ptr->menu_level)
    {
        menu_ctrl_ptr->menu_stack[menu_level].group_id         = menu_ctrl_ptr->cur_group_id;
        menu_ctrl_ptr->menu_stack[menu_level].first_item_index = menu_ctrl_ptr->first_item_index;
        menu_ctrl_ptr->menu_stack[menu_level].cur_item_index   = menu_ctrl_ptr->cur_item_index;
        menu_ctrl_ptr->menu_stack[menu_level].rect             = menu_ctrl_ptr->rect;
        menu_ctrl_ptr->menu_stack[menu_level].parent_node_ptr  = menu_ctrl_ptr->cur_parent_node_ptr;
        menu_ctrl_ptr->menu_stack[menu_level].offset_y         = menu_ctrl_ptr->offset_y;

        menu_ctrl_ptr->menu_level++;
    }
    else
    {
        //SCI_TRACE_LOW:"PushMenuStack: menu stack %d is full!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_2340_112_2_18_3_20_22_239,(uint8*)"d",menu_ctrl_ptr->menu_level);
    }
}

/*****************************************************************************/
//  Description : pop menu from stack
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void PopMenuStack(
                        GUIMENU_CTRL_T      *menu_ctrl_ptr 
                        )
{
    uint16      menu_level = 0;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }

    if (0 < menu_ctrl_ptr->menu_level)
    {
        menu_ctrl_ptr->menu_level--;

        menu_level                      = menu_ctrl_ptr->menu_level;
        menu_ctrl_ptr->cur_group_id     = menu_ctrl_ptr->menu_stack[menu_level].group_id;
        menu_ctrl_ptr->first_item_index = menu_ctrl_ptr->menu_stack[menu_level].first_item_index;
        menu_ctrl_ptr->cur_item_index   = menu_ctrl_ptr->menu_stack[menu_level].cur_item_index;
        menu_ctrl_ptr->rect             = menu_ctrl_ptr->menu_stack[menu_level].rect;
        menu_ctrl_ptr->offset_y         = menu_ctrl_ptr->menu_stack[menu_level].offset_y;
    }
    else
    {
        //SCI_TRACE_LOW:"PopMenuStack: menu stack %d is null!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_2374_112_2_18_3_20_22_240,(uint8*)"d",menu_ctrl_ptr->menu_level);
    }
}

/*****************************************************************************/
//  Description : reset menu, stop timer and free resource etc.
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void ResetMenuInfo(
                         GUIMENU_CTRL_T *menu_ctrl_ptr
                         )
{
    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }
    
    //stop item text timer
    StopItemTextTimer(menu_ctrl_ptr);
    menu_ctrl_ptr->item_str_index = 0;
    
    StopAutoMoveTimer(menu_ctrl_ptr);

    StopMoveStartTimer(menu_ctrl_ptr);

    StopShakeIconTimer(menu_ctrl_ptr);

    StopTitleIconDispTimer(menu_ctrl_ptr);

    StopCubeBackTimer(menu_ctrl_ptr);

    menu_ctrl_ptr->is_moving = FALSE;

    menu_ctrl_ptr->slide_state = MMK_TP_SLIDE_NONE;
}

/*****************************************************************************/
//  Description : handle menu cancel key
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleMenuCancelKey(
                                  GUIMENU_CTRL_T    *menu_ctrl_ptr,
                                  MMI_MESSAGE_ID_E  msg_id
                                  )
{
    BOOLEAN     is_handled = FALSE;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return FALSE;
    }

    if (menu_ctrl_ptr->is_move_state)
    {
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuCancelKey)
        {
            is_handled = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    HandleMenuCancelKey(menu_ctrl_ptr, msg_id);
        }
    }
    else
    {
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuCancelKey)
        {
            is_handled = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    HandleMenuCancelKey(menu_ctrl_ptr, msg_id);
        }

// PDA风格的二级菜单，返回时直接退出
#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
        if (!is_handled && 0 != menu_ctrl_ptr->menu_level
            && (GUIMENU_STYLE_THIRD == menu_ctrl_ptr->cur_style
                || GUIMENU_STYLE_SECOND == menu_ctrl_ptr->cur_style))
#else
        if (!is_handled && 0 != menu_ctrl_ptr->menu_level)
#endif
        {
            BOOLEAN             is_update = FALSE;
            uint16              start_level = 0;
#ifdef UI_P3D_SUPPORT
            BOOLEAN             is_pop_menu = GUIMENU_IsPopMenu(menu_ctrl_ptr);
            
            GUI_RECT_T          pre_menu_rect = menu_ctrl_ptr->rect;
            GUI_RECT_T          lcd_rect = MMITHEME_GetFullScreenRect();
            const GUI_LCD_DEV_INFO *lcd_dev_info = MMITHEME_GetDefaultLcdDev();


            if (is_pop_menu && MMITHEME_Is3DOpen())
            {
                MMIDEFAULT_SaveMoveToNewBuf();
            }
#endif
            //is need update previous pop-up menu
            is_update = IsUpdatePrevPopMenu(&start_level,menu_ctrl_ptr);

            //pop menu from stack
            PopMenuStack(menu_ctrl_ptr);

            //reset menu info
            ResetMenuInfo(menu_ctrl_ptr);

            // reset info 
            RestoreInfoInCancel(menu_ctrl_ptr);

            //destroy progress
            if (PNULL != menu_ctrl_ptr->prgbox_ctrl_ptr)
            {
                MMK_DestroyControl(menu_ctrl_ptr->prgbox_ctrl_ptr->handle);
                menu_ctrl_ptr->prgbox_ctrl_ptr = PNULL;
            }

            if (is_update)
            {
                //update previous pop-up menu
                UpdatePrevPopMenu(start_level,menu_ctrl_ptr);
            }

#ifdef UI_P3D_SUPPORT 
            if (!is_pop_menu && MMITHEME_Is3DOpen())
            {
                //save old lcd buf
                MMK_HandleWinMoveMsg(menu_ctrl_ptr->win_handle, MSG_CLOSE_WINDOW, PNULL);                 
            }
#endif
    
            //display previous menu
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                DisplayMenu(menu_ctrl_ptr);

#ifdef UI_P3D_SUPPORT
            //animate effect
            if (MMITHEME_Is3DOpen())
            {
                if (is_pop_menu)
                {
                    UILAYER_InvalidateRect( LCD_ID_0, &lcd_rect );
                    MMIDEFAULT_SaveOldMoveBuf(SCR_EFFECT_BUF_TYPE_WIN_SWITCH);

                    MMITHEME_PopupMenuEffect(&pre_menu_rect, start_level + 1, FALSE);
                }
                else
                {
                    MMITHEME_StoreUpdateRect(lcd_dev_info, lcd_rect);
                    //paint move 3d effect.
                    MMK_HandleWinMoveMsg(menu_ctrl_ptr->win_handle, MSG_FULL_PAINT, PNULL);                    
                }
            }
#endif

            is_handled = TRUE;
        }
    }

    if (!is_handled)
    {
        //notify parent control or parent window
        NotifyParentMsg(menu_ctrl_ptr->handle,MSG_APP_CANCEL);
    }

    return (is_handled);
}

/*****************************************************************************/
//  Description : 重置经过cancel后menu数据
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL void RestoreInfoInCancel(
                               GUIMENU_CTRL_T    *menu_ctrl_ptr
                               )
{
    if (menu_ctrl_ptr->is_static)
    {
        menu_ctrl_ptr->cur_group_ptr        = (GUIMENU_GROUP_T *)MMITHEME_GetMenuGroup(menu_ctrl_ptr->cur_group_id);
        menu_ctrl_ptr->cur_group_info_ptr   = GetGroupInfo(menu_ctrl_ptr, menu_ctrl_ptr->cur_group_id);
        menu_ctrl_ptr->item_total_num       = menu_ctrl_ptr->cur_group_info_ptr->visible_child_item_num;
        menu_ctrl_ptr->cur_style            = menu_ctrl_ptr->cur_group_ptr->menu_style;
        menu_ctrl_ptr->cur_button_style     = menu_ctrl_ptr->cur_group_info_ptr->button_style;
    }
    else
    {
        menu_ctrl_ptr->cur_parent_node_ptr  = menu_ctrl_ptr->cur_parent_node_ptr->parent_node_ptr;
        menu_ctrl_ptr->item_total_num       = menu_ctrl_ptr->cur_parent_node_ptr->visible_child_node_num;
        menu_ctrl_ptr->cur_style            = menu_ctrl_ptr->cur_parent_node_ptr->sub_menu_style;
        menu_ctrl_ptr->cur_button_style     = menu_ctrl_ptr->cur_parent_node_ptr->button_style;
    }

    CheckButtonStyle(menu_ctrl_ptr);

    menu_ctrl_ptr->is_first_disp = FALSE;
}

/*****************************************************************************/
//  Description : is update previous pop-up and return update start menu level
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 当前是pop-up menu,cancel后需要根据Rect判断是否刷新之前的pop-up menu
/*****************************************************************************/
LOCAL BOOLEAN IsUpdatePrevPopMenu(
                                  uint16            *level_ptr,     //in
                                  GUIMENU_CTRL_T    *menu_ctrl_ptr  //in
                                  )
{
    BOOLEAN         result = FALSE;
    BOOLEAN         is_covered = FALSE;
    uint16          i = 0;
    uint16          menu_level = 0;
    GUI_RECT_T      cur_rect = {0};
    GUI_RECT_T      prev_rect = {0};

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != level_ptr); /*assert verified*/
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if ((PNULL == menu_ctrl_ptr)||(PNULL == level_ptr))
    {
        return FALSE;
    }

    if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
    {
        cur_rect = menu_ctrl_ptr->rect;
        for (i=menu_ctrl_ptr->menu_level; i>0; i--)
        {
            prev_rect = menu_ctrl_ptr->menu_stack[i-1].rect;
            //前一个pop-up menu rect是否完全覆盖后面一个pop-up menu rect
            if (GUI_RectIsCovered(prev_rect,cur_rect))
            {
                is_covered = TRUE;
                menu_level = (uint16)(i- 1);
                break;
            }
        }

        //need previous pop-up menu
        //if (i != menu_ctrl_ptr->menu_level)
        {
            result = TRUE;
            //if (!is_covered)
            {
                //update previous window
                GUIMENU_UpdatePrevWindow(menu_ctrl_ptr);
            }
        }

        *level_ptr = menu_level;
    }

    return (result);
}

/*****************************************************************************/
//  Description : handle menu direction key
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleMenuDirectionKey(
                                          GUIMENU_CTRL_T    *menu_ctrl_ptr,
                                          MMI_MESSAGE_ID_E  msg_id
                                          )
{
    BOOLEAN         result = FALSE;
    MMI_RESULT_E    handle_result = MMI_RESULT_TRUE;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return MMI_RESULT_FALSE;
    }

    switch (msg_id)
    {
    case MSG_APP_UP:
    case MSG_KEYREPEAT_UP:
        handle_result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
            HandleMenuUpKey(menu_ctrl_ptr);
        break;

    case MSG_APP_DOWN:
    case MSG_KEYREPEAT_DOWN:
        handle_result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
            HandleMenuDownKey(menu_ctrl_ptr);
        break;

    case MSG_APP_LEFT:
    case MSG_KEYREPEAT_LEFT:
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuLeftKey)
        {
            result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                        HandleMenuLeftKey(menu_ctrl_ptr,msg_id);

            if (result)
            {
                if (MMI_DIRECTION_RIGHT == MMITHEME_GetDisDirection())
                {
                    //enter the next menu or notify ok msg
                    HandleMenuOkKey(menu_ctrl_ptr,MSG_APP_OK);
                }
                else
                {
                    //return the parent menu or notify cancel msg
                    HandleMenuCancelKey(menu_ctrl_ptr, MSG_APP_CANCEL);
                }
            }
        }
        break;

    case MSG_APP_RIGHT:
    case MSG_KEYREPEAT_RIGHT:
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuRightKey)
        {
            result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                        HandleMenuRightKey(menu_ctrl_ptr,msg_id);

            if (result)
            {
                if (MMI_DIRECTION_RIGHT == MMITHEME_GetDisDirection())
                {
                    //return the parent menu or notify cancel msg
                    HandleMenuCancelKey(menu_ctrl_ptr, MSG_APP_CANCEL);
                }
                else
                {
                    //enter the next menu or notify ok msg
                    HandleMenuOkKey(menu_ctrl_ptr,MSG_APP_OK);
                }
            }
        }
        break;

    default:
        //SCI_TRACE_LOW:"== HandleMenuDirectionKey == msg_id %d is not handled"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_2703_112_2_18_3_20_23_241,(uint8*)"d", msg_id);
        break;
    }

    return handle_result;
}

/*****************************************************************************/
//  Description : handle menu number key
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN HandleMenuNumKey(
                               GUIMENU_CTRL_T   *menu_ctrl_ptr,
                               MMI_MESSAGE_ID_E msg_id
                               )
{
    BOOLEAN             result = FALSE;
    uint16              i = 0;
    uint16              key_max_num = 0;
    uint16              num_idnex = 0;
    uint16              total_num = 0;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return FALSE;
    }

    //get number key index
    key_max_num = sizeof(s_guimenu_numkey_map)/sizeof(GUIMENU_NUMKEY_MAP_T);
    for (i=0; i<key_max_num; i++)
    {
        if (msg_id == s_guimenu_numkey_map[i].key_msg)
        {
            num_idnex = s_guimenu_numkey_map[i].num_index;
            break;
        }
    }

    //set total number
    if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetTotalItemNum)
    {
        total_num = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetTotalItemNum(menu_ctrl_ptr, menu_ctrl_ptr->cur_page_index);
    }
    else
    {
        total_num = menu_ctrl_ptr->item_total_num;
    }

    if (num_idnex < total_num)
    {
        //handle number key
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuNumKey)
        {
            result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                            HandleMenuNumKey(num_idnex,menu_ctrl_ptr);

            if (result)
            {
                //enter the next menu or notify ok msg
                result = HandleMenuOkKey(menu_ctrl_ptr,MSG_APP_WEB);
            }
        }
    }

    return (result);
}

/*****************************************************************************/
//  Description : handle menu tp down msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpDown(
                            DPARAM          param,
                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                            )
{
    GUI_POINT_T     tp_point = {0};

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }
    
    //set tp press point
    tp_point.x = MMK_GET_TP_X(param);
    tp_point.y = MMK_GET_TP_Y(param);

    ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
        HandleMenuTpDown(&tp_point,menu_ctrl_ptr);

    if (MMITHEME_IsIstyle() && GUIMENU_ISTYLE == menu_ctrl_ptr->cur_style)
    {
        MMITHEME_StopIdleSsTimer();
    }
}

/*****************************************************************************/
//  Description : handle menu tp up msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpUp(
                          DPARAM            param,
                          GUIMENU_CTRL_T    *menu_ctrl_ptr
                          )
{
    BOOLEAN         result = FALSE;
    GUI_POINT_T     tp_point = {0};

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }
    
    //set tp press point
    tp_point.x = MMK_GET_TP_X(param);
    tp_point.y = MMK_GET_TP_Y(param);

    result = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                HandleMenuTpUp(&tp_point,menu_ctrl_ptr);

    if (result)
    {
        //enter the next menu or notify ok msg
        HandleMenuOkKey(menu_ctrl_ptr,MSG_TP_PRESS_UP);
    }
    else
    {
        if ((GUIMENU_IsPopMenu(menu_ctrl_ptr)) && // pop menu
            (!GUI_PointIsInRect(tp_point,menu_ctrl_ptr->rect) // tp up点不在menu区域
            && !GUI_PointIsInRect(menu_ctrl_ptr->slide_start_point,menu_ctrl_ptr->rect) // 起始点也不在区域
            && MMK_TP_SLIDE_NONE == menu_ctrl_ptr->slide_state)) // 控件滑动状态为none
        {
            //return the parent menu or notify cancel msg
            HandleMenuCancelKey(menu_ctrl_ptr, MSG_TP_PRESS_UP);
        }
        else if (MMITHEME_IsIstyle() && GUIMENU_ISTYLE == menu_ctrl_ptr->cur_style)
        {
            MMITHEME_RestartIdleSsTimer();
        }
    }
}

/*****************************************************************************/
//  Description : handle menu tp move msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpMove(
                            DPARAM          param,
                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                            )
{
    GUI_POINT_T     tp_point = {0};

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }
    
    //set tp press point
    tp_point.x = MMK_GET_TP_X(param);
    tp_point.y = MMK_GET_TP_Y(param);

    ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
        HandleMenuTpMove(&tp_point,menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void StopItemTextTimer(
                             GUIMENU_CTRL_T     *menu_ctrl_ptr
                             )
{
    if (PNULL != menu_ctrl_ptr && 0 < menu_ctrl_ptr->item_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->item_timer_id);
        menu_ctrl_ptr->item_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartItemTextTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->item_timer_id)
    {
        menu_ctrl_ptr->item_timer_id = MMK_CreateWinTimer(menu_ctrl_ptr->handle,menu_ctrl_ptr->second_menu_theme.item_scroll_period,FALSE);
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartAutoMoveTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->tp_timer_id)
    {
        menu_ctrl_ptr->tp_timer_id = MMK_CreateWinTimer(
            menu_ctrl_ptr->handle, 
            menu_ctrl_ptr->common_theme.auto_move_period,
            TRUE );
    }
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopAutoMoveTimer(
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->tp_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->tp_timer_id);
        menu_ctrl_ptr->tp_timer_id = 0;
        menu_ctrl_ptr->continue_move_type = CONTINUE_MOVE_INVALID;
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartCubeBackTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->cube_back_timer_id)
    {
        menu_ctrl_ptr->cube_back_timer_id = MMK_CreateWinTimer(
            menu_ctrl_ptr->handle, 
            menu_ctrl_ptr->common_theme.cube_move_back_period,
            FALSE );
    }
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopCubeBackTimer(
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->cube_back_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->cube_back_timer_id);
        menu_ctrl_ptr->cube_back_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartMoveStartTimer(
                               GUIMENU_CTRL_T *menu_ctrl_ptr
                               )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->start_move_timer_id)
    {
        menu_ctrl_ptr->start_move_timer_id = MMK_CreateWinTimer(
            menu_ctrl_ptr->handle, 
            menu_ctrl_ptr->common_theme.icon_move_start_period,
            FALSE );
    }
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopMoveStartTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->start_move_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->start_move_timer_id);
        menu_ctrl_ptr->start_move_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartShakeIconTimer(
                                GUIMENU_CTRL_T *menu_ctrl_ptr
                                )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->shake_icon_timer_id)
    {
        menu_ctrl_ptr->shake_icon_timer_id = MMK_CreateWinTimer(
                    menu_ctrl_ptr->handle, 
                    menu_ctrl_ptr->common_theme.icon_shake_period,
                    FALSE);
    }
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopShakeIconTimer(
                              GUIMENU_CTRL_T *menu_ctrl_ptr
                              )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->shake_icon_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->shake_icon_timer_id);
        menu_ctrl_ptr->shake_icon_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartTitleIconDispTimer(
                                   GUIMENU_CTRL_T *menu_ctrl_ptr
                                   )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->titleidx_timer_id)
    {
        menu_ctrl_ptr->titleidx_timer_id = MMK_CreateWinTimer(
                    menu_ctrl_ptr->handle, 
                    menu_ctrl_ptr->common_theme.icon_move_start_period,
                    FALSE);
    }
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopTitleIconDispTimer(
                                  GUIMENU_CTRL_T *menu_ctrl_ptr
                                  )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->titleidx_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->titleidx_timer_id);
        menu_ctrl_ptr->titleidx_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopTitleIconDispTimer(
                                           GUIMENU_CTRL_T     *menu_ctrl_ptr
                                           )
{
    StopTitleIconDispTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartTitleIconDispTimer(
                                            GUIMENU_CTRL_T     *menu_ctrl_ptr
                                            )
{
    StartTitleIconDispTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopItemTextTimer(
                                      GUIMENU_CTRL_T     *menu_ctrl_ptr
                                      )
{
    StopItemTextTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartItemTextTimer(
                                       GUIMENU_CTRL_T     *menu_ctrl_ptr
                                       )
{
    StartItemTextTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartShakeIconTimer(
                                        GUIMENU_CTRL_T *menu_ctrl_ptr
                                        )
{
    StartShakeIconTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopShakeIconTimer(
                                       GUIMENU_CTRL_T *menu_ctrl_ptr
                                       )
{
    StopShakeIconTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartMoveTimer(
                                   GUIMENU_CTRL_T *menu_ctrl_ptr
                                   )
{
    StartMoveStartTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopMoveTimer(
                                  GUIMENU_CTRL_T *menu_ctrl_ptr
                                  )
{
    StopMoveStartTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartAutoSwitchTimer(
                                         GUIMENU_CTRL_T *menu_ctrl_ptr
                                         )
{
    StartAutoMoveTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopAutoSwitchTimer(
                                        GUIMENU_CTRL_T *menu_ctrl_ptr
                                        )
{
    StopAutoMoveTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartCubeBackTimer(
                                       GUIMENU_CTRL_T *menu_ctrl_ptr
                                       )
{
    StartCubeBackTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopCubeBackTimer(
                                      GUIMENU_CTRL_T *menu_ctrl_ptr
                                      )
{
    StopCubeBackTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : handle menu scroll bar touch panel msg
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void HandleMenuTpScroll(
                              uint16            first_item_index,
                              GUIMENU_CTRL_T    *menu_ctrl_ptr
                              )
{
    uint32 total_item_num = 0;
    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }
    //////////////////////////////////////////////////////////////////////////
    // modified by feng.xiao for NEWMS00169472
    if (GUIMENU_GetStatePtr(menu_ctrl_ptr, GUIMENU_STATE_PRG_PIXEL_SCROLL))
    {
        total_item_num = menu_ctrl_ptr->prgbox_ctrl_ptr->total_item_num;
    } 
    else
    {
        total_item_num = (uint32)menu_ctrl_ptr->item_total_num;
    }
    if (first_item_index < total_item_num)
    {
        //menu scroll bar handle tp
        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
            HandleMenuTpScroll(first_item_index,menu_ctrl_ptr);
    }
}

/*****************************************************************************/
//  Description : return menu group id and menu id
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_GetId(
                   MMI_CTRL_ID_T        ctrl_id,
                   MMI_MENU_GROUP_ID_T  *group_id_ptr,  //group id
                   MMI_MENU_ID_T        *menu_id_ptr    //menu id
                   )
{
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;
    GUIMENU_ITEM_T  cur_item = {0};

    if (PNULL == menu_id_ptr || PNULL == group_id_ptr)
    {
        return;
    }

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr && menu_ctrl_ptr->is_static)
    {
        //get current item pointer
        if (GetCurItem(menu_ctrl_ptr, &cur_item, PNULL))
        {
            *menu_id_ptr = cur_item.menu_id;
        }

        //get cur group id
        *group_id_ptr = GetCurGroupId(menu_ctrl_ptr);
    }
}

/*****************************************************************************/
//  Description : get menu pointer by control id
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_CTRL_T* GetMenuPtr(
                                 MMI_HANDLE_T   ctrl_handle
                                 )
{
    IGUICTRL_T  *ctrl_ptr = MMK_GetCtrlPtr(ctrl_handle);
    
    if (PNULL != ctrl_ptr)
    {
        if (!MenuTypeOf(ctrl_ptr))
        {
            ctrl_ptr = PNULL;
            //SCI_TRACE_LOW:"== GetMenuPtr == ctrl_handle 0x%x is not menu"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_3281_112_2_18_3_20_24_242,(uint8*)"d", ctrl_handle);
        }
    }
    
    return (GUIMENU_CTRL_T*)ctrl_ptr;
}

#if defined (IM_ENGINE_SOGOU)
GUIMENU_CTRL_T* GUIMENU_GetMenuPtr(
                                   MMI_HANDLE_T ctrl_handle
                                   )
{
    return (GUIMENU_CTRL_T*)GetMenuPtr(ctrl_handle);
}
#endif
/*****************************************************************************/
//  Description : menu type of
//  Global resource dependence : 
//  Author:Jassmine
//  Note: judge control type of
/*****************************************************************************/
LOCAL BOOLEAN MenuTypeOf(
                         IGUICTRL_T     *menu_ptr
                         )
{
    BOOLEAN     result = FALSE;

    if (SPRD_GUI_MENU_ID == IGUICTRL_GetCtrlGuid(menu_ptr))
    {
        result = TRUE;
    }

    return (result);
}

/*****************************************************************************/
//  Description : set menu the first and selected item
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_SetFirstAndSelectedItem(
                                     uint16         first_item_index,
                                     uint16         cur_item_index,
                                     MMI_CTRL_ID_T  ctrl_id
                                     )
{
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //set first and selected item
        if (0 == menu_ctrl_ptr->menu_level)
        {
            menu_ctrl_ptr->cur_item_index   = cur_item_index;
            menu_ctrl_ptr->first_item_index = first_item_index;
        }
        else
        {
            menu_ctrl_ptr->menu_stack[menu_ctrl_ptr->menu_level].first_item_index = first_item_index;
            menu_ctrl_ptr->menu_stack[menu_ctrl_ptr->menu_level].cur_item_index   = cur_item_index;
        }

        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->AdjustFirstAndDisplay)
        {
            //adjust menu first item
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                AdjustFirstAndDisplay(menu_ctrl_ptr, FALSE);
        }
    }
}

/*****************************************************************************/
//  Description : adjust top item index
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL uint16 AdjustTopItemIndex(
                                GUIMENU_CTRL_T  *menu_ctrl_ptr,
                                uint16 top_item_index
                                )
{
    uint16 page_item_num = 0;
    uint16 top_index = 0;

    top_index = top_item_index;

    if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetLineNumPage)
    {
        page_item_num = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->GetLineNumPage(menu_ctrl_ptr);

        if (top_item_index + (page_item_num - 1) < menu_ctrl_ptr->cur_item_index)
        {
            top_index = menu_ctrl_ptr->cur_item_index - (page_item_num - 1);
        }
    }

    return top_index;
}

/*****************************************************************************/
//  Description : set current selected item
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 可以自动调整top值
/*****************************************************************************/
void GUIMENU_SetSelectedItem(
                             MMI_CTRL_ID_T  ctrl_id,
                             uint16         cur_item_index
                             )
{
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;

    //get menu control pointer
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);

    if (PNULL != menu_ctrl_ptr)
    {
        //set first and selected item
        if (0 == menu_ctrl_ptr->menu_level)
        {
            menu_ctrl_ptr->cur_item_index = cur_item_index;

            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->AdjustFirstAndDisplay)
            {
                //adjust menu first item
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    AdjustFirstAndDisplay(menu_ctrl_ptr, FALSE);
            }
            menu_ctrl_ptr->first_item_index = AdjustTopItemIndex(menu_ctrl_ptr, menu_ctrl_ptr->first_item_index);
        }
        else
        {
            menu_ctrl_ptr->menu_stack[menu_ctrl_ptr->menu_level].cur_item_index = cur_item_index;

            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->AdjustFirstAndDisplay)
            {
                //adjust menu first item
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                    AdjustFirstAndDisplay(menu_ctrl_ptr, FALSE);
            }
            menu_ctrl_ptr->menu_stack[menu_ctrl_ptr->menu_level].first_item_index = AdjustTopItemIndex(menu_ctrl_ptr, menu_ctrl_ptr->first_item_index);
        }

        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->ResetDisplayItem)
        {
            int16 offset_y = 0;

            //adjust menu first item
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->ResetDisplayItem(menu_ctrl_ptr, &offset_y);
            menu_ctrl_ptr->offset_y += offset_y;
        }
    }
}

/*****************************************************************************/
//  Description :set menu title,for dynamic menu or volatile static menu title
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_SetMenuTitle(
                          MMI_STRING_T      *title_text_ptr,//title text
                          MMI_CTRL_ID_T     ctrl_id         //control id
                          )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    
    if (PNULL == title_text_ptr)
    {
        return;
    }

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //set menu title
        menu_ctrl_ptr->title_len = (uint16)(MIN(GUIMENU_TITLE_MAX_LEN,title_text_ptr->wstr_len));
        MMI_WSTRNCPY( menu_ctrl_ptr->title_wstr,GUIMENU_TITLE_MAX_LEN,
            title_text_ptr->wstr_ptr,title_text_ptr->wstr_len,
            menu_ctrl_ptr->title_len);
    }
}

/*****************************************************************************/
//  Description : replace string
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void ReplaceString(
                         MMI_STRING_T     *dst_str_ptr,
                         MMI_STRING_T     *src_str_ptr
                         )
{
    if (PNULL != dst_str_ptr && PNULL != src_str_ptr)
    {
        if (PNULL != dst_str_ptr->wstr_ptr)
        {
            SCI_FREE(dst_str_ptr->wstr_ptr);
            dst_str_ptr->wstr_len = 0;
        }

        if (PNULL != src_str_ptr->wstr_ptr && 0 != src_str_ptr->wstr_len)
        {
            dst_str_ptr->wstr_len = src_str_ptr->wstr_len;
            dst_str_ptr->wstr_ptr = SCI_ALLOC_APP((dst_str_ptr->wstr_len + 1) * sizeof(wchar));
            SCI_MEMSET(dst_str_ptr->wstr_ptr, 0, (dst_str_ptr->wstr_len + 1) * sizeof(wchar));

            MMI_WSTRNCPY(
                dst_str_ptr->wstr_ptr, dst_str_ptr->wstr_len,
                src_str_ptr->wstr_ptr, src_str_ptr->wstr_len,
                src_str_ptr->wstr_len);
        }
    }
}

/***************************************************************************//*!
//  Description :设置Title的扩展方法
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetMenuTitleEx(
   	                                  MMI_CTRL_ID_T             ctrl_id,        // [in]
          	                          GUIMENU_TITLE_INFO_T      *title_info_ptr // [in]
                                      )
{
    BOOLEAN                 result = FALSE;
    GUIMENU_CTRL_T          *menu_ctrl_ptr  = PNULL;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;
    GUIMENU_NODE_T          *root_node_ptr  = PNULL;
    GUIMENU_NODE_T          *node_ptr       = PNULL;

    if (PNULL == title_info_ptr)
    {
        return FALSE;
    }

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        if (menu_ctrl_ptr->is_static && title_info_ptr->is_static)
        {
            group_info_ptr = GetGroupInfo(menu_ctrl_ptr, title_info_ptr->group_id);
            if (PNULL != group_info_ptr)
            {
                ReplaceString(&group_info_ptr->title_str, title_info_ptr->title_ptr);
                ReplaceString(&group_info_ptr->sub_title_str, title_info_ptr->sub_title_ptr);

                if (0 != title_info_ptr->title_icon_id)
                {
                    group_info_ptr->title_icon_id = title_info_ptr->title_icon_id;
                }

                result = TRUE;
            }
        }
        else if (!menu_ctrl_ptr->is_static && !title_info_ptr->is_static)
        {
            //get parent node pointer
            root_node_ptr = menu_ctrl_ptr->root_node_ptr;
            node_ptr = FindNodeById(title_info_ptr->node_id, root_node_ptr);

            if (PNULL != node_ptr)
            {
                ReplaceString(&node_ptr->title_str, title_info_ptr->title_ptr);
                ReplaceString(&node_ptr->sub_title_str, title_info_ptr->sub_title_ptr);

                if (0 != title_info_ptr->title_icon_id)
                {
                    node_ptr->title_icon_id = title_info_ptr->title_icon_id;
                }

                result = TRUE;
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : creat dynamic menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_CreatDynamic(
                          GUI_BOTH_RECT_T  *rect_ptr,  //may PNULL,pop-up no need rect
                          MMI_WIN_ID_T      win_id,
                          MMI_CTRL_ID_T     ctrl_id,
                          GUIMENU_STYLE_E   menu_style  //menu style
                          )
{
    GUIMENU_CreatDynamicEx(
            GUIMENU_DYNA_NODE_MAX_NUM, rect_ptr, win_id, ctrl_id, menu_style);
}

/*****************************************************************************/
//  Description : creat dynamic menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_CreatDynamicEx(
                            uint16          node_max_num,   //in:
                            GUI_BOTH_RECT_T *rect_ptr,      //may PNULL,pop-up no need rect
                            MMI_WIN_ID_T    win_id,
                            MMI_CTRL_ID_T   ctrl_id,
                            GUIMENU_STYLE_E menu_style      //menu style
                            )
{
    BOOLEAN                 is_param_ok = TRUE;
    GUIMENU_INIT_DATA_T     init_data = {0};
    MMI_CONTROL_CREATE_T    ctrl_create = {0};

    //set init data
    init_data.is_static  = FALSE;
    init_data.node_max_num = node_max_num;
    init_data.menu_style = menu_style;
    if (PNULL != rect_ptr)
    {
        init_data.both_rect = *rect_ptr;
    }

    switch (menu_style)
    {
    case GUIMENU_STYLE_NULL:
    case GUIMENU_STYLE_ICON:
    case GUIMENU_STYLE_TITLE_ICON:
    case GUIMENU_STYLE_ROTATE:
    case GUIMENU_STYLE_OPTION_PAGE:
    case GUIMENU_ISTYLE:
    case GUIMENU_STYLE_CUBE:
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
	case GUIMENU_STYLE_GO:
#else
    case GUIMENU_STYLE_SLIDE_PAGE:
    case GUIMENU_STYLE_SPHERE:
    case GUIMENU_STYLE_CYLINDER:
    case GUIMENU_STYLE_TRAPEZOID:
    case GUIMENU_STYLE_BINARY_STAR:
    case GUIMENU_STYLE_SHUTTER:
    case GUIMENU_STYLE_CHARIOT:
#endif  
        //SCI_TRACE_LOW:"== GUIMENU_CreatDynamic == menu_style %d is error!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_3614_112_2_18_3_20_24_243,(uint8*)"d",menu_style);
        is_param_ok = FALSE;
        break;

    case GUIMENU_STYLE_SECOND:
    case GUIMENU_STYLE_THIRD:
        if (PNULL == rect_ptr)
        {
            //SCI_TRACE_LOW:"== GUIMENU_CreatDynamic == rect_ptr is PNULL"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_3622_112_2_18_3_20_24_244,(uint8*)"");
            is_param_ok = FALSE;
        }
        break;

    default:
        break;
    }

    if (is_param_ok)
    {
        //set control param
        ctrl_create.ctrl_id           = ctrl_id;
        ctrl_create.guid              = SPRD_GUI_MENU_ID;
        ctrl_create.init_data_ptr     = &init_data;
        ctrl_create.parent_win_handle = win_id;

        //creat control
        MMK_CreateControl(&ctrl_create);
    }
}

/*****************************************************************************/
//  Description : in dynamic menu, insert node
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_InsertNode(
                        uint16                  nodex_index,    //node index,from 0
                        uint32                  node_id,        //node id,from 1
                        uint32                  parent_node_id, //parent node id
                        GUIMENU_DYNA_ITEM_T     *node_item_ptr, //node item
                        MMI_CTRL_ID_T           ctrl_id         //control id
                        )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T      *root_node_ptr = PNULL;
    GUIMENU_NODE_T      *parent_node_ptr = PNULL;
    GUIMENU_NODE_T      *new_node_ptr = PNULL;
    
    if (PNULL == node_item_ptr)
    {
        return;
    }

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        parent_node_ptr = FindNodeById(parent_node_id,root_node_ptr);

        if (PNULL != parent_node_ptr)
        {
            //creat new node
            new_node_ptr = CreatNewNode(&menu_ctrl_ptr->node_memory);
        
            //set node values
            new_node_ptr->node_id = node_id;

            new_node_ptr->is_visible = TRUE;
            new_node_ptr->is_grayed = node_item_ptr->is_grayed;

            // init style is inherited from parent style and style of root
            // node is set by creating.
            new_node_ptr->sub_menu_style = parent_node_ptr->sub_menu_style;

            new_node_ptr->node_wstr_len = node_item_ptr->item_text_ptr->wstr_len;
            new_node_ptr->node_wstr_ptr = SCI_ALLOC_APP((new_node_ptr->node_wstr_len + 1) * sizeof(wchar));
            SCI_MEMSET(new_node_ptr->node_wstr_ptr, 0, (new_node_ptr->node_wstr_len + 1) * sizeof(wchar));

            MMI_WSTRNCPY( new_node_ptr->node_wstr_ptr, new_node_ptr->node_wstr_len,
                node_item_ptr->item_text_ptr->wstr_ptr, node_item_ptr->item_text_ptr->wstr_len,
                new_node_ptr->node_wstr_len);

            new_node_ptr->select_icon_id = node_item_ptr->select_icon_id;

            //insert node
            InsertNewNode(nodex_index,new_node_ptr,parent_node_ptr);
        }

        //set item count
        if (parent_node_id == menu_ctrl_ptr->cur_parent_node_ptr->node_id)
        {
            menu_ctrl_ptr->item_total_num = menu_ctrl_ptr->cur_parent_node_ptr->visible_child_node_num;
        }
    }
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : in dynamic menu, replace node, 携带icon变更
//  Global resource dependence : 
//  Author: sky.qgp
//  Note:
/*****************************************************************************/
void GUIMENU_ReplaceNodeEx(
							uint16          node_index,    //node index,from 0
							uint32          node_id,        //node id,from 1
							uint32          parent_node_id, //parent node id
							GUIMENU_DYNA_ITEM_T     *node_item_ptr, //node item
							MMI_CTRL_ID_T           ctrl_id         //control id
							)
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T      *root_node_ptr = PNULL;
    GUIMENU_NODE_T      *parent_node_ptr = PNULL;
    GUIMENU_NODE_T      *cur_node_ptr = PNULL;

	if(PNULL == node_item_ptr)
	{
		return;
	}

    //get menu control pointer
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
	if(PNULL == menu_ctrl_ptr)
	{
		return;
	}
	
    //get parent node pointer
    root_node_ptr = menu_ctrl_ptr->root_node_ptr;
    parent_node_ptr = FindNodeById(parent_node_id,root_node_ptr);

    if (PNULL != parent_node_ptr)
    {
		cur_node_ptr = GUIMENU_FindNodeByIndex(node_index, parent_node_ptr);
		if(PNULL == cur_node_ptr)
		{
			return;
		}
		
		//set node values
		cur_node_ptr->node_id = node_id;
		cur_node_ptr->node_wstr_len = node_item_ptr->item_text_ptr->wstr_len;
        if(cur_node_ptr->node_wstr_ptr != NULL)
        {
            SCI_FREE(cur_node_ptr->node_wstr_ptr);
            cur_node_ptr->node_wstr_ptr = NULL;
        }
        cur_node_ptr->node_wstr_ptr = SCI_ALLOC_APP((cur_node_ptr->node_wstr_len + 1) * sizeof(wchar));
        SCI_MEMSET(cur_node_ptr->node_wstr_ptr, 0, (cur_node_ptr->node_wstr_len + 1) * sizeof(wchar));
		if(NULL == cur_node_ptr->node_wstr_ptr)
		{
			return;
		}

		if(PNULL != node_item_ptr->item_text_ptr)
		{
			MMI_WSTRNCPY( cur_node_ptr->node_wstr_ptr, cur_node_ptr->node_wstr_len,
				node_item_ptr->item_text_ptr->wstr_ptr, node_item_ptr->item_text_ptr->wstr_len, cur_node_ptr->node_wstr_len );
			if(node_item_ptr->select_icon_id != 0)
			{
				cur_node_ptr->select_icon_id = node_item_ptr->select_icon_id;
			}
		}
    }
}
#endif
/*****************************************************************************/
//  Description : find node pointer by node id
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* FindNodeById(
                                   uint32           node_id,        //node id
                                   GUIMENU_NODE_T   *root_node_ptr  //root node pointer
                                   )
{
    GUIMENU_NODE_T      *node_ptr = PNULL;

    if (PNULL != root_node_ptr)
    {
        //find child node
        node_ptr = FindNodeById(node_id,root_node_ptr->child_node_ptr);
        if (PNULL == node_ptr)
        {
            //find neighbour node
            node_ptr = FindNodeById(node_id,root_node_ptr->neighbour_node_ptr);
            if (PNULL == node_ptr)
            {
                //compare node id
                if (node_id == root_node_ptr->node_id)
                {
                    node_ptr = root_node_ptr;
                }
            }
        }
    }

    return (node_ptr);
}

/*****************************************************************************/
//  Description : insert new node in dynamic menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void InsertNewNode(
                         uint16             new_node_index, //new node index,from o
                         GUIMENU_NODE_T     *new_node_ptr,  //new node pointer
                         GUIMENU_NODE_T     *parent_node_ptr//parent node pointer
                         )
{
    uint16              i = 0;
    GUIMENU_NODE_T      *previous_node_ptr = PNULL;
    
    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != new_node_ptr);      /*assert verified*/
    //SCI_ASSERT(PNULL != parent_node_ptr);   /*assert verified*/
    if ((PNULL == new_node_ptr)||(PNULL == parent_node_ptr))
    {
        return;
    }

    //set parent node chile number
    parent_node_ptr->child_node_num++;
    parent_node_ptr->visible_child_node_num++;

    //set new node parent
    new_node_ptr->parent_node_ptr = parent_node_ptr;

    // add to header
    if (0 == new_node_index || PNULL == parent_node_ptr->child_node_ptr)
    {
        new_node_ptr->neighbour_node_ptr = parent_node_ptr->child_node_ptr;
        parent_node_ptr->child_node_ptr = new_node_ptr;
    }
    else
    {
        previous_node_ptr = parent_node_ptr->child_node_ptr;

        for (i = 1; i < new_node_index && PNULL != previous_node_ptr->neighbour_node_ptr; i++)
        {
            previous_node_ptr = previous_node_ptr->neighbour_node_ptr;
        }

        SCI_ASSERT(PNULL != previous_node_ptr); /*assert verified*/
        new_node_ptr->neighbour_node_ptr = previous_node_ptr->neighbour_node_ptr;
        previous_node_ptr->neighbour_node_ptr = new_node_ptr;
    }
}

/*****************************************************************************/
//  Description : in dynamic menu, replace node
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_ReplaceNode(
                         uint16          nodex_index,    //node index,from 0
                         uint32          node_id,        //node id,from 1
                         uint32          parent_node_id, //parent node id
                         MMI_STRING_T    *node_text_ptr, //node text
                         MMI_CTRL_ID_T   ctrl_id         //control id
                         )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T      *root_node_ptr = PNULL;
    GUIMENU_NODE_T      *parent_node_ptr = PNULL;
    GUIMENU_NODE_T      *cur_node_ptr = PNULL;

    if (PNULL == node_text_ptr)
    {
        return;
    }

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        parent_node_ptr = FindNodeById(parent_node_id,root_node_ptr);

        if (PNULL != parent_node_ptr)
        {
            cur_node_ptr = GUIMENU_FindNodeByIndex(nodex_index, parent_node_ptr);
            if (PNULL != cur_node_ptr)
            {
                //set node values
                cur_node_ptr->node_id = node_id;

                if (PNULL != cur_node_ptr->node_wstr_ptr)
                {
                    SCI_FREE(cur_node_ptr->node_wstr_ptr);
                }

                cur_node_ptr->node_wstr_len = node_text_ptr->wstr_len;
                cur_node_ptr->node_wstr_ptr = SCI_ALLOC_APP((cur_node_ptr->node_wstr_len + 1) * sizeof(wchar));
                SCI_MEMSET(cur_node_ptr->node_wstr_ptr, 0, (cur_node_ptr->node_wstr_len + 1) * sizeof(wchar));

                MMI_WSTRNCPY( cur_node_ptr->node_wstr_ptr,cur_node_ptr->node_wstr_len,
                    node_text_ptr->wstr_ptr,node_text_ptr->wstr_len,
                    cur_node_ptr->node_wstr_len);

                //set item count
                if (parent_node_id == menu_ctrl_ptr->cur_parent_node_ptr->node_id)
                {
                    menu_ctrl_ptr->item_total_num = menu_ctrl_ptr->cur_parent_node_ptr->child_node_num;
                }
            }
        }
    }
}

/*****************************************************************************/
//  Description : in dynamic menu, replace node
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_SetNodeGrayed(
                           BOOLEAN          is_grayed,      //set node grayed
                           uint16           nodex_index,    //node index,from 0
                           uint32           parent_node_id, //parent node id
                           MMI_CTRL_ID_T    ctrl_id         //control id
                           )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T      *root_node_ptr = PNULL;
    GUIMENU_NODE_T      *parent_node_ptr = PNULL;
    GUIMENU_NODE_T      *cur_node_ptr = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        parent_node_ptr = FindNodeById(parent_node_id,root_node_ptr);

        if (PNULL != parent_node_ptr)
        {
            cur_node_ptr = GUIMENU_FindNodeByIndex(nodex_index,parent_node_ptr);
            if (PNULL != cur_node_ptr)
            {
                //set node is grayed
                cur_node_ptr->is_grayed = is_grayed;
            }
        }
    }
}

/*****************************************************************************/
//  Description : in dynamic menu, set node grayed
//  Global resource dependence : 
//  Author: aoke.hu
//  Note:
/*****************************************************************************/
void GUIMENU_SetNodeGrayedByNodeId(
                           BOOLEAN          is_grayed,      //set node grayed
                           uint32           node_id,                // [in]
                           uint32           parent_node_id, //parent node id
                           MMI_CTRL_ID_T    ctrl_id         //control id
                           )
{
    uint16 node_index = 0;

    if(GUIMENU_GetIndexByNodeId(ctrl_id,parent_node_id,node_id,&node_index))
    {
        GUIMENU_SetNodeGrayed(is_grayed,node_index,parent_node_id,ctrl_id);
    }
}

/*****************************************************************************/
//  Description : find node pointer by index
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
GUIMENU_NODE_T* GUIMENU_FindNodeByIndex(
                                        uint16          node_index,
                                        GUIMENU_NODE_T  *parent_node_ptr
                                        )
{
    uint16          i = 0;
    GUIMENU_NODE_T  *find_node_ptr = PNULL;

    if (PNULL == parent_node_ptr || PNULL == parent_node_ptr->child_node_ptr)
    {
        return find_node_ptr;
    }

    find_node_ptr = parent_node_ptr->child_node_ptr;
    for (i=0; i<node_index; i++)
    {
        find_node_ptr = find_node_ptr->neighbour_node_ptr;
    }

    return (find_node_ptr);
}

/*****************************************************************************/
//  Description : return dynamic menu current node id
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
uint32 GUIMENU_GetCurNodeId(
                            MMI_CTRL_ID_T   ctrl_id //control id
                            )
{
    uint32          node_id = 0;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T  *cur_node_ptr = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //get current node pointer
        cur_node_ptr = GetNodeByVisibleIndex(menu_ctrl_ptr->cur_item_index,menu_ctrl_ptr->cur_parent_node_ptr);
        if (PNULL != cur_node_ptr)
        {
            node_id = cur_node_ptr->node_id;
        }
        else
        {
            //SCI_TRACE_LOW:"== GUIMENU_GetCurNodeId == error when GetNodeByVisibleIndex"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_4028_112_2_18_3_20_25_245,(uint8*)"");
        }
    }
    
    return (node_id);
}

/*****************************************************************************/
//  Description : set pop-up menu item status for radio or check pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_SetPopItemStatus(
                              BOOLEAN                   is_selected,
                              GUIMENU_POP_SEL_INFO_T    *selected_info_ptr
                              )
{
    GUIMENU_CTRL_T          *menu_ctrl_ptr = PNULL;
    GUIMENU_POP_SEL_ITEM_T  *sel_item_ptr = PNULL;

    if (PNULL == selected_info_ptr)
    {
        return;
    }
    
    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(selected_info_ptr->ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //convert handle
        selected_info_ptr->ctrl_id = MMK_ConvertIdToHandle(selected_info_ptr->ctrl_id);

        //is exist
        sel_item_ptr = IsExistSelItem(selected_info_ptr,&menu_ctrl_ptr->selected_list);
    
        if (is_selected)
        {
            if (PNULL == sel_item_ptr)
            {
                //creat new selected item
                sel_item_ptr = CreatNewSelItem(selected_info_ptr);

                //add list
                AddSelItemToList(sel_item_ptr,&menu_ctrl_ptr->selected_list);
            }
            else
            {
                //SCI_TRACE_LOW:"GUIMENU_SetPopItemStatus:selected item has exist"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_4074_112_2_18_3_20_25_246,(uint8*)"");
            }
        }
        else
        {
            if (PNULL == sel_item_ptr)
            {
                //SCI_TRACE_LOW:"GUIMENU_SetPopItemStatus:selected item not exist"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_4081_112_2_18_3_20_25_247,(uint8*)"");
            }
            else
            {
                //delete the selected item
                DeleteSelItem(sel_item_ptr,&menu_ctrl_ptr->selected_list);
            }
        }
    }
}

/*****************************************************************************/
// 获取radio或check风格处于选中状态的菜单项
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC uint16 GUIMENU_GetCheckInfo(
                                   MMI_CTRL_ID_T          ctrl_id,
                                   GUIMENU_CHECK_INFO_T   *check_info_ptr,
                                   uint16                 array_len
                                   )
{
    GUIMENU_CTRL_T          *menu_ctrl_ptr = PNULL;
    GUIMENU_POP_SEL_ITEM_T  *item_ptr = PNULL;
    uint16                  check_count = 0;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        item_ptr = menu_ctrl_ptr->selected_list.init_item_ptr;

        while (PNULL != item_ptr)
        {
            if (check_count < array_len && PNULL != check_info_ptr)
            {
                check_info_ptr[check_count].group_id = item_ptr->item_info.group_id;
                check_info_ptr[check_count].menu_id = item_ptr->item_info.menu_id;
                check_info_ptr[check_count].node_id = item_ptr->item_info.node_id;
            }

            check_count++;

            item_ptr = item_ptr->next_ptr;
        }
    }

    return check_count;
}

/*****************************************************************************/
//  Description : selected item is exist for radio/check pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_POP_SEL_ITEM_T* IsExistSelItem(
                                             GUIMENU_POP_SEL_INFO_T     *selected_info_ptr, //in
                                             GUIMENU_POP_SELECT_LIST_T  *selected_list_ptr  //in
                                             )
{
    GUIMENU_POP_SEL_ITEM_T  *item_ptr = PNULL;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != selected_info_ptr); /*assert verified*/
    //SCI_ASSERT(PNULL != selected_list_ptr); /*assert verified*/
    if ((PNULL == selected_info_ptr)||(PNULL == selected_list_ptr))
    {
        return PNULL;
    }

    item_ptr = selected_list_ptr->init_item_ptr;
    while (PNULL != item_ptr)
    {
        if ((item_ptr->item_info.is_static == selected_info_ptr->is_static) &&
            (item_ptr->item_info.node_id == selected_info_ptr->node_id) &&
            (item_ptr->item_info.ctrl_id == selected_info_ptr->ctrl_id) &&
            (item_ptr->item_info.menu_id == selected_info_ptr->menu_id) &&
            (item_ptr->item_info.group_id == selected_info_ptr->group_id))
        {
            break;
        }
        item_ptr = item_ptr->next_ptr;
    }

    return (item_ptr);
}

/*****************************************************************************/
//  Description : creat one new selected item
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_POP_SEL_ITEM_T* CreatNewSelItem(
                                              GUIMENU_POP_SEL_INFO_T    *selected_info_ptr
                                              )
{
    GUIMENU_POP_SEL_ITEM_T  *sel_item_ptr = PNULL;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != selected_info_ptr); /*assert verified*/
    if (PNULL == selected_info_ptr)
    {
        return PNULL;
    }

    //alloc selected item memory
    sel_item_ptr = SCI_ALLOC_APP(sizeof(GUIMENU_POP_SEL_ITEM_T));
    SCI_MEMSET(sel_item_ptr,0,sizeof(GUIMENU_POP_SEL_ITEM_T));

    //set selected item memory
    sel_item_ptr->item_info.is_static = selected_info_ptr->is_static;
    sel_item_ptr->item_info.node_id   = selected_info_ptr->node_id;
    sel_item_ptr->item_info.ctrl_id   = selected_info_ptr->ctrl_id;
    sel_item_ptr->item_info.menu_id   = selected_info_ptr->menu_id;
    sel_item_ptr->item_info.group_id  = selected_info_ptr->group_id;

    return (sel_item_ptr);
}

/*****************************************************************************/
//  Description : add one new selected item to list
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void AddSelItemToList(
                            GUIMENU_POP_SEL_ITEM_T      *sel_item_ptr,      //in
                            GUIMENU_POP_SELECT_LIST_T   *selected_list_ptr  //in
                            )
{
    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != selected_list_ptr); /*assert verified*/
    if (PNULL == selected_list_ptr)
    {
        return;
    }

    if (PNULL != sel_item_ptr)
    {
        if (PNULL == selected_list_ptr->init_item_ptr)
        {
            selected_list_ptr->init_item_ptr = sel_item_ptr;
        }
        else
        {
            sel_item_ptr->prev_ptr = selected_list_ptr->last_item_ptr;
            selected_list_ptr->last_item_ptr->next_ptr = sel_item_ptr;
        }

        selected_list_ptr->last_item_ptr = sel_item_ptr;
    }
}

/*****************************************************************************/
//  Description : delete one selected item from list
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void DeleteSelItem(
                         GUIMENU_POP_SEL_ITEM_T     *sel_item_ptr,      //in
                         GUIMENU_POP_SELECT_LIST_T  *selected_list_ptr  //in
                         )
{
    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != sel_item_ptr); /*assert verified*/
    //SCI_ASSERT(PNULL != selected_list_ptr); /*assert verified*/
    if ((PNULL == sel_item_ptr)||(PNULL == selected_list_ptr))
    {
        return;
    }

    if (PNULL == sel_item_ptr->prev_ptr)
    {
        if (PNULL == sel_item_ptr->next_ptr)
        {
            selected_list_ptr->init_item_ptr = PNULL;
            selected_list_ptr->last_item_ptr = PNULL;
        }
        else
        {
            sel_item_ptr->next_ptr->prev_ptr = PNULL;
            selected_list_ptr->init_item_ptr = sel_item_ptr->next_ptr;
        }
    }
    else
    {
        if (PNULL == sel_item_ptr->next_ptr)
        {
            sel_item_ptr->prev_ptr->next_ptr = PNULL;
            selected_list_ptr->last_item_ptr = sel_item_ptr->prev_ptr;
        }
        else
        {
            sel_item_ptr->next_ptr->prev_ptr = sel_item_ptr->prev_ptr;
            sel_item_ptr->prev_ptr->next_ptr = sel_item_ptr->next_ptr;
        }
    }

    SCI_FREE(sel_item_ptr);
}

/*****************************************************************************/
//  Description : free all selected item memory
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL void FreeSelListMemory(
                             GUIMENU_CTRL_T     *menu_ctrl_ptr
                             )
{
    GUIMENU_POP_SEL_ITEM_T  *sel_item_ptr = PNULL;
    GUIMENU_POP_SEL_ITEM_T  *free_item_ptr = PNULL;

    sel_item_ptr = menu_ctrl_ptr->selected_list.init_item_ptr;
    while(PNULL != sel_item_ptr)
    {
        free_item_ptr = sel_item_ptr;
        sel_item_ptr = sel_item_ptr->next_ptr;
        SCI_FREE(free_item_ptr);
    }
}

/*****************************************************************************/
//  Description : set pop-up menu item status for radio or check pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
BOOLEAN GUIMENU_GetPopItemStatus(
                                 GUIMENU_POP_SEL_INFO_T *selected_info_ptr
                                 )
{
    BOOLEAN                 result = FALSE;
    GUIMENU_CTRL_T          *menu_ctrl_ptr = PNULL;
    GUIMENU_POP_SEL_ITEM_T  *item_ptr = PNULL;

    if (PNULL == selected_info_ptr)
    {
        return result;
    }
    
    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(selected_info_ptr->ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //convert handle
        selected_info_ptr->ctrl_id = MMK_ConvertIdToHandle(selected_info_ptr->ctrl_id);

        item_ptr = menu_ctrl_ptr->selected_list.init_item_ptr;
        while (PNULL != item_ptr)
        {
            if ((item_ptr->item_info.is_static == selected_info_ptr->is_static) &&
                (item_ptr->item_info.node_id == selected_info_ptr->node_id) &&
                (item_ptr->item_info.ctrl_id == selected_info_ptr->ctrl_id) &&
                (item_ptr->item_info.menu_id == selected_info_ptr->menu_id) &&
                (item_ptr->item_info.group_id == selected_info_ptr->group_id))
            {
                result = TRUE;
                break;
            }
            item_ptr = item_ptr->next_ptr;
        }
    }

    return (result);
}

/*****************************************************************************/
//  Description : set menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_SetRect(
                            MMI_HANDLE_T    ctrl_handle,
                            GUI_RECT_T      *rect_ptr
                            )
{
    GUI_RECT_T      menu_rect = {0};
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_handle);
    if (PNULL != menu_ctrl_ptr)
    {
        menu_rect = menu_ctrl_ptr->rect;
        //set rect
        menu_ctrl_ptr->rect = *rect_ptr;

        GUI_SetVOrHRect( menu_ctrl_ptr->handle, rect_ptr,&(menu_ctrl_ptr->both_rect));

        // 当区域不相等的时候，需要重新初始化
        if (!GUI_EqualRect(menu_rect, menu_ctrl_ptr->rect))
        {
            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DestoryMenu)
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DestoryMenu(menu_ctrl_ptr);
            }

            if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu)
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu(menu_ctrl_ptr);
            }
        }
    }
}

/*****************************************************************************/
//  Description : in dynamic menu, set menu softkey
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
void GUIMENU_SetDynamicMenuSoftkey(
                                   MMI_CTRL_ID_T    ctrl_id,
                                   MMI_TEXT_ID_T    left_softkey_text_id,
                                   MMI_TEXT_ID_T    middle_softkey_text_id,
                                   MMI_TEXT_ID_T    right_softkey_text_id
                                   )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    
    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        menu_ctrl_ptr->dyna_softkey_id[0] = left_softkey_text_id;
        menu_ctrl_ptr->dyna_softkey_id[1] = middle_softkey_text_id;
        menu_ctrl_ptr->dyna_softkey_id[2] = right_softkey_text_id;
    }
}

/*****************************************************************************/
//  Description : set menu background color
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_SetMenuBgColor(
                                   MMI_CTRL_ID_T    ctrl_id,
                                   GUI_COLOR_T		bg_color
                                   )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    
    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //set background color
        menu_ctrl_ptr->common_theme.bg_color = bg_color;
        menu_ctrl_ptr->common_theme.bg_image = 0;
    }
}

/*****************************************************************************/
//  Description : get menu font color
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC GUI_COLOR_T GUIMENU_GetMenuFontColor(
                                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                                            )
{
    GUI_COLOR_T     font_color = 0;

    if (menu_ctrl_ptr->is_set_font_color)
    {
        font_color = menu_ctrl_ptr->font_color;
    }
    else
    {
        if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
        {
            font_color = MMITHEME_GetCurThemeFontColor(MMI_THEME_POPMENU);
        }
        else if (GUIMENU_IsMainMenu(menu_ctrl_ptr))
        {
            font_color = MMITHEME_GetCurThemeFontColor(MMI_THEME_MAINMENU_FONT);
        }
        else
        {
            font_color = MMITHEME_GetCurThemeFontColor(MMI_THEME_MENU);
        }
    }
    
    return (font_color);
}

/*****************************************************************************/
//  Description : get main menu font color
//  Global resource dependence : 
//  Author: hongbo.lan
//  Note:
/*****************************************************************************/
PUBLIC GUI_COLOR_T GUIMENU_GetMMSoftkeyFontColor(
                                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                                            )
{
    GUI_COLOR_T     font_color = 0;

    if (GUIMENU_IsMainMenu(menu_ctrl_ptr))
    {    
        font_color = MMITHEME_GetCurThemeFontColor(MMI_THEME_SOFTKEY_FONT);
    }

    return (font_color);
}                                            

/*****************************************************************************/
//  Description : set menu font
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_SetMenuFontColor(
                                     MMI_CTRL_ID_T  ctrl_id,
                                     GUI_COLOR_T    font_color
                                     )
{
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    
    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //set font
        menu_ctrl_ptr->is_set_font_color = TRUE;
        menu_ctrl_ptr->font_color        = font_color;
    }
}

/*****************************************************************************/
//  Description : destruct
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MenuDestruct(
                           IGUICTRL_T   *iguictrl_ptr
                           )
{
    BOOLEAN         result = TRUE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = (GUIMENU_CTRL_T*)iguictrl_ptr;
    GUIMENU_STATIC_ITEM_T *cur_static_item_ptr = PNULL;

    //reset menu info
    ResetMenuInfo(menu_ctrl_ptr);

    while (PNULL != menu_ctrl_ptr->static_item_ptr)
    {
        cur_static_item_ptr = menu_ctrl_ptr->static_item_ptr;
        menu_ctrl_ptr->static_item_ptr = menu_ctrl_ptr->static_item_ptr->next_item_ptr;

        if (PNULL != cur_static_item_ptr->text_str.wstr_ptr)
        {
            SCI_FREE(cur_static_item_ptr->text_str.wstr_ptr);
        }

        SCI_FREE(cur_static_item_ptr);
    }

    if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style])
        &&(PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DestoryMenu))
    {
        ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DestoryMenu(menu_ctrl_ptr);
    }
    
    //free dynamic menu node memory
    if (!menu_ctrl_ptr->is_static)
    {
        FreeStrBuffer(menu_ctrl_ptr->root_node_ptr);

        if (PNULL != menu_ctrl_ptr->node_memory.init_mem_ptr)
        {
            SCI_FREE(menu_ctrl_ptr->node_memory.init_mem_ptr);
        }
    }
    else
    {
        FreeGroupInfoBuffer(menu_ctrl_ptr->root_group_info_ptr);

        if (PNULL != menu_ctrl_ptr->root_group_info_ptr)
        {
            SCI_FREE(menu_ctrl_ptr->root_group_info_ptr);
        }
    }

    //free selected item list memory
    FreeSelListMemory(menu_ctrl_ptr);

    //free prgbox control memory
    if (PNULL != menu_ctrl_ptr->prgbox_ctrl_ptr)
    {
        menu_ctrl_ptr->prgbox_ctrl_ptr = PNULL;
    }

    if (PNULL != menu_ctrl_ptr->temp_invisible_index_ptr)
    {
        SCI_FREE(menu_ctrl_ptr->temp_invisible_index_ptr);
    }

    return (result);
}

/*****************************************************************************/
//  Description : free all string buffer
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* FreeStrBuffer(
                                    GUIMENU_NODE_T   *root_node_ptr  //root node pointer
                                    )
{
    GUIMENU_NODE_T *node_ptr = PNULL;

    if (PNULL != root_node_ptr)
    {
        //find child node
        node_ptr = FreeStrBuffer(root_node_ptr->child_node_ptr);
        if (PNULL == node_ptr)
        {
            //find neighbour node
            node_ptr = FreeStrBuffer(root_node_ptr->neighbour_node_ptr);
            if (PNULL == node_ptr)
            {
                //free item string buffer
                if (PNULL != root_node_ptr->node_wstr_ptr)
                {
                    SCI_FREE(root_node_ptr->node_wstr_ptr);
                }

                // free title string buffer
                if (PNULL != root_node_ptr->title_str.wstr_ptr)
                {
                    SCI_FREE(root_node_ptr->title_str.wstr_ptr);
                    root_node_ptr->title_str.wstr_len = 0;
                }

                // free sub title string buffer
                if (PNULL != root_node_ptr->sub_title_str.wstr_ptr)
                {
                    SCI_FREE(root_node_ptr->sub_title_str.wstr_ptr);
                    root_node_ptr->sub_title_str.wstr_len = 0;
                }
            }
        }
    }

    return (node_ptr);
}

/*****************************************************************************/
//  Description : set menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MenuSetRect(
                          IGUICTRL_T        *ctrl_ptr,
                          const CAF_RECT_T  *rect_ptr
                          )
{
    BOOLEAN         result = TRUE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;

    menu_ctrl_ptr = (GUIMENU_CTRL_T *)ctrl_ptr;

    if (!GUI_EqualRect(menu_ctrl_ptr->rect,*rect_ptr))
    {
        //set rect
        menu_ctrl_ptr->rect         = *rect_ptr;
        menu_ctrl_ptr->display_rect = *rect_ptr;

        GUI_SetVOrHRect( menu_ctrl_ptr->handle, (GUI_RECT_T *)rect_ptr,&(menu_ctrl_ptr->both_rect));

        //stop item text timer
        StopItemTextTimer(menu_ctrl_ptr);
        menu_ctrl_ptr->item_str_index = 0;

        //destroy progress
        if (PNULL != menu_ctrl_ptr->prgbox_ctrl_ptr)
        {
            MMK_DestroyControl(menu_ctrl_ptr->prgbox_ctrl_ptr->handle);
            menu_ctrl_ptr->prgbox_ctrl_ptr = PNULL;
        }

        //get modify rect
        ModifyMenuRect(menu_ctrl_ptr);
    }

    return (result);
}

/*****************************************************************************/
//  Description : modify menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL void ModifyMenuRect(
                          GUIMENU_CTRL_T    *menu_ctrl_ptr
                          )
{
    switch (menu_ctrl_ptr->cur_style)
    {
    case GUIMENU_STYLE_SECOND:
    case GUIMENU_STYLE_THIRD:
        SecondMenuSetRect(menu_ctrl_ptr);
        break;
        
    case GUIMENU_STYLE_POPUP:
    case GUIMENU_STYLE_POPUP_RADIO:
    case GUIMENU_STYLE_POPUP_CHECK:
    case GUIMENU_STYLE_POPUP_AUTO:
    case GUIMENU_STYLE_POPUP_RECT:
        PopMenuSetRect(menu_ctrl_ptr);
        break;

    default:
        if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]) && 
            (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect))
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect(menu_ctrl_ptr);
        }
        break;
    }
}

/*****************************************************************************/
//  Description : update previous second menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL void SecondMenuSetRect(
                             GUIMENU_CTRL_T     *menu_ctrl_ptr
                             )
{
    int16                   offset_y = 0;
    uint16                  i = 0;
    uint16                  menu_level = 0;
    uint16                  first_item_index = 0;
    uint16                  cur_item_index = 0;
    GUI_RECT_T              menu_rect = {0};
    GUIMENU_NODE_T          *cur_parent_node_ptr = PNULL;
    MMI_MENU_GROUP_ID_T     cur_group_id = 0;

    SCI_ASSERT((GUIMENU_STYLE_SECOND == menu_ctrl_ptr->cur_style) || (GUIMENU_STYLE_THIRD == menu_ctrl_ptr->cur_style));    /*assert verified*/

    //save current menu info
    menu_level          = menu_ctrl_ptr->menu_level;
    cur_group_id        = menu_ctrl_ptr->cur_group_id;
    cur_item_index      = menu_ctrl_ptr->cur_item_index;
    first_item_index    = menu_ctrl_ptr->first_item_index;
    menu_rect           = menu_ctrl_ptr->rect;
    cur_parent_node_ptr = menu_ctrl_ptr->cur_parent_node_ptr;
    offset_y            = menu_ctrl_ptr->offset_y;

    for (i=0; i<=menu_level; i++)
    {
        if (i < menu_level)
        {
            //set menu info
            SetMenuInfo(i,
                menu_ctrl_ptr->menu_stack[i].first_item_index,
                menu_ctrl_ptr->menu_stack[i].cur_item_index,
                menu_rect,
                menu_ctrl_ptr->menu_stack[i].offset_y,
                menu_ctrl_ptr->menu_stack[i].group_id,
                menu_ctrl_ptr->menu_stack[i].parent_node_ptr,
                menu_ctrl_ptr);
        }
        else
        {
            //reset current menu info
            SetMenuInfo(menu_level,
                first_item_index,
                cur_item_index,
                menu_rect,
                offset_y,
                cur_group_id,
                cur_parent_node_ptr,
                menu_ctrl_ptr);
        }
                
        if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]) && 
            (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect))
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect(menu_ctrl_ptr);
        }

        if (i < menu_level)
        {
            //modify rect and first item index
            menu_ctrl_ptr->menu_stack[i].rect             = menu_ctrl_ptr->rect;
            menu_ctrl_ptr->menu_stack[i].first_item_index = menu_ctrl_ptr->first_item_index;
            menu_ctrl_ptr->menu_stack[i].offset_y         = menu_ctrl_ptr->offset_y;
        }
    }
}

/*****************************************************************************/
//  Description : update previous pop-up menu
//  Global resource dependence : 
//  Author: Jassmine
//  Note:  当前是pop-up menu,旋转后需要刷新之前所有的pop-up menu
/*****************************************************************************/
LOCAL void PopMenuSetRect(
                          GUIMENU_CTRL_T    *menu_ctrl_ptr
                          )
{
    int16                   offset_y = 0;
    uint16                  i = 0;
    uint16                  menu_level = 0;
    uint16                  first_item_index = 0;
    uint16                  cur_item_index = 0;
    uint16                  cur_item_top = 0;
    GUI_RECT_T              menu_rect = {0};
    GUIMENU_NODE_T          *cur_parent_node_ptr = PNULL;
    MMI_MENU_GROUP_ID_T     cur_group_id = 0;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    if (PNULL == menu_ctrl_ptr)
    {
        return;
    }
    

    if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
    {
        if (0 < menu_ctrl_ptr->menu_level)
        {
            //save current menu info
            menu_level          = menu_ctrl_ptr->menu_level;
            cur_group_id        = menu_ctrl_ptr->cur_group_id;
            cur_item_index      = menu_ctrl_ptr->cur_item_index;
            first_item_index    = menu_ctrl_ptr->first_item_index;
            menu_rect           = menu_ctrl_ptr->rect;
            offset_y            = menu_ctrl_ptr->offset_y;
            cur_parent_node_ptr = menu_ctrl_ptr->cur_parent_node_ptr;

            //need update previous all pop-up menu
            for (i=0; i<=menu_level; i++)
            {
                if (i < menu_level)
                {
                    //set menu info
                    SetMenuInfo(i,
                        menu_ctrl_ptr->menu_stack[i].first_item_index,
                        menu_ctrl_ptr->menu_stack[i].cur_item_index,
                        menu_ctrl_ptr->menu_stack[i].rect,
                        menu_ctrl_ptr->menu_stack[i].offset_y,
                        menu_ctrl_ptr->menu_stack[i].group_id,
                        menu_ctrl_ptr->menu_stack[i].parent_node_ptr,
                        menu_ctrl_ptr);
                }

                if (0 < i)
                {
                    cur_item_top = ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->
                                    GetCurItemTop(menu_ctrl_ptr);
                }

                if (i == menu_level)
                {
                    //reset current menu info
                    SetMenuInfo(menu_level,
                        first_item_index,
                        cur_item_index,
                        menu_rect,
                        offset_y,
                        cur_group_id,
                        cur_parent_node_ptr,
                        menu_ctrl_ptr);
                }

                //calculate pop-up menu rect
                CalculatePopupRect(cur_item_top,menu_ctrl_ptr);

                if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]) && 
                    (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect))
                {
                    ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect(menu_ctrl_ptr);
                }

                if (i < menu_level)
                {
                    //modify rect and first item index
                    menu_ctrl_ptr->menu_stack[i].rect             = menu_ctrl_ptr->rect;
                    menu_ctrl_ptr->menu_stack[i].first_item_index = menu_ctrl_ptr->first_item_index;
                    menu_ctrl_ptr->menu_stack[i].offset_y         = menu_ctrl_ptr->offset_y;
                }
            }
        }
        else
        {
            //calculate pop-up menu rect
            CalculatePopupRect(cur_item_top,menu_ctrl_ptr);

            if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]) && 
                (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect))
            {
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect(menu_ctrl_ptr);
            }
        }
    }
}

/*****************************************************************************/
//  Description : get menu rect
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN MenuGetRect(
                          IGUICTRL_T    *iguictrl_ptr, 
                          CAF_RECT_T    *rect_ptr
                          )
{
    BOOLEAN         result = TRUE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = (GUIMENU_CTRL_T*)iguictrl_ptr;
    
    if (GUIMENU_IsPopMenu(menu_ctrl_ptr))
    {
#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
        if (GUIMENU_STYLE_POPUP == menu_ctrl_ptr->cur_style
            || GUIMENU_STYLE_OPTIONS == menu_ctrl_ptr->cur_style)
        {
            rect_ptr->left   = 0;
            rect_ptr->top    = 0;
            rect_ptr->right  = (int16)(MMITHEME_GetMainScreenRightPixel() - 1);
            rect_ptr->bottom = (int16)(MMITHEME_GetMainScreenBottomPixel() - 1);
        }
        else
        {
            *rect_ptr = menu_ctrl_ptr->rect;
        }
#else
        //pop-up menu, rect is full screen - softkey
        //因为它需要处理非menu rect区域,关闭pop-up menu
        rect_ptr->left   = 0;
        rect_ptr->top    = 0;
        rect_ptr->right  = (int16)(MMITHEME_GetMainScreenRightPixel() - 1);

        if (GUIMENU_STYLE_POPUP_AUTO == menu_ctrl_ptr->cur_style)
        {
            rect_ptr->bottom = (int16)(MMITHEME_GetMainScreenBottomPixel() - 1);
        }
        else
        {
            rect_ptr->bottom = (int16)(MMITHEME_GetMainScreenBottomPixel() - MMITHEME_GetSoftkeyHeight()- 1);
        }
#endif
    }
    else
    {
        *rect_ptr = menu_ctrl_ptr->rect;
    }

    return (result);
}

/*****************************************************************************/
//  Description : 设置菜单是否处于移动状态
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetMenuMoveState(
                                        MMI_CTRL_ID_T  ctrl_id,
                                        BOOLEAN		is_move
                                        )
{
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T      *menu_ctrl_ptr = PNULL;
    
    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if(PNULL != menu_ctrl_ptr)
    {
        if (is_move) // 
        {
            if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style])
                &&(PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->MoveInit))
            {
                result = TRUE;
                menu_ctrl_ptr->is_move_state = is_move;
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->MoveInit(menu_ctrl_ptr);
            }
        }
        else
        {
            if ((PNULL != menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style])
                &&(PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->MoveDestory))
            {
                result = TRUE;
                menu_ctrl_ptr->is_move_state = is_move;
                ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->MoveDestory(menu_ctrl_ptr);
            }
        }
    }

    return result;
}

/*lint +e737*/

#ifdef DYNAMIC_MODULE_SUPPORT    
//added by andrew.zhang at 2009/07/06
/*****************************************************************************/
//  Description:   insert node
//  Author: Andrew.Zhang
//  Param
//      me_ptr :control object
//		nodex_index :node index,from 0
//		node_id :node id,from 1
//		parent_node_id :parent node id
//		node_item_ptr :node item
//  Return:
//       TRUE:success;FALSE:fail
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CAFMENU_InsertNode( 
                                 IGUIMENU_T *me_ptr, 
                                 uint16 nodex_index, 
                                 uint32 node_id, 
                                 uint32 parent_node_id, 
                                 CAF_GUIMENU_DYNA_ITEM_T *node_item_ptr 
                                 )
{
	GUIMENU_DYNA_ITEM_T guimenu_dyna_item = {0};		
    GUIMENU_CTRL_T	*menu_ctrl_ptr = (GUIMENU_CTRL_T*)me_ptr;

	if (PNULL == node_item_ptr)
	{
        return FALSE;
	}
	
	guimenu_dyna_item.is_grayed = FALSE;
	guimenu_dyna_item.select_icon_id = node_item_ptr->select_icon_id;
	guimenu_dyna_item.item_text_ptr = node_item_ptr->item_text_ptr;

	GUIMENU_InsertNode(
                        						nodex_index,
                        						node_id,
                        						parent_node_id,
                        						(GUIMENU_DYNA_ITEM_T *)&guimenu_dyna_item, 
                        						menu_ctrl_ptr->handle);

	return TRUE;
}


/*****************************************************************************/
//  Description:   handle control event
//  Author: Andrew.Zhang
//  Param
//       me_ptr :control object
//  Return:
//       current node id
//  Note: 
/*****************************************************************************/
LOCAL uint32 CAFMENU_GetCurNodeId( 
                                  IGUIMENU_T *me_ptr 
                                  )
{
    GUIMENU_CTRL_T	*menu_ctrl_ptr = (GUIMENU_CTRL_T*)me_ptr;

    return GUIMENU_GetCurNodeId(menu_ctrl_ptr->handle);
}

/*****************************************************************************/
//  Description:   set menu background color
//  Author: Andrew.Zhang
//  Param
//       me_ptr :control object
//       bg_color :background color
//  Return:
//       TRUE:success;FALSE:fail
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN CAFMENU_SetBgColor( 
                                 IGUIMENU_T *me_ptr, 
                                 CAF_COLOR_T bg_color 
                                 )
{
    GUIMENU_CTRL_T	*menu_ctrl_ptr = (GUIMENU_CTRL_T*)me_ptr;

	GUIMENU_SetMenuBgColor( menu_ctrl_ptr->handle, bg_color );

    return TRUE;
}
#endif

/*****************************************************************************/
//  Description : 通过index获取node id
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetNodeIdByIndex(
                                        MMI_CTRL_ID_T   ctrl_id,                // [in]
                                        uint32          parent_node_id,         // [in]
                                        uint16          index,                  // [in]
                                        uint32          *node_id_ptr            // [out]
                                        )
{
    BOOLEAN         result = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T  *root_node_ptr = PNULL;
    GUIMENU_NODE_T  *parent_node_ptr = PNULL;
    GUIMENU_NODE_T  *cur_node_ptr = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr && !menu_ctrl_ptr->is_static && PNULL != node_id_ptr)
    {
        //get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        parent_node_ptr = FindNodeById(parent_node_id, root_node_ptr);

        if (PNULL != parent_node_ptr)
        {
            cur_node_ptr = GUIMENU_FindNodeByIndex(index, parent_node_ptr);
            if (PNULL != cur_node_ptr)
            {
                *node_id_ptr = cur_node_ptr->node_id;
                result = TRUE;
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : 通过node id获取index
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetIndexByNodeId(
                                        MMI_CTRL_ID_T   ctrl_id,                // [in]
                                        uint32          parent_node_id,         // [in]
                                        uint32          node_id,                // [in]
                                        uint16          *index_ptr              // [out]
                                        )
{
    BOOLEAN         result = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;
    GUIMENU_NODE_T  *root_node_ptr = PNULL;
    GUIMENU_NODE_T  *parent_node_ptr = PNULL;
    GUIMENU_NODE_T  *cur_node_ptr = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr && !menu_ctrl_ptr->is_static && PNULL != index_ptr)
    {
        //get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        parent_node_ptr = FindNodeById(parent_node_id, root_node_ptr);

        if (PNULL != parent_node_ptr)
        {
            *index_ptr = 0;

            cur_node_ptr = parent_node_ptr->child_node_ptr;
            while (PNULL != cur_node_ptr)
            {
                if (cur_node_ptr->node_id == node_id)
                {
                    result = TRUE;
                    break;
                }

                (*index_ptr)++;

                cur_node_ptr = cur_node_ptr->neighbour_node_ptr;
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : set item grayed or not, this method is for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetItemGrayed(
                                     MMI_CTRL_ID_T        ctrl_id,    // [in]
                                     MMI_MENU_GROUP_ID_T  group_id,   // [in]
                                     MMI_MENU_ID_T        menu_id,    // [in]
                                     BOOLEAN              is_grayed   // [in]
                                     )
{
    int32 i = 0;
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T          *menu_ctrl_ptr  = PNULL;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;
    GUIMENU_GROUP_T         *group_ptr      = PNULL;
    GUIMENU_ITEM_INFO_T     *item_info_ptr  = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        group_info_ptr = GetGroupInfo(menu_ctrl_ptr, group_id);

        if (PNULL != group_info_ptr)
        {
            group_ptr = (GUIMENU_GROUP_T*)MMITHEME_GetMenuGroup(group_id);
            SCI_ASSERT(PNULL != group_ptr); /*assert verified*/

            item_info_ptr = GET_ITEM_INFO(group_info_ptr);

            for (i = 0; i < group_ptr->item_count; i++)
            {
                if (group_ptr->item_ptr[i].menu_id == menu_id)
                {
                    item_info_ptr[i].is_grayed = is_grayed;

                    result = TRUE;

                    break;
                }
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : set item visible, this method is for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetItemVisible(
                                      MMI_CTRL_ID_T        ctrl_id,    // [in]
                                      MMI_MENU_GROUP_ID_T  group_id,   // [in]
                                      MMI_MENU_ID_T        menu_id,    // [in]
                                      BOOLEAN              is_visible  // [in]
                                      )
{
    int32 i = 0;
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T          *menu_ctrl_ptr  = PNULL;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;
    GUIMENU_GROUP_T         *group_ptr      = PNULL;
    GUIMENU_ITEM_INFO_T     *item_info_ptr  = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        group_info_ptr = GetGroupInfo(menu_ctrl_ptr, group_id);

        if (PNULL != group_info_ptr)
        {
            group_ptr = (GUIMENU_GROUP_T*)MMITHEME_GetMenuGroup(group_id);
            SCI_ASSERT(PNULL != group_ptr); /*assert verified*/

            item_info_ptr = GET_ITEM_INFO(group_info_ptr);

            for (i = 0; i < group_ptr->item_count; i++)
            {
                if (group_ptr->item_ptr[i].menu_id == menu_id)
                {
                    if (item_info_ptr[i].is_visible && !is_visible)        // while being invisible
                    {
                        item_info_ptr[i].is_visible = is_visible;
                        group_info_ptr->visible_child_item_num--;

                        // when it is the current display menu, set item_total_num too
                        if (group_id == menu_ctrl_ptr->cur_group_id)
                        {
                            menu_ctrl_ptr->item_total_num--;
                        }
                    }
                    else if (!item_info_ptr[i].is_visible && is_visible)   // while being visible
                    {
                        item_info_ptr[i].is_visible = is_visible;
                        group_info_ptr->visible_child_item_num++;

                        if (group_id == menu_ctrl_ptr->cur_group_id)
                        {
                            menu_ctrl_ptr->item_total_num++;
                        }
                    }

                    result = TRUE;

                    break;
                }
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : set item text and image
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetStaticItem(
                                     MMI_CTRL_ID_T        ctrl_id,    // [in]
                                     MMI_MENU_GROUP_ID_T  group_id,   // [in]
                                     MMI_MENU_ID_T        menu_id,    // [in]
                                     MMI_STRING_T         *text_ptr,  // [in] PNULL表示不设置文本
                                     MMI_IMAGE_ID_T       icon_id     // [in] 0或者IMAGE_NULL表示不设置图片
                                     )
{
    BOOLEAN result = FALSE;
    BOOLEAN is_item_found = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr  = PNULL;
    GUIMENU_STATIC_ITEM_T *cur_static_item_ptr = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        cur_static_item_ptr = MMIMENU_GetStaticItem(menu_ctrl_ptr, group_id, menu_id);

        if (PNULL == cur_static_item_ptr)
        {
            cur_static_item_ptr = SCI_ALLOC_APP(sizeof(GUIMENU_STATIC_ITEM_T));
            SCI_MEMSET(cur_static_item_ptr, 0, sizeof(GUIMENU_STATIC_ITEM_T));

            cur_static_item_ptr->group_id = group_id;
            cur_static_item_ptr->menu_id  = menu_id;
        }
        else
        {
            is_item_found = TRUE;
        }

        if (PNULL != text_ptr && text_ptr->wstr_len > 0)
        {
            if (PNULL != cur_static_item_ptr->text_str.wstr_ptr)
            {
                SCI_FREE(cur_static_item_ptr->text_str.wstr_ptr);
            }

            cur_static_item_ptr->text_str.wstr_len = text_ptr->wstr_len;
            cur_static_item_ptr->text_str.wstr_ptr = SCI_ALLOC_APP((text_ptr->wstr_len + 1) * sizeof(wchar));
            SCI_MEMSET(cur_static_item_ptr->text_str.wstr_ptr, 0, (text_ptr->wstr_len + 1) * sizeof(wchar));

            MMI_WSTRNCPY(
                    cur_static_item_ptr->text_str.wstr_ptr, cur_static_item_ptr->text_str.wstr_len,
                    text_ptr->wstr_ptr, text_ptr->wstr_len,
                    text_ptr->wstr_len);
        }

        if (0 != icon_id && MMITHEME_GetInvalidImageID() != icon_id)
        {
            cur_static_item_ptr->icon_id = icon_id;
        }

        if (!is_item_found)
        {
            if (PNULL != menu_ctrl_ptr->static_item_ptr)
            {
                cur_static_item_ptr->next_item_ptr = menu_ctrl_ptr->static_item_ptr;
            }

            menu_ctrl_ptr->static_item_ptr = cur_static_item_ptr;
        }

        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
//  Description : set item visible, this method is for dynamic menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetNodeVisible(
                                      MMI_CTRL_ID_T ctrl_id,        // [in]
                                      uint16        nodex_index,    // [in] node index,from 0
                                      uint32        parent_node_id, // [in]
                                      BOOLEAN       is_visible      // [in]
                                      )
{
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr  = PNULL;
    GUIMENU_NODE_T  *root_node_ptr  = PNULL;
    GUIMENU_NODE_T  *parent_node_ptr = PNULL;
    GUIMENU_NODE_T  *cur_node_ptr   = PNULL;

    // get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        // get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        parent_node_ptr = FindNodeById(parent_node_id, root_node_ptr);

        if (PNULL != parent_node_ptr)
        {
            // find specified node
            cur_node_ptr = GUIMENU_FindNodeByIndex(nodex_index, parent_node_ptr);

            if (PNULL != cur_node_ptr)
            {
                if (cur_node_ptr->is_visible && !is_visible)        // while being invisible
                {
                    cur_node_ptr->is_visible = is_visible;
                    parent_node_ptr->visible_child_node_num--;

                    // when it is the current display menu, set item_total_num too
                    if (parent_node_ptr == menu_ctrl_ptr->cur_parent_node_ptr)
                    {
                        menu_ctrl_ptr->item_total_num--;
                    }
                }
                else if (!cur_node_ptr->is_visible && is_visible)   // while being visible
                {
                    cur_node_ptr->is_visible = is_visible;
                    parent_node_ptr->visible_child_node_num++;

                    if (parent_node_ptr == menu_ctrl_ptr->cur_parent_node_ptr)
                    {
                        menu_ctrl_ptr->item_total_num++;
                    }
                }

                result = TRUE;
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : get title text
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetTitleText(
                                    GUIMENU_CTRL_T  *menu_ctrl_ptr, //[in]
                                    MMI_STRING_T    *title_str_ptr, //[out]
                                    MMI_STRING_T    *sub_title_str_ptr, // [out]
                                    MMI_IMAGE_ID_T  *title_icon_id_ptr  // [out]
                                    )
{
    if (PNULL == menu_ctrl_ptr)
    {
        return FALSE;
    }

    if (menu_ctrl_ptr->is_static)
    {
        if (PNULL != title_str_ptr)
        {
            if (PNULL != menu_ctrl_ptr->cur_group_info_ptr->title_str.wstr_ptr
                && 0 != menu_ctrl_ptr->cur_group_info_ptr->title_str.wstr_len)
            {
                *title_str_ptr = menu_ctrl_ptr->cur_group_info_ptr->title_str;
            }
            else if (0 < menu_ctrl_ptr->title_len)
            {
                title_str_ptr->wstr_len = menu_ctrl_ptr->title_len;
                title_str_ptr->wstr_ptr = menu_ctrl_ptr->title_wstr;
            }
            else
            {
                MMITHEME_GetLabelTextByLang(
                        menu_ctrl_ptr->cur_group_ptr->title_string_id,
                        title_str_ptr);
            }
        }

        if (PNULL != sub_title_str_ptr)
        {
            if (PNULL != menu_ctrl_ptr->cur_group_info_ptr->sub_title_str.wstr_ptr
                && 0 != menu_ctrl_ptr->cur_group_info_ptr->sub_title_str.wstr_len)
            {
                *sub_title_str_ptr = menu_ctrl_ptr->cur_group_info_ptr->sub_title_str;
            }
        }

        if (PNULL != title_icon_id_ptr)
        {
            *title_icon_id_ptr = menu_ctrl_ptr->cur_group_info_ptr->title_icon_id;
        }
    }
    else
    {
        if (PNULL != title_str_ptr)
        {
            if (PNULL != menu_ctrl_ptr->cur_parent_node_ptr->title_str.wstr_ptr
                && 0 != menu_ctrl_ptr->cur_parent_node_ptr->title_str.wstr_len)
            {
                *title_str_ptr = menu_ctrl_ptr->cur_parent_node_ptr->title_str;
            }
            //if it is the first level menu
            else if (PNULL == menu_ctrl_ptr->cur_parent_node_ptr->parent_node_ptr)
            {
                title_str_ptr->wstr_len = menu_ctrl_ptr->title_len;
                title_str_ptr->wstr_ptr = menu_ctrl_ptr->title_wstr;
            }
            else
            {
                title_str_ptr->wstr_len = menu_ctrl_ptr->cur_parent_node_ptr->node_wstr_len;
                title_str_ptr->wstr_ptr = menu_ctrl_ptr->cur_parent_node_ptr->node_wstr_ptr;
            }
        }

        if (PNULL != sub_title_str_ptr)
        {
            if (PNULL != menu_ctrl_ptr->cur_parent_node_ptr->sub_title_str.wstr_ptr
                && 0 != menu_ctrl_ptr->cur_parent_node_ptr->sub_title_str.wstr_len)
            {
                *sub_title_str_ptr = menu_ctrl_ptr->cur_parent_node_ptr->sub_title_str;
            }
        }

        if (PNULL != title_icon_id_ptr)
        {
            *title_icon_id_ptr = menu_ctrl_ptr->cur_parent_node_ptr->title_icon_id;
        }
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : get pop sel info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetPopSelInfo(
                                     GUIMENU_CTRL_T           *menu_ctrl_ptr, //[in]
                                     uint16                   item_index,     //[in]
                                     GUIMENU_POP_SEL_INFO_T   *item_info_ptr  //[out]
                                     )
{
    BOOLEAN         result          = FALSE;
    GUIMENU_NODE_T  *item_node_ptr  = PNULL;
    GUIMENU_ITEM_T  *item_ptr       = PNULL;

    if (PNULL == menu_ctrl_ptr || PNULL == item_info_ptr)
    {
        return result;
    }

    item_info_ptr->ctrl_id   = menu_ctrl_ptr->handle;
    item_info_ptr->is_static = menu_ctrl_ptr->is_static;
    if (menu_ctrl_ptr->is_static)
    {
        item_ptr = GetItemByVisibleIndex(menu_ctrl_ptr, item_index, PNULL);
        if (PNULL != item_ptr)
        {
            item_info_ptr->menu_id = item_ptr->menu_id;
            item_info_ptr->group_id = menu_ctrl_ptr->cur_group_id;

            result = TRUE;
        }
    }
    else
    {
        item_node_ptr = GetNodeByVisibleIndex(item_index,menu_ctrl_ptr->cur_parent_node_ptr);
        if (PNULL != item_node_ptr)
        {
            item_info_ptr->node_id = item_node_ptr->node_id;

            result = TRUE;
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : get item text string, icon id, and is_grayed state
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view
//        if it is option page style, get the item in page.
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetItem(
                               GUIMENU_CTRL_T    *menu_ctrl_ptr, // [in]
                               uint16            item_index,     // [in]
                               MMI_STRING_T      *text_ptr,      // [out]
                               MMI_IMAGE_ID_T    *icon_id_ptr,   // [out]
                               BOOLEAN           *is_grayed_ptr  // [out]
                               )
{
    BOOLEAN         result          = FALSE;
    GUIMENU_NODE_T  *item_node_ptr  = PNULL;
    GUIMENU_ITEM_T  *item_ptr       = PNULL;
    GUIMENU_ITEM_INFO_T item_info   = {0};
    GUIMENU_STATIC_ITEM_T *static_item_ptr = PNULL;

    if (PNULL == menu_ctrl_ptr ||
        (PNULL == text_ptr && PNULL == icon_id_ptr && PNULL == is_grayed_ptr))
    {
        return result;
    }

    if (GUIMENU_STYLE_OPTION_PAGE == menu_ctrl_ptr->cur_style)
    {
        result = FALSE;
    }
    else if (GUIMENU_ISTYLE == menu_ctrl_ptr->cur_style ||
        GUIMENU_STYLE_CUBE == menu_ctrl_ptr->cur_style ||
        GUIMENU_STYLE_ICON == menu_ctrl_ptr->cur_style || 
        GUIMENU_STYLE_TITLE_ICON == menu_ctrl_ptr->cur_style 
#ifdef MENU_CRYSTALCUBE_SUPPORT
        ||GUIMENU_STYLE_CRYSTAL_CUBE == menu_ctrl_ptr->cur_style
#endif
#ifdef MENU_CYCLONE_SUPPORT
        ||GUIMENU_STYLE_CYCLONE == menu_ctrl_ptr->cur_style 
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
		||GUIMENU_STYLE_GO == menu_ctrl_ptr->cur_style
#else
        || GUIMENU_STYLE_SLIDE_PAGE == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_SPHERE == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_CYLINDER == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_TRAPEZOID == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_BINARY_STAR== menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_SHUTTER == menu_ctrl_ptr->cur_style
        || GUIMENU_STYLE_CHARIOT == menu_ctrl_ptr->cur_style
#endif
    )
    {
        MMI_TEXT_ID_T text_id = 0;
        
        GUIMENU_MAINMENU_INFO_T mainmenu_info = {0};
        
        GetCurItemPtr(menu_ctrl_ptr, &item_info, &mainmenu_info); 
        
		if (GUIMENU_MAINMENU_DATA_TYPE_STATIC == mainmenu_info.menu_data_type &&
			PNULL != mainmenu_info.menu_data.static_menu_ptr)
		{
			if (PNULL != text_ptr)
			{
				text_id = mainmenu_info.menu_data.static_menu_ptr->text_str_id;
				MMITHEME_GetLabelTextByLang(text_id, text_ptr);
			}
			result = TRUE;
		}
#ifdef MMI_PDA_SUPPORT
		else if(GUIMENU_MAINMENU_DATA_TYPE_FILEFOLD== mainmenu_info.menu_data_type
			&& PNULL != mainmenu_info.menu_data.filefold_menu_ptr)
		{
			if (PNULL != text_ptr)
			{
				text_ptr->wstr_ptr = mainmenu_info.menu_data.filefold_menu_ptr->text;
				text_ptr->wstr_len = mainmenu_info.menu_data.filefold_menu_ptr->text_len;
			}
			
			result = TRUE;
		}
#endif
#ifdef DYNAMIC_MAINMENU_SUPPORT
		else if(GUIMENU_MAINMENU_DATA_TYPE_DYNAMIC == mainmenu_info.menu_data_type
			&& PNULL != mainmenu_info.menu_data.dynamic_menu.ori_data_ptr)
		{
			if (PNULL != text_ptr)
			{
				text_ptr->wstr_ptr = mainmenu_info.menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.text;
				text_ptr->wstr_len = mainmenu_info.menu_data.dynamic_menu.ori_data_ptr->dynamic_menu_info.text_len;
			}

			result = TRUE;
		}
#endif
    }
    else
    {
		if (menu_ctrl_ptr->is_static)
		{
			item_ptr = GetItemByVisibleIndex(menu_ctrl_ptr, item_index, &item_info);
			
			static_item_ptr = MMIMENU_GetStaticItem(menu_ctrl_ptr, menu_ctrl_ptr->cur_group_id, item_ptr->menu_id);
			
			if (PNULL != text_ptr)
			{
				if (PNULL != static_item_ptr && PNULL != static_item_ptr->text_str.wstr_ptr)
				{
					*text_ptr = static_item_ptr->text_str;
				}
				else
				{
					MMITHEME_GetLabelTextByLang(item_ptr->text_str_id, text_ptr);
				}
			}
			
			if (PNULL != icon_id_ptr)
			{
				if (PNULL != static_item_ptr && 0 != static_item_ptr->icon_id)
				{
					*icon_id_ptr = static_item_ptr->icon_id;
				}
				else
				{
					*icon_id_ptr = item_ptr->select_icon_id;
				}
			}
			
			if (PNULL != is_grayed_ptr)
			{
				// check global setting first
				*is_grayed_ptr = MMITHEME_IsMenuItemGrayed(
                    menu_ctrl_ptr->win_handle,
                    menu_ctrl_ptr->cur_group_id,
                    item_ptr->menu_id);
				
				// then check whether user set this menu object grayed
				if (!(*is_grayed_ptr))
				{
					*is_grayed_ptr = item_info.is_grayed;
				}
			}
			
			result = TRUE;
		}
		else
		{
			//get current item node pointer
			item_node_ptr = GetNodeByVisibleIndex(
				item_index,
				menu_ctrl_ptr->cur_parent_node_ptr);
			if (PNULL != item_node_ptr)
			{
				if (PNULL != text_ptr)
				{
					text_ptr->wstr_len = item_node_ptr->node_wstr_len;
					text_ptr->wstr_ptr = item_node_ptr->node_wstr_ptr;
				}
				
				if (PNULL != is_grayed_ptr)
				{
					*is_grayed_ptr = item_node_ptr->is_grayed;
				}
				
				if (PNULL != icon_id_ptr)
				{
					*icon_id_ptr = item_node_ptr->select_icon_id;
				}
				
				result = TRUE;
			}
		}
    }
	
    return result;
}

/*****************************************************************************/
//  Description : get softkey text id
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view
/*****************************************************************************/
PUBLIC MMI_TEXT_ID_T* GUIMENU_GetSoftkeyText(
                                             GUIMENU_CTRL_T    *menu_ctrl_ptr  // [in]
                                             )
{
    MMI_TEXT_ID_T   *softkey_id_ptr = PNULL;
    GUIMENU_ITEM_T  *item_ptr       = PNULL;

    if (PNULL == menu_ctrl_ptr)
    {
        return PNULL;
    }

    if (menu_ctrl_ptr->is_static)
    {
        item_ptr = GetItemByVisibleIndex(menu_ctrl_ptr, menu_ctrl_ptr->cur_item_index, PNULL);
        softkey_id_ptr = (MMI_IMAGE_ID_T *)item_ptr->button_id;
    }
    else
    {
        softkey_id_ptr = menu_ctrl_ptr->dyna_softkey_id;
    }

    return softkey_id_ptr;
}

/*****************************************************************************/
//  Description : get visible item count in specified option page
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view
/*****************************************************************************/
PUBLIC uint16 GUIMENU_GetItemCountInOptionPage(
                                               GUIMENU_CTRL_T *menu_ctrl_ptr,   // [in]
                                               uint16 option_page_index         // [in] visible option page index
                                               )
{
    MMI_MENU_GROUP_ID_T     cur_op_menu_group_id = 0;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;

    // when style is wrong, return count 0.
    // now, option page style is just for static.
    if (GUIMENU_STYLE_OPTION_PAGE != menu_ctrl_ptr->cur_style
        || !menu_ctrl_ptr->is_static
        || option_page_index > menu_ctrl_ptr->cur_group_ptr->item_count)
    {
        return 0;
    }

    cur_op_menu_group_id = GetGroupIdInOptionPage(menu_ctrl_ptr, option_page_index);

    group_info_ptr = GetGroupInfo(menu_ctrl_ptr, cur_op_menu_group_id);

    return group_info_ptr->visible_child_item_num;
}

/*****************************************************************************/
//  Description : translate the visible index to absolute index in item array or list
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view. the index is in the current display group
//        if it is option page style, it translates the page index. if you want
//        to translate the item index, please use GUIMENU_TranslateVisibleIndex
/*****************************************************************************/
PUBLIC uint16 GUIMENU_VisibleIndex2AbsIndex(
                                            GUIMENU_CTRL_T *menu_ctrl_ptr, // [in]
                                            uint16 visible_index           // [in]
                                            )
{
    int32   i = 0;
    uint16  cur_visible_index = 0;
    uint16  absolute_index = 0;
    GUIMENU_NODE_T      *find_node_ptr = PNULL;
    GUIMENU_ITEM_INFO_T *item_info_ptr = PNULL;

    if (menu_ctrl_ptr->is_static)
    {
        // if visible item count is equal to total item count, visible index is
        // absolute index.
        if (menu_ctrl_ptr->cur_group_info_ptr->visible_child_item_num
            == menu_ctrl_ptr->cur_group_ptr->item_count)
        {
            absolute_index = visible_index;
        }
        else
        {
            item_info_ptr = GET_ITEM_INFO(menu_ctrl_ptr->cur_group_info_ptr);

            for (i = 0; i < menu_ctrl_ptr->cur_group_ptr->item_count; i++)
            {
                if (item_info_ptr[i].is_visible)
                {
                    if (visible_index == cur_visible_index)
                    {
                        break;
                    }

                    cur_visible_index++;
                }

                absolute_index++;
            }

            // 必须要保证绝对索引值小于节点的总数，否则就是传入的visible_index有误
            SCI_ASSERT(absolute_index < menu_ctrl_ptr->cur_group_ptr->item_count);  /*assert verified*/
        }
    }
    else
    {
        // if visible item count is equal to total item count, visible index is
        // absolute index.
        if (menu_ctrl_ptr->cur_parent_node_ptr->visible_child_node_num
            == menu_ctrl_ptr->cur_parent_node_ptr->child_node_num)
        {
            absolute_index = visible_index;
        }
        else
        {
            find_node_ptr = menu_ctrl_ptr->cur_parent_node_ptr->child_node_ptr;
            while (PNULL != find_node_ptr)
            {
                if (find_node_ptr->is_visible)
                {
                    if (visible_index == cur_visible_index)
                    {
                        break;
                    }

                    cur_visible_index++;
                }

                absolute_index++;

                find_node_ptr = find_node_ptr->neighbour_node_ptr;
            }

            // 必须要保证绝对索引值小于节点的总数，否则就是传入的visible_index有误
            SCI_ASSERT(absolute_index < menu_ctrl_ptr->cur_parent_node_ptr->child_node_num);    /*assert verified*/
        }
    }

    return absolute_index;
}

/*****************************************************************************/
//  Description : translate the visible item index in option page to absolute index
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is just for option page style
/*****************************************************************************/
PUBLIC uint16 GUIMENU_TranslateVisibleIndex(
                                            GUIMENU_CTRL_T *menu_ctrl_ptr, // [in]
                                            uint16 visible_index           // [in]
                                            )
{
    GUIMENU_GROUP_T       *store_cur_group_ptr = menu_ctrl_ptr->cur_group_ptr;
    GUIMENU_GROUP_INFO_T  *store_cur_group_info_ptr = menu_ctrl_ptr->cur_group_info_ptr;
    MMI_MENU_GROUP_ID_T   cur_op_group_id = 0;
    uint16                absolute_index = 0;

    // when the style is wrong, do not translate and just return visible index.
    // now, option page style is just for static.
    if (GUIMENU_STYLE_OPTION_PAGE != menu_ctrl_ptr->cur_style
        || !menu_ctrl_ptr->is_static)
    {
        return visible_index;
    }

    // this call is to get group id in current option page.
    cur_op_group_id = GetGroupIdInOptionPage(menu_ctrl_ptr, menu_ctrl_ptr->cur_page_index);

    // I want to use GUIMENU_VisibleIndex2AbsIndex to translate index, so just
    // set some varibles and restore them after finish.
    menu_ctrl_ptr->cur_group_ptr = MMITHEME_GetMenuGroup(cur_op_group_id);
    menu_ctrl_ptr->cur_group_info_ptr = GetGroupInfo(menu_ctrl_ptr, cur_op_group_id);

    absolute_index = GUIMENU_VisibleIndex2AbsIndex(menu_ctrl_ptr, visible_index);

    menu_ctrl_ptr->cur_group_ptr = store_cur_group_ptr;
    menu_ctrl_ptr->cur_group_info_ptr = store_cur_group_info_ptr;

    return absolute_index;
}

/*****************************************************************************/
//  Description : current menu is have child menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for menu view.
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsExistChildMenu(                                // [ret] TRUE when has child menu
                                        GUIMENU_CTRL_T  *menu_ctrl_ptr, // [in]
                                        uint16          visible_index,  // [in]
                                        BOOLEAN         *is_grayed_ptr  // [out] may PNULL
                                        )
{
    BOOLEAN         is_exist_child_menu = FALSE;
    BOOLEAN         is_grayed = FALSE;
    GUIMENU_ITEM_T  *item_ptr = PNULL;
    GUIMENU_NODE_T  *item_node_ptr  = PNULL;
    GUIMENU_ITEM_INFO_T item_info   = {0};

    if (menu_ctrl_ptr->is_static)
    {
        item_ptr = GetItemByVisibleIndex(menu_ctrl_ptr, visible_index, &item_info);

        // check global setting first
        is_grayed = MMITHEME_IsMenuItemGrayed(
                                        menu_ctrl_ptr->win_handle,
                                        menu_ctrl_ptr->cur_group_id,
                                        item_ptr->menu_id);

        // then check whether user set this menu object grayed
        if (!is_grayed)
        {
            is_grayed = item_info.is_grayed;
        }

        //get the next menu group
        if (PNULL != MMITHEME_GetMenuGroup(item_ptr->link_group_id))
        {
            is_exist_child_menu  = TRUE;
        }
    }
    else
    {
        item_node_ptr = GetNodeByVisibleIndex(
                                        visible_index,
                                        menu_ctrl_ptr->cur_parent_node_ptr);
        if (PNULL != item_node_ptr)
        {
            is_grayed = item_node_ptr->is_grayed;

            if (PNULL != item_node_ptr->child_node_ptr)
            {
                is_exist_child_menu  = TRUE;
            }
        }
    }

    if (PNULL != is_grayed_ptr)
    {
        *is_grayed_ptr = is_grayed;
    }

    return (is_exist_child_menu);
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsMoveState(
                                   MMI_CTRL_ID_T        ctrl_id
                                   )
{
    BOOLEAN         result = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        result = menu_ctrl_ptr->is_move_state;
    }

    return (result);
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_CancelMoveIcon(
                                      MMI_CTRL_ID_T        ctrl_id
                                      )
{
    BOOLEAN         result = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        if (((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuEndKey != PNULL)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuEndKey(menu_ctrl_ptr);
        }

        result = menu_ctrl_ptr->is_move_state;
    }

    return (result);
}

/*****************************************************************************/
// 	Description : 设置半透背景
//	Global resource dependence : 
//  Author: xiaoqing.lu
//	Note:
/*****************************************************************************/
PUBLIC void GUIMENU_SetHalfTransparent(
                                       BOOLEAN               is_transparent, //in:
                                       MMI_CTRL_ID_T         ctrl_id        //in:
                                       )
{
    GUIMENU_CTRL_T          *menu_ctrl_ptr  = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);

    if (PNULL != menu_ctrl_ptr)
    {
        menu_ctrl_ptr->is_transparent = is_transparent;
    }
}

/*****************************************************************************/
//  Description : set style of sub menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetSubMenuStyle(
                                       MMI_CTRL_ID_T      ctrl_id,      // [in]
                                       uint32             node_id,      // [in]
                                       GUIMENU_STYLE_E    style         // [in]
                                       )
{
    BOOLEAN         result          = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr  = PNULL;
    GUIMENU_NODE_T  *root_node_ptr  = PNULL;
    GUIMENU_NODE_T  *node_ptr       = PNULL;

    // this method is available only in popup menu
    if (GUIMENU_STYLE_POPUP == style
        || GUIMENU_STYLE_POPUP_RADIO == style
        || GUIMENU_STYLE_POPUP_CHECK == style)
    {
        //get menu pointer by control id
        menu_ctrl_ptr = GetMenuPtr(ctrl_id);
        if (PNULL != menu_ctrl_ptr)
        {
            //get parent node pointer
            root_node_ptr = menu_ctrl_ptr->root_node_ptr;
            node_ptr = FindNodeById(node_id, root_node_ptr);

            if (PNULL != node_ptr
                && (GUIMENU_STYLE_POPUP == node_ptr->sub_menu_style
#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
                    || GUIMENU_STYLE_POPUP_AUTO == node_ptr->sub_menu_style
#endif
                    || GUIMENU_STYLE_POPUP_RADIO == node_ptr->sub_menu_style
                    || GUIMENU_STYLE_POPUP_CHECK == node_ptr->sub_menu_style))
            {
                node_ptr->sub_menu_style = style;
                result = TRUE;
            }
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : get sub menu style
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC GUIMENU_STYLE_E GUIMENU_GetSubMenuStyle(
                                               MMI_CTRL_ID_T      ctrl_id,  // [in]
                                               uint32             node_id   // [in]
                                               )
{
    GUIMENU_STYLE_E style           = GUIMENU_STYLE_NULL;
    GUIMENU_CTRL_T  *menu_ctrl_ptr  = PNULL;
    GUIMENU_NODE_T  *root_node_ptr  = PNULL;
    GUIMENU_NODE_T  *node_ptr       = PNULL;

    //get menu pointer by control id
    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        //get parent node pointer
        root_node_ptr = menu_ctrl_ptr->root_node_ptr;
        node_ptr = FindNodeById(node_id, root_node_ptr);

        if (PNULL != node_ptr)
        {
            style = node_ptr->sub_menu_style;
        }
    }

    return style;
}

/*****************************************************************************/
//  Description : init group info and item info for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN InitStaticMenuInfo(
                                 GUIMENU_CTRL_T      *menu_ctrl_ptr, //[in]
                                 MMI_MENU_GROUP_ID_T group_id        //[in]
                                 )
{
    uint32 group_count = 0;
    uint32 size = 0;

    size = CalcStaticMenuInfoSize(group_id, &group_count);

    // it will assert when SCI_ALLOC_APP fails, so do not check return value.
    menu_ctrl_ptr->root_group_info_ptr = SCI_ALLOC_APP(size);

    SCI_MEMSET(menu_ctrl_ptr->root_group_info_ptr, 0, size);

    SetStaticMenuInfo(menu_ctrl_ptr->root_group_info_ptr, group_id, &group_count);

    return TRUE;
}

/*****************************************************************************/
//  Description : calculate the size which group info and item info want
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL uint32 CalcStaticMenuInfoSize(                                // [ret] the size
                                    MMI_MENU_GROUP_ID_T group_id,   // [in] current group id
                                    uint32 *group_count_ptr         // [out] how many group in static menu
                                    )
{
    int32   i = 0;
    uint32  size = 0;
    GUIMENU_GROUP_T *group_ptr = PNULL;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != group_count_ptr); /*assert verified*/
    if (PNULL == group_count_ptr)
    {
        return size;
    }

    group_ptr = (GUIMENU_GROUP_T*)MMITHEME_GetMenuGroup(group_id);
    if (PNULL != group_ptr)
    {
        (*group_count_ptr)++;

        // the size which this group wants, it includes group info and item info.
        size += (sizeof(GUIMENU_GROUP_INFO_T) + group_ptr->item_count * sizeof(GUIMENU_ITEM_INFO_T));

        // calculate this sub menu group
        for (i = 0; i < group_ptr->item_count; i++)
        {
            size += CalcStaticMenuInfoSize(group_ptr->item_ptr[i].link_group_id, group_count_ptr);
        }
    }

    return size;
}

/*****************************************************************************/
//  Description : set all menu info, include group info and item info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: 
/*****************************************************************************/
LOCAL GUIMENU_GROUP_INFO_T* SetStaticMenuInfo(                                      // [ret] point to next group info
                                              GUIMENU_GROUP_INFO_T *group_info_ptr, // [in] set in this group info if it has group
                                              MMI_MENU_GROUP_ID_T group_id,         // [in]
                                              uint32 *remain_group_count_ptr        // [in:out] the remaining group count
                                              )
{
    int32                   i = 0;
    GUIMENU_GROUP_T         *group_ptr = PNULL;
    GUIMENU_GROUP_INFO_T    *next_group_info_ptr = group_info_ptr;
    GUIMENU_ITEM_INFO_T     *current_item_info_ptr = PNULL;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != remain_group_count_ptr); /*assert verified*/
    if (PNULL == remain_group_count_ptr)
    {
        return PNULL;
    }

    group_ptr = (GUIMENU_GROUP_T*)MMITHEME_GetMenuGroup(group_id);
    if (PNULL != group_ptr)
    {
        (*remain_group_count_ptr)--;

        // if it is the last group, set next_group_info_ptr NULL, otherwise,
        // next_group_info_ptr is pointer to next group info.
        if (0 == (*remain_group_count_ptr))
        {
            group_info_ptr->next_group_info_ptr = PNULL;
        }
        else
        {
            // each info has group info and item info. so next group info is
            // offset to the size of them.
            group_info_ptr->next_group_info_ptr = 
                    (GUIMENU_GROUP_INFO_T*)((uint32)group_info_ptr          // the prev group info address
                    + sizeof(GUIMENU_GROUP_INFO_T)                          // group info size
                    + group_ptr->item_count * sizeof(GUIMENU_ITEM_INFO_T)); // item info size
        }

        group_info_ptr->visible_child_item_num = group_ptr->item_count;
        group_info_ptr->group_id = group_id;

        next_group_info_ptr = group_info_ptr->next_group_info_ptr;

        current_item_info_ptr = GET_ITEM_INFO(group_info_ptr);

        // set all sub group info
        for (i = 0; i < group_ptr->item_count; i++)
        {
            current_item_info_ptr[i].is_grayed = FALSE;
            current_item_info_ptr[i].is_visible = TRUE;

            next_group_info_ptr = SetStaticMenuInfo(
                                        next_group_info_ptr, 
                                        group_ptr->item_ptr[i].link_group_id,
                                        remain_group_count_ptr);
        }
    }

    return next_group_info_ptr;
}

/*****************************************************************************/
//  Description : get item
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu.
//        if it is option page style, get the item in page.
/*****************************************************************************/
LOCAL GUIMENU_ITEM_T* GetItemByVisibleIndex(
                                            GUIMENU_CTRL_T *menu_ctrl_ptr,  //[in]
                                            uint16 visible_index,           //[in]
                                            GUIMENU_ITEM_INFO_T *out_item_info_ptr // [out]
                                            )
{
    int32                   i = 0;
    uint16                  item_count = 0;
    uint16                  cur_visible_index = 0;
    GUIMENU_ITEM_T          *item_ptr = PNULL;
    GUIMENU_ITEM_INFO_T     *item_info_ptr = PNULL;
    GUIMENU_GROUP_T         *menu_group_ptr = PNULL;
    MMI_MENU_GROUP_ID_T     cur_op_menu_group_id = 0;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != menu_ctrl_ptr); /*assert verified*/
    //SCI_ASSERT(PNULL != menu_ctrl_ptr->cur_group_ptr);  /*assert verified*/
    if ((PNULL == menu_ctrl_ptr)||(PNULL == menu_ctrl_ptr->cur_group_ptr))
    {
        return PNULL;
    }

    // here, we should get group and total item count to find all items.
    // and we should get item info to see whether item is visible or not.
    if (GUIMENU_STYLE_OPTION_PAGE == menu_ctrl_ptr->cur_style)
    {
        // if option page style, get item in page.
        // this method is to get group id in current option page.
        cur_op_menu_group_id = GetGroupIdInOptionPage(
                                    menu_ctrl_ptr, 
                                    menu_ctrl_ptr->cur_page_index);

        group_info_ptr = GetGroupInfo(menu_ctrl_ptr, cur_op_menu_group_id);

        menu_group_ptr = (GUIMENU_GROUP_T *)MMITHEME_GetMenuGroup(cur_op_menu_group_id);
        item_count = menu_group_ptr->item_count;
        item_info_ptr = GET_ITEM_INFO(group_info_ptr);
    }
    else
    {
        // 静态菜单必须要有cur_group_ptr和cur_group_info_ptr
        SCI_ASSERT(PNULL != menu_ctrl_ptr->cur_group_info_ptr); /*assert verified*/

        menu_group_ptr = menu_ctrl_ptr->cur_group_ptr;
        item_count = menu_ctrl_ptr->cur_group_ptr->item_count;
        item_info_ptr = GET_ITEM_INFO(menu_ctrl_ptr->cur_group_info_ptr);
    }

    // check all items in this group. for using visible index, we do not 
    // consider invisible item
    for (i = 0; i < item_count; i++)
    {
        if (item_info_ptr[i].is_visible)
        {
            if (visible_index == cur_visible_index)
            {
                item_ptr = (GUIMENU_ITEM_T*)&menu_group_ptr->item_ptr[i];

                if (PNULL != out_item_info_ptr)
                {
                    *out_item_info_ptr = item_info_ptr[i];
                }

                break;
            }

            cur_visible_index++;
        }
    }

    return item_ptr;
}

/*****************************************************************************/
//  Description : get node pointer by visible index
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL GUIMENU_NODE_T* GetNodeByVisibleIndex(
                                            uint16          node_index,      //[in]
                                            GUIMENU_NODE_T  *parent_node_ptr //[in]
                                            )
{
    int32           i = 0;
    GUIMENU_NODE_T  *find_node_ptr = PNULL;

    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != parent_node_ptr); /*assert verified*/
    if (PNULL == parent_node_ptr)
    {
        return PNULL;
    }

    if (node_index >= parent_node_ptr->visible_child_node_num)
    {
        return PNULL;
    }

    SCI_ASSERT(PNULL != parent_node_ptr->child_node_ptr);   /*assert verified*/
    find_node_ptr = parent_node_ptr->child_node_ptr;
    while (PNULL != find_node_ptr)
    {
        if (!find_node_ptr->is_visible)
        {
            find_node_ptr = find_node_ptr->neighbour_node_ptr;
            continue;
        }

        if (i == node_index)
        {
            break;
        }

        find_node_ptr = find_node_ptr->neighbour_node_ptr;
        i++;
    }

    // 如果找不到对应的node, 说明parent_node_ptr存的信息有误，肯定是插删节点时没有更新所致
    SCI_ASSERT(PNULL != find_node_ptr); /*assert verified*/

    return (find_node_ptr);
}

/*****************************************************************************/
//  Description : get group info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu
/*****************************************************************************/
LOCAL GUIMENU_GROUP_INFO_T* GetGroupInfo(
                                         GUIMENU_CTRL_T *menu_ctrl_ptr, //[in]
                                         MMI_MENU_GROUP_ID_T group_id   //[in]
                                         )
{
    GUIMENU_GROUP_INFO_T *group_info_ptr = menu_ctrl_ptr->root_group_info_ptr;
    while(PNULL != group_info_ptr)
    {
        if (group_info_ptr->group_id == group_id)
        {
            break;
        }

        group_info_ptr = group_info_ptr->next_group_info_ptr;
    }

    return group_info_ptr;
}

/*****************************************************************************/
//  Description : get group info
//  Global resource dependence : 
//  Author: hua.fang
//  Note: this method is for static menu
/*****************************************************************************/
LOCAL void FreeGroupInfoBuffer(
                               GUIMENU_GROUP_INFO_T *root_group_info_ptr //[in]
                               )
{
    GUIMENU_GROUP_INFO_T *group_info_ptr = root_group_info_ptr;

    while(PNULL != group_info_ptr)
    {
        if (PNULL != group_info_ptr->title_str.wstr_ptr)
        {
            SCI_FREE(group_info_ptr->title_str.wstr_ptr);
            group_info_ptr->title_str.wstr_len = 0;
        }

        if (PNULL != group_info_ptr->sub_title_str.wstr_ptr)
        {
            SCI_FREE(group_info_ptr->sub_title_str.wstr_ptr);
            group_info_ptr->sub_title_str.wstr_len = 0;
        }

        group_info_ptr = group_info_ptr->next_group_info_ptr;
    }
}

/*****************************************************************************/
//  Description : get group id in specified option page
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL MMI_MENU_GROUP_ID_T GetGroupIdInOptionPage(
                                                 GUIMENU_CTRL_T *menu_ctrl_ptr,     //[in]
                                                 uint16         option_page_index   //[in] visible option page index
                                                 )
{
    MMI_MENU_GROUP_ID_T     cur_op_menu_group_id = 0;
    uint16                  absolute_option_page = 0;

    // option_page_index is visible index, so translate it to absolute index.
    absolute_option_page = GUIMENU_VisibleIndex2AbsIndex(menu_ctrl_ptr, option_page_index);

    cur_op_menu_group_id = menu_ctrl_ptr->cur_group_ptr->item_ptr[absolute_option_page].link_group_id;

    return cur_op_menu_group_id;
}
//end

/*****************************************************************************/
//  Description : get item for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC GUIMENU_STATIC_ITEM_T *MMIMENU_GetStaticItem(
                                           GUIMENU_CTRL_T       *menu_ctrl_ptr,    // [in]
                                           MMI_MENU_GROUP_ID_T  group_id,          // [in]
                                           MMI_MENU_ID_T        menu_id            // [in]
                                           )
{
    GUIMENU_STATIC_ITEM_T *cur_static_item_ptr = menu_ctrl_ptr->static_item_ptr;

    while (PNULL != cur_static_item_ptr)
    {
        if (cur_static_item_ptr->group_id == group_id 
            && cur_static_item_ptr->menu_id == menu_id)
        {
            break;
        }

        cur_static_item_ptr = cur_static_item_ptr->next_item_ptr;
    }

    return cur_static_item_ptr;
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StopRedrawTimer(
                           GUIMENU_CTRL_T *menu_ctrl_ptr
                           )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->redrew_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->redrew_timer_id);
        menu_ctrl_ptr->redrew_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void StartRedrawTimer(
                            GUIMENU_CTRL_T *menu_ctrl_ptr
                            )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->redrew_timer_id)
    {
        menu_ctrl_ptr->redrew_timer_id = MMK_CreateWinTimer(
            menu_ctrl_ptr->handle, 
            MMITHEME_MENU_REDRAW_TIME,
            FALSE );
    }
}

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopRedrawTimer(
                                    GUIMENU_CTRL_T *menu_ctrl_ptr
                                    )
{
    StopRedrawTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartRedrawTimer(
                                     GUIMENU_CTRL_T *menu_ctrl_ptr
                                     )
{
    StartRedrawTimer(menu_ctrl_ptr);
}

/*****************************************************************************/
//  Description : set the max width for auto popup menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note: only effective at auto popup menu
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetMaxWidth(
                                   MMI_CTRL_ID_T    ctrl_id,            // [in]
                                   uint16           max_horz_width,     // [in]
                                   uint16           max_vert_width      // [in]
                                   )
{
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T *menu_ctrl_ptr = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        menu_ctrl_ptr->max_horz_width = max_horz_width;
        menu_ctrl_ptr->max_vert_width = max_vert_width;

        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
//  Description : set effective point in menu rect
//  Global resource dependence : 
//  Author: hua.fang
//  Note: only effective at auto popup menu
//        effective_point will indicate which point in menu rect will be effective
//        eg. if effective_point is GUIMENU_EP_BOTTOM_RIGHT, then right and bottom
//          in menu rect will be the start display point. the left and top will
//          be minus width and height automatically.
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetEffectivePoint(
                                         MMI_CTRL_ID_T    ctrl_id,          // [in]
                                         GUIMENU_EP_E     effective_point   // [in]
                                         )
{
    BOOLEAN result = FALSE;
    GUIMENU_CTRL_T *menu_ctrl_ptr = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr)
    {
        menu_ctrl_ptr->effective_point = effective_point;

        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
//  Description : set state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetState( 
                                MMI_CTRL_ID_T    ctrl_id,
                                uint32  src_state,
                                BOOLEAN is_true
                                )
{
    return GUIMENU_SetStatePtr(GetMenuPtr(ctrl_id), src_state, is_true);
}


/*****************************************************************************/
//  Description : set state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetState( 
                                MMI_CTRL_ID_T    ctrl_id,
                                uint32  src_state
                                )
{
    return GUIMENU_GetStatePtr(GetMenuPtr(ctrl_id), src_state);
}

/*****************************************************************************/
//  Description : set state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetStatePtr( 
                                   GUIMENU_CTRL_T *menu_ctrl_ptr,
                                   uint32  src_state,
                                   BOOLEAN is_true
                                   )
{
    BOOLEAN result = FALSE;

    if (PNULL != menu_ctrl_ptr)
    {
        SetState(&menu_ctrl_ptr->menu_state, src_state, is_true);

        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
//  Description : get state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetStatePtr( 
                                   GUIMENU_CTRL_T *menu_ctrl_ptr,
                                   uint32  src_state
                                   )
{
    BOOLEAN result = FALSE;

    if (PNULL != menu_ctrl_ptr)
    {
        result = GetState(menu_ctrl_ptr->menu_state, src_state);
    }

    return result;
}

/*****************************************************************************/
//  Description : set state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL void SetState( 
                    uint32* dst_state_ptr,
                    uint32  src_state,
                    BOOLEAN is_true
                    )
{
    uint32 state;
    
    //kevin.lou modified:delete assert
    //SCI_ASSERT(PNULL != dst_state_ptr); /*assert verified*/
    if (PNULL == dst_state_ptr)
    {
        return;
    }
    
    state = *dst_state_ptr;
    
    if( is_true )
    {
        state |= src_state;
    }
    else
    {
        state &= ~src_state;
    }
    
    *dst_state_ptr = state;
}

/*****************************************************************************/
//  Description : get state
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetState(
                       uint32 dst_state,
                       uint32 src_state
                       )
{
    return (BOOLEAN)( ( dst_state & src_state ) == src_state );
}

/*****************************************************************************/
//  Description : 设置button的显示风格
//  Global resource dependence : 
//  Author: hua.fang
//  Note:该方法只对GUIMENU_STYLE_POPUP_AUTO,GUIMENU_STYLE_POPUP_CHECK,GUIMENU_STYLE_POPUP_RADIO,GUIMENU_STYLE_POPUP_RECT有效
/*****************************************************************************/
PUBLIC void GUIMENU_SetButtonStyle(
                                   MMI_CTRL_ID_T            ctrl_id,         // [in]
                                   GUIMENU_BUTTON_INFO_T    *button_info_ptr // [in]
                                   )
{
    GUIMENU_CTRL_T          *menu_ctrl_ptr  = PNULL;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;
    GUIMENU_NODE_T          *root_node_ptr  = PNULL;
    GUIMENU_NODE_T          *node_ptr       = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != button_info_ptr && PNULL != menu_ctrl_ptr)
    {
        if (menu_ctrl_ptr->is_static && button_info_ptr->is_static)
        {
            group_info_ptr = GetGroupInfo(menu_ctrl_ptr, button_info_ptr->group_id);
            if (PNULL != group_info_ptr)
            {
                group_info_ptr->button_style = button_info_ptr->button_style;

                if (menu_ctrl_ptr->cur_group_id == button_info_ptr->group_id)
                {
                    menu_ctrl_ptr->cur_button_style = button_info_ptr->button_style;
                }
            }
        }
        else if (!menu_ctrl_ptr->is_static && !button_info_ptr->is_static)
        {
            //get parent node pointer
            root_node_ptr = menu_ctrl_ptr->root_node_ptr;
            node_ptr = FindNodeById(button_info_ptr->node_id, root_node_ptr);

            if (PNULL != node_ptr)
            {
                node_ptr->button_style = button_info_ptr->button_style;

                if (menu_ctrl_ptr->cur_parent_node_ptr->node_id == button_info_ptr->node_id)
                {
                    menu_ctrl_ptr->cur_button_style = button_info_ptr->button_style;
                }
            }
        }
    }
}

/***************************************************************************//*!
@brief 设置格数
@author hua.fang
@param ctrl_id [in] 控件ID
@param grid_count [in] 格数
@note GUIMENU_STYLE_OPTIONS有效
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetGridNum(
                                  MMI_CTRL_ID_T  ctrl_id,      // [in]
                                  uint16         grid_count    // [in]
                                  )
{
    BOOLEAN         result = FALSE;
    GUIMENU_CTRL_T  *menu_ctrl_ptr  = PNULL;

    menu_ctrl_ptr = GetMenuPtr(ctrl_id);
    if (PNULL != menu_ctrl_ptr && GUIMENU_STYLE_OPTIONS == menu_ctrl_ptr->cur_style)
    {
        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DestoryMenu)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->DestoryMenu(menu_ctrl_ptr);
        }

        menu_ctrl_ptr->options_menu_theme.max_item_num = grid_count;

        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->InitMenu(menu_ctrl_ptr);
        }

        if (PNULL != ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect)
        {
            ((GUIMENU_PROCESS_T*)(menu_ctrl_ptr->process_func[menu_ctrl_ptr->cur_style]))->HandleMenuModifyRect(menu_ctrl_ptr);
        }

        result = TRUE;
    }

    return result;
}

/*****************************************************************************/
//  Description : 更新下面的窗口
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_UpdatePrevWindow(
                                     GUIMENU_CTRL_T *menu_ctrl_ptr  // [in]
                                     )
{
    MMI_HANDLE_T    prev_win_handle = 0;

    prev_win_handle = MMK_GetPrevWinHandle(menu_ctrl_ptr->win_handle);
    MMK_SendMsg(prev_win_handle,MSG_FULL_PAINT,PNULL);
}

/*****************************************************************************/
//  Description : 临时隐藏前几项
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetItemVisibleByNum(
                                           GUIMENU_CTRL_T   *menu_ctrl_ptr,     // [in]
                                           uint16           num,                // [in]
                                           BOOLEAN          visible             // [in]
                                           )
{
    BOOLEAN result = FALSE;
    int32   i = 0;
    uint16  index = 0;
    GUIMENU_GROUP_INFO_T    *group_info_ptr = PNULL;
    GUIMENU_GROUP_T         *group_ptr      = PNULL;
    GUIMENU_ITEM_INFO_T     *item_info_ptr  = PNULL;
    GUIMENU_NODE_T          *parent_node_ptr = PNULL;
    GUIMENU_NODE_T          *cur_node_ptr   = PNULL;

    if (visible && PNULL != menu_ctrl_ptr->temp_invisible_index_ptr)    // 之前有隐藏项，现在将其显示
    {
        if (menu_ctrl_ptr->is_static)
        {
            group_ptr = menu_ctrl_ptr->cur_group_ptr;
            group_info_ptr = menu_ctrl_ptr->cur_group_info_ptr;
            item_info_ptr = GET_ITEM_INFO(group_info_ptr);

            for (i = 0; i < menu_ctrl_ptr->temp_invisible_num; i++)
            {
                // 之前没隐藏的索引号
                index = menu_ctrl_ptr->temp_invisible_index_ptr[i];
                SCI_ASSERT(index < group_ptr->item_count);  /*assert verified*/

                // 如果用户没有恢复过，将其显示
                if (!item_info_ptr[index].is_visible)
                {
                    item_info_ptr[index].is_visible = TRUE;
                    group_info_ptr->visible_child_item_num++;
                    menu_ctrl_ptr->item_total_num++;
                }
            }
        }
        else
        {
            // get parent node pointer
            parent_node_ptr = menu_ctrl_ptr->cur_parent_node_ptr;

            for (i = 0; i < menu_ctrl_ptr->temp_invisible_num; i++)
            {
                index = menu_ctrl_ptr->temp_invisible_index_ptr[i];
                SCI_ASSERT(index < parent_node_ptr->child_node_num);    /*assert verified*/

                // find specified node
                cur_node_ptr = GUIMENU_FindNodeByIndex(index, parent_node_ptr);

                if (PNULL != cur_node_ptr && !cur_node_ptr->is_visible)
                {
                    cur_node_ptr->is_visible = TRUE;
                    parent_node_ptr->visible_child_node_num++;
                    menu_ctrl_ptr->item_total_num++;
                }
            }
        }

        SCI_FREE(menu_ctrl_ptr->temp_invisible_index_ptr);
        menu_ctrl_ptr->temp_invisible_num = 0;
        result = TRUE;
    }
    else if (!visible && PNULL == menu_ctrl_ptr->temp_invisible_index_ptr)  // 之前没有隐藏过，现在隐藏
    {
        SCI_ASSERT(num > 0);    /*assert verified*/
        SCI_ASSERT(num < menu_ctrl_ptr->item_total_num);    /*assert verified*/

        menu_ctrl_ptr->temp_invisible_num = 0;
        menu_ctrl_ptr->temp_invisible_index_ptr = SCI_ALLOC_APP(num * sizeof(uint16));
        SCI_MEMSET(menu_ctrl_ptr->temp_invisible_index_ptr, 0, num * sizeof(uint16));

        if (menu_ctrl_ptr->is_static)
        {
            group_ptr = menu_ctrl_ptr->cur_group_ptr;
            group_info_ptr = menu_ctrl_ptr->cur_group_info_ptr;
            item_info_ptr = GET_ITEM_INFO(group_info_ptr);

            for (i = 0; i < group_ptr->item_count; i++)
            {
                if (item_info_ptr[i].is_visible)        // while being invisible
                {
                    item_info_ptr[i].is_visible = FALSE;
                    group_info_ptr->visible_child_item_num--;
                    menu_ctrl_ptr->item_total_num--;

                    // 存储被隐藏的索引号
                    menu_ctrl_ptr->temp_invisible_index_ptr[menu_ctrl_ptr->temp_invisible_num] = i;
                    menu_ctrl_ptr->temp_invisible_num++;

                    // 已达到个数
                    if (menu_ctrl_ptr->temp_invisible_num == num)
                    {
                        result = TRUE;
                        break;
                    }
                }
            }
        }
        else
        {
            // get parent node pointer
            parent_node_ptr = menu_ctrl_ptr->cur_parent_node_ptr;

            for (i = 0; i < parent_node_ptr->child_node_num; i++)
            {
                // find specified node
                cur_node_ptr = GUIMENU_FindNodeByIndex(i, parent_node_ptr);

                if (PNULL != cur_node_ptr && cur_node_ptr->is_visible)
                {
                    cur_node_ptr->is_visible = FALSE;
                    parent_node_ptr->visible_child_node_num--;
                    menu_ctrl_ptr->item_total_num--;

                    // 存储被隐藏的索引号
                    menu_ctrl_ptr->temp_invisible_index_ptr[menu_ctrl_ptr->temp_invisible_num] = i;
                    menu_ctrl_ptr->temp_invisible_num++;

                    // 已达到个数
                    if (menu_ctrl_ptr->temp_invisible_num == num)
                    {
                        result = TRUE;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        //SCI_TRACE_LOW:"GUIMENU_SetItemVisibleByNum error: visible: %d , ptr: %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,GUIMENU_6878_112_2_18_3_20_31_248,(uint8*)"dd", visible, menu_ctrl_ptr->temp_invisible_index_ptr);
    }

    return result;
}

/*****************************************************************************/
//  Description : 关闭菜单
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_Close(
                          GUIMENU_CTRL_T *menu_ctrl_ptr     // [in]
                          )
{
    NotifyParentMsg(menu_ctrl_ptr->handle, MSG_APP_CANCEL);
}

/*****************************************************************************/
//  Description : 发penok消息
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
// 内部方法
/*****************************************************************************/
PUBLIC void GUIMENU_NotifyPenOK(
                                GUIMENU_CTRL_T *menu_ctrl_ptr     // [in]
                                )
{
    NotifyParentMsg(menu_ctrl_ptr->handle, MSG_TP_PRESS_UP);
}

/*****************************************************************************/
//  Description : check button style
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
//  button指的是context menu底下的按钮
/*****************************************************************************/
LOCAL void CheckButtonStyle(
                            GUIMENU_CTRL_T *menu_ctrl_ptr
                            )
{
    // 因为CHECK风格点击后不发消息，所以必须要有OK CANCEL键
    if (GUIMENU_STYLE_POPUP_CHECK == menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->cur_button_style = GUIMENU_BUTTON_STYLE_OK_CANCEL;
    }
    else
    {
        // iphone风格下，因为没有cancel按钮，所以要出cancel button
        if (GUIMENU_BUTTON_STYLE_NONE == menu_ctrl_ptr->cur_button_style
            && MMITHEME_IsIstyle())
        {
            menu_ctrl_ptr->cur_button_style = GUIMENU_BUTTON_STYLE_CANCEL;
        }
    }
}

#ifdef GUISUBPOPMENU_CONTEXT_SUPPORT
/*****************************************************************************/
//  Description : check sub menu style
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
LOCAL void CheckSubMenuStyle(
                             GUIMENU_CTRL_T *menu_ctrl_ptr
                             )
{
    if (GUIMENU_STYLE_POPUP_CHECK != menu_ctrl_ptr->cur_style
        && GUIMENU_STYLE_POPUP_RADIO != menu_ctrl_ptr->cur_style
        && GUIMENU_STYLE_THIRD != menu_ctrl_ptr->cur_style
        && GUIMENU_STYLE_SECOND != menu_ctrl_ptr->cur_style)
    {
        menu_ctrl_ptr->cur_style = GUIMENU_STYLE_POPUP_AUTO;
    }
}
#endif
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
#if defined(PDA_UI_MAINMENU_SUPPORT_SELECT_ANIM)
/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
LOCAL void StartHighlightIconDispTimer(
									   GUIMENU_CTRL_T *menu_ctrl_ptr
									   )
{
    if (PNULL != menu_ctrl_ptr && 0 == menu_ctrl_ptr->highlight_timer_id)
    {
        menu_ctrl_ptr->highlight_timer_id = MMK_CreateWinTimer(
			menu_ctrl_ptr->handle, 
			GO_HIGHLIGHT_DISPLAY_TIMER_OUT,
			FALSE);
    }
}
/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
LOCAL void StopHighlightIconDispTimer(
									  GUIMENU_CTRL_T *menu_ctrl_ptr
									  )
{
    if (PNULL != menu_ctrl_ptr && 0 != menu_ctrl_ptr->highlight_timer_id)
    {
        MMK_StopTimer(menu_ctrl_ptr->highlight_timer_id);
        menu_ctrl_ptr->highlight_timer_id = 0;
    }
}

/*****************************************************************************/
//  Description : stop Highlight timer
//  Global resource dependence : 
//  Author:
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopHighlightIconDispTimer(
											   GUIMENU_CTRL_T     *menu_ctrl_ptr
											   )
{
    StopHighlightIconDispTimer(menu_ctrl_ptr);
}
/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartHighlightIconDispTimer(
												GUIMENU_CTRL_T     *menu_ctrl_ptr
												)
{
    GUIMENU_SELECT_EFFECT_STYLE_E     mainmenu_select_style = SELECT_EFFECT_ROUNDLIGHT;
	
    mainmenu_select_style = MMIAPISET_GetMainmenuSelectStyle();
    
    if(SELECT_EFFECT_STATIC != mainmenu_select_style
        &&(mainmenu_select_style<SELECT_EFFECT_MAX))
    {
        StartHighlightIconDispTimer(menu_ctrl_ptr);
    }
}
#endif//(PDA_UI_MAINMENU_SUPPORT_SELECT_ANIM)
#endif//PDA_UI_SUPPORT_MANIMENU_GO

#endif

