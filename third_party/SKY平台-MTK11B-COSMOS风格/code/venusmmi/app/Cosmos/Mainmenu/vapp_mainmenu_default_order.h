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
 *  vapp_mainmenu_default_order.h
 *
 * Project:
 * --------
 *  Cosmos
 *
 * Description:
 * ------------
 *  Cosmos Mainmenu Default Order
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/***************************************************************************** 
 * Include
 *****************************************************************************/

#ifndef __VAPP_MAINMENU_DEFAULT_ORDER_H__
#define __VAPP_MAINMENU_DEFAULT_ORDER_H__

// declaration
extern const mmi_app_package_name_struct vappMainmenuDefaultOrder[];
extern const VfxS32 vappMainmenuDefaultOrderCount;

#ifdef __VAPP_MAINMANU_INSTANTIATE_DEFAULT_ORDER__

// definition
const mmi_app_package_name_struct vappMainmenuDefaultOrder[] = 
{
    // the shortcut bar
    "native.mtk.dialer",
    "native.mtk.msg",
    "native.mtk.musicplayer",
    "native.mtk.home",
    
    // the page content
    "native.mtk.contact",
    "native.mtk.calllog",
    "native.mtk.calendar",
    "native.mtk.camcorder",
    "native.mtk.setting",
    "native.mtk.alarm",
    "native.mtk.opera",
    "native.mtk.vapp_dlagent",
    "native.mtk.gallery",
    "native.mtk.calculator",
    "native.mtk.fmgr",
    "native.mtk.appmanager",
    "native.mtk.soundrec",
    "native.mtk.wifi",
    "native.mtk.vapp_notes",
    "native.mtk.task",
    "native.mtk.fmradio",
    "native.mtk.sync",
    "native.mtk.vapp_email",
    "native.mtk.worldclock",
    "native.mtk.app_converter",
    "native.mtk.sat",
    #if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 
    "native.mtk.hccall",
    #endif
    ""
};

const VfxS32 vappMainmenuDefaultOrderCount = sizeof(vappMainmenuDefaultOrder) / sizeof(mmi_app_package_name_struct);

#endif

#endif // __VAPP_MAINMENU_DEFAULT_ORDER_H__

