#include "system_start.h"
#include "system.h"
#include "bsp_timer.h"
#include "drv_LCD.h"

uint8_t Rx_buf[33];

/**
	* @brief  系统初始化
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
	* @brief  系统任务循环
	* @retval None
	*/
void System_Loop(void)
{
	
}


