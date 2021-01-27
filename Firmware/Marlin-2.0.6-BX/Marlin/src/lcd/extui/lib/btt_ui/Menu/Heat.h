#ifndef _HEAT_H_
#define _HEAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../TSC_Configuration.h"
#include "../API/Settings.h"
#include "../API/Temperature.h"

void menuHeat(void);
void heatSetCurrentIndex(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
