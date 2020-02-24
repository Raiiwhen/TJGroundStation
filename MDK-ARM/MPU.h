#ifndef __MPU_H_
#define __MPU_H_

#include "stm32f4xx.h"
#include "main.h"

void IMU_Init(void);
uint8_t MPU_ID(uint8_t* id);
uint8_t MPU_Rd_Reg(uint8_t reg);
void MPU_Rd_Raw(short* raw);
float get_MPU_T(void);

#endif
