/*****************************************************************************
 ** �ļ�			: main.c
 ** ����			: Zheng23
 ** ����			: 			
 ** ����			:   ����8·PWM�������8·�������������20����ֳ�8��2500΢�룬
 						��8����Ԫ���ȼ������һ����Ԫ�Ķ��PWM�����ߵ������ȣ���
						Ϊ���ƶ�������������Ϊ2500΢�룬ʣ��ģ�20000 - 2500��
						�������ʣ�µ�7����Ԫʱ�䣬��һ����Ԫ�Ķ��PWM������ȫ��Ϊ
						�͵�ƽ�ˣ���ʱ�Ϳ��԰��ĵĴ���ʣ�µ�7����Ԫ�������ˣ�ͬ��
						�ڶ�����ԪҲ��һ��������ѭ����ȥ�Ϳ��Կ���8·PWM����ˡ�

/*****************************************************************************/
//#include <STC15.h>
//#include"STC89C5xRC.H"


//һ��Ҫ��12M�ľ���
//һ��Ҫ��12M�ľ���
//һ��Ҫ��12M�ľ���
//һ��Ҫ��12M�ľ���
//һ��Ҫ��12M�ľ���
//һ��Ҫ��12M�ľ���


#include "reg51.h"

#define bool	bit
#define uint8	unsigned char    
#define uint16	unsigned short int
#define uint32	unsigned long 

sbit SERVO0=P0^0;			//���ƶ����PWM�����
sbit SERVO1=P0^1;			
sbit SERVO2=P0^2;		
sbit SERVO3=P0^3;
sbit SERVO4=P0^4;		
sbit SERVO5=P0^5;		
sbit SERVO6=P0^6;			
sbit SERVO7=P0^7;

sbit LED=P2^7;		//������

uint16 ServoPwmDuty[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM������
uint16 ServoPwmDutySet[8] = {1500,1500,1500,1500,1500,1500,1500,1500};	//PWM������
double ServoPwmDutyInc[8];		//Ϊ���ٶȿ��ƣ���PWM�������仯ʱ��ÿ2.5ms��20ms������PWM����

bool Flag_20ms = 0;		//20�����ʶλ���ڶ�ʱ�ж�������λ
bool Flag_2_5ms = 0;	//2.5�����ʶλ���ڶ�ʱ�ж�������λ
bool ServoPwmDutyHaveChange = 0;	//�����б仯��־λ
uint16 ServoTime = 2000;			//����ӵ�ǰ�Ƕ��˶���ָ���Ƕȵ�ʱ�䣬Ҳ���ǿ����ٶ�
/***********************************************************
* ��    �ƣ� DelayMs(uint16 ms) 
* ��    �ܣ� ��ʱms����
* ��ڲ����� ms	����
* ���ڲ����� ��
* ˵    ���� 					 
/**********************************************************/
void DelayMs(uint16 ms)
{
	uint16 i,j;
	for(i=0;i<800;i++)		//89��Ƭ����85,12ϵ�е�Ƭ����800
		for(j=0;j<ms;j++);
}

/***********************************************************
* ��    �ƣ�InitTimer0()
* ��    �ܣ�ʱ��0��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����12M����12��Ƶ�����Լ�����ÿ����һ��������1΢�룬��ȫ���������Ƶľ���Ҫ��
			��Ϊ��ʱ����TH0��TL0��Ҫȫ��������0xFF���ڼ�1�����ͻ�����жϣ�����Ҫ�����
			x������жϣ���ôTH0��TL0��Ӧ�ø�ֵ��0xFFFF-x��	�����ֵ��ʼ����������ʱ�ж�					 
/**********************************************************/ 
void InitTimer0(void)
{
//	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
//	AUXR |= 0x00;		//T0,T1������12T
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0x00;			//���ö�ʱ��ֵ
	TH0 = 0x00;			//���ö�ʱ��ֵ
	TF0 = 0;			//���TF0��־
	TR0 = 1;			//��ʱ��0��ʼ��ʱ
	ET0 = 1;			//����ʱ��0�ж�
}

/***********************************************************
* ��    �ƣ�Timer0Value(uint16 pwm)  
* ��    �ܣ�����ʱ��0��������ֵ������ʱ�ж� 
* ��ڲ�����pwm		���ƶ����PWM������ֵ����Χ��500~2500��
* ���ڲ�������
* ˵    ����12M����12��Ƶ�����Լ�����ÿ����һ��������1΢�룬��ȫ���������Ƶľ���Ҫ��
			��Ϊ��ʱ����TH0��TL0��Ҫȫ��������0xFF���ڼ�1�����ͻ�����жϣ�����Ҫ�����
			pwm������жϣ���ôTH0��TL0��Ӧ�ø�ֵ��0xFFFF-pwm��	�����ֵ��ʼ����������ʱ�ж� 					 
/**********************************************************/ 
void Timer0Value(uint16 pwm)
{
	uint16 value;
	value=0xffff-pwm;		
	TR0 = 0;
	TL0=value;			//16λ���ݸ�8λ���ݸ�ֵĬ�Ͻ�16λ���ݵĵͰ�λֱ�Ӹ�����λ����
    TH0=value>>8;		//��16λ��������8λ��Ҳ���ǽ���8λ�Ƶ��Ͱ�λ���ٸ�ֵ��8λ����	
	TR0 = 1;
}

/***********************************************************
* ��    �ƣ� ServoPwmDutyCompare()
* ��    �ܣ� ����仯�Ƚϼ��ٶȿ���
* ��ڲ����� ��
* ���ڲ����� ��
* ˵    ���� 					 
/**********************************************************/
void ServoPwmDutyCompare()
{
	uint8 i;
	
	static uint16 ServoPwmDutyIncTimes;	//��Ҫ�����Ĵ���
	static bool ServoRunning = 0;	//���������ָ���ٶ��˶���ָ���������Ӧ��λ��
	if(ServoRunning == 0 && ServoPwmDutyHaveChange)//ֹͣ�˶������������仯ʱ�Ž��м���
	{
		ServoPwmDutyHaveChange = 0;
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
		ServoRunning = 1;	//�����ʼ����
	}
	if(ServoRunning)
	{
		ServoPwmDutyIncTimes--;
		for(i=0;i<8;i++)
		{
			if(ServoPwmDutyIncTimes == 0)
			{		//���һ�ε�����ֱ�ӽ��趨ֵ������ǰֵ
				ServoPwmDuty[i] = ServoPwmDutySet[i];
				ServoRunning = 0;	//�����趨λ�ã����ֹͣ�˶�
			}
			else
			{
				ServoPwmDuty[i] = ServoPwmDutySet[i] + 
					(signed short int)(ServoPwmDutyInc[i] * ServoPwmDutyIncTimes);
			}
		}
		
	}
}

/***********************************************************
* ��    �ƣ� main()
* ��    �ܣ� ��ں���
* ��ڲ����� ��
* ���ڲ����� ��
* ˵    ���� 					 
/**********************************************************/ 
void main(void)
{
	uint8 i;
	uint16 Time;
	InitTimer0();	//��ʱ��0��ʼ��
	EA = 1;			//�����ж�
	while(1)		//��ѭ��
	{	
		if(Flag_20ms)	// Flag_2_5ms	2.5�������һ��ServoPwmDutyCompare()��20�������һ�� 	
		{	
			Flag_20ms = 0;
			Flag_2_5ms = 0;
			Time++;
			ServoPwmDutyCompare();
		}
		if(Time > 110)
		{
			Time = 0;
			LED = ~LED;
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
			ServoPwmDutyHaveChange = 1;
		}
	}
}

/***********************************************************
* ��    �ƣ� Timer0_isr() interrupt 1 using 1
* ��    �ܣ� ʱ��0�жϴ���
* ��ڲ����� ��
* ���ڲ����� ��
* ˵    ���� 					 
/**********************************************************/ 
void Timer0_isr(void) interrupt 1 using 1
{
	static uint16 i = 1;	//��̬������ÿ�ε��ú���ʱ������һ��������ֵ��
							//��ȫ�ֱ������ƣ���ͬ����ֻ�����ڴ˺����ڲ�
	switch(i)
	{
		case 1:
			SERVO0 = 1;	//PWM���ƽŸߵ�ƽ
			//����ʱ��0��ֵ������Pwm0Duty�����������жϣ��´��жϻ������һ��case���
			Timer0Value(ServoPwmDuty[0]);
			Flag_20ms = 1;
			Flag_2_5ms = 1;
			break;
		case 2:
			SERVO0 = 0;	//PWM���ƽŵ͵�ƽ
			//�˼�������ֵ�������жϱ�ʾ��һ����ԪҪ��������Ŀ�ʼ
			Timer0Value(2500-ServoPwmDuty[0]);	
			break;
		case 3:
			SERVO1 = 1;	
			Timer0Value(ServoPwmDuty[1]);
			Flag_2_5ms = 1;
			break;
		case 4:
			SERVO1 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[1]);	
			break;
		case 5:
			SERVO2 = 1;	
			Timer0Value(ServoPwmDuty[2]);
			Flag_2_5ms = 1;
			break;
		case 6:
			SERVO2 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[2]);	
			break;	
		case 7:
			SERVO3 = 1;	
			Timer0Value(ServoPwmDuty[3]);
			Flag_2_5ms = 1;
			break;
		case 8:
			SERVO3 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[3]);	
			break;	
		case 9:
			SERVO4 = 1;	
			Timer0Value(ServoPwmDuty[4]);
			Flag_2_5ms = 1;
			break;
		case 10:
			SERVO4 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[4]);	
			break;	
		case 11:
			SERVO5 = 1;	
			Timer0Value(ServoPwmDuty[5]);
			Flag_2_5ms = 1;
			break;
		case 12:
			SERVO5 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[5]);	
			break;
		case 13:
			SERVO6 = 1;	
			Timer0Value(ServoPwmDuty[6]);
			Flag_2_5ms = 1;
			break;
		case 14:
			SERVO6 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[6]);	
			break;
		case 15:
			SERVO7 = 1;	
			Timer0Value(ServoPwmDuty[7]);
			Flag_2_5ms = 1;
			break;
		case 16:
			SERVO7 = 0;	//PWM���ƽŵ͵�ƽ
			Timer0Value(2500-ServoPwmDuty[7]);
			i = 0;	
			break;				 
	}
	i++;
}
