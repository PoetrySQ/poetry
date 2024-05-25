/*
 * task.c
 *
 *  Created on: May 7, 2024
 *      Author: SSQ
 */


#include <task.h>

#include <sample.h>
#include <config.h>
#include <sogiPll.h>
#include <DCL.h>
#include <lowPassFilter.h>
#include <stateMachine.h>

/*
 * 创建结构体变量
 */
taskObject runInitTaskObject = {initTask,1,1,0};
taskObject checkTaskObject = {checkTask,1,1,0};
/*
 * 创建任务清单
 * 节拍：1ms,1kHz
 * taskListTim2[2]
 * 包含任务：
 * 0自检任务
 * 1自检指示灯任务，1s
 */
taskObject taskListTim2[2] = {
		{initTask,1,1,0},
		{ledTask,1000,1000,0}
};


/*
 * 创建任务清单
 * 节拍：0.1ms,10kHz
 * taskListTim1[2]
 * 包含任务：
 * 0环路任务
 * 1环路指示灯任务，500ms
 */
taskObject taskListTim1[2] = {
		{loopTask,1,1,0},
		{ledTask,2500,2500,0}
};

void initTask()
{
	runInit();
}

void checkTask()
{
	checkSelfState();
}

void ledTask()
{
	HAL_GPIO_TogglePin(GPIOB, LEDG_Pin);
}

void loopTask()
{
	HAL_GPIO_WritePin(GPIOB, test_Pin, SET);
	stopAllInterrupt();
	adcReadConvert(&adcSampleValue);
	/*累加RMS*/
	calcRmsSum(&rmsCalcValue);

	//计算锁相环
	sogiPllLoop_calc(adcSampleValue.vGrid.data, &gridPll);
/*
 * 全桥控制电路，根据锁相角度，打开相应桥臂
 */
#if 1
	//正向
	if(gridPll.theta < PI && gridPll.theta > 0)
	{
		forwardBridge();
	}
	//反向
	if(gridPll.theta > PI && gridPll.theta < PI_2)
	{
		reverseBridge();
	}
#endif
#if 0
	//判断是否锁相成功,通过比较当前锁相环输出fo和电网频率fg的误差值不大于0.01
	if((gridPll.fo - adcSampleValue.fGrid.data) < 0.8)
	{
		//锁相成功标志
		control.PLL_FLAG = 1;
	}
	else
	{
		//锁相失败，清0锁相标志
		control.PLL_FLAG = 0;
		fault.PLL_ERROR = 1;
		control.ERROR_FLAG = 1;
	}
#endif

	//判断逆变电压是否建立，建立逆变电压后，吸合继电器
	if(rmsCalcValue.vInvRMS.rms > GRID_CONNECTED_VOLTAGE && control.PLL_FLAG == 1)
	{
		//吸合继电器，并网
		openRelay();
		//开启继电器
		control.REALY_STATE = 1;
	}
	//并网条件,满足以下条件允许并网
	//锁相成功
	//逆变电压建立
	if(control.REALY_STATE)
	{
		//并网标志
		control.GRID_CONNECTED_FLAG = 1;
	}
	//电流控制环路运行条件
	//锁相成功
	//继电器吸合(同并网控制字bit8)
	//电流环路开始运行，反激发波
	if(control.CURRENT_MODE && control.GRID_CONNECTED_FLAG)
	{
		runCurrentLoop();

	}
	if(control.VOLTAGE_MODE && control.GRID_CONNECTED_FLAG)
	{
		runVoltageLoop();
	}

	protectCheck();
	//HAL_GPIO_TogglePin(GPIOC, test_Pin);
	//有效值测
//周期平均
#if 1
	//运行环路
	if(rmsCalcValue.rmsFlag == 200)
	{
		calcRms(&rmsCalcValue);
		rmsCalcValue.rmsFlag = 1;
	}

#endif
//递归移动
#if 0
	rmsCalcValue.vGridRMS.rms = calcMovingRms(&vGridMovingRms, adcSampleValue.vGrid.data);
	rmsCalcValue.iGridRMS.rms = calcMovingRms(&iGridMovingRms, adcSampleValue.iGrid.data);
	rmsCalcValue.vInvRMS.rms = calcMovingRms(&vInvMovingRms, adcSampleValue.vInv.data);
#endif
	if(control.ERROR_FLAG != 1 && control.STOP_MACHINE != 1)
	{
		startAllInterrupt();
	}
}
/*
 * 任务运行函数，放进ADC转换完成中断里面
 * 函数功能：判断中断来源然后选择不同的任务遍历循环
 */
void runTask()
{
	static uint8_t i;
	static uint8_t j;
	/*
	 * 判断定时器来源
	 */
	if(control.TIMER1 == 1)
	{
		for(i = 0; i < 2; i++)
		{
			if(taskListTim1[i].beats != 0)
			{
				taskListTim1[i].beats--;
				if(taskListTim1[i].beats == 0)
				{
					taskListTim1[i].beats = taskListTim1[i].count;
					taskListTim1[i].task();
				}
			}
		}
	}
	if(control.TIMER2 == 1)
	{
		for(j = 0; j < 2; j++)
		{
			if(taskListTim2[j].beats != 0)
			{
				taskListTim2[j].beats--;
				if(taskListTim2[j].beats == 0)
				{
					taskListTim2[j].beats = taskListTim2[j].count;
					taskListTim2[j].task();
				}
			}
		}
	}
	HAL_GPIO_WritePin(GPIOB, test_Pin, RESET);
	return;
}
