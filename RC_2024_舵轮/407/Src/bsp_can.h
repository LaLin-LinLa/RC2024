#ifndef __BSP_CAN_H
#define __BSP_CAN_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define CanFilter_0     (0  << 3)
#define CanFilter_1     (1  << 3)
#define CanFilter_2     (2  << 3)
#define CanFilter_3     (3  << 3)
#define CanFilter_4     (4  << 3)
#define CanFilter_5     (5  << 3)
#define CanFilter_6     (6  << 3)
#define CanFilter_7     (7  << 3)
#define CanFilter_8     (8  << 3)
#define CanFilter_9     (9  << 3)
#define CanFilter_10    (10 << 3)
#define CanFilter_11    (11 << 3)
#define CanFilter_12    (12 << 3)
#define CanFilter_13    (13 << 3)
#define CanFilter_14    (14 << 3)
#define CanFilter_15    (15 << 3)
#define CanFilter_16    (16 << 3)
#define CanFilter_17    (17 << 3)
#define CanFilter_18    (18 << 3)
#define CanFilter_19    (19 << 3)
#define CanFilter_20    (20 << 3)
#define CanFilter_21    (21 << 3)
#define CanFilter_22    (22 << 3)
#define CanFilter_23    (23 << 3)
#define CanFilter_24    (24 << 3)
#define CanFilter_25    (25 << 3)
#define CanFilter_26    (26 << 3)
#define CanFilter_27    (27 << 3)

#define CanFifo_0       (0 << 2)
#define CanFifo_1       (1 << 2)

#define Can_STDID       (0 << 1)
#define Can_EXTID       (1 << 1)

#define Can_DataType    (0 << 0)
#define Can_RemoteType  (1 << 0)

#define CAN_LINE_BUSY 0
#define CAN_SUCCESS   1
#define CAN_FIFO_SIZE 1024

typedef struct CAN_RxMessage
{
  CAN_RxHeaderTypeDef header;
  uint8_t             data[8];
}CAN_RxBuffer;

uint8_t bsp_can_Init(CAN_HandleTypeDef* hcan, uint8_t rx_fifox, void (*pFunc)(CAN_RxBuffer*));
void CAN_Filter_Mask_Config(CAN_HandleTypeDef * hcan, uint8_t object_para,uint32_t Id,uint32_t MaskId);
uint8_t CANx_SendData(CAN_HandleTypeDef* hcan,uint16_t ID,uint8_t *pData,uint16_t Len, uint8_t tx_fifox);
uint8_t CANx_SendExtData(CAN_HandleTypeDef* hcan,uint16_t ID,uint8_t *pData,uint16_t Len, uint8_t tx_fifox);



#ifdef __cplusplus
}
#endif
#endif
