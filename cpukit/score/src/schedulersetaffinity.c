/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)

bool _Scheduler_Set_affinity(
  Thread_Control          *the_thread,
  size_t                   cpusetsize,
  const cpu_set_t         *cpuset
)
{
  bool ok;

  if ( _CPU_set_Is_large_enough( cpusetsize ) ) {
#if defined(RTEMS_SMP)
    uint32_t cpu_count = _SMP_Get_processor_count();
    uint32_t cpu_index;

    ok = false;

    for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
      if ( CPU_ISSET_S( (int) cpu_index, cpusetsize, cpuset ) ) {
        const Scheduler_Control *scheduler_of_cpu =
          _Scheduler_Get_by_CPU_index( cpu_index );

        if ( scheduler_of_cpu != NULL ) {
          ok = ( *scheduler_of_cpu->Operations.set_affinity )(
            scheduler_of_cpu,
            the_thread,
            cpusetsize,
            cpuset
          );
        }

        break;
      }
    }
#else
    ok = _Scheduler_default_Set_affinity_body(
      _Scheduler_Get( the_thread ),
      the_thread,
      cpusetsize,
      cpuset
    );
#endif
  } else {
    ok = false;
  }

  return ok;
}

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
