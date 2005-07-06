/**
 *  @file cpu_asm.S
 *
 *  This file contains the implementation of exception handlers.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2002 by Advent Networks, Inc.
 *      Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *      Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2004
 *      Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define __asm__
#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>
#include <asm_macros.h>
/* @cond  INCLUDE_ASM */

/**
 *  This routine performs a normal non-FP context.
 *  function void _CPU_Context_switch( run_context, heir_context )
 *  R0 = run_context    R1 = heir_context
 *
 *  This function copies the current registers to where r0 points, then
 *  restores the ones from where r1 points.
 *
 *  Using the ldm/stm opcodes save 2-3 us on 100 MHz ARM9TDMI with
 *  a 16 bit data bus.
 *
 */
    .align
/* .section  .iwram   */
PUBLIC_ARM_FUNCTION(_CPU_Context_switch)
/* Start saving context */
	mrs	r2, cpsr
	stmia	r0,  {r2, r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}
/* Start restoring context */
_restore:
	ldmia	r1,  {r2, r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}
	msr	cpsr, r2
	mov	pc, lr
LABEL_END(_CPU_Context_switch)

/**
 * This function copies the restores the registers from where r0 points.
 * function void _CPU_Context_restore( new_context )
 * It must match _CPU_Context_switch()
 *
 */
PUBLIC_ARM_FUNCTION(_CPU_Context_restore)
        mov     r1, r0
        b       _restore
LABEL_END(_CPU_Context_restore)

/**
 * function _Exception_Handler_Undef_Swi
 * Can't use exception vectors in GBA
 * @todo  _Exception_Handler_Undef_Swi: Unused handler needed by ../score/cpu_asm.S
 *
 */
        .global _Exception_Handler_Undef_Swi
_Exception_Handler_Undef_Swi:
	mov	pc, lr

/**
 * function _Exception_Handler_Abort
 * Can't use exception vectors in GBA
 * @todo  _Exception_Handler_Abort: Unused handler needed by ../score/cpu_asm.S
 *
 */
        .global _Exception_Handler_Abort
_Exception_Handler_Abort:
	mov	pc, lr
/* @endcond     */
