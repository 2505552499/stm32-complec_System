#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//�ⲿ�ж� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/01  
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  

#define REVERSE  PBin(5)
/***************ȫ�ֱ���******************/
extern int count;
extern s8 menu;
extern s8 stopPage;
extern s8 page;
extern u16 steps;
extern u8 index;
extern u8 set;
//extern u16 calories;

					    
/****************************/	 
void EXTIX_Init(void);//IO��ʼ��
void NavKey(u16 adcNavKey);
		 					    
#endif

























