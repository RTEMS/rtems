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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

bool _Scheduler_Set_affinity(
  Thread_Control       *the_thread,
  size_t                cpusetsize,
  const cpu_set_t      *cpuset
)
{
  Processor_mask             affinity;
  Processor_mask_Copy_status status;
  const Scheduler_Control   *scheduler;
  Scheduler_Node            *node;
  ISR_lock_Context           lock_context;
  bool                       ok;

  status = _Processor_mask_From_cpu_set_t( &affinity, cpusetsize, cpuset );
  if ( !_Processor_mask_Is_at_most_partial_loss( status ) ) {
    return false;
  }

  /*
   * Reduce affinity set to the online processors to be in line with
   * _Thread_Initialize() which sets the default affinity to the set of online
   * processors.
   */
  _Processor_mask_And( &affinity, _SMP_Get_online_processors(), &affinity );

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );

  node = _Thread_Scheduler_get_home_node( the_thread );
#if defined(RTEMS_SMP)
  ok = ( *scheduler->Operations.set_affinity )(
    scheduler,
    the_thread,
    node,
    &affinity
  );

  if ( ok ) {
    _Processor_mask_Assign( &the_thread->Scheduler.Affinity, &affinity );
  }
#else
  ok = _Scheduler_default_Set_affinity_body(
    scheduler,
    the_thread,
    node,
    &affinity
  );
#endif

  _Scheduler_Release_critical( scheduler, &lock_context );
  return ok;
}
