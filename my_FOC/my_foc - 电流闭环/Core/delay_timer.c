#include "delay_timer.h"

#define TIMERx				TIMER5

volatile uint32_t TimerCnt;

void delay_timer_Init(void)
{
	timer_enable(TIMERx);
}

/**
	* @brief  更新定时器时间
	* @not    将此函数加入定时器中断服务函数中
	* @param  None
	* @retval None
	*/
void Update_TimerTick(void)
{
	if(timer_interrupt_flag_get(TIMERx, TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(TIMERx, TIMER_INT_FLAG_UP);
		TimerCnt++;
	}
}

/**
	* @brief  获取定时器时间
	* @param  None
	* @retval 实时时间
	*/
uint32_t Get_SystemTimer(void)
{
	return TIMER_CNT(TIMERx) + TimerCnt * 0xffff;
}

/**
* @brief  微秒级软件堵塞延时
* @param  cnt : 延时时数
* @retval None
*/
void delay_us_nos(uint32_t cnt)
{
	uint32_t temp = cnt  + microsecond();
	while(temp >= microsecond());
}

/**
* @brief  毫秒级软件堵塞延时
* @param  cnt : 延时时数
* @retval None
*/
void delay_ms_nos(uint32_t cnt)
{
	uint32_t temp = cnt * 1000 + microsecond();
	while(temp >= microsecond());
}

