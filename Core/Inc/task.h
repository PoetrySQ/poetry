/*
 * task.h
 *
 *  Created on: May 7, 2024
 *      Author: SSQ
 */

#ifndef INC_TASK_H_
#define INC_TASK_H_

#include <common.h>

/*
 *任务宏定义
 *任务最大数量
 */

/*
 *创建任务结构
 *成员1：函数指针
 *成员2：节拍			一个节拍0.1ms
 *成员3：计数值
 *成员4：运行标志		0为关闭，1为打开
 */
typedef struct{
	void (*task)(void);
	uint32_t beats;
	uint32_t count;
	uint8_t runFlag;
}taskObject;

void initTask();
void checkTask();
void ledTask();
void loopTask();
void runTask();

extern taskObject runInitTaskObject;
extern taskObject checkTaskObject;
extern taskObject taskListTim2[2];
extern taskObject taskListTim1[2];

static inline void changeTaskTim2ToInit()
{
	taskListTim2[0] = runInitTaskObject;
}

static inline void changeTaskTim2ToCheck()
{
	taskListTim2[0] = checkTaskObject;
}

#endif /* INC_TASK_H_ */
