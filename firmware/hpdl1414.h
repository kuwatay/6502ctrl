/* 6502ctrl (https://github.com/kuwatay/6502cttl)
 * Copyright 2021 Yoshitaka Kuwata a.k.a. morecat_lab
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

#include "bus.h"

extern void hpdl1414_init() ;
extern void hpdl1414_clear();
extern void hpdl1414_disp(bus_stat status);
extern void hpdl1414_disp_adrs(uint16_t adr);
extern void hpdl1414_disp_data(uint8_t data);
extern void hpdl1414_strcpy(char *str);
extern void hpdl1414_strcpy_P(PGM_P str);
extern void hpdl1414_write(uint8_t address, uint8_t data);
extern void hpdl1414_address(uint8_t address);
extern void hpdl1414_data(uint8_t data);

/* PORT G */
#define HPDL1414_W2 (1 << 5)
#define HPDL1414_W2_HI PORTG |= HPDL1414_W2
#define HPDL1414_W2_LO PORTG &= ~HPDL1414_W2
#define HPPD1414_CTRLG_OUTPUT_INIT DDRG |= (HPDL1414_W2)

/* PORT D */
#define HPDL1414_D0 (1 << 0)
#define HPDL1414_D0_HI PORTD |= HPDL1414_D0
#define HPDL1414_D0_LO PORTD &= ~HPDL1414_D0
#define HPDL1414_D1 (1 << 1)
#define HPDL1414_D1_HI PORTD |= HPDL1414_D1
#define HPDL1414_D1_LO PORTD &= ~HPDL1414_D1
#define HPPD1414_CTRLD_OUTPUT_INIT DDRD |= (HPDL1414_D0 | HPDL1414_D1)

/* PORT H */
#define HPDL1414_D2 (1 << 0)
#define HPDL1414_D2_HI PORTH |= HPDL1414_D2
#define HPDL1414_D2_LO PORTH &= ~HPDL1414_D2
#define HPDL1414_D3 (1 << 1)
#define HPDL1414_D3_HI PORTH |= HPDL1414_D3
#define HPDL1414_D3_LO PORTH &= ~HPDL1414_D3
#define HPDL1414_D4 (1 << 3)
#define HPDL1414_D4_HI PORTH |= HPDL1414_D4
#define HPDL1414_D4_LO PORTH &= ~HPDL1414_D4
#define HPDL1414_D5 (1 << 4)
#define HPDL1414_D5_HI PORTH |= HPDL1414_D5
#define HPDL1414_D5_LO PORTH &= ~HPDL1414_D5
#define HPPD1414_CTRLH_OUTPUT_INIT DDRH |= (HPDL1414_D2 | HPDL1414_D3 | HPDL1414_D4 | HPDL1414_D5)

/* PORT E */
#define HPDL1414_D6 (1 << 3)
#define HPDL1414_D6_HI PORTE |= HPDL1414_D6
#define HPDL1414_D6_LO PORTE &= ~HPDL1414_D6
#define HPDL1414_W0 (1 << 4)
#define HPDL1414_W0_HI PORTE |= HPDL1414_W0
#define HPDL1414_W0_LO PORTE &= ~HPDL1414_W0
#define HPDL1414_W1 (1 << 5)
#define HPDL1414_W1_HI PORTE |= HPDL1414_W1
#define HPDL1414_W1_LO PORTE &= ~HPDL1414_W1
#define HPPD1414_CTRLE_OUTPUT_INIT DDRE |= (HPDL1414_D6 | HPDL1414_W0 | HPDL1414_W1)

/* PORT J */
#define HPDL1414_A0 (1 << 0)
#define HPDL1414_A0_HI PORTJ |= HPDL1414_A0
#define HPDL1414_A0_LO PORTJ &= ~HPDL1414_A0
#define HPDL1414_A1 (1 << 1)
#define HPDL1414_A1_HI PORTJ |= HPDL1414_A1
#define HPDL1414_A1_LO PORTJ &= ~HPDL1414_A1
#define HPPD1414_CTRLJ_OUTPUT_INIT DDRJ |= (HPDL1414_A0 | HPDL1414_A1)
