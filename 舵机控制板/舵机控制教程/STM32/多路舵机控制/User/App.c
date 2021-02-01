/*****************************************************************************
 ** File			: App.c
 ** Author			: Zheng23
 ** Date			: 			
 ** Function		: 应用层
*****************************************************************************/

#include "include.h"

uint16 ServoPwmDuty[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM脉冲宽度



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
	for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 500;		//脉冲宽度在500微秒，对应-90°
		DelayMs(1000);					//延时1秒
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1000;		//脉冲宽度在1000微秒，对应-45°
		DelayMs(1000);
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1500;		//脉冲宽度在1500微秒，对应0°
		DelayMs(1000);					//延时1秒
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 2000;		//脉冲宽度在2000微秒，对应45°
		DelayMs(1000);
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 2500;		//脉冲宽度在2500微秒，对应90°
		DelayMs(1000);					//延时1秒
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 2000;	
		DelayMs(1000);
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1500;		
		DelayMs(1000);					//延时1秒
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1000;	
		DelayMs(1000);

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
				break;
			case 2:
				SERVO0 = 0;	//PWM控制脚低电平
				//此计数器赋值产生的中断表示下一个单元要进行任务的开始
				Timer3ARRValue(2500-ServoPwmDuty[0]);	
				break;
			case 3:
				SERVO1 = 1;	
				Timer3ARRValue(ServoPwmDuty[1]);
				break;
			case 4:
				SERVO1 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[1]);	
				break;
			case 5:
				SERVO2 = 1;	
				Timer3ARRValue(ServoPwmDuty[2]);
				break;
			case 6:
				SERVO2 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[2]);	
				break;	
			case 7:
				SERVO3 = 1;	
				Timer3ARRValue(ServoPwmDuty[3]);
				break;
			case 8:
				SERVO3 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[3]);	
				break;	
			case 9:
				SERVO4 = 1;	
				Timer3ARRValue(ServoPwmDuty[4]);
				break;
			case 10:
				SERVO4 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[4]);	
				break;	
			case 11:
				SERVO5 = 1;	
				Timer3ARRValue(ServoPwmDuty[5]);
				break;
			case 12:
				SERVO5 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[5]);	
				break;
			case 13:
				SERVO6 = 1;	
				Timer3ARRValue(ServoPwmDuty[6]);
				break;
			case 14:
				SERVO6 = 0;	//PWM控制脚低电平
				Timer3ARRValue(2500-ServoPwmDuty[6]);	
				break;
			case 15:
				SERVO7 = 1;	
				Timer3ARRValue(ServoPwmDuty[7]);
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



