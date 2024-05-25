/*
 * CSL.c
 *
 *  Created on: Apr 10, 2024
 *      Author: SSQ
 */

#include <CSL.h>
#include <PID.h>

cslCofficient mainCSL;

//Current Sharing Loop Initialization
void CSL_init(cslCofficient *formalCSL, float kp, float ki, float kd, float i_max, float i_min)
{
	formalCSL->deltaDuty = 0.0;
	formalCSL->error = 0.0;
	formalCSL->flybackError = 0.0;
	PID_init(kp, ki, kd, i_max, i_min, &cslPid);
}

//Calculate Current Sharing Loop
//@return deltaDuty
float CSL_calc(cslCofficient *formalCSL, float flyback_i1, float flyback_i2)
{
	formalCSL->flybackError = flyback_i1 - flyback_i2;
	formalCSL->error = 0 - formalCSL->flybackError;
	//需要对deltaDuty限幅，限制单周期均流调节占空比的最大步长
	PI_transfer(formalCSL->error, &cslPid);
	formalCSL->deltaDuty = cslPid.out;

	//限幅
	if(formalCSL->deltaDuty >= 0.1){
		formalCSL->deltaDuty = 0.1;
	}
	else if (formalCSL->deltaDuty <= -0.1) {
		formalCSL->deltaDuty = -0.1;
	}
	return formalCSL->deltaDuty;
}
