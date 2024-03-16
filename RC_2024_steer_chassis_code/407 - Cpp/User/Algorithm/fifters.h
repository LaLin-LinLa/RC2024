#ifndef  __FIFTERS_H
#define  __FIFTERS_H
#ifdef  __cplusplus
extern "C"{
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

	/**
 * A simple low pass filter.
 *
 * @param value
 * The filtered value.
 *
 * @param sample
 * Next sample.
 *
 * @param filter_constant
 * Filter constant. Range 0.0 to 1.0, where 1.0 gives the unfiltered value.
 */
#define UTILS_LP_FAST(value, sample, filter_constant)    (value -= (filter_constant) * ((value) - (sample)))		
	
typedef struct LP_Filter_s{
	bool flag[2];	//数据变化方向: 1表示增加;	0表示减小
	float k;			//滤波系数
	float k_init;	//初始滤波系数(默认稳态系数)
	float k_gain;	//滤波系数增益
	uint8_t cnt;	//滤波器计数
	float threshold_data;		//采样数据增量阀值
	uint8_t threshold_cnt;	//滤波器计数阀值
} LP_Filter_t;

float LP_Filter(LP_Filter_t* filter, float data_new, float data_last);

#ifdef  __cplusplus
}
#endif
#endif 

