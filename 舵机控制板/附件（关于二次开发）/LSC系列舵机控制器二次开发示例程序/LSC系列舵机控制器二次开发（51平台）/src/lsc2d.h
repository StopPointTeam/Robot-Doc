/******************************************************
 * FileName:      lsc2d.h
 * Company:       乐幻索尔
 * Date:          2016/07/09  09:30
 * Description:   Lobot舵机控制板二次开发示例，基于stc89c52rc开发，
 *                本文件包含了二次开发各命令的函数声明，数据结构等
 *****************************************************/
#ifndef LSC2D_H_
#define LSC2D_H_

#include "typedef.h"
#include <reg52.h>
#include <stdarg.h>
#include <string.h>
#include "uart.h"

#define FRAME_HEADER 0x55             //帧头
#define CMD_SERVO_MOVE 0x03           //舵机移动指令
#define CMD_ACTION_GROUP_RUN 0x06     //运行动作组指令
#define CMD_ACTION_GROUP_STOP 0x07    //停止动作组指令
#define CMD_ACTION_GROUP_SPEED 0x0B   //设置动作组运行速度
#define CMD_GET_BATTERY_VOLTAGE 0x0F  //获取电池电压指令

typedef struct _lobot_servo_ {  //舵机ID，目标地址结构体
	uint8_t ID;
	uint16_t Position;
} LobotServo;

extern void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time);
extern void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time);
extern void moveServos(uint8_t Num, uint16_t Time, ...);
extern void runActionGroup(uint8_t numOfAction, uint16_t Times);
extern void stopActionGroup(void);
extern void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed);
extern void setAllActionGroupSpeed(uint16_t Speed);
extern uint16_t getBatteryVoltage(uint16_t Timeout);

#endif
