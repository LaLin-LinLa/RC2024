/**
  ******************************************************************************
  * @file    bsp_timer.c/h
  * @brief   Code for Timer driver in STM32 series MCU	 
	@verbatim  V1.0
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
      -# 调用bsp_Timer_Init初始化定时器
			-# 调用delay_us_nos以实现微秒级软件堵塞延时
			-# 调用delay_ms_nos以实现毫秒级软件堵塞延时
    @attention
			-# 本模块为定时器堵塞式延时,默认定时器为基本定时器7
      -# 本模块仅支持LL库
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have any 
  * bugs, update the version Number, write dowm your name and the date. The most
  * important thing is make sure the users will have clear and definite under-
  * standing through your new brief.
  ******************************************************************************
*/
#include "bsp_timer.h"

#define TIMx TIM10

volatile uint32_t SystemTimerCnt;
static uint8_t delay_mode = 0;

/**
	* @brief  初始化定时器
	* @param  mode : 选择使用模块定时还是HAL定时
	*					0：使用HAL库延时 1：使用定时器模块延时
	* @retval None
	*/
void bsp_Timer_Init(uint8_t mode)
{
	delay_mode = mode;
	LL_TIM_EnableIT_UPDATE(TIMx);
	LL_TIM_ClearFlag_UPDATE(TIMx);
	LL_TIM_EnableCounter(TIMx);
}


/**
	* @brief  获取定时器时间
	* @param  None
	* @retval 实时时间
	*/
uint32_t Get_SystemTimer(void)
{
	return TIMx->CNT + SystemTimerCnt * 0xffff;
}

/**
	* @brief  更新定时器时间
	* @not    将此函数加入定时器中断服务函数中
	* @param  None
	* @retval None
	*/
void Update_SystemTick(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIMx) == SET)
	{
		LL_TIM_ClearFlag_UPDATE(TIMx);
		SystemTimerCnt++;
	}
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
	if(delay_mode)
	{
		uint32_t temp = cnt * 1000 + microsecond();
		while(temp >= microsecond());
	}
	else
		HAL_Delay(cnt);
}


