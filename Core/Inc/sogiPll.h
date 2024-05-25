/*
 * sogiPll.h
 *
 *  Created on: Jan 27, 2024
 *      Author: SSQ
 */

#ifndef INC_SOGIPLL_H_
#define INC_SOGIPLL_H_

#include <common.h>
//! \brief  Defines the SPLL_1PH_SOGI_OSG_COEFF structure
//!
typedef struct{
	float osg_k;
	float osg_x;
	float osg_y;
	float osg_b0;
	float osg_b2;
	float osg_a1;
	float osg_a2;
	float osg_qb0;
	float osg_qb1;
	float osg_qb2;
} sogiPllOsgCofficient;

//! \brief  Defines the SPLL_1PH_SOGI_LPF_COEFF structure
//!
typedef struct{
    float32_t b1;
    float32_t b0;
}sogiLpfCofficient;

//Define the SOGI_PLL structure
typedef struct{
	float	u[3];		//acInput
	float	alpha[3];	//the SOGI Output Of Orthogonal alpha
	float	beta[3];	//the SOGI Output Of Orthogonal beta
	float	u_D[2];		//the D-axis Of Park Transfer
	float	u_Q[2];		//the Q-axis Of Park Transfer
	float   ylf[2];     //the Loop filter data storage
	float	fo;			//the Output Of Frequency(Hz)
	float	fn;			//the Nominal Frequency
	float	theta;		//the Output Of Angle (0-2pi)
	float	cosine;		//the Output Cosine Of PLL
	float	sine;		//the Output Sine Of PLL
	float	sogi_n;
	float	Ti;
	sogiPllOsgCofficient osgCofficient;
	sogiLpfCofficient LpfCofficient;
}sogiPll;


//初始化参数
void sogiPll_init(sogiPll *);
void sogiConfig(sogiPll *pll,
                         float32_t acFreq,
                         float32_t isrFrequency,
                         float32_t lpf_b0,
                         float32_t lpf_b1);
void sogiPllOsgCofficientCalc(sogiPll *pll);
//计算一次锁相环路
void sogiPllLoop_calc(float vin, sogiPll *);

extern sogiPll gridPll;

#endif /* INC_SOGIPLL_H_ */

