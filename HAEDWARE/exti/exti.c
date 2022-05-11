#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "adc.h"
#include "timer.h"
#define uchar unsigned char
//////////////////////////////////////////////////////////////////////////////////	 
//�ⲿ�ж� ��������			   
////////////////////////////////////////////////////////////////////////////////// 	  
int count = 0;
s8 stopWatch_num = 0;
u16 steps = 0;
u16 calories = 0;
s8 menu = 1;  //�˵�
s8 page = 1;  //ҳ��
s8 stopPage = 0;
u8 set = 0;	   //����λ
u8 index = 0;
uchar show_w1 = 0;
uchar show_w2 = 0;
uchar show_w3 = 0;

//�ⲿ�ж�0�������
/*
void EXTI0_IRQHandler(void)
{
	//adcNavKey = GetNavKey(10);
	//delay_ms(10);//����		 
	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ  	  PC0
}
*/

//�ⲿ�ж�1�������
void EXTI1_IRQHandler(void)
{
	delay_ms(10);//����
	if (KEY2 == 0) 
	{
		switch(menu)
		{
			case 1:
				switch(page)
				{
					case 1:
						break;
					default:
						break;
				}
				break;
			case 2:
				if(page == 2)
				{			  
					if(set == 1) 
					{
						temp_hour = 0;
						temp_minute = 0;
						temp_second = 0;
						set = 0;
					}
				}
				if(page == 3)
				{
				 	if(timing_pause == 0)
					{
					 ten_ms = 0;
					 second_2 = 0;
					 minute_2 = 0;
					}
				}
				break;
			case 3:
				break;
			default:
				break;
		}
	} 		 
	EXTI->PR=1<<1;  //���LINE1�ϵ��жϱ�־λ  	 PC1
}



//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	
	delay_ms(10);//����
	if(KEY1==0)	 //����1
	{
		switch(menu)
		{
			case 1:
				switch(page)
				{
					case 1:
						break;
					default:
						break;
				}
				break;
			case 2:
				if(page == 2)
				{			  
					if(set == 1) 
					{
						hour_1 = temp_hour;
						minute_1 = temp_minute;
						second_1 = temp_second;
						set = 0;
					}
					else
					{
						tag_down = tag_down == 0 ? 1 : 0;	//��ʼ��ͣ����ʱ
					}
				}
				if(page == 3)
				{
					timing_pause = timing_pause == 0 ? 1 : 0; //��ʼ��ͣ��ʱ	
				}
				break;
			case 3:
				break;
			default:
				break;
		}
	}		 
	EXTI->PR=1<<2;  //���LINE2�ϵ��жϱ�־λ  		  PC2
}

//�ⲿ�ж�9-5�������
void EXTI9_5_IRQHandler(void)
{
	delay_ms(30);
	if(REVERSE == 1)
	{
		steps++;
	}
	EXTI->PR=1<<5; //���LINE5�ϵ��жϱ�־λ PC5
}


void NavKey(u16 adcNavKey)
{
 	switch(adcNavKey)
	{
		case 0:	//K3
		
			if(menu == 2 && page == 2)
			{
				set = 1;
				index = 0;
				temp_hour = hour_1;
				temp_minute = minute_1;
				temp_second = second_1;
				 
			}
			if(menu == 2 && page == 3)
			{
				if(ten_ms || second_2 || minute_2)
			 	{
			  		stopWatch[stopWatch_num].ten_ms = ten_ms;
					stopWatch[stopWatch_num].second = second_2;
					stopWatch[stopWatch_num].minute = minute_2;
					stopWatch_num++;
					if(stopWatch_num == 5)
						stopWatch_num = 0;
			 	}
			}
			
			break; 
		case 1:	//��
			if(set == 1)
			{
				if(menu == 2 && page == 2)
				{
					if(index > 0)
						index--;
				}
			}
			else if(menu == 2 && page == 3)
			{
				stopPage++;
				if(stopPage == 6)
					stopPage = 0;	
			}
			else
			{
				menu++;
				page = 1;
				if(menu == 5)
					menu = 0;
			}
			break; 
		case 2:	//��
			if(set == 1)
			{
				if(menu == 2 && page == 2)
				{
					switch(index)
					{
						case 0:
							temp_second--;
							if(temp_second % 10 == 9)
								temp_second += 10;
							break;
						case 1:
							temp_second -= 10;
							if(temp_second < 0)
								temp_second += 60;
							break;
						case 2:
							break;
						case 3:
							temp_minute--;
							if(temp_minute % 10 == 9)
							 	temp_minute += 10;
							break;
						case 4:
							temp_minute -= 10;
							if(temp_minute < 0)
								temp_minute += 60;
							break;
						case 5:
							break;
						case 6:
							temp_hour--;
							if(temp_hour == 19)
							 temp_hour += 4;
							if(temp_hour % 10 == 9)
								temp_hour += 10;
							break;
						case 7:
							temp_hour -= 10;
							if(temp_hour < 0 )
								temp_hour = 0;
							break;
						default:
							break;
					}
			 	}
			}
			else
			{
				page++;
				switch(menu)
				{
				 	case 0:
						if(page == 3)
							page = 0;
						break;
					case 1:
						if(page == 4)
							page = 0;
						break;
					case 2:
						if(page == 4)
							page = 0;
						break;
					case 3:
						if(page == 3)
							page = 0;
						break;
					case 4:
						if(page == 3)
							page = 0;
						break;
					case 5:
						if(page == 3)
							page = 0;
						break;
					default:
						break;
				}
			} 
			break;
		case 3:	//��
			menu = 1;
			page = 1;
			break; 
		case 4: //��
			if(set == 1)
			{
				if(menu == 2 && page == 2)
				{ 
					if(index < 7)
						index++;
				}
			}
			else if(menu == 2 && page == 3)
			{
				stopPage--;	
				if(stopPage < 0)
					stopPage = 5;
			}
			else
			{
				menu--;
				page = 1;
				if(menu == -1)
					menu = 4;
			}
			break; 
		case 5:	//��
			if(set == 1)
			{
				if(menu == 2 && page == 2)
				{
					switch(index)
					{
						case 0:
							temp_second++;
							if(temp_second % 10 == 0)
								temp_second -= 10;
							break;
						case 1:
							temp_second += 10;
							if(temp_second > 60)
								temp_second -= 60;
							break;
						case 2:
							break;
						case 3:
							temp_minute++;
							if(temp_minute % 10 == 0)
							 	temp_minute -= 10;
							break;
						case 4:
							temp_minute += 10;
							if(temp_minute > 60)
								temp_minute -= 60;
							break;
						case 5:
							break;
						case 6:
							temp_hour++;
							if(temp_hour > 23)
								 temp_hour -= 4;
							if(temp_hour % 10 == 0)
								temp_hour -= 10;
							break;
						case 7:
							temp_hour += 10;
							if(temp_hour > 23)
								temp_hour = 23;
							break;
						default:
							break;
					}
				}	
			}
			else
			{
				page--;
				switch(menu)
				{
				 	case 0:
						if(page == -1)
							page = 2;
						break;
					case 1:
						if(page == -1)
							page = 3;
						break;
					case 2:
						if(page == -1)
							page = 3;
						break;
					case 3:
						if(page == -1)
							page = 2;
						break;
					case 4:
						if(page == -1)
							page = 2;
						break;
					case 5:
						if(page == -1)
							page = 2;
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
}



//�ⲿ�жϳ�ʼ������
//��ʼ��PC0-2Ϊ�ж�����.
void EXTIX_Init(void)
{
	RCC->APB2ENR|=1<<4;     //ʹ��PORTCʱ��
//	RCC->APB2ENR |= 1<<3;	//����portBʱ��
	GPIOC->CRL&=0XFFFFF00F;//PC1-2���ó�����	  
	GPIOC->CRL|=0X00000880;
//	GPIOB->CRL &= 0xff0ffff;
//	GPIOB->CRL |= 0x00800000; 	//PB.5����
//	GPIOB->ODR |= 0x00000020;	//��������

	
//	Ex_NVIC_Config(GPIO_C,0,FTIR);//�½��ش���
	Ex_NVIC_Config(GPIO_C,1,FTIR);//�½��ش���
	Ex_NVIC_Config(GPIO_C,2,FTIR);//�½��ش���
	Ex_NVIC_Config(GPIO_B,5,FTIR);
//	Ex_NVIC_Config(GPIO_B,5,RTIR);//�����ش���
//	Ex_NVIC_Config(GPIO_B,5,3);

//	MY_NVIC_Init(2,2,EXTI0_IRQChannel,2);//��ռ2�������ȼ�2����2
	MY_NVIC_Init(2,1,EXTI1_IRQChannel,2);//��ռ2�������ȼ�1����2	   
	MY_NVIC_Init(2,0,EXTI2_IRQChannel,2);//��ռ2�������ȼ�0����2
	MY_NVIC_Init(2,2,EXTI9_5_IRQChannel,2);//��ռ2�������ȼ�0����2

	//MY_NVIC_Init(1,3,ADC1_2_IRQChannel,2);
}












