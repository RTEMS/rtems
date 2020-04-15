/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_scheduler_ident_by_processor_set(
  size_t           cpusetsize,
  const cpu_set_t *cpuset,
  rtems_id        *id
)
{
  Processor_mask              set;
  Processor_mask_Copy_status  status;
  uint32_t                    cpu_index;
  const Scheduler_Control    *scheduler;

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  status = _Processor_mask_From_cpu_set_t( &set, cpusetsize, cpuset );
  if ( status == PROCESSOR_MASK_COPY_INVALID_SIZE ) {
    return RTEMS_INVALID_SIZE;
  }

  _Processor_mask_And( &set, &set, _SMP_Get_online_processors() );
  cpu_index = _Processor_mask_Find_last_set( &set );
  if ( cpu_index == 0 ) {
    return RTEMS_INVALID_NAME;
  }

  scheduler = _Scheduler_Get_by_CPU( _Per_CPU_Get_by_index( cpu_index - 1) );
#if defined(RTEMS_SMP)
  if ( scheduler == NULL ) {
    return RTEMS_INCORRECT_STATE;
  }
#else
  _Assert( scheduler != NULL );
#endif

  *id = _Scheduler_Build_id( _Scheduler_Get_index( scheduler ) );
  return RTEMS_SUCCESSFUL;
}
