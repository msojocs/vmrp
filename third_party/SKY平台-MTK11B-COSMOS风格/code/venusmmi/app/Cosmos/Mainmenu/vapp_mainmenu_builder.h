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
 *  vapp_mainmenu_builder.h
 *
 * Project:
 * --------
 *  Cosmos
 *
 * Description:
 * ------------
 *  Cosmos Mainmenu
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/***************************************************************************** 
 * Include
 *****************************************************************************/

#ifndef __VAPP_MAINMENU_BUILDER_H__
#define __VAPP_MAINMENU_BUILDER_H__

#include "MMIDatatype.h"
#include "vcp_page_menu_util.h"
#include "vfx_text_frame.h"
#ifdef __MMI_DSM_NEW__
#include "mmi_rp_vapp_widget_def.h"
#include "vapp_widget_primitive.h"
#include "mrp_include.h"
#endif

struct AppEntry
{
    VfxImageSrc appIcon;
    VfxWString appTitle;
    VfxS32 entry;
};

class VcpPageMenuCell;

class VappMainmenuPageMenu;

#define VAPP_COSMOS_MAINMENU_TEST_PAGE_BREAK 0xFFFFFFFF

class VappMainmenuCell : public VcpPageMenuCell
{
public:
#ifdef __MMI_DSM_NEW__
	VcpImageButton* m_stickyButton;
#endif    

    VappMainmenuCell(const VfxImageSrc & image, const VfxWString & title);

    void setCellAppName(mmi_app_package_name_struct appName);
    void getCellAppName(mmi_app_package_name_struct & result);

    void setOnBottomBar(VfxBool onBottomBar);
    void setIsPreinstall(VfxBool isPreinstall);
    void setEditMode(VfxBool editMode);

    virtual void onCellDisplay();

protected:
    virtual void onInit();

private:
    void updateIconImage(const VfxImageSrc & image);
    void updateTitle(const VfxWString & title);
    void updateOnBottomBar(VfxBool onBottomBar);
    void updateEditMode(VfxBool editMode);
    void updateIsPreinstall(VfxBool isPreinstall);

    VfxImageSrc                 m_cellImage;
    VfxWString                  m_cellTitle;

    mmi_app_package_name_struct m_appName;
    VfxBool                     m_firstDisplay;
    VfxBool                     m_onBottomBar;
    VfxBool                     m_editMode;
    VfxBool                     m_isPreinstall;
    VfxFrame *                  m_iconBgFrame;
    VfxFrame *                  m_iconBgFrameEdit;
    VfxFrame *                  m_iconFrame;
    VfxFrame *                  m_smallIconFrame;
    VfxTextFrame *              m_textFrame;
    VfxFrame *                  m_textBgFrame;
    VfxFrame *                  m_preinstallFrame;
};

class VappMainmenuBuilder : public VfxObject
{
public:
    VappMainmenuBuilder();

    void setAppOrderData(mmi_app_package_name_struct orderData[], VfxS32 count);
    void setAppGroupData(mmi_app_package_name_struct groupData[], VfxS32 count);

    void loadMenu(VappMainmenuPageMenu * menu);
    void saveMenu(VappMainmenuPageMenu * menu);

    void checkCellState(VappMainmenuPageMenu * menu);

private:
    mmi_app_package_name_struct *   m_orderData;
    VfxS32                          m_orderDataCount;
    mmi_app_package_name_struct *   m_groupData;
    VfxS32                          m_groupDataCount;


    void initFrames(VappMainmenuPageMenu * menu);
    VfxS32 repositionFrameIndex(VappMainmenuPageMenu * menu, VfxS32 index);
    void reorderFrames(VappMainmenuPageMenu * menu);
};

VappMainmenuCell * createMainmenuCell(mmi_app_package_name_struct & appName, VfxFrame * parent, VfxBool preinstall);

VappMainmenuCell * createMainmenuCellRaw(VfxImageSrc image, const VfxWString &title, VfxFrame * parent);

VfxBool isPreinstallApp(mmi_app_package_name_struct appName, mmi_app_package_name_struct preinstall[], VfxU32 preCount);

#endif // __VAPP_MAINMENU_BUILDER_H__

