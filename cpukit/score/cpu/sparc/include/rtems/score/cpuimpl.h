/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This header file defines implementation interfaces pertaining to the
 *   port of the executive to the SPARC processor.
 */

/*
 * Copyright (c) 1989, 2007 On-Line Applications Research Corporation (OAR)
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreCPUSPARC SPARC
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief SPARC Architecture Support
 *
 * @{
 */

/** This defines the size of the minimum stack frame. */
#define SPARC_MINIMUM_STACK_FRAME_SIZE 0x60

/*
 *  Offsets of fields with CPU_Interrupt_frame for assembly routines.
 */

/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_PSR_OFFSET         SPARC_MINIMUM_STACK_FRAME_SIZE + 0x00
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_PC_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x04
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_NPC_OFFSET         SPARC_MINIMUM_STACK_FRAME_SIZE + 0x08
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G1_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x0c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G2_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x10
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G3_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x14
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G4_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x18
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G5_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x1c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G7_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x24
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I0_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x28
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I1_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x2c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I2_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x30
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I3_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x34
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I4_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x38
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I5_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x3c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I6_FP_OFFSET       SPARC_MINIMUM_STACK_FRAME_SIZE + 0x40
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I7_OFFSET          SPARC_MINIMUM_STACK_FRAME_SIZE + 0x44
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_Y_OFFSET           SPARC_MINIMUM_STACK_FRAME_SIZE + 0x48
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_TPC_OFFSET         SPARC_MINIMUM_STACK_FRAME_SIZE + 0x4c

/** This defines the size of the ISF area for use in assembly. */
#define CPU_INTERRUPT_FRAME_SIZE SPARC_MINIMUM_STACK_FRAME_SIZE + 0x50

#define SPARC_FP_CONTEXT_OFFSET_F0_F1 0
#define SPARC_FP_CONTEXT_OFFSET_F2_F3 8
#define SPARC_FP_CONTEXT_OFFSET_F4_F5 16
#define SPARC_FP_CONTEXT_OFFSET_F6_F7 24
#define SPARC_FP_CONTEXT_OFFSET_F8_F9 32
#define SPARC_FP_CONTEXT_OFFSET_F10_F11 40
#define SPARC_FP_CONTEXT_OFFSET_F12_F13 48
#define SPARC_FP_CONTEXT_OFFSET_F14_F15 56
#define SPARC_FP_CONTEXT_OFFSET_F16_F17 64
#define SPARC_FP_CONTEXT_OFFSET_F18_F19 72
#define SPARC_FP_CONTEXT_OFFSET_F20_F21 80
#define SPARC_FP_CONTEXT_OFFSET_F22_F23 88
#define SPARC_FP_CONTEXT_OFFSET_F24_F25 96
#define SPARC_FP_CONTEXT_OFFSET_F26_F27 104
#define SPARC_FP_CONTEXT_OFFSET_F28_F29 112
#define SPARC_FP_CONTEXT_OFFSET_F30_F31 120
#define SPARC_FP_CONTEXT_OFFSET_FSR 128

#if ( SPARC_HAS_FPU == 1 )
  #define CPU_PER_CPU_CONTROL_SIZE 8
#else
  #define CPU_PER_CPU_CONTROL_SIZE 0
#endif

#define CPU_THREAD_LOCAL_STORAGE_VARIANT 20

#if ( SPARC_HAS_FPU == 1 )
  /**
   * @brief Offset of the CPU_Per_CPU_control::fsr field relative to the
   * Per_CPU_Control begin.
   */
  #define SPARC_PER_CPU_FSR_OFFSET 0

  #if defined(SPARC_USE_LAZY_FP_SWITCH)
    /**
     * @brief Offset of the CPU_Per_CPU_control::fp_owner field relative to the
     * Per_CPU_Control begin.
     */
    #define SPARC_PER_CPU_FP_OWNER_OFFSET 4
  #endif
#endif

#define SPARC_REGISTER_WINDOW_OFFSET_LOCAL( i ) ( ( i ) * 4 )
#define SPARC_REGISTER_WINDOW_OFFSET_INPUT( i ) ( ( i ) * 4 + 32 )
#define SPARC_REGISTER_WINDOW_SIZE 64

#define SPARC_EXCEPTION_OFFSET_PSR 0
#define SPARC_EXCEPTION_OFFSET_PC 4
#define SPARC_EXCEPTION_OFFSET_NPC 8
#define SPARC_EXCEPTION_OFFSET_TRAP 12
#define SPARC_EXCEPTION_OFFSET_WIM 16
#define SPARC_EXCEPTION_OFFSET_Y 20
#define SPARC_EXCEPTION_OFFSET_GLOBAL( i ) ( ( i ) * 4 + 24 )
#define SPARC_EXCEPTION_OFFSET_OUTPUT( i ) ( ( i ) * 4 + 56 )
#define SPARC_EXCEPTION_OFFSET_WINDOWS( i ) ( ( i ) * 64 + 88 )

#if SPARC_HAS_FPU == 1
#define SPARC_EXCEPTION_OFFSET_FSR 536
#define SPARC_EXCEPTION_OFFSET_FP( i ) ( ( i ) * 8 + 544 )
#define SPARC_EXCEPTION_FRAME_SIZE 672
#else
#define SPARC_EXCEPTION_FRAME_SIZE 536
#endif

#if defined(SPARC_USE_SYNCHRONOUS_FP_SWITCH)
#define SPARC_FP_FRAME_OFFSET_FO_F1 (SPARC_MINIMUM_STACK_FRAME_SIZE + 0)
#define SPARC_FP_FRAME_OFFSET_F2_F3 (SPARC_FP_FRAME_OFFSET_FO_F1 + 8)
#define SPARC_FP_FRAME_OFFSET_F4_F5 (SPARC_FP_FRAME_OFFSET_F2_F3 + 8)
#define SPARC_FP_FRAME_OFFSET_F6_F7 (SPARC_FP_FRAME_OFFSET_F4_F5 + 8)
#define SPARC_FP_FRAME_OFFSET_F8_F9 (SPARC_FP_FRAME_OFFSET_F6_F7 + 8)
#define SPARC_FP_FRAME_OFFSET_F1O_F11 (SPARC_FP_FRAME_OFFSET_F8_F9 + 8)
#define SPARC_FP_FRAME_OFFSET_F12_F13 (SPARC_FP_FRAME_OFFSET_F1O_F11 + 8)
#define SPARC_FP_FRAME_OFFSET_F14_F15 (SPARC_FP_FRAME_OFFSET_F12_F13 + 8)
#define SPARC_FP_FRAME_OFFSET_F16_F17 (SPARC_FP_FRAME_OFFSET_F14_F15 + 8)
#define SPARC_FP_FRAME_OFFSET_F18_F19 (SPARC_FP_FRAME_OFFSET_F16_F17 + 8)
#define SPARC_FP_FRAME_OFFSET_F2O_F21 (SPARC_FP_FRAME_OFFSET_F18_F19 + 8)
#define SPARC_FP_FRAME_OFFSET_F22_F23 (SPARC_FP_FRAME_OFFSET_F2O_F21 + 8)
#define SPARC_FP_FRAME_OFFSET_F24_F25 (SPARC_FP_FRAME_OFFSET_F22_F23 + 8)
#define SPARC_FP_FRAME_OFFSET_F26_F27 (SPARC_FP_FRAME_OFFSET_F24_F25 + 8)
#define SPARC_FP_FRAME_OFFSET_F28_F29 (SPARC_FP_FRAME_OFFSET_F26_F27 + 8)
#define SPARC_FP_FRAME_OFFSET_F3O_F31 (SPARC_FP_FRAME_OFFSET_F28_F29 + 8)
#define SPARC_FP_FRAME_OFFSET_FSR (SPARC_FP_FRAME_OFFSET_F3O_F31 + 8)
#define SPARC_FP_FRAME_SIZE (SPARC_FP_FRAME_OFFSET_FSR + 8)
#endif

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#if ( SPARC_HAS_FPU == 1 )
  /**
   * @brief Memory location to store the FSR register during interrupt
   * processing.
   *
   * This is a write-only field.  The FSR is written to force a completion of
   * floating point operations in progress.
   */
  uint32_t fsr;

#if defined(SPARC_USE_LAZY_FP_SWITCH)
  /**
   * @brief The current floating point owner.
   */
  struct _Thread_Control *fp_owner;
#else
  /* See Per_CPU_Control::Interrupt_frame */
  uint32_t reserved_for_alignment_of_interrupt_frame;
#endif
#endif
} CPU_Per_CPU_control;

/**
 * @brief The pointer to the current per-CPU control is available via register
 * g6.
 *
 * @note Older C and C++ standards have made the register keyword a hint
 * at block scope level. C17 defines register as a reserved keyword with
 * no meaning. Using __extension__ allows the GNU behavior for global
 * file scope. See RTEMS Issue 5250
 * (https://gitlab.rtems.org/rtems/rtos/rtems/-/issues/5250) for a detailed
 * discussion.
 */
__extension__ register struct
   Per_CPU_Control *_SPARC_Per_CPU_current __asm__( "g6" );

#define _CPU_Get_current_per_CPU_control() _SPARC_Per_CPU_current

#define _CPU_Get_thread_executing() ( _SPARC_Per_CPU_current->executing )

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

static inline void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( "unimp 0" );
}

static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
   register uint32_t g7 __asm__( "g7" );

   g7 = context->g7;

   /* Make sure that the register assignment is not optimized away */
   __asm__ volatile ( "" : : "r" ( g7 ) );
}

static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  return (void *) context->g7;
}

#if defined(RTEMS_PROFILING)
/**
 * @brief Reads the CPU counter while interrupts are disabled.
 */
CPU_Counter_ticks _SPARC_Counter_read_ISR_disabled( void );
#endif

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
