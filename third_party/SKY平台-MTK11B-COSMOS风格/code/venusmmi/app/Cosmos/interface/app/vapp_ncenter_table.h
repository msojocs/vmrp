/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES. ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 * Filename:
 * ---------
 *  vapp_ncenter_table.h
 *
 * Project:
 * --------
 *  COSMOS Notification Center
 *
 * Description:
 * ------------
 * 
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __VAPP_NCENTER_TABLE_H__
#define __VAPP_NCENTER_TABLE_H__

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "MMI_features.h"

#if defined(NCENTER_TABLE_DEFINED)

// please add your head here
#include "vapp_ncenter_test_cell.h"
#include "vapp_ss.h"
#include "vapp_ucm_def.h"
#ifdef __MMI_BT_SUPPORT__
#include "vapp_bt_ncenter_cell.h"
#endif
#include "vapp_clog_gprot.h"
#ifdef __MMI_FM_RADIO__
#include "vapp_fm_radio_ncenter.h"
#endif
#ifdef __J2ME__
#include "vapp_javaagencysrc.h"
#endif
#include "vapp_msg_gprot.h"
#include "vapp_mre_app_ncenter.h"
#ifdef __MMI_EMAIL__
#include "vapp_email_ncenter.h"
#endif
#include "vapp_music_player_ncentercell.h"
#include "vapp_sms_notifier_scrn.h"
#ifdef WAP_SUPPORT
#include "vapp_wap_push_notification.h"
#endif
#include "vapp_phb_backup_restore.h"
#ifdef __SYNCML_SUPPORT__
#include "vapp_sync_ncenter.h"
#endif
#if defined (__MMI_DOWNLOAD_AGENT__) && defined (BROWSER_SUPPORT)
#include "..\DLAgent\vapp_dla_ncenter.h"
#endif
#include "Vapp_sat_ncenter.h"

#ifdef __MMI_NITZ__
#include "vapp_nitz_gprot.h"
#endif

#ifdef __SOCIAL_NETWORK_SUPPORT__
#include "vapp_sns_gprot.h"
#endif /* __SOCIAL_NETWORK_SUPPORT__ */

#ifdef __MMI_DSM_NEW__
#include "vapp_mythroad_ncenter.h"
#endif
/***************************************************************************** 
 * Define
 *****************************************************************************/
 
#define VAPP_NCENTER_EVENT_CELL_TABLE    static const vapp_ncenter_cell_cfg_struct g_vapp_ncenter_event_cell_create_tbl[] =
#define VAPP_NCENTER_ONGOING_CELL_TABLE    static const vapp_ncenter_cell_cfg_struct g_vapp_ncenter_ongoing_cell_create_tbl[] =

#define VAPP_NCENTER_DECLARE_CELL(_class)     \
    {                                     \
        _class##::createCell              \
    }

#define VAPP_NCENTER_EVENT_LAST_ITEM
#define VAPP_NCENTER_ONGOING_LAST_ITEM

#define VAPP_NCENTER_EVENT_CELL_TOTAL        (sizeof(g_vapp_ncenter_event_cell_create_tbl) / sizeof(vapp_ncenter_cell_cfg_struct))
#define VAPP_NCENTER_ONGOING_CELL_TOTAL        (sizeof(g_vapp_ncenter_ongoing_cell_create_tbl) / sizeof(vapp_ncenter_cell_cfg_struct))

#else /* defined(NCENTER_TABLE_DEFINED) */

typedef void *(*vapp_ncenter_cell_create_cb)(void *parentObj);

typedef struct
{
    vapp_ncenter_cell_create_cb        create;
} vapp_ncenter_cell_cfg_struct;

#define VAPP_NCENTER_EVENT_CELL_TABLE        enum
#define VAPP_NCENTER_ONGOING_CELL_TABLE        enum
#define VAPP_NCENTER_DECLARE_CELL(_class)  VAPP_NCENTER_CELL_ENUM_##_class
#endif /* defined(NCENTER_TABLE_DEFINED) */


/***************************************************************************** 
 * Cell table
 *****************************************************************************/
// notice that if you cannot use one class register two table at the same time 

VAPP_NCENTER_EVENT_CELL_TABLE
{
    // register cell here
    VAPP_NCENTER_DECLARE_CELL(VappFirstCell),
    VAPP_NCENTER_DECLARE_CELL(VappSecondCell),
    VAPP_NCENTER_DECLARE_CELL(VappMREFirstCell),
    VAPP_NCENTER_DECLARE_CELL(VappMRESecondCell),
    VAPP_NCENTER_DECLARE_CELL(VappMREThirdCell),
    VAPP_NCENTER_DECLARE_CELL(VappMREFourthCell),
#ifdef __J2ME__
    VAPP_NCENTER_DECLARE_CELL(VappJavaEventCell),
#endif
    VAPP_NCENTER_DECLARE_CELL(VappSsNCellSim1),
#if (MMI_MAX_SIM_NUM >= 2)
    VAPP_NCENTER_DECLARE_CELL(VappSsNCellSim2),
#if (MMI_MAX_SIM_NUM >= 3)
        VAPP_NCENTER_DECLARE_CELL(VappSsNCellSim3),
#if (MMI_MAX_SIM_NUM >= 4)
        VAPP_NCENTER_DECLARE_CELL(VappSsNCellSim4),
#endif /* (MMI_MAX_SIM_NUM >= 4) */
#endif /* (MMI_MAX_SIM_NUM >= 3) */
#endif /* (MMI_MAX_SIM_NUM >= 2) */
#ifdef __MMI_BT_SUPPORT__
    VAPP_NCENTER_DECLARE_CELL(VappBtRecvLocalCell),
    VAPP_NCENTER_DECLARE_CELL(VappBtRecvExternalCell),
#endif
    VAPP_NCENTER_DECLARE_CELL(VappClogNCenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgUnreadNcenterCell), 
    VAPP_NCENTER_DECLARE_CELL(VappMsgUnsentNcenterCell), 
    VAPP_NCENTER_DECLARE_CELL(VappMsgSim1SmsFullNcenterCell), 
#if (MMI_MAX_SIM_NUM >= 2)
    VAPP_NCENTER_DECLARE_CELL(VappMsgSim2SmsFullNcenterCell), 
#endif 
#if (MMI_MAX_SIM_NUM >= 3)
    VAPP_NCENTER_DECLARE_CELL(VappMsgSim3SmsFullNcenterCell), 
#endif
#if (MMI_MAX_SIM_NUM >= 4)
    VAPP_NCENTER_DECLARE_CELL(VappMsgSim4SmsFullNcenterCell), 
#endif
    VAPP_NCENTER_DECLARE_CELL(VappMsgPhoneSmsFullNcenterCell), 
#ifdef __MMI_MMS_IN_UM__
    VAPP_NCENTER_DECLARE_CELL(VappMsgPhoneMmsFullNcenterCell), 
    VAPP_NCENTER_DECLARE_CELL(VappMsgCardMmsFullNcenterCell), 
#endif /* __MMI_MMS_IN_UM__ */
#ifdef __MMI_PROV_IN_UM__
    VAPP_NCENTER_DECLARE_CELL(VappMsgProvFullNcenterCell), 
#endif /* __MMI_PROV_IN_UM__ */
#ifdef __MMI_EMAIL__
    VAPP_NCENTER_DECLARE_CELL(VappEmailNcUnread),
    VAPP_NCENTER_DECLARE_CELL(VappEmailNcSendFail),
    VAPP_NCENTER_DECLARE_CELL(VappEmailNcRefreshFail),
    #ifdef __MMI_EMAIL_IMAP_PUSH__
    VAPP_NCENTER_DECLARE_CELL(VappEmailNcPushRefreshFail),
    #endif
#endif /* __MMI_EMAIL__ */
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim1Line1Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim1Line2Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingFaxSim1Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingEmailSim1Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingNetMsgSim1Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVedioSim1Cell),
#if (MMI_MAX_SIM_NUM >= 2)
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim2Line1Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim2Line2Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingFaxSim2Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingEmailSim2Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingNetMsgSim2Cell),
    VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVedioSim2Cell),
#endif
#if (MMI_MAX_SIM_NUM >= 3)
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim3Line1Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim3Line2Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingFaxSim3Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingEmailSim3Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingNetMsgSim3Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVedioSim3Cell),
#endif
#if (MMI_MAX_SIM_NUM >= 4)
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim4Line1Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVoiceSim4Line2Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingFaxSim4Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingEmailSim4Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingNetMsgSim4Cell),
	VAPP_NCENTER_DECLARE_CELL(VappMsgWaitingVedioSim4Cell),
#endif
#ifdef WAP_SUPPORT
    VAPP_NCENTER_DECLARE_CELL(VappWapPushNCenterCell),
#endif
    VAPP_NCENTER_DECLARE_CELL(VappContactMultiProcessingNCenterCell),
#ifdef __SYNCML_SUPPORT__
    VAPP_NCENTER_DECLARE_CELL(VappSyncNCenterUnfinishCell),
    VAPP_NCENTER_DECLARE_CELL(VappSyncNCenterRegularCell),
#endif
#if defined (__MMI_DOWNLOAD_AGENT__) && defined (BROWSER_SUPPORT)
    VAPP_NCENTER_DECLARE_CELL(VcpDLAgentNCenterPushObjectCell),
    VAPP_NCENTER_DECLARE_CELL(VcpDLAgentNCenterFailedCell),
    VAPP_NCENTER_DECLARE_CELL(VcpDLAgentNCenterCompletedCell),
#endif
    VAPP_NCENTER_DECLARE_CELL(VappSatNCenterEventCell1),
#if (MMI_MAX_SIM_NUM >= 2)
    VAPP_NCENTER_DECLARE_CELL(VappSatNCenterEventCell2),
#if (MMI_MAX_SIM_NUM >= 3)
    VAPP_NCENTER_DECLARE_CELL(VappSatNCenterEventCell3),
#if (MMI_MAX_SIM_NUM >= 4)
    VAPP_NCENTER_DECLARE_CELL(VappSatNCenterEventCell4),
#endif
#endif
#endif /* MMI_MAX_SIM_NUM >= 2 */
#ifdef __MMI_NITZ__
    VAPP_NCENTER_DECLARE_CELL(VappNitzNCenterEventCell),
#endif
#if defined(__MMI_SNS_PHOTO_UPLOAD__)
    VAPP_NCENTER_DECLARE_CELL(VcuiSnsUploadNCenterCell),
#endif
#ifdef __MMI_SNS_CENTER__
    VAPP_NCENTER_DECLARE_CELL(VappSnsUnpostedNCenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappSnsUnpostedCmmtNCenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappSnsNewReqNCenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappSnsNotifyNCenterCell),
#endif /* __MMI_SNS_CENTER__ */
#ifdef __MMI_SNS_MESSAGES__
    VAPP_NCENTER_DECLARE_CELL(VappSnsNewMsgNCenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappSnsUnsentMsgNCenterCell),
#endif /* __MMI_SNS_MESSAGES__ */
    VAPP_NCENTER_EVENT_LAST_ITEM
};


VAPP_NCENTER_ONGOING_CELL_TABLE
{
    // register cell here
#if defined(NCENTER_TABLE_DEFINED)    
    VAPP_NCENTER_DECLARE_CELL(VappThirdCell),
#else
    VAPP_NCENTER_DECLARE_CELL(VappThirdCell) = VAPP_NCENTER_EVENT_LAST_ITEM,
#endif
    VAPP_NCENTER_DECLARE_CELL(VappFourthCell),
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell1),   
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell2),    
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell3),   
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell4),   
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell5),   
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell6),   
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell7),        
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell8),   
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell9),  
    VAPP_NCENTER_DECLARE_CELL(VappMREOngoingCell10),
#ifdef __J2ME__
    VAPP_NCENTER_DECLARE_CELL(VappJavaOngoingCell1),
    VAPP_NCENTER_DECLARE_CELL(VappJavaOngoingCell2),
    VAPP_NCENTER_DECLARE_CELL(VappJavaOngoingCell3),
#endif
    VAPP_NCENTER_DECLARE_CELL(VappUcmNCenterCell),
#ifdef __MMI_OPP_SUPPORT__
    VAPP_NCENTER_DECLARE_CELL(VappBtSendingCell),
#endif
#ifdef __MMI_FM_RADIO__
    VAPP_NCENTER_DECLARE_CELL(VappFMRadioNCenterCell),
#endif
#ifdef __MMI_EMAIL__
    VAPP_NCENTER_DECLARE_CELL(VappEmailNcSending),
    VAPP_NCENTER_DECLARE_CELL(VappEmailNcRefreshing),
#endif /* __MMI_EMAIL__ */
    VAPP_NCENTER_DECLARE_CELL(VappMusicPlayerNCenterCell),
#ifdef __SYNCML_SUPPORT__
    VAPP_NCENTER_DECLARE_CELL(VappSyncNCenterOngoingCell),
#endif
#if defined (__MMI_DOWNLOAD_AGENT__) && defined (BROWSER_SUPPORT)
    VAPP_NCENTER_DECLARE_CELL(VcpDLAgentNCenterDownloadingCell),
#endif
#ifdef __MMI_DSM_NEW__
    VAPP_NCENTER_DECLARE_CELL(VappMyThroadNcenterCell),
#ifdef __PME_SUPPORT__
    VAPP_NCENTER_DECLARE_CELL(VappMyThroadPmeImNcenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappMyThroadPmeEmailNcenterCell),
    VAPP_NCENTER_DECLARE_CELL(VappMyThroadPmeSystemNcenterCell),
#endif
#endif/*__MMI_DSM_NEW__*/
    VAPP_NCENTER_ONGOING_LAST_ITEM
};

#if defined(NCENTER_TABLE_DEFINED)    
#else
#define VAPP_NCENTER_EVENT_CELL_TOTAL VAPP_NCENTER_EVENT_LAST_ITEM
#define VAPP_NCENTER_ONGOING_CELL_TOTAL VAPP_NCENTER_ONGOING_LAST_ITEM - VAPP_NCENTER_EVENT_LAST_ITEM
#endif
/***************************************************************************** 
 * External interface
 *****************************************************************************/

extern const vapp_ncenter_cell_cfg_struct *vapp_ncenter_get_event_cell_table(void);

extern const vapp_ncenter_cell_cfg_struct *vapp_ncenter_get_ongoing_cell_table(void);



#endif /* __VAPP_NCENTER_TABLE_H__ */

