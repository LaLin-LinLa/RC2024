#ifndef __MOD_CHASSIS_H
#define __MOD_CHASSIS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "drv_motor.h"
#include "drv_dr16.h"
#include "my_pid.h"

#define	STEER_PID_POLARITY	1

/*==========底盘控制数据结构体==========*/
typedef struct
{
	const DR16_ctrl_t *chassis_rc_ctrl;		//dr16遥控器数据指针
	Driver_Motor_t	drive_motor[4];				//驱动轮电机信息
	Steer_Motor_t		steer_motor[4];				//转向轮电机信息
	myPidTypeDef_t	drive_speed_pid[4];		//驱动轮速度环pid
	myPidTypeDef_t	steer_speed_pid[4];		//转向轮速度环pid
	myPidTypeDef_t	steer_angle_pid[4];		//转向轮角度环pid
	
	float	Vx, Vy, Vz;						//底盘前进速度Vy, 平移速度Vx, 自旋速度Vz
	int   driver_dir[4];				//驱动轮方向
	float set_driver_speed[4];
	float	set_steer_ecd[4];
	float	set_steer_angle[4];
} Chassis_Control_t;

void Chassis_Init(Chassis_Control_t *chassis_init);
void Chassis_Loop(Chassis_Control_t *chassis_loop);

#ifdef __cplusplus
}
#endif
#endif
