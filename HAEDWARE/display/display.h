/******************************PRESENTͷ�ļ�************************
* ���ߣ��޼���
******************************************************************/

#ifndef _PRESENT_H
#define _PRESENT_H

#include "sys.h"
#include "led.h"
#include "delay.h"
#include "exti.h"

#define PA0 PAin(0)
#define PA1 PAin(1)

#define V_Ro PCin(4) //���
#define V_Rt PCin(5) //����

/****************ȫ�ֱ���****************************/


/*****************����***********************/
void Display(void);
void DisplayTemperature(float adcTemperature); //�¶���ʾģ��
void DisplayLight(u16 adcLight); //������ʾģ��
void DisplayDigitalClock(s8 hour, s8 minute, s8 second, u16 i, u8 temp); //ʱ����ʾģ��
void DisplayStopwatch(s8 ten_ms, s8 second, s8 minute); //�����ʾģ��
void DisplayNavKey(u16 adcNavKey, u16 adcNav); //����ֵ��ʾ
void DisplayPage0(u16 totalPage); //��0ҳչʾ
void DisplaySteps(u16 steps); //����չʾ
void DisplayCalories(u16 steps); //��·��չʾ
void DisplayDate(void); //ʱ����ʾģ�飨ʱ����)
void DisplayCalendar(void); //��������ʾģ��
void DisplayWeek(void); //������ʾģ��
void DisplayMenuState(void); //�����˵�λ��״̬
void DisplayStopState(void); //�������λ��״̬

#endif

