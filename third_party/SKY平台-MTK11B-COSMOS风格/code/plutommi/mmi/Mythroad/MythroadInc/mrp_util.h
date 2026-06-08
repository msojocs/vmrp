#ifndef __MRP_PAL_UTIL_H__
#define __MRP_PAL_UTIL_H__


/**
 * \brief 计算unicode字串的长度(byte),大小端模式都可以
 * 
 * \param txt	[in]要求长度的字串
 * \return 字传的长度(字节)
 */
int mr_str_wstrlen(char * txt);


/**
 * \brief unicode的大小端转换
 *
 * \param str	[in]要转换的unicode字串
 * \return 转换成功的字串
 */
char *mr_str_convert_endian(char * str);

/**
* \brief unicode的大小端转换,传入大小参数,防止字串没有结束符导致数据转换出错
*
 * \param str	[in]要转换的unicode字串
 * \param size [in] 要转换的字串的大小.
 * \return 转换成功的字串
 */
char *mr_str_convert_endian_with_size(char * str,int32 size);

/**
 * \brief 将GB编码的字符串转成UNICODE编码
 *
 * \param src	[in] GB编码的字符串
 * \param dest	[out] UNICODE编码的字符串，需要调用者确保有足够的内存
 * \return nothing
 */
void mr_str_gb_to_ucs2(char * src, char *dest);


/**
 * \brief  将UNICODE的字符串转成GB编码的字符串
 *
 * \param src		[in]UNICODE编码的字符串
 * \param dest		[out]GB编码的字符串，需要调用者确保有足够的内存
 * \param isBigEn	[in]src的UNICODE编码格式是否为大端
 * \return nothing
 */
int32 mr_str_ucs2_to_gb(char * src, char *dest, BOOL isBigEn);


/**
 * \brief dup the wstr
 */
uint8* mr_str_dup_ucs2(char* str, int maxlen);


/**
 * \brief dup the utf8 string
 */
uint8* mr_str_dup_utf8(char* str, int maxlen);


/**
 * \brief draw the bitmap
 */
void mr_draw_bitmap(U16 *dBuffer, U16 *data, int32 dx, int32 dy, int32 sw, int32 sh, int32 dw, U16 tColor);

/**
  * 判断GB转化表是否存在
  */
int32 mr_chset_init(void);

/**
  * 加载GB转化表
  */
int mr_chset_load_map(void);

/**
  * 卸载GB转化表
  */
int mr_chset_unload_map();

/**
  * 中文UCS2编码转换成GB编码
  *
  * @param ucs2	一个中文字符的UCS2小端编码
  * @param gb	返回中文字符的大端GB编码  
  *
  * @return 是否转换成功
  */
int mr_chset_ucs2_to_gb(const uint8* ucs2, uint8* gb);

/**
  * 中文GB编码转换成UCS2编码
  *
  * @param gb	一个中文字符的大端GB编码
  * @param ucs2	返回中文字符的UCS2小端编码
  *
  * @return 是否转换成功
  */
int mr_chset_gb_to_ucs2(const uint8* gb, uint8* ucs2);

#endif
