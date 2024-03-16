#ifndef _USART_H_
#define _USART_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define	USART0_baudrate		115200		//´®¿Ú0²¨ÌØÂÊ

#define USART0_RX_BUFFER_SIZE 64
#define USART2_RX_BUFFER_SIZE 64
#define USART0_RX_LEN         (USART0_RX_BUFFER_SIZE >> 1)
#define USART2_RX_LEN         (USART2_RX_BUFFER_SIZE >> 1)

extern uint8_t		Usart0_Rx_Buff[USART0_RX_BUFFER_SIZE];
extern uint8_t		Usart2_Rx_Buff[USART2_RX_BUFFER_SIZE];

void usart_Init(void);
extern void usart0_service(void);

#ifdef __cplusplus
}
#endif
#endif
