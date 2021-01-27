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

#include "../inc/MarlinConfig.h"

#if ENABLED(SDSUPPORT)

#include "cardreader.h"

#include "../MarlinCore.h"
#include "../lcd/ultralcd.h"

#if ENABLED(DWIN_CREALITY_LCD)
  #include "../lcd/dwin/dwin.h"
#endif

#include "../module/planner.h"        // for synchronize
#include "../module/printcounter.h"
#include "../gcode/queue.h"
#include "../module/settings.h"
#include "../module/stepper/indirection.h"

#if ENABLED(EMERGENCY_PARSER)
  #include "../feature/e_parser.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../feature/powerloss.h"
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  #include "../feature/pause.h"
#endif

// public:

card_flags_t CardReader::flag;
char CardReader::filename[FF_LFN_BUF + 1], CardReader::longFilename[FF_LFN_BUF + 1];
int8_t CardReader::autostart_index;
FatApi CardReader::file;

CardReader::CardReader() {
  #if ENABLED(SDCARD_SORT_ALPHA)
    sort_count = 0;
    #if ENABLED(SDSORT_GCODE)
      sort_alpha = true;
      sort_folders = FOLDER_SORTING;
      //sort_reverse = false;
    #endif
  #endif
  flag.sdprinting = flag.saving = flag.logging = false;
  memset(flag.mounted, 0, sizeof(flag.mounted));
  // Disable autostart until card is initialized
  autostart_index = -1;

  #if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)
    SET_INPUT_PULLUP(SD_DETECT_PIN);
  #endif
  
  #if PIN_EXISTS(SDPOWER)
    OUT_WRITE(SDPOWER_PIN, HIGH); // Power the SD reader
  #endif
}

FRESULT CardReader::print_gcode_node (
  char *path,    /* Path name buffer with the sub-directory to delete */
  UINT sz_buff,   /* Size of path name buffer (items) */
  FILINFO *fno    /* Name read buffer */
)
{
  UINT i, j;
  FRESULT fr;
  DIR dir;
  static uint8_t not_root = 1;  // 0 means now is root dir, else means not root

  fr = f_opendir(&dir, path); /* Open the directory */
  if (fr != FR_OK) return fr;

  for (i = 0; path[i]; i++); /* Get current path length */
  path[i++] = '/';

  for (;;) {
    fr = f_readdir(&dir, fno);  /* Get a directory item */
    if (fr != FR_OK || !fno->fname[0]) break;   /* End of directory? */
    j = 0;
    do {    /* Make a path name */
      if (i + j >= sz_buff) { /* Buffer over flow? */
        fr = FR_DENIED;
        goto end_print;    /* Fails when buffer overflow */
      }
      path[i + j] = fno->fname[j];
    } while (fno->fname[j++]);
    if ((fno->fattrib & AM_HID) == 0) {  /* Skip hidden files */
      if (fno->fattrib & AM_DIR) {    /* Item is a directory */
        not_root++;
        fr = print_gcode_node(path, sz_buff, fno);
        not_root--;
      } else {                        /* Item is a file */
        fr = FR_OK;
        char *fn = fno->altname[0] ? fno->altname : fno->fname;
        if (strstr(fn, ".g") || strstr(fn, ".G")) { // Non-backup *.G* files are accepted
          if (not_root) SERIAL_CHAR('/');
          SERIAL_ECHO(path);
          SERIAL_CHAR(' ');
          SERIAL_ECHOLN(fno->fsize);
        }
      }
    }
    if (fr != FR_OK) break;
  }

end_print:
  path[--i] = 0;  /* Restore the path name */
  f_closedir(&dir);

  return fr;
}

//
// Recursive method to list all files within a folder
//
void CardReader::printListing(const char * const path/*=nullptr*/) {

  #define BUFFER_SIZE 1024
  char dirBuffer[BUFFER_SIZE];
  FILINFO tmpInfo;
  strncpy(dirBuffer, path, BUFFER_SIZE);
  print_gcode_node(dirBuffer, BUFFER_SIZE, &tmpInfo);
}

//
// List all files on the SD card
//
void CardReader::ls() {

  FS_SOURCE saveVol = infoFile.source;

  for (uint8_t vol = 0; vol < sizeof(flag.mounted); vol++) {
    infoFile.source = (FS_SOURCE)vol;
    printListing(getCurFileSource());
  }

  infoFile.source = saveVol;
}

#if ENABLED(LONG_FILENAME_HOST_SUPPORT)

  //
  // Get a long pretty path based on a DOS 8.3 path
  //
  void CardReader::printLongPath(char * const path) {

    char segment_path[strlen(path) + 1];
    char lfn[FF_LFN_BUF + 1];
    uint16_t segment_start = 0;
    uint16_t segment_i = 0;
    bool first_separator = true;
    // i = 1,  skip the first '/'
    for (uint16_t i = 0; ; i++) {
      if (path[i] == '/' && first_separator) {
        first_separator = false;
        continue;
      }
      if (path[i] == '/' || path[i] == 0) {
        segment_path[segment_i] = 0;
        if (file.get_filename(segment_path, NULL, lfn)) {
          SERIAL_CHAR('/');
          SERIAL_ECHO(lfn[0] ? lfn : "???");
        } else {
          SERIAL_EOL();
          SERIAL_ECHO_START();
          SERIAL_ECHOPAIR(STR_SD_CANT_OPEN_SUBDIR, segment_path + segment_start);
          break;
        }
        segment_start = segment_i;
        if (path[i] == 0) break;
      }
      segment_path[segment_i++] = path[i];
    }

    SERIAL_EOL();
  }

#endif // LONG_FILENAME_HOST_SUPPORT

//
// Echo the DOS 8.3 filename (and long filename, if any)
//
void CardReader::printFilename() {
  if (file.isOpen()) {
    char sfn[FF_SFN_BUF + 1];
    char lfn[FF_LFN_BUF + 1];
    file.get_filename(sfn, lfn);
    SERIAL_ECHO(sfn);
    #if ENABLED(LONG_FILENAME_HOST_SUPPORT)
      if (lfn[0]) {
        SERIAL_ECHO(' ');
        SERIAL_ECHO(lfn);
      }
    #endif
  }
  else
    SERIAL_ECHOPGM("(no file)");

  SERIAL_EOL();
}

#include "./API/vfs.h"
void CardReader::mount(bool mount_all) {

  bool mount_ok = false;
  FS_SOURCE saveVol = infoFile.source;

  if (mount_all) {
    for (uint8_t vol = 0; vol < sizeof(flag.mounted); vol++) {

      infoFile.source = (FS_SOURCE)vol;

      flag.mounted[infoFile.source] = false;

      if (mountFS()) {
        mount_ok = true;
        flag.mounted[infoFile.source] = true;
      }
    }
  } else {
    flag.mounted[infoFile.source] = false;

    if (mountFS()) {
      mount_ok = true;
      flag.mounted[infoFile.source] = true;
    }
  }

  infoFile.source = saveVol;
  if (mount_ok) {
    SERIAL_ECHO_MSG(STR_SD_CARD_OK);
  } else {
    ui.set_status_P(GET_TEXT(MSG_SD_INIT_FAIL), -1);
    SERIAL_ECHO_MSG(STR_SD_INIT_FAIL);
  }
  ui.refresh();
}

void CardReader::manage_media() {
  static uint8_t prev_stat = 2;       // First call, no prior state

  uint8_t stat = uint8_t(IS_SD_INSERTED());
  if (stat == prev_stat) return;

  flag.workDirIsRoot = true;          // Return to root on mount/release

  if (ui.detected()) {

    uint8_t old_stat = prev_stat;
    prev_stat = stat;                 // Change now to prevent re-entry

    if (stat) {                       // Media Inserted
      safe_delay(500);                // Some boards need a delay to get settled
      mount();                        // Try to mount the media
      #if MB(FYSETC_CHEETAH, FYSETC_CHEETAH_V12, FYSETC_AIO_II)
        reset_stepper_drivers();      // Workaround for Cheetah bug
      #endif
      if (!isMounted()) stat = 0;     // Not mounted?
    }
    else {
      #if PIN_EXISTS(SD_DETECT)
        release();                    // Card is released
      #endif
    }

    ui.media_changed(old_stat, stat); // Update the UI

    if (stat) {
      TERN_(SDCARD_EEPROM_EMULATION, settings.first_load());
      if (old_stat == 2)              // First mount?
        TERN(POWER_LOSS_RECOVERY,
          recovery.check(),           // Check for PLR file. (If not there it will beginautostart)
          beginautostart()            // Look for auto0.g on the next loop
        );
    }
  }
}

void CardReader::release() {
  endFilePrint();
  memset(flag.mounted, 0, sizeof(flag.mounted));
  flag.workDirIsRoot = true;
  #if ALL(SDCARD_SORT_ALPHA, SDSORT_USES_RAM, SDSORT_CACHE_NAMES)
    nrFiles = 0;
  #endif
}

void CardReader::openAndPrintFile(const char *name) {
  char cmd[4 + strlen(name) + 1]; // Room for "M23 ", filename, and null
  extern const char M23_STR[];
  sprintf_P(cmd, M23_STR, name);
  for (char *c = &cmd[4]; *c; c++) *c = tolower(*c);
  queue.enqueue_one_now(cmd);
  queue.enqueue_now_P(M24_STR);
}

void CardReader::startFileprint() {
  if (isMounted()) {
    flag.sdprinting = true;
    TERN_(SD_RESORT, flush_presort());
  }
}

void CardReader::endFilePrint(TERN_(SD_RESORT, const bool re_sort/*=false*/)) {
  TERN_(ADVANCED_PAUSE_FEATURE, did_pause_print = 0);
  TERN_(DWIN_CREALITY_LCD, HMI_flag.print_finish = flag.sdprinting);
  flag.sdprinting = flag.abort_sd_printing = false;
  if (isFileOpen()) file.close();
  TERN_(SD_RESORT, if (re_sort) presort());
}

void CardReader::openLogFile(char * const path) {
  flag.logging = DISABLED(SDCARD_READONLY);
  TERN(SDCARD_READONLY,,openFileWrite(path));
}

//
// Get the root-relative DOS path of the selected file
//
void CardReader::getAbsFilename(char *dst) {
  strncpy(dst, infoFile.title, MAX_PATH_LEN);
}


void openFailed(const char * const fname) {
  SERIAL_ECHOLNPAIR(STR_SD_OPEN_FILE_FAIL, fname, ".");
}

void announceOpen(const uint8_t doing, const char * const path) {
  if (doing) {
    PORT_REDIRECT(SERIAL_BOTH);
    SERIAL_ECHO_START();
    SERIAL_ECHOPGM("Now ");
    serialprintPGM(doing == 1 ? PSTR("doing") : PSTR("fresh"));
    SERIAL_ECHOLNPAIR(" file: ", path);
    PORT_RESTORE();
  }
}

//
// Open a file by DOS path for read
// The 'subcall_type' flag indicates...
//   - 0 : Standard open from host or user interface.
//   - 1 : (file open) Opening a new sub-procedure.
//   - 1 : (no file open) Opening a macro (M98).
//   - 2 : Resuming from a sub-procedure
//
void CardReader::openFileRead(char * const path, const uint8_t subcall_type/*=0*/) {
  if (!isMounted()) return;

  switch (subcall_type) {
    case 0:      // Starting a new print. "Now fresh file: ..."
      announceOpen(2, path);
      break;
  }

  endFilePrint();

  file.get_filename(path, filename, longFilename);
  if (file.open(path, FA_READ)) {
    strncpy(infoFile.title, path, MAX_PATH_LEN);
    PORT_REDIRECT(SERIAL_BOTH);
    SERIAL_ECHOLNPAIR(STR_SD_FILE_OPENED, path, STR_SD_SIZE, file.fileSize());
    SERIAL_ECHOLNPGM(STR_SD_FILE_SELECTED);
    PORT_RESTORE();
    ui.set_status(longFilename[0] ? longFilename : filename);
  }
  else
    openFailed(filename);
}

inline void echo_write_to_file(const char * const fname) {
  SERIAL_ECHOLNPAIR(STR_SD_WRITE_TO_FILE, fname);
}

//
// Open a file by DOS path for write
//
void CardReader::openFileWrite(char * const path) { // MSQTODO
  /*if (!isMounted()) return;

  announceOpen(2, path);
  file_subcall_ctr = 0;

  endFilePrint();

  SdFile *curDir;
  const char * const fname = diveToFile(false, curDir, path);
  if (!fname) return;

  #if ENABLED(SDCARD_READONLY)
    openFailed(fname);
  #else
    if (file.open(curDir, fname, O_CREAT | O_APPEND | O_WRITE | O_TRUNC)) {
      flag.saving = true;
      selectFileByName(fname);
      TERN_(EMERGENCY_PARSER, emergency_parser.disable());
      echo_write_to_file(fname);
      ui.set_status(fname);
    }
    else
      openFailed(fname);
  #endif*/
}

//
// Delete a file by name in the working directory
//
void CardReader::removeFile(const char * const name) {
  if (!isMounted()) return;

  endFilePrint();

  const char * const fname = getCurGcodeName(name);
  if (!fname) return;

  #if ENABLED(SDCARD_READONLY)
    SERIAL_ECHOLNPAIR("Deletion failed (read-only), File: ", fname, ".");
  #else
    if (file.remove(name)) {
      SERIAL_ECHOLNPAIR("File deleted:", fname);
      TERN_(SDCARD_SORT_ALPHA, presort());
    }
    else
      SERIAL_ECHOLNPAIR("Deletion failed, File: ", fname, ".");
  #endif
}

void CardReader::report_status() {
  if (isPrinting()) {
    SERIAL_ECHOPGM(STR_SD_PRINTING_BYTE);
    SERIAL_ECHO(file.filePos());
    SERIAL_CHAR('/');
    SERIAL_ECHOLN(file.fileSize());
  }
  else
    SERIAL_ECHOLNPGM(STR_SD_NOT_PRINTING);
}

void CardReader::write_command(char * const buf) {
  char* begin = buf;
  char* npos = nullptr;
  char* end = buf + strlen(buf) - 1;

  file.writeError = false;
  if ((npos = strchr(buf, 'N')) != nullptr) {
    begin = strchr(npos, ' ') + 1;
    end = strchr(npos, '*') - 1;
  }
  end[1] = '\r';
  end[2] = '\n';
  end[3] = '\0';
  file.write(begin);

  if (file.writeError) SERIAL_ERROR_MSG(STR_SD_ERR_WRITE_TO_FILE);
}

//
// Run the next autostart file. Called:
// - On boot after successful card init
// - After finishing the previous autostart file
// - From the LCD command to run the autostart file
//

void CardReader::checkautostart() {

  if (autostart_index < 0 || flag.sdprinting) return;

  if (!isMounted()) mount();
  TERN_(SDCARD_EEPROM_EMULATION, else settings.first_load());

  // Don't run auto#.g when a PLR file exists
  if (isMounted() && TERN1(POWER_LOSS_RECOVERY, !recovery.valid())) {
    char autoname[8];
    sprintf_P(autoname, PSTR("auto%c.g"), autostart_index + '0');

    FILINFO  finfo;
    DIR      dir;
    char     sfn[FF_SFN_BUF + 1];

    if (f_opendir(&dir, getCurFileSource()) == FR_OK) {
      for (;;) {
        if (f_readdir(&dir,&finfo) !=FR_OK || finfo.fname[0] == 0)  break;
        if ((finfo.fattrib&AM_HID) != 0)  continue;

        if ((finfo.fattrib&AM_DIR) != AM_DIR) {
          strncpy(sfn, finfo.altname, FF_SFN_BUF + 1);
          for (int8_t i = (int8_t)strlen(sfn); i--;) sfn[i] = tolower(sfn[i]);
          if (sfn[9] != '~' && strncmp((char*)sfn, autoname, 5) == 0) {
            openAndPrintFile(autoname);
            autostart_index++;
            return;
          }
        }
      }
      f_closedir(&dir);
    }

  }
  autostart_index = -1;
}

void CardReader::beginautostart() {
  autostart_index = 0;
}

void CardReader::closefile(const bool store_location) {
  file.sync();
  file.close();
  flag.saving = flag.logging = false;
  TERN_(EMERGENCY_PARSER, emergency_parser.enable());

  if (store_location) {
    //future: store printer state, filename and position for continuing a stopped print
    // so one can unplug the printer and continue printing the next day.
  }
}

//
// Get info for a file in the working directory by index
//
void CardReader::selectFileByIndex(const uint16_t nr) {
  if (nr < infoFile.f_num) {
    strcpy(longFilename, infoFile.file[nr]);
    flag.filenameIsDir = false;
  } else if (nr < infoFile.F_num + infoFile.f_num) {
    strcpy(longFilename, infoFile.folder[nr - infoFile.f_num]);
    flag.filenameIsDir = true;
  }
  strcpy(filename, longFilename);
}

void CardReader::cd(const char * relpath) {

  if (EnterDir(relpath)) {
    scanPrintFilesFatFs();
    flag.workDirIsRoot = false;
    TERN_(SDCARD_SORT_ALPHA, presort());
  } else {
    SERIAL_ECHO_START();
    SERIAL_ECHOLNPAIR(STR_SD_CANT_ENTER_SUBDIR, relpath);
  }
 /* SdFile newDir;
  SdFile *parent = workDir.isOpen() ? &workDir : &root;

  if (newDir.open(parent, relpath, O_READ)) {
    workDir = newDir;
    flag.workDirIsRoot = false;
    if (workDirDepth < MAX_DIR_DEPTH)
      workDirParents[workDirDepth++] = workDir;
  }
  else {
    SERIAL_ECHO_START();
    SERIAL_ECHOLNPAIR(STR_SD_CANT_ENTER_SUBDIR, relpath);
  }*/
}

#if ENABLED(SDCARD_SORT_ALPHA)

  /**
   * Get the name of a file in the working directory by sort-index
   */
  void CardReader::getfilename_sorted(const uint16_t nr) {
    selectFileByIndex(TERN1(SDSORT_GCODE, sort_alpha) && (nr < sort_count)
      ? sort_order[nr] : nr);
  }
#endif

  // MSQTODO
int8_t CardReader::cdup() {
  if (flag.workDirIsRoot) return 0;

  ExitDir();
  if (IsRootDir()) flag.workDirIsRoot = true;

  scanPrintFilesFatFs();
  /*if (workDirDepth > 0) {                                               // At least 1 dir has been saved
    workDir = --workDirDepth ? workDirParents[workDirDepth - 1] : root; // Use parent, or root if none
    TERN_(SDCARD_SORT_ALPHA, presort());
  }
  if (!workDirDepth) flag.workDirIsRoot = true;
  return workDirDepth;*/
  return !flag.workDirIsRoot;
}

uint16_t CardReader::get_num_Files() {
  if (!isMounted()) return 0;
  // scanPrintFilesFatFs();
  return (infoFile.F_num + infoFile.f_num);
}

//
// Return from procedure or close out the Print Job
//
void CardReader::fileHasFinished() {
  planner.synchronize();
  file.close();

  endFilePrint(TERN_(SD_RESORT, true));

  marlin_state = MF_SD_COMPLETE;
}

#if ENABLED(AUTO_REPORT_SD_STATUS)
  uint8_t CardReader::auto_report_sd_interval = 0;
  millis_t CardReader::next_sd_report_ms;
  #if HAS_MULTI_SERIAL
    int8_t CardReader::auto_report_port;
  #endif

  void CardReader::auto_report_sd_status() {
    millis_t current_ms = millis();
    if (auto_report_sd_interval && ELAPSED(current_ms, next_sd_report_ms)) {
      next_sd_report_ms = current_ms + 1000UL * auto_report_sd_interval;
      PORT_REDIRECT(auto_report_port);
      report_status();
    }
  }
#endif // AUTO_REPORT_SD_STATUS

#if ENABLED(POWER_LOSS_RECOVERY)

  bool CardReader::jobRecoverFileExists() {

    uint16_t vol_len = strlen(getCurFileSource());
    uint16_t name_len = strlen(recovery.filename);

    char path[vol_len + name_len + 1];
    strcpy(path, getCurFileSource());
    strcat(path, recovery.filename);
    path[vol_len + name_len] = 0;

    return f_file_exists(path);
  }

  void CardReader::openJobRecoveryFile(const bool read) {
    if (!isMounted()) return;
    if (recovery.file.isOpen()) return;

    uint16_t vol_len = strlen(getCurFileSource());
    uint16_t name_len = strlen(recovery.filename);

    char path[vol_len + name_len + 1];
    strcpy(path, getCurFileSource());
    strcat(path, recovery.filename);
    path[vol_len + name_len + 0] = 0;

    if (!recovery.file.open(path, read ? FA_READ : FA_OPEN_ALWAYS | FA_WRITE))
      SERIAL_ECHOLNPAIR(STR_SD_OPEN_FILE_FAIL, path, ".");
    else if (!read)
      echo_write_to_file(path);
  }

  // Removing the job recovery file currently requires closing
  // the file being printed, so during SD printing the file should
  // be zeroed and written instead of deleted.
  void CardReader::removeJobRecoveryFile() {
    if (jobRecoverFileExists()) {
      recovery.init();

      uint16_t vol_len = strlen(getCurFileSource());
      uint16_t name_len = strlen(recovery.filename);

      char path[vol_len + name_len + 2];
      strcpy(path, getCurFileSource());
      strcat(path, "/");
      strcat(path, recovery.filename);
      path[vol_len + name_len + 1] = 0;

      removeFile(path);
      #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
        SERIAL_ECHOPGM("Power-loss file delete");
        serialprintPGM(jobRecoverFileExists() ? PSTR(" failed.\n") : PSTR("d.\n"));
      #endif
    }
  }

#endif // POWER_LOSS_RECOVERY

#endif // SDSUPPORT
