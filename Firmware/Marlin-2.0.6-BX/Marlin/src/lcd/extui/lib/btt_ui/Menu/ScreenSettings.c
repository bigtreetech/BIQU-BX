#include "ScreenSettings.h"
#include "../includes.h"
#include "../API/Colors.h"

#ifdef BUZZER_PIN // Speaker
  #define BUZZER_KEY_INDEX KEY_ICON_3
#endif


#ifdef ST7920_SPI // LCD12864 color settings
  #ifdef BUZZER_PIN
    #define LCD12864_BG_INDEX KEY_ICON_4
  #else
    #define LCD12864_BG_INDEX KEY_ICON_3
  #endif
  #define LCD12864_FN_INDEX (LCD12864_BG_INDEX+1)
#endif
static SETTINGS now;

MENUITEMS screenSettingsItems = {
  // title
  LABEL_SCREEN_SETTINGS,
  // icon                       label
   {{ICON_LANGUAGE,             LABEL_LANGUAGE},
    {ICON_ROTATE_UI,            LABEL_ROTATE_UI},
    {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK}}
};
LISTITEM totalItems[TSC_LANGUAGE_NUM];

void menuLanguageCallBack(void)
{
  #define LANGUAGE_PAGE_COUNT  (LANGUAGE_NUM + LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE


  KEY_VALUES key_num = menuKeyGetValue();
  switch (key_num)
  {
  case KEY_ICON_5:
    listWidgetPreviousPage();
    break;

  case KEY_ICON_6:
    listWidgetNextPage();
    break;

  case KEY_ICON_7:
    infoMenu.cur--;
    break;

  default:
    if(key_num < LISTITEM_PER_PAGE){
      uint16_t cur_item = infoSettings.language;
      uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
      if (tmp_i != cur_item) { // has changed
        totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
        listWidgetRefreshItem(cur_item); // refresh unchecked status
        cur_item = tmp_i;
        totalItems[cur_item].icon = ICONCHAR_CHECKED;
        listWidgetRefreshItem(cur_item); // refresh checked status

        infoSettings.language = cur_item;
        menuDrawTitle(textSelect(LABEL_LANGUAGE));
      }
    }
    break;
  }

  if((infoMenu.menu[infoMenu.cur] == menuLanguage) && memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    statusScreen_setReady(); // restore msg buffer when language is changed
    storePara();
  }
}

void menuLanguage(void)
{
  LABEL title = {LABEL_LANGUAGE};
  // fill language items
  uint8_t tmp_language = infoSettings.language;
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (i == tmp_language) {
      totalItems[i].icon = ICONCHAR_CHECKED;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    infoSettings.language = i;
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = textSelect(LABEL_LANGUAGE);
  }
  infoSettings.language = tmp_language;

  listWidgetCreate(title, totalItems, COUNT(totalItems), infoSettings.language / LISTITEM_PER_PAGE);
  now = infoSettings;
  menuSetFrontCallBack(menuLanguageCallBack);
}

#ifdef ST7920_SPI
static uint8_t cur_item = 0;
LISTITEM st7920_totalItems[LCD_COLOR_COUNT];
const LABEL lcd_colors_names[LCD_COLOR_COUNT] =
{
  LABEL_WHITE,
  LABEL_BLACK,
  LABEL_RED,
  LABEL_GREEN,
  LABEL_BLUE,
  LABEL_CYAN,
  LABEL_MAGENTA,
  LABEL_YELLOW,
  LABEL_ORANGE,
  LABEL_PURPLE,
  LABEL_LIME,
  LABEL_BROWN,
  LABEL_DARKBLUE,
  LABEL_DARKGREEN,
  LABEL_GRAY,
  LABEL_DARKGRAY,
};

void menuSimulatorBackGroundColor(void);
void menuSimulatorBackGroundColorCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch (key_num)
  {
  case KEY_ICON_5:
    listWidgetPreviousPage();
    break;

  case KEY_ICON_6:
    listWidgetNextPage();
    break;

  case KEY_ICON_7:
    infoMenu.cur--;
    break;

  default:
    if(key_num < LISTITEM_PER_PAGE){
      uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
      if (tmp_i != cur_item) { // has changed
        st7920_totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
        listWidgetRefreshItem(cur_item); // refresh unchecked status
        cur_item = tmp_i;
        st7920_totalItems[cur_item].icon = ICONCHAR_CHECKED;
        listWidgetRefreshItem(cur_item); // refresh checked status

        infoSettings.marlin_mode_bg_color = lcd_colors[cur_item];
      }
    }
    break;
  }

  if((infoMenu.menu[infoMenu.cur] != menuSimulatorBackGroundColor) && memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuSimulatorBackGroundColor(void)
{
  LABEL title = {LABEL_SIMULATOR_BG_COLOR};

  uint8_t cur_item = 0;

  // fill items
  for(uint8_t i = 0; i < COUNT(st7920_totalItems); i++) {
    if (infoSettings.marlin_mode_bg_color == lcd_colors[i]) {
      st7920_totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      st7920_totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    st7920_totalItems[i].itemType = LIST_LABEL;
    st7920_totalItems[i].titlelabel = lcd_colors_names[i];
  }

  listWidgetCreate(title, st7920_totalItems, COUNT(st7920_totalItems), cur_item/ LISTITEM_PER_PAGE);
  now = infoSettings;
  menuSetFrontCallBack(menuSimulatorBackGroundColorCallBack);
}

void menuSimulatorFontColor(void);
void enuSimulatorFontColorCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch (key_num)
  {
  case KEY_ICON_5:
    listWidgetPreviousPage();
    break;

  case KEY_ICON_6:
    listWidgetNextPage();
    break;

  case KEY_ICON_7:
    infoMenu.cur--;
    break;

  default:
    if(key_num < LISTITEM_PER_PAGE){
      uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
      if (tmp_i != cur_item) { // has changed
        st7920_totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
        listWidgetRefreshItem(cur_item); // refresh unchecked status
        cur_item = tmp_i;
        st7920_totalItems[cur_item].icon = ICONCHAR_CHECKED;
        listWidgetRefreshItem(cur_item); // refresh checked status

        infoSettings.marlin_mode_font_color = lcd_colors[cur_item];
      }
    }
    break;
  }

  if((infoMenu.menu[infoMenu.cur] != menuSimulatorFontColor) && memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuSimulatorFontColor(void)
{
  LABEL title = {LABEL_SIMULATOR_FONT_COLOR};
  // fill items
  for(uint8_t i = 0; i < COUNT(st7920_totalItems); i++) {
    if (infoSettings.marlin_mode_font_color == lcd_colors[i]) {
      st7920_totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      st7920_totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    st7920_totalItems[i].itemType = LIST_LABEL;
    st7920_totalItems[i].titlelabel = lcd_colors_names[i];
  }

  listWidgetCreate(title, st7920_totalItems, COUNT(st7920_totalItems), cur_item/ LISTITEM_PER_PAGE);
  now = infoSettings;
  menuSetFrontCallBack(enuSimulatorFontColorCallBack);
}

#endif

#ifdef BUZZER_PIN

LISTITEMS sounditems = {
// title
LABEL_FEATURE_SETTINGS,
// icon                 ItemType      Item Title        item value text(only for custom value)
{
  {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_TOUCH_SOUND,  LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_ALERT_SOUND,  LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_TOAST_SOUND,  LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
  {ICONCHAR_BACK,       LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},}
};

void menuSoundSettings(void);
void menuSoundSettingsCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
  case KEY_ICON_0:
    infoSettings.touchSound = (infoSettings.touchSound + 1) % 2;
    sounditems.items[key_num].icon = (infoSettings.touchSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
    menuDrawListItem(&sounditems.items[key_num], key_num);
    break;

  case KEY_ICON_1:
    infoSettings.alertSound = (infoSettings.alertSound + 1) % 2;
    sounditems.items[key_num].icon = (infoSettings.alertSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
    menuDrawListItem(&sounditems.items[key_num], key_num);
    break;

  case KEY_ICON_2:
    infoSettings.toastSound = (infoSettings.toastSound + 1) % 2;
    sounditems.items[key_num].icon = (infoSettings.toastSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
    menuDrawListItem(&sounditems.items[key_num], key_num);
    break;

  case KEY_ICON_7:
    infoMenu.cur--;
    break;

  default:
    break;
  }

  if ((infoMenu.menu[infoMenu.cur] != menuSoundSettings) && memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
} //menuSoundSettings

void menuSoundSettings(void)
{
  sounditems.items[0].icon = (infoSettings.touchSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[1].icon = (infoSettings.alertSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[2].icon = (infoSettings.toastSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  menuDrawListPage(&sounditems);
  now = infoSettings;
  menuSetFrontCallBack(menuSoundSettingsCallBack);
}

#endif

void menuScreenSettingsCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      infoMenu.menu[++infoMenu.cur] = menuLanguage;
      break;

    case KEY_ICON_1:
      infoSettings.rotate_ui = !infoSettings.rotate_ui;
      ltdc_dir = infoSettings.rotate_ui;
      // if(!CapScreen)
      // {
      //   TSC_Calibration();
      // }
      storePara();
      LCD_RefreshDirection();
      menuDrawPage(&screenSettingsItems);
      break;

    case KEY_ICON_2:
      // if(CapScreen == true) break;
      // TSC_Calibration();
      // menuDrawPage(&screenSettingsItems);
      break;

    #ifdef BUZZER_PIN
    case BUZZER_KEY_INDEX:
      infoMenu.menu[++infoMenu.cur] = menuSoundSettings;
      break;
    #endif

    #ifdef ST7920_SPI
    case LCD12864_BG_INDEX:
      infoMenu.menu[++infoMenu.cur] = menuSimulatorBackGroundColor;
      break;

    case LCD12864_FN_INDEX:
      infoMenu.menu[++infoMenu.cur] = menuSimulatorFontColor;
      break;
    #endif

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:
      break;
  }
  loopFrontEnd();
  if((infoMenu.menu[infoMenu.cur] != menuScreenSettings) && memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuScreenSettings(void)
{
    //load buzzer icon
  #ifdef BUZZER_PIN
    screenSettingsItems.items[BUZZER_KEY_INDEX].icon = ICON_SOUND;
    screenSettingsItems.items[BUZZER_KEY_INDEX].label.index = LABEL_SOUND;
  #endif

  #ifdef ST7920_SPI
    // LCD12864 background color
    screenSettingsItems.items[LCD12864_BG_INDEX].icon = ICON_BKCOLOR;
    screenSettingsItems.items[LCD12864_BG_INDEX].label.index = LABEL_CUSTOM;

    screenSettingsItems.items[LCD12864_FN_INDEX].icon = ICON_FONTCOLOR;
    screenSettingsItems.items[LCD12864_FN_INDEX].label.index = LABEL_CUSTOM;

    for(u8 i = 0; i < LCD_COLOR_COUNT; i++)
    {
      if(infoSettings.marlin_mode_bg_color == lcd_colors[i])
      {
        screenSettingsItems.items[LCD12864_BG_INDEX].label = lcd_color_names[i];
      }
    }

    // LCD12864 font color
    for(u8 i = 0; i < LCD_COLOR_COUNT; i++)
    {
      if(infoSettings.marlin_mode_font_color == lcd_colors[i])
      {
        screenSettingsItems.items[LCD12864_FN_INDEX].label = lcd_color_names[i];
      }
    }

  #endif

  now = infoSettings;
  if(CapScreen){
    screenSettingsItems.items[2].icon = ICON_BACKGROUND;
    screenSettingsItems.items[2].label.index = LABEL_BACKGROUND;
  }
  else {
    screenSettingsItems.items[2].icon = ICON_TOUCHSCREEN_ADJUST;
    screenSettingsItems.items[2].label.index = LABEL_TOUCHSCREEN_ADJUST;
  }
  menuDrawPage(&screenSettingsItems);
  menuSetFrontCallBack(menuScreenSettingsCallBack);
}
