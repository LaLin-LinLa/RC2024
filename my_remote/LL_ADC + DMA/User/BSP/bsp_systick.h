#ifndef _BSP_SYSTICK_H_
#define _BSP_SYSTICK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

void bsp_systick_Init(uint32_t temp);
void delay_us_sys(__IO uint32_t nTime);
extern void Update_TimingDelay(void);
#define delay_ms_sys(x)	delay_us_sys(100*x)

#ifdef __cplusplus
}
#endif
#endif
