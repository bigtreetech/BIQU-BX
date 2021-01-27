#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SPI_FRE_DIV_2   0
#define SPI_FRE_DIV_4   1
#define SPI_FRE_DIV_8   2
#define SPI_FRE_DIV_16  3
#define SPI_FRE_DIV_32  4
#define SPI_FRE_DIV_64  5
#define SPI_FRE_DIV_128 6
#define SPI_FRE_DIV_256 7

void SPI3_CS_Set(uint8_t level);
void SPI3_Init(void);
void SPI3_SetSpeed(uint8_t SpeedSet);
uint8_t SPI3_ReadWriteByte(uint8_t TxData);

#ifdef __cplusplus
}
#endif

#endif
