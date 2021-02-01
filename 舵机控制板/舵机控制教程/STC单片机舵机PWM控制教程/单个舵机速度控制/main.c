/*****************************************************************************
 ** �ļ�			: main.c
 ** ����			: Zheng23
 ** ����			: 			
 ** ����			: 
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

sbit LED=P2^7;		//������

uint16 ServoPwmDuty=1500;	//PWM������
uint16 ServoPwmDutySet=1500;
double ServoPwmDutyInc;

bool Flag_20ms = 0;	//20�����ʶλ���ڶ�ʱ�ж�������λ
bool ServoPwmDutyHaveChange = 0;	//�����б仯��־λ

uint16 ServoTime = 2000;
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
	for(i=0;i<85;i++)	//89��Ƭ����85,12ϵ�е�Ƭ����800
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

	static uint16 ServoPwmDutyIncTimes;	//��Ҫ�����Ĵ���
	static bool ServoRunning = 0;	//���������ָ���ٶ��˶���ָ���������Ӧ��λ��
	if(ServoRunning == 0 && ServoPwmDutyHaveChange)//ֹͣ�˶������������仯ʱ�Ž��м���
	{
		ServoPwmDutyHaveChange = 0;
		ServoPwmDutyIncTimes = ServoTime/20;	
		if(ServoPwmDutySet > ServoPwmDuty)
		{
			ServoPwmDutyInc = ServoPwmDutySet - ServoPwmDuty;
			ServoPwmDutyInc = -ServoPwmDutyInc;
		}
		else
		{
			ServoPwmDutyInc = ServoPwmDuty - ServoPwmDutySet;
			
		}
		ServoPwmDutyInc /= ServoPwmDutyIncTimes;//ÿ�ε���������
		ServoRunning = 1;	//�����ʼ����
	}
	if(ServoRunning)
	{
		ServoPwmDutyIncTimes--;
		if(ServoPwmDutyIncTimes == 0)
		{		//���һ�ε�����ֱ�ӽ��趨ֵ������ǰֵ
			ServoPwmDuty = ServoPwmDutySet;
			ServoRunning = 0;	//�����趨λ�ã����ֹͣ�˶�
		}
		else
		{
			ServoPwmDuty = ServoPwmDutySet + 
				(signed short int)(ServoPwmDutyInc * ServoPwmDutyIncTimes);
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
	uint16 Time;
	InitTimer0();	//��ʱ��0��ʼ��
	EA = 1;			//�����ж�
	while(1)		//��ѭ��
	{	
		if(Flag_20ms)
		{
			Time++;
			Flag_20ms = 0;
			ServoPwmDutyCompare();
		}
		if(Time > 110)
		{
			Time = 0;
			LED = ~LED;
			if(ServoPwmDutySet == 500)
			{
				ServoPwmDutySet = 2500;			//��������2500΢�룬��Ӧ90��
			}
			else
			{
				ServoPwmDutySet = 500;			//��������500΢�룬��Ӧ-90��
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
			Timer0Value(ServoPwmDuty);
			Flag_20ms = 1;	
			break;
		case 2:
			SERVO0 = 0;	//PWM���ƽŵ͵�ƽ
			//�����������ʣ�µ�ʱ��(20000-Pwm0Duty)ȫ�ǵ͵�ƽ�ˣ�Pwm0Duty + (20000-Pwm0Duty) = 20000����������Ϊһ������20���� 
			Timer0Value(20000-ServoPwmDuty);
			i = 0;	
			break;	 
	}
	i++;
}
