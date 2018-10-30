#include "gui_themes.h"
#include "AppMain.h"
#include "mmi_rp_app_mycustom_def.h"
#include "Mmi_frm_scenario_gprot.h"
#include "GlobalResDef.h"
#include "mmi_cb_mgr_gprot.h"
#include "gdi_include.h"
#include "mdi_datatype.h"
#include "wgui_categories_util.h"
#include "mdi_audio.h"
#include "gpiosrvgprot.h"
#include "ReminderSrvGprot.h"
#include "DateTimeGprot.h"
#include "Fs_gprot.h"
#include "MMIDataType.h"
#include "Cbm_api.h"
#include "SimCtrlSrvGprot.h"
#include "CphsSrvGprot.h"
#include "NwInfoSrv.h"
#include "MMIDataType.h"
#include "kal_general_types.h"
#include "NwInfoSrvGprot.h"
#include "CharBatSrvGprot.h"

#include "MMIDataType.h"
#include "SppSrvGprot.h"
#include "BTMMISpp.h"
#include "mmi_frm_events_gprot.h"
#include "GlobalResDef.h"
#include "MMI_trc_Int.h"
#include "mmi_rp_app_bluetooth_def.h"
#include "mmi_frm_scenario_gprot.h"
#include "mmi_cb_mgr_gprot.h"

#include "FileData.h"
#include "BT.h"

#define MY_TEST_UUID 0x1101
#define MY_TEST_SPP_REJECT_CASE_FLAG 0x00000001

struct s_bt_config* p_config = NULL;

#if 0
typedef struct
{
    SppHandle sppsrv_hd;
    S32 conn_id;
    U32 flag;
    CHAR buf[100];
} sppc_cntx;

static sppc_cntx g_spp_cntx;
#endif




static void bt_cm_notifier(U32 event, void* para)
{
	srv_bt_cm_dev_struct *dev_info;
 	srv_bt_cm_power_status_enum bt_power_status;
	U8 pin_key[12] = {0};
	
	kal_prompt_trace(MOD_XDM, "-------test_bt_cm_notifier-------");
 	bt_power_status = srv_bt_cm_get_power_status();
	kal_prompt_trace(MOD_XDM, "bt_power_status = %d", bt_power_status);
	
	switch (event){
		
        case SRV_BT_CM_EVENT_ACTIVATE:{
		kal_prompt_trace(MOD_XDM, "event = SRV_BT_CM_EVENT_ACTIVATE");

		dev_info = srv_bt_cm_get_host_dev_info();
		//本机信息?
		kal_prompt_trace(MOD_XDM, "dev name [%s][0x%02x:%02x:%02x:%02x:%02x:%02x]", dev_info->name,
		((dev_info->bd_addr.nap & 0xff00) >> 8),
		(dev_info->bd_addr.nap & 0x00ff),
		(dev_info->bd_addr.uap),
		((dev_info->bd_addr.lap & 0xff0000) >> 16),
		((dev_info->bd_addr.lap & 0x00ff00) >> 8),
		(dev_info->bd_addr.lap & 0x0000ff));
		kal_prompt_trace(MOD_XDM, "SRV_BT_CM_EVENT_ACTIVATE end");
		//test_btspp_start();  //不需要

		//秘钥修改放到这里先
		//srv_bt_cm_pair_ind_struct * data = (srv_bt_cm_pair_ind_struct *)user_data;
		kal_prompt_trace(MOD_XDM, "test_passkey_input_cb");

		//strcpy((CHAR*)pin_key, "0000");
		strcpy((CHAR*)pin_key, p_config->bt_pwd);
		if (SRV_BT_CM_RESULT_SUCCESS != srv_bt_cm_send_passkey(&(dev_info->bd_addr), pin_key, MMI_TRUE))
		{
			kal_prompt_trace(MOD_XDM, "srv_bt_cm_send_passkey fail");
		}
		else
			kal_prompt_trace(MOD_XDM, "srv_bt_cm_send_passkey sucess");
		
		break;
        }
        case SRV_BT_CM_EVENT_DEACTIVATE:{
		kal_prompt_trace(MOD_XDM, "[LXZ]event = SRV_BT_CM_EVENT_DEACTIVATE");
		break;
        }
        case SRV_BT_CM_EVENT_SET_VISIBILITY:{
		kal_prompt_trace(MOD_XDM, "[LXZ]event = SRV_BT_CM_EVENT_SET_VISIBILITY");
            	break;
        }

        default:{
		kal_prompt_trace(MOD_XDM, "[LXZ]evt = %d", event);
            	break;
        }
    }
}

//修改密码
static void passkey_input_cb(void * user_data)
{
	srv_bt_cm_pair_ind_struct * data = (srv_bt_cm_pair_ind_struct *)user_data;
	U8 pin_key[12] = {0};

	kal_prompt_trace(MOD_XDM, "test_passkey_input_cb");

	strcpy((CHAR*)pin_key, p_config->bt_pwd);
	if (SRV_BT_CM_RESULT_SUCCESS != srv_bt_cm_send_passkey(&(data->dev_addr), pin_key, MMI_TRUE))
	{
		kal_prompt_trace(MOD_XDM, "[callback]:[test_passkey_input_cb]:SRV_BT_CM_RESULT_SUCCESS fail");
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "[callback]:[test_passkey_input_cb]:SRV_BT_CM_RESULT_SUCCESS sucess");
	}
	
}

static void mmi_bt_pair_result_cb(void * user_data)
{
	srv_bt_cm_pair_res_struct *data = (srv_bt_cm_pair_res_struct *)user_data;
	kal_prompt_trace(MOD_XDM, "[callback]:[test_passkey_input_cb]:mmi_bt_pair_result_cb");
	kal_prompt_trace(MOD_XDM, " data->result [%d]", data->result);    
	
	if (SRV_BT_CM_RESULT_SUCCESS == data->result)
	{
		
		U8 n = srv_bt_cm_get_paired_dev_num();
		kal_prompt_trace(MOD_XDM,"SRV_BT_CM_RESULT_SUCCESS");
		kal_prompt_trace(MOD_XDM,"vs_btcm_pair_result_cb paired num[%d]", n);
		if (0 < n)
		{
			S32 i = 0;
			for (i = 0; i < n; i++)
			{
				srv_bt_cm_dev_struct * dev_paired_p = NULL;
				dev_paired_p = srv_bt_cm_get_paired_dev_info(i);
				if (dev_paired_p)
				{
					kal_prompt_trace(MOD_XDM, "dev name=[%s] mac Addr:0x%02x:%02x:%02x:%02x:%02x:%02x", dev_paired_p->name,
					((dev_paired_p->bd_addr.nap & 0xff00) >> 8),
					(dev_paired_p->bd_addr.nap & 0x00ff),
					(dev_paired_p->bd_addr.uap),
					((dev_paired_p->bd_addr.lap & 0xff0000) >> 16),
					((dev_paired_p->bd_addr.lap & 0x00ff00) >> 8),
					(dev_paired_p->bd_addr.lap & 0x0000ff));
				}
				
			}
		}
		//VS_CASE_BTCM_FINISHED(MMI_TRUE, 0, L"vs_btcm_pair");
	}
	else
	{
		kal_prompt_trace(MOD_XDM,"SRV_BT_CM_RESULT_FAILED");
	}

	//DS_OUTLOG("vs_btcm_pair_result_cb E");        
}

void bt_cm_callback(U32 event, void* para)
 {	
 	srv_bt_cm_dev_struct *dev_info;
	srv_bt_cm_power_status_enum bt_power_status;
	srv_bt_cm_pair_res_struct *data = (srv_bt_cm_pair_res_struct *)para;
	
	kal_prompt_trace(MOD_XDM, "----------------test_bt_cm_callback---------------");
 	bt_power_status = srv_bt_cm_get_power_status();
	kal_prompt_trace(MOD_XDM, "bt_power_status = %d, ret= %d", bt_power_status, data->result);
	
	switch(event)
	{
		case SRV_BT_CM_EVENT_INQUIRY_IND:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_INQUIRY_IND");
			break;
		case SRV_BT_CM_EVENT_INQUIRY_COMPLETE:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_INQUIRY_COMPLETE");
			break;
		case SRV_BT_CM_EVENT_BEGIN_ACTIVATE:		// 1. 开始激活
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_BEGIN_ACTIVATE");
			break;
		case SRV_BT_CM_EVENT_BEGIN_DEACTIVATE:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_BEGIN_DEACTIVATE");
			break;
		case SRV_BT_CM_EVENT_BOND_RES:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_BOND_RES");
			break;
		case SRV_BT_CM_EVENT_PAIR_IND:		// 2. 匹配
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_PAIR_IND");
#if 1	//这里
			passkey_input_cb(para);
#endif
			break;
		case SRV_BT_CM_EVENT_PAIR_RES:		// 3. 匹配结束
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_PAIR_RES");
#if 1	//这里
			mmi_bt_pair_result_cb(para);
#endif
			break;
		case SRV_BT_CM_EVENT_SECURITY_USER_CONFIRM:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SECURITY_USER_CONFIRM");
			break;
		case SRV_BT_CM_EVENT_SECURITY_PASSKEY_NOTIFY:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SECURITY_PASSKEY_NOTIFY");
			break;
		case SRV_BT_CM_EVENT_SECURITY_KEYPRESS_NOTIFY:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SECURITY_KEYPRESS_NOTIFY");
			break;
		case SRV_BT_CM_EVENT_CONNECT_REQ:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_CONNECT_REQ");
			kal_prompt_trace(MOD_XDM, "vs_btcm_pair_result_cb data->result [%d]", data->result);        
			if (SRV_BT_CM_RESULT_SUCCESS == data->result)
			{
				U8 n = srv_bt_cm_get_paired_dev_num();
				kal_prompt_trace(MOD_XDM, "vs_btcm_pair_result_cb paired num[%d]", n);
				if (0 < n)
				{
					S32 i = 0;
					for (i = 0; i < n; i++)
					{
						srv_bt_cm_dev_struct * dev_paired_p = NULL;
						dev_paired_p = srv_bt_cm_get_paired_dev_info(i);
						if (dev_paired_p)
						kal_prompt_trace(MOD_XDM, "vs_btcm_pair_result_cb Addr:0x%02x:%02x:%02x:%02x:%02x:%02x", 
							((dev_paired_p->bd_addr.nap & 0xff00) >> 8),
							(dev_paired_p->bd_addr.nap & 0x00ff),
							(dev_paired_p->bd_addr.uap),
							((dev_paired_p->bd_addr.lap & 0xff0000) >> 16),
							((dev_paired_p->bd_addr.lap & 0x00ff00) >> 8),
							(dev_paired_p->bd_addr.lap & 0x0000ff));
					}
				}
				//VS_CASE_BTCM_FINISHED(MMI_TRUE, 0, L"vs_btcm_pair");
			}
			break;
		case SRV_BT_CM_EVENT_CONNECT_RES:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_CONNECT_RES");
			break;
		case SRV_BT_CM_EVENT_DISCONNECT_IND:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_DISCONNECT_IND");
			break;
		case SRV_BT_CM_EVENT_RELEASE_ALL_CONN:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_RELEASE_ALL_CONN");
			break;
		case SRV_BT_CM_EVENT_REFRESH_PROFILE:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_REFRESH_PROFILE");
			break;
		case SRV_BT_CM_EVENT_SET_AUTHORIZE:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SET_AUTHORIZE");
			break;
		case SRV_BT_CM_EVENT_UNBLOCK:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_UNBLOCK");
			break;
		case SRV_BT_CM_EVENT_MYDEV_DEL:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_MYDEV_DEL");
			break;
		case SRV_BT_CM_EVENT_SET_AUTHENTICATION:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SET_AUTHENTICATION");
			break;
		case SRV_BT_CM_EVENT_SET_SIMAP:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SET_SIMAP");
			break;
		case SRV_BT_CM_EVENT_SCO_IND:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SCO_IND");
			break;
		case SRV_BT_CM_EVENT_PANIC_IND:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_PANIC_IND");
			break;
		case SRV_BT_CM_EVENT_READ_REMOTE_DEV_NAME:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_READ_REMOTE_DEV_NAME");
			break;
		case SRV_BT_CM_EVENT_SET_LIMIT_DISCOVERABLE_MODE:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_SET_LIMIT_DISCOVERABLE_MODE");
			break;
		case SRV_BT_CM_EVENT_CHIP_DETECT:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_CHIP_DETECT");
			break;
		case SRV_BT_CM_EVENT_BOND_CANCEL:
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_BOND_CANCEL");
			break;

	}
}

//蓝牙初始化
void mmi_bt_open(struct s_bt_config* config)
{
	srv_bt_cm_power_status_enum bt_power_status;
	//U8 test_bt_name[] = "APLEXBT";
	S32 bt_status;
	srv_bt_cm_dev_struct *dev_info;
	srv_bt_cm_dev_struct* host_info;
	p_config = config;
	
	kal_prompt_trace(MOD_XDM, "p_config.bt_name=%s", p_config->bt_name);
	kal_prompt_trace(MOD_XDM, "p_config.bt_pwd=%s", p_config->bt_pwd);
	
	//memset(&g_spp_cntx , 0x00, sizeof(g_spp_cntx));
	srv_bt_cm_set_host_dev_name(p_config->bt_name);
	//设置回调函数
	srv_bt_cm_set_notify((srv_bt_cm_notifier)bt_cm_notifier, 
		SRV_BT_CM_EVENT_ACTIVATE |
        SRV_BT_CM_EVENT_DEACTIVATE |
        SRV_BT_CM_EVENT_SET_VISIBILITY |
        SRV_BT_CM_EVENT_SET_NAME, NULL);

	srv_bt_cm_set_notify((srv_bt_cm_notifier)bt_cm_callback, 
	    SRV_BT_CM_EVENT_INQUIRY_IND              |
	    SRV_BT_CM_EVENT_INQUIRY_COMPLETE     |      
	    SRV_BT_CM_EVENT_BEGIN_ACTIVATE        |    
	    SRV_BT_CM_EVENT_BEGIN_DEACTIVATE    |      
	    SRV_BT_CM_EVENT_BOND_RES                  |
	    SRV_BT_CM_EVENT_PAIR_IND                    |
	    SRV_BT_CM_EVENT_PAIR_RES          |
	    SRV_BT_CM_EVENT_SECURITY_USER_CONFIRM    |  
	    SRV_BT_CM_EVENT_SECURITY_PASSKEY_NOTIFY   |  
	    SRV_BT_CM_EVENT_SECURITY_KEYPRESS_NOTIFY   | 
	    SRV_BT_CM_EVENT_CONNECT_REQ               |
	    SRV_BT_CM_EVENT_CONNECT_RES                |
	    SRV_BT_CM_EVENT_DISCONNECT_IND        |
	    SRV_BT_CM_EVENT_RELEASE_ALL_CONN     |
	    SRV_BT_CM_EVENT_REFRESH_PROFILE  |
	    SRV_BT_CM_EVENT_SET_AUTHORIZE      |        
	    SRV_BT_CM_EVENT_UNBLOCK                  |   
	    SRV_BT_CM_EVENT_MYDEV_DEL                 |  
	    SRV_BT_CM_EVENT_SET_AUTHENTICATION   |      
	    SRV_BT_CM_EVENT_SET_SIMAP                  |
	    SRV_BT_CM_EVENT_SCO_IND                    |
	    SRV_BT_CM_EVENT_PANIC_IND                   |  
	    SRV_BT_CM_EVENT_READ_REMOTE_DEV_NAME |       
	    SRV_BT_CM_EVENT_SET_LIMIT_DISCOVERABLE_MODE |
	    SRV_BT_CM_EVENT_CHIP_DETECT                 |
	    SRV_BT_CM_EVENT_BOND_CANCEL  , NULL);

	//检测蓝牙电源情况
	bt_power_status = srv_bt_cm_get_power_status();
	kal_prompt_trace(MOD_XDM, "bt_power_status=%d", bt_power_status);
	
	if (bt_power_status == SRV_BT_CM_POWER_OFF) {
		kal_prompt_trace(MOD_XDM, "BT OFF");
		srv_bt_cm_set_host_dev_name(p_config->bt_name);
		srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
		
		bt_status = srv_bt_cm_switch_on();
		kal_prompt_trace(MOD_XDM, "bt_status = %d", bt_status);
	} else if (bt_power_status == SRV_BT_CM_POWER_ON) {
		srv_bt_cm_set_host_dev_name(p_config->bt_name);
		srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
 		
 		host_info = srv_bt_cm_get_host_dev_info();
		kal_prompt_trace(MOD_XDM, "host name=[%s] mac addr = [0x%02x:%02x:%02x:%02x:%02x:%02x]", host_info->name,
                ((host_info->bd_addr.nap & 0xff00) >> 8),
                (host_info->bd_addr.nap & 0x00ff),
                (host_info->bd_addr.uap),
                ((host_info->bd_addr.lap & 0xff0000) >> 16),
                ((host_info->bd_addr.lap & 0x00ff00) >> 8),
                (host_info->bd_addr.lap & 0x0000ff));
#if 0 //这里
			test_btspp_start();
#endif
		}
}

void mmi_bt_close()
{	
	//掉电
	srv_bt_cm_switch_off();
}



