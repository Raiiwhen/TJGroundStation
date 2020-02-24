#ifndef __IMU_H_
#define __IMU_H_

typedef struct{
	float x;
	float y;
	float z;
}v3;

int IMU_exe(short* raw);
void update_IMU_var(float feed);
float get_IMU_tmp(void);
#endif
