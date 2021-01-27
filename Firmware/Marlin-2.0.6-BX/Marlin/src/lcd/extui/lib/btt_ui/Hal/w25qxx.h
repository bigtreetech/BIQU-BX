#ifndef _W25QXX_H_
#define _W25QXX_H_

#ifdef __cplusplus
extern "C" { /* C-declarations for C++ */
#endif

#include <stdint.h>

#define W25Q_WriteEnable      0x06
#define W25Q_WriteDisable     0x04
#define W25Q_ReadStatusReg1   0x05
#define W25Q_ReadStatusReg2   0x35
#define W25Q_ReadStatusReg3   0x15
#define W25Q_WriteStatusReg1  0x01
#define W25Q_WriteStatusReg2  0x31
#define W25Q_WriteStatusReg3  0x11
#define W25Q_ReadData         0x03
#define W25Q_FastReadData     0x0B
#define W25Q_FastReadDual     0x3B
#define W25Q_PageProgram      0x02
#define W25Q_BlockErase       0xD8
#define W25Q_SectorErase      0x20
#define W25Q_ChipErase        0xC7
#define W25Q_PowerDown        0xB9
#define W25Q_ReleasePowerDown 0xAB
#define W25Q_DeviceID         0xAB
#define W25Q_ManufactDeviceID 0x90
#define W25Q_JedecDeviceID    0x9F
#define W25Q_Enable4ByteAddr  0xB7
#define W25Q_Exit4ByteAddr    0xE9
#define W25Q_SetReadParam     0xC0
#define W25Q_EnterQPIMode     0x38
#define W25Q_ExitQPIMode      0xFF
#define W25Q_FastReadQuad     0xEB

#define WIP_Flag              0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte            0xA5

#define SPI_FLASH_SectorSize  4096
#define SPI_FLASH_PageSize    256

#define KBYTE(x)  (x * 1024ul)
#define MBYTE(x)  (x * 1024ul * 1024ul)

void W25Qxx_Init(void);
uint16_t W25Qxx_ReadID(void);
uint32_t W25Qxx_ReadCapacity(void);
uint8_t W25Qxx_ReadSR(uint8_t command);
void W25Qxx_WriteSR(uint8_t command,uint8_t sr);
void W25Qxx_WaitForWriteEnd(void);
void W25Qxx_WriteEnable(void);
void W25Qxx_WriteDisable(void);
void W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
void W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Qxx_EraseSector(uint32_t SectorAddr);
void W25Qxx_EraseBlock(uint32_t BlockAddr);
void W25Qxx_EraseBulk(void);
void W25QXX_Memory_Mapped_Enable(void);
void W25Qxx_Read16bitBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);

#ifdef __cplusplus
} /* C-declarations for C++ */
#endif

#endif
