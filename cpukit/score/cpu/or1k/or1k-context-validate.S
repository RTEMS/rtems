/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

.section	.text

PUBLIC(_CPU_Context_validate)
SYM(_CPU_Context_validate):

  l.addi r1, r1, -140

  l.sw  8(r1), r2
  l.sw  12(r1), r3
  l.sw  16(r1), r4
  l.sw  20(r1), r5
  l.sw  24(r1), r6
  l.sw  28(r1), r7
  l.sw  32(r1), r8
  l.sw  36(r1), r9
  l.sw  40(r1), r10
  l.sw  44(r1), r11
  l.sw  48(r1), r12
  l.sw  52(r1), r13
  l.sw  56(r1), r14
  l.sw  60(r1), r15
  l.sw  64(r1), r16
  l.sw  68(r1), r17
  l.sw  72(r1), r18
  l.sw  76(r1), r19
  l.sw  80(r1), r20
  l.sw  84(r1), r21
  l.sw  88(r1), r22
  l.sw  92(r1), r23
  l.sw  96(r1), r24
  l.sw  100(r1), r25
  l.sw  104(r1), r26
  l.sw  108(r1), r27
  l.sw  112(r1), r28
  l.sw  116(r1), r29
  l.sw  120(r1), r30
  l.sw  124(r1), r31

  /* Fill */

  /* r13 is used for temporary values */
  l.add r13, r0, r0

  /* r31 contains the stack pointer */
  l.add r31, r1, r0

.macro fill_register reg
  l.addi	r13, r13, 1
  l.addi	\reg, r13, 0
.endm

  fill_register	r2
  fill_register	r3
  fill_register	r4
  fill_register	r5
  fill_register	r6
  fill_register	r7
  fill_register	r8
  fill_register	r9
  fill_register	r10
  fill_register	r11
  fill_register	r12
  fill_register	r13
  fill_register	r14
  fill_register	r15
  fill_register	r16
  fill_register	r17
  fill_register	r18
  fill_register	r19
  fill_register	r20
  fill_register	r21
  fill_register	r22
  fill_register	r23
  fill_register	r24
  fill_register	r25
  fill_register	r26
  fill_register	r27
  fill_register	r28
  fill_register	r29
  fill_register	r30
  fill_register	r31

	/* Check */
check:

.macro check_register reg
	l.addi r13, r13, 1
	l.sfne \reg, r13
	l.bf   restore
	l.nop
.endm

	l.sfne r31, r1
	l.bf restore
	l.nop

	l.addi r13, r0, 0

	check_register	r2
	check_register	r3
	check_register	r4
	check_register	r5
	check_register	r6
	check_register	r7
	check_register	r8
	check_register	r9
	check_register	r10
  check_register	r11
  check_register	r12
  check_register	r13
  check_register	r14
  check_register	r15
  check_register	r16
  check_register	r17
  check_register	r18
  check_register	r19
  check_register	r20
	check_register	r21
	check_register	r22
	check_register	r23
	check_register	r24
	check_register	r25
	check_register	r26
	check_register	r27
	check_register	r28
	check_register	r29
	check_register	r30
	check_register	r31

	l.j check
	l.nop

/* Restore */
restore:

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

  l.addi r1, r1, 140

  l.jr r9
  l.nop
