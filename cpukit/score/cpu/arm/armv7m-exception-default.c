/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <rtems/score/armv7m.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void __attribute__((naked)) _ARMV7M_Exception_default( void )
{
  __asm__ volatile (
    "sub sp, %[cpufsz]\n"
    "stm sp, {r0-r12}\n"
    "mov r2, lr\n"
    "mrs r1, msp\n"
    "mrs r0, psp\n"
    "cmn r2, #3\n"
    "itt ne\n"
    "movne r0, r1\n"
    "addne r0, %[cpufsz]\n"
    "add r2, r0, %[v7mlroff]\n"
    "add r1, sp, %[cpulroff]\n"
    "ldm r2, {r3-r5}\n"
    "stm r1, {r3-r5}\n"
    "mrs r1, ipsr\n"
    "str r1, [sp, %[cpuvecoff]]\n"

    /* Argument for high level handler */
    "mov r0, sp\n"

    /* Clear VFP context pointer */
    "add r3, sp, %[cpuvfpoff]\n"
    "mov r1, #0\n"
    "str r1, [r3]\n"

#ifdef ARM_MULTILIB_VFP
    /* Ensure that the FPU is enabled */
    "ldr r4, =%[cpacr]\n"
    "tst r4, #(0xf << 20)\n"
    "bne 1f\n"

    /* Save VFP context */
    "sub sp, %[vfpsz]\n"
    "add r4, sp, #4\n"
    "bic r4, r4, #7\n"
    "str r4, [r3]\n"
    "vmrs r2, FPSCR\n"
    "stmia r4!, {r1-r2}\n"
    "vstmia r4!, {d0-d15}\n"
    "mov r1, #0\n"
    "mov r2, #0\n"
    "adds r3, r4, #128\n"
    "2:\n"
    "stmia r4!, {r1-r2}\n"
    "cmp r4, r3\n"
    "bne 2b\n"
    "1:\n"
#endif

    "b _ARM_Exception_default\n"
    :
    : [cpufsz] "i" (sizeof(CPU_Exception_frame)),
      [v7mfsz] "i" (sizeof(ARMV7M_Exception_frame)),
      [cpulroff] "i" (offsetof(CPU_Exception_frame, register_lr)),
      [v7mlroff] "i" (offsetof(ARMV7M_Exception_frame, register_lr)),
      [cpuvecoff] "J" (offsetof(CPU_Exception_frame, vector)),
      [cpuvfpoff] "i" (ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET),
      [cpacr] "i" (ARMV7M_CPACR),
      [vfpsz] "i" (ARM_VFP_CONTEXT_SIZE)
  );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
