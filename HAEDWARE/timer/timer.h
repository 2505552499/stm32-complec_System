/****************定时器********************
* 
*******************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "rtc.h"
#include "exti.h"


typedef struct StopWatch
{
 s8 ten_ms;
 s8 second;
 s8 minute;
}StopWatch;


void TimerxInit(u16 arr, u16 psc); //定时器初始化
void Timer2Init(u16 arr, u16 psc); //TIM2定时器初始化
void Timer4Init(u16 arr, u16 psc); //TIM4定时器初始化
u32 Rtc_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec); //根据年月日时分秒计算rtc中的32位值
u8 Rtc_Get(u32 rtcValue); //根据rtc值计算年月日时分秒星期存到calendar1结构体中
u8 Rtc_Get_Week(u16 year, u8 month, u8 day); //根据年月日计算星期


/***********全局变量************/
extern s8 hour;
extern s8 minute;
extern s8 second;
extern s8 ten_ms, second_2, minute_2;
extern s8 hour_1;
extern s8 minute_1;
extern s8 second_1;
extern u8 tag_down, timing_pause;
extern s8 temp_hour;
extern s8 temp_minute;
extern s8 temp_second;
extern Calendar calendar1;
extern StopWatch stopWatch[5];
extern u32 Rtc_Value;

#endif



