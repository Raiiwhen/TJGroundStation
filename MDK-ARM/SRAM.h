#ifndef _K4S561632_H
#define _K4S561632_H

#include "main.h"

#define K4S561632J_SIZE		                     0x2000000 //64MB address space

/* Data BaseAddress */
#define SDRAM_BANK_ADDR              ((uint32_t)0xC0000000)
#define SDRAM_END_ADDR							 ((uint32_t)0xC0FFFFFF)

#define SDRAM_RW16(ADDR)						 *(__IO uint16_t*)(ADDR + SDRAM_BANK_ADDR)
#define SDRAM_RW8(ADDR)							 *(__IO uint16_t*)(ADDR + SDRAM_BANK_ADDR)

uint32_t SRAM_Init(void);

#endif
