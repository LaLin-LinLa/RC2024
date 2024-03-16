#include "delay_timer.h"

#define TIMERx				TIMER5

volatile uint32_t TimerCnt;

void delay_timer_Init(void)
{
	timer_enable(TIMERx);
}

/**
	* @brief  ���¶�ʱ��ʱ��
	* @not    ���˺������붨ʱ���жϷ�������
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
	* @brief  ��ȡ��ʱ��ʱ��
	* @param  None
	* @retval ʵʱʱ��
	*/
uint32_t Get_SystemTimer(void)
{
	return TIMER_CNT(TIMERx) + TimerCnt * 0xffff;
}

/**
* @brief  ΢�뼶���������ʱ
* @param  cnt : ��ʱʱ��
* @retval None
*/
void delay_us_nos(uint32_t cnt)
{
	uint32_t temp = cnt  + microsecond();
	while(temp >= microsecond());
}

/**
* @brief  ���뼶���������ʱ
* @param  cnt : ��ʱʱ��
* @retval None
*/
void delay_ms_nos(uint32_t cnt)
{
	uint32_t temp = cnt * 1000 + microsecond();
	while(temp >= microsecond());
}

