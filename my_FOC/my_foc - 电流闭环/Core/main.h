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

/* FLASH MAP ---------------------------------------------*/
#define PAGE_SIZE               ((uint32_t)0x400U)    // 1KB

#define APP_MAIN_ADDR            ((uint32_t)(0x8000000 +  0 * PAGE_SIZE))        // Page 0
#define APP_BACK_ADDR            ((uint32_t)(0x8000000 + 50 * PAGE_SIZE))        // Page 50
#define APP_MAX_SIZE            ((uint32_t)(50 * PAGE_SIZE))                    // 50KB

#define BOOTLOADER_ADDR            ((uint32_t)(0x8000000 + 100 * PAGE_SIZE))        // Page 100
#define BOOTLOADER_MAX_SIZE        ((uint32_t)(10 * PAGE_SIZE))                    // 10KB

#define USR_CONFIG_ADDR            ((uint32_t)(0x8000000 + 110 * PAGE_SIZE))        // Page 110
#define USR_CONFIG_MAX_SIZE        ((uint32_t)(5 * PAGE_SIZE))                        // 5KB

#define COGGING_MAP_ADDR        ((uint32_t)(0x8000000 + 115 * PAGE_SIZE))        // Page 115
#define COGGING_MAP_MAX_SIZE    ((uint32_t)(10 * PAGE_SIZE))                    // 10KB	

extern void delay_ms(const uint16_t ms);

#ifdef __cplusplus
}
#endif
#endif

