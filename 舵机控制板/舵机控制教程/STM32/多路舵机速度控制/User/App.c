/*****************************************************************************
 ** File			: App.c
 ** Author			: Zheng23
 ** Date			: 			
 ** Function		: 应用层
*****************************************************************************/

#include "include.h"

uint16 ServoPwmDuty[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM脉冲宽度
uint16 ServoPwmDutySet[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM脉冲宽度
double ServoPwmDutyInc[8];		//为了速度控制，当PWM脉宽发生变化时，每2.5ms或20ms递增的PWM脉宽

bool Flag_20ms = FALSE;		//20毫秒标识位，在定时中断里面置位
bool Flag_2_5ms = FALSE;	//2.5毫秒标识位，在定时中断里面置位
bool ServoPwmDutyHaveChange = FALSE;	//脉宽有变化标志位
uint16 ServoTime = 2000;			//舵机从当前角度运动到指定角度的时间，也就是控制速度



void InitIO(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟
	   	 
	GPIOA->CRH&=0XFFFFFFF0; 
	GPIOA->CRH|=0X00000003;//PA8 推挽输出   	 
    GPIOA->ODR|=1<<8;      //PA8 输出高
    
	GPIOB->CRH&=0X0;
	GPIOB->CRH|=0X33333333;//PB推挽输出
//	GPIOB->ODR|=1<<2;      //PB输出高 
}


void TaskRun(void)
{
	uint8 i;
	static uint16 Time;
	if(Flag_20ms)	// Flag_2_5ms	2.5毫秒调用一次ServoPwmDutyCompare()或20毫秒调用一次	
	{	
		Flag_20ms = FALSE;
		Flag_2_5ms = FALSE;
		Time++;
		ServoPwmDutyCompare();
	}
	if(Time > 110)
	{
		Time = 0;
		LED0 = ~LED0;
		if(ServoPwmDutySet[0] == 500)
		{
			ServoPwmDutySet[0] = 2500;			//脉冲宽度在2500微秒，对应90°
			for(i = 1;i<8;i++)
				ServoPwmDutySet[i] = 500;		//脉冲宽度在500微秒，对应-90°
		}
		else
		{
			ServoPwmDutySet[0] = 500;			//脉冲宽度在500微秒，对应-90°
			for(i = 1;i<8;i++)
				ServoPwmDutySet[i] = 1500;		//脉冲宽度在1500微秒，对应0°
		}
		ServoPwmDutyHaveChange = TRUE;
	}

}


void ServoPwmDutyCompare(void)//脉宽变化比较及速度控制
{
	uint8 i;
	
	static uint16 ServoPwmDutyIncTimes;	//需要递增的次数
	static bool ServoRunning = FALSE;	//舵机正在以指定速度运动到指定的脉宽对应的位置
	if(ServoRunning == FALSE && ServoPwmDutyHaveChange)//停止运动并且脉宽发生变化时才进行计算
	{
		ServoPwmDutyHaveChange = FALSE;
//		ServoPwmDutyIncTimes = ServoTime*2/5;	//ServoTime/(20/8)	//当每2.5ms调用一次ServoPwmDutyCompare()函数时用此句
		ServoPwmDutyIncTimes = ServoTime/20;	//当每20ms调用一次ServoPwmDutyCompare()函数时用此句
		for(i=0;i<8;i++)
		{
			//if(ServoPwmDuty[i] != ServoPwmDutySet[i])
			{
				if(ServoPwmDutySet[i] > ServoPwmDuty[i])
				{
					ServoPwmDutyInc[i] = ServoPwmDutySet[i] - ServoPwmDuty[i];
					ServoPwmDutyInc[i] = -ServoPwmDutyInc[i];
				}
				else
				{
					ServoPwmDutyInc[i] = ServoPwmDuty[i] - ServoPwmDutySet[i];
					
				}
				ServoPwmDutyInc[i] /= ServoPwmDutyIncTimes;//每次递增的脉宽
			}
		}
		ServoRunning = TRUE;	//舵机开始动作
	}
	if(ServoRunning)
	{
		ServoPwmDutyIncTimes--;
		for(i=0;i<8;i++)
		{
			if(ServoPwmDutyIncTimes == 0)
			{		//最后一次递增就直接将设定值赋给当前值
				ServoPwmDuty[i] = ServoPwmDutySet[i];
				ServoRunning = FALSE;	//到达设定位置，舵机停止运动
			}
			else
			{
				ServoPwmDuty[i] = ServoPwmDutySet[i] + 
					(signed short int)(ServoPwmDutyInc[i] * ServoPwmDutyIncTimes);
			}
		}
		
	}
}


//将PWM脉宽转化成自动装载寄存器的值
void Timer3ARRValue(uint16 pwm)	
{
	TIM3->ARR = pwm + 1;
}


//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		
	static uint16 i = 1;
	
	if(TIM3->SR&0X0001)//溢出中断
	{
		switch(i)
		{
			case 1:
				SERVO0 = 1;	//PWM控制脚高电平
				//给定时器0赋值，计数Pwm0Duty个脉冲后产生中断，下次中断会进入下一个case语句
				Timer3ARRValue(ServoPwmDuty[0]);
				Flag_20ms = TRUE;
				Flag_2_5ms = TRUE;
				break;
			case 2:
				SERVO0 = 0;	//PWM控制脚低电平
				//此计数器赋值产生的中断表示下一个单元要进行任务的开始
				Timer3ARRValue(2500-ServoPwmDuty[0]);	
				break;
			case 3:
				SERVO1 = 1;	
				Timer3ARRValue(ServoPwmDuty[1]);
				Flag_2_5ms = TRUE;
				break;
			case 4:
				SERVO1 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[1]);	
				break;
			case 5:
				SERVO2 = 1;	
				Timer3ARRValue(ServoPwmDuty[2]);
				Flag_2_5ms = TRUE;
				break;
			case 6:
				SERVO2 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[2]);	
				break;	
			case 7:
				SERVO3 = 1;	
				Timer3ARRValue(ServoPwmDuty[3]);
				Flag_2_5ms = TRUE;
				break;
			case 8:
				SERVO3 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[3]);	
				break;	
			case 9:
				SERVO4 = 1;	
				Timer3ARRValue(ServoPwmDuty[4]);
				Flag_2_5ms = TRUE;
				break;
			case 10:
				SERVO4 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[4]);	
				break;	
			case 11:
				SERVO5 = 1;	
				Timer3ARRValue(ServoPwmDuty[5]);
				Flag_2_5ms = TRUE;
				break;
			case 12:
				SERVO5 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[5]);	
				break;
			case 13:
				SERVO6 = 1;	
				Timer3ARRValue(ServoPwmDuty[6]);
				Flag_2_5ms = TRUE;
				break;
			case 14:
				SERVO6 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[6]);	
				break;
			case 15:
				SERVO7 = 1;	
				Timer3ARRValue(ServoPwmDuty[7]);
				Flag_2_5ms = TRUE;
				break;
			case 16:
				SERVO7 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[7]);
				i = 0;	
				break;				 
		}
		i++;
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}



