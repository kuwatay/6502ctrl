; Apple I Monitor ROM
; Steve Wozniak
; 1976
; --------------------------------------------------------

	CPU	W65C02S
;.segment "MONITOR"		;

;.export ECHO
;	org $ff00
	org $fe00
; --------------------------------------------------------
; Zero page variables
XAML	:= $24
XAMH	:= $25
STL	:= $26
STH	:= $27
L	:= $28
H	:= $29
YSAV	:= $2A
MODE	:= $2B

DMARQ1  := $2C
DMARQ1_CMD := $2D
DMARQ1_DATA := $30
	
DMARQ2  := $31
DMARQ2_CMD := $31
DMARQ2_DATA := $34
	
; I/O locations
IN	:= $0200		; Input buffer is $0200 to $027F
;KBD	:= $D010		; Keyboard data
;DSP	:= $D012		; Display data
;KBDCR	:= $D011		; Keyboard control register
;DSPCR	:= $D013		; Display control register

DMA_HALT = 0
DMA_SIO = 1

DMA_RESET = 1	
DMA_READ = 2
DMA_WRITE = 3	
DMA_STATUS = 4
DMA_READ_BLOCK = 5
	
;ORIGINAL_WAZ = 1
	IFDEF ORIGINAL_WAZ
OUTPUT_MASK = $80
	ENDIF
	
	IFNDEF ORIGINAL_WAZ
OUTPUT_MASK = $00
	ENDIF
	
; ASCII codes
CR	= $0D | OUTPUT_MASK	; Carriage return
ESC	= $1B | OUTPUT_MASK	; Escape
SLASH	= '\\' | OUTPUT_MASK	; 
UNDERSC = '_' | OUTPUT_MASK	; Underscore (acts as backspace)
DOT	= '.' | OUTPUT_MASK	; .
COLON	= ':' | OUTPUT_MASK	; :
R	= 'R' | OUTPUT_MASK	; "R"
SPACE	= ' ' | OUTPUT_MASK	; blank
ZERO	= '0' | OUTPUT_MASK	; "0"

; --------------------------------------------------------
RESET:	cld			; Clear decimal arithmetic mode
;	cli
	ldy	#$7F		; Mask for DSP data direction register
;	sty	DSP		; Set it up
;	lda	#$A7		; KBD and DSP control register mask
;	sta	KBDCR		; Enable interrupts, set CA1, CB1 for
;	sta	DSPCR		;   positive edge sense/output mode
NOTCR:	cmp	#UNDERSC	; Backspace?
	beq	BACKSPACE	; Yes
	cmp	#ESC		; ESC?
	beq	ESCAPE		; Yes
	iny			; Advance text index
	bpl	NEXTCHAR	; Auto ESC if > 127
ESCAPE:	lda	#SLASH		; "\"
	jsr	ECHO		; Output it
GETLINE:
	lda	#CR		; CR
	jsr	ECHO		; Output it
	ldy	#$01		; Initialize text index
BACKSPACE:
	dey			; Back up text index
	bmi	GETLINE		; Beyond start of line, reinitialize
NEXTCHAR:
	jsr	getc
;	lda	KBDCR		; Key ready?
;	bpl	NEXTCHAR	; Loop until ready
;	lda	KBD		; Load character. B7 should be '1'
	sta	IN,y		; Add to text buffer
	jsr	ECHO		; Display character
	cmp	#CR		; CR?
	bne	NOTCR		; No
	ldy	#$FF		; Reset text index
	lda	#$00		; For XAM mode
	tax			; 0 -> x
;; added
	BRA 	SETSTOR
SETBLK:	LDA 	#$AE
	BRA	SETMODE
;; added
SETSTOR:
	asl			; Leaves $7B if setting STOR mode
SETMODE:
	sta	MODE		; $00 = XAM, $7B = STOR, $AE = BLOCK XAM
				; [ NEW	$3E: BLOCK XAM ]
BLSKIP:	iny			; Advance text index
NEXTITEM:
	lda	IN,y		; Get character
	cmp	#CR		; CR?
	beq	GETLINE		; Yes, done this line
	cmp	#DOT		; "."?
	bcc	BLSKIP		; Skip delimiter
;	beq	SETMODE		; Set BLOCK XAM mode
	BEQ	SETBLK		; Set BLOCK XAM mode
	cmp	#COLON		; ":"?
	beq	SETSTOR		; Yes, set STOR mode
	cmp	#R		; "R"?
	beq	RUN		; Yes, run user program
	stx	L		; $00 -> L
	stx	H		;   and H
	sty	YSAV		; Save Y for comparison
NEXTHEX:
	lda	IN,y		; Get character for hex test
;	eor	#$B0		; Map digits to $0-9
	eor	#$30		; Map digits to $0-9
	cmp	#$0A		; Digit?
	bcc	DIG		; Yes
	adc	#$88		; Map letter "A"-"F" to $FA-FF
	cmp	#$FA		; Hex letter?
	bcc	NOTHEX		; No, character not hext
DIG:	asl
	asl			; Hex digit to MSD of A
	asl
	asl
	ldx	#$04		; Shift count
HEXSHIFT:
	asl			; Hex digit left, MSB to carry
	rol	L		; Rotate into LSD
	rol	H		; Rotate into MSD's
	dex			; Done 4 shifts?
	bne	HEXSHIFT	; No, loop
	iny			; Advance text index
	bne	NEXTHEX		; Always taken. Check next character for hex
NOTHEX:
	cpy	YSAV		; Check if L, H empty (no hex digits)
	beq	ESCAPE		; Yes, generate ESC sequence
;  NOT WORK
;	LDA	#$7f
;	AND	MODE
;	BNE	NOTSTOR
;  NOT WORK
	bit	MODE		; Test MODE byte [NEW CHECK]
	bvc	NOTSTOR		; B6 = 0 for STOR, 1 for XAM and BLOCK XAM
	lda	L		; LSD's of hex data
	sta	(STL,x)		; Store at current 'store index'
	inc	STL		; Increment store index
	bne	NEXTITEM	; Get next item (no carry)
	inc	STH		; Add carry to 'store index' high order
TONEXTITEM:
	jmp	NEXTITEM	; Get next command item
RUN:	jmp	(XAML)		; Run at current XAM index
NOTSTOR:
	bmi	XAMNEXT		; B7=0 for XAM, 1 for BLOCK XAM [NEW CHECK]
	ldx	#$02		; Byte count
SETADR:	lda	L-1,x		; Copy hex data to
	sta	STL-1,x		;   'store index'
	sta	XAML-1,x	; And to 'XAM index'
	dex			; Next of 2 bytes
	bne	SETADR		; Loop unless X=0
NXTPRNT:
	bne	PRDATA		; NE means no address to print
	lda	#CR		; CR
	jsr	ECHO		; Output it
	lda	XAMH		; 'Examine index' high-order byte
	jsr	PRBYTE		; Output it in hex format
	lda	XAML		; Low-order 'examine index' byte
	jsr	PRBYTE		; Output it in hex format
	lda	#COLON		; ":"
	jsr	ECHO		; Output it
PRDATA:	lda	#SPACE		; Blank
	jsr	ECHO		; Output it
	lda	(XAML,x)	; Get data byte at 'examine index'
	jsr	PRBYTE		; Output it in hex format
XAMNEXT:
	stx	MODE		; 0 -> MODE (XAM mode)
	lda	XAML
	cmp	L		; Compare 'examine index' to hex data
	lda	XAMH
	sbc	H
	bcs	TONEXTITEM	; Not less, so no more data to output
	inc	XAML
	bne	MOD8CHK		; Increment 'examine index'
	inc	XAMH
MOD8CHK:
	lda	XAML		; Check low-order 'examine index' byte
	and	#$07		;   For MOD 8 = 0
	bpl	NXTPRNT		; Always taken
PRBYTE:	pha			; Save A for LSD
	lsr
	lsr
	lsr			; MSD to LSD position
	lsr
	jsr	PRHEX		; Output hex digit
	pla			; Restore A
PRHEX:	and	#$0F		; Mask LSD for hex print
	ora	#ZERO		; Add "0"
;	cmp	#$BA		; Digit?
	cmp	#$3A		; Digit?
	bcc	ECHO		; Yes, output it
	adc	#$06		; Add offset for letter
ECHO:
;;; putc
putc:
	sta dmach_2
	bra putc1
	FDB dmarq_2
putc1:	wai
	rts
	
;;; getc
	FDB dmarq_1
getc:	wai
	lda dmach_1
	rts
dmarq_1:
 	FCB DMA_SIO 		; dmano
 	FCB DMA_READ_BLOCK	; read
 	FDB $ffff		; dma request block address (unused for SIO)
dmach_1:
 	FCB 0			; dma data byte
dmarq_2:
 	FCB DMA_SIO 		; dmano
 	FCB DMA_WRITE		; write
 	FDB $ffff		; dma request block address (unused for SIO)
dmach_2:
 	FCB 0			; dma data byte
; --------------------------------------------------------
	org	$fffa		;
;	FDB	$0000		; (unused)
	FDB	$0F00		; (NMI vector)
	FDB	RESET		; (RESET vector)
	FDB	$0000		; (IRQ vector)
