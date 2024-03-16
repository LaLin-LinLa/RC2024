#include "usr_config.h"

tUsrConfig UsrConfig = {0};

void USR_CONFIG_set_default_config(void)
{
	//Motor
	UsrConfig.motor_pole_pairs = 7;
	UsrConfig.motor_phase_resistance = 0.59f;
	UsrConfig.motor_phase_inductance = 0.000320f;
	//Control
	UsrConfig.current_ctrl_bw = 1000;
	// Encoder
	UsrConfig.calib_current = 8.0f;
	UsrConfig.calib_voltage = 3.0f;
	//Calibration
	UsrConfig.calib_current = 0.8f;
  UsrConfig.calib_voltage = 3.0f;
}
