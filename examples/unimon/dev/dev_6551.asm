;;;
;;;	MCS6551 Console Driver
;;;

INIT:
	LDA	#ACCR_V
	STA	ACIACR
	LDA	#ACCM_V
	STA	ACIACM
	RTS
	
CONIN:
	LDA	ACIASR
	AND	#$08
	BEQ	CONIN
	LDA	ACIADR
	RTS
	
CONST:
	LDA	ACIASR
	AND	#$08
	LSR	A
	LSR	A
	LSR	A
	RTS
	
CONOUT:
	PHA
CO0:
	LDA	ACIASR
	AND	#$10
	BEQ	CO0
	PLA
	STA	ACIADR
	RTS
