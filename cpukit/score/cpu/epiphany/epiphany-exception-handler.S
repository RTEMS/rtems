/**
 * @file
 *
 * @addtogroup RTEMSScoreCPU
 *
 * @brief Epiphany exception support implementation.
 */

/*
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/cpu.h>

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

EXTERN(bsp_start_vector_table_begin)
EXTERN(_Thread_Dispatch)
PUBLIC(_ISR_Handler)

.section .text, "ax"
.align 4
TYPE_FUNC(_ISR_Handler)
SYM(_ISR_Handler):
  /* Reserve space for CPU_Exception_frame */
  sub sp, sp, #(CPU_EXCEPTION_FRAME_SIZE)

  str r0,  [sp]
  str r1,  [sp,1]
  str r2,  [sp,2]
  str r3,  [sp,3]
  str r4,  [sp,4]
  str r5,  [sp,5]
  str r6,  [sp,6]
  str r7,  [sp,7]
  str r8,  [sp,8]
  str r9,  [sp,9]
  str r10, [sp,10]
  str fp,  [sp,11]
  str r12, [sp,12]

  /* Save interrupted task stack pointer */
  add r1, sp, #(CPU_EXCEPTION_FRAME_SIZE + 8)
  str r1,[sp,13]

  str lr,  [sp,14]
  str r15, [sp,15]
  str r16, [sp,16]
  str r17, [sp,17]
  str r18, [sp,18]
  str r19, [sp,19]
  str r20, [sp,20]
  str r21, [sp,21]
  str r22, [sp,22]
  str r23, [sp,23]
  str r24, [sp,24]
  str r25, [sp,25]
  str r26, [sp,26]
  str r27, [sp,27]
  str r28, [sp,28]
  str r29, [sp,29]
  str r30, [sp,30]
  str r31, [sp,31]
  str r32, [sp,32]
  str r33, [sp,33]
  str r34, [sp,34]
  str r35, [sp,35]
  str r36, [sp,36]
  str r37, [sp,37]
  str r38, [sp,38]
  str r39, [sp,39]
  str r40, [sp,40]
  str r41, [sp,41]
  str r42, [sp,42]
  str r43, [sp,43]
  str r44, [sp,44]
  str r45, [sp,45]
  str r46, [sp,46]
  str r47, [sp,47]
  str r48, [sp,48]
  str r49, [sp,49]
  str r50, [sp,50]
  str r51, [sp,51]
  str r52, [sp,52]
  str r53, [sp,53]
  str r54, [sp,54]
  str r55, [sp,55]
  str r56, [sp,56]
  str r57, [sp,57]
  str r58, [sp,58]
  str r59, [sp,59]
  str r60, [sp,60]
  str r61, [sp,61]
  /* r62 and r63 are saved from start.S interrupt entry
   * and hold vector number and _ISR_Handler address repsectively.
   */

  /* Save status register */
  movfs r1,status
  str r1, [sp,62]

  /* Save config register */
  movfs r1,config
  str r1, [sp,63]

  /* Save interrupt return address register */
  movfs r1,iret
  str r1, [sp,64]

  mov  r33, %low(_Per_CPU_Information)
  movt r33, %high(_Per_CPU_Information)

       add  r6, r33, #(PER_CPU_ISR_NEST_LEVEL)
       add  r8, r33, #(PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)

  /* Increment nesting level and disable thread dispatch */
  ldr  r5, [r6]
  ldr  r7, [r8]
  add  r5, r5, #1
  add  r7, r7, #1
  str  r5, [r6]
  str  r7, [r8]

  /* Keep sp (Exception frame address) in r32 - Callee saved */
  mov  r32, sp

  /* Keep _Per_CPU_Information address in r33 - Callee saved */
  mov  r33, r18

  /* Call the exception handler from vector table.
   * First function arg for C handler is vector number,
   * and the second is a pointer to exception frame.
   */
  mov  r0,  r62
  mov  r1,  sp

  mov  r27, r62
  lsl  r27, r27, #2
  mov  r26, %low(bsp_start_vector_table_begin)
  movt r15, #0
  add  r27, r27, r26
  ldr  r27, [r27]

  /* Do not switch stacks if we are in a nested interrupt. At
   * this point r5 should be holding ISR_NEST_LEVEL value.
   */
  sub  r37, r5, #1
  bgtu jump_to_c_handler

  /* Switch to RTEMS dedicated interrupt stack */
  add     sp, r18, #(PER_CPU_INTERRUPT_STACK_HIGH)
  ldr     sp, [sp]

jump_to_c_handler:
  jalr r27

  /* Switch back to the interrupted task stack */
  mov  sp, r32

  /* Get the address of _Per_CPU_Information */
  mov r18, r33

  /* Decrement nesting level and enable multitasking */
  add  r6, r18, #(PER_CPU_ISR_NEST_LEVEL)
       add  r8, r18, #(PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)

  ldr     r5, [r6]
  ldr     r7, [r8]
  sub     r5, r5, #1
  sub     r7, r7, #1
  str     r5, [r6]
  str     r7, [r8]

  /* Check if _ISR_Nest_level > 0 */
  sub     r37, r5, #0
  bgtu    exception_frame_restore

  /* Check if _Thread_Dispatch_disable_level > 0 */
  sub     r37, r7, #0
  bgtu    exception_frame_restore

  /* Check if dispatch needed */
  add     r31, r18, #(PER_CPU_DISPATCH_NEEDED)
  ldr     r31, [r31]

  sub     r35, r31, #0
  beq     exception_frame_restore

  mov     r35, %low(_Thread_Dispatch)
  movt    r35, %high(_Thread_Dispatch)
  jalr    r35

exception_frame_restore:

  ldr r1,  [sp,1]
  ldr r2,  [sp,2]
  ldr r3,  [sp,3]
  ldr r4,  [sp,4]
  ldr r5,  [sp,5]
  ldr r6,  [sp,6]
  ldr r7,  [sp,7]
  ldr r8,  [sp,8]
  ldr r9,  [sp,9]
  ldr r10, [sp,10]
  ldr fp,  [sp,11]
  ldr r12, [sp,12]
  ldr lr,  [sp,14]
  ldr r15, [sp,15]
  ldr r16, [sp,16]
  ldr r17, [sp,17]
  ldr r18, [sp,18]
  ldr r19, [sp,19]
  ldr r20, [sp,20]
  ldr r21, [sp,21]
  ldr r22, [sp,22]
  ldr r23, [sp,23]
  ldr r24, [sp,24]
  ldr r25, [sp,25]
  ldr r26, [sp,26]
  ldr r27, [sp,27]
  ldr r28, [sp,28]
  ldr r29, [sp,29]
  ldr r30, [sp,30]
  ldr r31, [sp,31]
  ldr r32, [sp,32]
  ldr r34, [sp,34]
  ldr r36, [sp,36]
  ldr r38, [sp,38]
  ldr r39, [sp,39]
  ldr r40, [sp,40]
  ldr r41, [sp,41]
  ldr r42, [sp,42]
  ldr r43, [sp,43]
  ldr r44, [sp,44]
  ldr r45, [sp,45]
  ldr r46, [sp,46]
  ldr r47, [sp,47]
  ldr r48, [sp,48]
  ldr r49, [sp,49]
  ldr r50, [sp,50]
  ldr r51, [sp,51]
  ldr r52, [sp,52]
  ldr r53, [sp,53]
  ldr r54, [sp,54]
  ldr r55, [sp,55]
  ldr r56, [sp,56]
  ldr r57, [sp,57]
  ldr r58, [sp,58]
  ldr r59, [sp,59]
  ldr r60, [sp,60]
  ldr r61, [sp,61]

  /* Restore status register */
  ldr r0,[sp,62]
  movts status, r0

  /* Restore config register */
  ldr r0, [sp,63]
  movts config, r0

  /* Restore interrupt return address register */
  ldr   r0, [sp,64]
  movts iret, r0

  ldr r0,[sp]

  /* Restore interrupted task's stack pointer */
  ldr sp, [sp,13]

  /* r62 and r63 are saved from start.S interrupt entry
   * and hold vector number and _ISR_Handler address repsectively.
   */
  ldr r62, [sp, -8]
  ldr r63, [sp, -4]

  /* return from interrupt */
  rti
