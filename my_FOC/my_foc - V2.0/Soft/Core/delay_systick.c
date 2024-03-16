/**
  ******************************************************************************
  * @file    delay_systick.c/h
  * @brief   Code for SysTick driver in GD32 series MCU	 
	@verbatim  V1.0
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
      -# ����delay_systick_Init��ʼ��ϵͳ�δ�ʱ��
			-# ����delay_us_sys��ʵ��΢�뼶���������ʱ
			-# ����delay_ms_sys��ʵ�ֺ��뼶���������ʱ
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
  * @brief  ϵͳ�δ�ʱ�� SysTick ��ʼ��
  * @retval ��
  */
void delay_systick_Init(void)
{
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
}

/**
  * @brief   us��ʱ����
  * @retval  ��
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
  * @brief   ms��ʱ����
  * @retval  ��
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


