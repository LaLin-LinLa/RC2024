#include "foc.h"
#include "pwm_cur.h"
#include "usr_config.h"

float voltage_limit=12.0f;

FOC_Control_t	foc = {0
};

/**
  * @brief  foc初始化
  * @retval 无
  */
void FOC_Init(void)
{
	foc.i_a = foc.i_b = foc.i_c = 0.0f;
	foc.v_bus = foc.v_bus_filt = 0.0f;
	foc.i_q = foc.I_q_integral = 0.0f;
	foc.i_d = foc.I_d_integral = 0.0f;
	FOC_update_current_ctrl_gain(UsrConfig.current_ctrl_bw);
}
/**
  * @brief  foc更新PI增益
  * @retval 无
  */
void FOC_update_current_ctrl_gain(float bw)
{
    float bandwidth = MIN(bw, 0.25f * PWM_FREQUENCY);
    foc.I_Kp = UsrConfig.motor_phase_inductance * bandwidth;
    foc.I_Ki = UsrConfig.motor_phase_resistance * bandwidth;
}


/**
  * @brief  foc电流开环
  * @retval 无
  */
void FOC_current_open(float id_set, float iq_set, float phase)
{
	// Inverse park transform
	float mod_alpha, mod_beta;
	inverse_park(id_set, iq_set, phase, &mod_alpha, &mod_beta);
	
	// SVM
	if(0 == svm(mod_alpha, mod_beta, &foc.dtc_a, &foc.dtc_b, &foc.dtc_c)){
			set_a_duty((uint16_t)(foc.dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
			set_b_duty((uint16_t)(foc.dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
			set_c_duty((uint16_t)(foc.dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
	}
}

/**
  * @brief  foc电压开环
  * @retval 无
  */
void FOC_voltage(float Vd_set, float Vq_set, float phase)
{
    // Modulation
    float V_to_mod = 1.0f / (foc.v_bus_filt * (2.0f / 3.0f));
    float mod_d = V_to_mod * Vd_set;
    float mod_q = V_to_mod * Vq_set;
    
    // Vector modulation saturation, lock integrator if saturated
    float mod_scalefactor = 0.80f * SQRT3_BY_2 / sqrtf(SQ(mod_d) + SQ(mod_q));
    if (mod_scalefactor < 1.0f) {
        mod_d *= mod_scalefactor;
        mod_q *= mod_scalefactor;
    }
    
    // Inverse park transform
    float mod_alpha;
    float mod_beta;
    inverse_park(mod_d, mod_q, phase, &mod_alpha, &mod_beta);

    // SVM
    if(0 == svm(mod_alpha, mod_beta, &foc.dtc_a, &foc.dtc_b, &foc.dtc_c)){
        set_a_duty((uint16_t)(foc.dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
        set_b_duty((uint16_t)(foc.dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
        set_c_duty((uint16_t)(foc.dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
    }
}

/**
  * @brief  foc电流闭环
  * @retval 无
  */
void FOC_current(float Id_set, float Iq_set, float phase, float phase_vel)
{
		// Clarke变换，将测量的三相电流abc变为两相i_alpha和i_beta(等幅值变换)
    float i_alpha, i_beta;
    clarke_transform(foc.i_a, foc.i_b, foc.i_c, &i_alpha, &i_beta);
    
		// Park变换，根据测得的电角度值phase,将固定的alpha和beta坐标转变为转动的DQ坐标
    park_transform(i_alpha, i_beta, phase, &foc.i_d, &foc.i_q);
    
    float mod_to_V = foc.v_bus_filt * (2.0f / 3.0f);		//最大相电压为母线电压V_bus的2/3
    float V_to_mod = 1.0f / mod_to_V;
    
    // 应用PI控制
    float Ierr_d = Id_set - foc.i_d;	//计算i_d误差
    float Ierr_q = Iq_set - foc.i_q;	//计算i_q误差	
		//Kp控制转换电压
    float mod_d = V_to_mod * (foc.I_d_integral + Ierr_d * foc.I_Kp);	
    float mod_q = V_to_mod * (foc.I_q_integral + Ierr_q * foc.I_Kp);
    // 抗积分饱和处理
    float mod_scalefactor = 0.80f * SQRT3_BY_2 / sqrtf(SQ(mod_d) + SQ(mod_q));	
    if (mod_scalefactor < 1.0f) {
        mod_d *= mod_scalefactor;
        mod_q *= mod_scalefactor;
        foc.I_d_integral *= 0.99f;
        foc.I_d_integral *= 0.99f;
    } else {
        foc.I_q_integral += Ierr_d * (foc.I_Ki * CURRENT_MEASURE_PERIOD);		//积分累加
        foc.I_q_integral += Ierr_q * (foc.I_Ki * CURRENT_MEASURE_PERIOD);
    }

		// 反Park变换，将运动的DQ坐标转化为静止的alpha和beta坐标
    float mod_alpha, mod_beta;
    float pwm_phase = phase + 1.5f*phase_vel * CURRENT_MEASURE_PERIOD;	//值得注意的是该角度不是Park变换时的电角度I_phase,而是估算的更新角度，在进行反Park时相位需更新1.5个周期
    inverse_park(mod_d, mod_q, pwm_phase, &mod_alpha, &mod_beta);
    
    // Used for report
//    foc.i_q = i_q;
//    UTILS_LP_FAST(foc.i_q_filt, foc.i_q, 0.01f);
//    foc.i_d = i_d;
//    UTILS_LP_FAST(foc.i_d_filt, foc.i_d, 0.01f);
//    foc.i_bus = (mod_d * i_d + mod_q * i_q);
//    UTILS_LP_FAST(foc.i_bus_filt, foc.i_bus, 0.01f);
//    foc.power_filt = foc.v_bus_filt * foc.i_bus_filt;

    // SVM
    if(0 == svm(mod_alpha, mod_beta, &foc.dtc_a, &foc.dtc_b, &foc.dtc_c)){
        set_a_duty((uint16_t)(foc.dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
        set_b_duty((uint16_t)(foc.dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
        set_c_duty((uint16_t)(foc.dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
    }
}




