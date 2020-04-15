/**
 *  @file
 *
 *  @brief RTEMS Task Wake After
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_task_wake_after(
  rtems_interval ticks
)
{
  /*
   * It is critical to obtain the executing thread after thread dispatching is
   * disabled on SMP configurations.
   */
  Thread_Control  *executing;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
    executing = _Per_CPU_Get_executing( cpu_self );

    if ( ticks == 0 ) {
      _Thread_Yield( executing );
    } else {
      _Thread_Set_state( executing, STATES_WAITING_FOR_TIME );
      _Thread_Wait_flags_set( executing, THREAD_WAIT_STATE_BLOCKED );
      _Thread_Add_timeout_ticks( executing, cpu_self, ticks );
    }
  _Thread_Dispatch_direct( cpu_self );
  return RTEMS_SUCCESSFUL;
}
