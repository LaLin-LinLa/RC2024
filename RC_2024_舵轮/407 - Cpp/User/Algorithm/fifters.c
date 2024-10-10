#include "fifters.h"
#include "util.h"

//void	LP_Filter_Init(LP_Filter_t*filter, )
//{
//	
//}

float LP_Filter(LP_Filter_t* filter, float data_new, float data_last)
{
	filter->flag[1] = filter->flag[0];
	
	if((data_new	-	data_last)>0)
		filter->flag[0]=1;		//flag=1表示采样值增加
	else if((data_new	-	data_last)<0)
		filter->flag[0]=0;		//flag=0表示采样值减少
	
	
	if(filter->flag[0] == filter->flag[1])		//变化方向一致
	{
		filter->cnt++;
		if(_ABS(data_new-data_last)>filter->threshold_data)
			filter->cnt+=5;
		
		if(filter->cnt >filter->threshold_cnt)
		{
			filter->k = filter->k_init + filter->k_gain;
			filter->cnt = 0;
		}
	}
	else		//采样值变化稳定
	{
		filter->cnt = 0;
		filter->k = filter->k_init;
	}
	data_last=(1-filter->k)*data_last+filter->k*data_new;
	return data_last;
}

