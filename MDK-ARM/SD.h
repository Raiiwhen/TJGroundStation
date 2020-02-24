#ifndef __SD_H_
#define __SD_H_

#include "stm32f4xx.h"
uint32_t SD_Init(void);

uint32_t SD_RdBlock(uint32_t block, uint8_t *pDst, uint32_t length);
uint32_t SD_WrBlock(uint32_t block, uint8_t *pSrc, uint16_t length);
uint32_t SD_WrStr(uint32_t page, uint8_t *pSrc, uint32_t length);

char* SD_info(void);

#endif
