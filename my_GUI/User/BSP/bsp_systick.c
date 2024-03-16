/**
  ******************************************************************************
  * @file    bsp_systick.c/h
  * @brief   Code for SysTick driver in STM32 series MCU	 
	@verbatim  V1.0
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
      -# 调用bsp_systick_Init初始化系统滴答定时器
			-# 调用delay_us_sys以实现微秒级软件堵塞延时
			-# 调用delay_ms_sys以实现毫秒级软件堵塞延时
    @attention
			-# 本模块为定时器堵塞式延时
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
#include "bsp_systick.h"

static __IO uint32_t Time_cnt;


/**
  * @brief  系统滴答定时器 SysTick 初始化
  * @param  temp	
	*					SystemFrequency / 1000    1ms中断一次
	*					SystemFrequency / 100000	 10us中断一次
	*					SystemFrequency / 1000000 1us中断一次
  * @retval 无
  */
void bsp_systick_Init(uint32_t ticks)
{
	if (HAL_SYSTICK_Config(SystemCoreClock / ticks))
	{  
		while (1);
	}
}

/**
  * @brief   us延时程序,10us为一个单位
  * @param  
  *	@arg nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
  * @retval  无
  */
void delay_us_sys(__IO uint32_t nTime)
{ 
	Time_cnt = nTime;	

	while(Time_cnt != 0);
}

/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
void Update_TimingDelay(void)
{
	if (Time_cnt != 0x00)
	{ 
		Time_cnt--;
	}
}
