#include "NAND.h"
#include "main.h"
#include "stm32f4xx_it.h"

#define PAGE_SIZE 8628
uint32_t NAND_BSY_WIDTH;

#define _BSY_PRE      100 //tWB 100ns
#define _BSY_RST 10000000 //tRST 10ms
#define _BSY_ER  10000000 //tBERS 10ms
#define _BSY_WR   5000000 //tPROG 5ms
#define _BSY_RD    400000 //tR 400us

static uint8_t NAND_WAIT_RB_RISING(uint32_t ns){
	LED_G = 1; 
	NAND_BSY_WIDTH = 0;
	ns = (uint32_t)(ns/5.5 + 1);
	while(NAND_BSY == 1){
		NAND_BSY_WIDTH++;
		if(NAND_BSY_WIDTH == _BSY_PRE)return 0;
	}
	while(NAND_BSY == 0){
		NAND_BSY_WIDTH++;
		if(NAND_BSY_WIDTH == ns)return 0;
	}
	LED_G = 0;
	
	return 1;
}

void delay_ms(uint32_t ms){
	while(ms--);
}

uint8_t  NAND_WrPage(uint32_t page, uint8_t *pSrc, uint16_t col){
  static uint32_t ColAddr = 0U, RowAddr = 0U;
  static uint32_t cnt , _page, _block;
    
  /* NAND raw address calculation */
	if(col > PAGE_SIZE) col = PAGE_SIZE;
	if(page > 265727) page = 265727;
	_block = page / 128;
	_page = page % 128;
	ColAddr  = col;
	RowAddr  = ((_page<<17) | (_block<<5))&0x00ff0000;//3rd
	RowAddr |= 								 (_block<<5)&0x0000ff00;//4th
	RowAddr |= 								 (_block<<5)&0x000000ff;//5th
	
	/* Send write page command sequence */
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = NAND_CMD_WRITE0;

	/* Send Addr, last addr byte by ATTRIBUTE space to fit tADL*/
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_1ST_CYCLE(ColAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_2ND_CYCLE(ColAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_1ST_CYCLE(RowAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_2ND_CYCLE(RowAddr);
	*(__IO uint8_t *)((uint32_t)(0x88000000   | ADDR_AREA)) = ADDR_3RD_CYCLE(RowAddr);
	/*tADL 300ns*/
	for(cnt = 0; cnt < 60; cnt++){}
	/* Write data to memory */
	for(cnt = 0; cnt < PAGE_SIZE; cnt++)
	{
		*(__IO uint8_t *)NAND_DEVICE2 = pSrc[cnt];
	}
 
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;

	/* waiting ready */
	if(!NAND_WAIT_RB_RISING(_BSY_WR))return 0x04|NAND_Status_70H();
  
  return NAND_Status_70H();
}

uint8_t  NAND_ErBlock(uint32_t _block){
	__IO uint32_t RowAddr = 0U;

  /* NAND raw address calculation */
	if(_block > 2076)_block = 2075;
	RowAddr  = (_block<<5)&0x00ff0000;//3rd
	RowAddr |= (_block<<5)&0x0000ff00;//4th
	RowAddr |= (_block<<5)&0x000000ff;//5th	
	
  /* Send Erase block command sequence */
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = NAND_CMD_ERASE0;

  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_1ST_CYCLE(RowAddr);
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_2ND_CYCLE(RowAddr);
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_3RD_CYCLE(RowAddr);
    
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = NAND_CMD_ERASE1; 
	  
  /* wait for const delay 1.5(<10) ms */
	if(!NAND_WAIT_RB_RISING(_BSY_ER))return 0x04|NAND_Status_70H();
	
	return NAND_Status_70H();  
}

uint8_t  NAND_RdPage(uint32_t page, uint8_t *pDst, uint16_t col){
  uint32_t ColAddr = 0U, RowAddr = 0U;
  uint32_t cnt = 1, _page, _block;
    
  /* NAND raw address calculation */
	if(col > PAGE_SIZE) col = PAGE_SIZE;
	if(page > 265727) page = 265727;
	_block = page / 128;
	_page = page % 128;
	ColAddr  = col;
	RowAddr  = ((_page<<17) | (_block<<5))&0x00ff0000;//3rd
	RowAddr |= 								 (_block<<5)&0x0000ff00;//4th
	RowAddr |= 								 (_block<<5)&0x000000ff;//5th
	
	/*1st byte of read page command*/
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = NAND_CMD_AREA_A;
 
	/* Send Address */
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_1ST_CYCLE(ColAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_2ND_CYCLE(ColAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_1ST_CYCLE(RowAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_2ND_CYCLE(RowAddr);
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = ADDR_3RD_CYCLE(RowAddr);

	/*2nd byte of read page command*/
	*(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA))  = NAND_CMD_AREA_TRUE1;
	
	/*wait for cache ready (a rising edge on R/B pin)*/
	if(!NAND_WAIT_RB_RISING(_BSY_RD))return 0x04|NAND_Status_70H();
	
	/* Get Data into Buffer */    
	for(cnt=0 ; cnt < 8628 - ColAddr; cnt++)
	{
		pDst[cnt] = *(uint8_t *)NAND_DEVICE2;
	}
	
  return NAND_Status_70H();
}


uint8_t NAND_Status_70H(void){
  uint8_t data = 0U;
  /* Send Read status operation command */
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = NAND_CMD_STATUS;
  /*tWHR 60 ns*/
	data = 12;
	while(data--){}
  /* Read status register data */
  data = *(__IO uint8_t *)((uint32_t) NAND_DEVICE2);

  /* Return the status */
	return data;
}

uint8_t NAND_RST(void){
   /* Send RST command sequence */   
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA)) = 0xFF;
	/*wait for cache ready (a rising edge on R/B pin)*/
	if(!NAND_WAIT_RB_RISING(_BSY_RST))return 0x40|NAND_Status_70H();
  
	return NAND_Status_70H();
}

uint8_t  NAND_ID(uint32_t* ID){
  __IO uint32_t data = 0U;
  __IO uint16_t _data = 0U;
	
  /* Send Read ID command sequence */   
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | CMD_AREA))  = NAND_CMD_READID;
  *(__IO uint8_t *)((uint32_t)(NAND_DEVICE2 | ADDR_AREA)) = 0x00;
	
  /* Read result sequence 6 byte in all*/   
	data = *(__IO uint32_t *)NAND_DEVICE2;
	_data = *(__IO uint16_t *)NAND_DEVICE2;
	
	/* Return the data read */
	ID[0] = data;
	ID[1] = _data;	
	
  return data;
}



