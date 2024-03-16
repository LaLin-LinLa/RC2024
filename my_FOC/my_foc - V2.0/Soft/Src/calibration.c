#include "calibration.h"
#include "pwm_cur.h"
#include "usr_config.h"
#include "fsm_task.h"
#include "encoder.h"
#include "foc.h"

#define SAMPLES_PER_PPAIR       128U

typedef enum eCalibStep{
    CS_NULL = 0,
    //相电阻校准
    CS_MOTOR_R_START,
    CS_MOTOR_R_LOOP,
    CS_MOTOR_R_END,
    //相电感校准
    CS_MOTOR_L_START,
    CS_MOTOR_L_LOOP,
    CS_MOTOR_L_END,
    //极对数校准
    CS_DIR_PP_START,
    CS_DIR_PP_LOOP,
    CS_DIR_PP_END,
    
    CS_ENCODER_START,
    CS_ENCODER_CW_LOOP,
    CS_ENCODER_CCW_LOOP,
    CS_ENCODER_END,
    
    CS_REPORT_OFFSET_LUT,
}tCalibStep;

static tCalibStep mCalibStep = CS_MOTOR_R_START;

/**
  * @brief  电机校准任务
  * @retval 无
  */
void CALIBRATION_loop(void)
{
	static uint32_t loop_count;
	static float voltages[2];
	static float Ialphas[2];
	
	//R
	static const float kI = 2.0f;
	static const uint32_t num_R_cycles = CURRENT_MEASURE_HZ * 2;
	//L
	static const uint32_t num_L_cycles = CURRENT_MEASURE_HZ / 2;
	//Encoder
	static float phase_set;
	static float start_count;
	static int16_t sample_count;
  static float next_sample_time;
	
	static const float calib_phase_vel = M_PI;		//电角速度
	float time = (float)loop_count * CURRENT_MEASURE_PERIOD;		//校准程序总计时
	const float voltage = UsrConfig.calib_current * UsrConfig.motor_phase_resistance * 3.0f / 2.0f;
	
	switch(mCalibStep){
		 case CS_NULL:
        break;
		 
		 
		 //相电阻校准
		 case CS_MOTOR_R_START:
		 {
			 loop_count = 0;
			 voltages[0] = 0.0f;
			 mCalibStep = CS_MOTOR_R_LOOP;
		 }	
		 break;
		 case CS_MOTOR_R_LOOP:
		 {
				voltages[0] += kI * CURRENT_MEASURE_PERIOD * (UsrConfig.calib_current - foc.i_a);
				FOC_voltage(voltages[0], 0, 0);
				if(loop_count >= num_R_cycles){
						PWMC_TurnOnLowSides();
						mCalibStep = CS_MOTOR_R_END;
				}
		 }
		 break;
		 case CS_MOTOR_R_END:
		 {
				UsrConfig.motor_phase_resistance = (voltages[0] / UsrConfig.calib_current) * 2.0f / 3.0f;
				mCalibStep = CS_MOTOR_L_START;
		 }
		 break;
 		 //相电感校准
		case CS_MOTOR_L_START:
		{
			loop_count = 0;
			Ialphas[0] = 0.0f;
			Ialphas[1] = 0.0f;
			voltages[0] = -UsrConfig.calib_voltage;
			voltages[1] = +UsrConfig.calib_voltage;
			FOC_voltage(voltages[0], 0, 0);
			mCalibStep = CS_MOTOR_L_LOOP;
	  }
		break;
		case CS_MOTOR_L_LOOP:
		{
				int i = loop_count & 1;
				Ialphas[i] += foc.i_a;
				// Test voltage along phase A
				FOC_voltage(voltages[i], 0, 0);
				
				if(loop_count >= (num_L_cycles<<1)){
						PWMC_TurnOnLowSides();
						mCalibStep = CS_MOTOR_L_END;
				}
		}
		break;
		case CS_MOTOR_L_END:
		{
				float dI_by_dt = (Ialphas[1] - Ialphas[0]) / (float)(CURRENT_MEASURE_PERIOD * num_L_cycles);
				float L = UsrConfig.calib_voltage / dI_by_dt;
				UsrConfig.motor_phase_inductance = L * 2.0f / 3.0f;
				FOC_update_current_ctrl_gain(UsrConfig.current_ctrl_bw);
				
				phase_set = 0;
				loop_count = 0;
				mCalibStep = CS_DIR_PP_START;
		}
		break;
		case CS_DIR_PP_START:
		{
			FOC_voltage((voltage * time / 2.0f), 0, phase_set);
			if (time >= 2.0f){
					start_count = (float)Encoder.shadow_count;
					mCalibStep = CS_DIR_PP_LOOP;
					break;
			}
	  }
		break;      
		case CS_DIR_PP_LOOP:
		{
				phase_set += calib_phase_vel * CURRENT_MEASURE_PERIOD;
				FOC_voltage(voltage, 0, phase_set);
				if(phase_set >= 2.0f * M_2PI){
						mCalibStep = CS_DIR_PP_END;
				}
	  }
		break;
            
		case CS_DIR_PP_END:
		{
				int32_t diff = Encoder.shadow_count - start_count;
				
				// Check direction
				if(diff > 0){
						UsrConfig.encoder_dir = +1;
				}else{
						UsrConfig.encoder_dir = -1;
				}
				
				// Motor pole pairs
				UsrConfig.motor_pole_pairs = round(2.0f / ABS(diff/(float)Encoder.Encoder_cpr));	

				mCalibStep = CS_ENCODER_START;
		}
		break;
		
		//编码器校准
//		case CS_ENCODER_START:
//		{
//			phase_set = 0;
//			loop_count = 0;
//			sample_count = 0;
//			next_sample_time = 0;
//			mCalibStep = CS_ENCODER_CW_LOOP;
//		}
//		break;
	//顺时针
//		case CS_ENCODER_CW_LOOP:
//		{
//            if(sample_count < (UsrConfig.motor_pole_pairs * SAMPLES_PER_PPAIR)){
//                if(time > next_sample_time){
//                    next_sample_time += M_2PI / ((float)SAMPLES_PER_PPAIR * calib_phase_vel);
//                    
//                    int count_ref = ((float)(phase_set * Encoder.Encoder_cpr)) / (M_2PI * (float)UsrConfig.motor_pole_pairs);
//									
//                    int error = Encoder.ecd - count_ref;
//                    error += Encoder.Encoder_cpr * (error<0);
//                    p_error_arr[sample_count] = error;
//                    
//                    sample_count ++;
//                }
//                
//                phase_set += calib_phase_vel * CURRENT_MEASURE_PERIOD;
//            }else{
//                phase_set -= calib_phase_vel * CURRENT_MEASURE_PERIOD;
//                loop_count = 0;
//                sample_count --;
//                next_sample_time = 0;
//                mCalibStep = CS_ENCODER_CCW_LOOP;
//                break;
//            }
//            FOC_voltage(voltage, 0, phase_set);
//		} 
//	  break;
		
		
		default:
       break;
		 
	}
	loop_count++;
	
}
