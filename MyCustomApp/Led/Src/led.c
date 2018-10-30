#include "led.h"
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

typedef enum e_color
{
	eREAD = 0,
	eGREEN,
	eALLCOLOR
}e_color;

typedef struct s_LED
{
	kal_uint32 on_time_ms;	//亮灯时长
	kal_uint32 off_time_ms;	//灭灯时长
	e_color color;
	kal_bool status;	//此时亮灯还是灭灯
	kal_uint16 pin_1;
	kal_uint16 pin_2;
	kal_bool isCharge;	//是否正在充电
} s_LED;

static struct s_LED led;

static void set_led_off(void)
{
	switch(led.color)
	{
		case eREAD:
			kal_prompt_trace(MOD_XDM, "led read off");
			GPIO_WriteIO(1, led.pin_1);
			GPIO_WriteIO(1, led.pin_2);
			break;
		case eGREEN:
			kal_prompt_trace(MOD_XDM, "led green off");
			GPIO_WriteIO(1, led.pin_1);
			GPIO_WriteIO(1, led.pin_2);
			break;
		default:
			break;
	}
}

static void set_led_on(void)
{
	switch(led.color)
	{
		case eREAD:
			kal_prompt_trace(MOD_XDM, "led read on");
			GPIO_WriteIO(0, led.pin_1);
			GPIO_WriteIO(0, led.pin_2);
			break;
		case eGREEN:
			kal_prompt_trace(MOD_XDM, "led green on");
			GPIO_WriteIO(0, led.pin_1);
			GPIO_WriteIO(0, led.pin_2);
			break;
		default:
			kal_prompt_trace(MOD_XDM, "led error");
			break;
	}
}

static void mmi_task_led(void)
{		
	kal_prompt_trace(MOD_XDM, "mmi_task_led isCharge=%d, status=%d", led.isCharge, led.status);
	
	if(led.isCharge == KAL_TRUE)
	{
		set_led_on();
		StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK1, 500, mmi_task_led);
	}
	else
	{
		if(led.status == TRUE)
		{
			led.status = FALSE;
			set_led_on();
			StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK1, led.on_time_ms, mmi_task_led);
		}
		else
		{
			led.status = TRUE;
			set_led_off();
			StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK1, led.off_time_ms, mmi_task_led);
		}		
	}
}

//检测是否正在充电
static void mmi_task_detect_charge(void)
{
#if 1
	MMI_BOOL ret;
	battery_level_enum level;

	ret = srv_charbat_is_charging();	//是否正在充电

	if(ret==MMI_TRUE)
	{
		
		level = srv_charbat_get_battery_level();

		if(level < BATTERY_LEVEL_3)
		{
			led_set_status(e_state_charging);
			kal_prompt_trace(MOD_XDM, "[power]is charging");
		}
		else
		{
			led_set_status(e_state_charge_full);
			kal_prompt_trace(MOD_XDM, "[power]is charge full");
		}
			
	}
	else
	{
		kal_prompt_trace(MOD_XDM, "[power]no charging");
		//led_set_status(e_state_normal);
	}
#endif
}

void led_set_status(enum e_led_state state)
{
	kal_prompt_trace(MOD_XDM, "led_set_status:%d", state);
	
	switch(state)
	{
		case e_state_normal:
			led.on_time_ms = 100;			
			led.off_time_ms = 4900;	//5秒闪一次，也就是灭灯时间为4.9秒
			led.color = eGREEN;
			break;
		case e_state_gprs:
			led.on_time_ms = 100;			
			led.off_time_ms = 400;		// 1秒闪两次，也就是灭灯时间为400ms秒
			led.color = eGREEN;
			break;
		case e_state_bt:
			led.on_time_ms = 100;			
			led.off_time_ms = 235;		// 1秒闪三次，也就是灭灯时间为235ms秒
			led.color = eGREEN;
			break;
		case e_state_charging:
			led.color = eGREEN;
			led.on_time_ms = 500;
			break;
		case e_state_charge_full:
			led.on_time_ms = 500;
			led.color = eREAD;
			break;
		default:
			break;
	}
}

static void mmi_led_test(void)
{
	static int stat = e_state_normal;
	
	led_set_status(stat);
		stat++;

	if(stat > e_state_charge_full)
		stat = e_state_normal;
	
	StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK3, 20000, mmi_led_test);
}

void led_init(void)
{
	kal_prompt_trace(MOD_XDM, "------------led_init------------");
	memset(&led, 0x0, sizeof(led));
	led.on_time_ms = 0;
	led.off_time_ms = 1000;
	led.pin_1 = 5;
	led.pin_2 = 6;
	led.color = eGREEN;
	led.isCharge = KAL_FALSE;
	
	GPIO_ModeSetup(led.pin_1 , 0);
	GPIO_ModeSetup(led.pin_2 , 0);
	GPIO_InitIO(1,led.pin_1);
	GPIO_InitIO(1,led.pin_2);

	led_set_status(e_state_normal);
	
	StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK1, led.on_time_ms+led.off_time_ms, mmi_task_led);
	//检测是否正在充电
	StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK2, 1000, mmi_task_detect_charge);
	//测试
	StartTimer(TIMER_ID_MY_CUSTOM_LED_TASK3, 20000, mmi_led_test);
}



