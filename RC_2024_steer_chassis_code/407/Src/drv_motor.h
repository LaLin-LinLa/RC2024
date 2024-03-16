#ifndef  __DRV_MOTOR_H
#define  __DRV_MOTOR_H
#ifdef  __cplusplus
extern "C"{
#endif
#include "system.h"

/*==========电机测量原始信息==========*/
typedef struct
{
  vu16 	ecd;				//编码器值
	vu16 	last_ecd;		//上一次编码器值
	u16		ecd_offset;	//初始校准编码器值
	vs32	round_cnt;	//圈数
	__IO int 	total_ecd;	//总机械角度
	vs16 	speed_rpm;	//转速
	vs16	torque;			//力矩
	uint8_t	calib;		//校准标志量
} motor_measure_t;

/*==========转向电机内部信息==========*/
typedef struct
{
	const motor_measure_t *motor_measure;
	float	Ramp_Inc;				//斜坡系数
	int16_t set_current;	//输出电流[-10000~+10000]
} Steer_Motor_t;
/*==========驱动电机内部信息==========*/
typedef struct
{
	const motor_measure_t *motor_measure;
	float	Ramp_Inc;				//斜坡系数
	int16_t set_current;	//输出电流[-16384~+16384]
} Driver_Motor_t;

/*==========电机标准帧ID==========*/
typedef enum
{
	Null_id											= 0x000,
	
	CAN_C610_ALL_ID        			= 0x200,
	C610_1               				= 0x201,
	C610_2							 				= 0x202,
	C610_3               				= 0x203,
	C610_4							 				= 0x204,
	
	CAN_C620_ALL_ID        			= 0x200,
	CAN_C620_ALL_ID2        		= 0x1FF,
	C620_1               				= 0x201,
	C620_2							 				= 0x202,
	C620_3               				= 0x203,
	C620_4							 				= 0x204,
	
	
} can_msg_id_e;

void Steer_motor_calib(uint8_t i, uint8_t num);
void get_motor_measure(motor_measure_t* motor, uint8_t* Data);
extern const motor_measure_t *get_Driver_Measure_Point(uint8_t i);
extern const motor_measure_t *get_Steer_Measure_Point(uint8_t i);
extern void Steer_motor_DatePrcess_IT(uint32_t ID, uint8_t *Data);
extern void Driver_motor_DatePrcess_IT(uint32_t ID, uint8_t *Data);
extern void Steer_motor_DatePrcess(void);
extern void Driver_motor_DatePrcess(void);
void CAN_CMD_DRIVER(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);
void CAN_CMD_STEER(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

#ifdef  __cplusplus
}
#endif
#endif 

