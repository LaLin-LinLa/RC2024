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
}

/**
	* @brief  ϵͳ����ѭ��
	* @retval None
	*/
void System_Loop(void)
{
	for(uint8_t i=0; i<2;i++)
	{
		HAL_ADC_Start(&hadc1);									//����ADCת��
		HAL_ADC_PollForConversion(&hadc1, 10);	//�ȴ�ADCת�����
		Buf[i] = HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);
	adc_value[0] = Buf[0]*3.3/4096;
	adc_value[1] = Buf[1]*3.3/4096;
}

/**
	* @brief  ϵͳ�����ʼ��
	* @retval None
	*/
void RTOS_Init(void)
{
	
}

