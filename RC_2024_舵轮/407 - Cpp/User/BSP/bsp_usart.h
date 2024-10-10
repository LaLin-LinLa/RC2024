#ifndef __BSP_USART_H
#define __BSP_USART_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef void (*usart_call_back)(uint8_t *buf, uint16_t len);
typedef struct
{
  uint16_t rx_buffer_size;
  uint8_t *rx_buffer0;
	uint8_t *rx_buffer1;
  usart_call_back call_back_f;
} usart_manage_obj_t;


void bsp_usart1_Init(uint8_t* rx_buffr0, uint8_t* rx_buffr1, uint16_t rx_len, usart_call_back fun);
void bsp_usart2_Init(uint8_t* rx_buffr0, uint8_t* rx_buffr1, uint16_t rx_len, usart_call_back fun);
void bsp_uart4_Init(uint8_t* rx_buffr0, uint8_t* rx_buffr1, uint16_t len, usart_call_back fun);

void Usart1_DMA_Transmit(uint32_t data, uint16_t ndtr);
extern void usart1_IQR_Service(void);
extern void usart2_IQR_Service(void);
extern void uart4_IQR_Service(void);

#ifdef __cplusplus
}
#endif
#endif

