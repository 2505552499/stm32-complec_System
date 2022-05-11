/******************************ADCͷ�ļ�************************
* ���ߣ�������
******************************************************************/

#ifndef _ADC_H
#define _ADC_H

#include "sys.h"
#include "led.h"
#include "delay.h"

#define PA0 PAin(0)
#define PA1 PAin(1)

#define V_Ro PCin(4) //���
#define V_Rt PCin(5) //����


void VoltageAdcInit(void); //�˿ڳ�ʼ����ADC1��ʼ����ADC2��ʼ��
void VoltageAdc1Init(void); //��ѹ����ADC1��ʼ��
void VoltageAdc2Init(void); //��ѹ����ADC2��ʼ��
float GetVoltage(u8 adcx, u8 ch); //��ȡ��ѹֵ
void VoltageDisplay(u8 adcx, float vol); //��ʾ��ѹֵ
u8 GetNavKey(u16 ch);	//��ȡ��������3λֵ
u16 GetAdc(u8 adcx, u8 ch); //��ȡADC
void UltrasonicAdcInit(void);//����������ADC��ʼ��
u16 GetUltrasonicAdc(u8 ch);
void AdcDescen(void); //���ģʽת��ͨ��14��15��16��10
void adcScan(void); //���¸�ADCֵ 
void AdcInit(void); //�˿ڣ�ADC1��ʼ����ͨ��10��14��15��16
u16 GetTemperatureAdc(u8 ch);
float GetTemperature(void);
float GetInnerTemperature(void);
u16 GetLightAdc(u8 ch);

/*****************ȫ�ֱ���*********************/
extern float adcTemperature; //�ⲿ�¶�
extern float adcInnerTemperature; //CPU�¶�
extern u16 adcNavKey; //��������ֵ 7�ް���
extern u16 adcLight; //����ֵ
extern u16 adcNav;

#endif

