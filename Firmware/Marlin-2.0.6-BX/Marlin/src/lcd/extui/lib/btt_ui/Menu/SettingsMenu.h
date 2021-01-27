#ifndef _SETTINGSMENU_H_
#define _SETTINGSMENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../API/Settings.h"

typedef struct
{
  uint32_t SYSCLK_Frequency;
  uint32_t HCLK_Frequency;
  uint32_t PCLK1_Frequency;
  uint32_t PCLK2_Frequency;
  uint32_t PCLK1_Timer_Frequency;
  uint32_t PCLK2_Timer_Frequency;
}CLOCKS;

extern CLOCKS mcuClocks;

extern const uint32_t item_baudrate[ITEM_BAUDRATE_NUM];
extern const uint8_t machine_type[64];

void menuInfo(void);
void menuSettings(void);
void infoSetFirmwareName(uint8_t *name, uint8_t name_len);
void infoSetMachineType(uint8_t *machine, uint8_t type_len);

#ifdef __cplusplus
}
#endif

#endif
