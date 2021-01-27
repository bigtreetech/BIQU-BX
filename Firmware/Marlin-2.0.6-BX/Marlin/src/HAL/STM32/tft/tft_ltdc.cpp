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

#include "../../../inc/MarlinConfig.h"

#if HAS_LTDC_TFT

#include "tft_ltdc.h"
#include "pinconfig.h"
#include "../ltdc/gt911.h"
#include "../../../lcd/extui/lib/btt_ui/Hal/w25qxx.h"
#include "../../../lcd/extui/lib/btt_ui/API/UI/touch_process.h"

uint16_t TFT_LTDC::x_min = 0;
uint16_t TFT_LTDC::x_max = 0;
uint16_t TFT_LTDC::y_min = 0;
uint16_t TFT_LTDC::y_max = 0;
uint16_t TFT_LTDC::x_cur = 0;
uint16_t TFT_LTDC::y_cur = 0; 
uint8_t TFT_LTDC::reg = 0;

void TFT_LTDC::Init() {
  LTDC_Init();
    GT911_Init();
  W25Qxx_Init();
}

uint32_t TFT_LTDC::GetID() {
  return 0x1111;
}

 uint32_t TFT_LTDC::ReadID(uint16_t Reg) {
   return 0x1111;
 }

bool TFT_LTDC::isBusy() {
  return false;
}

void TFT_LTDC::WriteData(uint16_t data) {  
  switch (reg) {
    case 0x01: x_cur = x_min = data; return;
    case 0x02: x_max = data; return;
    case 0x03: y_cur = y_min = data; return;
    case 0x04: y_max = data; return;
  }
  Transmit(data); 
}

void TFT_LTDC::WriteReg(uint16_t Reg) {
  reg = Reg;
}

void TFT_LTDC::TransmitDMA(uint32_t MemoryIncrease, uint16_t *Data, uint16_t Count) {

  while (x_cur != x_min && Count) {
    Transmit(*Data);
    if (MemoryIncrease == DMA_PINC_ENABLE) Data++;
    Count--;
  }

  uint16_t width = x_max - x_min + 1;
  uint16_t height = Count / width;
  uint16_t x_end_cnt = Count - (width * height);

  if (height) {
    if (MemoryIncrease == DMA_PINC_ENABLE) {
      LTDC_Color_Fill(x_min, y_cur, x_min + width, y_cur + height, Data);
      Data += width * height;
    } else {
      LTDC_Fill(x_min, y_cur, x_min + width, y_cur + height, *Data);
    }
    y_cur += height;
  }

  while (x_end_cnt) {
    Transmit(*Data);
    if (MemoryIncrease == DMA_PINC_ENABLE) Data++;
    x_end_cnt--;
  }
}

#endif // HAS_LTDC_TFT
