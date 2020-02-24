#include "SRAM.h"
#include "stm32f4xx_hal_sdram.h"

extern SDRAM_HandleTypeDef hsdram1;

/****
	*	PD0-FMC_D2,PD1-FMC_D3,PD4-FMC_NOE,PD5-FMC_NWE,PD7-FMC_NE1,PD8-FMC_D13
	*	PD9-FMC_D14,PD10-FMC_D15,PD11-FMC_CLE,PD12-FMC_ALE,PD14-FMC_D0,PD15-FMC_D1
	*	PE0-FMC_NBL0,PE1-FMC_NBL1,PE7-FMC_D4,PE8-FMC_D5,PE9-FMC_D6,PE10-FMC_D7,PE11-FMC_D8
	*	PE12-FMC_D9,PE13-FMC_D10,PE14-FMC_D11,PE15-FMC_D12,PF0-FMC_A0,PF1-FMC_A1,PF2-FMC_A2
	*	PF3-FMC_A3,PF4-FMC_A4,PF5-FMC_A5,PF11-FMC_SDRAS,PF12-FMC_A6,PF13-FMC_A7,PF14-FMC_A8,
	*	PF15-FMC_A9,PG0-FMC_A10,PG1-FMC_A11,PG2-FMC_A12,PG4-FMC_BA0,PG5-FMC_BA1,PG7-FMC_INT3
	*	PG8-FMC_SDCLK,PG9-FMC_NCE3,PG15-FMC_SDCAS,PH2-FMC_SDCKE0,PH3-FMC_SDNE0,PH5-FMC_SDNWE
*****/

/*using SRAM of SAMSUNG K4S561632J*/

/* Bank selection */
#define FMC_BANK_SDRAM                           FMC_Bank1_SDRAM  

#define FMC_COMMAND_TARGET_BANK                  FMC_Command_Target_bank1

/* Data width */
#define SDRAM_MEMORY_WIDTH                       FMC_SDMemory_Width_16b 

/* SDRAM CAS Latency */
#define SDRAM_CAS_LATENCY                        FMC_CAS_Latency_2  

#define SDCLOCK_PERIOD                           FMC_SDClock_Period_3        /* Default configuration used with LCD */

#define SDRAM_READBURST                          FMC_Read_Burst_Enable  

/* The relevant definition of FMC SDRAM mode configuring register */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)   

uint32_t SRAM_Init(void){
	uint32_t TEMP = 0;
	FMC_SDRAM_CommandTypeDef cmd;

	/* Open the clock */
	cmd.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	cmd.AutoRefreshNumber = 0;
	cmd.ModeRegisterDefinition = 0;

	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

	if(FMC_SDRAM_SendCommand(FMC_Bank5_6, &cmd, 10)!=HAL_OK)return HAL_TIMEOUT;

	//SDRAM_Delay(10);

	/* Recharge */
	cmd.CommandMode = FMC_SDRAM_CMD_PALL;
	cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	cmd.AutoRefreshNumber = 0;
	cmd.ModeRegisterDefinition = 0;

	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

	if(FMC_SDRAM_SendCommand(FMC_Bank5_6, &cmd, 10)!=HAL_OK)return HAL_TIMEOUT;

	/* AutoRefresh */
	cmd.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	cmd.AutoRefreshNumber = 2;
	cmd.ModeRegisterDefinition = 0;

	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

	if(FMC_SDRAM_SendCommand(FMC_Bank5_6, &cmd, 10)!=HAL_OK)return HAL_TIMEOUT;

	TEMP = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4          	|
					   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL  |
					   SDRAM_MODEREG_CAS_LATENCY_2          |
					   SDRAM_MODEREG_OPERATING_MODE_STANDARD|
					   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	/* LoadMode */
	cmd.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	cmd.AutoRefreshNumber = 1;
	cmd.ModeRegisterDefinition = TEMP;

	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

	if(FMC_SDRAM_SendCommand(FMC_Bank5_6, &cmd, 10)!=HAL_OK)return HAL_TIMEOUT;
    /* Set the refresh counter */
	/*刷新速率 = (COUNT + 1) x SDRAM 频率时钟
	COUNT =（ SDRAM 刷新周期/行数) - 20*/
	/* 64ms/4096=15.62us (15.62 us x FSDCLK) - 20 = 1486 */
		
  FMC_Bank5_6->SDRTR |= (1486<<1);
	while(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	
	return HAL_OK;
}
