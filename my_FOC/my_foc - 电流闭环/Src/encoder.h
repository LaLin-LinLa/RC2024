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
    int raw;
    int count_in_cpr;
    int count_in_cpr_prev;
		int Encoder_cpr;
	
	
    int64_t shadow_count;
    
    // pll use
    float pos_cpr_counts;
    float vel_estimate_counts;		//¹À¼ÆËÙ¶È
    
    float pos;
    float vel;
    
    float phase;
    float phase_vel;
    
    float pll_kp;
    float pll_ki;
    float interpolation;
    float snap_threshold;

} Encoder_Struct_t;

extern Encoder_Struct_t	Encoder;

void Encoder_Init(void);
bool Encoder_sample(void);
void ENCODER_loop(void);

#ifdef __cplusplus
}
#endif
#endif
