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
	char host[50];   //�����ַ
	int port;		//�˿ں�
	unsigned char  msgtypes ;	
	MQTTString receivedTopic;
	int rc;
	char buf[400];	//���ͻ���	
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
	float longitude;	//����
	float latitude;	//ά��
	
}g_package;


extern void mtk_helloworld(void);		//���Դ���
extern void main_task_start(void);

extern void mqtt_init(void);
extern void mmi_prase_bt_data(char* buf);
extern void mqtt_set_type(unsigned char type);
#if 0
extern void publish_func();		//���Ժ���
#endif
extern void publish_message_func(char*  buff);
extern void prase_json(kal_char* buf);


//��Ҫʹ��
//����
int bt_time_handler(char* buf);
int bt_date_handler(char* buf);
int bt_serial_number_handler(char* buf);
int bt_switch_onoff_handler(char* buf);
//MQTT����
char* mqtt_init_handler(void);
//JSON����
int json_time_handler(cJSON *body);
int json_serial_number_handler(cJSON *body);
int json_BasicInfo_handler(cJSON *body);



#endif
