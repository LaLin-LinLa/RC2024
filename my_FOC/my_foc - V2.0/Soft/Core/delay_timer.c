#include "delay_timer.h"

#define TIMERx				TIMER5
#define	TIM_NVIC			4

volatile uint32_t TimerCnt;

void delay_timer_Init(void)
{
	rcu_periph_clock_enable(RCU_TIMER5);

	timer_parameter_struct timer_initpara = {0};
	/*��ʱ����������*/
	timer_deinit(TIMER5);
	timer_struct_para_init(&timer_initpara);											//��ʼ����ʱ������
	timer_initpara.prescaler         = 120-1;											//Ԥ��Ƶֵ
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;				//����ģʽ�����Ķ���ģʽ
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;					//�����������ϼ���
	timer_initpara.period            = 65535;												//����
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;					//ʱ�ӷ�Ƶ����
	timer_init(TIMER5, &timer_initpara);

	nvic_irq_enable(TIMER5_IRQn, TIM_NVIC, 0);
	timer_interrupt_enable(TIMER5, TIMER_INT_UP);
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

