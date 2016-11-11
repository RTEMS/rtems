/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2013 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARM_MULTILIB_ARCH_V4

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

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
