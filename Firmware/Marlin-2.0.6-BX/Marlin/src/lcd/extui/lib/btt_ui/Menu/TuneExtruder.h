#ifndef _TUNE_EXTRUDER_H_
#define _TUNE_EXTRUDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../TSC_Configuration.h"
#include "../API/Settings.h"
#include "../API/Temperature.h"

void menuTuneExtruder(void);
void menuNewExtruderESteps(void);

#ifdef __cplusplus
}
#endif

#endif
