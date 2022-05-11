#include "buzzer.h"
#include "delay.h"
#include "led.h"
void Buzzer_Init(void)
{
	RCC->APB2ENR |= 1<<6;
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000; 
}

void Buzzer_s(u8 second)
{
	u16 i = 200 * second;
	u16 j = 1;
	u8 temp = 0x00;
	LED_SEL = 1;
	LedValue(0x01);
	while(j < i)
	{
		beep = ~beep;
		delay_ms(5);
		j++;
		if(j % 50 == 0)
		{
			LedValue(temp);
			temp = temp == 0x00 ? 0x01 : 0x00;
		}
	}
	beep = 0;
	LED_SEL = 0;
}
