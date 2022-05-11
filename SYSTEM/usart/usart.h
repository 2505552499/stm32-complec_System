#ifndef __USART_H
#define __USART_H
#include <stm32f10x_map.h>
#include <stm32f10x_nvic.h>	 
#include "timer.h"
#include "exti.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
////////////////////////////////////////////////////////////////////////////////// 	  
 
	  	
extern u8 USART_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_STA;         //����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_USART1_RX //ʹ�ܴ���1����
void uart_init(u32 pclk2,u32 bound);
void SetTime(s8 _hour, s8 _minute, s8 _second);
void SetDate(s16 year, s8 month, s8 day, s8 hour, s8 minute, s8 second);

#endif	   
















