/******************************************************
* 双ADC通道测电压值
* 测量电压应<3.3  PA0或PA1接正极，负极接地
* PA0测量的电压显示与左边，PA1测量的电压值显示与右边

******************************温度与光照测量************************
* 温度与光照
* 温度显示与左边，光照显示在右边
***********************************END*****************************/

#include "adc.h"
#include "exti.h"
#include "math.h"

float adcTemperature = 0; //外部温度
float adcInnerTemperature = 0; //CPU温度
u16 adcNavKey = 7; //导航按键值 7无按下
u16 adcNav = 0;
u16 adcLight = 0; //光照值

/****************初始化函数********************
* 初始化
* IO口初始化、ADC1初始化、ADC2初始化
******************************************************/
void VoltageAdcInit(void)
{
	//初始化IO口
	RCC->APB2ENR |= 1<<2;     //使能PORTA口时钟
	GPIOA->CRL &= 0xffffff00; //PA0 1  模拟输入
	
	RCC->CFGR &= ~(3<<14);    //分频因子清零   ADCPRE【15：14】 00：2分频 01：4 10：6 11：8
	RCC->CFGR |= 2<<14;       //6分频  SYSCLK/DIV2=12M ADC时钟设置为12M,ADC1最大时钟不能超过14M!
	
	VoltageAdc1Init();
	VoltageAdc2Init();
}

/****************初始化函数********************
* ADC1初始化
******************************************************/
void VoltageAdc1Init(void)
{
	RCC->APB2ENR |= 1<<9;      //ADC1时钟使能
	RCC->APB2RSTR |= 1<<9;     //ADC1复位
	RCC->APB2RSTR &= ~(1<<9);  //复位结束
	
	ADC1->CR1 &= 0xf0ffff;     //工作模式清零
	ADC1->CR1 |= 0<<16;        //独立工作模式
	ADC1->CR1 &= ~(1<<8);      //非扫描模式
	ADC1->CR2 &= ~(1<<1);      //单次转换模式
	ADC1->CR2 &= ~(7<<17); 
	ADC1->CR2 |= 7<<17;        //SWSTART：软件控制转换
	ADC1->CR2 |= 1<<20;        //使用外部触发（SWSTART），必须使用一个事件来触发
	ADC1->CR2 &= ~(1<<11);     //右对齐
	ADC1->SQR1 &= ~(0xf<<20);
	ADC1->SQR1 &= 0<<20;       //1个转换在规则序列中，也就是只转换规则序列1
	
	ADC1->SMPR2 &= 0xfffffff0; //通道0采样时间清空
	ADC1->SMPR2 |= 7<<0;       //通道0 239.5周期，提高采用时间可以提高精确度
	
	ADC1->CR2 |= 1<<0;         //开启AD转换器
	ADC1->CR2 |= 1<<3;         //使能复位校准
	while( ADC1->CR2 & 1<<3 )
		;                        //等待校准结束
	ADC1->CR2 |= 1<<2;         //开启AD校准
	while( ADC1->CR2 & 1<<2 )
		;                        //等待校准结束
}

/****************初始化函数********************
* ADC2初始化
******************************************************/
void VoltageAdc2Init(void)
{
	RCC->APB2ENR |= 1<<10;     //ADC2时钟使能
	RCC->APB2RSTR |= 1<<10;    //ADC2复位
	RCC->APB2RSTR &= ~(1<<10); //复位结?
	
	ADC2->CR1 &= 0xf0ffff;     //工作模式清零
	ADC2->CR1 |= 0<<16;        //独立工作模式
	ADC2->CR1 &= ~(1<<8);      //非扫描模式
	ADC2->CR2 &= ~(1<<1);      //单次转换模式
	ADC2->CR2 &= ~(7<<17); 
	ADC2->CR2 |= 7<<17;        //SWSTART：软件控制转换
	ADC2->CR2 |= 1<<20;        //使用外部触发（SWSTART），必须使用一个事件来触发
	ADC2->CR2 &= ~(1<<11);     //右对齐
	ADC2->SQR1 &= ~(0xf<<20);
	ADC2->SQR1 &= 0<<20;       //1个转换在规则序列中，也就是只转换规则序列1
	
	ADC2->SMPR2 &= ~(7<<3);    //通道1采样时间清空
	ADC2->SMPR2 |= 7<<3;       //通道1 239.5周期，提高采用时间可以提高精确度
	
	ADC2->CR2 |= 1<<0;         //开启AD转换器
	ADC2->CR2 |= 1<<3;         //使能复位校准
	while( ADC2->CR2 & 1<<3 )
		;                        //等待校准结束
	ADC2->CR2 |= 1<<2;         //开启AD校准
	while( ADC2->CR2 & 1<<2 )
		;                        //等待校准结束
}

/****************获取ADC值函数********************
* 获取ADC的值，测量的电压应<3.3  PA0或PA1接正极，负极接地
* adcx: 1表示ADC1; 2表示ADC2
* ch: 通道值
* 返回得到的ADC的值
******************************************************/

u16 GetAdc(u8 adcx, u8 ch)//ch是通道编号0-17
{
	u16 adcValue;
	adcValue = 0;
	if( adcx==1 )
	{
		//设置转换序列
		ADC1->SQR3 &= ~(0xffffffff); //规则序列1 通道ch
		ADC1->SQR3 |= ch;
		ADC1->CR2 |= 1<<22;       //启动规则转换通道  软件触发
		while( !(ADC1->SR & 1<<1) )
			;                       //等待转换结束
		adcValue = ADC1->DR;
	}
	else if( adcx==2 )
	{
		//设置转换序列
		ADC2->SQR3 &= 0xffffffe0; //规则序列1 通道ch
		ADC2->SQR3 |= ch;
		ADC2->CR2 |= 1<<22;       //启动规则转换通道
		while( !(ADC2->SR & 1<<1) )
			;                       //等待转换结束
		adcValue = ADC2->DR;
	}
	return adcValue;            //返回ADC的值
}


void AdcDescen(void)
{
 	u16 adcValue = 0;
	u16 temp = adcNavKey;
//通道14光照
	ADC1->CR2 |= 1<<22;
	//if(ADC1->SR & 1<<4)
	delay_us(100);
	adcLight = ADC1->DR;
	
//通道15外部温度
	ADC1->CR2 |= 1<<22;
	//while(ADC1->DR == adcLight);
	delay_us(100);
	adcValue = ADC1->DR;
	adcTemperature = (float)adcValue*(3.3/4096); //当前温度电压值
	adcTemperature = adcTemperature *10000/(3.3-adcTemperature)/1000; //温度电阻阻值
	adcTemperature = (float)1.0/( (float)log(adcTemperature*1000/10000)/3950 + 1.0/(273.16+25) ) - 273.16; //计算温度

//通道16内部温度
	ADC1->CR2 |= 1<<22;
//	while(ADC1->DR == adcValue);
	delay_us(100);
	adcValue = ADC1->DR;
	adcInnerTemperature = (1.43 - adcValue) / 4.3 + 25;

//通道10导航按键
	ADC1->CR2 |= 1<<22;
//	while(ADC1->DR == adcValue);
	delay_us(100);
	adcValue = ADC1->DR;
	adcNav = adcValue;
	adcNavKey = adcNav>>9;
	if(adcNavKey != temp) 
	{
		NavKey(adcNavKey);
	}
}

/****************获取电压值函数********************
* ADC转化为电压值
* adcx: 1表示ADC1; 2表示ADC2
* ch: 通道值
* 返回电压值
******************************************************/
float GetVoltage(u8 adcx, u8 ch)
{
	u16 adcValue = 0;
	float vol = 0;
	//adcValue = GetAdc( adcx, ch );
	vol = 3.3*(float)adcValue/4096;
	return vol;
}

/****************显示对应端口的电压值函数********************
* 显示，占三位
* adcx: 1表示ADC1; 2表示ADC2
* vol: 电压值
* PA0测量的电压显示与左边，PA1测量的电压值显示与右边
******************************************************/
void VoltageDisplay(u8 adcx, float vol)
{
	u8 baiWei, shiWei, geWei;

  baiWei = (u8)vol % 10;
	shiWei = (u8)(vol*10)%10;
	geWei = (u8)(vol*100)%10;
	if( adcx==1 )
	{
		PortationDisplay(0, baiWei);
		delay_ms(1);
		SetLed(1, shiWei);
		delay_ms(1);
		SetLed(2, geWei);
		delay_ms(1);
	}
	else if( adcx==2 )
	{
		PortationDisplay(5, baiWei);
		delay_ms(1);
		SetLed(6, shiWei);
		delay_ms(1);
		SetLed(7, geWei);
		delay_ms(1);
  }
}
/****************根据10通道AD值输出按下的导航键
方向  无   K3	右	下	里	左	上
	  7	    0	1	2	3	4	5

****************/
u8 GetNavKey(u16 ch) //扫描并判断导航方向
{
	u16 adcValue;
	GetAdc(1, 10);

	//获取唯一可区分导航方向的三位二进制数9：取11 10 9位区分导航按键
	adcValue = adcValue>>9; 
	adcValue &= 7<<0;
	return adcValue;
}

/***************温度和光照ADC的初始化函数********************
* 初始化ADC1，通道14 15
******************************************************/
void AdcInit(void)
{
	//初始化IO口
	RCC->APB2ENR |= 1<<4;      //使能PORTC口时钟
	GPIOC->CRL &= 0xff00ffff;  //PC4 5  模拟输入
	GPIOC->CRL &= 0xfffffff0;  //PC0 模拟输入
	RCC->CFGR &= ~(3<<14);     //分频因子清零
	RCC->CFGR |= 2<<14;        //6分频  SYSCLK/DIV2=12M ADC时钟设置为12M,ADC1最大时钟不能超过14M!
	
	RCC->APB2ENR |= 1<<9;      //ADC1时钟使能
	RCC->APB2RSTR |= 1<<9;     //ADC1复位
	RCC->APB2RSTR &= ~(1<<9);  //复位结束
	
	ADC1->CR1 &= 0xf0ffff;     //工作模式清零
	ADC1->CR1 |= 0<<16;        //独立工作模式
	ADC1->CR1 |= 1<<23;
	ADC1->CR1 &= ~(1<<8);      //非扫描模式
	ADC1->CR1 |= 1<<5;	//启动EOC中断
	ADC1->CR1 |= 1<<11; //启动间断模式
	ADC1->CR1 |= 0<<13; //间断模式每次扫描1个通道

	ADC1->CR2 &= ~(1<<1);      //单次转换模式
	ADC1->CR2 &= ~(7<<17); 
	ADC1->CR2 |= 7<<17;        //SWSTART：软件控制转换
	ADC1->CR2 |= 1<<20;        //使用外部触发（SWSTART），必须使用一个事件来触发
	ADC1->CR2 &= ~(1<<11);     //右对齐
	ADC1->SQR1 &= ~(0xf<<20);
	//ADC1->SQR1 &= 0<<20;       //1个转换在规则序列中，也就是只转换规则序列1
	ADC1->SQR1 |= 0x3<<20;	//4个转换在规则序列中
	ADC1->SQR3 |= 14<<0;	//第1个转换
	ADC1->SQR3 |= 15<<5;
	ADC1->SQR3 |= 16<<10;
	ADC1->SQR3 |= 10<<15;	

	ADC1->SMPR1 &= 0xffe00ff0; //通道10导航，14光照,15温度，16内部温度采样时间清空
	ADC1->SMPR1 |= 7<<0;      //通道10 239.5周期，提高采用时间可以提高精确度
	ADC1->SMPR1 |= 7<<12;      //通道14 239.5周期，提高采用时间可以提高精确度
	ADC1->SMPR1 |= 7<<15;      //通道15 239.5周期，提高采用时间可以提高精确度
	ADC1->SMPR1 |= 7<<18;      //通道16 239.5周期，提高采用时间可以提高精确度
  
	ADC1->CR2 |= 1<<0;         //开启AD转换器
	ADC1->CR2 |= 1<<3;         //使能复位校准
	while( ADC1->CR2 & 1<<3 )
		;                        //等待校准结束
	ADC1->CR2 |= 1<<2;         //开启AD校准
	while( ADC1->CR2 & 1<<2 )
		;                        //等待校准结束
//	MY_NVIC_Init(2, 2, ADC1_2_IRQChannel, 2);
	
}




/***************获取温度的ADC的值函数********************
* 获取ADC1的ADC值
* ch为通道值
* 返回ADC1的ADC值
******************************************************/
u16 GetTemperatureAdc(u8 ch)
{
	u16 adcValue = 0;
	//adcValue = GetAdc( 1,ch );
	return adcValue;
}

/***************ADC值转换成温度值函数********************
* 通过ADC值计算温度值
* 返回温度值
******************************************************/
float GetTemperature( void )
{
	u16 temperatureAdc = 0;
	float temperature = 0.0;
 	temperatureAdc = GetTemperatureAdc( 15 );       //通道15注入的AD值
	temperature = (float)temperatureAdc*(3.3/4096); //当前温度电压值
	temperature = temperature *10000/(3.3-temperature)/1000; //温度电阻阻值
	temperature = (float)1.0/( (float)log(temperature*1000/10000)/3950 + 1.0/(273.16+25) ) - 273.16; //计算温度
	
	return temperature;
}

float GetInnerTemperature(void)
{
	u16 temperatureAdc = 0;
	float temperature = 0.0;
 	temperatureAdc = GetTemperatureAdc( 16 );       //通道16注入的AD值
	temperature = (1.43 - temperatureAdc) / 4.3 + 25;	
	return temperature;
}

/***************光照强度的ADC值函数********************
* 光照强度的ADC值
* ch为通道值
* 返回光照的ADC值
******************************************************/
u16 GetLightAdc(u8 ch)
{
	u16 adcValue = 0;
	GetAdc(1, ch);
	return adcValue;
}

//更新各ADC值
void adcScan(void)
{
		adcTemperature = GetTemperature();
	//	adcLight = GetLightAdc(14);
		adcNavKey = GetNavKey(10);
		//adcInnerTemperature = GetInnerTemperature();
	//	adcNav = GetAdc(1, 10);
		
}


