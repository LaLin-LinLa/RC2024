#ifndef __DRV_PC_H
#define __DRV_PC_H
#ifdef __cplusplus
extern "C" {
#endif
#include "system.h"

#define PC_FRAME_LENGTH	10u
#define PC_HEAD					0x5a
#define PC_TAIL					0xa5

/*接收PC数据包*/
typedef __PACKED_STRUCT
{
	/*数据接收包，用户自定义*/
	uint8_t head;
	double Data[2];
	uint8_t end;

} PC_rx_Pack_t;
/*发送上位机数据包*/
typedef __PACKED_STRUCT
{
	/*数据发送包，用户自定义*/
	float data[8];			//整型数据
	uint8_t	tail[4];		//包尾数据
	
} PC_tx_Pack_t;

/*发送上位机共用体*/
typedef __PACKED_UNION
{
	PC_tx_Pack_t	txpack;
	unsigned char PcDataArray[sizeof(PC_tx_Pack_t)];
} PC_tx_Union_t;
/*接收上位机共用体*/
typedef __PACKED_UNION
{
	PC_rx_Pack_t	rxpack;
	unsigned char PcDataArray[sizeof(PC_rx_Pack_t)];
} PC_rx_Union_t;

void PC_DatePrcess(volatile const uint8_t * buf);
void PC_DateSend(PC_tx_Union_t *PC_tx);
extern PC_rx_Union_t	PC_rx_union;

extern const PC_rx_Pack_t *get_PC_point(void);

#ifdef __cplusplus
}
#endif
#endif
