#ifndef _DRV_JOYSTICK_H_
#define _DRV_JOYSTICK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "system.h" 


typedef struct{
	 int X_value;
	 int Y_value;
	
} joystick_ctrl_t;

extern joystick_ctrl_t	joystick;

void drv_joystick_Init(void);
void joystick_DatePrcess(void);

#ifdef __cplusplus
}
#endif
#endif

