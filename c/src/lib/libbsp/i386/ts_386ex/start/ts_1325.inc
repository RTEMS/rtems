/*
  These are just some useful macros that control the TS-1325's
  LEDs and push-button switch. Useful for debugging.

  NOTE:	This *must* be 16-bit compatible code to work in start.s
	
  Tony Ambardar
*/

	.macro	LED_OFF
	movw	$P1LTC,	dx
	inb	dx,	al
	orb	$0b01000000,	al
	andb	$0b11011111,	al
	outb	al,	dx
	.endm

	.macro	LED_GREEN
	movw	$P1LTC,	dx
	inb	dx,	al
	orb	$0b01100000,	al
	outb	al,	dx
	.endm

	.macro	LED_YELLOW
	movw	$P1LTC,	dx
	inb	dx,	al
	orb	$0b00100000,	al
	andb	$0b10111111,	al
	outb	al,	dx
	.endm

	.macro	LED_RED
	movw	$P1LTC,	dx
	inb	dx,	al
	andb	$0b10011111,	al
	outb	al,	dx
	.endm

	.macro	WAIT_BUTTON     # Wait till the button is pressed for a bit.
	movw	$P1PIN,	dx      # ~25-30 cycles per loop, 25MHz -> 1 sec.
	movl	$300000,ecx    # "Timer" count determines how long.
0:	inb	dx,	al
	andb	$0b00000001,al
	jnz	0b              # Button pressed?
	decl	ecx
	jnz	0b              # CX count expired?
	.endm


