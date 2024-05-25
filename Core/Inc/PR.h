/*
 * PR.h
 *
 *  Created on: Jan 28, 2024
 *      Author: SSQ
 */

#ifndef INC_PR_H_
#define INC_PR_H_

#include <arm_math.h>

//Define The Coefficient Of PR
//Transfer Function
//					2*kr*wc*s
//PR(s) = kp + -----------------
//				s^2 + 2*wc*s + w0^2
//Difference Equation
//A0*y[n] + A1*y[n-1] + A2*y[n-2] = B0*x[n] + B1*x[n-1] + B2*x[n-2]
typedef struct{
	float kp;	//The Coefficient Of Proportion
	float kr;	//The Coefficient Of Resonance
	float f0;	//Center Frequency
	float fc;	//Bandwidth
	float Ts;	//Sample Time
	float A0,A1,A2;		//The Coefficient Of den
	float B0,B1,B2;		//The Coefficient Of num
	float yn,yn_1,yn_2;	//Output
	float xn,xn_1,xn_2;	//Input
}PR;

//Initialization
void PR_init(PR *,float kp, float kr, float f0, float fc, float Ts);

//Calculate The PR Controller
void PR_calc(PR *);

extern PR firstPr;

#endif /* INC_PR_H_ */
