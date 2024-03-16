#ifndef _FSM_TASK_H_
#define _FSM_TASK_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct FSM_control_s{
	enum{
		BOOT_UP = 0,
		CALIBRATION,
		IDLE,
		TEST,
		RUN,
	}FSM_state;
	
} FSM_control_t;

void FSM_high_fre_task(void);
void FSM_low_fre_task(void);

extern FSM_control_t	fsm;

#ifdef __cplusplus
}
#endif
#endif
