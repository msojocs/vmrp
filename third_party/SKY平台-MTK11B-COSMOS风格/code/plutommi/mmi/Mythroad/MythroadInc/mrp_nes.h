#ifndef __MRP_PAL_NES_H__
#define __MRP_PAL_NES_H__


#define MR_DEFAULT_NES_DIR   			        "MyNes"

#if 1//ndef NAND_SUPPORT//huangsunbo 20110608	
	#define MR_DEFAULT_NES_DRV                             MMI_CARD_DRV
#else
	#define MR_DEFAULT_NES_DRV                             MMI_PUBLIC_DRV
#endif



/**
 * \brief NES 模块初始化
 *
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_nes_initialize(void);


/**
 * \brief 获取NES游戏按键布局
 *
 * \param output 		[out]按键布局信息结构
 * \param output_len	[out]结构长度
 * \return void
 */
int32 mr_nes_get_keypad_map(uint8** output, int32* output_len);


/**
 * \brief 退出游戏
 *
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_nes_exit_game(void);


/**
 * \brief 获取手柄按键值
 *
 * \param output		[out]NES 手柄按键信息结构体
 * \param output_len	[out]结构体长度
 * \return MR_SUCCESS on success, MR_FAILED otherwise
 */
int32 mr_nes_get_joypad_key(uint8**  output, int32* output_len);


/**
 * \brief 启动NES游戏
 *
 * \return MR_SUCCESS on success, MR_FAILED otherwise.
 */
int32 mr_nes_start_game(void);


/**
 * \brief 创建NES运行支持目录
 *
 * \return void
 */
void mr_nes_create_root_dir(void);


/**
 * \brief 返回NES运行的目录,不带盘符的目录信息
 *
 * \param output		[out]目录信息字符串
 * \param output_len	[out]字符串长度
 * \return MR_SUCCESS on success, MR_FAILED otherwise. 
 */
int32 mr_nes_get_root_dir(uint8**output,int32* output_len);


int32 mr_nes_operate(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
#endif
