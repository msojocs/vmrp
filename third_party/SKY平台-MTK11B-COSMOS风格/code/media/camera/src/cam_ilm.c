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

/*******************************************************************************
 * Filename:
 * ---------
 *   cam_ilm.c
 *
 * Project:
 * --------
 *   Maui
 *
 * Description:
 * ------------
 *   This file includes send-ilm related funcions of image module.
 *
 * Author:
 * -------
 * -------
 *
 *==============================================================================
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#ifndef MED_NOT_PRESENT

/*==== INCLUDES ===========*/

/* system includes */
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h" /* Task message communiction */
#include "nvram_struct.h"
#include "nvram_data_items.h"

/* local includes */
#include "med_global.h"
#include "med_struct.h"
#include "med_context.h"
#include "cam_main.h"

#include "kal_general_types.h"
#include "stack_config.h"
#include "stack_ltlcom.h"
#include "kal_public_api.h"
#include "string.h"
    
#ifdef __MED_CAM_MOD__

static void cam_send_outgoing_ilm(module_type dest_mod_id, kal_uint16 msg_id, void* local_param_ptr);
static void cam_send_incoming_ilm(module_type src_mod_id, module_type dest_mod_id, kal_uint16 msg_id, void* local_param_ptr);

#define CONSTRUCT_LOCAL_PARAM(st, name) \
    st* name = (st*) construct_local_para(sizeof(st), TD_CTRL)

static void cam_send_outgoing_ilm(module_type dest_mod_id, kal_uint16 msg_id, void* local_param_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct* ilm = NULL;
    module_type src_mod_id;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    src_mod_id = stack_int_get_active_module_id();

    ilm = allocate_ilm(src_mod_id);
    ilm->src_mod_id = src_mod_id;
    ilm->sap_id = MED_SAP;

    ilm->msg_id = (msg_type) msg_id;
    ilm->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm->peer_buff_ptr = NULL;

    ilm->dest_mod_id = dest_mod_id;

    msg_send_ext_queue(ilm);
}


static void cam_send_incoming_ilm(module_type src_mod_id, module_type dest_mod_id, kal_uint16 msg_id, void* local_param_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_ptr = allocate_ilm(src_mod_id);
    ilm_ptr->src_mod_id = src_mod_id;
    ilm_ptr->dest_mod_id = dest_mod_id;
    ilm_ptr->sap_id = MED_SAP;

    ilm_ptr->msg_id = (msg_type) msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm_ptr->peer_buff_ptr = NULL;

    msg_send_ext_queue(ilm_ptr);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_stitch_result_ind
 * DESCRIPTION
 *  This function is to send stitch result indication to MMI task
 * PARAMETERS
 *  result [in]
 *  jepg_file_size [in]
 *  jpeg_file_width [in]
 *  jpge_file_height [in]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_stitch_result_ind(kal_uint8 seq_no, kal_int16 result, kal_uint32 jpeg_file_address,kal_uint32 jpeg_file_size, kal_uint32 jpeg_file_width, kal_uint32 jpeg_file_height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
  
    CONSTRUCT_LOCAL_PARAM(media_cam_stitch_result_ind_struct,msg_p);
    msg_p->seq_no = seq_no;
    msg_p->result = result;
    msg_p->jpeg_file_address = jpeg_file_address;
    msg_p->jpeg_file_size = jpeg_file_size;
    msg_p->jpeg_file_width = jpeg_file_width;
    msg_p->jpeg_file_height = jpeg_file_height;
    
    cam_send_outgoing_ilm(MOD_MMI,MSG_ID_MEDIA_CAM_STITCH_RESULT_IND,(void*)msg_p);
}

/*****************************************************************************
 * FUNCTION
 *  cam_send_xenon_flash_status_ind
 * DESCRIPTION
 *  This function is to send xenon flash status indication.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  status         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_xenon_flash_status_ind(kal_uint16 status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_xenon_flash_ind_struct,ind_p);

    ind_p->status = status;

    cam_send_outgoing_ilm(cam_context_p->src_mod,MSG_ID_MEDIA_CAM_XENON_FLASH_STATUS_IND,(void*)ind_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_detect_sensor_id_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *          
 *         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_set_camera_id_req(module_type src_mod_id, void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_set_camera_id_req_struct, msg_p);
    
    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_set_camera_id_req_struct) - sizeof(local_para_struct));
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SET_CAMERA_ID_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_sensor_query_capability_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *          
 *         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_sensor_query_capability_req(module_type src_mod_id, void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_sensor_query_capability_req_struct, msg_p);
    
    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_sensor_query_capability_req_struct) - sizeof(local_para_struct));
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SENSOR_QUERY_CAPBILITY_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_sensor_query_capability_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *          
 *         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_sensor_query_zoom_req(module_type src_mod_id, void *data)
{
    CONSTRUCT_LOCAL_PARAM(media_cam_zoom_capability_req_struct, msg_p);
    
    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_zoom_capability_req_struct) - sizeof(local_para_struct));
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SENSOR_QUERY_ZOOM_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_nvram_startup_req
 * DESCRIPTION
 *  This function is to send cal for reading nvram.
 * PARAMETERS
 *  ilm_ptr     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_nvram_startup_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    // send a message to CAL task to read camera NVRAM data
    ilm_ptr = allocate_ilm(MOD_MED);
    ilm_ptr->src_mod_id = MOD_MED;
    ilm_ptr->sap_id = MED_SAP;
    ilm_ptr->msg_id = MSG_ID_CAL_STARTUP_REQ;
    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;
    ilm_ptr->dest_mod_id = MOD_CAL;
    msg_send_ext_queue(ilm_ptr);    
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_power_up_req
 * DESCRIPTION
 *  This function is to send camera power up request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  seq_num         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_power_up_req(module_type src_mod_id, void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_power_up_req_struct,msg_p);

    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_power_up_req_struct) - sizeof(local_para_struct));

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_POWER_UP_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_power_down_req
 * DESCRIPTION
 *  This function is to send camera power down request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  delay_time      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_power_down_req(module_type src_mod_id, kal_int16 delay_time)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_power_down_req_struct,msg_p);

    msg_p->delay_time = delay_time;

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_POWER_DOWN_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_preview_req
 * DESCRIPTION
 *  This function is to send camera preview request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  data            [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_preview_req(module_type src_mod_id, void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    module_type dest_mod_id;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_preview_req_struct,msg_p);

    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_preview_req_struct) - sizeof(local_para_struct));

    dest_mod_id = MOD_MED;

    cam_send_incoming_ilm(src_mod_id,dest_mod_id,MSG_ID_MEDIA_CAM_PREVIEW_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_stop_req
 * DESCRIPTION
 *  This function is to send camera stop request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_stop_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_stop_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_STOP_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_capture_req
 * DESCRIPTION
 *  This function is to send camera capture request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  data            [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_capture_req(module_type src_mod_id, void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_capture_req_struct,msg_p);

    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_capture_req_struct) - sizeof(local_para_struct));

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_CAPTURE_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_encode_capture_image_req
 * DESCRIPTION
 *  This function is to send encode image request.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  param_id        [IN]        
 *  value           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_encode_capture_image_req(module_type src_mod_id, kal_uint8 **buf_pp, kal_uint32 *buf_size_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_encode_image_req_struct, msg_p);

    msg_p->buf_pp = buf_pp;
    msg_p->buf_size_p = buf_size_p;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_ENCODE_IMG_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_set_param_req
 * DESCRIPTION
 *  This function is to send set parameter request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  param_id        [IN]        
 *  value           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_set_param_req(module_type src_mod_id, kal_uint16 param_id, kal_int16 value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_set_param_req_struct,msg_p);

    msg_p->param_id = param_id;
    msg_p->value = value;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SET_PARAM_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_set_flash_req
 * DESCRIPTION
 *  This function is to send set flash request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  flash_mode      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_set_flash_req(module_type src_mod_id, kal_int16 flash_mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_set_flash_req_struct,msg_p);

    msg_p->flash_mode = flash_mode;

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SET_FLASH_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_set_cap_resolution_req
 * DESCRIPTION
 *  This function is to send set flash request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  flash_mode      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_set_cap_resolution_req(module_type src_mod_id, void *config)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_cam_cap_resolution_struct *setting_p = (media_cam_cap_resolution_struct*)config;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_cap_resolution_struct, msg_p);

    msg_p->image_resolution = setting_p->image_resolution;
    msg_p->image_width  = setting_p->image_width;
    msg_p->image_height = setting_p->image_height;

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SET_CAP_RESOLUTION_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_set_overlay_palette_req
 * DESCRIPTION
 *  This function is to send set overlay palette request message.
 * PARAMETERS
 *  src_mod_id          [IN]        
 *  palette_size        [IN]        
 *  palette_addr        [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_set_overlay_palette_req(module_type src_mod_id, kal_uint8 palette_size, kal_uint32 *palette_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_set_overlay_palette_req_struct,msg_p);

    msg_p->palette_size = palette_size;
    msg_p->palette_addr = palette_addr;

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SET_OVERLAY_PALETTE_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_ready_ind
 * DESCRIPTION
 *  This function is to send camera ready indication message.
 * PARAMETERS
 *  result      [IN]        
 *  module_type src_mod_id(?)
 *  kal_uint16 param_id(?)
 *  kal_int16 value(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_ready_ind(kal_int16 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_ready_ind_struct,ind_p);

    ind_p->result = result;
    ind_p->seq_num = cam_context_p->seq_num;

    cam_send_outgoing_ilm(cam_context_p->src_mod,MSG_ID_MEDIA_CAM_READY_IND,(void*)ind_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_preview_fail_ind
 * DESCRIPTION
 *  This function is to send camera preview fail indication message.
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_preview_fail_ind(kal_int16 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_preview_fail_ind_struct,ind_p);

    ind_p->result = result;

    cam_send_outgoing_ilm(cam_context_p->src_mod,MSG_ID_MEDIA_CAM_PREVIEW_FAIL_IND,(void*)ind_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_event_ind
 * DESCRIPTION
 *  This function is to send camera event indication message.
 * PARAMETERS
 *  event       [IN]        
 *  cause       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_event_ind(kal_uint32 event, kal_uint8 cause)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_event_ind_struct,ind_p);

    ind_p->event = event;
    ind_p->cause = cause;

    cam_send_outgoing_ilm(MOD_MED,MSG_ID_MEDIA_CAM_EVENT_IND,(void*)ind_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_capture_event_ind
 * DESCRIPTION
 *  This function is to send camera capture event indication message.
 * PARAMETERS
 *  result      [IN]        
 *  count       [IN]        
 *  kal_uint8 event(?)
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_capture_event_ind(kal_int16 result, kal_uint16 count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_capture_event_ind_struct,ind_p);

    ind_p->result = result;
    ind_p->count = count;

    cam_send_outgoing_ilm(cam_context_p->src_mod,MSG_ID_MEDIA_CAM_CAPTURE_EVENT_IND,(void*)ind_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_autofocus_process_done_ind
 * DESCRIPTION
 *  This function is to send camera autofocus event indication message.
 * PARAMETERS
 *  event       [IN]        
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_autofocus_process_done_ind(kal_uint8 result, kal_uint32 zone)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_af_ind_struct,ind_p);

    ind_p->af_search_result = result;
    ind_p->af_success_zone = zone;

    cam_send_outgoing_ilm(cam_context_p->src_mod,MSG_ID_MEDIA_CAM_AF_PROCESS_DONE_IND,(void*)ind_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_start_fast_zoom_req
 * DESCRIPTION
 *  This function is to send camera start fast zoom request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  zoom_in         [IN]        
 *  zoom_limit      [IN]        
 *  zoom_step       [IN]        
 *  zoom_speed      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_start_fast_zoom_req(
        module_type src_mod_id,
        kal_bool zoom_in,
        kal_uint8 zoom_limit,
        kal_uint8 zoom_step,
        kal_uint8 zoom_speed)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_start_fast_zoom_req_struct,msg_p);

    msg_p->zoom_in = zoom_in;
    msg_p->zoom_limit = zoom_limit;
    msg_p->zoom_step = zoom_step;
    msg_p->zoom_speed = zoom_speed;

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_START_FAST_ZOOM_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_stop_fast_zoom_req
 * DESCRIPTION
 *  This function is to send camera stop fast zoom request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_stop_fast_zoom_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_stop_fast_zoom_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_STOP_FAST_ZOOM_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_get_fast_zoom_factor_req
 * DESCRIPTION
 *  This function is to send camera get fast zoom factor request message.
 * PARAMETERS
 *  src_mod_id      [IN]        
 *  factor          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_get_fast_zoom_factor_req(module_type src_mod_id, kal_uint32 *factor, kal_uint32 *step)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_get_fast_zoom_factor_req_struct,msg_p);

    msg_p->factor = factor;
    msg_p->step = step;

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_GET_FAST_ZOOM_FACTOR_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_save_req
 * DESCRIPTION
 *  This function is to send camera save request message.
 * PARAMETERS
 *  src_mod_id      [IN]                
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_save_req(module_type src_mod_id, kal_wchar* filepath)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_save_req_struct,msg_p);
    msg_p->filepath = filepath;
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SAVE_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_get_buffer_content_req
 * DESCRIPTION
 *  This function is to send camera get buffer content request message.
 * PARAMETERS
 *  src_mod_id          [IN]        
 *  buffer_address      [IN]         
 *  buffer_size         [IN]    
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_get_buffer_content_req(module_type src_mod_id, void* data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
     
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_get_buff_content_req_struct,msg_p);

    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_get_buff_content_req_struct) - sizeof(local_para_struct));

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_GET_BUFFER_CONTENT_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_get_predicted_image_size_req
 * DESCRIPTION
 *  This function is to send camera get predicted image size request message.
 * PARAMETERS
 *  src_mod_id          [IN]        
 *  buffer_address      [IN]         
 *  buffer_size         [IN]    
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_get_predicted_image_size_req(module_type src_mod_id, void* data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
     
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_get_predicted_image_size_req_struct,msg_p);

    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) data) + sizeof(local_para_struct),
        sizeof(media_cam_get_predicted_image_size_req_struct) - sizeof(local_para_struct));

    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_GET_PREDICTED_IMAGE_SIZE_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_get_focus_zone_req
 * DESCRIPTION
 *  This function is to send camera get focus zone
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_get_focus_zone_req(module_type src_mod_id, media_cam_af_zone_para_struct* zone)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_af_get_zone_struct,msg_p);
    msg_p->zone_para_p = zone;    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_GET_FOCUS_ZONE_REQ,(void*)msg_p);
}


#if defined(__MED_CAM_FD_SUPPORT__)
/*****************************************************************************
 * FUNCTION
 *  cam_send_get_fd_result_req
 * DESCRIPTION
 *  This function is to send camera get fd result request
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_get_fd_result_req(module_type src_mod_id, media_cam_fd_result_struct* fd_result_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_get_fd_result_struct,msg_p);
    msg_p->fd_result_p = fd_result_p;    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_GET_FD_RESULT_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_pause_fd_req
 * DESCRIPTION
 *  This function is to send camera pause fd process request
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_pause_fd_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_pause_fd_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PAUSE_FD_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_resume_fd_req
 * DESCRIPTION
 *  This function is to send camera resume fd process request
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_resume_fd_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_resume_fd_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_RESUME_FD_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_set_fd_id_req
 * DESCRIPTION
 *  This function is to send camera set fd id (as focus zone) request
 * PARAMETERS
 *  src_mod_id          [IN]        
 *  fd_id               [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_set_fd_id_req(module_type src_mod_id, kal_uint32 fd_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_set_fd_id_req_struct,msg_p);
    msg_p->fd_id = fd_id;
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_SET_FD_ID_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_unset_fd_id_req
 * DESCRIPTION
 *  This function is to send camera unset fd id (as focus zone) request
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_unset_fd_id_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_unset_fd_id_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_UNSET_FD_ID_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_fd_result_ind
 * DESCRIPTION
 *  This function is to send camera face detection event indication message.
 * PARAMETERS
 *  event       [IN]        
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_fd_result_ind(media_cam_fd_result_struct* result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_fd_result_ind_struct,ind_p);
    memcpy(&(ind_p->fd_result),result, sizeof(media_cam_fd_result_struct));
    cam_send_outgoing_ilm(cam_context_p->src_mod,MSG_ID_MEDIA_CAM_FD_RESULT_IND,(void*)ind_p);
}
#endif /* defined(__MED_CAM_FD_SUPPORT__) */


#if defined(__MED_CAM_PANORAMA_VIEW_SUPPORT__)
/*****************************************************************************
 * FUNCTION
 *  cam_send_start_stitch_req
 * DESCRIPTION
 *  This function is to send camera stitch stitch panorama request to media task
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_start_stitch_req(module_type src_mod_id, media_cam_start_stitch_req_struct *panorama_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_start_stitch_req_struct,msg_p);
    kal_mem_cpy(
        ((kal_uint8*) msg_p) + sizeof(local_para_struct),
        ((kal_uint8*) panorama_data) + sizeof(local_para_struct),
        sizeof(media_cam_start_stitch_req_struct) - sizeof(local_para_struct));
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_START_STITCH_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_abort_panorama_req
 * DESCRIPTION
 *  This function is to send camera abort panorama process request to media task
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_abort_stitch_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_stop_stitch_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_STOP_STITCH_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_resume_stitch_req
 * DESCRIPTION
 *  This function is to send camera resume stitch panorama request to media task
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_resume_stitch_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_resume_stitch_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_RESUME_STITCH_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_pause_stitch_req
 * DESCRIPTION
 *  This function is to send camera pause panorama process request to media task
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_pause_stitch_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_pause_stitch_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PAUSE_STITCH_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_panorama_reset_3a_req
 * DESCRIPTION
 *  This function is to send reset panorama 3a request to media task
 * PARAMETERS
 *  src_mod_id          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_reset_panorama_3a_req(module_type src_mod_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_reset_panoarama_3a_req_struct,msg_p);
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PANORAMA_RESET_3A_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_stitch_memory_req
 * DESCRIPTION
 *  This function is to send camera stitch memory setting req
 * PARAMETERS
 *  src_mod_id          [IN]
 *  stitch_data         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_stitch_memory_req(module_type src_mod_id, media_cam_set_stitch_mem_req_struct *stitch_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_set_stitch_mem_req_struct,msg_p);
    msg_p->buffer_address = stitch_data->buffer_address;
    msg_p->buffer_size = stitch_data->buffer_size;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED, MSG_ID_MEDIA_CAM_SET_PANORAMA_STITCH_MEM_REQ,(void*)msg_p);
}
#endif /* __MED_CAM_PANORAMA_VIEW_SUPPORT__ */


/*****************************************************************************
 * FUNCTION
 *  cam_send_profiling_set_active_req
 * DESCRIPTION
 *  This function is to send camera stitch memory setting req
 * PARAMETERS
 *  src_mod_id          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_profiling_set_active_req(module_type src_mod_id, kal_bool is_active)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_profiling_set_active_req_struct, msg_p);
    msg_p->is_active = is_active;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PROFILING_SET_ACTIVE_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_profiling_set_buffer_req
 * DESCRIPTION
 *  This function is to send camera stitch memory setting req
 * PARAMETERS
 *  src_mod_id          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_profiling_set_buffer_req(module_type src_mod_id, kal_uint32 buf_address, kal_uint32 buf_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_profiling_set_buffer_req_struct, msg_p);
    msg_p->buffer_address = buf_address;
    msg_p->buffer_size = buf_size;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PROFILING_SET_BUFFER_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_profiling_log_req
 * DESCRIPTION
 *  This function is to send camera stitch memory setting req
 * PARAMETERS
 *  src_mod_id          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_profiling_log_req(module_type src_mod_id, kal_uint32 log_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_profiling_log_req_struct, msg_p);
    msg_p->log_id = log_id;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PROFILING_LOG_REQ,(void*)msg_p);
}


/*****************************************************************************
 * FUNCTION
 *  cam_send_profiling_get_append_req
 * DESCRIPTION
 *  This function is to send camera stitch memory setting req
 * PARAMETERS
 *  src_mod_id          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void cam_send_profiling_get_append_req(module_type src_mod_id, kal_uint32 buf_address)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    CONSTRUCT_LOCAL_PARAM(media_cam_profiling_get_append_req_struct, msg_p);
    msg_p->buffer_address = buf_address;
    
    cam_send_incoming_ilm(src_mod_id,MOD_MED,MSG_ID_MEDIA_CAM_PROFILING_GET_APPEND_REQ,(void*)msg_p);
}


/* ---- SKY_MULTIMEDIA start ---- */
#if defined(__MMI_DSM_NEW__) && defined(__DSM_MEDIA_EXT_H264_ENCODE__)&& defined(__MTK_TARGET__)
void mr_media_cam_send_data_req(module_type src_mod_id)
{
    media_cam_mr_data_req_struct *msg_p = NULL;
    ilm_struct *ilm_ptr = NULL;

    msg_p = (media_cam_mr_data_req_struct*)construct_local_para(sizeof(media_cam_mr_data_req_struct), TD_CTRL);

    ilm_ptr = allocate_ilm(src_mod_id);
    ilm_ptr->src_mod_id = src_mod_id;
    ilm_ptr->dest_mod_id = MOD_MED;
    ilm_ptr->sap_id = MED_SAP;
    ilm_ptr->msg_id = (msg_type)MSG_ID_MEDIA_CAM_MPCHAT_DATAREQ_IND;
    ilm_ptr->local_para_ptr = (local_para_struct*)msg_p;
    ilm_ptr->peer_buff_ptr = NULL;
    kal_prompt_trace(MOD_MED, "mr_media_cam_send_data_req");

    msg_send_ext_queue(ilm_ptr);
}

void mr_media_cam_send_data_rsp(kal_int32 pData, kal_uint32 nDataSize)
{
    media_cam_mr_data_cnf_struct *ind_p;
    ilm_struct *ilm_ptr = NULL;
    module_type src_mod_id;
	
    // send data confirm message to L4
    ind_p = (media_cam_mr_data_cnf_struct*)construct_local_para(sizeof(media_cam_mr_data_cnf_struct), TD_CTRL);
    ind_p->pData = (kal_uint8 *)pData;
    ind_p->nDataSize = nDataSize;

    src_mod_id = stack_get_active_module_id();
    if(src_mod_id != MOD_MED)
       src_mod_id = MOD_VISUAL_HISR;
	  
    ilm_ptr = allocate_ilm(src_mod_id);
    ilm_ptr->src_mod_id = src_mod_id;
    ilm_ptr->dest_mod_id = cam_context_p->src_mod;
    ilm_ptr->sap_id = MED_SAP;

    ilm_ptr->msg_id = (msg_type)MSG_ID_MEDIA_CAM_MPCHAT_DATARSP_IND;
    ilm_ptr->local_para_ptr = (local_para_struct*)ind_p;
    ilm_ptr->peer_buff_ptr = NULL;
    kal_prompt_trace(MOD_MED, "mr_media_cam_send_data_rsp");

    msg_send_ext_queue(ilm_ptr);
}

#endif 
/* ---- SKY_MULTIMEDIA end ---- */

#endif /* __MED_CAM_MOD__ */ 

#endif /* MED_NOT_PRESENT */ 

