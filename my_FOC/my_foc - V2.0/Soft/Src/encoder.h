#ifndef _ENCODER_H_
#define _ENCODER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"



// SPI NCS
#define NCS_SET()          GPIO_BOP(GPIOA) = (uint32_t)GPIO_PIN_4;
#define NCS_RESET()        GPIO_BC(GPIOA) = (uint32_t)GPIO_PIN_4;

typedef struct sMT6816 {
    bool no_mag;
    bool over_speed;
    uint32_t angle;
	
    uint8_t rx_err_count;
    uint8_t check_err_count;
} tMT6816;

typedef struct{
   int ecd;
	 int ecd_offset;
	
	 int count_in_cpr;		//编码器脉冲计数
	 int count_in_cpr_prev;//编码器上一次脉冲计数
	 int64_t shadow_count;
	 
   int Encoder_cpr;			//编码器总脉冲数
   float phase;         //电角度
   float phase_vel;     //电角速度
	
	 //PLL use
	 float pos_cpr_counts;			//估计位置
   float vel_estimate_counts;	//估计速度
	 float vel;
	 float pll_kp;
   float pll_ki;
	 float snap_threshold;
	
} Encoder_Struct_t;

extern Encoder_Struct_t	Encoder;

void Encoder_Init(void);
void Encoder_loop(void);

#ifdef __cplusplus
}
#endif
#endif
