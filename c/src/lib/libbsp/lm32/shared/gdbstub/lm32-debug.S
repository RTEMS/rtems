/*
 *  lm32 debug exception vectors
 *
 *  Michael Walle <michael@walle.cc>, 2009
 *
 *  If debugging is enabled the debug exception base address (deba) gets
 *  remapped to this file.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include "bspopts.h"

.section .text
/* (D)EBA alignment */
.align    256
.globl  _deba

_deba:
debug_reset_handler:
	/* Clear r0 */
	xor	r0,r0,r0
	/* Disable interrupts */
	wcsr	IE, r0
	/* Mask all interrupts */
	wcsr	IM,r0
	/* Jump to original crt0 */
	.extern crt0
	mvhi	r1, hi(crt0)
	ori	r1, r1, lo(crt0)
	b	r1
	nop
	nop
debug_breakpoint_handler:
	/* Clear r0 in case it was corrupted */
	xor	r0, r0, r0
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	sw	(r0+116), ra
	sw	(r0+128), ba
	calli	save_all
	calli	handle_exception
	calli	b_restore_and_return
debug_instruction_bus_error_handler:
	/* Clear r0 in case it was corrupted */
	xor	r0, r0, r0
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	sw	(r0+116), ra
	sw	(r0+128), ea
	calli	save_all
	calli	handle_exception
	calli	e_restore_and_return
debug_watchpoint_handler:
	/* Clear r0 in case it was corrupted */
	xor	r0, r0, r0
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	sw	(r0+116), ra
	sw	(r0+128), ba
	calli	save_all
	calli	handle_exception
	calli	b_restore_and_return
debug_data_bus_error_handler:
	/* Clear r0 in case it was corrupted */
	xor	r0, r0, r0
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	sw	(r0+116), ra
	sw	(r0+128), ea
	calli	save_all
	calli	handle_exception
	calli	e_restore_and_return
debug_divide_by_zero_handler:
	/* Clear r0 in case it was corrupted */
	xor	r0, r0, r0
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	sw	(r0+116), ra
	sw	(r0+128), ea
	calli	save_all
	calli	handle_exception
	calli	e_restore_and_return
debug_interrupt_handler:
	bi	debug_isr_handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop
debug_system_call_handler:
	/* Clear r0 in case it was corrupted */
	xor	r0, r0, r0
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	sw	(r0+116), ra
	sw	(r0+128), ea
	calli	save_all
	calli	handle_exception
	calli	e_restore_and_return

debug_isr_handler:
	addi	sp, sp, -156
	sw	(sp+4), r1
	sw	(sp+8), r2
	sw	(sp+12), r3
	sw	(sp+16), r4
	sw	(sp+20), r5
	sw	(sp+24), r6
	sw	(sp+28), r7
	sw	(sp+32), r8
	sw	(sp+36), r9
	sw	(sp+40), r10
	sw	(sp+44), ra
	sw	(sp+48), ea
	sw	(sp+52), ba
	sw	(sp+56), r11
	sw	(sp+60), r12
	sw	(sp+64), r13
	sw	(sp+68), r14
	sw	(sp+72), r15
	sw	(sp+76), r16
	sw	(sp+80), r17
	sw	(sp+84), r18
	sw	(sp+88), r19
	sw	(sp+92), r20
	sw	(sp+96), r21
	sw	(sp+100), r22
	sw	(sp+104), r23
	sw	(sp+108), r24
	sw	(sp+112), r25
	sw	(sp+116), r26
	sw	(sp+120), r27
	/* 124 - SP */
	addi	r1, sp, 156
	sw	(sp+124), r1
	/* 128 - PC */
	sw	(sp+128), ea
	/* 132 - EID */
	mvi	r1, 6
	sw	(sp+132), r1
	rcsr	r1, EBA
	sw	(sp+136), r1
	rcsr	r1, DEBA
	sw	(sp+140), r1
	rcsr	r1, IE
	sw	(sp+144), r1

	/* This is the same code as in cpu_asm.S */
	rcsr	r2, IP
	rcsr	r3, IM
	mv	r1, r0
	and	r2, r2, r3
	mvi	r3, 1
	be	r2, r0, 3f
1:
	and	r4, r2, r3
	bne	r4, r0, 2f
	sli	r3, r3, 1
	addi	r1, r1, 1
	bi	1b
2:
	addi	r2, sp, 4

	.extern __ISR_Handler
	mvhi	r3, hi(__ISR_Handler)
	ori	r3, r3, lo(__ISR_Handler)
	call	r3
3:
	lw	r1, (sp+4)
	lw	r2, (sp+8)
	lw	r3, (sp+12)
	lw	r4, (sp+16)
	lw	r5, (sp+20)
	lw	r6, (sp+24)
	lw	r7, (sp+28)
	lw	r8, (sp+32)
	lw	r9, (sp+36)
	lw	r10, (sp+40)
	lw	ra, (sp+44)
	lw	ea, (sp+48)
	lw	ba, (sp+52)
	lw	r11, (sp+56)
	lw	r12, (sp+60)
	lw	r13, (sp+64)
	lw	r14, (sp+68)
	lw	r15, (sp+72)
	lw	r16, (sp+76)
	lw	r17, (sp+80)
	lw	r18, (sp+84)
	lw	r19, (sp+88)
	lw	r20, (sp+92)
	lw	r21, (sp+96)
	lw	r22, (sp+100)
	lw	r23, (sp+104)
	lw	r24, (sp+108)
	lw	r25, (sp+112)
	lw	r26, (sp+116)
	lw	r27, (sp+120)
	lw	ea, (sp+136)
	wcsr	EBA, ea
	lw	ea, (sp+140)
	wcsr	DEBA, ea
	/* Restore EA from PC */
	lw	ea, (sp+128)
	/* Stack pointer must be restored last, in case it has been updated */
	lw	sp, (sp+124)
	eret

save_all:
	sw	(r0+4), r1
	sw	(r0+8), r2
	sw	(r0+12), r3
	sw	(r0+16), r4
	sw	(r0+20), r5
	sw	(r0+24), r6
	sw	(r0+28), r7
	sw	(r0+32), r8
	sw	(r0+36), r9
	sw	(r0+40), r10
	sw	(r0+44), r11
	sw	(r0+48), r12
	sw	(r0+52), r13
	sw	(r0+56), r14
	sw	(r0+60), r15
	sw	(r0+64), r16
	sw	(r0+68), r17
	sw	(r0+72), r18
	sw	(r0+76), r19
	sw	(r0+80), r20
	sw	(r0+84), r21
	sw	(r0+88), r22
	sw	(r0+92), r23
	sw	(r0+96), r24
	sw	(r0+100), r25
	sw	(r0+104), r26
	sw	(r0+108), r27
	sw	(r0+112), sp
	/* 116 - RA - saved in handler code above */
	sw	(r0+120), ea
	sw	(r0+124), ba
	/* 128 - PC - saved in handler code above */
	/* 132 - EID - saved below */
	rcsr	r1, EBA
	sw	(r0+136), r1
	rcsr	r1, DEBA
	sw	(r0+140), r1
	rcsr	r1, IE
	sw	(r0+144), r1

	/* Work out EID from exception entry point address */
	andi    r1, ra, 0xff
	srui    r1, r1, 5
	sw      (r0+132), r1

	/* Save pointer to registers */
	mv	r1, r0

	/* Restore r0 to 0 */
	xor	r0, r0, r0

	/* Save r0 (hardcoded to 0) */
	sw	(r1+0), r0
	ret


/* Restore gp registers */
restore_gp:
	lw	r1, (r0+4)
	lw	r2, (r0+8)
	lw	r3, (r0+12)
	lw	r4, (r0+16)
	lw	r5, (r0+20)
	lw	r6, (r0+24)
	lw	r7, (r0+28)
	lw	r8, (r0+32)
	lw	r9, (r0+36)
	lw	r10, (r0+40)
	lw	r11, (r0+44)
	lw	r12, (r0+48)
	lw	r13, (r0+52)
	lw	r14, (r0+56)
	lw	r15, (r0+60)
	lw	r16, (r0+64)
	lw	r17, (r0+68)
	lw	r18, (r0+72)
	lw	r19, (r0+76)
	lw	r20, (r0+80)
	lw	r21, (r0+84)
	lw	r22, (r0+88)
	lw	r23, (r0+92)
	lw	r24, (r0+96)
	lw	r25, (r0+100)
	lw	r26, (r0+104)
	lw	r27, (r0+108)
	ret

/* Restore registers and return from exception */
e_restore_and_return:
	/* first restore gp registers */
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	calli	restore_gp
	lw	sp, (r0+112)
	lw	ra, (r0+116)
	lw	ba, (r0+124)
	lw	ea, (r0+136)
	wcsr	EBA, ea
	lw	ea, (r0+140)
	wcsr	DEBA, ea
	/* Restore EA from PC */
	lw	ea, (r0+128)
	xor	r0, r0, r0
	eret

/* Restore registers and return from breakpoint */
b_restore_and_return:
	/* first restore gp registers */
	mvhi	r0, hi(registers)
	ori	r0, r0, lo(registers)
	calli   restore_gp
	lw	sp, (r0+112)
	lw	ra, (r0+116)
	lw	ea, (r0+120)
	lw	ba, (r0+136)
	wcsr	EBA, ba
	lw	ba, (r0+140)
	wcsr	DEBA, ba
	/* Restore BA from PC */
	lw	ba, (r0+128)
	xor	r0, r0, r0
	bret

