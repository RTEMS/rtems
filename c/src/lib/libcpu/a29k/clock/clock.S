;
#if 0
;	.include "register.ah"
        .include "amd.ah"
        .include "pswmacro.ah"

	.text
;	.equ	IN,0x2000000
	.reg	CLOCK,gr78
	.reg	SECS,gr79
	.equ 	CPUCLK,16
	.equ	RATE,100
	.reg	it0,gr64
	.reg	it1,gr65
	.reg	v0,gr96
timer_clear_sup:
	const	it0,IN
	consth	it0,IN
	mfsr	it1,tmr
	andn	it1,it1,it0
	mtsr	tmr,it1
	const	it0,RATE
	cplt	it0,CLOCK,it0
	jmpf	it0,carry
	add	CLOCK,CLOCK,1
	iret
carry:
	const	CLOCK,0
	add	SECS,SECS,1
	iret

	.equ	TICKS,(CPUCLK*1000000/RATE)
;	.equ	IE,0x1000000
timer_init_sup:
	const	it0,TICKS
	consth	it0,TICKS
	mtsr	tmc,it0
	const	it0,(IE|TICKS)
	consth	it0,(IE|TICKS)
	mtsr	tmr,it0
	const	SECS,0
	const	CLOCK,0
	iret

timer_get_sup:
	add	gr96,SECS,0
	iret

timer_disable_sup:
	mfsr	it0,ops
	const	it1,TD
	consth	it1,TD
	andn	it0,it1,it0
	mtsr	ops,it0
	iret
#endif

	.global	_a29k_init_timer
_a29k_init_timer:
#if 0
	push	msp,lr0
	const	lr2,INIT_TIMER
	consth	lr2,INIT_TIMER
	const	lr3,timer_init_sup
	consth	lr3,timer_init_sup
	const	v0,__settrap
	consth	v0,__settrap
	calli	lr0,v0
	nop
	
	const	lr2,CLEAR_TIMER
	consth	lr2,CLEAR_TIMER
	const	lr3,timer_clear_sup
	consth	lr3,timer_clear_sup
	const	v0,__settrap
	consth	v0,__settrap
	calli	lr0,v0
	nop
	
	const	lr2,DISABLE_TIMER
	consth	lr2,DISABLE_TIMER
	const	lr3,timer_disable_sup
	consth	lr3,timer_disable_sup
	const	v0,__settrap
	consth	v0,__settrap
	calli	lr0,v0
	nop
	
	const	lr2,GET_TIMER
	consth	lr2,GET_TIMER
	const	lr3,timer_get_sup
	consth	lr3,timer_get_sup
	const	v0,__settrap
	consth	v0,__settrap
	calli	lr0,v0
	nop
	
	asneq	INIT_TIMER,gr1,gr1
	pop	lr0,msp
	jmpi	lr0
	nop
#endif

	.global	_a29k_clear_timer
_a29k_clear_timer:
#if 0
;	asneq	CLEAR_TIMER,gr1,gr1
	jmpi lr0
	nop
#endif

	.global	_a29k_disable_timer
_a29k_disable_timer:
#if 0
	asneq	DISABLE_TIMER,gr1,gr1
	jmpi lr0
	nop
#endif

	.global	_a29k_get_timer
_a29k_get_timer:
#if 0
	asneq	GET_TIMER,gr1,gr1
	jmpi lr0
	nop
#endif
