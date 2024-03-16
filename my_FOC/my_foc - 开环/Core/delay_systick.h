#ifndef _DELAY_SYSTICK_H_
#define _DELAY_SYSTICK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

void delay_systick_Init(void);
void delay_us_sys(__IO uint32_t counts);
void delay_ms_sys(__IO uint32_t counts);

#ifdef __cplusplus
}
#endif
#endif
