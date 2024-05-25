/*
 * sogiPll.c
 *
 *  Created on: Jan 26, 2024
 *      Author: SSQ
 */


#include <sogiPll.h>
#include <config.h>
sogiPll	gridPll;

//SOGI-PLL And sogiPid Initialization
void sogiPll_init(sogiPll *pll)
{
	pll->u[0] = 0.0;
	pll->u[1] = 0.0;
	pll->u[2] = 0.0;

	//__disable_irq();
	pll->alpha[0] = 0.0;
	pll->alpha[1] = 0.0;
	pll->alpha[2] = 0.0;

	pll->beta[0] = 0.0;
	pll->beta[1] = 0.0;
	pll->beta[2] = 0.0;

	pll->u_D[0] = 0.0;
	pll->u_D[1] = 0.0;

	pll->u_Q[0] = 0.0;
	pll->u_Q[1] = 0.0;

	pll->fo = 0.0;

	pll->sine = 0.0;
	pll->cosine = 0.0;

	pll->theta = 0.0;
	//锁相频率fn = 50 Hz
	pll->fn = 0.0;
	pll->sogi_n = 0.0;

	pll->Ti = 1.0/10000;
}

void sogiConfig(sogiPll *pll,
                         float32_t acFreq,
                         float32_t isrFrequency,
                         float32_t lpf_b0,
                         float32_t lpf_b1)
{
    pll->fn=acFreq;
    pll->Ti=((1.0f)/isrFrequency);

    sogiPllOsgCofficientCalc(pll);

    pll->LpfCofficient.b0=lpf_b0;
    pll->LpfCofficient.b1=lpf_b1;
}

//! \brief Calculates the SPLL_1PH_SOGI coefficient
//! \param *pll The SPLL_1PH_SOGI structure
//! \return None
//!
void sogiPllOsgCofficientCalc(sogiPll *pll)
{
    float32_t osgx,osgy,temp, wn;
    wn= pll->fn *(float32_t) 2.0f * (float32_t) 3.14159265f;               //锁相频率
    pll->osgCofficient.osg_k=(float32_t)(0.5);                                 //系数K
    osgx = (float32_t)(2.0f*0.5f*wn*pll->Ti);                         //
    pll->osgCofficient.osg_x=(float32_t)(osgx);
    osgy = (float32_t)(wn*pll->Ti*wn*pll->Ti);
    pll->osgCofficient.osg_y=(float32_t)(osgy);
    temp = (float32_t)1.0/(osgx+osgy+4.0f);
    pll->osgCofficient.osg_b0=((float32_t)osgx*temp);
    pll->osgCofficient.osg_b2=((float32_t)(-1.0f)*pll->osgCofficient.osg_b0);
    pll->osgCofficient.osg_a1=((float32_t)(2.0*(4.0f-osgy))*temp);
    pll->osgCofficient.osg_a2=((float32_t)(osgx-osgy-4)*temp);
    pll->osgCofficient.osg_qb0=((float32_t)(0.5f*osgy)*temp);
    pll->osgCofficient.osg_qb1=(pll->osgCofficient.osg_qb0*(float32_t)(2.0));
    pll->osgCofficient.osg_qb2=pll->osgCofficient.osg_qb0;
}

void sogiPllLoop_calc(float vin, sogiPll *pll)
{
    // Update the pll->u[0] with the grid value
    pll->u[0]=vin;

    //
    // Orthogonal Signal Generator
    //
    pll->alpha[0]=(pll->osgCofficient.osg_b0*
                       (pll->u[0]-pll->u[2])) +
                       (pll->osgCofficient.osg_a1*pll->alpha[1]) +
                       (pll->osgCofficient.osg_a2*pll->alpha[2]);

    pll->alpha[2]=pll->alpha[1];
    pll->alpha[1]=pll->alpha[0];

    pll->beta[0]=(pll->osgCofficient.osg_qb0*pll->u[0]) +
                        (pll->osgCofficient.osg_qb1*pll->u[1]) +
                        (pll->osgCofficient.osg_qb2*pll->u[2]) +
                        (pll->osgCofficient.osg_a1*pll->beta[1]) +
                        (pll->osgCofficient.osg_a2*pll->beta[2]);

    pll->beta[2]=pll->beta[1];
    pll->beta[1]=pll->beta[0];

    pll->u[2]=pll->u[1];
    pll->u[1]=pll->u[0];

	//Calculate Park Transfer
//	arm_sin_cos_f32(pll->theta,&pll->sine,&pll->cosine);
	pll->u_D[0] = pll->alpha[0] * pll->cosine + pll->beta[0] * pll->sine;
	pll->u_Q[0] = pll->beta[0] * pll->cosine - pll->alpha[0] * pll->sine;

    //
    // Loop Filter
    //
    pll->ylf[0]=pll->ylf[1] +
                     (pll->LpfCofficient.b0*pll->u_Q[0]) +
                     (pll->LpfCofficient.b1*pll->u_Q[1]);
    pll->ylf[1]=pll->ylf[0];

    pll->u_Q[1]=pll->u_Q[0];

    //
    // VCO
    //
    pll->fo=pll->fn+pll->ylf[0];
	//PD
    pll->theta=pll->theta + (pll->fo*pll->Ti)*
                       (float32_t)(2.0*3.1415926f);

    if(pll->theta>(float32_t)(2.0*3.1415926f))
    {
        pll->theta=pll->theta - (float32_t)(2.0*3.1415926f);
        //pll->theta=0;
    }

    pll->sine=(float32_t)sinf(pll->theta);
    pll->cosine=(float32_t)cosf(pll->theta);

}
