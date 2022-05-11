#include "usart.h"
#include "sys.h"


//********************************************************************************
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//********************************************************************************
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 									 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO状态设置
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
#ifdef EN_USART1_RX		  //如果使能了接收
	//使能接收中断
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//组2，最低优先级 
#endif
}




#ifdef EN_USART1_RX //如果使能了接收 	
u8 USART_RX_BUF[64]; //接收缓冲,最大64个字节.
s16 paraArray[10]; //接收参数缓冲区
u8 instrNum = 4; //总指令数
char instrArray[][10] = {"Help", "SetTime", "Quit", "SetDate"}; //指令集
s8 instrIndex = -1; //指令下标
u8 paraGet = 0; //是否接收参数
u8 t; //循环计数
u8 len; //接收到的数据长度
u8 paraNum; //接收到的参数个数
char *dest_str; //


u8 SizeOf(char dest[])
{
	u8 i = 0;
	while(dest[i++] != '\0');
	return i - 1;
}

//判断字符串是否相等，不区分大小写
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

//返回值为参数个数	 2014 05 26
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

//串口1中断服务程序 
u8 USART_RX_STA=0;       //接收状态标记	 
void USART1_IRQHandler(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x80)==0)//接收未完成
		{
			if(USART_RX_STA&0x40)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x80;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x40;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3F]=res;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  
		
		if(USART_RX_STA&0x80)
		{
			count = 0;
			len	= USART_RX_STA&0x3f;
//			printf("len:%d\r\n", len);
			if(isEqual(USART_RX_BUF, instrArray[2], len))	//Quit指令的优先级最高
				paraGet = 0;
			if(paraGet == 0)
			{	
					
				for(t = 0; t < instrNum; t++)
				{
					if(isEqual(USART_RX_BUF, instrArray[t], len))
						{
							instrIndex = t;
					//		printf("t:%d\r\n", t);
					//		printf("相等\r\n");
							printf("开始执行指令：%s\r\n", instrArray[t]);
							break;
						}
					if(t == instrNum - 1)
					{
						printf("您输入的指令不合法!\r\n");
						printf("请输入Help查看相关帮助\r\n");
					}
				}
			}
			switch(instrIndex)
			{
				case 0:
					 printf("本系统共有5个菜单，每个菜单有不同的页数******\r\n");
					 printf("可以通过以下指令来控制相应功能***************\r\n");
					 printf("SetTime----------------------设置计时器的时间\r\n");
					 printf("Qui----------终止需要传参的指令，回到初始状态\r\n");
					 printf("SetDate--------------------------设置当前日期\r\n");
					 printf("=============================================\r\n");

					 instrIndex = -1; //执行完该指令了
					break;
				case 1:
					if(paraGet == 0)
					{
						printf("请输入时分秒，以空格隔开\r\n");
						printf("样例：23 02 11\r\n");
						paraGet = 1; //开始获取参数
					}
					else
					{
						paraNum = getParaNum(USART_RX_BUF, len);
						printf("接收到参数\r\n");
						if(paraNum != 3)
						{
							printf("输入参数个数不匹配，请重新输入\r\n");
						}
						else
						{
							if(paraArray[0] >= 0 && paraArray[1] < 24 && paraArray[1] >= 0 && paraArray[1] < 60 && paraArray[2] > 0 && paraArray[2] <= 60)
							{
								paraGet = 0; //参数获取完毕
								SetTime(paraArray[0], paraArray[1], paraArray[2]);
								printf("%s 指令执行指令完毕\r\n", instrArray[instrIndex]);
								instrIndex = -1; //执行完该指令了
							}
							else 
							printf("输入的参数不合法,请重新输入\r\n");
						}
					}
					break;
				case 2:
					paraGet = 0;
					printf("%s 指令执行指令完毕\r\n", instrArray[instrIndex]);
					instrIndex = -1;
					break;
				case 3:
					if(paraGet == 0)
					{
						printf("请输入年月日时分秒，以空格隔开\r\n");
						printf("样例：2022 5 1 12 02 11\r\n");
						paraGet = 1; //开始获取参数
					}
					else
					{
						paraNum = getParaNum(USART_RX_BUF, len);
						printf("接收到参数\r\n");
						if(paraNum != 6)
						{
							printf("输入参数个数不匹配，请重新输入\r\n");
						}
						else
						{
							if(paraArray[0] > 1970 && paraArray[0] < 2099 && paraArray[1] > 0 && paraArray[1] < 13 && paraArray[2] > 0 && paraArray[2] < 32
							&& paraArray[3] >= 0 && paraArray[3] < 24 && paraArray[4] >= 0 && paraArray[4] < 60 && paraArray[5] > 0 && paraArray[5] <= 60)
							{
							    paraGet = 0; //参数获取完毕
								SetDate(paraArray[0], paraArray[1], paraArray[2], paraArray[3], paraArray[4], paraArray[5]);
								printf("%s 指令执行指令完毕\r\n", instrArray[instrIndex]);
								instrIndex = -1; //执行完该指令了
							}
							else 
							printf("输入的参数不合法,请重新输入\r\n");
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

