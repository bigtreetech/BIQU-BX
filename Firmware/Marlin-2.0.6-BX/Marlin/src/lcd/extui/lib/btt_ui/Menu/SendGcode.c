#include "SendGcode.h"
#include "../includes.h"

#define TERMINAL_MAX_CHAR (BTT_LCD_WIDTH/BYTE_WIDTH * (BTT_LCD_HEIGHT - BYTE_HEIGHT) /BYTE_HEIGHT)
#define CURSOR_START_X 0
#define CURSOR_END_X   (BTT_LCD_WIDTH-BTT_LCD_WIDTH/2.5)
#define CURSOR_START_Y ICON_START_Y //TITLE_END_Y
#define CURSOR_END_Y   BTT_LCD_HEIGHT
const GUI_RECT terminalRect = {0, 0, BTT_LCD_WIDTH, BTT_LCD_HEIGHT};
char terminalBuf[TERMINAL_MAX_CHAR];

typedef enum
{
  SOFT_KEY_123 = 0,
  SOFT_KEY_ABC,
}SOFT_KEY_TYPE;

SOFT_KEY_TYPE softKeyType = SOFT_KEY_123;

// 26 buttons
const GUI_RECT rect_of_Gkey[]={
  {0*GKEY_WIDTH+CURSOR_END_X, 0*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH+CURSOR_END_X, 0*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH+CURSOR_END_X, 0*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH+CURSOR_END_X, 0*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y},

  {0*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH+CURSOR_END_X, 1*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y},

  {0*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH+CURSOR_END_X, 2*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y},

  {0*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH+CURSOR_END_X, 3*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y},

  {0*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH+CURSOR_END_X, 4*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y},

  {0*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y, 1*GKEY_WIDTH+CURSOR_END_X, 6*GKEY_HEIGHT+ICON_START_Y},
  {1*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y, 2*GKEY_WIDTH+CURSOR_END_X, 6*GKEY_HEIGHT+ICON_START_Y},
  {2*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y, 3*GKEY_WIDTH+CURSOR_END_X, 6*GKEY_HEIGHT+ICON_START_Y},
  {3*GKEY_WIDTH+CURSOR_END_X, 5*GKEY_HEIGHT+ICON_START_Y, 4*GKEY_WIDTH+CURSOR_END_X, 6*GKEY_HEIGHT+ICON_START_Y},

  // Back  
  {CURSOR_START_X, 0, CURSOR_END_X, CURSOR_START_Y,},
  // Send
  {3*GKEY_WIDTH+5+CURSOR_END_X, 0*GKEY_HEIGHT+5,  4*GKEY_WIDTH-5+CURSOR_END_X, ICON_START_Y-5},
};

// const char * const softKeyValue[][26] = {
//   {
//     "1", "2", "3", "M", "G", "Del",
//     "4", "5", "6", "X", "Y", "Space",
//     "7", "8", "9", "Z", "E", "ABC",
//     ".", "0", "-", "S", "T", " ",
//     "Back", "Send",
//   },
//   {
//     "A", "B", "C", "D", "F", "Del",
//     "H", "I", "J", "K", "L", "Space",
//     "N", "O", "P", "Q", "R", "123",
//     "U", "V", "W", " ", " ", " ",
//     "Back", "Send",
//   }
// };

const char * const softKeyValue[][26] = {
  {
    "G", "M", "P", "Del",
    "0", "1", "2", "Space",
    "3", "4", "5", "ABC",
    "6", "7", "8", "S",
    "9", ".", "-", "T",
    "X", "Y", "Z", "E",
    " ", "Send",
  },
  {
    "A", "B", "C", "Del",
    "D", "F", "H", "Space",
    "I", "J", "K", "123",
    "L", "N", "O", "P",
    "Q", "R", "U", "V",
    "W", "X", "Y", "Z",
    " ", "Send",
  }
};

GKEY_VALUES GKeyGetValue(void)
{
  return (GKEY_VALUES)KEY_GetValue(sizeof(rect_of_Gkey)/sizeof(rect_of_Gkey[0]), rect_of_Gkey);
}
GKEY_VALUES GKeyGetValue_Press(void)
{
  return (GKEY_VALUES)KEY_Keep_pressing(sizeof(rect_of_Gkey)/sizeof(rect_of_Gkey[0]), rect_of_Gkey);
}

void sendGcodeReDrawButton(uint8_t positon, uint8_t pressed)
{
  if(positon > GKEY_SEND) return;
  if(pressed)
  {
    GUI_SetColor(GRAY);
    GUI_SetBkColor(BLACK);
  }
  GUI_ClearPrect(rect_of_Gkey+positon);
  GUI_DispStringInPrect(rect_of_Gkey + positon, (uint8_t *)softKeyValue[softKeyType][positon]);
  if(pressed)
  {
    GUI_SetColor(BLACK);
    GUI_SetBkColor(GRAY);
  }
}

void menuSendGcodeCallBack(void)
{
  GUI_SetColor(BLACK);
  GUI_RECT gcodeRect = {CURSOR_END_X+5, 0+5, rect_of_Gkey[GKEY_SEND].x0-10, rect_of_Gkey[GKEY_SEND].y1};
  static char gcodeBuf[CMD_MAX_CHAR];
  static uint8_t nowIndex = 0,
                lastIndex = 0;
  CHAR_INFO info;
  static int16_t cursorX = CURSOR_START_X,
                 cursorY = CURSOR_START_Y;
  static uint16_t lastTerminalIndex = 0;
  GKEY_VALUES key_num   = GKeyGetValue();
  switch(key_num)
  {
    case GKEY_IDLE:
      break;

    case GKEY_BACK:
      infoMenu.cur--;
      break;

    case GKEY_SEND:
      if (nowIndex)
      {
        gcodeBuf[nowIndex++] = '\n'; // End char '\n' for Gcode
        gcodeBuf[nowIndex] = 0;
        storeCmd(gcodeBuf);
        gcodeBuf[nowIndex = 0] = 0;
      }
      break;

    case GKEY_ABC_123:
      softKeyType = (softKeyType == SOFT_KEY_123) ? SOFT_KEY_ABC : SOFT_KEY_123;
      for (uint8_t i = 0; i < GKEY_BACK; i++)
      {
        GUI_DispStringInPrect(rect_of_Gkey + i, (uint8_t *)softKeyValue[softKeyType][i]);
      }
      break;

    case GKEY_DEL:
      if(nowIndex)
        gcodeBuf[--nowIndex] = 0;
      break;

    case GKEY_SPACE:
      if(nowIndex < CMD_MAX_CHAR - 1)
      {
        gcodeBuf[nowIndex++] = ' ';
        gcodeBuf[nowIndex] = 0;
      }
      break;

    default:
      if(nowIndex < CMD_MAX_CHAR - 1)
      {
        gcodeBuf[nowIndex++] = softKeyValue[softKeyType][key_num][0];
        gcodeBuf[nowIndex] = 0;
      }
      break;
  }
  if(lastIndex != nowIndex)
  {
    lastIndex = nowIndex;
    GUI_ClearPrect(&gcodeRect);
  }
  GUI_DispStringInPrect(&gcodeRect, (uint8_t *)gcodeBuf);

  while (terminalBuf[lastTerminalIndex])
  {
    getCharacterInfo((uint8_t *)&terminalBuf[lastTerminalIndex], &info);
    if (cursorX + info.pixelWidth > CURSOR_END_X || (terminalBuf[lastTerminalIndex] == '\n' && cursorX != CURSOR_START_X)) // Next Line
    {
      cursorX = CURSOR_START_X;
      cursorY += info.pixelHeight;
      if (cursorY + info.pixelHeight > CURSOR_END_Y)
      {
        lastTerminalIndex = 0;
        terminalBuf[0] = 0;
        break;
      }
    }
    if(terminalBuf[lastTerminalIndex] != '\n')
    {
      if (cursorY + info.pixelHeight > CURSOR_END_Y) // Clear window
      {
        cursorX = CURSOR_START_X;
        cursorY = CURSOR_START_Y;
        GUI_ClearRect(CURSOR_START_X, CURSOR_START_Y, CURSOR_END_X-1, CURSOR_END_Y);
      }
      GUI_DispOne(cursorX, cursorY, (uint8_t *)&terminalBuf[lastTerminalIndex]);
      cursorX += info.pixelWidth;
    }
    lastTerminalIndex += info.bytes;
  }

  if(infoMenu.menu[infoMenu.cur] != menuSendGcode)
  {
    cursorX = CURSOR_START_X;
    cursorY = CURSOR_START_Y;
    GUI_RestoreColorDefault();
  }
}

void menuSendGcode(void)
{
  GUI_RECT gcodeRect = {CURSOR_END_X+5, 0+5, rect_of_Gkey[GKEY_SEND].x0-5, rect_of_Gkey[GKEY_SEND].y1};

  GUI_SetBkColor(DARKBLUE);
  GUI_ClearRect(0, 0, BTT_LCD_WIDTH, rect_of_Gkey[0].y0);
  GUI_SetBkColor(GRAY);
  GUI_ClearPrect(&gcodeRect);                          //gcode display area
  GUI_ClearRect(0, rect_of_Gkey[0].y0, BTT_LCD_WIDTH, BTT_LCD_HEIGHT);  // serial message display area
  GUI_SetColor(GRAY);
  GUI_FillPrect(&rect_of_Gkey[GKEY_SEND]);  // Send button
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetColor(WHITE);
  GUI_DispStringInRect(CURSOR_START_X, 0, CURSOR_END_X, CURSOR_START_Y, (uint8_t *)"Touch to exit");
  GUI_SetColor(BLACK);
  GUI_VLine(CURSOR_END_X-1, 0 , BTT_LCD_HEIGHT);       //Keyboard vertical line
  for (uint8_t i = 0; i < COUNT(softKeyValue[0]); i++)
  {
    // GUI_DrawPrect(rect_of_Gkey + i);  // Draw keyboard border
    GUI_DispStringInPrect(rect_of_Gkey + i, (uint8_t *)softKeyValue[softKeyType][i]);
  }
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  TSC_ReDrawIcon = sendGcodeReDrawButton;

  menuSetFrontCallBack(menuSendGcodeCallBack);
}

void sendGcodeTerminalCache(char *stream, TERMINAL_SRC src)
{
  const char* const terminalSign[] = {"Send: ", "Rcv: "};
  if (infoMenu.menu[infoMenu.cur] != menuSendGcode) return;
  if (strlen(terminalBuf) + strlen(stream) + strlen(terminalSign[src]) >= TERMINAL_MAX_CHAR)
  {
    terminalBuf[0] = 0;
  }
  strlcat(terminalBuf, terminalSign[src], TERMINAL_MAX_CHAR);
  strlcat(terminalBuf, stream, TERMINAL_MAX_CHAR);
}
