#include "system_start.h"
#include "system.h"
#include "bsp_timer.h"
#include "bsp_adc.h"
#include "drv_joystick.h"
#include "drv_lcd_init.h"
#include "drv_lcd.h"
#include "drv_24l01.h"

volatile uint32_t LCDCnt;
uint32_t Tx_buf[] = {0};

/**
	* @brief  系统初始化
	* @retval None
	*/
void System_Init(void)
{	 
	bsp_Timer_Init(1);
	drv_joystick_Init();
	drv_LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
  drv_24l01_Init();
	while(NRF24L01_Check())
	{
		LCD_ShowString(10,30,"NRF24L01 Error",RED,WHITE,16, 0);
		delay_ms_nos(200);
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
 		delay_ms_nos(200);
	}
	LCD_ShowString(10,30,"NRF24L01 OK",RED,WHITE,16, 0);
	NRF24L01_TX_Mode();
	delay_ms_nos(2000);
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
}

/**
	* @brief  系统任务循环
	* @retval None`0
	*/
void System_Loop(void)
{
	uint8_t NRF_sta;
	LCDCnt++;
	Tx_buf[0] = LCDCnt;
	joystick_DatePrcess();
	NRF_sta = NRF24L01_TxPacket((uint8_t*)Tx_buf);
	if(NRF_sta==TX_OK)
	{
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_ShowString(0,0,"TX_OK",RED,WHITE,24,0);
	}
	else if(NRF_sta==MAX_TX)
	{
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_ShowString(0,0,"TX_MAX",RED,WHITE,24,0);
	}
	else
	{
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_ShowString(0,0,"TX_EORR",RED,WHITE,24,0);
	}
//	LCDCnt++;
//	LCD_ShowChinese(0,0,"中景园电子",RED,WHITE,24,0);
//	LCD_ShowString(24,30,"cnt:",RED,WHITE,16,0); 
//	LCD_ShowIntNum(72,30,LCDCnt,4,RED,WHITE,16);
//	delay_ms_nos(1000);
	
//	LCD_Fill(0,0,LCD_W,LCD_H,BLUE);
//	LCD_Fill(0,0,LCD_W,LCD_H,RED);
//	LCD_Fill(0,0,LCD_W,LCD_H,MAGENTA);
	
	
}

/**
	* @brief  系统任务初始化
	* @retval None
	*/
void RTOS_Init(void)
{
	
}

