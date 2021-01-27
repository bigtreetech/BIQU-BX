#ifndef _SD_H_
#define _SD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
        

#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06     

#define SD_SLOT_BOARD   0x00
#define SD_SLOT_TFT     0x01

#define CMD0    0 
#define CMD1    1
#define CMD8    8
#define CMD9    9
#define CMD10   10
#define CMD12   12
#define CMD16   16
#define CMD17   17
#define CMD18   18 
#define CMD23   23
#define CMD24   24
#define CMD25   25
#define CMD41   41
#define CMD55   55
#define CMD58   58
#define CMD59   59


#define SD_DATA_OK                0x05
#define SD_DATA_CRC_ERROR         0x0B
#define SD_DATA_WRITE_ERROR       0x0D
#define SD_DATA_OTHER_ERROR       0xFF


#define SD_RESPONSE_NO_ERROR      0x00
#define SD_IN_IDLE_STATE          0x01
#define SD_ERASE_RESET            0x02
#define SD_ILLEGAL_COMMAND        0x04
#define SD_COM_CRC_ERROR          0x08
#define SD_ERASE_SEQUENCE_ERROR   0x10
#define SD_ADDRESS_ERROR          0x20
#define SD_PARAMETER_ERROR        0x40
#define SD_RESPONSE_FAILURE       0xFF


void SD_Set_Slot(uint8_t slot);
uint8_t SD_Init(void);
uint8_t SD_ReadDisk(uint8_t *buf, uint32_t sector, uint32_t cnt);
uint8_t SD_WriteDisk(uint8_t *buf, uint32_t sector, uint32_t cnt);

uint8_t SD_Wait_Ready(void);
uint8_t SD_Get_Ack(uint8_t Response);
uint32_t SD_Get_Sector_Count(void);
uint8_t SD_GetCID(uint8_t *cid_data);
uint8_t SD_GetCSD(uint8_t *csd_data);

#ifdef __cplusplus
}
#endif

#endif


