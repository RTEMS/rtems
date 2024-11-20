/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCShared
 *
 * @brief This assembler source file contains the LEON3-specific
 *   bsp_idle_thread() implementation.
 */

/*
 *  Idle Thread Body
 *
 *  This routine puts LEON3 in power-down mode.
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
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



#include <rtems/asm.h>

/*
 * LEON specific power-down function
 *
 * This implementation contains the workaround for the GR712RC
 * power-down errata described in GR712RC-UM, version 2.16, section
 * 1.7.8. The workaround is compatible with other LEON3 components.
 */

        .align 4
        PUBLIC(bsp_idle_thread)
        PUBLIC(leon3_power_down_loop)
SYM(bsp_idle_thread):
SYM(leon3_power_down_loop):
pwdloop:
          /*
           * Address 0xfffffff0 is the read-only AHB plug & play region at
           * least for the UT699, GR712RC, and GR740.
           */
          set     0xfffffff0, %o0

          /*
           * Make sure the following two instructions are in one cache line.
           * This ensures that the instructions are fetched from the
           * instruction cache when the processor leaves power-down mode
           * (unless a SEU occurs in the instruction cache line).
           *
           * Use a load which bypasses the MMU and cache (ASI 0x1c).
           */
          .align 32
          mov     %g0, %asr19
          lda     [%o0] 0x1c, %g0

          /*
           * Place the branch far enough away from the preceding load, to make
           * sure the preceding load is executed before the branch to prevent
           * an instruction fetch from memory.
           */
          .align 32
          ba,a    pwdloop
           nop
