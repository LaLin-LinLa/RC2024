#ifndef _FSM_TASK_H_
#define _FSM_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

// Motor controler state
typedef enum eFSMState {
    BOOT_UP            = 0,
    IDLE            = 1,
    RUN                = 2,
    CALIBRATION        = 3,
    ANTICOGGING     = 4,
} tFSMState;

typedef struct sFSMStatusword {
    union {
        uint32_t status_code;
        struct {
            uint32_t switched_on            : 1;
            uint32_t target_reached         : 1;
            uint32_t current_limit_active    : 1;
            uint32_t PADDING                : 29;
        };
    } status;
    
    union {
        uint32_t errors_code;
        struct {
            // FATAL
            uint32_t adc_selftest_fatal    : 1;    // 1<<0
            uint32_t encoder_offline    : 1;    // 1<<1
            uint32_t PADDING_1            : 15;
            // ERROR
            uint32_t over_voltage        : 1;    // 1<<16
            uint32_t under_voltage        : 1;    // 1<<17
            uint32_t over_current        : 1;    // 1<<18
            uint32_t PADDING_2            : 13;
        };
    } errors;
} tFSMStatusword;

extern volatile tFSMStatusword StatuswordNew;
extern volatile tFSMStatusword StatuswordOld;


tFSMState FSM_get_state(void);
void FSM_init(void);
int FSM_set_state(tFSMState state);
void FSM_high_frequency_task(void);

#ifdef __cplusplus
}
#endif
#endif
