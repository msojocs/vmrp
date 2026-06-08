#ifdef __MMI_DSM_NEW__
#define __NEWSIMULATOR 
#include "MMI_features.h" 
#include "stdC.h"
#include "PixtelDataTypes.h"

#include "GlobalDefs.h"
#include "CommonScreens.h"
#include "PhbSrvGprot.h"
#include "SettingDefs.h" /*For FDN on check*/
#include "SettingsGdcl.h"
#include "Nvram_user_defs.h"
#include "Ucs2prot.h"

#include "mrp_include.h"

#undef __NEWSIMULATOR

T_MR_PHB_SEARCH_RSP phb_search_rsp;
T_MR_PHB_SET_ENTRY_RSP phb_set_entry_rsp;
T_MR_PHB_GET_ENTRY_RSP phb_get_entry_rsp;
T_MR_PHB_SET_OWNER_ENTRY_RSP phb_set_owner_entry_rsp;
T_MR_PHB_GET_OWNER_ENTRY_RSP phb_get_owner_entry_rsp;
//T_MR_PHB_GET_COUNT_RSP phb_get_count_rsp;

T_MR_PHB_ENTRY  phb_entry;

extern U16 PhoneBookEntryCount; 
extern U8 g_phb_enter_from;
extern U16 g_phb_name_index[];

extern S8 pbName[];
extern S8 pbNumber[];
#if !defined(__MMI_PHB_NO_OPTIONAL_FIELD__) || defined(__MMI_PHB_USIM_FIELD__)
extern S8 pbHomeNumber[];
extern S8 pbOfficeNumber[];
extern S8 pbFaxNumber[];
extern S8 pbEmailAddress[];
#endif

extern U8 g_phb_enter_from;    

int mr_phone_win = 1;

#define   __SKY_LOG_PB__
#ifdef __SKY_LOG_PB__
	#define log_pb                 mr_printf
#else
	#define log_pb(...)
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////
 
#if (MTK_VERSION < 0x09B0952)
#include ".\mt07a-09a\mrp_phonebook(07a-09a).c"
#else
#include ".\mt09b\mrp_phonebook(09b).c"
#endif 
 

int32 mr_phonebook_get_contact_item(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if((output == 0)||(output_len == 0))
	{
		return MR_FAILED;
	}
	
	*output =(uint8*) &phb_entry;
	*output_len = sizeof(T_MR_PHB_ENTRY);

	return MR_SUCCESS;
}



int32 mr_phonebook_search(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_MR_PHB_SEARCH_REQ * phb_search_req = (T_MR_PHB_SEARCH_REQ*)input;
	int32 srch_rst;

	if((phb_search_req == NULL)||(phb_search_req->pattern == NULL))
		return MR_PHB_ERROR;
	
	mr_str_convert_endian_with_size(phb_search_req->pattern,MR_PHB_MAX_SEARCH_PATTERN);
	
	srch_rst = _mmi_dsm_phb_search_entry(phb_search_req,&phb_search_rsp);

	if (srch_rst  ==  MR_PHB_SUCCESS)
	{
		mr_str_convert_endian_with_size(phb_search_rsp.phb_entry.name,MR_PHB_MAX_NAME);
		mr_str_convert_endian_with_size(phb_search_rsp.phb_entry.number,MR_PHB_MAX_NUMBER);
		mr_str_convert_endian_with_size(phb_search_rsp.phb_entry.homeNumber,MR_PHB_MAX_NUMBER);
		mr_str_convert_endian_with_size(phb_search_rsp.phb_entry.officeNumber,MR_PHB_MAX_NUMBER);
		mr_str_convert_endian_with_size(phb_search_rsp.phb_entry.faxNumber,MR_PHB_MAX_NUMBER);
		mr_str_convert_endian_with_size(phb_search_rsp.phb_entry.emailAddress,MR_PHB_MAX_NUMBER);
		*output = (uint8*)&phb_search_rsp;
		*output_len = sizeof(phb_search_rsp);
	}
	return srch_rst;
}


int32 mr_phonebook_set_entry(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_MR_PHB_SET_ENTRY_REQ *phb_set_entry_req = (T_MR_PHB_SET_ENTRY_REQ*)input;
	int32 set_rst;

	if(phb_set_entry_req == NULL)
		return MR_PHB_ERROR;
		
	mr_str_convert_endian_with_size(phb_set_entry_req->entry.name,MR_PHB_MAX_NAME);
	mr_str_convert_endian_with_size(phb_set_entry_req->entry.number,MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(phb_set_entry_req->entry.homeNumber,MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(phb_set_entry_req->entry.officeNumber,MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(phb_set_entry_req->entry.faxNumber,MR_PHB_MAX_NUMBER);
	mr_str_convert_endian_with_size(phb_set_entry_req->entry.emailAddress,MR_PHB_MAX_NUMBER);
	
	set_rst = _mmi_dsm_phb_write_entry(phb_set_entry_req);

	log_pb("set_rst=%d",set_rst);
	
	if(set_rst == MR_PHB_SUCCESS)
	{
		*output = (uint8*)&phb_set_entry_rsp;
		*output_len = sizeof(phb_set_entry_rsp);
	}
	return set_rst;
}


int32 mr_phonebook_get_entry(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_MR_PHB_GET_ENTRY_REQ *phb_get_entry_req = (T_MR_PHB_GET_ENTRY_REQ*)input;
	int32 read_rst;

	if(phb_get_entry_req == NULL)
		return MR_PHB_ERROR;
	
	read_rst = _mmi_dsm_phb_read_entry(phb_get_entry_req,&phb_get_entry_rsp);

	if(read_rst == MR_PHB_SUCCESS)
	{
		mr_str_convert_endian_with_size(phb_get_entry_rsp.entry.name,MR_PHB_MAX_NAME);
		mr_str_convert_endian_with_size(phb_get_entry_rsp.entry.number,MR_PHB_MAX_NAME);
		mr_str_convert_endian_with_size(phb_get_entry_rsp.entry.homeNumber,MR_PHB_MAX_NAME);
		mr_str_convert_endian_with_size(phb_get_entry_rsp.entry.officeNumber,MR_PHB_MAX_NAME);
		mr_str_convert_endian_with_size(phb_get_entry_rsp.entry.faxNumber,MR_PHB_MAX_NAME);
		mr_str_convert_endian_with_size(phb_get_entry_rsp.entry.emailAddress,MR_PHB_MAX_NAME);
		
		*output = (uint8*)&phb_get_entry_rsp;
		*output_len = sizeof(phb_get_entry_rsp);
	}
	return read_rst;
}


int32 mr_phonebook_get_count(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_MR_PHB_GET_COUNT_REQ *phb_get_count_req = (T_MR_PHB_GET_COUNT_REQ*)input;
	int cnt_rst;

	if(phb_get_count_req == NULL)
		return MR_PHB_ERROR;
	
	cnt_rst = _mmi_dsm_phb_get_count(phb_get_count_req,(T_MR_PHB_GET_COUNT_RSP*)&g_mr_common_rsp);
	
	if(cnt_rst == MR_PHB_SUCCESS)
	{
		*output = (uint8*)&g_mr_common_rsp;
		*output_len = sizeof(int32);
	}
	return cnt_rst;
}


int32 mr_phonebook_show_contact_list(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	extern int mr_phone_win;
	mmi_dsm_entry_phone_book();
	if(mr_phone_win == 1)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 mr_phonebook_get_total_num(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_MR_PHB_GET_TOTAL_RSP * pTotal;
	if( output)
	{
		pTotal = (T_MR_PHB_GET_TOTAL_RSP *)(*output);
		pTotal->phone = srv_phb_get_total_contact(PHB_STORAGE_NVRAM);
		pTotal->sim =  srv_phb_get_total_contact(PHB_STORAGE_SIM);
		pTotal->sim2= 0;
		pTotal->sim3= 0;
		pTotal->sim4= 0;
#ifdef __MMI_DUAL_SIM_MASTER__

		pTotal->sim2= srv_phb_get_total_contact(PHB_STORAGE_SIM2);

		//三卡宏,不同厂商,宏名称可能不一样
		#ifdef __RAGENT_SIM3_SUPPORT__
		pTotal->sim3= srv_phb_get_total_contact(PHB_STORAGE_SIM3);
		#endif
		
		//四卡宏,不同厂商,宏名称可能不一样
		#ifdef  __RAGENT_SIM4_SUPPORT__
		pTotal->sim4= srv_phb_get_total_contact(PHB_STORAGE_SIM4);
		#endif
		
#else

		pTotal->sim2= 0;
		pTotal->sim3= 0;
		pTotal->sim4= 0;
		
#endif
		*output_len = sizeof(T_MR_PHB_GET_TOTAL_RSP);
		return MR_SUCCESS;
	}
	return MR_FAILED;
       
}

#endif

