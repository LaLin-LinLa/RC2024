#include "encoder.h"
#include "pwm_cur.h"
#include "usr_config.h"

tMT6816						MT6816;
Encoder_Struct_t	Encoder = {0};

static void MT6816_Init(void);

/**
  * @brief  传感器初始化
  */
void Encoder_Init(void)
{
	MT6816_Init();
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
//		NCS_SET();
//		NCS_RESET();
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			spi_i2s_data_transmit(SPI0, ((0x80 | 0x04)<<8));
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			timeout_cnt = 0;
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)){ if(timeout_cnt ++ > timeout_cnt_max){goto TIMEOUT;}}
			data[1] = spi_i2s_data_receive(SPI0);
//		NCS_SET();
//		NCS_RESET();
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
void Encoder_loop(void)
{
	if(Encoder_sample()){
		Encoder.ecd = MT6816.angle - Encoder.ecd_offset;
	}
	//判断经过编码器0点时值的跳变 
	while(Encoder.ecd > Encoder.Encoder_cpr) Encoder.ecd -= Encoder.Encoder_cpr;
	while(Encoder.ecd < 0) Encoder.ecd += Encoder.Encoder_cpr;
	Encoder.count_in_cpr = Encoder.ecd;
	
	//增量计数
	int delta_count = Encoder.count_in_cpr - Encoder.count_in_cpr_prev;
	Encoder.count_in_cpr_prev = Encoder.count_in_cpr;
	while(delta_count > +(Encoder.Encoder_cpr>>1)) delta_count -= Encoder.Encoder_cpr;
	while(delta_count < -(Encoder.Encoder_cpr>>1)) delta_count += Encoder.Encoder_cpr;
	
	Encoder.shadow_count += delta_count;
	
	/* Run vel PLL */
  Encoder.pos_cpr_counts += CURRENT_MEASURE_PERIOD * Encoder.vel_estimate_counts;
	float delta_pos_cpr_counts = (float)(Encoder.count_in_cpr - (int)Encoder.pos_cpr_counts);
	while(delta_pos_cpr_counts > +(Encoder.Encoder_cpr>>1)) delta_pos_cpr_counts -= Encoder.Encoder_cpr;
	while(delta_pos_cpr_counts < -(Encoder.Encoder_cpr>>1)) delta_pos_cpr_counts += Encoder.Encoder_cpr;
	Encoder.pos_cpr_counts += CURRENT_MEASURE_PERIOD * Encoder.pll_kp * delta_pos_cpr_counts;
	while(Encoder.pos_cpr_counts > Encoder.Encoder_cpr) Encoder.pos_cpr_counts -= Encoder.Encoder_cpr;
	while(Encoder.pos_cpr_counts < 0          ) Encoder.pos_cpr_counts += Encoder.Encoder_cpr;
	Encoder.vel_estimate_counts += CURRENT_MEASURE_PERIOD * Encoder.pll_ki * delta_pos_cpr_counts;
	
	// align delta-sigma on zero to prevent jitter
	bool snap_to_zero_vel = false;
	if (ABS(Encoder.vel_estimate_counts) < Encoder.snap_threshold) {
			Encoder.vel_estimate_counts = 0.0f;
			snap_to_zero_vel = true;
	}
	
	UTILS_LP_MOVING_AVG_APPROX(Encoder.vel, (Encoder.vel_estimate_counts / ((float)(Encoder.Encoder_cpr))), 5);
	Encoder.phase = (Encoder.ecd * M_2PI * UsrConfig.motor_pole_pairs) / (float)Encoder.Encoder_cpr;	//电角度=机械角度*极对数
	Encoder.phase_vel = Encoder.vel * M_2PI * UsrConfig.motor_pole_pairs;
}
