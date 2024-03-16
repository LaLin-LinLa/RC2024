#ifndef __SYSTEM_OS_H
#define __SYSTEM_OS_H
#ifdef __cplusplus
extern "C" {
#endif

	#include <FreeRTOS.h>
	#include <task.h>
	#include <queue.h>
	#include <croutine.h>
	#include <FreeRTOSConfig.h>
	#include "cmsis_os.h"
	/*任务堆栈*/
	#define Tiny_Stack_Size       64
	#define Small_Stack_Size      128
	#define Normal_Stack_Size     256
	#define Large_Stack_Size      512
	#define Huge_Stack_Size       1024
	/*任务优先级*/
	#define PriorityVeryLow       1
	#define PriorityLow           2
	#define PriorityBelowNormal   3
	#define PriorityNormal        4
	#define PriorityAboveNormal   5
	#define PriorityHigh          6
	#define PrioritySuperHigh     7
	#define PriorityRealtime      8
	
#ifdef __cplusplus
}
#endif
#endif








