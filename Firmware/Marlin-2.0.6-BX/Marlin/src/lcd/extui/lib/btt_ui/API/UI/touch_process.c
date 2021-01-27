#include "touch_process.h"
#include "../../includes.h"
#include "../../../../../../HAL/STM32/ltdc/gt911.h"

float tsc_x_inc;
float tsc_x_offset;
float tsc_y_inc;
float tsc_y_offset;
//static volatile bool touchScreenIsPress=false;
// #define touchScreenIsPress !XPT2046_Read_Pen()
bool touchSound = true;
bool CapScreen = 0;

// void TS_Get_Coordinates(uint16_t *x, uint16_t *y)
// {
//   uint16_t tp_x = XPT2046_Repeated_Compare_AD(CMD_RDX);
//   uint16_t tp_y = XPT2046_Repeated_Compare_AD(CMD_RDY);

//   *x = tsc_x_inc * tp_x + tsc_x_offset;
//   *y = tsc_y_inc * tp_y + tsc_y_offset;
// }

// #define TS_ERR_RANGE 10
// uint8_t calibrationEnsure(uint16_t x,uint16_t y)
// {
//   uint32_t i;
//   uint16_t tp_x,tp_y,x_offset;
//   int lcd_x,lcd_y;
//   GUI_SetColor(BLACK);
//   GUI_FillCircle(x,y,5);
//   for(i=0;i<10;i++)
//   {
//     GUI_DrawPoint(x+i,y);
//     GUI_DrawPoint(x-i,y);
//     GUI_DrawPoint(x,y+i);
//     GUI_DrawPoint(x,y-i);
//   }
//   while(!isPress()) {
//     uint16_t x, y;
//     TS_Get_Coordinates(&x, &y);
//   };
//   tp_x = XPT2046_Repeated_Compare_AD(CMD_RDX);
//   tp_y = XPT2046_Repeated_Compare_AD(CMD_RDY);

//   lcd_x = tsc_x_inc * tp_x + tsc_x_offset;
//   lcd_y = tsc_y_inc * tp_y + tsc_y_offset;


//   if(lcd_x < x+TS_ERR_RANGE && lcd_x>x-TS_ERR_RANGE  && lcd_y > y-TS_ERR_RANGE && lcd_y<y+TS_ERR_RANGE)
//   {
//     x_offset=(BTT_LCD_WIDTH - GUI_StrPixelWidth(textSelect(LABEL_ADJUST_OK))) >> 1;
//     GUI_DispString(x_offset, BTT_LCD_HEIGHT-40, textSelect(LABEL_ADJUST_OK));
//     Delay_ms(1000);
//   }
//   else
//   {
//     while(isPress()) {
//       uint16_t x, y;
//       TS_Get_Coordinates(&x, &y);
//     };
//     GUI_SetColor(RED);
//     x_offset=(BTT_LCD_WIDTH - GUI_StrPixelWidth(textSelect(LABEL_ADJUST_FAILED))) >> 1;
//     GUI_DispString(x_offset, BTT_LCD_HEIGHT-40, textSelect(LABEL_ADJUST_FAILED));
//     GUI_DispDec(0,0,lcd_x,3,0);
//     GUI_DispDec(0,20,lcd_y,3,0);
//     Delay_ms(1000);
//     return 0;
//   }
//   return 1;
// }

// void TSC_Calibration(void)
// {
//   uint16_t x_offset;
//   uint32_t LCD_X[3] = {40, BTT_LCD_WIDTH-40, BTT_LCD_WIDTH-40};
//   uint32_t LCD_Y[3] = {40, 40, BTT_LCD_HEIGHT-40};
//   uint16_t TP_X[3],TP_Y[3];
//   uint32_t tp_num = 0;
//   int i;

//   do
//   {
//     GUI_Clear(WHITE);
//     GUI_SetColor(BLACK);
//     GUI_SetBkColor(WHITE);
//     x_offset=(BTT_LCD_WIDTH - GUI_StrPixelWidth(textSelect(LABEL_ADJUST_TITLE))) >> 1;
//     GUI_DispString(x_offset, 5, textSelect(LABEL_ADJUST_TITLE));
//     x_offset=(BTT_LCD_WIDTH - GUI_StrPixelWidth(textSelect(LABEL_ADJUST_INFO))) >> 1;
//     GUI_DispString(x_offset, 25, textSelect(LABEL_ADJUST_INFO));
//     GUI_SetColor(RED);
//     for(tp_num = 0;tp_num<3;tp_num++)
//     {
//       GUI_FillCircle(LCD_X[tp_num],LCD_Y[tp_num],3);
//       for(i=0;i<10;i++)
//       {
//         GUI_DrawPoint(LCD_X[tp_num]+i,LCD_Y[tp_num]);
//         GUI_DrawPoint(LCD_X[tp_num]-i,LCD_Y[tp_num]);
//         GUI_DrawPoint(LCD_X[tp_num],LCD_Y[tp_num]+i);
//         GUI_DrawPoint(LCD_X[tp_num],LCD_Y[tp_num]-i);
//       }
//       while(isPress() == false)
//       {
//         #if ENABLED(USE_WATCHDOG)
//         IWDG1->KR=0XAAAA;
//         #endif
//       }
//       TP_X[tp_num] = XPT2046_Repeated_Compare_AD(CMD_RDX);
//       TP_Y[tp_num] = XPT2046_Repeated_Compare_AD(CMD_RDY);
//       while(isPress() != false)
//       {
//         #if ENABLED(USE_WATCHDOG)
//         IWDG1->KR=0XAAAA;
//         #endif
//       }

//     }

//     #define Y0 LCD_X[0]
//     #define Y1 LCD_X[1]
//     #define X0 TP_X[0]
//     #define X1 TP_X[1]
//     tsc_x_inc = (Y1 - Y0) * 1.0f / (X1 - X0);
//     tsc_x_offset = (X0 * Y1 - X1 * Y0)  * 1.0f / (X0 - X1);
//     #undef Y0
//     #undef Y1
//     #undef X0
//     #undef X1
//     #define Y0 LCD_Y[1]
//     #define Y1 LCD_Y[2]
//     #define X0 TP_Y[1]
//     #define X1 TP_Y[2]
//     tsc_y_inc = (Y1 - Y0) * 1.0f / (X1 - X0);
//     tsc_y_offset = (X0 * Y1 - X1 * Y0)  * 1.0f / (X0 - X1);
//   }while(calibrationEnsure(BTT_LCD_WIDTH/2, BTT_LCD_HEIGHT/2)==0);

//   GUI_RestoreColorDefault();
// }

// uint16_t Key_value(uint8_t total_rect,const GUI_RECT* menuRect)
// {
//   uint8_t i=0;
//   uint16_t x, y;

//   TS_Get_Coordinates(&x,&y);
//   for(i=0;i<total_rect;i++)
//   {
//     if((x>menuRect[i].x0)&&(x<menuRect[i].x1)&&(y>menuRect[i].y0)&&(y<menuRect[i].y1))
//     {
//       #ifdef BUZZER_PIN
//         if(touchSound == true) BUZZER_PLAY(sound_keypress);
//       #endif
//       return i;
//     }
//   }
//   return IDLE_TOUCH;
// }

void loopTouchScreen(void) // Handle in interrupt
{
  // static uint8_t touch;
  // if(!XPT2046_Read_Pen())
  // {
  //   if(touch >= 20) // 20ms
  //   {
  //     touchScreenIsPress = true;
  //   }
  //   else
  //   {
  //     touch++;
  //   }
  // }
  // else
  // {
  //   touchScreenIsPress = false;
  //   touch = 0;
  // }
}

uint8_t isPress(void)
{
  static bool pressed = false;

//   if(CapScreen == false)
//   {
//      static uint32_t nextTime = 0;
//      bool nowPressed = !XPT2046_Read_Pen();
//      if (nowPressed) {
//        if (nextTime <= OS_GetTimeMs()) {
//        pressed = true;
//        }
//      } else {
//        pressed = false;
//        nextTime = OS_GetTimeMs() + 10;
//      }  
//       CapScreen=false;
//    } 
//  else
  {
    u16 x ,y;
    if(ReadRelasePoint(&x,&y)==1)
     {
       pressed=true;
		 }
			else
		 {
        pressed=false;
		 }
     CapScreen=true;
  }
 
 
    return pressed; 
}

void (*TSC_ReDrawIcon)(uint8_t positon, uint8_t is_press) = NULL;

uint16_t KEY_GetValue(uint8_t total_rect,const GUI_RECT* menuRect)
{
  static bool firstPress = true;
	static uint16_t   key_num = IDLE_TOUCH;
	uint16_t key_return=IDLE_TOUCH;
 
//  if(CapScreen == false)
//   {
//     if (isPress())
//     {
//       if(firstPress)
//       {
//         key_num = Key_value(total_rect, menuRect);
//         firstPress = false;
//         if(TSC_ReDrawIcon)
//         TSC_ReDrawIcon(key_num, 1);
//       }
//     }
// 		else
// 		{
// 			if (firstPress == false )
// 	  	{
//         if(TSC_ReDrawIcon)
//         TSC_ReDrawIcon(key_num, 0);
//         key_return = key_num;
//         key_num = IDLE_TOUCH;
//         firstPress = true;
// 			}
// 		}
//     CapScreen=0;
//   }
//   else 
  { 
    	u16 x ,y,i;
		switch (ReadRelasePoint(&x,&y))
		{
			case 1:
        if(firstPress)
         {
           for(i=0;i<total_rect;i++)
           {
            if((x>menuRect[i].x0)&&(x<menuRect[i].x1)&&(y>menuRect[i].y0)&&(y<menuRect[i].y1))
            {
             #ifdef BUZZER_PIN
              if(touchSound == true) BUZZER_PLAY(sound_keypress);
             #endif
             key_num=i;
            }
           } 
            firstPress = false;
            if(TSC_ReDrawIcon)
            TSC_ReDrawIcon(key_num, 1);
         }
					break;
			case 2:
          if (firstPress == false )
         {
            firstPress = true;
            if(TSC_ReDrawIcon)
            {
            TSC_ReDrawIcon(key_num, 0);
            }      
            key_return=key_num;
            key_num = IDLE_TOUCH;
         }		
					break;
				  default:
					break;
		}
     CapScreen=1;
  }
    return key_return;
}              

uint16_t KEY_Keep_pressing(uint8_t total_rect,const GUI_RECT* menuRect)//Read the value that has been pressed
{
	static uint16_t   key_num = IDLE_TOUCH;
	uint16_t key_return=IDLE_TOUCH;
    
  // if(CapScreen==false)
  // {   
  //   if (!XPT2046_Read_Pen())
	// 	{
  //   	key_num = Key_value(total_rect, menuRect);
	// 		if(TSC_ReDrawIcon)
	// 		TSC_ReDrawIcon(key_num, 1);
	// 	}
  //   else
  //   {
  //     key_num = IDLE_TOUCH;
  //     if(TSC_ReDrawIcon)
	// 		TSC_ReDrawIcon(key_num, 0);
  //   }
  //   CapScreen=false;
  // }
  // else
  // {
    u16 x ,y,i;
    if(ReadRelasePoint(&x,&y)==1)
    {
      for(i=0;i<total_rect;i++)
      {
        if((x>menuRect[i].x0)&&(x<menuRect[i].x1)&&(y>menuRect[i].y0)&&(y<menuRect[i].y1))
        {
          #ifdef BUZZER_PIN
          if(touchSound == true) BUZZER_PLAY(sound_keypress);
          #endif
          key_num=i;
        }
      }  
      if((i==0)||(i==3))
      {
        if(TSC_ReDrawIcon)
        TSC_ReDrawIcon(key_num, 1);
      }
    }
    else
    { 
       key_num = IDLE_TOUCH;
      if(TSC_ReDrawIcon)
      {
      TSC_ReDrawIcon(key_num, 0);
      }     
    }
  //   CapScreen=true;
  // }
  key_return = key_num;
  key_num = IDLE_TOUCH;
  return key_return;
}

typedef enum
{
  NO_CLICK,
  FIRST_CLICK,
  FIRST_RELEASE,
  SECOND_CLICK,
  LONG_PRESS,
}KEY_STATUS;

#define KEY_DOUOBLE_SPACE        15     //
#define KEY_LONG_PRESS_START     200    //

#define KEY_LONG_PRESS_SPACE_MAX 10     //
#define KEY_LONG_PRESS_SPACE_MIN 2      //

//uint16_t KEY_GetValue(uint8_t total_rect,const GUI_RECT* menuRect)
//{
//  uint16_t key_return=NO_TOUCH;

//  static uint16_t  first_key  = NO_TOUCH;
//  static uint32_t  first_time = 0;
//  static uint8_t   long_press_space = KEY_LONG_PRESS_SPACE_MAX;

//  static KEY_STATUS nowStatus = NO_CLICK;    //������ǰ��״̬

//  if(touchScreenIsPress)
//  {
//    switch(nowStatus)
//    {
//      case NO_CLICK:
//        nowStatus=FIRST_CLICK;
//        first_key=Key_value(total_rect,menuRect);
//        first_time=OS_GetTime();
//        break;

//      case FIRST_CLICK:
//        if(OS_GetTime()-first_time>KEY_LONG_PRESS_START)
//        {
//          nowStatus  = LONG_PRESS;
//          first_key |= KEY_LONG_CLICK;
//        }
//        break;

//      case FIRST_RELEASE:
//        if(first_key == Key_value(total_rect,menuRect))
//        {
//          nowStatus  = SECOND_CLICK;
//          first_key |= KEY_DOUBLE_CLICK;
//        }
//        else
//        {
//          nowStatus=NO_CLICK;
//        }
//        break;

//      case SECOND_CLICK:
//        if(OS_GetTime()-first_time>KEY_LONG_PRESS_START)
//        {
//          nowStatus  = LONG_PRESS;
//          first_key |= KEY_LONG_CLICK;
//        }
//        break;

//      case LONG_PRESS:
//        if(OS_GetTime()-first_time>long_press_space)
//        {
//          if(long_press_space>KEY_LONG_PRESS_SPACE_MIN)
//            long_press_space--;
//          first_time=OS_GetTime();
//          key_return = first_key;
//        }
//        break;

//      default:
//        break;
//    }
//  }
//  else
//  {
//    switch(nowStatus)
//    {
//      case FIRST_CLICK:
//        nowStatus=FIRST_RELEASE;
//        break;

//      case FIRST_RELEASE:
//        if(OS_GetTime()-first_time>KEY_DOUOBLE_SPACE)
//        {
//        nowStatus  = NO_CLICK;
//        key_return = first_key;
//        }
//        break;

//      case SECOND_CLICK:
//        nowStatus  = NO_CLICK;
//        key_return = first_key;
//        break;

//      case LONG_PRESS:
//        nowStatus  = NO_CLICK;
//        key_return = first_key | KEY_LONG_RELEASE;
//        break;

//      default:
//        break;
//    }
//  }
//  return key_return;
//}

// uint16_t KNOB_GetRV(GUI_RECT *knob)
// {
//   uint16_t key_return=IDLE_TOUCH;
//   uint16_t x=0,y=0;

//   static uint16_t oldx=0,oldy=0;
//   static uint32_t mytime;

//   if(touchScreenIsPress && OS_GetTimeMs() > mytime)
//   {
//     mytime = OS_GetTimeMs() + 10;
//     TS_Get_Coordinates(&x,&y);
//     if(x>knob->x0&&x<knob->x1&&y>knob->y0&&y<knob->y1)
//     {
//       if(x>oldx+5)
//       {
//         if(oldy>(knob->y0+knob->y1)/2)
//         {
//           key_return=KNOB_DEC;
//         }
//         else
//         {
//           key_return=KNOB_INC;
//         }
//       }
//       else if(x<oldx-5)
//       {
//         if(oldy>(knob->y0+knob->y1)/2)
//         {
//           key_return=KNOB_INC;
//         }
//         else
//         {
//           key_return=KNOB_DEC;
//         }
//       }
//       if(y>oldy+5)
//       {
//         if(x>(knob->x0+knob->x1)/2)
//         {
//           key_return=KNOB_INC;
//         }
//         else
//         {
//           key_return=KNOB_DEC;
//         }
//       }
//       else if(y<oldy-5)
//       {
//         if(x>(knob->x0+knob->x1)/2)
//         {
//           key_return=KNOB_DEC;
//         }
//         else
//         {
//           key_return=KNOB_INC;
//         }
//       }
//     }
//   }
//   if(key_return != IDLE_TOUCH)
//   {
//     oldx=x;
//     oldy=y;
//   }
//   return key_return;
// }
