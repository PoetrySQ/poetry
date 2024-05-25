/*
 * DCL.h
 * Digital Control Loop
 *  Created on: Jan 31, 2024
 *      Author: SSQ
 */

#ifndef INC_DCL_H_
#define INC_DCL_H_

#include <common.h>
#include <config.h>
#include <sample.h>

//更换ADC触发源函数
#define externTrig(ins, x) do{ \
    ins->CFGR &= ~(0xF << 6); \
    ins->CFGR |= (x); \
} while(0)

typedef struct
{
	float absVgrid;		//The Gain Of Reference
	float kpv;		//The Gain Of Voltage PV
	float error;	//The Input Of PR Controller
	uint8_t d;		//The Duty Of Output
	uint32_t dutyComp;
	//HRTIM比较值
	uint32_t COMP1A;
	uint32_t COMP2A;
	uint32_t COMP3A;

	uint32_t COMP1B;
	uint32_t COMP2B;
	uint32_t COMP3B;
	uint32_t tdCountA[2];
	uint32_t tdCountB[2];
}dclCofficient;


//Declaration
void DCL_init(dclCofficient *formalDCL, float kp, float kr, float f0, float fc, float Ts);
void currentLoopCalc(dclCofficient *formalDCL, float iL, float Vpv, float Vg, float flyback_i1, float flyback_i2);
void calcDeadTime(float Lleak, float Cclamp, float Cw, uint32_t* DeadCount);

void runCurrentLoop();
void runVoltageLoop();
void protectCheck();

extern dclCofficient mainDCL;

//反激发波函数
static inline void startPWM()
{
	control.PWM_ENABLE = 1;
	HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_MASTER);
	HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
	HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2);
	HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
	HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1 + HRTIM_OUTPUT_TB2);
}

//反激封波函数，关闭PWM
static inline void stopPWM()
{
	control.PWM_ENABLE = 0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2);
	HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_A);
	HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TB1 + HRTIM_OUTPUT_TB2);
	HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_B);
	HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_MASTER);
}

//全桥正向函数
static inline void forwardBridge()
{
	HAL_GPIO_WritePin(GPIOB, forwardBridge_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, reverseBridge_Pin, RESET);
}
//全桥反向
static inline void reverseBridge()
{
	HAL_GPIO_WritePin(GPIOB, reverseBridge_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, forwardBridge_Pin, RESET);
}

//对应F334C8T6接口
#if 0
//关闭驱动芯片，IR2110S
//对应GPIO
//PB3->全桥
//PB4->Flyback1
//PB5->Flyback2
static inline void stopDriver()
{
	//拉高芯片SD引脚相对应GPIO即可
	HAL_GPIO_WritePin(GPIOB, SD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SD1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SD2_Pin, GPIO_PIN_SET);
}
static inline void startDriver()
{
	//拉低芯片SD引脚相对应GPIO即可
	//拉高芯片SD引脚相对应GPIO即可
	HAL_GPIO_WritePin(GPIOB, SD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SD1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SD2_Pin, GPIO_PIN_RESET);
}
#endif
//开启中断
static inline void startAllInterrupt()
{
	 __enable_irq();
}
//关闭中断
static inline void stopAllInterrupt()
{
	 __disable_irq();
}
//更新PWM函数
static inline void updatePWM()
{
	//更新比较值
	//A相
	__HAL_HRTIM_SETCOMPARE(&hhrtim1,0x0,HRTIM_COMPAREUNIT_1,mainDCL.COMP1A);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1,0x0,HRTIM_COMPAREUNIT_2,mainDCL.COMP2A);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1,0x0,HRTIM_COMPAREUNIT_3,mainDCL.COMP3A);
	//B相
	__HAL_HRTIM_SETCOMPARE(&hhrtim1,0x1,HRTIM_COMPAREUNIT_1,mainDCL.COMP1B);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1,0x1,HRTIM_COMPAREUNIT_2,mainDCL.COMP2B);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1,0x1,HRTIM_COMPAREUNIT_3,mainDCL.COMP3B);

}
//浮点数限幅函数
static inline float upDownLimitFloat(float arg, float max, float min)
{
	if(arg > max)
	{
		arg = max;
	}
	if(arg <= min)
	{
		arg = min;
	}
	return arg;
}
//无符号整数限幅函数
static inline uint32_t upDownLimitUint(uint32_t arg, uint32_t max, uint32_t min)
{
	if(arg > max)
	{
		arg = max;
	}
	if(arg <= min)
	{
		arg = min;
	}
	return arg;
}
//浮点数占空比转换为MCU的HRTIM的相应值
static inline uint32_t calcDuty(float d)
{
	return (uint32_t)(d * 46080);
}
//关闭TIM1
static inline void stopTim1Interrupt()
{
	HAL_TIM_Base_Stop(&htim1);
	control.TIMER1 = 0;
}
//关闭TIM2
static inline void stopTim2Interrupt()
{
	HAL_TIM_Base_Stop(&htim2);
	control.TIMER2 = 0;
}
//开启TIM1
static inline void startTim1Interrupt()
{
	stopTim2Interrupt();
	if(HAL_TIM_Base_Start(&htim1) != HAL_OK)
	{
		//开启定时器失败
		control.TIMER1 = 2;
	}
	else
	{
		//开启定时器成功
		control.TIMER1 = 1;
	}
	control.TIMER2 = 0;
}

//开启TIM2
static inline void startTim2Interrupt()
{
	stopTim1Interrupt();
	if(HAL_TIM_Base_Start(&htim2) != HAL_OK)
	{
		//开启定时器失败
		control.TIMER2 = 2;
	}
	else
	{
		//开启定时器成功
		control.TIMER2 = 1;
	}
	control.TIMER1 = 0;
}

//更改ADC触发源为TIM1
static inline void changeADCTrigToTim1()
{
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_TRGO;
	externTrig(hadc1.Instance,ADC_EXTERNALTRIGCONV_T1_TRGO);

}
//更改ADC触发源为TIM2
static inline void changeADCTrigToTim2()
{
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
	externTrig(hadc1.Instance,ADC_EXTERNALTRIGCONV_T2_TRGO);
}
//打开继电器
static inline void openRelay()
{
	HAL_GPIO_WritePin(GPIOB, AC_Relay_Pin, GPIO_PIN_SET);
	control.REALY_STATE = 1;
}
//关闭继电器
static inline void closeRelay()
{
	HAL_GPIO_WritePin(GPIOB, AC_Relay_Pin, GPIO_PIN_RESET);
	control.REALY_STATE = 0;
}
//亮红灯
static inline void openRed()
{
	HAL_GPIO_WritePin(GPIOB, LEDR_Pin, RESET);
	HAL_GPIO_WritePin(GPIOB, LEDG_Pin, SET);
}
//亮绿灯
static inline void openGreen()
{
	HAL_GPIO_WritePin(GPIOB, LEDR_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, LEDG_Pin, RESET);
}
//闪绿灯,1s一次
static inline void flashGreen()
{
	HAL_GPIO_TogglePin(GPIOB, LEDG_Pin);
}
#endif /* INC_DCL_H_ */
