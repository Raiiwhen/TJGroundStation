/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4] = "0:"; /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
#include "RTC.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
	FRESULT tmp;
	uint32_t br = 0;
	char buff[20] = {0};
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
	tmp = f_mount(&USERFatFS, (TCHAR const*)USERPath, 1);
//	printf("Fatfs mount %d\r\n", tmp);
	
	tmp = f_open(&USERFile,"0:/meow.txt",FA_READ);
//	printf("Fatfs open %d\r\n",tmp);
	
	tmp = f_read(&USERFile,buff,7,&br);
//	printf("Fatfs read %d [%d]\r\n",tmp,br);
//	printf("Fatfs buff %s", buff);
	
	tmp = f_close(&USERFile);
//	printf("Fatfs close %d\r\n",tmp);
	
  /* additional user code for init */     
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
	DWORD rtc = 0;
	
	uint8_t pSrc[8] = {0};
	RTC_Get_Date(&pSrc[0],&pSrc[1],&pSrc[2],&pSrc[3]);
	RTC_Get_Time(&pSrc[4], &pSrc[5], &pSrc[6], &pSrc[7]);
	
	rtc |= (pSrc[0]-20) << 25;
	rtc |= (pSrc[1]) << 21;
	rtc |= (pSrc[2]) << 16;
	rtc |= (pSrc[4]) << 11;
	rtc |= (pSrc[5]) << 5;
	rtc |= (pSrc[6]) / 2;
	
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
