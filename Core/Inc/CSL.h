/*
 * CSL.h
 * Current Sharing Loop
 *  Created on: Jan 31, 2024
 *      Author: SSQ
 */

#ifndef INC_CSL_H_
#define INC_CSL_H_

typedef struct
{
	//两路反激误差
	float flybackError;
	//PI输入误差
	float error;
	//变化占空比
	float deltaDuty;
}cslCofficient;

//Declaration
void CSL_init(cslCofficient *formalCSL, float kp, float ki, float kd, float i_max, float i_min);
float CSL_calc(cslCofficient *formalCSL, float flyback_i1, float flyback_i2);

extern cslCofficient mainCSL;

#endif /* INC_CSL_H_ */
