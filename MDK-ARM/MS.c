#include "MS.h"

#define SPI_MS SPI5
#define SYS_FREQ 180000000

uint16_t MS_PROM[8];
int64_t MS_dT;
uint32_t D1, D2;

static uint8_t MS_WRByte(uint8_t data){
	uint8_t res;
	LED_G = 1;

	while(!(SPI_MS->SR&0x0002));//1: TX empty
	SPI_MS->DR = data;
	while(!(SPI_MS->SR&0x0001));//1: RX not empty	
	res = SPI_MS->DR;

	LED_G = 0;	
	return res;
}

void MS_Init(void){
	uint32_t delay = SYS_FREQ/1000;
	
	CS_MS = 0;
	MS_WRByte(MS_RESET);
	CS_MS = 1;
	while(delay--);
	MS_UpdatePROM();
}

uint16_t* MS_UpdatePROM(void){
	uint8_t cnt;
	
	for(cnt=0;cnt<8;cnt++){
		CS_MS = 0;
		MS_WRByte(MS_RD_PROM+cnt*2);
		MS_PROM[cnt] = MS_WRByte(0x00);
		MS_PROM[cnt] <<= 8;
		MS_PROM[cnt] |= MS_WRByte(0x00);		
		CS_MS = 1;
	}
	
	return MS_PROM;
}

void MS_TRIG_Temperature(uint8_t level){
	CS_MS = 0;
	MS_WRByte(MS_TRIG_T + (level>4 ? 4 : level)*2);
	CS_MS = 1;
}

void MS_TRIG_Pressure(uint8_t level){
	CS_MS = 0;
	MS_WRByte(MS_TRIG_P + (level>4 ? 4 : level)*2);
	CS_MS = 1;
}



float MS_Read_Temperature(void){
	int64_t TEMP = 0;
	
	CS_MS = 0;
	MS_WRByte(MS_RD_ADC);
	D2 =  MS_WRByte(0x00);
	D2 <<= 8;
	D2 |= MS_WRByte(0x00);
	D2 <<= 8;
	D2 |= MS_WRByte(0x00);
	CS_MS = 1;
	
	MS_dT = (float)D2 - (float)MS_PROM[5] * 256;
	TEMP = 2000 + ((MS_dT * MS_PROM[6]) / 8388608);
	
	if(TEMP> 8500)TEMP = 8500;
	if(TEMP<-4000)TEMP = -4000;
	
	return (float)TEMP/100;
}

float MS_Read_Pressure(void){
	int64_t SENSE, D1, P;
	int64_t OFF = ((float)MS_PROM[2] * 32768) + (float)MS_PROM[4]*MS_dT / 128;
	
	CS_MS = 0;
	MS_WRByte(MS_RD_ADC);
	D1 =  MS_WRByte(0x00);
	D1 <<= 8;
	D1 |= MS_WRByte(0x00);
	D1 <<= 8;
	D1 |= MS_WRByte(0x00);
	CS_MS = 1;
	
	SENSE = (int64_t)MS_PROM[1] * 32768 + (int64_t)MS_PROM[3] * MS_dT / 256;
	P = (D1 * SENSE / 2097152 - OFF) / 32768;
	
	return (float)P/100;
}
