#ifndef _DELAY_TIMER_H_
#define _DELAY_TIMER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define microsecond()    Get_SystemTimer()

void delay_timer_Init(void);
void Update_TimerTick(void);
void delay_us_nos(uint32_t cnt);
void delay_ms_nos(uint32_t cnt);

#ifdef __cplusplus
}
#endif
#endif
