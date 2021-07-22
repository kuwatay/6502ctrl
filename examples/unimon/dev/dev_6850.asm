;;;
;;;	MC6850 Console Driver
;;;

INIT:
	LDA	#$03
	STA	ACIAC
	NOP
	NOP
	LDA	#$15		; x16 8bit N 1
	STA	ACIAC

	RTS

CONIN:
	LDA	ACIAC
	AND	#$01
	BEQ	CONIN
	LDA	ACIAD

	RTS

CONST:
	LDA	ACIAC
	AND	#$01

	RTS

CONOUT:
	PHA
CO0:
	LDA	ACIAC
	AND	#$02
	BEQ	CO0
	PLA
	STA	ACIAD

	RTS