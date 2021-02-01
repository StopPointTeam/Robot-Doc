/*****************************************************************************
 ** 文件			: main.c
 ** 作者			: Zheng23
 ** 日期			: 			
 ** 功能			:   产生8路PWM输出控制8路舵机，方法：将20毫秒分成8个2500微秒，
 						即8个单元，先计算出第一个单元的舵机PWM控制线的脉冲宽度，因
						为控制舵机的脉冲宽度最大为2500微秒，剩余的（20000 - 2500）
						毫秒就是剩下的7个单元时间，第一个单元的舵机PWM控制线全部为
						低电平了，这时就可以安心的处理剩下的7个单元的事情了，同理
						第二个单元也是一样，这样循环下去就可以控制8路PWM输出了。

/*****************************************************************************/
//#include <STC15.h>
//#include"STC89C5xRC.H"


//一定要用12M的晶振
//一定要用12M的晶振
//一定要用12M的晶振
//一定要用12M的晶振
//一定要用12M的晶振
//一定要用12M的晶振

#include "reg51.h"

#define uint8	unsigned char    
#define uint16	unsigned short int
#define uint32	unsigned long 

sbit SERVO0=P0^0;			//控制舵机的PWM输出口
sbit SERVO1=P0^1;			
sbit SERVO2=P0^2;		
sbit SERVO3=P0^3;
sbit SERVO4=P0^4;		
sbit SERVO5=P0^5;		
sbit SERVO6=P0^6;			
sbit SERVO7=P0^7;
			
uint16 ServoPwmDuty[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM脉冲宽度
/***********************************************************
* 名    称： DelayMs(uint16 ms) 
* 功    能： 延时ms毫秒
* 入口参数： ms	毫秒
* 出口参数： 无
* 说    明： 					 
/**********************************************************/
void DelayMs(uint16 ms)
{
	uint16 i,j;
	for(i=0;i<85;i++)		//89单片机用85,12系列单片机用800
		for(j=0;j<ms;j++);
}
/***********************************************************
* 名    称：InitTimer0()
* 功    能：时钟0初始化
* 入口参数：无
* 出口参数：无
* 说    明：12M晶振，12分频，所以计数器每递增一个数就是1微秒，完全满足舵机控制的精度要求
			因为定时器是TH0，TL0都要全部计数到0xFF后在计1个数就会产生中断，所以要想产生
			x毫秒的中断，那么TH0，TL0就应该赋值（0xFFFF-x）	从这个值开始计数产生定时中断					 
/**********************************************************/ 
void InitTimer0(void)
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x00;			//设置定时初值
	TH0 = 0x00;			//设置定时初值
	TF0 = 0;			//清除TF0标志
	TR0 = 1;			//定时器0开始计时
	ET0 = 1;			//开定时器0中断
}
/***********************************************************
* 名    称：Timer0Value(uint16 pwm)  
* 功    能：给定时器0计数器赋值产生定时中断 
* 入口参数：pwm		控制舵机的PWM脉冲宽度值（范围：500~2500）
* 出口参数：无
* 说    明：12M晶振，12分频，所以计数器每递增一个数就是1微秒，完全满足舵机控制的精度要求
			因为定时器是TH0，TL0都要全部计数到0xFF后在计1个数就会产生中断，所以要想产生
			pwm毫秒的中断，那么TH0，TL0就应该赋值（0xFFFF-pwm）	从这个值开始计数产生定时中断 					 
/**********************************************************/ 
void Timer0Value(uint16 pwm)
{
	uint16 value;
	value=0xffff-pwm;		
	TR0 = 0;
	TL0=value;			//16位数据给8位数据赋值默认将16位数据的低八位直接赋给八位数据
    TH0=value>>8;		//将16位数据右移8位，也就是将高8位移到低八位，再赋值给8位数据	
	TR0 = 1;
}
/***********************************************************
* 名    称： main()
* 功    能： 入口函数
* 入口参数： 无
* 出口参数： 无
* 说    明： 					 
/**********************************************************/ 
void main(void)
{
	uint8 i;
	InitTimer0();	//定时器0初始化
	EA = 1;			//开总中断
	while(1)		//大循环
	{	
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
}

/***********************************************************
* 名    称： Timer0_isr() interrupt 1 using 1
* 功    能： 时钟0中断处理
* 入口参数： 无
* 出口参数： 无
* 说    明： 					 
/**********************************************************/ 
void Timer0_isr(void) interrupt 1 using 1
{
	static uint16 i = 1;	//静态变量：每次调用函数时保持上一次所赋的值，
							//跟全局变量类似，不同是它只能用于此函数内部
	switch(i)
	{
		case 1:
			SERVO0 = 1;	//PWM控制脚高电平
			//给定时器0赋值，计数Pwm0Duty个脉冲后产生中断，下次中断会进入下一个case语句
			Timer0Value(ServoPwmDuty[0]);	
			break;
		case 2:
			SERVO0 = 0;	//PWM控制脚低电平
			//此计数器赋值产生的中断表示下一个单元要进行任务的开始
			Timer0Value(2500-ServoPwmDuty[0]);	
			break;
		case 3:
			SERVO1 = 1;	
			Timer0Value(ServoPwmDuty[1]);	
			break;
		case 4:
			SERVO1 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[1]);	
			break;
		case 5:
			SERVO2 = 1;	
			Timer0Value(ServoPwmDuty[2]);	
			break;
		case 6:
			SERVO2 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[2]);	
			break;	
		case 7:
			SERVO3 = 1;	
			Timer0Value(ServoPwmDuty[3]);	
			break;
		case 8:
			SERVO3 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[3]);	
			break;	
		case 9:
			SERVO4 = 1;	
			Timer0Value(ServoPwmDuty[4]);	
			break;
		case 10:
			SERVO4 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[4]);	
			break;	
		case 11:
			SERVO5 = 1;	
			Timer0Value(ServoPwmDuty[5]);	
			break;
		case 12:
			SERVO5 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[5]);	
			break;
		case 13:
			SERVO6 = 1;	
			Timer0Value(ServoPwmDuty[6]);	
			break;
		case 14:
			SERVO6 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[6]);	
			break;
		case 15:
			SERVO7 = 1;	
			Timer0Value(ServoPwmDuty[7]);	
			break;
		case 16:
			SERVO7 = 0;	//PWM控制脚低电平
			Timer0Value(2500-ServoPwmDuty[7]);
			i = 0;	
			break;				 
	}
	i++;
}
