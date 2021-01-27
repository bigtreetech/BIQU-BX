#include "Arduino.h"
#include "sdram.h"
//#include "PLdelay.h"
//#include "../../../../../core/serial.h"
//#include "../../inc/MarlinConfig.h"
//volatile u16 *testsram =(volatile u16 *)0XC0000000;

u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval)
{
	u32 retry=0;
	u32 tempreg=0; 
	tempreg|=cmd<<0;			
	tempreg|=1<<(4-bankx);		
	tempreg|=refresh<<5;		
	tempreg|=regval<<9;			
	FMC_Bank5_6_R->SDCMR=tempreg;	
	while((FMC_Bank5_6_R->SDSR&(1<<5)))
	{
		retry++;
		if(retry>0X1FFFFF)return 1; 
	}
	return 0;	
} 

void SDRAM_RGBLCD_Set(void)
{
	u32 mregval=0;
	FMC_Bank5_6_R->SDCR[0]&=~(3<<7);	
	FMC_Bank5_6_R->SDCR[0]|=2<<7;		
	mregval|=3<<0;					
	mregval|=0<<3;					
	mregval|=2<<4;					
	mregval|=0<<7;					
	mregval|=1<<9;					
	SDRAM_Send_Cmd(0,4,0,mregval);	
}


void SDRAM_Init(void)
{ 
	u32 sdctrlreg=0,sdtimereg=0;
	u16 mregval=0; 

	RCC->AHB3ENR|=1<<12;
	RCC->AHB4ENR|=0X1F<<2;
	GPIO_Set(GPIOC,PIN0|PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);			//PC0/2/3			
	GPIO_Set(GPIOD,3<<0|7<<8|3<<14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PD0/1/8/9/10/14/15		
	GPIO_Set(GPIOE,3<<0|0X1FF<<7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);			//PE0/1/7~15				
	GPIO_Set(GPIOF,0X3F<<0|0X1F<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PG0~5/11~15					
	GPIO_Set(GPIOG,7<<0|3<<4|PIN8|PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);	//PF0~2/4/5/8/15				
 
 	GPIO_AF_Set(GPIOC,0,12);	//PC0,AF12
 	GPIO_AF_Set(GPIOC,2,12);	//PC2,AF12
 	GPIO_AF_Set(GPIOC,3,12);	//PC3,AF12
	
 	GPIO_AF_Set(GPIOD,0,12);	//PD0,AF12 
 	GPIO_AF_Set(GPIOD,1,12);	//PD1,AF12 
 	GPIO_AF_Set(GPIOD,8,12);	//PD8,AF12
 	GPIO_AF_Set(GPIOD,9,12);	//PD9,AF12
 	GPIO_AF_Set(GPIOD,10,12);	//PD10,AF12  
 	GPIO_AF_Set(GPIOD,14,12);	//PD14,AF12
 	GPIO_AF_Set(GPIOD,15,12);	//PD15,AF12
	
 	GPIO_AF_Set(GPIOE,0,12);	//PE0,AF12 
 	GPIO_AF_Set(GPIOE,1,12);	//PE1,AF12 
 	GPIO_AF_Set(GPIOE,7,12);	//PE7,AF12
 	GPIO_AF_Set(GPIOE,8,12);	//PE8,AF12
 	GPIO_AF_Set(GPIOE,9,12);	//PE9,AF12
 	GPIO_AF_Set(GPIOE,10,12);	//PE10,AF12
 	GPIO_AF_Set(GPIOE,11,12);	//PE11,AF12
 	GPIO_AF_Set(GPIOE,12,12);	//PE12,AF12
 	GPIO_AF_Set(GPIOE,13,12);	//PE13,AF12
 	GPIO_AF_Set(GPIOE,14,12);	//PE14,AF12
 	GPIO_AF_Set(GPIOE,15,12);	//PE15,AF12

 	GPIO_AF_Set(GPIOF,0,12);	//PF0,AF12 
 	GPIO_AF_Set(GPIOF,1,12);	//PF1,AF12 
 	GPIO_AF_Set(GPIOF,2,12);	//PF2,AF12
 	GPIO_AF_Set(GPIOF,3,12);	//PF3,AF12
 	GPIO_AF_Set(GPIOF,4,12);	//PF4,AF12
 	GPIO_AF_Set(GPIOF,5,12);	//PF5,AF12
 	GPIO_AF_Set(GPIOF,11,12);	//PF11,AF12
 	GPIO_AF_Set(GPIOF,12,12);	//PF12,AF12
 	GPIO_AF_Set(GPIOF,13,12);	//PF13,AF12
 	GPIO_AF_Set(GPIOF,14,12);	//PF14,AF12
 	GPIO_AF_Set(GPIOF,15,12);	//PF15,AF12
	
 	GPIO_AF_Set(GPIOG,0,12);	//PG0,AF12 
 	GPIO_AF_Set(GPIOG,1,12);	//PG1,AF12 
 	GPIO_AF_Set(GPIOG,2,12);	//PG2,AF12
 	GPIO_AF_Set(GPIOG,4,12);	//PG4,AF12
 	GPIO_AF_Set(GPIOG,5,12);	//PG5,AF12  
 	GPIO_AF_Set(GPIOG,8,12);	//PG8,AF12
 	GPIO_AF_Set(GPIOG,15,12);	//PG15,AF12	
    
 	sdctrlreg|=1<<0;				
	sdctrlreg|=2<<2;				
	sdctrlreg|=1<<4;				
	sdctrlreg|=1<<6;			
	sdctrlreg|=3<<7;			
	sdctrlreg|=0<<9;			
	sdctrlreg|=2<<10;				
	sdctrlreg|=1<<12;			
	sdctrlreg|=0<<13;				
 	FMC_Bank5_6_R->SDCR[0]=sdctrlreg;	

	sdtimereg|=1<<0;				
	sdtimereg|=7<<4;				
	sdtimereg|=4<<8;				
	sdtimereg|=6<<12;				
	sdtimereg|=1<<16;				
	sdtimereg|=1<<20;				
	sdtimereg|=1<<24;			
 	FMC_Bank5_6_R->SDTR[0]=sdtimereg;
	FMC_Bank1_R->BTCR[0]|=(u32)1<<31;	

	SDRAM_Send_Cmd(0,1,0,0);
	delay(1);		
//    delay_us(500);				   
	SDRAM_Send_Cmd(0,2,0,0);		
	SDRAM_Send_Cmd(0,3,8,0);		
	mregval|=3<<0;					
	mregval|=0<<3;					
	mregval|=3<<4;				
	mregval|=0<<7;				
	mregval|=1<<9;					
	SDRAM_Send_Cmd(0,4,0,mregval);	
	

	FMC_Bank5_6_R->SDRTR=839<<1;	

} 

void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank5_SDRAM_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(vu8*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
	}
}




