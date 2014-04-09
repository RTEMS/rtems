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

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)

#include <rtems/rtems/tasks.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_scheduler_get_processor_set(
  rtems_id   scheduler_id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
)
{
  rtems_status_code sc;

  if ( cpuset != NULL ) {
    const Scheduler_Control *scheduler;

    if ( _Scheduler_Get_by_id( scheduler_id, &scheduler ) ) {
      if ( _CPU_set_Is_large_enough( cpusetsize ) ) {
        _Scheduler_Get_processor_set( scheduler, cpusetsize, cpuset );

        sc = RTEMS_SUCCESSFUL;
      } else {
        sc = RTEMS_INVALID_NUMBER;
      }
    } else {
      sc = RTEMS_INVALID_ID;
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  return sc;
}

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
