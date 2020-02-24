#include "SD.h"
#include "string.h"
#include "stdio.h"
#include "stm32f4xx_hal_sd.h"

extern SD_HandleTypeDef hsd;

uint32_t SD_RdBlock(uint32_t block, uint8_t *pDst, uint32_t length){
	uint8_t res = 0;
	res = HAL_SD_ReadBlocks(&hsd,pDst,block,length / hsd.SdCard.BlockSize, 10);
	res <<= 4;
	res |= HAL_SD_GetState(&hsd);
	return res;
}

uint32_t SD_WrBlock(uint32_t block, uint8_t *pSrc, uint16_t length){
	uint8_t res = 0;
	res = HAL_SD_WriteBlocks(&hsd,pSrc,block,length / hsd.SdCard.BlockSize, 10);
	res <<= 4;
	res |= HAL_SD_GetState(&hsd);
	return res;
}


char* SD_info(void){
	//template "SDHC  Ver.1 Class[1461], 3.40 GB";
	static char info[40] = {0};
	char size[10] = {0};
	switch(hsd.SdCard.CardType){
		case CARD_SDSC:memcpy(info,"SDSC ",5);break;
		case CARD_SDHC_SDXC:memcpy(info,"SDHC ",5);break;
		case CARD_SECURED:memcpy(info,"SDV1 ",5);break;
		default:memcpy(info,"[err]",5);
	}
	switch(hsd.SdCard.CardVersion){
		case CARD_V1_X:strcat(info,"V1 ");break;
		case CARD_V2_X:strcat(info,"V2 ");break;
		default: strcat(info,"[ERR]");
	}
	sprintf(size,"%.1f GB",hsd.SdCard.BlockNbr * hsd.SdCard.BlockSize /1024 / 1024/ 1024.0f );
	strcat(info, size);
	
	return info;
}



