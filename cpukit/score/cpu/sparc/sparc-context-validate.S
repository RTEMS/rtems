/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/percpu.h>

#define FRAME_OFFSET_BUFFER_0 (SPARC_MINIMUM_STACK_FRAME_SIZE)
#define FRAME_OFFSET_BUFFER_1 (FRAME_OFFSET_BUFFER_0 + 0x04)
#define FRAME_OFFSET_BUFFER_2 (FRAME_OFFSET_BUFFER_1 + 0x04)
#define FRAME_OFFSET_L0 (FRAME_OFFSET_BUFFER_2 + 0x04)
#define FRAME_OFFSET_L1 (FRAME_OFFSET_L0 + 0x04)
#define FRAME_OFFSET_L2 (FRAME_OFFSET_L1 + 0x04)
#define FRAME_OFFSET_L3 (FRAME_OFFSET_L2 + 0x04)
#define FRAME_OFFSET_L4 (FRAME_OFFSET_L3 + 0x04)
#define FRAME_OFFSET_L5 (FRAME_OFFSET_L4 + 0x04)
#define FRAME_OFFSET_L6 (FRAME_OFFSET_L5 + 0x04)
#define FRAME_OFFSET_L7 (FRAME_OFFSET_L6 + 0x04)
#define FRAME_OFFSET_I0 (FRAME_OFFSET_L7 + 0x04)
#define FRAME_OFFSET_I1 (FRAME_OFFSET_I0 + 0x04)
#define FRAME_OFFSET_I2 (FRAME_OFFSET_I1 + 0x04)
#define FRAME_OFFSET_I3 (FRAME_OFFSET_I2 + 0x04)
#define FRAME_OFFSET_I4 (FRAME_OFFSET_I3 + 0x04)
#define FRAME_OFFSET_I5 (FRAME_OFFSET_I4 + 0x04)
#define FRAME_OFFSET_I6 (FRAME_OFFSET_I5 + 0x04)
#define FRAME_OFFSET_I7 (FRAME_OFFSET_I6 + 0x04)
#define FRAME_END (FRAME_OFFSET_I7 + 0x04)
#define FRAME_SIZE \
  ((FRAME_END + CPU_STACK_ALIGNMENT - 1) & ~(CPU_STACK_ALIGNMENT - 1))

/*
 * The FSR pattern is masked with undefined fields, reserved fields, ftt
 * (cleared by fmovs), cexc (cleared by fmovs), and system-specific values
 * (e.g. FPU architecture version, FP queue).
 */
#define FSR_PATTERN_MASK 0xcf800fe0

.macro check_register reg
	sub	%g1, 1, %g1
	cmp	%g1, \reg
	bne	restore_registers
	 nop
.endm

.macro check_float_register reg
	sub	%g1, 1, %g1
	st	\reg, [%sp + FRAME_OFFSET_BUFFER_0]
	cmp	%g0, %sp
	fmovs	\reg, \reg
	be	restore_registers
	 nop
	cmp	%g0, %g0
	fmovs	\reg, \reg
	bne	restore_registers
	 nop
	ld	[%sp + FRAME_OFFSET_BUFFER_0], %o1
	cmp	%g1, %o1
	bne	restore_registers
	 nop
.endm

.macro write_register reg
	add	%g1, 1, %g1
	mov	%g1, \reg
.endm

.macro write_float_register reg
	add	%g1, 1, %g1
	st	%g1, [%sp + FRAME_OFFSET_BUFFER_0]
	ld	[%sp + FRAME_OFFSET_BUFFER_0], \reg
.endm

        .align 4
        PUBLIC(_CPU_Context_validate)
SYM(_CPU_Context_validate):

	/* g2 indicates if the FPU should be checked */
#if defined(SPARC_USE_LAZY_FP_SWITCH)
	ld	[%g6 + PER_CPU_OFFSET_EXECUTING], %g2
	ld	[%g2 + %lo(SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET)], %g2
#else
	mov	%psr, %g2
	sethi	%hi(SPARC_PSR_EF_MASK), %g3
	and	%g2, %g3, %g2
#endif

	/* g1 is used to save the original pattern */
	mov	%o0, %g1

	/* g4 establishes window counter */
	clr	%g4

	add	%sp, -FRAME_SIZE, %sp

	st	%l0, [%sp + FRAME_OFFSET_L0]
	st	%l1, [%sp + FRAME_OFFSET_L1]
	st	%l2, [%sp + FRAME_OFFSET_L2]
	st	%l3, [%sp + FRAME_OFFSET_L3]
	st	%l4, [%sp + FRAME_OFFSET_L4]
	st	%l5, [%sp + FRAME_OFFSET_L5]
	st	%l6, [%sp + FRAME_OFFSET_L6]
	st	%l7, [%sp + FRAME_OFFSET_L7]
	st	%i0, [%sp + FRAME_OFFSET_I0]
	st	%i1, [%sp + FRAME_OFFSET_I1]
	st	%i2, [%sp + FRAME_OFFSET_I2]
	st	%i3, [%sp + FRAME_OFFSET_I3]
	st	%i4, [%sp + FRAME_OFFSET_I4]
	st	%i5, [%sp + FRAME_OFFSET_I5]
	st	%i6, [%sp + FRAME_OFFSET_I6]
	st	%i7, [%sp + FRAME_OFFSET_I7]

	cmp	%g4, 0
	bne	write_locals_and_outputs
	 nop
	be	check_for_fp
	 nop

new_check_cycle:
	clr	%g4
	add	%g4, 1, %g4
	ld	[%sp + FRAME_OFFSET_BUFFER_1], %g1
	b	switch_to_next_window
	 nop
	/* Write pattern values into registers */

check_for_fp:
	cmp	%g2, 0
	be	write_y
	 nop

	/* Write masked pattern to FSR */
	st	%fsr, [%sp + FRAME_OFFSET_BUFFER_0]
	ld	[%sp + FRAME_OFFSET_BUFFER_0], %o1
	add	%g1, 1, %g1
	sethi	%hi(FSR_PATTERN_MASK), %g3
	or	%g3, %lo(FSR_PATTERN_MASK), %g3
	and	%g1, %g3, %g3
	or	%o1, %g3, %g3
	st	%g3, [%sp + FRAME_OFFSET_BUFFER_0]
	ld	[%sp + FRAME_OFFSET_BUFFER_0], %fsr

	write_float_register	%f0
	write_float_register	%f1
	write_float_register	%f2
	write_float_register	%f3
	write_float_register	%f4
	write_float_register	%f5
	write_float_register	%f6
	write_float_register	%f7
	write_float_register	%f8
	write_float_register	%f9
	write_float_register	%f10
	write_float_register	%f11
	write_float_register	%f12
	write_float_register	%f13
	write_float_register	%f14
	write_float_register	%f15
	write_float_register	%f16
	write_float_register	%f17
	write_float_register	%f18
	write_float_register	%f19
	write_float_register	%f20
	write_float_register	%f21
	write_float_register	%f22
	write_float_register	%f23
	write_float_register	%f24
	write_float_register	%f25
	write_float_register	%f26
	write_float_register	%f27
	write_float_register	%f28
	write_float_register	%f29
	write_float_register	%f30
	write_float_register	%f31

write_y:
	write_register	%y

	write_register	%i0
	write_register	%i1
	write_register	%i2
	write_register	%i3
	write_register	%i4
	write_register	%i5
	/* Don't write register $i6 => frame pointer */
	/* Don't write register $i7 => return address */
	b	write_locals_and_outputs
	 nop

switch_to_next_window:
	save	%sp, -FRAME_SIZE, %sp

write_locals_and_outputs:
	/* l0 is used as a scratch register */
	write_register	%l1
	write_register	%l2
	write_register	%l3
	write_register	%l4
	write_register	%l5
	write_register	%l6
	write_register	%l7
	write_register	%o1
	write_register	%o2
	write_register	%o3
	write_register	%o4
	write_register	%o5
	/* Don't write register $o6 => stack pointer */
	/* Don't write register $o7 => return address */

	add	%g4, 1, %g4
	cmp	%g4, 1
	bne	no_store
	 nop
	st	%g1, [%sp + FRAME_OFFSET_BUFFER_1]

no_store:
	cmp	%g4, SPARC_NUMBER_OF_REGISTER_WINDOWS
	bne	switch_to_next_window
	 nop

	/* Dummy increment to set up reverse mechanism for checking process */
	add	%g1, 1, %g1
	clr	%g4

	/* Checking begins here */
window_checking:
	cmp	%g4, SPARC_NUMBER_OF_REGISTER_WINDOWS
	be	y_checking
	 nop

further_checking:
	cmp	%g4, 0
	bne	goto_local_registers
	 nop

	/* Check normal registers */
	check_register	%o5
	check_register	%o4
	check_register	%o3
	check_register	%o2
	check_register	%o1

goto_local_registers:
	check_register	%l7
	check_register	%l6
	check_register	%l5
	check_register	%l4
	check_register	%l3
	check_register	%l2
	check_register	%l1

	check_register	%i5
	check_register	%i4
	check_register	%i3
	check_register	%i2
	check_register	%i1
	/*
	For the last window i0 also needs to be checked as this variable
	is not overwritten by the outputs of another window.
	*/
	add	%g4, 1, %g4
	cmp	%g4, SPARC_NUMBER_OF_REGISTER_WINDOWS
	bne	dont_check_i0
	 nop
	check_register	%i0
	b	y_checking
	 nop

dont_check_i0:
	restore

	ba	window_checking
	 nop

	/* Check Y register */
y_checking:
	st	%o1, [%sp + FRAME_OFFSET_BUFFER_2]
	mov	%y, %o1
	check_register	%o1
	ld	[%sp + FRAME_OFFSET_BUFFER_2], %o1
	cmp	%g2, 0
	be	new_check_cycle
	 nop

	st	%o1, [%sp + FRAME_OFFSET_BUFFER_2]
	SPARC_LEON3FT_B2BST_NOP
	/* Check floating point registers */
	check_float_register	%f31
	check_float_register	%f30
	check_float_register	%f29
	check_float_register	%f28
	check_float_register	%f27
	check_float_register	%f26
	check_float_register	%f25
	check_float_register	%f24
	check_float_register	%f23
	check_float_register	%f22
	check_float_register	%f21
	check_float_register	%f20
	check_float_register	%f19
	check_float_register	%f18
	check_float_register	%f17
	check_float_register	%f16
	check_float_register	%f15
	check_float_register	%f14
	check_float_register	%f13
	check_float_register	%f12
	check_float_register	%f11
	check_float_register	%f10
	check_float_register	%f9
	check_float_register	%f8
	check_float_register	%f7
	check_float_register	%f6
	check_float_register	%f5
	check_float_register	%f4
	check_float_register	%f3
	check_float_register	%f2
	check_float_register	%f1
	check_float_register	%f0

	st	%fsr, [%sp + FRAME_OFFSET_BUFFER_0]
	ld	[%sp + FRAME_OFFSET_BUFFER_0], %o1
	sub	%g1, 1, %g1
	clr	%g3
	sethi	%hi(FSR_PATTERN_MASK), %g3
	or	%g3, %lo(FSR_PATTERN_MASK), %g3
	and	%g1, %g3, %g3
	and	%o1, %g3, %o1
	cmp	%o1, %g3
	bne	restore_registers
	 ld	[%sp + FRAME_OFFSET_BUFFER_2], %o1

	b	new_check_cycle
	 nop

	/****** RESTORE STARTS HERE *******/

	/* Restore non-volatile registers */

restore_registers:
	and	%g4, (SPARC_NUMBER_OF_REGISTER_WINDOWS - 1), %g4
	cmp	%g4, 0
	be	real_restore
	 nop
	restore
	sub	%g4, 1, %g4
	bne	restore_registers
	 nop

real_restore:
	ld	[%sp + FRAME_OFFSET_L0], %l0
	ld	[%sp + FRAME_OFFSET_L1], %l1
	ld	[%sp + FRAME_OFFSET_L2], %l2
	ld	[%sp + FRAME_OFFSET_L3], %l3
	ld	[%sp + FRAME_OFFSET_L4], %l4
	ld	[%sp + FRAME_OFFSET_L5], %l5
	ld	[%sp + FRAME_OFFSET_L6], %l6
	ld	[%sp + FRAME_OFFSET_L7], %l7
	ld	[%sp + FRAME_OFFSET_I0], %i0
	ld	[%sp + FRAME_OFFSET_I1], %i1
	ld	[%sp + FRAME_OFFSET_I2], %i2
	ld	[%sp + FRAME_OFFSET_I3], %i3
	ld	[%sp + FRAME_OFFSET_I4], %i4
	ld	[%sp + FRAME_OFFSET_I5], %i5
	ld	[%sp + FRAME_OFFSET_I6], %i6
	ld	[%sp + FRAME_OFFSET_I7], %i7

	sub	%sp, -FRAME_SIZE, %sp

return_value:
	/* Load callback address and jump back */
	jmp	%o7 + 8
	 add	%sp, FRAME_SIZE, %sp
