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
 * @file w6502.c w6502 run loop and debugger
 */

#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "w6502.h"
#include "bus.h"
#include "disasm.h"
#include "uart.h"
#include "dmarq.h"
#include "util.h"

#ifdef HPDL1414
#include "hpdl1414.h"
#endif


/**
 * Breakpoints and watch names
 */
const char debug_names[] PROGMEM = "memrd\0memwr\0opfetch\0bus\0dma";

/**
 * Breakpoint and watch ranges
 */
range breaks[] = {{0xffff, 0}, {0xffff, 0}, {0xffff, 0}, {0xffff, 0}, {0xffff, 0}};
range watches[] = {{0xffff, 0}, {0xffff, 0}, {0xffff, 0}, {0xffff, 0}, {0xffff, 0}};

/**
 * Whether to stop when the halt signal occurs
 */
//uint8_t halt_mask = RESET | HALT;

/**
 * Reset the W6502 to a specified address
 */
void w6502_reset(uint32_t addr)
{
    uint8_t reset_vect[] = { (addr & 0xFF), ((addr >> 8) & 0xFF) };
    mem_write_banked(0xfffc, reset_vect, 2);

    bus_release();

    RESET_OUTPUT;
    RESET_LO;
    clk_cycle(3);
    RESET_HI;
    RESET_INPUT;
    clk_cycle(7);  // forward 7 cycle for vector fetch

    bus_request();
#if defined(BANK_PORT) || defined(BANK_BASE)
    mem_bank_addr(base_addr);
#endif

}

/**
 * Run the W6502 at full speed
 */
void w6502_run(void)
{
    watch_flag = 0;
    bus_release();
    clk_run();

    for (;;) {
      while (GET_RDY) {
	if (watch_flag)
	  break;
      }
      clk_stop();
      bus_stat status = bus_status();
      //PHI2_LO;
      bus_request();
      if (watch_flag) {
	  printf_P(PSTR("Keyboard HALT at $%04x\n"), status.addr);
	  return;
      }
      // DMA request
      uint8_t rtn = dma_dispatch(status.addr-3); // use relative address for dmareq

      IRQ_OUTPUT;
      IRQ_LO;
      bus_release();
      PHI2_HI;
      PHI2_LO;
      IRQ_HI;
      IRQ_INPUT;

      if (rtn == DMA_RETURN_EXIT) {
	printf_P(PSTR("HALT REQUEST at $%04x\n"), status.addr);
	// generate dummy IRQ 
	// bus_request();
	break;
      } else {
	clk_run();
      }
    }
    PHI2_LO;
    while (!GET_RESET)
        ;
    bus_request();
}

/**
 * Run the W6502 with watches and breakpoints for a specified number of instructions
 */
void w6502_debug(uint32_t cycles)
{

  watch_flag = 0;
  uint8_t bus_available = 0;

  if (cycles > 0)
    cycles--; 

  bus_release();
  for (;;) {
    
    if (watch_flag)
      break;

    PHI2_HI;
    //PHI2_LO;
    bus_stat status = bus_status();
    PHI2_LO;

    if (SYNC_STATUS) { // opcode fetch
      if (INRANGE(breaks, OPFETCH, status.addr)) { // break
	printf_P(PSTR("break at $%04x :opcode fetch\n"), status.addr);
	bus_request();
	bus_available = 1;
	disasm_mem(status.addr, status.addr);
	uart_flush();
	break;
      } else if (INRANGE(watches, OPFETCH, status.addr)) {  // watch
	bus_request();
	bus_available = 1;
	disasm_mem(status.addr, status.addr);
	uart_flush();
      }
    }

    if (RW_STATUS) { // memory read
      if (INRANGE(breaks, MEMRD, status.addr)) {
	printf_P(PSTR("break at $%04x :memrd\n"), status.addr);
	bus_log(status);
#ifdef HPDL1414
	hpdl1414_disp(bus_status());
#endif
	uart_flush();
	break;
      } else if (INRANGE(watches, MEMRD, status.addr)) {
	bus_log(status);
#ifdef HPDL1414
	hpdl1414_disp(bus_status());
#endif
	uart_flush();
      } else if (INRANGE(watches, BUS, status.addr)) {
	bus_log(status);
#ifdef HPDL1414
	hpdl1414_disp(bus_status());
#endif
	uart_flush();
      }
    } else { // memory write
      if (INRANGE(breaks, MEMWR, status.addr)) {
	printf_P(PSTR("break at $%04x :memwe\n"), status.addr);
	bus_log(status);
#ifdef HPDL1414
	hpdl1414_disp(bus_status());
#endif
	uart_flush();
	break;
      } else if (INRANGE(watches, MEMWR, status.addr)) {
	bus_log(status);
#ifdef HPDL1414
	hpdl1414_disp(bus_status());
#endif
	uart_flush();
      } else if (INRANGE(watches, BUS, status.addr)) {
	bus_log(status);
#ifdef HPDL1414
	hpdl1414_disp(bus_status());
#endif
	uart_flush();
      }
    }

    // handle DMA REQUEST
    if (!RDY_STATUS) { // dma request
      if(dma_debug)
	printf_P(PSTR("%04x WAI($CB) request\n"), status.addr-1);
      
      if (bus_available == 0)
	bus_request();
      uint8_t rtn = dma_dispatch(status.addr-3); // use relative address for dmareq
      uart_flush();

      IRQ_OUTPUT;
      IRQ_LO;
      bus_release();
      bus_available = 0;
      PHI2_HI;
      PHI2_LO;

      IRQ_HI;
      IRQ_INPUT;

      if (rtn == DMA_RETURN_EXIT) {
	printf_P(PSTR("HALT REQUEST at $%04x\n"), status.addr);
	break;
      }
      if (dma_debug)
	printf_P(PSTR("DMA finished\n"));
      //      bus_log(status);
    }

    if (bus_available == 1)
      bus_release();

    if (cycles-- == 0)
      break;
  }
  
  bus_request();

}
