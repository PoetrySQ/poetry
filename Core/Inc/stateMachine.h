/*
 * stateMachine.h
 *
 *  Created on: Mar 15, 2024
 *      Author: SSQ
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include <common.h>

typedef struct FSM
{
	volatile uint8_t stCurState;
	volatile uint8_t stNextState;
}fsmT;

//状态
enum State {
	initState = 0,
	checkSelf,
	workState,
	errorState
};
extern enum State fsmState;
extern fsmT stFsm;
void runInit();
void checkSelfState();
void faultHandle();

void fsmInit(fsmT* pFsm,uint8_t curState);
void startFsm();


#endif /* INC_STATEMACHINE_H_ */
