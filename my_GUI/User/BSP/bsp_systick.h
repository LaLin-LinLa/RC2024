#ifndef _BSP_SYSTICK_H_
#define _BSP_SYSTICK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define SYSTICK_MS_TICK  	1000
#define SYSTICK_10US_TICK 100000
#define SYSTICK_US_TICK 	1000000

void bsp_systick_Init(uint32_t ticks);
void delay_us_sys(__IO uint32_t nTime);
extern void Update_TimingDelay(void);
#define delay_ms_sys(x)	delay_us_sys(100*x)

#ifdef __cplusplus
}
#endif
#endif
