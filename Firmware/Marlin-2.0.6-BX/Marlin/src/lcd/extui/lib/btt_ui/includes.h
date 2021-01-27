#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define requestCommandInfoIsRunning()    0
#define loopVolumeSource()
#include "../../../../inc/MarlinConfigPre.h"
#include "../../../../HAL/STM32/ltdc/ltdc.h"

#include "MarlinAPI.h"

#include "Variants/variants.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "my_misc.h"
#include "os_timer.h"

#include "API/printf/printf.h"
#include "API/boot.h"

#include "API/Colors.h"
// #include "lcd.h"
#include "HAL/LCD_Init.h"
// #include "lcd_dma.h"
#include "API/UI/GUI.h"
#include "API/Language/TSC_Language.h"
#include "API/Language/utf8_decode.h"

// #include "uart.h"
// #include "Serial.h"
// #include "spi.h"
// #include "sw_spi.h"
// #include "CircularQueue.h"
// #include "spi_slave.h"
// #include "timer_pwm.h"

// #include "usbh_core.h"
// #include "usbh_usr.h"
// #include "usbh_msc_core.h"

// #include "sd.h"
#include "Hal/w25qxx.h"
#include "Hal/buzzer.h"

#include "API/LCD_Encoder.h"
// #include "ST7920_Simulator.h"
// #include "HD44780_Simulator.h"
#include "API/UI/ui_draw.h"
#include "API/UI/touch_process.h"
#include "API/interfaceCmd.h"
#include "API/coordinate.h"
#include "../../sd/fatfs/ff.h"
#include "../../sd/API/vfs.h"
#include "../../sd/API/myfatfs.h"
// #include "Gcode/gcode.h"
// #include "Gcode/mygcodefs.h"
// #include "flashStore.h"
// #include "parseACK.h"
#include "API/Temperature.h"
#include "API/Settings.h"
#include "API/Printing.h"
#include "API/MachineParameters.h"
#include "API/FanControl.h"
#include "API/SpeedControl.h"

// #include "extend.h"
#include "API/menu.h"
#include "API/UI/list_item.h"
#include "API/UI/list_widget.h"
#include "API/UI/Numpad.h"
#include "API/Notification.h"

#include "Menu/Popup.h"
#include "Menu/NotificationMenu.h"
#include "SanityCheck.h"

#include "API/Printing.h"
#include "API/ledcolor.h"

//menu
#include "Menu/MainPage.h"
#include "Menu/Heat.h"
#include "Menu/PreheatMenu.h"
#include "Menu/Move.h"
#include "Menu/Home.h"
#include "Menu/Print.h"
#include "Menu/More.h"
#include "Menu/Speed.h"
#include "Menu/BabyStep.h"
#include "Menu/Parametersetting.h"

#include "Menu/Extrude.h"
#include "Menu/Fan.h"
#include "Menu/SettingsMenu.h"
#include "Menu/PrintingMenu.h"
#include "Menu/ScreenSettings.h"
#include "Menu/MachineSettings.h"
#include "Menu/FeatureSettings.h"
#include "Menu/SendGcode.h"
#include "Menu/leveling.h"
#include "Menu/UBLSaveLoad.h"
#include "Menu/BLTouch.h"
#include "Menu/ProbeOffset.h"
#include "Menu/PowerFailed.h"

#include "Menu/Mode.h"
#include "Menu/Selectmode.h"
#include "Menu/MarlinMode.h"

#include "Menu/UnifiedMove.h"
#include "Menu/UnifiedHeat.h"
#include "Menu/StatusScreen.h"

#include "Menu/Tuning.h"
#include "Menu/Pid.h"
#include "Menu/TuneExtruder.h"
#include "Menu/ConnectionSettings.h"


#define MAX_MENU_DEPTH 10       // max sub menu depth
typedef void (*FP_MENU)(void);

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // Menu function buffer
  uint8_t cur;                   // Current menu index in buffer
}MENU;

extern MENU infoMenu;

void menuSetFrontCallBack(FP_MENU cb);

#ifdef __cplusplus
}
#endif

#endif
