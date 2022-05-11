#ifndef __RTC_H
#define __RTC_H

#include "sys.h"
#include "delay.h"

typedef struct Calendar{
	u16 w_year;
	u16 w_month;
	u32 w_date;
	u32 hour;
	u32 min;
	u32 sec;
	u8 week;
}Calendar;

u8 Rtc_Init(void);
u8 Is_Leap_Year(u16 year);
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);
u8 RTC_Get(void);
u8 RTC_Get_Week(u16 year, u8 month, u8 day);

//extern Calendar calendar;
extern const u8 table_week[12];
extern const u8 mon_table[12];



#endif
