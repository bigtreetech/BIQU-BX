#ifndef _SDRAM_H_
#define _SDRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"

#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) 		

u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
void SDRAM_RGBLCD_Set(void);	
void SDRAM_Init(void);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);

//void SARAM_REST(void);
#ifdef __cplusplus
}
#endif

#endif


