#ifndef _GT911_H_
#define _GT911_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"
    
#define IIC_SCL(x)    digitalWrite(PE2,x)		//SCL //SCL
#define IIC_SDA(x)    digitalWrite(PE6,x)		//SDA	 
#define READ_SDA      digitalRead(PE6)  
// #define READ_PEN      GPIO_Pin_Get(GPIOE,PIN3)
#define READ_PEN      digitalRead(PE3)

#define GT_CMD_WR         0X28         
#define GT_CMD_RD         0X29       
   
#define GT_CTRL_REG      0X8040      
#define GT_CFGS_REG      0X8047       
#define GT_CHECK_REG     0X80FF       
#define GT_PID_REG       0X8140      
#define GT_GSTID_REG     0X814E

typedef struct _POINT{
  u16 x;
  u16 y;
}POINT, *PPOINT;

typedef struct _GT911_POINT_DATA{
  u8 cnt;
  POINT points[5];
}GT911_POINT_DATA, *PGT911_POINT_DATA;

extern GT911_POINT_DATA     gt911_data;

extern u8 data[0X40];

u8 GT911_Init(void);
void GT911_DeInit(void);
u8 ReadID(void);
void READXY();
void GetPointData(u8 cnt, u8 data[]);
u8 GT911_Send_Cfg(u8 mode);
void GT911_RD_Reg(u16 reg,u8 *buf,u8 len);
u8 GT911_WR_Reg(u16 reg,u8 *buf,u8 len);

u8 ReadRelasePoint(u16 *px,u16 *py);
u8 ReadPressPoint(u16 *px,u16 *py);


#ifdef __cplusplus
}
#endif

#endif
