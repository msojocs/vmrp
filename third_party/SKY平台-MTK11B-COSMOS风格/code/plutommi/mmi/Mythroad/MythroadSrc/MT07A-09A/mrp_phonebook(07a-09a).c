
extern MMI_PHB_ENTRY_BCD_STRUCT	PhoneBook[];
//extern MMI_PHB_ENTRY_STRUCT	g_phb_sim_list[];	/*Maximum entry of MSISDN, SDL and SOS in SIM*/
//extern U16	g_phb_sim_list_count;

extern MMI_PHB_LIST_VIEW phbListView;


#if  !defined(__MMI_PHB_NO_OPTIONAL_FIELD__) 
extern PHB_OPTIONAL_FIELDS_STRUCT	PhoneBookOptionalFields;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
#include "PhoneBookResDef.h"
#include "PhoneBookProt.h"
#include "ProtocolEvents.h"

extern U16 g_phb_name_index[];
extern S8 pbName[];
extern S8 pbNumber[];


static void dsm_phb_op_edit_entry_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MSG_PHB_SET_ENRTY_REQ_STRUCT *myMsgPtr;
    MYQUEUE Message;
    U16 store_index = 0;
    U8 storage;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    myMsgPtr = (MSG_PHB_SET_ENRTY_REQ_STRUCT*) OslConstructDataPtr(sizeof(MSG_PHB_SET_ENRTY_REQ_STRUCT));
    store_index = g_phb_name_index[g_phb_cntx.active_index];
	
#ifdef __MMI_DUAL_SIM_MASTER__
   if(store_index <MAX_PB_PHONE_ENTRIES)
   	storage = MMI_NVRAM;
   else if(store_index < MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES)
   	storage = MMI_SIM;
   else
   	storage = MMI_SIM2;
#else
    storage = (store_index >= MAX_PB_PHONE_ENTRIES) ? MMI_SIM : MMI_NVRAM;
#endif

    myMsgPtr->type = MMI_PHB_PHONEBOOK;
    myMsgPtr->no_data = 1;
    myMsgPtr->list.storage = storage;
    myMsgPtr->list.index = 0xffff;
	
#ifdef __MMI_DUAL_SIM_MASTER__
   if(storage == MMI_NVRAM)
   	myMsgPtr->list.record_index =store_index+1;
   else if(storage == MMI_SIM)
   	myMsgPtr->list.record_index =store_index+1 -MAX_PB_PHONE_ENTRIES;
   else
   	myMsgPtr->list.record_index = store_index+1-MAX_PB_PHONE_ENTRIES-MAX_PB_SIM_ENTRIES;
#else
    myMsgPtr->list.record_index = (storage == MMI_SIM) ? (store_index + 1) - MAX_PB_PHONE_ENTRIES : (store_index + 1);
#endif

    mmi_phb_op_set_entry_prepare_name_number(&myMsgPtr->list, pbName, pbNumber);

    Message.oslSrcId = MOD_MMI;
    Message.oslDestId = MOD_L4C;
    Message.oslMsgId = PRT_PHB_SET_ENTRY_REQ;
    Message.oslDataPtr = (oslParaType*) myMsgPtr;
    Message.oslPeerBuffPtr = NULL;
	ClearProtocolEventHandler(PRT_PHB_SET_ENTRY_RSP);
    SetProtocolEventHandler(mmi_phb_op_edit_entry_rsp, PRT_PHB_SET_ENTRY_RSP);
    OslMsgSendExtQueue(&Message);
}
static void _mmi_dsm_phb_copy_entry_from_phb(U16 store_index, T_MR_PHB_ENTRY* entry);

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
void mmi_dsm_phb_send_entry(void)
{
#if (MTK_VERSION < 0x09B0952)
	_mmi_dsm_phb_get_entry(g_phb_name_index[g_phb_cntx.active_index_second],&phb_entry);
#endif	
	mr_app_send_event_wrapper(MR_LOCALUI_EVENT, MR_LOCALUI_KEY_OK, 0);
}

void mmi_dsm_phb_GoBackHistory(void)
{
	mr_app_send_event_wrapper(MR_LOCALUI_EVENT, MR_LOCALUI_KEY_CANCEL, 0);
}
void mmi_dsm_phb_write_rsp(int32 op,int32 ret)
{
	mr_event(MR_PHB_EVENT,op,ret);
}

static void dsm_phb_op_add_entry_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MSG_PHB_SET_ENRTY_REQ_STRUCT *myMsgPtr;
    MYQUEUE Message;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

#ifdef __MMI_DUAL_SIM_MASTER__
    if (g_phb_cntx.selected_storage == MMI_SIM2)
    {
        MTPNP_PFAL_ADN_AddEntry();
        return;
    }
#endif	/* __MMI_DUAL_SIM_MASTER__ */
    myMsgPtr = (MSG_PHB_SET_ENRTY_REQ_STRUCT*) OslConstructDataPtr(sizeof(MSG_PHB_SET_ENRTY_REQ_STRUCT));
    myMsgPtr->type = MMI_PHB_PHONEBOOK;
    myMsgPtr->no_data = 1;
    myMsgPtr->list.storage = g_phb_cntx.selected_storage;
    myMsgPtr->list.index = 0xFFFF;
    myMsgPtr->list.record_index = 0xFFFF;

    mmi_phb_op_set_entry_prepare_name_number(&myMsgPtr->list, pbName, pbNumber);

    Message.oslSrcId = MOD_MMI;
    Message.oslDestId = MOD_L4C;
    Message.oslMsgId = PRT_PHB_SET_ENTRY_REQ;
    Message.oslDataPtr = (oslParaType*) myMsgPtr;
    Message.oslPeerBuffPtr = NULL;
	ClearProtocolEventHandler(PRT_PHB_SET_ENTRY_RSP);
    SetProtocolEventHandler(mmi_phb_op_add_entry_rsp, PRT_PHB_SET_ENTRY_RSP);
    OslMsgSendExtQueue(&Message);
}


#if(MTK_VERSION >=0x0924)
static void dsm_phb_delete_callback(U16 status ,U16 index)
{
	
}
#endif


static void dsm_phb_op_delete_entry_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MSG_PHB_DELETE_ENTRY_REQ_STRUCT *myMsgPtr;
    MYQUEUE Message;
    U16 store_index;
    U8 storage;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    store_index = g_phb_name_index[g_phb_cntx.active_index];

    myMsgPtr = (MSG_PHB_DELETE_ENTRY_REQ_STRUCT*) OslConstructDataPtr(sizeof(MSG_PHB_DELETE_ENTRY_REQ_STRUCT));

    storage = (store_index >= MAX_PB_PHONE_ENTRIES) ? MMI_SIM : MMI_NVRAM;

    myMsgPtr->storage = storage;
    myMsgPtr->del_all = MMI_FALSE;
    myMsgPtr->no_data = 1;
    myMsgPtr->type = MMI_PHB_PHONEBOOK;
    /*
     *The index means "record_index" here. This change only for MMI_PHB_PHONEBOOK(ADN) type
     * for other type( BDN ,FDN, MSISDN...) This field still need to use index as parameter.
     */
    myMsgPtr->index = (storage == MMI_SIM) ? (store_index + 1) - MAX_PB_PHONE_ENTRIES : (store_index + 1);
    Message.oslSrcId = MOD_MMI;
    Message.oslDestId = MOD_L4C;
    Message.oslMsgId = PRT_PHB_DEL_ENTRY_REQ;
    Message.oslDataPtr = (oslParaType*) myMsgPtr;
    Message.oslPeerBuffPtr = NULL;
#if(MTK_VERSION >=0x0924)
    g_mmi_phb_op_cntx.store_index = store_index;
    g_mmi_phb_op_cntx.callback = dsm_phb_delete_callback;
    g_mmi_phb_op_cntx.src_storage = mmi_phb_get_storage(store_index);
    SetProtocolEventHandler(mmi_phb_delete_contact_rsp, PRT_PHB_DEL_ENTRY_RSP);
#else
    SetProtocolEventHandler(mmi_phb_op_delete_entry_rsp, PRT_PHB_DEL_ENTRY_RSP);
#endif
    OslMsgSendExtQueue(&Message);
}


static int dsm_phb_get_index_by_store(U8 storage,U16 index)
{
	int pos;
	U16 store_index;
	U16 count =0;
	
	for(pos = 0 ; pos < PhoneBookEntryCount ; pos++)
	{
		store_index = g_phb_name_index[pos];
		
		if((storage == MMI_STORAGE_BOTH) ||
		(storage == MMI_NVRAM && store_index < MAX_PB_PHONE_ENTRIES) ||
		(storage == MMI_SIM && (store_index >= MAX_PB_PHONE_ENTRIES)&&(store_index <MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES))
#ifdef __MMI_DUAL_SIM_MASTER__	
		||(storage == MMI_SIM2&&store_index>=(MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES))
#endif				
		)
			count++;
		
		if((count-1) == index)
			return pos;
	}
	
	return -1;
}


U16 _mmi_dsm_phb_get_index_by_store_index(U8 storage, U16 in_store_index)
{
	U16 i;
	U16 count = 0;
	U16 store_index;

	for(i = 0 ; i <PhoneBookEntryCount ; i++)
	{
		store_index = g_phb_name_index[i];
		
		if( (storage == MR_PHB_BOTH) || \
			((storage == MR_PHB_NVM) && (store_index < MAX_PB_PHONE_ENTRIES)) || \
			((storage == MR_PHB_SIM) && (store_index >= MAX_PB_PHONE_ENTRIES)))
			count++;
		
		if(store_index == in_store_index)
			return (count-1);
	}
	return 0xffff;
}


static void _mmi_dsm_phb_copy_entry_from_phb(U16 store_index, T_MR_PHB_ENTRY* entry)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	S16 pError;
	
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	memcpy(entry->name, PhoneBook[store_index].alpha_id.name,(MAX_PB_NAME_LENGTH+1)*ENCODING_LENGTH);
	mmi_phb_convert_get_ucs2_number(entry->number, store_index);
	
#if !defined(__MMI_PHB_NO_OPTIONAL_FIELD__)
	if(store_index < MAX_PB_PHONE_ENTRIES) /*In NVRAM*/
	{
		ReadRecord(NVRAM_EF_PHB_FIELDS_LID, (U16)(store_index + 1), (void*)&PhoneBookOptionalFields, OPTIONAL_FIELDS_RECORD_SIZE, &pError);
		
		mmi_asc_to_ucs2(entry->homeNumber, (PS8)PhoneBookOptionalFields.homeNumber);
		mmi_asc_to_ucs2(entry->officeNumber, (PS8)PhoneBookOptionalFields.officeNumber);
		mmi_asc_to_ucs2(entry->faxNumber, (PS8)PhoneBookOptionalFields.faxNumber);
		mmi_asc_to_ucs2(entry->emailAddress, (PS8)PhoneBookOptionalFields.emailAddress);
		
	}
	else /*clear buffer*/
	{
		memset(entry->homeNumber,0,2);
		memset(entry->officeNumber,0,2);
		memset(entry->faxNumber,0,2);
		memset(entry->emailAddress,0,2);
	}
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
	if(!g_phb_cntx.phb_ready || g_phb_cntx.processing )
	{
		return MR_PHB_NOT_READY;
	}
	
	/*Check if search pattern empty*/
	if(mmi_ucs2strlen((S8*)phb_search_req->pattern) == 0)
	{
		return MR_PHB_NOT_FOUND;
	}
	
	/*
	* search by name
	*/
	if(phb_search_req->search_type == MR_PHB_BY_NAME)
	{
		/*Find the closest entry*/
		index = mmi_phb_search_name_binary_search(0, (PhoneBookEntryCount - 1), phb_search_req->pattern);
		store_index = g_phb_name_index[index];
		/*Check if the closest entry prefix match the pattern*/
		mmi_phb_compare_ucs2_strings(phb_search_req->pattern, (S8*)PhoneBook[store_index].alpha_id.name, &same_count, &distance);
		
		if(mmi_ucs2strlen(phb_search_req->pattern) == same_count)
		{
			_mmi_dsm_phb_copy_entry_from_phb(store_index, &(phb_search_rsq->phb_entry));
			phb_search_rsq->store_index = store_index;
			phb_search_rsq->index = _mmi_dsm_phb_get_index_by_store_index(MR_PHB_BOTH, store_index);
			return MR_PHB_SUCCESS;
		}
		else
       	{
			return MR_PHB_NOT_FOUND;
       	}
	}
	/*
	* search by number
	*/
	else if(phb_search_req->search_type == MR_PHB_BY_NUMBER)
	{
		U32 num;
		U8 number_ASCII[MAX_PB_NUMBER_LENGTH+1+1];
		
		/*Number not sorted yet*/
		if(g_phb_cntx.populate_count != 0xffff)
		{
			return MR_PHB_NOT_READY;
		}
		
		memset(number_ASCII, 0, ENCODING_LENGTH);
		mmi_ucs2_to_asc((S8*)number_ASCII, phb_search_req->pattern);
		
		num = mmi_phb_util_convert_number_to_int(number_ASCII);
		
		if(num < INVALID_NUMBER) 
		{
#if (MTK_VERSION >= 0x06b)
			store_index = mmi_phb_lookup_table_search(num, 0, (U16)(g_phb_cntx.lookup_table_count-1),(S8*)number_ASCII,MMI_STORAGE_BOTH); 
#else
			store_index = mmi_phb_lookup_table_search(num, 0, (U16)(g_phb_cntx.lookup_table_count-1),(S8*)number_ASCII); 
#endif

			if(store_index < 0xffff) /* storage location in array, begin from 0.*/
			{
				_mmi_dsm_phb_copy_entry_from_phb(store_index, &phb_search_rsq->phb_entry);
				phb_search_rsq->store_index = store_index;
				phb_search_rsq->index = _mmi_dsm_phb_get_index_by_store_index(MR_PHB_BOTH, store_index);
				return MR_PHB_SUCCESS;
			}
			else
			{
				return MR_PHB_NOT_FOUND;
			}
		}
		else
		{
			return MR_PHB_NOT_FOUND;
		}
	}
	else
   	{
		return MR_PHB_NOT_SUPPORT;
   	}
	return MR_PHB_NOT_SUPPORT;
}


int32 _mmi_dsm_phb_write_entry(T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req)
{
	if(!g_phb_cntx.phb_ready || g_phb_cntx.processing )
	{
		return MR_PHB_NOT_READY;
	}
	
	/*
	* This is an [Add] operation.
	*/
	if(phb_set_entry_req->index == 0xffff)
	{
	/*
	*  Check if name and number are both empty and all numbers are valid.
		*/
		if ((!mmi_ucs2strlen(phb_set_entry_req->entry.name) || !mmi_ucs2strlen(phb_set_entry_req->entry.number)) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.number) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.homeNumber) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.officeNumber) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.faxNumber))
		{
			return MR_PHB_ERROR;
		}
		
		/*
		* Check free space and assign storage location
		*/
		if(phb_set_entry_req->storage == MR_PHB_BOTH)
		{
			if(PhoneBookEntryCount < MAX_PB_ENTRIES)
			{
				if(g_phb_cntx.phone_used < g_phb_cntx.phone_total)	/*Save to NVRAM first*/
				{
					g_phb_cntx.selected_storage = MMI_NVRAM;
				}
				else			
				{
#ifdef __MMI_DUAL_SIM_MASTER__					
					if(mr_sim_get_active() == DSM_SLAVE_SIM)
						g_phb_cntx.selected_storage = MMI_SIM2;	
					else
#endif						
						g_phb_cntx.selected_storage = MMI_SIM;	
				}
			}
			else
			{
				return MR_PHB_STORAGE_FULL;
			}
		}
		else if(phb_set_entry_req->storage == MR_PHB_SIM)
		{
#ifdef __MMI_DUAL_SIM_MASTER__			
			if(mr_sim_get_active() == DSM_SLAVE_SIM)
			{
				int total = MTPNP_AD_ADN_SIM2GetCapacity();
				int used = MTPNP_AD_ADN_SIM2GetUsedNumber();
				if(total > used)
					g_phb_cntx.selected_storage = MMI_SIM2;
				else
					return MR_PHB_STORAGE_FULL;
			}
			else
#endif				
			{
				if(g_phb_cntx.sim_used < g_phb_cntx.sim_total)
					g_phb_cntx.selected_storage = MMI_SIM;
				else
					return MR_PHB_STORAGE_FULL;
			}
		}
		else if(phb_set_entry_req->storage == MR_PHB_NVM)
		{
			if(g_phb_cntx.phone_used < g_phb_cntx.phone_total)
				g_phb_cntx.selected_storage = MMI_NVRAM;
			else
				return MR_PHB_STORAGE_FULL;
		}
		else
		{
			return MR_PHB_ERROR;
		}
		
		/*
		* Prepare data for name and number field
		*/
		mmi_phb_util_clear_buffer(TRUE);
		memcpy(pbName, phb_set_entry_req->entry.name, (MAX_PB_NAME_LENGTH+1)*ENCODING_LENGTH);
		memcpy(pbNumber, phb_set_entry_req->entry.number,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		
		/*
		* Prepare data for optional fields
		*/
#if !defined(__MMI_PHB_NO_OPTIONAL_FIELD__) || defined(__MMI_PHB_USIM_FIELD__)
		memcpy(pbHomeNumber, phb_set_entry_req->entry.homeNumber,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		memcpy(pbOfficeNumber, phb_set_entry_req->entry.officeNumber,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		memcpy(pbFaxNumber, phb_set_entry_req->entry.faxNumber,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		memcpy(pbEmailAddress, phb_set_entry_req->entry.emailAddress,(MAX_PB_EMAIL_LENGTH+1)*ENCODING_LENGTH);
#endif		
		/*
		* prepare data for optional id
		*/
		g_phb_cntx.selected_pic_index = 0;
		g_phb_cntx.selected_ring_index = 0;
		g_phb_cntx.selected_grp_index = 0;
		g_phb_cntx.image_location = MMI_PHB_IMAGE_NO_SELECT;
		
		g_phb_enter_from = MMI_PHB_ENTER_FROM_DSM;
		dsm_phb_op_add_entry_req();
		return MR_PHB_SUCCESS;
	}
	/*
	* Support [Delete] here.
	*/
	else if(!mmi_ucs2strlen(phb_set_entry_req->entry.name) && !mmi_ucs2strlen(phb_set_entry_req->entry.number))
	{
		int index;
		U16 store_index;
		
		if(phb_set_entry_req->storage == MR_PHB_SIM)
		{
#ifdef __MMI_DUAL_SIM_MASTER__			
			if(mr_sim_get_active() == DSM_SLAVE_SIM)
				g_phb_cntx.selected_storage = MMI_SIM2;
			else
#endif				
				g_phb_cntx.selected_storage = MMI_SIM;
		}

		else if(phb_set_entry_req->storage == MR_PHB_NVM)
			g_phb_cntx.selected_storage = MMI_NVRAM;
		else
			g_phb_cntx.selected_storage = MMI_STORAGE_BOTH;
		
		index = dsm_phb_get_index_by_store(g_phb_cntx.selected_storage,phb_set_entry_req ->index);

		if(index <0)
			return MR_PHB_ERROR;

		g_phb_cntx.active_index = index; 
		g_phb_enter_from = MMI_PHB_ENTER_FROM_DSM;
#ifdef __MMI_DUAL_SIM_MASTER__		
	       store_index = g_phb_name_index[g_phb_cntx.active_index];
	        if (store_index >= MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES)
	        {
	             MTPNP_AD_DELETE_ADN_Record((unsigned short)(store_index + 1 - MAX_PB_PHONE_ENTRIES - MAX_PB_SIM_ENTRIES));
	             return MR_PHB_SUCCESS;                     
	        }
#endif	/* __MMI_DUAL_SIM_MASTER__ */

		dsm_phb_op_delete_entry_req();
		return MR_PHB_SUCCESS;
	}
	/*
	* Support [Update] here. 
	*/   
	else
	{
	/*
	*  Check if name and number are both empty and all numbers are valid.
		*/
		int index;
		U16 store_index;
		
		if ((!mmi_ucs2strlen(phb_set_entry_req->entry.name) || !mmi_ucs2strlen(phb_set_entry_req->entry.number)) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.number) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.homeNumber) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.officeNumber) ||
			!mmi_phb_op_check_valid_number(phb_set_entry_req->entry.faxNumber))
		{
			return MR_PHB_ERROR;
		}
		
		/*
		* Prepare data for name and number field
		*/
		mmi_phb_util_clear_buffer(TRUE);
		memcpy(pbName, phb_set_entry_req->entry.name, (MAX_PB_NAME_LENGTH+1)*ENCODING_LENGTH);
		memcpy(pbNumber, phb_set_entry_req->entry.number,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		
		/*
		* Prepare data for optional fields
		*/
		#if !defined(__MMI_PHB_NO_OPTIONAL_FIELD__) || defined(__MMI_PHB_USIM_FIELD__)
		memcpy(pbHomeNumber, phb_set_entry_req->entry.homeNumber,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		memcpy(pbOfficeNumber, phb_set_entry_req->entry.officeNumber,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		memcpy(pbFaxNumber, phb_set_entry_req->entry.faxNumber,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		memcpy(pbEmailAddress, phb_set_entry_req->entry.emailAddress,(MAX_PB_EMAIL_LENGTH+1)*ENCODING_LENGTH);
		#endif
		if(phb_set_entry_req->storage == MR_PHB_SIM)
		{
#ifdef __MMI_DUAL_SIM_MASTER__			
			if(mr_sim_get_active() == DSM_SLAVE_SIM)
				g_phb_cntx.selected_storage = MMI_SIM2;
			else
#endif				
				g_phb_cntx.selected_storage = MMI_SIM;
		}
		else if(phb_set_entry_req->storage == MR_PHB_NVM)
			g_phb_cntx.selected_storage = MMI_NVRAM;
		else
			g_phb_cntx.selected_storage = MMI_STORAGE_BOTH;

		index = dsm_phb_get_index_by_store(g_phb_cntx.selected_storage,phb_set_entry_req ->index);

		if(index <0)
			return MR_PHB_ERROR;
		
		g_phb_cntx.active_index = index;
		g_phb_enter_from = MMI_PHB_ENTER_FROM_DSM;
#ifdef __MMI_DUAL_SIM_MASTER__	
	       store_index = g_phb_name_index[g_phb_cntx.active_index];
	       if (store_index >= MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES)
	       {
		        MTPNP_PFAL_ADN_EDITEntry((MTPNP_UINT16)
                                 (g_phb_name_index[g_phb_cntx.active_index] + 1 - mmi_phb_get_max_pb_phone_entries() -
                                  mmi_phb_get_max_pb_sim_entries()));
			return MR_PHB_SUCCESS;
	        }
#endif
		dsm_phb_op_edit_entry_req();
		return MR_PHB_SUCCESS;	
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
	
	if(!g_phb_cntx.phb_ready || g_phb_cntx.processing )
	{
		return MR_PHB_NOT_READY;
	}
	
	/*
	* Get entry data
	*/
	{
	/*
	*  Check valid index first.
		*/
		if( phb_get_entry_req->storage == MR_PHB_BOTH)
		{
			if(phb_get_entry_req->index >= PhoneBookEntryCount)
				return MR_PHB_OUT_OF_INDEX;
			else
				storage = MMI_STORAGE_BOTH;
		}
		else if(phb_get_entry_req->storage == MR_PHB_NVM)
		{
			if(phb_get_entry_req->index >= g_phb_cntx.phone_used)
				return MR_PHB_OUT_OF_INDEX;
			else
				storage = MMI_NVRAM;
		}
		else if(phb_get_entry_req->storage == MR_PHB_SIM)
		{
#ifdef __MMI_DUAL_SIM_MASTER__		
			if(mr_sim_get_active() == DSM_SLAVE_SIM)
			{
				if(phb_get_entry_req->index >= MTPNP_AD_ADN_SIM2GetUsedNumber())
					return MR_PHB_OUT_OF_INDEX;
				else
					storage = MMI_SIM2;
			}
			else
#endif				
			{
				if(phb_get_entry_req->index >= g_phb_cntx.sim_used)
					return MR_PHB_OUT_OF_INDEX;
				else
					storage =MMI_SIM;
			}
		}
		
		{
			/*Find index position*/
			for(pos = 0 ; pos < PhoneBookEntryCount ; pos++)
			{
				store_index = g_phb_name_index[pos];
				
				if((storage == MMI_STORAGE_BOTH) ||
				(storage == MMI_NVRAM && store_index < MAX_PB_PHONE_ENTRIES) ||
				(storage == MMI_SIM && (store_index >= MAX_PB_PHONE_ENTRIES)&&(store_index <MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES))
#ifdef __MMI_DUAL_SIM_MASTER__	
				||(storage == MMI_SIM2&&store_index>=(MAX_PB_PHONE_ENTRIES+MAX_PB_SIM_ENTRIES))
#endif				
				)
					count++;
				
				if((count-1) == phb_get_entry_req->index)
					break;
			}
			/*Copy entry by assigned count number*/
			_mmi_dsm_phb_copy_entry_from_phb(store_index, &phb_get_entry_rsp->entry);
			
			return MR_PHB_SUCCESS;
		}
	}
	
	return MR_PHB_NOT_SUPPORT;
}


int32 _mmi_dsm_phb_get_count(T_MR_PHB_GET_COUNT_REQ* phb_get_count_req,T_MR_PHB_GET_COUNT_RSP *phb_get_count_rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/

	if(!g_phb_cntx.phb_ready || g_phb_cntx.processing )
	{
		return MR_PHB_NOT_READY;
	}
	
	if(phb_get_count_req->storage == MR_PHB_BOTH) 
	{
		phb_get_count_rsp->count = PhoneBookEntryCount;
		return MR_PHB_SUCCESS;
	}
	else
	{
		if(phb_get_count_req->storage == MR_PHB_NVM)
		{
			phb_get_count_rsp->count = g_phb_cntx.phone_used;
			return MR_PHB_SUCCESS;
		}
		else
		{
			if(phb_get_count_req->storage == MR_PHB_SIM)
			{
#ifdef __MMI_DUAL_SIM_MASTER__				
				if(mr_sim_get_active() == DSM_SLAVE_SIM)
					phb_get_count_rsp->count = MTPNP_AD_ADN_SIM2GetUsedNumber();
				else
#endif					
					phb_get_count_rsp->count = g_phb_cntx.sim_used;
				return MR_PHB_SUCCESS;
			}
			return MR_PHB_NOT_SUPPORT;
		}
	}
}


void mmi_dsm_entry_phone_book(void)
{
	mr_phone_win = 1;

	if(g_phb_cntx.phb_ready && !g_phb_cntx.processing)
	{
		if(PhoneBookEntryCount)
		{
			phbListView = MMI_PHB_LIST_FOR_DSM_ENTRY;			
			memset(&phb_entry,0,sizeof(T_MR_PHB_ENTRY));
			mmi_phb_list_pre_entry_second_level();
		}
		else
		{
			mr_phone_win = 0;
		}
	}
	else
	{
		mr_phone_win = 0 ;
	}
}


int32 mr_phonebook_destroy_contact_list(void)
{	
	GoBackHistory();
	return MR_SUCCESS;
}
/****************************************************************************
函数名:static int wstrlen(char * txt)
描  述:计算大端模式unicode字串的长度(byte)
参  数:txt:要求长度的字传
返  回:字传的长度
****************************************************************************/
int wstrlen(char * txt)
{
	int lenth=0;
	unsigned char * ss=(unsigned char*)txt;

	while(((*ss<<8)+*(ss+1))!=0)
	{
		lenth+=2;
		ss+=2;
	}
	return lenth;
}

/****************************************************************************
函数名:char *ConvertString(char * str)
描  述:unicode的大小端转换
参  数:str:要转换的unicode字传
返  回:转换成功的字传
****************************************************************************/
static char *ConvertString(char * str)
{
	char temp;
	int i;
       int len = wstrlen(str);
	   
	for (i = 0; i < len ; i += 2)
	{
            temp = str[i];
	     str[i] = str[i+1];
	     str[i+1] = temp;
	}
	
	return str;
}

int32 _mmi_dsm_phb_write_owner_entry(T_MR_PHB_SET_OWNER_ENTRY_REQ* phb_set_owner_entry_req)
{
	
	if(!g_phb_cntx.phb_ready || g_phb_cntx.processing )
	{
		return MR_PHB_NOT_READY;
	}
	
	
	/*
	* This is an [Add] operation.
	*/
	if(mmi_ucs2strlen(phb_set_owner_entry_req->entry.number)&&mmi_ucs2strlen(phb_set_owner_entry_req->entry.name))
	{
		
	/*
	* big endian <->litter endian
		*/
		ConvertString(phb_set_owner_entry_req->entry.name);
		ConvertString(phb_set_owner_entry_req->entry.number);
		/*
		*  Check if name and number are both empty and all numbers are valid.
		*/
		if ((!mmi_ucs2strlen(phb_set_owner_entry_req->entry.name) && !mmi_ucs2strlen(phb_set_owner_entry_req->entry.number)) ||
			!mmi_phb_op_check_valid_number(phb_set_owner_entry_req->entry.number))
		{
			return MR_PHB_ERROR;
		}
		
		
		/*
		* Prepare data for name and number field
		*/
		mmi_phb_util_clear_buffer(TRUE);
		memcpy(pbName, phb_set_owner_entry_req->entry.name, (MAX_PB_NAME_LENGTH+1)*ENCODING_LENGTH);
		memcpy(pbNumber, phb_set_owner_entry_req->entry.number,(MAX_PB_NUMBER_LENGTH+1+1)*ENCODING_LENGTH);
		
		
		/*
		* prepare data for optional id
		*/
		g_phb_cntx.active_index = 0;
		g_phb_enter_from = MMI_PHB_ENTER_FROM_DSM;
		mmi_phb_owner_number_edit_req();
		return MR_PHB_SUCCESS;
	}
	/*
	* Support [Delete] here.
	*/
	else if(!mmi_ucs2strlen(phb_set_owner_entry_req->entry.name) && !mmi_ucs2strlen(phb_set_owner_entry_req->entry.number))
	{
		
		g_phb_cntx.active_index = 0;
		g_phb_enter_from = MMI_PHB_ENTER_FROM_DSM;
		mmi_phb_owner_number_erase_req();
		return MR_PHB_SUCCESS;
	}
	else
	{
		return MR_PHB_ERROR;
	}
	/*
	* Support [Update] here. 
	*/   
}


int32 mr_phonebook_get_status(void)
{
	if(!g_phb_cntx.phb_ready || g_phb_cntx.processing )
		return MR_PHB_NOT_READY;
	else
		return MR_PHB_READY;
}


