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
#include "./fatfs/ff.h"
#include <stdint.h>
#include <string.h>

/**
 * \class SdFile
 * \brief SdBaseFile with Print.
 */
class FatApi {
 public:
  FatApi() {}
  FatApi(const char* name, BYTE oflag);
  FIL fat_fil;
  
  bool writeError;
  bool file_opened;

  bool get_filename(const char* path, char* sfn, char *lfn = NULL);
  bool get_filename(char* sfn, char *lfn = NULL);
  bool isOpen() const { return file_opened; }
  uint32_t filePos() const { return fat_fil.fptr; }
  uint32_t fileSize() const { return fat_fil.obj.objsize; }
  
  bool open(const char* path, BYTE mode);
  bool sync();
  bool close();
  bool remove(const char* path);
  bool seekSet(const uint32_t pos);
  int16_t read();
  int16_t read(void* buf, uint16_t nbyte);
  size_t write(uint8_t b);
  int16_t write(const void* buf, uint16_t nbyte);
  void write(const char* str);
  void write_P(PGM_P str);
  void writeln_P(PGM_P str);
};