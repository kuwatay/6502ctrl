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
 * FITNESS FOR a PARTICULAR PURPOSE AND NONINFRINGEMENT. in NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER in AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, out OF OR in CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS in THE SOFTWARE.
 */

#include "disasm.h"
#include "bus.h"

#include <avr/pgmspace.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
 * @file disasm.c 6502 disassembler
 * 
 */

// disassbmler tables are from the folloing
// https://github.com/hoglet67/AtomBusMon/blob/master/firmware/dis6502.c
// GPL2

enum
{
  IMP, IMPA, MARK2, BRA, BITZ, BITR, IMM, ZP, ZPX, ZPY, INDX, INDY, IND, MARK3, ABS, ABSX, ABSY, IND16, IND1X
};

enum
  {
    I_ADC,
    I_AND,
    I_ASL,
    I_BBR, // added
    I_BBS, // added
    I_BCC,
    I_BCS,
    I_BEQ,
    I_BIT,
    I_BMI,
    I_BNE,
    I_BPL,
    I_BRA,
    I_BRK,
    I_BVC,
    I_BVS,
    I_CLC,
    I_CLD,
    I_CLI,
    I_CLV,
    I_CMP,
    I_CPX,
    I_CPY,
    I_DEC,
    I_DEX,
    I_DEY,
    I_EOR,
    I_INC,
    I_INX,
    I_INY,
    I_JMP,
    I_JSR,
    I_LDA,
    I_LDX,
    I_LDY,
    I_LSR,
    I_NOP,
    I_ORA,
    I_PHA,
    I_PHP,
    I_PHX,
    I_PHY,
    I_PLA,
    I_PLP,
    I_PLX,
    I_PLY,
    I_RMB,
    I_ROL,
    I_ROR,
    I_RTI,
    I_RTS,
    I_SBC,
    I_SEC,
    I_SED,
    I_SEI,
    I_SMB,
    I_STA,
    I_STP,
    I_STX,
    I_STY,
    I_STZ,
    I_TAX,
    I_TAY,
    I_TRB,
    I_TSB,
    I_TSX,
    I_TXA,
    I_TXS,
    I_TYA,
    I_WAI,
    I_XXX
  };

static const char opString[] PROGMEM = "\
ADC\
AND\
ASL\
BBR\
BBS\
BCC\
BCS\
BEQ\
BIT\
BMI\
BNE\
BPL\
BRA\
BRK\
BVC\
BVS\
CLC\
CLD\
CLI\
CLV\
CMP\
CPX\
CPY\
DEC\
DEX\
DEY\
EOR\
INC\
INX\
INY\
JMP\
JSR\
LDA\
LDX\
LDY\
LSR\
NOP\
ORA\
PHA\
PHP\
PHX\
PHY\
PLA\
PLP\
PLX\
PLY\
RMB\
ROL\
ROR\
RTI\
RTS\
SBC\
SEC\
SED\
SEI\
SMB\
STA\
STP\
STX\
STY\
STZ\
TAX\
TAY\
TRB\
TSB\
TSX\
TXA\
TXS\
TYA\
WAI\
---\
";

static const unsigned char dopname[256] PROGMEM =
{
 /*    00     01     02	    03     04     05     06     07     08     09     0A     0B     0C     0D     0E     0F */
/*00*/ I_BRK, I_ORA, I_XXX, I_XXX, I_TSB, I_ORA, I_ASL, I_RMB, I_PHP, I_ORA, I_ASL, I_XXX, I_TSB, I_ORA, I_ASL, I_BBR,
/*10*/ I_BPL, I_ORA, I_ORA, I_XXX, I_TRB, I_ORA, I_ASL, I_RMB, I_CLC, I_ORA, I_INC, I_XXX, I_TRB, I_ORA, I_ASL, I_BBR,
/*20*/ I_JSR, I_AND, I_XXX, I_XXX, I_BIT, I_AND, I_ROL, I_RMB, I_PLP, I_AND, I_ROL, I_XXX, I_BIT, I_AND, I_ROL, I_BBR,
/*30*/ I_BMI, I_AND, I_AND, I_XXX, I_BIT, I_AND, I_ROL, I_RMB, I_SEC, I_AND, I_DEC, I_XXX, I_BIT, I_AND, I_ROL, I_BBR,
/*40*/ I_RTI, I_EOR, I_XXX, I_XXX, I_XXX, I_EOR, I_LSR, I_RMB, I_PHA, I_EOR, I_LSR, I_XXX, I_JMP, I_EOR, I_LSR, I_BBR,
/*50*/ I_BVC, I_EOR, I_EOR, I_XXX, I_XXX, I_EOR, I_LSR, I_RMB, I_CLI, I_EOR, I_PHY, I_XXX, I_XXX, I_EOR, I_LSR, I_BBR,
/*60*/ I_RTS, I_ADC, I_XXX, I_XXX, I_STZ, I_ADC, I_ROR, I_RMB, I_PLA, I_ADC, I_ROR, I_XXX, I_JMP, I_ADC, I_ROR, I_BBR,
/*70*/ I_BVS, I_ADC, I_ADC, I_XXX, I_STZ, I_ADC, I_ROR, I_RMB, I_SEI, I_ADC, I_PLY, I_XXX, I_JMP, I_ADC, I_ROR, I_BBR,
/*80*/ I_BRA, I_STA, I_XXX, I_XXX, I_STY, I_STA, I_STX, I_SMB, I_DEY, I_BIT, I_TXA, I_XXX, I_STY, I_STA, I_STX, I_BBS,
/*90*/ I_BCC, I_STA, I_STA, I_XXX, I_STY, I_STA, I_STX, I_SMB, I_TYA, I_STA, I_TXS, I_XXX, I_STZ, I_STA, I_STZ, I_BBS,
/*A0*/ I_LDY, I_LDA, I_LDX, I_XXX, I_LDY, I_LDA, I_LDX, I_SMB, I_TAY, I_LDA, I_TAX, I_XXX, I_LDY, I_LDA, I_LDX, I_BBS,
/*B0*/ I_BCS, I_LDA, I_LDA, I_XXX, I_LDY, I_LDA, I_LDX, I_SMB, I_CLV, I_LDA, I_TSX, I_XXX, I_LDY, I_LDA, I_LDX, I_BBS,
/*C0*/ I_CPY, I_CMP, I_XXX, I_XXX, I_CPY, I_CMP, I_DEC, I_SMB, I_INY, I_CMP, I_DEX, I_WAI, I_CPY, I_CMP, I_DEC, I_BBS,
/*D0*/ I_BNE, I_CMP, I_CMP, I_XXX, I_XXX, I_CMP, I_DEC, I_SMB, I_CLD, I_CMP, I_PHX, I_STP, I_XXX, I_CMP, I_DEC, I_BBS,
/*E0*/ I_CPX, I_SBC, I_XXX, I_XXX, I_CPX, I_SBC, I_INC, I_SMB, I_INX, I_SBC, I_NOP, I_XXX, I_CPX, I_SBC, I_INC, I_BBS,
/*F0*/ I_BEQ, I_SBC, I_SBC, I_XXX, I_XXX, I_SBC, I_INC, I_SMB, I_SED, I_SBC, I_PLX, I_XXX, I_XXX, I_SBC, I_INC, I_BBS
};

static const unsigned char dopaddr[256] PROGMEM =
{
/*     00   01     02   03   04     05     06    07     08    09     0A     0B   0C       0D     0E    0F */
/*00*/ IMP, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMPA,  IMP,  ABS,	  ABS,	 ABS,  BITR,
/*10*/ BRA, INDY,  IND, IMP,  ZP,   ZPX,   ZPX,	 BITZ,	IMP,  ABSY,  IMPA,  IMP,  ABS,	  ABSX,	 ABSX, BITR,
/*20*/ ABS, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMPA,  IMP,  ABS,	  ABS,	 ABS,  BITR,
/*30*/ BRA, INDY,  IND, IMP,  ZPX,  ZPX,   ZPX,	 BITZ,	IMP,  ABSY,  IMPA,  IMP,  ABSX,	  ABSX,	 ABSX, BITR,
/*40*/ IMP, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMPA,  IMP,  ABS,	  ABS,	 ABS,  BITR,
/*50*/ BRA, INDY,  IND, IMP,  ZP,   ZPX,   ZPX,	 BITZ,	IMP,  ABSY,  IMP,   IMP,  ABS,	  ABSX,	 ABSX, BITR,
/*60*/ IMP, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMPA,  IMP,  IND16,  ABS,	 ABS,  BITR,
/*70*/ BRA, INDY,  IND, IMP,  ZPX,  ZPX,   ZPX,	 BITZ,	IMP,  ABSY,  IMP,   IMP,  IND1X,  ABSX,	 ABSX, BITR,
/*80*/ BRA, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMP,   IMP,  ABS,	  ABS,	 ABS,  BITR,
/*90*/ BRA, INDY,  IND, IMP,  ZPX,  ZPX,   ZPY,	 BITZ,	IMP,  ABSY,  IMP,   IMP,  ABS,	  ABSX,	 ABSX, BITR,
/*A0*/ IMM, INDX,  IMM, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMP,   IMP,  ABS,	  ABS,	 ABS,  BITR,
/*B0*/ BRA, INDY,  IND, IMP,  ZPX,  ZPX,   ZPY,	 BITZ,	IMP,  ABSY,  IMP,   IMP,  ABSX,	  ABSX,	 ABSY, BITR,
/*C0*/ IMM, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMP,   IMP,  ABS,	  ABS,	 ABS,  BITR,
/*D0*/ BRA, INDY,  IND, IMP,  ZP,   ZPX,   ZPX,	 BITZ,	IMP,  ABSY,  IMP,   IMP,  ABS,	  ABSX,	 ABSX, BITR,
/*E0*/ IMM, INDX,  IMP, IMP,  ZP,   ZP,	   ZP,	 BITZ,	IMP,  IMM,   IMP,   IMP,  ABS,	  ABS,	 ABS,  BITR,
/*F0*/ BRA, INDY,  IND, IMP,  ZP,   ZPX,   ZPX,	 BITZ,	IMP,  ABSY,  IMP,   IMP,  ABS,	  ABSX,	 ABSX, BITR
};

//#define LOOKUP(table, i1, i2) pgm_read_byte(&table[i1][i2])
//#define LOOKUP4(table, i1, i2, i3) pgm_read_byte(&table[i1][i2][i3])

static uint8_t disasm_index = 0;   /**< index of next byte within 256 byte buffer */
static uint32_t disasm_addr = 0;   /**< address of next chunk to read from external RAM */
static uint8_t *disasm_buf;        /**< pointer to disassembly buffer */

static uint8_t instr_bytes[8];     /**< bytes contained in the current instruction */
static uint8_t instr_length = 0;   /**< length of the current construction */

// Disassemble an instruction
uint8_t disasm(uint8_t (*input)(), char *output)
{
  uint8_t op = input();
  int8_t mode = pgm_read_byte(dopaddr + op);
  uint8_t p1 = (mode > MARK2) ? input() : 0;
  uint8_t p2 = (mode > MARK3) ? input() : 0;

  uint8_t opIndex = pgm_read_byte(dopname + op) * 3;

  // opcode 3 bytes
  for (uint8_t i = 0; i < 3 ; i++) {
    *output++ = pgm_read_byte(opString + opIndex + i);
  }

  switch (mode){
  case IMP:
    break;
  case IMPA:
    output += sprintf_P(output, PSTR("A"));
    break;
  case BITR:
    *output++ = ((op >> 4) & 0x7) + '0';  // add '0'-'7' to opcode
    // no break here
  case BRA:
    output += sprintf_P(output, PSTR(" $%04x"), (disasm_addr + ((p1 < 128) ? p1 : p1 - 256))); //  display relative address
    break;
  case IMM:
    output += sprintf_P(output, PSTR(" #$%02X"), p1);
    break;
  case BITZ:
    *output++ = ((op >> 4) & 0x7) + '0';  // add '0'-'7' to opcode
    // no break here
  case ZP:
    output += sprintf_P(output, PSTR(" $%02X"), p1);
    break;
  case ZPX:
    output += sprintf_P(output, PSTR(" $%02X,X"), p1);
    break;
  case ZPY:
    output += sprintf_P(output, PSTR(" $%02X,Y"), p1);
    break;
  case IND:
    output += sprintf_P(output, PSTR(" ($%02X)"), p1);
    break;
  case INDX:
    output += sprintf_P(output, PSTR(" ($%02X,X)"), p1);
    break;
  case INDY:
    output += sprintf_P(output, PSTR(" ($%02X,Y)"), p1);
    break;
  case ABS:
    output += sprintf_P(output, PSTR(" $%02X%02X"), p2, p1);
    break;
  case ABSX:
    output += sprintf_P(output, PSTR(" $%02X%02X,X"), p2, p1);
    break;
  case ABSY:
    output += sprintf_P(output, PSTR(" $%02X%02X,Y"), p2, p1);
    break;
  case IND16:
    output += sprintf_P(output, PSTR(" ($%02X%02X)"), p2, p1);
    break;
  case IND1X:
    output += sprintf_P(output, PSTR(" ($%02X%02X,X)"), p2, p1);
    break;
  }

  *output = '\0';
}


/**
 * Return next byte for instruction from memory
 */
uint8_t disasm_next_byte()
{
    if (disasm_index == 0)
        mem_read(disasm_addr, disasm_buf, 256);
    disasm_addr++;
    instr_bytes[instr_length] = disasm_buf[disasm_index++];
    return instr_bytes[instr_length++];
}

/**
 * Disassemble instructions from external memory to console
 */
void disasm_mem(uint32_t start, uint32_t end)
{
    char mnemonic[64];
    uint8_t buf[256];
    uint8_t i;

    disasm_addr = start;
    disasm_index = 0;
    disasm_buf = buf;

    while (start <= disasm_addr && disasm_addr <= end) {
        instr_length = 0;
        printf_P(PSTR("%05lx "), disasm_addr);
        disasm(disasm_next_byte, mnemonic);
        for (i = 0; i < instr_length; i++) {
            printf_P(PSTR("%02x "), instr_bytes[i]);
        }
        i = 4 - instr_length;
        while (i--)
            printf_P(PSTR("   "));
        for (i = 0; i < instr_length; i++) {
            if (0x20 <= instr_bytes[i] && instr_bytes[i] <= 0x7e)
                printf_P(PSTR("%c"), instr_bytes[i]);
            else
                printf_P(PSTR("."));
        }
        i = 4 - instr_length;
        while (i--)
            printf_P(PSTR(" "));
        printf_P(PSTR(" %s\n"), mnemonic);
    }
}
