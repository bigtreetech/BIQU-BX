#include "Mode.h"
#include "../includes.h"

bool serialHasBeenInitialized = false;
bool freshboot = true;

void Serial_ReSourceDeInit(void)
{
  if (!serialHasBeenInitialized) return;
  serialHasBeenInitialized = false;
  // Serial_DeInit();
}

void Serial_ReSourceInit(void)
{
  if (serialHasBeenInitialized) return;
  serialHasBeenInitialized = true;

  // memset(&infoHost, 0, sizeof(infoHost));
  reminderSetUnConnected(); // reset connect status
  // Serial_Init(infoSettings.baudrate);
}

void infoMenuSelect(void)
{
  infoMenu.cur = 0;

  switch(infoSettings.mode)
  {
    case LCD_MODE_MARLIN:
      #ifdef SHOW_BTT_BOOTSCREEN
        if (freshboot)
        {
          LOGO_ReadDisplay();
          freshboot = false;
        }
      #endif

      #if defined(ST7920_SPI) || defined(LCD2004_simulator)
        infoMenu.menu[infoMenu.cur] = menuMarlinMode;
      #endif
      break;

    case LCD_MODE_SERIAL_TSC:
    {
      #ifdef SHOW_BTT_BOOTSCREEN
        if (freshboot)
        {
          LOGO_ReadDisplay();
          freshboot = false;
        }
      #endif
      GUI_RestoreColorDefault();
      if(infoSettings.unified_menu == 1) //if Unified menu is selected
        infoMenu.menu[infoMenu.cur] = menuStatus; //status screen as default screen on boot
      else
        infoMenu.menu[infoMenu.cur] = classicMenu;   // classic UI
      break;
    }

    case LCD_MODE_RPI:
      LTDC_DeInit();
      GT911_DeInit();
      MarlinAPI_RPI_Set(true);
      infoMenu.menu[infoMenu.cur] = menuRPIMenu;   // classic UI
      break;
  }
}
