/*****************************************************************************
 ** File			: App.c
 ** Author			: Zheng23
 ** Date			: 			
 ** Function		: Ӧ�ò�
*****************************************************************************/

#include "include.h"

uint16 ServoPwmDuty[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM������



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
	for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 500;		//��������500΢�룬��Ӧ-90��
		DelayMs(1000);					//��ʱ1��
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1000;		//��������1000΢�룬��Ӧ-45��
		DelayMs(1000);
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1500;		//��������1500΢�룬��Ӧ0��
		DelayMs(1000);					//��ʱ1��
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 2000;		//��������2000΢�룬��Ӧ45��
		DelayMs(1000);
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 2500;		//��������2500΢�룬��Ӧ90��
		DelayMs(1000);					//��ʱ1��
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 2000;	
		DelayMs(1000);
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1500;		
		DelayMs(1000);					//��ʱ1��
		for(i = 0;i<8;i++)
			ServoPwmDuty[i] = 1000;	
		DelayMs(1000);

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
				break;
			case 2:
				SERVO0 = 0;	//PWM���ƽŵ͵�ƽ
				//�˼�������ֵ�������жϱ�ʾ��һ����ԪҪ��������Ŀ�ʼ
				Timer3ARRValue(2500-ServoPwmDuty[0]);	
				break;
			case 3:
				SERVO1 = 1;	
				Timer3ARRValue(ServoPwmDuty[1]);
				break;
			case 4:
				SERVO1 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[1]);	
				break;
			case 5:
				SERVO2 = 1;	
				Timer3ARRValue(ServoPwmDuty[2]);
				break;
			case 6:
				SERVO2 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[2]);	
				break;	
			case 7:
				SERVO3 = 1;	
				Timer3ARRValue(ServoPwmDuty[3]);
				break;
			case 8:
				SERVO3 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[3]);	
				break;	
			case 9:
				SERVO4 = 1;	
				Timer3ARRValue(ServoPwmDuty[4]);
				break;
			case 10:
				SERVO4 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[4]);	
				break;	
			case 11:
				SERVO5 = 1;	
				Timer3ARRValue(ServoPwmDuty[5]);
				break;
			case 12:
				SERVO5 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[5]);	
				break;
			case 13:
				SERVO6 = 1;	
				Timer3ARRValue(ServoPwmDuty[6]);
				break;
			case 14:
				SERVO6 = 0;	//PWM���ƽŵ͵�ƽ
				Timer3ARRValue(2500-ServoPwmDuty[6]);	
				break;
			case 15:
				SERVO7 = 1;	
				Timer3ARRValue(ServoPwmDuty[7]);
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



