#include "delay.h"
#include "uart.h"
#include "LobotServoController.h"
#include "bool.h"

 int main(void)
 {
	int i = 0;
 	SystemInit();//系统时钟等初始化
	delay_init(72);	     //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uartInit(9600);//串口初始化为9600
	 
	moveServo(1, 2000, 1000); //1秒移动1号舵机至2000位置
	delay_ms(3000);
	moveServos(2, 800, 2,1200,9,2300); //800毫秒2号舵机到1200位置，9号舵机到2300位置
	delay_ms(2000);
	runActionGroup(1, 1); //运行1号动作组1次
	delay_ms(1500);
  stopActionGroup();  //停止动作组运行
  setActionGroupSpeed(1, 200);//将1号动作组运行速度设置为200%
	runActionGroup(1, 1); //运行1号动作组1次
	delay_ms(1000);
	stopActionGroup();  //停止动作组运行
	while(1){
		
		receiveHandle(); //接收处理
		delay_ms(1);
		i++;
		if(i ==2000)
			getBatteryVoltage(); //发送获取电池电压指令
		if(i == 3000) {	
			printf("Bvolt:%d mv\r\n",batteryVolt); //打印电池电压
			i = 0;
	}
 }
}
