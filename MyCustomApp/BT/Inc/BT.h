#ifndef __BT_H__
#define __BT_H__

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


extern void mmi_bt_open(struct s_bt_config* config);
extern void mmi_bt_close();

#endif


