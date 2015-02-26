/**
 * @file bsp-start-init-registers.S
 *
 * @brief ARM register initialization routines.
 */

/*
 * Copyright (c) 2015 Taller Technologies.  All rights reserved.
 *
 * @author Martin Galvan <martin.galvan@tallertechnologies.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * These routines initialize the core and VFP registers of ARM CPUs.
 * This is necessary for boards that operate in a 1oo1D fashion,
 * such as the TMS570.
 */

#include <rtems/asm.h>

.section .text
.syntax unified
.cpu cortex-r4
.arm

/* Initialization of the ARM core registers. */
FUNCTION_ENTRY(bsp_start_init_registers_core)
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	mov r9, #0
	mov r10, #0
	mov r11, #0
	mov r12, #0
	mov r13, #0

	bx lr
FUNCTION_END(bsp_start_init_registers_core)

/* Initialization of the FIQ mode banked registers. */
FUNCTION_ENTRY(bsp_start_init_registers_banked_fiq)
	mov r8, #0
	mov r9, #0
	mov r10, #0
	mov r11, #0
	mov r12, #0

	bx lr
FUNCTION_END(bsp_start_init_registers_banked_fiq)

#ifdef ARM_MULTILIB_VFP

/* Initialization of the FPU registers. */
FUNCTION_ENTRY(bsp_start_init_registers_vfp)
	mov r0, #0
	vmov d0, r0, r0
	vmov d1, r0, r0
	vmov d2, r0, r0
	vmov d3, r0, r0
	vmov d4, r0, r0
	vmov d5, r0, r0
	vmov d6, r0, r0
	vmov d7, r0, r0
	vmov d8, r0, r0
	vmov d9, r0, r0
	vmov d10, r0, r0
	vmov d11, r0, r0
	vmov d12, r0, r0
	vmov d13, r0, r0
	vmov d14, r0, r0
	vmov d15, r0, r0

#ifdef ARM_MULTILIB_VFP_D32
	vmov d16, r0, r0
	vmov d17, r0, r0
	vmov d18, r0, r0
	vmov d19, r0, r0
	vmov d20, r0, r0
	vmov d21, r0, r0
	vmov d22, r0, r0
	vmov d23, r0, r0
	vmov d24, r0, r0
	vmov d25, r0, r0
	vmov d26, r0, r0
	vmov d27, r0, r0
	vmov d28, r0, r0
	vmov d29, r0, r0
	vmov d30, r0, r0
	vmov d31, r0, r0
#endif /* ARM_MULTILIB_VFP_D32 */

	bx lr
FUNCTION_END(bsp_start_init_registers_vfp)

#endif /* ARM_MULTILIB_VFP */
