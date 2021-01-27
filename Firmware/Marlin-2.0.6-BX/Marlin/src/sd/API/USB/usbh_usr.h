#ifndef __USBH_DISKIO_H
#define __USBH_DISKIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../../fatfs/diskio.h"

#define		USB_DEFAULT_BLOCK_SIZE		512

uint8_t USBH_UDISK_IsConnected(void);
DSTATUS USBH_UDISK_Status(void);
DRESULT USBH_UDISK_Read(uint8_t* buf, uint32_t sector, uint32_t cnt);
DRESULT USBH_UDISK_Write(uint8_t* buf, uint32_t sector, uint32_t cnt);
DRESULT USBH_UDISK_IOCtl(BYTE cmd, void *buff);

#ifdef __cplusplus
}
#endif

#endif 
