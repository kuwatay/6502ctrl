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
 * @file bus.h Low-level control of the Z80 bus
 */

#ifndef BUS_H
#define BUS_H

#include <avr/io.h>

/**
 * Address Bus
 */

#define ADDRLO_DDR DDRA
#define ADDRLO_PORT PORTA
#define ADDRLO_PIN PINA

#ifdef MEGA
#define ADDRHI_DDR DDRK
#define ADDRHI_PORT PORTK
#define ADDRHI_PIN PINK
#else
#define ADDRHI_DDR DDRC
#define ADDRHI_PORT PORTC
#define ADDRHI_PIN PINC
#endif

#define ADDR_INPUT (ADDRLO_DDR = 0x00, ADDRHI_DDR = 0x00)
#define ADDR_OUTPUT (ADDRLO_DDR = 0xFF, ADDRHI_DDR = 0xff)

#define GET_ADDRLO ADDRLO_PIN
#define SET_ADDRLO(addr) ADDRLO_PORT = (addr)

#define GET_ADDRHI ADDRHI_PIN
#define SET_ADDRHI(addr) ADDRHI_PORT = (addr)

#define GET_ADDR (GET_ADDRLO | (GET_ADDRHI << 8))
#define SET_ADDR(addr) (SET_ADDRLO((addr) & 0xFF), SET_ADDRHI((addr) >> 8))

/**
 * Data Bus
 */

#define DATA_DDR DDRL
#define DATA_PORT PORTL
#define DATA_PIN PINL

#define DATA_INPUT DATA_DDR = 0x00
#define DATA_OUTPUT DATA_DDR = 0xFF
#define GET_DATA DATA_PIN
#define SET_DATA(data) DATA_PORT = (data)

/**
 * PORTG Flags
 */
#define RAME (1 << 2)
#define RAME_HI PORTG |= RAME
#define RAME_LO PORTG &= ~RAME
#define RAME_OUTPUT DDRG |= RAME
#define RAME_INPUT DDRG &= ~RAME
#define RAME_STATUS (status.flags2 & RAME)

#define RW (1 << 1)
#define GET_RW (PING & RW)
#define RW_HI PORTG |= RW
#define RW_LO PORTG &= ~RW
#define RW_OUTPUT DDRG |= RW
#define RW_INPUT DDRG &= ~RW
#define RW_STATUS (status.flags2 & RW)

#define CTRLG_OUTPUT_INIT DDRG |= (RW | RAME)
#define CTRLG_MASK (RAME | RW)
/**
 * CONTROL_PORT Flags
 */
#ifdef MEGA
// Arduino Mega Shield version
#define CONT_PORT PORTF
#define CONT_DDR  DDRF
#define CONT_PIN  PINF
#else
// BREADBOARD VERSION USE PORTK
#define CONT_PORT PORTK
#define CONT_DDR  DDRK
#define CONT_PIN  PINK
#endif

#define VP (1 << 0)
#define GET_VP (CONT_PIN & VP)
#define VP_INPUT CONT_DDR &= ~VP
#define VP_STATUS (status.flags & VP)

#define RDY (1 << 1)
#define GET_RDY (CONT_PIN & RDY)
#define RDY_HI CONT_PORT |= RDY
#define RDY_LO CONT_PORT &= ~RDY
#define RDY_OUTPUT CONT_DDR |= RDY
#define RDY_INPUT CONT_DDR &= ~RDY
#define RDY_STATUS (status.flags & RDY)

#define RESET (1 << 2)
#define GET_RESET (CONT_PIN & RESET)
#define RESET_HI CONT_PORT |= RESET
#define RESET_LO CONT_PORT &= ~RESET
#define RESET_OUTPUT CONT_DDR |= RESET
#define RESET_INPUT CONT_DDR &= ~RESET
#define RESET_STATUS (status.flags & RESET)

#define MLB (1 << 3)
#define GET_MLB (CONT_PIN & MLB)
#define MLB_INPUT CONT_DDR &= ~MLB
#define MLB_STATUS (status.flags & MLB)

#define IRQ (1 << 4)
#define GET_IRQ (CONT_PIN & IRQ)
#define IRQ_HI CONT_PORT |= IRQ
#define IRQ_LO CONT_PORT &= ~IRQ
#define IRQ_OUTPUT CONT_DDR |= IRQ
#define IRQ_INPUT CONT_DDR &= ~IRQ
#define IRQ_STATUS (status.flags & IRQ)

#define NMI (1 << 5)
#define GET_NMI (CONT_PIN & NMI)
#define NMI_HI CONT_PORT |= NMI
#define NMI_LO CONT_PORT &= ~NMI
#define NMI_OUTPUT CONT_DDR |= NMI
#define NMI_INPUT CONT_DDR &= ~NMI
#define NMI_STATUS (status.flags & NMI)

#define SYNC (1 << 6)
#define GET_SYNC (CONT_PIN & SYNC)
#define SYNC_INPUT CONT_DDR &= ~SYNC
#define SYNC_STATUS (status.flags & SYNC)

#define BE (1 << 7)
#define GET_BE (CONT_PIN & BE)
#define BE_HI CONT_PORT |= BE
#define BE_LO CONT_PORT &= ~BE
#define BE_OUTPUT CONT_DDR |= BE
#define BE_INPUT CONT_DDR &= ~BE
#define BE_STATUS (status.flags & BE)

#define CTRLK_OUTPUT_INIT CONT_DDR |= RESET


/* PORT H */
#define PHI2 (1 << 6)
#define GET_PHI2 (PINH & PHI2)
#define PHI2_HI PORTH |= PHI2
#define PHI2_LO PORTH &= ~PHI2
#define PHI2_TOGGLE PINH |= PHI2
#define PHI2_STATUS (status.flags2 & PHI2)

#define CTRLH_OUTPUT_INIT DDRH |= PHI2
#define CTRLH_MASK PHI2
#define GET_BUSACK (0) // DUMMY

/**
 * Complete bus status all in one place
 */
typedef struct {
        uint8_t flags;
        uint8_t flags2;
        uint16_t addr;
        uint8_t data;
} bus_stat;

extern uint8_t clkdiv;
extern uint32_t base_addr;

void clk_cycle(uint8_t cycles);
void clk_run(void);
void clk_stop(void);

uint8_t bus_request(void);
void bus_release(void);
void bus_init(void);

bus_stat bus_status(void);
bus_stat bus_status_fast(void);
void bus_log(bus_stat status);

uint8_t io_out(uint8_t addr, uint8_t value);
uint8_t io_in(uint8_t addr);

#if defined(BANK_PORT) || defined(BANK_BASE)
void mem_bank(uint8_t bank, uint8_t page);
void mem_bank_addr(uint32_t addr);
#else
#define mem_bank(bank, page)
#define mem_bank_addr(addr)
#endif

typedef void (*pagefunc_t)(uint8_t, uint8_t, void*);
void mem_read_page(uint8_t start, uint8_t end, void *buf);
void mem_write_page(uint8_t start, uint8_t end, void *buf);
void mem_write_page_P(uint8_t start, uint8_t end, void *buf);
void mem_iterate(uint16_t start, uint16_t end, pagefunc_t dopage, void *buf);
void mem_iterate_banked(uint32_t start, uint32_t end, pagefunc_t dopage, void *buf);

#define mem_read(addr, buf, len) mem_iterate((addr), ((addr) + ((len) - 1)), mem_read_page, (buf));
#define mem_write(addr, buf, len) mem_iterate((addr), ((addr) + ((len) - 1)), mem_write_page, (buf));
#define mem_write_P(addr, buf, len) mem_iterate((addr), ((addr) + ((len) - 1)), mem_write_page_P, (void *)(buf));

#define mem_read_banked(addr, buf, len) mem_iterate_banked((addr), ((addr) + ((len) - 1)), mem_read_page, (buf));
#define mem_write_banked(addr, buf, len) mem_iterate_banked((addr), ((addr) + ((len) - 1)), mem_write_page, (buf));
#define mem_write_banked_P(addr, buf, len) mem_iterate_banked((addr), ((addr) + ((len) - 1)), mem_write_page_P, (void *)(buf));


#endif
