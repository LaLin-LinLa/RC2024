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

/*������λ�����ݰ�*/
typedef __packed struct
{
	/*���ݷ��Ͱ����û��Զ���*/
	float data[3];			
	uint8_t	tail[4];		//��β����
	
} PC_tx_Pack_t;
/*������λ��������*/
typedef __packed union
{
	PC_tx_Pack_t	txpack;
	unsigned char PcDataArray[sizeof(PC_tx_Pack_t)];
} PC_tx_Union_t;

extern uint8_t 		Usart0_Rx_Buff[USART0_RX_BUFFER_SIZE];
extern uint8_t 		Usart2_Rx_Buff[USART2_RX_BUFFER_SIZE];

extern void usart0_call_back(void);
extern void usart2_call_back(void);
void Usart0_DMA_Transmit(uint32_t data, uint16_t ndtr);
void PC_DateSend(PC_tx_Union_t *PC_tx);

#ifdef __cplusplus
}
#endif
#endif
