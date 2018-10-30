
#ifndef __LBS_H__
#define __LBS_H__

#include "mmi_features.h"

#if defined(__MMI_MYSTUDY_LOCATION__)
#include "MMIDataType.h"

/**********************************************************************
* ��������
**********************************************************************/

#define LBS_SAL_CELL_NBR_MAX	6

typedef struct{
    U16 arfcn;           /*ARFCN ( ��������Ƶ����� Absolute Radio Frequency Channel Number - ARFCN)*/
    U8  bsic;            /*BSIC ( ��վʶ���� Base Station Identity Code )*/
    U8  rxlev;           /*Received signal level �ź�ǿ�ȵȼ�*/
    U16 mcc;             /*MCC �ƶ�������*/
    U16 mnc;             /*MNC �ƶ�������*/
    U16 lac;             /*LAC λ������ location area code */
    U16 ci;              /*CI  С��ʶ���� Cell ID*/
}lbs_cell_info_struct;

typedef struct{
    U16 count;
    lbs_cell_info_struct info[LBS_SAL_CELL_NBR_MAX];
}lbs_data_struct;

/**********************************************************************
* ��������
**********************************************************************/

/*��ȡ LBS ��Ϣ*/
extern lbs_data_struct *mmi_lbs_get_data(void);

#endif

#endif/*__LBS_H__*/

