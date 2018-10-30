#ifndef __LED_H__
#define __LED_H__

#include "MMI_features.h"

#include "MMI_include.h"
#include "MMIDataType.h"

#define STATE_NORMAL 0x00
#define STATE_GPS	   0x01
#define STATE_BT	   0x02
#define STATE_CHARGE 0x10
#define is_charge(n)	(n >>4 )
#define get_state(n)	(n & 0xff)

typedef enum e_led_state
{
	e_state_normal = 0,
	e_state_gprs,
	e_state_bt,
	e_state_charging, //正在充电
	e_state_charge_full	//已经充满电
}e_led_state;

void led_init(void);
void led_set_status(enum e_led_state state);

#endif
