#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//外部中断 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/01  
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  

#define REVERSE  PBin(5)
/***************全局变量******************/
extern int count;
extern s8 menu;
extern s8 stopPage;
extern s8 page;
extern u16 steps;
extern u8 index;
extern u8 set;
//extern u16 calories;

					    
/****************************/	 
void EXTIX_Init(void);//IO初始化
void NavKey(u16 adcNavKey);
		 					    
#endif

























