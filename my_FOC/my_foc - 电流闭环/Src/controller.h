#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef enum {
    CONTROL_MODE_TORQUE_RAMP = 0,   //��������ģʽ
    CONTROL_MODE_VELOCITY_RAMP,     //ת������ģʽ
    CONTROL_MODE_POSITION_FILTER,   //λ���˲�ģʽ
    CONTROL_MODE_POSITION_PROFILE,  //λ������ģʽ
} tControlMode;

typedef struct sController{
    float input_position;
    float input_velocity;
    float input_torque;
    
    float input_position_buffer;
    float input_velocity_buffer;
    float input_torque_buffer;
    
    float pos_setpoint;
    float vel_setpoint;
    float torque_setpoint;

    bool input_updated;
    float input_pos_filter_kp;
    float input_pos_filter_ki;
    float vel_integrator_torque;
} tController;

extern tController Controller;

void CONTROLLER_init(void);
void CONTROLLER_reset(void);
void CONTROLLER_loop(void);
void CONTROLLER_update_input_pos_filter_gain(float bw);

#ifdef __cplusplus
}
#endif
#endif
