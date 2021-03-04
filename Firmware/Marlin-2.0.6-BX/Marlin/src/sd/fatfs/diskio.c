/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#define SD_SPI_SUPPORT
#define U_DISK_SUPPORT
#ifdef U_DISK_SUPPORT
  #include "../API/USB/usbh_usr.h"
#endif
//#include "variants.h"

#ifdef SD_SPI_SUPPORT
  #include "../API/sd.h"
#else
  #include "sdio_sdcard.h"
#endif

#include "../API/wtd_called.h"

/* Definitions of physical drive number for each drive */\
#define DEV_MMC		0	/* MMC/SD card to physical drive 0 */
#define DEV_USB		1	/* USB disk to physical drive 1 */

static volatile DSTATUS diskStatus[FF_VOLUMES] = {STA_NOINIT, STA_NOINIT, STA_NOINIT};	/* Disk status */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  if (pdrv >= FF_VOLUMES)	return STA_NOINIT;

	return diskStatus[pdrv];
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  for(uint8_t i = 0; i < FF_VOLUMES; i++)
  {
    diskStatus[i] = STA_NOINIT;
  }

	switch (pdrv) {
    case VOLUMES_TFT_SD:
      SD_Set_Slot(SD_SLOT_TFT);
      if (SD_Init() == 0)
      {
        diskStatus[pdrv] &= ~STA_NOINIT;
      }
      break;

    #ifdef U_DISK_SUPPORT
      case VOLUMES_U_DISK:
        if (USBH_UDISK_IsConnected())
        {
          diskStatus[pdrv] &= ~STA_NOINIT;
        }
        break;
    #endif

    case VOLUMES_BOARD_SD:
      SD_Set_Slot(SD_SLOT_BOARD);
      if (SD_Init() == 0)
      {
        diskStatus[pdrv] &= ~STA_NOINIT;
      }
      break;

      default:
        return RES_PARERR;
	}
	return diskStatus[pdrv];
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
#include "string.h"
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
  BYTE tmp_buff[count * 512];
  uint8_t retry = 5;
  if (!count) return RES_PARERR;
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;

  wtd_update();

	switch (pdrv) {
    case VOLUMES_TFT_SD:
      SD_Set_Slot(SD_SLOT_TFT);

    read_again:
      while (SD_ReadDisk((uint8_t *)buff, sector, count) != 0 && --retry) { // read error
        wtd_update();
        SD_Init();	//init again
      }

      while (SD_ReadDisk((uint8_t *)tmp_buff, sector, count) != 0 && --retry) { // read error
        wtd_update();
        SD_Init();	//init again
      }
      if (memcmp(tmp_buff, buff, count * 512) != 0)
        goto read_again;

      if (retry) return RES_OK;
      else return RES_ERROR;

    #ifdef U_DISK_SUPPORT
      case VOLUMES_U_DISK:
        if(USBH_UDISK_Read(buff, sector, count) == 0)
          return RES_OK;
        else
          return RES_ERROR;
    #endif

    case VOLUMES_BOARD_SD:
      SD_Set_Slot(SD_SLOT_BOARD);

      while (SD_ReadDisk((uint8_t *)buff, sector, count) != 0 && --retry) { // read error
        wtd_update();
        SD_Init();	//init again
      }

      if (retry) return RES_OK;
      else return RES_ERROR;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
  uint8_t retry = 5;
  if (!count) return RES_PARERR;
  if (diskStatus[pdrv] & STA_NOINIT) return RES_NOTRDY;
  if (diskStatus[pdrv] & STA_PROTECT) return RES_WRPRT;

  wtd_update();

	switch (pdrv) {
    case VOLUMES_TFT_SD:
      SD_Set_Slot(SD_SLOT_TFT);

      while (SD_WriteDisk((uint8_t *)buff, sector, count) != 0 && --retry) { // read error
        wtd_update();
        SD_Init();	//init again
      }

      if (retry) return RES_OK;
      else return RES_ERROR;
      SD_Set_Slot(SD_SLOT_TFT);

    #ifdef U_DISK_SUPPORT
      case VOLUMES_U_DISK:
        if(USBH_UDISK_Write((uint8_t*)buff, sector, count) == 0)
          return RES_OK;
        else
          return RES_ERROR;
    #endif

    case VOLUMES_BOARD_SD:
      SD_Set_Slot(SD_SLOT_BOARD);

      while (SD_WriteDisk((uint8_t *)buff, sector, count) != 0 && --retry) { // read error
        wtd_update();
        SD_Init();	//init again
      }

      if (retry) return RES_OK;
      else return RES_ERROR;

  }

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
    case VOLUMES_TFT_SD:
      return RES_OK;

    #ifdef U_DISK_SUPPORT
      case VOLUMES_U_DISK:
      {
        return USBH_UDISK_IOCtl(cmd, buff);
      }
    #endif

    case VOLUMES_BOARD_SD:
      return RES_OK;
	}

	return RES_PARERR;
}
