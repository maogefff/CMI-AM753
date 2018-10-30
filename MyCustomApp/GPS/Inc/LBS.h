
#ifndef __LBS_H__
#define __LBS_H__

#include "mmi_features.h"

#if defined(__MMI_MYSTUDY_LOCATION__)
#include "MMIDataType.h"

/**********************************************************************
* 数据类型
**********************************************************************/

#define LBS_SAL_CELL_NBR_MAX	6

typedef struct{
    U16 arfcn;           /*ARFCN ( 绝对无线频道编号 Absolute Radio Frequency Channel Number - ARFCN)*/
    U8  bsic;            /*BSIC ( 基站识别码 Base Station Identity Code )*/
    U8  rxlev;           /*Received signal level 信号强度等级*/
    U16 mcc;             /*MCC 移动国家码*/
    U16 mnc;             /*MNC 移动网络码*/
    U16 lac;             /*LAC 位置区码 location area code */
    U16 ci;              /*CI  小区识别码 Cell ID*/
}lbs_cell_info_struct;

typedef struct{
    U16 count;
    lbs_cell_info_struct info[LBS_SAL_CELL_NBR_MAX];
}lbs_data_struct;

/**********************************************************************
* 函数声明
**********************************************************************/

/*获取 LBS 信息*/
extern lbs_data_struct *mmi_lbs_get_data(void);

#endif

#endif/*__LBS_H__*/

