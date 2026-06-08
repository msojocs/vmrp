
#include "phbcuigprot.h"
#include "phbsrvgprot.h"
#include "vcui_phb_gprot.h"

static mmi_id dsm_phoneBook_cui_id = GRP_ID_INVALID;
static mmi_id mythroad_id = GRP_ID_INVALID;

///////////////////////////////////////////////////////////////////////////////////////////////////
static void _mmi_dsm_phb_get_entry(U16 store_index, T_MR_PHB_ENTRY *phb_entry);

static void mmi_dsm_phb_GoBackHistory(void)
{
	mr_app_send_event_wrapper(MR_LOCALUI_EVENT, MR_LOCALUI_KEY_CANCEL, 0);
}


static void mmi_dsm_phb_send_entry(void)
{
	mr_app_send_event_wrapper(MR_LOCALUI_EVENT, MR_LOCALUI_KEY_OK, 0);
}


static mmi_ret mmi_dsm_select_from_phb(mmi_event_struct *evt)
{
#ifndef __COSMOS_MMI_PACKAGE__

	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	cui_phb_select_contact_result_struct* select_contact_result;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	switch(evt->evt_id)
	{
		case EVT_ID_GROUP_DEINIT:
			/* need addd code to release buffer */
			dsm_phoneBook_cui_id = GRP_ID_INVALID;
			break;
		case EVT_ID_PHB_SELECT_CONTACT:
			select_contact_result = (cui_phb_select_contact_result_struct*)evt;
			memset(&phb_entry,0,sizeof(phb_entry));
			_mmi_dsm_phb_get_entry(select_contact_result->store_index,&phb_entry);
			mr_phonebook_destroy_contact_list();
			mmi_dsm_phb_send_entry();
			break;
		case EVT_ID_PHB_SELECT_CONTACT_CANCEL:
			mr_phonebook_destroy_contact_list();
			mmi_dsm_phb_GoBackHistory();
			break;
		case EVT_ID_GROUP_EXIT:
			mmi_dsm_phb_GoBackHistory();
			break;
		default:
			break;
	}
	return MMI_RET_OK;
	
#else
	vcui_phb_list_result_event_struct* result;
	
	switch(evt->evt_id)
	{
        case EVT_ID_CUI_PHB_LIST_RESULT:
		result = (vcui_phb_list_result_event_struct*) evt;

		if (result->result && result->select_count == 1)
		{  
			memset(&phb_entry,0,sizeof(phb_entry));
			_mmi_dsm_phb_get_entry(vcui_phb_list_get_contact_id(dsm_phoneBook_cui_id), &phb_entry);
			mr_phonebook_destroy_contact_list();
			mmi_dsm_phb_send_entry();            
			}
		else
		{
			mr_phonebook_destroy_contact_list();
			mmi_dsm_phb_GoBackHistory();			
		}

		return MMI_RET_OK;
	}
	
	return MMI_RET_DONT_CARE;   
#endif	
}


static void dsm_phb_op_single_cb(S32 result, U16 store_index, void *user_data)
{
	int32 action = (int32)user_data;

	if (result >= 0)	
		result = MR_SUCCESS;
	else
		result = MR_FAILED;

	mr_event(MR_PHB_EVENT, action, result);
}


static void _mmi_dsm_phb_copy_entry_from_phb(U16 store_index, T_MR_PHB_ENTRY* entry)
{
	mmi_phb_num_type_enum type ;
	mmi_phb_wcs_num_struct contact;
	int32 size = MR_PHB_MAX_NUMBER;
	int32 i = SRV_PHB_ENTRY_FIELD_OPT_NUM_1;
	if(!entry ||store_index >= MMI_PHB_ENTRIES)
		return;
	//需要有个'\0'
	srv_phb_get_name(store_index,(U16 *)entry->name,(MR_PHB_MAX_NAME-2)/2);
	srv_phb_get_number(store_index, (U16 *) entry->number,(MR_PHB_MAX_NUMBER-2)/2);

#if (!defined(__MMI_PHB_NO_OPTIONAL_FIELD__))	
       //用小的长度,防止数据溢出
	size = (MMI_PHB_NUMBER_PLUS_LENGTH > MR_PHB_MAX_NUMBER)  ? MR_PHB_MAX_NUMBER:MMI_PHB_NUMBER_PLUS_LENGTH;
	//number 1
	while(i <= SRV_PHB_ENTRY_FIELD_OPT_NUM_3)
	{
		if(MMI_TRUE == srv_phb_get_optional_number( store_index, i, (U16 *) contact.number, &type, MMI_PHB_NUMBER_PLUS_LENGTH))
		{
			switch(type)
			{
				case MMI_PHB_NUM_TYPE_HOME:
					memcpy(entry->homeNumber,contact.number,size);
					break;	
				case MMI_PHB_NUM_TYPE_OFFICE:
					memcpy(entry->officeNumber,contact.number,size);
					break;	
				case MMI_PHB_NUM_TYPE_FAX:
					memcpy(entry->faxNumber,contact.number,size);
					break;	
			}
		}
		i  <<= 1;
	}
#endif

#if (!defined(__MMI_PHB_NO_OPTIONAL_FIELD__))	
	srv_phb_get_email_address(store_index, (U16 *) entry->emailAddress,size /2);
#endif	
}


int32 _mmi_dsm_phb_search_entry(T_MR_PHB_SEARCH_REQ *phb_search_req,T_MR_PHB_SEARCH_RSP *phb_search_rsq)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U16 store_index;
	U16 index;
	S32 distance;
	U8 same_count;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	log_pb("search %s: %d, %d", phb_search_req->pattern, phb_search_req->search_type, phb_search_req->storage);

	if(!srv_phb_startup_is_phb_ready())
	{
		return MR_PHB_NOT_READY;
	}

	if(mmi_ucs2strlen((S8*)phb_search_req->pattern) == 0)
	{
		return MR_PHB_NOT_FOUND;
	}
	
	if(phb_search_req->search_type == MR_PHB_BY_NAME)
	{
		index = srv_phb_oplib_search((U16*)phb_search_req->pattern,SRV_PHB_SEARCH_TYPE_NAME);
	}
	else if(phb_search_req->search_type == MR_PHB_BY_NUMBER)
	{
		index = srv_phb_oplib_search((U16*)phb_search_req->pattern,SRV_PHB_SEARCH_TYPE_NUM);
	}

	if(index <0)
	{
		return MR_PHB_NOT_FOUND;
	}
	else
	{
		//store 从0开始,sort 单个类型 从1 开始,全部从0开始
		if(phb_search_req->search_type == MR_PHB_BY_NAME)
		{
			phb_search_rsq->index = index;
			phb_search_rsq->store_index = srv_phb_sse_sort_index_to_store_index(PHB_STORAGE_MAX,index);
		}
		else
		{
			phb_search_rsq->store_index = index;
			phb_search_rsq->index = srv_phb_sse_store_index_to_sort_index(PHB_STORAGE_MAX,index)-1;
		}
		
		log_pb("%s: %d, %d", __FILE__, phb_search_rsq->index, phb_search_rsq->store_index);

		if(phb_search_rsq->store_index >= MMI_PHB_ENTRIES)
			return MR_PHB_NOT_FOUND;
		
		_mmi_dsm_phb_copy_entry_from_phb(phb_search_rsq->store_index, &(phb_search_rsq->phb_entry));
		return MR_PHB_SUCCESS;
	}

}
void _mmi_dsm_phb_free_memory(srv_phb_contact_info_struct* pcontact)
{
	int32 i = 0;
	if(pcontact != 0)
	{
 		for( i = 0;i< 3;i++)
		{
			if(pcontact->field_data.opt_num[i] != 0)
			{
				srv_phb_mem_free(pcontact->field_data.opt_num[i] );
				pcontact->field_data.opt_num[i]  = 0;
			}
		}
	}	
}
static int32 _mmi_dsm_phb_add_field(srv_phb_contact_info_struct* pcontact_info ,T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req)
{
	int32 i = 0;
	int32 tpye[3] = {MMI_PHB_NUM_TYPE_HOME,MMI_PHB_NUM_TYPE_OFFICE,MMI_PHB_NUM_TYPE_FAX};
	char* address[3] = {0};
	if((0 == pcontact_info)||(0 == phb_set_entry_req))
	{
		return MR_FAILED;
	}
	
	pcontact_info->field_data.opt_num[0] = NULL;
	pcontact_info->field_data.opt_num[1] = NULL;
	pcontact_info->field_data.opt_num[2] = NULL;
	address[0] = (char*)phb_set_entry_req->entry.homeNumber;
	address[1] = (char*)phb_set_entry_req->entry.officeNumber;
	address[2] = (char*)phb_set_entry_req->entry.faxNumber;
	for(i = 0;i < 3;i++)
	{
		pcontact_info->field_data.opt_num[i] = srv_phb_mem_malloc(sizeof(mmi_phb_wcs_num_struct), SRV_PHB_MEMORY_TYPE_ADM);
		if (pcontact_info->field_data.opt_num[i] == NULL)
		{
			_mmi_dsm_phb_free_memory(pcontact_info);
			return MR_FAILED;
		}
		pcontact_info->field_data.opt_num[i]->type = tpye[i];
		mmi_ucs2ncpy((char *)pcontact_info->field_data.opt_num[i]->number, (const char *)address[i], MMI_PHB_NUMBER_PLUS_LENGTH);
	}
	return MR_SUCCESS;
}
static int32 _mmi_dsm_phb_add_entry(T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req)
{
	U8 storage;
	srv_phb_contact_info_struct contact_info;

	if(0 == phb_set_entry_req)
	{
		return MR_PHB_ERROR;
	}
	/*
	 *  Check if name and number are both empty and all numbers are valid.
	 */
//	if ((!mmi_ucs2strlen(phb_set_entry_req->entry.name) || 
//		!mmi_ucs2strlen(phb_set_entry_req->entry.number)) ||
	//不再判断空数据,只判断数据是否是错误.
	if (	!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.number) ||
		!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.homeNumber) ||
		!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.officeNumber) ||
		!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.faxNumber))
	{
		return MR_PHB_ERROR;
	}

	memset(&contact_info, 0, sizeof(srv_phb_contact_info_struct));
	contact_info.field_mask = 
		SRV_PHB_ENTRY_FIELD_NAME | 
		SRV_PHB_ENTRY_FIELD_NUMBER | 
		SRV_PHB_ENTRY_FIELD_HOME | 
		SRV_PHB_ENTRY_FIELD_OFFICE | 
		SRV_PHB_ENTRY_FIELD_FAX | 
		SRV_PHB_ENTRY_FIELD_EMAIL1;
	contact_info.field_data.name = (U16 *)phb_set_entry_req->entry.name;
	contact_info.field_data.number = (U16*)phb_set_entry_req->entry.number;
	contact_info.field_data.email_address = (U16*)phb_set_entry_req->entry.emailAddress;

	if (phb_set_entry_req->storage == MR_PHB_BOTH)
	{
		if (srv_phb_get_used_contact(PHB_STORAGE_MAX) >= (srv_phb_get_total_contact(PHB_STORAGE_MAX)))
		{
			return MR_PHB_STORAGE_FULL;
		}
		else if (srv_phb_get_used_contact(PHB_STORAGE_NVRAM) < (srv_phb_get_total_contact(PHB_STORAGE_NVRAM)))
		{
			storage = PHB_STORAGE_NVRAM;
		}			
		else if (srv_phb_get_used_contact(mr_sim_active_id_dsm2phb()) < (srv_phb_get_total_contact(mr_sim_active_id_dsm2phb())))
		{
			storage = mr_sim_active_id_dsm2phb();
		}
		else
		{
			return MR_PHB_STORAGE_FULL;
		}
	}
	else if (phb_set_entry_req->storage == MR_PHB_SIM)
	{
		if(srv_phb_get_used_contact(mr_sim_active_id_dsm2phb()) < (srv_phb_get_total_contact(mr_sim_active_id_dsm2phb())))
		{
			storage = mr_sim_active_id_dsm2phb();
		}
		else
		{
			return MR_PHB_STORAGE_FULL;
		}
	}
	else
	{
		if (srv_phb_get_used_contact(PHB_STORAGE_NVRAM) < (srv_phb_get_total_contact(PHB_STORAGE_NVRAM)))
		{
			storage = PHB_STORAGE_NVRAM;
		}
		else
		{
			return MR_PHB_STORAGE_FULL;	
		}
	}

	//注意该函数有内存申请
	if(MR_FAILED == _mmi_dsm_phb_add_field(&contact_info,phb_set_entry_req))
	{
		return MR_PHB_ERROR;
	}
	
	srv_phb_oplib_add_contact(
		storage,
		SRV_PHB_INVALID_INDEX,
		&contact_info,
		dsm_phb_op_single_cb,
		(void *)MR_PHB_OP_WRITE);
		
	//释放内存
	_mmi_dsm_phb_free_memory(&contact_info);

	return MR_PHB_SUCCESS;

}
static int32 _mmi_dsm_phb_delete_entry(T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req)
{
	S32 store_index;
	U8 storage;
	if(0 ==phb_set_entry_req)
	{
		return MR_PHB_ERROR;
	}
	
	if (phb_set_entry_req->storage == MR_PHB_SIM)
	{
		storage = mr_sim_active_id_dsm2phb();
	}
	else if (phb_set_entry_req->storage == MR_PHB_NVM)
	{
		storage = PHB_STORAGE_NVRAM;
	}
	else
	{
		storage = PHB_STORAGE_MAX;
	}
	
	if (storage != PHB_STORAGE_MAX)
	{
		store_index = srv_phb_sse_sort_index_to_store_index(storage, phb_set_entry_req->index + 1);
	}
	else
	{
		store_index = srv_phb_sse_sort_index_to_store_index(storage, phb_set_entry_req->index);
	}
	
	
	if (store_index < 0)
	{
		return MR_PHB_ERROR;
	}
	
	srv_phb_oplib_delete_contact(
        store_index,
        dsm_phb_op_single_cb,
        (void *)MR_PHB_OP_DELETE);

	return MR_PHB_SUCCESS;

}
static int32 _mmi_dsm_phb_update_entry(T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req)
{
	int index;
	U16 store_index;
	U8 storage;
	srv_phb_contact_info_struct contact_info;
	
	if(0 ==phb_set_entry_req)
	{
		return MR_PHB_ERROR;
	}
	
	/*
	 *  Check if name and number are both empty and all numbers are valid.
	 */
	 //不再检查是否是空数据,只检查是否数据出错
//	if ((!mmi_ucs2strlen(phb_set_entry_req->entry.name) || 
//		!mmi_ucs2strlen(phb_set_entry_req->entry.number)) ||
	if (	!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.number) ||
		!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.homeNumber) ||
		!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.officeNumber) ||
		!srv_phb_check_valid_number((U16 *)phb_set_entry_req->entry.faxNumber))
	{
		return MR_PHB_ERROR;
	}
	
	memset(&contact_info, 0, sizeof(srv_phb_contact_info_struct));
	contact_info.field_mask = 
		SRV_PHB_ENTRY_FIELD_NAME | 
		SRV_PHB_ENTRY_FIELD_NUMBER | 
		SRV_PHB_ENTRY_FIELD_HOME | 
		SRV_PHB_ENTRY_FIELD_OFFICE | 
		SRV_PHB_ENTRY_FIELD_FAX | 
		SRV_PHB_ENTRY_FIELD_EMAIL1;
	contact_info.field_data.name = (U16 *)phb_set_entry_req->entry.name;
	contact_info.field_data.number = (U16 *)phb_set_entry_req->entry.number;

	contact_info.field_data.email_address = (U16*)phb_set_entry_req->entry.emailAddress;
	if (phb_set_entry_req->storage == MR_PHB_SIM)
	{
		storage = mr_sim_active_id_dsm2phb();
	}
	else if (phb_set_entry_req->storage == MR_PHB_NVM)
	{
		storage = PHB_STORAGE_NVRAM;
	}	
	else
	{
		storage = PHB_STORAGE_MAX;
	}
	
	if (storage != PHB_STORAGE_MAX)
	{
		store_index = srv_phb_sse_sort_index_to_store_index(storage, phb_set_entry_req->index + 1);
	}
	else
	{
		store_index = srv_phb_sse_sort_index_to_store_index(storage, phb_set_entry_req->index);
	}
	
	//注意该函数有内存申请
	if(MR_FAILED == _mmi_dsm_phb_add_field(&contact_info,phb_set_entry_req))
	{
		return MR_PHB_ERROR;
	}
	
	srv_phb_oplib_update_contact(
		store_index,
		&contact_info,
		dsm_phb_op_single_cb,
		(void*)MR_PHB_OP_EDIT);
	//释放内存
	_mmi_dsm_phb_free_memory(&contact_info);
	
	return MR_PHB_SUCCESS;
}
int32 _mmi_dsm_phb_write_entry(T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req)
{
	if(!srv_phb_startup_is_phb_ready())
	{
		return MR_PHB_NOT_READY;
	}
	
	kal_prompt_trace(MOD_MMI,"write: %d, %d, %s, %s", phb_set_entry_req->index, phb_set_entry_req->storage, phb_set_entry_req->entry.name, phb_set_entry_req->entry.number);

	/*
	 * This is an [Add] operation.
	 */
	if (phb_set_entry_req->index == 0xffff)
	{
		return _mmi_dsm_phb_add_entry(phb_set_entry_req);
	}
	/*
	* Suppose [Delete] here.
	*/
	else if (!mmi_ucs2strlen(phb_set_entry_req->entry.name) && !mmi_ucs2strlen(phb_set_entry_req->entry.number))
	{
		return _mmi_dsm_phb_delete_entry(phb_set_entry_req);
	}
	/*
	* Suppose [Update] here. 
	*/   
	else
	{
		return _mmi_dsm_phb_update_entry(phb_set_entry_req);
	}
}



int32 _mmi_dsm_phb_read_entry(T_MR_PHB_GET_ENTRY_REQ* phb_get_entry_req,T_MR_PHB_GET_ENTRY_RSP *phb_get_entry_rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U16 pos;
	U16 count = 0;
	U16 store_index = 0;
	U16 storage;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	
	if(!srv_phb_startup_is_phb_ready())
	{
		return MR_PHB_NOT_READY;
	}
	
	if( phb_get_entry_req->storage == MR_PHB_BOTH)
	{
		if(phb_get_entry_req->index >= srv_phb_get_used_contact(PHB_STORAGE_MAX))
			return MR_PHB_OUT_OF_INDEX;
		else
			storage = PHB_STORAGE_MAX;
	}
	else if( phb_get_entry_req->storage == MR_PHB_NVM)
	{
		if(phb_get_entry_req->index >= srv_phb_get_used_contact(PHB_STORAGE_NVRAM))
			return MR_PHB_OUT_OF_INDEX;
		else
			storage = PHB_STORAGE_NVRAM;
	}
	else
	{

		storage = mr_sim_active_id_dsm2phb();
		count= srv_phb_get_used_contact(storage);
		
		if(phb_get_entry_req->index >= count)
			return MR_PHB_OUT_OF_INDEX;
	}

	if(storage != PHB_STORAGE_MAX)
	{
		store_index = srv_phb_sse_sort_index_to_store_index(storage,phb_get_entry_req->index+1);
	}
	else
	{
		store_index = srv_phb_sse_sort_index_to_store_index(storage,phb_get_entry_req->index);
	}
	if(store_index <0)
		return MR_PHB_ERROR;
	_mmi_dsm_phb_copy_entry_from_phb(store_index, &phb_get_entry_rsp->entry);
	return MR_PHB_SUCCESS;
}


int32 _mmi_dsm_phb_get_count(T_MR_PHB_GET_COUNT_REQ* phb_get_count_req,T_MR_PHB_GET_COUNT_RSP *phb_get_count_rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	if(!srv_phb_startup_is_phb_ready())
	{
		return MR_PHB_NOT_READY;
	}
	
	if(phb_get_count_req->storage == MR_PHB_BOTH) 
	{
		phb_get_count_rsp->count = srv_phb_get_used_contact(PHB_STORAGE_MAX);
		return MR_PHB_SUCCESS;
	}
	else
	{
		if(phb_get_count_req->storage == MR_PHB_NVM)
		{
			phb_get_count_rsp->count = srv_phb_get_used_contact(PHB_STORAGE_NVRAM);
			return MR_PHB_SUCCESS;
		}
		else
		{
			if(phb_get_count_req->storage == MR_PHB_SIM)
			{
				phb_get_count_rsp->count = srv_phb_get_used_contact(mr_sim_active_id_dsm2phb());

				return MR_PHB_SUCCESS;
			}
			return MR_PHB_NOT_SUPPORT;
		}
	}	
}


void mmi_dsm_entry_phone_book(void)
{
	mr_phone_win = 1;

	if(srv_phb_startup_is_phb_ready())
	{
		mythroad_id = vapp_mythroad_launch(NULL, 0, mmi_dsm_select_from_phb);
		dsm_phoneBook_cui_id = vcui_phb_list_create(mythroad_id);

		log_pb("phone_book mythroad_id=%d", mythroad_id);
		log_pb("phone_book dsm_phoneBook_cui_id=%d", dsm_phoneBook_cui_id);

		if (dsm_phoneBook_cui_id != GRP_ID_INVALID)
		{
	            vcui_phb_list_set_req_type(dsm_phoneBook_cui_id, VAPP_PHB_LIST_SINGLE_SELECTOR);		
	            vcui_phb_list_run(dsm_phoneBook_cui_id);			
		}
		else
		{
			dsm_phoneBook_cui_id = GRP_ID_INVALID;
			vapp_mythroad_terminate(mythroad_id);
			mr_phone_win = 0;
		}
	}
	else
	{
		mr_phone_win = 0;
	}
}


int32 mr_phonebook_destroy_contact_list(void)
{
	if(dsm_phoneBook_cui_id != GRP_ID_INVALID)
	{
		vcui_phb_list_close(dsm_phoneBook_cui_id);
		mmi_frm_group_close(GRP_ID_MYTHROAD);
		dsm_phoneBook_cui_id = GRP_ID_INVALID;
		vapp_mythroad_terminate(mythroad_id);
	}

	return MR_SUCCESS;
}


static void _mmi_dsm_phb_get_entry(U16 store_index, T_MR_PHB_ENTRY *phb_entry)
{
	_mmi_dsm_phb_copy_entry_from_phb(store_index, phb_entry);
	mr_str_convert_endian(phb_entry->name);
	mr_str_convert_endian(phb_entry->number);
	mr_str_convert_endian(phb_entry->homeNumber);
	mr_str_convert_endian(phb_entry->officeNumber);
	mr_str_convert_endian(phb_entry->faxNumber);
	mr_str_convert_endian(phb_entry->emailAddress);
}


int32 mr_phonebook_get_status(void)
{
	if(!srv_phb_startup_is_phb_ready())
		return MR_PHB_NOT_READY;
	else
		return MR_PHB_READY;
}

