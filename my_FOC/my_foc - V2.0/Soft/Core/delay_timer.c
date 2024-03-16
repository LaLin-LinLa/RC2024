#include "delay_timer.h"

#define TIMERx				TIMER5
#define	TIM_NVIC			4

volatile uint32_t TimerCnt;

void delay_timer_Init(void)
{
	rcu_periph_clock_enable(RCU_TIMER5);

	timer_parameter_struct timer_initpara = {0};
	/*定时器参数配置*/
	timer_deinit(TIMER5);
	timer_struct_para_init(&timer_initpara);											//初始化定时器参数
	timer_initpara.prescaler         = 120-1;											//预分频值
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;				//对齐模式：中心对齐模式
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;					//计数方向：向上计数
	timer_initpara.period            = 65535;												//周期
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;					//时钟分频因子
	timer_init(TIMER5, &timer_initpara);

	nvic_irq_enable(TIMER5_IRQn, TIM_NVIC, 0);
	timer_interrupt_enable(TIMER5, TIMER_INT_UP);
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

