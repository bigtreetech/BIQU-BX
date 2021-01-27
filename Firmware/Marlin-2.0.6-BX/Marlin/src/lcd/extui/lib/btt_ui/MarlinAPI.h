#ifndef _MARLIN_API_H_
#define _MARLIN_API_H_

#include <stdbool.h>
#include <stdint.h>
#include "API/coordinate.h"
#include "API/MachineParameters.h"


/******************************************************************** Heater ***********************************************************************/

void MarnlinAPI_SetTargetTemp(uint8_t index, int16_t temp);
uint16_t MarnlinAPI_GetTargetTemp(uint8_t index);
int16_t MarnlinAPI_GetCurrentTemp(uint8_t index);
uint8_t MarnlinAPI_GetCurrentTool(void);
uint8_t MarnlinAPI_GetCurrentHotend(void);


/******************************************************************** Gcode **********************************************************************/
bool MarlinAPI_HasInjectCommands(void);
void MarlinAPI_InjectCommand(char * const gcode);


/******************************************************************** Printing ***********************************************************************/

bool MarlinAPI_StartPrint(char *filename);
void MarlinAPI_AbortPrinting(void);
bool MarlinAPI_IsReadingMedia(void);
bool MarlinAPI_IsPaused(void);
uint32_t MarlinAPI_PrintProgress(void);
uint32_t MarlinAPI_PrintTime(void);
void MarlinAPI_PausePrint(void);
void MarlinAPI_ResumePrint(void);


/******************************************************************** Fan ***********************************************************************/

uint8_t MarlinAPI_GetFanPercent(uint8_t index);
void MarlinAPI_SetFanPercent(uint8_t index, uint8_t percent);


/******************************************************************** Speed ***********************************************************************/

uint16_t MarlinAPI_GetMoveSpeedPercent(void);
void MarlinAPI_SetMoveSpeedPercent(uint16_t percent);
uint16_t MarlinAPI_GetFlowPercent(void);
void MarlinAPI_SetFlowPercent(uint16_t percent);


/******************************************************************** Coordinate ***********************************************************************/

float MarlinAPI_GetMoveSpeedMM_S(void);
float MarlinAPI_GetAxisPosition(AXIS axis);
bool MarlinAPI_isPositionKnown(void);


/******************************************************************** Leveling ***********************************************************************/

bool MarlinAPI_LevelingActived(void);


/******************************************************************** BabyStep ***********************************************************************/

void MarlinAPI_BabyStep(float distance);
float MarlinAPI_BabyStepTotal(void);


/******************************************************************** Marlin mode ***********************************************************************/

void MarlinAPI_LCDInit(void);
void MarlinAPI_LCDUpdate(void);
void MarlinAPI_LCDKill(const char *lcd_error, const char *lcd_component);
void MarlinAPI_EncoderInit(void);
bool MarlinAPI_Encoder_ReadStep(uint8_t io_index);
void MarlinAPI_RPI_PowerInit(void);
void MarlinAPI_RPI_Set(bool enable);


/******************************************************************** Neopixel ***********************************************************************/

void MarlinAPI_WS2812_Send_DAT(uint32_t ws2812_dat);


/******************************************************************** Buzzer ***********************************************************************/

void MarlinAPI_Buzzer(const uint32_t frequency, const uint32_t duration);


/******************************************************************** Power loss recovery ***********************************************************************/

bool MarlinAPI_PowerFailedExist(void);
void MarlinAPI_PowerFailedResume(void);
void MarlinAPI_PowerFailedDelete(void);


/******************************************************************** Parameter ***********************************************************************/

float MarlinApi_GetParameter(PARAMETER_NAME name, uint8_t index);
void MarlinApi_SetParameter(PARAMETER_NAME name, uint8_t index, float val);


/******************************************************************** Host Action ***********************************************************************/

void MarlinAPI_breakAndContinue(void);
void MarlinAPI_resumeAndPurge(void);
void MarlinAPI_resumeAndContinue(void);


/******************************************************************** MISC ***********************************************************************/

uint32_t MarlinAPI_GetMS(void);
void MarlinAPI_SetPWM(uint8_t duty);
void MarlinAPI_EMStop(void);
void MarlinAPI_MainLoop(void);

#endif
