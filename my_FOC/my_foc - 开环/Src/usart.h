#ifndef _USART_H_
#define _USART_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define USART0_RX_BUFFER_SIZE 64
#define USART2_RX_BUFFER_SIZE 64
#define USART0_RX_LEN         (USART0_RX_BUFFER_SIZE >> 1)
#define USART2_RX_LEN         (USART2_RX_BUFFER_SIZE >> 1)

extern uint8_t 		Usart0_Rx_Buff[USART0_RX_BUFFER_SIZE];
extern uint8_t 		Usart2_Rx_Buff[USART2_RX_BUFFER_SIZE];

extern void usart0_call_back(void);
extern void usart2_call_back(void);

#ifdef __cplusplus
}
#endif
#endif
