
#ifdef __MMI_DSM_NEW__
#define __NEWSIMULATOR 

#include "MMI_features.h" 
#include "stdC.h"
#include "PixtelDataTypes.h"
#include "GlobalDefs.h"
#include "CommonScreens.h"
#include "SettingDefs.h"
#include "fmt_def.h"
#include "filemgr.h"
#include "SettingProfile.h"
#include "FileManagerDef.h"
#include "FileManagerGProt.h"
#include "filemgrcuigprot.h"
#include "FileMgrType.h"
#include "mrp_include.h"

#include "vapp_fmgr_cui_gprot.h"

#if(MTK_VERSION <= 0x0812)
#include "MMIDataType.h "
#endif

#undef __NEWSIMULATOR

static FMGR_FILTER filter;
static MR_T_FMGR_REQ mr_fmgr_req;
static BOOL dsm_is_processing = FALSE;
mr_fmgrAction dsmFmgrAction;


int32 mr_fmgr_filter_fmgr(void)
{
	FMGR_FILTER_INIT(&filter);
	return MR_SUCCESS;
}

int32 mr_fmgr_filter_clear(int32 fmgr_type)
{
	int32 mtk_fmgr_type = FMGR_TYPE_ALL;

	switch(fmgr_type)
	{
		case MR_FMGR_TYPE_ALL:    
			mtk_fmgr_type = FMGR_TYPE_ALL;
			break;
		case MR_FMGR_TYPE_FOLDER:
			mtk_fmgr_type = FMGR_TYPE_FOLDER;
			break;
		case MR_FMGR_TYPE_FOLDER_DOT:
			mtk_fmgr_type = FMGR_TYPE_FOLDER_DOT;
			break;
		case MR_FMGR_TYPE_UNKNOW:
			mtk_fmgr_type = FMGR_TYPE_UNKNOWN;
			break;
		case MR_FMGR_TYPE_BMP:
			mtk_fmgr_type = FMGR_TYPE_BMP;
			break;
		case MR_FMGR_TYPE_JPG:
			mtk_fmgr_type = FMGR_TYPE_JPG;
			break;
		case MR_FMGR_TYPE_JPEG:
			mtk_fmgr_type = FMGR_TYPE_JPEG;
			break;
		case MR_FMGR_TYPE_GIF:
			mtk_fmgr_type = FMGR_TYPE_GIF;
			break;
		case MR_FMGR_TYPE_PNG:
			mtk_fmgr_type = FMGR_TYPE_PNG;
			break;
		case MR_FMGR_TYPE_EMS :
			mtk_fmgr_type = FMGR_TYPE_EMS;
			break; 
		case MR_FMGR_TYPE_ANM :
			mtk_fmgr_type = FMGR_TYPE_ANM;
			break;
		case MR_FMGR_TYPE_WBMP :
			mtk_fmgr_type = FMGR_TYPE_WBMP;
			break;
		case MR_FMGR_TYPE_WBM :
			mtk_fmgr_type = FMGR_TYPE_WBM;
			break;
		case MR_FMGR_TYPE_IMY :
			mtk_fmgr_type = FMGR_TYPE_IMY;
			break;
		case MR_FMGR_TYPE_MID :
			mtk_fmgr_type = FMGR_TYPE_MID;
			break;
		case MR_FMGR_TYPE_MIDI :
			mtk_fmgr_type = FMGR_TYPE_MIDI;
			break;
		case MR_FMGR_TYPE_WAV :
			mtk_fmgr_type = FMGR_TYPE_WAV;
			break;
		case MR_FMGR_TYPE_AMR :
			mtk_fmgr_type = FMGR_TYPE_AMR;
			break;
		case MR_FMGR_TYPE_AAC :
			mtk_fmgr_type = FMGR_TYPE_AAC;
			break;
		case MR_FMGR_TYPE_DAF :
			mtk_fmgr_type = FMGR_TYPE_DAF;
			break;
		case MR_FMGR_TYPE_VM :
			mtk_fmgr_type = FMGR_TYPE_VM;
			break;
		case MR_FMGR_TYPE_AWB :
			mtk_fmgr_type = FMGR_TYPE_AWB;
			break;
		case MR_FMGR_TYPE_AIF :
			mtk_fmgr_type = FMGR_TYPE_AIF;
			break;
		case MR_FMGR_TYPE_AIFF :
			mtk_fmgr_type = FMGR_TYPE_AIFF;
			break;
		case MR_FMGR_TYPE_AIFC :
			mtk_fmgr_type = FMGR_TYPE_AIFC;
			break;
		case MR_FMGR_TYPE_AU :
			mtk_fmgr_type = FMGR_TYPE_AU;
			break;
		case MR_FMGR_TYPE_SND :
			mtk_fmgr_type = FMGR_TYPE_SND;
			break;
		case MR_FMGR_TYPE_M4A :
			mtk_fmgr_type = FMGR_TYPE_M4A;
			break;
		case MR_FMGR_TYPE_MMF  :
			mtk_fmgr_type = FMGR_TYPE_MMF;
			break;
		case MR_FMGR_TYPE_WMA :
			mtk_fmgr_type = FMGR_TYPE_WMA;
			break;
		case  MR_FMGR_TYPE_3GP :
			mtk_fmgr_type = FMGR_TYPE_3GP;
			break;
		case MR_FMGR_TYPE_MP4 :
			mtk_fmgr_type = FMGR_TYPE_MP4;
			break;
		case MR_FMGR_TYPE_AVI :	
			mtk_fmgr_type = FMGR_TYPE_AVI;
			break;
		case MR_FMGR_TYPE_JAD :
			mtk_fmgr_type = FMGR_TYPE_JAD;
			break;
		case MR_FMGR_TYPE_JAR :
			mtk_fmgr_type = FMGR_TYPE_JAR;
			break;
		case MR_FMGR_TYPE_VCF :
			mtk_fmgr_type = FMGR_TYPE_VCF;
			break;
		case MR_FMGR_TYPE_VCS :
			mtk_fmgr_type = FMGR_TYPE_VCS;
			break;
		case MR_FMGR_TYPE_THEME:
			mtk_fmgr_type = FMGR_TYPE_THEME;
			break;
		case MR_FMGR_TYPE_MRP:
			mtk_fmgr_type = FMGR_TYPE_MRP;
			break;
		case MR_FMGR_TYPE_NES:
			mtk_fmgr_type = FMGR_TYPE_NES;
			break;
		case MR_FMGR_TYPE_ZPK:
			mtk_fmgr_type = FMGR_TYPE_ZPK;
			break;
		case MR_FMGR_TYPE_ZIP:
			mtk_fmgr_type = FMGR_TYPE_ZIP;
			break;
	}
	FMGR_FILTER_CLEAR(&filter, mtk_fmgr_type);
	return MR_SUCCESS;
}


int32 mr_fmgr_filter_set(int32 fmgr_type)
{
	int32 mtk_fmgr_type = FMGR_TYPE_ALL;

	
	switch(fmgr_type)
	{
		case MR_FMGR_TYPE_ALL:    
			mtk_fmgr_type = FMGR_TYPE_ALL;
			break;
		case MR_FMGR_TYPE_FOLDER:
			mtk_fmgr_type = FMGR_TYPE_FOLDER;
			break;
		case MR_FMGR_TYPE_FOLDER_DOT:
			mtk_fmgr_type = FMGR_TYPE_FOLDER_DOT;
			break;
		case MR_FMGR_TYPE_UNKNOW:
			mtk_fmgr_type = FMGR_TYPE_UNKNOWN;
			break;
		case MR_FMGR_TYPE_BMP:
			mtk_fmgr_type = FMGR_TYPE_BMP;
			break;
		case MR_FMGR_TYPE_JPG:
			mtk_fmgr_type = FMGR_TYPE_JPG;
			break;
		case MR_FMGR_TYPE_JPEG:
			mtk_fmgr_type = FMGR_TYPE_JPEG;
			break;
		case MR_FMGR_TYPE_GIF:
			mtk_fmgr_type = FMGR_TYPE_GIF;
			break;
		case MR_FMGR_TYPE_PNG:
			mtk_fmgr_type = FMGR_TYPE_PNG;
			break;
		case MR_FMGR_TYPE_EMS :
			mtk_fmgr_type = FMGR_TYPE_EMS;
			break;
		case MR_FMGR_TYPE_ANM :
			mtk_fmgr_type = FMGR_TYPE_ANM;
			break;
		case MR_FMGR_TYPE_WBMP :
			mtk_fmgr_type = FMGR_TYPE_WBMP;
			break;
		case MR_FMGR_TYPE_WBM :
			mtk_fmgr_type = FMGR_TYPE_WBM;
			break;
		case MR_FMGR_TYPE_IMY :
			mtk_fmgr_type = FMGR_TYPE_IMY;
			break;
		case MR_FMGR_TYPE_MID :
			mtk_fmgr_type = FMGR_TYPE_MID;
			break;
		case MR_FMGR_TYPE_MIDI :
			mtk_fmgr_type = FMGR_TYPE_MIDI;
			break;
		case MR_FMGR_TYPE_WAV :
			mtk_fmgr_type = FMGR_TYPE_WAV;
			break;
		case MR_FMGR_TYPE_AMR :
			mtk_fmgr_type = FMGR_TYPE_AMR;
			break;
		case MR_FMGR_TYPE_AAC :
			mtk_fmgr_type = FMGR_TYPE_AAC;
			break;
		case MR_FMGR_TYPE_DAF :
			mtk_fmgr_type = FMGR_TYPE_DAF;
			break;
		case MR_FMGR_TYPE_VM :
			mtk_fmgr_type = FMGR_TYPE_VM;
			break;
		case MR_FMGR_TYPE_AWB :
			mtk_fmgr_type = FMGR_TYPE_AWB;
			break;
		case MR_FMGR_TYPE_AIF :
			mtk_fmgr_type = FMGR_TYPE_AIF;
			break;
		case MR_FMGR_TYPE_AIFF :
			mtk_fmgr_type = FMGR_TYPE_AIFF;
			break;
		case MR_FMGR_TYPE_AIFC :
			mtk_fmgr_type = FMGR_TYPE_AIFC;
			break;
		case MR_FMGR_TYPE_AU :
			mtk_fmgr_type = FMGR_TYPE_AU;
			break;
		case MR_FMGR_TYPE_SND :
			mtk_fmgr_type = FMGR_TYPE_SND;
			break;
		case MR_FMGR_TYPE_M4A :
			mtk_fmgr_type = FMGR_TYPE_M4A;
			break;
		case MR_FMGR_TYPE_MMF  :
			mtk_fmgr_type = FMGR_TYPE_MMF;
			break;
		case MR_FMGR_TYPE_WMA :
			mtk_fmgr_type = FMGR_TYPE_WMA;
			break;
		case MR_FMGR_TYPE_3GP :
			mtk_fmgr_type = FMGR_TYPE_3GP;
			break;
		case MR_FMGR_TYPE_MP4 :
			mtk_fmgr_type = FMGR_TYPE_MP4;
			break;
		case MR_FMGR_TYPE_AVI :	
			mtk_fmgr_type = FMGR_TYPE_AVI;
			break;
		case MR_FMGR_TYPE_JAD :
			mtk_fmgr_type = FMGR_TYPE_JAD;
			break;
		case MR_FMGR_TYPE_JAR :
			mtk_fmgr_type = FMGR_TYPE_JAR;
			break;
		case MR_FMGR_TYPE_VCF :
			mtk_fmgr_type = FMGR_TYPE_VCF;
			break;
		case MR_FMGR_TYPE_VCS :
			mtk_fmgr_type = FMGR_TYPE_VCS;
			break;
		case MR_FMGR_TYPE_THEME:
			mtk_fmgr_type = FMGR_TYPE_THEME;
			break;
		case MR_FMGR_TYPE_MRP:
			mtk_fmgr_type = FMGR_TYPE_MRP;
			break;
		case MR_FMGR_TYPE_NES:
			mtk_fmgr_type = FMGR_TYPE_NES;
			break;
		case MR_FMGR_TYPE_ZIP:	
			mtk_fmgr_type = FMGR_TYPE_ZIP;
			break;
		case MR_FMGR_TYPE_ZPK:
			mtk_fmgr_type = FMGR_TYPE_ZPK;
			break;
	}
	
	FMGR_FILTER_SET(&filter, mtk_fmgr_type);	
	return MR_SUCCESS;
}

void mr_set_fmgr_req(MR_T_FMGR_REQ *fmgr_req)
{
	memcpy(&mr_fmgr_req,fmgr_req,sizeof(MR_T_FMGR_REQ));	
}

MR_T_FMGR_REQ* mr_get_fmgr_req()
{
	return &mr_fmgr_req;
}

void mmi_dsm_exit_from_fmgr_emptry_folder(void)
{
#if 0
	DeleteUptoScrID(MENU_DSM_FMGR_APP);
	DeleteNHistory(1);
	GoBackHistory();
#endif	
}

static void mmi_dsm_fmgr_select_result_callback(void *filepath, int is_short)
{
#if 0
       S32	str_len;
	   
  	extern mr_fmgrAction dsmFmgrAction;


	str_len = mmi_ucs2strlen((PS8)filepath);
		
	if(str_len + 3 > FMGR_PATH_CHAR_COUNT)
	{
		memset(dsmFmgrAction.dir,0,sizeof(dsmFmgrAction.dir));
		dsmFmgrAction.fmgrAction = MR_DIALOG_KEY_CANCEL;
		DisplayPopup((PU8)GetString(FMGR_FS_PATH_OVER_LEN_ERROR_TEXT),
						 IMG_GLOBAL_WARNING, 
						 1, 
						 __MR_CFG_VAR_NOTIFY_DURATION__, 
						 WARNING_TONE);
		DeleteUptoScrID(MENU_DSM_FMGR_APP);
		return;	
	}
	
   	if(filepath == NULL)
   	{
		memset(dsmFmgrAction.dir,0,sizeof(dsmFmgrAction.dir));
		dsmFmgrAction.fmgrAction = MR_DIALOG_KEY_CANCEL;
		mr_app_send_event_wrapper(MR_LOCALUI_EVENT,MR_DIALOG_KEY_CANCEL,0);
		return;
   	}
	else if(str_len > 0)
	{
		char temppath[FMGR_MAX_PATH_LEN] = {0};
		
		mmi_ucs2ncpy((PS8)temppath, (PS8)filepath, str_len);

		if(temppath[0] == MMI_SYSTEM_DRV)
			temppath[0] = 'A';
		else if(temppath[0] == MMI_PUBLIC_DRV)
		{
			if(MMI_PUBLIC_DRV == MMI_CARD_DRV)
			{
				temppath[0] = 'C';
			}
			else
			{
				temppath[0] = 'B';
			}
		}
		else 
			temppath[0] = 'C';

		mr_str_ucs2_to_gb((char*)temppath,(char *)dsmFmgrAction.dir,0);
		mr_fs_separator_local_to_vm((U8 *)(dsmFmgrAction.dir));

		dsmFmgrAction.fmgrAction = MR_DIALOG_KEY_OK;
#ifdef MMI_ON_HARDWARE_P
		kal_prompt_trace(MOD_MMI,"Select File Name = %s",dsmFmgrAction.dir);
#endif		
		mr_app_send_event_wrapper(MR_LOCALUI_EVENT,MR_DIALOG_KEY_OK,0);
		
	}
#endif	
}

static mmi_id dsm_fmgr_cui_id = GRP_ID_INVALID;
static mmi_id mythroad_id = GRP_ID_INVALID;

static mmi_ret dsm_fmgr_proc(mmi_event_struct* evt)
{
	int32 ret;
	switch (evt->evt_id)
    {
        case EVT_ID_VCUI_FILE_SELECTOR_RESULT:
        {
			vcui_file_selector_result_event_struct* file_event = (vcui_file_selector_result_event_struct*)evt;
			srv_fmgr_fileinfo_struct info;
			WCHAR path[SRV_FMGR_PATH_MAX_LEN + 1];
			
			if (dsm_is_processing)
			{
				break; 
			}
			
			dsm_is_processing = TRUE;
			
			if (file_event->result > 0)
			{
				vcui_file_selector_get_selected_filepath(dsm_fmgr_cui_id, &info, path, (SRV_FMGR_PATH_MAX_LEN + 1) * 2);
				
				if (path[0] == SRV_FMGR_SYSTEM_DRV)
					path[0] = 'A';
				else if (path[0] == SRV_FMGR_PUBLIC_DRV)
					path[0] = 'B';
				else
					path[0] = 'C';

				mr_str_ucs2_to_gb((char*)path, (char*)dsmFmgrAction.dir, 0);
			
				mr_fs_separator_local_to_vm((U8 *)(dsmFmgrAction.dir));
				ret = MR_DIALOG_KEY_OK;
			}
			else
			{
				ret = MR_DIALOG_KEY_CANCEL;
			}
			
			//mmi_frm_group_close(GRP_ID_MYTHROAD);
			//dsm_fmgr_cui_id = GRP_ID_INVALID;
			vcui_file_selector_close(dsm_fmgr_cui_id);
			dsm_fmgr_cui_id = GRP_ID_INVALID;			
			vapp_mythroad_terminate(mythroad_id);			
			mythroad_id = GRP_ID_INVALID;
			mr_app_send_event_wrapper(MR_LOCALUI_EVENT, ret, 0);
            		return MMI_RET_OK;
        }
        case EVT_ID_VCUI_FOLDER_SELECTOR_RESULT:
        {
			vcui_folder_selector_result_event_struct* folder_event = (vcui_folder_selector_result_event_struct*)evt;
			srv_fmgr_fileinfo_struct info;
			WCHAR path[SRV_FMGR_PATH_MAX_LEN + 1]; 
			
			if (dsm_is_processing)
			{
				break; 
			}
			
			dsm_is_processing = TRUE;
			
			if (folder_event->result > 0)
			{
				vcui_folder_selector_get_selected_filepath(dsm_fmgr_cui_id, &info, path, (SRV_FMGR_PATH_MAX_LEN + 1) * 2);
				
				if (path[0] == SRV_FMGR_SYSTEM_DRV)
					path[0] = 'A';
				else if (path[0] == SRV_FMGR_PUBLIC_DRV)
					path[0] = 'B';
				else
					path[0] = 'C';
					
				mr_str_ucs2_to_gb((char*)path, (char*)dsmFmgrAction.dir, 0);
				mr_fs_separator_local_to_vm((U8 *)(dsmFmgrAction.dir));
				ret = MR_DIALOG_KEY_OK;
			}
			else
			{
				ret = MR_DIALOG_KEY_CANCEL;
			}
			
			//mmi_frm_group_close(GRP_ID_MYTHROAD);
			//dsm_fmgr_cui_id = GRP_ID_INVALID;
			vcui_file_selector_close(dsm_fmgr_cui_id);
			dsm_fmgr_cui_id = GRP_ID_INVALID;			
			vapp_mythroad_terminate(mythroad_id);
			mythroad_id = GRP_ID_INVALID;
			mr_app_send_event_wrapper(MR_LOCALUI_EVENT, ret, 0);
            		return MMI_RET_OK; 
        }
    }

	return MMI_RET_DONT_CARE; 
}

void mmi_dsm_entry_fmgr_screen(void)
{
#if 1
	UI_character_type storagePath[FMGR_MAX_PATH_LEN+1]={0};
	MR_T_FMGR_REQ* fmgr_req = NULL;
	U8 tempdrv;
	char switchbuf[DSM_MAX_FILE_LEN]={0};
	
	fmgr_req = mr_get_fmgr_req();
	
	if(fmgr_req->dft_list == MR_DSPL_NES)
	{
		sprintf((char *)switchbuf, "%c:\\%s\\",MR_DEFAULT_NES_DRV,MR_DEFAULT_NES_DIR);
		mr_str_gb_to_ucs2(switchbuf,(char *)storagePath);
	}
	else if (fmgr_req->dft_list == MR_DSPL_IMAGE)
	{
		mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
	}
	else if (fmgr_req->dft_list == MR_DSPL_AUDIO)
	{
		mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
	}
	else if(fmgr_req->dft_list == MR_DSPL_VIDEO)
	{
		mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
	}
	else if(fmgr_req->dft_list == MR_DSPL_NONE)
	{
		U8 drv = (U8)(fmgr_req->list_path[0]);

		if(strlen(fmgr_req->list_path) > 0)
		{
			if (drv == 'A'||drv == 'a')
			{
				tempdrv = MMI_SYSTEM_DRV;
			}
			else if(drv == 'B'||drv == 'b')
			{
				tempdrv = MMI_PUBLIC_DRV;
			}
			else if(drv == 'C'||drv == 'c')
			{
				tempdrv =MMI_CARD_DRV;
			}
			else
			{
				ASSERT(0);
				return;
			}
				
			sprintf((char *)switchbuf,"%c:\\%s\\",(S8)tempdrv,(fmgr_req->list_path+3));
			mr_fs_separator_vm_to_local((U8 *)switchbuf);
			mr_str_gb_to_ucs2(switchbuf,(char*)storagePath);
		}
		else
		{
			mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
		}
	}
	else if(fmgr_req->dft_list == MR_DSPL_FOLDER)
	{
		mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
	}

	internalCovered = MR_TRUE;
	dsm_is_processing = FALSE;

	mythroad_id = vapp_mythroad_launch(NULL, 0, dsm_fmgr_proc);
	dsm_fmgr_cui_id = vcui_file_selector_create(mythroad_id, storagePath, &filter, CUI_FILE_SELECTOR_STYLE_FILE_ONLY, CUI_FILE_SELECTOR_OPT_FIXED_PATH_ON);

	if (dsm_fmgr_cui_id == GRP_ID_INVALID) return;

	vcui_file_selector_run(dsm_fmgr_cui_id);
	
#elif 0
	internalCovered = MR_TRUE;

	mmi_frm_group_create(GRP_ID_ROOT, GRP_ID_MYTHROAD, dsm_fmgr_proc, NULL);
	mmi_frm_group_enter(GRP_ID_MYTHROAD, MMI_FRM_NODE_SMART_CLOSE_FLAG);

	dsm_fmgr_cui_id = cui_file_selector_create(GRP_ID_MYTHROAD, L"root", &filter, CUI_FILE_SELECTOR_STYLE_FILE_AND_PATH, 0);

	if (dsm_fmgr_cui_id == GRP_ID_INVALID) return;

	cui_file_selector_run(dsm_fmgr_cui_id);
#else
	UI_character_type storagePath[FMGR_MAX_PATH_LEN+1]={0};
	MR_T_FMGR_REQ* fmgr_req = NULL;
	U8 tempdrv;
	char switchbuf[DSM_MAX_FILE_LEN]={0};
	extern pBOOL IsBackHistory;


	if(IsBackHistory == MMI_TRUE)
	{
		if(dsmFmgrAction.fmgrAction == MR_DIALOG_KEY_CANCEL)
		{
			mr_app_send_event_wrapper(MR_LOCALUI_EVENT,MR_DIALOG_KEY_CANCEL,0);
		}
		else
		{
			mr_app_send_event_wrapper(MR_LOCALUI_EVENT,MR_DIALOG_KEY_OK,0);
		}
		
		return;
	}
	
	fmgr_req = mr_get_fmgr_req();

	EntryNewScreen(MENU_DSM_FMGR_APP, NULL, mmi_dsm_entry_fmgr_screen, NULL);
	
	if(fmgr_req->dft_list == MR_DSPL_NES)
	{
		sprintf((char *)switchbuf, "%c:\\%s\\",MR_DEFAULT_NES_DRV,MR_DEFAULT_NES_DIR);
		mr_str_gb_to_ucs2(switchbuf,(char *)storagePath);
	}
	else if (fmgr_req->dft_list == MR_DSPL_IMAGE)
	{
		mmi_imgview_get_storage_file_path((PS8)storagePath);
	}
	else if (fmgr_req->dft_list == MR_DSPL_AUDIO)
	{
		mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
	}
	else if(fmgr_req->dft_list == MR_DSPL_VIDEO)
	{
            #if defined(__MMI_VIDEO_PLAY_SUPPORT__) || (defined(__MMI_VIDEO_RECORDER__) && !defined(__MMI_CAMCORDER__))
		mmi_vdoply_get_storage_file_path((PS8)storagePath);
            #endif
	}
	else if(fmgr_req->dft_list == MR_DSPL_NONE)
	{
		U8 drv = (U8)(fmgr_req->list_path[0]);

		if(strlen(fmgr_req->list_path) > 0)
		{
			if (drv == 'A'||drv == 'a')
			{
				tempdrv = MMI_SYSTEM_DRV;
			}
			else if(drv == 'B'||drv == 'b')
			{
				tempdrv = MMI_PUBLIC_DRV;
			}
			else if(drv == 'C'||drv == 'c')
			{
				tempdrv =MMI_CARD_DRV;
			}
			else
			{
				ASSERT(0);
				return;
			}
				
			sprintf((char *)switchbuf,"%c:\\%s\\",(S8)tempdrv,(fmgr_req->list_path+3));
			mr_fs_separator_vm_to_local((U8 *)switchbuf);
			mr_str_gb_to_ucs2(switchbuf,(char*)storagePath);
		}
		else
		{
			mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
		}
	}
	else if(fmgr_req->dft_list == MR_DSPL_FOLDER)
	{
		mmi_ucs2cpy((PS8)storagePath, (PS8)L"root");
		mmi_fmgr_select_path_and_enter(APP_DSM, 
									FMGR_SELECT_FOLDER|FMGR_SELECT_REPEAT,  
									filter, 
									(PS8)storagePath,
									(PsExtFuncPtr)mmi_dsm_fmgr_select_result_callback);
		return;
	}

	mmi_fmgr_select_path_and_enter(APP_DSM, 
									FMGR_SELECT_FILE|FMGR_SELECT_REPEAT,  
									filter, 
									(PS8)storagePath,
									(PsExtFuncPtr)mmi_dsm_fmgr_select_result_callback);
	/* check if device is busy or not */
#endif
}

mmi_id mr_fmgr_get_cui_id(void)
{
	mmi_id id = dsm_fmgr_cui_id;
	return id;
}

int32 mr_fmgr_exit(void)
{
#if 0
	dsmFmgrAction.fmgrAction = MR_DIALOG_KEY_CANCEL;
	
	if(IsScreenPresent(MENU_DSM_FMGR_APP))
	{
		DeleteUptoScrID(MENU_DSM_FMGR_APP);
		DeleteNHistory(1);
		GoBackHistory();
	}
	else if(GetActiveScreenId() == MENU_DSM_FMGR_APP)
	{
		GoBackHistory();
	}
#endif	
	return MR_SUCCESS;
}

int32 dsmCheckBrowerExit(void)
{
#if 0
	if((GetActiveScreenId() == SCR_FMGR_MAIN) 
		||(GetActiveScreenId() == SCR_FMGR_EXPLORER)
		||IsScreenPresent(SCR_FMGR_MAIN)
		||IsScreenPresent(SCR_FMGR_EXPLORER))
	{
		return 1;
	}else{
		return 0;
	}
#endif	
}

int32 mr_fmgr_show(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	dsmFmgrAction.fmgrAction = MR_DIALOG_KEY_CANCEL;
	memset(dsmFmgrAction.dir,0,sizeof(dsmFmgrAction.dir));
	mr_set_fmgr_req((MR_T_FMGR_REQ*)input);
	mmi_dsm_entry_fmgr_screen();
	return MR_SUCCESS;
}


int32 mr_fmgr_get_selected_path(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	*output =(uint8 *)(dsmFmgrAction.dir);
	*output_len = strlen(dsmFmgrAction.dir);
	return MR_SUCCESS;
}

#endif

