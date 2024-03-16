#ifndef _USR_CONFIG_H_
#define _USR_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef struct sUsrConfig{
	
	//Motor
	float motor_phase_resistance;    	// ��������[R]        (0~10)
  float motor_phase_inductance;    	// �������[H]        (0~10)
	int32_t motor_pole_pairs;        	// ���������[PP]       (2~30)
	//Control
	int		current_ctrl_bw; 						// [rad/s] Current loop bandwidth 100~2000
	
	//Encoder
	float calib_current;              // ���У׼����[A]      (0~45)
	float calib_voltage;              // ���У׼��ѹ[V]      (0~50)
	int32_t encoder_dir;							// ���ת��	[AuTo]
} tUsrConfig;

void USR_CONFIG_set_default_config(void);

extern tUsrConfig UsrConfig;

#ifdef __cplusplus
}
#endif
#endif
