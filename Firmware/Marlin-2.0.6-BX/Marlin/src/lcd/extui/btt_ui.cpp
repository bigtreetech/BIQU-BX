/*********************
 * example.cpp *
 *********************/

/****************************************************************************
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <https://www.gnu.org/licenses/>.                              *
 ****************************************************************************/

#include "../../inc/MarlinConfigPre.h"

#if ENABLED(BTT_STYLE_UI)

#include "ui_api.h"

#include "lib/btt_ui/includes.h"
#include "../../HAL/STM32/ltdc/ltdc.h"
#include "../../HAL/STM32/ltdc/gt911.h"

MENU infoMenu = {.menu = {menuStatus}, .cur = 0};  // Menu structure
FP_MENU menuFrontCallBack = 0;
// To implement a new UI, complete the functions below and
// read or update Marlin's state using the methods in the
// ExtUI methods in "../ui_api.h"
//
// Although it may be possible to access other state
// variables from Marlin, using the API here possibly
// helps ensure future compatibility.

void usrConfirmLoop(void) {
  if (!wait_for_user) {
    infoMenu.cur--;
  }
}

void menuSetFrontCallBack(FP_MENU cb) {
  menuFrontCallBack = cb;
}

namespace ExtUI {
  void onStartup() {

    MarlinAPI_RPI_PowerInit();
    LTDC_Init();
    GT911_Init();
 
    W25Qxx_Init();
    readStoredPara();
    ltdc_dir = infoSettings.rotate_ui;
    scanUpdates();

    if (readIsTSCExist() == false) // Read settings parameter
    {
      // if(CapScreen == false)
      //  {
      //   TSC_Calibration();
      //   storePara();
      //  }
    }
    else if (readIsRestored())
    {
      storePara();
    }

    infoMenuSelect();

    HW_EncoderInit();
    mcuClocks.SYSCLK_Frequency = HAL_RCC_GetSysClockFreq();
    mcuClocks.HCLK_Frequency = HAL_RCC_GetHCLKFreq();
    mcuClocks.PCLK1_Frequency = HAL_RCC_GetPCLK1Freq();
    mcuClocks.PCLK2_Frequency = HAL_RCC_GetPCLK1Freq();

    knob_LED_Init();
    marlinSetupMachine();
    fanControlInit();
    statusScreen_setReady();
  }

  void onIdle() {
    static FP_MENU lastMenu = 0;
    if (lastMenu != infoMenu.menu[infoMenu.cur]) { // Menu switch
      lastMenu = infoMenu.menu[infoMenu.cur];
      lastMenu();
    }

    if (menuFrontCallBack) { // Call back is vaild
      menuFrontCallBack();
    }

    loopBackEnd();
    // loopFrontEnd();

   // Avoid xpt2046 tpen pin hang up
  //  if(CapScreen == false)
  //      {
  //       uint16_t x, y;
  //       TS_Get_Coordinates(&x, &y);
  //      }
  }
  void onPrinterKilled(PGM_P const error, PGM_P const component) {
    popupDrawPage(DIALOG_TYPE_ERROR, NULL, (uint8_t*)component, (uint8_t*)error, NULL, NULL);
  }
  void onMediaInserted() {};
  void onMediaError() {};
  void onMediaRemoved() {};
  void onPlayTone(const uint16_t frequency, const uint16_t duration) {}
  void onPrintTimerStarted() {}
  void onPrintTimerPaused() {}
  void onPrintTimerStopped() {}
  void onFilamentRunout(const extruder_t extruder) {
    showDialog(DIALOG_TYPE_INFO, (uint8_t*)"onFilamentRunout", (uint8_t*)"onFilamentRunout, continue?",
                   textSelect(LABEL_CONFIRM), NULL, ExtUI::setUserConfirmed, NULL, NULL);
  }
  void onUserConfirmRequired(const char * const msg) {
    showDialog(DIALOG_TYPE_QUESTION, (uint8_t*)"Printer is Paused", (uint8_t*)"Paused for user, continue?",
                   textSelect(LABEL_CONFIRM), NULL, ExtUI::setUserConfirmed, NULL, usrConfirmLoop);
  }
  void onStatusChanged(const char * const msg) {
    statusScreen_setMsg((uint8_t *)textSelect(LABEL_STATUS), (uint8_t *)msg);

    if (infoMenu.menu[infoMenu.cur] == menuSendGcode || infoMenu.menu[infoMenu.cur] == menuStatus) return;

    addToast(DIALOG_TYPE_INFO, (char *)msg);
  }
  void onFactoryReset() {}

  void onStoreSettings(char *buff) {
    // Called when saving to EEPROM (i.e. M500). If the ExtUI needs
    // permanent data to be stored, it can write up to eeprom_data_size bytes
    // into buff.

    // Example:
    //  static_assert(sizeof(myDataStruct) <= ExtUI::eeprom_data_size);
    //  memcpy(buff, &myDataStruct, sizeof(myDataStruct));
  }

  void onLoadSettings(const char *buff) {
    // Called while loading settings from EEPROM. If the ExtUI
    // needs to retrieve data, it should copy up to eeprom_data_size bytes
    // from buff

    // Example:
    //  static_assert(sizeof(myDataStruct) <= ExtUI::eeprom_data_size);
    //  memcpy(&myDataStruct, buff, sizeof(myDataStruct));
  }

  void onConfigurationStoreWritten(bool success) {
    // Called after the entire EEPROM has been written,
    // whether successful or not.
  }

  void onConfigurationStoreRead(bool success) {
    // Called after the entire EEPROM has been read,
    // whether successful or not.
  }

  #if HAS_MESH
    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float zval) {
      // Called when any mesh points are updated
    }

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const ExtUI::probe_state_t state) {
      // Called to indicate a special condition
    }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {
      // Called for temperature PID tuning result
    }
  #endif
}

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI
