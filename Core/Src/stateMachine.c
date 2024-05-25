#include <stateMachine.h>

#include <common.h>
#include <sample.h>
#include <config.h>
#include <sogiPll.h>
#include <DCL.h>
#include <lowPassFilter.h>
#include <task.h>
#include <mppt.h>
//#include <movingRms.h>

enum State fsmState;
fsmT stFsm;

//初始化状态
void runInit()
{
	/*如果还已经初始化，则运行*/
	if(control.INIT_FLAG == 1)
	{
		adcReadConvert(&adcSampleValue);

		/*累加RMS*/
		calcRmsSum(&rmsCalcValue);
		//初始化过程中执行RMS计算
		if(rmsCalcValue.rmsFlag == 20)
		{
			calcRms(&rmsCalcValue);
			rmsCalcValue.rmsFlag = 1;
		}
	}
	/*如果还未进行初始化，则运行*/
	if(control.INIT_FLAG == 0)
	{
	//校准并打开ADC
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED); //校准
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED); //校准
	HAL_ADC_Start(&hadc2);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcValueBuffer, 4);
	//打开捕获定时器
	HAL_TIM_IC_Start_IT(&htim17, TIM_CHANNEL_1);


	//初始化配置
	adcValueInit(&adcSampleValue);
	adcSampleGainInit(&adcSampleValue);
	rmsValueInit(&rmsCalcValue);
	systemCoefficientInit(&systemValue);
	mpptInit(&mpptMain);
#if 0
	//初始化有效值计算函数
	movingRmsInit(&vGridMovingRms, MOVINGRMS_MAX_L);
	movingRmsInit(&iGridMovingRms, MOVINGRMS_MAX_L);
	movingRmsInit(&vInvMovingRms, MOVINGRMS_MAX_L);
#endif

	//初始化锁相环参数
	sogiPll_init(&gridPll);
	sogiConfig(&gridPll,
	                     50,
	                     10000,
	                     (float32_t) 166.9743385,
	                     (float32_t) -166.2661165);
	sogiPllOsgCofficientCalc(&gridPll);
	//初始化1Hz滤波器
	lowPassFilterInit(&lowPass1Hz, &lowPass1HzCofficient, 0.5, 0.001);
	//初始化100Hz滤波器
	lowPassFilterInit(&lowPass100Hz, &lowPass100HzCofficient, 100.0, systemValue.Ti);
	//初始化DCL
	DCL_init(&mainDCL, 0.1, 20, 100.0, 0.1, 1/systemValue.Ti);
	//初始化CSL
	//CSL_init(1.5, 5, 0, 0.005, -0.005);

	//初始化状态
	faultStateInit(&fault);
	controlStateInit(&control);
#if 0
	openRelay();
#endif

	openGreen();
	//初始化完成
	control.INIT_FLAG = 1;
	}

	//初始化完毕，等待启机指令
	if(control.START_MACHINE == 1)
	{
		stFsm.stNextState = checkSelf;
		rmsCalcValue.rmsFlag = 1;
	}
}

//上电自检函数
void checkSelfState()
{
	adcReadConvert(&adcSampleValue);
	openRelay();
//继电器自检，检测吸合继电器后，逆变电压值与电网电压值的差值
#if 1
	/*累加RMS*/
	calcRmsSum(&rmsCalcValue);
	//初始化过程中执行RMS计算
	if(rmsCalcValue.rmsFlag == 1000)
	{
		calcRms(&rmsCalcValue);
		rmsCalcValue.rmsFlag = 1;
	}
	else
	{
		return;
	}
	if(abs(adcSampleValue.vInv.raw - adcSampleValue.vGrid.raw) > 100)
	{
		//继电器故障
		fault.REALY_ERROR = 1;
		control.ERROR_FLAG = 1;
	}
	closeRelay();
#endif

	//检查电网电压
	//检测电网电压是否在允许最大值与最小值。
//中国标准
//拟定，Vgrid=190~240，具体查看标准32004
#if 1
	if(rmsCalcValue.vGridRMS.rms > systemValue.maxVoltageGrid)
	{
		fault.GRID_OVER_VOLTAGE = 1;
		control.ERROR_FLAG = 1;
		//printf("电网过压");
	}
	if(rmsCalcValue.vGridRMS.rms < systemValue.minVoltageGrid)
	{
		fault.GRID_UNDER_VOLTAGE = 1;
		control.ERROR_FLAG = 1;
		//printf("电网欠压");
	}
#endif
//外国标准，暂定
#if 0
#endif
//反激电流自检
#if 1
	if((adcSampleValue.flybackI1.data<1) && (adcSampleValue.flybackI2.data<1))
	{
		fault.SAMPLE_ERROR = 0;
	}
	else
	{
		fault.SAMPLE_ERROR = 1;
		//printf("反激电流采样异常");
		control.ERROR_FLAG = 1;
	}
#endif
//PV电压自检
#if 1
	//检查PV电压
	if(adcSampleValue.vPV.data < systemValue.minVoltagePV)
	{
		//欠压
		fault.PV_UNDER_VOLTAGE = 1;
		//printf("PV欠压");
		control.ERROR_FLAG = 1;
	}
	if(adcSampleValue.vPV.data > systemValue.maxVoltagePV)
	{
		//过压
		fault.PV_OVER_VOLTAGE = 1;
		//printf("PV过压");
		control.ERROR_FLAG = 1;
	}
#endif

//检查是否锁相成功
#if 0

#endif
	if(control.ERROR_FLAG == 0)
	{
		stFsm.stNextState = workState;
	}
	else
	{
		stFsm.stNextState = errorState;
	}
}

void faultHandle()
{
	openRed();
	//报故障
	//检查故障是否存在，存在则进入
	if(control.ERROR_FLAG == 1)
	{
		//检查是否符合故障清楚条件
		if(control.CLEAR_FAULT == 1)
		{
			//故障清除,重新进入初始化状态
			controlStateInit(&control);
			faultStateInit(&fault);
			control.INIT_FLAG = 1;
			stopTim1Interrupt();
			stopTim2Interrupt();
			stFsm.stNextState = initState;
			startAllInterrupt();
			openGreen();
		}
		else
		{
			stFsm.stNextState = errorState;
		}
	}
	//故障不存在，但是仍进入了故障状态（Debug更改状态进入）
	else
	{
		stFsm.stNextState = initState;
		startAllInterrupt();
		stopPWM();
		openGreen();
	}
}

void fsmInit(fsmT* pFsm,uint8_t curState)
{
	pFsm->stNextState = initState;
	pFsm->stCurState = curState;
}

void startFsm()
{
	while(1)
	{
		switch(stFsm.stCurState)
		{
			//初始化状态
			case initState:
				control.FSM_STATE = initState;
				//如果TIM2没有开启，则进入
				if(control.TIMER2 == 0)
				{
					changeTaskTim2ToInit();
					startTim2Interrupt();
					changeADCTrigToTim2();
					startAllInterrupt();
				}
				else if(control.ERROR_FLAG == 1)
				{
					stFsm.stNextState = errorState;
				}
				break;
			//自检状态
			case checkSelf:
				if(control.FSM_STATE != checkSelf)
				{
					control.FSM_STATE = checkSelf;
					changeTaskTim2ToCheck();
				}
				else if(control.ERROR_FLAG == 1)
				{
					stFsm.stNextState = errorState;
				}
				break;
			//工作状态
			case workState:
				if(control.FSM_STATE != workState)
				{
					//发波，开启环路定时器
					startPWM();
					changeADCTrigToTim1();
					startTim1Interrupt();
					control.FSM_STATE = workState;
				}
				else if(control.ERROR_FLAG == 1)
				{
					stFsm.stNextState = errorState;
				}
				break;
			//故障状态
			case errorState:
				control.FSM_STATE = errorState;
				stopAllInterrupt();
				faultHandle();
				break;
		}
		stFsm.stCurState = stFsm.stNextState;
	}
}
