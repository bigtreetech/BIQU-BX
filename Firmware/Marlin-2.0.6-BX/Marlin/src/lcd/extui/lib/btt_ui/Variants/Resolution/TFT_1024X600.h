#ifndef _TFT_1024_600_H_
#define _TFT_1024_480_H_

#define BTT_LCD_WIDTH   1024
#define BTT_LCD_HEIGHT  600

#ifndef BYTE_HEIGHT
  #define BYTE_HEIGHT 24
#endif
#ifndef BYTE_WIDTH
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
#endif

#ifndef LARGE_BYTE_HEIGHT
  #define LARGE_BYTE_HEIGHT 32
#endif
#ifndef LARGE_BYTE_WIDTH
  #define LARGE_BYTE_WIDTH  18
#endif

#define ICON_WIDTH   200
#define ICON_HEIGHT  200
#define TITLE_END_Y  60
#define ICON_START_Y (TITLE_END_Y+10)

#define LIST_ICON_WIDTH   180
#define LIST_ICON_HEIGHT  140

#define INFOBOX_WIDTH     456
#define INFOBOX_HEIGHT    200

#define SMALLICON_WIDTH   24
#define SMALLICON_HEIGHT  24

// Status screen menu
#define SSICON_COUNT              5
#define SSICON_WIDTH              160
#define SSICON_HEIGHT             200
//#define SS_SPACE_X              ((BTT_LCD_WIDTH - SSICON_WIDTH*5) - ((BTT_LCD_WIDTH - ICON_WIDTH*4)/4)*3.5)
#define SS_SPACE_X                (((BTT_LCD_WIDTH - (SSICON_WIDTH * SSICON_COUNT + (BTT_LCD_WIDTH - ICON_WIDTH*4)/4)) / (SSICON_COUNT-1))+0.5)
#define SSICON_VAL_Y0             53
#define SSICON_VAL_COLOR          LCD_WHITE
#define SSICON_VAL2_Y0            85
#define SSICON_VAL2_COLOR         LCD_ORANGE
#define SSICON_NAME_Y0            140
#define SSICON_NAME_COLOR         LCD_WHITE

#define STATUS_MSG_ICON_XOFFSET   10
#define STATUS_MSG_ICON_YOFFSET   10
#define STATUS_MSG_TITLE_XOFFSET  10
#define STATUS_MSG_BODY_XOFFSET   5
#define STATUS_MSG_BODY_YOFFSET   130
#define STATUS_MSG_BODY_BOTTOM    30

#define STATUS_GANTRY_YOFFSET     6

#define NAME_LARGE_FONT           false
#define VAL_LARGE_FONT            true
#define VAL2_LARGE_FONT           false

//Heating Menu
#define PREHEAT_TITLE_Y   25
#define PREHEAT_TOOL_Y    95
#define PREHEAT_BED_Y     157

// Printing menu
#define PICON_LG_WIDTH     330
#define PICON_SM_WIDTH     260
#define PICON_HEIGHT       100
#define PICON_SPACE_X      ((BTT_LCD_WIDTH - PICON_LG_WIDTH*2 - PICON_SM_WIDTH - SPACE_X )/2)
#define PICON_SPACE_Y      (ICON_HEIGHT + SPACE_Y - PICON_HEIGHT*2)/2
#define PICON_TITLE_X      105
#define PICON_TITLE_Y      5
#define PICON_VAL_X        100
#define PICON_VAL_Y        (PICON_HEIGHT/2)
#define PICON_VAL_LG_EX    320
#define PICON_VAL_SM_EX    250

// Popup menu
#define POPUP_RECT_START_X  (SPACE_X * 2)
#define POPUP_RECT_END_X    (BTT_LCD_WIDTH - POPUP_RECT_START_X)
#define POPUP_RECT_WIDTH    (POPUP_RECT_END_X - POPUP_RECT_START_X)

#define POPUP_RECT_WINDOW         {POPUP_RECT_START_X, TITLE_END_Y, POPUP_RECT_END_X, BTT_LCD_HEIGHT - SPACE_Y}
#define POPUP_TITLE_HEIGHT        80
#define POPUP_BOTTOM_HEIGHT       105
#define POPUP_BUTTON_HEIGHT       75

#define POPUP_BUTTON_WIDTH    (POPUP_RECT_WIDTH / 3)
#define POPUP_SINGLE_BUTTON_START_X_FROM_WINDOW  ((POPUP_RECT_WIDTH - POPUP_BUTTON_WIDTH) / 2)
#define POPUP_SINGLE_BUTTON_START_X  (POPUP_RECT_START_X + POPUP_SINGLE_BUTTON_START_X_FROM_WINDOW)
#define POPUP_SINGLE_BUTTON_END_X    (POPUP_RECT_END_X - POPUP_SINGLE_BUTTON_START_X_FROM_WINDOW)
#define POPUP_DOUBLE_BUTTON_START_X_FROM_WINDOW  ((POPUP_RECT_WIDTH - POPUP_BUTTON_WIDTH * 2) / 4)
#define POPUP_DOUBLE_BUTTON_START_X  (POPUP_RECT_START_X + POPUP_DOUBLE_BUTTON_START_X_FROM_WINDOW)
#define POPUP_DOUBLE_BUTTON_END_X    (POPUP_RECT_END_X - POPUP_DOUBLE_BUTTON_START_X_FROM_WINDOW)

#define POPUP_RECT_SINGLE_CONFIRM {POPUP_SINGLE_BUTTON_START_X, BTT_LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, POPUP_SINGLE_BUTTON_END_X, BTT_LCD_HEIGHT - SPACE_Y - 10}
#define POPUP_RECT_DOUBLE_CONFIRM {POPUP_DOUBLE_BUTTON_START_X, BTT_LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, POPUP_DOUBLE_BUTTON_START_X + POPUP_BUTTON_WIDTH, BTT_LCD_HEIGHT - SPACE_Y - 10}
#define POPUP_RECT_DOUBLE_CANCEL  {POPUP_DOUBLE_BUTTON_END_X - POPUP_BUTTON_WIDTH, BTT_LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, POPUP_DOUBLE_BUTTON_END_X, BTT_LCD_HEIGHT - SPACE_Y - 10}

// W25Qxx SPI Flash
/*
 * Because of W25Qxx takes 4K as a sector, the entire sector must be erased before writing.
 * The starting address of each file must be 4K(0x1000) aligned in order to avoid erasing the data of the previous file
 */
#define W25QXX_SECTOR_SIZE (0x1000) // 4096byte = 4K

#ifndef LOGO_MAX_SIZE
  #define LOGO_MAX_SIZE           0x12C000    // 1024*600*2 = 0x12C000 bytes
  #define WORD_UNICODE_SIZE       0x480000    // 24*24/8 * 65536(unicode) = 0x480000 bytes(4.5M)
  #define BYTE_ASCII_SIZE           0x1000    // 24*12/8 * 95(visible ascii) = 0x0D5C (+0x1000 4K)
  #define LARGE_FONT_SIZE           0x3000
  #define ICON_MAX_SIZE            0x16000    // 220*200*2 = 0x157C0 (+0x16000) per button icon
  #define STRINGS_STORE_MAX_SIZE    0x5000    // label strings max size
  #define PRINT_GCODES_MAX_SIZE     0x5000    // start/end/cancel gcodes  max size
  #define INFO_SETTINGS_MAX_SIZE    0x1000    // infoSettings size
  #define CUSTOM_GCODE_MAX_SIZE     0x5000    // custom gocdes max size
  #define INFOBOX_MAX_SIZE         0x19000    // 476*200*2 = 0x2E7C0 (+0x2F000)
  #define SMALL_ICON_MAX_SIZE       0x2000    // 24*24*2 = 0x480 (+0x1000) per small icon
#endif

// The offset of the model preview icon in the gcode file
#define MODEL_PREVIEW_OFFSET 0x2B304

#endif
