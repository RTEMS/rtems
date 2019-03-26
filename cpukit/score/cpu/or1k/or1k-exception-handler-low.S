/**
 * @file
 *
 * @addtogroup RTEMSScoreCPU
 *
 * @brief OR1K exception support implementation.
 */

/*
 *  COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include "rtems/score/or1k-utility.h"

.align 4
.text
PUBLIC(_ISR_Handler)
.type    _ISR_Handler,@function

 SYM(_ISR_Handler):

  l.addi  r1, r1, -140

  l.sw  8(r1),r2
  /* r3 is saved by BSP exception handler */
  l.sw  16(r1),r4
  l.sw  20(r1),r5
  l.sw  24(r1),r6
  l.sw  28(r1),r7
  l.sw  32(r1),r8
  l.sw  36(r1),r9
  l.sw  40(r1),r10
  l.sw  44(r1),r11
  l.sw  48(r1),r12
  l.sw  52(r1),r13
  l.sw  56(r1),r14
  l.sw  60(r1),r15
  l.sw  64(r1),r16
  l.sw  68(r1),r17
  l.sw  72(r1),r18
  l.sw  76(r1),r19
  l.sw  80(r1),r20
  l.sw  84(r1),r21
  l.sw  88(r1),r22
  l.sw  92(r1),r23
  l.sw  96(r1),r24
  l.sw  100(r1),r25
  l.sw  104(r1),r26
  l.sw  108(r1),r27
  l.sw  112(r1),r28
  l.sw  116(r1),r29
  l.sw  120(r1),r30
  l.sw  124(r1),r31

  /* Exception level related registers */

  /* EPCR */
  l.mfspr r13, r0, CPU_OR1K_SPR_EPCR0
  l.sw  128(r1), r13 /* epcr */

  /* EEAR */
  l.mfspr r13, r0, CPU_OR1K_SPR_EEAR0
  l.sw  132(r1), r13 /* eear */

  /* ESR */
  l.mfspr r13, r0, CPU_OR1K_SPR_ESR0
  l.sw  136(r1), r13  /* esr */

  /* Increment nesting level */
  l.movhi r6, hi(ISR_NEST_LEVEL)
  l.ori   r6, r6, lo(ISR_NEST_LEVEL)

  /* Disable multitasking */
  l.movhi r8, hi(THREAD_DISPATCH_DISABLE_LEVEL)
  l.ori   r8, r8, lo(THREAD_DISPATCH_DISABLE_LEVEL)

  l.lwz   r5, 0(r6)
  l.lwz   r7, 0(r8)
  l.addi  r5, r5, 1
  l.addi  r7, r7, 1
  l.sw    0(r6), r5
  l.sw    0(r8), r7

  /* Save interrupted task stack pointer */
  l.addi r4, r1, 340
  l.sw   4(r1), r4

  /* Save interrupted task r3 (first arg) value */
  l.addi r4, r1, 140
  l.lwz  r4, 0(r4)
  l.sw  12(r1), r4

  /* Keep r1 (Exception frame address) in r14 */
  l.add   r14, r1, r0

  /* Call the exception handler from vector table */

  /* First function arg for C handler is vector number,
   * and the second is a pointer to exception frame.
   */
  l.add  r13, r3, r0
  l.add  r4, r1, r0
  l.slli r13, r13, 2
  l.addi r13, r13, lo(bsp_start_vector_table_begin)
  l.lwz  r13, 0(r13)

  /* Do not switch stacks if we are in a nested interrupt. At
   * this point r5 should be holding ISR_NEST_LEVEL value.
   */
  l.sfgtui r5, 1
  l.bf jump_to_c_handler
  l.nop

   /* Switch to RTEMS dedicated interrupt stack */
  l.movhi r1, hi(INTERRUPT_STACK_HIGH)
  l.ori   r1, r1, lo(INTERRUPT_STACK_HIGH)
  l.lwz   r1, 0(r1)

jump_to_c_handler:
  l.jalr r13
  l.nop

  /* Switch back to the interrupted task stack */
  l.add r1, r14, r0

  /* Decrement nesting level */
  l.movhi r6, hi(ISR_NEST_LEVEL)
  l.ori   r6, r6, lo(ISR_NEST_LEVEL)

  /* Enable multitasking */
  l.movhi r8, hi(THREAD_DISPATCH_DISABLE_LEVEL)
  l.ori   r8, r8, lo(THREAD_DISPATCH_DISABLE_LEVEL)

  l.lwz   r5, 0(r6)
  l.lwz   r7, 0(r8)
  l.addi  r5, r5, -1
  l.addi  r7, r7, -1
  l.sw    0(r6), r5
  l.sw    0(r8), r7

  /* Check if _ISR_Nest_level > 0 */
  l.sfgtui r5, 0
  l.bf exception_frame_restore
  l.nop

  /* Check if _Thread_Dispatch_disable_level > 0 */
  l.sfgtui r7, 0
  l.bf exception_frame_restore
  l.nop

  /* Check if dispatch needed */
  l.movhi r31, hi(DISPATCH_NEEDED)
  l.ori   r31, r31, lo(DISPATCH_NEEDED)
  l.lwz   r31, 0(r31)
  l.sfeq  r31, r0
  l.bf    exception_frame_restore
  l.nop

  l.movhi r13, hi(_Thread_Dispatch)
  l.ori   r13, r13, lo(_Thread_Dispatch)
  l.jalr  r13
  l.nop

 SYM(exception_frame_restore):

  /* Exception level related registers */

  /* EPCR */
  l.lwz  r13,  128(r1)
  l.mtspr r0, r13, CPU_OR1K_SPR_EPCR0

  /* EEAR */
  l.lwz  r13,  132(r1)
  l.mtspr r0, r13, CPU_OR1K_SPR_EEAR0

  /* ESR */
  l.lwz  r13,  136(r1)
  l.mtspr r0, r13, CPU_OR1K_SPR_ESR0

  l.lwz  r2,  8(r1)
  l.lwz  r3,  12(r1)
  l.lwz  r4,  16(r1)
  l.lwz  r5,  20(r1)
  l.lwz  r6,  24(r1)
  l.lwz  r7,  28(r1)
  l.lwz  r8,  32(r1)
  l.lwz  r9,  36(r1)
  l.lwz  r10, 40(r1)
  l.lwz  r11, 44(r1)
  l.lwz  r12, 48(r1)
  l.lwz  r13, 52(r1)
  l.lwz  r14, 56(r1)
  l.lwz  r15, 60(r1)
  l.lwz  r16, 64(r1)
  l.lwz  r17, 68(r1)
  l.lwz  r18, 72(r1)
  l.lwz  r19, 76(r1)
  l.lwz  r20, 80(r1)
  l.lwz  r21, 84(r1)
  l.lwz  r22, 88(r1)
  l.lwz  r23, 92(r1)
  l.lwz  r24, 96(r1)
  l.lwz  r25, 100(r1)
  l.lwz  r26, 104(r1)
  l.lwz  r27, 108(r1)
  l.lwz  r28, 112(r1)
  l.lwz  r29, 116(r1)
  l.lwz  r30, 120(r1)
  l.lwz  r31, 124(r1)

  /* Unwind exception frame */
  l.addi r1, r1, 140

  /* Red-zone */
  l.addi r1, r1, 200

  l.rfe
  l.nop
