/******************************ADC头文件************************
* 作者：宁晓兰
******************************************************************/

#ifndef _ADC_H
#define _ADC_H

#include "sys.h"
#include "led.h"
#include "delay.h"

#define PA0 PAin(0)
#define PA1 PAin(1)

#define V_Ro PCin(4) //测光
#define V_Rt PCin(5) //测温


void VoltageAdcInit(void); //端口初始化，ADC1初始化，ADC2初始化
void VoltageAdc1Init(void); //电压测量ADC1初始化
void VoltageAdc2Init(void); //电压测量ADC2初始化
float GetVoltage(u8 adcx, u8 ch); //获取电压值
void VoltageDisplay(u8 adcx, float vol); //显示电压值
u8 GetNavKey(u16 ch);	//获取导航按键3位值
u16 GetAdc(u8 adcx, u8 ch); //获取ADC
void UltrasonicAdcInit(void);//超声波测量ADC初始化
u16 GetUltrasonicAdc(u8 ch);
void AdcDescen(void); //间断模式转换通道14、15、16、10
void adcScan(void); //更新各ADC值 
void AdcInit(void); //端口，ADC1初始化，通道10、14、15、16
u16 GetTemperatureAdc(u8 ch);
float GetTemperature(void);
float GetInnerTemperature(void);
u16 GetLightAdc(u8 ch);

/*****************全局变量*********************/
extern float adcTemperature; //外部温度
extern float adcInnerTemperature; //CPU温度
extern u16 adcNavKey; //导航按键值 7无按下
extern u16 adcLight; //光照值
extern u16 adcNav;

#endif

