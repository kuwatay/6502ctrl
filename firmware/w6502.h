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
 * @file w6502.h w6502 run loop and debugger
 */

#ifndef W6502_H
#define W6502_H

#include <stdint.h>
#include "bus.h"


enum {MEMRD, MEMWR, OPFETCH, BUS, DMA, DEBUGCNT};

typedef struct {
        uint16_t start;
        uint16_t end;
} range;

extern range breaks[];
extern range watches[];
extern uint8_t halt_mask;
extern const char debug_names[];

#define INRANGE(ranges, type, addr) ((ranges)[(type)].start <= (addr) && (addr) <= (ranges)[type].end)
#define ENABLED(ranges, type) ((ranges)[(type)].start <= (ranges)[(type)].end)

void w6502_page(uint32_t p);
void w6502_reset(uint32_t addr);
void w6502_run(void);
void w6502_debug(uint32_t cycles);

#endif
