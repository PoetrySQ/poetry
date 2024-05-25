/*
 * PID.c
 *
 *  Created on: Jan 26, 2024
 *      Author: SSQ
 */


#include <PID.h>

PID cslPid;
PID mpptPid;

void PID_init(float kp, float ki, float kd, float i_max, float i_min,PID* pid)
{
	//初始化PID参数
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->i_max = i_max;
	pid->i_min = i_min;
	//初始化积分器
	pid->i_sum = 0;
	pid->out = 0;
}

//Proportion Control Machine
void P_transfer(float error,PID * pid)
{
	float m = pid->kp*error;
	pid->out = m > pid->i_max ? pid->i_max
		: m < pid->i_min ? pid->i_min
		:m;

}

//Proportion And Integral Control Machine
void PI_transfer(float error,PID* pid)
{
	pid->out = pid->kp * error + pid->i_sum;
	//积分器累加值
	pid->i_sum = pid->ki * error;
	//积分器限幅
	if(pid->i_sum > pid->i_max)
		pid->i_sum = pid->i_max;
	else if(pid->i_sum < pid->i_min)
		pid->i_sum = pid->i_min;
}
