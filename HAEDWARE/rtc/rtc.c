
#include "rtc.h"



Calendar calendar;

					  



u8 Rtc_Init(void)
{
	u8 temp = 0;
	if(BKP->DR1 != 0x5050)	//第一次配置
	{
		RCC->APB1ENR |= 1<<28; //使能电源时钟
		RCC->APB1ENR |= 1<<27; //使能备份时钟
		PWR->CR |= 1<<8; //取消备份区写保护
		RCC->BDCR |= 1<<16; //备份区域软复位
		RCC->BDCR &= ~(1<<16); //备份区域软复位结束
		RCC->BDCR |= 1<<0; //开启外部低速震荡器
		while((!(RCC->BDCR & 0x02)) && temp < 250)
		{
			temp++;
			delay_ms(10);
		};
		if(temp >= 250)
			return 1; //初始化时钟失败，晶振有问题
		RCC->BDCR |= 1<<8; //LSE作为RTC时钟
		RCC->BDCR |= 1<<15; //RTC时钟使能
		while(!(RTC->CRL & (1<<5))); //等待RTC寄存器操作完成
		while(!(RTC->CRL & (1<<3))); //等待RTC寄存器同步
		RTC->CRH |= 0x01; //允许秒中断
		while(!(RTC->CRL & (1<<5))); //等待RTC寄存器操作完成
		RTC->CRL |= 1<<4; //允许配置
		RTC->PRLH = 0x0000;
		RTC->PRLL = 32767; //时钟周期设置 理论值：32767
		RTC_Set(2022, 4, 24, 17, 53, 55); //设置时间
		RTC->CRL &= ~(1<<4); //配置更新
		while(!(RTC->CRL & (1<<5))); //等待RTC寄存器操作完成
		BKP->DR1 = 0x5050;
//		printf("FIRST TIME\n");
	}
	else
	{
		while(!(RTC->CRL & (1<<3))); //等待RTC寄存器同步
		RTC->CRH |= 0x01; //允许秒中断
		while(!(RTC->CRL & (1<<5))); //等待RTC寄存器操作完成
//		pritnf("OK!\n");
	}
	MY_NVIC_Init(0, 0, RTC_IRQChannel, 2); //优先级设置
	RTC_Get(); //更新时间
	return 0; //ok
}

void RTC_IRQHandler(void)
{
	if(RTC->CRL & 0x0001) //秒钟中断
	{
		RTC_Get(); //更新时间
	}
	if(RTC->CRL & 0x0002) //闹钟中断
	{
		RTC->CRL &= ~(0x0002); //清闹钟中断
	}
	RTC->CRL &= 0x0ffa; //清楚溢出，秒钟中断标志
	while(!(RTC->CRL & (1<<5))); //等待RTC寄存器操作完成
}
	


//判断是否是闰年
//月份    1  2  3  4  5  6  7  8  9 10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//year: 年份
//返回值：该年份是不是闰年 1：是 0：不是
u8 Is_Leap_Year(u16 year)
{
	if(year % 4 == 0)
	{
		if(year % 100 == 0)
		{
			if(year % 400 == 0) 
				return 1;
			else 
				return 0;
		}
		else 
			return 1;
	}
	else 
		return 0;
}

//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值：0：成功 其他：错误代码
//月份数据表
const u8 table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //月修正数据表
//平年的月份日期表
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//syear, smon, sday, hour, min, sec
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
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

	//设置时钟
	RCC->APB1ENR |= 1<<28; //使能电源时钟
	RCC->APB1ENR |= 1<<27; //使能备份时钟
	PWR->CR |= 1<<8; //取消备份区写保护
	
	RTC->CRL |= 1<<4; //允许配置
	RTC->CNTL = seccount & 0xffff;
	RTC->CNTH = seccount >> 16;
	RTC->CRL &= ~(1<<4); //配置更新
	while(!(RTC->CRL & (1<<5))); //等待RTC寄存器操作完成
	RTC_Get(); //设置完成之后更新一下数据
	return 0;
}

//得到当前的时间， 结果保存在calendar结构体里面
//返回值 0：成功
u8 RTC_Get(void)
{
	static u16 daycnt = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp1 = 0;
	timecount = RTC->CNTH; //得到计数器中的值（高16位）
	timecount <<= 16;
	timecount += RTC->CNTL;

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
		calendar.w_year = temp1; //得到年份
		temp1 = 0;
		while(temp >= 28) //超过了一个月
		{	 	
			if(Is_Leap_Year(calendar.w_year) && temp1 == 1) //当年是不是闰年/2月份
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
		calendar.w_month = temp1 + 1;  //得到月份
		calendar.w_date = temp + 1; //得到日期
	}
	temp = timecount % 86400; //对计算到的秒数对一天的秒数取余，得到不够一天的秒数
	calendar.hour = temp / 3600; //小时
	calendar.min = (temp % 3600) / 60; //分钟
	calendar.sec= (temp % 3600) % 60; //秒钟
	calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); //获取星期
	return 0;
}

//获取星期
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
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


			










































