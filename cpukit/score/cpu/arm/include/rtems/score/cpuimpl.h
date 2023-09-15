/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This header file defines implementation interfaces pertaining to the
 *   port of the executive to the ARM architecture.
 */

/*
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
 * @defgroup RTEMSScoreCPUARM ARM
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief ARM Architecture Support
 *
 * @{
 */

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifdef ARM_MULTILIB_ARCH_V4

#if defined(ARM_MULTILIB_VFP_D32)
#define CPU_INTERRUPT_FRAME_SIZE 240
#elif defined(ARM_MULTILIB_VFP)
#define CPU_INTERRUPT_FRAME_SIZE 112
#else
#define CPU_INTERRUPT_FRAME_SIZE 40
#endif

#endif /* ARM_MULTILIB_ARCH_V4 */

#define CPU_THREAD_LOCAL_STORAGE_VARIANT 11

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARM_MULTILIB_ARCH_V4

typedef struct {
#ifdef ARM_MULTILIB_VFP
  uint32_t fpscr;
#ifdef ARM_MULTILIB_VFP_D32
  double d16;
  double d17;
  double d18;
  double d19;
  double d20;
  double d21;
  double d22;
  double d23;
  double d24;
  double d25;
  double d26;
  double d27;
  double d28;
  double d29;
  double d30;
  double d31;
#endif /* ARM_MULTILIB_VFP_D32 */
  double d0;
  double d1;
  double d2;
  double d3;
  double d4;
  double d5;
  double d6;
  double d7;
#endif /* ARM_MULTILIB_VFP */
#ifdef ARM_MULTILIB_HAS_STORE_RETURN_STATE
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r7;
  uint32_t r9;
  uint32_t r12;
  uint32_t lr;
  uint32_t return_pc;
  uint32_t return_cpsr;
#else /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */
  uint32_t r9;
  uint32_t lr;
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t return_pc;
  uint32_t return_cpsr;
  uint32_t r7;
  uint32_t r12;
#endif /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */
} CPU_Interrupt_frame;

#ifdef RTEMS_SMP

static inline struct Per_CPU_Control *_ARM_Get_current_per_CPU_control( void )
{
  struct Per_CPU_Control *cpu_self;

  /* Use PL1 only Thread ID Register (TPIDRPRW) */
  __asm__ volatile (
    "mrc p15, 0, %0, c13, c0, 4"
    : "=r" ( cpu_self )
  );

  return cpu_self;
}

#define _CPU_Get_current_per_CPU_control() _ARM_Get_current_per_CPU_control()

#endif /* RTEMS_SMP */

#endif /* ARM_MULTILIB_ARCH_V4 */

RTEMS_NO_RETURN void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error );

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

static inline void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( "udf" );
}

static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c0, 3" : : "r" ( context->thread_id ) : "memory"
  );
#else
  (void) context;
#endif
}

static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  return (void *) context->thread_id;
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
