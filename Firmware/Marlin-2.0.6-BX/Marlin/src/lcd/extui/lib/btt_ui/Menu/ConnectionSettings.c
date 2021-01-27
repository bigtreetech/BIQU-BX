#include "ConnectionSettings.h"
#include "../includes.h"

// Set uart pins to input, free uart
void menuDisconnect(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(20, 0, BTT_LCD_WIDTH-20, BTT_LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, BTT_LCD_HEIGHT - (BYTE_HEIGHT*2), BTT_LCD_WIDTH-20, BTT_LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_ReSourceDeInit();
  while(!isPress()) {
  #ifdef LCD_LED_PWM_CHANNEL
    loopDimTimer();
  #endif
  }
  while(isPress()) {
  #ifdef LCD_LED_PWM_CHANNEL
    loopDimTimer();
  #endif
  }
  Serial_ReSourceInit();
  loopFrontEnd();
  infoMenu.cur--;
}

const char* item_baudrate_str[ITEM_BAUDRATE_NUM] = {"2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};
const uint32_t  item_baudrate[ITEM_BAUDRATE_NUM] = {2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000};
static SETTINGS now;
static LABEL title = {LABEL_BAUDRATE};
static LISTITEM totalItems[ITEM_BAUDRATE_NUM];
static uint8_t cur_item = 0;
void menuBaudrate(void);

void menuBaudrateCallBack(void)
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
        totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
        listWidgetRefreshItem(cur_item); // refresh unchecked status
        cur_item = tmp_i;
        totalItems[cur_item].icon = ICONCHAR_CHECKED;
        listWidgetRefreshItem(cur_item); // refresh checked status

        infoSettings.baudrate = item_baudrate[cur_item];
        Serial_ReSourceDeInit(); // Serial_Init() will malloc a dynamic memory, so Serial_DeInit() first to free, then malloc again.
        Serial_ReSourceInit();
        reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);
      }
    }
    break;
  }

  if((infoMenu.menu[infoMenu.cur] != menuBaudrate) && memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
  loopFrontEnd();
}

void menuBaudrate(void)
{
    // fill baudrate items
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (infoSettings.baudrate == item_baudrate[i]) {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = (uint8_t *)item_baudrate_str[i];
  }

  listWidgetCreate(title, totalItems, COUNT(totalItems), cur_item / LISTITEM_PER_PAGE);
  now = infoSettings;
  menuSetFrontCallBack(menuBaudrateCallBack);
}

void menuConnectionSettingsCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch (key_num)
  {
    // case KEY_ICON_0:
    //   infoMenu.menu[++infoMenu.cur] = menuBaudrate;
    //   break;

    // case KEY_ICON_1:
    //   infoMenu.menu[++infoMenu.cur] = menuDisconnect;
    //   break;

    case KEY_ICON_0:
      // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
      // it may need to wait for a space to open up in the command queue.
      // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
      // Serial_Puts(SERIAL_PORT, "M112\n");
      MarlinAPI_EMStop();
      break;

    case KEY_ICON_1:
      storeCmd("M81\n");
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default :
      break;
  }
}

void menuConnectionSettings(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  const MENUITEMS connectionSettingsItems = {
    // title
    LABEL_CONNECTION_SETTINGS,
    // icon                         label
    // {{ICON_BAUD_RATE,               LABEL_BAUDRATE},
    //  {ICON_DISCONNECT,              LABEL_DISCONNECT},
    {{ICON_STOP,                    LABEL_EMERGENCYSTOP},
     {ICON_SHUT_DOWN,               LABEL_SHUT_DOWN},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  menuDrawPage(&connectionSettingsItems);
  menuSetFrontCallBack(menuConnectionSettingsCallBack);
}