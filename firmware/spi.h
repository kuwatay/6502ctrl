/* 6502ctrl (https://github.com/kuwatay/6502cttl)
 * Copyright 2021 Yoshiyaka Kuwata a.k.a. morecat_lab
 *
 * based on z80ctrl (https://github.com/jblang/z80ctrl)
 * Copyright 2018 J.B. Langston
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file spi.h SPI chip select pin definitions
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <avr/io.h>

// SPI pins
#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SPI_PIN PINB

#define SCK 1
#define MISO 3
#define MOSI 2
#define SS  0

#define SD_SEL SPI_PORT &= ~(1 << SS)
#define SD_UNSEL SPI_PORT |= (1 << SS)

#define SPI_SLOW SPCR |= ((1 << SPR1) | (1 << SPR0));
#define SPI_FAST SPCR &= ~((1 << SPR1) | (1 << SPR0))

#define SPI_PHASE0 SPCR &= ~(1 << CPHA)
#define SPI_PHASE1 SPCR |= (1 << CPHA)

#define SPI_ENABLE SPCR |= (1 << SPE)
#define SPI_DISABLE SPCR &= ~(1 << SPE)

#define MISO_INPUT SPI_DDR &= ~(1 << MISO)
#define MISO_OUTPUT SPI_DDR |= (1 << MISO)
#define MISO_LO SPI_PORT &= ~(1 << MISO)
#define MISO_HI SPI_PORT |= (1 << MISO)
#define GET_MISO (SPI_PIN & (1 << MISO))

#define SCK_LO SPI_PORT &= ~(1 << SCK)
#define SCK_HI SPI_PORT |= (1 << SCK)

void spi_init();
uint8_t spi_exchange(uint8_t val);

#endif
