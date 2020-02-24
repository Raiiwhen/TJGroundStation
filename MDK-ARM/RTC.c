#include "RTC.h"
#include "stm32f429xx.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

static uint8_t RTC_DEC2BCD(uint8_t val)
{
	uint8_t high4bit = 0; 
	while(val >= 10){
		high4bit++;
		val -= 10;
	}
	//choose last 20[dec] to return
	return ((uint8_t)(high4bit<<4)|val);
}

static uint8_t RTC_BCD2DEC(uint8_t val)
{
	uint8_t temp=0;
	temp=(val>>4)*10;//high 4 bit
	return (temp+(val&0X0F));
}

static void RTC_Write_BKR(uint32_t BKRx,uint32_t data)
{
	uint32_t temp=0; 
	temp=RTC_BASE+0x50+BKRx*4;   
	(*(uint32_t*)temp)=data; 
}

uint32_t RTC_Read_BKR(uint32_t BKRx, uint32_t* data){
	*data = (*(uint32_t*)(RTC_BASE+0x50+BKRx*4));
	return *data;
}
uint8_t RTC_Init(void){
	uint32_t bkpflag=0;
	uint32_t tempreg=0;
	RCC->APB1ENR|=1<<28;					//enable power clock
	PWR->CR|=1<<8;								//enable backup register
	bkpflag = *(uint32_t*)(RTC_BASE+0x50);		//read BKP0
	if(bkpflag!=0X5050)						//never set before
	{
		/*enable clock source LSE*/
		RCC->CSR|=1<<0;							//ENABLE LSI
		while(!(RCC->CSR&0x02));		//LSI Ready
		RCC->BDCR|=1<<0;						//ENABLE LSE
		while((RCC->BDCR&0X02)==0);	//LSE Ready
		tempreg  = RCC->BDCR;				//read BDCR
		tempreg &= ~(3<<8);					//00: RTC clock, no clock 
		tempreg |= 1<<8;						//01: RTC clock, LSE
		tempreg |= 1<<15;						//1: enable RTC
		RCC->BDCR = tempreg;
		/*disable write protect*/
		RTC->WPR = 0xCA;
		RTC->WPR = 0x53;
		/*erase RTC*/
		RTC->CR = 0;
		/*enter RTC init mode*/
		RTC->ISR |=  1<<7;
		while(!RTC->ISR & (1<<6));
		/*RTC program*/
		RTC->PRER=0XFF;				//Frtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC->PRER|=0X7F<<16;
		RTC->CR&=~(1<<6);//24h mode
		/*esc RTC init mode*/
		RTC->ISR&=~(1<<7);
		/*write protect*/
		RTC->WPR=0xFF;
		if(bkpflag!=0X5051)
		{
			RTC_Set_Time(10,54,56,0);
			RTC_Set_Date(20,01,27,7);
		}
		RTC_Write_BKR(0,0X5050);			  //using LSE
	}
	return 0;
}

void RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	static uint32_t temp=0;
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	RTC->ISR |= 1<<7; 					//enter init mode
	while(!(RTC->ISR&(1<<6)));	//wait for enable
	temp =	
			(((uint32_t)	ampm&0X01)				<< 22	)
			|((uint32_t)	RTC_DEC2BCD(hour)	<< 16	)
			|((uint32_t)	RTC_DEC2BCD(min)	<< 8	)
			|(						RTC_DEC2BCD(sec)				);
	RTC->TR=temp;
	RTC->ISR&=~(1<<7);
}


void RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	uint32_t temp=0;
	RTC->WPR=0xCA;
	RTC->WPR=0x53;
	RTC->ISR |= 1<<7; 					//enter init mode
	while(!(RTC->ISR&(1<<6)));	//wait for enable 
	temp = 
			(((uint32_t) week&0X07)				 	<<13	)
			|((uint32_t) RTC_DEC2BCD(year)	<<16	)
			|((uint32_t) RTC_DEC2BCD(month) <<8		)
			|(					 RTC_DEC2BCD(date)		 		); 
	RTC->DR=temp;
	RTC->ISR&=~(1<<7);
}

static uint8_t RTC_Wait_Synchro(void)
{
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	RTC->ISR&=~(1<<5);//bit 5 set to 0
	while(!(RTC->ISR&(1<<5)));
	RTC->WPR=0xFF;
	return 0;
}

void RTC_Get_Time(uint8_t *hour,uint8_t *min,uint8_t *sec,uint8_t *ampm)
{
	uint32_t temp=0;
 	while(RTC_Wait_Synchro());
	temp=RTC->TR;
	*hour=RTC_BCD2DEC((temp>>16)&0X3F);
	*min=RTC_BCD2DEC((temp>>8)&0X7F);
	*sec=RTC_BCD2DEC(temp&0X7F);
	*ampm=temp>>22; 
}

void RTC_Get_Date(uint8_t *year,uint8_t *month,uint8_t *date,uint8_t *week)
{
	uint32_t temp=0;
 	while(RTC_Wait_Synchro());
	temp=RTC->DR;
	*year=RTC_BCD2DEC((temp>>16)&0XFF);
	*month=RTC_BCD2DEC((temp>>8)&0X1F);
	*date=RTC_BCD2DEC(temp&0X3F);
	*week=(temp>>13)&0X07; 
}

uint32_t RTC_raw2str(uint8_t* pSrc, char* pDst){
	char temp[]=" 17:45:52";
	sprintf(pDst,"20%02d-%02d-%02d",pSrc[0], pSrc[1], pSrc[2]);
	sprintf(temp, " %02d:%02d:%02d",pSrc[4], pSrc[5], pSrc[6]);
	strcat(pDst, temp);
	switch(pSrc[3]){
		case 1:strcat(pDst,", Mon.");break;
		case 2:strcat(pDst,", Tues.");break;
		case 3:strcat(pDst,", Wed.");break;
		case 4:strcat(pDst,", Thurs.");break;
		case 5:strcat(pDst,", Fri.");break;
		case 6:strcat(pDst,", Satur.");break;
		case 7:strcat(pDst,", Sun.");break;
		default:strcat(pDst,"[ERR]");
	}	
	
	return 0;
}
