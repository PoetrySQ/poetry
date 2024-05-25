/*
 * mppt.h
 *
 *  Created on: May 25, 2024
 *      Author: SSQ
 */

#ifndef INC_MPPT_H_
#define INC_MPPT_H_

typedef struct{
	float Verr;	//误差电压
	float P_current;		//当前功率
	float P_previous;		//上一个周期功率
	float V_current;		//当前输入电压
	float V_previous;		//上一个周期电压
	float I_current;		//当前输入电压
	float I_previous;		//上一个周期电压
	float dV;				//电压增量
	float dI;				//电流增量
	float step;				//步长
	float Vref;
}mpptVariables;

extern mpptVariables mpptMain;

void mpptPno(mpptVariables *Pno, float Vpv, float Ipv);
void mpptInc(mpptVariables *Inc ,float Vpv, float Ipv);
void mpptInit(mpptVariables *arg);

#endif /* INC_MPPT_H_ */
