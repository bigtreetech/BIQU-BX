#include "PowerFailed.h"
#include "../includes.h"

static bool powerFailedExist(void)
{
  return MarlinAPI_PowerFailedExist();
}

void powerFailedResume(void)
{
  MarlinAPI_PowerFailedResume();
}

void powerFailedDelete(void)
{
  MarlinAPI_PowerFailedDelete();
}

bool plring = false;
void menuPowerOffCallBack(void)
{
  uint16_t key_num = KEY_GetValue(2, doubleBtnRect);
  switch(key_num)
  {
    case KEY_POPUP_CONFIRM:
      infoMenu.menu[1] = menuPrintFromSource;
      infoMenu.menu[2] = menuBeforePrinting;
      infoMenu.cur = 2;
      plring = true;
      powerFailedResume();
      break;

    case KEY_POPUP_CANCEL:
      ExitDir();
      infoMenu.cur--;
      powerFailedDelete();
      break;
  }

  #ifdef SD_CD_PIN
  if(isVolumeExist(infoFile.source) != true)
  {
    infoMenu.cur--;
  }
  #endif
  // loopProcess();
  loopFrontEnd();
}


void menuPowerOff(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispString((BTT_LCD_WIDTH - GUI_StrPixelWidth(textSelect(LABEL_LOADING)))/2, BTT_LCD_HEIGHT/2 - BYTE_HEIGHT, textSelect(LABEL_LOADING));

  if(mountFS()==true && powerFailedExist())
  {
    popupDrawPage(DIALOG_TYPE_QUESTION, bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t* )infoFile.title,
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

    menuSetFrontCallBack(menuPowerOffCallBack);
  }
  else
  {
    infoMenu.cur--;
  }
}
