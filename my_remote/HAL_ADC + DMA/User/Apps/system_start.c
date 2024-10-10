#include "system_start.h"
#include "system.h"
#include "bsp_timer.h"

extern ADC_HandleTypeDef hadc1;
uint16_t Buf[2];
float	adc_value[2];

/**
	* @brief  ϵͳ��ʼ��
	* @retval None
	*/
void System_Init(void)
{	     
	bsp_Timer_Init(1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)Buf, 2);
}

/**
	* @brief  ϵͳ����ѭ��
	* @retval None
	*/
void System_Loop(void)
{
	
}

/**
	* @brief  ϵͳ�����ʼ��
	* @retval None
	*/
void RTOS_Init(void)
{
	
}

