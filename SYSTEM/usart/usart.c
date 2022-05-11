#include "usart.h"
#include "sys.h"


//********************************************************************************
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//********************************************************************************
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 									 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO״̬����
		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
#ifdef EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//��2��������ȼ� 
#endif
}




#ifdef EN_USART1_RX //���ʹ���˽��� 	
u8 USART_RX_BUF[64]; //���ջ���,���64���ֽ�.
s16 paraArray[10]; //���ղ���������
u8 instrNum = 4; //��ָ����
char instrArray[][10] = {"Help", "SetTime", "Quit", "SetDate"}; //ָ�
s8 instrIndex = -1; //ָ���±�
u8 paraGet = 0; //�Ƿ���ղ���
u8 t; //ѭ������
u8 len; //���յ������ݳ���
u8 paraNum; //���յ��Ĳ�������
char *dest_str; //


u8 SizeOf(char dest[])
{
	u8 i = 0;
	while(dest[i++] != '\0');
	return i - 1;
}

//�ж��ַ����Ƿ���ȣ������ִ�Сд
u8 isEqual(u8 Usart[], char dest[], u8 len)
{
	u8 temp = SizeOf(dest);
	u8 i = 0;
//	printf("sizeof:%d\r\n", temp);
	if(temp != len) 
		return 0;
	for(i = 0; i < len; i++)
	{
		if(Usart[i] > 64 && Usart[i] < 91)
		{
		  if(Usart[i] != dest[i] && Usart[i] + 32 != dest[i])
			return 0;
		}
		if(Usart[i] >96 && Usart[i] < 123)
		{
			if(Usart[i] != dest[i] && Usart[i] - 32 != dest[i])
			return 0;
		}
		
	}
	return 1;
} 

//����ֵΪ��������	 2014 05 26
u8 getParaNum(u8 Usart[], u8 len)
{
	u16 sum = 0; 
	u8 i = 0, cnt = 0;
	for( i = 0; i < len; ++i)
	{
		if(USART_RX_BUF[i] != 32)
			sum = sum * 10 + USART_RX_BUF[i] - 48;
		else 
		{
			if(USART_RX_BUF[i - 1] != 32)
			{
				paraArray[cnt++] = sum;	
				sum = 0;
			}
		}
		if(i == len - 1)
		{
			paraArray[cnt++] = sum;
			sum = 0;
		}
	}
	return cnt;		
}

void SetTime(s8 _hour, s8 _minute, s8 _second)
{
	hour = _hour;
	minute = _minute;
	second = _second;
}	

void SetDate(s16 year, s8 month, s8 day, s8 hour, s8 minute, s8 second)
{
	Rtc_Value = Rtc_Set(year, month, day, hour, minute, second);
	Rtc_Get(Rtc_Value);
}

//����1�жϷ������ 
u8 USART_RX_STA=0;       //����״̬���	 
void USART1_IRQHandler(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x80)==0)//����δ���
		{
			if(USART_RX_STA&0x40)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x80;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x40;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3F]=res;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  
		
		if(USART_RX_STA&0x80)
		{
			count = 0;
			len	= USART_RX_STA&0x3f;
//			printf("len:%d\r\n", len);
			if(isEqual(USART_RX_BUF, instrArray[2], len))	//Quitָ������ȼ����
				paraGet = 0;
			if(paraGet == 0)
			{	
					
				for(t = 0; t < instrNum; t++)
				{
					if(isEqual(USART_RX_BUF, instrArray[t], len))
						{
							instrIndex = t;
					//		printf("t:%d\r\n", t);
					//		printf("���\r\n");
							printf("��ʼִ��ָ�%s\r\n", instrArray[t]);
							break;
						}
					if(t == instrNum - 1)
					{
						printf("�������ָ��Ϸ�!\r\n");
						printf("������Help�鿴��ذ���\r\n");
					}
				}
			}
			switch(instrIndex)
			{
				case 0:
					 printf("��ϵͳ����5���˵���ÿ���˵��в�ͬ��ҳ��******\r\n");
					 printf("����ͨ������ָ����������Ӧ����***************\r\n");
					 printf("SetTime----------------------���ü�ʱ����ʱ��\r\n");
					 printf("Qui----------��ֹ��Ҫ���ε�ָ��ص���ʼ״̬\r\n");
					 printf("SetDate--------------------------���õ�ǰ����\r\n");
					 printf("=============================================\r\n");

					 instrIndex = -1; //ִ�����ָ����
					break;
				case 1:
					if(paraGet == 0)
					{
						printf("������ʱ���룬�Կո����\r\n");
						printf("������23 02 11\r\n");
						paraGet = 1; //��ʼ��ȡ����
					}
					else
					{
						paraNum = getParaNum(USART_RX_BUF, len);
						printf("���յ�����\r\n");
						if(paraNum != 3)
						{
							printf("�������������ƥ�䣬����������\r\n");
						}
						else
						{
							if(paraArray[0] >= 0 && paraArray[1] < 24 && paraArray[1] >= 0 && paraArray[1] < 60 && paraArray[2] > 0 && paraArray[2] <= 60)
							{
								paraGet = 0; //������ȡ���
								SetTime(paraArray[0], paraArray[1], paraArray[2]);
								printf("%s ָ��ִ��ָ�����\r\n", instrArray[instrIndex]);
								instrIndex = -1; //ִ�����ָ����
							}
							else 
							printf("����Ĳ������Ϸ�,����������\r\n");
						}
					}
					break;
				case 2:
					paraGet = 0;
					printf("%s ָ��ִ��ָ�����\r\n", instrArray[instrIndex]);
					instrIndex = -1;
					break;
				case 3:
					if(paraGet == 0)
					{
						printf("������������ʱ���룬�Կո����\r\n");
						printf("������2022 5 1 12 02 11\r\n");
						paraGet = 1; //��ʼ��ȡ����
					}
					else
					{
						paraNum = getParaNum(USART_RX_BUF, len);
						printf("���յ�����\r\n");
						if(paraNum != 6)
						{
							printf("�������������ƥ�䣬����������\r\n");
						}
						else
						{
							if(paraArray[0] > 1970 && paraArray[0] < 2099 && paraArray[1] > 0 && paraArray[1] < 13 && paraArray[2] > 0 && paraArray[2] < 32
							&& paraArray[3] >= 0 && paraArray[3] < 24 && paraArray[4] >= 0 && paraArray[4] < 60 && paraArray[5] > 0 && paraArray[5] <= 60)
							{
							    paraGet = 0; //������ȡ���
								SetDate(paraArray[0], paraArray[1], paraArray[2], paraArray[3], paraArray[4], paraArray[5]);
								printf("%s ָ��ִ��ָ�����\r\n", instrArray[instrIndex]);
								instrIndex = -1; //ִ�����ָ����
							}
							else 
							printf("����Ĳ������Ϸ�,����������\r\n");
						}
					}
					break;
				default:
					break;
			}
			printf("\r\n");
			USART_RX_STA = 0;
		} 		 									     
	}  											 
} 
#endif

