#ifndef __FILEDATA_H__
#define __FILEDATA_H__

#include "Fs_gprot.h"
#include "mmi_rp_app_mycustom_def.h"
#include "Mmi_frm_scenario_gprot.h"
#include "GlobalResDef.h"

#define	MACHINE_SERIAL_NUMBER_SIZE 15

#define MQTT_USERNAME_SIZE 50
#define MQTT_PASSWD_SIZE 	 50
#define MQTT_TOPIC_SIZE 	 50
#define MQTT_CLIENTID_SIZE 	 50

#define BT_NAME_SIZE 	 50
#define BT_PWD_SIZE 	 50

#define NET_WEBSITE_SIZE 	 50

#define SWITCH_ON 	1
#define SWITCH_OFF  0
#define SWITCH_FLAG_GPRS	0x00000001
#define SWITCH_FLAG_GPS	0x00000002
#define SWITCH_FLAG_BT		0x00000004

#define switch_flag_on(f, s)  	(f = (f & (~s)))
#define switch_flag_off(f, s)	(f = (f | s))
#define switch_flag_get(f, s)	(((~f) & s) != 0 ? SWITCH_ON : SWITCH_OFF)

typedef struct s_mqtt_data{
	char username[MQTT_USERNAME_SIZE];
	char passwd[MQTT_PASSWD_SIZE];
	char topic[MQTT_TOPIC_SIZE];
	char clientID[MQTT_CLIENTID_SIZE];
}s_mqtt_data;

typedef struct s_net_config{
	char website[NET_WEBSITE_SIZE];
	int port;
}s_net_config;


typedef struct s_bt_config{
	char bt_name[BT_NAME_SIZE];	//蓝牙默认名称:capstone
	char bt_pwd[BT_PWD_SIZE];	//蓝牙默认密码:123456
}s_bt_config;

typedef struct s_machine_config{
	int temp_offset; 	//温度补偿参数:2byte表示小数点左边，2byte表示小数点右边
	int workmode;	//工作模式:0:工作模式1:调试模式
	//第1位:	0:GPRS开1:GPRS闭,默认常开
	//第2位:	0:GPS开  1:GPS闭,默认常闭
	//第3位:	0:蓝牙开1:蓝牙常,默认常闭
	unsigned int switch_flag;	 
	unsigned int ref_time;	//云端上传间隔,单位秒
}s_machine_config;

typedef struct file_data{
	U8 bcc;	//校验和
	U8 isTestMode;	//0:工作模式, 1:调试模式     是否为测试模式
	U8 machine_serial_number[MACHINE_SERIAL_NUMBER_SIZE];
	struct s_mqtt_data mqtt_data;
	struct s_net_config net_config;
	struct s_bt_config bt_config;
	struct s_machine_config machine_config;
}file_data;


extern void init_data(void);

struct file_data* get_file_data(void);
struct s_mqtt_data* get_mqtt_config_data(void);
struct s_bt_config* get_bt_config_data(void);
struct s_machine_config* get_machine_config_data(void);
struct s_net_config* get_net_config_data(void);


#endif

