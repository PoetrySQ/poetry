

/*
 * config.h
 *
 *  Created on: Apr 9, 2024
 *      Author: SSQ
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <common.h>

//版本号
#define Version 1.0

//额定参数


//常量
#define PI_2 2*PI

//采样偏移量
#define VOLTAGE_OFFSET 2.5
#define CURRENT_OFFSET 3

//采样系数
//后续建议加入最原始计算公式，目前直接写入计算好的采样系数
#define SAMPLE_COFFICIENT 3.3/4096
#define CT1_COFFICIENT 20/7.5	//第一路反激
#define CT2_COFFICIENT 20/7.5	//第二路反激
#define VPV_COFFICIENT 10		//PV电压
#define IPV_COFFICIENT 10/5.1	//PV电流
#define INV_COFFICIENT 366.575	//逆变电压
#define INV_OFFSET 595.879		//逆变偏置
#define VG_COFFICIENT 366.575	//电网电压
#define VG_OFFSET 595.879		//电网偏置
#define IG_COFFICIENT 0.905773	//逆变电流
#define IG_OFFSET 1.808215		//电流偏置

//并网电压允许值
#define GRID_CONNECTED_VOLTAGE 215

//温度检测值
#define AVGSLOPE 4.3
#define V25 1.43

//ACF参数单位 nH,nF
//ACF参数A相
#define L_LEAK_A 9000
#define C_CLAMP_A 10
#define CA_A 0.174
#define CSW_A 0.628
//ACF参数B相
#define L_LEAK_B 100
#define C_CLAMP_B 10
#define CA_B 0.174
#define CSW_B 0.628

typedef struct
{
	float maxVoltagePV;
	float minVoltagePV;
	float maxVinvVoltage;
	float minVinvVoltage;
	float maxVoltageGrid;
	float minVoltageGrid;
	float maxFrequence;
	float minFrequence;
	float maxGridConnnectedCurrent;
	float pi;
	float Ti;
}systemCofficient;

//控制状态
typedef struct{
	uint8_t FSM_STATE;
	uint8_t START_MACHINE;
	uint8_t STOP_MACHINE;
	uint8_t CLEAR_FAULT;
	uint8_t VOLTAGE_MODE;
	uint8_t CURRENT_MODE;
	uint8_t GRID_CONNECTED_FLAG;
	uint8_t PLL_FLAG;
	uint8_t ERROR_FLAG;
	uint8_t REALY_STATE;
	uint8_t INIT_FLAG;
	uint8_t TIMER1;
	uint8_t TIMER2;
	uint8_t PWM_ENABLE;
}controlState;

//故障状态
typedef struct{
	uint8_t PV_UNDER_VOLTAGE;
	uint8_t PV_OVER_VOLTAGE;
	uint8_t GRID_UNDER_VOLTAGE;
	uint8_t GRID_OVER_VOLTAGE;
	uint8_t GRID_UNDER_FREQ;
	uint8_t GRID_OVER_FREQ;
	uint8_t INV_OVER_VOLTAGE;
	uint8_t INV_OVER_CURRENT;
	uint8_t SAMPLE_ERROR;
	uint8_t REALY_ERROR;
	uint8_t PLL_ERROR;
}faultState;

extern uint8_t fsmState;
extern controlState control;
extern faultState fault;
extern systemCofficient systemValue;
//ADC检测值
extern uint32_t adcValueBuffer[4];

//HRTIM比较值
extern uint32_t COMP1A;
extern uint32_t COMP2A;
extern uint32_t COMP3A;

extern uint32_t COMP1B;
extern uint32_t COMP2B;
extern uint32_t COMP3B;



void systemCoefficientInit(systemCofficient *system);
void controlStateInit(controlState *arg);
void faultStateInit(faultState *arg);
#endif /* INC_GLOBAL_H_ */

