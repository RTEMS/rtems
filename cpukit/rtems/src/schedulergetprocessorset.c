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
  const Scheduler_Control *scheduler;

  if ( cpuset == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !_Scheduler_Get_by_id( scheduler_id, &scheduler ) ) {
    return RTEMS_INVALID_ID;
  }

  if ( !_CPU_set_Is_large_enough( cpusetsize ) ) {
    return RTEMS_INVALID_NUMBER;
  }

  _Scheduler_Get_processor_set( scheduler, cpusetsize, cpuset );

  return RTEMS_SUCCESSFUL;
}

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
