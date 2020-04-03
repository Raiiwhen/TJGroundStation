#include "IMU.h"
#include "mat.h"
#include <stdio.h>

#define SCALE_2G 0.00060425
#define SCALE_2000dps 0.3051758

v3 acc,gyro;
short tmp;

int IMU_exe(short* raw){
	/*convert to SI*/
	tmp = raw[0];
	acc.x = raw[0] * SCALE_2G;
	acc.y = raw[1] * SCALE_2G;
	acc.z = raw[2] * SCALE_2G;
	gyro.x = raw[4] * SCALE_2000dps;
	gyro.y = raw[5] * SCALE_2000dps;
	gyro.z = raw[6] * SCALE_2000dps;
	/*Kalmen filter*/
	
	
	return 0;
}

float get_IMU_tmp(void){
	char str[100] = {0};
	float buff[18] = {0,0,0,1,12,1,0,0,1,12,1,0,0,1,12,1,0,0};
	
	matrix A = {0}, B = {0}, C = {0}, D = {0};
	A = mat_unit(3);
	B = mat_unit(3);
	C = mat_add(A,B);
	C = mat_mult(C,C);
	mat_init(&D, 6, 3);
	mat_fill(&D, buff);
	//printf("%s",mat_put(D,str));
	
	return tmp;
}
