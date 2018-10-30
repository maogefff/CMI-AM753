#include "AppMain.h"
#include "gui_themes.h"
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

#include "MQTTFormat.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"
#include "StackTrace.h"

#include "FileData.h"
#include "Socket.h"
#include "cJSON.h"

struct s_mqtt_data* p_mqtt_config;
struct s_net_config* p_net_config;

g_mqtt_conn mqtt;
MQTTPacket_connectData mqtt_socket_data = MQTTPacket_connectData_initializer;
MQTTString mqtt_socket_topicString = MQTTString_initializer;
char* send_buff_mqtt = NULL;

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

			//mqtt.msgtypes = SUBSCRIBE;
			mqtt_set_type(SUBSCRIBE);
			fresh_mqtt();
			//启动定时器，1秒后再次进入
			//StartTimer(TIMER_ID_MY_CUSTOM_TIMER, 1000, fresh_mqtt);
			break;

		case SUBSCRIBE:        //订阅消息
			kal_prompt_trace(MOD_XDM, "[TYPE SUBSCRIBE]");
			//mqtt_socket_topicString.cstring = "test-iot-sub";
			mqtt_socket_topicString.cstring = p_mqtt_config->topic;	//订阅的消息
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
			//拆包
			mqtt.rc = MQTTDeserialize_publish(&mqtt.dup, &mqtt.qos, &mqtt.retained, &mqtt.msgid, &mqtt.receivedTopic,
											&mqtt.payload_in, &mqtt.payloadlen_in, mqtt.buf, mqtt.buflen);

			kal_prompt_trace(MOD_XDM, "payloadlen_in = %d, message arrived payload_in = %s", mqtt.payloadlen_in, mqtt.payload_in);
			kal_prompt_trace(MOD_XDM, "payload_in1 = %s", mqtt.payload_in+20);
			kal_prompt_trace(MOD_XDM, "payload_in4 = %s", mqtt.payload_in+160);
			kal_prompt_trace(MOD_XDM, "payload_in4 = %s", mqtt.payload_in+180);

			
			prase_json(mqtt.payload_in);

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

static void init_mqtt()
{
	
	mqtt_socket_data.keepAliveInterval = 50;
	mqtt_socket_data.cleansession = 1;
	mqtt_socket_data.MQTTVersion = 3;		//这里修改为3
#if 0
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
#endif
	//用户名
	mqtt_socket_data.username.cstring = p_mqtt_config->username;
	//密码
	mqtt_socket_data.password.cstring = p_mqtt_config->passwd;
	//客户ID
	mqtt_socket_data.clientID.cstring = p_mqtt_config->clientID;
	//网址
	memset(mqtt.host, 0x0, sizeof(mqtt.host));
	strcpy(mqtt.host, p_net_config->website);
	//端口
	mqtt.port = p_net_config->port;
	
	mqtt.msgtypes= CONNECT;
	mqtt.rc = 0;
	mqtt.buflen = sizeof(mqtt.buf);
	memset(mqtt.buf, 0x00, mqtt.buflen);
	mqtt.msgid = 1;
	mqtt.granted_qos =0;
	mqtt.len = 0;
	mqtt.req_qos = 1;
}

void publish_message_func(char*  buff)
{
	kal_prompt_trace(MOD_XDM, "publish_func");
	//发布消息
	test_mqtt_publish(p_mqtt_config->topic, buff);
}

int transport_getdata_mqtt(unsigned char* buf, int count)
{
	return count;
}

//hj
static void socket_mqtt_callback(enum_socket_state state, void *data_buff, kal_int32 size)
{
	int i=0;
	unsigned char*  buff = (unsigned char*)data_buff;
	kal_prompt_trace(MOD_XDM, "socket_test_callback  state=%d", state);

	switch(state)
	{
		case SOC_STATE_SEND_SUCCESS:
			kal_prompt_trace(MOD_XDM, "send data");
			break;
		case SOC_STATE_RECV_SUCCESS:
			kal_prompt_trace(MOD_XDM, "recv data");
			//返回的是类型
			mqtt.rc=MQTTPacket_read(data_buff, size, transport_getdata_mqtt);  
			//mqtt_socket_rc = buff[0];
			kal_prompt_trace(MOD_XDM,"mqtt_socket_rc = %d", mqtt.rc);
			if(mqtt.rc >0)
			{
				//mqtt.msgtypes = mqtt.rc;		//这里!!!
				mqtt_set_type(mqtt.rc);
				kal_prompt_trace(MOD_XDM,"MQTT is get recv:");
				memset(mqtt.buf, 0x00, sizeof(mqtt.buf));
				memcpy(mqtt.buf, buff, size);	
				fresh_mqtt();
			}
			//prase_json((kal_char*)data_buff);
			
			break;
	}
}

void mqtt_set_type(unsigned char type)
{
	mqtt.msgtypes = type;
}
void mqtt_init(void)
{
	p_mqtt_config = get_mqtt_config_data();
	p_net_config = get_net_config_data();
	
	init_mqtt();
	mmi_socket_init(MMI_SIM1, socket_mqtt_callback);
	mqtt_set_type(CONNECT);
	fresh_mqtt();//MQTT请求连接
}

