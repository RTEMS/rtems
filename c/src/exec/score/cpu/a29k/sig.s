;/*
; *  $Id$
; */

;	.include "register.ah"
	.include "amd.ah"
	.include "pswmacro.ah"
	.comm	WindowSize,4
	.text
	.reg	it0,gr64
	.reg	it1,gr65
	.reg	it2,gr66
	.reg	it3,gr67
	.reg	v0,gr96
	.reg	v1,gr97
	.reg	v2,gr98
	.reg	v3,gr99
	.reg	trapreg,it0
	.reg	FPStat0,gr79
	.reg	FPStat1,gr79
	.reg	FPStat2,gr79
	.reg	FPStat3,gr79

	.global _intr14
_intr14:
	const	it3,14
	sup_sv
	jmp	interrupt
	nop

	.global _intr18
_intr18:
	const	it3,18
	sup_sv
	jmp	interrupt
	nop

	.global	_intr19
_intr19:
	const	it3,19
	sup_sv
	jmp	interrupt
	nop

interrupt:
	push	msp,it3
	push	msp,gr1
	push	msp,rab
	const	it0,512
	sub	rab,rfb,it0	;set rab = rfb-512
	pushsr	msp,it0,pc0
	pushsr	msp,it0,pc1
	pushsr	msp,it0,pc2
	pushsr	msp,it0,cha
	pushsr	msp,it0,chd
	pushsr	msp,it0,chc
	pushsr	msp,it0,alu
	pushsr	msp,it0,ops
	push	msp,tav
;
;now come off freeze, and go to user-mode code.
;ensure load/store does not restart
;
	mtsrim	chc,0

	mfsr	it0, cps
	const	it1, FZ
	consth	it1, FZ
	andn	it0, it0, it1
	const	it1,(DI|TD)
	consth	it1,(DI|TD)
	or	it0,it1,it0
	mtsr	cps, it0
; fall through to _sigcode

	.extern	_a29k_ISR_Handler
	.global	_sigcode
_sigcode:

	push	msp, lr1				; R stack support
	push	msp, rfb				; support
	push	msp, msp				; M stack support

;	push	msp, FPStat0				; Floating point 0
;	push	msp, FPStat1				; Floating point 1
;	push	msp, FPStat2				; Floating point 2
;	push	msp, FPStat3				; Floating point 3
	sub	msp,msp,4*4

	pushsr	msp, tav, IPA				; save user mode special
	pushsr	msp, tav, IPB				; save user mode special
	pushsr	msp, tav, IPC				; save user mode special
	pushsr	msp, tav, Q				; save user mode special

	sub	msp, msp, 29*4				; gr96-gr124
	mtsrim	cr, 29-1				;
	storem	0, 0, gr96, msp				;


	const	v0, WindowSize 				; Window Size value
	consth	v0, WindowSize 				; Window Size value
	load	0, 0, v0, v0				; load	Window size	
	add	v2, msp, SIGCTX_RAB			; intr RAB value	

	load 	0, 0, v2, v2				; rab value

	sub	v1, rfb, v2				;
	cpgeu	v1, v1, v0				;
	jmpt	v1, nfill				; jmp if spill
	add	v1, gr1, 8				;

	cpgtu	v1, v1, rfb				; longjump test
	jmpt	v1, nfill				;
	nop						;

ifill:
	add	v0, msp, SIGCTX_RAB+4			;
	push	v0, rab					;
	const	v2, fill+4				;
	consth	v2, fill+4				;

	push	v0, v2					; resave PC0
	sub	v2, v2, 4				;
	push	v0, v2					; resave PC1
	const	v2, 0					;
	
	sub	v0, v0, 3*4				;
	push	v0, v2					;

nfill:
	cpgtu	v0, gr1, rfb				; if gr1>rfb -> gr1=rfb 
	jmpt	v0, lower				;
	cpltu	v0, gr1, rab				;
	jmpt	v0, raise				; gr1<rab then gr1=rab
	nop						;

sendsig:
	sub	gr1, gr1, RALLOC			;
	asgeu	V_SPILL, gr1, rab			;
	add	lr1, rfb, 0				;
	add	v1, msp, SIGCTX_SIG			;

cont:
	add	lr2,it3,0				; signal #
	call	lr0, _a29k_ISR_Handler			; call the handler
	nop

	nop						; WASTE
	jmp	_a29k_sigdfl 				; return code
	nop						; WASTE
	nop						; ALIGN

lower:
	jmp	sendsig					;
	add	gr1, rfb, 0				;
raise:
	jmp	sendsig					;
	add	gr1, rab, 0				;

	
	.global	_a29k_sigdfl_sup
_a29k_sigdfl_sup:
	repair_R_stack					;
	repair_regs					;
	sig_return					; return
	halt						; never executes


	.global _sigret
_sigret:
;assume msp points to tav
	mfsr	it0,cps
	const	it1,FZ
	or	it1,it0,it1
	mtsr	cps,it1
	nop
	nop
_sigret1:
	pop	tav,msp
	popsr	ops,it0,msp
	popsr	alu,it0,msp
	popsr	chc,it0,msp
	popsr	chd,it0,msp
	popsr	cha,it0,msp
	popsr	pc2,it0,msp
	popsr	pc1,it0,msp
	popsr	pc0,it0,msp
	pop	rab,msp
	pop	it0,msp
	add	gr1,it0,0
	add	msp,msp,4	;discount signal
	iret

_a29k_sigdfl:
	asneq	SIGDFL,gr1,gr1
	jmpi	lr0
	nop
