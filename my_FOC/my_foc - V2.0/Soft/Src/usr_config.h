#ifndef _USR_CONFIG_H_
#define _USR_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef struct sUsrConfig{
	
	//Motor
	float motor_phase_resistance;    	// 电机相电阻[R]        (0~10)
  float motor_phase_inductance;    	// 电机相电感[H]        (0~10)
	int32_t motor_pole_pairs;        	// 电机极对数[PP]       (2~30)
	//Control
	int		current_ctrl_bw; 						// [rad/s] Current loop bandwidth 100~2000
	
	//Encoder
	float calib_current;              // 电机校准电流[A]      (0~45)
	float calib_voltage;              // 电机校准电压[V]      (0~50)
	int32_t encoder_dir;							// 电机转向	[AuTo]
} tUsrConfig;

void USR_CONFIG_set_default_config(void);

extern tUsrConfig UsrConfig;

#ifdef __cplusplus
}
#endif
#endif
