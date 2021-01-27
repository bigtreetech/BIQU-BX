#include "sw_spi.h"
#include "../../../ui_api.h"

void SW_SPI_Config(_SW_SPI *sw_spi, _SPI_MODE mode, uint8_t dataSize,
uint16_t  cs,
uint16_t  sck,
uint16_t  miso,
uint16_t  mosi)
{
  sw_spi->cs = cs;
  sw_spi->sck = sck;
  sw_spi->miso = miso;
  sw_spi->mosi = mosi;
  sw_spi->mode = mode;
  sw_spi->dataSize = dataSize;

  OUT_WRITE(sw_spi->cs, 1);   // CS
  OUT_WRITE(sw_spi->sck, 1);  // SCK
  OUT_WRITE(sw_spi->mosi, 0); // MOSI
  SET_INPUT(sw_spi->miso);    // MISO
}

#define SCK_HIGH()    WRITE(sw_spi->sck, 1); ExtUI::delay_us(2) // The speed of xpt2046 should not be too fast
#define SCK_LOW()     WRITE(sw_spi->sck, 0); ExtUI::delay_us(2) // It is better to be below 200KHz.
#define MOSI_WRITE(n) WRITE(sw_spi->mosi, n)
#define MISO_READ()   READ(sw_spi->miso)

uint16_t SW_SPI_Read_Write(_SW_SPI *sw_spi, uint16_t d)
{
  uint8_t data_bits = sw_spi->dataSize;
  uint8_t i=0;
  uint16_t rcv=0;
  switch(sw_spi->mode)
  {
    case _SPI_MODE0:
      for(i=0; i<data_bits; i++)
      {
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_HIGH();
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_LOW();
      }
      break;
    case _SPI_MODE1:
      for(i=0; i<data_bits; i++)
      {
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_HIGH();
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_LOW();
      }
      break;
    case _SPI_MODE2:
      for(i=0; i<data_bits; i++)
      {
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_LOW();
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_HIGH();
      }
      break;
    case _SPI_MODE3:
      for(i=0; i<data_bits; i++)
      {
        MOSI_WRITE(d&0x80);
        d <<= 1;
        SCK_LOW();
        rcv <<=1;
        rcv |= MISO_READ();
        SCK_HIGH();
      }
      break;
  }
  return rcv;
}

void SW_SPI_CS_Set(_SW_SPI *sw_spi, uint8_t level)
{
  WRITE(sw_spi->cs, level);
}
