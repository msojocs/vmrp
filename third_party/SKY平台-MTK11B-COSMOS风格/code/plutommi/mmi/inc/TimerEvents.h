/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
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
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * TimerEvents.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   This file is intends for MMI timer resource.
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
/**
 * Copyright Notice
 * ?2002 - 2003, Pixtel Communications, Inc., 1489 43rd Ave. W.,
 * Vancouver, B.C. V6M 4K8 Canada. All Rights Reserved.
 *  (It is illegal to remove this copyright notice from this software or any
 *  portion of it)
 */
/**************************************************************

   FILENAME : TimerEvents.h

   PURPOSE     : struct, defines & globle variables

   REMARKS     : nil

   AUTHOR      : Magesh k

   DATE     : sep' 03, 2002

**************************************************************/

#ifndef _PIXCOM_TIMER_EVENT_H
#define _PIXCOM_TIMER_EVENT_H

#ifndef __MMI_FEATURES__
#error "Please include MMI_features.h before this header file!"
#endif

typedef enum
{
    /*
     * Start for for Keypad based timer.
     */
    KEY_TIMER_ID_NONE = 0,
    KEY_TIMER_ID0 = 1,
    KEY_TIMER_ID1,
    KEY_TIMER_ID2,
    KEY_TIMER_ID3,
    KEY_TIMER_ID4,
    KEY_TIMER_ID5,
    KEY_TIMER_ID6,
    KEY_TIMER_ID7,
    KEY_TIMER_ID8,
    KEY_TIMER_ID9,
    KEY_TIMER_ID10,
    KEY_TIMER_ID11,
    KEY_TIMER_ID12,
    KEY_TIMER_ID13,
    KEY_TIMER_ID14,
    KEY_TIMER_ID15,
    KEY_TIMER_ID16,
    KEY_TIMER_ID17,
    KEY_TIMER_ID18,
    KEY_TIMER_ID19,
    KEY_TIMER_ID20,
    KEY_TIMER_ID21,
    KEY_TIMER_ID22,
    KEY_TIMER_ID23,
    KEY_TIMER_ID24,
    KEY_TIMER_ID25,
    KEY_TIMER_ID26,
    KEY_TIMER_ID27,
    KEY_TIMER_ID28,
    KEY_TIMER_ID29,
    KEY_TIMER_ID30,
    KEY_TIMER_ID31,
    KEY_TIMER_ID32,
    KEY_TIMER_ID33,

    /*
     * Start for for UI based timer.
     */
    UI_TIMER_ID_BASE,
    UI_TIMER_ID_MAX = (UI_TIMER_ID_BASE + 10),

    /*
     * Start for MMI APP based timer
     */
    MMI_TIMER_BASE = UI_TIMER_ID_MAX + 1,

    MMI_MEMORY_MONITOR_TIMER,

    /*
     * For GUI
     */
    UI_ELEMENTS_TIMER,

    /*
     * For Sim detection timers
     */
    SAT_MESSAGE_WAIT_TIMER,
    SAT_TERMINAL_RESP_TIMER,
    /* micha1108 */
    SAT_PLAY_TONE_TIMER,
    SAT_SEND_DTMF_TIMER,
    SAT_HELP_TIMER,
    /* micha0201 */
    SAT_500_TIMER,
    SAT_3000_TIMER,
    SAT_REFRESH_REBOOT_TIMER,
    KEYPADUNLOCKEDMESSAGE_TIMER,

    /*
     * Timer for showing "press to unlock" after pressing
     */
    KEYPADUNLOCKEDHELPMESSAGE_TIMER,

    /*
     * For Call Management 
     */
    CM_ALERT_NOTIFYDURATION_TIMER,
    CM_AUTOANSWER_NOTIFYDURATION_TIMER,
    CM_GET_TIME_CALLBACK,
    CM_NOTIFYDURATION_TIMER,
    CM_CTR_TIMER,
    CM_AUTO_QUICK_END_TIMER,
    CM_VIB_NOTIFYDURATION_TIMER,

	/*
     * For SS
     */ 
    USSR_SESSION_TIMER,
    SS_NOTIFY_TIMER,

    /*
     * For Bootup/shutdown
     */
    SYSTEM_SHUTDOWN,
    TIMER_REAL_TIME_CLOCK,
    APHORISM_SCR_TIMER,
    WELCOME_SCR_TIMER,
    KEYPAD_LOCK_TIMER,
    LOW_BATTERY_SCR_TIMER,
    BOOTUP_CHARGER_DETECT_TIMER,
    SEARCHING_SCREEN_TIMER, /* Robin 1107 */
    RAC_ACTIVATION_TIMER, /* Gemini: timer of RAC activation */
    BOOTUP_WAITING_EVENT_TIMER,
    BOOTUP_ANIMATION_TIMER,
    BOOTUP_WAITING_SCREEN_TIMER,
    BOOTUP_WELCOME_SCREEN_TIMER,
    SHUTDOWN_PROCESS_TIMER,
    
    /*
     * for PHB 
     */
    PHB_NOTIFYDURATION_TIMER,
    PHB_READ_OPT_FILED_TIMER,
    PHB_READ_SPEED_DIAL_TIMER,
    PHB_QUICK_SEARCH_TIMER,
    PHB_LIST_SPEAK_NAME_TIMER,
    PHB_NAME_SORT_TIMER,  // only for __MMI_PHB_BOOT_UP_RECURSIVE_SORTING__
    PHB_PBAP_WRITER_TIMER,
    PHB_1ST_STARTUP_ASK_TIMER,
    PHB_1ST_STARTUP_ASK_TIMER_SIM2,
    PHB_1ST_STARTUP_ASK_TIMER_SIM3,
    PHB_1ST_STARTUP_ASK_TIMER_SIM4,
	PHB_CHOOSE_FIELD_TIMER,

    /*
     * for Call Log
     */
    CHIST_READ_LOG_TIMER,
    CHIST_READ_LOG_SIM2_TIMER,

    /*
     * for SMS
     */
    MESSAGES_SMS_BOOTUP_TIMER_ID,
    MESSAGES_SMS_BOOTUP_NO_LIST_RSP_TIMER_ID,
    MESSAGES_CB_BOOTUP_TIMER_ID,
    MESSAGES_SUBLCD_TIMER_ID,
    MESSAGES_INPROGRESS_TIMER_ID,
    MESSAGES_TTS_COUNT_DOWN_TIMER_ID,
    MESSAGES_LMS_RECV_TIMER_ID,
    MESSAGES_EMS_OBJECT_PLAY_TIMER_ID,
    MESSAGES_CONTINUE_SEND_SMS_PROCESS_TIMER_ID,

	MMI_MSG_FAILED_POPUP_TIMER_ID,

    /*
     * for Help Application Timer
     */
    HELP_APPLICATION_TIMER,

    /*
     * for Ring Tone Compser Timer
     */
    TIMER_COMPOSE_RINGTONE,

    /*
     * for Alarm timer
     */
    ALARM_TIMER_WAIT_FOR_CALL,
    ALARM_TIMER_WAIT_FOR_RESPONSE,
    ALARM_SIMULATE_ON_WIN32,
    ALARM_ALERT_NOTIFYDURATION_TIMER,

	/*
	 *	TDL timer	
	 */
	TDL_TIMER_WAIT_FOR_CALL,
	
    /*
     * for Idle and Misc 
     */
    KEY_VOL_TIMER_ID,
    SP_FAIL_TIMER,
    EARPHONE_SCR_TIMER,
    BACKLIGHT_TIMER,
    IDLE_SHOW_BADFILE_WARNING_TIMER,
    IDLE_SHOW_SUB_BADFILE_WARNING_TIMER,
    IDLE_APP_AUTO_LOCK_TIMER_ID,
    EMERGENCY_NO_DIALED_TRANSIANT_SCR_TIMER,
    SML_AUTOLOCK_IDLE_NOTIFY_TIMER,
    IDLE_HS_DEVICE_STATUS_REFRESH_TIMER,
    IDLE_HS_ENTRY_GUARD_TIMER,
    IDLE_HS_SOFTNOTE_GUARD_TIMER,
    IDLE_HS_RECENT_EVENT_GUARD_TIMER,
    IDLE_HS_CLEAR_ALL_GUARD_TIMER,
    IDLE_TB_ENTRY_GUARD_TIMER,
    IDLE_NMGR_GUARD_TIMER,
    IDLE_NMGR_PROC_NEXT_TIMER,

    IDLE_HS32_REVERT_TIMER,
    
#ifdef __OP12__
    IDLE_DELAYED_RSK_TIMER,
#endif

    /*
     * for SIM 
     */
    CODE_VERIFIED_TIMER,

    /*
     * for Organize,  Fun and Game
     */
    PLAY_DOWNLOAD_AUDIO,
    SCREENSAVER_TIMER,
    AT_TEST_TIMER,
    TIMER_POPUP_HEALTH_MENSIS,

    /*
     * for input method
     */
    T9_REQ_TIMER,
    T9_FRAME_TIMER,
    BLINKING_CURSOR,
    ZI_REQ_TIMER,
    ZI_FRAME_TIMER,
    NOTE_MULTITAP_TIMER,
    CSTAR_REQ_TIMER,
    CSTAR_FRAME_TIMER,

    /*
     * for Engineering mode
     */
    EM_NOTIFYDURATION_TIMER,
    EM_RINGTONE_HIGHLIGHT_TIMER,
    EM_GPRS_PING_TIMER,
    EM_GPRS_SOC_DEMO_APP_TIMER,
    EM_NW_EVENT_PLAY_TONE_TIMER,
    FM_LCD_COLOR_CHANGE_TIMER,
    FM_AUTO_TEST_COMMNON_TIMER,
    FM_RTC_START_TIMER,
    FM_VIB_TIMER,
    FM_GENERAL_TIMER,

    /*for CUI ToneSelector*/
    CUI_TONE_SELECTOR_PLAY_TIMER,

    /*
     * for Profiles and Settings
     */
    TIMER_PROFILES_TONE_PLAY,
    TIMER_PROFILES_ALERT_PLAY,
    TIMER_KEYPAD_BACKLIGHT,
    /* micha1109 */
    TIMER_SET_GPRS_MODE,
    AT_PUK_IN_CALL_TIMER,
    FLIGHT_MODE_CAMP_ON_TONE_TIMER,
    FLIGHT_MODE_DEACTIVATE_BT_TIMER,

    /*
     * for Common Screen
     */
    POPUP_TIMER,
    BOOTUP_POPUP_TIMER,
    POPUP_SUBLCD,

    /*
     * for USB
     */
    USB_SHUTDOWN_SCR_TIMER,
    PENDING_USBDETECTION_HDLR_TIMER,

    /*
     * for file management
     */
    FMGR_AUDIO_TIMER,
    FMGR_SCR_TIMER,
    FMGR_PROG_UPDATE_TIMER,
    FMGR_DELAY_DECODE_0,
    FMGR_DELAY_DECODE_1,
    FMGR_DELAY_DECODE_2,
    FMGR_DELAY_DECODE_3,
    FMGR_DELAY_DECODE_4,
    FMGR_DELAY_DECODE_5,
    FMGR_DELAY_DECODE_6,
    FMGR_DELAY_DECODE_7,
    FMGR_DELAY_DECODE_8,
    FMGR_DELAY_DECODE_9,
    FMGR_DELAY_DECODE_10,
    FMGR_DELAY_DECODE_11,
    FMGR_DELAY_DECODE_12,
    FMGR_DELAY_DECODE_13,
    FMGR_DELAY_DECODE_14,
    FMGR_DELAY_DECODE_15,
    FMGR_DM_FWU_SHUTDOWN_TIMER,


    /*
     * for Bluetooth
     */
    BT_INQ_TIMER,
    BT_RING_TIMER,
    BT_AUD_OFF_TIMER,
    BT_DEV_DISC_TIMER,
    BT_DISCOVER_PERIOD_TIMER,
    BT_CMD_GUARD_TIMER,
    BT_MAKE_IDLE_STATE_TIMER,
    BT_WAIT_POWER_OFF_TIMER,
    BT_WAIT_POWER_ON_TIMER,
    BT_POPUP_TIMER,
    BT_PASSCODE_INPUT_PROMPTED_TIMER,
    BT_DISCONNECT_2ND_INCOMING_HFG_TIMER,
    BT_DEACTIVATE_PROHIBITED_PROFILES_TIMER,
    BT_ACTIVATE_PROFILE_TIMER,
    BT_WAIT_HW_READY_TIMER,
    BT_LP2_TESTING_TIMER,
    BT_LP2_DELAY_TIMER,
    BT_DELAY_DEACTIVATE_OBEX_RELATED_PROFILES_TIMER,
    BT_DELAY_AVRCP_CONN_POPUP_TIMER,
    BT_DELAY_AVRCP_DISC_POPUP_TIMER,
    BT_POWER_OFF_GUARD_TIMER,
    BT_DELAY_SCO_CONN_POPUP_TIMER,
    BT_RELEASE_ALL_CON_GUARD_TIMER,
    BT_DUMMY_SCR_TIMER,
    BT_TEMP_VISIBLE_TIMER,
    BT_BPP_DUMMY_SCR_TIMER,
    BT_SEND_DUMMY_TIMER,
    BT_AUTHORIZE_SCREEN_OPP_TIMER,
    BT_AUTHORIZE_SCREEN_FTP_TIMER,
    BT_AUTHORIZE_SCREEN_BIP_TIMER,
    BT_AUTHORIZE_SCREEN_HID_TIMER,
    BT_AUTHORIZE_SCREEN_HFP_TIMER,
    BT_AUTHORIZE_SCREEN_HSP_TIMER,
    BT_AUTHORIZE_SCREEN_SPP_TIMER,
    BT_AUTHORIZE_SCREEN_DUN_TIMER,
    BT_AUTHORIZE_SCREEN_FAX_TIMER,
    BT_AUTHORIZE_SCREEN_SIMAP_TIMER,
    BT_AUTHORIZE_SCREEN_PBAP_TIMER,
    BT_AUTHORIZE_SCREEN_SYNC_TIMER,
    
    
    /*
     * for BT OPP request timeout
     */
    BT_OPP_REQ_TIMER,
    BT_OPP_ACCEPT_CONNECT_TIMER,
    BT_OPP_ABORT_REQ_TIMER,
    BT_OPP_PUSH_IND_TIMER,
    BT_OPP_CACULATE_SENDING_SIZE,
    /*
     * for BT FTP request timeout
     */
    BT_FTPS_PUSH_FH_CHECK_TIMER,
    BT_FTPC_BROWSE_FILES_FAILED_TIMER,

    /*
     * for BT BPP printing timeout
     */
    BT_BPP_PRINTING_TIMER,
    
    /*
     * for BT BIP request timeout
     */
    BT_BIP_REQ_TIMER,
    BT_BIP_BLINKING_TIMER,
    BT_BIP_FS_TIMER,

    /*
     * for BT HID user confirm timeout
     */
    BT_HID_USER_CNF_TIMER,

    /*
     * for profiels service
     */    
    SRV_PROF_STORE_CUR_TIMER,
    SRV_PROF_STORE_BUF_TIMER,
    
    /*
     * for audio player
     */
    AUDPLY_PROCESS_ADD_FILE_TIMER,
    AUDPLY_COUNT_PLAY_TIMER,
    AUDPLY_DELAY_PLAY_TIMER,
    AUDPLY_SINGLE_PLAY_TIMER,
    AUDPLY_SEEKING_TIMER,
    AUDPLY_STATUS_ICON_TIMER,
    
    /*
     * for media player
     */
    MEDPLY_WAIT_NEXT_TIMER,
    MEDPLY_DELAY_INPUT_TIMER,
    MEDPLY_SINGLE_WAIT_NEXT_TIMER,
    MEDPLY_DELAY_PLST_POPUP_TIMER,
    MEDPLY_UPDATE_TIME_TIMER,
    MEDPLY_DELAY_ENTER_STATE_TIMER,
    MEDPLY_STREAM_BUFFER_TIMER,
    MEDPLY_PDL_BUFFER_TIMER,
    MEDPLY_PDL_CALC_DOWNLOAD_TIMER,
    MEDPLY_TOUCH_AUTO_RUN_TIMER,
    MEDPLY_STATUS_ICON_TIMER,
    MEDPLY_INTRO_PLAY_TIMER,
    MEDPLY_WAIT_DATACOUNT_CHECK,
    MEDPLY_GENERATE_TIMER,
    MEDPLY_STATE_MACHINE_TIMER,
    MEDPLY_TOUCH_LONG_PRESS_TIMER,
    MEDPLY_WIDGET_CALLBACK_TIMER,
    MEDPLY_KURO_TASK_TIMER,
    MEDPLY_KURO_GPRS_TIMER,
    MEDPLY_KURO_SMS_TIMER,
    MEDPLY_KURO_GET_TOTAL_FEE_TIMER,
    MEDPLY_KURO_DOWNLOAD_TIMER,
    MEDPLY_KURO_UPDATE_DOWNLOAD_MANAGER_TIMER,
    MEDPLY_KURO_SMS_SHORT_TIMER,
    MEDPLY_KURO_SCAN_TIMER,
    
    /*
     * for AB repeater
     */
    ABREPEAT_PROGRESS_BAR_DRAW_TIMER,
    ABREPEAT_BUILD_CACHE_DELAY_TIMER,
    ABREPEAT_DUMMY_FILE_PROG_TIMER,

    /*
     * for mdi_audio_play_text
     */
    MDI_AUDIO_PLAYER_TEXT_TIMER,
    MDI_AUDIO_CRESCENDO_TIMER,
    
    /*
     * for fm radio
     */
    FMRDO_TWINKLE_FREQUENCY_TIMER,
    FMRDO_REPEAT_STEP_TIMER,
    FMRDO_VOLUME_DISPLAY_TMER,

    /*
     * for sound recorder SMS record
     */
    SND_SMS_RECORD_STOP_TIMER,
    
    /*
     * for MMIAPI
     */
    MMIAPI_SMS_TIMER,

    /*
     * for GPIO
     */
    GPIO_KEYPAD_LIGHT_TIMER,

    /*
     * for WAP
     */
    WAP_NOT_READY_TIMER,
    WAP_GOTO_IDLE_TIMER,
    WAP_EDITOR_MESSAGEBOX_TIMER,

    /*
     * for J2ME
     */
    J2ME_TIMEALARM_TIMER,
    J2ME_PUSHINSTALL_TIMER,
    J2ME_DMINSTALL_TIMER,
    J2ME_NOT_READY_TIMER,
    J2ME_VM_TERMINATING_TIMER,
    J2ME_VM_EVENT_TIMER,

    /*
     * for DownloadAgent
     */
    MMI_DA_TIMER_UPDATE_STATUS,
    MMI_DA_WPS_TIMER_0,
    MMI_DA_WPS_TIMER_END = MMI_DA_WPS_TIMER_0 + 10,
    MMI_DA_TIMER_TRIGGERED_PROC_TIMER_BASE,
    MMI_DA_TIMER_TRIGGERED_PROC_TIMER_END = MMI_DA_TIMER_TRIGGERED_PROC_TIMER_BASE + 5,
    
    /*
     * for VRSD
     */
    VRSD_DIAL_AUTO_DIAL_TIMER,
    VRSD_DIAL_PLAYBACK_TIMER,
    VRSD_PLAYTONE_TIMER,

    /*
     * for touch screen
     */
    PEN_POLLING_TIMER,

    /*
     * for GDI,( allocate 128 timer id )
     */
    GDI_TIMER_BASE,
    GDI_TIMER_END = GDI_TIMER_BASE + 128,

    /*
     * for SVG
     */
    SVG_TIMER_BASE,

    /* micha_PoC */
    /*
     * for PoC
     */
    POC_IND_TIMER,

    /*
     * for Shcedule FM Recording
     */
    FMSR_CHCEK_STOP_TIMER,

    /*
     * for calling background sound
     */
    BGSND_HIGHLIGHT_TIMER,
    BGSND_INTERVAL_TIMER,

    /*
     * for Jataayu WAP and MMS
     */
    JDD_TIMER_00,
    JDD_TIMER_01,
    JDD_TIMER_02,
    JDD_TIMER_03,
    JDD_TIMER_04,
    JBROWSER_WAP_NOT_READY_TIMER,
    JMMS_WAP_NOT_READY_TIMER,
    JMMS_NO_COMM_TIMER_CHECK,
    JMMS_VIEW_PREVIEW_TIMER,
	JMMS_ASYNC_PARSE_TIMER,
	JMMS_ASYNC_SEND_TIMER,
	JMMS_ASYNC_SAVE_TIMER,
    JMMS_SEND_TIMEOUT_TIMER,

	/* For Generic Browser */
	BRW_SPLASH_SCREEN_TIMER,
	BRW_SERVER_CONNECTION_TIMER,
#ifdef OBIGO_Q05A
	BRW_LOADING_TIME_CHECK_TIMER,
#endif
#ifdef __MMI_BRW_FILE_UPLOAD_SUPPORT__
	BRW_UPLOAD_COMPLETED_TIMER,
#endif
	/* For Generic Browser */
    /*
     * for Unified Message
     */
    UM_INPROGRESS_TIMER_ID,
    EBOOK_RECURSIVE_TIMER,
   	UMMS_SEND_TIMER,
   	UMMS_NOTIFY_TIMER,
	UMMS_DOWNLOAD_TIMER,
	UMMS_INPROGRESS_TIMER_ID,
    MMS_VIEWER_TIMER,
    SRV_MMS_BGSR_TIMER,    /*Add for BGSR*/
    UMMS_SETTINGS_TIMER,
    /*
     * AV BT app
     */ 
    AV_BT_DISC_TIMER,
    AV_BT_START_TIMER,
    AV_BT_DELAY_PAUSE_TIMER,
    BT_AVRCP_AUTO_RELEASE_TIMER,
    BT_AVRCP_TEST_TIMER,

    /*
     * for SyncML
     */
    SYNCML_POWER_ON_SYNC_TIMER,
    SYNCML_DATA_CHANGE_DELAY_TIMER,

    /*
     * for VoIP
     */
    VOIP_AUTOLOGIN_TIMER,
    VOIP_AUTOANSWER_TIMER,
    VOIP_AUTOREDIAL_TIMER,
    VOIP_DTMF_TIMER,
    VOIP_POPUP_TIMER,

    /*
     * for UCM
     */
    UCM_NOTIFY_DURATION_TIMER,
    UCM_CONFIRM_TIMER_ID,
    UCM_AUTO_REDIAL_TIMER,
    UCM_AUTO_ANSWER_TIMER,
    UCM_SEND_DTMF_TIMER,
    UCM_AUTO_ANSWER_MACHINE_TIMER,
    UCM_AUTO_ANSWER_MACHINE_END_TIMER,

    UCM_FTE_AUTO_LOCK_TIMER_ID,
    UCM_VT_FALL_TO_VOICE_NOTIFY_TIMER_ID,
    /*
     * for VT
     */
    VT_START_CAMERA_QUERY_TIMER,

    /*
     * for Unified Composer
     */
    UC_INPROGRESS_TIMER_ID,
    UC_MSG_TYPE_CHECK_TIMER_ID,
    UM_BATTERY_LOW_CHECK_TIMER_ID,
    UC_IMG_RESIZE_TIMER_ID,

    UC_CREATE_RSP_RETRY_TIMER_ID,   
    /*
     * for Audio via SCO
     */
    BT_SCO_CONNECT_TIMER,
    BT_SCO_ENABLE_CONNECT_DELAY_TIMER,
    /*
    * for Ebook
    */
    EBR_SEARCH_BLOCK_TIMER,
    EBOOK_RESTORE_DEFAULT_SETTING_TIMER,
    EBOOK_REFRESH_BOOK_SHELF_TIMER,
    EBOOK_FTE_REFRESH_OPEN_BOOK_TIMER,
    
    /*
     * Avatar
     */
    AVATAR_DELAY_DECODE_0,
    AVATAR_DELAY_DECODE_1,
    AVATAR_DELAY_DECODE_2,
    AVATAR_DELAY_DECODE_3,
    AVATAR_DELAY_DECODE_4,


    AVATAR_DELAY_DECODE_5,
    AVATAR_DELAY_DECODE_6,
    AVATAR_DELAY_DECODE_7,
    AVATAR_DELAY_DECODE_8,
    AVATAR_DELAY_DECODE_9,

    /* for Q05A Browser */
	BRW_PSH_INIT_TIMER,

    /*
     * for owl sound integration
     */
    OWL_SND_VIB_TIMER,

    /*
     * for Category screen browser
     */
    CSB_AUTO_MODE_TIMER_ID,
    /*
    * for dictionary
    */
    DICTIONARY_TTS_TIMER,

	/*
	 *for DM Self-Register
	 */
	DM_SR_REPEAT_TIMER_ID,
	DM_SR_IMSI_REPEAT_TIMER_ID,
	DM_SR_SIM2_IMSI_REPEAT_TIMER_ID,

    /*
     * for Language Learning
     */
     LANGLN_UPDATE_TIMER_ID,
     LANGLN_DK_PLAY_DELAY_TIME,
     LANGLN_SK_PLAY_DELAY_TIME,
     LANGLN_LK_PLAYING_TIMER,
     LANGLN_LK_INTERVAL_TIMER,
     LANGLN_WK_ALERT_TOP_TIMER_ID,
	 
    /*
	 * for DCD
	 */
	 DCD_TIMER,
	 DCD_RETRY_TIMER,
	 DCD_TTL_TIMER,
	 
    /*
     * EM benchmark
     */
     BENCHMARK_TIMER_ID,

	 /*Certificate Manger */
	 MMI_CERTMAN_TIMER_ID,
#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
    /*
     * GIS
     */
     GIS_TIMER_ID_0,
     GIS_TIMER_ID_1,
     GIS_TIMER_ID_2,
     GIS_TIMER_ID_3,
     GIS_TIMER_ID_4,
     GIS_TIMER_ID_5,
     GIS_TIMER_ID_6,
     GIS_TIMER_ID_7,
     GIS_TIMER_ID_8,
     GIS_TIMER_ID_9,
     GIS_TIMER_ID_10,
     GIS_TIMER_ID_11,
     GIS_TIMER_ID_12,
     GIS_TIMER_ID_13,
     GIS_TIMER_ID_14,
     GIS_TIMER_ID_15,
     GIS_TIMER_ID_16,
     GIS_TIMER_ID_17,
     GIS_TIMER_ID_18,
     GIS_TIMER_ID_19,
     
        SWTR_START_TIMER,
        SWTR_DURATION_TIMER,

/*
 * for dual mode support
 * MERCURY MASTER USE
 */
#ifdef __MMI_DUAL_SIM_MASTER__
    MTPNP_RELEASE_SLAVE_PWRKEY_TIMER,
    MTPNP_RESET_SLAVE_TIMER,
    MTPNP_MODESWITCH_TIMER,
    MTPNP_AUTOANSWER_NOTIFYDURATION_TIMER,
    MTPNP_NOTIFYDURATION_TIMER,
    MTPNP_NOTIFY_INCOMING_CONFLICT,
    MTPNP_ALERT_NOTIFYDURATION_TIMER,
    MTPNP_DYNAMIC_FM_RETRY_TIMER,
    MTPNP_SWITCHMODE_DEACTIVATE_BT_TIMER,
#endif	/* __MMI_DUAL_SIM_MASTER__ */


     /*
     * GPS Management
     */
#if defined(__GPS_SUPPORT__) || defined(__BT_GPS_SUPPORT__)
     GPS_MGR_NOTIFY_TIMER_ID_1,
     GPS_MGR_NOTIFY_TIMER_ID_2,
     GPS_MGR_NOTIFY_TIMER_ID_3,
     GPS_MGR_NOTIFY_TIMER_ID_4,
     GPS_MGR_NOTIFY_TIMER_ID_5,
     GPS_MGR_SET_MODE_TIMER_ID,
     EM_GPS_CLOSE_TIMER_ID,
#endif

#if defined(__J2ME__)
     JUI_UPDATE_STATUS_ICON,
#endif

#if defined(__JAVA_MANAGER__)
     JAVA_MANAGER_TIMER_ID,
#endif
	/*
     * for Sound recorder
     */
    SNDREC_TIMER_FILELIST_SEEK,

    /* 
    *   for auto anwermachie
    */
    AUTOAM_TIMER_KEY_DETECT,

/* IMPS logo timer */
    IMPS_LOGO_TIMER,
    IMPS_TAB_TIMER,
    IMPS_AUTO_START_WAIT_TIMER,
    IMPS_CHECK_SYSTEM_MSG_TIMER,
    IMPS_POPUP_TIMER,
    IMPS_CHECK_PENDING_MSG_TIMER,

#ifdef __DRM_SUPPORT__
    RMGR_SCAN_DISK_TIMER,
#endif

    /* for settings */
    MMI_PHNSET_COPY_WAIT_TIMER,
    MMI_NETSET_3G_WAIT_TIMER,

#if defined(__MMI_ATV_SUPPORT__) || defined(__MMI_MOBILE_TV_CMMB_SUPPORT__)
    MTV_PLAYER_UT_TIMER,
#endif
#ifdef __MMI_ATV_SUPPORT__
    MTV_PLAYER_UPDATE_SIGNAL_TIMER,
    MTV_PLAYER_UPDATE_PLAYER_STATUS,
    MTV_PLAYER_SERVICE_TIMEOUT_TIMER,
    MTV_PLAYER_BT_ANIMATION_TIMER,
    MTV_PLAYER_VOLUME_ICON_TIMER,
    MTV_PLAYER_CHANNEL_LIST_DELAY_TIMER,
    MTV_PLAYER_UPDATE_SCAN_STATUS,
#endif /* __MMI_ATV_SUPPORT__ */

#ifdef __MMI_MOBILE_TV_CMMB_SUPPORT__
    DTV_PLAYER_UPDATE_SIGNAL_TIMER,
    DTV_PLAYER_UPDATE_PLAYER_STATUS,
    DTV_PLAYER_SERVICE_TIMEOUT_TIMER,
    DTV_PLAYER_BT_ANIMATION_TIMER,
    DTV_PLAYER_VOLUME_ICON_TIMER,
    DTV_PLAYER_CHANNEL_LIST_DELAY_TIMER,
    DTV_PLAYER_UPDATE_SCAN_STATUS,
    DTV_PLAYER_ESG_TIMEOUT_TIMER,
    DTV_PLAYER_DISPLAY_ACCESS_DENIED_TIMER,
    DTV_PLAYER_NW_STATUS,
#endif /* __MMI_MOBILE_TV_CMMB_SUPPORT__ */

#if defined (__FLAVOR_VENDOR_SDK__) || defined(__MAUI_SDK_TEST__)
    MMI_DEVAPP_TIMER_ID_0,
    MMI_DEVAPP_TIMER_ID_1,
    MMI_DEVAPP_TIMER_ID_2,
    MMI_DEVAPP_TIMER_ID_3,
    MMI_DEVAPP_TIMER_ID_4,
    MMI_DEVAPP_TIMER_ID_5,
    MMI_DEVAPP_TIMER_ID_6,
    MMI_DEVAPP_TIMER_ID_7,
    MMI_DEVAPP_TIMER_ID_8,
    MMI_DEVAPP_TIMER_ID_9,
#endif


#ifdef __MMI_FM_RADIO_SCHEDULER__
    FMRDO_SCHD_STOP_TIMER,
#endif 
#ifdef __UCWEB6__
    UCWEB_TIMER_BASE_ID,
    UCWEB_TIMER_END_ID = UCWEB_TIMER_BASE_ID + 5,
#endif
#ifdef __QQIM_SUPPORT__
	  QQ_TIMER,
#endif

#ifdef __A8BOX_SUPPORT__
    A8BOX_TIMDER_ID_0,
    A8BOX_TIMDER_ID_1,
    A8BOX_TIMDER_ID_2,
    A8BOX_TIMDER_ID_3,
    A8BOX_TIMDER_ID_4,
    A8BOX_TIMDER_ID_5,
    A8BOX_TIMDER_ID_6,
    A8BOX_TIMDER_ID_7,
    A8BOX_TIMDER_ID_8,
    A8BOX_TIMDER_ID_9,
#endif

    J2ME_LAUNCH_TIMER,

    /*
     * for Venus UI Engine
     */
#ifdef __MMI_VUI_ENGINE__
    VENUS_UI_TIMER_BASE,
    VENUS_UI_TIMER_END = VENUS_UI_TIMER_BASE + 128,
#endif

    /*
     * for Email MMI
     */
    EMAIL_MMI_CONT_PARSER_TIMER,
    EMAIL_MMI_EMN_NOTIFY_TIMER,
    /*
     * for Email auto check
     */
    EMAIL_MMI_AUTO_CHECK_TIMER_0,
    EMAIL_MMI_AUTO_CHECK_TIMER_1,
    EMAIL_MMI_AUTO_CHECK_TIMER_2,
    EMAIL_MMI_AUTO_CHECK_TIMER_3,
    EMAIL_MMI_AUTO_CHECK_TIMER_4,
    EMAIL_MMI_AUTO_CHECK_TIMER_5,
    EMAIL_MMI_AUTO_CHECK_TIMER_6,
    EMAIL_MMI_AUTO_CHECK_TIMER_7,
    EMAIL_MMI_AUTO_CHECK_TIMER_8,
    EMAIL_MMI_AUTO_CHECK_TIMER_9,

    /* for Vapp Email Mgr Job scheduler */
    VAPP_EMAIL_MGR_JOB_SCHEDULER,

#ifdef __QQMOVIE_SUPPORT__
    QQMOVIE_TIMER_ID_0,
	QQMOVIE_TIMER_ID_1,
	QQMOVIE_TIMER_ID_2,
	QQMOVIE_TIMER_ID_3,
	QQMOVIE_TIMER_ID_4,
	QQMOVIE_TIMER_ID_5,
	QQMOVIE_TIMER_ID_6,
	QQMOVIE_TIMER_ID_7,
	QQMOVIE_TIMER_ID_8,
	QQMOVIE_TIMER_ID_9,
#endif

    //For MMS V01 Agent
    MMSV01_AGENT_WAP_NOT_READY_TIMER_ID,
    MMSV01_AGENT_SEND_TIMER,
#ifdef __FM_RADIO_HW_SCAN__
    TIMER_MMI_FMRDO_CHECK_HW_SCAN,
#endif 

#if (!defined(HARDWARE_LED_NOT_SUPPORT))||defined(SIGNAL_LED_SUPPORT)
    /* phone setup led */
    PHONESET_LED_SWITCHING_TIMER,
#endif    
MRE_SYS_TIMER_0,
MRE_SYS_TIMER_1,
MRE_SYS_TIMER_2,
MRE_SYS_TIMER_3,
MRE_SYS_TIMER_4,
MRE_SYS_TIMER_5,
MRE_SYS_TIMER_6,
MRE_SYS_TIMER_7,
MRE_SYS_TIMER_8,
MRE_SYS_TIMER_9,
MRE_SYS_TIMER_10,
MRE_SYS_TIMER_11,
MRE_SYS_TIMER_12,
MRE_SYS_TIMER_13,
MRE_SYS_TIMER_14,
MRE_SYS_TIMER_15,
MRE_SYS_TIMER_16,
MRE_SYS_TIMER_17,
MRE_SYS_TIMER_18,
MRE_SYS_TIMER_19,
MRE_SYS_TIMER_20,
MRE_SYS_TIMER_21,
MRE_SYS_TIMER_22,
MRE_SYS_TIMER_23,
MRE_SYS_TIMER_24,
MRE_SYS_TIMER_25,
MRE_SYS_TIMER_26,
MRE_SYS_TIMER_27,
MRE_SYS_TIMER_28,
MRE_SYS_TIMER_29,
MRE_SYS_TIMER_30,
MRE_SYS_TIMER_31,
MRE_SYS_TIMER_32,
MRE_SYS_TIMER_33,
MRE_SYS_TIMER_34,
MRE_SYS_TIMER_35,
MRE_SYS_TIMER_36,
MRE_SYS_TIMER_37,
MRE_SYS_TIMER_38,
MRE_SYS_TIMER_39,
MRE_SYS_TIMER_40,
MRE_SYS_TIMER_41,
MRE_SYS_TIMER_42,
MRE_SYS_TIMER_43,
MRE_SYS_TIMER_44,
MRE_SYS_TIMER_45,
MRE_SYS_TIMER_46,
MRE_SYS_TIMER_47,
MRE_SYS_TIMER_48,
MRE_SYS_TIMER_49,
MRE_SYS_TIMER_50,
MRE_SYS_TIMER_51,
MRE_SYS_TIMER_52,
MRE_SYS_TIMER_53,
MRE_SYS_TIMER_54,
MRE_SYS_TIMER_55,
MRE_SYS_TIMER_56,
MRE_SYS_TIMER_57,
MRE_SYS_TIMER_58,
MRE_SYS_TIMER_59,
MRE_SYS_TIMER_60,
MRE_SYS_TIMER_61,
MRE_SYS_TIMER_62,
MRE_SYS_TIMER_63,
MRE_SYS_TIMER_64,
MRE_SYS_TIMER_65,
MRE_SYS_TIMER_66,
MRE_SYS_TIMER_67,
MRE_SYS_TIMER_68,
MRE_SYS_TIMER_69,
MRE_SYS_TIMER_70,
MRE_SYS_TIMER_71,
MRE_SYS_TIMER_72,
MRE_SYS_TIMER_73,
MRE_SYS_TIMER_74,
MRE_SYS_TIMER_75,
MRE_SYS_TIMER_76,
MRE_SYS_TIMER_77,
MRE_SYS_TIMER_78,
MRE_SYS_TIMER_79,
MRE_SYS_TIMER_80,
MRE_SYS_TIMER_81,
MRE_SYS_TIMER_82,
MRE_SYS_TIMER_83,
MRE_SYS_TIMER_84,
MRE_SYS_TIMER_85,
MRE_SYS_TIMER_86,
MRE_SYS_TIMER_87,
MRE_SYS_TIMER_88,
MRE_SYS_TIMER_89,
MRE_SYS_TIMER_90,
MRE_SYS_TIMER_91,
MRE_SYS_TIMER_92,
MRE_SYS_TIMER_93,
MRE_SYS_TIMER_94,
MRE_SYS_TIMER_95,
MRE_SYS_TIMER_96,
MRE_SYS_TIMER_97,
MRE_SYS_TIMER_98,
MRE_SYS_TIMER_99,

#ifdef OPERA_BROWSER
    OPERA_TIMER_ID_1,
    OPERA_TIMER_ID_2,
    OPERA_TIMER_ID_3,
    OPERA_TIMER_ID_4,
#endif

#ifdef __NMC_SUPPORT__
    NMC_TIMER,
#endif
    VS_FW_CUI_MENU_TIMER_1,
    VS_SDK_SCENARIO,
#ifdef __MMI_OP01_WIFI__
    WIFI_AUTO_SEARCH_TIMER,
#endif

#ifdef __WPS_SUPPORT__
    WIFI_WPS_CONNECT_TIMER,
#endif

    CAT9010_TIMER,

#if 0
#ifdef __MMI_TETHERING__
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif /* __MMI_TETHERING__ */
#endif
#if defined(__ALIPAY_SUPPORT__)
    ALIPAY_TIMER_ID0,
    ALIPAY_TIMER_ID1,
    ALIPAY_TIMER_ID2,
    ALIPAY_TIMER_ID3,
    ALIPAY_TIMER_ID4,
    ALIPAY_TIMER_ID5,
    ALIPAY_TIMER_ID6,
    ALIPAY_TIMER_ID7,
    ALIPAY_TIMER_ID8,
    ALIPAY_TIMER_ID9,
#endif /* __ALIPAY_MTK_SUPPORT__ */

    PXS_SENSOR_GET_RAW_DATA_TIMER,

#ifdef __COSMOS_MUSICPLY__
    COSMOS_MUSICPLY_TIMER_FIND_NEXT,
#endif

#ifdef __OP01_FWPBW__
    DIALER_PLAY_GSM_READY_TIMER,
    DIALER_PLAY_GSM_BUSY_TIMER,
    DIALER_PLAY_GSM_BUSY_DELAY_TIMER,
#endif

#ifdef __MMI_DSM_NEW__//11B1224²»ÐèÒªÒÆÖ²
    #include "..\mythroad\mythroadinc\mrp_app_timer_def.h"
    #ifdef __HC_CALL_NEW__
    HCCALL_SYS_TIMER,
    HCCALL_UI_TIMER,
    HCCALL_CALL_TIMER,
    #endif
    #ifdef __PME_SUPPORT__
    PME_TIMER,
    #endif
#endif/*__MMI_DSM_NEW__*/


    AVK_PLUTO_TIMER_TEST_BASE,
    AVK_PLUTO_TIMER_TEST_MAX = (AVK_PLUTO_TIMER_TEST_BASE + 9),

    MAX_TIMERS  /* Imp: Please do not modify this */
} MMI_TIMER_IDS;

#endif /* _PIXCOM_TIMER_EVENT_H */ 

