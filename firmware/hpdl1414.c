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
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#include <stdio.h>
#include "hpdl1414.h"


void hpdl1414_init() {
  // set port direction 
  HPPD1414_CTRLG_OUTPUT_INIT;
  HPPD1414_CTRLD_OUTPUT_INIT;
  HPPD1414_CTRLH_OUTPUT_INIT;
  HPPD1414_CTRLE_OUTPUT_INIT;
  HPPD1414_CTRLJ_OUTPUT_INIT;
  // set initial state
  HPDL1414_W0_HI;
  HPDL1414_W1_HI;
  HPDL1414_W2_HI;
}

void hpdl1414_clear() {
  for (uint8_t i = 0 ; i < 12 ; i++) {
    hpdl1414_write(i, 0x20); // fill with space
  }
}

void hpdl1414_disp(bus_stat status){
  hpdl1414_disp_adrs(status.addr);
  hpdl1414_disp_data(status.data);
  hpdl1414_write(4, (RW_STATUS ? 'R' : 'W'));
  hpdl1414_write(3, (RDY_STATUS ? 'R' : 'H'));
  hpdl1414_write(2, (!BE_STATUS ? 'B' : ' '));
  hpdl1414_write(1, (!VP_STATUS ? 'V' : ' '));
  hpdl1414_write(0, (!IRQ_STATUS ? 'I' : ' '));
}

void hpdl1414_disp_adrs(uint16_t adr) {
  uint8_t i,c;
  for (i = 0; i < 4 ; i++) {
    c = (adr >> ((3-i)*4)) & 0xf;
    if (c > 9)
      c += 'A' - 10;
    else
      c += '0';
      
    hpdl1414_write(11-i, c); // digit 8-11
  }

}
void hpdl1414_disp_data(uint8_t data) {
  uint8_t i,c;
  for (i = 0; i < 2 ; i++) {
    c = (data >> ((1-i)*4)) & 0xf;
    if (c > 9)
      c += 'A' - 10;
    else
      c += '0';
      
    hpdl1414_write(7-i, c);  //digit 6-7
  }
  hpdl1414_write(5, '_');

}
// write STR
void hpdl1414_strcpy(char *str) {
  char *p = str;
  for (uint8_t i = 0 ; i < 12 ; i++) {
    if (*p == 0) break;
    hpdl1414_write(11-i, *p++);
  }
}

// write PSTR 
void hpdl1414_strcpy_P(PGM_P str) {
  uint8_t i,c;
  for (i=0; i < 12 ; i++) {
    if ((c = pgm_read_byte(str+i)) == 0)
      break;
    hpdl1414_write(11-i, c);
  }
}

void hpdl1414_write(uint8_t address, uint8_t data) {
  hpdl1414_address(address);
  hpdl1414_data(data);
  switch ((address >>2) & 0x03) {
  case 0:
    HPDL1414_W0_LO;
    _delay_ms(1);
    HPDL1414_W0_HI;
    _delay_ms(1);
    break;
  case 1:
    HPDL1414_W1_LO;
    _delay_ms(1);
    HPDL1414_W1_HI;
    _delay_ms(1);
    break;
  case 2:
    HPDL1414_W2_LO;
    _delay_ms(1);
    HPDL1414_W2_HI;
    _delay_ms(1);
    break;
  }
}

void hpdl1414_address(uint8_t address) {
  if ((address >> 0) & 0x01)
    HPDL1414_A0_HI;
  else
    HPDL1414_A0_LO;

  if ((address >> 1) & 0x01)
    HPDL1414_A1_HI;
  else
    HPDL1414_A1_LO;

}

void hpdl1414_data(uint8_t data) {
  if ((data >> 0) & 0x01)
    HPDL1414_D0_HI;
  else
    HPDL1414_D0_LO;

  if ((data >> 1) & 0x01)
    HPDL1414_D1_HI;
  else
    HPDL1414_D1_LO;

  if ((data >> 2) & 0x01)
    HPDL1414_D2_HI;
  else
    HPDL1414_D2_LO;

  if ((data >> 3) & 0x01)
    HPDL1414_D3_HI;
  else
    HPDL1414_D3_LO;

  if ((data >> 4) & 0x01)
    HPDL1414_D4_HI;
  else
    HPDL1414_D4_LO;

  if ((data >> 5) & 0x01)
    HPDL1414_D5_HI;
  else
    HPDL1414_D5_LO;

  if ((data >> 6) & 0x01)
    HPDL1414_D6_HI;
  else
    HPDL1414_D6_LO;
}

  
