#include "console.h"
#include "string.h"
#include "pwr_mgt.h"
#include "MPU.h"
#include "MS.h"
#include "W25.h"
#include "RTC.h"
#include "ltc2944.h"
#include "IMU.h"
#include "fatfs.h"
#include "NAND.h"
#include "ff.h"
#include "SD.h"
#include "SRAM.h"

#define CONSOLE_UART USART1
#define LOG_PAGE 0

uint8_t page_buff[8629];
uint8_t page_wbuff[8629];
volatile uint8_t mst_mode;

extern SD_HandleTypeDef hsd;
extern UART_HandleTypeDef huart1;

/* basic support function*/
static void delay_ms(uint16_t ms){
	uint32_t delay_ms_macro; 
	while(ms--){
		delay_ms_macro = 180000; 
		while(delay_ms_macro--);
	}
}

#define RX_BUFF_SIZE 128
char RX_BUFF[RX_BUFF_SIZE];
#define TX_BUFF_SIZE 128
char TX_BUFF[TX_BUFF_SIZE];
volatile uint8_t RX_CNT;
volatile char CONSOLE_FLAG;

#pragma import(__use_no_semihosting)             
FILE __stdout;       
struct __FILE 							{	int handle; 			}; 
void _sys_exit(int x)			 	{ x = x; 						} 
void console_setflag(void)	{	CONSOLE_FLAG = 1;	}
int fputc(int ch, FILE *f){
	while((CONSOLE_UART->SR&0X40)==0);
	CONSOLE_UART->DR = (uint8_t) ch;      
	return ch;
}
/* end support*/

void console_log(void){
	uint8_t pSrc[8] = {0};
	uint32_t cnt = 0, _cnt =0;
	uint8_t tmp = 0;
	char str[] = "2020-02-13, 17:45:52  Thurs. ";
	uint8_t pre_cmd[]="IMU -tmp\r\n";

	RTC_Get_Date(&pSrc[0],&pSrc[1],&pSrc[2],&pSrc[3]);
	RTC_Get_Time(&pSrc[4], &pSrc[5], &pSrc[6], &pSrc[7]);
	RTC_raw2str(pSrc, str);
	
	printf("\r\n\r\n");
	printf("<SYS V1.0>\r\n");
	printf("@TJMewo masterraii@icloud.com\r\n");
	printf("%s\r\n",str);
	
	/*add .log document*/
	W25_RdPage(LOG_PAGE, page_buff, 2048);
	if(memcmp(page_buff,"meowlog",7)){
		printf("log file not found.\r\n");
	}else{
		cnt = page_buff[7];
		memcpy(page_wbuff,page_buff,2048);
		page_wbuff[7] = ++cnt;
		memcpy(page_wbuff+cnt*8,pSrc,8);
		memcpy(page_wbuff+22*8,pSrc,8);
	#if 0
		W25_ErBlock(0);
		W25_WrPage(0, page_wbuff, 2048);
	#else
		W25_WrStr(0, page_wbuff, 2048);
	#endif
	}
////	
//	/*add .log document*/
//	W25_RdPage(LOG_PAGE, page_buff, 2048);
//	if(memcmp(page_buff,"meowlog",7) || page_buff[8]==0xff){
//		printf("new log file.\r\n");
//		memset(page_wbuff,0xff,2048);
//		memcpy(page_wbuff,"meowlog\0",8);
//		page_wbuff[8]=0x00;	//first log create
//		page_wbuff[9]=0x01;
//		W25_ErBlock(LOG_PAGE/64);
//		W25_WrPage(LOG_PAGE,page_wbuff,2048);
//	}
//	_cnt = 8;
//	while(page_buff[_cnt] != 0xff){//get log counter
//		tmp = ~page_buff[_cnt];
//		while(tmp){
//			cnt += tmp&0x01;
//			tmp >>= 1;
//		}
//		_cnt++;
//	}
//	if(page_buff[_cnt-1] == 0x00){
//		page_buff[_cnt]=0x7f; //next byte start count
//	}else{
//		page_buff[_cnt-1] >>= 1; //this byte continue count
//	}
//	W25_WrPage(LOG_PAGE,page_buff,2048);
//	printf("log cnt %d\r\n",cnt+1);	
	
	/*exe pre cmd at log*/
	CONSOLE_FLAG = 1;
	RX_CNT = strlen((char*)pre_cmd);
	strcpy(RX_BUFF,(char*)pre_cmd);
	delay_ms(1);
	
	/**debug content*/
//	SDRAM_RW8(22) = 9;
//	printf("SDRAM %d\r\n",SDRAM_RW8(22));
	
}

void console_exe(void){
	if(!CONSOLE_FLAG){
		return;
	}else{
		if(RX_BUFF[0] < 64){
			/*TJMonster interface*/		
			switch(RX_BUFF[0]){
				case 8:	//sync flag
					mst_sync((uint8_t*)TX_BUFF);
					HAL_UART_Transmit_DMA(&huart1,(uint8_t*)TX_BUFF,128);
					break;
				case 9:	//get multi channel data
					mst_mode = 0x09;
					break;
				case 10:	//start real time transmit
					mst_mode = (mst_mode==0x0a ? 0x00: 0x0a);
					break;
				case 11:	//stop real time transmit
					mst_mode = 0x0b;
					break;
				case 12:	//reset cmd
					reset_val();
					break;

				default: 
					break;
			}
		}else{
			/*console interface*/
			printf("console> ");
			if(!strcmp(RX_BUFF,"hi\r\n"))						{printf("yo~");}
			else if(!memcmp(RX_BUFF,"IIC1",4))			{cmd_iic1();}
			else if(!memcmp(RX_BUFF,"flash",5))			{cmd_flash();}
			else if(!memcmp(RX_BUFF,"RTC",3))				{cmd_rtc();}
			else if(!memcmp(RX_BUFF,"MPU",3))				{cmd_mpu();}
			else if(!memcmp(RX_BUFF,"W25",3))				{cmd_w25();}
			else if(!memcmp(RX_BUFF,"MS",2))				{cmd_ms();}
			else if(!memcmp(RX_BUFF,"IMU",3))				{cmd_imu();}
			else if(!memcmp(RX_BUFF,"SYS",3))  			{cmd_SYSInfo();}
			else if(!memcmp(RX_BUFF,"/",1))  				{cmd_storage();}
			else if(!strcmp(RX_BUFF,"exit\r\n"))		{cmd_exit();}
			else {
				RX_BUFF[RX_CNT-1] = '\0' ;
				printf("unknown cmd '%s'\r\n",RX_BUFF);
				cmd_help();
			}
			printf("\r\n");
		}
		/*reset buff*/
		memset(RX_BUFF,0,128);
		RX_CNT = 0;
		CONSOLE_FLAG = 0;
		HAL_UART_Receive_DMA(&huart1,(uint8_t*)RX_BUFF,128);
		LED_Y = !LED_Y;
		return;
	}
}

void cmd_help(void){
	printf("console> help:\r\n");
	printf("\t|[hi]\r\n");
	printf("\t|[IIC1]\r\n");
	printf("\t|[flash]\r\n");
	printf("\t|[RTC]\r\n");
	printf("\t|[MPU]\r\n");
	printf("\t|[W25]\r\n");
	printf("\t|[MS]\r\n");
	printf("\t|[IMU]\r\n");
	printf("\t|[SYS]\r\n");
	printf("\t|[/]\r\n");
	printf("\t|[exit]\r\n");
}

void cmd_SYSInfo(void){
	uint8_t temp[8] = "melog";
	uint16_t _cnt=0;
	char str[] = " 2020-02-11 23:41:36, Tues.    ";

	printf("SYS: ");
	if(!strcmp(RX_BUFF,"SYS -freq\r\n")){
		printf("SysClockFreq = %dMhz",HAL_RCC_GetSysClockFreq()/1000000);
	}else if(!strcmp(RX_BUFF,"SYS -log\r\n")){
		W25_RdPage(LOG_PAGE, page_buff, 2048);
		if(memcmp(page_buff,"meowlog",7)){printf("log file not found.");return;}
		printf(" %d log records:\r\n",page_buff[7]);
		for(_cnt=1; _cnt<page_buff[7]+1; _cnt++){
			memcpy(temp,page_buff+_cnt*8,8);
			RTC_raw2str(temp, str);
			printf(" [%4d] %s\r\n",_cnt,str);
		}
	}else if(!strcmp(RX_BUFF,"SYS -Pwr\r\n")){
		printf("%4.2fW ",LTC_getI() * LTC_getV());
		printf("%4.2fV ,%d%c  ",LTC_getV(), (int)LTC_getQ(), '%');
		printf("BKP %4.2fV",get_BAT());
	}else{
		printf("Unknown function char.");
		printf("\tTry:\r\n");
		printf("\t|SYS -freq\r\n");
		printf("\t|SYS -Pwr\r\n");
		printf("\t|SYS -log\r\n");
	}
}

void cmd_iic1(void){
	uint16_t cnt, _cnt = 0;
	printf("IIC1: ");
	if(!strcmp(RX_BUFF,"IIC1 -ls\r\n")){
		for(cnt=0; cnt<256; cnt++){
			if(ltc_ack(cnt)){
				_cnt++;
			}
		}
		if(_cnt){
			printf("%d devices echo.",_cnt);
			for(cnt=0; cnt<256; cnt++){
				if(ltc_ack(cnt)) printf("\r\n\t\tdevice[0x%02X]",cnt);
			}
		}else{
			printf("No device echo on IIC1 bus.");			
		}		
	}else{
		printf("unknown character.\r\n");
		printf("\tTry:\r\n");
		printf("\t|IIC1 -ls\r\n");
	}
}

void cmd_flash(void){
	uint32_t pulse=0;
	uint8_t cnt=0,_cnt=0;
	printf("flash:");
	if(!strcmp(RX_BUFF,"flash -shutdown\r\n"))	{
		printf("shutdown");
		TIM5->CCER = 0x0000;
	}else if(!strcmp(RX_BUFF,"flash -run\r\n"))	{
		printf("running");
		TIM5->CCER = 0x0010;
	}else if(!strcmp(RX_BUFF,"flash -?\r\n"))	{
		printf("pulse = %6.2f%c(~5000)",(float)TIM5->CCR2/50,'%');
	}else if(!memcmp(RX_BUFF,"flash -",7)){
		printf("pulse = ");
		for(cnt=strlen(RX_BUFF)-9;cnt!=0;cnt--){
			pulse *= 10;
			pulse += (RX_BUFF[ 7 + _cnt++ ] - 48);
		}
		if(pulse<5001){
			printf("%6.2f%c(~5000)",(float)pulse/50,'%');
			TIM5->CCR2 = pulse;
		}else{
			printf("input a int <5000");
		}
	}else {
		printf("unknown function char\r\n");	
		printf("\tTry:\r\n");
		printf("\t|flash -shutdown\r\n");
		printf("\t|flash -run\r\n");
		printf("\t|flash -?\r\n");
		printf("\t|flash -(<5000)\r\n");
	}
}

void cmd_mpu(void){
	uint8_t data=0, cnt=0;
	short raw[7];
	printf("MPU: ");
	if(!strcmp(RX_BUFF,"MPU -ID\r\n"))	{
		MPU_ID(&data);
		switch(data){
			case 0x71:printf("MPU9250 Deceted.");break;
			case 0xac:printf("ICM20601 Deceted.");break;
			default: 	printf("No Matched Device ID[0x%2X]",data);
		}
	}else if(!strcmp(RX_BUFF,"MPU -LS\r\n")){
		for(cnt = 0; cnt<128; cnt++){
			data = MPU_Rd_Reg(cnt);
			printf("\r\n    [%3d]0x%02X",cnt,data);
		}
	}else if(!strcmp(RX_BUFF,"MPU -raw\r\n")){
		MPU_Rd_Raw(raw);
		printf("raw %6d %6d %6d |%6d |%6d %6d %6d ",raw[0], raw[1], raw[2],raw[3], raw[4], raw[5],raw[6]);
	}else if(!strcmp(RX_BUFF,"MPU -T\r\n")){
		printf("%4.1f C",get_MPU_T());
	}else{
		printf("unknown function char.\r\n");
		printf("\tTry:\r\n");
		printf("\t|MPU -ID\r\n");
		printf("\t|MPU -LS\r\n");
		printf("\t|MPU -raw\r\n");
	}
}

void cmd_imu(void){
	uint8_t data=0;
	static int cnt;
	printf("IMU: ");
	if(!strcmp(RX_BUFF,"IMU -ID\r\n"))	{
		MPU_ID(&data);
		switch(data){
			case 0x71:printf("MPU9250 Deceted.");break;
			case 0xac:printf("ICM20601 Deceted.");break;
			default: 	printf("No Matched Device ID[0x%2X]",data);
		}
	}else if(!strcmp(RX_BUFF,"IMU -tmp\r\n")){
		cnt++;
		printf("cnt(%d) = %d;",cnt,(short)get_IMU_tmp());
	}else{
		printf("unknown function char.\r\n");
		printf("\tTry:\r\n");
		printf("\t|IMU -ID\r\n");
		printf("\t|IMU -tmp\r\n");
	}
}

void cmd_rtc(void){
	uint8_t cnt = 0;
	uint8_t tmp0, tmp1, tmp2, tmp3;
	uint32_t data = 0;
	printf("RTC: ");
	if(!strcmp(RX_BUFF,"RTC -BKP\r\n"))	{
		printf("Back Up Register");
		for(cnt=0;cnt<20;cnt++){
			RTC_Read_BKR(cnt,&data);
			printf("\r\n             [BKP%02d]%08X",cnt,data);
		}
	}else if(!strcmp(RX_BUFF,"RTC\r\n")){
		RTC_Get_Date(&tmp0, &tmp1, &tmp2, &tmp3);
		printf("20%02d-%02d-%02d",tmp0, tmp1, tmp2);
		switch(tmp3){
			case 1:printf(", Mon.");break;
			case 2:printf(", Tues.");break;
			case 3:printf(", Wed.");break;
			case 4:printf(", Thurs.");break;
			case 5:printf(", Fri.");break;
			case 6:printf(", Satur.");break;
			case 7:printf(", Sun.");break;
			default: printf("[ERR]");
		}
		RTC_Get_Time(&tmp0, &tmp1, &tmp2, &tmp3);
		printf(" %02d:%02d:%02d",tmp0, tmp1, tmp2);
		if(tmp3)
			printf(" AM");
		else
			printf(" PM");
	}else if(!memcmp(RX_BUFF,"RTC -date",9)){
		/*RTC -date 2020/01/24_5*/
		tmp0 = (RX_BUFF[12]-48)*10 + (RX_BUFF[13]-48);
		tmp1 = (RX_BUFF[15]-48)*10 + (RX_BUFF[16]-48);
		tmp2 = (RX_BUFF[18]-48)*10 + (RX_BUFF[19]-48);
		tmp3 = (RX_BUFF[21]-48);
		RTC_Set_Date(tmp0, tmp1, tmp2, tmp3);
		printf("[set]20%02d-%02d-%02d, weekdeay%02d",tmp0, tmp1, tmp2, tmp3);
	}else if(!memcmp(RX_BUFF,"RTC -time",9)){
		/*RTC -time 06:53:22,pm*/
		tmp0 = (RX_BUFF[10]-48)*10 + (RX_BUFF[11]-48);
		tmp1 = (RX_BUFF[13]-48)*10 + (RX_BUFF[14]-48);
		tmp2 = (RX_BUFF[16]-48)*10 + (RX_BUFF[17]-48);
		tmp3 = (RX_BUFF[19]=='a')?1:0;
		RTC_Set_Time(tmp0, tmp1, tmp2, tmp3);
		printf("[set]%02d:%02d:%02d",tmp0, tmp1, tmp2);
	}else {
		printf("unknown function char\r\n");
		printf("\tTry:\r\n");
		printf("\t|RTC -BKP\r\n");
		printf("\t|RTC\r\n");
		printf("\t|RTC -date 2020/01/24_5\r\n");
		printf("\t|RTC -time 06:53:22,pm\r\n");
	}
}


void cmd_w25(void){
	uint32_t data=0;
	printf("W25: ");
	if(!strcmp(RX_BUFF,"W25 -ID\r\n"))	{
				W25_ID(&data,0);
				switch(data){
					case 0x00EFAB21:printf("W25M02GV Deceted.");break;
					default: 	printf("No Matched Device ID[0x%08X]",data);
				}
	}else if(!strcmp(RX_BUFF,"W25 -STA\r\n")){
				printf("STA[0x%06X]",W25_STA());
	}else {
				printf("unknown function char\r\n");
				printf("\tTry:\r\n");
				printf("\t|W25 -ID\r\n");
				printf("\t|W25 -STA");
	}
}
void cmd_storage(void){
	uint32_t ID[2] = {0};
	uint32_t res;
	uint32_t cnt = 0, _cnt = 0;
	
	/*get page from cmd*/
	for(cnt=6,_cnt=0;RX_BUFF[cnt] != 0x0d; cnt++){
		_cnt *= 10;
		_cnt += (RX_BUFF[cnt] - 48);
	}
	
	if(!strcmp(RX_BUFF,"/NAND FLASH -ID\r\n")){
					printf("NAND FLASH ID: ");
					NAND_ID(ID);
					if			(ID[0] == 0x7284d5ec)	{printf("SAMSUNG K9GAG08U0E, 2GB MLC.");}
					else if (ID[0] == 0x72c5d7ec)	{printf("SAMSUNG K9LBG08U0E, 4GB MLC.");}
					else													{printf("No matched [0x%08X-%04X]",ID[0],ID[1]);}
	}else if(!strcmp(RX_BUFF,"/SD -ID\r\n")){
					res = (HAL_SD_Init(&hsd) | HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B));
					if(res)												{printf("SD Init failed.[0x%04X]",res);return;}
					printf("%s",SD_info());
	}else if(!memcmp(RX_BUFF,"/-r ",3)){
	/*	/-r 0-324,12			*/
					/*choose driver(0: NAND 1:W25 2:SD 3:SRAM)*/
					switch(RX_BUFF[4]){
						case '0':
								printf("NAND Flash ");
								if(_cnt > 265728){printf("\t[para ERR]: page < 265728");break;}
								printf("Block.%d Page.%d ", _cnt/128, _cnt%128);
								res = NAND_RdPage(_cnt, page_buff,0);
								printf("[read 0x%02X] ",res);
									break;
						case '1':
								printf("W25:");
								if(_cnt > 65536){printf("\t[para ERR]: page < 65536");break;}
								printf("Block.%d Page.%d ", _cnt/64, _cnt%64);
								res = W25_RdPage(_cnt, page_buff,2112);
								printf("[read %.2f us] ",res/180.0);
								break;
						case '2':
								printf("SD:");
								if(_cnt > hsd.SdCard.BlockNbr){printf("\t[para ERR]: page < %d",hsd.SdCard.BlockNbr);return;;}
								printf("Block.%d",_cnt);
								res = SD_RdBlock(_cnt, page_buff, 512);		printf(" [read 0x%02X]",res);
								break;
						default: 
									printf("Invalid driver.e.g.[/-r 0-324,12]");
					}
	}else if(!memcmp(RX_BUFF,"/-e ",3)){
	/*/-e 0-324*/
				switch(RX_BUFF[4]){
					case '0':	printf("NAND Flash:");
								if(_cnt > 265728){printf("\t[para ERR]: page < 265728");break;}
								printf("Block.%d         ", _cnt/128);
								res = NAND_ErBlock(_cnt/128);
								printf("[erase 0x%02X] ",res);
								printf("[RST]");NAND_RST();
								break;
				case '1': 
								printf("W25:");
								if(_cnt > 65536){printf("\t[para ERR]: page < 65536");break;}
								printf("Block.%d Page.~ ", _cnt/64);
								res = W25_ErBlock(_cnt/64);
								printf("[erase %.2f us] ",res/180.0);
								break;
				case '2': 
								printf("SD: No erase operation.");
								break;
				default:  printf("Invalid driver.e.g.[/-r 0-324,12]");
				}
	}else if(!memcmp(RX_BUFF,"/-w ",3)){
	/*/-w 0-324,12*/
					memset(page_wbuff,0xff,1024);
					memcpy(page_wbuff,"meowlog\0",8);
					switch(RX_BUFF[4]){
						case '0':
									printf("NAND Flash:");
									if(_cnt > 265728){printf("\t[para ERR]: page < 265728");break;}
									printf("Block.%d Page.%d ", _cnt/128, _cnt%128);
									res = NAND_WrPage(_cnt, page_wbuff,0);
									printf("[write 0x%02X] ",res);
									printf("[RST]");NAND_RST();
									break;
						case '1':
									printf("W25:");
									if(_cnt > 65535){printf("\t[para ERR]: page < 65536");break;}
									printf("Block.%d Page.%d ", _cnt/64, _cnt%64);
									res = W25_WrPage(_cnt, page_wbuff,2112);
									printf("[write %.2f us] ",res/180.0);
									break;
						case '2':
									printf("SD:");
									if(_cnt > hsd.SdCard.BlockNbr){printf("\t[para ERR]: page < %d",hsd.SdCard.BlockNbr);return;}
									printf("Block.%d", _cnt);
									res = SD_WrBlock(_cnt, page_wbuff, 512);	printf(" [write 0x%02X]",res);
									break;
						default: 
									printf("Invalid driver.e.g.[/-r 0-324,12]");
					}
	}else if(!strcmp(RX_BUFF,"/-buff\r\n")){
					printf("BUFF:\r\n[tmp]");
					for(cnt=0; cnt<16; cnt++){//256*32
						printf("\r\n\t%4d:",cnt*32);
						for(_cnt=0; _cnt<32; _cnt++)printf(" %02X",page_buff[cnt*32+_cnt]);
					}
//					printf("\r\n[spare]");
//					for(cnt=64; cnt<64+4; cnt++){//27*16
//						printf("\r\n\t%4d:",cnt*16);
//						for(_cnt=0; _cnt<16; _cnt++)printf(" %02X",page_buff[cnt*16+_cnt]);
//					}
	}else{
					printf("unknown function char\r\n");
					printf("\tTry:\r\n");
					printf("\t|/NAND FLASH -ID\r\n");
					printf("\t|/SD -ID\r\n");
					printf("\t|/-r 0-324,12\r\n");
					printf("\t|/-w 0-324,12\r\n");
					printf("\t|/-e 0-324\r\n");
	}
}

void cmd_ms(void){
	uint16_t* data;
	static uint8_t CMD_MS_CNT = 3;
	uint8_t cnt;
	printf("MS: ");
	if(!strcmp(RX_BUFF,"MS -RST\r\n")){
		MS_Init();
		printf("MS Reset.");
		
	}else if(!strcmp(RX_BUFF,"MS -PROM\r\n")){
		data = MS_UpdatePROM();
		printf("PROM\r\n");
		for(cnt=0;cnt<8;cnt++)printf("\t\tC%d[0x%04X][%5d]\r\n",cnt,data[cnt],data[cnt]);
	}else if(!strcmp(RX_BUFF,"MS -T\r\n")){
		if(CMD_MS_CNT&0x01){
			MS_TRIG_Temperature(4);
			printf("Temperature triggered.");
		}else{
			printf("Temperature %5.2f C",MS_Read_Temperature());
		}
		CMD_MS_CNT ^= 0x01;
	}else if(!strcmp(RX_BUFF,"MS -P\r\n")){
		if(CMD_MS_CNT&0x02){
			MS_TRIG_Temperature(3);
			delay_ms(10);
			MS_Read_Temperature();
			MS_TRIG_Pressure(4);
			printf("Pressure triggered.");
		}else{
			printf("Pressure %5.2fkPa",MS_Read_Pressure());
		}
		CMD_MS_CNT ^= 0x02;
	}else {
		printf("unknown function char\r\n");
		printf("\tTry:\r\n");
		printf("\t|MS -RST\r\n");
		printf("\t|MS -PROM\r\n");
		printf("\t|MS -T\r\n");
		printf("\t|MS -P");
	}
}

void cmd_exit(void){
	printf("Hit [RESET] or [S3] key to awake.\r\n");
	printf("SYS STDBY.");
	SYS_STDBY();
}

void mst_sync(uint8_t* echo){
	uint8_t buffer[16] = {0};
	uint8_t pSrc[8] = {0};
	uint8_t mpu_id;
	uint32_t nand_id;
	uint32_t w25_id;
	
	buffer[0] = 0xb1;
	buffer[1] = 0xb1;
	
	RTC_Get_Date(&pSrc[0],&pSrc[1],&pSrc[2],&pSrc[3]);
	RTC_Get_Time(&pSrc[4], &pSrc[5], &pSrc[6], &pSrc[7]);
	memcpy(buffer+2,pSrc,3);
	memcpy(buffer+5,pSrc+4,2);
	
	MPU_ID(&mpu_id);
	switch(mpu_id){
		case 0x71:buffer[7] = 0x01;break;		//MPU9250
		case 0xac:buffer[7] = 0x02;break;		//ICM20601
		default: 	buffer[7] = mpu_id;				//error
	}
	
	NAND_ID(&nand_id);
	switch(nand_id){
		case 0x7284d5ec:	buffer[8] = 0x01; break;//SAMSUNG K9GAG08U0E, 2GB MLC
		case 0x72c5d7ec:  buffer[8] = 0x02; break;//SAMSUNG K9LBG08U0E, 4GB MLC
		
		default: buffer[8] = (uint8_t)nand_id;//unknown
	}
	
	buffer[9] = 0xff;
	buffer[10] = 0xff;
	buffer[11] = 0xff;
	
	W25_ID(&w25_id,0);
	switch(w25_id){
		case 0x00EFAB21:buffer[12] = 0x01;break;//W25M02GV
		default: buffer[12] = (uint8_t)w25_id;
	}
	
	buffer[13] = 0xff;
	buffer[14] = 0xff;
	buffer[15] = 0xd1;

	memset(echo,16,0);
	memcpy(echo,buffer,16);
}

void mst_upload(uint8_t* pSrc, uint8_t length){
	memset(TX_BUFF, 0, 128);
	memcpy(TX_BUFF, pSrc, length>128?128:length);
	HAL_UART_Transmit_DMA(&huart1,(uint8_t*)TX_BUFF,128);
}

