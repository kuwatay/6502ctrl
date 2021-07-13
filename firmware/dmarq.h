/* z80ctrl (https://github.com/jblang/z80ctrl)
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
 * @file dma.c DMA request handler
 */

#ifndef DMARQ_H
#define DMARQ_H

#include <stdint.h>
//#include "bus.h"

typedef struct {
  uint8_t dmano;
  uint8_t dmacmd;
  uint16_t mbadrs;
  uint8_t dmadat;
} dmarq_t;


typedef enum {
  DMA_HALT,
  DMA_SIO,
  DMA_FILEDMA,
  DMA_BDOSEMU,
  DMA_IOX,
} dma_no;

typedef enum {
  DMA_RESET = 1,
  DMA_READ,
  DMA_WRITE,
  DMA_STATUS,
  DMA_READ_BLOCK
} dma_cmd;

typedef enum {
  DMA_RETURN_OK = 0,
  DMA_RETURN_ERROR,
  DMA_RETURN_EXIT = 0xff,
} dma_return;

uint8_t dma_dispatch(uint16_t dma);

extern uint8_t dma_debug;

#endif // DMARQ_H
