#include "ui_draw.h"
#include "../../includes.h"

uint32_t getBMPsize(uint8_t * w, uint8_t * h, uint32_t address)
{
  uint16_t len = sizeof(uint16_t);
  W25Qxx_ReadBuffer(w, address, len);
  address +=len;
  W25Qxx_ReadBuffer(h, address, len);
  address +=len;
  return address;
}

#ifdef STM32_HAS_FSMC

void lcd_frame_display(uint16_t sx,uint16_t sy,uint16_t w,uint16_t h, uint32_t addr);

#else

void lcd_frame_display(uint16_t sx,uint16_t sy,uint16_t w,uint16_t h, uint32_t addr)
{
  uint16_t *color =(uint16_t *)0XC0400000;
  // W25Qxx_ReadBuffer(color, addr, w * h *2);
  // for (uint16_t y = sy; y < sy + h; y++) {
  //   for (uint16_t x = sx; x < sx + w; x++) {
  //     uint16_t c = color[0] << 8 | color[1];
  //     color += 2;
  //    GUI_DrawPixel(x, y, c);
  //   }
  // }
  
  W25Qxx_Read16bitBuffer(color, addr, w * h *2);
  LTDC_Color_Fill(sx,sy,sx+w,sy+h,color);
}
#endif

void LOGO_ReadDisplay(void)
{
  uint16_t w, h;
  uint32_t addr = getBMPsize((u8 *)&w, (u8 *)&h, LOGO_ADDR);
  lcd_frame_display(0, 0, w, h, addr);
}

void ICON_ReadDisplay(uint16_t sx,uint16_t sy, uint8_t icon)
{
  uint16_t w, h;
  uint32_t addr = getBMPsize((u8 *)&w, (u8 *)&h, ICON_ADDR(icon));
  lcd_frame_display(sx, sy, w, h, addr);
}

uint16_t modelFileReadHalfword(FIL* fp)
{
	uint8_t ascii[4];
  UINT mybr;
  f_read(fp, ascii, 4 ,&mybr);
  if (mybr != 4) return 0;
	return string_2_uint32(ascii, 2);
}

bool model_DirectDisplay(GUI_POINT pos, char *gcode)
{
  FIL  gcodeFile;

  if(f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK) return false;
  // Move the file cursor to the corresponding resolution area
  f_lseek(&gcodeFile, MODEL_PREVIEW_OFFSET);
  // Check whether the icon size matches
  if (modelFileReadHalfword(&gcodeFile) != ICON_WIDTH || modelFileReadHalfword(&gcodeFile) != ICON_HEIGHT) return false;
  // Move to next line
  f_lseek(&gcodeFile, gcodeFile.fptr + 3);

  for (uint16_t y = 0; y < ICON_HEIGHT; y++) {
    for (uint16_t x = 0; x < ICON_WIDTH; x++) {
      GUI_DrawPixel(pos.x + x, pos.y + y, modelFileReadHalfword(&gcodeFile));
    }
    // Move to next line
    f_lseek(&gcodeFile, gcodeFile.fptr + 3);
  }

  return true;
}

bool model_DecodeToFlash(char *gcode)
{
  uint32_t addr = ICON_ADDR(ICON_PREVIEW);
  uint16_t bnum;
  uint16_t w = ICON_WIDTH;
  uint16_t h = ICON_HEIGHT;
  uint8_t buf[256];
  FIL  gcodeFile;

  if(f_open(&gcodeFile, gcode, FA_OPEN_EXISTING | FA_READ) != FR_OK) return false;
  // Move the file cursor to the corresponding resolution area
  f_lseek(&gcodeFile, MODEL_PREVIEW_OFFSET);
  // Check whether the icon size matches
  if (modelFileReadHalfword(&gcodeFile) != w || modelFileReadHalfword(&gcodeFile) != h) return false;
  // Move to next line
  f_lseek(&gcodeFile, gcodeFile.fptr + 3);

  for(bnum = 0; bnum < (w*h*2+W25QXX_SECTOR_SIZE-1)/W25QXX_SECTOR_SIZE; bnum++) {
    W25Qxx_EraseSector(addr + bnum*W25QXX_SECTOR_SIZE);
  }
  bnum=0;

  memcpy(buf, (uint8_t *)&w, sizeof(uint16_t));
  bnum += sizeof(uint16_t);
  memcpy(buf + bnum, (uint8_t *)&h, sizeof(uint16_t));
  bnum += sizeof(uint16_t);

  for (uint16_t y = 0; y < h; y++) {
    for (uint16_t x = 0; x < w; x++) {
      uint16_t color = modelFileReadHalfword(&gcodeFile);
      buf[bnum++]=(uint8_t)(color >> 8);
      buf[bnum++]=(uint8_t)(color & 0xFF);

      if(bnum == 256)
      {
        W25Qxx_WritePage(buf,addr,256);
        addr+=256;
        bnum=0;
      }
    }
    // Move to next line
    f_lseek(&gcodeFile, gcodeFile.fptr + 3);
  }
  W25Qxx_WritePage(buf, addr, bnum);

  return true;
}

//draw icon with different length and width (sx & sy cordinates for top left of icon, w width, h height, addr flash byte address)
void ICON_CustomReadDisplay(uint16_t sx,uint16_t sy,uint16_t w, uint16_t h, uint32_t address)
{
  // uint16_t w,h;
  address = getBMPsize((u8 *)&w, (u8 *)&h, address);
  lcd_frame_display(sx, sy, w, h, address);
}

void SMALLICON_ReadDisplay(uint16_t sx,uint16_t sy, uint8_t icon)
{
  lcd_frame_display(sx, sy, SMALLICON_WIDTH, SMALLICON_HEIGHT, SMALL_ICON_ADDR(icon));
}

void ICON_PressedDisplay(uint16_t sx,uint16_t sy, uint8_t icon)
{
  uint16_t w,h;
  uint16_t *color =(uint16_t *)0XC0400000;
  uint16_t mode=0x0FF0;
  uint32_t address = getBMPsize((uint8_t *)&w, (uint8_t *)&h, ICON_ADDR(icon));
  uint32_t Num = w * h *2;
  W25Qxx_Read16bitBuffer(color, address, Num);
  Num = Num>>1;              // 8bit Length >> 16bit length
  for (uint32_t i = 0; i < Num; i++) {
    color[i] = color[i] & mode;
  }
  LTDC_Color_Fill(sx,sy, sx+w, sy+h, color);
}
