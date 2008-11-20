
#define SPL 0x3d
#define SPH 0x3e
#define SREG 0x3f
#define RAMPZ 0x3b

#define RAMEND (64 * 1024)

/*
	.weak	__stack
	.set	__stack, RAMEND - 1
*/

	.weak	__heap_end
	.set	__heap_end, 0


	.section .vectors,"ax",@progbits
	.global	__vectors
	.func	__vectors
__vectors:
__reset:
	jmp	__init
	.endfunc	

	.section .init0,"ax",@progbits
	.weak	__init
;	.func	__init
__init:
	
	.section .init2,"ax",@progbits
	clr	r1
	out	0x3f, r1
	ldi	r28,lo8(__stack)
	ldi	r29,hi8(__stack)
	out	SPH, r29
	out	SPL, r28

	/* Only for >64K devices with RAMPZ, replaces the default code
	   provided by libgcc.S which is only linked in if necessary.  */

	.section .init4,"ax",@progbits
	.global __do_copy_data
__do_copy_data:
	.global	__do_clear_bss
__do_clear_bss:
#if 0
	ldi	r17, hi8(__data_end)
	
	ldi	r26, lo8(__data_start)
	ldi	r27, hi8(__data_start)
	
	ldi	r30, lo8(__data_load_start)
	ldi	r31, hi8(__data_load_start)
	ldi	r16, hh8(__data_load_start)
	out	RAMPZ, r16

0:	
	cpi	r26, lo8(__data_end)
	cpc	r27, r17
	breq	1f
	
	elpm	r0, Z+
	st	X+, r0
	rjmp	0b
1:	
#endif

	.section .init9,"ax",@progbits
	call	boot_card
	jmp	exit
;	.endfunc

	.global	exit
	.func	exit
exit:	out 0x2f, r0
	.endfunc

	.global	abort
	.func	abort
abort:	out 0x29, r0
	.endfunc
