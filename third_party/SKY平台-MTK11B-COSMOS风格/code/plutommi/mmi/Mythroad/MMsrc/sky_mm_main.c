#if (defined(__DSM_SPLAYER__)  || defined(__MMI_DSM_MEDIA_EXT__)) && defined(__MTK_TARGET__)
#include "sky_mm_def.h"
#include "gpioInc.h"
#include "Mdi_audio.h"
#include "Mdi_datatype.h"
#include "FileManagerGProt.h"
#include "Fs_type.h"
#include "Fs_func.h"
#include "DateTimeGprot.h"
#include "DateTimeType.h"
#include "GlobalConstants.h"
#include "CommonScreens.h"
#include "customer_ps_inc.h"

#include "mmi_msg_context.h"
#include "stack_ltlcom.h"

#include "mrporting.h"
#include "sky_mm_basetypes.h"
#include "sky_mm_port.h"
#if (MTK_VERSION == 0x11B1212)
#include "med_struct.h"//Allan add 20120411
#endif//Allan modify 20120411

//rwei for ctrl buffer
typedef union
{
#if defined(__DSM_SPLAYER__)
	media_vid_open_req_struct open_req_struct;
	media_vid_file_ready_ind_struct file_ready_ind_struct;
	media_vid_play_req_struct paly_req_struct;
	media_vid_seek_req_struct seek_req_struct;
	media_vid_pause_req_struct pause_req_struct;
	media_vid_stop_req_struct stop_req_struct;
	media_vid_close_req_struct close_req_strcut;
#if (MTK_VERSION < 0x11A1212)
	media_vid_get_content_desc_req_struct get_desc_req_struct;
#else
    media_vid_get_frame_req_struct get_frame_req_struct;
#endif//Allan modify 20120411
#endif
#if defined(__DSM_CAM_ENC_SUPPORT__) 
	l4aud_media_play_req_struct l4audio_play_req_struct;
#endif
	int dummy;
}SKY_MM_MSG_LEN;

int SPlayer_get_MsgLen(void)
{
	return sizeof(SKY_MM_MSG_LEN);
}
SKY_MM_MSG_LEN g_Sky_mm_Msg = {0};

#if defined(__DSM_SPLAYER__)
#include "sky_mm_SPlayer_api.h"

#define STATUS_OPEN   (1<<1)
#define STATUS_PLAY   (1<<2)
#define STATUS_SEEK   (1<<3)
#define STATUS_PAUSE  (1<<4)
#define STATUS_STOP   (1<<5)
#define STATUS_CLOSE  (1<<6)
#define STATUS_NULL   (1<<7)

extern unsigned int g_splayer_result;
void SPlayer_main_task(task_entry_struct * task_entry_ptr,ilm_struct_cpy_t   *current_ilm);
extern void SkyMMSetDisplayProtocol(void);
#endif

static kal_bool Sky_Multimedia_task_init(task_indx_type task_indx)
{
	#if defined(__DSM_CAM_ENC_SUPPORT__) 
		MPchat_task_init();
	#endif


	return KAL_TRUE;
}
void Sky_Multimedia_main_task(task_entry_struct * task_entry_ptr)
{
   	ilm_struct_cpy_t   current_ilm;
	kal_uint32   my_index;
	ilm_struct   rec_ilm;
	void *pMsgBuffer = (void *)&g_Sky_mm_Msg;
 	
      kal_get_my_task_index(&my_index);
	#if defined(__DSM_SPLAYER__)
		g_splayer_result = SPLAYER_NULL;
		SPLAYER_SET_EVENT(SPLAYER_EVT_IDLE);
		#if defined(__SKYMM__DISPLAY_IN_MMI__)
		SkyMMSetDisplayProtocol();//rwei
		#endif
	#endif
	
	// 
   	while ( 1 ) 
	{
	#if defined(__DSM_CAM_ENC_SUPPORT__) 
		l4aud_media_play_req_struct * msg_p = NULL;
	#endif//Allan modify 20120411
		receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &rec_ilm);
        ilm_struct_2_ilm_struct_cpy((ilm_struct_cpy_t*)&current_ilm,(ilm_struct *)&rec_ilm);
		memcpy(pMsgBuffer,(void *)rec_ilm.local_para_ptr,sizeof(SKY_MM_MSG_LEN));
		current_ilm.local_para_ptr = pMsgBuffer;
		free_ilm(&rec_ilm);

		#ifdef __DSM_SPLAYER__
		if(current_ilm.msg_id >= MSG_ID_SPLAYER_BEGIN && current_ilm.msg_id <= MSG_ID_SPLAYER_END) 
		{
			SPlayer_main_task(task_entry_ptr,&current_ilm);
		}
		else
		#endif
		#if defined(__DSM_CAM_ENC_SUPPORT__) 
			if(current_ilm.msg_id == MSG_ID_MPCHAT_ENCODE_REQ)
			{
				msg_p = (void *)current_ilm.local_para_ptr;	
				mr_media_Encode_req_hdlr(msg_p);
			}
		#endif /* __MMI_DSM_MEDIA_EXT__  */
				
		kal_sleep_task(2);
   	}
    //sk_dbg_print("Sky_Multimedia_main_task ---");
}

kal_bool Sky_Multimedia_task_create(comptask_handler_struct **handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static const comptask_handler_struct ms_handler_info = 
    {
        Sky_Multimedia_main_task,  /* task entry function */
        Sky_Multimedia_task_init,       /* task initialization function */
        NULL,           /* task configuration function */
        NULL,           /* task reset handler */
        NULL,           /* task termination handler */
    };

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *handle = (comptask_handler_struct*) & ms_handler_info;
    return KAL_TRUE;
}


int ilm_struct_2_ilm_struct_cpy(ilm_struct_cpy_t *pDataCpy,ilm_struct *pData )
{
	pDataCpy->dest_mod_id = pData->dest_mod_id;
	pDataCpy->src_mod_id = pData->src_mod_id;
	pDataCpy->sap_id = pData->sap_id;
	pDataCpy->msg_id = pData->msg_id;
	pDataCpy->local_para_ptr = pData->local_para_ptr;
	pDataCpy->peer_buff_ptr = pData->peer_buff_ptr;
}

int ilm_struct_cpy_2_ilm_struct(ilm_struct *pData ,ilm_struct_cpy_t *pDataCpy)
{
	pData->src_mod_id = (module_type)pDataCpy->src_mod_id;
	pData->dest_mod_id = (module_type)pDataCpy->dest_mod_id;
	pData->sap_id = (sap_type)pDataCpy->sap_id;
	pData->msg_id = (msg_type)pDataCpy->msg_id;
	pData->local_para_ptr = (local_para_struct *)pDataCpy->local_para_ptr;
	pData->peer_buff_ptr = (peer_buff_struct  *)pDataCpy->peer_buff_ptr;
}

#if defined(__DSM_SPLAYER__)
//打开文件后发送回馈消息
void SPlayer_OpenFileInd(kal_int16 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    media_vid_file_ready_ind_struct *ind_p;
    ilm_struct *ilm_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ind_p = (media_vid_file_ready_ind_struct*) construct_local_para(sizeof(media_vid_file_ready_ind_struct), TD_CTRL);

    ind_p->result = result;
    ind_p->image_width = SPlayer_get_vid_image_width();
    ind_p->image_height = SPlayer_get_vid_image_height();
#if (MTK_VERSION <= 0x11A1132)
    ind_p->total_frame_num = SPlayer_get_vid_total_frame_num();
#else
#endif
    ind_p->total_time = SPlayer_get_vid_total_time();
    ind_p->seq_num = SPlayer_get_vid_seq_num();
    ind_p->handle = SPlayer_get_vid_handle();
    ind_p->file_handle = SPlayer_get_vid_file_handle();
	//1112_V36以下编译出错，请注释这一行
    ind_p->is_seekable = SPlayer_get_vid_is_seekable();
#if (MTK_VERSION <= 0x11A1132)
    ilm_ptr = allocate_ilm((custom_module_type)MOD_MULTIMEDIA);
    ilm_ptr->src_mod_id = (custom_module_type)MOD_MULTIMEDIA;
#else
    ilm_ptr = allocate_ilm(MOD_MULTIMEDIA);
    ilm_ptr->src_mod_id = MOD_MULTIMEDIA;
#endif
    ilm_ptr->dest_mod_id = SPlayer_get_vid_src_mod();
    ilm_ptr->sap_id = (sap_type)MED_SAP;

    ilm_ptr->msg_id = MSG_ID_MEDIA_VID_FILE_READY_IND;
    ilm_ptr->local_para_ptr = (local_para_struct*) ind_p;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);

}

void skymm_vid_open_req_hdlr(void *pdata)
{
	ilm_struct ilm_struct = {0};
	
	ilm_struct_cpy_2_ilm_struct(&ilm_struct,(ilm_struct_cpy_t *)pdata);
	vid_open_req_hdlr(&ilm_struct);
}

int skymm_GetMsg_nowait(kal_msgqid ext_msg_q_id)
{

	ilm_struct_cpy_t current_ilm;

	ilm_struct   rec_ilm;
	void *pMsgBuffer = NULL;
	kal_uint16 msg_size = 0;
	if(KAL_SUCCESS == kal_deque_msg(ext_msg_q_id, (void*)&rec_ilm, &msg_size, KAL_NO_WAIT))
	{
		pMsgBuffer = MMMalloc(SPlayer_get_MsgLen());
		if(!pMsgBuffer)
		{
			sk_dbg_print("skymm_GetMsg_nowait pMsgBuffer=NULL!!!!! ");
			return 0;
		}
	
		ilm_struct_2_ilm_struct_cpy((ilm_struct_cpy_t*)&current_ilm,(ilm_struct *)&rec_ilm);
		memcpy(pMsgBuffer,(void *)rec_ilm.local_para_ptr,SPlayer_get_MsgLen());
		current_ilm.local_para_ptr = pMsgBuffer;
		free_ilm(&rec_ilm);

		SPlayer_MessageHdlr(&current_ilm);
		if(pMsgBuffer)
          {      
			MMFree(pMsgBuffer);
            pMsgBuffer = NULL;
         }
	}
	return 1;
}
#endif
#endif

