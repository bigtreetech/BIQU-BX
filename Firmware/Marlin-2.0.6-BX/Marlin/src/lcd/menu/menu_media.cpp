/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

//
// SD Card Menu
//

#include "../../inc/MarlinConfigPre.h"

#if BOTH(HAS_LCD_MENU, SDSUPPORT)

#include "menu_item.h"
#include "../../sd/cardreader.h"

void lcd_sd_updir() {
  ui.encoderPosition = card.cdup() ? ENCODER_STEPS_PER_MENU_ITEM : 0;
  encoderTopLine = 0;
  ui.screen_changed = true;
  ui.refresh();
}

#if ENABLED(SD_REPRINT_LAST_SELECTED_FILE)

  uint16_t sd_encoder_position = 0xFFFF;
  int8_t sd_top_line, sd_items;

  void MarlinUI::reselect_last_file() {
    if (sd_encoder_position == 0xFFFF) return;
    goto_screen(menu_media, sd_encoder_position, sd_top_line, sd_items);
    sd_encoder_position = 0xFFFF;
    defer_status_screen();
  }

#endif

inline void sdcard_start_selected_file() {
  EnterDir(card.filename);
  card.openAndPrintFile(infoFile.title);
  ui.return_to_status();
  ui.reset_status();
}

class MenuItem_sdfile : public MenuItem_sdbase {
  public:
    static inline void draw(const bool sel, const uint8_t row, PGM_P const pstr, CardReader &theCard) {
      MenuItem_sdbase::draw(sel, row, pstr, theCard, false);
    }
    static void action(PGM_P const pstr, CardReader &) {
      #if ENABLED(SD_REPRINT_LAST_SELECTED_FILE)
        // Save menu state for the selected file
        sd_encoder_position = ui.encoderPosition;
        sd_top_line = encoderTopLine;
        sd_items = screen_items;
      #endif
      #if ENABLED(SD_MENU_CONFIRM_START)
        MenuItem_submenu::action(pstr, []{
          char * const longest = card.longest_filename();
          char buffer[strlen(longest) + 2];
          buffer[0] = ' ';
          strcpy(buffer + 1, longest);
          MenuItem_confirm::select_screen(
            GET_TEXT(MSG_BUTTON_PRINT), GET_TEXT(MSG_BUTTON_CANCEL),
            sdcard_start_selected_file, ui.goto_previous_screen,
            GET_TEXT(MSG_START_PRINT), buffer, PSTR("?")
          );
        });
      #else
        sdcard_start_selected_file();
        UNUSED(pstr);
      #endif
    }
};

class MenuItem_sdfolder : public MenuItem_sdbase {
  public:
    static inline void draw(const bool sel, const uint8_t row, PGM_P const pstr, CardReader &theCard) {
      MenuItem_sdbase::draw(sel, row, pstr, theCard, true);
    }
    static void action(PGM_P const, CardReader &theCard) {
      card.cd(theCard.filename);
      encoderTopLine = 0;
      ui.encoderPosition = 2 * (ENCODER_STEPS_PER_MENU_ITEM);
      ui.screen_changed = true;
      TERN_(HAS_GRAPHICAL_LCD, ui.drawing_screen = false);
      ui.refresh();
    }
};

void menu_media_source() {
  ui.encoder_direction_menus();

  #if HAS_GRAPHICAL_LCD
    static uint16_t fileCnt;
    if (ui.first_page) fileCnt = card.get_num_Files();
  #else
    const uint16_t fileCnt = card.get_num_Files();
  #endif

  START_MENU();
  switch (infoFile.source) {
    case TFT_SD:
      BACK_ITEM(MSG_MEDIA_TFTSD_MENU);
      break;

    case TFT_UDISK:
      BACK_ITEM(MSG_MEDIA_UDISK_MENU);
      break;

    case BOARD_SD:
      BACK_ITEM(MSG_MEDIA_BOARDSD_MENU);
      break;

    default: break;

  }

  if (card.flag.workDirIsRoot) {
    #if !PIN_EXISTS(SD_DETECT)
      ACTION_ITEM(MSG_REFRESH, []{ encoderTopLine = 0; card.mount(false); scanPrintFilesFatFs();});
    #endif
  }
  else if (card.isMounted())
    ACTION_ITEM_P(PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);

  if (ui.should_draw()) for (uint16_t i = 0; i < fileCnt; i++) {
    if (_menuLineNr == _thisItemNr) {
      card.getfilename_sorted(SD_ORDER(i, fileCnt));
      if (card.flag.filenameIsDir)
        MENU_ITEM(sdfolder, MSG_MEDIA_MENU, card);
      else
        MENU_ITEM(sdfile, MSG_MEDIA_MENU, card);
    }
    else
      SKIP_ITEM();
    }

  END_MENU();
}

bool reseted = false;
void menu_media_tftsd() {
  if (!reseted) {
    infoFile.source = TFT_SD;
    resetInfoFile();
    card.flag.workDirIsRoot = true;
    scanPrintFilesFatFs();
    reseted = true;
  }
  menu_media_source();
}

void menu_media_udisk() {
  if (!reseted) {
    infoFile.source = TFT_UDISK;
    resetInfoFile();
    card.flag.workDirIsRoot = true;
    scanPrintFilesFatFs();
    reseted = true;
  }
  menu_media_source();
}

void menu_media_boardsd() {
  if (!reseted) {
    infoFile.source = BOARD_SD;
    resetInfoFile();
    card.flag.workDirIsRoot = true;
    scanPrintFilesFatFs();
    reseted = true;
  }
  menu_media_source();
}

void menu_media() {
  reseted = false;
  START_MENU();
  BACK_ITEM(MSG_MAIN);
  SUBMENU(MSG_MEDIA_TFTSD_MENU, menu_media_tftsd);
  SUBMENU(MSG_MEDIA_UDISK_MENU, menu_media_udisk);
  SUBMENU(MSG_MEDIA_BOARDSD_MENU, menu_media_boardsd);
  END_MENU();
}

#endif // HAS_LCD_MENU && SDSUPPORT
