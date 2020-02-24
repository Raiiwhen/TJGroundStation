#include "stm32f4xx.h"

extern uint32_t ADC_TMP[64];

float get_Temperature(float* temp);
float get_BAT(float* temp);
