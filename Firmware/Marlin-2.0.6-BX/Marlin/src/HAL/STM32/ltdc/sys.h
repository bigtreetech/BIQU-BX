#ifndef _SYS_H_
#define _SYS_H	_ 
#include "stm32h7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

////////////////////////////////////////////////////////////////////////////////// 

#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6 
#define GPIO_H 				7 
#define GPIO_I 				8 
#define GPIO_J 				9 
#define GPIO_K 				10 

#define FTIR   				1  		
#define RTIR   				2  	


#define GPIO_MODE_IN    	0		
#define GPIO_MODE_OUT		1		
#define GPIO_MODE_AF		2		
#define GPIO_MODE_AIN		3		

#define GPIO_SPEED_LOW		0		
#define GPIO_SPEED_MID		1		
#define GPIO_SPEED_FAST		2		
#define GPIO_SPEED_HIGH		3		

#define GPIO_PUPD_NONE		0	
#define GPIO_PUPD_PU		1	
#define GPIO_PUPD_PD		2	
#define GPIO_PUPD_RES		3		

#define GPIO_OTYPE_PP		0	
#define GPIO_OTYPE_OD		1		


#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 



void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);	
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status);
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx);
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq);



// void delay_init(u16 SYSCLK);
// void delay_ms(u16 nms);
// void delay_us(u32 nus);

#ifdef __cplusplus
}
#endif

#endif




