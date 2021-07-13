; @file loop.asm: simple loop
;  use vasm for assemble
;  vasm6502_oldstyle -Fbin -dotdir -L loop.lst loop.asm
        .org 200h

loop:	jmp loop
