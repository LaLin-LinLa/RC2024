/**
  ******************************************************************************
  * @file    bsp_spi.c/h
  * @brief   Code for SPI driver in STM32 series MCU	 
	@verbatim  V1.0
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
			-# 使用bsp_spi_Init	启动spi传输
      -# 调用SPI_ReadWrite_8Byte 读写8字节数据
			-# 调用SPI_ReadWrite_16Byte 读写16字节数据
    @attention
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
#include "bsp_spi.h"

/**
  * @brief  SPI初始化
  */
void bsp_spi_Init(SPI_TypeDef* SPIx)
{
	LL_SPI_Enable(SPIx);
}

void SPI_Delay(uint16_t t)
{
	while(t--);
}

/**
	* @brief  SPI发送(8byte)
  * @param  要发送的字节
  */
void SPI_Send_8Byte(SPI_TypeDef* SPIx, uint8_t TxData, uint16_t t)
{
	uint32_t timeout_cnt_num = 1000;
	uint32_t timeout_cnt = 0;
	while(!LL_SPI_IsActiveFlag_TXE(SPIx)){
		timeout_cnt ++;
		if(timeout_cnt > timeout_cnt_num){
			break;
		}
	}

	SPIx->DR = TxData;
	SPI_Delay(t);
}


/**
  * @brief  SPI读写(8byte)
  * @param  要发送的字节
  * @retval 读取到的字节
  */
uint8_t SPI_ReadWrite_8Byte(SPI_TypeDef* SPIx, uint8_t TxData)
{
	uint32_t timeout_cnt_num = 500;
	uint32_t timeout_cnt = 0;
	while(!LL_SPI_IsActiveFlag_TXE(SPIx)){
		timeout_cnt ++;
		if(timeout_cnt > timeout_cnt_num){
			break;
		}
	}

	SPIx->DR = TxData;

	timeout_cnt = 0;
	while(!LL_SPI_IsActiveFlag_RXNE(SPIx)){
		timeout_cnt ++;
		if(timeout_cnt > timeout_cnt_num){
			break;
		}
	}
	return SPIx->DR;

}

/**
  * @brief  SPI读写(16byte)
  * @param  要发送的字节
  * @retval 读取到的字节
  */
uint16_t SPI_ReadWrite_16Byte(SPI_TypeDef* SPIx, uint16_t TxData)
{
	uint32_t timeout_cnt_num = 500;
	uint32_t timeout_cnt = 0;
	while(!LL_SPI_IsActiveFlag_TXE(SPIx)){
		timeout_cnt ++;
		if(timeout_cnt > timeout_cnt_num){
			break;
		}
	}
	SPIx->DR = (uint16_t)TxData;

	timeout_cnt = 0;
	while(!LL_SPI_IsActiveFlag_RXNE(SPIx)){
		timeout_cnt ++;
		if(timeout_cnt > timeout_cnt_num){
			break;
		}
	}
	return LL_SPI_ReceiveData16(SPIx);;

}

/**
  * @brief  SPI速度设置
	* @param  SPIx: SPI选择
	* @param  SPI_BaudRatePrescaler: SPI速率
  */
void SPI_SetSpeed(SPI_TypeDef* SPIx, uint8_t Prescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(Prescaler));	//断言检查
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=Prescaler;	
	LL_SPI_Enable(SPIx);
} 

