/**
 * @file
 *
 * @ingroup ScoreCpuset
 *
 * @brief Routines to Control a CPU Set.
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __RTEMS_HAVE_SYS_CPUSET_H__
#include <sys/cpuset.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/percpu.h>

static CPU_set_Control cpuset_default;

/*
 * _CPU_set_Handler_initialization
 */
void _CPU_set_Handler_initialization()
{
  int i;
  int max_cpus;

  /* We do not support a cpu count over CPU_SETSIZE  */
  max_cpus = _SMP_Get_processor_count();

  /* This should never happen */
  _Assert( max_cpus <= CPU_SETSIZE );

  /*  Initialize the affinity to be the set of all available CPU's   */
  cpuset_default.set     = &cpuset_default.preallocated;
  cpuset_default.setsize = sizeof( *cpuset_default.set );
  CPU_ZERO_S( cpuset_default.setsize, &cpuset_default.preallocated );

  for (i=0; i<max_cpus; i++)
    CPU_SET_S(i, cpuset_default.setsize, cpuset_default.set );
}

/**
 * _CPU_set_Is_valid
 *
 * This routine validates a cpuset size corresponds to
 * the system correct size, that at least one
 * valid cpu is set and that no invalid cpus are set.
 */
int _CPU_set_Is_valid( const cpu_set_t *cpuset, size_t setsize )
{
  cpu_set_t             temp;

  if ( !cpuset )
    return -1;

  if (setsize != cpuset_default.setsize )
    return -1;

  /* Validate at least 1 valid cpu is set in cpuset */
  CPU_AND_S( cpuset_default.setsize, &temp, cpuset, cpuset_default.set );

  if ( CPU_COUNT_S( setsize, &temp ) == 0 )
    return -1;

  /* Validate that no invalid cpu's are set in cpuset */
  if ( !CPU_EQUAL_S( setsize, &temp, cpuset ) )
    return -1;

  return 0;
}

/**
 * _CPU_set_Default
 *
 * This routine returns the default cpuset.
 */
const CPU_set_Control *_CPU_set_Default()
{
  return &cpuset_default;
}
#endif


