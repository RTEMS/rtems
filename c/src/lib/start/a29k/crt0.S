; @(#)crt0.s	1.3 96/05/31 14:40:27, AMD
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright 1988, 1989, 1990 Advanced Micro Devices, Inc.
;
; This software is the property of Advanced Micro Devices, Inc	(AMD)  which
; specifically	grants the user the right to modify, use and distribute this
; software provided this notice is not removed or altered.  All other rights
; are reserved by AMD.
;
; AMD MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS
; SOFTWARE.  IN NO EVENT SHALL AMD BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL
; DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING, PERFORMANCE, OR
; USE OF THIS SOFTWARE.
;
; So that all may benefit from your experience, please report  any  problems
; or  suggestions about this software to the 29K Technical Support Center at
; 800-29-29-AMD (800-292-9263) in the USA, or 0800-89-1131  in	the  UK,  or
; 0031-11-1129 in Japan, toll free.  The direct dial number is 512-462-4118.
;
; Advanced Micro Devices, Inc.
; 29K Support Products
; Mail Stop 573
; 5900 E. Ben White Blvd.
; Austin, TX 78741
; 800-292-9263
;
;  /* $Id$ */
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.file	"crt0.s"

; crt0.s version 3.3-0
;
; This module gets control from the OS.
; It saves away the Am29027 Mode register settings and
; then sets up the pointers to the resident spill and fill
; trap handlers. It then establishes argv and argc for passing
; to main. It then calls _main. If main returns, it calls _exit.
;
;	void = start( );
;	NOTE - not C callable (no lead underscore)
;
	.include	"sysmac.h"
;
;
	.extern V_SPILL, V_FILL
;	.comm	__29027Mode, 8	; A shadow of the mode register
	.comm	__LibInit, 4

	.text
	.extern _main, _exit, _atexit

	.word	0			; Terminating tag word
	.global start
start:
	sub	gr1, gr1, 6 * 4
	asgeu	V_SPILL, gr1, rab	; better not ever happen
	add	lr1, gr1, 6 * 4

;
; Save the initial value of the Am29027's Mode register
;
; If your system does not enter crt0 with value for Am29027's Mode 
; register in gr96 and gr97, and also the coprocessor is active 
; uncomment the next 4 instructions.
;
;	const	gr96, 0xfc00820
;	consth	gr96, 0xfc00820
;	const	gr97, 0x1375
;	store	1, 3, gr96, gr97
;
;	const	gr98, __29027Mode
;	consth	gr98, __29027Mode
;	store	0, 0, gr96, gr98
;	add	gr98, gr98, 4
;	store	0, 0, gr97, gr98
;
; Now call the system to setup the spill and fill trap handlers
;
	const	lr3, spill
	consth	lr3, spill
	const	lr2, V_SPILL
	syscall setvec
	const	lr3, fill
	consth	lr3, fill
	const	lr2, V_FILL
	syscall setvec

;
; atexit(_fini)
;
;        const   lr0, _atexit
;        consth  lr0, _atexit
;        const   lr2,__fini
;        calli   lr0,lr0
;        consth  lr2,__fini
;
; Now call _init
;
;        const   lr0, __init
;        consth  lr0, __init
;        calli   lr0,lr0
;        nop

;
; Get the argv base address and calculate argc.
;
	syscall getargs
	add	lr3, v0, 0		; argv
	add	lr4, v0, 0
	constn	lr2, -1
argcloop:				; scan for NULL terminator
	load	0, 0, gr97, lr4
	add	lr4, lr4, 4
	cpeq	gr97, gr97, 0
	jmpf	gr97, argcloop
	add	lr2, lr2, 1
;
; Now call LibInit, if there is one. To aid runtime libraries
; that need to do some startup initialization, we have created
; a bss variable called LibInit. If the library doesn't need
; any run-time initialization, the variable is still 0. If the
; library does need run-time initialization, the library will
; contain a definition like
; void (*_LibInit)(void) = LibInitFunction;
; The linker will match up our bss LibInit with this data LibInit
; and the variable will not be 0.  This results in the LibInit routine
; being called via the calli instruction below.
;
	const	lr0, __LibInit
	consth	lr0, __LibInit
	load	0, 0, lr0, lr0
	cpeq	gr96, lr0, 0
	jmpt	gr96, NoLibInit
	nop
	calli	lr0, lr0
	nop
NoLibInit:

;
; Call RAMInit to initialize the data memory.
; 
; The following code segment was used to create the two flavors of the 
; run-time initialization routines (crt0_1.o, and crt0_2.o) as described
; in the User's Manual.  If osboot is used to create a stand-alone
; application, or the call to RAMInit is made in the start-up routine, 
; then the following is not needed.
;
;       .ifdef ROM_LOAD
;       .extern RAMInit
;
;       const   lr0, RAMInit
;       consth  lr0, RAMInit
;       calli   gr96, lr0
;       nop
;       .else
;       nop
;       nop
;       nop
;       nop
;       .endif

;
; Uncomment the following .comm, if you ARE NOT using osboot as released
; with the High C 29K product, AND plan to use the romcoff utility to
; move code and/or data sections to ROM.
;
;       .comm   RAMInit, 4
; 
; Furthermore, if the above is uncommented, then use the following logic
; to call the RAMInit function, if needed.  
;
;       const   lr0, RAMInit
;       consth  lr0, RAMInit
;       load    0, 0, gr96, lr0
;       cpeq    gr96, gr96, 0           ; nothing there?
;       jmpt    gr96, endRAMInit        ; yes, nothing to init
;       nop
;       calli   gr96, lr0               ; no, then instruction found
;       nop                             ; execute function.
;


;
; call main, passing it 2 arguments. main( argc, argv )
;
	const	lr0, _main
	consth	lr0, _main
	calli	lr0, lr0
	nop
;
; call exit
;
	const	lr0, _exit
	consth	lr0, _exit
	calli	lr0, lr0
	add	lr2, gr96, 0
;
; Should never get here, but just in case
;
loop:
	syscall exit
	jmp	loop
	nop
	.sbttl	"Spill and Fill trap handlers"
	.eject
;
;	SPILL, FILL trap handlers
;
; Note that these Spill and Fill trap handlers allow the OS to
; assume that the only registers of use are between gr1 and rfb.
; Therefore, if the OS desires to, it may simply preserve from
; lr0 for (rfb-gr1)/4 registers when doing a context save.
;
;
; Here is the spill handler
;
; spill registers from [*gr1..*rab)
; and move rab downto where gr1 points
;
; rab must change before rfb for signals to work
;
; On entry:	rfb - rab = windowsize, gr1 < rab
; Near the end: rfb - rab > windowsize, gr1 == rab
; On exit:	rfb - rab = windowsize, gr1 == rab
;
	.global spill
spill:
	sub	tav, rab, gr1	; tav = number of bytes to spill
	srl	tav, tav, 2	; change byte count to word count
	sub	tav, tav, 1	; make count zero based
	mtsr	CR, tav		; set Count Remaining register
	sub	tav, rab, gr1
	sub	tav, rfb, tav	; pull down free bound and save it in rab
	add	rab, gr1, 0	; first pull down allocate bound
	storem	0, 0, lr0, tav	; store lr0..lr(tav) into rfb
	jmpi	tpc		; return...
	add	rfb, tav, 0
;
; Here is the fill handler
;
; fill registers from [*rfb..*lr1)
; and move rfb upto where lr1 points.
;
; rab must change before rfb for signals to work
;
; On entry:	rfb - rab = windowsize, lr1 > rfb
; Near the end: rfb - rab < windowsize, lr1 == rab + windowsize
; On exit:	rfb - rab = windowsize, lr1 == rfb
;
	.global fill
fill:
	const	tav, 0x80 << 2
	or	tav, tav, rfb	; tav = ((rfb>>2) | 0x80)<<2 == [rfb]<<2
	mtsr	IPA, tav	; ipa = [rfb]<<2 == 1st reg to fill
				; gr0 is now the first reg to fill
	sub	tav, lr1, rfb	; tav = number of bytes to fill
	add	rab, rab, tav	; push up allocate bound
	srl	tav, tav, 2	; change byte count to word count
	sub	tav, tav, 1	; make count zero based
	mtsr	CR, tav		; set Count Remaining register
	loadm	0, 0, gr0, rfb	; load registers
	jmpi	tpc		; return...
	add	rfb, lr1, 0	; ... first pushing up free bound

;
; The __init function
;
;        .sect   .init,text
;        .use .init
;        .global __init
;__init:
;        sub     gr1,gr1,16
;        asgeu   V_SPILL,gr1,gr126
;        add     lr1,gr1,24
;
;
; The __fini function
;
;        .sect   .fini,text
;        .use .fini
;        .global __fini
;__fini:
;        sub     gr1,gr1,16
;        asgeu   V_SPILL,gr1,gr126
;        add     lr1,gr1,24
;
	.end
