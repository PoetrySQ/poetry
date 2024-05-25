/*
 * sample.c
 *
 *  Created on: May 1, 2024
 *      Author: SSQ
 */

#include <sample.h>
#include <config.h>
#include <lowPassFilter.h>
#include <stateMachine.h>
adcValue adcSampleValue;
rmsValue rmsCalcValue;
//testStruct testStructValue;

void adcValueInit(adcValue *adc)
{
	//ADC采样值
	adc->flybackI1.data = 0;
	adc->flybackI2.data = 0;
	adc->vPV.data = 0;
	adc->iPV.data = 0;

	adc->fGrid.data = 50;
	adc->vInv.data = 0;
	adc->vGrid.data = 0;
	adc->iGrid.data = 0;
	adc->Temperature.data = 0;
	adc->vPll.data = 0;
}

//计算初始化函数
void rmsValueInit(rmsValue *arg)
{
	arg->rmsFlag = 1;
	arg->vGridRMS.rms = 0;
	arg->vInvRMS.rms = 0;
	arg->iGridRMS.rms = 0;

	arg->vGridRMS.sum = 0;
	arg->vInvRMS.sum = 0;
	arg->iGridRMS.sum = 0;
}

//初始化每个增益
void adcSampleGainInit(adcValue *arg)
{
	arg->vPV.gain = SAMPLE_COFFICIENT * VPV_COFFICIENT;
	arg->iPV.gain = SAMPLE_COFFICIENT * IPV_COFFICIENT;
	arg->flybackI1.gain = SAMPLE_COFFICIENT * CT1_COFFICIENT;
	arg->flybackI2.gain = SAMPLE_COFFICIENT * CT2_COFFICIENT;
	arg->vInv.gain = SAMPLE_COFFICIENT * INV_COFFICIENT;
	arg->vGrid.gain = SAMPLE_COFFICIENT * VG_COFFICIENT;
	arg->iGrid.gain = SAMPLE_COFFICIENT * IG_COFFICIENT;
	arg->fGrid.gain = 1;
}

//ADC读取转换函数，AD->raw
void adcReadConvert(adcValue *arg)
{
	//存放原始值
	//ADC1
	arg->flybackI1.raw = adcValueBuffer[0] & 0x0000FFFF;
	arg->flybackI2.raw = adcValueBuffer[1] & 0x0000FFFF;
	arg->vPV.raw = adcValueBuffer[2] & 0x0000FFFF;
	arg->iPV.raw = adcValueBuffer[3] & 0x0000FFFF;
	//ADC2
	arg->vInv.raw = (adcValueBuffer[0] & 0xFFFF0000) >> 16;
	arg->vGrid.raw = (adcValueBuffer[1] & 0xFFFF0000) >> 16;
	arg->iGrid.raw = (adcValueBuffer[2] & 0xFFFF0000) >> 16;
	arg->Temperature.data = (((((adcValueBuffer[3] & 0xFFFF0000) >> 16) * 3.3)/4095 - V25)/AVGSLOPE + 25);
//浮点数模式
#if 1
	//数据转换
	arg->flybackI1.data = arg->flybackI1.gain * arg->flybackI1.raw;
	arg->flybackI2.data = arg->flybackI2.gain * arg->flybackI2.raw;
	arg->vPV.data = arg->vPV.gain * arg->vPV.raw;
	arg->iPV.data = arg->iPV.gain * arg->iPV.raw;
	arg->vInv.data = arg->vInv.gain * arg->vInv.raw - INV_OFFSET;
	arg->vGrid.data = arg->vGrid.gain * arg->vGrid.raw - VG_OFFSET;
	arg->iGrid.data = arg->iGrid.gain * arg->iGrid.raw - IG_OFFSET;
#endif
//滤波
#if 0
	arg->vGrid.data = calcLowPassFilter(&lowPass100Hz, &lowPass100HzCofficient, arg->vGrid.data);
#endif
//定点数模式
#if 0

#endif

#if 0
	calcLowPassFilter(&lowPass100Hz,&lowPass100HzCofficient,adcSampleValue.vGrid.data);
#endif
}
#if 1
//有效值累加计算
void calcRmsSum(rmsValue *arg)
{
	arg->vGridRMS.sum += adcSampleValue.vGrid.data * adcSampleValue.vGrid.data;
	arg->vInvRMS.sum += adcSampleValue.vInv.data * adcSampleValue.vInv.data;
	arg->iGridRMS.sum += adcSampleValue.iGrid.data * adcSampleValue.iGrid.data;

	arg->rmsFlag++;
}
//当累加一个周期后调用此函数计算有效值
void calcRms(rmsValue *arg)
{
	arm_sqrt_f32(arg->vGridRMS.sum / arg->rmsFlag, &arg->vGridRMS.rms);
	arm_sqrt_f32(arg->vInvRMS.sum / arg->rmsFlag, &arg->vInvRMS.rms);
	arm_sqrt_f32(arg->iGridRMS.sum / arg->rmsFlag, &arg->iGridRMS.rms);
	if(control.FSM_STATE == initState || control.FSM_STATE == checkSelf)
	{
		arg->vGridRMS.rms = calcLowPassFilter(&lowPass1Hz, &lowPass1HzCofficient, arg->vGridRMS.rms);
	}

	arg->rmsFlag = 0;
	arg->vGridRMS.sum = 0;
	arg->vInvRMS.sum = 0;
	arg->iGridRMS.sum = 0;
}
#endif
