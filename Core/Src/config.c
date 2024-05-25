/*
 * config.c
 *
 *  Created on: Apr 10, 2024
 *      Author: SSQ
 */


#include <config.h>

systemCofficient systemValue;
controlState control;
faultState fault;

//ADC检测值
uint32_t adcValueBuffer[4];

void systemCoefficientInit(systemCofficient *system)
{
	//系统参数
	system->Ti = 0.0001f;
	system->pi = 3.14159265f;
	system->maxFrequence = 52;
	system->minFrequence = 47;
	system->maxVoltagePV = 36;
	system->minVoltagePV = 15;
	system->maxVinvVoltage = 240;
	system->minVinvVoltage = 220;
	system->maxVoltageGrid = 240;
	system->minVoltageGrid = 190;
	system->maxGridConnnectedCurrent = 0.707; //有效值，软件只保护有效值
}

void controlStateInit(controlState *arg)
{
	arg->FSM_STATE = 0;
	arg->START_MACHINE = 0;
	arg->STOP_MACHINE = 0;
	arg->VOLTAGE_MODE = 0;
	arg->CURRENT_MODE = 0;
	arg->GRID_CONNECTED_FLAG = 0;
	arg->PLL_FLAG = 0;
	arg->ERROR_FLAG = 0;
	arg->CLEAR_FAULT = 0;
	arg->REALY_STATE = 0;
	arg->INIT_FLAG = 0;
	arg->TIMER1 = 0;
	arg->TIMER2 = 0;
	arg->PWM_ENABLE = 0;
}

void faultStateInit(faultState *arg)
{
	arg->PV_UNDER_VOLTAGE = 0;
	arg->PV_OVER_VOLTAGE = 0;
	arg->GRID_UNDER_VOLTAGE = 0;
	arg->GRID_OVER_VOLTAGE = 0;
	arg->GRID_UNDER_FREQ = 0;
	arg->GRID_OVER_FREQ = 0;
	arg->INV_OVER_CURRENT = 0;
	arg->INV_OVER_VOLTAGE = 0;
	arg->SAMPLE_ERROR = 0;
	arg->REALY_ERROR = 0;
	arg->PLL_ERROR = 0;
}

