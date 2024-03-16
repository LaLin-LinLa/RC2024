#include "board_config.h"
#include "delay_timer.h"
#include "pwm_cur.h"
#include "foc.h"
#include "encoder.h"
#include "fsm_task.h"
#include "usr_config.h"


int main(void)
{
	__disable_irq();
	USR_CONFIG_set_default_config();
	FOC_Init();
	delay_timer_Init();
	BSP_Init();
	PWMC_Init();
	Encoder_Init();
	
	__enable_irq();
	
	// wait voltage stable
	for(uint8_t i=0, j=0; i<250; i++){
		if(foc.v_bus_filt > 20){
				if(++j > 20){
						break;
				}
		}
		delay_ms(2);
	}
	while(PWMC_CurrentReadingPolarization()){}
	
	fsm.FSM_state = IDLE;
		
	PWMC_SwitchOnPWM();
	
	//将转子转动至a轴
	set_a_duty(HALF_PWM_PERIOD_CYCLES);
	set_b_duty(0);
	set_c_duty(0);
	delay_ms(500);
	Encoder.ecd_offset = Encoder.ecd;		
	Encoder.ecd = 0;
	PWMC_TurnOnLowSides();	
	
	while(1)
	{
		//模拟电机角度变换
//		theta_test += 0.01745329f;
//		if(theta_test > 16384.0f)
//		{
//			theta_test -= 16384.0f;
//		}
//		
//		// Inverse park transform
//		float mod_alpha, mod_beta;
//		inverse_park(0, iq_value, theta_test, &mod_alpha, &mod_beta);
//		
//		// SVM
//		if(0 == svm(mod_alpha, mod_beta, &foc.dtc_a, &foc.dtc_b, &foc.dtc_c)){
//				set_a_duty((uint16_t)(foc.dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
//				set_b_duty((uint16_t)(foc.dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
//				set_c_duty((uint16_t)(foc.dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
//		}
//		
//		delay_us_nos(50);
		
		
	}
}

void delay_ms(const uint16_t ms)
{
    volatile uint32_t i = ms * 17200;
    while (i-- > 0) {
        __NOP();
    }
}
