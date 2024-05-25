/*
 * PID.h
 *
 *  Created on: Jan 28, 2024
 *      Author: SSQ
 */

#ifndef INC_PID_H_
#define INC_PID_H_

//Define The Coefficient Of PID
typedef struct{
	float	kp;		//比例系数
	float	ki;		//积分系数
	float	kd;		//微分系数
	float	i_max;	//积分器限幅最大值
	float	i_min;	//积分器限幅最小值
	float	i_sum;	//积分器累加值
	float	out;
}PID;

//PID参数初始化函数
void PID_init(float kp, float ki, float kd, float i_max, float i_min,PID* pid);

//比例控制
void P_transfer(float error,PID* pid);

//比例积分控制
void PI_transfer(float error,PID* pid);

////比例积分微分控制
//float PID_transfer(float error,PID* pid);

//Create A Current Sharing Loop PID Structure
extern PID cslPid;
extern PID mpptPid;

#endif /* INC_PID_H_ */




