/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013, 2014 embedded brains GmbH
 *
 * Copyright (C) 2019 DornerWorks
 *
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
 *        and Josh Whitehead <josh.whitehead@dornerworks.com>
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

#include <bsp.h>
#include <bsp/start.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
    /*
     * On reset, V will be set.  This points the exceptions to the FSBL's vectors.  The FSBL
     * should clear this bit before booting RTEMS but in some debugging
     * configurations the bit may not be.  The other bits should already be clear
     * on reset.  Since the correct settings in these bits are critical,
     * make sure SCTLR[M, I, A, C, V] are cleared.  Afterwards, exceptions are
     * handled by RTEMS.
     * After setting the SCTLR, invalidate the caches.
     * Note 1: The APU also does these steps in start.S in _start in the #if block:
     *         `#if (__ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A') || __ARM_ARCH >= 8`
     * Note 2: Not all Arm R cores need this (like the TMS570).  So, this probably should
     *         be in this hook and not in start.S
     *
     * Ref: https://developer.arm.com/documentation/ddi0460/c/System-Control/Register-descriptions/c1--System-Control-Register?lang=en
     */

    __asm__ volatile(
      "mrc p15, 0, r0, c1, c0, 0 \n"
      "bic r1, r0, #0x3000 \n"     /* Clear V[13] and I[12] */
      "bic r1, r1, #0x7 \n"        /* Clear C[2] A[1] and M[0] */
      "mcr p15, 0, r1, c1, c0, 0 \n"

      /* Invalidate caches */
      "mov      r0,#0 \n"
      "dsb \n"
      "mcr      p15, 0, r0, c7, c5, 0 \n"
      "mcr      p15, 0, r0, c15, c5, 0 \n"
      "isb \n"
        : :);
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  zynqmp_setup_mpu_and_cache();
  bsp_start_clear_bss();
}
