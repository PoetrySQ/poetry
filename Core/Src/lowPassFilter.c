/*
 * lowPassFliter.c
 *
 *  Created on: Apr 28, 2024
 *      Author: SSQ
 */


#include <lowPassFilter.h>

#include <common.h>
#include <config.h>

lowPassFilter lowPass100Hz;
lowPassFilter lowPass1Hz;
lowPassFilterCofficient lowPass100HzCofficient;
lowPassFilterCofficient lowPass1HzCofficient;


//Input[0]:滤波器结构
//Input[1]:参数结构
//Input[2]:带宽（Hz）
//Input[3]:采样时间（s）
void lowPassFilterInit(lowPassFilter *filter,
		lowPassFilterCofficient *filterCofficient,
		float bandWidth,
		float Ts)
{
	filter->x[0] = 0.0;
	filter->x[1] = 0.0;
	filter->y[0] = 0.0;
	filter->y[1] = 0.0;

	filter->fc = bandWidth;
	filter->tao = 1/(PI_2*filter->fc);
	filter->Tsample = Ts;

	filterCofficient->alpha = filter->Tsample / (filter->Tsample + filter->tao);
}


//计算低通滤波器
float calcLowPassFilter(lowPassFilter *filter,
		lowPassFilterCofficient *filterCofficient,
		float u)
{
	filter->x[0] = u;
	filter->y[0] = filterCofficient->alpha * filter->x[0] + filter->y[1] * (1 - filterCofficient->alpha);

	filter->x[1] = filter->x[0];
	filter->y[1] = filter->y[0];

	return filter->y[0];
}
