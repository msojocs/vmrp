#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "kbd_table.h"
#include "mdi_datatype.h"
#include "mdi_audio.h"
#include "keypad.h"

#include "mrp_include.h"

#ifdef __MMI_DSM_JOYPAD__
#include "gamepad.h"
#endif



#ifdef __MR_CFG_ENTRY_NES__
#if defined(__MR_CFG_FEATURE_OVERSEA__)
#include "dsm_dsmgame.h"
#else
#include "dsm_neslist.h"
#endif

#if defined(__MR_CFG_FEATURE_OVERSEA__)
extern U8 mr_all_list[];
#define mr_nesEntry mr_all_list
#endif


#ifndef __MMI_DSM_NES_APP_TCARD__
#if defined(__ARM9_MMU__)
#include "dsm_nes35.h"
#elif defined(MT6252)
#include "dsm_nes52.h"
#else
#include "dsm_nes53.h"
#endif
#endif
#endif


#ifdef MMI_ON_HARDWARE_P
#ifdef __MMI_DSM_MEM_NES_SUPPORT__
#pragma arm section zidata = "LARGEPOOL_ZI"
#ifndef MT6252
static kal_uint8 mr_ext_mem[0x20000];
#else
static kal_uint8 mr_ext_mem[0x25000];
#endif
#pragma arm section zidata

#pragma arm section rwdata = "INTERNRW1", zidata = "INTERNZI1"

#if (defined(MT6225) ||  defined(MT6253)  || defined(MT6253T) ||defined(MT6235) ||defined(MT6235B) )
kal_uint8 mr_inter_mem[0x5000];
#elif (defined(MT6226) ||defined(MT6226M)||defined(MT6226D))
kal_uint8 mr_inter_mem[0x8000];
#endif

#pragma arm section rwdata,zidata

#endif
#endif


extern char *dsmEntry;
static mr_nes_keys_st mr_nes_keys;
extern const keypad_struct *keypad_Get_Data(void);


#ifdef __MMI_DSM_JOYPAD__
static mr_nes_joypad_st mr_nes_joypad;
#endif
///////////////////////////////////////////////////////////////////////////////////////////////

int32 mr_nes_initialize(void)
{
#ifdef MMI_ON_HARDWARE_P
#ifdef __MR_CFG_ENTRY_NES__
#ifdef __MMI_DSM_MEM_NES_SUPPORT__
	mr_ext_mem[0] = 0;
#if  !defined(__ARM9_MMU__)  && !defined(MT6252)
	mr_inter_mem[0] =0;
#endif
#endif
	mr_nes_create_root_dir();
#endif
#endif
	return MR_SUCCESS;
}


static kal_uint8 mr_nes_translate_device_key(kal_uint8 device_key)
{
#ifdef MMI_ON_HARDWARE_P
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11B1132)
	return device_key;
#else
	kal_uint8 i = 0;
	
	for(i=0; i<kbd_allkeys; i++)
	{
		if(device_key == keypad_Get_Data()->keypad[i])
			return i;
	}
	return 0;
#endif
#else
	return 0;
#endif
}


int32 mr_nes_get_keypad_map(uint8** output, int32* output_len)
{
	mr_nes_keys.k_up = mr_nes_translate_device_key(DEVICE_KEY_UP);//
	mr_nes_keys.k_down = mr_nes_translate_device_key(DEVICE_KEY_DOWN);
	mr_nes_keys.k_left = mr_nes_translate_device_key(DEVICE_KEY_LEFT);
	mr_nes_keys.k_right = mr_nes_translate_device_key(DEVICE_KEY_RIGHT);
	mr_nes_keys.k_a = mr_nes_translate_device_key(DEVICE_KEY_3);
	mr_nes_keys.k_b = mr_nes_translate_device_key(DEVICE_KEY_HASH);
	mr_nes_keys.auto_a = mr_nes_translate_device_key(DEVICE_KEY_6);
	mr_nes_keys.auto_b = mr_nes_translate_device_key(DEVICE_KEY_0);
	mr_nes_keys.k_se = mr_nes_translate_device_key(DEVICE_KEY_1);
	mr_nes_keys.k_st = mr_nes_translate_device_key(DEVICE_KEY_2);
	*output = (uint8*)&mr_nes_keys;
	*output_len = sizeof(mr_nes_keys);

	return MR_SUCCESS;
}


static U32 mr_nes_get_joypad_device_key(void)
{
#ifdef __MMI_DSM_JOYPAD__
	return EV_GameSimuKeyValue();
#else
	return 0;
#endif
}


/*bit 0     1      2     3     4   5       6      7-      8-      9      10    11     12     13    14 
*      up  down left right   a  b   autoa autob select  start  topl  topr  mute  esc  menu
*/
//,>>
int32 mr_nes_get_joypad_key(uint8**  output, int32* output_len)
{
#ifdef __MMI_DSM_JOYPAD__
	U32 joypad_value = 0;
	
	joypad_value = mr_nes_get_joypad_device_key();

	mr_nes_joypad.state = (joypad_value>>31)&0x0001;//(uint8)mr_get_joypad_status();
	
	joypad_value = joypad_value&0x0000ffff;
	
	mr_nes_joypad.k_up = joypad_value&0x0001;
	mr_nes_joypad.k_down= (joypad_value>>1)&0x0001;
	mr_nes_joypad.k_left = (joypad_value>>2)&0x0001;
	mr_nes_joypad.k_right= (joypad_value>>3)&0x0001;
	mr_nes_joypad.k_a = (joypad_value>>4)&0x0001;
	mr_nes_joypad.k_b = (joypad_value>>5)&0x0001;
	mr_nes_joypad.k_auto_a= (joypad_value>>6)&0x0001;
	mr_nes_joypad.k_auto_b= (joypad_value>>7)&0x0001;
	mr_nes_joypad.k_select = (joypad_value>>8)&0x0001;
	mr_nes_joypad.k_start= (joypad_value>>9)&0x0001;
	mr_nes_joypad.k_c= (joypad_value>>10)&0x0001; /*TOP L*/ 
	mr_nes_joypad.k_d= (joypad_value>>11)&0x0001;/*TOP R*/
	mr_nes_joypad.k_e= (joypad_value>>13)&0x0001;/*esc*/
	mr_nes_joypad.k_f= (joypad_value>>14)&0x0001;/*menu*/
	
	*output = (uint8*)&mr_nes_joypad;
	*output_len = sizeof(mr_nes_joypad);
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif	
}


int32 mr_nes_exit_game(void)
{
#ifdef __MMI_DSM_JOYPAD__
	EV_ExitGameSimu();
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}


int32 mr_nes_start_game(void)
{
#ifdef __MMI_DSM_JOYPAD__
	EV_EntryGameSimu();
	return MR_SUCCESS;
#else
	return MR_FAILED;
#endif
}

int32 MMIMRAPP_getKeyNum(int32 param)
{
	kal_prompt_trace(MOD_MMI,"MMIMRAPP_getKeyNum, %d", kbd_allkeys);

	return MR_PLAT_VALUE_BASE + kbd_allkeys;
}

int32 MMIMRAPP_getKeyState(int32 param)
{
#ifdef WIN32//huangsunbo 20111025 for modis
	return MR_PLAT_VALUE_BASE;
#else
	uint32 state = 0;

	if(param < kbd_allkeys)
	{
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
		HKBD_CTRL_KEY_STA_T keysta;
		DCL_HANDLE handle;
			
		keysta.key = param;
		handle = DclHKBD_Open(DCL_KBD, 0);
		DclHKBD_Control(handle,HKBD_CMD_GET_KEY_STATUS , (DCL_CTRL_DATA_T*)&keysta);
		state = keysta.sta;
#else		
		state = kbd_IsKeyPressed(keypad_Get_Data()->keypad[param]);
#endif		
	}

	kal_prompt_trace(MOD_MMI,"MMIMRAPP_getKeyState, %d, %d", param, state);

	return MR_PLAT_VALUE_BASE + state;
#endif	
}

int32 MMIMRAPP_nesActionInd(int32 param)

{
	kal_prompt_trace(MOD_MMI,"MMIMRAPP_nesActionInd");

	return MR_IGNORE;
}

#ifdef __MR_CFG_ENTRY_NES__
extern char  filenamebuf[(DSM_MAX_FILE_LEN+1)*2] ;//huangsunbo 20110608 for nes
void mr_nes_create_root_dir(void)
{
	int ret;
	char temp[DSM_MAX_FILE_LEN * ENCODING_LENGTH] = {0};

	memset(filenamebuf,0,sizeof(filenamebuf));
	
	sprintf(filenamebuf,"%c:\\%s",MR_DEFAULT_NES_DRV,MR_DEFAULT_NES_DIR);
	mr_fs_separator_vm_to_local((U8 *)filenamebuf);
	mr_str_gb_to_ucs2(filenamebuf,temp);

	ret = FS_GetAttributes((WCHAR *)temp);
	
	if(ret < 0||(!(ret&FS_ATTR_DIR)))
	{	
		FS_CreateDir((WCHAR *)temp);
#ifdef __MMI_DSM_HIDE_FOLDER__
		FS_SetAttributes((WCHAR *)temp,ret|FS_ATTR_HIDDEN);
#endif
	}
}
#endif


int32 mr_nes_get_root_dir(uint8**output,int32* output_len)
{
	if(output == NULL||output_len == NULL)
		return MR_FAILED;
	*output = (uint8 *)MR_DEFAULT_NES_DIR;
	*output_len = strlen(MR_DEFAULT_NES_DIR);
	return MR_SUCCESS;
}


int32 mr_nes_operate(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	if(*input == 0)/*START GAME*/
	{
		return mr_nes_start_game();
	}
	else if(*input == 1)/*EXIT GAME*/
	{
		return mr_nes_exit_game();
	}
	else if(*input == 2)/*GET JOYPAD KEY VALUE*/
	{
		return mr_nes_get_joypad_key(output,output_len);
	}
	return MR_FAILED;
}


///////////////////////////////////////////////////////////////////////////////////////

static void dsm_closekeytone(void)
{
	mdi_audio_stop_all();
	/* force all playing keypad tone off */
	//AudioStopReq(GetCurKeypadTone());// shawshank //huangsunbo 20110608 for 10a complier problem
	/* disable key pad tone */
	mmi_frm_kbd_set_tone_state(MMI_KEY_TONE_DISABLED);
}


#ifdef __MR_CFG_ENTRY_NES__
void DsmNes()
{
	if(dsmGameInit()!=MR_SUCCESS)
		return;
	
	if(dsmInit() != MR_SUCCESS)
		return;
	
	dsm_closekeytone();
	mr_registerAPP(NULL,0, 1);
	mr_registerAPP(NULL,0, 0);
 

	mr_registerAPP((uint8 *) mr_nesEntry, sizeof(mr_nesEntry), 9);
 
#ifndef __MMI_DSM_NES_APP_TCARD__
	#if defined( __MMI_MAINLCD_240X320__)
	mr_registerAPP((uint8 *) mr_nes324, sizeof(mr_nes324), 6);	
	#if !defined(MT6252)
	mr_registerAPP((uint8 *) mr_nes256, sizeof(mr_nes256), 5);
	mr_registerAPP((uint8 *) mr_nes326, sizeof(mr_nes326), 7);
	#endif
	#elif defined( __MMI_MAINLCD_176X220__)
	mr_registerAPP((uint8 *) mr_nes177, sizeof(mr_nes177), 6);	
	#if !defined(MT6252)
	mr_registerAPP((uint8 *) mr_nes176, sizeof(mr_nes176), 5);	
	mr_registerAPP((uint8 *) mr_nes178, sizeof(mr_nes178), 7);
	#endif
	#elif defined( __MMI_MAINLCD_240X400__)
	mr_registerAPP((uint8 *) mr_nes404, sizeof(mr_nes404), 6);	
	#if !defined(MT6252)
	mr_registerAPP((uint8 *) mr_nes400, sizeof(mr_nes400), 5);	
	mr_registerAPP((uint8 *) mr_nes406, sizeof(mr_nes406), 7);
	#endif
	#else
	mr_registerAPP(NULL,0, 5);
	mr_registerAPP(NULL,0, 6);
	mr_registerAPP(NULL,0, 7);
	#endif
#else
	mr_registerAPP(NULL,0, 5);
	mr_registerAPP(NULL,0, 6);
	mr_registerAPP(NULL,0, 7);
#endif


	//dsmEntry = "*J,1000,1,0,3,3,5,0,0,Nes Game,813105,nes.mrp";
	dsmEntry = "*J,1010,1,0,3,3,5,0,0,Nes Game,813105,nes.mrp";

	//dsmEntry = "*J,0011030231202000000";//270du
	//dsmEntry = "*J,0011030231102000000";//90du
	mr_app_entry_dsm_screen();
}


void HighlightDsmNes(void)
{
	ChangeLeftSoftkey(STR_GLOBAL_OK, IMG_GLOBAL_OK);
	ChangeRightSoftkey(STR_GLOBAL_BACK,IMG_GLOBAL_BACK);
	SetLeftSoftkeyFunction(DsmNes,KEY_EVENT_UP);
	SetRightSoftkeyFunction(GoBackHistory,KEY_EVENT_UP);   
	//huangsunbo 20110608
	SetCenterSoftkeyFunction(DsmNes, KEY_EVENT_UP);
	ChangeCenterSoftkey(NULL, IMG_GLOBAL_COMMON_CSK);
}

#endif


#endif

