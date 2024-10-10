#include "IMU.h"
#include "math.h"
#include "Smooth_Filter.h"
#include "kalman.h"
#include "pid.h"

PidTypeDef     IMU_Temp_PID;
SmoothFilter_t IMU_Temp_Filter;
extKalman_t 	 kalman;

//陀螺仪温度控制PID
#define IMU_Temp_PID_Kp     400.0
#define IMU_Temp_PID_Ki     0.5
#define IMU_Temp_PID_Kd     0.5
#define IMU_Temp_Set        70.0    //陀螺仪预设温度

static volatile float twoKp = twoKpDef;                                           // 2 * proportional gain (Kp)
static volatile float twoKi = twoKiDef;  
static volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
static volatile float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f; // integral error terms scaled by Ki

static void IMU_Init_Calibration(IMU_Data_t *imu_data);
static void IMU_InData_Solve(IMU_Data_t *imu_data);
static void mahony_ahrs_updateIMU(IMU_Data_t *imu_data);

/**
	* @brief  IMU初始化
	* @retval None
	*/
void IMU_Init(IMU_Data_t *imu_data)
{
	#if _USE_MPU9250
	while(MPU9250_Init());
	#endif
	float pid[3] = {IMU_Temp_PID_Kp, IMU_Temp_PID_Ki, IMU_Temp_PID_Kd};
	IMU_Init_Calibration(imu_data);
	SmoothFilter_Init(&IMU_Temp_Filter, 8);
	PID_Init(&IMU_Temp_PID, PID_POSITION, pid, 1000, 0);
	KalmanCreate(&kalman,20,200);
}

/**
	* @brief  IMU循环
	* @retval None
	*/
void IMU_Loop(IMU_Data_t *imu_data)
{
	//陀螺仪PID恒温调节
	MPU9250_Get_Temp(&imu_data->temp);
	imu_data->temp = SmoothFilter_Calc(&IMU_Temp_Filter, imu_data->temp);
	MPU9250_Set_Heat_PWM((int)PID_Calc(&IMU_Temp_PID, imu_data->temp, IMU_Temp_Set));
	IMU_InData_Solve(imu_data);
	KalmanFilter(&kalman, imu_data->accel_x);
	KalmanFilter(&kalman, imu_data->accel_y);
	KalmanFilter(&kalman, imu_data->accel_z);
	KalmanFilter(&kalman, imu_data->gyro_x);
	KalmanFilter(&kalman, imu_data->gyro_y);
	KalmanFilter(&kalman, imu_data->gyro_z);
	
	mahony_ahrs_updateIMU(imu_data);
	
	
	//绝对
	imu_data->yaw = imu_data->yaw_new;
	//相对
	if((imu_data->yaw_new - imu_data->yaw_last) >= 330) {
	imu_data->yaw_count--;
	}
	else if((imu_data->yaw_new - imu_data->yaw_last) <= -330) {
		imu_data->yaw_count++;
	}
	imu_data->yaw_total = imu_data->yaw_count*360 + imu_data->yaw_new;
	
}


/**
  * @brief  快速开方
  * @param  原始数据
  * @retval 开方后的倒数
  */
static float invSqrt(float x) 
{
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;

  i = 0x5f3759df - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));

  return y;
}

/**
  * @brief	获取陀螺仪原始数据
  * @param  IMU数据结构体
  * @retval void
  */
static void IMU_Get_Raw_Data(IMU_Data_t *imu_data) 
{
	IMU_Get_Gyro(&imu_data->gyro_x, &imu_data->gyro_y, &imu_data->gyro_z);
	IMU_Get_Accel(&imu_data->accel_x, &imu_data->accel_y, &imu_data->accel_z);
	
}

/**
  * @brief  加速度校准初始化
  * @param  IMU数据结构体
  * @retval void
  */
static void IMU_Init_Calibration(IMU_Data_t *imu_data) {
	uint16_t i=0, cnt=0;
	for(i = 0;i < 2048 ;i++)
	{
		IMU_Get_Raw_Data(imu_data);
		if(i>300)
		{
      imu_data->gyro_x_offset += imu_data->gyro_x;
			imu_data->gyro_y_offset += imu_data->gyro_y;
			imu_data->gyro_z_offset += imu_data->gyro_z;
		  cnt++;
			IMU_Init_Delay_ms(2);
		}
	}
	imu_data->gyro_x_offset /= (float)cnt;
  imu_data->gyro_y_offset /= (float)cnt;
	imu_data->gyro_z_offset /= (float)cnt;
}


/**
  * @brief  陀螺仪数据处理
  * @param  IMU数据结构体
  * @retval void
  */
static void IMU_InData_Solve(IMU_Data_t *imu_data)
{
	char i;
	float	gx_sum, gy_sum, gz_sum;
	float	ax_sum, ay_sum, az_sum;
	for(i = 0; i<50; i++)
	{
		IMU_Get_Raw_Data(imu_data);
		
		gx_sum += (imu_data->gyro_x-imu_data->gyro_x_offset);
		gy_sum += (imu_data->gyro_y-imu_data->gyro_y_offset);
		gz_sum += (imu_data->gyro_z-imu_data->gyro_z_offset);
		ax_sum += imu_data->accel_x;
		ay_sum += imu_data->accel_y;
		az_sum += imu_data->accel_z;
	}
	
	imu_data->gyro_x = gx_sum/50 *DEG2RAD;
	imu_data->gyro_y = gy_sum/50 *DEG2RAD;
	imu_data->gyro_z = gz_sum/50 *DEG2RAD;
	
	imu_data->accel_x = ax_sum/50 *ACCEL_GRAVITY;
	imu_data->accel_y = ay_sum/50 *ACCEL_GRAVITY;
	imu_data->accel_z = az_sum/50 *ACCEL_GRAVITY;	
	
}

/**
  * @brief  六轴姿态解(不含磁强计)
  * @param  IMU数据结构体
  */
static void mahony_ahrs_updateIMU(IMU_Data_t *imu_data) 
{
	float recipNorm;
  float halfvx, halfvy, halfvz;
  float halfex, halfey, halfez;
	float qa, qb, qc;
	
	// 仅当加速度计测量有效时才计算反馈(避免加速度计归一化中的NaN)
  if (!((imu_data->accel_x == 0.0f) && (imu_data->accel_y == 0.0f) && (imu_data->accel_z == 0.0f)))
  {
		//归一化加速度计测量
		recipNorm = invSqrt(imu_data->accel_x*imu_data->accel_x + imu_data->accel_y*imu_data->accel_y + imu_data->accel_z*imu_data->accel_z);
		imu_data->accel_x *= recipNorm;
		imu_data->accel_y *= recipNorm;
		imu_data->accel_z *= recipNorm;
		
		// 估计重力的方向
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;
			
		// 误差是估计的重力方向和测量的重力方向的叉积之和
		halfex = (imu_data->accel_y * halfvz - imu_data->accel_z * halfvy);
		halfey = (imu_data->accel_z * halfvx - imu_data->accel_x * halfvz);
		halfez = (imu_data->accel_x * halfvy - imu_data->accel_y * halfvx);

		//计算和应用积分反馈，如果启用
		if (twoKi > 0.0f)
		{
			integralFBx += twoKi * halfex * (1.0f / sampleFreq); 
			integralFBy += twoKi * halfey * (1.0f / sampleFreq);
			integralFBz += twoKi * halfez * (1.0f / sampleFreq);
			imu_data->gyro_x += integralFBx; 
			imu_data->gyro_y += integralFBy;
			imu_data->gyro_z += integralFBz;
		}
		else
		{
			integralFBx = 0.0f; //防止整体清盘
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}
		//应用比例反馈
		imu_data->gyro_x += twoKp * halfex;
		imu_data->gyro_y += twoKp * halfey;
		imu_data->gyro_z += twoKp * halfez;
	}

	// 积分四元数的变化率
  imu_data->gyro_x *= (0.5f * (1.0f / sampleFreq)); // pre-multiply common factors
  imu_data->gyro_y *= (0.5f * (1.0f / sampleFreq));
  imu_data->gyro_z *= (0.5f * (1.0f / sampleFreq));
  qa = q0;
  qb = q1;
  qc = q2;
  q0 += (-qb * imu_data->gyro_x - qc * imu_data->gyro_y - q3 * imu_data->gyro_z);
  q1 += (qa * imu_data->gyro_x + qc * imu_data->gyro_z - q3 * imu_data->gyro_y);
  q2 += (qa * imu_data->gyro_y - qb * imu_data->gyro_z + q3 * imu_data->gyro_x);
  q3 += (qa * imu_data->gyro_z + qb * imu_data->gyro_y - qc * imu_data->gyro_x);
	
	// 正常化四元数
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
  imu_data->roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f; // roll     -pi----pi
  imu_data->pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f;                                // pitch    -pi/2----pi/2
	imu_data->yaw_last = imu_data->yaw_new;
  imu_data->yaw_new = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 57.3f;  // yaw      -pi----pi
	
}




