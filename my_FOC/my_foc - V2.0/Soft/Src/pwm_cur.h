#ifndef _PWM_CUR_H_
#define _PWM_CUR_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "util.h"


#define PWM_FREQUENCY               20000
#define CURRENT_MEASURE_HZ          PWM_FREQUENCY     //电流频率20KHz
#define CURRENT_MEASURE_PERIOD      (float)(1.0f/(float)CURRENT_MEASURE_HZ)   //电流测量周期50us

#define TIMER0_CLK_MHz              120
#define PWM_PERIOD_CYCLES            (uint16_t)((TIMER0_CLK_MHz*(uint32_t)1000000u/((uint32_t)(PWM_FREQUENCY)))&0xFFFE)
#define HALF_PWM_PERIOD_CYCLES       (uint16_t)(PWM_PERIOD_CYCLES>>1)

#define SHUNT_RESISTENCE            (0.015f)	//采样电阻
#define V_SCALE                     ((float)(19.0f * 3.3f / 4095.0f)) //V_scale = Vref / (2^N-1) / V_pro
#define I_SCALE                     ((float)((3.3f / 4095.0f) / SHUNT_RESISTENCE))

#define READ_Vbus_ADC()             ((uint16_t)(ADC_IDATA0(ADC0)))
#define READ_IPHASE_B_ADC()         ((uint16_t)(ADC_IDATA1(ADC0)))
#define READ_IPHASE_C_ADC()         ((uint16_t)(ADC_IDATA0(ADC1)))
#define NTC_RES(adc_val)		((4095.0f * 3300.0f) / adc_val - 3300.0f)
#define READ_TEMP		(1.0f / ((logf(NTC_RES(adc_tmp[0]) / 10000.0f) / 3380.0f) + (1.0f / 298.15f)) - 273.15f)

extern uint16_t 	adc_tmp[1];
extern int16_t phase_b_adc_offset;
extern int16_t phase_c_adc_offset;

//读取Vbus电压
static inline float read_vbus(void) {
	return (float)(READ_Vbus_ADC()) * V_SCALE;
}
//读取Ib电流
static inline float read_iphase_b(void) {
    return (float)(READ_IPHASE_B_ADC() - phase_b_adc_offset) * I_SCALE;
}
//读取Ic电流
static inline float read_iphase_c(void) {
    return (float)(READ_IPHASE_C_ADC() - phase_c_adc_offset) * I_SCALE;
}
//读取温度
static inline void read_temp(float *volPin){
	UTILS_LP_FAST(*volPin, READ_TEMP, 0.1f);                 
}

static inline void set_a_duty(uint32_t duty)            { TIMER_CH0CV(TIMER0) = duty; }
static inline void set_b_duty(uint32_t duty)            { TIMER_CH1CV(TIMER0) = duty; }
static inline void set_c_duty(uint32_t duty)            { TIMER_CH2CV(TIMER0) = duty; }

void PWMC_Init(void);
void PWMC_SwitchOnPWM(void);
void PWMC_SwitchOffPWM(void);
void PWMC_TurnOnLowSides(void);
int PWMC_CurrentReadingPolarization(void);

#ifdef __cplusplus
}
#endif
#endif
