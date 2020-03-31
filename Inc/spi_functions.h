//
// Created by macros on 23.03.20.
//

#ifndef SPI_FUNCTIONS_H
#define SPI_FUNCTIONS_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>

void spiInit (SPI_TypeDef * SPIx);
void write(uint8_t data);
uint8_t read();

#endif
