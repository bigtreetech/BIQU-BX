#include "usbh_usr.h"
#include "STM32_USB_Host_Library/Core/Inc/usbh_def.h"
#include "STM32_USB_Host_Library/Class/MSC/Inc/usbh_msc.h"
#include "../../../../src/lcd/extui/lib/btt_ui/API/UI/GUI.h"

USBH_HandleTypeDef hUSBHost;

#define WHITE                 0xFFFF
#define BLACK                 0x0000
#define RED                   0xF800
#define GREEN                 0x0760
#define BLUE                  0x00DF
#define CYAN                  0x07FF
#define MAGENTA               0xF81F
#define YELLOW                0xFFE0
#define ORANGE                0xFB23
#define PURPLE                0x7817
#define LIME                  0xBFE0
#define BROWN                 0X9240
#define DARKBLUE              0X0030
#define DARKGREEN             0x0340
#define GRAY                  0X8430
#define DARKGRAY              0x2124

bool u_disk_ready = false;

static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch (id)
  {
    case HOST_USER_SELECT_CONFIGURATION:
      break;
    case HOST_USER_CLASS_ACTIVE:
      u_disk_ready = true;
      break;
    case HOST_USER_CLASS_SELECTED:
      break;
    case HOST_USER_CONNECTION:
    break;
    case HOST_USER_DISCONNECTION:
      u_disk_ready = false;
      break;
    case HOST_USER_UNRECOVERED_ERROR:
    break;
    default:
      break;
  }
}

void USBH_HAL_Init(void)
{
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);
  USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
  USBH_Start(&hUSBHost);
}

void USB_HAL_Loop(void)
{
  USBH_Process(&hUSBHost);
}

uint8_t USBH_UDISK_IsConnected(void)
{
  if (!hUSBHost.device.is_connected || hUSBHost.device.is_disconnected || !u_disk_ready) return 0;
  return 1;
}

// rtv: 0: ready, 1: not ready
DSTATUS USBH_UDISK_Status(void)
{
  MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;

  if (!USBH_UDISK_IsConnected()) return RES_ERROR;

  if (USBH_MSC_UnitIsReady(&hUSBHost,MSC_Handle->current_lun))
  {
    return RES_OK;
  }
  else
  {
    return RES_ERROR;
  }
}


//
DRESULT USBH_UDISK_Read(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;

  if (!USBH_UDISK_IsConnected()) return RES_ERROR;

  while(!USBH_MSC_IsReady(&hUSBHost))
  {
    // Precess the unfinished USB event before being called by FatFs
    USBH_Process(&hUSBHost);
  }

  if(USBH_MSC_Read(&hUSBHost, MSC_Handle->current_lun, sector, buf, cnt) == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info);
    switch (info.sense.asc)
    {
      case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
      case SCSI_ASC_MEDIUM_NOT_PRESENT:
      case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
        USBH_ErrLog("USB Disk is not ready!");
        res = RES_NOTRDY;
        break;
      default:
        res = RES_ERROR;
        break;
    }
  }
  return res;
}

DRESULT USBH_UDISK_Write(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;

  if (!USBH_UDISK_IsConnected()) return RES_ERROR;

  while(!USBH_MSC_IsReady(&hUSBHost))
  {
    // Precess the unfinished USB event before being called by FatFs
    USBH_Process(&hUSBHost);
  }

  if(USBH_MSC_Write(&hUSBHost, MSC_Handle->current_lun, sector, buf, cnt) == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun, &info);
    switch (info.sense.asc)
    {
      case SCSI_ASC_WRITE_PROTECTED:
        USBH_ErrLog("USB Disk is Write protected!");
        res = RES_WRPRT;
        break;
      case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
      case SCSI_ASC_MEDIUM_NOT_PRESENT:
      case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
        USBH_ErrLog("USB Disk is not ready!");
        res = RES_NOTRDY;
        break;
      default:
        res = RES_ERROR;
        break;
    }
  }
  return res;
}

DRESULT USBH_UDISK_IOCtl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;

  if (!USBH_UDISK_IsConnected()) return RES_ERROR;

  switch(cmd)
  {
    case CTRL_SYNC:
      res = RES_OK;
      break;
    case GET_SECTOR_COUNT:
      if(USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info) == USBH_OK)
      {
        *(DWORD*)buff = info.capacity.block_nbr;
        res = RES_OK;
      }
      else
      {
        res = RES_ERROR;
      }
      break;
    case GET_SECTOR_SIZE:
      if(USBH_MSC_GetLUNInfo(&hUSBHost,MSC_Handle->current_lun,&info) == USBH_OK)
      {
        *(DWORD*)buff = info.capacity.block_size;
        res = RES_OK;
      }
      else
      {
        res = RES_ERROR;
      }
      break;
    case GET_BLOCK_SIZE:
      if(USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info) == USBH_OK)
      {
        *(DWORD*)buff  =info.capacity.block_size / USB_DEFAULT_BLOCK_SIZE;
        res = RES_OK;
      }
      else
      {
        res = RES_ERROR;
      }
      break;
    default:
      res = RES_PARERR;
  }
  return res;
}
