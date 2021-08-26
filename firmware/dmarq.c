/* 6502ctrl (https://github.com/kuwatay/6502cttl)
 * Copyright 2021 Yoshitaka Kuwata a.k.a. morecat_lab
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
 * @file dmarq.c DMA request handler
 */

#include "dmarq.h"
#include "uart.h"
#include "bus.h"
//#include "iox.h"
#include "util.h"
//#include "diskemu.h"
#include "sioemu.h"
#include "filedma.h"
//#include "bdosemu.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>

uint8_t dma_debug = 0;

void dump_dmarq(dmarq_t *dma) {
  printf_P(PSTR("dmarq %04x\n"), dma);
  printf_P(PSTR("dmano %02x\n"), dma->dmano);
  printf_P(PSTR("dmacmd %02x\n"), dma->dmacmd);
}

/**
 * Handle DMA request
 */
uint8_t dma_dispatch(uint16_t dmap)
{
  uint16_t dmarq_addr;
  dmarq_t dmarq;

  //read memory
  mem_read(dmap, &dmarq_addr, sizeof(uint16_t)); // read dmarq address
  mem_read(dmarq_addr, &dmarq, sizeof(dmarq_t)); // read struct dmarq

  if (dma_debug) {
    printf_P(PSTR("dmap %04x\n"), dmap);
    printf_P(PSTR("dmarq_addr %04x\n"), dmarq_addr);
    printf_P(PSTR("dmano %02x\n"), dmarq.dmano);
    printf_P(PSTR("dmacmd %02x\n"), dmarq.dmacmd);
    printf_P(PSTR("mbadrs %04x\n"), dmarq.mbadrs);
    printf_P(PSTR("dmadat %02x\n"), dmarq.dmadat);
  }
  
  switch (dmarq.dmano) {
  case DMA_HALT:
    return DMA_RETURN_EXIT;
  case DMA_SIO:
    switch (dmarq.dmacmd) {
    case DMA_RESET:
      return DMA_RETURN_OK; // do nothing
    case DMA_READ:
      dmarq.dmadat = sio0_read();
      break;
    case DMA_WRITE:
      {
	uint8_t data = dmarq.dmadat;
	sio0_write(data);
      }
      return DMA_RETURN_OK;
    case DMA_STATUS:
      dmarq.dmadat = sio0_status();
      break;
    case DMA_READ_BLOCK:
      while ((dmarq.dmadat = sio0_read()) == 0)
	;
      break;
    default: // ERROR
      return DMA_RETURN_ERROR;
    }
    // wtire back to dmarq to memory
    mem_write(dmarq_addr, &dmarq, sizeof(dmarq_t)); // write struct dmarq
    return DMA_RETURN_OK;

    break;
  case DMA_FILEDMA:  // not implemented
    break;
  case DMA_BDOSEMU:  // not implemented
    break;
  default:
    return DMA_RETURN_ERROR; // ERROR
  }
}

