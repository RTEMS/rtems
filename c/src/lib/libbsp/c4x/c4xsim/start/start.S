/*
 *  This start.S is a combination of the files boot.s and crt0.s
 *  provided by Herman ...XXX
 */

	; .file	"crt0.s"
	.ref	_c_int00
	.global	start
	.global	_start

	.if	.tms320C40

	.sect	".const"
mem_control:
	.word	000100000h
mem_data:
	.word	03ef78050h

	.text
start:
_start:
	ldi	0800h,st
	ldp	@mem_control
	ldi	@mem_control,ar0
	ldp	@mem_data
	ldi	@mem_data,r0
	sti	r0,*+ar0(0)
	sti	r0,*+ar0(4)
	br	_c_int00

	.else

	.sect	".const"
mem_control:
	.word	000808000h
mem_data:
	.word	000001f00h

	.text
start:
_start:
	ldi	0800h,st
	ldp	@mem_control
	ldi	@mem_control,ar0
	ldp	@mem_data
	ldi	@mem_data,r0
	sti	r0,*+ar0(0)
	sti	r0,*+ar0(4)
	br	_c_int00

	.endif
	; .file	"boot.s"
	.global	__stack
	.global	_c_int00
	.ref	.bss
	.ref	_boot_card
	.ref	_exit

__stack: .usect  ".stack",0
        .text
stack_addr:  .word  __stack

_c_int00:
        ldp     stack_addr
        ldi     @stack_addr,sp
        ldi     sp,ar3
        ldp     .bss		; For the small model, set up the DP to .bss
	ldi	0,ar2		; make sure cmdline=0 with regparm
	push	ar2		; and memparm
        call    _boot_card
        call	_sim_exit
