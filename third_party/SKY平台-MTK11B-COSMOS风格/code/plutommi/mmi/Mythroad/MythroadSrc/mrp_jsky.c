#ifdef __MMI_DSM_NEW__
#ifdef __MMI_DSM_NEW_JSKY__
#include "MMI_Include.h"
#include "GlobalDefs.h"
#include "fmt_def.h"
#include "filemgr.h"
#include "SettingProfile.h"
#include "FileManagerDef.h"
#include "FileManagerGProt.h"
#include "mrp_include.h"
#include "reg_base.h"
#include "keypad_hw.h"
#include "keypad_sw.h" 
#include "kbd_table.h"
#include "fmt_def.h"
#include "soc_consts.h"
#include "gui.h"
#include "mrporting.h"
#include "mrp_jsky.h"
#include "mmidsmapi.h"

#define KEYS_COUNT  kbd_allkeys
extern stFontAttribute		MMI_small_font;
extern stFontAttribute		MMI_medium_font;
extern stFontAttribute		MMI_large_font;
static int32 g_jsky_logicKeyCell[KEYS_COUNT];

static uint16 *g_jsky_low_key;
static uint16 *g_jsky_mid_key;
static uint16 *g_jsky_hi_key;
static uint16 *g_jsky_hi1_key;
static uint16 *g_jsky_hi2_key;

static int32 g_bInited = 0;

static void setFontAttr(int32 font);
static U32 mr_jCharNumInWidth(UI_character_type *String, U32 width);
static void mr_jdrawText_n(UI_character_type *str,int32 x, int32 y,int32 n,uint8 r, uint8 g, uint8 b,int32 fontSize);
extern const keypad_struct *keypad_Get_Data(void); 



static kal_int8 mr_jsky_map_device_key(kal_uint8 device_key)
{
#ifdef MMI_ON_HARDWARE_P
	kal_uint8 i = 0;
	
	for(i=0; i<kbd_allkeys; i++)
	{
		if(device_key == keypad_Get_Data()->keypad[i])
			return i;
	}
	return -1;
#else
	return -1;
#endif
}

//keybd.c中定义
extern int32 SKY_get_key_size(void);
extern int16 SKY_get_mmi_key_code(U8 device_key_code);
extern int16 SKY_get_device_key_code(U8 index);
//mrp_application.c中定义
extern int32 mr_app_translate_keycode(U16 keycode);

//mmi key to sky key
int32 mr_jsky_translate_keycode(U16 keycode)
{
	if(keycode == KEY_POWER)
	{
		return MR_KEY_POWER;
	}
	else
	{
		return mr_app_translate_keycode(keycode);
	}
}

int32 dsmKeyScanInit()
{
	int i;
	int32 keysize = 0;
	int16 keycode = 0;
	kal_int8 index = 0;
	g_jsky_low_key = (uint16*)(KP_LOW_KEY);
	g_jsky_mid_key = (uint16*)(KP_MID_KEY);
	g_jsky_hi_key  = (uint16*)(KP_MID1_KEY);
#ifdef DRV_KBD_64KEYS_ABOVE	
	g_jsky_hi1_key = (uint16*)(KP_MID2_KEY);
#endif
	g_jsky_hi2_key = (uint16*)(KP_HI_KEY);
	
	g_bInited = 1;

	for(i=0;i<KEYS_COUNT;i++)
	{
		g_jsky_logicKeyCell[i] = MR_KEY_NONE;	
	}

	keysize = SKY_get_key_size();
	for(i = 0; i < keysize; i++)
	{
		keycode = SKY_get_device_key_code(i);
		index = mr_jsky_map_device_key(keycode); 
		if(index >= 0 && index < KEYS_COUNT)
		{
			g_jsky_logicKeyCell[index] = mr_jsky_translate_keycode(SKY_get_mmi_key_code(keycode));
		}

//		mr_trace("key_init,%d,%d,%d,%d",index,keycode,SKY_get_mmi_key_code(keycode),g_jsky_logicKeyCell[index]);
	}
	
	
	return MR_SUCCESS;
}

/*判断某个索引值的按键是否被按下*/
int32 dsmKeyScanKeyPressed(uint32 keyIndex)
{
	if (keyIndex<16)
	{
		 if (((*g_jsky_low_key) & (1<<keyIndex)) == 0)
		 {
			return 1;	
		 }
	}
	else if (keyIndex<32)
	{
		if (((*g_jsky_mid_key) & (1<<(keyIndex-16))) == 0)
		{
			return 1;
		}
	}
	else if (keyIndex<48)
	{
		if (((*g_jsky_hi_key) & (1<<(keyIndex - 32))) == 0)
		{
			return 1;
		}
	}
	else if (keyIndex<64)
	{
		if (((*g_jsky_hi1_key) & (1<<(keyIndex - 48))) == 0)
		{
			return 1;
		}
	}else
	{
		if (((*g_jsky_hi2_key) & (1<<(keyIndex-64))) ==0)
		{
			return 1;
		}
	}

	return 0;
}


/*得到目前正被按下的按键的索引值*/
int32 dsmKeyScanGetPressedKeyIndex()
{
	int i = 0,j=0;
	int16 reg16 = 0;
	int32 pressedKeyIndex = 0; 
	
	for(i=0;i<5;i++)
	{
		if (i==0)
		{
			reg16 = *g_jsky_low_key;
		}else
		if (i==1)
		{
			reg16 = *g_jsky_mid_key;
		}else
		if (i==2)
		{
			reg16 = *g_jsky_hi_key;	
		}else
		if (i==3)
		{
			reg16 = *g_jsky_hi1_key;
		}else
		if (i==4)
		{
			reg16 = *g_jsky_hi2_key;
		}

		for(j=0;j<16;j++)
		{
			if ((reg16 & (1<<j))==0)
			{
				pressedKeyIndex = 16*i + j;
				return pressedKeyIndex;
			}
			
		}
	}
	

	return MR_FAILED;			
}

const int32* dsmKeyScanGetLogicKeyMap(int32 *count)
{
	if (count)  *count = KEYS_COUNT;
	
	
	return &g_jsky_logicKeyCell[0];
}


int32 mr_jsky_function(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	static uint32 sky_function = 1;
	sky_function = sky_jvm_getVersion();
	*output = (uint8*)&sky_function;
	*output_len = sizeof(uint32);
      return MR_SUCCESS;
}

int32 mr_jsky_init(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	dsmKeyScanInit();
	sky_jvm_init(input,input_len,output,output_len);			
	return MR_SUCCESS;
}

int32 mr_jsky_vector_get_bitmap(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int ret = MR_FAILED;
	
	VectorCharReq *vCharReq = (VectorCharReq*)input;
	int width,height;
	char *pBitmap;

	/*zhukai:25 平台暂时先不要考虑矢量字体*/
	if(0)//SKY_font_get_loaded_enable())
	{
		//ret = SKY_getVectorBitmap(vCharReq->unicode,vCharReq->fontSize,&width,&height,bmpVChar);  TODO: open me later
		*output = (uint8*)bmpVChar;
		 *output_len = 36*36*2;
	}                
			    	
	return ret==1?MR_SUCCESS:MR_FAILED;
	
}
int32 mr_jsky_vector_font_env(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	/*zhukai: 25平台暂时先不要考虑矢量字体*/
	if (0)//SKY_font_get_loaded_enable())
	{
		isSkyVectorFontLoad = 1;
	}
	else
	{
		isSkyVectorFontLoad = 0;
	}
	*output = (uint8*)&isSkyVectorFontLoad;
	*output_len = sizeof(int32);				
	return MR_SUCCESS;
}

int32 mr_jsky_file_trucate(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	FTruncateReq *trunReq = (FTruncateReq *)input;

	if (FS_Seek(trunReq->hFile,trunReq->size,FS_FILE_BEGIN)>=0)
	{
	     if (FS_Truncate(trunReq->hFile) == FS_NO_ERROR)
		 {
		 	return MR_SUCCESS;
		 }
	}

	return MR_FAILED;
}
int32 mr_jsky_file_attribute(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return mr_attributes((const char*)input);
}
int32 mr_jsky_file_foldersize(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return mr_getfoldersize((const char*)input);		
}
int32 mr_jsky_file_setattribute(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	SetAttributeReq *trunReq = (SetAttributeReq*)input;

	return mr_setattributes(trunReq->fileName,trunReq->attr);
}

int32 mr_jsky_file_keypressed(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 keyIndex;

	keyIndex = *(int32*)input;
	return dsmKeyScanKeyPressed(keyIndex);
}

int32 mr_jsky_file_get_pressed_keyindex(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 keyIndex;

	keyIndex = *(int32*)input;
	return dsmKeyScanKeyPressed(keyIndex);
}

int32 mr_jsky_file_get_logickey_map(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32* logicKeyMap;
	int32 mapLen;

	*output = (uint8*)dsmKeyScanGetLogicKeyMap(&mapLen);
	//开了会当机,上层没有传下这个参数.
	//*output_len = sizeof(uint8*);
	return mapLen;
}

int32 mr_jsky_return_idle(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	return MR_IGNORE;
}

int32 mr_jsky_puch_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	gdi_layer_push_clip();
	return MR_SUCCESS;
}

int32 mr_jsky_pop_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	gdi_layer_pop_clip();
	return MR_SUCCESS;
}

int32 mr_jsky_set_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	skyclip* temp = (skyclip*)input;
	if(temp!= NULL)
	{
		gdi_layer_set_clip(temp->x1,temp->y1,temp->x2,temp->y2);

		return MR_SUCCESS;
	}
	else 
	{
		return MR_FAILED;
	}
}

int32 mr_jsky_get_clip(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	skyclip   temp;
	if((output != NULL)&&(output_len != NULL))
	{
		gdi_layer_get_clip(&temp.x1,&temp.y1,&temp.x2,&temp.y2);
		
		*output = (uint8*)&temp;
		*output_len = sizeof(skyclip*);
		
		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}

static UI_character_type  doubleChar[3];
int32 mr_jsky_get_string_widhgt(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	TextWidthHeightReq *req;
	TextWidthHeightResp *resp;

	req = (TextWidthHeightReq *)input;
	resp = (TextWidthHeightResp *)output;

	if (!req || !resp) return MR_FAILED;

	setFontAttr(req->fontSize);

	if (req->nCharCount>1 || req->nCharCount==0 )
	{
		//gui_measure_string_n((UI_character_type*)req->pStr,(S32)req->nCharCount,(S32*)&resp->width,(S32*)&resp->height);
		Get_StringWidthHeight_variant((U8*)req->pStr, 0, (S32)req->nCharCount, (S32*)&resp->width, (S32*)&resp->height, 1 << 16, 0, 1);
	}else
	if (req->nCharCount==1)
	{
		S32 w1,w2,h;
		
		doubleChar[0] = *(UI_character_type*)req->pStr;
		doubleChar[1] = doubleChar[0];
		doubleChar[2] = 0;
			
		//gui_measure_string_n((UI_character_type*)doubleChar,2,(S32*)&w2,(S32*)&h);
		//gui_measure_string_n((UI_character_type*)doubleChar,1,(S32*)&w1,(S32*)&h);
		Get_StringWidthHeight_variant((U8*)doubleChar, 0, (S32)2, (S32*)&w2, (S32*)&h, 1 << 16, 0, 1);
		Get_StringWidthHeight_variant((U8*)doubleChar, 0, (S32)1, (S32*)&w1, (S32*)&h, 1 << 16, 0, 1);

		resp->width = w2 - w1;
		resp->height = h;
		
	}
	

	return MR_SUCCESS;
}

int32 mr_jsky_charnum_inwidth(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	CharNumInWidthReq *req;
	S32 strHeight = 0, strWidth = 0;
	int32 nCount;
	
	req = (CharNumInWidthReq *)input;

	if (!req) return MR_FAILED;

	if(req->pStr == NULL) return 0;

	if (req->fontSize==MR_FONT_SMALL)
	{
		gui_set_font(&MMI_small_font);
	}
	else	if (req->fontSize==MR_FONT_MEDIUM)
	{
		gui_set_font(&MMI_medium_font);
	}
	else	if (req->fontSize==MR_FONT_BIG)
	{
		gui_set_font(&MMI_large_font);
	}
	else
	{
		gui_set_font(&MMI_medium_font);
	}

	nCount =  Get_StringWidthHeight_variant((U8*)req->pStr, 0, req->n, &strWidth, &strHeight, req->width, 0, 1);

      if (nCount==req->n)  return nCount;

	nCount++;
       Get_StringWidthHeight_variant((U8*)req->pStr,0,nCount,&strWidth,&strHeight,1<<16,0,1);
	while(strWidth<req->width)
	{
		nCount++;
		Get_StringWidthHeight_variant((U8*)req->pStr,0,nCount,&strWidth,&strHeight,1<<16,0,1);
	}

	if (strWidth>req->width) nCount--;
	
	return nCount;	
}

int32 mr_jsky_draw_text(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	DrawTextReq *req;

	req = (DrawTextReq*)input;

	if (!req) return MR_FAILED;

	mr_jdrawText_n((UI_character_type*)req->pStr,req->x,req->y,req->n,req->r,req->g,req->b,req->fontSize);
	
	return MR_SUCCESS;
}

static U32 mr_jCharNumInWidth(UI_character_type *String, U32 width)
{
	U32 nwidth = 0, chwidth, chheight;
	U16 ch;
	int32 firstChar;
	U32 nChar = 0;
	U32 gap = 1;
		
	if (String == NULL)
	{
		return 0;
	}

	ch = *String++;
	firstChar = 1;

	do
	{
		gui_measure_character(ch,&chwidth,&chheight);
		nwidth+= chwidth;
		if (!firstChar) 
		{
			nwidth += gap;
			firstChar = 0;
		}
		nChar++;
		ch = *String++;
	} while (nwidth <= width && ch != 0 && ch != 0x0A && ch != 0x0D);

	if (nwidth>width) nChar--;

	return nChar;
}

extern void mmi_fe_get_char_info_of_all_lang(U8 size, S32* pheight, S32* pascent, S32* pdescent);
extern U32  mmi_fe_show_string_n_baseline(U32 x_unsigned,U32 y_unsigned,U8 *String,S32 Len,S32 baseline);

static void mr_jdrawText_n(UI_character_type *str,int32 x, int32 y,int32 n,uint8 r, uint8 g, uint8 b,int32 fontSize)
{
#if (MTK_VERSION > 0x09B0952)
	S32 baseline = 0;
	S32 height, ascent, descent;

	setFontAttr(fontSize);

	if (r==0 && 
		g==0 && 
		b>247)
	{
		b = 247;
	}
	
	gui_set_text_color(gui_color(r,g,b));
	mmi_fe_get_char_info_of_all_lang(UI_font->size, &height, &ascent, &descent);
	baseline = ascent - ascent/4;
	mmi_fe_show_string_n_baseline((U32)x, (U32)y, (U8*)str, n, baseline);
#else
	if (fontSize==MR_FONT_SMALL)
	{
		gui_set_font(&MMI_small_font);
	}
	else	if (fontSize==MR_FONT_MEDIUM)
	{
		gui_set_font(&MMI_medium_font);
	}
	else	if (fontSize==MR_FONT_BIG)
	{
		gui_set_font(&MMI_large_font);
	}
	else
	{
		gui_set_font(&MMI_medium_font);
	}

	if (r==0 && 
		g==0 && 
		b>247)
	{
		b = 247;
	}
	
	gui_set_text_color(gui_color(r,g,b));
	gui_move_text_cursor(x,y);
	gui_print_text_n(str,n);

#endif
}



enum{
	MR_FONT_EFF_BASE = 0x00000004,

    MR_FONT_BOLD = MR_FONT_EFF_BASE,
    MR_FONT_ITALIC = (MR_FONT_EFF_BASE << 1),
    MR_FONT_OBLIQUE = (MR_FONT_EFF_BASE << 2),
    MR_FONT_UNDER_LINE = (MR_FONT_EFF_BASE << 3),
    MR_FONT_SMALL_CAP = (MR_FONT_EFF_BASE << 4)
};

static void setFontAttr(int32 font)
{
	stFontAttribute uiFont = {	0,0,0,MEDIUM_FONT,0,1};
	int32 fontTmp = font & (MR_FONT_EFF_BASE - 1);

	if (fontTmp==MR_FONT_SMALL)
	{
		uiFont.size = SMALL_FONT;
	}
	else if (fontTmp==MR_FONT_MEDIUM)
	{
		uiFont.size = MEDIUM_FONT;
	}
	else if (fontTmp==MR_FONT_BIG)
	{
		uiFont.size = LARGE_FONT;
	}

	uiFont.bold   = (font & MR_FONT_BOLD) ? FONTATTRIB_BOLD : 0;
	uiFont.italic = (font & MR_FONT_ITALIC) ? FONTATTRIB_ITALIC : 0;
	uiFont.underline = (font & MR_FONT_UNDER_LINE) ? FONTATTRIB_UNDERLINE : 0;
	uiFont.smallCaps = (font & MR_FONT_SMALL_CAP) ? SMALLCAPS_DISPLAY_FONT : 0;
	uiFont.oblique = (font & MR_FONT_OBLIQUE) ? FONTATTRIB_OBLIQUE : 0;

	gui_set_font(&uiFont);
}




int32 unet_Soc_SetOpt(int32 s, uint32 option,void* value,uint8 valueLen);
int32 unet_Soc_GetOpt(int32 s,uint32 option,void * value,uint8 valueLen);

int32 mrOpt2localOpt(int32 mr_opt)
{
	int32 option;
	
	switch(mr_opt)
	{
		case MR_SOCKET_OPTION_DELAY:
			option = SOC_NODELAY;
			break;
			
		case MR_SOCKET_OPTION_KEEPALIVE:
			option = SOC_KEEPALIVE;
			break;
			
		case MR_SOCKET_OPTION_LINGER:
			option = SOC_LINGER;
			break;
			
		case MR_SOCKET_OPTION_RCVBUF:
			option = SOC_RCVBUF;
			break;
		
		case MR_SOCKET_OPTION_SNDBUF:
			option = SOC_SENDBUF;
			break;
			
		case MR_SOCKET_OPTION_NREAD:
			option = SOC_NREAD;
			break;
		default:  /*not support*/
			return MR_FAILED;
	}

	return option;
}
int32 mr_jsky_set_opt(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	NetOpt* para = (NetOpt*)input ;
	int32 option;

	mr_trace("mr_jsky_set_opt");
	if(para == NULL)
	{
		return MR_FAILED;
	}

	option = mrOpt2localOpt(para->option);
	if (option==MR_FAILED)
	{
	
		return MR_FAILED;
	}
	
	return unet_Soc_SetOpt(para->soc,(uint32)option,para->value,para->valueLen);
}

int32 mr_jsky_Get_opt(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	NetOpt* para = (NetOpt*)input ;
	int32 option;

	mr_trace("mr_jsky_get_opt");
	
	if(para == NULL)
	{
		return MR_FAILED;
	}

	option = mrOpt2localOpt(para->option);
	if (option==MR_FAILED)
	{
		return MR_FAILED;
	}
	
	return unet_Soc_GetOpt(para->soc,(uint32)option,para->value,para->valueLen);
}

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
int32 mr_jsky_set_zoom(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	OriAppScreenSize* para = (OriAppScreenSize*)input ;

	mr_trace("mr_jsky_screen_zoom");
	
	if(para == NULL)
	{
		mr_trace("@1");
		return MR_FAILED;
	}

	return mr_layer_zoom_enable(para->width, para->height);
}
#endif

int32 mr_jsky_input_Capability(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	int32 keysize = 0;
	int16 keydevice = 0;
	int16 keymmi = 0;
	int16 i = 0,j = 0;
	int32 ret = 0;
	
#ifdef  __MMI_TOUCH_SCREEN__
	ret |= INPUT_CAP_TOUCH_SCREEN;			
#endif

#ifdef __MMI_DSM_MOTION_SUPPORT__
	ret |= INPUT_CAP_G_SENSOR;			
#endif

#if defined(__MMI_TOUCH_SCREEN__) && !defined(WIN32)
#if defined(__TOUCH_PANEL_CAPACITY__) || defined(__TP_AUX_909_SUPPORT__)
	ret |= INPUT_CAP_MULTI_TOUCH;
#endif
#endif

#ifdef __MR_CFG_FEATURE_QWERTY_KEYPAD__
	ret |= INPUT_CAP_KEY_QWERTY;	
#else
       //有些手机定义了下面这些键,但实际上又没用,此时会判断出错.
       //如果以下代码没办法准确判断到手机按键参数,
       //那只有现场根据手机情况写死了.
       //如果没有五向键和数字键,直接注掉相关代码
	keysize = SKY_get_key_size();
	for(i = 0; i < keysize; i++)
	{
		keydevice = SKY_get_device_key_code(i);
		keymmi = SKY_get_mmi_key_code(keydevice); 
		if( (keymmi == KEY_LEFT_ARROW)  \
		    ||(keymmi == KEY_RIGHT_ARROW) \
		    ||(keymmi == KEY_UP_ARROW) \
		    ||(keymmi == KEY_DOWN_ARROW))
		{
			ret |= INPUT_CAP_KEY_ARROW;
		}
		if((keymmi >= KEY_0)&&((keymmi <= KEY_9)))
		{
			j++;
		}
	}
	if( j >= 10)
	{
		ret |= INPUT_CAP_KEY_NUMBER;			
	}
#endif		

	if (output)  *((int32*)output) = ret;
	if (output_len) *output_len = sizeof(int32);
		
	return MR_SUCCESS;
}

extern int32 mr_media_audio_get_play_seconds_ms(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
MR_PLATEX_FUNC_BEGIN(jsky)
	MR_PLATEX_FUNC_ENTRY_SUPPORT(MR_PLATEX_CODE_JSKY_SUPPORT, MR_SUCCESS)
//	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_BASE,NULL)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_FUNCTION,mr_jsky_function)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_INIT,mr_jsky_init)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_VECTOR_FONT_GET_BITMAP,mr_jsky_vector_get_bitmap)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_VECTOR_FONT_ENV,mr_jsky_vector_font_env)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_TRUNCATE,mr_jsky_file_trucate)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_ATTRIBUTE,mr_jsky_file_attribute)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_FOLDERSIZE,mr_jsky_file_foldersize)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_SETATTRIBUTE,mr_jsky_file_setattribute)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_KEYPRESSED,mr_jsky_file_keypressed)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_GETPRESSEDKEYINDEX,mr_jsky_file_get_pressed_keyindex)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_FILE_GETLOGICKEYMAP,mr_jsky_file_get_logickey_map)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_RETURN_IDLE,mr_jsky_return_idle)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_CLIP_PUSH,mr_jsky_puch_clip)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_CLIP_POP,mr_jsky_pop_clip)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_CLIP_SET,mr_jsky_set_clip)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_CLIP_GET,mr_jsky_get_clip)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_GETSTRING_WIDTHHEIGHT,mr_jsky_get_string_widhgt)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_CHARNUMINWIDTH,mr_jsky_charnum_inwidth)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_DRAWTEXT,mr_jsky_draw_text)
	//mr_media_audio_get_play_seconds_ms 在mrp_media.c中定义
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_GET_TOTAL_TIME_MS,mr_media_audio_get_play_seconds_ms)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_SET_OPT,mr_jsky_set_opt)
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_GET_OPT,mr_jsky_Get_opt)
#ifdef __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_SET_ZOOM,mr_jsky_set_zoom)
#endif	
	MR_PLATEX_FUNC_ENTRY(MR_SKY_JVM_INPUT_CAPABILITY,mr_jsky_input_Capability)
MR_PLATEX_FUNC_END()

#ifdef WIN32
int32 sky_jvm_init(uint8* input, int32 input_len, uint8** output, int32* output_len) { return MR_SUCCESS; }
int32 sky_jvm_getVersion(void) { return MR_SUCCESS; }
#endif

#endif/*end __MMI_DSM_NEW_JSKY__*/

#endif/*end __MMI_DSM_NEW__*/

