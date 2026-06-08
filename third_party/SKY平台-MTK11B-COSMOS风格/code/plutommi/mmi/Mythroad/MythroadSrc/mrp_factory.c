#ifdef __MMI_DSM_NEW__    
#include "AppMgrSrvGProt.h"
#include "AppMgrSrvProt.h"
#include "mmi_rp_all_defs.h"
#include "vapp_mainmenu_gprot.h"
#include "Conversions.h"
#include "mrp_include.h"

extern U32 srv_mrpfactory_get_app_package_num(srv_appmgr_flag_type flag);
extern mmi_ret srv_mrpfactory_get_app_package_list(srv_appmgr_flag_type flag, mmi_app_package_name_struct *name_array, U32 num);
extern mmi_ret srv_mrpfactory_get_app_package_name(const mmi_app_package_char *app_name, WCHAR *string);
extern mmi_ret srv_mrpfactory_get_app_package_image(const mmi_app_package_char *app_name, mmi_image_src_struct *image);
extern mmi_ret srv_mrpfactory_get_app_package_info(const mmi_app_package_char *app_name, srv_app_info_struct *info);
extern MMI_ID srv_mrpfactory_launch_ex(srv_appfactory_launch_type_enum type, const mmi_app_package_char *app_name, void* param, U32 param_size);
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1224)
extern mmi_ret srv_mrpfactory_get_unique_app_id_list(srv_appmgr_flag_type flag, U32 *id, U32 num);
extern mmi_ret srv_mrpfactory_get_package_name_by_unique_id(U32 id, mmi_app_package_char *app_name);
extern U32 srv_mrpfactory_get_unique_app_id(const mmi_app_package_char *app_name);
#endif

 srv_appfactory_func_table_struct mrp_appfactory_func_table = {
    SRV_APP_FACTORY_MRP_TYPE,
    srv_mrpfactory_get_app_package_num,
    srv_mrpfactory_get_app_package_list,
    srv_mrpfactory_get_app_package_name,
    srv_mrpfactory_get_app_package_image,
    srv_mrpfactory_get_app_package_info,
    srv_mrpfactory_launch_ex
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1224)
    ,NULL,
    srv_mrpfactory_get_unique_app_id_list,
    srv_mrpfactory_get_package_name_by_unique_id,
    srv_mrpfactory_get_unique_app_id
#endif    
  };


/* Shortcut Info struct (show in main menu) */
typedef struct
{
    char* package_name;
    U32 app_id;
    U16 app_str_id;    /* app name resource id */
    U16 app_img_id;    /* app icon resource id */
} srv_mrp_appmgr_shortcut_info_struct;

static srv_mrp_appmgr_shortcut_info_struct g_mrp_shortcut_info_list[] = 
{
#ifdef __MR_CFG_ENTRY_GAMECENTER__  	
	{"mrp.cookie", MR_APPID_COOKIE, STR_DSM_GAME_CENTER, IMG_DSM_LIST_GAME},
#endif	

#ifdef __MR_CFG_ENTRY_MRPSTORE__  	
	{"mrp.mrpstore", MR_APPID_MRPSTORE, STR_DSM_MRPSTORE, IMG_DSM_LIST_GAME},
#endif	

#ifdef __MR_CFG_ENTRY_SKYBUDDY__
	{"mrp.buddy", MR_APPID_SKYBUDDY, STR_DSM_SKYBUDDY, IMG_DSM_LIST_SKYBUDDY},
#endif	

#ifdef __MR_CFG_ENTRY_NETGAME__
	{"mrp.netgames", MR_APPID_NET_GAMES, STR_DSM_NETGAME, IMG_DSM_LIST_MPZONE},
#endif	

#ifdef __MR_CFG_ENTRY_SHOUXIN__
	{"mrp.shouxin", MR_APPID_SHOUXIN, STR_DSM_SHOUXIN, IMG_DSM_LIST_SHOUXIN},
#endif	

#ifdef __MR_CFG_ENTRY_EBOOK__
	{"mrp.skyread", MR_APPID_SKYREAD, STR_DSM_BOOK, IMG_DSM_LIST_BOOK},
#endif	

#ifdef __MR_CFG_ENTRY_QQ__
	#if defined(__MMI_NGUAQ__) 
	{"mrp.qq", MR_APPID_NGUAQ, STR_DSM_QQ, IMG_DSM_LIST_QQ},
	#elif defined(__MMI_SKYQQ__) 
	{"mrp.qq", MR_APPID_SKYQQ, STR_DSM_QQ, IMG_DSM_LIST_QQ},
	#else
	{"mrp.qq", MR_APPID_QQ2008, STR_DSM_QQ, IMG_DSM_LIST_QQ},
	#endif
#endif

#ifdef __MR_CFG_ENTRY_MSN__
	{"mrp.msn", MR_APPID_MSN, STR_DSM_MSN, IMG_DSM_LIST_MSN},
#endif

#ifdef __MR_CFG_ENTRY_FETION__
	{"mrp.otafx3", MR_APPID_FETION, STR_DSM_FETION, IMG_DSM_LIST_FETION},
#endif

#ifdef __MR_CFG_ENTRY_KAIXIN__
	{"mrp.kaixin", MR_APPID_KAIXIN, STR_DSM_KAIXIN, IMG_DSM_LIST_KAIXIN},
#endif

#ifdef __MR_CFG_ENTRY_TIANQI__
	{"mrp.otatq", MR_APPID_TIANQI, STR_DSM_TIANQI, IMG_DSM_LIST_TIANQI},
#endif	

#ifdef __MR_CFG_ENTRY_MPLIVE__
	{"mrp.mplive", MR_APPID_MPLIVE, STR_DSM_MPLIVE, IMG_DSM_LIST_MPLIVE},
#endif

#ifdef __MR_CFG_ENTRY_MPCHAT__
	{"mrp.mpchat", MR_APPID_MPCHAT, STR_DSM_MPCHAT, IMG_DSM_LIST_MPCHAT},
#endif

#ifdef __MR_CFG_ENTRY_TOMCAT__
	{"mrp.talkcat", MR_APPID_TOMCAT, STR_DSM_TOMCAT, IMG_DSM_LIST_TOMCAT},
#endif

#ifdef __MR_CFG_ENTRY_FRUIT__
	{"mrp.fruit", MR_APPID_FRUIT, STR_DSM_FRUIT, IMG_DSM_LIST_FRUIT},
#endif

#ifdef __MR_CFG_ENTRY_JS__
	{"mrp.js", MR_APPID_JS, STR_DSM_JS, IMG_DSM_LIST_JS},
#endif

#ifdef __MR_CFG_ENTRY_SKIRT__
	{"mrp.skirt", MR_APPID_SKIRT, STR_DSM_SKIRT, IMG_DSM_LIST_SKIRT},
#endif

#ifdef __MR_CFG_ENTRY_BYDR__
	{"mrp.bydr", MR_APPID_BYDR, STR_DSM_BYDR, IMG_DSM_LIST_BYDR},
#endif

#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
	{"mrp.gfkxn", MR_APPID_ANGRYBIRD, STR_DSM_BIRD, IMG_DSM_LIST_ANGRYBIRD},
#endif

#ifdef __MR_CFG_ENTRY_SINAWEIBO__
	{"mrp.weibo", MR_APPID_WEIBO, STR_DSM_SINAWEIBO, IMG_DSM_LIST_SINAWEIBO},
#endif	

#ifdef __MR_CFG_ENTRY_NETCLOCK__
	{"mrp.nclock", MR_APPID_NETCLOCK, STR_DSM_NCLOCK, IMG_DSM_LIST_NETCLOCK},
#endif	

#ifdef __MR_CFG_ENTRY_NES__
	{"mrp.nes", MR_APPID_NES, STR_DSM_NES, IMG_DSM_LIST_NES},
#endif

#ifdef __MR_CFG_ENTRY_FACEBOOK__
	{"mrp.fbook", MR_APPID_FACEBOOK, STR_DSM_FACEBOOK, IMG_DSM_LIST_MSN},
#endif

#ifdef __MR_CFG_ENTRY_TWITTER__
	{"mrp.twitter", MR_APPID_TWITTER, STR_DSM_TWITTER, IMG_DSM_LIST_MSN},
#endif
#ifdef __SKY_SECURITY_GUARD__
	{"mrp.mpsjws", MR_APPID_GUARD, STR_ID_SKY_SG, IMG_DSM_LIST_GUARD},
#endif	


#ifdef __MR_CFG_ENTRY_YAHOO__
	{"mrp.yahoo", MR_APPID_YAHOO, STR_DSM_YAHOO, IMG_DSM_LIST_GAME},
#endif



};

U32 srv_mrpfactory_get_table_size(void)
{
	return sizeof(g_mrp_shortcut_info_list)/sizeof(srv_mrp_appmgr_shortcut_info_struct);
}

static const WCHAR* srv_mrpfactory_get_cfg_path(void);
static const WCHAR* srv_mrpfactory_get_root_path(void);

U32 srv_mrpfactory_get_app_package_num(srv_appmgr_flag_type flag)
{
	int dyn_app_num = 0;

#ifdef __MR_CFG_FEATURE_DAM__
	dyn_app_num = FS_Count(srv_mrpfactory_get_cfg_path(), FS_FILE_TYPE, NULL, 0);
	if (dyn_app_num < 0) dyn_app_num = 0;
#endif

	switch(flag)
	{
	case SRV_APPMGR_PRE_INSTALLED_APP_PACKAGE_FLAG_TYPE:
		return 0;
		
	case SRV_APPMGR_ALL_APP_PACKAGE_IN_MAIN_MENU_FLAG_TYPE:
	default:
		return srv_mrpfactory_get_table_size() + dyn_app_num;
	}
}


mmi_ret srv_mrpfactory_get_app_package_list(srv_appmgr_flag_type flag, mmi_app_package_name_struct *name_array, U32 num)
{
	U32 i = 0, j = 0;
	U32 app_num = srv_mrpfactory_get_table_size();
    
	for (i = 0; i < app_num; i ++)
	{
		if (SRV_APPMGR_ALL_APP_PACKAGE_FLAG_TYPE == flag ||
			SRV_APPMGR_ALL_APP_PACKAGE_IN_MAIN_MENU_FLAG_TYPE == flag )
		{
			memcpy((mmi_app_package_char*)(name_array+j), g_mrp_shortcut_info_list[i].package_name, MMI_APP_NAME_MAX_LEN);
			j++;
		}

		if (j >= num) return MMI_RET_OK;
	}

#ifdef __MR_CFG_FEATURE_DAM__
{
	int handle;
	FS_DOSDirEntry entry;
	WCHAR filename[MMI_APP_NAME_MAX_LEN] = {0};
	WCHAR name_pattern[DSM_MAX_FILE_LEN] = {0};

	kal_wsprintf(name_pattern, "%wmrp.dyn.*", srv_mrpfactory_get_cfg_path());
	handle = FS_FindFirst(name_pattern, 0, 0, &entry, filename, MMI_APP_NAME_MAX_LEN);
	if (handle < 0) return MMI_RET_OK;
	
	do {
		mmi_chset_convert(MMI_CHSET_UCS2, MMI_CHSET_UTF8, (char*)filename, (char*)&name_array[j], MMI_APP_NAME_MAX_LEN);
		++j;
	} while(FS_FindNext(handle, &entry, filename, MMI_APP_NAME_MAX_LEN) >= 0 && j < num);

	FS_FindClose(handle);
}
#endif

	return MMI_RET_OK;
}


mmi_ret srv_mrpfactory_get_app_package_name(const mmi_app_package_char *app_name, WCHAR *string)
{
	U32 i = 0;
	U32 app_num = srv_mrpfactory_get_table_size();

#ifdef __MR_CFG_FEATURE_DAM__
	if (0 == strncmp(app_name, "mrp.dyn.", 8))
	{
		int f;
		WCHAR filename[DSM_MAX_FILE_LEN] = {0};
		mr_dam_appinfo_t dam_appinfo;
		kal_uint32 read;

		kal_wsprintf(filename, "%w%s", srv_mrpfactory_get_cfg_path(), app_name);

		f = FS_Open(filename, FS_READ_ONLY);
		if (f > 0)
		{
			memset(&dam_appinfo, 0, sizeof(mr_dam_appinfo_t));
			FS_Read(f, &dam_appinfo, sizeof(mr_dam_appinfo_t), &read);
			FS_Close(f);

			if (strcmp(Get_Current_Lang_CountryCode(), "zh-CN") == 0)
			{
				mr_str_convert_endian(dam_appinfo.title_ch);
				mmi_wcsncpy(string, (WCHAR*)dam_appinfo.title_ch, MR_DAM_TITLE_LENGHT_MAX / 2);
			}
			else
			{
				mr_str_gb_to_ucs2(dam_appinfo.title_en, (char*)string);
			}
		}

		return MMI_RET_OK;
	}
#endif

	for (i = 0; i < app_num; i ++)
	{
		if (0 == strncmp((const char *)g_mrp_shortcut_info_list[i].package_name, app_name, MMI_APP_NAME_MAX_LEN))
		{
			MMI_ASSERT(g_mrp_shortcut_info_list[i].app_str_id);
			mmi_wcsncpy(string, (WCHAR*)get_string(g_mrp_shortcut_info_list[i].app_str_id), MMI_APP_NAME_MAX_LEN);
			return MMI_RET_OK;
		}
	}
	
	return MMI_RET_ERR;
}


mmi_ret srv_mrpfactory_get_app_package_image(const mmi_app_package_char *app_name, mmi_image_src_struct *image)
{
	U32 i = 0;
	U32 app_num = srv_mrpfactory_get_table_size();

#ifdef __MR_CFG_FEATURE_DAM__
	if (0 == strncmp(app_name, "mrp.dyn.", 8))
	{
		int f;
		WCHAR filename[DSM_MAX_FILE_LEN] = {0};
		mr_dam_appinfo_t dam_appinfo;
		kal_uint32 read;

		kal_wsprintf(filename, "%w%s", srv_mrpfactory_get_cfg_path(), app_name);

		f = FS_Open(filename, FS_READ_ONLY);
		if (f > 0)
		{
			int file_attr;
			
			memset(&dam_appinfo, 0, sizeof(mr_dam_appinfo_t));
			FS_Read(f, &dam_appinfo, sizeof(mr_dam_appinfo_t), &read);
			FS_Close(f);
			
			image->type = MMI_IMAGE_SRC_TYPE_PATH;
			mr_fs_separator_vm_to_local(dam_appinfo.image_path);
			kal_wsprintf(image->data.path, "%w%s", srv_mrpfactory_get_root_path(), dam_appinfo.image_path);

			// 如果图片不存在再转换一次
			file_attr = FS_GetAttributes(image->data.path);
			if (file_attr < FS_NO_ERROR)
			{
				WCHAR* p;
				
				p = kal_wstrrchr(image->data.path, '.');
				memset(filename, 0, sizeof(filename));
				kal_wstrncpy(filename, image->data.path, p - image->data.path);

				mr_skybmp2bmp(filename, image->data.path);
//todo 设置默认图片
#if 1
				if( FS_GetAttributes(image->data.path) < FS_NO_ERROR )
				{
					image->type = MMI_IMAGE_SRC_TYPE_RES_ID;
					image->data.res_id = IMG_DSM_LIST_MPZONE;
				}
#endif
//end
			}
		}

		return MMI_RET_OK;
	}
#endif

	for (i = 0; i < app_num; i ++)
	{
		if (0 == strncmp((const char *)g_mrp_shortcut_info_list[i].package_name, app_name, MMI_APP_NAME_MAX_LEN))
		{
			image->type = MMI_IMAGE_SRC_TYPE_RES_ID;
			image->data.res_id = g_mrp_shortcut_info_list[i].app_img_id;
			return MMI_RET_OK;
		}
	}
	
	return MMI_RET_ERR;
}

mmi_ret srv_mrpfactory_get_app_package_info(const mmi_app_package_char *app_name, srv_app_info_struct *info)
{
	U32 i = 0;
	U32 app_num = srv_mrpfactory_get_table_size();

	memset(info, 0, sizeof(srv_app_info_struct));

	srv_mrpfactory_get_app_package_name(app_name, info->string);
	srv_mrpfactory_get_app_package_image(app_name, &info->image);
	
	return MMI_RET_OK;
}

MMI_ID srv_mrpfactory_launch_ex(srv_appfactory_launch_type_enum type, const mmi_app_package_char *app_name, void* param, U32 param_size)
{
	U32 i = 0;
	U32 app_num = srv_mrpfactory_get_table_size();

	mmi_ret     ret = MMI_RET_ERR;
	MMI_ID      app_group_id = GRP_ID_INVALID;

#ifdef __MR_CFG_FEATURE_DAM__
	if (0 == strncmp(app_name, "mrp.dyn.", 8))
	{
		int f;
		WCHAR filename[DSM_MAX_FILE_LEN] = {0};
		mr_dam_appinfo_t dam_appinfo;
		kal_uint32 read;

		kal_wsprintf(filename, "%w%s", srv_mrpfactory_get_cfg_path(), app_name);

		f = FS_Open(filename, FS_READ_ONLY);
		if (f > 0)
		{
			memset(&dam_appinfo, 0, sizeof(mr_dam_appinfo_t));
			FS_Read(f, &dam_appinfo, sizeof(mr_dam_appinfo_t), &read);
			FS_Close(f);

			mr_app_start_mrp_by_cookie(dam_appinfo.start_data);
		}

		return MMI_RET_OK;
	}
#endif

	for (i = 0; i < app_num; i ++)
	{
		if (0 == strncmp((const char *)g_mrp_shortcut_info_list[i].package_name, app_name, MMI_APP_NAME_MAX_LEN))
		{
                    mr_app_start_mrp_by_appid(g_mrp_shortcut_info_list[i].app_id);
		}
	}
	
	return app_group_id;
}
 

MMI_ID srv_mrpfactory_launch(const mmi_app_package_char *app_name, mmi_app_launch_param_struct* param, U32 param_size)
{
	return srv_mrpfactory_launch_ex(SRV_APP_FACTORY_APP_LAUNCH_TYPE, app_name, (void*)param, param_size);
} 

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1224)
mmi_ret srv_mrpfactory_get_unique_app_id_list(srv_appmgr_flag_type flag, U32 *id, U32 num)
{
	U32 i = 0, j = 0;
	U32 app_num = srv_mrpfactory_get_table_size();
    
	for (i = 0; i < app_num; i ++)
	{
		if (SRV_APPMGR_ALL_APP_PACKAGE_FLAG_TYPE == flag ||
			SRV_APPMGR_ALL_APP_PACKAGE_IN_MAIN_MENU_FLAG_TYPE == flag )
		{
			id[j] = VAPP_MYTHROAD + i;
			j++;
		}

		if (j >= num) return MMI_RET_OK;
	}

	return MMI_RET_OK;
}

mmi_ret srv_mrpfactory_get_package_name_by_unique_id(U32 id, mmi_app_package_char *app_name)
{
	U32 i = 0;
	U32 app_num = srv_mrpfactory_get_table_size();

	for (i = 0; i < app_num; i ++)
	{
		if (id == (VAPP_MYTHROAD + i))
		{
			memcpy(app_name, g_mrp_shortcut_info_list[i].package_name, MMI_APP_NAME_MAX_LEN);
			return MMI_RET_OK;
		}
	}
	
	return MMI_RET_ERR;
}

U32 srv_mrpfactory_get_unique_app_id(const mmi_app_package_char *app_name)
{
	U32 i = 0;
	U32 app_num = srv_mrpfactory_get_table_size();

	for (i = 0; i < app_num; i ++)
	{
		if (0 == strncmp((const char *)g_mrp_shortcut_info_list[i].package_name, app_name, MMI_APP_NAME_MAX_LEN))
		{
			return (VAPP_MYTHROAD + i);
		}
	}
	
	return 0;
}
#endif

static const WCHAR* srv_mrpfactory_get_cfg_path(void)
{
	static WCHAR path[] = L"c:\\mythroad\\@dam\\";

	path[0] = MMI_CARD_DRV;

	FS_CreateDir(path);

	return path;
}

static const WCHAR* srv_mrpfactory_get_root_path(void)
{
	static WCHAR path[] = L"c:\\mythroad\\";

	path[0] = MMI_CARD_DRV;

	return path;
}

#ifdef __MR_CFG_FEATURE_DAM__
void srv_mrpfactory_install_app(const mr_dam_appinfo_t* info)
{
	WCHAR file_name[DSM_MAX_FILE_LEN];
	int f;
	kal_uint32 written;
	WCHAR  tmp[DSM_MAX_FILE_LEN];	// 存放转换后的文件名
	mr_dam_appinfo_t appinfo;	// 保存修改后的信息

	memcpy(&appinfo, info, sizeof(mr_dam_appinfo_t));
	//huangsunbo 20111027 解决单独菜单入口应用下载以后,主菜单会再增加一个菜单问题.
	#ifdef __MR_CFG_ENTRY_MPLIVE__
	if( appinfo.appid == MR_APPID_MPLIVE )
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_MPCHAT__
	if( appinfo.appid == MR_APPID_MPCHAT )
	{
		return;
	}
	#endif


	#ifdef __MR_CFG_ENTRY_EBOOK__
	if( appinfo.appid == MR_APPID_SKYREAD )
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_QQ__
	if( appinfo.appid == MR_APPID_SKYQQ  ||  appinfo.appid == MR_APPID_QQ2008 )
	{
		return;
	}
	#endif

	
	#ifdef __MR_CFG_ENTRY_TOMCAT__
	if( appinfo.appid == MR_APPID_TOMCAT)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_FRUIT__
	if( appinfo.appid == MR_APPID_FRUIT)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
	if( appinfo.appid == MR_APPID_ANGRYBIRD)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_MSN__
	if( appinfo.appid == MR_APPID_MSN)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_FETION__
	if( appinfo.appid == MR_APPID_FETION)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_KAIXIN__
	if( appinfo.appid == MR_APPID_KAIXIN)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_SINAWEIBO__
	if( appinfo.appid == MR_APPID_WEIBO)
	{
		return;
	}
	#endif


	#ifdef __MR_CFG_ENTRY_TIANQI__
	if( appinfo.appid == MR_APPID_TIANQI)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_NETCLOCK__
	if( appinfo.appid == MR_APPID_NETCLOCK)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_NETGAME__
	if( appinfo.appid == MR_APPID_NET_GAMES  || appinfo.appid == 400001)
	{
		return;
	}
	#endif

	#ifdef __MR_CFG_ENTRY_SHOUXIN__
	if( appinfo.appid == MR_APPID_SHOUXIN )
	{
		return;
	}
	#endif


	//huangsunbo 20111027 end...
	kal_wsprintf(file_name, "%wmrp.dyn.%u", srv_mrpfactory_get_cfg_path(), appinfo.appid);

	f = FS_Open(file_name, FS_CREATE_ALWAYS | FS_READ_WRITE);

	if (f < 0) return;

	strcat((char*)appinfo.image_path, ".bmp");
	//FS_Write(f, (void*)&appinfo, sizeof(mr_dam_appinfo_t), &written);
	//FS_Close(f);

	// 转换图片格式
	mr_fs_separator_vm_to_local((U8*)info->image_path);
	kal_wsprintf(file_name, "%w%s", srv_mrpfactory_get_root_path(), info->image_path);

	mr_fs_separator_vm_to_local(appinfo.image_path);
	kal_wsprintf(tmp, "%w%s", srv_mrpfactory_get_root_path(), appinfo.image_path);

	if (mr_skybmp2bmp(file_name, tmp) == MR_TRUE)
	{
		FS_Write(f, (void*)&appinfo, sizeof(mr_dam_appinfo_t), &written);
	}
	else
	{
		FS_Write(f, (void*)info, sizeof(mr_dam_appinfo_t), &written);
	}
	
	FS_Close(f);

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1132)
	srv_appmgr_update_app_info(NULL, EVT_ID_SRV_APPMGR_INSTALL_PACKAGE);
#else	
	vapp_mainmenu_app_state_notify(VAPP_MAINMENU_NOTIFY_APP_ADD, NULL);
#endif	
}

void srv_mrpfactory_remove_app(const mr_dam_remove_info_t* info)
{
	WCHAR file_name[DSM_MAX_FILE_LEN];
	int f;
	mr_dam_appinfo_t appinfo;
	kal_uint32 read;

	kal_wsprintf(file_name, "%wmrp.dyn.%u", srv_mrpfactory_get_cfg_path(), info->appid);

	f = FS_Open(file_name, FS_READ_ONLY);

	if (f > 0)
	{
		FS_Read(f, &appinfo, sizeof(appinfo), &read);

		mr_fs_separator_vm_to_local((U8*)appinfo.image_path);
		kal_wsprintf(file_name, "%w%s", srv_mrpfactory_get_root_path(), appinfo.image_path);
		FS_Delete(file_name);	
		
		FS_Close(f);
	}

	kal_wsprintf(file_name, "%wmrp.dyn.%u", srv_mrpfactory_get_cfg_path(), info->appid);

	FS_Delete(file_name);	
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1132)
	srv_appmgr_update_app_info(NULL, EVT_ID_SRV_APPMGR_UNINSTALL_PACKAGE);
#else
	vapp_mainmenu_app_state_notify(VAPP_MAINMENU_NOTIFY_APP_REMOVE, NULL);
#endif	
}

void srv_mrpfactory_refresh_app(void)
{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1212)
#elif (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1132)
	srv_appmgr_update_app_info(NULL, EVT_ID_SRV_APPMGR_UPDATE_COUNT);
#else
	vapp_mainmenu_app_state_notify(VAPP_MAINMENU_NOTIFY_APP_REFRESH_ALL, NULL);
#endif	
}

void srv_mrpfactory_uninstall_app(char* name)
{
	WCHAR file_name[DSM_MAX_FILE_LEN];
	
	kal_wsprintf(file_name, "%w%s", srv_mrpfactory_get_cfg_path(), name);

	FS_Delete(file_name);		
}
#endif

#endif/*__MMI_DSM_NEW__*/
