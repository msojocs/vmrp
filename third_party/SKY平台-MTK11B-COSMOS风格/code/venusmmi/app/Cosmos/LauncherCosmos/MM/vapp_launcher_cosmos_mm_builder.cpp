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
 *  vapp_launcher_cosmos_mm_builder.cpp
 *
 * Project:
 * --------
 *  Cosmos Launcher 2.0 - Launcher Cosmos
 *
 * Description:
 * ------------
 *  Launcher Cosmos Main Menu Builder
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/


#include "MMI_features.h"

#ifdef __MMI_VUI_LAUNCHER_COSMOS__


/*****************************************************************************
 * Include
 *****************************************************************************/

#include "vrt_datatype.h"
#include "vrt_system.h"

#include "vfx_timer.h"

#include "vcp_include.h"
#include "vcp_page_menu_util.h"

#include "vapp_launcher_cosmos_mm_builder.h"
#include "vapp_launcher_cosmos_mm_default_order.h"
#include "vapp_launcher_cosmos_mm_gprot.h"
#include "vapp_launcher_cosmos_mm_page_menu.h"
#include "vapp_launcher_cosmos_mm_shortcut_bar.h"
#include "mmi_rp_vapp_launcher_cosmos_mm_def.h"

extern "C"
{
#include "AppMgrSrvGprot.h"
#include "CallLogSrvGprot.h"
#ifdef __MMI_EMAIL__
#include "EmailSrvGprot.h"
#endif // __MMI_EMAIL__
#include "UmSrvGprot.h"
}

#ifdef __MTK_TARGET__
//#define __MAUI_SOFTWARE_LA__
#endif // __MTK_TARGET__

#ifdef __MAUI_SOFTWARE_LA__
extern "C"
{
#include "SST_sla.h"
}
#endif 


/*****************************************************************************
 * Define
 *****************************************************************************/

//#define VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST
//#define VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST_PAGE_COUNT 10
//#define VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST_APP_COUNT_PER_PAGE 16


/*****************************************************************************
 * Global Variable
 *****************************************************************************/

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_DELAY_CREATE_COUNTER_TIME = 800;

// cell content layout
#if defined(__MMI_MAINLCD_480X800__)
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH = 120;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_HEIGHT = 122;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH = 72;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_HEIGHT = 72;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X = (VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH) / 2;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y = 11;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_X = 8; // (120 - 104) / 2
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_Y = 0;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_WIDTH = 26;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_HEIGHT = 26;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_X = 39;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_Y = 40;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN = 2;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_X = VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_Y = 88;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_WIDTH = VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_HEIGHT = 23 + 3 * 2;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN = 8;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X = VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN + VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y = 93;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH = VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_HEIGHT = 23;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_OFFSET_Y = 6;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_3DIGITS_WIDTH = 65;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_FONT_SIZE = 23;

#elif defined(__MMI_MAINLCD_240X320__) || defined(__MMI_MAINLCD_240X400__)
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH = 60;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_HEIGHT = 73;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH = 38;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_HEIGHT = 38;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X = (VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH) / 2;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y = 10;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_X = 0; // (60 - 60) / 2
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_Y = 0;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_WIDTH = 12;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_HEIGHT = 12;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_X = 22;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_Y = 22;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN = 0;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_X = VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_Y = 51;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_WIDTH = VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_HEIGHT = 15;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN = 3;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X = VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN + VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y = 52;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH = VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_HEIGHT = 13;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_OFFSET_Y = 6;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_3DIGITS_WIDTH = 34;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_FONT_SIZE = 12;

#else // default HVGA 320X480
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH = 80;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_HEIGHT = 90;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH = 48;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_HEIGHT = 48;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X = (VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH) / 2;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y = 13;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_X = 0; // (80 - 80) / 2
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_Y = 0;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_WIDTH = 18;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_HEIGHT = 18;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_X = 26;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_Y = 26;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN = 1;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_X = VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_Y = 65;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_WIDTH = VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_HEIGHT = 19;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN = 5;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X = VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN + VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y = 67;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH = VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BASE_X_MARGIN - 2 * VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_HEIGHT = 15;

static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_OFFSET_Y = 5;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_3DIGITS_WIDTH = 42;
static const VfxS32 VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_FONT_SIZE = 15;

#endif


/***************************************************************************** 
 * Static Declaration
 *****************************************************************************/

#ifdef __MMI_EMAIL__
static void srvEmailAllMsgUnreadNotifyCallback(srv_email_all_msg_unread_notify_type type, U32 num, void *userdata);
static void srvEmailAppInitStateCallback(srv_email_init_state_enum state, S32 err_type, void *user_data);
#endif


/***************************************************************************** 
 * Local Function
 *****************************************************************************/

#ifdef __MMI_EMAIL__
void srvEmailAllMsgUnreadNotifyCallback(srv_email_all_msg_unread_notify_type type, U32 num, void *userdata)
{
    vapp_launcher_cosmos_mm_email_unread_notify_evt_struct evt;
    
    MMI_FRM_INIT_EVENT(&evt, EVT_ID_VAPP_LAUNCHER_COSMOS_MM_EMAIL_UNREAD_NOTIFY);
    MMI_FRM_CB_EMIT_EVENT(&evt);
}

void srvEmailAppInitStateCallback( srv_email_init_state_enum state, S32 err_type, void *user_data)
{
    if (state == SRV_EMAIL_INIT_STATE_READY)
    {
        vapp_launcher_cosmos_mm_email_unread_notify_evt_struct evt;
        
        MMI_FRM_INIT_EVENT(&evt, EVT_ID_VAPP_LAUNCHER_COSMOS_MM_EMAIL_UNREAD_NOTIFY);
        MMI_FRM_CB_EMIT_EVENT(&evt);

        srv_email_acct_cache_notify_set(srvEmailAllMsgUnreadNotifyCallback, NULL);
    }
}
#endif // __MMI_EMAIL__


/***************************************************************************** 
 * Global Function
 *****************************************************************************/

VappLauncherCosmosMmCell *vappLauncherCosmosMmCreateCellRaw(VfxImageSrc image, const VfxWString &title, VfxFrame *parent)
{
    // create the cell
    VappLauncherCosmosMmCell *cell;
    VFX_OBJ_CREATE_EX(cell, VappLauncherCosmosMmCell, parent, (image, title));

    return cell;
}

VappLauncherCosmosMmCell *vappLauncherCosmosMmCreateCell(mmi_app_package_name_struct &appName, VfxObject *parent, VfxBool preinstall)
{
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM6", SA_start);
#endif
    // get app info
    VfxImageSrc image;
    VfxWString title;
    srv_app_info_struct info;
    
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM7", SA_start);
#endif
    
    mmi_ret result = srv_appmgr_get_app_package_info(appName, &info);
    
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM7", SA_stop);
    SLA_CustomLogging("MM7", SA_start);
#endif
    
    VFX_ASSERT(result != MMI_RET_ERR);
    if (info.image.type == MMI_IMAGE_SRC_TYPE_RES_ID)
    {
        image = VfxImageSrc(info.image.data.res_id);
    }
    else if (info.image.type == MMI_IMAGE_SRC_TYPE_PATH)
    {
        VfxWString path;
        path.loadFromMem(info.image.data.path);
        image = VfxImageSrc(path);
    }
    else
    {
        VFX_ASSERT(VFX_FALSE);
    }
    title.loadFromMem(info.string);
    
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM7", SA_stop);
#endif

    if (strncmp(appName, "native.mtk.launcher", MMI_APP_NAME_MAX_LEN) == 0)
    {
        title.loadFromRes(STR_ID_VAPP_LAUNCHER_COSMOS_MM_HOME);
    }
    
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM6", SA_stop);
    SLA_CustomLogging("MM6", SA_start);
#endif

    VappLauncherCosmosMmCell * cell;
    VFX_OBJ_CREATE_EX(cell, VappLauncherCosmosMmCell, parent, (image, title));
    cell->setCellAppName(appName);
    cell->setIsPreinstall(preinstall);
    cell->setHidden(VFX_TRUE);

    // TODO: Get APP counter info from APP Manager in the future
    if (strncmp(appName, "native.mtk.msg", MMI_APP_NAME_MAX_LEN) == 0
        || strncmp(appName, "native.mtk.calllog", MMI_APP_NAME_MAX_LEN) == 0
#ifdef __MMI_EMAIL__
        || strncmp(appName, "native.mtk.vapp_email", MMI_APP_NAME_MAX_LEN) == 0
#endif // __MMI_EMAIL__
        )
    {
        cell->startDelayCreateCounter();
    }

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM6", SA_stop);
#endif
    return cell;
}

VfxBool vappLauncherCosmosMmIsPreInstalledApp(mmi_app_package_name_struct appName, mmi_app_package_name_struct preInstalledApp[], VfxU32 preInstalledAppCount)
{
    srv_appmgr_app_package_type_enum type = srv_appmgr_get_app_package_type(appName);
    if (type != SRV_APPMGR_MRE_APP_PACKAGE_TYPE)
    {
        return VFX_FALSE;
    }
    for (VfxU32 i = 0; i < preInstalledAppCount; ++i)
    {
        if (strncmp(appName, preInstalledApp[i], MMI_APP_NAME_MAX_LEN) == 0)
        {
            return VFX_TRUE;
        }
    }
    return VFX_FALSE;
}


/*****************************************************************************
 * Class VappLauncherCosmosMmCellStableFrame
 *****************************************************************************/

VFX_IMPLEMENT_CLASS("VappLauncherCosmosMmCellStableFrame", VappLauncherCosmosMmCellStableFrame, VfxFrame);

VappLauncherCosmosMmCellStableFrame::VappLauncherCosmosMmCellStableFrame(mmi_app_package_name_struct *appPackageName, VfxImageSrc *appImage, VfxWString *appString) :
    m_appPackageName(appPackageName),
    m_appImage(appImage),
    m_appString(appString),
    m_onShortcut(VFX_FALSE),
    m_isPreinstall(VFX_FALSE),
    m_isTextFrameHidden(VFX_FALSE)
{
}

void VappLauncherCosmosMmCellStableFrame::updateOnShortcut(VfxBool onShortcut)
{
    if (m_onShortcut != onShortcut)
    {
        m_onShortcut = onShortcut;

        if (m_isTextFrameHidden)
        {
            return;
        }
        
        invalidate();
    }
}

void VappLauncherCosmosMmCellStableFrame::updateIsPreinstall(VfxBool isPreinstall)
{
    if (m_isPreinstall != isPreinstall)
    {
        m_isPreinstall = isPreinstall;

        invalidate();
    }
}

void VappLauncherCosmosMmCellStableFrame::updateTextFrameHidden(VfxBool isTextFrameHidden)
{
    if (m_isTextFrameHidden != isTextFrameHidden)
    {
        m_isTextFrameHidden = isTextFrameHidden;

        invalidate();
    }
}

void VappLauncherCosmosMmCellStableFrame::onDraw(VfxDrawContext &dc)
{
    drawIcon(dc);

    drawTitle(dc);
}

void VappLauncherCosmosMmCellStableFrame::drawIcon(VfxDrawContext &dc)
{
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MDI", SA_start);
#endif

    // Draw app icon
    VfxBool isSmallIcon = VFX_FALSE;
    VfxBool isMRE = VFX_FALSE;
    VfxBool isJava = VFX_FALSE;
    VfxBool isWidget = VFX_FALSE;
    srv_appmgr_app_package_type_enum type = srv_appmgr_get_app_package_type(*m_appPackageName);
    if (type == SRV_APPMGR_MRE_APP_PACKAGE_TYPE)
    {
        isMRE = VFX_TRUE;
    }
    else if (type == SRV_APPMGR_JAVA_APP_PACKAGE_TYPE)
    {
        isJava = VFX_TRUE;
    }
#ifdef __GADGET_SUPPORT__
    else if (type == SRV_APPMGR_WIDGET_APP_PACKAGE_TYPE)
    {
        isWidget = VFX_TRUE;
    }
#endif

    if (isMRE || isJava)
    {
        if ((m_appImage->getSize().width < VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH / 2) &&
            (m_appImage->getSize().height < VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_HEIGHT / 2))
        {
            isSmallIcon = VFX_TRUE;
        }
    }

    if (isSmallIcon)
    {
        if (isMRE)
        {
            dc.drawImageFromRes(VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X,
                                VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y,
                                IMG_ID_VAPP_LAUNCHER_COSMOS_MM_ICON_MRE_RESIZE);
        }
        else if (isJava)
        {
            dc.drawImageFromRes(VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X,
                                VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y,
                                IMG_ID_VAPP_LAUNCHER_COSMOS_MM_ICON_JAVA_RESIZE);
        }

        VfxRect imageRect(VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X + VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_X,
                          VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y + VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_OFFSET_Y,
                          VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_WIDTH,
                          VAPP_LAUNCHER_COSMOS_MM_CELL_SMALL_ICON_HEIGHT);
        dc.drawResizedImage(imageRect, *m_appImage);
    }
    else
    {
        if (isMRE || isWidget || isJava)
        {
            VfxRect imageRect(VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X,
                              VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y,
                              VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_WIDTH,
                              VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_HEIGHT);
            dc.drawResizedImage(imageRect, *m_appImage);
        }
        else
        {
            dc.drawImage(VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X,
                         VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y,
                         *m_appImage);
        }
    }

    if (m_isPreinstall)
    {
        // Draw preinstall image
        dc.drawImageFromRes(VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_X,
                            VAPP_LAUNCHER_COSMOS_MM_CELL_APP_ICON_POS_Y,
                            IMG_ID_VAPP_LAUNCHER_COSMOS_MM_ICON_PREINSTALL_APP);
    }
    
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MDI", SA_stop);
#endif
}

void VappLauncherCosmosMmCellStableFrame::drawTitle(VfxDrawContext &dc)
{
    if (!m_isTextFrameHidden)
    {
    #ifdef __MAUI_SOFTWARE_LA__
        SLA_CustomLogging("MDT", SA_start);
    #endif
    
        VfxFontDesc fontDesc;
        fontDesc.size = VFX_FONT_DESC_VF_SIZE(VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_FONT_SIZE);
        dc.setFont(fontDesc);
        dc.setFillColor(VFX_COLOR_WHITE);

    #ifdef __MAUI_SOFTWARE_LA__
        SLA_CustomLogging("MDT", SA_stop);
    #endif
        
        VfxS32 stringWidth;
        if (strncmp(*m_appPackageName, "native.mtk.launcher_cosmos_mm", MMI_APP_NAME_MAX_LEN) == 0 ||
            strncmp(*m_appPackageName, "native.mtk.launcher", MMI_APP_NAME_MAX_LEN) == 0)
        {
            stringWidth = fontDesc.measureStr(*m_appString).width;
        }
        else
        {
            stringWidth = vappLauncherCosmosMmGetStringWidth(*m_appPackageName);
            if (stringWidth == -1)
            {
                stringWidth = fontDesc.measureStr(*m_appString).width;
                vappLauncherCosmosMmSetStringWidth(*m_appPackageName, stringWidth);
            }
        }
        
    #ifdef __MAUI_SOFTWARE_LA__
        SLA_CustomLogging("MDT", SA_start);
    #endif
        
        vrt_text_dir_type_enum dir = vrt_sys_get_text_dir();
    
        if (stringWidth <= VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH)
        {
            if (m_onShortcut)
            {
                VfxRect imageRect(VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_X + (VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH - stringWidth) / 2,
                                  VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_Y,
                                  stringWidth + VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_X_MARGIN * 2,
                                  VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_HEIGHT);
                dc.drawResizedImageFromRes(imageRect, IMG_ID_VAPP_LAUNCHER_COSMOS_MM_FONT_BG);
            }
            
            if (dir == MG_TEXT_DIR_TYPE_L2R)
            {
                dc.drawText(VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X + (VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH - stringWidth) / 2,
                            VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y,
                            *m_appString);
            }
            else
            {
                dc.drawText((VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 1) - (VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X + (VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH - stringWidth) / 2),
                            VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y,
                            *m_appString);
            }
        }
        else
        {
            if (m_onShortcut)
            {
                VfxRect imageRect(VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_X,
                                  VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_OFFSET_Y,
                                  VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_WIDTH,
                                  VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_BG_HEIGHT);
                dc.drawResizedImageFromRes(imageRect, IMG_ID_VAPP_LAUNCHER_COSMOS_MM_FONT_BG);
            }
            
            dc.setEllipsis(VFX_WSTR("..."));
            if (dir == MG_TEXT_DIR_TYPE_L2R)
            {
                dc.drawTextTruncated(VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X,
                                     VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y,
                                     *m_appString,
                                     VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH);
            }
            else
            {
                dc.drawTextTruncated((VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH - 1) - VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_X,
                                     VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_OFFSET_Y,
                                     *m_appString,
                                     VAPP_LAUNCHER_COSMOS_MM_CELL_TITLE_WIDTH);
            }
        }
        
    #ifdef __MAUI_SOFTWARE_LA__
        SLA_CustomLogging("MDT", SA_stop);
    #endif
    }
}


/*****************************************************************************
 * Class VappLauncherCosmosMmCell
 *****************************************************************************/

VFX_IMPLEMENT_CLASS("VappLauncherCosmosMmCell", VappLauncherCosmosMmCell, VcpPageMenuCell);

VappLauncherCosmosMmCell::VappLauncherCosmosMmCell(const VfxImageSrc & image, const VfxWString & title):
    m_cellImage(image),
    m_cellTitle(title),
    m_cellCounter(-1),
    m_firstDisplay(VFX_TRUE),
    m_onShortcut(VFX_FALSE),
    m_editMode(VFX_FALSE),
    m_isPreinstall(VFX_FALSE),
    m_isAppCounterHidden(VFX_FALSE),
    m_isTextFrameHidden(VFX_FALSE),
    m_iconBgFrameEdit(NULL),
    m_stableFrameContainer(NULL),
    m_appCounterBgFrame(NULL),
    #ifdef __MMI_DSM_NEW__//huangsunbo 20120723
    m_stickyButton(NULL),
    #endif
    m_appCounterTextFrame(NULL),
    m_delayCreateCounterTimer(NULL)
{
    memset(m_appName, 0, MMI_APP_NAME_MAX_LEN);
}

void VappLauncherCosmosMmCell::setCellAppName(mmi_app_package_name_struct appName)
{
    strncpy(m_appName, appName, MMI_APP_NAME_MAX_LEN - 1);
}

void VappLauncherCosmosMmCell::getCellAppName(mmi_app_package_name_struct & result)
{
    strncpy(result, m_appName, MMI_APP_NAME_MAX_LEN - 1);
}

void VappLauncherCosmosMmCell::setOnShortcut(VfxBool onShortcut)
{
    if (m_onShortcut != onShortcut)
    {
        m_onShortcut = onShortcut;

        if (m_stableFrameContainer == NULL)
        {
            return;
        }

        m_stableFrameContainer->updateOnShortcut(m_onShortcut);
    }
}

void VappLauncherCosmosMmCell::setEditMode(VfxBool editMode)
{
    if (m_editMode != editMode)
    {
        m_editMode = editMode;

        if (m_stableFrameContainer == NULL)
        {
            return;
        }

        updateEditMode(m_editMode);
    }
    
    setAppCounterHidden(editMode);
}

void VappLauncherCosmosMmCell::setIsPreinstall(VfxBool isPreinstall)
{
    if (m_isPreinstall != isPreinstall)
    {
        m_isPreinstall = isPreinstall;

        if (m_stableFrameContainer == NULL)
        {
            return;
        }

        m_stableFrameContainer->updateIsPreinstall(m_isPreinstall);
    }
}

void VappLauncherCosmosMmCell::setAppCounter(VfxS32 appCounter)
{
    if (m_cellCounter != appCounter)
    {
        m_cellCounter = appCounter;
        
        if (m_stableFrameContainer == NULL)
        {
            return;
        }
        
        updateAppCounter(m_cellCounter);
    }
}

void VappLauncherCosmosMmCell::setAppCounterHidden(VfxBool isHidden)
{
    if (m_isAppCounterHidden != isHidden)
    {
        m_isAppCounterHidden = isHidden;

        updateAppCounter(m_cellCounter);
        
        if (m_appCounterBgFrame)
        {
            m_appCounterBgFrame->setHidden(isHidden);
        }
        if (m_appCounterTextFrame)
        {
            m_appCounterTextFrame->setHidden(isHidden);
        }
    }
}

void VappLauncherCosmosMmCell::setTextFrameHidden(VfxBool isHidden)
{
    if (m_isTextFrameHidden != isHidden)
    {
        m_isTextFrameHidden = isHidden;

        if (m_stableFrameContainer == NULL)
        {
            return;
        }
        
        m_stableFrameContainer->updateOnShortcut(m_onShortcut);
        m_stableFrameContainer->updateTextFrameHidden(m_isTextFrameHidden);
    }
}

void VappLauncherCosmosMmCell::updateCacheMode()
{
    if (m_stableFrameContainer == NULL)
    {
        return;
    }

    if (isEditModeTransitioning())
    {
        setCacheMode(VFX_CACHE_MODE_AUTO);
        m_stableFrameContainer->setCacheMode(VFX_CACHE_MODE_FORCE);
    }
    else
    {
        setCacheMode(VFX_CACHE_MODE_FORCE);
        m_stableFrameContainer->setCacheMode(VFX_CACHE_MODE_AUTO);
    }

    if (m_iconBgFrameEdit->forceGetOpacity() == 1.0f)
    {
        m_iconBgFrameEdit->setCacheMode(VFX_CACHE_MODE_FREEZE);
    }
    else
    {
        m_iconBgFrameEdit->setCacheMode(VFX_CACHE_MODE_AUTO);
    }
}

void VappLauncherCosmosMmCell::startDelayCreateCounter()
{
    VFX_OBJ_CREATE(m_delayCreateCounterTimer, VfxTimer, this);
    m_delayCreateCounterTimer->m_signalTick.connect(this, &VappLauncherCosmosMmCell::onDelayCreateCounterTimerTick);
    m_delayCreateCounterTimer->setStartDelay(VAPP_LAUNCHER_COSMOS_MM_CELL_DELAY_CREATE_COUNTER_TIME);
    m_delayCreateCounterTimer->start();
}

void VappLauncherCosmosMmCell::onCellSerialize()
{
    m_firstDisplay = VFX_TRUE;

    VFX_OBJ_CLOSE(m_iconBgFrameEdit);
    VFX_OBJ_CLOSE(m_stableFrameContainer);
}

void VappLauncherCosmosMmCell::onCellDisplay()
{
    if (!m_firstDisplay)
    {
        return;
    }

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM4", SA_start);
#endif

    m_firstDisplay = VFX_FALSE;

    setHidden(VFX_FALSE);
    setOpaqueMode(VFX_FRAME_OPAQUE_MODE_TRANSLUCENT);

    if (!m_onShortcut)
    {
        // add icon bg shadow frame -- at the bottom most
        VFX_OBJ_CREATE(m_iconBgFrameEdit, VfxImageFrame, this);
        m_iconBgFrameEdit->setImgContent(VfxImageSrc(IMG_ID_VAPP_LAUNCHER_COSMOS_MM_ICON_SHADOW));
        m_iconBgFrameEdit->setPos(VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_X, VAPP_LAUNCHER_COSMOS_MM_CELL_ICON_SHADOW_POS_Y);
        m_iconBgFrameEdit->setOpacity(0.0f);
        m_iconBgFrameEdit->setAutoAnimate(VFX_TRUE);
        m_iconBgFrameEdit->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_TRANSLUCENT);
    }

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_start);
#endif
    
    // create a stable items container
    VFX_OBJ_CREATE_EX(m_stableFrameContainer, VappLauncherCosmosMmCellStableFrame, this, (&m_appName, &m_cellImage, &m_cellTitle));
    m_stableFrameContainer->setSize(VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH, VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_HEIGHT);
    m_stableFrameContainer->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_TRANSLUCENT);

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_stop);
#endif

    m_stableFrameContainer->updateOnShortcut(m_onShortcut);

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_start);
#endif

    m_stableFrameContainer->updateIsPreinstall(m_isPreinstall);

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_stop);
#endif

    m_stableFrameContainer->updateTextFrameHidden(m_isTextFrameHidden);

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_start);
#endif

#if defined(__MMI_DSM_NEW__) && defined(__MR_CFG_FEATURE_DAM__)
	if( NULL == m_stickyButton )//huangsunbo 20120723
	{
		VFX_OBJ_CREATE(m_stickyButton, VcpImageButton, this);
	}
	m_stickyButton->setImage(VcpStateImage(IMG_ID_VAPP_WIDGET_BUTTON_DELETE, IMG_ID_VAPP_WIDGET_BUTTON_DELETE, IMG_ID_VAPP_WIDGET_BUTTON_DELETE, IMG_ID_VAPP_WIDGET_BUTTON_DELETE));
	m_stickyButton->setPos(getRect().getWidth() / 2, 0);
	m_stickyButton->setSize(getRect().getWidth() / 2, getRect().getHeight() / 2);	
	m_stickyButton->setHidden(VFX_TRUE);
#endif	

    updateEditMode(m_editMode);

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_stop);
#endif
    
    updateAppCounter(m_cellCounter);

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM4", SA_stop);
#endif
}

void VappLauncherCosmosMmCell::onInit()
{
    VcpPageMenuCell::onInit();

    // init size
    setSize(VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_WIDTH, VAPP_LAUNCHER_COSMOS_MM_CELL_VISIBLE_HEIGHT);

    // default hidden on creation
    setHidden(VFX_TRUE);
}

void VappLauncherCosmosMmCell::onDeinit()
{
    if (strncmp(m_appName, "native.mtk.msg", MMI_APP_NAME_MAX_LEN) == 0)
    {
        mmi_frm_cb_dereg_event(EVT_ID_SRV_UM_NOTIFY_REFRESH, &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_dereg_event(EVT_ID_SRV_UM_NOTIFY_READY, &VappLauncherCosmosMmCell::eventHandler, this);
    }
    else if (strncmp(m_appName, "native.mtk.calllog", MMI_APP_NAME_MAX_LEN) == 0)
    {
        mmi_frm_cb_dereg_event(EVT_ID_SRV_CLOG_READY, &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_dereg_event(EVT_ID_SRV_CLOG_ADD_LOG, &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_dereg_event(EVT_ID_SRV_CLOG_UPDATE_LOG, &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_dereg_event(EVT_ID_SRV_CLOG_UPDATE_ALL, &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_dereg_event(EVT_ID_SRV_CLOG_DEL_LOG, &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_dereg_event(EVT_ID_SRV_CLOG_DEL_ALL, &VappLauncherCosmosMmCell::eventHandler, this);
    }
#ifdef __MMI_EMAIL__
    else if (strncmp(m_appName, "native.mtk.vapp_email", MMI_APP_NAME_MAX_LEN) == 0)
    {
        mmi_frm_cb_dereg_event(EVT_ID_VAPP_LAUNCHER_COSMOS_MM_EMAIL_UNREAD_NOTIFY, &VappLauncherCosmosMmCell::eventHandler, this);
    }
#endif // __MMI_EMAIL__
    
    VcpPageMenuCell::onDeinit();
}

void VappLauncherCosmosMmCell::updateEditMode(VfxBool editMode)
{
    if (m_iconBgFrameEdit == NULL)
    {
        return;
    }
    
    VfxAutoAnimate::begin();
    VfxAutoAnimate::setDuration(250);
    if (editMode)
    {
        m_iconBgFrameEdit->setOpacity(1.0f);
    }
    else
    {
        m_iconBgFrameEdit->setOpacity(0.0f);
    }

    if (strncmp(m_appName, "native.mtk.launcher", MMI_APP_NAME_MAX_LEN) == 0)
    {
        if (editMode)
        {
            m_stableFrameContainer->setOpacity(0.3f);
        }
        else
        {
            m_stableFrameContainer->setOpacity(1.0f);
        }
    }
    for (VfxS32 i = 0; i < VAPP_LAUNCHER_COSMOS_MM_ORGANIZE_UNCHANGABLE_PAGE_ITEM_COUNT; ++i)
    {
        if (strncmp(m_appName, vappLauncherCosmosMmDefaultOrder[VAPP_LAUNCHER_COSMOS_MM_BAR_CELL_COUNT+i], MMI_APP_NAME_MAX_LEN) == 0)
        {
            if (editMode)
            {
                m_stableFrameContainer->setOpacity(0.3f);
            }
            else
            {
                m_stableFrameContainer->setOpacity(1.0f);
            }
            break;
        }
    }
    VfxAutoAnimate::commit();
	
#if defined(__MMI_DSM_NEW__) && defined(__MR_CFG_FEATURE_DAM__)
	if (editMode)
	{
		if (strncmp(m_appName, "mrp.dyn.", 8) == 0)
		{
			m_stickyButton->setHidden(VFX_FALSE);
		}
	}
	else
	{
		m_stickyButton->setHidden(VFX_TRUE);	
	}
#endif	
}

void VappLauncherCosmosMmCell::updateAppCounter(VfxS32 appCounter)
{
    if (m_isAppCounterHidden)
    {
        return;
    }
    
    VfxBool isCacheModeFreeze = VFX_FALSE;
    if (getCacheMode() == VFX_CACHE_MODE_FREEZE)
    {
        isCacheModeFreeze = VFX_TRUE;
        setCacheMode(VFX_CACHE_MODE_AUTO);
    }
    
    if (appCounter > 0)
    {
        if (m_appCounterBgFrame == NULL)
        {
            VFX_OBJ_CREATE(m_appCounterBgFrame, VfxImageFrame, this);
        }
        m_appCounterBgFrame->setHidden(m_isAppCounterHidden);
        m_appCounterBgFrame->setImgContent(VfxImageSrc(IMG_ID_VAPP_LAUNCHER_COSMOS_MM_APP_COUNTER_BG));
        m_appCounterBgFrame->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE);
        m_appCounterBgFrame->bringToFront();
        if (appCounter >= 100)
        {
            m_appCounterBgFrame->setSize(VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_3DIGITS_WIDTH, m_appCounterBgFrame->getSize().height);
        }
        m_appCounterBgFrame->setPos(getSize().width - m_appCounterBgFrame->getSize().width, VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_OFFSET_Y);

        if (m_appCounterTextFrame == NULL)
        {
            VFX_OBJ_CREATE(m_appCounterTextFrame, VfxTextFrame, m_appCounterBgFrame);
        }
        m_appCounterTextFrame->setHidden(m_isAppCounterHidden);
        VfxFontDesc fontDesc;
        fontDesc.size = VFX_FONT_DESC_VF_SIZE(VAPP_LAUNCHER_COSMOS_MM_CELL_COUNTER_FONT_SIZE);
        m_appCounterTextFrame->setFont(fontDesc);
        m_appCounterTextFrame->setColor(VFX_COLOR_WHITE);
        m_appCounterTextFrame->setPos(m_appCounterBgFrame->getSize().width / 2, m_appCounterBgFrame->getSize().height/ 2);
        m_appCounterTextFrame->setAnchor(0.55f, 0.55f);
        m_appCounterTextFrame->setAutoResized(VFX_TRUE);
        m_appCounterTextFrame->bringToFront();
        if (appCounter < 1000)
        {
            VfxWString count;
            m_appCounterTextFrame->setString(count.format("%d", appCounter));
        }
        else
        {
            m_appCounterTextFrame->setString(VFX_WSTR("999"));
        }
    }
    else
    {
        if (m_appCounterTextFrame != NULL)
        {
            VFX_OBJ_CLOSE(m_appCounterTextFrame);
        }
        if (m_appCounterBgFrame != NULL)
        {
            VFX_OBJ_CLOSE(m_appCounterBgFrame);
        }
    }

    if (isCacheModeFreeze)
    {
        setCacheMode(VFX_CACHE_MODE_FREEZE);
    }
}

VfxBool VappLauncherCosmosMmCell::isEditModeTransitioning()
{
    if (NULL == m_iconBgFrameEdit)
    {
        return VFX_FALSE;
    }
    const VfxFloat currentEditOpacity = m_iconBgFrameEdit->forceGetOpacity();
    const VfxFloat targetEditOpacity = m_iconBgFrameEdit->getOpacity();
    return (currentEditOpacity != targetEditOpacity);
}

void VappLauncherCosmosMmCell::onDelayCreateCounterTimerTick(VfxTimer *source)
{
    // TODO: Get APP counter info from APP Manager in the future
    if (strncmp(m_appName, "native.mtk.msg", MMI_APP_NAME_MAX_LEN) == 0)
    {
        srv_um_get_msg_num_result data;
        if (srv_um_check_msg_num(SRV_UM_SIM_ALL, SRV_UM_MSG_ALL, &data) == SRV_UM_RESULT_OK)
        {
            setAppCounter((VfxS32)(data.inbox_unread_msg_number));
        }
        
        mmi_frm_cb_reg_event(EVT_ID_SRV_UM_NOTIFY_REFRESH,  &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_reg_event(EVT_ID_SRV_UM_NOTIFY_READY, &VappLauncherCosmosMmCell::eventHandler, this);
    }
    else if (strncmp(m_appName, "native.mtk.calllog", MMI_APP_NAME_MAX_LEN) == 0)
    {
        setAppCounter((VfxS32)srv_clog_get_unread_missed_call_num());
        
        mmi_frm_cb_reg_event(EVT_ID_SRV_CLOG_READY,  &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_reg_event(EVT_ID_SRV_CLOG_ADD_LOG,  &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_reg_event(EVT_ID_SRV_CLOG_UPDATE_LOG,  &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_reg_event(EVT_ID_SRV_CLOG_UPDATE_ALL,  &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_reg_event(EVT_ID_SRV_CLOG_DEL_LOG,  &VappLauncherCosmosMmCell::eventHandler, this);
        mmi_frm_cb_reg_event(EVT_ID_SRV_CLOG_DEL_ALL,  &VappLauncherCosmosMmCell::eventHandler, this);
    }
#ifdef __MMI_EMAIL__
    else if (strncmp(m_appName, "native.mtk.vapp_email", MMI_APP_NAME_MAX_LEN) == 0)
    {
        VfxU32 unreadNum;
        if (srv_email_acct_total_unread_num_get(&unreadNum) == SRV_EMAIL_RESULT_SUCC)
        {
            setAppCounter((VfxS32)unreadNum);
            srv_email_acct_cache_notify_set(srvEmailAllMsgUnreadNotifyCallback, NULL);
        }
        else
        {
            srv_email_app_init_set_callback(srvEmailAppInitStateCallback, this);
        }
        
        mmi_frm_cb_reg_event(EVT_ID_VAPP_LAUNCHER_COSMOS_MM_EMAIL_UNREAD_NOTIFY,  &VappLauncherCosmosMmCell::eventHandler, this);
    }
#endif // __MMI_EMAIL__
}

mmi_ret VappLauncherCosmosMmCell::eventHandler(mmi_event_struct *event)
{
    VappLauncherCosmosMmCell *cell = (VappLauncherCosmosMmCell *)event->user_data;

    switch (event->evt_id)
    {
        // Update message counter
        case EVT_ID_SRV_UM_NOTIFY_READY:
        case EVT_ID_SRV_UM_NOTIFY_REFRESH:
            srv_um_get_msg_num_result data;
            if (srv_um_check_msg_num(SRV_UM_SIM_ALL, SRV_UM_MSG_ALL, &data) == SRV_UM_RESULT_OK)
            {
                cell->setAppCounter((VfxS32)(data.inbox_unread_msg_number));
            }
            else
            {
                cell->setAppCounter(0);
            }
            break;

        // Update call log counter
        case EVT_ID_SRV_CLOG_READY:
        case EVT_ID_SRV_CLOG_ADD_LOG:
        case EVT_ID_SRV_CLOG_UPDATE_LOG:
        case EVT_ID_SRV_CLOG_UPDATE_ALL:
        case EVT_ID_SRV_CLOG_DEL_LOG:
        case EVT_ID_SRV_CLOG_DEL_ALL:
            cell->setAppCounter((VfxS32)srv_clog_get_unread_missed_call_num());
            break;

    #ifdef __MMI_EMAIL__
        // Update email counter
        case EVT_ID_VAPP_LAUNCHER_COSMOS_MM_EMAIL_UNREAD_NOTIFY:
            VfxU32 unreadNum;
            srv_email_acct_total_unread_num_get(&unreadNum);
            cell->setAppCounter((VfxS32)unreadNum);
            break;
    #endif // __MMI_EMAIL__
            
        default:
            break;
    }
    return MMI_RET_OK;
}


/*****************************************************************************
 * Class VappLauncherCosmosMmBuilder
 *****************************************************************************/

VFX_IMPLEMENT_CLASS("VappLauncherCosmosMmBuilder", VappLauncherCosmosMmBuilder, VfxObject);

VappLauncherCosmosMmBuilder::VappLauncherCosmosMmBuilder():
    m_orderData(NULL),
    m_orderDataCount(0),
    m_groupData(NULL),
    m_groupDataCount(0)
{
}

void VappLauncherCosmosMmBuilder::setAppOrderData(mmi_app_package_name_struct orderData[], VfxS32 count)
{
    m_orderData = orderData;
    m_orderDataCount = count;
}

void VappLauncherCosmosMmBuilder::setAppGroupData(mmi_app_package_name_struct groupData[], VfxS32 count)
{
    m_groupData = groupData;
    m_groupDataCount = count;
}

void VappLauncherCosmosMmBuilder::loadMenu(VappLauncherCosmosMmPageMenu * menu)
{
    initFrames(menu);
}

void VappLauncherCosmosMmBuilder::saveMenu(VappLauncherCosmosMmPageMenu * menu)
{
    VfxS32 orderIndex = 0;
    for (VfxS32 i = 0; i < menu->getPageCount(); ++i)
    {
        for (VfxS32 j = 0; j < menu->getMenuItemCountPerPage(); ++j)
        {
            VcpPageMenuCell * cell = menu->getMenuItem(i * menu->getMenuItemCountPerPage() + j);
            if (cell == NULL)
            {
                break;
            }
            VappLauncherCosmosMmCell * cosmosCell = (VappLauncherCosmosMmCell *)cell;
            cosmosCell->getCellAppName(m_orderData[orderIndex]);
            ++orderIndex;
        }
        strncpy(m_orderData[orderIndex], "", MMI_APP_NAME_MAX_LEN);
        ++orderIndex;
    }

    // fill the remaining order data with empty string
    for (VfxS32 i = orderIndex; i < m_orderDataCount; ++i)
    {
        strncpy(m_orderData[orderIndex], "", MMI_APP_NAME_MAX_LEN);
    }

    // save group data
    VfxS32 groupOrderIndex = 0;
    for (VfxS32 j = 0; j < 4 && groupOrderIndex < m_groupDataCount; ++j)
    {
        VcpPageMenuCell * cell = menu->getGroupMenuItem(j);
        if (cell == NULL)
        {
            strncpy(m_groupData[groupOrderIndex], "", MMI_APP_NAME_MAX_LEN);
            ++groupOrderIndex;
            continue;
        }
        VappLauncherCosmosMmCell * cosmosCell = (VappLauncherCosmosMmCell *)cell;
        cosmosCell->getCellAppName(m_groupData[groupOrderIndex]);
        ++groupOrderIndex;
    }
    
    vappLauncherCosmosMmClearStringWidth();
}

void VappLauncherCosmosMmBuilder::initFrames(VappLauncherCosmosMmPageMenu * menu)
{
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM4", SA_start);
#endif

    mmi_app_package_name_struct *preInstalledAppArray = NULL;
    U32 preInstalledAppCount = srv_appmgr_get_app_package_num(SRV_APPMGR_PRE_INSTALLED_APP_PACKAGE_FLAG_TYPE);
    if (preInstalledAppCount)
    {
        VFX_SYS_ALLOC_MEM(preInstalledAppArray, sizeof(mmi_app_package_name_struct) * preInstalledAppCount );
        mmi_ret result;
        result = srv_appmgr_get_app_package_list(SRV_APPMGR_PRE_INSTALLED_APP_PACKAGE_FLAG_TYPE, preInstalledAppArray, preInstalledAppCount);
        VFX_ASSERT(result != MMI_RET_ERR);
    }

    VfxS32 cellIndex = 0;
    VfxS32 pageCount = 0;
        
#ifdef VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST
    for (VfxS32 j = 0; j < VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST_PAGE_COUNT; ++j)
    for (VfxS32 i = 0; i < VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST_APP_COUNT_PER_PAGE; ++i)
#else
    for (VfxS32 i = 0; i < m_orderDataCount; ++i)
#endif
    {
        if (strncmp(m_orderData[i], "DELETED", MMI_APP_NAME_MAX_LEN) == 0)
        {
            continue;
        }
        if (strncmp(m_orderData[i], "", MMI_APP_NAME_MAX_LEN) == 0)
        {
            if (i > 0 && strncmp(m_orderData[i - 1], "", MMI_APP_NAME_MAX_LEN) == 0)
            {
                break;
            }
            cellIndex = (cellIndex + menu->getMenuItemCountPerPage() - 1) / menu->getMenuItemCountPerPage() * menu->getMenuItemCountPerPage();
            continue;
        }

        VfxBool isPreInstalled = VFX_FALSE;
        if (preInstalledAppArray)
        {
            isPreInstalled = vappLauncherCosmosMmIsPreInstalledApp(m_orderData[i], preInstalledAppArray, preInstalledAppCount);
        }
        VappLauncherCosmosMmCell * cell = ::vappLauncherCosmosMmCreateCell(m_orderData[i], menu, isPreInstalled);
        cell->setIsUnhittable(VFX_TRUE);

        // reposition if necessary
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_start);
#endif
        cellIndex = repositionFrameIndex(menu, cellIndex);
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM5", SA_stop);
#endif
        menu->setMenuItem(cellIndex, cell);

        ++cellIndex;
        pageCount = VFX_MAX( (cellIndex + menu->getMenuItemCountPerPage() - 1) / menu->getMenuItemCountPerPage(), pageCount);
    }

#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM4", SA_stop);
    SLA_CustomLogging("MM4", SA_start);
#endif
    
#ifdef VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST
    menu->setPageCount(VAPP_LAUNCHER_COSMOS_MM_MEMORY_TEST_PAGE_COUNT);
#else
    menu->setPageCount(pageCount);
#endif

    VfxS32 groupCellIndex = 0;
    for (VfxS32 i = 0; i < m_groupDataCount; ++i)
    {
        if ( (strncmp(m_groupData[i], "", MMI_APP_NAME_MAX_LEN) == 0) ||
            (strncmp(m_groupData[i], "DELETED", MMI_APP_NAME_MAX_LEN) == 0) )
        {
            ++groupCellIndex;
            continue;
        }

        VfxBool isPreInstalled = VFX_FALSE;
        if (preInstalledAppArray)
        {
            isPreInstalled = vappLauncherCosmosMmIsPreInstalledApp(m_orderData[i], preInstalledAppArray, preInstalledAppCount);
        }
        VappLauncherCosmosMmCell * cell = ::vappLauncherCosmosMmCreateCell(m_groupData[i], menu, isPreInstalled);
        cell->setIsUnhittable(VFX_TRUE);
        menu->setGroupMenuItem(groupCellIndex, cell);

        ++groupCellIndex;
    }
    
    if (preInstalledAppArray)
    {
        VFX_SYS_FREE_MEM(preInstalledAppArray);
    }
    
#ifdef __MAUI_SOFTWARE_LA__
    SLA_CustomLogging("MM4", SA_stop);
#endif
}

VfxS32 VappLauncherCosmosMmBuilder::repositionFrameIndex(VappLauncherCosmosMmPageMenu * menu, VfxS32 index)
{
    if (index < menu->getMaxPageCount() * menu->getMenuItemCountPerPage() )
    {
        return index;
    }

    for (VfxS32 pageIndex = menu->getMaxPageCount() - 1; pageIndex >= 0; --pageIndex)
    {
        for (VfxS32 i = 0; i < menu->getMenuItemCountPerPage(); ++i)
        {
            const VfxS32 index = pageIndex * menu->getMenuItemCountPerPage() + i;
            {
                if (menu->getMenuItem(index) == NULL)
                {
                    return index;
                }
            }
        }
    }
    VFX_ASSERT(VFX_FALSE);
    return index;
}


#endif // __MMI_VUI_LAUNCHER_COSMOS__

