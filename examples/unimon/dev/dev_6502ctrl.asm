;;;
;;;	6502ctrl DMA Console Driver
;;;

INIT:
	RTS

;;; conin
	fdb dmarq_1
CONIN:	wai
	lda dmach_1
	rts

dmarq_1:
	fcb DMA_SIO 		; dmano
	fcb DMA_READ		; read
	fdb $ffff		; dma request block address (unused for SIO)
dmach_1:
	fcb 0			; dma data byte
;;; status
	fdb dmarq_3
CONST:	wai
	lda dmach_3
	and #01
	rts

dmarq_3:
	fcb DMA_SIO 		; dmano
	fcb DMA_STATUS	; status
	fdb $ffff		; dma request block address (unused for SIO)
dmach_3:
	fcb 0			; dma data byte

;;; conout
CONOUT:
	sta dmach_2
	bra putc1
	fdb dmarq_2
putc1:	wai
	rts

dmarq_2:
	fcb DMA_SIO 		; dmano
	fcb DMA_WRITE		; write
	fdb $ffff		; dma request block address (unused for SIO)
dmach_2:
	fcb 0			; dma data byte
