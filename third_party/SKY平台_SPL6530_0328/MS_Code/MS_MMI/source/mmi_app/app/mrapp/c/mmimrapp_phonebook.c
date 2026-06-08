#include "mmi_app_mrapp_trc.h"
#ifdef MRAPP_SUPPORT

#include "sci_types.h"
#include "mmimrapp_export.h"
#include "mmimrapp_internal.h"
#include "mmimrapp_phonebook.h"
#include "mmi_common.h"
#include "mmipb_common.h"
#include "mmipb_storage.h"
#include "mmipb_menu.h"
#include "mmipb_export.h"


T_MR_PHB_SEARCH_RSP    g_phb_search_rsp   = {0};
T_MR_PHB_GET_COUNT_RSP g_phb_getcount_rsp = {0};
T_MR_PHB_COMMON_RSP    g_phb_common_rsp   = {0};
T_MR_PHB_GET_ENTRY_RSP g_phb_get_entry_rsp= {0};
MMIPB_CONTACT_T *g_pb_set_contact = NULL;

int32 MMIMRAPP_PHBSearchEntry(uint8* input, int32 input_len, uint8** output, int32* output_len)
{ /*lint !e18*/
    uint16    entry_id = 0;
	uint16	  storage_id = 0;
    uint8     number[MR_PHB_MAX_NUMBER] = {0};
    MMIPB_NAME_T       name = {0};
    MMIPB_BCD_NUMBER_T bcd_num = {0};
    MMI_PARTY_NUMBER_T party_num = {0};
    T_MR_PHB_SEARCH_REQ *req = (T_MR_PHB_SEARCH_REQ *)input; 
	
    if(input == NULL || input_len == 0 || output == NULL || output_len == NULL)
    {
        return MR_FAILED;
    }

    if(!MMIAPIPB_IsPbReady())
    {
        return MR_PHB_NOT_READY;
    }
    MMIMR_Trace(("MRAPP[PB] search_type = %d len = %d", req->search_type, MMIAPICOM_Wstrlen(req->pattern)*2)); /*lint !e64*/

	/* 大小端转换 */
#ifndef WIN32
	mr_str_convert_endian_with_size(req->pattern, MR_PHB_MAX_SEARCH_PATTERN);
#endif
	
    if(req->search_type == MR_PHB_BY_NAME)
    {
		int i = 0;
		int j = 0;
#if (SPR_VERSION >= 0x12A1216)
        uint32 group = 0;
#endif
		wchar *search_name_buf = NULL;
		BOOLEAN is_find = FALSE;
		MMIPB_HANDLE_T pb_handle = 0; 
		MMI_STRING_T  search_name = {0};
		uint16 search_count = 0;
		BOOLEAN result = FALSE;
		uint8	tele_len = 0;
		MMIPB_BCD_NUMBER_T search_number[MMIPB_MAX_NV_PHONE_NUM] = {0};
		uint8	temp_num[MMIPB_BCD_NUMBER_MAX_LEN*2 + 2]= {0};
		
        name.wstr_len = MIN(MMIPB_NAME_MAX_LEN, MMIAPICOM_Wstrlen(req->pattern)); /*lint !e666 !e64 !e64*/
        MMIAPICOM_Wstrncpy(name.wstr, req->pattern, name.wstr_len); /*lint !e64*/
        for(i=0; i<name.wstr_len; i++)
        {
            //SCI_TRACE_LOW:"name_watr[%02d] = 0x%02x"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_PHONEBOOK_65_112_2_18_3_12_50_2,(uint8*)"dd", i, name.wstr[i]);
        }

		search_name_buf = (wchar*)SCI_ALLOCA(MMIPB_NAME_MAX_LEN);
		memset((void*)search_name_buf, 0, (size_t)MMIPB_NAME_MAX_LEN); /*lint !e831*/
		search_name.wstr_ptr = search_name_buf;

		pb_handle = MMIAPIPB_SearchList(NULL, MMIPB_LIST_FIELD_WHOLE, &search_count);
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry pb_handle = %d, search_count = %d", pb_handle, search_count));

		if(pb_handle >= 0 && search_count > 0) /*lint !e685 !e568*/
		{
			for(i=0; i<search_count; i++)
			{
#if (SPR_VERSION >= 0x12A1216)
				result = MMIAPIPB_GetSearchEntry(pb_handle, &search_name, MMIPB_NAME_MAX_LEN, &search_number, &group, i); /*lint !e64*/
#else
                result = MMIAPIPB_GetSearchEntry(pb_handle, &search_name, MMIPB_NAME_MAX_LEN, &search_number, i); /*lint !e64*/
#endif
				MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSearchEntry result = %d", result));
				if(result)
				{
					MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSearchEntry search_name_wstr_len = %d", search_name.wstr_len));
					for(j=0; j<search_name.wstr_len; j++)
        			{
            			//SCI_TRACE_LOW:"search_name_watr[%02d] = 0x%02x"
            			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_PHONEBOOK_86_112_2_18_3_12_51_3,(uint8*)"dd", j, search_name.wstr_ptr[j]);
        			}
					
					if(MMIAPICOM_Wstrnstr(search_name.wstr_ptr, search_name.wstr_len, name.wstr, name.wstr_len))
					{
						g_phb_search_rsp.index = i;
						is_find = TRUE;
						break;
					}
				}
			}
			
			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSearchEntry is_find:%d", is_find));
			if(is_find)
        	{
				/*get name*/
				MMIAPICOM_Wstrcpy((uint16*)g_phb_search_rsp.phb_entry.name, name.wstr);
				
				/*get mobile number*/
				tele_len = MMIAPICOM_GenDispNumber( 
							MMIPB_GetNumberTypeFromUint8(search_number[MR_PHB_MOBILE].npi_ton), 
							(uint8)MIN(search_number[MR_PHB_MOBILE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
							search_number[MR_PHB_MOBILE].number,  
							temp_num,
							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
    						);
				MMI_STRNTOWSTR(g_phb_search_rsp.phb_entry.number, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

				/* 大小端转换 */
#ifndef WIN32
				mr_str_convert_endian_with_size(g_phb_search_rsp.phb_entry.name, MR_PHB_MAX_NAME);
				mr_str_convert_endian_with_size(g_phb_search_rsp.phb_entry.number, MR_PHB_MAX_NUMBER);
#endif
			
				*output = (uint8*)&g_phb_search_rsp;
				*output_len = sizeof(T_MR_PHB_SEARCH_RSP);

				if(search_name_buf != NULL) /*lint !e774 !e831*/
				{
					SCI_FREE(search_name_buf);
				}
				MMIAPIPB_SearchExit(pb_handle);
           		return MR_PHB_SUCCESS;
        	}
			else
			{
				if(search_name_buf != NULL) /*lint !e774 !e831*/
				{
					SCI_FREE(search_name_buf);
				}
				MMIAPIPB_SearchExit(pb_handle);
            	return MR_PHB_NOT_FOUND;
			}
		}
        else
        {
			if(search_name_buf != NULL) /*lint !e774 !e831*/
			{
				SCI_FREE(search_name_buf);
			}
			MMIAPIPB_SearchExit(pb_handle);
            return MR_PHB_NOT_FOUND;
        }
    }
    else if(req->search_type == MR_PHB_BY_NUMBER)
    { 
		BOOLEAN search_res = FALSE;
		uint8	num_index = 0;
		uint8	tele_len = 0;
		uint8	temp_num[MMIPB_BCD_NUMBER_MAX_LEN*2 + 2]= {0};
		MMIPB_CONTACT_T pb_contact = {0};
		MMIPB_ERROR_E result = MMIPB_ERROR_ERROR;

        MMIAPICOM_WstrToStr(req->pattern, number); /*lint !e64*/
	 	MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSearchEntry num:%s", number));

        MMIAPICOM_GenPartyNumber((uint8*)number, (int16)strlen(number), &party_num); /*lint !e64*/
		bcd_num.number_len = (uint8)party_num.num_len;
		bcd_num.npi_ton = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, 0);
		MMI_MEMCPY(bcd_num.number, MMIPB_BCD_NUMBER_MAX_LEN, party_num.bcd_num, party_num.num_len, party_num.num_len);

		search_res = MMIPB_SearchPhoneNumber(&bcd_num, &entry_id, &storage_id, &num_index); /*lint !e718 !e18*/
		if(search_res)
        {
			result = MMIAPIPB_GetContactInfo(entry_id, storage_id, &pb_contact);
   			if(MMIPB_ERROR_SUCCESS == result)
			{
				/*get name*/
				MMIAPICOM_Wstrcpy((uint16*)g_phb_search_rsp.phb_entry.name, pb_contact.name.wstr);

				/*get mobile number*/
				tele_len = MMIAPICOM_GenDispNumber( 
							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_MOBILE].npi_ton), 
							(uint8)MIN(pb_contact.number[MR_PHB_MOBILE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
							pb_contact.number[MR_PHB_MOBILE].number,  
							temp_num,
							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
    						);
				MMI_STRNTOWSTR(g_phb_search_rsp.phb_entry.number, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/
			}

			/* 大小端转换 */
#ifndef WIN32
			mr_str_convert_endian_with_size(g_phb_search_rsp.phb_entry.name, MR_PHB_MAX_NAME);
			mr_str_convert_endian_with_size(g_phb_search_rsp.phb_entry.number, MR_PHB_MAX_NUMBER);
#endif
			
			*output = (uint8*)&g_phb_search_rsp;
			*output_len = sizeof(T_MR_PHB_SEARCH_RSP);
            return MR_PHB_SUCCESS;
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
}

uint16 MMIMRAPP_PHBGetMaxNum(int32 storage)
{
	uint16 total_num = 0;
	uint16 total_sim = 0;
	uint16 total_nv  = 0;

	MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetMaxNum storage = %d", storage));
	switch(storage)
    {
    case MR_PHB_SIM:
		total_sim = MMIPB_GetSimContactCount(MMIMRAPP_GetActiveSim());
		total_num = total_sim;
        break;
    case MR_PHB_NVM:
        total_nv = MMIAPIPB_GetPhoneContactCount();
		total_num = total_nv;
        break;
    case MR_PHB_BOTH:
		total_sim = MMIAPIPB_GetAllSimContactCount();
		total_nv = MMIAPIPB_GetPhoneContactCount();
		total_num = total_sim + total_nv;
        break;
    default:
        return MR_PHB_NOT_SUPPORT;
    }
	return total_num;
}

void MMIMRAPP_PHBSetEntryCallBack (MMIPB_HANDLE_T handle, MMIPB_ERROR_E error)
{
	if(g_pb_set_contact != NULL)
	{
		SCI_FREE(g_pb_set_contact);
	}
	
	if(MMIPB_ERROR_SUCCESS == error)
	{
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBCallBack op: %d, error: %d", g_phb_common_rsp.op, error));
    	mr_event(MR_PHB_EVENT, g_phb_common_rsp.op, MR_SUCCESS);
	}
	else
	{
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBCallBack op: %d, error: %d", g_phb_common_rsp.op, error));
    	mr_event(MR_PHB_EVENT, g_phb_common_rsp.op, MR_FAILED);
	}

	MMIAPIPB_CloseHandle(handle); /*lint !e718 !e18*/
}

int32 MMIMRAPP_PHBSetEntry(uint8* input, int32 input_len, uint8** output, int32* output_len)
{ /*lint !e18*/
    int32  result  = 0;
	int32  resValue = MR_FAILED;
	uint8  total = 0;
	uint16 contact_id = 0;
    uint16 storage_id= 0;
	uint16 search_count = 0;
	MMIPB_HANDLE_T handle = 0;
	MMIPB_HANDLE_T pb_handle = 0; 
	PHONEBOOK_STORAGE_E storage = 0;
    uint8  number[MR_PHB_MAX_NUMBER] = {0};
	MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_MAX;
	MMIPB_ERROR_E pb_result = MMIPB_ERROR_ERROR;
    MMI_PARTY_NUMBER_T party_num = {0};
    T_MR_PHB_SET_ENTRY_REQ *req = (T_MR_PHB_SET_ENTRY_REQ *)input;
	
    if(input == NULL || input_len != (int32)sizeof(T_MR_PHB_SET_ENTRY_REQ))
    {
        return MR_PHB_ERROR;
    }

    if(!MMIAPIPB_IsPbReady())
    {
        return MR_PHB_NOT_READY;
    }

	/* 大小端转换 */
#ifndef WIN32
	mr_str_convert_endian_with_size(req->entry.name, MR_PHB_MAX_NAME);
	mr_str_convert_endian_with_size(req->entry.number, MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(req->entry.homeNumber, MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(req->entry.officeNumber, MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(req->entry.faxNumber, MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(req->entry.emailAddress, MR_PHB_MAX_NUMBER);
#endif

	/* 删除 添加和修改操作的handle,在callback函数中释放 */
	handle = MMIAPIPB_CreateHandle(); /*lint !e718 !e18*/
	g_pb_set_contact = (MMIPB_CONTACT_T*)SCI_ALLOCA(sizeof(MMIPB_CONTACT_T));
	memset(g_pb_set_contact, 0, sizeof(MMIPB_CONTACT_T));

	/*
	 * This is an [Delete] operation.
	 */
    if(MMIAPICOM_Wstrlen(req->entry.name) == 0 && MMIAPICOM_Wstrlen(req->entry.number) == 0) /*lint !e64 !e64*/
    {
		//SCI_TRACE_LOW:"MMIMRAPP_PHBSetEntry del req->index = %d"
		SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_PHONEBOOK_319_112_2_18_3_12_51_4,(uint8*)"d",req->index);
		if(req->index > MMIMRAPP_PHBGetMaxNum(req->storage)) 
		{
			return MR_PHB_OUT_OF_INDEX;
		}

		g_phb_common_rsp.op = MR_PHB_OP_DELETE;
		
		pb_handle = MMIAPIPB_SearchList(NULL, MMIPB_LIST_FIELD_WHOLE, &search_count);
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry pb_handle = %d, search_count = %d", pb_handle, search_count));
		
		if(pb_handle > 0 && search_count > 0)
		{
    		int32 i = 0;
            int32 index = req->index;
            MN_DUAL_SYS_E dual_sys = MMI_DUAL_SYS_MAX;

            if(req->storage == MR_PHB_BOTH)
            {
    			result = MMIAPIPB_GetSearchEntryID(pb_handle, req->index, &contact_id, &storage_id);

    			if(result)
    			{
    				pb_result = MMIAPIPB_DeleteContactAsyn(handle, contact_id, storage_id, MMIMRAPP_PHBSetEntryCallBack);
		            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
                    {
            			resValue = MR_SUCCESS;
                    }
                    else
                    {
                        resValue = MR_FAILED;
                    }
    			}
            }
            else if(req->storage == MR_PHB_NVM)
            {
        		for(i=0; i<search_count; i++)
                {
                    result = MMIAPIPB_GetSearchEntryID(pb_handle, i, &contact_id, &storage_id);

                    if(result == FALSE)
                    {
                        resValue = MR_FAILED;
                        break;
                    }
                    
                    if(MMIPB_IsInNvGroupByStorage(storage_id))
                    {
                        if(index == 0)
                        {
                            pb_result = MMIAPIPB_DeleteContactAsyn(handle, contact_id, storage_id, MMIMRAPP_PHBSetEntryCallBack);
				            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
                            {
                    			resValue = MR_SUCCESS;
                            }
                            else
                            {
                                resValue = MR_FAILED;
                            }
                            break;
                        }
                        else
                        {
                            index--;
                        }
                    }
                } 
            }     
            else if(req->storage == MR_PHB_SIM)
            {
        		for(i=0; i<search_count; i++)
                {
                    result = MMIAPIPB_GetSearchEntryID(pb_handle, i, &contact_id, &storage_id);

                    if(result == FALSE)
                    {
                        resValue = MR_FAILED;
                        break;
                    }

                    dual_sys = MMIPB_GetDualSysByStorageId(storage_id);
                    MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry dual_sys = %d", dual_sys));
                    if(dual_sys == MMIMRAPP_GetActiveSim())
                    {
                        if(index == 0)
                        {
                            pb_result = MMIAPIPB_DeleteContactAsyn(handle, contact_id, storage_id, MMIMRAPP_PHBSetEntryCallBack);
				            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
                            {
                    			resValue = MR_SUCCESS;
                            }
                            else
                            {
                                resValue = MR_FAILED;
                            }
                            break;
                        }
                        else
                        {
                            index--;
                        }
                    }
                } 
            }
            else
            {
                resValue = MR_FAILED;
            }
		}
		else
		{
			resValue = MR_FAILED;
		}
    }
    else
    {
    	/*
    	 * This is an [Add] operation.
		 */
		g_pb_set_contact->name.wstr_len = MIN(MMIPB_NAME_MAX_LEN, MMIAPICOM_Wstrlen(req->entry.name)); /*lint !e666 !e64 !e64*/
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry name_len :%d", g_pb_set_contact->name.wstr_len));
		MMIAPICOM_Wstrncpy(g_pb_set_contact->name.wstr, req->entry.name, g_pb_set_contact->name.wstr_len*2); /*lint !e64*/

    	//set mobile number
		SCI_MEMSET(&party_num, 0x00, sizeof(MMI_PARTY_NUMBER_T));
		MMIAPICOM_WstrToStr(req->entry.number, number); /*lint !e64*/
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry num[%d]:%s", MR_PHB_MOBILE, number));
		MMIAPICOM_GenPartyNumber((uint8*)number, (int16)strlen(number), &party_num); /*lint !e64*/
		g_pb_set_contact->number[MR_PHB_MOBILE].number_len = (uint8)party_num.num_len;
		g_pb_set_contact->number[MR_PHB_MOBILE].npi_ton = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, 0);
		MMI_MEMCPY(g_pb_set_contact->number[MR_PHB_MOBILE].number, MMIPB_BCD_NUMBER_MAX_LEN, party_num.bcd_num, party_num.num_len, party_num.num_len);

   	 	//set home number
		SCI_MEMSET(&party_num, 0x00, sizeof(MMI_PARTY_NUMBER_T));
		MMIAPICOM_WstrToStr(req->entry.homeNumber, number); /*lint !e64*/
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry num[%d]:%s", MR_PHB_HOME, number));
		MMIAPICOM_GenPartyNumber((uint8*)number, (int16)strlen(number), &party_num); /*lint !e64*/
		g_pb_set_contact->number[MR_PHB_HOME].number_len = (uint8)party_num.num_len;
		g_pb_set_contact->number[MR_PHB_HOME].npi_ton = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, 0);
		MMI_MEMCPY(g_pb_set_contact->number[MR_PHB_HOME].number, MMIPB_BCD_NUMBER_MAX_LEN, party_num.bcd_num, party_num.num_len, party_num.num_len);

#ifndef PB_SUPPORT_LOW_MEMORY
    	//set office number
		SCI_MEMSET(&party_num, 0x00, sizeof(MMI_PARTY_NUMBER_T));
		MMIAPICOM_WstrToStr(req->entry.officeNumber, number); /*lint !e64*/
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry num[%d]:%s", MR_PHB_OFFICE, number));
		MMIAPICOM_GenPartyNumber((uint8*)number, (int16)strlen(number), &party_num); /*lint !e64*/
		g_pb_set_contact->number[MR_PHB_OFFICE].number_len = (uint8)party_num.num_len; 
		g_pb_set_contact->number[MR_PHB_OFFICE].npi_ton = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, 0); 
		MMI_MEMCPY(g_pb_set_contact->number[MR_PHB_OFFICE].number, MMIPB_BCD_NUMBER_MAX_LEN, party_num.bcd_num, party_num.num_len, party_num.num_len); 

    	//set fax number
		SCI_MEMSET(&party_num, 0x00, sizeof(MMI_PARTY_NUMBER_T));
		MMIAPICOM_WstrToStr(req->entry.faxNumber, number); /*lint !e64*/
		MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry num[%d]:%s", MR_PHB_FAX, number));
		MMIAPICOM_GenPartyNumber((uint8*)number, (int16)strlen(number), &party_num); /*lint !e64*/
		g_pb_set_contact->number[MR_PHB_FAX].number_len = (uint8)party_num.num_len; 
		g_pb_set_contact->number[MR_PHB_FAX].npi_ton = MMIAPICOM_ChangeTypePlanToUint8(party_num.number_type, 0); 
		MMI_MEMCPY(g_pb_set_contact->number[MR_PHB_FAX].number, MMIPB_BCD_NUMBER_MAX_LEN, party_num.bcd_num, party_num.num_len, party_num.num_len); 
#endif
#ifdef MMIPB_MAIL_SUPPORT
		//set email
		MMIAPICOM_Wstrcpy(g_pb_set_contact->mail.wstr, req->entry.emailAddress); /*lint !e64*/
		g_pb_set_contact->mail.wstr_len = MMIAPICOM_Wstrlen(g_pb_set_contact->mail.wstr);
#endif
		if(req->index == 0xFFFF)
		{
			g_phb_common_rsp.op = MR_PHB_OP_WRITE;
			//SCI_TRACE_LOW:"MMIMRAPP_PHBSetEntry add req->index = %d"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_PHONEBOOK_399_112_2_18_3_12_51_5,(uint8*)"d",req->index);

			if(req->storage == MR_PHB_NVM)
			{
				storage = PHONEBOOK_NV_STORAGE;
			}
			else if(req->storage == MR_PHB_SIM)
			{
				storage = PHONEBOOK_SIM_STORAGE;
				dual_sys = MMIMRAPP_GetActiveSim(); /*lint !e718*/
			}
			else
			{
				storage = PHONEBOOK_NV_STORAGE;
			}
			storage_id = MMIAPIPB_GetStorageId(dual_sys, storage);

			g_pb_set_contact->storage_id = storage_id;
			g_pb_set_contact->contact_id = 0;
			g_pb_set_contact->name.wstr_len = MMIAPICOM_Wstrlen(g_pb_set_contact->name.wstr);
			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry name_len :%d", g_pb_set_contact->name.wstr_len));
			
			pb_result = MMIAPIPB_AddContactAsyn(handle, g_pb_set_contact, MMIMRAPP_PHBSetEntryCallBack);
			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry pb_result :%d", pb_result));

            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
            {
    			resValue = MR_SUCCESS;
            }
            else
            {
                resValue = MR_FAILED;
            }
		}
		/*
		 * This is an [Modify] operation.
		 */
		else
		{
			g_phb_common_rsp.op = MR_PHB_OP_EDIT;

			//SCI_TRACE_LOW:"MMIMRAPP_PHBSetEntry modify req->index = %d"
			SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIMRAPP_PHONEBOOK_432_112_2_18_3_12_51_6,(uint8*)"d",req->index);
			pb_handle = MMIAPIPB_SearchList(NULL, MMIPB_LIST_FIELD_WHOLE, &search_count);
			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry pb_handle = %d, search_count = %d", pb_handle, search_count));

			if(pb_handle > 0 && search_count > 0)
			{
                int32 i = 0;
                int32 index = req->index;
                MN_DUAL_SYS_E dual_sys = MMI_DUAL_SYS_MAX;

                if(req->storage == MR_PHB_BOTH)
                {
    				result = MMIAPIPB_GetSearchEntryID(pb_handle, req->index, &contact_id, &storage_id);

    				if(result)
    				{
    					g_pb_set_contact->contact_id = contact_id;
    					g_pb_set_contact->storage_id = storage_id;
    					g_pb_set_contact->name.wstr_len = MMIAPICOM_Wstrlen(g_pb_set_contact->name.wstr);
    					pb_result = MMIAPIPB_ModifyContactAsyn(handle, g_pb_set_contact, MMIMRAPP_PHBSetEntryCallBack);

			            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
                        {
                			resValue = MR_SUCCESS;
                        }
                        else
                        {
                            resValue = MR_FAILED;
                        }
    				}
                }
                else if(req->storage == MR_PHB_NVM)
                {
            		for(i=0; i<search_count; i++)
                    {
                        result = MMIAPIPB_GetSearchEntryID(pb_handle, i, &contact_id, &storage_id);

                        if(result == FALSE)
                        {
                            resValue = MR_FAILED;
                            break;
                        }
                        
                        if(MMIPB_IsInNvGroupByStorage(storage_id))
                        {
                            if(index == 0)
                            {
            					g_pb_set_contact->contact_id = contact_id;
            					g_pb_set_contact->storage_id = storage_id;
            					g_pb_set_contact->name.wstr_len = MMIAPICOM_Wstrlen(g_pb_set_contact->name.wstr);
            					pb_result = MMIAPIPB_ModifyContactAsyn(handle, g_pb_set_contact, MMIMRAPP_PHBSetEntryCallBack);
					            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
                                {
                        			resValue = MR_SUCCESS;
                                }
                                else
                                {
                                    resValue = MR_FAILED;
                                }
                                break;
                            }
                            else
                            {
                                index--;
                            }
                        }
                    } 
                }     
                else if(req->storage == MR_PHB_SIM)
                {
            		for(i=0; i<search_count; i++)
                    {
                        result = MMIAPIPB_GetSearchEntryID(pb_handle, i, &contact_id, &storage_id);

                        if(result == FALSE)
                        {
                            resValue = MR_FAILED;
                            break;
                        }

                        dual_sys = MMIPB_GetDualSysByStorageId(storage_id);
                        MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry dual_sys = %d", dual_sys));
                        if(dual_sys == MMIMRAPP_GetActiveSim())
                        {
                            if(index == 0)
                            {
            					g_pb_set_contact->contact_id = contact_id;
            					g_pb_set_contact->storage_id = storage_id;
            					g_pb_set_contact->name.wstr_len = MMIAPICOM_Wstrlen(g_pb_set_contact->name.wstr);
            					pb_result = MMIAPIPB_ModifyContactAsyn(handle, g_pb_set_contact, MMIMRAPP_PHBSetEntryCallBack);
					            if(pb_result == MMIPB_ERROR_SUCCESS || pb_result == MMIPB_ERROR_PENDING)
                                {
                        			resValue = MR_SUCCESS;
                                }
                                else
                                {
                                    resValue = MR_FAILED;
                                }
                                break;
                            }
                            else
                            {
                                index--;
                            }
                        }
                    } 
                }
                else
                {
                    resValue = MR_FAILED;
                }
			}
			else
			{
				resValue = MR_FAILED;
			}
		}
    }

    //MMIMRAPP_StartPHBOperateTimer(); /*lint !e718 !e18*/
    /* 退出搜索的handle */
    MMIAPIPB_SearchExit(pb_handle);
    MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry op: %d result: %d", g_phb_common_rsp.op, resValue));

    if(resValue == MR_SUCCESS)
    {
        return MR_PHB_SUCCESS;
    }
    else
    {
        return MR_PHB_ERROR;
    }
}


int32 MMIMRAPP_PHBGetEntry(uint8* input, int32 input_len, uint8** output, int32* output_len)
{ /*lint !e18*/
    BOOLEAN result = FALSE;
	MMIPB_ERROR_E getEntry_res = MMIPB_ERROR_ERROR;
    MMIPB_CONTACT_T pb_contact = {0};
    uint16    contact_id = 0;
	uint16	  storage_id = 0;
	uint8	tele_len = 0;
	uint16 search_count = 0;
	MMIPB_HANDLE_T pb_handle = 0;
	uint8	temp_num[MMIPB_BCD_NUMBER_MAX_LEN*2 + 2]= {0};
    int32   resValue = MR_FAILED;
    T_MR_PHB_GET_ENTRY_REQ *req = (T_MR_PHB_GET_ENTRY_REQ *)input;

	MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry index: %d storage: %d", req->index, req->storage));
    if(input == NULL || input_len == 0 || output == NULL || output_len == NULL)
    {
        return MR_PHB_ERROR;
    }

    if(!MMIAPIPB_IsPbReady())
    {
        return MR_PHB_NOT_READY;
    }

	if (req->index >= MMIMRAPP_PHBGetMaxNum(req->storage))
	{
		return MR_PHB_OUT_OF_INDEX;
	}
	
	pb_handle = MMIAPIPB_SearchList(NULL, MMIPB_LIST_FIELD_WHOLE, &search_count);
	MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry pb_handle = %d, search_count = %d", pb_handle, search_count));

	if(pb_handle > 0 && search_count > 0)
	{
		int32 i = 0;
        int32 index = req->index;
        MN_DUAL_SYS_E dual_sys = MMI_DUAL_SYS_MAX;

        if(req->storage == MR_PHB_BOTH)
        {
    		result = MMIAPIPB_GetSearchEntryID(pb_handle, req->index, &contact_id, &storage_id);
		
    		if(result)
    		{
    			getEntry_res = MMIAPIPB_GetContactInfo(contact_id, storage_id, &pb_contact);
    			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry getEntry_res = %d", getEntry_res));

    			if(MMIPB_ERROR_SUCCESS == getEntry_res)
        		{
        			SCI_MEMSET(&g_phb_get_entry_rsp,0x0,sizeof(T_MR_PHB_GET_ENTRY_RSP));
            		/*get name*/
    				MMIAPICOM_Wstrcpy((uint16*)g_phb_get_entry_rsp.entry.name, pb_contact.name.wstr);

    				/*get mobile number*/
    				tele_len = MMIAPICOM_GenDispNumber( 
    							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_MOBILE].npi_ton), 
    							(uint8)MIN(pb_contact.number[MR_PHB_MOBILE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
    							pb_contact.number[MR_PHB_MOBILE].number,  
    							temp_num,
    							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
        						);
    				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.number, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

    				/*get home number*/
    				tele_len = MMIAPICOM_GenDispNumber( 
    							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_HOME].npi_ton), 
    							(uint8)MIN(pb_contact.number[MR_PHB_HOME].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
    							pb_contact.number[MR_PHB_HOME].number,  
    							temp_num,
    							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
        						);
    				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.homeNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

#ifndef PB_SUPPORT_LOW_MEMORY
    				/*get office number*/
    				tele_len = MMIAPICOM_GenDispNumber( 
    							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_OFFICE].npi_ton), 
    							(uint8)MIN(pb_contact.number[MR_PHB_OFFICE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
    							pb_contact.number[MR_PHB_OFFICE].number,  
    							temp_num,
    							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
        						);
    				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.officeNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

    				/*get fax number*/
    				tele_len = MMIAPICOM_GenDispNumber( 
    							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_FAX].npi_ton), 
    							(uint8)MIN(pb_contact.number[MR_PHB_FAX].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
    							pb_contact.number[MR_PHB_FAX].number,  
    							temp_num,
    							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
        						);
    				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.faxNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/
        
    				/*get email*/
    				MMIAPICOM_Wstrcpy((uint16*)g_phb_get_entry_rsp.entry.emailAddress, pb_contact.mail.wstr);
#endif
    				/* 大小端转换 */
#ifndef WIN32
    				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.name, MR_PHB_MAX_NAME);
    				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.number, MR_PHB_MAX_NUMBER);
    				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.homeNumber, MR_PHB_MAX_NUMBER);
    				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.officeNumber, MR_PHB_MAX_NUMBER);
    				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.faxNumber, MR_PHB_MAX_NUMBER);
    				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.emailAddress, MR_PHB_MAX_NUMBER);
#endif
    		
            		*output = (uint8*)&g_phb_get_entry_rsp;
            		*output_len = sizeof(T_MR_PHB_GET_ENTRY_RSP);

                    resValue = MR_SUCCESS;
        		}
    		}
        }
        else if(req->storage == MR_PHB_NVM)
        {
            for(i=0; i<search_count; i++)
            {
                result = MMIAPIPB_GetSearchEntryID(pb_handle, i, &contact_id, &storage_id);

                if(result == FALSE)
                {
                    resValue = MR_FAILED;
                    break;
                }
                
                if(MMIPB_IsInNvGroupByStorage(storage_id))
                {
                    if(index == 0)
                    {
            			getEntry_res = MMIAPIPB_GetContactInfo(contact_id, storage_id, &pb_contact);
            			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry getEntry_res = %d", getEntry_res));

            			if(MMIPB_ERROR_SUCCESS == getEntry_res)
                		{
                			SCI_MEMSET(&g_phb_get_entry_rsp,0x0,sizeof(T_MR_PHB_GET_ENTRY_RSP));
                    		/*get name*/
            				MMIAPICOM_Wstrcpy((uint16*)g_phb_get_entry_rsp.entry.name, pb_contact.name.wstr);

            				/*get mobile number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_MOBILE].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_MOBILE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_MOBILE].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.number, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

            				/*get home number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_HOME].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_HOME].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_HOME].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.homeNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

#ifndef PB_SUPPORT_LOW_MEMORY
            				/*get office number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_OFFICE].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_OFFICE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_OFFICE].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.officeNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

            				/*get fax number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_FAX].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_FAX].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_FAX].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.faxNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/
                
            				/*get email*/
            				MMIAPICOM_Wstrcpy((uint16*)g_phb_get_entry_rsp.entry.emailAddress, pb_contact.mail.wstr);
#endif

            				/* 大小端转换 */
#ifndef WIN32
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.name, MR_PHB_MAX_NAME);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.number, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.homeNumber, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.officeNumber, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.faxNumber, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.emailAddress, MR_PHB_MAX_NUMBER);
#endif
            		
                    		*output = (uint8*)&g_phb_get_entry_rsp;
                    		*output_len = sizeof(T_MR_PHB_GET_ENTRY_RSP);
                            resValue = MR_SUCCESS;
                         }
                         break;
                    }
                    else
                    {
                        index--;
                    }
                }
            } 
        }
        else if(req->storage == MR_PHB_SIM)
        {
            for(i=0; i<search_count; i++)
            {
                result = MMIAPIPB_GetSearchEntryID(pb_handle, i, &contact_id, &storage_id);

                if(result == FALSE)
                {
                    resValue = MR_FAILED;
                    break;
                }

                dual_sys = MMIPB_GetDualSysByStorageId(storage_id);
                MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBSetEntry dual_sys = %d", dual_sys));
                if(dual_sys == MMIMRAPP_GetActiveSim())
                {
                    if(index == 0)
                    {
            			getEntry_res = MMIAPIPB_GetContactInfo(contact_id, storage_id, &pb_contact);
            			MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetEntry getEntry_res = %d", getEntry_res));

            			if(MMIPB_ERROR_SUCCESS == getEntry_res)
                		{
                			SCI_MEMSET(&g_phb_get_entry_rsp,0x0,sizeof(T_MR_PHB_GET_ENTRY_RSP));
                    		/*get name*/
            				MMIAPICOM_Wstrcpy((uint16*)g_phb_get_entry_rsp.entry.name, pb_contact.name.wstr);

            				/*get mobile number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_MOBILE].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_MOBILE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_MOBILE].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.number, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

            				/*get home number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_HOME].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_HOME].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_HOME].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.homeNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

#ifndef PB_SUPPORT_LOW_MEMORY
            				/*get office number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_OFFICE].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_OFFICE].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_OFFICE].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.officeNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/

            				/*get fax number*/
            				tele_len = MMIAPICOM_GenDispNumber( 
            							MMIPB_GetNumberTypeFromUint8(pb_contact.number[MR_PHB_FAX].npi_ton), 
            							(uint8)MIN(pb_contact.number[MR_PHB_FAX].number_len, MMIPB_BCD_NUMBER_MAX_LEN), 
            							pb_contact.number[MR_PHB_FAX].number,  
            							temp_num,
            							(MMIPB_BCD_NUMBER_MAX_LEN*2 + 2)
                						);
            				MMI_STRNTOWSTR(g_phb_get_entry_rsp.entry.faxNumber, (MR_PHB_MAX_NUMBER-1)/2, temp_num, (MMIPB_BCD_NUMBER_MAX_LEN*2 + 2), tele_len); /*lint !e64*/
                
            				/*get email*/
            				MMIAPICOM_Wstrcpy((uint16*)g_phb_get_entry_rsp.entry.emailAddress, pb_contact.mail.wstr);
#endif

            				/* 大小端转换 */
#ifndef WIN32
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.name, MR_PHB_MAX_NAME);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.number, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.homeNumber, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.officeNumber, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.faxNumber, MR_PHB_MAX_NUMBER);
            				mr_str_convert_endian_with_size(g_phb_get_entry_rsp.entry.emailAddress, MR_PHB_MAX_NUMBER);
#endif
            		
                    		*output = (uint8*)&g_phb_get_entry_rsp;
                    		*output_len = sizeof(T_MR_PHB_GET_ENTRY_RSP);
                            resValue = MR_SUCCESS;
                        }
                        break;
                    }
                    else
                    {
                        index--;
                    }
                }
            }
        }
	}

	MMIAPIPB_SearchExit(pb_handle);
    if(resValue == MR_SUCCESS)
    {
		return MR_PHB_SUCCESS;
    }
	else
    {
        return MR_PHB_ERROR;
    }
}

int32 MMIMRAPP_PHBGetCount(uint8* input, int32 input_len, uint8** output, int32* output_len)
{ /*lint !e18*/
	int i = 0;
	uint16 storage_id = 0;
    uint16 total_nv  = 0;
    uint16 total_sim = 0;
    T_MR_PHB_GET_COUNT_REQ *req = NULL;
    
    if(input == NULL || input_len == 0 || output == NULL || output_len == NULL)
    {
        return MR_FAILED;
    }

    if(!MMIAPIPB_IsPbReady())
    {
        return MR_PHB_NOT_READY;
    }

    req = (T_MR_PHB_GET_COUNT_REQ *)input;
    MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetCount storage=%d", req->storage));
    switch(req->storage)
    {
    case MR_PHB_SIM:
		total_sim = MMIPB_GetSimContactCount(MMIMRAPP_GetActiveSim());
        break;
    case MR_PHB_NVM:
        total_nv = MMIAPIPB_GetPhoneContactCount();
        break;
    case MR_PHB_BOTH:
		total_sim = MMIAPIPB_GetAllSimContactCount();
		total_nv = MMIAPIPB_GetPhoneContactCount();
        break;
    default:
        return MR_PHB_NOT_SUPPORT;
    }

    SCI_MEMSET(&g_phb_getcount_rsp, 0x00, sizeof(T_MR_PHB_GET_COUNT_RSP));
    g_phb_getcount_rsp.count = (int32)(total_sim + total_nv);

    *output = (uint8*)&g_phb_getcount_rsp;
    *output_len = sizeof(T_MR_PHB_GET_COUNT_RSP);
	MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetCount total=%d sim:%d nv:%d", g_phb_getcount_rsp.count, total_sim, total_nv));
    return MR_PHB_SUCCESS;
}

int32 MMIMRAPP_PHBGetStatus(void)
{ /*lint !e18*/
    if(!MMIAPIPB_IsPbReady())
    {
        return MR_PHB_NOT_READY;
    }
    else
    {
        return MR_PHB_READY;
    }
}

void MMIMRAPP_PHBCallBack(void)
{ /*lint !e18*/
    MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBCallBack op: %d result: %d", g_phb_common_rsp.op, g_phb_common_rsp.result));
    mr_event(MR_PHB_EVENT, g_phb_common_rsp.op, g_phb_common_rsp.result);
}

int32 MMIMRAPP_PHBGetTotalNum(uint8* input, int32 input_len, uint8** output, int32* output_len)
{ /*lint !e18*/
	int i = 0;
    uint16 total_sim[MN_DUAL_SYS_MAX] = {0};
	T_MR_PHB_GET_TOTAL_RSP totalPHBNum = {0};

    if(output == NULL || output_len == NULL || *output == NULL)
    {
        return MR_FAILED;
    }

    SCI_MEMSET(&totalPHBNum, 0x00, sizeof(T_MR_PHB_GET_TOTAL_RSP));                
    totalPHBNum.phone = (int32)MMIPB_GetMaxContactNumOfNV();

    for(i=0; i<MN_DUAL_SYS_MAX; i++)
    {
		if(MMIAPIPHONE_GetSimExistedStatus(i))
		{
        	total_sim[i] = MMIPB_GetMaxContactNumOfSim(i);
		}
    }

#if defined MULTI_SIM_SYS_SINGLE 
    totalPHBNum.sim  = (int32)total_sim[0];
#elif defined MULTI_SIM_SYS_DUAL
	totalPHBNum.sim  = (int32)total_sim[0];
    totalPHBNum.sim2 = (int32)total_sim[1];
#elif defined MULTI_SIM_SYS_TRI
	totalPHBNum.sim  = (int32)total_sim[0];
    totalPHBNum.sim2 = (int32)total_sim[1];
    totalPHBNum.sim3 = (int32)total_sim[2];
#elif defined MULTI_SIM_SYS_QUAD
	totalPHBNum.sim  = (int32)total_sim[0];
    totalPHBNum.sim2 = (int32)total_sim[1];
    totalPHBNum.sim3 = (int32)total_sim[2];
    totalPHBNum.sim4 = (int32)total_sim[3];
#endif

	MMIMR_Trace(("MRAPP[PB] MMIMRAPP_PHBGetTotalNum phone:%d sim:%d sim2:%d sim3:%d sim4:%d", totalPHBNum.phone,
										totalPHBNum.sim, totalPHBNum.sim2, totalPHBNum.sim3, totalPHBNum.sim4));
	
	SCI_MEMCPY(*output, &totalPHBNum, sizeof(T_MR_PHB_GET_TOTAL_RSP));
    *output_len = sizeof(T_MR_PHB_GET_TOTAL_RSP);

    return MR_SUCCESS;
}

#endif
