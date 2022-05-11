/******************************************************
* ˫ADCͨ�����ѹֵ
* ������ѹӦ<3.3  PA0��PA1�������������ӵ�
* PA0�����ĵ�ѹ��ʾ����ߣ�PA1�����ĵ�ѹֵ��ʾ���ұ�

******************************�¶�����ղ���************************
* �¶������
* �¶���ʾ����ߣ�������ʾ���ұ�
***********************************END*****************************/

#include "adc.h"
#include "exti.h"
#include "math.h"

float adcTemperature = 0; //�ⲿ�¶�
float adcInnerTemperature = 0; //CPU�¶�
u16 adcNavKey = 7; //��������ֵ 7�ް���
u16 adcNav = 0;
u16 adcLight = 0; //����ֵ

/****************��ʼ������********************
* ��ʼ��
* IO�ڳ�ʼ����ADC1��ʼ����ADC2��ʼ��
******************************************************/
void VoltageAdcInit(void)
{
	//��ʼ��IO��
	RCC->APB2ENR |= 1<<2;     //ʹ��PORTA��ʱ��
	GPIOA->CRL &= 0xffffff00; //PA0 1  ģ������
	
	RCC->CFGR &= ~(3<<14);    //��Ƶ��������   ADCPRE��15��14�� 00��2��Ƶ 01��4 10��6 11��8
	RCC->CFGR |= 2<<14;       //6��Ƶ  SYSCLK/DIV2=12M ADCʱ������Ϊ12M,ADC1���ʱ�Ӳ��ܳ���14M!
	
	VoltageAdc1Init();
	VoltageAdc2Init();
}

/****************��ʼ������********************
* ADC1��ʼ��
******************************************************/
void VoltageAdc1Init(void)
{
	RCC->APB2ENR |= 1<<9;      //ADC1ʱ��ʹ��
	RCC->APB2RSTR |= 1<<9;     //ADC1��λ
	RCC->APB2RSTR &= ~(1<<9);  //��λ����
	
	ADC1->CR1 &= 0xf0ffff;     //����ģʽ����
	ADC1->CR1 |= 0<<16;        //��������ģʽ
	ADC1->CR1 &= ~(1<<8);      //��ɨ��ģʽ
	ADC1->CR2 &= ~(1<<1);      //����ת��ģʽ
	ADC1->CR2 &= ~(7<<17); 
	ADC1->CR2 |= 7<<17;        //SWSTART���������ת��
	ADC1->CR2 |= 1<<20;        //ʹ���ⲿ������SWSTART��������ʹ��һ���¼�������
	ADC1->CR2 &= ~(1<<11);     //�Ҷ���
	ADC1->SQR1 &= ~(0xf<<20);
	ADC1->SQR1 &= 0<<20;       //1��ת���ڹ��������У�Ҳ����ֻת����������1
	
	ADC1->SMPR2 &= 0xfffffff0; //ͨ��0����ʱ�����
	ADC1->SMPR2 |= 7<<0;       //ͨ��0 239.5���ڣ���߲���ʱ�������߾�ȷ��
	
	ADC1->CR2 |= 1<<0;         //����ADת����
	ADC1->CR2 |= 1<<3;         //ʹ�ܸ�λУ׼
	while( ADC1->CR2 & 1<<3 )
		;                        //�ȴ�У׼����
	ADC1->CR2 |= 1<<2;         //����ADУ׼
	while( ADC1->CR2 & 1<<2 )
		;                        //�ȴ�У׼����
}

/****************��ʼ������********************
* ADC2��ʼ��
******************************************************/
void VoltageAdc2Init(void)
{
	RCC->APB2ENR |= 1<<10;     //ADC2ʱ��ʹ��
	RCC->APB2RSTR |= 1<<10;    //ADC2��λ
	RCC->APB2RSTR &= ~(1<<10); //��λ���
	
	ADC2->CR1 &= 0xf0ffff;     //����ģʽ����
	ADC2->CR1 |= 0<<16;        //��������ģʽ
	ADC2->CR1 &= ~(1<<8);      //��ɨ��ģʽ
	ADC2->CR2 &= ~(1<<1);      //����ת��ģʽ
	ADC2->CR2 &= ~(7<<17); 
	ADC2->CR2 |= 7<<17;        //SWSTART���������ת��
	ADC2->CR2 |= 1<<20;        //ʹ���ⲿ������SWSTART��������ʹ��һ���¼�������
	ADC2->CR2 &= ~(1<<11);     //�Ҷ���
	ADC2->SQR1 &= ~(0xf<<20);
	ADC2->SQR1 &= 0<<20;       //1��ת���ڹ��������У�Ҳ����ֻת����������1
	
	ADC2->SMPR2 &= ~(7<<3);    //ͨ��1����ʱ�����
	ADC2->SMPR2 |= 7<<3;       //ͨ��1 239.5���ڣ���߲���ʱ�������߾�ȷ��
	
	ADC2->CR2 |= 1<<0;         //����ADת����
	ADC2->CR2 |= 1<<3;         //ʹ�ܸ�λУ׼
	while( ADC2->CR2 & 1<<3 )
		;                        //�ȴ�У׼����
	ADC2->CR2 |= 1<<2;         //����ADУ׼
	while( ADC2->CR2 & 1<<2 )
		;                        //�ȴ�У׼����
}

/****************��ȡADCֵ����********************
* ��ȡADC��ֵ�������ĵ�ѹӦ<3.3  PA0��PA1�������������ӵ�
* adcx: 1��ʾADC1; 2��ʾADC2
* ch: ͨ��ֵ
* ���صõ���ADC��ֵ
******************************************************/

u16 GetAdc(u8 adcx, u8 ch)//ch��ͨ�����0-17
{
	u16 adcValue;
	adcValue = 0;
	if( adcx==1 )
	{
		//����ת������
		ADC1->SQR3 &= ~(0xffffffff); //��������1 ͨ��ch
		ADC1->SQR3 |= ch;
		ADC1->CR2 |= 1<<22;       //��������ת��ͨ��  �������
		while( !(ADC1->SR & 1<<1) )
			;                       //�ȴ�ת������
		adcValue = ADC1->DR;
	}
	else if( adcx==2 )
	{
		//����ת������
		ADC2->SQR3 &= 0xffffffe0; //��������1 ͨ��ch
		ADC2->SQR3 |= ch;
		ADC2->CR2 |= 1<<22;       //��������ת��ͨ��
		while( !(ADC2->SR & 1<<1) )
			;                       //�ȴ�ת������
		adcValue = ADC2->DR;
	}
	return adcValue;            //����ADC��ֵ
}


void AdcDescen(void)
{
 	u16 adcValue = 0;
	u16 temp = adcNavKey;
//ͨ��14����
	ADC1->CR2 |= 1<<22;
	//if(ADC1->SR & 1<<4)
	delay_us(100);
	adcLight = ADC1->DR;
	
//ͨ��15�ⲿ�¶�
	ADC1->CR2 |= 1<<22;
	//while(ADC1->DR == adcLight);
	delay_us(100);
	adcValue = ADC1->DR;
	adcTemperature = (float)adcValue*(3.3/4096); //��ǰ�¶ȵ�ѹֵ
	adcTemperature = adcTemperature *10000/(3.3-adcTemperature)/1000; //�¶ȵ�����ֵ
	adcTemperature = (float)1.0/( (float)log(adcTemperature*1000/10000)/3950 + 1.0/(273.16+25) ) - 273.16; //�����¶�

//ͨ��16�ڲ��¶�
	ADC1->CR2 |= 1<<22;
//	while(ADC1->DR == adcValue);
	delay_us(100);
	adcValue = ADC1->DR;
	adcInnerTemperature = (1.43 - adcValue) / 4.3 + 25;

//ͨ��10��������
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

/****************��ȡ��ѹֵ����********************
* ADCת��Ϊ��ѹֵ
* adcx: 1��ʾADC1; 2��ʾADC2
* ch: ͨ��ֵ
* ���ص�ѹֵ
******************************************************/
float GetVoltage(u8 adcx, u8 ch)
{
	u16 adcValue = 0;
	float vol = 0;
	//adcValue = GetAdc( adcx, ch );
	vol = 3.3*(float)adcValue/4096;
	return vol;
}

/****************��ʾ��Ӧ�˿ڵĵ�ѹֵ����********************
* ��ʾ��ռ��λ
* adcx: 1��ʾADC1; 2��ʾADC2
* vol: ��ѹֵ
* PA0�����ĵ�ѹ��ʾ����ߣ�PA1�����ĵ�ѹֵ��ʾ���ұ�
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
/****************����10ͨ��ADֵ������µĵ�����
����  ��   K3	��	��	��	��	��
	  7	    0	1	2	3	4	5

****************/
u8 GetNavKey(u16 ch) //ɨ�貢�жϵ�������
{
	u16 adcValue;
	GetAdc(1, 10);

	//��ȡΨһ�����ֵ����������λ��������9��ȡ11 10 9λ���ֵ�������
	adcValue = adcValue>>9; 
	adcValue &= 7<<0;
	return adcValue;
}

/***************�¶Ⱥ͹���ADC�ĳ�ʼ������********************
* ��ʼ��ADC1��ͨ��14 15
******************************************************/
void AdcInit(void)
{
	//��ʼ��IO��
	RCC->APB2ENR |= 1<<4;      //ʹ��PORTC��ʱ��
	GPIOC->CRL &= 0xff00ffff;  //PC4 5  ģ������
	GPIOC->CRL &= 0xfffffff0;  //PC0 ģ������
	RCC->CFGR &= ~(3<<14);     //��Ƶ��������
	RCC->CFGR |= 2<<14;        //6��Ƶ  SYSCLK/DIV2=12M ADCʱ������Ϊ12M,ADC1���ʱ�Ӳ��ܳ���14M!
	
	RCC->APB2ENR |= 1<<9;      //ADC1ʱ��ʹ��
	RCC->APB2RSTR |= 1<<9;     //ADC1��λ
	RCC->APB2RSTR &= ~(1<<9);  //��λ����
	
	ADC1->CR1 &= 0xf0ffff;     //����ģʽ����
	ADC1->CR1 |= 0<<16;        //��������ģʽ
	ADC1->CR1 |= 1<<23;
	ADC1->CR1 &= ~(1<<8);      //��ɨ��ģʽ
	ADC1->CR1 |= 1<<5;	//����EOC�ж�
	ADC1->CR1 |= 1<<11; //�������ģʽ
	ADC1->CR1 |= 0<<13; //���ģʽÿ��ɨ��1��ͨ��

	ADC1->CR2 &= ~(1<<1);      //����ת��ģʽ
	ADC1->CR2 &= ~(7<<17); 
	ADC1->CR2 |= 7<<17;        //SWSTART���������ת��
	ADC1->CR2 |= 1<<20;        //ʹ���ⲿ������SWSTART��������ʹ��һ���¼�������
	ADC1->CR2 &= ~(1<<11);     //�Ҷ���
	ADC1->SQR1 &= ~(0xf<<20);
	//ADC1->SQR1 &= 0<<20;       //1��ת���ڹ��������У�Ҳ����ֻת����������1
	ADC1->SQR1 |= 0x3<<20;	//4��ת���ڹ���������
	ADC1->SQR3 |= 14<<0;	//��1��ת��
	ADC1->SQR3 |= 15<<5;
	ADC1->SQR3 |= 16<<10;
	ADC1->SQR3 |= 10<<15;	

	ADC1->SMPR1 &= 0xffe00ff0; //ͨ��10������14����,15�¶ȣ�16�ڲ��¶Ȳ���ʱ�����
	ADC1->SMPR1 |= 7<<0;      //ͨ��10 239.5���ڣ���߲���ʱ�������߾�ȷ��
	ADC1->SMPR1 |= 7<<12;      //ͨ��14 239.5���ڣ���߲���ʱ�������߾�ȷ��
	ADC1->SMPR1 |= 7<<15;      //ͨ��15 239.5���ڣ���߲���ʱ�������߾�ȷ��
	ADC1->SMPR1 |= 7<<18;      //ͨ��16 239.5���ڣ���߲���ʱ�������߾�ȷ��
  
	ADC1->CR2 |= 1<<0;         //����ADת����
	ADC1->CR2 |= 1<<3;         //ʹ�ܸ�λУ׼
	while( ADC1->CR2 & 1<<3 )
		;                        //�ȴ�У׼����
	ADC1->CR2 |= 1<<2;         //����ADУ׼
	while( ADC1->CR2 & 1<<2 )
		;                        //�ȴ�У׼����
//	MY_NVIC_Init(2, 2, ADC1_2_IRQChannel, 2);
	
}




/***************��ȡ�¶ȵ�ADC��ֵ����********************
* ��ȡADC1��ADCֵ
* chΪͨ��ֵ
* ����ADC1��ADCֵ
******************************************************/
u16 GetTemperatureAdc(u8 ch)
{
	u16 adcValue = 0;
	//adcValue = GetAdc( 1,ch );
	return adcValue;
}

/***************ADCֵת�����¶�ֵ����********************
* ͨ��ADCֵ�����¶�ֵ
* �����¶�ֵ
******************************************************/
float GetTemperature( void )
{
	u16 temperatureAdc = 0;
	float temperature = 0.0;
 	temperatureAdc = GetTemperatureAdc( 15 );       //ͨ��15ע���ADֵ
	temperature = (float)temperatureAdc*(3.3/4096); //��ǰ�¶ȵ�ѹֵ
	temperature = temperature *10000/(3.3-temperature)/1000; //�¶ȵ�����ֵ
	temperature = (float)1.0/( (float)log(temperature*1000/10000)/3950 + 1.0/(273.16+25) ) - 273.16; //�����¶�
	
	return temperature;
}

float GetInnerTemperature(void)
{
	u16 temperatureAdc = 0;
	float temperature = 0.0;
 	temperatureAdc = GetTemperatureAdc( 16 );       //ͨ��16ע���ADֵ
	temperature = (1.43 - temperatureAdc) / 4.3 + 25;	
	return temperature;
}

/***************����ǿ�ȵ�ADCֵ����********************
* ����ǿ�ȵ�ADCֵ
* chΪͨ��ֵ
* ���ع��յ�ADCֵ
******************************************************/
u16 GetLightAdc(u8 ch)
{
	u16 adcValue = 0;
	GetAdc(1, ch);
	return adcValue;
}

//���¸�ADCֵ
void adcScan(void)
{
		adcTemperature = GetTemperature();
	//	adcLight = GetLightAdc(14);
		adcNavKey = GetNavKey(10);
		//adcInnerTemperature = GetInnerTemperature();
	//	adcNav = GetAdc(1, 10);
		
}


