/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This source file contains the implementation of _SPARC_Bad_trap().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

        /*
         * The trap handler entry was set up by TRAP().
         */
        PUBLIC(_SPARC_Bad_trap)
SYM(_SPARC_Bad_trap):

        /*
         * Do not use the existing stack since it may be invalid.  Use the ISR
         * stack for this processor.  If the trap was caused from within
         * interrupt context, then a return to the context which caused the
         * trap would be unreliable.
         */
        set     SYM(_ISR_Stack_size), %l5

#if defined(RTEMS_SMP) && defined(__leon__)
        rd      %asr17, %l6
        srl     %l6, LEON3_ASR17_PROCESSOR_INDEX_SHIFT, %l6
        add     %l6, 1, %l4
        smul    %l4, %l5, %l5
#endif
        set     SYM(_ISR_Stack_area_begin), %l7
        add     %l7, %l5, %l7
        andn    %l7, CPU_STACK_ALIGNMENT - 1, %l7

        /*
         * Establish an area on the stack for a CPU_Exception_frame.
         */
        sub     %l7, SPARC_EXCEPTION_FRAME_SIZE, %l7

        /*
         * Start saving the context which caused the trap.
         */
        mov     %wim, %l4
        rd      %y, %l5
        std     %l0, [%l7 + SPARC_EXCEPTION_OFFSET_PSR]
        SPARC_LEON3FT_B2BST_NOP
        std     %l2, [%l7 + SPARC_EXCEPTION_OFFSET_NPC]
        SPARC_LEON3FT_B2BST_NOP
        st      %l4, [%l7 + SPARC_EXCEPTION_OFFSET_WIM]
        st      %l5, [%l7 + SPARC_EXCEPTION_OFFSET_Y]
        std     %g0, [%l7 + SPARC_EXCEPTION_OFFSET_GLOBAL(0)]
        SPARC_LEON3FT_B2BST_NOP
        std     %g2, [%l7 + SPARC_EXCEPTION_OFFSET_GLOBAL(2)]
        SPARC_LEON3FT_B2BST_NOP
        std     %g4, [%l7 + SPARC_EXCEPTION_OFFSET_GLOBAL(4)]
        SPARC_LEON3FT_B2BST_NOP
        std     %g6, [%l7 + SPARC_EXCEPTION_OFFSET_GLOBAL(6)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i0, [%l7 + SPARC_EXCEPTION_OFFSET_OUTPUT(0)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i2, [%l7 + SPARC_EXCEPTION_OFFSET_OUTPUT(2)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i4, [%l7 + SPARC_EXCEPTION_OFFSET_OUTPUT(4)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i6, [%l7 + SPARC_EXCEPTION_OFFSET_OUTPUT(6)]

        /*
         * Initialize %g6 since it may be corrupt.
         */
        set     SYM(_Per_CPU_Information), %g6
#if defined(RTEMS_SMP) && defined(__leon__)
        sll     %l6, PER_CPU_CONTROL_SIZE_LOG2, %l4
        add     %g6, %l4, %g6
#endif

        /*
         * Disable WIM traps.
         */
        mov     %g0, %wim
        nop
        nop
        nop

        /*
         * Save the remaining register windows.
         */
        set     SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g2
        add     %l7, SPARC_EXCEPTION_OFFSET_WINDOWS(0), %g3

.Lsave_register_windows:

        restore
        std     %l0, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(0)]
        SPARC_LEON3FT_B2BST_NOP
        std     %l2, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(2)]
        SPARC_LEON3FT_B2BST_NOP
        std     %l4, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(4)]
        SPARC_LEON3FT_B2BST_NOP
        std     %l6, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_LOCAL(6)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i0, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(0)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i2, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(2)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i4, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(4)]
        SPARC_LEON3FT_B2BST_NOP
        std     %i6, [%g3 + SPARC_REGISTER_WINDOW_OFFSET_INPUT(6)]
        add     %g3, SPARC_REGISTER_WINDOW_SIZE, %g3
        subcc   %g2, 1, %g2
        bne     .Lsave_register_windows
         nop

        /*
         * Go back to register window at trap entry.
         */
        restore

        /*
         * Initialize the WIM based on the PSR[CWP] to have all register
         * windows available for the fatal error procedure.
         */
        and     %l0, SPARC_PSR_CWP_MASK, %l4
        set     1, %l5
        sll     %l5, %l4, %l5
        mov     %l5, %wim

#if SPARC_HAS_FPU == 1
        /*
         * Enable the FPU in the new PSR (PSR[EF] == 1).
         */
        sethi   %hi(SPARC_PSR_EF_MASK), %l4
        or      %l0, %l4, %l0
#endif

        /*
         * Enable traps and disable interrupts.
         */
        or      %l0, 0xf20, %l0
        wr      %l0, %psr
        nop
        nop
        nop

#if SPARC_HAS_FPU == 1
        st      %fsr, [%l7 + SPARC_EXCEPTION_OFFSET_FSR]
        std     %f0, [%l7 + SPARC_EXCEPTION_OFFSET_FP(0)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f2, [%l7 + SPARC_EXCEPTION_OFFSET_FP(1)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f4, [%l7 + SPARC_EXCEPTION_OFFSET_FP(2)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f6, [%l7 + SPARC_EXCEPTION_OFFSET_FP(3)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f8, [%l7 + SPARC_EXCEPTION_OFFSET_FP(4)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f10, [%l7 + SPARC_EXCEPTION_OFFSET_FP(5)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f12, [%l7 + SPARC_EXCEPTION_OFFSET_FP(6)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f14, [%l7 + SPARC_EXCEPTION_OFFSET_FP(7)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f16, [%l7 + SPARC_EXCEPTION_OFFSET_FP(8)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f18, [%l7 + SPARC_EXCEPTION_OFFSET_FP(9)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f20, [%l7 + SPARC_EXCEPTION_OFFSET_FP(10)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f22, [%l7 + SPARC_EXCEPTION_OFFSET_FP(11)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f24, [%l7 + SPARC_EXCEPTION_OFFSET_FP(12)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f26, [%l7 + SPARC_EXCEPTION_OFFSET_FP(13)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f28, [%l7 + SPARC_EXCEPTION_OFFSET_FP(14)]
        SPARC_LEON3FT_B2BST_NOP
        std     %f30, [%l7 + SPARC_EXCEPTION_OFFSET_FP(15)]
#endif

        /*
         * Call _Terminate( RTEMS_FATAL_SOURCE_EXCEPTION, %l0 ).
         */
        sub     %l7, SPARC_MINIMUM_STACK_FRAME_SIZE, %sp
        set     9, %o0
        call    SYM(_Terminate)
         mov    %l7, %o1
