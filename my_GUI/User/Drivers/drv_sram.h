#ifndef _DRV_SRAM_H_
#define _DRV_SRAM_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "system.h"

void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 NumHalfwordToWrite);
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 NumHalfwordToRead);
void FSMC_SRAM_Test(uint16_t x,uint16_t y);

#ifdef __cplusplus
}
#endif
#endif

