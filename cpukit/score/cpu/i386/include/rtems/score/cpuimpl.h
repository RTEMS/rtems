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
 * @defgroup RTEMSScoreCPUi386 i386
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief i386 Architecture Support
 *
 * @{
 */

#define CPU_PER_CPU_CONTROL_SIZE 0

#define CPU_INTERRUPT_FRAME_SIZE 52

#define CPU_THREAD_LOCAL_STORAGE_VARIANT 20

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
  __asm__ volatile ( ".word 0" );
}

static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
  uint32_t tmp;
  uint32_t cpu_index;

#ifdef RTEMS_SMP
  cpu_index = _CPU_SMP_Get_current_processor();
#else
  cpu_index = 0;
#endif

  __asm__ volatile (
    "movl " RTEMS_XSTRING( I386_CONTEXT_CONTROL_GS_0_OFFSET ) "(%2), %0\n"
    "movl %0, _Global_descriptor_table+24(,%1,8)\n"
    "movl " RTEMS_XSTRING( I386_CONTEXT_CONTROL_GS_1_OFFSET ) "(%2), %0\n"
    "movl %0, _Global_descriptor_table+28(,%1,8)\n"
    "leal 24(,%1,8), %0\n"
    "movl %0, %%gs\n"
    : "=&r" ( tmp )
    : "r" ( cpu_index ), "r" ( context )
    : "memory"
  );
}

static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  return (void *) &context->gs;
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
