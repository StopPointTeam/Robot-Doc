/******************************************************
 * FileName:      uart.h
 * Company:       乐幻索尔
 * Date:           2016/07/08  20:02
 * Description:    Lobot舵机控制板二次开发示例，基于stc89c52rc开发，
 *                 本文件包含了串口初始化和串口数据发送的函数声明
 *****************************************************/

#ifndef UART_H_
#define UART_H_

#include "typedef.h"
#include <reg52.h>

extern void uartInit(void);
extern void uartWriteByte(uint8_t dat);
extern void uartWriteBuf(uint8_t *buf, uint8_t len);



#endif
