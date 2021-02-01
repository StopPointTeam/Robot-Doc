/******************************************************
 * FileName:      uart.c
 * Company:       乐幻索尔
 * Date:           2016/07/08  20:02
 * Description:    Lobot舵机控制板二次开发示例，基于stc89c52rc开发，
 *                  本文件包含了串口初始化和串口数据发送的实现。
 *                  @@特别说明：舵机控制板的通信口只支持9600波特率@@
 *****************************************************/
#include "uart.h"

void uartInit() //波特率9600
{
	SCON = 0x50;
	TMOD = 0x20;
	TH1 = TL1 = -(11059200L / 12 / 32 / 9600);
	TR1 = 1;
	ES = 1;           //使能Uart中断
	EA = 1;           //开中断
}


uint8_t busy = 0;   //串口发送忙标识

/*********************************************************************************
 * Function:    uartWriteByte
 * Description：串口发送一个字节数据
 * Parameters:   dat：被发数据
 * Return:       无返回
 * Others:
 **********************************************************************************/
void uartWriteByte(uint8_t dat)
{
	while (busy);
	busy = 1;
	SBUF = dat;
}

/*********************************************************************************
 * Function:    uartWriteBuf
 * Description：串口发送数据
 * Parameters:   buf：被发数据首地址，len:发送数据长度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void uartWriteBuf(uint8_t *buf, uint8_t len)
{
	ES = 1;
	while (len--) {
		while (busy);
		busy = 1;
		SBUF = *buf++;
	}
}
