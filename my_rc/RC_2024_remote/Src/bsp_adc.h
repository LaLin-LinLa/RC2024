#ifndef  __BSP_ADC_H
#define  __BSP_ADC_H
#ifdef  __cplusplus
extern "C"{
#endif
#include "main.h"

void bsp_adc_Init(uint16_t *buf, uint16_t len);
uint16_t ADC1_GetValue(uint32_t ch);


#ifdef  __cplusplus
}
#endif
#endif 

