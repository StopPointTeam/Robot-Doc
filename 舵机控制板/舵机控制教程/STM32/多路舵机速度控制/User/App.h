#ifndef _APP_H_
#define _APP_H_


//LED端口定义
#define LED0 PAout(8)// PA8


#define SERVO0 PBout(8)// PB0	
#define SERVO1 PBout(9)// PB1
#define SERVO2 PBout(10)// PB2
#define SERVO3 PBout(11)// PB3
#define SERVO4 PBout(12)// PB4
#define SERVO5 PBout(13)// PB5
#define SERVO6 PBout(14)// PB6
#define SERVO7 PBout(15)// PB7



void InitIO(void);
void TaskRun(void);
void ServoPwmDutyCompare(void);//脉宽变化比较及速度控制

#endif
