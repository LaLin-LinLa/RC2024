#include "usart.h"

uint8_t		Usart0_Rx_Buff[USART0_RX_BUFFER_SIZE] = {0};
uint8_t		Usart2_Rx_Buff[USART2_RX_BUFFER_SIZE] = {0};

int fputc(int ch, FILE *f)
{
	usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	return ch;
}

/**
  * @brief  串口0DMA传输
 **/
void Usart0_DMA_Transmit(uint32_t data, uint16_t ndtr)
{
	while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
	usart_flag_clear(USART0, USART_FLAG_TC);
	dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_G);
	dma_channel_disable(DMA0, DMA_CH3);
	dma_memory_address_config(DMA0, DMA_CH3, data);
	dma_transfer_number_config(DMA0, DMA_CH3, ndtr);
	dma_channel_enable(DMA0, DMA_CH3);
}


/**
 * @brief USART0中断回调函数
 */
void usart0_call_back(void)
{
	static uint16_t this_time_rx_len = 0;
  if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) == SET)
  {
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
    usart_data_receive(USART0);
    dma_channel_disable(DMA0, DMA_CH4);
    this_time_rx_len = USART0_RX_LEN - dma_transfer_number_get(DMA0,DMA_CH4);
    


    dma_memory_address_config(DMA0,DMA_CH4,(uint32_t)Usart0_Rx_Buff);
		dma_transfer_number_config(DMA0,DMA_CH4,USART0_RX_LEN);
		dma_channel_enable(DMA0, DMA_CH4);		/* 开启DMA传输 */

  }
}

/**
 * @brief USART2中断回调函数
 */
void usart2_call_back(void)
{
	static uint16_t this_time_rx_len = 0;
  if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE) == SET)
  {
    usart_interrupt_flag_clear(USART2, USART_INT_FLAG_IDLE);
    usart_data_receive(USART2);
    dma_channel_disable(DMA0, DMA_CH2);
    this_time_rx_len = USART2_RX_LEN - dma_transfer_number_get(DMA0,DMA_CH2);
    


    dma_memory_address_config(DMA0,DMA_CH2,(uint32_t)Usart2_Rx_Buff);
		dma_transfer_number_config(DMA0,DMA_CH2,USART2_RX_LEN);
		dma_channel_enable(DMA0, DMA_CH2);		/* 开启DMA传输 */

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
	
	Usart0_DMA_Transmit((uint32_t)&PC_tx->PcDataArray, sizeof(PC_tx_Pack_t));
}
