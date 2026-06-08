#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "ProfilesAppGprot.h"
#include "AlarmSrvGprot.h"
#include "SSCStringHandle.h"
#include "ProfilesSrv.h"
#include "WPSSProtos.h"
#include "NITZSrvGprot.h"
#include "DtcntSrvGprot.h"
#include "DateTimeGprot.h"
#include "app_str.h"
#include "custom_mmi_folders_config.h"
#include "Cache_sw.h"
#if defined(__DYNAMIC_SWITCH_CACHEABILITY__) && defined(__MTK_TARGET__)
#include "mmu.h"
#endif

#include "mrp_include.h"

extern int32 mr_setting_wallpaper_adp(int16* path);
extern int32 mr_setting_screensaver_adp(int16* path);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int32 mr_setting_set_wallpaper(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_wallpaper_info_t* req = ( mr_wallpaper_info_t*)input;
	U16 fullpathname[DSM_MAX_FILE_LEN] ={0};

	if(req == NULL || input_len == 0 || req->filename == NULL)
		return MR_FAILED;

	mr_fs_get_filename((char*)fullpathname, (char*)req->filename);
	
	if (req->setType == MR_IDLE_BG_PAPER)
	{
		if (mr_setting_wallpaper_adp(fullpathname))
			return MR_SUCCESS;
		else
			return MR_FAILED;
	}
	else if (req->setType == MR_IDLE_SAVER_PAPER)
	{
		if (mr_setting_screensaver_adp(fullpathname))
			return MR_SUCCESS;
		else
			return MR_FAILED;
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_setting_set_ring(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	T_DSM_RING_SET *ringSet = (T_DSM_RING_SET*)input;
	WCHAR fullpathname[DSM_MAX_FILE_LEN] = {0};

	if (ringSet == NULL)
	{
		return MR_FAILED;
	}
		
	mr_fs_get_filename((char*)fullpathname, (char*)ringSet->path);

	if (ringSet->type == MR_ALARM_RING)
	{
		srv_alm_node_type *node;
		uint8 i;

		for (i = 0; i < NUM_OF_ALM; i++)
		{
			node = srv_alm_read(i);	

			if (node->Hour != 0xFF)
			{
				node->AudioOptionValue =  0;
				node->AudioOption = SRV_PROF_AUDIO_RES_FILE;
				mmi_wcscpy((WCHAR *)node->tone_path, fullpathname);

				srv_alm_write(i, node);
			}
		}

		return MR_SUCCESS;
	} 
	else
	{
		enum
		{
			INCOMING_SIM1_EXT_SLOT = 0,
    #if (SRV_PROF_MT_CALL_TONE_AMOUNT > 1)
			INCOMING_SIM2_EXT_SLOT,
    #endif
    #if (SRV_PROF_MT_CALL_TONE_AMOUNT > 2)
			INCOMING_SIM3_EXT_SLOT,
    #endif
    #if (SRV_PROF_MT_CALL_TONE_AMOUNT > 3)
			INCOMING_SIM4_EXT_SLOT,
    #endif
			MESSAGE_SIM1_EXT_SLOT,
    #if (SRV_PROF_SMS_TONE_AMOUNT > 1)
			MESSAGE_SIM2_EXT_SLOT,
    #endif
    #if (SRV_PROF_SMS_TONE_AMOUNT > 2)
			MESSAGE_SIM3_EXT_SLOT,
    #endif
    #if (SRV_PROF_SMS_TONE_AMOUNT > 3)
			MESSAGE_SIM4_EXT_SLOT,
    #endif
    		NUM_OF_SLOT
		};
		
		srv_prof_settings_enum prof_settings;
		int32 active_sim;
		int32 extId = 0;
		uint8 slot = 0xFF;
		uint8 offset;

		active_sim = mr_sim_get_active();

		if (ringSet->type == MR_CALL_RING)
		{
			if (active_sim == DSM_SLAVE_SIM)
			{
				prof_settings = SRV_PROF_SETTINGS_MT_CALL_CARD2_TONE;
			#if (SRV_PROF_MT_CALL_TONE_AMOUNT > 1)
				slot = INCOMING_SIM2_EXT_SLOT;
				offset = 1;
			#endif
			}
			else if (active_sim == DSM_THIRD_SIM)
			{
				prof_settings = SRV_PROF_SETTINGS_MT_CALL_TONE + 2;	// SRV_PROF_SETTINGS_MT_CALL_CARD3_TONE
			#if (SRV_PROF_MT_CALL_TONE_AMOUNT > 2)
				slot = INCOMING_SIM3_EXT_SLOT;
				offset = 2;
			#endif
			}
			else if (active_sim == DSM_THIRD_SIM)
			{
				prof_settings = SRV_PROF_SETTINGS_MT_CALL_TONE + 3;	// SRV_PROF_SETTINGS_MT_CALL_CARD4_TONE
			#if (SRV_PROF_MT_CALL_TONE_AMOUNT > 3)
				slot = INCOMING_SIM4_EXT_SLOT;
				offset = 3;
			#endif
			}
			else
			{
				prof_settings = SRV_PROF_SETTINGS_MT_CALL_TONE;
				slot = INCOMING_SIM1_EXT_SLOT;
				offset = 0;
			}
		}
		else if (ringSet->type == MR_SMS_RING)
		{
			if (active_sim == DSM_SLAVE_SIM)
			{
				prof_settings = SRV_PROF_SETTINGS_SMS_CARD2_TONE;
			#if (SRV_PROF_SMS_TONE_AMOUNT > 1)
				slot = MESSAGE_SIM2_EXT_SLOT;
				offset = 1;
			#endif
			}
			else if (active_sim == DSM_THIRD_SIM)
			{
				prof_settings = SRV_PROF_SETTINGS_SMS_TONE + 2;	// SRV_PROF_SETTINGS_SMS_CARD3_TONE
			#if (SRV_PROF_SMS_TONE_AMOUNT > 2)
				slot = MESSAGE_SIM3_EXT_SLOT;
				offset = 2;
			#endif
			}
			else if (active_sim == DSM_THIRD_SIM)
			{
				prof_settings = SRV_PROF_SETTINGS_SMS_TONE + 3;	// SRV_PROF_SETTINGS_SMS_CARD4_TONE
			#if (SRV_PROF_SMS_TONE_AMOUNT > 3)
				slot = MESSAGE_SIM4_EXT_SLOT;
				offset = 3;
			#endif
			}
			else
			{
				prof_settings = SRV_PROF_SETTINGS_SMS_TONE;
				slot = MESSAGE_SIM1_EXT_SLOT;
				offset = 0;
			}
		}
		
		if (slot == 0xFF)
		{
			return MR_FAILED;
		}
		
		srv_prof_set_file_path_to_ext_melody(slot, fullpathname);
        extId = (int32)srv_prof_get_audio_id_from_audio_resourece(SRV_PROF_AUDIO_RES_FILE, slot);
        srv_prof_set_current_profile_value(prof_settings, (void*)&extId);

        if (ringSet->type == MR_CALL_RING)
        {
	        {
		    #if (SRV_PROF_VT_CALL_TONE_AMOUNT > 0)
		        srv_prof_set_current_profile_value(SRV_PROF_SETTINGS_VT_CALL_TONE + offset, (void*)&extId);
		    #endif
		    }

	        {
	            srv_prof_setting_struct settings_silent;
	            srv_prof_get_profile_values(SRV_PROF_PROFILE_2, &settings_silent);
	            settings_silent.mt_call_tone[0] = extId;
	        #if (SRV_PROF_VT_CALL_TONE_AMOUNT > 0)
	            settings_silent.vt_call_tone[0] = extId;
	        #endif
	            srv_prof_set_profile_values(SRV_PROF_PROFILE_2, &settings_silent);
	        }
        }
        else if (ringSet->type == MR_SMS_RING)
        {
        	srv_prof_set_current_profile_value(SRV_PROF_SETTINGS_MMS_TONE + offset, (void*)&extId);
       		srv_prof_set_current_profile_value(SRV_PROF_SETTINGS_VOICE_TONE + offset, (void*)&extId);

            {
                srv_prof_setting_struct settings_silent;
                srv_prof_get_profile_values(SRV_PROF_PROFILE_2, &settings_silent);

                settings_silent.sms_tone[0] = extId;
            #if (SRV_PROF_MMS_TONE_AMOUNT > 0)
                settings_silent.mms_tone[0] = extId;
            #endif
            #if (SRV_PROF_VOICE_TONE_AMOUNT > 0)
                settings_silent.voice_tone[0] = extId;
            #endif
                srv_prof_set_profile_values(SRV_PROF_PROFILE_2, &settings_silent);
            }
        }

		return MR_SUCCESS;
	}
}


int32 mr_setting_set_theme(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_IGNORE;
}


int32 mr_setting_restore_theme(int32 param)
{
	return MR_IGNORE;
}


int32 mr_setting_get_active_theme_id(int32 param)
{
	return MR_FAILED;
}


int32 mr_setting_set_theme_mainmenu_index(int32 index)
{
	return MR_IGNORE;
}


int32 mr_setting_get_cur_scene(int32 param)
{
	int32 scene;

	switch (srv_prof_get_activated_profile())
	{
	case MMI_PROFILE_GENERAL:
		scene = MR_SCENE_NORMAL;
		break;

	case MMI_PROFILE_MEETING:
		scene = MR_SCENE_MEETING;
		break;

	case MMI_PROFILE_OUTDOOR:
		scene = MR_SCENE_OUTDOOR;
		break;

	default:
		scene = MR_SCENE_MUTE;
		break;
	
	}

	return scene + MR_PLAT_VALUE_BASE;	
}


int32 mr_setting_check_wifi_support(int32 param)
{
#if defined(__MMI_DSM_WIFI_SUPPORT__) && defined(__WIFI_SUPPORT__)
	/*如果wifi处于关闭的状态的话，也是返回MR_FAILED*/
	if (srv_dtcnt_wlan_status() == SRV_DTCNT_WLAN_STATUS_CONNECTED)
	{
		return MR_SUCCESS;
	} else
#endif
	return MR_FAILED;
}


int32  mr_setting_wifi_switch(int32 s)
{
#ifdef __MMI_DSM_WIFI_SUPPORT__
	extern BOOL isDsmVIAWIFI;
	isDsmVIAWIFI = s? TRUE : FALSE;
	return MR_SUCCESS;
#else
	return MR_IGNORE;
#endif
}


int32 mr_setting_wifi_exist(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
//是否存在WIFI功能
#ifdef __WIFI_SUPPORT__
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}


int32 mr_setting_get_screen_type(int32 param)
{
#ifdef  __MMI_TOUCH_SCREEN__
#ifdef __MMI_REDUCED_KEYPAD__
	return MR_ONLY_TOUCH_SCREEN;
#else
	return MR_TOUCH_SCREEN;
#endif
#else
	return MR_NORMAL_SCREEN;
#endif

}

int32 mr_setting_get_applist_title(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(output == NULL||output_len == NULL)
		return MR_FAILED;
		
	*output = (uint8 *)MR_APPLIST_TITLE;
	*output_len = strlen(MR_APPLIST_TITLE);
	
	return MR_SUCCESS;
}


int32 mr_setting_get_font_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	static int32 wordInfo = (__MR_CFG_VAR_ENWORD_H__<<24 ) |(__MR_CFG_VAR_ENWORD_W__<<16) |(__MR_CFG_VAR_CHWORD_H__<<8 ) |(__MR_CFG_VAR_CHWORD_W__) ;

	if(output == NULL||output_len== NULL)
		return MR_FAILED;

	*output = (unsigned char*)&wordInfo;
	*output_len = 4;
	
	return MR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* s_support_langs_ssc[] = {
#if defined(__MMI_LANG_ENGLISH__)		
	SSC_ENGLISH,
#endif
#if defined(__MMI_LANG_TR_CHINESE__)		
	SSC_TCHINESE, 
#endif
#if defined(__MMI_LANG_SPANISH__)
	SSC_SPANISH, 
#endif
#if defined(__MMI_LANG_DANISH__)
	SSC_DANISH, 
#endif
#if defined(__MMI_LANG_POLISH__)
	SSC_POLISH,
#endif
#if defined(__MMI_LANG_FRENCH__)
	SSC_FRENCH, 
#endif
#if defined(__MMI_LANG_GERMAN__)
	SSC_GERMAN, 
#endif
#if defined(__MMI_LANG_ITALIAN__)
	SSC_ITALIAN, 
#endif
#if defined(__MMI_LANG_THAI__)
	SSC_THAI, 
#endif
#if defined(__MMI_LANG_RUSSIAN__)
	SSC_RUSSIAN,
#endif
#if defined(__MMI_LANG_BULGARIAN__)
	SSC_BULGARIAN, 
#endif
#if defined(__MMI_LANG_UKRAINIAN__)
	SSC_UKRAINIAN, 
#endif
#if defined(__MMI_LANG_PORTUGUESE__)
	SSC_PORTUGUESE, 
#endif
#if defined(__MMI_LANG_TURKISH__)
	SSC_TURKISH, 
#endif
#if defined(__MMI_LANG_VIETNAMESE__)
	SSC_VIETNAMESE, 
#endif
#if defined(__MMI_LANG_INDONESIAN__)
	SSC_INDONESIAN, 
#endif
#if defined(__MMI_LANG_CZECH__)
	SSC_CZECH, 
#endif
#if defined(__MMI_LANG_MALAY__)
	SSC_MALAY, 
#endif
#if defined(__MMI_LANG_FINNISH__)
	SSC_FINNISH, 
#endif
#if defined(__MMI_LANG_HUNGARIAN__)
	SSC_HUNGARIAN, 
#endif
#if defined(__MMI_LANG_SLOVAK__)
	SSC_SLOVAK, 
#endif
#if defined(__MMI_LANG_DUTCH__)
	SSC_DUTCH, 
#endif
#if defined(__MMI_LANG_NORWEGIAN__)
	SSC_NORWEGIAN, 
#endif
#if defined(__MMI_LANG_SWEDISH__)
	SSC_SWEDISH, 
#endif
#if defined(__MMI_LANG_CROATIAN__)
	SSC_CROATIAN, 
#endif
#if defined(__MMI_LANG_ROMANIAN__)
	SSC_ROMANIAN, 
#endif
#if defined(__MMI_LANG_SLOVENIAN__)
	SSC_SLOVENIAN, 
#endif
#if defined(__MMI_LANG_GREEK__)
	SSC_GREEK, 
#endif
#if defined (__MMI_LANG_HEBREW__)
	SSC_HEBREW, 
#endif
#if defined (__MMI_LANG_ARABIC__)
	SSC_ARABIC, 
#endif
#if defined (__MMI_LANG_PERSIAN__)
	SSC_PERSIAN, 
#endif
#if defined (__MMI_LANG_URDU__)
	SSC_URDU, 
#endif
#if defined (__MMI_LANG_HINDI__)
	SSC_HINDI, 
#endif
#if defined (__MMI_LANG_MARATHI__)
	SSC_MARATHI, 
#endif
#if defined (__MMI_LANG_TAMIL__)
	SSC_TAMIL, 
#endif
#if defined (__MMI_LANG_BENGALI__)
	SSC_BENGALI,
#endif
#if defined (__MMI_LANG_PUNJABI__)
	SSC_PUNJABI, 
#endif
#if defined (__MMI_LANG_TELUGU__)
	SSC_TELUGU, 
#endif
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined (__MMI_LANG_SA_PORTUGUESE__)	//sky chenwei add for south American
	SSC_SA_PORTUGUESE, 
#endif
#if defined (__MMI_LANG_SA_SPANISH__)		//sky chenwei add for south American
	SSC_SA_SPANISH, 
#endif
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_LANG_SM_CHINESE__)
	SSC_SCHINESE, 
#endif
};


static const int32 s_support_langs[] = {
#if defined(__MMI_LANG_ENGLISH__)		
	MR_ENGLISH,
#endif
#if defined(__MMI_LANG_TR_CHINESE__)		
	MR_TCHINESE,
#endif
#if defined(__MMI_LANG_SPANISH__)
	MR_SPANISH,
#endif
#if defined(__MMI_LANG_DANISH__)
	MR_DANISH,
#endif
#if defined(__MMI_LANG_POLISH__)
	MR_POLISH,
#endif
#if defined(__MMI_LANG_FRENCH__)
	MR_FRENCH,
#endif
#if defined(__MMI_LANG_GERMAN__)
	MR_GERMAN,
#endif
#if defined(__MMI_LANG_ITALIAN__)
	 MR_ITALIAN,
#endif
#if defined(__MMI_LANG_THAI__)
	MR_THAI,
#endif
#if defined(__MMI_LANG_RUSSIAN__)
	MR_RUSSIAN,
#endif
#if defined(__MMI_LANG_BULGARIAN__)
	MR_BULGARIAN,
#endif
#if defined(__MMI_LANG_UKRAINIAN__)
	MR_UKRAINIAN,
#endif
#if defined(__MMI_LANG_PORTUGUESE__)
	MR_PORTUGUESE,
#endif
#if defined(__MMI_LANG_TURKISH__)
	MR_TURKISH,
#endif
#if defined(__MMI_LANG_VIETNAMESE__)
	MR_VIETNAMESE,
#endif
#if defined(__MMI_LANG_INDONESIAN__)
	MR_INDONESIAN,
#endif
#if defined(__MMI_LANG_CZECH__)
	MR_CZECH,
#endif
#if defined(__MMI_LANG_MALAY__)
	MR_MALAY,
#endif
#if defined(__MMI_LANG_FINNISH__)
	MR_FINNISH,
#endif
#if defined(__MMI_LANG_HUNGARIAN__)
	MR_HUNGARIAN,
#endif
#if defined(__MMI_LANG_SLOVAK__)
	MR_SLOVAK,
#endif
#if defined(__MMI_LANG_DUTCH__)
	MR_DUTCH,
#endif
#if defined(__MMI_LANG_NORWEGIAN__)
	MR_NORWEGIAN,
#endif
#if defined(__MMI_LANG_SWEDISH__)
	MR_SWEDISH,
#endif
#if defined(__MMI_LANG_CROATIAN__)
	MR_CROATIAN,
#endif
#if defined(__MMI_LANG_ROMANIAN__)
	MR_ROMANIAN,
#endif
#if defined(__MMI_LANG_SLOVENIAN__)
	MR_SLOVENIAN,
#endif
#if defined(__MMI_LANG_GREEK__)
	MR_GREEK,
#endif
#if defined (__MMI_LANG_HEBREW__)
	MR_HEBREW,
#endif
#if defined (__MMI_LANG_ARABIC__)
	MR_ARABIC,
#endif
#if defined (__MMI_LANG_PERSIAN__)
	MR_PERSIAN,
#endif
#if defined (__MMI_LANG_URDU__)
	MR_URDU,
#endif
#if defined (__MMI_LANG_HINDI__)
	MR_HINDI,
#endif
#if defined (__MMI_LANG_MARATHI__)
	MR_MARATHI,
#endif
#if defined (__MMI_LANG_TAMIL__)
	MR_TAMIL,
#endif
#if defined (__MMI_LANG_BENGALI__)
	MR_BENGALI,
#endif
#if defined (__MMI_LANG_PUNJABI__)
	MR_PUNJABI,
#endif
#if defined (__MMI_LANG_TELUGU__)
	MR_TELUGU,
#endif
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */

#if defined (__MMI_LANG_SA_PORTUGUESE__)	//sky chenwei add for south American
	MR_PORTUGUESE,
#endif
#if defined (__MMI_LANG_SA_SPANISH__)		//sky chenwei add for south American
	MR_SPANISH,
#endif
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#if defined(__MMI_LANG_SM_CHINESE__)
	MR_CHINESE,
#endif
};


int32 mr_setting_get_cur_lang(int32 param)
{
	extern sLanguageDetails *gLanguageArray;
	extern U16 gCurrLangIndex;
 	int i;

	for(i = 0; i < sizeof(s_support_langs_ssc)/sizeof(s_support_langs_ssc[0]); i++)
	{
		if (strcmp((PS8)gLanguageArray[gCurrLangIndex].aLangSSC, s_support_langs_ssc[i]) == 0)
		{
			return s_support_langs[i];
		}
	}
	
	return MR_ENGLISH;
}


int32 mr_setting_get_support_langs(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(!output || !output_len){
		return MR_FAILED;
	}
	
	*output = (uint8*)s_support_langs;
	*output_len = sizeof(s_support_langs);
	
	return MR_SUCCESS;
}


int32 mr_setting_get_gmt_timezone(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
#ifdef __COSMOS_MMI_PACKAGE__

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1212)
	(*(int32*)output) = mmi_wc_get_time_zone(mmi_wc_get_home_index()) * 4;
	return MR_SUCCESS;
#else
	extern srv_nitz_context_struct g_srv_nitz_cntx;

	if(!output || !output_len || *output_len != sizeof(int32))
	{
		return MR_FAILED;
	}

	(*(int32*)output) = g_srv_nitz_cntx.nitz_timezone;

	return MR_SUCCESS;
#endif

#else // __COSMOS_MMI_PACKAGE__

#ifdef __MMI_WC_TZ_SUPPORT__
	int32 CurrTZ;
	S16 tmpTimeZone;
	S16 error;
#ifdef __MMI_DUAL_CLOCK__
	S16 tmpFrmTZ;
#endif

	if(!output || !output_len || *output_len != sizeof(int32)){
		return MR_FAILED;
	}

	//ReadValue(NVRAM_TIMEZONE, &tmpTimeZone, DS_SHORT, &error);		
	CurrTZ = tmpTimeZone;

#ifdef __MMI_DUAL_CLOCK__
	//ReadValue(NVRAM_FRN_TIMEZONE, &tmpFrmTZ, DS_SHORT, &error);		
	CurrTZ = (FLOAT)tmpFrmTZ;
#endif /* __MMI_DUAL_CLOCK__ */

	(*(int32*)output) = CurrTZ;
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif

#endif
}


int32 mr_setting_set_datetime(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	MYTIME *pMytime = (MYTIME *)(input);

	if(pMytime == NULL)
		return MR_FAILED;
	
	mmi_dt_set_rtc_dt(pMytime);
	
	return MR_SUCCESS;
}


#ifdef __MR_CFG_FEATURE_HANDSETID_EX__
extern kal_char* release_verno(void) ;
int32 mr_setting_get_handsetID_ex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_userinfo usrinfo;
	char tmp[__MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__+1] = {0};
	if(!input || input_len < __MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__)
		return MR_FAILED;

	mr_getUserInfo(&usrinfo);
	#if 1//huangsunbo 20120106 部分服务器只能支持长度为32的扩展机型，添加长度限制
	if( strlen((char*)release_verno()) <= (__MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__ -10) )
	{
		strcpy(tmp,release_verno());
	}
	else
	{
		strncpy(tmp,release_verno(),__MR_CFG_VAR_HANDSETID_EX_LENGHT_MAX__ - 10 );
	}
	
	kal_snprintf((char*)input, input_len, "%s!%s", usrinfo.type, tmp);
	#else
	kal_snprintf((char*)input, input_len, "%s!%s", usrinfo.type, __MR_CFG_VAR_HANDSETID_EX__);
	#endif
	return MR_SUCCESS;
}
#endif

int32 mr_setting_image_encode(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_image_encode_info* info;
	GDI_RESULT ret = GDI_FAILED;
	WCHAR* path;
	WCHAR* tmp;

	info = (mr_image_encode_info*)input;

	path = OslMalloc(mr_str_wstrlen((char*)info->path) + 2);
	if (path == NULL)
		return MR_FAILED;

	path[0] = 0;
	app_ucs2_strcat((kal_int8*)path, (kal_int8*)info->path);

	if(path[0] == 'A'||path[0] == 'a')
	{
		path[0] = MMI_SYSTEM_DRV;
	}
	else if(path[0] == 'B'||path[0] == 'b')
	{
		path[0] = MMI_PUBLIC_DRV;
	}
	else
	{
		path[0] = MMI_CARD_DRV;
	}
	
	tmp = path;
	while(tmp = (WCHAR*)app_ucs2_strchr((kal_int8*)tmp, '/'))
	{
		*tmp = '\\';
		++tmp;
	}

	if (info->type == IMG_BMP)
	{
		ret = gdi_image_bmp_encode_file(info->w, info->h, GDI_COLOR_FORMAT_16, info->buf, (PS8)path);
	}
	else if (info->type == IMG_JPG)
	{
		mr_layer_create_info_t layer_info;
		mr_common_rsp_t* rsp;
		int32 len;

		layer_info.x = 0;
		layer_info.y = 0;
		layer_info.w = info->w;
		layer_info.h = info->h;
		layer_info.size = info->w * info->h * 2;
		layer_info.buffer = info->buf;
		
		if (mr_layer_create((uint8*)&layer_info, sizeof(layer_info), (uint8**)&rsp, &len, NULL) == MR_SUCCESS)
		{
			ret = gdi_image_encode_layer_to_jpeg(rsp->p1, (PS8)path);

			mr_layer_free(rsp->p1);
		}
	}

	OslMfree(path);
	
	if (ret == GDI_SUCCEED)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}

void test_image_encode(void)
{
	mr_image_encode_info info;
	U8* buf;

	gdi_layer_get_buffer_ptr(&buf);
	
	info.w = 320;
	info.h = 480;
	info.buf = GDI_LAYERS[0].buf_ptr;
	info.path = (int16*)L"C:/test.jpg";
	info.type = IMG_JPG;

	mr_setting_image_encode((uint8*)&info, 0, 0, 0, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////

MR_PLATEX_FUNC_BEGIN(setting)
	MR_PLATEX_FUNC_ENTRY_SUPPORT(MR_PLATEX_CODE_SETTING_SUPPORT, MR_SUCCESS)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_GET_GMT_TZ, mr_setting_get_gmt_timezone)
	MR_PLATEX_FUNC_ENTRY(MR_PALTEX_CODE_SETTING_GET_SUPPORT_LANGS, mr_setting_get_support_langs)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_WIFI_EXIST, mr_setting_wifi_exist)
#ifdef __MR_CFG_FEATURE_HANDSETID_EX__	
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_GET_HANDSETID_EX, mr_setting_get_handsetID_ex)
#endif

	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_GET_SYSTEM_IMAGE_PATH, mr_setting_get_system_image_path)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_IMAGE_ENCODE, mr_setting_image_encode)
	
#ifdef __MR_CFG_SET_SMS_STATUS__
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_SMS_SET_STATUS, mr_sms_set_sms_status)
#endif
/* 52_xsgrz_patch begin */
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_CONFIG_SAMPLING_PERIOD, mr_app_config_sampling_period)
	MR_PLATEX_FUNC_ENTRY(MR_PLATEX_CODE_SETTING_CONFIG_MOVE_OFFSET, mr_app_config_move_offset)
/* 52_xsgrz_patch end */
MR_PLATEX_FUNC_END()

#endif

