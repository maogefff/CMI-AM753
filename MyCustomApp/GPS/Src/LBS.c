
#include "mmi_features.h"

#if defined(__MMI_MYSTUDY_LOCATION__)
/**********************************************************************
* 头文件
**********************************************************************/

#include "LBS.h"
#include "nbr_public_struct.h"
#include "mmi_frm_events_gprot.h"
/**********************************************************************
/* 宏常量定义                                   
**********************************************************************/

/**********************************************************************
/* 全局变量   
**********************************************************************/
static lbs_data_struct g_lbs_data = {0x00};

/**********************************************************************
/* 函数声明   
**********************************************************************/

/**********************************************************************
/* 函数体
**********************************************************************/
/*获取基站*/
static void mmi_lbs_cell_info(l4c_nbr_cell_info_ind_struct *msg_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
	gas_nbr_cell_info_struct cell_info={0x00};
	U8 i = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(NULL == msg_ptr)
	{
        return;
	}
	if (KAL_TRUE == msg_ptr->is_nbr_info_valid)
	{
		memcpy((void *)&cell_info, (void *)(&(msg_ptr->ps_nbr_cell_info_union.gas_nbr_cell_info)), sizeof(gas_nbr_cell_info_struct));
	}
	else
	{
		memset((void *)&cell_info, 0x00, sizeof(gas_nbr_cell_info_struct));    
	}
    
	kal_prompt_trace(MOD_WAP, "-lbs-%d(cell_num=%d)-%s-", __LINE__, cell_info.nbr_cell_num,__FILE__);
	if(cell_info.nbr_cell_num <= 0)
	{
		return;
	}

	memset(&g_lbs_data, 0x00, sizeof(g_lbs_data));
	g_lbs_data.count = cell_info.nbr_cell_num+1;/*搜索到的基站个数*/
    if(g_lbs_data.count > LBS_SAL_CELL_NBR_MAX)
    {
        g_lbs_data.count = LBS_SAL_CELL_NBR_MAX; 
    }
	g_lbs_data.info[0].arfcn = cell_info.nbr_meas_rslt.nbr_cells[cell_info.serv_info.nbr_meas_rslt_index].arfcn;
	g_lbs_data.info[0].bsic = cell_info.nbr_meas_rslt.nbr_cells[cell_info.serv_info.nbr_meas_rslt_index].bsic;
	g_lbs_data.info[0].rxlev = cell_info.nbr_meas_rslt.nbr_cells[cell_info.serv_info.nbr_meas_rslt_index].rxlev;
	g_lbs_data.info[0].mcc = cell_info.serv_info.gci.mcc;
	g_lbs_data.info[0].mnc = cell_info.serv_info.gci.mnc;
	g_lbs_data.info[0].lac = cell_info.serv_info.gci.lac;
	g_lbs_data.info[0].ci = cell_info.serv_info.gci.ci;

	for(i = 0; i<cell_info.nbr_cell_num && i<LBS_SAL_CELL_NBR_MAX-1; i++)
	{
		g_lbs_data.info[i+1].arfcn = cell_info.nbr_meas_rslt.nbr_cells[cell_info.nbr_cell_info[i].nbr_meas_rslt_index].arfcn;
		g_lbs_data.info[i+1].bsic = cell_info.nbr_meas_rslt.nbr_cells[cell_info.nbr_cell_info[i].nbr_meas_rslt_index].bsic;
		g_lbs_data.info[i+1].rxlev = cell_info.nbr_meas_rslt.nbr_cells[cell_info.nbr_cell_info[i].nbr_meas_rslt_index].rxlev;
		g_lbs_data.info[i+1].mcc = cell_info.nbr_cell_info[i].gci.mcc;
		g_lbs_data.info[i+1].mnc = cell_info.nbr_cell_info[i].gci.mnc;
		g_lbs_data.info[i+1].lac = cell_info.nbr_cell_info[i].gci.lac;
		g_lbs_data.info[i+1].ci = cell_info.nbr_cell_info[i].gci.ci;
	}
    
#if 1
    for(i = 0; i<g_lbs_data.count; i++)
    {
        kal_prompt_trace(MOD_WAP, "-lbs-%d(mcc=%d, mnc=%d, lac=%d, ci=%d, rxlev=%d, bsic=%d, arfcn=%d)-%s-", __LINE__, 
            g_lbs_data.info[i].mcc, g_lbs_data.info[i].mnc, g_lbs_data.info[i].lac, g_lbs_data.info[i].ci, 
            g_lbs_data.info[i].rxlev, g_lbs_data.info[i].bsic, g_lbs_data.info[i].arfcn, 
            __FILE__);
    }
#endif
}

/*LBS 信息更新*/
void mmi_lbs_update(void)
{
    SetProtocolEventHandler(mmi_lbs_cell_info, MSG_ID_L4C_NBR_CELL_INFO_REG_CNF);
    SetProtocolEventHandler(mmi_lbs_cell_info, MSG_ID_L4C_NBR_CELL_INFO_IND);
    mmi_frm_send_ilm(MOD_L4C, MSG_ID_L4C_NBR_CELL_INFO_REG_REQ, NULL, NULL);    
}

/*获取 LBS 信息*/
lbs_data_struct *mmi_lbs_get_data(void)
{
    return &g_lbs_data;
}

#endif/*__MMI_MYSTUDY_LOCATION__*/

