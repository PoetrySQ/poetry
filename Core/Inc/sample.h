/*
 * sample.h
 *
 *  Created on: May 1, 2024
 *      Author: SSQ
 */

#ifndef INC_SAMPLE_H_
#define INC_SAMPLE_H_

#include <common.h>
//ADC数据结构
typedef struct{
	uint32_t raw;
	float gain;
	float data;
}adcValueStruct;
//有效值数据结构
typedef struct{
	float sum;
	float rms;
}rmsValueStruct;

typedef struct{
	adcValueStruct vPll;
	adcValueStruct fGrid;

	adcValueStruct iGrid;
	adcValueStruct vInv;
	adcValueStruct vGrid;
	adcValueStruct Temperature;

	adcValueStruct flybackI1;
	adcValueStruct flybackI2;
	adcValueStruct vPV;
	adcValueStruct iPV;
}adcValue;

typedef struct{
	rmsValueStruct iGridRMS;
	rmsValueStruct vGridRMS;
	rmsValueStruct vInvRMS;
	uint16_t rmsFlag;
}rmsValue;

extern adcValue adcSampleValue;
extern rmsValue rmsCalcValue;
void adcValueInit(adcValue *adc);
void adcSampleGainInit(adcValue *arg);
void calcAdcSample(adcValue *arg);
void adcReadConvert(adcValue *arg);
void rmsValueInit(rmsValue *arg);

#if 1
void calcRmsSum(rmsValue *arg);
void calcRms(rmsValue *arg);
#endif

#endif /* INC_SAMPLE_H_ */
