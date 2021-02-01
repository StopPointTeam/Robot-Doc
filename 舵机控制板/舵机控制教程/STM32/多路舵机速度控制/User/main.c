#include "include.h"

int main(void)
{
	InitStm32Clock(9);	//9±∂∆µ
	InitDelay(72);	//œµÕ≥ ±÷”72M
	InitTimerx(10000,72);	//
	InitIO();
	for(;;)
	{
		TaskRun();
	}
}
