/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "stm32h7xx.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "w25qxx.h"
#include "../API/Settings.h"

const uint8_t cap_ID[14]      = {0x10,      0x11,       0x12,       0x01,       0x13,       0x14,     0x15,     0x16,     0x17,     0x18,      0x19,      0x43,     0x4B,     0x00,};
const uint32_t flash_size[14] = {KBYTE(64), KBYTE(128), KBYTE(256), KBYTE(512), MBYTE(1), MBYTE(2), MBYTE(4), MBYTE(8), MBYTE(16), MBYTE(32), MBYTE(8), MBYTE(8), KBYTE(256), KBYTE(512)};

uint8_t W25QXX_QPI_MODE = 0;		//QSPI mode:[0 SPI mode];[1 QSPI mode] .
uint16_t W25QXX_TYPE = 0;

uint8_t QSPI_Wait_Flag(uint32_t flag, uint8_t sta, uint32_t wtime)
{
	uint8_t flagsta = 0;
	while(wtime--)
	{
		flagsta = (QUADSPI->SR & flag) ? 1 : 0;
		if(flagsta == sta)break;
	}
	if(wtime) return 0;
	else return 1;
}

uint8_t QSPI_Init(int ClockPrescaler, int FifoThreshold, int FlashSize)
{
  uint32_t tempreg=0;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_QSPI_CLK_ENABLE();    		//PORTB clock enable
  __HAL_RCC_GPIOF_CLK_ENABLE();    		//PORTF clock enable
  __HAL_RCC_GPIOB_CLK_ENABLE();   		//QSPI clock enable
  /**QUADSPI GPIO Configuration
  PF6     ------> QUADSPI_BK1_IO3
  PF7     ------> QUADSPI_BK1_IO2
  PF8     ------> QUADSPI_BK1_IO0
  PF9     ------> QUADSPI_BK1_IO1
  PB2     ------> QUADSPI_CLK
  PB10     ------> QUADSPI_BK1_NCS */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_RCC_QSPI_FORCE_RESET();
	__HAL_RCC_QSPI_RELEASE_RESET();
	if(QSPI_Wait_Flag(1<<QUADSPI_SR_BUSY_Pos, 0, 0XFFFF) == 0)
	{
    tempreg  = ((ClockPrescaler-1) << QUADSPI_CR_PRESCALER_Pos);   //ClockPrescaler  AHB clock
    tempreg |= ((FifoThreshold-1) << QUADSPI_CR_FTHRES_Pos);      //4byte FIFO threshold le
    tempreg |= (0U << QUADSPI_CR_FSEL_Pos);        //1 Flash memory
    tempreg |= (0U << QUADSPI_CR_DFM_Pos);         //disable Dual-flash mode
    tempreg |= (1U << QUADSPI_CR_SSHIFT_Pos);      // Sample shift
    QUADSPI->CR = tempreg;
    tempreg  = ((FlashSize-1) << QUADSPI_DCR_FSIZE_Pos); //2^25 32M Flash
    tempreg |= (4U << QUADSPI_DCR_CSHT_Pos);            //5cycle
    tempreg |= (1U << QUADSPI_DCR_CKMODE_Pos);          //mode3
    QUADSPI->DCR = tempreg;
    QUADSPI->CR |= (1U << QUADSPI_CR_EN_Pos);          //endble QSPI

	}else return 1;
	return 0;                                            //Success return 0
}

void QSPI_Send_CMD(uint8_t cmd, uint32_t addr, uint8_t mode,  uint8_t cycle)
{
	uint32_t tempreg=0;
	if(QSPI_Wait_Flag(1<<QUADSPI_SR_BUSY_Pos, 0, 0XFFFF)==0)
	{
		tempreg  = (0U << QUADSPI_CCR_DDRM_Pos);   //disable DDRmode
		tempreg |= (0U << QUADSPI_CCR_SIOO_Pos);   //Each time sent command
		tempreg |= (0U << QUADSPI_CCR_FMODE_Pos);   //Indirect write mode
		tempreg |= ((uint32_t)mode>>6)<<24;		      //data mode
		tempreg |= ((uint32_t)(mode>>4)&0X03)<<12;	//addr size
		tempreg |= ((uint32_t)(mode>>2)&0X03)<<10;	//addr mode
		tempreg |= ((uint32_t)(mode>>0)&0X03)<<8;	  //cmd mode
		tempreg |= (cycle << QUADSPI_CCR_DCYC_Pos);   //empty cycle
		tempreg |= cmd;
		QUADSPI->CCR = tempreg;
		if(mode&0X0C)
		{
			QUADSPI->AR=addr;
		}
		if((mode&0XC0)==0)
		{
			if(QSPI_Wait_Flag(1<<QUADSPI_SR_TCF_Pos,1,0XFFFF) == 0)
			{
				QUADSPI->FCR |= QUADSPI_FCR_CTCF;			//clear CTEF flag
			}
		}
	}
}

uint8_t QSPI_Receive(uint8_t* buf, uint32_t datalen)
{
	uint32_t tempreg = QUADSPI->CCR;
	uint32_t addrreg = QUADSPI->AR;
	uint8_t status;
  __IO uint32_t *data_reg = &QUADSPI->DR;
	QUADSPI->DLR = datalen-1;
	tempreg &= ~(3<<QUADSPI_CCR_FMODE_Pos);
	tempreg |= 1<<QUADSPI_CCR_FMODE_Pos;
	QUADSPI->FCR |= 1<<QUADSPI_FCR_CTCF_Pos;
	QUADSPI->CCR = tempreg;
	QUADSPI->AR = addrreg;
	while(QSPI_Wait_Flag(3<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF));
  for( ; datalen--; )
  {
    *buf++ = *(__IO uint8_t *)data_reg;
  }
	if(QSPI_Wait_Flag(3<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF)==0)
	{
		QUADSPI->CR |= 1<<QUADSPI_CR_ABORT_Pos;
		status = QSPI_Wait_Flag(1<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF);
		if(status==0)
		{
			QUADSPI->FCR|=1<<QUADSPI_FCR_CTCF_Pos;
			status = QSPI_Wait_Flag(1<<QUADSPI_SR_BUSY_Pos, 0, 0XFFFF);
		}
	}
	return status;
}
uint8_t QSPI_16bitReceive(uint16_t* buf, uint32_t datalen)
{
	uint32_t tempreg = QUADSPI->CCR;
	uint32_t addrreg = QUADSPI->AR;
	uint8_t status;
  __IO uint32_t *data_reg = &QUADSPI->DR;
	QUADSPI->DLR = datalen-1;
	tempreg &= ~(3<<QUADSPI_CCR_FMODE_Pos);
	tempreg |= 1<<QUADSPI_CCR_FMODE_Pos;
	QUADSPI->FCR |= 1<<QUADSPI_FCR_CTCF_Pos;
	QUADSPI->CCR = tempreg;
	QUADSPI->AR = addrreg;
	while(QSPI_Wait_Flag(3<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF));
  uint32_t i = datalen>>1;
  if(infoSettings.rotate_ui)
  {
    for( ; datalen-=2; )
    {
      *buf++ = *(__IO uint8_t *)data_reg<<8 | *(__IO uint8_t *)data_reg;
    }
  }
  else
  {
    for( ; datalen-=2; )
    {
      buf[i--] = *(__IO uint8_t *)data_reg<<8 | *(__IO uint8_t *)data_reg;
    }
  }
	if(QSPI_Wait_Flag(3<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF)==0)
	{
		QUADSPI->CR |= 1<<QUADSPI_CR_ABORT_Pos;
		status = QSPI_Wait_Flag(1<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF);
		if(status==0)
		{
			QUADSPI->FCR|=1<<QUADSPI_FCR_CTCF_Pos;
			status = QSPI_Wait_Flag(1<<QUADSPI_SR_BUSY_Pos, 0, 0XFFFF);
		}
	}
	return status;
}

uint8_t QSPI_Transmit(uint8_t* buf, uint32_t datalen)
{
	uint32_t tempreg = QUADSPI->CCR;
	uint8_t status = 1;
	__IO uint32_t *data_reg = &QUADSPI->DR;
	QUADSPI->DLR = datalen-1;
	tempreg &= ~(3<<QUADSPI_CCR_FMODE_Pos);
	tempreg |= 0<<QUADSPI_CCR_FMODE_Pos;
	QUADSPI->FCR |= 1<<QUADSPI_FCR_CTCF_Pos;
	QUADSPI->CCR = tempreg;
  while(QSPI_Wait_Flag(3<<QUADSPI_SR_TCF_Pos, 1, 0XFFFF));
	for( ; datalen--; )
	{
		*(__IO uint8_t *)data_reg = *buf++;
	}
	if(QSPI_Wait_Flag(1<<QUADSPI_SR_TCF_Pos,1,0XFFFF)==0)
	{
		QUADSPI->CR|=1<<QUADSPI_CR_ABORT_Pos;
		status = QSPI_Wait_Flag(1<<QUADSPI_SR_TCF_Pos,1,0XFFFF);
		if(status==0)
		{
			QUADSPI->FCR |= 1<<QUADSPI_FCR_CTCF_Pos;
			status=QSPI_Wait_Flag(1<<QUADSPI_SR_BUSY_Pos,0,0XFFFF);
		}
	}
	return status;
}

void QSPI_Enable(void)
{
	uint8_t stareg2;
	stareg2=W25Qxx_ReadSR(W25Q_ReadStatusReg2);
	if((stareg2&0X02)==0)
	{
		W25Qxx_WriteEnable();
		stareg2 |= 1<<1;
		W25Qxx_WriteSR(W25Q_WriteStatusReg2,stareg2);
	}
	QSPI_Send_CMD(W25Q_EnterQPIMode,0, (0<<6)|(0<<4)|(0<<2)|(1<<0), 0);
  W25QXX_QPI_MODE = 1;
}

void QSPI_Disable(void)
{
	QSPI_Send_CMD(W25Q_ExitQPIMode,0, (0<<6)|(0<<4)|(0<<2)|(3<<0), 0);
}


uint8_t W25Qxx_ReadSR(uint8_t command)
{
  uint8_t byte;
	if(W25QXX_QPI_MODE)QSPI_Send_CMD(command,0,(3<<6)|(0<<4)|(0<<2)|(3<<0),0);
	else QSPI_Send_CMD(command,0,(1<<6)|(0<<4)|(0<<2)|(1<<0),0);
	QSPI_Receive(&byte,1);
	return byte;
}

void W25Qxx_WriteSR(uint8_t command,uint8_t sr)
{
	if(W25QXX_QPI_MODE)QSPI_Send_CMD(command,0,(3<<6)|(0<<4)|(0<<2)|(3<<0),0);
	else QSPI_Send_CMD(command,0,(1<<6)|(0<<4)|(0<<2)|(1<<0),0);
	QSPI_Transmit(&sr,1);
}

void W25Qxx_WaitForWriteEnd(void)
{
	while((W25Qxx_ReadSR(W25Q_ReadStatusReg1)&0x01)==0x01);
}

void W25Qxx_WriteEnable(void)
{

	if(W25QXX_QPI_MODE)QSPI_Send_CMD(W25Q_WriteEnable,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);
	else QSPI_Send_CMD(W25Q_WriteEnable,0,(0<<6)|(0<<4)|(0<<2)|(1<<0),0);
}

void W25Qxx_WriteDisable(void)
{
  if(W25QXX_QPI_MODE)QSPI_Send_CMD(W25Q_WriteDisable,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);
  else QSPI_Send_CMD(W25Q_WriteDisable,0,(0<<6)|(0<<4)|(0<<2)|(1<<0),0);
}


uint16_t W25Qxx_ReadID(void)
{
	uint8_t temp[2];
	uint16_t deviceid;
	if(W25QXX_QPI_MODE)QSPI_Send_CMD(W25Q_ManufactDeviceID,0,(3<<6)|(2<<4)|(3<<2)|(3<<0),0);
	else QSPI_Send_CMD(W25Q_ManufactDeviceID,0,(1<<6)|(2<<4)|(1<<2)|(1<<0),0);
	QSPI_Receive(temp,2);
	deviceid=(temp[0]<<8)|temp[1];
	return deviceid;
}

uint32_t W25Qxx_ReadCapacity(void)
{
  uint8_t id = (uint8_t)(W25Qxx_ReadID() & 0x00ff);

  for (uint8_t i = 0; i < sizeof(cap_ID); i++)
  {
    if (id == cap_ID[i])
    {
      return flash_size[i];
    }
  }
  return 0;
}

void W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  QSPI_Send_CMD(W25Q_FastReadData,ReadAddr, (3<<6)|(3<<4)|(3<<2)|(3<<0), 8);
  QSPI_Receive(pBuffer,NumByteToRead);
}

void W25Qxx_Read16bitBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  QSPI_Send_CMD(W25Q_FastReadData,ReadAddr, (3<<6)|(3<<4)|(3<<2)|(3<<0), 8);
  QSPI_16bitReceive(pBuffer,NumByteToRead);
}

void W25Qxx_WritePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	W25Qxx_WriteEnable();
	QSPI_Send_CMD(W25Q_PageProgram,WriteAddr, (3<<6)|(3<<4)|(3<<2)|(3<<0), 0);
	QSPI_Transmit(pBuffer,NumByteToWrite);
	W25Qxx_WaitForWriteEnd();
}

void W25Qxx_WriteBuffer(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
  if (Addr == 0) //WriteAddr is sFLASH_PAGESIZE aligned
  {
    if (NumOfPage == 0) // NumByteToWrite < sFLASH_PAGESIZE
    {
      W25Qxx_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else //NumByteToWrite > sFLASH_PAGESIZE
    {
      while(NumOfPage--)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
      W25Qxx_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else // WriteAddr is not sFLASH_PAGESIZE aligned  
  {
   //  SERIAL_ECHO("Addr!=0");
    if (NumOfPage == 0) // NumByteToWrite < sFLASH_PAGESIZE 
    {
      //  SERIAL_ECHO("NumOfPage=0");
      if (NumOfSingle > count) // (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE
      {
      //   SERIAL_ECHO("NumOfSingle > count");
        temp = NumOfSingle - count;
        W25Qxx_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
        W25Qxx_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
      //   SERIAL_ECHO("NumOfSingle <= count");
        W25Qxx_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else //NumByteToWrite > sFLASH_PAGESIZE
    {
      // SERIAL_ECHO("NumOfPage!=0");
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      W25Qxx_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        W25Qxx_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

void W25Qxx_EraseBulk(void)
{
  // SERIAL_ECHO_MSG("Erase_Bulk");
  W25Qxx_WriteEnable();
  W25Qxx_WaitForWriteEnd();
	QSPI_Send_CMD(W25Q_ChipErase,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);
	W25Qxx_WaitForWriteEnd();
}

void W25Qxx_EraseBlock(uint32_t BlockAddr)
{
 	// SERIAL_ECHO_MSG("Erase_Block:",BlockAddr);
  W25Qxx_WriteEnable();
  W25Qxx_WaitForWriteEnd();
	QSPI_Send_CMD(W25Q_BlockErase,BlockAddr,(0<<6)|(3<<4)|(3<<2)|(3<<0),0);
  W25Qxx_WaitForWriteEnd();
}


void W25Qxx_EraseSector(uint32_t SectorAddr)
{
  // SERIAL_ECHO_MSG("Erase_Sectir:",SectorAddr);
  W25Qxx_WriteEnable();
  W25Qxx_WaitForWriteEnd();
	QSPI_Send_CMD(W25Q_SectorErase,SectorAddr,(0<<6)|(3<<4)|(3<<2)|(3<<0),0);
  W25Qxx_WaitForWriteEnd();
}


void W25Qxx_Init(void)
{
  uint8_t temp;
	QSPI_Init(2, 4, 25);  // 1/2AHB Clock, Fifo Threshold 4(1~32), Flash Size 2^25=32M
  QSPI_Enable();
	W25QXX_TYPE=W25Qxx_ReadID();
	if(W25QXX_TYPE == 0xEF18)
  {
    temp=W25Qxx_ReadSR(W25Q_ReadStatusReg3);
    if((temp&0X01)==0)
		{
			W25Qxx_WriteEnable();
			QSPI_Send_CMD(W25Q_Enable4ByteAddr,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);
		}
		W25Qxx_WriteEnable();
		QSPI_Send_CMD(W25Q_SetReadParam,0, (3<<6)|(0<<4)|(0<<2)|(3<<0), 0);
		temp=3<<4;
		QSPI_Transmit(&temp,1);
  }
}
