/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief V850 Set up Basic CPU Dependency Settings Based on Compiler Settings
 *
 * This file sets up basic CPU dependency settings based on
 * compiler settings.  For example, it can determine if
 * floating point is available.  This particular implementation
 * is specified to the Renesas v850 port.
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

#ifndef _RTEMS_SCORE_V850_H
#define _RTEMS_SCORE_V850_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NO CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */
#define CPU_MODEL_NAME  "rtems_multilib"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 0

#elif defined(__v850e2v3__)
#define CPU_MODEL_NAME  "v850e2v3"
#define V850_HAS_FPU 1
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850e2__)
#define CPU_MODEL_NAME  "v850e2"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850es__)
#define CPU_MODEL_NAME  "v850es"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850e1__)
#define CPU_MODEL_NAME  "v850e1"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850e__)
#define CPU_MODEL_NAME  "v850e"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#else
#define CPU_MODEL_NAME  "v850"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 0

#endif

/*
 *  Define the name of the CPU family.
 */
#define CPU_NAME "v850 CPU"

/*
 *  Method to set the Program Status Word (PSW)
 */
#define v850_set_psw( _psw ) \
    __asm__ __volatile__( "ldsr %0, psw" : : "r" (_psw) )

/*
 *  Method to obtain the Program Status Word (PSW)
 */
#define v850_get_psw( _psw ) \
    __asm__ __volatile__( "stsr psw, %0" : "=&r" (_psw) )

/*
 *  Masks and bits in the Program Status Word (PSW)
 */
#define V850_PSW_ZERO_MASK                       0x01
#define V850_PSW_IS_ZERO                         0x01
#define V850_PSW_IS_NOT                          0x00

#define V850_PSW_SIGN_MASK                       0x02
#define V850_PSW_SIGN_IS_NEGATIVE                0x02
#define V850_PSW_SIGN_IS_ZERO_OR_POSITIVE        0x00

#define V850_PSW_OVERFLOW_MASK                   0x02
#define V850_PSW_OVERFLOW_OCCURRED               0x02
#define V850_PSW_OVERFLOW_DID_NOT_OCCUR          0x00

#define V850_PSW_CARRY_OR_BORROW_MASK            0x04
#define V850_PSW_CARRY_OR_BORROW_OCCURRED        0x04
#define V850_PSW_CARRY_OR_BORROW_DID_NOT_OCCUR   0x00

#define V850_PSW_SATURATION_MASK                 0x10
#define V850_PSW_SATURATION_OCCURRED             0x10
#define V850_PSW_SATURATION_DID_NOT_OCCUR        0x00

#define V850_PSW_INTERRUPT_DISABLE_MASK          0x20
#define V850_PSW_INTERRUPT_DISABLE               0x20
#define V850_PSW_INTERRUPT_ENABLE                0x00

#define V850_PSW_EXCEPTION_IN_PROCESS_MASK       0x40
#define V850_PSW_EXCEPTION_IN_PROCESS            0x40
#define V850_PSW_EXCEPTION_NOT_IN_PROCESS        0x00

#define V850_PSW_NMI_IN_PROCESS_MASK             0x80
#define V850_PSW_NMI_IN_PROCESS                  0x80
#define V850_PSW_NMI_NOT_IN_PROCESS              0x00

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_V850_H */
