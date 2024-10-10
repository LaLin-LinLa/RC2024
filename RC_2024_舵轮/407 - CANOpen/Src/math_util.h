#ifndef __MATH_UTIL_H
#define __MATH_UTIL_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#ifndef _PI
#define	_PI	3.14159265
#endif
#define _ABS(x)         ( (x)>0?(x):-(x) )
#define SQ(x)        ((x)*(x))
#define _LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

float Ramp_Cale( float target, float target_last, float Inc);
int16_t int16_buffer(int16_t Value, int16_t buffer);
float float_map(float a, float amin, float amax, float bmin, float bmax);
int int_map(int a, int amin, int amax, int bmin, int bmax);
		
#ifdef __cplusplus
}
#endif
#endif
