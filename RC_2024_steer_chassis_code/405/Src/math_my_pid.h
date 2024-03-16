#ifndef  __MATH_MY_PID_H
#define  __MATH_MY_PID_H
#ifdef  __cplusplus
extern "C"{
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint32_t (*SystemTick_Fun)(void);

enum PID_MODE
{
    PID_MODE_POS = 0,	
    PID_MODE_DEL,		
};

typedef struct myPidTypeDef_s
{
		uint8_t mode;
    //PID 三参数
    float Kp;
    float Ki;
    float Kd;
	
    float max_out;  //输出限幅
    float max_iout; //积分输出限幅

		float set;
    float fdb[3];		//测量值

    float out;
    float Pout;
    float Iout;
    float Dout;
    float Dbuf[2];  //微分项 0现在 1过去
    float error[3]; //误差项 0现在 1过去 2以前
		
		float DeadZone;	//死区，需为整数
		
		bool D_of_Current;	//是否开启微分先行
		bool I_of_Change;		//是否开启变速积分
		bool discretize;		//是否开启自行整定采样时间
		float I_A;					//变速积分A
		float I_B;					//变速积分B
		
} myPidTypeDef_t;

void myPID_Clear(myPidTypeDef_t *pid);
uint8_t getMicroTick_regist(uint32_t (*getTick_fun)(void));
void Init_myPID(myPidTypeDef_t *pid, uint8_t mode, const float PID[6]);
float myPID_Calc(myPidTypeDef_t *pid,float ref,float set);

#ifdef  __cplusplus
}
#endif
#endif 

