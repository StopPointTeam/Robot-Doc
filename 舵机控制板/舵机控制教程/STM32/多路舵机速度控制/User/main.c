#include "include.h"

int main(void)
{
	InitStm32Clock(9);	//9��Ƶ
	InitDelay(72);	//ϵͳʱ��72M
	InitTimerx(10000,72);	//
	InitIO();
	for(;;)
	{
		TaskRun();
	}
}
