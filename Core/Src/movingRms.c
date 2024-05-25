/*
 * movingRms.c
 *
 *  Created on: May 5, 2024
 *      Author: SSQ
 */


#include <movingRms.h>

movingRmsStruct vGridMovingRms;
movingRmsStruct iGridMovingRms;
movingRmsStruct vInvMovingRms;

void movingRmsInit(movingRmsStruct *arg, uint16_t L)
{
	arg->L = L;
	arg->invL = 1.0f / (float)arg->L;

	for(uint16_t n = 0; n<L; n++)
	{
		arg->movingRmsBuffer[n] = 0.0f;
	}

	arg->count = 0;

	arg->outSquare = 0.0f;
}

float calcMovingRms(movingRmsStruct *arg, float in)
{
	float inSquare;
	inSquare = in * in;

	arg->movingRmsBuffer[arg->count] = inSquare;

	if(arg->count == arg->L - 1)
	{
		arg->count = 0;
	}
	else{
		arg->count++;
	}

	arg->outSquare = arg->outSquare + arg->invL * (inSquare - arg->movingRmsBuffer[arg->count]);

	return arg->outSquare;
}









