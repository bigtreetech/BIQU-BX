#include "../../ui_api.h"
#include "../../../../gcode/queue.h"
#include "includes.h"


/******************************************************************** Heater ***********************************************************************/

//Set target temperature
void MarnlinAPI_SetTargetTemp(uint8_t index, int16_t temp)
{
  ExtUI::setTargetTemp_celsius((float)temp, (ExtUI::heater_t)index);
}

//Get target temperature
uint16_t MarnlinAPI_GetTargetTemp(uint8_t index)
{
  return ExtUI::getTargetTemp_celsius((ExtUI::heater_t)index);
}

// Get current temperature
int16_t MarnlinAPI_GetCurrentTemp(uint8_t index)
{
  return ExtUI::getActualTemp_celsius((ExtUI::heater_t)index);
}

// Get current Tool (Extruder)
uint8_t MarnlinAPI_GetCurrentTool(void)
{
  return ExtUI::getActiveTool();
}

// Get current hotend index in arry T[]
uint8_t MarnlinAPI_GetCurrentHotend(void)
{
  return ExtUI::getActiveTool();
}


/******************************************************************** Printing ***********************************************************************/

bool MarlinAPI_StartPrint(char *filename)
{
  ExtUI::printFile(filename);
  return true;
}

void MarlinAPI_AbortPrinting(void)
{
  ExtUI::stopPrint();
}

bool MarlinAPI_IsReadingMedia(void)
{
  return ExtUI::isPrintingFromMedia();
}

bool MarlinAPI_IsPaused(void)
{
  return ExtUI::isPrintingFromMediaPaused();
}

uint32_t MarlinAPI_PrintProgress(void)
{
  return ExtUI::getProgress_percent();
}

uint32_t MarlinAPI_PrintTime(void)
{
  return ExtUI::getProgress_seconds_elapsed();
}

void MarlinAPI_PausePrint(void)
{
  return ExtUI::pausePrint();
}

void MarlinAPI_ResumePrint(void)
{
  return ExtUI::resumePrint();
}


/******************************************************************** Gcode **********************************************************************/

bool MarlinAPI_HasInjectCommands(void)
{
  return (queue.injected_commands[0] != 0);
}

void MarlinAPI_InjectCommand(char * const gcode)
{
  queue.inject(gcode);
}


/******************************************************************** Fan ***********************************************************************/

uint8_t MarlinAPI_GetFanPercent(uint8_t index)
{
  return ExtUI::getTargetFan_percent((ExtUI::fan_t)index);
}

void MarlinAPI_SetFanPercent(uint8_t index, uint8_t percent)
{
  return ExtUI::setTargetFan_percent((float)percent, (ExtUI::fan_t)index);
}


/******************************************************************** Speed ***********************************************************************/

uint16_t MarlinAPI_GetMoveSpeedPercent(void)
{
  return ExtUI::getFeedrate_percent();
}

void MarlinAPI_SetMoveSpeedPercent(uint16_t percent)
{
  return ExtUI::setFeedrate_percent((float)percent);
}

uint16_t MarlinAPI_GetFlowPercent(void)
{
  return ExtUI::getFlowPercentage(ExtUI::E0);
}

void MarlinAPI_SetFlowPercent(uint16_t percent)
{
  return ExtUI::setFlow_percent((int16_t)percent, ExtUI::E0);
}


/******************************************************************** Coordinate ***********************************************************************/

float MarlinAPI_GetMoveSpeedMM_S(void)
{
  //return ExtUI::getFeedrate_mm_s();
  return 0;
}

float MarlinAPI_GetAxisPosition(AXIS axis)
{
  switch (axis)
  {
  case TSC_X_AXIS:
  case TSC_Y_AXIS:
  case TSC_Z_AXIS:
    return ExtUI::getAxisPosition_mm((ExtUI::axis_t)axis);
    
  case TSC_E_AXIS:
    return ExtUI::getAxisPosition_mm((ExtUI::extruder_t)ExtUI::E0);
  
  default:
    return 0;
  }
}

bool MarlinAPI_isPositionKnown(void)
{
  return ExtUI::isPositionKnown();
}


/******************************************************************** Leveling ***********************************************************************/

bool MarlinAPI_LevelingActived(void)
{
  return ExtUI::getLevelingActive();
}


/******************************************************************** BabyStep ***********************************************************************/

#include "../../feature/babystep.h"
void MarlinAPI_BabyStep(float distance)
{
  babystep.add_mm(Z_AXIS, distance);
}

float MarlinAPI_BabyStepTotal(void)
{
  return (babystep.axis_total[BS_AXIS_IND(Z_AXIS)] / ExtUI::getAxisSteps_per_mm(ExtUI::Z));
}


/******************************************************************** Marlin mode ***********************************************************************/

#include "../../../ultralcd.h"
void MarlinAPI_LCDInit(void)
{
  ui.marlin_init();
}

void MarlinAPI_LCDUpdate(void)
{
  ui.marlin_update();
}

void MarlinAPI_LCDKill(PGM_P lcd_error, PGM_P lcd_component)
{
  ui.marlin_kill_screen(lcd_error, lcd_component);
}


const uint16_t encPin[]  = {LCD_ENCA_PIN,  LCD_ENCB_PIN,  LCD_BTN_PIN};
void MarlinAPI_EncoderInit(void)
{
  for(uint8_t i = 0; i < COUNT(encPin); i++)
  {
    SET_INPUT_PULLUP(encPin[i]);
  }
}

bool MarlinAPI_Encoder_ReadStep(uint8_t io_index)
{
  return !READ(encPin[io_index]);
}

void MarlinAPI_RPI_PowerInit(void)
{
  OUT_WRITE(RPI_POWER_PIN, LOW);
}

void MarlinAPI_RPI_Set(bool enable)
{
  WRITE(RPI_POWER_PIN, enable);
}

/******************************************************************** Neopixel ***********************************************************************/

#include "../../feature/leds/leds.h"
void MarlinAPI_WS2812_Send_DAT(uint32_t ws2812_dat)
{
  const LEDColor color = MakeLEDColor(
    (ws2812_dat >> 8) & 0xFF,  // R
    (ws2812_dat >> 16) & 0xFF, // B
    (ws2812_dat) & 0xFF,       // G
    0,
    255
  );

  leds2.set_color(color);
}


/******************************************************************** Buzzer ***********************************************************************/

void MarlinAPI_Buzzer(const uint32_t frequency, const uint32_t duration)
{
  // Limits the tone duration to 0-5 seconds.
  //NOMORE(duration, 5000U);
#ifdef BTT_SKR_SE_BX_WITHBOOT
  BUZZ(duration, frequency);
#endif
}


/******************************************************************** Power loss recovery ***********************************************************************/

#include "../../../../feature/powerloss.h"
bool MarlinAPI_PowerFailedExist(void)
{
  if (card.jobRecoverFileExists()) {
    recovery.check();
    strncpy(infoFile.title, recovery.info.sd_filename, MAX_PATH_LEN);
    return true;
  }
  return false;
}

void MarlinAPI_PowerFailedResume(void)
{
  recovery.resume();
}

void MarlinAPI_PowerFailedDelete(void)
{
  recovery.cancel();
}


/******************************************************************** Parameter ***********************************************************************/

#include "../../module/planner.h"
float MarlinApi_GetParameter(PARAMETER_NAME name, uint8_t index)
{
  //if(index >= parameter_element_count[name]) return 0.0f;
  switch (name)
  {
  case P_STEPS_PER_MM:
    return ExtUI::getAxisSteps_per_mm((ExtUI::axis_t)index);

  case P_CURRENT:
    return (index <= Z_STEPPER) ? ExtUI::getAxisCurrent_mA((ExtUI::axis_t)index) : ExtUI::getAxisCurrent_mA((ExtUI::extruder_t)(index - E_STEPPER));

  case P_MAX_FEED_RATE:
    return ExtUI::getAxisMaxFeedrate_mm_s((ExtUI::axis_t)index);

  case P_MAX_ACCELERATION:
    return ExtUI::getAxisMaxAcceleration_mm_s2((ExtUI::axis_t)index);

  case P_ACCELERATION:
    switch (index)
    {
    case 0: // Printing
      return ExtUI::getPrintingAcceleration_mm_s2();
    case 1: // Retract
      return ExtUI::getRetractAcceleration_mm_s2();
    case 2: // Travel
      return ExtUI::getTravelAcceleration_mm_s2();

    default:
      return 0;
    }

  case P_JERK:
    // #if HAS_CLASSIC_JERK
    //   #error "not code here" // MSQTODO
    // #else
      return 0;
    // #endif

  case P_PROBE_OFFSET:
    return ExtUI::getProbeOffset_mm((ExtUI::axis_t)index);

  case P_BUMPSENSITIVITY:
    return ExtUI::getTMCBumpSensitivity((ExtUI::axis_t)index);

  case P_FWRETRACT:
    // #if ENABLED(FWRETRACT)
    //   #error "not code here" // MSQTODO
    // #else
      return 0;
    // #endif

  case P_FWRECOVER:
    // #if ENABLED(FWRETRACT)
    //   #error "not code here" // MSQTODO
    // #else
      return 0;
    // #endif

  case P_AUTO_RETRACT:
    // #if ENABLED(FWRETRACT)
    //   #error "not code here" // MSQTODO
    // #else
      return 0;
    // #endif

  case P_LIN_ADV:
    #if ENABLED(LIN_ADVANCE)
      return ExtUI::getLinearAdvance_mm_mm_s((ExtUI::extruder_t)index);
    #else
      return 0;
    #endif

  case P_ABL_STATE:
    switch (index)
    {
    case 0: // M420 S
      return ExtUI::getLevelingActive();;
    case 1: // M420 Z
      return planner.z_fade_height;
    default:
      return 0;
    }

  case P_OFFSET_TOOL:
    // #if ENABLED(HAS_HOTEND_OFFSET)
    //   #error "not code here" // MSQTODO
    // #else
      return 0;
    // #endif

  case P_HYBRID_THRESHOLD:
    // #if ENABLED(HYBRID_THRESHOLD)
    //   #error "not code here" // MSQTODO
    // #else
      return 0;
    // #endif

  default:
    return 0.0f;
  }
}

void MarlinApi_SetParameter(PARAMETER_NAME name, uint8_t index, float val)
{
  //if(index >= parameter_element_count[name]) return 0.0f;
  switch (name)
  {
  // case P_STEPS_PER_MM:
  //   return ExtUI::getAxisSteps_per_mm((ExtUI::axis_t)index);

  // case P_CURRENT:
  //   return (index <= Z_STEPPER) ? ExtUI::getAxisCurrent_mA((ExtUI::axis_t)index) : ExtUI::getAxisCurrent_mA((ExtUI::extruder_t)(index - E_STEPPER));

  // case P_MAX_FEED_RATE:
  //   return ExtUI::getAxisMaxFeedrate_mm_s((ExtUI::axis_t)index);

  // case P_MAX_ACCELERATION:
  //   return ExtUI::getAxisMaxAcceleration_mm_s2((ExtUI::axis_t)index);

  // case P_ACCELERATION:
  //   switch (index)
  //   {
  //   case 0: // Printing
  //     return ExtUI::getPrintingAcceleration_mm_s2();
  //   case 1: // Retract
  //     return ExtUI::getRetractAcceleration_mm_s2();
  //   case 2: // Travel
  //     return ExtUI::getTravelAcceleration_mm_s2();

  //   default:
  //     return 0;
  //   }

  // case P_JERK:
  //   // #if HAS_CLASSIC_JERK
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  case P_PROBE_OFFSET:
    ExtUI::setProbeOffset_mm(val, (ExtUI::axis_t)index);
    break;

  // case P_BUMPSENSITIVITY:
  //   return ExtUI::getTMCBumpSensitivity((ExtUI::axis_t)index);

  // case P_FWRETRACT:
  //   // #if ENABLED(FWRETRACT)
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  // case P_FWRECOVER:
  //   // #if ENABLED(FWRETRACT)
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  // case P_AUTO_RETRACT:
  //   // #if ENABLED(FWRETRACT)
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  // case P_LIN_ADV:
  //   // #if ENABLED(LIN_ADVANCE)
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  // case P_ABL_STATE:
  //   switch (index)
  //   {
  //   case 0: // M420 S
  //     return ExtUI::getLevelingActive();;
  //   case 1: // M420 Z
  //     return planner.z_fade_height;
  //   default:
  //     return 0;
  //   }

  // case P_OFFSET_TOOL:
  //   // #if ENABLED(HAS_HOTEND_OFFSET)
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  // case P_HYBRID_THRESHOLD:
  //   // #if ENABLED(HYBRID_THRESHOLD)
  //   //   #error "not code here" // MSQTODO
  //   // #else
  //     return 0;
  //   // #endif

  default:
    break;
  }
}


/******************************************************************** Host Action ***********************************************************************/

void MarlinAPI_breakAndContinue(void)
{
  wait_for_user = wait_for_heatup = false;
}

void MarlinAPI_resumeAndPurge(void)
{
  #if ENABLED(HOST_PROMPT_SUPPORT)
    host_response_handler(0);
  #endif
}

void MarlinAPI_resumeAndContinue(void)
{
  #if ENABLED(HOST_PROMPT_SUPPORT)
    host_response_handler(1);
  #endif
}


/******************************************************************** MISC ***********************************************************************/

uint32_t MarlinAPI_GetMS(void) {
  return millis();
}

void MarlinAPI_SetPWM(uint8_t duty) {
  analogWrite(LCD_LED_PIN, duty * 255 / 100);
  // PinName p = digitalPinToPinName(LCD_LED_PIN);
  // if (pin_in_pinmap(p, PinMap_PWM)) {
  //   if (is_pin_configured(p, g_anOutputPinConfigured) == false) {
  //     set_pin_configured(p, g_anOutputPinConfigured);
  //   }
  //   uint32_t ulValue = mapResolution(duty * 255 / 100, _writeResolution, PWM_RESOLUTION);
  //   pwm_start(p, 500, ulValue);
  // }
}

void MarlinAPI_EMStop(void)
{
  #if ENABLED(EMERGENCY_PARSER)
    emergency_parser.killed_by_M112 = true;
  #else
    MarlinAPI_InjectCommand("M112\n");
  #endif
}

#include "../../../../MarlinCore.h"
#include "../../../../module/endstops.h"
#include "../../../../sd/cardreader.h"
void abortSDPrinting();
void finishSDPrinting();
void MarlinAPI_MainLoop(void)
{
  idle();

  #if ENABLED(SDSUPPORT)
    card.checkautostart();
    if (card.flag.abort_sd_printing) abortSDPrinting();
    if (marlin_state == MF_SD_COMPLETE) finishSDPrinting();
  #endif

  queue.advance();

  endstops.event_handler();

  TERN_(HAS_TFT_LVGL_UI, printer_state_polling());
}
