
#ifndef __GPS_H__
#define __GPS_H__

#include "mmi_features.h"


#include "MMIDataType.h"


//��������
typedef struct
{
    FLOAT    longitude;     //����
    FLOAT    latitude;      //γ��
    S8       north_south;   //��-��
    S8       east_west;     //��-��
}stu_gps_data;

typedef struct
{
    S32 port;       //GPS �����˿�
    S32 port_handle;//-1:GPS �ر�״̬; ���� GPS ���ڴ�״̬
}stu_gps_handle;


extern stu_gps_data *mmi_gps_get_data(void);
extern void mmi_gps_open(void);
extern void mmi_gps_close(void);


#endif/*__GPS_H__*/

