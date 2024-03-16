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
	
	 int count_in_cpr;		//�������������
	 int count_in_cpr_prev;//��������һ���������
	 int64_t shadow_count;
	 
   int Encoder_cpr;			//��������������
   float phase;         //��Ƕ�
   float phase_vel;     //����ٶ�
	
	 //PLL use
	 float pos_cpr_counts;			//����λ��
   float vel_estimate_counts;	//�����ٶ�
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
