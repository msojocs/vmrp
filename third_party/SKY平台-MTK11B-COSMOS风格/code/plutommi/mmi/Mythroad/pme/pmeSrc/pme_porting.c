#ifdef __PME_SUPPORT__

#include "MMI_Include.h"

#include "mrporting.h"
#include "pme_i.h"
#include "pme.h"
#include "pme_def.h"

#ifndef __IS_10A__
#include "FileManagerGprot.h"
#include "Mmi_frm_events_gprot.h"
#endif
#include "timerevents.h"
#include "med_utility.h"

/*for net*/
#include "MMI_features.h"
#include "MMIDataType.h"
#include "ProtocolEvents.h"
#include "mmi_msg_struct.h"
#include "Soc_api.h"
#include "custom_config.h"
#include "mmi_frm_queue_gprot.h"
#include "mmi_frm_mem_gprot.h"

#ifdef __IS_11A__
#include "nvram_common_defs.h"
#else
#include "QueueGprot.h"
#include "OslMemory.h"
#endif

#include "GlobalConstants.h"
#include "mmi_msg_struct.h"
#include "mmi_frm_events_gprot.h"

#if defined( __IS_10A__)||defined(__IS_11A__)
#include "dtcntsrvgprot.h"
#include "dtcntsrviprot.h"
#include "DtcntSrvIntStruct.h"
#include "FileMgrSrvGProt.h"
#else
#include "mmidsmdef.h"
#endif

#include "DataAccountEnum.h"
#include "DataAccountStruct.h"
#include "DataAccountProt.h"
#include "mrp_features.h"
#include "mrp_net.h"
#include "mmidsmapi.h"
#include "Mrp_apn.h"
#include "mrp_sysinfo.h"

#include "FileManagerResDef.h"

#include "gui.h"


#ifdef __MED_IN_ASM__
#include "app_mem_med.h"
#define med_alloc_ext_mem(size) applib_asm_alloc_anonymous_nc(size)
#define med_free_ext_mem(ptr) applib_asm_free_anonymous(*ptr)
#endif

#if defined( __IS_10A__)||defined(__IS_11A__)
#define MMI_SYSTEM_DRV SRV_FMGR_SYSTEM_DRV
#define MMI_PUBLIC_DRV SRV_FMGR_PUBLIC_DRV
#define MMI_CARD_DRV SRV_FMGR_CARD_DRV
#endif

#define PME_NETWORK_REQUESTID		0x0e0ac091
#if defined( __IS_10A__)||defined(__IS_11A__)
#define PME_NETWORK_ACNTNAME		L"PME GPRS"
#else
#define PME_NETWORK_ACNTNAME		"PME GPRS"
#endif

/*apn other context s*/
typedef struct
{
	char 				*userName;
	char 				*passWord;
	unsigned char 			dns[4];
	unsigned char 			authentication_type;
}pme_apn_account_st;

/*for GPRS active*/
static kal_uint32 pme_net_acct_id = 0xFFFFFFFF;
static kal_uint8 pme_net_app_id = 0;
//static kal_int8 pme_sim_act_id = CBM_SIM_ID_SIM1;
static int32 pme_nwk_account_id = 0;
/*the net notify cb*/
static void* pme_gethostbyname_cb = NULL;
//static void* pme_socketnotify_cb = NULL;
static void* pme_netinit_cb = NULL;
//static kal_int8 pme_net_apn_set = 0;

static uint16 s_pme_port_filefullnamebuf_ucs2[128];

/*****************************************************************************
 * SUPPORT FUNCTION
 *****************************************************************************/

static uint16* pme_get_filename_ucs2(char *filename) 
{
	mmi_chset_utf8_to_ucs2_string(s_pme_port_filefullnamebuf_ucs2, sizeof(s_pme_port_filefullnamebuf_ucs2), filename);
	return (uint16*)s_pme_port_filefullnamebuf_ucs2;
}

int32 pme_trace_mod(void)
{
	return MOD_MMI;
}

int32 pme_port_get_langId(void)
{
	int32 ret = mr_setting_get_cur_lang(0);

	if(ret >= 1000){
		ret -= 1000;
	}else{
		ret = 1;
	}

	return ret;
}

int32 pme_port_init_user(void)
{

	return MR_SUCCESS;
}

int32 pme_port_get_handInfo(mr_userinfo *info)
{
	int32 ret;
	static int8 first = 0;

	/*select the actived sim*/
	if(first == 0 && GetDsmState() == 0){//temp.
		first = 1;
		mr_sim_initialize();
	}

	ret = mr_getNetworkID();
	if(ret == MR_NET_ID_NONE){
		return MR_FAILED;
	}
	
	ret = mr_getUserInfo(info);
	if(ret != MR_SUCCESS){
		return MR_FAILED;
	}
	
	return MR_SUCCESS;
}

void pme_port_start_wap(char* url)
{
	mr_connectWAP(url);
}

void pme_port_start_call(char* number)
{
	mr_call(number);
}

void pme_port_start_sms(char* number, char* content, int32 type)
{
	mr_sendSms(number, content, type);
}

void pme_port_start_app(int32 appid, char* param, char* saveName)
{
	mr_pme_port_start_app(appid, param, saveName, FALSE);
}

void pme_port_dload_app(int32 appid, char* path, char* saveName)
{
	//*J,1000,6,0,3,3,5,0,0,rstone,814039,rstone.mrp
	mr_pme_port_start_app(appid, path, saveName, TRUE);
}

void pme_port_start_base_timer(int32 ms, void* func)
{
	//StartNonAlignTimer(PME_TIMER, ms, (FuncPtr)func);
	StartTimer(PME_TIMER, ms, (FuncPtr)func);
}

void pme_port_stop_base_timer(void)
{
	StopTimer(PME_TIMER);
}

void pme_port_mem_free(void **str)
{
	if(str && *str){
		med_free_ext_mem((void**)str);
		*str = NULL;
	}
}

char* pme_port_mem_malloc(int32 len)
{
	if(len > 0){
		return (char*)med_alloc_ext_mem(len);
	}
	return NULL;
}

void* pme_port_ctrl_malloc(int32 size)
{
	return (void*)OslMalloc(size);
}

void pme_port_ctrl_free(void** pptr)
{
	if(pptr){
		if(*pptr){
			OslMfree(*pptr);
			*pptr = NULL;
		}
	}
}

void pme_port_str_free(void **str)
{
	if(str){
		pme_port_mem_free((void**)str);
	}
}

char* pme_port_str_dup(uint8 *str, uint32 len)
{
	char *new_str = NULL;

	if(str && len){
		new_str = pme_port_mem_malloc(len + 1);
		if(new_str){
			memcpy (new_str, str, len);
			new_str[len] = 0x00;
		}
	}

	return new_str;
}

int32 pme_port_utf8_to_ucs2_string_ex(uint8 *dest, int32 dest_size, uint8 *src, int32 src_len)
{
    int pos = 0;
    int cnt;
	int posi = 0;

    dest_size -= (dest_size % 2);
    while (*src && pos < dest_size - 2 && posi < src_len){
        cnt = mmi_chset_utf8_to_ucs2(dest + pos, src);
        if (cnt == 0)   /* abnormal case */
        {
            break;
        }        
        else    /* normal case */
        {
        	posi += cnt;
            src += cnt;
            pos += 2;
        }
    }
	
    dest[pos] = 0;
    dest[pos + 1] = 0;
    return pos + 2;
}

int32 pme_port_utf8_to_ucs2_string(uint8 *dest, int32 dest_size, uint8 *src)
{
	return (int32)mmi_chset_utf8_to_ucs2_string((kal_uint8*)dest, (kal_int32)dest_size, (kal_uint8*)src);
}

int32 pme_port_ucs2_to_utf8_string(uint8 *dest, int32 dest_size, uint8 *src)
{
	return (int32)mmi_chset_ucs2_to_utf8_string((kal_uint8*)dest, (kal_int32)dest_size, (kal_uint8*)src);
}

int32 pme_port_wstrcpy(uint16 *dest, uint16 *src)
{
	kal_wstrcpy((WCHAR *)dest, (const WCHAR *)src);
}
//20110117 wsl start change ,修改为存文件方式.
WCHAR g_pme_ucs[16] = {0};
char g_pme_asc[16] = {0};
int32 pme_port_get_setting_info(char * buf)
{	
	FS_HANDLE fh  = 0;	
	UINT readnum = 0;
	
	if(buf == 0)
	{
		return 0;
	}
	
	memset(g_pme_ucs,0,sizeof(WCHAR)*16);
	memset(g_pme_asc,0,sizeof(char)*16);
	
	sprintf(g_pme_asc,"%c:\\pme.cfg",MMI_SYSTEM_DRV);
	mmi_asc_to_ucs2((S8*)g_pme_ucs,(S8*)g_pme_asc);
	fh = FS_Open((WCHAR *)g_pme_ucs, FS_READ_ONLY);
	if(fh >= 0)
	{
		if(FS_NO_ERROR == FS_Read(fh, (void *)buf, PME_NV_CONTENT_SIZE, &readnum))
		{
		 	FS_Close(fh);
		 	return readnum;
		}
		FS_Close(fh);		 
	}
	///不能直接返回0,所有参数都需要保存下.
	return pme_port_save_setting_info(buf);
	
}

int32 pme_port_save_setting_info(char * buf)
{	
	FS_HANDLE fh  = 0;	
	UINT writenum = 0;
	
	if(buf == 0)
	{
		return -1;
	}
	
	memset(g_pme_ucs,0,sizeof(WCHAR)*16);
	memset(g_pme_asc,0,sizeof(char)*16);
	
	sprintf(g_pme_asc,"%c:\\pme.cfg",MMI_SYSTEM_DRV);
	mmi_asc_to_ucs2((S8*)g_pme_ucs,(S8*)g_pme_asc);
	
	fh = FS_Open((WCHAR *)g_pme_ucs, FS_CREATE_ALWAYS|FS_READ_WRITE);
	if(fh >= 0)
	{
		if(FS_NO_ERROR == FS_Write( fh, buf, PME_NV_CONTENT_SIZE, &writenum))
		{
			FS_Close(fh);
			return writenum;
		}
		FS_Close(fh);		
	}	
	return 0;		
}

int32 pme_port_nvset_read(char* buf)
{
#if 1
	return pme_port_get_setting_info(buf);
#else
	S16 error = 0;
	return (int32)ReadRecord(NVRAM_EF_MMI_PME_LID, 1, (void*)buf, PME_NV_CONTENT_SIZE, &error);
#endif
}

int32 pme_port_nvset_save(char* buf)
{
#if 1
	return pme_port_save_setting_info(buf);
#else
	S16 error = 0;
	return (int32)WriteRecord(NVRAM_EF_MMI_PME_LID, 1, (void*)buf, PME_NV_CONTENT_SIZE, &error);
#endif
}
//end wsl change ,修改为存文件方式.

int32 pme_port_get_drv_num(void)
{
	return 2;
}

char pme_port_get_drv_name(char dummy)
{
	if(dummy == 'B'||dummy == 'b'){
		return MMI_PUBLIC_DRV;
	}else{
		return MMI_CARD_DRV;
	}
}

int32 pme_port_get_drv_status(char drv)
{
	if (FS_GetDevStatus(drv, FS_MOUNT_STATE_ENUM) != FS_NO_ERROR){
		return MR_FAILED;
	}

	return MR_SUCCESS;
}

int32 pme_port_storage_wr_forbid(void)
{	
#ifdef __USB_IN_NORMAL_MODE__

#ifdef __IS_10A__
	if(srv_usb_is_in_mass_storage_mode())
#else
	if (mmi_usb_is_in_mass_storage_mode())
#endif
	{
		/* in mass storage mode */
		pme_ui_trace((MOD_MMI,"pme_port_storage_wr_forbid(): usb is busy"));
		return TRUE;
	}
#endif /* __USB_IN_NORMAL_MODE__ */ 	

	pme_ui_trace((MOD_MMI,"pme_port_storage_wr_forbid(): OK"));
	return FALSE;
}

int32 pme_port_check_msdc_status(void)
{
#if defined( __IS_10A__)||defined(__IS_11A__)

#ifdef WIN32
	return MR_MSDC_OK;
#else
#ifdef __USB_IN_NORMAL_MODE__
#ifdef __IS_10A__
	if(!srv_usb_is_in_mass_storage_mode())
#else
	if(!mmi_usb_is_in_mass_storage_mode())
#endif
	{
		if(srv_fmgr_drv_is_accessible(MMI_CARD_DRV)){
			return MR_MSDC_OK;
		}else{
			return MR_MSDC_NOT_EXIST;
		}
	}else{
		return MR_MSDC_NOT_USEFULL;
	}
#else
	if(srv_fmgr_drv_is_accessible(MMI_CARD_DRV)){
		return MR_MSDC_OK;
	}else{
		return MR_MSDC_NOT_EXIST;
	}
#endif
#endif

#else
	//:TODO:
#ifdef __USB_IN_NORMAL_MODE__
#ifdef __IS_10A__
	if(srv_usb_is_in_mass_storage_mode())
#else
	if (mmi_usb_is_in_mass_storage_mode())
#endif
	{
		return MR_MSDC_NOT_USEFULL;
	}
#endif
//	if(fmgr_is_msdc_present() == FALSE){
//		return MR_MSDC_NOT_EXIST;
//	}
	
	return MR_MSDC_OK;
#endif
}

int32 pme_port_storage_is_ready(void)
{	
#ifdef __USB_IN_NORMAL_MODE__
#ifdef __IS_10A__
	if(srv_usb_is_in_mass_storage_mode())
#else
	if (mmi_usb_is_in_mass_storage_mode())
#endif
	{
		/* in mass storage mode */
		mmi_usb_app_unavailable_popup(0);	/* pass 0 will show default string */
		pme_ui_trace((MOD_MMI,"pme_all_ready(): usb is busy"));
		return FALSE;
	}
#endif /* __USB_IN_NORMAL_MODE__ */ 	
	
#if 0//ndef NAND_SUPPORT	
	/*判断是否有t card */
	if(fmgr_is_msdc_present() == FALSE){
		mmi_usb_app_unavailable_popup(FMGR_FS_MSDC_NOT_PRESENT_TEXT);
		return FALSE;
	}
#endif

	pme_ui_trace((MOD_MMI,"pme_all_ready(): OK"));
	return TRUE;
}

int32 pme_port_get_drv_space(uint8 drv, pme_disk_info_st *spaceInfo)
{
	U64 disk_free_space, disk_total_space;
	FS_DiskInfo		disk_info;
	S32				fs_ret;
	U16 path[4];
	char aPath[4] ={0};
	
	disk_free_space	= 0;
	disk_total_space	= 0;
	sprintf(aPath,"%c:\\", drv);
	mmi_asc_n_to_ucs2((S8 *)path, aPath, 3);
	
	fs_ret = FS_GetDiskInfo((PU16)path, &disk_info, FS_DI_BASIC_INFO|FS_DI_FREE_SPACE);
	if(fs_ret >= 0){
		disk_free_space  = (U64)disk_info.FreeClusters*disk_info.SectorsPerCluster*disk_info.BytesPerSector;
		disk_total_space  = (U64)disk_info.TotalClusters*disk_info.SectorsPerCluster*disk_info.BytesPerSector;
		
		if((disk_total_space/10) >(U64)1024*1024*1024)
		{
			spaceInfo->tUnit = 1024*1024*1024;
		}
		else if((disk_total_space/10) > 1024*1024)
		{
			spaceInfo->tUnit = 1024*1024;
		}
		else if((disk_total_space/10) > 1024)
		{
			spaceInfo->tUnit = 1024;
		}
		else 
		{
			spaceInfo->tUnit = 1;
		}
		
		spaceInfo->total = disk_total_space/spaceInfo->tUnit;

		if((disk_free_space/10) >(U64)1024*1024*1024)
		{
			spaceInfo->unit = 1024*1024*1024;
		}
		else if((disk_free_space/10) > 1024*1024)
		{
			spaceInfo->unit = 1024*1024;
		}
		else if((disk_free_space/10) > 1024)
		{
			spaceInfo->unit = 1024;
		}
		else 
		{
			spaceInfo->unit = 1;
		}
		
		spaceInfo->account = disk_free_space/spaceInfo->unit;
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}

int32 pme_port_storage_space_is_lower(uint8 drv, int32 kb)
{
	pme_disk_info_st spaceInfo = {0};
	int32 ret;
	
	ret = pme_port_get_drv_space(drv, &spaceInfo);
	if(ret != MR_SUCCESS || !spaceInfo.total || !spaceInfo.account 
		|| (spaceInfo.unit <= 1024 && spaceInfo.account*spaceInfo.unit < kb*1024)
		){
		return TRUE;
	}

	return FALSE;
}

int32 pme_port_file_getLen(char* filename)
{
	int filehandle = -1;
	UINT size;
	int32 ferrno;

	filehandle = FS_Open((WCHAR *)pme_get_filename_ucs2(filename), FS_READ_ONLY);
	pme_port_trace((MOD_MMI, "pme_port_file_getLen(), filehandle: %d", filehandle));
	if (filehandle < FS_NO_ERROR)
		return MR_FAILED;
	
	ferrno = FS_GetFileSize( filehandle, &size);
	FS_Close(filehandle);
	if (ferrno == FS_NO_ERROR){
		return size;
	}else{
		return MR_FAILED;
	}
}

MR_FILE_HANDLE pme_port_file_open(char* filename,  uint32 mode)
{
	uint32 new_mode = 0;	
	int32 ferrno;

	if (mode & MR_FILE_RDONLY ) 
		new_mode |= FS_READ_ONLY;
	if (mode & MR_FILE_WRONLY ) 
		new_mode |= FS_READ_WRITE;
	if (mode & MR_FILE_RDWR ) 
		new_mode |= FS_READ_WRITE;
	if (mode & MR_FILE_CREATE ) 
		new_mode |= FS_CREATE;
	if(mode & MR_FILE_COMMITTED)
		new_mode |= FS_COMMITTED;
	if(mode & MR_FILE_SHARD)
		new_mode  |= FS_OPEN_SHARED;

#ifndef __PME_NOHIDE_FOLDER__
	new_mode |= FS_ATTR_HIDDEN;
#endif

	ferrno = FS_Open((WCHAR *)pme_get_filename_ucs2(filename), new_mode);
	pme_port_trace((MOD_MMI, "FS_Open(), ferrno: %d", ferrno));
	if (ferrno < 0) 
		return 0;
	else
		return ferrno;
}

int32 pme_port_file_read(MR_FILE_HANDLE f,void *p,uint32 l)
{
	UINT readnum = 0;
	int32 ferrno;

	ferrno = FS_Read(f, p, l, &readnum);
	pme_port_trace((MOD_MMI, "FS_Read(), ferrno: %d", ferrno));
	if  (ferrno == FS_NO_ERROR)
		return readnum;
	else 
		return MR_FAILED;
}

int32 pme_port_file_write(MR_FILE_HANDLE f,void *p,uint32 l)
{
	UINT writenum = 0;
	int32 ferrno;

	ferrno = FS_Write( f, p, l, &writenum);
	pme_port_trace((MOD_MMI, "FS_Write(), ferrno: %d", ferrno));
	if  (ferrno == FS_NO_ERROR)
		return writenum;
	else 
		return MR_FAILED;
}

int32 pme_port_file_close(MR_FILE_HANDLE f)
{
	int32 ferrno;
	
	ferrno = FS_Close(f);
	pme_port_trace((MOD_MMI, "FS_Close(), ferrno: %d", ferrno));
	if (ferrno == FS_NO_ERROR)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 pme_port_file_info(char* filename)
{
	int32 ferrno;

	ferrno = FS_GetAttributes((WCHAR *) pme_get_filename_ucs2(filename));
	pme_port_trace((MOD_MMI, "FS_GetAttributes(), ferrno: %d", ferrno));
	if (ferrno < 0) 
		return MR_IS_INVALID;
	
	if (ferrno & FS_ATTR_DIR) 
		return MR_IS_DIR;
	else
		return MR_IS_FILE;	 	 
}

int32 pme_port_file_seek(MR_FILE_HANDLE f, int32 pos, int method)
{
	int32 ferrno;

	ferrno = FS_Seek( f, (int) pos, method);
	pme_port_trace((MOD_MMI, "FS_Seek(), ferrno: %d", ferrno));
	if (ferrno < 0)
		return MR_FAILED;
	else 
		return MR_SUCCESS;
}

int32 pme_port_file_remove(char* filename)
{
	int32 ferrno;

	ferrno = FS_Delete(( WCHAR *) pme_get_filename_ucs2(filename));  
	pme_port_trace((MOD_MMI, "FS_Delete(), ferrno: %d", ferrno));
	if (ferrno == FS_NO_ERROR) 
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 pme_port_file_rename(char* oldname, const char* newname)
{
	U16 fullpathname_2[128] = {0};
	int32 ferrno;

	mmi_chset_utf8_to_ucs2_string(fullpathname_2, sizeof(fullpathname_2), newname);
	ferrno = FS_Rename(( WCHAR *) pme_get_filename_ucs2(oldname), ( WCHAR *)fullpathname_2);
	pme_port_trace((MOD_MMI, "FS_Rename(), ferrno: %d", ferrno));
	if (ferrno == FS_NO_ERROR) 
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

int32 pme_port_file_mkDir(char* name)
{
	int32 ferrno;
	WCHAR *fullpathname = ( WCHAR *)pme_get_filename_ucs2(name);

	ferrno = FS_CreateDir(fullpathname) ;	 
	pme_port_trace((MOD_MMI, "FS_CreateDir(), ferrno: %d, %s", ferrno, name));
	if (ferrno == FS_NO_ERROR) {
#ifndef __PME_NOHIDE_FOLDER__
		FS_SetAttributes((WCHAR*)fullpathname, FS_GetAttributes((WCHAR*)fullpathname) | FS_ATTR_HIDDEN);
#endif	
		return MR_SUCCESS;
	}else
		return MR_FAILED;
}

int32 pme_port_file_rmDir(char* fullFileName)
{
	int32 ferrno;

	ferrno = FS_RemoveDir(( WCHAR *)pme_get_filename_ucs2(fullFileName)) ;
	pme_port_trace((MOD_MMI, "FS_RemoveDir(), ferrno: %d", ferrno));
	if (ferrno == FS_NO_ERROR) 
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


MR_FILE_HANDLE pme_port_file_findStart(char* path, char* buffer, uint32 len, int32* isDir)
{
	FS_DOSDirEntry file_info;
	U16 *findPath;
	int32 ferrno;
	U16 dsmFindBuffer[128] ={0};
	
#if 0//(MTK_VERSION < 0x0852)	
	S8 NameBuffer[4] = {0};
	findPath = pme_get_filename_ucs2(path);
	FS_SetCurrentDir((const WCHAR*)findPath);

	mmi_chset_utf8_to_ucs2_string(NameBuffer, sizeof(NameBuffer), "*");
	ferrno = FS_FindFirst((const WCHAR*)NameBuffer, 0, 0 ,&file_info, (WCHAR*)dsmFindBuffer, len);
#else
	findPath = pme_get_filename_ucs2(path);
	mmi_ucs2cat((S8 *)findPath, (S8 *)L"\\*"); 	
	ferrno = FS_FindFirst((const WCHAR*)findPath, 0, 0 ,&file_info, (WCHAR*)dsmFindBuffer, len);
#endif
	pme_port_trace((MOD_MMI, "FS_FindFirst(), ferrno: %d", ferrno));

	if(isDir){
		if(file_info.Attributes & FS_ATTR_DIR){
			*isDir = TRUE;
		}else{
			*isDir = FALSE;
		}
	}


	if( ferrno < 0 )
		return MR_FAILED;
	   
	mmi_chset_ucs2_to_utf8_string(buffer, len, dsmFindBuffer);
	return ferrno;
}

int32 pme_port_file_findGetNext(MR_FILE_HANDLE search_handle, char* buffer, uint32 len, int32* isDir)
{
	FS_DOSDirEntry file_info;
	U16 dsmFindBuffer[128] ={0};
	
	pme_port_trace((MOD_MMI, "pme_port_file_findGetNext(), search_handle: %d", search_handle));
	if ( FS_FindNext( search_handle, &file_info, (WCHAR*)dsmFindBuffer, len ) == FS_NO_ERROR ){
		mmi_chset_ucs2_to_utf8_string(buffer, len, dsmFindBuffer);
		if(isDir){
			if(file_info.Attributes & FS_ATTR_DIR){
				*isDir = TRUE;
			}else{
				*isDir = FALSE;
			}
		}

		return MR_SUCCESS;
	}else{
		return MR_FAILED;
	}
}

void pme_port_file_findStop(MR_FILE_HANDLE search_handle)
{
	pme_port_trace((MOD_MMI, "pme_port_file_findStop(), search_handle: %d", search_handle));
	FS_FindClose((FS_HANDLE)search_handle);
}

static int32 pme_net_get_apn_setting(void)
{
	/*read from file "apn"*/
	return 0;
}

static MMI_BOOL pme_net_gethostbyname_notify(void* inMsg)
{
	app_soc_get_host_by_name_ind_struct *dns_ind = (app_soc_get_host_by_name_ind_struct *)inMsg;
	t_pme_dsn_cb cb = (t_pme_dsn_cb)pme_gethostbyname_cb;
	uint32 ipaddr = 0;
	pme_net_trace((MOD_MMI,"pme_net_gethostbyname_notify(): %x, %x", (int32)inMsg, (int32)cb));

	if(cb && dns_ind && dns_ind->request_id == PME_NETWORK_REQUESTID){
		pme_gethostbyname_cb = NULL;
		if(dns_ind->result == KAL_TRUE){
			memcpy(&ipaddr, dns_ind->addr, 4);
			pme_net_trace((MOD_MMI,"ipaddr : %x, %x,%x,%x,%x, len:%d, ip: %x", ipaddr, dns_ind->addr[0], dns_ind->addr[1], dns_ind->addr[2], dns_ind->addr[3], dns_ind->addr_len, soc_ntohl(ipaddr)));
			cb(soc_ntohl(ipaddr)); 
		}else{
			cb(MR_FAILED);
		}
		
		mmi_frm_clear_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND, (PsIntFuncPtr)pme_net_gethostbyname_notify);
		return MMI_TRUE;
	}
	
	return MMI_FALSE;
}

static MMI_BOOL pme_net_socket_notify(void* inMsg)
{
#ifndef WIN32
	app_soc_notify_ind_struct *soc_notify = (app_soc_notify_ind_struct *) inMsg;
	int32 event;
	
	if(soc_notify){
		pme_net_trace((MOD_MMI, "pme_net_socket_notify(), event_type: %d", soc_notify->event_type));
		switch(soc_notify->event_type){
			case SOC_WRITE:
				event = PME_SOC_WRITE;
				break;
			case SOC_READ:
				event = PME_SOC_READ;
				break;
			case SOC_CLOSE:
				event = PME_SOC_CLOSE;
				break;
		}
		
		if(pme_socket_event(soc_notify->socket_id, event) == MR_SUCCESS){
			return MMI_TRUE;
		}
	}
#endif

	return MMI_FALSE;
}

#if defined( __IS_10A__)||defined(__IS_11A__)
static uint32 pme_find_acc_prof_id_by_apn(const char* apn, cbm_sim_id_enum sim)
{
	extern srv_dtcnt_store_info_context g_srv_dtcnt_store_ctx;
	int i;

	for (i = 0; i < SRV_DTCNT_PROF_MAX_ACCOUNT_NUM; ++i)
	{
		if (g_srv_dtcnt_store_ctx.acc_list[i].in_use &&
			g_srv_dtcnt_store_ctx.acc_list[i].bearer_type == SRV_DTCNT_BEARER_GPRS &&
			g_srv_dtcnt_store_ctx.acc_list[i].sim_info == (sim + 1) &&
			app_stricmp((char*)g_srv_dtcnt_store_ctx.acc_list[i].dest_name, (char*)apn) == 0)            
		{       
			return g_srv_dtcnt_store_ctx.acc_list[i].acc_id;
		}
	}

	return -1;		
}

static uint32 pme_find_acc_prof_id_by_name(const WCHAR* account_name)
{
	int i, j;
	srv_dtcnt_result_enum ret;
	srv_dtcnt_store_prof_qry_struct store_prof_qry;
	srv_dtcnt_prof_str_info_qry_struct prof_str_info_qry;
	U16 name[SRV_DTCNT_PROF_MAX_ACC_NAME_LEN + 1] = {0};

	for (j = 0; j < CBM_SIM_ID_TOTAL; ++j)
	{
		memset(&store_prof_qry, 0, sizeof(store_prof_qry));
		store_prof_qry.qry_info.filters = SRV_DTCNT_STORE_QRY_TYPE_SIM | SRV_DTCNT_STORE_QRY_TYPE_BEARER_TYPE;
		store_prof_qry.qry_info.sim_qry_info = j + 1;
		store_prof_qry.qry_info.bearer_qry_info = SRV_DTCNT_BEARER_GPRS;

		ret = srv_dtcnt_store_qry_ids(&store_prof_qry);
		if (ret == SRV_DTCNT_RESULT_SUCCESS)
		{
			for (i = 0; i < store_prof_qry.num_ids; ++i)
			{
				prof_str_info_qry.dest = (S8*)name;
				prof_str_info_qry.dest_len = sizeof(name);
				srv_dtcnt_get_account_name(store_prof_qry.ids[i], &prof_str_info_qry, SRV_DTCNT_ACCOUNT_PRIMARY);
				
				if (kal_wstrcmp(account_name, name) == 0)
				{
					return store_prof_qry.ids[i];
				}
			}
		}	
	}

	return -1;	
}

static int32 pme_add_acc_prof_id(cbm_sim_id_enum sim, const WCHAR* account_name, char *apn, pme_apn_account_st *accountInfo)
{
	int i;
	srv_dtcnt_result_enum ret;
	srv_dtcnt_store_prof_data_struct prof_info;
	srv_dtcnt_prof_gprs_struct prof_gprs;
	U32 acct_id;
	
	pme_net_trace((MOD_MMI, "pme_add_acc_prof_id()"));
	
	// 添加数据账户
	memset(&prof_gprs, 0, sizeof(prof_gprs));
	prof_gprs.APN = apn;
	prof_gprs.prof_common_header.sim_info = sim + 1;
	prof_gprs.prof_common_header.AccountName = (const U8*)account_name;

	if (accountInfo){
		prof_gprs.prof_common_header.Auth_info.AuthType = accountInfo->authentication_type;
		strcpy((char*)prof_gprs.prof_common_header.Auth_info.UserName, accountInfo->userName);
		strcpy ((char*)prof_gprs.prof_common_header.Auth_info.Passwd, accountInfo->passWord);
		for ( i = 0; i < 4; i++) {
			prof_gprs.prof_common_header.PrimaryAddr[i] = accountInfo->dns[i];
		}		
	}

	prof_info.prof_data = &prof_gprs;
	prof_info.prof_fields = SRV_DTCNT_PROF_FIELD_ALL;
	prof_info.prof_type = SRV_DTCNT_BEARER_GPRS;

	ret = srv_dtcnt_store_add_prof(&prof_info, &acct_id);
	pme_net_trace((MOD_MMI, "ret: %d, %d", ret, acct_id));
	if(ret == SRV_DTCNT_RESULT_SUCCESS){
		return acct_id;	
	}else{
		return -1;
	}
}

static void pme_mmi_dtcnt_update_disp_list(U32 acct_id, const U8 *acct_name, const U8 *addr, U8 bearer_type, U32 account_type, U8 conn_type, U8 sim_info, U8 read_only)
{
#if 0
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)

#elif (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)
	extern void mmi_dtcnt_update_disp_list(U32 acct_id, U8 bearer_type, mmi_dtcnt_acct_type_enum account_type, U8 conn_type, U8 sim_info, U8 read_only);

	mmi_dtcnt_update_disp_list(acct_id, bearer_type, account_type, conn_type, sim_info, read_only);
#else
	extern void mmi_dtcnt_update_disp_list(U32 acct_id, const U8 *acct_name, const U8 *addr, U8 bearer_type, mmi_dtcnt_acct_type_enum account_type, U8 conn_type, U8 sim_info, U8 read_only);

	mmi_dtcnt_update_disp_list(acct_id, acct_name, addr, bearer_type, account_type, conn_type, sim_info, read_only);
#endif
#endif
}

static BOOL pme_update_acc_prof_id(cbm_sim_id_enum sim, uint32 acc_prof_id, const WCHAR* account_name, char *apn, pme_apn_account_st *accountInfo)
{
	srv_dtcnt_result_enum ret;
	srv_dtcnt_store_prof_data_struct prof_info;
	srv_dtcnt_prof_gprs_struct prof_gprs = {0};
	int i;
	
	prof_gprs.APN = apn;
	prof_gprs.prof_common_header.sim_info = sim + 1;
	prof_gprs.prof_common_header.AccountName = (const U8*)account_name;

	if (accountInfo){
		prof_gprs.prof_common_header.Auth_info.AuthType = accountInfo->authentication_type;
		strcpy((char*)prof_gprs.prof_common_header.Auth_info.UserName, accountInfo->userName);
		strcpy ((char*)prof_gprs.prof_common_header.Auth_info.Passwd, accountInfo->passWord);
		for ( i = 0; i < 4; i++) {
			prof_gprs.prof_common_header.PrimaryAddr[i] = accountInfo->dns[i];
		}		
	}

	prof_gprs.prof_common_header.acct_type = SRV_DTCNT_PROF_TYPE_USER_CONF;	
	prof_gprs.prof_common_header.px_service = SRV_DTCNT_PROF_PX_SRV_HTTP;
	
	prof_info.prof_data = &prof_gprs;
	prof_info.prof_fields = SRV_DTCNT_PROF_FIELD_ALL;
	prof_info.prof_type = SRV_DTCNT_BEARER_GPRS;

	ret = srv_dtcnt_store_update_prof(acc_prof_id, &prof_info);
	if (ret == SRV_DTCNT_RESULT_SUCCESS){
		pme_mmi_dtcnt_update_disp_list(acc_prof_id, prof_gprs.prof_common_header.AccountName, prof_gprs.APN, DATA_ACCOUNT_BEARER_GPRS,
			prof_gprs.prof_common_header.acct_type, 1, prof_gprs.prof_common_header.sim_info, prof_gprs.prof_common_header.read_only);	

		return acc_prof_id;
	}else{
		return -1;
	}
}

static int32 pme_net_create_apn_account(void* cb, char *apn, pme_apn_account_st *accountInfo)
{
	// 1. 搜索系统中存在的数据账户
	// 2. 检查虚拟机平台账号是否已存在
	// 3. 如果存在则返回这个账号ID，如果不存在继续
	// 4. 添加一个虚拟机平台数据账号
	// 5. 如果成功 返回账号ID，如果失败则返回指定的账号ID
	uint32 acc_prof_id;
	mmi_sim_enum sim1;
	cbm_sim_id_enum sim = CBM_SIM_ID_SIM1;
	pme_net_trace((MOD_MMI, "pme_net_create_apn_account(), apn: %s, T: %d", apn, mr_getTime()));

	if(!apn ){
		return MR_FAILED;
	}
	
	/*select sim*/
	sim1 = mr_sim_active_id_dsm2mmi();
	for(sim = CBM_SIM_ID_TOTAL - 1; sim > CBM_SIM_ID_SIM1; sim--){
		if((1<<sim) == sim1){
			break;
		}
	}

	/*select account*/
	acc_prof_id = pme_find_acc_prof_id_by_apn(apn, sim);
	
	pme_net_trace((MOD_MMI, "1: %d", acc_prof_id));
	if (acc_prof_id == -1){
		acc_prof_id = pme_add_acc_prof_id(sim, PME_NETWORK_ACNTNAME, apn, accountInfo);
	}

	pme_net_trace((MOD_MMI, "2: %d", acc_prof_id));
	if (acc_prof_id == -1){
		pme_net_trace((MOD_MMI, "3: %d", acc_prof_id));
		if (acc_prof_id == -1){
			acc_prof_id = pme_find_acc_prof_id_by_name(PME_NETWORK_ACNTNAME);
		}

		if (acc_prof_id == -1){
			acc_prof_id = SRV_DTCNT_PROF_MAX_ACCOUNT_NUM - 1;
		}

		acc_prof_id = pme_update_acc_prof_id(sim, acc_prof_id, PME_NETWORK_ACNTNAME, apn, accountInfo);
	}	

	pme_net_trace((MOD_MMI, "acc_prof_id: %d, T: %d", acc_prof_id, mr_getTime()));
	if (acc_prof_id != -1){
		pme_net_acct_id = acc_prof_id;		
		pme_nwk_account_id = cbm_encode_data_account_id(pme_net_acct_id, sim, pme_net_app_id, 0);
		return MR_SUCCESS;
	}else{
		return MR_FAILED;
	}
	
}
#else

static void pme_gprs_status_detach_req(void)
{
#ifdef __MMI_GPRS_FEATURES__
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MYQUEUE msgStruct;
    mmi_nw_set_attach_req_struct *local_data;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    msgStruct.oslSrcId = MOD_MMI;
    msgStruct.oslDestId = MOD_L4C;
    msgStruct.oslMsgId = MSG_ID_MMI_NW_SET_ATTACH_REQ;
    local_data = (mmi_nw_set_attach_req_struct*) OslConstructDataPtr(sizeof(mmi_nw_set_attach_req_struct));
    local_data->is_poweroff = MMI_FALSE;
    local_data->opcode = RAC_DETACH;
    local_data->type = RAC_PS;
    msgStruct.oslDataPtr = (oslParaType*) local_data;
    msgStruct.oslPeerBuffPtr = NULL;
    OslMsgSendExtQueue(&msgStruct);
    return;
#endif /* __MMI_GPRS_FEATURES__ */ 
}

static MMI_BOOL pme_gprs_set_account_rsp(void* info)
{
	mmi_ps_set_gprs_data_account_rsp_struct *localPtr;

	if(pme_netinit_cb != NULL){
		MR_INIT_NETWORK_CB cb;
		
		cb = (MR_INIT_NETWORK_CB)pme_netinit_cb;
		pme_netinit_cb = NULL;
		localPtr = (mmi_ps_set_gprs_data_account_rsp_struct *)info;
		if (localPtr != NULL && localPtr->result == KAL_TRUE) {
		#ifdef __MMI_DUAL_SIM_MASTER__	
			if(mr_sim_get_active() == DSM_SLAVE_SIM)
			{
				pme_nwk_account_id = cbm_encode_data_account_id(pme_get_gprs_data_account(), 1, pme_net_app_id, 0);
			}
			else
		#endif		
			{
				pme_nwk_account_id = cbm_encode_data_account_id(pme_get_gprs_data_account(), 0, pme_net_app_id, 0);
			}
			cb(MR_SUCCESS);
		}else {
			pme_net_trace((MOD_MMI, "GPRS FAILED!!"));
			cb(MR_FAILED);
		}
		return MMI_TRUE;
	}
	
	return MMI_FALSE;
}

static int32 pme_net_create_apn_account(void* cb, char *apn, pme_apn_account_st *accountInfo)
{
	mmi_ps_set_gprs_data_account_req_struct *myMsgPtr;
	U8	ref_count;
	U16	msg_len;
	MYQUEUE Message;
	U8 i = 0;

	if(!apn ){
		return MR_FAILED;
	}
	
	myMsgPtr = (mmi_ps_set_gprs_data_account_req_struct*) OslConstructDataPtr(sizeof(mmi_ps_set_gprs_data_account_req_struct));
 	if(myMsgPtr == NULL){
		return MR_FAILED;
	}

	ref_count = myMsgPtr->ref_count;
	msg_len = myMsgPtr->msg_len;
	memset(myMsgPtr,'\0',sizeof(mmi_ps_set_gprs_data_account_req_struct));
	myMsgPtr->ref_count = ref_count;
	myMsgPtr->msg_len = msg_len;
	myMsgPtr->gprs_account.context_id = 10;//unet_ApnAL_GetCntxId(absAccountId);
	myMsgPtr->gprs_account.name_dcs = 0;
	myMsgPtr->gprs_account.name_length = strlen(PME_NETWORK_ACNTNAME);
	strcpy ((char*)myMsgPtr->gprs_account.name, PME_NETWORK_ACNTNAME);
	strcpy ((char*)myMsgPtr->gprs_account.apn, apn);
	myMsgPtr->gprs_account.apn_length = strlen(apn);

	if(accountInfo){
		myMsgPtr->gprs_account.authentication_type = accountInfo->authentication_type;
		strcpy ((char*)myMsgPtr->gprs_account.user_name, accountInfo->userName);
		strcpy ((char*)myMsgPtr->gprs_account.password, accountInfo->passWord);
		for ( i = 0; i < 4; i++) {
			myMsgPtr->gprs_account.dns[i] = accountInfo->dns[i];
		}
	}else{
		myMsgPtr->gprs_account.authentication_type = 0;
		strcpy ((char*)myMsgPtr->gprs_account.user_name, "");
		strcpy ((char*)myMsgPtr->gprs_account.password, "");
		memset(myMsgPtr->gprs_account.dns, 0, sizeof(myMsgPtr->gprs_account.dns));	
	}

	myMsgPtr->profile_type = DATA_ACCT_GPRS_PROF;
	DSM_SET_GPRS_ACCOUT_DEFAULT_VALUE

	Message.oslSrcId=MOD_MMI;
	Message.oslDestId=MOD_L4C;
	Message.oslMsgId = PRT_MMI_PS_SET_GPRS_DATA_ACCOUNT_REQ;
	Message.oslDataPtr = (oslParaType *)myMsgPtr;
	Message.oslPeerBuffPtr= NULL;
	mmi_frm_set_protocol_event_handler(PRT_MMI_PS_SET_GPRS_DATA_ACCOUNT_RSP, (PsIntFuncPtr)pme_gprs_set_account_rsp, MMI_FALSE);
	OslMsgSendExtQueue(&Message);
	
	pme_netinit_cb = cb;
	return MR_WAITING;
}
#endif

int32 pme_port_net_network_init(MR_INIT_NETWORK_CB cb)
{
	T_DSM_DATA_ACCOUNT dsm_account;
	pme_apn_account_st account;
	int32 apnset = pme_net_get_apn_setting();
	int32 i;

	pme_net_trace((MOD_MMI, "pme_port_net_network_init(), %d", pme_net_app_id));

#if defined(__IS_10A__)||defined(__IS_11A__)
	//if(pme_net_app_id == 0){//init just once.
		cbm_register_app_id(&pme_net_app_id);
		cbm_hold_bearer(pme_net_app_id);	
	//}
#else
	pme_net_app_id = cbm_register_app_id(STR_PME_MAINSCR, IMG_PME_STATUS_ICON_SYS);
	cbm_hold_bearer(MOD_MMI, pme_net_app_id);	
#endif

	pme_net_trace((MOD_MMI, "%d", pme_net_app_id));

	if(apnset == 1 && dsm_data_account_get(&dsm_account) >= 0){
		account.userName = (char*)dsm_account.user_name;
		account.passWord = (char*)dsm_account.password;
		account.authentication_type = dsm_account.authentication_type;
		memcpy(account.dns, dsm_account.dns, 4);
		
		return pme_net_create_apn_account((void*)cb, (char*)dsm_account.apn, &account);
	}else if(mr_getNetworkID() == MR_NET_ID_OTHER){
		int32 len;
		unsigned char *netId = NULL;
		T_DSM_APN *pApn = NULL;	

		mr_net_get_network_plmn(&netId, &len);
		
#if !defined(WIN32)
{
		int32 dst = GetDsmState();
		mr_app_set_state(1);
		pApn = mr_getAPN(netId);
		mr_app_set_state(dst);
}		
#endif

		if(pApn == NULL){
			return MR_FAILED;
		}else{
			account.authentication_type = pApn->authentication_type;
			memcpy(account.dns ,pApn->dns,4);
			account.passWord = pApn->passWord;
			account.userName = pApn->userName;
			
			return pme_net_create_apn_account((void*)cb, (char*)pApn->apn, &account);
		}
	}
	
	return pme_net_create_apn_account((void*)cb, "cmnet", NULL);
}

void pme_port_net_network_close(void)
{
	int32 ret;
	pme_net_trace((MOD_MMI,"pme_port_net_network_close(), %d, %d", pme_net_acct_id, pme_net_app_id));

	if (pme_gethostbyname_cb){// 终止域名解析
		soc_abort_dns_query(KAL_FALSE, 0, KAL_TRUE, PME_NETWORK_REQUESTID, KAL_FALSE, 0, KAL_FALSE, 0);
	}

	mmi_frm_clear_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND, (PsIntFuncPtr)pme_net_gethostbyname_notify);
	mmi_frm_clear_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND, (PsIntFuncPtr)pme_net_socket_notify);
#if defined( __IS_10A__)||defined(__IS_11A__)
	ret = cbm_release_bearer(pme_net_app_id);
	ret = cbm_deregister_app_id(pme_net_app_id);
	//ret = srv_dtcnt_store_delete_prof(pme_net_acct_id, MMI_FALSE, NULL, NULL);
	pme_net_acct_id = 0xFFFFFFFF;
#else
	ret = cbm_release_bearer(MOD_MMI, pme_net_app_id);
	pme_gprs_status_detach_req();//temp.
#endif
	pme_net_trace((MOD_MMI,"cbm_release_bearer(), ret: %d", ret));
	pme_net_trace((MOD_MMI,"srv_dtcnt_store_delete_prof(), ret: %d", ret));
}

int32 pme_port_net_gethostbyname(const char *name, MR_GET_HOST_CB cb)
{
	kal_uint32 ipAddr;
	kal_uint8 addr_len=0;
	kal_int8 ret;

	ret = soc_gethostbyname(KAL_FALSE,
					   MOD_MMI,
					   (kal_int32)PME_NETWORK_REQUESTID,
					   (const kal_char *)name,
					   (kal_uint8*)&ipAddr,
					   (kal_uint8*)&addr_len,
					   (kal_uint8)0,
					   pme_nwk_account_id);
	
	pme_net_trace((MOD_MMI,"ret, %d, %d, %d, %d", ret, pme_nwk_account_id, pme_net_acct_id, pme_net_app_id));
	if(ret == SOC_SUCCESS) { 
		return soc_ntohl(ipAddr);
	}else if (ret == SOC_WOULDBLOCK){		
		pme_gethostbyname_cb = (void*)cb;
		mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND, (PsIntFuncPtr)pme_net_gethostbyname_notify, MMI_TRUE);
		return MR_WAITING; 
	}	
	
	return MR_FAILED;
}

int32 pme_port_net_socket_create(int32 type, int32 protocol)
{
#ifndef WIN32
	int32 mtk_soc_type;
	int32 mtk_protocol_type;
	int32 socketid;
	kal_uint8 val;
	kal_int8 result; 

	mtk_protocol_type = 0;		
	mtk_soc_type = (type == MR_SOCK_STREAM) ? SOC_SOCK_STREAM : SOC_SOCK_DGRAM;
	
	socketid = soc_create(SOC_PF_INET, mtk_soc_type, mtk_protocol_type, MOD_MMI, pme_nwk_account_id); 
	if(socketid >= 0){
		val = KAL_TRUE;
		result = soc_setsockopt((kal_uint8)socketid, SOC_NBIO, &val, sizeof(val));

		val = SOC_READ|SOC_WRITE|SOC_CLOSE;
		soc_setsockopt((kal_uint8)socketid, SOC_ASYNC, &val, sizeof(val));
		mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND, (PsIntFuncPtr)pme_net_socket_notify, MMI_TRUE);

		//pme_net_socket_id = socketid;
		return socketid;
	}
#endif

	return MR_FAILED;
}

void pme_port_net_socket_close(int32 s)
{
	pme_net_trace((MOD_MMI, "pme_port_net_socket_close(), s: %d", s));
	soc_close(s);
	//pme_net_socket_id = -1;
}

int32 pme_port_net_send_to(int32 s, const char *buf, int len, int32 ip, uint16 port)
{
#ifndef WIN32
	int32  ret_val;
	sockaddr_struct addr;
	kal_uint32 nIp;

	if(!buf || !len){
		return 0;
	}

	nIp = soc_htonl((unsigned long)ip);
	memcpy(addr.addr, (char*)&nIp, 4);
	addr.addr_len = 4;
	addr.port = (kal_uint16)port;

	ret_val = soc_sendto(s, (kal_uint8 *)buf, (kal_int32)len, 0, &addr);
	pme_net_trace((MOD_MMI, "soc_sendto(), s: %d, ret: %d, ip: %d, %d, %d, %d", s, ret_val, addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3]));
	if (ret_val >= 0){
		return ret_val;
	}else if (ret_val == SOC_WOULDBLOCK){
		return 0;
	}
#endif	

	return MR_FAILED;
}

int32 pme_port_net_recv_from(int32 s, char *buf, int len, int32 *ip, uint16 *port)
{
#ifndef WIN32
	int32  ret_val;
	sockaddr_struct addr;
	int32 temp;
	
	if(!buf || !len || !ip || !port){
		return MR_FAILED;
	}
	
	ret_val = soc_recvfrom(s, (kal_uint8 *)buf, (kal_int32)len, 0, &addr);
	*port = addr.port;
	memcpy(&temp, addr.addr, 4);
	*ip = soc_ntohl(temp);
	pme_net_trace((MOD_MMI, "soc_recvfrom(), s: %d, ret: %d, port: %d, ip: 0x%x, %d.%d.%d.%d", s, ret_val, addr.port, *ip, addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3]));
			
	if (ret_val >= 0){
		return ret_val;
	}else if (ret_val == SOC_WOULDBLOCK){
		return 0;
	}
#endif

	return MR_FAILED;
}

int pme_check_config_file(char *name)
{	
	int fh = FS_Open((WCHAR*)name,FS_READ_ONLY);

	pme_net_trace((MOD_MMI, "pme_check_config_file fh=%d",fh));

	if(fh > FS_NO_ERROR)
	{
		FS_Close(fh);
		pme_net_trace((MOD_MMI, "return 1"));
		return 1;
	}
	else
	{
		pme_net_trace((MOD_MMI, "return 0"));
		return 0;
	}
}

#if 0
static char pme_net_gethostbyname_ipaddr[4] = 0;
kal_int8 pme_net_socket_id = -1;
static soc_event_enum	pme_net_socket_event_type;

/*****************************************************************************
 * net task coding
 *****************************************************************************/

static void pme_net_task_send_msg(module_type dst_id, kal_uint16 msg_id, void *local_param_ptr)
{
    ilm_struct *ilm_ptr = allocate_ilm(MOD_PME);
    ilm_ptr->src_mod_id = MOD_PME;
    ilm_ptr->dest_mod_id = dst_id;
    //ilm_ptr->sap_id = 0;
    ilm_ptr->msg_id = (msg_type) msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;

    msg_send_ext_queue(ilm_ptr);
}

static void pme_net_gethostbyname_notify(void* inMsg)
{
	app_soc_get_host_by_name_ind_struct *dns_ind = (app_soc_get_host_by_name_ind_struct *)inMsg;

	if(!dns_ind || dns_ind->request_id != PME_NETWORK_REQUESTID){
		return;
	}
	
	if(dns_ind->result == KAL_TRUE){
		memcpy(pme_net_gethostbyname_ipaddr, dns_ind->addr, 4);
	}else{
		memset(pme_net_gethostbyname_ipaddr, 0, 4);
	}
	
	pme_net_task_send_msg(MOD_MMI, MSG_ID_DSM_SOC_HOSTIND, NULL);
}

static void pme_net_socket_notify(void* inMsg)
{
	app_soc_notify_ind_struct *soc_notify = (app_soc_notify_ind_struct *) inMsg;
	
	if(!soc_notify || soc_notify->socket_id != pme_net_socket_id){
		return;
	}

	pme_net_socket_event_type = soc_notify->event_type;
	pme_net_task_send_msg(MOD_MMI, MSG_ID_DSM_SOC_NOTIIND, NULL);
}

static void pme_net_task_main(task_entry_struct * task_entry_ptr)
{
	ilm_struct current_ilm;
    kal_uint32 my_index;

    kal_get_my_task_index(&my_index);
	while ( 1 )
	{
		receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);
        stack_set_active_module_id(my_index, MOD_PME);
        //stack_set_active_module_id(my_index, current_ilm.dest_mod_id);
        
		switch (current_ilm.msg_id) 
		{
			case MSG_ID_APP_SOC_NOTIFY_IND: 
				pme_net_socket_notify(current_ilm.local_para_ptr);				
				break;
			case MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND:
				pme_net_gethostbyname_notify(current_ilm.local_para_ptr);
				break;
			default:
			     break;
		}
		
		free_ilm(&current_ilm);
	}
}


kal_bool pme_net_task_create(comptask_handler_struct **handle)
{
    static const comptask_handler_struct custom1_handler_info = 
    {
        pme_net_task_main,   /* task entry function */
        NULL, 				 /* task initialization function */
        NULL,                   /* task configuration function */
        NULL,                   /* task reset handler */
        NULL,                   /* task termination handler */
    };

    *handle = (comptask_handler_struct*)&custom1_handler_info;
    return KAL_TRUE;
}
#endif

#endif

