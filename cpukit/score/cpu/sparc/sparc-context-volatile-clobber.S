/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpuimpl.h>

#define SCRATCH_0 (SPARC_MINIMUM_STACK_FRAME_SIZE)
#define SCRATCH_1 (SCRATCH_0 + 0x04)
#define FRAME_END (SCRATCH_1 + 0x04)
#define FRAME_SIZE \
  ((FRAME_END + CPU_STACK_ALIGNMENT - 1) & ~(CPU_STACK_ALIGNMENT - 1))

.macro clobber_register reg
	sub	%g2, 1, %g2
	mov	%g2, \reg
.endm

.macro clobber_fp_register reg
	sub	%g2, 1, %g2
	st	%g2, [%sp + SCRATCH_0]
	ld	[%sp + SCRATCH_0], \reg
.endm

	.section	".bss"
	.align 4

	/*
	 * Use a global variable to vary the clobbered windows in each
	 * invocation to test the window overflow and underflow conditions.
	 */
window_clobber_count:
	.skip	4

	.section	".text"
	.align	4

        PUBLIC(_CPU_Context_volatile_clobber)
SYM(_CPU_Context_volatile_clobber):

	/* Increment number of flushed windows by one */
	sethi	%hi(window_clobber_count), %o1
	ld	[%o1 + %lo(window_clobber_count)], %o2
	add	%o2, 1, %o2
	st	%o2, [%o1 + %lo(window_clobber_count)]

	/* Clear window counter number */
	clr	%g1

	/* Save pattern to global register */
	mov	%o0, %g2

window_clobber:

	/* Switch window */

	save	%sp, -FRAME_SIZE, %sp

	/* Check how many windows shall be flushed */
	sethi	%hi(window_clobber_count), %o1
	ld	[%o1 + %lo(window_clobber_count)], %o2
	st	%o2, [%o1 + %lo(window_clobber_count)]
	and	%o2, (SPARC_NUMBER_OF_REGISTER_WINDOWS - 1), %o1
	cmp	%o1, 0
	bne	no_manual_update
	 nop
	add	%o1, SPARC_NUMBER_OF_REGISTER_WINDOWS, %o1

no_manual_update:
	/* Register to determine whether FPU is switched on */
	mov	%psr, %o2
	sethi	%hi(SPARC_PSR_EF_MASK), %o3
	and 	%o3, %o2, %o2

	clobber_register	%o3
	clobber_register	%o4
	clobber_register	%o5
	/* Don't overwrite return address $o7 */
	clobber_register	%g3
	clobber_register	%g4
	clobber_register	%y

	cmp	%o2, 0
	be	window_update_check
	 nop

	clobber_fp_register	%f0
	clobber_fp_register	%f1
	clobber_fp_register	%f2
	clobber_fp_register	%f3
	clobber_fp_register	%f4
	clobber_fp_register	%f5
	clobber_fp_register	%f6
	clobber_fp_register	%f7
	clobber_fp_register	%f8
	clobber_fp_register	%f9
	clobber_fp_register	%f10
	clobber_fp_register	%f11
	clobber_fp_register	%f12
	clobber_fp_register	%f13
	clobber_fp_register	%f14
	clobber_fp_register	%f15
	clobber_fp_register	%f16
	clobber_fp_register	%f17
	clobber_fp_register	%f18
	clobber_fp_register	%f19
	clobber_fp_register	%f20
	clobber_fp_register	%f21
	clobber_fp_register	%f22
	clobber_fp_register	%f23
	clobber_fp_register	%f24
	clobber_fp_register	%f25
	clobber_fp_register	%f26
	clobber_fp_register	%f27
	clobber_fp_register	%f28
	clobber_fp_register	%f29
	clobber_fp_register	%f30
	clobber_fp_register	%f31

window_update_check:

	/* Counter to how many windows were switched */
	add	%g1, 1, %g1
	cmp	%g1, %o1
	bl	window_clobber
	 nop

restore_check:

	cmp	%g1, 0
	be	clobber_return
	 nop

	restore
	sub	%g1, 1, %g1
	ba	restore_check
	 nop

clobber_return:

	jmp	%o7 + 8
	 nop
