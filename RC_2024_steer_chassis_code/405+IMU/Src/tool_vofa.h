#ifndef  __TOOL_VOFA_H
#define  __TOOL_VOFA_H
#ifdef  __cplusplus
extern "C"{
#endif
#include "main.h"

typedef union     
{
	float fdata;
	unsigned long ldata;
}FloatLongType;

void JustFloat_Send(float * fdata,uint16_t fdata_num,USART_TypeDef *Usart_choose);

#ifdef  __cplusplus
}
#endif
#endif 

