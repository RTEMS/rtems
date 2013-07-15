/**
 * @file  rtems/score/cpuatomic.h
 *
 * This include file implements the atomic operations for sparc and defines
 * atomic data types which are used by the atomic operations API file. This
 * file should use fixed name cpuatomic.h and should be included in atomic
 * operations API file atomic.h. If the architecture works at the UP mode it
 * will not define atomic ops. If the architecture works at SMP mode, most of
 * the parts of implementations are based on stdatomic.h.
 */

#ifndef _RTEMS_SCORE_ATOMIC_CPU_H
#define _RTEMS_SCORE_ATOMIC_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMS atomic implementation
 *
 */

/**@{*/

#if !defined(RTEMS_SMP)
# error "Now atomic implementation only supports SMP mode."
#else
#include <rtems/score/cpustdatomic.h>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/*  end of include file */
