/*
 * PR.c
 *
 *  Created on: Jan 26, 2024
 *      Author: SSQ
 */
#include <PR.h>

PR firstPr;
//Initialization
void PR_init(PR * pr,float kp, float kr, float f0, float fc, float Ts)
{
	pr->kp = kp;
	pr->kr = kr;
	pr->f0 = f0;
	pr->fc = fc;
	pr->Ts = Ts;

	float wc = 2 * PI * fc;
	float w0 = 2 * PI * f0;
	//Calculate The Coefficient Of Difference Equation

	pr->A0 = 4/Ts/Ts+4*wc/Ts+w0*w0;
	pr->A1 = 2*w0*w0-8/Ts/Ts;
	pr->A2 = 4/Ts/Ts-4*wc/Ts+w0*w0;

	pr->B0 = 4*kp/Ts/Ts+4*wc*(kp+kr)/Ts+kp*w0*w0;
	pr->B1 = 2*kp*w0*w0-8*kp/Ts/Ts;
	pr->B2 = 4*kp/Ts/Ts-4*wc*(kp+kr)/Ts+kp*w0*w0;

	//Initialize The Coefficient Of Input And Output
	pr->xn = 0.0;
	pr->xn_1 = 0.0;
	pr->xn_2 = 0.0;

	pr->yn = 0.0;
	pr->yn_1 = 0.0;
	pr->yn_2 = 0.0;
}

//Calculate The PR Controller
void PR_calc(PR *pr)
{
	pr->yn = (pr->xn * pr->B0 + pr->xn_1 * pr->B1 + pr->xn_2 * pr->B2 - pr->yn_1 * pr->A1 - pr->yn_2 * pr->A2)/pr->A0;

	//Update
	pr->yn_2 = pr->yn_1;
	pr->yn_1 = pr->yn;

	pr->xn_2 = pr->xn_1;
	pr->xn_1 = pr->xn;
}




