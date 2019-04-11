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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_scheduler_ident_by_processor(
  uint32_t  cpu_index,
  rtems_id *id
)
{
  const Scheduler_Control *scheduler;

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( cpu_index >= _SMP_Get_processor_maximum() ) {
    return RTEMS_INVALID_NAME;
  }

  scheduler = _Scheduler_Get_by_CPU( _Per_CPU_Get_by_index( cpu_index ) );
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
