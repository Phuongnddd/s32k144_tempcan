#ifndef SPI_H_
#define SPI_H_

#include "S32K144.h"
#include <stdint.h>


void SPI_Init(void);

void SPI_WriteByte(
        uint8_t data);

void SPI_DelayShort(void);


#endif
