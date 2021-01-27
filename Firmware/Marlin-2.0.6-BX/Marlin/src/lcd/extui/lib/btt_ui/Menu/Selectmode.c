#include "../includes.h"
// #include "xpt2046.h"
// #include "GPIO_Init.h"
#include "Selectmode.h"
#include "../API/LCD_Encoder.h"

bool skipMode = false;

#define MODE_SPACE_X (BTT_LCD_WIDTH / LCD_MODE_COUNT)
#define MODE_START_X ((BTT_LCD_WIDTH - ICON_WIDTH * LCD_MODE_COUNT) / LCD_MODE_COUNT / 2)
#define MODE_START_Y ((BTT_LCD_HEIGHT - ICON_HEIGHT - BYTE_HEIGHT) / 2)

const GUI_RECT rect_of_mode[LCD_MODE_COUNT]={
  //3 select icon
  {MODE_START_X + 0 * MODE_SPACE_X, MODE_START_Y, MODE_START_X + 0 * MODE_SPACE_X + ICON_WIDTH, MODE_START_Y + ICON_HEIGHT},
  {MODE_START_X + 1 * MODE_SPACE_X, MODE_START_Y, MODE_START_X + 1 * MODE_SPACE_X + ICON_WIDTH, MODE_START_Y + ICON_HEIGHT},
  {MODE_START_X + 2 * MODE_SPACE_X, MODE_START_Y, MODE_START_X + 2 * MODE_SPACE_X + ICON_WIDTH, MODE_START_Y + ICON_HEIGHT},
};

const uint8_t icon_mode [LCD_MODE_COUNT]={
  ICON_MARLIN,
  ICON_BIGTREETECH,
  ICON_RPI,
};

void drawModeIcon(void)
{
  for(uint8_t i = 0;i < LCD_MODE_COUNT; i++)
  {
    ICON_ReadDisplay(rect_of_mode[i].x0, rect_of_mode[i].y0, icon_mode[i]);
  }

  const GUI_RECT mode_title_rect[LCD_MODE_COUNT] = {
    {0,                rect_of_mode[0].y1 + BYTE_HEIGHT/2,   MODE_SPACE_X,     rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT},
    {MODE_SPACE_X,     rect_of_mode[0].y1 + BYTE_HEIGHT/2,   2 * MODE_SPACE_X, rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT},
    {2 * MODE_SPACE_X, rect_of_mode[0].y1 + BYTE_HEIGHT/2,   3 * MODE_SPACE_X, rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT},
  };

  GUI_RestoreColorDefault();

  if(infoSettings.marlin_type == LCD12864)
    GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"LCD12864 Mode");
  else
    GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"LCD2004 Mode");

  GUI_DispStringInPrect(&mode_title_rect[1],(uint8_t *)"Touch Mode");
  GUI_DispStringInPrect(&mode_title_rect[2],(uint8_t *)"RPI Mode");
}

bool LCD_ReadPen(uint16_t intervals)
{
  // static uint32_t TouchTime = 0;
  // if (CapScreen) return false;
  // if(!XPT2046_Read_Pen())
  // {
  //   if(OS_GetTimeMs() - TouchTime > intervals)
  //   {
  //     return true;
  //   }
  // }
  // else
  // {
  //   TouchTime = OS_GetTimeMs();
  // }
  return false;
}

MKEY_VALUES MKeyGetValue(void)
{
  return (MKEY_VALUES)KEY_GetValue(COUNT(rect_of_mode), rect_of_mode);
}

void drawSelectedMode(int8_t nowMode)
{
  const uint8_t border_off = 4;
  for (uint8_t i = 0; i < LCD_MODE_COUNT; i++)
  {
    GUI_SetColor(i == nowMode ? infoSettings.marlin_mode_font_color : BACKGROUND_COLOR);
    GUI_DrawRect(rect_of_mode[i].x0 - border_off, rect_of_mode[i].y0 - border_off,
                 rect_of_mode[i].x1 + border_off, rect_of_mode[i].y1 + border_off);
  }
  GUI_RestoreColorDefault();
}

void loopCheckMode(void)
{
//  #ifndef CLEAN_MODE_SWITCHING_SUPPORT
//  IDEALLY I would like to be able to swap even when the TFT is in printing mode
//  but before I can allow that I need a way to make sure that we swap back into the right mode (and correct screen)
//  and I really want a reliable way to DETECT that the TFT should be in printing mode even when the print was started externally.
  if(isPrinting() || skipMode)
    return;

  if(infoMenu.menu[infoMenu.cur] == menuMode)
    return;

//  #endif
  if(LCD_ReadPen(LCD_CHANGE_MODE_INTERVALS) || encoder_ReadBtn(LCD_CHANGE_MODE_INTERVALS))
  {
    infoMenu.menu[++infoMenu.cur] = menuMode;
  }
}

int8_t  nowMode;
void menuModeCallBack(void)
{
  #if LCD_ENCODER_SUPPORT
    while(encoder_ReadBtn(LCD_BUTTON_INTERVALS)) {MarlinAPI_MainLoop();}      //wait for button release
  #else
    while((!CapScreen && !XPT2046_Read_Pen())){MarlinAPI_MainLoop();}      //wait for touch release
  #endif

  MKEY_VALUES key_num = MKeyGetValue();

  if(key_num == MKEY_0 || key_num == MKEY_1 || key_num == MKEY_2)
  {
    nowMode = key_num;
    infoMenu.cur--;
  }

  #if LCD_ENCODER_SUPPORT
    if(encoderPosition)
    {
      nowMode = NOBEYOND(0, nowMode + encoderPosition, LCD_MODE_COUNT - 1);
      drawSelectedMode(nowMode);
      encoderPosition = 0;
    }

    if(encoder_ReadBtn(0))
    {
      infoMenu.cur--;
    }
    loopCheckEncoderSteps();
  #endif

  #ifdef LCD_LED_PWM_CHANNEL
    loopDimTimer();
  #endif

  if (infoSettings.serial_alwaysOn == 1)
  {
    loopBackEnd();
  }

  if (infoMenu.menu[infoMenu.cur] != menuMode)
  {
    if (infoSettings.mode != nowMode)
    {
      infoSettings.mode = nowMode;
      storePara();
    }    
    infoMenuSelect();
  }
}


void menuMode(void)
{
  if (infoSettings.mode == LCD_MODE_RPI) {
    MarlinAPI_RPI_Set(false);
    LTDC_Init();
    if(GT911_Init())
    {
      CapScreen = true;
    }
    else
    {
      // CapScreen = false;
      // XPT2046_Init();
    }
  }

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif
  nowMode = infoSettings.mode;
  GUI_Clear(infoSettings.bg_color);
  drawModeIcon();
  drawSelectedMode(nowMode);
  TSC_ReDrawIcon = NULL; // Disable icon redraw callback function
  menuSetFrontCallBack(menuModeCallBack);
}
