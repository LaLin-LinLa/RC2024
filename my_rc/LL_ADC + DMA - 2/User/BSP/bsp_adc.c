#include "bsp_adc.h"

/**
	* @brief  ADC��ʼ��
	*/
void bsp_adc_Init(uint16_t *buf, uint16_t len)
{
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_CHANNEL_0,(uint32_t)buf);
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_CHANNEL_0,LL_ADC_DMA_GetRegAddr(ADC1,LL_ADC_DMA_REG_REGULAR_DATA));
	LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_0, len);
	LL_DMA_DisableStream(DMA2, LL_DMA_CHANNEL_0);
	LL_DMA_EnableStream(DMA2, LL_DMA_CHANNEL_0);
	
	LL_ADC_Enable(ADC1); //ʹ��ADC
	/*У׼����F4����ִ��*/
//	LL_ADC_StartCalibration(ADC1);//��ʼУ׼ADC1 
//	while(LL_ADC_IsCalibrationOnGoing(ADC1)); //�ȴ�ADCУ׼���
	LL_ADC_REG_StartConversionSWStart(ADC1);
	LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
}

/**
	* @brief  ��ȡADC1ͨ��x��ֵ
	* @retval 
	*/
uint16_t ADC1_GetValue(uint32_t ch)
{
	uint16_t adc_value = 0;
	LL_ADC_REG_SetSequencerRanks(ADC1,LL_ADC_REG_RANK_1,ch);	//���ù�������1��ͨ��x
	LL_ADC_REG_StartConversionSWStart(ADC1);	//��������ͨ��ת��
	while(!LL_ADC_IsActiveFlag_EOCS(ADC1)){};//�ȴ�ת������	
	adc_value=LL_ADC_REG_ReadConversionData12(ADC1);
	return adc_value;
}
