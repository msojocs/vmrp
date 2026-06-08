#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"
#include "conversions.h"
#include "med_utility.h"
#include "soc_api.h"

#include "mrp_include.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int mr_str_wstrlen(char * txt)
{
	int lenth=0;
	unsigned char * ss=(unsigned char*)txt;

	while(ss[0] | ss[1])
	{
		lenth+=2;
		ss+=2;
	}
	return lenth;
}


char *mr_str_convert_endian(char * str)
{
	char temp;
	int i;
       int len = mr_str_wstrlen(str);
	   
	for (i = 0; i < len ; i += 2)
	{
            temp = str[i];
	     str[i] = str[i+1];
	     str[i+1] = temp;
	}
	
	return str;
}
//防止传入的字串没有结束符,导致数据转换异常.
char *mr_str_convert_endian_with_size(char * str,int32 size)
{
	char temp = 0;
	int i = 0;
       int len = 0;
       
	if(str == 0)
	{
		return 0;
	}
	len = mr_str_wstrlen(str);
	len = (len > size) ? size : len;
	for (i = 0; i < len ; i += 2)
	{
            temp = str[i];
	     str[i] = str[i+1];
	     str[i+1] = temp;
	}
	
	return str;
}


void mr_str_gb_to_ucs2(char * src,char *dest)
{
       int32 di=0,i=0;
	char stemp[4];
	char dtemp[4];
	
	while(*(src+i))
	{
	       if(*(src+i) > 0x7f)
	      	{
	      	      stemp[0] = *(src+i);
		      stemp[1] = *(src+i+1);
		      stemp[2] = 0;
#ifdef __MR_CFG_FEATURE_MOPOZS_DOWNLOAD_GBTABLE__		
			mr_chset_load_map();
		      mr_chset_gb_to_ucs2((const uint8*)stemp, (uint8*)dtemp);
#else		      
		      mmi_chset_convert(MMI_CHSET_GB2312,MMI_CHSET_UCS2, stemp, dtemp, 4);
#endif		      
		      *(dest+di) = dtemp[0];
		      *(dest+di+1) = dtemp[1];  
		      i = i+2;
		     di = di+2;
	      	}
		else
		{
		       *(dest+di) = *(src+i);
			*(dest+di+1) =0;
			i++;
			di = di+2;
		}
	}
	*(dest+di) = 0;
	*(dest+di+1) = 0;
}


/*
 * \brief 虚拟机的lib中使用了这个函数名
 */
void dsmGB2UCS2(char * src,char *dest)
{
#ifndef __MR_CFG_FEATURE_OVERSEA__
#if !(defined(__MMI_CHSET_GB2312__) || defined(__MR_CFG_FEATURE_MOPOZS_DOWNLOAD_GBTABLE__))
#error "SKY: must define __MMI_CHSET_GB2312__"
#endif
#endif
	mr_str_gb_to_ucs2(src, dest);
}


int32 mr_str_ucs2_to_gb(char * src, char *dest, BOOL isBigEn)
{
	int32 di = 0,i= 0;
	int32 len = mr_str_wstrlen(src);
	char stemp[4];
       char dtemp[4];
	char *p;

	if(len <= 0)
		return MR_FAILED;

	p = OslMalloc(len+2);
	if(p == NULL)
	{
		*dest = 0;
		return MR_FAILED;
	}

       memcpy(p,src,len+2);

	if(!isBigEn)   
		mr_str_convert_endian(p);
	
	for(i = 0;i<len;i = i+2)
	{
	       if(*(p+i) == 0)
	       {
	              *(dest+di) = *(p+i+1);
			di++;
	       }
		else
		{
#if (MTK_VERSION >= 0x06b)	
		        stemp[0]=*(p+i+1);
			 stemp[1] = *(p+i);
#else
		        stemp[0]=*(p+i);
			 stemp[1] = *(p+i+1);
#endif			 
			 stemp[2] = 0;
			 stemp[3] = 0;
#ifdef __MR_CFG_FEATURE_MOPOZS_DOWNLOAD_GBTABLE__	
			mr_chset_load_map();
			mr_chset_ucs2_to_gb((const uint8*)stemp, (uint8*)dtemp);
#else		        
		       mmi_chset_convert(MMI_CHSET_UCS2, MMI_CHSET_GB2312, stemp, dtemp, 4);
#endif		        
			*(dest+di) = dtemp[0];
			*(dest+di+1) = dtemp[1];
			di = di+2;
		}
	}
	*(dest+di) = 0;
	OslMfree(p);

	return MR_SUCCESS;
}


uint8* mr_str_dup_ucs2(char* str, int maxlen)
{
	uint8* tmpstr;
	S32 slen;

	if(!str) return NULL;

	slen = mr_str_wstrlen((char*)str) + 2;
	if(maxlen > 0 && slen > maxlen) slen = maxlen & 0xFFFFFFFE;
	
	tmpstr  = (uint8*)OslMalloc(slen);
	if(!tmpstr){ 
		return NULL;
	}
	
	kal_mem_set(tmpstr, 0, slen);
	kal_mem_cpy(tmpstr, str, slen - 2);
	return tmpstr;
}


uint8* mr_str_dup_utf8(char* str, int maxlen)
{
	uint8* tmpstr;
	S32 len;

	if(!str) return NULL;

	len = strlen(str)+1;
	if(maxlen > 0 && len > maxlen) len = maxlen;
	
	tmpstr = (uint8*)OslMalloc(len);
	if(!tmpstr) return NULL;

	kal_mem_set(tmpstr, 0, len);
	kal_mem_cpy(tmpstr, str, len - 1);
	return tmpstr;
}


void mr_draw_bitmap(U16 *dBuffer, U16 *data, int32 dx, int32 dy, int32 sw, int32 sh, int32 dw, U16 tColor)
{
	U16*pDest = dBuffer + dy * dw + dx;
	U16 *pSrc = data;
	int i =0, j=0;
	
	for(j =0; j<sh; j++)
	{		
		for(i=0; i<sw; i++)
		{
			if(*pSrc != tColor)
			{
				*pDest =* pSrc;
			}
			
			pDest ++;
			pSrc ++;
		}
		pDest += (dw-sw);
	}
}

typedef struct {
	uint16 ucs2;	// 大端
	uint16 gb;	// 大端
} mr_ucs2_gb_pair;

typedef struct {
	int32 count;
	mr_ucs2_gb_pair* pairs;
} mr_ucs2_to_gb_map;

typedef struct {
	int32 zone_count;
	int32 zone_size;	//每一个zone大小为94
	uint8* zones;
	uint16* unicodes;
	uint8 zone_key[0xF7];		// 现在最大为0xF7
} mr_gb_to_ucs2_map;

static mr_gb_to_ucs2_map* gb_to_ucs2_map = NULL;
static mr_ucs2_to_gb_map* ucs2_to_gb_map = NULL;

/**
  * 中文GB编码转换成UCS2编码
  *
  * @param gb	一个中文字符的大端GB编码
  * @param ucs2	返回中文字符的UCS2小端编码
  *
  * @return 是否转换成功
  */
int mr_chset_gb_to_ucs2(const uint8* gb, uint8* ucs2)
{
	uint8 msb, lsb;	// GB编码的高位字节与低位字节
	int msb_index, lsb_index;		// 在转换表中对应的索引

	// 初始化为0
	*ucs2 = 0;
	*(ucs2 + 1) = 0;
	
	if (gb_to_ucs2_map == NULL)
	{
		return MR_FAILED;
	}

	msb = *gb;
	lsb = *(gb + 1);

	msb_index = gb_to_ucs2_map->zone_key[msb];

	if (msb_index < gb_to_ucs2_map->zone_count)
	{
		
		lsb_index = lsb - 0xA0 -1;
		if (lsb_index >=0 && lsb_index < gb_to_ucs2_map->zone_size)
		{
			uint16 ucs2_big;

			ucs2_big = gb_to_ucs2_map->unicodes[msb_index * gb_to_ucs2_map->zone_size + lsb_index];
			*ucs2 = (ucs2_big & 0xFF00) >> 8;
			*(ucs2 + 1) = ucs2_big & 0x00FF;

			mr_trace("msb_index=%d", msb_index);
			mr_trace("lsb_index=%d", lsb_index);
			return MR_SUCCESS;
		}
	}

	return MR_FAILED;
}

/**
  * 中文UCS2编码转换成GB编码
  *
  * @param ucs2	一个中文字符的UCS2小端编码
  * @param gb	返回中文字符的大端GB编码  
  *
  * @return 是否转换成功
  */
int mr_chset_ucs2_to_gb(const uint8* ucs2, uint8* gb)
{
	int left, right, middle;
	uint16 tmp1, tmp2, tmp3;

	// 初始化为0
	*gb = 0;
	*(gb + 1) = 0;

	left = 0;
	right = ucs2_to_gb_map->count - 1;
	tmp1 = (*ucs2) | (*(ucs2 + 1)  << 8);

	while (left <= right)
	{
		middle = (left + right) /2;
		tmp2 = soc_ntohs(ucs2_to_gb_map->pairs[middle].ucs2);

		if (tmp2 == tmp1)
		{
			tmp3 = ucs2_to_gb_map->pairs[middle].gb;
			*gb = tmp3 & 0x00FF;
			*(gb + 1) = (tmp3 & 0xFF00) >> 8;

			return MR_SUCCESS;
		}
		else if (tmp2 < tmp1)
		{
			left = middle + 1;
		}
		else
		{
			right = middle - 1;
		}
	}

	return MR_FAILED;
}

int mr_chset_load_map(void)
{
	static mr_gb_to_ucs2_map gb_to_ucs2;
	static mr_ucs2_to_gb_map ucs2_to_gb;
	MR_FILE_HANDLE f;
	int i;
	int32 tmp;
	int32 read_cnt;

	if (gb_to_ucs2_map != NULL) goto next;

	mr_trace("mr_chset_load_map 1");

	// 先加载GB转UCS2表
	f = mr_open("system\\2312_u16be.bin", MR_FILE_RDONLY);
	if (f <= 0)	return MR_FAILED;

	mr_read(f, &tmp, sizeof(int32));
	gb_to_ucs2.zone_count = soc_ntohl(tmp);
	gb_to_ucs2.zone_size = 94;
	
	gb_to_ucs2.zones = med_alloc_ext_mem(gb_to_ucs2.zone_count);
	gb_to_ucs2.unicodes = med_alloc_ext_mem(gb_to_ucs2.zone_size * gb_to_ucs2.zone_count * sizeof(uint16));
	if (gb_to_ucs2.zones == NULL || gb_to_ucs2.unicodes == NULL)
	{
		mr_trace("mr_chset_load_map: no mem 1");
		mr_close(f);

		if (gb_to_ucs2.zones != NULL ) med_free_ext_mem((void**)&gb_to_ucs2.zones);
		if (gb_to_ucs2.unicodes != NULL) med_free_ext_mem((void**)&gb_to_ucs2.unicodes);
		
		return MR_FAILED;
	}

	read_cnt = mr_read(f, gb_to_ucs2.zones, gb_to_ucs2.zone_count);
	read_cnt = mr_read(f, gb_to_ucs2.unicodes, gb_to_ucs2.zone_size * gb_to_ucs2.zone_count * sizeof(uint16));

	mr_close(f);	

	// 初始化zone_key，为了能更快的查找
	memset(gb_to_ucs2.zone_key, 0xFF, sizeof(gb_to_ucs2.zone_key));
	for (i = 0; i < gb_to_ucs2.zone_count; ++i)
	{
		gb_to_ucs2.zone_key[gb_to_ucs2.zones[i]] = i;
	}

	gb_to_ucs2_map = &gb_to_ucs2;

next:

	if (ucs2_to_gb_map != NULL) return MR_SUCCESS;

	mr_trace("mr_chset_load_map 2");
	
	// 加载UCS2转GB表
	f = mr_open("system\\u16b_2312.bin", MR_FILE_RDONLY);
	if (f <= 0) return MR_FAILED;

	mr_read(f, &tmp, sizeof(int32));
	ucs2_to_gb.count = soc_ntohl(tmp);
	ucs2_to_gb.pairs = med_alloc_ext_mem(ucs2_to_gb.count * sizeof(mr_ucs2_gb_pair));
	if (ucs2_to_gb.pairs == NULL)
	{
		mr_trace("mr_chset_load_map: no mem 2");
		
		mr_close(f);

		return MR_FAILED;
	}

	read_cnt = mr_read(f, ucs2_to_gb.pairs, ucs2_to_gb.count * sizeof(mr_ucs2_gb_pair));

	mr_close(f);

	ucs2_to_gb_map = &ucs2_to_gb;

	mr_trace("mr_chset_load_map 3");
	
	return MR_SUCCESS;
}

int mr_chset_unload_map()
{
	if (gb_to_ucs2_map != NULL)
	{
		mr_trace("mr_chset_unload_map 1");
		
		med_free_ext_mem((void**)&gb_to_ucs2_map->zones);
		med_free_ext_mem((void**)&gb_to_ucs2_map->unicodes);

		gb_to_ucs2_map = NULL;
	}

	if (ucs2_to_gb_map != NULL)
	{
		mr_trace("mr_chset_unload_map 2");
		
		med_free_ext_mem((void**)&ucs2_to_gb_map->pairs);

		ucs2_to_gb_map = NULL;
	}
}

int32 mr_chset_init(void)
{
	int32 file_info;
	
	file_info = mr_info("system\\2312_u16be.bin");	
	if (file_info != MR_IS_FILE)	return MR_FAILED;

	file_info = mr_info("system\\u16b_2312.bin");
	if (file_info != MR_IS_FILE) return MR_FAILED;

	return MR_SUCCESS;
}

#endif

