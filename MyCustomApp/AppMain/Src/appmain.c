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

//我自己的
#include "Socket.h"
#include "GPS.h"
#include "FileData.h"
#include "cJSON.h"
#include "led.h"
#include "BT.h"
//mqtt部分
#include "MQTTFormat.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"
#include "StackTrace.h"
//蓝牙部分
#include "MMIDataType.h"
#include "SppSrvGprot.h"
#include "BTMMISpp.h"
#include "mmi_frm_events_gprot.h"
#include "GlobalResDef.h"
#include "MMI_trc_Int.h"
#include "mmi_rp_app_bluetooth_def.h"
#include "mmi_frm_scenario_gprot.h"
#include "mmi_cb_mgr_gprot.h"

typedef struct
{
	char *commandString;
	int (*func)(char *buf);
} s_bt_handler;

typedef struct
{
	char *status;
	char* (*func)(void);
} s_mqtt_handler;

typedef struct
{
	char *key;
	int (*func)(cJSON *body);
} s_prase_json_handler;

const s_bt_handler g_bt_handler_table[ ] =
{    
	{"time",bt_time_handler},
	{"date",bt_date_handler},
	{"serialnumber",bt_serial_number_handler},
	{"switch",bt_switch_onoff_handler},
	{NULL, NULL}
};

const s_mqtt_handler g_mqtt_handler_table[ ] =
{    
	{"init",mqtt_init_handler},
	{NULL, NULL}
};

const s_prase_json_handler g_prase_json_handler_table[ ] =
{    
	{"time",json_time_handler},
	{"BasicInfo",json_BasicInfo_handler},
	{"serialnumber",json_serial_number_handler},
	{NULL, NULL}
};

char mqtt_status[15];
cJSON * js_root;
char* mqtt_send_buff = NULL;


//配置
struct file_data* p_file_data = NULL;
struct s_bt_config* p_bt_config = NULL;
struct s_machine_config* p_machine_config = NULL;

static unsigned int current_switch_flag;

int bt_time_handler(char* buf)
{
	char* tmp = "i am time";
	kal_prompt_trace(MOD_XDM, "bt_time_handler:--%s", buf);
	send_bt_spp_byaplex(tmp, strlen(tmp));
	return 0;
}

int bt_date_handler(char* buf)
{
	char* tmp = "i am date";
	kal_prompt_trace(MOD_XDM, "bt_date_handler:--%s", buf);
	send_bt_spp_byaplex(tmp, strlen(tmp));
	return 0;
}

int bt_serial_number_handler(char* buf)
{
	kal_prompt_trace(MOD_XDM, "bt_serial_number_handler:--%s", buf);
	return 0;
}

int bt_switch_onoff_handler(char* buf)
{
	kal_prompt_trace(MOD_XDM, "bt_switch_onoff_handler:--%s", buf);
	return 0;
}


/////////////////JSON部分///////////////////////////////////
static void* get_prase_value(cJSON * body)
{
	kal_prompt_trace(MOD_XDM, "type=%d", body->type);	
	switch(body->type)
	{
		case cJSON_Number:
			return (void*)&body->valueint;
		case cJSON_String:
			kal_prompt_trace(MOD_XDM, "name value is %s",body->valuestring);
			return (void*)body->valuestring;
		case cJSON_Array:
		case cJSON_Object:
		default:
			break;
	}

	return NULL;
}

int json_time_handler(cJSON *body)
{
	cJSON *key = NULL;
	char* sValue = NULL;
	kal_prompt_trace(MOD_XDM, "------json_time_handler---------");	
	
	//从子元素里拿到键值对
	key  = cJSON_GetObjectItem(body, "time");
	if(key !=NULL)
		kal_prompt_trace(MOD_XDM, "get name ok");	
	else
		kal_prompt_trace(MOD_XDM, "get name fail");	

	//判断这个键值对的类型
	sValue = get_prase_value(key);

	if(sValue != NULL)
	{
		kal_prompt_trace(MOD_XDM, "time value is %s",sValue);
	}

	key  = cJSON_GetObjectItem(body, "date");
	if(key !=NULL)
		kal_prompt_trace(MOD_XDM, "get name ok");	
	else
		kal_prompt_trace(MOD_XDM, "get name fail");	

	//判断这个键值对的类型
	sValue = get_prase_value(key);

	if(sValue != NULL)
	{
		kal_prompt_trace(MOD_XDM, "date value is %s",sValue);
	}

	return 0;
}


int json_BasicInfo_handler(cJSON *body)
{
	cJSON *key = NULL;
	char* sValue = NULL;
	int * iValue = NULL;
	kal_prompt_trace(MOD_XDM, "------json_BasicInfo_handler---------");	
	
	//从子元素里拿到键值对
	key  = cJSON_GetObjectItem(body, "name");
	if(key !=NULL)
		kal_prompt_trace(MOD_XDM, "get name ok");	
	else
		kal_prompt_trace(MOD_XDM, "get name fail");	

	//判断这个键值对的类型
	sValue = get_prase_value(key);

	if(sValue != NULL)
	{
		kal_prompt_trace(MOD_XDM, "name value is %s",sValue);
	}

	key  = cJSON_GetObjectItem(body, "serial_number");
	if(key !=NULL)
		kal_prompt_trace(MOD_XDM, "get serial_number ok");	
	else
		kal_prompt_trace(MOD_XDM, "get serial_number fail");	

	//判断这个键值对的类型
	sValue = get_prase_value(key);

	if(sValue != NULL)
	{
		kal_prompt_trace(MOD_XDM, "serial_number value is %s",sValue);
	}

	key  = cJSON_GetObjectItem(body, "version");
	if(key !=NULL)
		kal_prompt_trace(MOD_XDM, "get version ok");	
	else
		kal_prompt_trace(MOD_XDM, "get version fail");	

	//判断这个键值对的类型
	iValue = get_prase_value(key);

	if(iValue != NULL)
	{
		kal_prompt_trace(MOD_XDM, "version value is %d",*iValue);
	}
	return 0;
}

int json_serial_number_handler(cJSON *body)
{
	return 0;
}

void prase_json(kal_char* buf)
{
	cJSON *body = NULL;
	cJSON *name = NULL;
	cJSON *value = NULL;
	char* statusString = NULL;
	//拿到根对象
	cJSON *root = cJSON_Parse((char *)buf);
	int i = 0;
	
	if(root!=NULL)
	{
		kal_prompt_trace(MOD_XDM, "cJSON_Parse ok");	
	}
	else
		kal_prompt_trace(MOD_XDM, "cJSON_Parse fail");	


	for (i = 0 ; g_prase_json_handler_table[i].key != NULL; i++ )
	{
		statusString = g_prase_json_handler_table[i].key;
		//拿到子元素
		body = cJSON_GetObjectItem(root, statusString);
		if(body!=NULL)
		{
			kal_prompt_trace(MOD_XDM, "get body ok, body=%s", g_prase_json_handler_table[i].key);	
			g_prase_json_handler_table[i].func(body) ;
		}
		else
		{
			kal_prompt_trace(MOD_XDM, "get body fail, body=%s", g_prase_json_handler_table[i].key);
		}
			
	}
}

char* mqtt_init_handler(void)
{
	cJSON * js_body1 =  NULL;
	cJSON * js_body2 =  NULL;
	cJSON * js_body3 =  NULL;
	cJSON * js_body4 =  NULL;
	const char *const body = "BasicInfo";
	const char *const body2 = "time";
	const char *const body3 = "time2";
	const char *const body4 = "time3";
	
	MYTIME curr_time={0x00};
	U8 data[15] = {0x00};		
	
	//创建一个json对象
	js_root = cJSON_CreateObject();

	//在对象中插入第一个元素
	js_body1 = cJSON_CreateObject();
	cJSON_AddItemToObject(js_root, body, js_body1);
	cJSON_AddStringToObject(js_body1,"name","aplex");
	cJSON_AddStringToObject(js_body1,"serial_number",p_file_data->machine_serial_number);
	cJSON_AddNumberToObject(js_body1,"version", 1);

	//在对象中插入第二个元素
	GetDateTime(&curr_time);
	//sprintf(data, "%04d/%02d/%02d %02d:%02d:%02d", curr_time.nYear,curr_time.nMonth,curr_time.nDay,curr_time.nHour,curr_time.nMin,curr_time.nSec);
	memset(data, 0x0, sizeof(data));
	sprintf(data, "%04d/%02d/%02d", curr_time.nYear,curr_time.nMonth,curr_time.nDay);
	
	js_body2 = cJSON_CreateObject();
	cJSON_AddItemToObject(js_root, body2, js_body2);
	cJSON_AddStringToObject(js_body2,"date",data);

	memset(data, 0x0, sizeof(data));
	sprintf(data, "%02d:%02d:%02d", curr_time.nHour,curr_time.nMin,curr_time.nSec);
	cJSON_AddStringToObject(js_body2,"time",data);

	//测试
	js_body3 = cJSON_CreateObject();
	cJSON_AddItemToObject(js_root, body3, js_body3);
	cJSON_AddStringToObject(js_body3,"date3",data);

	memset(data, 0x0, sizeof(data));
	sprintf(data, "%02d:%02d:%02d", curr_time.nHour,curr_time.nMin,curr_time.nSec);
	cJSON_AddStringToObject(js_body3,"time3",data);

	// 4
	js_body4 = cJSON_CreateObject();
	cJSON_AddItemToObject(js_root, body4, js_body4);
	cJSON_AddStringToObject(js_body4,"date3",data);

	memset(data, 0x0, sizeof(data));
	sprintf(data, "%02d:%02d:%02d", curr_time.nHour,curr_time.nMin,curr_time.nSec);
	cJSON_AddStringToObject(js_body4,"time3",data);


	
	//转换成字符串
	mqtt_send_buff = cJSON_PrintUnformatted(js_root);
	cJSON_Delete(js_root);
	
	return mqtt_send_buff;
}

static char* mqtt_pack()
{
	int i = 0;
	char* statusString = NULL;
	
	for (i = 0 ; g_mqtt_handler_table[i].status != NULL; i++ )
	{
		statusString = g_mqtt_handler_table[i].status;
		if (strcmp(mqtt_status, statusString) == 0 )
		{
			return g_mqtt_handler_table[i].func() ;
		}
	}
	return NULL;
}

//拆包
static void mqtt_unpack()
{

}

static mqtt_set_status(char* status)
{
	memset(mqtt_status, 0x0, sizeof(mqtt_status));
	strcpy(mqtt_status, status);
}

/**	蓝牙部分包**/
//组包
static void bt_pack()
{

}

//拆包
static void bt_unpack(char* buf)
{
	int index = 0;
	int i;
	int length = strlen(buf);
	char cmd_name[20+1]; 
	char* cmdString = NULL;
	
	while ((buf[index] != '=' ) && index < length)  
	{
		cmd_name[index] = buf[index] ;
		index ++;
	}
	cmd_name[index] = '\0' ;    
	kal_prompt_trace(MOD_XDM, "cmd_name=%s", cmd_name);

	for (i = 0 ; g_bt_handler_table[i].commandString != NULL; i++ )
	{
		cmdString = g_bt_handler_table[i].commandString;
		if (strcmp(cmd_name, cmdString) == 0 )
		{
			if (g_bt_handler_table[i].func(buf+index+1) == 0) 
			{
				kal_prompt_trace(MOD_XDM, "OK");
			}
			else
			{
				kal_prompt_trace(MOD_XDM, "ERROR");
			}
			return ;
		}
	}
}

void upload_by_mqtt()
{
	char* sendbuff = NULL;
	
	sendbuff = mqtt_pack();
	//publish_func();
	if(sendbuff != NULL)
		publish_message_func(sendbuff);

	StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 15000, upload_by_mqtt);
#if 0	
	if(p_machine_config->ref_time > 0)	
		StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, p_machine_config->ref_time, upload_by_mqtt);
	else
		StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 5000, upload_by_mqtt);
#endif
}

void gps_get()
{
	U8 temp[128]={0x00};
	stu_gps_data *gps_data=NULL;
	
	if(switch_flag_get(current_switch_flag, SWITCH_FLAG_GPS)==SWITCH_ON)
	{
		gps_data = mmi_gps_get_data();	//拿到GPS数据
		sprintf((char*)temp,"gps(%f,%f)",gps_data->latitude, gps_data->longitude);         
		kal_prompt_trace(MOD_XDM, "my gps = %s\n", temp);
	}
}

//配置模式，全开
static void config_all(void)
{
	//GPRS
	StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 1000, upload_by_mqtt);
	switch_flag_on(current_switch_flag, SWITCH_FLAG_GPRS);
	//GPS
	mmi_gps_open();
	StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 1000, gps_get);
	switch_flag_on(current_switch_flag, SWITCH_FLAG_GPS);
	//蓝牙
	switch_flag_on(current_switch_flag, SWITCH_FLAG_BT);
	mmi_bt_open(p_bt_config);

 }

 static void config_init(void)
 {
	mqtt_init();	//
	//云端上送
	mqtt_set_status("init");
	StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 5000, upload_by_mqtt);
	switch_flag_on(current_switch_flag, SWITCH_FLAG_GPRS);
#if 0
 	//开GPRS
 	if(switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_GPRS)==SWITCH_ON)
	{
		kal_prompt_trace(MOD_XDM, "OPEN GPRS");
		mqtt_init();	//
		//云端上送
		mqtt_set_status("init");
		StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 1000, upload_by_mqtt);
		switch_flag_on(current_switch_flag, SWITCH_FLAG_GPRS);
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "CLOSE GPRS");
		switch_flag_off(current_switch_flag, SWITCH_FLAG_GPRS);
	}
	//开GPS
	if(switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_GPS)==SWITCH_ON)
	{
		kal_prompt_trace(MOD_XDM, "OPEN GPS");
		mmi_gps_open();
		//GPS
		StartTimer(TIMER_ID_MY_CUSTOM_MQTT1, 1000, gps_get);
		switch_flag_on(current_switch_flag, SWITCH_FLAG_GPS);
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "CLOSE GPS");
		mmi_gps_close();
		switch_flag_off(current_switch_flag, SWITCH_FLAG_GPS);
	}
	//开蓝牙
	if(switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_BT)==SWITCH_ON)
	{
		kal_prompt_trace(MOD_XDM, "OPEN BT");
		switch_flag_on(current_switch_flag, SWITCH_FLAG_BT);
		mmi_bt_open(p_bt_config);
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "CLOSE BT");
		mmi_bt_close();
		switch_flag_off(current_switch_flag, SWITCH_FLAG_BT);
	}
#endif
 }

//接收到的蓝牙
void mmi_prase_bt_data(char* buf)
{
	kal_prompt_trace(MOD_XDM, "prase:%s\n", buf);
	bt_unpack(buf);//拆包
}

void main_task_start(void)
{
	kal_prompt_trace(MOD_XDM, "-------------main_task_start-------------");
	init_data();

	p_file_data = get_file_data();
	p_bt_config = get_bt_config_data();
	p_machine_config = get_machine_config_data();

	if(p_file_data->isTestMode == 0)	//工作模式
		config_init();	//根据配置进行初始化
	else
		config_all();
#if 0
	led_init();
#endif
}
