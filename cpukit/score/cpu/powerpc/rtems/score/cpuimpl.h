/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#ifdef RTEMS_SMP

/* Use SPRG0 for the per-CPU control of the current processor */
#define PPC_PER_CPU_CONTROL_REGISTER 272

#endif /* RTEMS_SMP */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RTEMS_SMP

static inline struct Per_CPU_Control *_PPC_Get_current_per_CPU_control( void )
{
  struct Per_CPU_Control *cpu_self;

  __asm__ volatile (
    "mfspr %0, " RTEMS_XSTRING( PPC_PER_CPU_CONTROL_REGISTER )
    : "=r" ( cpu_self )
  );

  return cpu_self;
}

#define _CPU_Get_current_per_CPU_control() _PPC_Get_current_per_CPU_control()

#endif /* RTEMS_SMP */

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
