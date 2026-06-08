#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "MessagesMiscell.h"
#include "med_utility.h"
#include "mdi_datatype.h"
#include "mdi_audio.h"

#ifdef __MMI_UCM__
#include "ucmgprot.h"
#endif

#include "mrp_include.h"

#ifdef __MMI_SKY_FONT__
#include "SKY_font_porting.h"
#endif

#include "GpioSrvGprot.h"


#if (MTK_VERSION <= 0x0852)
#include "gpioInc.h"
#endif

#if(MTK_VERSION <= 0x0812)
#include "App_datetime.h"
#endif

//#include "SimDetectionDef.h"

#include "imeisrvgprot.h"
#include "simctrlsrvgprot.h"
#if (__MR_CFG_VAR_MTK_VERSION__ < 0x11B1224)	
#include "TimerEvents.h"
#endif

#define ALIGN_ADDR(P)  (((uint32)(P) + (sizeof(double) - 1)) & (~(sizeof(double) - 1)))
#define ALIGN_LEN(P)   (((uint32)(P)) & (~(sizeof(double) - 1)))

extern U32 gnCurrentFont;
static char printfbuf[256];

#ifdef MMI_ON_HARDWARE_P
#ifndef DSM_USE_MED_BUFFER
    #if defined(__ARM9_MMU__)
        #pragma arm section rwdata = "NONCACHEDRW", zidata = "NONCACHEDZI"
        __align(4)     kal_uint8 dsm_buffer[__MR_CFG_VAR_VMMEM_SIZE__];
        #pragma arm section rwdata , zidata
    #else
        #pragma arm section zidata = "LARGEPOOL_ZI"
        __align(4)     kal_uint8 dsm_buffer[__MR_CFG_VAR_VMMEM_SIZE__];
        #pragma arm section zidata
    #endif
#endif
#else
	char dsm_buffer[__MR_CFG_VAR_VMMEM_SIZE__];
#endif


#ifdef __MED_IN_ASM__
#define med_ext_left_size applib_mem_ap_get_total_left_size
#define med_alloc_ext_mem(size) applib_mem_ap_alloc(APPLIB_MEM_AP_ID_MYTHROAD, size)
//#define med_alloc_ext_mem_cacheable(size) med_alloc_asm_mem_cacheable(APP_MYTHROAD, size)
#define med_free_ext_mem(ptr) applib_mem_ap_free(*ptr)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
函数名:void mr_drawBitmap(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h)
描  述:在屏幕上绘制一张bmp图片(图片数据是能直接打屏的)
参  数:bmp:bmp数据指针
       x,y,w,h:bmp图片的要画的位置x，y坐标，图片的大小w，h宽，高
返  回:
****************************************************************************/
void mr_drawBitmap(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h)
{		
	if(mr_app_get_state() == DSM_RUN)
	{
	/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
	#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
		if (mr_layer_zoom_resize(x, y, w, h) != MR_SUCCESS)
	#endif
		DSM_MTK_PREFIX(BLT_double_buffer)( x, y, x + w-1,  y + h-1 );
	}
}


/****************************************************************************
函数名:int32 mr_getScreenInfo(mr_screeninfo * screeninfo)
描  述:得到手机屏幕的参数
参  数:screeninfo:保存返回参数的数据结构
返  回:成功返回:MR_SUCCESS
       失败返回:MR_FAILED
****************************************************************************/
int32 mr_getScreenInfo(mr_screeninfo * screeninfo)
{
      if(screeninfo == NULL)
	  	return MR_FAILED;

/* reenable it, if have any problem, app need redo it */
/* __MMI_DSM_NEW_JSKY__ begin support screen rotate */
#if 1 //enable it when rotation needed
      screeninfo->height = UI_device_height;
      screeninfo->width = UI_device_width;
#else
      screeninfo->height = LCD_HEIGHT;
      screeninfo->width = LCD_WIDTH;
#endif
/* __MMI_DSM_NEW_JSKY__ end support screen rotate */
/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	if (mr_layer_zoom_is_enabled())
	{
		screeninfo->width = mr_layer_zoom_get_width();
		screeninfo->height = mr_layer_zoom_get_height();
	}
#endif

      screeninfo->bit = 16;
      return MR_SUCCESS;
}


/****************************************************************************
函数名:const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height)
描  述:取得指定汉字得点阵信息
参  数:ch:汉字得unicode编码
       fontSize:要得到汉字得大小
       with,height:汉字得宽高(以象素为单位)
返  回:取得汉字得点阵信息
****************************************************************************/
const char *mr_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height)
{	
	stFontAttribute	 MMI_medium_font = { 0,0,0,MEDIUM_FONT,0,1 };
	stFontAttribute	 MMI_small_font = { 0,0,0,SMALL_FONT,0,1 };
	stFontAttribute	 MMI_large_font = { 0,0,0,LARGE_FONT,0,1 };
#if 1
	if (fontSize == MR_FONT_BIG) 
		DSM_MTK_PREFIX(set_font)(&MMI_large_font);
	else if(fontSize == MR_FONT_MEDIUM)
		DSM_MTK_PREFIX(set_font)(&MMI_medium_font);
	else 
		DSM_MTK_PREFIX(set_font)(&MMI_small_font);
#else
	DSM_MTK_PREFIX(set_font)(&MMI_medium_font);
#endif

#ifdef __MMI_VECTOR_FONT_SUPPORT__ 
	if (ch == ' ')
	{
		*width = 10;
	}
	else
	{
		*width = gui_get_character_width(ch);
	}
#else
	*width = gui_get_character_width(ch);
#endif	
	*height = gui_get_character_height();

	return (char *)-1;
}


#define R_OF_RGB565(p)      ((p) >> 11)
#define G_OF_RGB565(p)      (((p) >> 5) & 0x3f)
#define B_OF_RGB565(p)      ((p) & 0x1f)
#ifndef INLINE
#ifdef MMI_ON_HARDWARE_P
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

static INLINE U8 rgb565_to_rgb888_b(uint16 p)
{
	U8 c;
	c = B_OF_RGB565(p);
	return (c << 3) | (c & 0x7); 
}

static INLINE U8 rgb565_to_rgb888_g(uint16 p)
{
	U8 c;
	c = G_OF_RGB565(p);
	return ((c << 2) | (c & 0x3));
}

static INLINE U8 rgb565_to_rgb888_r(uint16 p)
{
	U8  c;
	c = R_OF_RGB565(p);
	return ((c << 3) | (c & 0x7)) ;
}


extern U32 mmi_fe_show_string_baseline(S32 x, S32 y, U8 *String, S32 baseline, U8 default_direction);
/*
 * SDK中的原型为void mr_platDrawCharReal(uint16 ch, int16 x, int16 y, uint16 color)
 * 以前移植层的原型为void mr_platDrawCharReal(uint16 ch, int32 x, int32 y, uint16 color)
 */
void mr_platDrawCharReal(uint16 ch, int16 x, int16 y, uint16 color)
{
	U8 r,g,b;
	U32 color888;
#ifdef __MMI_VECTOR_FONT_SUPPORT__	
	U16 tmp[2];
	S32 height, ascent, descent;
	S32 cWidth,cHeight;
#endif	

	gui_set_text_color(gui_color(rgb565_to_rgb888_r(color), rgb565_to_rgb888_g(color), rgb565_to_rgb888_b(color)));

#ifdef __MMI_VECTOR_FONT_SUPPORT__
	tmp[0] =ch;
	tmp[1] =0;
	
	#ifdef __MMI_MAINLCD_480X800__
	y = y-2;
	#endif
	
	#ifndef __MR_CFG_FEATURE_OVERSEA__
		mmi_fe_get_char_info_of_all_lang(UI_font->size, &height, &ascent, &descent);
		mmi_fe_show_string_baseline(x, y, (U8*)tmp, ascent - 3, 0);
	#else
		if (r2lMMIFlag)
		{
			mmi_fe_get_char_info_of_all_lang(UI_font->size, &height, &ascent, &descent);
			cWidth = gui_get_character_width(ch);//todo
			if( cWidth < 4 ){
				cWidth = 4;
			}
				
			x +=cWidth;
			mmi_fe_show_string_baseline(x, y, (U8*)tmp, ascent - 3, 0);
		}
		else
		{
			mmi_fe_get_char_info_of_all_lang(UI_font->size, &height, &ascent, &descent);
			mmi_fe_show_string_baseline(x, y, (U8*)tmp, ascent - 3, 0);
		}

	#endif
#else
	gui_print_character_at_xy(ch, x, y);
#endif
}


/****************************************************************************
函数名:void mr_stopShake(void)
描  述:关闭振动器
参  数:无
返  回:无
****************************************************************************/
int32 mr_stopShake(void)
{
      srv_vibrator_post_req(VIB_PATTERN_1, PLAY_STOP);
	return MR_SUCCESS;
}


/****************************************************************************
函数名:void stop_vibrator(void)
描  述:关闭振动器
参  数:无
返  回:无
****************************************************************************/
static void stop_vibrator(void)
{
      srv_vibrator_post_req(VIB_PATTERN_1, PLAY_STOP);
}


/****************************************************************************
函数名:mr_startShake(int32 ms)
描  述:启动震动器
参  数:ms:要震动持续得时间(毫秒)
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_startShake(int32 ms)
{
	StopTimer(DSM_VIBRATOR_TIMER);
      srv_vibrator_post_req(VIB_PATTERN_1, PLAY_STOP);
      srv_vibrator_post_req(VIB_PATTERN_1, PLAY_REPEAT);
	StartTimer(DSM_VIBRATOR_TIMER, (U32) ms, stop_vibrator);	
	return MR_SUCCESS;
}

static BOOL is_playing = FALSE;

static void play_callback(mdi_result result)
{
	mr_trace("play_callback, result = %d", result);
	
	switch (result)
	{
		case MDI_AUDIO_END_OF_FILE:
		case MDI_AUDIO_BAD_FORMAT:
		case MDI_AUDIO_TERMINATED:
		default:
			is_playing = FALSE;
			break;
	}
}


/****************************************************************************
函数名:int32 mr_playSound(int type, const void* data, uint32 dataLen, int32 loop)
描  述:播放音乐
参  数:type:音乐得类型
           MR_SOUND_MIDI,midi音乐
           MR_SOUND_WAV, wav音乐
           MR_SOUND_MP3  mp3音乐
       data:音乐数据buf得指针
       dataLen:音乐数据得长度
       loop:是否循环播放的标志
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_playSound(int type, const void* data, uint32 dataLen, int32 loop)
{
	U8 format;
	U8 playstyle;
	mdi_result result;
	
	if (!srv_prof_if_can_ring())
		return MR_SUCCESS;

	if(!mdi_audio_mma_is_idle() )
	{
		//可以在这里加上震动处理
		mr_startShake(1000);
		return MR_SUCCESS;
	}

	//if(IsBacklightOn()==MMI_FALSE)
    //{
	//	TurnOnBacklight(1);	
	//}		

	if (MR_SOUND_MIDI == type)
		format = MDI_FORMAT_SMF;
	else if (MR_SOUND_MP3 == type)
		format = MDI_FORMAT_DAF;
	else if (MR_SOUND_WAV == type)
		format = MDI_FORMAT_WAV;
	else if(MR_SOUND_PCM == type)
		format = MDI_FORMAT_PCM_8K;
	else if(MR_SOUND_M4A == type)
		format = MDI_FORMAT_M4A;
	else if(MR_SOUND_AMR == type)
		format = MDI_FORMAT_AMR;
	else if(MR_SOUND_AMR_WB == type)
		format = MDI_FORMAT_AMR_WB;
	else
		format = MDI_NO_OF_FORMAT;
	
	if (loop) 
		playstyle = DEVICE_AUDIO_PLAY_INFINITE;
	else
		playstyle = DEVICE_AUDIO_PLAY_ONCE;

#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x11A1112)
	result = mdi_audio_play_string_with_vol_path(
				(void*)data,
				(U32)dataLen,
				format,
				playstyle,
				play_callback,
				NULL,
				MDI_AUD_VOL_EX(mr_media_audio_get_vol()),
				MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
#else
	result = mdi_audio_play_string_with_vol_path(
				(void*)data,
				(U32)dataLen,
				format,
				playstyle,
				NULL,
				play_callback,
				MDI_AUD_VOL_EX(mr_media_audio_get_vol()),
				MDI_AUD_PTH_EX(MDI_DEVICE_SPEAKER2));
#endif

	 if (result == MDI_AUDIO_SUCCESS)
	 {
	 	is_playing = TRUE;
	 	return MR_SUCCESS;
	 }
	 else
	 {
	 	is_playing = FALSE;
	 	return MR_FAILED;
	 }
}


/****************************************************************************
函数名:int32 mr_stopSound (int type)
描  述:停止音乐播放
参  数:type:要停止播放得音乐类型
         MR_SOUND_MIDI,midi音乐
         MR_SOUND_WAV, wav音乐
         MR_SOUND_MP3  mp3音乐
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_stopSound (int type)
{
	mdi_result result = MDI_AUDIO_SUCCESS;	
	mr_trace("mr_stopSound, is_playing = %d", is_playing);
	if (!is_playing)
	{
		return MR_SUCCESS;
	}
	result = mdi_audio_stop_string();
	if (result == MDI_AUDIO_SUCCESS)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


/****************************************************************************
函数名:int32 mr_timerStart(uint16 t)
描  述:启动dsm定时器
参  数:t:定时器溢出时间(ms)
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_timerStart(uint16 t)
{
	//StartNonAlignTimer(DSM_TIMER, t, mr_callBackFun);
	//StartTimer(DSM_TIMER, (U32) t, mr_callBackFun);
	StartNonAlignTimer(DSM_TIMER, t, (FuncPtr)mr_timer);
	return MR_SUCCESS;
}


/****************************************************************************
函数名:int32 mr_timerStop(void)
描  述:停止dsm定时器
参  数:无
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_timerStop(void)
{
      StopTimer(DSM_TIMER);
      return MR_SUCCESS;
}


/****************************************************************************
函数名:void mr_printf(const char *format,...)
描  述:打印trace信息
参  数:format:要打印信息的格式
       ...要被打印的信息
返  回:无
****************************************************************************/
void mr_printf(const char *format,...)
{
	va_list params;
	
	memset(printfbuf, 0, sizeof(printfbuf));
	va_start(params, format);
	//vsprintf(printfbuf, format, params);
	_vsnprintf(printfbuf,250,format,params);
#ifdef MMI_ON_HARDWARE_P	
	kal_prompt_trace(MOD_MMI,"VM_LOG:%s",printfbuf);
#endif
	va_end (params);
}

 
/****************************************************************************
函数名:int32 mr_mem_get (char** mem_base, uint32* mem_len)
描  述:为dsm申请运行需要的内存
参  数:mem_base:dsm运行要内存的指针的指针
       mem_len :内存的大小
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_mem_get (char** mem_base, uint32* mem_len)
{
#ifdef __MMI_DSM_WEATHER_APP__
	extern int weather_start;
#endif

#ifndef MMI_ON_HARDWARE_P
	*mem_base = dsm_buffer;
	*mem_len = __MR_CFG_VAR_VMMEM_SIZE__;
	return MR_SUCCESS;
#else
#ifdef DSM_USE_MED_BUFFER
	int32 mem_size = __MR_CFG_VAR_VMMEM_SIZE__;

	mr_printf("mr_mem_get: %d", *mem_len);
	if(*mem_len > 0 && *mem_len < 650*1024){ //这里的650k是系统默认的大小
		mem_size = *mem_len;
	}

#ifdef __MMI_DSM_WEATHER_APP__
	if(weather_start)
	{
		mem_size = 350*1024;
	}
#endif

#ifdef MMI_ON_HARDWARE_P	
	kal_prompt_trace(MOD_MMI,"mr_mem_get, med_ext_left_size[%d]KB,size[%d]KB",med_ext_left_size()/1024, mem_size);
#endif

#ifndef __MED_IN_ASM__
	if(med_ext_left_size() < (mem_size + 150 * 1024))	// 保留一些多媒体内存
	{
		kal_prompt_trace(MOD_MMI,"mr_mem_get, malloc failed");
		return MR_FAILED;
	}
#endif

#if 0 // defined(__ARM9_MMU__)
       *mem_base = (char *)med_alloc_ext_mem_cacheable(mem_size);
#else
       *mem_base = (char *)med_alloc_ext_mem(mem_size);
#endif

	if(*mem_base == NULL)
	{
		kal_prompt_trace(MOD_MMI,"mr_mem_get, get mem failed!!!");
		MMI_ASSERT(0);
		return MR_FAILED;
	}
	else
	{
		*mem_len = mem_size;
		memset(*mem_base,0,*mem_len);
		kal_prompt_trace(MOD_MMI,"mr_mem_get, get mem suc!!! %x", *mem_base);
		return MR_SUCCESS;
	}
#else
	int32 len;	
	*mem_base = (char *)ALIGN_ADDR(dsm_buffer);
	len = (int32)*mem_base -(int32)dsm_buffer;
	*mem_len = ALIGN_LEN(__MR_CFG_VAR_VMMEM_SIZE__ -len);
	memset(*mem_base,0,*mem_len);
	return MR_SUCCESS;
#endif	
#endif
}


/****************************************************************************
函数名:int32 mr_mem_free (char* mem, uint32 mem_len)
描  述:dsm退出后释放由mr_mem_get申请到的内存
参  数:meme   :要释放的内存的指针
       mem_len:要释放的内存大小
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_mem_free (char* mem, uint32 mem_len)
{
#ifndef MMI_ON_HARDWARE_P
	return MR_SUCCESS;
#else
#ifdef DSM_USE_MED_BUFFER
	//media_free_ext_buffer(MOD_MMI, (void * *)(&mem));
	kal_prompt_trace(MOD_MMI,"mr_mem_free!!! %x", mem);
	med_free_ext_mem((void * *)(&mem));
	return MR_SUCCESS;
#else
	return MR_SUCCESS;
#endif
#endif
}


/****************************************************************************
函数名:int32 mr_exit(void)
描  述:dsm退出通知主机
参  数:无
返  回:无
****************************************************************************/
int32 mr_exit(void)
{
	mr_trace("** mr_exit **");
	StopTimer(DSM_TIMER_MAX);
	
	if(mr_app_get_state() != DSM_STOP){
		if(mr_app_get_state() != DSM_BACK_RUN) 	ClearAllKeyHandler();
		mr_app_set_state(DSM_RUN); //将状态设置成DSM_RUN,主要是为了能强制退出
		StartTimer(DSM_TIMER_MAX, 50, mr_app_force_exit_dsm_screen);
	}
	
	return MR_SUCCESS;
}


/****************************************************************************
函数名:uint32 mr_getTime(void)
描  述:获取系统时间，单位毫秒
参  数:无
返  回:当前系统时间
****************************************************************************/
uint32 mr_getTime(void)
{
	kal_uint32 time = 0;
	uint32 return_t;
	kal_get_time(&time);
	return_t = (time* 0.004615 + 0.5)*1000;
	return return_t;
}


/****************************************************************************
函数名:int32 mr_getDatetime(mr_datetime* datetime)
描  述:取得日期信息
参  数:datetime:保存取得的日期信息
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_getDatetime(mr_datetime* datetime)
{
	applib_time_struct time;

	MMI_ASSERT(datetime != NULL);
	
	applib_dt_get_rtc_time(&time);

	datetime->year = time.nYear;
	datetime->month = time.nMonth;
	datetime->day = time.nDay;
	datetime->hour = time.nHour;
	datetime->minute = time.nMin;
	datetime->second = time.nSec;

	return MR_SUCCESS;
}


/****************************************************************************
函数名:void mr_call(char * pNumber)
描  述:呼叫一个号码
参  数:pNumber:被叫号码
返  回:无
****************************************************************************/
static void do_call(char* pNumber)
{
	S8 OutNumber [(MAX_DIGITS+1)*ENCODING_LENGTH] = {0};
       int len = 0;
       int32 sim_id;
	srv_ucm_call_type_enum call_type;
	mmi_ucm_make_call_para_struct makecall_para;   

       len = strlen(pNumber) > MAX_DIGITS?MAX_DIGITS:strlen(pNumber);
	   
	mmi_asc_n_to_ucs2((S8 *) OutNumber,( S8 *) pNumber,len);
	
	sim_id = mr_sim_get_active();
	
	if (sim_id == DSM_SLAVE_SIM)
	{
		call_type = SRV_UCM_VOICE_CALL_TYPE_SIM2;
	}
	else if (sim_id == DSM_THIRD_SIM)
	{
		call_type = 0x0100;	// SRV_UCM_VOICE_CALL_TYPE_SIM3	// 直接用数字是为了和以前的版本兼容
	}
	else if (sim_id == DSM_FOURTH_SIM)
	{
		call_type = 0x0800;	// SRV_UCM_VOICE_CALL_TYPE_SIM4	// 直接用数字是为了和以前的版本兼容
	}
	else
	{
		call_type = SRV_UCM_VOICE_CALL_TYPE;
	}
	
	mmi_ucm_init_call_para(&makecall_para);
	makecall_para.dial_type = call_type;
	makecall_para.ucs2_num_uri = (U16*)OutNumber;
	mmi_ucm_call_launch(0, &makecall_para);	
}

void mr_call(char* pNumber)
{
	static S8 OutNumber[(MAX_DIGITS+1)];

	memset(OutNumber, 0, sizeof(OutNumber));

	strncpy(OutNumber, pNumber, MAX_DIGITS);

	gui_start_timer_ex(100, do_call, OutNumber);
}


/****************************************************************************
函数名:int32 mr_sleep(uint32 ms)
描  述:使dsm sleep(应使dsm运行的线程睡眠)
参  数:ms(sleep的时间，单位ms)
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_sleep(uint32 ms)
{
   #ifdef MMI_ON_HARDWARE_P
	kal_sleep_task(kal_milli_secs_to_ticks(ms));
   #endif
	return MR_SUCCESS;
}


/****************************************************************************
函数名:int32 mr_getUserInfo(mr_userinfo* info)
描  述:取得手机的相关信息
参  数:info:保存取得手机信息的数据结构
返  回:MR_SUCCESS,MR_FAILED
****************************************************************************/
int32 mr_getUserInfo(mr_userinfo* info)
{	
	mmi_sim_enum sim;
	CHAR tmp[SRV_MAX_IMSI_LEN + 1];
	
	if (info == NULL)
		return MR_FAILED;

	sim = mr_sim_active_id_dsm2mmi();
	
	memset(info, 0, sizeof(mr_userinfo));
	memset(tmp, 0, sizeof(tmp));

	srv_imei_get_imei(sim, (CHAR*)info->IMEI, sizeof(info->IMEI));

	srv_sim_ctrl_get_imsi(sim, tmp, sizeof(tmp));
	memcpy(info->IMSI, tmp + 1, sizeof(info->IMSI));
	
	strncpy(info->manufactory, __MR_CFG_VAR_FACTORYID__, 7);
	strncpy(info->type, __MR_CFG_VAR_HANDSETID__, 7);
	info->ver = 101000000 + (__MR_CFG_VAR_PLAT_VER__ * 10000) + __MR_CFG_VAR_FAE_VER__;

#ifdef __MMI_DUAL_SIM_MASTER__
	info->ver += 1000;
#endif

#ifdef MMI_ON_HARDWARE_P
	kal_prompt_trace(MOD_MMI,"imei = %s",info->IMEI);
	kal_prompt_trace(MOD_MMI,"imsi = %s",info->IMSI);
	kal_prompt_trace(MOD_MMI,"factory = %s",info->manufactory);
	kal_prompt_trace(MOD_MMI,"type = %s",info->type);
	kal_prompt_trace(MOD_MMI,"ver = %d",info->ver);
#endif

	memset(info->spare, 0, sizeof(info->spare) );
       return MR_SUCCESS;
}


/**
 * \brief cache 同步函数
 *	在老的移植版本中会区分是否是35的平台再使用不同的lib库，导致库文件较多比较难维护。
 * 	而35和非35的主要区别在于这个函数，就是CACHE同步
 */
int32 mr_cacheSync(void* addr, int32 len)
{
	mr_trace("mr_cacheSync: %x, %d", addr, len);
	
#ifndef WIN32	
#if defined(__ARM9_MMU__)
	clean_arm9_dcache((kal_uint32)addr, (kal_uint32)len);
	invalidate_arm9_icache((kal_uint32)addr, (kal_uint32)len);
#endif

#if defined(__ARM11_MMU__)
	clean_dcache((kal_uint32)addr, (kal_uint32)len);
	invalidate_icache((kal_uint32)addr, (kal_uint32)len);
#endif
#endif

	return MR_SUCCESS;
}


#endif

