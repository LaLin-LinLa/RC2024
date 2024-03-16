#include "system_start.h"
#include "system.h"
#include "bsp_timer.h"
#include "bsp_adc.h"
#include "drv_joystick.h"
#include "drv_lcd_init.h"
#include "drv_lcd.h"
#include "drv_24l01.h"
#include "drv_W25Qx.h"

volatile uint32_t LCDCnt;
uint8_t Tx_buf[] = {0};
uint16_t ID;

/**
	* @brief  系统初始化
	* @retval None
	*/
void System_Init(void)
{	 
	bsp_Timer_Init(1);
	drv_joystick_Init();
//	drv_W25Qx_Init();
//	drv_LCD_Init();
//	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
//  drv_24l01_Init();
//	while(NRF24L01_Check())
//	{
//		LCD_ShowString(10,30,"NRF24L01 Error",RED,WHITE,16, 0);
//		delay_ms_nos(200);
//		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
// 		delay_ms_nos(200);
//	}
//	LCD_ShowString(10,30,"NRF24L01 OK",RED,WHITE,16, 0);
//	NRF24L01_TX_Mode();
//	delay_ms_nos(2000);
//	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
}

/**
	* @brief  系统任务循环
	* @retval None`0
	*/
extern uint16_t Temp[2];
void System_Loop(void)
{
	
	
}

/**
	* @brief  系统任务初始化
	* @retval None
	*/
void RTOS_Init(void)
{
	
}

