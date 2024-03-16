#include "fsm_task.h"
#include "foc.h"
#include "pwm_cur.h"
#include "encoder.h"
#include "calibration.h"

FSM_control_t	fsm;

float Id_set = 0.0f;
float Iq_set = 0.0f;
float Uq_set = 0.0f;
float Ud_set = 0.0f;
float phase_set = 0.0f;
float phase_vel = 0.0f;

/**
  * @brief  FSM高频任务
  * @retval 无
  */
void FSM_high_fre_task(void)
{
	
	
	LED(1);
	read_temp(&foc.temp);
	Encoder_loop();
	//三相电流电压采样
	foc.v_bus = read_vbus();
	UTILS_LP_FAST(foc.v_bus_filt, foc.v_bus, 0.05f);
	foc.i_b   = read_iphase_b();
	foc.i_c   = read_iphase_c();
	foc.i_a   = -foc.i_b-foc.i_c;
	
	switch(fsm.FSM_state)
  {
  	case BOOT_UP:
  	break;
  	case IDLE:
  	break;
		case CALIBRATION:
		{
			CALIBRATION_loop();
		}
		break;
		case TEST:
		{
			//FOC_current_open(Id_set, Iq_set, Encoder.phase);
			FOC_current(0, Iq_set, Encoder.phase, phase_vel);
			//FOC_voltage(Ud_set, Uq_set, phase_set);
		}
		break;
		case RUN:
		{
			
		}
		break;
		
  	default:
  		break;
  }
	LED(0);
}

/**
  * @brief  FSM低频频任务
  * @retval 无
  */
void FSM_low_fre_task(void)
{

	
}
