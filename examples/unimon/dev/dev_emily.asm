;;;
;;;	EMILY Board Console Driver
;;;

INIT:
	LDA	#$00
	STA	SMBASE+1
	STA	SMBASE+2	; Command

	LDA	#$A5
	STA	SMBASE		; Signature

	LDA	#$CC
	STA	SMBASE+1	; Handshake

	RTS

CONIN:
	LDA	SMBASE+1	; Handshake
	CMP	#$CC
	BEQ	CONIN

	LDA	#$02
	STA	SMBASE+2	; Command

	LDA	#$CC
	STA	SMBASE+1	; Handshake
CI0:
	LDA	SMBASE+1	; Handshake
	CMP	#$CC
	BEQ	CI0

	LDA	SMBASE+4	; Data[0]

	RTS
	
CONST:
	LDA	SMBASE+1	; Handshake
	CMP	#$CC
	BEQ	CONST

	LDA	#$03
	STA	SMBASE+2	; Command

	LDA	#$CC
	STA	SMBASE+1	; Handshake
CS0:
	LDA	SMBASE+1	; Handshake
	CMP	#$CC
	BEQ	CS0

	LDA	SMBASE+4	; Data[0]

	RTS

CONOUT:
	PHA
CO0:	
	LDA	SMBASE+1	; Handshake
	CMP	#$CC
	BEQ	CO0

	LDA	#$01
	STA	SMBASE+2	; Command

	PLA
	STA	SMBASE+4	; Data[0]

	LDA	#$CC
	STA	SMBASE+1	; Handshake

	RTS
