/*
 * DCL.c
 *
 *  Created on: Apr 9, 2024
 *      Author: SSQ
 */


#include <DCL.h>

#include <mppt.h>
#include <PR.h>
#include <PID.h>
#include <sogiPll.h>
#include <stateMachine.h>

dclCofficient mainDCL;

//@param [in] kp: kp
//@param [in] kr: kr
//@param [in] f0: 中心频率
//@param [in] fc: 截止频率
//@param [in] Ts: 环路周期
void DCL_init(dclCofficient *formalDCL,
		float kp,
		float kr,
		float f0,
		float fc,
		float Ts)
{

	formalDCL->absVgrid = 0;
	formalDCL->kpv = 0;
	formalDCL->error = 0;
	formalDCL->d = 0;
	formalDCL->COMP1A = 0;
	formalDCL->COMP2A = 0;
	formalDCL->COMP3A = 0;

	formalDCL->COMP1B = 0;
	formalDCL->COMP2B = 0;
	formalDCL->COMP3B = 0;


	PR_init(&firstPr, kp, kr, f0, fc, Ts);

	//初始化死区值
	calcDeadTime(L_LEAK_A, C_CLAMP_A, CA_A + CSW_A, mainDCL.tdCountA);
	calcDeadTime(L_LEAK_B, C_CLAMP_B, CA_B + CSW_B, mainDCL.tdCountB);
}

//环路计算函数
//@param [in] pll: 锁相环地址
//@param [in] iL: 输出电感电流
//@param [in] Vpv: 光伏板电压
//@param [in] Vg: 电网采样电压
//@param [in] flyback_i1: 第一路反激原边电流
//@param [in] flyback_i2: 第二路反激原边电流

void currentLoopCalc(dclCofficient *formalDCL, float iL, float Vpv, float Vg, float flyback_i1, float flyback_i2)
{
/*
 * Flyback current filtering
 * Using lowpassfilter to filter the current
 * Create two structure of filter and Initialize them
 */
#if 0

#endif

	//参考电流和电感电流取绝对值
//	arm_abs_f32(&Vpll, &Vpll,1);
	arm_abs_f32(&iL, &iL, 1);
	arm_abs_f32(&gridPll.sine, &formalDCL->absVgrid, 1);

	//锁相环输出的Sine幅值为1，需要根据kg设置不同的增益，控制参考电流
	//计算误差
	formalDCL->error = formalDCL->absVgrid * mpptPid.out  - iL;
	firstPr.xn = formalDCL->error;

	//运行PR控制器
	PR_calc(&firstPr);

	//加入前馈控制
	formalDCL->d = (uint32_t)(firstPr.yn + Vg / (Vpv * formalDCL->kpv + Vg));


	//进均流环
	//d += (uint32_t)CSL_calc(flyback_i1, flyback_i2);

	formalDCL->d = upDownLimitUint(formalDCL->d, 98, 10);

	formalDCL->dutyComp = calcDuty(formalDCL->d / 100.0);
	//限制最大占空比为0.7
	//限制最小占空比为0.01
	formalDCL->dutyComp = upDownLimitUint(formalDCL->dutyComp, 32256, 460);

	formalDCL->COMP1A = formalDCL->dutyComp;
	formalDCL->COMP2A = formalDCL->COMP1A + mainDCL.tdCountA[0];
	formalDCL->COMP3A = 46080 - mainDCL.tdCountA[1];

	formalDCL->COMP1B = formalDCL->dutyComp;
	formalDCL->COMP2B = formalDCL->COMP1B + mainDCL.tdCountB[0];
	formalDCL->COMP3B = 46080 - mainDCL.tdCountB[1];

	updatePWM();
}

void voltageLoopCalc()
{
	return;
}



void protectCheck()
{
	//运行过程中故障检测，都检测瞬时值

	//检测电网电压是否在允许最大值与最小值。
//中国标准
//拟定，Vgrid=190~240，具体查看标准32004
#if 0
	if(adcSampleValue.vGrid.data > systemValue.maxVoltageGrid)
	{
		fault.GRID_OVER_VOLTAGE = 1;
		control.ERROR_FLAG = 1;
		//printf("电网过压");
	}
	if(adcSampleValue.vGrid.data < systemValue.minVoltageGrid)
	{
		fault.GRID_UNDER_VOLTAGE = 1;
		control.ERROR_FLAG = 1;
		//printf("电网欠压");
	}
#endif
//外国标准，暂定
#if 0
#endif
	//检测逆变是否过压
	if(adcSampleValue.vInv.data > systemValue.maxVoltageGrid)
	{
		fault.INV_OVER_VOLTAGE = 1;
		control.ERROR_FLAG = 1;
		//printf("逆变过压");
	}
	//检测逆变是否过流
	if(adcSampleValue.iGrid.data > systemValue.maxGridConnnectedCurrent)
	{
		fault.INV_OVER_CURRENT = 1;
		control.ERROR_FLAG = 1;
		//printf("逆变过流");
	}

	//检查故障字
	if(control.ERROR_FLAG)
	{
		//有故障，停止发波，停止中断
		stopAllInterrupt();
		stopPWM();
		//返回故障状态
		stFsm.stNextState = errorState;
	}
	//检测停机指令
	if(control.STOP_MACHINE == 1)
	{
		//停机
		stopAllInterrupt();
		stopPWM();
		//返回初始化状态
		controlStateInit(&control);
		faultStateInit(&fault);
		control.INIT_FLAG = 1;
		stopTim1Interrupt();
		stopTim2Interrupt();
		stFsm.stNextState = initState;
		startAllInterrupt();
	}
}

//死区计算函数
//T: 46080
//Input[0]: 漏感量，单位nH
//Input[1]: 钳位电容值，单位nF
//Input[2]: MOS结电容，单位nF
//DeadCount[0]: 辅助开关管开通前死区时间，单位ns
//DeadCount[1]: 主开关管开通前死区时间，通过设计辅助开关管关断时间，来实现下一周期触发主开关管开通时间前的死区。即提前关断时间

void calcDeadTime(float Lleak, float Cclamp, float Cw, uint32_t* DeadCount)
{
	float DeadTime[2];
	//计算时间
	DeadTime[0] = 1;
	//pi*sqrt(Lleak*(Cclamp+Cw)/2)
	DeadTime[1] = PI*sqrtf(Lleak*(Cclamp+Cw))/2;
	//转化为计数量
	DeadCount[0] = (DeadTime[0] / 10000) * 46080;
	DeadCount[1] = (DeadTime[1] / 10000) * 46080;
}

//电流环以及电压环放在ADC转换完成中断中运行
//电流环
void runCurrentLoop()
{
	//mppt Calculate
	mpptPno(&mpptMain, adcSampleValue.vPV.data, adcSampleValue.iPV.data);
	PI_transfer(mpptMain.Vref - adcSampleValue.vPV.data, &mpptPid);
	//Limiting Output amplitude of PI
	mpptPid.out = upDownLimitFloat(mpptPid.out, 0.6, 0);
	//计算电流环
	currentLoopCalc(&mainDCL,
			adcSampleValue.iGrid.data,
			adcSampleValue.vPV.data,
			adcSampleValue.vGrid.data,
			adcSampleValue.flybackI1.data,
			adcSampleValue.flybackI2.data);
}
//电压环
void runVoltageLoop()
{
	mainDCL.d = upDownLimitUint(mainDCL.d, 98, 10);

	mainDCL.dutyComp = calcDuty(mainDCL.d/100.0);
	//限制最大占空比为0.7
	//限制最小占空比为0.01
	mainDCL.dutyComp = upDownLimitUint(mainDCL.dutyComp, 32256, 460);

	mainDCL.COMP1A = mainDCL.dutyComp;
	mainDCL.COMP2A = mainDCL.dutyComp + mainDCL.tdCountA[0];
	mainDCL.COMP3A = 46080 - mainDCL.tdCountA[1];

	mainDCL.COMP1B = mainDCL.dutyComp;
	mainDCL.COMP2B = mainDCL.dutyComp + mainDCL.tdCountB[0];
	mainDCL.COMP3B = 46080 - mainDCL.tdCountB[1];

	updatePWM();
}



