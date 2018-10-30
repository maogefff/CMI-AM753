#include "mmi_features.h"

#include "cbm_consts.h"
#include "app2soc_struct.h"
#include "soc_api.h"
#include "DtcntSrvGprot.h"
#include "mmi_frm_events_gprot.h"
#include "mmi_frm_mem_gprot.h"
#include "mmi_frm_nvram_gprot.h"
#include "cbm_api.h"

#include "Socket.h"
#include "cJSON.h"


static stu_socket_data g_socket_data={0x00};
static void mmi_socket_release(void);
static void mmi_socket_soc_notify_handle(void* msg);

MMI_BOOL mmi_socket_get_online(void)
{
    return g_socket_data.online;
}

void mmi_socket_set_online(MMI_BOOL status)
{
    g_socket_data.online = status;
}

void mmi_socket_set_statue(enum_socket_state statue)
{
	g_socket_data.statue = statue;
}

enum_socket_state mmi_socket_get_statue(void)
{
	return g_socket_data.statue;
}


static S32 mmi_socket_soc_send_data(void)
{    
	S32 send_bytes=0;    

	if(g_socket_data.fd<0 || 0==g_socket_data.send_data_size)
	{     
		return 0;
	}
	 
	send_bytes = soc_send(g_socket_data.fd, g_socket_data.send_data_buff, g_socket_data.send_data_size, 0);            
	if(send_bytes > 0)
	{
		kal_prompt_trace(MOD_XDM, "send sucess!!");
		g_socket_data.statue = SOC_STATE_SEND_SUCCESS;
		g_socket_data.callback(SOC_STATE_SEND_SUCCESS, (void*)g_socket_data.send_data_buff, send_bytes);
	}
	else if(SOC_WOULDBLOCK==send_bytes)
	{        
		g_socket_data.statue = SOC_STATE_WOULDBLOCK;
		mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND,mmi_socket_soc_notify_handle, MMI_TRUE);
	}
	else
	{
		g_socket_data.statue = SOC_SEND_ERROR;
		kal_prompt_trace(MOD_XDM, "send error!!");
	}
	return send_bytes;    
}


static S32 mmi_socket_soc_recv_data(void)
{
	S32 recv_bytes = -1;
	U8 *recv_buff_ptr = NULL, *temp_buff=NULL, *p1=NULL, *p2=NULL;

	memset(g_socket_data.recv_data_buff,0x00,SOC_RECV_BUFFER_SIZE);
	g_socket_data.recv_data_size = 0;
	recv_bytes = soc_recv(g_socket_data.fd, g_socket_data.recv_data_buff, SOC_RECV_BUFFER_SIZE, 0);

	if(recv_bytes > 0)
	{
		g_socket_data.recv_data_size = recv_bytes;

		kal_prompt_trace(MOD_XDM, "read sucess, recv size=%d", recv_bytes);
		
		g_socket_data.callback(SOC_STATE_RECV_SUCCESS, (void*)g_socket_data.recv_data_buff, recv_bytes);

	}
	else
	{
		if(SOC_WOULDBLOCK==recv_bytes)
		{
			mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND,mmi_socket_soc_notify_handle, MMI_TRUE);
		}
		else 
		{
			kal_prompt_trace(MOD_XDM, "read error");
		}
	}

	return recv_bytes;
}


static void mmi_socket_soc_notify_handle(void* msg)
{
	app_soc_notify_ind_struct *soc_notify = (app_soc_notify_ind_struct*)msg;
	kal_int8 result = 0;
	//kal_prompt_trace(MOD_XDM, "mmi_socket_soc_notify_handle 1");
	if(NULL == soc_notify)
	{
		kal_prompt_trace(MOD_XDM, "mmi_socket_soc_notify_handle 2");
		return;
	}

	if(SOC_BEARER_FAIL == soc_notify->error_cause)
	{
		kal_prompt_trace(MOD_XDM, "mmi_socket_soc_notify_handle 3");
		mmi_socket_release();
		return;
	}
	kal_prompt_trace(MOD_XDM, "mmi_socket_soc_notify_handle 4 soc_notify->event_type=%d", soc_notify->event_type);
	//有数据发送的时候，标志 GPRS 为激活状态
	mmi_socket_set_online(MMI_TRUE);

	switch (soc_notify->event_type)
	{
		case SOC_READ:
		{
			mmi_socket_soc_recv_data();
			break;
		}	        
		case SOC_CONNECT:
		case SOC_WRITE:
		{
			mmi_socket_soc_send_data();
			break;
		}	
		case SOC_CLOSE:
		{
			mmi_socket_release();
			break;
		}	
		default:
		{
			break;
		}
	}
	//kal_prompt_trace(MOD_XDM, "mmi_socket_soc_notify_handle 5");

}

static void mmi_socket_get_host_name_cb(void *msg)
{
	kal_int32 result = 0;
	app_soc_get_host_by_name_ind_struct* dns_ind = NULL;
	sockaddr_struct server_addr = {0x00};
	kal_prompt_trace(MOD_XDM, "mmi_socket_get_host_name_cb");
	if (msg == NULL)
	{                
		return;
	}    

	dns_ind = (app_soc_get_host_by_name_ind_struct *)msg;
	mmi_frm_clear_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND, (PsIntFuncPtr)mmi_socket_get_host_name_cb);	

	//注意:!!!!!!!!!!!!!!!!!!!!!!!!!!!，这里是我刚才去掉的
	//mmi_socket_get_soc_addr_msg(&server_addr, NULL);
	mmi_socket_insert_port(&server_addr, g_socket_data.port);
	if (dns_ind->result == KAL_TRUE)
	{
		memcpy((char *)&server_addr.addr, (char*)dns_ind->addr, dns_ind->addr_len);
		server_addr.addr_len = dns_ind->addr_len;
	}
    	kal_prompt_trace(MOD_XDM, "ip:%d.%d.%d.%d, len=%d", server_addr.addr[0],server_addr.addr[1],server_addr.addr[2],server_addr.addr[3], server_addr.addr_len);
	result = soc_connect(dns_ind->request_id, &server_addr);
	
	if(result==SOC_SUCCESS)
	{
		kal_prompt_trace(MOD_XDM, "get host name soc_connect sucess");
		mmi_socket_soc_send_data();
		return;
	}
	else if (result == SOC_WOULDBLOCK)
	{        
		kal_prompt_trace(MOD_XDM, "mmi_socket_get_host_name_cb2");
		mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND,mmi_socket_soc_notify_handle, MMI_TRUE);
		return;
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "mmi_socket_get_host_name_cb3: error");
		//if(NULL != g_socket_data.callback)
		{
			//g_socket_data.callback(SOC_CONNECT_FAILED,&g_socket_data);
		}
		mmi_socket_release();        
		return;
	}
}

U32 mmi_socket_get_account_id(void)
{
	cbm_app_info_struct app_info = {0x00};
	cbm_sim_id_enum sim = CBM_SIM_ID_TOTAL;
	kal_int8 ret;

	if(0 == g_socket_data.account_id)
	{
		switch(g_socket_data.sim_id)
		{
			case MMI_SIM1:
			{
				sim = CBM_SIM_ID_SIM1;
				break;
			}
			case MMI_SIM2:
			{
				sim = CBM_SIM_ID_SIM2;
				break;
			}
			case MMI_SIM3:
			{
				sim = CBM_SIM_ID_SIM3;        
				break;
			}
			case MMI_SIM4:
			{
				sim = CBM_SIM_ID_SIM4;        
				break;
			}
			default:
			{
				return 0;
			}
		}

		app_info.app_icon_id = 0;
		app_info.app_str_id = 0;
		app_info.app_type = DTCNT_APPTYPE_BRW_HTTP|DTCNT_APPTYPE_MRE_WAP|DTCNT_APPTYPE_MRE_NET|DTCNT_APPTYPE_DEF;
		//生成app id
		g_socket_data.app_id = CBM_INVALID_APP_ID;
		ret = cbm_register_app_id_with_app_info(&app_info, (U8*)&g_socket_data.app_id);
		
		if (ret != CBM_OK)
		{
			kal_prompt_trace(MOD_XDM, "app id faile appid =%d", g_socket_data.app_id);		
			return KAL_FALSE;
		}

		//cbm_hold_bearer
		kal_prompt_trace(MOD_XDM, "sim id=%d", sim);		
		kal_prompt_trace(MOD_XDM, "app id =%d", g_socket_data.app_id);		
		//g_socket_data.account_id = cbm_encode_data_account_id(CBM_DEFAULT_ACCT_ID, sim, g_socket_data.app_id, MMI_FALSE); 
		g_socket_data.account_id=cbm_encode_data_account_id(g_socket_data.account_id,g_socket_data.sim_id,g_socket_data.app_id,KAL_FALSE);
		kal_prompt_trace(MOD_XDM, "account_id =%d", g_socket_data.account_id);		

	}

	return  g_socket_data.account_id;
}

static kal_int8 mmi_socket_create(const U32 account_id)
{
	U8 socket_opt = 1;
	kal_int8 fd  = -1;

	fd = soc_create(SOC_PF_INET, SOC_SOCK_STREAM, 0, MOD_MMI, account_id);
	if (fd < 0)
	{
		return KAL_FALSE;
	}

	socket_opt = KAL_TRUE;
	if (soc_setsockopt(fd, SOC_NBIO, &socket_opt, sizeof(socket_opt)) < 0)
	{
		return KAL_FALSE;
	}
	socket_opt = SOC_READ | SOC_WRITE | SOC_CONNECT | SOC_CLOSE;
	if (soc_setsockopt(fd, SOC_ASYNC, &socket_opt, sizeof(socket_opt)) < 0)
	{
		return KAL_FALSE;
	}

	return fd;
}


static void mmi_socket_release(void)
{
	mmi_socket_set_online(MMI_FALSE);

	if(g_socket_data.fd >= 0)
	{
		soc_close(g_socket_data.fd);
		g_socket_data.fd = -1;
	}

	memset(g_socket_data.send_data_buff, 0x00, SOC_SEND_BUFFER_SIZE);
	g_socket_data.send_data_size = 0;

	memset(g_socket_data.recv_data_buff, 0x00, SOC_RECV_BUFFER_SIZE);
	g_socket_data.recv_data_size = 0;    
}


static int mmi_socket_insert_port(sockaddr_struct *soc_addr, kal_uint16 port)
{
	if(soc_addr == NULL)
	{
		return KAL_FALSE;
	}

	soc_addr->sock_type = SOC_SOCK_STREAM;
	soc_addr->port = port;	
	soc_addr->addr_len = 0x04;
	
	return KAL_TRUE;   
}


static MMI_BOOL mmi_socket_set_send_content(void *send_data, U32 data_bytes)
{
	if(NULL==send_data || 0==data_bytes || data_bytes>SOC_SEND_BUFFER_SIZE)
	{
		return MMI_FALSE;
	}
	memset(g_socket_data.send_data_buff,0x00,SOC_SEND_BUFFER_SIZE);    
	memcpy(g_socket_data.send_data_buff,send_data,data_bytes);    
	g_socket_data.send_data_size = data_bytes;

	return MMI_TRUE;
}

/*
void mmi_socket_set_callback(soc_net_cb soc_cb)
{
    //g_socket_data.callback = soc_cb;
}
*/

int mmi_socket_send(kal_char* addr, U32 port, void* send_data, U32 data_bytes)
{

	sockaddr_struct soc_addr = {0x00};
	kal_bool ip_validity = KAL_FALSE;
	S32 result = 0;
	kal_char server_addr[128]={0x00};

	if(MMI_FALSE == mmi_socket_set_send_content(send_data, data_bytes))
	{
		return 1;
	}
	//如果没有释放，直接发送数据
	if(g_socket_data.fd >= 0)
	{
		if(MMI_FALSE == mmi_socket_set_send_content(send_data, data_bytes))
		{
			return 2;
		}
		
		result = mmi_socket_soc_send_data();
		
		if(result < 0)
		{
			mmi_socket_release();
		}
		else
		{
			return 3;
		}
	}
	//填充ip端口
	mmi_socket_insert_port(&soc_addr, port);
	g_socket_data.port = port;
	//第1步:获取account id
	g_socket_data.account_id = mmi_socket_get_account_id();
	kal_prompt_trace(MOD_XDM, "g_socket_data.account_id=%d", g_socket_data.account_id);
	//第2步: 创建 socket id
	g_socket_data.fd = mmi_socket_create(g_socket_data.account_id );
	kal_prompt_trace(MOD_XDM, "g_socket_data.fd=%d", g_socket_data.fd);

	if(g_socket_data.fd < 0)
	{
		mmi_socket_release();
		return 4;
	}

	//第3步: connect
	if(soc_ip_check(addr, soc_addr.addr, &ip_validity)==KAL_FALSE || KAL_FALSE==ip_validity)
	{
		result = soc_gethostbyname(KAL_FALSE,MOD_MMI, g_socket_data.fd, addr, soc_addr.addr, (kal_uint8 *)&soc_addr.addr_len,0,g_socket_data.account_id);
		kal_prompt_trace(MOD_XDM, "addr1=%d, 2=%d, 3=%d, 4=%d, len=%d", soc_addr.addr[0],soc_addr.addr[1],soc_addr.addr[2],soc_addr.addr[3], soc_addr.addr_len);
		if (SOC_SUCCESS != result)
		{            
			if (result == SOC_WOULDBLOCK)
			{            
				mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND, (PsIntFuncPtr)mmi_socket_get_host_name_cb, TRUE);                
				return 6;
			}
			else/*连接出错, 释放 socket*/
			{
				mmi_socket_release();
				return 7;
			}
		}
	}
	kal_prompt_trace(MOD_XDM, "addr2=%d, 2=%d, 3=%d, 4=%d, len=%d", soc_addr.addr[0],soc_addr.addr[1],soc_addr.addr[2],soc_addr.addr[3], soc_addr.addr_len);
	result = soc_connect(g_socket_data.fd, &soc_addr);/*连接网络*/
	if(result==SOC_SUCCESS)
	{        
		mmi_socket_soc_send_data();
		return 8;
	}   
	else if (SOC_WOULDBLOCK == result)	//一般是进入这里，一旦连接上，调用回调函数处理
	{
		mmi_frm_set_protocol_event_handler(MSG_ID_APP_SOC_NOTIFY_IND,mmi_socket_soc_notify_handle, MMI_TRUE);
		return 9;
	}
	else
	{
		mmi_socket_release();
	}

	return 10;

}

void mmi_socket_init(mmi_sim_enum sim_id, socket_callback_func callback)
{
	memset(&g_socket_data, 0x00, sizeof(g_socket_data));
	g_socket_data.fd = -1;
	g_socket_data.sim_id = MMI_SIM1;
	g_socket_data.statue = SOC_STATE_START;
	g_socket_data.callback = callback;
}



