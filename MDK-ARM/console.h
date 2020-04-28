#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "main.h"
#include "stdio.h"

void put_len(const uint8_t* buff, uint16_t length);
void console_setflag(void);
void console_exe(void);

uint32_t cmd_RDpara(uint16_t obj);
uint32_t cmd_WRpara(uint16_t obj, uint16_t para);

void console_log(void);
/*cmd list*/
void cmd_battery(void);
void cmd_help(void);
void cmd_iic1(void);
void cmd_flash(void);
void cmd_rtc(void);
void cmd_mpu(void);
void cmd_w25(void);
void cmd_ms(void);
void cmd_imu(void);
void cmd_SYSInfo(void);
void cmd_storage(void);
void cmd_exit(void);
/*master list*/
void mst_sync(uint8_t* echo);
void mst_upload(uint8_t* pSrc, uint8_t length);

#endif
