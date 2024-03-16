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
 * @brief FOC控制器重置
 */
void CONTROLLER_reset(void)
{
  
}

/**
 * @brief FOC控制器执行
 */
void CONTROLLER_loop(void)
{
    float vel_des;
    const float pos_meas = Encoder.pos;	//传感器实时测量位置
    const float vel_meas = Encoder.vel;	//传感器实时测量转速
    const float phase_meas = Encoder.phase;
    const float phase_vel_meas = Encoder.phase_vel;
    
    if(FSM_get_state() == RUN)
		{
			switch (UsrConfig.control_mode)
			{
				/*转速爬升模式*/
				case CONTROL_MODE_VELOCITY_RAMP:
				{
					//更新设定转速、力矩
					float max_step_size = ABS(CURRENT_MEASURE_PERIOD * UsrConfig.velocity_ramp_rate);		//最大爬升步距=电流采样周期*转速爬升速率
					float full_step = Controller.input_velocity - Controller.vel_setpoint;							//转速步距=当前输入转速(实时转速) - 设定转速(期望转速)
					float step = CLAMP(full_step, -max_step_size, max_step_size);												//实际步距=转速步距限幅
					Controller.vel_setpoint += step;		//设定转速更新：+转速步距
					Controller.torque_setpoint = (step / CURRENT_MEASURE_PERIOD) * UsrConfig.inertia;		//设定力矩更新: (转速步距/电流采样周期)*转动惯量
					
					//转速达标检测
					if(StatuswordNew.status.target_reached == 0){
							if(ABS(Controller.input_velocity - vel_meas) < UsrConfig.target_velcity_window){
								//若目标转速与设定转速差值小于给定窗口，则转速达标
								StatuswordNew.status.target_reached = 1;
							}
					}
					
				 } 
				 break;   
				 default:
					 break;
			}
        
			/*=======================位置闭环控制=======================*/
			vel_des = Controller.vel_setpoint;		//更新设定转速
			if (UsrConfig.control_mode >= CONTROL_MODE_POSITION_FILTER){
				//若位置控制模式，则进行位置闭环控制
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

    //期望转速限幅
    vel_des = CLAMP(vel_des, -UsrConfig.velocity_limit, +UsrConfig.velocity_limit);

    /*=======================力矩闭环控制=======================*/
    float torque = Controller.torque_setpoint;		//更新设定力矩
    float v_err = 0.0f;
    if(UsrConfig.control_mode >= CONTROL_MODE_VELOCITY_RAMP)
		{
			 //非力矩控制模式下，则进行转速反馈误差计算更新
       v_err = vel_des - vel_meas;
       torque += UsrConfig.vel_gain * v_err;	//根据转速误差进行力矩补偿
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

    //力矩限幅
    bool limited = false;
    float max_torque = UsrConfig.current_limit * UsrConfig.torque_constant;	//最大力矩=力矩限幅*电机扭矩常数
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

    //转速积分更新
    if (UsrConfig.control_mode < CONTROL_MODE_VELOCITY_RAMP) 
		{
			//力矩控制模式下重置积分
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

