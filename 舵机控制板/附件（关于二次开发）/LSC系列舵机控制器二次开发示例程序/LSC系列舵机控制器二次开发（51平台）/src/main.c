/******************************************************
 * FileName:      main.c
 * Company:       乐幻索尔
 * Date:          2016/07/09  09:30
 * Description:   Lobot舵机控制板二次开发示例，基于stc89c52rc开发
 *                @@特别说明：舵机控制板的通信口只支持9600波特率@@
 *****************************************************/

#include "lsc2d.h"
#include "intrins.h"

//延时1000ms
void Delay1000ms()              //@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 8;
	j = 1;
	k = 243;
	do {
		do {
			while (--k);
		} while (--j);
	} while (--i);
}

LobotServo servos[2];

int main()
{
	LobotServo servos[2];
	uint16_t voltage;
	uint8_t i;

	uartInit();     //初始化串口
	Delay1000ms();

	runActionGroup(100, 0);  //运行100号动作组
	i = 5;
	while (i--)
		Delay1000ms();
	stopActionGroup();      //停止动作组
	Delay1000ms();

	setActionGroupSpeed(100, 200);  //设置100号动作组速度为200%
	runActionGroup(100, 0);         //运行100号动作组
	i = 5;
	while (i--)
		Delay1000ms();
	stopActionGroup();     //停止动作组
	Delay1000ms();

	moveServo(0, 1500, 1000);  //1000ms移动0号舵机至1500位置

	Delay1000ms();
	moveServo(2, 800, 1000);  //1000ms移动2号舵机至800位置
	Delay1000ms();

	moveServos(5, 1000, 0, 1300, 2, 700, 4, 600, 6, 900, 8, 790); //1000ms移动5个舵机
	//0号舵机至1300位置，2号舵机至700位置，4号舵机至600位置，6号舵机至900位置，8号舵机至790位置
	Delay1000ms();

	servos[0].ID = 2; //舵机号2
	servos[0].Position = 1400; //1400位置
	servos[1].ID = 4; //舵机号4
	servos[1].Position = 700;  //700位置
	moveServosByArray(servos, 2, 1000); //移动2个舵机1000ms，位置和舵机号有 servos指定
	Delay1000ms();

	voltage = getBatteryVoltage(5000); //获得电池电压

	while (1);
}
