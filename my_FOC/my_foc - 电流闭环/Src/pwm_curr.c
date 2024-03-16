#include "pwm_curr.h"

uint16_t ADC0_value[1] = {0};
int16_t phase_b_adc_offset;
int16_t phase_c_adc_offset;

/**
  * @brief  PWM初始化
  * @retval 无
  */
void PWMC_init(void)
{
  adc_interrupt_disable(ADC0, ADC_INT_EOIC);
  adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
  /*ADC0*/ 
  adc_enable(ADC0);   //ADC0使能
  delay_ms(1);
  adc_calibration_enable(ADC0);   //ADC0校准
  adc_dma_mode_enable(ADC0);			//开启ADC0 DMA传输
  adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);  //开启ADC0规则通道软件触发
  adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
  adc_interrupt_enable(ADC0, ADC_INT_EOIC);
  /*ADC1*/
  adc_enable(ADC1);
  delay_ms(1);
  adc_calibration_enable(ADC1);
  //dbg_periph_enable(DBG_TIMER0_HOLD);

  timer_repetition_value_config(TIMER0, 1);

  /* Set all duty to 50% */
  set_a_duty(((uint32_t) HALF_PWM_PERIOD_CYCLES >> 1));
  set_b_duty(((uint32_t) HALF_PWM_PERIOD_CYCLES >> 1));
  set_c_duty(((uint32_t) HALF_PWM_PERIOD_CYCLES >> 1));

  timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
  timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
  timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
  timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
  timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
  timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);

  timer_enable(TIMER0);
  timer_primary_output_config(TIMER0, ENABLE);
  timer_auto_reload_shadow_enable(TIMER0);

}

/**
  * @brief  PWM开启
  * @retval 无
  */
void PWMC_SwitchOnPWM(void)
{
    /* Set all duty to 50% */
    set_a_duty(((uint32_t) HALF_PWM_PERIOD_CYCLES >> 1));
    set_b_duty(((uint32_t) HALF_PWM_PERIOD_CYCLES >> 1));
    set_c_duty(((uint32_t) HALF_PWM_PERIOD_CYCLES >> 1));

    /* wait for a new PWM period */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    while(RESET == timer_flag_get(TIMER0, TIMER_FLAG_UP)){};
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);

    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);
}

/**
  * @brief  PWM关闭
  * @retval 无
  */
void PWMC_SwitchOffPWM(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);
    
    /* wait for a new PWM period */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    while(RESET == timer_flag_get(TIMER0, TIMER_FLAG_UP)){};
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
}

void PWMC_TurnOnLowSides(void)
{
    /* Set all duty to 0% */
    set_a_duty(0);
    set_b_duty(0);
    set_c_duty(0);

    /* wait for a new PWM period */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    while(RESET == timer_flag_get(TIMER0, TIMER_FLAG_UP)){};
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);

    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);
}

int PWMC_CurrentReadingPolarization(void)
{
    int i = 0;
    int adc_sum_b = 0;
    int adc_sum_c = 0;
    
    /* Clear Update Flag */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    /* Wait until next update */
    while(RESET == timer_flag_get(TIMER0, TIMER_FLAG_UP)){};
    /* Clear Update Flag */
    timer_flag_clear(TIMER0, TIMER_FLAG_UP);
    
    while(i < 64) {
        if(timer_flag_get(TIMER0, TIMER_FLAG_UP) == SET){
            timer_flag_clear(TIMER0, TIMER_FLAG_UP);
            
            i ++;
            adc_sum_b += READ_IPHASE_B_ADC();
            adc_sum_c += READ_IPHASE_C_ADC();
        }
    }
    
    phase_b_adc_offset = adc_sum_b/i;
    phase_c_adc_offset = adc_sum_c/i;
    
    // offset check
    i = 0;
    const int Vout = 2122;
    const int check_threshold = 200;
    if(phase_b_adc_offset > (Vout + check_threshold) || phase_b_adc_offset < (Vout - check_threshold)){
        i = -1;
    }
    if(phase_c_adc_offset > (Vout + check_threshold) || phase_c_adc_offset < (Vout - check_threshold)){
        i = -1;
    }
    
    return i;
}

