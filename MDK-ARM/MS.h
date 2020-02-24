#ifndef __MS_H_
#define __MS_H_

#include "stm32f4xx.h"
#include "main.h"
/*
MS5611 SPI interface up to 20Mhz serial clock
*/
/*
refered from

https://www.te.com/commerce/DocumentDelivery/DDEController?Action
=srchrtrv&DocNm=MS5611-01BA03&DocType=Data+Sheet&DocLang=English
*/

/*CMD*/
#define MS_RESET 0x1e //at least 2.8ms delay after reload
#define MS_RD_ADC 0x00
#define MS_RD_PROM 0xa0 //0xa0 + 2*cnt, cnt[0~7] PROM[0xa0~0xae]
#define MS_TRIG_T 0x40 //0x40 + 2*cnt, cnt[0~4] TRIG[0x40~0x48]
#define MS_TRIG_P 0x50 //0x50 + 2*cnt, cnt[0~5] TRIG[0x50~0x58]

void MS_Init(void);
uint16_t* MS_UpdatePROM(void);
void MS_TRIG_Temperature(uint8_t level);
void MS_TRIG_Pressure(uint8_t level);
float MS_Read_Temperature(void);
float MS_Read_Pressure(void);

#endif
