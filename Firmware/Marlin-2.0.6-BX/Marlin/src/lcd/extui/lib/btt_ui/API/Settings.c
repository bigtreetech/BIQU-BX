#include "Settings.h"
#include "../includes.h"

#include "../../../../../inc/MarlinConfigPre.h"

SETTINGS infoSettings;
MACHINESETTINGS infoMachineSettings;

const uint16_t default_max_temp[]      = HEAT_MAX_TEMP;
const uint16_t default_max_fanPWM[]    = FAN_MAX_PWM;
const uint16_t default_size_min[]      = {X_MIN_POS,Y_MIN_POS,Z_MIN_POS};
const uint16_t default_size_max[]      = {X_MAX_POS,Y_MAX_POS,Z_MAX_POS};
const uint16_t default_move_speed[]    = {SPEED_MOVE_SLOW, DEFAULT_SPEED_MOVE, SPEED_MOVE_FAST};
const uint16_t default_ext_speed[]     = {EXTRUDE_SLOW_SPEED, EXTRUDE_NORMAL_SPEED, EXTRUDE_FAST_SPEED};
const uint16_t default_level_speed[]   = {LEVELING_POINT_XY_FEEDRATE,LEVELING_POINT_XY_FEEDRATE,LEVELING_POINT_Z_FEEDRATE};
const uint16_t default_pause_speed[]   = {NOZZLE_PAUSE_XY_FEEDRATE, NOZZLE_PAUSE_XY_FEEDRATE, NOZZLE_PAUSE_Z_FEEDRATE, NOZZLE_PAUSE_E_FEEDRATE};
const uint16_t default_preheat_ext[]   = PREHEAT_HOTEND;
const uint16_t default_preheat_bed[]   = PREHEAT_BED;
const uint8_t default_custom_enabled[] = CUSTOM_GCODE_ENABLED;

// Reset settings data
void infoSettingsReset(void)
{
  infoSettings.baudrate             = BAUDRATE;
  infoSettings.language             = DEFAULT_LANGUAGE;
  infoSettings.mode                 = DEFAULT_LCD_MODE;
  infoSettings.unified_menu         = UNIFIED_MENU;

  infoSettings.bg_color             = lcd_colors[BACKGROUND_COLOR];
  infoSettings.font_color           = lcd_colors[FONT_COLOR];
  infoSettings.title_bg_color       = lcd_colors[TITLE_BACKGROUND_COLOR];
  infoSettings.reminder_color       = lcd_colors[REMINDER_FONT_COLOR];
  infoSettings.sd_reminder_color    = lcd_colors[VOLUME_REMINDER_FONT_COLOR];
  infoSettings.status_xyz_bg_color  = lcd_colors[STATUS_XYZ_BG_COLOR];
  infoSettings.list_border_color    = lcd_colors[LISTVIEW_BORDER_COLOR];
  infoSettings.list_button_color    = lcd_colors[LISTVIEW_ICON_COLOR];

  infoSettings.touchSound           = _ENABLED;
  infoSettings.toastSound           = _ENABLED;
  infoSettings.alertSound           = _ENABLED;

  infoSettings.terminalACK          = _DISABLED;
  infoSettings.move_speed           = _ENABLED;
  infoSettings.knob_led_color       = STARTUP_KNOB_LED_COLOR;
  infoSettings.knob_led_idle        = _ENABLED;
  infoSettings.send_start_gcode     = _DISABLED;
  infoSettings.send_end_gcode       = _DISABLED;
  infoSettings.send_cancel_gcode    = _ENABLED;
  infoSettings.persistent_info      = _ENABLED;
  infoSettings.file_listmode        = _ENABLED;
  infoSettings.ack_notification     = ACK_NOTIFICATION_STYLE;

  infoSettings.lcd_brightness       = DEFAULT_LCD_BRIGHTNESS;
  infoSettings.lcd_idle_brightness  = DEFAULT_LCD_IDLE_BRIGHTNESS;
  infoSettings.lcd_idle_timer       = DEFAULT_LCD_IDLE_TIMER;

  infoSettings.serial_alwaysOn        = SERIAL_ALWAYS_ON;
  infoSettings.marlin_mode_bg_color   = lcd_colors[MARLIN_BKCOLOR];
  infoSettings.marlin_mode_font_color = lcd_colors[MARLIN_FNCOLOR];
  infoSettings.marlin_mode_showtitle  = MARLIN_SHOW_BANNER;
  infoSettings.marlin_mode_fullscreen = DEFAULT_ST7920_FULLSCREEN_MODE;
  infoSettings.marlin_type            = LCD12864;

  infoSettings.auto_off               = _DISABLED;
  infoSettings.ps_active_high         = PS_ON_ACTIVE_HIGH;
  infoSettings.auto_off_temp          = AUTO_SHUT_DOWN_MAXTEMP;

  infoSettings.runout                 = _DISABLED;
  infoSettings.runout_invert          = FIL_RUNOUT_INVERTING;
  infoSettings.runout_noise_ms        = FIL_NOISE_THRESHOLD;
  infoSettings.runout_distance        = FILAMENT_RUNOUT_DISTANCE_MM;

  infoSettings.powerloss_en           = _ENABLED;
  infoSettings.powerloss_home         = HOME_BEFORE_PLR;
  infoSettings.powerloss_invert       = PS_ON_ACTIVE_HIGH;
  infoSettings.powerloss_z_raise      = POWER_LOSS_ZRAISE;
  infoSettings.btt_ups                = BTT_MINI_UPS;

//machine specific settings

  infoSettings.hotend_count           = HOTEND_NUM;
  infoSettings.bed_en                 = _ENABLED;
  infoSettings.chamber_en             = _DISABLED;
  infoSettings.ext_count              = EXTRUDER_NUM;
  infoSettings.fan_count              = FAN_NUM;
  infoSettings.fan_ctrl_count         = FAN_CTRL_NUM;
  infoSettings.auto_load_leveling     = AUTO_SAVE_LOAD_LEVELING_VALUE;
  infoSettings.onboardSD              = _AUTO;     //ENABLED / DISABLED / AUTO
  infoSettings.m27_refresh_time       = M27_REFRESH;
  infoSettings.m27_active             = M27_WATCH_OTHER_SOURCES;
  infoSettings.longFileName           = _AUTO;     //ENABLED / DISABLED / AUTO

  for(int i = 0; i < MAX_HEATER_COUNT; i++)
  {
    infoSettings.max_temp[i]          = default_max_temp[i];
  }

  infoSettings.min_ext_temp           = PREVENT_COLD_EXTRUSION_MINTEMP;

  for(int i = 0; i < MAX_FAN_COUNT ;i++)
  {
    infoSettings.fan_max[i]           = default_max_fanPWM[i];
  }

  infoSettings.fan_percentage         = _ENABLED;

  for(int i = 0; i < AXIS_NUM ;i++) //x, y, z
  {
    infoSettings.invert_axis[i]       = _DISABLED;
    infoSettings.machine_size_min[i]  = default_size_min[i];
    infoSettings.machine_size_max[i]  = default_size_max[i];
    infoSettings.level_feedrate[i]    = default_level_speed[i];
  }

  for(int i = 0; i < SPEED_COUNT ;i++)
  {
    infoSettings.axis_speed[i]        = default_move_speed[i];
    infoSettings.ext_speed[i]         = default_ext_speed[i];
  }

  infoSettings.pause_retract_len      = NOZZLE_PAUSE_RETRACT_LENGTH;
  infoSettings.resume_purge_len       = NOZZLE_RESUME_PURGE_LENGTH;


  infoSettings.pause_pos[TSC_X_AXIS]  = NOZZLE_PAUSE_X_POSITION;  // X
  infoSettings.pause_pos[TSC_Y_AXIS]  = NOZZLE_PAUSE_Y_POSITION;  // Y
  infoSettings.pause_z_raise          = NOZZLE_PAUSE_Z_RAISE;

  for(int i = 0; i < TOTAL_AXIS ;i++)
  {
    infoSettings.pause_feedrate[i]    = default_pause_speed[i]; // X, Y, Z, E
  }

  infoSettings.level_edge             = LEVELING_EDGE_DISTANCE;
  infoSettings.level_z_pos            = LEVELING_POINT_Z;
  infoSettings.level_z_raise          = LEVELING_POINT_MOVE_Z;

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    infoSettings.preheat_temp[i]      = default_preheat_ext[i];
    infoSettings.preheat_bed[i]       = default_preheat_bed[i];
  }
  resetConfig();
}

void initMachineSetting(void){
  // some settings are assumes as active unless reported disabled by marlin
  infoMachineSettings.EEPROM                  = _ENABLED;
  infoMachineSettings.autoReportTemp          = _DISABLED;
  infoMachineSettings.autoLevel               = _ENABLED;
  infoMachineSettings.zProbe                  = _ENABLED;
  infoMachineSettings.levelingData            = _ENABLED;
  infoMachineSettings.softwarePower           = _DISABLED;
  infoMachineSettings.toggleLights            = _DISABLED;
  infoMachineSettings.caseLightsBrightness    = _DISABLED;
  infoMachineSettings.emergencyParser         = _DISABLED;
  infoMachineSettings.promptSupport           = _DISABLED;
  infoMachineSettings.onboard_sd_support      = _ENABLED;
  infoMachineSettings.autoReportSDStatus      = _DISABLED;
  infoMachineSettings.enableubl               = _DISABLED;
}

void setupMachine(void)
{
  #ifdef ENABLE_UBL_VALUE
    if (infoMachineSettings.autoLevel == 1 && ENABLE_UBL_VALUE == 1) {
      infoMachineSettings.enableubl = _ENABLED;
    }
  #endif
  #ifdef AUTO_SAVE_LOAD_LEVELING_VALUE
    if (infoMachineSettings.autoLevel == 1 && infoMachineSettings.EEPROM == 1 && infoSettings.auto_load_leveling == 1){
      storeCmd("M420 S1\n");
    }
  #endif
  if(infoMachineSettings.isMarlinFirmware != 1) //Smoothieware does not report detailed M115 capabilities
  {
    infoMachineSettings.EEPROM                  = _ENABLED;
    infoMachineSettings.autoReportTemp          = _DISABLED;
    infoMachineSettings.autoLevel               = _ENABLED;
    infoMachineSettings.zProbe                  = _ENABLED;
    infoMachineSettings.levelingData            = _ENABLED;
    infoMachineSettings.emergencyParser         = _ENABLED;
    infoMachineSettings.autoReportSDStatus      = _DISABLED;
  }
  if (infoSettings.onboardSD == _ENABLED)
  {
    infoMachineSettings.onboard_sd_support = _ENABLED;
  }
  else if (infoSettings.onboardSD == _DISABLED)
  {
    infoMachineSettings.onboard_sd_support = _DISABLED;
  }

  if (infoSettings.longFileName == _ENABLED)
  {
    infoMachineSettings.long_filename_support = _ENABLED;
  }
  else if (infoSettings.longFileName == _DISABLED)
  {
    infoMachineSettings.long_filename_support = _DISABLED;
  }
  // mustStoreCmd("M503 S0\n");
}


void marlinSetupMachine(void)
{
  infoMachineSettings.isMarlinFirmware = 1;
  infoMachineSettings.EEPROM = ENABLED(EEPROM_SETTINGS);
  infoMachineSettings.autoReportTemp = ENABLED(AUTO_REPORT_TEMPERATURES);
  infoMachineSettings.autoLevel = ENABLED(HAS_AUTOLEVEL);
  infoMachineSettings.zProbe =_ENABLED;
  infoMachineSettings.levelingData = ENABLED(HAS_LEVELING);
  infoMachineSettings.softwarePower = ENABLED(PSU_CONTROL);
  infoMachineSettings.toggleLights = ENABLED(CASE_LIGHT_ENABLE);
  infoMachineSettings.caseLightsBrightness = TERN0(CASE_LIGHT_ENABLE, PWM_PIN(CASE_LIGHT_PIN));
  infoMachineSettings.promptSupport = ENABLED(HOST_PROMPT_SUPPORT);
  infoMachineSettings.autoReportSDStatus = ENABLED(AUTO_REPORT_SD_STATUS);
  infoMachineSettings.emergencyParser = ENABLED(EMERGENCY_PARSER);
  infoMachineSettings.long_filename_support = ENABLED(LONG_FILENAME_HOST_SUPPORT);
  infoMachineSettings.onboard_sd_support = ENABLED(SDSUPPORT);

  setupMachine();
}

float flashUsedPercentage(void)
{
  uint32_t total = W25Qxx_ReadCapacity();
  float percent = ((float)FLASH_USED * 100) / total;
  return percent;
}
