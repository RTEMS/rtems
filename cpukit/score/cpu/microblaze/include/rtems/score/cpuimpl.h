/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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
 * @defgroup RTEMSScoreCPUMicroBlaze MicroBlaze
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief MicroBlaze Architecture Support
 *
 * @{
 */

#define CPU_PER_CPU_CONTROL_SIZE 0

#define CPU_INTERRUPT_FRAME_SIZE 56

#define CPU_THREAD_LOCAL_STORAGE_VARIANT 10

#define MICROBLAZE_INTERRUPT_FRAME_R3   0
#define MICROBLAZE_INTERRUPT_FRAME_R4   4
#define MICROBLAZE_INTERRUPT_FRAME_R5   8
#define MICROBLAZE_INTERRUPT_FRAME_R6  12
#define MICROBLAZE_INTERRUPT_FRAME_R7  16
#define MICROBLAZE_INTERRUPT_FRAME_R8  20
#define MICROBLAZE_INTERRUPT_FRAME_R9  24
#define MICROBLAZE_INTERRUPT_FRAME_R10 28
#define MICROBLAZE_INTERRUPT_FRAME_R11 32
#define MICROBLAZE_INTERRUPT_FRAME_R12 36
#define MICROBLAZE_INTERRUPT_FRAME_R14 40
#define MICROBLAZE_INTERRUPT_FRAME_R15 44
#define MICROBLAZE_INTERRUPT_FRAME_R18 48
#define MICROBLAZE_INTERRUPT_FRAME_MSR 52

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

static inline void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( ".word 0x0" );
}

static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
  /*
   * There is nothing to do since the thread-local storage area is obtained by
   * calling __tls_get_addr().
   */
  (void) context;
}

static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  return (void *) context->thread_pointer;
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
