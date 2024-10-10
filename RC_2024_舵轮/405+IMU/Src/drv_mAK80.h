#ifndef  __DRV_MAK80_H
#define  __DRV_MAK80_H
#ifdef  __cplusplus
extern "C"{
#endif
#include "system.h"

typedef enum {
	CAN_PACKET_SET_DUTY = 0, 			//ռ�ձ�ģʽ
	CAN_PACKET_SET_CURRENT, 			//������ģʽ
	CAN_PACKET_SET_CURRENT_BRAKE, //����ɲ��ģʽ
	CAN_PACKET_SET_RPM, 					//ת��ģʽ
	CAN_PACKET_SET_POS, 					//λ��ģʽ
	CAN_PACKET_SET_ORIGIN_HERE, 	//����ԭ��ģʽ
	CAN_PACKET_SET_POS_SPD, 			//λ���ٶȻ�ģʽ
} CAN_PACKET_ID;



#ifdef  __cplusplus
}
#endif
#endif 

