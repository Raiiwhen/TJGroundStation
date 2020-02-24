#include "pwr_mgt.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f429xx.h"
#include "main.h"

uint16_t ADC_TMP[16];
extern ADC_HandleTypeDef hadc1;

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

void SYS_STDBY(void){
	
  __HAL_RCC_AHB1_FORCE_RESET();       //复位所有IO口 
//	RCC->AHB1RSTR = 0xffffffff;	//RESET all IO ports
	   
	__HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟
//	RCC->APB1ENR = RCC_APB1ENR_PWREN;//enable power management clock
	__HAL_RCC_BACKUPRESET_FORCE();      //复位备份区域
	HAL_PWR_EnableBkUpAccess();         //后备区域访问使能  
//	RCC->BDCR;

  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //清除Wake_UP标志
//	PWR->CR |= PWR_FLAG_WU << 2;
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //设置WKUP用于唤醒
//	/*set PA0 as awake mode*/
//	PWR->CSR = 0x00000100;
	HAL_PWR_EnterSTANDBYMode();                         //进入待机模式   
//	/*enter stdby*/
//	PWR->CR |= PWR_CR_PDDS;
//	SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP_Msk;
//	/*core stdby, wait for PA0 rising edge*/
//	__wfi();
}
