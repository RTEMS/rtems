/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.
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
 * @brief Optional method to get the executing thread.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.  In case this macro is undefined, the default
 * implementation uses the per-CPU information and the current processor index
 * to get the executing thread.
 */
#define _CPU_Get_thread_executing() ( _CPU_Per_CPU_current->executing )

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
