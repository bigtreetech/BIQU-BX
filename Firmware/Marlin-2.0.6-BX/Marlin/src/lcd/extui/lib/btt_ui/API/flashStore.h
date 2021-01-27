#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "Settings.h"

void readStoredPara(void);
void storePara(void);
bool readIsTSCExist(void);
bool readIsRestored(void);

#ifdef __cplusplus
}
#endif

#endif
