;;; -*- asm -*-
;;;
;;; Common header file
;;;

	RELAXED	ON

;;; Constants
CR:	EQU	0x0D
LF:	EQU	0x0A
BS:	EQU	0x08
DEL:	EQU	0x7F

;;; Functions
low	function	x,(x & 255)
high	function	x,(x >> 8)

	RELAXED	OFF
