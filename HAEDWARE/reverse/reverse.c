#include "reverse.h"

void Reverse_Init(void) 
{
	RCC->APB2ENR |= 1<<3;	//使能portB时钟
	GPIOB->CRL &= 0xff0fffff;
	GPIOB->CRL |= 0x00800000;	//PB.5上拉输入
	GPIOB->ODR |= 0x00000010;	//上拉
}
