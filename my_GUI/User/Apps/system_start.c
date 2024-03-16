#include "system_start.h"
#include "system.h"
#include "bsp_timer.h"
#include "drv_LCD.h"

uint8_t Rx_buf[33];

/**
	* @brief  ϵͳ��ʼ��
	* @retval None
	*/
void System_Init(void)
{	     
	bsp_Timer_Init(1);
	drv_LCD_Init();
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
}

/**
	* @brief  ϵͳ����ѭ��
	* @retval None
	*/
void System_Loop(void)
{
	
}


