; Some nasm macros to turn on TS-1325 LEDs and wait for button presses.
; This should be '%include'ed in your nasm source file.
;
; Tony Ambardar

P1LTC  equ  0F862h
P1PIN  equ  0F860h

%macro	LED_OFF		0
	mov	dx, P1LTC
	in	al, dx
	or	al, 01000000b		; turn off red
	and	al, 11011111b		; turn off green
	out	dx, al
%endmacro

%macro	LED_GRN		0
	mov	dx, P1LTC
	in	al, dx
	or	al, 01100000b		; turn off red, turn on green
	out	dx, al
%endmacro

%macro	LED_YEL		0
	mov	dx, P1LTC
	in	al, dx
	or	al, 00100000b		; turn on green
	and	al, 10111111b		; turn on red
	out	dx, al
%endmacro

%macro	LED_RED		0
	mov	dx, P1LTC
	in	al, dx
	and	al, 10011111b		; turn on red, turn off green
	out	dx, al
%endmacro

%macro	PSW_WAIT	0
	mov	dx, P1PIN		; Get PSW state
	mov	ecx, 80000h
%%read	in	al, dx
	test	al, 00000001b		; is PSW asserted?
	jnz	%%read			; if not, we're done
	dec	ecx
	jnz	%%read
%endmacro

