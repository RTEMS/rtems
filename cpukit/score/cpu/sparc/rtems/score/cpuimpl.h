/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2015, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

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
