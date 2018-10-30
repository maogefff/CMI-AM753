#ifndef __APPMAIN_H__
#define __APPMAIN_H__

//#include "transport.h"
//#include "MQTTConnect.h"
#include "gdi_include.h"
#include "mdi_datatype.h"

#include "MQTTFormat.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"
#include "StackTrace.h"
#include "cJSON.h"

typedef struct
{
	char host[50];   //网络地址
	int port;		//端口号
	unsigned char  msgtypes ;	
	MQTTString receivedTopic;
	int rc;
	char buf[400];	//发送缓冲	
	int buflen;
	int payloadlen_in;
	unsigned char* payload_in;
	unsigned short msgid;
	int subcount;
	int granted_qos;
	unsigned char sessionPresent;
	unsigned char connack_rc;
	unsigned short submsgid;
	int len;
	int req_qos;
	unsigned char dup;
	int qos;
	unsigned char retained;
} g_mqtt_conn;

typedef struct 
{
	float longitude;	//经度
	float latitude;	//维度
	
}g_package;


extern void mtk_helloworld(void);		//测试代码
extern void main_task_start(void);

extern void mqtt_init(void);
extern void mmi_prase_bt_data(char* buf);
extern void mqtt_set_type(unsigned char type);
#if 0
extern void publish_func();		//测试函数
#endif
extern void publish_message_func(char*  buff);
extern void prase_json(kal_char* buf);


//不要使用
//蓝牙
int bt_time_handler(char* buf);
int bt_date_handler(char* buf);
int bt_serial_number_handler(char* buf);
int bt_switch_onoff_handler(char* buf);
//MQTT部分
char* mqtt_init_handler(void);
//JSON部分
int json_time_handler(cJSON *body);
int json_serial_number_handler(cJSON *body);
int json_BasicInfo_handler(cJSON *body);



#endif
