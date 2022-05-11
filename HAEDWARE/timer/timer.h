/****************��ʱ��********************
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


void TimerxInit(u16 arr, u16 psc); //��ʱ����ʼ��
void Timer2Init(u16 arr, u16 psc); //TIM2��ʱ����ʼ��
void Timer4Init(u16 arr, u16 psc); //TIM4��ʱ����ʼ��
u32 Rtc_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec); //����������ʱ�������rtc�е�32λֵ
u8 Rtc_Get(u32 rtcValue); //����rtcֵ����������ʱ�������ڴ浽calendar1�ṹ����
u8 Rtc_Get_Week(u16 year, u8 month, u8 day); //���������ռ�������


/***********ȫ�ֱ���************/
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



