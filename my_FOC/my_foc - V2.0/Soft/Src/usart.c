#include "usart.h"

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	return ch;
}
#endif

uint8_t		Usart0_Rx_Buff[USART0_RX_BUFFER_SIZE] = {0};
uint8_t		Usart2_Rx_Buff[USART2_RX_BUFFER_SIZE] = {0};

/**
  * @brief  ���ڳ�ʼ��
 **/
void usart_Init(void)
{
	usart_enable(USART0);
}

/**
  * @brief  ����0DMA����
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
 * @brief USART0�жϷ�����
 */
void usart0_service(void)
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
		dma_channel_enable(DMA0, DMA_CH4);		/* ����DMA���� */

  }
}
