/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief CPU Port Implementation API
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

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreCPUMIPS MIPS
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief MIPS Architecture Support
 *
 * @{
 */

#define CPU_PER_CPU_CONTROL_SIZE 0

#define CPU_THREAD_LOCAL_STORAGE_VARIANT 10

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

RTEMS_NO_RETURN void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error );

static inline void _CPU_Context_volatile_clobber( uintptr_t pattern )
{
  (void) pattern;

  /* TODO */
}

static inline void _CPU_Context_validate( uintptr_t pattern )
{
  (void) pattern;

  while (1) {
    /* TODO */
  }
}

static inline void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( ".word -1" );
}

static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
  (void) context;
}

static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  (void) context;
  return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
