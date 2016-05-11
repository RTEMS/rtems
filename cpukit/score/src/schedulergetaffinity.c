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

bool _Scheduler_Get_affinity(
  Thread_Control *the_thread,
  size_t          cpusetsize,
  cpu_set_t      *cpuset
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context         lock_context;
  bool                     ok;

  if ( !_CPU_set_Is_large_enough( cpusetsize ) ) {
    return false;
  }

  scheduler = _Scheduler_Get( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );

#if defined(RTEMS_SMP)
  ok = ( *scheduler->Operations.get_affinity )(
    scheduler,
    the_thread,
    cpusetsize,
    cpuset
  );
#else
  ok = _Scheduler_default_Get_affinity_body(
    scheduler,
    the_thread,
    cpusetsize,
    cpuset
  );
#endif

  _Scheduler_Release_critical( scheduler, &lock_context );
  return ok;
}

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
