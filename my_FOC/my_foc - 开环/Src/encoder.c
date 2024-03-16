#include "encoder.h"
#include "util.h"
#include "pwm_curr.h"
#include "user_config.h"

tMT6816						MT6816;
Encoder_Struct_t	Encoder;

static void MT6816_Init(void);

/**
  * @brief  传感器初始化
  */
void Encoder_Init(void)
{
	MT6816_Init();

	// Init
	Encoder.raw = 0;
	Encoder.count_in_cpr = 0;
	Encoder.count_in_cpr_prev = 0;
	Encoder.shadow_count = 0;
	Encoder.pos_cpr_counts = 0;
	Encoder.vel_estimate_counts = 0;
	Encoder.pos = 0;
	Encoder.vel = 0;
	Encoder.phase = 0;
	Encoder.phase_vel = 0;
	
	Encoder.interpolation = 0;
    
	int encoder_pll_bw = 2000;
	float bandwidth = MIN(encoder_pll_bw, 0.25f * PWM_FREQUENCY);
	Encoder.pll_kp = 2.0f * bandwidth;                  // basic conversion to discrete time
	Encoder.pll_ki = 0.25f * SQ(Encoder.pll_kp);         // Critically damped
	Encoder.snap_threshold = 0.5f * CURRENT_MEASURE_PERIOD * Encoder.pll_ki;

}

/**
  * @brief  MT6816初始化
  */
static void MT6816_Init(void)
{	
	Encoder.Encoder_cpr = 16384;
}

/**
  * @brief  传感器采样
  */
bool Encoder_sample(void){
	bool		 pc_flag;
	uint16_t pc;
	uint8_t h_count;
	uint8_t data[3];
	uint32_t timeout_cnt;
	static const uint32_t timeout_cnt_max = 10;

	/*===============MT6816 begin===============*/
	for(uint8_t i=0;i<3;i++){
		NCS_RESET();
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			spi_i2s_data_transmit(SPI0, (((0x80 | 0x03))<<8));
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			data[0] = spi_i2s_data_receive(SPI0);
		NCS_SET();
		NCS_RESET();
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			spi_i2s_data_transmit(SPI0, ((0x80 | 0x04)<<8));
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			data[1] = spi_i2s_data_receive(SPI0);
		NCS_SET();
		NCS_RESET();
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			spi_i2s_data_transmit(SPI0, ((0x80 | 0x05)<<8));
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			data[2] = spi_i2s_data_receive(SPI0);
		NCS_SET();

		//奇偶校验
		pc = ((data[0] & 0x00FF) << 8) | (data[1] & 0x00FF);
		h_count = 0;
		for(uint8_t j=0; j<16; j++){
			if(pc & (0x0001 << j))
				h_count++;
		}
		if(h_count & 0x01){
			pc_flag = false;
			if(MT6816.check_err_count < 0xFF)
					MT6816.check_err_count++;
		}
		else{
			pc_flag = true;break;}
	}
	if(pc_flag)
	{
		MT6816.angle = pc >> 2;
		MT6816.no_mag = (bool)(pc & (0x0001 << 1));  
		MT6816.over_speed = data[2] & 0x04;
		return true;
	}
	else
		return false;
	/*===============MT6816 end===============*/

TIMEOUT:
	NCS_SET();
	if(MT6816.rx_err_count < 0xFF)
		MT6816.rx_err_count++;
	return false;

}

/**
  * @brief  传感器循环
  */
void ENCODER_loop(void)
{
	if(Encoder_sample()){
		if(UsrConfig.encoder_dir == +1){
				Encoder.raw = MT6816.angle;
		}else{
				Encoder.raw = (Encoder.Encoder_cpr - MT6816.angle);
		}
	}

	/* Linearization */
	//插值函数线性化
	int off_1 = UsrConfig.offset_lut[(Encoder.raw)>>7];                                        // lookup table lower entry
	int off_2 = UsrConfig.offset_lut[((Encoder.raw>>7)+1)%128];                                // lookup table higher entry
	//off_1和off_2为相邻的两个数
	int off_interp = off_1 + ((off_2 - off_1)*(Encoder.raw - ((Encoder.raw>>7)<<7))>>7);   		 //线性化公式y=kx+b 右移7位就是除以128    
	int count = Encoder.raw - off_interp;
	
	/*  Wrap in ENCODER_CPR */
	//判断经过编码器0点时值的跳变 
	while(count > Encoder.Encoder_cpr) count -= Encoder.Encoder_cpr;
	while(count < 0          ) count += Encoder.Encoder_cpr;
	Encoder.count_in_cpr = count;
	
	/* Delta count */
	//增量计数
	int delta_count = Encoder.count_in_cpr - Encoder.count_in_cpr_prev;
	Encoder.count_in_cpr_prev = Encoder.count_in_cpr;
	while(delta_count > +(Encoder.Encoder_cpr>>1)) delta_count -= Encoder.Encoder_cpr;
	while(delta_count < -(Encoder.Encoder_cpr>>1)) delta_count += Encoder.Encoder_cpr;
	
	/* Add measured delta to encoder count */
	Encoder.shadow_count += delta_count;

	/* Run vel PLL */
	//龙伯格观测器PLL
	Encoder.pos_cpr_counts += CURRENT_MEASURE_PERIOD * Encoder.vel_estimate_counts;
	float delta_pos_cpr_counts = (float)(Encoder.count_in_cpr - (int)Encoder.pos_cpr_counts);
	while(delta_pos_cpr_counts > +(Encoder.Encoder_cpr>>1)) delta_pos_cpr_counts -= ((float)Encoder.Encoder_cpr);
	while(delta_pos_cpr_counts < -(Encoder.Encoder_cpr>>1)) delta_pos_cpr_counts += ((float)Encoder.Encoder_cpr);
	Encoder.pos_cpr_counts += CURRENT_MEASURE_PERIOD * Encoder.pll_kp * delta_pos_cpr_counts;
	while(Encoder.pos_cpr_counts > Encoder.Encoder_cpr) Encoder.pos_cpr_counts -= ((float)Encoder.Encoder_cpr);
	while(Encoder.pos_cpr_counts < 0          ) Encoder.pos_cpr_counts += ((float)Encoder.Encoder_cpr);
	Encoder.vel_estimate_counts += CURRENT_MEASURE_PERIOD * Encoder.pll_ki * delta_pos_cpr_counts;
	// align delta-sigma on zero to prevent jitter
	bool snap_to_zero_vel = false;
	if (ABS(Encoder.vel_estimate_counts) < Encoder.snap_threshold) {
			Encoder.vel_estimate_counts = 0.0f;
			snap_to_zero_vel = true;
	}
	
	// run encoder count interpolation
	// if we are stopped, make sure we don't randomly drift
	if (snap_to_zero_vel) {
			Encoder.interpolation = 0.5f;
	// reset interpolation if encoder edge comes
	} else if (delta_count > 0) {
			Encoder.interpolation = 0.0f;
	} else if (delta_count < 0) {
			Encoder.interpolation = 1.0f;
	} else {
			// Interpolate (predict) between encoder counts using vel_estimate,
			Encoder.interpolation += CURRENT_MEASURE_PERIOD * Encoder.vel_estimate_counts;
			// don't allow interpolation indicated position outside of [enc, enc+1)
			if (Encoder.interpolation > 1.0f) Encoder.interpolation = 1.0f;
			if (Encoder.interpolation < 0.0f) Encoder.interpolation = 0.0f;
	}
	float interpolated_enc = Encoder.count_in_cpr - UsrConfig.encoder_offset + Encoder.interpolation;
	while(interpolated_enc > Encoder.Encoder_cpr) interpolated_enc -= Encoder.Encoder_cpr;
	while(interpolated_enc < 0          ) interpolated_enc += Encoder.Encoder_cpr;
	
	float shadow_count_f = Encoder.shadow_count;
	float turns = shadow_count_f / ((float)Encoder.Encoder_cpr);
	float residual = shadow_count_f - turns * ((float)Encoder.Encoder_cpr);

	/* Outputs from Encoder for Controller */
	Encoder.pos = turns + residual / ((float)Encoder.Encoder_cpr);
	UTILS_LP_MOVING_AVG_APPROX(Encoder.vel, (Encoder.vel_estimate_counts / ((float)Encoder.Encoder_cpr)), 5);
	Encoder.phase = (interpolated_enc * M_2PI * UsrConfig.motor_pole_pairs) / ((float)Encoder.Encoder_cpr);
	Encoder.phase_vel = Encoder.vel * M_2PI * UsrConfig.motor_pole_pairs;

}
