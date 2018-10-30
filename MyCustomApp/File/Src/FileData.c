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
#include "SppSrvGprot.h"
#include "BTMMISpp.h"
#include "mmi_frm_events_gprot.h"
#include "GlobalResDef.h"
#include "MMI_trc_Int.h"
#include "mmi_rp_app_bluetooth_def.h"
#include "mmi_frm_scenario_gprot.h"
#include "mmi_cb_mgr_gprot.h"

#include "FileData.h"

#define DATA_FILE "appData.txt"

//全局变量
struct file_data g_file_data;

struct file_data* get_file_data(void)
{
	return &g_file_data;
}

struct s_mqtt_data* get_mqtt_config_data(void)
{
	return &g_file_data.mqtt_data;
}

struct s_bt_config* get_bt_config_data(void)
{
	return &g_file_data.bt_config;
}

struct s_machine_config* get_machine_config_data(void)
{
	return &g_file_data.machine_config;
}

struct s_net_config* get_net_config_data(void)
{
	return &g_file_data.net_config;
}


static void file_print(struct file_data* p_data)
{
	kal_prompt_trace(MOD_XDM, "machine_serial_number:%s", p_data->machine_serial_number);
	kal_prompt_trace(MOD_XDM, "isTestMode:%d", p_data->isTestMode);
	kal_prompt_trace(MOD_XDM, "bcc:%d", p_data->bcc);

	kal_prompt_trace(MOD_XDM, "-------mqtt:");
	kal_prompt_trace(MOD_XDM, "username:%s", p_data->mqtt_data.username);
	kal_prompt_trace(MOD_XDM, "machine_serial_number:%s", p_data->mqtt_data.passwd);
	kal_prompt_trace(MOD_XDM, "topic:%s", p_data->mqtt_data.topic);
	kal_prompt_trace(MOD_XDM, "clientID:%s", p_data->mqtt_data.clientID);

	kal_prompt_trace(MOD_XDM, "-------net:");
	kal_prompt_trace(MOD_XDM, "config.website:%s", p_data->net_config.website);
	kal_prompt_trace(MOD_XDM, "config.port:%d", p_data->net_config.port);

	kal_prompt_trace(MOD_XDM, "-------bt:");
	kal_prompt_trace(MOD_XDM, "config.bt_name:%s", p_data->bt_config.bt_name);
	kal_prompt_trace(MOD_XDM, "config.bt_pwd:%s", p_data->bt_config.bt_pwd);

	kal_prompt_trace(MOD_XDM, "-------machine:");
	kal_prompt_trace(MOD_XDM, "config.temp_offset:%d", p_data->machine_config.temp_offset);
	kal_prompt_trace(MOD_XDM, "config.workmode:%d", p_data->machine_config.workmode);
	kal_prompt_trace(MOD_XDM, "config.switch_flag:%d", p_data->machine_config.switch_flag);
	kal_prompt_trace(MOD_XDM, "config.ref_time:%d", p_data->machine_config.ref_time);
}

//bcc校验
U8 CheckSum(U8* data, int len)
{
	U8 CheckCode = 1;
	int i = 0;
	//异或校验
	for (i = 0; i < len; i++)
	{
		CheckCode ^= data[i];
	}
	kal_prompt_trace(MOD_XDM, "[i = %d]:len = %d, CheckCode = %d\n", i, len, CheckCode);
	kal_prompt_trace(MOD_XDM, "data[0]=%d\ndata[1]=%d\ndata[2]=%d\ndata[3]=%d\ndata[4]=%d\ndata[5]=%d\ndata[6]=%d\ndata[7]=%d\n", 
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

	kal_prompt_trace(MOD_XDM, "[i = %d]:len = %d, CheckCode = %d\n", i, len, CheckCode);

	kal_prompt_trace(MOD_XDM, "data[%d]=%d\ndata[%d]=%d\ndata[%d]=%d\ndata[%d]=%d\ndata[%d]=%d\ndata[%d]=%d\ndata[%d]=%d\ndata[%d]=%d\n", 
		i-8, data[i-8], i-7, data[i-7], i-6, data[i-6], i-5, data[i-5], i-4, data[i-4], i-3, data[i-3], i-2, data[i-2], i-1, data[i-1]);

	return CheckCode;
}

static void init_default_filedata(void)
{
	char file[50];
	int fd;
	int wl;
	
	memset(file, 0x00, sizeof(file));
	memset(&g_file_data, 0x00, sizeof(g_file_data));
	
	//kal_wsprintf((unsigned short *)file, "%c:\\appData2.txt", SRV_FMGR_SYSTEM_DRV);
	kal_wsprintf((unsigned short *)file, "%c:\\%s", SRV_FMGR_SYSTEM_DRV, DATA_FILE);
	
	fd = FS_Open((WCHAR *)file, FS_READ_WRITE | FS_CREATE);
	if(fd<0)
	{
		kal_prompt_trace(MOD_XDM, "file open write error:%d\n", fd);
	}
	//串号
	strcpy(g_file_data.machine_serial_number, "1234567890");
	//测试模式
	g_file_data.isTestMode = 0;

	//mqtt配置
	strcpy(g_file_data.mqtt_data.username, "zengjf/sz_monitor_room");
	strcpy(g_file_data.mqtt_data.passwd, "QE0BHFvFnIkBRIaJtPYzo3m/63Esv5fzzMr9tYVOsHo=");
	strcpy(g_file_data.mqtt_data.topic, "test-iot-sub");
	strcpy(g_file_data.mqtt_data.clientID, "MQTT_FX_Client124");

	//网络配置
	strcpy(g_file_data.net_config.website, "zengjf.mqtt.iot.gz.baidubce.com");
	g_file_data.net_config.port = 1883;

	//蓝牙配置
	strcpy(g_file_data.bt_config.bt_name ,	"capstone");
	strcpy(g_file_data.bt_config.bt_pwd ,	"123456");

	//机器开关
	switch_flag_on(g_file_data.machine_config.switch_flag , SWITCH_FLAG_GPRS);
	switch_flag_off(g_file_data.machine_config.switch_flag , SWITCH_FLAG_GPS);
	switch_flag_on(g_file_data.machine_config.switch_flag , SWITCH_FLAG_BT);
	g_file_data.machine_config.temp_offset = 0;
	g_file_data.machine_config.workmode = 0;
	g_file_data.machine_config.ref_time = 300;

	//校验
	g_file_data.bcc = CheckSum((U8 *)&g_file_data+sizeof(g_file_data.bcc), sizeof(g_file_data)-sizeof(g_file_data.bcc));
	kal_prompt_trace(MOD_XDM, "[file] bcc = %d\n", g_file_data.bcc);
	
	FS_Write(fd, &g_file_data, sizeof(g_file_data), &wl);
	FS_Commit(fd);
	//关闭
	FS_Close(fd);

}

void init_data(void)
{
	char file[50];
	int fd;
	int rl;
	int ret;
	U8 bcc;
	
	memset(file, 0x00, sizeof(file));
	kal_prompt_trace(MOD_XDM, "[file] init_data\n");
	
	kal_wsprintf((unsigned short *)file, "%c:\\%s", SRV_FMGR_SYSTEM_DRV, DATA_FILE);
	//打开文件
	fd = FS_Open((WCHAR *)file, FS_READ_WRITE );

	if(fd<0)
	{
		//FS_Close(fd);
		FS_Delete((WCHAR *)file);
		kal_prompt_trace(MOD_XDM, "[file] open read error:%d\n", fd);
		init_default_filedata();
		fd = FS_Open((WCHAR *)file, FS_READ_ONLY | FS_CREATE);
	}

	kal_prompt_trace(MOD_XDM, "[file] init sucess\n");
	memset(&g_file_data, 0x00, sizeof(g_file_data));
	
	//读文件
	FS_Read(fd, &g_file_data, sizeof(g_file_data), &rl);
	file_print( &g_file_data );
	//校验数据
	bcc = CheckSum((U8 *)&g_file_data+sizeof(g_file_data.bcc), sizeof(g_file_data)-sizeof(g_file_data.bcc));
	kal_prompt_trace(MOD_XDM, "g_file_data.bcc = %d, bcc=%d", g_file_data.bcc, bcc);
	
	if(g_file_data.bcc != bcc)
	{
		FS_Close(fd);
		FS_Delete((WCHAR *)file);

		init_default_filedata();
		fd = FS_Open((WCHAR *)file, FS_READ_ONLY | FS_CREATE);
		memset(&g_file_data, 0x00, sizeof(g_file_data));
		//读文件
		FS_Read(fd, &g_file_data, sizeof(g_file_data), &rl);
	}
	file_print( &g_file_data );

	//关闭
	FS_Close(fd);
}



