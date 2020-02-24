#ifndef __NAND_H_
#define __NAND_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal_nand.h"

//HAL_StatusTypeDef  NAND_Config(NAND_DeviceConfigTypeDef *pDeviceConfig);
uint8_t  NAND_ID(uint32_t *pNAND_ID);
uint8_t  NAND_RST(void);

uint8_t  NAND_RdPage(uint32_t page, uint8_t *pDst, uint16_t col);
uint8_t  NAND_WrPage(uint32_t page, uint8_t *pSrc, uint16_t col);
uint8_t  NAND_RdSpare(uint32_t page, uint8_t *pDst);
uint8_t  NAND_WrSpare(uint32_t page, uint8_t *pSrc);
uint8_t  NAND_ErBlock(uint32_t _block);

uint8_t NAND_Status_70H(void);

#endif
