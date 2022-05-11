/******************************�¶�����ղ���************************
* �¶������
* �¶���ʾ����ߣ�������ʾ���ұ�
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

/***************************������*****************************/
int main()
{
//	u8 rtc = 0;	//RTCʵʱʱ�ӳ�ʼ������ֵ0 Ϊ�ɹ�
	
	Stm32_Clock_Init( 6 ); 
	delay_init( 72 );
	uart_init(72,115200);	 //���ڳ�ʼ��Ϊ115200
//	rtc = Rtc_Init();  //û���ⲿ��Դ���޷���������������ʱ�޷�ʹ��
	Timer2Init(9999, 7199);	//������ʱ��2
	TimerxInit(9999, 7199); //������ʱ��3
	Timer4Init(99, 7199);   //������ʱ��4
	AdcInit();
	LED_Init();
	EXTIX_Init();
	Reverse_Init();
	Buzzer_Init();
	
	LED_SEL = 0;
//	if(rtc == 0)
	Display();
}

