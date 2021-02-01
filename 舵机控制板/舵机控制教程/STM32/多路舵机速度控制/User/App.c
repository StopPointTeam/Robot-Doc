/*****************************************************************************
 ** File			: App.c
 ** Author			: Zheng23
 ** Date			: 			
 ** Function		: Ӧ�ò�
*****************************************************************************/

#include "include.h"

uint16 ServoPwmDuty[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM������
uint16 ServoPwmDutySet[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM������
double ServoPwmDutyInc[8];		//Ϊ���ٶȿ��ƣ���PWM�������仯ʱ��ÿ2.5ms��20ms������PWM����

bool Flag_20ms = FALSE;		//20�����ʶλ���ڶ�ʱ�ж�������λ
bool Flag_2_5ms = FALSE;	//2.5�����ʶλ���ڶ�ʱ�ж�������λ
bool ServoPwmDutyHaveChange = FALSE;	//�����б仯��־λ
uint16 ServoTime = 2000;			//����ӵ�ǰ�Ƕ��˶���ָ���Ƕȵ�ʱ�䣬Ҳ���ǿ����ٶ�



void InitIO(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��
	   	 
	GPIOA->CRH&=0XFFFFFFF0; 
	GPIOA->CRH|=0X00000003;//PA8 �������   	 
    GPIOA->ODR|=1<<8;      //PA8 �����
    
	GPIOB->CRH&=0X0;
	GPIOB->CRH|=0X33333333;//PB�������
//	GPIOB->ODR|=1<<2;      //PB����� 
}


void TaskRun(void)
{
	uint8 i;
	static uint16 Time;
	if(Flag_20ms)	// Flag_2_5ms	2.5�������һ��ServoPwmDutyCompare()��20�������һ��	
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
			ServoPwmDutySet[0] = 2500;			//��������2500΢�룬��Ӧ90��
			for(i = 1;i<8;i++)
				ServoPwmDutySet[i] = 500;		//��������500΢�룬��Ӧ-90��
		}
		else
		{
			ServoPwmDutySet[0] = 500;			//��������500΢�룬��Ӧ-90��
			for(i = 1;i<8;i++)
				ServoPwmDutySet[i] = 1500;		//��������1500΢�룬��Ӧ0��
		}
		ServoPwmDutyHaveChange = TRUE;
	}

}


void ServoPwmDutyCompare(void)//����仯�Ƚϼ��ٶȿ���
{
	uint8 i;
	
	static uint16 ServoPwmDutyIncTimes;	//��Ҫ�����Ĵ���
	static bool ServoRunning = FALSE;	//���������ָ���ٶ��˶���ָ���������Ӧ��λ��
	if(ServoRunning == FALSE && ServoPwmDutyHaveChange)//ֹͣ�˶������������仯ʱ�Ž��м���
	{
		ServoPwmDutyHaveChange = FALSE;
//		ServoPwmDutyIncTimes = ServoTime*2/5;	//ServoTime/(20/8)	//��ÿ2.5ms����һ��ServoPwmDutyCompare()����ʱ�ô˾�
		ServoPwmDutyIncTimes = ServoTime/20;	//��ÿ20ms����һ��ServoPwmDutyCompare()����ʱ�ô˾�
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
				ServoPwmDutyInc[i] /= ServoPwmDutyIncTimes;//ÿ�ε���������
			}
		}
		ServoRunning = TRUE;	//�����ʼ����
	}
	if(ServoRunning)
	{
		ServoPwmDutyIncTimes--;
		for(i=0;i<8;i++)
		{
			if(ServoPwmDutyIncTimes == 0)
			{		//���һ�ε�����ֱ�ӽ��趨ֵ������ǰֵ
				ServoPwmDuty[i] = ServoPwmDutySet[i];
				ServoRunning = FALSE;	//�����趨λ�ã����ֹͣ�˶�
			}
			else
			{
				ServoPwmDuty[i] = ServoPwmDutySet[i] + 
					(signed short int)(ServoPwmDutyInc[i] * ServoPwmDutyIncTimes);
			}
		}
		
	}
}


//��PWM����ת�����Զ�װ�ؼĴ�����ֵ
void Timer3ARRValue(uint16 pwm)	
{
	TIM3->ARR = pwm + 1;
}


//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		
	static uint16 i = 1;
	
	if(TIM3->SR&0X0001)//����ж�
	{
		switch(i)
		{
			case 1:
				SERVO0 = 1;	//PWM���ƽŸߵ�ƽ
				//����ʱ��0��ֵ������Pwm0Duty�����������жϣ��´��жϻ������һ��case���
				Timer3ARRValue(ServoPwmDuty[0]);
				Flag_20ms = TRUE;
				Flag_2_5ms = TRUE;
				break;
			case 2:
				SERVO0 = 0;	//PWM���ƽŵ͵�ƽ
				//�˼�������ֵ�������жϱ�ʾ��һ����ԪҪ��������Ŀ�ʼ
				Timer3ARRValue(2500-ServoPwmDuty[0]);	
				break;
			case 3:
				SERVO1 = 1;	
				Timer3ARRValue(ServoPwmDuty[1]);
				Flag_2_5ms = TRUE;
				break;
			case 4:
				SERVO1 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[1]);	
				break;
			case 5:
				SERVO2 = 1;	
				Timer3ARRValue(ServoPwmDuty[2]);
				Flag_2_5ms = TRUE;
				break;
			case 6:
				SERVO2 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[2]);	
				break;	
			case 7:
				SERVO3 = 1;	
				Timer3ARRValue(ServoPwmDuty[3]);
				Flag_2_5ms = TRUE;
				break;
			case 8:
				SERVO3 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[3]);	
				break;	
			case 9:
				SERVO4 = 1;	
				Timer3ARRValue(ServoPwmDuty[4]);
				Flag_2_5ms = TRUE;
				break;
			case 10:
				SERVO4 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[4]);	
				break;	
			case 11:
				SERVO5 = 1;	
				Timer3ARRValue(ServoPwmDuty[5]);
				Flag_2_5ms = TRUE;
				break;
			case 12:
				SERVO5 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[5]);	
				break;
			case 13:
				SERVO6 = 1;	
				Timer3ARRValue(ServoPwmDuty[6]);
				Flag_2_5ms = TRUE;
				break;
			case 14:
				SERVO6 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[6]);	
				break;
			case 15:
				SERVO7 = 1;	
				Timer3ARRValue(ServoPwmDuty[7]);
				Flag_2_5ms = TRUE;
				break;
			case 16:
				SERVO7 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[7]);
				i = 0;	
				break;				 
		}
		i++;
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}



