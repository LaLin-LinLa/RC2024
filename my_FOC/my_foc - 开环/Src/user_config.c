#include "user_config.h"
#include "controller.h"

tUsrConfig UsrConfig;

void USR_CONFIG_set_default_config(void)
{
    // Motor
    UsrConfig.invert_motor_dir = 0;
    UsrConfig.inertia = 0.0001f;
    UsrConfig.torque_constant = 0.0612f;
    UsrConfig.motor_pole_pairs = 7;
//    UsrConfig.motor_phase_resistance = 0.59f;
//    UsrConfig.motor_phase_inductance = 0.000320f;
		UsrConfig.motor_phase_resistance = 0.00f;
    UsrConfig.motor_phase_inductance = 0.000f;
    UsrConfig.current_limit = 10;
    UsrConfig.velocity_limit = 60; 
    
    // Anticogging
    UsrConfig.anticogging_enable = 1;
    
    // Controller
    UsrConfig.control_mode = CONTROL_MODE_VELOCITY_RAMP;
    UsrConfig.pos_gain = 160.0f;
    UsrConfig.vel_gain = 0.05f;
    UsrConfig.vel_integrator_gain = 0.02f;
    UsrConfig.current_ctrl_bw = 1000;
    UsrConfig.sync_target_enable = 0;
    UsrConfig.target_velcity_window = 0.5f;
    UsrConfig.target_position_window = 0.01f;
    UsrConfig.torque_ramp_rate = 0.1f;
    UsrConfig.velocity_ramp_rate = 100.0f;
    UsrConfig.position_filter_bw = 2;
    UsrConfig.profile_velocity = 30;
    UsrConfig.profile_accel = 50;
    UsrConfig.profile_decel = 50;

    // Protect
    UsrConfig.protect_under_voltage = 8.0;
    UsrConfig.protect_over_voltage  = 28;
    UsrConfig.protect_over_current = 20;
    UsrConfig.protect_i_bus_max = 5;
    UsrConfig.protect_i_leak_max = 10.0f;
    
    // CAN
//    UsrConfig.node_id = 1;
//    UsrConfig.can_baudrate = CAN_BAUDRATE_500K;
//    UsrConfig.heartbeat_consumer_ms = 0;
//    UsrConfig.heartbeat_producer_ms = 0;
    
    // Encoder
    UsrConfig.calib_valid = 0;
		UsrConfig.calib_current = 8.0f;
    UsrConfig.calib_voltage = 3.0f;
		UsrConfig.encoder_offset = 16268;
}

