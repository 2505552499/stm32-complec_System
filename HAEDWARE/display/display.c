#include "display.h"
#include "key.h"
#include "delay.h"
#include "exti.h"
#include "sys.h"
#include "led.h"
#include "timer.h"
#include "adc.h"
#include "rtc.h"

void Display(void)
{
	u8 i = 0;
	u16 j = 0;
	u8 temp = 17;
	//adcScan();
	AdcDescen();
	while(1)
	{
		i++;
		if (i > 10 ) 
		{
			//GetAdc(1, 10);
			AdcDescen();
			i = 0;
		}
		switch(menu % 5)
		{
			case 0:
				DisplayMenuState();
				switch(page % 3)
				{
					case 0:
						DisplayPage0(3);
						break;
					case 1:
						DisplaySteps(steps);
						break;
					case 2:
						DisplayCalories(steps);
						break;
					default:
						break;
				}
				
				break;
			case 1 :
				DisplayMenuState();
				switch(page % 4)
				{
					case 0:
						DisplayPage0(4);
						break;
					case 1:
						DisplayDate();
						break;
					case 2:
						DisplayCalendar();
						break;
					case 3:
						DisplayWeek();
						break;
					default:
						break;
				}
				break;
			case 2 :
				DisplayMenuState();
				switch(page % 4)
				{
					case 0:
						DisplayPage0(4);
						break;
					case 1:
						DisplayDigitalClock(hour, minute, second, j, temp);
						break;
					case 2:
						j++;
						if(j == 15)
						{
							switch(index)
							{
								case 0:
									temp = temp == 17 ? temp_second % 10 : 17;
									break;
								case 1:
									temp = temp == 17 ? temp_second / 10 : 17;
									break;
								case 3:
									temp = temp == 17 ? temp_minute % 10 : 17;
									break;
								case 4:
									temp = temp == 17 ? temp_minute / 10 : 17;
									break;
								case 6:
									temp = temp == 17 ? temp_hour % 10 : 17;
									break;
								case 7: 
									temp = temp == 17 ? temp_hour / 10 : 17;
									break;
								default:
									break;
							}
						}
						if(j == 30)
							j = 0;
						DisplayDigitalClock(hour_1, minute_1, second_1, j, temp);
						break;
					case 3:
						DisplayStopState();
						switch(stopPage)
						{
							case 0:
								DisplayStopwatch(ten_ms, second_2, minute_2);
								break;
							case 1:
								DisplayStopwatch(stopWatch[0].ten_ms, stopWatch[0].second, stopWatch[0].minute);
								break;
							case 2:
								DisplayStopwatch(stopWatch[1].ten_ms, stopWatch[1].second, stopWatch[1].minute);
								break;
							case 3:
								DisplayStopwatch(stopWatch[2].ten_ms, stopWatch[2].second, stopWatch[2].minute);
								break;
							case 4:
								DisplayStopwatch(stopWatch[3].ten_ms, stopWatch[3].second, stopWatch[3].minute);
								break;
							case 5:
								DisplayStopwatch(stopWatch[4].ten_ms, stopWatch[4].second, stopWatch[4].minute);
								break;
							default:
								break;
						}
						
						break;
					default:
						break;
				}
				break;
			case 3 :
				DisplayMenuState();
				switch(page % 3)
				{
					case 0:
						DisplayPage0(3);
						break;
					case 1:
						DisplayTemperature(adcTemperature);
						DisplayLight(adcLight);
						break;
					case 2:	 
						DisplayTemperature(adcInnerTemperature);
						DisplayLight(adcLight);
						break;
					default:
						break;
				}
				break;
			case 4 :
			/*
				DisplayMenuState();
				switch(page % 3)
				{
					case 0:
						DisplayPage0(3);
						break;
					case 1:
						DisplayNavKey(adcNavKey, adcNav);
						break;
					case 2:
						RTC_Get();
						DisplayDate();
						break;
					default:
						break;
				}
			*/
				break;
			case 5 :
				break;
			default :
				break;
		}
	}
}

void DisplayMenuState(void)
{
	LED_SEL = 1;
	LedValue(0x80>>menu);
	delay_ms(2);
}

void DisplayStopState(void)
{
	LED_SEL = 1;
 	LedValue(0x20>>stopPage);
	delay_ms(2);
}


void DisplayPage0(u16 totalPage)
{
	LED_SEL = 0;
 	SetLed(0, menu);
	delay_ms(1);
	SetLed(1, 17);
	delay_ms(1);
	SetLed(2, 17);
	delay_ms(1);
	SetLed(3, 17);
	delay_ms(1);
	SetLed(4, 17);
	delay_ms(1);
	SetLed(5, 17);
	delay_ms(1);
	SetLed(6, 17);
	delay_ms(1);
	SetLed(7, totalPage);
	delay_ms(1);
}
/*�¶���ʾģ��*/
void DisplayTemperature(float adcTemperature)
{
	LED_SEL = 0;
	SetLed(0, adcTemperature/10);
	delay_ms(1);
	PortationDisplay(1,(u8)adcTemperature%10);
	delay_ms(1);
	SetLed(2, (u8)(adcTemperature*10)%10);
	delay_ms(1);
}

/*������ʾģ��*/
void DisplayLight(u16 adcLight)
{
	LED_SEL = 0;
//	SetLed(0, 17);
//	delay_ms(1);
//	SetLed(1, 17);
//	delay_ms(1);
//	SetLed(2, 17);
//	delay_ms(1);
//	SetLed(3, 17);
//	delay_ms(1);
	SetLed(4, adcLight/1000);
	delay_ms(1);
	SetLed(5, adcLight%1000/100);
	delay_ms(1);
	SetLed(6, adcLight%100/10);
	delay_ms(1);
	SetLed(7, adcLight%10);
	delay_ms(1);
//	LED_SEL = 1;
//	LedValue(0x04);
//	delay_ms(4);
}


/*������ģ��*/
void DisplayCalendar(void)
{
	LED_SEL = 0;
	SetLed(0, calendar1.w_year / 1000);  //��ʾ ��
	delay_ms(1);
	SetLed(1, (calendar1.w_year % 1000) / 100);
	delay_ms(1);
	SetLed(2, (calendar1.w_year % 100) / 10);
	delay_ms(1);
	SetLed(3, (calendar1.w_year % 10) / 1);
	delay_ms(1);
	
	SetLed(4, calendar1.w_month / 10); //��ʾ ��
	delay_ms(1);
	SetLed(5, calendar1.w_month % 10);       
	delay_ms(1);
	SetLed(6, calendar1.w_date / 10); //��ʾ ��
	delay_ms(1);
	SetLed(7, calendar1.w_date % 10);
	delay_ms(1);
}

/*������ʾģ��*/
void DisplayWeek(void)
{
	LED_SEL = 0;
	SetLed(0, calendar1.w_month / 10);  //��ʾ ��
	delay_ms(1);
	SetLed(1, calendar1.w_month % 10);
	delay_ms(1);
	SetLed(2, 16);        //��ʾ"-"����
	delay_ms(1);
	SetLed(3, calendar1.w_date / 10); //��ʾ ��
	delay_ms(1);
	SetLed(4, calendar1.w_date % 10);
	delay_ms(1);
	SetLed(5, 17);        //����ʾ
	delay_ms(1);
	SetLed(6, 17); 
	delay_ms(1);
	SetLed(7, calendar1.week); //��ʾ����
	delay_ms(1);	
}


/*ʱ����ʾģ��*/
void DisplayDate(void)
{
	LED_SEL = 0;
	SetLed(0, calendar1.hour/10);  //��ʾ ʱ
	delay_ms(1);
	SetLed(1, calendar1.hour%10);
	delay_ms(1);
	SetLed(2, 16);        //��ʾ"-"����
	delay_ms(1);
	SetLed(3, calendar1.min/10); //��ʾ ��
	delay_ms(1);
	SetLed(4, calendar1.min%10);
	delay_ms(1);
	SetLed(5, 16);        //��ʾ"-"����
	delay_ms(1);
	SetLed(6, calendar1.sec/10); //��ʾ ��
	delay_ms(1);
	SetLed(7, calendar1.sec%10);
	delay_ms(1);
}
	

/*�����ʾģ��*/
void DisplayStopwatch(s8 ten_ms, s8 second, s8 minute)
{
	LED_SEL = 0;
	SetLed(0, minute / 10);  //��ʾ ��
	delay_ms(1);
	SetLed(1, minute % 10);
	delay_ms(1);
	SetLed(2, 16);        //��ʾ"-"����
	delay_ms(1);
	SetLed(3, second / 10); //��ʾ ��
	delay_ms(1);
	SetLed(4, second % 10);
	delay_ms(1);
	SetLed(5, 16);        //��ʾ"-"����
	delay_ms(1);
	SetLed(6, ten_ms / 10); //��ʾ 10ms
	delay_ms(1);
	SetLed(7, ten_ms % 10);
	delay_ms(1);
}

/*����ʱ�����ʾģ��*/
void DisplayDigitalClock(s8 hour, s8 minute, s8 second, u16 i, u8 temp)
{
	if(set == 1)
	{
		LED_SEL = 0;
		if(index != 7)
		{
			SetLed(0, temp_hour/10);  //��ʾ ʱ
			delay_ms(1);
		}
		if(index != 6)
		{
			SetLed(1, temp_hour%10);
			delay_ms(1);
		}
		SetLed(2, 16);        //��ʾ"-"����
		delay_ms(1);
		if(index !=  4)
		{
			SetLed(3, temp_minute/10); //��ʾ ��
			delay_ms(1);
		}
		if(index != 3)
		{
			SetLed(4, temp_minute%10);
			delay_ms(1);
		}					
		SetLed(5, 16);        //��ʾ"-"����
		delay_ms(1);
		if(index != 1)
		{
			SetLed(6, temp_second/10); //��ʾ ��
			delay_ms(1);
		}
		if(index != 0)
		{
			SetLed(7, temp_second%10);
			delay_ms(1);
		}
		if(i > 15 && i <= 30)
		{
			switch(index)
			{
				case 0:
					SetLed(7, temp);
					delay_ms(1);
					break;
				case 1:
					SetLed(6, temp);
					delay_ms(1);
					break;
				case 3:
					SetLed(4, temp);
					delay_ms(1);
					break;
				case 4:
					SetLed(3, temp);
					delay_ms(1);
					break;
				case 6:
					SetLed(1, temp);
					delay_ms(1);
					break;
				case 7:
					SetLed(0, temp);
					delay_ms(1);
					break;
				default:
					break;
			}
			//SetLed(7 - index, temp);
			//delay_us(100);
		}
	}
	else
	{
		LED_SEL = 0;
		SetLed(0, hour/10);  //��ʾ ʱ
		delay_ms(1);
		SetLed(1, hour%10);
		delay_ms(1);
		SetLed(2, 16);        //��ʾ"-"����
		delay_ms(1);
		SetLed(3, minute/10); //��ʾ ��
		delay_ms(1);
		SetLed(4, minute%10);
		delay_ms(1);
		SetLed(5, 16);        //��ʾ"-"����
		delay_ms(1);
		SetLed(6, second/10); //��ʾ ��
		delay_ms(1);
		SetLed(7, second%10);
		delay_ms(1);
	}

}

/*����������ʾģ��*/
void DisplayNavKey(u16 adcNavKey, u16 adcNav)
{
	LED_SEL = 0;
	SetLed(0,adcNavKey);
	delay_ms(1);
	SetLed(4,adcNav/1000);
	delay_ms(1);
	SetLed(5,adcNav%1000/100);
	delay_ms(1);
	SetLed(6,adcNav%100/10);
	delay_ms(1);
	SetLed(7,adcNav%10);
	delay_ms(1);
}

/*������ʾģ��*/
void DisplaySteps(u16 steps)
{
	LED_SEL = 0;
	SetLed(3, steps / 10000);
	delay_ms(1);
	SetLed(4, (steps % 10000) / 1000);
	delay_ms(1);
	SetLed(5, (steps % 1000) / 100);
	delay_ms(1);
	SetLed(6, (steps % 100) / 10);
	delay_ms(1);
	SetLed(7, steps % 10);
	delay_ms(1);
}

/*��·����ʾģ��*/
void DisplayCalories(u16 steps)
{
	int calories = (steps * 28);
	LED_SEL = 0;
	SetLed(2, calories / 100000);
	delay_ms(1);
	SetLed(3, (calories % 100000) / 10000);
	delay_ms(1);
	SetLed(4, (calories % 10000) / 1000);
	delay_ms(1);
	SetLed(5, (calories % 1000) / 100);
	delay_ms(1);
	SetLed(6, (calories % 100) / 10);
	delay_ms(1);
	SetLed(7, calories % 10);
	delay_ms(1);
}

























