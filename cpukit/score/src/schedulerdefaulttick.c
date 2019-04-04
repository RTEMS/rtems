/**
 * @file
 *
 * @brief Default Scheduler At Tick Handler
 *
 * @ingroup RTEMSScoreScheduler
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/smp.h>
#include <rtems/config.h>

void _Scheduler_default_Tick(
  const Scheduler_Control *scheduler,
  Thread_Control          *executing
)
{
  (void) scheduler;

  /*
   *  If the thread is not preemptible or is not ready, then
   *  just return.
   */

  if ( !executing->is_preemptible )
    return;

  if ( !_States_Is_ready( executing->current_state ) )
    return;

  /*
   *  The cpu budget algorithm determines what happens next.
   */

  switch ( executing->budget_algorithm ) {
    case THREAD_CPU_BUDGET_ALGORITHM_NONE:
      break;

    case THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE:
    #if defined(RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE)
      case THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE:
    #endif
      if ( (int)(--executing->cpu_time_budget) <= 0 ) {

        /*
         *  A yield performs the ready chain mechanics needed when
         *  resetting a timeslice.  If no other thread's are ready
         *  at the priority of the currently executing thread, then the
         *  executing thread's timeslice is reset.  Otherwise, the
         *  currently executing thread is placed at the rear of the
         *  FIFO for this priority and a new heir is selected.
         */
        _Thread_Yield( executing );
        executing->cpu_time_budget =
          rtems_configuration_get_ticks_per_timeslice();
      }
      break;

    #if defined(RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT)
      case THREAD_CPU_BUDGET_ALGORITHM_CALLOUT:
	if ( --executing->cpu_time_budget == 0 )
	  (*executing->budget_callout)( executing );
	break;
    #endif
  }
}
