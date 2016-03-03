/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

static bool _Thread_Is_scheduled( const Thread_Control *the_thread )
{
#if defined(RTEMS_SMP)
  return the_thread->Scheduler.state == THREAD_SCHEDULER_SCHEDULED;
#else
  return _Thread_Is_executing( the_thread );
#endif
}

void _Thread_Get_CPU_time_used(
  Thread_Control    *the_thread,
  Timestamp_Control *cpu_time_used
)
{
  ISR_lock_Context lock_context;

  _Scheduler_Acquire( the_thread, &lock_context );

  if ( _Thread_Is_scheduled( the_thread ) ) {
    _Thread_Update_CPU_time_used( the_thread, _Thread_Get_CPU( the_thread ) );
  }

  *cpu_time_used = the_thread->cpu_time_used;

  _Scheduler_Release( the_thread, &lock_context );
}
