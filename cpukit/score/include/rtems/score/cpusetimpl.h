/**
 *  @file  rtems/score/cpusetimpl.h
 *
 *  @brief Implementation Prototypes for CPU Set
 *
 *  This file contains the implementation prototypes for
 *  CPU set methods.
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUSETIMPL_H
#define _RTEMS_SCORE_CPUSETIMPL_H

#include <rtems/score/cpuset.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __RTEMS_HAVE_SYS_CPUSET_H__

/**
 * _CPU_set_Is_valid
 *
 * This routine validates a cpuset size corresponds to
 * the system correct size, that at least one
 * valid cpu is set and that no invalid cpus are set.
 */
bool _CPU_set_Is_valid( const cpu_set_t *cpuset, size_t setsize );

/**
 * _CPU_set_Show
 *
 * This routine will print the value of the given cpuset.
 */
void _CPU_set_Show( const char *description, const cpu_set_t   *cpuset);

/**
 * _CPU_set_Show_default
 *
 * This routine will print the value of the default cpuset.
 */
void _CPU_set_Show_default( const char *description );

/**
 * _CPU_set_Default
 *
 * This routine returns the default cpuset for
 * this system.
 */
const CPU_set_Control *_CPU_set_Default(void);

#endif

/**
 * _CPU_set_Handler_initialization
 *
 * This routine validates a cpuset sets at least one
 * valid cpu and that it does not set any invalid
 * cpus.
 */

#if __RTEMS_HAVE_SYS_CPUSET_H__ && defined( RTEMS_SMP )
void _CPU_set_Handler_initialization(void);
#else
#define _CPU_set_Handler_initialization()  do { } while ( 0 )
#endif

/**@}*/

#ifdef __cplusplus
}
#endif
#endif
/* end of include file */
