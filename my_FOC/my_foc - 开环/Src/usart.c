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
