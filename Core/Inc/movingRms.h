/*
 * movingRms.h
 *
 *  Created on: May 5, 2024
 *      Author: SSQ
 */

#ifndef INC_MOVINGRMS_H_
#define INC_MOVINGRMS_H_
#include <common.h>

#define MOVINGRMS_MAX_L 512

typedef struct{
	//窗口长度
	uint16_t L;
	//1除以窗口长度
	float invL;
	//计数值
	uint16_t count;
	float movingRmsBuffer[MOVINGRMS_MAX_L];

	//平方值

	float outSquare;
}movingRmsStruct;

extern movingRmsStruct vGridMovingRms;
extern movingRmsStruct iGridMovingRms;
extern movingRmsStruct vInvMovingRms;
void movingRmsInit(movingRmsStruct *arg, uint16_t L);
float calcMovingRms(movingRmsStruct *arg, float in);

#endif /* INC_MOVINGRMS_H_ */
