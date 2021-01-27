#include "../../inc/MarlinConfig.h"
#include "spi.h"

void SPI3_Pins_Init(void)
{
  pinmap_pinout(digitalPin[MOSI_PIN], PinMap_SPI_MOSI);
  pinmap_pinout(digitalPin[MISO_PIN], PinMap_SPI_MISO);
  pinmap_pinout(digitalPin[SCK_PIN], PinMap_SPI_SCLK);
}

void SPI3_Init(void)
{	 
  uint32_t tempreg=0;

  SPI3_Pins_Init();

  RCC->APB1LENR|=1<<15;

  RCC->D2CCIP1R&=~(7<<12);
  RCC->D2CCIP1R|=0<<12;
  RCC->APB1LRSTR|=1<<15;
  RCC->APB1LRSTR&=~(1<<15);

  SPI3->CR1|=1<<12;
  SPI3->CFG1=7<<28;
  SPI3->CFG1|=7<<0;
  tempreg=(uint32_t)1<<31;
  tempreg|=0<<29;
  tempreg|=1<<26;
  tempreg|=1<<25;
  tempreg|=1<<24;
  tempreg|=0<<23;
  tempreg|=1<<22;
  tempreg|=0<<19;
  tempreg|=0<<17;
  SPI3->CFG2=tempreg;
  SPI3->I2SCFGR&=~(1<<0);
  SPI3->CR1|=1<<0;
}   

void SPI3_SetSpeed(uint8_t SpeedSet)
{
  SpeedSet&=0X07;
  SPI3->CR1&=~(1<<0);
  SPI3->CFG1&=~(7<<28);
  SPI3->CFG1|=(uint32_t)SpeedSet<<28;
  SPI3->CR1|=1<<0;
} 

uint8_t SPI3_ReadWriteByte(uint8_t TxData)
{			 	   
  uint8_t RxData=0;	
  SPI3->CR1|=1<<0;
  SPI3->CR1|=1<<9;

  while((SPI3->SR & (1<<1))==0);
  *(volatile uint8_t *)&SPI3->TXDR=TxData;
  while((SPI3->SR & (1<<0))==0);
  RxData=*(volatile uint8_t *)&SPI3->RXDR;

  SPI3->IFCR|=3<<3;
  SPI3->CR1&=~(1<<0);
  return RxData;
}
