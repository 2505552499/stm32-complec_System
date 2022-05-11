/******************************PRESENT头文件************************
* 作者：邹加新
******************************************************************/

#ifndef _PRESENT_H
#define _PRESENT_H

#include "sys.h"
#include "led.h"
#include "delay.h"
#include "exti.h"

#define PA0 PAin(0)
#define PA1 PAin(1)

#define V_Ro PCin(4) //测光
#define V_Rt PCin(5) //测温

/****************全局变量****************************/


/*****************函数***********************/
void Display(void);
void DisplayTemperature(float adcTemperature); //温度显示模块
void DisplayLight(u16 adcLight); //光照显示模块
void DisplayDigitalClock(s8 hour, s8 minute, s8 second, u16 i, u8 temp); //时间显示模块
void DisplayStopwatch(s8 ten_ms, s8 second, s8 minute); //秒表显示模块
void DisplayNavKey(u16 adcNavKey, u16 adcNav); //导航值显示
void DisplayPage0(u16 totalPage); //第0页展示
void DisplaySteps(u16 steps); //步数展示
void DisplayCalories(u16 steps); //卡路里展示
void DisplayDate(void); //时间显示模块（时分秒)
void DisplayCalendar(void); //年月日显示模块
void DisplayWeek(void); //星期显示模块
void DisplayMenuState(void); //所处菜单位置状态
void DisplayStopState(void); //秒表所处位置状态

#endif

