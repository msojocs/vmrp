
#define _MMICONNECT_WINTAB_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_app_connection_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "mmi_default.h"
#include "mmidisplay_data.h"
#include "mmiconnection_id.h"
#include "window_parse.h"
#include "mmiacc.h"
#include "mmi_menutable.h"
#include "mmi_common.h"
#include "mmicom_string.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmiidle_subwintab.h"
#include "mmiphone_export.h"
#include "mmiconnection_menutable.h"
#include "mmiconnection_text.h"
#include "mmiconnection_image.h"
#include "mmiconnection_id.h"
#include "mmitv_out.h"
#include "mmiconnection_export.h"
#include "mmibt_export.h" // baokun add
#include "mmi_nv.h"
#include "mmimenu_export.h"
#include "mmipub.h"
#include "guirichtext.h"
#include "guiedit.h"
#include "guiim_base.h"
#include "mmiset_text.h"
//#include "mmiset_call.h"
#include "mmiset_call_export.h"
#ifdef WIFI_SUPPORT
#include "mmiwifi_export.h"
#endif
#include "mn_api.h"
#include "mmipdp_export.h"
#ifndef MMI_PDA_SUPPORT
#include "guictrl_api.h"
#endif
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef MMS_SUPPORT
#include "mmimms_export.h"
#endif
//#include "mmibrw_text.h"
#ifdef BLUETOOTH_SUPPORT
//#include "mmibt_text.h"
#endif
#ifdef GPS_SUPPORT
#include "mmigps_text.h"
#endif
#ifdef _ATEST_
#include "atest_ota.h"
#endif//_ATEST_
//#include "mmieng.h"
#include "mmieng_export.h"
#ifdef DCD_SUPPORT
#include "mmidcd_export.h"
#endif
#ifdef MMI_GPRS_SUPPORT
#include "mmi_data.h"
#endif
#include "mmi_appmsg.h"
#include "mmiconnection_internal.h"
#if defined(DATA_ACCOUNT_IN_TCARD_SUPPORT) && defined(BROWSER_SUPPORT)
#include "gui_ucs2b_converter.h"
#include "mmiudisk_export.h"
#endif
#ifdef MMI_GPRS_SWITCH_SUPPORT
#include "mmiidle_statusbar.h"
#endif
/*---------------------------------------------------------------------------*/
/*                          MACRO DEFINITION                                 */
/*---------------------------------------------------------------------------*/
#define MMICONNECTION_DETAIL_LINE_SPACE    5

#ifdef MMI_GPRS_SUPPORT
#define NETWORK_ACCOUNT_MAGIC_FLAG "NTAC"
#define NETWORK_ACCOUNT_MAGIC_OFFSET 0x00
#define NETWORK_ACCOUNT_MAGIC_LENGTH 0x04

#define NETWORK_ACCOUNT_VERSION_OFFSET 0x04
#define NETWORK_ACCOUNT_VERSION_LENGTH 0x02

#define NETWORK_ACCOUNT_NUM_OFFSET 0x06

#define NETWORK_ACCOUNT_HEADER_LENGTH 0x08

#define NETWORK_ACCOUNT_OFFSET_LENGTH 0x04

#define NETWORK_ACCOUNT_ITEM_MCC_OFFSET 0x02
#define NETWORK_ACCOUNT_ITEM_MNC_OFFSET 0x04
#define NETWORK_ACCOUNT_ITEM_PORT_OFFSET 0x06
#define NETWORK_ACCOUNT_ITEM_ACCESS_TYPE_OFFSET 0x08
#define NETWORK_ACCOUNT_ITEM_ACCESS_OPTION_OFFSET 0x09
#define NETWORK_ACCOUNT_ITEM_AUTH_TYPE_OFFSET 0x0A
#define NETWORK_ACCOUNT_ITEM_ACCOUNT_TYPE_OFFSET 0x0B

#define NETWORK_ACCOUNT_ITEM_NAME_LEN_OFFSET 0x0C
#define NETWORK_ACCOUNT_ITEM_NAME_LEN_LENGTH 0x02

#define NETWORK_ACCOUNT_ITEM_GATEWAY_LENGTH 0x02
#define NETWORK_ACCOUNT_ITEM_DNS1_LENGTH 0x02
#define NETWORK_ACCOUNT_ITEM_DNS2_LENGTH 0x02
#define NETWORK_ACCOUNT_ITEM_USERNAME_LENGTH 0x02
#define NETWORK_ACCOUNT_ITEM_PASSWORD_LENGTH 0x02
#define NETWORK_ACCOUNT_ITEM_APN_LENGTH 0x02
#define NETWORK_ACCOUNT_ITEM_HOMEPAGE_LENGTH 0x02

#if 1
#define CONVERT2U8TOU16(a, b) (a + ((b << 8)&0xFF00))
#define CONVERT4U8TOU32(a, b, c, d) (a + ((b << 8)&0xFF00) + ((c << 16)&0xFF0000) + ((d << 24)&0xFF000000))
#else
#define CONVERT2U8TOU16(a, b) (b + ((a << 8)&0xFF00))
#define CONVERT4U8TOU32(a, b, c, d) (d + ((c << 8)&0xFF00) + ((b << 16)&0xFF0000) + ((a << 24)&0xFF000000))
#endif
#endif

#define MMICONNECTION_DEFAULT_DNS       "0.0.0.0"
#define MMICONNECTION_INVALID_GATEWAY_FOR_SNTP              "0.0.0.0"
#define MMICONNECTION_CMCC_DERECT_LINK_APN              "cmnet"

#ifdef DATA_ACCOUNT_IN_TCARD_SUPPORT
#define NETACCOUNT_FILE_TCARD_PATH      L"E:\\System\\Network_Account\\"
#define NETACCOUNT_FILE_TCARD1_PATH     L"F:\\System\\Network_Account\\"
#define NETACCOUNT_FILE_UDISK_PATH      L"D:\\System\\Network_Account\\"
#ifdef WIN32
#define NETACCOUNT_FILE_DOWNLOAD_URL   L"http://172.16.14.173:8082/daveruan/R_NETWORK_ACCOUNT.ntac"  //Index of/daveruan/R_NETWORK_ACCOUNT.ntac
#else
#define NETACCOUNT_FILE_DOWNLOAD_URL   L"http://51wre.com/ntac/"
#endif
#define NETACCOUNT_FILE_PATH "E:\\System\\Network_Account\\R_NETWORK_ACCOUNT"
#define NETACCOUNT_FILE_NAME "R_NETWORK_ACCOUNT.ntac"
#endif
/*---------------------------------------------------------------------------*/
/*                         STATIC DEFINITION                                 */
/*---------------------------------------------------------------------------*/
#ifdef MMI_GPRS_SUPPORT
LOCAL MMICONNECTION_SETTING_T  s_connection_setting_info = {0};
LOCAL MN_DUAL_SYS_E s_connection_handling_sim_id = MN_DUAL_SYS_1;
LOCAL MMICONNECTION_CALLBACK_FUNC s_connection_callback_func_ptr = PNULL;
#endif
#ifdef MMI_GPRS_SWITCH_SUPPORT
LOCAL BOOLEAN s_gprs_turned_off = FALSE;
#endif
MMICONNECTION_NOTIFY_LIST_T    *s_subscriber_list_ptr = PNULL;

//MS00216980 cheney
#define MMIDCD_HOME_PAGE             "http://dcd1.monternet.com"
#define MMIDCD_HOME_PAGE_TEST    "http://218.206.176.248"

#ifndef MMI_GUI_STYLE_TYPICAL
#define MMICONNECT_STRNUM_MAX_LEN                    40
#endif /* MMI_GUI_STYLE_TYPICAL */

//说明：此表用来做连接设置的搜索用，可以扩充，注意wap的主页可以省略http://，mms的彩信中心不可以省略
//table数目原则上不大于24条，如果大于24条，需要修改MMICMSBRW_Reaserch函数
#ifdef MMI_GPRS_SUPPORT
#define LINK_SETTING_TABLE_LEN  (sizeof(setting_search_table)/sizeof(MMICONNECTION_LINKSETTING_ITEM_T))
const MMICONNECTION_LINKSETTING_ITEM_T  setting_search_table[]=
{
    //China
    //MCC/MNC:460/00 CMCC
    {460, 00, 80, 1, MN_PAP, MMICONNECTION_ACCOUNT_BROWSER, TXT_CONNECTION_CMCCWAP, "10.0.0.172", "", "", "cmwap", "http://wap.monternet.com"},
    {460, 00, 80, 1, MN_PAP, MMICONNECTION_ACCOUNT_MMS, TXT_CONNECTION_CHINAMOBILEMMS, "10.0.0.172", "", "", "cmwap", "http://mmsc.monternet.com"},
    {460, 00, 0, 1, MN_PAP, MMICONNECTION_ACCOUNT_BROWSER, TXT_CONNECTION_CMCCWWW, "0.0.0.0", "", "", "cmnet", "http://www.monternet.com"},

    //MCC/MNC:460/01 CUCC
    {460, 01, 80, 1, MN_PAP, MMICONNECTION_ACCOUNT_BROWSER, TXT_CONNECTION_CUCC_WAP, "10.0.0.172", "", "", "uniwap", "http://wap.uni-info.com.cn"},
    {460, 01, 80, 1, MN_PAP, MMICONNECTION_ACCOUNT_MMS, TXT_CONNECTION_CUCC_MMS, "10.0.0.172", "", "", "uniwap", "http://mmsc.myuni.com.cn"},

#ifdef DCD_SUPPORT
    {460, 00, 80, 1, MN_PAP, MMICONNECTION_ACCOUNT_DCD, TXT_CONNECTION_CMCCDCD, "10.0.0.172", "", "", "cmwap", MMIDCD_HOME_PAGE},
#endif
};
#endif

#define LINK_CUSTOMED_LEN  (sizeof(s_customed_index)/sizeof(s_customed_index[0]))

#ifdef CMCC_UI_STYLE
#ifndef DCD_SUPPORT
static uint8 s_customed_index[]={0,1,2};//移动定制项在上表中的编号
#else
static uint8 s_customed_index[]={0,1,2,5};//移动定制项在上表中的编号
#endif
#else
#ifndef LOW_MEMORY_SUPPORT_32X16
#ifndef DCD_SUPPORT
const uint8 s_customed_index[]={0,1,2,3,4};//移动定制项在上表中的编号
#else
const uint8 s_customed_index[]={0,1,2,3,4,5};//移动定制项在上表中的编号
#endif
#else
const uint8 s_customed_index[]={0,1,2};//移动定制项在上表中的编号
#endif
#endif

#define LINK_CUSTOMED_RES_LEN  (sizeof(s_customed_res_index)/sizeof(s_customed_res_index[0]))
const uint8 s_customed_res_index[]={0,1,2,3,4};//工具默认只加载前面5个
#ifdef MMI_GPRS_SUPPORT
LOCAL uint8 s_connection_index = 1;  /*lint -e551*/
#endif

typedef BOOLEAN (*PLMN_CMP_CALLBACK)(PHONE_PLMN_NAME_E plmn1, PHONE_PLMN_NAME_E plmn2);

/**--------------------------------------------------------------------------*
 **                         TYPE DEFINITION                                  *
 **--------------------------------------------------------------------------*/
typedef enum
{ 
    MMICONNECTION_LINKSET_OP_NULL,
    MMICONNECTION_LINKSET_OP_EDIT,
    MMICONNECTION_LINKSET_OP_READ,
    MMICONNECTION_LINKSET_OP_NEW
}MMICONNECTION_LINKSET_OP_E;

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
// 	Description : set the select list of the searchresult list box to high priority
//	Global resource dependence : 
//  Author:wenbao.bai
//	Note: 
/****************************************************************************/
LOCAL void MMICONNECTION_SetHighPriList(MN_DUAL_SYS_E dual_sys, uint8 list_index);

/*****************************************************************************/
// 	Description :clear the priority of the setting link lists box
//	Global resource dependence : 
//  Author:wenbao.bai
//	Note: 
/****************************************************************************/
LOCAL void MMICONNECTION_ClearHighPriList(MN_DUAL_SYS_E dual_sys);
#endif

/*****************************************************************************/
//  Description :
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void ChangeAppSelectedIndex(MN_DUAL_SYS_E dual_sys, uint16 list_index, MMICONNECTION_NOTIFY_EVENT_E event);

/*****************************************************************************/
//  Description :append one line icon text list item
//  Global resource dependence : none
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
LOCAL void AppendOneLineTextListItem(
                                        MMI_CTRL_ID_T       ctrl_id,
                                        GUIITEM_STYLE_E    item_style,
                                        MMI_TEXT_ID_T       left_softkey_id,
                                        MMI_IMAGE_ID_T      icon_id,
                                        wchar*        wstr_ptr,
                                        uint16              wstr_len,
                                        uint16              pos,
                                        BOOLEAN             is_update
                                     );

/*****************************************************************************/
// 	Description :append one line check list item, which has three softkey, and mid softkey is update dynamic
//	Global resource dependence : none
//  Author: bei.wu
//	Note:
/*****************************************************************************/							
LOCAL void AppendOneLineCheckListItem(
                                      MMI_CTRL_ID_T       ctrl_id,    
                                      GUIITEM_STYLE_E    item_style,
                                      MMI_TEXT_ID_T       left_softkey_id,
                                      MMI_TEXT_ID_T       middle_softkey_id,
                                      MMI_IMAGE_ID_T      icon_id,
                                      wchar*        wstr_ptr,
                                      uint16              wstr_len,
                                      uint16              pos,
                                      BOOLEAN             is_update
                                      );
	
/****************************************************************************/	
// 	Description : translate the setting item to richtext item, and add it to richtext control
//	Global resource dependence : none
//  Author: bei.wu
//	Note: 
/*****************************************************************************/
LOCAL void SetSettingDetailEntry(MMI_CTRL_ID_T ctrl_id, 
                                 MMICONNECTION_LINKSETTING_DETAIL_T* item
                                 );

/*****************************************************************************/
// 	Description : Connection menu
//	Global resource dependence : 
//  Author: ryan.xu
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleConnectionMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param);

#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
//  Description : Is Same Network Plmn
//  Global resource dependence : none
//  Author: dave.ruan
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN IsSameNetworkPlmn(PHONE_PLMN_NAME_E plmn1, PHONE_PLMN_NAME_E plmn2);

/*****************************************************************************/
//  Description : Is Same Network name
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN IsSameNetworkName(PHONE_PLMN_NAME_E plmn1, PHONE_PLMN_NAME_E plmn2);

/*****************************************************************************/
// 	Description : handle message of setting list window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingListWinMsg(
                                           MMI_WIN_ID_T	    win_id,
                                           MMI_MESSAGE_ID_E	msg_id,
                                           DPARAM           param
                                           );

/*****************************************************************************/
// 	Description : handle message of setting link list window of browser
//	Global resource dependence : none
//  Author: bei.wu
//	Note:   cr115674
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkDetailViewWinMsg(
                                                     MMI_WIN_ID_T	    win_id, 
                                                     MMI_MESSAGE_ID_E	msg_id, 
                                                     DPARAM				param
                                                     );

/*****************************************************************************/
// 	Description : handle message of setting link list menu window of connection
//      This menu is to deal to a selected list such as,edit,delete,etc.
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkListMenuWinMsg(
                                                   MMI_WIN_ID_T	    win_id, 
                                                   MMI_MESSAGE_ID_E	msg_id, 
                                                   DPARAM			param
                                                   );

/*****************************************************************************/
// 	Description : handle message of  search link list window
//	Global resource dependence : none
//  Author: wenbao.bai
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSearchResultLinkListWinMsg(
                                                    MMI_WIN_ID_T    	win_id,
                                                    MMI_MESSAGE_ID_E	msg_id,
                                                    DPARAM				param
                                                    );

/*****************************************************************************/
//     Description : to set the MMICONNECTION_GPRS_SETTINGS_CTRL_ID
//    Global resource dependence : 
//  Author:wancan.you
//    Note: 
/*****************************************************************************/
LOCAL void SetGPRSSettingsWin(void);

/*****************************************************************************/
//     Description : to handle window message about gprs settings.
//    Global resource dependence : 
//  Author: wancan.you
//    Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleGPRSSettingsWinMsg(
                                                       MMI_WIN_ID_T        win_id, 
                                                       MMI_MESSAGE_ID_E msg_id, 
                                                       DPARAM            param
                                                       );

/*****************************************************************************/
// 	Description : handle message of Add  setting list window of connection
//	Global resource dependence : none
//  Author: wenbao.bai
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleAddSettingListWinMsg(
                                              MMI_WIN_ID_T	    win_id, 
                                              MMI_MESSAGE_ID_E	msg_id, 
                                              DPARAM			param
                                              );

/*****************************************************************************/
// 	Description : handle message of setting link list window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkMenuWinMsg(
                                                MMI_WIN_ID_T    	win_id, 
                                                MMI_MESSAGE_ID_E	msg_id, 
                                                DPARAM				param
                                                );

/*****************************************************************************/
// 	Description : handle message of Securityconnect setting list window of connection
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingSecurityconnectListWinMsg(
                                                          MMI_WIN_ID_T	win_id, 
                                                          MMI_MESSAGE_ID_E	msg_id, 
                                                          DPARAM				param
                                                          );


/*****************************************************************************/
// 	Description : handle message of input username name window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputUserNameWinMsg(
                                             MMI_WIN_ID_T	win_id, 
                                             MMI_MESSAGE_ID_E	msg_id, 
                                             DPARAM				param
                                             );

/*****************************************************************************/
// 	Description : handle message of input password window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputPasswordWinMsg(
                                             MMI_WIN_ID_T	win_id, 
                                             MMI_MESSAGE_ID_E	msg_id, 
                                             DPARAM				param
                                             );

/*****************************************************************************/
// 	Description : handle message of input apn window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputApnWinMsg(
                                        MMI_WIN_ID_T	win_id, 
                                        MMI_MESSAGE_ID_E	msg_id, 
                                        DPARAM				param
                                        );


/*****************************************************************************/
// 	Description : handle message of input gateway window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputGatewayWinMsg(
                                            MMI_WIN_ID_T	win_id, 
                                            MMI_MESSAGE_ID_E	msg_id, 
                                            DPARAM				param
                                            );

/*****************************************************************************/
// 	Description : handle message of port list window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePortListWinMsg(
                                              MMI_WIN_ID_T	win_id, 
                                              MMI_MESSAGE_ID_E	msg_id, 
                                              DPARAM				param
                                              );

/*****************************************************************************/
// 	Description : handle message of input agent name window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputAgentNameWinMsg(
                                              MMI_WIN_ID_T	win_id, 
                                              MMI_MESSAGE_ID_E	msg_id, 
                                              DPARAM				param
                                              );

/*****************************************************************************/
// 	Description : handle message of input homepage window of connection
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputHomepageWinMsg(
                                             MMI_WIN_ID_T	win_id, 
                                             MMI_MESSAGE_ID_E	msg_id, 
                                             DPARAM				param
                                             );

/*****************************************************************************/
// 	Description : 
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkAdvanceWinMsg(
                                                MMI_WIN_ID_T	win_id, 
                                                MMI_MESSAGE_ID_E	msg_id, 
                                                DPARAM				param
                                                );

/*****************************************************************************/
// 	Description : handle message of setting dns window of connection
//	Global resource dependence : none
//  Author: bei.wu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkDNSWinMsg(
                                                MMI_WIN_ID_T	    win_id, 
                                                MMI_MESSAGE_ID_E	msg_id, 
                                                DPARAM				param
                                                );
/*****************************************************************************/
// 	Description : handle message of setting net type window of connection
//	Global resource dependence : none
//  Author: bei.wu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkNetTypeWinMsg(
                                                MMI_WIN_ID_T	    win_id, 
                                                MMI_MESSAGE_ID_E	msg_id, 
                                                DPARAM				param
                                                );

#ifdef ACCOUNT_AUTO_ADAPT_SUPPORT
/*****************************************************************************/
//  Description : Auto Adapting Network account
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL void AutoAdapting(MN_DUAL_SYS_E dual_sys, MMICONNECTION_ACCOUNT_TYPE_E account_type, BOOLEAN is_need_clean);
#endif

#ifndef MMI_MULTI_SIM_SYS_SINGLE
/*****************************************************************************/
//     Description : to handle window message
//    Global resource dependence : 
//  Author:wancan.you
//    Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  ConnectionSelectSimCallback(uint32 dual_sys, BOOLEAN isReturnOK, DPARAM param);

#endif

/*****************************************************************************/
//     Description : Handle SIM Plug Notify Connection
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
LOCAL void HandleSIMPlugNotifyConnectionFunc(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E notify_event, uint32 param);

#endif

/*****************************************************************************/
// 	Description : insert dynamenu node by label
//	Global resource dependence : 
//  Author:fen.xie
//	Note:
/*****************************************************************************/
LOCAL void InsertNodeByLabel(
                    MMI_CTRL_ID_T      ctrl_id,        //控件id
                    MMI_TEXT_ID_T   	text_id,       //插入的节点的文本
                    uint32          	node_id,        //节点编号
                    uint32          	parent_node_id, //父节点编号
                    uint16          	index,           //位置
                    MMI_IMAGE_ID_T      select_icon_id, //item selected icon id
                    BOOLEAN    is_grayed   //is_grayed
                    );

#ifdef MMI_GPRS_SUPPORT

/*****************************************************************************/
//  Description :Parse Network Account Info
//  Global resource dependence : none
//  Author:dave.ruan
//  Note:   is_tcard_file: Tcard file OR resource
/*****************************************************************************/
LOCAL BOOLEAN ParseNetworkAccountInfo(MMICONNECTION_RES_INFO_T *res_info_ptr, MMIRES_DATA_INFO_T file_info);

/*****************************************************************************/
//  Description : Load NetAccount Info From Resource
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN LoadNetworkAccountInfoFromRes(MMICONNECTION_RES_INFO_T *res_info_ptr);

/*****************************************************************************/
//  Description : Load NetAccount Info From Table List
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN LoadNetworkAccountInfoFromTableList(MMICONNECTION_RES_INFO_T *res_info_ptr);

/*****************************************************************************/
//  Description : Load NetAccount Info
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN LoadNetworkAccountInfo(MMICONNECTION_RES_INFO_T *res_info_ptr);

/*****************************************************************************/
//  Description : Unload NetAccount Info
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN UnloadNetworkAccountInfo(MMICONNECTION_RES_INFO_T *res_info_ptr);

/*****************************************************************************/
//  Description : Save NetAccount Info
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN SaveNetworkAccountInfo(MMICONNECTION_LINKSETTING_DETAIL_T *link_detail_ptr,//IN/OUT
                                                                            MMICONNECTION_RES_ITEM_T *res_item_ptr,//IN
                                                                            BOOLEAN is_customed_detail,//IN
                                                                            BOOLEAN is_default_unchange,
                                                                            uint16 list_index);//IN



#ifndef MMI_GUI_STYLE_TYPICAL
/*****************************************************************************/
//  Description :Init pda connection main list
//  Global resource dependence : 
//  Author: rong.gu 
//  Note: 
/*****************************************************************************/
LOCAL void InitPdaConnectionMainListCtrl(void);

/*****************************************************************************/
// 	Description : Setting link menu
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL void  InitPdaSettingLinkListCtrl(MMICONNECTION_LINKSETTING_DETAIL_T* setting_detail);

/*****************************************************************************/
// 	Description : Setting advance ctrl
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL void  InitPdaSettingAdvanceCtrl(MMICONNECTION_LINKSETTING_DETAIL_T* setting_detail);

/*****************************************************************************/
// 	Description : Setting link menu long press list item
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleSettingListLongOKMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param);

/*****************************************************************************/
// 	Description : Connection long ok menu,for mms,browser,dcd,java..
//	Global resource dependence : 
//  Author: jixin.xu
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleLinkListLongOKMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param);

									 
#endif /* MMI_GUI_STYLE_TYPICAL */

/*****************************************************************************/
//  Description :setting link list choose win handle fun
//  Global resource dependence : 
//  Author:jixin.xu
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleWapSettingListWinMsg(
                                         MMI_WIN_ID_T win_id, 
                                         MMI_MESSAGE_ID_E msg_id, 
                                         DPARAM param);
//////////////////////////////////////////////////////////////////////////
// added by feng.xiao
#ifdef DATA_ROAMING_SUPPORT
/*****************************************************************************/
// 	Description : handle data roaming query window
//	Global resource dependence : none
//  Author: feng.xiao
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDataRoamingQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
/*****************************************************************************/
// 	Description : handle data roaming cut off query window
//	Global resource dependence : none
//  Author: feng.xiao
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDataRoamingCutoffQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
#endif
#endif

/*****************************************************************************/
//  Description : Select SIM API of Connection app
//  Global resource dependence : 
//  Author:sam.hua
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E MMICONNECTION_SelectSIMFunc(void);

#ifdef DATA_ACCOUNT_IN_TCARD_SUPPORT
#if defined(BROWSER_SUPPORT)
/*****************************************************************************/
// 	Description : CheckNetworkAccountResFileToDownload
//	Global resource dependence : none
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL void CheckNetworkAccountResFileToDownload(void);

/*****************************************************************************/
// 	Description : HandleReplaceExistFileQueryWinMsg
//	Global resource dependence : none
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleReplaceExistFileQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*******************************************************************/
//  Interface:		HandleNetworkAccountDownloadCB
//  Description : 	Handle Network Account DownloadCB
//  Global resource dependence : 
//  Author: dave.ruan
//  Note:
/*******************************************************************/
LOCAL void HandleNetworkAccountDownloadCB(
								   MMIBROWSER_MIME_CALLBACK_PARAM_T *param_ptr
								   );
#endif

/*******************************************************************/
//  Interface:		MoveFileToNetworkAccount
//  Description : 	网络下载的数据优先存储在T卡,相同文件名可以覆盖。
//  Global resource dependence : 
//  Author: dave.ruan
//  Note:
/*******************************************************************/
LOCAL void MoveFileToNetworkAccount(const wchar *file_path);

/*****************************************************************************/
//  Description : get location file dir
//  Global resource dependence : 
//  Author:dave.ruan
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetValidNetAccountFileDir(uint16* device_dir,const wchar *file_name_ptr,uint32 file_size);

/*****************************************************************************/
// 	Description : handle refrsh setting list query window
//	Global resource dependence : none
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleRefreshSettingListQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

#endif

/*****************************************************************************/
// 	Description : Get GPRS Mode Settings.
//	Global resource dependence : none
//    Author: 
//	Note:
/*****************************************************************************/
LOCAL MMICONNECTION_GPRS_SETTINGS_E MMICONNECTION_GetGPRSModeStatus(void);

#ifdef MMI_GPRS_SWITCH_SUPPORT

/*****************************************************************************/
//  Description : Turn On GRPS
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIAPICONNECTION_TurnOffGPRS(void);
/*****************************************************************************/
//  Description : Turn off GPRS
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIAPICONNECTION_TurnOnGPRS(void);

#if defined MMI_GUI_STYLE_TYPICAL
/*****************************************************************************/
//  Description : GPRS switch win handler
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleGprsSwitchWinMsg(MMI_WIN_ID_T win_id, 
                                          MMI_MESSAGE_ID_E msg_id,
                                          DPARAM param);
#endif
#endif
/**--------------------------------------------------------------------------*
**                         WINDOW  DEFINITION                               *
**--------------------------------------------------------------------------*/
#ifdef MMI_GPRS_SUPPORT
WINDOW_TABLE( MMICONNECTION_LINKLIST_WIN_TAB ) = 
{
    WIN_FUNC((uint32)HandleWapSettingListWinMsg),
    WIN_ID(MMICONNECTION_LINKLIST_WIN_ID),
    WIN_STYLE( WS_HAS_SOFTKEY),
    WIN_TITLE(TXT_COMMON_WWW_ACCOUNT),   
    WIN_SOFTKEY(STXT_OPTION, TXT_NULL, STXT_RETURN),//STXT_OPTION
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMICONNECTION_LIST_SETTING_CTRL_ID),
    END_WIN
};
#endif

#ifdef MMI_GUI_STYLE_TYPICAL
WINDOW_TABLE(MMICONNECTION_MAIN_MENU_WIN_TAB) = 
{
    WIN_FUNC( (uint32)HandleConnectionMenuWinMsg),
    WIN_ID( MMICONNECTION_MENU_WIN_ID ),
    WIN_TITLE(TXT_SETTINGS_LINK),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
    END_WIN
};
#else
WINDOW_TABLE(MMICONNECTION_MAIN_MENU_WIN_TAB) = 
{
    WIN_FUNC( (uint32)HandleConnectionMenuWinMsg),
    WIN_ID( MMICONNECTION_MENU_WIN_ID ),
    WIN_TITLE(TXT_SETTINGS_LINK),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMICONNECTION_MENU_CTRL_ID),   
#ifndef MMI_PDA_SUPPORT
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
#endif /* MMI_PDA_SUPPORT */
    END_WIN
};
#endif /* MMI_GUI_STYLE_TYPICAL */

#ifdef MMI_GPRS_SUPPORT

#ifdef MMI_PDA_SUPPORT
WINDOW_TABLE(MMICONNECTION_SETTINGLINKLIST_LONGOKMENU_WIN_TAB)=
{
    WIN_FUNC( (uint32)HandleSettingListLongOKMenuWinMsg),
    WIN_ID(MMICONNECTION_SETTINGLINKLISTLONGOKMENU_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    CREATE_POPMENU_CTRL(MENU_CONNECTION_LONGOK_LINKLIST,MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID),
    END_WIN
};

WINDOW_TABLE(MMICONNECTION_LINKLIST_LONGOKMENU_WIN_TAB)=
{
    WIN_FUNC( (uint32)HandleLinkListLongOKMenuWinMsg),
    WIN_ID(MMICONNECTION_LINKLISTLONGOKMENU_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    CREATE_POPMENU_CTRL(MENU_CONNECTION_LINKLIST_LONGOK,MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID),
    END_WIN
};
#endif

WINDOW_TABLE(MMICONNECTION_SETTINGLINKLISTMENU_WIN_TAB)=
{
    WIN_FUNC( (uint32)HandleSettingLinkListMenuWinMsg),
    WIN_ID(MMICONNECTION_SETTINGLINKLISTMENU_WIN_ID),
    WIN_STYLE( WS_HAS_TRANSPARENT),
    CREATE_POPMENU_CTRL(MENU_CONNECTION_SETTING_LINKLIST,MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK,TXT_NULL,STXT_RETURN),
    END_WIN
}; 


//the window for connect setting list
WINDOW_TABLE(MMICONNECTION_SETTINGLIST_WIN_TAB) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC((uint32)HandleSettingListWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLIST_WIN_ID),
    WIN_TITLE(TXT_COMMON_WWW_ACCOUNT),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMICONNECTION_LIST_SETTING_CTRL_ID),
    END_WIN
};

WINDOW_TABLE(MMICONNECTION_SEARCHLINKLIST_WIN_TAB) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC((uint32)HandleSearchResultLinkListWinMsg),    
    WIN_ID(MMICONNECTION_SEARCHLINKLIST_WIN_ID),
    WIN_TITLE(TXT_CONNECTION_LINKLIST),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E,MMICONNECTION_LIST_SEARCHLINKLIST_CTRL_ID),
    END_WIN
};

//GPRS Settings
WINDOW_TABLE( MMICONNECTION_GPRS_SETTINGS_WIN_TAB ) = 
{
    WIN_TITLE( TXT_DATA_CONNECTION_SET),
    WIN_FUNC((uint32) HandleGPRSSettingsWinMsg),    
    WIN_ID( MMICONNECTION_GPRS_SETTINGS_WIN_ID ),
    #ifdef MMI_PDA_SUPPORT
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
    #else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    #endif
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMICONNECTION_GPRS_SETTINGS_CTRL_ID ),
	END_WIN
};

//the window for connect link setting list
WINDOW_TABLE(MMICONNECTION_ADDSETTINGLIST_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleAddSettingListWinMsg),    
    WIN_ID(MMICONNECTION_ADDSETTINGLIST_WIN_ID),
    WIN_TITLE(TXT_COMMON_WWW_ACCOUNT),
    WIN_SOFTKEY(TXT_COMMON_OK, STXT_SELECT, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_CHECKLIST_E,MMICONNECTION_LIST_SETTINGADDLIST_CTRL_ID),
    END_WIN
};

#if !defined MMI_GUI_STYLE_TYPICAL
//the window for setting menu
WINDOW_TABLE(MMICONNECTION_SETTINGLINKMENU_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingLinkMenuWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLINKMENU_WIN_ID),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
#endif
    WIN_TITLE(TXT_NULL),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMICONNECTION_MENU_SETTINGLINK_CTRL_ID),    
    END_WIN
};
#else
//the window for setting menu
WINDOW_TABLE(MMICONNECTION_SETTINGLINKMENU_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingLinkMenuWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLINKMENU_WIN_ID),
    WIN_TITLE(TXT_NULL),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_MENU_CTRL(MENU_CONNECTION_SETTING_LINK, MMICONNECTION_MENU_SETTINGLINK_CTRL_ID),
    END_WIN
};
#endif

WINDOW_TABLE(MMICONNECTION_SETTINGLINKDETAILVIEW_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingLinkDetailViewWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLINKDETAILVIEW_WIN_ID),
    WIN_TITLE(TXT_COMMON_LINKSETTING),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_RICHTEXT_CTRL(MMICONNECTION_RICHTEXT_SETTINGLINKDETAILVIEW_CTRL_ID),
    END_WIN
};

//the window for security connect setting list
WINDOW_TABLE(MMICONNECTION_SETTINGSECURITYCONNECTLIST_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingSecurityconnectListWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGSECURITYCONNECTLIST_WIN_ID),
    WIN_TITLE(TXT_CONNECTION_AUTH_TYPE),
#ifdef MMI_PDA_SUPPORT
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
#else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#endif
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMICONNECTION_LIST_SETTINGSECURITYCONNECT_CTRL_ID),
    END_WIN
};  

//the window for input user name
WINDOW_TABLE(MMICONNECTION_INPUTUSERNAME_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleInputUserNameWinMsg),    
    WIN_ID(MMICONNECTION_INPUTUSERNAME_WIN_ID),
    WIN_TITLE(TXT_COMMON_USERNAME),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //参数为测试用
    CREATE_EDIT_TEXT_CTRL(MMICONNECTION_MAX_USERNAME_LEN ,MMICONNECTION_EDIT_INPUTUSERNAME_CTRL_ID),
    END_WIN
};

//the window for input password
WINDOW_TABLE(MMICONNECTION_INPUTPASSWORD_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleInputPasswordWinMsg),    
    WIN_ID(MMICONNECTION_INPUTPASSWORD_WIN_ID),
    WIN_TITLE(TXT_COMMON_PASSWORD),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //参数为测试用
    CREATE_EDIT_PASSWORD_CTRL(MMICONNECTION_MAX_PASSWORD_LEN ,MMICONNECTION_EDIT_INPUTPASSWORD_CTRL_ID),
    END_WIN
};

//the window for input apn
WINDOW_TABLE(MMICONNECTION_INPUTAPN_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleInputApnWinMsg),    
    WIN_ID(MMICONNECTION_INPUTAPN_WIN_ID),
    WIN_TITLE(TXT_CONNECTION_APN),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //参数为测试用
    CREATE_EDIT_TEXT_CTRL(MMICONNECTION_MAX_APN_LEN ,MMICONNECTION_EDIT_INPUTAPN_CTRL_ID),
    END_WIN
};


//the window for input gateway
WINDOW_TABLE(MMICONNECTION_INPUTGATEWAY_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleInputGatewayWinMsg),    
    WIN_ID(MMICONNECTION_INPUTGATEWAY_WIN_ID),
    WIN_TITLE(TXT_COMMON_GATEWAY),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //参数为测试用

#ifdef MMI_HTTP_PROXY_IP_ONLY_SUPPORT    
    CREATE_EDIT_IP_CTRL(MMICONNECTION_EDIT_INPUTGATEWAY_CTRL_ID),
#else
    CREATE_EDIT_TEXT_CTRL(MMICONNECTION_MAX_GATEWAY_LEN, MMICONNECTION_EDIT_INPUTGATEWAY_CTRL_ID),
#endif
    END_WIN
};


//the window for port list
WINDOW_TABLE(MMICONNECTION_PORTLIST_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandlePortListWinMsg),    
    WIN_ID(MMICONNECTION_EDITPORT_WIN_ID),
    WIN_TITLE(TXT_CONNECTION_PORT),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_EDIT_DIGITAL_CTRL(MMICONNECTION_MAX_PORT_LEN,MMICONNECTION_EDIT_INPUTPORT_CTRL_ID),
    END_WIN
};

//the window for input agent name
WINDOW_TABLE(MMICONNECTION_INPUTAGENTNAME_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleInputAgentNameWinMsg),    
    WIN_ID(MMICONNECTION_INPUTAGENTNAME_WIN_ID),
    WIN_TITLE(TXT_CONNECTION_CHANGENAME),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //参数为测试用
    CREATE_EDIT_TEXT_CTRL(MMICONNECTION_MAX_LINKMODENAME_LEN ,MMICONNECTION_EDIT_INPUTAGENTNAME_CTRL_ID),
    END_WIN
};


//the window for input homepage
WINDOW_TABLE(MMICONNECTION_INPUTHOMEPAGE_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleInputHomepageWinMsg),    
    WIN_ID(MMICONNECTION_INPUTHOMEPAGE_WIN_ID),
     WIN_TITLE(TXT_COMMON_HOMEPAGE),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //参数为测试用
    CREATE_EDIT_TEXT_CTRL(MMICONNECTION_HOMEPAGE_MAX_URL_LEN ,MMICONNECTION_EDIT_INPUTHOMEPAGE_CTRL_ID),
    END_WIN
};


#if !defined MMI_GUI_STYLE_TYPICAL
//the window for setting link dns
WINDOW_TABLE(MMICONNECTION_SETTINGLINK_ADVANCE_WIN_TAB) = 
{
    WIN_FUNC( (uint32)HandleSettingLinkAdvanceWinMsg),
    WIN_ID( MMICONNECTION_SETTINGLINK_ADVANCE_WIN_ID ),
    WIN_TITLE(TXT_CONNECTION_ADVANCE),
#ifdef MMI_PDA_SUPPORT
    WIN_STYLE(WS_HAS_BUTTON_SOFTKEY),
#endif    
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID),    
    END_WIN
};
#else
//the window for setting link dns
WINDOW_TABLE(MMICONNECTION_SETTINGLINK_ADVANCE_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingLinkAdvanceWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLINK_ADVANCE_WIN_ID),
    WIN_TITLE(TXT_NULL),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_MENU_CTRL(MENU_CONNECTION_SETTING_LINK_ADVANCE, MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID),					
    END_WIN
};
#endif

//the window for setting link dns
WINDOW_TABLE(MMICONNECTION_SETTINGLINK_DNS_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingLinkDNSWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLINK_DNS_WIN_ID),
    WIN_TITLE(TXT_CONNECTION_DNS),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_EDIT_IP_CTRL(MMICONNECTION_EDIT_INPUT_DNS_CTRL_ID),
    END_WIN
};

//the window for setting net type
WINDOW_TABLE(MMICONNECTION_SETTINGLINK_NETTYPE_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleSettingLinkNetTypeWinMsg),    
    WIN_ID(MMICONNECTION_SETTINGLINK_NETTYPE_WIN_ID),
    WIN_TITLE(TXT_NETWORK_TYPE),
#ifdef MMI_PDA_SUPPORT
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),
#else
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
#endif
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMICONNECTION_LIST_SELECT_NETTYPE_CTRL_ID),
    END_WIN
};

#ifndef MMI_MULTI_SIM_SYS_SINGLE
/*		//Sam.hua	use SelectSimFunc 
WINDOW_TABLE(MMICONNECTION_SELECT_SIM_WIN_TAB) = 
{
    WIN_TITLE(TXT_SIM_SEL),
    WIN_FUNC((uint32) HandleConnectionSelectSimWinMsg),    
    WIN_ID( MMICONNECTION_SELECT_SIM_WIN_ID ),
    WIN_SOFTKEY(STXT_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMICONNECTION_SELECT_SIM_CTRL_ID),

    END_WIN
};
*/
#endif
#endif

#ifdef MMI_GPRS_SWITCH_SUPPORT      
#if defined MMI_GUI_STYLE_TYPICAL
WINDOW_TABLE(MMICONNECTION_GPRS_SWITCH_WIN_TAB) = 
{
    WIN_FUNC((uint32)HandleGprsSwitchWinMsg),    
    WIN_ID(MMICONNECTION_GPRS_SWITCH_WIN_ID),
    WIN_TITLE(TXT_GPRS),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID),
    END_WIN
};

#endif
#endif


/*---------------------------------------------------------------------------*/
/*                          FUNCTION DEFINITION                              */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description :append one line icon text list item
//  Global resource dependence : none
//  Author: James.Zhang
//  Note: 
/*****************************************************************************/
LOCAL void AppendOneLineTextListItem(
                                     MMI_CTRL_ID_T       ctrl_id,
                                     GUIITEM_STYLE_E    item_style,
                                     MMI_TEXT_ID_T       left_softkey_id,
                                     MMI_IMAGE_ID_T      icon_id,
                                     wchar*        wstr_ptr,
                                     uint16              wstr_len,
                                     uint16              pos,
                                     BOOLEAN             is_update
                                     )
{
    GUILIST_ITEM_T      item_t = {0};/*lint !e64*/
    GUILIST_ITEM_DATA_T item_data = {0};/*lint !e64*/
    uint16                  i         = 0;
    
    item_t.item_style    = item_style;
    item_t.item_data_ptr = &item_data;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = TXT_NULL;
    item_data.softkey_id[2] = STXT_RETURN;
    
    if( 0 != icon_id )
    {
        item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = icon_id;
        i++;
    }
    
    item_data.item_content[i].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[i].item_data.text_buffer.wstr_len = wstr_len;
    
    item_data.item_content[i].item_data.text_buffer.wstr_ptr = wstr_ptr;
    
    if( is_update )
    {
        GUILIST_ReplaceItem( ctrl_id, &item_t, pos );
    }
    else
    {
        GUILIST_AppendItem( ctrl_id, &item_t );
    }
}


/*****************************************************************************/
//  Description :append one line check list item, which has three softkey, and mid softkey is update dynamic
//  Global resource dependence : none
//  Author: bei.wu
//  Note: 
/*****************************************************************************/
LOCAL void AppendOneLineCheckListItem(
                                        MMI_CTRL_ID_T       ctrl_id,            //id of list control
                                        GUIITEM_STYLE_E     item_style,          //item style
                                        MMI_TEXT_ID_T       left_softkey_id,    //left softkey id
                                        MMI_TEXT_ID_T       middle_softkey_id,  //middle softkey id
                                        MMI_IMAGE_ID_T      icon_id,            //icon id, default is null
                                        wchar*        wstr_ptr,           //content string, unicode
                                        uint16              wstr_len,           //content string len
                                        uint16              pos,                //the item index of in list
                                        BOOLEAN             is_update           //need to updata this item or not
                                     )
{
    GUILIST_ITEM_T      item_t = {0};/*lint !e64*/
    GUILIST_ITEM_DATA_T item_data = {0};/*lint !e64*/
    uint16                  i         = 0;
    
    item_t.item_style    = item_style;
    item_t.item_data_ptr = &item_data;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = middle_softkey_id;
    item_data.softkey_id[2] = STXT_RETURN;
    
    if( 0 != icon_id )
    {
        item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = icon_id;
        i++;
    }
    
    item_data.item_content[i].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[i].item_data.text_buffer.wstr_len = wstr_len;

    item_data.item_content[i].item_data.text_buffer.wstr_ptr = wstr_ptr;
    
    if( is_update )
    {
        GUILIST_ReplaceItem( ctrl_id, &item_t, pos );
    }
    else
    {
        GUILIST_AppendItem( ctrl_id, &item_t );
    }
}

/*****************************************************************************/
//  Description :
//  Global resource dependence : none
//  Author: 
//  Note: 
/*****************************************************************************/
LOCAL void ChangeAppSelectedIndex(MN_DUAL_SYS_E dual_sys, uint16 list_index, MMICONNECTION_NOTIFY_EVENT_E event)
{
    //调用各个APP应用提供的函数
    MMICONNECTION_NOTIFY_LIST_T *cursor = PNULL;

    cursor = s_subscriber_list_ptr;
    while (PNULL != cursor)
    {
        cursor->notify_info.notify_func(event, list_index, dual_sys);
        cursor = cursor->next;
    }
}

/****************************************************************************/	
// 	Description : translate the setting item to richtext item, and add it to richtext control
//	Global resource dependence : none
//  Author: bei.wu
//	Note: 
/*****************************************************************************/
LOCAL void SetSettingDetailEntry(MMI_CTRL_ID_T ctrl_id, MMICONNECTION_LINKSETTING_DETAIL_T* item)
{
    GUIRICHTEXT_ITEM_T  item_data = {0};/*lint !e64*/
    uint16 index = 0;
    MMI_STRING_T  dststring={0};
#ifndef MMI_DUALMODE_ENABLE
    CONNECTION_MMI_TEXT_ID_E  text_id[2] = {TXT_COMMON_COMMON_NORMAL, TXT_COMM_SECURITY};/*lint !e64*/
#endif
    wchar ucs2buf[MMICONNECTION_HOMEPAGE_MAX_URL_LEN+1] = {0};
    uint8 port_str[MMICONNECTION_MAX_PORT_LEN+2] = {0};
    uint32 port_str_len = 0;
    MMI_STRING_T        colon ={0};
    wchar colon_char[2] = {':', 0};
    
    item_data.img_type = GUIRICHTEXT_IMAGE_NONE;
    item_data.text_type = GUIRICHTEXT_TEXT_BUF;

    colon.wstr_ptr = colon_char;
    colon.wstr_len = 1;
#ifndef MMI_DUALMODE_ENABLE
    //是否加密
    MMIAPICOM_CatIDAndString(&dststring,TXT_CONNECTION_AUTH_TYPE, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    MMI_GetLabelTextByLang(text_id[item->auth_type],&dststring);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
#endif
    
    //用户名
    MMIAPICOM_CatIDAndString(&dststring, TXT_COMMON_USERNAME, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    MMIAPICOM_StrToWstr((uint8 *)item->username, ucs2buf);
    item_data.text_data.buf.len = item->username_len;//MIN(item->username_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = ucs2buf;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    //密码
    MMIAPICOM_CatIDAndString(&dststring, TXT_COMMON_PASSWORD, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    MMIAPICOM_StrToWstr((uint8 *)item->password, ucs2buf);
    item_data.text_data.buf.len = item->password_len;//MIN(item->password_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = ucs2buf;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    //接入点
    MMIAPICOM_CatIDAndString(&dststring, TXT_CONNECTION_APN, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    MMIAPICOM_StrToWstr((uint8 *)item->apn, ucs2buf);
    item_data.text_data.buf.len = item->apn_len;//MIN(item->apn_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = ucs2buf;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    //网关
    MMIAPICOM_CatIDAndString(&dststring, TXT_COMMON_GATEWAY, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    MMIAPICOM_StrToWstr((uint8 *)item->gateway, ucs2buf);
    item_data.text_data.buf.len = item->gateway_len;//MIN(item->gateway_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = ucs2buf;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    //端口
    MMIAPICOM_CatIDAndString(&dststring, TXT_CONNECTION_PORT, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    SCI_MEMSET(ucs2buf, 0, sizeof(ucs2buf));
    _snprintf((char *)port_str, (MMICONNECTION_MAX_PORT_LEN+1),"%d", item->port);
    MMIAPICOM_StrToWstr((uint8 *)port_str, ucs2buf);
    port_str_len = strlen((char *)port_str);
    item_data.text_data.buf.len = MIN(port_str_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = ucs2buf;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    
    //连接设置名称
    MMIAPICOM_CatIDAndString(&dststring, TXT_CONNECTION_CHANGENAME, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    item_data.text_data.buf.len = item->name_len;//MIN(item->name_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = item->name;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //主页或者彩信中心
    MMIAPICOM_CatIDAndString(&dststring, TXT_COMMON_HOMEPAGE, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }
    
    SCI_MEMSET(ucs2buf, 0, sizeof(ucs2buf));
    MMIAPICOM_StrToWstr((uint8 *)item->homepage, ucs2buf);
    dststring.wstr_ptr = ucs2buf;
    dststring.wstr_len = item->homepage_len;
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //网络类型
    MMIAPICOM_CatIDAndString(&dststring, TXT_NETWORK_TYPE, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }

    if(item->access_type)
    {
        MMI_GetLabelTextByLang(TXT_CONNECTION_NETTYPE_HTTP, &dststring);
    }
    else
    {
        MMI_GetLabelTextByLang(TXT_CONNECTION_NETTYPE_WAP, &dststring);
    }
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);

    //DNS
    MMIAPICOM_CatIDAndString(&dststring, TXT_CONNECTION_DNS, &colon);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
    if(PNULL != dststring.wstr_ptr)
    {
        SCI_FREE(dststring.wstr_ptr);
        dststring.wstr_ptr = PNULL;
    }

    SCI_MEMSET(ucs2buf, 0, sizeof(ucs2buf));
    MMIAPICOM_StrToWstr((uint8 *)item->dns, ucs2buf);
    dststring.wstr_ptr = ucs2buf;
    dststring.wstr_len = SCI_STRLEN((char *)item->dns);
    item_data.text_data.buf.len = MIN(dststring.wstr_len, GUIRICHTEXT_TEXT_MAX_LEN);
    item_data.text_data.buf.str_ptr = dststring.wstr_ptr;
    GUIRICHTEXT_AddItem(ctrl_id, &item_data, &index);
}

#ifndef MMI_GUI_STYLE_TYPICAL
/*****************************************************************************/
// 	Description : 1 line list
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL void AppendPdaConnectionItem1Line(
                                 MMI_CTRL_ID_T  ctrl_id,    //控件ID
                                 MMI_TEXT_ID_T  text_1,     //主文本
                                 uint32         user_data   //用户数据
                                 )
{
    GUILIST_ITEM_T      item_t = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    
    item_t.item_style    = GUIITEM_STYLE_ONE_LINE_TEXT_ICON;
    item_t.item_data_ptr = &item_data;
    item_t.user_data     = user_data;
    
    //text 1
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_ID;
    item_data.item_content[0].item_data.text_id = text_1;

    GUILIST_AppendItem(ctrl_id, &item_t);
}

/*****************************************************************************/
//  Description : 2 line list
//  Global resource dependence : 
//  Author: rong.gu 
//  Note: 
/*****************************************************************************/
LOCAL void AppendPdaConnectionItem2Line(
                                 MMI_CTRL_ID_T  ctrl_id,    //控件ID
                                 MMI_TEXT_ID_T  text_1,     //主文本
                                 MMI_STRING_T  text_str,     //副文本
                                 MMI_IMAGE_ID_T image_id,   //图标ID
                                 uint32         user_data   //用户数据
                                 )
{
    GUILIST_ITEM_T      item_t = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    wchar temp_wstr[MMICONNECT_STRNUM_MAX_LEN + 2] = {0};
    wchar temp_len = 0;
    if(text_str.wstr_ptr != PNULL && text_str.wstr_len != 0)
    {
        SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
        MMIAPICOM_Wstrncpy(temp_wstr,
            text_str.wstr_ptr,
            text_str.wstr_len);
        temp_len = text_str.wstr_len;
        item_t.item_style    = GUIITEM_STYLE_TWO_LINE_TEXT_AND_TEXT_ANIM;
    }
    else
    {
        item_t.item_style    = GUIITEM_STYLE_ONE_LINE_TEXT_ICON;
    }
    
    item_t.item_data_ptr = &item_data;
    item_t.user_data     = user_data;
    
    //text 1
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_ID;
    item_data.item_content[0].item_data.text_id = text_1;

    //text 2    
    if(text_str.wstr_ptr != PNULL && text_str.wstr_len != 0)
    {
        item_data.item_content[2].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[2].item_data.text_buffer.wstr_ptr = temp_wstr;
        item_data.item_content[2].item_data.text_buffer.wstr_len = temp_len;
    }
    
    //anim
    item_data.item_content[1].item_data_type    = GUIITEM_DATA_IMAGE_ID;
    item_data.item_content[1].item_data.image_id = image_id;
    
    GUILIST_AppendItem(ctrl_id, &item_t);
}


/*****************************************************************************/
// 	Description : Connection menu
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL void  InitPdaConnectionMainListCtrl(void)
{
    MMI_CTRL_ID_T       ctrl_id = MMICONNECTION_MENU_CTRL_ID;
    MMI_STRING_T text_str= {0};
    MMICONNECTION_GPRS_SETTINGS_E gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
    MMI_TEXT_ID_T    text_id = TXT_NULL;
    uint16 dual_sys = MN_DUAL_SYS_1;
    uint32 sim_num                        = 0;
    MN_PHONE_SELECT_PLMN_T  sel_net_mode    =   {MN_PHONE_PLMN_SELECT_MANUAL_MODE, 0};/*lint !e785*/
    uint16 index = 0;
    uint16 cur_index = 0;
    uint16 topItemIndex = 0;
#ifdef MMI_GPRS_SWITCH_SUPPORT
    MMICONNECTION_SETTING_GPRS_SWITCH_E gprs_switch_status = MMICONNECTION_SETTING_GPRS_SWITCH_ON;
#endif
#ifdef DATA_ROAMING_SUPPORT
    MMICONNECTION_SETTING_DATA_ROAMING_E data_roaming_status = MMAPIICONNECTION_Setting_GetDataRoamingStatus();
    MMI_IMAGE_ID_T image_id = NULL;
#endif
#ifdef MMI_GPRS_SWITCH_SUPPORT
	MMI_IMAGE_ID_T	image_switch_id = NULL;
#endif
    cur_index = GUILIST_GetCurItemIndex(ctrl_id);
    topItemIndex = GUILIST_GetTopItemIndex(ctrl_id);


    GUILIST_SetMaxItem(ctrl_id, MMI_CONNECTION_MENU_LIST_TYPE_MAX, FALSE);

#ifdef MMI_GPRS_SUPPORT
    AppendPdaConnectionItem1Line(            
    ctrl_id,
    TXT_COMMON_WWW_ACCOUNT,
    (uint32)MMI_CONNECTION_MENU_LIST_GPRS_NET_USERID
    );

    index++;

#ifdef MMI_GPRS_SWITCH_SUPPORT
    gprs_switch_status = MMIAPICONNECTION_GetGPRSSwitchStatus();
    if(gprs_switch_status == MMICONNECTION_SETTING_GPRS_SWITCH_ON)
    {
        image_switch_id = IMAGE_CHECK_SELECTED_ICON;
    }
    else
    {      
        image_switch_id = IMAGE_CHECK_UNSELECTED_ICON;
    }        
    SCI_MEMSET(&text_str,0,sizeof(MMI_STRING_T));

    AppendPdaConnectionItem2Line(ctrl_id,
                                 TXT_GPRS_SERVICE,
                                 text_str,
                                 image_switch_id,
                                 MMI_CONNECTION_MENU_LIST_GPRS_SWITCH);    
    index++;                                 
#endif
    
    
//////////////////////////////////////////////////////////////////////////
// added by feng.xiao
#ifdef DATA_ROAMING_SUPPORT
    if(data_roaming_status == MMICONNECTION_SETTING_DATA_ROAMING_ENABLE)
    {
        image_id = IMAGE_CHECK_SELECTED_ICON;
    }
    else
    {      
        image_id = IMAGE_CHECK_UNSELECTED_ICON;
    }        
    text_id = TXT_CONNECTION_DATA_ROAMING_TIP;
    MMI_GetLabelTextByLang(text_id, &text_str);
    AppendPdaConnectionItem2Line(ctrl_id,
                                 TXT_CONNECTION_DATA_ROAMING,
                                 text_str,
                                 image_id,
                                 MMI_CONNECTION_MENU_LIST_GPRS_ROAMING);

    index++;
    
#endif
    gprs_settings = MMICONNECTION_GetGPRSModeStatus();
    switch(gprs_settings)
    {
        case MMICONNECTION_GPRS_ATTACH_ONNEED:
            text_id = TXT_CONNECTION_GPRS_DETACH;
            break;
            
        case MMICONNECTION_GPRS_ALWAYS:
            text_id = TXT_CONNECTION_GPRS_ALWAYS;
            break;
            
#ifdef MMI_DUALMODE_ENABLE
        case MMICONNECTION_GPRS_ALWAYS_LC:
            text_id = TXT_ENERGY_SAVING;
            break;
#endif
        default:
            break;
    }
    
    MMI_GetLabelTextByLang(text_id, &text_str);
    AppendPdaConnectionItem2Line(
    ctrl_id, 
    TXT_DATA_CONNECTION_SET,
    text_str,
    IMAGE_COMMON_RIGHT_ARROW,
    (uint32)MMI_CONNECTION_MENU_LIST_GPRS_MODESET
    );


#ifdef MMI_GPRS_SWITCH_SUPPORT
    if(gprs_switch_status == MMICONNECTION_SETTING_GPRS_SWITCH_OFF)
    {
        GUILIST_SetItemGray(ctrl_id, index, TRUE);
    }
    else
#endif
    {
        sim_num = MMIAPIPHONE_GetSimAvailableNum(PNULL, 0);
        if (1 < sim_num)
        {
            GUILIST_SetItemGray(ctrl_id, index, TRUE);
        }
        else
        {
            GUILIST_SetItemGray(ctrl_id, index, FALSE);
        }
    }
    index++;
#endif

    SCI_MEMSET(&text_str, 0x00, sizeof(text_str));

#ifdef  MMI_DUALMODE_ENABLE
    MNPHONE_GetPlmnSelectModeEx(MN_DUAL_SYS_1, &sel_net_mode);    
    switch (sel_net_mode.select_mode)
    {
    case MN_PHONE_PLMN_SELECT_AUTO_MODE:
        text_id = TXT_COMMON_AUTO_SELECT;
        break;
        
    case MN_PHONE_PLMN_SELECT_MANUAL_MODE:
        text_id = TXT_SET_MANUAL_SEL;
        break;
        
    default:     
        text_id = TXT_NULL;
        break;
    }

    MMI_GetLabelTextByLang(text_id, &text_str);

    AppendPdaConnectionItem2Line(
    ctrl_id, 
    TXT_NETWORK_SELECT,
    text_str,
    IMAGE_COMMON_RIGHT_ARROW,
    (uint32)MMI_CONNECTION_MENU_LIST_NETSETTING
    );
    index++;
#else
    sim_num = MMIAPIPHONE_GetSimAvailableNum(&dual_sys, 1);    
    if(1 == sim_num )
    {
        MNPHONE_GetPlmnSelectModeEx((MN_DUAL_SYS_E)dual_sys, &sel_net_mode);
        switch (sel_net_mode.select_mode)
        {
        case MN_PHONE_PLMN_SELECT_AUTO_MODE:
            text_id = TXT_COMMON_AUTO_SELECT;
            break;
            
        case MN_PHONE_PLMN_SELECT_MANUAL_MODE:
            text_id = TXT_SET_MANUAL_SEL;
            break;
            
        default:     
            text_id = TXT_NULL;
            break;
        }

        MMI_GetLabelTextByLang(text_id, &text_str);    
        AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_NETWORK_SELECT,
        text_str,
        IMAGE_COMMON_RIGHT_ARROW,
        (uint32)MMI_CONNECTION_MENU_LIST_NETWORK
        );

        index++;
    }
    else
    {
        AppendPdaConnectionItem1Line(
        ctrl_id,
        TXT_NETWORK_SELECT,
        (uint32)MMI_CONNECTION_MENU_LIST_NETWORK
        );

        index++;
    }
#endif

#ifdef MMI_PREFER_PLMN_SUPPORT
    SCI_MEMSET(&text_str, 0x00, sizeof(text_str));

    AppendPdaConnectionItem2Line(
    ctrl_id, 
    TXT_SET_PREFER_NETWORK,
    text_str,
    IMAGE_COMMON_RIGHT_ARROW,
    (uint32)MMI_CONNECTION_MENU_LIST_GPRS_PREFER_NETWORK
    );

    index++;
#endif

    GUILIST_SetCurItemIndex(ctrl_id, cur_index);
    GUILIST_SetTopItemIndex(ctrl_id, topItemIndex);
}
#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
// 	Description : Connection long ok menu
//	Global resource dependence : 
//  Author: rong.gu
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleSettingListLongOKMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param)
{
   
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    uint32          group_id = 0, menu_id = 0;
    uint16           i = 0;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;
    uint16          list_index = 0;
    //SCI_TRACE_LOW:"SMS: HandleSmsChatMenuOptWinMsg, msg_id = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_1626_112_2_18_2_12_8_0,(uint8*)"d", msg_id);
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {                
            const GUILIST_ITEM_T *list_item = NULL;
            MMI_STRING_T title_text = {0};
            list_item = GUILIST_GetItemPtrByIndex(MMICONNECTION_LIST_SETTING_CTRL_ID, GUILIST_GetCurItemIndex( MMICONNECTION_LIST_SETTING_CTRL_ID));
            if(list_item && list_item->item_data_ptr)
            {
                title_text  = list_item->item_data_ptr->item_content[1].item_data.text_buffer;
            }
            GUIMENU_SetMenuTitle(&title_text, MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID);                
            MMK_SetAtvCtrl(win_id, MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID);            
        }
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:            
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_PENOK:  
        GUIMENU_GetId(MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID,&group_id,&menu_id);
        switch(menu_id)
        {
        case ID_CONNECTION_SETTING_DELETE:
            i = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);        
            if(!s_connection_setting_info.setting_detail[dual_sys][i].is_customed_detail)
            {
                 if (1 == s_connection_setting_info.settinglink_num[dual_sys])
                 {
                     MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_DELETE_FAIL);
                     MMK_CloseWin(win_id);//返回上一级
                 }
                 else
                 {
                     MMIPUB_OpenQueryWinByTextId(TXT_DELETE_QUERY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
                 }
            }
            break;
        
        default:
            break;
        }
        break;

    case MSG_PROMPTWIN_OK:                                    //delete
        #if 0
        if (1 == s_connection_setting_info.settinglink_num[dual_sys])
        {
            MMIPUB_CloseQuerytWin(PNULL);
            MMK_CloseWin(win_id);
            break;
        }
        #endif
        list_index = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);
        if((list_index < s_connection_setting_info.settinglink_num[dual_sys]) && (s_connection_setting_info.settinglink_num[dual_sys] > 0))                                       
        {
            for(i = (uint32)list_index; i < (uint32)(s_connection_setting_info.settinglink_num[dual_sys]-1); i++)
            {
                SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][i],&s_connection_setting_info.setting_detail[dual_sys][i+1],sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            }
            SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys] - 1],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            s_connection_setting_info.settinglink_num[dual_sys]--;
        }
        else if((list_index ==  s_connection_setting_info.settinglink_num[dual_sys]) && (s_connection_setting_info.settinglink_num[dual_sys] > 0))
        {
            SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys] - 1],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            s_connection_setting_info.settinglink_num[dual_sys]--;
        }
        MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO,&s_connection_setting_info);
        
        //调整应用选中的ITEM
        ChangeAppSelectedIndex(dual_sys, list_index, MMICONNECTION_NOTIFY_EVENT_DEL_ITEM);
#ifndef MMI_PDA_SUPPORT       
        MMIPUB_OpenAlertSuccessWin(TXT_DELETED);
#endif
        GUILIST_RemoveItem(MMICONNECTION_LIST_SETTING_CTRL_ID, list_index);
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;
        
    case MSG_PROMPTWIN_CANCEL:
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
   
    return recode;

}

/*****************************************************************************/
// 	Description : Connection long ok menu,for mms,browser,dcd,java..
//	Global resource dependence : 
//  Author: jixin.xu
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleLinkListLongOKMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param)
{
   
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    uint32          group_id = 0, menu_id = 0;
    uint16           i = 0;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;
    uint16          list_index = 0;

    //SCI_TRACE_LOW:"[MMICONNECTION]: HandleLinkListLongOKMenuWinMsg, msg_id = 0x%x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_1742_112_2_18_2_12_9_1,(uint8*)"d", msg_id);
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {                
            const GUILIST_ITEM_T *list_item = NULL;
            MMI_STRING_T title_text = {0};
            list_item = GUILIST_GetItemPtrByIndex(MMICONNECTION_LIST_SETTING_CTRL_ID, GUILIST_GetCurItemIndex( MMICONNECTION_LIST_SETTING_CTRL_ID));
            if(list_item && list_item->item_data_ptr)
            {
                title_text  = list_item->item_data_ptr->item_content[1].item_data.text_buffer;
            }
            GUIMENU_SetMenuTitle(&title_text, MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID);                
            MMK_SetAtvCtrl(win_id, MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID);            
        }
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:    
    case MSG_CTL_PENOK:  
        GUIMENU_GetId(MMICONNECTION_MENU_SETTINGLINK_LONGOK_CTRL_ID,&group_id,&menu_id);
        switch(menu_id)
        {
        case ID_CONNECTION_SETTING_DELETE:
            i = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);        
            if(!s_connection_setting_info.setting_detail[dual_sys][i].is_customed_detail)
            {
                if (1 == s_connection_setting_info.settinglink_num[dual_sys])
                {
                    MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_DELETE_FAIL);          
                    MMK_CloseWin(win_id);//返回上一级
                }
                else
                {
                    MMIPUB_OpenQueryWinByTextId(TXT_DELETE_QUERY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
                }                        
            }                    
            break;
            
        case ID_CONNECTION_SETTING_EDIT:                                                                           //编辑一个选中的连接
            {
                uint32 temp_param=0;
                uint16 index= 0;
                MMICONNECTION_LINKSET_OP_E op = MMICONNECTION_LINKSET_OP_EDIT;
                temp_param = op<<16;
                index = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);
                temp_param+=index;      
                //打开设置菜单窗口
                MMK_CreateWin((uint32*)MMICONNECTION_SETTINGLINKMENU_WIN_TAB, (ADD_DATA)temp_param);
                MMK_CloseWin(win_id);//返回上一级
            }
            break;
        
        default:
            break;
    }
    break;
        
    case MSG_PROMPTWIN_OK:                                    //delete
        list_index = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);
        if((list_index < s_connection_setting_info.settinglink_num[dual_sys]) && (s_connection_setting_info.settinglink_num[dual_sys] > 0))                                       
        {
            for(i = (uint32)list_index; i < (uint32)(s_connection_setting_info.settinglink_num[dual_sys]-1); i++)
            {
                SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][i],&s_connection_setting_info.setting_detail[dual_sys][i+1],sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            }
            SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys] - 1],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            s_connection_setting_info.settinglink_num[dual_sys]--;
        }
        else if((list_index ==  s_connection_setting_info.settinglink_num[dual_sys]) && (s_connection_setting_info.settinglink_num[dual_sys] > 0))
        {
            SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys] - 1],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            s_connection_setting_info.settinglink_num[dual_sys]--;
        }
        MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO,&s_connection_setting_info);
        
        //调整应用选中的ITEM
        ChangeAppSelectedIndex(dual_sys, list_index, MMICONNECTION_NOTIFY_EVENT_DEL_ITEM);
        //MMIPUB_OpenAlertSuccessWin(TXT_DELETED);
        GUILIST_RemoveItem(MMICONNECTION_LIST_SETTING_CTRL_ID, list_index);
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;
        
    case MSG_PROMPTWIN_CANCEL:
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
   
    return recode;

}
#endif


/*****************************************************************************/
// 	Description : Connection menu
//	Global resource dependence : 
//  Author: rong.gu
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleConnectionMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param)
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_MENU_CTRL_ID;
    uint16 index = 0;
    uint32 user_data = 0;

    //SCI_TRACE_LOW:"mmiconnection_wintab.c, HandleConnectionMenuWinMsg(), msg_id = %x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_1866_112_2_18_2_12_9_2,(uint8*)"d", msg_id);
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;

    case MSG_FULL_PAINT:
        InitPdaConnectionMainListCtrl();
        break;

    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_KEYDOWN_WEB:
        index = GUILIST_GetCurItemIndex(ctrl_id);
        GUILIST_GetItemData(ctrl_id, index, &user_data);
        switch ( (MMI_CONNECTION_MAIN_LIST_TYPE_E)user_data )
        {


#ifdef MMI_GPRS_SUPPORT
        case MMI_CONNECTION_MENU_LIST_GPRS_NET_USERID:  
            MMIAPICONNECTION_OpenSettingListWin();
            break;

        case MMI_CONNECTION_MENU_LIST_GPRS_MODESET:
            MMIAPICONNECTION_OpenGprsWin();
            break;
#endif
#ifdef MMI_DUALMODE_ENABLE
        case MMI_CONNECTION_MENU_LIST_NETSETTING:
            MMIAPISET_OpenSelNetWorkWin(MN_DUAL_SYS_1);
            break;
#else
        case MMI_CONNECTION_MENU_LIST_NETWORK:
            MMIAPISET_OpenSetNetWorkWin();
            break;
#endif

#ifdef MMI_PREFER_PLMN_SUPPORT
        case MMI_CONNECTION_MENU_LIST_GPRS_PREFER_NETWORK:
            MMIAPISET_OpenPreferNetWorkWin();
            break;
#endif

        //////////////////////////////////////////////////////////////////////////
        // added by feng.xiao
#ifdef DATA_ROAMING_SUPPORT
        case MMI_CONNECTION_MENU_LIST_GPRS_ROAMING:
        {
            if (MMAPIICONNECTION_Setting_GetDataRoamingStatus() == MMICONNECTION_SETTING_DATA_ROAMING_ENABLE)
            {
                //search the SIM in roaming status with activated pdp links
                if (MMIAPICONNECTION_IsRoamingActivedPdpLinkExist())
                {
                    MMIPUB_OpenQueryTextWinByTextId(TXT_CONNECTION_DATA_ROAMING_CUTOFF_QUERY,
                                                    PNULL,
                                                    HandleDataRoamingCutoffQueryWinMsg);
                }
                else
                {
                    MMIAPICONNECTION_Setting_SetDataRoamingStatus(MMICONNECTION_SETTING_DATA_ROAMING_DISABLE);
                }
            }
            else
            {
                MMIPUB_OpenQueryTextWinByTextId(TXT_CONNECTION_DATA_ROAMING_QUERY,
                                                PNULL,
                                                HandleDataRoamingQueryWinMsg);
            }

            MMK_SendMsg(win_id, MSG_FULL_PAINT, NULL);
            break;
        }
#endif

#ifdef MMI_GPRS_SWITCH_SUPPORT
        case MMI_CONNECTION_MENU_LIST_GPRS_SWITCH:
        {
            if (MMIAPICONNECTION_GetGPRSSwitchStatus()== MMICONNECTION_SETTING_GPRS_SWITCH_OFF)
            {
                MMIAPICONNECTION_SetGPRSSwitchStatus(MMICONNECTION_SETTING_GPRS_SWITCH_ON);
            }
            else
            {
                MMIAPICONNECTION_SetGPRSSwitchStatus(MMICONNECTION_SETTING_GPRS_SWITCH_OFF);
            }    
          

            MMK_SendMsg(win_id, MSG_FULL_PAINT, NULL);  //Update window
            break;
        }
#endif
#if 0
#ifdef WIFI_SUPPORT
        case ID_CONNECTION_WIFI:
            MMIAPIWIFI_OpenMainWin();
            break;
#endif

#ifdef MMI_TVOUT_ENABLE
        case ID_CONNECTION_OPEN_TV_OUT:
            if(TRUE == MMITHEME_IsTVOpen())
            {
                MMIAPITVOUT_Close();
            }
            else
            {
                MMIAPITVOUT_OpenWin();
            }
            break;

        case ID_CONNECTION_TV_OUT_SETTING:
            MMIAPITVOUTSetting_OpenWin();
            break;
#endif

#ifdef BLUETOOTH_SUPPORT
        case ID_CONNECTION_BLUETOOTH:
            MMIAPIBT_OpenMainMenuWin();
            break;
#endif

#ifdef GPS_SUPPORT		
        case ID_CONNECTION_GPS:
            MMIAPIGPS_OpenMainWin();
            break;
#endif
#endif
        default:
            break;
        }

        break;

    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CLOSE_WINDOW:
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }

    return (result);
}
#else

#ifdef MMI_GPRS_SWITCH_SUPPORT
/*****************************************************************************/
// 	Description : Set GPRS switch win.
//	Global resource dependence : none
//    Author: 
//	Note:
/*****************************************************************************/
LOCAL void SetGPRSSwitchSettingsWin(void)
{
    uint16 cur_selection = 0;
    MMICONNECTION_GPRS_SETTINGS_E gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
    
    GUILIST_SetMaxItem(MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID, MMICONNECTION_SETTING_GPRS_SWITCH_MAX, FALSE );//max item 2

    // add the item to listbox
    MMIAPISET_AppendListItemByTextId( TXT_COMMON_ON, TXT_COMMON_OK, MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID, GUIITEM_STYLE_ONE_LINE_RADIO);
    MMIAPISET_AppendListItemByTextId( TXT_COMMON_OFF, TXT_COMMON_OK, MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID, GUIITEM_STYLE_ONE_LINE_RADIO);

    gprs_settings = MMIAPICONNECTION_GetGPRSSwitchStatus();

    cur_selection = (uint16)gprs_settings;
        
    GUILIST_SetSelectedItem(MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID, cur_selection, TRUE);
    GUILIST_SetCurItemIndex(MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID, cur_selection);
}

/*****************************************************************************/
// 	Description : Handle GPRS switch setting win.
//	Global resource dependence : none
//    Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleGprsSwitchWinMsg(MMI_WIN_ID_T win_id, 
                                          MMI_MESSAGE_ID_E msg_id,
                                          DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMICONNECTION_SETTING_GPRS_SWITCH_E gprs_settings = MMICONNECTION_SETTING_GPRS_SWITCH_ON;
    uint16 cur_selection = 0;
	
    switch (msg_id)
    {   
    case MSG_OPEN_WINDOW:       
        MMK_SetAtvCtrl(win_id, MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID);
        SetGPRSSwitchSettingsWin();
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_COMMON_OK, TXT_NULL, STXT_RETURN, FALSE);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        cur_selection = GUILIST_GetCurItemIndex(MMICONNECTION_GPRSSWITCH_LIST_CTRL_ID);

        gprs_settings = (MMICONNECTION_SETTING_GPRS_SWITCH_E)cur_selection;
        
        MMIAPICONNECTION_SetGPRSSwitchStatus(gprs_settings);
        MMK_CloseWin( win_id );
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin( win_id );
        break;
		
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}                                          
#endif

/*****************************************************************************/
// 	Description : Connection menu
//	Global resource dependence : 
//  Author: ryan.xu
//	Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleConnectionMenuWinMsg(	
									 MMI_WIN_ID_T win_id, 
									 MMI_MESSAGE_ID_E msg_id,
									 DPARAM param)
{
    MMI_RESULT_E    result = MMI_RESULT_TRUE;
    uint32  node_id = 0;
    GUI_BOTH_RECT_T both_rect = MMITHEME_GetWinClientBothRect(win_id);
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_MENU_CTRL_ID;


    //SCI_TRACE_LOW:"mmiconnection_wintab.c, HandleConnectionMenuWinMsg(), msg_id = %x"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_2010_112_2_18_2_12_9_3,(uint8*)"d", msg_id);
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_COMMON_OK, TXT_NULL, STXT_RETURN, FALSE);
        GUIAPICTRL_SetBothRect(ctrl_id, &both_rect);
        MMK_SetAtvCtrl(MMICONNECTION_MENU_WIN_ID, MMICONNECTION_MENU_CTRL_ID);
        break;

    case MSG_GET_FOCUS:
        {
#ifdef MMI_GPRS_SUPPORT
            uint16 sim_sys                        = MN_DUAL_SYS_1;
            uint32 sim_num                        = 0;

            sim_num = MMIAPIPHONE_GetSimAvailableNum(&sim_sys, 1);
#ifdef MMI_GPRS_SWITCH_SUPPORT      
            if ((1 < sim_num) || MMICONNECTION_SETTING_GPRS_SWITCH_OFF == MMIAPICONNECTION_GetGPRSSwitchStatus())
#else
            if (1 < sim_num)
#endif
            {
                GUIMENU_SetNodeGrayed(TRUE, s_connection_index, 0, MMICONNECTION_MENU_CTRL_ID);
            }
            else
            {
                GUIMENU_SetNodeGrayed(FALSE, s_connection_index, 0, MMICONNECTION_MENU_CTRL_ID);
            }
#endif
        }
        break;

    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_KEYDOWN_WEB:
        node_id = GUIMENU_GetCurNodeId(MMICONNECTION_MENU_CTRL_ID);
        
        switch ( node_id )
        {
#ifdef MMI_TVOUT_ENABLE
        case ID_CONNECTION_OPEN_TV_OUT:
            if(TRUE == MMITHEME_IsTVOpen())
            {
                MMIAPITVOUT_Close();
            }
            else
            {
                MMIAPITVOUT_OpenWin();
            }
            break;

        case ID_CONNECTION_TV_OUT_SETTING:
            MMIAPITVOUTSetting_OpenWin();
            break;
#endif

#ifdef BLUETOOTH_SUPPORT
        case ID_CONNECTION_BLUETOOTH:
            MMIAPIBT_OpenMainMenuWin();
            break;
#endif

#ifdef GPS_SUPPORT		
        case ID_CONNECTION_GPS:
            MMIAPIGPS_OpenMainWin();
            break;
#endif

#ifdef MMI_GPRS_SUPPORT
        case ID_CONNECTION_NET_USERID:  
            MMIAPICONNECTION_OpenSettingListWin();
            break;

        case ID_CONNECTION_GPRS:
            MMIAPICONNECTION_OpenGprsWin();
            break;
#endif
        case ID_CONNECTION_SET_NETWORK://ID_SET_NETWORK
            MMIAPISET_OpenSetNetWorkWin();
            break;
#ifdef MMI_DUALMODE_ENABLE
        case ID_CONNECTION_SET_NETSETTING:
            MMIAPISET_OpenSelNetWorkWin(MN_DUAL_SYS_1);
            break;
#endif

        default:
            break;

#ifdef MMI_GPRS_SWITCH_SUPPORT
        case ID_CONNECTION_GRPS_SWITCH:
            MMK_CreateWin((uint32)MMICONNECTION_GPRS_SWITCH_WIN_TAB,PNULL);
            break;

#endif
#ifdef WIFI_SUPPORT
        case ID_CONNECTION_WIFI:
            MMIAPIWIFI_OpenMainWin();
            break;
#endif
        }

        break;

    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CLOSE_WINDOW:
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }

    return (result);
}
#endif


#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
// 	Description : handle message of setting list window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingListWinMsg(
                                           MMI_WIN_ID_T	    win_id,
                                           MMI_MESSAGE_ID_E	msg_id,
                                           DPARAM           param
                                           )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint16 i = 0;
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_LIST_SETTING_CTRL_ID;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //动态添加LIST ITEM
        GUILIST_SetMaxItem( ctrl_id, MMICONNECTION_MAX_SETTING_NUM, FALSE );//max item 20
#ifdef _ATEST_
#ifdef WIN32
        ATESTOTA_GetTotalMsgNumberInInternetAccount(s_connection_setting_info.settinglink_num[dual_sys]);
#endif//WIN32
#endif//_ATEST_

        for(i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)                              //显示LIST
        {
            if (!s_connection_setting_info.setting_detail[dual_sys][i].is_customed_detail)
            {
                AppendOneLineTextListItem(ctrl_id,
                    GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                    STXT_OPTION,
                    IMAGE_CONNECTION_LINK_ITEM_RW,
                    s_connection_setting_info.setting_detail[dual_sys][i].name,
                    s_connection_setting_info.setting_detail[dual_sys][i].name_len,
                    i,
                    FALSE
                    );
            }
            else
            {
                AppendOneLineTextListItem(ctrl_id,
                    GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                    STXT_OPTION,
                    IMAGE_CONNECTION_LINK_ITEM_RO,
                    s_connection_setting_info.setting_detail[dual_sys][i].name,
                    s_connection_setting_info.setting_detail[dual_sys][i].name_len,
                    i,
                    FALSE
                    );
            }
        }            

#ifdef MMI_PDA_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;
        
    case MSG_GET_FOCUS:
        {
            uint16 oldItemIndex = 0;
            uint16 oldSettinNum = 0;
            uint16 topItemIndex = 0;
            
            oldItemIndex = GUILIST_GetCurItemIndex(ctrl_id);
            oldSettinNum = GUILIST_GetTotalItemNum(ctrl_id);
            topItemIndex = GUILIST_GetTopItemIndex(ctrl_id);

            GUILIST_SetMaxItem(ctrl_id, MMICONNECTION_MAX_SETTING_NUM,FALSE);//max item 20
            for(i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)                             
            {
                
                if(!s_connection_setting_info.setting_detail[dual_sys][i].is_customed_detail)
                {
                    AppendOneLineTextListItem(ctrl_id,
                        GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                        STXT_OPTION,
                        IMAGE_CONNECTION_LINK_ITEM_RW,
                        s_connection_setting_info.setting_detail[dual_sys][i].name,
                        s_connection_setting_info.setting_detail[dual_sys][i].name_len,
                        i,
                        FALSE
                        );
                }
                else
                {
                    AppendOneLineTextListItem(ctrl_id,
                        GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                        STXT_OPTION,
                        IMAGE_CONNECTION_LINK_ITEM_RO,
                        s_connection_setting_info.setting_detail[dual_sys][i].name,
                        s_connection_setting_info.setting_detail[dual_sys][i].name_len,
                        i,
                        FALSE
                        );
                }
            }

            GUILIST_SetCurItemIndex(ctrl_id, oldItemIndex);
            GUILIST_SetTopItemIndex(ctrl_id, topItemIndex);
        }
        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
        MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLINKLISTMENU_WIN_TAB, PNULL);
        break;

#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_LIST_LONGOK:
        MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLINKLIST_LONGOKMENU_WIN_TAB, PNULL);	
        break;
#endif

    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    case MSG_APP_WEB:
        {
            uint32 temp_param=0;
            MMICONNECTION_LINKSET_OP_E op = MMICONNECTION_LINKSET_OP_EDIT;

            i = GUILIST_GetCurItemIndex(ctrl_id);
#ifdef _ATEST_
#ifdef WIN32       
            ATESTOTA_GetOtaConfigInfoFromLinkSetting(s_connection_setting_info.setting_detail[dual_sys][i]); // added by Mingxuan.Zhu
#endif//WIN32
#endif//_ATEST_
            if((!s_connection_setting_info.setting_detail[dual_sys][i].is_customed_detail) && (0 != s_connection_setting_info.settinglink_num[dual_sys]))
            {
                op = MMICONNECTION_LINKSET_OP_EDIT;
                temp_param = op<<16;
                temp_param+=i;   
                MMK_CreateWin((uint32*)MMICONNECTION_SETTINGLINKMENU_WIN_TAB, (ADD_DATA)temp_param);
            }
            else
            {
                op = MMICONNECTION_LINKSET_OP_READ;
                temp_param = op<<16;
                temp_param+=i;  
                MMK_CreateWin((uint32*)MMICONNECTION_SETTINGLINKDETAILVIEW_WIN_TAB, (ADD_DATA)temp_param);
            }
        }
        break;
        
    case MSG_CLOSE_WINDOW:
        MMK_CloseWin(MMICONNECTION_SETTINGLINKMENU_WIN_ID);
        MMK_CloseWin(MMICONNECTION_SETTINGSECURITYCONNECTLIST_WIN_ID);
        MMK_CloseWin(MMICONNECTION_INPUTAGENTNAME_WIN_ID);
        MMK_CloseWin(MMICONNECTION_INPUTUSERNAME_WIN_ID);
        MMK_CloseWin(MMICONNECTION_INPUTPASSWORD_WIN_ID);
        MMK_CloseWin(MMICONNECTION_INPUTAPN_WIN_ID);
        MMK_CloseWin(MMICONNECTION_INPUTGATEWAY_WIN_ID);
        MMK_CloseWin(MMICONNECTION_EDITPORT_WIN_ID);		
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:
        //SCI_TRACE_LOW:"[MMICONNECTION]:HandleSettingListWinMsg() msg_id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_2294_112_2_18_2_12_10_4,(uint8*)"d", msg_id);
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}


/*****************************************************************************/
// 	Description : handle message of setting link list menu window of connection
//      This menu is to deal to a selected list such as,edit,delete,etc.
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkListMenuWinMsg(
                                                   MMI_WIN_ID_T	    win_id, 
                                                   MMI_MESSAGE_ID_E	msg_id, 
                                                   DPARAM			param
                                                   )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint16			list_index = 0;
    uint32			group_id = 0;
    uint32			menu_id = 0;
    uint32			i = 0;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //加入对移动定制项的特殊处理
        i = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);
        
        if(0 == s_connection_setting_info.settinglink_num[dual_sys] ||
            s_connection_setting_info.setting_detail[dual_sys][i].is_customed_detail)
        {
            GUIMENU_SetItemGrayed(MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID,MENU_CONNECTION_SETTING_LINKLIST,ID_CONNECTION_SETTING_DELETE,TRUE);	
            GUIMENU_SetItemGrayed(MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID,MENU_CONNECTION_SETTING_LINKLIST,ID_CONNECTION_SETTING_EDIT,TRUE);
        }
        else
        {
            GUIMENU_SetItemGrayed(MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID,MENU_CONNECTION_SETTING_LINKLIST,ID_CONNECTION_SETTING_DELETE,FALSE);	
            GUIMENU_SetItemGrayed(MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID,MENU_CONNECTION_SETTING_LINKLIST,ID_CONNECTION_SETTING_EDIT,FALSE);
        }

        MMK_SetAtvCtrl(win_id,MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID);
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_COMMON_OK, TXT_NULL, STXT_RETURN, FALSE);
        break;
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        GUIMENU_GetId(MMICONNECTION_MENU_SETTINGLINKLIST_CTRL_ID, (MMI_MENU_GROUP_ID_T *)(&group_id),(MMI_MENU_ID_T *) (&menu_id));  
        switch(menu_id)//菜单节点ID
        {
        case ID_CONNECTION_SETTING_SETGBUILD:     //新建连接
            if(s_connection_setting_info.settinglink_num[dual_sys] >= MMICONNECTION_MAX_SETTING_NUM)
            {
                MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_LISTFULL);
            }
            else
            {
                //参数构成：操作类型+index，高16位放操作类型，低16位放index
                uint32 temp_param=0;
                MMICONNECTION_LINKSET_OP_E op = MMICONNECTION_LINKSET_OP_NEW;
                temp_param = op<<16;
                temp_param+=s_connection_setting_info.settinglink_num[dual_sys];      
                SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys]],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T) );
                //打开设置菜单窗口
                MMK_CreateWin((uint32*)MMICONNECTION_SETTINGLINKMENU_WIN_TAB, (ADD_DATA)temp_param);
            }
            MMK_CloseWin(win_id);
            break;
        case ID_CONNECTION_SETTING_DELETE:      										//删除一个选中的连接
            if (1 == s_connection_setting_info.settinglink_num[dual_sys])
            {
                MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_DELETE_FAIL);
                MMK_CloseWin(win_id);//返回上一级
            }
            else
            {
                MMIPUB_OpenQueryWinByTextId(TXT_DELETE_QUERY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
            }
            break;
            
        case ID_CONNECTION_SETTING_EDIT:                                                                           //编辑一个选中的连接
            {
                uint32 temp_param=0;
                uint16 index= 0;
                MMICONNECTION_LINKSET_OP_E op = MMICONNECTION_LINKSET_OP_EDIT;
                temp_param = op<<16;
                index = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);
                temp_param+=index;      
                //打开设置菜单窗口
                MMK_CreateWin((uint32*)MMICONNECTION_SETTINGLINKMENU_WIN_TAB, (ADD_DATA)temp_param);
                MMK_CloseWin(win_id);//返回上一级
            }
            break;
        case ID_CONNECTION_SETTING_ADDLINKSETTING:                                                  //从连接清单中添加
            if(s_connection_setting_info.settinglink_num[dual_sys] >= MMICONNECTION_MAX_SETTING_NUM)
            {
                MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_LISTFULL);
            }
            else
            {
                //打开设置列表窗口
                MMK_CreateWin((uint32*)MMICONNECTION_ADDSETTINGLIST_WIN_TAB, PNULL);
            }
            MMK_CloseWin(win_id);
            break;
        case ID_CONNECTION_SETTING_REASERCH:  //根据PLMN 从连接清单中搜索合适的配置
            {
                /*
                在这里修改搜索算法的目的是为了克服原来的算法只能在搜索列表小于24的情况下才能使用
                修改后的算法没有这个限制，只要个数在UINT的数据的范围内即可
                */
                PHONE_PLMN_NAME_E name = PLMN_INVALID;
                uint32 search_num =0;                

                if (!MMIAPIPHONE_IsSimCardOk(dual_sys))
                {
                    MMIAPIPHONE_AlertSimNorOKStatus();
                    recode = MMI_RESULT_FALSE;
                }
                else
                {
                    BOOLEAN is_res_data_exist = FALSE;
                    MMICONNECTION_RES_INFO_T res_info = {0};

                    is_res_data_exist = LoadNetworkAccountInfo(&res_info);

                    if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
                    {
                        UnloadNetworkAccountInfo(&res_info);
                        break;
                    }
                    
                    MMIAPIPHONE_GetFullPlmn(dual_sys, PNULL, &name);

                    for(i = 0;i<res_info.res_num;i ++)
                    {
                        PHONE_PLMN_NAME_E plmn_name = PLMN_INVALID;
                        MN_PLMN_T plmn = {0};

                        plmn.mcc = res_info.res_item_ptr[i].mcc;
                        plmn.mnc = res_info.res_item_ptr[i].mnc;

                        plmn_name = MMIAPIPHONE_GetNetWorkId(&plmn);

                        if (IsSameNetworkName(name, plmn_name))
                        {
                            search_num++;
                        }
                    }

                    UnloadNetworkAccountInfo(&res_info);
                    
                    if(0 == search_num)
                    {
                        MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_HAVENOITEM); 
                    }
                    else
                    {
                        //打开设置列表窗口
                        MMK_CreateWin((uint32*)MMICONNECTION_SEARCHLINKLIST_WIN_TAB, PNULL);//根据PLMN 从连接清单中搜索合适的配置
                    }
                }
                MMK_CloseWin(win_id);
            }
            break;
            
#if defined(DATA_ACCOUNT_IN_TCARD_SUPPORT) && defined(BROWSER_SUPPORT)
        case ID_CONNECTION_SETTING_DOWNLOAD_RES:
            //打开下载数据窗口
            CheckNetworkAccountResFileToDownload();
            MMK_CloseWin(win_id);
            break;
#endif
        default:
            break;
            
        }
        break;
        
    case MSG_PROMPTWIN_OK:                                    //delete
        list_index = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SETTING_CTRL_ID);
        if((list_index < s_connection_setting_info.settinglink_num[dual_sys]) && (s_connection_setting_info.settinglink_num[dual_sys] > 0))                                       
        {
            for(i = (uint32)list_index; i < (uint32)(s_connection_setting_info.settinglink_num[dual_sys]-1); i++)
            {
                SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][i],&s_connection_setting_info.setting_detail[dual_sys][i+1],sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            }
            SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys] - 1],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            s_connection_setting_info.settinglink_num[dual_sys]--;
        }
        else if((list_index ==  s_connection_setting_info.settinglink_num[dual_sys]) && (s_connection_setting_info.settinglink_num[dual_sys] > 0))
        {
            SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][s_connection_setting_info.settinglink_num[dual_sys] - 1],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            s_connection_setting_info.settinglink_num[dual_sys]--;
        }
        MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO,&s_connection_setting_info);
        //调整应用选中的ITEM
        ChangeAppSelectedIndex(dual_sys, list_index, MMICONNECTION_NOTIFY_EVENT_DEL_ITEM);
#ifndef MMI_GUI_STYLE_TYPICAL
        MMIPUB_OpenAlertSuccessWin(TXT_DELETED);
#endif
        GUILIST_RemoveItem(MMICONNECTION_LIST_SETTING_CTRL_ID, list_index);
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);		
        break;
    case MSG_PROMPTWIN_CANCEL:
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

/*****************************************************************************/
// 	Description : handle message of  search link list window
//	Global resource dependence : none
//  Author: wenbao.bai
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSearchResultLinkListWinMsg(
                                                    MMI_WIN_ID_T	    win_id,
                                                    MMI_MESSAGE_ID_E	msg_id,
                                                    DPARAM				param
                                                    )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint32			i = 0,j=0,k=0;
    uint16 cur_select = 0;
    PHONE_PLMN_NAME_E name = PLMN_INVALID;
    BOOLEAN is_find = FALSE;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;
    PHONE_PLMN_NAME_E plmn_name = PLMN_INVALID;
    MN_PLMN_T plmn = {0};
    BOOLEAN is_res_data_exist = FALSE;
    MMICONNECTION_RES_INFO_T res_info = {0};

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }
        
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            is_res_data_exist = LoadNetworkAccountInfo(&res_info);

            if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
            {
                MMK_CloseWin(win_id);
                UnloadNetworkAccountInfo(&res_info);
                break;
            }

            MMK_SetAtvCtrl(MMICONNECTION_SEARCHLINKLIST_WIN_ID,MMICONNECTION_LIST_SEARCHLINKLIST_CTRL_ID);
            GUILIST_SetMaxItem(MMICONNECTION_LIST_SEARCHLINKLIST_CTRL_ID, res_info.res_num,FALSE);
            
            MMIAPIPHONE_GetFullPlmn(dual_sys, PNULL, &name);

            for(i = 0; i < res_info.res_num; i ++)
            {
                plmn.mcc = res_info.res_item_ptr[i].mcc;
                plmn.mnc = res_info.res_item_ptr[i].mnc;

                plmn_name = MMIAPIPHONE_GetNetWorkId(&plmn);

                if (IsSameNetworkName(name, plmn_name))
                {
                    AppendOneLineTextListItem(MMICONNECTION_LIST_SEARCHLINKLIST_CTRL_ID,
                        GUIITEM_STYLE_ONE_LINE_ICON_TEXT,
                        TXT_COMMON_OK,
                        IMAGE_RADIO_UNSELECTED_ICON,
                        res_info.res_item_ptr[i].name,
                        res_info.res_item_ptr[i].name_len,
                        i,
                        FALSE
                        );
                }
            }

            UnloadNetworkAccountInfo(&res_info);
        }
        break;
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        {
            cur_select = GUILIST_GetCurItemIndex(MMICONNECTION_LIST_SEARCHLINKLIST_CTRL_ID);

            is_res_data_exist = LoadNetworkAccountInfo(&res_info);

            if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
            {
                UnloadNetworkAccountInfo(&res_info);
                break;
            }
            
            MMIAPIPHONE_GetFullPlmn(dual_sys, PNULL, &name);
            
            for(i = 0; i < res_info.res_num; i ++)
            {
                plmn.mcc = res_info.res_item_ptr[i].mcc;
                plmn.mnc = res_info.res_item_ptr[i].mnc;

                plmn_name = MMIAPIPHONE_GetNetWorkId(&plmn);

                for (j = 0; j < MMI_DUAL_SYS_MAX; j++)
                {
                    if (IsSameNetworkName(name, plmn_name))
                    {
                        if(k == cur_select)
                        {
                            is_find = TRUE;

                            break;
                        }

                        k ++;
                        break;
                    }
                }

                if (is_find)
                {
                    break;
                }
            }
            
            if(s_connection_setting_info.settinglink_num[dual_sys]<MMICONNECTION_MAX_SETTING_NUM && i < res_info.res_num)
            {
                uint8 index = s_connection_setting_info.settinglink_num[dual_sys];

                SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][index],
                                                            &res_info.res_item_ptr[i], FALSE, TRUE, i);
                
                MMICONNECTION_ClearHighPriList(dual_sys);
                MMICONNECTION_SetHighPriList(dual_sys, index);
                s_connection_setting_info.settinglink_num[dual_sys] ++;
                MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

                MMICONNECTION_AdaptingAccount(dual_sys, res_info.res_item_ptr[i].account_type);//默认列表中添加的帐户，允许做一次帐户匹配

                //MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);

                MMK_CloseWin(win_id);
                MMK_CloseWin(MMICONNECTION_SETTINGLINKLISTMENU_WIN_ID);
            }
            else
            {
            	MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_LISTFULL);
            }

            UnloadNetworkAccountInfo(&res_info);
        }
        break;
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

/*****************************************************************************/
//     Description : to set the MMICONNECTION_GPRS_SETTINGS_CTRL_ID
//    Global resource dependence : 
//  Author:wancan.you
//    Note: 
/*****************************************************************************/
LOCAL void SetGPRSSettingsWin(void)
{
    uint16 cur_selection = 0;
    MMICONNECTION_GPRS_SETTINGS_E gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
    
    GUILIST_SetMaxItem(MMICONNECTION_GPRS_SETTINGS_CTRL_ID, MMICONNECTION_GPRS_SETTINGS_MAX, FALSE );//max item 2

    // add the item to listbox
    MMIAPISET_AppendListItemByTextId( TXT_CONNECTION_GPRS_DETACH, TXT_COMMON_OK, MMICONNECTION_GPRS_SETTINGS_CTRL_ID, GUIITEM_STYLE_ONE_LINE_RADIO);
    MMIAPISET_AppendListItemByTextId( TXT_CONNECTION_GPRS_ALWAYS, TXT_COMMON_OK, MMICONNECTION_GPRS_SETTINGS_CTRL_ID, GUIITEM_STYLE_ONE_LINE_RADIO);
#ifdef MMI_DUALMODE_ENABLE
    MMIAPISET_AppendListItemByTextId( TXT_ENERGY_SAVING, TXT_COMMON_OK, MMICONNECTION_GPRS_SETTINGS_CTRL_ID, GUIITEM_STYLE_ONE_LINE_RADIO);
#endif  
    gprs_settings = MMICONNECTION_GetGPRSModeStatus();

    cur_selection = (uint16)gprs_settings;
        
    GUILIST_SetSelectedItem(MMICONNECTION_GPRS_SETTINGS_CTRL_ID, cur_selection, TRUE);
    GUILIST_SetCurItemIndex(MMICONNECTION_GPRS_SETTINGS_CTRL_ID, cur_selection);
}

/*****************************************************************************/
//     Description : to handle window message about gprs settings.
//    Global resource dependence : 
//  Author: wancan.you
//    Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleGPRSSettingsWinMsg(
                                                       MMI_WIN_ID_T        win_id, 
                                                       MMI_MESSAGE_ID_E msg_id, 
                                                       DPARAM            param
                                                       )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMICONNECTION_GPRS_SETTINGS_E gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
    uint16 cur_selection = 0;
	
    switch (msg_id)
    {   
    case MSG_OPEN_WINDOW:       
        MMK_SetAtvCtrl(win_id, MMICONNECTION_GPRS_SETTINGS_CTRL_ID);
        SetGPRSSettingsWin();
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_COMMON_OK, TXT_NULL, STXT_RETURN, FALSE);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        cur_selection = GUILIST_GetCurItemIndex(MMICONNECTION_GPRS_SETTINGS_CTRL_ID);

        gprs_settings = (MMICONNECTION_GPRS_SETTINGS_E)cur_selection;
        
        MMIAPICONNECTION_SetGPRSStatus(gprs_settings);

        if (!MMIAPIPDP_PsIsPdpLinkExist())
        {
            if (MMICONNECTION_GPRS_ATTACH_ONNEED == MMICONNECTION_GetGPRSModeStatus())
            {
                MMIAPIPHONE_SetGPRSDetach();
            }
            else
            {
                MMIAPIPHONE_SetGPRSAttach();
            }
        }

#ifdef MMI_DUALMODE_ENABLE
#ifndef WIN32
        {
            uint32 i = 0;
            MMICONNECTION_GPRS_SETTINGS_E grps_status = MMICONNECTION_GetGPRSModeStatus();
        
            for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
            {
                if (MMICONNECTION_GPRS_ALWAYS_LC == grps_status)
                {
                    MNGPRS_SetEnergySavingMode(i, TRUE);
                }
                else
                {
                    MNGPRS_SetEnergySavingMode(i, FALSE);
                }
            }
        }
#endif
#endif
#ifndef MMI_PDA_SUPPORT
        MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
#endif       
        MMK_CloseWin( win_id );
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin( win_id );
        break;
		
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}
#endif

/*****************************************************************************/
// 	Description : insert dynamenu node by label
//	Global resource dependence : 
//  Author:fen.xie
//	Note:
/*****************************************************************************/
LOCAL void InsertNodeByLabel(
                    MMI_CTRL_ID_T      ctrl_id,        //控件id
                    MMI_TEXT_ID_T   	text_id,       //插入的节点的文本
                    uint32          	node_id,        //节点编号
                    uint32          	parent_node_id, //父节点编号
                    uint16          	index,           //位置
                    MMI_IMAGE_ID_T      select_icon_id, //item selected icon id
                    BOOLEAN    is_grayed   //is_grayed
                    )
{
    MMI_STRING_T            kstring = {0};
    GUIMENU_DYNA_ITEM_T     node_item = {0};
    
    
    MMI_GetLabelTextByLang(text_id, &kstring);
    node_item.item_text_ptr = &kstring;
    node_item.select_icon_id = select_icon_id;
    node_item.is_grayed = is_grayed;
    GUIMENU_InsertNode(index,node_id,parent_node_id,&node_item,ctrl_id);
}

#ifdef MMI_GPRS_SWITCH_SUPPORT
/*****************************************************************************/
//  Description : Get GPRS switch mode (on or mff)
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC MMICONNECTION_SETTING_GPRS_SWITCH_E MMIAPICONNECTION_GetGPRSSwitchStatus(void)
{
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    MMICONNECTION_SETTING_GPRS_SWITCH_E  gprs_switch_settings=MMICONNECTION_SETTING_GPRS_SWITCH_ON;
    
    MMINV_READ(MMINV_CONNECTION_GPRS_SWITCH,&gprs_switch_settings,return_value);
    
    if (MN_RETURN_SUCCESS != return_value)
    {
        gprs_switch_settings = MMICONNECTION_SETTING_GPRS_SWITCH_ON;
        MMINV_WRITE(MMINV_CONNECTION_GPRS_SWITCH, &gprs_switch_settings);
    }

    return gprs_switch_settings;
}
/*****************************************************************************/
//  Description : Set GPRS switch mode (on or mff)
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_SetGPRSSwitchStatus(MMICONNECTION_SETTING_GPRS_SWITCH_E gprs_switch_settings)
{

    if (gprs_switch_settings == MMIAPICONNECTION_GetGPRSSwitchStatus()) //Same value
    {
        return TRUE;
    }

    MMINV_WRITE(MMINV_CONNECTION_GPRS_SWITCH, &gprs_switch_settings);

    if (gprs_switch_settings == MMICONNECTION_SETTING_GPRS_SWITCH_ON)
    {
        s_gprs_turned_off = FALSE;
        MMIAPICONNECTION_TurnOnGPRS();
    }
    else
    {
        s_gprs_turned_off = TRUE;
        MMIAPICONNECTION_TurnOffGPRS();
    }
    MAIN_UpdateIdleSignalState();
    
    return TRUE;
}
/*****************************************************************************/
//  Description :Turn Off GPRS
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIAPICONNECTION_TurnOffGPRS(void)
{
    if (MMIAPIPDP_PsIsPdpLinkExist())
    {
        MMIAPIPDP_ReleaseAll(NULL);    
    }
    else
    {
        MMIAPIPHONE_SetGPRSDetach();
    }
    return TRUE;
}


/*****************************************************************************/
//  Description :Turn on GPRS
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN MMIAPICONNECTION_TurnOnGPRS(void)
{
    if (!MMIAPIPDP_PsIsPdpLinkExist())
    {
        if (MMICONNECTION_GPRS_ATTACH_ONNEED == MMICONNECTION_GetGPRSModeStatus())
        {
            MMIAPIPHONE_SetGPRSDetach();
        }
        else
        {
            MMIAPIPHONE_SetGPRSAttach();
        }
    }
    return TRUE;
}


#endif


/*****************************************************************************/
//  Description : Set GPRS Attach mode (always or manual)
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_SetGPRSStatus(MMICONNECTION_GPRS_SETTINGS_E gprs_settings)
{
#ifdef MMI_GPRS_SUPPORT
    MMINV_WRITE(MMINV_CONNECTION_GPRS_SETTINGS, &gprs_settings);
#endif

}

/*****************************************************************************/
// 	Description : Get GPRS Mode Settings.
//	Global resource dependence : none
//    Author: 
//	Note:
/*****************************************************************************/
LOCAL MMICONNECTION_GPRS_SETTINGS_E MMICONNECTION_GetGPRSModeStatus(void)
{
    MMICONNECTION_GPRS_SETTINGS_E gprs_mode_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
#ifdef MMI_GPRS_SUPPORT
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;

    MMINV_READ(MMINV_CONNECTION_GPRS_SETTINGS, &gprs_mode_settings, return_value);

    if (MN_RETURN_SUCCESS != return_value)
    {
#ifdef MMI_DUALMODE_ENABLE
        gprs_mode_settings = MMICONNECTION_GPRS_ALWAYS_LC;
#else
        gprs_mode_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
#endif

        MMINV_WRITE(MMINV_CONNECTION_GPRS_SETTINGS, &gprs_mode_settings);
    }

    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_2841_112_2_18_2_12_11_5,(uint8*)"d", gprs_mode_settings);
#endif
    return gprs_mode_settings;
}


/*****************************************************************************/
//  Description : Get GPRS Attach mode (always or manual)
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC MMICONNECTION_GPRS_SETTINGS_E MMIAPICONNECTION_GetGPRSStatus(void)
{
    MMICONNECTION_GPRS_SETTINGS_E gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
#ifdef MMI_GPRS_SUPPORT
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
#ifdef MMI_GPRS_SWITCH_SUPPORT
    if (MMICONNECTION_SETTING_GPRS_SWITCH_OFF == MMIAPICONNECTION_GetGPRSSwitchStatus())
    {
        return MMICONNECTION_GPRS_TURNED_OFF;
    }
#endif
    MMINV_READ(MMINV_CONNECTION_GPRS_SETTINGS, &gprs_settings, return_value);

    if (MN_RETURN_SUCCESS != return_value)
    {
#ifdef MMI_DUALMODE_ENABLE
        gprs_settings = MMICONNECTION_GPRS_ALWAYS_LC;
#else
        gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
#endif

        MMINV_WRITE(MMINV_CONNECTION_GPRS_SETTINGS, &gprs_settings);
    }

    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_2841_112_2_18_2_12_11_5,(uint8*)"d", gprs_settings);
#endif
    return gprs_settings;
}

/*****************************************************************************/
//  Description : 是否需要自动Attach GPRS
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_isGPRSNeedAutoAttach(void)
{
    MMICONNECTION_GPRS_SETTINGS_E gprs_settings = MMICONNECTION_GPRS_ATTACH_ONNEED;
    BOOLEAN ret = FALSE;
    gprs_settings = MMIAPICONNECTION_GetGPRSStatus();

#ifdef MMI_GPRS_SWITCH_SUPPORT
    if ((MMICONNECTION_GPRS_ATTACH_ONNEED == gprs_settings)||(MMICONNECTION_GPRS_TURNED_OFF == gprs_settings))
#else
    if (MMICONNECTION_GPRS_ATTACH_ONNEED == gprs_settings)
#endif    
    {
        ret = FALSE;
    }
    else
    {
        ret = TRUE;
    }
    return ret;
    
}

/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author:
//  Note: 
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_isGPRSSwitchedOff(void)
{
#ifdef MMI_GPRS_SWITCH_SUPPORT
    return s_gprs_turned_off;
#else  
    return FALSE;
#endif
}


#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
// 	Description : handle message of Add  setting list window of browser
//	Global resource dependence : none
//  Author: wenbao.bai
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleAddSettingListWinMsg(
                                              MMI_WIN_ID_T  	win_id,
                                              MMI_MESSAGE_ID_E	msg_id,
                                              DPARAM			param
                                              )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint16			i = 0;
    MMI_CTRL_ID_T   ctrl_id = MMICONNECTION_LIST_SETTINGADDLIST_CTRL_ID;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;
    BOOLEAN is_res_data_exist = FALSE;
    MMICONNECTION_RES_INFO_T res_info = {0};
    MMI_TEXT_ID_T middle_softkey_id = TXT_NULL;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        is_res_data_exist = LoadNetworkAccountInfo(&res_info);

        if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
        {
            MMK_CloseWin(win_id);
            UnloadNetworkAccountInfo(&res_info);
            break;
        }

        MMK_SetAtvCtrl(MMICONNECTION_ADDSETTINGLIST_WIN_ID,MMICONNECTION_LIST_SETTINGADDLIST_CTRL_ID);
        //动态添加LIST ITEM
        GUILIST_SetMaxItem(ctrl_id, res_info.res_num,FALSE);//max item 20

        if (s_connection_setting_info.settinglink_num[dual_sys] < MMICONNECTION_MAX_SETTING_NUM)
        {
            GUILIST_SetMaxSelectedItem(ctrl_id, MMICONNECTION_MAX_SETTING_NUM - s_connection_setting_info.settinglink_num[dual_sys]);
            middle_softkey_id = STXT_SELECT;
        }
        else
        {
            GUILIST_SetMaxSelectedItem(ctrl_id, 0);
            middle_softkey_id = TXT_NULL;
        }
        GUILIST_SetOwnSofterKey(ctrl_id, TRUE);
        for(i = 0; i<res_info.res_num; i++)
        {
            AppendOneLineCheckListItem(ctrl_id,
                GUIITEM_STYLE_ONE_LINE_CHECK,
                TXT_COMMON_OK,
                middle_softkey_id,
                0,
                res_info.res_item_ptr[i].name,
                res_info.res_item_ptr[i].name_len,
                i,
                FALSE
                );
        }

        UnloadNetworkAccountInfo(&res_info);
        break;
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    case MSG_SK_SKB_ICON_PRESSED:
        {
            const GUILIST_ITEM_T *item_ptr = PNULL;
            GUILIST_ITEM_T  item = {0};/*lint !e64*/
            GUILIST_ITEM_DATA_T item_data = {0};/*lint !e64*/
            uint16 cur_index = 0;
            uint16 selected_num = 0;
            
            selected_num = GUILIST_GetSelectedItemIndex(ctrl_id, PNULL, 0);
            cur_index = GUILIST_GetCurItemIndex(ctrl_id);
            item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id, cur_index);
            if(PNULL == item_ptr)
            {
                recode = MMI_RESULT_FALSE;
                break;
            }
            if(GUILIST_GetItemState(item_ptr, GUIITEM_STATE_SELECTED))
            {
                GUILIST_SetSelectedItem(ctrl_id, cur_index, FALSE);
            }
            else
            {
                if (s_connection_setting_info.settinglink_num[dual_sys] + selected_num < MMICONNECTION_MAX_SETTING_NUM)
                {
                    GUILIST_SetSelectedItem(ctrl_id, cur_index, TRUE);
                }
            }

            selected_num = GUILIST_GetSelectedItemIndex(ctrl_id, PNULL, 0);

            is_res_data_exist = LoadNetworkAccountInfo(&res_info);

            if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
            {
                UnloadNetworkAccountInfo(&res_info);
                break;
            }

            for(i = 0; i<res_info.res_num; i++)
            {
                item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id, i);

                if (GUILIST_GetItemState(item_ptr, GUIITEM_STATE_SELECTED))
                {
                    middle_softkey_id = STXT_CANCEL;
                }
                else
                {
                    if (s_connection_setting_info.settinglink_num[dual_sys] + selected_num < MMICONNECTION_MAX_SETTING_NUM)
                    {
                        middle_softkey_id = STXT_SELECT;
                    }
                    else
                    {
                        middle_softkey_id = TXT_NULL;
                    }
                }

                SCI_MEMSET(&item_data, 0x00, sizeof(GUILIST_ITEM_DATA_T));
                SCI_MEMCPY(&item_data, item_ptr->item_data_ptr, sizeof(GUILIST_ITEM_DATA_T));
                item_data.softkey_id[1] = middle_softkey_id;

                SCI_MEMSET(&item, 0x00, sizeof(GUILIST_ITEM_T));
                SCI_MEMCPY(&item, item_ptr, sizeof(GUILIST_ITEM_T));
                item.item_data_ptr = &item_data;
                GUILIST_ReplaceItem(ctrl_id, &item, i);
            }

            UnloadNetworkAccountInfo(&res_info);

            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
        break;
#ifndef MMI_PDA_SUPPORT        
    case MSG_CTL_OK:
    case MSG_APP_OK:
        case MSG_APP_MENU:
        {
            const GUILIST_ITEM_T *item_ptr = NULL;
            BOOLEAN    modify_yes=FALSE;//对应项是否有改动
            BOOLEAN    update_yes=FALSE;//是否有变动，控制是否要写NV
            BOOLEAN is_need_adapt[MMICONNECTION_ACCOUNT_MAX] = {0};
            uint16 adapt_index = 0;

            if(s_connection_setting_info.settinglink_num[dual_sys] >= MMICONNECTION_MAX_SETTING_NUM)
            {
                MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_LISTFULL);
                break;
            }
            
            is_res_data_exist = LoadNetworkAccountInfo(&res_info);

            if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
            {
                UnloadNetworkAccountInfo(&res_info);
                break;
            }

            //Selete or deselect			
            for(;i<res_info.res_num;i++)
            {
                item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id,i);
                modify_yes=FALSE;
                if(GUILIST_GetItemState(item_ptr,GUIITEM_STATE_SELECTED))
                {
                    modify_yes=TRUE;
                    update_yes = TRUE;
                }
                if(modify_yes && s_connection_setting_info.settinglink_num[dual_sys]<MMICONNECTION_MAX_SETTING_NUM)
                {
                    uint16 index = s_connection_setting_info.settinglink_num[dual_sys];

                    SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][index],
                                                                &res_info.res_item_ptr[i], FALSE, TRUE, i);

                    MMICONNECTION_ClearHighPriList(dual_sys);
                    MMICONNECTION_SetHighPriList(dual_sys, (uint8)index);
                    s_connection_setting_info.settinglink_num[dual_sys] ++;

                    adapt_index = (uint16)res_info.res_item_ptr[i].account_type - (uint16)MMICONNECTION_ACCOUNT_BROWSER;
                    if (adapt_index < MMICONNECTION_ACCOUNT_MAX)
                    {
                        is_need_adapt[adapt_index] = TRUE;
                    }
                } //if(modify_yes)              
                
                if(s_connection_setting_info.settinglink_num[dual_sys] > MMICONNECTION_MAX_SETTING_NUM)
                {
                    break;
                }
            }//for(;i<res_info.res_num;i++)
            if(!update_yes)
            {
                uint16 index = s_connection_setting_info.settinglink_num[dual_sys];   
                
                i = GUILIST_GetCurItemIndex(ctrl_id);
                GUILIST_SetSelectedItem(ctrl_id, i, TRUE);

                SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][index],
                                                            &res_info.res_item_ptr[i], FALSE, TRUE, i);

                MMICONNECTION_ClearHighPriList(dual_sys);
                MMICONNECTION_SetHighPriList(dual_sys, index);
                s_connection_setting_info.settinglink_num[dual_sys] ++;
                update_yes = TRUE;

                adapt_index = (uint16)res_info.res_item_ptr[i].account_type - (uint16)MMICONNECTION_ACCOUNT_BROWSER;
                if (adapt_index < MMICONNECTION_ACCOUNT_MAX)
                {
                    is_need_adapt[adapt_index] = TRUE;
                }
            }

            MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

            for (i = 0; i < MMICONNECTION_ACCOUNT_MAX; i++)
            {
                if (is_need_adapt[i])
                {
                    MMICONNECTION_AdaptingAccount(dual_sys, (MMICONNECTION_ACCOUNT_TYPE_E)i);//默认列表中添加的帐户，允许做一次帐户匹配
                }
            }

            UnloadNetworkAccountInfo(&res_info);
            
            MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
            MMK_CloseWin(win_id);
        }
        break;	
#endif        
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
#if !defined MMI_GUI_STYLE_TYPICAL
        {
            const GUILIST_ITEM_T *item_ptr = NULL;
            BOOLEAN    modify_yes=FALSE;//对应项是否有改动
            BOOLEAN    update_yes=FALSE;//是否有变动，控制是否要写NV
            BOOLEAN is_need_adapt[MMICONNECTION_ACCOUNT_MAX] = {0};
            uint16 adapt_index = 0;

            if(s_connection_setting_info.settinglink_num[dual_sys] >= MMICONNECTION_MAX_SETTING_NUM)
            {
                MMK_CloseWin(win_id);
                break;
            }
            
            is_res_data_exist = LoadNetworkAccountInfo(&res_info);

            if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
            {
                UnloadNetworkAccountInfo(&res_info);
                break;
            }

            //Selete or deselect			
            for(;i<res_info.res_num;i++)
            {
                item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id,i);
                modify_yes=FALSE;
                if(GUILIST_GetItemState(item_ptr,GUIITEM_STATE_SELECTED))
                {
                    modify_yes=TRUE;
                    update_yes = TRUE;
                }
                if(modify_yes && s_connection_setting_info.settinglink_num[dual_sys]<MMICONNECTION_MAX_SETTING_NUM)
                {
                    uint16 index = s_connection_setting_info.settinglink_num[dual_sys];

                    SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][index],
                                                                &res_info.res_item_ptr[i], FALSE, TRUE, i);

                    MMICONNECTION_ClearHighPriList(dual_sys);
                    MMICONNECTION_SetHighPriList(dual_sys, (uint8)index);
                    s_connection_setting_info.settinglink_num[dual_sys] ++;

                    adapt_index = (uint16)res_info.res_item_ptr[i].account_type - (uint16)MMICONNECTION_ACCOUNT_BROWSER;
                    if (adapt_index < MMICONNECTION_ACCOUNT_MAX)
                    {
                        is_need_adapt[adapt_index] = TRUE;
                    }
                } //if(modify_yes)              
                
                if(s_connection_setting_info.settinglink_num[dual_sys] > MMICONNECTION_MAX_SETTING_NUM)
                {
                    break;
                }
            }//for(;i<res_info.res_num;i++)


            MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

            for (i = 0; i < MMICONNECTION_ACCOUNT_MAX; i++)
            {
                if (is_need_adapt[i])
                {
                    MMICONNECTION_AdaptingAccount(dual_sys, (MMICONNECTION_ACCOUNT_TYPE_E)i);//默认列表中添加的帐户，允许做一次帐户匹配
                }
            }

            UnloadNetworkAccountInfo(&res_info);            
    //        MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
            MMK_CloseWin(win_id);
        }
#else
        MMK_CloseWin(win_id);
#endif
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode ;
}


/*****************************************************************************/
//  Description :setting link list choose win handle fun
//  Global resource dependence : 
//  Author:jixin.xu
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleWapSettingListWinMsg(
                                         MMI_WIN_ID_T win_id, 
                                         MMI_MESSAGE_ID_E msg_id, 
                                         DPARAM param
                                           )
{
    MMI_RESULT_E err_code                                = MMI_RESULT_TRUE;
    uint16  link_index                                   = 0;
    MN_DUAL_SYS_E  dual_sys                              = MN_DUAL_SYS_MAX;
    uint8 index                                          = 0;
    uint16 sim_default_index                             = 0;
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_LIST_SETTING_CTRL_ID;
    uint32 add_data       = (uint32)MMK_GetWinAddDataPtr(win_id);

    dual_sys = (add_data)&0xFF;/*lint !e64*/
    sim_default_index = (add_data & 0xFF00) >> 8;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //动态添加LIST ITEM
        GUILIST_SetMaxItem( ctrl_id, MMICONNECTION_MAX_SETTING_NUM, FALSE );//max item 8
#ifndef MMI_PDA_SUPPORT
        GUILIST_SetOwnSofterKey(ctrl_id, TRUE);
#endif
        for(index = 0; index < s_connection_setting_info.settinglink_num[dual_sys]; index++)                              //显示LIST
        {
            MMI_IMAGE_ID_T image_id = IMAGE_CONNECTION_LINK_ITEM_RW;

            if(s_connection_setting_info.setting_detail[dual_sys][index].is_customed_detail)
            {
                image_id = IMAGE_CONNECTION_LINK_ITEM_RO;
            }

            AppendOneLineTextListItem(ctrl_id,
                GUIITEM_STYLE_ONE_LINE_RADIO_ICON,
                STXT_OPTION,
                image_id,
                s_connection_setting_info.setting_detail[dual_sys][index].name,
                s_connection_setting_info.setting_detail[dual_sys][index].name_len,
                index,
                FALSE
                );
        }
#ifdef MMI_PDA_SUPPORT
        GUIWIN_SetTitleButtonState(win_id, GUITITLE_BUTTON_RIGHT, TITLE_BUTTON_NORMAL, FALSE);
#endif
        GUILIST_SetSelectedItem(ctrl_id,sim_default_index,TRUE);
        GUILIST_SetCurItemIndex(ctrl_id,sim_default_index);	
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;
        
    case MSG_GET_FOCUS:
        {
            uint16 old_mark_num = 0;
            //MMI_IMAGE_ID_T image_id = IMAGE_CONNECTION_LINK_ITEM_RW;

            old_mark_num = GUILIST_GetTotalItemNum(ctrl_id);
            for(index = old_mark_num; index < s_connection_setting_info.settinglink_num[dual_sys]; index++)                             
            {
                MMI_IMAGE_ID_T image_id = IMAGE_CONNECTION_LINK_ITEM_RW;
                if(s_connection_setting_info.setting_detail[dual_sys][index].is_customed_detail)
                {
                    image_id = IMAGE_CONNECTION_LINK_ITEM_RO;
                }
                AppendOneLineTextListItem(ctrl_id,
                    GUIITEM_STYLE_ONE_LINE_RADIO_ICON,
                    STXT_OPTION,
                    image_id,
                    s_connection_setting_info.setting_detail[dual_sys][index].name,
                    s_connection_setting_info.setting_detail[dual_sys][index].name_len,
                    index,
                    FALSE
                    );
            }
//             {
//                 const GUILIST_ITEM_T    *item_ptr = PNULL;
//                 BOOLEAN is_select = FALSE;
//                 
//                 image_id = IMAGE_CONNECTION_LINK_ITEM_RW;
//                 link_index = GUILIST_GetCurItemIndex(ctrl_id);
//                 item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id,link_index);
//                 is_select = GUILIST_GetItemState(item_ptr, GUIITEM_STATE_SELECTED);
//                 if(s_connection_setting_info.setting_detail[dual_sys][link_index].is_customed_detail)
//                 {
//                     image_id = IMAGE_CONNECTION_LINK_ITEM_RO;
//                 }
//                 AppendOneLineTextListItem(ctrl_id,
//                     GUIITEM_STYLE_ONE_LINE_RADIO_ICON,
//                     STXT_OPTION,
//                     image_id,
//                     s_connection_setting_info.setting_detail[dual_sys][link_index].name,
//                     s_connection_setting_info.setting_detail[dual_sys][link_index].name_len,
//                     link_index,
//                     TRUE
//                     );
//                 if (is_select)
//                 {
//                     GUILIST_SetItemState(item_ptr, GUIITEM_STATE_SELECTED, TRUE);
//                 }
//             }
        }
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
        link_index = GUILIST_GetCurItemIndex(ctrl_id);
        GUILIST_SetSelectedItem(ctrl_id, link_index, TRUE);
        if (PNULL != s_connection_callback_func_ptr)
        {
            (*s_connection_callback_func_ptr)(dual_sys, link_index);
        }
        MMK_CloseWin(win_id);

        break;

    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_MENU:
        s_connection_handling_sim_id = dual_sys;
        MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLINKLISTMENU_WIN_TAB, PNULL);
        break;
#ifdef MMI_PDA_SUPPORT
    case MSG_CTL_LIST_LONGOK:
        s_connection_handling_sim_id = dual_sys;
        MMK_CreateWin((uint32 *)MMICONNECTION_LINKLIST_LONGOKMENU_WIN_TAB, PNULL);
        break;
#endif
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        //todo 关闭窗口
        MMK_CloseWin(win_id);
        break;

    default :
        err_code = MMI_RESULT_FALSE;
        break;
    }    
    
    return err_code;
}


#if !defined MMI_GUI_STYLE_TYPICAL
/*****************************************************************************/
// 	Description : Setting link menu
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL void  InitPdaSettingLinkListCtrl(MMICONNECTION_LINKSETTING_DETAIL_T* setting_detail)
{
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_MENU_SETTINGLINK_CTRL_ID;
    uint8 temp_str[MMICONNECT_STRNUM_MAX_LEN + 2] = {0};
    wchar temp_wstr[MMICONNECT_STRNUM_MAX_LEN + 2] = {0};
    MMI_STRING_T text_str= {0};
    MMI_TEXT_ID_T text_id = TXT_NULL;
    uint16 cur_index = GUILIST_GetCurItemIndex(ctrl_id);
    uint16 topItemIndex = GUILIST_GetTopItemIndex(ctrl_id);

    if(PNULL == setting_detail)    
    {
       //SCI_TRACE_LOW:"setting_detail is null"
       SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_3336_112_2_18_2_12_12_6,(uint8*)"");
       return;
    }
    //set max item
    GUILIST_SetMaxItem(ctrl_id, 10, FALSE );   
#ifndef MMI_DUALMODE_ENABLE
    if(0 == (uint16)setting_detail->auth_type)
    {
       text_id = TXT_COMMON_COMMON_NORMAL;
    }
    else if(1 == (uint16)setting_detail->auth_type)
    {
       text_id = TXT_COMM_SECURITY;    
    }
    MMI_GetLabelTextByLang(text_id, &text_str);      
    AppendPdaConnectionItem2Line(
    ctrl_id, 
    TXT_CONNECTION_AUTH_TYPE,
    text_str,
    IMAGE_COMMON_RIGHT_ARROW,
    (uint32)ID_CONNECTION_SETTING_SECRET
    );
#endif
    
    SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
    text_str.wstr_len = MIN(setting_detail->username_len, MMICONNECT_STRNUM_MAX_LEN);
    MMI_STRNTOWSTR(temp_wstr, MMICONNECT_STRNUM_MAX_LEN, setting_detail->username, text_str.wstr_len, text_str.wstr_len);
    text_str.wstr_ptr = temp_wstr;
    text_str.wstr_len = MMIAPICOM_Wstrlen(temp_wstr) ;
    AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_COMMON_USERNAME,
        text_str,
        IMAGE_NULL,
        (uint32)ID_CONNECTION_SETTING_USERNAME
        );

    {//display password as '*'
        uint16  i = 0;
        
        SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
        text_str.wstr_len = SCI_STRLEN((char*)(setting_detail->password)) ;
        text_str.wstr_len = MIN(text_str.wstr_len, MMICONNECT_STRNUM_MAX_LEN);
        for (i = 0; i < text_str.wstr_len; i++)
        {
            temp_wstr[i] = 0x2A;//*
        }
        text_str.wstr_ptr = temp_wstr;
        AppendPdaConnectionItem2Line(
            ctrl_id, 
            TXT_COMMON_PASSWORD,
            text_str,
            IMAGE_NULL,
            (uint32)ID_CONNECTION_SETTING_PASSWORD
            );
    }

    SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
    text_str.wstr_len = MIN(setting_detail->apn_len, MMICONNECT_STRNUM_MAX_LEN);
    MMI_STRNTOWSTR(temp_wstr, MMICONNECT_STRNUM_MAX_LEN, setting_detail->apn, text_str.wstr_len, text_str.wstr_len);
    text_str.wstr_ptr = temp_wstr;
    text_str.wstr_len = MMIAPICOM_Wstrlen(temp_wstr) ;
    AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_CONNECTION_APN,
        text_str,
        IMAGE_NULL,
        (uint32)ID_CONNECTION_SETTING_APN
        );

    SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
    text_str.wstr_len = MIN(setting_detail->gateway_len, MMICONNECT_STRNUM_MAX_LEN);
    MMI_STRNTOWSTR(temp_wstr, MMICONNECT_STRNUM_MAX_LEN, setting_detail->gateway, text_str.wstr_len, text_str.wstr_len);
    text_str.wstr_ptr = temp_wstr;
    text_str.wstr_len = MMIAPICOM_Wstrlen(temp_wstr) ;
    AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_COMMON_GATEWAY,
        text_str,
        IMAGE_NULL,
        (uint32)ID_CONNECTION_SETTING_GATEWAY
        );

    SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
    sprintf((char *)temp_str, "%d", setting_detail->port);
    text_str.wstr_len = SCI_STRLEN((char*)temp_str);
    text_str.wstr_len = MIN(text_str.wstr_len, MMICONNECT_STRNUM_MAX_LEN);
    MMI_STRNTOWSTR(temp_wstr, MMICONNECT_STRNUM_MAX_LEN, temp_str, text_str.wstr_len, text_str.wstr_len);
    text_str.wstr_ptr = temp_wstr;
    text_str.wstr_len = MMIAPICOM_Wstrlen(temp_wstr) ;
    AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_CONNECTION_PORT,
        text_str,
        IMAGE_NULL,
        (uint32)ID_CONNECTION_SETTING_PORT
        );

    SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
    text_str.wstr_ptr = setting_detail->name;
    text_str.wstr_len = MMIAPICOM_Wstrlen(setting_detail->name) ;
    AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_CONNECTION_CHANGENAME,
        text_str,
        IMAGE_NULL,
        (uint32)ID_CONNECTION_SETTING_CHANGENAME
        );

    SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
    text_str.wstr_len = MIN(setting_detail->homepage_len, MMICONNECT_STRNUM_MAX_LEN);
    MMI_STRNTOWSTR(temp_wstr, MMICONNECT_STRNUM_MAX_LEN, setting_detail->homepage, text_str.wstr_len, text_str.wstr_len);
    text_str.wstr_ptr = temp_wstr;
    text_str.wstr_len = MMIAPICOM_Wstrlen(temp_wstr) ;
    AppendPdaConnectionItem2Line(
        ctrl_id, 
        TXT_CONNECTION_SETHOMEPAGE,
        text_str,
        IMAGE_NULL,
        (uint32)ID_CONNECTION_SETTING_SETHOMEPAGE
        );

    AppendPdaConnectionItem1Line(            
        ctrl_id,
        TXT_CONNECTION_ADVANCE,
        (uint32)ID_CONNECTION_SETTING_ADVANCE
        );

    GUILIST_SetCurItemIndex(ctrl_id, cur_index);
    GUILIST_SetTopItemIndex(ctrl_id, topItemIndex);
}

/*****************************************************************************/
// 	Description : handle message of setting link list window of browser
//	Global resource dependence : none
//  Author: rong.gu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkMenuWinMsg(
                                               MMI_WIN_ID_T 	win_id, 
                                               MMI_MESSAGE_ID_E	msg_id, 
                                               DPARAM		    param
                                               )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint32          input_param=0;
    static  MMICONNECTION_LINKSETTING_DETAIL_T* temp_setting_detail=PNULL;
    uint16			list_index = 0;
    MMICONNECTION_LINKSET_OP_E op=MMICONNECTION_LINKSET_OP_NULL;
    CONNECTION_MMI_TEXT_ID_E	text_id[] = {TXT_CONNECTION_SETTING1,TXT_CONNECTION_SETTING2,TXT_CONNECTION_SETTING3,TXT_CONNECTION_SETTING4,TXT_CONNECTION_SETTING5,\
        TXT_CONNECTION_SETTING6,TXT_CONNECTION_SETTING7,TXT_CONNECTION_SETTING8,TXT_CONNECTION_SETTING9,TXT_CONNECTION_SETTING10,TXT_CONNECTION_SETTING11,TXT_CONNECTION_SETTING12,\
        TXT_CONNECTION_SETTING13,TXT_CONNECTION_SETTING14,TXT_CONNECTION_SETTING15,TXT_CONNECTION_SETTING16,TXT_CONNECTION_SETTING17,TXT_CONNECTION_SETTING18,TXT_CONNECTION_SETTING19,TXT_CONNECTION_SETTING20}; //wenbao.bai 2007.10.30
    MMI_STRING_T    string_info={0};
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }
       
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(MMICONNECTION_SETTINGLINKMENU_WIN_ID,MMICONNECTION_MENU_SETTINGLINK_CTRL_ID);
        temp_setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T*)SCI_ALLOCA(sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
        if(temp_setting_detail==PNULL)
        {
            MMIPUB_OpenAlertFailWin(TXT_COMMON_NO_MEMORY); 
            MMK_CloseWin(win_id);
        }
        else
        {
            input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
            op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
            list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
            SCI_MEMSET(temp_setting_detail,0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            
            if(op==MMICONNECTION_LINKSET_OP_EDIT || op==MMICONNECTION_LINKSET_OP_READ)
            {
                SCI_MEMCPY(temp_setting_detail,&s_connection_setting_info.setting_detail[dual_sys][list_index],sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            }
            else if(op==MMICONNECTION_LINKSET_OP_NEW)
            {
                uint32 i = 0;
                uint32 j = 0;
                uint16 text_num = sizeof(text_id)/sizeof(text_id[0]);
                BOOLEAN is_name_exist = FALSE;

                for (i = 0; i < text_num; i++)
                {
                    MMI_GetLabelTextByLang(text_id[i],&string_info);

                    string_info.wstr_len = MIN(string_info.wstr_len, MMICONNECTION_MAX_LINKMODENAME_LEN);

                    is_name_exist = FALSE;

                    for (j = 0; j < s_connection_setting_info.settinglink_num[dual_sys]; j++)
                    {
                        if (string_info.wstr_len == s_connection_setting_info.setting_detail[dual_sys][j].name_len
                            && 0 == memcmp(s_connection_setting_info.setting_detail[dual_sys][j].name, string_info.wstr_ptr, string_info.wstr_len*sizeof(wchar)))
                        {
                            is_name_exist = TRUE;
                            break;
                        }
                    }

                    if (!is_name_exist)
                    {
                        break;
                    }
                }

                if (!is_name_exist)
                {
                    MMIAPICOM_Wstrncpy(temp_setting_detail->name,string_info.wstr_ptr,string_info.wstr_len);
                    temp_setting_detail->name_len = string_info.wstr_len;
                }

                /* added by plum peng for cr125973 */
                temp_setting_detail->access_type = 1;
            }

            GUIWIN_SetTitleText(win_id, temp_setting_detail->name, temp_setting_detail->name_len, FALSE);
#ifdef MMI_PDA_SUPPORT
            GUIWIN_SetSoftkeyTextId(win_id,  STXT_SAVE, TXT_NULL, STXT_RETURN, FALSE);
#else
            GUIWIN_SetSoftkeyTextId(win_id,  STXT_SAVE, TXT_EDIT, STXT_RETURN, FALSE);
#endif
        }
        InitPdaSettingLinkListCtrl(temp_setting_detail);
        break;


    case MSG_GET_FOCUS:
        InitPdaSettingLinkListCtrl(temp_setting_detail);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        {
#ifdef MMI_PDA_SUPPORT
            MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

            if (MSG_CTL_PENOK == msg_id
                && MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID == ctrl_id)
            {
                MMK_SendMsg(win_id, MSG_CTL_OK, PNULL);
            }
            else if (MSG_CTL_PENOK == msg_id
                && MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID == ctrl_id)
            {
                MMK_SendMsg(win_id, MSG_CTL_CANCEL, PNULL);
            }
            else
            {
#endif
                uint16 index = 0;
                uint32 user_data  = 0;
                index = GUILIST_GetCurItemIndex(MMICONNECTION_MENU_SETTINGLINK_CTRL_ID);
                GUILIST_GetItemData(MMICONNECTION_MENU_SETTINGLINK_CTRL_ID, index, &user_data);
                                             
                switch((MMICONNECTION_ID_MENU_E)user_data)//菜单节点ID
                {
                    
                case ID_CONNECTION_SETTING_SECRET:
                    MMK_CreatePubListWin((uint32*)MMICONNECTION_SETTINGSECURITYCONNECTLIST_WIN_TAB, (ADD_DATA)temp_setting_detail);
                    break;
                    
                case ID_CONNECTION_SETTING_USERNAME://用户名:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTUSERNAME_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑用户名窗口
                    break;
                    
                case ID_CONNECTION_SETTING_PASSWORD://密码:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTPASSWORD_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑密码窗口
                    break;
                    
                case ID_CONNECTION_SETTING_APN://接入点名称:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTAPN_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑接入点名窗口
                    break;
                    
                case ID_CONNECTION_SETTING_GATEWAY://网关IP:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTGATEWAY_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑网关IP窗口
                    break;
                    
                case ID_CONNECTION_SETTING_PORT://网关端口:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_PORTLIST_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开选择网关端口窗口
                    break;
                    
                case ID_CONNECTION_SETTING_CHANGENAME://更改名称:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTAGENTNAME_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑设置名称窗口
                    break;

                case ID_CONNECTION_SETTING_SETHOMEPAGE:   //进行主页设置
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTHOMEPAGE_WIN_TAB, (ADD_DATA)temp_setting_detail);
                    break;

                case ID_CONNECTION_SETTING_ADVANCE:
                    MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLINK_ADVANCE_WIN_TAB, (ADD_DATA)temp_setting_detail);
                    break;

                default:
                    break;
                }
#ifdef MMI_PDA_SUPPORT
            }
#endif
        }
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
        op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
        list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引

        if(0 == memcmp(&s_connection_setting_info.setting_detail[dual_sys][list_index],temp_setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)))
        {
            MMK_CloseWin(win_id);
        }
        else
        {
            MMIPUB_OpenQueryWinByTextId(TXT_EXIT_QUERY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
        }
        break;
    case MSG_PROMPTWIN_OK:
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;
    case MSG_PROMPTWIN_CANCEL:
        MMIPUB_CloseQuerytWin(PNULL);
        break;
    case MSG_APP_OK:
    case MSG_CTL_OK:
        if(temp_setting_detail->name_len==0)
        {
            MMIPUB_OpenAlertWarningWin(TXT_ENVSET_EMPTY_NAME);
        }
        else
        {
            input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
            op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
            list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
            if(op!=MMICONNECTION_LINKSET_OP_READ)
            {
                if (0 != memcmp(&s_connection_setting_info.setting_detail[dual_sys][list_index],temp_setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)))
                {
                    SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][list_index],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                    SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][list_index],temp_setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                    s_connection_setting_info.setting_detail[dual_sys][list_index].is_default_unchange = FALSE;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].list_index = 0;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].hplmn = 0;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].account_type = MMICONNECTION_ACCOUNT_MAX;
                }
                if(op==MMICONNECTION_LINKSET_OP_NEW)
                {
                    s_connection_setting_info.settinglink_num[dual_sys]++;
                }

                MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

                if (MMICONNECTION_LINKSET_OP_EDIT == op)
                {
                    ChangeAppSelectedIndex(dual_sys, list_index, MMICONNECTION_NOTIFY_EVENT_EDIT_ITEM);
                }
            }
#ifndef MMI_PDA_SUPPORT            
            MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);            
#endif           
            MMK_CloseWin(win_id);
            MMK_CloseWin(MMICONNECTION_SETTINGLINKLISTMENU_WIN_ID);
            if (MMK_IsOpenWin(MMICONNECTION_LINKLIST_WIN_ID))
            {
                const GUILIST_ITEM_T *item_ptr = PNULL;
                BOOLEAN is_select              = FALSE;
                MMI_IMAGE_ID_T image_id        = IMAGE_CONNECTION_LINK_ITEM_RW;
                MMI_CTRL_ID_T ctrl_id          = MMICONNECTION_LIST_SETTING_CTRL_ID;
                uint16  link_index             = 0;

                link_index = GUILIST_GetCurItemIndex(ctrl_id);
                if (link_index < MMICONNECTION_MAX_SETTING_NUM)
                {
                    item_ptr = GUILIST_GetItemPtrByIndex(ctrl_id,link_index);
                    is_select = GUILIST_GetItemState(item_ptr, GUIITEM_STATE_SELECTED);
                    if(s_connection_setting_info.setting_detail[dual_sys][link_index].is_customed_detail)
                    {
                        image_id = IMAGE_CONNECTION_LINK_ITEM_RO;
                    }
                    AppendOneLineTextListItem(ctrl_id,
                        GUIITEM_STYLE_ONE_LINE_RADIO_ICON,
                        STXT_OPTION,
                        image_id,
                        s_connection_setting_info.setting_detail[dual_sys][link_index].name,
                        s_connection_setting_info.setting_detail[dual_sys][link_index].name_len,
                        link_index,
                        TRUE
                        );
                    if (is_select)
                    {
                        GUILIST_SetItemState((GUILIST_ITEM_T *)item_ptr, GUIITEM_STATE_SELECTED, TRUE);
                    }
                }

            }
        }
        break;
    case MSG_CLOSE_WINDOW:
        if(temp_setting_detail!=PNULL)
        {
            SCI_FREE(temp_setting_detail);
            temp_setting_detail = PNULL;
        }
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode; 
}

#else
/*****************************************************************************/
// 	Description : handle message of setting link list window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkMenuWinMsg(
                                               MMI_WIN_ID_T 	win_id, 
                                               MMI_MESSAGE_ID_E	msg_id, 
                                               DPARAM		    param
                                               )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint32          input_param=0;
    static  MMICONNECTION_LINKSETTING_DETAIL_T* temp_setting_detail=PNULL;
    uint32			group_id = 0;
    uint32			menu_id = 0;
    uint16			list_index = 0;
    MMICONNECTION_LINKSET_OP_E op=MMICONNECTION_LINKSET_OP_NULL;
    CONNECTION_MMI_TEXT_ID_E	text_id[] = {TXT_CONNECTION_SETTING1,TXT_CONNECTION_SETTING2,TXT_CONNECTION_SETTING3,TXT_CONNECTION_SETTING4,TXT_CONNECTION_SETTING5,\
        TXT_CONNECTION_SETTING6,TXT_CONNECTION_SETTING7,TXT_CONNECTION_SETTING8,TXT_CONNECTION_SETTING9,TXT_CONNECTION_SETTING10,TXT_CONNECTION_SETTING11,TXT_CONNECTION_SETTING12,\
        TXT_CONNECTION_SETTING13,TXT_CONNECTION_SETTING14,TXT_CONNECTION_SETTING15,TXT_CONNECTION_SETTING16,TXT_CONNECTION_SETTING17,TXT_CONNECTION_SETTING18,TXT_CONNECTION_SETTING19,TXT_CONNECTION_SETTING20}; //wenbao.bai 2007.10.30
    MMI_STRING_T    string_info={0};
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }
       
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(MMICONNECTION_SETTINGLINKMENU_WIN_ID,MMICONNECTION_MENU_SETTINGLINK_CTRL_ID);
        temp_setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T*)SCI_ALLOCA(sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
        if(temp_setting_detail==PNULL)
        {
            MMIPUB_OpenAlertFailWin(TXT_COMMON_NO_MEMORY); 
            MMK_CloseWin(win_id);
        }
        else
        {
            input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
            op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
            list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
            SCI_MEMSET(temp_setting_detail,0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            
            if(op==MMICONNECTION_LINKSET_OP_EDIT || op==MMICONNECTION_LINKSET_OP_READ)
            {
                SCI_MEMCPY(temp_setting_detail,&s_connection_setting_info.setting_detail[dual_sys][list_index],sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
            }
            else if(op==MMICONNECTION_LINKSET_OP_NEW)
            {
                uint32 i = 0;
                uint32 j = 0;
                uint16 text_num = sizeof(text_id)/sizeof(text_id[0]);
                BOOLEAN is_name_exist = FALSE;

                for (i = 0; i < text_num; i++)
                {
                    MMI_GetLabelTextByLang(text_id[i],&string_info);

                    string_info.wstr_len = MIN(string_info.wstr_len, MMICONNECTION_MAX_LINKMODENAME_LEN);

                    is_name_exist = FALSE;

                    for (j = 0; j < s_connection_setting_info.settinglink_num[dual_sys]; j++)
                    {
                        if (string_info.wstr_len == s_connection_setting_info.setting_detail[dual_sys][j].name_len
                            && 0 == memcmp(s_connection_setting_info.setting_detail[dual_sys][j].name, string_info.wstr_ptr, string_info.wstr_len*sizeof(wchar)))
                        {
                            is_name_exist = TRUE;
                            break;
                        }
                    }

                    if (!is_name_exist)
                    {
                        break;
                    }
                }

                if (!is_name_exist)
                {
                    MMIAPICOM_Wstrncpy(temp_setting_detail->name,string_info.wstr_ptr,string_info.wstr_len);
                    temp_setting_detail->name_len = string_info.wstr_len;
                }

                /* added by plum peng for cr125973 */
                temp_setting_detail->access_type = 1;
            }

            
#ifdef MMI_PDA_SUPPORT
            GUIWIN_SetSoftkeyTextId(win_id,  STXT_SAVE, TXT_NULL, STXT_RETURN, FALSE);
#else
            GUIWIN_SetSoftkeyTextId(win_id,  STXT_SAVE, TXT_EDIT, STXT_RETURN, FALSE);
#endif
        }
        break;
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        {
#ifdef MMI_PDA_SUPPORT
            MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

            if (MSG_CTL_PENOK == msg_id
                && MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID == ctrl_id)
            {
                MMK_SendMsg(win_id, MSG_CTL_OK, PNULL);
            }
            else if (MSG_CTL_PENOK == msg_id
                && MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID == ctrl_id)
            {
                MMK_SendMsg(win_id, MSG_CTL_CANCEL, PNULL);
            }
            else
            {
#endif
                GUIMENU_GetId(MMICONNECTION_MENU_SETTINGLINK_CTRL_ID, (MMI_MENU_GROUP_ID_T *)(&group_id),(MMI_MENU_ID_T *) (&menu_id));  
                switch(menu_id)//菜单节点ID
                {
                    
                case ID_CONNECTION_SETTING_SECRET:
                    MMK_CreatePubListWin((uint32*)MMICONNECTION_SETTINGSECURITYCONNECTLIST_WIN_TAB, (ADD_DATA)temp_setting_detail);
                    break;
                    
                case ID_CONNECTION_SETTING_USERNAME://用户名:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTUSERNAME_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑用户名窗口
                    break;
                    
                case ID_CONNECTION_SETTING_PASSWORD://密码:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTPASSWORD_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑密码窗口
                    break;
                    
                case ID_CONNECTION_SETTING_APN://接入点名称:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTAPN_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑接入点名窗口
                    break;
                    
                case ID_CONNECTION_SETTING_GATEWAY://网关IP:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTGATEWAY_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑网关IP窗口
                    break;
                    
                case ID_CONNECTION_SETTING_PORT://网关端口:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_PORTLIST_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开选择网关端口窗口
                    break;
                    
                case ID_CONNECTION_SETTING_CHANGENAME://更改名称:
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTAGENTNAME_WIN_TAB, (ADD_DATA)temp_setting_detail);//打开编辑设置名称窗口
                    break;

                case ID_CONNECTION_SETTING_SETHOMEPAGE:   //进行主页设置
                    MMK_CreatePubEditWin((uint32*)MMICONNECTION_INPUTHOMEPAGE_WIN_TAB, (ADD_DATA)temp_setting_detail);
                    break;

                case ID_CONNECTION_SETTING_ADVANCE:
                    MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLINK_ADVANCE_WIN_TAB, (ADD_DATA)temp_setting_detail);
                    break;

                default:
                    break;
                }
#ifdef MMI_PDA_SUPPORT
            }
#endif
        }
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
        op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
        list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引

        //是否修改, ADD_NEW is modified
        if(0 == memcmp(&s_connection_setting_info.setting_detail[dual_sys][list_index],temp_setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)))
        {
            MMK_CloseWin(win_id);
        }
        else
        {
            MMIPUB_OpenQueryWinByTextId(TXT_EXIT_QUERY,IMAGE_PUBWIN_QUERY,PNULL,PNULL);
        }
        break;
    case MSG_PROMPTWIN_OK:
        MMIPUB_CloseQuerytWin(PNULL);
        MMK_CloseWin(win_id);
        break;
    case MSG_PROMPTWIN_CANCEL:
        MMIPUB_CloseQuerytWin(PNULL);
        break;
    case MSG_APP_OK:
    case MSG_CTL_OK:
        if(temp_setting_detail->name_len==0)
        {
            MMIPUB_OpenAlertWarningWin(TXT_ENVSET_EMPTY_NAME);
        }
        else
        {
            input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
            op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
            list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
            if(op!=MMICONNECTION_LINKSET_OP_READ)
            {
                if (0 != memcmp(&s_connection_setting_info.setting_detail[dual_sys][list_index],temp_setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)))
                {
                    SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][list_index],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                    SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][list_index],temp_setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                    s_connection_setting_info.setting_detail[dual_sys][list_index].is_default_unchange = FALSE;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].list_index = 0;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].hplmn = 0;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].account_type = MMICONNECTION_ACCOUNT_MAX;
                }
                if(op==MMICONNECTION_LINKSET_OP_NEW)
                {
                    s_connection_setting_info.settinglink_num[dual_sys]++;
                }

                MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);
                
            }
            MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);            
            MMK_CloseWin(win_id);
            MMK_CloseWin(MMICONNECTION_SETTINGLINKLISTMENU_WIN_ID);
        }
        break;
    case MSG_CLOSE_WINDOW:
        if(temp_setting_detail!=PNULL)
        {
            SCI_FREE(temp_setting_detail);
            temp_setting_detail = PNULL;
        }
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode; 
}
#endif

/*****************************************************************************/
// 	Description : handle message of setting link list window of browser
//	Global resource dependence : none
//  Author: 
//	Note:   
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkDetailViewWinMsg(
                                                     MMI_WIN_ID_T	win_id, 
                                                     MMI_MESSAGE_ID_E	msg_id, 
                                                     DPARAM				param
                                                     )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T   ctrl_id = MMICONNECTION_RICHTEXT_SETTINGLINKDETAILVIEW_CTRL_ID;
    uint16			list_index = 0;
    uint32          input_param=0;
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }

    input_param = (uint32)MMK_GetWinAddDataPtr(win_id);
    list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        SetSettingDetailEntry(ctrl_id, &s_connection_setting_info.setting_detail[dual_sys][list_index]);
        GUIRICHTEXT_SetLineSpace(ctrl_id, MMICONNECTION_DETAIL_LINE_SPACE);
        GUIWIN_SetSoftkeyTextId(win_id,  TXT_NULL, TXT_NULL, STXT_RETURN, FALSE);
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;

    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}


/*****************************************************************************/
// 	Description : handle message of setting security connect list window of browser
//	Global resource dependence : none
//  Author: 
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingSecurityconnectListWinMsg(
                                                          MMI_WIN_ID_T  	win_id, 
                                                          MMI_MESSAGE_ID_E	msg_id, 
                                                          DPARAM			param
                                                          )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint16			list_index = 0;
    uint16          i=0;
    MMI_CTRL_ID_T   ctrl_id = MMICONNECTION_LIST_SETTINGSECURITYCONNECT_CTRL_ID;
    CONNECTION_MMI_TEXT_ID_E	text_id[2] = {TXT_COMMON_COMMON_NORMAL, TXT_COMM_SECURITY};/*lint !e64*/
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail=(MMICONNECTION_LINKSETTING_DETAIL_T*)MMK_GetWinAddDataPtr(win_id);
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMK_SetAtvCtrl(MMICONNECTION_SETTINGSECURITYCONNECTLIST_WIN_ID,MMICONNECTION_LIST_SETTINGSECURITYCONNECT_CTRL_ID);
        list_index = (uint16)setting_detail->auth_type;
        //动态添加LIST ITEM
        GUILIST_SetMaxItem( ctrl_id,2,FALSE );//max item 2
        for(i = 0;i<2;i++)
        {
#if !defined MMI_GUI_STYLE_TYPICAL
            MMIAPISET_AppendListItemByTextId(text_id[i], TXT_NULL, ctrl_id, GUIITEM_STYLE_ONE_LINE_RADIO );
#else
            MMIAPISET_AppendListItemByTextId(text_id[i], TXT_COMMON_OK, ctrl_id, GUIITEM_STYLE_ONE_LINE_RADIO );
#endif
        }
        GUILIST_SetSelectedItem(ctrl_id,list_index,TRUE);
        GUILIST_SetCurItemIndex(ctrl_id,list_index);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        list_index = GUILIST_GetCurItemIndex(ctrl_id);
        setting_detail->auth_type = (uint32)list_index;//(MN_PCO_AUTH_TYPE_E)list_index;
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:	
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}


/*****************************************************************************/
// 	Description : handle message of input username window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputUserNameWinMsg(
                                             MMI_WIN_ID_T	win_id, 
                                             MMI_MESSAGE_ID_E	msg_id, 
                                             DPARAM				param
                                             )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_STRING_T string_info = {0};
    MMI_CTRL_ID_T   ctrl_id = MMICONNECTION_EDIT_INPUTUSERNAME_CTRL_ID;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail=(MMICONNECTION_LINKSETTING_DETAIL_T*)MMK_GetWinAddDataPtr(win_id);
    wchar ucs2buf[MMICONNECTION_MAX_USERNAME_LEN+1] = {0};
    uint16 ucs2buf_len = 0;
    GUIIM_TYPE_T allow_im = GUIIM_TYPE_NONE;
    GUIIM_TYPE_T init_im = GUIIM_TYPE_NONE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //set im
        allow_im = GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL;/*lint !e655*/
        init_im = GUIIM_TYPE_ABC;
        GUIEDIT_SetIm(ctrl_id, allow_im, init_im);
        MMIAPICOM_StrToWstr((uint8 *)setting_detail->username, ucs2buf);
        //填入缺省值
        GUIEDIT_SetString(ctrl_id,
            ucs2buf,
            setting_detail->username_len);
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        //保存用户名
        GUIEDIT_GetString(ctrl_id,&string_info);
        SCI_MEMSET(setting_detail->username,0,sizeof(setting_detail->username));
        MMIAPICOM_WstrToStr(string_info.wstr_ptr, (uint8 *)ucs2buf);//值允许输入ASCII码，可以用这个函数
        ucs2buf_len = strlen((char*)ucs2buf);
        SCI_MEMCPY(setting_detail->username,(uint8*)ucs2buf,ucs2buf_len);
        setting_detail->username_len = strlen((char*)ucs2buf);
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:		
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}


/*****************************************************************************/
// 	Description : handle message of input password window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputPasswordWinMsg(
                                             MMI_WIN_ID_T	    win_id, 
                                             MMI_MESSAGE_ID_E	msg_id, 
                                             DPARAM				param
                                             )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_STRING_T   string_info = {0};
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T*)MMK_GetWinAddDataPtr(win_id);
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUTPASSWORD_CTRL_ID;
    wchar tempbuf[MMICONNECTION_MAX_PASSWORD_LEN+1] = {0}; 
    uint16 tempbuf_len = 0;
    BOOLEAN is_disp_im_icon = TRUE;
    BOOLEAN is_disp_num_info = TRUE;
    GUIIM_TYPE_T allow_im = GUIIM_TYPE_NONE;
    GUIIM_TYPE_T init_im = GUIIM_TYPE_NONE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //set im
        allow_im = GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL;/*lint !e655*/
        init_im = GUIIM_TYPE_ABC;
        GUIEDIT_SetIm(ctrl_id,allow_im,init_im);

        GUIEDIT_SetStyle(ctrl_id, GUIEDIT_STYLE_MULTI);

        GUIEDIT_SetDispImIcon(ctrl_id, &is_disp_im_icon, &is_disp_num_info);

        MMIAPICOM_StrToWstr((uint8 *)setting_detail->password, tempbuf);
        //填入缺省值
        GUIEDIT_SetString(ctrl_id,tempbuf,setting_detail->password_len);
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;

    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        //保存密码
        GUIEDIT_GetString(ctrl_id,&string_info);
        MMIAPICOM_WstrToStr(string_info.wstr_ptr, (uint8 *)tempbuf);//值允许输入ASCII，可以用这个函数
        SCI_MEMSET(setting_detail->password,0,sizeof(setting_detail->password));
        tempbuf_len = strlen((char*)tempbuf);
        SCI_MEMCPY(setting_detail->password,(uint8*)tempbuf,tempbuf_len);
        setting_detail->password_len = strlen((char*)tempbuf);
        MMK_CloseWin(win_id);
        break;
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:
        
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}


/*****************************************************************************/
// 	Description : handle message of input apn window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputApnWinMsg(
                                        MMI_WIN_ID_T	win_id,
                                        MMI_MESSAGE_ID_E	msg_id,
                                        DPARAM				param
                                        )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_STRING_T string_info = {0};
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUTAPN_CTRL_ID;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T*)MMK_GetWinAddDataPtr(win_id);
    wchar tempbuf[MMICONNECTION_MAX_APN_LEN+1] = {0};
    uint16 tempbuf_len = 0;
    GUIIM_TYPE_T allow_im = GUIIM_TYPE_NONE;
    GUIIM_TYPE_T init_im = GUIIM_TYPE_NONE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //set im
        allow_im = GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL;/*lint !e655*/
        init_im = GUIIM_TYPE_ABC;
        GUIEDIT_SetIm(ctrl_id,allow_im,init_im);
        MMIAPICOM_StrToWstr((uint8 *)setting_detail->apn, tempbuf); 
        //填入缺省值
        GUIEDIT_SetString(ctrl_id,tempbuf,setting_detail->apn_len);												
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        //保存接入点名称
        GUIEDIT_GetString(ctrl_id,&string_info);
        MMIAPICOM_WstrToStr(string_info.wstr_ptr, (uint8 *)tempbuf);//只允许输入ASCII，可以用这个函数
        SCI_MEMSET(setting_detail->apn,0,sizeof(setting_detail->apn));
        tempbuf_len = strlen((char*)tempbuf);
        SCI_MEMCPY(setting_detail->apn,(uint8*)tempbuf,tempbuf_len);
        setting_detail->apn_len = strlen((char*)tempbuf);
        MMK_CloseWin(win_id);
        break;    
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:
        
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}


/*****************************************************************************/
// 	Description : handle message of input gateway window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputGatewayWinMsg(
                                            MMI_WIN_ID_T	win_id, 
                                            MMI_MESSAGE_ID_E	msg_id, 
                                            DPARAM				param
                                            )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T*)MMK_GetWinAddDataPtr(win_id);
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUTGATEWAY_CTRL_ID;
#ifdef MMI_HTTP_PROXY_IP_ONLY_SUPPORT
    uint32 temp =0;
#else
    MMI_STRING_T string_info = {0};
    wchar tempbuf[MMICONNECTION_MAX_GATEWAY_LEN + 1] = {0};
    uint16 tempbuf_len = 0;
    GUIIM_TYPE_T allow_im = GUIIM_TYPE_NONE;
    GUIIM_TYPE_T init_im = GUIIM_TYPE_NONE;
#endif
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
#ifdef MMI_HTTP_PROXY_IP_ONLY_SUPPORT
        MMIAPICONNECTION_IpStrToNum(setting_detail->gateway, &temp);
        GUIEDIT_SetIP (ctrl_id,temp);
#else
        allow_im = GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL;/*lint !e655*/
        init_im = GUIIM_TYPE_ABC;

        GUIEDIT_SetIm(ctrl_id,allow_im,init_im);

        MMIAPICOM_StrToWstr((uint8 *)setting_detail->gateway, tempbuf); 

        GUIEDIT_SetString(ctrl_id, tempbuf, setting_detail->gateway_len);
#endif

        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
#ifdef MMI_HTTP_PROXY_IP_ONLY_SUPPORT
        temp = GUIEDIT_GetIP(ctrl_id);
        //change uint32 to string
        MMIAPICONNECTION_IpNumToStr(temp, (uint8 *)&setting_detail->gateway);
        setting_detail->gateway_len = SCI_STRLEN((char*)setting_detail->gateway);
#else
        GUIEDIT_GetString(ctrl_id, &string_info);
        MMIAPICOM_WstrToStr(string_info.wstr_ptr, (uint8 *)tempbuf);//只允许输入ASCII，可以用这个函数
        SCI_MEMSET(setting_detail->gateway, 0, sizeof(setting_detail->gateway));
        tempbuf_len = strlen((char*)tempbuf);
        SCI_MEMCPY(setting_detail->gateway,(uint8*)tempbuf,tempbuf_len);
        setting_detail->gateway_len = strlen((char*)tempbuf);
#endif
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:	
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}


/*****************************************************************************/
// 	Description : handle message of port list window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePortListWinMsg(
                                        MMI_WIN_ID_T	    win_id, 
                                        MMI_MESSAGE_ID_E	msg_id, 
                                        DPARAM				param
                                        )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint32          port_value = 0;
    uint8           port_str[MMICONNECTION_MAX_PORT_LEN+2] = {0};
    wchar           port_str2[MMICONNECTION_MAX_PORT_LEN+2] = {0};
    uint8			port_len = 0;
    MMI_STRING_T string_info= {0};
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T*)MMK_GetWinAddDataPtr(win_id);
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUTPORT_CTRL_ID;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //填入缺省值
        //if(setting_detail->port!=0)
        {
            _snprintf((char*)port_str, (MMICONNECTION_MAX_PORT_LEN+1),"%d", setting_detail->port);
            port_len=strlen((char*)port_str);
            MMIAPICOM_StrToWstr((uint8 *)port_str, port_str2);
            GUIEDIT_SetString(ctrl_id,port_str2,port_len);
        }        	
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        GUIEDIT_GetString(ctrl_id,&string_info);
        if(0 == string_info.wstr_len)
        {
            _snprintf((char*)port_str, (MMICONNECTION_MAX_PORT_LEN+1),"%d", setting_detail->port);
            port_len=strlen((char*)port_str);
            MMIAPICOM_StrToWstr((uint8 *)port_str, port_str2);
            GUIEDIT_SetString(ctrl_id,port_str2,port_len);
            MMIPUB_OpenAlertWarningWin(TXT_CONNECTION_PORT_EMPTY);
            break;
        }
        MMIAPICOM_WstrToStr(string_info.wstr_ptr, (uint8 *)port_str2);//只允许输入数字，可以用这个函数
        port_value = atoi((char*)port_str2);
        if(MMICONNECTION_MAX_PORT_VALUE < port_value)
        {
            _snprintf((char*)port_str, (MMICONNECTION_MAX_PORT_LEN+1),"%d", setting_detail->port);
            port_len=strlen((char*)port_str);
            MMIAPICOM_StrToWstr((uint8 *)port_str, port_str2);
            GUIEDIT_SetString(ctrl_id,port_str2,port_len);
            MMIPUB_OpenAlertWarningWin(TXT_INVAILD_DEFLECTED_NUMBER); 
        }
        else
        {
            setting_detail->port = (uint16)port_value;
            MMK_CloseWin(win_id);
        }
        
        
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:
        
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}


/*****************************************************************************/
// 	Description : handle message of input agent name window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputAgentNameWinMsg(
                                              MMI_WIN_ID_T  	win_id, 
                                              MMI_MESSAGE_ID_E	msg_id, 
                                              DPARAM			param
                                              )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_STRING_T string_info = {0};
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUTAGENTNAME_CTRL_ID;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T *)MMK_GetWinAddDataPtr(win_id);
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUIEDIT_SetString(ctrl_id,setting_detail->name,setting_detail->name_len);
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        GUIEDIT_GetString(ctrl_id,&string_info);
        SCI_MEMSET(setting_detail->name,0,sizeof(setting_detail->name));
        MMIAPICOM_Wstrncpy(setting_detail->name,string_info.wstr_ptr,string_info.wstr_len);
        setting_detail->name_len = string_info.wstr_len;
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:	
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}

/*****************************************************************************/
// 	Description : handle message of input homepage window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleInputHomepageWinMsg(
                                             MMI_WIN_ID_T	    win_id, 
                                             MMI_MESSAGE_ID_E	msg_id, 
                                             DPARAM				param
                                             )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_STRING_T string_info = {0};
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail =(MMICONNECTION_LINKSETTING_DETAIL_T *)MMK_GetWinAddDataPtr(win_id);
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUTHOMEPAGE_CTRL_ID;
    wchar tempbuf[MMICONNECTION_HOMEPAGE_MAX_URL_LEN+1]= {0};
    uint16 tempbuf_len = 0;
    GUIIM_TYPE_T allow_im = GUIIM_TYPE_NONE;
    GUIIM_TYPE_T init_im = GUIIM_TYPE_NONE;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        //set im
        allow_im = GUIIM_TYPE_ENGLISH | GUIIM_TYPE_ABC | GUIIM_TYPE_DIGITAL;/*lint !e655*/
        init_im = GUIIM_TYPE_ABC;
        GUIEDIT_SetIm(ctrl_id,allow_im,init_im);
        GUIEDIT_SetImTag(ctrl_id, GUIIM_TAG_URL);
        MMIAPICOM_StrToWstr((uint8 *)setting_detail->homepage, tempbuf);
        GUIEDIT_SetString(ctrl_id,tempbuf,setting_detail->homepage_len);
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        GUIEDIT_GetString(ctrl_id,&string_info);
        MMIAPICOM_WstrToStr(string_info.wstr_ptr, (uint8 *)tempbuf);//只允许输入ASCII，可以用这个函数
        SCI_MEMSET(setting_detail->homepage,0,sizeof(setting_detail->homepage));
        tempbuf_len = strlen((char*)tempbuf);
        SCI_MEMCPY(setting_detail->homepage,tempbuf,tempbuf_len);
        setting_detail->homepage_len=strlen((char *)tempbuf);
        MMK_CloseWin(win_id);
        break;    
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:	
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}


#if !defined MMI_GUI_STYLE_TYPICAL
/*****************************************************************************/
// 	Description : Connection setting advance
//	Global resource dependence : 
//  Author: rong.gu 
//	Note: 
/*****************************************************************************/
LOCAL void  InitPdaSettingAdvanceCtrl(MMICONNECTION_LINKSETTING_DETAIL_T* setting_detail)
{
    MMI_CTRL_ID_T       ctrl_id = MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID;
    MMI_STRING_T text_str= {0};
    MMI_TEXT_ID_T    text_id = TXT_NULL;
    wchar temp_wstr[MMICONNECT_STRNUM_MAX_LEN + 2] = {0};
    uint16 cur_index = GUILIST_GetCurItemIndex(ctrl_id);

    //set max item
    GUILIST_SetMaxItem(ctrl_id, 2, FALSE );   
    SCI_MEMSET(&text_str, 0x00, sizeof(text_str));
    MMIAPICOM_StrToWstr((uint8 *)setting_detail->dns, temp_wstr); 
    text_str.wstr_ptr = temp_wstr;
    text_str.wstr_len = MMIAPICOM_Wstrlen(temp_wstr) ;
    AppendPdaConnectionItem2Line(
    ctrl_id, 
    TXT_CONNECTION_DNS,
    text_str,
    IMAGE_NULL,
    (uint32)ID_CONNECTION_SETTING_DNS
    );

    SCI_MEMSET(&text_str, 0x00, sizeof(text_str));    
    if(0 == setting_detail->access_type)
    {
        text_id = TXT_CONNECTION_NETTYPE_WAP;
    }
    else
    {
        text_id = TXT_CONNECTION_NETTYPE_HTTP;
    }
    MMI_GetLabelTextByLang(text_id, &text_str);    
    AppendPdaConnectionItem2Line(
    ctrl_id, 
    TXT_NETWORK_TYPE,
    text_str,
    IMAGE_COMMON_RIGHT_ARROW,
    (uint32)ID_CONNECTION_SETTING_NETTYPE
    );

    GUILIST_SetCurItemIndex(ctrl_id, cur_index);
}


/*****************************************************************************/
// 	Description : handle message of setting link advance window using list for pda
//	Global resource dependence : none
//  Author: rong.gu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkAdvanceWinMsg(
                                                  MMI_WIN_ID_T  	win_id, 
                                                  MMI_MESSAGE_ID_E	msg_id, 
                                                  DPARAM			param
                                                  )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T *)MMK_GetWinAddDataPtr(win_id);
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX /*&& MSG_CLOSE_WINDOW != msg_id*/)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUIWIN_SetSoftkeyTextId(win_id,  STXT_SAVE, TXT_NULL, STXT_RETURN, FALSE);        
        InitPdaSettingAdvanceCtrl(setting_detail);
        MMK_SetAtvCtrl(win_id, MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID);
        break;
        
    case MSG_GET_FOCUS:
        InitPdaSettingAdvanceCtrl(setting_detail);
        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        {
            MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T*) param)->src_id;

            if (MSG_CTL_PENOK == msg_id
                && MMICOMMON_BUTTON_SOFTKEY_LEFT_CTRL_ID == ctrl_id)
            {
                MMK_SendMsg(win_id, MSG_CTL_OK, PNULL);
            }
            else if (MSG_CTL_PENOK == msg_id
                && MMICOMMON_BUTTON_SOFTKEY_RIGHT_CTRL_ID == ctrl_id)
            {
                MMK_SendMsg(win_id, MSG_CTL_CANCEL, PNULL);
            }
            else
            {
                uint16 index = 0;
                uint32 user_data  = 0;
                index = GUILIST_GetCurItemIndex(MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID);
                GUILIST_GetItemData(MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID, index, &user_data);
                              
                switch((MMICONNECTION_ID_MENU_E)user_data)
                {
                case ID_CONNECTION_SETTING_DNS:
                    MMK_CreatePubEditWin((uint32 *)MMICONNECTION_SETTINGLINK_DNS_WIN_TAB, MMK_GetWinAddDataPtr(win_id));
                    break;
                case ID_CONNECTION_SETTING_NETTYPE:
                    MMK_CreatePubListWin((uint32 *)MMICONNECTION_SETTINGLINK_NETTYPE_WIN_TAB, MMK_GetWinAddDataPtr(win_id));
                    break;
                default:
                    break;
                }    
            }
        }
        break;
        
    case MSG_APP_OK:    
    case MSG_CTL_OK:
        if(setting_detail->name_len==0)
        {
            MMIPUB_OpenAlertWarningWin(TXT_ENVSET_EMPTY_NAME);
        }
        else
        {
            uint16 list_index = 0;
            uint32 input_param=0;            
            MMICONNECTION_LINKSET_OP_E op=MMICONNECTION_LINKSET_OP_NULL;
            
            input_param = (uint32)MMK_GetWinAddDataPtr(MMICONNECTION_SETTINGLINKMENU_WIN_ID);
            op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
            list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
            if(op!=MMICONNECTION_LINKSET_OP_READ)
            {
                if (0 != memcmp(&s_connection_setting_info.setting_detail[dual_sys][list_index],setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)))
                {
                    SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][list_index],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                    SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][list_index],setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                    s_connection_setting_info.setting_detail[dual_sys][list_index].is_default_unchange = FALSE;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].list_index = 0;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].hplmn = 0;
                    s_connection_setting_info.setting_detail[dual_sys][list_index].account_type = MMICONNECTION_ACCOUNT_MAX;
                }
                if(op==MMICONNECTION_LINKSET_OP_NEW)
                {
                    s_connection_setting_info.settinglink_num[dual_sys]++;
                }
                MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);
                
            }            
            MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);            
            MMK_CloseWin(win_id);
            MMK_CloseWin(MMICONNECTION_SETTINGLINKMENU_WIN_ID);
            MMK_CloseWin(MMICONNECTION_SETTINGLINKLISTMENU_WIN_ID);
        }
        break;
            
        case MSG_CTL_CANCEL:        
            MMK_CloseWin(win_id);
            break;   
        default:	
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}


#else
/*****************************************************************************/
// 	Description : handle message of setting link advance window of browser
//	Global resource dependence : none
//  Author: figo.feng
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkAdvanceWinMsg(
                                                  MMI_WIN_ID_T  	win_id, 
                                                  MMI_MESSAGE_ID_E	msg_id, 
                                                  DPARAM			param
                                                  )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    uint32			group_id = 0;
    uint32			menu_id = 0;
    MMI_STRING_T title = {0};
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T *)MMK_GetWinAddDataPtr(win_id);
    MN_DUAL_SYS_E dual_sys = s_connection_handling_sim_id;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        //MMK_CloseWin(win_id);
        return MMI_RESULT_FALSE;
    }

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMI_GetLabelTextByLang(TXT_CONNECTION_ADVANCE, &title);
        GUIMENU_SetMenuTitle(&title, MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID);
        MMK_SetAtvCtrl(win_id, MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID);
        break;
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
        GUIMENU_GetId(MMICONNECTION_MENU_SETTINGLINK_ADVANCE_CTRL_ID, (MMI_MENU_GROUP_ID_T *)(&group_id),(MMI_MENU_ID_T *) (&menu_id));  
        switch(menu_id)
        {
        case ID_CONNECTION_SETTING_DNS:
            MMK_CreatePubEditWin((uint32 *)MMICONNECTION_SETTINGLINK_DNS_WIN_TAB, MMK_GetWinAddDataPtr(win_id));
            break;
        case ID_CONNECTION_SETTING_NETTYPE:
            MMK_CreatePubListWin((uint32 *)MMICONNECTION_SETTINGLINK_NETTYPE_WIN_TAB, MMK_GetWinAddDataPtr(win_id));
            break;
        default:
            break;
        }    
        break;
        case MSG_APP_OK:    
        case MSG_CTL_OK:
            if(setting_detail->name_len==0)
            {
                MMIPUB_OpenAlertWarningWin(TXT_ENVSET_EMPTY_NAME);
            }
            else
            {
                uint16 list_index = 0;
                uint32 input_param=0;            
                MMICONNECTION_LINKSET_OP_E op=MMICONNECTION_LINKSET_OP_NULL;
                
                input_param = (uint32)MMK_GetWinAddDataPtr(MMICONNECTION_SETTINGLINKMENU_WIN_ID);
                op=(MMICONNECTION_LINKSET_OP_E )(input_param>>16);
                list_index = input_param&0xFFFF;//低16位表示菜单中的list_index，也就是全局变量的索引
                if(op!=MMICONNECTION_LINKSET_OP_READ)
                {
                    if (0 != memcmp(&s_connection_setting_info.setting_detail[dual_sys][list_index],setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)))
                    {
                        SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys][list_index],0,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                        SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][list_index],setting_detail,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
                        s_connection_setting_info.setting_detail[dual_sys][list_index].is_default_unchange = FALSE;
                        s_connection_setting_info.setting_detail[dual_sys][list_index].list_index = 0;
                        s_connection_setting_info.setting_detail[dual_sys][list_index].hplmn = 0;
                        s_connection_setting_info.setting_detail[dual_sys][list_index].account_type = MMICONNECTION_ACCOUNT_MAX;
                    }
                    if(op==MMICONNECTION_LINKSET_OP_NEW)
                    {
                        s_connection_setting_info.settinglink_num[dual_sys]++;
                    }
                    MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);
                    
                }            
                MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);            
                MMK_CloseWin(win_id);
                MMK_CloseWin(MMICONNECTION_SETTINGLINKMENU_WIN_ID);
                MMK_CloseWin(MMICONNECTION_SETTINGLINKLISTMENU_WIN_ID);
            }
            break;
            
        case MSG_CTL_CANCEL:        
            MMK_CloseWin(win_id);
            break;   
        default:	
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}
#endif

/*****************************************************************************/
// 	Description : handle message of setting dns window of browser
//	Global resource dependence : none
//  Author: bei.wu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkDNSWinMsg(
                                              MMI_WIN_ID_T	win_id, 
                                              MMI_MESSAGE_ID_E	msg_id, 
                                              DPARAM				param
                                              )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_EDIT_INPUT_DNS_CTRL_ID;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T *)MMK_GetWinAddDataPtr(win_id);
    uint32 temp =0;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        MMIAPICONNECTION_IpStrToNum(setting_detail->dns, &temp);
        GUIEDIT_SetIP(ctrl_id,temp);
        MMK_SetAtvCtrl(win_id,ctrl_id);
        break;
        
    case MSG_CTL_MIDSK:
    case MSG_APP_OK:
    case MSG_CTL_OK:
    case MSG_CTL_PENOK:
    case MSG_APP_WEB:
        temp = GUIEDIT_GetIP(ctrl_id);
        //change the uint32 to string
        MMIAPICONNECTION_IpNumToStr(temp, (uint8 *)&setting_detail->dns);
        MMK_CloseWin(win_id);
        break;
        
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:	
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
    
}

/*****************************************************************************/
// 	Description : handle message of setting net type window of browser
//	Global resource dependence : none
//  Author: bei.wu
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleSettingLinkNetTypeWinMsg(
												 MMI_WIN_ID_T	    win_id, 
												 MMI_MESSAGE_ID_E	msg_id, 
												 DPARAM				param
												 )
{
    MMI_RESULT_E	recode = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T ctrl_id = MMICONNECTION_LIST_SELECT_NETTYPE_CTRL_ID;
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_detail = (MMICONNECTION_LINKSETTING_DETAIL_T *)MMK_GetWinAddDataPtr(win_id);
    uint16 list_index = 0;
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUILIST_SetMaxItem(ctrl_id, 2, FALSE);
#ifdef MMI_PDA_SUPPORT
        MMIAPISET_AppendListItemByTextId(TXT_CONNECTION_NETTYPE_WAP, TXT_COMMON_OK, ctrl_id, GUIITEM_STYLE_ONE_LINE_RADIO);
        MMIAPISET_AppendListItemByTextId(TXT_CONNECTION_NETTYPE_HTTP, TXT_COMMON_OK, ctrl_id, GUIITEM_STYLE_ONE_LINE_RADIO);
#else
        MMIAPISET_AppendListItemByTextId(TXT_CONNECTION_NETTYPE_WAP, TXT_NULL, ctrl_id, GUIITEM_STYLE_ONE_LINE_RADIO);
        MMIAPISET_AppendListItemByTextId(TXT_CONNECTION_NETTYPE_HTTP, TXT_NULL, ctrl_id, GUIITEM_STYLE_ONE_LINE_RADIO);
#endif
        GUILIST_SetSelectedItem(ctrl_id, setting_detail->access_type,TRUE);
        GUILIST_SetCurItemIndex(ctrl_id, setting_detail->access_type);
        MMK_SetAtvCtrl(win_id, ctrl_id);
        break;
        case MSG_CTL_MIDSK:
    case MSG_APP_OK:
    case MSG_CTL_OK:
    case MSG_CTL_PENOK:
    case MSG_APP_WEB:
        list_index = GUILIST_GetCurItemIndex(ctrl_id);
        setting_detail->access_type = list_index;
        MMK_CloseWin(win_id);
        break;
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);//返回上一级
        break;
        
    default:	
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

/*****************************************************************************/
// 	Description : set the select list of the searchresult list box to high priority
//	Global resource dependence : 
//  Author:wenbao.bai
//	Note: 
/****************************************************************************/
LOCAL void MMICONNECTION_SetHighPriList(MN_DUAL_SYS_E dual_sys, uint8 list_index)
{
    if (list_index >= MMICONNECTION_MAX_SETTING_NUM || dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }
    
    s_connection_setting_info.setting_detail[dual_sys][list_index].priority = TRUE;
}
/*****************************************************************************/
// 	Description :clear the priority of the setting link lists box
//	Global resource dependence : 
//  Author:wenbao.bai
//	Note: 
/****************************************************************************/
LOCAL void MMICONNECTION_ClearHighPriList(MN_DUAL_SYS_E dual_sys)
{
    uint8 i = 0;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }
    
    for(i = 0;i<s_connection_setting_info.settinglink_num[dual_sys]; i ++)
    {
        s_connection_setting_info.setting_detail[dual_sys][i].priority = FALSE;
    }
}

#ifndef MMI_MULTI_SIM_SYS_SINGLE
/*****************************************************************************/
//     Description : to handle window message
//    Global resource dependence : 
//  Author:wancan.you
//    Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E  ConnectionSelectSimCallback(uint32 dual_sys, BOOLEAN isReturnOK, DPARAM param)
{
    MMI_RESULT_E recode  = MMI_RESULT_TRUE;
    if (isReturnOK)
    {
        s_connection_handling_sim_id = (MN_DUAL_SYS_E)dual_sys;

        MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLIST_WIN_TAB, PNULL);
    }
    return recode;
}
#endif
#endif

/*****************************************************************************/
// 	Description : 连接列表修改语言
//	Global resource dependence : 
//  Author: 
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_ChangeSettingListLanguage(void)
{
#ifdef MMI_GPRS_SUPPORT
    uint16 i = 0;
    uint32 dual_sys = 0;
    BOOLEAN is_res_data_exist = FALSE;
    MMICONNECTION_RES_INFO_T res_info = {0};

    is_res_data_exist = LoadNetworkAccountInfo(&res_info);

    if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
    {
        UnloadNetworkAccountInfo(&res_info);
        return;
    }

    //SCI_TRACE_LOW:"MMIAPICONNECTION_ChangeSettingListLanguage"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_4987_112_2_18_2_12_15_7,(uint8*)"" );

    for (dual_sys = 0; dual_sys < MMI_DUAL_SYS_MAX; dual_sys++)
    {
        for(i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)
        {
            if (s_connection_setting_info.setting_detail[dual_sys][i].is_default_unchange)
            {
                uint16 j = s_connection_setting_info.setting_detail[dual_sys][i].list_index;

                if (j < res_info.res_num)
                {
                    s_connection_setting_info.setting_detail[dual_sys][i].name_len = MIN(res_info.res_item_ptr[j].name_len, MMICONNECTION_MAX_LINKMODENAME_LEN);
                    MMIAPICOM_Wstrncpy(s_connection_setting_info.setting_detail[dual_sys][i].name,res_info.res_item_ptr[j].name,s_connection_setting_info.setting_detail[dual_sys][i].name_len);
                }
            }
        }
    }
    MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

    UnloadNetworkAccountInfo(&res_info);
#endif
}

/*****************************************************************************/
// 	Description : 连接列表重置
//	Global resource dependence : 
//  Author: 
//	Note:
/*****************************************************************************/
PUBLIC void MMICONNECTION_ResetSettingList(BOOLEAN is_clean_user_data)
{
#ifdef MMI_GPRS_SUPPORT
    uint16 i = 0;
    uint16 reset_list_num = 0;
    uint16 custom_list_num = LINK_CUSTOMED_LEN;
    uint32 dual_sys = 0;
    MMICONNECTION_RES_INFO_T res_info = {0};
    BOOLEAN is_res_data_exist = FALSE;
    const uint8 *customed_index_ptr = PNULL;

    //SCI_TRACE_LOW:"MMICONNECTION_ResetSettingList is_clean_user_data=%d, settinglink_num=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5026_112_2_18_2_12_16_8,(uint8*)"dd", is_clean_user_data, s_connection_setting_info.settinglink_num[dual_sys]);

    is_res_data_exist = LoadNetworkAccountInfo(&res_info);

    if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
    {
        UnloadNetworkAccountInfo(&res_info);
        return;
    }

    if (res_info.is_res_data)
    {
        custom_list_num = LINK_CUSTOMED_RES_LEN;
        customed_index_ptr = (const uint8 *)s_customed_res_index;
    }
    else
    {
        custom_list_num = LINK_CUSTOMED_LEN;
        customed_index_ptr = (const uint8 *)s_customed_index;
    }
    
    custom_list_num = MIN(custom_list_num, res_info.res_num);
    custom_list_num = MIN(custom_list_num, MMICONNECTION_MAX_SETTING_NUM);

    if (is_clean_user_data)
    {
        reset_list_num = MMICONNECTION_MAX_SETTING_NUM;
        for (dual_sys = 0; dual_sys < MMI_DUAL_SYS_MAX; dual_sys++)
        {
            s_connection_setting_info.settinglink_num[dual_sys] = custom_list_num;
            SCI_MEMSET(s_connection_setting_info.setting_detail[dual_sys], 0, (sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)*reset_list_num));
        }    
    }
    
    for (dual_sys = 0; dual_sys < MMI_DUAL_SYS_MAX; dual_sys++)
    {
        for(i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)
        {
            if (is_clean_user_data)
            {
                uint16 j = customed_index_ptr[i];
                BOOLEAN is_customed_detail = FALSE;

                if (j >= res_info.res_num)
                {
                    break;
                }

                if (MMICONNECTION_ACCESS_READONLY == res_info.res_item_ptr[j].access_option)
                {
                    is_customed_detail = TRUE;
                }
                else
                {
                    is_customed_detail = FALSE;
                }

                SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][i],
                                                            &res_info.res_item_ptr[j], is_customed_detail, TRUE, j);

                //MMICONNECTION_SetHighPriList(dual_sys, (uint8)i);
            }
            else if (s_connection_setting_info.setting_detail[dual_sys][i].is_default_unchange)
            {
                uint16 j = s_connection_setting_info.setting_detail[dual_sys][i].list_index;

                if (j < res_info.res_num && PNULL != res_info.res_item_ptr[j].name && 0 != res_info.res_item_ptr[j].name_len)
                {
                    s_connection_setting_info.setting_detail[dual_sys][i].name_len = MIN(res_info.res_item_ptr[j].name_len, MMICONNECTION_MAX_LINKMODENAME_LEN);
                    MMIAPICOM_Wstrncpy(s_connection_setting_info.setting_detail[dual_sys][i].name,res_info.res_item_ptr[j].name,s_connection_setting_info.setting_detail[dual_sys][i].name_len);
                }
            }
        }
    }

    MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

    UnloadNetworkAccountInfo(&res_info);
#endif
}

/*****************************************************************************/
// 	Description : 连接列表重置
//	Global resource dependence : 
//  Author: 
//	Note:
/*****************************************************************************/
PUBLIC void MMICONNECTION_ResetSettingListEx(MN_DUAL_SYS_E dual_sys, BOOLEAN is_clean_user_data)
{
#ifdef MMI_GPRS_SUPPORT
    uint16 i = 0;
    uint16 reset_list_num = 0;
    uint16 custom_list_num = LINK_CUSTOMED_LEN;
    BOOLEAN is_res_data_exist = FALSE;
    MMICONNECTION_RES_INFO_T res_info = {0};
    const uint8 *customed_index_ptr = PNULL;

    is_res_data_exist = LoadNetworkAccountInfo(&res_info);

    //SCI_TRACE_LOW:"MMICONNECTION_ResetSettingList is_clean_user_data=%d, settinglink_num=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5123_112_2_18_2_12_16_9,(uint8*)"dd", is_clean_user_data, s_connection_setting_info.settinglink_num[dual_sys]);

    if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
    {
        UnloadNetworkAccountInfo(&res_info);
        return;
    }

    if (res_info.is_res_data)
    {
        custom_list_num = LINK_CUSTOMED_RES_LEN;
        customed_index_ptr = (const uint8 *)s_customed_res_index;
    }
    else
    {
        custom_list_num = LINK_CUSTOMED_LEN;
        customed_index_ptr = (const uint8 *)s_customed_index;
    }
    
    custom_list_num = MIN(custom_list_num, res_info.res_num);
    custom_list_num = MIN(custom_list_num, MMICONNECTION_MAX_SETTING_NUM);

    if (is_clean_user_data)
    {
        reset_list_num = MMICONNECTION_MAX_SETTING_NUM;

        s_connection_setting_info.settinglink_num[dual_sys] = custom_list_num;
        SCI_MEMSET(s_connection_setting_info.setting_detail[dual_sys], 0, (sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)*reset_list_num));
    }
    
    for(i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)
    {
        if (is_clean_user_data)
        {
            uint16 j = customed_index_ptr[i];
            BOOLEAN is_customed_detail = FALSE;

            if (j >= res_info.res_num)
            {
                break;
            }

            if (MMICONNECTION_ACCESS_READONLY == res_info.res_item_ptr[j].access_option)
            {
                is_customed_detail = TRUE;
            }
            else
            {
                is_customed_detail = FALSE;
            }

            SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][i],
                                                        &res_info.res_item_ptr[j], is_customed_detail, TRUE, j);

            //MMICONNECTION_SetHighPriList(dual_sys, (uint8)i);
        }
        else if (s_connection_setting_info.setting_detail[dual_sys][i].is_default_unchange)
        {
            uint16 j = s_connection_setting_info.setting_detail[dual_sys][i].list_index;

            if (j < res_info.res_num)
            {
                s_connection_setting_info.setting_detail[dual_sys][i].name_len = MIN(res_info.res_item_ptr[j].name_len, MMICONNECTION_MAX_LINKMODENAME_LEN);
                MMIAPICOM_Wstrncpy(s_connection_setting_info.setting_detail[dual_sys][i].name,res_info.res_item_ptr[j].name,s_connection_setting_info.setting_detail[dual_sys][i].name_len);
            }
        }
    }

    MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

    UnloadNetworkAccountInfo(&res_info);
#endif
}
/*****************************************************************************/
// 	Description : open connection menu
//	Global resource dependence : 
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_OpenMenuWin(void)
{
    MMI_WIN_ID_T    win_id = MMICONNECTION_MENU_WIN_ID;
    MMI_CTRL_ID_T   ctrl_id = MMICONNECTION_MENU_CTRL_ID;
#ifdef MMI_TVOUT_ENABLE
    MMI_TEXT_ID_T   label = TXT_NULL;
#endif
    GUI_BOTH_RECT_T both_rect = {0};
    BOOLEAN is_grayed = FALSE;
    MMI_STRING_T    title_str = {0};    
    uint32  i = 0;
    MMI_TEXT_ID_T text_id = TXT_NULL;
    
    MMK_CloseWin(win_id);

    MMK_CreateWin((uint32 *)MMICONNECTION_MAIN_MENU_WIN_TAB,PNULL);

    both_rect = MMITHEME_GetWinClientBothRect(win_id);
    //creat dynamic menu
    GUIMENU_CreatDynamic(&both_rect, win_id, ctrl_id, GUIMENU_STYLE_THIRD);
    MMI_GetLabelTextByLang(TXT_SETTINGS_LINK, &title_str);
    GUIMENU_SetMenuTitle(&title_str, ctrl_id);

#ifdef BLUETOOTH_SUPPORT
    if(GUIMENU_ISTYLE != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_CUBE != MMIAPISET_GetMainmenuStyle()
        #ifdef PDA_UI_SUPPORT_MANIMENU_GO
		&& GUIMENU_STYLE_GO != MMIAPISET_GetMainmenuStyle()
		#else
        &&GUIMENU_STYLE_SLIDE_PAGE != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_SPHERE != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_CYLINDER != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_TRAPEZOID != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_BINARY_STAR != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_SHUTTER != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_CHARIOT != MMIAPISET_GetMainmenuStyle()
        #endif
        )
    {
        InsertNodeByLabel(
            ctrl_id,//ctrl_id
            TXT_BLUETOOTH,//text_ptr 
            ID_CONNECTION_BLUETOOTH,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_LINKSET_BT,
            is_grayed
            );
    }
#endif 

#ifdef MMI_TVOUT_ENABLE
    if(TRUE == MMITHEME_IsTVOpen())
    {
        label = TXT_CONNECTION_EXIT_TVOUT;
    }
    else
    {
        label = TXT_CONNECTION_OPEN_TVOUT;
    }

    InsertNodeByLabel(
            ctrl_id,//ctrl_id
            label,//text_id 
            ID_CONNECTION_OPEN_TV_OUT,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_LINKSET_TV,
            is_grayed
            ); 

#endif

#ifdef MMI_GPRS_SUPPORT
    InsertNodeByLabel(
            ctrl_id,//ctrl_id
            TXT_COMMON_WWW_ACCOUNT,//text_ptr 
            ID_CONNECTION_NET_USERID,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_LINKSET_NET,
            is_grayed
            ); 

#ifdef MMI_GPRS_SWITCH_SUPPORT

    InsertNodeByLabel(
            ctrl_id,//ctrl_id
            TXT_GPRS_SERVICE,//text_ptr 
            ID_CONNECTION_GRPS_SWITCH,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_LINKSET_NET,
            FALSE
            ); 

    
#endif

#ifdef MMI_GPRS_SWITCH_SUPPORT
    if ((MMIAPICONNECTION_GetGPRSSwitchStatus() == MMICONNECTION_SETTING_GPRS_SWITCH_OFF)||(MMIAPISET_GetMultiSysSettingNum(PNULL, 0) > 1))
#else
    if (MMIAPISET_GetMultiSysSettingNum(PNULL, 0) > 1)
#endif    
    {
        is_grayed = TRUE;
    }
    else
    {
        is_grayed = FALSE;
    }

    s_connection_index = i;

    InsertNodeByLabel(
            ctrl_id,//ctrl_id
            TXT_DATA_CONNECTION_SET,//text_ptr 
            ID_CONNECTION_GPRS,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_CONNECTION_GPRS,
            is_grayed
            ); 
    //////////////////////////////////////////////////////////////////////////
    // added by feng.xiao
#ifdef  DATA_ROAMING_SUPPORT
        InsertNodeByLabel(ctrl_id,//ctrl_id
                          TXT_CONNECTION_DATA_ROAMING,//text_ptr 
                          ID_CONNECTION_DATA_ROAMING,//node_id
                          0, //parent_node_id
                          (uint16)i++,//index
                          IMAGE_SECMENU_ICON_CONNECTION_GPRS,
                          FALSE); 
    #endif
#endif

/* BEGIN: Added by George.Liu 01526, 2010/7/23   PN:MergeToMw1029forCustom */
#ifdef WIFI_SUPPORT
    if (GUIMENU_ISTYLE != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_CUBE != MMIAPISET_GetMainmenuStyle()
        #ifdef PDA_UI_SUPPORT_MANIMENU_GO
		&& GUIMENU_STYLE_GO != MMIAPISET_GetMainmenuStyle()
		#else
        && GUIMENU_STYLE_SLIDE_PAGE != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_SPHERE != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_CYLINDER != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_TRAPEZOID != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_BINARY_STAR != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_SHUTTER != MMIAPISET_GetMainmenuStyle()
        && GUIMENU_STYLE_CHARIOT != MMIAPISET_GetMainmenuStyle()
        #endif
        )
    {
        InsertNodeByLabel(
                ctrl_id,//ctrl_id
                TXT_WIFI,
                ID_CONNECTION_WIFI,//node_id
                0, //parent_node_id
                (uint16)i++,//index
                IMAGE_SECMENU_ICON_LINKSET_WIFI,
                FALSE
                ); 
    }
#endif

#ifdef MMI_PREFER_PLMN_SUPPORT
    text_id = TXT_NETWORK_SETTING;
#else
    text_id = TXT_NETWORK_SELECT;
#endif

#ifdef MMI_DUALMODE_ENABLE
 	InsertNodeByLabel(
            ctrl_id,//ctrl_id
            text_id,//text_ptr 
            ID_CONNECTION_SET_NETSETTING,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_CALLSET_NETSET,
            FALSE
            ); 
#else
    InsertNodeByLabel(
            ctrl_id,//ctrl_id
            text_id,//text_ptr 
            ID_CONNECTION_SET_NETWORK,//node_id
            0, //parent_node_id
            (uint16)i++,//index
            IMAGE_SECMENU_ICON_CALLSET_NETSEL,
            FALSE
            ); 
#endif

/* END:   Added by George.Liu 01526, 2010/7/23   PN:MergeToMw1029forCustom */

}

/*****************************************************************************/
// 	Description : close connection menu
//	Global resource dependence : 
//  Author: ryan.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_CloseMenuWin(void)
{
	MMK_CloseWin(MMICONNECTION_MENU_WIN_ID);
}

#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
// 	Description : open setting list window of connection
//	Global resource dependence : none
//  Author:
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_OpenSettingListWin(void)
{
    uint16 sim_num = 0;
    uint16 sim_ok = 0;

    sim_num = MMIAPIPHONE_GetSimCardOkNum(&sim_ok, 1);

    //SCI_TRACE_LOW:"[MMICONNECTION]:MMIAPICONNECTION_OpenSettingListWin sim_ok=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5398_112_2_18_2_12_16_10,(uint8*)"d", sim_ok);

    if (0 == sim_num)
    {
        MMIAPIPHONE_AlertSimNorOKStatus();
    }
    else if (1 == sim_num)
    {
        s_connection_handling_sim_id = (MN_DUAL_SYS_E)sim_ok;

        MMK_CreateWin((uint32 *)MMICONNECTION_SETTINGLIST_WIN_TAB, PNULL);
    }
#ifndef MMI_MULTI_SIM_SYS_SINGLE
    else
    {
        //MMK_CreateWin((uint32 *)MMICONNECTION_SELECT_SIM_WIN_TAB, PNULL);
        MMICONNECTION_SelectSIMFunc();
    }
#endif
}
#endif

/*****************************************************************************/
// 	Description : open setting list window of connection
//	Global resource dependence : none
//  Author: Ying.Xu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_OpenGprsWin(void)
{
    //SCI_TRACE_LOW("[MMICONNECTION]:MMIAPICONNECTION_OpenGprsWin enter");
    //打开设置列表窗口  
#ifdef MMI_GPRS_SUPPORT
    MMK_CreatePubListWin((uint32*)MMICONNECTION_GPRS_SETTINGS_WIN_TAB, PNULL);    
#endif
}

#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
//     Description : Handle SIM Plug Notify Connection
//    Global resource dependence : 
//  Author: wancan.you
//    Note:
/*****************************************************************************/
LOCAL void HandleSIMPlugNotifyConnectionFunc(MN_DUAL_SYS_E dual_sys, MMIPHONE_SIM_PLUG_EVENT_E notify_event, uint32 param)
{
    //SCI_TRACE_LOW:"HandleSIMPlugNotifyConnectionFunc dual_sys=%d, notify_event=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5440_112_2_18_2_12_16_11,(uint8*)"dd", dual_sys, notify_event);

    switch (notify_event)
    {
    case MMIPHONE_NOTIFY_SIM_PLUG_IN:
    case MMIPHONE_NOTIFY_SIM_PLUG_OUT:
        if (dual_sys == s_connection_handling_sim_id)
        {
            MMK_CloseWin(MMICONNECTION_SETTINGLIST_WIN_ID);
        }

#ifndef MMI_MULTI_SIM_SYS_SINGLE
        MMK_CloseWin(MMICONNECTION_COM_SELECT_SIM_WIN_ID);
#endif
        break;

    default:
        break;
    }
}
#endif

/*****************************************************************************/
// 	Description : init connection
//	Global resource dependence : none
//  Author:
//	Note:
/*****************************************************************************/
PUBLIC void MMICONNECTION_Init(void)
{
#ifdef MMI_GPRS_SUPPORT
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    MMIPHONE_SIM_PLUG_NOTIFY_INFO_T sim_plug_notify_info = {0};

    MMINV_READ(MMINV_CONNECTION_SETTING_INFO,&s_connection_setting_info,return_value);

    if(MN_RETURN_SUCCESS != return_value)
    {
        //SCI_TRACE_LOW:"[MMICONNECTION]:MMICONNECTION_Init setting read Failed"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5476_112_2_18_2_12_16_12,(uint8*)"");

        MMICONNECTION_ResetSettingList(TRUE);//设固定设置
    }

#ifdef MMI_GPRS_SWITCH_SUPPORT
    if (MMICONNECTION_SETTING_GPRS_SWITCH_OFF==MMIAPICONNECTION_GetGPRSSwitchStatus())
    {
        s_gprs_turned_off = TRUE;
    }
    else
    {
        s_gprs_turned_off = FALSE;
    }
#endif    
    sim_plug_notify_info.module_id = MMI_MODULE_CONNECTION;
    sim_plug_notify_info.notify_func = HandleSIMPlugNotifyConnectionFunc;
    MMIAPIPHONE_RegisterSIMPlugNotifyInfo(&sim_plug_notify_info);
    
#ifdef DATA_ACCOUNT_IN_TCARD_SUPPORT
    MMICONNECTION_RegBrowserCB();
#endif    
#endif
}

/*****************************************************************************/
// 	Description : Get link setting item by index
//	Global resource dependence : none
//  Author:
//	Note:
/*****************************************************************************/
PUBLIC MMICONNECTION_LINKSETTING_DETAIL_T* MMIAPICONNECTION_GetLinkSettingItemByIndex(MN_DUAL_SYS_E dual_sys, uint8 index)
{
#ifdef MMI_GPRS_SUPPORT
    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return PNULL;
    }

    if (index >= s_connection_setting_info.settinglink_num[dual_sys])
    {
        index= 0;
    }

    return &s_connection_setting_info.setting_detail[dual_sys][index];
#else
    return PNULL;
#endif
}

/*****************************************************************************/
// 	Description : Get link setting num
//	Global resource dependence : none
//  Author:
//	Note:
/*****************************************************************************/
PUBLIC uint8 MMIAPICONNECTION_GetLinkSettingNum(MN_DUAL_SYS_E dual_sys)
{
#ifdef MMI_GPRS_SUPPORT
    if (dual_sys < MMI_DUAL_SYS_MAX)
    {
        return s_connection_setting_info.settinglink_num[dual_sys];
    }

    return 0;
#else
    return 0;
#endif
}


/*****************************************************************************/
// 	Description : set homepage by index
//	Global resource dependence : none
//  Author: aoke.hu
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_SetHomepage(
                                        MN_DUAL_SYS_E dual_sys,
                                         uint8 index,
                                         const MMI_STRING_T *string_ptr
                                         )
{
    BOOLEAN return_val = FALSE;
#ifdef MMI_GPRS_SUPPORT
    MMICONNECTION_LINKSETTING_DETAIL_T *setting_item_ptr = PNULL;

    if (PNULL == string_ptr)
    {
        return return_val;
    }

    setting_item_ptr = MMIAPICONNECTION_GetLinkSettingItemByIndex(dual_sys, index);
    if(setting_item_ptr
        &&!setting_item_ptr->is_customed_detail
        && string_ptr->wstr_len > 0
        && string_ptr->wstr_len <= MMICONNECTION_HOMEPAGE_MAX_URL_LEN)
    {
        MMIAPICOM_WstrToStr(string_ptr->wstr_ptr, (uint8 *)setting_item_ptr->homepage);
        setting_item_ptr->homepage_len = string_ptr->wstr_len;
        setting_item_ptr->is_default_unchange = FALSE;
        MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);
        return_val = TRUE;
    }
#endif
    return return_val;
}

/*****************************************************************************/
// 	Description : 新增一项LIST
//	Global resource dependence : none
//  Author: aoke.hu
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_AddNewItem(
                                        MN_DUAL_SYS_E dual_sys,
                                        const MMICONNECTION_LINKSETTING_DETAIL_T *setting_item_ptr
                                        )
{
    BOOLEAN return_val = FALSE;
#ifdef MMI_GPRS_SUPPORT
    uint16 list_index = 0;

    if(PNULL != setting_item_ptr
        && dual_sys < MMI_DUAL_SYS_MAX
        && s_connection_setting_info.settinglink_num[dual_sys] < MMICONNECTION_MAX_SETTING_NUM)
    {
        list_index = s_connection_setting_info.settinglink_num[dual_sys];
        SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][list_index],setting_item_ptr,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
        s_connection_setting_info.settinglink_num[dual_sys]++; 
        MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);
        return_val = TRUE;
    }
#endif
    return return_val;
}

/*****************************************************************************/
// 	Description : 替代原有一项LIST的内容
//	Global resource dependence : none
//  Author: aoke.hu
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_ReplaceItem(
                                         MN_DUAL_SYS_E dual_sys,
                                         uint16 list_index,
                                         const MMICONNECTION_LINKSETTING_DETAIL_T *setting_item_ptr
                                         )
{
    BOOLEAN return_val = FALSE;

#ifdef MMI_GPRS_SUPPORT
    if (PNULL == setting_item_ptr || dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return return_val;
    }

    if(list_index < s_connection_setting_info.settinglink_num[dual_sys])
    {
        SCI_MEMCPY(&s_connection_setting_info.setting_detail[dual_sys][list_index],setting_item_ptr,sizeof(MMICONNECTION_LINKSETTING_DETAIL_T));
        MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);
        return_val = TRUE;
    }

#endif
    return return_val;
}

/*****************************************************************************/
// 	Description : 注册
//	Global resource dependence : none
//  Author: aoke.hu
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPICONNECTION_RegisterNotifyInfo(MMICONNECTION_NOTIFY_INFO_T *notify_info_ptr)
{
    BOOLEAN return_val = FALSE;
    MMICONNECTION_NOTIFY_LIST_T *cursor = PNULL;

    if(PNULL != notify_info_ptr)
    {
        if(PNULL == s_subscriber_list_ptr)
        {
            s_subscriber_list_ptr = SCI_ALLOC_APP(sizeof(MMICONNECTION_NOTIFY_LIST_T));
            SCI_MEMSET(s_subscriber_list_ptr,0,sizeof(MMICONNECTION_NOTIFY_LIST_T));
            SCI_MEMCPY(&(s_subscriber_list_ptr->notify_info),notify_info_ptr,sizeof(MMICONNECTION_NOTIFY_INFO_T));
        }
        else
        {
            cursor = s_subscriber_list_ptr;
            while(PNULL != cursor->next)
            {
                if(cursor->notify_info.module_id == notify_info_ptr->module_id)
                {
                    return FALSE;   //已经注册过了
                }
                cursor = cursor->next;
            }
            if(cursor->notify_info.module_id == notify_info_ptr->module_id)
            {
                return FALSE;   //已经注册过了
            }
            cursor->next = SCI_ALLOC_APP(sizeof(MMICONNECTION_NOTIFY_LIST_T));
            SCI_MEMSET(cursor->next,0,sizeof(MMICONNECTION_NOTIFY_LIST_T));
            SCI_MEMCPY(&(cursor->next->notify_info),notify_info_ptr,sizeof(MMICONNECTION_NOTIFY_INFO_T));
        }
        return_val = TRUE;
    }

    return return_val;
}

/*****************************************************************************/
// 	Description : 销毁
//	Global resource dependence : none
//  Author: aoke.hu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_UnregisterNotifyInfo(uint32 module_id)
{
    MMICONNECTION_NOTIFY_LIST_T *cursor = PNULL;
    MMICONNECTION_NOTIFY_LIST_T *cursor_free = PNULL;

    cursor = s_subscriber_list_ptr;
    if(PNULL != cursor)
    {
        if(cursor->notify_info.module_id == module_id)
        {
            s_subscriber_list_ptr = cursor->next;
            SCI_FREE(cursor);
        }
        else
        {
            while(PNULL != cursor->next)
            {
                if(cursor->next->notify_info.module_id == module_id)
                {
                    cursor_free = cursor->next;
                    cursor->next = cursor_free->next;
                    SCI_FREE(cursor_free);
                    break;
                }
                cursor = cursor->next;
            }
        }
    }
    
}

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : get s_connection_setting_info
//  Global resource dependence : none
//  Author: 
//  Note:for SKY
/*****************************************************************************/
PUBLIC MMICONNECTION_SETTING_T * MMIMRAPP_GetConnetInfo(void)
{
	return &s_connection_setting_info;
}
#endif

#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
//  Description : Is Same Network Plmn
//  Global resource dependence : none
//  Author: dave.ruan
//  Note:cmp mcc and mnc
/*****************************************************************************/
LOCAL BOOLEAN IsSameNetworkPlmn(PHONE_PLMN_NAME_E plmn1, PHONE_PLMN_NAME_E plmn2)
{
    const MMI_NETWORK_NAME_T *plmn1_network_info_ptr = PNULL;
    const MMI_NETWORK_NAME_T *plmn2_network_info_ptr = PNULL;

    plmn1_network_info_ptr = MMIAPIPHONE_GetNetWorkInfo(plmn1);
    plmn2_network_info_ptr = MMIAPIPHONE_GetNetWorkInfo(plmn2);

    if (PNULL != plmn1_network_info_ptr && PNULL != plmn2_network_info_ptr)
    {
        if ((plmn1_network_info_ptr->mcc == plmn2_network_info_ptr->mcc)
            &&(plmn1_network_info_ptr->mnc == plmn2_network_info_ptr->mnc))
        {
            return TRUE;
        }
    }

    return FALSE;
}
/*****************************************************************************/
//  Description : Is Same Network Plmn
//  Global resource dependence : none
//  Author: wancan.you
//  Note:cmp mcc and network name
/*****************************************************************************/
LOCAL BOOLEAN IsSameNetworkName(PHONE_PLMN_NAME_E plmn1, PHONE_PLMN_NAME_E plmn2)
{
    const MMI_NETWORK_NAME_T *plmn1_network_info_ptr = PNULL;
    const MMI_NETWORK_NAME_T *plmn2_network_info_ptr = PNULL;
    BOOLEAN is_plmn1_india_mcc = FALSE;
    BOOLEAN is_plmn2_india_mcc = FALSE;
    BOOLEAN is_plmn_name_bsnl_cellone = FALSE;

    plmn1_network_info_ptr = MMIAPIPHONE_GetNetWorkInfo(plmn1);
    plmn2_network_info_ptr = MMIAPIPHONE_GetNetWorkInfo(plmn2);

    if (PNULL != plmn1_network_info_ptr && PNULL != plmn2_network_info_ptr)
    {
        if (404 == plmn1_network_info_ptr->mcc
            || 405 == plmn1_network_info_ptr->mcc)
        {
            is_plmn1_india_mcc = TRUE;
        }

        if (404 == plmn2_network_info_ptr->mcc
            || 405 == plmn2_network_info_ptr->mcc)
        {
            is_plmn2_india_mcc = TRUE;
        }

        //Cellone and BSNL use the same network account
        if((plmn1_network_info_ptr->name_index == TXT_NET_CELLONE && plmn2_network_info_ptr->name_index == TXT_NET_BSNL)
            ||(plmn1_network_info_ptr->name_index == TXT_NET_BSNL && plmn2_network_info_ptr->name_index == TXT_NET_CELLONE))
        {
            is_plmn_name_bsnl_cellone = TRUE;
        }
        
        if (((plmn1_network_info_ptr->mcc == plmn2_network_info_ptr->mcc) || (is_plmn1_india_mcc && is_plmn2_india_mcc))
            && (plmn1_network_info_ptr->name_index == plmn2_network_info_ptr->name_index 
            ||is_plmn_name_bsnl_cellone))
        {
            return TRUE;
        }
    }

    return FALSE;
}

#ifdef ACCOUNT_AUTO_ADAPT_SUPPORT
/*****************************************************************************/
//  Description : Auto Adapting Network account
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL void AutoAdapting(MN_DUAL_SYS_E dual_sys, MMICONNECTION_ACCOUNT_TYPE_E account_type, BOOLEAN is_need_clean)
{
    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        return;
    }

    //SCI_TRACE_LOW:"AutoAdapting dual_sys=%d, account_type=%d, is_need_clean=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5786_112_2_18_2_12_17_13,(uint8*)"ddd", dual_sys, account_type, is_need_clean);

    if (SIM_STATUS_OK == MMIAPIPHONE_GetSimStatus(dual_sys))
    {
        uint32 i =0;
        PHONE_PLMN_NAME_E hplmn = PLMN_INVALID;
        PHONE_PLMN_NAME_E conection_plmn_name[MMI_DUAL_SYS_MAX] = {0};
        MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
        BOOLEAN is_account_appointed[MMICONNECTION_ACCOUNT_MAX] = {0};
        PLMN_CMP_CALLBACK plmn_cmp_fun = PNULL;
        
        MMINV_READ(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name, return_value);

        if (return_value != MN_RETURN_SUCCESS)
        {
            SCI_MEMSET(conection_plmn_name, 0x00, sizeof(conection_plmn_name));

            MMINV_WRITE(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name);
        }

        MMIAPIPHONE_GetFullPlmn(dual_sys, PNULL, &hplmn);

        //SCI_TRACE_LOW:"AutoAdapting hplmn=%d, conection_plmn_name=%d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5808_112_2_18_2_12_17_14,(uint8*)"dd", hplmn, conection_plmn_name[dual_sys]);

        if (PLMN_INVALID != hplmn && 0 != hplmn && !IsSameNetworkPlmn(hplmn, conection_plmn_name[dual_sys]))
        {
            BOOLEAN is_res_data_exist = FALSE;
            MMICONNECTION_RES_INFO_T res_info = {0};
            uint16 custom_list_num = LINK_CUSTOMED_LEN;
            BOOLEAN is_plmn_same= FALSE;
            
            is_res_data_exist = LoadNetworkAccountInfo(&res_info);

            if (!is_res_data_exist || PNULL == res_info.res_item_ptr)
            {
                UnloadNetworkAccountInfo(&res_info);
                return;
            }

            if (res_info.is_res_data)
            {
                custom_list_num = LINK_CUSTOMED_RES_LEN;
            }
            else
            {
                custom_list_num = LINK_CUSTOMED_LEN;
            }
            
            custom_list_num = MIN(custom_list_num, res_info.res_num);
            custom_list_num = MIN(custom_list_num, MMICONNECTION_MAX_SETTING_NUM);

            conection_plmn_name[dual_sys] = hplmn;
            MMINV_WRITE(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name);

            //判断是否有相同的PLMN
            for(i = 0; i< res_info.res_num; i++)
            {
                MN_PLMN_T plmn ={0};
                PHONE_PLMN_NAME_E plmn_name =PLMN_INVALID;
                
                plmn.mcc = res_info.res_item_ptr[i].mcc;
                plmn.mnc = res_info.res_item_ptr[i].mnc;
                plmn_name = MMIAPIPHONE_GetNetWorkId(&plmn);

                if(IsSameNetworkPlmn(hplmn, plmn_name))
                {
                    is_plmn_same = TRUE;
                    break;
                }
            }
            
            //根据PLMN是否相同来确定调用的函数
            if(is_plmn_same)
            {
                plmn_cmp_fun = IsSameNetworkPlmn;
            }
            else
            {
                plmn_cmp_fun = IsSameNetworkName;
            }
            
            if (is_need_clean)
            {
#ifdef CMCC_UI_STYLE //定制版本不清除锁定账号
                uint16 reset_list_num = 0;
                BOOLEAN is_need_add_default = TRUE;

                for (i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)
                {
                    if (plmn_cmp_fun(hplmn, (PHONE_PLMN_NAME_E)s_connection_setting_info.setting_detail[dual_sys][i].hplmn))
                    {
                        is_need_add_default = FALSE;
                        break;
                    }
                }

                //SCI_TRACE_LOW:"AutoAdapting is_need_add_default=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_5882_112_2_18_2_12_17_15,(uint8*)"d", is_need_add_default);

                if (s_connection_setting_info.settinglink_num[dual_sys] > custom_list_num)
                {
                    reset_list_num = s_connection_setting_info.settinglink_num[dual_sys] - custom_list_num;
                    s_connection_setting_info.settinglink_num[dual_sys] = custom_list_num;

                    SCI_MEMSET((MMICONNECTION_LINKSETTING_DETAIL_T *)&s_connection_setting_info.setting_detail[dual_sys][custom_list_num],
                                        0x00, (reset_list_num * sizeof(MMICONNECTION_LINKSETTING_DETAIL_T)));
                }
#else
                SCI_MEMSET(&s_connection_setting_info.setting_detail[dual_sys], 0x00, sizeof(s_connection_setting_info.setting_detail[dual_sys]));
                s_connection_setting_info.settinglink_num[dual_sys] = 0;
#endif

#ifdef CMCC_UI_STYLE //定制版本不清除锁定账号
                if (is_need_add_default)
#endif
                {
                    for (i = 0; i < res_info.res_num; i++)
                    {
                        PHONE_PLMN_NAME_E plmn_name = PLMN_INVALID;
                        MN_PLMN_T plmn = {0};

                        if (s_connection_setting_info.settinglink_num[dual_sys] >= MMICONNECTION_MAX_SETTING_NUM)
                        {
                            break;
                        }
                        plmn.mcc = res_info.res_item_ptr[i].mcc;
                        plmn.mnc = res_info.res_item_ptr[i].mnc;
                        plmn_name = MMIAPIPHONE_GetNetWorkId(&plmn);
                        
                        if (plmn_cmp_fun(hplmn, plmn_name))
                        {
                            uint16 j = s_connection_setting_info.settinglink_num[dual_sys];
                            BOOLEAN is_customed_detail = FALSE;

                            if (MMICONNECTION_ACCESS_READONLY == res_info.res_item_ptr[i].access_option)
                            {
                                is_customed_detail = TRUE;
                            }
                            else
                            {
                                is_customed_detail = FALSE;
                            }

                            SaveNetworkAccountInfo(&s_connection_setting_info.setting_detail[dual_sys][j],
                                                                        &res_info.res_item_ptr[i], is_customed_detail, TRUE, i);

                            MMICONNECTION_SetHighPriList(dual_sys, (uint8)j);

                            s_connection_setting_info.settinglink_num[dual_sys]++;
                        }                        
                    }

                    MMINV_WRITE(MMINV_CONNECTION_SETTING_INFO, &s_connection_setting_info);

                    if (0 == s_connection_setting_info.settinglink_num[dual_sys])
                    {
                        MMICONNECTION_ResetSettingListEx(dual_sys, TRUE);
                    }
                }
            }

            UnloadNetworkAccountInfo(&res_info);

            for (i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)
            {
                if (plmn_cmp_fun(hplmn, (PHONE_PLMN_NAME_E)s_connection_setting_info.setting_detail[dual_sys][i].hplmn))
                {
                    if (MMICONNECTION_ACCOUNT_MAX == account_type
                        || (s_connection_setting_info.setting_detail[dual_sys][i].account_type == account_type))
                    {
                        switch (s_connection_setting_info.setting_detail[dual_sys][i].account_type)
                        {
                        case MMICONNECTION_ACCOUNT_BROWSER:
                            if (!is_account_appointed[MMICONNECTION_ACCOUNT_BROWSER])
                            {
                                is_account_appointed[MMICONNECTION_ACCOUNT_BROWSER] = TRUE;
                            #ifdef BROWSER_SUPPORT
                                MMIAPIBROWSER_SetNetSettingIndex(dual_sys, i);
                            #endif
                            }
                            break;

                        case MMICONNECTION_ACCOUNT_MMS:
                            if (!is_account_appointed[MMICONNECTION_ACCOUNT_MMS])
                            {
                                is_account_appointed[MMICONNECTION_ACCOUNT_MMS] = TRUE;

                            #ifdef MMS_SUPPORT
                                MMIAPIMMS_Setting_SetNetSettingIndex(dual_sys, i);
                            #endif
                            }
                            break;
                            
                        case MMICONNECTION_ACCOUNT_DCD:
                            if (!is_account_appointed[MMICONNECTION_ACCOUNT_DCD])
                            {
                                is_account_appointed[MMICONNECTION_ACCOUNT_DCD] = TRUE;

                            #ifdef DCD_SUPPORT
                                MMIAPIDCD_Setting_SetNetSettingIndex(dual_sys, i);
                            #endif
                            }
                            break;

                        default:
                            break;
                        }
                    }
                }
            }
        }
        else if(PLMN_INVALID == hplmn && (0== s_connection_setting_info.settinglink_num[dual_sys] ||MN_RETURN_SUCCESS != return_value))
        {
            //if  plmn is invalid, reset default
            MMICONNECTION_ResetSettingListEx(dual_sys, TRUE);
        }
        else
        {   
            /*do nothing*/
        }
    }
}
#endif

/*****************************************************************************/
//  Description : Auto Adapting specified account
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
PUBLIC void MMICONNECTION_AdaptingAccount(MN_DUAL_SYS_E dual_sys, MMICONNECTION_ACCOUNT_TYPE_E account_type)
{
#ifdef ACCOUNT_AUTO_ADAPT_SUPPORT
    uint32 i = 0;
    PHONE_PLMN_NAME_E conection_plmn_name[MMI_DUAL_SYS_MAX] = {0};

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {

        SCI_MEMSET(conection_plmn_name, 0x00, sizeof(conection_plmn_name));

        MMINV_WRITE(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name);

        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            AutoAdapting((MN_DUAL_SYS_E)i, account_type, FALSE);
        }
    }
    else
    {
        MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;

        MMINV_READ(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name, return_value);

        if (return_value != MN_RETURN_SUCCESS)
        {
            SCI_MEMSET(conection_plmn_name, 0x00, sizeof(conection_plmn_name));
        }
        else
        {
            conection_plmn_name[dual_sys] = 0x00;
        }

        MMINV_WRITE(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name);

        AutoAdapting(dual_sys, account_type, FALSE);
    }
#endif
}

/*****************************************************************************/
//  Description : Auto Adapting Network account
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
PUBLIC void MMICONNECTION_AutoAdapting(MN_DUAL_SYS_E dual_sys)
{
#ifdef ACCOUNT_AUTO_ADAPT_SUPPORT
    uint32 i = 0;

    if (dual_sys >= MMI_DUAL_SYS_MAX)
    {
        for (i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
        {
            AutoAdapting((MN_DUAL_SYS_E)i, MMICONNECTION_ACCOUNT_MAX, TRUE);
        }
    }
    else
    {
        AutoAdapting(dual_sys, MMICONNECTION_ACCOUNT_MAX, TRUE);
    }
#endif
}

/*****************************************************************************/
//  Description : Get account default index
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
PUBLIC uint8 MMIAPICONNECTION_GetAccountDefaultIndex(MN_DUAL_SYS_E dual_sys,
                                                                                                        MMICONNECTION_ACCOUNT_TYPE_E account_type)
{
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    uint32 i = 0;

    //SCI_TRACE_LOW:"[MMICONNECTION]:MMICONNECTION_GetAccountDefaultIndex dual_sys=%d,account_type=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6086_112_2_18_2_12_18_16,(uint8*)"dd", dual_sys, account_type);

    if (dual_sys >= MMI_DUAL_SYS_MAX || account_type >= MMICONNECTION_ACCOUNT_MAX)
    {
        return 0;
    }

    if (0 == s_connection_setting_info.settinglink_num[dual_sys])
    {
        MMINV_READ(MMINV_CONNECTION_SETTING_INFO,&s_connection_setting_info,return_value);

        if (MN_RETURN_SUCCESS != return_value)
        {
            //SCI_TRACE_LOW:"[MMICONNECTION]:MMICONNECTION_GetAccountDefaultIndex setting read Failed"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6099_112_2_18_2_12_18_17,(uint8*)"");
            MMICONNECTION_ResetSettingList(TRUE);//设固定设置
        }
    }

    for (i = 0; i < s_connection_setting_info.settinglink_num[dual_sys]; i++)
    {
        if (account_type == s_connection_setting_info.setting_detail[dual_sys][i].account_type)
        {
            return i;
        }
    }

    return 0;
}
#endif

#ifdef MMI_GPRS_SUPPORT
/*****************************************************************************/
//  Description :Parse Network Account Info
//  Global resource dependence : none
//  Author:dave.ruan
//  Note:   is_tcard_file: Tcard file OR resource
/*****************************************************************************/
LOCAL BOOLEAN ParseNetworkAccountInfo(MMICONNECTION_RES_INFO_T *res_info_ptr, MMIRES_DATA_INFO_T file_info)
{   
    BOOLEAN is_res_data_ok = TRUE;
    uint16 account_num = 0;
    
    if (PNULL != file_info.data_ptr && NETWORK_ACCOUNT_HEADER_LENGTH < file_info.data_len)
    {
        //is magic header "ntac"
        if(0 != memcmp(file_info.data_ptr,NETWORK_ACCOUNT_MAGIC_FLAG,NETWORK_ACCOUNT_MAGIC_LENGTH))
        {
            return FALSE;
        }
        
        account_num = MMICOM_READ_2BYTE(file_info.data_ptr, NETWORK_ACCOUNT_NUM_OFFSET);

        if (0 == account_num)
        {
            is_res_data_ok = FALSE;
        }
        else if (PNULL != res_info_ptr)
        {
            uint16 i = 0;
            uint32 offset_addr = 0;
            uint32 data_offset = 0;

            res_info_ptr->res_item_ptr = SCI_ALLOCAZ(sizeof(MMICONNECTION_RES_ITEM_T) * account_num);

            if (PNULL == res_info_ptr->res_item_ptr)
            {
                res_info_ptr->res_num = 0;
            }
            else
            {
                //res_info_ptr->res_num = account_num;
                res_info_ptr->is_res_data = TRUE;
            
                for (i = 0; i < account_num; i++)
                {
                    uint8 a = 0;
                    uint8 b = 0;
                    uint8 c = 0;
                    uint8 d = 0;
                    uint16 len = 0;
                    uint32 temp_offset_addr = 0;
                    const wchar *name_ptr = PNULL;
#if defined(__BIG_ENDIAN) || defined(__BigEndian)
                    uint16 j = 0;
#endif
                    a = MMICOM_READ_BYTE(file_info.data_ptr, (NETWORK_ACCOUNT_HEADER_LENGTH + i * NETWORK_ACCOUNT_OFFSET_LENGTH));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (NETWORK_ACCOUNT_HEADER_LENGTH + i * NETWORK_ACCOUNT_OFFSET_LENGTH + 1));
                    c = MMICOM_READ_BYTE(file_info.data_ptr, (NETWORK_ACCOUNT_HEADER_LENGTH + i * NETWORK_ACCOUNT_OFFSET_LENGTH + 2));
                    d = MMICOM_READ_BYTE(file_info.data_ptr, (NETWORK_ACCOUNT_HEADER_LENGTH + i * NETWORK_ACCOUNT_OFFSET_LENGTH + 3));

                    temp_offset_addr = offset_addr;

                    offset_addr = CONVERT4U8TOU32(a, b, c, d);

                    if (0 != data_offset &&
                        temp_offset_addr + data_offset != offset_addr )//资源出错
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    //get mcc
                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_MCC_OFFSET));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_MCC_OFFSET + 1));

                    res_info_ptr->res_item_ptr[i].mcc = CONVERT2U8TOU16(a, b);

                    //get mnc
                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_MNC_OFFSET));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_MNC_OFFSET + 1));

                    res_info_ptr->res_item_ptr[i].mnc = CONVERT2U8TOU16(a, b);

                    //get port
                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_PORT_OFFSET));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_PORT_OFFSET + 1));

                    res_info_ptr->res_item_ptr[i].port = CONVERT2U8TOU16(a, b);

                    //get access type 
                    res_info_ptr->res_item_ptr[i].access_type = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_ACCESS_TYPE_OFFSET));

                    //get access option 
                    res_info_ptr->res_item_ptr[i].access_option = (MMICONNECTION_ACCESS_OPTION_E)MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_ACCESS_OPTION_OFFSET));

                    //get auth type
                    res_info_ptr->res_item_ptr[i].auth_type = (MN_PCO_AUTH_TYPE_E)MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_AUTH_TYPE_OFFSET));

                    //get account type
                    res_info_ptr->res_item_ptr[i].account_type = (MMICONNECTION_ACCOUNT_TYPE_E)MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_ACCOUNT_TYPE_OFFSET));

                    //get name
                    name_ptr = (const wchar *)(file_info.data_ptr + offset_addr + NETWORK_ACCOUNT_ITEM_NAME_LEN_OFFSET + NETWORK_ACCOUNT_ITEM_NAME_LEN_LENGTH);
                    
                    len = MMIAPICOM_Wstrlen(name_ptr);
                    res_info_ptr->res_item_ptr[i].name_len = MIN(MMICONNECTION_MAX_LINKMODENAME_LEN, len);

                    res_info_ptr->res_item_ptr[i].name = SCI_ALLOCAZ((res_info_ptr->res_item_ptr[i].name_len + 1) * sizeof(wchar));
                    if(PNULL == res_info_ptr->res_item_ptr[i].name)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

#if defined(__BIG_ENDIAN) || defined(__BigEndian)
                    for (j = 0; j < res_info_ptr->res_item_ptr[i].name_len; j++)
                    {
                        res_info_ptr->res_item_ptr[i].name[j] = ((name_ptr[j]<< 8)&0xFF00) + ((name_ptr[j]>>8)&0x00FF);
                    }
#else
                    MMI_WSTRNCPY(res_info_ptr->res_item_ptr[i].name, res_info_ptr->res_item_ptr[i].name_len, name_ptr, res_info_ptr->res_item_ptr[i].name_len, res_info_ptr->res_item_ptr[i].name_len);
#endif

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_NAME_LEN_OFFSET));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + NETWORK_ACCOUNT_ITEM_NAME_LEN_OFFSET + 1));

                    data_offset = CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_NAME_LEN_OFFSET + NETWORK_ACCOUNT_ITEM_NAME_LEN_LENGTH;

                    //get gateway
                    res_info_ptr->res_item_ptr[i].gateway = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_GATEWAY_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].gateway)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].gateway);
                    res_info_ptr->res_item_ptr[i].gateway_len = MIN(MMICONNECTION_MAX_GATEWAY_LEN, len);
                    
                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_GATEWAY_LENGTH;

                    //get dns1
                    res_info_ptr->res_item_ptr[i].dns1 = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_DNS1_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].dns1)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].dns1);
                    res_info_ptr->res_item_ptr[i].dns1_len = MIN(MMICONNECTION_MAX_DNS_LEN, len);

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_DNS1_LENGTH;

                    //get dns2
                    res_info_ptr->res_item_ptr[i].dns2 = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_DNS2_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].dns2)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].dns2);
                    res_info_ptr->res_item_ptr[i].dns2_len = MIN(MMICONNECTION_MAX_DNS_LEN, len);

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_DNS2_LENGTH;

                    //get username
                    res_info_ptr->res_item_ptr[i].username = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_USERNAME_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].username)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].username);
                    res_info_ptr->res_item_ptr[i].username_len = MIN(MMICONNECTION_MAX_USERNAME_LEN, len);

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_USERNAME_LENGTH;

                    //get password
                    res_info_ptr->res_item_ptr[i].password = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_PASSWORD_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].password)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].password);
                    res_info_ptr->res_item_ptr[i].password_len = MIN(MMICONNECTION_MAX_PASSWORD_LEN, len);

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_PASSWORD_LENGTH;

                    //get apn
                    res_info_ptr->res_item_ptr[i].apn = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_APN_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].apn)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].apn);
                    res_info_ptr->res_item_ptr[i].apn_len = MIN(MMICONNECTION_MAX_APN_LEN, len);

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_APN_LENGTH;

                    //get homepage
                    res_info_ptr->res_item_ptr[i].homepage = (const char *)(file_info.data_ptr + offset_addr + data_offset + NETWORK_ACCOUNT_ITEM_HOMEPAGE_LENGTH);
                    if(PNULL == res_info_ptr->res_item_ptr[i].homepage)
                    {
                        is_res_data_ok = FALSE;

                        break;
                    }

                    len = strlen(res_info_ptr->res_item_ptr[i].homepage);
                    res_info_ptr->res_item_ptr[i].homepage_len = MIN(MMICONNECTION_HOMEPAGE_MAX_URL_LEN, len);

                    a = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset));
                    b = MMICOM_READ_BYTE(file_info.data_ptr, (offset_addr + data_offset + 1));

                    data_offset += CONVERT2U8TOU16(a, b) + NETWORK_ACCOUNT_ITEM_HOMEPAGE_LENGTH;
                }

                res_info_ptr->res_num = i;
                //is_res_data_ok = TRUE;
            }
        }
    }
    else
    {
        is_res_data_ok = FALSE;
    }
    
    //SCI_TRACE_LOW:"ParseNetworkAccountInfo is_res_data_ok=%d, account_num=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6374_112_2_18_2_12_18_18,(uint8*)"dd", is_res_data_ok, account_num);

    return is_res_data_ok;
}

/*****************************************************************************/
//  Description : Load NetAccount Info From Resource
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN LoadNetworkAccountInfoFromRes(MMICONNECTION_RES_INFO_T *res_info_ptr)
{
#ifdef CMCC_UI_STYLE
    return FALSE;  //cmcc load from list
#else

    MMIRES_DATA_INFO_T file_info = {0}; /*lint !e64*/
    BOOLEAN is_res_data_exist = FALSE;
    uint16 account_num = 0;
    
#ifdef DATA_ACCOUNT_IN_TCARD_SUPPORT
    if(!MMIAPIUDISK_UdiskIsRun())
    {
        uint16 ntac_file_path_w[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
        char ntac_file_path[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
        MMIFILE_HANDLE file_handle = 0; 

        strncpy( ntac_file_path,NETACCOUNT_FILE_PATH,strlen((char*)NETACCOUNT_FILE_PATH));
        strcat(ntac_file_path,".ntac");

        MMI_STRNTOWSTR(ntac_file_path_w, MMIFILE_FULL_PATH_MAX_LEN, (uint8*)ntac_file_path, MMIFILE_FULL_PATH_MAX_LEN, MMIFILE_FULL_PATH_MAX_LEN );
        file_handle = MMIAPIFMM_CreateFile((const uint16*)ntac_file_path_w,SFS_MODE_OPEN_EXISTING|SFS_MODE_READ,0,0);

        if (SFS_INVALID_HANDLE != file_handle)
        { 
            file_info.type = 0;
            file_info.data_len = MMIAPIFMM_GetFileSize(file_handle);
            file_info.data_ptr = SCI_ALLOCAZ(file_info.data_len);

            if (PNULL != file_info.data_ptr)
            {
                uint32 read_size = 0;
                SFS_ERROR_E error = SFS_NO_ERROR;
                
                error = MMIAPIFMM_ReadFile(file_handle, file_info.data_ptr, file_info.data_len, &read_size, PNULL);
                if ((SFS_NO_ERROR == error) && ParseNetworkAccountInfo(res_info_ptr, file_info))//解析文件正常,返回TRUE
                {
                    MMIAPIFMM_CloseFile(file_handle);

                    SCI_FREE(file_info.data_ptr);

                    return TRUE;
                }
                else
                {
                    UnloadNetworkAccountInfo(res_info_ptr);
                }

                SCI_FREE(file_info.data_ptr);
            }

            MMIAPIFMM_CloseFile(file_handle);
        }

     }
#endif    

    //is_res_data_exist is false if come this line
    is_res_data_exist = MMI_GetDataInfo(R_NETWORK_ACCOUNT, &file_info);
    
    if (is_res_data_exist && ParseNetworkAccountInfo(res_info_ptr, file_info))
    {
        is_res_data_exist = TRUE;
    }
    else
    {
        UnloadNetworkAccountInfo(res_info_ptr);

        is_res_data_exist = FALSE;
    }

    //SCI_TRACE_LOW:"LoadNetworkAccountInfoFromRes is_res_data_exist=%d, account_num=%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6454_112_2_18_2_12_18_19,(uint8*)"dd", is_res_data_exist, account_num);

    return is_res_data_exist;
#endif
}

/*****************************************************************************/
//  Description : Load NetAccount Info From Table List
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN LoadNetworkAccountInfoFromTableList(MMICONNECTION_RES_INFO_T *res_info_ptr)
{
    if (PNULL != res_info_ptr)
    {
        res_info_ptr->res_item_ptr = SCI_ALLOCAZ(sizeof(MMICONNECTION_RES_ITEM_T) * LINK_SETTING_TABLE_LEN);

        if (PNULL == res_info_ptr->res_item_ptr)
        {
            res_info_ptr->res_num = 0;

            return FALSE;
        }
        else
        {
            uint32 i =0;

            res_info_ptr->res_num = LINK_SETTING_TABLE_LEN;
            res_info_ptr->is_res_data = FALSE;

            for (i = 0; i < LINK_SETTING_TABLE_LEN; i++)
            {
                MMI_STRING_T name_string = {0};
#ifdef CMCC_UI_STYLE
                uint32 j =0;
                uint16 custom_list_num = LINK_CUSTOMED_LEN;
#endif
                //mcc
                res_info_ptr->res_item_ptr[i].mcc = setting_search_table[i].mcc;

                //mnc
                res_info_ptr->res_item_ptr[i].mnc = setting_search_table[i].mnc;

                //port
                res_info_ptr->res_item_ptr[i].port = setting_search_table[i].port;

                //access_type
                res_info_ptr->res_item_ptr[i].access_type = setting_search_table[i].access_type;

                //access_option
                res_info_ptr->res_item_ptr[i].access_option = MMICONNECTION_ACCESS_EDITABLE;
#ifdef CMCC_UI_STYLE
                for (j = 0; j < custom_list_num; j++)
                {
                    if (i == (uint32)s_customed_index[j])
                    {
                        res_info_ptr->res_item_ptr[i].access_option = MMICONNECTION_ACCESS_READONLY;

                        break;
                    }
                }
#endif

                //auth_type
                res_info_ptr->res_item_ptr[i].auth_type = setting_search_table[i].auth_type;

                //account_type
                res_info_ptr->res_item_ptr[i].account_type = setting_search_table[i].account_type;

                //name
                MMI_GetLabelTextByLang(setting_search_table[i].name_index, &name_string);
                res_info_ptr->res_item_ptr[i].name_len = MIN(name_string.wstr_len, MMICONNECTION_MAX_USERNAME_LEN);
                res_info_ptr->res_item_ptr[i].name = SCI_ALLOCAZ((res_info_ptr->res_item_ptr[i].name_len + 1) * sizeof(wchar));
                MMI_WSTRNCPY(res_info_ptr->res_item_ptr[i].name,
                            res_info_ptr->res_item_ptr[i].name_len,
                            name_string.wstr_ptr,
                            res_info_ptr->res_item_ptr[i].name_len,
                            res_info_ptr->res_item_ptr[i].name_len);

                //gateway
                res_info_ptr->res_item_ptr[i].gateway = setting_search_table[i].gateway;
                res_info_ptr->res_item_ptr[i].gateway_len = strlen(res_info_ptr->res_item_ptr[i].gateway);
                
                //dns1
                res_info_ptr->res_item_ptr[i].dns1 = (const char *)MMICONNECTION_DEFAULT_DNS;
                res_info_ptr->res_item_ptr[i].dns1_len = strlen(res_info_ptr->res_item_ptr[i].dns1);

                //dns2
                res_info_ptr->res_item_ptr[i].dns2= (const char *)MMICONNECTION_DEFAULT_DNS;
                res_info_ptr->res_item_ptr[i].dns2_len = strlen(res_info_ptr->res_item_ptr[i].dns2);

                //username
                res_info_ptr->res_item_ptr[i].username = setting_search_table[i].username;
                res_info_ptr->res_item_ptr[i].username_len = strlen(setting_search_table[i].username);

                //password
                res_info_ptr->res_item_ptr[i].password = setting_search_table[i].password;
                res_info_ptr->res_item_ptr[i].password_len = strlen(setting_search_table[i].password);

                //apn
                res_info_ptr->res_item_ptr[i].apn = setting_search_table[i].apn;
                res_info_ptr->res_item_ptr[i].apn_len = strlen(setting_search_table[i].apn);

                //homepage
                res_info_ptr->res_item_ptr[i].homepage = setting_search_table[i].homepage;
                res_info_ptr->res_item_ptr[i].homepage_len = strlen(setting_search_table[i].homepage);
            }
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : Load NetAccount Info
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN LoadNetworkAccountInfo(MMICONNECTION_RES_INFO_T *res_info_ptr)
{
    BOOLEAN is_res_data_exist = FALSE;

    is_res_data_exist = LoadNetworkAccountInfoFromRes(res_info_ptr);

    if (!is_res_data_exist)
    {
        is_res_data_exist = LoadNetworkAccountInfoFromTableList(res_info_ptr);
    }

    return is_res_data_exist;
}

/*****************************************************************************/
//  Description : Unload NetAccount Info
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN UnloadNetworkAccountInfo(MMICONNECTION_RES_INFO_T *res_info_ptr)
{
    if (PNULL != res_info_ptr && PNULL != res_info_ptr->res_item_ptr)
    {
        uint32 i = 0;

        for (i = 0; i < res_info_ptr->res_num; i++)
        {
            if (PNULL != res_info_ptr->res_item_ptr[i].name)
            {
                SCI_FREE(res_info_ptr->res_item_ptr[i].name);
                res_info_ptr->res_item_ptr[i].name = PNULL;
            }
        }
    
        SCI_FREE(res_info_ptr->res_item_ptr);
        res_info_ptr->res_item_ptr = PNULL;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : Save NetAccount Info
//  Global resource dependence : none
//  Author: wancan.you
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN SaveNetworkAccountInfo(MMICONNECTION_LINKSETTING_DETAIL_T *link_detail_ptr,//IN/OUT
                                                                            MMICONNECTION_RES_ITEM_T *res_item_ptr,//IN
                                                                            BOOLEAN is_customed_detail,//IN
                                                                            BOOLEAN is_default_unchange,
                                                                            uint16 list_index)//IN
{
    PHONE_PLMN_NAME_E plmn_name = PLMN_INVALID;
    MN_PLMN_T plmn = {0};

    if (PNULL == link_detail_ptr || PNULL == res_item_ptr)
    {
        return FALSE;
    }

    plmn.mcc = res_item_ptr->mcc;
    plmn.mnc = res_item_ptr->mnc;
    plmn_name = MMIAPIPHONE_GetNetWorkId(&plmn);

    //access type
    link_detail_ptr->access_type = res_item_ptr->access_type;

    //auth type
    link_detail_ptr->auth_type=res_item_ptr->auth_type;

    //port
    link_detail_ptr->port = res_item_ptr->port;

    //name
    link_detail_ptr->name_len = MIN(res_item_ptr->name_len, MMICONNECTION_MAX_LINKMODENAME_LEN);
    SCI_MEMSET(link_detail_ptr->name, 0x00, sizeof(link_detail_ptr->name));
    MMIAPICOM_Wstrncpy(link_detail_ptr->name, res_item_ptr->name, link_detail_ptr->name_len);

    //gateway
    link_detail_ptr->gateway_len = res_item_ptr->gateway_len;
    SCI_MEMSET(link_detail_ptr->gateway,0x00,sizeof(link_detail_ptr->gateway));
    SCI_MEMCPY(link_detail_ptr->gateway,res_item_ptr->gateway,link_detail_ptr->gateway_len);

    //user name
    link_detail_ptr->username_len = MIN(res_item_ptr->username_len, MMICONNECTION_MAX_USERNAME_LEN);
    SCI_MEMSET(link_detail_ptr->username, 0x00, sizeof(link_detail_ptr->username));
    SCI_MEMCPY(link_detail_ptr->username, res_item_ptr->username, link_detail_ptr->username_len);

    //password
    link_detail_ptr->password_len = MIN(res_item_ptr->password_len, MMICONNECTION_MAX_PASSWORD_LEN);
    SCI_MEMSET(link_detail_ptr->password, 0x00, sizeof(link_detail_ptr->password));
    SCI_MEMCPY(link_detail_ptr->password, res_item_ptr->password, link_detail_ptr->password_len);

    //apn
    link_detail_ptr->apn_len = MIN(res_item_ptr->apn_len, MMICONNECTION_MAX_APN_LEN);
    SCI_MEMSET(link_detail_ptr->apn, 0x00, sizeof(link_detail_ptr->apn));
    SCI_MEMCPY(link_detail_ptr->apn, res_item_ptr->apn, link_detail_ptr->apn_len);

    //home page
    link_detail_ptr->homepage_len = MIN(res_item_ptr->homepage_len, MMICONNECTION_HOMEPAGE_MAX_URL_LEN);
    SCI_MEMSET(link_detail_ptr->homepage, 0x00, sizeof(link_detail_ptr->homepage));
    SCI_MEMCPY(link_detail_ptr->homepage, res_item_ptr->homepage, link_detail_ptr->homepage_len);

    //dns1
    link_detail_ptr->dns_len = MIN(res_item_ptr->dns1_len, MMICONNECTION_MAX_DNS_LEN);
    SCI_MEMSET(link_detail_ptr->dns, 0x00, sizeof(link_detail_ptr->dns));
    SCI_MEMCPY(link_detail_ptr->dns, res_item_ptr->dns1, link_detail_ptr->dns_len);

    //is_customed_detail
    link_detail_ptr->is_customed_detail = is_customed_detail;

    //priority
    //link_detail_ptr->priority = FALSE;

    //is_default_unchange
    link_detail_ptr->is_default_unchange = is_default_unchange;

    //list index
    link_detail_ptr->list_index = list_index;

    //hplmn
    link_detail_ptr->hplmn = plmn_name;

    //account_type
    link_detail_ptr->account_type = res_item_ptr->account_type;

    return TRUE;
}

/*****************************************************************************/
// 	Description : create Linklist Win for mms,browser,dcd,java
//	Global resource dependence : none
//  Author: jixin.xu
//	Note:
/*****************************************************************************/
PUBLIC void MMICONNECTION_CreateLinklistWin(ADD_DATA param, MMICONNECTION_CALLBACK_FUNC callback_func)
{
    /*avoid 窗口重入*/
    MMK_CloseWin(MMICONNECTION_LINKLIST_WIN_ID);

    s_connection_callback_func_ptr = callback_func;
    MMK_CreateWin((uint32*)MMICONNECTION_LINKLIST_WIN_TAB, param);
}

/*****************************************************************************/
// 	Description : check avalid data account without gateway IP for SNTP of current SIM
//	Global resource dependence : s_connection_setting_info
//    Author: dave.ruan
//	Note:item_num_ptr total derect link item number;
//             detail_item:setting detail,include dual_sys and item_index
//             dual_sys sim index
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_GetCurSIMNoProxyAccountIndex(MN_DUAL_SYS_E   dual_sys, uint8* item_num,MMICONNECTION_SETTING_DETAIL_ITEM_T* detail_item,uint8 max_item_num )
{
    uint8    settinglink_dual_sys_index = 0;
    uint8    total_settinglink_num = 0;
    MN_PLMN_T cur_service_plmn = {0};
    
    if(PNULL == item_num ||PNULL == detail_item)
    {
        //SCI_TRACE_LOW:"MMIAUT::GetCurSIMNoProxyAccountIndex  item_num ||detail_item is NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6732_112_2_18_2_12_19_20,(uint8*)"");
        return;
    }
    
    MMIAPIPHONE_GetCurPLMN(&cur_service_plmn, dual_sys);

    if(MMIAPIPHONE_IsSimOk(dual_sys))//SIM init complete
    {
        for(settinglink_dual_sys_index = 0; settinglink_dual_sys_index < s_connection_setting_info.settinglink_num[dual_sys];settinglink_dual_sys_index++)
        {
            if(0 < s_connection_setting_info.setting_detail[dual_sys][settinglink_dual_sys_index].apn_len
                &&(0 == strncmp(MMICONNECTION_INVALID_GATEWAY_FOR_SNTP, (char*)s_connection_setting_info.setting_detail[dual_sys][settinglink_dual_sys_index].gateway,strlen(MMICONNECTION_INVALID_GATEWAY_FOR_SNTP)))
            )
            {
#ifndef WIN32   //only mcc = 460,apn is cmnet;other mcc is use default,not really in china
                    if(460 != cur_service_plmn.mcc  
                    && (0==strncmp(MMICONNECTION_CMCC_DERECT_LINK_APN,(char*)s_connection_setting_info.setting_detail[dual_sys][settinglink_dual_sys_index].apn,strlen(MMICONNECTION_CMCC_DERECT_LINK_APN))))     
                    {
                        continue;
                    }
#endif
                if(total_settinglink_num >= max_item_num)
                {
                    break;
                }
                
                detail_item[total_settinglink_num].dual_sys = dual_sys;
                detail_item[total_settinglink_num].item_index = settinglink_dual_sys_index;
                total_settinglink_num++;
            }
            //SCI_TRACE_LOW:"MMIAUT::SearchDirectLinkAccountIndex total_settinglink_num =%d,dual_sys =%d,settinglink_dual_sys_index=%d"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6763_112_2_18_2_12_19_21,(uint8*)"ddd",total_settinglink_num,dual_sys,settinglink_dual_sys_index);
        }
    }
    * item_num = total_settinglink_num;
    
    //SCI_TRACE_LOW:"MMIAUT::SearchDirectLinkAccountIndex * item_num =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6768_112_2_18_2_12_19_22,(uint8*)"d",* item_num);
}

/*****************************************************************************/
// 	Description : check avalid data account without gateway IP for SNTP
//	Global resource dependence : s_connection_setting_info
//  Author: dave.ruan
//	Note:item_num_ptr total derect link item number;
//             detail_item:setting detail,include dual_sys and item_index
/*****************************************************************************/
PUBLIC void MMIAPICONNECTION_GetNoProxyAccountIndex(uint8* item_num,MMICONNECTION_SETTING_DETAIL_ITEM_T* detail_item, uint8 max_item_num)
{
    MN_DUAL_SYS_E   dual_sys =MMI_DUAL_SYS_MAX;
    uint8    settinglink_dual_sys_index = 0;
    uint8    total_settinglink_num = 0;
    
    if(PNULL == item_num ||PNULL == detail_item)
    {
        //SCI_TRACE_LOW:"MMIAUT::GetNoProxyAccountIndex  item_num ||detail_item is NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6784_112_2_18_2_12_19_23,(uint8*)"");
        return;
    }
    
    for(dual_sys = MN_DUAL_SYS_1; dual_sys < MMI_DUAL_SYS_MAX; dual_sys ++)
    {
        MN_PLMN_T cur_service_plmn = {0};
        
        MMIAPIPHONE_GetCurPLMN(&cur_service_plmn, dual_sys);

        if(MMIAPIPHONE_IsSimOk(dual_sys))//SIM init complete
        {
            for(settinglink_dual_sys_index = 0;settinglink_dual_sys_index < s_connection_setting_info.settinglink_num[dual_sys];settinglink_dual_sys_index++)
            {
                if(0 < s_connection_setting_info.setting_detail[dual_sys][settinglink_dual_sys_index].apn_len
                    &&(0 == strncmp(MMICONNECTION_INVALID_GATEWAY_FOR_SNTP, (char*)s_connection_setting_info.setting_detail[dual_sys][settinglink_dual_sys_index].gateway,strlen(MMICONNECTION_INVALID_GATEWAY_FOR_SNTP)))
                )
                {
                    //排除一种异常的直连情况，国外使用默认互联网CMCC的时候，提示不存在
#ifndef WIN32   //only mcc = 460,apn is cmnet;other mcc is use default,not really in china
                    if(460 != cur_service_plmn.mcc  
                    && (0==strncmp(MMICONNECTION_CMCC_DERECT_LINK_APN,(char*)s_connection_setting_info.setting_detail[dual_sys][settinglink_dual_sys_index].apn,strlen(MMICONNECTION_CMCC_DERECT_LINK_APN))))     
                    {
                        continue;
                    }
#endif
                    if(total_settinglink_num >= max_item_num)
                    {
                        break;
                    }
                    
                    detail_item[total_settinglink_num].dual_sys = dual_sys;
                    detail_item[total_settinglink_num].item_index = settinglink_dual_sys_index;
                    total_settinglink_num++;
                }
                //SCI_TRACE_LOW:"MMIAUT::SearchDirectLinkAccountIndex total_settinglink_num =%d,dual_sys =%d,settinglink_dual_sys_index=%d"
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6820_112_2_18_2_12_19_24,(uint8*)"ddd",total_settinglink_num,dual_sys,settinglink_dual_sys_index);
            }
        }
    }
    * item_num = total_settinglink_num;
    
    //SCI_TRACE_LOW:"MMIAUT::SearchDirectLinkAccountIndex * item_num =%d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_6826_112_2_18_2_12_19_25,(uint8*)"d",* item_num);
}

#endif
/*****************************************************************************/
//  Description : Select SIM API of Connection app
//  Global resource dependence : 
//  Author:sam.hua
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E MMICONNECTION_SelectSIMFunc(void)
{
#if !defined(MMI_MULTI_SIM_SYS_SINGLE)
    MMI_WIN_ID_T    selectSimWinID = MMICONNECTION_COM_SELECT_SIM_WIN_ID;
    MMIPHONE_SELECT_SIM_DATA_T  caller_data={0};

    caller_data.append_type = (uint32)MMISET_APPEND_SIM_OK;
    caller_data.callback_func =ConnectionSelectSimCallback ;
    caller_data.select_type= MMIPHONE_SELECTION_SIM_ONLY;

    caller_data.publist_id = MMICONNECTION_COM_SELECT_SIM_CTRL_ID;
    caller_data.win_id_ptr = &selectSimWinID;
    caller_data.user_data  = PNULL;
    caller_data.extra_data = PNULL;
    
    MMIAPIPHONE_OpenSelectSimWin(&caller_data);

#endif
    return MMI_RESULT_TRUE;
}


//////////////////////////////////////////////////////////////////////////
// added by feng.xiao
#ifdef DATA_ROAMING_SUPPORT
/*****************************************************************************/
// 	Description : handle data roaming query window
//	Global resource dependence : none
//  Author: feng.xiao
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDataRoamingQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E	res = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
		{
            MMIAPICONNECTION_Setting_SetDataRoamingStatus(MMICONNECTION_SETTING_DATA_ROAMING_ENABLE);
            MMK_CloseWin(win_id);
			break;
		}
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
    default:
        {
            res = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param );
            break;
        }
    }
    return res;
}

/*****************************************************************************/
// 	Description : handle data roaming cut off query window
//	Global resource dependence : none
//  Author: feng.xiao
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDataRoamingCutoffQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E	res = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
		{
            MMIAPIPDP_ReleaseAll(NULL);
            MMIAPICONNECTION_Setting_SetDataRoamingStatus(MMICONNECTION_SETTING_DATA_ROAMING_DISABLE);
            MMK_CloseWin(win_id);
			break;
		}
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
    default:
        {
            res = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param ); 
            break;
        }
    }
    return res;
}
#endif

#if defined(DATA_ACCOUNT_IN_TCARD_SUPPORT)
#if defined(BROWSER_SUPPORT)
/*****************************************************************************/
// 	Description : CheckNetworkAccountResFileToDownload
//	Global resource dependence : none
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL void CheckNetworkAccountResFileToDownload(void)
{
    uint16 ntac_file_path_w[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
    char ntac_file_path[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
    uint16 file_path_len = 0;
    
    strncpy( ntac_file_path,NETACCOUNT_FILE_PATH,strlen((char*)NETACCOUNT_FILE_PATH));
    strcat(ntac_file_path,".ntac");

    MMI_STRNTOWSTR(ntac_file_path_w, MMIFILE_FULL_PATH_MAX_LEN, (uint8*)ntac_file_path, MMIFILE_FULL_PATH_MAX_LEN, MMIFILE_FULL_PATH_MAX_LEN );
    file_path_len = MMIAPICOM_Wstrlen((const wchar *)ntac_file_path_w);
    
    if(MMIAPIFMM_IsFileExist((const wchar *)ntac_file_path_w,file_path_len))
    {
        MMIPUB_OpenQueryTextWinByTextId(TXT_CONNECTION_REPLACE_DATA_QUERY,
                                        PNULL,
                                        HandleReplaceExistFileQueryWinMsg);
    }
    else
    {
        MMICONNECTION_OpenNetAccountDownLoadUrl();
    }
}

/*****************************************************************************/
// 	Description : handle refrsh setting list query window
//	Global resource dependence : none
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleReplaceExistFileQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
        MMICONNECTION_OpenNetAccountDownLoadUrl();
        MMK_CloseWin(win_id);
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

    default:
        res = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param );
        break;
    }

    return res;
}

/*******************************************************************/
//  Interface:	MMICONNECTION_OpenNetAccountDownLoadUrl
//  Description : 	mmi connection Open Net account  DownLoad Url
//  Global resource dependence : 
//  Author:dave.ruan
//  Note:
/*******************************************************************/
PUBLIC void MMICONNECTION_OpenNetAccountDownLoadUrl(void)
{
    uint8 *url_ptr = PNULL;
    MMIBROWSER_ENTRY_PARAM entry_param = {0};/*lint !e64*/
    uint16 str_len = (uint16)(MMIAPICOM_Wstrlen(NETACCOUNT_FILE_DOWNLOAD_URL) * 3 + 1);

    url_ptr = SCI_ALLOCAZ(str_len * sizeof(uint8));	
    if (PNULL != url_ptr)
    {
    	GUI_WstrToUTF8(url_ptr, str_len, NETACCOUNT_FILE_DOWNLOAD_URL, MMIAPICOM_Wstrlen(NETACCOUNT_FILE_DOWNLOAD_URL));
    	
    	entry_param.type = MMIBROWSER_ACCESS_URL;
    	entry_param.dual_sys = MN_DUAL_SYS_MAX;
    	entry_param.url_ptr = (char *)url_ptr;
    	entry_param.user_agent_ptr = PNULL;
    	
    	MMIAPIBROWSER_Entry(&entry_param);
    	SCI_FREE(url_ptr);
    }
}

/*******************************************************************/
//  Interface:		HandleNetworkAccountDownloadCB
//  Description : 	Handle Network Account DownloadCB
//  Global resource dependence : 
//  Author: dave.ruan
//  Note:
/*******************************************************************/
LOCAL void HandleNetworkAccountDownloadCB(
								   MMIBROWSER_MIME_CALLBACK_PARAM_T *param_ptr
								   )
{    
    if (PNULL != param_ptr && MMIBROWSER_MIME_TYPE_NTAC == param_ptr->mime_type)
    {
        //SCI_TRACE_LOW:"HandleNetworkAccountDownloadCB enter"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMICONN_MANAGER_WINTAB_7030_112_2_18_2_12_19_26,(uint8*)"");
        MoveFileToNetworkAccount(param_ptr->file_name);
        MMIAPIBROWSER_Close();
        MMIPUB_OpenQueryTextWinByTextId(TXT_CONNECTION_REFRESH_ACCOUNT,
                                        PNULL,
                                        HandleRefreshSettingListQueryWinMsg);
    }
}
#endif

/*******************************************************************/
//  Description : 	MMICONNECTION_RegBrowserCB
//  Global resource dependence : 
//  Author: dave.ruan
//  Note:
/*******************************************************************/
PUBLIC void MMICONNECTION_RegBrowserCB(void)
{
#ifdef BROWSER_SUPPORT    
	MMIBROWSER_MIME_DOWNLOAD_PARAM_T ntac_callback = {0};

	ntac_callback.call_back = HandleNetworkAccountDownloadCB;	
	ntac_callback.mime_type = MMIBROWSER_MIME_TYPE_NTAC;
	MMIAPIBROWSER_RegMimeFunc(&ntac_callback);
#endif
}

/*******************************************************************/
//  Interface:		MoveFileToNetworkAccount
//  Description : 	网络下载的数据优先存储在T卡,相同文件名可以覆盖。
//  Global resource dependence : 
//  Author: dave.ruan
//  Note:
/*******************************************************************/
LOCAL void MoveFileToNetworkAccount(const wchar *file_path)
{
    wchar *dest_file_path_ptr = PNULL;
    wchar   file_name[MMIFILE_FILE_NAME_MAX_LEN+1]   = {0};
    uint16  file_name_len = 0;
    uint16  file_path_len = 0;
    uint32  file_size = 0;
    SFS_ERROR_E sfs_error = SFS_NO_ERROR;
    wchar new_full_path[MMIFILE_FULL_PATH_MAX_LEN+1]   = {0};
    uint16 ntac_file_name_len = 0;
    uint16 new_full_path_len = MMIFILE_FULL_PATH_MAX_LEN+1;
    
    if(PNULL == file_path)
    {
        return;
    }
    
    file_path_len = MMIAPICOM_Wstrlen(file_path);
    MMIAPIFMM_SplitFullPath(file_path, file_path_len,PNULL, PNULL, PNULL, PNULL, 
                                    file_name, &file_name_len); 

    //check if file name is "R_NETWORK_ACCOUNT.ntac", if not rename it, then copy to dest dir
    ntac_file_name_len  = strlen(NETACCOUNT_FILE_NAME);
    if(0 != MMIAPICOM_WstrStrncmp((const wchar*)file_name, (const uint8*)NETACCOUNT_FILE_NAME, ntac_file_name_len))
    {
        wchar ntac_file_name_w[MMIFILE_FULL_PATH_MAX_LEN+1] = {0};
        wchar  device_name[MMIFILE_DEVICE_NAME_MAX_LEN+2] = {0};
        uint16  device_name_len = 0;
        wchar  dir_name[MMIFILE_DIR_NAME_MAX_LEN+2] = {0};
        uint16  dir_name_len = 0;
        
        MMI_STRNTOWSTR(
                                ntac_file_name_w, 
                                MMIFILE_FULL_PATH_MAX_LEN,
                                (uint8*)NETACCOUNT_FILE_NAME, 
                                MMIFILE_FULL_PATH_MAX_LEN,
                                ntac_file_name_len);

        //get real file name
        if(MMIAPIFMM_SplitFullPath(file_path, file_path_len,
                             device_name, &device_name_len,
                             dir_name, &dir_name_len,
                             NULL, NULL))
        {
            MMIAPIFMM_CombineFullPath(device_name, device_name_len,
                                   dir_name, dir_name_len,
                                   ntac_file_name_w, ntac_file_name_len,
                                   new_full_path, &new_full_path_len);
            file_path_len = new_full_path_len;
            
            if(MMIAPIFMM_IsFileExist(new_full_path,new_full_path_len))
            {
                sfs_error = MMIAPIFMM_DeleteFile(new_full_path, NULL);
            }
            
            if (SFS_NO_ERROR == sfs_error)
            {
                if(MMIAPIFMM_RenameFile(file_path,MMIAPICOM_Wstrlen(file_path),
                                                            ntac_file_name_w,strlen(NETACCOUNT_FILE_NAME)))
                {
                    //use new_full_path instead of file_path
                }
                else
                {
                    SCI_MEMSET(new_full_path, 0x00, MMIFILE_FILE_NAME_MAX_LEN);
                    MMI_WSTRNCPY(
                                                new_full_path, 
                                                MMIFILE_FILE_NAME_MAX_LEN, 
                                                file_path, 
                                                MMIFILE_FILE_NAME_MAX_LEN,
                                                file_path_len);
                }
            }
            else
            {
                SCI_MEMSET(new_full_path, 0x00, MMIFILE_FILE_NAME_MAX_LEN);
                MMI_WSTRNCPY(
                                            new_full_path, 
                                            MMIFILE_FILE_NAME_MAX_LEN, 
                                            file_path, 
                                            MMIFILE_FILE_NAME_MAX_LEN,
                                            file_path_len);
            }
        }
    }
    else
    {
        //use file_path
        SCI_MEMSET(new_full_path, 0x00, MMIFILE_FILE_NAME_MAX_LEN);
        MMI_WSTRNCPY(
                                    new_full_path, 
                                    MMIFILE_FILE_NAME_MAX_LEN, 
                                    file_path, 
                                    MMIFILE_FILE_NAME_MAX_LEN,
                                    file_path_len);
    }

    dest_file_path_ptr = (wchar *)SCI_ALLOCAZ((MMIFILE_FILE_NAME_MAX_LEN + 1)*sizeof(wchar));
    if(PNULL == dest_file_path_ptr)
    {
        return;
    }
    
    MMIAPIFMM_GetFileInfo(new_full_path,file_path_len,&file_size,PNULL,PNULL);

    MMIAPIFMM_SplitFullPath(new_full_path, file_path_len,PNULL, PNULL, PNULL, PNULL, 
                                    file_name, &file_name_len); 

    //dest_file_path is set in GetValidNetAccountFileDir
    if(GetValidNetAccountFileDir(dest_file_path_ptr, file_name, file_size))
    {
        sfs_error = MMIAPIFMM_CopyFile(new_full_path, dest_file_path_ptr);
        if (SFS_NO_ERROR == sfs_error)
        {
            sfs_error = MMIAPIFMM_DeleteFile(new_full_path, NULL);
        }
        else if (SFS_ERROR_NO_SPACE == sfs_error)
        {
           MMIPUB_OpenAlertWarningWin( TXT_NO_SPACE);
        }
        else
        {
           MMIPUB_OpenAlertWarningWin(TXT_MMI_SAVEFAILED);
        }
    }
    else
    {
        MMIPUB_OpenAlertWarningWin(TXT_NO_SPACE);
    }
    
    SCI_FREE(dest_file_path_ptr);
}

/*****************************************************************************/
//  Description : get location file dir
//  Global resource dependence : 
//  Author:dave.ruan
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN GetValidNetAccountFileDir(uint16* device_dir,const wchar *file_name_ptr,uint32 file_size)
{
    MMIFILE_DEVICE_E file_dev_type = MMIAPIFMM_GetDefaultDisk();
    BOOLEAN          result = TRUE;
    
    if(MMIAPIFMM_IsEnoughSpace(MMIFMM_STORE_TYPE_FIXED, file_dev_type, file_size, PNULL))
    {
        switch (file_dev_type)
        {
        case MMI_DEVICE_UDISK:
            MMIAPICOM_Wstrncpy(device_dir,NETACCOUNT_FILE_UDISK_PATH,MMIAPICOM_Wstrlen(NETACCOUNT_FILE_UDISK_PATH));
            break;
        case MMI_DEVICE_SDCARD:
            MMIAPICOM_Wstrncpy(device_dir,NETACCOUNT_FILE_TCARD_PATH,MMIAPICOM_Wstrlen(NETACCOUNT_FILE_TCARD_PATH));
            break;
        case MMI_DEVICE_SDCARD_1:
            MMIAPICOM_Wstrncpy(device_dir,NETACCOUNT_FILE_TCARD1_PATH,MMIAPICOM_Wstrlen(NETACCOUNT_FILE_TCARD1_PATH));
            break;
        default:
            MMIAPICOM_Wstrncpy(device_dir,NETACCOUNT_FILE_UDISK_PATH,MMIAPICOM_Wstrlen(NETACCOUNT_FILE_UDISK_PATH));
            break;
        }
        MMIAPICOM_Wstrcat(device_dir, file_name_ptr);
    }
    else
    {
        result = FALSE;
    }
    return result;
}

/*****************************************************************************/
// 	Description : handle refrsh setting list query window
//	Global resource dependence : none
//    Author: dave.ruan
//	Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleRefreshSettingListQueryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_OK:
    case MSG_APP_OK:
        MMICONNECTION_RefreshSettingList();
        MMK_CloseWin(win_id);
        break;

    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

    default:
        res = MMIPUB_HandleQueryWinMsg( win_id, msg_id, param );
        break;
    }

    return res;
}

/*******************************************************************/
//  Description : 	MMICONNECTION_RefreshSettingList
//  Global resource dependence : 
//  Author: dave.ruan
//  Note: refresh  setting list after load list
/*******************************************************************/
PUBLIC void MMICONNECTION_RefreshSettingList(void)
{
#ifdef ACCOUNT_AUTO_ADAPT_SUPPORT
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    PHONE_PLMN_NAME_E conection_plmn_name[MMI_DUAL_SYS_MAX] = {0};
    
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;

    MMINV_READ(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name, return_value);

    if (return_value != MN_RETURN_SUCCESS)
    {
        SCI_MEMSET(conection_plmn_name, 0x00, sizeof(conection_plmn_name));
    }
   
    for(dual_sys = MN_DUAL_SYS_1; dual_sys < MMI_DUAL_SYS_MAX;dual_sys++)
    {
        if(MMIAPIPHONE_IsSimCardOk(dual_sys))
        {
            conection_plmn_name[dual_sys] = 0x00;
            MMINV_WRITE(MMINV_CONNECTION_CONNECTION_PLMN, conection_plmn_name);
            MMICONNECTION_AutoAdapting(dual_sys);
        }
    }
#endif    
    MMIPUB_OpenAlertSuccessWin(TXT_COMPLETE);
}

#endif
