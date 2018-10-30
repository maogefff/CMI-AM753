#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "MMI_features.h"

#include "soc_api.h"
#include "MMIDataType.h"
#include "gui.h"
#include "nvram_user_defs.h"
#include "ssl_api.h"


#define SOC_SEND_BUFFER_SIZE            (512)//发送缓冲大小
#define SOC_RECV_BUFFER_SIZE            (512)//接收缓冲大小



/*socket 执行的状态*/
typedef enum{
	SOC_STATE_START = 0,    
	SOC_STATE_CREATE_SUCCESS,             
	SOC_STATE_CONNECT_SUCCESS,             
	SOC_STATE_SEND_SUCCESS,              
	SOC_STATE_RECV_SUCCESS,    
	SOC_STATE_WOULDBLOCK,
	SOC_CONNECT_RELEASE,             
	//出错
	SOC_CONN_ERROR = -1,          
	SOC_SEND_ERROR = -2,         
	SOC_RECV_ERROR = -3,        
	SOC_OTHER_ERROR = -4,         
}enum_socket_state;

typedef void (*socket_callback_func)(enum_socket_state state, void *soc_data, kal_int32 size);

//socket 数据
typedef struct __stu_network_soc{
	enum_socket_state statue;	//当前状态
	//kal_uint8	addr[MAX_SOCK_ADDR_LEN];
	kal_uint16	port; 
	kal_int8 fd;    
	mmi_sim_enum sim_id;//sim卡
	//soc_net_cb callback;   
	MMI_BOOL online;
	U32 account_id;
	U32 app_id;    
	//发送缓冲
	U8 send_data_buff[SOC_SEND_BUFFER_SIZE];
	U32 send_data_size;         
	//接收缓冲
	U8 recv_data_buff[SOC_RECV_BUFFER_SIZE];         
	U32 recv_data_size;   
	//设置一个回调函数，方便后面处理
	socket_callback_func callback;
}stu_socket_data;


void mmi_socket_init(mmi_sim_enum sim_id, socket_callback_func callback);
int mmi_socket_send(kal_char* addr, U32 port, void* send_data, U32 data_bytes);
enum_socket_state mmi_socket_get_statue(void);


#endif /* __SOCKET_H__ */


