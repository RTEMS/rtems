/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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
 * @defgroup RTEMSScoreCPUAArch64 AArch64
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief ARM AArch64 Architecture Support
 *
 * @{
 */

#define CPU_PER_CPU_CONTROL_SIZE 0

#define CPU_INTERRUPT_FRAME_SIZE 0x2E0

#define CPU_THREAD_LOCAL_STORAGE_VARIANT 11

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

RTEMS_NO_RETURN void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error );

typedef struct {
  uint64_t x0;
  uint64_t register_lr_original;
  uint64_t register_lr;
  uint64_t x1;
  uint64_t x2;
  uint64_t x3;
  uint64_t x4;
  uint64_t x5;
  uint64_t x6;
  uint64_t x7;
  uint64_t x8;
  uint64_t x9;
  uint64_t x10;
  uint64_t x11;
  uint64_t x12;
  uint64_t x13;
  uint64_t x14;
  uint64_t x15;
  uint64_t x16;
  uint64_t x17;
  uint64_t x18;
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
#ifdef AARCH64_MULTILIB_VFP
  uint128_t q0;
  uint128_t q1;
  uint128_t q2;
  uint128_t q3;
  uint128_t q4;
  uint128_t q5;
  uint128_t q6;
  uint128_t q7;
  uint128_t q8;
  uint128_t q9;
  uint128_t q10;
  uint128_t q11;
  uint128_t q12;
  uint128_t q13;
  uint128_t q14;
  uint128_t q15;
  uint128_t q16;
  uint128_t q17;
  uint128_t q18;
  uint128_t q19;
  uint128_t q20;
  uint128_t q21;
  uint128_t q22;
  uint128_t q23;
  uint128_t q24;
  uint128_t q25;
  uint128_t q26;
  uint128_t q27;
  uint128_t q28;
  uint128_t q29;
  uint128_t q30;
  uint128_t q31;
#endif /* AARCH64_MULTILIB_VFP */
  uint64_t register_elr;
  uint64_t register_spsr;
  uint64_t register_fpsr;
  uint64_t register_fpcr;
} CPU_Interrupt_frame;

#ifdef RTEMS_SMP

static inline
struct Per_CPU_Control *_AARCH64_Get_current_per_CPU_control( void )
{
  struct Per_CPU_Control *cpu_self;
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TPIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  /* Use EL1 Thread ID Register (TPIDR_EL1) */
  cpu_self = (struct Per_CPU_Control *)(uintptr_t)value;

  return cpu_self;
}

#define _CPU_Get_current_per_CPU_control() \
  _AARCH64_Get_current_per_CPU_control()

#endif /* RTEMS_SMP */

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

static inline void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( ".inst 0x0" );
}

static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
  __asm__ volatile (
    "msr TPIDR_EL0, %0" : : "r" ( context->thread_id ) : "memory"
  );
}

static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  return (void *)(uintptr_t) context->thread_id;
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
