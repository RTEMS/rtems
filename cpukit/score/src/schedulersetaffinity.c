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
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );

  if ( !_CPU_set_Is_large_enough( cpusetsize ) ) {
    return false;
  }

#if defined(RTEMS_SMP)
  return ( *scheduler->Operations.set_affinity )(
    scheduler,
    the_thread,
    cpusetsize,
    cpuset
  );
#else
  return _Scheduler_default_Set_affinity_body(
    scheduler,
    the_thread,
    cpusetsize,
    cpuset
  );
#endif
}

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
