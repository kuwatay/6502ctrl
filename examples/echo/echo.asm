; @file echo.asm: hello world example program
;  use vasm for assemble
;  vasm6502_oldstyle -Fbin -dotdir -L hello.lst hello.asm
        .org 200h

LF = 0ah
CR = 0dh
SP = 0xff

DMA_HALT = 0
DMA_SIO = 1

DMA_RESET = 1	
DMA_READ = 2
DMA_WRITE = 3	
DMA_STATUS = 4
	
start:	
        ldx #SP
        txs
	
	ldx #0
loop:	lda msg,x
	beq echo
	jsr putc
	inx
	bra loop

echo:	jsr getc
	jsr putc
	bra echo

	
msg:    .byte "input char to echo", CR, LF, 0


;;; DMA Serial I/O routine

	;; getc
	.word dmarq_1
getc:	wai
	lda dmach_1
	rts

dmarq_1:
	.byte DMA_SIO 		; dmano
	.byte DMA_READ		; read
	.word $ffff		; dma request block address (unused for SIO)
dmach_1:
	.byte 0			; dma data byte
	

	;; putc
putc:	sta dmach_2
	bra putc1
	.word dmarq_2
putc1:	wai
	rts

dmarq_2:
	.byte DMA_SIO 		; dmano
	.byte DMA_WRITE		; write
	.word $ffff		; dma request block address (unused for SIO)
dmach_2:
	.byte 0			; dma data byte
	
	;; status
	.word dmarq_3
stat:	wai
	lda dmach_3
	rts

dmarq_3:
	.byte DMA_SIO 		; dmano
	.byte DMA_STATUS	; status
	.word $ffff		; dma request block address (unused for SIO)
dmach_3:
	.byte 0			; dma data byte


	.word dmarq_halt
halt:	wai
	rts
dmarq_halt:
	.byte DMA_HALT
	.byte 0xff		;dummy
	.word 0xffff		;dummy
	.byte 0xff		;dummy
