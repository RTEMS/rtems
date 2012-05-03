/*
 *  This file contains all assembly code for the
 *  NIOS2 implementation of RTEMS.
 *
 *  Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 *  Derived from no_cpu/cpu_asm.S, copyright (c) 1989-1999,
 *    On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>

    .set noat

    .globl _exception_vector

_exception_vector:

    /*
     * First, re-wind so we're pointed to the instruction where the exception
     * occurred.
     */

    addi ea, ea, -4

    /*
     * Now test to determine the cause of the exception.
     */

    /* TODO: Look at [ea] if there was an unknown/trap instruction */

    /* If interrupts are globally disabled, it certainly was no interrupt */
    rdctl et, estatus
    andi et, et, 1
    beq et, zero, _Exception_Handler

    /* If no interrupts are pending, it was a software exception */
    rdctl et, ipending
    beq et, zero, _Exception_Handler

    /*
     * Falling through to here means that this was a hardware interrupt.
     */

    br _ISR_Handler

/* =====================================================================
 * Exception handler:
 *   Responsible for unimplemented instructions and other software
 *   exceptions. Not responsible for hardware interrupts. Currently,
 *   software exceptions are regarded as error conditions, and the
 *   handling isn't perfect. */

_Exception_Handler:

    /* stw et, 108(sp') => stw et, -20(sp) */
    stw et, -20(sp)
    mov et, sp
    addi sp, sp, -128

    stw r1,   0(sp)
    stw r2,   4(sp)
    stw r3,   8(sp)

    rdctl r1, estatus
    rdctl r2, ienable
    rdctl r3, ipending

    stw r4,   12(sp)
    stw r5,   16(sp)
    stw r6,   20(sp)
    stw r7,   24(sp)
    stw r8,   28(sp)
    stw r9,   32(sp)
    stw r10,  36(sp)
    stw r11,  40(sp)
    stw r12,  44(sp)
    stw r13,  48(sp)
    stw r14,  52(sp)
    stw r15,  56(sp)
    stw r16,  60(sp)
    stw r17,  64(sp)
    stw r18,  68(sp)
    stw r19,  72(sp)
    stw r20,  76(sp)
    stw r21,  80(sp)
    stw r22,  84(sp)
    stw r23,  88(sp)
    stw gp,   92(sp)
    stw fp,   96(sp)
    /* sp */
    stw et,  100(sp)
    stw ra,  104(sp)
    /* stw et,  108(sp) */
    stw ea,  112(sp)

    /* status */
    stw r1, 116(sp)
    /* ienable */
    stw r2, 120(sp)
    /* ipending */
    stw r3, 124(sp)

    /*
     * Restore the global pointer.
     */

    movhi gp, %hiadj(_gp)
    addi gp, gp, %lo(_gp)

    /*
     * Pass a pointer to the stack frame as the input argument of the
     * exception handler (CPU_Exception_frame *).
     */

    mov r4, sp

    /*
     * Call the exception handler.
     */

    .extern __Exception_Handler
    call __Exception_Handler

stuck_in_exception:
    br stuck_in_exception

    /*
     * Restore the saved registers, so that all general purpose registers
     * have been restored to their state at the time the interrupt occured.
     */

    ldw r1,   0(sp)
    ldw r2,   4(sp)
    ldw r3,   8(sp)
    ldw r4,   12(sp)
    ldw r5,   16(sp)
    ldw r6,   20(sp)
    ldw r7,   24(sp)
    ldw r8,   28(sp)
    ldw r9,   32(sp)
    ldw r10,  36(sp)
    ldw r11,  40(sp)
    ldw r12,  44(sp)
    ldw r13,  48(sp)
    ldw r14,  52(sp)
    ldw r15,  56(sp)
    ldw r16,  60(sp)
    ldw r17,  64(sp)
    ldw r18,  68(sp)
    ldw r19,  72(sp)
    ldw r20,  76(sp)
    ldw r21,  80(sp)
    ldw r22,  84(sp)
    ldw r23,  88(sp)
    ldw gp,   92(sp)
    ldw fp,   96(sp)
    ldw ra,  104(sp)

    /* Disable interrupts */
    wrctl status, r0

    ldw ea,  112(sp)
    ldw et,  116(sp)

    /* FIXME: Enable interrupts after exception processing */
    ori et, et, 1
    wrctl estatus, et
    ldw et,  108(sp)

    /* Restore stack pointer */
    ldw sp,  100(sp)

    eret

/* ===================================================================== */

    .section .text

_ISR_Handler:

    /*
     * Process an external hardware interrupt.
     *
     * First, preserve all callee saved registers on
     * the stack. (See the Nios2 ABI documentation for details).
     *
     * Do we really need to save all?
     *
     * If this is interrupting a task (and not another interrupt),
     * everything is saved into the task's stack, thus putting us
     * in a situation similar to when the task calls a subroutine
     * (and only the CPU_Context_Control subset needs to be changed)
     */

    rdctl et, estatus

    /* Keep this in the same order as CPU_Interrupt_frame: */

    addi sp, sp, -76
    stw r1,  0(sp)
    stw r2,  4(sp)
    stw r3,  8(sp)
    stw r4,  12(sp)
    stw r5,  16(sp)
    stw r6,  20(sp)
    stw r7,  24(sp)
    stw r8,  28(sp)
    stw r9,  32(sp)
    stw r10, 36(sp)
    stw r11, 40(sp)
    stw r12, 44(sp)
    stw r13, 48(sp)
    stw r14, 52(sp)
    stw r15, 56(sp)
    stw ra,  60(sp)
    stw gp,  64(sp)
    /* et contains status */
    stw et,  68(sp)
    stw ea,  72(sp)

#if REMOVED_BY_CCJ
    /*
     * Obtain a bitlist of the pending interrupts.
     */

    rdctl et, ipending

    /*
     * Restore the global pointer to the expected value.
     */

    movhi gp, %hiadj(_gp)
    addi gp, gp, %lo(_gp)

    /*
     * Search through the bit list stored in r24(et) to find the first enabled
     * bit. The offset of this bit is the index of the interrupt that is
     * to be handled.
     */

    mov r4, zero
6:
    andi r3, r24, 1
    bne r3, zero, 7f
    addi r4, r4, 1
    srli r24, r24, 1
    br 6b
7:

    /*
     * Having located the interrupt source, r4 contains the index of the
     * interrupt to be handled. r5, the 2nd argument to the function,
     * will point to the CPU_Interrupt_frame.
     */

    mov     r5, sp
#endif
	
    .extern __ISR_Handler
    call    __ISR_Handler

    /*
     * Now that the interrupt processing is complete, prepare to return to
     * the interrupted code.
     */

    /*
     * Restore the saved registers, so that all general purpose registers
     * have been restored to their state at the time the interrupt occured.
     */

    ldw r1,   0(sp)
    ldw r2,   4(sp)
    ldw r3,   8(sp)
    ldw r4,  12(sp)
    ldw r5,  16(sp)
    ldw r6,  20(sp)
    ldw r7,  24(sp)
    ldw r8,  28(sp)
    ldw r9,  32(sp)
    ldw r10, 36(sp)
    ldw r11, 40(sp)
    ldw r12, 44(sp)
    ldw r13, 48(sp)
    ldw r14, 52(sp)
    ldw r15, 56(sp)
    ldw ra,  60(sp)
    ldw gp,  64(sp)

    /* Disable interrupts */
    wrctl status, r0

    /* Restore the exception registers */

    /* load saved ea into ea */
    ldw ea,  72(sp)
    /* load saved estatus into et */
    ldw et,  68(sp)
    /* Always have interrupts enabled when we return from interrupt */
    ori et, et, 1
    wrctl estatus, et
    /* Restore the stack pointer */
    addi sp, sp, 76

    /*
     * Return to the interrupted instruction.
     */
    eret


