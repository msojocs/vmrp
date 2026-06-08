/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2010
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

/*******************************************************************************
 * Filename:
 * ---------
 *   cam_msg_handler.c
 *
 * Project:
 * --------
 *   Maui
 *
 * Description:
 * ------------
 *   This file includes message handle functions of image module.
 *
 * Author:
 * -------
 * -------
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by P4 VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 04 08 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 28 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 28 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 18 2011 vincent.cho
 * removed!
 * .
 *
 * 03 18 2011 vincent.cho
 * removed!
 * .
 *
 * 03 16 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 11 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 08 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 04 2011 cathytr.huang
 * removed!
 * Make power off be a blocking function.
 *
 * 03 03 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 03 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 03 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 02 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 02 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 02 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 02 2011 cathytr.huang
 * removed!
 * .
 *
 * 03 02 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 24 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 24 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 23 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 23 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 23 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 23 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 22 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 18 2011 cathytr.huang
 * removed!
 * Remove chip compile option.
 *
 * 02 09 2011 cathytr.huang
 * removed!
 * Panorama revises pause/resume.
 *
 * 02 09 2011 cathytr.huang
 * removed!
 * Change structure naming.
 *
 * 02 09 2011 cathytr.huang
 * removed!
 * .
 *
 * 02 08 2011 cathytr.huang
 * removed!
 * Add cam get buffer content operation.
 *
 * 02 08 2011 cathytr.huang
 * removed!
 * Change thumbnail color format.
 *
 * 01 28 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 27 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 26 2011 cathytr.huang
 * removed!
 * Revise memory check handling.
 *
 * 01 24 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 24 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 21 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 20 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 19 2011 cathytr.huang
 * removed!
 * Add error handling.
 *
 * 01 14 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 12 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 12 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 11 2011 cathytr.huang
 * removed!
 * 
 * .
 *
 * 01 10 2011 cathytr.huang
 * removed!
 * .
 *
 * 01 03 2011 cathytr.huang
 * removed!
 * .
 *
 * 12 29 2010 cathytr.huang
 * removed!
 * Panorama revise.
 *
 * 12 28 2010 cathytr.huang
 * removed!
 * .
 *
 * 12 23 2010 cathytr.huang
 * removed!
 * .
 *
 * 12 22 2010 cathytr.huang
 * removed!
 * .
 *
 * 12 21 2010 cathytr.huang
 * removed!
 * Add LCM scan line code.
 *
 * 12 16 2010 cathytr.huang
 * removed!
 * Add feature compile option.
 *
 * 12 15 2010 cathytr.huang
 * removed!
 * .
 *
 * 12 15 2010 cathytr.huang
 * removed!
 * Add UI rotate handling.
 *
 * 12 15 2010 cathytr.huang
 * removed!
 * Fix panorama IT issue.
 *
 * 12 14 2010 cathytr.huang
 * removed!
 * Revise code.
 *
 * 12 14 2010 cathytr.huang
 * removed!
 * Add panorama new API and revise profiling.
 *
 * 12 12 2010 cathytr.huang
 * removed!
 * Buffer overlay.
 *
 * 12 08 2010 cathytr.huang
 * removed!
 * Revise panorama.
 *
 * 12 02 2010 cathytr.huang
 * removed!
 * Fix build error.
 *
 * 12 01 2010 cathytr.huang
 * removed!
 * Panorama revise.
 *
 * 12 01 2010 cathytr.huang
 * removed!
 * DDV2 element check in.
 *
 * 11 30 2010 cathytr.huang
 * removed!
 * .
 *
 * 11 30 2010 cathytr.huang
 * removed!
 * Panorama and HW layer revision.
 *
 * 11 30 2010 cathytr.huang
 * NULL
 * Panorama check in & HW layer revise.
 *
 * 11 23 2010 cathytr.huang
 * NULL
 * Revise MEM CAM memory.
 *
 * 11 22 2010 cathytr.huang
 * NULL
 * Fix build error.
 *
 * 11 21 2010 cathytr.huang
 * NULL
 * New memory macro check in.
 *
 * 11 18 2010 cathytr.huang
 * NULL
 * Fix MT6276 capture issue.
 *
 * 11 17 2010 cathytr.huang
 * NULL
 * Remove jpeg header.
 *
 * 11 17 2010 cathytr.huang
 * NULL
 * MED CAM FD revise code.
 *
 * 11 11 2010 bin.han
 * NULL
 * .Check in capture profiling
 *
 * 11 11 2010 cathytr.huang
 * NULL
 * Remove the old ISP API from MED CAM.
 *
 * 11 10 2010 cathytr.huang
 * NULL
 * Fix continue shot issue .
 *
 * 11 10 2010 cathytr.huang
 * NULL
 * Revise code.
 *
 * 11 10 2010 cathytr.huang
 * NULL
 * Revise FD external memory.
 *
 * 11 08 2010 cathytr.huang
 * NULL
 * Add MT6276/6255 raw capture buffer.
 *
 * 11 08 2010 cathytr.huang
 * NULL
 * Revise sensor query function.
 *
 * 11 05 2010 cathytr.huang
 * NULL
 * Add capture with thumbnail and get AF zone.
 *
 * 11 04 2010 cathytr.huang
 * NULL
 * Revise code.
 *
 * 11 04 2010 cathytr.huang
 * NULL
 * Revise code.
 *
 * 11 03 2010 cathytr.huang
 * NULL
 * Fix MED CAM BSS and continue shot.
 *
 * 11 02 2010 cathytr.huang
 * NULL
 * Camcorder adds HW layer usage for making tearing free.
 *
 * 11 02 2010 cathytr.huang
 * NULL
 * HW layer revise and enlarge memory size for concurrency.
 *
 * 11 01 2010 cathytr.huang
 * NULL
 * Remove some redundant codes .
 *
 * 11 01 2010 cathytr.huang
 * NULL
 * Fix BSS buffer issue.
 *
 * 10 31 2010 cathytr.huang
 * NULL
 * Fix Burstshot capture.
 *
 * 10 29 2010 cathytr.huang
 * NULL
 * Modify MED CAM BSS code.
 *
 * 10 29 2010 cathytr.huang
 * NULL
 * .
 *
 * 10 28 2010 cathytr.huang
 * NULL
 * Fix zoom issue and FSAL handle.
 *
 * 10 27 2010 cathytr.huang
 * NULL
 * Fix MED camera capture timeout error.
 *
 * 10 26 2010 cathytr.huang
 * NULL
 * Fix "fail to capture" issue.
 *
 * 10 26 2010 cathytr.huang
 * NULL
 * .
 *
 * 10 25 2010 cathytr.huang
 * NULL
 * According to sensor rotate value, to decide CAL preview rotate value.
 *
 * 10 25 2010 cathytr.huang
 * NULL
 * Fix preview issue
 *
 * 10 23 2010 cathytr.huang
 * NULL
 * MED CAM check in
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by P4 VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifndef MED_NOT_PRESENT

/*==== INCLUDES =========*/
/* system includes */
#include "kal_release.h"
#include "kal_trace.h"

#include "app_ltlcom.h" /* Task message communiction */

#ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__
#include "FSAL.h"
#endif
#include "cache_sw.h"
#include "mmu.h"

/* local includes */
#include "med_global.h"
#include "med_main.h"
#include "med_struct.h"
#include "med_api.h"
#include "med_context.h"
#include "med_smalloc.h"
#include "med_utility.h"
#include "cam_main.h"
#include "cam_v_main.h"
#include "cal_comm_def.h"
#include "fd_comm_def.h"        // face detection common define header file
#include "sd_comm_def.h"        // smile detection common define header file

#ifdef __MED_TVO_MOD__
#include "tv_out.h"
#include "tvo_main.h"
#endif /* __MED_TVO_MOD__ */

#ifdef __CAM_TRACE_ON__
#include "med_trc.h"
#endif

#include "kal_general_types.h"
#include "kal_public_api.h"
#include "cal_api.h"
#include "fs_type.h"
#include "fs_func.h"
#include "fs_errcode.h"
#include "kal_public_defs.h"
#include "mm_comm_def.h"
#include "string.h"
#include "pp_comm_def.h"
#include "cal_mem_def.h"
#include "pp_mem_def.h"

#ifdef __MED_CAM_MOD__

/**************************************************************************************************
 * External Functions Declaration
 **************************************************************************************************/
#if defined(__MM_DEBUG_MEASURE__)
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#else /* defined(__MM_DEBUG_MEASURE__) */
#define MED_CAM_START_LOGGING(_symbol_)
#define MED_CAM_STOP_LOGGING(_symbol_)
#endif /* defined(__MM_DEBUG_MEASURE__) */

#if defined(__MED_MEM_DEBUG_ON__) && defined(__MTK_TARGET__) && !defined(WIN32)
kal_uint32 g_u32_cam_start_time;
kal_uint32 g_u32_cam_end_time;
kal_uint32 g_u32_cam_duration;
#define MED_CAM_GET_START_TIME(_trace_) \
        do{\
            kal_get_time(&g_u32_cam_start_time);\
        }while(0)
#define MED_CAM_GET_STOP_TIME_AND_PRINT_TRACE(_trace_)\
        do{\
            kal_get_time(&g_u32_cam_end_time);\
            g_u32_cam_duration = kal_ticks_to_milli_secs(g_u32_cam_end_time - g_u32_cam_start_time);\
            kal_trace(TRACE_GROUP_2,_trace_, g_u32_cam_duration,__LINE__);\
        }while(0)
#else /* defined(__MED_MEM_DEBUG_ON__) && defined(__MTK_TARGET__) && !defined(WIN32) */
#define MED_CAM_GET_START_TIME(_trace_)
#define MED_CAM_GET_STOP_TIME_AND_PRINT_TRACE(_trace_) 
#endif /* defined(__MED_MEM_DEBUG_ON__) && defined(__MTK_TARGET__) && !defined(WIN32) */

#if defined(SENSOR_ON_BODY)
#define CAM_MAIN_SENSOR_MIRROR  (0) // 1 : need to mirror
#elif defined(SENSOR_ON_COVER)
#define CAM_MAIN_SENSOR_MIRROR  (1) // 1 : need to mirror
#else
#define CAM_MAIN_SENSOR_MIRROR  (0)
#endif

#define CAM_SUB_SENSOR_MIRROR   (1)

#if defined(__MAIN_LCM_SCANLINE_ROTATION_0__)
#define MAIN_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_0)
#elif defined(__MAIN_LCM_SCANLINE_ROTATION_90__)
#define MAIN_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_270)
#elif defined(__MAIN_LCM_SCANLINE_ROTATION_180__)
#define MAIN_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_180)
#elif defined(__MAIN_LCM_SCANLINE_ROTATION_270__)
#define MAIN_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_90)
#else
#define MAIN_LCM_CROSS_SENSOR_ANGLE  (0)
#endif

#if defined(__SUB_LCM_SCANLINE_ROTATION_0__)
#define SUB_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_0)
#elif defined(__SUB_LCM_SCANLINE_ROTATION_90__)
#define SUB_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_270)
#elif defined(__SUB_LCM_SCANLINE_ROTATION_180__)
#define SUB_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_180)
#elif defined(__SUB_LCM_SCANLINE_ROTATION_270__)
#define SUB_LCM_CROSS_SENSOR_ANGLE  (MM_IMAGE_ROTATE_90)
#else
#define SUB_LCM_CROSS_SENSOR_ANGLE  (0)
#endif

#if defined(SENSOR_ROTATE_0)
#define CAM_MAIN_SENSOR_ROTATE  (MM_IMAGE_ROTATE_0)
#elif defined(SENSOR_ROTATE_90)
#define CAM_MAIN_SENSOR_ROTATE  (MM_IMAGE_ROTATE_90)
#elif defined(SENSOR_ROTATE_180)
#define CAM_MAIN_SENSOR_ROTATE  (MM_IMAGE_ROTATE_180)
#elif defined(SENSOR_ROTATE_270)
#define CAM_MAIN_SENSOR_ROTATE  (MM_IMAGE_ROTATE_270)
#else
#define CAM_MAIN_SENSOR_ROTATE  (0)
#endif

#if defined(SUB_SENSOR_ROTATE_0)
#define CAM_SUB_SENSOR_ROTATE  (MM_IMAGE_ROTATE_0)
#elif defined(SUB_SENSOR_ROTATE_90)
#define CAM_SUB_SENSOR_ROTATE  (MM_IMAGE_ROTATE_90)
#elif defined(SUB_SENSOR_ROTATE_180)
#define CAM_SUB_SENSOR_ROTATE  (MM_IMAGE_ROTATE_180)
#elif defined(SUB_SENSOR_ROTATE_270)
#define CAM_SUB_SENSOR_ROTATE  (MM_IMAGE_ROTATE_270)
#else
#define CAM_SUB_SENSOR_ROTATE  (0)
#endif

#define CAM_ABS(val) (((val) < 0) ? -(val) : (val))

void cam_free_working_buffer(void);
void cam_capture_still_image_stop(void);
static void cam_panorama_start_stitch(kal_uint32 working_buffer_address, kal_uint32 working_buffer_size);
static void cam_panorama_stop_stitch(void);
static void cam_panorama_stop_bg_stitch(void);
/**************************************************************************************************
 * MACRO Definition
 **************************************************************************************************/
#define CAM_FREE_CAPTURE_BUFFER()do{\
                                    if (cam_context_p->capture_buffer_p)\
                                    {\
                                        med_free_asm_mem(cam_context_p->app_id, (void**)&cam_context_p->capture_buffer_p);\
                                        cam_context_p->capture_buffer_size = 0;\
                                    }\
                                }while(0)

                                
#define CAM_FREE_STITCH_BUFFER()   do{\
                                    if (cam_context_p->stitch_buffer_address)\
                                    {\
                                        med_free_asm_mem(cam_context_p->app_id, (void**)&cam_context_p->stitch_buffer_address);\
                                        cam_context_p->stitch_buffer_size = 0;\
                                    }\
                                }while(0)


#define CAM_PREVIEW_CHECK_IS_BUFFER_AVAILABLE(pointer)   do{\
                                    if (pointer == NULL)\
                                    {\
                                        CAM_FREE_CAPTURE_BUFFER();\
                                        cam_free_working_buffer();\
                                        cam_set_result(MED_RES_MEM_INSUFFICIENT);\
                                        CAM_SET_EVENT(CAM_EVT_PREVIEW);\
                                        return;\
                                    }\
                                }while(0)

#define CAM_CAPTURE_CHECK_IS_BUFFER_AVAILABLE(pointer)   do{\
                                    if (pointer == NULL)\
                                    {\
                                        CAM_FREE_CAPTURE_BUFFER();\
                                        cam_free_working_buffer();\
                                        cam_set_result(MED_RES_MEM_INSUFFICIENT);\
                                        CAM_SET_EVENT(CAM_EVT_CAPTURE);\
                                        return;\
                                    }\
                                }while(0)
                                
#define CAM_CAPTURE_TIMEOUT_DURATION (5000)
#define CAM_FD_PROCESS_TIMEOUT_DURATION (20)
/**************************************************************************************************
 * Static Variables
 **************************************************************************************************/


/**************************************************************************************************
 * Static Functions Declaration
 **************************************************************************************************/


void static cam_util_fit_box(
                kal_int32 bbox_width,
                kal_int32 bbox_height,
                kal_int32 src_width,
                kal_int32 src_height,
                kal_int32 *offset_x,
                kal_int32 *offset_y,
                kal_int32 *dest_width,
                kal_int32 *dest_height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (src_width == 0 || src_height == 0 || bbox_width == 0 || bbox_height == 0)
    {
        *dest_width = 0;
        *dest_height = 0;
    }
    else if (src_width * bbox_height > src_height * bbox_width)
    {
        *dest_width = bbox_width;
        *dest_height = src_height * bbox_width / src_width;
    }
    else
    {
        *dest_width = src_width * bbox_height / src_height;
        *dest_height = bbox_height;
    }

    /* non-ratio resize if different is small. */
    {
        int diff;

        diff = CAM_ABS(*dest_height - bbox_height);
        if (diff <= 2 && *dest_width == bbox_width)
        {
            *dest_height = bbox_height;
        }

        diff = CAM_ABS(*dest_width - bbox_width);
        if (diff <= 2 && *dest_height == bbox_height)
        {
            *dest_width = bbox_width;
        }
    }

    *offset_x = (bbox_width - *dest_width) / 2;
    *offset_y = (bbox_height - *dest_height) / 2;

    if(*dest_width==0) *dest_width = 1;
    if(*dest_height==0) *dest_height = 1;

}


/*****************************************************************************
 * FUNCTION
 *  cam_extmem_switch_cachable_region
 * DESCRIPTION
 *  Switch cachable region
 * PARAMETERS
 *  mem_ptr     [OUT]
 *  size        [IN]
 *  b_cachable  [IN]
 * RETURNS
 *  void
 *****************************************************************************/
kal_bool cam_extmem_switch_cachable_region(kal_uint32* mem_ptr, kal_uint32 size, kal_bool is_cacheable)
{
#if defined(__MTK_TARGET__) && defined(__DYNAMIC_SWITCH_CACHEABILITY__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_bool ret = KAL_FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if (is_cacheable == KAL_FALSE && INT_QueryIsCachedRAM(*mem_ptr, size) == KAL_TRUE)
    {
        dynamic_switch_cacheable_region((void*)mem_ptr, size, PAGE_NO_CACHE);
        ret = KAL_TRUE;
    }
    else
    {
        if (INT_QueryIsNonCachedRAM(*mem_ptr, size) == KAL_TRUE)
        {
            dynamic_switch_cacheable_region((void*)mem_ptr, size, PAGE_CACHEABLE);
            ret = KAL_TRUE;
        }
    }

    return ret;
#else
    return 0;
#endif /* (defined(MT6235) || defined(MT6235B)) && defined(__MTK_TARGET__) && defined(__ARM9_MMU__) */    
}


/*****************************************************************************
 * FUNCTION
 *  cam_alloc_capture_buffer
 * DESCRIPTION
 *  This function is to allocate capture buffer.
 * PARAMETERS
 *  size        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_alloc_capture_buffer(kal_uint16 app_id, kal_uint32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(!cam_context_p->capture_buffer_p); // should free capture buffer before allocating.

#if defined(__DYNAMIC_SWITCH_CACHEABILITY__)
    size = ((size + 31) >> 5) << 5;
#endif
    cam_context_p->capture_buffer_p = (kal_uint32) med_alloc_asm_mem_cacheable(app_id, size);
    cam_context_p->capture_buffer_size = size;

    cam_extmem_switch_cachable_region(&cam_context_p->capture_buffer_p, cam_context_p->capture_buffer_size, KAL_FALSE);

}


/*****************************************************************************
 * FUNCTION
 *  cam_allocate_memory_for_framebuffer
 * DESCRIPTION
 *  Release internal/external memory used in video module.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void cam_profiling_log(kal_uint32 stage)
{
#if defined(__MTK_INTERNAL__)
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
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
}


/*****************************************************************************
 * FUNCTION
 *  cam_open_image_file
 * DESCRIPTION
 *  This function is to open image file for capture.
 * PARAMETERS
 *  file_name       [IN]
 * RETURNS
 *  open result
 *****************************************************************************/
kal_int32 cam_open_image_file(kal_wchar *file_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 result;
#ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__
    FSAL_Status fsal_ret;
    kal_int32 fsal_error;
#endif    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check if the format valid */
    /* open the file */
#ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__    
    fsal_ret = FSAL_Open(&cam_context_p->fsal_file, (void*)file_name, FSAL_WRITE);
    if (fsal_ret != FSAL_OK)
    {
        fsal_error = FSAL_GetLastError(&cam_context_p->fsal_file);
        if (fsal_error == FS_DRIVE_NOT_FOUND)
        {
            return MED_RES_NO_DISC;
        }
        else if (fsal_error == FS_DISK_FULL)
        {
            return MED_RES_DISC_FULL;
        }
        else
        {
            return MED_RES_OPEN_FILE_FAIL;
        }
    }
        
    FSAL_SetBuffer(&cam_context_p->fsal_file, MAX_JPEG_ENCODE_MARGIN_LEN, (kal_uint8 *)cam_context_p->capture_buffer_p);
#else
    cam_context_p->file_handle = FS_Open((kal_wchar*) file_name, FS_CREATE | FS_READ_WRITE);

    if (cam_context_p->file_handle >= 0)
    {
        /* Check free memory space first */
        if ((result = med_check_disc_space(file_name, CAPTURE_MEM_MARGIN)) != MED_RES_OK)
        {
            FS_Close(cam_context_p->file_handle);
            FS_Delete((kal_wchar*) file_name);
            return result;
        }           
    }
    else if (cam_context_p->file_handle == FS_WRITE_PROTECTION)
    {
        return MED_RES_WRITE_PROTECTION;
    }
    else if ((cam_context_p->file_handle == FS_ROOT_DIR_FULL) || (cam_context_p->file_handle == FS_DISK_FULL))
    {
        return MED_RES_DISC_FULL;
    }
    else
    {
        return MED_RES_OPEN_FILE_FAIL;
    }
#endif    
    return MED_RES_OK;
}


/*****************************************************************************
 * FUNCTION
 *  cam_close_image_file
 * DESCRIPTION
 *  This function is to close image file for capture.
 * PARAMETERS
 *  size        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
kal_int32 cam_close_image_file(kal_uint32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 result;
#ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__
    kal_int32 fsal_error;
    FSAL_Status fsal_ret;
#endif
    kal_uint32 len;
    kal_int32 res = MED_RES_OK;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__    
    result = (kal_int32)size;
    fsal_error = FSAL_GetLastError(&cam_context_p->fsal_file);       
    fsal_ret = FSAL_Close(&cam_context_p->fsal_file);
    
    if (!(result > 0) || fsal_ret == FSAL_CLOSE_ERROR)
    {
        FS_Delete((kal_wchar*) cam_context_p->file_name_p);

        if (fsal_ret == FSAL_CLOSE_ERROR)
        {
            fsal_error = FSAL_GetLastError(&cam_context_p->fsal_file);
            result = JPEG_SW_ENC_WRITE_FILE_FAILED;
        }
        
        if (result == JPEG_SW_ENC_OUT_OF_OUTPUT_BUFFER)
        {
            res = MED_RES_ERROR;
        }
        else if (result == JPEG_SW_ENC_WRITE_FILE_FAILED)
        {
            if (fsal_error == FS_DRIVE_NOT_FOUND)
            {
                res = MED_RES_NO_DISC;
            }
            else if (fsal_error == FS_DISK_FULL)
            {
                res = MED_RES_DISC_FULL;
            }
            else
            {
                res = MED_RES_OPEN_FILE_FAIL;
            }
        }
        else
        {
            res = MED_RES_OPEN_FILE_FAIL;
        }
    }
#else
    if (size > 0)
    {
        MED_CAM_GET_START_TIME(CAM_WRITE_TO_FILE_DURATION);
        MED_CAM_START_LOGGING("CFS");
        result = FS_Write(cam_context_p->file_handle, (void*)cam_context_p->jpeg_image_address, size, &len);
        MED_CAM_STOP_LOGGING("CFS");

        MED_CAM_START_LOGGING("CFC");
        FS_Close(cam_context_p->file_handle);
        MED_CAM_STOP_LOGGING("CFC");
        MED_CAM_GET_STOP_TIME_AND_PRINT_TRACE(CAM_WRITE_TO_FILE_DURATION);

        if (result != FS_NO_ERROR)
        {
            CAM_CLOSE_IMAGE_RESULT_TRACE(result);
            FS_Delete((kal_wchar*) cam_context_p->file_name_p);
            if (result == FS_DRIVE_NOT_FOUND)
            {
                res = MED_RES_NO_DISC;
            }
            else if (result == FS_DISK_FULL)
            {
                res = MED_RES_DISC_FULL;
            }
            else
            {
                res = MED_RES_OPEN_FILE_FAIL;
            }
        }
        else if (size != len)
        {
            FS_Delete((kal_wchar*) cam_context_p->file_name_p);
            res = MED_RES_DISC_FULL;
        }
    }
    else
    {
        /* Close and delete the file */
        FS_Close(cam_context_p->file_handle);
        FS_Delete((kal_wchar*) cam_context_p->file_name_p);
        res = MED_RES_DISC_FULL;
    }
#endif

    cam_profiling_log(CAL_CAMERA_PROFILING_JPEG_SAVE_FILE_DONE);

    return res;
}


/*****************************************************************************
 * FUNCTION
 *  cam_preview_callback
 * DESCRIPTION
 *  This function is to handle camera preview callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_preview_callback(kal_uint8 cause)
{

}

#if defined (ISP_SUPPORT)
void cam_capture_timeout_callback(void *args)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (CAM_IN_STATE(CAM_CAPTURE))
    {
        cam_capture_still_image_stop();
        cam_free_working_buffer();
        CAM_ENTER_STATE(CAM_READY);

        if (cam_context_p->snapshot_number == 1)
        {
            /* if single shot, use blocking mode */
            /* delete opened file handle and delete file */
            cam_close_image_file(0);
            cam_set_result(MED_RES_ERROR);
            CAM_SET_EVENT(CAM_EVT_CAPTURE);
        }
        else if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BEST_SHOT)
        {
            cam_context_p->capture_buffer_p = NULL;
            cam_context_p->app_capture_buffer_p = NULL;

            /* delete opened file handle and delete file */
            cam_close_image_file(0);

            /* if bestshot, use blocking mode */
            cam_set_result(MED_RES_ERROR);
            CAM_SET_EVENT(CAM_EVT_CAPTURE);
        }
        else
        {
            cam_send_capture_event_ind(MED_RES_ERROR, 0);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  cam_quickview_image_callback
 * DESCRIPTION
 *  This function is to handle camera quick view image ready callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_quickview_image_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_CAL_BUFFER_CB_STRUCT quickview_ptr;
    kal_uint8 result;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    quickview_ptr = (P_CAL_BUFFER_CB_STRUCT)pCallbackPara;
    cam_context_p->is_quickview_ready = KAL_TRUE;

    if (quickview_ptr->ErrorStatus == MM_ERROR_NONE)
        result = MED_RES_OK;
    else
        result = MED_RES_FAIL;

    cam_send_event_ind(CAM_EVT_QUICKVIEW, result);

}


/*****************************************************************************
 * FUNCTION
 *  cam_quickview_image_callback
 * DESCRIPTION
 *  This function is to handle camera thunbnail image ready callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_thumbnail_image_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_CAL_BUFFER_CB_STRUCT thumbnail_ptr;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    thumbnail_ptr = (P_CAL_BUFFER_CB_STRUCT)pCallbackPara;

    cam_context_p->thumbnail_image_address = thumbnail_ptr->ImageBuffAddr;
    cam_context_p->thumbnail_image_size    = thumbnail_ptr->ImageBuffSize;
    cam_context_p->is_thumbnail_ready      = KAL_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_image_reday_callback
 * DESCRIPTION
 *  This function is to handle camera main jpeg image ready callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_panorama_image_reday_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_CAL_BUFFER_CB_STRUCT image_ptr;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    image_ptr = (P_CAL_BUFFER_CB_STRUCT)pCallbackPara;
    
    if (image_ptr->ErrorStatus == MM_ERROR_NONE)
    {       
        cam_context_p->stitch_image_format = image_ptr->ImageBuffFormat;
        if (image_ptr->ImageBuffFormat == MM_IMAGE_FORMAT_JPEG)
        {
            cam_context_p->stitch_image_address = image_ptr->ImageBuffAddr;
            cam_context_p->stitch_image_size    = image_ptr->ImageBuffSize;
        }
        else
        {
            cam_context_p->stitch_image_address = image_ptr->ImageBuffYAddr;
            cam_context_p->stitch_image_size = 
                                    image_ptr->ImageBuffYSize + 
                                    image_ptr->ImageBuffUSize + 
                                    image_ptr->ImageBuffVSize;
        }
    }
    else
        cam_context_p->stitch_image_address = NULL;
    
    cam_context_p->captured_number++;
    
    cam_send_event_ind(CAM_EVT_PANO_IMG_READY, (kal_uint8)MED_RES_OK);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */    
}



/*****************************************************************************
 * FUNCTION
 *  cam_panorama_bg_stitch_ready_callback
 * DESCRIPTION
 *  This function is to handle camera main jpeg image ready callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_panorama_bg_stitch_ready_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->bg_stitch_count = (kal_uint32)pCallbackPara;

    cam_send_event_ind(CAM_EVT_PANO_BG_STITCH_READY, (kal_uint8)MED_RES_OK);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */    
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_stitch_ready_callback
 * DESCRIPTION
 *  This function is to handle camera main jpeg image ready callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_panorama_stitch_ready_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_CAL_PANO_RESULT_STRUCT jpeg_ptr;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    jpeg_ptr = (P_CAL_PANO_RESULT_STRUCT)pCallbackPara;
       
    cam_context_p->stitch_jpeg_file_address = jpeg_ptr->ImageBuffAddr;
    cam_context_p->stitch_jpeg_file_size    = jpeg_ptr->ImageBuffSize;
    cam_context_p->stitch_jpeg_file_width   = jpeg_ptr->ImgWidth;
    cam_context_p->stitch_jpeg_file_height  = jpeg_ptr->ImgHeight;

    if (jpeg_ptr->ErrorStatus == MM_ERROR_NONE)
        cam_context_p->stitch_result = MED_RES_OK;
    else
        cam_context_p->stitch_result = MED_RES_FAIL;
    
    cam_send_event_ind(CAM_EVT_PANO_STITCH_READY, (kal_uint8)MED_RES_OK);

#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */    
}


/*****************************************************************************
 * FUNCTION
 *  cam_main_jpeg_image_callback
 * DESCRIPTION
 *  This function is to handle camera main jpeg image ready callback.
 * PARAMETERS
 *  cause       [IN]
 *  ilm_struct *ilm_ptr(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_main_jpeg_image_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_CAL_BUFFER_CB_STRUCT jpeg_ptr;
    kal_uint8 result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    jpeg_ptr = (P_CAL_BUFFER_CB_STRUCT)pCallbackPara;

    cam_context_p->jpeg_image_address = jpeg_ptr->ImageBuffAddr;
    cam_context_p->jpeg_image_size    = jpeg_ptr->ImageBuffSize;

    cam_context_p->captured_number++;

    if (jpeg_ptr->ErrorStatus == MM_ERROR_NONE)
        result = MED_RES_OK;
    else
        result = MED_RES_FAIL;

    cam_send_event_ind(CAM_EVT_CAPTURE, (kal_uint8)result);
}


/*****************************************************************************
 * FUNCTION
 *  cam_focus_callback
 * DESCRIPTION
 *  This function is to handle camera focus callback.
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
void cam_focus_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_CAL_CAMERA_AF_RESULT_STRUCT pAfResult;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pAfResult = (P_CAL_CAMERA_AF_RESULT_STRUCT)pCallbackPara;

    kal_mem_cpy((void*)&cam_context_p->af_result, (void*)pAfResult, sizeof(CAL_CAMERA_AF_RESULT_STRUCT));
    
    cam_send_event_ind(CAM_EVT_AUTOFOCUS, (kal_uint8)pAfResult->AfSearchResult);
}


/*****************************************************************************
 * FUNCTION
 *  cam_fd_result_callback
 * DESCRIPTION
 *  CAL callback function
 * PARAMETERS
 * 
 * RETURNS
 *  void
 *****************************************************************************/
void cam_fd_result_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
#if defined(__MED_CAM_FD_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_FD_RESULT_STRUCT pFdRsult;
    kal_uint8 i = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pFdRsult = (P_FD_RESULT_STRUCT)pCallbackPara;

    memset(&cam_context_p->fd_result, 0, sizeof(FD_RESULT_STRUCT));

    cam_context_p->fd_result.face_no = pFdRsult->face_no;
    cam_context_p->fd_result.image_width = pFdRsult->image_width;
    cam_context_p->fd_result.image_height = pFdRsult->image_height;
    
    for (i = 0; i < cam_context_p->fd_result.face_no; i++)
    {
        cam_context_p->fd_result.rect_x0[i] = pFdRsult->rect_x0[i] * cam_context_p->preview_width / pFdRsult->image_width;
        cam_context_p->fd_result.rect_x1[i] = pFdRsult->rect_x1[i] * cam_context_p->preview_width / pFdRsult->image_width;
        cam_context_p->fd_result.rect_y0[i] = pFdRsult->rect_y0[i] * cam_context_p->preview_height / pFdRsult->image_height;
        cam_context_p->fd_result.rect_y1[i] = pFdRsult->rect_y1[i] * cam_context_p->preview_height / pFdRsult->image_height;
    }
    
#endif /* defined(__MED_CAM_FD_SUPPORT__) && (MT6225_SERIES) */
}


/*****************************************************************************
 * FUNCTION
 *  cam_fd_result_callback
 * DESCRIPTION
 *  CAL callback function
 * PARAMETERS
 * 
 * RETURNS
 *  void
 *****************************************************************************/
void cam_sd_result_callback(void *pCallbackPara, kal_uint16 CallbackParaLen)
{
#if defined(__MED_CAM_FD_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    P_SD_RESULT_STRUCT pFdRsult;
    kal_uint8 i = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pFdRsult = (P_SD_RESULT_STRUCT)pCallbackPara;

    kal_mem_cpy((void*)&cam_context_p->sd_result, (void*)pFdRsult, sizeof(SD_RESULT_STRUCT));

    for (i = 0; i < cam_context_p->sd_result.face_no; i++)
    {
        cam_context_p->sd_result.rect_x0[i] = cam_context_p->sd_result.rect_x0[i] * cam_context_p->preview_width / pFdRsult->image_width;
        cam_context_p->sd_result.rect_x1[i] = cam_context_p->sd_result.rect_x1[i] * cam_context_p->preview_width / pFdRsult->image_width;
        cam_context_p->sd_result.rect_y0[i] = cam_context_p->sd_result.rect_y0[i] * cam_context_p->preview_height / pFdRsult->image_height;
        cam_context_p->sd_result.rect_y1[i] = cam_context_p->sd_result.rect_y1[i] * cam_context_p->preview_height / pFdRsult->image_height;
    }

    for (i = 0; i < cam_context_p->sd_result.smile_detected_no; i++)
    {
        cam_context_p->sd_result.sd_rect_x0[i] = cam_context_p->sd_result.sd_rect_x0[i] * cam_context_p->preview_width / pFdRsult->image_width;
        cam_context_p->sd_result.sd_rect_x1[i] = cam_context_p->sd_result.sd_rect_x1[i] * cam_context_p->preview_width / pFdRsult->image_width;
        cam_context_p->sd_result.sd_rect_y0[i] = cam_context_p->sd_result.sd_rect_y0[i] * cam_context_p->preview_height / pFdRsult->image_height;
        cam_context_p->sd_result.sd_rect_y1[i] = cam_context_p->sd_result.sd_rect_y1[i] * cam_context_p->preview_height / pFdRsult->image_height;        
    }
#endif /* defined(__MED_CAM_FD_SUPPORT__) && (MT6225_SERIES) */
}


MM_ERROR_CODE_ENUM cam_cal_callback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen)
{
#if defined (ISP_SUPPORT)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if (CbId == CAL_CBID_QUICK_VIEW_IMAGE_READY)
    {
        cam_quickview_image_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_THUMBNAIL_IMAGE_READY)
    {
        cam_thumbnail_image_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_STILL_IMAGE_READY)
    {
        cam_main_jpeg_image_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_PANO_IMAGE_READY)
    {
        cam_panorama_image_reday_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_PANO_BG_ADD_IMAGE_READY)
    {
        cam_panorama_bg_stitch_ready_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_PANO_RESULT)
    {
        cam_panorama_stitch_ready_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_AF_RESULT)
    {
        cam_focus_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_FD_RESULT)
    {
        cam_fd_result_callback(pCallbackPara, CallbackParaLen);
    }
    else if (CbId == CAL_CBID_SD_RESULT)
    {
        cam_sd_result_callback(pCallbackPara, CallbackParaLen);
    }
#endif
    return MM_ERROR_NONE;
}


/*****************************************************************************
 * FUNCTION
 *  cam_set_camera_id_req_hdlr
 * DESCRIPTION
 *  This function is to select main/sub sensor.
 * PARAMETERS
 *  ilm_ptr     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_set_camera_id_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_set_camera_id_req_struct* req_p;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//    ASSERT(CAM_IN_STATE(CAM_IDLE));
        
    req_p = (media_cam_set_camera_id_req_struct*)ilm_ptr->local_para_ptr;
    cam_context_p->cam_id = req_p->cam_id;
    
    cam_set_result(MED_RES_OK);
    CAM_SET_EVENT(CAM_EVT_BLOCK);    
}


/*****************************************************************************
 * FUNCTION
 *  cam_sensor_query_capability_req_hdlr
 * DESCRIPTION
 *  This function is to query sensor capability information.
 * PARAMETERS
 *  ilm_ptr     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_sensor_query_capability_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_sensor_query_capability_req_struct *req_p = NULL;
    media_cam_qurey_sensor_general_capability_req_struct* info_p = NULL;
    media_cam_query_iso_binning_capability_req_struct* iso_p = NULL;
    media_cam_query_capture_overlay_req_struct* overlay_p= NULL;
    CAL_FEATURE_CTRL_STRUCT CalFeaturePara;
    CAL_FEATURE_CAMERA_BINNING_INFO_STRUCT BinningInfo;
    CAL_CAMERA_CAPTURE_OVERLAY_CAPABILITY_OUT_STRUCT CameraOverlayOut;
    P_CAL_FEATURE_TYPE_ENUM_STRUCT pCamInfo=(P_CAL_FEATURE_TYPE_ENUM_STRUCT) &(CalFeaturePara.FeatureInfo.FeatureEnum);
    kal_uint32 i = 0;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req_p = (media_cam_sensor_query_capability_req_struct*)ilm_ptr->local_para_ptr;
    
    if (CAM_IN_STATE(CAM_IDLE))
    {
        // Select main or sub sensor
        CalFeaturePara.FeatureId = CAL_FEATURE_CAMERA_SOURCE_SELECT;
        CalFeaturePara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalFeaturePara.FeatureSetValue = (kal_uint32)cam_context_p->cam_id;
        CalFeaturePara.FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalFeaturePara, NULL, 0, NULL);
    }

    CalFeaturePara.FeatureId = req_p->item_id;        
    CalFeaturePara.FeatureCtrlCode = CAL_FEATURE_QUERY_OPERATION;
    CalFeaturePara.FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
    CalFeaturePara.CameraMode = req_p->camera_mode;
        
    if (req_p->item_id == CAL_FEATURE_CAMERA_BINNING_INFO) // CAL_FEATURE_TYPE_STRUCTURED
    {
        iso_p = (media_cam_query_iso_binning_capability_req_struct*)req_p->struct_p;
                
        CalFeaturePara.FeatureId=CAL_FEATURE_CAMERA_BINNING_INFO;
        CalFeaturePara.FeatureCtrlCode=CAL_FEATURE_QUERY_OPERATION;
        CalFeaturePara.FeatureInfo.FeatureStructured.pBinningInfo=&BinningInfo;
        CalCtrl(CAL_FEATURE_CTRL, &CalFeaturePara, &CalFeaturePara, sizeof(CalFeaturePara), NULL);
        
        kal_mem_cpy((void*)&(iso_p->IsoBinningInfo), (void*)&(BinningInfo.IsoBinningInfo), sizeof(BinningInfo));
    }
    else if (req_p->item_id == CAL_FEATUER_CAMERA_CAPTURE_OVERLAY_CAPABILITY) // CAL_FEATURE_TYPE_STRUCTURED
    {                    
        CalFeaturePara.FeatureInfo.FeatureStructured.pCamOverlayCap = &CameraOverlayOut;
        CalCtrl(CAL_FEATURE_CTRL, &CalFeaturePara, &CalFeaturePara, sizeof(CalFeaturePara), NULL);

        overlay_p = (media_cam_query_capture_overlay_req_struct*)req_p->struct_p;
        
        overlay_p->is_support = CameraOverlayOut.CaptureOverlaySupport;
    }
    else // CAL_FEATURE_TYPE_ENUMERATE
    {
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalFeaturePara, (void*)&CalFeaturePara, sizeof(CalFeaturePara), NULL);
    
        info_p = (media_cam_qurey_sensor_general_capability_req_struct*)req_p->struct_p;

        info_p->is_support = pCamInfo->IsSupport;
        info_p->item_count = pCamInfo->ItemCount;

        if (info_p->is_support)
        {
            ASSERT(MAX_CAMERA_SUPPORT_ITEM_COUNT >= pCamInfo->ItemCount);
            for (i=0;i<pCamInfo->ItemCount;i++)
            {
                info_p->support_item[i] = pCamInfo->SupportItem[i];
            }
        }
        else
        {
            info_p->item_count = 0;
        }
    }
    
    cam_set_result(MED_RES_OK);
    CAM_SET_EVENT(CAM_EVT_BLOCK);     
}


/*****************************************************************************
 * FUNCTION
 *  cam_sensor_query_zoom_info_req_hdlr
 * DESCRIPTION
 *  This function is to query sensor zoom information.
 * PARAMETERS
 *  ilm_ptr     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_sensor_query_zoom_info_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_FEATURE_CTRL_STRUCT CalInPara, CalOutPara;
    CAL_CAMERA_DIGITAL_ZOOM_IN_STRUCT zoomInPara;
    CAL_CAMERA_DIGITAL_ZOOM_OUT_STRUCT zoomOutPara;
    media_cam_zoom_capability_req_struct* req_p;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (CAM_IN_STATE(CAM_IDLE))
    {
        // Select main or sub sensor
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_SOURCE_SELECT;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureSetValue = (kal_uint32)cam_context_p->cam_id;
        CalInPara.FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, (void*)&CalOutPara, sizeof(CalOutPara), NULL);

    }
    req_p = (media_cam_zoom_capability_req_struct*)ilm_ptr->local_para_ptr;

    CalInPara.FeatureId = CAL_FEATURE_CAMERA_DIGITAL_ZOOM;
    CalInPara.FeatureCtrlCode = CAL_FEATURE_QUERY_OPERATION;
    CalInPara.FeatureInfo.FeatureStructured.pCamDzInPara = &zoomInPara;
    CalInPara.CameraMode = req_p->camera_mode;
    CalOutPara.FeatureInfo.FeatureStructured.pCamDzOutPara = &zoomOutPara;

    zoomInPara.CameraImageSize = (CAM_IMAGE_SIZE_ENUM)req_p->image_size;
    CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, (void*)&CalOutPara, sizeof(CalOutPara), NULL);

    req_p->result_p->DzType = CalOutPara.FeatureInfo.FeatureStructured.pCamDzOutPara->DzType;
    req_p->result_p->MaxZoomFactor = CalOutPara.FeatureInfo.FeatureStructured.pCamDzOutPara->MaxZoomFactor;
    req_p->result_p->TotalZoomSteps = CalOutPara.FeatureInfo.FeatureStructured.pCamDzOutPara->TotalZoomSteps;
    req_p->result_p->pZoomFactorListTable = CalOutPara.FeatureInfo.FeatureStructured.pCamDzOutPara->pZoomFactorListTable;
    
    cam_set_result(MED_RES_OK);
    CAM_SET_EVENT(CAM_EVT_BLOCK);
}


/*****************************************************************************
 * FUNCTION
 *  cam_free_working_buffer
 * DESCRIPTION
 *  This function is to release cam related memory.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void cam_free_working_buffer(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cam_context_p->frame_buffer2_p)
    {
        med_free_asm_mem(cam_context_p->app_id, (void **)&cam_context_p->frame_buffer2_p);
        cam_context_p->frame_buffer_size = 0;
    }
    
    if (cam_context_p->extmem_start_address)
    {
        med_free_asm_mem(cam_context_p->app_id, (void **)&cam_context_p->extmem_start_address);
        cam_context_p->extmem_size = 0;
    }

#if defined(__MED_CAM_FD_SUPPORT__)
    if (cam_context_p->extmem_fd_start_address)
    {
        med_free_asm_mem(cam_context_p->app_id, (void **)&cam_context_p->extmem_fd_start_address);
    }
#endif

    if (cam_context_p->raw_image_buffer_address)
    {
        med_free_asm_mem(cam_context_p->app_id, (void **)&cam_context_p->raw_image_buffer_address);
        cam_context_p->raw_image_buffer_address = NULL;
    }
    

    if (cam_context_p->thumbnail_buffer_address)
    {
        med_free_asm_mem(cam_context_p->app_id,(void **)&cam_context_p->thumbnail_buffer_address);
        cam_context_p->thumbnail_buffer_size = 0;
    }

}


/*****************************************************************************
 * FUNCTION
 *  cam_power_up_req_hdlr
 * DESCRIPTION
 *  This function is to handle power up request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_power_up_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_power_up_req_struct *req_p;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    MM_ERROR_CODE_ENUM ret = MM_ERROR_NONE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_power_up_req_struct*) ilm_ptr->local_para_ptr;

    cam_context_p->seq_num = req_p->seq_num;
    cam_context_p->app_id = req_p->app_id;

    if (CAM_IN_STATE(CAM_IDLE))
    {
        // Select main or sub sensor
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_SOURCE_SELECT;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureSetValue = (kal_uint32)cam_context_p->cam_id;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, NULL, 0, NULL);
        
        ret = CalOpen(cam_cal_callback);

        if (ret == MM_ERROR_NONE)
        {
            CAM_ENTER_STATE(CAM_READY);
            cam_set_result(MED_RES_OK);
        }
        else
        {
            cam_set_result(MED_RES_FAIL);
        }
    }
    else if (CAM_IN_STATE(CAM_READY))
    {
        cam_send_ready_ind(MED_RES_OK);
    }
    CAM_SET_EVENT(CAM_EVT_POWER_UP);
}


/*****************************************************************************
 * FUNCTION
 *  cam_power_down_req_hdlr
 * DESCRIPTION
 *  This function is to handle power down request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_power_down_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalScenarioPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    if (!CAM_IN_STATE(CAM_IDLE))
    {
        if (CAM_IN_STATE(CAM_PREVIEW))
        {
            // Stop preview
            CalScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_PREVIEW;
            CalScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
            CalScenarioPara.ScenarioCtrlParaLen=0;
            CalScenarioPara.pScenarioCtrlPara=NULL;
            CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalScenarioPara, NULL, 0, NULL);
        }
        else if (CAM_IN_STATE(CAM_CAPTURE))
        {
            // Stop capture
            cam_capture_still_image_stop();
        }
    
        CalClose();
    
        /* to release the memory allocated in MED_MODE_BUFFER mode */
        CAM_FREE_CAPTURE_BUFFER();
        cam_free_working_buffer();
        CAM_ENTER_STATE(CAM_IDLE);
    }
    CAM_SET_EVENT(CAM_EVT_POWER_DOWN);
}


/*****************************************************************************
 * FUNCTION
 *  cam_preview_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera preview request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_preview_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_preview_req_struct *req_p;
    CAL_SCENARIO_CTRL_STRUCT CalPreviewScenarioPara;
    CAL_CAMERA_PREVIEW_STRUCT CalPreviewPara;
    CAL_LCD_STRUCT CalLcdPara;
    CAL_ISP_PREVIEW_STRUCT CalIspPreviewPara;
    CAL_POSTPROC_STRUCT CalPostProcPara;
    CAL_MDP_PREVIEW_STRUCT CalMdpPreviewPara;
    MM_ERROR_CODE_ENUM ret;
    MM_IMAGE_ROTATE_ENUM PreviewRotate;
    MM_IMAGE_ROTATE_ENUM IncludedAngle; 
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    if (CAM_IN_STATE(CAM_READY))
    {
        CAM_FREE_CAPTURE_BUFFER();
        
        req_p = (media_cam_preview_req_struct*) ilm_ptr->local_para_ptr;

        cam_context_p->update_layer = req_p->update_layer;
        cam_context_p->hw_update_layer = req_p->hw_update_layer;

        cam_context_p->preview_offset_x = req_p->preview_offset_x;
        cam_context_p->preview_offset_y = req_p->preview_offset_y;

        cam_context_p->extmem_start_address = 0;
        cam_context_p->extmem_size = 0;
        cam_context_p->ui_rotate = req_p->ui_rotate;
        cam_context_p->binning_mode = req_p->binning_mode;

        cam_context_p->frame_buffer1_p = req_p->frame_buffer1_address;
        cam_context_p->frame_buffer2_p = (kal_uint32) med_alloc_asm_mem(cam_context_p->app_id, req_p->frame_buffer_size);
        CAM_PREVIEW_CHECK_IS_BUFFER_AVAILABLE(cam_context_p->frame_buffer2_p);

        if (cam_context_p->lcd_id == 0) // MainLCD
        {           
            if (cam_context_p->cam_id == CAL_CAMERA_SOURCE_MAIN)
            {
                cam_context_p->sensor_rotate = CAM_MAIN_SENSOR_ROTATE;
            #if defined(SENSOR_INSIDE_CLAM)
                cam_context_p->sensor_rotate += MM_IMAGE_ROTATE_180;
            #endif

                PreviewRotate = (cam_context_p->sensor_rotate + MAIN_LCM_CROSS_SENSOR_ANGLE) % 4;            
                if (CAM_MAIN_SENSOR_MIRROR)
                    PreviewRotate += MM_IMAGE_H_MIRROR;
            }
            else
            {
                cam_context_p->sensor_rotate = CAM_SUB_SENSOR_ROTATE;
                
                PreviewRotate = (cam_context_p->sensor_rotate + MAIN_LCM_CROSS_SENSOR_ANGLE) % 4;
                if (CAM_SUB_SENSOR_MIRROR)
                    PreviewRotate += MM_IMAGE_H_MIRROR;
            }
        }
        else // SubLCD
        {
            if (cam_context_p->cam_id == CAL_CAMERA_SOURCE_MAIN)
            {
                cam_context_p->sensor_rotate = CAM_MAIN_SENSOR_ROTATE;
                
                PreviewRotate = (cam_context_p->sensor_rotate + SUB_LCM_CROSS_SENSOR_ANGLE) % 4;
                if (CAM_MAIN_SENSOR_MIRROR)
                    PreviewRotate += MM_IMAGE_H_MIRROR;
            }
            else
            {
                cam_context_p->sensor_rotate = CAM_SUB_SENSOR_ROTATE;

                PreviewRotate = (cam_context_p->sensor_rotate + SUB_LCM_CROSS_SENSOR_ANGLE) % 4;
                if (CAM_SUB_SENSOR_MIRROR)
                    PreviewRotate += MM_IMAGE_H_MIRROR;
            }            
        }

        IncludedAngle = CAM_ABS(cam_context_p->ui_rotate - cam_context_p->sensor_rotate);
        if (IncludedAngle == MM_IMAGE_ROTATE_90 || IncludedAngle == MM_IMAGE_ROTATE_270)
        {
            cam_context_p->preview_width = req_p->preview_height;
            cam_context_p->preview_height = req_p->preview_width;            
        }
        else
        {
            cam_context_p->preview_width = req_p->preview_width;
            cam_context_p->preview_height = req_p->preview_height;
        }

        cam_context_p->included_angle = IncludedAngle;
        cam_context_p->preview_rotate = PreviewRotate;
        
        CalMdpPreviewPara.LcdUpdateEnable = cam_context_p->lcd_update = req_p->lcd_update;        
        CalMdpPreviewPara.DisplayImageWidth = cam_context_p->preview_width;
        CalMdpPreviewPara.DisplayImageHeight = cam_context_p->preview_height;
        CalMdpPreviewPara.ImageDataFormat = cam_context_p->image_data_format = req_p->image_data_format;
        CalMdpPreviewPara.FrameBuffAddr1 = cam_context_p->frame_buffer1_p;
        CalMdpPreviewPara.FrameBuffAddr2 = cam_context_p->frame_buffer2_p;
        CalMdpPreviewPara.FrameBuffSize = cam_context_p->frame_buffer_size = req_p->frame_buffer_size;
        CalMdpPreviewPara.OverlayEnable = cam_context_p->overlay_frame_mode = req_p->overlay_frame_mode;
        CalMdpPreviewPara.OverlayPaletteAddr = cam_context_p->overlay_palette_addr = (kal_uint32)req_p->overlay_palette_addr;
        CalMdpPreviewPara.OverlayPaletteSize = cam_context_p->overlay_palette_size = req_p->overlay_palette_size;
        CalMdpPreviewPara.OverlayColorDepth = cam_context_p->overlay_color_depth = req_p->overlay_color_depth;
        CalMdpPreviewPara.OverlayFrameSourceKey = cam_context_p->overlay_frame_source_key = req_p->overlay_frame_source_key;
        CalMdpPreviewPara.OverlayFrameWidth = cam_context_p->overlay_frame_width = req_p->overlay_frame_width;
        CalMdpPreviewPara.OverlayFrameHeight = cam_context_p->overlay_frame_height = req_p->overlay_frame_height;
        CalMdpPreviewPara.OverlayFrameBuffAddr = cam_context_p->overlay_frame_buffer_address = req_p->overlay_frame_buffer_address;

        CalMdpPreviewPara.ImageRotAngle = PreviewRotate;
        CalMdpPreviewPara.ImageDataFormat = cam_context_p->image_data_format = req_p->image_data_format;

        CalIspPreviewPara.ZoomStep = cam_context_p->zoom_step = req_p->zoom_step;
        CalIspPreviewPara.ContrastLevel = cam_context_p->contrast_level = req_p->contrast;
        CalIspPreviewPara.SaturationLevel = cam_context_p->saturation_level = req_p->saturation;
        CalIspPreviewPara.SharpnessLevel = cam_context_p->sharpness_level = req_p->sharpness_level;
        CalIspPreviewPara.WbMode = cam_context_p->wb_mode = req_p->WB;
        CalIspPreviewPara.EvValue = cam_context_p->ev_value = req_p->exposure;
        CalIspPreviewPara.BandingFreq = cam_context_p->banding_freq = req_p->banding_freq;
        CalIspPreviewPara.ImageEffect = cam_context_p->image_effect = req_p->effect;
        CalIspPreviewPara.SceneMode = cam_context_p->scene_mode = req_p->scene_mode;
        CalIspPreviewPara.AeMeteringMode = cam_context_p->ae_metering_mode = req_p->ae_metering_mode;
        CalIspPreviewPara.FlashMode = cam_context_p->flash_mode = req_p->flash_mode;
        CalIspPreviewPara.AfOperationMode = cam_context_p->af_operation_mode = req_p->af_operation_mode;
        CalIspPreviewPara.AfRange = cam_context_p->af_range = req_p->af_range;

        CalLcdPara.WfcElementHandle = req_p->layer_element;
        CalLcdPara.WfcContextHandle = req_p->blt_ctx;
        CalLcdPara.WfcDeviceHandle = req_p->blt_dev;
        
        cam_context_p->fd_enable = req_p->fd_enable;

        cam_context_p->extmem_size += CAL_PREVIEW_EXT_MEMORY_SIZE;

        CalPostProcPara.PostProcMode = 0;
        if (cam_context_p->fd_enable == KAL_TRUE)
        {
            CalPostProcPara.PostProcMode = PP_MODE_FACE_DETECT;
        }

        if (cam_context_p->current_shot_number && 
            cam_context_p->capture_mode == CAM_STILL_CAPTURE_PANO_VIEW)
        {
            CalPostProcPara.PostProcMode = PP_MODE_PANORAMA;
        }

        if (CalPostProcPara.PostProcMode)
            cam_context_p->extmem_size += CAL_PP_PREVIEW_EXT_MEMORY_SIZE;
        
        if (cam_context_p->extmem_size)
        {
            cam_context_p->extmem_start_address = (kal_uint32) med_alloc_asm_mem_framebuffer(cam_context_p->app_id, cam_context_p->extmem_size);
            CAM_PREVIEW_CHECK_IS_BUFFER_AVAILABLE(cam_context_p->extmem_start_address);
        }

        CalPreviewPara.ExtmemStartAddress = cam_context_p->extmem_start_address;
        CalPreviewPara.ExtmemSize = cam_context_p->extmem_size;

        CalPreviewPara.pLcdPara = &CalLcdPara;
        CalPreviewPara.pMdpPara = &CalMdpPreviewPara;
        CalPreviewPara.pIspPara = &CalIspPreviewPara;
        CalPreviewPara.pPostProcPara = &CalPostProcPara;

        CalPreviewScenarioPara.ScenarioId = CAL_SCENARIO_CAMERA_PREVIEW;
        CalPreviewScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_START;
        CalPreviewScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_PREVIEW_STRUCT);
        CalPreviewScenarioPara.pScenarioCtrlPara = &CalPreviewPara;

        ret = CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalPreviewScenarioPara, NULL, 0, NULL);

        if (ret == MM_ERROR_NONE)
        {
            CAM_ENTER_STATE(CAM_PREVIEW);
        }
        else
        {
            cam_free_working_buffer();
            cam_set_result(MED_RES_FAIL);
        }
    }
    else
    {
        cam_set_result(MED_RES_BUSY);
    }

    CAM_SET_EVENT(CAM_EVT_PREVIEW);
}


/*****************************************************************************
 * FUNCTION
 *  cam_stop_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera stop request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_stop_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalScenarioPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    if (CAM_IN_STATE(CAM_PREVIEW))
    {
        // Stop preview
        CalScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_PREVIEW;
        CalScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
        CalScenarioPara.ScenarioCtrlParaLen=0;
        CalScenarioPara.pScenarioCtrlPara=NULL;
        CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalScenarioPara, NULL, 0, NULL);

        cam_free_working_buffer();
        CAM_ENTER_STATE(CAM_READY);
        
    }
    else if (CAM_IN_STATE(CAM_CAPTURE))
    {
        // Stop capture
        CalScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_STILL_CAPTURE;
        CalScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
        CalScenarioPara.ScenarioCtrlParaLen=0;
        CalScenarioPara.pScenarioCtrlPara=NULL;
        CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalScenarioPara, NULL, 0, NULL);
        
        CAM_FREE_CAPTURE_BUFFER();    
        cam_free_working_buffer();
        CAM_ENTER_STATE(CAM_READY);
    }
    
    CAM_SET_EVENT(CAM_EVT_STOP);

}


/*****************************************************************************
 * FUNCTION
 *  cam_stop_fast_zoom
 * DESCRIPTION
 *  This function is to stop zoom operation if camera is zooming.
 * PARAMETERS
 *  cause       [IN]
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void cam_stop_fast_zoom(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    CAL_CAMERA_FAST_ZOOM_STRUCT ZoomPara;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(cam_context_p->is_fast_zoom == KAL_TRUE)
    {
        ZoomPara.ZoomStart = KAL_FALSE;
       
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_FAST_ZOOM;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;       
        CalInPara.FeatureInfo.FeatureStructured.pCamFastZoom = &ZoomPara;
        CalInPara.FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, NULL, 0, NULL);
        
        cam_context_p->is_fast_zoom = KAL_FALSE;
    }

}


/*****************************************************************************
 * FUNCTION
 *  cam_start_fast_zoom_req_hdlr
 * DESCRIPTION
 *  This function is to handle the request of starting the camera fast zoom.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_start_fast_zoom_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_start_fast_zoom_req_struct *req_p;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    CAL_CAMERA_FAST_ZOOM_STRUCT ZoomPara;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    if (CAM_IN_STATE(CAM_PREVIEW))
    {
        req_p = (media_cam_start_fast_zoom_req_struct*) ilm_ptr->local_para_ptr;

        ZoomPara.ZoomStart = KAL_TRUE;
        ZoomPara.ZoomIn = req_p->zoom_in;
        ZoomPara.ZoomLimit = req_p->zoom_limit;
        ZoomPara.ZoomStep = req_p->zoom_step;
        ZoomPara.ZoomSpeed = req_p->zoom_speed;

        CalInPara.FeatureId = CAL_FEATURE_CAMERA_FAST_ZOOM;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureInfo.FeatureStructured.pCamFastZoom = &ZoomPara;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, NULL, 0, NULL);
        
        cam_context_p->is_fast_zoom = KAL_TRUE;
    }
    
    CAM_SET_EVENT(CAM_EVT_FAST_ZOOM);
}


/*****************************************************************************
 * FUNCTION
 *  cam_stop_fast_zoom_req_hdlr
 * DESCRIPTION
 *  This function is to handle the request of stopping the camera fast zoom.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_stop_fast_zoom_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    CAL_CAMERA_FAST_ZOOM_STRUCT ZoomPara;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    if (CAM_IN_STATE(CAM_PREVIEW))
    {
        ZoomPara.ZoomStart = KAL_FALSE;
        
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_FAST_ZOOM;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureInfo.FeatureStructured.pCamFastZoom = &ZoomPara;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, NULL, 0, NULL);
        
        cam_context_p->is_fast_zoom = KAL_FALSE;
    }
    CAM_SET_EVENT(CAM_EVT_FAST_ZOOM);
}


/*****************************************************************************
 * FUNCTION
 *  cam_get_fast_zoom_factor_req_hdlr
 * DESCRIPTION
 *  This function is to handle the request of getting the camera fast zoom factor.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_get_fast_zoom_factor_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_get_fast_zoom_factor_req_struct *req_p;
    CAL_FEATURE_CTRL_STRUCT CalInPara, CalOutPara;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* isp_fast_zoom_struct zoom_data; */

    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_get_fast_zoom_factor_req_struct*) ilm_ptr->local_para_ptr;

    if (req_p->factor)
    {
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_CURRENT_ZOOM_FACTOR;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, (void*)&CalOutPara, sizeof(CalOutPara), NULL);
        *req_p->factor = CalOutPara.FeatureCurrentValue;
    }
    if (req_p->step)
    {
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_CURRENT_ZOOM_STEP;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;
        CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, (void*)&CalOutPara, sizeof(CalOutPara), NULL);        
        *req_p->step = CalOutPara.FeatureCurrentValue;
    }
    CAM_SET_EVENT(CAM_EVT_GET_FAST_ZOOM_FACTOR);
}


/*****************************************************************************
 * FUNCTION
 *  cam_get_focus_zone_req_hdlr
 * DESCRIPTION
 *  This function is to get af zone from drv and return the values to MMI
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_get_focus_zone_req_hdlr(ilm_struct* ilm_ptr)
{
#ifdef __CAM_AUTO_FOCUS__
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_af_get_zone_struct *req_p;
    media_cam_af_zone_para_struct *zone_para_p;
    CAL_FEATURE_CTRL_STRUCT CalInPara, CalOutPara;
    P_CAL_CAMERA_GET_AF_ZONE_STRUCT pAFInfo;
    kal_uint8 i = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_af_get_zone_struct*) ilm_ptr->local_para_ptr;
    zone_para_p = req_p->zone_para_p;

    if (CAM_IN_STATE(CAM_PREVIEW))
    {
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_GET_AF_ZONE;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;

        CalCtrl(CAL_FEATURE_CTRL, &CalInPara, &CalOutPara, sizeof(CalOutPara), NULL);
        
        pAFInfo = CalOutPara.FeatureInfo.FeatureStructured.pCamGetAfZoneOutPara;
        zone_para_p->af_active_zone = pAFInfo->AfActiveZone;
        kal_mem_cpy((void*)&(zone_para_p->af_zone), (void*)&(pAFInfo->AfZoneLcd), sizeof(pAFInfo->AfZoneLcd));

        if (cam_context_p->included_angle == MM_IMAGE_ROTATE_90 || cam_context_p->included_angle == MM_IMAGE_ROTATE_270)
        {
            for (i = 0; i < MEDIA_CAM_AF_ZONE_MAX_NO; i++)
            {
                zone_para_p->af_zone[i].af_zone_w = pAFInfo->AfZoneLcd[i].H;
                zone_para_p->af_zone[i].af_zone_h = pAFInfo->AfZoneLcd[i].W;

                zone_para_p->af_zone[i].af_zone_x = pAFInfo->AfZoneLcd[i].Y;
                zone_para_p->af_zone[i].af_zone_y = pAFInfo->AfZoneLcd[i].X;
            }
        }        
        
    }
    else
    {
        ASSERT(0);
    }   
    CAM_SET_EVENT(CAM_EVT_GET_FOCUS_ZONE);
#else
    ASSERT(0);
#endif

}


void cam_save_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 result = MED_RES_OK;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* state must be in CAM_CAPTURE */
    cam_context_p->is_waiting_save = KAL_FALSE;
    
    if (cam_context_p->media_mode == MED_MODE_BUFFER)
    {
        if (cam_context_p->is_main_jpeg_ready)
        {
        #ifndef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__
            result = cam_open_image_file((kal_wchar*)cam_context_p->file_name_p);
            if (result == MED_RES_OK)
        #endif
            {
                result = cam_close_image_file(cam_context_p->jpeg_image_size);
            }
        
            cam_set_result(result);
        }
        else
        {
            cam_context_p->is_waiting_save = KAL_TRUE;
        }
    }
    else
    {
        cam_set_result(MED_RES_ERROR);      
    }

    if (!cam_context_p->is_waiting_save)
    {
        CAM_SET_EVENT(CAM_EVT_SAVE);
    }
}


/*****************************************************************************
 * FUNCTION
 *  cam_capture_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera capture request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
kal_int32 cam_capture_still_image_start(
    kal_uint32 buffer_address, 
    kal_uint32 buffer_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_POSTPROC_STRUCT CalPostCapturePara;
    CAL_SCENARIO_CTRL_STRUCT CalCaptureScenarioPara;
    CAL_ISP_STILL_CAPTURE_STRUCT CalIspCapturePara;
    CAL_MDP_STILL_CAPTURE_STRUCT CalMdpCapturePara;
    CAL_CAMERA_STILL_CAPTURE_STRUCT CalCapturePara;
    CAL_JPEG_ENC_STRUCT CalJpegEncPara;
    CAL_FEATURE_CTRL_STRUCT CalInPara, CalOutPara;
    MM_ERROR_CODE_ENUM ret;

    kal_int32 resized_offset_x = 0;
    kal_int32 resized_offset_y = 0;
    kal_int32 resized_width = 0;
    kal_int32 resized_height = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_util_fit_box(
            160,
            120,
            cam_context_p->image_width,
            cam_context_p->image_height,
            &resized_offset_x,
            &resized_offset_y,
            &resized_width,
            &resized_height);

    cam_context_p->raw_capture_enable = KAL_FALSE;
#if defined(CAM_OFFLINE_CAPTURE)      
    cam_context_p->raw_capture_enable = KAL_TRUE;
#elif defined(CAM_STANDARD_CAPTURE)
    CalInPara.FeatureId = CAL_FEATURE_CAMERA_CURRENT_ZOOM_STEP;
    CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;
    CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, (void*)&CalOutPara, sizeof(CalOutPara), NULL);        
    if (CalOutPara.FeatureCurrentValue !=0)
    {
        cam_context_p->raw_capture_enable = KAL_TRUE;
    }
#endif

    CalPostCapturePara.PostProcMode = 0;
    if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_PANO_VIEW)
    {
        CalPostCapturePara.PostProcMode = PP_MODE_PANORAMA;
    }

    CalMdpCapturePara.MainImageDataFormat = MM_IMAGE_FORMAT_JPEG;
    CalMdpCapturePara.MainImageJpegYuvSampleFormat = MM_IMAGE_FORMAT_YUV422;
    CalMdpCapturePara.MainImageWidth = cam_context_p->image_width;
    CalMdpCapturePara.MainImageHeight = cam_context_p->image_height;
    CalMdpCapturePara.MainImageBuffAddr = buffer_address;
    CalMdpCapturePara.MainImageBuffSize = buffer_size;
    CalMdpCapturePara.MainImageWorkingBuffAddr = cam_context_p->capture_buffer_p;
    CalMdpCapturePara.MainImageWorkingBuffSize = cam_context_p->capture_buffer_size;
    CalMdpCapturePara.MainImageRotAngle = (MM_IMAGE_ROTATE_ENUM)cam_context_p->capture_rotate;
    CalMdpCapturePara.OverlayEnable = cam_context_p->overlay_frame_mode;
    CalMdpCapturePara.OverlayPaletteAddr = cam_context_p->overlay_palette_addr;
    CalMdpCapturePara.OverlayPaletteSize = cam_context_p->overlay_palette_size;
    CalMdpCapturePara.OverlayColorDepth = cam_context_p->overlay_color_depth;
    CalMdpCapturePara.OverlayFrameSourceKey = cam_context_p->overlay_frame_source_key;
    CalMdpCapturePara.OverlayFrameWidth = cam_context_p->overlay_frame_width;
    CalMdpCapturePara.OverlayFrameHeight = cam_context_p->overlay_frame_height;
    CalMdpCapturePara.OverlayFrameBuffAddr = cam_context_p->overlay_frame_buffer_address;
//    CalMdpCapturePara.OverlayPaletteMode = IMG_PALETTE_MODE_8BPP;

    CalMdpCapturePara.QuickViewImageEnable = cam_context_p->quickview_output;
    CalMdpCapturePara.QuickViewImageFormat = MM_IMAGE_FORMAT_RGB565;
    CalMdpCapturePara.QuickViewImageWidth = cam_context_p->quickview_output_width ;
    CalMdpCapturePara.QuickViewImageHeight = cam_context_p->quickview_output_height;
    CalMdpCapturePara.QuickViewImageBuffAddr = cam_context_p->quickview_output_buffer_address;
    CalMdpCapturePara.QuickViewImageBuffSize = cam_context_p->quickview_output_buffer_size;
    CalMdpCapturePara.QuickViewImageRotAngle = (MM_IMAGE_ROTATE_ENUM)cam_context_p->capture_rotate;    
    
    if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_PANO_VIEW)
        CalMdpCapturePara.ThumbnailImageEnable = KAL_FALSE;
    else
        CalMdpCapturePara.ThumbnailImageEnable = KAL_TRUE;
    
    CalMdpCapturePara.ThumbnailImageFormat = MM_IMAGE_FORMAT_RGB565;
    CalMdpCapturePara.ThumbnailImageWidth = resized_width;
    CalMdpCapturePara.ThumbnailImageHeight = resized_height;
    CalMdpCapturePara.ThumbnailImageRotAngle = (MM_IMAGE_ROTATE_ENUM)cam_context_p->capture_rotate;
    CalMdpCapturePara.ThumbnailImageBuffAddr = cam_context_p->thumbnail_buffer_address;
    CalMdpCapturePara.ThumbnailImageBuffSize = cam_context_p->thumbnail_buffer_size;
    CalMdpCapturePara.LcdUpdateEnable = KAL_FALSE;

    CalIspCapturePara.BinningMode = cam_context_p->binning_mode;
    CalIspCapturePara.MultishotInfo = (MULTISHOT_INFO_ENUM)cam_context_p->multishot_info;
    CalIspCapturePara.ExpoBracketLevel = cam_context_p->expo_bracket_level;
    CalIspCapturePara.RawCaptureEnable = cam_context_p->raw_capture_enable;
    CalIspCapturePara.StillCaptureMode = (CAL_CAMERA_STILL_CAPTURE_ENUM)cam_context_p->capture_mode;    

    CalJpegEncPara.JpegEncQuality = (JPEG_ENCODE_QUALITTY_ENUM)cam_context_p->image_quality;
    //CalJpegEncPara.JpegSamplingFormat = JPEG_YUV_FORMAT_YUV420;
    CalJpegEncPara.JpegSamplingFormat = MM_IMAGE_FORMAT_YUV422;
#ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__
    CalJpegEncPara.pDstFileHandle = &cam_context_p->fsal_file;
#else
    CalJpegEncPara.pDstFileHandle = NULL;
#endif

    CalCapturePara.ExtmemStartAddress = cam_context_p->extmem_start_address;
    CalCapturePara.ExtmemSize = cam_context_p->extmem_size;

    CalCapturePara.pMdpPara = &CalMdpCapturePara;
    CalCapturePara.pIspPara = &CalIspCapturePara;
    CalCapturePara.pJpegEncPara = &CalJpegEncPara;
    CalCapturePara.pPostProcPara = &CalPostCapturePara;

    CalCaptureScenarioPara.ScenarioId = CAL_SCENARIO_CAMERA_STILL_CAPTURE;
    CalCaptureScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_START;
    CalCaptureScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_STILL_CAPTURE_STRUCT);
    CalCaptureScenarioPara.pScenarioCtrlPara = &CalCapturePara;

    cam_context_p->is_quickview_ready = KAL_FALSE;
    cam_context_p->is_thumbnail_ready = KAL_FALSE;
    cam_context_p->is_main_jpeg_ready = KAL_FALSE;

    MED_CAM_START_LOGGING("CPJ");
    MED_CAM_GET_START_TIME(CAM_CAPTURE_JPEG_DURATION);

    CAM_ENTER_STATE(CAM_CAPTURE);        
    ret = CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalCaptureScenarioPara, NULL, 0, 0);

    return (kal_int32)ret;
}


/*****************************************************************************
 * FUNCTION
 *  cam_capture_still_image_stop
 * DESCRIPTION
 *  This function is to stop camera capture.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_capture_still_image_stop(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalCaptureScenarioPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CalCaptureScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_STILL_CAPTURE;
    CalCaptureScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
    CalCaptureScenarioPara.ScenarioCtrlParaLen=0;
    CalCaptureScenarioPara.pScenarioCtrlPara=NULL;
    CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalCaptureScenarioPara, NULL, 0, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  cam_capture_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera capture request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_capture_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_capture_req_struct *req_p;
#if defined(__YUVCAM_INTERPOLATION_SW__)
    kal_int32 sensor_max_width = 0;
    kal_int32 sensor_max_height = 0;
#endif
    CAL_SCENARIO_CTRL_STRUCT CalScenarioPara;
    CAL_FEATURE_CTRL_STRUCT CalFeaturePara;
    kal_uint32 buffer_address = 0;
    kal_uint32 buffer_size = 0;
    kal_int32 ret;
    kal_bool  is_first_capture = KAL_FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_capture_req_struct*) ilm_ptr->local_para_ptr;

    #if defined(__MED_CAM_XENON_FLASH_SUPPORT__)
    if (cam_context_p->xenon_flash_status != CAM_XENON_FLASH_OFF)
        med_stop_timer(CAM_XENON_FLASH_CHECK_TIMER);
    #endif /* defined(__MED_CAM_XENON_FLASH_SUPPORT__) */

    /* it is for MoDis, we return from here */
    #if !defined(__MTK_TARGET__)
    {
        cam_set_result(MED_RES_BUSY);
        CAM_SET_EVENT(CAM_EVT_CAPTURE);
        return;
    }
    #endif

    if (CAM_IN_STATE(CAM_READY) || CAM_IN_STATE(CAM_PREVIEW))
    {
        CAM_FREE_CAPTURE_BUFFER();

        /* stop prevew before capture */ 
        if(CAM_IN_STATE(CAM_PREVIEW))
        {
            CalScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_PREVIEW;
            CalScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
            CalScenarioPara.ScenarioCtrlParaLen=0;
            CalScenarioPara.pScenarioCtrlPara=NULL;
            CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalScenarioPara, NULL, 0, NULL);
            CAM_ENTER_STATE(CAM_READY);
            is_first_capture = KAL_TRUE;
        }

        /* release preview related memory */
        cam_free_working_buffer();

        ASSERT(req_p->source_device == CAM_SRC_ISP || req_p->source_device == CAM_SRC_MEM);
        if ((req_p->source_device != CAM_SRC_ISP && req_p->source_device != CAM_SRC_MEM))
        {
            cam_set_result(MED_RES_ERROR);
            CAM_ENTER_STATE(CAM_READY);
            CAM_SET_EVENT(CAM_EVT_CAPTURE);
            return;
        }

        cam_context_p->media_mode = req_p->media_mode;
        cam_context_p->source_device = req_p->source_device;
        cam_context_p->snapshot_number = req_p->snapshot_number;
        cam_context_p->capture_mode = req_p->capture_mode;    

        cam_context_p->continue_capture = req_p->continue_capture;
        cam_context_p->captured_number = 0;
        cam_context_p->expo_bracket_level = 0;

    #if defined(__YUVCAM_INTERPOLATION_SW__)
        #if defined(__IMAGE_SENSOR_03M__)
        sensor_max_width = 640;
        sensor_max_height = 480;
        #elif defined(__IMAGE_SENSOR_1M__)
        sensor_max_width = 1280;
        sensor_max_height = 1024;        
        #elif defined(__IMAGE_SENSOR_2M__)
        sensor_max_width = 1600;
        sensor_max_height = 1200;
        #else
        ASSERT(0);
        #endif

        if (req_p->image_width > sensor_max_width && req_p->image_height > sensor_max_height)
        {
            if (req_p->image_width % 16 != 0)
            {
                ASSERT(0);
            }

            cam_context_p->interpolate_image_width = req_p->image_width;
            cam_context_p->interpolate_image_height = req_p->image_height;
            req_p->image_width = sensor_max_width;
            req_p->image_height = sensor_max_height;
        }
        else
        {
            cam_context_p->interpolate_image_width = 0;
            cam_context_p->interpolate_image_height = 0;            
        }
    #endif /* __YUVCAM_INTERPOLATION_SW__ */

        cam_context_p->capture_rotate = cam_context_p->included_angle;
        if (cam_context_p->cam_id == CAL_CAMERA_SOURCE_MAIN)
        {
            if (CAM_MAIN_SENSOR_MIRROR)
                cam_context_p->capture_rotate = cam_context_p->included_angle + MM_IMAGE_H_MIRROR;
        }
        else
        {
            if (CAM_SUB_SENSOR_MIRROR)
                cam_context_p->capture_rotate = cam_context_p->included_angle + MM_IMAGE_H_MIRROR;
        }
    

        cam_context_p->image_width = req_p->image_width;
        cam_context_p->image_height = req_p->image_height;
        cam_context_p->overlay_frame_mode = req_p->overlay_frame_mode;
        cam_context_p->overlay_palette_addr = (kal_uint32)req_p->overlay_palette_addr;
        cam_context_p->overlay_palette_size = req_p->overlay_palette_size;
        cam_context_p->overlay_color_depth = req_p->overlay_color_depth;
        cam_context_p->overlay_frame_source_key = req_p->overlay_frame_source_key;
        cam_context_p->overlay_frame_width = req_p->overlay_frame_width;
        cam_context_p->overlay_frame_height = req_p->overlay_frame_height;
        cam_context_p->overlay_frame_buffer_address = req_p->overlay_frame_buffer_address;

        cam_context_p->quickview_output = req_p->quickview_output;
        cam_context_p->quickview_output_width = req_p->quickview_output_width;
        cam_context_p->quickview_output_height = req_p->quickview_output_height;
        cam_context_p->quickview_output_buffer_address = req_p->quickview_output_buffer_address;
        cam_context_p->quickview_output_buffer_size = req_p->quickview_output_buffer_size;

        cam_context_p->snapshot_number = req_p->snapshot_number;
        cam_context_p->expo_bracket_level = 0;
        cam_context_p->capture_mode = req_p->capture_mode;    
        cam_context_p->image_quality = req_p->image_quality;
        cam_context_p->jpeg_file_info_p = (jpegs_struct *)req_p->jpegs_p;
        cam_context_p->capture_buffer_pp = req_p->capture_buffer_p;
        cam_context_p->capture_buffer_size_p = req_p->file_size_p;
        cam_context_p->is_waiting_save = KAL_FALSE;
        cam_context_p->stitch_direction = req_p->direction;
        cam_context_p->file_name_p = req_p->filename_p;

        if (cam_context_p->image_resolution == CAM_IMAGE_SIZE_WALLPAPER)
            cam_context_p->capture_buffer_size = MAX_CAM_CAPTURE_LCD_ISP_BUF_SIZE;
        else
            cam_context_p->capture_buffer_size = MAX_CAM_CAPTURE_ISP_BUF_SIZE;
        
        cam_context_p->capture_buffer_p = (kal_uint32) med_alloc_asm_mem_framebuffer(cam_context_p->app_id ,cam_context_p->capture_buffer_size);
        CAM_CAPTURE_CHECK_IS_BUFFER_AVAILABLE(cam_context_p->capture_buffer_p);
            
        if (req_p->media_mode == MED_MODE_BUFFER)
        {
            if (req_p->capture_mode != CAM_STILL_CAPTURE_NORMAL && 
                req_p->capture_mode != CAM_STILL_CAPTURE_CONT_SHOT &&
                req_p->capture_mode != CAM_STILL_CAPTURE_ADD_FRAME)
            {
                ASSERT(0);
            }

        #ifdef __YUVCAM_ENCODE_DIRECT_WRITE_FILE__
            ret = cam_open_image_file(cam_context_p->file_name_p);
            if (ret != MED_RES_OK)
            {
                cam_free_working_buffer();
                cam_set_result(ret);
                CAM_SET_EVENT(CAM_EVT_CAPTURE);
                return;
            }
        #endif

            if (cam_context_p->continue_capture == KAL_FALSE)
                cam_context_p->multishot_info = MULTISHOT_FALSE;
            else if (cam_context_p->continue_capture == KAL_TRUE && is_first_capture)
                cam_context_p->multishot_info = MULTISHOT_FIRST;
            else if (cam_context_p->continue_capture == KAL_TRUE)
                cam_context_p->multishot_info = MULTISHOT_NOT_FIRST;
            
            buffer_address = cam_context_p->capture_buffer_p;
            buffer_size = cam_context_p->capture_buffer_size;

            cam_context_p->jpeg_buffer_address = cam_context_p->capture_buffer_p;
            cam_context_p->jpeg_buffer_size = cam_context_p->capture_buffer_size;
        }
        else if (req_p->media_mode == MED_MODE_ARRAY)
        {
            cam_context_p->app_capture_buffer_p = (kal_uint32)req_p->app_capture_buffer_p;
            cam_context_p->app_capture_buffer_size = req_p->app_capture_buffer_size;

            cam_context_p->multishot_info = MULTISHOT_FIRST;
            
            if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BEST_SHOT)
            {
                cam_context_p->bss_buffer1_p = cam_context_p->app_capture_buffer_p;
                cam_context_p->bss_buffer1_size = cam_context_p->app_capture_buffer_size >> 1;

                cam_context_p->bss_buffer2_p = cam_context_p->bss_buffer1_p + cam_context_p->bss_buffer1_size;
                cam_context_p->bss_buffer2_size = cam_context_p->bss_buffer1_size;

                buffer_address = cam_context_p->jpeg_buffer_address = cam_context_p->bss_buffer1_p;
                buffer_size = cam_context_p->jpeg_buffer_size = cam_context_p->bss_buffer1_size;
            }
            else
            {
                buffer_address = cam_context_p->app_capture_buffer_p;
                buffer_size =  cam_context_p->app_capture_buffer_size;

                cam_context_p->jpeg_buffer_address = cam_context_p->app_capture_buffer_p;
                cam_context_p->jpeg_buffer_size = cam_context_p->app_capture_buffer_size;
            }
        }

        if (cam_context_p->image_resolution == CAM_IMAGE_SIZE_WALLPAPER)
            cam_context_p->extmem_size = MAX_CAM_CAPTURE_LCD_ISP_WORK_BUF_SIZE;
        else    
            cam_context_p->extmem_size = MAX_CAM_CAPTURE_ISP_WORK_BUF_SIZE;

        if (cam_context_p->extmem_size)
        {
            cam_context_p->extmem_start_address = (kal_uint32) med_alloc_asm_mem_framebuffer(cam_context_p->app_id, cam_context_p->extmem_size);
            CAM_CAPTURE_CHECK_IS_BUFFER_AVAILABLE(cam_context_p->extmem_start_address);
        }

        if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_PANO_VIEW)
        {
        #if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
            // set direction of panorama
            ASSERT(cam_context_p->stitch_direction < CAM_NO_OF_PANO_DIR);
            
            CalFeaturePara.FeatureId = CAL_FEATURE_CAMERA_PANO_DIRECTION;
            CalFeaturePara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
            CalFeaturePara.FeatureSetValue = (kal_uint32)cam_context_p->stitch_direction;
            CalCtrl(CAL_FEATURE_CTRL, (void*)&CalFeaturePara, NULL, 0, NULL);

            cam_context_p->multishot_info = MULTISHOT_FALSE;
        #endif            
        }
        else
        {
            cam_context_p->thumbnail_buffer_size = MAX_CAM_CAPTURE_THUMBNAIL_BUF_SIZE;
            cam_context_p->thumbnail_buffer_address = (kal_uint32) med_alloc_asm_mem_framebuffer(cam_context_p->app_id, cam_context_p->thumbnail_buffer_size);
            CAM_CAPTURE_CHECK_IS_BUFFER_AVAILABLE(cam_context_p->thumbnail_buffer_address);            
        }
        
        ret = cam_capture_still_image_start(buffer_address, buffer_size);

        if (ret != MM_ERROR_NONE)
        {
            CAM_ENTER_STATE(CAM_READY);
            cam_free_working_buffer();
            cam_set_result(MED_RES_ERROR);
            CAM_SET_EVENT(CAM_EVT_CAPTURE);
        }
        else
        {
            if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BURST_SHOT)
            {
                cam_set_result(MED_RES_OK);
                CAM_SET_EVENT(CAM_EVT_CAPTURE);
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  cam_get_encoded_image_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera capture request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_encode_capture_image_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_encode_image_req_struct *req_p;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req_p = (media_cam_encode_image_req_struct*)ilm_ptr->local_para_ptr;

    if (cam_context_p->media_mode != MED_MODE_BUFFER)
    {
        cam_set_result(MED_RES_ERROR);
        CAM_SET_EVENT(CAM_EVT_ENCODE_JPEG);
        return;
    }
    
    if (CAM_IN_STATE(CAM_READY) && cam_context_p->is_main_jpeg_ready)
    {
        if (cam_context_p->is_main_jpeg_ready)
        {
            *req_p->buf_pp = (kal_uint8*)cam_context_p->jpeg_image_address;
            *req_p->buf_size_p = cam_context_p->jpeg_image_size;
            cam_set_result(MED_RES_OK);
        }
        else
        {
            *req_p->buf_pp = NULL;
            *req_p->buf_size_p = 0;
            cam_set_result(MED_RES_ERROR);
        }
        
        CAM_SET_EVENT(CAM_EVT_ENCODE_JPEG);
    }
    else if (CAM_IN_STATE(CAM_CAPTURE))
    {
        //Wait the main image callback
        cam_context_p->capture_buffer_pp = (void**)req_p->buf_pp;
        cam_context_p->capture_buffer_size_p = req_p->buf_size_p;
    }
    else
    {
        ASSERT(0);
    }
}


kal_uint8 cam_bestshot_compare_result(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    CAL_FEATURE_CTRL_STRUCT CalInPara, CalOutPara;
    CAL_CAMERA_GET_BSS_RESULT_IN_STRUCT BestShotIn;
    CAL_CAMERA_GET_BSS_RESULT_OUT_STRUCT BestShotOut;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CalInPara.FeatureId = CAL_FEATURE_CAMERA_GET_BSS_RESULT;
    CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;
    CalInPara.FeatureType = CAL_FEATURE_TYPE_STRUCTURED;
    CalInPara.FeatureInfo.FeatureStructured.pCamGetBssInPara = &BestShotIn;

    BestShotIn.SnapShotIndex = cam_context_p->captured_number - 1;
    BestShotIn.JpegSize = cam_context_p->jpeg_image_size;

    CalOutPara.FeatureInfo.FeatureStructured.pCamGetBssOutPara = &BestShotOut;
    CalCtrl(CAL_FEATURE_CTRL, &CalInPara, &CalOutPara, sizeof(CalOutPara), NULL);

    return CalOutPara.FeatureInfo.FeatureStructured.pCamGetBssOutPara->BestShotIndex;
}


void cam_get_fd_result_req_hdlr(ilm_struct* ilm_ptr)
{
#if defined(__MED_CAM_FD_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_get_fd_result_struct *req_p;
    media_cam_fd_result_struct *fd_result_p;
    kal_int8 i;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;
    if (CAM_IN_STATE(CAM_PREVIEW) && cam_context_p->fd_enable == KAL_TRUE)
    {
        req_p = (media_cam_get_fd_result_struct*) ilm_ptr->local_para_ptr;
        fd_result_p = req_p->fd_result_p;
        
        if (cam_context_p->sd_enable)
        {
        #if defined(__SMILE_SHUTTER_SUPPORT__)
            fd_result_p->face_no = cam_context_p->sd_result.face_no;
            fd_result_p->smile_detected_no = cam_context_p->sd_result.smile_detected_no;
            fd_result_p->sd_end_flag = cam_context_p->sd_result.sd_end_flag;
            if (fd_result_p->smile_detected_no >0 || (fd_result_p->sd_end_flag == KAL_TRUE))
            {
                kal_trace(TRACE_GROUP_1,CAM_FD_GET_RESULT_1,fd_result_p->smile_detected_no ,fd_result_p->sd_end_flag);
            }

            for (i=0; i<fd_result_p->face_no; i++)
            {
                ASSERT (i<MAX_CAM_FD_NO);

                fd_result_p->rect_x0[i]  = cam_context_p->sd_result.rect_x0[i];
                fd_result_p->rect_x1[i]  = cam_context_p->sd_result.rect_x1[i];
                fd_result_p->rect_y0[i]  = cam_context_p->sd_result.rect_y0[i];
                fd_result_p->rect_y1[i]  = cam_context_p->sd_result.rect_y1[i];
            }

            for (i=0; i<fd_result_p->smile_detected_no; i++)
            {
                fd_result_p->sd_rect_x0[i] = cam_context_p->sd_result.sd_rect_x0[i];
                fd_result_p->sd_rect_y0[i] = cam_context_p->sd_result.sd_rect_y0[i];
                fd_result_p->sd_rect_x1[i] = cam_context_p->sd_result.sd_rect_x1[i];
                fd_result_p->sd_rect_y1[i] = cam_context_p->sd_result.sd_rect_y1[i];
            }    
        #endif /* defined(__SMILE_SHUTTER_SUPPORT__) */   
        }
        else
        {
            fd_result_p->face_no = cam_context_p->fd_result.face_no;

            for (i=0; i<fd_result_p->face_no; i++)
            {
                ASSERT (i<MAX_CAM_FD_NO);

                fd_result_p->rect_x0[i]  = cam_context_p->fd_result.rect_x0[i];
                fd_result_p->rect_x1[i]  = cam_context_p->fd_result.rect_x1[i];
                fd_result_p->rect_y0[i]  = cam_context_p->fd_result.rect_y0[i];
                fd_result_p->rect_y1[i]  = cam_context_p->fd_result.rect_y1[i];
            }
        }
    }
    else
    {
        ASSERT(0);
    }
#endif /* __MED_CAM_FD_SUPPORT__ */

    CAM_SET_EVENT(CAM_EVT_BLOCK);
}


void cam_set_fd_id_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}


void cam_unset_fd_id_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}


/*****************************************************************************
 * FUNCTION
 *  cam_start_fd_req_hdlr
 * DESCRIPTION
 *  This function is to resume fd process
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_resume_fd_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    CAM_SET_EVENT(CAM_EVT_SET_PARAM);
}


/*****************************************************************************
 * FUNCTION
 *  cam_pause_fd_req_hdlr
 * DESCRIPTION
 *  This function is to pause fd process
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_pause_fd_req_hdlr(ilm_struct *ilm_ptr)
{
    CAM_SET_EVENT(CAM_EVT_SET_PARAM);
}


/*****************************************************************************
 * FUNCTION
 *  cam_set_cap_resolution_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera set cap resolution request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_set_cap_resolution_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_cap_resolution_struct *req_p;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    MM_ERROR_CODE_ENUM ret;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_cap_resolution_struct*) ilm_ptr->local_para_ptr;

    cam_context_p->image_resolution = req_p->image_resolution;
    cam_context_p->image_width  = req_p->image_width;
    cam_context_p->image_height = req_p->image_height;

    CalInPara.FeatureId = CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE;
    CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
    CalInPara.FeatureSetValue = cam_context_p->image_resolution;
    ret = CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, NULL, 0, NULL); 

    cam_set_result((kal_int32)ret);      

    CAM_SET_EVENT(CAM_EVT_SET_PARAM);
}


/*****************************************************************************
 * FUNCTION
 *  cam_set_param_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera set parameter request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_set_param_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_set_param_req_struct *req_p;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    MM_ERROR_CODE_ENUM ret;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_set_param_req_struct*) ilm_ptr->local_para_ptr;

    if (CAM_IN_STATE(CAM_PREVIEW) || CAM_IN_STATE(CAM_CAPTURE) || CAM_IN_STATE(CAM_CAPTURE_BARCODE))
    {
        CalInPara.FeatureId = req_p->param_id;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureSetValue = req_p->value;
        ret = CalCtrl(CAL_FEATURE_CTRL, (void*)&CalInPara, NULL, 0, NULL); 

        if (req_p->param_id == CAL_FEATURE_CAMERA_SMILE_DETECT_START)
        {
            cam_context_p->sd_enable = (kal_bool)req_p->value;
        }

        cam_set_result((kal_int32)ret);      
    }
    else
    {
        cam_set_result(MED_RES_FAIL);
    }

    CAM_SET_EVENT(CAM_EVT_SET_PARAM);

}


/*****************************************************************************
 * FUNCTION
 *  cam_set_flash_req_hdlr
 * DESCRIPTION
 *  This function is to handle camera set flash request.
 * PARAMETERS
 *  ilm_ptr     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_set_flash_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_set_flash_req_struct *req_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->src_mod = ilm_ptr->src_mod_id;

    req_p = (media_cam_set_flash_req_struct*) ilm_ptr->local_para_ptr;

}


/*****************************************************************************
 * FUNCTION
 *  cam_set_overlay_palette_req_hdlr
 * DESCRIPTION
 *  This function is to handle set overlay palette request.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_set_overlay_palette_req_hdlr(ilm_struct *ilm_ptr)
{
#if (MT6228_SERIES)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_set_overlay_palette_req_struct *msg_p =
        (media_cam_set_overlay_palette_req_struct*) ilm_ptr->local_para_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(DRV_IDP_OLD_DESIGN)
    set_overlay_palette(msg_p->palette_size, msg_p->palette_addr);
#else
    ASSERT(0);
#endif

#endif /* MT6228_SERIES */
    CAM_SET_EVENT(CAM_EVT_SET_PAL);
}


/*****************************************************************************
 * FUNCTION
 *  cam_stitch_result_ind_hdlr
 * DESCRIPTION
 *  This function is to handle finish stitch panorama process indication from media-v
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_stitch_result_ind_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* not reach here */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
}


/*****************************************************************************
 * FUNCTION
 *  cam_start_stitch_req_hdlr
 * DESCRIPTION
 *  This function is to handle Start stitch Panorama Process request from MMI,
 *  it will send mesg to MED_V and wait indication from MED_V, then set event.
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_start_stitch_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_start_stitch_req_struct *req_p;
    kal_uint32 buffer_address = NULL;
    kal_uint32 buffer_size = 0;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req_p = (media_cam_start_stitch_req_struct*)ilm_ptr->local_para_ptr;
    
    cam_context_p->stitch_seq_no = req_p->seq_no;

    cam_context_p->jpeg_buffer_address = req_p->target_buffer_address;
    cam_context_p->jpeg_buffer_size = req_p->target_buffer_size;
    cam_context_p->quickview_output = req_p->quickview_output;
    cam_context_p->quickview_output_buffer_address = req_p->quickview_output_buffer_address;
    cam_context_p->quickview_output_buffer_size = req_p->quickview_output_buffer_size;
    cam_context_p->quickview_output_width = req_p->quickview_output_width;
    cam_context_p->quickview_output_height = req_p->quickview_output_height;
    
    if (CAM_IN_STATE(CAM_READY) && !cam_context_p->is_bg_stitching)
    {
        req_p = (media_cam_start_stitch_req_struct*) ilm_ptr->local_para_ptr;

        CAM_FREE_STITCH_BUFFER();
        CAM_FREE_CAPTURE_BUFFER();

        if (cam_context_p->stitch_app_buffer_address)
        {
            buffer_address = cam_context_p->stitch_app_buffer_address;
            buffer_size = cam_context_p->stitch_app_buffer_size;            
        }
        else
        {
            cam_context_p->stitch_buffer_size = MED_CAM_PANORAMA_STITCH_SIZE;
            cam_context_p->stitch_buffer_address = (kal_uint32)med_alloc_asm_mem_framebuffer(
                                                                    cam_context_p->app_id, 
                                                                    cam_context_p->stitch_buffer_size);

            buffer_address = cam_context_p->stitch_buffer_address;
            buffer_size = cam_context_p->stitch_buffer_size;          
        }
        cam_context_p->is_stitch_done = KAL_FALSE;
        cam_panorama_start_stitch(buffer_address, buffer_size);

        cam_context_p->current_shot_number = 0;
        /* enter STITCH state */
        CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_FINAL);
    }
    else
    {
        cam_context_p->is_waiting_stitch = KAL_TRUE;
    }
    /* set event */
    CAM_SET_EVENT(CAM_EVT_BLOCK);

#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* not reach here */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
}


/*****************************************************************************
 * FUNCTION
 *  cam_stop_stitch_req_hdlr
 * DESCRIPTION
 *  This function is to handle Stop stitch Panorama Process request from MMI,
 *  it will send mesg to MED_V and wait indication from MED_V, then set event.
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_stop_stitch_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_BG) || CAM_PANO_IN_STATE(CAM_PANO_STITCH_BG_PAUSE))
    {
        cam_panorama_stop_bg_stitch();
        CAM_PANO_ENTER_STATE(CAM_PANO_IDLE);
    }
    else if(CAM_PANO_IN_STATE(CAM_PANO_STITCH_FINAL) || CAM_IN_STATE(CAM_PANO_STITCH_PAUSE))
    {
        cam_panorama_stop_stitch();
        cam_context_p->stitch_seq_no++;
        
        CAM_PANO_ENTER_STATE(CAM_PANO_IDLE);
    }

    cam_context_p->is_waiting_stitch = KAL_FALSE;
    cam_context_p->is_stitch_done = KAL_FALSE;
    cam_context_p->current_shot_number = 0;
    cam_context_p->stitch_app_buffer_address = NULL;
    cam_context_p->stitch_app_buffer_size = 0;
    CAM_FREE_STITCH_BUFFER();
    
    CAM_SET_EVENT(CAM_EVT_BLOCK);

#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* not reach here */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
}



/*****************************************************************************
 * FUNCTION
 *  cam_reset_panorama_3a_req_hdlr
 * DESCRIPTION
 *  This function is to handle Stop stitch Panorama Process request from MMI,
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_reset_panorama_3a_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/  
    if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_BG) || CAM_PANO_IN_STATE(CAM_PANO_STITCH_BG_PAUSE))
    {
        cam_panorama_stop_bg_stitch();
        CAM_PANO_ENTER_STATE(CAM_PANO_IDLE);
    }
    else if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_FINAL) || CAM_PANO_IN_STATE(CAM_PANO_STITCH_PAUSE))
    {
        cam_panorama_stop_stitch();
        cam_context_p->stitch_seq_no++;

        CAM_PANO_ENTER_STATE(CAM_PANO_IDLE);
    }
    
    cam_context_p->is_waiting_stitch = KAL_FALSE;
    cam_context_p->is_stitch_done = KAL_FALSE;
    cam_context_p->current_shot_number = 0;
    cam_context_p->stitch_app_buffer_address = NULL;
    cam_context_p->stitch_app_buffer_size = 0;

    CAM_FREE_STITCH_BUFFER();
    
    CAM_SET_EVENT(CAM_EVT_BLOCK);

#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* not reach here */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
}


/*****************************************************************************
 * FUNCTION
 *  cam_pause_stitch_req_hdlr
 * DESCRIPTION
 *  This function is to pause stitch process.
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_pause_stitch_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalStitchScenarioPara;
    CAL_CAMERA_POSTPROC_STRUCT CalPostProcPara;
    CAL_POSTPROC_STRUCT PostProcPara;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    #ifdef __CAM_TRACE_ON__
    kal_trace(TRACE_GROUP_2, CAM_STITCH_PAUSE, cam_context_p->state);
    #endif
    
    if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_FINAL))
    {
        PostProcPara.PostProcMode = PP_MODE_PANORAMA;
        CalPostProcPara.pPostProcPara = &PostProcPara;
        
        CalStitchScenarioPara.ScenarioId = CAL_SCENARIO_POST_PROCESSING;
        CalStitchScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_PAUSE;
        CalStitchScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_POSTPROC_STRUCT);
        CalStitchScenarioPara.pScenarioCtrlPara = &CalPostProcPara;
            
        CalCtrl(CAL_SCENARIO_CTRL, (void *)&CalStitchScenarioPara, NULL, 0, NULL);
        
        CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_PAUSE);
        cam_set_result(MED_RES_OK);
    }
    else if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_BG))
    {
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_BG_PANO_ADD_IMAGE_PAUSE;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalCtrl(CAL_FEATURE_CTRL, &CalInPara, NULL, 0, NULL);
        CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_BG_PAUSE);
        cam_set_result(MED_RES_OK);
    }
    else
    {
        cam_set_result(MED_RES_FAIL);
    }
    
    CAM_FREE_STITCH_BUFFER();
    CAM_SET_EVENT(CAM_EVT_BLOCK);
#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* should not reach here! */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
}


/*****************************************************************************
 * FUNCTION
 *  cam_resume_stitch_req_hdlr
 * DESCRIPTION
 *  This function is to pause stitch process.
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_resume_stitch_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalStitchScenarioPara;
    CAL_CAMERA_POSTPROC_STRUCT CalPostProcPara;
    CAL_POSTPROC_STRUCT PostProcPara;
    kal_uint32 buffer_address = NULL;
    kal_uint32 buffer_size = 0;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    CAL_FEATURE_CAMERA_PANO_ADD_IMAGE_STRUCT PanoAddImg;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    #ifdef __CAM_TRACE_ON__
    kal_trace(TRACE_GROUP_2, CAM_STITCH_RESUME, cam_context_p->state);
    #endif

    if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_PAUSE) && !cam_context_p->is_bg_stitching)
    {
        if (cam_context_p->is_stitch_done == KAL_TRUE)
        {
            cam_send_stitch_result_ind(cam_context_p->stitch_seq_no, 
                                       cam_context_p->stitch_result,
                                       cam_context_p->stitch_jpeg_file_address, 
                                       cam_context_p->stitch_jpeg_file_size,
                                       cam_context_p->stitch_jpeg_file_width,
                                       cam_context_p->stitch_jpeg_file_height);
            cam_context_p->is_stitch_done = KAL_FALSE;
            CAM_PANO_ENTER_STATE(CAM_PANO_IDLE);
        }
        else
        {
            if (cam_context_p->stitch_app_buffer_address)
            {
                buffer_address = cam_context_p->stitch_app_buffer_address;
                buffer_size = cam_context_p->stitch_app_buffer_size;            
            }
            else
            {
                cam_context_p->stitch_buffer_size = MED_CAM_PANORAMA_STITCH_SIZE;
                cam_context_p->stitch_buffer_address = (kal_uint32)med_alloc_asm_mem_framebuffer(
                                                                        cam_context_p->app_id, 
                                                                        cam_context_p->stitch_buffer_size);
                buffer_address = cam_context_p->stitch_buffer_address;
                buffer_size = cam_context_p->stitch_buffer_size;          
            }
        
            PostProcPara.PostProcMode = PP_MODE_PANORAMA;
            CalPostProcPara.pPostProcPara = &PostProcPara;
            CalPostProcPara.ExtmemStartAddress = buffer_address;
            CalPostProcPara.ExtmemSize  = buffer_size;
    
            CalStitchScenarioPara.ScenarioId = CAL_SCENARIO_POST_PROCESSING;
            CalStitchScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_RESUME;
            CalStitchScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_POSTPROC_STRUCT);
            CalStitchScenarioPara.pScenarioCtrlPara = &CalPostProcPara;
                
            CalCtrl(CAL_SCENARIO_CTRL, (void *)&CalStitchScenarioPara, NULL, 0, NULL);
            CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_FINAL);
        }
        cam_set_result(MED_RES_OK);
    }
    else if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_BG_PAUSE))
    {
        // Add image for bg stitch
        if (cam_context_p->stitch_app_buffer_address)
        {
            PanoAddImg.ExtmemStartAddr = cam_context_p->stitch_app_buffer_address;
            PanoAddImg.ExtmemSize = cam_context_p->stitch_app_buffer_size;            
        }
        else
        {
            if (cam_context_p->stitch_buffer_address == NULL)
            {
                cam_context_p->stitch_buffer_size = MED_CAM_BG_STITCH_EXT_MEM_SIZE;
                cam_context_p->stitch_buffer_address = (kal_uint32)med_alloc_asm_mem_framebuffer(
                                                                        cam_context_p->app_id, 
                                                                        cam_context_p->stitch_buffer_size);
            }

            PanoAddImg.ExtmemStartAddr = cam_context_p->stitch_buffer_address;
            PanoAddImg.ExtmemSize = cam_context_p->stitch_buffer_size;
        }

        PanoAddImg.CurrentShotNumber = cam_context_p->current_shot_number;
        PanoAddImg.SrcImgFormat = cam_context_p->stitch_image_format;
        PanoAddImg.StitchDirection = cam_context_p->stitch_direction;
        PanoAddImg.ImageWidth = cam_context_p->image_width;
        PanoAddImg.ImageHeight = cam_context_p->image_height;
        PanoAddImg.ImageBufferAddr = cam_context_p->stitch_image_address;
        PanoAddImg.ImageBufferSize = cam_context_p->stitch_image_size;
        
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_BG_PANO_ADD_IMAGE_RESUME;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureType = CAL_FEATURE_TYPE_STRUCTURED;
        CalInPara.FeatureInfo.FeatureStructured.pPanoAddImgPara = &PanoAddImg;

        CalCtrl(CAL_FEATURE_CTRL, &CalInPara, NULL, 0, NULL);
        CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_BG);
        cam_set_result(MED_RES_OK);
    }
    else
    {
        cam_set_result(MED_RES_FAIL);
    }

    CAM_SET_EVENT(CAM_EVT_BLOCK);
#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* should not reach here! */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    
}


/*****************************************************************************
 * FUNCTION
 *  cam_set_stitch_mem_hdlr
 * DESCRIPTION
 *  This function is to handle set stitch memory.
 * PARAMETERS
 *  ilm_ptr     [in]  ilm struct
 * RETURNS
 *  void
 *****************************************************************************/
void cam_set_stitch_mem_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_set_stitch_mem_req_struct *req_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (CAM_IN_STATE(CAM_READY) || CAM_IN_STATE(CAM_PREVIEW))
    {
        req_p = (media_cam_set_stitch_mem_req_struct*) ilm_ptr->local_para_ptr;

        if (req_p->buffer_size >= MED_CAM_PANORAMA_STITCH_SIZE)
        {
            cam_context_p->stitch_app_buffer_address = req_p->buffer_address;
            cam_context_p->stitch_app_buffer_size = req_p->buffer_size;
        }
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        ASSERT(0);
    }
    CAM_SET_EVENT(CAM_EVT_BLOCK);
#else /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
    /* should not reach here! */
    ASSERT(0);
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */
}


/*****************************************************************************
 * FUNCTION
 *  cam_quickview_event_ind_hdlr
 * DESCRIPTION
 *  This function is to handle camera quickview event indication.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_quickview_event_ind_hdlr(media_cam_event_ind_struct* ind_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cam_context_p->media_mode == MED_MODE_BUFFER &&
        cam_context_p->quickview_output)
    {
        cam_set_result(ind_p->cause);
        CAM_SET_EVENT(CAM_EVT_CAPTURE);        
    }
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_image_ready_ind_hdlr
 * DESCRIPTION
 *  This function is to handle camera panorama event indication.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_panorama_image_ready_ind_hdlr(media_cam_event_ind_struct* ind_p)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    CAL_FEATURE_CAMERA_PANO_ADD_IMAGE_STRUCT PanoAddImg;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_capture_still_image_stop();
    
    if (cam_context_p->stitch_image_address)
    {
        cam_context_p->current_shot_number++;
        
        if (cam_context_p->capture_buffer_pp && cam_context_p->capture_buffer_size_p)
        {
            *cam_context_p->capture_buffer_pp = (void*)cam_context_p->stitch_image_address;
            *cam_context_p->capture_buffer_size_p = cam_context_p->stitch_image_size;
        }
        cam_set_result(MED_RES_OK);
        
        // Add image for bg stitch
        if (cam_context_p->stitch_app_buffer_address)
        {
            PanoAddImg.ExtmemStartAddr = cam_context_p->stitch_app_buffer_address;
            PanoAddImg.ExtmemSize = cam_context_p->stitch_app_buffer_size;            
        }
        else
        {
            if (cam_context_p->stitch_buffer_address == NULL)
            {
                cam_context_p->stitch_buffer_size = MED_CAM_BG_STITCH_EXT_MEM_SIZE;
                cam_context_p->stitch_buffer_address = (kal_uint32)med_alloc_asm_mem_framebuffer(
                                                                        cam_context_p->app_id, 
                                                                        cam_context_p->stitch_buffer_size);
            }

            PanoAddImg.ExtmemStartAddr = cam_context_p->stitch_buffer_address;
            PanoAddImg.ExtmemSize = cam_context_p->stitch_buffer_size;
        }

        PanoAddImg.CurrentShotNumber = cam_context_p->current_shot_number;
        PanoAddImg.SrcImgFormat = cam_context_p->stitch_image_format;
        PanoAddImg.StitchDirection = cam_context_p->stitch_direction;
        PanoAddImg.ImageWidth = cam_context_p->image_width;
        PanoAddImg.ImageHeight = cam_context_p->image_height;
        PanoAddImg.ImageBufferAddr = cam_context_p->stitch_image_address;
        PanoAddImg.ImageBufferSize = cam_context_p->stitch_image_size;
        
        CalInPara.FeatureId = CAL_FEATURE_CAMERA_BG_PANO_ADD_IMAGE_START;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
        CalInPara.FeatureType = CAL_FEATURE_TYPE_STRUCTURED;
        CalInPara.FeatureInfo.FeatureStructured.pPanoAddImgPara = &PanoAddImg;

        CalCtrl(CAL_FEATURE_CTRL, &CalInPara, NULL, 0, NULL);
        cam_context_p->is_bg_stitching = KAL_TRUE;
        CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_BG);
    }
    else
    {
        if (cam_context_p->capture_buffer_pp && cam_context_p->capture_buffer_size_p)
        {
            *cam_context_p->capture_buffer_pp = (void*)NULL;
            *cam_context_p->capture_buffer_size_p = 0;
        }        
        cam_set_result(MED_RES_FAIL);
    }
    cam_free_working_buffer();
    CAM_ENTER_STATE(CAM_READY);
    CAM_SET_EVENT(CAM_EVT_CAPTURE);

#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */   
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_bg_stitch_ready_ind_hdlr
 * DESCRIPTION
 *  This function is to handle camera panorama event indication.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_panorama_bg_stitch_ready_ind_hdlr(media_cam_event_ind_struct* ind_p)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 buffer_address = NULL;
    kal_uint32 buffer_size = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cam_context_p->bg_stitch_count == cam_context_p->current_shot_number)
        cam_context_p->is_bg_stitching = KAL_FALSE;
    
    if (cam_context_p->is_waiting_stitch && !cam_context_p->is_bg_stitching)
    {
        CAM_FREE_STITCH_BUFFER();
        CAM_FREE_CAPTURE_BUFFER();

        if (cam_context_p->stitch_app_buffer_address)
        {
            buffer_address = cam_context_p->stitch_app_buffer_address;
            buffer_size = cam_context_p->stitch_app_buffer_size;            
        }
        else
        {
            cam_context_p->stitch_buffer_size = MED_CAM_PANORAMA_STITCH_SIZE;
            cam_context_p->stitch_buffer_address = (kal_uint32)med_alloc_asm_mem_framebuffer(
                                                                    cam_context_p->app_id, 
                                                                    cam_context_p->stitch_buffer_size);

            buffer_address = cam_context_p->stitch_buffer_address;
            buffer_size = cam_context_p->stitch_buffer_size;            
        }
        cam_context_p->is_stitch_done = KAL_FALSE;
        cam_panorama_start_stitch(buffer_address, buffer_size);

        cam_context_p->current_shot_number = 0;
        cam_context_p->is_waiting_stitch = KAL_FALSE;
        /* enter STITCH state */
        CAM_PANO_ENTER_STATE(CAM_PANO_STITCH_FINAL);   
    }

#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */   
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_stitch_ready_ind_hdlr
 * DESCRIPTION
 *  This function is to handle camera panorama event indication.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_panorama_stitch_ready_ind_hdlr(media_cam_event_ind_struct* ind_p)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
            
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cam_context_p->is_stitch_done = KAL_TRUE;
    CAM_FREE_STITCH_BUFFER();

    cam_context_p->stitch_app_buffer_address = NULL;
    cam_context_p->stitch_app_buffer_size = 0;

    if(CAM_PANO_IN_STATE(CAM_PANO_STITCH_FINAL) || CAM_IN_STATE(CAM_PANO_STITCH_PAUSE))
        cam_panorama_stop_stitch();

    #ifdef __CAM_TRACE_ON__
    kal_trace(TRACE_GROUP_2, CAM_STITCH_DONE, cam_context_p->state);
    #endif
    
    if (CAM_PANO_IN_STATE(CAM_PANO_STITCH_FINAL))
    {
        cam_send_stitch_result_ind(
            cam_context_p->stitch_seq_no, 
            cam_context_p->stitch_result,
            cam_context_p->stitch_jpeg_file_address, 
            cam_context_p->stitch_jpeg_file_size,
            cam_context_p->stitch_jpeg_file_width,
            cam_context_p->stitch_jpeg_file_height);
        
        CAM_PANO_ENTER_STATE(CAM_PANO_IDLE);
    }


#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */   
}


/*****************************************************************************
 * FUNCTION
 *  cam_capture_event_ind_hdlr
 * DESCRIPTION
 *  This function is to handle camera capture event indication.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_capture_event_ind_hdlr(media_cam_event_ind_struct* ind_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 buffer_address = 0, buffer_size = 0;
    kal_uint8 bestshot_index;
#if defined(__CAM_BSS_DEBUG_MODE__)
    FS_HANDLE file_handle;
    kal_uint32 len;
    kal_wchar file_name[512], idx[20];   
#endif 
    kal_int32 result = MED_RES_OK; 
    kal_int32 average_size = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MED_CAM_STOP_LOGGING("CPJ");
    MED_CAM_GET_STOP_TIME_AND_PRINT_TRACE(CAM_CAPTURE_JPEG_DURATION);

    med_stop_timer(CAM_CAPTURE_TIMEOUT_TIMER);
    cam_capture_still_image_stop();
    cam_context_p->is_main_jpeg_ready = KAL_TRUE;

    CAM_ENTER_STATE(CAM_READY);
    if (cam_context_p->jpeg_image_size <= 0 || ind_p->cause != MED_RES_OK)
    {
        cam_context_p->jpeg_image_size = 0;
        result = MED_RES_FAIL;
    }

    if (result == MED_RES_FAIL)
    {
        if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BURST_SHOT)
        {
            cam_send_capture_event_ind(result, (kal_uint16) cam_context_p->captured_number);
        }
        else if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_EV_BRACKETING ||
                 cam_context_p->capture_mode == CAM_STILL_CAPTURE_PANO_VIEW ||
                 cam_context_p->capture_mode == CAM_STILL_CAPTURE_AUTO_RAMA ||
                 cam_context_p->capture_mode == CAM_STILL_CAPTURE_BEST_SHOT)
        {
            cam_set_result(result);
            CAM_SET_EVENT(CAM_EVT_CAPTURE);
        }
        else
        {
            if (cam_context_p->capture_buffer_pp != NULL)
            {
                cam_set_result(result);
                CAM_SET_EVENT(CAM_EVT_ENCODE_JPEG);
            }
            else if (cam_context_p->is_waiting_save)
            {
                cam_save_req_hdlr(NULL);
                cam_set_result(result);
            }
            else if (!cam_context_p->quickview_output)
            {
                cam_set_result(result);
                CAM_SET_EVENT(CAM_EVT_CAPTURE);
            }
        }
        
        cam_free_working_buffer();
        return;
    }

    if (cam_context_p->jpeg_file_info_p)
    {
        cam_context_p->jpeg_file_info_p->file_address[cam_context_p->captured_number - 1] = cam_context_p->jpeg_image_address;
        cam_context_p->jpeg_file_info_p->file_size[cam_context_p->captured_number - 1] = cam_context_p->jpeg_image_size;
    }

    if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BURST_SHOT)
    {   
        cam_send_capture_event_ind(MED_RES_OK, (kal_uint16) cam_context_p->captured_number);
    }
    else if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_EV_BRACKETING)
    {
        cam_context_p->expo_bracket_level++;
    }
    else if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BEST_SHOT)
    {
    #if defined(__CAM_BSS_DEBUG_MODE__)
        kal_wsprintf(idx, ".%d.jpg", cam_context_p->captured_number - 1);
        kal_wstrcpy(file_name, cam_context_p->file_name_p);
        kal_wstrcat(file_name,idx);

        file_handle = FS_Open((kal_wchar*) file_name, FS_CREATE | FS_READ_WRITE);

        if (file_handle >= FS_NO_ERROR)
        {
            result = FS_Write(file_handle, (void*)cam_context_p->jpeg_image_address, cam_context_p->jpeg_image_size, &len);
            FS_Close(file_handle);
            if ((result < FS_NO_ERROR)||(cam_context_p->jpeg_image_size != len))
            {
                FS_Delete((kal_wchar*) file_name);
            }
        }
    #endif /* __CAM_BSS_DEBUG_MODE__ */
    }
    
    if (cam_context_p->captured_number == cam_context_p->snapshot_number)
    {
        cam_free_working_buffer();
        if (cam_context_p->media_mode == MED_MODE_BUFFER)
        {
            if (cam_context_p->capture_buffer_pp != NULL)
            {
                *cam_context_p->capture_buffer_pp = (void*)cam_context_p->jpeg_image_address;
                *cam_context_p->capture_buffer_size_p = cam_context_p->jpeg_image_size;
                cam_set_result(result);
                CAM_SET_EVENT(CAM_EVT_ENCODE_JPEG);

                cam_context_p->capture_buffer_pp = NULL;
                cam_context_p->capture_buffer_size_p = NULL;
            }
            else if (cam_context_p->is_waiting_save)
            {
                cam_save_req_hdlr(NULL);
            }
            else if (!cam_context_p->quickview_output)
            {
                cam_set_result(result);
                CAM_SET_EVENT(CAM_EVT_CAPTURE);
            }
        }
        else if (cam_context_p->media_mode == MED_MODE_ARRAY)
        {
            if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_EV_BRACKETING ||
                cam_context_p->capture_mode == CAM_STILL_CAPTURE_PANO_VIEW ||
                cam_context_p->capture_mode == CAM_STILL_CAPTURE_AUTO_RAMA)
            {
                cam_set_result(result);
                CAM_SET_EVENT(CAM_EVT_CAPTURE);
            }
            else if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BEST_SHOT)
            {
                MED_CAM_GET_START_TIME(CAM_BSS_DURATION);
                bestshot_index = cam_bestshot_compare_result();
                MED_CAM_GET_STOP_TIME_AND_PRINT_TRACE(CAM_BSS_DURATION);

                if (bestshot_index == cam_context_p->captured_number - 1)
                {
                    cam_context_p->best_image_p = cam_context_p->jpeg_image_address;
                    cam_context_p->best_image_size = cam_context_p->jpeg_image_size;                    
                }

                if (cam_context_p->capture_buffer_pp && cam_context_p->capture_buffer_size_p)
                {
                    *cam_context_p->capture_buffer_pp = (void*)cam_context_p->best_image_p;
                    *cam_context_p->capture_buffer_size_p = cam_context_p->best_image_size;
                }
    
                cam_set_result(result);
                CAM_SET_EVENT(CAM_EVT_CAPTURE);
            }            
        }
    }
    else if (cam_context_p->captured_number < cam_context_p->snapshot_number)
    {
        if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BURST_SHOT || 
            cam_context_p->capture_mode == CAM_STILL_CAPTURE_EV_BRACKETING)
        {
            ASSERT(cam_context_p->jpeg_image_address >= cam_context_p->jpeg_image_address);
            cam_context_p->jpeg_buffer_size -= (cam_context_p->jpeg_image_address - cam_context_p->jpeg_buffer_address) + cam_context_p->jpeg_image_size;
            cam_context_p->jpeg_buffer_address = cam_context_p->jpeg_image_address + cam_context_p->jpeg_image_size;

            buffer_address = cam_context_p->jpeg_buffer_address;
            buffer_size = cam_context_p->jpeg_buffer_size;

            if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BURST_SHOT)
            {
                average_size = buffer_size/(cam_context_p->snapshot_number - (cam_context_p->captured_number-1));
                if (cam_context_p->jpeg_file_info_p->file_size[cam_context_p->captured_number-1] > average_size)
                {
                    if (cam_context_p->image_quality > JPEG_ENCODE_QUALITY_LOW)
                    {
                        cam_context_p->image_quality--;
                    }
                }                
            }
        }
        else if (cam_context_p->capture_mode == CAM_STILL_CAPTURE_BEST_SHOT)
        {
            MED_CAM_GET_START_TIME(CAM_BSS_DURATION);
            bestshot_index = cam_bestshot_compare_result();
            MED_CAM_GET_STOP_TIME_AND_PRINT_TRACE(CAM_BSS_DURATION);

            if (bestshot_index == cam_context_p->captured_number - 1)
            {
                cam_context_p->best_image_buffer_p = cam_context_p->jpeg_buffer_address;
                cam_context_p->best_image_p = cam_context_p->jpeg_image_address;
                cam_context_p->best_image_size = cam_context_p->jpeg_image_size;             
            }

            if (cam_context_p->best_image_buffer_p == cam_context_p->bss_buffer2_p)
            {
                cam_context_p->jpeg_buffer_address = cam_context_p->bss_buffer1_p;
                cam_context_p->jpeg_buffer_size = cam_context_p->bss_buffer1_size;                
            }
            else
            {
                cam_context_p->jpeg_buffer_address = cam_context_p->bss_buffer2_p;
                cam_context_p->jpeg_buffer_size = cam_context_p->bss_buffer2_size;
            }
            buffer_address = cam_context_p->jpeg_buffer_address;
            buffer_size = cam_context_p->jpeg_buffer_size;
        }

        if (cam_context_p->captured_number == cam_context_p->snapshot_number - 1)
            cam_context_p->multishot_info = MULTISHOT_END;
        else
            cam_context_p->multishot_info = MULTISHOT_NOT_FIRST;
        
        cam_capture_still_image_start(buffer_address, buffer_size);
    }
        
}


/*****************************************************************************
 * FUNCTION
 *  cam_event_ind_hdlr
 * DESCRIPTION
 *  This function is to handle camera event indication.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_event_ind_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_event_ind_struct *ind_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ind_p = (media_cam_event_ind_struct*) ilm_ptr->local_para_ptr;

    if ((ind_p->event == CAM_EVT_CAPTURE) && CAM_IN_STATE(CAM_CAPTURE))
    {
        cam_capture_event_ind_hdlr(ind_p);
    }
    else if ((ind_p->event == CAM_EVT_AUTOFOCUS))
    {
        cam_send_autofocus_process_done_ind(ind_p->cause, cam_context_p->af_result.AfSuccessZone);
    }
    else if ((ind_p->event == CAM_EVT_QUICKVIEW) && CAM_IN_STATE(CAM_CAPTURE))
    {
        cam_quickview_event_ind_hdlr(ind_p);
    }
    else if (ind_p->event == CAM_EVT_PANO_IMG_READY && CAM_IN_STATE(CAM_CAPTURE))
    {
        cam_panorama_image_ready_ind_hdlr(ind_p);
    }
    else if (ind_p->event == CAM_EVT_PANO_BG_STITCH_READY)
    {
        cam_panorama_bg_stitch_ready_ind_hdlr(ind_p);
    }
    else if (ind_p->event == CAM_EVT_PANO_STITCH_READY)
    {
        cam_panorama_stitch_ready_ind_hdlr(ind_p);
    }
}


/*****************************************************************************
 * FUNCTION
 *  cam_get_buffer_content_req_hdlr
 * DESCRIPTION
 *  This function is to get frame buffer content.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_get_buffer_content_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_get_buff_content_req_struct *req_p;
    CAL_CAMERA_GET_PREVIEW_IMAGE_STRUCT GetBuffPara;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    MM_IMAGE_ROTATE_ENUM IncludedAngle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (CAM_IN_STATE(CAM_PREVIEW))
    {
        if (cam_context_p->lcd_id == 0) // MainLCD
        {           
            if (cam_context_p->cam_id == CAL_CAMERA_SOURCE_MAIN)
            {
                IncludedAngle = (MM_IMAGE_ROTATE_ENUM)CAM_ABS(cam_context_p->ui_rotate - CAM_MAIN_SENSOR_ROTATE);
            }
            else
            {
                IncludedAngle = (MM_IMAGE_ROTATE_ENUM)CAM_ABS(cam_context_p->ui_rotate - CAM_SUB_SENSOR_ROTATE);
            }
        }
        else // SubLCD
        {
            if (cam_context_p->cam_id == CAL_CAMERA_SOURCE_MAIN)
            {
                IncludedAngle = (MM_IMAGE_ROTATE_ENUM)CAM_ABS(cam_context_p->ui_rotate - CAM_MAIN_SENSOR_ROTATE);
            }
            else
            {
                IncludedAngle = (MM_IMAGE_ROTATE_ENUM)CAM_ABS(cam_context_p->ui_rotate - CAM_SUB_SENSOR_ROTATE);
            }            
        }

        req_p = (media_cam_get_buff_content_req_struct*)ilm_ptr->local_para_ptr;

        CalInPara.FeatureId = CAL_FEATURE_CAMERA_GET_PREVIEW_IMAGE;
        CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;
        CalInPara.FeatureInfo.FeatureStructured.pCamGetPreviewImageInPara = &GetBuffPara;

        GetBuffPara.ImageRotAngle = (MM_IMAGE_ROTATE_ENUM)cam_context_p->preview_rotate;
        GetBuffPara.ImageBuffAddr = req_p->buffer_address;
        GetBuffPara.ImageBuffSize = req_p->buffer_size;
        GetBuffPara.ImageDataFormat = (MM_IMAGE_FORMAT_ENUM)cam_context_p->image_data_format;
        if (IncludedAngle == MM_IMAGE_ROTATE_90 || IncludedAngle == MM_IMAGE_ROTATE_270)
        {  
            GetBuffPara.ImageWidth = req_p->buffer_height;
            GetBuffPara.ImageHeight = req_p->buffer_width;
        }
        else
        {
            GetBuffPara.ImageWidth = req_p->buffer_width;
            GetBuffPara.ImageHeight = req_p->buffer_height;
        }
        
        CalCtrl(CAL_FEATURE_CTRL, &CalInPara, NULL, 0, NULL);
        cam_set_result(MED_RES_OK);
    }
    else
    {
        cam_set_result(MED_RES_FAIL);
    }
    CAM_SET_EVENT(CAM_EVT_GET_BUFF_CONTENT);
}


/*****************************************************************************
 * FUNCTION
 *  cam_get_predicted_image_size_req_hdlr
 * DESCRIPTION
 *  This function is to get predicted capture image size.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_get_predicted_image_size_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_get_predicted_image_size_req_struct *req_p;
    MM_ERROR_CODE_ENUM ret = MM_ERROR_NONE;
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    CAL_CAMERA_GET_JPEG_SIZE_ESTIMATION_STRUCT JpegSizeEstimation;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req_p = (media_cam_get_predicted_image_size_req_struct*)ilm_ptr->local_para_ptr;

    JpegSizeEstimation.ImageWidth=req_p->image_width;
    JpegSizeEstimation.ImageHeight=req_p->image_height;
    JpegSizeEstimation.JpegEncQuality=(JPEG_ENCODE_QUALITTY_ENUM)req_p->image_qty;
    JpegSizeEstimation.JpegSamplingFormat=MM_IMAGE_FORMAT_YUV422;
    CalInPara.FeatureId = CAL_FEATURE_CAMERA_GET_JPEG_SIZE_ESTIMATION;
    CalInPara.FeatureCtrlCode = CAL_FEATURE_GET_OPERATION;
    CalInPara.FeatureInfo.FeatureStructured.pGetJpegSizeEstimationPara = &JpegSizeEstimation;
    ret = CalCtrl(CAL_FEATURE_CTRL, &CalInPara, &CalInPara, sizeof(CalInPara), NULL);
    *(req_p->image_size_p)=CalInPara.FeatureCurrentValue;

    cam_set_result(ret);
    CAM_SET_EVENT(CAM_EVT_BLOCK);
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_start_stitch
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void cam_panorama_start_stitch(kal_uint32 working_buffer_address, kal_uint32 working_buffer_size)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalStitchScenarioPara;
    CAL_CAMERA_POSTPROC_STRUCT CalPostProcPara;
    CAL_POSTPROC_STRUCT PostProcPara;
    CAL_MDP_POSTPROC_STRUCT MdpPostProcPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MdpPostProcPara.QuickViewImageEnable = cam_context_p->quickview_output;
    MdpPostProcPara.QuickViewImageFormat = MM_IMAGE_FORMAT_RGB565;
    MdpPostProcPara.QuickViewImageRotAngle = MM_IMAGE_ROTATE_0;
    MdpPostProcPara.QuickViewImageWidth = cam_context_p->quickview_output_width;
    MdpPostProcPara.QuickViewImageHeight = cam_context_p->quickview_output_height;
    MdpPostProcPara.QuickViewImageBuffAddr = cam_context_p->quickview_output_buffer_address;
    
    //call CAL to do stitch
    PostProcPara.PostProcMode = PP_MODE_PANORAMA;
    CalPostProcPara.ExtmemStartAddress = working_buffer_address;
    CalPostProcPara.ExtmemSize  = working_buffer_size;
    CalPostProcPara.JpegBuffAddr = cam_context_p->jpeg_buffer_address;
    CalPostProcPara.JpegBuffSize = cam_context_p->jpeg_buffer_size;
    CalPostProcPara.pPostProcPara = &PostProcPara;
    CalPostProcPara.pMdpPara = &MdpPostProcPara;

    CalStitchScenarioPara.ScenarioId = CAL_SCENARIO_POST_PROCESSING;
    CalStitchScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_START;
    CalStitchScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_POSTPROC_STRUCT );
    CalStitchScenarioPara.pScenarioCtrlPara = & CalPostProcPara;
    CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalStitchScenarioPara, NULL, 0, NULL);
#endif
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_stop_stitch
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void cam_panorama_stop_stitch(void)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalStitchScenarioPara;
    CAL_CAMERA_POSTPROC_STRUCT CalPostProcPara;
    CAL_POSTPROC_STRUCT PostProcPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    PostProcPara.PostProcMode = PP_MODE_PANORAMA;
    CalPostProcPara.pPostProcPara = &PostProcPara;
    
    CalStitchScenarioPara.ScenarioId = CAL_SCENARIO_POST_PROCESSING;
    CalStitchScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_STOP;
    CalStitchScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_POSTPROC_STRUCT);
    CalStitchScenarioPara.pScenarioCtrlPara = &CalPostProcPara;
        
    CalCtrl(CAL_SCENARIO_CTRL, (void *)&CalStitchScenarioPara, NULL, 0, NULL);
#endif
}


/*****************************************************************************
 * FUNCTION
 *  cam_panorama_stop_bg_stitch
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void cam_panorama_stop_bg_stitch(void)
{
#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_FEATURE_CTRL_STRUCT CalInPara;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CalInPara.FeatureId = CAL_FEATURE_CAMERA_BG_PANO_ADD_IMAGE_STOP;
    CalInPara.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
    CalCtrl(CAL_FEATURE_CTRL, &CalInPara, NULL, 0, NULL);
#endif
}


void cam_profiling_set_active_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MTK_INTERNAL__)
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
/* under construction !*/
/* under construction !*/
#endif    
}


void cam_profiling_set_buffer_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MTK_INTERNAL__)
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
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
}


void cam_profiling_log_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MTK_INTERNAL__)
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
/* under construction !*/
/* under construction !*/
#endif
}


void cam_profiling_get_append_req_hdlr(ilm_struct *ilm_ptr)
{
#if defined(__MTK_INTERNAL__)
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
/* under construction !*/
#endif
}

/* ---- SKY_MULTIMEDIA start ---- */
#if defined(__MMI_DSM_NEW__) && defined(__DSM_MEDIA_EXT_H264_ENCODE__)&& defined(__MTK_TARGET__)
void mr_media_cam_datareq_ind_hdlr(ilm_struct *ilm_ptr)
{ 
    gMrCam_datareq_ind_flag = 1;
    cam_context_p->src_mod = ilm_ptr->src_mod_id;
//#if defined(MT6253) || defined(MT6253T)|| defined(MT6238) || defined(MT6239)  || defined(MT6252) || defined(MT6236) || defined(MT6236B)
    {
	extern  mr_media_cam_GetData_Ind_Hdr(void);
	mr_media_cam_GetData_Ind_Hdr();	
    }
//#endif

}
#endif 
/* ---- SKY_MULTIMEDIA end ---- */

#else
#error "no such combination"
#endif

#endif /* __MED_CAM_MOD__ */

#endif /* MED_NOT_PRESENT */

