#ifndef __MRP_PAL_SYSINFO_H__
#define __MRP_PAL_SYSINFO_H__


#define DSM_MASTER_SIM                                   (-1)
#define DSM_SLAVE_SIM                                      (-2)
#define DSM_THIRD_SIM                                      (-3)
#define DSM_FOURTH_SIM                                      (-4)


#define DSM_MAINMENU_SUPPORT_SIZE             (36)
#define MAX_IMSI_LEN                                        17


typedef void (*MCB)(void);

typedef struct
{
	char *name;
	MCB func;
}T_MITEM;


typedef struct
{
	int32 num;
	int32 index;
	MCB exitFunc;
	MCB bookFunc;
	MCB gameFunc;
	T_MITEM item[DSM_MAINMENU_SUPPORT_SIZE];
}T_MAINMENU;



/**
 * \brief 初始化sim卡信息
 *	SIM 卡信息初始化是系统初始化的重要步骤, 有些本地应用需要使用平台移植层的网络接口
 *	也需要调用这个接口先进行SIM卡相关的初始化工作。
 */
void mr_sim_initialize(void);


/**
 * \brief 设置激活的sim卡
 * 
 * \param simid SIM卡的id， DSM_MASTER_SIM表示主卡， DSM_SLAVE_SIM表示付卡
 * \return 
 *	- MR_SUCCESS 成功
 *	- MR_FAILED 失败
 */
int32 mr_sim_set_active(int32 simId);


/**
 * \brief 获取当前激活的SIM 卡ID
 *
 * \return DSM_MASTER_SIM 表示主卡，DSM_SLAVE_SIM表示附卡
 */
int32 mr_sim_get_active(void);


/**
 * \brief 获取SIM卡信息
 */
int32 mr_sim_get_info(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 往底层发请求去获得imei号
 */
void mr_imei_initialize(void);


/**
 * \brief 往底层发送获取IMSI号请求
 *
 */
void mr_imsi_initialize(void);


/**
 * \brief bsid 信息初始化
 *
 */
int32 mr_bsid_initialize(int32 param);


/**
 * \brief 获取小区信息
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_get_cellinfo(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief bsid模块是否
 *
 */
int32 mr_bsid_terminate(int32 param);


/**
 * \brief 信号模块初始化
 *
 */
int32 mr_signal_initialize(int32 param);


/**
 * \brief 获取信号强度
 *
 * \param output		[out]信号信息结构
 * \param output_len	[out]结构长度
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_signal_get_level(uint8**output, int32 *output_len);


/**
 * \brief 信号模块释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_signal_terminate(int32 param);


/**
 * \brief 获取menu信息
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_get_mainmenu_info(uint8**output,int32 *output_len,MR_PLAT_EX_CB *cb);


#endif
