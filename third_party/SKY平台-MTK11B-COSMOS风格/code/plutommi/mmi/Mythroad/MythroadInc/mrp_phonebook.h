#ifndef __MMI_DSM_PHONE_BOOK_H__
#define __MMI_DSM_PHONE_BOOK_H__



typedef struct 
{
	S8	name[MR_PHB_MAX_NAME];
	S8	number[MR_PHB_MAX_NUMBER];
	S8	homeNumber[MR_PHB_MAX_NUMBER];
	S8	officeNumber[MR_PHB_MAX_NUMBER];
	S8	faxNumber[MR_PHB_MAX_NUMBER];
	S8	emailAddress[MR_PHB_MAX_NUMBER];
}T_MR_PHB_ENTRY;


typedef struct
{
	S8 pattern[MR_PHB_MAX_SEARCH_PATTERN];
	U16 search_type;
	U16 storage;
}T_MR_PHB_SEARCH_REQ;


typedef struct
{
	U16 index;
	U16 store_index;
	T_MR_PHB_ENTRY phb_entry;
}T_MR_PHB_SEARCH_RSP;


typedef struct 
{
	U16 storage;
	U16 index;
	T_MR_PHB_ENTRY entry;
}T_MR_PHB_SET_ENTRY_REQ;


typedef struct
{
	U16 storage_index;
} T_MR_PHB_SET_ENTRY_RSP;


typedef struct
{
	U16 index;
	U16  storage;
}T_MR_PHB_GET_ENTRY_REQ;


typedef struct
{
	T_MR_PHB_ENTRY entry;
}T_MR_PHB_GET_ENTRY_RSP;


typedef struct
{
	U16 index;
	//U16  storage;
}T_MR_PHB_GET_OWNER_ENTRY_REQ;


typedef struct
{
	T_MR_PHB_ENTRY entry;
}T_MR_PHB_GET_OWNER_ENTRY_RSP;


typedef struct
{
	T_MR_PHB_ENTRY entry;
}T_MR_PHB_SET_OWNER_ENTRY_REQ;


typedef struct
{
	int32 noused;
}T_MR_PHB_SET_OWNER_ENTRY_RSP;


typedef struct
{
	int32 storage;
}T_MR_PHB_GET_COUNT_REQ;


typedef struct
{
	int32 count;
}T_MR_PHB_GET_COUNT_RSP;


enum
{
	MR_PHB_OP_WRITE,
	MR_PHB_OP_READ,
	MR_PHB_OP_EDIT,
	MR_PHB_OP_DELETE
};

typedef struct
{
	int32 phone;
	int32 sim;
	int32 sim2;
	int32 sim3;
	int32 sim4;
}T_MR_PHB_GET_TOTAL_RSP;

extern T_MR_PHB_SEARCH_RSP phb_search_rsp;
extern T_MR_PHB_SET_ENTRY_RSP phb_set_entry_rsp;
extern T_MR_PHB_GET_ENTRY_RSP phb_get_entry_rsp;
extern T_MR_PHB_SET_OWNER_ENTRY_RSP phb_set_owner_entry_rsp;
extern T_MR_PHB_GET_OWNER_ENTRY_RSP phb_get_owner_entry_rsp;
extern T_MR_PHB_GET_COUNT_RSP phb_get_count_rsp;


void phb_trace(T_MR_PHB_ENTRY *entry);


/**
 * \brief 电话本查找
 */
int32 _mmi_dsm_phb_search_entry(T_MR_PHB_SEARCH_REQ *phb_search_req,T_MR_PHB_SEARCH_RSP *phb_search_rsq);


/**
 * \brief 写电话本
 */
int32 _mmi_dsm_phb_write_entry(T_MR_PHB_SET_ENTRY_REQ* phb_set_entry_req);


/**
 * \brief 读取电话本
 */
int32 _mmi_dsm_phb_read_entry(T_MR_PHB_GET_ENTRY_REQ* phb_get_entry_req,T_MR_PHB_GET_ENTRY_RSP *phb_get_entry_rsp);


/**
 * \brief 获取电话本条数
 */
int32 _mmi_dsm_phb_get_count(T_MR_PHB_GET_COUNT_REQ* phb_get_count_req,T_MR_PHB_GET_COUNT_RSP *phb_get_count_rsp);


/**
 * \brief 获取
 */
int32 mr_phonebook_get_contact_item(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 销毁电话本
 */
int32 mr_phonebook_destroy_contact_list(void);


/**
 * \brief 进入电话本
 */
void mmi_dsm_entry_phone_book(void);


/**
 * \brief 获取电话本状态
 */
int32 mr_phonebook_get_status(void);


int32 mr_phonebook_show_contact_list(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_phonebook_get_count(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_phonebook_get_entry(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_phonebook_set_entry(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_phonebook_search(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

#endif


/*#auto end#*/