#ifndef __MMI_DSM_UNET_H__
#define __MMI_DSM_UNET_H__
/******************************************************************
DATE:20090609
AUTHOR:SKY
FILE: UNINET INTERFACE
	统一gprs、socket管理，为多联网应用提供统一便捷接口服务
********************************************************************/

#include "mrporting.h"

/**--------------------------------------------------------------------------*
 **                         CONST & STRUCTS                                    *
 **--------------------------------------------------------------------------*/

#define UNET_INVALID_HANDLE			(0)
#define UNET_SPEC_WIFI_HANDLE		(0xEFEF)//wifi标志handle

typedef int32 (*t_unet_act_cb)(int32 result);
typedef int32 (*t_unet_dsn_cb)(int32 result);
typedef int32 t_unet_handle; //a app mod handle

/*sim select*/
typedef enum
{
	UNET_SIM_MASTER,
	UNET_SIM_SLAVE,
	UNET_SIM_THIRD,
	UNET_SIM_FOURTH,
	UNET_SIM_MAX
}e_unet_sim_type;

/*apn other context s*/
typedef struct
{
	char 				*userName;
	char 				*passWord;
	unsigned char 			dns[4];
	unsigned char 			authentication_type;
}t_unet_apn_account;

/**--------------------------------------------------------------------------*
 **                         PUBLIC FUNCTIONS DECLARE                                 *
 **--------------------------------------------------------------------------*/

/****************************************************************************
函数名:void unet_ApnAL_Init(uint8 csdAccount, uint8 gprsAccount)
描  述:unet 初始化，仅开机时调用
参  数:uint8 csdAccount - 平台上CSD帐号位总数
		uint8 gprsAccount - 平台上GPRS帐号为总数
返  回:void
****************************************************************************/
void unet_ApnAL_Init(uint8 csdAccount, uint8 gprsAccount);

/****************************************************************************
函数名: int32 unet_ApnAL_ActiveCntx(char *apn, t_unet_apn_account *accountInfo, t_unet_act_cb cb, e_unet_sim_type simId, int32 *appHandle)
描  述:建立apn context
参  数:const char *apn, - apn
		t_unet_apn_account *accountInfo - 其他帐号属性,可以是NULL即无需特殊指定
		t_unet_act_cb cb,  - 异步结果回调
		e_unet_sim_type simId,  - 双卡双待使用，单卡请填UNET_SIM_MASTER
		int32 *appHandle - appHandle address
返  回:MR_SUCCESS 操作成功
	       :MR_FAILED    操作失败
	       :MR_WAITING 此操作是个异步过程，需要通过cb 进行异步通知
****************************************************************************/
int32 unet_ApnAL_ActiveCntx(char *apn, t_unet_apn_account *accountInfo, t_unet_act_cb cb, e_unet_sim_type simId, int32 *appHandle);

/****************************************************************************
函数名:int32 unet_ApnAL_CloseCntx(int32 *appHandle)
描  述:注销apn 连接
参  数:int32 *appHandle  - appHandle address
返  回:MR_SUCCESS 操作成功
	       :MR_FAILED    操作失败
****************************************************************************/
int32 unet_ApnAL_CloseCntx(int32 *appHandle);

/****************************************************************************
函数名:int32 unet_ApnAl_activeCntx_Cust(t_unet_act_cb cb, e_unet_sim_type simId, int32 *appHandle)
描  述: 功能同unet_ApnAL_ActiveCntx(), 目的是将platex的apn account设置统一到unet流程中-见dsm调用实例
参  数: t_unet_act_cb cb,  - 异步结果回调
		e_unet_sim_type simId,  - 双卡双待使用，单卡请填UNET_SIM_MASTER
 		int32 *appHandle - appHandle address
返  回:MR_SUCCESS 操作成功
	       :MR_FAILED    操作失败
	       :MR_WAITING 此操作是个异步过程，需要通过cb 进行异步通知
****************************************************************************/
int32 unet_ApnAl_ActiveCntx_Cust(t_unet_act_cb cb, e_unet_sim_type simId, int32 *appHandle);

/****************************************************************************
函数名: int32 unet_Soc_GetHostByName(const char *name, t_unet_dsn_cb cb, int32 appHandle)
描     述: 请求进行域名解析
参     数: const char *name, - host name
		t_unet_dsn_cb cb, - 异步结果回调
		int32 appHandle - apphandle
返  回:MR_SUCCESS 操作成功
	       :MR_FAILED    操作失败
	       :MR_WAITING 此操作是个异步过程，需要通过cb 进行异步通知
****************************************************************************/
int32 unet_Soc_GetHostByName(const char *name, t_unet_dsn_cb cb, int32 appHandle);

/****************************************************************************
函数名: int32 unet_Soc_State(int32 s)
描  述: 查询socket(连接)状态
参  数: int32 s
返  回:MR_SUCCESS 已连接
	       :MR_FAILED   连接失败
	       :MR_WAITING  建立连接中
****************************************************************************/
int32 unet_Soc_State(int32 s);

/****************************************************************************
函数名: void unet_Soc_Break(int32 appHandle)
描  述: 某app的socket被外界强行破坏-比如sim卡切换时
参  数: int32 appHandle - apphandle
返  回:void
****************************************************************************/
void unet_Soc_Break(int32 appHandle);

/****************************************************************************
函数名:int32 unet_soc_create(int32 type, int32 protocol, int32 appHandle)
描     述: 创建一个socket
参     数: int32 type, - MR_SOCK_STREAM or SOCK_DGRAM
		int32 protocol, - invalid
		int32 appHandle - apphandle
返     回: 创建成功的socket的句柄
****************************************************************************/
int32 unet_Soc_Create(int32 type, int32 protocol, int32 appHandle);

/****************************************************************************
函数名: int32 unet_Soc_Connect(int32 s, int32 ip, uint16 port, int32 type)
描     述:  建立连接
参     数: int32 s, - socket id
		int32 ip, - 主机ip
		uint16 port, - 端口
		int32 type -    MR_SOCKET_BLOCK,          //阻塞方式（同步方式）
  					 MR_SOCKET_NONBLOCK       //非阻塞方式（异步方式）
返  回:MR_SUCCESS 已连接
	       :MR_FAILED   连接失败
	       :MR_WAITING  建立连接中
****************************************************************************/
int32 unet_Soc_Connect(int32 s, int32 ip, uint16 port, int32 type);

/****************************************************************************
函数名: int32 unet_Soc_Close(int32 s)
描     述:  关闭连接
参     数: int32 s - socket id
返  回:MR_SUCCESS 操作成功
	       :MR_FAILED    操作失败
****************************************************************************/
int32 unet_Soc_Close(int32 s);

/****************************************************************************
函数名: int32 unet_Soc_Recv(int32 s, const char *buf, int len)
描     述:  recv data
参     数: int32 s, char *buf, int len
返     回: >=0, get data len
		-1, fatol err
****************************************************************************/
int32 unet_Soc_Recv(int32 s, const char *buf, int len);

/****************************************************************************
函数名: int32 unet_Soc_Send(int32 s, const char *buf, int len)
描     述:  send data
参     数: int32 s, char *buf, int len
返     回: >=0, send data len
		-1, fatol err
****************************************************************************/
int32 unet_Soc_Send(int32 s, const char *buf, int len);

/****************************************************************************
函数名:int32 unet_Soc_Sendto(int32 s, const char *buf, int len, int32 ip, uint16 port)
描  述:UDP 发送 接口
参  数:
返  回:
****************************************************************************/
int32 unet_Soc_Sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);

/****************************************************************************
函数名:int32 unet_Soc_Recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port)
描  述:UDP 接收 接口
参  数:
返  回:
****************************************************************************/
int32 unet_Soc_Recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);

/****************************************************************************
函数名: int32 unet_Soc_Bind(mr_bind_st *d_bind)
描  述: bind
参  数:
返  回:
****************************************************************************/
int32 unet_Soc_Bind(mr_bind_st *d_bind);


/****************************************************************************
函数名: void unet_Soc_Notifyvoid
描     述:异步socket消息处理
参     数:
返     回:
****************************************************************************/
MMI_BOOL unet_Soc_Notify(void* inMsg);


/****************************************************************************
函数名: void mr_soc_app_soc_get_host_by_name_ind(void* inMsg)
描  述: 域名解析消息处理
参  数:
返  回:void
****************************************************************************/
MMI_BOOL unet_Soc_GetHBN_Notify(void* inMsg);


/**
 * \brief 
 */
void mr_net_set_protocol_event_handler(U16 eventID, PsIntFuncPtr funcPtr, MMI_BOOL isMultiHandler);


/**
 * \brief
 */
void mr_net_clear_protocol_event_handler(U16 eventID, PsIntFuncPtr funcPtr);


#endif//__MMI_DSM_UNET_H__

/*#auto end#*/