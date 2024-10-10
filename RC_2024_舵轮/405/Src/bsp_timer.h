#ifndef  __BSP_TIMER_H
#define  __BSP_TIMER_H
#ifdef  __cplusplus
extern "C"{
#endif
#include "main.h"

#define microsecond()    Get_SystemTimer()

void bsp_Timer_Init(uint8_t mode);
void Update_SystemTick(void);
uint32_t Get_SystemTimer(void);
void delay_ms_nos(uint32_t cnt);
void delay_us_nos(uint32_t cnt);


#ifdef  __cplusplus
}
#endif
#endif 

