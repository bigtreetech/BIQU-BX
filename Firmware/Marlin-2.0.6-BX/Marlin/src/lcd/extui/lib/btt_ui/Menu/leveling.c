#include "leveling.h"
#include "../includes.h"
static MENUITEMS autoLevelingItems = {
    // title
    LABEL_ABL,
    // icon                label
    {{ICON_LEVELING,       LABEL_START},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_Z_FADE,          LABEL_ABL_Z},
    {ICON_PROBE_OFFSET,    LABEL_Z_OFFSET},
    {ICON_BLTOUCH,         LABEL_BLTOUCH},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACK,            LABEL_BACK}}
  };
static bool leveled = false;

void menuWaitHeatBedCallBack(void)
{
  loopFrontEnd();
  if (heatGetCurrentTemp(TSC_BED) >= 49)
  {
    storeCmd("G28\n");
    if (infoMachineSettings.enableubl == 1)
    {
      storeCmd("G29 P1\n");
      storeCmd("G29 P3\n");
      storeCmd("M118 A1 UBL Complete\n");
    }
    else
    {
      storeCmd("G29\n");
      leveled = true;
    }
    infoMenu.cur--;
  }
}
void menuWaitHeatBed(void)
{
  static MENUITEMS waitHeatBedItems = {
    // title
    .title.address = (uint8_t *)"Heating hotbed",
    // icon                label
    {{ICON_BACKGROUND,     LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND},
    {ICON_BACKGROUND,      LABEL_BACKGROUND}}
  };
  menuDrawPage(&waitHeatBedItems);
  GUI_DispStringInRect(0, 0, BTT_LCD_WIDTH, BTT_LCD_HEIGHT, (uint8_t *)"Please wait for the hotbed temperature to rise to 50℃ for auto bed leveling");
  menuSetFrontCallBack(menuWaitHeatBedCallBack);
  
  heatSetTargetTemp(TSC_BED, 50);
}
void gotoWaitHeatingBed(void)
{
  infoMenu.menu[++infoMenu.cur] = menuWaitHeatBed;
}

void menuAutoLevelingCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
    showDialog(DIALOG_TYPE_ALERT,  (uint8_t *)"Heating hotbed", (uint8_t *)"It is necessary to heat the hotbed to 50℃ before auto leveling. Confirm heating?",
                textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), gotoWaitHeatingBed, NULL, NULL);
      
      break;

    case KEY_ICON_1:
      if (infoMachineSettings.enableubl == 1)
        ublSaveLoad(true);
      break;

    case KEY_ICON_2:
      if (infoMachineSettings.enableubl == 1)
        ublSaveLoad(false);
      break;

    case KEY_ICON_3:
      {
        char tempstr[30];
        sprintf(tempstr, "Min:%.2f | Max:%.2f", Z_FADE_MIN_VALUE, Z_FADE_MAX_VALUE);
        float val = numPadFloat((uint8_t *)tempstr, getParameter(P_ABL_STATE, 1), 0.0f, false);
        storeCmd("M420 Z%.2f\n", NOBEYOND(Z_FADE_MIN_VALUE, val, Z_FADE_MAX_VALUE));
        menuDrawPage(&autoLevelingItems);
      }
      break;

    case KEY_ICON_4:
      storeCmd("M851\n");
      infoMenu.menu[++infoMenu.cur] = menuProbeOffset;
      break;

    case KEY_ICON_5:
      infoMenu.menu[++infoMenu.cur] = menuBLTouch;
      break;

    case KEY_ICON_7:
      if (leveled == true && infoMachineSettings.EEPROM == 1)
      {
        leveled = false;
        showDialog(DIALOG_TYPE_QUESTION, textSelect(autoLevelingItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
      }
      else
      {
        infoMenu.cur--;
      }
      break;

    default:
      break;
  }
  loopFrontEnd();
}
void menuAutoLeveling(void)
{
  if (infoMachineSettings.enableubl == 1)
  {
    autoLevelingItems.title.index = LABEL_ABL_SETTINGS_UBL;
    autoLevelingItems.items[1].icon = ICON_S_SAVE;
    autoLevelingItems.items[1].label.index = LABEL_SAVE;
    autoLevelingItems.items[2].icon = ICON_S_LOAD;
    autoLevelingItems.items[2].label.index = LABEL_LOAD;
  }

  menuDrawPage(&autoLevelingItems);
  menuSetFrontCallBack(menuAutoLevelingCallBack);
}

// 1 title, ITEM_PER_PAGE items (icon + label)
const MENUITEMS manualLevelingItems = {
  // title
  LABEL_LEVELING,
  // icon                         label
  {{ICON_POINT_1,                 LABEL_POINT_1},
   {ICON_POINT_2,                 LABEL_POINT_2},
   {ICON_POINT_3,                 LABEL_POINT_3},
   {ICON_POINT_4,                 LABEL_POINT_4},
   {ICON_POINT_5,                 LABEL_POINT_5},
   {ICON_LEVEL_EDGE_DISTANCE,     LABEL_DISTANCE},
   {ICON_DISABLE_STEPPERS,        LABEL_XY_UNLOCK},
   {ICON_BACK,                    LABEL_BACK},}
};

void moveToLevelingPoint(uint8_t point)
{
  int16_t pointPosition[5][2] = {
    {infoSettings.machine_size_min[TSC_X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[TSC_Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[TSC_X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[TSC_Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[TSC_X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[TSC_Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[TSC_X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[TSC_Y_AXIS] - infoSettings.level_edge},
    {(infoSettings.machine_size_min[TSC_X_AXIS] + infoSettings.machine_size_max[TSC_X_AXIS]) / 2, (infoSettings.machine_size_min[TSC_Y_AXIS] + infoSettings.machine_size_max[TSC_Y_AXIS]) / 2},
  };

  if(coordinateIsKnown() == false)
  {
    storeCmd("G28\n");
  }

  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[TSC_Z_AXIS]);
  storeCmd("G0 X%d Y%d F%d\n", pointPosition[point][0], pointPosition[point][1], infoSettings.level_feedrate[TSC_X_AXIS]);
  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_pos, infoSettings.level_feedrate[TSC_Z_AXIS]);
}

void menuManualLevelingCallBack(void)
{
  KEY_VALUES key_num = menuKeyGetValue();
  switch(key_num)
  {
    case KEY_ICON_0:
      moveToLevelingPoint(0);
      break;

    case KEY_ICON_1:
      moveToLevelingPoint(1);
      break;

    case KEY_ICON_2:
      moveToLevelingPoint(2);
      break;

    case KEY_ICON_3:
      moveToLevelingPoint(3);
      break;

    case KEY_ICON_4:
      moveToLevelingPoint(4);
      break;

    case KEY_ICON_5:
      {
        char tempstr[30];
        sprintf(tempstr, "%Min:%d | Max:%d", LEVELING_EDGE_DISTANCE_MIN, LEVELING_EDGE_DISTANCE_MAX);
        int val = numPadInt((uint8_t *)tempstr, infoSettings.level_edge, LEVELING_EDGE_DISTANCE_DEFAULT, false);
        infoSettings.level_edge = NOBEYOND(LEVELING_EDGE_DISTANCE_MIN, val, LEVELING_EDGE_DISTANCE_MAX);
        menuDrawPage(&manualLevelingItems);
        menuSetFrontCallBack(menuManualLevelingCallBack);
      }
      break;

    case KEY_ICON_6:
      storeCmd("M84 X Y E\n");
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:
      break;
  }
  loopFrontEnd();
}

void menuManualLeveling(void)
{
  menuDrawPage(&manualLevelingItems);
  menuSetFrontCallBack(menuManualLevelingCallBack);
}