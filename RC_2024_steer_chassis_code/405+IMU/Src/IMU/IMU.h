#ifndef _IMU_H_
#define _IMU_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "system.h"

#define _USE_MPU9250								1

#if _USE_MPU9250
#include "drv_mpu9250.h"
#define	IMU_Get_Gyro	MPU9250_Get_Gyro_transform
#define IMU_Get_Accel	MPU9250_Get_Accel_transform
#endif

#define IMU_Init_Delay_ms	delay_ms_nos

/*以下宏定义无需修改*/
#ifndef PI
#define PI                          3.14159265358979f
#endif
#define DEG2RAD                     (PI / 180.0f)
#define RAD2DEG                     (180.0f / PI)
#define twoKpDef (2.0f * 1.0f)   	
#define twoKiDef (2.0f * 0.001f)  

/*以下宏定义视情况修改*/
#define FLASH_ADDR									0x08070000
#define ACCEL_GRAVITY               9.80665f
#define GYROX_DEADLINE              0.002
#define GYROY_DEADLINE              0.002
#define GYROZ_DEADLINE              0.002
#define IMU_CALC_INIT_TIME          1000
#define sampleFreq 500.0f        	// 采样频率Hz
#define IMU_ENABLE_ACCEL_CALI_DATA  1    //是否进行加速度校准


typedef struct{
	
	float	temp;
	
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float accel_x;
	float accel_y;
	float accel_z;
	
	float	gyro_x_offset;
	float	gyro_y_offset;
	float	gyro_z_offset;
	
	float pitch;
	float roll;
	float yaw;
	float	yaw_total;
	float yaw_last;
	float yaw_new;
	float yaw_count;

} IMU_Data_t;


void IMU_Init(IMU_Data_t *imu_data);
void IMU_Loop(IMU_Data_t *imu_data);

#ifdef __cplusplus
}
#endif
#endif

