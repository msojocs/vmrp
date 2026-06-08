#ifndef __MRP_PAL_FS_H__
#define __MRP_PAL_FS_H__


typedef struct
{
	uint32 total;
	uint32 tUnit;
	uint32 account;
	uint32 unit;
}T_DSM_DISK_INFO;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PAL 层接口


/**
 * \brief 由相对路径的文件名接成绝对路径名
 *
 * \param filename	[in]相对路径的文件名
 * \param outputbuf	[out]转换好的绝对路径文件名(outputbuf的大小要大于等于DSM_MAX_FILE_LEN * ENCODING_LENGTH)
 * \return 绝对路径的文件名
 */
char* mr_fs_get_filename(char * outputbuf, const char *filename);


/**
 * \brief 路径分割符转换，将本地的分割符"\"转成vm的分割符"/"
 *
 * \param path	[in]路径名 GB编码
 * \return 无
 */
void mr_fs_separator_local_to_vm(U8 * path);


/**
 * \breif 路径分割符转换，将vm的分割符"/"转成本地的分割符"\"
 *
 * \param path [in]路径名 GB编码
 * \return 无
 */
void mr_fs_separator_vm_to_local(U8 * path);


/**
 * \brief 创建vm 默认目录:默认目录中用来保存"设置信息",
 *             其他一些掉电不能丢失的信息默认目录不能创建
 *             在可卸载的存储卡上
 */
void mr_fs_create_root_dir(void);


/**
 * \brief 创建vm 运行时候的根目录
 */
void mr_fs_create_vm_dir(void);


/**
 * \brief 设置dsm 启动时候的盘符和路径(务必调用这个接口)
 * 
 * \param drv	[in] 系统盘符
 * \param path	[in] 除去盘符的路径
 */
void mr_fs_set_launch_dir(U8 drv,char *path);


/**
 * \brief VM 对路径操作的接口
 *
 * \param input		[in] vm 中的绝对路径
 * \param input_len	[in] 路径长度，字节数
 * \param output
 * \param output_len
 * \return 操作结果
 *	- MR_SUCCESS 操作成功
 *	- MR_FAILED 操作失败
 */
int32 mr_fs_switch_path(uint8* input, int32 input_len, uint8** output, int32* output_len);


/**
 * \brief 获取系统磁盘空间
 *
 * \param input
 * \param input_len
 * \param spaceInfo
 * \return 
 */
int32 mr_fs_get_free_space(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief  设置工作盘符
 *
 * \param drv
 */
void mr_fs_set_work_drv(U8 drv);


/**
 * \brief 获取当前的工作盘符
 *
 * \return 
 */
U8 mr_fs_get_work_drv(void);


/**
 * \brief 设置工作路径
 *
 * \param path
 */
void mr_fs_set_work_path(char *path);


/**
 * \brief 获取工作路径
 *
 * \return 
 */
char *mr_fs_get_work_path(void);


/**
 * \brief 返回到VM根目录
 */
void mr_fs_switch_to_root_dir(void);


/**
 * \brief 检查T卡状态
 */
int32 mr_fs_check_msdc_status(int32 param);


/** 
 * \brief 获取当前读写位置
 */
int32 mr_fs_get_position(int32 param);


/**
 * \brief 获取文件属性
 */
int32 mr_fs_get_create_time(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 虚拟机移植接口

/**
 * \brief 得到指定文件得大小
 *
 * \param filename	[in]所指定得文件名
 * \return 成功返回文件大小
 *      失败返回:MR_FAILED
 */
int32 mr_getLen(const char* filename);


/**
 * \brief 打开一个文件
 *
 * \param filename	[in]要打开得文件得文件名
 * \param mode		[in]打开文件得模式
 * \return 成功返回打开得文件句柄,失败:NULL
 */
MR_FILE_HANDLE mr_open(const char* filename,  uint32 mode);


/**
 * \brief 读取文件中得数据
 *
 * \param f	[in]要读得文件得句柄
 * \param p	[out]缓存得指针
 * \param l	[in]缓存得大小
 * \return 
 */
int32 mr_read(MR_FILE_HANDLE f,void *p,uint32 l);


/**
 * \brief 往一个文件中写入数据
 *
 * \param f	[in]要写入得文件得句柄
 * \param p	[in]缓存得指针
 * \param l	[int]要写入数据得大小
 * \return 
 */
int32 mr_write(MR_FILE_HANDLE f,void *p,uint32 l);


/**
 * \brief 关闭一个文件
 *
 * \param f	[in]要关闭得文件得句柄
 * \return NR_SUCCESS,MR_FAILED
 */
int32 mr_close(MR_FILE_HANDLE f);


/**
 * \brief 得到一个文件信息
 * \param filename
 * \return 是文件:MR_IS_FILE
 *      是目录:MR_IS_DIR
 *      无效:  MR_IS_INVALID
 */
int32 mr_info(const char* filename);


/**
 * \brief 偏移文件读写指针
 *
 * \param f			[in]文件句柄
 * \param pos		[in]要偏移得数量
 * \param method	[in]偏移起算的位置
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_seek(MR_FILE_HANDLE f, int32 pos, int method);


/**
 * \brief 删除一个文件
 * 
 * \param filename	[in]要被删除的文件的文件名
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_remove(const char* filename);


/**
 * \brief 对一个文件进行重命名
 * 
 * \param oldname	[in]原文件名
 * \param newname	[in]新文件名
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_rename(const char* oldname, const char* newname);


/**
 * \brief 创建一个目录
 *
 * \parma name	[in]目录名
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_mkDir(const char* name);


/**
 * \brief 删除一个目录
 *
 * \param name	[in]被删除的目录名
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_rmDir(const char* name);

/**
 * \brief 初始化一个文件目录的搜索，并返回第一搜索。
 *
 * \param name	 	[in]要搜索的目录名
 * \param buffer	[out]保存第一个搜索结果的buf
 * \param len		[in]buf的大小
 * \return 成功:第一个搜索结果的句柄
 *      失败:MR_FAILED
 */
MR_FILE_HANDLE mr_findStart(const char* name, char* buffer, uint32 len);


/**
 * \brief 搜索目录的下一个结果
 *
 * \param search_handle 	[in]目录的句柄
 * \param buffer			[out]存放搜索结果的buf
 * \param len           	[in]buf的大小
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_findGetNext(MR_FILE_HANDLE search_handle, char* buffer, uint32 len);


/**
 * \brief 停止当前的搜索
 *
 * \param search_handle	[in]搜索句柄
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_findStop(MR_SEARCH_HANDLE search_handle);


/**
 * \brief 该函数用于调试使用，返回的是最后一次操作文件失败的错误信息，返回的错误
 *      信息具体含义与平台上使用的文件系统有关。
 *
 * \return MR_SUCCESS,MR_FAILED
 */
int32 mr_ferrno(void);


#endif
