#include "Fan.h"
#include "../includes.h"

static uint8_t curIndex = 0;

// 1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS fanItems = {
// title
LABEL_FAN,
// icon                       label
  {{ICON_DEC,                 LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_FAN ,                 LABEL_FAN},
  {ICON_FAN_HALF_SPEED,       LABEL_FAN_HALF_SPEED},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_BACK,                 LABEL_BACK},}
};

const ITEM itemFan[3] = {
  //icon                label
  {ICON_FAN,            LABEL_FAN},
  {ICON_FAN_HALF_SPEED, LABEL_FAN_HALF_SPEED},
  {ICON_FAN_FULL_SPEED, LABEL_FAN_FULL_SPEED}
};

void fanSpeedReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", fanID[curIndex]);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);
  }

  if(infoSettings.fan_percentage == 1)
    sprintf(tempstr, "  %d%%  ", fanGetSpeedPercent(curIndex));
  else
    sprintf(tempstr, "  %d  ", (int)fanGetSpeed(curIndex));

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

static uint8_t lastFan;
static bool Fan_Half_or_Full = 0;
void menuFanCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  uint8_t actFan = fanGetSpeedPercent(curIndex);
  uint8_t actFanPercent = fanGetSpeedPercent(curIndex);
  
  #ifdef SUTOADD
  KEY_VALUES key_num_P=menuKeyPressValue();
  switch (key_num_P)
    {
      case  KEY_ICON_0:
      if (actFan > 0)
        {
          if (infoSettings.fan_percentage == 1)
            fanSetSpeedPercent(curIndex, --actFanPercent);
          else
            fanSetSpeed(curIndex, --actFan);
        }
        break;
      case  KEY_ICON_3:
      if (actFan < infoSettings.fan_max[curIndex])
        {
            if (infoSettings.fan_percentage == 1)
              fanSetSpeedPercent(curIndex, ++actFanPercent);
            else
              fanSetSpeed(curIndex, ++actFan);
        }
        break;
      default:
        break;
    }
  #endif

  switch (key_num)
  {
    #ifndef SUTOADD
    case  KEY_ICON_0:
      if (actFan > 0)
        {
          if (infoSettings.fan_percentage == 1)
            fanSetSpeedPercent(curIndex, --actFanPercent);
          else
            fanSetSpeed(curIndex, --actFan);
        }
      break;

    case  KEY_ICON_3:
      if (actFan < infoSettings.fan_max[curIndex])
        {
            if (infoSettings.fan_percentage == 1)
              fanSetSpeedPercent(curIndex, ++actFanPercent);
            else
              fanSetSpeed(curIndex, ++actFan);
        }
      break;
    #endif
    case KEY_ICON_4:
      if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
      {
        curIndex = (curIndex + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
        fanSpeedReDraw(false);
      }
      else
      {
        actFan = 100;
        fanSetSpeedPercent(curIndex, actFan);
      }
      break;

    case KEY_ICON_5:
      if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
      {
        if(Fan_Half_or_Full) fanSetSpeedPercent(curIndex, 100);   //switch half or full speed
        else fanSetSpeedPercent(curIndex, 50);
        Fan_Half_or_Full = (Fan_Half_or_Full + 1) % 2;
        fanItems.items[key_num] = itemFan[Fan_Half_or_Full+1];
        menuDrawItem(&fanItems.items[key_num], key_num);
      }
      else
      {
        actFan = 50;
        fanSetSpeedPercent(curIndex, actFan);
      }
      break;

    case KEY_ICON_6:
      fanSetSpeedPercent(curIndex, 0);
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:
    #if LCD_ENCODER_SUPPORT
      if(encoderPosition)
      {
        if (actFan < infoSettings.fan_max[curIndex] && encoderPosition > 0)
        {
          if (infoSettings.fan_percentage ==  1)
          {
            fanSetSpeedPercent(curIndex, ++actFanPercent);
          }
          else
          {
            fanSetSpeed(curIndex, ++actFan);
          }
        }

        if (actFan > 0 && encoderPosition < 0) {
          if (infoSettings.fan_percentage == 1)
          {
            fanSetSpeedPercent(curIndex, --actFanPercent);
          }
          else
          {
            fanSetSpeed(curIndex, --actFan);
          }
        }
        encoderPosition = 0;
      }
    #endif
    break;
  }
  loopFrontEnd();
  if (lastFan != fanGetSpeedPercent(curIndex))
  {
    lastFan = fanGetSpeedPercent(curIndex);
    fanSpeedReDraw(true);
  }
}

void menuFan(void)
{
  lastFan = fanGetSpeed(curIndex);
  if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    fanItems.items[KEY_ICON_4] = itemFan[0];
  else
  {
    fanItems.items[KEY_ICON_4] = itemFan[2];
    fanItems.items[KEY_ICON_5] = itemFan[1];
  }
  menuDrawPage(&fanItems);
  fanSpeedReDraw(false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  menuSetFrontCallBack(menuFanCallBack);
}