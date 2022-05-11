/******************************温度与光照测量************************
* 温度与光照
* 温度显示与左边，光照显示在右边
******************************************************************/

#include "sys.h"
#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "adc.h"
#include "exti.h"
#include "timer.h"
#include "display.h"
#include "reverse.h"
#include "buzzer.h"
#include "usart.h"

/***************************主函数*****************************/
int main()
{
//	u8 rtc = 0;	//RTC实时时钟初始化返回值0 为成功
	
	Stm32_Clock_Init( 6 ); 
	delay_init( 72 );
	uart_init(72,115200);	 //串口初始化为115200
//	rtc = Rtc_Init();  //没有外部电源，无法操作备份区，暂时无法使用
	Timer2Init(9999, 7199);	//启动定时器2
	TimerxInit(9999, 7199); //启动定时器3
	Timer4Init(99, 7199);   //启动定时器4
	AdcInit();
	LED_Init();
	EXTIX_Init();
	Reverse_Init();
	Buzzer_Init();
	
	LED_SEL = 0;
//	if(rtc == 0)
	Display();
}

