#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../Variants/variants.h"


typedef enum
{
  TSC_X_AXIS=0,
  TSC_Y_AXIS,
  TSC_Z_AXIS,
  TSC_E_AXIS,
  TOTAL_AXIS
}AXIS;


extern const char axis_id[TOTAL_AXIS];

bool  coorGetRelative(void);
bool  eGetRelative(void);
bool  coordinateIsKnown(void);
void  coordinateSetKnown(bool known);
float coordinateGetAxisTarget(AXIS axis);
uint32_t coordinateGetFeedRate(void);
// void  coordinateGetAll(COORDINATE *tmp);
float coordinateGetAxisActual(AXIS axis);

#ifdef __cplusplus
}
#endif

#endif
