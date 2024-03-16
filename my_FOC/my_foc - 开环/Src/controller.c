#include "controller.h"
#include "fsm_task.h"
#include "user_config.h"
#include "pwm_curr.h"
#include "encoder.h"
#include "foc.h"
#include "util.h"

tController Controller;

void CONTROLLER_init(void)
{
    CONTROLLER_update_input_pos_filter_gain(UsrConfig.position_filter_bw);
}

void CONTROLLER_update_input_pos_filter_gain(float bw)
{
    float bandwidth = MIN(bw, 0.25f * PWM_FREQUENCY);
    Controller.input_pos_filter_ki = 2.0f * bandwidth;
    Controller.input_pos_filter_kp = 0.25f * SQ(Controller.input_pos_filter_ki);
}

/**
 * @brief FOC����������
 */
void CONTROLLER_reset(void)
{
  
}

/**
 * @brief FOC������ִ��
 */
void CONTROLLER_loop(void)
{
    float vel_des;
    const float pos_meas = Encoder.pos;	//������ʵʱ����λ��
    const float vel_meas = Encoder.vel;	//������ʵʱ����ת��
    const float phase_meas = Encoder.phase;
    const float phase_vel_meas = Encoder.phase_vel;
    
    if(FSM_get_state() == RUN)
		{
			switch (UsrConfig.control_mode)
			{
				/*ת������ģʽ*/
				case CONTROL_MODE_VELOCITY_RAMP:
				{
					//�����趨ת�١�����
					float max_step_size = ABS(CURRENT_MEASURE_PERIOD * UsrConfig.velocity_ramp_rate);		//�����������=������������*ת����������
					float full_step = Controller.input_velocity - Controller.vel_setpoint;							//ת�ٲ���=��ǰ����ת��(ʵʱת��) - �趨ת��(����ת��)
					float step = CLAMP(full_step, -max_step_size, max_step_size);												//ʵ�ʲ���=ת�ٲ����޷�
					Controller.vel_setpoint += step;		//�趨ת�ٸ��£�+ת�ٲ���
					Controller.torque_setpoint = (step / CURRENT_MEASURE_PERIOD) * UsrConfig.inertia;		//�趨���ظ���: (ת�ٲ���/������������)*ת������
					
					//ת�ٴ����
					if(StatuswordNew.status.target_reached == 0){
							if(ABS(Controller.input_velocity - vel_meas) < UsrConfig.target_velcity_window){
								//��Ŀ��ת�����趨ת�ٲ�ֵС�ڸ������ڣ���ת�ٴ��
								StatuswordNew.status.target_reached = 1;
							}
					}
					
				 } 
				 break;   
				 default:
					 break;
			}
        
			/*=======================λ�ñջ�����=======================*/
			vel_des = Controller.vel_setpoint;		//�����趨ת��
			if (UsrConfig.control_mode >= CONTROL_MODE_POSITION_FILTER){
				//��λ�ÿ���ģʽ�������λ�ñջ�����
					float pos_err = Controller.pos_setpoint - pos_meas;
					vel_des += UsrConfig.pos_gain * pos_err;
			}
			
		}
		else
		{
        // Anticogging calib
        // float pos_err = Controller.input_position - (Encoder.count_in_cpr / ENCODER_CPR_F);
        // if(pos_err > +0.5f) pos_err -= 1.0f;
        // if(pos_err < -0.5f) pos_err += 1.0f;
        // vel_des = UsrConfig.pos_gain * pos_err;
    }

    //����ת���޷�
    vel_des = CLAMP(vel_des, -UsrConfig.velocity_limit, +UsrConfig.velocity_limit);

    /*=======================���رջ�����=======================*/
    float torque = Controller.torque_setpoint;		//�����趨����
    float v_err = 0.0f;
    if(UsrConfig.control_mode >= CONTROL_MODE_VELOCITY_RAMP)
		{
			 //�����ؿ���ģʽ�£������ת�ٷ������������
       v_err = vel_des - vel_meas;
       torque += UsrConfig.vel_gain * v_err;	//����ת�����������ز���
        // Velocity integral action before limiting
       torque += Controller.vel_integrator_torque;
    }
    if (UsrConfig.control_mode < CONTROL_MODE_VELOCITY_RAMP) 
		{
			float Tmax = (+UsrConfig.velocity_limit - vel_meas) * UsrConfig.vel_gain;
			float Tmin = (-UsrConfig.velocity_limit - vel_meas) * UsrConfig.vel_gain;
			torque = CLAMP(torque, Tmin, Tmax);
    }
    // Anticogging
    // if(UsrConfig.anticogging_enable && AnticoggingValid){
    //     int16_t index = nearbyintf(COGGING_MAP_NUM * Encoder.count_in_cpr / ENCODER_CPR_F);
    //     if(index >= COGGING_MAP_NUM){
    //         index = 0;
    //     }
    //     torque += pCoggingMap->map[index] / 5000.0f;
    // }

    //�����޷�
    bool limited = false;
    float max_torque = UsrConfig.current_limit * UsrConfig.torque_constant;	//�������=�����޷�*���Ť�س���
    if (torque > +max_torque) {
        limited = true;
        torque = +max_torque;
    }
    if (torque < -max_torque) {
        limited = true;
        torque = -max_torque;
    }
    if(limited){
        StatuswordNew.status.current_limit_active = 1;
    }else{
        if(ABS(torque) < 0.9f * max_torque){
            StatuswordNew.status.current_limit_active = 0;
        }
    }

    //ת�ٻ��ָ���
    if (UsrConfig.control_mode < CONTROL_MODE_VELOCITY_RAMP) 
		{
			//���ؿ���ģʽ�����û���
			Controller.vel_integrator_torque = 0.0f;
    }
		else
		{
			if (limited) {
					Controller.vel_integrator_torque *= 0.99f;
			} else {
					Controller.vel_integrator_torque += (UsrConfig.vel_integrator_gain * CURRENT_MEASURE_PERIOD) * v_err;
			}
    }
    
    float iq_set = torque / UsrConfig.torque_constant;
    FOC_current(0, iq_set, phase_meas, phase_vel_meas);
}

