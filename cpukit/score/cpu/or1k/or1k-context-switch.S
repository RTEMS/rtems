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
#include "rtems/score/or1k-utility.h"

.text
.align 4

PUBLIC(_CPU_Context_switch)
PUBLIC(_CPU_Context_restore)
PUBLIC(_CPU_Context_restore_fp)
PUBLIC(_CPU_Context_save_fp)

SYM(_CPU_Context_switch):
  l.sw  0(r3),r1
  l.sw  4(r3),r2
  l.sw  8(r3),r3
  l.sw  12(r3),r4
  l.sw  16(r3),r5
  l.sw  20(r3),r6
  l.sw  24(r3),r7
  l.sw  28(r3),r8
  l.sw  32(r3),r9
  /* Skip r10 as it's preserved to be used by TLS */
  /* The following set if registers are preserved across function calls */
  l.sw  52(r3),r14
  l.sw  60(r3),r16
  l.sw  68(r3),r18
  l.sw  76(r3),r20
  l.sw  84(r3),r22
  l.sw  92(r3),r24
  l.sw  100(r3),r26
  l.sw  108(r3),r28
  l.sw  116(r3),r30

  /* Supervision Register */
  l.mfspr r13,r0, CPU_OR1K_SPR_SR
  l.sw  124(r3),r13

  /* EPCR */
  l.mfspr r13, r0, CPU_OR1K_SPR_EPCR0
  l.sw  128(r3), r13 /* epcr */

  /* EEAR */
  l.mfspr r13, r0, CPU_OR1K_SPR_EEAR0
  l.sw  132(r3), r13 /* eear */

  /* ESR */
  l.mfspr r13, r0, CPU_OR1K_SPR_ESR0
  l.sw  136(r3), r13  /* esr */

SYM(restore):
  l.lwz   r13,124(r4)
  l.mtspr r0,r13, CPU_OR1K_SPR_SR

  /* Exception level related registers */

  /* EPCR */
  l.lwz  r13,  128(r4)
  l.mtspr r0, r13, CPU_OR1K_SPR_EPCR0

  /* EEAR */
  l.lwz  r13,  132(r4)
  l.mtspr r0, r13, CPU_OR1K_SPR_EEAR0

  /* ESR */
  l.lwz  r13,  136(r4)
  l.mtspr r0, r13, CPU_OR1K_SPR_ESR0

  l.lwz  r1,0(r4)
  l.lwz  r2,4(r4)
  l.lwz  r3,8(r4)
  /* Skip r4 as it contains the current buffer address */
  l.lwz  r5,16(r4)
  l.lwz  r6,20(r4)
  l.lwz  r7,24(r4)
  l.lwz  r8,28(r4)
  l.lwz  r9,32(r4)
  l.lwz  r14,52(r4)
  l.lwz  r16,60(r4)
  l.lwz  r18,68(r4)
  l.lwz  r20,76(r4)
  l.lwz  r22,84(r4)
  l.lwz  r24,92(r4)
  l.lwz  r26,100(r4)
  l.lwz  r28,108(r4)
  l.lwz  r30,116(r4)

  l.lwz  r4,12(r4)

  l.jr   r9
  l.nop

 SYM(_CPU_Context_restore):
  l.add   r4,r3,r0
  l.add   r13,r0,r0
  l.j     restore
  l.nop

 SYM(_CPU_Context_restore_fp):
  l.nop

 SYM(_CPU_Context_save_fp):
  l.nop
