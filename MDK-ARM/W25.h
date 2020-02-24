#ifndef __W25_H_
#define __W25_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal_spi.h"

/*based on W25M02GVZEJG*/

#define W25_CMD_ID		0x9F
#define W25_CMD_RSTA	0x0F
#define W25_CMD_WSTA	0x1F
#define W25_CMD_EN_W	0x06
#define W25_CMD_DIS_W	0x04
#define W25_CMD_ERA		0xD8
#define W25_CMD_WR		0x02
#define W25_CMD_RWR		0x84
#define W25_CMD_PEXT	0x10
#define W25_CMD_PRD		0x13
#define W25_CMD_RD		0x03
#define W25_CMD_FRD		0x0B

void W25_ID(uint32_t* id, uint8_t die);
uint8_t W25_Init(void);
uint32_t W25_STA(void);

uint32_t W25_RdPage(uint32_t page, uint8_t *pDst, uint32_t length);
uint32_t W25_ErBlock(uint32_t _block);
uint32_t W25_WrPage(uint32_t page, uint8_t *pSrc, uint16_t length);
uint32_t W25_WrStr(uint32_t page, uint8_t *pSrc, uint32_t length);

#endif
