#include "fsm_task.h"
#include "encoder.h"
#include "pwm_curr.h"
#include "controller.h"
#include "calibration.h"
#include "anticogging.h"
#include "user_config.h"
#include "util.h"
#include "foc.h"
#include "usart.h"

typedef struct sFSM{
    tFSMState state;
    tFSMState state_next;
    uint8_t state_next_ready;
} tFSM;

static volatile tFSM mFSM;

volatile tFSMStatusword StatuswordNew;
volatile tFSMStatusword StatuswordOld;;

#define CHARGE_BOOT_CAP_MS        10
#define CHARGE_BOOT_CAP_TICKS    (uint16_t)((PWM_FREQUENCY * CHARGE_BOOT_CAP_MS)/ 1000)
static uint16_t mChargeBootCapDelay = 0;


void FSM_init(void){
    mFSM.state = IDLE;
    mFSM.state_next = IDLE;
    mFSM.state_next_ready = 0;
    
    StatuswordNew.status.status_code = 0;
    StatuswordNew.errors.errors_code = 0;
    StatuswordOld = StatuswordNew;
}

/**
  * @brief  FSM获取状态
  * @retval 无
  */
tFSMState FSM_get_state(void){
    return mFSM.state;
}

/**
  * @brief  设定FSM状态
  * @retval 0 Success
	*					-1 Invalid
  *         -2 Error code
	*					-3 Calib invalid
  */
int FSM_set_state(tFSMState state)
{
    int ret = 0;

    switch(mFSM.state){
        case BOOT_UP:
            if(state == IDLE){
                mFSM.state_next = IDLE;
            }else{
                ret = -1;
            }
            break;
        
        case IDLE:
            switch (state) {
                case IDLE:
                    FOC_disarm();
                    mChargeBootCapDelay = 0;
                    mFSM.state_next = IDLE;
                    break;
                
                case RUN:
                    if(StatuswordNew.errors.errors_code){
                        ret = -2;
                    }else if(!UsrConfig.calib_valid){
                        ret = -3;
                    }else{
                        FOC_arm();
                        mChargeBootCapDelay = CHARGE_BOOT_CAP_TICKS;
                        mFSM.state_next = RUN;
                    }
                    break;
                    
                case CALIBRATION:
                    if(StatuswordNew.errors.errors_code){
                        ret = -2;
                    }else{
                        FOC_arm();
                        mChargeBootCapDelay = CHARGE_BOOT_CAP_TICKS;
                        mFSM.state_next = CALIBRATION;
                    }
                    break;
                    
                case ANTICOGGING:
                    if(StatuswordNew.errors.errors_code){
                        ret = -2;
                    }else if(!UsrConfig.calib_valid){
                        ret = -3;
                    }else{
                        FOC_arm();
                        mChargeBootCapDelay = CHARGE_BOOT_CAP_TICKS;
                        mFSM.state_next = ANTICOGGING;
                    }
                    break;

                default:
                    ret = -1;
                    break;
            }
            break;

        default:
            if(state == IDLE){
                mFSM.state_next = IDLE;
            }else{
                ret = -1;
            }
            break;
    }

    mFSM.state_next_ready = 0;

    return ret;
}

static void enter_state(void)
{
    switch(mFSM.state){
        case BOOT_UP:
            break;
        
        case IDLE:
            break;
        
        case RUN:
            CONTROLLER_reset();
            StatuswordNew.status.switched_on = 1;
            StatuswordNew.status.target_reached = 1;
            StatuswordNew.status.current_limit_active = 0;
            StatuswordOld.status = StatuswordNew.status;
            break;
        
        case CALIBRATION:
            CALIBRATION_start();
            break;
        
        case ANTICOGGING:
            CONTROLLER_reset();
            ANTICOGGING_start();
            break;
        
        default:
            break;
    }
}

static void exit_state(void)
{
    switch(mFSM.state){
        case BOOT_UP:
            //CAN_reset_rx_timeout();
            //CAN_reset_tx_timeout();
            mFSM.state_next_ready = 1;
            break;
        
        case IDLE:
            if(mChargeBootCapDelay){
                mChargeBootCapDelay --;
            }else{
                mFSM.state_next_ready = 1;
            }
            break;
        
        case RUN:
            FOC_disarm();
            StatuswordNew.status.switched_on = 0;
            StatuswordNew.status.target_reached = 0;
            StatuswordNew.status.current_limit_active = 0;
            StatuswordOld.status = StatuswordNew.status;
            mFSM.state_next_ready = 1;
            break;
        
        case CALIBRATION:
            CALIBRATION_end();
            mFSM.state_next_ready = 1;
            break;
        
        case ANTICOGGING:
            ANTICOGGING_end();
            mFSM.state_next_ready = 1;
            break;

        default:
            break;
    }
}
/**
  * @brief  FSM低频任务
  * @retval 无
  */
void FSM_low_priority_task(void){
	
}

/**
  * @brief  FSM高频任务
  * @retval 无
  */
void FSM_high_frequency_task(void){
  /*状态切换*/
  if(mFSM.state_next != mFSM.state){
    exit_state();
    if(mFSM.state_next_ready){
      mFSM.state = mFSM.state_next;
      enter_state();
    }
  }
	
	
	ENCODER_loop();
	
	/*三相电流采样*/
  Foc.v_bus = (float)(READ_Vbus_ADC())*V_SCALE;                          //Vbus采样
  UTILS_LP_FAST(Foc.v_bus_filt, Foc.v_bus, 0.05f);                       //滤波
  Foc.i_b   = (float)(READ_IPHASE_B_ADC()-phase_b_adc_offset)*I_SCALE;   //Ib采样
  Foc.i_c   = (float)(READ_IPHASE_C_ADC()-phase_c_adc_offset)*I_SCALE;   //Ic采样
  Foc.i_a   = -Foc.i_b-Foc.i_c;                                          //Ia计算
	
	/*状态机执行任务*/
  switch (mFSM.state)
  {
		case BOOT_UP:
    break;
		case CALIBRATION:
		{
			CALIBRATION_loop();
		}	
		break;
		case ANTICOGGING:
				ANTICOGGING_loop();
		case RUN:
		{
			CONTROLLER_loop();
			
			// check leak current
			if(ABS(Foc.i_a + Foc.i_b + Foc.i_c) > UsrConfig.protect_i_leak_max){
					FOC_disarm();
	//				MCT_set_state(IDLE);
	//				StatuswordNew.errors.over_current = 1;
			}

			// check motor current
			// If Ia + Ib + Ic == 0 holds then we have: Inorm^2 = Id^2 + Iq^2 = Ialpha^2 + Ibeta^2 = 2/3 * (Ia^2 + Ib^2 + Ic^2)
			float Inorm_sq = 2.0f / 3.0f * (SQ(Foc.i_a) + SQ(Foc.i_b) + SQ(Foc.i_c));
			if (Inorm_sq > SQ(UsrConfig.protect_over_current)) {
					FOC_disarm();
	//				MCT_set_state(IDLE);
	//				StatuswordNew.errors.over_current = 1;
			}

			// check I bus current
			if(Foc.i_bus > UsrConfig.protect_i_bus_max){
					FOC_disarm();
	//				MCT_set_state(IDLE);
	//				StatuswordNew.errors.over_current = 1;
			}
			
		}
		break;
	  default:
    break;
		
	}
	
	
	
	
}
//void FSM_high_frequency_task(void){
//  /*状态切换*/
//  if(mFSM.state_next != mFSM.state){
//    exit_state();
//    if(mFSM.state_next_ready){
//      mFSM.state = mFSM.state_next;
//      enter_state();
//    }
//  }

//  /*编码器任务*/
//  ENCODER_loop();
//  /*三相电流采样*/
//  Foc.v_bus = (float)(READ_Vbus_ADC())*V_SCALE;                          //Vbus采样
//  UTILS_LP_FAST(Foc.v_bus_filt, Foc.v_bus, 0.05f);                       //滤波
//  Foc.i_b   = (float)(READ_IPHASE_B_ADC()-phase_b_adc_offset)*I_SCALE;   //Ib采样
//  Foc.i_c   = (float)(READ_IPHASE_C_ADC()-phase_c_adc_offset)*I_SCALE;   //Ic采样
//  Foc.i_a   = -Foc.i_b-Foc.i_c;                                          //Ia计算
//  
//  /*状态机执行任务*/
//  switch (mFSM.state)
//  {
//    case BOOT_UP:
//    {
//			
//    }
//    break;
//    case CALIBRATION:
//    {

//    }
//    break;        
//    case ANTICOGGING:
//    {

//    }
//    break;
//    case RUN:
//    {
//      CONTROLLER_loop();
//            
//      // check leak current
//      if(ABS(Foc.i_a + Foc.i_b + Foc.i_c) > UsrConfig.protect_i_leak_max){
//          FOC_disarm();
//          FSM_set_state(IDLE);
//          StatuswordNew.errors.over_current = 1;
//      }
//  
//      // check motor current
//      // If Ia + Ib + Ic == 0 holds then we have: Inorm^2 = Id^2 + Iq^2 = Ialpha^2 + Ibeta^2 = 2/3 * (Ia^2 + Ib^2 + Ic^2)
//      float Inorm_sq = 2.0f / 3.0f * (SQ(Foc.i_a) + SQ(Foc.i_b) + SQ(Foc.i_c));
//      if (Inorm_sq > SQ(UsrConfig.protect_over_current)) {
//          FOC_disarm();
//          FSM_set_state(IDLE);
//          StatuswordNew.errors.over_current = 1;
//      }

//      // check I bus current
//      if(Foc.i_bus > UsrConfig.protect_i_bus_max){
//          FOC_disarm();
//          FSM_set_state(IDLE);
//          StatuswordNew.errors.over_current = 1;
//      }
//    }
//    break;
//  default:
//    break;
//  }
//}
