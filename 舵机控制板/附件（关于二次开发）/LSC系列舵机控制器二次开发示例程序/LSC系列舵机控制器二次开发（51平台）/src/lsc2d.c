/******************************************************
 * FileName:      lsc2d.c
 * Company:       乐幻索尔
 * Date:          2016/07/09  09:30
 * Description:   Lobot舵机控制板二次开发示例，基于stc89c52rc开发，
 *                本文件包含了二次开发各命令的具体实现。
 *                @@特别说明：舵机控制板的通信口只支持9600波特率@@
 *****************************************************/

#include "lsc2d.h"

#define GET_LOW_BYTE(A) ((uint8_t)(A))
//宏函数 获得A的低八位
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))
//宏函数 获得B的高八位

uint8_t buf[103];  //建立发送缓存

/*********************************************************************************
 * Function:  moveServo
 * Description： 控制单个舵机转动
 * Parameters:   sevoID:舵机ID，Position:目标位置,Time:转动时间
                    舵机ID取值:0<=舵机ID<=31,Time取值: Time > 0
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time)
{
	if (servoID > 31 || !(Time > 0)) { //舵机ID不能打于31,可根据对应控制板修改
		return;
	}
	buf[0] = buf[1] = FRAME_HEADER;    //填充帧头
	buf[2] = 8;
	buf[3] = CMD_SERVO_MOVE;           //数据长度=要控制舵机数*3+5，此处=1*3+5//填充舵机移动指令
	buf[4] = 1;                        //要控制的舵机个数
	buf[5] = GET_LOW_BYTE(Time);       //取得时间的低八位
	buf[6] = GET_HIGH_BYTE(Time);      //取得时间的高八位
	buf[7] = servoID;                  //舵机ID
	buf[8] = GET_LOW_BYTE(Position);   //取得目标位置的低八位
	buf[9] = GET_HIGH_BYTE(Position);  //取得目标位置的高八位

	uartWriteBuf(buf, 10);
}

/*********************************************************************************
 * Function:  moveServosByArray
 * Description： 控制多个舵机转动
 * Parameters:   servos[]:舵机结体数组，Num:舵机个数,Time:转动时间
                    0 < Num <= 32,Time > 0
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time)
{
	uint8_t index = 7;
	uint8_t i = 0;

	if (Num < 1 || Num > 32 || !(Time > 0)) {
		return;                                          //舵机数不能为零和大与32，时间不能为零
	}
	buf[0] = buf[1] = FRAME_HEADER;                    //填充帧头
	buf[2] = Num * 3 + 5;                              //数据长度 = 要控制舵机数*3+5
	buf[3] = CMD_SERVO_MOVE;                           //填充舵机移动指令
	buf[4] = Num;                                      //要控制的舵机个数
	buf[5] = GET_LOW_BYTE(Time);                       //取得时间的低八位
	buf[6] = GET_HIGH_BYTE(Time);                      //取得时间的高八位

	for (i = 0; i < Num; i++) {                        //循环填充舵机ID和对应目标位置
		buf[index++] = servos[i].ID;                     //填充舵机ID
		buf[index++] = GET_LOW_BYTE(servos[i].Position); //填充目标位置低八位
		buf[index++] = GET_HIGH_BYTE(servos[i].Position);//填充目标位置高八位
	}

	uartWriteBuf(buf, buf[2] + 2);             //发送帧，长度为数据长度+两个字节的帧头
}

/*********************************************************************************
 * Function:  moveServos
 * Description： 控制多个舵机转动
 * Parameters:   Num:舵机个数,Time:转动时间,...:舵机ID,转动角，舵机ID,转动角度 如此类推
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServos(uint8_t Num, uint16_t Time, ...)
{
	uint8_t index = 7;
	uint8_t i = 0;
	uint16_t temp;
	va_list arg_ptr = NULL;  //可变参数

	va_start(arg_ptr, Time); //取得可变参数首地址
	if (Num < 1 || Num > 32 || (!(Time > 0)) || arg_ptr == NULL) {
		return;               //舵机数不能为零和大与32，时间不能为零，可变参数不能为空
	}

	buf[0] = buf[1] = FRAME_HEADER;      //填充帧头
	buf[2] = Num * 3 + 5;                //数据长度 = 要控制舵机数 * 3 + 5
	buf[3] = CMD_SERVO_MOVE;             //舵机移动指令
	buf[4] = Num;                        //要控制舵机数
	buf[5] = GET_LOW_BYTE(Time);         //取得时间的低八位
	buf[6] = GET_HIGH_BYTE(Time);        //取得时间的高八位

	for (i = 0; i < Num; i++) {//从可变参数中取得并循环填充舵机ID和对应目标位置
		temp = va_arg(arg_ptr, uint8_t);//可参数中取得舵机ID
		buf[index++] = GET_LOW_BYTE(temp);
		temp = va_arg(arg_ptr, uint16_t);  //可变参数中取得对应目标位置
		buf[index++] = GET_LOW_BYTE(temp); //填充目标位置低八位
		buf[index++] = GET_HIGH_BYTE(temp);//填充目标位置高八位
	}

	va_end(arg_ptr);  //置空arg_ptr

	uartWriteBuf(buf, buf[2] + 2);   //发送帧
}

/*********************************************************************************
 * Function:  runActionGroup
 * Description： 运行指定动作组
 * Parameters:   NumOfAction:动作组序号, Times:执行次数
 * Return:       无返回
 * Others:       Times = 0 时无限循环
 **********************************************************************************/
void runActionGroup(uint8_t numOfAction, uint16_t Times)
{
	buf[0] = buf[1] = FRAME_HEADER;  //填充帧头
	buf[2] = 5;                      //数据长度，数据帧除帧头部分数据字节数，此命令固定为5
	buf[3] = CMD_ACTION_GROUP_RUN;   //填充运行动作组命令
	buf[4] = numOfAction;            //填充要运行的动作组号
	buf[5] = GET_LOW_BYTE(Times);    //取得要运行次数的低八位
	buf[6] = GET_HIGH_BYTE(Times);   //取得要运行次数的高八位

	uartWriteBuf(buf, 7);            //发送数据帧
}

/*********************************************************************************
 * Function:  stopActiongGroup
 * Description： 停止动作组运行
 * Parameters:   Speed: 目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void stopActionGroup(void)
{
	buf[0] = FRAME_HEADER;     //填充帧头
	buf[1] = FRAME_HEADER;
	buf[2] = 2;                //数据长度，数据帧除帧头部分数据字节数，此命令固定为2
	buf[3] = CMD_ACTION_GROUP_STOP;   //填充停止运行动作组命令

	uartWriteBuf(buf, 4);      //发送数据帧
}

/*********************************************************************************
 * Function:  setActionGroupSpeed
 * Description： 设定指定动作组的运行速度
 * Parameters:   NumOfAction: 动作组序号 , Speed:目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed)
{
	buf[0] = buf[1] = FRAME_HEADER;   //填充帧头
	buf[2] = 5;                       //数据长度，数据帧除帧头部分数据字节数，此命令固定为5
	buf[3] = CMD_ACTION_GROUP_SPEED;  //填充设置动作组速度命令
	buf[4] = numOfAction;             //填充要设置的动作组号
	buf[5] = GET_LOW_BYTE(Speed);     //获得目标速度的低八位
	buf[6] = GET_HIGH_BYTE(Speed);    //获得目标熟读的高八位

	uartWriteBuf(buf, 7);              //发送数据帧
}

/*********************************************************************************
 * Function:  setAllActionGroupSpeed
 * Description： 设置所有动作组的运行速度
 * Parameters:   Speed: 目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void setAllActionGroupSpeed(uint16_t Speed)
{
	setActionGroupSpeed(0xFF, Speed);  //调用动作组速度设定，组号为0xFF时设置所有组的速度
}

uint8_t receiveBuf[6];      //建立接收缓存
uint8_t gotBatteryVoltage;  //接收到电压数据标识

/*********************************************************************************
 * Function:  getBatteryVoltage
 * Description： 发送获取电池电压命令
 * Parameters:   Timeout：重试次数
 * Return:       无返回
 * Others:
 **********************************************************************************/
uint16_t getBatteryVoltage(uint16_t timeOut)
{
	uint16_t Voltage = 0;
	buf[0] = FRAME_HEADER;  //填充帧头
	buf[1] = FRAME_HEADER;
	buf[2] = 2;             //数据长度，数据帧除帧头部分数据字节数，此命令固定为2
	buf[3] = CMD_GET_BATTERY_VOLTAGE;  //填充后的电池电压命令

	uartWriteBuf(buf, 4);   //发送数据帧

	while (timeOut--) {
		if (gotBatteryVoltage) {   //接收到电压数据
			Voltage = (((uint16_t)receiveBuf[5]) << 8) | receiveBuf[4];   //合高地八位为16位整数
			gotBatteryVoltage = 0; //置gotBatteryVoltage为0
			ES = 1;          //使能串口中断
			return Voltage;  //返回电压数据
		}
	}

	return 0;  //失败，返回0
}

extern uint8_t busy; //声明串口发送忙标识

/*********************************************************************************
 * Function:  receiveHandle
 * Description： 串口中断函数，处理接收数据，发送中断
 * Parameters:   无输入参数
 * Return:       无返回
 * Others:
 **********************************************************************************/
void receiveHandle() interrupt 4
{
	static uint8_t count;
	if (RI) {  //接收中断
		RI = 0;  //复位接收中断标识
		receiveBuf[count] = SBUF;   //读接收寄存器
		if (count < 2 && receiveBuf[count] == FRAME_HEADER) {  //辨识帧头
			count++;
		}else if (count == 2) {   //数据长度
			count++;
		}else if ((count - 1) < receiveBuf[2]) {   //接收数据
			count++;
		}else if ((count - 1) == receiveBuf[2]) {  //完成接收
			count = 0;
			if (receiveBuf[3] == CMD_GET_BATTERY_VOLTAGE) {
				gotBatteryVoltage = 1;  //置1接收到电压数据标识
				ES = 0;  //失能串口中断
			}
		}else{
			count = 0; //接收计数置0
			memset(receiveBuf, 0, sizeof(receiveBuf));   //置0 缓存
		}
	}
	if (TI) {     //发送中断
		TI = 0;     //复位发送中断标识
		busy = 0;   //串口发送忙标识置0
	}
}
