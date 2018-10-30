
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
#if 1
#if 1
#include "vmopt.h"
#include "vmsys.h"
#include "vmsim.h"
#include "vmlog.h"
#include "vmpromng.h"

#include "vmenv.h"
#include "ModeSwitchSrvGProt.h"
#include "SimCtrlSrvGprot.h"
#include "ImeiSrvGprot.h"

#include "CphsSrvGprot.h"

#include "sim_public_enum.h"
#include "simaccesssrvgprot.h"
#include "DtcntSrvGprot.h"
#include "SmsSrvGprot.h"

#include "NwInfoSrv.h"
#include "MMIDataType.h"
#include "kal_general_types.h"
#include "NwInfoSrvGprot.h"
#include "CharBatSrvGprot.h"
#endif
//TASK的
#if 1
#include "kal_trace.h"
#include "stack_msgs.h"         /* enum for message IDs */
#include "app_ltlcom.h"         /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "dcl.h"
#include "gps_main.h"
#include "gps_init.h"
#include "gps_sm.h"
#include "gps_trc.h"
#include "stack_ltlcom.h"
#include "stack_config.h"
#include "kal_general_types.h"
#include "kal_public_api.h"
#include "kal_internal_api.h"
#endif

//我自己的
#include "Socket.h"
#include "GPS.h"
#include "FileData.h"
//#include "mqtt.h"
#include "cJSON.h"
#include "led.h"
#include "BT.h"
//mqtt部分
//#include "transport.h"
//#include "MQTTConnect.h"
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

#endif
#if 1
//测试变量
//static mqtt_broker_handle_t broker;
//mqtt的通信变量
g_mqtt_conn mqtt;
MQTTPacket_connectData mqtt_socket_data = MQTTPacket_connectData_initializer;
MQTTString mqtt_socket_topicString = MQTTString_initializer;


void test_sim();

//文件测试
void mmi_file_test_write()
{
	char dir[30];
	char file[50];
	char tmp[11];
	int fd;
	static int i = 0;
	int wl;

	i++;
	memset(dir, 0x00, sizeof(dir));
	memset(file, 0x00, sizeof(file));
	memset(tmp, 0x00, sizeof(tmp));

	
	//创建文件夹
	//kal_wsprintf((unsigned short *)dir, "%c:\\%w", SRV_FMGR_SYSTEM_DRV, L"appData");
	//fd = FS_CreateDir((WCHAR *)dir);

	//创建文件
	kal_wsprintf((unsigned short *)file, "%c:\\appData.txt", SRV_FMGR_SYSTEM_DRV);
	fd = FS_Open((WCHAR *)file, FS_READ_WRITE | FS_CREATE);
	if(fd<0)
	{
		memset(file, 0x00, sizeof(file));
		kal_prompt_trace(MOD_MMS, "file1 open write error:%d\n", fd);
	}else
		kal_prompt_trace(MOD_MMS, "file1 open write success:%d\n", fd);
	
	//写文件
	sprintf(tmp, "test data = %d", i);
	FS_Write(fd, tmp, strlen(tmp), &wl);
	kal_prompt_trace(MOD_MMS, "file write len = %d\n", wl);
	//关闭
	FS_Close(fd);
	
}

void mmi_file_test_read()
{
	char file[50];
	char tmp[11];
	int fd;
	int rl;
	
	memset(file, 0x00, sizeof(file));
	memset(tmp, 0x00, sizeof(tmp));
	
	kal_wsprintf((unsigned short *)file, "%c:\\appData.txt", SRV_FMGR_SYSTEM_DRV);
	//打开文件
	fd = FS_Open((WCHAR *)file, FS_READ_ONLY );
	if(fd<0)
	{
		kal_prompt_trace(MOD_MMS, "file open read error:%d\n", fd);
	}
	//读文件
	FS_Read(fd, tmp, sizeof(tmp), &rl);
	kal_prompt_trace(MOD_MMS, "file read len = %d\n", rl);
	kal_prompt_trace(MOD_MMS, "read data:%s\n", tmp);
	//关闭
	FS_Close(fd);
}

void mmi_fresh_count()
{
	static int i = 0;
	U8  teststr[40] = {0x00};	
	i++;
	
	//mmi_file_test_write();
	//mmi_file_test_read();
	memset(teststr, 0x00, sizeof(teststr));

	kal_wsprintf((unsigned short *) teststr, "aplex count=%d", i);
	gui_set_text_color(UI_COLOR_WHITE);	//设置字体颜色为蓝色
	gui_move_text_cursor(25, 30);		//设置字体位置
	gui_set_font(&MMI_medium_font);	//设置字体大小
	//gui_print_text((UI_string_type)L"welcome TEST 1");
	gui_print_text((UI_string_type)teststr);

	gui_BLT_double_buffer(0, 0, UI_DEVICE_WIDTH, UI_DEVICE_HEIGHT);//设置分辨率
	
	//MMI_TRACE(MMI_FW_TRC_G1_FRM, TRC_MMI_FRM_TIMER_START_ALIGN_TIMER, timerid, delay, funcPtr);
	kal_prompt_trace(MOD_MMS, "aplex count=%d\n", i);
	//定时器
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, mmi_fresh_count);

	//文件测试
	mmi_file_test_write();
	mmi_file_test_read();

}

void test_timer(void)
{
	kal_prompt_trace(MOD_MMI, "aplex mmi_my_mtk_test_timer\n");
	//使用定时器
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, mmi_fresh_count);
}


void time_callback(MMI_BOOL tmp1, void* tmp2)
{
	MYTIME curr_time={0x00};
	U8 soc_data[64] = {0x00};		
	kal_prompt_trace(MOD_XDM, "[time]time is aleady setting");
	GetDateTime(&curr_time);
	sprintf(soc_data, "%04d/%02d/%02d %02d:%02d:%02d", curr_time.nYear,curr_time.nMonth,curr_time.nDay,curr_time.nHour,curr_time.nMin,curr_time.nSec);
	kal_prompt_trace(MOD_XDM, "[time]time data = %s\n", soc_data);
}

void test_file()
{
	mmi_file_test_write();
	mmi_file_test_read();
}

void test_gps()
{
	U8 temp[128]={0x00};
	stu_gps_data *gps_data=NULL;
	kal_prompt_trace(MOD_XDM, "---------------gps test------------------------");
	
	
	kal_prompt_trace(MOD_XDM, "test_gps");
	mmi_gps_open();
	kal_prompt_trace(MOD_XDM, "test_gps111");
	gps_data = mmi_gps_get_data();	//拿到GPS数据
	sprintf((char*)temp,"gps(%f,%f)",gps_data->latitude, gps_data->longitude);         
	kal_prompt_trace(MOD_XDM, "my gps = %s\n", temp);
	kal_prompt_trace(MOD_XDM, "test_gps222");
}

void test_time()
{
	MYTIME curr_time={0x00}; 	//时间
	kal_prompt_trace(MOD_XDM, "---------------time test------------------------");
	curr_time.nYear = 2014;
	curr_time.nMonth = 6;
	curr_time.nDay = 4;
	curr_time.nHour = 12;
	curr_time.nMin = 34;
	curr_time.nSec = 56;
	//设置时间
	mmi_dt_set_dt((const MYTIME *)&curr_time, time_callback, NULL);

}

void socket_test_callback(enum_socket_state state, void *data_buff, kal_int32 size)
{
	kal_prompt_trace(MOD_XDM, "socket_test_callback  state=%d", state);

	switch(state)
	{
		case SOC_STATE_SEND_SUCCESS:
			break;
		case SOC_STATE_RECV_SUCCESS:
			kal_prompt_trace(MOD_XDM, "recv data = %s", data_buff);
			break;
	}
}


void test_socket()
{
	//ip连接测试
	kal_char dest_ip[]="47.98.195.146";	
	kal_uint16 port_ip = 4328;              
	//域名连接测试
	kal_char dest_dname[]="www.baidu.com";	
	kal_uint16 port_dname = 80;         
	kal_char send_buf[] = "hello aplex";
	int ret = 0;

	mmi_socket_init(MMI_SIM1, socket_test_callback);
#if 1
	kal_prompt_trace(MOD_XDM, "--------ip socket test----------");
	ret = mmi_socket_send(dest_ip, port_ip, send_buf, strlen(send_buf)+1);
	kal_prompt_trace(MOD_XDM, "ret = %d", ret);
#endif
	kal_prompt_trace(MOD_XDM, "--------domain name socket test----------");
	ret = mmi_socket_send(dest_dname, port_dname, send_buf, strlen(send_buf)+1);
	kal_prompt_trace(MOD_XDM, "ret = %d", ret);

	//test_socket_huang();
}


/*
{
	"body":{
		"name":"aplex",
		"value":600
	}

	"body2":{
		"name2":"hello",
		"value2":123
	}

}
*/
char * create_json()
{
	cJSON * root;
	cJSON * js_body =  NULL;
	cJSON * js_body1 =  NULL;
	const char *const body = "body";
	const char *const body2 = "body2";
	char* text;

	//创建一个json对象
	root = cJSON_CreateObject();

	if(!root) {
		kal_prompt_trace(MOD_XDM, "create json root fail");	
		return;
	}
	else 
		kal_prompt_trace(MOD_XDM, "create json root ok");

	//在对象中插入第一个元素
	js_body = cJSON_CreateObject();
	cJSON_AddItemToObject(root, body, js_body);
	cJSON_AddStringToObject(js_body,"name","aplex");
	cJSON_AddNumberToObject(js_body,"value",600);

	//在对象中插入第二个元素
	js_body1 = cJSON_CreateObject();
	cJSON_AddItemToObject(root, body2, js_body1);
	cJSON_AddStringToObject(js_body1,"name","hello");
	cJSON_AddNumberToObject(js_body1,"value",123);

	//转化为字符串
	text = cJSON_PrintUnformatted(root);
	//注意:如果这里加了调试信息，会报错，
	//因为后面的free先释放，然后再调用trace
	kal_prompt_trace(MOD_XDM, "create js string is %s",text);
	//text = cJSON_Print(root);
	//kal_prompt_trace(MOD_XDM, "send format %s",text);

	//一定要释放，我这里只是因为调试
	//free(s);
	//cJSON_Delete(root);
	return text;

}

void test_prase_json(kal_char* buf)
{
	cJSON *body = NULL;
	cJSON *name = NULL;
	cJSON *value = NULL;
	//拿到根对象
	cJSON *root = cJSON_Parse((char *)buf);

	if(root!=NULL)
	{
		kal_prompt_trace(MOD_XDM, "cJSON_Parse ok");	
	}
	else
		kal_prompt_trace(MOD_XDM, "cJSON_Parse fail");	


	//拿到子元素
	body = cJSON_GetObjectItem(root, "body");
	if(body!=NULL)
		kal_prompt_trace(MOD_XDM, "get body ok");	
	else
		kal_prompt_trace(MOD_XDM, "get body fail");	
	
	//从子元素里拿到键值对
	name = cJSON_GetObjectItem(body, "name");
	if(body!=NULL)
		kal_prompt_trace(MOD_XDM, "get name ok");	
	else
		kal_prompt_trace(MOD_XDM, "get name fail");	

	//判断这个键值对的类型
	kal_prompt_trace(MOD_XDM, "name type is %d",name->type);
	switch(name->type)
	{
		case cJSON_Number:
			break;
		case cJSON_String:
			kal_prompt_trace(MOD_XDM, "name value is %s",name->valuestring);
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:
			break;
		default:
			break;
	}


	value = cJSON_GetObjectItem(body, "value");
	if(value!=NULL)
		kal_prompt_trace(MOD_XDM, "get value ok");	
	else
		kal_prompt_trace(MOD_XDM, "get value fail");	

	//判断这个键值对的类型
	kal_prompt_trace(MOD_XDM, "name type is %d",value->type);
	switch(value->type)
	{
		case cJSON_Number:
			kal_prompt_trace(MOD_XDM, "name value is %d",value->valueint);
			break;
		case cJSON_String:
			kal_prompt_trace(MOD_XDM, "name value is %s",value->valuestring);
			break;
		case cJSON_Array:
			break;
		case cJSON_Object:
			break;
		default:
			break;
	}

	body = cJSON_GetObjectItem(root, "body2");
	name = cJSON_GetObjectItem(body, "name");
	kal_prompt_trace(MOD_XDM, "name value is %s",name->valuestring);
	value = cJSON_GetObjectItem(body, "value");
	kal_prompt_trace(MOD_XDM, "name value is %d",value->valueint);
}

void socket_json_test_callback(enum_socket_state state, void *data_buff, kal_int32 size)
{
	kal_prompt_trace(MOD_XDM, "socket_test_callback  state=%d", state);

	switch(state)
	{
		case SOC_STATE_SEND_SUCCESS:
			break;
		case SOC_STATE_RECV_SUCCESS:
			kal_prompt_trace(MOD_XDM, "recv data = %s", (kal_char*)data_buff);
			test_prase_json((kal_char*)data_buff);
			break;
	}
}

void test_json()
{
	int ret = 0;
	//test_socket_json();
	//SetKeyHandler(test_socket_json, KEY_LSK, KEY_EVENT_UP);
	//ip连接测试
	kal_char dest_ip[]="47.98.195.146";	
	kal_uint16 port_ip = 4328;              
	kal_char* send_buf;

	kal_prompt_trace(MOD_XDM, "--------test_json----------------");
	send_buf = create_json();
	mmi_socket_init(MMI_SIM1, socket_json_test_callback);
	ret = mmi_socket_send(dest_ip, port_ip, send_buf, strlen(send_buf)+1);
	kal_prompt_trace(MOD_XDM, "ret = %d", ret);
}



void test_display()
{
	gui_set_text_color(UI_COLOR_BLACK);//设置颜色
	gui_move_text_cursor(10,10);//位置
	gui_set_font(&MMI_large_font);//字体大小
	gui_print_text(L"Hello world!");//打印字符串
	gui_BLT_double_buffer(0, 0, UI_DEVICE_WIDTH, UI_DEVICE_HEIGHT);	//刷新屏幕

}


/////////////////////MQTT//////////////////////////////
#if 0
int test_mqtt_publish(char *pTopic,char *pMessage)
{
	//地址
	int ret;
	int len,rc;
	int msglen = strlen(pMessage);
	int buflen = sizeof(mqtt.buf);

	buflen = sizeof(mqtt.buf);
	memset(mqtt.buf, 0x00, buflen);
	len = MQTTSerialize_publish(mqtt.buf , buflen , 0, 0, 0, 0, mqtt_socket_topicString, (unsigned char*)pMessage, msglen);
	
	ret = mmi_socket_send(mqtt.host, mqtt.port,  (unsigned char*)mqtt.buf, len);
	kal_prompt_trace(MOD_XDM, "[PUBLISH] ret =  %d", ret);
}


void fresh_mqtt()
{
	int ret;
	//地址
	int i;
	kal_prompt_trace(MOD_XDM, "-----fresh_mqtt msgtypes=%d-----", mqtt.msgtypes);
	//修改测试状态
	switch(mqtt.msgtypes)
	{
		case CONNECT:       //请求连接 
			kal_prompt_trace(MOD_XDM, "[TYPE CONNECT]");
			mqtt.len = MQTTSerialize_connect(mqtt.buf, mqtt.buflen, &mqtt_socket_data);        
			kal_prompt_trace(MOD_XDM, "[CONNECT] len=%d", mqtt.len);
			ret = mmi_socket_send(mqtt.host, mqtt.port,  (unsigned char*)mqtt.buf, mqtt.len);
			kal_prompt_trace(MOD_XDM, "CONNECT ret =  %d", ret);
			mqtt.msgtypes = 0;
			break;
			
		case CONNACK:       //连接确认
			kal_prompt_trace(MOD_XDM, "[TYPE CONNACK]");
			if (MQTTDeserialize_connack(&mqtt.sessionPresent, &mqtt.connack_rc, mqtt.buf, mqtt.buflen) != 1 || mqtt.connack_rc != 0)
			{
				kal_prompt_trace(MOD_XDM, "Unable to connect, return code %d\n", mqtt.connack_rc);
			}
			else 
				kal_prompt_trace(MOD_XDM, "MQTT is concet OK!");

			mqtt.msgtypes = SUBSCRIBE;
			//启动定时器，1秒后再次进入
			StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, fresh_mqtt);
			break;

		case SUBSCRIBE:        //订阅消息
			kal_prompt_trace(MOD_XDM, "[TYPE SUBSCRIBE]");
			mqtt_socket_topicString.cstring = "test-iot-sub";
			mqtt.len = MQTTSerialize_subscribe(mqtt.buf, mqtt.buflen, 0, mqtt.msgid, 1, &mqtt_socket_topicString, &mqtt.req_qos);
			//rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
			ret = mmi_socket_send(mqtt.host, mqtt.port,  (unsigned char*)mqtt.buf, mqtt.len);
			kal_prompt_trace(MOD_XDM, "SUBSCRIBE ret =  %d", ret);

			kal_prompt_trace(MOD_XDM, "client subscribe:[%s]",mqtt_socket_topicString.cstring);
			mqtt.msgtypes = 0;
			break;
			
		case SUBACK:        //订阅确认
			kal_prompt_trace(MOD_XDM, "[TYPE SUBACK]");
			mqtt.rc = MQTTDeserialize_suback(&mqtt.submsgid, 1, &mqtt.subcount, &mqtt.granted_qos, mqtt.buf, mqtt.buflen);                                                        
			kal_prompt_trace(MOD_XDM,"[SUBACK]:granted qos is %d\n", mqtt.granted_qos);  
			kal_prompt_trace(MOD_XDM,"[SUBACK]:mqtt_socket_rc %d\n", mqtt.rc);
			mqtt.msgtypes = 0;
			if(mqtt.rc==1)
			{
				kal_prompt_trace(MOD_XDM,"sub sucess\n");
			}
			else
			{
				kal_prompt_trace(MOD_XDM,"sub fail\n");
			}
			break;
			
		case PUBLISH:        //发布消息的接收
			kal_prompt_trace(MOD_XDM, "[TYPE PUBLISH]");
			//test_mqtt_publish("test-iot-sub", "aplex test");
			//拆包
			mqtt.rc = MQTTDeserialize_publish(&mqtt.dup, &mqtt.qos, &mqtt.retained, &mqtt.msgid, &mqtt.receivedTopic,
											&mqtt.payload_in, &mqtt.payloadlen_in, mqtt.buf, mqtt.buflen);
			
			kal_prompt_trace(MOD_XDM, "message arrived payload_in = %s", mqtt.payload_in);

			if(mqtt.qos == 1)
			{
				kal_prompt_trace(MOD_XDM, "publish qos is 1,send publish ack.");
				memset(mqtt.buf,0,mqtt.buflen);
				mqtt.len = MQTTSerialize_ack(mqtt.buf,mqtt.buflen,PUBACK,mqtt.dup,mqtt.msgid);   //publish ack                        
				//rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
				ret = mmi_socket_send(mqtt.host, mqtt.port,  (unsigned char*)mqtt.buf, mqtt.len);
				kal_prompt_trace(MOD_XDM, "[PUBLISH] ret =  %d", ret);
			}
			mqtt.msgtypes = 0;
			break;

		case PUBACK:        //发布确认
			kal_prompt_trace(MOD_XDM, "[TYPE PUBACK]");
			kal_prompt_trace(MOD_XDM, "PUBACK OK!!!!!!!!!!");
			mqtt.msgtypes = 0;
			break;
		case PUBREC:        
			kal_prompt_trace(MOD_XDM, "[TYPE PUBREC]");
			kal_prompt_trace(MOD_XDM, "PUBREC!");     //just for qos2
			break;
		case PUBREL:        
			kal_prompt_trace(MOD_XDM, "PUBREL!");        //just for qos2
			break;
		case PUBCOMP:      
			kal_prompt_trace(MOD_XDM, "PUBCOMP!");        //just for qos2
			break;
		case PINGREQ:        //ping请求
			mqtt.len = MQTTSerialize_pingreq(mqtt.buf, mqtt.buflen);
			//rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
			ret = mmi_socket_send(mqtt.host, mqtt.port,  (unsigned char*)mqtt.buf, mqtt.len);
			kal_prompt_trace(MOD_XDM, "[PINGREQ] ret =  %d", ret);
			kal_prompt_trace(MOD_XDM, "time to ping mqtt server to take alive!");
			mqtt.msgtypes = 0;
			break;
		case PINGRESP:        //ping应答
			kal_prompt_trace(MOD_XDM, "mqtt server Pong");                                                        
			mqtt.msgtypes = 0;
			break;
	}
}

void init_mqtt()
{
	
	mqtt_socket_data.keepAliveInterval = 50;
	mqtt_socket_data.cleansession = 1;
	mqtt_socket_data.MQTTVersion = 3;		//这里修改为3
	//用户名
	mqtt_socket_data.username.cstring = "zengjf/sz_monitor_room";
	//密码
	mqtt_socket_data.password.cstring = "QE0BHFvFnIkBRIaJtPYzo3m/63Esv5fzzMr9tYVOsHo=";
	//客户ID
	mqtt_socket_data.clientID.cstring = "MQTT_FX_Client124";
	//网址
	memset(mqtt.host, 0x0, sizeof(mqtt.host));
	strcpy(mqtt.host, "zengjf.mqtt.iot.gz.baidubce.com");
	//端口
	mqtt.port = 1883;

	mqtt.msgtypes= CONNECT;
	mqtt.rc = 0;
	mqtt.buflen = sizeof(mqtt.buf);
	memset(mqtt.buf, 0x00, mqtt.buflen);
	mqtt.msgid = 1;
	mqtt.granted_qos =0;
	mqtt.len = 0;
	mqtt.req_qos = 1;
}

void publish_func()
{
	kal_prompt_trace(MOD_XDM, "publish_func");
	//发布主题
	//mqtt_publish(&broker, "test-iot-sub", "hello aplex", 0);	
	test_mqtt_publish("test-iot-sub", "aplex testdfjoweifjweaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbfweofjweoifjweooooooooooooooooooooooooo");
}

void subscribe_func()
{
	kal_prompt_trace(MOD_XDM, "subscribe_func");
	//订阅主题test-iot-sub
	//mqtt_subscribe(&broker, "test-iot-sub", NULL);
}

int transport_getdata_mqtt(unsigned char* buf, int count)
{
	return count;
}

void socket_mqtt_test_callback(enum_socket_state state, void *data_buff, kal_int32 size)
{
	int i=0;
	unsigned char*  buff = (unsigned char*)data_buff;
	kal_prompt_trace(MOD_XDM, "socket_test_callback  state=%d", state);

	switch(state)
	{
		case SOC_STATE_SEND_SUCCESS:
			kal_prompt_trace(MOD_XDM, "send data");
			//for(i=0; i<size; i++)
			//	kal_prompt_trace(MOD_XDM, "data[%d] = %x",i,  buff[i]);
			
			break;
		case SOC_STATE_RECV_SUCCESS:
			kal_prompt_trace(MOD_XDM, "recv data");
			//for(i=0; i<size; i++)
			//	kal_prompt_trace(MOD_XDM, "data[%d] = %x",i,  buff[i]);
			//返回的是类型
			mqtt.rc=MQTTPacket_read(data_buff, size, transport_getdata_mqtt);  
			//mqtt_socket_rc = buff[0];
			kal_prompt_trace(MOD_XDM,"mqtt_socket_rc = %d", mqtt.rc);
			if(mqtt.rc >0)
			{
				mqtt.msgtypes = mqtt.rc;
				kal_prompt_trace(MOD_XDM,"MQTT is get recv:");
				memset(mqtt.buf, 0x00, sizeof(mqtt.buf));
				memcpy(mqtt.buf, buff, size);
				fresh_mqtt();
			}
			//test_prase_json((kal_char*)data_buff);
			
			break;
	}
}


void test_mqtt()
{
	kal_prompt_trace(MOD_XDM, "-----------start test mqtt-------------");
	init_mqtt();
	mmi_socket_init(MMI_SIM1, socket_mqtt_test_callback);
	SetKeyHandler(publish_func, KEY_LSK, KEY_EVENT_UP);
	fresh_mqtt();
	//使用定时器
	//StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 2000, fresh_mqtt);
	//初始化MQTT,client id没有用
	//mmi_socket_set_callback(mmi_mystudy_socket_cb);
      //mqtt_init(&broker, "1000179356:xhfddc_test_21:1:1.0.0");
      //mqtt_connect(&broker);
	//SetKeyHandler(subscribe_func, KEY_0, KEY_EVENT_UP);
/////////////////////////////////////////////////////////////////////////////////
}

#endif
/////////////////////MQTT结束//////////////////////////////////////////////////////////
void mmi_my_mtk_func(void)
{
	kal_prompt_trace(MOD_XDM, "i am key1111111111111111");
	//test_mqtt();
}

void mmi_my_powerkey_func(void)
{
	kal_prompt_trace(MOD_XDM, "i am power key");
	//test_mqtt();
	//test_socket_huang();
	//test_sim();
}


void test_key()
{
	SetKeyHandler(mmi_my_mtk_func, KEY_LSK, KEY_EVENT_UP);
}
/*
static void sim_read_handle(srv_sim_cb_struct *param)
{
    //srv_eons_struct *eons;
    srv_sim_file_info_struct *info;
    sim_service_state_enum state;

    MMI_ASSERT(param && param->user_data && param->data);

    //eons = (srv_eons_struct *)param->user_data;
    info = (srv_sim_file_info_struct *)param->data;

//#if (MMI_MAX_SIM_NUM >= 2)
//    state = sim_service_table_query(SERVICE_OPLMN_LIST,
//                l4c_gemini_get_switched_sim_id((sim_interface_enum)mmi_frm_sim_to_index(eons->sim)));
//#else
    state = sim_service_table_query(SERVICE_OPLMN_LIST);
//#endif

    if (!param->result || state != SERVICE_SUPPORT || !info->num_of_rec ||
        info->file_size / info->num_of_rec < SRV_EONS_OPL_RECORD_SIZE)
    {
        SRV_EONS_LOG((TRC_SRV_EONS_READ_OPL_INFO_HDLR, eons->sim, 600));

        srv_eons_update_module_state(eons->sim, SRV_EONS_MOD_OPL);
    }
    else
    {
        SRV_EONS_LOG((TRC_SRV_EONS_READ_OPL_INFO_HDLR, eons->sim, 200));

        eons->num_of_opl_rec = info->num_of_rec;
        eons->size_of_opl_file = info->file_size;

        srv_eons_read_opl_record(eons);
    }
}
*/

VMINT sim_read_iccid_callback(srv_sim_cb_struct* callback_data)
{
	srv_sim_cb_struct* sim_cb_struct = callback_data;
	srv_sim_data_struct* sim_data = (srv_sim_data_struct*)sim_cb_struct->data;
	int i = 0;   
	char iccid[21];

	kal_prompt_trace(MOD_XDM, "enter sim_read_iccid_callback size=%d", sizeof(iccid));
	kal_prompt_trace(MOD_XDM, "length=%d", sim_data->length);
	memset(iccid, 0, sizeof(iccid));
	if (sim_cb_struct->result)
	{
		U8  * tmp = sim_data->data;
		for (i = 0; i < 10; i++)
		{            
			//kal_prompt_trace(MOD_XDM, "tmp = %d", tmp[i]);
			iccid[2*i] = (tmp[i]&0x0f) + 0x30;
			iccid[2*i+1] = ((tmp[i]>>4 )&0x0f) + 0x30;
		}
		kal_prompt_trace(MOD_XDM, "sim card iccid = %s", iccid);
	}
	return 0;
}

VMINT sim_read_imsi_callback(srv_sim_cb_struct* callback_data)
{
	srv_sim_cb_struct* sim_cb_struct = callback_data;
	srv_sim_data_struct* sim_data = (srv_sim_data_struct*)sim_cb_struct->data;
	int i = 0;   
	char imsi[16];
	memset(imsi, 0, sizeof(imsi));
	kal_prompt_trace(MOD_XDM, "immi length=%d", sim_data->length);
	
	for(i = 0; i<5; i++)
	{
		U8  * tmp = sim_data->data;
		kal_prompt_trace(MOD_XDM, "sim card imsi = %d", tmp[i]);
		//kal_prompt_trace(MOD_XDM, "sim card imsi = %c", tmp[i]);
		imsi[2*i] = (tmp[i]&0x0f) + 0x30;
		imsi[2*i+1] = ((tmp[i]>>4 )&0x0f) + 0x30;
	}

	kal_prompt_trace(MOD_XDM, "sim card imsi = %s", imsi);
	return 0;
}

#if 0
MMI_BOOL my_srv_nw_info_rx_level_ind_hdlr(void *msg, S32 src_mod)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_sim_enum sim;
    srv_nw_info_cntx_struct *cntx;
    mmi_nw_rx_level_ind_struct *rx_level_ind;
    U8 target_percentage;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    sim = mmi_frm_l4c_mod_to_sim((module_type)src_mod);
    cntx = srv_nw_info_get_cntx(sim);
    rx_level_ind = (mmi_nw_rx_level_ind_struct*)msg;

    /* qdBm = dBm * 4 */
    if (rx_level_ind->rat == L4C_RAT_GSM)
    {
        if (rx_level_ind->rssi_in_qdbm < 0)
        {
            cntx->signal.gsm_band = srv_nw_info_get_band_from_l4_value(rx_level_ind->current_band);
            cntx->signal.gsm_RSSI_in_qdBm = rx_level_ind->rssi_in_qdbm;
            target_percentage = srv_nw_info_get_percentage_from_gsm_rssi(cntx->signal.gsm_RSSI_in_qdBm);
        }
    }
    return MMI_FALSE;
}
#endif
	
void test_sim()
{
	MMI_BOOL ret;
	CHAR imsi[100];

	U16 * ucs2_name;
	CHAR myname[128];	

	const WCHAR *wstrnum = NULL;
	kal_char cstrnum[64];
	const WCHAR *wstrname = NULL;
	kal_char cstrname[64];
	CHAR  imei_buf[15 + 1] = {0}; 
	
	char teststring[100];
	//用于测试读取卡号和用户名的
	srv_cphs_group_enum group_name = SRV_CPHS_GROUP_OTHER_SUPP_SERVICES;
	srv_cphs_service_enum service = SRV_CPHS_SERVICE_CUG;
	mmi_sim_enum select_sim = MMI_SIM1;


	kal_prompt_trace(MOD_XDM, "---------------sim test------------------------");
	memset(teststring, 0x00, sizeof(teststring));
	strcpy(teststring, "english test");
	kal_prompt_trace(MOD_XDM, "string=%s", teststring);

	memset(teststring, 0x00, sizeof(teststring));
	strcpy(teststring, "中文测试");
	kal_prompt_trace(MOD_XDM, "string=%s", teststring);


	//1. 读取ICCID
	/*
            U16 file_index,
            U8 *file_path,
            U16 record_index,
            U16 record_size,
            mmi_sim_enum sim_id,
            SrvSimCallbackFunc callback,
            void *user_data)
	*/
	//这个ok
	 ret = srv_sim_read_record(FILE_ICCID_IDX,NULL,0,10,MMI_SIM1,sim_read_iccid_callback,NULL);
/*
	ret = srv_sim_get_file_info(
            (U16)FILE_ICCID_IDX,
            NULL,
            MMI_SIM1,
            sim_read_iccid_callback,
            NULL);
*/
	//2. 获取imei
	if(srv_imei_get_imei(MMI_SIM1,imei_buf,15+1))
	{
		ret = imei_buf[0] == '\0'?MMI_FALSE:MMI_TRUE;
		if(ret==MMI_TRUE)
			kal_prompt_trace(MOD_XDM, "sim card imei get ok");
		else
			kal_prompt_trace(MOD_XDM, "sim card imei get fail");
		
		ret == srv_imei_is_valid(MMI_SIM1);
		if(ret==MMI_TRUE)
			kal_prompt_trace(MOD_XDM, "sim card imei is valid");
		else
			kal_prompt_trace(MOD_XDM, "sim card imei is not valid");
		
		kal_prompt_trace(MOD_XDM, "imei=%s", imei_buf);
		
	}    

	//2. 读取IMSI
	/*ret = srv_sim_read_record(FILE_U_IMSI_IDX,NULL,0,10,MMI_SIM1,sim_read_imsi_callback,NULL);
	ret = srv_sim_get_file_info(
            (U16)FILE_U_IMSI_IDX,
            NULL,
            MMI_SIM1,
            sim_read_imsi_callback,
            NULL);
       */     
       memset(imsi, 0, sizeof(imsi));
	ret = srv_sim_ctrl_get_imsi(MMI_SIM1, (CHAR*)imsi, 17);

	if(ret==MMI_TRUE)
	{
		kal_prompt_trace(MOD_XDM, "sim card imsi read ok ");
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "sim card imsi read error ");
	}
	kal_prompt_trace(MOD_XDM, "sim card imsi = %s", imsi);
	/*
	memset(imsi, 0, sizeof(imsi));
	ret = srv_sim_ctrl_get_imsi(MMI_SIM2, (CHAR*)imsi, 17);

	if(ret==MMI_TRUE)
	{
		kal_prompt_trace(MOD_XDM, "sim2 card imsi read ok ");
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "sim2 card imsi read error ");
	}
	kal_prompt_trace(MOD_XDM, "sim2 card imsi = %s", imsi);
	*/
	//3. 读取mcc, 未成功
	/*
	memset(imsi, 0,sizeof(imsi));
       srv_sim_ctrl_get_home_plmn(MMI_SIM1, imsi, 7);
   	kal_prompt_trace(MOD_XDM, "sim2 card mcc = %s", imsi);
	*/
	//4. 读取状态(暂时未研究)

	//5.  获取MSISDN(电话号码)(未成功)
	//srv_phb_owner_number_get_name();
	//srv_phb_owner_number_get_number();
	//ucs2_name = srv_phb_owner_number_get_name((U8) mmi_frm_sim_to_index(MMI_SIM1), (U16) SRV_CPHS_MSISDN_TYPE_LINE1);
	//mmi_ucs2_to_asc(myname, ucs2_name);
	//kal_prompt_trace(MOD_XDM, "sim name=%s ", myname);
	//SRV_CPHS_MSISDN_TYPE_LINE1
#if 0
	ret = srv_cphs_is_valid_csp(MMI_SIM1); //Need to insert SIM which support CPHS
	if(ret == MMI_FALSE)
		kal_prompt_trace(MOD_XDM, "srv_cphs_is_valid_csp fail ");
	else
		kal_prompt_trace(MOD_XDM, "srv_cphs_is_valid_csp ok ");

	/*srv_cphs_is_service_available*/ 
	ret = srv_cphs_is_service_available(group_name, service, select_sim);
	if(ret == MMI_FALSE)
		kal_prompt_trace(MOD_XDM, "srv_cphs_is_service_available fail ");
	else
		kal_prompt_trace(MOD_XDM, "srv_cphs_is_service_available ok ");
	
	wstrname = srv_cphs_get_msisdn_name(SRV_CPHS_MSISDN_TYPE_LINE1, MMI_SIM1);
	mmi_wcs_to_asc(cstrname, (WCHAR *)wstrname);
	kal_prompt_trace(MOD_XDM, "sim name1=%s ", cstrname);

	wstrname = srv_cphs_get_msisdn_name(SRV_CPHS_MSISDN_TYPE_LINE2, MMI_SIM1);
	mmi_wcs_to_asc(cstrname, (WCHAR *)wstrname);
	kal_prompt_trace(MOD_XDM, "sim name2=%s ", cstrname);
	
	wstrnum = srv_cphs_get_msisdn_number(SRV_CPHS_MSISDN_TYPE_LINE1, MMI_SIM1);
	mmi_wcs_to_asc(cstrnum, (WCHAR *)wstrnum);
	kal_prompt_trace(MOD_XDM, "sim number1=%s ", cstrnum);

	wstrnum = srv_cphs_get_msisdn_number(SRV_CPHS_MSISDN_TYPE_LINE2, MMI_SIM1);
	mmi_wcs_to_asc(cstrnum, (WCHAR *)wstrnum);
	kal_prompt_trace(MOD_XDM, "sim number2=%s ", cstrnum);

#endif
	//6. 获取信号强度
	//mmi_frm_set_multi_protocol_event_handler(MSG_ID_MMI_NW_RX_LEVEL_IND, (PsIntFuncPtr)my_srv_nw_info_rx_level_ind_hdlr);
}

void test_call()
{
	kal_prompt_trace(MOD_XDM, "-----------test_call----------------");
	MakeCall((CHAR*)L"13267159816");
}

#if 0

#define MAX_DIGITS_SMS  21

void mmi_send_sms_callback(srv_sms_callback_struct* callback_data)
{
    kal_prompt_trace(MOD_XDM, "mmi_send_sms_callback ret=%d", callback_data->result);
}
	
void my_mmi_sample_send_sms_callback(srv_sms_callback_struct* callback_data)
{
	kal_prompt_trace(MOD_XDM, "[message]mmi_send_chinese_sms_callback result=%d, action=%d, cause=%d\n",callback_data->result, callback_data->action, callback_data->cause);

	if (callback_data->result == MMI_TRUE)
	{
		// send successfully 
		kal_prompt_trace(MOD_XDM, "[message]MMI_send_chinese_sms_callback successfully");
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "[message]MMI_send_chinese_sms_callback error = %d", callback_data->cause);
		switch(callback_data->cause)
		{
			case SRV_SMS_CAUSE_SEND_ABORT:
			case SRV_SMS_CAUSE_NOT_READY:
			case SRV_SMS_CAUSE_SEND_BUSY:
			case SRV_SMS_CAUSE_MEM_FULL:
			case SRV_SMS_CAUSE_MEM_INSUFFICIENT:
			case SRV_SMS_CAUSE_DATA_INVALID:
			case SRV_SMS_CAUSE_DATA_EXCEED:
			case SRV_SMS_CAUSE_NUMBER_EMPTY:
			case SRV_SMS_CAUSE_NUMBER_INVALID:
			case SRV_SMS_CAUSE_SC_EMPTY:
			case SRV_SMS_CAUSE_SC_INVALID:
			case SRV_SMS_CAUSE_FDL_FAIL:
			case SRV_SMS_CAUSE_AT_CONFLICT:
			case SRV_SMS_CAUSE_OP_CONFLICT:
			case SRV_SMS_CAUSE_EMAIL_NO_SPACE:
			case SRV_SMS_CAUSE_UNKNOWN_ERROR:
			default:
			// send failed 
			break;
		}
	}
}


void mmi_send_chinese_sms(S8* number, U16* content)
{
    SMS_HANDLE send_handle;

    send_handle = srv_sms_get_send_handle();
    if (send_handle == NULL)
    {
	// can not get handle 
	kal_prompt_trace(MOD_XDM, "[message]srv_sms_get_send_handle can not get handle");
	return;
    }

    if ((U16)srv_sms_is_bg_send_action_busy() == MMI_TRUE)
    {
	// background sending is busy 
	kal_prompt_trace(MOD_XDM, "[message]srv_sms_get_send_handle background sending is busy");
	return;
    }

    /* set address number */
    srv_sms_set_address(send_handle, number);
    /* set content of DCS encoding */
    srv_sms_set_content_dcs(send_handle, SRV_SMS_DCS_UCS2);
    /* set content */
    srv_sms_set_content(send_handle, (S8*)content, (U16)((mmi_wcslen(content)+1)*2));

    /* set SIM1 */
    srv_sms_set_sim_id(send_handle, SRV_SMS_SIM_1);

    /* send request */
    srv_sms_send_msg(send_handle, my_mmi_sample_send_sms_callback, NULL);
}

void test_message()
{
	
/*
	U8 ucs2_data[256]={0x00},temp[128]={0x00};
	sprintf((char*)temp,"aplex test");            
	mmi_asc_to_ucs2((S8*)ucs2_data, (S8*)temp);
	srv_sms_send_ucs2_text_msg((char*)ucs2_data, (char*)L"13267159816", SRV_SMS_SIM_1,mmi_send_sms_callback,NULL);
*/
	U8 ucs2_data[256]={0x00},data[128]={0x00};
	U8 ucs2_number[256] = {0},number[128] = {0};

	kal_prompt_trace(MOD_XDM, "--------test_message----------------");

	memset(data, 0x00, sizeof(data));
	memset(number, 0x00, sizeof(number));
	
	strcpy(number, "13267159816");
	//sprintf((char*)data,"aplex test");            
	strcpy(data, "aplex test");
	
	mmi_asc_to_ucs2((S8*)ucs2_data, (S8*)data);
	mmi_asc_to_ucs2((S8*)ucs2_number, (S8*)number);
	
	mmi_send_chinese_sms(ucs2_number, (U16*)ucs2_data);
}

#endif

kal_uint8 chinese_content_buff[4]={96,79,125,89};///////你好的UCS2编码值，UCS2-LIT，

void mmi_send_chinese_sms_callback(srv_sms_callback_struct* callback_data)
{
	kal_prompt_trace(MOD_SMS, "result=%d, action=%d, cause=%d\n",callback_data->result,
	callback_data->action, callback_data->cause);
	if (callback_data->result == MMI_TRUE)
	{
		/* send successfully */
	}
	else
	{
		switch(callback_data->cause)
		{
			case SRV_SMS_CAUSE_SEND_ABORT:
			case SRV_SMS_CAUSE_NOT_READY:
			case SRV_SMS_CAUSE_SEND_BUSY:
			case SRV_SMS_CAUSE_MEM_FULL:
			case SRV_SMS_CAUSE_MEM_INSUFFICIENT:
			case SRV_SMS_CAUSE_DATA_INVALID:
			case SRV_SMS_CAUSE_DATA_EXCEED:
			case SRV_SMS_CAUSE_NUMBER_EMPTY:
			case SRV_SMS_CAUSE_NUMBER_INVALID:
			case SRV_SMS_CAUSE_SC_EMPTY:
			case SRV_SMS_CAUSE_SC_INVALID:
			case SRV_SMS_CAUSE_FDL_FAIL:
			case SRV_SMS_CAUSE_AT_CONFLICT:
			case SRV_SMS_CAUSE_OP_CONFLICT:
			case SRV_SMS_CAUSE_EMAIL_NO_SPACE:
			case SRV_SMS_CAUSE_UNKNOWN_ERROR:
			default:
			/* send failed */
			break;
		}
	}
}
void mmi_send_chinese_sms()
{
	SMS_HANDLE send_handle;
	send_handle = srv_sms_get_send_handle();
	if (send_handle == NULL)
	{
		// can not get handle 
		kal_prompt_trace(MOD_XDM, "[message]srv_sms_get_send_handle can not get handle");
		return;
	}

	if ((U16)srv_sms_is_bg_send_action_busy() == MMI_TRUE)
	{
		// background sending is busy 
		kal_prompt_trace(MOD_XDM, "[message]srv_sms_get_send_handle background sending is busy");
		return;
	}

	/* set address number */
	srv_sms_set_address(send_handle, (CHAR*)(L"13267159816"));
	/* set content of DCS encoding */
	srv_sms_set_content_dcs(send_handle, SRV_SMS_DCS_UCS2);
	/* set content */
	srv_sms_set_content(send_handle, (S8*)chinese_content_buff, 4);
	/* set SIM1 */
	srv_sms_set_sim_id(send_handle, SRV_SMS_SIM_1);
	/* send request */
	srv_sms_send_msg(send_handle, mmi_send_chinese_sms_callback, NULL);
}

void test_message()
{
	kal_prompt_trace(MOD_XDM, "--------test_message----------------");
	mmi_send_chinese_sms();
}


void mmi_get_power()
{
	MMI_BOOL ret;
	battery_level_enum level;
	static int cnt = 0;

	cnt++;
	if(cnt>=300)
	{
		cnt=0;
		ret = srv_charbat_is_charging();	//是否正在充电

		if(ret==MMI_TRUE)
			kal_prompt_trace(MOD_XDM, "[power]is charging");
		else
			kal_prompt_trace(MOD_XDM, "[power]no charging");

		level = srv_charbat_get_battery_level();
		kal_prompt_trace(MOD_XDM, "[power]battery level = %d", level);
	}

	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, mmi_get_power);
}

void test_power()
{
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, mmi_get_power);
}

kal_bool led_on_off;
void mmi_led_blue_switch(void)
{	
	char on_off4;
	char on_off8;
	char on_off9;
#if 0
	if(KAL_TRUE == led_on_off)
	{
		led_on_off = KAL_FALSE;
		GPIO_WriteIO(1, 5);
		GPIO_WriteIO(1, 6);
		GPIO_WriteIO(1, 7);
		kal_prompt_trace(MOD_XDM, "led on");
	}
	else
	{
		led_on_off = KAL_TRUE;
		GPIO_WriteIO(0, 5);
		GPIO_WriteIO(0, 6);
		GPIO_WriteIO(0, 7);
		kal_prompt_trace(MOD_XDM, "led off");
	}
#endif
	on_off4 = GPIO_ReadIO(4);
	on_off8 = GPIO_ReadIO(8);
	on_off9 = GPIO_ReadIO(9);
	if(on_off4)
	{
		kal_prompt_trace(MOD_XDM, "read4 high");
	}else
	{
		kal_prompt_trace(MOD_XDM, "read4 low");
	}

	if(on_off8)
	{
		kal_prompt_trace(MOD_XDM, "read8 high");
	}else
	{
		kal_prompt_trace(MOD_XDM, "read8 low");
	}

	if(on_off9)
	{
		kal_prompt_trace(MOD_XDM, "read9 high");
	}else
	{
		kal_prompt_trace(MOD_XDM, "read9 low");
	}
	
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 100, mmi_led_blue_switch);
}
//
void test_drv_gpio()
{
	//输出的
	GPIO_ModeSetup(5, 0);
	GPIO_ModeSetup(6, 0);
	GPIO_ModeSetup(7, 0);
	GPIO_InitIO(1,5);
	GPIO_InitIO(1,6);
	GPIO_InitIO(1,7);
	//输入的
	GPIO_ModeSetup(4, 0);
	GPIO_ModeSetup(8, 0);
	GPIO_ModeSetup(9, 0);
	GPIO_InitIO(0,4);
	GPIO_InitIO(0,8);
	GPIO_InitIO(0,9);

	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 100, mmi_led_blue_switch);
}

void mmi_task_test(void)
{
	kal_uint32 my_index;
	static kal_uint32 i = 0;
	static kal_bool  is_first = KAL_TRUE;
	kal_uint32* local_data = (kal_uint32*) mmi_construct_msg_local_para(sizeof(kal_uint32));

	//ilm_struct *ilm_ptr = allocate_ilm(MOD_ZENGJF_SEND);
	//ilm_ptr->src_mod_id = MOD_ZENGJF_SEND;
	ilm_struct *ilm_ptr = allocate_ilm(MOD_MMI);

	i++;
	*local_data = i;

	ilm_ptr->src_mod_id = MOD_MMI;
	ilm_ptr->dest_mod_id = MOD_ZENGJF_TEST;
	ilm_ptr->msg_id = MSG_ID_ZENGJFTEST;
	ilm_ptr->sap_id = 0;
	ilm_ptr->peer_buff_ptr = NULL;
	//ilm_ptr->local_para_ptr = NULL;
	ilm_ptr->local_para_ptr = (local_para_struct *)local_data;
	if(is_first == KAL_TRUE)
	{
		is_first = KAL_FALSE;
		kal_get_my_task_index(&my_index);
		kal_prompt_trace(MOD_XDM, "zengjfsend_task start...");
	}

	msg_send_ext_queue(ilm_ptr);
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, mmi_task_test);
}

void test_conn_task()
{
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, mmi_task_test);
}

///////////////////蓝牙////////////////////////////////////////////////////
#if 1
//kal_uint32 conn_id;
#define MY_TEST_UUID 0x1101
//#define MY_TEST_UUID 0xFFFC
#define MY_TEST_SPP_REJECT_CASE_FLAG 0x00000001

typedef struct
{
    SppHandle sppsrv_hd;
    S32 conn_id;
    U32 flag;
    CHAR buf[100];
} sppc_cntx;

static sppc_cntx g_spp_cntx;

#if 1
void mmi_bt_send_test()
{
	kal_uint8 spp_send_data[] = "test_srv_spp_write:Temp:25.36\r\n";
#if 0
	CHAR buf[] = "hello";
	UART_CTRL_OWNER_T port_owner;
	UART_CTRL_PUT_BYTES_T data;
	DCL_HANDLE dcl_handle = DclSerialPort_Open(conn_ind->port, 0);
	DclSerialPort_Control(dcl_handle, SIO_CMD_GET_OWNER_ID, (DCL_CTRL_DATA_T*)&port_owner);
	// if (conn->conn_state != SRV_SPP_CONN_STATE_CONNECTED)
	// return ;
	kal_prompt_trace(MOD_TST, "%s: ownerid is %d", __func__, port_owner.u4OwenrId);
	kal_prompt_trace(MOD_TST, "%s: MOD_ATCI is %d", __func__, MOD_ATCI);
	kal_prompt_trace(MOD_TST, "%s: MOD_MMI is %d", __func__, MOD_MMI);
	kal_prompt_trace(MOD_TST, "%s: MOD_BT is %d", __func__, MOD_BT);
	data.u4OwenrId = MOD_ATCI;
	data.puBuffaddr = buf;
	data.u2Length = strlen(buf);
	DclSerialPort_Control(dcl_handle, SIO_CMD_PUT_BYTES, (DCL_CTRL_DATA_T*)&data);
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 2000, mmi_bt_send_test);
#endif
	kal_prompt_trace(MOD_XDM, "mmi_bt_send_test");
	test_srv_spp_write(g_spp_cntx.conn_id, spp_send_data, strlen(spp_send_data));
}
#endif

 void bt_timer(void)
{
	kal_prompt_trace(MOD_MMI, "aplex timer\n");
	//使用定时器
	StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 2000, mmi_bt_send_test);
}
 
S32 test_bt_spp_event_handler(U32 evt, void* para)
{	
	static kal_bool is_alart = KAL_FALSE;
	kal_uint32 ret = 0;
	kal_uint8 btspp_buff[SRV_SPP_MIN_BUFF_SIZE];
	kal_uint8 spp_send_data[] = "test_srv_spp_write:Temp:25.36\r\n";
	kal_uint8 spp_read_data[SRV_SPP_MIN_BUFF_SIZE];
	srv_spp_event_cntx *cntx = (srv_spp_event_cntx*)para;

	const srv_bt_cm_dev_struct* dev_info = srv_bt_cm_get_dev_info_by_index(0, SRV_BT_CM_PAIRED_DEV);
	if (dev_info)
	{
		kal_prompt_trace(MOD_XDM, "dev0 name=[%s] mac Addr:0x%02x:%02x:%02x:%02x:%02x:%02x", dev_info->name,
		((dev_info->bd_addr.nap & 0xff00) >> 8),
		(dev_info->bd_addr.nap & 0x00ff),
		(dev_info->bd_addr.uap),
		((dev_info->bd_addr.lap & 0xff0000) >> 16),
		((dev_info->bd_addr.lap & 0x00ff00) >> 8),
		(dev_info->bd_addr.lap & 0x0000ff));
	}
	
 	kal_prompt_trace(MOD_XDM, "-------test_bt_spp_event_handler-----------------------");
	kal_prompt_trace(MOD_XDM, "app_btspp_cb conn_id[%d] evt[%d]",cntx->para, evt);
	kal_prompt_trace(MOD_XDM, "before conn_id=%d", g_spp_cntx.conn_id);
	g_spp_cntx.conn_id = cntx->para;
 
	switch(evt){
		case SRV_SPP_EVENT_BIND_FAIL:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = SRV_SPP_EVENT_BIND_FAIL");
			break;
		case SRV_SPP_EVENT_CONNECT:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = SRV_SPP_EVENT_CONNECT, ret = %d", cntx->ret);
			//bt_timer();
			test_srv_spp_write(g_spp_cntx.conn_id, spp_send_data, strlen(spp_send_data));
			kal_prompt_trace(MOD_XDM, "cntx->ret=%d", cntx->ret);
			if (cntx->ret)
			{
				g_spp_cntx.conn_id = cntx->para;
				kal_prompt_trace(MOD_XDM, "g_spp_cntx.conn_id=%d", g_spp_cntx.conn_id);
				//AVK_CASE_RESULT(AVK_PASS);
			}
			else
			{
				if (!(g_spp_cntx.flag & MY_TEST_SPP_REJECT_CASE_FLAG))
				{
					kal_prompt_trace(MOD_XDM, "aaaa");
					g_spp_cntx.flag |= MY_TEST_SPP_REJECT_CASE_FLAG;
					
					if (dev_info)
					{
						kal_prompt_trace(MOD_XDM, "bbb");
						srv_spp_connect(
						g_spp_cntx.sppsrv_hd,
						MY_TEST_UUID,
						(srv_bt_cm_bt_addr*)&(dev_info->bd_addr),
						g_spp_cntx.buf,
						SRV_SPP_MIN_BUFF_SIZE/2,
						SRV_SPP_MIN_BUFF_SIZE/2);
					}
					else
					{
						kal_prompt_trace(MOD_XDM, "ccc");
						//mmi_display_popup(
						//(UI_string_type)L"Please let the SPP server as 1st paired device!",
						//MMI_EVENT_FAILURE);

						srv_spp_close(g_spp_cntx.sppsrv_hd);
						//AVK_LOG_FUN(srv_spp_close, MMI_TRUE);
						g_spp_cntx.sppsrv_hd = -1;

						//AVK_CASE_RESULT(AVK_FAIL);
						//AVK_ASYN_DONE();
					}
				}
				else
				{
					//AVK_CASE_RESULT(AVK_FAIL);
					kal_prompt_trace(MOD_XDM, "ddd");
					srv_spp_close(g_spp_cntx.sppsrv_hd);
					//AVK_LOG_FUN(srv_spp_close, MMI_TRUE);
					g_spp_cntx.sppsrv_hd = -1;

					//AVK_ASYN_DONE();
				}
			}
			break;
	        case SRV_SPP_EVENT_AUTHORIZE:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = VM_SRV_SPP_EVENT_AUTHORIZE");
			srv_spp_accept(g_spp_cntx.conn_id, btspp_buff, SRV_SPP_MIN_BUFF_SIZE / 2, SRV_SPP_MIN_BUFF_SIZE / 2);
	        	break;
	        case SRV_SPP_EVENT_READY_TO_WRITE:
	        	kal_prompt_trace(MOD_XDM, "[LXZ]evt = VM_SRV_SPP_EVENT_READY_TO_WRITE");
			//test_srv_spp_write(conn_id, spp_data, strlen(spp_data));
	        	break;
	        case SRV_SPP_EVENT_READY_TO_READ:
	        	kal_prompt_trace(MOD_XDM, "[LXZ]evt = VM_SRV_SPP_EVENT_READY_TO_READ");
			test_srv_spp_read(g_spp_cntx.conn_id, spp_read_data, SRV_SPP_MIN_BUFF_SIZE);
			//test_srv_spp_write(conn_id, spp_send_data, strlen(spp_send_data));
	        	break;
	        case SRV_SPP_EVENT_DISCONNECT:
	        	kal_prompt_trace(MOD_XDM, "[LXZ]evt = SRV_SPP_EVENT_DISCONNECT");
	        	break;
		case SRV_SPP_EVENT_START:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = SRV_SPP_EVENT_START");
			break;
		case SRV_SPP_EVENT_SCO_CONNECT:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = SRV_SPP_EVENT_SCO_CONNECT");
			break;
		case SRV_SPP_EVENT_SCO_DISCONNECT:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = SRV_SPP_EVENT_SCO_DISCONNECT");
			break;
		default:
			kal_prompt_trace(MOD_XDM, "[LXZ]evt = %d", evt);
			break;
    	}
}
 
 //启动测试
void test_btspp_start(void)
{
	kal_prompt_trace(MOD_XDM, "test_btspp_start");
	//mmi_bt_sppc_init();
	//mmi_bt_spp_server_init();
	//mmi_bt_spp_init();
	//mmi_bt_spp_server_init();
	//mmi_bt_spp_test();
#if 0
	//SppHandle handle;
	U32 evt_mask = 0;
	//WCHAR service_name[] ={'B','T',' ','S','P','P',' ','A','P','L','E','X','t','e','s','t', 0};
 	kal_prompt_trace(MOD_XDM, "5555");
	kal_prompt_trace(MOD_XDM, "[LXZ]In %s", __func__);
	//mmi_bt_spp_init();
	//srv_spp_init();
	g_spp_cntx.sppsrv_hd = srv_spp_open();
	if (g_spp_cntx.sppsrv_hd == SRV_SPP_INVALID_HANDLE) {
		kal_prompt_trace(MOD_XDM, "[LXZ]spp_open error");
		return;
	} else {
		kal_prompt_trace(MOD_XDM, "[LXZ]spp_open succeed");
	}
 
	evt_mask = SRV_SPP_EVENT_BIND_FAIL|
		SRV_SPP_EVENT_AUTHORIZE|
		SRV_SPP_EVENT_CONNECT|
		SRV_SPP_EVENT_READY_TO_WRITE|
		SRV_SPP_EVENT_READY_TO_READ|
		SRV_SPP_EVENT_DISCONNECT;	

	
	srv_spp_set_notify_hdlr(g_spp_cntx.sppsrv_hd, evt_mask, test_bt_spp_event_handler, NULL);
 
	srv_spp_set_security_level(g_spp_cntx.sppsrv_hd, SRV_SPP_SEC_NAN);
 
	srv_spp_bind(g_spp_cntx.sppsrv_hd, MY_TEST_UUID);
	            

       //srv_spp_set_service_name(g_spp_cntx.sppsrv_hd, service_name);
#endif
}

 void spp_start()
 {
	
 }
void test_bt_cm_notifier(U32 event, void* para)
{
	srv_bt_cm_dev_struct *dev_info;
 	srv_bt_cm_power_status_enum bt_power_status;
	U8 pin_key[12] = {0};
	
	kal_prompt_trace(MOD_XDM, "-------test_bt_cm_notifier-------");
 	bt_power_status = srv_bt_cm_get_power_status();
	kal_prompt_trace(MOD_XDM, "bt_power_status = %d", bt_power_status);
	switch (event){
		
        case SRV_BT_CM_EVENT_ACTIVATE:{
		/* After activated, continue to scan for devices */
		kal_prompt_trace(MOD_XDM, "[LXZ]event = SRV_BT_CM_EVENT_ACTIVATE");

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
		kal_prompt_trace(MOD_XDM, "SRV_BT_CM_EVENT_ACTIVATE end2");

		//秘钥修改放到这里先
		//srv_bt_cm_pair_ind_struct * data = (srv_bt_cm_pair_ind_struct *)user_data;
		kal_prompt_trace(MOD_XDM, "test_passkey_input_cb");

		strcpy((CHAR*)pin_key, "0000");
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

 
 void mmi_bt_switch(void)
{
	srv_bt_cm_power_status_enum bt_power_status;
	U8 test_bt_name[] = "LXZ";
	S32 bt_status;
	srv_bt_cm_dev_struct *dev_info;

	kal_prompt_trace(MOD_XDM, "--------mmi_bt_switch-----------------------------");
		//检测蓝牙电源情况
	bt_power_status = srv_bt_cm_get_power_status();
	kal_prompt_trace(MOD_XDM, "bt_power_status%d", bt_power_status);
	if (bt_power_status == SRV_BT_CM_POWER_OFF) {
		kal_prompt_trace(MOD_XDM, "[LXZ]BT OFF");
 		kal_prompt_trace(MOD_XDM, "777");
		srv_bt_cm_set_host_dev_name(test_bt_name);
		srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
		
		bt_status = srv_bt_cm_switch_on();
		kal_prompt_trace(MOD_XDM, "[LXZ]bt_status = %d", bt_status);
	} else if (bt_power_status == SRV_BT_CM_POWER_ON) {
		kal_prompt_trace(MOD_XDM, "888");
		kal_prompt_trace(MOD_XDM, "[LXZ]BT ON");
		srv_bt_cm_set_host_dev_name(test_bt_name);
		srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
 
		kal_prompt_trace(MOD_XDM, "[LXZ]BTCM vm_btcm_get_host_dev_info[%s][0x%02x:%02x:%02x:%02x:%02x:%02x]", dev_info->name,
                ((dev_info->bd_addr.nap & 0xff00) >> 8),
                (dev_info->bd_addr.nap & 0x00ff),
                (dev_info->bd_addr.uap),
                ((dev_info->bd_addr.lap & 0xff0000) >> 16),
                ((dev_info->bd_addr.lap & 0x00ff00) >> 8),
                (dev_info->bd_addr.lap & 0x0000ff));
 
		test_btspp_start();
	}else{
		//StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 5000, mmi_bt_switch);
	}
 }

static void test_passkey_input_cb(void * user_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    srv_bt_cm_pair_ind_struct * data = (srv_bt_cm_pair_ind_struct *)user_data;
    U8 pin_key[12] = {0};
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    kal_prompt_trace(MOD_XDM, "test_passkey_input_cb");

    strcpy((CHAR*)pin_key, "0000");
    if (SRV_BT_CM_RESULT_SUCCESS != srv_bt_cm_send_passkey(&(data->dev_addr), pin_key, MMI_TRUE))
    {
	 kal_prompt_trace(MOD_XDM, "[callback]:[test_passkey_input_cb]:SRV_BT_CM_RESULT_SUCCESS fail");
        //VS_CASE_BTCM_FINISHED(MMI_FALSE, 0, L"vs_btcm_pair");
        //return;
    }else
    	kal_prompt_trace(MOD_XDM, "[callback]:[test_passkey_input_cb]:SRV_BT_CM_RESULT_SUCCESS sucess");
    //DS_OUTLOG("vs_btcm_passkey_input_cb E");        
    // type the key 123 as the pair pin-code
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

 void test_bt_cm_callback(U32 event, void* para)
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
			test_passkey_input_cb(para);
			break;
		case SRV_BT_CM_EVENT_PAIR_RES:		// 3. 匹配结束
			kal_prompt_trace(MOD_XDM, "[bluetooth]SRV_BT_CM_EVENT_PAIR_RES");
			mmi_bt_pair_result_cb(para);
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
void test_bt_init(void)
{
	srv_bt_cm_power_status_enum bt_power_status;
	U8 test_bt_name[] = "APLEXBT";
	S32 bt_status;
	srv_bt_cm_dev_struct *dev_info;
 	static U8 is_init = 0;
	srv_bt_cm_dev_struct* host_info;
	
	if(!is_init)
	{
		memset(&g_spp_cntx , 0x00, sizeof(g_spp_cntx));
		srv_bt_cm_set_host_dev_name(test_bt_name);
		//srv_bt_cm_init();
		//设置回调函数
		kal_prompt_trace(MOD_XDM, "first 111111");
		srv_bt_cm_set_notify((srv_bt_cm_notifier)test_bt_cm_notifier, 
			SRV_BT_CM_EVENT_ACTIVATE |
	        SRV_BT_CM_EVENT_DEACTIVATE |
	        SRV_BT_CM_EVENT_SET_VISIBILITY |
	        SRV_BT_CM_EVENT_SET_NAME, NULL);

		srv_bt_cm_set_notify((srv_bt_cm_notifier)test_bt_cm_callback, 
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
		
	 	kal_prompt_trace(MOD_XDM, "2222");
		//检测蓝牙电源情况
		bt_power_status = srv_bt_cm_get_power_status();
		kal_prompt_trace(MOD_XDM, "bt_power_status=%d", bt_power_status);
		
		if (bt_power_status == SRV_BT_CM_POWER_OFF) {
			kal_prompt_trace(MOD_XDM, "[LXZ]BT OFF");
	 		kal_prompt_trace(MOD_XDM, "3333");
			srv_bt_cm_set_host_dev_name(test_bt_name);
			srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
			
			bt_status = srv_bt_cm_switch_on();
			kal_prompt_trace(MOD_XDM, "[LXZ]bt_status = %d", bt_status);
		} else if (bt_power_status == SRV_BT_CM_POWER_ON) {
			kal_prompt_trace(MOD_XDM, "4444");
			kal_prompt_trace(MOD_XDM, "[LXZ]BT ON");
			srv_bt_cm_set_host_dev_name(test_bt_name);
			srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
	 		//----------------  add by hj
	 		host_info = srv_bt_cm_get_host_dev_info();
			kal_prompt_trace(MOD_XDM, "host name=[%s] mac addr = [0x%02x:%02x:%02x:%02x:%02x:%02x]", host_info->name,
	                ((host_info->bd_addr.nap & 0xff00) >> 8),
	                (host_info->bd_addr.nap & 0x00ff),
	                (host_info->bd_addr.uap),
	                ((host_info->bd_addr.lap & 0xff0000) >> 16),
	                ((host_info->bd_addr.lap & 0x00ff00) >> 8),
	                (host_info->bd_addr.lap & 0x0000ff));
			test_btspp_start();
		}
	}
	else{
		kal_prompt_trace(MOD_XDM, "second 1111");

		host_info = srv_bt_cm_get_host_dev_info();
		
		kal_prompt_trace(MOD_XDM, "host name=[%s] mac addr = [0x%02x:%02x:%02x:%02x:%02x:%02x]", host_info->name,
                ((host_info->bd_addr.nap & 0xff00) >> 8),
                (host_info->bd_addr.nap & 0x00ff),
                (host_info->bd_addr.uap),
                ((host_info->bd_addr.lap & 0xff0000) >> 16),
                ((host_info->bd_addr.lap & 0x00ff00) >> 8),
                (host_info->bd_addr.lap & 0x0000ff));
	}
	
	is_init = 1;
}


void test_bt_init2()
{
	//vs_btcm_start();
	//mmi_bt_scr_init();

	srv_bt_cm_power_status_enum bt_power_status;
	U8 test_bt_name[] = "APLEXBT22";
	S32 bt_status;
	srv_bt_cm_dev_struct *dev_info;
 	static U8 is_init = 0;
	srv_bt_cm_dev_struct* host_info;

	if(!is_init)
	{
		mmi_bt_scr_init();
		bt_power_status = srv_bt_cm_get_power_status();
		kal_prompt_trace(MOD_XDM, "bt_power_status=%d", bt_power_status);
		if (bt_power_status == SRV_BT_CM_POWER_OFF) {
	 		kal_prompt_trace(MOD_XDM, "3333");
			srv_bt_cm_set_host_dev_name(test_bt_name);
			srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
			
			bt_status = srv_bt_cm_switch_on();
			kal_prompt_trace(MOD_XDM, "[LXZ]bt_status = %d", bt_status);
		} else if (bt_power_status == SRV_BT_CM_POWER_ON) {
			kal_prompt_trace(MOD_XDM, "4444");
			srv_bt_cm_set_host_dev_name(test_bt_name);
			srv_bt_cm_set_visibility(SRV_BT_CM_VISIBILITY_ON);
	 		host_info = srv_bt_cm_get_host_dev_info();
			kal_prompt_trace(MOD_XDM, "host name=[%s] mac addr = [0x%02x:%02x:%02x:%02x:%02x:%02x]", host_info->name,
	                ((host_info->bd_addr.nap & 0xff00) >> 8),
	                (host_info->bd_addr.nap & 0x00ff),
	                (host_info->bd_addr.uap),
	                ((host_info->bd_addr.lap & 0xff0000) >> 16),
	                ((host_info->bd_addr.lap & 0x00ff00) >> 8),
	                (host_info->bd_addr.lap & 0x0000ff));
			test_btspp_start();
		}
	}
	else
	{

	}

	is_init = 1;
}

 #endif

 void test_keypower_cb()
 {
	test_send_spp_byaplex();
 }
 /*
void send_spp_test()
 {
	CHAR buf[] = "hello";
	UART_CTRL_OWNER_T port_owner;
	UART_CTRL_PUT_BYTES_T data;
	DCL_HANDLE dcl_handle = DclSerialPort_Open(conn_ind->port, 0);
	DclSerialPort_Control(dcl_handle, SIO_CMD_GET_OWNER_ID, (DCL_CTRL_DATA_T*)&port_owner);
	// if (conn->conn_state != SRV_SPP_CONN_STATE_CONNECTED)

	// return ;
	
	kal_prompt_trace(MOD_XDM, "%s: ownerid is %d", __func__, port_owner.u4OwenrId);
	kal_prompt_trace(MOD_XDM, "%s: MOD_ATCI is %d", __func__, MOD_ATCI);
	kal_prompt_trace(MOD_XDM, "%s: MOD_MMI is %d", __func__, MOD_MMI);
	kal_prompt_trace(MOD_XDM, "%s: MOD_BT is %d", __func__, MOD_BT);

	data.u4OwenrId = MOD_ATCI;
	data.puBuffaddr = buf;
	data.u2Length = strlen(buf);
	DclSerialPort_Control(dcl_handle, SIO_CMD_PUT_BYTES, (DCL_CTRL_DATA_T*)&data);
}
 */

void set_flag_test()
{
	int ret;
	kal_prompt_trace(MOD_XDM, "-------set_flag_test on test-------");
	p_machine_config->switch_flag = 0xffffffff;
	kal_prompt_trace(MOD_XDM, "0switch_flag = %d", p_machine_config->switch_flag);
	
	switch_flag_on(p_machine_config->switch_flag, SWITCH_FLAG_GPRS);
	kal_prompt_trace(MOD_XDM, "1switch_flag = %x", p_machine_config->switch_flag);

	switch_flag_on(p_machine_config->switch_flag, SWITCH_FLAG_GPS);
	kal_prompt_trace(MOD_XDM, "2switch_flag = %x", p_machine_config->switch_flag);

	switch_flag_on(p_machine_config->switch_flag, SWITCH_FLAG_BT);
	kal_prompt_trace(MOD_XDM, "3switch_flag = %x", p_machine_config->switch_flag);

	kal_prompt_trace(MOD_XDM, "4 get switch_flag = %d", switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_GPRS));
	kal_prompt_trace(MOD_XDM, "4 get switch_flag = %d", switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_GPS));
	kal_prompt_trace(MOD_XDM, "4 get switch_flag = %d", switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_BT));


	kal_prompt_trace(MOD_XDM, "-------set_flag_test off test-------");
	p_machine_config->switch_flag = 0;
	kal_prompt_trace(MOD_XDM, "0switch_flag = %x", p_machine_config->switch_flag);
	
	switch_flag_off(p_machine_config->switch_flag, SWITCH_FLAG_GPRS);
	kal_prompt_trace(MOD_XDM, "1switch_flag = %x", p_machine_config->switch_flag);

	switch_flag_off(p_machine_config->switch_flag, SWITCH_FLAG_GPS);
	kal_prompt_trace(MOD_XDM, "2switch_flag = %x", p_machine_config->switch_flag);

	switch_flag_off(p_machine_config->switch_flag, SWITCH_FLAG_BT);
	kal_prompt_trace(MOD_XDM, "3switch_flag = %x", p_machine_config->switch_flag);

	kal_prompt_trace(MOD_XDM, "4 get switch_flag = %d", switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_GPRS));
	kal_prompt_trace(MOD_XDM, "4 get switch_flag = %d", switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_GPS));
	kal_prompt_trace(MOD_XDM, "4 get switch_flag = %d", switch_flag_get(p_machine_config->switch_flag, SWITCH_FLAG_BT));


}

 #endif
 void mtk_helloworld(void)
{
	static int is_init = 0;
	kal_prompt_trace(MOD_XDM, "11111111DDDDDDDDDDD");
	//init_data();
	//测试显示
	//test_display();
	//按键测试,模拟器OK
	//test_key();
	//测试文件读写，开发板OK
	//test_file();
	//SetKeyHandler(init_data, KEY_POWER, KEY_EVENT_UP);//蓝牙发送  OK
	//测试定时器，开发板OK
	//test_timer();
	//测试GPS，不能用模拟器，开发板OK
	//test_gps();	
	//测试时间，开发板OK
	//test_time();
	//测试sim卡的各种信息，开发板OK
	//test_sim();	
	//电话测试，未成功
	//test_call();
	//SetKeyHandler(test_call, KEY_POWER, KEY_EVENT_UP);	//电源键
	//短信测试，未成功
	//test_message();
	//SetKeyHandler(test_message, KEY_POWER, KEY_EVENT_UP);	//电源键
	//测试socket，模拟器OK，开发板(FAIL)，无json，估计是account问题
	//test_socket();
	//测试json
	//test_json();	//OK
	//socket测试
	//SetKeyHandler(mmi_my_powerkey_func, KEY_POWER, KEY_EVENT_UP);
	//SetKeyHandler(mmi_my_powerkey_func, KEY_LSK, KEY_EVENT_UP);
	//mmi_my_powerkey_func();
	//SetKeyHandler(mmi_my_powerkey_func, KEY_POWER, KEY_EVENT_UP);	//电源键
	//测试MQTT，OK了
	//test_mqtt();
	//电源检测
	//test_power();
	//GPIO驱动测试//OK
	//test_drv_gpio();
	//线程通信测试
	//test_conn_task();
	//蓝牙测试
	//test_bt_init();
	//SetKeyHandler(test_keypower_cb, KEY_POWER, KEY_EVENT_UP);//蓝牙发送  OK
	//test_bt_init2();
	//设置测试
	//set_flag_test();
}
