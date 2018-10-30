/*Needed header files of the compile option in XML files */
#include "mmi_features.h"

#ifdef __MMI_FM_RADIO_SCHEDULE_REC__

#include "CustResDef.h"         /* for image/audio path */

/* Need this line to tell parser that XML start, must after all #include. */
<?xml version="1.0" encoding="UTF-8"?>

<APP id="APP_FMSCHEDULEREC" name="STR_ID_FMSR_BASE">
	<INCLUDE file="GlobalResDef.h"/>
	<INCLUDE file="mmi_rp_app_mainmenu_def.h"/>


	<STRING id="STR_ID_FMSR_APP"/>
	<STRING id="STR_ID_FMSR_SAVE_CONFIRM"/>
	<STRING id="STR_ID_FMSR_CHAN_SETTING"/>
	<STRING id="STR_ID_FMSR_CHAN_LIST"/>
	<STRING id="STR_ID_FMSR_CHAN_MANUAL"/>
	<STRING id="STR_ID_FMSR_REC_SETTING"/>
	<STRING id="STR_ID_FMSR_REPEAT"/>
	<STRING id="STR_ID_FMSR_ONCE"/>
	<STRING id="STR_ID_FMSR_EVERYDAY"/>
	<STRING id="STR_ID_FMSR_WEEKLY"/>
	<STRING id="STR_ID_FMSR_MONTHLY"/>
	<STRING id="STR_ID_FMSR_REC_FORMAT"/>
	<STRING id="STR_ID_FMSR_REC_TYPE"/>
	<STRING id="STR_ID_FMSR_AMR"/>
	<STRING id="STR_ID_FMSR_WAV"/>

#if defined(__I2S_DAF_RECORD__)
	<STRING id="STR_ID_FMSR_DAF"/>
#endif

#if !defined(__FMRADIO_I2S_SUPPORT__) || defined(__I2S_AWB_RECORD__)
#if defined(AMRWB_ENCODE) && defined(AMRWB_DECODE)
	<STRING id="STR_ID_FMSR_AWB"/>
#endif
#endif

#ifndef __FMRADIO_I2S_SUPPORT__
	<STRING id="STR_ID_FMSR_REC_QUALITY"/>
#endif

	<STRING id="STR_ID_FMSR_NEW_FILE"/>
	<STRING id="STR_ID_FMSR_APPEND_FILE"/>
	<STRING id="STR_ID_FMSR_RECORDING"/>
	<STRING id="STR_ID_FMSR_BAD_FORMAT"/>
	<STRING id="STR_ID_FMSR_INVALID_FORMAT"/>
	<STRING id="STR_ID_FMSR_DEVICE_BUSY"/>
	<STRING id="STR_ID_FMSR_OPEN_FILE_ERROR"/>
	<STRING id="STR_ID_FMSR_NO_CARD"/>
	<STRING id="STR_ID_FMSR_WRITE_PROTECTION_ERROR"/>
	<STRING id="STR_ID_FMSR_VALID_FREQUENCY"/>
	<STRING id="STR_ID_FMSR_FILENAME_TOO_LONG"/>
	<STRING id="STR_ID_FMSR_FILE_EMPTY"/>
	<STRING id="STR_ID_FMSR_END_TIME_INCORRECT"/>
	<STRING id="STR_ID_FMSR_START_TIME_INCORRECT"/>
	<STRING id="STR_ID_FMSR_CUSTOM_DAYS_INCORRECT"/>
	<STRING id="STR_ID_FMSR_TIME_OVERLAP"/>
	<STRING id="STR_ID_FMSR_SYSTEM_INTERRUPT"/>
	<STRING id="STR_ID_FMSR_USER_STOP"/>
	<STRING id="STR_ID_FMSR_USER_PAUSE"/>
	<STRING id="STR_ID_FMSR_USE_DEFAULT_FILE"/>
	<STRING id="STR_ID_FMSR_TIME_EXPIRE"/>
	<STRING id="STR_ID_FMSR_INVALID_FREQUENCY"/>
	<STRING id="STR_ID_FMSR_CANT_CREATE_FOLDER"/>
	<STRING id="STR_ID_FMSR_IN_USB_MODE"/>
	<STRING id="STR_ID_FMSR_RECORD_IN_BT_SCO"/>
	<STRING id="STR_ID_FMSR_RECORD_FINISH_1"/>
	<STRING id="STR_ID_FMSR_RECORD_FINISH_2"/>
	<STRING id="STR_ID_FMSR_RECORD_FINISH_3"/>
	<STRING id="STR_ID_FMSR_RECORD_FINISH_4"/>
	<STRING id="STR_ID_FMSR_APPEND_ERORR"/>

	<IMAGE id="IMG_ID_FMSR_APP">CUST_IMG_PATH"\\\\MainLCD\\\\FMScheduleRec\\\\SB_FMschedule.bmp"</IMAGE>

#ifdef __MMI_FMSR_SKIN_SYN_TO_THEME__
	<IMAGE id="IMG_ID_FMSR_BACKGROUND">CUST_IMG_PATH"\\\\MainLCD\\\\FMScheduleRec\\\\FMS_REC.png"</IMAGE>
#else
	<IMAGE id="IMG_ID_FMSR_BACKGROUND">CUST_IMG_PATH"\\\\MainLCD\\\\FMScheduleRec\\\\FMS_REC.gif"</IMAGE>
#endif /*__MMI_FMSR_SKIN_SYN_TO_THEME__*/


<!--Menu Tree Area-->
<MENU id="MENU_ID_FMSR_MAIN" type="APP_MAIN" parent="MAIN_MENU_MULTIMEDIA_MENUID" str="STR_ID_FMSR_APP" img="IMG_ID_FMSR_APP" highlight="mmi_fmsr_highlight_app"/>

<!--Menu Item Area-->
<MENUITEM id="MENU_ID_FMSR_CHAN_LIST" str="STR_ID_FMSR_CHAN_LIST" img="IMG_ID_FMSR_APP" highlight="mmi_fmsr_highlight_chan_list"/>
<MENUITEM id="MENU_ID_FMSR_CHAN_MANUAL" str="STR_ID_FMSR_CHAN_MANUAL" img="IMG_ID_FMSR_APP" highlight="mmi_fmsr_highlight_chan_manual"/>


<!--Menu Tree Area-->
<MENU id="MENU_ID_FMSR_CHAN_SETTING" parent="0" type="OPTION" str="STR_ID_FMSR_CHAN_SETTING" img="IMG_ID_FMSR_APP">
	<MENUITEM_ID>MENU_ID_FMSR_CHAN_LIST</MENUITEM_ID>
	<MENUITEM_ID>MENU_ID_FMSR_CHAN_MANUAL</MENUITEM_ID>
</MENU>

	<SCREEN id="SCR_ID_FMSR_BASE"/>
	<SCREEN id="SCR_ID_FMSR_MAIN_LIST"/>
	<SCREEN id="SCR_ID_FMSR_MAIN_SETTING"/>
	<SCREEN id="SCR_ID_FMSR_CHAN_SETTNIG"/>
	<SCREEN id="SCR_ID_FMSR_CHAN_LIST"/>
	<SCREEN id="SCR_ID_FMSR_CHAN_MANUAL"/>
	<SCREEN id="SCR_ID_FMSR_REC_SETTING"/>
	<SCREEN id="SCR_ID_FMSR_RECORDING"/>
	<SCREEN id="SCR_ID_FMSR_SHOW_MESSAGE"/>

    <TIMER id="FMSR_CHCEK_STOP_TIMER"/>

</APP>
#endif // __MMI_FM_RADIO_SCHEDULE_REC__
