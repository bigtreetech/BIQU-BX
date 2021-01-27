#include "../includes.h"
#include "Temperature.h"


const char* heaterID[MAX_HEATER_COUNT]            = HEAT_SIGN_ID;
const char* const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char* heatCmd[MAX_HEATER_COUNT]             = HEAT_CMD;
const char* heatWaitCmd[MAX_HEATER_COUNT]         = HEAT_WAIT_CMD;

static HEATER   heater = {{}, NOZZLE0};
static int16_t  lastTarget[MAX_HEATER_COUNT] = {0};
static uint32_t heat_update_time = TEMPERATURE_QUERY_SLOW_DURATION;
static bool     heat_update_waiting = false;
static bool     heat_send_waiting[MAX_HEATER_COUNT];

uint32_t nextHeatCheckTime = 0;

//Set target temperature
void heatSetTargetTemp(uint8_t index, int16_t temp)
{
  MarnlinAPI_SetTargetTemp(index, temp);
}
//Sync target temperature
void heatSyncTargetTemp(uint8_t index, int16_t temp)
{
}

//Get target temperature
uint16_t heatGetTargetTemp(uint8_t index)
{
  return MarnlinAPI_GetTargetTemp(index);
}

// Set current temperature
void heatSetCurrentTemp(uint8_t index, int16_t temp)
{
}

// Get current temperature
int16_t heatGetCurrentTemp(uint8_t index)
{
  return MarnlinAPI_GetCurrentTemp(index);
}

// Is heating waiting to heat up
bool heatGetIsWaiting(uint8_t index)
{
  return false;
}

// Check all heater if there is a heater waiting to be waited
bool heatHasWaiting(void)
{
  return false;
}

// Set heater waiting status
void heatSetIsWaiting(uint8_t tool, HEATER_WAIT isWaiting)
{
}

void heatClearIsWaiting(void)
{
}

// Set current Tool (Extruder)
void heatSetCurrentTool(uint8_t tool)
{
}

// Get current Tool (Extruder)
uint8_t heatGetCurrentTool(void)
{
  return MarnlinAPI_GetCurrentTool();
}

// Get current hotend index in arry T[]
uint8_t heatGetCurrentHotend(void)
{
  return (infoSettings.hotend_count == 1) ? NOZZLE0 : MarnlinAPI_GetCurrentTool();
}

// Check whether the index is a valid heater index.
bool heaterIsValid(uint8_t index)
{
  if (index >= infoSettings.hotend_count && index < MAX_HOTEND_COUNT) return false;
  if (!infoSettings.bed_en && index == TSC_BED) return false;
  if (!infoSettings.chamber_en && index == TSC_CHAMBER) return false;
  return true;
}

// Set temperature update time interval
void heatSetUpdateTime(uint32_t time)
{
  heat_update_time=time;
}

// Set whether we need to query the current temperature
void heatSetUpdateWaiting(bool isWaiting)
{
  heat_update_waiting = isWaiting;
}

// Set whether the heating command has been sent
void heatSetSendWaiting(uint8_t index, bool isWaiting)
{
  heat_send_waiting[index] = isWaiting;
}

// Get whether has heating command in Queue
bool heatGetSendWaiting(uint8_t index)
{
  return heat_send_waiting[index];
}

void updateNextHeatCheckTime(void)
{
  nextHeatCheckTime = OS_GetTimeMs() + heat_update_time;
}

void loopCheckHeater(void)
{

}
