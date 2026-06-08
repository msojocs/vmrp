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
 *  vapp_mainmenu_builder.cpp
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
 * removed!
 *
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
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "vcp_include.h"
#include "vapp_mainmenu_builder.h"
#include "vapp_mainmenu_page_menu.h"
#include "vcp_page_menu_util.h"
#include "mmi_rp_vapp_cosmos_mainmenu_def.h"

extern "C" {
#include "AppMgrSrvGprot.h"
}

// cell content layout
#if defined(__MMI_MAINLCD_480X800__)
static const VfxS32 MAINMENU_CELL_VISIBLE_WIDTH = 120;
static const VfxS32 MAINMENU_CELL_VISIBLE_HEIGHT = 122;

static const VfxS32 MAINMENU_CELL_DEFAULT_ICON_PICTURE_WIDTH = 72;
static const VfxS32 MAINMENU_CELL_DEFAULT_ICON_PICTURE_HEIGHT = 72;

static const VfxS32 MAINMENU_CELL_ICON_WIDTH = 78;
static const VfxS32 MAINMENU_CELL_ICON_HEIGHT = 78;
static const VfxS32 MAINMENU_CELL_ICON_OFFSET_X = 21;
static const VfxS32 MAINMENU_CELL_ICON_OFFSET_Y = 8;

static const VfxS32 MAINMENU_CELL_SMALL_ICON_WIDTH = 26;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_HEIGHT = 26;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_OFFSET_X = 39 + 3;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_OFFSET_Y = 40 + 3;

static const VfxS32 MAINMENU_CELL_TITLE_BASE_X_MARGIN = 2;

static const VfxS32 MAINMENU_CELL_TITLE_BG_OFFSET_X = MAINMENU_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_BG_OFFSET_Y = 88;
static const VfxS32 MAINMENU_CELL_TITLE_BG_WIDTH = MAINMENU_CELL_VISIBLE_WIDTH - 2 * MAINMENU_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_BG_HEIGHT = 23 + 3 * 2;

static const VfxS32 MAINMENU_CELL_TITLE_BG_X_MARGIN = 8;

static const VfxS32 MAINMENU_CELL_TITLE_OFFSET_X = MAINMENU_CELL_TITLE_BASE_X_MARGIN + MAINMENU_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_OFFSET_Y = 93;
static const VfxS32 MAINMENU_CELL_TITLE_WIDTH = MAINMENU_CELL_VISIBLE_WIDTH - 2 * MAINMENU_CELL_TITLE_BASE_X_MARGIN - 2 * MAINMENU_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_HEIGHT = 23;

static const VfxS32 MAINMENU_CELL_TITLE_FONT_SIZE = 19; // font size 21 = real size 23

#elif defined(__MMI_MAINLCD_240X320__) || defined(__MMI_MAINLCD_240X400__)
static const VfxS32 MAINMENU_CELL_VISIBLE_WIDTH = 60;
static const VfxS32 MAINMENU_CELL_VISIBLE_HEIGHT = 73;

static const VfxS32 MAINMENU_CELL_DEFAULT_ICON_PICTURE_WIDTH = 38;
static const VfxS32 MAINMENU_CELL_DEFAULT_ICON_PICTURE_HEIGHT = 38;

static const VfxS32 MAINMENU_CELL_ICON_WIDTH = 43;
static const VfxS32 MAINMENU_CELL_ICON_HEIGHT = 43;
static const VfxS32 MAINMENU_CELL_ICON_OFFSET_X = 8; // (60 - 43) / 2
static const VfxS32 MAINMENU_CELL_ICON_OFFSET_Y = 8;

static const VfxS32 MAINMENU_CELL_SMALL_ICON_WIDTH = 12;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_HEIGHT = 12;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_OFFSET_X = 22 + 2;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_OFFSET_Y = 22 + 2;

static const VfxS32 MAINMENU_CELL_TITLE_BASE_X_MARGIN = 0;

static const VfxS32 MAINMENU_CELL_TITLE_BG_OFFSET_X = MAINMENU_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_BG_OFFSET_Y = 52;
static const VfxS32 MAINMENU_CELL_TITLE_BG_WIDTH = MAINMENU_CELL_VISIBLE_WIDTH - 2 * MAINMENU_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_BG_HEIGHT = 14;

static const VfxS32 MAINMENU_CELL_TITLE_BG_X_MARGIN = 3;

static const VfxS32 MAINMENU_CELL_TITLE_OFFSET_X = MAINMENU_CELL_TITLE_BASE_X_MARGIN + MAINMENU_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_OFFSET_Y = 53;
static const VfxS32 MAINMENU_CELL_TITLE_WIDTH = MAINMENU_CELL_VISIBLE_WIDTH - 2 * MAINMENU_CELL_TITLE_BASE_X_MARGIN - 2 * MAINMENU_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_HEIGHT = 12;

static const VfxS32 MAINMENU_CELL_TITLE_FONT_SIZE = 11; // font size 11 = real size 12

#else // default HVGA 320X480
static const VfxS32 MAINMENU_CELL_VISIBLE_WIDTH = 80;
static const VfxS32 MAINMENU_CELL_VISIBLE_HEIGHT = 90;

static const VfxS32 MAINMENU_CELL_DEFAULT_ICON_PICTURE_WIDTH = 48;
static const VfxS32 MAINMENU_CELL_DEFAULT_ICON_PICTURE_HEIGHT = 48;

static const VfxS32 MAINMENU_CELL_ICON_WIDTH = 54;
static const VfxS32 MAINMENU_CELL_ICON_HEIGHT = 54;
static const VfxS32 MAINMENU_CELL_ICON_OFFSET_X = 13;
static const VfxS32 MAINMENU_CELL_ICON_OFFSET_Y = 10;

static const VfxS32 MAINMENU_CELL_SMALL_ICON_WIDTH = 18;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_HEIGHT = 18;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_OFFSET_X = 26 + 1;
static const VfxS32 MAINMENU_CELL_SMALL_ICON_OFFSET_Y = 26 + 1;

static const VfxS32 MAINMENU_CELL_TITLE_BASE_X_MARGIN = 1;

static const VfxS32 MAINMENU_CELL_TITLE_BG_OFFSET_X = MAINMENU_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_BG_OFFSET_Y = 65;
static const VfxS32 MAINMENU_CELL_TITLE_BG_WIDTH = MAINMENU_CELL_VISIBLE_WIDTH - 2 * MAINMENU_CELL_TITLE_BASE_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_BG_HEIGHT = 19;

static const VfxS32 MAINMENU_CELL_TITLE_BG_X_MARGIN = 5;

static const VfxS32 MAINMENU_CELL_TITLE_OFFSET_X = MAINMENU_CELL_TITLE_BASE_X_MARGIN + MAINMENU_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_OFFSET_Y = 67;
static const VfxS32 MAINMENU_CELL_TITLE_WIDTH = MAINMENU_CELL_VISIBLE_WIDTH - 2 * MAINMENU_CELL_TITLE_BASE_X_MARGIN - 2 * MAINMENU_CELL_TITLE_BG_X_MARGIN;
static const VfxS32 MAINMENU_CELL_TITLE_HEIGHT = 15;

static const VfxS32 MAINMENU_CELL_TITLE_FONT_SIZE = 13; // font size 13 = real size 15

#endif

/*****************************************************************************
 * Class VappMainmenuCell
 *****************************************************************************/

VappMainmenuCell::VappMainmenuCell(const VfxImageSrc & image, const VfxWString & title):
    m_cellImage(image),
    m_cellTitle(title),
    m_firstDisplay(VFX_TRUE),
    m_onBottomBar(VFX_FALSE),
    m_editMode(VFX_FALSE),
    m_isPreinstall(VFX_FALSE),
    m_iconBgFrame(NULL),
    m_iconFrame(NULL),
    m_smallIconFrame(NULL),
    m_textFrame(NULL),
#if defined(__MMI_DSM_NEW__)//by coffee.che 20120516
    m_stickyButton (NULL),
#endif
    m_textBgFrame(NULL),
    m_preinstallFrame(NULL)
{
    memset(m_appName, 0, MMI_APP_NAME_MAX_LEN);
}

void VappMainmenuCell::setCellAppName(mmi_app_package_name_struct appName)
{
    strncpy(m_appName, appName, MMI_APP_NAME_MAX_LEN - 1);
}

void VappMainmenuCell::getCellAppName(mmi_app_package_name_struct & result)
{
    strncpy(result, m_appName, MMI_APP_NAME_MAX_LEN - 1);
}

void VappMainmenuCell::setOnBottomBar(VfxBool onBottomBar)
{
    if (m_onBottomBar != onBottomBar)
    {
        m_onBottomBar = onBottomBar;

        if (m_textFrame == NULL)
        {
            return;
        }

        updateOnBottomBar(m_onBottomBar);
    }
}

void VappMainmenuCell::setEditMode(VfxBool editMode)
{
    if (m_editMode != editMode)
    {
        m_editMode = editMode;

        if (m_iconBgFrame == NULL)
        {
            return;
        }

        updateEditMode(m_editMode);
    }
}

void VappMainmenuCell::setIsPreinstall(VfxBool isPreinstall)
{
    if (m_isPreinstall != isPreinstall)
    {
        m_isPreinstall = isPreinstall;

        if (m_iconBgFrame == NULL)
        {
            return;
        }

        updateIsPreinstall(m_isPreinstall);
    }
}

void VappMainmenuCell::updateIconImage(const VfxImageSrc & image)
{
    if (m_iconFrame == NULL)
    {
        return;
    }

    VfxBool isSmallIcon = VFX_FALSE;
    VfxBool isMRE = VFX_FALSE;
    VfxBool isJava = VFX_FALSE;
    if (strncmp(m_appName, "mre.", 4) == 0)
    {
        isMRE = VFX_TRUE;
    }
    if (strncmp(m_appName, "java.", 5) == 0)
    {
        isJava = VFX_TRUE;
    }

    if (isMRE || isJava)
    {
        if ( (image.getSize().width < MAINMENU_CELL_DEFAULT_ICON_PICTURE_WIDTH / 2) && 
            (image.getSize().height < MAINMENU_CELL_DEFAULT_ICON_PICTURE_HEIGHT / 2) )
        {
            isSmallIcon = VFX_TRUE;
        }
    }

    if (isSmallIcon)
    {
        if (m_smallIconFrame == NULL)
        {
            VFX_OBJ_CREATE(m_smallIconFrame, VfxFrame, this);
        }
        m_smallIconFrame->setPos(m_iconFrame->getPos().x + MAINMENU_CELL_SMALL_ICON_OFFSET_X,
                m_iconFrame->getPos().y + MAINMENU_CELL_SMALL_ICON_OFFSET_Y);
        m_smallIconFrame->setSize(MAINMENU_CELL_SMALL_ICON_WIDTH,
                MAINMENU_CELL_SMALL_ICON_HEIGHT);
        m_smallIconFrame->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE_ASPECT_FILL);
        m_smallIconFrame->setImgContent(image);

        if (isMRE)
        {
            m_iconFrame->setImgContent(VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_ICON_MRE_RESIZE) );
        }
        if (isJava)
        {
            m_iconFrame->setImgContent(VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_ICON_JAVA_RESIZE) );
        }
    }
    else
    {
        m_iconFrame->setImgContent(image);
    }
}

void VappMainmenuCell::updateTitle(const VfxWString & title)
{
    if (m_textFrame == NULL)
    {
        return;
    }
    m_textFrame->setString(title);
    VfxSize size = m_textFrame->getFont().measureStr(title);
    if (size.width <= MAINMENU_CELL_TITLE_WIDTH)
    {
        m_textFrame->setSize(size.width, MAINMENU_CELL_TITLE_HEIGHT);
        m_textFrame->setPos(MAINMENU_CELL_TITLE_OFFSET_X +
                (MAINMENU_CELL_TITLE_WIDTH - size.width) / 2, MAINMENU_CELL_TITLE_OFFSET_Y);
        m_textBgFrame->setSize(size.width + MAINMENU_CELL_TITLE_BG_X_MARGIN * 2,
                MAINMENU_CELL_TITLE_BG_HEIGHT);
        m_textBgFrame->setPos(MAINMENU_CELL_TITLE_BG_OFFSET_X +
                (MAINMENU_CELL_TITLE_WIDTH - size.width) / 2, MAINMENU_CELL_TITLE_BG_OFFSET_Y);
    }
    else if (size.width <= MAINMENU_CELL_TITLE_BG_WIDTH)
    {
        m_textFrame->setSize(size.width, MAINMENU_CELL_TITLE_HEIGHT);
        m_textFrame->setPos(MAINMENU_CELL_TITLE_BG_OFFSET_X +
                (MAINMENU_CELL_TITLE_BG_WIDTH - size.width) / 2, MAINMENU_CELL_TITLE_OFFSET_Y);
        m_textBgFrame->setSize(MAINMENU_CELL_TITLE_BG_WIDTH, MAINMENU_CELL_TITLE_BG_HEIGHT);
        m_textBgFrame->setPos(MAINMENU_CELL_TITLE_BG_OFFSET_X, MAINMENU_CELL_TITLE_BG_OFFSET_Y);
    }
    else
    {
        m_textFrame->setSize(MAINMENU_CELL_TITLE_BG_WIDTH, MAINMENU_CELL_TITLE_HEIGHT);
        m_textFrame->setPos(MAINMENU_CELL_TITLE_BG_OFFSET_X, MAINMENU_CELL_TITLE_OFFSET_Y);
        m_textBgFrame->setSize(MAINMENU_CELL_TITLE_BG_WIDTH, MAINMENU_CELL_TITLE_BG_HEIGHT);
        m_textBgFrame->setPos(MAINMENU_CELL_TITLE_BG_OFFSET_X, MAINMENU_CELL_TITLE_BG_OFFSET_Y);
    }
}

void VappMainmenuCell::updateOnBottomBar(VfxBool onBottomBar)
{
    if (onBottomBar)
    {
        m_textBgFrame->setHidden(VFX_FALSE);
        if (m_textFrame == NULL)
        {
            return;
        }
        m_textFrame->setColor(VFX_COLOR_WHITE);
    }
    else
    {
        m_textBgFrame->setHidden(VFX_TRUE);
        if (m_textFrame == NULL)
        {
            return;
        }
        m_textFrame->setColor(VFX_COLOR_BLACK);
    }
}

void VappMainmenuCell::updateIsPreinstall(VfxBool isPreinstall)
{
    if (isPreinstall)
    {
        if (m_preinstallFrame == NULL)
        {
            VFX_OBJ_CREATE(m_preinstallFrame, VfxFrame, this);
            m_preinstallFrame->setImgContent(VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_ICON_PREINSTALL_APP) );
            m_preinstallFrame->setSize(MAINMENU_CELL_ICON_WIDTH, MAINMENU_CELL_ICON_HEIGHT);
            m_preinstallFrame->setPos(MAINMENU_CELL_ICON_OFFSET_X, MAINMENU_CELL_ICON_OFFSET_Y);
        }
    }
    else
    {
        VFX_OBJ_CLOSE(m_preinstallFrame);
    }
}

void VappMainmenuCell::updateEditMode(VfxBool editMode)
{
    VfxAutoAnimate::begin();
    VfxAutoAnimate::setDuration(500);
	
    if (editMode)
    {
        m_iconBgFrameEdit->setOpacity(1.0f);
    }
    else
    {
        m_iconBgFrameEdit->setOpacity(0.0f);
    }
	
    if (strncmp(m_appName, "native.mtk.home", MMI_APP_NAME_MAX_LEN) == 0)
    {
        if (editMode)
        {
            m_iconFrame->setOpacity(0.3f);
            m_textFrame->setOpacity(0.3f);
        }
        else
        {
            m_iconFrame->setOpacity(1.0f);
            m_textFrame->setOpacity(1.0f);
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

void VappMainmenuCell::onCellDisplay()
{
    if (!m_firstDisplay)
    {
        return;
    }

    m_firstDisplay = VFX_FALSE;

    setHidden(VFX_FALSE);

    // add icon bg frame
    VFX_OBJ_CREATE(m_iconBgFrame, VfxImageFrame, this);
    m_iconBgFrame->setImgContent(VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_ICON_BG) );
    m_iconBgFrame->setPos(MAINMENU_CELL_ICON_OFFSET_X, MAINMENU_CELL_ICON_OFFSET_Y);

    // add icon bg frame
    VFX_OBJ_CREATE(m_iconBgFrameEdit, VfxImageFrame, this);
    m_iconBgFrameEdit->setImgContent(VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_ICON_SHADOW) );
    m_iconBgFrameEdit->setPos(MAINMENU_CELL_ICON_OFFSET_X, MAINMENU_CELL_ICON_OFFSET_Y);
    m_iconBgFrameEdit->setOpacity(0.0f);
    m_iconBgFrameEdit->setAutoAnimate(VFX_TRUE);

    // add icon frame
    VFX_OBJ_CREATE(m_iconFrame, VfxFrame, this);
    m_iconFrame->setSize(MAINMENU_CELL_ICON_WIDTH, MAINMENU_CELL_ICON_HEIGHT);
    m_iconFrame->setPos(MAINMENU_CELL_ICON_OFFSET_X, MAINMENU_CELL_ICON_OFFSET_Y);

    // add text bg frame
    VFX_OBJ_CREATE(m_textBgFrame, VfxFrame, this);
    m_textBgFrame->setImgContent(VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_FONT_BG) );
    m_textBgFrame->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE);
    m_textBgFrame->setPos(MAINMENU_CELL_TITLE_BG_OFFSET_X, MAINMENU_CELL_TITLE_BG_OFFSET_Y);
    m_textBgFrame->setSize(MAINMENU_CELL_TITLE_BG_WIDTH, MAINMENU_CELL_TITLE_BG_HEIGHT);
    m_textBgFrame->setHidden(VFX_TRUE);

    // add text frame
    VFX_OBJ_CREATE(m_textFrame, VfxTextFrame, this);
    VfxFontDesc fontDesc;
    fontDesc.size = VFX_FONT_DESC_VF_SIZE(MAINMENU_CELL_TITLE_FONT_SIZE);
    m_textFrame->setFont(fontDesc);
    m_textFrame->setColor(VFX_COLOR_BLACK);
    m_textFrame->setFullLineHeightMode(VFX_TRUE);
    m_textFrame->setPos(MAINMENU_CELL_TITLE_OFFSET_X, MAINMENU_CELL_TITLE_OFFSET_Y);
    m_textFrame->setSize(MAINMENU_CELL_TITLE_WIDTH, MAINMENU_CELL_TITLE_HEIGHT);
    m_textFrame->setAnchor(0.0f, 0.0f);
    m_textFrame->setAutoResized(VFX_FALSE);
    m_textFrame->setTruncateMode(VfxTextFrame::TRUNCATE_MODE_END);

#if defined(__MMI_DSM_NEW__) && defined(__MR_CFG_FEATURE_DAM__)
	 if(NULL == m_stickyButton)//coffey.che 20120516
 	{
		VFX_OBJ_CREATE(m_stickyButton, VcpImageButton, this);
 	}
	m_stickyButton->setImage(VcpStateImage(IMG_ID_VAPP_WIDGET_BUTTON_DELETE, IMG_ID_VAPP_WIDGET_BUTTON_DELETE, IMG_ID_VAPP_WIDGET_BUTTON_DELETE, IMG_ID_VAPP_WIDGET_BUTTON_DELETE));
	m_stickyButton->setPos(getRect().getWidth() / 2, 0);
	m_stickyButton->setSize(getRect().getWidth() / 2, getRect().getHeight() / 2);	
	m_stickyButton->setHidden(VFX_TRUE);
#endif	

    // update state
    updateIconImage(m_cellImage);
    updateTitle(m_cellTitle);
    updateOnBottomBar(m_onBottomBar);
    updateEditMode(m_editMode);
    updateIsPreinstall(m_isPreinstall);
}

void VappMainmenuCell::onInit()
{
    VcpPageMenuCell::onInit();

    // init size
    setSize(MAINMENU_CELL_VISIBLE_WIDTH, MAINMENU_CELL_VISIBLE_HEIGHT);

    // default hidden on creation
    setHidden(VFX_TRUE);

    return;
}

/*****************************************************************************
 * Class VappMainmenuBuilder
 *****************************************************************************/

VappMainmenuBuilder::VappMainmenuBuilder():
    m_orderData(NULL),
    m_orderDataCount(0),
    m_groupData(NULL),
    m_groupDataCount(0)
{
}

void VappMainmenuBuilder::setAppOrderData(mmi_app_package_name_struct orderData[], VfxS32 count)
{
    m_orderData = orderData;
    m_orderDataCount = count;
}

void VappMainmenuBuilder::setAppGroupData(mmi_app_package_name_struct groupData[], VfxS32 count)
{
    m_groupData = groupData;
    m_groupDataCount = count;
}

void VappMainmenuBuilder::loadMenu(VappMainmenuPageMenu * menu)
{
    VfxBool firstInit = VFX_TRUE;
    for (VfxS32 i = 0; i < VAPP_COSMOS_MAINMENU_MAX_APP; ++i)
    {
        if (menu->getMenuItem(i) != NULL)
        {
            firstInit = VFX_FALSE;
            break;
        }
    }

    if (firstInit)
    {
        initFrames(menu);
        firstInit = VFX_FALSE;
    }
    else
    {
        //reorderFrames(menu);
    }
}

void VappMainmenuBuilder::initFrames(VappMainmenuPageMenu * menu)
{
    mmi_app_package_name_struct * array = NULL;
    U32 preCount = srv_appmgr_get_app_package_num(SRV_APPMGR_PRE_INSTALLED_APP_PACKAGE_FLAG_TYPE);
    if (preCount)
    {
        VFX_SYS_ALLOC_MEM(array, sizeof(mmi_app_package_name_struct) * preCount );
        mmi_ret result;
        result = srv_appmgr_get_app_package_list(SRV_APPMGR_PRE_INSTALLED_APP_PACKAGE_FLAG_TYPE, array, sizeof(mmi_app_package_name_struct) * preCount);
        VFX_ASSERT(result != MMI_RET_ERR);
    }

    VfxS32 cellIndex = 0;
    VfxS32 pageCount = 0;
    for (VfxS32 i = 0; i < m_orderDataCount; ++i)
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

        VfxBool isPre = VFX_FALSE;
        if (array)
        {
            isPre = isPreinstallApp(m_orderData[i], array, preCount);
        }
        VappMainmenuCell * cell = ::createMainmenuCell(m_orderData[i], menu, isPre);
        cell->setOnBottomBar(VFX_FALSE);
        // reposition if necessary
        cellIndex = repositionFrameIndex(menu, cellIndex);
        menu->setMenuItem(cellIndex, cell);

        ++cellIndex;
        pageCount = VFX_MAX( (cellIndex + menu->getMenuItemCountPerPage() - 1) / menu->getMenuItemCountPerPage(), pageCount);
    }

    menu->setPageCount(pageCount);

    VfxS32 groupCellIndex = 0;
    for (VfxS32 i = 0; i < m_groupDataCount; ++i)
    {
        if ( (strncmp(m_groupData[i], "", MMI_APP_NAME_MAX_LEN) == 0) ||
            (strncmp(m_groupData[i], "DELETED", MMI_APP_NAME_MAX_LEN) == 0) )
        {
            ++groupCellIndex;
            continue;
        }

        VfxBool isPre = VFX_FALSE;
        if (array)
        {
            isPre = isPreinstallApp(m_orderData[i], array, preCount);
        }
        VappMainmenuCell * cell = ::createMainmenuCell(m_groupData[i], menu, isPre);
        cell->setOnBottomBar(VFX_TRUE);
        menu->setGroupMenuItem(groupCellIndex, cell);

        ++groupCellIndex;
    }
    if (array)
    {
        VFX_SYS_FREE_MEM(array);
    }
}

VfxS32 VappMainmenuBuilder::repositionFrameIndex(VappMainmenuPageMenu * menu, VfxS32 index)
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

void VappMainmenuBuilder::saveMenu(VappMainmenuPageMenu * menu)
{
    // checks
    checkCellState(menu);

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
            VappMainmenuCell * cosmosCell = (VappMainmenuCell *)cell;
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
            strncpy(m_groupData[groupOrderIndex], "DELETED", MMI_APP_NAME_MAX_LEN);
            ++groupOrderIndex;
            continue;
        }
        VappMainmenuCell * cosmosCell = (VappMainmenuCell *)cell;
        cosmosCell->getCellAppName(m_groupData[groupOrderIndex]);
        ++groupOrderIndex;
    }


    // checks
    checkCellState(menu);
}


VappMainmenuCell * createMainmenuCellRaw(VfxImageSrc image, const VfxWString &title, VfxFrame * parent)
{
    // create the cell
    VappMainmenuCell * cell;
    VFX_OBJ_CREATE_EX(cell, VappMainmenuCell, parent, (image, title));

    return cell;
}

VappMainmenuCell * createMainmenuCell(mmi_app_package_name_struct & appName, VfxFrame * parent, VfxBool preinstall)
{
    // get app info
    VfxImageSrc image;
    VfxWString title;

    srv_app_info_struct info;
    mmi_ret result = srv_appmgr_get_app_package_info(appName, &info);
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
    else if (info.image.type == MMI_IMAGE_SRC_TYPE_NULL)
    {
        image = VfxImageSrc(IMG_ID_VAPP_COSMOS_MAINMENU_ICON_HOME);
    }
    else
    {
        VFX_ASSERT(VFX_FALSE);
    }
    title.loadFromMem(info.string);

    VappMainmenuCell * cell;
    VFX_OBJ_CREATE_EX(cell, VappMainmenuCell, parent, (image, title));
    cell->setCellAppName(appName);
    cell->setOnBottomBar(VFX_TRUE);
    cell->setIsPreinstall(preinstall);
    cell->setHidden(VFX_TRUE);

    return cell;
}

VfxBool isPreinstallApp(mmi_app_package_name_struct appName, mmi_app_package_name_struct preinstall[], VfxU32 preCount)
{
    if (strncmp(appName, "mre", 3) != 0)
    {
        return VFX_FALSE;
    }
    for (VfxU32 i = 0; i < preCount; ++i)
    {
        if (strncmp(appName, preinstall[i], MMI_APP_NAME_MAX_LEN) == 0)
        {
            return VFX_TRUE;
        }
    }
    return VFX_FALSE;
}

void VappMainmenuBuilder::checkCellState(VappMainmenuPageMenu * menu)
{
}

