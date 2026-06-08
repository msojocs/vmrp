#ifndef __MRP_PAL_NET_H__
#define __MRP_PAL_NET_H__


typedef struct
{
    uint8 authentication_type;
    uint8 user_name[32];
    uint8 password[32];
    uint8 dns[4];
    uint8 apn[100];
}T_DSM_DATA_ACCOUNT;



#if (MTK_VERSION >= 0x0936)
#define DSM_SET_GPRS_ACCOUT_DEFAULT_VALUE  \
    myMsgPtr->gprs_account.dcomp_algo = 0x02; \
    myMsgPtr->gprs_account.hcomp_algo = 0x02; \
    myMsgPtr->gprs_account.pdp_type = 0x21;\
    myMsgPtr->gprs_account.pdp_addr_len = 0x01;
#else
#define DSM_SET_GPRS_ACCOUT_DEFAULT_VALUE
#endif


extern int32 gDsmUseNVdataAccount;



/**
 * \brief 初始化网络模块
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_net_initialize(void);


/**
 * \brief 网络模块释放
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_net_terminate(void);


/**
 * \brief 关闭全部VM的socket
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_net_socket_breakall(void);


/**
 * \brief 设置事件处理函数
 *
 * \param eventID	[in] 事件ID
 * \param funcPtr	[in] 事件处理函数
 * \param isMultiHandler	[in] 是否支持多事件处理函数
 */
void mr_net_set_protocol_event_handler(U16 eventID, PsIntFuncPtr funcPtr, MMI_BOOL isMultiHandler);


/**
 * \brief 清空事件回调函数
 *
 * \param eventID	[in] 事件ID
 * \param funcPtr	[in] 事件处理函数
 */
void mr_net_clear_protocol_event_handler(U16 eventID, PsIntFuncPtr funcPtr);


/**
 * \brief  初始化网络信息，拨号账户，及进行拨号连接gprs
 *
 * \param cb	[in]如果是异步的实现方式，则通过这个回调通知操作结果
 * \param mode	[in]拨号方式, 分同步和异步
 * \return 
 *		- MR_SUCCESS 操作成功
 *		- MR_FAILED 操作失败
 *		- MR_WAITING 此操作是个异步过程，需要通过cb 进行异步通知
 */
int32 mr_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode);


/**
 * \brief 请求进行域名解析
 *
 * \param name	[in]域名
 * \param cb	[in]回调函数
 * \return 
 *		- MR_WAITING 表示异步调用，正在处理过程中
 *		- MR_FAILED 查询失败
 *		- 其他值，表示成功，返回对应的IP地址
 */
int32 mr_getHostByName(const char *name, MR_GET_HOST_CB cb);


/**
 * \brief 创建一个socket
 *
 * \param type		[in] 类型
 * \param protocol	[in] 协议族 
 * \return 创建成功的socket的句柄，失败返回MR_FAILED
 */
int32 mr_socket(int32 type, int32 protocol);


/**
 * \brief 连接服务器
 *
 * \param s		[in] socket 句柄
 * \param ip	[in] 服务器端IP地址
 * \param port	[in] 服务端端口号
 * \param type	[in] 同步，异步模式
 * \return 成功返回MR_SUCCESS, 异步返回MR_WAITING,失败返回MR_FAILED
 */
int32 mr_connect(int32 s, int32 ip, uint16 port, int32 type);


/**
 * \brief 关闭socket
 *
 * \param s	[in] 待关闭的socket
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_closeSocket(int32 s);


/**
 * \brief 接受数据
 *
 * \param s		[in] socket 句柄
 * \param buf	[in] 存放接受数据的BUFFER
 * \param len	[in] 接受数据BUFFER的大小
 * \return 
 *		- >= 0 表示实际接收的数据字节数
 *		- < 0 表示出错
 */
int32 mr_recv (int32 s, char *buf, int len);


/**
 * \brief 发送数据
 *
 * \param s		[in] socket 句柄
 * \param buf	[in] 发送数据的BUFFER
 * \param len	[in] 发送数据的长度，字节
 * \return 
 *		- >=0 表示时间发送的字节数
 *		- < 0 表示出错
 */
int32 mr_send(int32 s, const char *buf, int len);


/**
 * \brief 发送UDP 数据报
 *
 * \param s		[in] socket 句柄
 * \param buf	[in] 待发送数据的BUFFER
 * \param len	[in] 发送数据的长度
 * \param ip	[in] 对端的IP地址
 * \param port	[in] port对端的端口号
 */
int32 mr_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);


/**
 * \brief 接受UDP数据包
 *
 * \param s		[in] socket 句柄
 * \param buf	[in] 接收数据的BUFFER
 * \param len	[in] 接收BUFFER的长度
 * \param ip	[in] 对端的IP地址
 * \param port	[in] 对端的端口号
 * \return
 *		- >= 0 表示实际接收的数据
 *		- < 0 表示出错
 */
int32 mr_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);


/**
 * \brief 关闭网络
 *
 * \return 成功返回MR_SUCCESS, 失败返回MR_FAILED
 */
int32 mr_closeNetwork(void);


/**
 * \brief 检查连接状态, 用于在异步连接的时候判断当前的连接状态
 *
 * \param s		[s] socket 句柄
 * \return 
 *		- MR_SUCCESS 表示连接成功
 *		- MR_WAITING 表示连接失败
 *		- MR_FAILED 出错
 */
int32 mr_net_check_connect(int32 s);


/**
 * \brief bind 到对应的ip 和端口, 用于UDP
 *
 * \param d_bind	[in] bind 信息，主要是socket句柄，要bind的IP及端口
 * \return MR_SUCCESS 表示成功，MR_FAILED 表示失败
 */
int32 mr_net_soc_bind(mr_bind_st *d_bind);


/**
 * \brief 设置数据账号
 * 
 * \param req	[in] 账号信息
 * \return 成功MR_SUCCESS, 失败MR_FAILED，异步操作返回MR_WAITING
 */
int32 mr_net_set_data_account(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 获取数据账号
 *
 * \return 成功MR_SUCCESS, 失败MR_FAILED，异步操作返回MR_WAITING
 */
int32 mr_net_get_data_account_req(int32 param);


/**
 * \brief 调用UI界面设置数据账号
 * 
 */
void dsmMtkUISetDataAccount(void);


/**
 * \brief 进入WAP浏览器
 * 
 * \param wap	[in] url地址
 */
void mr_connectWAP(char* wap);


/**
 * \brief 得到用户使用的是什么卡(sim)
 *
 * \return 
 *		MR_NET_ID_MOBILE:中国移动
 *		MR_NET_ID_CN    :中国联通
 *		MR_NET_ID_CDMA	:联通CDMA
 *		MR_NET_ID_NONE  :无卡
 */
int32 mr_getNetworkID(void);


/**
 * \brief 获取当前的PLMN
 */
int32 mr_net_get_network_plmn(uint8**output,int32 *output_len);


/**
 * \brief 查询时间戳
 */
int32 mr_net_gprs_info_query(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);


/**
 * \brief 更新时间戳
 */
void mr_net_gprs_info_update(void);


/**
 * \brief 保存时间戳信息
 */
void mr_net_gprs_info_save(void);


/**
 * \brief 加载时间戳信息
 */
void mr_get_gprs_info_load(void);


int32 mr_net_get_data_account(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
int32 mr_net_data_account_show_ui(int32 param);
int32 mr_net_data_account_set_use_flag(int32 param);
#endif

