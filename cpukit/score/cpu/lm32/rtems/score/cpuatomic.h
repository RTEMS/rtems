/**
 * @file  rtems/score/cpuatomic.h
 *
 * This include file implements the atomic operations for lm32 and defines
 * atomic data types which are used by the atomic operations API file. This
 * file should use fixed name cpuatomic.h and should be included in atomic
 * operations API file atomic.h. If the architecture works at the UP mode it
 * will use a generic atomic ops using disable/enable-IRQ simulated. If the
 * the architecture works at SMP mode, most of the parts of implementations
 * are imported from FreeBSD kernel.
 */

#ifndef _RTEMS_SCORE_ATOMIC_CPU_H
#define _RTEMS_SCORE_ATOMIC_CPU_H

#include <rtems/score/genericcpuatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic implementation
 *
 */

/**@{*/

#if !defined(RTEMS_SMP)
#include <rtems/score/genericatomicops.h>
#else
#endif

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
