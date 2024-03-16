#include "foc.h"
#include "pwm_cur.h"
#include "usr_config.h"

float voltage_limit=12.0f;

FOC_Control_t	foc = {0
};

/**
  * @brief  foc��ʼ��
  * @retval ��
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
  * @brief  foc����PI����
  * @retval ��
  */
void FOC_update_current_ctrl_gain(float bw)
{
    float bandwidth = MIN(bw, 0.25f * PWM_FREQUENCY);
    foc.I_Kp = UsrConfig.motor_phase_inductance * bandwidth;
    foc.I_Ki = UsrConfig.motor_phase_resistance * bandwidth;
}


/**
  * @brief  foc��������
  * @retval ��
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
  * @brief  foc��ѹ����
  * @retval ��
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
  * @brief  foc�����ջ�
  * @retval ��
  */
void FOC_current(float Id_set, float Iq_set, float phase, float phase_vel)
{
		// Clarke�任�����������������abc��Ϊ����i_alpha��i_beta(�ȷ�ֵ�任)
    float i_alpha, i_beta;
    clarke_transform(foc.i_a, foc.i_b, foc.i_c, &i_alpha, &i_beta);
    
		// Park�任�����ݲ�õĵ�Ƕ�ֵphase,���̶���alpha��beta����ת��Ϊת����DQ����
    park_transform(i_alpha, i_beta, phase, &foc.i_d, &foc.i_q);
    
    float mod_to_V = foc.v_bus_filt * (2.0f / 3.0f);		//������ѹΪĸ�ߵ�ѹV_bus��2/3
    float V_to_mod = 1.0f / mod_to_V;
    
    // Ӧ��PI����
    float Ierr_d = Id_set - foc.i_d;	//����i_d���
    float Ierr_q = Iq_set - foc.i_q;	//����i_q���	
		//Kp����ת����ѹ
    float mod_d = V_to_mod * (foc.I_d_integral + Ierr_d * foc.I_Kp);	
    float mod_q = V_to_mod * (foc.I_q_integral + Ierr_q * foc.I_Kp);
    // �����ֱ��ʹ���
    float mod_scalefactor = 0.80f * SQRT3_BY_2 / sqrtf(SQ(mod_d) + SQ(mod_q));	
    if (mod_scalefactor < 1.0f) {
        mod_d *= mod_scalefactor;
        mod_q *= mod_scalefactor;
        foc.I_d_integral *= 0.99f;
        foc.I_d_integral *= 0.99f;
    } else {
        foc.I_q_integral += Ierr_d * (foc.I_Ki * CURRENT_MEASURE_PERIOD);		//�����ۼ�
        foc.I_q_integral += Ierr_q * (foc.I_Ki * CURRENT_MEASURE_PERIOD);
    }

		// ��Park�任�����˶���DQ����ת��Ϊ��ֹ��alpha��beta����
    float mod_alpha, mod_beta;
    float pwm_phase = phase + 1.5f*phase_vel * CURRENT_MEASURE_PERIOD;	//ֵ��ע����ǸýǶȲ���Park�任ʱ�ĵ�Ƕ�I_phase,���ǹ���ĸ��½Ƕȣ��ڽ��з�Parkʱ��λ�����1.5������
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




