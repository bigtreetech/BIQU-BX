#ifndef _LTDC_H_
#define _LTDC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"

typedef struct  
{							 
	uint32_t pwidth;			
	uint32_t pheight;		
	uint16_t hsw;			
	uint16_t vsw;			
	uint16_t hbp;		
	uint16_t vbp;			
	uint16_t hfp;			
	uint16_t vfp;			
	uint8_t  activelayer;		
	uint8_t  dir;				
	uint16_t width;			
	uint16_t height;			
	uint32_t pixsize;		
}_ltdc_dev; 	  
extern _ltdc_dev lcdltdc;							


#define LCD_PIXFORMAT_ARGB8888		0X00		
#define LCD_PIXFORMAT_RGB888 		0X01		
#define LCD_PIXFORMAT_RGB565 		0X02 		
#define LCD_PIXFORMAT_ARGB1555		0X03		
#define LCD_PIXFORMAT_ARGB4444		0X04	
#define LCD_PIXFORMAT_L8			0X05		
#define LCD_PIXFORMAT_AL44			0X06		
#define LCD_PIXFORMAT_AL88 			0X07		

///////////////////////////////////////////////////////////////////////


#define LCD_PIXFORMAT				LCD_PIXFORMAT_RGB565	

#define LTDC_BACKLAYERCOLOR			0X00000000	

#define LCD_FRAME_BUF_ADDR			0XC0000000  	

#define LCD_LED(x)		 GPIO_Pin_Set(GPIOB,PIN5,x)		

void LTDC_Init(void);
void LTDC_DeInit(void);
void LTDC_Switch(uint8_t sw);
void LTDC_Layer_Switch(uint8_t layerx,uint8_t sw);
void LTDC_Select_Layer(uint8_t layerx);

void LTDC_Draw_Point(uint16_t x,uint16_t y,uint32_t color);
uint32_t LTDC_Read_Point(uint16_t x,uint16_t y);	
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color);		
void LTDC_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);
void LTDC_Clear(uint16_t color);

extern uint8_t ltdc_dir;
#ifdef __cplusplus
}
#endif

#endif
