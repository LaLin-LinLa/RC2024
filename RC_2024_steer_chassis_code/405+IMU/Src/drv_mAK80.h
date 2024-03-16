#ifndef  __DRV_MAK80_H
#define  __DRV_MAK80_H
#ifdef  __cplusplus
extern "C"{
#endif
#include "system.h"

typedef enum {
	CAN_PACKET_SET_DUTY = 0, 			//占空比模式
	CAN_PACKET_SET_CURRENT, 			//电流环模式
	CAN_PACKET_SET_CURRENT_BRAKE, //电流刹车模式
	CAN_PACKET_SET_RPM, 					//转速模式
	CAN_PACKET_SET_POS, 					//位置模式
	CAN_PACKET_SET_ORIGIN_HERE, 	//设置原点模式
	CAN_PACKET_SET_POS_SPD, 			//位置速度环模式
} CAN_PACKET_ID;



#ifdef  __cplusplus
}
#endif
#endif 

