/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2007 On-Line Applications Research Corporation (OAR)
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#if ( SPARC_HAS_FPU == 1 )
  #define CPU_PER_CPU_CONTROL_SIZE 8
#else
  #define CPU_PER_CPU_CONTROL_SIZE 4
#endif

/**
 * @brief Offset of the CPU_Per_CPU_control::isr_dispatch_disable field
 * relative to the Per_CPU_Control begin.
 */
#define SPARC_PER_CPU_ISR_DISPATCH_DISABLE 0

#if ( SPARC_HAS_FPU == 1 )
  /**
   * @brief Offset of the CPU_Per_CPU_control::fsr field relative to the
   * Per_CPU_Control begin.
   */
  #define SPARC_PER_CPU_FSR_OFFSET 4
#endif

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * This flag is context switched with each thread.  It indicates
   * that THIS thread has an _ISR_Dispatch stack frame on its stack.
   * By using this flag, we can avoid nesting more interrupt dispatching
   * attempts on a previously interrupted thread's stack.
   */
  uint32_t isr_dispatch_disable;

#if ( SPARC_HAS_FPU == 1 )
  /**
   * @brief Memory location to store the FSR register during interrupt
   * processing.
   *
   * This is a write-only field.  The FSR is written to force a completion of
   * floating point operations in progress.
   */
  uint32_t fsr;
#endif
} CPU_Per_CPU_control;

/**
 * @brief The pointer to the current per-CPU control is available via register
 * g6.
 */
register struct Per_CPU_Control *_SPARC_Per_CPU_current __asm__( "g6" );

#define _CPU_Get_current_per_CPU_control() _SPARC_Per_CPU_current

#define _CPU_Get_thread_executing() ( _SPARC_Per_CPU_current->executing )

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
