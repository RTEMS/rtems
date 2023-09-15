/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
 *
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreCPUx86-64 x86-64
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief x86-64 Architecture Support
 *
 * @{
 */

#define CPU_PER_CPU_CONTROL_SIZE 0

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

#endif /* !ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
