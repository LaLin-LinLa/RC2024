#ifndef _PWM_CURR_H_
#define _PWM_CURR_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define PWM_FREQUENCY               20000
#define CURRENT_MEASURE_HZ          PWM_FREQUENCY     //电流频率
#define CURRENT_MEASURE_PERIOD      (float)(1.0f/(float)CURRENT_MEASURE_HZ)   //电流测量周期

#define TIMER0_CLK_MHz              120
#define PWM_PERIOD_CYCLES            (uint16_t)((TIMER0_CLK_MHz*(uint32_t)1000000u/((uint32_t)(PWM_FREQUENCY)))&0xFFFE)
#define HALF_PWM_PERIOD_CYCLES       (uint16_t)(PWM_PERIOD_CYCLES>>1)

#define SHUNT_RESISTENCE            (0.015f)
#define V_SCALE                     ((float)(19.0f * 3.3f / 4095.0f)) //V_scale = Vref / (2^N-1) / V_pro
#define I_SCALE                     ((float)((3.3f / 4095.0f) / SHUNT_RESISTENCE))

#define READ_Vbus_ADC()             ((uint16_t)(ADC_IDATA0(ADC0)))
#define READ_IPHASE_B_ADC()         ((uint16_t)(ADC_IDATA1(ADC0)))
#define READ_IPHASE_C_ADC()         ((uint16_t)(ADC_IDATA0(ADC1)))


extern uint16_t 	ADC0_value[1];
extern int16_t phase_b_adc_offset;
extern int16_t phase_c_adc_offset;

static inline void set_a_duty(uint32_t duty)            { TIMER_CH0CV(TIMER0) = duty; }
static inline void set_b_duty(uint32_t duty)            { TIMER_CH1CV(TIMER0) = duty; }
static inline void set_c_duty(uint32_t duty)            { TIMER_CH2CV(TIMER0) = duty; }

void PWMC_init(void);
void PWMC_SwitchOnPWM(void);
void PWMC_SwitchOffPWM(void);
void PWMC_TurnOnLowSides(void);
int PWMC_CurrentReadingPolarization(void);

#ifdef __cplusplus
}
#endif
#endif
