#include "sys.h"					

void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	
		curpin=BITx&pos;
		if(curpin==pos)	
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	
			GPIOx->MODER|=MODE<<(pinpos*2);	
			if((MODE==0X01)||(MODE==0X02))	
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));
				GPIOx->OTYPER&=~(1<<pinpos) ;		
				GPIOx->OTYPER|=OTYPE<<pinpos;		
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	
		}
	}
} 
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx)
{ 
	if(GPIOx->IDR&pinx)return 1;	
	else return 0;					
}

void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
}   

void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status)
{
	
	if(status&0X01)GPIOx->BSRR=pinx;
	else GPIOx->BSRR=pinx;				
}

	 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);
}

void Cache_Enable(void)
{
    SCB_EnableICache();	
    SCB_EnableDCache();	
	SCB->CACR|=1<<2;	
}

