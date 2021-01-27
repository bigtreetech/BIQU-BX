#include "ltdc.h" 
#include "sdram.h" 
#include "gt911.h"

uint8_t ltdc_dir = 0;

volatile u16 *ltdc_lcd_framebuf =(volatile u16 *)0XC0000000;
			
_ltdc_dev lcdltdc;	

void LTDC_Switch(u8 sw)
{
	if(sw==1)LTDC->GCR|=1<<0;		
	else if(sw==0)LTDC->GCR&=~(1<<0);	
}

void LTDC_Layer_Switch(u8 layerx,u8 sw)
{
	if(sw==1)
	{
		if(layerx==0)LTDC_Layer1->CR|=1<<0;		
		else LTDC_Layer2->CR|=1<<0;				
	}else if(sw==0)  
	{
		if(layerx==0)LTDC_Layer1->CR&=~(1<<0);
		else LTDC_Layer2->CR&=~(1<<0);		
	}
	LTDC->SRCR|=1<<0;							
}


void LTDC_Select_Layer(u8 layerx)
{
	lcdltdc.activelayer=layerx;
}


// void LTDC_Display_Dir(u8 dir)
// {
//     lcdltdc.dir=dir; 	
// 	if(dir==0)		
// 	{
// 		lcdltdc.width=lcdltdc.pheight;
// 		lcdltdc.height=lcdltdc.pwidth;	
// 	}else if(dir==1)	
// 	{
// 		lcdltdc.width=lcdltdc.pwidth;
// 		lcdltdc.height=lcdltdc.pheight;
// 	}
// }


void LTDC_Draw_Point(u16 x,u16 y,u32 color)
{ 
	
		uint16_t arryX, arryY;
		if (ltdc_dir) {
			arryX = y;
			arryY = x;
		} else {
			arryX = lcdltdc.pheight-y-1;
			arryY = lcdltdc.pwidth-x-1;
		}
    ltdc_lcd_framebuf[(lcdltdc.pwidth * arryX) + arryY] = color;

}

uint32_t LTDC_Read_Point(u16 x,u16 y)
{ 
    uint16_t arryX, arryY;
            if (ltdc_dir) {
                arryX = y;
                arryY = x;
            } else {
                arryX = lcdltdc.pheight-y-1;
                arryY = lcdltdc.pwidth-x-1;
            }
    return ltdc_lcd_framebuf[(lcdltdc.pwidth * arryX) + arryY];
}

void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{
  if (sx == ex || sy == ey) return;
  u32 psx,psy,pex,pey;
  u32 timeout=0; 
  u16 offline;
  u32 addr; 
  if (ltdc_dir) {
    psx=sx;psy=sy;
    pex=ex-1;pey=ey-1;
  } else {
    psx=lcdltdc.pwidth-ex;psy=lcdltdc.pheight-ey;
    pex=lcdltdc.pwidth-sx-1;pey=lcdltdc.pheight-sy-1;
  }

  offline=lcdltdc.pwidth-(pex-psx+1);
  addr = (u32)&ltdc_lcd_framebuf[(lcdltdc.pwidth * psy) + psx];
  //addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
  RCC->AHB3ENR|=1<<4;				
  DMA2D->CR&=~(1<<0);			
  DMA2D->CR=3<<16;				
  DMA2D->OPFCCR=LCD_PIXFORMAT;	
  DMA2D->OOR=offline;				
  DMA2D->OMAR=addr;			
  DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	
  DMA2D->OCOLR=color;				
  DMA2D->CR|=1<<0;				
  while((DMA2D->ISR&(1<<1))==0)	
  {
    timeout++;
    if(timeout>0X1FFFFF)break;	
  }  
  DMA2D->IFCR|=1<<1;			
}


void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
  if (sx == ex || sy == ey) return;
	u32 psx,psy,pex,pey;	
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	

	if (ltdc_dir) {
		psx=sx;psy=sy;
		pex=ex-1;pey=ey-1;
		addr=(u32)&ltdc_lcd_framebuf[(lcdltdc.pwidth * psy) + psx];
	} else {
		psx=lcdltdc.pwidth-ex;psy=lcdltdc.pheight-ey;
		pex=lcdltdc.pwidth-sx-1;pey=lcdltdc.pheight-sy-1;
		addr=(u32)&ltdc_lcd_framebuf[(lcdltdc.pwidth * psy) + psx];
	}

	offline=lcdltdc.pwidth-(pex-psx+1);
//	offline/=2;
	
//	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	RCC->AHB3ENR|=1<<4;				
	DMA2D->CR&=~(1<<0);				
	DMA2D->CR=0<<16;				
	DMA2D->FGPFCCR=LCD_PIXFORMAT;	
	DMA2D->FGOR=0;					
	DMA2D->OOR=offline;				
	DMA2D->FGMAR=(u32)color;		
	DMA2D->OMAR=addr;				
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	
	DMA2D->CR|=1<<0;				
	while((DMA2D->ISR&(1<<1))==0)	
	{
		timeout++;
		if(timeout>0X1FFFFF)break;
	} 
	DMA2D->IFCR|=1<<1;					
}  


void LTDC_Clear(uint16_t color)
{
		LTDC_Fill(0,0,1024,600,color);
	//LTDC_Fill(0,0,lcdltdc.width,lcdltdc.height,color);
}



u8 LTDC_Clk_Set(u32 pll3n,u32 pll3m,u32 pll3r)
{ 
	u16 retry=0;
	u8 status=0; 
	RCC->CR&=~(1<<28);			
	while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;
 	if(retry==0X1FFF)status=1;	
	else   
	{ 
		RCC->PLLCKSELR&=~(0X3F<<20);	
		RCC->PLLCKSELR|=pll3m<<20;		
		RCC->PLL3DIVR&=~(0X1FF<<0);		
		RCC->PLL3DIVR|=(pll3n-1)<<0;	
		RCC->PLL3DIVR&=~(0X7F<<24);		
		RCC->PLL3DIVR|=(pll3r-1)<<24;	

		RCC->PLLCFGR&=~(0X0F<<8);		
		RCC->PLLCFGR|=0<<10;			
		RCC->PLLCFGR|=0<<9;				
		RCC->PLLCFGR|=1<<24;			
		RCC->CR|=1<<28;					
		while(((RCC->CR&(1<<29))==0)&&(retry<0X1FFF))retry++;	
		if(retry==0X1FFF)status=2;	
 	} 
	return status;
}   

void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height,u8 pixformat)
{
	u32 temp; 
	if(layerx==0)
	{
		temp=(sx+width+((LTDC->BPCR&0X0FFF0000)>>16))<<16; 
		LTDC_Layer1->WHPCR=(sx+((LTDC->BPCR&0X0FFF0000)>>16)+1)|temp;
		temp=(sy+height+(LTDC->BPCR&0X7FF))<<16; 
		LTDC_Layer1->WVPCR=(sy+(LTDC->BPCR&0X7FF)+1)|temp;	
		if(pixformat==0)temp=4;									
		else if(pixformat==1)temp=3;							
		else if(pixformat==5||pixformat==6)temp=1;				
		else temp=2;											
		LTDC_Layer1->CFBLR=(width*temp<<16)|(width*temp+7);	
		LTDC_Layer1->CFBLNR=height;								
	}else
	{
		temp=(sx+width+((LTDC->BPCR&0X0FFF0000)>>16))<<16; 
		LTDC_Layer2->WHPCR=(sx+((LTDC->BPCR&0X0FFF0000)>>16)+1)|temp;
		temp=(sy+height+(LTDC->BPCR&0X7FF))<<16; 
		LTDC_Layer2->WVPCR=(sy+(LTDC->BPCR&0X7FF)+1)|temp;		
		if(pixformat==0)temp=4;									
		else if(pixformat==1)temp=3;							
		else if(pixformat==5||pixformat==6)temp=1;				
		else temp=2;											
		LTDC_Layer2->CFBLR=(width*temp<<16)|(width*temp+7);		
		LTDC_Layer2->CFBLNR=height;								
	}	 
	LTDC_Layer_Switch(layerx,1);									
}

void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor)
{
		LTDC_Layer1->CFBAR=bufaddr;					
		LTDC_Layer1->PFCR=pixformat;				
		LTDC_Layer1->CACR=alpha;					
		LTDC_Layer1->DCCR=((u32)alpha0<<24)|bkcolor;
		LTDC_Layer1->BFCR=((u32)bfac1<<8)|bfac2;	
} 

void LTDC_Reversal(u8 status)
{
    ltdc_dir=status;
}
void LTDC_Init(void)
{   
  u32 tempreg=0;

  //Stm32_Clock_Init(160,5,2,4);
  //delay_init(400);
  SDRAM_Init();

  SDRAM_RGBLCD_Set();

  RCC->APB3ENR|=1<<3;
  RCC->AHB4ENR|=1<<1;
  RCC->AHB4ENR|=0XF<<5;
  GPIO_Set(GPIOB,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PD);
  GPIO_Set(GPIOF,PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
  GPIO_Set(GPIOG,3<<6|1<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
  GPIO_Set(GPIOH,0X7F<<9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
  GPIO_Set(GPIOI,7<<0|0XF<<4|3<<9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);

  GPIO_AF_Set(GPIOF,10,14);		//PF10,AF14

  GPIO_AF_Set(GPIOG,6,14);		//PG6,AF14
  GPIO_AF_Set(GPIOG,7,14);		//PG7,AF14
  GPIO_AF_Set(GPIOG,11,14);		//PG11,AF14

  GPIO_AF_Set(GPIOH,9,14);		//PH9,AF14
  GPIO_AF_Set(GPIOH,10,14);		//PH10,AF14
  GPIO_AF_Set(GPIOH,11,14);		//PH11,AF14
  GPIO_AF_Set(GPIOH,12,14);		//PH12,AF14
  GPIO_AF_Set(GPIOH,13,14);		//PH13,AF14
  GPIO_AF_Set(GPIOH,14,14);		//PH14,AF14
  GPIO_AF_Set(GPIOH,15,14);		//PH15,AF14

  GPIO_AF_Set(GPIOI,0,14);		//PI0,AF14
  GPIO_AF_Set(GPIOI,1,14);		//PI1,AF14
  GPIO_AF_Set(GPIOI,2,14);		//PI2,AF14
  GPIO_AF_Set(GPIOI,4,14);		//PI4,AF14
  GPIO_AF_Set(GPIOI,5,14);		//PI5,AF14
  GPIO_AF_Set(GPIOI,6,14);		//PI6,AF14
  GPIO_AF_Set(GPIOI,7,14);		//PI7,AF14
  GPIO_AF_Set(GPIOI,9,14);		//PI9,AF14
  GPIO_AF_Set(GPIOI,10,14);		//PI10,AF14

  lcdltdc.pwidth=1024;
  lcdltdc.pheight=600;
  lcdltdc.hsw=20;
  lcdltdc.vsw=3;
  lcdltdc.hbp=140;
  lcdltdc.vbp=20;
  lcdltdc.hfp=160;
  lcdltdc.vfp=12;
  // LTDC_Clk_Set(300,25,6);
  LTDC_Clk_Set(300,25,6);		//6=50MHZ
  // LTDC_Clk_Set(300,50,3);
  tempreg=0<<28;
  tempreg|=0<<29;
  tempreg|=0<<30;
  tempreg|=0<<31;
  LTDC->GCR=tempreg;
  tempreg=(lcdltdc.vsw-1)<<0;
  tempreg|=(lcdltdc.hsw-1)<<16;
  LTDC->SSCR=tempreg;

  tempreg=(lcdltdc.vsw+lcdltdc.vbp-1)<<0;
  tempreg|=(lcdltdc.hsw+lcdltdc.hbp-1)<<16;
  LTDC->BPCR=tempreg;

  tempreg=(lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1)<<0;
  tempreg|=(lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1)<<16;
  LTDC->AWCR=tempreg;

  tempreg=(lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1)<<0;
  tempreg|=(lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1)<<16;
  LTDC->TWCR=tempreg;

  LTDC->BCCR=LTDC_BACKLAYERCOLOR;
  //	LTDC_Switch(1);
  LTDC_Switch(1);

  #if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
    ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
    lcdltdc.pixsize=4;
  #else
    //	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
    //ltdc_framebuf[1]=(u32*)&ltdc_lcd_framebuf1;
    lcdltdc.pixsize=2;
  #endif

  LTDC_Layer_Parameter_Config(0,(u32)ltdc_lcd_framebuf, LCD_PIXFORMAT,255,0,6,7,0X000000);
  LTDC_Layer_Window_Config(0,0,0,lcdltdc.pwidth,lcdltdc.pheight,LCD_PIXFORMAT);

  //LTDC_Layer_Parameter_Config(1,(u32)ltdc_framebuf[1],LCD_PIXFORMAT,127,0,6,7,0X000000);
  //LTDC_Layer_Window_Config(1,0,0,lcdltdc.pwidth,lcdltdc.pheight,LCD_PIXFORMAT);



  //	lcddev.width=lcdltdc.pwidth;
  //	lcddev.height=lcdltdc.pheight;
  //	LTDC_Display_Dir(1);
  LTDC_Select_Layer(0);
  LTDC_Clear(0);
  LCD_LED(1);
  //LTDC_Reversal(1);
  // GT911_Init();
  //  LTDC_Clear(DMA2D_BGCOLR_BLUE); 
}

void LTDC_DeInit(void)
{
//   RCC->APB3ENR &= ~(1<<3); // disable LTDC clock
//   RCC->APB3RSTR |= 1 << 3;
//   RCC->APB3RSTR &= ~(1 << 3);

  LTDC_Switch(0);
  GPIO_Set(GPIOF, PIN10, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_NONE);
  GPIO_Set(GPIOG, 3 << 6 | 1 << 11, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_NONE);
  GPIO_Set(GPIOH, 0X7F << 9, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_NONE);
  GPIO_Set(GPIOI, 7 << 0 | 0XF << 4 | 3 << 9, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_NONE);

//   GPIO_AF_Set(GPIOF,10,14);		//PF10,AF14

//   GPIO_AF_Set(GPIOG,6,14);		//PG6,AF14
//   GPIO_AF_Set(GPIOG,7,14);		//PG7,AF14
//   GPIO_AF_Set(GPIOG,11,14);		//PG11,AF14

//   GPIO_AF_Set(GPIOH,9,14);		//PH9,AF14
//   GPIO_AF_Set(GPIOH,10,14);		//PH10,AF14
//   GPIO_AF_Set(GPIOH,11,14);		//PH11,AF14
//   GPIO_AF_Set(GPIOH,12,14);		//PH12,AF14
//   GPIO_AF_Set(GPIOH,13,14);		//PH13,AF14
//   GPIO_AF_Set(GPIOH,14,14);		//PH14,AF14
//   GPIO_AF_Set(GPIOH,15,14);		//PH15,AF14

//   GPIO_AF_Set(GPIOI,0,14);		//PI0,AF14
//   GPIO_AF_Set(GPIOI,1,14);		//PI1,AF14
//   GPIO_AF_Set(GPIOI,2,14);		//PI2,AF14
//   GPIO_AF_Set(GPIOI,4,14);		//PI4,AF14
//   GPIO_AF_Set(GPIOI,5,14);		//PI5,AF14
//   GPIO_AF_Set(GPIOI,6,14);		//PI6,AF14
//   GPIO_AF_Set(GPIOI,7,14);		//PI7,AF14
//   GPIO_AF_Set(GPIOI,9,14);		//PI9,AF14
//   GPIO_AF_Set(GPIOI,10,14);		//PI10,AF14
}
