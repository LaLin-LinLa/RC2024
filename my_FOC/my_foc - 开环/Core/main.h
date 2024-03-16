#ifndef _MAIN_H_
#define _MAIN_H_
#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#include "gd32c10x.h"
#include "gd32c10x_libopt.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern void delay_ms(const uint16_t ms);

#ifdef __cplusplus
}
#endif
#endif

