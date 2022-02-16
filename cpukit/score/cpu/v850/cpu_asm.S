/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#if 0
/**
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  v850 Specific Information:
 *
 *  The v850 appears to always have soft float.
 */
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
)
{
}

/**
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  v850 Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
)
{
}
#endif

/**
 *  This routine performs a normal non-FP context switch.
 *
 *  v850 Specific Information:
 *
 *  + r6 - running thread
 *  + r7 - heir thread
 */
#define V850_CONTEXT_CONTROL_R1_OFFSET   0
#define V850_CONTEXT_CONTROL_R3_OFFSET   4
#define V850_CONTEXT_CONTROL_R20_OFFSET  8
#define V850_CONTEXT_CONTROL_R21_OFFSET 12
#define V850_CONTEXT_CONTROL_R22_OFFSET 16
#define V850_CONTEXT_CONTROL_R23_OFFSET 20
#define V850_CONTEXT_CONTROL_R24_OFFSET 24
#define V850_CONTEXT_CONTROL_R25_OFFSET 28
#define V850_CONTEXT_CONTROL_R26_OFFSET 32
#define V850_CONTEXT_CONTROL_R27_OFFSET 36
#define V850_CONTEXT_CONTROL_R28_OFFSET 40
#define V850_CONTEXT_CONTROL_R29_OFFSET 44
#define V850_CONTEXT_CONTROL_R31_OFFSET 48
#define V850_CONTEXT_CONTROL_PSW_OFFSET 52

        .section .text
        .global  __CPU_Context_switch
        .type    __CPU_Context_switch, @function
__CPU_Context_switch:
        st.w     r1,V850_CONTEXT_CONTROL_R1_OFFSET[r6]
        st.w     r3,V850_CONTEXT_CONTROL_R3_OFFSET[r6]
        st.w     r20,V850_CONTEXT_CONTROL_R20_OFFSET[r6]
        st.w     r21,V850_CONTEXT_CONTROL_R21_OFFSET[r6]
        st.w     r22,V850_CONTEXT_CONTROL_R22_OFFSET[r6]
        st.w     r23,V850_CONTEXT_CONTROL_R23_OFFSET[r6]
        st.w     r24,V850_CONTEXT_CONTROL_R24_OFFSET[r6]
        st.w     r25,V850_CONTEXT_CONTROL_R25_OFFSET[r6]
        st.w     r26,V850_CONTEXT_CONTROL_R27_OFFSET[r6]
        st.w     r27,V850_CONTEXT_CONTROL_R27_OFFSET[r6]
        st.w     r28,V850_CONTEXT_CONTROL_R28_OFFSET[r6]
        st.w     r29,V850_CONTEXT_CONTROL_R29_OFFSET[r6]
        st.w     r31,V850_CONTEXT_CONTROL_R31_OFFSET[r6]
	stsr     psw,r21
        st.w     r21,V850_CONTEXT_CONTROL_PSW_OFFSET[r6]
restore:
        ld.w     V850_CONTEXT_CONTROL_R1_OFFSET[r7],r1
        ld.w     V850_CONTEXT_CONTROL_R3_OFFSET[r7],r3
        ld.w     V850_CONTEXT_CONTROL_R20_OFFSET[r7],r20
        ld.w     V850_CONTEXT_CONTROL_R21_OFFSET[r7],r21
        ld.w     V850_CONTEXT_CONTROL_R22_OFFSET[r7],r22
        ld.w     V850_CONTEXT_CONTROL_R23_OFFSET[r7],r23
        ld.w     V850_CONTEXT_CONTROL_R24_OFFSET[r7],r24
        ld.w     V850_CONTEXT_CONTROL_R25_OFFSET[r7],r25
        ld.w     V850_CONTEXT_CONTROL_R27_OFFSET[r7],r26
        ld.w     V850_CONTEXT_CONTROL_R27_OFFSET[r7],r27
        ld.w     V850_CONTEXT_CONTROL_R28_OFFSET[r7],r28
        ld.w     V850_CONTEXT_CONTROL_R29_OFFSET[r7],r29
        ld.w     V850_CONTEXT_CONTROL_R31_OFFSET[r7],r31
        ld.w     V850_CONTEXT_CONTROL_PSW_OFFSET[r7],r7
	ldsr     r7,psw
        jmp      [r31]


/**
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 *  v850 Specific Information:
 *
 *  Move second parameter to first and jump to normal restore
 */
        .section .text
        .global  __CPU_Context_restore
        .type    __CPU_Context_restore, @function
__CPU_Context_restore:
        mov      r6, r7         /* move to second parameter register */
        br       restore
