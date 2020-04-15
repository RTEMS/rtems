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

#include <rtems/score/schedulerimpl.h>

bool _Scheduler_Get_affinity(
  Thread_Control *the_thread,
  size_t          cpusetsize,
  cpu_set_t      *cpuset
)
{
  const Scheduler_Control    *scheduler;
  ISR_lock_Context            lock_context;
  const Processor_mask       *affinity;
  Processor_mask_Copy_status  status;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );

#if defined(RTEMS_SMP)
  affinity = &the_thread->Scheduler.Affinity;
#else
  affinity = &_Processor_mask_The_one_and_only;
#endif
  status = _Processor_mask_To_cpu_set_t( affinity, cpusetsize, cpuset );

  _Scheduler_Release_critical( scheduler, &lock_context );
  return status == PROCESSOR_MASK_COPY_LOSSLESS;
}
