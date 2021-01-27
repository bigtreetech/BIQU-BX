#include "MarlinMode.h"
// #include "spi_slave.h"
// #include "HD44780.h"
#include "../includes.h"

#if defined(ST7920_SPI) || defined(LCD2004_simulator)

// typedef void (*CB_INIT)(CIRCULAR_QUEUE *);
// typedef void (*CB_DEINIT)(void);
// typedef bool (*CB_DATA)(uint8_t *);
// typedef void (*CB_PARSE)(uint8_t);

void menuMarlinModeCallBack(void)
{
  MarlinAPI_LCDUpdate();
}


void menuMarlinMode(void)
{
  GUI_Clear(infoSettings.marlin_mode_bg_color);
  GUI_SetColor(infoSettings.marlin_mode_font_color);
  GUI_SetBkColor(infoSettings.marlin_mode_bg_color);
  if(infoSettings.marlin_mode_showtitle == 1){
    STRINGS_STORE tempST;
    W25Qxx_ReadBuffer((uint8_t *)&tempST,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));
    GUI_DispStringInRect(0, 0, BTT_LCD_WIDTH, BYTE_HEIGHT, (uint8_t *)tempST.marlin_title);
  }
  menuSetFrontCallBack(menuMarlinModeCallBack);
  MarlinAPI_LCDInit();
}

void menuRPIMenuCallBack(void)
{
}

void menuRPIMenu(void)
{
  menuSetFrontCallBack(menuRPIMenuCallBack);
}

#endif
