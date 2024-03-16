#include "drv_joystick.h"
#include "bsp_adc.h"

#define EORR			5
#define DP		7

joystick_ctrl_t	joystick;
uint16_t Temp[2];

int int_map(int a, int amin, int amax, int bmin, int bmax)
{
	int adel = amax - amin, bdel = bmax - bmin;
	if(a > amax)a = amax;
	if(a < amin)a = amin;
	return (bdel * ((float)(a-amin) / adel))+bmin;
}

int16_t int16_buffer(int16_t Value, int16_t buffer)
{
  if(Value > 0) 
	{
		Value -= buffer;
		if(Value < 0) Value=0;
	}
	else if(Value < 0)
	{
		Value += buffer;
		if(Value > 0) Value=0;
	}
	return Value;
}

/**
	* @brief  Ä£ÄâÒ¡¸Ë³õÊ¼»¯
	* @param  None
	* @retval None
	*/
void drv_joystick_Init(void)
{
	bsp_adc_Init(Temp, 2);
}

void joystick_DatePrcess(void)
{
	float X_ave = 0.0f, Y_ave = 0.0f;
	int X_buf[DP], Y_buf[DP];
	int X_Max = 0, X_Min = 0;
	int Y_Max = 0, Y_Min = 0;
	
	for(char i=0;i<DP;i++)
	{	
		X_buf[i] = Temp[0];
		Y_buf[i] = Temp[1];
		
		if(i == 0)
		{
			X_Max = X_Min = X_buf[0];
			Y_Max = Y_Min = Y_buf[0];
		}
		else if(i>0)
		{
			if((X_buf[i] - X_buf[i-1]) > EORR)
			{
				X_buf[i] = X_buf[i-1];
			}
			if((Y_buf[i] - Y_buf[i-1]) > EORR)
			{
				Y_buf[i] = Y_buf[i-1];
			}
		}
		if(X_buf[i] > X_Max)	X_Max = X_buf[i];
		if(X_buf[i] < X_Min)	X_Min = X_buf[i];
		if(Y_buf[i] > Y_Max)	Y_Max = Y_buf[i];
		if(Y_buf[i] < Y_Min)	Y_Min = Y_buf[i];
		
		X_ave += X_buf[i];
		Y_ave += Y_buf[i];
	}
	
//	joystick.X_value = (X_ave - X_Max - X_Min)/(DP-2);
//	joystick.Y_value = (Y_ave - Y_Max - Y_Min)/(DP-2);
	joystick.X_value = int16_buffer(int_map((int)((X_ave - X_Max - X_Min)/(DP-2)), 0, 4090, -684, 684), 24);
	joystick.Y_value = int16_buffer(int_map((int)((Y_ave - Y_Max - Y_Min)/(DP-2)), 0, 4090, -680, 680), 20);
	
}

