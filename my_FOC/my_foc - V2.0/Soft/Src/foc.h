#ifndef _FOC_H_
#define _FOC_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef struct FOC_Control_s {
  float v_bus, v_bus_filt, i_a, i_b, i_c;
	float i_q,i_d;
	float dtc_a, dtc_b, dtc_c;
	float	temp;
	
	float I_Kp;		//电流环Kp
	float I_Ki;		//电流环Ki
	float I_d_integral;		//I_d积分值
	float I_q_integral;		//I_q积分值
	
} FOC_Control_t;

extern FOC_Control_t	foc;

void FOC_Init(void);
void FOC_update_current_ctrl_gain(float bw);
void FOC_voltage(float Vd_set, float Vq_set, float phase);
void FOC_current_open(float id_set, float iq_set, float phase);
void FOC_current(float Id_set, float Iq_set, float phase, float phase_vel);

#ifdef __cplusplus
}
#endif
#endif
