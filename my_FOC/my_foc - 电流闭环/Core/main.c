#include "main.h"
#include "system_board.h"
#include "delay_timer.h"
#include "pwm_curr.h"
#include "encoder.h"
#include "FOC.h"
#include "util.h"
#include "usart.h"
#include "user_config.h"
#include "controller.h"
#include "fsm_task.h"

PC_tx_Union_t TX = {0};
uint8_t Cel = 0;

int main(void)
{
	__disable_irq();
	BSP_Init();
	
	USR_CONFIG_set_default_config();
	FSM_init();
	PWMC_init();
	FOC_init();
	Encoder_Init();
	delay_timer_Init();
	
	
	PWMC_SwitchOnPWM();
	__enable_irq();
	
	for(uint8_t i=0, j=0; i<250; i++){
        if(Foc.v_bus_filt > 20){
            if(++j > 20){
                break;
            }
        }
        delay_ms(2);
    }
    
	while(PWMC_CurrentReadingPolarization());
	
	
		
	while(1)
	{
		Encoder_sample();
//		TX.txpack.data[0] = Foc.i_a * 100;
//		TX.txpack.data[1] = Foc.i_b * 100;
//		TX.txpack.data[2] = Foc.i_c * 100;
		
//		TX.txpack.data[0] = Foc.i_q * 1000;
//		TX.txpack.data[1] = iq_set* 1000;
//		TX.txpack.data[2] = Foc.dtc_a * 1000;
		
//		TX.txpack.data[0] = Encoder.raw;
//		
//		PC_DateSend(&TX);
		if(Cel!=0)
		{
			if(Cel == 1)
			{
				FSM_set_state(CALIBRATION);
				Cel = 0;
			}
			if(Cel == 2)
			{
				FSM_set_state(RUN);
				Cel = 0;
			}
			if(Cel == 3)
			{
				FSM_set_state(ANTICOGGING);
				Cel = 0;
			}
			if(Cel == 4)
			{
				FSM_set_state(IDLE);
				Cel = 0;
			}
		}
	}
	
}

void delay_ms(const uint16_t ms)
{
    volatile uint32_t i = ms * 17200;
    while (i-- > 0) {
        __NOP();
    }
}
