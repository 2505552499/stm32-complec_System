/******************************************************
* À´ADCÕ®µ¿≤‚µÁ—π÷µ
* ≤‚¡øµÁ—π”¶<3.3  PA0ªÚPA1Ω”’˝º´£¨∏∫º´Ω”µÿ
* PA0≤‚¡øµƒµÁ—πœ‘ æ”Î◊Û±ﬂ£¨PA1≤‚¡øµƒµÁ—π÷µœ‘ æ”Î”“±ﬂ

******************************Œ¬∂»”Îπ‚’’≤‚¡ø************************
* Œ¬∂»”Îπ‚’’
* Œ¬∂»œ‘ æ”Î◊Û±ﬂ£¨π‚’’œ‘ æ‘⁄”“±ﬂ
***********************************END*****************************/

#include "adc.h"
#include "exti.h"
#include "math.h"

float adcTemperature = 0; //Õ‚≤øŒ¬∂»
float adcInnerTemperature = 0; //CPUŒ¬∂»
u16 adcNavKey = 7; //µº∫Ω∞¥º¸÷µ 7Œﬁ∞¥œ¬
u16 adcNav = 0;
u16 adcLight = 0; //π‚’’÷µ

/****************≥ı ºªØ∫Ø ˝********************
* ≥ı ºªØ
* IOø⁄≥ı ºªØ°¢ADC1≥ı ºªØ°¢ADC2≥ı ºªØ
******************************************************/
void VoltageAdcInit(void)
{
	//≥ı ºªØIOø⁄
	RCC->APB2ENR |= 1<<2;     // πƒ‹PORTAø⁄ ±÷”
	GPIOA->CRL &= 0xffffff00; //PA0 1  ƒ£ƒ‚ ‰»Î
	
	RCC->CFGR &= ~(3<<14);    //∑÷∆µ“Ú◊”«Â¡„   ADCPRE°æ15£∫14°ø 00£∫2∑÷∆µ 01£∫4 10£∫6 11£∫8
	RCC->CFGR |= 2<<14;       //6∑÷∆µ  SYSCLK/DIV2=12M ADC ±÷”…Ë÷√Œ™12M,ADC1◊Ó¥Û ±÷”≤ªƒ‹≥¨π˝14M!
	
	VoltageAdc1Init();
	VoltageAdc2Init();
}

/****************≥ı ºªØ∫Ø ˝********************
* ADC1≥ı ºªØ
******************************************************/
void VoltageAdc1Init(void)
{
	RCC->APB2ENR |= 1<<9;      //ADC1 ±÷” πƒ‹
	RCC->APB2RSTR |= 1<<9;     //ADC1∏¥Œª
	RCC->APB2RSTR &= ~(1<<9);  //∏¥ŒªΩ· ¯
	
	ADC1->CR1 &= 0xf0ffff;     //π§◊˜ƒ£ Ω«Â¡„
	ADC1->CR1 |= 0<<16;        //∂¿¡¢π§◊˜ƒ£ Ω
	ADC1->CR1 &= ~(1<<8);      //∑«…®√Ëƒ£ Ω
	ADC1->CR2 &= ~(1<<1);      //µ•¥Œ◊™ªªƒ£ Ω
	ADC1->CR2 &= ~(7<<17); 
	ADC1->CR2 |= 7<<17;        //SWSTART£∫»Ìº˛øÿ÷∆◊™ªª
	ADC1->CR2 |= 1<<20;        // π”√Õ‚≤ø¥•∑¢£®SWSTART£©£¨±ÿ–Î π”√“ª∏ˆ ¬º˛¿¥¥•∑¢
	ADC1->CR2 &= ~(1<<11);     //”“∂‘∆Î
	ADC1->SQR1 &= ~(0xf<<20);
	ADC1->SQR1 &= 0<<20;       //1∏ˆ◊™ªª‘⁄πÊ‘Ú–Ú¡–÷–£¨“≤æÕ «÷ª◊™ªªπÊ‘Ú–Ú¡–1
	
	ADC1->SMPR2 &= 0xfffffff0; //Õ®µ¿0≤…—˘ ±º‰«Âø’
	ADC1->SMPR2 |= 7<<0;       //Õ®µ¿0 239.5÷‹∆⁄£¨Ã·∏ﬂ≤…”√ ±º‰ø…“‘Ã·∏ﬂæ´»∑∂»
	
	ADC1->CR2 |= 1<<0;         //ø™∆ÙAD◊™ªª∆˜
	ADC1->CR2 |= 1<<3;         // πƒ‹∏¥Œª–£◊º
	while( ADC1->CR2 & 1<<3 )
		;                        //µ»¥˝–£◊ºΩ· ¯
	ADC1->CR2 |= 1<<2;         //ø™∆ÙAD–£◊º
	while( ADC1->CR2 & 1<<2 )
		;                        //µ»¥˝–£◊ºΩ· ¯
}

/****************≥ı ºªØ∫Ø ˝********************
* ADC2≥ı ºªØ
******************************************************/
void VoltageAdc2Init(void)
{
	RCC->APB2ENR |= 1<<10;     //ADC2 ±÷” πƒ‹
	RCC->APB2RSTR |= 1<<10;    //ADC2∏¥Œª
	RCC->APB2RSTR &= ~(1<<10); //∏¥ŒªΩ· 
	
	ADC2->CR1 &= 0xf0ffff;     //π§◊˜ƒ£ Ω«Â¡„
	ADC2->CR1 |= 0<<16;        //∂¿¡¢π§◊˜ƒ£ Ω
	ADC2->CR1 &= ~(1<<8);      //∑«…®√Ëƒ£ Ω
	ADC2->CR2 &= ~(1<<1);      //µ•¥Œ◊™ªªƒ£ Ω
	ADC2->CR2 &= ~(7<<17); 
	ADC2->CR2 |= 7<<17;        //SWSTART£∫»Ìº˛øÿ÷∆◊™ªª
	ADC2->CR2 |= 1<<20;        // π”√Õ‚≤ø¥•∑¢£®SWSTART£©£¨±ÿ–Î π”√“ª∏ˆ ¬º˛¿¥¥•∑¢
	ADC2->CR2 &= ~(1<<11);     //”“∂‘∆Î
	ADC2->SQR1 &= ~(0xf<<20);
	ADC2->SQR1 &= 0<<20;       //1∏ˆ◊™ªª‘⁄πÊ‘Ú–Ú¡–÷–£¨“≤æÕ «÷ª◊™ªªπÊ‘Ú–Ú¡–1
	
	ADC2->SMPR2 &= ~(7<<3);    //Õ®µ¿1≤…—˘ ±º‰«Âø’
	ADC2->SMPR2 |= 7<<3;       //Õ®µ¿1 239.5÷‹∆⁄£¨Ã·∏ﬂ≤…”√ ±º‰ø…“‘Ã·∏ﬂæ´»∑∂»
	
	ADC2->CR2 |= 1<<0;         //ø™∆ÙAD◊™ªª∆˜
	ADC2->CR2 |= 1<<3;         // πƒ‹∏¥Œª–£◊º
	while( ADC2->CR2 & 1<<3 )
		;                        //µ»¥˝–£◊ºΩ· ¯
	ADC2->CR2 |= 1<<2;         //ø™∆ÙAD–£◊º
	while( ADC2->CR2 & 1<<2 )
		;                        //µ»¥˝–£◊ºΩ· ¯
}

/****************ªÒ»°ADC÷µ∫Ø ˝********************
* ªÒ»°ADCµƒ÷µ£¨≤‚¡øµƒµÁ—π”¶<3.3  PA0ªÚPA1Ω”’˝º´£¨∏∫º´Ω”µÿ
* adcx: 1±Ì æADC1; 2±Ì æADC2
* ch: Õ®µ¿÷µ
* ∑µªÿµ√µΩµƒADCµƒ÷µ
******************************************************/

u16 GetAdc(u8 adcx, u8 ch)//ch «Õ®µ¿±‡∫≈0-17
{
	u16 adcValue;
	adcValue = 0;
	if( adcx==1 )
	{
		//…Ë÷√◊™ªª–Ú¡–
		ADC1->SQR3 &= ~(0xffffffff); //πÊ‘Ú–Ú¡–1 Õ®µ¿ch
		ADC1->SQR3 |= ch;
		ADC1->CR2 |= 1<<22;       //∆Ù∂ØπÊ‘Ú◊™ªªÕ®µ¿  »Ìº˛¥•∑¢
		while( !(ADC1->SR & 1<<1) )
			;                       //µ»¥˝◊™ªªΩ· ¯
		adcValue = ADC1->DR;
	}
	else if( adcx==2 )
	{
		//…Ë÷√◊™ªª–Ú¡–
		ADC2->SQR3 &= 0xffffffe0; //πÊ‘Ú–Ú¡–1 Õ®µ¿ch
		ADC2->SQR3 |= ch;
		ADC2->CR2 |= 1<<22;       //∆Ù∂ØπÊ‘Ú◊™ªªÕ®µ¿
		while( !(ADC2->SR & 1<<1) )
			;                       //µ»¥˝◊™ªªΩ· ¯
		adcValue = ADC2->DR;
	}
	return adcValue;            //∑µªÿADCµƒ÷µ
}


void AdcDescen(void)
{
 	u16 adcValue = 0;
	u16 temp = adcNavKey;
//Õ®µ¿14π‚’’
	ADC1->CR2 |= 1<<22;
	//if(ADC1->SR & 1<<4)
	delay_us(100);
	adcLight = ADC1->DR;
	
//Õ®µ¿15Õ‚≤øŒ¬∂»
	ADC1->CR2 |= 1<<22;
	//while(ADC1->DR == adcLight);
	delay_us(100);
	adcValue = ADC1->DR;
	adcTemperature = (float)adcValue*(3.3/4096); //µ±«∞Œ¬∂»µÁ—π÷µ
	adcTemperature = adcTemperature *10000/(3.3-adcTemperature)/1000; //Œ¬∂»µÁ◊Ë◊Ë÷µ
	adcTemperature = (float)1.0/( (float)log(adcTemperature*1000/10000)/3950 + 1.0/(273.16+25) ) - 273.16; //º∆À„Œ¬∂»

//Õ®µ¿16ƒ⁄≤øŒ¬∂»
	ADC1->CR2 |= 1<<22;
//	while(ADC1->DR == adcValue);
	delay_us(100);
	adcValue = ADC1->DR;
	adcInnerTemperature = (1.43 - adcValue) / 4.3 + 25;

//Õ®µ¿10µº∫Ω∞¥º¸
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

/****************ªÒ»°µÁ—π÷µ∫Ø ˝********************
* ADC◊™ªØŒ™µÁ—π÷µ
* adcx: 1±Ì æADC1; 2±Ì æADC2
* ch: Õ®µ¿÷µ
* ∑µªÿµÁ—π÷µ
******************************************************/
float GetVoltage(u8 adcx, u8 ch)
{
	u16 adcValue = 0;
	float vol = 0;
	//adcValue = GetAdc( adcx, ch );
	vol = 3.3*(float)adcValue/4096;
	return vol;
}

/****************œ‘ æ∂‘”¶∂Àø⁄µƒµÁ—π÷µ∫Ø ˝********************
* œ‘ æ£¨’º»˝Œª
* adcx: 1±Ì æADC1; 2±Ì æADC2
* vol: µÁ—π÷µ
* PA0≤‚¡øµƒµÁ—πœ‘ æ”Î◊Û±ﬂ£¨PA1≤‚¡øµƒµÁ—π÷µœ‘ æ”Î”“±ﬂ
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
/****************∏˘æ›10Õ®µ¿AD÷µ ‰≥ˆ∞¥œ¬µƒµº∫Ωº¸
∑ΩœÚ  Œﬁ   K3	”“	œ¬	¿Ô	◊Û	…œ
	  7	    0	1	2	3	4	5

****************/
u8 GetNavKey(u16 ch) //…®√Ë≤¢≈–∂œµº∫Ω∑ΩœÚ
{
	u16 adcValue;
	GetAdc(1, 10);

	//ªÒ»°Œ®“ªø…«¯∑÷µº∫Ω∑ΩœÚµƒ»˝Œª∂˛Ω¯÷∆ ˝9£∫»°11 10 9Œª«¯∑÷µº∫Ω∞¥º¸
	adcValue = adcValue>>9; 
	adcValue &= 7<<0;
	return adcValue;
}

/***************Œ¬∂»∫Õπ‚’’ADCµƒ≥ı ºªØ∫Ø ˝********************
* ≥ı ºªØADC1£¨Õ®µ¿14 15
******************************************************/
void AdcInit(void)
{
	//≥ı ºªØIOø⁄
	RCC->APB2ENR |= 1<<4;      // πƒ‹PORTCø⁄ ±÷”
	GPIOC->CRL &= 0xff00ffff;  //PC4 5  ƒ£ƒ‚ ‰»Î
	GPIOC->CRL &= 0xfffffff0;  //PC0 ƒ£ƒ‚ ‰»Î
	RCC->CFGR &= ~(3<<14);     //∑÷∆µ“Ú◊”«Â¡„
	RCC->CFGR |= 2<<14;        //6∑÷∆µ  SYSCLK/DIV2=12M ADC ±÷”…Ë÷√Œ™12M,ADC1◊Ó¥Û ±÷”≤ªƒ‹≥¨π˝14M!
	
	RCC->APB2ENR |= 1<<9;      //ADC1 ±÷” πƒ‹
	RCC->APB2RSTR |= 1<<9;     //ADC1∏¥Œª
	RCC->APB2RSTR &= ~(1<<9);  //∏¥ŒªΩ· ¯
	
	ADC1->CR1 &= 0xf0ffff;     //π§◊˜ƒ£ Ω«Â¡„
	ADC1->CR1 |= 0<<16;        //∂¿¡¢π§◊˜ƒ£ Ω
	ADC1->CR1 |= 1<<23;
	ADC1->CR1 &= ~(1<<8);      //∑«…®√Ëƒ£ Ω
	ADC1->CR1 |= 1<<5;	//∆Ù∂ØEOC÷–∂œ
	ADC1->CR1 |= 1<<11; //∆Ù∂Øº‰∂œƒ£ Ω
	ADC1->CR1 |= 0<<13; //º‰∂œƒ£ Ω√ø¥Œ…®√Ë1∏ˆÕ®µ¿

	ADC1->CR2 &= ~(1<<1);      //µ•¥Œ◊™ªªƒ£ Ω
	ADC1->CR2 &= ~(7<<17); 
	ADC1->CR2 |= 7<<17;        //SWSTART£∫»Ìº˛øÿ÷∆◊™ªª
	ADC1->CR2 |= 1<<20;        // π”√Õ‚≤ø¥•∑¢£®SWSTART£©£¨±ÿ–Î π”√“ª∏ˆ ¬º˛¿¥¥•∑¢
	ADC1->CR2 &= ~(1<<11);     //”“∂‘∆Î
	ADC1->SQR1 &= ~(0xf<<20);
	//ADC1->SQR1 &= 0<<20;       //1∏ˆ◊™ªª‘⁄πÊ‘Ú–Ú¡–÷–£¨“≤æÕ «÷ª◊™ªªπÊ‘Ú–Ú¡–1
	ADC1->SQR1 |= 0x3<<20;	//4∏ˆ◊™ªª‘⁄πÊ‘Ú–Ú¡–÷–
	ADC1->SQR3 |= 14<<0;	//µ⁄1∏ˆ◊™ªª
	ADC1->SQR3 |= 15<<5;
	ADC1->SQR3 |= 16<<10;
	ADC1->SQR3 |= 10<<15;	

	ADC1->SMPR1 &= 0xffe00ff0; //Õ®µ¿10µº∫Ω£¨14π‚’’,15Œ¬∂»£¨16ƒ⁄≤øŒ¬∂»≤…—˘ ±º‰«Âø’
	ADC1->SMPR1 |= 7<<0;      //Õ®µ¿10 239.5÷‹∆⁄£¨Ã·∏ﬂ≤…”√ ±º‰ø…“‘Ã·∏ﬂæ´»∑∂»
	ADC1->SMPR1 |= 7<<12;      //Õ®µ¿14 239.5÷‹∆⁄£¨Ã·∏ﬂ≤…”√ ±º‰ø…“‘Ã·∏ﬂæ´»∑∂»
	ADC1->SMPR1 |= 7<<15;      //Õ®µ¿15 239.5÷‹∆⁄£¨Ã·∏ﬂ≤…”√ ±º‰ø…“‘Ã·∏ﬂæ´»∑∂»
	ADC1->SMPR1 |= 7<<18;      //Õ®µ¿16 239.5÷‹∆⁄£¨Ã·∏ﬂ≤…”√ ±º‰ø…“‘Ã·∏ﬂæ´»∑∂»
  
	ADC1->CR2 |= 1<<0;         //ø™∆ÙAD◊™ªª∆˜
	ADC1->CR2 |= 1<<3;         // πƒ‹∏¥Œª–£◊º
	while( ADC1->CR2 & 1<<3 )
		;                        //µ»¥˝–£◊ºΩ· ¯
	ADC1->CR2 |= 1<<2;         //ø™∆ÙAD–£◊º
	while( ADC1->CR2 & 1<<2 )
		;                        //µ»¥˝–£◊ºΩ· ¯
//	MY_NVIC_Init(2, 2, ADC1_2_IRQChannel, 2);
	
}




/***************ªÒ»°Œ¬∂»µƒADCµƒ÷µ∫Ø ˝********************
* ªÒ»°ADC1µƒADC÷µ
* chŒ™Õ®µ¿÷µ
* ∑µªÿADC1µƒADC÷µ
******************************************************/
u16 GetTemperatureAdc(u8 ch)
{
	u16 adcValue = 0;
	//adcValue = GetAdc( 1,ch );
	return adcValue;
}

/***************ADC÷µ◊™ªª≥…Œ¬∂»÷µ∫Ø ˝********************
* Õ®π˝ADC÷µº∆À„Œ¬∂»÷µ
* ∑µªÿŒ¬∂»÷µ
******************************************************/
float GetTemperature( void )
{
	u16 temperatureAdc = 0;
	float temperature = 0.0;
 	temperatureAdc = GetTemperatureAdc( 15 );       //Õ®µ¿15◊¢»ÎµƒAD÷µ
	temperature = (float)temperatureAdc*(3.3/4096); //µ±«∞Œ¬∂»µÁ—π÷µ
	temperature = temperature *10000/(3.3-temperature)/1000; //Œ¬∂»µÁ◊Ë◊Ë÷µ
	temperature = (float)1.0/( (float)log(temperature*1000/10000)/3950 + 1.0/(273.16+25) ) - 273.16; //º∆À„Œ¬∂»
	
	return temperature;
}

float GetInnerTemperature(void)
{
	u16 temperatureAdc = 0;
	float temperature = 0.0;
 	temperatureAdc = GetTemperatureAdc( 16 );       //Õ®µ¿16◊¢»ÎµƒAD÷µ
	temperature = (1.43 - temperatureAdc) / 4.3 + 25;	
	return temperature;
}

/***************π‚’’«ø∂»µƒADC÷µ∫Ø ˝********************
* π‚’’«ø∂»µƒADC÷µ
* chŒ™Õ®µ¿÷µ
* ∑µªÿπ‚’’µƒADC÷µ
******************************************************/
u16 GetLightAdc(u8 ch)
{
	u16 adcValue = 0;
	GetAdc(1, ch);
	return adcValue;
}

//∏¸–¬∏˜ADC÷µ
void adcScan(void)
{
		adcTemperature = GetTemperature();
	//	adcLight = GetLightAdc(14);
		adcNavKey = GetNavKey(10);
		//adcInnerTemperature = GetInnerTemperature();
	//	adcNav = GetAdc(1, 10);
		
}


