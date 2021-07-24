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
 * @file bus.c Low-level control of the Z80 bus
 */

#include "bus.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#include <stdio.h>

/**
 * Run the clock for a specified number of cycles
 */
void clk_cycle(uint8_t cycles)
{
    uint8_t i;
    for (i = 0; i < cycles; i++) {
      PHI2_HI; //        CLK_HI;
      PHI2_LO; //        CLK_LO;
    }
}

uint8_t clkdiv = 8; // 2MHz

/**
 * Run the w6502's clock
 */
void clk_run(void)
{
    // Fast PWM mode with adjustable top and no prescaler
    TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B |= (1 << WGM22) | (1 << CS20);
    OCR2A = (clkdiv - 1);
    OCR2B = (clkdiv - 1) >> 1;
}

/**
 * Stop the w6502's clock
 */
void clk_stop()
{
    TCCR2A = 0;
    TCCR2B = 0;
    OCR2A = 0;
    OCR2B = 0;
}

/**
 *  Request control of the bus from the w6502
 */
uint8_t bus_request(void)
{
  uint8_t i = 255;

  if (GET_RDY) {  // not halted
    RDY_OUTPUT;
    RDY_LO;
    while (GET_RDY) {
      //PHI2_TOGGLE;
      if (i-- == 0) {
	printf_P(PSTR("HALT request timed out\n"));
	RDY_INPUT;
	return 0;
      }
    }
  }
  BE_LO;  // cpu bus HI-Z
  RAME_OUTPUT;
  RAME_HI; // disable ram
  RW_OUTPUT;
  ADDR_OUTPUT;
  DATA_INPUT;
  return 1;
}

/**
 *  Return control of the bus to the w6502
 */
void bus_release(void)
{
    uint8_t i = 255;

    RW_INPUT;
    ADDR_INPUT;
    DATA_INPUT;
    SET_DATA(0); // Disable pullups on data and address lines
    SET_ADDRLO(0);
    BE_HI;  // cpu bus active
    RAME_LO;  // enable ram

    RDY_INPUT;
    // Clock the w6502 until it takes back control of the bus
    while (!GET_RDY)  {
      PHI2_TOGGLE;
      if (i-- == 0) {
	printf_P(PSTR("bus release timed out\n"));
	return;
      }
    }
}

/**
 * Retrieve the current bus status
 */
bus_stat bus_status(void)
{
    bus_stat status;
    status.flags = CONT_PIN;
    status.flags2 = (PING & CTRLG_MASK) | (PINH & CTRLH_MASK);
    status.data = GET_DATA;
    status.addr = GET_ADDR;
    return status;
}

/**
 * Log the bus status
 */
void bus_log(bus_stat status)
{
    printf_P(
        PSTR("\t%04x %02x %c    %s %s    %s %s %s %s %s %s %s %s\n"),
        status.addr,
        status.data,
        0x20 <= status.data && status.data <= 0x7e ? status.data : ' ',
        PHI2_STATUS ? "PH-H" : "PH-L",
        RW_STATUS ? "rd" : "wr",
        !VP_STATUS ? "/vp" : "   ",
        SYNC_STATUS ? "sync" : "    ",
        RDY_STATUS ? "rdy" : "hlt",
        !BE_STATUS ? "/be" : "   ",
        !MLB_STATUS ? "/ml" : "   ",
        !RAME_STATUS ? "/rame" : "     ", 
        !IRQ_STATUS ? "/irq" : "    ",
        !NMI_STATUS ? "/nmi" : "    ",
        !RESET_STATUS ? "/res" : "    ");
}

/**
 * Initialize the bus
 */
void bus_init(void)
{
    // Initialize control signals
    CTRLH_OUTPUT_INIT;
    CTRLK_OUTPUT_INIT;
    CTRLG_OUTPUT_INIT;

    // Reset the processor
    RESET_OUTPUT;
    RESET_LO;
    clk_cycle(8);
    RESET_HI;
    RESET_INPUT;

    // Start out in control of the bus
    bus_request();
}


uint32_t base_addr = 0;

/**
 * Bank in the page at the specified address
 */
#ifdef BANK_PORT
uint8_t mem_banks = 0;

void mem_bank_addr(uint32_t addr)
{
    mem_banks = (addr >> 15) & 0x0e;
    mem_banks |= (mem_banks + 1) << 4;
    io_out(BANK_PORT, mem_banks);
}
#elif defined(BANK_BASE)
#define BANK_ENABLE (BANK_BASE + 4)
uint8_t mem_banks[] = {0, 0, 0, 0};
void mem_bank(uint8_t bank, uint8_t addr)
{
    bank &= 3;
    addr &= 0x3f;
    io_out(BANK_ENABLE, 1);
    io_out(BANK_BASE + bank, addr);
    mem_banks[bank] = addr;
}

void mem_bank_addr(uint32_t addr)
{
    uint8_t page = (addr >> 14) & 0x3c;
    for (uint8_t i = 0; i < 4; i++)
        mem_bank(i, page + i);
}
#endif

void mem_read_page(uint8_t start, uint8_t end, void *buf)
{
  //    if (GET_BUSACK)
  //        return;
    uint8_t *bufbyte = buf;
    DATA_INPUT;
    PHI2_LO;
    RW_HI;
    RAME_LO;
    do {
        SET_ADDRLO(start++);
        PHI2_HI;
        _NOP();
        _NOP();
        *bufbyte++ = GET_DATA;
	PHI2_LO;
    } while (start <= end && start != 0);
    RAME_HI;
}

void mem_write_page(uint8_t start, uint8_t end, void *buf)
{
//    if (GET_BUSACK)
//        return;
    uint8_t *bufbyte = buf;
    DATA_OUTPUT;
    PHI2_LO;
    RW_LO;
    RAME_LO;
    do {
        PHI2_HI;
        SET_ADDRLO(start++);
        SET_DATA(*bufbyte++);
        PHI2_LO;
    } while (start <= end && start != 0);
    RAME_HI;
    DATA_INPUT;
}

void mem_write_page_P(uint8_t start, uint8_t end, void *buf)
{
  //    if (GET_BUSACK)
  //        return;
    uint8_t *bufbyte = buf;
    DATA_OUTPUT;
    PHI2_LO;
    RW_LO;
    RAME_LO;
    do {
        PHI2_LO;
        SET_ADDRLO(start++);
        SET_DATA(pgm_read_byte(bufbyte++));
        PHI2_HI;
    } while (start <= end && start != 0);
    RAME_HI;
    DATA_INPUT;
}

void mem_iterate(uint16_t start, uint16_t end, pagefunc_t dopage, void *buf)
{
  //    if (GET_BUSACK)
  //        return;
    uint8_t starthi = start >> 8;
    uint8_t startlo = start & 0xff;
    uint8_t endhi = end >> 8;
    uint8_t endlo = end & 0xff;
    //    RAME_LO;
    //    PHI2_LO;
    //    MREQ_LO;
    SET_ADDRHI(starthi);
    if (starthi == endhi) {
        dopage(startlo, endlo, buf);
    } else {
        dopage(startlo, 0xff, buf);
        buf += 0x100 - startlo;
        starthi++;
        while (starthi < endhi) {
            SET_ADDRHI(starthi++);
            dopage(0, 0xff, buf);
            buf += 0x100;
        }
        SET_ADDRHI(starthi);
        dopage(0, endlo, buf);
    }
    //    PHI2_HI;
    //    RAME_HI;
    //    MREQ_HI;
}

void mem_iterate_banked(uint32_t start, uint32_t end, pagefunc_t dopage, void *buf)
{
  //if (GET_BUSACK)
  //        return;
    start += base_addr;
    end += base_addr;
    mem_bank_addr(start);
    if ((start & 0x10000) == (end & 0x10000)) {
        mem_iterate(start & 0xffff, end & 0xffff, dopage, buf);
    } else {
        mem_iterate(start & 0xffff, 0xffff, dopage, buf);
        buf += 0x10000 - start;
        mem_bank_addr(end);
        mem_iterate(0, end & 0xffff, dopage, buf);
    }
}

