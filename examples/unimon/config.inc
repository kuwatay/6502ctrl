;;; -*- asm -*-
;;;
;;; Universal Monitor 6502 config file (6502ctrl)
;;;

;;;
;;; Memory
;;;

ROM_B:	EQU	$F600
ENTRY:	EQU	$FF80		; Entry point
	
WORK_B:	EQU	$0000		; Must fit in ZERO page
STACK:	EQU	$01FF

BUFLEN:	EQU	16		; Buffer length ( 16 or above )

;;;
;;;  Options
;;;

USE_IDENT = 1			; CPU Identification
USE_REGCMD = 1			; Register related commands
	
;;;
;;; MOS Technology MCS6551
;;;

USE_DEV_6551 = 0
	IF USE_DEV_6551
	;;
ACIADR:	EQU	$8000		; Data Register
ACIASR:	EQU	$8001		; Status Register
ACIACM:	EQU	$8002		; Command Register
ACIACR:	EQU	$8003		; Control Register
ACCR_V:	EQU	$1E		; Control: 8bit, 9600bps
ACCM_V:	EQU	$0B		; Command: No-parity
	ENDIF

;;;
;;; Motorola MC6850 (ACIA)
;;;

USE_DEV_6850 = 0
	IF USE_DEV_6850
	;;
ACIAC:	EQU	$8018		; Control / Status Register
ACIAD:	EQU	$8019		; Data Register
	ENDIF

;;;
;;; EMILY Board (Shared Memory)
;;;

USE_DEV_EMILY = 0
	IF USE_DEV_EMILY
	;;
SMBASE:	EQU	$FFE0
	ENDIF

;;;
;;; 6502ctrl DMA mode
;;;
USE_DEV_6502CTRL = 1
	IF USE_DEV_6502CTRL
	;;
DMA_HALT EQU	0
DMA_SIO	EQU	1

DMA_RESET EQU	1	
DMA_READ EQU	2
DMA_WRITE EQU 	3	
DMA_STATUS EQU 	4
	ENDIF
	
