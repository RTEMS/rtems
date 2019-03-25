/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

void _CPU_Context_volatile_clobber( uintptr_t pattern );

void _CPU_Context_validate( uintptr_t pattern );

RTEMS_INLINE_ROUTINE void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( "udf" );
}

RTEMS_INLINE_ROUTINE void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
