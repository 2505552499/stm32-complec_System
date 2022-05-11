/****************��ʱ��ʵ��********************
* 
* ���ߣ�������
***************************************************/

#include "timer.h"
#include "buzzer.h"

Calendar calendar1;
u32 Rtc_Value;
StopWatch stopWatch[5]; 



//�����ӵ�ʱ���֡���
s8 hour = 0, minute = 0, second = 0;
s8 ten_ms = 0, second_2 = 0, minute_2 = 0;
s8 hour_1 = 0, minute_1 = 0, second_1 = 20;
s8 temp_hour = 0, temp_minute = 0, temp_second = 0;
u8 tag_down = 0, timing_pause; //����ʱλ�ͼ�ʱλ

/****************��ͨ������ʼ������********************
* ͨ�ö�ʱ���жϳ�ʼ��
* ����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
* arr���Զ���װֵ��
* psc��ʱ��Ԥ��Ƶ��
* ����ʹ�õ��Ƕ�ʱ��3!
******************************************************/
void TimerxInit(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1<<1; //TIM3ʱ��ʹ��
	TIM3->ARR = arr;      //�趨�������Զ���װֵ9999��10Ϊ1ms	10000Ϊ1s
//	TIM3->ARR = arr;	  //�趨�������Զ���װֵ99�� 10Ϊ1ms	100Ϊ0.01s
	TIM3->PSC = psc;      //Ԥ��Ƶ��7200���õ�10KHZ�ļ���ʱ��
	
	TIM3->DIER |= 1<<0;   //��������ж�
	TIM3->CR1 |= 0x01;    //ʹ�ܶ�ʱ��3

	
	
	MY_NVIC_Init(1, 3, TIM3_IRQChannel, 2); //��ռ1�������ȼ�3����2
}

void Timer2Init(u16 arr, u16 psc)
{
	Rtc_Value = Rtc_Set(2022, 4, 27, 19, 37, 22);
	Rtc_Get(Rtc_Value);
	RCC->APB1ENR |= 1<<0;  //TIM2ʱ��ʹ��
	TIM2->ARR = arr;      //�趨�������Զ���װֵ��10Ϊ1ms
	TIM2->PSC = psc;      //Ԥ��Ƶ��7200���õ�10KHZ�ļ���ʱ��
	
	TIM2->DIER |= 1<<0;   //��������ж�
	TIM2->CR1 |= 0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1, 3, TIM2_IRQChannel, 2); //��ռ1�������ȼ�3����2
}

void Timer4Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1<<2; //TIM4ʱ��ʹ��
//	TIM3->ARR = arr;      //�趨�������Զ���װֵ9999��10Ϊ1ms	10000Ϊ1s
	TIM4->ARR = arr;	  //�趨�������Զ���װֵ99�� 10Ϊ1ms	100Ϊ0.01s
	TIM4->PSC = psc;      //Ԥ��Ƶ��7200���õ�10KHZ�ļ���ʱ��
	
	TIM4->DIER |= 1<<0;   //��������ж�
	TIM4->CR1 |= 0x01;    //ʹ�ܶ�ʱ��4

	
	
	MY_NVIC_Init(1, 3, TIM4_IRQChannel, 2); //��ռ1�������ȼ�3����2
}



/****************��ʱ��3���жϺ���********************
* ��ʱ��3���жϺ���
* ÿ���жϣ�second��һ
******************************************************/

void TIM2_IRQHandler( void )
{
	
	if( TIM2->SR & 0x0001) //���(����)�ж�
	{
		Rtc_Value++;
		Rtc_Get(Rtc_Value); //����ʱ��	
	}
	
	TIM2->SR &= ~(1<<0); //����жϱ�־λ
}


void TIM3_IRQHandler( void )
{
	
	if( TIM3->SR & 0x0001) //���(����)�ж�
	{
		second++;
		if( second>59 )
		{
			second = 0;
			minute++;
			if( minute>59 )
			{
				minute = 0;
				hour++;
				if( hour>23 )
					hour = 0;
			}
		}
	}
	
	if(TIM3->SR & 0x0001)
	{
		if(tag_down == 1) 
		{
		second_1--;
		if(second_1 < 0 && (minute_1 != 0 || hour_1 != 0))
		{
			if(hour_1 == 0) 
			{
				second_1 = 59;
				minute_1--;
			}
			else
			{
				if(minute_1 == 0)
				{
					hour_1--;
					minute_1 = 59;
					second_1 = 59;
				}
				else 
				{	
					minute_1--;
					second_1 = 59;
				}
			}
		}
		if(second_1 < 0 && minute_1 == 0 && hour_1 == 0)
		{
			second_1 = 0;
			tag_down = 0;  
			Buzzer_s(3);//��������������3s
		}
		}
	}
	TIM3->SR &= ~(1<<0); //����жϱ�־λ
}

void TIM4_IRQHandler( void )
{
	
	if( TIM4->SR & 0x0001 && timing_pause == 1 && stopPage == 0) //���(����)�ж�
	{
		ten_ms++;
		if( ten_ms>99 )
		{
			ten_ms = 0;
			second_2++;
			if( second_2>59 )
			{
				second_2 = 0;
				minute_2++;
				if( minute_2 > 59 )
					minute_2 = 0;
			}
		}
	}
	TIM4->SR &= ~(1<<0); //����жϱ�־λ
}

u32 Rtc_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
	u16 t;
	u32 seccount = 0;
	if(syear < 1970 || syear > 2099) 
		return 1;
	for(t = 1970; t < syear; t++) //��������ݵ��������
	{
		if(Is_Leap_Year(t))
			seccount += 31622400; //�����������
		else
			seccount += 31536000; //ƽ���������
	}
	smon -= 1;
	for(t = 0; t < smon; t++) //��ǰ���·ݵ����������
	{
		seccount += (u32)mon_table[t] * 86400; //�·�m���������
		if(Is_Leap_Year(syear) && t == 1)
			seccount += 86400; //����2�·�����һ�������
	}
	seccount += (u32)(sday - 1) * 86400; //	��ǰ�����ڵ����������
	seccount += (u32)hour * 3600; //Сʱ������
	seccount += (u32)min * 60; //����������
	seccount += sec; //�������������
	return seccount;
}

u8 Rtc_Get(u32 rtcValue)
{
	static u16 daycnt = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp1 = 0;
	timecount = rtcValue;

	temp = timecount / 86400; //�õ�������һ���������
	if(daycnt != temp) //����һ����
	{
		daycnt = temp;
		temp1 = 1970; //��1970�꿪ʼ
		while(temp >= 365)
		{
			if(Is_Leap_Year(temp1)) //������
			{
				if(temp >= 366)
					temp -= 366;
				else
					break;
			}
			else temp -= 365; //ƽ��
			temp1++;
		}
		calendar1.w_year = temp1; //�õ����
		temp1 = 0;
		while(temp >= 28) //������һ����
		{	 	
			if(Is_Leap_Year(calendar1.w_year) && temp1 == 1) //�����ǲ�������/2�·�
			{
				if(temp >= 29) 
					temp -= 29;
				else
					break;
			}
			else
			{
				if(temp >= mon_table[temp1])
					temp -= mon_table[temp1]; //ƽ��
				else
					break;
			}
			temp1++;
		}
		calendar1.w_month = temp1 + 1;  //�õ��·�
		calendar1.w_date = temp + 1; //�õ�����
	}
	temp = timecount % 86400; //�Լ��㵽��������һ�������ȡ�࣬�õ�����һ�������
	calendar1.hour = temp / 3600; //Сʱ
	calendar1.min = (temp % 3600) / 60; //����
	calendar1.sec= (temp % 3600) % 60; //����
	calendar1.week = Rtc_Get_Week(calendar1.w_year, calendar1.w_month, calendar1.w_date); //��ȡ����
	return 0;
}

//��ȡ����
u8 Rtc_Get_Week(u16 year, u8 month, u8 day)
{
	u16 temp2;
	u8 yearH, yearL;

	yearH = year / 100;
	yearL = year % 100;
	if(yearH > 19)
		yearL += 100;
	//����������ֻ��1900��֮���
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if(yearL % 4 == 0 && month < 3)
		temp2--;
	return temp2 % 7;
}

