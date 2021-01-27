#include "coordinate.h"
#include "string.h"
#include "../includes.h"

const char axis_id[TOTAL_AXIS] = {'X', 'Y', 'Z', 'E'};

bool coorGetRelative(void)
{
  return 0;
}

bool eGetRelative(void)
{
  return 0;
}

bool coordinateIsKnown(void)
{
  return MarlinAPI_isPositionKnown();
}

float coordinateGetAxisTarget(AXIS axis)
{
  return MarlinAPI_GetAxisPosition(axis);
}

uint32_t coordinateGetFeedRate(void)
{
  return MarlinAPI_GetMoveSpeedMM_S();
}

// void coordinateGetAll(COORDINATE *tmp)
// {
//   memcpy(tmp, &targetPosition, sizeof(targetPosition));
// }


float coordinateGetAxisActual(AXIS axis)
{
  return MarlinAPI_GetAxisPosition(axis);
}
