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
#pragma once

#include "../inc/MarlinConfig.h"
#include "fat_api.h"
#include "./fatfs/ff.h"
#include "./API/myfatfs.h"
#include "./API/vfs.h"

#define IFSD(A,B) TERN(SDSUPPORT,A,B)
#if ENABLED(SDSUPPORT)

#if BOTH(SDCARD_SORT_ALPHA, SDSORT_DYNAMIC_RAM)
  #define SD_RESORT 1
#endif

#if ENABLED(SDCARD_RATHERRECENTFIRST) && DISABLED(SDCARD_SORT_ALPHA)
  #define SD_ORDER(N,C) ((C) - 1 - (N))
#else
  #define SD_ORDER(N,C) N
#endif

/*#define MAX_DIR_DEPTH      10       // Maximum folder depth
#define MAXDIRNAMELENGTH   8       // DOS folder name size
#define MAXPATHNAMELENGTH  1024    // (1 + (MAXDIRNAMELENGTH + 1) * (MAX_DIR_DEPTH) + 1 + FILENAME_LENGTH) // "/" + N * ("ADIRNAME/") + "filename.ext"*/

typedef struct {
  bool saving:1,
       logging:1,
       sdprinting:1,
       filenameIsDir:1,
       workDirIsRoot:1,
       mounted[FF_VOLUMES],
       abort_sd_printing:1
       #if ENABLED(BINARY_FILE_TRANSFER)
         , binary_mode:1
       #endif
    ;
} card_flags_t;

class CardReader {
public:
  static card_flags_t flag;                         // Flags (above)
  static char filename[FF_LFN_BUF + 1],                 // DOS 8.3 filename of the selected item
              longFilename[FF_LFN_BUF + 1];             // Long name of the selected item



  CardReader();
  static void mount(bool mount_all = true);
  static void release();
  static inline bool isMounted() { return true; }//flag.mounted[infoFile.source]; }
  static void ls();

  // Handle media insert/remove
  static void manage_media();

  // SD Card Logging
  static void openLogFile(char * const path);
  static void write_command(char * const buf);

  // Auto-Start files
  static int8_t autostart_index;                    // Index of autoX.g files
  static void beginautostart();
  static void checkautostart();
  
  static void openFileRead(char * const path, const uint8_t subcall=0);
  static void openFileWrite(char * const path);
  static void closefile(const bool store_location=false);
  static void removeFile(const char * const name);
  
  static inline char* longest_filename() { return longFilename[0] ? longFilename : filename; }
  #if ENABLED(LONG_FILENAME_HOST_SUPPORT)
    static void printLongPath(char * const path);   // Used by M33
  #endif

  static void cd(const char *relpath);
  static int8_t cdup();

  static uint16_t get_num_Files();
  // Select a file
  static void selectFileByIndex(const uint16_t nr);
  static void selectFileByName(const char* const match);

  static void openAndPrintFile(const char *name);   // (working directory)
  static void fileHasFinished();
  static void getAbsFilename(char *dst);
  static void printFilename();
  static void startFileprint();
  static void endFilePrint(TERN_(SD_RESORT, const bool re_sort=false));
  static void report_status();
  static inline void pauseSDPrint() { flag.sdprinting = false; }  
  static inline bool isPaused() { return isFileOpen() && !flag.sdprinting; }
  static inline bool isPrinting() { return flag.sdprinting; }

  static inline uint8_t percentDone() { return (isFileOpen() && getFileSize()) ? getIndex() / ((getFileSize() + 99) / 100) : 0; }

  #if ENABLED(SDCARD_SORT_ALPHA)
    static void presort();
    static void getfilename_sorted(const uint16_t nr);
    #if ENABLED(SDSORT_GCODE)
      FORCE_INLINE static void setSortOn(bool b) { sort_alpha = b; presort(); }
      FORCE_INLINE static void setSortFolders(int i) { sort_folders = i; presort(); }
      //FORCE_INLINE static void setSortReverse(bool b) { sort_reverse = b; }
    #endif
  #else
    FORCE_INLINE static void getfilename_sorted(const uint16_t nr) { selectFileByIndex(nr);}
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    static bool jobRecoverFileExists();
    static void openJobRecoveryFile(const bool read);
    static void removeJobRecoveryFile();
  #endif

  static inline bool isFileOpen() { return isMounted() && file.isOpen(); }
  static inline uint32_t getIndex() { return file.fat_fil.fptr; }
  static inline uint32_t getFileSize() { return file.fat_fil.obj.objsize; }
  static inline bool eof() { return file.fat_fil.fptr >= file.fat_fil.obj.objsize; }
  static inline void setIndex(const uint32_t index) { f_lseek(&file.fat_fil, index); }
  //static inline char* getWorkDirName() { workDir.getDosName(filename); return filename; }
  static inline int16_t get() { return (int16_t)file.read(); }
  static inline int16_t read(void* buf, uint16_t nbyte) { return file.isOpen() ? file.read(buf, nbyte) : -1; }
  static inline int16_t write(void* buf, uint16_t nbyte) { return file.isOpen() ? file.write(buf, nbyte) : -1; }

  static FRESULT print_gcode_node (char *path, UINT sz_buff, FILINFO *fno);
  static void printListing(const char * const path = 0/*=nullptr*/) ;

private:
  static FatApi file;
  static uint8_t file_subcall_ctr;
};

#if ENABLED(USB_FLASH_DRIVE_SUPPORT)
  #define IS_SD_INSERTED() Sd2Card::isInserted()
#elif PIN_EXISTS(SD_DETECT)
  #define IS_SD_INSERTED() (READ(SD_DETECT_PIN) == SD_DETECT_STATE)
#else
  // No card detect line? Assume the card is inserted.
  #define IS_SD_INSERTED() true
#endif

#define IS_SD_PRINTING()  card.flag.sdprinting
#define IS_SD_PAUSED()    card.isPaused()
#define IS_SD_FILE_OPEN() card.isFileOpen()

extern CardReader card;

#else // !SDSUPPORT

#define IS_SD_PRINTING()  false
#define IS_SD_PAUSED()    false
#define IS_SD_FILE_OPEN() false

#define LONG_FILENAME_LENGTH 0

#endif // !SDSUPPORT