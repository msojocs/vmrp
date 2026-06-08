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
 *  vapp_ncenter.cpp
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
 * removed!
 * removed!
 * removed!
 *
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
 * removed!
 * removed!
 *
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
 * removed!
 *
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
 * removed!
 * removed!
 *
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
 * removed!
 *
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
 * removed!
 *
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
 * removed!
 *
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
 * removed!
 * removed!
 *
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
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
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
 * removed!
 *
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
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
 * removed!
 * removed!
 *
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
 * removed!
 *
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
 * removed!
 * removed!
 *
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
 * removed!
 *
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
 * removed!
 * removed!
 *
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
 * removed!
 *
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
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

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "vfx_uc_include.h"
#include "vtst_rt_main.h"
#include "vcp_form.h"
#include "vapp_ncenter.h"
#include "vapp_ncenter_base_cell.h"
#include "vcp_status_icon_bar.h"
#include "mmi_rp_app_ncenter_def.h"
#include "mmi_rp_app_cosmos_global_def.h"

extern "C" 
{
    #include "wgui_categories_util.h"
    #include "mmi_common_app_trc.h"
    #include "vfx_adp_device.h"
     /*sunmontech Jason test code at 20120328 13:37*/
    #ifdef __NCENTER_ADD_QUICK_SETTINGS__
    	#include "ProfilesSrvGprot.h"
	#include "ProfilesSrv.h"
	#include "gpiosrvgprot.h"
	#include "SimCtrlSrvGprot.h"
	#include "ProfilesAppGprot.h"
     #endif
}
#include "vapp_ncenter_system_cell.h"
#include "vfx_primitive_frame.h"
#include "vapp_launcher_gprot.h"
#include "vapp_platform_context.h"
#include "vcp_global_popup.h"
#include "mmi_rp_app_cosmos_global_def.h"

#if defined(__MMI_SAFE_MODE__)
#include "SafeModeSrvGprot.h"
#endif

extern "C" void mmi_pen_send_event_to_new_screen(void);
extern "C" void vapp_ncenter_entry_small_screen(void);
 /*sunmontech Jason test code at 20120405 14:37*/
#ifdef __NCENTER_ADD_QUICK_SETTINGS__ 
#include "vcp_segment_button.h"
#include "mmi_rp_vapp_sound_settings_def.h"
//#include "vapp_sound_settings.h"
#include "Cosmos\SoundSettings\vapp_sound_settings.h"
#include "vapp_bt_main.h"
#include "vapp_gpio_gprot.h"
#include "vapp_setting_flight_mode_setting.h"
#include "mmi_rp_vapp_device_def.h"
#include "vapp_sound_settings_gprot.h"
#include "mmi_rp_vapp_sound_settings_def.h"
#include "mmi_rp_app_gpio_def.h"
extern "C" srv_prof_ret srv_prof_activate_profile(srv_prof_internal_profile_enum profile);
extern "C"  MMI_BOOL srv_prof_if_can_vibrate(void);
extern "C" void srv_gpio_setting_set_bl_level(S32 level, MMI_BOOL save_flag);
extern "C" U8 srv_gpio_setting_get_save_power_mode_status(void);
extern "C" void srv_gpio_setting_active_save_power_mode(U8 onoff);
class VappNCenterQuickSettings;
class VappSoundSettingsMainPage;
class VappBtPowerSwitchCell;
#endif
/***************************************************************************** 
 * Define
 *****************************************************************************/

// layout
#if defined(__MMI_MAINLCD_320X480__)
    #define VAPP_NCENTER_DEFAULT_WIDTH              (320)
    #define VAPP_NCENTER_DEFAULT_HEIGHT             (480)
    #define VAPP_NCENTER_CLEAR_CELL_HEIGHT          (51)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_X        (10)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_WIDTH    (300)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_HEIGHT   (47)
/*sunmontech Jason test code at 20120328 13:37*/
#ifdef __NCENTER_ADD_QUICK_SETTINGS__
    #define VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT     (30)
    #define VAPP_NCENTER_BOTTOM_HEIGHT              (53)
#else
    #define VAPP_NCENTER_BOTTOM_HEIGHT              (37)
#endif
    #define VAPP_NCENTER_NO_EVENT_TEXT_SIZE         (20)

#elif defined(__MMI_MAINLCD_480X800__)
    #define VAPP_NCENTER_DEFAULT_WIDTH              (480)
    #define VAPP_NCENTER_DEFAULT_HEIGHT             (800)
    #define VAPP_NCENTER_CLEAR_CELL_HEIGHT          (86)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_X        (14)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_WIDTH    (452)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_HEIGHT   (84)

    #define VAPP_NCENTER_BOTTOM_HEIGHT              (47)
    #define VAPP_NCENTER_NO_EVENT_TEXT_SIZE         (35)
#elif defined(__MMI_MAINLCD_240X320__) || defined(__MMI_MAINLCD_240X400__)
    #define VAPP_NCENTER_DEFAULT_WIDTH              (240)
    #if defined(__MMI_MAINLCD_240X320__)
    #define VAPP_NCENTER_DEFAULT_HEIGHT             (320)
    #else
    #define VAPP_NCENTER_DEFAULT_HEIGHT             (400)
    #endif
    #define VAPP_NCENTER_CLEAR_CELL_HEIGHT          (45)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_X        (14)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_WIDTH    (220)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_HEIGHT   (45)

    #define VAPP_NCENTER_BOTTOM_HEIGHT              (24)
    #define VAPP_NCENTER_NO_EVENT_TEXT_SIZE         (20)
#else
    #define VAPP_NCENTER_DEFAULT_WIDTH              (320)
    #define VAPP_NCENTER_DEFAULT_HEIGHT             (480)
    #define VAPP_NCENTER_CLEAR_CELL_HEIGHT          (51)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_X        (10)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_WIDTH    (300)
    #define VAPP_NCENTER_CLEAR_CELL_BUTTON_HEIGHT   (47)

    #define VAPP_NCENTER_BOTTOM_HEIGHT              (37)
    #define VAPP_NCENTER_NO_EVENT_TEXT_SIZE         (20)
#endif



#define VAPP_NCENTER_Z_ORDER                    (5500)
#define VAPP_NCENTER_Y_VELOCITY_THRESHOLD       (150)

#if MMI_MAX_SIM_NUM == 1
#define CELL_MEM_POOL 25
#elif MMI_MAX_SIM_NUM == 2
#define CELL_MEM_POOL 25
#elif MMI_MAX_SIM_NUM == 3
#define CELL_MEM_POOL 25
#elif MMI_MAX_SIM_NUM == 4
#define CELL_MEM_POOL 25
#else
#define CELL_MEM_POOL 25
#endif
/***************************************************************************** 
 * Class VappNCenterClearCell 
 *****************************************************************************/

VappNCenterClearCell::VappNCenterClearCell()
{};

void VappNCenterClearCell::onInit()
{
    VfxControl::onInit();
    setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
    setSize(VAPP_NCENTER_DEFAULT_WIDTH, VAPP_NCENTER_CLEAR_CELL_HEIGHT);

    VFX_OBJ_CREATE(m_clearButton, VcpButton, this);
    m_clearButton->setAnchor(0.5, 0.5);
    m_clearButton->setPos(VAPP_NCENTER_DEFAULT_WIDTH/2, VAPP_NCENTER_CLEAR_CELL_HEIGHT/2);
    m_clearButton->setIsAutoResized(VFX_TRUE, 0, VAPP_NCENTER_CLEAR_CELL_BUTTON_WIDTH);
	m_clearButton->setAutoAnimate(VFX_FALSE);
    //m_clearButton->setSize(VAPP_NCENTER_CLEAR_CELL_BUTTON_WIDTH, VAPP_NCENTER_CLEAR_CELL_BUTTON_HEIGHT);
    m_clearButton->setText(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_CLEAR_NOTIFICATION));
    m_clearButton->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);
    
}

/***************************************************************************** 
 * Class VappNCenter 
 *****************************************************************************/
VFX_IMPLEMENT_CLASS("VappNCenter", VappNCenter, VfxControl);
VFX_OBJ_IMPLEMENT_SINGLETON_CLASS(VappNCenter);

VappNCenter::VappNCenter() :
    m_panelMother(NULL),
    m_panel(NULL),
    m_form(NULL),
    m_enterTimer(NULL),
    m_leaveTimeline(NULL),
    m_eventCount(0),
    m_ongoingCount(0),
#if defined(__MMI_SAFE_MODE__)
    m_safeModeCell(NULL),
#endif
    m_operatorCell(NULL),
    m_bottomHeight(VAPP_NCENTER_BOTTOM_HEIGHT),
    m_callbackObj(NULL),
    m_clearCell(NULL),
    m_disableLeave(VFX_FALSE),
    m_disableDrag(VFX_TRUE),
    m_isInCallBack(VFX_FALSE),
    m_noTextFrame(NULL),
    m_noTextFrameCopy(NULL),
    m_entered(VFX_FALSE),
    m_currentDir(VFX_SCR_ROTATE_TYPE_NORMAL),
    m_isDrag(VFX_FALSE),
    m_enterOpera(VFX_FALSE),
    m_statusBar(NULL),
   #ifdef __NCENTER_ADD_QUICK_SETTINGS__
    m_currPageID(NCENTER_PAGE_ID_NOTIFICATIONS),
    m_segBtn(NULL),
    m_istoolbarclick(VFX_FALSE),
    m_vol(VOL_3),
    m_max(VOL_7),
    m_min(VOL_0),
    //m_slider(NULL),
   #endif
    m_isPauseRoate(VFX_FALSE)
{};

// TODO: remove when Szuwei's global memory ready
static VfxU32 g_vappNCenterMemPool[CELL_MEM_POOL * 1024 / sizeof(VfxU32)];


void VappNCenter::onInit()
{
    VFX_OBJ_CREATE(m_context, VfxContext, NULL);
    m_context->assignPool((VfxU8 *)g_vappNCenterMemPool, CELL_MEM_POOL * 1024);
    
    VfxControl::onInit();

    
    VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
    topLevel->addChildFrame(this);
    topLevel->m_signalActiveScreenBeforeEnter.connect(this, &VappNCenter::onActiveScreenEnter);
    topLevel->m_signalActiveScreenExit.connect(this, &VappNCenter::onActiveScreenExit);
    topLevel->m_signalRotated.connect(this, &VappNCenter::onRotate);
    topLevel->m_signalActivePopupEnter.connect(this, &VappNCenter::onPopupChanged);


    setSize(VAPP_NCENTER_DEFAULT_WIDTH, VAPP_NCENTER_DEFAULT_HEIGHT);
    setHidden(VFX_TRUE);
    setPosZ(VAPP_NCENTER_Z_ORDER);

    VfxS32 i;
    const vapp_ncenter_cell_cfg_struct *tbl = vapp_ncenter_get_event_cell_table();
    
	vfx_sys_mem_pool_status_struct output,old;
	m_context->statistics(old);
	
    MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CONTEXT, old.total_size, old.used_size);
    for (i = 0; i < VAPP_NCENTER_EVENT_CELL_TOTAL; i++)
    {
        m_isShowEventTable[i] = VFX_FALSE;
        // Create each event cell          
        VFX_ASSERT(tbl[i].create);

        
        VappNCenterCell *cell = (VappNCenterCell *)tbl[i].create(this);
        cell->m_signalStateChanged.connect(this, &VappNCenter::onStateChanged);
        cell->m_signalRequestLeave.connect(this, &VappNCenter::onRequestLeave);
        m_cellEventTable[i] = cell;

		m_context->statistics(output);
		MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_EVENT, cell->getId(), output.used_size - old.used_size, output.used_size);
		old = output;
    }

    const vapp_ncenter_cell_cfg_struct *tbl2 = vapp_ncenter_get_ongoing_cell_table();

    for (i = 0; i < VAPP_NCENTER_ONGOING_CELL_TOTAL; i++)
    {
        m_isShowOngoingTable[i] = VFX_FALSE;
        // Create each ongoing cell      
        VFX_ASSERT(tbl2[i].create);

        
        VappNCenterCell *cell = (VappNCenterCell *)tbl2[i].create(this);
        cell->m_signalStateChanged.connect(this, &VappNCenter::onStateChanged);
        cell->m_signalRequestLeave.connect(this, &VappNCenter::onRequestLeave);
        m_cellOngoingTable[i] = cell;
		m_context->statistics(output);
		MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_ONGOING, cell->getId(), output.used_size - old.used_size, output.used_size);
		old = output;
    }
    
}


VfxBool VappNCenter::onPenInput(VfxPenEvent &event)
{   
    if (m_panel)
    {
        switch (event.type)
        {
            case VFX_PEN_EVENT_TYPE_DOWN:
                {
                    VfxPoint dowmpt = event.getRelDownPos(this);

                    if (m_entered && dowmpt.y < MMI_STATUS_BAR_HEIGHT)
                    {
                        return VFX_FALSE;
                    }

					if (m_entered)
                    {
                        if(m_enterOpera)
                        {
                            reEnterNCenterScr();
                        }
                        if (!m_isPauseRoate)
                        {
                            vfx_adp_srs_pause();
                            m_isPauseRoate = VFX_TRUE;
                        }
                        
                        m_signalEvent.emit(VAPP_NCENTER_EVENT_EXIT);
                    }
					m_isDrag = VFX_TRUE;
					
                    
                    m_originY = m_panel->getPos().y + m_panel->getSize().height;
                    
                    VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
                    topLevel->releaseFocus(m_form); //relase Focus to original holder when pen down

                    VfxScreen* screen = topLevel->getActiveScr();
                    if (screen)
                    {
                        screen->setHidden(VFX_FALSE);
                    }

                }
                return VFX_TRUE;
            case VFX_PEN_EVENT_TYPE_ABORT:
            case VFX_PEN_EVENT_TYPE_UP:
				{
					m_isInCallBack = VFX_FALSE;
					m_panel->setAutoAnimate(VFX_TRUE);
                    m_form->setAutoAnimate(VFX_TRUE);
                    m_bottom->setAutoAnimate(VFX_TRUE);
					// use velocity to determine leave or not
					VfxPoint dowmpt = event.getRelDownPos(this);
					VfxPoint pt     = event.getRelPredictPos(this);
					VfxS32   yv;
                    
                    if (event.timeStamp == event.downTimeStamp)
                    {
                        // means only tap
                        yv = 0;
                    }
                    else
                    {
                        yv = (1000 * (pt.y - dowmpt.y)) / (event.timeStamp - event.downTimeStamp);
                    }

                    if (yv > VAPP_NCENTER_Y_VELOCITY_THRESHOLD)
					{
						m_panel->setPos(0, 0);
                        // TODO: 2 phase enter
                        m_panel->setSize(m_panelMother->getSize());
                        setIsDisabled(VFX_TRUE);
                        m_form->setIsDisabled(VFX_TRUE);

                        VFX_OBJ_CREATE(m_enterTimer, VfxTimer, m_panel);
                        m_enterTimer->m_signalTick.connect(this, &VappNCenter::onEnterEnd);
                        m_enterTimer->setStartDelay(VfxAutoAnimate::getDuration());
                        m_enterTimer->start();
					}
					else if (yv < -1 * VAPP_NCENTER_Y_VELOCITY_THRESHOLD) 
					{
						leave();
					}
					else
					{
						if (pt.y > 0.5 * getSize().height)
						{
							m_panel->setPos(0, 0);
                            m_panel->setSize(m_panelMother->getSize());
                            setIsDisabled(VFX_TRUE);
                            m_form->setIsDisabled(VFX_TRUE);

                            VFX_OBJ_CREATE(m_enterTimer, VfxTimer, m_panel);
                            m_enterTimer->m_signalTick.connect(this, &VappNCenter::onEnterEnd);
                            m_enterTimer->setStartDelay(VfxAutoAnimate::getDuration());
                            m_enterTimer->start();
						}
						else
						{
							leave();
						}
					}
				}
                break;
            case VFX_PEN_EVENT_TYPE_MOVE:
                {
                    VfxPoint dowmpt = event.getRelDownPos(this);
                    VfxPoint pt     = event.getRelPos(this);

                    VfxS32 newY = m_originY + (pt.y - dowmpt.y);
                    VfxS32 finalY;

                    VfxAutoAnimate::begin();
                    VfxAutoAnimate::setDisable(VFX_TRUE);

                    if (!m_entered)
                    {
						if(newY <= MMI_STATUS_BAR_HEIGHT)
						{
							m_panel->setSize(m_panel->getSize().width, m_form->getContentSize().height + m_bottomHeight);
							finalY = newY - (m_bottomHeight - MMI_STATUS_BAR_HEIGHT) - m_panel->getSize().height;
                            m_panel->setPos(0, finalY);
						}
                        else if (newY > MMI_STATUS_BAR_HEIGHT && newY < m_bottomHeight)
                        {
    						m_panel->setSize(m_panel->getSize().width, m_form->getContentSize().height + m_bottomHeight);
							finalY = newY - m_panel->getSize().height;
                            m_panel->setPos(0, finalY);
                        }
                        else if (newY >= m_bottomHeight && newY < m_form->getContentSize().height + m_bottomHeight && newY < m_panelMother->getSize().height)
                        {
    						m_panel->setSize(m_panel->getSize().width, m_form->getContentSize().height + m_bottomHeight);
                            finalY = newY - m_panel->getSize().height;
                            m_panel->setPos(0, finalY);
                        }
                        else if (newY >= m_form->getContentSize().height + m_bottomHeight && newY < m_panelMother->getSize().height)
                        {
                            m_panel->setPos(0, 0);
                            m_panel->setSize(m_panel->getSize().width, newY);
                        }
                        else
                        {
							if (m_form->getContentSize().height + m_bottomHeight > m_panelMother->getSize().height)
							{								
								m_panel->setPos(0, m_panelMother->getSize().height - m_form->getContentSize().height - m_bottomHeight);
								m_panel->setSize(m_panel->getSize().width, m_form->getContentSize().height + m_bottomHeight);
							}
							else
							{
								finalY = m_panelMother->getSize().height;
								m_panel->setPos(0, 0);
								m_panel->setSize(m_panel->getSize().width, finalY);
							}						                            
                        }
                    }
                    else
                    {
                        // only move position if already entered.
                        finalY = newY - m_originY;

                        if (finalY > 0)
                        {
                            finalY = 0;
                        }
                        
                        m_panel->setPos(0, finalY);
                        
                    }

                    VfxAutoAnimate::commit();
                }
                break;
            default:
                break;
        }
    }
    return VFX_TRUE;
}


VfxBool VappNCenter::onKeyInput(VfxKeyEvent &event)
{
#ifdef __MMI_DSM_NEW__
    if (event.keyCode == VFX_KEY_CODE_BACK && event.type == VFX_KEY_EVENT_TYPE_UP)
#else
    if (event.keyCode == VFX_KEY_CODE_BACK && event.type == VFX_KEY_EVENT_TYPE_DOWN)
#endif		
    {
        if(m_enterOpera)
        {
            reEnterNCenterScr();
            m_isInCallBack = VFX_FALSE;
            VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
            topLevel->releaseFocus(m_form);
            leaveInternal();
        }
        else
        {
            leave();            
        }
        return VFX_TRUE;
    }
    else if (event.keyCode == VFX_KEY_CODE_HOME && event.type == VFX_KEY_EVENT_TYPE_DOWN)
    {
        if (vapp_launcher_is_active())
        {
            leave();
        }
        return VFX_FALSE;
    }
    return VfxControl::onKeyInput(event);
}

void VappNCenter::insertTableItem(VfxS32 index, VfxS32 **table, VfxS32 *totalCount)
{
    VfxS32 i;
    for (i = *totalCount; i >= 0; i--)
    {
        (*table)[i+1] = (*table)[i];
    }
    (*totalCount) ++;
    (*table)[0] = index;
}

void VappNCenter::deleteTableItem(VfxS32 index, VfxS32 **table, VfxS32 *totalCount)
{
    VfxS32 i,j;

    for (i = 0; i < *totalCount; i++)
    {
        if ((*table)[i] == index)
        {
            break;
        }
    }

    if (i == *totalCount)
    {
        // can't find item
        VFX_DEV_ASSERT(0);
        return;
    }

    for (j = i; j < *totalCount; j ++)
    {
        (*table)[j] = (*table)[j+1];
    }

    (*totalCount) --;
}

void VappNCenter::updateTableItem(VfxS32 index, VfxS32 **table, VfxS32 *totalCount)
{
    VfxS32 i,j;

    for (i = 0; i < *totalCount; i++)
    {
        if ((*table)[i] == index)
        {
            break;
        }
    }

    if (i == *totalCount)
    {
        // can't find item
        VFX_DEV_ASSERT(0);
        return;
    }

    for (j = i - 1; j >= 0; j --)
    {
        (*table)[j+1] = (*table)[j];
    }

    (*table)[0] = index;

}

void VappNCenter::onStateChanged(VfxObject *obj, VappNcenterState state)
{
    VfxBool isShow;

	if (state == VAPP_NCENTER_STATE_SHOW)
	{
		isShow = VFX_TRUE;
	}
	else
	{
        isShow = VFX_FALSE;
	}
    
    VappNCenterCell *cell = (VappNCenterCell *)obj;
    VfxS32 index = cell->getId();

    VfxBool *showTable;
    VfxS32 *countPtr;
    VfxS32 *orderTable;
    VappNCenterCellType type = cell->getType();

    MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ONSTATE_CHANGED_1, type, index, isShow);

    if (type == VAPP_NCENTER_TYPE_EVENT)
    {
        showTable = m_isShowEventTable;
        orderTable = m_eventOrderTable;
        countPtr = &m_eventCount;
    }
    else
    {
		index -= VAPP_NCENTER_EVENT_CELL_TOTAL;
        showTable = m_isShowOngoingTable;
        orderTable = m_ongoingOrderTable;
        countPtr = &m_ongoingCount;
    }


    VfxBool needDelete = VFX_FALSE;
    VfxBool needAdd = VFX_FALSE;
    if (isShow == VFX_FALSE)
    {
        if (showTable[index] == VFX_TRUE)
        {
            showTable[index] = VFX_FALSE;
            deleteTableItem(index, &orderTable, countPtr);
            needDelete = VFX_TRUE;
        }
    }
    else
    {
        if (showTable[index] == VFX_FALSE)
        {
            showTable[index] = VFX_TRUE;
            insertTableItem(index, &orderTable, countPtr);
            needAdd = VFX_TRUE;
        }
        else
        {
            updateTableItem(index, &orderTable, countPtr);
            needDelete = VFX_TRUE;
            needAdd = VFX_TRUE;
        }
    }


    // The NCenter doesn't drag down, just update table
    if (m_panel == NULL || m_disableCellViewUpdate)
    {
        return;
    }
/*sunmontech Jason debug at 20120410*/
#ifdef __NCENTER_ADD_QUICK_SETTINGS__	
if(m_currPageID == NCENTER_PAGE_ID_QUICK_SETTINGS)
	return ;
#endif

    MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ONSTATE_CHANGED_2, type, index, isShow, needDelete, needAdd);

    if (needDelete)
    {
        if (!needAdd)
        {
            // if only delete for move, do not call release view
            MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_RELEASE_VIEW_NEED_DELETE, type, index);
            cell->releaseView();
        }

        // if releaseView trigger screen or popup changed, it may trigger flush post emit the NCenter will hide now
        if (!m_isInCallBack && getHidden() == VFX_TRUE || !m_form)
        {
            return;
        }
        
        if (type == VAPP_NCENTER_TYPE_EVENT)
        {
            m_form->removeItem(cell->getId());
            
            if (m_eventCount == 0)
            {
                m_form->removeItem('Clea');
                m_form->removeItem('Even');
                VFX_OBJ_CLOSE(m_clearCell);
            }
        }
        else
        {
            m_form->removeItem(cell->getId());

            if (m_ongoingCount == 0)
            {
                m_form->removeItem('Ongo');
            }
        }

        if (m_eventCount == 0 && m_ongoingCount == 0)
        {
            if (m_noTextFrame == NULL)
            {
                createNoTextFrame();
            }
        }
    }

    if (needAdd)
    {
        if (!needDelete)
        {
            // if only add for move, do not call create view
            VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
            cell->layout(getSize().width, topLevel->getScreenRotateType());
            MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_VIEW_NEED_ADD, type, index);
            cell->createView();
        }


        if (m_noTextFrame)
        {
            m_form->removeItem('noEv');
            VFX_OBJ_CLOSE(m_noTextFrameCopy);
            VFX_OBJ_CLOSE(m_noTextFrame);
        }



        VcpFormItemCustomFrame *sometext;
        VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);

        sometext->setSize(cell->getSize());
        sometext->attachCustomFrame(cell);
        sometext->setHeight(cell->getFoldHeight());

        if (type == VAPP_NCENTER_TYPE_EVENT)
        {
            if (m_form->getItem('Even') == NULL)
            {
                VcpFormItemCaption *capItem;
                VFX_OBJ_CREATE(capItem,  VcpFormItemCaption, m_form);
                capItem->setText(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_NOTIFICATIONS));
                m_form->insertItem(capItem, m_operatorCell->getId(), 'Even', VFX_FALSE);
            }
            m_form->insertItem(sometext, 'Even', cell->getId(), VFX_FALSE);
            m_form->insertSeparator(cell->getId());
            if (m_clearCell == NULL)
            {
                VFX_OBJ_CREATE(m_clearCell, VappNCenterClearCell, m_panelMother);
                m_clearCell->setBounds(0, 0, getSize().width, VAPP_NCENTER_CLEAR_CELL_HEIGHT);
                m_clearCell->m_clearButton->m_signalClicked.connect(this, &VappNCenter::onEventClearAll);
                
                VcpFormItemCustomFrame *sometext2;
                VFX_OBJ_CREATE(sometext2, VcpFormItemCustomFrame, m_form);

                sometext2->attachCustomFrame(m_clearCell);
                m_form->insertItem(sometext2, cell->getId(), 'Clea', VFX_FALSE);
            }

            VcpFormItemBase *lable = m_form->getItem('Even');
            VfxPoint lablePos = lable->getPos();
            VfxS32 totalHeight = m_form->getContentSize().height;
            if (lablePos.y < m_form->getSize().height)
            {
                m_form->scrollRectToVisible(VfxRect(lablePos.x, 0, m_form->getSize().width, m_form->getSize().height), VFX_TRUE);
            }
            else if (lablePos.y > totalHeight - m_form->getSize().height)
            {
                m_form->scrollRectToVisible(VfxRect(lablePos.x, totalHeight - m_form->getSize().height, m_form->getSize().width, m_form->getSize().height), VFX_TRUE);
            }
            else
            {
                m_form->scrollRectToVisible(VfxRect(lablePos.x, lablePos.y, m_form->getSize().width, m_form->getSize().height), VFX_TRUE);
            }
        }
        else
        {
            if (m_form->getItem('Ongo') == NULL)
            {
                m_form->addCaption(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_BACKGROUND_APPLICATIONS), 'Ongo');
            }
            
            m_form->insertItem(sometext, 'Ongo', cell->getId(), VFX_FALSE);
            m_form->insertSeparator(cell->getId()); 

            VcpFormItemBase *lable = m_form->getItem('Ongo');
            VfxPoint lablePos = lable->getPos();
            VfxS32 totalHeight = m_form->getContentSize().height;

            if (lablePos.y < m_form->getSize().height)
            {
                m_form->scrollRectToVisible(VfxRect(lablePos.x, 0, m_form->getSize().width, m_form->getSize().height), VFX_TRUE);
            }
            else if (lablePos.y > totalHeight - m_form->getSize().height)
            {
                m_form->scrollRectToVisible(VfxRect(lablePos.x, totalHeight - m_form->getSize().height, m_form->getSize().width, m_form->getSize().height), VFX_TRUE);
            }
            else
            {
                m_form->scrollRectToVisible(VfxRect(lablePos.x, lablePos.y, m_form->getSize().width, m_form->getSize().height), VFX_TRUE);
            }
        }
    }

}


void VappNCenter::onRequestLeave(VfxObject *obj)
{
	if (getHidden() == VFX_FALSE && m_panel)
    {
        m_callbackObj = (VappNCenterCell*) obj;
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_REQUEST_LEAVE, m_callbackObj->getType(), m_callbackObj->getId());

        if(m_enterOpera)
        {
            reEnterNCenterScr();
            m_isInCallBack = VFX_FALSE;

            VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
            topLevel->releaseFocus(m_form);
            leaveInternal();
        }
        else
        {
            leave();            
        }
	}
}

void VappNCenter::setTestPanel(VappNCTestPanel *panel)
{
    VfxS32 i;

    for (i = 0; i < VAPP_NCENTER_EVENT_CELL_TOTAL; i++)
    {
        m_cellEventTable[i]->setTestPanel(panel);
    }

    for (i = 0; i < VAPP_NCENTER_ONGOING_CELL_TOTAL; i++)
    {
        m_cellOngoingTable[i]->setTestPanel(panel);
    }

    addChildFrame(panel);
}


void VappNCenter::enter()
{
	// hidden = true and no object is requesting leave
    if (getHidden() != VFX_FALSE && !m_callbackObj)
    {
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ENTER);

        if (!m_isPauseRoate)
        {
            vfx_adp_srs_pause();
            m_isPauseRoate = VFX_TRUE;
        }
        VfxAutoAnimate::begin();
        VfxAutoAnimate::setDisable(VFX_TRUE);

        m_signalEvent.emit(VAPP_NCENTER_EVENT_ENTER); // Entering NCenter
        
        VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
        topLevel->addChildFrame(this);
        m_currentDir = topLevel->getScreenRotateType();
        setSize(getParentFrame()->getSize());

	/*sunmontech Jason test code at 20120328 13:37*/
	#ifdef __NCENTER_ADD_QUICK_SETTINGS__	
        if (m_currPageID == NCENTER_PAGE_ID_QUICK_SETTINGS)
        {
		createQuickSettingsView();
        }
	else
	#endif
	{
	        createSystemView();
        	 createCellView();
	}	 
        setHidden(VFX_FALSE);
        
        
        VfxS32 initHeight = m_form->getContentSize().height + m_bottomHeight;
        if (initHeight > m_panelMother->getSize().height)
        {
            initHeight = m_panelMother->getSize().height;
        }

    /*sunmontech Jason test code at 20120328 */
#ifdef __NCENTER_ADD_QUICK_SETTINGS__
	if((m_currPageID == NCENTER_PAGE_ID_QUICK_SETTINGS||m_currPageID == NCENTER_PAGE_ID_NOTIFICATIONS)&&m_istoolbarclick==VFX_TRUE)
	{
		m_panel->setPos(0, 0);
		m_panel->setSize(m_panelMother->getSize());
		setIsDisabled(VFX_TRUE);
		m_form->setIsDisabled(VFX_TRUE);
		  if (getHidden() == VFX_FALSE)
		    {
		        if (m_isPauseRoate)
		        {
		            vfx_adp_srs_resume();
		            m_isPauseRoate = VFX_FALSE;
		        }
		        m_signalEvent.emit(VAPP_NCENTER_EVENT_ENTERED);
		        VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
		        topLevel->captureFocus(m_form); //key support
		        m_form->setIsUnhittable(VFX_FALSE);
		        m_form->setIsDisabled(VFX_FALSE);

		        setIsDisabled(VFX_FALSE);
		        m_entered = VFX_TRUE;
		        m_isDrag = VFX_FALSE;
		        if(m_noTextFrameCopy)
		        {
		            m_noTextFrameCopy->setSize(getSize().width, VAPP_NCENTER_ONGOING_CELL_HEIGHT);
		            m_form->getItem('noEv')->setHeight(VAPP_NCENTER_ONGOING_CELL_HEIGHT);
		                
		        }
		            
		    }
		  m_istoolbarclick = VFX_FALSE;
	}
	else
	{
	        m_panel->setSize(m_panel->getSize().width, initHeight);
	        m_panel->setPos(VfxPoint(0, m_bottomHeight - m_panel->getSize().height));
	        m_form->setIsUnhittable(VFX_TRUE);		
	}
#else
        m_panel->setSize(m_panel->getSize().width, initHeight);
        m_panel->setPos(VfxPoint(0, m_bottomHeight - m_panel->getSize().height));
        m_form->setIsUnhittable(VFX_TRUE);
#endif

        capture();
        VfxAutoAnimate::commit();    

    }
}

void VappNCenter::leave()
{
    if (getHidden() == VFX_FALSE && m_panel)
    {
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_LEAVE);

		setIsDisabled(VFX_TRUE);

        VFX_OBJ_CREATE(m_leaveTimeline, VfxPointTimeline, m_panel);



        m_leaveTimeline->setTarget(m_panel);
        m_leaveTimeline->setTargetPropertyId(VRT_FRAME_PROPERTY_ID_POS);
        m_leaveTimeline->setDurationTime(250);
        
        m_leaveTimeline->setIsFromCurrent(VFX_TRUE);
		m_leaveTimeline->setToValue(VfxPoint(0, 0 - m_panel->getSize().height));
        m_leaveTimeline->setTimingFunc(VFX_TIMING_FUNC_ID_EASE_OUT);
        m_leaveTimeline->m_signalStopped.connect(this, &VappNCenter::onLeaveEnd); 
        m_leaveTimeline->start();

        VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
        topLevel->releaseFocus(m_form);
        VfxScreen* screen = topLevel->getActiveScr();
        if (screen)
        {
            screen->setHidden(VFX_FALSE);
        }
        

		//timer new feature
		VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
        renderer->blockAfterNextCommit(250, VfxCallback0(this, &VappNCenter::leaveInternal));

    }
}


void VappNCenter::onLeaveEnd(VfxBaseTimeline *timeline, VfxBool isCompleted)
{
    MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ON_LEAVE_END);
    if (isCompleted) // if not completed, that means m_leaveTimeline's parent is closed
    {
        VFX_OBJ_CLOSE(m_leaveTimeline);
    }
    //leaveInternal();
}


void VappNCenter::onEnterEnd(VfxTimer *source)
{
    VFX_OBJ_CLOSE(m_enterTimer);
    
    if (getHidden() == VFX_FALSE)
    {
        if (m_isPauseRoate)
        {
            vfx_adp_srs_resume();
            m_isPauseRoate = VFX_FALSE;
        }
        m_signalEvent.emit(VAPP_NCENTER_EVENT_ENTERED);
        VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
        topLevel->captureFocus(m_form); //key support
        m_form->setIsUnhittable(VFX_FALSE);
        m_form->setIsDisabled(VFX_FALSE);

        setIsDisabled(VFX_FALSE);
        m_entered = VFX_TRUE;
        m_isDrag = VFX_FALSE;

	#ifdef __NCENTER_ADD_QUICK_SETTINGS__
	  if(m_currPageID == NCENTER_PAGE_ID_QUICK_SETTINGS)
	  	return;
	#endif
        if(m_noTextFrameCopy)
        {
            m_noTextFrameCopy->setSize(getSize().width, VAPP_NCENTER_ONGOING_CELL_HEIGHT);
            m_form->getItem('noEv')->setHeight(VAPP_NCENTER_ONGOING_CELL_HEIGHT);
                
        }
            
	    VfxS32 i;    
	    for (i = m_eventCount - 1; i >= 0 ; i--)
	    {
	        VappNCenterCell* cell = m_cellEventTable[m_eventOrderTable[i]];		        
	        
	        cell->getParentFrame()->setCacheMode(VFX_CACHE_MODE_AUTO);		        
	    }

	
	    for (i = m_ongoingCount - 1; i >= 0; i--)
	    {		        
	        VappNCenterCell* cell = m_cellOngoingTable[m_ongoingOrderTable[i]];	
	        cell->getParentFrame()->setCacheMode(VFX_CACHE_MODE_AUTO);
	    }		    
	    
	    m_operatorCell->getParentFrame()->setCacheMode(VFX_CACHE_MODE_AUTO);
    }
}
void VappNCenter::onRequestUpdate(VfxId id, VfxS32 height)
{
	//only update when height changed
	if (m_form->getItem(id)->getSize().height != height)
	{
		if (m_noTextFrame)
		{
			FramePropertyEffectData data;
			data.minY = m_noTextFrame->getSize().height ;
			data.maxY = m_panel->getSize().height - m_bottom->getSize().height;
	        
			m_noTextFrame->setPropertyEffectCallback(
				m_form->getItem(id),
				noTextEffectCallbackForForm,
				&data,
				sizeof(data),
				VRT_FPE_TRIGGER_TYPE_DIRTY);

			m_noTextFrameCopy->setPropertyEffectCallback(
				m_noTextFrame,
				noTextEffectCallbackHide,
				NULL,
				0,
				VRT_FPE_TRIGGER_TYPE_DIRTY);
	    
		}
	    

		if (m_form->getItem(id))
		{
			VfxAutoAnimate::begin();
			VfxAutoAnimate::setDisable(VFX_FALSE);
			VfxAutoAnimate::setTimingFunc(VFX_TIMING_FUNC_ID_EASE_IN);   
			VfxAutoAnimate::setDuration(300);    
			m_form->getItem(id)->setHeight(height);
			VfxAutoAnimate::commit();    
	   
	//    	m_form->scrollItemToView(id, VCPFORM_SCROLL_BOTTOM, VFX_TRUE);
		}
	}
    
    
}

void VappNCenter::onEventClearAll(VfxObject* sender, VfxId id)
{
    MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ON_EVENT_CLEAR_ALL);
    VFX_UNUSED(sender);
    VFX_UNUSED(id);

    VfxS32 i;
    for (i = 0; i < VAPP_NCENTER_EVENT_CELL_TOTAL; i++)
    {
        m_cellEventTable[i]->onClearAll();
    }
}


void VappNCenter::createSystemView()
{
    wgui_status_icon_bar_enter_ncenter();

    VFX_DEV_ASSERT(m_panelMother == NULL);
    VFX_OBJ_CLOSE(m_panelMother);

    VFX_OBJ_CREATE(m_panelMother, VfxFrame, VFX_OBJ_GET_INSTANCE(VappPlatformContext));
    addChildFrame(m_panelMother);
    m_panelMother->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_COLOR_KEY);
    m_panelMother->setPos(0, MMI_STATUS_BAR_HEIGHT);
    m_panelMother->setSize(getSize().width, getSize().height - MMI_STATUS_BAR_HEIGHT);
    m_panelMother->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    

    VFX_OBJ_CREATE(m_panel, VfxFrame, m_panelMother);
    m_panel->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_FULLY_OPAQUE);
    m_panel->setSize(m_panelMother->getSize());
    m_panel->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);
    
    VFX_OBJ_CREATE(m_form, VcpForm, m_panel);
    m_form->setPos(0, 0);
    m_form->getFirstChildFrame()->setCacheMode(VFX_CACHE_MODE_PREVENT_PARENTS);
    m_form->setSize(getSize().width, getSize().height - MMI_STATUS_BAR_HEIGHT - m_bottomHeight);
    m_form->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    VcpFormItemCustomFrame *sometext;

#if defined(__MMI_SAFE_MODE__)
    // Safe mode cell.
    if (srv_safe_mode_is_active())
    {
        VFX_OBJ_CREATE(m_safeModeCell, VappSafeModeNotificationCell, m_panel);
        m_safeModeCell->setSize(getSize().width, m_safeModeCell->getSize().height);
        m_safeModeCell->m_signalRequestUpdate.connect(this, &VappNCenter::onRequestUpdate);
        m_safeModeCell->m_signalRequestLeave.connect(this, &VappNCenter::onRequestLeave);

        VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);
        sometext->setSize(m_safeModeCell->getSize());
        sometext->attachCustomFrame(m_safeModeCell);
        sometext->setHeight(m_safeModeCell->getFoldHeight());
        sometext->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE);
        
        m_form->addItem(sometext, m_safeModeCell->getId());
    }
#endif /* defined(__MMI_SAFE_MODE__) */

    // Operator name cell.
    VFX_OBJ_CREATE(m_operatorCell, VappNCenterOperatorCell, m_panel);
    m_operatorCell->setSize(getSize().width, m_operatorCell->getSize().height);
    m_operatorCell->m_signalRequestUpdate.connect(this, &VappNCenter::onRequestUpdate);
        
    VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);

    sometext->setSize(m_operatorCell->getSize());
    sometext->attachCustomFrame(m_operatorCell);
    sometext->setHeight(m_operatorCell->getFoldHeight());
    sometext->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE);
    sometext->setCacheMode(VFX_CACHE_MODE_FREEZE);
    
    m_form->addItem(sometext, m_operatorCell->getId());

/*sunmontech Jason test code at 20120328 13:37*/
#ifdef __NCENTER_ADD_QUICK_SETTINGS__
// bottom 
    VFX_OBJ_CREATE(m_bottom, VfxFrame, m_panel);
    m_bottom->setPos(0, m_panel->getSize().height - m_bottomHeight);
    m_bottom->setSize(getSize().width, m_bottomHeight-VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottom->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE);
    //m_bottom->setImgContent(VfxImageSrc(VAPP_IMG_NCENTER_DRAG_BAR_BG));
    m_bottom->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
    m_bottom->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    VFX_OBJ_CREATE(m_bottomImage, VfxImageFrame, m_bottom);
    m_bottomImage->setAnchor(0.5f, 0.5f);
    m_bottomImage->setPos(m_bottom->getSize().width/2, m_bottom->getSize().height/2);
    m_bottomImage->setSize(getSize().width, m_bottomHeight-VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottomImage->setResId(VAPP_IMG_NCENTER_DRAG_BAR);
    //m_bottom->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
    m_bottomImage->setAlignParent(VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID);


 //m_bottom_segmentbutton 
    VFX_OBJ_CREATE(m_bottom_segmentbutton, VfxFrame, m_panel);
    m_bottom_segmentbutton->setPos(0, m_panel->getSize().height -VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottom_segmentbutton->setSize(getSize().width, VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottom_segmentbutton->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE);
    //m_bottom_segmentbutton->setImgContent(VfxImageSrc(VAPP_IMG_NCENTER_DRAG_BAR_BG));
    m_bottom_segmentbutton->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
    m_bottom_segmentbutton->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

  VFX_OBJ_CREATE(m_segBtn, VcpSegmentButton, m_bottom_segmentbutton);
    //m_segBtn->setPos(0, m_panel->getSize().height -VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_segBtn->setSize(m_bottom_segmentbutton->getSize());
    //m_segBtn->setSize(getSize().width,m_panel->getSize().height - VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_segBtn->setType(VCP_SEGMENT_BUTTON_TYPE_RADIO);
    m_segBtn->setStyle(VCP_SEGMENT_BUTTON_STYLE_TEXT_ONLY);
    // Create 2 buttoned segment button
    VcpStateImage NotificationsImg, QsettingsImg;
	NotificationsImg.setImage(0, 0, 0, 0);
	QsettingsImg.setImage(0, 0, 0, 0);
    m_segBtn->addButton(SEGMENT_BTN_NOTIFICATIONS, NotificationsImg, VFX_WSTR_RES(STR_ID_VAPP_NCENTER_NOTIFICATION));
    m_segBtn->addButton(SEGMENT_BTN_QUICK_SETTINGS, QsettingsImg, VFX_WSTR_RES(STR_ID_VAPP_NCENTER_QUICK_SETTINGS));
    // Connect segment button with your own callback
    m_segBtn->m_signalButtonClicked.connect(this, &VappNCenter::onToolBarClick); // common button click func
    
    updataToolBarStatus();

#else
    // bottom 
    VFX_OBJ_CREATE(m_bottom, VfxFrame, m_panel);
    m_bottom->setPos(0, m_panel->getSize().height - m_bottomHeight);
    m_bottom->setSize(getSize().width, m_bottomHeight);
    m_bottom->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE);
    m_bottom->setImgContent(VfxImageSrc(VAPP_IMG_NCENTER_DRAG_BAR_BG));
    m_bottom->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    VFX_OBJ_CREATE(m_bottomImage, VfxImageFrame, m_bottom);
    m_bottomImage->setAnchor(0.5f, 0.5f);
    m_bottomImage->setPos(m_bottom->getSize().width/2, m_bottom->getSize().height/2);
    m_bottomImage->setSize(getSize().width, m_bottomHeight);
    m_bottomImage->setResId(VAPP_IMG_NCENTER_DRAG_BAR);
    m_bottomImage->setAlignParent(VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID);
#endif/*~sunmontech Jason test code at 20120328 13:37*/
}


void VappNCenter::releaseSystemView()
{
    setHidden(VFX_TRUE);
    if (m_eventCount > 0)
    {
        m_form->removeItem('Clea');
        VFX_OBJ_CLOSE(m_clearCell);
    }
    if (m_noTextFrame)
    {
        VFX_OBJ_CLOSE(m_noTextFrame);
    }
    VFX_OBJ_CLOSE(m_statusBar);        
    VFX_OBJ_CLOSE(m_leaveTimeline);
    VFX_OBJ_CLOSE(m_clearCell);
    VFX_OBJ_CLOSE(m_noTextFrame);
    VFX_OBJ_CLOSE(m_noTextFrameCopy);
    VFX_OBJ_CLOSE(m_form);
    VFX_OBJ_CLOSE(m_panel);
    VFX_OBJ_CLOSE(m_panelMother);
}


void VappNCenter::createCellView()
{
    VfxS32 i;
    VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
    VfxScrRotateTypeEnum rotate = topLevel->getScreenRotateType();
    VfxS32 width = getSize().width;
    VfxId lastId = 0xFFFF;

    if (m_eventCount > 0)
    {
        VcpFormItemCaption *capItem;
        VFX_OBJ_CREATE(capItem,  VcpFormItemCaption, m_form);
        capItem->setText(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_NOTIFICATIONS));
        m_form->insertItem(capItem, m_operatorCell->getId(), 'Even', VFX_FALSE);
    }

    for (i = m_eventCount - 1; i >= 0 ; i--)
    {
        VappNCenterCell* cell = m_cellEventTable[m_eventOrderTable[i]];
        cell->layout(width, rotate);
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_VIEW_CREATE_CELL_VIEW, cell->getType(), cell->getId());
        cell->createView();

        cell->m_signalRequestUpdate.connect(this, &VappNCenter::onRequestUpdate);
        VcpFormItemCustomFrame *sometext;
        VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);

        sometext->setSize(cell->getSize());
        sometext->attachCustomFrame(cell);
        sometext->setHeight(cell->getFoldHeight());
        sometext->setCacheMode(VFX_CACHE_MODE_FREEZE);
 
        m_form->insertItem(sometext, 'Even', cell->getId(), VFX_FALSE);
        m_form->insertSeparator(cell->getId());

        if (lastId == 0xFFFF)
        {
            lastId = cell->getId();
        }
    }

    if (m_eventCount > 0)
    {
        VFX_OBJ_CREATE(m_clearCell, VappNCenterClearCell, m_panelMother);
        m_clearCell->setBounds(0, 0, getSize().width, VAPP_NCENTER_CLEAR_CELL_HEIGHT);
        m_clearCell->m_clearButton->m_signalClicked.connect(this, &VappNCenter::onEventClearAll);
        
        VcpFormItemCustomFrame *sometext2;
        VFX_OBJ_CREATE(sometext2, VcpFormItemCustomFrame, m_form);

        sometext2->attachCustomFrame(m_clearCell);
        m_form->insertItem(sometext2, lastId, 'Clea', VFX_FALSE);
        
    }

    if (m_ongoingCount > 0)
    {
        m_form->addCaption(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_BACKGROUND_APPLICATIONS), 'Ongo');
    }

    for (i = m_ongoingCount - 1; i >= 0; i--)
    {
        
        VappNCenterCell* cell = m_cellOngoingTable[m_ongoingOrderTable[i]];
        cell->layout(width, rotate);
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_VIEW_CREATE_CELL_VIEW, cell->getType(), cell->getId());
        cell->createView();
        cell->m_signalRequestUpdate.connect(this, &VappNCenter::onRequestUpdate);

        VcpFormItemCustomFrame *sometext;
        VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);

        sometext->setSize(cell->getSize());
        sometext->attachCustomFrame(cell);
		sometext->setHeight(cell->getFoldHeight());
		sometext->setCacheMode(VFX_CACHE_MODE_FREEZE);

        m_form->insertItem(sometext, 'Ongo', cell->getId(), VFX_FALSE);
        m_form->insertSeparator(cell->getId());       
    }

    if (m_eventCount == 0 && m_ongoingCount == 0)
    {
        createNoTextFrame();
    }
}


void VappNCenter::releaseCellView()
{
    VfxS32 i;
    for (i = 0; i < VAPP_NCENTER_EVENT_CELL_TOTAL; i++)
    {   
        if (m_isShowEventTable[i])
        {
            VappNCenterCell* cell = m_cellEventTable[i];

            MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_VIEW_RELEASE_CELL_VIEW, cell->getType(), cell->getId());
            cell->releaseView();
        }   
    }


    for (i = 0; i < VAPP_NCENTER_ONGOING_CELL_TOTAL; i++)
    {
        if (m_isShowOngoingTable[i])
        {
            VappNCenterCell* cell = m_cellOngoingTable[i];

            MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_CREATE_VIEW_RELEASE_CELL_VIEW, cell->getType(), cell->getId());
            cell->releaseView();
        }
    }

}


    /*sunmontech Jason test code at 20120328 */
#ifdef __NCENTER_ADD_QUICK_SETTINGS__
void VappNCenter::updataToolBarStatus()
{
    if (m_currPageID == NCENTER_PAGE_ID_NOTIFICATIONS)
        m_segBtn->setButtonIsHighlighted(SEGMENT_BTN_NOTIFICATIONS, VFX_TRUE);
    else
        m_segBtn->setButtonIsHighlighted(SEGMENT_BTN_QUICK_SETTINGS, VFX_TRUE);
    
}

void VappNCenter::onToolBarClick(VfxObject* sender, VfxId btnID)
{
	m_istoolbarclick = VFX_TRUE;
	
   if(btnID == SEGMENT_BTN_QUICK_SETTINGS)
   {
    	VappNCenter *NcenterQuickSettings = VFX_OBJ_GET_INSTANCE(VappNCenter);
	setPageID(NCENTER_PAGE_ID_QUICK_SETTINGS);
	releaseSysViewAndCellView();
	//leaveInternal();
	NcenterQuickSettings->enter();
   	
   }
   else if(btnID == SEGMENT_BTN_NOTIFICATIONS)
   {
   	VappNCenter *Ncenter = VFX_OBJ_GET_INSTANCE(VappNCenter);
  	setPageID(NCENTER_PAGE_ID_NOTIFICATIONS);
	//leaveInternal();
	releaseQSView();
	Ncenter->enter();
   }
   else
   {
   	;
   }
    updataToolBarStatus();
}
void VappNCenter::setPageID(NcenterPageIDEnum PageID)
{
    m_currPageID = PageID;
}

void VappNCenter::releaseSysViewAndCellView()
{
		releaseSystemView();
		releaseCellView();
		//setHidden(VFX_TRUE);
		m_callbackObj = NULL;
		
}
void VappNCenter::enterQuickSettings()
{
	// hidden = true and no object is requesting leave
    if (getHidden() != VFX_FALSE && !m_callbackObj)
    {
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ENTER);

        if (!m_isPauseRoate)
        {
            vfx_adp_srs_pause();
            m_isPauseRoate = VFX_TRUE;
        }
        VfxAutoAnimate::begin();
        VfxAutoAnimate::setDisable(VFX_TRUE);

        m_signalEvent.emit(VAPP_NCENTER_EVENT_ENTER); // Entering NCenter
        
        VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
        topLevel->addChildFrame(this);
        m_currentDir = topLevel->getScreenRotateType();
        setSize(getParentFrame()->getSize());
		
	createQuickSettingsView();
	   
        setHidden(VFX_FALSE);
        
        
        VfxS32 initHeight = m_form->getContentSize().height + m_bottomHeight;
        if (initHeight > m_panelMother->getSize().height)
        {
            initHeight = m_panelMother->getSize().height;
        }
        
        m_panel->setSize(m_panel->getSize().width, initHeight);
        m_panel->setPos(VfxPoint(0, m_bottomHeight - m_panel->getSize().height));
        m_form->setIsUnhittable(VFX_TRUE);

        capture();
        VfxAutoAnimate::commit();    

    }
}
void VappNCenter::createQuickSettingsView()
{
	wgui_status_icon_bar_enter_ncenter();
    VFX_DEV_ASSERT(m_panelMother == NULL);
    VFX_OBJ_CLOSE(m_panelMother);

    VFX_OBJ_CREATE(m_panelMother, VfxFrame, VFX_OBJ_GET_INSTANCE(VappPlatformContext));
    addChildFrame(m_panelMother);
    m_panelMother->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_COLOR_KEY);
    m_panelMother->setPos(0, MMI_STATUS_BAR_HEIGHT);
    m_panelMother->setSize(getSize().width, getSize().height - MMI_STATUS_BAR_HEIGHT);
    m_panelMother->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    

    VFX_OBJ_CREATE(m_panel, VfxFrame, m_panelMother);
    m_panel->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_FULLY_OPAQUE);
    m_panel->setSize(m_panelMother->getSize());
    m_panel->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    VFX_OBJ_CREATE(m_form, VcpForm, m_panel);
    m_form->setPos(0, 0);
    m_form->getFirstChildFrame()->setCacheMode(VFX_CACHE_MODE_PREVENT_PARENTS);
    m_form->setSize(getSize().width, getSize().height - MMI_STATUS_BAR_HEIGHT - m_bottomHeight);
    m_form->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);


/*jason test code */
   //Flightmodesetting
   VappFlightModeSettingCell *m_Flightmodesetting;
   VFX_OBJ_CREATE(m_Flightmodesetting, VappFlightModeSettingCell, m_form);
   m_form->addItem(m_Flightmodesetting, NCENTER_QSETTINGS_FLIGHTMODE_S);
    //BlueTooth Switch  
    #ifdef __MMI_BT_SUPPORT__
    	VappBtPowerSwitchCell  *m_Btswitcher;
    	VFX_OBJ_CREATE(m_Btswitcher, VappBtPowerSwitchCell, m_form);
	m_form->addItem(m_Btswitcher, NCENTER_QSETTINGS_BT_S);
    #endif
    //Silent Switch
    VcpFormItemSwitchCell *silent_switch;
	VFX_OBJ_CREATE(silent_switch, VcpFormItemSwitchCell, m_form);
	silent_switch->setMainText(VFX_WSTR_RES(STR_ID_VAPP_SND_SET_SILENT_CAP));
    silent_switch->setHintText(VFX_WSTR_RES(STR_ID_VAPP_SND_SET_SILENT_HINT));
    if(srv_prof_is_profile_activated(SRV_PROF_SILENT_MODE) > 0 )
    {
        silent_switch->setSwitchStatus(VFX_TRUE);
    }
    else
    {
        silent_switch->setSwitchStatus(VFX_FALSE);
    }
    m_form->addItem(silent_switch,NCENTER_QSETTINGS_SILENT_S);   
    silent_switch->m_signalSwitchChangeReq.connect(this, &VappNCenter::onSilentSwitchChanged);
     
    //Vibration Switch
    VcpFormItemSwitchCell *vib_switch;
	VFX_OBJ_CREATE(vib_switch, VcpFormItemSwitchCell, m_form);
	vib_switch->setMainText(VFX_WSTR_RES(STR_ID_VAPP_SND_SET_VIB_CAP));
	vib_switch->setHintText(VFX_WSTR_RES(STR_ID_VAPP_SND_SET_VIB_HINT));
    if(srv_prof_if_can_vibrate())
    {
        vib_switch->setSwitchStatus(VFX_TRUE);
    }
    else
    {
        vib_switch->setSwitchStatus(VFX_FALSE);
    }
    m_form->addItem(vib_switch, NCENTER_QSETTINGS_VIB_S);
    vib_switch->m_signalSwitchChangeReq.connect(this, &VappSoundSettingsMainPage::onSwitchChanged);
#ifdef __SAVE_POWER_MODE__
   //VappSavePowerModeSwitchCell *m_savePowerSwitcher;
   VFX_OBJ_CREATE(m_savePowerSwitcher, VcpFormItemSwitchCell, m_form);
   m_savePowerSwitcher->setMainText(VFX_WSTR_RES(STR_SETTING_SAVE_POWER_MODE));
    if(srv_gpio_setting_get_save_power_mode_status())
    {
        m_savePowerSwitcher->setSwitchStatus(VFX_TRUE);
    }
    else
    {
        m_savePowerSwitcher->setSwitchStatus(VFX_FALSE);
    }
    m_form->addItem(m_savePowerSwitcher, NCENTER_QSETTINGS_SAVE_POWER_S);
    m_savePowerSwitcher->m_signalSwitchChangeReq.connect(this, &VappNCenter::onSavePowerModeSwitchChanged);
#endif

#ifdef __MMI_SETTING_VIBRATE_SWITCH__
  	VcpFormItemCustomFrame *VibSwithItemSlider;
	VFX_OBJ_CREATE(VibSwithItemSlider, VcpFormItemCustomFrame, m_form);

    VfxFrame *vibframe;
    VFX_OBJ_CREATE(vibframe, VfxFrame, VibSwithItemSlider);
    vibframe->setSize(LCD_WIDTH, CELL_HEIGHT);

    //VcpSlider *m_slider;
    VFX_OBJ_CREATE(m_vib_slider, VcpSlider, vibframe);    
    m_vib_slider->setPos(CELL_WIDTH / 2, CELL_HEIGHT / 2);
    m_vib_slider->setAnchor(0.5, 0.5);
    m_vib_slider->setLayout(VCP_SLIDER_LAYOUT_HORZ);
    m_vib_slider->setLength(SLIDER_LEN);
    m_vib_slider->setMaxValue(3);
    m_vib_slider->setMinValue(0);
    m_vib_slider->setCurrentValue(g_phnset_gpio_cntx_p->touch_vib_level);
    m_vib_slider->setTotalDiscreteStepsForPen(3-0);
    m_vib_slider->m_signalThumbReleased.connect(this, &VappNCenter::onTouchVibSliderChange);
    m_vib_slider->m_signalThumbDrag.connect(this, &VappNCenter::onTouchVibSliderChange);
    m_vib_slider->m_signalThumbPressed.connect(this, &VappNCenter::onTouchVibSliderChange);

   // VfxTextFrame *vibtext;
    //VFX_OBJ_CREATE(vibtext, VfxTextFrame, vibframe);
   // vibtext->setString(VFX_WSTR_RES(STR_SETTING_SAVE_POWER_MODE));
   // vibtext->setColor(VFX_COLOR_RES(CLR_COSMOS_TEXT_EMPTY));
    //vibtext->setFont(VfxFontDesc(VCP_LIST_MENU_FONT_SIZE_SMALL));
    //vibtext->setAlignMode(VfxTextFrame::ALIGN_MODE_CENTER);
   // vibtext->setAnchor(0.5, 0.5);
    //vibtext->setPos(MIN_IMG_LEFT_TAP, CELL_HEIGHT / 2);
    VfxImageFrame *touchvib_min;
    VFX_OBJ_CREATE(touchvib_min, VfxImageFrame, vibframe);
    touchvib_min->setAnchor(0, 0.5);
    touchvib_min->setPos(ICON_X, CELL_HEIGHT / 2);
    touchvib_min->setResId(IMG_ID_VAPP_DEVICE_BRIGHTNESS_MIN);

    VfxImageFrame *touchvib_max;
    VFX_OBJ_CREATE(touchvib_max, VfxImageFrame, vibframe);
    touchvib_max->setAnchor(1, 0.5);
    touchvib_max->setPos(CELL_WIDTH - MAX_IMG_RIGHT_TAP, CELL_HEIGHT / 2);
    touchvib_max->setResId(IMG_ID_VAPP_DEVICE_BRIGHTNESS_MAX);
	
    VibSwithItemSlider->attachCustomFrame(vibframe);
    VibSwithItemSlider->setHeight(CELL_HEIGHT);
    m_form->addItem(VibSwithItemSlider, NCENTER_QSETTINGS_TOUCH_VIB_S);
#endif
//Brightness setting

    //VappBrightnessCell *m_Brightsetting;
    //VFX_OBJ_CREATE(m_Brightsetting, VappBrightnessCell, m_form);
    //m_form->addItem(m_Brightsetting, NCENTER_QSETTINGS_BRIGHTNESS_S);
	
  	VcpFormItemCustomFrame *customItemSlider;
	VFX_OBJ_CREATE(customItemSlider, VcpFormItemCustomFrame, m_form);

    VfxFrame *frame;
    VFX_OBJ_CREATE(frame, VfxFrame, customItemSlider);
    frame->setSize(LCD_WIDTH, CELL_HEIGHT);

    //VcpSlider *m_slider;
    VFX_OBJ_CREATE(m_bl_slider, VcpSlider, frame);    
    m_bl_slider->setPos(CELL_WIDTH / 2, CELL_HEIGHT / 2);
    m_bl_slider->setAnchor(0.5, 0.5);
    m_bl_slider->setLayout(VCP_SLIDER_LAYOUT_HORZ);
    m_bl_slider->setLength(SLIDER_LEN);
    m_bl_slider->setMaxValue(5);
	m_bl_slider->setMinValue(1);
    m_bl_slider->setCurrentValue(g_phnset_gpio_cntx_p->bl_setting_level);
    MMI_TRACE(VGPIO_TRC_INFO, TRC_VGPIO_SETTING_CURR_VALUE, g_phnset_gpio_cntx_p->bl_setting_level);
    
    m_bl_slider->setTotalDiscreteStepsForPen(5-1);
	m_bl_slider->m_signalThumbReleased.connect(this, &VappNCenter::onSliderChange);
    m_bl_slider->m_signalThumbDrag.connect(this, &VappNCenter::onSliderChange);
    m_bl_slider->m_signalThumbPressed.connect(this, &VappNCenter::onSliderChange);

    VfxImageFrame *brighness_min;
    VFX_OBJ_CREATE(brighness_min, VfxImageFrame, frame);
    brighness_min->setAnchor(0, 0.5);
    brighness_min->setPos(ICON_X, CELL_HEIGHT / 2);
    brighness_min->setResId(IMG_ID_VAPP_DEVICE_BRIGHTNESS_MIN);
    
    VfxImageFrame *brighness_max;
    VFX_OBJ_CREATE(brighness_max, VfxImageFrame, frame);
    brighness_max->setAnchor(1, 0.5);
    brighness_max->setPos(CELL_WIDTH - MAX_IMG_RIGHT_TAP, CELL_HEIGHT / 2);
    brighness_max->setResId(IMG_ID_VAPP_DEVICE_BRIGHTNESS_MAX);
    
	customItemSlider->attachCustomFrame(frame);
    customItemSlider->setHeight(CELL_HEIGHT);
    m_form->addItem(customItemSlider, NCENTER_QSETTINGS_BRIGHTNESS_S);

  //Ring Vol Launcher
  	VcpFormItemCustomFrame *volItemSlider;
	VFX_OBJ_CREATE(volItemSlider, VcpFormItemCustomFrame, m_form);

    VfxFrame *vol_frame;
    VFX_OBJ_CREATE(vol_frame, VfxFrame, volItemSlider);
    vol_frame->setSize(LCD_WIDTH, CELL_HEIGHT);
	
     VFX_OBJ_CREATE(m_vol_slider, VcpSlider, vol_frame);
    m_vol_slider->setPos(BAR_X, CELL_HEIGHT/4);
    m_vol_slider->setLayout(VCP_SLIDER_LAYOUT_HORZ);
    m_vol_slider->setLength(BAR_LENGTH);
    m_vol_slider->setMaxValue(m_max);
    m_vol_slider->setMinValue(m_min);
    m_vol_slider->setTotalDiscreteStepsForPen(m_max - m_min);
    m_vol_slider->setCurrentValue(m_vol);

    m_vol_slider->m_signalUserChangedValue.connect(this, &VappNCenter::onVolSliderMoved);

    if (srv_prof_get_activated_profile() != MMI_PROFILE_SILENT)
    	setInitVol(MDI_AUD_VOL_GET_EX_LEVEL(srv_prof_get_ring_vol()));
  
    VfxImageFrame* vol_icon;
    VFX_OBJ_CREATE(vol_icon,VfxImageFrame,vol_frame);
    vol_icon->setResId(IMG_ID_VAPP_SND_SET_ICON_RING_MAX);
    vol_icon->setPos(ICON_X,CELL_HEIGHT / 4);	

    volItemSlider->attachCustomFrame(vol_frame);
    volItemSlider->setHeight(CELL_HEIGHT);
    m_form->addItem(volItemSlider, NCENTER_QSETTINGS_RING_VOL_L);	
	
    /*VFX_OBJ_CREATE(m_operatorCell, VappNCenterOperatorCell, m_panel);
    m_operatorCell->setSize(getSize().width, m_operatorCell->getSize().height);
    m_operatorCell->m_signalRequestUpdate.connect(this, &VappNCenter::onRequestUpdate);
	
    VcpFormItemCustomFrame *sometext;
    VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);

    sometext->setSize(m_operatorCell->getSize());
    sometext->attachCustomFrame(m_operatorCell);
    sometext->setHeight(m_operatorCell->getFoldHeight());
    sometext->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE);
    
    m_form->addItem(sometext, m_operatorCell->getId());*/

// bottom 
    VFX_OBJ_CREATE(m_bottom, VfxFrame, m_panel);
    m_bottom->setPos(0, m_panel->getSize().height - m_bottomHeight);
    m_bottom->setSize(getSize().width, m_bottomHeight-VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottom->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_TOP);
   // m_bottom->setImgContent(VfxImageSrc(VAPP_IMG_NCENTER_DRAG_BAR_BG));
    m_bottom->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
    m_bottom->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

    VFX_OBJ_CREATE(m_bottomImage, VfxImageFrame, m_bottom);
    m_bottomImage->setAnchor(0.5f, 0.5f);
    m_bottomImage->setPos(m_bottom->getSize().width/2, m_bottom->getSize().height/2);
    m_bottomImage->setSize(getSize().width, m_bottomHeight-VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottomImage->setResId(VAPP_IMG_NCENTER_DRAG_BAR);
    m_bottomImage->setAlignParent(VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID, VFX_FRAME_ALIGNER_MODE_MID);


 //m_bottom_segmentbutton 
    VFX_OBJ_CREATE(m_bottom_segmentbutton, VfxFrame, m_panel);
    m_bottom_segmentbutton->setPos(0, m_panel->getSize().height -VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottom_segmentbutton->setSize(getSize().width, VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_bottom_segmentbutton->setContentPlacement(VFX_FRAME_CONTENT_PLACEMENT_TYPE_RESIZE);
    //m_bottom_segmentbutton->setImgContent(VfxImageSrc(VAPP_IMG_NCENTER_DRAG_BAR_BG));
    m_bottom_segmentbutton->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
    m_bottom_segmentbutton->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_NONE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);

  VFX_OBJ_CREATE(m_segBtn, VcpSegmentButton, m_bottom_segmentbutton);
    //m_segBtn->setPos(0, m_panel->getSize().height -VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_segBtn->setSize(m_bottom_segmentbutton->getSize());
    //m_segBtn->setSize(getSize().width,m_panel->getSize().height - VAPP_NCENTER_SEGMENT_BUTTON_HEIGHT);
    m_segBtn->setType(VCP_SEGMENT_BUTTON_TYPE_RADIO);
    m_segBtn->setStyle(VCP_SEGMENT_BUTTON_STYLE_TEXT_ONLY);
    // Create 2 buttoned segment button
    VcpStateImage NotificationsImg, QsettingsImg;
	NotificationsImg.setImage(0, 0, 0, 0);
	QsettingsImg.setImage(0, 0, 0, 0);
    m_segBtn->addButton(SEGMENT_BTN_NOTIFICATIONS, NotificationsImg, VFX_WSTR_RES(STR_ID_VAPP_NCENTER_NOTIFICATION));
    m_segBtn->addButton(SEGMENT_BTN_QUICK_SETTINGS, QsettingsImg, VFX_WSTR_RES(STR_ID_VAPP_NCENTER_QUICK_SETTINGS));
    // Connect segment button with your own callback
    m_segBtn->m_signalButtonClicked.connect(this, &VappNCenter::onToolBarClick); // common button click func
    
    updataToolBarStatus();
 #ifdef __SAVE_POWER_MODE__
    updateSavePowerCell();
 #endif
    updateSilent();

}
void VappNCenter::releaseQSView()
{
  if (getHidden() == VFX_FALSE && m_isInCallBack == VFX_FALSE && m_form)
    {
	MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_LEAVE_INTERNAL);

	wgui_status_icon_bar_leave_ncenter();
	setHidden(VFX_TRUE);
	releaseQuickSettingsView();
	removeFromParentFrame();
	setIsDisabled(VFX_FALSE);
	m_entered = VFX_FALSE;
	m_isDrag = VFX_FALSE;
	m_enterOpera = VFX_FALSE;
	m_isInCallBack = VFX_FALSE;

	//timer new feature
	VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
	renderer->blockAfterNextCommit(1, VfxCallback0(this, &VappNCenter::leaveInternalEx));
    }
}
void VappNCenter::releaseQuickSettingsView()
{
	setHidden(VFX_TRUE);
	m_callbackObj = NULL;
	
	
       VFX_OBJ_CLOSE(m_statusBar);        
       VFX_OBJ_CLOSE(m_leaveTimeline);
       VFX_OBJ_CLOSE(m_form);
	VFX_OBJ_CLOSE(m_segBtn);
	//VFX_OBJ_CLOSE(silent_switch);  
       VFX_OBJ_CLOSE(m_panel);
	VFX_OBJ_CLOSE(m_panelMother);
}
#ifdef __SAVE_POWER_MODE__
void VappNCenter::updateSavePowerCell(void)
{
	VcpFormItemBase* blcell;
	VcpFormItemBase* vibcell;
	VfxBool isDisabledXXXCell = VFX_FALSE;
	//VfxS32 newVaule = (VfxS32)(3 + 0.4999);
	if(srv_gpio_setting_get_save_power_mode_status())
	{
		m_bl_slider->setCurrentValue((SRV_GPIO_BACKLIGHT_MAX_LEVEL + 1) / 2);
		srv_backlight_set_real_level(SRV_BACKLIGHT_TYPE_MAINLCD, (SRV_GPIO_BACKLIGHT_MAX_LEVEL + 1) / 2);
		#ifdef __MMI_SETTING_VIBRATE_SWITCH__
		m_vib_slider->setCurrentValue(0);
		#endif
		isDisabledXXXCell = MMI_TRUE;
	}
	else
	{
		m_bl_slider->setCurrentValue(g_phnset_gpio_cntx_p->bl_setting_level);
		srv_gpio_setting_set_bl_level((VfxS32)g_phnset_gpio_cntx_p->bl_setting_level, MMI_TRUE);
	    #ifdef __MMI_SETTING_VIBRATE_SWITCH__
		m_vib_slider->setCurrentValue(g_phnset_gpio_cntx_p->touch_vib_level);
		srv_gpio_setting_set_touch_vib_level((VfxS32)g_phnset_gpio_cntx_p->touch_vib_level, MMI_TRUE);
	   #endif
	}

	blcell = m_form->getItem(NCENTER_QSETTINGS_BRIGHTNESS_S);
	blcell->setIsDisabled(isDisabledXXXCell);
 #ifdef __MMI_SETTING_VIBRATE_SWITCH__
	vibcell = m_form->getItem(NCENTER_QSETTINGS_TOUCH_VIB_S);
	vibcell->setIsDisabled(isDisabledXXXCell);	
#endif
}
void VappNCenter::onSavePowerModeSwitchChanged(VcpFormItemSwitchCell* sender, VfxId id, VfxBool on_off)
{
	srv_gpio_setting_active_save_power_mode(on_off);
	if(srv_gpio_setting_get_save_power_mode_status())
    		m_savePowerSwitcher->setSwitchStatus(VFX_TRUE);
	else
    		m_savePowerSwitcher->setSwitchStatus(VFX_FALSE);
	updateSavePowerCell();
}
#endif
void VappNCenter::onSilentSwitchChanged(VcpFormItemSwitchCell* sender, VfxId id, VfxBool on_off)
{
    VFX_UNUSED(sender);

            if(srv_prof_is_profile_activated(SRV_PROF_SILENT_MODE) > 0 )
            {
                //on -> off
                srv_prof_activate_profile(SRV_PROF_PROFILE_1);
                sender->setSwitchStatus(VFX_FALSE);
                updateSilent();
            }
            else
            {
                //off -> on
                srv_prof_activate_profile(SRV_PROF_PROFILE_2);
                sender->setSwitchStatus(VFX_TRUE);
                updateSilent();
            }
	     //vapp_sound_settings_update_status_icon();
                   
}
void VappNCenter::updateSilent(void)
{
    VfxBool silent_on;

    if(srv_prof_is_profile_activated(SRV_PROF_SILENT_MODE) > 0)
    {
        silent_on = VFX_TRUE;
    }
    else
    {
        silent_on = VFX_FALSE;
    }

    vapp_sound_settings_update_status_icon();

    VcpFormItemBase* item;
    for(VfxU8 i= NCENTER_QSETTINGS_RING_VOL_L; i<NCENTER_QSETTINGS_TOTAL ; i++ )
    {
        switch(i)
        {
            case NCENTER_QSETTINGS_RING_VOL_L:
            {
		if (srv_sim_ctrl_get_num_of_inserted()== 0)
		{
			break;
		}
		else
		{
			if(!srv_sim_ctrl_is_inserted(MMI_SIM1))
	            	{
	                	i++;
	            	}
	            	else
	            	{
	                	break;
	            	} 
		}
          }
            default:
                break;
        }
        item = m_form->getItem(i);
        item->setIsDisabled(silent_on);
    }	
}
#ifdef __MMI_SETTING_VIBRATE_SWITCH__
void VappNCenter::onTouchVibSliderChange(VcpSlider * obj, VfxFloat newlevel)
{
    VfxU8 newVaule = (VfxU8)(newlevel + 0.4999);
    MMI_TRACE(VGPIO_TRC_INFO, TRC_VGPIO_SETTING_SLIDER_CHANGE, newVaule);
    
    srv_gpio_setting_set_touch_vib_level(newlevel, MMI_TRUE);	
}
#endif
void VappNCenter::onSliderChange(VcpSlider * obj, VfxFloat newlevel)
{
    VfxS32 newVaule = (VfxS32)(newlevel + 0.4999);
    m_bl_slider->setCurrentValue(newVaule);
    MMI_TRACE(VGPIO_TRC_INFO, TRC_VGPIO_SETTING_SLIDER_CHANGE, newVaule);
    
	srv_gpio_setting_set_bl_level((VfxS32)newlevel, MMI_TRUE);
}
void VappNCenter::setInitVol(VfxU8 vol)
{
    if(vol > m_max)
    {
        vol = m_max;
    }

    if(vol < m_min)
    {
        vol = m_min;
    }    

    m_vol = vol;

    m_vol_slider->setCurrentValue(m_vol);

}
void VappNCenter::onVolSliderMoved(VfxObject *obj, VfxFloat oldval, VfxFloat newval)
{
    srv_prof_volume_level ring_vol;
    if(m_vol == (VfxU8)newval)
    {
        return ;
    }
    m_vol = newval;
    ring_vol =  (srv_prof_volume_level)m_vol;
    srv_prof_set_current_profile_value(SRV_PROF_SETTINGS_RING_VOL_LEVEL, (void*)&ring_vol);	
    vapp_sound_settings_update_status_icon();
}
#endif/*__NCENTER_ADD_QUICK_SETTINGS__*/
void VappNCenter::layoutAllCell(VfxScrRotateTypeEnum rotate)
{
    VfxS32 i;
    for (i = 0; i < VAPP_NCENTER_EVENT_CELL_TOTAL; i++)
    {
        if (m_isShowEventTable[i])
        {
            VappNCenterCell* cell = m_cellEventTable[i];
            cell->layout(getSize().width, rotate);
        }
    }

    if (m_eventCount > 0)
    {
        m_clearCell->setSize(getSize().width, m_clearCell->getSize().height);
    }

    for (i = 0; i < VAPP_NCENTER_ONGOING_CELL_TOTAL; i++)
    {
        if (m_isShowOngoingTable[i])
        {
            VappNCenterCell* cell = m_cellOngoingTable[i];
            cell->layout(getSize().width, rotate);
        }
    }

#if defined(__MMI_SAFE_MODE__)
    if (m_safeModeCell)
    {
        m_safeModeCell->setSize(getSize().width, m_safeModeCell->getSize().height);
        m_safeModeCell->layout(getSize().width, rotate);
    }
#endif /* defined(__MMI_SAFE_MODE__) */

    m_operatorCell->setSize(getSize().width, m_operatorCell->getSize().height);
    m_operatorCell->layout(getSize().width, rotate);
    if (m_noTextFrame)
    {
        m_form->removeItem('noEv');
        VFX_OBJ_CLOSE(m_noTextFrame);
        createNoTextFrame();
    }

}


void VappNCenter::onRotate(VfxScreenRotateParam param)
{
    if (getHidden() == VFX_FALSE && m_currentDir != param.rotateTo)
	{
        if(m_enterOpera)
        {
            VfxAutoAnimate::begin();
            VfxAutoAnimate::setDisable(VFX_TRUE);

            setBounds(getParentFrame()->getBounds());
            layoutAllCell(param.rotateTo);
            
	        if(m_noTextFrameCopy)
	        {
	            m_noTextFrameCopy->setSize(getSize().width, m_noTextFrameCopy->getSize().height);
	        }

            m_currentDir = param.rotateTo;
            
            VfxAutoAnimate::commit();
        }
        else
        {
    		VFX_OBJ_CLOSE(m_statusBar);
					
			if (!m_isDrag)
			{
			    VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);        
                VfxScreen* screen = topLevel->getActiveScr();
                if (screen)
                {
                    screen->setHidden(VFX_TRUE);
                }
	            VFX_OBJ_CREATE(m_statusBar, VcpStatusIconBar, m_panelMother);
	            addChildFrame(m_statusBar);
	            m_statusBar->setPos(0, 0);
	            m_statusBar->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_AUTO);
	            m_statusBar->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
	            m_statusBar->setSize(getSize().width, m_statusBar->getSize().height);
				m_statusBar->setAutoAnimate(VFX_TRUE);
				VfxAutoAnimate::initAnimateBegin();
                VfxAutoAnimate::begin();				
				VfxAutoAnimate::setDuration(param.durTime);
				VfxAutoAnimate::setDisable(param.durTime == 0);
                m_statusBar->setSize(getParentFrame()->getSize().width, m_statusBar->getSize().height);    
                VfxAutoAnimate::commit();
                VfxAutoAnimate::initAnimateEnd();
    
			}

            
            VfxAutoAnimate::begin();
            VfxAutoAnimate::setDisable(VFX_TRUE);
            
            setAnchor(0.5f, 0.5f);
            setPos(getParentFrame()->getBounds().getMidPoint());
            
            VfxAutoAnimate::commit();
            
            VfxTransformTimeline *timeline;
            VFX_OBJ_CREATE(timeline, VfxTransformTimeline, m_panelMother);
           
            timeline->setTarget(this);
            timeline->setTargetPropertyId(VRT_FRAME_PROPERTY_ID_TRANSFORM);

            timeline->setDurationTime(param.durTime);
            

            

            VfxTransform t;
            t.data.affine.rz = VFX_DEG_TO_RAD(90.0f) * 
                (((VfxU32)param.rotateTo - (VfxU32)param.rotateFrom + 4) % 4);
            if (t.data.affine.rz > VFX_DEG_TO_RAD(180.0f))
            {
                t.data.affine.rz -= VFX_DEG_TO_RAD(360.0f);
            }
         
            timeline->setFromValue(t);
            t.setIdentity();
            timeline->setToValue(t);
            timeline->m_signalStopped.connect(this, &VappNCenter::onRotateEnd); 
            timeline->start();

            setAutoAnimate(VFX_TRUE);
            m_panelMother->setAutoAnimate(VFX_TRUE);
            VfxAutoAnimate::begin();
            VfxAutoAnimate::setDuration(param.durTime);
            VfxAutoAnimate::setDisable(param.durTime == 0);
            
            setBounds(getParentFrame()->getBounds());
            layoutAllCell(param.rotateTo);
            
	        if(m_noTextFrameCopy)
	        {
	            m_noTextFrameCopy->setSize(getSize().width, m_noTextFrameCopy->getSize().height);
	        }
            VfxAutoAnimate::commit();

            m_currentDir = param.rotateTo;
        }

	}	
}


void VappNCenter::onActiveScreenEnter(VfxWeakPtr<VfxScreen> scr)
{
    MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ON_SCREEN_CHANGED, getHidden());

    if (getHidden() == VFX_FALSE)
    {
        if (m_disableLeave == VFX_FALSE)
        {
            VappNCenterScr* nc = VFX_OBJ_DYNAMIC_CAST(scr.get(), VappNCenterScr);
            if(nc==NULL)
            {
                m_isInCallBack = VFX_FALSE;
				            
            }
			if (m_form)
			{
			    VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
	            topLevel->releaseFocus(m_form);
			}

            leaveInternal();
        }
        else
        {        
			if(m_callbackObj == NULL)
			{
	            VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
            	topLevel->captureFocus(m_form);

			}
        }
    }
    m_disableLeave = VFX_FALSE;
    m_disableCellViewUpdate = VFX_FALSE;
}
 

void VappNCenter::onActiveScreenExit(VfxWeakPtr<VfxScreen> scr)
{
    // when drag it should leave immediately.
    if (m_isDrag)
    {
        VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
        topLevel->releaseFocus(m_form);

        leaveInternal();
    }
}


void VappNCenter::onPopupChanged(VfxWeakPtr<VfxBasePopup> popup)
{
    if(popup.get())
    {
        if (getHidden() == VFX_FALSE)
        {
            VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);
            VcpGlobalPopupScr *globalPopupScr = VFX_OBJ_DYNAMIC_CAST(topLevel->getActiveScr(), VcpGlobalPopupScr);

            // not a global popup
            if (globalPopupScr == NULL)
            {
                topLevel->releaseFocus(m_form);
                leaveInternal();
            }
        }    
    }
}


void VappNCenter::leaveInternal()
{
    //m_panel->setPos(VfxPoint(0,  m_bottomHeight - m_panel->getSize().height));
    if (getHidden() == VFX_FALSE && m_isInCallBack == VFX_FALSE && m_form)
    {
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_LEAVE_INTERNAL);
        
        wgui_status_icon_bar_leave_ncenter();
        setHidden(VFX_TRUE);
	/*sunmontech Jason test code at 20120328 13:37*/
	#ifdef __NCENTER_ADD_QUICK_SETTINGS__	
	if (m_currPageID == NCENTER_PAGE_ID_QUICK_SETTINGS)//jason
		releaseQuickSettingsView();
	else
	#endif
		releaseSystemView();  

        removeFromParentFrame();
		
	    setIsDisabled(VFX_FALSE);
	    
	    m_entered = VFX_FALSE;
	    m_isDrag = VFX_FALSE;
	    m_enterOpera = VFX_FALSE;
	    m_isInCallBack = VFX_FALSE;
    
	    //timer new feature
		VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
        renderer->blockAfterNextCommit(1, VfxCallback0(this, &VappNCenter::leaveInternalEx));
    }
}

void VappNCenter::leaveInternalEx()
{    
    if (m_isPauseRoate)
    {
        vfx_adp_srs_resume();
        m_isPauseRoate = VFX_FALSE;
    }
    m_signalEvent.emit(VAPP_NCENTER_EVENT_EXITED);    


    
    if (m_callbackObj)
    {
        // becasue screen change will trigger post invoke.
        // do not call leaveInternal again when in callback
        m_isInCallBack = VFX_TRUE;
        MMI_TRACE(TRACE_GROUP_10, TRC_VAPP_NCENTER_ON_NCENTER_LEAVE, m_callbackObj->getType(), m_callbackObj->getId());
        m_callbackObj->onNCenterLeave();
        m_isInCallBack = VFX_FALSE;
    }
    m_callbackObj = NULL;
 	/*sunmontech Jason test code at 20120328 13:37*/
	#ifdef __NCENTER_ADD_QUICK_SETTINGS__	
 	if (m_currPageID == NCENTER_PAGE_ID_NOTIFICATIONS)//jason
 	#endif
 	releaseCellView();		
    
	m_signaLeaveEnd.emit();
    
}

void VappNCenter::onRotateEnd(VfxBaseTimeline *timeline, VfxBool isCompleted)
{
	if (isCompleted && m_panelMother)
    {
        VFX_OBJ_CLOSE(timeline);
    }
    VfxTopLevel *topLevel = VFX_OBJ_GET_INSTANCE(VfxTopLevel);        
    VfxScreen* screen = topLevel->getActiveScr();
    if (screen)
    {
        screen->setHidden(VFX_FALSE);
    }
    VFX_OBJ_CLOSE(m_statusBar);

    VfxAutoAnimate::begin();
    VfxAutoAnimate::setDisable(VFX_TRUE);
    setPos(0, 0);
    setAnchor(0, 0);

    VfxAutoAnimate::commit();
}


void VappNCenter::createNoTextFrame()
{
    VFX_OBJ_CREATE(m_noTextFrame, VfxTextFrame, m_panel);
    m_noTextFrame->setString(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_NO_NOTIFICATION));
    m_noTextFrame->setColor(VFX_COLOR_RES(CLR_COSMOS_TEXT_EMPTY));
    m_noTextFrame->setFont(VFX_FONT_RES(FNT_COSMOS_TEXT_EMPTY));
    m_noTextFrame->setAlignMode(VfxTextFrame::ALIGN_MODE_CENTER);
    m_noTextFrame->setAnchor(0.5f, 1.0f);
    m_noTextFrame->setPos(getSize().width/2, m_panel->getSize().height-m_bottom->getSize().height);
    VFX_OBJ_CREATE(m_noTextFrameCopy, VfxTextFrame, m_form);
    m_noTextFrameCopy->setString(VFX_WSTR_RES(STR_ID_VAPP_NCENTER_NO_NOTIFICATION));
    m_noTextFrameCopy->setColor(VFX_COLOR_RES(CLR_COSMOS_TEXT_EMPTY));
    m_noTextFrameCopy->setFont(VFX_FONT_RES(FNT_COSMOS_TEXT_EMPTY));
    m_noTextFrameCopy->setAlignMode(VfxTextFrame::ALIGN_MODE_CENTER);    
    m_noTextFrameCopy->setAutoResized(VFX_FALSE);
    m_noTextFrameCopy->setVerticalToCenter(VFX_TRUE);

    if(m_entered)
    {
        m_noTextFrameCopy->setSize(getSize().width, VAPP_NCENTER_ONGOING_CELL_HEIGHT); 
    }
    else
    {
        m_noTextFrameCopy->setSize(getSize().width, VAPP_NCENTER_NO_EVENT_TEXT_SIZE);
    }


    m_bottom->bringToFront();
    VcpFormItemCustomFrame *sometext;
    VFX_OBJ_CREATE(sometext, VcpFormItemCustomFrame, m_form);


    sometext->attachCustomFrame(m_noTextFrameCopy);
    sometext->setAutoAnimate(VFX_TRUE);
 
    m_form->addItem(sometext, 'noEv'); 


   VfxS32 half = (getSize().height - MMI_STATUS_BAR_HEIGHT - m_form->getContentSize().height - m_bottom->getSize().height) / 2 ;

    FramePropertyEffectData data;


    if (m_entered)
    {

        data.minY = m_noTextFrame->getSize().height ;
        data.maxY = m_panel->getSize().height - m_bottom->getSize().height;

        m_noTextFrame->setPropertyEffectCallback(
            m_form->getItem(m_operatorCell->getId()),
            noTextEffectCallbackForForm,
            &data,
            sizeof(data),
            VRT_FPE_TRIGGER_TYPE_DIRTY);
    }
    else
    {
		 data.minY = m_form->getItem(m_operatorCell->getId())->getSize().height + m_noTextFrame->getSize().height /2 ;
        data.maxY = m_panel->getSize().height - m_bottom->getSize().height + m_noTextFrame->getSize().height / 2;
        
        m_noTextFrame->setPropertyEffectCallback(
        m_panel,
        noTextEffectCallback,
        &data,
        sizeof(data),
        VRT_FPE_TRIGGER_TYPE_DIRTY);
    }

     m_noTextFrameCopy->setPropertyEffectCallback(
            m_noTextFrame,
            noTextEffectCallbackHide,
            NULL,
            0,
            VRT_FPE_TRIGGER_TYPE_DIRTY);
    
}


vrt_render_dirty_type_enum VappNCenter::noTextEffectCallback(
               vrt_frame_visual_property_struct *target_frame,
               const vrt_frame_visual_property_struct *watch_frame,
               void *user_data,
               vrt_u32 user_data_size)
{
    VFX_DEV_ASSERT(target_frame);
    VFX_DEV_ASSERT(watch_frame);
    VFX_DEV_ASSERT(user_data_size == sizeof(FramePropertyEffectData));
    FramePropertyEffectData *userData = (FramePropertyEffectData *)user_data;

    VfxS32 finalY = watch_frame->bounds.size.height - VAPP_NCENTER_BOTTOM_HEIGHT;
	VfxS32 half = ( userData->maxY - userData->minY) / 2;
	if (finalY > userData->minY + half)
	{
		finalY = userData->minY + half;
	}
    target_frame->pos.y = finalY;
    return VRT_RENDER_DIRTY_TYPE_DIRTY;
}
vrt_render_dirty_type_enum VappNCenter::noTextEffectCallbackForForm(
               vrt_frame_visual_property_struct *target_frame,
               const vrt_frame_visual_property_struct *watch_frame,
               void *user_data,
               vrt_u32 user_data_size)
{
    VFX_DEV_ASSERT(target_frame);
    VFX_DEV_ASSERT(watch_frame);
    VFX_DEV_ASSERT(user_data_size == sizeof(FramePropertyEffectData));
    FramePropertyEffectData *userData = (FramePropertyEffectData *)user_data;

    VfxS32 half = ( userData->maxY - userData->minY - watch_frame->bounds.size.height) / 2;

    
    if (half<VAPP_NCENTER_ONGOING_CELL_HEIGHT/2)
    {   
        target_frame->opacity = 1;
        target_frame->pos.y = userData->minY + watch_frame->bounds.size.height + (VAPP_NCENTER_ONGOING_CELL_HEIGHT - VAPP_NCENTER_NO_EVENT_TEXT_SIZE)/2; 
    }    
    else
    {
        target_frame->opacity = 1;        
        target_frame->pos.y = userData->minY + watch_frame->bounds.size.height + half;
    }

    if(half<0)
    {
        target_frame->opacity = 0;
    }
        

    return VRT_RENDER_DIRTY_TYPE_DIRTY;
}
vrt_render_dirty_type_enum VappNCenter::noTextEffectCallbackHide(
               vrt_frame_visual_property_struct *target_frame,
               const vrt_frame_visual_property_struct *watch_frame,
               void *user_data,
               vrt_u32 user_data_size)
{
    VFX_DEV_ASSERT(target_frame);
    VFX_DEV_ASSERT(watch_frame);
    VFX_UNUSED(user_data);
    VFX_UNUSED(user_data_size);

        
    target_frame->opacity = 1 - watch_frame->opacity;

    return VRT_RENDER_DIRTY_TYPE_DIRTY;
}

void VappNCenter::reEnterNCenterScr()
{      
        //if(m_enterOpera && mmi_frm_scrn_is_present(GRP_ID_ROOT, SCR_ID_NCENTER, MMI_FRM_NODE_ALL_FLAG) && m_isInCallBack == VFX_FALSE)
        if(m_ncenterScrPtr.get())
        {
            // !m_enterOpera means in Java or others pluto screen
            if((!m_enterOpera || m_currentDir != m_ncenterScrPtr.get()->getDir()) && m_isInCallBack == VFX_FALSE)
            {
                
                m_isInCallBack = VFX_TRUE;
                
                m_isDrag = VFX_FALSE;                           
                setHidden(VFX_TRUE);                            
                
                BOOL isFreeze = gdi_lcd_get_freeze();
                gdi_lcd_freeze(TRUE);
                
                mmi_pen_send_event_to_new_screen();     
                mmi_frm_scrn_close(GRP_ID_ROOT, SCR_ID_NCENTER);
                
                
                                        
                setHidden(VFX_FALSE);
                gdi_lcd_freeze(isFreeze);
                mmi_pen_send_event_to_new_screen();     
                vapp_ncenter_entry_small_screen();
                
            }
        }

}
/***************************************************************************** 
 * Class VappNCenterScr 
 *****************************************************************************/
VFX_IMPLEMENT_CLASS("VappNCenterScr", VappNCenterScr, VfxAppCatScr);
void VappNCenterScr::onInit()
{
    VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
    m_dir = renderer->getPreScreenRotateType();
    renderer->setScreenRotateType(m_dir);
    
    VfxAppCatScr::onInit();

    VFX_OBJ_CREATE(m_statusBar, VcpStatusIconBar, this);
    m_statusBar->setPos(0, 0);
    m_statusBar->setBgColor(VFX_COLOR_RES(CLR_COSMOS_BG_MAIN));
	m_statusBar->setSize(getSize().width, m_statusBar->getSize().height);
	m_statusBar->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_AUTO);

    VfxScreenRotateParam para;
    para.rotateTo = m_dir;
    VappNCenter *NCenter = VFX_OBJ_GET_INSTANCE(VappNCenter);
    NCenter->setNCenterScr(this);
    NCenter->onRotate(para);
    NCenter->m_signaLeaveEnd.connect(this, &VappNCenterScr::onLeaveEnd);
}

static mmi_ret vapp_ncenter_close_scr(mmi_event_struct *evt)
{
	VFX_UNUSED(evt);
    mmi_frm_scrn_close(GRP_ID_ROOT, SCR_ID_NCENTER);  
    return MMI_RET_OK;
}    
void VappNCenterScr::onEnter(VfxBool isBackward)
{
    VfxAppCatScr::onEnter(isBackward);

    VappNCenter *NCenter = VFX_OBJ_GET_INSTANCE(VappNCenter);
    if (isBackward)
    {
		// close scr only when disableLeave = false
        if (NCenter->getDisableLeaveOnScreeChange() == FALSE)
        {            
			NCenter->setEnterOpera(VFX_FALSE);
       		// use MMI_FRM_POST_EVENT instead vfxPostInvoke0 to ensure post event after pluto entering flow
            mmi_event_struct postInvoke;
            MMI_FRM_INIT_EVENT(&postInvoke, 0);
            MMI_FRM_POST_EVENT(&postInvoke, vapp_ncenter_close_scr, NULL);           
        }
    }
    else
    {
		NCenter->disableLeaveOnScreenChange();
        NCenter->enter();
    }
}


VfxScrRotateTypeEnum VappNCenterScr::onQueryRotate(const VfxScreenRotateParam &param)
{
    VappNCenter *NCenter = VFX_OBJ_GET_INSTANCE(VappNCenter);
    if (NCenter->getEnterOpera())
    {
    	return param.rotateTo;        
    }
    else
    {
        return m_dir;
    }
}


void VappNCenterScr::onRotate(const VfxScreenRotateParam &param)
{
    VappNCenter *NCenter = VFX_OBJ_GET_INSTANCE(VappNCenter);
    if (NCenter->getEnterOpera())
    {
        
        VfxAutoAnimate::begin();
        VfxAutoAnimate::setDisable(VFX_TRUE);

		VfxAppCatScr::onRotate(param);
        m_statusBar->setSize(getSize().width, m_statusBar->getSize().height);
        
        VfxAutoAnimate::commit();

    }
    else
    {
		VfxAppCatScr::onRotate(param);
        m_statusBar->setSize(getSize().width, m_statusBar->getSize().height);

    }
}


void VappNCenterScr::onLeaveEnd()
{
    mmi_frm_scrn_close(GRP_ID_ROOT, SCR_ID_NCENTER);
}

/*sunmontech Jason test code at 20120328 */
#ifdef __NCENTER_ADD_QUICK_SETTINGS__
 /***************************************************************************** 
 * Class VappNCenterQuickSettings 
 *****************************************************************************/
VFX_IMPLEMENT_CLASS("VappNCenterQuickSettings", VappNCenterQuickSettings, VfxControl);
VFX_OBJ_IMPLEMENT_SINGLETON_CLASS(VappNCenterQuickSettings);  
VappNCenterQuickSettings::VappNCenterQuickSettings() :
    m_panel(NULL),
    m_form(NULL)
{};
void VappNCenterQuickSettings::onInit()
{
	 VfxControl::onInit();
}
void VappNCenterQuickSettings::enter()
{

        m_signalEvent.emit(VAPP_NCENTER_EVENT_ENTER); // Entering NCenter
        createView();

    
}
void VappNCenterQuickSettings::createView()
{
	wgui_status_icon_bar_enter_ncenter();
   VFX_OBJ_CREATE(m_panel, VfxFrame, this);
    m_panel->setOpaqueMode(VFX_FRAME_OPAQUE_MODE_FULLY_OPAQUE);
    m_panel->setSize(getSize());
    m_panel->setAlignParent(VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE, VFX_FRAME_ALIGNER_MODE_SIDE);
    
    VFX_OBJ_CREATE(m_form, VcpForm, this);
    m_form->setAutoAnimate(VFX_TRUE);
    m_form->setSize(getSize());
    m_form->setAlignParent(
        VFX_FRAME_ALIGNER_MODE_SIDE,
        VFX_FRAME_ALIGNER_MODE_SIDE,
        VFX_FRAME_ALIGNER_MODE_SIDE,
        VFX_FRAME_ALIGNER_MODE_SIDE); 

/*jason test code */
    //Silent Switch
    VcpFormItemSwitchCell *silent_switch;
	VFX_OBJ_CREATE(silent_switch, VcpFormItemSwitchCell, this);
	silent_switch->setMainText(VFX_WSTR_RES(STR_ID_VAPP_SND_SET_SILENT_CAP));
    silent_switch->setHintText(VFX_WSTR_RES(STR_ID_VAPP_SND_SET_SILENT_HINT));
    if(srv_prof_is_profile_activated(SRV_PROF_SILENT_MODE) > 0 )
    {
        silent_switch->setSwitchStatus(VFX_TRUE);
    }
    else
    {
        silent_switch->setSwitchStatus(VFX_FALSE);
    }
    m_form->addItem(silent_switch, 1);
}

VfxBool VappNCenterQuickSettings::onPenInput(VfxPenEvent &event)
{
	return VFX_TRUE;
}

VfxBool VappNCenterQuickSettings::onKeyInput(VfxKeyEvent &event)
{
	return VfxControl::onKeyInput(event);
}
#endif
