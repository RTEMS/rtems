/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_scheduler_get_processor_set(
  rtems_id   scheduler_id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
)
{
  const Scheduler_Control    *scheduler;
  const Processor_mask       *processor_set;
  Processor_mask_Copy_status  status;

  if ( cpuset == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  scheduler = _Scheduler_Get_by_id( scheduler_id );
  if ( scheduler == NULL ) {
    return RTEMS_INVALID_ID;
  }

  processor_set = _Scheduler_Get_processors( scheduler );
  status = _Processor_mask_To_cpu_set_t( processor_set, cpusetsize, cpuset );
  if ( status != PROCESSOR_MASK_COPY_LOSSLESS ) {
    return RTEMS_INVALID_NUMBER;
  }

  return RTEMS_SUCCESSFUL;
}
