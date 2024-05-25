/*
 * lowPassFliter.h
 *
 *  Created on: Apr 28, 2024
 *      Author: SSQ
 */

#ifndef INC_LOWPASSFILTER_H_
#define INC_LOWPASSFILTER_H_

typedef struct
{
	float x[2];
	float y[2];
	float Tsample;
	float tao;
	float fc;
}lowPassFilter;
//Difference equation cofficient is calculated by the function of Init
typedef struct
{
	float alpha;
}lowPassFilterCofficient;

extern lowPassFilter lowPass1Hz;
extern lowPassFilter lowPass100Hz;
extern lowPassFilterCofficient lowPass1HzCofficient;
extern lowPassFilterCofficient lowPass100HzCofficient;

void lowPassFilterInit(lowPassFilter *filter,
		lowPassFilterCofficient *filterCofficient,
		float bandWidth,
		float Ts);
float calcLowPassFilter(lowPassFilter *filter,
		lowPassFilterCofficient *filterCofficient,
		float u);



#endif /* INC_LOWPASSFILTER_H_ */
