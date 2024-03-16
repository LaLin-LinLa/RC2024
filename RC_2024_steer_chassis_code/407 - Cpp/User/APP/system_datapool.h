#ifndef __SYSTEM_DATAPOOL_H
#define __SYSTEM_DATAPOOL_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

extern CAN_HandleTypeDef 	hcan1;
extern CAN_HandleTypeDef 	hcan2;

#define USART1_RX_BUFFER_SIZE 32
#define UART4_RX_BUFFER_SIZE 32
#define USART2_RX_BUFFER_SIZE 32

extern uint8_t Usart1_Rx_Buff[2][USART1_RX_BUFFER_SIZE];
extern uint8_t Uart4_Rx_Buff[2][UART4_RX_BUFFER_SIZE];
extern uint8_t Usart2_Rx_Buff[2][USART2_RX_BUFFER_SIZE];


typedef struct
{
  uint8_t  port_num;
  int16_t  len;
  void*    address;
}USART_COB;

typedef struct{
  uint16_t  ID;
  uint8_t   DLC;
  void*    address;
}CAN_COB;


#ifdef __cplusplus
}
#endif
#endif
