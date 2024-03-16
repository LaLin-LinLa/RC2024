#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define OFFSET_LUT_NUM        128U

typedef struct sUsrConfig{
  // Motor
    int32_t invert_motor_dir;       // 0 False : 1 True
    float inertia;                  //转动惯量(电流/(转速/时间)) [A/(turn/s^2)](0~100)
    float torque_constant;          //扭矩常数(转矩/电流) [N*m/A](0~10)               
    int32_t motor_pole_pairs;       //电机极对数 [PP]	(2~30)
    float motor_phase_resistance;   //电机相电阻 [R]  (0~10)               
    float motor_phase_inductance;   //电机相电感 [H]  (0~10)              
    float current_limit;            //电流限制 [A]                  (0~45)
    float velocity_limit;           //转速限幅 [turn/s](0~100)             
    
    // Calibration
    float calib_current;              // [A]                  (0~45)
    float calib_voltage;              // [V]                  (0~50)
    
    // Controller
    int32_t control_mode;
    float pos_gain;                 //位置增益 (0~1000)                      
    float vel_gain;                 //                      (0~1000)
    float vel_integrator_gain;      //速度积分增益 (0~1000)                      
    float current_ctrl_bw;             //                      (2~5000)
    int32_t anticogging_enable;     // 0 False : 1 True
    int32_t sync_target_enable;        // 0 False : 1 True
    float target_velcity_window;    //目标转速窗口 [turn/s](0~100)             
    float target_position_window;    // [turn]               (0~100)
    float torque_ramp_rate;            // [Nm/s]               (0~100)
    float velocity_ramp_rate;       //转速爬升速率 [(turn/s)/s](0~1000)
    float position_filter_bw;       //                      (2~5000)
    float profile_velocity;            // [turn/s]             (0~100)
    float profile_accel;            // [(turn/s)/s]         (0~1000)
    float profile_decel;            // [(turn/s)/s]         (0~1000)
    
    // Protect
    float protect_under_voltage;    // [V]                  (0~50)
    float protect_over_voltage;        // [V]                  (0~50)
    float protect_over_current;        // [A]                  (0~45)
    float protect_i_bus_max;        // [A]                  (0~10)
    float protect_i_leak_max;       // [A]                  (0~50)
    
    // CAN
    int32_t node_id;                //                      (1~31)
    int32_t can_baudrate;           // REF: tCanBaudrate
    int32_t heartbeat_consumer_ms;  // rx heartbeat timeout in ms : 0 Disable       (0~600000)
    int32_t heartbeat_producer_ms;  // tx heartbeat interval in ms : 0 Disable      (0~600000)
    
    // Encoder
    int32_t calib_valid;                // (Auto)
    int32_t encoder_dir;                // (Auto)
    int32_t encoder_offset;                // (Auto)
    int32_t offset_lut[OFFSET_LUT_NUM];    // (Auto)

    uint32_t crc;
}tUsrConfig;

extern tUsrConfig UsrConfig;
void USR_CONFIG_set_default_config(void);

#ifdef __cplusplus
}
#endif
#endif
