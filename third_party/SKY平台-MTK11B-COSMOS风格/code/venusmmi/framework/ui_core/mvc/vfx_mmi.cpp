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
 *  vfx_mmi.cpp
 *
 * Project:
 * --------
 *  Venus UI
 *
 * Description:
 * ------------
 *  Description
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/***************************************************************************** 
 * Include
 *****************************************************************************/

#include "vfx_mmi.h"


#include "vfx_cpp_base.h"
#include "vfx_system.h"
#include "vfx_object.h"
#include "vfx_async_signal.h"
#include "vfx_renderer.h"
#include "vfx_timer.h"
#include "vrt_canvas.h"
#include "vfx_logger.h"
#include "vfx_adp_device.h"


#ifdef __MMI_DSM_NEW__
#include "mrp_include.h"
#endif


#if !defined(__MRE_SDK__)

#include "vadp_sys_trc.h"
extern "C"
{
#include "MMIDataType.h"
}
#endif /* !defined(__MRE_SDK__) */


/***************************************************************************** 
 * Pre-Declaration
 *****************************************************************************/

#if !defined(__MRE_SDK__)
extern "C" void vdat_gpio_set_backlight_state(VfxBool on);
extern "C" void mmi_frm_pen_reset_context_for_new_screen(void);
#endif


/***************************************************************************** 
 * Define
 *****************************************************************************/

#define VFX_MMI_EMIT_LOOP_THRESHOLD     1000


/***************************************************************************** 
 * Global function
 *****************************************************************************/

void vfx_mmi_sys_init()
{
    if (vfx_sys_get_canvas_state() != VFX_SYS_STATE_UNKNOWN)
    {
        // Venus engine has been initialized, do nothing.
        //  In some case, init function may be called more than once.
        return;
    }

    VFX_LOG(VFX_INFO, VFX_MMI_INIT);

    vfx_sys_init();
}


void vfx_mmi_onidle(void)
{
    // It is the timing that there is no primitive now.
}


void vfx_mmi_check_update(void)
{
    // It is the timing that we handled a primitive or input.
    // We should process post event now.

    if (vfx_sys_get_canvas_state() == VFX_SYS_STATE_UNKNOWN)
    {
        // The venus engine is not init yet.
        return;
    }

    VFX_LOG(VFX_INFO3, VFX_MMI_CHECK_UPDATE_BEGIN);

    VfxS32 updateCountDown = VFX_MMI_EMIT_LOOP_THRESHOLD;
    VfxAsyncSignalQueue *queue = VFX_OBJ_GET_INSTANCE(VfxAsyncSignalQueue);
    do
    {
        // NOTE: 
        //  Assert here if it have infinit loop with post emit.
        //  For example, start a timer 0 in a timer callback.
        VFX_ASSERT(updateCountDown != 0);
        updateCountDown--;
        
        // Process Post Event
        queue->processEmit();       
    } while (!queue->isEmpty());


#if !defined(__MRE_SDK__)

#if defined(__MMI_TOUCH_SCREEN__) && defined(__MTK_TARGET__)
    // Reset pen event if the screen has been changed
    if (vfx_sys_get_canvas_state() != VFX_SYS_STATE_ENTRY)
    {
        mmi_frm_pen_reset_context_for_new_screen();
    }                
#endif

#endif

    VFX_LOG(VFX_INFO3, VFX_MMI_CHECK_UPDATE_END);
}


void vfx_mmi_update_scene(void)
{
    if (vfx_sys_get_canvas_state() == VFX_SYS_STATE_UNKNOWN)
    {
        // The venus engine is not init yet.
        return;
    }

    VfxAsyncSignalQueue *queue = VFX_OBJ_GET_INSTANCE(VfxAsyncSignalQueue);
    do
    {
        VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
        VfxMsec currentTime = renderer->updateScene();

        renderer->processBlockNextCommit(currentTime);

        // Commit all timers
        //  it will also callback the zero timer
        VfxTimerManager *timerManager = VFX_OBJ_GET_INSTANCE(VfxTimerManager);
        timerManager->processCommit(currentTime);

        // Process Post Event
        queue->processEmit();       
    } while (!queue->isEmpty());
}


void vfx_mmi_before_process_msg(void)
{
    // It is the timing before process any primitive.
}


#if !defined(__MRE_SDK__)

extern "C"
mmi_ret vfx_mmi_lcd_sleepout_handler(mmi_event_struct *evt)
{
    VFX_LOG(VFX_INFO, VFX_MMI_LCD_SLEEPOUT);

    VfxTimerManager *timerManager = VFX_OBJ_GET_INSTANCE(VfxTimerManager);
    timerManager->sleepOut();

    VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
    renderer->sleepOut();

    vdat_gpio_set_backlight_state(VFX_TRUE);
    
    vfx_mmi_check_update();
    vfx_mmi_update_scene();
#if defined(__MMI_DSM_NEW__)  && ((__MR_CFG_VAR_MTK_VERSION__ < 0x11B1212))
    // 11B1212版本不需要移植
    /* 解决在VM在运行的时候不锁屏*/
    /* 当黑屏从新亮屏的时候，如果*/
    /* 当前的屏幕不是venus screen，那么*/
    /* 如果直接调用vrt_canvas_wait_update */
    /* 则会assert，系统死机 */
    if (vfx_sys_get_canvas_state() == VFX_SYS_STATE_ENTRY)
    {
    	vrt_canvas_wait_update();
    }
#else
    vrt_canvas_wait_update();
#endif
    return MMI_RET_OK;
}


extern "C"
mmi_ret vfx_mmi_lcd_sleepin_handler(mmi_event_struct *evt)
{
    VFX_LOG(VFX_INFO, VFX_MMI_LCD_SLEEPIN);

    VfxRenderer *renderer = VFX_OBJ_GET_INSTANCE(VfxRenderer);
    renderer->sleepIn();

    VfxTimerManager *timerManager = VFX_OBJ_GET_INSTANCE(VfxTimerManager);
    timerManager->sleepIn();

    vdat_gpio_set_backlight_state(VFX_FALSE);
    
    vfx_mmi_check_update();    
    
    return MMI_RET_OK;
}

extern "C"
void vfx_mmi_screen_entered(void)
{
    if (vfx_sys_get_canvas_state() != VFX_SYS_STATE_ENTRY)
    {      
        return;
    }

    if (vfx_adp_lcd_get_state() == VFX_ADP_LCD_STATE_DISABLE)
    {
        return;
    }
    
    vrt_canvas_resume();
    VFX_LOG(VFX_INFO, VFX_MMI_CANVAS_RESUMED);
}

#endif

void vfx_mmi_resume_canvas(void)
{
    VFX_LOG(VFX_INFO, VFX_MMI_RESUME_CANVAS);

    if (vfx_sys_get_canvas_state() == VFX_SYS_STATE_ENTRY)
    {      
        vrt_canvas_resume();
        VFX_LOG(VFX_INFO, VFX_MMI_CANVAS_RESUMED);
    }
}

