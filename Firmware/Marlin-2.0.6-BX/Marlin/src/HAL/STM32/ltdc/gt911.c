#include "Arduino.h"
#include "sys.h" 
#include "gt911.h"
#include "string.h"
#include "ltdc.h"

u16 xsave=0,ysave=0;
u8 data[0X40];

GT911_POINT_DATA  gt911_data;
		
void SDA_IN(void)
{
   GPIO_Set(GPIOE,PIN6,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
}

void SDA_OUT(void)
{
   GPIO_Set(GPIOE,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);
}

void IIC_Init(void)
{                         
   pinMode(PE2,OUTPUT);
   pinMode(PE6,OUTPUT);
	 IIC_SDA(1);	  	  
	 IIC_SCL(1);
}

void IIC_Start(void)
{
     SDA_OUT();    
     IIC_SDA(1);            
     IIC_SCL(1);
     delayMicroseconds(4);
     IIC_SDA(0);
     delayMicroseconds(4);
     IIC_SCL(0);
}      

void IIC_Stop(void)
{
     SDA_OUT();
     IIC_SCL(0);
     IIC_SDA(0);
     delayMicroseconds(4);
     IIC_SCL(1); 
     IIC_SDA(1);
     delayMicroseconds(4);                                   
}

u8 IIC_Wait_Ack(void)
{
     u8 ucErrTime=0;
     SDA_IN();
     IIC_SDA(1);delayMicroseconds(1);       
     IIC_SCL(1);delayMicroseconds(1);     
     while(READ_SDA)
     {
         ucErrTime++;
         if(ucErrTime>250)
         {
             IIC_Stop();
             return 1;
         }
     }
     IIC_SCL(0);   
     return 0;  
} 

void IIC_Ack(void)
{
     IIC_SCL(0);
     SDA_OUT();
     IIC_SDA(0);
     delayMicroseconds(2);
     IIC_SCL(1);
     delayMicroseconds(2);
     IIC_SCL(0);
}

void IIC_NAck(void)
{
     IIC_SCL(0);
     SDA_OUT();
     IIC_SDA(1);
     delayMicroseconds(2);
     IIC_SCL(1);
     delayMicroseconds(2);
     IIC_SCL(0);
}                                          

void IIC_Send_Byte(u8 txd)
{                        
     u8 t;   
     SDA_OUT();
     IIC_SCL(0);
     for(t=0;t<8;t++)
     {              
         if((txd&0x80)>>7)
             IIC_SDA(1);
         else
             IIC_SDA(0);
         txd<<=1;
         delayMicroseconds(2);  
         IIC_SCL(1);
         delayMicroseconds(2); 
         IIC_SCL(0);    
         delayMicroseconds(2);
     }     
}         

u8 IIC_Read_Byte(unsigned char ack)
{
     unsigned char i,receive=0;
     SDA_IN();
     for(i=0;i<8;i++ )
     {
         IIC_SCL(0); 
         delayMicroseconds(2);
         IIC_SCL(1);
         receive<<=1;
         if(READ_SDA)receive++;   
         delayMicroseconds(1); 
     }                     
     if (!ack)
         IIC_NAck();
     else
         IIC_Ack(); 
     return receive;
}

void GT911_INT_Init(void)
{  
     pinMode(PE3,OUTPUT);
}

void GT911_INT(u8 cmd)
{
     if(cmd) digitalWrite(PE3,1); 
     else     digitalWrite(PE3,0); 
}

void GT911_INT_Change(void)
{
    pinMode(PE3,INPUT);
}

void GT911_RST_Init(void)
{
	pinMode(PE4,OUTPUT);
	RCC->AHB4ENR|=1<<4;		
	GPIO_Set(GPIOE,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);
	GPIO_Pin_Set(GPIOE,PIN4,0); 
}

void GT911_RST(u8 cmd)
{
        if(cmd) digitalWrite(PE4,1); 
     else     digitalWrite(PE4,0); 
}

u8 GT911_WR_Reg(u16 reg,u8 *buf,u8 len)
{
     u8 i;
     u8 ret=0;
     IIC_Start();
     IIC_Send_Byte(GT_CMD_WR);       
     IIC_Wait_Ack();
     IIC_Send_Byte(reg>>8);       
     IIC_Wait_Ack();                                                           
     IIC_Send_Byte(reg&0XFF);       
     IIC_Wait_Ack();  
     for(i=0;i<len;i++)
     {       
         IIC_Send_Byte(buf[i]);     
         ret=IIC_Wait_Ack();
         if(ret)break;  
     }
     IIC_Stop();                   
     return ret; 
}

void GT911_RD_Reg(u16 reg,u8 *buf,u8 len)
{
     u8 i; 
      IIC_Start();    
      IIC_Send_Byte(GT_CMD_WR);   
     IIC_Wait_Ack();
      IIC_Send_Byte(reg>>8);       
     IIC_Wait_Ack();                                                           
      IIC_Send_Byte(reg&0XFF);      
     IIC_Wait_Ack();  
      IIC_Start();              
     IIC_Send_Byte(GT_CMD_RD);        
     IIC_Wait_Ack();       
     for(i=0;i<len;i++)
     {       
         buf[i]=IIC_Read_Byte(i==(len-1)?0:1); 
     } 
     IIC_Stop();   
}

u8 ReadID(void)
{
     char tmp[4]={0};
    GT911_RD_Reg(GT_PID_REG, tmp, 4); 
         if(strcmp(tmp,"911")==0)
         {
             return 1;
         }
         else
         {
             return 0;
         }
}

u8 GT911_Init(void)
{
  GT911_RST_Init();
  GT911_INT_Init();
  IIC_Init();

  GT911_RST(0);
  GT911_INT(1);
  delayMicroseconds(200);
  GT911_RST(1);
  delay(6);
  GT911_INT(0);
  delay(55);
  GT911_INT_Change();
  delay(50);
  return ReadID();
}

void GT911_DeInit(void)
{
  GT911_RST_Init();
  GT911_INT_Init();
  GT911_RST(0);
  GT911_INT(1);
  delayMicroseconds(200);
  GT911_RST(1);
  delay(6);
  GT911_INT(0);
  delay(55);
  GT911_INT_Change();
  delay(50);
  pinMode(PE2,INPUT);
  pinMode(PE3,INPUT);
  pinMode(PE4,INPUT);
  pinMode(PE6,INPUT);
}

void GetPointData(u8 cnt, u8 data[])
{
     gt911_data.cnt = 0;
     switch(cnt){
         case 5:
             gt911_data.points[4].x = data[0x31]<<8 | data[0x30];
             gt911_data.points[4].y = data[0x33]<<8 | data[0x32];        
         case 4:
             gt911_data.points[3].x = data[0x29]<<8 | data[0x28];
             gt911_data.points[3].y = data[0x2B]<<8 | data[0x2A];        
         case 3:
             gt911_data.points[2].x = data[0x21]<<8 | data[0x20];
             gt911_data.points[2].y = data[0x23]<<8 | data[0x22];
         case 2:
             gt911_data.points[1].x = data[0x19]<<8 | data[0x18];
             gt911_data.points[1].y = data[0x1B]<<8 | data[0x1A];
         case 1:
             gt911_data.points[0].x = data[0x11]<<8 | data[0x10];
             gt911_data.points[0].y = data[0x13]<<8 | data[0x12];            
             break;
         default:
             break;
     }

    gt911_data.cnt = cnt;
}

void ShowPointData(void)
{
	  u8 i;
     u8 cnt = gt911_data.cnt;
     for(i=0; i<cnt; i++){
       //  printf("Point%d(%d,%d)\t", i+1, gt911_data.points[i].x, gt911_data.points[i].y);
     }
  //   printf("\r\n");
     memset(&gt911_data, 0, sizeof(gt911_data));
}

void READXY()
{  
	u8 tmp;
  if(READ_PEN==0)
	{
		delay(10);
		while(READ_PEN==0)
		{}
	 
     GT911_RD_Reg(0x8140, data, 0x40);
     tmp = data[0x0E];
     if((tmp&0x80) && ((tmp&0x0f)>0)){
         GetPointData(tmp&0x0f, data);
         ShowPointData();
     }
     tmp = 0;
     GT911_WR_Reg(0x814E, &tmp, 1);
	}
}
u8 ReadPressPoint(u16 *px,u16 *py)
{
   u8 tmp;
   u8 staus=0;//key yes/no
	 		
  GT911_RD_Reg(0x814E,&staus,1);
  if(staus==0x81)
  {
      GT911_RD_Reg(0x8140, data, 0x40);
        tmp = data[0x0E];
        if((tmp&0x80) && ((tmp&0x0f)>0)){
                GetPointData(tmp&0x0f, data);		
        }
        tmp = 0;
          GT911_WR_Reg(0x814E, &tmp, 1);
         *px=gt911_data.points[0].x;
		 *py=gt911_data.points[0].y;
          return 1;//sucess
  }
  else
  {
    tmp = 0;
	  GT911_WR_Reg(0x814E, &tmp, 1);
    return 0;
  }
  
}
//return 0 No saved value after not clicking or releasing
//return 1 Clicking
//return 2 It has been released and has a saved value
u8 ReadRelasePoint(u16 *px,u16 *py)
{
   u8 tmp;
   u8 staus=0;//key yes/no
	 		
  GT911_RD_Reg(0x814E,&staus,1);
	
	if((staus&0x80)==0x80)
	{
	  switch (staus)
      {
       case   0x81:
            GT911_RD_Reg(0x8140, data, 0x40);
            tmp = data[0x0E];
            if((tmp&0x80) && ((tmp&0x0f)>0)){
            GetPointData(tmp&0x0f, data);		
            }
            if(ltdc_dir==1)
            {
            *px=gt911_data.points[0].x;
            *py=gt911_data.points[0].y;
            }
            else
            {
            *px=1024-gt911_data.points[0].x;
            *py=600-gt911_data.points[0].y;
            }
            tmp = 0;
            GT911_WR_Reg(0x814E, &tmp, 1);
		   return 1;
      	
      	case   0x80:   
            if((xsave!=gt911_data.points[0].x)&&(ysave!=gt911_data.points[0].y))
            {
                if(ltdc_dir==1)
                {
                *px=gt911_data.points[0].x;
                *py=gt911_data.points[0].y;
                }
                else
                {
                *px=1024-gt911_data.points[0].x;
                *py=600-gt911_data.points[0].y;
                }
                tmp = 0;
                GT911_WR_Reg(0x814E, &tmp, 1);
                xsave=*px;
                ysave=*py;
                return 2;
            }
            else
            {
                tmp = 0;
                GT911_WR_Reg(0x814E, &tmp, 1);
                return 0;
            }	 
      	default:
            tmp = 0;
            GT911_WR_Reg(0x814E, &tmp, 1);
      		break;
      }
	}
	else
	{
		xsave=0;
		ysave=0;
	return 0;
	}
}

