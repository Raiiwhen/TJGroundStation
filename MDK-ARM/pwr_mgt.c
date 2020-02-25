#include "pwr_mgt.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f429xx.h"
#include "main.h"

uint16_t ADC_TMP[16];
extern ADC_HandleTypeDef hadc1;

void SYS_STDBY(void){
	uint32_t tempreg;
 	RCC->AHB1RSTR|=0X01FE;
  while(WK_UP);
	RCC->AHB1RSTR|=1<<0;
	/*config RTC INT*/
	RCC->APB1ENR|=1<<28;
	PWR->CR|=1<<8;
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	tempreg=RTC->CR&(0X0F<<12);
	RTC->CR&=~(0XF<<12);
	RTC->ISR&=~(0X3F<<8);
	PWR->CR|=1<<2; 
	RTC->CR|=tempreg;
	RTC->WPR=0xFF;
	/*Config STD_BY*/
	SCB->SCR|=1<<2;				//enable sleep deep bit (SYS->CTRL)	   
	RCC->APB1ENR|=1<<28;	//enable pwr_mgt clock
	PWR->CSR|=1<<8;     	//config WKUP Key
	PWR->CR|=1<<2;      	//clear WKUP flag
	PWR->CR|=1<<1;      	//set PDDS
	/*__wfi cmd*/
	__ASM volatile("wfi");		  

}

float get_BAT(void){
	uint8_t cnt;
	uint32_t sum = 0;
	float data = 0;
	
	for(cnt = 0; cnt < 16; cnt++){
		sum += ADC_TMP[cnt];
	}
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_TMP,16);
	data = sum /16 * 3.0f / 4096;
	if(data < 2.7f)
		BAT_CTL = 1;
	else
		BAT_CTL = 0;
	return data;
}

float get_TMP(void){
	uint8_t cnt;
	uint32_t sum = 0;
	float data = 0;
	
	for(cnt = 0; cnt < 16; cnt++){
		sum += ADC_TMP[cnt];
	}
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_TMP,16);
	data = sum /16 * 3.0f / 4096;
	if(data < 2.7f)
		BAT_CTL = 1;
	else
		BAT_CTL = 0;
	return data;
}
