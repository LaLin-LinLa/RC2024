/**
  ******************************************************************************
  * @file    delay_systick.c/h
  * @brief   Code for SysTick driver in GD32 series MCU	 
	@verbatim  V1.0
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
      -# 调用delay_systick_Init初始化系统滴答定时器
			-# 调用delay_us_sys以实现微秒级软件堵塞延时
			-# 调用delay_ms_sys以实现毫秒级软件堵塞延时
    @attention
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have any 
  * bugs, update the version Number, write dowm your name and the date. The most
  * important thing is make sure the users will have clear and definite under-
  * standing through your new brief.
  ******************************************************************************
*/
#include "delay_systick.h"

#define COUNTS_MS	((float)SystemCoreClock/8000)
#define COUNTS_US	((float)SystemCoreClock/8000000)

/**
  * @brief  系统滴答定时器 SysTick 初始化
  * @retval 无
  */
void delay_systick_Init(void)
{
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
}

/**
  * @brief   us延时程序
  * @retval  无
  */
void delay_us_sys(__IO uint32_t counts)
{
	uint32_t ctl;
	SysTick->LOAD = (uint32_t)(counts * COUNTS_US);
	SysTick->VAL = 0x0000U;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	do{
		ctl = SysTick->CTRL;
	}while((ctl & SysTick_CTRL_ENABLE_Msk) && !(ctl & SysTick_CTRL_COUNTFLAG_Msk));
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x0000U;
}

/**
  * @brief   ms延时程序
  * @retval  无
  */
void delay_ms_sys(__IO uint32_t counts)
{
		uint32_t ctl;
	SysTick->LOAD = (uint32_t)(counts * COUNTS_MS);
	SysTick->VAL = 0x0000U;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	do{
		ctl = SysTick->CTRL;
	}while((ctl & SysTick_CTRL_ENABLE_Msk) && !(ctl & SysTick_CTRL_COUNTFLAG_Msk));
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x0000U;
}


