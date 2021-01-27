#ifndef _UI_DRAW_H_
#define _UI_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "../../Variants/variants.h"
#include "GUI.h"

#define SPACE_X ((BTT_LCD_WIDTH - ICON_WIDTH*4)/4)
#define START_X (SPACE_X/2)
#define SPACE_X_PER_ICON (BTT_LCD_WIDTH/4)

#define SPACE_Y ((BTT_LCD_HEIGHT - ICON_START_Y - ICON_HEIGHT*2)/2)

/*about GCODE KEY*/
#define GKEY_WIDTH  (BTT_LCD_WIDTH/2.5/4)
#define GKEY_HEIGHT ((BTT_LCD_HEIGHT - ICON_START_Y)/6)

void LCD_DMA_Config(void);

void lcd_frame_display(uint16_t sx,uint16_t sy,uint16_t w,uint16_t h, uint32_t addr);
void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
bool model_DirectDisplay(GUI_POINT pos, char *gcode);
bool model_DecodeToFlash(char *gcode);
void ICON_CustomReadDisplay(uint16_t sx,uint16_t sy,uint16_t w, uint16_t h, uint32_t addr);
void SMALLICON_ReadDisplay(uint16_t sx,uint16_t sy, uint8_t icon);
void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon);

#ifdef __cplusplus
}
#endif

#endif
