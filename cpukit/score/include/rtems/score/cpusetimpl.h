/**
 *  @file
 *
 *  @brief Implementation Helper for CPU Set
 *
 *  This file contains the implementation helpers inlines and prototypes for
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
#include <rtems/score/smp.h>

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __RTEMS_HAVE_SYS_CPUSET_H__

/**
 * @brief Determine If the CPU Set if Valid
 *
 * This routine validates a cpuset size corresponds to the system
 * correct size, that at least one valid cpu is set and that no invalid
 * cpus are set.
 *
 * @param[in] cpuset is the cpuset to validate
 * @param[in] setsize is the number of CPUs in the cpuset
 *
 * @return true if the set is valid
 */
bool _CPU_set_Is_valid( const cpu_set_t *cpuset, size_t setsize );

/**
 * @brief Print the CPU Set
 *
 * This routine will print the value of the given cpuset.
 *
 * @param[in] description is a string to print before the value.
 * @param[in] cpuset is the cpuset to validate
 */
void _CPU_set_Show( const char *description, const cpu_set_t  *cpuset);

/**
 * @brief Print the Default CPU Set
 *
 * This routine will print the value of the default cpuset.
 *
 * @param[in] description is a string to print before the value.
 */
void _CPU_set_Show_default( const char *description );

/**
 * @brief Obtain the Default CPU Set
 *
 * This routine returns the default cpuset for this system.
 *
 * @return a pointer to the default cpuset.
 */
const CPU_set_Control *_CPU_set_Default(void);

/**
 * @brief Obtain the Maximum Number of CPUs Representable in CPU Set
 *
 * This routine returns maximum number of CPUs that can be represented
 * in the @a setsize specified.
 *
 * @return the number of representable cores in the cpuset
 */
RTEMS_INLINE_ROUTINE size_t _CPU_set_Maximum_CPU_count( size_t setsize )
{
  return setsize * CHAR_BIT;
}

/**
 * @brief Is this CPU Set Large Enough?
 *
 * This method can be used to determine if a particular cpuset is
 * large enough for the number of CPUs in the system.
 *
 * @param[in] setsize is the number of CPUs in the cpuset
 *
 * @return true if the @a setsize is sufficient
 */
RTEMS_INLINE_ROUTINE bool _CPU_set_Is_large_enough(
  size_t setsize
)
{
  return _CPU_set_Maximum_CPU_count( setsize ) >= _SMP_Get_processor_count();
}

/**
 * @brief Fill in CPU_set_Control
 *
 * This method fills in a CPU_set_Control based upon a cpu_set_t and
 * size information.
 *
 * @param[in] cpuset is the cpuset to validate
 * @param[in] setsize is the number of CPUs in the cpuset
 */
static inline void _CPU_set_Set(
  size_t           setsize,
  cpu_set_t       *cpuset,
  CPU_set_Control *set
)
{
  set->set     = &set->preallocated;
  set->setsize = setsize;
  CPU_COPY( set->set, cpuset );
}
#endif

/**
 * @brief Initialize the CPU Set Handler
 *
 * This routine validates a cpuset sets at least one valid cpu and that
 * it does not set any invalid cpus.
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
