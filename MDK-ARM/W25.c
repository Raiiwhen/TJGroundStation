#include "W25.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

#define SPI_W25 SPI4
#define __10ms 1800000
#define __PAGE_SIZE 2048
#define __DIE_SIZE 1024

static void delay_ms(uint16_t ms){
	uint32_t cnt; 
	while(ms--){
		for(cnt=180000;cnt>0;cnt--);
	}
}

static uint8_t W25_WRByte(uint8_t data){
	while(!(SPI_W25->SR & 0x0002));//TXE 1: TX empty
	SPI_W25 -> DR = data;
	while(!(SPI_W25->SR & 0x0001));//RNE 1: RX not empty
	data = SPI_W25->DR;
	return data;
}

static void W25_DieSw(uint8_t die){
	CS_W25 = 0;
	W25_WRByte(0xc2);
	W25_WRByte(die);
	CS_W25 = 1;
}

static void W25_CFG(uint8_t reg, uint8_t data){
	if(reg!=0xa0 && reg!=0xb0 && reg!= 0xc0)return;
	CS_W25 = 0;
	W25_WRByte(0x1f);
	W25_WRByte(reg);
	W25_WRByte(data);
	CS_W25 =1;
}

uint8_t W25_Init(void){
/*set logic pin*/
	CS_W25 = 1;
	W25_WP = 1;
	W25_HD = 1;
/*softwate RST*/
	CS_W25 = 0;
	W25_WRByte(0xff);
	CS_W25 = 1;
	delay_ms(5);
/*select die 0*/
	W25_DieSw(0);
/*cfg*/
	W25_CFG(0xa0,0x00);//rst block protect bits
	return 0;
}


void W25_ID(uint32_t* id, uint8_t die){
	uint32_t data = 0;
	uint8_t buff;
	
	CS_W25 = 0;
	W25_WRByte(W25_CMD_ID);
	W25_WRByte(0xff);
	buff = W25_WRByte(0xff);
	data |= (uint32_t)buff << 16;
	buff = W25_WRByte(0xff);
	data |= (uint32_t)buff << 8;
	buff = W25_WRByte(0xff);
	data |= (uint32_t)buff << 0;
	CS_W25 = 1;
	
	*id = data;
}

uint32_t W25_STA(void){
	uint32_t sta;
	uint8_t data;
	
	CS_W25 = 0;
	W25_WRByte(0x0f);
	W25_WRByte(0xa0);
	data = W25_WRByte(0xff);
	CS_W25 = 1;
	sta = (uint32_t)data<<16;

	CS_W25 = 0;
	W25_WRByte(0x0f);
	W25_WRByte(0xb0);
	data = W25_WRByte(0xff);
	CS_W25 = 1;
	sta |= (uint32_t)data<<8;

	CS_W25 = 0;
	W25_WRByte(0x0f);
	W25_WRByte(0xc0);
	data = W25_WRByte(0xff);
	CS_W25 = 1;
	sta |= (uint32_t)data<<0;

	return sta;
}

uint32_t W25_ErBlock(uint32_t _block){
	uint32_t cnt;
	
	_block *= 64;
	if(_block>65536)_block=65535;

	/*write enable*/
	CS_W25 = 0;
	W25_WRByte(0x06);
	CS_W25 = 1;
	
	/* erase */
	CS_W25 = 0;
	W25_WRByte(0xd8);
	W25_WRByte(0xff);
	W25_WRByte((uint8_t)(_block>>8));
	W25_WRByte((uint8_t)(_block>>0));
	CS_W25 = 1;
	
	while(W25_STA()&0x00000001){LED_G = 1;if(cnt++ > __10ms)return cnt;}LED_G = 0;
	
	return cnt;
}

uint32_t  W25_WrPage(uint32_t page, uint8_t *pSrc, uint16_t length){
	uint32_t cnt;
	if(length>__PAGE_SIZE)length=__PAGE_SIZE;
	if(page>65536)length=65535;
	/*Write Enable*/
	CS_W25 = 0;
	W25_WRByte(0x06);
	CS_W25 = 1;
	delay_ms(1);
	/*write buffer*/
	CS_W25 = 0;
	W25_WRByte(0x84);
	W25_WRByte((uint8_t)((__PAGE_SIZE-length)>>8));
	W25_WRByte((uint8_t)((__PAGE_SIZE-length)>>0));
	for(cnt=0; cnt<length; cnt++){
		W25_WRByte(*(pSrc+cnt));
	}
	CS_W25 = 1;
	delay_ms(1);
	/*execute*/
	CS_W25 = 0;
	W25_WRByte(0x10);
	W25_WRByte(0xff);
	W25_WRByte((uint8_t)(page>>8));
	W25_WRByte((uint8_t)(page));
	CS_W25 = 1;
	/*wait for writing*/
	while(W25_STA()&0x00000001){LED_G = 1;if(cnt++ > __10ms)return cnt;}LED_G = 0;
	
	return cnt;
}

uint32_t W25_RdPage(uint32_t page, uint8_t *pDst, uint32_t length){
	uint32_t cnt = 0;
	if(length>__PAGE_SIZE)length=__PAGE_SIZE;
	
	/* read to buffer */
	CS_W25 = 0;
	W25_WRByte(0x13);
	W25_WRByte(0xff);
	W25_WRByte((uint8_t)(page>>8));
	W25_WRByte((uint8_t)(page));
	CS_W25 = 1;
	while(W25_STA()&0x00000001){LED_G = 1;if(cnt++ > __10ms)return cnt;}LED_G = 0;
	
	/* clk out data to pDst*/
	CS_W25 = 0;
	W25_WRByte(0x03);
	W25_WRByte((uint8_t)((__PAGE_SIZE-length)>>8));
	W25_WRByte((uint8_t)((__PAGE_SIZE-length)>>0));
	W25_WRByte(0xff);
	for(cnt=0; cnt<length; cnt++)pDst[cnt]=W25_WRByte(0xff);
	CS_W25 = 1;
	
	return cnt;
}

uint32_t W25_WrStr(uint32_t page, uint8_t *pSrc, uint32_t length){
	uint8_t buff[2048];
	uint32_t cnt=0, _page=0, _block = 0;
	uint32_t tout = 0;
	
	_page	 = page%64;
	_block = page/64;
	/* erase buffer block */
	tout += W25_ErBlock(1023);
	/* copy to buffer block */
	for(cnt=0; cnt<64; cnt++){
		tout += W25_RdPage(_block*64 + cnt, buff, 2048);
		tout += W25_WrPage(65408		 + cnt, buff, 2048);//block 1023 start from page 65408
	}
	
	/* erase taget block */
	tout += W25_ErBlock(_block);
	/* copy back and insert data*/
	for(cnt=0; cnt<64; cnt++){
		tout += W25_RdPage(65408		 + cnt, buff, 2048);//block 1023 start from page 65408
		if(cnt == _page)memcpy(buff,pSrc,length);
		tout += W25_WrPage(_block*64 + cnt, buff, 2048);
	}
	
	return tout;
}


