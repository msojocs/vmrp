#ifndef __MMI_DSM_FMGR_H__
#define __MMI_DSM_FMGR_H__

#if(MTK_VERSION > 0x0812)
#include "FileMgrGProt.h"
#else
#include "FileMgr.h"
#endif

#ifndef FMGR_MAX_PATH_LEN
#define FMGR_MAX_PATH_LEN 260
#endif

/**
 * \brief 显示类型
 */
typedef enum
{
	MR_DSPL_NONE,
	MR_DSPL_IMAGE,
	MR_DSPL_AUDIO,
	MR_DSPL_VIDEO,
	MR_DSPL_NES,
	MR_DSPL_FOLDER //add by janus.lin 20101018
}MR_E_FMGR_DFT_DSPL;


/**
 * \brief 请求数据结构
 */
typedef struct 
{	
	int32 dft_list;
	char  list_path[FMGR_MAX_PATH_LEN];
}MR_T_FMGR_REQ;


/**
 * \brief 操作结果
 */
typedef struct 
{
	int32 fmgrAction;
	char dir[256];
}mr_fmgrAction;


/**
 * \brief 初始化过滤器
 * 
 * \return void
 */
int32 mr_fmgr_filter_fmgr(void);


/**
 * \brief 设置要显示的过滤类型
 *
 * \param fmgr_type	[in] 要显示的类型
 */
int32 mr_fmgr_filter_set(int32 fmgr_type);


/**
 * \brief 清除特定的过滤类型
 *
 * \param fmgr_type [in] 要清楚的类型
 * \return void
 */
int32 mr_fmgr_filter_clear(int32 fmgr_type);


/**
 * \brief 进入文件浏览界面
 * 
 * \return void
 */
void mmi_dsm_entry_fmgr_screen(void);


/**
 * \brief ?? 好像没被调用到
 */
void mmi_dsm_exit_from_fmgr_emptry_folder(void);


/**
 * \brief 退出文件浏览
 *
 * \return void
 */
int32 mr_fmgr_exit(void);


/**
 * \brief 检查是否已经处于文件浏览相关界面
 *
 * \return 0 表示否，1表示是
 */
int32 dsmCheckBrowerExit(void);


/**
 * \brief 设置文件浏览请求信息
 *
 * \param fmgr_req	[in]请求信息结构
 * \return void
 */
void mr_set_fmgr_req(MR_T_FMGR_REQ *fmgr_req);



int32 mr_fmgr_get_selected_path(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_fmgr_show(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
#endif


/*#auto end#*/
