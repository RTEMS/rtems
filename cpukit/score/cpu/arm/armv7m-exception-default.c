/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This source file contains the implementation of
 *   _ARMV7M_Exception_default().
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/armv7m.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void __attribute__((naked)) _ARMV7M_Exception_default( void )
{
    /* On exception entry, ARMv7M saves context state onto a stack pointed to
     * by either MSP or PSP. The value stored in LR indicates whether we were
     * in Thread or Handler mode, whether we were using the FPU (if any),
     * and which stack pointer we were using.
     * In particular, bit 2 of LR will be 0 if we were using MSP.
     *
     * For a more detailed explanation, see the Exception Entry Behavior
     * section of the ARMv7M Architecture Reference Manual.
     */

    /* As we're in Handler mode here, we'll always operate on MSP.
     * However, we need to store the right SP in our CPU_Exception_frame.
     */
  __asm__ volatile (
    "sub sp, %[cpufsz]\n"   /* Allocate space for a CPU_Exception_frame. */
    "stm sp, {r0-r12}\n"
    "tst lr, #4\n"          /* Check if bit 2 of LR is zero. If so, PSR.Z = 1 */
    "itte eq\n"             /* IF bit 2 of LR is zero... (PSR.Z == 1) */
    "mrseq r3, msp\n"       /* THEN we were using MSP */
    "addeq r3, %[cpufsz]\n" /* THEN, set r3 = old MSP value */
    "mrsne r3, psp\n"       /* ELSE it is not zero; we were using PSP */
    "add r2, r3, %[v7mlroff]\n"
    "add r1, sp, %[cpuspoff]\n"
    "ldm r2, {r4-r6}\n"     /* Grab LR, PC and xPSR from the stack */
    "tst lr, #16\n"         /* Check if we have an FP state on the frame */
    "ite eq\n"
    "addeq r3, #104\n"      /* Back to previous SP with FP state */
    "addne r3, #32\n"       /* Back to previous SP without FP state */
    "tst r6, #512\n"        /* Check xPSR if the SP was aligned */
    "it ne\n"
    "addne r3, #4\n"        /* Undo alignment */
    "stm r1, {r3-r6}\n"     /* Store to CPU_Exception_frame */
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
      [cpuspoff] "i" (offsetof(CPU_Exception_frame, register_sp)),
      [v7mlroff] "i" (offsetof(ARMV7M_Exception_frame, register_lr)),
      [cpuvecoff] "J" (offsetof(CPU_Exception_frame, vector)),
      [cpuvfpoff] "i" (ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET),
      [cpacr] "i" (ARMV7M_CPACR),
      [vfpsz] "i" (ARM_VFP_CONTEXT_SIZE)
  );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
