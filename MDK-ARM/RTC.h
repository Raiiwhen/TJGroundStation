#ifndef __RTC_H_
#define __RTC_H_

#include "stm32f4xx.h"

uint8_t RTC_Init(void);

/*set time sequence */
void RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);
void RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);

/*Back up register powered by VBAT pin, 20 register(32bit each) available.*/
/*using BKP0 0~3bit as RTC congfiure information*/
uint32_t RTC_Read_BKR(uint32_t BKRx, uint32_t* data);

/*read time sequence */
void RTC_Get_Time(uint8_t *hour,uint8_t *min,uint8_t *sec,uint8_t *ampm);
void RTC_Get_Date(uint8_t *year,uint8_t *month,uint8_t *date,uint8_t *week);

/*string process*/
uint32_t RTC_raw2str(uint8_t* pSrc, char* pDst);

#endif
