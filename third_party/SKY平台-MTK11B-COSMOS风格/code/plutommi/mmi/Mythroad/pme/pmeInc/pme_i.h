#ifndef __PME_I_H__
#define __PME_I_H__


#if 1
#define pme_net_trace(a)		kal_prompt_trace a
#define pme_port_trace(a)		kal_prompt_trace a
#define pme_ui_trace(a)		kal_prompt_trace a
#else
#define pme_net_trace(a)
#define pme_port_trace(a)
#define pme_ui_trace(a)
#endif


#define PME_STORAGE_FREE_SPACE_MIN		32//存储最小空间需求(K)

/*internal error code*/
//#define PME_ERR_NO_MEM			-3
//#define PME_ERR_NO_SPACE		-4
//#define PME_ERR_FOP_FAILED		-5
#define PME_ERR_FATAL			-5
#define PME_ERR_NO_FILE		-6

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#undef MAX
#define MAX(a, b)			((a) > (b)) ? (a) : (b)
#undef MIN
#define MIN(a, b)			((a) < (b)) ? (a) : (b)
#undef ABS
#define ABS(X)   ((X) > 0 ? (X) : -(X))
#undef TRUE
#define TRUE 				(1)
#undef FALSE
#define FALSE 			(0)

typedef int32 (*t_pme_dsn_cb)(int32 result);

/*push mode setting*/
typedef enum
{
	PME_NORMAL_MODE,
	PME_EMERGENCY_MODE,
	PME_SLEEP_MODE,
	PME_CLOSE,
	PME_MAX_MODE
}pme_runmode_en;

/*GPRS mode setting*/
typedef enum
{
	PME_GPRSOPEN_USED,//使用时开启
	PME_GPRSOPEN_ALWAYS,//保持开启
	PME_GPRSOPEN_MAX
}pme_gprsmode_en;

/*socket状态通知*/
typedef enum
{
    PME_SOC_CLOSE,  /* Notify for close */
	PME_SOC_READ,  /* Notify for read */
    PME_SOC_WRITE,  /* Notify for write */
}pme_socket_event_en;

/*UI depand*/
//options
typedef enum
{
	DAPLO_SELECT,//-选择
	DAPLO_DELETE,//-[删除]
	//DAPLO_INFO,//-[详情]
	DAPLO_HELP,//-HELP
	DAPLO_EXIT,//-退出
	DAPLO_MAX
}pme_list_option_en;

typedef enum
{
	DAPLN_MSG,//
	DAPLN_NAV,//-navi
	DAPLN_MAX
}pme_list_node_en;

typedef enum
{
	DAPLS_NAV_PREV,
	DAPLS_NAV_NEXT,
	DAPLS_MAX
}pme_list_str_en;

typedef enum
{
	PME_OTYPE_NONE,
	PME_OTYPE_WAP,
	PME_OTYPE_CALL,
	PME_OTYPE_SMS,
	PME_OTYPE_APPENTRY,
	PME_OTYPE_APPLOAD,
	PME_OTYPE_MAX
}pme_msg_otype_en;

typedef enum
{
	PME_STYPE_SYS,
	PME_STYPE_EMAIL,
	PME_STYPE_IM,
	PME_STYPE_MAX
}pme_msg_stype_en;

typedef enum
{
	PME_MSG_OPTION_REPLY,
	PME_MSG_OPTION_IM,
	PME_MSG_OPTION_EMAIL,
	PME_MSG_OPTION_OTHERS,
	PME_MSG_OPTION_WAP,
	PME_MSG_OPTION_CALL,
	PME_MSG_OPTION_SMS,
	PME_MSG_OPTION_APPENTRY,
	PME_MSG_OPTION_APPLOAD,
	PME_MSG_OPTION_MAX
}pme_msg_option_en;


typedef struct
{
	uint32 total;
	uint32 tUnit;
	uint32 account;
	uint32 unit;
}pme_disk_info_st;


/*************************************************************************
	****************************** 模块内函数*********************************
*************************************************************************/

int32 pme_socket_event(int32 s, int32 event);
int32 pme_list_init(int32 mainPageSize, int32 subPageSize);
void pme_list_exit(void);
int32 pme_list_msg_get_context(uint16** content, uint16** title);
int32 pme_list_msg_send_context(char* content, int32 len);
int32 pme_list_msg_get_type(int8* stype, int8* otype, int8* canReply);
int32 pme_list_msg_option(pme_msg_option_en option);
void pme_list_sub_exit(void);
int32 pme_list_sub_is_first_page(void);
void pme_list_sub_option(pme_list_node_en opType, pme_list_option_en op);
int32 pme_list_sub_get_item_type(int32 index);
void pme_list_sub_set_focut_msg(int32 hili);
void pme_list_sub_screen_back(void);
int32 pme_list_sub_get_items(uint8 **titleTab, int32 titleTabLen, uint8 **hintTab, uint16 *imgTab);
int32 pme_list_sub_init(void);
void pme_list_main_delete_account(void);
void pme_list_main_exit(void);
int32 pme_list_main_get_items(uint8 **titleTab, int32 titleTabLen, uint8 **hintTab, uint16 *imgTab);
void pme_list_main_set_focut_account(int32 hili);
int32 pme_list_main_init(void);
int32 pme_list_init(int32 mainPageSize, int32 subPageSize);
void pme_list_exit(void);
int32 pme_socket_event(int32 s, int32 event);
void pme_idle_notify(int flag);
int32 pme_set_mode(int32 mode);
int32 pme_get_mode(void);
int32 pme_set_ring(int32 mode);
int32 pme_get_ring(void);
int32 pme_set_storage(int32 mode);
int32 pme_get_storage(void);
int32 pme_get_hid_hcd(int32* hid, int32* hcd, int32* runMode);//MR_IGNORE - 当前push功能设置为关闭；

/*************************************************************************
	****************************** 模块外函数*********************************
*************************************************************************/

int32 pme_trace_mod(void);
int32 pme_port_get_langId(void);
int32 pme_port_get_handInfo(mr_userinfo *info);
void pme_port_start_base_timer(int32 ms, void* func);
void pme_port_stop_base_timer(void);
void pme_port_mem_free(void **str);
char* pme_port_mem_malloc(int32 len);
void pme_port_str_free(void **str);
char* pme_port_str_dup(uint8 *str, uint32 len);
int32 pme_port_utf8_to_ucs2_string_ex(uint8 *dest, int32 dest_size, uint8 *src, int32 src_len);
int32 pme_port_utf8_to_ucs2_string(uint8 *dest, int32 dest_size, uint8 *src);
int32 pme_port_ucs2_to_utf8_string(uint8 *dest, int32 dest_size, uint8 *src);
int32 pme_port_wstrcpy(uint16 *dest, uint16 *src);
void pme_port_start_wap(char* url);
void pme_port_start_call(char* number);
void pme_port_start_sms(char* number, char* content, int32 type);
void pme_port_start_app(int32 appid, char* param, char* saveName);
void pme_port_dload_app(int32 appid, char* path, char* saveName);
int32 pme_port_nvset_read(char* buf);
int32 pme_port_nvset_save(char* buf);
int32 pme_port_get_drv_num(void);
char pme_port_get_drv_name(char dummy);
int32 pme_port_get_drv_status(char drv);
int32 pme_port_storage_wr_forbid(void);
int32 pme_port_check_msdc_status(void);
int32 pme_port_storage_is_ready(void);
int32 pme_port_get_drv_space(uint8 drv, pme_disk_info_st *spaceInfo);
int32 pme_port_storage_space_is_lower(uint8 drv, int32 kb);
int32 pme_port_file_getLen(char* filename);
MR_FILE_HANDLE pme_port_file_open(char* filename,  uint32 mode);
int32 pme_port_file_read(MR_FILE_HANDLE f,void *p,uint32 l);
int32 pme_port_file_write(MR_FILE_HANDLE f,void *p,uint32 l);
int32 pme_port_file_close(MR_FILE_HANDLE f);
int32 pme_port_file_info(char* filename);
int32 pme_port_file_seek(MR_FILE_HANDLE f, int32 pos, int method);
int32 pme_port_file_remove(char* filename);
int32 pme_port_file_rename(char* oldname, const char* newname);
int32 pme_port_file_mkDir(char* name);
int32 pme_port_file_rmDir(char* fullFileName);
MR_FILE_HANDLE pme_port_file_findStart(char* path, char* buffer, uint32 len, int32* isDir);
int32 pme_port_file_findGetNext(MR_FILE_HANDLE search_handle, char* buffer, uint32 len, int32* isDir);
void pme_port_file_findStop(MR_FILE_HANDLE search_handle);

int32 pme_port_net_network_init(MR_INIT_NETWORK_CB cb);
void pme_port_net_network_close(void);
int32 pme_port_net_gethostbyname(const char *name, MR_GET_HOST_CB cb);
int32 pme_port_net_socket_create(int32 type, int32 protocol);
void pme_port_net_socket_close(int32 s);
int32 pme_port_net_send_to(int32 s, const char *buf, int len, int32 ip, uint16 port);
int32 pme_port_net_recv_from(int32 s, char *buf, int len, int32 *ip, uint16 *port);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* pme_list_get_cust_str(pme_list_str_en id);
void pme_main_screen_refresh(void);
void pme_main_screen_distroy(void);
uint16 pme_main_screen_get_list_icon(int32 haveNew);
void pme_list_screen_refresh(void);
void pme_list_screen_distroy(void);
uint16 pme_list_screen_get_list_icon(int32 isNew);
void pme_msg_textscreen_shown(void);
void pme_msg_replyscreen_entry(void);
void pme_ui_set_status_icon(int8 type, int32 open);
int32 pme_ui_is_list_screen_open(void);
void pme_ui_new_msg_notify(int8 type, char* sender);
void pme_ui_update_list_screen(void);
void pme_ui_intialize(void);
void pme_main_entry(void);

/////////////////////////////////////////////////////////////////////////

extern void mr_pme_port_start_app(int32 appid, char* param, char* saveName, int32 isDownload);


#endif
