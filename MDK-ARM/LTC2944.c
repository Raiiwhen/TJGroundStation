#include "ltc2944.h"
#include "stdio.h"

/********************************************************************************************
	*	M设置为256，最大可测电量1392.64mAh，每刻度电量为0.02125mAh,Q=1392.64*DATA/FFFFH
	*	电压最大量程为70.8V，V=70.8*DATA/FFFFH
	*	电流最大为1.28A，I=1.28*（DATA-7FFFH）/7FFFH
*********************************************************************************************/

static float voltage = 0;
static float current = 0;
static float quantity = battery;
enum Mode current_Mode = SCAN;

#ifdef IIC1_SOFT

static void delay_us(uint32_t us){
	uint32_t cnt;
	while(us--){
		for(cnt = 180; cnt>0; cnt--);
	}
}

static void IIC1_Start(void)
{
	SDA1_OUT();
	IIC1_SDA=1;	  	  
	IIC1_SCL=1;
	delay_us(2);
 	IIC1_SDA=0; 
	delay_us(2);
	IIC1_SCL=0;
}	  

static void IIC1_Stop(void)
{
	SDA1_OUT();
	IIC1_SCL=0;
	IIC1_SDA=0;
 	delay_us(4);
	IIC1_SCL=1; 
	IIC1_SDA=1;
	delay_us(4);							   	
}

static uint8_t IIC1_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA1_IN();
	IIC1_SDA=1;delay_us(1);	   
	IIC1_SCL=1;delay_us(1);	 
	while(READ1_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC1_Stop();
			return 1;
		}
	}
	IIC1_SCL=0;
	return 0;  
} 

static void IIC1_Ack(void)
{
	IIC1_SCL=0;
	SDA1_OUT();
	IIC1_SDA=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}

static void IIC1_NAck(void)
{
	IIC1_SCL=0;
	SDA1_OUT();
	IIC1_SDA=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}

static void IIC1_Send_Byte(uint8_t txd){                        
	uint8_t t;   
	SDA1_OUT(); 	    
	IIC1_SCL=0;
	for(t=0;t<8;t++)
	{              
		IIC1_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2); 
		IIC1_SCL=1;
		delay_us(2); 
		IIC1_SCL=0;	
		delay_us(2);
	}
} 	    

static uint8_t IIC1_Read_Byte(unsigned char ack){
	unsigned char i,receive=0;
	SDA1_IN();
  for(i=0;i<8;i++ )
	{
		IIC1_SCL=0; 
		delay_us(2);
		IIC1_SCL=1;
		receive<<=1;
		if(READ1_SDA)receive++;   
		delay_us(1); 
	}					 
	if (!ack)
		IIC1_NAck();
	else
		IIC1_Ack();
	
	return receive;
}

uint8_t ltc_Rd_Reg(uint8_t cnt){
	
	return 0xff;
}

uint8_t ltc_ack(uint8_t addr){
	IIC1_Start();
	IIC1_Send_Byte(addr);
	if(IIC1_Wait_Ack()){
		return 0;
	}else{
		IIC1_Stop();
		return 1;
	}
}

#endif

void ltc2944_setMode(enum Mode mode){	
	IIC1_Start();
	IIC1_Send_Byte(0xC8);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x01);	
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x22|(uint8_t)(mode<<6));
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Stop();
	if(mode == MANUAL)
		current_Mode = SLEEP;
	else
		current_Mode = mode;
}

void ltc2944_voltage(void){
	uint16_t voltage_0x;
	IIC1_Start();
	IIC1_Send_Byte(0xC8);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x08);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Start();
	IIC1_Send_Byte(0xC9);
	if(IIC1_Wait_Ack()){
		return;
	}
	voltage_0x = (uint16_t)IIC1_Read_Byte(1) << 8;
	voltage_0x |= IIC1_Read_Byte(0);
	IIC1_Stop();
	voltage = (float)voltage_0x*70.8f/0xFFFF;
}

void ltc2944_current(void){
	uint16_t current_0x;
	IIC1_Start();
	IIC1_Send_Byte(0xC8);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x0E);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Start();
	IIC1_Send_Byte(0xC9);
	if(IIC1_Wait_Ack()){
		return;
	}
	current_0x = (uint16_t)IIC1_Read_Byte(1) << 8;
	current_0x |= IIC1_Read_Byte(0);
	IIC1_Stop();
	current = (float)(current_0x- 0x7FFF)*1.28f/0x7FFF;
}
//获取电量
void ltc2944_quantity(void){
	uint16_t quantity_0x;
	IIC1_Start();
	IIC1_Send_Byte(0xC8);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x02);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Start();
	IIC1_Send_Byte(0xC9);
	if(IIC1_Wait_Ack()){
		return;
	}
	quantity_0x = (uint16_t)IIC1_Read_Byte(1) << 8;
	quantity_0x |= IIC1_Read_Byte(0);
	IIC1_Stop();
	quantity = battery - (float)(0xFFFF-quantity_0x)*0.02125f;
	
}
//设置电量寄存器
void ltc2944_setq(uint16_t quantity){
	IIC1_Start();
	IIC1_Send_Byte(0xC8);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x02);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte((uint8_t)(quantity>>8));
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte((uint8_t)(quantity>>0));
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Stop();
	
}

void ltc2944_fixq(void){
	
	ltc2944_voltage();
	ltc2944_current();
	ltc2944_quantity();
	if(quantity < 0){		//电量修正
		if(current > 0){
			quantity = (voltage - 3.8f)*1000;
			quantity = (quantity > 650)? 650 : ((quantity < 0)? 0 : quantity);
			ltc2944_setq(0xFFFF - (uint16_t)((battery-quantity)/0.02125f));
		}else{
			quantity = 650;
			ltc2944_setq(0xFFFF);
		}
	}else{
		quantity = (voltage - 3.8f)*1000;
		if(quantity > 650)
			quantity = 650;
		ltc2944_setq(0xFFFF - (uint16_t)((battery-quantity)/0.02125f));
	}
}


void ltc2944_init(void){	//配置控制寄存器01H
	IIC1_Start();
	IIC1_Send_Byte(0xC8);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x01);	
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Send_Byte(0x62);
	if(IIC1_Wait_Ack()){
		return;
	}
	IIC1_Stop();
	ltc2944_fixq();
}
