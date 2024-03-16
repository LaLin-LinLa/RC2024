#include "main.h"
#include "system_board.h"
#include "delay_timer.h"
#include "pwm_curr.h"
#include "FOC.h"
#include "util.h"
#include "encoder.h"
#include "user_config.h"

float iq_value = 0;
float theta_test    = 0;

int main(void)
{
	__disable_irq();
	BSP_Init();
	
	USR_CONFIG_set_default_config();
	
	Encoder_Init();
	delay_timer_Init();
	
	timer_enable(TIMER0);
  timer_repetition_value_config(TIMER0, 1);
  timer_primary_output_config(TIMER0, ENABLE);
  timer_auto_reload_shadow_enable(TIMER0);
	
	PWMC_SwitchOnPWM();
	__enable_irq();
	
	
	while(1)
	{
//		ENCODER_loop();
		//模拟电机角度变换
		theta_test += 0.01745329f;
		if(theta_test > 16384.0f)
		{
			theta_test -= 16384.0f;
		}
		
		// Inverse park transform
		float mod_alpha, mod_beta;
		inverse_park(0, iq_value, theta_test, &mod_alpha, &mod_beta);
		
		// SVM
		if(0 == svm(mod_alpha, mod_beta, &Foc.dtc_a, &Foc.dtc_b, &Foc.dtc_c)){
				set_a_duty((uint16_t)(Foc.dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
				set_b_duty((uint16_t)(Foc.dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
				set_c_duty((uint16_t)(Foc.dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
		}
		
		delay_us_nos(50);
	}
}

void delay_ms(const uint16_t ms)
{
    volatile uint32_t i = ms * 17200;
    while (i-- > 0) {
        __NOP();
    }
}
