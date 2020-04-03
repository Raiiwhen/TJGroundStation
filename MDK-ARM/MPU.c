#include "MPU.h"

#define SPI_MPU SPI1

uint8_t MPU_RDY;

static void delay_ms(uint16_t ms){
	uint32_t cnt; 
	while(ms--){
		for(cnt=180000;cnt>0;cnt--);
	}
}

uint8_t MPU_isRDY(void){
	return MPU_RDY;
}

static uint8_t MPU_WRByte(uint8_t data){
	while(!(SPI_MPU->SR&0x0002));
	SPI_MPU->DR = data;
	while(!(SPI_MPU->SR&0x0001));
	data = SPI_MPU->DR;
	return data;
}


uint8_t MPU_Rd_Reg(uint8_t reg){
	uint8_t data;
	CS_MPU = 0;
	MPU_WRByte(reg|0x80);
	data = MPU_WRByte(0xff);
	CS_MPU =1;
	return data;	
}

void MPU_Wr_Reg(uint8_t reg, uint8_t data){
	CS_MPU = 0;
	MPU_WRByte(reg&0x7f);
	MPU_WRByte(data);
	CS_MPU =1;
}

void IMU_Init(void){
	MPU_WRByte(0xff);
	delay_ms(1);
	MPU_Wr_Reg(107,0x80);//rst
	delay_ms(1);
	MPU_Wr_Reg(107,0x01);//auto select clock
	delay_ms(1);
	MPU_Wr_Reg(104,0x07);//rst all sensors
	delay_ms(1);
	MPU_Wr_Reg(106,0x01);//rst all data reg
	delay_ms(1);
	MPU_Wr_Reg(112,0x40);//disable dmp
	delay_ms(1);
	MPU_Wr_Reg(108,0x00);//enable all 6 axises
	delay_ms(1);
	MPU_Wr_Reg(25,0x00);//sample rate at 1kHz
	delay_ms(1);
	MPU_Wr_Reg(26,0x04);//set LPF 20Hz, band 20Hz, dealy 9.9ms at smp rate 1KHz
	delay_ms(1);
	MPU_Wr_Reg(27,3<<3);//gyro: +2000dps
	delay_ms(1);
	MPU_Wr_Reg(28,0<<3);//accel: +-2g
	delay_ms(1);
	MPU_Wr_Reg(29,0x04);//accel LPF 20Hz, 19.8ms at smp rate 1kHz
	delay_ms(1);
	MPU_Wr_Reg(35,0x00);//disable FIFO
	delay_ms(1);
	MPU_Wr_Reg(55,0x10);//INT Pin: rising, PP, 50us pulse; clr by any read
	delay_ms(1);
	MPU_Wr_Reg(56,0x01);//raw ready INT
	
	MPU_RDY = 1;
}

uint8_t MPU_ID(uint8_t* id){
	CS_MPU = 0;
	MPU_WRByte(0x75+0x80);
	*id = MPU_WRByte(0xff);
	CS_MPU =1;
	return *id;
}

static float MPU_T;

void MPU_Rd_Raw(short* raw){
	uint8_t cnt,temp0, temp1;
	CS_MPU = 0;
	MPU_WRByte(59|0x80);
	for(cnt = 0; cnt < 7; cnt++){
		temp0 = MPU_WRByte(0xff);
		temp1 = MPU_WRByte(0xff);
		raw[cnt] = ((uint8_t)temp0<<8) | temp1;
	}
	CS_MPU =1;
	
	/*data transfer*/
	MPU_T = (double)raw[3]/333.87 + 21.0f;
}


float get_MPU_T(void){
	return MPU_T;
}

