/**
  ******************************************************************************
  * @file    drv_PC.c/h
  * @brief   上位机数据收发通用处理代码
  * @version 2.0
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
      -# 本模块基于串口空闲中断DMA双缓存接收数据，用户在实际应用时应根据个人协议，
         在相应中断回调函数中做数据校验后，再调用PC_DatePrcess对原生数据解包
      -# 调用get_miniPC_point获取解包后的数据
      -# 调用PC_DateSend通过串口DMA发送包内容
      
    @attention
      本模块需要依赖bsp_usart.c/h，默认串口为串口1
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have any 
  * bugs, update the version Number, write dowm your name and the date. The most
  * important thing is make sure the users will have clear and definite under-
  * standing through your new brief.
  ******************************************************************************
*/
#include "drv_PC.h"
#include "bsp_usart.h"

PC_rx_Union_t	PC_rx_union = {0};
/**
  * @brief          通过指针获取上位机数据数据
  * @retval         none
  */
const PC_rx_Pack_t *get_PC_point(void)
{
    return &PC_rx_union.rxpack;
}

/**
  * @brief          上位机协议解析
  * @param[in]      buf: 原生数据指针
  * @retval         none
  */
void PC_DatePrcess(volatile const uint8_t * buf)
{
	/*数据解包，用户根据个人协议解包*/
	for(char i=0;i<18;i++)
	{
		PC_rx_union.PcDataArray[i] = buf[i];
	}
}
/**
	* @brief          发送上位机
	* @param[in]      miniPC_tx：发送数据共用体
  * @retval         none
  */
void PC_DateSend(PC_tx_Union_t *PC_tx)
{
  /*数据发送，用户根据个人协议发送*/
	//JustFloat Vofa协议
	PC_tx->txpack.tail[0] = 0x00;
	PC_tx->txpack.tail[1] = 0x00;
	PC_tx->txpack.tail[2] = 0x80;
	PC_tx->txpack.tail[3] = 0x7f;
	
	Usart1_DMA_Transmit((uint32_t)&PC_tx->PcDataArray, sizeof(PC_tx_Pack_t));
}
