/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This source file contains the implementation of
 *   _CPU_Exception_resume().
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
#include <rtems/score/percpu.h>
#include <libcpu/grlib-tn-0018.h>

/*
 *  void _CPU_Exception_resume( const CPU_Exception_frame *frame )
 *
 *  This routine restores the register set of the frame and continues the
 *  execution at the program counter of the frame.
 *
 *  _SPARC_Bad_trap() entered the window below the window of the trapped
 *  context.  It saved the windows in that order, so a restore per entry of
 *  the frame walks back to the window it entered.
 *
 *  Only the global registers survive a change of the current window pointer.
 *  So this routine keeps the frame in %g1 and restores %g1 last.  It touches
 *  no stack, because the stack pointer of the window it walks through belongs
 *  to the trapped context.
 */

        .align 4
        PUBLIC(_CPU_Exception_resume)
SYM(_CPU_Exception_resume):
        mov     %o0, %g1                      ! g1 = frame

        rd      %psr, %g2
        andn    %g2, SPARC_PSR_ET_MASK, %g2
        mov     %g2, %psr                     ! **** DISABLE TRAPS ****
        nop
        nop
        nop

        /*
         * Make every window available, so that a restore of this routine
         * raises no window underflow trap.
         */
        mov     %g0, %wim
        nop
        nop
        nop

        /*
         * The trap table stores the PSR after the trap decreased the CWP, so
         * the PSR of the frame carries the CWP of the window which the trap
         * entered.  The trap cleared PSR[ET] there.  RETT restores PSR[ET] and
         * increases the CWP again.
         */
        ld      [%g1 + SPARC_EXCEPTION_OFFSET_PSR], %g5
        andn    %g5, SPARC_PSR_ET_MASK, %g5

#if SPARC_HAS_FPU == 1
        /*
         * Enable the floating point unit to restore the floating point
         * registers.  The final write to the PSR installs the state of the
         * trapped context.
         */
        sethi   %hi(SPARC_PSR_EF_MASK), %g4
        or      %g5, %g4, %g4
        mov     %g4, %psr                     ! **** ENTER THE TRAP WINDOW ****
        nop
        nop
        nop

        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(0)], %f0
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(1)], %f2
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(2)], %f4
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(3)], %f6
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(4)], %f8
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(5)], %f10
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(6)], %f12
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(7)], %f14
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(8)], %f16
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(9)], %f18
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(10)], %f20
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(11)], %f22
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(12)], %f24
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(13)], %f26
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(14)], %f28
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_FP(15)], %f30
        ld      [%g1 + SPARC_EXCEPTION_OFFSET_FSR], %fsr
#else
        mov     %g5, %psr                     ! **** ENTER THE TRAP WINDOW ****
        nop
        nop
        nop
#endif

        /*
         * Restore the register windows.  One restore per entry of the frame
         * walks back to the window which the trap entered.
         */
        set     SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g2
        add     %g1, SPARC_EXCEPTION_OFFSET_WINDOWS(0), %g3

.Lrestore_register_windows:

        restore
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(0)], %l0
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(2)], %l2
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(4)], %l4
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(6)], %l6
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(0)], %i0
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(2)], %i2
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(4)], %i4
        ldd     [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(6)], %i6
        add     %g3, SPARC_REGISTER_WINDOW_SIZE, %g3
        subcc   %g2, 1, %g2
        bne     .Lrestore_register_windows
         nop

        /*
         * Go back to the window which the trap entered.
         */
        restore

        /*
         * The input registers of this window are the output registers of the
         * trapped context.
         */
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_OUTPUT(0)], %i0
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_OUTPUT(2)], %i2
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_OUTPUT(4)], %i4
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_OUTPUT(6)], %i6

        ld      [%g1 + SPARC_EXCEPTION_OFFSET_PC], %l1
        ld      [%g1 + SPARC_EXCEPTION_OFFSET_NPC], %l2
        ld      [%g1 + SPARC_EXCEPTION_OFFSET_Y], %l3
        wr      %l3, 0, %y
        mov     %g5, %l0                      ! l0 = PSR of the trap window

        ld      [%g1 + SPARC_EXCEPTION_OFFSET_WIM], %l4
        mov     %l4, %wim
        nop
        nop
        nop

        /*
         * Restore the global registers.  %g1 holds the frame, so it goes last.
         */
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_GLOBAL(2)], %g2
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_GLOBAL(4)], %g4
        ldd     [%g1 + SPARC_EXCEPTION_OFFSET_GLOBAL(6)], %g6

        TN0018_WAIT_IFLUSH %l3,%l4            ! GRLIB-TN-0018 work around macro

        mov     %l0, %psr                     ! restore the condition codes
        nop
        nop
        nop

        ld      [%g1 + SPARC_EXCEPTION_OFFSET_GLOBAL(1)], %g1
        TN0018_FIX %l3,%l4                    ! GRLIB-TN-0018 work around macro
        jmp     %l1
        rett    %l2

/* end of file */
