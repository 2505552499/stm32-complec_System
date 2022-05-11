/****************定时器实现********************
* 
* 作者：宁晓兰
***************************************************/

#include "timer.h"
#include "buzzer.h"

Calendar calendar1;
u32 Rtc_Value;
StopWatch stopWatch[5]; 



//数字钟的时，分、秒
s8 hour = 0, minute = 0, second = 0;
s8 ten_ms = 0, second_2 = 0, minute_2 = 0;
s8 hour_1 = 0, minute_1 = 0, second_1 = 20;
s8 temp_hour = 0, temp_minute = 0, temp_second = 0;
u8 tag_down = 0, timing_pause; //倒计时位和计时位

/****************普通按键初始化函数********************
* 通用定时器中断初始化
* 这里时钟选择为APB1的2倍，而APB1为36M
* arr：自动重装值。
* psc：时钟预分频数
* 这里使用的是定时器3!
******************************************************/
void TimerxInit(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1<<1; //TIM3时钟使能
	TIM3->ARR = arr;      //设定计数器自动重装值9999，10为1ms	10000为1s
//	TIM3->ARR = arr;	  //设定计数器自动重装值99， 10为1ms	100为0.01s
	TIM3->PSC = psc;      //预分频器7200，得到10KHZ的计数时钟
	
	TIM3->DIER |= 1<<0;   //允许更新中断
	TIM3->CR1 |= 0x01;    //使能定时器3

	
	
	MY_NVIC_Init(1, 3, TIM3_IRQChannel, 2); //抢占1，子优先级3，组2
}

void Timer2Init(u16 arr, u16 psc)
{
	Rtc_Value = Rtc_Set(2022, 4, 27, 19, 37, 22);
	Rtc_Get(Rtc_Value);
	RCC->APB1ENR |= 1<<0;  //TIM2时钟使能
	TIM2->ARR = arr;      //设定计数器自动重装值，10为1ms
	TIM2->PSC = psc;      //预分频器7200，得到10KHZ的计数时钟
	
	TIM2->DIER |= 1<<0;   //允许更新中断
	TIM2->CR1 |= 0x01;    //使能定时器3
	MY_NVIC_Init(1, 3, TIM2_IRQChannel, 2); //抢占1，子优先级3，组2
}

void Timer4Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1<<2; //TIM4时钟使能
//	TIM3->ARR = arr;      //设定计数器自动重装值9999，10为1ms	10000为1s
	TIM4->ARR = arr;	  //设定计数器自动重装值99， 10为1ms	100为0.01s
	TIM4->PSC = psc;      //预分频器7200，得到10KHZ的计数时钟
	
	TIM4->DIER |= 1<<0;   //允许更新中断
	TIM4->CR1 |= 0x01;    //使能定时器4

	
	
	MY_NVIC_Init(1, 3, TIM4_IRQChannel, 2); //抢占1，子优先级3，组2
}



/****************定时器3的中断函数********************
* 定时器3的中断函数
* 每次中断，second加一
******************************************************/

void TIM2_IRQHandler( void )
{
	
	if( TIM2->SR & 0x0001) //溢出(更新)中断
	{
		Rtc_Value++;
		Rtc_Get(Rtc_Value); //更新时间	
	}
	
	TIM2->SR &= ~(1<<0); //清除中断标志位
}


void TIM3_IRQHandler( void )
{
	
	if( TIM3->SR & 0x0001) //溢出(更新)中断
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
			Buzzer_s(3);//开启蜂鸣器警报3s
		}
		}
	}
	TIM3->SR &= ~(1<<0); //清除中断标志位
}

void TIM4_IRQHandler( void )
{
	
	if( TIM4->SR & 0x0001 && timing_pause == 1 && stopPage == 0) //溢出(更新)中断
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
	TIM4->SR &= ~(1<<0); //清除中断标志位
}

u32 Rtc_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
	u16 t;
	u32 seccount = 0;
	if(syear < 1970 || syear > 2099) 
		return 1;
	for(t = 1970; t < syear; t++) //把所有年份的秒钟相加
	{
		if(Is_Leap_Year(t))
			seccount += 31622400; //闰年的秒钟数
		else
			seccount += 31536000; //平年的秒钟数
	}
	smon -= 1;
	for(t = 0; t < smon; t++) //把前面月份的秒钟数相加
	{
		seccount += (u32)mon_table[t] * 86400; //月份m秒钟数相加
		if(Is_Leap_Year(syear) && t == 1)
			seccount += 86400; //闰年2月份增加一天的秒数
	}
	seccount += (u32)(sday - 1) * 86400; //	把前面日期的秒钟数相加
	seccount += (u32)hour * 3600; //小时秒钟数
	seccount += (u32)min * 60; //分钟秒钟数
	seccount += sec; //最后的秒钟数相加
	return seccount;
}

u8 Rtc_Get(u32 rtcValue)
{
	static u16 daycnt = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp1 = 0;
	timecount = rtcValue;

	temp = timecount / 86400; //得到天数（一天的秒数）
	if(daycnt != temp) //超过一天了
	{
		daycnt = temp;
		temp1 = 1970; //从1970年开始
		while(temp >= 365)
		{
			if(Is_Leap_Year(temp1)) //是闰年
			{
				if(temp >= 366)
					temp -= 366;
				else
					break;
			}
			else temp -= 365; //平年
			temp1++;
		}
		calendar1.w_year = temp1; //得到年份
		temp1 = 0;
		while(temp >= 28) //超过了一个月
		{	 	
			if(Is_Leap_Year(calendar1.w_year) && temp1 == 1) //当年是不是闰年/2月份
			{
				if(temp >= 29) 
					temp -= 29;
				else
					break;
			}
			else
			{
				if(temp >= mon_table[temp1])
					temp -= mon_table[temp1]; //平年
				else
					break;
			}
			temp1++;
		}
		calendar1.w_month = temp1 + 1;  //得到月份
		calendar1.w_date = temp + 1; //得到日期
	}
	temp = timecount % 86400; //对计算到的秒数对一天的秒数取余，得到不够一天的秒数
	calendar1.hour = temp / 3600; //小时
	calendar1.min = (temp % 3600) / 60; //分钟
	calendar1.sec= (temp % 3600) % 60; //秒钟
	calendar1.week = Rtc_Get_Week(calendar1.w_year, calendar1.w_month, calendar1.w_date); //获取星期
	return 0;
}

//获取星期
u8 Rtc_Get_Week(u16 year, u8 month, u8 day)
{
	u16 temp2;
	u8 yearH, yearL;

	yearH = year / 100;
	yearL = year % 100;
	if(yearH > 19)
		yearL += 100;
	//所过闰年数只算1900年之后的
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if(yearL % 4 == 0 && month < 3)
		temp2--;
	return temp2 % 7;
}

