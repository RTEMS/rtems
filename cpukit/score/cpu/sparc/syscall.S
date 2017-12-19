/*
 *  systrap.S
 *
 *  This file contains emulated system calls using software trap 0.
 *  The following calls are supported:
 *
 *    + SYS_exit	(halt)
 *    + SYS_irqdis	(disable interrupts)
 *    + SYS_irqset	(set interrupt level)
 *
 *  COPYRIGHT:
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *  Copyright (c) 2016, 2017 embedded brains GmbH
 *
 *  This terms of the RTEMS license apply to this file.
 *
 */

#include <rtems/asm.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/percpu.h>
#include "syscall.h"

	.section    ".text"
	/*
	 *  system call - halt
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    g1 = system call id (1)
	 *
	 *  System Call 1 (exit):
	 *    g2 = additional exit code 1
	 *    g3 = additional exit code 2
	 */

	PUBLIC(syscall)

SYM(syscall):
	ta	0			! syscall 1, halt with %g1,%g2,%g3 info

	PUBLIC(sparc_syscall_exit)

SYM(sparc_syscall_exit):

	mov	SYS_exit, %g1
	mov	%o0, %g2	! Additional exit code 1
	mov	%o1, %g3	! Additional exit code 2
	ta	SPARC_SWTRAP_SYSCALL

	/*
	 *  system call - Interrupt Disable
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = psr | SPARC_PSR_PIL_MASK
	 *
	 *  On exit:
	 *    g1 = old psr (to user)
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_irqdis)

SYM(syscall_irqdis):
	mov	%l3, %psr			! Set PSR. Write delay 3 instr
	or	%l0, SPARC_PSR_ET_MASK, %g1	! return old PSR with ET=1
	nop					! PSR write delay
	jmp	%l2				! Return to after TA 9.
	 rett	%l2 + 4

	/*
	 *  system call - Interrupt Enable
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = psr & ~0x0f00
	 *    g1 = new PIL to write (from user)
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_irqen)

SYM(syscall_irqen):
	and	%g1, SPARC_PSR_PIL_MASK, %l4	! %l4 = (%g1 & 0xf00)
	wr	%l3, %l4, %psr			! PSR = (PSR & ~0xf00) ^ %l4
	nop; nop				! PSR write delay;
	jmp	%l2				! Return to after TA 10.
	 rett	%l2 + 4

#if defined(SPARC_USE_SYNCHRONOUS_FP_SWITCH)
	/*
	 *  system call - Interrupt disable and set PSR[EF] according to caller
	 *                specified %g1
	 *
	 *  On entry:
	 *
	 *    g1 = the desired PSR[EF] value (from caller)
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = psr | SPARC_PSR_PIL_MASK
	 *
	 *  On exit:
	 *    g1 = old psr (to user)
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_irqdis_fp)

SYM(syscall_irqdis_fp):
	/*
	 * We cannot use an intermediate value for operations with the PSR[EF]
	 * bit since they use a 13-bit sign extension and PSR[EF] is bit 12.
	 */
	sethi	%hi(SPARC_PSR_EF_MASK), %l4

	andn	%l3, %l4, %l3			! Clear PSR[EF]
	and	%g1, %l4, %g1			! Select PSR[EF] only from %g1
	or	%l3, %g1, %l3			! Set PSR[EF] according to %g1
	mov	%l3, %psr			! Set PSR. Write delay 3 instr
	or	%l0, SPARC_PSR_ET_MASK, %g1	! return old PSR with ET=1
	nop					! PSR write delay
	jmp	%l2				! Return to after TA 9.
	 rett	%l2 + 4
#endif

#if defined(SPARC_USE_LAZY_FP_SWITCH)

	/*
	 *  system call - Perform a lazy floating point switch
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = SPARC_PSR_EF_MASK
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_lazy_fp_switch)

SYM(syscall_lazy_fp_switch):
	ld	[%g6 + PER_CPU_OFFSET_EXECUTING], %l4
	ld	[%g6 + PER_CPU_ISR_NEST_LEVEL], %l5
	ld	[%l4 + %lo(SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET)], %l6
	ld	[%g6 + SPARC_PER_CPU_FP_OWNER_OFFSET], %l7

	/* Ensure that we are not in interrupt context */
	cmp	%l5, 0
	bne	.Lillegal_use_of_floating_point_unit
	 or	%l0, %l3, %l0

	/* Ensure that we are a proper floating point thread */
	cmp	%l6, 0
	be	.Lillegal_use_of_floating_point_unit
	 ld	[%l4 + %lo(SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET)], %l6

	/* Set PSR[EF] to 1, PSR write delay 3 instructions! */
	mov	%l0, %psr

	/*
	 * Check if there is a floating point owner.  We have to check this
	 * here, since the floating point owner may have been deleted in the
	 * meantime.  Save the floating point context if necessary.
	 */
	cmp	%l7, 0
	be	.Lfp_save_done
	 nop
	ld	[%l7 + %lo(SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET)], %l5
	std	%f0, [%l5 + SPARC_FP_CONTEXT_OFFSET_F0_F1]
	SPARC_LEON3FT_B2BST_NOP
	std	%f2, [%l5 + SPARC_FP_CONTEXT_OFFSET_F2_F3]
	SPARC_LEON3FT_B2BST_NOP
	std	%f4, [%l5 + SPARC_FP_CONTEXT_OFFSET_F4_F5]
	SPARC_LEON3FT_B2BST_NOP
	std	%f6, [%l5 + SPARC_FP_CONTEXT_OFFSET_F6_F7]
	SPARC_LEON3FT_B2BST_NOP
	std	%f8, [%l5 + SPARC_FP_CONTEXT_OFFSET_F8_F9]
	SPARC_LEON3FT_B2BST_NOP
	std	%f10, [%l5 + SPARC_FP_CONTEXT_OFFSET_F10_F11]
	SPARC_LEON3FT_B2BST_NOP
	std	%f12, [%l5 + SPARC_FP_CONTEXT_OFFSET_F12_F13]
	SPARC_LEON3FT_B2BST_NOP
	std	%f14, [%l5 + SPARC_FP_CONTEXT_OFFSET_F14_F15]
	SPARC_LEON3FT_B2BST_NOP
	std	%f16, [%l5 + SPARC_FP_CONTEXT_OFFSET_F16_F17]
	SPARC_LEON3FT_B2BST_NOP
	std	%f18, [%l5 + SPARC_FP_CONTEXT_OFFSET_F18_F19]
	SPARC_LEON3FT_B2BST_NOP
	std	%f20, [%l5 + SPARC_FP_CONTEXT_OFFSET_F20_F21]
	SPARC_LEON3FT_B2BST_NOP
	std	%f22, [%l5 + SPARC_FP_CONTEXT_OFFSET_F22_F23]
	SPARC_LEON3FT_B2BST_NOP
	std	%f24, [%l5 + SPARC_FP_CONTEXT_OFFSET_F24_F25]
	SPARC_LEON3FT_B2BST_NOP
	std	%f26, [%l5 + SPARC_FP_CONTEXT_OFFSET_F26_F27]
	SPARC_LEON3FT_B2BST_NOP
	std	%f28, [%l5 + SPARC_FP_CONTEXT_OFFSET_F28_F29]
	SPARC_LEON3FT_B2BST_NOP
	std	%f30, [%l5 + SPARC_FP_CONTEXT_OFFSET_F30_F31]
	SPARC_LEON3FT_B2BST_NOP
	st	%fsr, [%l5 + SPARC_FP_CONTEXT_OFFSET_FSR]
	st	%g0, [%g6 + SPARC_PER_CPU_FP_OWNER_OFFSET]
	st	%l5, [%l7 + %lo(SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET)]

.Lfp_save_done:

	/* Restore the floating point context if necessary */
	st	%g0, [%l4 + %lo(SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET)]
	cmp	%l6, 0
	be	.Lfp_restore_done
	 nop
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F0_F1], %f0
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F2_F3], %f2
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F4_F5], %f4
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F6_F7], %f6
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F8_F9], %f8
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F10_F11], %f10
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F12_F13], %f12
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F14_F15], %f14
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F16_F17], %f16
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F18_F19], %f18
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F20_F21], %f20
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F22_F23], %f22
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F24_F25], %f24
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F26_F27], %f26
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F28_F29], %f28
	ldd	[%l6 + SPARC_FP_CONTEXT_OFFSET_F30_F31], %f30
	ld	[%l6 + SPARC_FP_CONTEXT_OFFSET_FSR], %fsr

.Lfp_restore_done:

	/*
	 * Restore condition codes.  PSR[EF] is 1 here.  Take PSR write delay
	 * into account (maximum is three instructions).
	 */
	mov	%l0, %psr
	nop

	/* Now, retry the floating point instruction with PSR[EF] == 1 */
	jmp	%l1
	 rett	%l2

.Lillegal_use_of_floating_point_unit:

	/*
	 * There is no need to restore the condition codes here, since
	 * _Internal_error() does not return.
	 */
	sethi	%hi(_Internal_error), %l1
	or	%l1, %lo(_Internal_error), %l1
	mov	38, %i0
	jmp	%l1
	 rett	%l1 + 4
#endif

#if defined(RTEMS_PARAVIRT)

        PUBLIC(_SPARC_Get_PSR)

SYM(_SPARC_Get_PSR):

	retl
	 rd     %psr, %o0

        PUBLIC(_SPARC_Set_PSR)

SYM(_SPARC_Set_PSR):

	mov     %o0, %psr
	nop
	nop
	nop
	retl
	 nop

        PUBLIC(_SPARC_Get_TBR)

SYM(_SPARC_Get_TBR):

	retl
	 rd    %tbr, %o0

        PUBLIC(_SPARC_Set_TBR)

SYM(_SPARC_Set_TBR):

	retl
	 wr    %o0, 0, %tbr

#endif /* defined(RTEMS_PARAVIRT) */

/* end of file */
